#include "attributes.h"
#include "bailout.h"
#include "error.h"
#include "open.h"
#include <sys/types.h>
#include "nowarn.h"
#include "close.h"
#include "str.h"
#include "alloc.h"
#include "byte.h"
#include "buffer.h"
#include "readwrite.h"
#include "fmt.h"
#include "mime.h"
#include "mail_in.h"
#include "env.h"
#include "seek.h"
#include "getln.h"
#include "case.h"
#include "fd.h"
#include "cdb.h"
#include "uogetopt.h"
#include "wait.h"
#include "pathexec.h"
#include "upgpverify.h"

#define TYPE_GPG 0
#define TYPE_PGP2 1
#define TYPE_PGP5 2

static stralloc boundary;
static char bspace[1024];
static buffer b;
static char messpace[1024];
static buffer mesbuf;
static char sigspace[1024];
static buffer sigbuf;
static const char *opt_rulesfile, *tmpdir;
static int opt_type;
static int opt_news_verify;
static int opt_rules_allow_default;
static unsigned long opt_max_len=1024*1024;
static unsigned long opt_passphrase_fd;
static char *x_pgp_sig;

#define CT_UNKNOWN         0
#define CT_MPART_ENCRYPTED 1
#define CT_MPART_SIGNED    2
#define CT_ENCRYPTED       3
static int content_type;

static struct check_result cres;
static struct check_param cpara;
static void die_qp(void) attribute_noreturn;
static void die_b64(void) attribute_noreturn;
static void die_mail_read(void) attribute_noreturn;
static void die_mail_eof(void) attribute_noreturn;
static void die_x_pgp_sig(void) attribute_noreturn;
static void die_write_mes(void) attribute_noreturn;
static void die_write_sig(void) attribute_noreturn;
static void die_write_tmp(void) attribute_noreturn;
static void die_cdb_open(void) attribute_noreturn;
static void die_cdb_failed(void) attribute_noreturn;

static void die_qp(void) { xbailout(100,0,"unable to decode qp part",0,0,0); }
static void die_b64(void) { xbailout(100,0,"unable to decode base64 part",0,0,0); }
static void die_mail_read(void) 
{ xbailout(111,errno,"failed to read mail",0,0,0); }
static void die_mail_eof(void) 
{ xbailout(111,0,"premature end-of-file reading mail",0,0,0); }
static void die_x_pgp_sig(void)
{ xbailout(100,0,"cannot parse x-pgp-sig header",0,0,0); }
static void die_write_mes(void)
{ xbailout(111,errno, "failed to write/copy to message buffer",0,0,0); }
static void die_write_sig(void)
{ xbailout(111,errno, "failed to write/copy to signature buffer",0,0,0); }
static void die_write_tmp(void)
{ xbailout(111,errno,"failed to write to temporary file",0,0,0); }

#define MODE_NIX 0
#define MODE_QP  1
#define MODE_B64 2
static int mode_mes;
static int mode_sig;



static int
handle_encoding2(const char *s, unsigned int l)
{
	stralloc sa=STRALLOC_INIT;
	stralloc tmp=STRALLOC_INIT;
	unsigned int off;
	int ret=MODE_NIX;
	if (!stralloc_copyb(&sa,s,l)) oom();
	if (!mime_content_type_init(&sa,&tmp, &off, &sa))
		/* may be empty line */
		return MODE_NIX;
	if (!stralloc_0(&sa)) oom();
	if (case_equals(sa.s,"quoted-printable")) 
		ret=MODE_QP;
	if (case_equals(sa.s,"base64")) 
		ret=MODE_B64;
	stralloc_free(&tmp);
	stralloc_free(&sa);
	return ret;
}

static int
handle_encoding(void)
{
	const char *p;
	p=mail_header("content-transfer-encoding");
	if (!p)
		return MODE_NIX;
	return handle_encoding2(p,str_len(p));
}

static
int write2sig(int fd, const char *s, unsigned int len)
{
	(void) fd;
	if (opt_max_len && 
	  len+cpara.message.len+cpara.signature.len>opt_max_len)
		xbailout(100,0,"overlong signature detected",0,0,0);
	if (!stralloc_catb(&cpara.signature,s,len)) oom();
	return len;
}
static
int write2mes(int fd, const char *s, unsigned int len)
{
	(void) fd;
	if (opt_max_len && 
	  len+cpara.message.len+cpara.signature.len>opt_max_len)
		xbailout(100,0,"overlong message detected",0,0,0);
	if (!stralloc_catb(&cpara.message,s,len)) oom();
	return len;
}

static void
parse_content_type(const char *ct0)
{
	stralloc ct=STRALLOC_INIT;
	stralloc tmp=STRALLOC_INIT;
	stralloc var=STRALLOC_INIT;
	stralloc val=STRALLOC_INIT;
	unsigned int off;
	if (!stralloc_copys(&ct,ct0)) oom();
	if (!mime_content_type_init(&ct,&tmp, &off, &ct))
		/* may be empty line */
		return;
/* Content-Type: multipart/signed; micalg=pgp-md5;
     protocol="application/pgp-signature"; boundary="BXVAT5kNtrzKuDFl" 
*/
	if (!stralloc_0(&ct)) oom();
	if (str_equal(ct.s,"multipart/encrypted")) 
		content_type=CT_MPART_ENCRYPTED;
	else if (str_equal(ct.s,"multipart/signed")) 
		content_type=CT_MPART_SIGNED;
	else
		return;  /* will give error later */
	while (mime_content_type_parse(&tmp, &off, &var, &val)) {
		if (!stralloc_0(&var)) oom();
		if (str_equal(var.s,"boundary")) {
			if (!stralloc_copys(&boundary,"--")) oom();
			if (!stralloc_cat(&boundary,&val)) oom();
		}
		if (str_equal(var.s,"protocol")) {
			if (!stralloc_0(&val)) oom();
			if (content_type==CT_MPART_ENCRYPTED) {
				if (str_diff(val.s,"application/pgp-encrypted")) {
					xbailout(100,0,"Unable to deal with multipart/encrypted ",
						"with protocol ",val.s,0);
				}
			} else if (content_type==CT_MPART_SIGNED) {
				if (str_diff(val.s,"application/pgp-signature")) {
					xbailout(100,0,"Unable to deal with multipart/signed ",
						"with protocol ",val.s,0);
				}
			}
		}
		if (str_equal(var.s,"micalg")) {
			if (!stralloc_0(&val)) oom();
			if (!str_start(val.s,"pgp-")) {
				warning(0,"cannot understand micalg ",val.s,0,0);
				return;
			}
			if (!stralloc_copys(&cpara.micalg,val.s+4)) oom();
		}
	}
}

static void
pgp_process(const char *what)
{
	unsigned int i;
	int pid;
	int r;
	int wstat;
	check_func fn;
	/* move micalg to uppercase */
	for (i=0;i<cpara.micalg.len;i++) {
		unsigned char c=cpara.micalg.s[i];
		if (c>='a' && c<='z')
			cpara.micalg.s[i]=c-'a'+'A';
	}

	if (opt_type==TYPE_PGP5) 
		fn=check_pgp5_sig;
	else if (opt_type==TYPE_PGP2) 
		fn=check_pgp2_sig;
	else
		fn=check_gpg_sig;
	
	pid=fn(&cpara,&cres);
	r=wait_pid(&wstat,pid);
	if (r==-1) 
		xbailout(100,0,"failed to wait for ",what," process",0);
	if (wait_crashed(wstat))
		xbailout(100,0,what, " process crashed",0,0);
	if (wait_exitcode(wstat)) {
		char nb[FMT_ULONG];
		nb[fmt_ulong(nb,wait_exitcode(wstat))]=0;
		xbailout(100,0,what, " process failed with code ",nb,0);
	}
}
static void 
check_sig(void)
{
	if (content_type==CT_MPART_ENCRYPTED || content_type==CT_ENCRYPTED) {
		cpara.need_decryption=1;
		pgp_process("decrypt/verify");
	} else {
		cpara.need_decryption=0;
		pgp_process("verify");
	}

	if (cres.long_keyid.s) {
		if (!stralloc_0(&cres.long_keyid)) oom();
		if (!pathexec_env("AUTH_LONG_KEYID",cres.long_keyid.s)) oom();
	}
	if (cres.fingerprint.s) {
		if (!stralloc_0(&cres.fingerprint)) oom();
		if (!pathexec_env("AUTH_FINGERPRINT",cres.fingerprint.s)) oom();
	}
	if (!stralloc_0(&cres.keyid)) oom();
	if (!pathexec_env("AUTH_KEYID",cres.keyid.s)) oom();
	if (!stralloc_0(&cres.user)) oom();
	if (!pathexec_env("AUTH_USER",cres.user.s)) oom();

	if (content_type==CT_MPART_ENCRYPTED) {
		static r822_headhash_t head;
		int x;
		const char *value;
		uo_uint32_t l;
		char cte[]="content-transfer-encoding";

		if (!stralloc_copy(&cpara.message,&cres.output)) oom();
		stralloc_free(&cres.output);
		if (cpara.message.len) {
			/* hash-header stops at end of header */
			if (r822_hash_header(&head,cpara.message.s,cpara.message.len)
					!= R822_OK)
				oom();
			x=r822_hash_lookup(&head,cte,sizeof(cte)-1,&value,&l);
			if (x) {
				switch (handle_encoding2(value,l)) {
				case MODE_NIX: mode_mes=MODE_NIX; break;
				case MODE_B64: mode_mes=MODE_B64; break;
				case MODE_QP:  mode_mes=MODE_QP; break;
				}
			}
			r822_hash_destroy(&head);
		}
	}
}

static void
handle_multipart_encrypted(buffer *in, buffer *mes, buffer *sig)
{
	stralloc sa=STRALLOC_INIT;
	int gotlf;
	int flag_inheader=0;
	static stralloc part_header;

	/* 0: vor --boundary nummer 1
	 * 1: nach --boundary/1, das heisst "Version: 1" als Body.
	 * 2: nach --boundary/2, das heisst Nachricht + evnt. signature.
	 * 3: nach --boundary--, das heisst fertig
	 */
	int phase=0; 
	part_header.len=0;
	while (1) {
		sa.len=0;
		if (-1==getln(in,&sa,&gotlf,'\n'))
			die_mail_read();
		if (!sa.len) break;
		if (!gotlf)
			die_mail_eof();
		if (sa.len>boundary.len) {
			if (byte_equal(boundary.s,boundary.len,sa.s))  
				if (sa.s[boundary.len]=='\n' 
				  ||sa.s[boundary.len]=='\r') {
				  	phase++;
					if (phase==3)
						xbailout(100,0,"cannot parse MIME structure: "
							"beyond end of multipart/signed",0,0,0);
					if (phase)
						flag_inheader=1;
					else
						flag_inheader=0;
					continue;
				}
			if (sa.len>boundary.len+2)  /* --\n */
				if (byte_equal(boundary.s,boundary.len,sa.s))  
					if (sa.s[boundary.len]=='-' 
					  &&sa.s[boundary.len]=='-') {
						phase++;
						if (phase!=3)
							xbailout(100,0,"cannot parse MIME structure: "
								"strange end of multipart/signed",0,0,0);
						break;
				}
		}
		if (phase==0) continue;
		if (flag_inheader) {
			unsigned int olen=sa.len;
			if (!stralloc_cat(&part_header,&sa)) oom();
			sa.len--;
			if (sa.len && sa.s[sa.len-1]=='\r')
				sa.len--;
			if (sa.len==0) {
				static r822_headhash_t head;
				int x;
				const char *value;
				uo_uint32_t l;
				char cte[]="content-transfer-encoding";
				flag_inheader=0;
				if (r822_hash_header(&head,part_header.s,part_header.len) 
					!= R822_OK)
						oom();
				part_header.len=0;
				x=r822_hash_lookup(&head,cte,sizeof(cte)-1,&value,&l);
				if (x) {
					int *mp;
					if (phase==1)
						mp=&mode_mes;
					else
						mp=&mode_sig;
					switch (handle_encoding2(value,l)) {
					case MODE_NIX: *mp=MODE_NIX; break;
					case MODE_B64: *mp=MODE_B64; break;
					case MODE_QP:  *mp=MODE_QP; break;
					}
				}
				r822_hash_destroy(&head);
			}
			if (phase==2)
				continue;
			sa.len=olen;
		}
		if (phase==1) {
			if (-1==buffer_put(mes,sa.s,sa.len)) 	
				die_write_mes();
		} else {
			if (-1==buffer_put(sig,sa.s,sa.len)) 	
				die_write_sig();
		}
		continue;
	}
	if (phase!=3)
		xbailout(100,0,"cannot find enough MIME parts for multipart/signed",
			0,0,0);
	if (-1==buffer_flush(mes))
		die_write_mes();
	if (-1==buffer_flush(sig))
		die_write_sig();
	stralloc_free(&sa);
	{
		unsigned int i;
		if (!stralloc_0(&cpara.message)) oom();
		i=0;
		while (1) {
			if (str_start(cpara.message.s+i,"Version: 1\n")) 
				break;
			i+=str_chr(cpara.message.s+i,'\n');
			if (!cpara.message.s[i])
				xbailout(100,0,"Unable to find Version: 1 in pgp encrypted"
					" message",0,0,0);
			i++;
		}
		cpara.message.len--; /* remove \0 */
	}
}
static void
handle_multipart_signed(buffer *in, buffer *mes, buffer *sig)
{
	stralloc sa=STRALLOC_INIT;
	int gotlf;
	int flag_inheader=0;
	static stralloc part_header;

	/* 0: vor --boundary nummer 1
	 * 1: nach --boundary/1, das heisst nachricht
	 * 2: nach --boundary/2, das heisst signature
	 * 3: nach --boundary--, das heisst fertig
	 */
	int phase=0; 
	part_header.len=0;
	while (1) {
		sa.len=0;
		if (-1==getln(in,&sa,&gotlf,'\n'))
			die_mail_read();
		if (!sa.len) break;
		if (!gotlf)
			die_mail_eof();
		if (sa.len>boundary.len) {
			if (byte_equal(boundary.s,boundary.len,sa.s))  
				if (sa.s[boundary.len]=='\n' 
				  ||sa.s[boundary.len]=='\r') {
				  	phase++;
					if (phase==3)
						xbailout(100,0,"cannot parse MIME structure: "
							"beyond end of multipart/signed",0,0,0);
					if (phase)
						flag_inheader=1;
					else
						flag_inheader=0;
					continue;
				}
			if (sa.len>boundary.len+2)  /* --\n */
				if (byte_equal(boundary.s,boundary.len,sa.s))  
					if (sa.s[boundary.len]=='-' 
					  &&sa.s[boundary.len]=='-') {
						phase++;
						if (phase!=3)
							xbailout(100,0,"cannot parse MIME structure: "
								"strange end of multipart/signed",0,0,0);
						break;
				}
		}
		if (phase==0) continue;
		if (flag_inheader) {
			unsigned int olen=sa.len;
			if (!stralloc_cat(&part_header,&sa)) oom();
			sa.len--;
			if (sa.len && sa.s[sa.len-1]=='\r')
				sa.len--;
			if (sa.len==0) {
				static r822_headhash_t head;
				int x;
				const char *value;
				uo_uint32_t l;
				char cte[]="content-transfer-encoding";
				flag_inheader=0;
				if (r822_hash_header(&head,part_header.s,part_header.len) 
					!= R822_OK)
						oom();
				part_header.len=0;
				x=r822_hash_lookup(&head,cte,sizeof(cte)-1,&value,&l);
				if (x) {
					int *mp;
					if (phase==1)
						mp=&mode_mes;
					else
						mp=&mode_sig;
					switch (handle_encoding2(value,l)) {
					case MODE_NIX: *mp=MODE_NIX; break;
					case MODE_B64: *mp=MODE_B64; break;
					case MODE_QP:  *mp=MODE_QP; break;
					}
				}
				r822_hash_destroy(&head);
			}
			if (phase==2)
				continue;
			sa.len=olen;
		}
		if (phase==1) {
			if (sa.len>5 && str_start(sa.s,"-----"))
				if (-1==buffer_put(mes,"- ",2)) 	
					die_write_mes();
			if (-1==buffer_put(mes,sa.s,sa.len)) 	
				die_write_mes();
		} else {
			if (-1==buffer_put(sig,sa.s,sa.len)) 	
				die_write_sig();
		}
		continue;
	}
	if (phase!=3)
		xbailout(100,0,"cannot find enough MIME parts for multipart/signed",
			0,0,0);
	if (-1==buffer_flush(mes))
		die_write_mes();
	if (-1==buffer_flush(sig))
		die_write_sig();
	stralloc_free(&sa);
	/* note: the message is now one character too long, since the lf
	 * before the boundary belongs to the boundary.
	 * On the other hand this LF is would have to be prepended
	 * before the BEGIN PGP SIGNATURE line, so we just leave it
	 * in and strip it later.
	 */
}

static int
is_pgp_line(stralloc *sa, const char *s, unsigned int slen)
{
	if (sa->s[0]!='-')
		return 0;
	if (sa->len<=slen)	
		return 0;
	if (!byte_equal(sa->s,slen,s))
		return 0;
	if (sa->s[slen]=='\n')
		return 1;
	if (sa->s[slen]!='\r')
		return 0;
	if (sa->len<slen+1)
		return 0;
	if (sa->s[slen+1]!='\n')
		return 0;
	return 1;
}

static void
handle_plain(buffer *in, buffer *mes, buffer *sig)
{
	stralloc sa=STRALLOC_INIT;
	int gotlf;
	int flag_inheader=0;
	int encrypted=0;
	char BPM[]="-----BEGIN PGP MESSAGE-----";
	char EPM[]="-----END PGP MESSAGE-----";
	char BPSM[]="-----BEGIN PGP SIGNED MESSAGE-----";
	char BPS[]="-----BEGIN PGP SIGNATURE-----";
	char EPS[]="-----END PGP SIGNATURE-----";
	/* 0: vor -----BEGIN PGP SIGNED MESSAGE-----
	 * 1: nach -----BEGIN PGP SIGNED MESSAGE-----, das heisst nachricht
	 * 2: nach -----BEGIN PGP SIGNATURE-----, das heisst signature
	 * 3: nach END PGP SIGNATURE or END PGP MESSAGE, das heisst fertig
	 * 11: after BEGIN PGP MESSAGE
	 */
	int phase=0; 
	while (1) {
		sa.len=0;
		if (-1==getln(in,&sa,&gotlf,'\n'))
			die_mail_read();
		if (!sa.len) break;
		if (!gotlf)
			die_mail_eof();
		if (sa.s[0]=='-') {
			if (is_pgp_line(&sa,BPM,sizeof(BPM)-1)) {
				if (phase!=0)
					xbailout(100,0,"cannot parse PGP structure: "
						"unexpected BEGIN PGP MESSAGE",0,0,0);
				phase=11;
				encrypted=1;
				flag_inheader=0; /* really 1, but this header is needed */
				if (-1==buffer_put(sig,sa.s,sa.len)) 	
					die_write_sig();
				continue;
			}
			if (is_pgp_line(&sa,BPSM,sizeof(BPSM)-1)) {
				if (phase!=0)
					xbailout(100,0,"cannot parse PGP structure: "
						"unexpected BEGIN PGP SIGNED MESSAGE",0,0,0);
				phase=1;
				flag_inheader=1;
				continue;
			}
			if (is_pgp_line(&sa,BPS,sizeof(BPS)-1)) {
				if (phase!=1)
					xbailout(100,0,"cannot parse PGP structure: "
						"unexpected BEGIN PGP SIGNATURE",0,0,0);
				phase=2;
				flag_inheader=0; /* 1, but this header is needed */
			}
			if (is_pgp_line(&sa,EPS,sizeof(EPS)-1)) {
				if (phase!=2)
					xbailout(100,0,"cannot parse PGP structure: "
						"unexpected END PGP SIGNATURE",0,0,0);
				phase=3;
				if (-1==buffer_put(sig,sa.s,sa.len)) 	
					die_write_sig();
				break;
			}
			if (is_pgp_line(&sa,EPM,sizeof(EPM)-1)) {
				if (phase!=11)
					xbailout(100,0,"cannot parse PGP structure: "
						"unexpected END PGP MESSAGE",0,0,0);
				phase=3;
				if (-1==buffer_put(sig,sa.s,sa.len)) 	
					die_write_sig();
				break;
			}
		}
		if (phase==0) continue;
		if (flag_inheader) {
			sa.len--;
			if (sa.len && sa.s[sa.len-1]=='\r')
				sa.len--;
			if (sa.len==0)
				flag_inheader=0;
			else {
				if (!stralloc_0(&sa)) oom();
				if (case_starts(sa.s,"Hash:")) {
					unsigned int spc=5;
					while(sa.s[spc]==' ' || sa.s[spc]=='\t')
						spc++;
					if (!stralloc_copys(&cpara.micalg,sa.s+spc)) oom();
				}
			}
			continue;
		}
		if (sa.s[sa.len-1]=='\n' && phase==1) {
			if (sa.len==1 || (sa.len==2 && sa.s[0]=='\r')) {
				if (-1==buffer_put(mes,sa.s,sa.len)) 	
					die_write_mes();
				continue;
			}
		}
		if (phase==1) {
			if (-1==buffer_put(mes,sa.s,sa.len)) 	
					die_write_mes();
		} else {
			if (-1==buffer_put(sig,sa.s,sa.len)) 	
				die_write_mes();
		}
		continue;
	}
	if (phase!=3)
		xbailout(100,0,"cannot find enough PGP parts",
			0,0,0);
	if (-1==buffer_flush(mes))
		die_write_mes();
	if (-1==buffer_flush(sig))
		die_write_sig();
	stralloc_free(&sa);
	if (encrypted)
		content_type=CT_ENCRYPTED;
}

/* x-pgp-sig contains the ppg signature and holds some header names 
 * to add to the payload, the body contains parts of the payload, too.
 */
static void
handle_news(buffer *in, buffer *mes, buffer *sig)
{
	stralloc sa=STRALLOC_INIT;
	unsigned int i;
	unsigned int j;
	char *headers;
	char *p;
	mode_sig=0;
	for (i=0;x_pgp_sig[i];i++)
		if (x_pgp_sig[i]==' ' || x_pgp_sig[i]=='\t')
			break;
	if (x_pgp_sig[i]==0)
		die_x_pgp_sig();

	if (-1==buffer_puts(sig,"-----BEGIN PGP SIGNATURE-----\n"))
			die_write_mes();
	if (-1==buffer_puts(sig,"Version: ")) die_write_mes();
	if (-1==buffer_put(sig,x_pgp_sig,i)) die_write_mes();
	if (-1==buffer_puts(sig,"\n\n")) die_write_mes();

	while (x_pgp_sig[i]==' ' || x_pgp_sig[i]=='\t' || x_pgp_sig[i]=='\n')
		i++;
	j=i;
	while (x_pgp_sig[j]!=' ' && x_pgp_sig[j]!='\t' && x_pgp_sig[j]!='\n'
		&& x_pgp_sig[j])
		j++;
	if (x_pgp_sig[j]==0)
		die_x_pgp_sig();
	if (-1==buffer_puts(mes,"X-Signed-Headers: ")) die_write_mes();
	if (-1==buffer_put(mes,x_pgp_sig+i,j-i)) die_write_mes();
	if (-1==buffer_puts(mes,"\n")) die_write_mes();

	headers=x_pgp_sig+i;
	for (p=x_pgp_sig+i;x_pgp_sig[i];i++) {
		int flag=0;
		if (x_pgp_sig[i]==' ' || x_pgp_sig[i]=='\t' || x_pgp_sig[i]=='\n')
			flag=1;
		if (flag==1 || x_pgp_sig[i]==',') {
			const char *q;
			x_pgp_sig[i]=0;
			if (!stralloc_copys(&sa,p)) oom();
			case_lowerb(sa.s,sa.len);
			if (!stralloc_0(&sa)) oom();
			q=mail_header(sa.s);
			if (!q)
				xbailout(100,0,"need ",sa.s, " header, but there is none",0);
			if (-1==buffer_puts(mes,p)) die_write_mes();
			if (-1==buffer_puts(mes,": ")) die_write_mes();
			if (-1==buffer_puts(mes,q)) die_write_mes();
			if (-1==buffer_puts(mes,"\n")) die_write_mes();
			p=x_pgp_sig+i+1;
			x_pgp_sig[i]=' ';
		}
		if (flag)
			break;
	}
	if (x_pgp_sig[i]==0)
		die_x_pgp_sig();
	for (;x_pgp_sig[i];i++) {
		while (x_pgp_sig[i]==' ' || x_pgp_sig[i]=='\t')
			i++;
		if (-1==buffer_put(sig,x_pgp_sig+i,1)) die_write_sig();
	}
	if (-1==buffer_puts(sig,"\n-----END PGP SIGNATURE-----\n"))
			die_write_mes();
	if (-1==buffer_flush(sig))
		die_write_sig();

/*
X-PGP-Sig: 2.6.3a From,Subject,Newsgroups,Followup-To,Sender,Message-ID,
+          Approved,Date
+          iQCVAwUBOsETzZE2Z+0RllKdAQGVAgP/ar3wBA2rTB4UcbYNVr0gWFdaXiwFOllc
+          QFHeiV8iui80vtXyodv2BrXhe9Fq2tiBrdev4IkBpXQlCpsw0AhMiNotMamdvSqa
+          yaXl8jP2qJ5MCdMbrQR6hUt/3BZPz4OvtidXUKJAn+icCEWjV+1C3gHLSpJgncAx
+          l2U6Ny9Y6eM=
+          =m53g
*/	
	if (-1==buffer_puts(mes,"\n")) die_write_mes();
	while (1) {
		int gotlf;
		sa.len=0;
		if (-1==getln(in,&sa,&gotlf,'\n'))
			xbailout(111,errno,"failed to read mail",0,0,0);
		if (!sa.len) break;
		if (!gotlf)
			xbailout(111,0,"premature EOF reading mail",0,0,0);
		if (-1==buffer_put(mes,sa.s,sa.len)) 	
			die_write_mes();
	}
	if (-1==buffer_puts(mes,"\n")) die_write_mes();
	if (-1==buffer_flush(mes))
		die_write_mes();
	stralloc_free(&sa);
}

static int
open_tmp(void)
{
	int fd;
	int fd2;
	static stralloc fn;
	char nb[FMT_ULONG];
	if (!stralloc_copys(&fn,tmpdir)) oom();
	if (!stralloc_cats(&fn,"/")) oom();
	if (!stralloc_catb(&fn,nb,fmt_ulong(nb,getpid()))) oom();
	if (!stralloc_cats(&fn,".tmp")) oom();
	if (!stralloc_0(&fn)) oom();
	unlink(fn.s);
	fd=open_excl(fn.s);
	if (fd==-1) xbailout(111,errno,"failed to open ",fn.s,0,0);
	fd2=open_readwrite(fn.s);
	if (fd2==-1) xbailout(111,errno,"failed to open ",fn.s,0,0);
	unlink(fn.s);
	close(fd);
	nb[fmt_ulong(nb,fd2)]=0;
	if (!pathexec_env("MESSAGE_FD",nb)) oom();
	return fd2;
}

static void
check2(struct cdb *cdb)
{
	static stralloc x=STRALLOC_INIT;
	uo_uint32_t datalen;
	unsigned int i;

	datalen=cdb_datalen(cdb);
	if (!stralloc_ready(&x,datalen)) oom();
	if (cdb_read(cdb,x.s,datalen,cdb_datapos(cdb)))
		xbailout(111,errno,"failed to read in CDB",0,0,0);
	x.len=datalen;
	if (!stralloc_0(&x)) oom();
	if (x.s[0]=='D')
		xbailout(100,0,"access denied",0,0,0);
	for (i=0;i<x.len-1;) { /* -1: compensate extra \0 */
		unsigned int j;
		if (x.s[i]!='+') 
			xbailout(111,0,"trouble understanding rules database",0,0,0);
		i++;
		j=str_chr(x.s+i,'=');
		if (!x.s[i+j])
			xbailout(111,0,"trouble understanding rules database",0,0,0);
		x.s[i+j]=0;
		pathexec_env(x.s+i,x.s+i+j+1);
		i+=j+1;
		while (x.s[i])
			i++;
		i++;
	}
}
static void die_cdb_open(void)
{ xbailout(111,errno,"failed to open ",opt_rulesfile,0,0);}
static void die_cdb_failed(void)
{ xbailout(111,errno,"failed to search in ",opt_rulesfile,0,0);}

static void check_cdb(void)
{
	int fd;
	static struct cdb cdb;
	static stralloc k;
	static stralloc e;
	unsigned int i;
	fd=open_read(opt_rulesfile);
	if (-1==fd) 
		die_cdb_open();
	cdb_init(&cdb,fd);

	if (cres.fingerprint.s) {
		if (!stralloc_copys(&k,"f*")) oom();
		if (!stralloc_cats(&k,cres.fingerprint.s)) oom();
		switch(cdb_find(&cdb,k.s,k.len)) {
		case -1: die_cdb_failed();
		case 1: 
			check2(&cdb);
			return;
		}
	}
	if (cres.long_keyid.s) {
		if (!stralloc_copys(&k,"l*")) oom();
		if (!stralloc_cats(&k,cres.long_keyid.s)) oom();
		switch(cdb_find(&cdb,k.s,k.len)) {
		case -1: die_cdb_failed();
		case 1: 
			check2(&cdb);
			return;
		}
	}

	if (!stralloc_copys(&k,"s*")) oom();
	if (!stralloc_cats(&k,cres.keyid.s)) oom();
	switch(cdb_find(&cdb,k.s,k.len)) {
	case -1: die_cdb_failed();
	case 1: 
		check2(&cdb);
		return;
	}

	if (!stralloc_copys(&k,"u*")) oom();
	if (!stralloc_cats(&k,cres.user.s)) oom();
	switch(cdb_find(&cdb,k.s,k.len)) {
	case -1: die_cdb_failed();
	case 1: 
		check2(&cdb);
		return;
	}

	switch (r822_address(&e, &cres.user, 0)) {
	case R822_OK:
		break;
	case R822_ERR_NOMEM: oom();
	default:
		xbailout(100,0,"access denied: error parsing user id",0,0,0);
		return;
	}

	for (i=0;i<e.len;i++) {
		if (e.s[i]=='A') {
			if (!stralloc_copys(&k,"a*")) oom();
			if (!stralloc_cats(&k,e.s+i+1)) oom();
			switch(cdb_find(&cdb,k.s,k.len)) {
			case -1: die_cdb_failed();
			case 1: 
				check2(&cdb);
				return;
			}
		}
	}
	if (!opt_rules_allow_default)
		xbailout(100,0,"access denied: unknown user",0,0,0);
}

static void
one2env(const char *hname, const char *ename)
{	
	const char *s;
	static stralloc t;
	unsigned int i;
	s=mail_header(hname);
	if (!s) return;
	t.len=0;
	for (i=0;s[i];i++) {
		unsigned char c;
		c=s[i];
		if (c>=32) 
			if (!stralloc_append(&t,s+i)) oom();
	}
	if (!stralloc_0(&t)) oom();
	if (!pathexec_env(ename,t.s)) oom();
}
static char **nav;
static unsigned int navi;
static unsigned int navn;

void
av_add(const char *s)
{
	union { char *c; const char *cc;} x;
	if (navi+1>=navn) {
		if (!navn) {
			navn=20;
			nav=(char **) alloc(navn*sizeof(char *));
		} else {
			navn*=2;
			if (!alloc_re((char **)&nav,navn/2 * sizeof(char *),
					navn * sizeof(char *)))
			if (!nav) oom();
		}
	}
	x.cc=s;
	nav[navi++]=x.c;
	nav[navi]=0;
}
void
av_add_copy(const char *s)
{
	int x=str_len(s)+1;
	char *p;
	p=(char *) alloc(x);
	if (!p) oom();
	byte_copy(p,x,s);
	av_add(p);
}
char ** av_get(void) { return nav; }

static void
header2environment(void)
{
	one2env("from","HEADER_FROM");
	one2env("sender","HEADER_SENDER");
	one2env("cc","HEADER_CC");
	one2env("to","HEADER_TO");
	one2env("subject","HEADER_SUBJECT");
	one2env("reply-to","HEADER_REPLY_TO");
	if (opt_news_verify) {
		one2env("followup-to","HEADER_FOLLOWUP_TO");
		one2env("newsgroups","HEADER_NEWSGROUPS");
	}
}
static void
handle_new_header(buffer *tb)
{
	const char *p;
	if (content_type==CT_ENCRYPTED) {
		if (-1==buffer_puts(tb,"\n")) 
			die_write_tmp();
		return;
	}
	p=mail_header("mime-version");
	if (!p || content_type==CT_UNKNOWN) {
		if (-1==buffer_puts(tb,"\n")) 
			die_write_tmp();
		return;
	}
	if (-1==buffer_puts(tb,"Mime-Version: ")
	 || -1==buffer_puts(tb,p)
	 || -1==buffer_puts(tb,"\n"))
			die_write_tmp();
}

uogetopt_t opts[]={
{'G',"gpg",UOGO_FLAG,&opt_type,TYPE_GPG,
 "Use GNU privacy guard.\n"
 "This is the default anyway. If 'gpg' is not\n"
 "in $PATH then use the --exec option.",0},
{'2',"pgp2",UOGO_FLAG,&opt_type,TYPE_PGP2,
 "Use PGP version 2.\n"
 "If 'pgp' is not in $PATH then use the --exec\n"
 "option.",0},
{'5',"pgp5",UOGO_FLAG,&opt_type,TYPE_PGP5,
 "Use PGP version 5.\n"
 "If 'pgpv' is not in $PATH then use the --exec\n"
 "option.",0},
{'e',"exec",UOGO_STRING,&cpara.xprog,0,
 "Executable to run for signature check.\n"
 "Use this program to check the signature.\n"
 "See --pgp2, --pgp5 and --gpg.", "PROGRAM"},
{'\0',"allow-key-retrieve",UOGO_FLAG,&cpara.allow_key_retrieve,0,
"Allow gpg to get missing keys from a key server.\n"
"GPG by default tries to retrieve missing public\n"
"keys from a key server. The upgpverify is to\n"
"disable this.\n"
"If you want to automatically retrieve keys then use\n"
"this option, which is ignored for PGP2 and PGP5.",0},
{'M',"max-len",UOGO_ULONG,&opt_max_len,0,
 "Maximum incoming message length in bytes.\n"
 "The default is 1 MB.\n","BYTES"},
{'P',"passphrase-fd",UOGO_ULONG,&opt_passphrase_fd,0,
 "Read passphrase from file descriptor N.\n"
 "Use this only if needed to decrypt messages.\n"
 "Do not use file descriptors lower than 3", "N"},
{'p',"pubring",UOGO_STRING,&cpara.pubring,0,
 "Public key ring file name.\n"
 "Note that PGP (not GPG) will read the key ring in\n"
 "$HOME/.pgp anyway, unless it's nonexistant.","PUBRING"},
{'r',"rules",UOGO_STRING,&opt_rulesfile,0,
 "Rules file name.", "RULESFILE"},
{'A',"allow-is-default",UOGO_FLAG,&opt_rules_allow_default,1,
 "Allow if no rule matches.\n"
 "The default is to deny access if no rule matches\n"
 "the signers key. Using this option changes the\n"
 "default to allow access.",0},
{'s',"secring",UOGO_STRING,&cpara.secring,0,
 "Use secret key ring SECRING.\n"
 "Note that PGP (not GPG) will read the key ring in\n"
 "$HOME/.pgp anyway, unless it's nonexistant.","SECRING"},
{'x',"allow-x-pgp-sig",UOGO_FLAG,&opt_news_verify,1,
 "Also handle news signed with x-pgp-sig.\n"
 "The program will parse the x-pgp-sig header and\n"
 "create a pgp payload from header and body.",0},
{'X',"require-x-pgp-sig",UOGO_FLAG,&opt_news_verify,2,
 "Treat message as news signed with x-pgp-sig.\n"
 "The program will parse the x-pgp-sig header and\n"
 "create a pgp payload from header and body.\n"
 "This differs from --allow-x-pgp-sig in that\n"
 "--require-x-pgp-sig will fail if the message\n"
 "has some other kind of PGP signature.",0},
{0,0,0,0,0,0,0}
};

int main(int argc, char **argv)
{
	const char *from;
	const char *subject;
	unsigned int headlen;
	int fd;
	unsigned int l;
	static stralloc rest;

	l=str_rchr(argv[0],'/');
	if (argv[0][l] && argv[0][l+1])
		flag_bailout_log_name=argv[0]+l+1;
	else
		flag_bailout_log_name=argv[0];


	uogo_posixmode=1;
	uogetopt(flag_bailout_log_name,PACKAGE,VERSION,
		&argc, argv, uogetopt_out,
		"[TMPDIR [program ...]]",
		opts, 0);
	tmpdir=argv[1];
	if (tmpdir)
		argv++;
	if (!opt_passphrase_fd)
		cpara.passphrase_fd=-1;
	else {
		if (opt_passphrase_fd < 3)
			xbailout(100,0,
				"passphrase file descriptor too low (>=3 excpected).",0,0,0);
		cpara.passphrase_fd=opt_passphrase_fd;
	}

	headlen=mail_parse_header(0,&rest);
	from=mail_header("from");
	if (!from) xbailout(100,0,"no from header",0,0,0);
	if (opt_news_verify) {
		const char *qs;
		unsigned int qsl;
		qs=mail_header("x-pgp-sig");
		if (qs) {
			qsl=str_len(qs)+1;
			x_pgp_sig=alloc(qsl);
			if (!x_pgp_sig) oom();
			byte_copy(x_pgp_sig,qsl,qs);
		} else if (opt_news_verify==2) 
			xbailout(100,0,"no x-pgp-sig header",0,0,0);
	}
	subject=mail_header("subject");
	if (rest.s && rest.len) {
		buffer bt;
		if (!tmpdir)
			xbailout(100,0,
				"cannot handle input from pipes without a temporary directory",
				0,0,0);
		fd=open_tmp();
		buffer_init(&bt,(buffer_op_t) write, fd,messpace,sizeof(messpace));
		if (-1==buffer_put(&bt,rest.s,rest.len)) 
			xbailout(111,errno,"failed to write to temporary file",0,0,0);
		buffer_init(&b,(buffer_op_t) read, 0,bspace,sizeof(bspace));
		while (1) {
			int r;
			char *x;
			r = buffer_feed(&b);
			if (r<0) {
				xbailout(111,errno,"failed to write to read mail",0,0,0);
			}
			if (r==0)
				break;
			x = buffer_peek(&b);
			if (-1==buffer_put(&bt,x,r)) 
				xbailout(111,errno,"failed to write to temporary file",0,0,0);
			buffer_seek(&b,r);
		}
		if (-1==buffer_flush(&bt)) 
			xbailout(111,errno,"failed to write to temporary file",0,0,0);
		if (-1==fd_move(0,fd))
			xbailout(111,errno,"failed to move file descriptor",0,0,0);
		stralloc_free(&rest);
	}
	seek_set(0,headlen);
	buffer_init(&b,(buffer_op_t) read, 0,bspace,sizeof(bspace));
	buffer_init(&mesbuf,(buffer_op_t) write2mes, 0,messpace,sizeof(messpace));
	buffer_init(&sigbuf,(buffer_op_t) write2sig, 0,sigspace,sizeof(sigspace));

	if (mail_header("mime-version")) {
		const char *ct0;
		ct0=mail_header("content-type");
		if (ct0)
			parse_content_type(ct0);
		if (content_type==CT_MPART_SIGNED) {
			handle_multipart_signed(&b,&mesbuf,&sigbuf);
		} else if (content_type==CT_MPART_ENCRYPTED) {
			handle_multipart_encrypted(&b,&mesbuf,&sigbuf);
		} else {
			switch (handle_encoding()) {
			case MODE_NIX: mode_mes=mode_sig=MODE_NIX; break;
			case MODE_B64: mode_mes=mode_sig=MODE_B64; break;
			case MODE_QP:  mode_mes=mode_sig=MODE_QP; break;
			}
			/* MIME-Version, but traditional format.
			 * get rid of encoding _now_. */
			if (x_pgp_sig)
				handle_news(&b,&mesbuf,&sigbuf);
			else
				handle_plain(&b,&mesbuf,&sigbuf);
			if (mode_mes==MODE_QP) {
				if (!mime_decode_qp(&cpara.message)) die_qp();
				mode_mes=0;
			} else if (mode_mes==MODE_B64) {
				if (!mime_decode_b64(&cpara.message)) die_b64();
				mode_mes=0;
			}
		}
	} else if (x_pgp_sig) {
		handle_news(&b,&mesbuf,&sigbuf);
	} else {
		handle_plain(&b,&mesbuf,&sigbuf);
	}
	if (mode_sig==MODE_QP) {
		if (!mime_decode_qp(&cpara.signature)) die_qp();
	}
	else if (mode_sig==MODE_B64) {
		if (!mime_decode_b64(&cpara.signature)) die_b64();
	}
	check_sig();
	/* need to strip one LF now. It's part of the boundary, not part
	 * of the message, and was left in this message because an 
	 * a \n was needed before the BEGIN PGP SIGNATURE.
	 */
	if (content_type!=CT_MPART_ENCRYPTED)
		if (cpara.message.len)
			cpara.message.len--;
	if (mode_mes) {
		if (boundary.len) {
			stralloc s0=STRALLOC_INIT;
			stralloc s1=STRALLOC_INIT;
			unsigned int i;
			unsigned int nexti;
			char *s;
			/* if we have:
			 *   --part
			 *   content-transfer-encoding: x
			 *   other-header: ...
			 *
			 *   body
			 * then body and not the whole message needs to be 
			 * undecoded. We throw away the "content-transfer-encoding" header.
			 */
			if (!stralloc_0(&cpara.message)) oom();
			s=cpara.message.s;
			for (i=0;s[i];i=nexti) {
				nexti=i+str_chr(s+i,'\n');
				if (s[nexti])
					nexti++;
				if (case_starts(s+i,"content-transfer-encoding:"))
					continue;
				if (!stralloc_catb(&s0,s+i,nexti-i)) oom();
				if (s[i]=='\n')
					break;
			}
			if (!stralloc_copyb(&s1,cpara.message.s+i+1,
					cpara.message.len-i-2)) oom(); /* 2 due to \0 */
			if (mode_mes==MODE_QP) {
				if (!mime_decode_qp(&s1)) die_qp();
			} else {
				if (!mime_decode_b64(&s1)) die_b64();
			}
			if (!stralloc_copy(&cpara.message,&s0)) oom();
			if (!stralloc_cat(&cpara.message,&s1)) oom();
			stralloc_free(&s1);
		} else {
			if (mode_mes==MODE_QP) {
				if (!mime_decode_qp(&cpara.message)) die_qp();
			} else {
				if (!mime_decode_b64(&cpara.message)) die_b64();
			}
		}
	}

	if (opt_rulesfile && *opt_rulesfile)
		check_cdb();

	if (!argv[1])
		_exit(0);
	fd=open_tmp();
	buffer_init(&mesbuf,(buffer_op_t) write, fd,messpace,sizeof(messpace));

	handle_new_header(&mesbuf);

	if (-1==buffer_put(&mesbuf,cpara.message.s,cpara.message.len)) 
		die_write_tmp();
	if (-1==buffer_flush(&mesbuf)) 
		die_write_tmp();
	if (-1==seek_begin(fd))
		xbailout(111,errno,"failed to seek in temporary file",0,0,0);

	header2environment();

	pathexec(argv+1);
	xbailout(111,errno,"failed to execute ",argv[1],0,0);
}

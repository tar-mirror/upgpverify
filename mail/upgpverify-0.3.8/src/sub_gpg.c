#include "attributes.h"
#include "error.h"
#include "stralloc.h"
#include "open.h"
#include "nowarn.h"
#include "close.h"
#include "str.h"
#include "byte.h"
#include "buffer.h"
#include "fmt.h"
#include "readwrite.h"
#include "env.h"
#include "seek.h"
#include "getln.h"
#include "case.h"
#include "fd.h"
#include "pathexec.h"
#include "bailout.h"
#include "upgpverify.h"

static buffer b;
static char bspace[1024];
static void die_pipe_write(void) attribute_noreturn;
static void die_pipe_write(void)
{ xbailout(111,errno,"failed to write to pipe",0,0,0); }

int 
check_gpg_sig(struct check_param *par, struct check_result *res)
{
	int pid;
	int pi0[2];
	int pi1[2];
	int pi2[2];
	int devnull;
	static stralloc sa;
	static stralloc *errmes;
	int found_begin_decryption=0;
	int found_end_decryption=0;
	int found_decryption_okay=0;
	stralloc b2=STRALLOC_INIT;

	sa.len=0;
	if (-1==pipe(pi0))
		xbailout(111,errno,"failed to create pipe",0,0,0);
	if (-1==pipe(pi1))
		xbailout(111,errno,"failed to create pipe",0,0,0);
	if (-1==pipe(pi2))
		xbailout(111,errno,"failed to create pipe",0,0,0);
	devnull=open_append("/dev/null");
	if (devnull==-1)
		xbailout(111,errno,"failed to open /dev/null",0,0,0);
	pid=fork();
	if (pid==-1)
		xbailout(111,errno,"failed to fork",0,0,0);

	if (pid==0) {
		char nb1[FMT_ULONG];
		char nb2[FMT_ULONG];
		av_add(par->xprog ? par->xprog : "gpg");
		av_add("--batch");

		if (par->pubring || par->secring)
			av_add("--no-default-keyring");
		if (par->pubring) {
			av_add("--keyring");
			av_add(par->pubring);
		}
		if (par->secring) {
			av_add("--secret-keyring");
			av_add(par->secring);
		}
		if (par->need_decryption)
			av_add("--decrypt");
		else
			av_add("--verify");
		av_add("--quiet");
		if (!par->allow_key_retrieve)
			av_add("--no-auto-key-retrieve");

		/* fd0: read input */
		if (-1==fd_move(0,pi0[0]))
			xbailout(111,errno,"failed to move file descriptor",0,0,0);
		/* fd1: write output */
		if (-1==fd_move(1,pi1[1]))
			xbailout(111,errno,"failed to move file descriptor",0,0,0);

		/* /dev/null for logging, we use --status-fd */
		av_add("--logger-fd");
		nb1[fmt_ulong(nb1,devnull)]=0;
		av_add(nb1);

		/* fd2: status messages */
		av_add("--status-fd");
		av_add("2");
		if (-1==fd_move(2,pi2[1]))
			xbailout(111,errno,"failed to move file descriptor",0,0,0);

		if (par->passphrase_fd != -1) {
			av_add("--passphrase-fd");
			nb2[fmt_ulong(nb2,par->passphrase_fd)]=0;
			av_add(nb2);
		}

		close(pi0[1]);
		close(pi1[0]);
		close(pi2[0]);
		pathexec(av_get());
		xbailout(111,errno,"failed to execute ",av_get()[0],0,0);
	}
	close(pi0[0]);
	close(pi1[1]);
	close(pi2[1]);
	buffer_init(&b,(buffer_op_t) write, pi0[1],bspace,sizeof(bspace));
	if (!par->need_decryption) {
		if (-1==buffer_puts(&b,"-----BEGIN PGP SIGNED MESSAGE-----\n"))
			die_pipe_write();
		if (par->micalg.s && par->micalg.len) {
			if (-1==buffer_puts(&b,"Hash: ")
				|| -1==buffer_put(&b,par->micalg.s,par->micalg.len)
				|| -1==buffer_puts(&b,"\n"))
				die_pipe_write();
		}
		if (-1==buffer_puts(&b,"\n")
		||  -1==buffer_put(&b,par->message.s,par->message.len)
		||  -1==buffer_put(&b,par->signature.s,par->signature.len))
			die_pipe_write();
	} else {
		/* "signature" is a complete PGP message */
		if (-1==buffer_put(&b,par->signature.s,par->signature.len))
			die_pipe_write();
	}
	if (-1==buffer_flush(&b)) 
		die_pipe_write();
	close(pi0[1]);

	if (par->passphrase_fd == -1) {
		/* just check signature */
		read_2_fd(pi1[0],&res->output, 1024, pi2[0],&b2, 4096);
	} else {
		read_2_fd(pi1[0],&res->output, 0, pi2[0],&b2, 4096);
	}

	/* now parse the status fd output */
	{
		unsigned int i;
		char *s;
		unsigned int l;
		s=b2.s;
		l=b2.len;
		i=0;
		while (i!=l) {
			unsigned int j;
			char *t;
			j=i+byte_chr(s+i,l-i,'\n');
			if (j==i) {
				i++;
				continue;
			}
			if (j==l)
				xbailout(111,errno,
					"unexpected end of file reading PGP output",0,0,0);

			/* safe due to \n */
			if (byte_equal(s+i,15,"[GNUPG:] TRUST_")) { i=j+1; continue; }
			if (byte_equal(s+i,15,"[GNUPG:] SIG_ID")) { i=j+1; continue; }
			s[j]=0;
			t=s+i;
			warning(0,t,0,0,0);
			if (!str_start(t,"[GNUPG:] ")) { i=j+1; continue; }
			t+=9;
			if (str_start(t,"GOODSIG ")) {
				/* GOODSIG 0123456789abcdef user id */
				unsigned int spc;
				t+=8;
				if (str_len(t)<16+2) continue;
				spc=str_chr(t,' ');
				if (!t[spc]) continue;
				if (!stralloc_copys(&res->user,t+spc+1)) oom();
				if (!stralloc_copyb(&res->long_keyid,t,16)) oom();
				if (!stralloc_copyb(&res->keyid,t+8,8)) oom();
			}
			if (str_start(t,"ENC_TO ")) {
				/* ENC_TO  <long keyid>  <keytype>  <keylength> */
				unsigned int spc;
				t+=7;
				if (str_len(t)<16) continue;
				spc=str_chr(t,' ');

				if (!t[spc]) continue;
				if (!stralloc_copyb(&res->enc_to,t,spc)) oom();
			}
			if (str_start(t,"VALIDSIG ")) {
				/* GOODSIG fingerprint sig_date sig_timestamp */
				unsigned int spc;
				t+=9;
				if (str_len(t)<32) continue;
				spc=str_chr(t,' ');

				if (!t[spc]) continue;
				if (!stralloc_copyb(&res->fingerprint,t,spc)) oom();
			}
			if (str_start(t,"BEGIN_DECRYPTION"))
				found_begin_decryption=1;
			if (str_start(t,"END_DECRYPTION"))
				found_end_decryption=1;
			if (str_start(t,"DECRYPTION_OKAY"))
				found_decryption_okay=1;

			s[j]='\n';
			i=j+1;
		}
	}
	if (par->need_decryption) {
		if (!found_begin_decryption)
			xbailout(100,0,"Unable to find BEGIN_DECRYPTION marker",0,0,0);
		if (!found_end_decryption)
			xbailout(100,0,"Unable to find END_DECRYPTION marker",0,0,0);
		if (!found_decryption_okay)
			xbailout(100,0,"Unable to find DECRYPTION_OKAY marker",0,0,0);
		if (!res->enc_to.s || !res->enc_to.len)
			xbailout(100,0,"Unable to find ENC_TO marker",0,0,0);
	}

	if (res->fingerprint.s  && res->keyid.s) {
		close(pi1[0]);
		close(pi2[0]);
		return pid;
	}

	errmes=&b2;
	if (!errmes->s)
		errmes=&res->output;
	{
		unsigned int i;
		if (!stralloc_0(errmes)) oom();
		i=str_chr(errmes->s,'\n');
		errmes->s[i]=0;
		xbailout(100,0,"PGP verification failed: ",errmes->s,0,0);
	}
}

#include "error.h"
#include "stralloc.h"
#include "open.h"
#include "nowarn.h"
#include "close.h"
#include "str.h"
#include "byte.h"
#include "buffer.h"
#include "readwrite.h"
#include "env.h"
#include "seek.h"
#include "getln.h"
#include "case.h"
#include "fmt.h"
#include "fd.h"
#include "pathexec.h"
#include "bailout.h"
#include "upgpverify.h"

static buffer b;
static char bspace[1024];

static void printit(unsigned char *s)
{
	unsigned int i;
	i=str_len(s);
	if (i && s[i-1]=='\a')
		s[i-1]=0;
	for (i=0;s[i];i++)
		if (s[i]<' ' || s[i] >0x7f)
			s[i]='_';
	if (s[1])
		warning(0,s,0,0,0);
}


int 
check_pgp2_sig(struct check_param *par, struct check_result *res)
{
	int pid;
	int pi0[2];
	int pi1[2];
	int pi2[2];
	int state;
	static stralloc sa;
	static stralloc b2;
	static stralloc errmes;

	sa.len=0;
	if (-1==pipe(pi0))
		xbailout(111,errno,"failed to create pipe",0,0,0);
	if (-1==pipe(pi1))
		xbailout(111,errno,"failed to create pipe",0,0,0);
	if (-1==pipe(pi2))
		xbailout(111,errno,"failed to create pipe",0,0,0);
	pid=fork();
	if (pid==-1)
		xbailout(111,errno,"failed to fork",0,0,0);
	if (pid==0) {
		static stralloc sapub;
		static stralloc sasec;
		av_add(par->xprog ? par->xprog : "pgp");
		av_add("+force");
		if (par->need_decryption) {
			char nb[FMT_ULONG];
			nb[fmt_ulong(nb,par->passphrase_fd)]=0;
			if (!pathexec_env("PGPPASSFD",nb)) oom();
		} else {
			av_add("-o");
			av_add("-");
		}
		av_add("+batchmode");
		av_add("-ft");
		if (par->pubring) {
			if (!stralloc_copys(&sapub,"+PubRing=")) oom();
			if (!stralloc_cats(&sapub,par->pubring)) oom();
			if (!stralloc_0(&sapub)) oom();
			av_add(sapub.s);
		}
		if (par->secring) {
			if (!stralloc_copys(&sasec,"+SecRing=")) oom();
			if (!stralloc_cats(&sasec,par->secring)) oom();
			if (!stralloc_0(&sasec)) oom();
			av_add(sasec.s);
		}
		av_add("+VERBOSE=0");
		close(pi0[1]);
		close(pi1[0]);
		close(pi2[0]);
		if (-1==fd_move(0,pi0[0]))
			xbailout(111,errno,"failed to move file descriptor",0,0,0);
		if (-1==fd_move(1,pi1[1]))
			xbailout(111,errno,"failed to move file descriptor",0,0,0);
		if (-1==fd_move(2,pi2[1]))
			xbailout(111,errno,"failed to move file descriptor",0,0,0);
		pathexec(av_get());
		xbailout(111,errno,"failed to execute ",av_get()[0],0,0);
	}
	close(pi0[0]);
	close(pi1[1]);
	close(pi2[1]);
	buffer_init(&b,(buffer_op_t) write, pi0[1],bspace,sizeof(bspace));
	if (!par->need_decryption) {
		if (-1==buffer_puts(&b,"-----BEGIN PGP SIGNED MESSAGE-----\n"))
			xbailout(111,errno,"failed to write to pipe",0,0,0);
		if (-1==buffer_puts(&b,"\n"))
			xbailout(111,errno,"failed to write to pipe",0,0,0);
		if (-1==buffer_put(&b,par->message.s,par->message.len)) 
			xbailout(111,errno,"failed to write to pipe",0,0,0);
	}
	if (-1==buffer_put(&b,par->signature.s,par->signature.len)) 
		xbailout(111,errno,"failed to write to pipe",0,0,0);
	if (-1==buffer_flush(&b)) 
		xbailout(111,errno,"failed to write to pipe",0,0,0);
	close(pi0[1]);

	if (!par->need_decryption == -1) {
		/* just check signature */
		read_2_fd(pi1[0],&res->output, 1024, pi2[0],&b2, 4096);
	} else
		read_2_fd(pi1[0],&res->output, 0, pi2[0],&b2, 4096);
	close(pi1[0]);
	/* now parse the output on fd 2 */
	state=0;
	if (!stralloc_0(&b2)) oom(); /* pgp2 sometime forgets to add \n */
	{
		unsigned int l=b2.len;
		unsigned char *s=b2.s;
		unsigned int i=0;
		while (i!=l) {
			unsigned int j;
			j=i+byte_chr(s+i,l-i,'\n');
			if (j==i) {
				i++;
				continue;
			}
			s[j]=0;
#define FFIL1 "File has signature.  Public key is required to check signature"
#define FFIL2 "."
#define FFIL3 "Warning: Unrecognized ASCII armor header label"
#define FFIL4 "Pretty Good Privacy(tm)"
#define FFIL5 "\a"
#define PFIL1 "WARNING:  Because this public key is not certified with a trusted"
#define PFIL2 "signature, it is not known with high confidence that this public key"
#define PFIL3 "actually belongs to: \""
			if (str_start(s+i,FFIL1)
			||  str_start(s+i,FFIL2)
			||  str_start(s+i,FFIL3)
			||  str_start(s+i,FFIL4)
			||  str_start(s+i,FFIL5)
			||  str_start(s+i,PFIL1)
			||  str_start(s+i,PFIL2)
			||  str_start(s+i,PFIL3)) {
				s[j]='\n';
				i=j+1;
				continue;
			}
			if (!stralloc_cats(&errmes,s+i)) oom();
			if (!stralloc_cats(&errmes,"\n")) oom();
			printit(s+i);
#define GOODSIG	"Good signature from user \""
			if (state==0 && str_start(s+i,GOODSIG)) {
				state=1;
				if (!stralloc_copys(&res->user,s+i+str_len(GOODSIG))) oom();
				if (res->user.len && res->user.s[res->user.len-1]=='.')
					res->user.len--;
				if (res->user.len && res->user.s[res->user.len-1]=='\"')
					res->user.len--;
				if (!stralloc_0(&res->user)) oom();
			} else if (state==1) {
				unsigned int x;
				x=str_chr(s+i,',');
				state=99;
				if (s[i+x]) 
					if (str_start(s+i+x,", key ID ")) {
						if (!stralloc_copys(&res->keyid,s+i+x+9)) oom();
						if (!stralloc_0(&res->keyid)) oom();
						x=str_chr(res->keyid.s,',');
						res->keyid.s[x]=0;
						state=2;
					}
			}
			s[j]='\n';
			i=j;
			if (i!=l)
				i++;
		}
	}

	if (state!=2) {
		unsigned int i;
		char *s;
		if (!errmes.s || !errmes.len)
			if (!stralloc_copy(&errmes,&res->output)) oom();
		if (!stralloc_0(&errmes)) oom();
		i=str_chr(errmes.s,'\n');
		errmes.s[i]=0;
		s=errmes.s;
		if (*s=='\a')
			s++;
		xbailout(100,0,"PGP verification failed: ",s,0,0);
	}
	return pid;
}

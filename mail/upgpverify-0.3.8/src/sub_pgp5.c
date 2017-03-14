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
#include "fd.h"
#include "fmt.h"
#include "pathexec.h"
#include "bailout.h"
#include "upgpverify.h"

static buffer b;
static char bspace[1024];

int 
check_pgp5_sig(struct check_param *par, struct check_result *res)
{
	int pid;
	int pi0[2];
	int pi1[2];
	int pi2[2];
	int state;
	static stralloc sa;
	static stralloc b2;

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
		av_add(par->xprog ? par->xprog : "pgpv");
		av_add("+force");
		if (par->need_decryption) {
			char nb[FMT_ULONG];
			nb[fmt_ulong(nb,par->passphrase_fd)]=0;
			if (!pathexec_env("PGPPASSFD",nb)) oom();
		} else {
			av_add("-o");
			av_add("/dev/null");
		}
		av_add("+batchmode=1");
		av_add("+OutputInformationFD=8");
		av_add("-f");
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
		if (par->micalg.s && par->micalg.len) {
			if (-1==buffer_puts(&b,"Hash: "))
				xbailout(111,errno,"failed to write to pipe",0,0,0);
			if (-1==buffer_put(&b,par->micalg.s,par->micalg.len))
				xbailout(111,errno,"failed to write to pipe",0,0,0);
			if (-1==buffer_puts(&b,"\n"))
				xbailout(111,errno,"failed to write to pipe",0,0,0);
		}
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
	} else {
		read_2_fd(pi1[0],&res->output, 0, pi2[0],&b2, 4096);
	}
	close(pi1[0]);
	close(pi2[0]);

	/* now parse the output on fd 2 */
	state=0;
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
			if (j==l)
				xbailout(111,errno,
					"unexpected end of file reading PGP output",0,0,0);
			s[j]=0;
			if (state==0 && str_start(s+i,"Good signature made")) {
				warning(0,s+i,0,0,0);
				state=1;
			} else if (state==1) { unsigned int x;
				x=str_chr(s+i,',');
				state=99;
				warning(0,s+i,0,0,0);
				if (s[i+x]==',') 
					if (str_start(s+i+x,", Key ID ")) {
						if (!stralloc_copys(&res->keyid,s+i+x+9)) oom();
						if (!stralloc_0(&res->keyid)) oom();
						x=str_chr(res->keyid.s,',');
						res->keyid.s[x]=0;
						state=2;
					}
			} else if (state==2 || state==3) {
				if (str_start(s+i,"   \"")) {
					warning(0,s+i,0,0,0);
					if (state==2) {
						if (!stralloc_copys(&res->user,s+i+4)) oom();
						if (res->user.len && res->user.s[res->user.len-1]=='\"')
							res->user.len--;
						if (!stralloc_0(&res->user)) oom();
						state=3;
					}
				}
			}
			s[j]='\n';
			i=j+1;
		}
	}
	if (state!=3) {
		if (b2.s)
			xbailout(100,0,"PGP verification failed: ",b2.s,0,0);
		else
			xbailout(100,0,"PGP verification failed: ",res->output.s,0,0);
	}
	return pid;
}

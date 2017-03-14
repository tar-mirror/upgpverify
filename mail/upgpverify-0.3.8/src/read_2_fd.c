#include "iopause.h"
#include "stralloc.h"
#include "upgpverify.h"
#include "readwrite.h"
#include "error.h"
#include "nonblock.h"

static int 
doread(int fd, stralloc *sa, unsigned int max)
{
	int x;
	if (!max) max=sa->len+512;
	if (sa->len==max) {
		char buf[512];
		x=read(fd,buf,sizeof(buf));
	} else {
		if (!stralloc_readyplus(sa,max-sa->len)) return -1;
		x=read(fd,sa->s+sa->len,max-sa->len);
	}
	if (x>0) {
		if (sa->len!=max)
			sa->len+=x;
		return x;
	}
	if (x==0)
		return 0;
	if (errno==error_again || errno==error_wouldblock || errno==error_intr)
		return 1;
	return -1;
}

int
read_2_fd(int fd0, stralloc *s0, unsigned int max0, 
	int fd1, stralloc *s1, unsigned int max1)
{
	struct taia stamp;
	struct taia deadline;
	iopause_fd x[2];
	x[0].fd=fd0;
	x[1].fd=fd1;
	x[0].events=IOPAUSE_READ;
	x[1].events=IOPAUSE_READ;
	nonblock(fd0,1);
	nonblock(fd1,1);
	while (1) {
		taia_now(&stamp);
		taia_uint(&deadline,300);
		taia_add(&deadline,&deadline,&stamp);
		iopause(x,2,&deadline,&stamp);
		if (x[0].revents) {
			int y;
			y=doread(fd0,s0,max0);
			if (y==-1)
				return -1;
			if (y==0)
				x[0].events=0;
		}
		if (x[1].revents) {
			int y;
			y=doread(fd1,s1,max1);
			if (y==-1)
				return -1;
			if (y==0)
				x[1].events=0;
		}
		if (!x[0].events && !x[1].events)
			return 0;
	}
}

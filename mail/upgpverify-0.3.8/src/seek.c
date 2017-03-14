#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include "seek.h"

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

off_t seek_set(int fd, off_t offset)
	{ return lseek(fd,offset,SEEK_SET); }
off_t seek_cur(int fd)
	{ return lseek(fd,(off_t)0,SEEK_CUR); }
off_t seek_cur_off(int fd, off_t off)
	{ return lseek(fd,(off_t)off,SEEK_CUR); }
off_t seek_end(int fd)
	{ return lseek(fd,(off_t)0,SEEK_END); }


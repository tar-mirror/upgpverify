#! /bin/sh
#
# poll/emulation decision
# most stuff directly taken from djb@pobox.com
#

cat >conftest.c <<EOF
#include <sys/types.h>
#include <fcntl.h>
#include <poll.h>

main()
{
	struct pollfd x;

	x.fd = open("conftest.c",O_RDONLY);
	if (x.fd == -1) _exit(111);
	x.events = POLLIN;
	if (poll(&x,1,10) == -1) _exit(1);
	if (x.revents != POLLIN) _exit(1);

	/* XXX: try to detect and avoid poll() imitation libraries */

	_exit(0);
}
EOF
use=
./auto-compile.sh -c conftest.c  2>/dev/null >/dev/null
if test $? -ne 0 ; then
	use=select
else
	./auto-link.sh conftest conftest.o
	if test $? -ne 0 ; then
		use=select
	else
		./conftest
		if test $? -ne 0 ; then
			use=select
		else
			use=poll
		fi
	fi
fi

cat <<EOF
#ifndef IOPAUSE_H
#define IOPAUSE_H
extern int iopause_force_select; /* set to 1 to enforce usage of select() */
#include <sys/types.h>
#include <sys/time.h>
EOF

if test "x$use" = xpoll ; then
cat <<EOF
#define IOPAUSE_POLL /* systype-info */

#include <poll.h>

typedef struct pollfd iopause_fd;
#define IOPAUSE_READ POLLIN
#define IOPAUSE_WRITE POLLOUT

#include "taia.h"

extern void iopause(iopause_fd *,unsigned int,struct taia *,struct taia *);

#endif
EOF
rm -f conftest conftest.o conftest.c
exit 0
fi

# have to use select

cat >conftest.c <<EOF
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
EOF
./compile.sh -c conftest.c  2>/dev/null >/dev/null
if test $? -eq 0 ; then
cat <<EOF
#define IOPAUSE_SYS_SELECT_H /* systype-info */
#include <sys/select.h>
EOF
fi

cat <<EOF
#undef IOPAUSE_POLL /* systype-info */
typedef struct {
	int fd;
	short events;
	short revents;
} iopause_fd;

#define IOPAUSE_READ 1
#define IOPAUSE_WRITE 4

#include "taia.h"

extern void iopause(iopause_fd *,unsigned int,struct taia *,struct taia *);

#endif
EOF
rm -f conftest.c conftest.o conftest

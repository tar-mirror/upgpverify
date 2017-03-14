/* Reimplementation of Daniel J. Bernsteins error library.
 * (C) 2001 Uwe Ohse, <uwe@ohse.de>.
 *   Report any bugs to <uwe@ohse.de>.
 * Placed in the public domain.
 * This file is automatically generated.
 */
/* @(#) $Id: s.error_str.c 1.4 01/08/09 09:41:43+00:00 uwe@fjoras.ohse.de $ */

#include <errno.h>
#include "error.h"
const char *error_str(int ec) {
	if (ec==0) return "no error";
#ifdef EACCES
	if (ec==EACCES) return "access denied";
#endif
	if (ec==error_acces) return "access denied";
#ifdef EAGAIN
	if (ec==EAGAIN) return "temporary failure";
#endif
	if (ec==error_again) return "temporary failure";
#ifdef ECONNREFUSED
	if (ec==ECONNREFUSED) return "connection refused";
#endif
	if (ec==error_connrefused) return "connection refused";
#ifdef EEXIST
	if (ec==EEXIST) return "file already exists";
#endif
	if (ec==error_exist) return "file already exists";
#ifdef EINPROGRESS
	if (ec==EINPROGRESS) return "operation in progress";
#endif
	if (ec==error_inprogress) return "operation in progress";
#ifdef EINTR
	if (ec==EINTR) return "interrupted system call";
#endif
	if (ec==error_intr) return "interrupted system call";
#ifdef EIO
	if (ec==EIO) return "input/output error";
#endif
	if (ec==error_io) return "input/output error";
#ifdef EISDIR
	if (ec==EISDIR) return "is a directory";
#endif
	if (ec==error_isdir) return "is a directory";
#ifdef EMLINK
	if (ec==EMLINK) return "too many links";
#endif
	if (ec==error_mlink) return "too many links";
#ifdef ENXIO
	if (ec==ENXIO) return "device not configured";
#endif
	if (ec==error_nodevice) return "device not configured";
#ifdef ENOENT
	if (ec==ENOENT) return "file does not exist";
#endif
	if (ec==error_noent) return "file does not exist";
#ifdef ENOMEM
	if (ec==ENOMEM) return "out of memory";
#endif
	if (ec==error_nomem) return "out of memory";
#ifdef EPERM
	if (ec==EPERM) return "permission denied";
#endif
	if (ec==error_perm) return "permission denied";
#ifdef EPIPE
	if (ec==EPIPE) return "broken pipe";
#endif
	if (ec==error_pipe) return "broken pipe";
#ifdef EPROTO
	if (ec==EPROTO) return "protocol error";
#endif
	if (ec==error_proto) return "protocol error";
#ifdef ETIMEDOUT
	if (ec==ETIMEDOUT) return "timed out";
#endif
	if (ec==error_timeout) return "timed out";
#ifdef ETXTBSY
	if (ec==ETXTBSY) return "text busy";
#endif
	if (ec==error_txtbsy) return "text busy";
#ifdef EWOULDBLOCK
	if (ec==EWOULDBLOCK) return "input/output would block";
#endif
	if (ec==error_wouldblock) return "input/output would block";
#ifdef E2BIG
	if (ec==E2BIG) return "argument list too long";
#endif
#ifdef EADDRINUSE
	if (ec==EADDRINUSE) return "address already used";
#endif
#ifdef EADDRNOTAVAIL
	if (ec==EADDRNOTAVAIL) return "address not available";
#endif
#ifdef EADV
	if (ec==EADV) return "advertise error";
#endif
#ifdef EAFNOSUPPORT
	if (ec==EAFNOSUPPORT) return "address family not supported";
#endif
#ifdef EALREADY
	if (ec==EALREADY) return "operation already in progress";
#endif
#ifdef EAUTH
	if (ec==EAUTH) return "authentication error";
#endif
#ifdef EBADF
	if (ec==EBADF) return "file descriptor not open";
#endif
#ifdef EBADMSG
	if (ec==EBADMSG) return "bad message type";
#endif
#ifdef EBADRPC
	if (ec==EBADRPC) return "RPC structure is bad";
#endif
#ifdef EBUSY
	if (ec==EBUSY) return "device busy";
#endif
#ifdef ECHILD
	if (ec==ECHILD) return "no child processes";
#endif
#ifdef ECOMM
	if (ec==ECOMM) return "communication error";
#endif
#ifdef ECONNABORTED
	if (ec==ECONNABORTED) return "connection aborted";
#endif
#ifdef ECONNRESET
	if (ec==ECONNRESET) return "connection reset";
#endif
	if (ec==error_connreset) return "connection reset";
#ifdef EDEADLK
	if (ec==EDEADLK) return "operation would cause deadlock";
#endif
#ifdef EDESTADDRREQ
	if (ec==EDESTADDRREQ) return "destination address required";
#endif
#ifdef EDOM
	if (ec==EDOM) return "input out of range";
#endif
#ifdef EDQUOT
	if (ec==EDQUOT) return "disk quota exceeded";
#endif
#ifdef EFAULT
	if (ec==EFAULT) return "bad address";
#endif
#ifdef EFBIG
	if (ec==EFBIG) return "file too big";
#endif
#ifdef EFTYPE
	if (ec==EFTYPE) return "bad file type";
#endif
#ifdef EHOSTDOWN
	if (ec==EHOSTDOWN) return "host down";
#endif
#ifdef EHOSTUNREACH
	if (ec==EHOSTUNREACH) return "host unreachable";
#endif
#ifdef EIDRM
	if (ec==EIDRM) return "identifier removed";
#endif
#ifdef EINVAL
	if (ec==EINVAL) return "invalid argument";
#endif
#ifdef EISCONN
	if (ec==EISCONN) return "already connected";
#endif
#ifdef ELOOP
	if (ec==ELOOP) return "symbolic link loop";
#endif
#ifdef EMFILE
	if (ec==EMFILE) return "process cannot open more files";
#endif
#ifdef EMSGSIZE
	if (ec==EMSGSIZE) return "message too long";
#endif
#ifdef EMULTIHOP
	if (ec==EMULTIHOP) return "multihop attempted";
#endif
#ifdef ENAMETOOLONG
	if (ec==ENAMETOOLONG) return "file name too long";
#endif
#ifdef ENEEDAUTH
	if (ec==ENEEDAUTH) return "not authenticated";
#endif
#ifdef ENETDOWN
	if (ec==ENETDOWN) return "network down";
#endif
#ifdef ENETRESET
	if (ec==ENETRESET) return "network reset";
#endif
#ifdef ENETUNREACH
	if (ec==ENETUNREACH) return "network unreachable";
#endif
#ifdef ENFILE
	if (ec==ENFILE) return "system cannot open more files";
#endif
#ifdef ENOBUFS
	if (ec==ENOBUFS) return "out of buffer space";
#endif
#ifdef ENODEV
	if (ec==ENODEV) return "device does not support operation";
#endif
#ifdef ENOEXEC
	if (ec==ENOEXEC) return "exec format error";
#endif
#ifdef ENOLCK
	if (ec==ENOLCK) return "no locks available";
#endif
#ifdef ENOLINK
	if (ec==ENOLINK) return "link severed";
#endif
#ifdef ENOMSG
	if (ec==ENOMSG) return "no message of desired type";
#endif
#ifdef ENONET
	if (ec==ENONET) return "machine not on network";
#endif
#ifdef ENOPROTOOPT
	if (ec==ENOPROTOOPT) return "protocol not available";
#endif
#ifdef ENOSPC
	if (ec==ENOSPC) return "out of disk space";
#endif
#ifdef ENOSR
	if (ec==ENOSR) return "out of stream resources";
#endif
#ifdef ENOSTR
	if (ec==ENOSTR) return "not a stream device";
#endif
#ifdef ENOSYS
	if (ec==ENOSYS) return "system call not available";
#endif
#ifdef ENOTBLK
	if (ec==ENOTBLK) return "not a block device";
#endif
#ifdef ENOTCONN
	if (ec==ENOTCONN) return "not connected";
#endif
#ifdef ENOTDIR
	if (ec==ENOTDIR) return "not a directory";
#endif
#ifdef ENOTEMPTY
	if (ec==ENOTEMPTY) return "directory not empty";
#endif
#ifdef ENOTSOCK
	if (ec==ENOTSOCK) return "not a socket";
#endif
#ifdef ENOTTY
	if (ec==ENOTTY) return "not a tty";
#endif
#ifdef EOPNOTSUPP
	if (ec==EOPNOTSUPP) return "operation not supported";
#endif
#ifdef EPFNOSUPPORT
	if (ec==EPFNOSUPPORT) return "protocol family not supported";
#endif
#ifdef EPROCLIM
	if (ec==EPROCLIM) return "too many processes";
#endif
#ifdef EPROCUNAVAIL
	if (ec==EPROCUNAVAIL) return "bad procedure for program";
#endif
#ifdef EPROGMISMATCH
	if (ec==EPROGMISMATCH) return "program version mismatch";
#endif
#ifdef EPROGUNAVAIL
	if (ec==EPROGUNAVAIL) return "RPC program unavailable";
#endif
#ifdef EPROTONOSUPPORT
	if (ec==EPROTONOSUPPORT) return "protocol not supported";
#endif
#ifdef EPROTOTYPE
	if (ec==EPROTOTYPE) return "incorrect protocol type";
#endif
#ifdef ERANGE
	if (ec==ERANGE) return "output out of range";
#endif
#ifdef EREMCHG
	if (ec==EREMCHG) return "remote address changed";
#endif
#ifdef EREMOTE
	if (ec==EREMOTE) return "too many levels of remote in path";
#endif
#ifdef EROFS
	if (ec==EROFS) return "read-only file system";
#endif
#ifdef ERPCMISMATCH
	if (ec==ERPCMISMATCH) return "RPC version mismatch";
#endif
#ifdef ERREMOTE
	if (ec==ERREMOTE) return "object not local";
#endif
#ifdef ESHUTDOWN
	if (ec==ESHUTDOWN) return "socket shut down";
#endif
#ifdef ESOCKTNOSUPPORT
	if (ec==ESOCKTNOSUPPORT) return "socket type not supported";
#endif
#ifdef ESPIPE
	if (ec==ESPIPE) return "unseekable descriptor";
#endif
#ifdef ESRCH
	if (ec==ESRCH) return "no such process";
#endif
#ifdef ESRMNT
	if (ec==ESRMNT) return "srmount error";
#endif
#ifdef ESTALE
	if (ec==ESTALE) return "stale NFS file handle";
#endif
#ifdef ETIME
	if (ec==ETIME) return "timer expired";
#endif
#ifdef ETOOMANYREFS
	if (ec==ETOOMANYREFS) return "too many references";
#endif
#ifdef EUSERS
	if (ec==EUSERS) return "too many users";
#endif
#ifdef EXDEV
	if (ec==EXDEV) return "cross-device link";
#endif
return "unknown error";
}

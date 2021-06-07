#include <errno.h>
#include <stddef.h>
char* show_errno(int errno)
{
#ifdef EPERM
    if (errno == EPERM) return "EPERM";
#endif
#ifdef ENOENT
    if (errno == ENOENT) return "ENOENT";
#endif
#ifdef ESRCH
    if (errno == ESRCH) return "ESRCH";
#endif
#ifdef EINTR
    if (errno == EINTR) return "EINTR";
#endif
#ifdef EIO
    if (errno == EIO) return "EIO";
#endif
#ifdef ENXIO
    if (errno == ENXIO) return "ENXIO";
#endif
#ifdef E2BIG
    if (errno == E2BIG) return "E2BIG";
#endif
#ifdef ENOEXEC
    if (errno == ENOEXEC) return "ENOEXEC";
#endif
#ifdef EBADF
    if (errno == EBADF) return "EBADF";
#endif
#ifdef ECHILD
    if (errno == ECHILD) return "ECHILD";
#endif
#ifdef EAGAIN
    if (errno == EAGAIN) return "EAGAIN";
#endif
#ifdef ENOMEM
    if (errno == ENOMEM) return "ENOMEM";
#endif
#ifdef EACCES
    if (errno == EACCES) return "EACCES";
#endif
#ifdef EFAULT
    if (errno == EFAULT) return "EFAULT";
#endif
#ifdef ENOTBLK
    if (errno == ENOTBLK) return "ENOTBLK";
#endif
#ifdef EBUSY
    if (errno == EBUSY) return "EBUSY";
#endif
#ifdef EEXIST
    if (errno == EEXIST) return "EEXIST";
#endif
#ifdef EXDEV
    if (errno == EXDEV) return "EXDEV";
#endif
#ifdef ENODEV
    if (errno == ENODEV) return "ENODEV";
#endif
#ifdef ENOTDIR
    if (errno == ENOTDIR) return "ENOTDIR";
#endif
#ifdef EISDIR
    if (errno == EISDIR) return "EISDIR";
#endif
#ifdef EINVAL
    if (errno == EINVAL) return "EINVAL";
#endif
#ifdef ENFILE
    if (errno == ENFILE) return "ENFILE";
#endif
#ifdef EMFILE
    if (errno == EMFILE) return "EMFILE";
#endif
#ifdef ENOTTY
    if (errno == ENOTTY) return "ENOTTY";
#endif
#ifdef ETXTBSY
    if (errno == ETXTBSY) return "ETXTBSY";
#endif
#ifdef EFBIG
    if (errno == EFBIG) return "EFBIG";
#endif
#ifdef ENOSPC
    if (errno == ENOSPC) return "ENOSPC";
#endif
#ifdef ESPIPE
    if (errno == ESPIPE) return "ESPIPE";
#endif
#ifdef EROFS
    if (errno == EROFS) return "EROFS";
#endif
#ifdef EMLINK
    if (errno == EMLINK) return "EMLINK";
#endif
#ifdef EPIPE
    if (errno == EPIPE) return "EPIPE";
#endif
#ifdef EDOM
    if (errno == EDOM) return "EDOM";
#endif
#ifdef ERANGE
    if (errno == ERANGE) return "ERANGE";
#endif
    return NULL;
}
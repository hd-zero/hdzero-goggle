#pragma once
/* <sys/select.h> stub for the Windows (mingw) emulator build. The device select()
   paths that include it never have real fds in the emulator, so this is inert. */
#include <sys/time.h>
#ifndef FD_SETSIZE
#define FD_SETSIZE 64
#endif
typedef struct { int fd_count; int fd_array[FD_SETSIZE]; } fd_set;
#define FD_ZERO(s)      ((s)->fd_count = 0)
#define FD_SET(fd, s)   ((void)(fd), (void)(s))
#define FD_CLR(fd, s)   ((void)(fd), (void)(s))
#define FD_ISSET(fd, s) (((void)(fd), (void)(s)), 0)
static inline int select(int n, fd_set *r, fd_set *w, fd_set *e, struct timeval *t) {
    (void)n; (void)r; (void)w; (void)e; (void)t; return -1;
}

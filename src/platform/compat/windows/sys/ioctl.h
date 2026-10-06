#pragma once
/* <sys/ioctl.h> stub for the Windows (mingw) emulator build (device ioctls dormant). */
static inline int ioctl(int fd, unsigned long request, ...) { (void)fd; (void)request; return -1; }

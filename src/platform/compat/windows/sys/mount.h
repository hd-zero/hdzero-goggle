#pragma once
/* <sys/mount.h> stub for the Windows (mingw) emulator build (statfs/mount dormant). */
struct statfs { unsigned long f_bsize, f_blocks, f_bfree, f_bavail, f_files, f_ffree; };
static inline int statfs(const char *p, struct statfs *b) { (void)p; if (b) { b->f_bsize = 0; b->f_bavail = 0; } return -1; }
static inline int mount(const char *s, const char *t, const char *f, unsigned long fl, const void *d) { (void)s; (void)t; (void)f; (void)fl; (void)d; return -1; }
static inline int umount(const char *t) { (void)t; return -1; }

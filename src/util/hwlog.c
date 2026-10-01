#include "hwlog.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

void hwlog(const char *fmt, ...) {
    FILE *f = fopen("/mnt/extsd/hwlog.txt", "a");
    if (!f)
        return;

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    fprintf(f, "[%6ld.%03ld] ", (long)ts.tv_sec, ts.tv_nsec / 1000000L);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);

    fputc('\n', f);
    fflush(f);
    fsync(fileno(f));
    fclose(f);
}

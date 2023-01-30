#include "log/log.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define get_filename(file) (strrchr(file, '/') ? strrchr(file, '/') + 1 : file)

static FILE *log_file = NULL;

static const char *log_level_names[] = {
    "DEBUG",   // -2
    "VERBOSE", // -1
    "INFO",    // 0
    "WARN",    // 1
    "ERROR",   // 2
    "FATAL",   // 3
};

int log_printf(const char *file, const char *func, int line, const int level, const char *fmt, ...) {
    va_list args1;
    va_start(args1, fmt);
    va_list args2;
    va_copy(args2, args1);
    char buf[1 + vsnprintf(NULL, 0, fmt, args1)];
    va_end(args1);
    vsnprintf(buf, sizeof buf, fmt, args2);
    va_end(args2);

    int ret = printf(
        "[%s][%s:%s:%d] %s\r\n",
        log_level_names[level + 2],
        get_filename(file),
        func,
        line,
        buf);

    if (log_file) {
        fprintf(log_file, "[%s][%s:%s:%d] %s\r\n",
                log_level_names[level + 2],
                get_filename(file),
                func,
                line,
                buf);
        fflush(log_file);
    }

    return ret;
}

bool log_enable_file(const char *filename) {
    log_file = fopen(filename, "w+");
    if (log_file) {
        return true;
    }
    return false;
}
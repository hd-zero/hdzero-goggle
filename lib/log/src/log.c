#include "log/log.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define get_filename(file) (strrchr(file, '/') ? strrchr(file, '/') + 1 : file)

static FILE *log_file = NULL;

static atomic_bool log_mutex_init = ATOMIC_VAR_INIT(false);
static pthread_mutex_t log_mutex;

static const char *log_level_names[] = {
    "DEBUG",   // -2
    "VERBOSE", // -1
    "INFO",    // 0
    "WARN",    // 1
    "ERROR",   // 2
    "FATAL",   // 3
};

int log_printf(const char *file, const char *func, int line, const int level, const char *fmt, ...) {
    // probably slow to check this every time
    if (!log_mutex_init) {
        pthread_mutex_init(&log_mutex, NULL);
        log_mutex_init = true;
    }

    pthread_mutex_lock(&log_mutex);

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
    fflush(stdout);

    if (log_file) {
        fprintf(log_file, "[%s][%s:%s:%d] %s\r\n",
                log_level_names[level + 2],
                get_filename(file),
                func,
                line,
                buf);
        fflush(log_file);
    }

    pthread_mutex_unlock(&log_mutex);

    return ret;
}

bool log_file_opened() {
    return log_file == NULL ? false : true;
}

bool log_file_open(const char *filename) {
    log_file = fopen(filename, "w+");
    if (log_file) {
        return true;
    }
    return false;
}

void log_file_close() {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}
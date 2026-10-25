#pragma once
// Portable subset of the POSIX per-process interval-timer API (timer_create /
// timer_settime / timer_delete) for platforms that lack it (macOS, Windows).
// Only the SIGEV_THREAD periodic-timer behaviour used by the head tracker is
// implemented, backed by a pthread. On Linux the real <time.h>/librt API is used
// instead, so this shim compiles to nothing there.
#if !defined(__linux__)

#include <signal.h> // struct sigevent, union sigval, SIGEV_THREAD
#include <time.h>   // struct timespec, clockid_t

#ifdef __cplusplus
extern "C" {
#endif

// macOS lacks struct itimerspec; Windows (mingw + winpthread) already defines it.
#if !defined(_WIN32)
struct itimerspec {
    struct timespec it_interval;
    struct timespec it_value;
};
#endif

// Windows (mingw) lacks sigevent / SIGEV_THREAD; macOS's <signal.h> provides them.
#if defined(_WIN32)
#ifndef SIGEV_THREAD
#define SIGEV_THREAD 2
#endif
union sigval {
    int sival_int;
    void *sival_ptr;
};
struct sigevent {
    int sigev_notify;
    union sigval sigev_value;
    void (*sigev_notify_function)(union sigval);
    void *sigev_notify_attributes;
};
#endif

typedef struct pt_timer *timer_t;

int timer_create(clockid_t clockid, struct sigevent *sevp, timer_t *timerid);
int timer_settime(timer_t timerid, int flags,
                  const struct itimerspec *new_value,
                  struct itimerspec *old_value);
int timer_delete(timer_t timerid);

#ifdef __cplusplus
}
#endif

#endif // !__linux__

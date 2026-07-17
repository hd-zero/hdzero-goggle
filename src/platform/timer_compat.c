#include "platform/timer_compat.h"

#if !defined(__linux__)

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

struct pt_timer {
    pthread_t thread;
    struct itimerspec spec;
    void (*fn)(union sigval);
    union sigval val;
    volatile int running;
};

static void *pt_timer_thread(void *arg) {
    struct pt_timer *t = (struct pt_timer *)arg;

    struct timespec delay = t->spec.it_value;
    if (delay.tv_sec || delay.tv_nsec)
        nanosleep(&delay, NULL);
    if (t->running && t->fn)
        t->fn(t->val);

    for (;;) {
        struct timespec iv = t->spec.it_interval;
        if ((iv.tv_sec == 0 && iv.tv_nsec == 0) || !t->running)
            break; // one-shot, or stopped
        nanosleep(&iv, NULL);
        if (!t->running)
            break;
        if (t->fn)
            t->fn(t->val);
    }
    return NULL;
}

int timer_create(clockid_t clockid, struct sigevent *sevp, timer_t *timerid) {
    (void)clockid;
    if (!timerid) {
        errno = EINVAL;
        return -1;
    }
    struct pt_timer *t = (struct pt_timer *)calloc(1, sizeof(*t));
    if (!t) {
        errno = ENOMEM;
        return -1;
    }
    if (sevp) {
        t->fn = sevp->sigev_notify_function;
        t->val = sevp->sigev_value;
    }
    *timerid = t;
    return 0;
}

int timer_settime(timer_t timerid, int flags,
                  const struct itimerspec *new_value,
                  struct itimerspec *old_value) {
    (void)flags;
    if (!timerid || !new_value) {
        errno = EINVAL;
        return -1;
    }
    if (old_value) {
        old_value->it_interval.tv_sec = 0;
        old_value->it_interval.tv_nsec = 0;
        old_value->it_value.tv_sec = 0;
        old_value->it_value.tv_nsec = 0;
    }
    timerid->spec = *new_value;
    if (!timerid->running) {
        timerid->running = 1;
        if (pthread_create(&timerid->thread, NULL, pt_timer_thread, timerid) != 0) {
            timerid->running = 0;
            return -1;
        }
    }
    return 0;
}

int timer_delete(timer_t timerid) {
    if (!timerid) {
        errno = EINVAL;
        return -1;
    }
    timerid->running = 0;
    pthread_detach(timerid->thread);
    // Intentionally not freed: the worker thread may still be waking, so leaking
    // a tiny struct at teardown is safer than a use-after-free. ht.c never deletes.
    return 0;
}

#endif // !__linux__

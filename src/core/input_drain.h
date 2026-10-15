#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <sys/types.h>

// Per-source/event-loop visit budget for HDZGOGGLE non-blocking drain.
// 64 input_event records: caps a chattering encoder so it cannot monopolize
// the input thread, while a typical detent burst (~tens of REL_Y) still
// finishes in one visit. A larger 89-raw burst yields after 64; the fd stays
// EPOLLIN-ready and the outer loop revisits. poll_due still runs after each
// visit so a 10ms decision is not starved.
#define INPUT_DRAIN_BUDGET 64

typedef enum {
    INPUT_DRAIN_PROCESS = 0, // complete record; handle it and continue
    INPUT_DRAIN_EAGAIN,      // kernel queue empty; end drain normally
    INPUT_DRAIN_EINTR,       // interrupted; retry the same read
    INPUT_DRAIN_BUDGET_HIT, // processed == budget; yield to epoll
    INPUT_DRAIN_SHORT,       // incomplete record
    INPUT_DRAIN_EOF,         // nread == 0
    INPUT_DRAIN_ERROR,       // other read error
} input_drain_status_t;

// Pure classifier for one non-blocking read attempt.
// processed is the number of complete records already handled this visit.
// Check BUDGET before calling read, or pass processed >= budget here.
input_drain_status_t input_drain_status(ssize_t nread,
                                        int err,
                                        size_t record_size,
                                        unsigned processed,
                                        unsigned budget);

#ifdef __cplusplus
}
#endif

#include "input_drain.h"

#include <errno.h>

input_drain_status_t input_drain_status(ssize_t nread,
                                        int err,
                                        size_t record_size,
                                        unsigned processed,
                                        unsigned budget) {
    if (processed >= budget)
        return INPUT_DRAIN_BUDGET_HIT;

    if (nread < 0) {
        if (err == EINTR)
            return INPUT_DRAIN_EINTR;
        if (err == EAGAIN || err == EWOULDBLOCK)
            return INPUT_DRAIN_EAGAIN;
        return INPUT_DRAIN_ERROR;
    }

    if (nread == 0)
        return INPUT_DRAIN_EOF;

    if ((size_t)nread != record_size)
        return INPUT_DRAIN_SHORT;

    return INPUT_DRAIN_PROCESS;
}

// Deterministic tests for the HDZGOGGLE non-blocking drain classifier.
//
// Build:
//   gcc -std=gnu11 -Wall -Wextra -I../src/core input_drain_test.c
//       ../src/core/input_drain.c -o input_drain_test
//   ./input_drain_test

#include <errno.h>
#include <stdio.h>

#include "input_drain.h"

static int g_failures = 0;
static int g_checks = 0;

static void check_true(const char *name, int cond) {
    g_checks++;
    if (cond)
        printf("PASS: %s\n", name);
    else {
        printf("FAIL: %s\n", name);
        g_failures++;
    }
}

int main(void) {
    const size_t rec = 24;
    const unsigned budget = INPUT_DRAIN_BUDGET;

    check_true("budget_is_64", INPUT_DRAIN_BUDGET == 64);

    check_true("process_complete_record",
               input_drain_status((ssize_t)rec, 0, rec, 0, budget) == INPUT_DRAIN_PROCESS);

    check_true("eagain_ends_drain",
               input_drain_status(-1, EAGAIN, rec, 3, budget) == INPUT_DRAIN_EAGAIN);
    check_true("ewouldblock_ends_drain",
               input_drain_status(-1, EWOULDBLOCK, rec, 3, budget) == INPUT_DRAIN_EAGAIN);

    check_true("eintr_retries",
               input_drain_status(-1, EINTR, rec, 7, budget) == INPUT_DRAIN_EINTR);

    check_true("short_read",
               input_drain_status(8, 0, rec, 0, budget) == INPUT_DRAIN_SHORT);

    check_true("eof",
               input_drain_status(0, 0, rec, 1, budget) == INPUT_DRAIN_EOF);

    check_true("real_error",
               input_drain_status(-1, EIO, rec, 0, budget) == INPUT_DRAIN_ERROR);

    check_true("budget_before_read",
               input_drain_status(-1, EAGAIN, rec, budget, budget) == INPUT_DRAIN_BUDGET_HIT);
    check_true("budget_at_limit_even_if_processable",
               input_drain_status((ssize_t)rec, 0, rec, budget, budget) == INPUT_DRAIN_BUDGET_HIT);

    check_true("just_under_budget_still_process",
               input_drain_status((ssize_t)rec, 0, rec, budget - 1, budget) == INPUT_DRAIN_PROCESS);

    printf("\n%d/%d checks passed\n", g_checks - g_failures, g_checks);
    return g_failures == 0 ? 0 : 1;
}

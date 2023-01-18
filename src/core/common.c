#include "common.hh"

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "defines.h"
#include "self_test.h"
#include "ui/page_common.h"

///////////////////////////////////////////////////////////////////////////////
// globals
pthread_mutex_t lvgl_mutex;
atomic_int g_key = 0;
atomic_int g_init_done = 0;
///////////////////////////////////////////////////////////////////////////////

uint8_t slow_key(left_dial_t key, uint8_t *state, uint8_t *cnt) {
    if ((key == LEFT_DAIL_CLICK) || (key == LEFT_DAIL_LONGPRESS)) {
        *state = 0;
        *cnt = 0;
        return key;
    }

    switch (*state) {
    case 0:
        *state = key + 10;
        *cnt = 1;
        break;

    case 11:
        if (key == LEFT_DAIL_UP) {
            if (*cnt == DIAL_SENSITIVITY) {
                *state = 0;
                return LEFT_DAIL_UP;
            } else
                (*cnt)++;
        } else { // key= 2
            *state = 12;
        }
        break;

    case 12:
        if (key == LEFT_DAIL_DOWN) {
            if (*cnt == DIAL_SENSITIVITY) {
                *state = 0;
                return LEFT_DAIL_DOWN;
            } else
                (*cnt)++;
        } else { // key =1
            *state = 11;
        }
        break;
    }
    return 0;
}

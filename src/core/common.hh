#ifndef __COMMON_HH__
#define __COMMON_HH__

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <lvgl/lvgl.h>

#include "defines.h"

#define DIAL_KEY_UP     1
#define DIAL_KEY_DOWN   2
#define DIAL_KEY_CLICK  3
#define DIAL_KEY_PRESS  4
#define RIGHT_KEY_CLICK 5
#define RIGHT_KEY_PRESS 6

extern atomic_int g_key;
extern atomic_int g_init_done;
extern pthread_mutex_t lvgl_mutex;

uint8_t slow_key(left_dial_t key, uint8_t *state, uint8_t *cnt);

#endif //__COMMON_HH__

#ifndef _INPUT_DEVICE_H
#define _INPUT_DEVICE_H

#include <stdbool.h>

typedef enum {
    RIGHT_CLICK = 0,
    RIGHT_LONG_PRESS = 1,
    RIGHT_DOUBLE_CLICK = 2
} right_button_t;

void input_device_init();
void tune_channel_timer();
void exit_tune_channel();
void rbtn_click(right_button_t click_type);

#endif

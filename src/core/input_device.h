#ifndef _INPUT_DEVICE_H
#define _INPUT_DEVICE_H

#include <stdbool.h>

void input_device_init();
void tune_channel_timer();
void exit_tune_channel();
void rbtn_click(bool is_short);

#endif

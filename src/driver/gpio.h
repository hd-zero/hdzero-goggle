#pragma once

#include <stdbool.h>

#define beep() beep_dur(500)

void gpio_init();
void gpio_open(int port_num);
void gpio_set(int port_num, bool val);

void beep_dur(int us);

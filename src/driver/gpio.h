#pragma once

#include <stdbool.h>

#define BEEP_SHORT 50
#define BEEP_LONG 200

#define beep() beep_dur(BEEP_SHORT)

void gpio_init();
void gpio_open(int port_num);
void gpio_set(int port_num, bool val);

void beep_dur(int ms);

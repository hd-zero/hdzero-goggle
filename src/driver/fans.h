#ifndef _FANS_H
#define _FANS_H

#include <stdint.h>

void fans_top_setspeed(uint8_t speed);
void fans_left_setspeed(uint8_t speed);
void fans_right_setspeed(uint8_t speed);

extern uint8_t fan_speeds[3]; //0=right,1=left,2=top

#endif

#ifndef _PORTING_H
#define _PORTING_H

#include <stdbool.h>

int lvgl_init_porting();
int lvgl_switch_to_720p();
int lvgl_switch_to_1080p();
void lvgl_screen_orbit(bool enable);

#endif

#ifndef _PAGE_POWER_H
#define _PAGE_POWER_H

#include <stdbool.h>

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

extern page_pack_t pp_power;

void set_voltage(int val);
void set_warning_type(int type);
void power_voltage_inc(void);
void power_voltage_dec(void);
void set_battery_S();

#endif

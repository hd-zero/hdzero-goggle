#ifndef _PAGE_POWER_H
#define _PAGE_POWER_H

#include <stdbool.h>

#include <lvgl/lvgl.h>

#include "page_common.h"

lv_obj_t *page_power_create(lv_obj_t *parent, panel_arr_t *arr);

void set_voltage(int val);
void set_warning_type(int type);
void power_set_toggle(int sel);
void power_voltage_inc(void);
void power_voltage_dec(void);
void set_battery_S();
#endif

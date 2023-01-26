#ifndef _PAGE_POWER_H
#define _PAGE_POWER_H

#define S_MIN_COUNT 2
#define S_MAX_COUNT 5
#define CELL_VOLTAGE_MIN 28
#define CELL_VOLTAGE_MAX 42

#define ROW_BATT_C_LABEL 0
#define ROW_S_COUNT_MODE 1
#define ROW_S_COUNT 2
#define ROW_CELL_VOLTAGE 3
#define ROW_WARN_TYPE 4
#define ROW_BACK 5

#include <stdbool.h>

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

extern page_pack_t pp_power;

void power_s_count_inc(void);
void power_s_count_dec(void);
void power_voltage_inc(void);
void power_voltage_dec(void);
void set_battery_S();

#endif

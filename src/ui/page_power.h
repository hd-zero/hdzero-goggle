#ifndef _PAGE_POWER_H
#define _PAGE_POWER_H

#define CELL_VOLTAGE_MIN 28
#define CELL_VOLTAGE_MAX 42

#define ROW_BATT_C_LABEL 0
#define ROW_CELL_COUNT_MODE 1
#define ROW_CELL_COUNT 2
#define ROW_CELL_VOLTAGE 3
#define ROW_OSD_DISPLAY_MODE 4
#define ROW_WARN_TYPE 5
#define ROW_BACK 6

#include <stdbool.h>

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

extern page_pack_t pp_power;

void power_cell_count_inc(void);
void power_cell_count_dec(void);
void power_voltage_inc(void);
void power_voltage_dec(void);

#endif

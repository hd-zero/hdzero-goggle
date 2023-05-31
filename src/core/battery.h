#pragma once

#include <stdbool.h>
#include <stdint.h>

#define CELL_MIN_COUNT 2
#define CELL_MAX_COUNT 6

typedef struct {
    int type; // cell count
    int voltage;
} sys_battery_t;

extern sys_battery_t g_battery;

void battery_init();
void battery_update();

bool battery_is_low();
int battery_get_millivolts(bool per_cell);
void battery_get_voltage_str(char* buf);
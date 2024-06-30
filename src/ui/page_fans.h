#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define MIN_FAN_TOP  1
#define MAX_FAN_TOP  5
#define MIN_FAN_SIDE 2
#define MAX_FAN_SIDE 9

#define FAN_TEMPERATURE_THR_H 750 // 75C
#define FAN_TEMPERATURE_THR_L 650 // 65C
#define RESPEED_WAIT_TIME     50  // 200 = 30s

#define SIDE_TEMPERATURE_RISKH 850 // 85C Risk high
#define TOP_TEMPERATURE_RISKH  500 //
#define TOP_TEMPERATURE_NORM   400 //

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

extern page_pack_t pp_fans;

void step_topfan();
void fans_auto_ctrl();
void change_topfan(uint8_t key);

#ifdef __cplusplus
}
#endif

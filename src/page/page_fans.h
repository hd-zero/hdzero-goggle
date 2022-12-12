#ifndef _PAGE_FANS_H
#define _PAGE_FANS_H

#define FAN_TEMPERATURE_THR_H  750  //75C
#define FAN_TEMPERATURE_THR_L  650  //65C
#define RESPEED_WAIT_TIME      200  //200 = 30s 

#define SIDE_TEMPERATURE_RISKH 850  //85C Risk high
#define TOP_TEMPERATURE_RISKH  500  //
#define TOP_TEMPERATURE_NORM   400  //

#include "lvgl/lvgl.h"
#include "page_common.h"

lv_obj_t *page_fans_create(lv_obj_t *parent, struct panel_arr *arr);

void fans_mode_toggle(int sel);
void fans_speed_dec(void);
void fans_speed_inc(void);
void step_topfan();
void fans_auto_ctrl();

#endif

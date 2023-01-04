#ifndef _STYLE_H
#define _STYLE_H
#include "lvgl/lvgl.h"

extern lv_style_t style_rootmenu;
extern lv_style_t style_submenu;
extern lv_style_t style_statusbar;
extern lv_style_t style_subpage;
extern lv_style_t style_context;
extern lv_style_t style_silder_main;
extern lv_style_t style_silder_select;
extern lv_style_t style_silder_indicator;
extern lv_style_t style_silder_knob;
extern lv_style_t style_silder_pressed_color;
extern lv_style_t style_scan;
extern lv_style_t style_select;
extern lv_style_t style_pb,style_pb_dark;

int style_init(void);
#endif

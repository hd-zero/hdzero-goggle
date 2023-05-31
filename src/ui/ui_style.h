#ifndef _STYLE_H
#define _STYLE_H
#include "lvgl/lvgl.h"

enum KB_STYLE {
    KB_STYLE_MAIN = 0,
    KB_STYLE_KEY,
    KB_STYLE_TEXT,
    KB_STYLE_CURSOR,

    KB_STYLE_COUNT
};

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
extern lv_style_t style_dropdown;
extern lv_style_t style_keyboard[KB_STYLE_COUNT];
extern lv_style_t style_pb, style_pb_dark;

int style_init(void);
#endif

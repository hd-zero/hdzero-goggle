#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include <stdbool.h>
#include <stdint.h>

#include <lvgl/lvgl.h>

#include "ui/page_common.h"

#define MAIN_MENU_ITEMS 11

typedef struct {
    lv_obj_t *page;
    lv_obj_t *icon;

    panel_arr_t p_arr;
} page_pack_t;

typedef struct {
    lv_obj_t *bar;
    int start;
    int val;
} progress_bar_t;

extern progress_bar_t progress_bar;

void main_menu_init();
void main_menu_show_toggle(void);
void main_menu_show(bool is_show);
bool main_menu_isshow(void);

void menu_nav(uint8_t key);

void submenu_nav(uint8_t key);
void submenu_exit();
void submenu_enter();
void submenu_fun(void);
void progress_bar_update();

void autoscan_exit(void);

#endif

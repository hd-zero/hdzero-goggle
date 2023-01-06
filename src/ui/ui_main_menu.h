#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include <stdbool.h>
#include <stdint.h>

#include <lvgl/lvgl.h>

#include "ui/page_common.h"

typedef enum {
    PAGE_AUTO_SCAN,
    PAGE_CONNECTIONS,
    PAGE_FANS,
    PAGE_HEADTRACKER,
    PAGE_IMAGE_SETTINGS,
    PAGE_PLAYBACK,
    PAGE_POWER,
    PAGE_RECORD,
    PAGE_SCAN_NOW,
    PAGE_SOURCE,
    PAGE_VERSION,

    PAGE_MAX,
} pages_t;

typedef struct {
    panel_arr_t p_arr;

    lv_obj_t *page;
    lv_obj_t *icon;

    void (*enter)();
    void (*exit)();
    void (*on_roller)(uint8_t key);
    void (*on_click)(uint8_t key, int sel);
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

#endif

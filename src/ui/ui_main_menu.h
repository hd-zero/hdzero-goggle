#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <lvgl/lvgl.h>

#include "ui/page_common.h"

typedef struct {
    char *name;
    panel_arr_t p_arr;

    lv_obj_t *page;
    lv_obj_t *icon;
    lv_obj_t *label;

    lv_obj_t *(*create)(lv_obj_t *parent, panel_arr_t *arr);
    void (*enter)();
    void (*exit)();
    void (*on_created)();
    void (*on_update)(uint32_t delta_ms);
    void (*on_roller)(uint8_t key);
    void (*on_click)(uint8_t key, int sel);
    void (*on_right_button)(bool is_short);

    int32_t post_bootup_run_priority;
    void (*post_bootup_run_function)(void(*complete_callback)());
} page_pack_t;

typedef struct {
    lv_obj_t *bar;
    int start;
    int val;
} progress_bar_t;

extern progress_bar_t progress_bar;

void main_menu_init();
void main_menu_update();
void main_menu_show(bool is_show);
bool main_menu_is_shown(void);

void menu_nav(uint8_t key);

void submenu_enter();
void submenu_exit();
void submenu_roller(uint8_t key);
void submenu_roller_no_selection_change(uint8_t key);
void submenu_click(void);
void submenu_right_button(bool is_short);
void progress_bar_update();

#ifdef __cplusplus
}
#endif

#include "page_sleep.h"

#include "driver/fans.h"
#include "page_fans.h"
#include "sleep_mode.h"
#include <log/log.h>
#include <stdio.h>

lv_obj_t *page_sleep_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Go Sleep:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_menu_cont_create(section);
    lv_obj_t *desc_label = lv_label_create(cont);
    lv_label_set_text(desc_label, "Click the Enter Button to go sleep.\n"
                                  "Click any button to exit sleep mode.");
    lv_obj_set_style_text_font(desc_label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_color(desc_label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(desc_label, 12, 0);
    lv_label_set_long_mode(desc_label, LV_LABEL_LONG_WRAP);

    return page;
}

int fans_auto_mode_save;
static fan_speed_t fan_speed_save;

static void page_sleep_enter() {
    LOGI("page_sleep_enter");
    go_sleep();
}

page_pack_t pp_sleep = {
    .create = page_sleep_create,
    .enter = page_sleep_enter,
    .exit = NULL,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = NULL,
    .on_click = NULL,
    .on_right_button = NULL,
    .name = "Go Sleep!",
};

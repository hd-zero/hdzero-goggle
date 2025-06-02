
#include <log/log.h>
#include <stdio.h>

#include "../conf/ui.h"

#include "driver/fans.h"
#include "lang/language.h"
#include "page_fans.h"
#include "page_sleep.h"
#include "sleep_mode.h"

lv_obj_t *page_sleep_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[128];
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, UI_PAGE_VIEW_SIZE);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, UI_PAGE_VIEW_SIZE);

    snprintf(buf, sizeof(buf), "%s:", _lang("Go Sleep"));
    create_text(NULL, section, false, buf, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_menu_cont_create(section);
    lv_obj_t *desc_label = lv_label_create(cont);
    snprintf(buf, sizeof(buf), "%s.\n%s.",
             _lang("Click the Enter Button to go sleep"),
             _lang("Click any button to exit sleep mode"));
    lv_label_set_text(desc_label, buf);
    lv_obj_set_style_text_font(desc_label, UI_PAGE_TEXT_FONT, 0);
    lv_obj_set_style_text_color(desc_label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(desc_label, UI_PAGE_TEXT_PAD, 0);
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

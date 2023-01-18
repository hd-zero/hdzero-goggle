#include "page_focus_chart.h"

#include <stdio.h>

static lv_obj_t *focus_chart_img;

lv_obj_t *page_focus_chart_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Focus Chart:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_menu_cont_create(section);
    lv_obj_t *desc_label = lv_label_create(cont);
    lv_label_set_text(desc_label, "Click the Enter Button to display the Back Focusing Chart.\nClick the Enter Button again to exit.");
    lv_obj_set_style_text_font(desc_label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_color(desc_label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(desc_label, 12, 0);
    lv_label_set_long_mode(desc_label, LV_LABEL_LONG_WRAP);

    focus_chart_img = lv_img_create(lv_scr_act());
    lv_obj_add_flag(focus_chart_img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(focus_chart_img, LV_OBJ_FLAG_FLOATING);
    lv_obj_clear_flag(focus_chart_img, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(focus_chart_img, 0, 0);
    lv_obj_set_size(focus_chart_img, 1920, 1080);

    char filename[128];
    sprintf(filename, "%s%s", RESOURCE_PATH, FOCUS_CHART_IMG);
    lv_img_set_src(focus_chart_img, filename);

    return page;
}

static void page_focus_chart_exit() {
    lv_obj_add_flag(focus_chart_img, LV_OBJ_FLAG_HIDDEN);
}

static void page_focus_chart_enter() {
    lv_obj_move_foreground(focus_chart_img);
    lv_obj_clear_flag(focus_chart_img, LV_OBJ_FLAG_HIDDEN);
}

static void page_focus_chart_on_click(uint8_t key, int sel) {
    lv_obj_add_flag(focus_chart_img, LV_OBJ_FLAG_HIDDEN);
    submenu_exit();
}

page_pack_t pp_focus_chart = {
    .create = page_focus_chart_create,
    .enter = page_focus_chart_enter,
    .exit = page_focus_chart_exit,
    .on_roller = NULL,
    .on_click = page_focus_chart_on_click,
};
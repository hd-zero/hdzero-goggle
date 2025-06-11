#include "page_focus_chart.h"

#include <stdio.h>

#include "../conf/ui.h"

#include "core/osd.h"
#include "lang/language.h"
#include "ui/ui_porting.h"

static lv_obj_t *focus_chart_img;

lv_obj_t *page_focus_chart_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[128];
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, UI_PAGE_VIEW_SIZE);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, UI_PAGE_VIEW_SIZE);

    snprintf(buf, sizeof(buf), "%s:", _lang("Focus Chart"));
    create_text(NULL, section, false, buf, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_menu_cont_create(section);
    lv_obj_t *desc_label = lv_label_create(cont);
    snprintf(buf, sizeof(buf), "%s.\n%s.", _lang("Click the Enter Button to display the Back Focusing Chart"), _lang("Click the Enter Button again to exit"));
    lv_label_set_text(desc_label, buf);
    lv_obj_set_style_text_font(desc_label, UI_PAGE_TEXT_FONT, 0);
    lv_obj_set_style_text_color(desc_label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(desc_label, UI_PAGE_TEXT_PAD, 0);
    lv_label_set_long_mode(desc_label, LV_LABEL_LONG_WRAP);

    focus_chart_img = lv_img_create(lv_scr_act());
    lv_obj_add_flag(focus_chart_img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(focus_chart_img, LV_OBJ_FLAG_FLOATING);
    lv_obj_clear_flag(focus_chart_img, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(focus_chart_img, 0, 0);
    lv_obj_set_size(focus_chart_img, DRAW_HOR_RES_FHD, DRAW_VER_RES_FHD);

    char filename[128];
    if (TARGET_GOGGLE == getTargetType()) {
        osd_resource_path(filename, "%s", OSD_RESOURCE_1080, FOCUS_CHART_IMG);
    } else if (TARGET_BOXPRO == getTargetType()) {
        osd_resource_path(filename, "%s", OSD_RESOURCE_720, FOCUS_CHART_IMG);
    }
    lv_img_set_src(focus_chart_img, filename);

    return page;
}

static void page_focus_chart_exit() {
    lv_obj_add_flag(focus_chart_img, LV_OBJ_FLAG_HIDDEN);
}

static void page_focus_chart_enter() {
    lv_obj_move_foreground(focus_chart_img);
    lv_obj_clear_flag(focus_chart_img, LV_OBJ_FLAG_HIDDEN);
    lvgl_screen_orbit(false);
}

static void page_focus_chart_on_click(uint8_t key, int sel) {
    lv_obj_add_flag(focus_chart_img, LV_OBJ_FLAG_HIDDEN);
    lvgl_screen_orbit(g_setting.osd.orbit > 0);
    submenu_exit();
}

page_pack_t pp_focus_chart = {
    .name = "Focus Chart",
    .create = page_focus_chart_create,
    .enter = page_focus_chart_enter,
    .exit = page_focus_chart_exit,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = NULL,
    .on_click = page_focus_chart_on_click,
    .on_right_button = NULL,
};
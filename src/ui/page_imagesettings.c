#include "page_imagesettings.h"

#include <stdio.h>
#include <stdlib.h>

#include "../conf/ui.h"

#include "core/app_state.h"
#include "core/common.hh"
#include "driver/hardware.h"
#include "driver/screen.h"
#include "lang/language.h"
#include "ui/page_common.h"
#include "ui/page_scannow.h"
#include "ui/page_source.h"
#include "ui/ui_image_setting.h"
#include "ui/ui_main_menu.h"
#include "ui/ui_style.h"

static lv_coord_t col_dsc[] = {UI_IMAGESETTING_COLS};
static lv_coord_t row_dsc[] = {UI_IMAGESETTING_ROWS};

static slider_group_t slider_group;
static slider_group_t slider_group1;
static slider_group_t slider_group2;
static slider_group_t slider_group3;
static slider_group_t slider_group4;
static slider_group_t slider_group5;

static lv_obj_t *page_imagesettings_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[288];
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, UI_PAGE_VIEW_SIZE);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, UI_PAGE_VIEW_SIZE);

    snprintf(buf, sizeof(buf), "%s:", _lang("Image Setting"));
    create_text(NULL, section, false, buf, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, UI_PAGE_VIEW_SIZE);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    create_slider_item(&slider_group, cont, "Panel", 12, g_setting.image.oled, 0);
    create_slider_item(&slider_group1, cont, _lang("Brightness"), 78, g_setting.image.brightness, 1);
    create_slider_item(&slider_group2, cont, _lang("Saturation"), 47, g_setting.image.saturation, 2);
    create_slider_item(&slider_group3, cont, _lang("Contrast"), 47, g_setting.image.contrast, 3);
    snprintf(buf, sizeof(buf), "Panel %s", _lang("Auto Off"));
    create_slider_item(&slider_group4, cont, buf, 3, g_setting.image.auto_off, 4);

    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    create_label_item(cont, buf, 1, 5, 1);

    lv_obj_t *label2 = lv_label_create(cont);
    snprintf(buf, sizeof(buf), "%s.\n %s.",
             _lang("To change image settings, click the Enter button to enter video mode"),
             _lang("Make sure a HDZero VTX or analog VTX is powered on for live video"));
    lv_label_set_text(label2, buf);
    lv_obj_set_style_text_font(label2, UI_PAGE_LABEL_FONT, 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label2, lv_color_hex(TEXT_COLOR_DEFAULT), 0);
    lv_obj_set_style_pad_top(label2, UI_PAGE_TEXT_PAD, 0);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_START, 1, 4,
                         LV_GRID_ALIGN_START, 6, 2);

    set_slider_value();

    return page;
}

void set_slider_value() {
    char buf[32];
    //	LOGI("set_slider_value %d %d %d %d.",g_setting.image.oled,g_setting.image.brightness,
    //											 g_setting.image.saturation,g_setting.image.contrast);

    snprintf(buf, sizeof(buf), "%d", g_setting.image.oled);
    lv_label_set_text(slider_group.label, buf);
    lv_slider_set_value(slider_group.slider, g_setting.image.oled, LV_ANIM_OFF);

    snprintf(buf, sizeof(buf), "%d", g_setting.image.brightness);
    lv_label_set_text(slider_group1.label, buf);
    lv_slider_set_value(slider_group1.slider, g_setting.image.brightness, LV_ANIM_OFF);

    snprintf(buf, sizeof(buf), "%d", g_setting.image.saturation);
    lv_label_set_text(slider_group2.label, buf);
    lv_slider_set_value(slider_group2.slider, g_setting.image.saturation, LV_ANIM_OFF);

    snprintf(buf, sizeof(buf), "%d", g_setting.image.contrast);
    lv_label_set_text(slider_group3.label, buf);
    lv_slider_set_value(slider_group3.slider, g_setting.image.contrast, LV_ANIM_OFF);

    if (g_setting.image.auto_off == 4)
        strcpy(buf, _lang("Never"));
    else
        snprintf(buf, sizeof(buf), "%d %s", g_setting.image.auto_off * 2 + 1, _lang("min"));
    lv_label_set_text(slider_group4.label, buf);
    lv_slider_set_value(slider_group4.slider, g_setting.image.auto_off, LV_ANIM_OFF);
}

static void page_imagesettings_enter() {
    app_state_push(APP_STATE_IMS);
    if (SOURCE_HDZERO == g_source_info.source) {
        progress_bar.start = 1;
        HDZero_open(g_setting.source.hdzero_bw);
        app_switch_to_hdzero(true);
        g_bShowIMS = true;
    } else if (SOURCE_HDMI_IN == g_source_info.source) {
        app_state_push(APP_STATE_SUBMENU);
        g_bShowIMS = false;
    } else {
        app_switch_to_analog(g_source_info.source == SOURCE_AV_IN);
        g_bShowIMS = true;
    }
}

page_pack_t pp_imagesettings = {
    .p_arr = {
        .cur = 0,
        .max = 6,
    },
    .name = "Image Setting",
    .create = page_imagesettings_create,
    .enter = page_imagesettings_enter,
    .exit = NULL,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = NULL,
    .on_click = NULL,
    .on_right_button = NULL,
};

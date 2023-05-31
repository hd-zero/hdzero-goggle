#include "page_imagesettings.h"

#include <stdio.h>
#include <stdlib.h>

#include "core/app_state.h"
#include "core/common.hh"
#include "core/settings.h"
#include "driver/hardware.h"
#include "driver/oled.h"
#include "ui/page_common.h"
#include "ui/page_scannow.h"
#include "ui/page_source.h"
#include "ui/ui_image_setting.h"
#include "ui/ui_main_menu.h"
#include "ui/ui_style.h"

static lv_coord_t col_dsc[] = {160, 160, 160, 160, 140, 220, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

static slider_group_t slider_group;
static slider_group_t slider_group1;
static slider_group_t slider_group2;
static slider_group_t slider_group3;
static slider_group_t slider_group4;
static slider_group_t slider_group5;

static lv_obj_t *page_imagesettings_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Image Settings:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    create_slider_item(&slider_group, cont, "OLED", 12, g_setting.image.oled, 0);
    create_slider_item(&slider_group1, cont, "Brightness", 78, g_setting.image.brightness, 1);
    create_slider_item(&slider_group2, cont, "Saturation", 47, g_setting.image.saturation, 2);
    create_slider_item(&slider_group3, cont, "Contrast", 47, g_setting.image.contrast, 3);
    create_slider_item(&slider_group4, cont, "OLED Auto off", 3, g_setting.image.auto_off, 4);

    create_label_item(cont, "< Back", 1, 5, 1);

    lv_obj_t *label2 = lv_label_create(cont);
    lv_label_set_text(label2, "To change image settings, click the Enter button to enter video mode. \nMake sure a HDZero VTX or analog VTX is powered on for live video.");
    lv_obj_set_style_text_font(label2, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label2, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(label2, 12, 0);
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

    sprintf(buf, "%d", g_setting.image.oled);
    lv_label_set_text(slider_group.label, buf);
    lv_slider_set_value(slider_group.slider, g_setting.image.oled, LV_ANIM_OFF);

    sprintf(buf, "%d", g_setting.image.brightness);
    lv_label_set_text(slider_group1.label, buf);
    lv_slider_set_value(slider_group1.slider, g_setting.image.brightness, LV_ANIM_OFF);

    sprintf(buf, "%d", g_setting.image.saturation);
    lv_label_set_text(slider_group2.label, buf);
    lv_slider_set_value(slider_group2.slider, g_setting.image.saturation, LV_ANIM_OFF);

    sprintf(buf, "%d", g_setting.image.contrast);
    lv_label_set_text(slider_group3.label, buf);
    lv_slider_set_value(slider_group3.slider, g_setting.image.contrast, LV_ANIM_OFF);

    if (g_setting.image.auto_off == 4)
        strcpy(buf, "Never");
    else
        sprintf(buf, "%d min", g_setting.image.auto_off * 2 + 1);
    lv_label_set_text(slider_group4.label, buf);
    lv_slider_set_value(slider_group4.slider, g_setting.image.auto_off, LV_ANIM_OFF);
}

static void page_imagesettings_enter() {
    app_state_push(APP_STATE_IMS);
    switch (g_source_info.source) {
    case SOURCE_HDZERO:
        progress_bar.start = 1;
        HDZero_open(g_hw_stat.hdz_bw);
        app_switch_to_hdzero(true);
        g_bShowIMS = true;
        break;

    case SOURCE_HDMI_IN: // no image setting support for HDMI in
        app_state_push(APP_STATE_SUBMENU);
        g_bShowIMS = false;
        break;

    case SOURCE_AV_IN:
        app_switch_to_analog(0);
        g_bShowIMS = true;
        break;

    case SOURCE_EXPANSION:
        app_switch_to_analog(1);
        g_bShowIMS = true;
        break;
    }
}

page_pack_t pp_imagesettings = {
    .p_arr = {
        .cur = 0,
        .max = 6,
    },
    .name = "Image Settings",
    .create = page_imagesettings_create,
    .enter = page_imagesettings_enter,
    .exit = NULL,
    .on_roller = NULL,
    .on_click = NULL,
    .on_right_button = NULL,
};

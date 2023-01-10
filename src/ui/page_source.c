#include "page_source.h"

#include <stdio.h>
#include <unistd.h>

#include <minIni.h>

#include "../core/common.hh"
#include "../core/osd.h"
#include "../driver/hardware.h"
#include "../driver/it66121.h"
#include "../driver/oled.h"
#include "page_common.h"
#include "page_scannow.h"
#include "ui/ui_main_menu.h"
#include "ui/ui_porting.h"
#include "ui/ui_style.h"

// local
static lv_coord_t col_dsc[] = {160, 160, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

static lv_obj_t *label[5];
static uint8_t oled_tst_mode = 0; // 0=Normal,1=CB; 2-Grid; 3=All Black; 4=All White,5=Boot logo
static bool in_sourcepage = false;

static lv_obj_t *page_source_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Sources:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    label[0] = create_label_item(cont, "HDZero", 1, 0, 3);
    label[1] = create_label_item(cont, "HDMI In", 1, 1, 3);
    label[2] = create_label_item(cont, "AV In", 1, 2, 3);
    label[3] = create_label_item(cont, "Expansion Module", 1, 3, 3);
    if (g_test_en) {
        pp_source.p_arr.max = 6;
        label[4] = create_label_item(cont, "OLED Pattern: Normal", 1, 4, 3);
        create_label_item(cont, "< Back", 1, 5, 3);
    } else {
        pp_source.p_arr.max = 5;
        label[4] = NULL;
        create_label_item(cont, "< Back", 1, 4, 3);
    }

    return page;
}

char *state2string(uint8_t status) {
    return status ? "#00FF00 Detected#" : "#C0C0C0 Disconnected";
}

void source_status_timer() {
    char buf[64];

    if (!in_sourcepage)
        return;

    if (g_setting.scan.channel > 8)
        sprintf(buf, "HDZero: F%d", (g_setting.scan.channel - 8) * 2);
    else
        sprintf(buf, "HDZero: R%d", g_setting.scan.channel);
    lv_label_set_text(label[0], buf);

    sprintf(buf, "HDMI In: %s", state2string(g_source_info.hdmi_in_status));
    lv_label_set_text(label[1], buf);

    sprintf(buf, "AV In: %s", state2string(g_source_info.av_in_status));
    lv_label_set_text(label[2], buf);

    sprintf(buf, "Expansion Module: %s", state2string(g_source_info.av_bay_status));
    lv_label_set_text(label[3], buf);

    if (g_test_en && label[3]) {
        uint8_t oled_tm = oled_tst_mode & 0x0F;
        char *pattern_label[6] = {"Normal", "Color Bar", "Grid", "All Black", "All White", "Boot logo"};
        char str[32];
        sprintf(str, "OLED Pattern: %s", pattern_label[oled_tm]);
        lv_label_set_text(label[4], str);
    }
}

static void page_source_on_click(uint8_t key, int sel) {
    switch (sel) {
    case 0:
        progress_bar.start = 1;
        HDZero_open();
        switch_to_video(true);
        g_menu_op = OPLEVEL_VIDEO;
        g_source_info.source = SOURCE_HDZERO;
        sel_audio_source(2);
        enable_line_out(true);
        break;

    case 1:
        if (g_source_info.hdmi_in_status)
            switch_to_hdmiin();
        break;

    case 2: // AV in
        switch_to_analog(0);
        g_menu_op = OPLEVEL_VIDEO;
        g_source_info.source = SOURCE_AV_IN;
        sel_audio_source(2);
        enable_line_out(true);
        break;

    case 3: // Module in
        switch_to_analog(1);
        g_menu_op = OPLEVEL_VIDEO;
        g_source_info.source = SOURCE_EXPANSION;
        sel_audio_source(2);
        enable_line_out(true);
        break;

    case 4:
        if (g_test_en && label[4]) {
            uint8_t oled_te = (oled_tst_mode != 0);
            uint8_t oled_tm = (oled_tst_mode & 0x0F) - 1;
            // LOGI("OLED TE=%d,TM=%d",oled_te,oled_tm);
            OLED_Pattern(oled_te, oled_tm, 4);
            oled_tst_mode++;
            if (oled_tst_mode >= 6)
                oled_tst_mode = 0;
            break;
        }
    }
}

static void page_source_enter() {
    in_sourcepage = true;
}

static void page_source_exit() {
    // LOGI("page_source_exit %d",oled_tst_mode);
    if ((oled_tst_mode != 0) && g_test_en) {
        OLED_Pattern(0, 0, 4);
        oled_tst_mode = 0;
    }
    in_sourcepage = false;
}

void switch_to_analog(bool is_bay) {
    Source_AV(is_bay);

    osd_show(true);

    lvgl_switch_to_720p();
    draw_osd_clear();
    lv_timer_handler();

    Display_Osd(g_setting.record.osd);

    g_setting.autoscan.last_source = is_bay ? SETTING_SOURCE_EXPANSION : SETTING_SOURCE_AV_IN;
    ini_putl("autoscan", "last_source", g_setting.autoscan.last_source, SETTING_INI);
}

void switch_to_hdmiin() {
    Source_HDMI_in();
    IT66121_close();
    sleep(2);

    if (g_hw_stat.hdmiin_vtmg == 1)
        lvgl_switch_to_1080p();
    else
        lvgl_switch_to_720p();

    osd_show(true);
    draw_osd_clear();
    lv_timer_handler();

    g_menu_op = OPLEVEL_VIDEO;
    g_source_info.source = SOURCE_HDMI_IN;
    enable_line_out(false);
    g_setting.autoscan.last_source = SETTING_SOURCE_HDMI_IN;
    ini_putl("autoscan", "last_source", g_setting.autoscan.last_source, SETTING_INI);
}

page_pack_t pp_source = {
    .p_arr = {
        .cur = 0,
        .max = 4,
    },

    .create = page_source_create,
    .enter = page_source_enter,
    .exit = page_source_exit,
    .on_roller = NULL,
    .on_click = page_source_on_click,
};

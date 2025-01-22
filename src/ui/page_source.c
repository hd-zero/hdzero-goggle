#include "page_source.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <minIni.h>

#include "core/app_state.h"
#include "core/common.hh"
#include "core/dvr.h"
#include "core/osd.h"
#include "core/settings.h"
#include "driver/beep.h"
#include "driver/hardware.h"
#include "driver/it66121.h"
#include "driver/oled.h"
#include "lang/language.h"
#include "ui/page_common.h"
#include "ui/page_scannow.h"
#include "ui/ui_main_menu.h"
#include "ui/ui_porting.h"
#include "ui/ui_style.h"

// local
static lv_coord_t col_dsc[] = {160, 160, 200, 160, 160, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

static lv_obj_t *label[5];
static uint8_t oled_tst_mode = 0; // 0=Normal,1=CB; 2-Grid; 3=All Black; 4=All White,5=Boot logo
static bool in_sourcepage = false;
static btn_group_t btn_group0, btn_group1, btn_group2, btn_group3;

static lv_obj_t *page_source_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[128];
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 960, 894);

    snprintf(buf, sizeof(buf), "%s:", _lang("Source"));
    create_text(NULL, section, false, buf, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 894);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    label[0] = create_label_item(cont, "HDZero", 1, 0, 3);
    snprintf(buf, sizeof(buf), "HDMI %s", _lang("In"));
    label[1] = create_label_item(cont, buf, 1, 1, 3);
    snprintf(buf, sizeof(buf), "AV %s", _lang("In"));
    label[2] = create_label_item(cont, buf, 1, 2, 3);
    label[3] = create_label_item(cont, _lang("Expansion Module"), 1, 3, 3);

    create_btn_group_item(&btn_group0, cont, 2, _lang("Analog Video"), "NTSC", "PAL", "", "", 4);
    btn_group_set_sel(&btn_group0, g_setting.source.analog_format);

    create_btn_group_item(&btn_group1, cont, 2, _lang("HDZero Band"), _lang("Raceband"), _lang("Lowband"), "", "", 5);
    btn_group_set_sel(&btn_group1, g_setting.source.hdzero_band);

    create_btn_group_item(&btn_group2, cont, 2, _lang("HDZero BW"), _lang("Wide"), _lang("Narrow"), "", "", 6);
    btn_group_set_sel(&btn_group2, g_setting.source.hdzero_bw);

    create_btn_group_item(&btn_group3, cont, 2, _lang("Analog Ratio"), _lang("4:3"), _lang("16:9"), "", "", 7);
    btn_group_set_sel(&btn_group3, g_setting.source.analog_ratio);

    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    if (g_setting.storage.selftest) {
        pp_source.p_arr.max = 10;
        label[4] = create_label_item(cont, "OLED Pattern: Normal", 1, 8, 3);
        create_label_item(cont, buf, 1, 9, 3);
    } else {
        pp_source.p_arr.max = 9;
        label[4] = NULL;
        create_label_item(cont, buf, 1, 8, 3);
    }
    return page;
}

char *state2string(uint8_t status) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "#%s %s#", status ? "00FF00" : "C0C0C0", status ? _lang("Connected") : _lang("Disconnected"));
    return buf;
}

void source_status_timer() {
    char buf[64];
    int ch;

    if (!in_sourcepage)
        return;

    ch = g_setting.scan.channel & 0x7F;
    if (g_setting.source.hdzero_band == SETTING_SOURCES_HDZERO_BAND_RACEBAND) {
        if (ch <= 8) {
            snprintf(buf, sizeof(buf), "HDZero: R%d", ch);
        } else {
            snprintf(buf, sizeof(buf), "HDZero: F%d", (ch - 8) * 2);
        }
    } else {
        if (ch > 8) {
            g_setting.scan.channel = 1;
        }
        snprintf(buf, sizeof(buf), "HDZero: L%d", ch);
    }
    lv_label_set_text(label[0], buf);

    snprintf(buf, sizeof(buf), "HDMI %s: %s", _lang("In"), state2string(g_source_info.hdmi_in_status));
    lv_label_set_text(label[1], buf);

    snprintf(buf, sizeof(buf), "AV %s: %s", _lang("In"), state2string(g_source_info.av_in_status));
    lv_label_set_text(label[2], buf);

    snprintf(buf, sizeof(buf), "%s: %s", _lang("Expansion Module"), state2string(g_source_info.av_bay_status));
    lv_label_set_text(label[3], buf);

    if (g_setting.storage.selftest && label[3]) {
        uint8_t oled_tm = oled_tst_mode & 0x0F;
        char *pattern_label[6] = {"Normal", "Color Bar", "Grid", "All Black", "All White", "Boot logo"};
        char str[32];
        snprintf(str, sizeof(buf), "OLED Pattern: %s", pattern_label[oled_tm]);
        lv_label_set_text(label[4], str);
    }
}

static void page_source_select_hdzero() {
    progress_bar.start = 1;
    app_switch_to_hdzero(true);
    app_state_push(APP_STATE_VIDEO);
    g_source_info.source = SOURCE_HDZERO;
    dvr_select_audio_source(2);
    dvr_enable_line_out(true);
}

static void page_source_select_hdmi() {
    if (g_source_info.hdmi_in_status)
        app_switch_to_hdmi_in();
}

static void page_source_select_av_in() {
    app_switch_to_analog(0);
    app_state_push(APP_STATE_VIDEO);
    g_source_info.source = SOURCE_AV_IN;
    dvr_select_audio_source(2);
    dvr_enable_line_out(true);
}

static void page_source_select_expansion() {
    app_switch_to_analog(1);
    app_state_push(APP_STATE_VIDEO);
    g_source_info.source = SOURCE_EXPANSION;
    dvr_select_audio_source(2);
    dvr_enable_line_out(true);
}

void source_toggle() {
    beep_dur(BEEP_SHORT);
    switch (g_source_info.source) {
    case SOURCE_HDZERO:
        page_source_select_expansion();
        break;
    case SOURCE_EXPANSION:
        page_source_select_hdzero();
        break;
    case SOURCE_AV_IN:
        page_source_select_hdzero();
        break;
    case SOURCE_HDMI_IN:
        page_source_select_hdzero();
        break;
    }
    Analog_Module_Power(0);
}

void source_cycle() {
    beep_dur(BEEP_SHORT);
    switch (g_source_info.source) {
    case SOURCE_HDZERO:
        if (g_source_info.hdmi_in_status) {
            page_source_select_hdmi();
        } else {
            page_source_select_av_in();
        }
        break;
    case SOURCE_EXPANSION:
        page_source_select_hdzero();
        break;
    case SOURCE_AV_IN:
        page_source_select_expansion();
        break;
    case SOURCE_HDMI_IN:
        page_source_select_av_in();
        break;
    }
    Analog_Module_Power(0);
}

static void page_source_on_click(uint8_t key, int sel) {
    switch (sel) {
    case 0: // HDZero in
        page_source_select_hdzero();
        break;

    case 1: // HDMI in
        page_source_select_hdmi();
        break;

    case 2: // AV in
        page_source_select_av_in();
        break;

    case 3: // Expansion module in
        page_source_select_expansion();
        break;

    case 4: // Analog video format
        btn_group_toggle_sel(&btn_group0);
        g_setting.source.analog_format = btn_group_get_sel(&btn_group0);
        ini_putl("source", "analog_format", g_setting.source.analog_format, SETTING_INI);
        break;

    case 5: // HDZero band format
        btn_group_toggle_sel(&btn_group1);
        g_setting.source.hdzero_band = btn_group_get_sel(&btn_group1);
        page_scannow_set_channel_label();
        ini_putl("source", "hdzero_band", g_setting.source.hdzero_band, SETTING_INI);
        break;

    case 6: // HDZero bw format
        btn_group_toggle_sel(&btn_group2);
        g_setting.source.hdzero_bw = btn_group_get_sel(&btn_group2);
        ini_putl("source", "hdzero_bw", g_setting.source.hdzero_bw, SETTING_INI);
        break;

    case 7: // Analog video format
        btn_group_toggle_sel(&btn_group3);
        g_setting.source.analog_ratio = btn_group_get_sel(&btn_group3);
        ini_putl("source", "analog_ratio", g_setting.source.analog_ratio, SETTING_INI);
        break;

    case 8:
        if (g_setting.storage.selftest && label[4]) {
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
    Analog_Module_Power(0);
}

static void page_source_enter() {
    in_sourcepage = true;
}

static void page_source_exit() {
    // LOGI("page_source_exit %d",oled_tst_mode);
    if ((oled_tst_mode != 0) && g_setting.storage.selftest) {
        OLED_Pattern(0, 0, 4);
        oled_tst_mode = 0;
    }
    in_sourcepage = false;
}

page_pack_t pp_source = {
    .p_arr = {
        .cur = 0,
        .max = 6,
    },

    .name = "Source",
    .create = page_source_create,
    .enter = page_source_enter,
    .exit = page_source_exit,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = NULL,
    .on_click = page_source_on_click,
    .on_right_button = NULL,
};

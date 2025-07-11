#include "page_source.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <minIni.h>

#include "../conf/ui.h"

#include "core/app_state.h"
#include "core/common.hh"
#include "core/dvr.h"
#include "core/osd.h"
#include "driver/beep.h"
#include "driver/hardware.h"
#include "driver/it66121.h"
#include "driver/screen.h"
#include "lang/language.h"
#include "ui/page_common.h"
#include "ui/page_scannow.h"
#include "ui/ui_main_menu.h"
#include "ui/ui_porting.h"
#include "ui/ui_style.h"

enum {
    ROW_GOGGLE_HDZERO = 0,
    ROW_GOGGLE_ANALOG,
    ROW_GOGGLE_HDMI,
    ROW_GOGGLE_AV,
    ROW_GOGGLE_HDZ_BAND,
    ROW_GOGGLE_HDZ_WIDTH,
    ROW_GOGGLE_ANALOG_RATIO,
    ROW_GOGGLE_ANALOG_VIDEO,
    ROW_GOGGLE_TEST_PATTERN,
    ROW_GOGGLE_BACK,
    ROW_GOGGLE_COUNT
};

enum {
    ROW_BOXPRO_ANALOG_VIDEO = -1,
    ROW_BOXPRO_HDZERO = 0,
    ROW_BOXPRO_ANALOG,
    ROW_BOXPRO_HDMI,
    ROW_BOXPRO_AV,
    ROW_BOXPRO_HDZ_BAND,
    ROW_BOXPRO_HDZ_WIDTH,
    ROW_BOXPRO_ANALOG_RATIO,
    ROW_BOXPRO_TEST_PATTERN,
    ROW_BOXPRO_BACK,
    ROW_BOXPRO_COUNT
};

#if HDZGOGGLE
#define ROW_HDZERO       ROW_GOGGLE_HDZERO
#define ROW_ANALOG       ROW_GOGGLE_ANALOG
#define ROW_HDMI         ROW_GOGGLE_HDMI
#define ROW_AV           ROW_GOGGLE_AV
#define ROW_HDZ_BAND     ROW_GOGGLE_HDZ_BAND
#define ROW_HDZ_WIDTH    ROW_GOGGLE_HDZ_WIDTH
#define ROW_ANALOG_RATIO ROW_GOGGLE_ANALOG_RATIO
#define ROW_ANALOG_VIDEO ROW_GOGGLE_ANALOG_VIDEO
#define ROW_TEST_PATTERN ROW_GOGGLE_TEST_PATTERN
#define ROW_BACK         ROW_GOGGLE_BACK
#define ROW_COUNT        ROW_GOGGLE_COUNT
#elif HDZBOXPRO
#define ROW_HDZERO       ROW_BOXPRO_HDZERO
#define ROW_ANALOG       ROW_BOXPRO_ANALOG
#define ROW_HDMI         ROW_BOXPRO_HDMI
#define ROW_AV           ROW_BOXPRO_AV
#define ROW_HDZ_BAND     ROW_BOXPRO_HDZ_BAND
#define ROW_HDZ_WIDTH    ROW_BOXPRO_HDZ_WIDTH
#define ROW_ANALOG_RATIO ROW_BOXPRO_ANALOG_RATIO
#define ROW_ANALOG_VIDEO ROW_BOXPRO_ANALOG_VIDEO
#define ROW_TEST_PATTERN ROW_BOXPRO_TEST_PATTERN
#define ROW_BACK         ROW_BOXPRO_BACK
#define ROW_COUNT        ROW_BOXPRO_COUNT
#endif

// local
static lv_coord_t col_dsc[] = {UI_SOURCE_COLS};
static lv_coord_t row_dsc[] = {UI_SOURCE_ROWS};

static lv_obj_t *label[6] = {NULL};
static uint8_t oled_tst_mode = 0; // 0=Normal, 1=CB, 2=Grid, 3=All Black, 4=All White, 5=Boot logo
static bool in_sourcepage = false;
static btn_group_t btn_group0, btn_group1, btn_group2, btn_group3;

static lv_obj_t *page_source_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[128];

    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, UI_PAGE_VIEW_SIZE);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, UI_PAGE_VIEW_SIZE);

    snprintf(buf, sizeof(buf), "%s:", _lang("Source"));
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

    label[0] = create_label_item(cont, "HDZero", 1, ROW_HDZERO, 3);
    snprintf(buf, sizeof(buf), "%s", _lang("Analog"));
    label[1] = create_label_item(cont, buf, 1, ROW_ANALOG, 3);
    snprintf(buf, sizeof(buf), "HDMI %s", _lang("In"));
    label[2] = create_label_item(cont, buf, 1, ROW_HDMI, 3);
    snprintf(buf, sizeof(buf), "AV %s", _lang("In"));
    label[3] = create_label_item(cont, buf, 1, ROW_AV, 3);

    create_btn_group_item(&btn_group1, cont, 2, _lang("HDZero Band"), _lang("Raceband"), _lang("Lowband"), "", "", ROW_HDZ_BAND);
    btn_group_set_sel(&btn_group1, g_setting.source.hdzero_band);

    create_btn_group_item(&btn_group2, cont, 2, _lang("HDZero BW"), _lang("Wide"), _lang("Narrow"), "", "", ROW_HDZ_WIDTH);
    btn_group_set_sel(&btn_group2, g_setting.source.hdzero_bw);

    create_btn_group_item(&btn_group3, cont, 2, _lang("Analog Ratio"), _lang("4:3"), _lang("16:9"), "", "", ROW_ANALOG_RATIO);
    btn_group_set_sel(&btn_group3, g_setting.source.analog_ratio);

    if (ROW_ANALOG_VIDEO > 0) {
        create_btn_group_item(&btn_group0, cont, 2, _lang("Analog Video"), "NTSC", "PAL", "", "", ROW_ANALOG_VIDEO);
        btn_group_set_sel(&btn_group0, g_setting.source.analog_format);
    }

    // By default this Row is hidden unless selftest is detected.
    label[4] = create_label_item(cont, "Display Pattern: Normal", 1, ROW_TEST_PATTERN, 3);
    lv_obj_add_flag(label[4], LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(pp_source.p_arr.panel[ROW_TEST_PATTERN], FLAG_SELECTABLE);
    if (g_setting.storage.selftest) {
        lv_obj_clear_flag(label[4], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(pp_source.p_arr.panel[ROW_TEST_PATTERN], FLAG_SELECTABLE);
    }

    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    create_label_item(cont, buf, 1, ROW_BACK, 3);
    pp_source.p_arr.max = ROW_COUNT;

    if (TARGET_GOGGLE == getTargetType()) {
        label[5] = create_label_item(cont, _lang("Analog input requires Expansion Module"), 1, ROW_COUNT, 3);
        lv_obj_set_style_text_font(label[5], UI_PAGE_LABEL_FONT, 0);
        lv_obj_set_style_pad_top(label[5], UI_PAGE_TEXT_PAD, 0);
    }

    return page;
}

char *state2string(uint8_t status) {
    static char buf[32];
    snprintf(buf, sizeof(buf), "#%s %s#", status ? "00FF00" : "C0C0C0", status ? _lang("Signal detected") : _lang("No signal"));
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
        } else if (ch <= 12) {
            snprintf(buf, sizeof(buf), "HDZero: F%d", (ch - 8) * 2);
        } else {
            g_setting.scan.channel = 1;
            snprintf(buf, sizeof(buf), "HDZero: R1");
        }
    } else {
        if (ch > 8) {
            g_setting.scan.channel = 1;
        }
        snprintf(buf, sizeof(buf), "HDZero: L%d", ch);
    }
    lv_label_set_text(label[0], buf);

    if (TARGET_GOGGLE == getTargetType()) {
        snprintf(buf, sizeof(buf), "%s: %s", _lang("Analog"), state2string(g_source_info.av_bay_status));
    } else if (TARGET_BOXPRO == getTargetType()) {
        snprintf(buf, sizeof(buf), "%s: %s", _lang("Analog"), channel2str(0, 0, g_setting.source.analog_channel));
    }
    lv_label_set_text(label[1], buf);

    snprintf(buf, sizeof(buf), "HDMI %s: %s", _lang("In"), state2string(g_source_info.hdmi_in_status));
    lv_label_set_text(label[2], buf);

    snprintf(buf, sizeof(buf), "AV %s: %s", _lang("In"), state2string(g_source_info.av_in_status));
    lv_label_set_text(label[3], buf);

    if (g_setting.storage.selftest && label[3]) {
        uint8_t oled_tm = oled_tst_mode & 0x0F;
        char *pattern_label[6] = {"Normal", "Color Bar", "Grid", "All Black", "All White", "Boot logo"};
        char str[32];
        snprintf(str, sizeof(buf), "Display Pattern: %s", pattern_label[oled_tm]);
        lv_label_set_text(label[4], str);
    }
}

static void page_source_select_hdzero() {
    progress_bar.start = 1;
    app_switch_to_hdzero(true);
    app_state_push(APP_STATE_VIDEO);
    g_source_info.source = SOURCE_HDZERO;
    dvr_select_audio_source(g_setting.record.audio_source);
    dvr_enable_line_out(true);
}

static void page_source_select_hdmi() {
    if (g_source_info.hdmi_in_status)
        app_switch_to_hdmi_in();
}

static void page_source_select_av_in() {
    app_switch_to_analog(SOURCE_AV_IN);
    app_state_push(APP_STATE_VIDEO);
    g_source_info.source = SOURCE_AV_IN;
    dvr_select_audio_source(g_setting.record.audio_source);
    dvr_enable_line_out(true);
}

static void page_source_select_analog() {
    app_switch_to_analog(SOURCE_AV_MODULE);
    app_state_push(APP_STATE_VIDEO);
    g_source_info.source = SOURCE_AV_MODULE;
    dvr_select_audio_source(g_setting.record.audio_source);
    dvr_enable_line_out(true);
}

void source_toggle() {
    beep_dur(BEEP_SHORT);
    switch (g_source_info.source) {
    case SOURCE_HDZERO:
        page_source_select_analog();
        break;
    case SOURCE_AV_MODULE:
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
    case SOURCE_AV_MODULE:
        page_source_select_hdzero();
        break;
    case SOURCE_AV_IN:
        page_source_select_analog();
        break;
    case SOURCE_HDMI_IN:
        page_source_select_av_in();
        break;
    }
    Analog_Module_Power(0);
}

static void page_source_on_click(uint8_t key, int sel) {
    if (sel == ROW_HDZERO) {
        page_source_select_hdzero();
    } else if (sel == ROW_ANALOG) {
        page_source_select_analog();
    } else if (sel == ROW_HDMI) {
        app_switch_to_hdmi_in();
    } else if (sel == ROW_AV) {
        page_source_select_av_in();
    } else if (sel == ROW_HDZ_BAND) {
        btn_group_toggle_sel(&btn_group1);
        g_setting.source.hdzero_band = btn_group_get_sel(&btn_group1);
        page_scannow_set_channel_label();
        ini_putl("source", "hdzero_band", g_setting.source.hdzero_band, SETTING_INI);
    } else if (sel == ROW_HDZ_WIDTH) {
        btn_group_toggle_sel(&btn_group2);
        g_setting.source.hdzero_bw = btn_group_get_sel(&btn_group2);
        ini_putl("source", "hdzero_bw", g_setting.source.hdzero_bw, SETTING_INI);
    } else if (sel == ROW_ANALOG_RATIO) {
        btn_group_toggle_sel(&btn_group3);
        g_setting.source.analog_ratio = btn_group_get_sel(&btn_group3);
        ini_putl("source", "analog_ratio", g_setting.source.analog_ratio, SETTING_INI);
    } else if (sel == ROW_ANALOG_VIDEO) {
        btn_group_toggle_sel(&btn_group0);
        g_setting.source.analog_format = btn_group_get_sel(&btn_group0);
        ini_putl("source", "analog_format", g_setting.source.analog_format, SETTING_INI);
    } else if (sel == ROW_TEST_PATTERN) {
        if (g_setting.storage.selftest && label[4]) {
            uint8_t oled_te = (oled_tst_mode != 0);
            uint8_t oled_tm = (oled_tst_mode & 0x0F) - 1;
            // LOGI("OLED TE=%d,TM=%d",oled_te,oled_tm);
            Screen_Pattern(oled_te, oled_tm, 4);
            if (++oled_tst_mode > 5) {
                oled_tst_mode = 0;
            }
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
        Screen_Pattern(0, 0, 4);
        oled_tst_mode = 0;
    }
    in_sourcepage = false;
}

page_pack_t pp_source = {
    .p_arr = {
        .cur = 0,
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

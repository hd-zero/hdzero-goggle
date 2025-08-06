#include "page_analog_rssi.h"

#include <stdio.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "../conf/ui.h"

#include "core/app_state.h"
#include "core/battery.h"
#include "core/common.hh"
#include "driver/dm5680.h"
#include "driver/hardware.h"
#include "driver/mcp3021.h"
#include "driver/rtc6715.h"
#include "lang/language.h"
#include "page_common.h"
#include "ui/ui_style.h"

#if defined(HDZBOXPRO) || defined(HDZGOGGLE2)

enum {
    ROW_CALIBRATE_RSSI_MIN = 0,
    ROW_CALIBRATE_RSSI_MAX,
    ROW_BACK,
    ROW_COUNT
};

static lv_coord_t col_dsc[] = {UI_ANALOG_RSSI_COLS};
static lv_coord_t row_dsc[] = {UI_ANALOG_RSSI_ROWS};

static lv_obj_t *calibrate_rssi_min_obj;
static lv_obj_t *calibrate_rssi_max_obj;

static lv_obj_t *page_analog_rssi_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[1024];

    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, UI_PAGE_VIEW_SIZE);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, UI_PAGE_VIEW_SIZE);

    snprintf(buf, sizeof(buf), "%s:", _lang("Analog RSSI"));
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

    snprintf(buf, sizeof(buf), "%s", _lang("Calibrate RSSI Min"));
    calibrate_rssi_min_obj = create_label_item(cont, buf, 1, ROW_CALIBRATE_RSSI_MIN, 3);
    snprintf(buf, sizeof(buf), "%s", _lang("Calibrate RSSI Max"));
    calibrate_rssi_max_obj = create_label_item(cont, buf, 1, ROW_CALIBRATE_RSSI_MAX, 3);

    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    create_label_item(cont, buf, 1, ROW_BACK, 1);

    lv_obj_t *note = lv_label_create(cont);
    snprintf(buf, sizeof(buf), "%s:\n    -%s\n    -%s\n%s:\n    -%s\n    -%s\n    -%s",
             _lang("Before calibrating RSSI min"),
             _lang("Remove the BoxPro antenna"),
             _lang("Turn off all VTX"),
             _lang("Before calibrating RSSI max"),
             _lang("Remove the BoxPro antenna"),
             _lang("Turn on the analog VTX to R1 25mw"),
             _lang("Place the VTX 2 meters away from the BoxPro"));
    lv_label_set_text(note, buf);

    lv_obj_set_style_text_font(note, UI_PAGE_LABEL_FONT, 0);
    lv_obj_set_style_text_align(note, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(note, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(note, UI_PAGE_TEXT_PAD, 0);
    lv_label_set_long_mode(note, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(note, LV_GRID_ALIGN_START, 1, 4, LV_GRID_ALIGN_START, 7, 2);

    return page;
}

static void on_enter() {
    rtc6715.init(1, 0);
    rtc6715.set_ch(33); // R1
}

static void on_exit() {
    rtc6715.init(0, 0);
}

static void on_created() {
}

static void on_update(uint32_t delta_ms) {
}

static void on_roller(uint8_t key) {
    lv_label_set_text(calibrate_rssi_min_obj, _lang("Calibrate RSSI Min"));
    lv_label_set_text(calibrate_rssi_max_obj, _lang("Calibrate RSSI Max"));
}

static void on_click(uint8_t key, int sel) {
    char buf[128];
    int volt_mv = 0;

    switch (sel) {
    case ROW_CALIBRATE_RSSI_MIN:
        LOGI("capture rssi voltage");

        for (int i = 0; i < 8; i++) {
            volt_mv += rtc6715.get_rssi();
        }

        volt_mv = volt_mv >> 3;
        ini_putl("analog_rssi", "calib_min", (uint16_t)volt_mv, SETTING_INI);
        g_setting.analog_rssi.calib_min = ini_getl("analog_rssi", "calib_min", g_setting_defaults.analog_rssi.calib_max, SETTING_INI);

        snprintf(buf, sizeof(buf), "%s #FFFF00 %s#", _lang("Calibrate RSSI Min"), _lang("Complete"));
        lv_label_set_text(calibrate_rssi_min_obj, buf);

        LOGI("result: calib_min=%dmv", g_setting.analog_rssi.calib_min);
        break;

    case ROW_CALIBRATE_RSSI_MAX:
        LOGI("capture rssi voltage");

        for (int i = 0; i < 8; i++) {
            volt_mv += rtc6715.get_rssi();
        }

        volt_mv = volt_mv >> 3;
        ini_putl("analog_rssi", "calib_max", (uint16_t)volt_mv, SETTING_INI);
        g_setting.analog_rssi.calib_max = ini_getl("analog_rssi", "calib_max", g_setting_defaults.analog_rssi.calib_max, SETTING_INI);

        snprintf(buf, sizeof(buf), "%s #FFFF00 %s#", _lang("Calibrate RSSI Max"), _lang("Complete"));
        lv_label_set_text(calibrate_rssi_max_obj, buf);

        LOGI("result: calib_max=%dmv", g_setting.analog_rssi.calib_max);
        break;
    default:
        break;
    }
}

static void on_right_button(bool is_short) {
}

page_pack_t pp_analog_rssi = {
    .p_arr = {
        .cur = 0,
        .max = ROW_COUNT,
    },
    .name = "Analog RSSI",
    .create = page_analog_rssi_create,
    .enter = on_enter,
    .exit = on_exit,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = on_roller,
    .on_click = on_click,
    .on_right_button = NULL,
};
#endif
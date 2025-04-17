#include "page_record.h"

#include <stdio.h>
#include <stdlib.h>

#include <minIni.h>

#include "../core/common.hh"
#include "core/settings.h"
#include "driver/rtc.h"
#include "lang/language.h"
#include "page_common.h"
#include "ui/ui_style.h"

static btn_group_t btn_group_record_mode;
static btn_group_t btn_group_format;
static btn_group_t btn_group_bitrate_scale;
static btn_group_t btn_group_record_osd;
static btn_group_t btn_group_record_audio;
static btn_group_t btn_group_audio_source;
static btn_group_t btn_group_file_naming;

static lv_coord_t col_dsc[] = {160, 200, 200, 160, 120, 120, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

static void update_visibility() {
    btn_group_enable(&btn_group_audio_source, btn_group_record_audio.current == 0);

    if (btn_group_record_audio.current == 0) {
        lv_obj_add_flag(pp_record.p_arr.panel[4], FLAG_SELECTABLE);
    } else {
        lv_obj_clear_flag(pp_record.p_arr.panel[4], FLAG_SELECTABLE);
    }

    btn_group_enable(&btn_group_file_naming, rtc_has_battery() == 0);

    if (rtc_has_battery() == 0) {
        lv_obj_add_flag(pp_record.p_arr.panel[5], FLAG_SELECTABLE);
    } else {
        lv_obj_clear_flag(pp_record.p_arr.panel[5], FLAG_SELECTABLE);
    }
}

static lv_obj_t *page_record_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[256];
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 980);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 984);

    snprintf(buf, sizeof(buf), "%s:", _lang("Record Option"));
    create_text(NULL, section, false, buf, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 680);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    create_btn_group_item(&btn_group_record_mode, cont, 2, _lang("Record Mode"), _lang("Auto"), _lang("Manual"), "", "", 0);
    create_btn_group_item(&btn_group_format, cont, 2, _lang("Record Format"), "MP4", "TS", "", "", 1);
    create_btn_group_item(&btn_group_bitrate_scale, cont, 3, _lang("Record Bitrate"), _lang("Normal"), "1/2", "1/4", "", 2);
    create_btn_group_item(&btn_group_record_osd, cont, 2, _lang("Record OSD"), _lang("Yes"), _lang("No"), "", "", 3);
    create_btn_group_item(&btn_group_record_audio, cont, 2, _lang("Record Audio"), _lang("Yes"), _lang("No"), "", "", 4);
    create_btn_group_item(&btn_group_audio_source, cont, 3, _lang("Audio Source"), _lang("Mic"), _lang("Line In"), _lang("A/V In"), "", 5);
    create_btn_group_item(&btn_group_file_naming, cont, 2, _lang("Naming Scheme"), _lang("Digits"), _lang("Date"), "", "", 6);
    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    create_label_item(cont, buf, 1, 7, 1);

    btn_group_set_sel(&btn_group_record_mode, g_setting.record.mode_manual ? 1 : 0);
    btn_group_set_sel(&btn_group_format, g_setting.record.format_ts ? 1 : 0);
    btn_group_set_sel(&btn_group_bitrate_scale, g_setting.record.bitrate_scale);
    btn_group_set_sel(&btn_group_record_osd, g_setting.record.osd ? 0 : 1);
    btn_group_set_sel(&btn_group_record_audio, g_setting.record.audio ? 0 : 1);
    btn_group_set_sel(&btn_group_audio_source, g_setting.record.audio_source);
    btn_group_set_sel(&btn_group_file_naming, g_setting.record.naming);

    lv_obj_t *label2 = lv_label_create(cont);
    snprintf(buf, sizeof(buf), "%s.\n%s.",
             _lang("MP4 format requires properly closing files or the files will be corrupt"),
             _lang("TS format is highly recommended"));
    lv_label_set_text(label2, buf);
    lv_obj_set_style_text_font(label2, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label2, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(label2, 12, 0);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_START, 1, 4,
                         LV_GRID_ALIGN_START, 8, 3);

    update_visibility();

    return page;
}

static void page_record_on_click(uint8_t key, int sel) {
    if (sel == 0) {
        btn_group_toggle_sel(&btn_group_record_mode);
        g_setting.record.mode_manual = btn_group_get_sel(&btn_group_record_mode);
        settings_put_bool("record", "mode_manual", g_setting.record.mode_manual);
    } else if (sel == 1) {
        btn_group_toggle_sel(&btn_group_format);
        g_setting.record.format_ts = btn_group_get_sel(&btn_group_format);
        settings_put_bool("record", "format_ts", g_setting.record.format_ts);
        if (g_setting.record.format_ts)
            ini_puts("record", "type", "ts", REC_CONF);
        else
            ini_puts("record", "type", "mp4", REC_CONF);
    } else if (sel == 2) {
        btn_group_toggle_sel(&btn_group_bitrate_scale);
        g_setting.record.bitrate_scale = btn_group_get_sel(&btn_group_bitrate_scale);
        ini_putl("record", "bitrate_scale", g_setting.record.bitrate_scale, SETTING_INI);
    } else if (sel == 3) {
        btn_group_toggle_sel(&btn_group_record_osd);
        g_setting.record.osd = !btn_group_get_sel(&btn_group_record_osd);
        settings_put_bool("record", "osd", g_setting.record.osd);
    } else if (sel == 4) {
        btn_group_toggle_sel(&btn_group_record_audio);
        g_setting.record.audio = !btn_group_get_sel(&btn_group_record_audio);
        settings_put_bool("record", "audio", g_setting.record.audio);
        update_visibility();
    } else if (sel == 5) {
        btn_group_toggle_sel(&btn_group_audio_source);
        g_setting.record.audio_source = btn_group_get_sel(&btn_group_audio_source);
        ini_putl("record", "audio_source", g_setting.record.audio_source, SETTING_INI);
    } else if (sel == 6) {
        if (rtc_has_battery() == 0) {
            btn_group_toggle_sel(&btn_group_file_naming);
            g_setting.record.naming = btn_group_get_sel(&btn_group_file_naming);
            ini_putl("record", "naming", g_setting.record.naming, SETTING_INI);
        }
    }
}

page_pack_t pp_record = {
    .p_arr = {
        .cur = 0,
        .max = 8,
    },
    .name = "Record Option",
    .create = page_record_create,
    .enter = NULL,
    .exit = NULL,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = NULL,
    .on_click = page_record_on_click,
    .on_right_button = NULL,
};

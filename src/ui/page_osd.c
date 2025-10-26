#include "page_osd.h"

#include <stdio.h>

#include <log/log.h>
#include <minIni.h>

#include "../conf/ui.h"

#include "core/app_state.h"
#include "core/common.hh"
#include "core/osd.h"
#include "driver/hardware.h"
#include "lang/language.h"
#include "page_common.h"
#include "ui/ui_osd_element_pos.h"
#include "ui/ui_porting.h"
#include "ui/ui_style.h"
#include "util/math.h"

enum {
    ROW_OSD_ORBIT = 0,
    ROW_OSD_MODE,
    ROW_OSD_STARTUP_VISIBILITY,
    ROW_ADJUST_OSD_ELEMENTS,
    ROW_BACK,

    ROW_COUNT,
    ROW_USER_HINT = ROW_COUNT
};

static lv_coord_t col_dsc[] = {UI_OSD_COLS};
static lv_coord_t row_dsc[] = {UI_OSD_ROWS};

static btn_group_t btn_group_osd_orbit;
static btn_group_t btn_group_osd_mode;
static btn_group_t btn_group_osd_startup_visibility;

static lv_obj_t *page_osd_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[640];
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, UI_PAGE_VIEW_SIZE);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, UI_PAGE_VIEW_SIZE);

    create_text(NULL, section, false, "OSD:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, UI_PAGE_VIEW_SIZE);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    // create menu entries
    create_label_item(cont, _lang("Adjust OSD Elements"), 1, ROW_ADJUST_OSD_ELEMENTS, 1);
    snprintf(buf, sizeof(buf), "OSD %s", _lang("Orbit"));
    create_btn_group_item(&btn_group_osd_orbit, cont, 3, buf, _lang("Off"), _lang("Min"), _lang("Max"), "", ROW_OSD_ORBIT);
    snprintf(buf, sizeof(buf), "OSD %s", _lang("Mode"));
    create_btn_group_item(&btn_group_osd_mode, cont, 2, buf, "4x3", "16x9", "", "", ROW_OSD_MODE);
    create_btn_group_item(&btn_group_osd_startup_visibility, cont, 3, _lang("At Startup"), _lang("Show"), _lang("Hide"), _lang("Last"), "", ROW_OSD_STARTUP_VISIBILITY);
    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    create_label_item(cont, buf, 1, ROW_BACK, 1);

    lv_obj_t *label_user_hint = lv_label_create(cont);
    snprintf(buf, sizeof(buf), "%s\n%s.\n%s,\n%s.\n%s.",
             _lang("Note: The positioning preview will display all OSD elements. Some elements might"),
             _lang("not show during normal operation, depending on input source and conditions"),
             _lang("OSD Element positioning is based on a 1280x720 canvas"),
             _lang("Positions can be set for 4x3 and 16x9 modes separately"),
             _lang("the Show Element toggle is shared between both modes"));
    lv_label_set_text(label_user_hint, buf);
    lv_obj_set_style_text_font(label_user_hint, UI_PAGE_LABEL_FONT, 0);
    lv_obj_set_style_text_align(label_user_hint, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label_user_hint, lv_color_hex(TEXT_COLOR_DEFAULT), 0);
    lv_obj_set_style_pad_top(label_user_hint, UI_PAGE_TEXT_PAD, 0);
    lv_label_set_long_mode(label_user_hint, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(label_user_hint, LV_GRID_ALIGN_START, 1, 4,
                         LV_GRID_ALIGN_START, ROW_USER_HINT, 2);

    // set ui values from settings
    btn_group_set_sel(&btn_group_osd_orbit, g_setting.osd.orbit);
    btn_group_set_sel(&btn_group_osd_mode, g_setting.osd.embedded_mode);
    btn_group_set_sel(&btn_group_osd_startup_visibility, g_setting.osd.startup_visibility);

    // Set OSD orbit
    lvgl_screen_orbit(g_setting.osd.orbit > 0);

    return page;
}

// when changing settings in the element position preview,
// this is used to reflect the changes on this settings page,
// in case the changed setting is visible here
void page_osd_update_ui_elements() {
    btn_group_set_sel(&btn_group_osd_mode, g_setting.osd.embedded_mode);
}

static void open_element_pos_preview() {
    if (SOURCE_HDZERO == g_source_info.source) {
        progress_bar.start = 1;
        HDZero_open(g_setting.source.hdzero_bw);
        app_switch_to_hdzero(true);
    } else if (SOURCE_HDMI_IN == g_source_info.source) {
        app_switch_to_hdmi_in();
    } else {
        app_switch_to_analog(g_source_info.source == SOURCE_AV_IN);
    }

    ui_osd_element_pos_on_enter();
    app_state_push(APP_STATE_OSD_ELEMENT_PREV);
}

static void on_click(uint8_t key, int sel) {
    switch (sel) {

    case ROW_ADJUST_OSD_ELEMENTS:
        open_element_pos_preview();
        break;

    case ROW_OSD_STARTUP_VISIBILITY:
        btn_group_toggle_sel(&btn_group_osd_startup_visibility);
        g_setting.osd.startup_visibility = btn_group_get_sel(&btn_group_osd_startup_visibility);
        ini_putl("osd", "startup_visibility", g_setting.osd.startup_visibility, SETTING_INI);
        break;

    case ROW_OSD_MODE:
        btn_group_toggle_sel(&btn_group_osd_mode);
        g_setting.osd.embedded_mode = btn_group_get_sel(&btn_group_osd_mode);
        ini_putl("osd", "embedded_mode", g_setting.osd.embedded_mode, SETTING_INI);
        osd_update_element_positions();
        break;

    case ROW_OSD_ORBIT:
        btn_group_toggle_sel(&btn_group_osd_orbit);
        g_setting.osd.orbit = btn_group_get_sel(&btn_group_osd_orbit);
        ini_putl("osd", "orbit", g_setting.osd.orbit, SETTING_INI);
        lvgl_screen_orbit(g_setting.osd.orbit > 0);
        break;

    default:
        break;
    }
}

page_pack_t pp_osd = {
    .p_arr = {
        .cur = 0,
        .max = ROW_COUNT},
    .name = "OSD",
    .create = page_osd_create,
    .enter = NULL,
    .exit = NULL,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = NULL,
    .on_click = on_click,
    .on_right_button = NULL,
};
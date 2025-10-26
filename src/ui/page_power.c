#include "page_power.h"

#include <stdio.h>

#include <log/log.h>
#include <minIni.h>

#include "../conf/ui.h"

#include "core/app_state.h"
#include "core/battery.h"
#include "core/common.hh"
#include "driver/dm5680.h"
#include "driver/hardware.h"
#include "driver/mcp3021.h"
#include "lang/language.h"
#include "page_common.h"
#include "ui/ui_style.h"

#define WARNING_CELL_VOLTAGE_MIN 2800
#define WARNING_CELL_VOLTAGE_MAX 4200
#define CALIBRATION_OFFSET_MIN   -2500
#define CALIBRATION_OFFSET_MAX   2500

enum {
    ROW_BATT_C_LABEL = 0,
    ROW_CELL_COUNT_MODE,
    ROW_CELL_COUNT,
    ROW_WARNING_CELL_VOLTAGE,
    ROW_CALIBRATION_OFFSET,
    ROW_OSD_DISPLAY_MODE,
    ROW_WARN_TYPE,
    ROW_POWER_ANA,
    ROW_BACK,

    ROW_COUNT
};

static slider_group_t slider_group_cell_voltage;
static btn_group_t btn_group_cell_count_mode;
static slider_group_t slider_group_cell_count;
static slider_group_t slider_group_calibration_offset;
static btn_group_t btn_group_osd_display_mode;
static btn_group_t btn_group_warn_type;
static btn_group_t btn_group_power_ana;

static slider_group_t *selected_slider_group = NULL;

static lv_coord_t col_dsc[] = {UI_POWER_COLS};
static lv_coord_t row_dsc[] = {UI_POWER_ROWS};
static lv_obj_t *label_cell_count;

static void page_power_update_cell_count() {
    char str[10];

    // re-trigger cell detection
    battery_init();

    LOGI("cell_count:%d", g_battery.type);
    ini_putl("power", "cell_count", g_battery.type, SETTING_INI);

    snprintf(str, sizeof(str), "%dS", g_battery.type);
    lv_label_set_text(label_cell_count, str);

    lv_slider_set_value(slider_group_cell_count.slider, g_battery.type, LV_ANIM_OFF);
    char buf[5];
    snprintf(buf, sizeof(buf), "%d", g_battery.type);
    lv_label_set_text(slider_group_cell_count.label, buf);

    const bool isAutoCellCount = btn_group_cell_count_mode.current == 0;
    slider_enable(&slider_group_cell_count, !isAutoCellCount);
    if (isAutoCellCount) {
        lv_obj_clear_flag(pp_power.p_arr.panel[2], FLAG_SELECTABLE);
    } else {
        lv_obj_add_flag(pp_power.p_arr.panel[2], FLAG_SELECTABLE);
    }
}

static void page_power_update_calibration_offset() {
    g_battery.offset = g_setting.power.calibration_offset;
    ini_putl("power", "calibration_offset_mv", g_battery.offset, SETTING_INI);

    lv_slider_set_value(slider_group_calibration_offset.slider, g_battery.offset, LV_ANIM_OFF);
    char buf[7];
    snprintf(buf, sizeof(buf), "%.2fV", g_battery.offset / 1000.0);
    lv_label_set_text(slider_group_calibration_offset.label, buf);
}

static lv_obj_t *page_power_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[128];

    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, UI_PAGE_VIEW_SIZE);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, UI_PAGE_VIEW_SIZE);

    snprintf(buf, sizeof(buf), "%s:", _lang("Power"));
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
    lv_obj_clear_flag(pp_power.p_arr.panel[0], FLAG_SELECTABLE);

    // create menu entries
    create_label_item(cont, _lang("Battery"), 1, ROW_BATT_C_LABEL, 1);
    label_cell_count = create_label_item(cont, "-S", 2, ROW_BATT_C_LABEL, 1);
    create_btn_group_item(&btn_group_cell_count_mode, cont, 2, _lang("Cell Mode"), _lang("Auto"), _lang("Manual"), "", "", ROW_CELL_COUNT_MODE);
    create_slider_item(&slider_group_cell_count, cont, _lang("Cell Count"), CELL_MAX_COUNT, g_setting.power.cell_count, ROW_CELL_COUNT);
    create_slider_item(&slider_group_cell_voltage, cont, _lang("Warning Cell Voltage"), WARNING_CELL_VOLTAGE_MAX, g_setting.power.voltage, ROW_WARNING_CELL_VOLTAGE);
    create_slider_item(&slider_group_calibration_offset, cont, _lang("Voltage Calibration"), 0, g_setting.power.calibration_offset, ROW_CALIBRATION_OFFSET);
    create_btn_group_item(&btn_group_osd_display_mode, cont, 2, _lang("Display Mode"), _lang("Total"), _lang("Cell Avg."), "", "", ROW_OSD_DISPLAY_MODE);
    create_btn_group_item(&btn_group_warn_type, cont, 3, _lang("Warning Type"), _lang("Beep"), _lang("Visual"), _lang("Both"), "", ROW_WARN_TYPE);

#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
    if (getHwRevision() >= HW_REV_2) {
        create_btn_group_item(&btn_group_power_ana, cont, 2, _lang("AnalogRX Power"), _lang("On"), _lang("Auto"), "", "", ROW_POWER_ANA);
        pp_power.p_arr.max = ROW_COUNT;
    } else {
        pp_power.p_arr.max = ROW_COUNT - 1;
    }
#elif defined(HDZBOXPRO)
    pp_power.p_arr.max = ROW_COUNT - 1;
#endif

    // Back entry
    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    create_label_item(cont, buf, 1, pp_power.p_arr.max - 1, 1);

    // set menu entry min/max values and labels
    char str[6];
    snprintf(str, sizeof(buf), "%.2f", g_setting.power.voltage / 1000.0);
    lv_slider_set_range(slider_group_cell_voltage.slider, WARNING_CELL_VOLTAGE_MIN, WARNING_CELL_VOLTAGE_MAX);
    lv_label_set_text(slider_group_cell_voltage.label, str);

    snprintf(str, sizeof(buf), "%d", g_setting.power.cell_count);
    lv_slider_set_range(slider_group_cell_count.slider, CELL_MIN_COUNT, CELL_MAX_COUNT);
    lv_label_set_text(slider_group_cell_count.label, str);

    snprintf(str, sizeof(buf), "%.2fV", g_setting.power.calibration_offset / 1000.0);
    lv_slider_set_range(slider_group_calibration_offset.slider, CALIBRATION_OFFSET_MIN, CALIBRATION_OFFSET_MAX);
    lv_label_set_text(slider_group_calibration_offset.label, str);

    // set menu entry current values, loaded from stored settings
    btn_group_set_sel(&btn_group_cell_count_mode, g_setting.power.cell_count_mode);
    lv_slider_set_value(slider_group_cell_count.slider, g_setting.power.cell_count, LV_ANIM_OFF);
    lv_slider_set_value(slider_group_cell_voltage.slider, g_setting.power.voltage, LV_ANIM_OFF);
    lv_slider_set_value(slider_group_calibration_offset.slider, g_setting.power.calibration_offset, LV_ANIM_OFF);
    btn_group_set_sel(&btn_group_osd_display_mode, g_setting.power.osd_display_mode);
    btn_group_set_sel(&btn_group_warn_type, g_setting.power.warning_type);
    btn_group_set_sel(&btn_group_power_ana, g_setting.power.power_ana);

    page_power_update_cell_count();
    page_power_update_calibration_offset();

    return page;
}

static void power_cell_count_inc(void) {
    int32_t value = 0;

    value = lv_slider_get_value(slider_group_cell_count.slider);
    if (value < CELL_MAX_COUNT)
        value += 1;
    g_setting.power.cell_count = value;

    page_power_update_cell_count();
}

static void power_cell_count_dec(void) {
    int32_t value = 0;

    value = lv_slider_get_value(slider_group_cell_count.slider);
    if (value > CELL_MIN_COUNT)
        value -= 1;

    g_setting.power.cell_count = value;

    page_power_update_cell_count();
}

static void power_warning_voltage_inc(void) {
    int32_t value = 0;

    value = lv_slider_get_value(slider_group_cell_voltage.slider);
    if (value < WARNING_CELL_VOLTAGE_MAX)
        value += 10;

    lv_slider_set_value(slider_group_cell_voltage.slider, value, LV_ANIM_OFF);

    char buf[6];
    snprintf(buf, sizeof(buf), "%.2f", value / 1000.0);
    lv_label_set_text(slider_group_cell_voltage.label, buf);

    g_setting.power.voltage = value;
    LOGI("vol:%d", g_setting.power.voltage);
    ini_putl("power", "voltage_mv", g_setting.power.voltage, SETTING_INI);
}

static void power_warning_voltage_dec(void) {
    int32_t value = 0;

    value = lv_slider_get_value(slider_group_cell_voltage.slider);
    if (value > WARNING_CELL_VOLTAGE_MIN)
        value -= 10;

    lv_slider_set_value(slider_group_cell_voltage.slider, value, LV_ANIM_OFF);
    char buf[6];
    snprintf(buf, sizeof(buf), "%.2f", value / 1000.0);
    lv_label_set_text(slider_group_cell_voltage.label, buf);

    g_setting.power.voltage = value;
    LOGI("vol:%d", g_setting.power.voltage);
    ini_putl("power", "voltage_mv", g_setting.power.voltage, SETTING_INI);
}

static void power_calibration_offset_inc(void) {
    int32_t value = 0;

    value = lv_slider_get_value(slider_group_calibration_offset.slider);
    if (value < CALIBRATION_OFFSET_MAX)
        value += 10;

    g_setting.power.calibration_offset = value;

    page_power_update_calibration_offset();
}

static void power_calibration_offset_dec(void) {
    int32_t value = 0;

    value = lv_slider_get_value(slider_group_calibration_offset.slider);
    if (value > CALIBRATION_OFFSET_MIN)
        value -= 10;

    g_setting.power.calibration_offset = value;

    page_power_update_calibration_offset();
}

static void page_power_exit_slider() {
    lv_obj_add_style(selected_slider_group->slider, &style_silder_main, LV_PART_MAIN);
    app_state_push(APP_STATE_SUBMENU);
    selected_slider_group = NULL;
}

static void page_power_exit() {
    if (selected_slider_group != NULL) {
        page_power_exit_slider();
    }
}

static void page_power_on_roller(uint8_t key) {
    if (selected_slider_group == NULL) {
        return;
    }

    if (key == DIAL_KEY_UP) {
        if (selected_slider_group == &slider_group_cell_voltage) {
            power_warning_voltage_dec();
        } else if (selected_slider_group == &slider_group_cell_count) {
            power_cell_count_dec();
        } else if (selected_slider_group == &slider_group_calibration_offset) {
            power_calibration_offset_dec();
        }
    } else if (key == DIAL_KEY_DOWN) {
        if (selected_slider_group == &slider_group_cell_voltage) {
            power_warning_voltage_inc();
        } else if (selected_slider_group == &slider_group_cell_count) {
            power_cell_count_inc();
        } else if (selected_slider_group == &slider_group_calibration_offset) {
            power_calibration_offset_inc();
        }
    }
}

static void page_power_on_click(uint8_t key, int sel) {

    if (selected_slider_group != NULL) {
        page_power_exit_slider();
        return;
    }

    switch (sel) {

    case ROW_CELL_COUNT_MODE:
        btn_group_toggle_sel(&btn_group_cell_count_mode);
        g_setting.power.cell_count_mode = btn_group_get_sel(&btn_group_cell_count_mode);
        ini_putl("power", "cell_count_mode", g_setting.power.cell_count_mode, SETTING_INI);

        page_power_update_cell_count();
        break;

    case ROW_CELL_COUNT:
        if (g_setting.power.cell_count_mode == SETTING_POWER_CELL_COUNT_MODE_AUTO)
            break;
        app_state_push(APP_STATE_SUBMENU_ITEM_FOCUSED);
        lv_obj_add_style(slider_group_cell_count.slider, &style_silder_select, LV_PART_MAIN);
        selected_slider_group = &slider_group_cell_count;
        break;

    case ROW_WARNING_CELL_VOLTAGE:
        app_state_push(APP_STATE_SUBMENU_ITEM_FOCUSED);
        lv_obj_add_style(slider_group_cell_voltage.slider, &style_silder_select, LV_PART_MAIN);
        selected_slider_group = &slider_group_cell_voltage;
        break;

    case ROW_CALIBRATION_OFFSET:
        app_state_push(APP_STATE_SUBMENU_ITEM_FOCUSED);
        lv_obj_add_style(slider_group_calibration_offset.slider, &style_silder_select, LV_PART_MAIN);
        selected_slider_group = &slider_group_calibration_offset;
        break;

    case ROW_OSD_DISPLAY_MODE:
        btn_group_toggle_sel(&btn_group_osd_display_mode);
        g_setting.power.osd_display_mode = btn_group_get_sel(&btn_group_osd_display_mode);
        ini_putl("power", "osd_display_mode", g_setting.power.osd_display_mode, SETTING_INI);
        break;

    case ROW_WARN_TYPE:
        btn_group_toggle_sel(&btn_group_warn_type);
        g_setting.power.warning_type = btn_group_get_sel(&btn_group_warn_type);
        ini_putl("power", "warning_type", g_setting.power.warning_type, SETTING_INI);
        break;

    case ROW_POWER_ANA:
        // Batch 2 goggles only
        if (getHwRevision() >= HW_REV_2) {
#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
            btn_group_toggle_sel(&btn_group_power_ana);
            g_setting.power.power_ana = btn_group_get_sel(&btn_group_power_ana);
            ini_putl("power", "power_ana_rx", g_setting.power.power_ana, SETTING_INI);
            Analog_Module_Power(1);
#endif
        }
        break;

    default:
        break;
    }
}

page_pack_t pp_power = {
    .p_arr = {
        .cur = 0,
        .max = ROW_COUNT,
    },
    .name = "Power",
    .create = page_power_create,
    .enter = NULL,
    .exit = page_power_exit,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = page_power_on_roller,
    .on_click = page_power_on_click,
    .on_right_button = NULL,
};

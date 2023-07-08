#include "page_power.h"

#include <stdio.h>

#include <log/log.h>
#include <minIni.h>

#include "core/app_state.h"
#include "core/battery.h"
#include "core/common.hh"
#include "core/settings.h"
#include "driver/dm5680.h"
#include "driver/hardware.h"
#include "driver/mcp3021.h"
#include "page_common.h"
#include "ui/ui_style.h"

#define WARNING_CELL_VOLTAGE_MIN 28
#define WARNING_CELL_VOLTAGE_MAX 42

enum {
    ROW_BATT_C_LABEL = 0,
    ROW_CELL_COUNT_MODE,
    ROW_CELL_COUNT,
    ROW_WARNING_CELL_VOLTAGE,
    ROW_OSD_DISPLAY_MODE,
    ROW_WARN_TYPE,
    ROW_POWER_ANA,
    ROW_BACK,

    ROW_COUNT
};

static slider_group_t slider_group_cell_voltage;
static btn_group_t btn_group_cell_count_mode;
static slider_group_t slider_group_cell_count;
static btn_group_t btn_group_osd_display_mode;
static btn_group_t btn_group_warn_type;
static btn_group_t btn_group_power_ana;

static lv_coord_t col_dsc[] = {160, 200, 160, 160, 120, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};
lv_obj_t *label_cell_count;

static void page_power_update_cell_count() {
    char str[10];

    // re-trigger cell detection
    battery_init();

    LOGI("cell_count:%d", g_battery.type);
    ini_putl("power", "cell_count", g_battery.type, SETTING_INI);

    sprintf(str, "%dS", g_battery.type);
    lv_label_set_text(label_cell_count, str);

    lv_slider_set_value(slider_group_cell_count.slider, g_battery.type, LV_ANIM_OFF);
    char buf[5];
    sprintf(buf, "%d", g_battery.type);
    lv_label_set_text(slider_group_cell_count.label, buf);
}

static lv_obj_t *page_power_create(lv_obj_t *parent, panel_arr_t *arr) {
    // Update number of rows based on Batch 2 vs Batch 1 options
    pp_power.p_arr.max = getHwRevision() >= HW_REV_2 ? ROW_COUNT : ROW_COUNT - 1;

    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Power:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    // create menu entries
    create_label_item(cont, "Battery", 1, ROW_BATT_C_LABEL, 1);
    label_cell_count = create_label_item(cont, "-S", 2, ROW_BATT_C_LABEL, 1);
    create_btn_group_item(&btn_group_cell_count_mode, cont, 2, "Cell Count Mode", "Auto", "Manual", "", "", ROW_CELL_COUNT_MODE);
    create_slider_item(&slider_group_cell_count, cont, "Cell Count", CELL_MAX_COUNT, g_setting.power.cell_count, ROW_CELL_COUNT);
    create_slider_item(&slider_group_cell_voltage, cont, "Warning Cell Voltage", WARNING_CELL_VOLTAGE_MAX, g_setting.power.voltage, ROW_WARNING_CELL_VOLTAGE);
    create_btn_group_item(&btn_group_osd_display_mode, cont, 2, "Display Mode", "Total", "Cell Avg.", "", "", ROW_OSD_DISPLAY_MODE);
    create_btn_group_item(&btn_group_warn_type, cont, 3, "Warning Type", "Beep", "Visual", "Both", "", ROW_WARN_TYPE);

    // Batch 2 goggles only
    if (getHwRevision() >= HW_REV_2) {
        create_btn_group_item(&btn_group_power_ana, cont, 2, "AnalogRX Power", "On", "Auto", "", "", ROW_POWER_ANA);
    }

    // Back entry
    create_label_item(cont, "< Back", 1, pp_power.p_arr.max - 1, 1);

    // set menu entry min/max values and labels
    char str[5];
    sprintf(str, "%d.%d", g_setting.power.voltage / 10, g_setting.power.voltage % 10);
    lv_slider_set_range(slider_group_cell_voltage.slider, WARNING_CELL_VOLTAGE_MIN, WARNING_CELL_VOLTAGE_MAX);
    lv_label_set_text(slider_group_cell_voltage.label, str);

    sprintf(str, "%d", g_setting.power.cell_count);
    lv_slider_set_range(slider_group_cell_count.slider, CELL_MIN_COUNT, CELL_MAX_COUNT);
    lv_label_set_text(slider_group_cell_count.label, str);

    // set menu entry current values, loaded from stored settings
    btn_group_set_sel(&btn_group_cell_count_mode, g_setting.power.cell_count_mode);
    lv_slider_set_value(slider_group_cell_count.slider, g_setting.power.cell_count, LV_ANIM_OFF);
    lv_slider_set_value(slider_group_cell_voltage.slider, g_setting.power.voltage, LV_ANIM_OFF);
    btn_group_set_sel(&btn_group_osd_display_mode, g_setting.power.osd_display_mode);
    btn_group_set_sel(&btn_group_warn_type, g_setting.power.warning_type);
    btn_group_set_sel(&btn_group_power_ana, g_setting.power.power_ana);

    page_power_update_cell_count();

    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, "*Cell count setting is disabled in auto mode");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label, lv_color_make(255, 255, 255), 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_pad_top(label, 12, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 1, 4,
                         LV_GRID_ALIGN_START, pp_power.p_arr.max, 3);

    return page;
}

void power_cell_count_inc(void) {
    int32_t value = 0;

    value = lv_slider_get_value(slider_group_cell_count.slider);
    if (value < CELL_MAX_COUNT)
        value += 1;
    g_setting.power.cell_count = value;

    page_power_update_cell_count();
}

void power_cell_count_dec(void) {
    int32_t value = 0;

    value = lv_slider_get_value(slider_group_cell_count.slider);
    if (value > CELL_MIN_COUNT)
        value -= 1;

    g_setting.power.cell_count = value;

    page_power_update_cell_count();
}

void power_warning_voltage_inc(void) {
    int32_t value = 0;

    value = lv_slider_get_value(slider_group_cell_voltage.slider);
    if (value < WARNING_CELL_VOLTAGE_MAX)
        value += 1;

    lv_slider_set_value(slider_group_cell_voltage.slider, value, LV_ANIM_OFF);

    char buf[5];
    sprintf(buf, "%d.%d", value / 10, value % 10);
    lv_label_set_text(slider_group_cell_voltage.label, buf);

    g_setting.power.voltage = value;
    LOGI("vol:%d", g_setting.power.voltage);
    ini_putl("power", "voltage", g_setting.power.voltage, SETTING_INI);
}

void power_warning_voltage_dec(void) {
    int32_t value = 0;

    value = lv_slider_get_value(slider_group_cell_voltage.slider);
    if (value > WARNING_CELL_VOLTAGE_MIN)
        value -= 1;

    lv_slider_set_value(slider_group_cell_voltage.slider, value, LV_ANIM_OFF);
    char buf[5];
    sprintf(buf, "%d.%d", value / 10, value % 10);
    lv_label_set_text(slider_group_cell_voltage.label, buf);

    g_setting.power.voltage = value;
    LOGI("vol:%d", g_setting.power.voltage);
    ini_putl("power", "voltage", g_setting.power.voltage, SETTING_INI);
}

static void page_power_on_click(uint8_t key, int sel) {

    switch (sel) {

    case ROW_CELL_COUNT_MODE:
        btn_group_toggle_sel(&btn_group_cell_count_mode);
        g_setting.power.cell_count_mode = btn_group_get_sel(&btn_group_cell_count_mode);
        ini_putl("power", "cell_count_mode", g_setting.power.cell_count_mode, SETTING_INI);

        page_power_update_cell_count();
        break;

    case ROW_CELL_COUNT:
        if (g_setting.power.cell_count_mode == SETTING_POWER_CELL_COUNT_MODE_AUTO)
            ;
        else if (g_app_state == PAGE_POWER_SLIDE_CELL_COUNT) {
            app_state_push(APP_STATE_SUBMENU);
            lv_obj_add_style(slider_group_cell_count.slider, &style_silder_main, LV_PART_MAIN);
        } else {
            app_state_push(PAGE_POWER_SLIDE_CELL_COUNT);
            lv_obj_add_style(slider_group_cell_count.slider, &style_silder_select, LV_PART_MAIN);
        }
        break;

    case ROW_WARNING_CELL_VOLTAGE:
        if (g_app_state == PAGE_POWER_SLIDE_WARNING_CELL_VOLTAGE) {
            app_state_push(APP_STATE_SUBMENU);
            lv_obj_add_style(slider_group_cell_voltage.slider, &style_silder_main, LV_PART_MAIN);
        } else {
            app_state_push(PAGE_POWER_SLIDE_WARNING_CELL_VOLTAGE);
            lv_obj_add_style(slider_group_cell_voltage.slider, &style_silder_select, LV_PART_MAIN);
        }
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
            btn_group_toggle_sel(&btn_group_power_ana);
            g_setting.power.power_ana = btn_group_get_sel(&btn_group_power_ana);
            ini_putl("power", "power_ana_rx", g_setting.power.power_ana, SETTING_INI);
            Analog_Module_Power(1); 
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
    .exit = NULL,
    .on_roller = NULL,
    .on_click = page_power_on_click,
    .on_right_button = NULL,
};

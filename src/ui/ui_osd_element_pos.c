#include "ui/ui_osd_element_pos.h"

#include <stdio.h>

#include <log/log.h>
#include <lvgl/lvgl.h>
#include <minIni.h>

#include "core/common.hh"
#include "core/osd.h"
#include "driver/hardware.h"
#include "driver/oled.h"
#include "log/log.h"
#include "ui/page_common.h"
#include "ui/page_osd.h"
#include "util/math.h"

#define CANVAS_WIDTH  445
#define CANVAS_HEIGHT 365

#define OSD_ELEMENT_MIN_X_POS 0
#define OSD_ELEMENT_MAX_X_POS 1280
#define OSD_ELEMENT_MIN_Y_POS 0
#define OSD_ELEMENT_MAX_Y_POS 720

// used for slider scroll acceleration
#define SLIDER_SCROLL_MIN_INTERVAL  10  // min time interval between scrolls (ms)
#define SLIDER_SCROLL_MAX_INTERVAL  100 // max time interval between scrolls (ms)
#define SLIDER_SCROLL_MIN_STEP_SIZE 1
#define SLIDER_SCROLL_MAX_STEP_SIZE 10

enum {
    ROW_OSD_MODE = 0,
    ROW_OSD_ELEMENT,
    ROW_OSD_SHOW_ELEMENT,
    ROW_OSD_ELEMENT_POS_X,
    ROW_OSD_ELEMENT_POS_Y,
    ROW_SAVE,
    ROW_CANCEL,
    ROW_RESET_ELEMENTS,

    ROW_COUNT
};

typedef enum {
    UI_STATE_SCROLL = 0,
    UI_STATE_FOCUSED
} ui_state_t;

typedef enum {
    CONFIRMATION_UNCONFIRMED = 0,
    CONFIRMATION_CONFIRMED,
    CONFIRMATION_TIMEOUT
} ui_confirmation_t;

// a list of all osd elements
// must contain the same elements, in the same order, as osd_element_list[]
typedef enum {
    OSD_ELEMENT_TOP_FAN_SPEED = 0,
    OSD_ELEMENT_LATENCY_LOCK,
    OSD_ELEMENT_VTX_TEMP,
    OSD_ELEMENT_VRX_TEMP,
    OSD_ELEMENT_BATTERY_LOW,
    OSD_ELEMENT_BATTERY_VOLTAGE,
    OSD_ELEMENT_CHANNEL,
    OSD_ELEMENT_SD_REC,
    OSD_ELEMENT_VLQ,
    OSD_ELEMENT_ANT0,
    OSD_ELEMENT_ANT1,
    OSD_ELEMENT_ANT2,
    OSD_ELEMENT_ANT3,
    OSD_ELEMENT_GOGGLE_TEMP_TOP,
    OSD_ELEMENT_GOGGLE_TEMP_LEFT,
    OSD_ELEMENT_GOGGLE_TEMP_RIGHT,

    OSD_ELEMENTS_TOTAL
} osd_elements_t;

typedef struct {
    char *name;
    char *name_settings;
} osd_element_t;

// sets if the ui is shown during the next ui update (ui_osd_element_pos_update())
static bool show_osd_element_pos_ui = false;

// elements making up the ui
static lv_coord_t col_dsc[] = {10, 95, 120, 120, 60, 30, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {40, 40, 40, 40, 40, 30, 30, 30, 30, LV_GRID_TEMPLATE_LAST};
static panel_arr_t ui_selection_panel = {.cur = 0, .max = ROW_COUNT};
static lv_obj_t *ui_root_container;
static btn_group_t btn_group_osd_mode;
static lv_obj_t *dropdown_osd_element;
static btn_group_t btn_group_osd_show_element;
static slider_group_t slider_group_osd_element_pos_x;
static slider_group_t slider_group_osd_element_pos_y;
static lv_obj_t *label_save_osd_elements;
static lv_obj_t *label_cancel_osd_elements;
static lv_obj_t *label_reset_all_osd_elements;

// to keep track of the current state of the ui
static int ui_state;
static int ui_cur_row;

// used for action confirmations
static lv_timer_t *reset_all_osd_elements_timer = NULL;
static int reset_all_elements_confirm = CONFIRMATION_UNCONFIRMED;
static int save_changes_confirm = CONFIRMATION_UNCONFIRMED;
static int cancel_changes_confirm = CONFIRMATION_UNCONFIRMED;

// used for slider scroll acceleration
static uint32_t slider_scroll_last_tick = 0;

// keeps track of the unchanged element settings in case user clicks cancel
static setting_osd_t ui_osd_el_pos_unchanged_settings;

// all elements with a display string and the corresponding setting name
// this must contain the same elements, in the same order as osd_goggle_element_e in settings.h
static osd_element_t osd_element_list[] = {
    {"Top Fan Speed", "topfan_speed"},
    {"Latency Lock", "latency_lock"},
    {"VTX Temp", "vtx_temp"},
    {"VRX Temp", "vrx_temp"},
    {"Battery Low", "battery_low"},
    {"Battery Voltage", "battery_voltage"},
    {"Channel", "channel"},
    {"DVR", "sd_rec"},
    {"VLQ", "vlq"},
    {"Antenna 1", "ant0"},
    {"Antenna 2", "ant1"},
    {"Antenna 3", "ant2"},
    {"Antenna 4", "ant3"},
    {"Goggle Temp Top", "goggle_temp_top"},
    {"Goggle Temp Left", "goggle_temp_left"},
    {"Goggle Temp Right", "goggle_temp_right"}};

// string used for the dropdown menu
static char osd_elements_str[512];

// utility functions
static setting_osd_goggle_element_t *get_osd_element_setting_entry(int element_idx);
static void fill_osd_elements_str();
static setting_osd_goggle_element_t *get_selected_osd_element_setting_entry();
static int persist_all_osd_element_settings(const setting_osd_t *settings_to_persist);

// ui utility functions
static void update_ui();
static void ui_set_selection(int row);

// used for user action confirmations like save/cancel/reset
static void reset_all_osd_elements_reset_label_text();
static void save_osd_elements_reset_label_text();
static void cancel_osd_elements_reset_label_text();
static void reset_all_osd_elements_timer_cb(struct _lv_timer_t *timer);

// handle slider acceleration when positioning elements
static int slider_scroll_speed_get_step();

// user input handling
static void osd_element_pos_x_dec();
static void osd_element_pos_y_dec();
static void osd_element_pos_x_inc();
static void osd_element_pos_y_inc();
static void ui_handle_roll_up();
static void ui_handle_roll_down();
static int ui_handle_click();

static setting_osd_goggle_element_t *get_osd_element_setting_entry(int element_idx) {
    return &g_setting.osd.element[element_idx];
}

// creates the string used for the dropdown menu
static void fill_osd_elements_str() {
    int max_element = OSD_ELEMENT_ANT3;

    if (g_setting.storage.selftest) {
        max_element = OSD_ELEMENT_GOGGLE_TEMP_RIGHT;
    }

    osd_elements_str[0] = '\0';
    for (int idx = 0; idx < max_element; idx++) {
        strcat(osd_elements_str, osd_element_list[idx].name);
        strcat(osd_elements_str, "\n");
    }
    strcat(osd_elements_str, osd_element_list[max_element].name);
}

static setting_osd_goggle_element_t *get_selected_osd_element_setting_entry() {
    int element_idx = lv_dropdown_get_selected(dropdown_osd_element);
    return get_osd_element_setting_entry(element_idx);
}

static int persist_all_osd_element_settings(const setting_osd_t *settings_to_persist) {
    int res = 1;

    res = ini_putl("osd", "embedded_mode", settings_to_persist->embedded_mode, SETTING_INI);

    for (int i = 0; i < OSD_ELEMENTS_TOTAL; i++)
        res &= settings_put_osd_element(&settings_to_persist->element[i], osd_element_list[i].name_settings);

    return res;
}

static void update_ui() {
    const setting_osd_goggle_element_t *element = get_selected_osd_element_setting_entry();

    btn_group_set_sel(&btn_group_osd_mode, g_setting.osd.embedded_mode);

    btn_group_set_sel(&btn_group_osd_show_element, !element->show);
    if (g_setting.osd.embedded_mode == EMBEDDED_4x3) {
        update_slider_item_with_value(&slider_group_osd_element_pos_x, element->position.mode_4_3.x);
        update_slider_item_with_value(&slider_group_osd_element_pos_y, element->position.mode_4_3.y);
    } else {
        update_slider_item_with_value(&slider_group_osd_element_pos_x, element->position.mode_16_9.x);
        update_slider_item_with_value(&slider_group_osd_element_pos_y, element->position.mode_16_9.y);
    }
}

static void ui_set_selection(int row) {
    set_select_item(&ui_selection_panel, row);
}

static void reset_all_osd_elements_reset_label_text() {
    lv_label_set_text(label_reset_all_osd_elements, "Reset all elements (both modes)");
    reset_all_elements_confirm = CONFIRMATION_UNCONFIRMED;
}

static void save_osd_elements_reset_label_text() {
    lv_label_set_text(label_save_osd_elements, "Save changes");
    lv_obj_set_style_text_font(label_save_osd_elements, &lv_font_montserrat_20, 0);
    save_changes_confirm = CONFIRMATION_UNCONFIRMED;
}

static void cancel_osd_elements_reset_label_text() {
    lv_label_set_text(label_cancel_osd_elements, "Cancel");
    lv_obj_set_style_text_font(label_cancel_osd_elements, &lv_font_montserrat_20, 0);
    cancel_changes_confirm = CONFIRMATION_UNCONFIRMED;
}

static void reset_all_osd_elements_timer_cb(struct _lv_timer_t *timer) {
    lv_timer_pause(reset_all_osd_elements_timer);
    reset_all_osd_elements_reset_label_text();
    reset_all_elements_confirm = CONFIRMATION_UNCONFIRMED;
}

static int slider_scroll_speed_get_step() {
    uint32_t interval = lv_tick_elaps(slider_scroll_last_tick);
    slider_scroll_last_tick = lv_tick_get();

    if (interval <= SLIDER_SCROLL_MIN_INTERVAL) {
        return SLIDER_SCROLL_MAX_STEP_SIZE;
    } else if (interval >= SLIDER_SCROLL_MAX_INTERVAL) {
        return SLIDER_SCROLL_MIN_STEP_SIZE;
    }

    float factor = (float)(interval - SLIDER_SCROLL_MIN_INTERVAL) / (SLIDER_SCROLL_MAX_INTERVAL - SLIDER_SCROLL_MIN_INTERVAL);
    int step = (int)((1.0 - factor) * (SLIDER_SCROLL_MAX_STEP_SIZE - SLIDER_SCROLL_MIN_STEP_SIZE));

    return step > 0 ? step : 1;
}

static void osd_element_pos_x_dec() {
    setting_osd_goggle_element_t *element = get_selected_osd_element_setting_entry();

    if (g_setting.osd.embedded_mode == EMBEDDED_4x3) {
        safe_update_value(OSD_ELEMENT_MIN_X_POS, OSD_ELEMENT_MAX_X_POS,
                          &element->position.mode_4_3.x,
                          -(slider_scroll_speed_get_step()));
    } else {
        safe_update_value(OSD_ELEMENT_MIN_X_POS, OSD_ELEMENT_MAX_X_POS,
                          &element->position.mode_16_9.x,
                          -(slider_scroll_speed_get_step()));
    }
}

static void osd_element_pos_y_dec() {
    setting_osd_goggle_element_t *element = get_selected_osd_element_setting_entry();

    if (g_setting.osd.embedded_mode == EMBEDDED_4x3) {
        safe_update_value(OSD_ELEMENT_MIN_Y_POS, OSD_ELEMENT_MAX_Y_POS,
                          &element->position.mode_4_3.y,
                          -(slider_scroll_speed_get_step()));
    } else {
        safe_update_value(OSD_ELEMENT_MIN_Y_POS, OSD_ELEMENT_MAX_Y_POS,
                          &element->position.mode_16_9.y,
                          -(slider_scroll_speed_get_step()));
    }
}

static void osd_element_pos_x_inc() {
    setting_osd_goggle_element_t *element = get_selected_osd_element_setting_entry();

    if (g_setting.osd.embedded_mode == EMBEDDED_4x3) {
        safe_update_value(OSD_ELEMENT_MIN_X_POS, OSD_ELEMENT_MAX_X_POS,
                          &element->position.mode_4_3.x,
                          (slider_scroll_speed_get_step()));
    } else {
        safe_update_value(OSD_ELEMENT_MIN_X_POS, OSD_ELEMENT_MAX_X_POS,
                          &element->position.mode_16_9.x,
                          (slider_scroll_speed_get_step()));
    }
}

static void osd_element_pos_y_inc() {
    setting_osd_goggle_element_t *element = get_selected_osd_element_setting_entry();

    if (g_setting.osd.embedded_mode == EMBEDDED_4x3) {
        safe_update_value(OSD_ELEMENT_MIN_Y_POS, OSD_ELEMENT_MAX_Y_POS,
                          &element->position.mode_4_3.y,
                          (slider_scroll_speed_get_step()));
    } else {
        safe_update_value(OSD_ELEMENT_MIN_Y_POS, OSD_ELEMENT_MAX_Y_POS,
                          &element->position.mode_16_9.y,
                          (slider_scroll_speed_get_step()));
    }
}

static void ui_handle_roll_up() {
    // if a confirmation is pending, cancel it
    if (reset_all_elements_confirm == CONFIRMATION_CONFIRMED)
        reset_all_osd_elements_reset_label_text();
    else if (save_changes_confirm == CONFIRMATION_CONFIRMED)
        save_osd_elements_reset_label_text();
    else if (cancel_changes_confirm == CONFIRMATION_CONFIRMED)
        cancel_osd_elements_reset_label_text();

    // if no item is focused, scroll through the rows
    if (ui_state == UI_STATE_SCROLL) {
        ui_cur_row++;
        if (ui_cur_row >= ROW_COUNT)
            ui_cur_row = 0;

        ui_set_selection(ui_cur_row);
        return;
    }

    // if an item is focused, user input is directed at that item
    switch (ui_cur_row) {
    case ROW_OSD_ELEMENT: {
        uint32_t evt = LV_KEY_DOWN;
        lv_event_send(dropdown_osd_element, LV_EVENT_KEY, &evt);
        update_ui();
    } break;

    case ROW_OSD_ELEMENT_POS_X:
        osd_element_pos_x_dec();
        update_ui();
        osd_update_element_positions();
        break;

    case ROW_OSD_ELEMENT_POS_Y:
        osd_element_pos_y_dec();
        update_ui();
        osd_update_element_positions();
        break;

    default:
        break;
    }
}

static void ui_handle_roll_down() {
    // if a confirmation is pending, cancel it
    if (reset_all_elements_confirm == CONFIRMATION_CONFIRMED)
        reset_all_osd_elements_reset_label_text();
    else if (save_changes_confirm == CONFIRMATION_CONFIRMED)
        save_osd_elements_reset_label_text();
    else if (cancel_changes_confirm == CONFIRMATION_CONFIRMED)
        cancel_osd_elements_reset_label_text();

    // if no item is focused, scroll through the rows
    if (ui_state == UI_STATE_SCROLL) {
        ui_cur_row--;
        if (ui_cur_row < 0)
            ui_cur_row = ROW_COUNT - 1;

        ui_set_selection(ui_cur_row);
        return;
    }

    // if an item is focused, user input is directed at that item
    switch (ui_cur_row) {
    case ROW_OSD_ELEMENT: {
        uint32_t evt = LV_KEY_UP;
        lv_event_send(dropdown_osd_element, LV_EVENT_KEY, &evt);
        update_ui();
    } break;

    case ROW_OSD_ELEMENT_POS_X:
        osd_element_pos_x_inc();
        update_ui();
        osd_update_element_positions();
        break;

    case ROW_OSD_ELEMENT_POS_Y:
        osd_element_pos_y_inc();
        update_ui();
        osd_update_element_positions();
        break;

    default:
        break;
    }
}

static int ui_handle_click() {
    // handle click depending on the current row
    switch (ui_cur_row) {
    case ROW_OSD_MODE:
        btn_group_toggle_sel(&btn_group_osd_mode);
        g_setting.osd.embedded_mode = btn_group_get_sel(&btn_group_osd_mode);
        update_ui();
        osd_update_element_positions();
        break;

    case ROW_OSD_ELEMENT:
        if (ui_state != UI_STATE_FOCUSED) {
            ui_state = UI_STATE_FOCUSED;
            lv_obj_t *list = lv_dropdown_get_list(dropdown_osd_element);
            lv_dropdown_open(dropdown_osd_element);
            lv_obj_add_style(list, &style_dropdown, LV_PART_MAIN);
            lv_obj_set_style_text_color(list, lv_color_make(0, 0, 0), LV_PART_SELECTED | LV_STATE_CHECKED);
        } else {
            ui_state = UI_STATE_SCROLL;
            lv_event_send(dropdown_osd_element, LV_EVENT_RELEASED, NULL);
        }

        break;

    case ROW_OSD_SHOW_ELEMENT:
        btn_group_toggle_sel(&btn_group_osd_show_element);
        setting_osd_goggle_element_t *element = get_selected_osd_element_setting_entry();
        element->show = btn_group_get_sel(&btn_group_osd_show_element) == 0;
        break;

    case ROW_OSD_ELEMENT_POS_X:
        if (ui_state != UI_STATE_FOCUSED) {
            ui_state = UI_STATE_FOCUSED;
            lv_obj_add_style(slider_group_osd_element_pos_x.slider, &style_silder_select, LV_PART_MAIN);
        } else {
            ui_state = UI_STATE_SCROLL;
            lv_obj_add_style(slider_group_osd_element_pos_x.slider, &style_silder_main, LV_PART_MAIN);
        }
        break;

    case ROW_OSD_ELEMENT_POS_Y:
        if (ui_state != UI_STATE_FOCUSED) {
            ui_state = UI_STATE_FOCUSED;
            lv_obj_add_style(slider_group_osd_element_pos_y.slider, &style_silder_select, LV_PART_MAIN);
        } else {
            ui_state = UI_STATE_SCROLL;
            lv_obj_add_style(slider_group_osd_element_pos_y.slider, &style_silder_main, LV_PART_MAIN);
        }
        break;

    case ROW_CANCEL:
        if (cancel_changes_confirm) {
            ui_osd_element_pos_cancel_and_hide();
            return 1;
        }

        lv_obj_set_style_text_font(label_cancel_osd_elements, &lv_font_montserrat_18, 0);
        lv_label_set_text(label_cancel_osd_elements, "#FFFF00 click to confirm/scroll to cancel#");
        cancel_changes_confirm = CONFIRMATION_CONFIRMED;
        return 0;

    case ROW_SAVE:
        if (save_changes_confirm) {
            persist_all_osd_element_settings(&g_setting.osd);
            show_osd_element_pos_ui = false;
            save_osd_elements_reset_label_text();

            // some of the changes we made might also be displayed on the osd settings page
            // so we need to make sure to update its ui elements
            page_osd_update_ui_elements();
            return 1;
        }

        lv_obj_set_style_text_font(label_save_osd_elements, &lv_font_montserrat_18, 0);
        lv_label_set_text(label_save_osd_elements, "#FFFF00 click to confirm/scroll to cancel#");
        save_changes_confirm = CONFIRMATION_CONFIRMED;
        return 0;

    case ROW_RESET_ELEMENTS:
        if (reset_all_elements_confirm == CONFIRMATION_TIMEOUT)
            return 0;

        if (reset_all_elements_confirm) {
            for (int i = 0; i < OSD_GOGGLE_NUM; i++) {
                g_setting.osd.element[i] = g_setting_defaults.osd.element[i];
            }

            update_ui();
            osd_update_element_positions();
            lv_label_set_text(label_reset_all_osd_elements, "#00FF00 Elements reset.#");
            lv_timer_reset(reset_all_osd_elements_timer);
            lv_timer_resume(reset_all_osd_elements_timer);
            reset_all_elements_confirm = CONFIRMATION_TIMEOUT;
        } else {
            lv_label_set_text(label_reset_all_osd_elements, "#FFFF00 click to confirm/scroll to cancel#");
            reset_all_elements_confirm = CONFIRMATION_CONFIRMED;
        }
        return 0;

    default:
        break;
    }

    return 0;
}

static void reset_ui() {
    if (ui_state == UI_STATE_FOCUSED) {
        switch (ui_cur_row) {
        case ROW_OSD_ELEMENT:
            lv_event_send(dropdown_osd_element, LV_EVENT_RELEASED, NULL);
            break;
        case ROW_OSD_ELEMENT_POS_X:
            lv_obj_add_style(slider_group_osd_element_pos_x.slider, &style_silder_main, LV_PART_MAIN);
            break;
        case ROW_OSD_ELEMENT_POS_Y:
            lv_obj_add_style(slider_group_osd_element_pos_y.slider, &style_silder_main, LV_PART_MAIN);
            break;

        default:
            break;
        }
    }

    ui_state = UI_STATE_SCROLL;
    ui_cur_row = ROW_OSD_MODE;

    ui_set_selection(ui_cur_row);
    lv_dropdown_set_selected(dropdown_osd_element, 0);

    save_osd_elements_reset_label_text();
    cancel_osd_elements_reset_label_text();
    reset_all_osd_elements_reset_label_text();

    update_ui();
}

void ui_osd_element_pos_init(void) {
    ui_root_container = lv_obj_create(lv_scr_act());
    lv_obj_add_flag(ui_root_container, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_size(ui_root_container, CANVAS_WIDTH, CANVAS_HEIGHT);
    lv_obj_align(ui_root_container, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_obj_set_layout(ui_root_container, LV_LAYOUT_GRID);
    lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(ui_root_container, &style_context, LV_PART_MAIN);
    lv_obj_set_style_grid_column_dsc_array(ui_root_container, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(ui_root_container, row_dsc, 0);
    create_select_item(&ui_selection_panel, ui_root_container);

    // create all elements
    create_btn_group_item_compact(&btn_group_osd_mode, ui_root_container, 2, "Mode", "4x3", "16x9", "", "", ROW_OSD_MODE, 40, 80, &lv_font_montserrat_20);

    create_label_item_compact(ui_root_container, "Element: ", 1, ROW_OSD_ELEMENT, 1, 40, LV_TEXT_ALIGN_LEFT, LV_GRID_ALIGN_START, &lv_font_montserrat_20);
    fill_osd_elements_str();
    dropdown_osd_element = create_dropdown_item(ui_root_container, osd_elements_str, 2, ROW_OSD_ELEMENT, 160, 30, 2, 2, LV_GRID_ALIGN_STRETCH, &lv_font_montserrat_20);

    create_btn_group_item_compact(&btn_group_osd_show_element, ui_root_container, 2, "Show", "Yes", "No", "", "", ROW_OSD_SHOW_ELEMENT, 40, 80, &lv_font_montserrat_20);
    create_slider_item_compact(&slider_group_osd_element_pos_x, ui_root_container, "Pos-X", OSD_ELEMENT_MAX_X_POS, 0, ROW_OSD_ELEMENT_POS_X, &lv_font_montserrat_20);
    create_slider_item_compact(&slider_group_osd_element_pos_y, ui_root_container, "Pos-Y", OSD_ELEMENT_MAX_Y_POS, 0, ROW_OSD_ELEMENT_POS_Y, &lv_font_montserrat_20);
    label_save_osd_elements = create_label_item_compact(ui_root_container, "Save changes", 0, ROW_SAVE, 5, 30, LV_TEXT_ALIGN_CENTER, LV_GRID_ALIGN_CENTER, &lv_font_montserrat_20);
    label_cancel_osd_elements = create_label_item_compact(ui_root_container, "Cancel", 0, ROW_CANCEL, 5, 30, LV_TEXT_ALIGN_CENTER, LV_GRID_ALIGN_CENTER, &lv_font_montserrat_20);
    label_reset_all_osd_elements = create_label_item_compact(ui_root_container, "Reset all elements (both modes)", 0, ROW_RESET_ELEMENTS, 5, 30, LV_TEXT_ALIGN_CENTER, LV_GRID_ALIGN_CENTER, &lv_font_montserrat_18);

    // make the menu semi-transparent
    lv_obj_set_style_bg_opa(ui_root_container, LV_OPA_70, 0);

    // set menu selections
    update_ui();

    // create timers
    reset_all_osd_elements_timer = lv_timer_create(reset_all_osd_elements_timer_cb, 3000, NULL);
    lv_timer_pause(reset_all_osd_elements_timer);

    // init slider scroll acceleration vars
    slider_scroll_last_tick = lv_tick_get();

    // set initial ui state
    ui_cur_row = ROW_OSD_MODE;
    ui_state = UI_STATE_SCROLL;
    ui_set_selection(ui_cur_row);
}

void ui_osd_element_pos_on_enter() {
    ui_osd_el_pos_unchanged_settings = g_setting.osd;

    reset_ui();

    show_osd_element_pos_ui = true;
}

void ui_osd_element_pos_update() {
    if (show_osd_element_pos_ui)
        lv_obj_clear_flag(ui_root_container, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(ui_root_container, LV_OBJ_FLAG_HIDDEN);
}

void ui_osd_element_pos_cancel_and_hide() {
    show_osd_element_pos_ui = false;
    g_setting.osd = ui_osd_el_pos_unchanged_settings;
    reset_ui();

    osd_update_element_positions();
}

int ui_osd_element_pos_handle_input(int key) {
    switch (key) {
    case DIAL_KEY_CLICK:
        return ui_handle_click();

    case DIAL_KEY_UP:
        ui_handle_roll_up();
        return 0;

    case DIAL_KEY_DOWN:
        ui_handle_roll_down();
        return 0;

    default:
        return 0;
    }
}

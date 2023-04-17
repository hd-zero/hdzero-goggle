#include "page_osd.h"

#include <stdio.h>

#include <log/log.h>
#include <minIni.h>

#include "core/app_state.h"
#include "core/common.hh"
#include "core/osd.h"
#include "core/settings.h"
#include "page_common.h"
#include "ui/ui_style.h"
#include "util/math.h"

#define OSD_ELEMENT_MIN_X_POS 0
#define OSD_ELEMENT_MAX_X_POS 1280
#define OSD_ELEMENT_MIN_Y_POS 0
#define OSD_ELEMENT_MAX_Y_POS 720

enum {
    ROW_OSD_MODE = 0,
    ROW_OSD_ELEMENT,
    ROW_OSD_SHOW_ELEMENT,
    ROW_OSD_ELEMENT_POS_X,
    ROW_OSD_ELEMENT_POS_Y,
    ROW_OSD_RESET_ELEMENTS,
    ROW_BACK,

    ROW_COUNT,
    ROW_USER_HINT = 7
};

typedef enum {
    OSD_ELEMENT_TOP_FAN_SPEED = 0,
    OSD_ELEMENT_LATENCY_LOCK,
    OSD_ELEMENT_VTX_TEMP,
    OSD_ELEMENT_VRX_TEMP,
    OSD_ELEMENT_BATTERY_LOW,
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

typedef enum {
    ALL_ELEMENT_RES_UNCONFIRMED = 0,
    ALL_ELEMENT_RES_CONFIRMED,
    ALL_ELEMENT_RES_TIMEOUT
} all_element_reset_confirm_t;

typedef enum {
    ELEMENT_POS_SCROLL_DIR_NONE = 0,
    ELEMENT_POS_SCROLL_DIR_X_INC,
    ELEMENT_POS_SCROLL_DIR_Y_INC,
    ELEMENT_POS_SCROLL_DIR_X_DEC,
    ELEMENT_POS_SCROLL_DIR_Y_DEC
} element_pos_scroll_dir_t;

typedef enum {
    ELEMENT_POS_SCROLL_SPEED_SLOW = 1,
    ELEMENT_POS_SCROLL_SPEED_MED = 5,
    ELEMENT_POS_SCROLL_SPEED_FAST = 20
} element_pos_scroll_speed_t;

typedef enum {
    ELEMENT_POS_SCROLL_SPEED_STEP_MED = 10 * ELEMENT_POS_SCROLL_SPEED_SLOW,
    ELEMENT_POS_SCROLL_SPEED_STEP_FAST = ELEMENT_POS_SCROLL_SPEED_STEP_MED + 10 * ELEMENT_POS_SCROLL_SPEED_MED,
} element_pos_scroll_speed_steps_t;

typedef struct {
    char *name;
    char *name_settings;
} osd_element_t;

static lv_coord_t col_dsc[] = {160, 180, 160, 160, 120, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

static btn_group_t btn_group_osd_type;
static lv_obj_t *dropdown_osd_element;
static btn_group_t btn_group_osd_show_element;
static slider_group_t slider_group_osd_element_pos_x;
static slider_group_t slider_group_osd_element_pos_y;
lv_obj_t *label_reset_all_osd_elements;

static lv_timer_t *reset_all_osd_elements_timer = NULL;

static bool element_dropdown_focused = false;
static bool element_pos_x_slider_focused = false;
static bool element_pos_y_slider_focused = false;
static int reset_all_elements_confirm = 0;

static lv_timer_t *element_pos_scroll_speed_timer = NULL;
static int element_pos_cur_scroll_amount = 0;
static int element_pos_cur_scroll_dir = ELEMENT_POS_SCROLL_DIR_NONE;
static int element_pos_cur_scroll_speed = ELEMENT_POS_SCROLL_SPEED_SLOW;

static char osd_elements_str[512];

static osd_element_t osd_element_list[] = {
    {"Top Fan Speed", "topfan_speed"},
    {"Latency Lock", "latency_lock"},
    {"VTX Temp", "vtx_temp"},
    {"VRX Temp", "vrx_temp"},
    {"Battery Low", "battery_low"},
    {"Channel", "channel"},
    {"SD Rec", "sd_rec"},
    {"VLQ", "vlq"},
    {"Antenna 1", "ant0"},
    {"Antenna 2", "ant1"},
    {"Antenna 3", "ant2"},
    {"Antenna 4", "ant3"},
    {"Goggle Temp Top", "goggle_temp_top"},
    {"Goggle Temp Left", "goggle_temp_left"},
    {"Goggle Temp Right", "goggle_temp_right"}};

static void page_osd_fill_osd_elements_str() {
    int max_element = OSD_ELEMENT_ANT3;

    if (g_test_en) {
        max_element = OSD_ELEMENT_GOGGLE_TEMP_RIGHT;
    }

    osd_elements_str[0] = '\0';
    for (int i = 0; i < max_element; i++) {
        strcat(osd_elements_str, osd_element_list[i].name);
        strcat(osd_elements_str, "\n");
    }
    strcat(osd_elements_str, osd_element_list[max_element].name);
}

static setting_osd_goggle_element_t *get_osd_element_setting_entry(int element_idx) {
    return &g_setting.osd.element[element_idx];

    // switch (element_idx) {
    // case OSD_ELEMENT_TOP_FAN_SPEED:
    //     return &g_setting.osd.elements.topfan_speed;

    // case OSD_ELEMENT_LATENCY_LOCK:
    //     return &g_setting.osd.elements.latency_lock;

    // case OSD_ELEMENT_VTX_TEMP:
    //     return &g_setting.osd.elements.vtx_temp;

    // case OSD_ELEMENT_VRX_TEMP:
    //     return &g_setting.osd.elements.vrx_temp;

    // case OSD_ELEMENT_BATTERY_LOW:
    //     return &g_setting.osd.elements.battery_low;

    // case OSD_ELEMENT_CHANNEL:
    //     return &g_setting.osd.elements.channel;

    // case OSD_ELEMENT_SD_REC:
    //     return &g_setting.osd.elements.sd_rec;

    // case OSD_ELEMENT_VLQ:
    //     return &g_setting.osd.elements.vlq;

    // case OSD_ELEMENT_ANT0:
    //     return &g_setting.osd.elements.ant0;

    // case OSD_ELEMENT_ANT1:
    //     return &g_setting.osd.elements.ant1;

    // case OSD_ELEMENT_ANT2:
    //     return &g_setting.osd.elements.ant2;

    // case OSD_ELEMENT_ANT3:
    //     return &g_setting.osd.elements.ant3;

    // case OSD_ELEMENT_GOGGLE_TEMP_TOP:
    //     return &g_setting.osd.elements.osd_tempe[0];

    // case OSD_ELEMENT_GOGGLE_TEMP_LEFT:
    //     return &g_setting.osd.elements.osd_tempe[1];

    // case OSD_ELEMENT_GOGGLE_TEMP_RIGHT:
    //     return &g_setting.osd.elements.osd_tempe[2];

    // default:
    //     break;
    // }
}

static setting_osd_goggle_element_t *page_osd_get_selected_osd_element_setting_entry() {
    int element_idx = lv_dropdown_get_selected(dropdown_osd_element);
    return get_osd_element_setting_entry(element_idx);
}

static void update_osd_element_ui_items() {
    const setting_osd_goggle_element_t *element = page_osd_get_selected_osd_element_setting_entry();

    btn_group_set_sel(&btn_group_osd_show_element, !element->show);

    if (g_setting.osd.embedded_mode == EMBEDDED_4x3) {
        update_slider_item_with_value(&slider_group_osd_element_pos_x, element->position.mode_4_3.x);
        update_slider_item_with_value(&slider_group_osd_element_pos_y, element->position.mode_4_3.y);
    } else {
        update_slider_item_with_value(&slider_group_osd_element_pos_x, element->position.mode_16_9.x);
        update_slider_item_with_value(&slider_group_osd_element_pos_y, element->position.mode_16_9.y);
    }
}

static void page_osd_reset_all_osd_elements_reset_label() {
    lv_label_set_text(label_reset_all_osd_elements, "Reset all elements");
    reset_all_elements_confirm = ALL_ELEMENT_RES_UNCONFIRMED;
}

static void page_osd_scroll_speed_timer_cb(lv_timer_t *timer) {
    lv_timer_pause(element_pos_scroll_speed_timer);
    element_pos_cur_scroll_dir = ELEMENT_POS_SCROLL_DIR_NONE;
    element_pos_cur_scroll_speed = ELEMENT_POS_SCROLL_SPEED_SLOW;
    element_pos_cur_scroll_amount = 0;
}

static void page_osd_reset_all_osd_elements_timer_cb(struct _lv_timer_t *timer) {
    lv_timer_pause(reset_all_osd_elements_timer);
    page_osd_reset_all_osd_elements_reset_label();
    reset_all_elements_confirm = ALL_ELEMENT_RES_UNCONFIRMED;
}

static lv_obj_t *page_osd_create(lv_obj_t *parent, panel_arr_t *arr) {
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Goggle OSD Settings:", LV_MENU_ITEM_BUILDER_VARIANT_2);

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
    create_btn_group_item(&btn_group_osd_type, cont, 2, "OSD Mode", "4x3", "16x9", "", "", ROW_OSD_MODE);

    create_label_item(cont, "Element: ", 1, ROW_OSD_ELEMENT, 1);
    page_osd_fill_osd_elements_str();
    dropdown_osd_element = create_dropdown_item(cont, osd_elements_str, 2, ROW_OSD_ELEMENT);
    lv_obj_set_width(dropdown_osd_element, 320);

    create_btn_group_item(&btn_group_osd_show_element, cont, 2, "Show Element", "Yes", "No", "", "", ROW_OSD_SHOW_ELEMENT);
    create_slider_item(&slider_group_osd_element_pos_x, cont, "Element Pos-X", OSD_ELEMENT_MAX_X_POS, 0, ROW_OSD_ELEMENT_POS_X);
    create_slider_item(&slider_group_osd_element_pos_y, cont, "Element Pos-Y", OSD_ELEMENT_MAX_Y_POS, 0, ROW_OSD_ELEMENT_POS_Y);

    label_reset_all_osd_elements = create_label_item(cont, "Reset all elements", 1, ROW_OSD_RESET_ELEMENTS, 1);
    lv_obj_set_width(label_reset_all_osd_elements, 600);

    lv_obj_t *label_user_hint = lv_label_create(cont);
    lv_label_set_text(label_user_hint, "OSD Element positioning is based on a 1280x720 canvas.\nPositions can be set for 4x3 and 16x9 modes separately,\nthe Show Element toggle is shared between both modes.");
    lv_obj_set_style_text_font(label_user_hint, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(label_user_hint, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(label_user_hint, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_pad_top(label_user_hint, 12, 0);
    lv_label_set_long_mode(label_user_hint, LV_LABEL_LONG_WRAP);
    lv_obj_set_grid_cell(label_user_hint, LV_GRID_ALIGN_START, 1, 4,
                         LV_GRID_ALIGN_START, ROW_USER_HINT, 2);

    // Back entry
    create_label_item(cont, "< Back", 1, ROW_BACK, 1);

    // set menu selections
    btn_group_set_sel(&btn_group_osd_type, g_setting.osd.embedded_mode);
    update_osd_element_ui_items();

    // create timers
    element_pos_scroll_speed_timer = lv_timer_create(page_osd_scroll_speed_timer_cb, 1000, NULL);
    lv_timer_pause(element_pos_scroll_speed_timer);
    reset_all_osd_elements_timer = lv_timer_create(page_osd_reset_all_osd_elements_timer_cb, 3000, NULL);
    lv_timer_pause(reset_all_osd_elements_timer);

    return page;
}

static int page_osd_reset_all_osd_elements() {
    int res = 1;

    for (int i = 0; i < OSD_ELEMENTS_TOTAL; i++) {
        g_setting.osd.element[i] = g_setting_defaults.osd.element[i];
        res &= settings_put_osd_element(&g_setting.osd.element[i], osd_element_list[i].name_settings);
    }

    // res = settings_put_osd_element(&g_setting.osd.elements.topfan_speed, osd_element_list[OSD_ELEMENT_TOP_FAN_SPEED].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.latency_lock, osd_element_list[OSD_ELEMENT_LATENCY_LOCK].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.vtx_temp, osd_element_list[OSD_ELEMENT_VTX_TEMP].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.vrx_temp, osd_element_list[OSD_ELEMENT_VRX_TEMP].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.battery_low, osd_element_list[OSD_ELEMENT_BATTERY_LOW].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.channel, osd_element_list[OSD_ELEMENT_CHANNEL].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.sd_rec, osd_element_list[OSD_ELEMENT_SD_REC].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.vlq, osd_element_list[OSD_ELEMENT_VLQ].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.ant0, osd_element_list[OSD_ELEMENT_ANT0].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.ant1, osd_element_list[OSD_ELEMENT_ANT1].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.ant2, osd_element_list[OSD_ELEMENT_ANT2].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.ant3, osd_element_list[OSD_ELEMENT_ANT3].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.osd_tempe[0], osd_element_list[OSD_ELEMENT_GOGGLE_TEMP_TOP].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.osd_tempe[1], osd_element_list[OSD_ELEMENT_GOGGLE_TEMP_LEFT].name_settings);
    // res &= settings_put_osd_element(&g_setting.osd.elements.osd_tempe[2], osd_element_list[OSD_ELEMENT_GOGGLE_TEMP_RIGHT].name_settings);

    return res;
}

static char *page_osd_get_selected_osd_element_setting_name() {
    return osd_element_list[lv_dropdown_get_selected(dropdown_osd_element)].name_settings;
}

static void page_osd_slider_scroll_acceleration(element_pos_scroll_dir_t direction) {
    if (element_pos_cur_scroll_dir != direction) {
        element_pos_cur_scroll_dir = direction;
        element_pos_cur_scroll_speed = ELEMENT_POS_SCROLL_SPEED_SLOW;
        element_pos_cur_scroll_amount = 0;

        lv_timer_reset(element_pos_scroll_speed_timer);
        lv_timer_resume(element_pos_scroll_speed_timer);
    } else {
        lv_timer_reset(element_pos_scroll_speed_timer);
        if (element_pos_cur_scroll_amount >= ELEMENT_POS_SCROLL_SPEED_STEP_FAST) {
            element_pos_cur_scroll_speed = ELEMENT_POS_SCROLL_SPEED_FAST;
        } else if (element_pos_cur_scroll_amount >= ELEMENT_POS_SCROLL_SPEED_STEP_MED) {
            element_pos_cur_scroll_speed = ELEMENT_POS_SCROLL_SPEED_MED;
        }
    }

    element_pos_cur_scroll_amount += element_pos_cur_scroll_speed;
}

static void page_osd_element_pos_x_dec() {
    setting_osd_goggle_element_t *element = page_osd_get_selected_osd_element_setting_entry();

    page_osd_slider_scroll_acceleration(ELEMENT_POS_SCROLL_DIR_X_DEC);

    if (g_setting.osd.embedded_mode == EMBEDDED_4x3) {
        safe_update_value(OSD_ELEMENT_MIN_X_POS, OSD_ELEMENT_MAX_X_POS,
                          &element->position.mode_4_3.x,
                          (-element_pos_cur_scroll_speed));
    } else {
        safe_update_value(OSD_ELEMENT_MIN_X_POS, OSD_ELEMENT_MAX_X_POS,
                          &element->position.mode_16_9.x,
                          (-element_pos_cur_scroll_speed));
    }
}

static void page_osd_element_pos_y_dec() {
    setting_osd_goggle_element_t *element = page_osd_get_selected_osd_element_setting_entry();

    page_osd_slider_scroll_acceleration(ELEMENT_POS_SCROLL_DIR_Y_DEC);

    if (g_setting.osd.embedded_mode == EMBEDDED_4x3) {
        safe_update_value(OSD_ELEMENT_MIN_Y_POS, OSD_ELEMENT_MAX_Y_POS,
                          &element->position.mode_4_3.y,
                          (-element_pos_cur_scroll_speed));
    } else {
        safe_update_value(OSD_ELEMENT_MIN_Y_POS, OSD_ELEMENT_MAX_Y_POS,
                          &element->position.mode_16_9.y,
                          (-element_pos_cur_scroll_speed));
    }
}

static void page_osd_element_pos_x_inc() {
    setting_osd_goggle_element_t *element = page_osd_get_selected_osd_element_setting_entry();

    page_osd_slider_scroll_acceleration(ELEMENT_POS_SCROLL_DIR_X_INC);

    if (g_setting.osd.embedded_mode == EMBEDDED_4x3) {
        safe_update_value(OSD_ELEMENT_MIN_X_POS, OSD_ELEMENT_MAX_X_POS,
                          &element->position.mode_4_3.x,
                          element_pos_cur_scroll_speed);
    } else {
        safe_update_value(OSD_ELEMENT_MIN_X_POS, OSD_ELEMENT_MAX_X_POS,
                          &element->position.mode_16_9.x,
                          element_pos_cur_scroll_speed);
    }
}

static void page_osd_element_pos_y_inc() {
    setting_osd_goggle_element_t *element = page_osd_get_selected_osd_element_setting_entry();

    page_osd_slider_scroll_acceleration(ELEMENT_POS_SCROLL_DIR_Y_INC);

    if (g_setting.osd.embedded_mode == EMBEDDED_4x3) {
        safe_update_value(OSD_ELEMENT_MIN_Y_POS, OSD_ELEMENT_MAX_Y_POS,
                          &element->position.mode_4_3.y,
                          element_pos_cur_scroll_speed);
    } else {
        safe_update_value(OSD_ELEMENT_MIN_Y_POS, OSD_ELEMENT_MAX_Y_POS,
                          &element->position.mode_16_9.y,
                          element_pos_cur_scroll_speed);
    }
}

static void page_osd_on_roller(uint8_t key) {
    if (reset_all_elements_confirm == ALL_ELEMENT_RES_CONFIRMED) {
        page_osd_reset_all_osd_elements_reset_label();
        return;
    }

    if (key == DIAL_KEY_UP) {

        if (element_dropdown_focused) {
            uint32_t evt = LV_KEY_DOWN;
            lv_event_send(dropdown_osd_element, LV_EVENT_KEY, &evt);
        } else if (element_pos_x_slider_focused) {
            page_osd_element_pos_x_dec();
        } else if (element_pos_y_slider_focused) {
            page_osd_element_pos_y_dec();
        }

    } else if (key == DIAL_KEY_DOWN) {

        if (element_dropdown_focused) {
            uint32_t evt = LV_KEY_UP;
            lv_event_send(dropdown_osd_element, LV_EVENT_KEY, &evt);
        } else if (element_pos_x_slider_focused) {
            page_osd_element_pos_x_inc();
        } else if (element_pos_y_slider_focused) {
            page_osd_element_pos_y_inc();
        }
    }

    update_osd_element_ui_items();
}

static void page_osd_on_click(uint8_t key, int sel) {

    switch (sel) {

    case ROW_OSD_MODE:
        btn_group_toggle_sel(&btn_group_osd_type);
        g_setting.osd.embedded_mode = btn_group_get_sel(&btn_group_osd_type);
        ini_putl("osd", "embedded_mode", g_setting.osd.embedded_mode, SETTING_INI);
        osd_update_mode();
        break;

    case ROW_OSD_ELEMENT:
        if (!element_dropdown_focused) {
            app_state_push(APP_STATE_SUBMENU_ITEM_FOCUSED);
            lv_obj_t *list = lv_dropdown_get_list(dropdown_osd_element);
            lv_dropdown_open(dropdown_osd_element);
            lv_obj_add_style(list, &style_dropdown, LV_PART_MAIN);
            lv_obj_set_style_text_color(list, lv_color_make(0, 0, 0), LV_PART_SELECTED | LV_STATE_CHECKED);
            element_dropdown_focused = true;
        } else {
            app_state_push(APP_STATE_SUBMENU);
            lv_event_send(dropdown_osd_element, LV_EVENT_RELEASED, NULL);
            element_dropdown_focused = false;
            int option = lv_dropdown_get_selected(dropdown_osd_element);
        }
        break;

    case ROW_OSD_SHOW_ELEMENT:
        btn_group_toggle_sel(&btn_group_osd_show_element);
        setting_osd_goggle_element_t *element = page_osd_get_selected_osd_element_setting_entry();
        element->show = btn_group_get_sel(&btn_group_osd_show_element) == 0;
        settings_put_osd_element_shown(element->show, page_osd_get_selected_osd_element_setting_name());
        osd_update_mode();
        break;

    case ROW_OSD_ELEMENT_POS_X:
        if (!element_pos_x_slider_focused) {
            app_state_push(APP_STATE_SUBMENU_ITEM_FOCUSED);
            lv_obj_add_style(slider_group_osd_element_pos_x.slider, &style_silder_select, LV_PART_MAIN);
            element_pos_x_slider_focused = true;
        } else {
            app_state_push(APP_STATE_SUBMENU);
            lv_obj_add_style(slider_group_osd_element_pos_x.slider, &style_silder_main, LV_PART_MAIN);

            setting_osd_goggle_element_t *element = page_osd_get_selected_osd_element_setting_entry();
            settings_put_osd_element_pos_x(&element->position, page_osd_get_selected_osd_element_setting_name());
            osd_update_mode();

            element_pos_x_slider_focused = false;
        }
        break;

    case ROW_OSD_ELEMENT_POS_Y:
        if (!element_pos_y_slider_focused) {
            app_state_push(APP_STATE_SUBMENU_ITEM_FOCUSED);
            lv_obj_add_style(slider_group_osd_element_pos_y.slider, &style_silder_select, LV_PART_MAIN);
            element_pos_y_slider_focused = true;
        } else {
            app_state_push(APP_STATE_SUBMENU);
            lv_obj_add_style(slider_group_osd_element_pos_y.slider, &style_silder_main, LV_PART_MAIN);

            setting_osd_goggle_element_t *element = page_osd_get_selected_osd_element_setting_entry();
            settings_put_osd_element_pos_y(&element->position, page_osd_get_selected_osd_element_setting_name());
            osd_update_mode();

            element_pos_y_slider_focused = false;
        }
        break;

    case ROW_OSD_RESET_ELEMENTS:
        if (reset_all_elements_confirm == ALL_ELEMENT_RES_TIMEOUT)
            return;

        if (reset_all_elements_confirm) {
            page_osd_reset_all_osd_elements();
            osd_update_mode();
            lv_label_set_text(label_reset_all_osd_elements, "#00FF00 Elements reset.#");
            lv_timer_reset(reset_all_osd_elements_timer);
            lv_timer_resume(reset_all_osd_elements_timer);
            reset_all_elements_confirm = ALL_ELEMENT_RES_TIMEOUT;
        } else {
            lv_label_set_text(label_reset_all_osd_elements, "#FFFF00 Click to confirm or Scroll to cancel...#");
            reset_all_elements_confirm = ALL_ELEMENT_RES_CONFIRMED;
        }

        break;

    default:
        break;
    }

    update_osd_element_ui_items();
}

page_pack_t pp_osd = {
    .p_arr = {
        .cur = 0,
        .max = ROW_COUNT},
    .name = "Goggle OSD",
    .create = page_osd_create,
    .enter = NULL,
    .exit = NULL,
    .on_roller = page_osd_on_roller,
    .on_click = page_osd_on_click,
    .on_right_button = NULL,
};
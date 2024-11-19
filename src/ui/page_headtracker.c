#include "page_headtracker.h"

#include <log/log.h>
#include <minIni.h>

#include "common.hh"
#include "core/app_state.h"
#include "core/settings.h"
#include "ht.h"
#include "lang/language.h"
#include "page_common.h"
#include "ui/ui_style.h"

typedef enum {
    PAGE1 = 1,
    PAGE2 = 2,
} page_t;

static btn_group_t page_select;
static page_t curr_page = 0;

static btn_group_t btn_group;

static lv_coord_t col_dsc[] = {160, 160, 160, 160, 160, 160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {55, 55, 55, 55, 55, 55, 60, 30, 40, 40, 40, LV_GRID_TEMPLATE_LAST};

static lv_obj_t *label_cali;
static lv_obj_t *label_center;
static slider_group_t slider_group;
static lv_timer_t *timer;
static lv_obj_t *pan;
static lv_obj_t *tilt;
static lv_obj_t *roll;

static btn_group_t alarm_state;
static lv_obj_t *label_alarm_angle;
static uint8_t set_alarm_wait_for_timer = 0;
static lv_timer_t *set_alarm_angle_timer = NULL;

bool angle_slider_selected;

static void update_visibility(uint8_t page) {

    // enable/disable elements
    if (g_setting.ht.enable && page == PAGE1) {
        lv_obj_clear_state(label_cali, STATE_DISABLED);
        lv_obj_clear_state(label_center, STATE_DISABLED);
        slider_enable(&slider_group, true);

        lv_obj_add_flag(pp_headtracker.p_arr.panel[1], FLAG_SELECTABLE);
        lv_obj_add_flag(pp_headtracker.p_arr.panel[2], FLAG_SELECTABLE);
        lv_obj_add_flag(pp_headtracker.p_arr.panel[3], FLAG_SELECTABLE);
        lv_obj_add_flag(pp_headtracker.p_arr.panel[4], FLAG_SELECTABLE);

    } else if (page == PAGE1) {
        lv_obj_add_state(label_cali, STATE_DISABLED);
        lv_obj_add_state(label_center, STATE_DISABLED);
        slider_enable(&slider_group, false);

        lv_obj_add_flag(pp_headtracker.p_arr.panel[1], FLAG_SELECTABLE);
        lv_obj_clear_flag(pp_headtracker.p_arr.panel[2], FLAG_SELECTABLE);
        lv_obj_clear_flag(pp_headtracker.p_arr.panel[3], FLAG_SELECTABLE);
        lv_obj_clear_flag(pp_headtracker.p_arr.panel[4], FLAG_SELECTABLE);
    }

    if (g_setting.ht.enable && page == PAGE2) {
        btn_group_enable(&alarm_state, true);

        lv_obj_add_flag(pp_headtracker.p_arr.panel[1], FLAG_SELECTABLE);
        lv_obj_clear_flag(pp_headtracker.p_arr.panel[3], FLAG_SELECTABLE);
        lv_obj_clear_flag(pp_headtracker.p_arr.panel[4], FLAG_SELECTABLE);

        if (g_setting.ht.alarm_state != SETTING_HT_ALARM_STATE_OFF) {
            lv_obj_clear_state(label_alarm_angle, STATE_DISABLED);
            lv_obj_add_flag(pp_headtracker.p_arr.panel[2], FLAG_SELECTABLE);

        } else {
            lv_obj_add_state(label_alarm_angle, STATE_DISABLED);
            lv_obj_clear_flag(pp_headtracker.p_arr.panel[2], FLAG_SELECTABLE);
        }
    } else if (page == PAGE2) {
        lv_obj_add_state(label_alarm_angle, STATE_DISABLED);
        btn_group_enable(&alarm_state, false);

        lv_obj_clear_flag(pp_headtracker.p_arr.panel[1], FLAG_SELECTABLE);
        lv_obj_clear_flag(pp_headtracker.p_arr.panel[2], FLAG_SELECTABLE);
        lv_obj_clear_flag(pp_headtracker.p_arr.panel[3], FLAG_SELECTABLE);
        lv_obj_clear_flag(pp_headtracker.p_arr.panel[4], FLAG_SELECTABLE);
    }

    // hiding and showing elements
    switch (page) {
    case PAGE1:
        // show page 1
        btn_group_show(&btn_group, true);
        slider_show(&slider_group, true);
        lv_obj_clear_flag(label_cali, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(label_center, LV_OBJ_FLAG_HIDDEN);

        // hide page 2
        btn_group_show(&alarm_state, false);
        lv_obj_add_flag(label_alarm_angle, LV_OBJ_FLAG_HIDDEN);

        break;

    case PAGE2:
        // hide page 1
        btn_group_show(&btn_group, false);
        slider_show(&slider_group, false);
        lv_obj_add_flag(label_cali, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(label_center, LV_OBJ_FLAG_HIDDEN);

        // show page 2
        btn_group_show(&alarm_state, true);
        lv_obj_clear_flag(label_alarm_angle, LV_OBJ_FLAG_HIDDEN);

        break;
    }
}

static void page_headtracker_set_alarm_reset() {
    lv_label_set_text(label_alarm_angle, _lang("Set Alarm Angle"));
    if (set_alarm_angle_timer != NULL) {
        lv_timer_del(set_alarm_angle_timer);
        set_alarm_angle_timer = NULL;
    }
}

static void page_headtracker_set_alarm_angle_timer_cb(struct _lv_timer_t *timer) {
    page_headtracker_set_alarm_reset();
    // code to execute when the timer elapses and the angle has to be set?
}

static lv_obj_t *page_headtracker_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[128];
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, 1053, 900);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, 1053, 894);

    snprintf(buf, sizeof(buf), "%s:", _lang("Head Tracker"));
    create_text(NULL, section, false, buf, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    create_select_item(arr, cont);

    create_btn_group_item(&page_select, cont, 2, _lang("Page"), _lang("Tracking"), _lang("Tilt Alarm"), "", "", 0);

    // page 2 items
    create_btn_group_item(&alarm_state, cont, 3, _lang("Alarm"), _lang("Off"), _lang("Video"), _lang("Arm"), "", 1);

    label_alarm_angle = create_label_item(cont, _lang("Set Alarm Angle"), 1, 2, 1);
    lv_obj_clear_flag(label_alarm_angle, LV_OBJ_FLAG_SCROLLABLE);

    // preload page 2 items
    btn_group_enable(&alarm_state, false);
    lv_obj_add_state(label_alarm_angle, STATE_DISABLED);

    // page 1 items
    create_btn_group_item(&btn_group, cont, 2, _lang("Tracking"), _lang("On"), _lang("Off"), "", "", 1);

    label_cali = create_label_item(cont, _lang("Calibrate"), 1, 2, 1);

    label_center = create_label_item(cont, _lang("Set Center"), 1, 3, 1);

    create_slider_item(&slider_group, cont, _lang("Max Angle"), 360, g_setting.ht.max_angle, 4);
    lv_slider_set_range(slider_group.slider, 0, 360);

    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    create_label_item(cont, buf, 1, 5, 1);

    btn_group_set_sel(&btn_group, !g_setting.ht.enable);
    btn_group_set_sel(&alarm_state, g_setting.ht.alarm_state);
    btn_group_set_sel(&page_select, 0);

    create_label_item(cont, _lang("Pan"), 1, 7, 1);
    pan = lv_bar_create(cont);
    lv_bar_set_range(pan, 1000, 2000);
    lv_obj_set_size(pan, 500, 25);
    lv_obj_center(pan);
    lv_bar_set_value(pan, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(pan, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN);
    lv_obj_set_style_radius(pan, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(pan, lv_color_make(0, 0xff, 0), LV_PART_INDICATOR);
    lv_obj_set_style_radius(pan, 0, LV_PART_INDICATOR);
    lv_obj_set_grid_cell(pan, LV_GRID_ALIGN_START, 2, 1,
                         LV_GRID_ALIGN_CENTER, 7, 1);

    create_label_item(cont, _lang("Tilt"), 1, 8, 1);
    tilt = lv_bar_create(cont);
    lv_bar_set_range(tilt, 1000, 2000);
    lv_obj_set_size(tilt, 500, 25);
    lv_obj_center(tilt);
    lv_bar_set_value(tilt, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(tilt, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN);
    lv_obj_set_style_radius(tilt, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(tilt, lv_color_make(0, 0xff, 0), LV_PART_INDICATOR);
    lv_obj_set_style_radius(tilt, 0, LV_PART_INDICATOR);
    lv_obj_set_grid_cell(tilt, LV_GRID_ALIGN_START, 2, 1,
                         LV_GRID_ALIGN_CENTER, 8, 1);

    create_label_item(cont, _lang("Roll"), 1, 9, 1);
    roll = lv_bar_create(cont);
    lv_bar_set_range(roll, 1000, 2000);
    lv_obj_set_size(roll, 500, 25);
    lv_obj_center(roll);
    lv_bar_set_value(roll, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(roll, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN);
    lv_obj_set_style_radius(roll, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(roll, lv_color_make(0, 0xff, 0), LV_PART_INDICATOR);
    lv_obj_set_style_radius(roll, 0, LV_PART_INDICATOR);
    lv_obj_set_grid_cell(roll, LV_GRID_ALIGN_START, 2, 1,
                         LV_GRID_ALIGN_CENTER, 9, 1);

    curr_page = PAGE1;
    update_visibility(curr_page);
    return page;
}

static void ht_angle_inc(void) {
    int32_t value = 0;
    char buf[12];

    value = lv_slider_get_value(slider_group.slider);
    if (value < 360)
        value += 1;

    lv_slider_set_value(slider_group.slider, value, LV_ANIM_OFF);

    snprintf(buf, sizeof(buf), "%d", value);
    lv_label_set_text(slider_group.label, buf);

    ht_set_maxangle(value);

    g_setting.ht.max_angle = value;
}

static void ht_angle_dec(void) {
    int32_t value = 0;
    char buf[12];

    value = lv_slider_get_value(slider_group.slider);
    if (value > 0)
        value -= 1;

    lv_slider_set_value(slider_group.slider, value, LV_ANIM_OFF);

    snprintf(buf, sizeof(buf), "%d", value);
    lv_label_set_text(slider_group.label, buf);

    ht_set_maxangle(value);

    g_setting.ht.max_angle = value;
}

static void page_headtracker_exit_slider() {
    app_state_push(APP_STATE_SUBMENU);
    lv_obj_add_style(slider_group.slider, &style_silder_main, LV_PART_MAIN);
    ini_putl("ht", "max_angle", g_setting.ht.max_angle, SETTING_INI);
    angle_slider_selected = false;
}

static void page_headtracker_on_roller(uint8_t key) {

    // Ignore commands until timer has expired before allowing user to proceed.
    if (set_alarm_angle_timer != NULL) {
        return;
    }

    if (angle_slider_selected == false) {
        return;
    }

    if (key == DIAL_KEY_UP) {
        ht_angle_dec();
    } else if (key == DIAL_KEY_DOWN) {
        ht_angle_inc();
    }
}

static void page_headtracker_on_click_page1(uint8_t key, int sel) {
    char buf[64];
    if (sel == 1) {
        btn_group_toggle_sel(&btn_group);
        g_setting.ht.enable = btn_group_get_sel(&btn_group) == 0;
        settings_put_bool("ht", "enable", g_setting.ht.enable);
        if (g_setting.ht.enable)
            ht_enable();
        else
            ht_disable();

        update_visibility(curr_page);
    } else if (sel == 2) {
        snprintf(buf, sizeof(buf), "%s...", _lang("Calibrating"));
        lv_label_set_text(label_cali, buf);
        lv_timer_handler();
        ht_calibrate();
        lv_label_set_text(label_cali, _lang("Re-calibrate"));
        lv_timer_handler();
    } else if (sel == 3) {
        ht_set_center_position();
    } else if (sel == 4) {
        if (angle_slider_selected) {
            page_headtracker_exit_slider();
        } else {
            app_state_push(APP_STATE_SUBMENU_ITEM_FOCUSED);
            lv_obj_add_style(slider_group.slider, &style_silder_select, LV_PART_MAIN);
            angle_slider_selected = true;
        }
    }
}

static void page_headtracker_on_click_page2(uint8_t key, int sel) {
    char buf[64];
    if (sel == 1) {
        btn_group_toggle_sel(&alarm_state);
        g_setting.ht.alarm_state = btn_group_get_sel(&alarm_state);
        ini_putl("ht", "alarm_state", g_setting.ht.alarm_state, SETTING_INI);
    } else if (sel == 2) {
        snprintf(buf, sizeof(buf), "%s...", "Updating Angle");
        lv_label_set_text(label_alarm_angle, buf);
        set_alarm_angle_timer = lv_timer_create(page_headtracker_set_alarm_angle_timer_cb, 1000, NULL);
        lv_timer_set_repeat_count(set_alarm_angle_timer, 1);
        ht_set_alarm_angle();
    }
}
static void page_headtracker_on_click(uint8_t key, int sel) {
    if (sel == 0) {
        btn_group_toggle_sel(&page_select);
        int page_select_btn = btn_group_get_sel(&page_select);
        LOGD("page_select: %d", btn_group_get_sel(&page_select));
        if (page_select_btn == 0) {
            curr_page = PAGE1;
        } else if (page_select_btn == 1) {
            curr_page = PAGE2;
        }
        update_visibility(curr_page);
    }
    switch (curr_page) {
    case PAGE1:
        page_headtracker_on_click_page1(key, sel);
        break;
    case PAGE2:
        page_headtracker_on_click_page2(key, sel);
        break;
    }

    update_visibility(curr_page);
}

static void page_headtracker_timer(struct _lv_timer_t *timer) {
    int16_t *channels = ht_get_channels();
    lv_bar_set_value(pan, channels[0], LV_ANIM_OFF);
    lv_bar_set_value(tilt, channels[1], LV_ANIM_OFF);
    lv_bar_set_value(roll, channels[2], LV_ANIM_OFF);
}

static void page_headtracker_enter() {
    lv_slider_set_value(slider_group.slider, g_setting.ht.max_angle, LV_ANIM_OFF);
    timer = lv_timer_create(page_headtracker_timer, 50, NULL);
    lv_timer_set_repeat_count(timer, -1);
    angle_slider_selected = false;
}

static void page_headtracker_exit() {
    LOGD("page_headtracker_exit");
    if (angle_slider_selected) {
        page_headtracker_exit_slider();
    }
    LOGD("page_headtracker_exit 2");
    lv_timer_del(timer);
    LOGD("page_headtracker_exit 3");
    page_headtracker_set_alarm_reset();
    LOGD("page_headtracker_exit 4");
}

page_pack_t pp_headtracker = {
    .p_arr = {
        .cur = 0,
        .max = 6,
    },
    .name = "Head Tracker",
    .create = page_headtracker_create,
    .enter = page_headtracker_enter,
    .exit = page_headtracker_exit,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = page_headtracker_on_roller,
    .on_click = page_headtracker_on_click,
    .on_right_button = NULL,
};
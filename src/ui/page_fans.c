#include "page_fans.h"

#include <stdio.h>

#include <log/log.h>
#include <minIni.h>

#include "../conf/ui.h"

#include "core/app_state.h"
#include "core/common.hh"
#include "driver/fans.h"
#include "driver/nct75.h"
#include "lang/language.h"
#include "ui/page_common.h"
#include "ui/page_fans.h"
#include "ui/ui_attribute.h"
#include "ui/ui_style.h"

typedef enum {
    FAN_TOP = 0,
    FAN_LEFT,
    FAN_RIGHT,
    FAN_COUNT
} fans_t;

typedef enum {
    FANS_MODE_NO_FAN = 0,
    FANS_MODE_TOP,
    FANS_MODE_SIDE,
} fans_mode_t;

static lv_coord_t col_dsc[] = {UI_FANS_COLS};
static lv_coord_t row_dsc[] = {UI_FANS_ROWS};

static fans_mode_t fans_mode = FANS_MODE_NO_FAN;

static btn_group_t btn_group_fans;

static slider_group_t slider_group[2];

static void update_visibility() {
    slider_enable(&slider_group[0], btn_group_fans.current != 0);
    if (btn_group_fans.current == 0) {
        lv_obj_clear_flag(pp_fans.p_arr.panel[1], FLAG_SELECTABLE);
    } else {
        lv_obj_add_flag(pp_fans.p_arr.panel[1], FLAG_SELECTABLE);
    }

    if (TARGET_GOGGLE == getTargetType()) {
        slider_enable(&slider_group[1], btn_group_fans.current != 0);
        if (btn_group_fans.current == 0) {
            lv_obj_clear_flag(pp_fans.p_arr.panel[2], FLAG_SELECTABLE);
        } else {
            lv_obj_add_flag(pp_fans.p_arr.panel[2], FLAG_SELECTABLE);
        }
    }
}

static lv_obj_t *page_fans_create(lv_obj_t *parent, panel_arr_t *arr) {
    int rows = 0;
    char buf[128];
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, UI_PAGE_VIEW_SIZE);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, UI_PAGE_VIEW_SIZE);

    snprintf(buf, sizeof(buf), "%s:", _lang(pp_fans.name));
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

    create_btn_group_item(&btn_group_fans, cont, 2, _lang("Auto Control"), _lang("On"), _lang("Off"), "", "", rows++);
    btn_group_set_sel(&btn_group_fans, !g_setting.fans.auto_mode);

    create_slider_item(&slider_group[0], cont, _lang("Top Fan"), MAX_FAN_TOP, 2, rows++);
    lv_slider_set_range(slider_group[0].slider, MIN_FAN_TOP, MAX_FAN_TOP);
    lv_slider_set_value(slider_group[0].slider, g_setting.fans.top_speed, LV_ANIM_OFF);
    snprintf(buf, sizeof(buf), "%d", g_setting.fans.top_speed);
    lv_label_set_text(slider_group[0].label, buf);

    if (TARGET_GOGGLE == getTargetType()) {
        create_slider_item(&slider_group[1], cont, _lang("Side Fans"), MAX_FAN_SIDE, 2, rows++);
        lv_slider_set_range(slider_group[1].slider, MIN_FAN_SIDE, MAX_FAN_SIDE);
        lv_slider_set_value(slider_group[1].slider, g_setting.fans.left_speed, LV_ANIM_OFF);
        snprintf(buf, sizeof(buf), "%d", g_setting.fans.left_speed);
        lv_label_set_text(slider_group[1].label, buf);
    }

    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    create_label_item(cont, buf, 1, rows++, 1);
    pp_fans.p_arr.max = rows;

    update_visibility();

    return page;
}

static void fans_top_speed_inc() {
    char buf[12];
    int32_t value = lv_slider_get_value(slider_group[0].slider);

    if (value < MAX_FAN_TOP)
        value += 1;

    lv_slider_set_value(slider_group[0].slider, value, LV_ANIM_OFF);

    snprintf(buf, sizeof(buf), "%d", value);
    lv_label_set_text(slider_group[0].label, buf);

    fans_top_setspeed(value);

    g_setting.fans.top_speed = value;
    ini_putl("fans", "top_speed", value, SETTING_INI);
}

static void fans_top_speed_dec() {
    char buf[12];
    int32_t value = lv_slider_get_value(slider_group[0].slider);

    if (value > MIN_FAN_TOP)
        value -= 1;

    lv_slider_set_value(slider_group[0].slider, value, LV_ANIM_OFF);
    snprintf(buf, sizeof(buf), "%d", value);
    lv_label_set_text(slider_group[0].label, buf);

    fans_top_setspeed(value);

    g_setting.fans.top_speed = (uint8_t)value;
    ini_putl("fans", "top_speed", value, SETTING_INI);
}

static void fans_side_speed_inc() {
    char buf[12];
    int32_t value = lv_slider_get_value(slider_group[1].slider);

    if (value < MAX_FAN_SIDE)
        value += 1;

    lv_slider_set_value(slider_group[1].slider, value, LV_ANIM_OFF);

    snprintf(buf, sizeof(buf), "%d", value);
    lv_label_set_text(slider_group[1].label, buf);

    g_setting.fans.left_speed = value;
    ini_putl("fans", "left_speed", value, SETTING_INI);
    g_setting.fans.right_speed = value;
    ini_putl("fans", "right_speed", value, SETTING_INI);
}

static void fans_side_speed_dec() {
    char buf[12];
    int32_t value = lv_slider_get_value(slider_group[1].slider);

    if (value > MIN_FAN_SIDE)
        value -= 1;

    lv_slider_set_value(slider_group[1].slider, value, LV_ANIM_OFF);
    snprintf(buf, sizeof(buf), "%d", value);
    lv_label_set_text(slider_group[1].label, buf);

    g_setting.fans.left_speed = value;
    ini_putl("fans", "left_speed", value, SETTING_INI);
    g_setting.fans.right_speed = value;
    ini_putl("fans", "right_speed", value, SETTING_INI);
}

static void fans_speed_inc(void) {
    if (fans_mode == FANS_MODE_TOP) {
        fans_top_speed_inc();
    } else if (fans_mode == FANS_MODE_SIDE) {
        fans_side_speed_inc();
    }
}

static void fans_speed_dec(void) {
    if (fans_mode == FANS_MODE_TOP) {
        fans_top_speed_dec();
    } else if (fans_mode == FANS_MODE_SIDE) {
        fans_side_speed_dec();
    }
}

static void page_fans_exit_slider() {
    lv_obj_t *slider;
    if (fans_mode == FANS_MODE_TOP) {
        slider = slider_group[0].slider;
    } else if (fans_mode == FANS_MODE_SIDE) {
        slider = slider_group[1].slider;
    } else {
        return;
    }

    app_state_push(APP_STATE_SUBMENU);
    lv_obj_add_style(slider, &style_silder_main, LV_PART_MAIN);
    fans_mode = FANS_MODE_NO_FAN;
}

static void page_fans_mode_exit() {
    if (fans_mode != FANS_MODE_NO_FAN) {
        page_fans_exit_slider();
    }
}

static void page_fans_mode_on_roller(uint8_t key) {
    if (g_app_state != APP_STATE_SUBMENU_ITEM_FOCUSED) {
        return;
    }

    if (key == DIAL_KEY_UP) {
        fans_speed_dec();
    } else if (key == DIAL_KEY_DOWN) {
        fans_speed_inc();
    }
}

static void page_fans_mode_on_click(uint8_t key, int sel) {
    lv_obj_t *slider = slider_group[0].slider;

    if (sel == 0) {
        btn_group_toggle_sel(&btn_group_fans);
        g_setting.fans.auto_mode = btn_group_get_sel(&btn_group_fans) == 0;
        settings_put_bool("fans", "auto", g_setting.fans.auto_mode);
        update_visibility();
        return;
    } else if (sel == 1) {
        slider = slider_group[0].slider;
        fans_mode = FANS_MODE_TOP;
    } else if (TARGET_GOGGLE == getTargetType() && sel == 2) {
        slider = slider_group[1].slider;
        fans_mode = FANS_MODE_SIDE;
    } else {
        return;
    }

    if (g_app_state == APP_STATE_SUBMENU_ITEM_FOCUSED) {
        page_fans_exit_slider();
    } else {
        app_state_push(APP_STATE_SUBMENU_ITEM_FOCUSED);
        lv_obj_add_style(slider, &style_silder_select, LV_PART_MAIN);
    }
}

void step_topfan() {
    char str[10];

    if (g_setting.fans.top_speed == MAX_FAN_TOP)
        g_setting.fans.top_speed = MIN_FAN_TOP;
    else
        g_setting.fans.top_speed++;

    fans_top_setspeed(g_setting.fans.top_speed);
    ini_putl("fans", "top_speed", g_setting.fans.top_speed, SETTING_INI);

    lv_slider_set_value(slider_group[0].slider, g_setting.fans.top_speed, LV_ANIM_OFF);
    snprintf(str, sizeof(str), "%d", g_setting.fans.top_speed);
    lv_label_set_text(slider_group[0].label, str);
}

///////////////////////////////////////////////////////////////////////////////
// Auto control side fans
static uint16_t respeed_cnt[FAN_COUNT] = {0, 0, 0}; //[0]=top,[1]=right,[2]=left
static uint8_t respeeding[FAN_COUNT] = {0, 0, 0};

uint8_t adj_speed(int which, uint8_t cur_speed, int tempe) {
    uint8_t new_speed = cur_speed;

    switch (which) {
    case FAN_TOP:
        if (tempe > FAN_TEMPERATURE_THR_H) {
            if (new_speed != MAX_FAN_TOP) {
                new_speed++;
                respeeding[which] = 1;
                respeed_cnt[which] = 0;
            }
        } else if (tempe < FAN_TEMPERATURE_THR_L) {
            if (new_speed != MIN_FAN_TOP) {
                new_speed--;
                respeeding[which] = 1;
                respeed_cnt[which] = 0;
            }
        }
        if (cur_speed != new_speed) {
            LOGI("Top Fan speed: %d (T=%d)", new_speed, tempe);
        }
        break;
    default:
        if (tempe > FAN_TEMPERATURE_THR_H) {
            if (new_speed != MAX_FAN_SIDE) {
                new_speed++;
                respeeding[which] = 1;
                respeed_cnt[which] = 0;
            }
        } else if (tempe < FAN_TEMPERATURE_THR_L) {
            if (new_speed != MIN_FAN_SIDE) {
                new_speed--;
                respeeding[which] = 1;
                respeed_cnt[which] = 0;
            }
        }
        if (cur_speed != new_speed) {
            LOGI("%s Fan speed: %d (T=%d)", which == FAN_LEFT ? "Left" : "Right", new_speed, tempe);
        }
        break;
    }

    return new_speed;
}

void fans_auto_ctrl_core(int which, int tempe, bool binit) {
    static uint8_t speed[FAN_COUNT] = {2, 2, 2};
    uint8_t new_spd;

    //////////////////////////////////////////////////////////////////////////////////
    // reinit auto speed
    if (binit) {
        speed[0] = speed[1] = speed[2] = 2; // Initial fan speed for auto mode
        respeed_cnt[0] = respeed_cnt[1] = respeed_cnt[2] = 0;
        respeeding[0] = respeeding[1] = respeeding[2] = 0;
        fans_top_setspeed(speed[0]);
        fans_left_setspeed(speed[1]);
        fans_right_setspeed(speed[2]);
    }

    if (respeeding[which]) {
        respeed_cnt[which]++;
        if (respeed_cnt[which] == RESPEED_WAIT_TIME) {
            respeeding[which] = false;
            respeed_cnt[which] = 0;
        }
        return;
    }

    new_spd = adj_speed(which, speed[which], tempe);
    if (new_spd != speed[which]) {
        speed[which] = new_spd;
        switch (which) {
        case FAN_TOP:
            fans_top_setspeed(speed[which]);
            break;
        case FAN_LEFT:
            fans_left_setspeed(speed[which]);
            break;
        case FAN_RIGHT:
            fans_left_setspeed(speed[which]);
            break;
        }
    }
}

bool rescue_from_hot() {
    static fan_speed_t speed_saved;
    static bool respeeding[3] = {false, false, false};

    // Right
    if (g_temperature.right > SIDE_TEMPERATURE_RISKH) {
        if (!respeeding[0]) {
            speed_saved.right = fan_speed.right;
            respeeding[0] = true;
            fans_right_setspeed(MAX_FAN_SIDE);
            LOGI("Right fan: rescue ON.\n");
        }
    } else if (respeeding[0] && (g_temperature.right < FAN_TEMPERATURE_THR_L)) {
        fans_right_setspeed(speed_saved.right);
        respeeding[0] = false;
        LOGI("Right fan: rescue OFF.");
    }

    // Left
    if (g_temperature.left > SIDE_TEMPERATURE_RISKH) {
        if (!respeeding[1]) {
            speed_saved.left = fan_speed.left;
            respeeding[1] = true;
            fans_left_setspeed(MAX_FAN_SIDE);
            LOGI("Left fan: rescue ON.\n");
        }
    } else if (respeeding[1] && (g_temperature.left < FAN_TEMPERATURE_THR_L)) {
        fans_left_setspeed(speed_saved.left);
        respeeding[1] = false;
        LOGI("Left fan: rescue OFF.");
    }

    // Top
    if (g_temperature.top > TOP_TEMPERATURE_RISKH) {
        if (!respeeding[2]) {
            speed_saved.top = fan_speed.top;
            respeeding[2] = true;
            fans_top_setspeed(MAX_FAN_TOP);
            LOGI("Top fan: rescue ON.\n");
        }
    } else if (respeeding[2] && (g_temperature.top < TOP_TEMPERATURE_NORM)) {
        fans_top_setspeed(speed_saved.top);
        respeeding[2] = false;
        LOGI("Top fan: rescue OFF.");
    }

    g_temperature.is_rescuing = respeeding[0] || respeeding[1] || respeeding[2];
    g_temperature.is_overheat = (g_temperature.top > TOP_TEMPERATURE_RISKH) &&
                                (g_temperature.right > SIDE_TEMPERATURE_RISKH) &&
                                (g_temperature.left > SIDE_TEMPERATURE_RISKH);

    return g_temperature.is_rescuing;
}

void fans_auto_ctrl() {
    static uint8_t auto_mode_d;
    static fan_speed_t speed;
    uint8_t binit_r, binit_f;

    if (rescue_from_hot())
        return;

    binit_r = (auto_mode_d == 0) && (g_setting.fans.auto_mode == 1); // Manual mode -> Auto
    binit_f = (auto_mode_d == 1) && (g_setting.fans.auto_mode == 0); // Auto   mode -> manual
    auto_mode_d = g_setting.fans.auto_mode;

    if (g_setting.fans.auto_mode) {
        fans_auto_ctrl_core(FAN_TOP, g_temperature.top, binit_r);
        fans_auto_ctrl_core(FAN_RIGHT, g_temperature.right, false);
        fans_auto_ctrl_core(FAN_LEFT, g_temperature.left, false);
    } else {
        if (binit_f)
            speed.top = speed.left = speed.right = 0xFF;

        if (speed.right != g_setting.fans.right_speed) {
            fans_right_setspeed(g_setting.fans.right_speed);
            speed.right = g_setting.fans.right_speed;
        }

        if (speed.left != g_setting.fans.left_speed) {
            fans_left_setspeed(g_setting.fans.left_speed);
            speed.left = g_setting.fans.left_speed;
        }

        if (speed.top != g_setting.fans.top_speed) {
            fans_top_setspeed(g_setting.fans.top_speed);
            speed.top = g_setting.fans.top_speed;
        }
    }
}

void change_topfan(uint8_t key) {
    if (key == DIAL_KEY_UP) {
        fans_top_speed_inc();
    } else if (key == DIAL_KEY_DOWN) {
        fans_top_speed_dec();
    }
}

page_pack_t pp_fans = {
    .p_arr = {
        .cur = 0,
        .max = 4,
    },
    .name = "Fan Control",
    .create = page_fans_create,
    .enter = NULL,
    .exit = page_fans_mode_exit,
    .on_created = NULL,
    .on_update = NULL,
    .on_roller = page_fans_mode_on_roller,
    .on_click = page_fans_mode_on_click,
    .on_right_button = NULL,
};
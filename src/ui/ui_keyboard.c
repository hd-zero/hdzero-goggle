#include "ui_keyboard.h"

#include <stdio.h>

#include "core/common.hh"
#include "core/osd.h"
#include "ui/page_common.h"
#include "ui/ui_style.h"

/**
 * Keyboard Type
 */
typedef struct {
    lv_obj_t *input;
    lv_obj_t *text;
    uint16_t button;
    lv_obj_t *dial_click_img;
    lv_obj_t *dial_click_text;
    lv_obj_t *dial_scroll_img;
    lv_obj_t *dial_scroll_text;
    lv_obj_t *right_button_img;
    lv_obj_t *right_button_text;
} keyboard_t;

static keyboard_t g_keyboard = {NULL, NULL, 1};

/**
 * Keyboard Layouts (Dial/Scroll Wheel Friendly)
 */
#define LV_KB_BTN(width) LV_BTNMATRIX_CTRL_POPOVER | width

static const char *g_keyboard_map_lc[] = {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "\n",
                                          "a", "s", "d", "f", "g", "h", "j", "k", "l", "\n",
                                          "z", "x", "c", "v", "b", "n", "m", "\n",
                                          "ABC", LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_BACKSPACE, ""};

static const lv_btnmatrix_ctrl_t g_keyboard_ctrl_lc_map[] = {
    LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4),
    LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3),
    LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1),
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, LV_BTNMATRIX_CTRL_CHECKED | 1, 4, LV_BTNMATRIX_CTRL_CHECKED | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2};

static const char *g_keyboard_map_uc[] = {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "\n",
                                          "A", "S", "D", "F", "G", "H", "J", "K", "L", "\n",
                                          "Z", "X", "C", "V", "B", "N", "M", "\n",
                                          "1#", LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_BACKSPACE, ""};

static const lv_btnmatrix_ctrl_t g_keyboard_ctrl_uc_map[] = {
    LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4),
    LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3),
    LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1),
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, LV_BTNMATRIX_CTRL_CHECKED | 1, 4, LV_BTNMATRIX_CTRL_CHECKED | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2};

static const char *g_keyboard_map_spec[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "\n",
                                            "+", "-", "*", "/", "=", "%", "!", "#", "<", ">", ",", ":", ";", "\n",
                                            "@", "\\", "_", "?", "$", "(", ")", "{", "}", "[", "]", "'", "\"", ".", "\n",
                                            "abc", LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_BACKSPACE, ""};

static const lv_btnmatrix_ctrl_t g_keyboard_ctrl_spec_map[] = {
    LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1),
    LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1),
    LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1),
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, LV_BTNMATRIX_CTRL_CHECKED | 1, 4, LV_BTNMATRIX_CTRL_CHECKED | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2};

/**
 * Private Functions
 */
static void keyboard_reset() {
    g_keyboard.button = 0;
    lv_btnmatrix_clear_btn_ctrl_all(g_keyboard.input, LV_BTNMATRIX_CTRL_CHECKED);
    lv_btnmatrix_set_btn_ctrl(g_keyboard.input, g_keyboard.button, LV_BTNMATRIX_CTRL_CHECKED);
    keyboard_clear_text();
}

static void keyboard_update(uint8_t key) {
    g_keyboard.button += key == DIAL_KEY_UP ? 1 : -1;

    if (g_keyboard.button == (uint16_t)-1) {
        g_keyboard.button = ((lv_btnmatrix_t *)g_keyboard.input)->btn_cnt - 1;
    } else if (g_keyboard.button >= ((lv_btnmatrix_t *)g_keyboard.input)->btn_cnt) {
        g_keyboard.button = 0;
    }
}

/**
 * External Functions
 */
void keyboard_init() {
    if (!g_keyboard.input || !g_keyboard.text) {
        char filename[128];

        // Create main objects and bind the two
        g_keyboard.input = lv_keyboard_create(lv_scr_act());
        g_keyboard.text = lv_textarea_create(lv_scr_act());
        lv_keyboard_set_textarea(g_keyboard.input, g_keyboard.text);

        // Placement
        lv_obj_align(g_keyboard.text, LV_ALIGN_BOTTOM_MID, 0, -372);
        lv_obj_set_size(g_keyboard.input, 1416, 374);
        lv_obj_set_size(g_keyboard.text, 1416, 60);

        // Layout
        lv_textarea_set_one_line(g_keyboard.text, true);
        lv_keyboard_set_map(g_keyboard.input, LV_KEYBOARD_MODE_TEXT_LOWER, g_keyboard_map_lc, g_keyboard_ctrl_lc_map);
        lv_keyboard_set_map(g_keyboard.input, LV_KEYBOARD_MODE_TEXT_UPPER, g_keyboard_map_uc, g_keyboard_ctrl_uc_map);
        lv_keyboard_set_map(g_keyboard.input, LV_KEYBOARD_MODE_SPECIAL, g_keyboard_map_spec, g_keyboard_ctrl_spec_map);

        // Styles
        lv_obj_add_style(g_keyboard.input, &style_keyboard[KB_STYLE_MAIN], LV_PART_MAIN);
        lv_obj_add_style(g_keyboard.input, &style_keyboard[KB_STYLE_KEY], LV_PART_ITEMS | LV_STATE_CHECKED);
        lv_obj_add_style(g_keyboard.text, &style_keyboard[KB_STYLE_TEXT], LV_PART_MAIN);
        lv_obj_add_style(g_keyboard.text, &style_keyboard[KB_STYLE_CURSOR], LV_PART_CURSOR | LV_STATE_FOCUSED);

        // Help - Dial Scroll
        g_keyboard.dial_scroll_img = lv_img_create(lv_scr_act());
        lv_obj_add_flag(g_keyboard.dial_scroll_img, LV_OBJ_FLAG_FLOATING);
        lv_obj_clear_flag(g_keyboard.dial_scroll_img, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_pos(g_keyboard.dial_scroll_img, 520, 100);
        osd_resource_path(filename, "%s", OSD_RESOURCE_720, DIAL_SCROLL_IMG);
        lv_img_set_src(g_keyboard.dial_scroll_img, filename);

        g_keyboard.dial_scroll_text = lv_label_create(lv_scr_act());
        lv_obj_add_style(g_keyboard.dial_scroll_text, &style_rootmenu, LV_PART_MAIN);
        lv_label_set_text(g_keyboard.dial_scroll_text, "Highlight Key");
        lv_obj_set_style_text_font(g_keyboard.dial_scroll_text, &lv_font_montserrat_26, 0);
        lv_obj_set_style_text_align(g_keyboard.dial_scroll_text, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_pos(g_keyboard.dial_scroll_text, 620, 130);

        // Help - Dial Click
        g_keyboard.dial_click_img = lv_img_create(lv_scr_act());
        lv_obj_add_flag(g_keyboard.dial_click_img, LV_OBJ_FLAG_FLOATING);
        lv_obj_clear_flag(g_keyboard.dial_click_img, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_pos(g_keyboard.dial_click_img, 920, 100);
        osd_resource_path(filename, "%s", OSD_RESOURCE_720, DIAL_CLICK_IMG);
        lv_img_set_src(g_keyboard.dial_click_img, filename);

        g_keyboard.dial_click_text = lv_label_create(lv_scr_act());
        lv_obj_add_style(g_keyboard.dial_click_text, &style_rootmenu, LV_PART_MAIN);
        lv_label_set_text(g_keyboard.dial_click_text, "Select Key");
        lv_obj_set_style_text_font(g_keyboard.dial_click_text, &lv_font_montserrat_26, 0);
        lv_obj_set_style_text_align(g_keyboard.dial_click_text, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_pos(g_keyboard.dial_click_text, 1020, 130);
        lv_label_set_recolor(g_keyboard.dial_click_text, true);

        // Help - Right Button
        g_keyboard.right_button_img = lv_img_create(lv_scr_act());
        lv_obj_add_flag(g_keyboard.right_button_img, LV_OBJ_FLAG_FLOATING);
        lv_obj_clear_flag(g_keyboard.right_button_img, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_pos(g_keyboard.right_button_img, 1270, 100);
        osd_resource_path(filename, "%s", OSD_RESOURCE_720, RIGHT_BUTTON_IMG);
        lv_img_set_src(g_keyboard.right_button_img, filename);

        g_keyboard.right_button_text = lv_label_create(lv_scr_act());
        lv_obj_add_style(g_keyboard.right_button_text, &style_rootmenu, LV_PART_MAIN);
        lv_label_set_text(g_keyboard.right_button_text, "Click: Close Keyboard\nHold: Erase Text");
        lv_obj_set_style_text_font(g_keyboard.right_button_text, &lv_font_montserrat_26, 0);
        lv_obj_set_style_text_align(g_keyboard.right_button_text, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_pos(g_keyboard.right_button_text, 1360, 116);
        lv_label_set_recolor(g_keyboard.right_button_text, true);

        keyboard_close();
    }
}

bool keyboard_active() {
    return !lv_obj_has_flag(g_keyboard.input, LV_OBJ_FLAG_HIDDEN);
}

void keyboard_open() {
    lv_obj_clear_flag(g_keyboard.input, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(g_keyboard.text, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_state(g_keyboard.text, LV_STATE_FOCUSED);
    lv_obj_clear_flag(g_keyboard.dial_click_img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(g_keyboard.dial_click_text, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(g_keyboard.dial_scroll_img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(g_keyboard.dial_scroll_text, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(g_keyboard.right_button_img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(g_keyboard.right_button_text, LV_OBJ_FLAG_HIDDEN);
}

void keyboard_close() {
    lv_obj_add_flag(g_keyboard.input, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(g_keyboard.text, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_state(g_keyboard.text, LV_STATE_FOCUSED);
    lv_obj_add_flag(g_keyboard.dial_click_img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(g_keyboard.dial_click_text, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(g_keyboard.dial_scroll_img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(g_keyboard.dial_scroll_text, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(g_keyboard.right_button_img, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(g_keyboard.right_button_text, LV_OBJ_FLAG_HIDDEN);

    keyboard_reset();
}

void keyboard_scroll(uint8_t key) {
    keyboard_update(key);
    lv_btnmatrix_clear_btn_ctrl_all(g_keyboard.input, LV_BTNMATRIX_CTRL_CHECKED);
    lv_btnmatrix_set_btn_ctrl(g_keyboard.input, g_keyboard.button, LV_BTNMATRIX_CTRL_CHECKED);
}

void keyboard_press() {
    const char *text = lv_btnmatrix_get_btn_text(g_keyboard.input, g_keyboard.button);
    lv_btnmatrix_set_selected_btn(g_keyboard.input, g_keyboard.button);
    lv_event_send(g_keyboard.input, LV_EVENT_VALUE_CHANGED, &g_keyboard.button);

    // Force keyboard layout position to remain consistent during switching of
    // layouts. If bottom row is updated then offset will need to be updated.
    if (text == "1#" || text == "abc" || text == "ABC") {
        g_keyboard.button = ((lv_btnmatrix_t *)g_keyboard.input)->btn_cnt - 5;
    }

    lv_btnmatrix_clear_btn_ctrl_all(g_keyboard.input, LV_BTNMATRIX_CTRL_CHECKED);
    lv_btnmatrix_set_btn_ctrl(g_keyboard.input, g_keyboard.button, LV_BTNMATRIX_CTRL_CHECKED);
}

void keyboard_clear_text() {
    lv_textarea_set_text(g_keyboard.text, "");
}

void keyboard_set_text(const char *text) {
    lv_textarea_set_text(g_keyboard.text, text);
}

int keyboard_get_text(char *buffer, const uint32_t size) {
    int written = 0;
    const char *text = lv_textarea_get_text(g_keyboard.text);
    if (0 < strlen(text)) {
        written = snprintf(buffer, size, "%s", text);
    }
    return written;
}

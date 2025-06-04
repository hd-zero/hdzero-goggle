#include "page_input.h"

#include <minIni.h>

#include "../conf/ui.h"

#include "core/app_state.h"
#include "core/common.hh"
#include "core/dvr.h"
#include "core/ht.h"
#include "core/input_device.h"
#include "core/osd.h"
#include "core/sleep_mode.h"

#include "lang/language.h"
#include "ui/page_fans.h"
#include "ui/page_source.h"
#include "ui/ui_image_setting.h"

/**
 * Various enumerations and typedefs
 */
typedef enum page_input_rows {
    ROLLER,

    LEFT_SHORT,
    LEFT_LONG,

    RIGHT_SHORT,
    RIGHT_LONG,
    RIGHT_DOUBLE,
    BACK_BTN,

    ROW_COUNT
} rowType_t;

typedef struct Action {
    uint16_t id;
    const char *name;
    union {
        void(*const btnFunction);
        void (*const rollerFunction)(uint8_t);
    } functionPtr;
} action_t;

/**
 * Compile-unit local variables, constants and fields
 */
static lv_coord_t col_dsc[] = {UI_INPUT_COLS};
static lv_coord_t row_dsc[] = {UI_INPUT_ROWS};

static void nop() {}
static void rollerNop(uint8_t key) { (void)key; }

static const action_t btnActions[] = {
    {.id = 10, .name = "None", .functionPtr = &nop},
    {.id = 0, .name = "Toggle OSD", .functionPtr = &osd_toggle},
    {.id = 1, .name = "Main menu", .functionPtr = &app_switch_to_menu},
    {.id = 2, .name = "Toggle DVR", .functionPtr = &dvr_toggle},
    {.id = 3, .name = "Center HT", .functionPtr = &ht_set_center_position},
    {.id = 4, .name = "Calibrate HT", .functionPtr = &ht_calibrate},
    {.id = 5, .name = "Go Sleep!", .functionPtr = &go_sleep},
    {.id = 6, .name = "Toggle fan speed", .functionPtr = &step_topfan},
    {.id = 7, .name = "Star DVR", .functionPtr = &dvr_star},
    {.id = 8, .name = "Toggle source", .functionPtr = &source_toggle},
    {.id = 9, .name = "Cycle source", .functionPtr = &source_cycle},
};

static const action_t rollerActions[] = {
    {.id = 3, .name = "None", .functionPtr = &rollerNop},
    {.id = 0, .name = "Switch channel", .functionPtr = &tune_channel},
    {.id = 1, .name = "Change fan speed", .functionPtr = &change_topfan},
    {.id = 2, .name = "Panel Brightness", .functionPtr = &change_oled_brightness}};

static rowType_t selectedRow = ROW_COUNT;
static lv_obj_t *pageItems[ROW_COUNT];
static int currentHighlight;
static uint16_t previousSelection;

/**
 * Build a '\n'-separated list of all available options for the dropdown element
 */
static void build_options_string(const action_t *const actions, size_t size, char *output) {
    for (size_t i = 0; i < size; i++) {
        strcat(output, _lang(actions[i].name));
        if (i < size - 1) {
            strcat(output, "\n");
        }
    }
}

/**
 * Get the action_t item from btnActions by the given id
 * @return The action_t item or NULL if id was not found
 */
static const action_t *actionFromId(const action_t *const actions, size_t size, uint16_t id) {
    for (size_t i = 0; i < size; i++) {
        if (actions[i].id == id) {
            return &actions[i];
        }
    }

    return NULL;
}
static const action_t *btnActionFromId(uint16_t id) { return actionFromId(btnActions, ARRAY_SIZE(btnActions), id); }
static const action_t *rollerActionFromId(uint16_t id) { return actionFromId(rollerActions, ARRAY_SIZE(rollerActions), id); }

/**
 * Get the array index of the action that has the given id.
 */
static size_t indexFromId(const action_t *const actions, size_t size, uint16_t id) {
    return actionFromId(actions, size, id) - actions;
}
static size_t btnIndexFromId(uint16_t id) { return indexFromId(btnActions, ARRAY_SIZE(btnActions), (id)); }
static size_t rollerIndexFromId(uint16_t id) { return indexFromId(rollerActions, ARRAY_SIZE(rollerActions), (id)); }

/**
 * Update the UI elements as the user navigates the page
 */
static void reset_dropdown_styles() {
    for (rowType_t i = 0; i < BACK_BTN; i++) {
        lv_obj_remove_style(pageItems[i], &style_dropdown, LV_PART_MAIN);
    }
}

/**
 * Pick the associated function pointers for the globally configured actions
 */
static void update_inputs() {
    roller_callback = rollerActionFromId(g_setting.inputs.roller)->functionPtr.rollerFunction;
    btn_click_callback = btnActionFromId(g_setting.inputs.left_click)->functionPtr.btnFunction;
    btn_press_callback = btnActionFromId(g_setting.inputs.left_press)->functionPtr.btnFunction;
    rbtn_click_callback = btnActionFromId(g_setting.inputs.right_click)->functionPtr.btnFunction;
    rbtn_press_callback = btnActionFromId(g_setting.inputs.right_press)->functionPtr.btnFunction;
    rbtn_double_click_callback = btnActionFromId(g_setting.inputs.right_double_click)->functionPtr.btnFunction;
}

/**
 * Accept the current selection and write the associated function pointer to
 * the global target variable.
 */
static void accept_dropdown(lv_obj_t *obj) {
    const uint16_t selectedOption = lv_dropdown_get_selected(obj);

    if (selectedRow == ROLLER) {
        g_setting.inputs.roller = rollerActions[selectedOption].id;
        ini_putl("inputs", "roller", g_setting.inputs.roller, SETTING_INI);
    } else {
        switch (selectedRow) {
        case LEFT_SHORT:
            g_setting.inputs.left_click = btnActions[selectedOption].id;
            ini_putl("inputs", "left_click", g_setting.inputs.left_click, SETTING_INI);
            break;
        case LEFT_LONG:
            g_setting.inputs.left_press = btnActions[selectedOption].id;
            ini_putl("inputs", "left_press", g_setting.inputs.left_press, SETTING_INI);
            break;
        case RIGHT_SHORT:
            g_setting.inputs.right_click = btnActions[selectedOption].id;
            ini_putl("inputs", "right_click", g_setting.inputs.right_click, SETTING_INI);
            break;
        case RIGHT_LONG:
            g_setting.inputs.right_press = btnActions[selectedOption].id;
            ini_putl("inputs", "right_press", g_setting.inputs.right_press, SETTING_INI);
            break;
        case RIGHT_DOUBLE:
            g_setting.inputs.right_double_click = btnActions[selectedOption].id;
            ini_putl("inputs", "right_double_click", g_setting.inputs.right_double_click, SETTING_INI);
            break;
        default:
            break;
        }
    }

    update_inputs();
    lv_event_send(obj, LV_EVENT_RELEASED, NULL);
    lv_dropdown_close(obj);
    selectedRow = ROW_COUNT;
    app_state_push(APP_STATE_SUBMENU);
}

/**
 * Revert the currently selected dropdown option to the previously set value
 */
static void cancel_dropdown(lv_obj_t *obj) {
    lv_dropdown_set_selected(obj, previousSelection);
    accept_dropdown(obj);
}

/**
 * Check if any dropdown is currently opened
 */
static bool is_any_dropdown_open() {
    return selectedRow < BACK_BTN;
}

/**
 * Main allocation routine for this page
 */
static lv_obj_t *page_input_create(lv_obj_t *parent, panel_arr_t *arr) {
    char buf[256];

    char rollerOptionsStr[256] = "";
    build_options_string(rollerActions, ARRAY_SIZE(rollerActions), rollerOptionsStr);
    char btnOptionsStr[256] = "";
    build_options_string(btnActions, ARRAY_SIZE(btnActions), btnOptionsStr);

    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, UI_PAGE_VIEW_SIZE);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, UI_PAGE_VIEW_SIZE);

    snprintf(buf, sizeof(buf), "%s:", _lang("Input"));
    create_text(NULL, section, false, buf, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *content = lv_obj_create(section);
    lv_obj_set_size(content, UI_PAGE_VIEW_SIZE);
    lv_obj_set_pos(content, 0, 0);
    lv_obj_set_layout(content, LV_LAYOUT_GRID);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(content, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(content, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(content, row_dsc, 0);

    create_select_item(arr, content);

    snprintf(buf, sizeof(buf), "%s:", _lang("Roller"));
    create_label_item(content, buf, 1, ROLLER, 1);
    pageItems[ROLLER] = create_dropdown_item(content, rollerOptionsStr, 2, ROLLER, UI_INPUT_DROPDOWN_WIDTH, row_dsc[ROLLER], 2, 10, LV_GRID_ALIGN_START, UI_PAGE_TEXT_FONT);
    lv_dropdown_set_selected(pageItems[ROLLER], rollerIndexFromId(g_setting.inputs.roller));

    snprintf(buf, sizeof(buf), "%s:", _lang("Left short"));
    create_label_item(content, buf, 1, LEFT_SHORT, 1);
    pageItems[LEFT_SHORT] = create_dropdown_item(content, btnOptionsStr, 2, LEFT_SHORT, UI_INPUT_DROPDOWN_WIDTH, row_dsc[LEFT_SHORT], 2, 10, LV_GRID_ALIGN_START, UI_PAGE_TEXT_FONT);
    lv_dropdown_set_selected(pageItems[LEFT_SHORT], btnIndexFromId(g_setting.inputs.left_click));

    snprintf(buf, sizeof(buf), "%s:", _lang("Left long"));
    create_label_item(content, buf, 1, LEFT_LONG, 1);
    pageItems[LEFT_LONG] = create_dropdown_item(content, btnOptionsStr, 2, LEFT_LONG, UI_INPUT_DROPDOWN_WIDTH, row_dsc[LEFT_LONG], 2, 10, LV_GRID_ALIGN_START, UI_PAGE_TEXT_FONT);
    lv_dropdown_set_selected(pageItems[LEFT_LONG], btnIndexFromId(g_setting.inputs.left_press));

    snprintf(buf, sizeof(buf), "%s:", _lang("Right short"));
    create_label_item(content, buf, 1, RIGHT_SHORT, 1);
    pageItems[RIGHT_SHORT] = create_dropdown_item(content, btnOptionsStr, 2, RIGHT_SHORT, UI_INPUT_DROPDOWN_WIDTH, row_dsc[RIGHT_SHORT], 2, 10, LV_GRID_ALIGN_START, UI_PAGE_TEXT_FONT);
    lv_dropdown_set_selected(pageItems[RIGHT_SHORT], btnIndexFromId(g_setting.inputs.right_click));

    snprintf(buf, sizeof(buf), "%s:", _lang("Right long"));
    create_label_item(content, buf, 1, RIGHT_LONG, 1);
    pageItems[RIGHT_LONG] = create_dropdown_item(content, btnOptionsStr, 2, RIGHT_LONG, UI_INPUT_DROPDOWN_WIDTH, row_dsc[RIGHT_LONG], 2, 10, LV_GRID_ALIGN_START, UI_PAGE_TEXT_FONT);
    lv_dropdown_set_selected(pageItems[RIGHT_LONG], btnIndexFromId(g_setting.inputs.right_press));

    snprintf(buf, sizeof(buf), "%s:", _lang("Right double"));
    create_label_item(content, buf, 1, RIGHT_DOUBLE, 1);
    pageItems[RIGHT_DOUBLE] = create_dropdown_item(content, btnOptionsStr, 2, RIGHT_DOUBLE, UI_INPUT_DROPDOWN_WIDTH, row_dsc[RIGHT_DOUBLE], 2, 10, LV_GRID_ALIGN_START, UI_PAGE_TEXT_FONT);
    lv_dropdown_set_selected(pageItems[RIGHT_DOUBLE], btnIndexFromId(g_setting.inputs.right_double_click));

    snprintf(buf, sizeof(buf), "< %s", _lang("Back"));
    pageItems[BACK_BTN] = create_label_item(content, buf, 1, BACK_BTN, 1);

    lv_obj_t *label = lv_label_create(content);
    snprintf(buf, sizeof(buf), "*%s\n%s",
             _lang("Settings apply to video mode only"),
             _lang("'Toggle source' will switch between HDZero and Expansion module"));
    lv_label_set_text(label, buf);
    lv_obj_set_style_text_font(label, UI_PAGE_LABEL_FONT, 0);
    lv_obj_set_style_pad_top(label, UI_PAGE_TEXT_PAD, 0);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 1, 2, LV_GRID_ALIGN_START, pp_input.p_arr.max, 1);

    update_inputs();

    return page;
}

/**
 * Main entry routine for this page
 */
static void page_input_enter() {
    currentHighlight = 0;
    pp_input.p_arr.cur = currentHighlight;
    reset_dropdown_styles();
    lv_obj_add_style(pageItems[currentHighlight], &style_dropdown, LV_PART_MAIN);
}

/**
 * Main exit routine for this page
 */
static void page_input_exit() {
    if (is_any_dropdown_open()) {
        cancel_dropdown(pageItems[selectedRow]);
    }

    reset_dropdown_styles();
}

/**
 * Main navigation routine for this page
 */
static void page_input_on_roller(uint8_t key) {
    if (is_any_dropdown_open()) {
        lv_obj_t *const targetItem = pageItems[selectedRow];
        uint32_t evt = (key == DIAL_KEY_DOWN) ? LV_KEY_UP : LV_KEY_DOWN;

        lv_event_send(targetItem, LV_EVENT_KEY, &evt);
    } else {
        reset_dropdown_styles();

        if (key == DIAL_KEY_UP) {
            if (++currentHighlight == ROW_COUNT) {
                currentHighlight = 0;
            }
        } else if (key == DIAL_KEY_DOWN) {
            if (--currentHighlight < 0) {
                currentHighlight = BACK_BTN;
            }
        }

        if (pageItems[currentHighlight]->class_p == &lv_dropdown_class) {
            lv_obj_add_style(pageItems[currentHighlight], &style_dropdown, LV_PART_MAIN);
        }
    }
}

/**
 * Main input selection routine for this page
 */
static void page_input_on_click(uint8_t key, int sel) {
    LV_UNUSED(key);

    if ((rowType_t)sel >= BACK_BTN) {
        return;
    }

    if (is_any_dropdown_open()) {
        accept_dropdown(pageItems[selectedRow]);
    } else {
        selectedRow = (rowType_t)sel;
        lv_obj_t *const currentItem = pageItems[selectedRow];

        lv_dropdown_open(currentItem);
        lv_obj_t *const list = lv_dropdown_get_list(currentItem);
        lv_obj_add_style(list, &style_dropdown, LV_PART_MAIN);
        lv_obj_set_style_text_color(list, lv_color_make(0, 0, 0), LV_PART_SELECTED | LV_STATE_CHECKED);
        previousSelection = lv_dropdown_get_selected(currentItem);
        app_state_push(APP_STATE_SUBMENU_ITEM_FOCUSED);
    }
}

/**
 * Main menu page data structure
 */
page_pack_t pp_input = {
    .p_arr = {
        .cur = 0,
        .max = ROW_COUNT,
    },
    .name = "Input",
    .create = page_input_create,
    .enter = page_input_enter,
    .exit = page_input_exit,
    .on_roller = page_input_on_roller,
    .on_click = page_input_on_click,
    .on_right_button = NULL,
};

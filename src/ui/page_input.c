#include "page_input.h"

#include <minIni.h>

#include "core/app_state.h"
#include "core/common.hh"
#include "core/dvr.h"
#include "core/ht.h"
#include "core/input_device.h"
#include "core/sleep_mode.h"
#include "core/osd.h"

#include "ui/page_fans.h"
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

/**
 * Compile-unit local variables, constants and fields
 */
static lv_coord_t col_dsc[] = {160, 200, 160, 160, 160, 120, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, 60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};

const char *btnOptions[] = {"Toggle OSD", "Main menu", "Toggle DVR", "Center HT", "Calibrate HT", "Go Sleep!", "Toggle fan speed"};
void (* const btnFunctionPointers[])() = {&osd_toggle, &app_switch_to_menu, &dvr_toggle, &ht_set_center_position, &ht_calibrate, &go_sleep, &step_topfan};

const char *rollerOptions[] = {"Switch channel", "Change fan speed", "OLED Brightness"};
void (* const rollerFunctionPointers[])(uint8_t) = {&tune_channel, &change_topfan, &change_oled_brightness};
const uint16_t rollerDefaultOption = 0;

static rowType_t selectedRow = ROW_COUNT;
static lv_obj_t *pageItems[ROW_COUNT];
static int currentHighlight;
static uint16_t previousSelection;

/**
 * Build a '\n'-separated list of all available options for the dropdown element
 */
static void build_options_string(const char** input, size_t arraySize, char* output) {
    output[0] = 0;
    for (size_t i = 0; i < arraySize; i++) {
        strcat(output, input[i]);
        if (i < arraySize - 1) {
            strcat(output, "\n");
        }
    }
}

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
    roller_callback = rollerFunctionPointers[g_setting.inputs.roller];
    btn_click_callback = btnFunctionPointers[g_setting.inputs.left_click];
    btn_press_callback = btnFunctionPointers[g_setting.inputs.left_press];
    rbtn_click_callback = btnFunctionPointers[g_setting.inputs.right_click];
    rbtn_press_callback = btnFunctionPointers[g_setting.inputs.right_press];
    rbtn_double_click_callback = btnFunctionPointers[g_setting.inputs.right_double_click];
}

/**
 * Accept the current selection and write the associated function pointer to
 * the global target variable.
 */
static void accept_dropdown(lv_obj_t *obj) {
    const uint16_t selectedOption = lv_dropdown_get_selected(obj);

    if (selectedRow == ROLLER) {
        g_setting.inputs.roller = selectedOption;
        ini_putl("inputs", "roller", g_setting.inputs.roller, SETTING_INI);
    } else {
        switch (selectedRow) {
        case LEFT_SHORT:
            g_setting.inputs.left_click = selectedOption;
            ini_putl("inputs", "left_click", g_setting.inputs.left_click, SETTING_INI);
            break;
        case LEFT_LONG:
            g_setting.inputs.left_press = selectedOption;
            ini_putl("inputs", "left_press", g_setting.inputs.left_press, SETTING_INI);
            break;
        case RIGHT_SHORT:
            g_setting.inputs.right_click = selectedOption;
            ini_putl("inputs", "right_click", g_setting.inputs.right_click, SETTING_INI);
            break;
        case RIGHT_LONG:
            g_setting.inputs.right_press = selectedOption;
            ini_putl("inputs", "right_press", g_setting.inputs.right_press, SETTING_INI);
            break;
        case RIGHT_DOUBLE:
            g_setting.inputs.right_double_click = selectedOption;
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
    int contentHeight = 0;
    for (size_t i = 0; i < (ARRAY_SIZE(row_dsc) - 1); i++) {
        contentHeight += row_dsc[i];
    }
    int contentWidth = 0;
    for (size_t i = 0; i < (ARRAY_SIZE(col_dsc) - 1); i++) {
        contentWidth += col_dsc[i];
    }

    char rollerOptionsStr[128];
    build_options_string(rollerOptions, ARRAY_SIZE(rollerOptions), rollerOptionsStr);
    char btnOptionsStr[128];
    build_options_string(btnOptions, ARRAY_SIZE(btnOptions), btnOptionsStr);

    lv_obj_t *page = lv_menu_page_create(parent, NULL);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(page, contentWidth + 93, contentHeight + 300);
    lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
    lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
    lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
    lv_obj_set_size(section, contentWidth + 93, contentHeight + 294);

    create_text(NULL, section, false, "Inputs:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *content = lv_obj_create(section);
    lv_obj_set_size(content, contentWidth, contentHeight);
    lv_obj_set_pos(content, 0, 0);
    lv_obj_set_layout(content, LV_LAYOUT_GRID);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(content, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(content, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(content, row_dsc, 0);

    create_select_item(arr, content);

    create_label_item(content, "Roller:", 1, ROLLER, 1);
    pageItems[ROLLER] = create_dropdown_item(content, rollerOptionsStr, 2, ROLLER, 320, row_dsc[ROLLER], 2, 10, LV_GRID_ALIGN_START, &lv_font_montserrat_26);
    lv_dropdown_set_selected(pageItems[ROLLER], g_setting.inputs.roller);

    create_label_item(content, "Left short:", 1, LEFT_SHORT, 1);
    pageItems[LEFT_SHORT] = create_dropdown_item(content, btnOptionsStr, 2, LEFT_SHORT, 320, row_dsc[LEFT_SHORT], 2, 10, LV_GRID_ALIGN_START, &lv_font_montserrat_26);
    lv_dropdown_set_selected(pageItems[LEFT_SHORT], g_setting.inputs.left_click);

    create_label_item(content, "Left long:", 1, LEFT_LONG, 1);
    pageItems[LEFT_LONG] = create_dropdown_item(content, btnOptionsStr, 2, LEFT_LONG, 320, row_dsc[LEFT_LONG], 2, 10, LV_GRID_ALIGN_START, &lv_font_montserrat_26);
    lv_dropdown_set_selected(pageItems[LEFT_LONG], g_setting.inputs.left_press);

    create_label_item(content, "Right short:", 1, RIGHT_SHORT, 1);
    pageItems[RIGHT_SHORT] = create_dropdown_item(content, btnOptionsStr, 2, RIGHT_SHORT, 320, row_dsc[RIGHT_SHORT], 2, 10, LV_GRID_ALIGN_START, &lv_font_montserrat_26);
    lv_dropdown_set_selected(pageItems[RIGHT_SHORT], g_setting.inputs.right_click);

    create_label_item(content, "Right long:", 1, RIGHT_LONG, 1);
    pageItems[RIGHT_LONG] = create_dropdown_item(content, btnOptionsStr, 2, RIGHT_LONG, 320, row_dsc[RIGHT_LONG], 2, 10, LV_GRID_ALIGN_START, &lv_font_montserrat_26);
    lv_dropdown_set_selected(pageItems[RIGHT_LONG], g_setting.inputs.right_press);

    create_label_item(content, "Right double:", 1, RIGHT_DOUBLE, 1);
    pageItems[RIGHT_DOUBLE] = create_dropdown_item(content, btnOptionsStr, 2, RIGHT_DOUBLE, 320, row_dsc[RIGHT_DOUBLE], 2, 10, LV_GRID_ALIGN_START, &lv_font_montserrat_26);
    lv_dropdown_set_selected(pageItems[RIGHT_DOUBLE], g_setting.inputs.right_double_click);

    pageItems[BACK_BTN] = create_label_item(content, "< Back", 1, BACK_BTN, 1);

    lv_obj_t *label = lv_label_create(content);
    lv_label_set_text(label, "*Settings apply to video mode only");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_pad_top(label, 12, 0);
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
        lv_obj_t * const targetItem = pageItems[selectedRow];
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

    if ((rowType_t) sel >= BACK_BTN) {
        return;
    }

    if (is_any_dropdown_open()) {
        accept_dropdown(pageItems[selectedRow]);
    } else {
        selectedRow = (rowType_t)sel;
        lv_obj_t * const currentItem = pageItems[selectedRow];

        lv_dropdown_open(currentItem);
        lv_obj_t * const list = lv_dropdown_get_list(currentItem);
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

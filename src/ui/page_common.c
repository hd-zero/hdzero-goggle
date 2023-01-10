#include "page_common.h"

#include <stdio.h>
#include <stdlib.h>

#include "ui/ui_attribute.h"

///////////////////////////////////////////////////////////////////////////////
// global
setting_t g_setting;

op_level_t g_menu_op = OPLEVEL_MAINMENU;
bool g_sdcard_enable = false;
bool g_sdcard_det_req = false;
int g_sdcard_size = 0;
bool g_autoscan_exit = true;
bool g_scanning = false;
bool g_showRXOSD = true;
bool g_latency_locked = false;
bool g_test_en = false;
source_info_t g_source_info;
/////////////////////////////////////////////////////////////////////////////

LV_IMG_DECLARE(img_arrow);
LV_IMG_DECLARE(img_arrow0);
LV_IMG_DECLARE(img_arrow1);
int create_text(struct menu_obj_s *s, lv_obj_t *parent, bool is_icon, const char *txt,
                lv_menu_builder_variant_t builder_variant) {
    lv_obj_t *obj = lv_menu_cont_create(parent);

    lv_obj_t *img = NULL;
    lv_obj_t *label = NULL;

    if (txt) {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        //    lv_obj_set_flex_grow(label, 1);
    }
    if (is_icon) {
        img = lv_img_create(obj);
        lv_img_set_src(img, &img_arrow);
    }

    if (builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && is_icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    if (s) {
        s->cont = obj;
        s->icon = img;
    }
    return 0;
}

void create_select_item(panel_arr_t *arr, lv_obj_t *parent) {
    arr->panel0 = lv_obj_create(parent);
    arr->panel1 = lv_obj_create(parent);
    arr->panel2 = lv_obj_create(parent);
    arr->panel3 = lv_obj_create(parent);
    arr->panel4 = lv_obj_create(parent);
    arr->panel5 = lv_obj_create(parent);
    arr->panel6 = lv_obj_create(parent);
    arr->panel7 = lv_obj_create(parent);
    arr->panel8 = lv_obj_create(parent);

    lv_obj_clear_flag(arr->panel0, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(arr->panel1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(arr->panel2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(arr->panel3, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(arr->panel4, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(arr->panel5, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(arr->panel6, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(arr->panel7, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(arr->panel8, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(arr->panel0, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel3, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel4, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel5, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel6, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel7, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel8, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_style(arr->panel0, &style_select, LV_PART_MAIN);
    lv_obj_add_style(arr->panel1, &style_select, LV_PART_MAIN);
    lv_obj_add_style(arr->panel2, &style_select, LV_PART_MAIN);
    lv_obj_add_style(arr->panel3, &style_select, LV_PART_MAIN);
    lv_obj_add_style(arr->panel4, &style_select, LV_PART_MAIN);
    lv_obj_add_style(arr->panel5, &style_select, LV_PART_MAIN);
    lv_obj_add_style(arr->panel6, &style_select, LV_PART_MAIN);
    lv_obj_add_style(arr->panel7, &style_select, LV_PART_MAIN);
    lv_obj_add_style(arr->panel8, &style_select, LV_PART_MAIN);

    lv_obj_set_grid_cell(arr->panel0, LV_GRID_ALIGN_STRETCH, 0, 6,
                         LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_grid_cell(arr->panel1, LV_GRID_ALIGN_STRETCH, 0, 6,
                         LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_set_grid_cell(arr->panel2, LV_GRID_ALIGN_STRETCH, 0, 6,
                         LV_GRID_ALIGN_STRETCH, 2, 1);
    lv_obj_set_grid_cell(arr->panel3, LV_GRID_ALIGN_STRETCH, 0, 6,
                         LV_GRID_ALIGN_STRETCH, 3, 1);
    lv_obj_set_grid_cell(arr->panel4, LV_GRID_ALIGN_STRETCH, 0, 6,
                         LV_GRID_ALIGN_STRETCH, 4, 1);
    lv_obj_set_grid_cell(arr->panel5, LV_GRID_ALIGN_STRETCH, 0, 6,
                         LV_GRID_ALIGN_STRETCH, 5, 1);
    lv_obj_set_grid_cell(arr->panel6, LV_GRID_ALIGN_STRETCH, 0, 6,
                         LV_GRID_ALIGN_STRETCH, 6, 1);
    lv_obj_set_grid_cell(arr->panel7, LV_GRID_ALIGN_STRETCH, 0, 6,
                         LV_GRID_ALIGN_STRETCH, 7, 1);
    lv_obj_set_grid_cell(arr->panel8, LV_GRID_ALIGN_STRETCH, 0, 6,
                         LV_GRID_ALIGN_STRETCH, 8, 1);
}
void set_select_item(const panel_arr_t *arr, int row) {
    lv_obj_add_flag(arr->panel0, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel3, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel4, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel5, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel6, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel7, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(arr->panel8, LV_OBJ_FLAG_HIDDEN);

    switch (row) {
    case 0:
        lv_obj_clear_flag(arr->panel0, LV_OBJ_FLAG_HIDDEN);
        break;
    case 1:
        lv_obj_clear_flag(arr->panel1, LV_OBJ_FLAG_HIDDEN);
        break;
    case 2:
        lv_obj_clear_flag(arr->panel2, LV_OBJ_FLAG_HIDDEN);
        break;
    case 3:
        lv_obj_clear_flag(arr->panel3, LV_OBJ_FLAG_HIDDEN);
        break;
    case 4:
        lv_obj_clear_flag(arr->panel4, LV_OBJ_FLAG_HIDDEN);
        break;
    case 5:
        lv_obj_clear_flag(arr->panel5, LV_OBJ_FLAG_HIDDEN);
        break;
    case 6:
        lv_obj_clear_flag(arr->panel6, LV_OBJ_FLAG_HIDDEN);
        break;
    case 7:
        lv_obj_clear_flag(arr->panel7, LV_OBJ_FLAG_HIDDEN);
        break;
    case 8:
        lv_obj_clear_flag(arr->panel8, LV_OBJ_FLAG_HIDDEN);
        break;
    default:
        break;
    }
}

lv_obj_t *create_label_item(lv_obj_t *parent, const char *name, int col, int row, int cols) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_top(label, 12, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(label, 320 * cols, 60);

    lv_label_set_recolor(label, true);

    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, col, cols,
                         LV_GRID_ALIGN_CENTER, row, 1);
    return label;
}

void create_slider_item(slider_group_t *slider_group, lv_obj_t *parent, const char *name, int range, int default_value, int row) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_top(label, 12, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(label, 320, 60);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 1, 2,
                         LV_GRID_ALIGN_CENTER, row, 1);

    slider_group->slider = lv_slider_create(parent);

    lv_obj_remove_style_all(slider_group->slider);
    lv_obj_add_style(slider_group->slider, &style_silder_main, LV_PART_MAIN);
    lv_obj_add_style(slider_group->slider, &style_silder_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(slider_group->slider, &style_silder_pressed_color, LV_PART_INDICATOR | LV_STATE_PRESSED);
    lv_obj_add_style(slider_group->slider, &style_silder_knob, LV_PART_KNOB);
    lv_obj_add_style(slider_group->slider, &style_silder_pressed_color, LV_PART_KNOB | LV_STATE_PRESSED);

    lv_obj_set_size(slider_group->slider, 320, 3);
    lv_slider_set_range(slider_group->slider, 0, range);
    lv_slider_set_value(slider_group->slider, default_value, LV_ANIM_OFF);
    lv_obj_set_grid_cell(slider_group->slider, LV_GRID_ALIGN_STRETCH, 3, 2,
                         LV_GRID_ALIGN_CENTER, row, 1);

    slider_group->label = lv_label_create(parent);
    char buf[25];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", default_value);
    lv_label_set_text(slider_group->label, buf);
    lv_obj_set_style_text_font(slider_group->label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(slider_group->label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_top(slider_group->label, 12, 0);
    lv_label_set_long_mode(slider_group->label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(slider_group->label, 160, 60);
    lv_obj_set_grid_cell(slider_group->label, LV_GRID_ALIGN_START, 5, 1,
                         LV_GRID_ALIGN_CENTER, row, 1);
}

void create_btn_item(lv_obj_t *parent, const char *name, int col, int row) {

    lv_obj_t *btn = lv_btn_create(parent);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);

    lv_obj_set_style_text_font(btn, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(btn, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_bg_color(btn, lv_color_make(19, 19, 19), 0);
    lv_obj_set_style_bg_opa(btn, 0x0, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_pad_top(btn, 0, 0);
    lv_obj_set_size(btn, 160, 60);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, col, 1,
                         LV_GRID_ALIGN_CENTER, row, 1);
}

static lv_coord_t col_dsc[] = {40, 150, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60, LV_GRID_TEMPLATE_LAST};

static void create_btn_with_arrow(lv_obj_t *parent, btn_with_arr_t *btn_a, const char *name, int row, int col) {
    btn_a->container = lv_obj_create(parent);
    lv_obj_set_size(btn_a->container, 200, 60);
    lv_obj_set_pos(btn_a->container, 0, 0);
    lv_obj_set_layout(btn_a->container, LV_LAYOUT_GRID);
    lv_obj_clear_flag(btn_a->container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(btn_a->container, &style_context, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn_a->container, 0x0, 0);
    lv_obj_set_style_grid_column_dsc_array(btn_a->container, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(btn_a->container, row_dsc, 0);
    lv_obj_set_grid_cell(btn_a->container, LV_GRID_ALIGN_START, col, 1,
                         LV_GRID_ALIGN_CENTER, row, 1);

    btn_a->arrow = lv_img_create(btn_a->container);
    lv_img_set_src(btn_a->arrow, &img_arrow1);
    lv_obj_add_flag(btn_a->arrow, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_grid_cell(btn_a->arrow, LV_GRID_ALIGN_END, 0, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    btn_a->btn = lv_btn_create(btn_a->container);
    btn_a->label = lv_label_create(btn_a->btn);
    lv_label_set_text(btn_a->label, name);
    lv_obj_set_style_text_align(btn_a->label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_font(btn_a->btn, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(btn_a->btn, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_bg_color(btn_a->btn, lv_color_make(19, 19, 19), 0);
    lv_obj_set_style_bg_opa(btn_a->btn, 0x0, 0);
    lv_obj_set_style_shadow_width(btn_a->btn, 0, 0);
    lv_obj_set_style_pad_top(btn_a->btn, 12, 0);
    lv_obj_set_size(btn_a->btn, 160, 60);
    lv_obj_set_grid_cell(btn_a->btn, LV_GRID_ALIGN_START, 1, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_set_style_pad_column(btn_a->container, 0, 0);
}

void btn_group_set_sel(btn_group_t *btn_group, int sel) {
    // LOGI("set sel, valid=%d, sel=%d", btn_group->valid, sel);
    for (int i = 0; i < btn_group->valid; i++) {
        if (i == sel) {
            btn_group->current = sel;
            lv_obj_clear_flag(btn_group->btn_a[i].arrow, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(btn_group->btn_a[i].arrow, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

int btn_group_get_sel(btn_group_t *btn_group) {
    return btn_group->current;
}
void btn_group_toggle_sel(btn_group_t *btn_group) {
    int sel = btn_group_get_sel(btn_group);
    int total = btn_group->valid;

    if (sel < total - 1) {
        sel++;
    } else {
        sel = 0;
    }
    btn_group_set_sel(btn_group, sel);
}

void create_btn_group_item(btn_group_t *btn_group, lv_obj_t *parent, int count, const char *name, const char *name0, const char *name1, const char *name2, const char *name3, int row) {
    if (count > 3)
        return;
    btn_group->valid = count;
    btn_group->current = 0;

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_top(label, 12, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(label, 320, 60);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 1, 2,
                         LV_GRID_ALIGN_CENTER, row, 1);

    create_btn_with_arrow(parent, &btn_group->btn_a[0], name0, row, 2);
    if (count >= 2) {
        create_btn_with_arrow(parent, &btn_group->btn_a[1], name1, row, 3);
    }

    if (count >= 3) {
        create_btn_with_arrow(parent, &btn_group->btn_a[2], name2, row, 4);
    }

    btn_group_set_sel(btn_group, 0);
}

void create_btn_group_item2(btn_group_t *btn_group, lv_obj_t *parent, int count, const char *name, const char *name0, const char *name1,
                            const char *name2, const char *name3, const char *name4, const char *name5, int row) {
    if (count > 6)
        return;
    btn_group->valid = count;
    btn_group->current = 0;

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_top(label, 12, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(label, 320, 60);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 1, 2,
                         LV_GRID_ALIGN_CENTER, row, 1);

    create_btn_with_arrow(parent, &btn_group->btn_a[0], name0, row, 2);
    if (count >= 2) {
        create_btn_with_arrow(parent, &btn_group->btn_a[1], name1, row, 3);
    }

    if (count >= 3) {
        create_btn_with_arrow(parent, &btn_group->btn_a[2], name2, row, 4);
    }

    if (count >= 4) {
        create_btn_with_arrow(parent, &btn_group->btn_a[3], name3, row + 1, 2);
    }

    if (count >= 5) {
        create_btn_with_arrow(parent, &btn_group->btn_a[4], name4, row + 1, 3);
    }

    if (count == 6) {
        create_btn_with_arrow(parent, &btn_group->btn_a[5], name5, row + 1, 4);
    }

    btn_group_set_sel(btn_group, 0);
}

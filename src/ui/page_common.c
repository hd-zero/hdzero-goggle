#include "page_common.h"

#include <stdio.h>
#include <stdlib.h>

#include "ui/ui_attribute.h"

///////////////////////////////////////////////////////////////////////////////
bool g_sdcard_enable = false;
bool g_sdcard_det_req = false;
int g_sdcard_size = 0;
bool g_autoscan_exit = true;
bool g_scanning = false;
bool g_latency_locked = false;
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
    int i;
    for (i = 0; i < MAX_PANELS; ++i) {
        arr->panel[i] = lv_obj_create(parent);
        lv_obj_clear_flag(arr->panel[i], LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(arr->panel[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(arr->panel[i], FLAG_SELECTABLE);
        lv_obj_add_style(arr->panel[i], &style_select, LV_PART_MAIN);
        lv_obj_set_grid_cell(arr->panel[i], LV_GRID_ALIGN_STRETCH, 0, 6,
                             LV_GRID_ALIGN_STRETCH, i, 1);
    }
}
void set_select_item(const panel_arr_t *arr, int row) {
    int i;
    for (i = 0; i < MAX_PANELS; ++i) {
        lv_obj_add_flag(arr->panel[i], LV_OBJ_FLAG_HIDDEN);
    }

    if (row >= 0 && row < MAX_PANELS) {
        lv_obj_clear_flag(arr->panel[row], LV_OBJ_FLAG_HIDDEN);
    }
}

lv_obj_t *create_msgbox_item(const char *title, const char *message) {
    lv_obj_t *msgbox = lv_msgbox_create(lv_scr_act(), title, message, NULL, false);
    lv_obj_set_width(msgbox, 600);
    lv_obj_center(msgbox);

    lv_obj_set_style_bg_color(msgbox, lv_color_make(19, 19, 19), 0);

    lv_obj_set_style_text_color(lv_msgbox_get_title(msgbox), lv_color_make(0, 255, 0), 0);
    lv_obj_set_style_text_color(lv_msgbox_get_text(msgbox), lv_color_make(255, 255, 255), 0);

    lv_obj_set_style_border_width(msgbox, 3, 0);
    lv_obj_set_style_border_color(msgbox, lv_palette_main(LV_PALETTE_RED), 0);

    return msgbox;
}

lv_obj_t *create_label_item_compact(lv_obj_t *parent, const char *name, int col, int row, int cols, int height, lv_text_align_t text_align, lv_grid_align_t col_align, const lv_font_t *font) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_align(label, text_align, 0);
    lv_obj_set_style_pad_top(label, (height - lv_font_get_line_height(font)) >> 1, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(label, 120 * cols, height);

    lv_label_set_recolor(label, true);
    lv_obj_set_style_text_color(label, COLOR_DISABLED, STATE_DISABLED);

    lv_obj_set_grid_cell(label, col_align, col, cols,
                         LV_GRID_ALIGN_CENTER, row, 1);
    return label;
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
    lv_obj_set_style_text_color(label, COLOR_DISABLED, STATE_DISABLED);

    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, col, cols,
                         LV_GRID_ALIGN_CENTER, row, 1);
    return label;
}

lv_obj_t *create_info_item(lv_obj_t *parent, const char *name, int col, int row, int cols) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_top(label, 12, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(label, 320 * cols, 40);

    lv_label_set_recolor(label, true);

    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, col, cols,
                         LV_GRID_ALIGN_CENTER, row, 1);
    return label;
}

void create_slider_item_compact(slider_group_t *slider_group, lv_obj_t *parent, const char *name, int range, int default_value, int row, const lv_font_t *font) {
    slider_group->name = lv_label_create(parent);
    lv_label_set_text(slider_group->name, name);
    lv_obj_set_style_text_font(slider_group->name, font, 0);
    lv_obj_set_style_text_align(slider_group->name, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_top(slider_group->name, lv_font_get_line_height(font) >> 1, 0);
    lv_label_set_long_mode(slider_group->name, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(slider_group->name, 200, 40);
    lv_obj_set_grid_cell(slider_group->name, LV_GRID_ALIGN_START, 1, 1,
                         LV_GRID_ALIGN_CENTER, row, 1);
    lv_obj_set_style_text_color(slider_group->name, COLOR_DISABLED, STATE_DISABLED);

    slider_group->slider = lv_slider_create(parent);

    lv_obj_remove_style_all(slider_group->slider);
    lv_obj_add_style(slider_group->slider, &style_silder_main, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider_group->slider, COLOR_DISABLED, LV_PART_MAIN | STATE_DISABLED);

    lv_obj_add_style(slider_group->slider, &style_silder_indicator, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_group->slider, COLOR_DISABLED, LV_PART_INDICATOR | STATE_DISABLED);
    lv_obj_add_style(slider_group->slider, &style_silder_pressed_color, LV_PART_INDICATOR | LV_STATE_PRESSED);

    lv_obj_set_style_bg_opa(slider_group->slider, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_pad_ver(slider_group->slider, 10, LV_PART_KNOB);
    lv_obj_set_style_pad_hor(slider_group->slider, 2, LV_PART_KNOB);
    lv_obj_set_style_bg_color(slider_group->slider, COLOR_DISABLED, LV_PART_KNOB | STATE_DISABLED);
    lv_obj_add_style(slider_group->slider, &style_silder_pressed_color, LV_PART_KNOB | LV_STATE_PRESSED);

    lv_obj_set_size(slider_group->slider, 0, 2);
    lv_slider_set_range(slider_group->slider, 0, range);
    lv_slider_set_value(slider_group->slider, default_value, LV_ANIM_OFF);
    lv_obj_set_grid_cell(slider_group->slider, LV_GRID_ALIGN_STRETCH, 2, 2,
                         LV_GRID_ALIGN_CENTER, row, 1);

    slider_group->label = lv_label_create(parent);
    char buf[25];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", default_value);
    lv_label_set_text(slider_group->label, buf);
    lv_obj_set_style_text_font(slider_group->label, font, 0);
    lv_obj_set_style_text_align(slider_group->label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_top(slider_group->label, lv_font_get_line_height(font) >> 1, 0);
    lv_label_set_long_mode(slider_group->label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(slider_group->label, 160, 40);
    lv_obj_set_grid_cell(slider_group->label, LV_GRID_ALIGN_START, 4, 1,
                         LV_GRID_ALIGN_CENTER, row, 1);
    lv_obj_set_style_text_color(slider_group->label, COLOR_DISABLED, STATE_DISABLED);
}

void create_slider_item(slider_group_t *slider_group, lv_obj_t *parent, const char *name, int range, int default_value, int row) {
    slider_group->name = lv_label_create(parent);
    lv_label_set_text(slider_group->name, name);
    lv_obj_set_style_text_font(slider_group->name, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(slider_group->name, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_top(slider_group->name, 12, 0);
    lv_label_set_long_mode(slider_group->name, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(slider_group->name, 320, 60);
    lv_obj_set_grid_cell(slider_group->name, LV_GRID_ALIGN_START, 1, 2,
                         LV_GRID_ALIGN_CENTER, row, 1);
    lv_obj_set_style_text_color(slider_group->name, COLOR_DISABLED, STATE_DISABLED);

    slider_group->slider = lv_slider_create(parent);

    lv_obj_remove_style_all(slider_group->slider);
    lv_obj_add_style(slider_group->slider, &style_silder_main, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider_group->slider, COLOR_DISABLED, LV_PART_MAIN | STATE_DISABLED);
    
    lv_obj_add_style(slider_group->slider, &style_silder_indicator, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_group->slider, COLOR_DISABLED, LV_PART_INDICATOR | STATE_DISABLED);
    lv_obj_add_style(slider_group->slider, &style_silder_pressed_color, LV_PART_INDICATOR | LV_STATE_PRESSED);

    lv_obj_add_style(slider_group->slider, &style_silder_knob, LV_PART_KNOB);
    lv_obj_set_style_bg_color(slider_group->slider, COLOR_DISABLED, LV_PART_KNOB | STATE_DISABLED);
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
    lv_obj_set_style_text_color(slider_group->label, COLOR_DISABLED, STATE_DISABLED);
}

void update_slider_item_with_value(slider_group_t *slider_group, int value) {
    char str[20];
    sprintf(str, "%d", value);
    lv_slider_set_value(slider_group->slider, value, LV_ANIM_OFF);
    lv_label_set_text(slider_group->label, str);
}

void create_btn_item(lv_obj_t *parent, const char *name, int col, int row) {

    lv_obj_t *btn = lv_btn_create(parent);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);

    lv_obj_set_style_text_font(btn, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(btn, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_bg_color(btn, lv_color_make(19, 19, 19), 0);
    lv_obj_set_style_bg_color(btn, COLOR_DISABLED, STATE_DISABLED);
    lv_obj_set_style_bg_opa(btn, 0x0, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_pad_top(btn, 0, 0);
    lv_obj_set_size(btn, 160, 60);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, col, 1,
                         LV_GRID_ALIGN_CENTER, row, 1);
}

lv_obj_t *create_dropdown_item(lv_obj_t *parent, const char *options, int col, int row, int width, int height, int col_span, int pad_top, lv_grid_align_t column_align, const lv_font_t *font) {

    lv_obj_t *obj = lv_dropdown_create(parent);
    lv_dropdown_set_options(obj, options);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_shadow_width(obj, 0, 0);
    lv_obj_set_style_pad_top(obj, pad_top, 0);
    lv_obj_set_size(obj, width, height);
    lv_obj_set_style_text_color(obj, COLOR_DISABLED, STATE_DISABLED);

    lv_obj_set_grid_cell(obj, column_align, col, col_span, LV_GRID_ALIGN_CENTER, row, 1);

    return obj;
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
    lv_obj_set_style_text_color(btn_a->label, COLOR_DISABLED, STATE_DISABLED);

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

static void create_btn_with_arrow_compact(lv_obj_t *parent, btn_with_arr_t *btn_a, const char *name, int row, int col, int height, int arrow_scale_percent, const lv_font_t *font) {
    uint16_t zoom_factor = (arrow_scale_percent * 256) / 100;

    btn_a->container = lv_obj_create(parent);
    lv_obj_set_size(btn_a->container, 200, height);
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
    lv_img_set_zoom(btn_a->arrow, zoom_factor);

    lv_obj_set_style_pad_top(btn_a->arrow, 0, 0);
    lv_obj_set_style_pad_bottom(btn_a->arrow, height >> 1, 0);

    lv_obj_add_flag(btn_a->arrow, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_grid_cell(btn_a->arrow, LV_GRID_ALIGN_END, 0, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    btn_a->btn = lv_btn_create(btn_a->container);
    btn_a->label = lv_label_create(btn_a->btn);
    lv_label_set_text(btn_a->label, name);
    lv_obj_set_style_text_align(btn_a->label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_font(btn_a->btn, font, 0);
    lv_obj_set_style_text_align(btn_a->btn, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_bg_color(btn_a->btn, lv_color_make(19, 19, 19), 0);
    lv_obj_set_style_bg_opa(btn_a->btn, 0x0, 0);
    lv_obj_set_style_shadow_width(btn_a->btn, 0, 0);
    lv_obj_set_style_text_color(btn_a->label, COLOR_DISABLED, STATE_DISABLED);

    lv_obj_set_style_pad_top(btn_a->btn, 0, 0);
    lv_obj_set_style_pad_bottom(btn_a->btn, 0, 0);

    lv_obj_set_size(btn_a->btn, 160, height);
    lv_obj_set_grid_cell(btn_a->btn, LV_GRID_ALIGN_START, 1, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_set_style_pad_column(btn_a->container, 0, 0);
}

void create_btn_group_item_compact(btn_group_t *btn_group, lv_obj_t *parent, int count, const char *name, const char *name0, const char *name1, const char *name2, const char *name3, int row, int height, int arrow_scale_percent, const lv_font_t *font) {
    if (count > 3)
        return;
    btn_group->valid = count;
    btn_group->current = 0;

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_top(label, 10, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(label, 120, height);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 1, 1,
                         LV_GRID_ALIGN_CENTER, row, 1);

    create_btn_with_arrow_compact(parent, &btn_group->btn_a[0], name0, row, 2, height, arrow_scale_percent, font);
    if (count >= 2) {
        create_btn_with_arrow_compact(parent, &btn_group->btn_a[1], name1, row, 3, height, arrow_scale_percent, font);
    }

    if (count >= 3) {
        create_btn_with_arrow_compact(parent, &btn_group->btn_a[2], name2, row, 4, height, arrow_scale_percent, font);
    }

    btn_group_set_sel(btn_group, 0);
}

void create_btn_group_item(btn_group_t *btn_group, lv_obj_t *parent, int count, const char *name, const char *name0, const char *name1, const char *name2, const char *name3, int row) {
    if (count > 3)
        return;
    btn_group->valid = count;
    btn_group->current = 0;

    btn_group->label = lv_label_create(parent);
    lv_label_set_text(btn_group->label, name);
    lv_obj_set_style_text_font(btn_group->label, &lv_font_montserrat_26, 0);
    lv_obj_set_style_text_align(btn_group->label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_top(btn_group->label, 12, 0);
    lv_label_set_long_mode(btn_group->label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_size(btn_group->label, 320, 60);
    lv_obj_set_grid_cell(btn_group->label, LV_GRID_ALIGN_START, 1, 2,
                         LV_GRID_ALIGN_CENTER, row, 1);
    lv_obj_set_style_text_color(btn_group->label, COLOR_DISABLED, STATE_DISABLED);

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
    lv_obj_set_style_text_color(label, COLOR_DISABLED, STATE_DISABLED);

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

void slider_show(slider_group_t *slider_group, bool visible) {
    if (visible) {
        lv_obj_clear_flag(slider_group->slider, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(slider_group->name, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(slider_group->label, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(slider_group->slider, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(slider_group->name, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(slider_group->label, LV_OBJ_FLAG_HIDDEN);
    }
}

void slider_enable(slider_group_t *slider_group, bool enable) {
    if (enable) {
        lv_obj_clear_state(slider_group->slider, STATE_DISABLED);
        lv_obj_clear_state(slider_group->label, STATE_DISABLED);
        lv_obj_clear_state(slider_group->name, STATE_DISABLED);
    } else {
        lv_obj_add_state(slider_group->slider, STATE_DISABLED);
        lv_obj_add_state(slider_group->label, STATE_DISABLED);
        lv_obj_add_state(slider_group->name, STATE_DISABLED);
    }
}

void btn_group_show(btn_group_t *btn_group, bool visible) {
    for (int i = 0; i < btn_group->valid; ++i) {
        if (visible) {
            lv_obj_clear_flag(btn_group->btn_a[i].label, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(btn_group->btn_a[i].label, LV_OBJ_FLAG_HIDDEN);
        }
    }

    int sel = btn_group_get_sel(btn_group);
    if (visible) {
        lv_obj_clear_flag(btn_group->btn_a[sel].arrow, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_group->btn_a[sel].label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_group->label, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(btn_group->btn_a[sel].arrow, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_group->btn_a[sel].label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_group->label, LV_OBJ_FLAG_HIDDEN);
    }
}

void btn_group_enable(btn_group_t *btn_group, bool enable) {
    for (int i = 0; i < btn_group->valid; ++i) {
        if (enable) {
            lv_obj_clear_state(btn_group->btn_a[i].label, STATE_DISABLED);
        } else {
            lv_obj_add_state(btn_group->btn_a[i].label, STATE_DISABLED);
        }
    }

    const int sel = btn_group_get_sel(btn_group);
    if (enable) {
        lv_obj_clear_state(btn_group->label, STATE_DISABLED);
        lv_obj_clear_flag(btn_group->btn_a[sel].arrow, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_state(btn_group->label, STATE_DISABLED);
        lv_obj_add_flag(btn_group->btn_a[sel].arrow, LV_OBJ_FLAG_HIDDEN);
    }
}

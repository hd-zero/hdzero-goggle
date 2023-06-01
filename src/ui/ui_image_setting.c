#include "ui/ui_image_setting.h"

#include <stdio.h>

#include <log/log.h>
#include <lvgl/lvgl.h>
#include <minIni.h>

#include "core/common.hh"
#include "core/osd.h"
#include "driver/hardware.h"
#include "driver/oled.h"
#include "ui/page_common.h"

///////////////////////////////////////////////////////////////////////////////
// locals
static lv_obj_t *canvas_ims;
static uint8_t canvas_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(IMS_CANVAS_WIDTH, IMS_CANVAS_HEIGHT)];
static lv_draw_line_dsc_t line_dsc;
static lv_draw_label_dsc_t label_dsc;
static ims_page_t ims_page;
static uint8_t ims_state;
// global
bool g_bShowIMS = false;

///////////////////////////////////////////////////////////////////////////////
//
static void ims_page_init(uint8_t *val) {
    int16_t x = 30;
    int16_t y = 15;

    ims_page.items[0].x = x;
    ims_page.items[0].y = y;
    ims_page.items[0].type = 1;
    strcpy(ims_page.items[0].title, "OLED:");
    ims_page.items[0].range[0] = 0;
    ims_page.items[0].range[1] = 12;
    ims_page.items[0].value = val[0];
    ims_page.items[0].state = 1;
    ims_page.selection = 0;

    ims_page.items[1].x = x;
    ims_page.items[1].y = y + 25;
    ims_page.items[1].type = 1;
    strcpy(ims_page.items[1].title, "Brightness:");
    ims_page.items[1].range[0] = 0;
    ims_page.items[1].range[1] = 78;
    ims_page.items[1].value = val[1];
    ims_page.items[1].state = 0;

    ims_page.items[2].x = x;
    ims_page.items[2].y = y + 50;
    ims_page.items[2].type = 1;
    strcpy(ims_page.items[2].title, "Saturation:");
    ims_page.items[2].range[0] = 0;
    ims_page.items[2].range[1] = 47;
    ims_page.items[2].value = val[2];
    ims_page.items[2].state = 0;

    ims_page.items[3].x = x;
    ims_page.items[3].y = y + 75;
    ims_page.items[3].type = 1;
    strcpy(ims_page.items[3].title, "Contrast:");
    ims_page.items[3].range[0] = 0;
    ims_page.items[3].range[1] = 47;
    ims_page.items[3].value = val[3];
    ims_page.items[3].state = 0;

    ims_page.items[4].x = x;
    ims_page.items[4].y = y + 100;
    ims_page.items[4].type = 1;
    strcpy(ims_page.items[4].title, "OLED Auto off:");
    ims_page.items[4].range[0] = 0;
    ims_page.items[4].range[1] = 4;
    ims_page.items[4].value = val[4];
    ims_page.items[4].state = 0;

    ims_page.items[5].x = x;
    ims_page.items[5].y = y + 125;
    ims_page.items[5].type = 0;
    strcpy(ims_page.items[5].title, "< Back");
    ims_page.items[5].state = 0;

    ims_page.items[6].x = x + 200;
    ims_page.items[6].y = y + 125;
    ims_page.items[6].type = 0;
    strcpy(ims_page.items[6].title, "Reset All");
    ims_page.items[6].state = 0;
}

static void show_ims_slider(uint8_t index) {
    ims_slider_t *p_slider = &ims_page.items[index];

    char buf[16];
    lv_point_t points[2];

    if (p_slider->state == 0) { // 0=not selected, 1=selected, 2=slider bar selected
        label_dsc.color = DARK_GRAY;
        line_dsc.color = DARK_GRAY;
    } else if (p_slider->state == 1) {
        label_dsc.color = LIGHT_WHITE;
        line_dsc.color = LIGHT_WHITE;
    } else {
        label_dsc.color = LIGHT_WHITE;
        line_dsc.color = LIGHT_GREEN;
    }

    lv_canvas_draw_text(canvas_ims, p_slider->x, p_slider->y, 200, &label_dsc, p_slider->title);
    if (p_slider->type == 0) {
        return;
    }

    switch (index) {
    case 4: { // auto off
        char *str_ao[5] = {"1 min", "3 min", "5 min", "7 min", "Never"};
        strcpy(buf, str_ao[p_slider->value]);
        break;
    }

    default:
        sprintf(buf, "%d", p_slider->value);
        break;
    }

    lv_canvas_draw_text(canvas_ims, 340 + p_slider->x, p_slider->y, 200, &label_dsc, buf);

    line_dsc.width = 3;
    points[0].x = SLIDER_XSTART;
    points[1].x = SLIDER_XEND;
    points[0].y = points[1].y = p_slider->y + 6;
    lv_canvas_draw_line(canvas_ims, points, 2, &line_dsc);

    line_dsc.width = 12;
    points[0].y = points[1].y = p_slider->y + 6;
    points[0].x = SLIDER_XSTART + p_slider->value * (SLIDER_XEND - SLIDER_XSTART - SLIDER_WIDTH) / (p_slider->range[1] - p_slider->range[0]);
    points[1].x = points[0].x + SLIDER_WIDTH;
    lv_canvas_draw_line(canvas_ims, points, 2, &line_dsc);
}

void ims_update() {
    if (!g_bShowIMS) {
        lv_obj_add_flag(canvas_ims, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    // Clear background
    lv_obj_clear_flag(canvas_ims, LV_OBJ_FLAG_HIDDEN);
    lv_canvas_fill_bg(canvas_ims, DARK, LV_OPA_COVER);

    // Draw each items
    for (uint8_t i = 0; i < IMS_ITEM_COUNT; i++) {
        show_ims_slider(i);
    }
}

void ims_init(void) {
    uint8_t defs[5];

    defs[0] = g_setting.image.oled;
    defs[1] = g_setting.image.brightness;
    defs[2] = g_setting.image.saturation;
    defs[3] = g_setting.image.contrast;
    defs[4] = g_setting.image.auto_off;

    canvas_ims = lv_canvas_create(lv_scr_act());
    lv_obj_clear_flag(canvas_ims, LV_OBJ_FLAG_SCROLLABLE);

    lv_canvas_set_buffer(canvas_ims, canvas_buf, IMS_CANVAS_WIDTH, IMS_CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align(canvas_ims, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(canvas_ims, LV_OBJ_FLAG_HIDDEN);
    lv_draw_line_dsc_init(&line_dsc);
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = &lv_font_montserrat_16; // LV_FONT_DEFAULT;
    ims_page_init(defs);
    g_bShowIMS = false;

    OLED_Brightness(defs[0]);
    Set_Brightness(defs[1]);
    Set_Saturation(defs[2]);
    Set_Contrast(defs[3]);
}

void ims_save() {
    g_setting.image.oled = ims_page.items[0].value;
    ini_putl("image", "oled", g_setting.image.oled, SETTING_INI);
    OLED_Brightness(g_setting.image.oled);

    g_setting.image.brightness = ims_page.items[1].value;
    ini_putl("image", "brightness", g_setting.image.brightness, SETTING_INI);
    Set_Brightness(g_setting.image.brightness);

    g_setting.image.saturation = ims_page.items[2].value;
    ini_putl("image", "saturation", g_setting.image.saturation, SETTING_INI);
    Set_Saturation(g_setting.image.saturation);

    g_setting.image.contrast = ims_page.items[3].value;
    ini_putl("image", "contrast", g_setting.image.contrast, SETTING_INI);
    Set_Contrast(g_setting.image.contrast);

    g_setting.image.auto_off = ims_page.items[4].value;
    ini_putl("image", "auto_off", g_setting.image.auto_off, SETTING_INI);

    osd_update_element_positions();
}

///////////////////////////////////////////////////////////////////////////////
// key:
//   1 = dial up
//   2 = dial down
//   3 = click
// return
//   1 = ims done
//   0 = ongoing
uint8_t ims_key(uint8_t key) {
    int16_t value;
    uint8_t ret = 0;
    LOGI("ims_key key: %d state: %d selection: %d", key, ims_state, ims_page.selection);

    if (ims_state == 0) {
        ims_state = 1;
    }

    if (ims_state == 1) { // select between items
        g_bShowIMS = true;

        switch (key) {
        case DIAL_KEY_UP:
            ims_page.items[ims_page.selection].state = 0;
            ims_page.selection++;
            if (ims_page.selection == IMS_ITEM_COUNT)
                ims_page.selection = 0;
            ims_page.items[ims_page.selection].state = 1;
            break;

        case DIAL_KEY_DOWN:
            ims_page.items[ims_page.selection].state = 0;
            if (ims_page.selection == 0)
                ims_page.selection = IMS_ITEM_COUNT - 1;
            else
                ims_page.selection--;
            ims_page.items[ims_page.selection].state = 1;
            break;

        case DIAL_KEY_CLICK:
            if (ims_page.selection == 5) { //"<Back"
                ims_state = 0;
                g_bShowIMS = false;
                ims_save();
                ret = 1;
            } else if (ims_page.selection == 6) { //"Reset All"
                ims_page.items[0].value = g_setting_defaults.image.oled;
                ims_page.items[1].value = g_setting_defaults.image.brightness;
                ims_page.items[2].value = g_setting_defaults.image.saturation;
                ims_page.items[3].value = g_setting_defaults.image.contrast;
                ims_page.items[4].value = g_setting_defaults.image.auto_off;
                ims_save();
            } else {
                ims_page.items[ims_page.selection].state = 2;
                ims_state = 2;
            }
            break;

        default:
            LOGE("ims_key unhandled key %d", key);
            ims_state = 0;
            break;
        }
    } else if (ims_state == 2) { // tune up/down values
        g_bShowIMS = true;
        value = 0;

        switch (key) {
        case DIAL_KEY_DOWN:
            value = ims_page.items[ims_page.selection].value;
            if (value != ims_page.items[ims_page.selection].range[1]) {
                value++;
                ims_page.items[ims_page.selection].value = value;
            }
            break;

        case DIAL_KEY_UP:
            value = ims_page.items[ims_page.selection].value;
            if (value != ims_page.items[ims_page.selection].range[0]) {
                value--;
                ims_page.items[ims_page.selection].value = value;
            }
            break;

        case DIAL_KEY_CLICK:
            value = ims_page.items[ims_page.selection].value;
            ims_page.items[ims_page.selection].state = 1;
            ims_state = 1;
            break;

        default:
            break;
        }

        switch (ims_page.selection) {
        case 0:
            OLED_Brightness(value);
            break;

        case 1:
            Set_Brightness(value);
            break;

        case 2:
            Set_Saturation(value);
            break;

        case 3:
            Set_Contrast(value);
            break;

        case 4:
            g_setting.image.auto_off = ims_page.items[4].value;
            ini_putl("image", "auto_off", g_setting.image.auto_off, SETTING_INI);
            break;

        default:
            LOGE("ims_key unhandled selection %d", ims_page.selection);
            ims_state = 0;
            g_bShowIMS = false;
            break;
        }
    }
    return ret;
}

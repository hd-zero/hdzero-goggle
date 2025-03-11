#include "ui/ui_statusbar.h"

#include <stdio.h>

#include <lvgl/lvgl.h>

#include "core/battery.h"
#include "core/common.hh"
#include "core/osd.h"
#include "core/settings.h"
#include "driver/beep.h"
#include "lang/language.h"
#include "ui/page_common.h"
#include "ui/page_playback.h"
#include "ui/page_storage.h"
#include "ui/page_wifi.h"
#include "ui/ui_porting.h"
#include "ui/ui_style.h"
#include "util/sdcard.h"

///////////////////////////////////////////////////////////////////////////////
// local
enum STATUS {
    STS_SDCARD,
    STS_SOURCE,
    STS_ELRS,
    STS_WIFI,
    STS_BATT,

    STS_TOTAL
};

static lv_obj_t *label[STS_TOTAL];

static lv_obj_t *img_sdc;
LV_IMG_DECLARE(img_sdcard);
LV_IMG_DECLARE(img_noSdcard);

static lv_obj_t *img_battery;
LV_IMG_DECLARE(img_bat);
LV_IMG_DECLARE(img_lowBattery);

int statusbar_init(void) {
    char buf[128];

    static lv_coord_t col_dsc[] = {264, 64, 267, 64, 267, 64, 267, 64, 267, 64, 267, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {48, 96, LV_GRID_TEMPLATE_LAST};

    /*Create a container with grid*/
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, DRAW_HOR_RES_FHD, 96);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(cont, lv_color_make(19, 19, 19), 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_radius(cont, 0, 0);
    lv_obj_set_style_pad_row(cont, 0, 0);
    lv_obj_set_style_pad_column(cont, 0, 0);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

    LV_IMG_DECLARE(img_logo);
    lv_obj_t *img0 = lv_img_create(cont);
    lv_img_set_src(img0, &img_logo);
    lv_obj_set_size(img0, 264, 96);
    lv_obj_set_grid_cell(img0, LV_GRID_ALIGN_CENTER, 0, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    img_sdc = lv_img_create(cont);
    lv_img_set_src(img_sdc, &img_sdcard);
    lv_obj_set_size(img_sdc, 64, 96);
    lv_obj_set_grid_cell(img_sdc, LV_GRID_ALIGN_CENTER, 1, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    LV_IMG_DECLARE(img_ic);
    lv_obj_t *img2 = lv_img_create(cont);
    lv_img_set_src(img2, &img_ic);
    lv_obj_set_size(img2, 64, 96);
    lv_obj_set_grid_cell(img2, LV_GRID_ALIGN_CENTER, 3, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    LV_IMG_DECLARE(img_esp);
    lv_obj_t *img3 = lv_img_create(cont);
    lv_img_set_src(img3, &img_esp);
    lv_obj_set_size(img3, 64, 96);
    lv_obj_set_grid_cell(img3, LV_GRID_ALIGN_CENTER, 5, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    LV_IMG_DECLARE(img_wifi);
    lv_obj_t *img4 = lv_img_create(cont);
    lv_img_set_src(img4, &img_wifi);
    lv_obj_set_size(img4, 64, 96);
    lv_obj_set_grid_cell(img4, LV_GRID_ALIGN_CENTER, 7, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    img_battery = lv_img_create(cont);
    lv_img_set_src(img_battery, &img_bat);
    lv_obj_set_size(img_battery, 64, 96);
    lv_obj_set_grid_cell(img_battery, LV_GRID_ALIGN_CENTER, 9, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    for (int i = 0; i < STS_TOTAL; ++i) {
        label[i] = lv_label_create(cont);
        lv_obj_set_width(label[i], 267); /*Set smaller width to make the lines wrap*/
        lv_obj_set_style_text_align(label[i], LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_color(label[i], lv_color_make(255, 255, 255), 0);
        lv_obj_set_style_text_font(label[i], &lv_font_montserrat_26, 0);

        if (i == STS_SDCARD) {
            lv_label_set_long_mode(label[i], LV_LABEL_LONG_SCROLL_CIRCULAR);
        } else {
            lv_label_set_long_mode(label[i], LV_LABEL_LONG_DOT); /*Break the long lines*/
        }

        lv_obj_set_grid_cell(label[i], LV_GRID_ALIGN_CENTER, ((i + 1) * 2), 1, LV_GRID_ALIGN_CENTER, 0, 1);
    }

    snprintf(buf, sizeof(buf), "%s                 ", _lang("SD Card"));

    lv_label_set_text(label[STS_SDCARD], buf);
    lv_label_set_recolor(label[STS_SDCARD], true);

    snprintf(buf, sizeof(buf), "%s: HDZero %s", _lang("RF"), channel2str(g_setting.source.hdzero_band, g_setting.scan.channel & 0x7F));
    lv_label_set_text(label[STS_SOURCE], buf);

    snprintf(buf, sizeof(buf), "ELRS: %s", _lang("Off"));
    lv_label_set_text(label[STS_ELRS], buf);

    snprintf(buf, sizeof(buf), "WiFi: %s", _lang("Off"));
    lv_label_set_text(label[STS_WIFI], buf);

    lv_label_set_text(label[STS_BATT], "       ");
    return 0;
}

void statubar_update(void) {
    char buf[128];
    memset(buf, 0, sizeof(buf));

    // display battery voltage
    battery_get_voltage_str(buf);
    lv_label_set_text(label[STS_BATT], buf);

    {
#define BEEP_INTERVAL 20
        static uint8_t beep_gap = 0;

        const bool low = battery_is_low();
        if (low)
            lv_img_set_src(img_battery, &img_lowBattery);
        else
            lv_img_set_src(img_battery, &img_bat);

        switch (g_setting.power.warning_type) {
        case SETTING_POWER_WARNING_TYPE_BEEP:
            if (low) {
                if (beep_gap++ == BEEP_INTERVAL) {
                    beep();
                    beep_gap = 0;
                }
                lv_obj_set_style_text_color(label[STS_BATT], lv_color_make(255, 255, 255), 0);
            }
            break;

        case SETTING_POWER_WARNING_TYPE_VISUAL:
            if (low)
                lv_obj_set_style_text_color(label[STS_BATT], lv_color_make(255, 0, 0), 0);
            else
                lv_obj_set_style_text_color(label[STS_BATT], lv_color_make(255, 255, 255), 0);
            break;

        case SETTING_POWER_WARNING_TYPE_BOTH:
            if (low) {
                if (beep_gap++ == BEEP_INTERVAL) {
                    beep();
                    beep_gap = 0;
                }
                lv_obj_set_style_text_color(label[STS_BATT], lv_color_make(255, 0, 0), 0);
            } else
                lv_obj_set_style_text_color(label[STS_BATT], lv_color_make(255, 255, 255), 0);
            break;
        default:
            break;
        }
    }

    static int channel_last = 0;
    static source_t source_last = SOURCE_HDZERO;
    static setting_sources_hdzero_band_t hdzero_band_last = SETTING_SOURCES_HDZERO_BAND_RACEBAND;
    if ((channel_last != g_setting.scan.channel) || (source_last != g_source_info.source) || (hdzero_band_last != g_setting.source.hdzero_band)) {
        memset(buf, 0, sizeof(buf));
        if (g_source_info.source == SOURCE_HDZERO) { // HDZero
            int ch = g_setting.scan.channel & 0x7F;
            snprintf(buf, sizeof(buf), "%s: HDZero %s", _lang("RF"), channel2str(g_setting.source.hdzero_band, g_setting.scan.channel & 0x7F));
        } else if (g_source_info.source == SOURCE_HDMI_IN)
            snprintf(buf, sizeof(buf), "HDMI %s", _lang("In"));
        else if (g_source_info.source == SOURCE_AV_IN)
            snprintf(buf, sizeof(buf), "AV %s", _lang("In"));
        else
            snprintf(buf, sizeof(buf), "%s", _lang("Expansion Module"));

        lv_label_set_text(label[STS_SOURCE], buf);
    }
    channel_last = g_setting.scan.channel;
    source_last = g_source_info.source;
    hdzero_band_last = g_setting.source.hdzero_band;

    if (page_storage_is_sd_repair_active()) {
        lv_img_set_src(img_sdc, &img_sdcard);
        lv_label_set_text(label[STS_SDCARD], _lang("Integrity check"));
    } else {
        if (g_sdcard_enable) {
            int cnt = get_videofile_cnt();
            float gb = sdcard_free_size() / 1024.0;
            lv_img_set_src(img_sdc, &img_sdcard);
            if (cnt != 0) {
                if (sdcard_is_full())
                    snprintf(buf, sizeof(buf), "%d %s, %s %s", cnt, _lang("clip(s)"), _lang("SD Card"), _lang("full"));
                else
                    snprintf(buf, sizeof(buf), "%d %s, %.2fGB %s", cnt, _lang("clip(s)"), gb, _lang("available"));
            } else {
                if (sdcard_is_full())
                    snprintf(buf, sizeof(buf), "#FF0000 %s %s#", _lang("SD Card"), _lang("full"));
                else
                    snprintf(buf, sizeof(buf), "%.2fGB %s", gb, _lang("available"));
            }
        } else {
            lv_img_set_src(img_sdc, &img_noSdcard);

            if (sdcard_inserted()) {
                strcpy(buf, _lang("Unsupported"));
            } else {
                strcpy(buf, _lang("No SD Card"));
            }
        }

        lv_label_set_text(label[STS_SDCARD], buf);
    }

    if (g_setting.elrs.enable) {
        snprintf(buf, sizeof(buf), "ELRS: %s ", _lang("On"));
        lv_label_set_text(label[STS_ELRS], buf);
    } else {
        snprintf(buf, sizeof(buf), "ELRS: %s ", _lang("Off"));
        lv_label_set_text(label[STS_ELRS], buf);
    }

    page_wifi_get_statusbar_text(buf, sizeof(buf));
    lv_label_set_text(label[STS_WIFI], buf);
}

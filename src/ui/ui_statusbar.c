#include "ui/ui_statusbar.h"

#include <stdio.h>

#include "../conf/ui.h"

#include "core/battery.h"
#include "core/common.hh"
#include "core/osd.h"
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
enum STATUS_GOGGLE {
    STS_GOGGLE_SDCARD,
    STS_GOGGLE_SOURCE,
    STS_GOGGLE_ELRS,
    STS_GOGGLE_WIFI,
    STS_GOGGLE_BATT,
    STS_GOGGLE_TOTAL
};

enum STATUS_BOXLITE {
    STS_BOXLITE_SDCARD,
    STS_BOXLITE_SOURCE,
    STS_BOXLITE_BATT,
    STS_BOXLITE_TOTAL
};

#define STS_SDCARD (g_setting.has_all_features ? STS_GOGGLE_SDCARD : STS_BOXLITE_SDCARD)
#define STS_SOURCE (g_setting.has_all_features ? STS_GOGGLE_SOURCE : STS_BOXLITE_SOURCE)
#define STS_ELRS   (STS_GOGGLE_ELRS)
#define STS_WIFI   (STS_GOGGLE_WIFI)
#define STS_BATT   (g_setting.has_all_features ? STS_GOGGLE_BATT : STS_BOXLITE_BATT)
#define STS_TOTAL  (g_setting.has_all_features ? STS_GOGGLE_TOTAL : STS_BOXLITE_TOTAL)

static lv_obj_t *label[STS_GOGGLE_TOTAL];
static lv_obj_t *img_sdc;
static lv_obj_t *img_battery;

LV_IMG_DECLARE(img_bat);
LV_IMG_DECLARE(img_esp);
LV_IMG_DECLARE(img_ic);
LV_IMG_DECLARE(img_logo);
LV_IMG_DECLARE(img_lowBattery);
LV_IMG_DECLARE(img_sdcard);
LV_IMG_DECLARE(img_noSdcard);
LV_IMG_DECLARE(img_wifi);

int statusbar_init(void) {
    char buf[128];

    lv_coord_t *col_dsc = UI_STATUS_BAR_COLS();
    lv_coord_t *row_dsc = UI_STATUS_BAR_ROWS();

    /*Create a container with grid*/
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, DRAW_HOR_RES_FHD, img_logo.header.h);
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

    lv_obj_t *img0 = lv_img_create(cont);
    lv_img_set_src(img0, &img_logo);
    lv_obj_set_size(img0, img_logo.header.w, img_logo.header.h);
    lv_obj_set_grid_cell(img0, LV_GRID_ALIGN_CENTER, 0, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    img_sdc = lv_img_create(cont);
    lv_img_set_src(img_sdc, &img_sdcard);
    lv_obj_set_size(img_sdc, img_sdcard.header.w, img_sdcard.header.h);
    lv_obj_set_grid_cell(img_sdc, LV_GRID_ALIGN_CENTER, 1, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_t *img2 = lv_img_create(cont);
    lv_img_set_src(img2, &img_ic);
    lv_obj_set_size(img2, img_ic.header.w, img_ic.header.h);
    lv_obj_set_grid_cell(img2, LV_GRID_ALIGN_CENTER, 3, 1,
                         LV_GRID_ALIGN_CENTER, 0, 1);

    if (g_setting.has_all_features) {
        lv_obj_t *img3 = lv_img_create(cont);
        lv_img_set_src(img3, &img_esp);
        lv_obj_set_size(img3, img_esp.header.w, img_esp.header.h);
        lv_obj_set_grid_cell(img3, LV_GRID_ALIGN_CENTER, 5, 1,
                             LV_GRID_ALIGN_CENTER, 0, 1);

        lv_obj_t *img4 = lv_img_create(cont);
        lv_img_set_src(img4, &img_wifi);
        lv_obj_set_size(img4, img_wifi.header.w, img_wifi.header.h);
        lv_obj_set_grid_cell(img4, LV_GRID_ALIGN_CENTER, 7, 1,
                             LV_GRID_ALIGN_CENTER, 0, 1);
    }

    img_battery = lv_img_create(cont);
    lv_img_set_src(img_battery, &img_bat);
    lv_obj_set_size(img_battery, img_bat.header.w, img_bat.header.h);
    lv_obj_set_grid_cell(img_battery, LV_GRID_ALIGN_CENTER, (g_setting.has_all_features ? 9 : 5), 1, LV_GRID_ALIGN_CENTER, 0, 1);

    for (int i = 0; i < STS_TOTAL; ++i) {
        label[i] = lv_label_create(cont);
        lv_obj_set_width(label[i], UI_STATUS_BAR_LABEL_WIDTH()); /*Set smaller width to make the lines wrap*/
        lv_obj_set_style_text_align(label[i], LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_set_style_text_color(label[i], lv_color_make(255, 255, 255), 0);
        lv_obj_set_style_text_font(label[i], UI_MENU_ENTRY_FONT, 0);

        if (i == STS_SDCARD) {
            lv_label_set_long_mode(label[i], LV_LABEL_LONG_SCROLL_CIRCULAR);
        } else {
            lv_label_set_long_mode(label[i], LV_LABEL_LONG_DOT); /*Break the long lines*/
        }

        lv_obj_set_grid_cell(label[i], LV_GRID_ALIGN_CENTER, ((i + 1) * 2), 1, LV_GRID_ALIGN_CENTER, 0, 1);
    }

    snprintf(buf, sizeof(buf), "%s", _lang("Detecting"));

    lv_label_set_text(label[STS_SDCARD], buf);
    lv_label_set_recolor(label[STS_SDCARD], true);

    if (g_source_info.source == SOURCE_HDZERO) {
        snprintf(buf, sizeof(buf), "%s: HDZero %s", _lang("RF"), channel2str(1, g_setting.source.hdzero_band, g_setting.scan.channel & 0x7F));
    } else if (g_source_info.source == SOURCE_HDMI_IN) {
        snprintf(buf, sizeof(buf), "HDMI %s", _lang("In"));
    } else if (g_source_info.source == SOURCE_AV_IN) {
        snprintf(buf, sizeof(buf), "AV %s", _lang("In"));
    } else if (g_source_info.source == SOURCE_AV_MODULE) {
        if (TARGET_GOGGLE == getTargetType()) {
            sprintf(buf, "%s: %s", _lang("RF"), _lang("Analog"));
        } else if (TARGET_BOXPRO == getTargetType()) {
            sprintf(buf, "%s: %s %s", _lang("RF"), _lang("Analog"), channel2str(0, 0, g_setting.source.analog_channel));
        }
    } else {
        sprintf(buf, " ");
    }

    lv_label_set_text(label[STS_SOURCE], buf);

    if (g_setting.has_all_features) {
        snprintf(buf, sizeof(buf), "ELRS: %s", _lang("Off"));
        lv_label_set_text(label[STS_ELRS], buf);

        snprintf(buf, sizeof(buf), "WiFi: %s", _lang("Off"));
        lv_label_set_text(label[STS_WIFI], buf);
    }

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

    static int hdzero_channel_last = 0;
    static int analog_channel_last = 0;
    static source_t source_last = SOURCE_HDZERO;
    static setting_sources_hdzero_band_t hdzero_band_last = SETTING_SOURCES_HDZERO_BAND_RACEBAND;
    uint8_t channel_changed = (hdzero_channel_last != g_setting.scan.channel) || (analog_channel_last != g_setting.source.analog_channel);
    if (channel_changed || (source_last != g_source_info.source) || (hdzero_band_last != g_setting.source.hdzero_band)) {
        memset(buf, 0, sizeof(buf));
        if (g_source_info.source == SOURCE_HDZERO) {
            snprintf(buf, sizeof(buf), "%s: HDZero %s", _lang("RF"), channel2str(1, g_setting.source.hdzero_band, g_setting.scan.channel & 0x7F));
        } else if (g_source_info.source == SOURCE_HDMI_IN) {
            snprintf(buf, sizeof(buf), "HDMI %s", _lang("In"));
        } else if (g_source_info.source == SOURCE_AV_IN) {
            snprintf(buf, sizeof(buf), "AV %s", _lang("In"));
        } else if (g_source_info.source == SOURCE_AV_MODULE) {
            if (TARGET_GOGGLE == getTargetType()) {
                sprintf(buf, "%s: %s", _lang("RF"), _lang("Analog"));
            } else if (TARGET_BOXPRO == getTargetType()) {
                sprintf(buf, "%s: %s %s", _lang("RF"), _lang("Analog"), channel2str(0, 0, g_setting.source.analog_channel));
            }
        } else {
            sprintf(buf, " ");
        }

        lv_label_set_text(label[STS_SOURCE], buf);
    }

    hdzero_channel_last = g_setting.scan.channel;
    analog_channel_last = g_setting.source.analog_channel;
    source_last = g_source_info.source;
    hdzero_band_last = g_setting.source.hdzero_band;

    if (page_storage_is_sd_repair_active()) {
        lv_img_set_src(img_sdc, &img_sdcard);
        lv_label_set_text(label[STS_SDCARD], _lang("Integrity check"));
    } else if (-1 == g_bootup_sdcard_state) {
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

    if (g_setting.has_all_features) {
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
}

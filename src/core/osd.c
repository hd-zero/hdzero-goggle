#include "osd.h"

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <log/log.h>
#include <lvgl/lvgl.h>
#include <minIni.h>

#include "core/app_state.h"
#include "core/battery.h"
#include "core/common.hh"
#include "core/dvr.h"
#include "core/elrs.h"
#include "core/msp_displayport.h"
#include "core/settings.h"
#include "driver/dm5680.h"
#include "driver/fans.h"
#include "driver/fbtools.h"
#include "driver/hardware.h"
#include "driver/nct75.h"
#include "driver/rtc.h"
#include "ui/page_common.h"
#include "ui/page_fans.h"
#include "ui/page_scannow.h"
#include "ui/ui_image_setting.h"
#include "ui/ui_porting.h"

extern const lv_font_t conthrax_26;
extern const lv_font_t robotomono_26;

typedef enum {
    FC_VARIANT_UNKNOWN = 0,
    FC_VARIANT_ARDU,
    FC_VARIANT_BTFL,
    FC_VARIANT_EMUF,
    FC_VARIANT_INAV,
    FC_VARIANT_QUIC
} fc_variant_t;

//////////////////////////////////////////////////////////////////
// local
static sem_t osd_semaphore;
static osd_resource_t is_fhd;
static fc_variant_t g_fc_variant_type = FC_VARIANT_UNKNOWN;

static uint16_t osd_buf_shadow[HD_VMAX][HD_HMAX];
static char clock_date[32] = {"2023/08/10"},
            clock_time[32] = {"12:00:00"},
            clock_format[8] = {"PM"};
static int clock_format_offsets[OSD_RESOURCE_TOTAL];

extern lv_style_t style_osd;
extern pthread_mutex_t lvgl_mutex;
extern int gif_cnt;

// Use SDCARD for Embedded Glyph if the glyph exists otherwise use goggle FS
void osd_resource_path(char *buf, const char *fmt, osd_resource_t osd_resource_type, ...) {
    char filename[128];
    char buf2[128];

    va_list args;
    va_start(args, osd_resource_type);
    vsprintf(filename, fmt, args);
    va_end(args);
    strcpy(buf2, buf);
    strcpy(buf, RESOURCE_PATH_SDCARD);

    if (osd_resource_type == OSD_RESOURCE_1080) {
        strcat(buf, "FHD/");
    }
    strcat(buf, filename);

    if (access(buf, F_OK) != 0) {
        strcpy(buf, buf2);
        strcpy(buf, RESOURCE_PATH);
        if (osd_resource_type == OSD_RESOURCE_1080) {
            strcat(buf, "FHD/");
        }
        strcat(buf, filename);
    }

    memmove(buf + 2, buf, strlen(buf) + 2);
    buf[0] = 'A';
    buf[1] = ':';
}

void osd_toggle() {
    g_setting.osd.is_visible = !g_setting.osd.is_visible;
    if (g_setting.osd.is_visible) {
        channel_osd_mode = CHANNEL_SHOWTIME;
    }

    settings_put_bool("osd", "is_visible", g_setting.osd.is_visible);
}

///////////////////////////////////////////////////////////////////////////////
// these are local for OSD controlling
static osd_hdzero_t g_osd_hdzero;
static lv_obj_t *img_arr[2][HD_VMAX][HD_HMAX];
static lv_obj_t *scr_main;
static lv_obj_t *scr_osd[2];                                                    // 0=720p,1=1080p
static uint32_t osdFont_hd[OSD_VNUM][OSD_HNUM][OSD_HEIGHT_HD][OSD_WIDTH_HD];    // 0x00bbggrr
static uint32_t osdFont_fhd[OSD_VNUM][OSD_HNUM][OSD_HEIGHT_FHD][OSD_WIDTH_FHD]; // 0x00bbggrr
static osd_font_t osd_font_hd;
static osd_font_t osd_font_fhd;

void osd_llock_show(bool bShow) {
    char buf[128];

    if (!bShow || !g_setting.osd.element[OSD_GOGGLE_LATENCY_LOCK].show) {
        lv_obj_add_flag(g_osd_hdzero.latency_lock[is_fhd], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if (g_latency_locked) {
        osd_resource_path(buf, "%s", is_fhd, LLOCK_bmp);
        lv_img_set_src(g_osd_hdzero.latency_lock[is_fhd], buf);
        lv_obj_clear_flag(g_osd_hdzero.latency_lock[is_fhd], LV_OBJ_FLAG_HIDDEN);
    } else
        lv_obj_add_flag(g_osd_hdzero.latency_lock[is_fhd], LV_OBJ_FLAG_HIDDEN);
}

void osd_rec_show(bool bShow) {
    char buf[128];

    if (!bShow || !g_setting.osd.element[OSD_GOGGLE_SD_REC].show) {
        lv_obj_add_flag(g_osd_hdzero.sd_rec[is_fhd], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if (!g_sdcard_enable) {
        osd_resource_path(buf, "%s", is_fhd, noSdcard_bmp);
        lv_img_set_src(g_osd_hdzero.sd_rec[is_fhd], buf);
        lv_obj_clear_flag(g_osd_hdzero.sd_rec[is_fhd], LV_OBJ_FLAG_HIDDEN);
    } else {
        if (dvr_is_recording) {
            osd_resource_path(buf, "%s", is_fhd, recording_bmp);
            lv_img_set_src(g_osd_hdzero.sd_rec[is_fhd], buf);
            lv_obj_clear_flag(g_osd_hdzero.sd_rec[is_fhd], LV_OBJ_FLAG_HIDDEN);
        } else
            lv_obj_add_flag(g_osd_hdzero.sd_rec[is_fhd], LV_OBJ_FLAG_HIDDEN);
    }
}

void osd_battery_low_show() {
    char buf[128];
    if (g_setting.power.warning_type == SETTING_POWER_WARNING_TYPE_BEEP) { // Beep only
        lv_obj_add_flag(g_osd_hdzero.battery_low[is_fhd], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if (battery_is_low() && g_setting.osd.element[OSD_GOGGLE_BATTERY_LOW].show) {
        osd_resource_path(buf, "%s", is_fhd, lowBattery_gif);
        lv_gif_set_src(g_osd_hdzero.battery_low[is_fhd], buf);
        lv_obj_clear_flag(g_osd_hdzero.battery_low[is_fhd], LV_OBJ_FLAG_HIDDEN);
    } else
        lv_obj_add_flag(g_osd_hdzero.battery_low[is_fhd], LV_OBJ_FLAG_HIDDEN);
}

void osd_battery_voltage_show(bool bShow) {
    if (!bShow || !g_setting.osd.element[OSD_GOGGLE_BATTERY_VOLTAGE].show) {
        lv_obj_add_flag(g_osd_hdzero.battery_voltage[is_fhd], LV_OBJ_FLAG_HIDDEN);
        return;
    }
    char buf[128];

    battery_get_voltage_str(buf);
    lv_label_set_text(g_osd_hdzero.battery_voltage[is_fhd], buf);

    if (battery_is_low())
        lv_obj_set_style_text_color(g_osd_hdzero.battery_voltage[is_fhd], lv_color_make(255, 0, 0), 0);
    else
        lv_obj_set_style_text_color(g_osd_hdzero.battery_voltage[is_fhd], lv_color_make(255, 255, 255), 0);

    lv_obj_clear_flag(g_osd_hdzero.battery_voltage[is_fhd], LV_OBJ_FLAG_HIDDEN);
}

void osd_clock_date_show(bool bShow) {
    if (!bShow || !g_setting.osd.element[OSD_GOGGLE_CLOCK_DATE].show) {
        lv_obj_add_flag(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_DATE], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_label_set_text(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_DATE], clock_date);
    lv_obj_set_style_text_color(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_DATE], lv_color_make(255, 255, 255), 0);
    lv_obj_clear_flag(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_DATE], LV_OBJ_FLAG_HIDDEN);
}

void osd_clock_time_show(bool bShow) {
    if (!bShow || !g_setting.osd.element[OSD_GOGGLE_CLOCK_TIME].show) {
        lv_obj_add_flag(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_TIME], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_FORMAT], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_label_set_text(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_TIME], clock_time);
    lv_label_set_text(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_FORMAT], clock_format);
    lv_obj_set_style_text_color(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_TIME], lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_color(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_FORMAT], lv_color_make(255, 255, 255), 0);
    lv_obj_clear_flag(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_TIME], LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_FORMAT], LV_OBJ_FLAG_HIDDEN);
}

void osd_clock_show(bool bShow) {
    // Update clock
    rtc_get_clock_osd_str(clock_date, sizeof(clock_date),
                          clock_time, sizeof(clock_time),
                          clock_format, sizeof(clock_format));

    osd_clock_date_show(bShow);
    osd_clock_time_show(bShow);
}

void osd_topfan_show(bool bShow) {
    char buf[128];
    if (!bShow || !g_setting.osd.element[OSD_GOGGLE_TOPFAN_SPEED].show) {
        lv_obj_add_flag(g_osd_hdzero.topfan_speed[is_fhd], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if (g_setting.fans.top_speed > 5)
        return;
    osd_resource_path(buf, "fan%d.bmp", is_fhd, fan_speed.top);
    lv_img_set_src(g_osd_hdzero.topfan_speed[is_fhd], buf);
    lv_obj_clear_flag(g_osd_hdzero.topfan_speed[is_fhd], LV_OBJ_FLAG_HIDDEN);
}

void osd_vrxtemp_show() {
    char buf[128];
    if (g_temperature.is_overheat && g_setting.osd.element[OSD_GOGGLE_VRX_TEMP].show) {
        osd_resource_path(buf, "%s", is_fhd, VrxTemp7_gif);
        lv_gif_set_src(g_osd_hdzero.vrx_temp[is_fhd], buf);
        lv_obj_clear_flag(g_osd_hdzero.vrx_temp[is_fhd], LV_OBJ_FLAG_HIDDEN);
    } else
        lv_obj_add_flag(g_osd_hdzero.vrx_temp[is_fhd], LV_OBJ_FLAG_HIDDEN);
}

void osd_vlq_show(bool bShow) {
    char buf[128];
    if (!bShow || !g_setting.osd.element[OSD_GOGGLE_VLQ].show) {
        lv_obj_add_flag(g_osd_hdzero.vlq[is_fhd], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if (link_quality > 8)
        return;
    if (rx_status[0].rx_valid || rx_status[1].rx_valid) {
        osd_resource_path(buf, "VLQ%d.bmp", is_fhd, link_quality + 1);
    } else {
        osd_resource_path(buf, "%s", is_fhd, "VLQ1.bmp");
    }

    lv_img_set_src(g_osd_hdzero.vlq[is_fhd], buf);
    lv_obj_clear_flag(g_osd_hdzero.vlq[is_fhd], LV_OBJ_FLAG_HIDDEN);
}

///////////////////////////////////:////////////////////////////////////////////
// OSD channel
// channel_osd_mode
//  = 0x80 | Channel
//  = 0x00 | Channel Show Time
uint8_t channel_osd_mode;

char *channel2str(uint8_t band, uint8_t channel) // channel=[1:18]
{
    static char *ChannelName[2][BASE_CH_NUM] = {
        {"R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "E1", "F1", "F2", "F4"},
        {"L1", "L2", "L3", "L4", "L5", "L6", "L7", "L8", "  ", "  ", "  ", "  "},
    };

    if ((channel > 0) && (channel <= CHANNEL_NUM))
        return ChannelName[band][channel - 1];
    else
        return ChannelName[band][0];
}

void osd_channel_show(bool bShow) {
    uint8_t ch;
    lv_color_t color;
    char buf[32];

    if (channel_osd_mode & 0x80) {
        ch = channel_osd_mode & 0x7F;
        color = lv_color_make(0xFF, 0x20, 0x20);
        sprintf(buf, "  To %s?  ", channel2str(g_setting.source.hdzero_band, ch));
        lv_obj_set_style_bg_opa(g_osd_hdzero.channel[is_fhd], LV_OPA_100, 0);
    } else {
        ch = g_setting.scan.channel & 0x7F;
        color = lv_color_make(0xFF, 0xFF, 0xFF);
        sprintf(buf, "CH:%s", channel2str(g_setting.source.hdzero_band, ch));
        lv_obj_set_style_bg_opa(g_osd_hdzero.channel[is_fhd], 0, 0);
    }

    lv_label_set_text(g_osd_hdzero.channel[is_fhd], buf);
    lv_obj_set_style_text_color(g_osd_hdzero.channel[is_fhd], color, 0);

    if (channel_osd_mode & 0x80)
        lv_obj_clear_flag(g_osd_hdzero.channel[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else if (bShow && channel_osd_mode && g_setting.osd.element[OSD_GOGGLE_CHANNEL].show)
        lv_obj_clear_flag(g_osd_hdzero.channel[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.channel[is_fhd], LV_OBJ_FLAG_HIDDEN);
}

static void osd_object_set_pos(uint8_t fhd, lv_obj_t *obj, setting_osd_goggle_element_positions_t *pos) {
    int x = (g_setting.osd.embedded_mode == EMBEDDED_16x9) ? pos->mode_16_9.x : pos->mode_4_3.x;
    int y = (g_setting.osd.embedded_mode == EMBEDDED_16x9) ? pos->mode_16_9.y : pos->mode_4_3.y;

    if (fhd) {
        x += x >> 1;
        y += y >> 1;
    }
    lv_obj_set_pos(obj, x, y);
}

static void osd_object_create_gif(uint8_t fhd, lv_obj_t **obj, const char *img, setting_osd_goggle_element_positions_t *pos, lv_obj_t *so) {
    *obj = lv_gif_create(so);
    lv_gif_set_src(*obj, img);
    if (fhd)
        lv_obj_set_size(*obj, 54, 54);
    else
        lv_obj_set_size(*obj, 36, 36);
    osd_object_set_pos(fhd, *obj, pos);
}

static void osd_object_create_img(uint8_t fhd, lv_obj_t **obj, const char *img, setting_osd_goggle_element_positions_t *pos, lv_obj_t *so) {
    *obj = lv_img_create(so);
    lv_img_set_src(*obj, img);
    if (fhd)
        lv_obj_set_size(*obj, 54, 54);
    else
        lv_obj_set_size(*obj, 36, 36);
    osd_object_set_pos(fhd, *obj, pos);
}

static void osd_object_create_label(uint8_t fhd, lv_obj_t **obj, char *text, setting_osd_goggle_element_positions_t *pos, lv_obj_t *so) {
    *obj = lv_label_create(so);
    lv_label_set_text(*obj, text);
    osd_object_set_pos(fhd, *obj, pos);

    lv_obj_set_style_text_color(*obj, lv_color_make(255, 255, 255), 0);

    switch (g_fc_variant_type) {
    case FC_VARIANT_ARDU:
    case FC_VARIANT_BTFL:
    case FC_VARIANT_EMUF:
    case FC_VARIANT_INAV:
        lv_obj_set_style_text_font(*obj, &conthrax_26, 0);
        break;
    case FC_VARIANT_QUIC:
        lv_obj_set_style_text_font(*obj, &robotomono_26, 0);
        break;
    default:
        lv_obj_set_style_text_font(*obj, &lv_font_montserrat_26, 0);
        break;
    }
}

void osd_show(bool show) {
    if (show)
        lv_obj_clear_flag(scr_osd[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else {
        lv_obj_add_flag(scr_osd[0], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(scr_osd[1], LV_OBJ_FLAG_HIDDEN);
    }
}

uint8_t RSSI2Ant(uint8_t rssi) {
    uint8_t ret, thr[5] = {0x10, 0x30, 0x50, 0x70, 0x90};

    if (rssi < thr[0])
        ret = 6;
    else if (rssi < thr[1])
        ret = 5;
    else if (rssi < thr[2])
        ret = 4;
    else if (rssi < thr[3])
        ret = 3;
    else if (rssi < thr[4])
        ret = 2;
    else
        ret = 1;

    return ret;
}

bool fhd_change() {
    if (fhd_req) {
        osd_show(false);
        if (fhd_req == 1) {
            lvgl_switch_to_1080p();
            osd_fhd(1);
            // LOGI("fhd_change to 1080p");
        } else {
            lvgl_switch_to_720p();
            osd_fhd(0);
            // LOGI("fhd_change to 720p");
        }
        osd_clear();
        osd_show(true);
        lv_timer_handler();
        fhd_req = 0;
        return true;
    }
    return false;
}

void osd_show_all_elements() {
    if (g_setting.osd.element[OSD_GOGGLE_CLOCK_DATE].show)
        lv_obj_clear_flag(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_DATE], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_DATE], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_CLOCK_TIME].show) {
        lv_obj_clear_flag(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_TIME], LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_FORMAT], LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_TIME], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(g_osd_hdzero.clock[is_fhd][OSD_CLOCK_FORMAT], LV_OBJ_FLAG_HIDDEN);
    }

    if (g_setting.osd.element[OSD_GOGGLE_TOPFAN_SPEED].show)
        lv_obj_clear_flag(g_osd_hdzero.topfan_speed[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.topfan_speed[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_BATTERY_LOW].show)
        lv_obj_clear_flag(g_osd_hdzero.battery_low[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.battery_low[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_BATTERY_VOLTAGE].show)
        lv_obj_clear_flag(g_osd_hdzero.battery_voltage[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.battery_voltage[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_VTX_TEMP].show)
        lv_obj_clear_flag(g_osd_hdzero.vtx_temp[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.vtx_temp[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_VRX_TEMP].show)
        lv_obj_clear_flag(g_osd_hdzero.vrx_temp[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.vrx_temp[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_LATENCY_LOCK].show)
        lv_obj_clear_flag(g_osd_hdzero.latency_lock[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.latency_lock[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_CHANNEL].show)
        lv_obj_clear_flag(g_osd_hdzero.channel[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.channel[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_SD_REC].show)
        lv_obj_clear_flag(g_osd_hdzero.sd_rec[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.sd_rec[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_VLQ].show)
        lv_obj_clear_flag(g_osd_hdzero.vlq[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.vlq[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_ANT0].show)
        lv_obj_clear_flag(g_osd_hdzero.ant0[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant0[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_ANT1].show)
        lv_obj_clear_flag(g_osd_hdzero.ant1[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant1[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_ANT2].show)
        lv_obj_clear_flag(g_osd_hdzero.ant2[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant2[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_ANT3].show)
        lv_obj_clear_flag(g_osd_hdzero.ant3[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant3[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (!g_setting.storage.selftest)
        return;

    if (g_setting.osd.element[OSD_GOGGLE_TEMP_TOP].show)
        lv_obj_clear_flag(g_osd_hdzero.osd_tempe[is_fhd][0], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.osd_tempe[is_fhd][0], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_TEMP_LEFT].show)
        lv_obj_clear_flag(g_osd_hdzero.osd_tempe[is_fhd][1], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.osd_tempe[is_fhd][1], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_TEMP_RIGHT].show)
        lv_obj_clear_flag(g_osd_hdzero.osd_tempe[is_fhd][2], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.osd_tempe[is_fhd][2], LV_OBJ_FLAG_HIDDEN);
}

void osd_elements_set_dummy_sources() {
    char buf[128];

    osd_resource_path(buf, "%s", is_fhd, VtxTemp1_bmp);
    lv_img_set_src(g_osd_hdzero.vtx_temp[is_fhd], buf);

    osd_resource_path(buf, "%s", is_fhd, ant2_bmp);
    lv_img_set_src(g_osd_hdzero.ant0[is_fhd], buf);

    osd_resource_path(buf, "%s", is_fhd, ant3_bmp);
    lv_img_set_src(g_osd_hdzero.ant1[is_fhd], buf);

    osd_resource_path(buf, "%s", is_fhd, ant4_bmp);
    lv_img_set_src(g_osd_hdzero.ant2[is_fhd], buf);

    osd_resource_path(buf, "%s", is_fhd, ant5_bmp);
    lv_img_set_src(g_osd_hdzero.ant3[is_fhd], buf);

    osd_resource_path(buf, "%s", is_fhd, recording_bmp);
    lv_img_set_src(g_osd_hdzero.sd_rec[is_fhd], buf);

    osd_resource_path(buf, "%s", is_fhd, VLQ9_bmp);
    lv_img_set_src(g_osd_hdzero.vlq[is_fhd], buf);

    osd_resource_path(buf, "%s", is_fhd, fan5_bmp);
    lv_img_set_src(g_osd_hdzero.topfan_speed[is_fhd], buf);
}

#define FC_OSD_CHECK_PERIOD 200 // 25ms
void osd_hdzero_update(void) {
    char buf[128], i;

    if (g_osd_update_cnt < FC_OSD_CHECK_PERIOD)
        g_osd_update_cnt++;
    else if (g_osd_update_cnt == FC_OSD_CHECK_PERIOD) {
        osd_clear();
        g_osd_update_cnt++;
    }

    if (fhd_change())
        return;

    // if the user is in the osd element position settings, show all elements
    if (g_app_state == APP_STATE_OSD_ELEMENT_PREV) {
        // show actual value so text length is correct, to make it easier to position
        osd_battery_voltage_show(true);

        // show actual date/time/format, to make it easier to position
        osd_clock_show(true);

        // some elements might not be visible, set dummy sources to show them
        osd_elements_set_dummy_sources();
        osd_show_all_elements();
        return;
    }

    bool showRXOSD = g_setting.osd.is_visible && (g_source_info.source == SOURCE_HDZERO);

    osd_rec_show(g_setting.osd.is_visible);
    osd_llock_show(g_setting.osd.is_visible);
    osd_topfan_show(g_setting.osd.is_visible);
    osd_battery_voltage_show(g_setting.osd.is_visible);
    osd_clock_show(g_setting.osd.is_visible);

    if (gif_cnt % 10 == 0) { // delay needed to allow gif to flash
        osd_resource_path(buf, "%s", is_fhd, VrxTemp7_gif);
        lv_gif_set_src(g_osd_hdzero.vrx_temp[is_fhd], buf);
        osd_vrxtemp_show();
    }

    if (showRXOSD && g_osd_hdzero.vtx_temp[is_fhd]) {
        if (vtxTempInfo & 0x80) {
            i = vtxTempInfo & 0xF;
            if (i == 0)
                i = 1;
            else if (i > 8)
                i = 8;
            osd_resource_path(buf, "VtxTemp%d.bmp", is_fhd, i);
        } else {
            osd_resource_path(buf, "%s", is_fhd, blank_bmp);
        }
        lv_img_set_src(g_osd_hdzero.vtx_temp[is_fhd], buf);
    }

    if (showRXOSD && g_setting.osd.element[OSD_GOGGLE_VTX_TEMP].show)
        lv_obj_clear_flag(g_osd_hdzero.vtx_temp[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.vtx_temp[is_fhd], LV_OBJ_FLAG_HIDDEN);

    osd_channel_show(showRXOSD);
    osd_vlq_show(showRXOSD);

    if (gif_cnt % 10 == 0) { // delay needed to allow gif to flash
        osd_resource_path(buf, "%s", is_fhd, lowBattery_gif);
        lv_gif_set_src(g_osd_hdzero.battery_low[is_fhd], buf);
        osd_battery_low_show();
    }

    osd_resource_path(buf, "ant%d.bmp", is_fhd, RSSI2Ant(rx_status[0].rx_rssi[1]));
    lv_img_set_src(g_osd_hdzero.ant0[is_fhd], buf);

    osd_resource_path(buf, "ant%d.bmp", is_fhd, RSSI2Ant(rx_status[0].rx_rssi[0]));
    lv_img_set_src(g_osd_hdzero.ant1[is_fhd], buf);

    osd_resource_path(buf, "ant%d.bmp", is_fhd, RSSI2Ant(rx_status[1].rx_rssi[1]));
    lv_img_set_src(g_osd_hdzero.ant2[is_fhd], buf);

    osd_resource_path(buf, "ant%d.bmp", is_fhd, RSSI2Ant(rx_status[1].rx_rssi[0]));
    lv_img_set_src(g_osd_hdzero.ant3[is_fhd], buf);

    if (showRXOSD && g_setting.osd.element[OSD_GOGGLE_ANT0].show)
        lv_obj_clear_flag(g_osd_hdzero.ant0[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant0[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (showRXOSD && g_setting.osd.element[OSD_GOGGLE_ANT1].show)
        lv_obj_clear_flag(g_osd_hdzero.ant1[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant1[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (showRXOSD && g_setting.osd.element[OSD_GOGGLE_ANT2].show)
        lv_obj_clear_flag(g_osd_hdzero.ant2[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant2[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (showRXOSD && g_setting.osd.element[OSD_GOGGLE_ANT3].show)
        lv_obj_clear_flag(g_osd_hdzero.ant3[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant3[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.storage.selftest) {
        sprintf(buf, "T:%d-%d", fan_speed.top, g_temperature.top / 10);
        lv_label_set_text(g_osd_hdzero.osd_tempe[is_fhd][0], buf);

        sprintf(buf, "L:%d-%d", fan_speed.left, g_temperature.left / 10);
        lv_label_set_text(g_osd_hdzero.osd_tempe[is_fhd][1], buf);

        sprintf(buf, "R:%d-%d", fan_speed.right, g_temperature.right / 10);
        lv_label_set_text(g_osd_hdzero.osd_tempe[is_fhd][2], buf);
    }
}

int osd_clear(void) {
    clear_screen();
    elrs_clear_osd();
    osd_signal_update();
    return 0;
}

static int draw_osd_on_screen(uint8_t row, uint8_t col) {
    pthread_mutex_lock(&lvgl_mutex);
    int index = osd_buf_shadow[row][col];
    if (is_fhd)
        lv_img_set_src(img_arr[is_fhd][row][col], &osd_font_fhd.data[index]);
    else
        lv_img_set_src(img_arr[is_fhd][row][col], &osd_font_hd.data[index]);
    pthread_mutex_unlock(&lvgl_mutex);

    return 0;
}

static void embedded_osd_init(uint8_t fhd) {
    char buf[128];
    lv_obj_t *so;

    fhd &= 1;
    so = scr_osd[fhd];

    osd_resource_path(buf, "%s", is_fhd, fan1_bmp);
    osd_object_create_img(fhd, &g_osd_hdzero.topfan_speed[fhd], buf, &g_setting.osd.element[OSD_GOGGLE_TOPFAN_SPEED].position, so);

    osd_resource_path(buf, "%s", is_fhd, VtxTemp1_bmp);
    osd_object_create_img(fhd, &g_osd_hdzero.vtx_temp[fhd], buf, &g_setting.osd.element[OSD_GOGGLE_VTX_TEMP].position, so);

    osd_resource_path(buf, "%s", is_fhd, lowBattery_gif);
    osd_object_create_gif(fhd, &g_osd_hdzero.battery_low[fhd], buf, &g_setting.osd.element[OSD_GOGGLE_BATTERY_LOW].position, so);

    osd_object_create_label(fhd, &g_osd_hdzero.battery_voltage[fhd], "1S 0.0V", &g_setting.osd.element[OSD_GOGGLE_BATTERY_VOLTAGE].position, so);
    lv_obj_set_style_bg_color(g_osd_hdzero.battery_voltage[fhd], lv_color_hex(0x010101), LV_PART_MAIN);

    osd_resource_path(buf, "%s", is_fhd, VrxTemp7_gif);
    osd_object_create_gif(fhd, &g_osd_hdzero.vrx_temp[fhd], buf, &g_setting.osd.element[OSD_GOGGLE_VRX_TEMP].position, so);

    osd_resource_path(buf, "%s", is_fhd, LLOCK_bmp);
    osd_object_create_img(fhd, &g_osd_hdzero.latency_lock[fhd], buf, &g_setting.osd.element[OSD_GOGGLE_LATENCY_LOCK].position, so);

    osd_object_create_label(fhd, &g_osd_hdzero.clock[fhd][OSD_CLOCK_DATE], clock_date, &g_setting.osd.element[OSD_GOGGLE_CLOCK_DATE].position, so);
    lv_obj_set_style_bg_color(g_osd_hdzero.clock[fhd][OSD_CLOCK_DATE], lv_color_hex(0x010101), LV_PART_MAIN);

    osd_object_create_label(fhd, &g_osd_hdzero.clock[fhd][OSD_CLOCK_TIME], clock_time, &g_setting.osd.element[OSD_GOGGLE_CLOCK_TIME].position, so);
    lv_obj_set_style_bg_color(g_osd_hdzero.clock[fhd][OSD_CLOCK_TIME], lv_color_hex(0x010101), LV_PART_MAIN);

    // Bind Clock Format Offset to Time
    setting_osd_goggle_element_positions_t position = g_setting.osd.element[OSD_GOGGLE_CLOCK_TIME].position;
    position.mode_4_3.x += clock_format_offsets[is_fhd];
    position.mode_16_9.x += clock_format_offsets[is_fhd];
    osd_object_create_label(fhd, &g_osd_hdzero.clock[fhd][OSD_CLOCK_FORMAT], clock_format, &position, so);
    lv_obj_set_style_bg_color(g_osd_hdzero.clock[fhd][OSD_CLOCK_FORMAT], lv_color_hex(0x010101), LV_PART_MAIN);

    osd_object_create_label(fhd, &g_osd_hdzero.channel[fhd], "CH:-- ", &g_setting.osd.element[OSD_GOGGLE_CHANNEL].position, so);
    lv_obj_set_style_bg_color(g_osd_hdzero.channel[fhd], lv_color_hex(0x010101), LV_PART_MAIN);
    lv_obj_set_style_radius(g_osd_hdzero.channel[fhd], 50, 0);
    channel_osd_mode = 0;

    osd_resource_path(buf, "%s", is_fhd, noSdcard_bmp);
    osd_object_create_img(fhd, &g_osd_hdzero.sd_rec[fhd], buf, &g_setting.osd.element[OSD_GOGGLE_SD_REC].position, so);

    osd_resource_path(buf, "%s", is_fhd, VLQ1_bmp);
    osd_object_create_img(fhd, &g_osd_hdzero.vlq[fhd], buf, &g_setting.osd.element[OSD_GOGGLE_VLQ].position, so);

    osd_resource_path(buf, "%s", is_fhd, ant1_bmp);
    osd_object_create_img(fhd, &g_osd_hdzero.ant0[fhd], buf, &g_setting.osd.element[OSD_GOGGLE_ANT0].position, so);
    osd_object_create_img(fhd, &g_osd_hdzero.ant1[fhd], buf, &g_setting.osd.element[OSD_GOGGLE_ANT1].position, so);
    osd_object_create_img(fhd, &g_osd_hdzero.ant2[fhd], buf, &g_setting.osd.element[OSD_GOGGLE_ANT2].position, so);
    osd_object_create_img(fhd, &g_osd_hdzero.ant3[fhd], buf, &g_setting.osd.element[OSD_GOGGLE_ANT3].position, so);

    if (g_setting.storage.selftest) {
        osd_object_create_label(fhd, &g_osd_hdzero.osd_tempe[fhd][0], "TOP:-.- oC", &g_setting.osd.element[OSD_GOGGLE_TEMP_TOP].position, so);
        osd_object_create_label(fhd, &g_osd_hdzero.osd_tempe[fhd][1], "LEFT:-.- oC", &g_setting.osd.element[OSD_GOGGLE_TEMP_LEFT].position, so);
        osd_object_create_label(fhd, &g_osd_hdzero.osd_tempe[fhd][2], "RIGHT:-.- oC", &g_setting.osd.element[OSD_GOGGLE_TEMP_RIGHT].position, so);
    }
}

void osd_update_element_positions() {
    osd_object_set_pos(is_fhd, g_osd_hdzero.topfan_speed[is_fhd], &g_setting.osd.element[OSD_GOGGLE_TOPFAN_SPEED].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.vtx_temp[is_fhd], &g_setting.osd.element[OSD_GOGGLE_VTX_TEMP].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.battery_low[is_fhd], &g_setting.osd.element[OSD_GOGGLE_BATTERY_LOW].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.battery_voltage[is_fhd], &g_setting.osd.element[OSD_GOGGLE_BATTERY_VOLTAGE].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.vrx_temp[is_fhd], &g_setting.osd.element[OSD_GOGGLE_VRX_TEMP].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.latency_lock[is_fhd], &g_setting.osd.element[OSD_GOGGLE_LATENCY_LOCK].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.sd_rec[is_fhd], &g_setting.osd.element[OSD_GOGGLE_SD_REC].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.vlq[is_fhd], &g_setting.osd.element[OSD_GOGGLE_VLQ].position);

    osd_object_set_pos(is_fhd, g_osd_hdzero.clock[is_fhd][OSD_CLOCK_DATE], &g_setting.osd.element[OSD_GOGGLE_CLOCK_DATE].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.clock[is_fhd][OSD_CLOCK_TIME], &g_setting.osd.element[OSD_GOGGLE_CLOCK_TIME].position);
    setting_osd_goggle_element_positions_t position = g_setting.osd.element[OSD_GOGGLE_CLOCK_TIME].position;
    position.mode_4_3.x += clock_format_offsets[is_fhd];
    position.mode_16_9.x += clock_format_offsets[is_fhd];
    osd_object_set_pos(is_fhd, g_osd_hdzero.clock[is_fhd][OSD_CLOCK_FORMAT], &position);

    osd_object_set_pos(is_fhd, g_osd_hdzero.channel[is_fhd], &g_setting.osd.element[OSD_GOGGLE_CHANNEL].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.ant0[is_fhd], &g_setting.osd.element[OSD_GOGGLE_ANT0].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.ant1[is_fhd], &g_setting.osd.element[OSD_GOGGLE_ANT1].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.ant2[is_fhd], &g_setting.osd.element[OSD_GOGGLE_ANT2].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.ant3[is_fhd], &g_setting.osd.element[OSD_GOGGLE_ANT3].position);
    if (g_setting.storage.selftest) {
        osd_object_set_pos(is_fhd, g_osd_hdzero.osd_tempe[is_fhd][0], &g_setting.osd.element[OSD_GOGGLE_TEMP_TOP].position);
        osd_object_set_pos(is_fhd, g_osd_hdzero.osd_tempe[is_fhd][1], &g_setting.osd.element[OSD_GOGGLE_TEMP_LEFT].position);
        osd_object_set_pos(is_fhd, g_osd_hdzero.osd_tempe[is_fhd][2], &g_setting.osd.element[OSD_GOGGLE_TEMP_RIGHT].position);
    }
}

static void fc_osd_init(uint8_t fhd, uint16_t OFFSET_X, uint16_t OFFSET_Y) {
    uint8_t osd_width = fhd ? OSD_WIDTH_FHD : OSD_WIDTH_HD;
    uint8_t osd_height = fhd ? OSD_HEIGHT_FHD : OSD_HEIGHT_HD;

    load_fc_osd_font(fhd);

    for (int i = 0; i < HD_VMAX; i++) {
        for (int j = 0; j < HD_HMAX; j++) {
            pthread_mutex_lock(&lvgl_mutex);
            img_arr[fhd][i][j] = lv_img_create(scr_osd[fhd]);
            lv_obj_set_size(img_arr[fhd][i][j], osd_width, osd_height);
            lv_obj_set_pos(img_arr[fhd][i][j], j * osd_width + OFFSET_X, i * osd_height + OFFSET_Y);
            pthread_mutex_unlock(&lvgl_mutex);
        }
    }
}

static void create_osd_scr(void) {
    scr_main = lv_scr_act();
    for (uint8_t i = 0; i < 2; i++) {
        scr_osd[i] = lv_obj_create(scr_main);
        lv_obj_clear_flag(scr_main, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_clear_flag(scr_osd[i], LV_OBJ_FLAG_SCROLLABLE);
        if (i)
            lv_obj_set_size(scr_osd[i], DRAW_HOR_RES_FHD, DRAW_VER_RES_FHD);
        else
            lv_obj_set_size(scr_osd[i], DRAW_HOR_RES_HD, DRAW_VER_RES_HD);
        lv_obj_add_flag(scr_osd[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_style(scr_osd[i], &style_osd, 0);
    }
}

int osd_init(void) {
    const uint16_t OFFSET_X = 20;
    const uint16_t OFFSET_Y = 40;

    is_fhd = 0;

    // Update clock
    rtc_get_clock_osd_str(clock_date, sizeof(clock_date),
                          clock_time, sizeof(clock_time),
                          clock_format, sizeof(clock_format));

    create_osd_scr();

    fc_osd_init(0, OFFSET_X, OFFSET_Y);
    embedded_osd_init(0);

    fc_osd_init(1, OFFSET_X + (OFFSET_X >> 1), OFFSET_Y + (OFFSET_Y >> 1));
    embedded_osd_init(1);

    sem_init(&osd_semaphore, 0, 1);

    return 0;
}

void osd_fhd(uint8_t fhd) {
    is_fhd = fhd;
}

///////////////////////////////////////////////////////////////////////////////
// load_osd_file
// load_fc_osd_font
int load_fc_osd_font_bmp(const char *file, uint8_t fhd) {
    char *buf;
    struct stat stFile;
    int fd, size, rd;
    int boundry_width;
    int osd_width = fhd ? OSD_WIDTH_FHD : OSD_WIDTH_HD;
    int osd_height = fhd ? OSD_HEIGHT_FHD : OSD_HEIGHT_HD;
    int line_size;

    LOGI("load_fc_osd_font_bmp: %s...", file);
    fd = open(file, O_RDONLY);
    if (fd < 0)
        return -1;

    fstat(fd, &stFile);
    size = stFile.st_size;
    buf = (char *)malloc(size);
    if (!buf)
        return -2;

    rd = read(fd, buf, size);
    if (rd != size)
        return -3;

    close(fd);

    bmpFileHead *bmp = (bmpFileHead *)buf;
    char *pb = buf + sizeof(bmpFileHead) + bmp->info.biClrUsed;

    if (bmp->info.biWidth == (OSD_HNUM * osd_width)) {
        // no boundry
        boundry_width = OSD_BOUNDRY_0;
    } else {
        // have boundry
        boundry_width = OSD_BOUNDRY_1;
    }

    line_size = (((osd_width + boundry_width) * OSD_HNUM + boundry_width) * 3 + 3) & 0xFFFC; // 4bytes align

    // read OSD font
    uint8_t h, v;
    uint8_t x, y;
    uint32_t addr;
    uint8_t offset;

    for (v = 0; v < OSD_VNUM; v++) {
        for (h = 0; h < OSD_HNUM; h++) {
            // calc vertical and horizontal black boundry.
            addr = (v + 1) * boundry_width * line_size + (h + 1) * boundry_width * 3;
            // calc size that have read.
            addr += v * osd_height * line_size;
            addr += h * osd_width * 3;

            for (y = 0; y < osd_height; y++) {
                for (x = 0; x < osd_width; x++) {
                    if (fhd)
                        osdFont_fhd[OSD_VNUM - v - 1][h][osd_height - y - 1][x] = (0xff << 24) + ((pb[addr + x * 3] & 0xff)) + ((pb[addr + x * 3 + 1] & 0xff) << 8) + ((pb[addr + x * 3 + 2] & 0xff) << 16);
                    else
                        osdFont_hd[OSD_VNUM - v - 1][h][osd_height - y - 1][x] = (0xff << 24) + ((pb[addr + x * 3] & 0xff)) + ((pb[addr + x * 3 + 1] & 0xff) << 8) + ((pb[addr + x * 3 + 2] & 0xff) << 16);
                }
                addr += line_size;
            }
        }
    }

    if (fhd) {
        for (v = 0; v < OSD_VNUM; v++) {
            for (h = 0; h < OSD_HNUM; h++) {
                int index = v * OSD_HNUM + h;
                osd_font_fhd.data[index].header.cf = LV_IMG_CF_TRUE_COLOR;
                osd_font_fhd.data[index].header.always_zero = 0;
                osd_font_fhd.data[index].header.reserved = 0;
                osd_font_fhd.data[index].header.w = osd_width;
                osd_font_fhd.data[index].header.h = osd_height;
                osd_font_fhd.data[index].data_size = osd_width * osd_height * LV_COLOR_SIZE / 8;
                osd_font_fhd.data[index].data = (uint8_t *)&osdFont_fhd[v][h][0][0];
            }
        }
    } else {
        for (v = 0; v < OSD_VNUM; v++) {
            for (h = 0; h < OSD_HNUM; h++) {
                int index = v * OSD_HNUM + h;
                osd_font_hd.data[index].header.cf = LV_IMG_CF_TRUE_COLOR;
                osd_font_hd.data[index].header.always_zero = 0;
                osd_font_hd.data[index].header.reserved = 0;
                osd_font_hd.data[index].header.w = osd_width;
                osd_font_hd.data[index].header.h = osd_height;
                osd_font_hd.data[index].data_size = osd_width * osd_height * LV_COLOR_SIZE / 8;
                osd_font_hd.data[index].data = (uint8_t *)&osdFont_hd[v][h][0][0];
            }
        }
    }
    // free(buf); //FIX ME, ntant, it seems system becomes unstable if uncomment this ???
    return 0;
}

void load_fc_osd_font(uint8_t fhd) {
    char fp[3][256];
    int i;

    if (fhd) {
        sprintf(fp[0], "%s%s_FHD_000.bmp", FC_OSD_SDCARD_PATH, fc_variant);
        sprintf(fp[1], "%s%s_FHD_000.bmp", FC_OSD_LOCAL_PATH, fc_variant);
        sprintf(fp[2], "%sBTFL_FHD_000.bmp", FC_OSD_LOCAL_PATH);
    } else {
        sprintf(fp[0], "%s%s_000.bmp", FC_OSD_SDCARD_PATH, fc_variant);
        sprintf(fp[1], "%s%s_000.bmp", FC_OSD_LOCAL_PATH, fc_variant);
        sprintf(fp[2], "%sBTFL_000.bmp", FC_OSD_LOCAL_PATH);
    }

    // Optimized for runtime execution
    if (0 == strncmp(fc_variant, "ARDU", sizeof(fc_variant))) {
        g_fc_variant_type = FC_VARIANT_ARDU;
    } else if (0 == strncmp(fc_variant, "BTFL", sizeof(fc_variant))) {
        g_fc_variant_type = FC_VARIANT_BTFL;
    } else if (0 == strncmp(fc_variant, "EMUF", sizeof(fc_variant))) {
        g_fc_variant_type = FC_VARIANT_EMUF;
    } else if (0 == strncmp(fc_variant, "INAV", sizeof(fc_variant))) {
        g_fc_variant_type = FC_VARIANT_INAV;
    } else if (0 == strncmp(fc_variant, "QUIC", sizeof(fc_variant))) {
        g_fc_variant_type = FC_VARIANT_QUIC;
    } else {
        g_fc_variant_type = FC_VARIANT_UNKNOWN;
    }

    // Bind Clock format to time OSD offsets
    switch (g_fc_variant_type) {
    case FC_VARIANT_ARDU:
    case FC_VARIANT_BTFL:
    case FC_VARIANT_EMUF:
    case FC_VARIANT_INAV:
        clock_format_offsets[OSD_RESOURCE_720] = 150;
        clock_format_offsets[OSD_RESOURCE_1080] = 100;
        break;
    case FC_VARIANT_QUIC:
        clock_format_offsets[OSD_RESOURCE_720] = 140;
        clock_format_offsets[OSD_RESOURCE_1080] = 90;
        break;
    default:
        clock_format_offsets[OSD_RESOURCE_720] = 124;
        clock_format_offsets[OSD_RESOURCE_1080] = 74;
        break;
    }

    for (i = 0; i < 3; i++) {
        if (!load_fc_osd_font_bmp(fp[i], fhd)) {
            LOGI(" succecss!");
            return;
        } else
            LOGE(" failed!");
    }
}

void osd_shadow_clear(void) {
    for (int i = 0; i < HD_VMAX; i++) {
        for (int j = 0; j < HD_HMAX; j++) {
            if (osd_buf_shadow[i][j] != 0x20) {
                osd_buf_shadow[i][j] = 0x20;
                draw_osd_on_screen(i, j);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Threads for updating FC OSD

void osd_signal_update() {
    sem_post(&osd_semaphore);
}

void *thread_osd(void *ptr) {
    static uint8_t fhd_d = 0;
    for (;;) {
        // wait for signal to render
        sem_wait(&osd_semaphore);

        // clear shadow buffer when mode changes
        if (fhd_d != is_fhd) {
            osd_shadow_clear();
            fhd_d = is_fhd;
        }

        // display osd
        for (int i = 0; i < HD_VMAX; i++) {
            for (int j = 0; j < HD_HMAX; j++) {
                uint16_t ch = fc_osd[i][j];
                if (ch == 0x20)
                    ch = elrs_osd[i][j];
                if (ch != osd_buf_shadow[i][j]) {
                    osd_buf_shadow[i][j] = ch;
                    draw_osd_on_screen(i, j);
                }
            }
        }
    }
    return NULL;
}
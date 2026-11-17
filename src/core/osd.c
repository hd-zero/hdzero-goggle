#include "osd.h"

#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <log/log.h>
#include <lvgl/lvgl.h>
#include <minIni.h>

#include "core/app_state.h"
#include "core/battery.h"
#include "core/common.hh"
#include "core/dvr.h"
#include "core/elrs.h"
#include "core/ht.h"
#include "core/msp_displayport.h"
#include "core/settings.h"
#include "driver/dm5680.h"
#include "driver/fans.h"
#include "driver/fbtools.h"
#include "driver/hardware.h"
#include "driver/i2c.h"
#include "driver/nct75.h"
#include "driver/rtc.h"
#include "driver/rtc6715.h"
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
static bool reset_armed_state = false; // Flag to reset armed state detection

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
    vsnprintf(filename, sizeof(filename), fmt, args);
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

#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)

void osd_show_hdmi_in_dvr(uint8_t is_show) {
    uint8_t reg;

    reg = I2C_Read(0x64, 0x8d);
    if (dvr_is_recording)
        reg |= 0x01;
    else
        reg &= 0xfe;
    I2C_Write(ADDR_FPGA, 0x8d, reg);
}

void osd_hdmi_in_dvr_update() {
    uint8_t reg;
    static uint8_t last_dvr_is_recording = 0;

    if (g_source_info.source != SOURCE_HDMI_IN) {
        if (last_dvr_is_recording != 0) {
            osd_show_hdmi_in_dvr(0);
            last_dvr_is_recording = 0;
        }
        return;
    }

    if (last_dvr_is_recording == dvr_is_recording)
        return;

    osd_show_hdmi_in_dvr(dvr_is_recording);

    last_dvr_is_recording = dvr_is_recording;
}
#endif
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
static lv_obj_t *analog_rssi_bar;

// Canvas buffers for head tracker OSD elements
// Compass spans full width at top, altitude spans full height on right
static lv_color_t cbuf_compass_hd[1280 * 60];   // Full width x 60px height
static lv_color_t cbuf_compass_fhd[1920 * 90];  // Full width x 90px height
static lv_color_t cbuf_altitude_hd[60 * 720];   // 60px width x full height
static lv_color_t cbuf_altitude_fhd[90 * 1080]; // 90px width x full height

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

    if (!g_sdcard_enable || !g_sdcard_ready) {
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
#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
    osd_hdmi_in_dvr_update();
#endif
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

void osd_analog_rssi_update_location() {
    if (g_setting.osd.embedded_mode == EMBEDDED_4x3)
        lv_obj_set_pos(analog_rssi_bar, g_setting.osd.element[OSD_GOGGLE_ANT0].position.mode_4_3.x, g_setting.osd.element[OSD_GOGGLE_ANT0].position.mode_4_3.y + 14);
    else
        lv_obj_set_pos(analog_rssi_bar, g_setting.osd.element[OSD_GOGGLE_ANT0].position.mode_16_9.x, g_setting.osd.element[OSD_GOGGLE_ANT0].position.mode_16_9.y + 14);
}

void osd_analog_rssi_create() {

    pthread_mutex_lock(&lvgl_mutex);
    analog_rssi_bar = lv_bar_create(scr_osd[0]);
    lv_obj_set_size(analog_rssi_bar, 128, 16);

    osd_analog_rssi_update_location();

    lv_bar_set_value(analog_rssi_bar, 75, LV_ANIM_OFF);
    lv_obj_set_style_border_width(analog_rssi_bar, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(analog_rssi_bar, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(analog_rssi_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(analog_rssi_bar, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(analog_rssi_bar, lv_color_hex(0x404040), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(analog_rssi_bar, lv_color_hex(0x00FF00), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(analog_rssi_bar, lv_color_hex(0x202020), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(analog_rssi_bar, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_flag(analog_rssi_bar, LV_OBJ_FLAG_HIDDEN);

    pthread_mutex_unlock(&lvgl_mutex);
}

void osd_analog_rssi_show(bool bShow) {
    char buf[128];
    // static uint8_t cnt = 0;

    if (!bShow) {
        lv_obj_add_flag(analog_rssi_bar, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_obj_clear_flag(analog_rssi_bar, LV_OBJ_FLAG_HIDDEN);

    int rssi_volt_mv;

    if (g_setting.analog_rssi.calib_min == g_setting.analog_rssi.calib_max)
        rssi_volt_mv = 0;
    else {
        rssi_volt_mv = rtc6715.rssi;
        if (rssi_volt_mv <= g_setting.analog_rssi.calib_min)
            rssi_volt_mv = 0;
        else if (rssi_volt_mv >= g_setting.analog_rssi.calib_max)
            rssi_volt_mv = 100;
        else
            rssi_volt_mv = (rssi_volt_mv - g_setting.analog_rssi.calib_min) * 100 / (g_setting.analog_rssi.calib_max - g_setting.analog_rssi.calib_min);
    }

    lv_bar_set_value(analog_rssi_bar, rssi_volt_mv, LV_ANIM_OFF);
}

///////////////////////////////////:////////////////////////////////////////////
// OSD channel
// channel_osd_mode
//  = 0x80 | Channel
//  = 0x00 | Channel Show Time
uint8_t channel_osd_mode;

char *channel2str(uint8_t is_hdzero, uint8_t is_lowband, uint8_t channel) // channel=[1:18]
{
    static char *hdzero_channel_name[2][BASE_CH_NUM] = {
        {"R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "E1", "F1", "F2", "F4"},
        {"L1", "L2", "L3", "L4", "L5", "L6", "L7", "L8", "  ", "  ", "  ", "  "},
    };

    static char *analog_channel_name[48] = {
        "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8",
        "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8",
        "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8",
        "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8",
        "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8",
        "L1", "L2", "L3", "L4", "L5", "L6", "L7", "L8"};

    if (is_hdzero) {
        if ((channel > 0) && (channel <= HDZERO_CHANNEL_NUM))
            return hdzero_channel_name[is_lowband][channel - 1];
        else
            return hdzero_channel_name[is_lowband][0];
    } else {
        return analog_channel_name[channel - 1];
    }
}

// Draw horizontal compass for head tracking (azimuth)
void osd_head_tracker_compass_draw(int16_t heading_deg) {
    if (!g_setting.osd.element[OSD_GOGGLE_HEAD_TRACKER_COMPASS].show) {
        lv_obj_add_flag(g_osd_hdzero.head_tracker_compass[is_fhd], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_obj_t *canvas = g_osd_hdzero.head_tracker_compass[is_fhd];
    int width = is_fhd ? 1920 : 1280; // Full screen width
    int height = is_fhd ? 60 : 42;    // Taller to prevent text cutoff

    // Clear canvas with transparent background
    lv_canvas_fill_bg(canvas, lv_color_hex(0x000000), LV_OPA_0);

    // Draw compass scale
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_white();
    line_dsc.width = is_fhd ? 2 : 1;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_white();
    label_dsc.opa = LV_OPA_COVER;
    label_dsc.font = &lv_font_montserrat_16;

    int center_x = width / 2;
    int pixels_per_deg = is_fhd ? 4 : 3; // More pixels per degree for better spacing
    int tick_height = is_fhd ? 20 : 14;  // Major tick height
    int label_y = is_fhd ? 24 : 17;      // Label position below ticks

    // Draw tick marks and labels
    for (int deg = -180; deg <= 180; deg += 10) {
        int actual_deg = (heading_deg + deg + 360) % 360;
        int x = center_x + (deg * pixels_per_deg);

        if (x >= 0 && x < width) {
            lv_point_t points[2];
            points[0].x = x;
            points[0].y = 0;
            points[1].x = x;

            if (deg % 30 == 0) {
                points[1].y = tick_height;
                lv_canvas_draw_line(canvas, points, 2, &line_dsc);

                // Draw degree label
                char label[8];
                snprintf(label, sizeof(label), "%d", actual_deg);
                lv_point_t label_pos = {x - (is_fhd ? 18 : 12), label_y};
                lv_canvas_draw_text(canvas, label_pos.x, label_pos.y, is_fhd ? 50 : 35, &label_dsc, label);
            } else {
                points[1].y = tick_height / 2;
                lv_canvas_draw_line(canvas, points, 2, &line_dsc);
            }
        }
    }

    // Draw center indicator (triangle pointing down) - this shows where user is pointing
    int tri_size = is_fhd ? 8 : 6;
    lv_point_t tri_points[4];
    tri_points[0].x = center_x;
    tri_points[0].y = 0;
    tri_points[1].x = center_x - tri_size;
    tri_points[1].y = tri_size * 2;
    tri_points[2].x = center_x + tri_size;
    tri_points[2].y = tri_size * 2;
    tri_points[3].x = center_x;
    tri_points[3].y = 0;

    line_dsc.color = lv_color_make(255, 0, 0);
    line_dsc.width = is_fhd ? 3 : 2;
    lv_canvas_draw_line(canvas, &tri_points[0], 2, &line_dsc);
    lv_canvas_draw_line(canvas, &tri_points[1], 2, &line_dsc);
    lv_canvas_draw_line(canvas, &tri_points[2], 2, &line_dsc);

    // Draw drone direction indicator if GPS data is valid and tracker is calibrated
    if (ht_antenna_tracker_is_calibrated()) {
        float drone_azimuth = ht_get_drone_azimuth();
        // Calculate angular difference between drone and current heading
        float angle_diff = drone_azimuth - heading_deg;

        // Normalize to -180 to 180
        while (angle_diff > 180)
            angle_diff -= 360;
        while (angle_diff < -180)
            angle_diff += 360;

        // Calculate x position for drone indicator
        int drone_x = center_x + (int)(angle_diff * pixels_per_deg);

        static int debug_counter = 0;
        if (++debug_counter % 30 == 0) { // Log every 30 frames (~1 second)
            LOGD("Drone indicator: azimuth=%.1f°, heading=%.1f°, diff=%.1f°, x=%d (visible: %d-%d)",
                 drone_azimuth, (float)heading_deg, angle_diff, drone_x, tri_size, width - tri_size);
        }

        // Only draw if within visible range
        if (drone_x >= tri_size && drone_x < width - tri_size) {
            // Draw drone indicator as a triangle pointing down (different from center)
            lv_point_t drone_tri[4];
            int drone_tri_size = is_fhd ? 10 : 7; // Slightly larger
            drone_tri[0].x = drone_x;
            drone_tri[0].y = 0;
            drone_tri[1].x = drone_x - drone_tri_size;
            drone_tri[1].y = drone_tri_size * 2;
            drone_tri[2].x = drone_x + drone_tri_size;
            drone_tri[2].y = drone_tri_size * 2;
            drone_tri[3].x = drone_x;
            drone_tri[3].y = 0;

            // Draw filled triangle in green to distinguish from red centering pin
            line_dsc.color = lv_color_make(0, 255, 0);
            line_dsc.width = is_fhd ? 4 : 3;
            lv_canvas_draw_line(canvas, &drone_tri[0], 2, &line_dsc);
            lv_canvas_draw_line(canvas, &drone_tri[1], 2, &line_dsc);
            lv_canvas_draw_line(canvas, &drone_tri[2], 2, &line_dsc);
        }
    }

    lv_obj_clear_flag(canvas, LV_OBJ_FLAG_HIDDEN);
}

// Draw vertical altitude/pitch indicator for head tracking
void osd_head_tracker_altitude_draw(int16_t pitch_deg) {
    if (!g_setting.osd.element[OSD_GOGGLE_HEAD_TRACKER_ALTITUDE].show) {
        lv_obj_add_flag(g_osd_hdzero.head_tracker_altitude[is_fhd], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_obj_t *canvas = g_osd_hdzero.head_tracker_altitude[is_fhd];
    int width = is_fhd ? 80 : 55;     // Wider to prevent text cutoff
    int height = is_fhd ? 1080 : 720; // Full screen height

    // Clear canvas with transparent background
    lv_canvas_fill_bg(canvas, lv_color_hex(0x000000), LV_OPA_0);

    // Draw altitude scale
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_white();
    line_dsc.width = is_fhd ? 2 : 1;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_white();
    label_dsc.opa = LV_OPA_COVER;
    label_dsc.font = &lv_font_montserrat_16;

    int center_y = height / 2;
    int pixels_per_deg = is_fhd ? 4 : 3; // More pixels per degree for better spacing
    int tick_width = is_fhd ? 28 : 20;   // Major tick width
    int label_x = is_fhd ? 32 : 23;      // Label position to the right of ticks

    // Draw tick marks and labels (pitch from -90 to +90 degrees visible)
    for (int deg = -90; deg <= 90; deg += 5) {
        int actual_deg = pitch_deg + deg;
        if (actual_deg < -90 || actual_deg > 90)
            continue;

        int y = center_y - (deg * pixels_per_deg);

        if (y >= 0 && y < height) {
            lv_point_t points[2];
            points[0].x = 0;
            points[0].y = y;
            points[1].y = y;

            if (deg % 15 == 0) {
                points[1].x = tick_width;
                lv_canvas_draw_line(canvas, points, 2, &line_dsc);

                // Draw degree label
                char label[8];
                snprintf(label, sizeof(label), "%d", actual_deg);
                lv_point_t label_pos = {label_x, y - (is_fhd ? 8 : 6)};
                lv_canvas_draw_text(canvas, label_pos.x, label_pos.y, is_fhd ? 40 : 28, &label_dsc, label);
            } else {
                points[1].x = tick_width / 2;
                lv_canvas_draw_line(canvas, points, 2, &line_dsc);
            }
        }
    }

    // Draw center indicator (horizontal line) - this shows where user is pointing
    lv_point_t center_line[2];
    center_line[0].x = 0;
    center_line[0].y = center_y;
    center_line[1].x = width - 1;
    center_line[1].y = center_y;
    line_dsc.color = lv_color_make(255, 0, 0);
    line_dsc.width = is_fhd ? 3 : 2;
    lv_canvas_draw_line(canvas, center_line, 2, &line_dsc);

    // Draw drone elevation indicator if GPS data is valid and tracker is calibrated
    if (ht_antenna_tracker_is_calibrated()) {
        float drone_elevation = ht_get_drone_elevation();
        // Calculate angular difference between drone and current pitch
        float angle_diff = drone_elevation - pitch_deg;

        // Calculate y position for drone indicator (inverted because y increases downward)
        int drone_y = center_y - (int)(angle_diff * pixels_per_deg);

        // Only draw if within visible range and within reasonable elevation bounds
        if (drone_y >= 10 && drone_y < height - 10) {
            // Draw drone indicator as a horizontal line in green
            lv_point_t drone_line[2];
            drone_line[0].x = 0;
            drone_line[0].y = drone_y;
            drone_line[1].x = width - 1;
            drone_line[1].y = drone_y;

            line_dsc.color = lv_color_make(0, 255, 0);
            line_dsc.width = is_fhd ? 4 : 3;
            lv_canvas_draw_line(canvas, drone_line, 2, &line_dsc);
        }
    }

    lv_obj_clear_flag(canvas, LV_OBJ_FLAG_HIDDEN);
}

void osd_channel_show(bool bShow) {
    uint8_t ch;
    lv_color_t color;
    char buf[32];

    if (channel_osd_mode & 0x80) {
        ch = channel_osd_mode & 0x7F;
        color = lv_color_make(0xFF, 0x20, 0x20);
        snprintf(buf, sizeof(buf), "  To %s?  ", channel2str(g_source_info.source == SOURCE_HDZERO, g_setting.source.hdzero_band, ch));
        lv_obj_set_style_bg_opa(g_osd_hdzero.channel[is_fhd], LV_OPA_100, 0);
    } else {
        if (g_source_info.source == SOURCE_HDZERO) {
            ch = g_setting.scan.channel & 0x7F;
        } else {
#if defined(HDZGOGGLE2) || defined(HDZBOXPRO)
            if (g_source_info.source == SOURCE_AV_MODULE) {
                ch = g_setting.source.analog_channel & 0x7F;
            } else {
                bShow = false;
            }
#elif defined(HDZGOGGLE)
            bShow = false;
#endif
        }

        if (bShow) {
            color = lv_color_make(0xFF, 0xFF, 0xFF);
            snprintf(buf, sizeof(buf), "CH:%s", channel2str(g_source_info.source == SOURCE_HDZERO, g_setting.source.hdzero_band, ch));
            lv_obj_set_style_bg_opa(g_osd_hdzero.channel[is_fhd], 0, 0);
        }
    }

    if (channel_osd_mode & 0x80 || (bShow && channel_osd_mode && g_setting.osd.element[OSD_GOGGLE_CHANNEL].show)) {
        lv_label_set_text(g_osd_hdzero.channel[is_fhd], buf);
        lv_obj_set_style_text_color(g_osd_hdzero.channel[is_fhd], color, 0);
        lv_obj_clear_flag(g_osd_hdzero.channel[is_fhd], LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(g_osd_hdzero.channel[is_fhd], LV_OBJ_FLAG_HIDDEN);
    }
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

#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
        if (fhd_req == 1) {
            lvgl_switch_to_1080p();
            osd_fhd(1);
            // LOGI("fhd_change to 1080p");
        } else {
            lvgl_switch_to_720p();
            osd_fhd(0);
            // LOGI("fhd_change to 720p");
        }
#elif defined(HDZBOXPRO)
        lvgl_switch_to_720p();
        osd_fhd(0);
        // LOGI("fhd_change to 720p");
#endif

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

#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
    if (g_setting.osd.element[OSD_GOGGLE_TEMP_LEFT].show)
        lv_obj_clear_flag(g_osd_hdzero.osd_tempe[is_fhd][1], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.osd_tempe[is_fhd][1], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.osd.element[OSD_GOGGLE_TEMP_RIGHT].show)
        lv_obj_clear_flag(g_osd_hdzero.osd_tempe[is_fhd][2], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.osd_tempe[is_fhd][2], LV_OBJ_FLAG_HIDDEN);
#elif defined(HDZBOXPRO)

#endif
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

    bool source_is_hdzero = (g_source_info.source == SOURCE_HDZERO);
    bool source_is_analog = (g_source_info.source == SOURCE_AV_MODULE);
    bool showRXOSD = false;

#if defined(HDZGOGGLE)
    if (source_is_hdzero) {
        showRXOSD = g_setting.osd.is_visible;
    }
#elif defined(HDZBOXPRO) || defined(HDZGOGGLE2)
    if (source_is_hdzero || source_is_analog) {
        showRXOSD = g_setting.osd.is_visible;
    }
#endif

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

    if (showRXOSD && source_is_hdzero && g_osd_hdzero.vtx_temp[is_fhd]) {
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

    if (showRXOSD && source_is_hdzero && g_setting.osd.element[OSD_GOGGLE_VTX_TEMP].show)
        lv_obj_clear_flag(g_osd_hdzero.vtx_temp[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.vtx_temp[is_fhd], LV_OBJ_FLAG_HIDDEN);

    osd_channel_show(showRXOSD);
    osd_vlq_show(showRXOSD && source_is_hdzero);

#if defined(HDZBOXPRO) || defined(HDZGOGGLE2)
    osd_analog_rssi_show(showRXOSD && source_is_analog);
#elif defined(HDZGOGGLE)

#endif

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

    if (showRXOSD && source_is_hdzero && g_setting.osd.element[OSD_GOGGLE_ANT0].show)
        lv_obj_clear_flag(g_osd_hdzero.ant0[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant0[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (showRXOSD && source_is_hdzero && g_setting.osd.element[OSD_GOGGLE_ANT1].show)
        lv_obj_clear_flag(g_osd_hdzero.ant1[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant1[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (showRXOSD && source_is_hdzero && g_setting.osd.element[OSD_GOGGLE_ANT2].show)
        lv_obj_clear_flag(g_osd_hdzero.ant2[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant2[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (showRXOSD && source_is_hdzero && g_setting.osd.element[OSD_GOGGLE_ANT3].show)
        lv_obj_clear_flag(g_osd_hdzero.ant3[is_fhd], LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant3[is_fhd], LV_OBJ_FLAG_HIDDEN);

    if (g_setting.storage.selftest) {
        snprintf(buf, sizeof(buf), "T:%d-%d", fan_speed.top, g_temperature.top / 10);
        lv_label_set_text(g_osd_hdzero.osd_tempe[is_fhd][0], buf);
#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
        snprintf(buf, sizeof(buf), "L:%d-%d", fan_speed.left, g_temperature.left / 10);
        lv_label_set_text(g_osd_hdzero.osd_tempe[is_fhd][1], buf);

        snprintf(buf, sizeof(buf), "R:%d-%d", fan_speed.right, g_temperature.right / 10);
        lv_label_set_text(g_osd_hdzero.osd_tempe[is_fhd][2], buf);
#elif defined(HDZBOXPRO)

#endif
    }

    // Update head tracker OSD elements with actual head tracker data
    // Apply inversion based on user settings
    int16_t heading_deg = (int16_t)ht_get_pan_angle();
    int16_t pitch_deg = (int16_t)ht_get_tilt_angle();

    if (g_setting.ht.pan_invert) {
        heading_deg = -heading_deg;
    }
    if (g_setting.ht.tilt_invert) {
        pitch_deg = -pitch_deg;
    }

    // Calibration instruction text is now updated in OSD thread (osd_embedded_thread)
    // for proper synchronization with arm_count changes

    // If calibrated, apply offset so compass shows 0° at calibrated heading
    if (ht_antenna_tracker_is_calibrated()) {
        float pan_offset = ht_get_pan_offset();
        heading_deg = (int16_t)(heading_deg - pan_offset);
        // Normalize to -180 to +180 range
        while (heading_deg > 180)
            heading_deg -= 360;
        while (heading_deg < -180)
            heading_deg += 360;
    }

    osd_head_tracker_compass_draw(heading_deg);
    osd_head_tracker_altitude_draw(pitch_deg);
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
#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
        osd_object_create_label(fhd, &g_osd_hdzero.osd_tempe[fhd][1], "LEFT:-.- oC", &g_setting.osd.element[OSD_GOGGLE_TEMP_LEFT].position, so);
        osd_object_create_label(fhd, &g_osd_hdzero.osd_tempe[fhd][2], "RIGHT:-.- oC", &g_setting.osd.element[OSD_GOGGLE_TEMP_RIGHT].position, so);
#elif defined(HDZBOXPRO)

#endif
    }

    // Initialize head tracker compass (horizontal at top)
    g_osd_hdzero.head_tracker_compass[fhd] = lv_canvas_create(so);
    if (fhd) {
        lv_canvas_set_buffer(g_osd_hdzero.head_tracker_compass[fhd], cbuf_compass_fhd, 1920, 60, LV_IMG_CF_TRUE_COLOR_ALPHA);
    } else {
        lv_canvas_set_buffer(g_osd_hdzero.head_tracker_compass[fhd], cbuf_compass_hd, 1280, 42, LV_IMG_CF_TRUE_COLOR_ALPHA);
    }
    lv_canvas_fill_bg(g_osd_hdzero.head_tracker_compass[fhd], lv_color_hex(0x000000), LV_OPA_TRANSP);
    lv_obj_set_style_bg_opa(g_osd_hdzero.head_tracker_compass[fhd], LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(g_osd_hdzero.head_tracker_compass[fhd], 0, 0);
    osd_object_set_pos(fhd, g_osd_hdzero.head_tracker_compass[fhd], &g_setting.osd.element[OSD_GOGGLE_HEAD_TRACKER_COMPASS].position);
    lv_obj_add_flag(g_osd_hdzero.head_tracker_compass[fhd], LV_OBJ_FLAG_HIDDEN);

    // Initialize head tracker altitude/pitch (vertical on right side)
    g_osd_hdzero.head_tracker_altitude[fhd] = lv_canvas_create(so);
    if (fhd) {
        lv_canvas_set_buffer(g_osd_hdzero.head_tracker_altitude[fhd], cbuf_altitude_fhd, 80, 1080, LV_IMG_CF_TRUE_COLOR_ALPHA);
    } else {
        lv_canvas_set_buffer(g_osd_hdzero.head_tracker_altitude[fhd], cbuf_altitude_hd, 55, 720, LV_IMG_CF_TRUE_COLOR_ALPHA);
    }
    lv_canvas_fill_bg(g_osd_hdzero.head_tracker_altitude[fhd], lv_color_hex(0x000000), LV_OPA_TRANSP);
    lv_obj_set_style_bg_opa(g_osd_hdzero.head_tracker_altitude[fhd], LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(g_osd_hdzero.head_tracker_altitude[fhd], 0, 0);
    osd_object_set_pos(fhd, g_osd_hdzero.head_tracker_altitude[fhd], &g_setting.osd.element[OSD_GOGGLE_HEAD_TRACKER_ALTITUDE].position);
    lv_obj_add_flag(g_osd_hdzero.head_tracker_altitude[fhd], LV_OBJ_FLAG_HIDDEN);

    // Create calibration instruction label (centered on screen)
    g_osd_hdzero.calibration_instruction[fhd] = lv_label_create(so);
    lv_label_set_text(g_osd_hdzero.calibration_instruction[fhd], "");
    lv_obj_set_style_text_color(g_osd_hdzero.calibration_instruction[fhd], lv_color_make(0, 255, 0), 0);
    lv_obj_set_style_bg_color(g_osd_hdzero.calibration_instruction[fhd], lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(g_osd_hdzero.calibration_instruction[fhd], LV_OPA_80, LV_PART_MAIN);
    lv_obj_set_style_pad_all(g_osd_hdzero.calibration_instruction[fhd], 10, 0);
    if (fhd) {
        lv_obj_set_style_text_font(g_osd_hdzero.calibration_instruction[fhd], &conthrax_26, 0);
        lv_obj_set_pos(g_osd_hdzero.calibration_instruction[fhd], 1920 / 2 - 200, 1080 / 2 - 20);
    } else {
        lv_obj_set_style_text_font(g_osd_hdzero.calibration_instruction[fhd], &conthrax_26, 0);
        lv_obj_set_pos(g_osd_hdzero.calibration_instruction[fhd], 1280 / 2 - 200, 720 / 2 - 20);
    }
    lv_obj_add_flag(g_osd_hdzero.calibration_instruction[fhd], LV_OBJ_FLAG_HIDDEN);
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

#if defined(HDZBOXPRO) || defined(HDZGOGGLE2)
    osd_analog_rssi_update_location();
#elif defined(HDZGOGGLE)

#endif

    if (g_setting.storage.selftest) {
        osd_object_set_pos(is_fhd, g_osd_hdzero.osd_tempe[is_fhd][0], &g_setting.osd.element[OSD_GOGGLE_TEMP_TOP].position);

#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
        osd_object_set_pos(is_fhd, g_osd_hdzero.osd_tempe[is_fhd][1], &g_setting.osd.element[OSD_GOGGLE_TEMP_LEFT].position);
        osd_object_set_pos(is_fhd, g_osd_hdzero.osd_tempe[is_fhd][2], &g_setting.osd.element[OSD_GOGGLE_TEMP_RIGHT].position);
#elif defined(HDZBOXPRO)

#endif
    }

    // Update head tracker element positions
    osd_object_set_pos(is_fhd, g_osd_hdzero.head_tracker_compass[is_fhd], &g_setting.osd.element[OSD_GOGGLE_HEAD_TRACKER_COMPASS].position);
    osd_object_set_pos(is_fhd, g_osd_hdzero.head_tracker_altitude[is_fhd], &g_setting.osd.element[OSD_GOGGLE_HEAD_TRACKER_ALTITUDE].position);
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

#if defined(HDZBOXPRO) || defined(HDZGOGGLE2)
    if (!fhd) {
        osd_analog_rssi_create();
    }
#endif
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

#if defined(HDZGOGGLE) || defined(HDZGOGGLE2)
    fc_osd_init(1, OFFSET_X + (OFFSET_X >> 1), OFFSET_Y + (OFFSET_Y >> 1));
#elif defined(HDZBOXPRO)
    fc_osd_init(1, OFFSET_X, OFFSET_Y);
#endif

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
        snprintf(fp[0], sizeof(fp[0]), "%s%s_FHD_000.bmp", FC_OSD_SDCARD_PATH, fc_variant);
        snprintf(fp[1], sizeof(fp[1]), "%s%s_FHD_000.bmp", FC_OSD_LOCAL_PATH, fc_variant);
        snprintf(fp[2], sizeof(fp[2]), "%sBTFL_FHD_000.bmp", FC_OSD_LOCAL_PATH);
    } else {
        snprintf(fp[0], sizeof(fp[0]), "%s%s_000.bmp", FC_OSD_SDCARD_PATH, fc_variant);
        snprintf(fp[1], sizeof(fp[1]), "%s%s_000.bmp", FC_OSD_LOCAL_PATH, fc_variant);
        snprintf(fp[2], sizeof(fp[2]), "%sBTFL_000.bmp", FC_OSD_LOCAL_PATH);
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

// Reset armed state detection (call after calibration reset)
void osd_reset_armed_state() {
    reset_armed_state = true;
    LOGI("OSD armed state detection will be reset");
}

// Betaflight OSD symbol codes
#define SYM_LAT      0x89
#define SYM_LON      0x98
#define SYM_ALTITUDE 0x7F
#define SYM_M        0x0C
#define SYM_FT       0x0F

// Parse latitude value from OSD buffer at the given position
// Returns true if valid latitude was parsed
static bool parse_latitude(int row, int col, double *out_lat) {
    // Parse latitude text after symbol
    // Format: 0x89 followed by ASCII text like " -12.3456789" or " 12.3456789"
    char lat_text[20];
    int text_pos = 0;
    bool parsing = true;

    for (int i = col + 1; i < col + 15 && i < HD_HMAX && parsing; i++) {
        uint16_t c = fc_osd[row][i];
        if (c >= 0x20 && c <= 0x7E) { // ASCII printable
            char ascii = (char)c;
            // Accept digits, decimal point, minus sign, and space
            if ((ascii >= '0' && ascii <= '9') || ascii == '.' || ascii == '-' || ascii == ' ') {
                if (text_pos < sizeof(lat_text) - 1) {
                    lat_text[text_pos++] = ascii;
                }
            } else {
                parsing = false; // Stop at first non-numeric character
            }
        } else {
            parsing = false; // Stop at special characters
        }
    }
    lat_text[text_pos] = '\0';

    // Parse the numeric string
    if (text_pos > 0) {
        double parsed_lat = atof(lat_text);
        if (parsed_lat >= -90.0 && parsed_lat <= 90.0) {
            *out_lat = parsed_lat;
            return true;
        }
    }
    return false;
}

// Parse longitude value from OSD buffer at the given position
// Returns true if valid longitude was parsed
static bool parse_longitude(int row, int col, double *out_lon) {
    // Parse longitude text after symbol
    char lon_text[20];
    int text_pos = 0;
    bool parsing = true;

    for (int i = col + 1; i < col + 15 && i < HD_HMAX && parsing; i++) {
        uint16_t c = fc_osd[row][i];
        if (c >= 0x20 && c <= 0x7E) { // ASCII printable
            char ascii = (char)c;
            if ((ascii >= '0' && ascii <= '9') || ascii == '.' || ascii == '-' || ascii == ' ') {
                if (text_pos < sizeof(lon_text) - 1) {
                    lon_text[text_pos++] = ascii;
                }
            } else {
                parsing = false;
            }
        } else {
            parsing = false;
        }
    }
    lon_text[text_pos] = '\0';

    if (text_pos > 0) {
        double parsed_lon = atof(lon_text);
        if (parsed_lon >= -180.0 && parsed_lon <= 180.0) {
            *out_lon = parsed_lon;
            return true;
        }
    }
    return false;
}

// Parse altitude value from OSD buffer at the given position
// Returns true if valid altitude was parsed (converted to meters)
static bool parse_altitude(int row, int col, float *out_alt) {
    // Log character dump for debugging
    char debug_chars[80];
    int debug_pos = 0;
    for (int j = col; j < col + 12 && j < HD_HMAX; j++) {
        debug_pos += snprintf(debug_chars + debug_pos, sizeof(debug_chars) - debug_pos, "%02X ", fc_osd[row][j]);
    }

    LOGD("Altitude symbol found at row %d, col %d: %s", row, col, debug_chars);

    // Parse altitude text
    char alt_text[15];
    int text_pos = 0;
    bool parsing = true;
    bool found = false;

    for (int i = col + 1; i < col + 12 && i < HD_HMAX && parsing; i++) {
        uint16_t c = fc_osd[row][i];

        // Check for unit symbols FIRST (they are 0x0C and 0x0F, which are < 0x20)
        if (c == SYM_M || c == SYM_FT) {
            alt_text[text_pos] = '\0';
            // Convert feet to meters if needed
            float alt = atof(alt_text);
            if (c == SYM_FT) {
                alt *= 0.3048f; // Convert feet to meters
            }
            LOGI("Altitude parsed: text='%s' value=%.2f%s",
                 alt_text, alt, c == SYM_FT ? " (ft->m)" : "m");

            if (alt >= -500000.0f && alt <= 1000000.0f) { // Reasonable altitude range
                *out_alt = alt;
                found = true;
            }
            parsing = false;
        } else if (c >= 0x20 && c <= 0x7E) {
            // ASCII printable character
            char ascii = (char)c;
            if ((ascii >= '0' && ascii <= '9') || ascii == '.' || ascii == '-' || ascii == ' ') {
                if (text_pos < sizeof(alt_text) - 1) {
                    alt_text[text_pos++] = ascii;
                }
            } else {
                // Non-numeric, non-unit ASCII character - stop
                parsing = false;
            }
        } else {
            // Non-ASCII, non-unit character
            parsing = false;
        }
    }
    return found;
}

// Parse GPS coordinates from Betaflight OSD text
// Scans OSD buffer for GPS LAT/LON symbols and parses the displayed coordinate text
void osd_parse_gps_data() {
    static double gps_lat = 0.0;
    static double gps_lon = 0.0;
    static float gps_alt = 0.0;
    static bool gps_valid = false;
    bool lat_found = false;
    bool lon_found = false;

    // Scan OSD buffer for GPS coordinate symbols
    for (int row = 0; row < HD_VMAX; row++) {
        for (int col = 0; col < HD_HMAX - 1; col++) {
            uint16_t ch = fc_osd[row][col];

            // Look for latitude symbol (0x89)
            if (ch == SYM_LAT && col < HD_HMAX - 10) {
                if (parse_latitude(row, col, &gps_lat)) {
                    lat_found = true;
                }
            }

            // Look for longitude symbol (0x98)
            if (ch == SYM_LON && col < HD_HMAX - 10) {
                if (parse_longitude(row, col, &gps_lon)) {
                    lon_found = true;
                }
            }

            // Look for altitude symbol (0x7F)
            if (ch == SYM_ALTITUDE && col < HD_HMAX - 8) {
                parse_altitude(row, col, &gps_alt);
            }
        }
    }

    // Update GPS data if we found valid coordinates
    if (lat_found && lon_found) {
        gps_valid = true;
        ht_antenna_tracker_update_gps(gps_lat, gps_lon, gps_alt, true);

        // Log when coordinates change significantly (for debugging)
        static double last_lat = 0.0;
        static double last_lon = 0.0;
        if (fabs(gps_lat - last_lat) > 0.00001 || fabs(gps_lon - last_lon) > 0.00001) {
            LOGI("GPS coords parsed from OSD: LAT=%.7f LON=%.7f ALT=%.1fm", gps_lat, gps_lon, gps_alt);

            last_lat = gps_lat;
            last_lon = gps_lon;
        }
    } else if (gps_valid) {
        // Lost GPS fix - mark as invalid
        gps_valid = false;
        ht_antenna_tracker_update_gps(gps_lat, gps_lon, gps_alt, false);
    }
}

// Detect if drone is armed by scanning OSD for armed indicator
// Betaflight displays "ARMED" in center of screen when arming (lasts ~0.5 seconds)
bool osd_detect_armed() {
    bool armed_text_found = false;

    // Scan OSD for "ARMED" text (full word only, not "ARM" or "DISARMED")
    for (int row = 0; row < HD_VMAX; row++) {
        for (int col = 0; col < HD_HMAX - 5; col++) { // Need at least 5 chars for "ARMED"
            // Check for exact match of "ARMED"
            if (fc_osd[row][col] == 'A' &&
                fc_osd[row][col + 1] == 'R' &&
                fc_osd[row][col + 2] == 'M' &&
                fc_osd[row][col + 3] == 'E' &&
                fc_osd[row][col + 4] == 'D') {

                // Make sure it's not part of "DISARMED" by checking previous char
                if (col == 0 || fc_osd[row][col - 1] != 'S') {
                    armed_text_found = true;
                    LOGD("ARMED text found at row %d, col %d", row, col);
                    break;
                }
            }
        }
        if (armed_text_found)
            break;
    }

    return armed_text_found;
}

// Handle GPS parsing, armed detection, and antenna tracker calibration
// This encapsulates all GPS-related logic in the OSD thread
static void handle_gps_and_calibration(bool *was_armed) {
    // Parse GPS coordinates from OSD (updates GPS data)
    osd_parse_gps_data();

    // Detect armed state
    bool is_armed = osd_detect_armed();

    // Check if armed state should be reset (after calibration reset)
    if (reset_armed_state) {
        *was_armed = is_armed; // Sync to current state
        reset_armed_state = false;
        LOGI("Armed state detection reset, current state: %s", is_armed ? "ARMED" : "DISARMED");
    }

    // Auto-calibrate on arm (rising edge)
    // Two-point calibration: first arm = user position, second arm = takeoff position
    if (is_armed && !(*was_armed)) {
        // GPS coordinates have already been parsed by osd_parse_gps_data()
        // Trigger calibration event with the current GPS data
        if (ht_antenna_tracker_is_gps_valid()) {
            uint8_t arm_count = ht_antenna_tracker_get_arm_count();
            LOGI("Drone armed (arm #%d) - triggering antenna tracker calibration step", arm_count + 1);
            ht_antenna_tracker_on_arm_event();
        } else {
            LOGW("Drone armed but no valid GPS fix - cannot calibrate");
        }
    } else if (!is_armed && *was_armed) {
        LOGI("Drone disarmed");
    }
    *was_armed = is_armed;

    // Update calibration instruction text (must be in OSD thread for sync)
    pthread_mutex_lock(&lvgl_mutex);
    uint8_t arm_count = ht_antenna_tracker_get_arm_count();
    bool gps_valid = ht_antenna_tracker_is_gps_valid();

    if (g_setting.ht.enable && gps_valid) {
        if (arm_count == 0) {
            lv_label_set_text(g_osd_hdzero.calibration_instruction[0], "Arm at user position");
            lv_label_set_text(g_osd_hdzero.calibration_instruction[1], "Arm at user position");
            lv_obj_clear_flag(g_osd_hdzero.calibration_instruction[0], LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(g_osd_hdzero.calibration_instruction[1], LV_OBJ_FLAG_HIDDEN);
        } else if (arm_count == 1) {
            lv_label_set_text(g_osd_hdzero.calibration_instruction[0], "Arm at takeoff");
            lv_label_set_text(g_osd_hdzero.calibration_instruction[1], "Arm at takeoff");
            lv_obj_clear_flag(g_osd_hdzero.calibration_instruction[0], LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(g_osd_hdzero.calibration_instruction[1], LV_OBJ_FLAG_HIDDEN);
        } else {
            // Calibration complete, hide instruction
            lv_obj_add_flag(g_osd_hdzero.calibration_instruction[0], LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(g_osd_hdzero.calibration_instruction[1], LV_OBJ_FLAG_HIDDEN);
        }
    } else {
        // Head tracker disabled or no GPS, hide instruction
        lv_obj_add_flag(g_osd_hdzero.calibration_instruction[0], LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(g_osd_hdzero.calibration_instruction[1], LV_OBJ_FLAG_HIDDEN);
    }
    pthread_mutex_unlock(&lvgl_mutex);
}

void *thread_osd(void *ptr) {
    static uint8_t fhd_d = 0;
    static bool was_armed = false;

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

        // Handle GPS parsing and antenna tracker calibration
        handle_gps_and_calibration(&was_armed);
    }
    return NULL;
}
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
#include "ui/page_common.h"
#include "ui/page_fans.h"
#include "ui/ui_image_setting.h"

//////////////////////////////////////////////////////////////////
// local
static sem_t osd_semaphore;

static uint16_t osd_buf_shadow[HD_VMAX][HD_HMAX];

extern lv_style_t style_osd;
extern pthread_mutex_t lvgl_mutex;

///////////////////////////////////////////////////////////////////////////////
// these are local for OSD controlling
static osd_hdzero_t g_osd_hdzero;
static lv_obj_t *img_arr[HD_VMAX][HD_HMAX];
static lv_obj_t *scr_main;
static lv_obj_t *scr_osd;
static uint32_t osdFont[OSD_VNUM][OSD_HNUM][OSD_HEIGHT][OSD_WIDTH]; // 0x00bbggrr
static osd_font_t osd_font;

void osd_llock_show(bool bShow) {
    char buf[128];

    if (!bShow || !g_setting.osd.elements.latency_lock.show) {
        lv_obj_add_flag(g_osd_hdzero.latency_lock, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if (g_latency_locked)
        lv_obj_clear_flag(g_osd_hdzero.latency_lock, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.latency_lock, LV_OBJ_FLAG_HIDDEN);
}

void osd_rec_show(bool bShow) {
    char buf[128];

    if (!bShow || !g_setting.osd.elements.sd_rec.show) {
        lv_obj_add_flag(g_osd_hdzero.sd_rec, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if (!g_sdcard_enable) {
        sprintf(buf, "%s%s", RESOURCE_PATH, noSdcard_bmp);
        lv_img_set_src(g_osd_hdzero.sd_rec, buf);
        lv_obj_clear_flag(g_osd_hdzero.sd_rec, LV_OBJ_FLAG_HIDDEN);
    } else {
        if (dvr_is_recording) {
            sprintf(buf, "%s%s", RESOURCE_PATH, recording_bmp);
            lv_img_set_src(g_osd_hdzero.sd_rec, buf);
            lv_obj_clear_flag(g_osd_hdzero.sd_rec, LV_OBJ_FLAG_HIDDEN);
        } else
            lv_obj_add_flag(g_osd_hdzero.sd_rec, LV_OBJ_FLAG_HIDDEN);
    }
}

void osd_battery_show() {
    if (g_setting.power.warning_type == 0) { // Beep only
        lv_obj_add_flag(g_osd_hdzero.battery_low, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if (battery_is_low() && g_setting.osd.elements.battery_low.show)
        lv_obj_clear_flag(g_osd_hdzero.battery_low, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.battery_low, LV_OBJ_FLAG_HIDDEN);
}

void osd_topfan_show(bool bShow) {
    char buf[128];
    if (!bShow || !g_setting.osd.elements.topfan_speed.show) {
        lv_obj_add_flag(g_osd_hdzero.topfan_speed, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if (g_setting.fans.top_speed > 5)
        return;
    sprintf(buf, "%sfan%d.bmp", RESOURCE_PATH, fan_speeds[2] + 1);
    lv_img_set_src(g_osd_hdzero.topfan_speed, buf);
    lv_obj_clear_flag(g_osd_hdzero.topfan_speed, LV_OBJ_FLAG_HIDDEN);
}

void osd_vrxtemp_show() {
    if (g_temperature.is_rescuing && g_setting.osd.elements.vrx_temp.show)
        lv_obj_clear_flag(g_osd_hdzero.vrx_temp, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.vrx_temp, LV_OBJ_FLAG_HIDDEN);
}

void osd_vlq_show(bool bShow) {
    char buf[128];
    if (!bShow || !g_setting.osd.elements.vlq.show) {
        lv_obj_add_flag(g_osd_hdzero.vlq, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if (link_quality > 8)
        return;
    if (rx_status[0].rx_valid || rx_status[1].rx_valid)
        sprintf(buf, "%sVLQ%d.bmp", RESOURCE_PATH, link_quality + 1); // VLQ7.bmp
    else
        sprintf(buf, "%sVLQ1.bmp", RESOURCE_PATH); // VLQ1.bmp

    lv_img_set_src(g_osd_hdzero.vlq, buf);
    lv_obj_clear_flag(g_osd_hdzero.vlq, LV_OBJ_FLAG_HIDDEN);
}

///////////////////////////////////:////////////////////////////////////////////
// OSD channel
// channel_osd_mode
//  = 0x80 | Channel
//  = 0x00 | Channel Show Time
uint8_t channel_osd_mode;

char *channel2str(uint8_t channel) // channel=1:10
{
    static char *ChannelName[] = {
        "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8",
        "F2", "F4", ""};

    if ((channel > 0) && (channel < 11))
        return ChannelName[channel - 1];
    else
        return ChannelName[0];
}

void osd_channel_show(bool bShow) {
    uint8_t ch;
    lv_color_t color;
    char buf[32];

    if (channel_osd_mode & 0x80) {
        ch = channel_osd_mode & 0xF;
        color = lv_color_make(0xFF, 0x20, 0x20);
        sprintf(buf, "  To %s?  ", channel2str(ch));
        lv_obj_set_style_bg_opa(g_osd_hdzero.channel, LV_OPA_100, 0);
    } else {
        ch = g_setting.scan.channel;
        color = lv_color_make(0xFF, 0xFF, 0xFF);
        sprintf(buf, "CH:%s", channel2str(ch));
        lv_obj_set_style_bg_opa(g_osd_hdzero.channel, 0, 0);
    }

    lv_label_set_text(g_osd_hdzero.channel, buf);
    lv_obj_set_style_text_color(g_osd_hdzero.channel, color, 0);

    if (channel_osd_mode & 0x80)
        lv_obj_clear_flag(g_osd_hdzero.channel, LV_OBJ_FLAG_HIDDEN);
    else if (bShow && channel_osd_mode && g_setting.osd.elements.channel.show)
        lv_obj_clear_flag(g_osd_hdzero.channel, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.channel, LV_OBJ_FLAG_HIDDEN);
}

static void osd_object_set_pos(lv_obj_t *obj, setting_osd_goggle_element_positions_t *pos) {
    switch (g_setting.osd.embedded_mode) {
    case EMBEDDED_16x9:
        lv_obj_set_pos(obj, pos->mode_16_9.x, pos->mode_16_9.y);
        break;

    default:
    case EMBEDDED_4x3:
        lv_obj_set_pos(obj, pos->mode_4_3.x, pos->mode_4_3.y);
        break;
    }
}

static void osd_object_create_gif(lv_obj_t **obj, const char *img, setting_osd_goggle_element_positions_t *pos) {
    *obj = lv_gif_create(scr_osd);
    lv_gif_set_src(*obj, img);
    lv_obj_set_size(*obj, 36, 36);
    osd_object_set_pos(*obj, pos);
}

static void osd_object_create_img(lv_obj_t **obj, const char *img, setting_osd_goggle_element_positions_t *pos) {
    *obj = lv_img_create(scr_osd);
    lv_img_set_src(*obj, img);
    lv_obj_set_size(*obj, 36, 36);
    osd_object_set_pos(*obj, pos);
}

static void osd_object_create_label(lv_obj_t **obj, char *text, setting_osd_goggle_element_positions_t *pos) {
    *obj = lv_label_create(scr_osd);
    lv_label_set_text(*obj, text);
    osd_object_set_pos(*obj, pos);
    lv_obj_set_style_text_color(g_osd_hdzero.channel, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(g_osd_hdzero.channel, &lv_font_montserrat_26, 0);
    lv_obj_set_style_bg_color(g_osd_hdzero.channel, lv_color_hex(0x010101), LV_PART_MAIN);
    lv_obj_set_style_radius(g_osd_hdzero.channel, 50, 0);
}

void osd_show(bool show) {
    if (show)
        lv_obj_clear_flag(scr_osd, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(scr_osd, LV_OBJ_FLAG_HIDDEN);
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

void osd_hdzero_update(void) {
    char buf[128], i;
    bool showRXOSD = g_showRXOSD && (g_source_info.source == SOURCE_HDZERO);

    osd_battery_show();
    osd_rec_show(g_showRXOSD);
    osd_llock_show(g_showRXOSD);
    osd_topfan_show(g_showRXOSD);

    osd_vrxtemp_show();

    if (showRXOSD && g_osd_hdzero.vtx_temp) {
        if (vtxTempInfo & 0x80) {
            i = vtxTempInfo & 0xF;
            if (i == 0)
                i = 1;
            else if (i > 8)
                i = 8;
            sprintf(buf, "%sVtxTemp%d.bmp", RESOURCE_PATH, i);
        } else
            sprintf(buf, "%s%s", RESOURCE_PATH, blank_bmp);
        lv_img_set_src(g_osd_hdzero.vtx_temp, buf);
    }

    if (showRXOSD && g_setting.osd.elements.vtx_temp.show)
        lv_obj_clear_flag(g_osd_hdzero.vtx_temp, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.vtx_temp, LV_OBJ_FLAG_HIDDEN);

    osd_channel_show(showRXOSD);

    osd_vlq_show(showRXOSD);

    sprintf(buf, "%sant%d.bmp", RESOURCE_PATH, RSSI2Ant(rx_status[0].rx_rssi[0]));
    lv_img_set_src(g_osd_hdzero.ant0, buf);
    sprintf(buf, "%sant%d.bmp", RESOURCE_PATH, RSSI2Ant(rx_status[0].rx_rssi[1]));
    lv_img_set_src(g_osd_hdzero.ant1, buf);
    sprintf(buf, "%sant%d.bmp", RESOURCE_PATH, RSSI2Ant(rx_status[1].rx_rssi[0]));
    lv_img_set_src(g_osd_hdzero.ant2, buf);
    sprintf(buf, "%sant%d.bmp", RESOURCE_PATH, RSSI2Ant(rx_status[1].rx_rssi[1]));
    lv_img_set_src(g_osd_hdzero.ant3, buf);

    if (showRXOSD && g_setting.osd.elements.ant0.show)
        lv_obj_clear_flag(g_osd_hdzero.ant0, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant0, LV_OBJ_FLAG_HIDDEN);

    if (showRXOSD && g_setting.osd.elements.ant1.show)
        lv_obj_clear_flag(g_osd_hdzero.ant1, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant1, LV_OBJ_FLAG_HIDDEN);

    if (showRXOSD && g_setting.osd.elements.ant2.show)
        lv_obj_clear_flag(g_osd_hdzero.ant2, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant2, LV_OBJ_FLAG_HIDDEN);

    if (showRXOSD && g_setting.osd.elements.ant3.show)
        lv_obj_clear_flag(g_osd_hdzero.ant3, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(g_osd_hdzero.ant3, LV_OBJ_FLAG_HIDDEN);

    if (g_test_en) {
        if (g_setting.osd.elements.goggle_temp_top.show) {
            sprintf(buf, "T:%d-%d", fan_speeds[2], g_temperature.top / 10);
            lv_label_set_text(g_osd_hdzero.goggle_temp_top, buf);
            lv_obj_clear_flag(g_osd_hdzero.goggle_temp_top, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(g_osd_hdzero.goggle_temp_top, LV_OBJ_FLAG_HIDDEN);
        }

        if (g_setting.osd.elements.goggle_temp_left.show) {
            sprintf(buf, "L:%d-%d", fan_speeds[1], g_temperature.left / 10);
            lv_label_set_text(g_osd_hdzero.goggle_temp_left, buf);
            lv_obj_clear_flag(g_osd_hdzero.goggle_temp_left, LV_OBJ_FLAG_HIDDEN);
        } else
            lv_obj_add_flag(g_osd_hdzero.goggle_temp_left, LV_OBJ_FLAG_HIDDEN);

        if (g_setting.osd.elements.goggle_temp_right.show) {
            sprintf(buf, "R:%d-%d", fan_speeds[0], g_temperature.right / 10);
            lv_label_set_text(g_osd_hdzero.goggle_temp_right, buf);
            lv_obj_clear_flag(g_osd_hdzero.goggle_temp_right, LV_OBJ_FLAG_HIDDEN);
        } else
            lv_obj_add_flag(g_osd_hdzero.goggle_temp_right, LV_OBJ_FLAG_HIDDEN);
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
    lv_img_set_src(img_arr[row][col], &osd_font.data[index]);
    pthread_mutex_unlock(&lvgl_mutex);

    return 0;
}

static void embedded_osd_init(void) {
    char buf[128];

    sprintf(buf, "%s%s", RESOURCE_PATH, fan1_bmp);
    osd_object_create_img(&g_osd_hdzero.topfan_speed, buf, &g_setting.osd.elements.topfan_speed.position);

    sprintf(buf, "%s%s", RESOURCE_PATH, LLOCK_bmp);
    osd_object_create_img(&g_osd_hdzero.latency_lock, buf, &g_setting.osd.elements.latency_lock.position);

    sprintf(buf, "%s%s", RESOURCE_PATH, VtxTemp1_bmp);
    osd_object_create_img(&g_osd_hdzero.vtx_temp, buf, &g_setting.osd.elements.vtx_temp.position);

    sprintf(buf, "%s%s", RESOURCE_PATH, VrxTemp7_gif);
    osd_object_create_gif(&g_osd_hdzero.vrx_temp, buf, &g_setting.osd.elements.vrx_temp.position);

    sprintf(buf, "%s%s", RESOURCE_PATH, lowBattery_gif);
    osd_object_create_gif(&g_osd_hdzero.battery_low, buf, &g_setting.osd.elements.battery_low.position);

    osd_object_create_label(&g_osd_hdzero.channel, "CH:-- ", &g_setting.osd.elements.channel.position);
    channel_osd_mode = 0;

    sprintf(buf, "%s%s", RESOURCE_PATH, noSdcard_bmp);
    osd_object_create_img(&g_osd_hdzero.sd_rec, buf, &g_setting.osd.elements.sd_rec.position);

    sprintf(buf, "%s%s", RESOURCE_PATH, VLQ1_bmp);
    osd_object_create_img(&g_osd_hdzero.vlq, buf, &g_setting.osd.elements.vlq.position);

    sprintf(buf, "%s%s", RESOURCE_PATH, ant1_bmp);
    osd_object_create_img(&g_osd_hdzero.ant0, buf, &g_setting.osd.elements.ant0.position);
    osd_object_create_img(&g_osd_hdzero.ant1, buf, &g_setting.osd.elements.ant1.position);
    osd_object_create_img(&g_osd_hdzero.ant2, buf, &g_setting.osd.elements.ant2.position);
    osd_object_create_img(&g_osd_hdzero.ant3, buf, &g_setting.osd.elements.ant3.position);

    if (g_test_en) {
        osd_object_create_label(&g_osd_hdzero.goggle_temp_top, "TOP:-.- oC", &g_setting.osd.elements.goggle_temp_top.position);
        osd_object_create_label(&g_osd_hdzero.goggle_temp_left, "LEFT:-.- oC", &g_setting.osd.elements.goggle_temp_left.position);
        osd_object_create_label(&g_osd_hdzero.goggle_temp_right, "RIGHT:-.- oC", &g_setting.osd.elements.goggle_temp_right.position);
    }
}

void osd_update_mode() {
    osd_object_set_pos(g_osd_hdzero.topfan_speed, &g_setting.osd.elements.topfan_speed.position);
    osd_object_set_pos(g_osd_hdzero.latency_lock, &g_setting.osd.elements.latency_lock.position);
    osd_object_set_pos(g_osd_hdzero.vtx_temp, &g_setting.osd.elements.vtx_temp.position);
    osd_object_set_pos(g_osd_hdzero.vrx_temp, &g_setting.osd.elements.vrx_temp.position);
    osd_object_set_pos(g_osd_hdzero.battery_low, &g_setting.osd.elements.battery_low.position);
    osd_object_set_pos(g_osd_hdzero.channel, &g_setting.osd.elements.channel.position);
    osd_object_set_pos(g_osd_hdzero.sd_rec, &g_setting.osd.elements.sd_rec.position);
    osd_object_set_pos(g_osd_hdzero.vlq, &g_setting.osd.elements.vlq.position);
    osd_object_set_pos(g_osd_hdzero.ant0, &g_setting.osd.elements.ant0.position);
    osd_object_set_pos(g_osd_hdzero.ant1, &g_setting.osd.elements.ant1.position);
    osd_object_set_pos(g_osd_hdzero.ant2, &g_setting.osd.elements.ant2.position);
    osd_object_set_pos(g_osd_hdzero.ant3, &g_setting.osd.elements.ant3.position);

    if (g_test_en) {
        osd_object_set_pos(g_osd_hdzero.goggle_temp_top, &g_setting.osd.elements.goggle_temp_top.position);
        osd_object_set_pos(g_osd_hdzero.goggle_temp_left, &g_setting.osd.elements.goggle_temp_left.position);
        osd_object_set_pos(g_osd_hdzero.goggle_temp_right, &g_setting.osd.elements.goggle_temp_right.position);
    }
}

static void fc_osd_init(void) {
    const uint16_t OFFSET_X = 20;
    const uint16_t OFFSET_Y = 40;

    load_fc_osd_font();

    for (int i = 0; i < HD_VMAX; i++) {
        for (int j = 0; j < HD_HMAX; j++) {
            pthread_mutex_lock(&lvgl_mutex);
            img_arr[i][j] = lv_img_create(scr_osd);
            lv_obj_set_size(img_arr[i][j], OSD_WIDTH, OSD_HEIGHT);
            lv_obj_set_pos(img_arr[i][j], j * OSD_WIDTH + OFFSET_X, i * OSD_HEIGHT + OFFSET_Y);
            pthread_mutex_unlock(&lvgl_mutex);
        }
    }
}

static void create_osd_scr(void) {
    scr_main = lv_scr_act();
    scr_osd = lv_obj_create(scr_main);
    lv_obj_clear_flag(scr_main, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(scr_osd, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(scr_osd, 1280, 720);
    lv_obj_add_flag(scr_osd, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_style(scr_osd, &style_osd, 0);
}

int osd_init(void) {
    create_osd_scr();

    fc_osd_init();
    embedded_osd_init();

    sem_init(&osd_semaphore, 0, 1);

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
// load_osd_file
// load_fc_osd_font
int load_fc_osd_font_bmp(const char *file) {
    char *buf;
    struct stat stFile;
    int fd, size, rd;
    int boundry_width;
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

    if (bmp->info.biWidth == (OSD_HNUM * OSD_WIDTH)) {
        // no boundry
        boundry_width = OSD_BOUNDRY_0;
        line_size = LINE_LENGTH_0;
    } else {
        // have boundry
        boundry_width = OSD_BOUNDRY_1;
        line_size = LINE_LENGTH_1;
    }

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
            addr += v * OSD_HEIGHT * line_size;
            addr += h * OSD_WIDTH * 3;

            for (y = 0; y < OSD_HEIGHT; y++) {
                for (x = 0; x < OSD_WIDTH; x++) {
                    osdFont[OSD_VNUM - v - 1][h][OSD_HEIGHT - y - 1][x] = (0xff << 24) + ((pb[addr + x * 3] & 0xff)) + ((pb[addr + x * 3 + 1] & 0xff) << 8) + ((pb[addr + x * 3 + 2] & 0xff) << 16);
                }
                addr += line_size;
            }
        }
    }
    for (v = 0; v < OSD_VNUM; v++) {
        for (h = 0; h < OSD_HNUM; h++) {
            int index = v * OSD_HNUM + h;
            osd_font.data[index].header.cf = LV_IMG_CF_TRUE_COLOR;
            osd_font.data[index].header.always_zero = 0;
            osd_font.data[index].header.reserved = 0;
            osd_font.data[index].header.w = 24;
            osd_font.data[index].header.h = 36;
            osd_font.data[index].data_size = 24 * 36 * LV_COLOR_SIZE / 8;
            osd_font.data[index].data = (uint8_t *)&osdFont[v][h][0][0];
        }
    }
    // free(buf); //FIX ME, ntant, it seems system becomes unstable if uncomment this ???
    return 0;
}

void load_fc_osd_font(void) {
    char fp[3][256];
    int i;

    sprintf(fp[0], "%s%s_000.bmp", FC_OSD_SDCARD_PATH, fc_variant);
    sprintf(fp[1], "%s%s_000.bmp", FC_OSD_LOCAL_PATH, fc_variant);
    sprintf(fp[2], "%sBTFL_000.bmp", FC_OSD_LOCAL_PATH);

    for (i = 0; i < 3; i++) {
        if (!load_fc_osd_font_bmp(fp[i])) {
            LOGI(" succecss!");
            return;
        } else
            LOGE(" failed!");
    }
}

///////////////////////////////////////////////////////////////////////////////
// Threads for updating FC OSD

void osd_signal_update() {
    sem_post(&osd_semaphore);
}

void *thread_osd(void *ptr) {
    for (;;) {
        // wait for signal to render
        sem_wait(&osd_semaphore);

        for (int i = 0; i < HD_VMAX; i++) {
            for (int j = 0; j < HD_HMAX; j++) {
                uint16_t ch = osd_buf[i][j];
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

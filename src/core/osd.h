#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <lvgl/lvgl.h>

#include "defines.h"

#define OSD_VNUM       32
#define OSD_HNUM       16
#define OSD_WIDTH_HD   24
#define OSD_HEIGHT_HD  36
#define OSD_WIDTH_FHD  36
#define OSD_HEIGHT_FHD 54
#define OSD_BOUNDRY_0  0
#define OSD_BOUNDRY_1  6

typedef enum {
    OSD_RESOURCE_720 = 0,
    OSD_RESOURCE_1080,

    OSD_RESOURCE_TOTAL
} osd_resource_t;

typedef enum {
    OSD_CLOCK_DATE = 0,
    OSD_CLOCK_TIME,
    OSD_CLOCK_FORMAT,

    OSD_CLOCK_TOTAL
} osd_clock_t;

typedef struct {
    lv_obj_t *topfan_speed[2];
    lv_obj_t *vtx_temp[2];
    lv_obj_t *battery_low[2];
    lv_obj_t *battery_voltage[2];
    lv_obj_t *vrx_temp[2];
    lv_obj_t *latency_lock[2];
    lv_obj_t *channel[2];
    lv_obj_t *sd_rec[2];
    lv_obj_t *vlq[2];
    lv_obj_t *ant0[2];
    lv_obj_t *ant1[2];
    lv_obj_t *ant2[2];
    lv_obj_t *ant3[2];
    lv_obj_t *osd_tempe[2][3]; // top,left,bot
    lv_obj_t *clock[2][OSD_CLOCK_TOTAL];
} osd_hdzero_t;

typedef struct
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} __attribute__((packed)) bitMAPFILEHEADER;

typedef struct
{
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} __attribute__((packed)) bitMAPINFOHEADER;

typedef struct
{
    bitMAPFILEHEADER file;
    bitMAPINFOHEADER info;
} __attribute__((packed)) bmpFileHead;

typedef struct {
    lv_img_dsc_t data[512];
} osd_font_t;

extern uint8_t channel_osd_mode;
extern lv_img_dsc_t img_menu_bg;

int osd_init(void);
int osd_clear(void);
void osd_fhd(uint8_t);
void osd_signal_update();
void osd_hdzero_update(void);
void osd_rec_update(bool enable);
void osd_show(bool show);
void osd_update_element_positions();
char *channel2str(uint8_t band, uint8_t channel);
void load_fc_osd_font(uint8_t);
void *thread_osd(void *ptr);
void osd_resource_path(char *buf, const char *fmt, osd_resource_t osd_resource_type, ...);
void osd_toggle();
int load_wallpaper(char *file_path);
#ifdef __cplusplus
}
#endif

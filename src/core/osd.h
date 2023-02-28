#ifndef _OSD_H
#define _OSD_H

#include <stdbool.h>

#include <lvgl/lvgl.h>

#include "defines.h"

#define OSD_VNUM      32
#define OSD_HNUM      16
#define OSD_WIDTH     24
#define OSD_HEIGHT    36
#define OSD_BOUNDRY_0 0
#define OSD_BOUNDRY_1 6
#define LINE_LENGTH_0 (OSD_WIDTH * OSD_HNUM * 3) // no boundry
#define LINE_LENGTH_1 1460                       // bmp have boundry

typedef struct {
    lv_obj_t *topfan_speed;
    lv_obj_t *latency_lock;
    lv_obj_t *vtx_temp;
    lv_obj_t *vrx_temp;
    lv_obj_t *battery_low;
    lv_obj_t *channel;
    lv_obj_t *sd_rec;
    lv_obj_t *vlq;
    lv_obj_t *ant0;
    lv_obj_t *ant1;
    lv_obj_t *ant2;
    lv_obj_t *ant3;
    lv_obj_t *goggle_temp_top;
    lv_obj_t *goggle_temp_left;
    lv_obj_t *goggle_temp_right;
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

int osd_init(void);
int osd_clear(void);
void osd_signal_update();
void osd_hdzero_update(void);
void osd_rec_update(bool enable);
void osd_show(bool show);
void osd_update_mode();
char *channel2str(uint8_t channel);
void load_fc_osd_font(void);
void *thread_osd(void *ptr);

#endif
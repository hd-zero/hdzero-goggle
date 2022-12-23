#ifndef _OSD_H
#define _OSD_H

#include <stdbool.h>
#include "lvgl/lvgl.h"
#include "defines.h"

#define OSD_VNUM 32  
#define OSD_HNUM 16
#define OSD_WIDTH 24
#define OSD_HEIGHT 36
#define OSD_BOUNDRY_0 0
#define OSD_BOUNDRY_1 6
#define LINE_LENGTH_0 (OSD_WIDTH * OSD_HNUM * 3) //no boundry
#define LINE_LENGTH_1 1460 //bmp have boundry

typedef struct {
	lv_obj_t *topfan_speed;		//0
	lv_obj_t *vtx_temp;		    //1
	lv_obj_t *battery;			//2	
	lv_obj_t *vrx_temp;			//3
    lv_obj_t *latency_lock;  	//4
	lv_obj_t *ch;				//middle
	lv_obj_t *sd_rec;			//5
	lv_obj_t *vlq;				//6
	lv_obj_t *ant0;				//7
	lv_obj_t *ant1;				//8
	lv_obj_t *ant2;				//9
	lv_obj_t *ant3;				//10
    lv_obj_t *osd_tempe[3];         //top,left,bot	
} osd_hdzero_t;


typedef struct
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
}__attribute__((packed)) bitMAPFILEHEADER ;

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
}__attribute__((packed)) bitMAPINFOHEADER;

typedef struct
{
    bitMAPFILEHEADER file;
    bitMAPINFOHEADER info;
}__attribute__((packed)) bmpFileHead;


typedef struct {
	lv_img_dsc_t data[512];
} osd_font_t;


extern bool is_recording;
extern uint8_t channel_osd_mode;

int draw_osd_clear(void);
int draw_osd_on_screen(uint8_t row, uint8_t col);
int osd_init(void);
void osd_hdzero_update(void);
void osd_rec_update(bool enable);
void osd_show(bool show);
char* channel2str(uint8_t channel);

void load_fc_osd_font(void);
void rbtn_click(bool is_short, int mode);

void *thread_osd(void *ptr);

void sel_audio_source(uint8_t audio_source);
void enable_line_out(bool bEnable);
void confirm_recording();
#endif

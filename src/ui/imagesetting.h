#ifndef __IMAGESETTING_H__
#define __IMAGESETTING_H__

#include <stdbool.h>
#include <stdint.h>

#define IMS_CANVAS_WIDTH    430
#define IMS_CANVAS_HEIGHT   160
#define SLIDER_XSTART       170
#define SLIDER_XEND         350
#define SLIDER_WIDTH        5

#define DARK                lv_color_make(0x26,0x26,0x26)
//#define DARK                lv_color_make(127,127,127) //transparent
#define DARK_GRAY           lv_color_make(0x80,0x80,0x80)
#define LIGHT_WHITE         lv_color_make(254,254,254)
#define LIGHT_GREEN         lv_color_make(0,255,0)

#define IMS_DEFAULT_LUM      8
#define IMS_DEFAULT_BRI      39
#define IMS_DEFAULT_SAT      16
#define IMS_DEFAULT_CON      16
#define IMS_DEFAULT_AO       2

#define IMS_ITEM_COUNT       7

typedef struct {
	int16_t     x;      //start pos
    int16_t     y;
    uint8_t     type;   //1=slider, 0=no slider
    char        title[32];
    int16_t     range[2];
    int16_t     value;
    uint8_t     state;  //0=not selected, 1=selected, 2=slider bar selected
} ims_slider_t;


typedef struct {
	ims_slider_t  items[IMS_ITEM_COUNT]; 
    uint8_t       selection;
} ims_page_t;


extern bool g_bShowIMS;

void ims_init(void);
uint8_t ims_key(uint8_t key);
void ims_update();
void save_ims();

#endif
#include <stdio.h>
#include "statusbar.h"
#include "lvgl/lvgl.h"
#include "page_common.h"
#include "playback.h"
#include "style.h"
#include "mcp3021.h"
#include "osd.h"
#include "common.hh"

///////////////////////////////////////////////////////////////////////////////
//local
static lv_obj_t * label0;
static lv_obj_t * label1;
static lv_obj_t * label2;
static lv_obj_t * label3;
static lv_obj_t * label4;

static lv_obj_t *img_sdc;
LV_IMG_DECLARE(img_sdcard);
LV_IMG_DECLARE(img_noSdcard);

static lv_obj_t *img_battery;
LV_IMG_DECLARE(img_bat);
LV_IMG_DECLARE(img_lowBattery);


int statusbar_init(void)
{
	char buf[128];

    static lv_coord_t col_dsc[] = {264, 64, 267, 64, 267,64, 267, 64, 267, 64, 267, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {48, 96, LV_GRID_TEMPLATE_LAST};

    /*Create a container with grid*/
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 1920, 96);
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

    label0 = lv_label_create(cont);
    lv_label_set_long_mode(label0, LV_LABEL_LONG_SCROLL_CIRCULAR); 
    lv_label_set_text(label0, "SD Card                 ");
    lv_obj_set_width(label0, 267);  /*Set smaller width to make the lines wrap*/
	lv_label_set_recolor(label0, true);    
    lv_obj_set_style_text_align(label0, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_set_style_text_color(label0, lv_color_make(255,255,255), 0);
	lv_obj_set_grid_cell(label0, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    label1 = lv_label_create(cont);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_DOT);     /*Break the long lines*/

	sprintf(buf, "RF:HDZero %s", channel2str(g_setting.scan.channel));

    lv_label_set_text(label1, buf);
    lv_obj_set_width(label1, 267);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_set_style_text_color(label1, lv_color_make(255,255,255), 0);
	lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_CENTER, 4, 1,
						 LV_GRID_ALIGN_CENTER, 0, 1);

    label2 = lv_label_create(cont);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_DOT);     /*Break the long lines*/
    lv_label_set_text(label2, "ESP32:Off");
    lv_obj_set_width(label2, 267);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_set_style_text_color(label2, lv_color_make(255,255,255), 0);
	lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_CENTER, 6, 1,
						 LV_GRID_ALIGN_CENTER, 0, 1);

    label3 = lv_label_create(cont);
    lv_label_set_long_mode(label3, LV_LABEL_LONG_DOT);     /*Break the long lines*/
    lv_label_set_text(label3, "Wifi:Off");
    lv_obj_set_width(label3, 267);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label3, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_set_style_text_color(label3, lv_color_make(255,255,255), 0);
	lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_CENTER, 8, 1,
						 LV_GRID_ALIGN_CENTER, 0, 1);

    label4 = lv_label_create(cont);
    lv_label_set_long_mode(label4, LV_LABEL_LONG_DOT);     /*Break the long lines*/
    lv_label_set_text(label4, "       ");
    lv_obj_set_width(label4, 267);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label4, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_set_style_text_color(label4, lv_color_make(255,255,255), 0);
	lv_obj_set_grid_cell(label4, LV_GRID_ALIGN_CENTER, 10, 1,
						 LV_GRID_ALIGN_CENTER, 0, 1);

	lv_obj_set_style_text_font(label0, &lv_font_montserrat_26, 0);
	lv_obj_set_style_text_font(label1, &lv_font_montserrat_26, 0);
	lv_obj_set_style_text_font(label2, &lv_font_montserrat_26, 0);
	lv_obj_set_style_text_font(label3, &lv_font_montserrat_26, 0);
	lv_obj_set_style_text_font(label4, &lv_font_montserrat_26, 0);

	return 0;
}

void  statubar_update(void)
{
	char buf[128];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%dS %d.%dV", g_battery.type,
			 g_battery.voltage/1000,
			 g_battery.voltage%1000/100);

    lv_label_set_text(label4, buf);

	{	
		#define BEEP_INTERVAL 20
		static uint8_t beep_gap = 0;
		int vol = g_battery.voltage/g_battery.type;
		bool low = (vol  <  g_setting.power.voltage * 100);

		if(low)
			lv_img_set_src(img_battery, &img_lowBattery);
		else
			lv_img_set_src(img_battery, &img_bat);

		switch(g_setting.power.warning_type) {
			case 0: //beep only:
				if(low) {
					if(beep_gap++ == BEEP_INTERVAL) {
						beep();
						beep_gap = 0;
					}
				lv_obj_set_style_text_color(label4, lv_color_make(255,255,255), 0);
				break;
			
			case 1: //visual
				if(low) 
					lv_obj_set_style_text_color(label4, lv_color_make(255,0,0), 0);
				else
					lv_obj_set_style_text_color(label4, lv_color_make(255,255,255), 0);
				break;

			case 2:
				if(low) {
					if(beep_gap++ == BEEP_INTERVAL) {
						beep();
						beep_gap = 0;
					}
					lv_obj_set_style_text_color(label4, lv_color_make(255,0,0), 0);
				}
				else
					lv_obj_set_style_text_color(label4, lv_color_make(255,255,255), 0);
				break;
			default:	
				break;	
			}
		}
	}


	static int channel_last = 0; 
	static int source_last = 0;
	if((channel_last != g_setting.scan.channel) || (source_last != g_source_info.source))
	{
		memset(buf, 0, sizeof(buf));
		if(g_source_info.source == 0) { //HDZero
			if(g_setting.scan.channel > 8)
				sprintf(buf, "RF:HDZero F%d", (g_setting.scan.channel - 8)*2);
			else
				sprintf(buf, "RF:HDZero R%d", g_setting.scan.channel);
		}
		else if(g_source_info.source == 1) 
			sprintf(buf, "HDMI In");
		else if(g_source_info.source == 2) 
			sprintf(buf, "AV In");
		else	
			sprintf(buf, "Expansion Module");

		lv_label_set_text(label1, buf);
	}
	channel_last = g_setting.scan.channel;
	source_last = g_source_info.source;



	if(g_sdcard_enable){
		int cnt = get_videofile_cnt();
		float gb = g_sdcard_size/1024.0;
		bool bFull = g_sdcard_size < 103;
		lv_img_set_src(img_sdc, &img_sdcard);
		if(cnt != 0) {
			if(bFull) 
				sprintf(buf, "%d clip(s), SD Card Full", cnt);
			else 	
 				sprintf(buf, "%d clip(s), %.2fGB available", cnt,gb);
		}
		else {
			if(bFull)
				sprintf(buf, "#FF0000 SD Card Full#");
			else 
				sprintf(buf, "%.2fGB available", gb);
		}
	}
	else {
		sprintf(buf, "No SD card");
		lv_img_set_src(img_sdc, &img_noSdcard);
	}

	lv_label_set_text(label0, buf);
}

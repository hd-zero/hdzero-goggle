#include "osd.h"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "common.hh"
#include "lvgl/lvgl.h"
#include "msp_displayport.h"
#include "../driver/mcp3021.h"
#include "../driver/nct75.h"
#include "../page/page_common.h"
#include "../page/page_fans.h"
#include "../driver/dm5680.h"
#include "../driver/hardware.h"
#include "fbtools.h"
#include "../minIni/minIni.h"
#include "../driver/fans.h"
#include "../core/imagesetting.h"

//////////////////////////////////////////////////////////////////
//Global
bool is_recording = false;

//local
static pthread_mutex_t dvr_mutex;

extern lv_style_t style_osd;
extern pthread_mutex_t lvgl_mutex;


///////////////////////////////////////////////////////////////////
//-1=error;
// 0=idle,1=recording,2=stopped,3=No SD card,4=recorf file path error,
// 5=SD card Full,6=Encoder error
void confirm_recording()
{
	int    ret = -1;
	FILE*  fp;  
	pthread_mutex_lock(&dvr_mutex);
	if(is_recording) {
		
		fp = fopen("/tmp/record.dat","r");  
		if(fp) {
			fscanf(fp,"%d",&ret);
			fclose(fp);
			//printf("SD card %d\n",ret);
		}
		if(ret != 1)
			osd_rec_update(false);
	}
	pthread_mutex_unlock(&dvr_mutex);
}

void enable_line_out(bool bEnable)
{
	char buf[128];
	if(bEnable) {
		sprintf(buf,"%s out_on", AUDIO_SEL_SH);
		system(buf);
		sprintf(buf,"%s out_linein_on", AUDIO_SEL_SH);
		system(buf);
		sprintf(buf,"%s out_dac_off", AUDIO_SEL_SH);
		system(buf);
	}
	else {
		sprintf(buf,"%s out_off", AUDIO_SEL_SH);
		system(buf);
	}
}

void sel_audio_source(uint8_t source)
{
	char buf[128];
	char* audio_source[3] = {
		"in_mic1",
		"in_mic2",
		"in_linein"
	};

	if(source>2) source = 2;
	sprintf(buf,"%s %s", AUDIO_SEL_SH, audio_source[source]);
	system(buf);
}

void update_record_conf()
{
	if(g_setting.record.format_ts)
		ini_puts("record", "type", "ts", REC_CONF);
	else
		ini_puts("record", "type", "mp4", REC_CONF);

	if(g_source_info.source == 0) {
		if(CAM_MODE == VR_540P90 || CAM_MODE == VR_540P90_CROP) {  //90fps
			ini_putl("venc", "fps", 90,    REC_CONF);
			ini_putl("venc", "kbps",34000, REC_CONF);
			ini_putl("venc", "h265",0,     REC_CONF);
			ini_putl("vi",   "fps", 90,    REC_CONF);
		}
		else {
			ini_putl("venc", "fps", 60,    REC_CONF);
			ini_putl("venc", "kbps",24000, REC_CONF);
			ini_putl("venc", "h265", 1,    REC_CONF);
			ini_putl("vi",   "fps", 60,    REC_CONF);
		}
	}
	else { //AV -- HDMI no record
		ini_putl("venc", "kbps",24000, REC_CONF);
		ini_putl("venc", "h265", 1,    REC_CONF);
		ini_putl("vi",   "fps", 60,    REC_CONF);
		if(g_hw_stat.av_pal)
			ini_putl("venc", "fps", 50,    REC_CONF);
		else	
			ini_putl("venc", "fps", 60,    REC_CONF);
	}

	ini_putl("record", "audio", g_setting.record.audio, REC_CONF);
	sel_audio_source(g_setting.record.audio_source);
}

//////////////////////////////////////////////////////////////////
//is_short: 
// =1: short pressed right button
// =0: long press
//mode:
//  = 0 to toggle 
//  = 1 to stop
//  = 2 to start
void rbtn_click(bool is_short, int mode)
{
	bool start_rec;
	
	Printf("rbtn_click: sdcard=%d, recording=%d, mode=%d\n",g_sdcard_enable,is_recording,mode);

	if(is_short) { // short press right button
		if(!g_sdcard_enable) return;
		
		pthread_mutex_lock(&dvr_mutex);
		if(mode == 1)
			start_rec = false;
		else if(mode == 2)
			start_rec = true;
		else	
			start_rec = !is_recording;

		if(!start_rec){
			if(is_recording) {
				osd_rec_update(false);
				system(REC_STOP);
				usleep(200000); //200ms
			}
		}
		else{	
			if((!is_recording) && (g_sdcard_size >= 103))  {
				update_record_conf();
				osd_rec_update(true);
				system(REC_START);
				sleep(2); //wait for record process 
			}
		}
		pthread_mutex_unlock(&dvr_mutex);
	}	
	else {//long press right button
		step_topfan();
	}
}

///////////////////////////////////////////////////////////////////////////////
// these are local for OSD controlling
static osd_hdzero_t g_osd_hdzero;
static lv_obj_t *img_arr[HD_VMAX][HD_HMAX];
static lv_obj_t *scr_main; 
static lv_obj_t *scr_osd;
static uint32_t   osdFont[OSD_VNUM][OSD_HNUM][OSD_HEIGHT][OSD_WIDTH] ; //0x00bbggrr
static osd_font_t osd_font;

void osd_rec_update(bool enable)
{
	is_recording = enable;
}

void osd_llock_show(bool bShow)
{
	char buf[128];

	if(!bShow) {
		lv_obj_add_flag(g_osd_hdzero.latency_lock, LV_OBJ_FLAG_HIDDEN);
		return;
	}
	
	if(g_latency_locked) {
		sprintf(buf,"%s%s",RESOURCE_PATH,LLOCK_bmp);
    	lv_img_set_src(g_osd_hdzero.latency_lock, buf);
		lv_obj_clear_flag(g_osd_hdzero.latency_lock, LV_OBJ_FLAG_HIDDEN);
	}
	else 
		lv_obj_add_flag(g_osd_hdzero.latency_lock, LV_OBJ_FLAG_HIDDEN);
}

void osd_rec_show(bool bShow)
{
	char buf[128];

	if(!bShow) {
		lv_obj_add_flag(g_osd_hdzero.sd_rec, LV_OBJ_FLAG_HIDDEN);
		return;
	}
	
	if(!g_sdcard_enable) {
		sprintf(buf,"%s%s",RESOURCE_PATH,noSdcard_bmp);
    	lv_img_set_src(g_osd_hdzero.sd_rec, buf);
		lv_obj_clear_flag(g_osd_hdzero.sd_rec, LV_OBJ_FLAG_HIDDEN);
	}
	else {
		if(is_recording) {
			sprintf(buf,"%s%s",RESOURCE_PATH,recording_bmp);
			lv_img_set_src(g_osd_hdzero.sd_rec, buf);
			lv_obj_clear_flag(g_osd_hdzero.sd_rec, LV_OBJ_FLAG_HIDDEN);
		}
		else
			lv_obj_add_flag(g_osd_hdzero.sd_rec, LV_OBJ_FLAG_HIDDEN);
	}
}

void osd_battery_show()
{
	if(g_setting.power.warning_type == 0) { //Beep only
		lv_obj_add_flag(g_osd_hdzero.battery, LV_OBJ_FLAG_HIDDEN);
		return;
	}

	int vol_sigle = g_battery.voltage/g_battery.type;
	if(vol_sigle  <  g_setting.power.voltage * 100)
		lv_obj_clear_flag(g_osd_hdzero.battery, LV_OBJ_FLAG_HIDDEN);
	else
		lv_obj_add_flag(g_osd_hdzero.battery, LV_OBJ_FLAG_HIDDEN);
}

void osd_topfan_show(bool bShow)
{
	char buf[128];
	if(!bShow) {
		lv_obj_add_flag(g_osd_hdzero.topfan_speed, LV_OBJ_FLAG_HIDDEN);
		return;
	}

	if(g_setting.fans.top_speed>5) return;
	sprintf(buf,"%sfan%d.bmp",RESOURCE_PATH,fan_speeds[2]+1);
    lv_img_set_src(g_osd_hdzero.topfan_speed, buf);
	lv_obj_clear_flag(g_osd_hdzero.topfan_speed, LV_OBJ_FLAG_HIDDEN);
}

void osd_vrxtemp_show(bool bShow)
{
	if(g_temperature.is_rescuing)			
		lv_obj_clear_flag(g_osd_hdzero.vrx_temp, LV_OBJ_FLAG_HIDDEN);
	else
		lv_obj_add_flag(g_osd_hdzero.vrx_temp, LV_OBJ_FLAG_HIDDEN);
}

void osd_vlq_show(bool bShow)
{
	char buf[128];
	if(!bShow) {
		lv_obj_add_flag(g_osd_hdzero.vlq, LV_OBJ_FLAG_HIDDEN);
		return;
	}

	if(link_quality > 8) return;
	if(rx_status[0].rx_valid || rx_status[1].rx_valid) 
		sprintf(buf,"%sVLQ%d.bmp",RESOURCE_PATH,link_quality+1); //VLQ7.bmp
	else 	
		sprintf(buf,"%sVLQ1.bmp",RESOURCE_PATH); //VLQ1.bmp

    lv_img_set_src(g_osd_hdzero.vlq, buf);
	lv_obj_clear_flag(g_osd_hdzero.vlq, LV_OBJ_FLAG_HIDDEN);
}

///////////////////////////////////:////////////////////////////////////////////
// OSD channel
//channel_osd_mode
//  = 0x80 | Channel
//  = 0x00 | Channel Show Time
uint8_t channel_osd_mode; 

char* channel2str(uint8_t channel) //channel=1:10
{
	static char* ChannelName[] = {
		"R1","R2","R3","R4","R5","R6","R7","R8",
		"F2","F4",""
	};

	if((channel>0) && (channel<11))
		return ChannelName[channel-1];
	else 
		return ChannelName[0];
}

void osd_channel_show(bool bShow)
{
	uint8_t ch;
	lv_color_t color;
	char buf[32];

	if(channel_osd_mode &0x80) {
		ch = channel_osd_mode & 0xF;
		color = lv_color_make(0xFF,0x20,0x20);
		sprintf(buf,"  To %s?  ", channel2str(ch));
		lv_obj_set_style_bg_opa(g_osd_hdzero.ch,LV_OPA_100,0);
	}
	else {
		ch = g_setting.scan.channel;
		color = lv_color_make(0xFF,0xFF,0xFF);
		sprintf(buf,"CH:%s", channel2str(ch));
		lv_obj_set_style_bg_opa(g_osd_hdzero.ch,0,0);
	}		

	lv_label_set_text(g_osd_hdzero.ch, buf);
	lv_obj_set_style_text_color(g_osd_hdzero.ch, color, 0);

	if(channel_osd_mode &0x80) 
		lv_obj_clear_flag(g_osd_hdzero.ch, LV_OBJ_FLAG_HIDDEN);
	else if(bShow && channel_osd_mode)
		lv_obj_clear_flag(g_osd_hdzero.ch, LV_OBJ_FLAG_HIDDEN);
	else	
		lv_obj_add_flag(g_osd_hdzero.ch, LV_OBJ_FLAG_HIDDEN);
}

static void create_osd_object(lv_obj_t **obj, const char *img, int index)
{
	int x = 0;

	if((index == 2) || (index == 3)) //GIF format for goggle low battery or goggle high temp
	{
    	*obj = lv_gif_create(scr_osd);
    	lv_gif_set_src(*obj, img);
	}
	else{
    	*obj = lv_img_create(scr_osd);
    	lv_img_set_src(*obj, img);
	}

    lv_obj_set_size(*obj, 36, 36);

	if(index < 5) 
		x = 40*index+160;
	else
		x = 1120 - (11-index)*40;

	lv_obj_set_pos(*obj, x, 0);
}


void osd_show(bool show)
{
	if(show)
		lv_obj_clear_flag(scr_osd, LV_OBJ_FLAG_HIDDEN);
	else	
		lv_obj_add_flag(scr_osd, LV_OBJ_FLAG_HIDDEN);
}


uint8_t RSSI2Ant(uint8_t rssi) 
{
	uint8_t ret,thr[5] = {0x10,0x30,0x50,0x70,0x90};

	if(rssi < thr[0]) ret = 6;
	else if(rssi < thr[1]) ret = 5;
	else if(rssi < thr[2]) ret = 4;
	else if(rssi < thr[3]) ret = 3;
	else if(rssi < thr[4]) ret = 2;
	else ret = 1;

	return ret;
}

void osd_hdzero_update(void)
{
	char buf[128],i;
	bool showRXOSD = g_showRXOSD && (g_source_info.source == 0);
	
	osd_battery_show();
	osd_rec_show(g_showRXOSD);
	osd_llock_show(g_showRXOSD);
	osd_topfan_show(g_showRXOSD);

	osd_vrxtemp_show(g_showRXOSD);

	if(showRXOSD && g_osd_hdzero.vtx_temp)
	{
		if(vtxTempInfo & 0x80) {
			i = vtxTempInfo&0xF;
			if(i==0) i=1;
			else if(i>8) i= 8;
			sprintf(buf,"%sVtxTemp%d",RESOURCE_PATH,i);  //"VtxTemp1.bmp"
		}
		else
			sprintf(buf,"%s%s",RESOURCE_PATH,blank_bmp);
		lv_img_set_src(g_osd_hdzero.vtx_temp, buf);
	}

	if(showRXOSD)
		lv_obj_clear_flag(g_osd_hdzero.vtx_temp, LV_OBJ_FLAG_HIDDEN);
	else		
		lv_obj_add_flag(g_osd_hdzero.vtx_temp, LV_OBJ_FLAG_HIDDEN);

	osd_channel_show(showRXOSD);

	osd_vlq_show(showRXOSD);

	sprintf(buf, "%sant%d.bmp", RESOURCE_PATH,RSSI2Ant(rx_status[0].rx_rssi[0]));
	lv_img_set_src(g_osd_hdzero.ant0, buf);
	sprintf(buf, "%sant%d.bmp", RESOURCE_PATH,RSSI2Ant(rx_status[0].rx_rssi[1]));
	lv_img_set_src(g_osd_hdzero.ant1, buf);
	sprintf(buf, "%sant%d.bmp", RESOURCE_PATH,RSSI2Ant(rx_status[1].rx_rssi[0]));
	lv_img_set_src(g_osd_hdzero.ant2, buf);
	sprintf(buf, "%sant%d.bmp", RESOURCE_PATH,RSSI2Ant(rx_status[1].rx_rssi[1]));
	lv_img_set_src(g_osd_hdzero.ant3, buf);

	if(showRXOSD) {
		lv_obj_clear_flag(g_osd_hdzero.ant0, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(g_osd_hdzero.ant1, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(g_osd_hdzero.ant2, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(g_osd_hdzero.ant3, LV_OBJ_FLAG_HIDDEN);
	}
	else{		
		lv_obj_add_flag(g_osd_hdzero.ant0, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(g_osd_hdzero.ant1, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(g_osd_hdzero.ant2, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(g_osd_hdzero.ant3, LV_OBJ_FLAG_HIDDEN);
	}

	if(g_test_en) {
		sprintf(buf,"T:%d-%d",fan_speeds[2],  g_temperature.top/10);
		lv_label_set_text(g_osd_hdzero.osd_tempe[0], buf);

		sprintf(buf,"L:%d-%d",fan_speeds[1], g_temperature.left/10);
		lv_label_set_text(g_osd_hdzero.osd_tempe[1], buf);

		sprintf(buf,"R:%d-%d",fan_speeds[0], g_temperature.right/10);
		lv_label_set_text(g_osd_hdzero.osd_tempe[2], buf);
	}
}

int draw_osd_clear(void)
{
	clear_screen();
	return 0;
}

int draw_osd_on_screen(uint8_t row, uint8_t col)
{
	pthread_mutex_lock(&lvgl_mutex);
	int index = osd_buf_shadow[row][col];
   	lv_img_set_src(img_arr[row][col], &osd_font.data[index]);
	pthread_mutex_unlock(&lvgl_mutex);

	return 0;
}


static void embedded_osd_init(void)
{
	char buf[128];

	sprintf(buf,"%s%s",RESOURCE_PATH,fan1_bmp);
	create_osd_object(&g_osd_hdzero.topfan_speed, buf, 0);  

	sprintf(buf,"%s%s",RESOURCE_PATH,VtxTemp1_bmp);
	create_osd_object(&g_osd_hdzero.vtx_temp, buf, 1);

	sprintf(buf,"%s%s",RESOURCE_PATH,lowBattery_gif);
	create_osd_object(&g_osd_hdzero.battery,  buf, 2);

	sprintf(buf,"%s%s",RESOURCE_PATH,VrxTemp7_gif);
	create_osd_object(&g_osd_hdzero.vrx_temp, buf, 3);

	sprintf(buf,"%s%s",RESOURCE_PATH,VrxTemp7_gif);
	create_osd_object(&g_osd_hdzero.latency_lock, buf, 4);
		
	g_osd_hdzero.ch = lv_label_create(scr_osd);
    lv_label_set_text(g_osd_hdzero.ch, "CH:-- ");
	lv_obj_set_pos(g_osd_hdzero.ch, 0, 0);
	lv_obj_align(g_osd_hdzero.ch, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_set_style_text_color(g_osd_hdzero.ch, lv_color_make(255,255,255), 0);
	lv_obj_set_style_text_font(g_osd_hdzero.ch, &lv_font_montserrat_26, 0);
	lv_obj_set_style_bg_color(g_osd_hdzero.ch, lv_color_hex(0x010101), LV_PART_MAIN);
	lv_obj_set_style_radius(g_osd_hdzero.ch,50, 0);
	channel_osd_mode = 0;
	
	sprintf(buf,"%s%s",RESOURCE_PATH,noSdcard_bmp);
	create_osd_object(&g_osd_hdzero.sd_rec,  buf, 5);

	sprintf(buf,"%s%s",RESOURCE_PATH,VLQ1_bmp);
	create_osd_object(&g_osd_hdzero.vlq,   buf, 6);

	sprintf(buf,"%s%s",RESOURCE_PATH,ant1_bmp);
	create_osd_object(&g_osd_hdzero.ant0,     buf, 7);
	create_osd_object(&g_osd_hdzero.ant1,     buf, 8);
	create_osd_object(&g_osd_hdzero.ant2,     buf, 9);
	create_osd_object(&g_osd_hdzero.ant3,     buf, 10);
	
	if(g_test_en){
		g_osd_hdzero.osd_tempe[0] = lv_label_create(scr_osd);
		lv_label_set_text(g_osd_hdzero.osd_tempe[0], "TOP:-.- oC");
		lv_obj_set_style_text_color(g_osd_hdzero.osd_tempe[0], lv_color_make(255,255,255), 0);
		lv_obj_set_pos(g_osd_hdzero.osd_tempe[0], 170, 50);
		lv_obj_set_style_text_font(g_osd_hdzero.osd_tempe[0], &lv_font_montserrat_26, 0);

		g_osd_hdzero.osd_tempe[1] = lv_label_create(scr_osd);
		lv_label_set_text(g_osd_hdzero.osd_tempe[1], "LEFT:-.- oC");
		lv_obj_set_style_text_color(g_osd_hdzero.osd_tempe[1], lv_color_make(255,255,255), 0);
		lv_obj_set_pos(g_osd_hdzero.osd_tempe[1], 270, 50);
		lv_obj_set_style_text_font(g_osd_hdzero.osd_tempe[1], &lv_font_montserrat_26, 0);

		g_osd_hdzero.osd_tempe[2] = lv_label_create(scr_osd);
		lv_label_set_text(g_osd_hdzero.osd_tempe[2], "RIGHT:-.- oC");
		lv_obj_set_style_text_color(g_osd_hdzero.osd_tempe[2], lv_color_make(255,255,255), 0);
		lv_obj_set_pos(g_osd_hdzero.osd_tempe[2], 370, 50);
		lv_obj_set_style_text_font(g_osd_hdzero.osd_tempe[2], &lv_font_montserrat_26, 0);
	}
}

static void fc_osd_init(void)
{
	const uint16_t OFFSET_X = 20;
	const uint16_t OFFSET_Y = 40;

	load_fc_osd_font();

	for(int i=0; i<HD_VMAX; i++)
	{
		for(int j=0; j<HD_HMAX; j++)
		{
			pthread_mutex_lock(&lvgl_mutex);
    		img_arr[i][j] = lv_img_create(scr_osd);
    		lv_obj_set_size(img_arr[i][j], OSD_WIDTH, OSD_HEIGHT);
    		lv_obj_set_pos(img_arr[i][j], j*OSD_WIDTH+OFFSET_X, i*OSD_HEIGHT + OFFSET_Y);
			pthread_mutex_unlock(&lvgl_mutex);
		}
	}
}

static void create_osd_scr(void)
{
	scr_main = lv_scr_act();
	scr_osd = lv_obj_create(scr_main);
	lv_obj_clear_flag(scr_main, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_clear_flag(scr_osd, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_size(scr_osd, 1280, 720);
	lv_obj_add_flag(scr_osd, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_style(scr_osd, &style_osd, 0);

}

int osd_init(void)
{
	create_osd_scr();

	fc_osd_init();
	embedded_osd_init();

	return 0;
}
///////////////////////////////////////////////////////////////////////////////
// load_osd_file
// load_fc_osd_font
int load_fc_osd_font_bmp(const char *file)
{
	char *buf;
	struct stat stFile;
	int fd,size,rd;
	int boundry_width;
	int line_size;

	Printf("load_fc_osd_font_bmp: %s...", file);
	fd = open(file, O_RDONLY);
	if(fd < 0) return -1;
	
	fstat(fd, &stFile);
	size = stFile.st_size;
	buf = (char *)malloc(size);
	if(!buf) return -2;
		
	rd = read(fd, buf, size);
	if(rd != size) return -3;

	close(fd);

	bmpFileHead *bmp = (bmpFileHead *)buf;
	char *pb = buf + sizeof(bmpFileHead) + bmp->info.biClrUsed;

	if(bmp->info.biWidth == (OSD_HNUM * OSD_WIDTH))
	{
		//no boundry
		boundry_width = OSD_BOUNDRY_0;
		line_size = LINE_LENGTH_0;
	}
	else
	{
		//have boundry
		boundry_width = OSD_BOUNDRY_1;
		line_size = LINE_LENGTH_1;
	}

    // read OSD font
    uint8_t h, v;
    uint8_t x, y;
    uint32_t addr;
    uint8_t offset;
    
    for (v = 0; v < OSD_VNUM; v++)
    {
        for (h = 0; h < OSD_HNUM; h++)
        {
            // calc vertical and horizontal black boundry.
            addr = (v + 1) * boundry_width * line_size + (h + 1) * boundry_width * 3;
            // calc size that have read.
            addr += v * OSD_HEIGHT * line_size;
			addr += h*OSD_WIDTH*3;

            for (y = 0; y < OSD_HEIGHT; y++)
            {
                for (x = 0; x < OSD_WIDTH; x++)
                {
                    osdFont[OSD_VNUM - v - 1][h][OSD_HEIGHT - y - 1][x] = (0xff<<24) + ((pb[addr + x * 3] & 0xff) ) + ((pb[addr + x * 3 + 1] & 0xff) << 8) + ((pb[addr + x * 3 + 2] & 0xff)<<16);
                }
                addr += line_size;
            }
        }
    }
    for (v = 0; v < OSD_VNUM; v++)
    {
        for (h = 0; h < OSD_HNUM; h++)
        {
			int index = v*OSD_HNUM + h;
			osd_font.data[index].header.cf= LV_IMG_CF_TRUE_COLOR;
			osd_font.data[index].header.always_zero = 0;
			osd_font.data[index].header.reserved = 0;
			osd_font.data[index].header.w = 24;
			osd_font.data[index].header.h = 36;
			osd_font.data[index].data_size = 24 * 36 * LV_COLOR_SIZE / 8;
			osd_font.data[index].data = (uint8_t *)&osdFont[v][h][0][0];  
		}
	}
	//free(buf); //FIX ME, ntant, it seems system becomes unstable if uncomment this ???
	return 0;
}

void load_fc_osd_font(void)
{
	char fp[3][256];
	int i;

	sprintf(fp[0], "%s%s_000.bmp", FC_OSD_SDCARD_PATH, fc_variant);
	sprintf(fp[1], "%s%s_000.bmp", FC_OSD_LOCAL_PATH, fc_variant);
	sprintf(fp[2], "%sBTFL_000.bmp", FC_OSD_LOCAL_PATH);

	for(i=0;i<3;i++)
	{
		if(!load_fc_osd_font_bmp(fp[i]))
		{
			Printf(" succecss!\n");
			return;
		}
		else
			Printf(" failed!\n");
	}
}
///////////////////////////////////////////////////////////////////////////////
// Threads for updating FC OSD
void *thread_osd(void *ptr)
{

	for(;;)
	{

		for(int i=0; i<HD_VMAX; i++)
		{
			for(int j=0; j<HD_HMAX; j++)
			{
				if(osd_buf[i][j] != osd_buf_shadow[i][j]) 
				{
					osd_buf_shadow[i][j] =osd_buf[i][j];
					draw_osd_on_screen(i, j);
				}
			}

		}
	//	usleep(2000); 
	}
return NULL;
}

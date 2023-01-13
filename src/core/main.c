#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#include <lvgl/lvgl.h>
#include <minIni.h>

#include "../bmi270/accel_gyro.h"
#include "../driver/dm5680.h"
#include "../driver/esp32.h"
#include "../driver/fans.h"
#include "../driver/hardware.h"
#include "../driver/i2c.h"
#include "../driver/it66021.h"
#include "../driver/it66121.h"
#include "../driver/mcp3021.h"
#include "../driver/oled.h"
#include "../driver/TP2825.h"
#include "common.hh"
#include "elrs.h"
#include "ht.h"
#include "input_device.h"
#include "osd.h"
#include "self_test.h"
#include "thread.h"
#include "thread.h"
#include "ui/page_power.h"
#include "ui/page_scannow.h"
#include "ui/page_source.h"
#include "ui/ui_image_setting.h"
#include "ui/ui_main_menu.h"
#include "ui/ui_porting.h"
#include "ui/ui_statusbar.h"

static void load_ini_setting(void)
{
	char str[128];
	
	FILE* fp;
	fp=fopen("/mnt/UDISK/setting.ini","r");
	if(fp){
		fclose(fp);
		sprintf(str,"cp -f /mnt/UDISK/setting.ini %s", SETTING_INI);
		system(str); usleep(10);
		system("rm /mnt/UDISK/setting.ini");
	}

  	ini_gets("scan", "channel", "1", str, sizeof(str), SETTING_INI);
	g_setting.scan.channel = atoi(str);


  	ini_gets("fans", "auto", "enable", str, sizeof(str), SETTING_INI);
	if(strcmp(str, "enable") == 0)	
		g_setting.fans.auto_mode = true;
	else
		g_setting.fans.auto_mode = false;
  	ini_gets("fans", "top_speed", "4", str, sizeof(str), SETTING_INI);
	g_setting.fans.top_speed = atoi(str);
  	ini_gets("fans", "left_speed", "5", str, sizeof(str), SETTING_INI);
	g_setting.fans.left_speed = atoi(str);
  	ini_gets("fans", "right_speed", "5", str, sizeof(str), SETTING_INI);
	g_setting.fans.right_speed = atoi(str);

  	ini_gets("autoscan", "status", "enable", str, sizeof(str), SETTING_INI);
	if(strcmp(str, "enable") == 0 || strcmp(str, "scan") == 0)
	{
		g_setting.autoscan.status = 0;
	}else if(strcmp(str, "disable") == 0 || strcmp(str, "last") == 0){
		g_setting.autoscan.status = 1;
	}else{
		g_setting.autoscan.status = 2;
	}
	g_setting.autoscan.source = ini_getl("autoscan", "source", SETTING_SOURCE_LAST, SETTING_INI);
	g_setting.autoscan.last_source = ini_getl("autoscan", "last_source", SETTING_SOURCE_HDZERO, SETTING_INI);

	//power
  	ini_gets("power", "voltage", "35", str, sizeof(str), SETTING_INI);
	g_setting.power.voltage = atoi(str);
  	ini_gets("power", "warning_type", "0", str, sizeof(str), SETTING_INI);
	g_setting.power.warning_type = atoi(str);
  	ini_gets("record", "mode_manual", "disable", str, sizeof(str), SETTING_INI);
	if(strcmp(str, "enable") == 0)	
	{
		g_setting.record.mode_manual = true;
	}else{
		g_setting.record.mode_manual = false;
	}
  	ini_gets("record", "format_ts", "enable", str, sizeof(str), SETTING_INI);
	if(strcmp(str, "enable") == 0)	
	{
		g_setting.record.format_ts = true;
	}else{
		g_setting.record.format_ts = false;
	}
  	ini_gets("record", "osd", "enable", str, sizeof(str), SETTING_INI);
	if(strcmp(str, "enable") == 0)	
	{
		g_setting.record.osd = true;
	}else{
		g_setting.record.osd = false;
	}
  	ini_gets("record", "audio", "enable", str, sizeof(str), SETTING_INI);
	if(strcmp(str, "enable") == 0)	
	{
		g_setting.record.audio = true;
	}else{
		g_setting.record.audio = false;
	}

  	ini_gets("record", "audio_source", "0", str, sizeof(str), SETTING_INI);
	g_setting.record.audio_source = atoi(str);

		//image
  	ini_gets("image", "oled", "7", str, sizeof(str), SETTING_INI);
	g_setting.image.oled = atoi(str);
  	ini_gets("image", "brightness", "0", str, sizeof(str), SETTING_INI);
	g_setting.image.brightness = atoi(str);
  	ini_gets("image", "saturation", "0", str, sizeof(str), SETTING_INI);
	g_setting.image.saturation = atoi(str);
  	ini_gets("image", "contrast", "0", str, sizeof(str), SETTING_INI);
	g_setting.image.contrast = atoi(str);
  	ini_gets("image", "auto_off", "2", str, sizeof(str), SETTING_INI);
	g_setting.image.auto_off = atoi(str);

	g_setting.ht.enable = ini_getl("ht", "enable", 0, SETTING_INI);
	if(!g_setting.ht.enable) 
			disable_ht();

	g_setting.elrs.enable = ini_getl("elrs", "enable", 0, SETTING_INI);

	//Check
    g_test_en = false;
    log_file = fopen(LOG_FILE,"r");
    if(log_file) {
		fclose(log_file);
		log_file = fopen(LOG_FILE,"w+");
    	if(log_file) g_test_en = true;
	}
}

static void *thread_autoscan(void *ptr)
{
	for(;;)
	{
		pthread_mutex_lock(&lvgl_mutex);
		main_menu_show(true);
		g_menu_op = OPLEVEL_SUBMENU;
		submenu_enter();
		pthread_mutex_unlock(&lvgl_mutex);

		if(g_autoscan_exit)
			goto a_exit;

		sleep(5);

		if(g_autoscan_exit)
			goto a_exit;
	}
	
a_exit:
	pthread_exit(NULL);
return NULL;
}


void start_running(void)
{
	int source;
	if(g_setting.autoscan.source == SETTING_SOURCE_LAST)
		source = g_setting.autoscan.last_source;
	else
		source = g_setting.autoscan.source;

	if(source == SETTING_SOURCE_HDZERO) {//HDZero
		g_source_info.source = SOURCE_HDZERO;
		HDZero_open();
		if(g_setting.autoscan.status == SETTING_AUTOSCAN_SCAN) {
			pthread_t pid;
			g_autoscan_exit = false;
			pthread_create(&pid,NULL,thread_autoscan,NULL);
		}
		else if(g_setting.autoscan.status == SETTING_AUTOSCAN_LAST) {
			g_menu_op = OPLEVEL_VIDEO;
			switch_to_video(true);
		}
		else{ //auto scan disabled, go to go directly to last saved channel
			g_menu_op = OPLEVEL_MAINMENU;
		}
	}
	else {
		g_menu_op = OPLEVEL_VIDEO;
		if(source == SETTING_SOURCE_EXPANSION) {//module Bay
			switch_to_analog(1);
			g_source_info.source = SOURCE_EXPANSION;
		}
		else if(source == SETTING_SOURCE_AV_IN) {//AV in
			switch_to_analog(0);
			g_source_info.source = SOURCE_AV_IN;
		}
		else { //HDMI in
			sleep(2);
			g_source_info.hdmi_in_status = IT66021_Sig_det();
			if(g_source_info.hdmi_in_status) {
				switch_to_hdmiin();
				g_source_info.source = SOURCE_HDMI_IN;
			}
			else {
				g_source_info.source = SOURCE_HDZERO;
				g_menu_op = OPLEVEL_MAINMENU;
			}
		}
	}

	set_voltage(g_setting.power.voltage);
	set_warning_type(g_setting.power.warning_type);

	if (g_setting.elrs.enable)
		enable_esp32();
}

static void device_init(void)
{
	self_test();
	enable_bmi270();
	IT66021_init();
	IT66121_init();
	TP2825_Config(0, 0);
	g_battery.type = 2;
	DM5680_req_ver(); 
	fans_top_setspeed(g_setting.fans.top_speed);
}


void lvgl_init()
{
	lv_init();
	style_init();
	lvgl_init_porting();
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_color_make(0xff, 0xff, 0xff), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(64, 64, 64), 0);
}

int main(int argc, char* argv[])
{
	pthread_mutex_init(&lvgl_mutex, NULL);

	gpio_init();
	lvgl_init();
	load_ini_setting();
	main_menu_init();
	statusbar_init();
	lv_timer_handler();
	input_device_init(); 
	
	iic_init();
	OLED_Startup();
	Display_UI_init();
	OLED_Pattern(0, 0, 0);
	
	uart_init();
	hw_stat_init();
	device_init();

	osd_init(); 
	ims_init();

	esp32_init();
	elrs_init();

	start_running(); //start to run from saved settings
	create_threads();
	g_init_done = 1;
	for(;;)
	{
		pthread_mutex_lock(&lvgl_mutex);
		statubar_update();
		osd_hdzero_update();
		ims_update();
		lv_timer_handler();
		source_status_timer();
		pthread_mutex_unlock(&lvgl_mutex);
		usleep(5000); 
	}
	return 0;
}

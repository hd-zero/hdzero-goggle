#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <sys/epoll.h>
#include <assert.h>
#include <errno.h>
#include "defines.h"
#include "input_device.h"
#include "porting.h"
#include <pthread.h>

#include "../minIni/minIni.h"

#include "main_menu.h"
#include "osd.h"
#include "imagesetting.h"
#include "common.hh"
#include "../page/page_scannow.h"
#include "../page/page_common.h"
#include "../page/page_fans.h"
#include "../page/page_power.h"
#include "../page/page_imagesettings.h"
#include "../page/page_playback.h"
#include "../page/page_source.h"

#include "../driver/oled.h"
#include "../driver/dm6302.h"
#include "../driver/hardware.h"
#include "../driver/i2c.h"
#include "../driver/uart.h"

///////////////////////////////////////////////////////////////////////////////
// Tune channel on video mode
#define TUNER_TIMER_LEN 	30
uint8_t    tune_state = 0; //0=init; 1=waiting for key; 2=tuning 
uint16_t   tune_timer = 0;


//action: 1 = tune up, 2 = tune down, 3 = confirm
void exit_tune_channel()
{
	tune_state = 0;
	tune_timer = 0;
	channel_osd_mode = 0;
}

void tune_channel(uint8_t action)
{
	static uint8_t channel = 0;
	
	Printf("tune_channel:%d\n",action);

	if(tune_state == 0) {
		channel_osd_mode = 0;
		tune_state = 1;
	}

	if(tune_state == 1) {
		if((action == DIAL_KEY_UP) || (action == DIAL_KEY_DOWN)) {
			tune_timer = TUNER_TIMER_LEN;
			tune_state = 2;
			channel = g_setting.scan.channel;
		}
		else if(action == DIAL_KEY_CLICK) {
			g_showRXOSD = !g_showRXOSD;
			if(g_showRXOSD)
				channel_osd_mode = CHANNEL_SHOWTIME;
		}
	}

	if(tune_state != 2) return;

	switch(action) {
		case DIAL_KEY_UP:  //Tune up
			if(channel == 10)
				channel = 1;
			else
				channel++;	
			break;

		case DIAL_KEY_DOWN:  //Tune down
			if(channel == 1)
				channel = 10;
			else
				channel--;	
			break;

		case DIAL_KEY_CLICK: //confirm to tune 
			if(	g_setting.scan.channel != channel) {
				g_setting.scan.channel = channel;
				ini_putl("scan", "channel", g_setting.scan.channel, SETTING_INI);
				rbtn_click(1,1); //stop recording
				switch_to_video(true);
			}
			tune_timer = 0;
			tune_state = 1;
			channel_osd_mode = CHANNEL_SHOWTIME;
			return;

		default:	
			perror("TuneChannel: bad command");
			break;
	}
	channel_osd_mode = 0x80|channel;
	tune_timer = TUNER_TIMER_LEN;
}

void tune_channel_timer()
{
	if(tune_state == 2) {
		if(!tune_timer) return;

		if(tune_timer == 1) {
			tune_state = 1;
			channel_osd_mode = CHANNEL_SHOWTIME;
		}
		tune_timer--;
		//Printf("tune_channel_timer:%d\n",tune_timer);
	}
	else {
		if(channel_osd_mode)
			channel_osd_mode--;
	}
}


///////////////////////////////////////////////////////////////////////////////
extern pthread_mutex_t lvgl_mutex;

static void switch_to_menumode()
{
	if(g_menu_op == OPLEVEL_IMS) {
		save_ims();
		set_slider_value();
	}

	g_menu_op = OPLEVEL_MAINMENU;
	rbtn_click(true, 1); //Stop recording if switching to menu mode from video mode regardless 

	Display_UI();
	lvgl_switch_to_1080p();
	exit_tune_channel();
	osd_show(false); 
	g_bShowIMS = false;
	main_menu_show(true);
	HDZero_Close(); 
	g_sdcard_det_req = 1;
}

static void btn_press(void) //long press left key
{
	Printf("btn_press (%d)\n",g_menu_op);
	if(g_scanning || !g_init_done) 	return;

	pthread_mutex_lock(&lvgl_mutex);

	g_autoscan_exit = true;
	if(g_menu_op == OPLEVEL_MAINMENU) //Main menu -> Video
	{
		if(g_source_info.source == 0) { //HDZero
			progress_bar.start  = 1;
			HDZero_open();
			switch_to_video(true);
		}
		else if(g_source_info.source == 1) //HDMI
			Source_HDMI_in();
		else if(g_source_info.source == 2) //AV in
			switch_to_analog(0);    
		else							//Expansion Module
			switch_to_analog(1);  

		g_menu_op = OPLEVEL_VIDEO;			
	}
	else if((g_menu_op == OPLEVEL_VIDEO) || (g_menu_op == OPLEVEL_IMS)) //video -> Main menu
		switch_to_menumode();
	else if(g_menu_op == OPLEVEL_PLAYBACK)
		pb_key(DIAL_KEY_PRESS);
	else { //Sub-menu  -> Main menu		
		submenu_exit();
		g_menu_op = OPLEVEL_MAINMENU;
		main_menu_show(true);
	}
	pthread_mutex_unlock(&lvgl_mutex);
}

static void btn_click(void)  //short press enter key
{
	Printf("btn_click (%d)\n",g_menu_op);
	if(!g_init_done) return;

	if(g_menu_op == OPLEVEL_VIDEO) {
		pthread_mutex_lock(&lvgl_mutex);
		tune_channel(DIAL_KEY_CLICK);
		pthread_mutex_unlock(&lvgl_mutex);
		return;
	}
	else if(g_menu_op == OPLEVEL_IMS) {
		pthread_mutex_lock(&lvgl_mutex);
		if(ims_key(DIAL_KEY_CLICK)) 
			switch_to_menumode();
		pthread_mutex_unlock(&lvgl_mutex);
		return;
	}
	
	if(!main_menu_isshow()) return;

	if(g_scanning) 	return;

	pthread_mutex_lock(&lvgl_mutex);
	
	autoscan_exit();
	if(g_menu_op == OPLEVEL_MAINMENU)
	{ 
		Printf("level = 1\n");
		g_menu_op = OPLEVEL_SUBMENU;
		submenu_enter();
	}
	else if((g_menu_op == OPLEVEL_SUBMENU) ||(g_menu_op == OPLEVEL_PLAYBACK))
	{ 
		submenu_fun();	
	}
	else if(g_menu_op == PAGE_FAN_SLIDE)
	{ 
		submenu_fun();	
	}
	else if(g_menu_op == PAGE_POWER_SLIDE)
	{ 
		submenu_fun();	
	}
	pthread_mutex_unlock(&lvgl_mutex);
}

static void roller_up(void)
{
	Printf("roller up (%d)\n",g_menu_op);

	if(g_scanning) 	return;

	pthread_mutex_lock(&lvgl_mutex);
	autoscan_exit();
	if(g_menu_op == OPLEVEL_MAINMENU) //main menu
	{
		menu_nav(DIAL_KEY_UP);
	}
	else if((g_menu_op == OPLEVEL_SUBMENU) ||(g_menu_op == OPLEVEL_PLAYBACK))
	{
		submenu_nav(DIAL_KEY_UP);
	}
	else if(g_menu_op == OPLEVEL_VIDEO)
	{
		if(g_source_info.source == 0) tune_channel(DIAL_KEY_UP);
	}
	else if(g_menu_op == OPLEVEL_IMS)
	{
		ims_key(DIAL_KEY_UP);
	}
	else if(g_menu_op == PAGE_FAN_SLIDE)
	{
		fans_speed_dec();
	}
	else if(g_menu_op == PAGE_POWER_SLIDE)
	{
		power_voltage_dec();
	}
	pthread_mutex_unlock(&lvgl_mutex);
}

static void roller_down(void)
{
	Printf("roller down (%d)\n",g_menu_op);

	if(g_scanning)
		return;

	pthread_mutex_lock(&lvgl_mutex);
	autoscan_exit();
	if(g_menu_op == OPLEVEL_MAINMENU)
	{
		menu_nav(DIAL_KEY_DOWN);
	}
	else if((g_menu_op == OPLEVEL_SUBMENU) ||(g_menu_op == OPLEVEL_PLAYBACK))
	{
		submenu_nav(DIAL_KEY_DOWN);
	}
	else if(g_menu_op == OPLEVEL_VIDEO)
	{
		if(g_source_info.source == 0) tune_channel(DIAL_KEY_DOWN);
	}
	else if(g_menu_op == OPLEVEL_IMS)
	{
		ims_key(DIAL_KEY_DOWN);
	}
	else if(g_menu_op == PAGE_FAN_SLIDE)
	{
		fans_speed_inc();
	}
	else if(g_menu_op == PAGE_POWER_SLIDE)
	{
		power_voltage_inc();
	}

	pthread_mutex_unlock(&lvgl_mutex);
}

static void get_event(int fd)
{
    struct input_event event;
	static int roller_value = 0;
	static int event_type_last = 0;
	static int btn_value = 0;
	static int btn_press_time = 0;

    read(fd, &event, sizeof(event));

    switch (event.type) {
        case EV_SYN:
            if (event.code == SYN_REPORT) {

				if(event_type_last == EV_REL)
				{
					if(roller_value == 1)
					{
						roller_up();
						g_key = DIAL_KEY_UP;
					}else{
						roller_down();
						g_key = DIAL_KEY_DOWN;
					}
				}
				else if(event_type_last == EV_KEY)
				{
					if(btn_value)
					{
						if(btn_press_time == 10)
						{
							btn_press();
							g_key = DIAL_KEY_PRESS;
						}
						btn_press_time++;
						//printf("btn down\n");
					}else{
						if(btn_press_time < 10){
							btn_click();
							g_key = DIAL_KEY_CLICK;
						}
						//else if(btn_press_time > 200){
						//	btn_super_press();
						//}
						btn_press_time = 0;
					}
				}
				else
				{

				}
                //printf("------------ syn report ----------\n");
            } else if (event.code == SYN_MT_REPORT) {
                //printf("----------- syn mt report ------------\n");
            }
            break;
        case EV_KEY:
            //printf("key code%d is %s!\n", event.code, event.value?"down":"up");
			btn_value = event.value;
			event_type_last = EV_KEY;
            break;
        case EV_ABS:
            if ((event.code == ABS_X) ||
                 (event.code == ABS_MT_POSITION_X)) {
                //printf("abs,x = %d\n", event.value);
            } else if ((event.code == ABS_Y) ||
                 (event.code == ABS_MT_POSITION_Y)) {
                //printf("abs,y = %d\n", event.value);
            } else if ((event.code == ABS_PRESSURE) ||
                (event.code == ABS_MT_PRESSURE)) {
                //printf("pressure value: %d\n", event.value);
            }
            break;
        case EV_REL:
            if (event.code == REL_X) {
                //printf("x = %d\n", event.value);
            } else if (event.code == REL_Y) {
				roller_value = event.value;
                //printf("y = %d\n", event.value);
            }
			event_type_last = EV_REL;
            break;
        default:
            //printf("unknown [type=%d, code=%d value=%d]\n", event.type, event.code, event.value);
            break;
    }
}

static void add_to_epfd(int epfd, int fd)
{
    int ret;
    struct epoll_event event = {
        .events = EPOLLIN,
        .data    = {
            .fd = fd,
        },
    };

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    assert(ret == 0);
}

#define ID_LEN 64
#define ID_CNT 4
static int epfd;
static struct epoll_event events[ID_CNT];
int input_device_open(void)
{
    char buf[ID_LEN];
    int fd;
    int i;
    int nFrom=0, nCount=ID_CNT;
    epfd = epoll_create(ID_CNT);
    assert(epfd > 0);

    for (i = nFrom; i < nFrom+nCount; i++) {
        snprintf(buf, ID_LEN, "/dev/input/event%d", i);
        fd = open(buf, O_RDONLY);
        if (fd >= 0) {
            add_to_epfd(epfd, fd);
            printf("opened %s\n", buf);
        }
    }
	return 0;
}

void input_device_loop(void)
{
	int ret = epoll_wait(epfd, events, ID_CNT, -1);
	if (ret < 0) {
		perror("epoll_wait");
		return;
	}

	for (int i = 0; i < ret; i++) {
		if (events[i].events&EPOLLIN) {
			get_event(events[i].data.fd);
		}
	}
}

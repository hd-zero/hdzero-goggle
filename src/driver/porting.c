#include "porting.h"

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include <log/log.h>

#include "fbtools.h"
#include "lvgl/lvgl.h"
#include "../core/common.hh"

#define DISP_HOR_RES (1920)
#define DISP_VER_RES (1080)

static int h_resolution =DISP_HOR_RES; 

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[DISP_HOR_RES * DISP_VER_RES  * 4 ];                        /*Declare a buffer for 1/10 screen size*/
static lv_disp_drv_t disp_drv;        /*Descriptor of a display driver*/

FBDEV fbdev;

static void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    int32_t  y;
    /*It's a very slow but simple implementation.
     *`set_pixel` needs to be written by you to a set pixel on the screen*/
    for(y = area->y1; y <= area->y2; y++) {
//        for(x = area->x1; x <= area->x2; x++) {
			fbdev.fb_mem_offset = (y*h_resolution+area->x1)*4;
			fbdev.fb_fix.smem_len = 4 * (area->x2 - area->x1+1);
			memcpy(fbdev.fb_mem + fbdev.fb_mem_offset, ((char *)color_p), fbdev.fb_fix.smem_len);
            color_p+=(area->x2 - area->x1 + 1);
 //       }
    }

    fb_sync(&fbdev);
//    fb_close(&fbdev);
    lv_disp_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}

static lv_disp_t *disp;
int lvgl_init_porting(void)
{
    memset(&fbdev, 0x0, sizeof(FBDEV));
    strncpy(fbdev.dev, "/dev/fb0", sizeof(fbdev.dev));
    if(fb_open(&fbdev)==-1)
    {
        LOGI("open frame buffer error");
        return -1;
    }
	LOGI("register disp drv");
//ע����ʾ����
	lv_disp_draw_buf_init(&draw_buf, buf1, NULL, DISP_HOR_RES * DISP_VER_RES * 4);  /*Initialize the display buffer.*/

	lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
	disp_drv.flush_cb = my_disp_flush;    /*Set your driver function*/
	disp_drv.draw_buf = &draw_buf;        /*Assign the buffer to the display*/
	disp_drv.hor_res = DISP_HOR_RES;   /*Set the horizontal resolution of the display*/
	disp_drv.ver_res = DISP_VER_RES;   /*Set the vertical resolution of the display*/
	disp = lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/

#if 0
//ע�������豸����
	lv_indev_drv_init(&indev_drv);             /*Basic initialization*/
	indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
	indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
	lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

	lv_indev_drv_init(&indev_button_drv);             /*Basic initialization*/
	indev_button_drv.type = LV_INDEV_TYPE_BUTTON;    /*Touch pad is a pointer-like device*/
	indev_button_drv.read_cb = button_read;      /*Set your driver function*/
	lv_indev_t *indev_button = lv_indev_drv_register(&indev_button_drv);         /*Finally register the driver*/
	lv_indev_set_button_points(indev_button, points_array);
#endif
	return 0;
}



int lvgl_switch_to_720p(void)
{
	lv_disp_draw_buf_init(&draw_buf, buf1, NULL, 1280* 720* 4);  /*Initialize the display buffer.*/
	disp_drv.draw_buf = &draw_buf;        /*Assign the buffer to the display*/

	disp_drv.hor_res = 1280;   /*Set the horizontal resolution of the display*/
	disp_drv.ver_res = 720;   /*Set the vertical resolution of the display*/
	lv_disp_drv_update(disp, &disp_drv);      /*Finally register the driver*/

	h_resolution = 1280;
	return 0;
}


int lvgl_switch_to_1080p(void)
{
	lv_disp_draw_buf_init(&draw_buf, buf1, NULL, 1920* 1080* 4);  /*Initialize the display buffer.*/
	disp_drv.draw_buf = &draw_buf;        /*Assign the buffer to the display*/

	disp_drv.hor_res = 1920;   /*Set the horizontal resolution of the display*/
	disp_drv.ver_res = 1080;   /*Set the vertical resolution of the display*/
	lv_disp_drv_update(disp, &disp_drv);      /*Finally register the driver*/

	h_resolution = 1920;
	return 0;
}



/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t wk_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

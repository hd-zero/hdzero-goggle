#include "ui/ui_porting.h"

#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include <log/log.h>

#include "../core/common.hh"
#include "fbtools.h"
#include "lvgl/lvgl.h"

#define DISP_HOR_RES (1920)
#define DISP_VER_RES (1080)

FBDEV fbdev;

static int h_resolution = DISP_HOR_RES;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_buf[DISP_HOR_RES * DISP_VER_RES * 4];
static lv_disp_drv_t disp_drv;
static lv_disp_t *disp;

static void hdz_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    for (lv_coord_t y = area->y1; y <= area->y2; y++) {
        fbdev.fb_mem_offset = (y * h_resolution + area->x1) * 4;
        fbdev.fb_fix.smem_len = 4 * (area->x2 - area->x1 + 1);
        memcpy(fbdev.fb_mem + fbdev.fb_mem_offset, ((char *)color_p), fbdev.fb_fix.smem_len);
        color_p += (area->x2 - area->x1 + 1);
    }

    fb_sync(&fbdev);

    lv_disp_flush_ready(disp);
}

int lvgl_init_porting(void) {
    memset(&fbdev, 0x0, sizeof(FBDEV));
    strncpy(fbdev.dev, "/dev/fb0", sizeof(fbdev.dev));
    if (fb_open(&fbdev) == -1) {
        LOGE("open frame buffer error");
        return -1;
    }
    LOGI("register disp drv");

    lv_disp_draw_buf_init(&draw_buf, disp_buf, NULL, DISP_HOR_RES * DISP_VER_RES * 4);

    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = hdz_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = DISP_HOR_RES;
    disp_drv.ver_res = DISP_VER_RES;
    disp = lv_disp_drv_register(&disp_drv);

    return 0;
}

int lvgl_switch_to_720p(void) {
    lv_disp_draw_buf_init(&draw_buf, disp_buf, NULL, 1280 * 720 * 4);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = 1280;
    disp_drv.ver_res = 720;
    lv_disp_drv_update(disp, &disp_drv);

    h_resolution = 1280;
    return 0;
}

int lvgl_switch_to_1080p(void) {
    lv_disp_draw_buf_init(&draw_buf, disp_buf, NULL, 1920 * 1080 * 4);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = 1920;
    disp_drv.ver_res = 1080;
    lv_disp_drv_update(disp, &disp_drv);

    h_resolution = 1920;
    return 0;
}

uint32_t wk_tick_get(void) {
    static uint64_t start_ms = 0;
    if (start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);

    const uint64_t now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;
    return now_ms - start_ms;
}

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define DISP_OVERSCAN    8
#define DISP_HOR_RES_HD  1280
#define DISP_VER_RES_HD  720
#define DRAW_HOR_RES_HD  (DISP_HOR_RES_HD + DISP_OVERSCAN)
#define DRAW_VER_RES_HD  (DISP_VER_RES_HD + DISP_OVERSCAN)
#define DISP_HOR_RES_FHD 1920
#define DISP_VER_RES_FHD 1080
#define DRAW_HOR_RES_FHD (DISP_HOR_RES_FHD + DISP_OVERSCAN)
#define DRAW_VER_RES_FHD (DISP_VER_RES_FHD + DISP_OVERSCAN)

int lvgl_init_porting();
int lvgl_switch_to_720p();
int lvgl_switch_to_1080p();
void lvgl_screen_orbit(bool enable);

#ifdef __cplusplus
}
#endif

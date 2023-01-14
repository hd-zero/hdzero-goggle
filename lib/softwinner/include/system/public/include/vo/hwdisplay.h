/*
**
** Copyright (C) 2008, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifndef _HWDISPLAY_H
#define _HWDISPLAY_H
//#include <utils/threads.h>
#include <semaphore.h>
#include <stdbool.h>
#include <pthread.h>

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

#include <video/sunxi_display2.h>

typedef struct disp_layer_config disp_layer_config;
typedef enum disp_output_type disp_output_type;
typedef enum disp_tv_mode disp_tv_mode;
typedef enum tag_DISP_CMD __DISP_t;
typedef enum disp_color_space disp_color_space;
typedef struct disp_rect disp_rect;
typedef struct disp_capture_info disp_capture_info;
typedef struct disp_output disp_output;

#define CHN_NUM 4

#define LYL_NUM 4
#define LAYER_DE 16
#define HLAY(chn, lyl) (chn*4+lyl)
#define HD2CHN(hlay) (hlay/4)
#define HD2LYL(hlay) (hlay%4)

#define UI_LYCHN 2
#define UI_LYLAY 0

#define SCREEN_NUM 2

#define ZORDER_MAX 16
#define ZORDER_MIN 0

//#include <include_media/media/hwdisp_def.h>

/*****************************************************************************/

#define DISP_DEV "/dev/disp"
#define FB_ANDROID_DEV   "/dev/graphics/fb%d"

#define SCREEN_0 0
#define SCREEN_1 1
#define RET_OK 0
#define RET_FAIL -1
#define CK_COLOR 0x000000
#define ID_RESERVED 3

#define HDL2ID(handle)  ((handle) - 101)
#define ID2HDL(ID)  ((ID) + 101)

#define VALID_LAYER 101

/*****************************************************************************/
struct view_info
{
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h; 
};

typedef struct
{
    unsigned long   number;
    
    unsigned long   top_y;              // the address of frame buffer, which contains top field luminance
    unsigned long   top_c;              // the address of frame buffer, which contains top field chrominance
    unsigned long   bottom_y;           // the address of frame buffer, which contains bottom field luminance
    unsigned long   bottom_c;           // the address of frame buffer, which contains bottom field chrominance

    signed char     bProgressiveSrc;    // Indicating the source is progressive or not
    signed char     bTopFieldFirst;     // VPO should check this flag when bProgressiveSrc is FALSE
    unsigned long   flag_addr;          //dit maf flag address
    unsigned long   flag_stride;        //dit maf flag line stride
    unsigned char   maf_valid;
    unsigned char   pre_frame_valid;
    unsigned int    handle;
}libhwclayerpara_t;

enum
{
    HWD_STATUS_REQUESTED    = 1,
    HWD_STATUS_NOTUSED      = 2,
    //HWD_STATUS_OPENED       = 4
};

enum e_hwc_format
{
    HWC_FORMAT_MINVALUE     = 0x50,
    HWC_FORMAT_RGBA_8888    = 0x51,
    HWC_FORMAT_RGB_565      = 0x52,
    HWC_FORMAT_BGRA_8888    = 0x53,
    HWC_FORMAT_YCbYCr_422_I = 0x54,
    HWC_FORMAT_CbYCrY_422_I = 0x55,
    HWC_FORMAT_MBYUV420     = 0x56,
    HWC_FORMAT_MBYUV422     = 0x57,
    HWC_FORMAT_YUV420PLANAR = 0x58,
    HWC_FORMAT_YUV411PLANAR     = 0x59,
    HWC_FORMAT_YUV422PLANAR     = 0x60,
    HWC_FORMAT_YUV444PLANAR     = 0x61,
    HWC_FORMAT_YUV420UVC        = 0x62,
    HWC_FORMAT_YUV420VUC        = 0x63,
    HWC_FORMAT_YUV422UVC        = 0x64,
    HWC_FORMAT_YUV422VUC        = 0x65,
    HWC_FORMAT_YUV411UVC        = 0x66,
    HWC_FORMAT_YUV411VUC        = 0x67,
    HWC_FORMAT_DEFAULT      = 0x99,    // The actual color format is determined
    HWC_FORMAT_MAXVALUE     = 0x100
};

struct src_info
{
    unsigned int w;
    unsigned int h;
    unsigned int crop_x;
    unsigned int crop_y;
    unsigned int crop_w;
    unsigned int crop_h;
    unsigned int format;    //HWC_FORMAT_YUV420VUC
    int color_space;    //disp_color_space
};

struct layer_dev {
    unsigned int req_stat;
    bool open_flag;
    disp_layer_config config;
    int rot_degree;
    pthread_mutex_t mlock;
};

struct hwdisp_mgr {
    struct layer_dev layer[CHN_NUM][LYL_NUM];
    int req_layer_cnt;

    int disp_fd;
    int cur_screen;

    pthread_mutex_t mlock;
    bool is_init;
};

/*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
int hw_display_init(void);
int hw_display_deinit(void);
//int hw_layer_request(unsigned char channel_id, unsigned char layer_id,
//                     unsigned char zorder, struct view_info *surface);
int hwd_init(void);
int hwd_exit(void);
//int hwd_layer_request(struct view_info* surface);
//int hwd_layer_request_v40(struct view_info* surface, int ui_video);
int hwd_layer_request_hlay(int hlay);
int hwd_layer_check_status(int hlay);
int hwd_layer_release(unsigned int hlay);
int hwd_layer_render(unsigned int hlay, libhwclayerpara_t *picture);
int hwd_layer_open(unsigned int hlay);
int hwd_layer_close(unsigned int hlay);
int hwd_layer_set_src(unsigned int hlay, struct src_info *src);
int hwd_layer_set_rect(unsigned int hlay, struct view_info *view);
int hwd_layer_top(unsigned int hlay);
int hwd_layer_bottom(unsigned int hlay);
int hwd_layer_set_zorder(unsigned int hlay, unsigned char zorder);
//int hwd_layer_ck_on(unsigned int hlay);
//int hwd_layer_ck_off(unsigned int hlay);
//int hwd_layer_ck_value(unsigned int hlay, unsigned int color);
int hwd_layer_exchange(unsigned int hlay1, unsigned int hlay2, int otherOnTop);
int hwd_layer_exchange_zorder(unsigned int hlay1, unsigned int hlay2);
int hwd_layer_switch(unsigned int hlay, int bOpen);
int hwd_layer_other_screen(int screen, unsigned int hlay1, unsigned int hlay2,disp_output_type disp_type, disp_tv_mode tv_mode);
//int hwd_layer_clear(unsigned int hlay);
int hwd_set_rot(int p_chn, int p_layer, int rot);
//int layer_request(int *pCh, int *pId);
int layer_config(__DISP_t cmd, disp_layer_config *pinfo);
int layer_release(int hlay);
//int layer_cmd(unsigned int hlay);
int layer_get_para(disp_layer_config *pinfo);
int layer_set_para(disp_layer_config *pinfo);
//int layer_set_normal(unsigned int hlay);
void openHdmi(int p_chn, int p_layer, disp_output_type p_disp_type, disp_tv_mode p_tv_mode);
int hwd_get_disp_type(int *disp_type, int *tv_mode);
int hwd_get_hdmi_hw_mode(disp_tv_mode *disp_mode);
#ifdef SDV_PRODUCT
int hwd_switch_vo_device(disp_output_type disp_type, disp_tv_mode tv_mode);
void getScreenInfo(int *swidth, int *sheight);
#else
int hwd_switch_vo_device(disp_output_type disp_type, disp_tv_mode tv_mode);
#endif
#ifdef __cplusplus
}
#endif
#endif

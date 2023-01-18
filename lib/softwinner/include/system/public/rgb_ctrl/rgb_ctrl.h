/************************************************************************************************/
/* Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.                                          */
/************************************************************************************************/
/**
 * @file aw_osd.h
 * @brief 
 * @author id: guixing
 * @version v0.1
 * @date 2016-08-28
 */

#ifndef _RGB_CTRL_H_
#define _RGB_CTRL_H_

/************************************************************************************************/
/*                                      Include Files                                           */
/************************************************************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <log/log_wrapper.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/************************************************************************************************/
/*                                     Macros & Typedefs                                        */
/************************************************************************************************/
#define MAX_COLOR_SIZE 8

#define RGB_DB
#ifdef RGB_DB
#define RGB_DB_PRT(fmt, args...)                                          \
    do {                                                                  \
        LOGD(fmt, ##args); \
    } while (0)
#endif

#define RGB_ERR
#ifdef RGB_ERR
#define RGB_ERR_PRT(fmt, args...)                                                                        \
    do {                                                                                                 \
        LOGE(fmt, ##args); \
    } while (0)
#endif

typedef enum tag_OSD_RGB_TYPE {
    OSD_RGB_555 = 0,
    OSD_RGB_565,
    OSD_RGB_24,
    OSD_RGB_32,
    OSD_RGB_BUTT,
} OSD_RGB_TYPE;

typedef enum tag_FONT_SIZE_TYPE {
    FONT_SIZE_16 = 0,
    FONT_SIZE_20,
    FONT_SIZE_24,
    FONT_SIZE_32,
    FONT_SIZE_40,
    FONT_SIZE_48,
    FONT_SIZE_56,
    FONT_SIZE_64,
    FONT_SIZE_BUTT,
} FONT_SIZE_TYPE;

typedef enum tag_CHAR_CODE_TYPE {
    CHAR_CODE_ASCII = 0,
    CHAR_CODE_GB2312,
    CHAR_CODE_UTF8,
    CHAR_CODE_USC2_LIT, /* unicode 2 little endian */
    CHAR_CODE_USC2_BIG, /* unicode 2 big endian */
    CHAR_CODE_BUTT,
} CHAR_CODE_TYPE;

/************************************************************************************************/
/*                                    Structure Declarations                                    */
/************************************************************************************************/
typedef struct tag_RGB_PIC_S {
    unsigned int wide;
    unsigned int high;
    OSD_RGB_TYPE rgb_type;
    int enable_mosaic;
    int mosaic_size;
    unsigned char background[MAX_COLOR_SIZE]; /* support rgb555, rgb565, rgb24, rgb32 */
    unsigned char mosaic_color[MAX_COLOR_SIZE];

    char *pic_addr;
    int pic_size;
} RGB_PIC_S;

typedef struct tag_PIC_REGION_S {
    int x;
    int y;
    int w;
    int h;
    OSD_RGB_TYPE rgb_type;
} PIC_REGION_S;

typedef struct tag_FONT_RGBPIC_S {
    FONT_SIZE_TYPE font_type;
    OSD_RGB_TYPE rgb_type;
    int enable_bg;
    unsigned char foreground[MAX_COLOR_SIZE];
    unsigned char background[MAX_COLOR_SIZE];
} FONT_RGBPIC_S;

/************************************************************************************************/
/*                                      Global Variables                                        */
/************************************************************************************************/
/* None */

/************************************************************************************************/
/*                                    Function Declarations                                     */
/************************************************************************************************/
/**
 * @brief 加载字体
 * @param font_type 字体大小类型
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int load_font_file(FONT_SIZE_TYPE font_type);

/**
 * @brief 卸载字体。调用该函数后，相关字体操作函数就不能被执行
 * @param
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int unload_gb2312_font(void);

/**
 * @brief 根据rgb_pic规格创建RGB矩形框
 * @param rgb_pic
 * - wide input 矩形的宽度
 * - high input 矩形的高度
 * - rgb_type      input  rgb的像素格式如，rgb555 rgb888 rgb8888等
 * - enable_mosaic input  是否使能背景马赛克功能
 * - mosaic_size   input  背景马赛克的宽高度
 * - background    input  矩形RGB图片的底色
 * - mosaic_color  input  背景马赛克的颜色
 * - pic_addr  output  创建成功后的图片地址，若失败则为NULL。使用完毕后记得
 * - pic_size  output  创建成功后的图片数据字节长度
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int create_rectangle_rgb(RGB_PIC_S *rgb_pic);

/**
 * @brief 根据rgb_pic规格创建RGB矩形框
 * @param font_pic
 * - font_type  input 字体大小类型
 * - rgb_type   input RGB格式
 * - enable_bg  input 是否使能背景色
 * - foreground input 字体颜色，即前景色
 * - background input 字体背景色
 * @param rgb_pic
 * - enable_mosaic input 是否使能背景马赛克功能
 * - mosaic_size   input  背景马赛克的宽高度
 * - mosaic_color  input  背景马赛克的颜色
 * - background  output  矩形RGB图片的底色，同font_pic中的background格式一样值
 * - rgb_type    output  rgb的像素格式，同font_pic中的rgb格式一样值
 * - pic_addr    output  创建成功后的图片地址，若失败则为NULL。使用完毕后记得
 * - pic_size    output  创建成功后的图片数据字节长度
 * - wide output 矩形的宽度
 * - high output 矩形的高度
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int create_font_rectangle(const char *code, const FONT_RGBPIC_S *font_pic, RGB_PIC_S *rgb_pic);

/**
 * @brief 释放rgb_pic图片
 * @param rgb_pic 待释放的rgb图片
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int release_rgb_picture(RGB_PIC_S *rgb_pic);

/**
 * @brief 将color填充满该图片
 * @param rgb_pic 待填充的RGB图片
 * @param color   所要填充的颜色。注意数据宽度为4字节及以上。
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int fill_rectangle_rgb(RGB_PIC_S *rgb_pic, const unsigned char *color);

/**
 * @brief 将src_pic图片，按位置信息叠加到dst_pic图片中
 * @param src_pic 源RGB图片
 * @param dst_pic 目标RGB图片
 * @param top     相对目标图片的顶部位置
 * @param left    相对目标图片的左边位置
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int draw_pic_in_pic(const RGB_PIC_S *src_pic, const RGB_PIC_S *dst_pic, int top, int left);

/**
 * @brief 将字体，按位置信息叠加到dst_pic图片中
 * @param code     字符编码序列
 * @param font_pic 字体RGB格式
 * @param dst_pic  目标RGB图片
 * @param top      相对目标图片的顶部位置
 * @param left     相对目标图片的左边位置
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int draw_font_in_pic(const char *code, const FONT_RGBPIC_S *font_pic, const RGB_PIC_S *dst_pic, int top, int left);

/**
 * 单独去加载某种大小的字体
 * @param font_type
 * @return
 *  - 成功 0
 *  - 失败 -1
 */
int unload_font_file(FONT_SIZE_TYPE font_type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _RGB_CTRL_H_ */

/*******************************************************************************
--                                                                            --
--                    CedarX Multimedia Framework                             --
--                                                                            --
--          the Multimedia Framework for Linux/Android System                 --
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                         Softwinner Products.                               --
--                                                                            --
--                   (C) COPYRIGHT 2011 SOFTWINNER PRODUCTS                   --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
*******************************************************************************/

#ifndef CDX_MediaFormat_H_
#define CDX_MediaFormat_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum CdxPixelFormat{
    /* Do not change these values (starting with CDX_PIXEL_FORMAT),
     * keep them in sync with system/core/include/system/graphics.h.
     */
    CDX_PIXEL_FORMAT_RGBA_8888          = 1,
    CDX_PIXEL_FORMAT_RGBX_8888          = 2,
    CDX_PIXEL_FORMAT_RGB_888            = 3,
    CDX_PIXEL_FORMAT_RGB_565            = 4,
    CDX_PIXEL_FORMAT_BGRA_8888          = 5,
    CDX_PIXEL_FORMAT_RGBA_5551          = 6,
    CDX_PIXEL_FORMAT_RGBA_4444          = 7,
    CDX_PIXEL_FORMAT_AW_NV12            = 0x101,
    CDX_PIXEL_FORMAT_AW_MB420           = 0x102,
    CDX_PIXEL_FORMAT_AW_MB411           = 0x103,
    CDX_PIXEL_FORMAT_AW_MB422           = 0x104,
    CDX_PIXEL_FORMAT_AW_YUV_PLANNER420  = 0x105,
    CDX_PIXEL_FORMAT_AW_NV61            = 0x106,    //yvu422, NV61

    CDX_PIXEL_FORMAT_YV12                   = 0x32315659, // YCrCb 4:2:0 Planar, HAL_PIXEL_FORMAT_YV12
    CDX_PIXEL_FORMAT_RAW_SENSOR             = 0x20,
    CDX_PIXEL_FORMAT_BLOB                   = 0x21,
    CDX_PIXEL_FORMAT_IMPLEMENTATION_DEFINED = 0x22,
    CDX_PIXEL_FORMAT_YCbCr_422_SP       = 0x10, // NV16
    CDX_PIXEL_FORMAT_YCrCb_420_SP       = 0x11, // NV21
    CDX_PIXEL_FORMAT_YCbCr_422_I        = 0x14, // YUY2
}CdxPixelFormat;

#ifdef __cplusplus
}
#endif

#endif

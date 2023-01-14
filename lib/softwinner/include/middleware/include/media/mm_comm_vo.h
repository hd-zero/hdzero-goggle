/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_vo.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/20
  Last Modified :
  Description   : common parameter definition for VO
  Function List :
  History       :
******************************************************************************/


#ifndef _AW_VO_COMMON_H_
#define _AW_VO_COMMON_H_

#include "plat_type.h"
#include "mm_common.h"
#include "mm_comm_video.h"

#define VO_DEF_CHN_BUF_LEN      8
#define VO_DEF_DISP_BUF_LEN		5
#define VO_DEF_VIRT_BUF_LEN		3

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef enum EN_VOU_ERR_CODE_E
{
    EN_ERR_VO_DEV_NOT_CONFIG	  = 0x40,
    EN_ERR_VO_DEV_NOT_ENABLE      = 0x41,
    EN_ERR_VO_DEV_HAS_ENABLED     = 0x42,
    EN_ERR_VO_DEV_HAS_BINDED      = 0x43,
    EN_ERR_VO_DEV_NOT_BINDED      = 0x44,

    EN_ERR_VO_NOT_ENABLE             = 0x45,
    EN_ERR_VO_NOT_DISABLE            = 0x46,
    EN_ERR_VO_NOT_CONFIG             = 0x47,

    EN_ERR_VO_CHN_NOT_DISABLE        = 0x48,
    EN_ERR_VO_CHN_NOT_ENABLE         = 0x49,
    EN_ERR_VO_CHN_NOT_CONFIG         = 0x4a,
    EN_ERR_VO_CHN_NOT_ALLOC          = 0x4b,

    EN_ERR_VO_CCD_INVALID_PAT        = 0x4c,
    EN_ERR_VO_CCD_INVALID_POS        = 0x4d,

    EN_ERR_VO_WAIT_TIMEOUT           = 0x4e,
    EN_ERR_VO_INVALID_VFRAME         = 0x4f,
    EN_ERR_VO_INVALID_RECT_PARA      = 0x50,
    EN_ERR_VO_SETBEGIN_ALREADY       = 0x51,
    EN_ERR_VO_SETBEGIN_NOTYET        = 0x52,
    EN_ERR_VO_SETEND_ALREADY         = 0x53,
    EN_ERR_VO_SETEND_NOTYET          = 0x54,

    EN_ERR_VO_GRP_INVALID_ID         = 0x55,
    EN_ERR_VO_GRP_NOT_CREATE         = 0x56,
    EN_ERR_VO_GRP_HAS_CREATED        = 0x57,
    EN_ERR_VO_GRP_NOT_DESTROY        = 0x58,
    EN_ERR_VO_GRP_CHN_FULL           = 0x59,
    EN_ERR_VO_GRP_CHN_EMPTY          = 0x5a,
    EN_ERR_VO_GRP_CHN_NOT_EMPTY      = 0x5b,
    EN_ERR_VO_GRP_INVALID_SYN_MODE   = 0x5c,
    EN_ERR_VO_GRP_INVALID_BASE_PTS   = 0x5d,
    EN_ERR_VO_GRP_NOT_START          = 0x5e,
    EN_ERR_VO_GRP_NOT_STOP           = 0x5f,
    EN_ERR_VO_GRP_INVALID_FRMRATE    = 0x60,
    EN_ERR_VO_GRP_CHN_HAS_REG        = 0x61,
    EN_ERR_VO_GRP_CHN_NOT_REG        = 0x62,
    EN_ERR_VO_GRP_CHN_NOT_UNREG      = 0x63,
    EN_ERR_VO_GRP_BASE_NOT_CFG       = 0x64,

    EN_ERR_GFX_NOT_DISABLE           = 0x65,
    EN_ERR_GFX_NOT_BIND              = 0x66,
    EN_ERR_GFX_NOT_UNBIND            = 0x67,
    EN_ERR_GFX_INVALID_ID            = 0x68,

    EN_ERR_VO_WBC_NOT_DISABLE        = 0x69,
    EN_ERR_VO_WBC_NOT_CONFIG         = 0x6a,

    EN_ERR_VO_CHN_AREA_OVERLAP       = 0x6b,

    EN_ERR_INVALID_WBCID          = 0x6c,
    EN_ERR_INVALID_LAYERID        = 0x6d,
    EN_ERR_VO_VIDEO_HAS_BINDED    = 0x6e,
    EN_ERR_VO_VIDEO_NOT_BINDED    = 0x6f,
    EN_ERR_VO_WBC_HAS_BIND           = 0x70,
    EN_ERR_VO_WBC_HAS_CONFIG         = 0x71,
    EN_ERR_VO_WBC_NOT_BIND           = 0x72,

    /* new added */
    EN_ERR_VO_BUTT

}EN_VOU_ERR_CODE_E;

/* System define error code */
#define ERR_VO_BUSY                  DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define ERR_VO_NO_MEM                DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define ERR_VO_NULL_PTR              DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define ERR_VO_SYS_NOTREADY          DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define ERR_VO_INVALID_DEVID         DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define ERR_VO_INVALID_CHNID         DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define ERR_VO_ILLEGAL_PARAM         DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define ERR_VO_NOT_SUPPORT           DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define ERR_VO_NOT_PERMIT            DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define ERR_VO_INVALID_WBCID         DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_WBCID)
#define ERR_VO_INVALID_LAYERID       DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_LAYERID)


/* device relative error code */
#define ERR_VO_DEV_NOT_CONFIG        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_CONFIG)
#define ERR_VO_DEV_NOT_ENABLE        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_ENABLE)
#define ERR_VO_DEV_HAS_ENABLED       DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_HAS_ENABLED)
#define ERR_VO_DEV_HAS_BINDED        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_HAS_BINDED)
#define ERR_VO_DEV_NOT_BINDED        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_DEV_NOT_BINDED)

/* video relative error code */
#define ERR_VO_VIDEO_NOT_ENABLE      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_NOT_ENABLE)
#define ERR_VO_VIDEO_NOT_DISABLE     DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_NOT_DISABLE)
#define ERR_VO_VIDEO_NOT_CONFIG      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_NOT_CONFIG)
#define ERR_VO_VIDEO_HAS_BINDED      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_VIDEO_HAS_BINDED)
#define ERR_VO_VIDEO_NOT_BINDED      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_VIDEO_NOT_BINDED)

/*wbc error code*/
#define ERR_VO_WBC_NOT_DISABLE     DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_WBC_NOT_DISABLE)
#define ERR_VO_WBC_NOT_CONFIG      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_WBC_NOT_CONFIG)
#define ERR_VO_WBC_HAS_CONFIG      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_WBC_HAS_CONFIG)
#define ERR_VO_WBC_NOT_BIND        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_WBC_NOT_BIND)
#define ERR_VO_WBC_HAS_BIND        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_WBC_HAS_BIND)

/* channel relative error code */
#define ERR_VO_CHN_NOT_DISABLE       DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_CHN_NOT_DISABLE)
#define ERR_VO_CHN_NOT_ENABLE        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_CHN_NOT_ENABLE)
#define ERR_VO_CHN_NOT_CONFIG        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_CHN_NOT_CONFIG)
#define ERR_VO_CHN_NOT_ALLOC         DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_CHN_NOT_ALLOC)
#define ERR_VO_CHN_AREA_OVERLAP      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_CHN_AREA_OVERLAP)
/* component state is same as user wanted */
#define ERR_VO_CHN_SAMESTATE         DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_VO_CHN_INVALIDSTATE      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_VO_CHN_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_VO_CHN_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)


/* cascade relatvie error code */
#define ERR_VO_INVALID_PATTERN       DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_CCD_INVALID_PAT)
#define ERR_VO_INVALID_POSITION      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_CCD_INVALID_POS)

/* misc */
#define ERR_VO_WAIT_TIMEOUT          DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_WAIT_TIMEOUT)
#define ERR_VO_INVALID_VFRAME        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_INVALID_VFRAME)
#define ERR_VO_INVALID_RECT_PARA     DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_INVALID_RECT_PARA)
#define ERR_VO_SETBEGIN_ALREADY      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_SETBEGIN_ALREADY)
#define ERR_VO_SETBEGIN_NOTYET       DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_SETBEGIN_NOTYET)
#define ERR_VO_SETEND_ALREADY        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_SETEND_ALREADY)
#define ERR_VO_SETEND_NOTYET         DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_SETEND_NOTYET)

/* sync group relative error code */
#define ERR_VO_GRP_INVALID_ID        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_INVALID_ID)
#define ERR_VO_GRP_NOT_CREATE        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_NOT_CREATE)
#define ERR_VO_GRP_HAS_CREATED       DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_HAS_CREATED)
#define ERR_VO_GRP_NOT_DESTROY       DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_NOT_DESTROY)
#define ERR_VO_GRP_CHN_FULL          DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_CHN_FULL)
#define ERR_VO_GRP_CHN_EMPTY         DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_CHN_EMPTY)
#define ERR_VO_GRP_CHN_NOT_EMPTY     DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_CHN_NOT_EMPTY)
#define ERR_VO_GRP_INVALID_SYN_MODE  DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_INVALID_SYN_MODE)
#define ERR_VO_GRP_INVALID_BASE_PTS  DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_INVALID_BASE_PTS)
#define ERR_VO_GRP_NOT_START         DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_NOT_START)
#define ERR_VO_GRP_NOT_STOP          DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_NOT_STOP)
#define ERR_VO_GRP_INVALID_FRMRATE   DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_INVALID_FRMRATE)
#define ERR_VO_GRP_CHN_HAS_REG       DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_CHN_HAS_REG)
#define ERR_VO_GRP_CHN_NOT_REG       DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_CHN_NOT_REG)
#define ERR_VO_GRP_CHN_NOT_UNREG     DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_CHN_NOT_UNREG)
#define ERR_VO_GRP_BASE_NOT_CFG      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_VO_GRP_BASE_NOT_CFG)


/* graphics relative error code */
#define ERR_VO_GFX_NOT_DISABLE     DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_GFX_NOT_DISABLE)
#define ERR_VO_GFX_NOT_BIND        DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_GFX_NOT_BIND)
#define ERR_VO_GFX_NOT_UNBIND      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_GFX_NOT_UNBIND)
#define ERR_VO_GFX_INVALID_ID      DEF_ERR(MOD_ID_VOU, EN_ERR_LEVEL_ERROR, EN_ERR_GFX_INVALID_ID)

/* vo inteface type */
#define VO_INTF_CVBS        (0x01L<<0)
#define VO_INTF_YPBPR       (0x01L<<1)
#define VO_INTF_VGA         (0x01L<<2)
#define VO_INTF_BT656       (0x01L<<3)
#define VO_INTF_BT1120      (0x01L<<4)
#define VO_INTF_HDMI        (0x01L<<5)
#define VO_INTF_LCD         (0x01L<<6)
#define VO_INTF_BT656_H     (0x01L<<7)
#define VO_INTF_BT656_L     (0x01L<<8)
#define VO_INTF_LCD_6BIT    (0x01L<<9)
#define VO_INTF_LCD_8BIT    (0x01L<<10)
#define VO_INTF_LCD_16BIT   (0x01L<<11)






#define VO_DEFAULT_CHN      -1          /* use vo buffer as pip buffer */

/*****************************************************************************
 * 3520 ADDed
 *****************************************************************************/
typedef int VO_INTF_TYPE_E;



typedef enum VO_INTF_SYNC_E
{
    VO_OUTPUT_PAL = 0,
    VO_OUTPUT_NTSC,

    VO_OUTPUT_1080P24,
    VO_OUTPUT_1080P25,
    VO_OUTPUT_1080P30,

    VO_OUTPUT_720P50,
    VO_OUTPUT_720P60,
    VO_OUTPUT_1080I50,
    VO_OUTPUT_1080I60,
    VO_OUTPUT_1080P50,
    VO_OUTPUT_1080P60,
    VO_OUTPUT_3840x2160_24,
    VO_OUTPUT_3840x2160_25,
    VO_OUTPUT_3840x2160_30,

    VO_OUTPUT_576P50,
    VO_OUTPUT_480P60,

    VO_OUTPUT_800x600_60,            /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1024x768_60,           /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1280x1024_60,          /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1366x768_60,           /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1440x900_60,           /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    VO_OUTPUT_1280x800_60,           /* 1280*800@60Hz VGA@60Hz*/
    VO_OUTPUT_1600x1200_60,          /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1680x1050_60,          /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1920x1200_60,          /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking)*/
    VO_OUTPUT_640x480_60,            /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    VO_OUTPUT_960H_PAL,              /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced)*/
    VO_OUTPUT_960H_NTSC,             /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced)*/
    VO_OUTPUT_320X240_30,            /* For ota5182 at 30 Hz  just for hi3516d/hi3518ev200, hi3516a not support*/
    VO_OUTPUT_320X240_50,            /* For ili9342 at 50 Hz ,just for hi3516d/hi3518ev200, hi3516a not support */
    VO_OUTPUT_240X320_50,            /* For ili9341 at 50 Hz ,just for hi3516d/hi3518ev200, hi3516a not support */
    VO_OUTPUT_240X320_60,
    VO_OUTPUT_USER,
    VO_OUTPUT_BUTT

} VO_INTF_SYNC_E;

typedef enum VO_DISPLAY_FIELD_E
{
  VO_FIELD_TOP,                 /* top field*/
  VO_FIELD_BOTTOM,              /* bottom field*/
  VO_FIELD_BOTH,                /* top and bottom field*/
  VO_FIELD_BUTT
} VO_DISPLAY_FIELD_E;

typedef enum VOU_ZOOM_IN_E
{
    VOU_ZOOM_IN_RECT = 0,       /* zoom in by rect */
    VOU_ZOOM_IN_RATIO,          /* zoom in by ratio */
    VOU_ZOOM_IN_BUTT
} VOU_ZOOM_IN_E;

typedef enum VO_CSC_MATRIX_E
{
    VO_CSC_MATRIX_IDENTITY = 0,
    
    VO_CSC_MATRIX_BT601_TO_BT709,
    VO_CSC_MATRIX_BT709_TO_BT601,

    VO_CSC_MATRIX_BT601_TO_RGB_PC,
    VO_CSC_MATRIX_BT709_TO_RGB_PC,

    VO_CSC_MATRIX_RGB_TO_BT601_PC,
    VO_CSC_MATRIX_RGB_TO_BT709_PC,

    VO_CSC_MATRIX_BUTT
} VO_CSC_MATRIX_E;

typedef struct VO_CHN_ATTR_S
{
    unsigned int mPriority;                /* video out overlay pri */
    RECT_S  mRect;                     /* rect of video out chn */
    BOOL mbDeflicker;                 /* deflicker or not */
}VO_CHN_ATTR_S;

typedef struct VO_BORDER_S
{    
    BOOL mbBorderEn;                   /*do Frame or not*/  
    BORDER_S mBorder;
}VO_BORDER_S;


typedef struct VO_QUERY_STATUS_S
{
    unsigned int mChnBufUsed;       /* channel buffer that been occupied */
} VO_QUERY_STATUS_S;

typedef struct VO_SYNC_INFO_S
{
    BOOL  mbSynm;     /* sync mode(0:timing,as BT.656; 1:signal,as LCD) */
    BOOL  mbIop;      /* interlaced or progressive display(0:i; 1:p) */
    unsigned char mIntfb;   /* interlace bit width while output */

    unsigned short   mVact ;  /* vertical active area */
    unsigned short   mVbb;    /* vertical back blank porch */
    unsigned short   mVfb;    /* vertical front blank porch */

    unsigned short   mHact;   /* herizontal active area */
    unsigned short   mHbb;    /* herizontal back blank porch */
    unsigned short   mHfb;    /* herizontal front blank porch */
    unsigned short   mHmid;   /* bottom herizontal active area */

    unsigned short   mBvact;  /* bottom vertical active area */
    unsigned short   mBvbb;   /* bottom vertical back blank porch */
    unsigned short   mBvfb;   /* bottom vertical front blank porch */

    unsigned short   mHpw;    /* horizontal pulse width */
    unsigned short   mVpw;    /* vertical pulse width */

    BOOL  mbIdv;      /* inverse data valid of output */
    BOOL  mbIhs;      /* inverse horizontal synch signal */
    BOOL  mbIvs;      /* inverse vertical synch signal */

} VO_SYNC_INFO_S;

typedef struct VO_PUB_ATTR_S
{
    unsigned int             mBgColor;          /* Background color of a device, in RGB format. */
    VO_INTF_TYPE_E           enIntfType;          /* Type of a VO interface */
    VO_INTF_SYNC_E           enIntfSync;          /* Type of a VO interface timing */
    VO_SYNC_INFO_S           stSyncInfo;          /* Information about VO interface timings */
} VO_PUB_ATTR_S;

typedef struct VO_CHN_VFRAME_TIMEOUT_S
{
    VIDEO_FRAME_INFO_S  stVFrame;
    int              mMilliSec;
} VO_CHN_VFRAME_TIMEOUT_S;

typedef struct VO_SCREEN_VFRAME_TIMEOUT_S
{
    VIDEO_FRAME_INFO_S  stVFrame;
    int              mMilliSec;
} VO_SCREEN_VFRAME_TIMEOUT_S;

typedef struct VO_USR_SEND_TIMEOUT_S
{
    VIDEO_FRAME_INFO_S  stVFrame;
    int              mMilliSec;
} VO_USR_SEND_TIMEOUT_S;


typedef enum VO_PART_MODE_E
{
    VO_PART_MODE_SINGLE	= 0,		/* single partition, which use software to make multi-picture in one hardware cell */
	VO_PART_MODE_MULTI	= 1,		/* muliti partition, each partition is a hardware cell */
	VO_PART_MODE_BUTT
	
} VO_PART_MODE_E;

typedef struct VO_VIDEO_LAYER_ATTR_S
{
    RECT_S stDispRect;                  /* Display resolution */
    SIZE_S stImageSize;                 /* Canvas size of the video layer */
    unsigned int mDispFrmRt;                /* Display frame rate */
    PIXEL_FORMAT_E enPixFormat;         /* Pixel format of the video layer */
    BOOL bDoubleFrame;               /* Whether to double frames */  
    BOOL bClusterMode;               /* Whether to take Cluster way to use memory*/ 
} VO_VIDEO_LAYER_ATTR_S;

typedef enum VOU_LAYER_DDR_E
{
    VOU_LAYER_DDR0 = 0,
    VOU_LAYER_DDR1 = 1,
    VOU_LAYER_DDR_BUTT
}VOU_LAYER_DDR_E;

typedef struct VO_ZOOM_RATIO_S
{
    unsigned int mXRatio;
    unsigned int mYRatio;
    unsigned int mWRatio;
    unsigned int mHRatio;    
} VO_ZOOM_RATIO_S;

typedef struct VO_ZOOM_ATTR_S
{
    VOU_ZOOM_IN_E   enZoomType;         /* choose the type of zoom in */
    union
    {
        RECT_S          stZoomRect;     /* zoom in by rect */
        VO_ZOOM_RATIO_S stZoomRatio;    /* zoom in by ratio */
    };
} VO_ZOOM_ATTR_S;

typedef struct VO_CSC_S
{
    VO_CSC_MATRIX_E enCscMatrix;
    unsigned int mLuma;                     /* luminance:   0 ~ 100 default: 50 */
    unsigned int mContrast;                 /* contrast :   0 ~ 100 default: 50 */
    unsigned int mHue;                      /* hue      :   0 ~ 100 default: 50 */
    unsigned int mSatuature;                /* satuature:   0 ~ 100 default: 50 */
} VO_CSC_S;


typedef struct VO_REGION_INFO_S
{
    RECT_S *pstRegion;    /*region attribute*/
    unsigned int mRegionNum;       /*count of the region*/
}VO_REGION_INFO_S;

typedef struct VO_REGION_LUMA_S
{
    VO_REGION_INFO_S stRegionInfo;     /*Information of the region*/
    unsigned int* pLumaData;             /*Luma data of the region*/
    int mMilliSec;               /*time parameter.less than 0 means waiting until get the luma data,
                                             equal to 0 means get the luma data no matter whether it can or not,
                                             more than 0 means waiting how long the time parameter it is*/
}VO_REGION_LUMA_S;

typedef struct VO_VIDEO_LAYER_ALPHA_S
{
    unsigned char mAlphaMode;   /* 0: Pixel Mode, 1: Global Mode */
    unsigned char mAlphaValue;
}VO_VIDEO_LAYER_ALPHA_S;

/* parameter for NATIVE_WINDOW_SET_SCALING_MODE */
enum {
    /* the window content is not updated (frozen) until a buffer of
     * the window size is received (enqueued)
     */
    NATIVE_WINDOW_SCALING_MODE_FREEZE           = 0,
    /* the buffer is scaled in both dimensions to match the window size */
    NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW  = 1,
    /* the buffer is scaled uniformly such that the smaller dimension
     * of the buffer matches the window size (cropping in the process)
     */
    NATIVE_WINDOW_SCALING_MODE_SCALE_CROP       = 2,
    /* the window is clipped to the size of the buffer's crop rectangle; pixels
     * outside the crop rectangle are treated as if they are completely
     * transparent.
     */
    NATIVE_WINDOW_SCALING_MODE_NO_SCALE_CROP    = 3,
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AW_VO_COMMON_H_*/


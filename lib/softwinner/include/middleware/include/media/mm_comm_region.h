/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_region.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/04/25
  Last Modified :
  Description   : include RGN struct , Marco and Error information
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_COMM_REGION_H__
#define __IPCLINUX_COMM_REGION_H__

#include "mm_common.h"
#include "mm_comm_video.h"
#include "plat_errno.h"
#include "plat_defines.h"


#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */



/* region flip value*/
typedef enum RGN_FLIP_FLAG_E
{
    RGN_FLIP_NONE       = 0,
    RGN_FLIP_HFLIP      = (1<<0),
    RGN_FLIP_VFLIP      = (1<<1),
    RGN_FLIP_BOTH_FLIP  = (RGN_FLIP_HFLIP | RGN_FLIP_VFLIP),
    RGN_FLIP_BUTT
} RGN_FLIP_FLAG_E;

typedef unsigned int RGN_HANDLE;

/* type of video regions */
typedef enum RGN_TYPE_E
{
    OVERLAY_RGN = 0,     /* video overlay region */
    COVER_RGN,
    COVEREX_RGN,
    OVERLAYEX_RGN,
    RGN_BUTT
} RGN_TYPE_E;

typedef enum INVERT_COLOR_MODE_E
{
    LESSTHAN_LUM_THRESH = 0,   /* the lum of the video is less than the lum threshold which is set by u32LumThresh  */ 
    MORETHAN_LUM_THRESH,       /* the lum of the video is more than the lum threshold which is set by u32LumThresh  */
    LESSTHAN_LUMDIFF_THRESH,   /* the lum diff between video and overlay area is less than threshold */
    MORETHAN_LUMDIFF_THRESH,   /* the lum diff between video and overlay area is more than threshold */
    INVERT_COLOR_BUTT
}INVERT_COLOR_MODE_E;

typedef struct OVERLAY_QP_INFO_S
{
    BOOL 	bAbsQp;
    int 		mQp;
	BOOL		bQpDisable;
}OVERLAY_QP_INFO_S;

typedef struct OVERLAY_INVERT_COLOR_S
{
    SIZE_S stInvColArea;                //It must be multipe of 16 but not more than 64.
    unsigned int mLumThresh;                //The threshold to decide whether invert the OSD's color or not.
    INVERT_COLOR_MODE_E enChgMod;      
    BOOL bInvColEn;                  //The switch of inverting color.
}OVERLAY_INVERT_COLOR_S;

typedef struct OVERLAY_ATTR_S
{
    /* bitmap pixel format,now only support ARGB1555 or ARGB4444 */
    PIXEL_FORMAT_E mPixelFmt;

    /* background color, pixel format depends on "enPixelFmt" */
    unsigned int mBgColor;

    /* region size,W:[4,4096],align:2,H:[4,4096],align:2 */
    SIZE_S mSize;
}OVERLAY_ATTR_S;

typedef struct OVERLAY_CHN_ATTR_S
{
    /* X:[0,4096],align:4,Y:[0,4096],align:4 */
    POINT_S stPoint;
    
    /* background an foreground transparence when pixel format is ARGB1555 
      * the pixel format is ARGB1555,when the alpha bit is 1 this alpha is value!
      * range:[0,128]
      */
    unsigned int mFgAlpha;

	/* background an foreground transparence when pixel format is ARGB1555 
      * the pixel format is ARGB1555,when the alpha bit is 0 this alpha is value!
      * range:[0,128]
      */
    unsigned int mBgAlpha;

    unsigned int mLayer;   /* OVERLAY region layer range:[0,7]*/

    OVERLAY_QP_INFO_S stQpInfo;

    OVERLAY_INVERT_COLOR_S stInvertColor;
}OVERLAY_CHN_ATTR_S;

typedef enum RGN_AREA_TYPE_E
{
    AREA_RECT = 0,
    AREA_QUAD_RANGLE,
    AREA_BUTT
} RGN_AREA_TYPE_E;

typedef struct hiRGN_QUADRANGLE_S
{
    BOOL bSolid;            /* whether solid or dashed quadrangle */
    unsigned int u32Thick;           /* Line Width of quadrangle, valid when dashed quadrangle */
    POINT_S stPoint[4];        /* points of quadrilateral */
} RGN_QUADRANGLE_S;

typedef struct COVER_CHN_ATTR_S
{
	RGN_AREA_TYPE_E     enCoverType;        /* rect or arbitary quadrilateral COVER */
    union
    {
        RECT_S              stRect;        	/* config of rect */
        RGN_QUADRANGLE_S    stQuadRangle;  	/* config of arbitary quadrilateral COVER */
    };
    unsigned int mColor;
    unsigned int mLayer;   						/* COVER region layer */
}COVER_CHN_ATTR_S;

typedef struct COVEREX_CHN_ATTR_S
{
	RGN_AREA_TYPE_E     enCoverType;       /* rect or arbitary quadrilateral COVER */
    union
    {
        RECT_S              stRect;        /* config of rect */
        RGN_QUADRANGLE_S    stQuadRangle;  /* config of arbitary quadrilateral COVER */
    };
    unsigned int u32Color;
    unsigned int u32Layer;   /* COVEREX region layer range:[0,7] */
}COVEREX_CHN_ATTR_S;

typedef struct OVERLAYEX_ATTR_S
{
    PIXEL_FORMAT_E enPixelFmt;

    /* background color, pixel format depends on "enPixelFmt" */
    unsigned int u32BgColor;

    /* region size,W:[4,1920],align:2,H:[4,1080],align:2 */
    SIZE_S stSize;
}OVERLAYEX_ATTR_S;

typedef struct OVERLAYEX_CHN_ATTR_S
{
    /* X:[0,4096],align:4,Y:[0,4636],align:4 */
    POINT_S stPoint;
    
    /* background an foreground transparence when pixel format is ARGB1555 
      * the pixel format is ARGB1555,when the alpha bit is 1 this alpha is value!
      * range:[0,255]
      */
    unsigned int u32FgAlpha;

	/* background an foreground transparence when pixel format is ARGB1555 
      * the pixel format is ARGB1555,when the alpha bit is 0 this alpha is value!
      * range:[0,255]
      */
    unsigned int u32BgAlpha;

    unsigned int u32Layer;   /* OVERLAYEX region layer range:[0,15]*/
}OVERLAYEX_CHN_ATTR_S;

typedef union RGN_ATTR_U
{
    OVERLAY_ATTR_S      stOverlay;      /* attribute of overlay region */
    OVERLAYEX_ATTR_S    stOverlayEx;    /* attribute of overlayex region */ 
} RGN_ATTR_U;

typedef union RGN_CHN_ATTR_U
{
    OVERLAY_CHN_ATTR_S      stOverlayChn;      /* attribute of overlay region */
    COVER_CHN_ATTR_S        stCoverChn;        /* attribute of cover region */
    COVEREX_CHN_ATTR_S      stCoverExChn;      /* attribute of coverex region */
    OVERLAYEX_CHN_ATTR_S    stOverlayExChn;    /* attribute of overlayex region */
} RGN_CHN_ATTR_U;

/* attribute of a region */
typedef struct RGN_ATTR_S
{
    RGN_TYPE_E enType;  /* region type */
    RGN_ATTR_U unAttr;  /* region attribute */
} RGN_ATTR_S;

/* attribute of a region */
typedef struct RGN_CHN_ATTR_S
{
    BOOL           bShow;
    RGN_TYPE_E        enType;     /* region type */
    RGN_CHN_ATTR_U    unChnAttr;  /* region attribute */
} RGN_CHN_ATTR_S;



#define RGN_MAX_BMP_UPDATE_NUM 16

typedef struct hiRGN_BMP_UPDATE_S
{
    POINT_S             stPoint;
    BITMAP_S            stBmp;
    unsigned int              u32Stride;
} RGN_BMP_UPDATE_S;

typedef struct hiRGN_BMP_UPDATE_CFG_S
{
    unsigned int              u32BmpCnt;
    RGN_BMP_UPDATE_S    astBmpUpdate[RGN_MAX_BMP_UPDATE_NUM];
} RGN_BMP_UPDATE_CFG_S;

typedef struct hiRGN_CANVAS_INFO_S
{
    unsigned int         u32PhyAddr;
    unsigned int         u32VirtAddr;
    SIZE_S         stSize;              
    unsigned int         u32Stride;
    PIXEL_FORMAT_E enPixelFmt;  
} RGN_CANVAS_INFO_S;



/* invlalid device ID */
#define ERR_RGN_INVALID_DEVID     DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define ERR_RGN_INVALID_CHNID     DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_RGN_ILLEGAL_PARAM     DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_RGN_EXIST             DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/*UN exist*/
#define ERR_RGN_UNEXIST           DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define ERR_RGN_NULL_PTR          DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define ERR_RGN_NOT_CONFIG        DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define ERR_RGN_NOT_SUPPORT       DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define ERR_RGN_NOT_PERM          DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define ERR_RGN_NOMEM             DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_RGN_NOBUF             DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define ERR_RGN_BUF_EMPTY         DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define ERR_RGN_BUF_FULL          DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* bad address, eg. used for copy_from_user & copy_to_user */
#define ERR_RGN_BADADDR           DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)
/* resource is busy, eg. destroy a venc chn without unregistering it */
#define ERR_RGN_BUSY              DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* System is not ready,maybe not initialed or loaded.
 * Returning the error code when opening a device file failed.
 */
#define ERR_RGN_NOTREADY          DEF_ERR(MOD_ID_RGN, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __IPCLINUX_COMM_REGION_H__ */




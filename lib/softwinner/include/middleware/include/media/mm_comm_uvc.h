/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_uvc.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/11/17
  Last Modified :
  Description   : common struct definition for uvc
  Function List :
  History       :
******************************************************************************/

#ifndef __MM_COMM_UVC_H__
#define __MM_COMM_UVC_H__

#pragma once

#include "plat_type.h"
#include "plat_errno.h"
//#include "typedef.h"
#include "mm_common.h"
//#include "mm_comm_video.h"
//#include "mm_comm_rc.h"
#include <linux/videodev2.h>

/* invlalid channel ID */
#define ERR_UVC_INVALID_CHNID     DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_UVC_ILLEGAL_PARAM     DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_UVC_EXIST             DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel exists */
#define ERR_UVC_UNEXIST           DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define ERR_UVC_NULL_PTR          DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define ERR_UVC_NOT_CONFIG        DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define ERR_UVC_NOT_SUPPORT       DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define ERR_UVC_NOT_PERM          DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define ERR_UVC_NOMEM             DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_UVC_NOBUF             DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define ERR_UVC_BUF_EMPTY         DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define ERR_UVC_BUF_FULL          DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define ERR_UVC_SYS_NOTREADY      DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* system is busy*/
#define ERR_UVC_BUSY              DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* component state is same as user wanted */
#define ERR_UVC_SAMESTATE         DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_UVC_INVALIDSTATE      DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_UVC_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_UVC_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_UVC, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)

/*typedef struct UVC_FRAME_BUF_INFO_S
{
    int   u32PoolId;
    unsigned int  u32Width;
    unsigned int  u32Height;
    int     ePixelFormat;
    char*   pData0;
    char*   pData1;
    unsigned int phyYBufAddr;
    unsigned int phyCBufAddr;
    int     nFrameRate;
} UVC_FRAME_BUF_INFO_S;*/

typedef struct USB_VIDEO_CAMERA_ATTR_S
{
    //int uvc_fd;
    char *devname;  //[in]
    unsigned int pixelformat;   //[in]  V4L2_PIX_FMT_H264, mjpeg/264/
    unsigned int uvcvideo_width;    //[in]
    unsigned int uvcvideo_height;   //[in]
    //unsigned int Image_Size;
}USB_VIDEO_CAMERA_ATTR_S;

typedef struct VDECODER_ATTR_S
{
    int capfmt; //[out] V4L2_PIX_FMT_H264, mjpeg/h264/raw
    int outfmt; //[in] nv21
    int width;  //[out] display_width
    int height; //[out]
    int subw;   //[in/out] display_width
    int subh;   //[in/out]
    int fps;    //[out]
    int scl;    //[out]
}VDECODER_ATTR_S;

typedef struct UVC_CHN_ATTR_S
{
    USB_VIDEO_CAMERA_ATTR_S UVC_ATTR;
    VDECODER_ATTR_S VDECODER_ATTR;
}UVC_CHN_ATTR_S;

typedef struct UVCVideoControl
{
    //Terminal Control
    char bAutoExposureMode;
    char bAutoExposurePriority;
    int dwExposureTimeAbsolute; //1:100us
    char bExposureTimeRelative;
    
    short wIrisAbsolute;
    char bIrisRelative;  
    
    char bPrivacy; 
    
    //Unit Control
    short wBacklightCompensation;
    short wBrightness;
    
    short wContrast;

    short wGain;

    char bPowerLineFrequency;//0:disable, 1:50Hz, 2:60HZ

    short wHue;
    char bHueAuto;
    
    short wSaturation;

    short wSharpness;

    short wGamma;

    short wWhiteBalanceTemperature;
    char wWhiteBalanceTemperatureAuto;
    short wWhiteBalanceBlue;
    short wWhiteBalanceRed;
    char bWhiteBalanceComponentAuto;       
    
}UVCVideoControl;

typedef struct UVCMotionControl
{
    //Terminal Control
    short wFocusAbsolute;
    char bFocusRelative;
    char bSpeed;
    char bFocusAuto;
    
    short wObjectveFocallLength;
    char bZoom;
    char DigitalZoom;
    char bZoomSpeed;

    int dwPanAbsolute;
    int dwTilAbsolute;

    char bPanRelative;
    char bPanSpeed;
    char bTiltRelative;
    char bTiltSpeed;

    short wRollAbsolute;
    char bRollRelative;
    char bRollSpeed;
    
}UVCMotionControl;

typedef enum UVC_CAPTURE_FORMAT
{ 
    //base on the UVC class 1.1
    UVC_YUY2 = V4L2_PIX_FMT_YUYV,
    UVC_NV12 = V4L2_PIX_FMT_YUV420, // 
    UVC_H264 = V4L2_PIX_FMT_H264,
    UVC_MJPEG = V4L2_PIX_FMT_MJPEG
}UVC_CAPTURE_FORMAT;


typedef struct UVC_ATTR_S
{
//    v4l2_format mFmt;
//    v4l2_capability mCap;
//    v4l2_streamparm mStreamParm;
//    v4l2_buffer mBuf;
//    v4l2_requestbuffers mRBuf;
//    v4l2_timecode mTimeCode;
    UVC_CAPTURE_FORMAT mPixelformat;   //[in]
    unsigned int mUvcVideo_Width;    //[in]
    unsigned int mUvcVideo_Height;   //[in]
    unsigned int mUvcVideo_Fps;
    unsigned int mUvcVideo_BufCnt;  //[in]
    
//    UVCVideoControl mUvcVideoAttr;
//    UVCMotionControl mUvcMotionAttr;
}UVC_ATTR_S;


typedef enum UvcMsgType {
    SetThreadState = 1,
    FlushThreadState,
}UvcMsgType;

#endif /* __MM_COMMON_UVC_H__ */

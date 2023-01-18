
/*
 ******************************************************************************
 *
 * common_vi.h
 *
 * Hawkview ISP - common_vi.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   3.0		  Yang Feng   	2016/04/01	VIDEO INPUT
 *
 *****************************************************************************
 */

#ifndef _AW_COMMON_VI_H_
#define _AW_COMMON_VI_H_

#include "mm_common.h"
#include <mm_comm_video.h>
#include "plat_errno.h"
#include "sunxi_camera_v2.h"
#include <aw_type.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MAX_VIPP_DEV_NUM 	4
#define MAX_VIDEO_NUM 		MAX_VIPP_DEV_NUM
#define MAX_VIR_CHN_NUM 	4

#define VI_ERR_PRIV_TIMEOUT (EN_ERR_BUTT + 1)
#define VI_ERR_PRIV_NOTENABLE (EN_ERR_BUTT + 2)
#define VI_ERR_PRIV_NOTDISABLE (EN_ERR_BUTT + 3)
#define VI_ERR_PRIV_NORM_UNMATCH (EN_ERR_BUTT + 4)
#define VI_ERR_PRIV_INVALID_PHYCHNID (EN_ERR_BUTT + 5)
#define VI_ERR_PRIV_FAILED_NOTBIND (EN_ERR_BUTT + 6)
#define VI_ERR_PRIV_FAILED_BINDED (EN_ERR_BUTT + 7)
#define VI_ERR_PRIV_UNEXIST (EN_ERR_BUTT + 8)
#define VI_ERR_PRIV_EXIST (EN_ERR_BUTT + 9)

#define ERR_VI_INVALID_CHNID DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define ERR_VI_INVALID_PARA DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define ERR_VI_INVALID_NULL_PTR DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define ERR_VI_FAILED_NOTCONFIG DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
#define ERR_VI_NOT_SUPPORT DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define ERR_VI_NOT_PERM DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define ERR_VI_INVALID_DEVID DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
#define ERR_VI_NOMEM DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
#define ERR_VI_BUF_EMPTY DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define ERR_VI_BUF_FULL DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define ERR_VI_SYS_NOTREADY DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define ERR_VI_BUSY DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

#define ERR_VI_FAILED_NOTENABLE DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, VI_ERR_PRIV_NOTENABLE)
#define ERR_VI_FAILED_NOTDISABLE DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, VI_ERR_PRIV_NOTDISABLE)
#define ERR_VI_CFG_TIMEOUT DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, VI_ERR_PRIV_TIMEOUT)
#define ERR_VI_NORM_UNMATCH DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, VI_ERR_PRIV_NORM_UNMATCH)
#define ERR_VI_INVALID_PHYCHNID DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, VI_ERR_PRIV_INVALID_PHYCHNID)
#define ERR_VI_FAILED_NOTBIND DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, VI_ERR_PRIV_FAILED_NOTBIND)
#define ERR_VI_FAILED_BINDED DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, VI_ERR_PRIV_FAILED_BINDED)
#define ERR_VI_UNEXIST DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, VI_ERR_PRIV_UNEXIST)
#define ERR_VI_EXIST DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, VI_ERR_PRIV_EXIST)
#define ERR_VI_SAMESTATE DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
#define ERR_VI_INVALIDSTATE DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
#define ERR_VI_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
#define ERR_VI_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)
#define ERR_VI_EIS_EFUSE_ERR DEF_ERR(MOD_ID_VIU, EN_ERR_LEVEL_ERROR, EN_ERR_EFUSE_ERROR)

//typedef unsigned char AW_U8;
//typedef unsigned short AW_U16;
//typedef unsigned int AW_U32;
//
//typedef signed char AW_S8;
//typedef short AW_S16;
//typedef int AW_S32;
// typedef AW_S32 AW_DEV;
// typedef AW_S32 AW_CHN;
//typedef unsigned long long AW_U64;
//typedef long long AW_S64;

//typedef void AW_VOID;
/*
typedef enum {
    AW_FALSE = 0,
    AW_TRUE = 1,
} AW_BOOL;
*/
/*
 * intf format
 */
typedef enum awVI_INTF_E {
    VI_IF_INTLV = 0x00,  /* 1SEG DATA in one channel */
    VI_IF_SPL = 0x01,    /* 2SEG 1SEG Y in one channel , 1SEG UV in second channel */
    VI_IF_PL = 0x02,     /* 3SEG YUV444 */
    VI_IF_PL_SPL = 0x03, /* 3SEG YUV444 to 2SEG YUV422 */

    VI_IF_CCIR656_1CH = 0x04,        /* 1SEG ccir656 1ch */
    VI_IF_CCIR656_1CH_SPL = 0x05,    /* 2SEG ccir656 1ch */
    VI_IF_CCIR656_1CH_PL = 0x06,     /* 3SEG ccir656 1ch */
    VI_IF_CCIR656_1CH_PL_SPL = 0x07, /* 3SEG to 2SEG ccir656 1ch */

    VI_IF_CCIR656_16BIT = 0x08, /* 16BIT ccir656 1ch */

    VI_IF_CCIR656_2CH = 0x0c, /* D7~D0:ccir656 2ch */
    VI_IF_CCIR656_4CH = 0x0d, /* D7~D0:ccir656 4ch */

    VI_IF_MIPI = 0x80, /* MIPI VI */
} VI_INTF_E;

/*
 *  data width
 */
typedef enum awVI_DATA_WIDTH_E {
    VI_8BIT = 0,
    VI_10BIT = 1,
    VI_12BIT = 2,
    VI_16BIT = 3,
} VI_DATA_WIDTH_E;

/*
 * input data format
 */
typedef enum awVI_INPUT_FMT_E {
    VI_RAW = 0,         /* raw stream  */
    VI_BAYER = 1,       /* byer rgb242 */
    VI_CCIR656 = 2,     /* ccir656     */
    VI_YUV422 = 3,      /* yuv422      */
    VI_YUV420 = 4,      /* yuv420      */
    VI_YUV422_16 = 4,   /* yuv422 16 bit */
    VI_YUV444 = 4,      /* yuv444 24 bit */
    VI_CCIR656_2CH = 5, /* ccir656 2 channel */
    VI_CCIR656_4CH = 7, /* ccir656 4 channel */
} VI_INPUT_FMT_E;

/*
 * output data format
 */
typedef enum awVI_OUTPUT_FMT_E {
    /* only when input is raw */
    VI_FIELD_RAW_8 = 0,
    VI_FIELD_RAW_10 = 1,
    VI_FIELD_RAW_12 = 2,
    VI_FIELD_RGB565 = 4,
    VI_FIELD_RGB888 = 5,
    VI_FIELD_PRGB888 = 6,
    VI_FRAME_RAW_8 = 8,
    VI_FRAME_RAW_10 = 9,
    VI_FRAME_RAW_12 = 10,
    VI_FRAME_RGB565 = 12,
    VI_FRAME_RGB888 = 13,
    VI_FRAME_PRGB888 = 14,

    /* only when input is bayer */
    VI_PLANAR_RGB242 = 0, /* planar rgb242 */

    /* only when input is ccir656 */
    VI_FIELD_PLANAR_YUV422 = 0, /* parse a field(odd or even) into planar yuv420 */
    VI_FIELD_PLANAR_YUV420 = 1, /* parse a field(odd or even) into planar yuv420 */
    VI_FRAME_PLANAR_YUV420 = 2,
    VI_FRAME_PLANAR_YUV422 = 3,
    VI_FIELD_UV_CB_YUV422 =
      4, /* parse and reconstruct evry 2 fields(odd and even) into a frame, format is planar yuv420 */
    VI_FIELD_UV_CB_YUV420 = 5,
    VI_FRAME_UV_CB_YUV420 = 6,
    VI_FRAME_UV_CB_YUV422 = 7,
    VI_FIELD_MB_YUV422 = 8,
    VI_FIELD_MB_YUV420 = 9,
    VI_FRAME_MB_YUV422 = 10,
    VI_FRAME_MB_YUV420 = 11,
    VI_FIELD_UV_CB_YUV422_10 = 12,
    VI_FIELD_UV_CB_YUV420_10 = 13,
    VI_INTLC_INTLV_YUV422 = 15,

    /* only when input is yuv422 */
    VI_PLANAR_YUV422 = 0, /* parse yuv422 into planar yuv422 */
    VI_PLANAR_YUV420 = 1, /* parse yuv422 into planar yuv420 */
    VI_UV_CB_YUV422 = 4,
    VI_UV_CB_YUV420 = 5,
    VI_MB_YUV422 = 8,
    VI_MB_YUV420 = 9,

    /* only when input is yuv444 */
    VI_FIELD_PLANAR_YUV444 = 0,       /* parse a field(odd or even) into planar yuv444 */
    VI_FIELD_UV_CB_YUV444_YUV422 = 1, /* parse a field(odd or even) into planar yuv422 */
    VI_FRAME_PLANAR_YUV444 = 2,
    VI_FRAME_UV_CB_YUV444_YUV422 = 3,
} VI_OUTPUT_FMT_E;

/*
 * field sequenc or polarity
 */
typedef enum awVI_FIELD_E {
    /* For Embedded Sync timing*/
    VI_FIELD_TF = 0, /* top filed first */
    VI_FIELD_BF = 1, /* bottom field first */

    /* For External Sync timing */
    VI_FIELD_NEG = 0, /* field negtive indicates odd field */
    VI_FIELD_POS = 1, /* field postive indicates odd field */
} VI_FIELD_E;

/*
 * input field selection, only when input is ccir656
 */
typedef enum awVI_FIELD_SEL_E {
    VI_ODD,    /* odd field */
    VI_EVEN,   /* even field */
    VI_EITHER, /* either field */
} VI_FIELD_SEL_E;

/*
 * input source type
 */
typedef enum awVI_SRC_TYPE_E {
    VI_PROGRESSIVE = 0, /* progressive */
    VI_INTERLACE = 1,   /* interlace */
} VI_SRC_TYPE_E;

/*
 * input data sequence
 */
typedef enum awVI_INPUT_SEQ_E {
    /* only when input is yuv422 */
    VI_YUYV = 0,
    VI_YVYU,
    VI_UYVY,
    VI_VYUY,

    /* only when input is byer */
    VI_RGRG = 0, /* first line sequence is RGRG... */
    VI_GRGR,     /* first line sequence is GRGR... */
    VI_BGBG,     /* first line sequence is BGBG... */
    VI_GBGB,     /* first line sequence is GBGB... */
} VI_INPUT_SEQ_E;

/*
 * input reference signal polarity
 */
typedef enum awVI_REF_POL_E {
    VI_LOW,  /* active low */
    VI_HIGH, /* active high */
} VI_REF_POL_E;

/*
 * input data valid of the input clock edge type
 */
typedef enum awVI_EDGE_POL_E {
    VI_FALLING, /* active falling */
    VI_RISING,  /* active rising */
} VI_EDGE_POL_E;

/*
 * vi interface configuration
 */
typedef struct awVI_INTF_CFG_S {
    VI_SRC_TYPE_E src_type;     /* interlaced or progressive */
    VI_DATA_WIDTH_E data_width; /* vi data width */
    VI_INTF_E interface;        /* vi interface */
} VI_INTF_CFG_S;

/*
 * vi timing configuration
 */
typedef struct awVI_TIMING_CFG_S {
    VI_FIELD_E field;     /* top or bottom field first / field polarity */
    VI_REF_POL_E vref;    /* input vref signal polarity */
    VI_REF_POL_E href;    /* input href signal polarity */
    VI_EDGE_POL_E sample; /* input data valid of the input clock edge type */
} VI_TIMING_CFG_S;

typedef struct awVI_CROP_RECT_S {
    AW_U32 u32LengthH;
    AW_U32 u32LengthV;
    AW_U32 u32LengthY;
    AW_U32 u32LengthC;
    AW_U32 u32StartH;
    AW_U32 u32StartV;
} VI_CROP_RECT_S;

/*
 * vi mode configuration
 */
typedef struct awVI_FMT_CFG_S {
    VI_INPUT_FMT_E input_fmt;   /* input data format */
    VI_OUTPUT_FMT_E output_fmt; /* output data format */
    VI_FIELD_SEL_E field_sel;   /* input field selection */
    VI_INPUT_SEQ_E input_seq;   /* input data sequence */
    VI_DATA_WIDTH_E data_width; /* vi data width */
    VI_CROP_RECT_S stCropRect;  /* vi Crop config */
} VI_FMT_CFG_S;

typedef struct awVI_DEV_ATTR_S {
    VI_INTF_CFG_S stIntfCfg;
    VI_FMT_CFG_S stFmtCfg;
    VI_TIMING_CFG_S stTimingCfg;
} VI_DEV_ATTR_S;
/*
 * vi buffer
 */

typedef enum awVI_BUF_SEL_E {
    VI_BUF_0_A = 0, /* FIFO for Y address A */
    VI_BUF_0_B,     /* FIFO for Y address B */
    VI_BUF_1_A,     /* FIFO for Cb address A */
    VI_BUF_1_B,     /* FIFO for Cb address B */
    VI_BUF_2_A,     /* FIFO for Cr address A */
    VI_BUF_2_B,     /* FIFO for Cr address B */
} VI_BUF_SEL_E;

/*
 * vi buffer configs
 */
typedef struct awVI_BUFFER_CFG_S {
    VI_BUF_SEL_E enBufSel;
    AW_U64 u64Addr;
} VI_BUFFER_CFG_S;

/*
 * vi capture status
 */
typedef struct awVI_CAPTURE_STATUS_S {
    AW_BOOL picture_in_progress;
    AW_BOOL video_in_progress;
} VI_CAPTURE_STATUS_S;

typedef enum awVI_CAP_MODE_E {
    VI_SCAP = 1,
    VI_VCAP,
} VI_CAP_MODE_E;

/*
 * vi interrupt
 */
typedef enum awVI_INT_SEL_E {
    VI_INT_CAPTURE_DONE = 0X1,
    VI_INT_FRAME_DONE = 0X2,
    VI_INT_BUF_0_OVERFLOW = 0X4,
    VI_INT_BUF_1_OVERFLOW = 0X8,
    VI_INT_BUF_2_OVERFLOW = 0X10,
    VI_INT_PROTECTION_ERROR = 0X20,
    VI_INT_HBLANK_OVERFLOW = 0X40,
    VI_INT_VSYNC_TRIG = 0X80,
    VI_INT_ALL = 0XFF,
} VI_INT_SEL_E;

typedef struct awANTISHAKE_KMAT_S {
    float KmatK1;
    float KmatK2;
    float KmatKx;
    float KmatKy;
} ANTISHAKE_KMAT_S;

typedef struct awANTISHAKE_ATTR_S {
    int frame_width;
    int frame_height;
    int frame_width_strie;
    int frame_height_strie;
    int number_of_input_buffers;
    int number_of_output_buffers;
    int operation_mode;
    int rq;
    int style;
    /*
    * we got default k-mat values for imx317 in SDK.
    * but if you have other sensors, you should set
    * <use_custom_kmat=1>,and fill <antishake_kmat>.
    * the important thing is: you must be responsible for your own values.
    */
    bool use_custom_kmat;
    ANTISHAKE_KMAT_S antishake_kmat;
} ANTISHAKE_ATTR_S;

/*
* shut time mode enum
*/
typedef enum awVI_SHUTTIME_MODE_E {
    VI_SHUTTIME_MODE_AUTO = 0,   /* fix fps to last current normal value */
    VI_SHUTTIME_MODE_PREVIEW,    /* same fps than AUTO, but has smaller exposure time */
    VI_SHUTTIME_MODE_NIGHT_VIEW, /* more than 1S time interval */
} VI_SHUTTIME_MODE_E;

/*
*@VI_SHUTTIME_RESET_MANUAL: user should reset the exposure time into normal. do not use it now.
*@VI_SHUTTIME_RESET_AUTO_NOW: user don't care reset action, someone else will do it.
*   reset action will be executed immediately after long exposure was set. sensor like imx317
*@VI_SHUTTIME_RESET_AUTO_DELAY: same as the previous one, but reset action will be
*   executed after get the long exposure video frame. adapt sensor like imx278
*/
typedef enum awVI_SHUTTIME_RESET_E {
    VI_SHUTTIME_RESET_AUTO_DELAY = 0,
//    VI_SHUTTIME_RESET_MANUAL,
    VI_SHUTTIME_RESET_AUTO_NOW,
} VI_SHUTTIME_RESET_E;

/*
* vi shutter time configuration
*@iTime: frame interval:[1/500S]->[iTime=500] [1/125S]->[iTime=125]
*                       [1S]->[iTime=-1]      [5S]->[iTime=-5]
*@iExpValue: not use until now
*@iGainValue: not use until now
*@bResetAuto: if this value is set, user needn't care auto resume the shutter time.
*@iShutterMode:
*       [VI_SHUTTIME_MODE_AUTO]: auto shutter(30fps,auto exp_abs)
*       [VI_SHUTTIME_MODE_PREVIEW]: preview mode(fps>=30)
*       [VI_SHUTTIME_MODE_NIGHT_VIEW]: night view mode(fps<30)
*/
typedef struct awVI_SHUTTIME_CFG_S {
    int iTime;
    int iExpValue;
    int iGainValue;
    VI_SHUTTIME_RESET_E eResetMode;
    VI_SHUTTIME_MODE_E  eShutterMode;
} VI_SHUTTIME_CFG_S;

typedef struct awVI_ATTR_S {
    enum v4l2_buf_type type;
    enum v4l2_memory memtype;
    struct v4l2_pix_format_mplane format;
    unsigned int nbufs;
    unsigned int nplanes;
    unsigned int fps;
    unsigned int capturemode;   //V4L2_MODE_VIDEO
    unsigned int use_current_win;   //0:config ISP param again; 1:use current ISP param
    unsigned int wdr_mode;
    unsigned int drop_frame_num; // drop frames number after enable vipp device(default 0).

} VI_ATTR_S;

struct rect {
    AW_U32 left;
    AW_U32 top;
    AW_U32 width;
    AW_U32 height;
};

#if 0
typedef struct awVI_OsdMaskRegion {
    int clipcount; /* number of clips */
    int chromakey;  //V4L2_PIX_FMT_RGB32 in mode overlay; color value in mode cover.
    int global_alpha;
    void *bitmap[64];   //!=NULL:overlay; NULL:cover
    struct rect region[64]; /* overlay or cover win */
} VI_OsdMaskRegion;
#endif

typedef struct awVI_Params {
    VI_DEV mDev;
    VI_CHN mChn;
    VI_ATTR_S *pstAttr;
	VIDEO_FRAME_INFO_S  *pstFrameInfo;
    AW_S32 s32MilliSec;

    //AW_U32 mOnOff;
    //VI_OsdMaskRegion *pstOsdMaskRegion;
} VI_Params;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*_AW_COMMON_VI_H_*/

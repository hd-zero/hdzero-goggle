/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_vdec.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/04/20
  Last Modified :
  Description   : common struct definition for VDEC
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_COMM_VDEC_H__
#define __IPCLINUX_COMM_VDEC_H__
#include "mm_comm_video.h"
#include "mm_common.h"
#include "plat_defines.h"
#include "plat_errno.h"
#include "plat_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

/*vdec channel attr(user interface)*/

//#define AW_IO_BLOCK   HI_TRUE
//#define AW_IO_NOBLOCK HI_FALSE

typedef struct VDEC_CHN_LUM_S {
    uint64_t mLumPixSum;         /* Luma sum of current frame */
    unsigned int mLumPixAverage; /* Luma average of current frame */
    uint64_t mPts;               /* PTS of current frame ?? */
} VDEC_CHN_LUM_S;

typedef enum VIDEO_MODE_E {
    VIDEO_MODE_STREAM = 0, /*send by stream*/
    VIDEO_MODE_FRAME,      /*send by frame*/
    VIDEO_MODE_BUTT
} VIDEO_MODE_E;

typedef struct VDEC_ATTR_JPEG_S {
    VIDEO_MODE_E enMode; /* video stream mode select */
} VDEC_ATTR_JPEG_S, *PTR_VDEC_ATTR_JPEG_S;

typedef struct VDEC_ATTR_VIDEO_S {
    unsigned int mRefFrameNum; /*ref pic num [1,16]*/
    VIDEO_MODE_E mMode;        /*send by stream or by frame*/
    int
      mSupportBFrame; /*enable chnl to support BFrame decode, 0: disable support BFrame decode,1: enable support BFrame*/
} VDEC_ATTR_VIDEO_S, *PTR_VDEC_ATTR_VIDEO_S;

typedef struct VDEC_CHN_ATTR_S {
    PAYLOAD_TYPE_E mType;    /* video type to be decoded */
    unsigned int mBufSize;   /* stream buf size(Byte) */
    unsigned int mPriority;  /* priority */
    unsigned int mPicWidth;  /* max pic width */
    unsigned int mPicHeight; /* max pic height */
    ROTATE_E mInitRotation;  //clockwise rotation: val=0 no rotation, val=1 90 degree; val=2 180 degree, val=3 270 degree
    PIXEL_FORMAT_E mOutputPixelFormat;
    BOOL mSubPicEnable;        //support second picture flag, now just for mjpeg,20180103 
    int mSubPicWidthRatio; // val = 0 : 1; 1 : 1/2; 2 : 1/4; 3 : 1/8; 4 : 1/16 
    int mSubPicHeightRatio;//[0 4],
    PIXEL_FORMAT_E mSubOutputPixelFormat; //sub channel output  pixelformat
    union {
        VDEC_ATTR_JPEG_S mVdecJpegAttr;   /* structure with jpeg or mjpeg type  */
        VDEC_ATTR_VIDEO_S mVdecVideoAttr; /* structure with video ( h264/mpeg4) */
    };
    int mnFrameBufferNum;   /* set frame number which is malloc by vdeclib, 0 means any number. only valid to jpeg.*/
    int mExtraFrameNum; /* let vdeclib malloc more frame buffer base on initial frame number.*/
} VDEC_CHN_ATTR_S;

typedef struct VDEC_STREAM_S {
    unsigned char* pAddr; /* stream address */
    unsigned int mLen;    /* stream len */
    uint64_t mPTS;        /* time stamp */
    BOOL mbEndOfFrame;    /* is the end of a frame */
    BOOL mbEndOfStream;   /* is the end of all stream */
} VDEC_STREAM_S;

typedef struct VDEC_USERDATA_S {
    unsigned char* pu8Addr;  /* userdata data vir address */
    unsigned int u32PhyAddr; /* userdata data phy address */
    unsigned int u32Len;     /* userdata data len */
    BOOL bValid;             /* is valid? */
} VDEC_USERDATA_S;

typedef struct VDEC_DECODE_FRAME_PARAM_S
{
    int mDecodeFrameNum;
}VDEC_DECODE_FRAME_PARAM_S;

typedef struct VDEC_DECODE_ERROR_S {
    int mFormatErr;        /* format error. eg: do not support filed */
    int mPicSizeErrSet;    /* picture width or height is larger than chnnel width or height*/
    int mStreamUnsprt;     /* unsupport the stream specification */
    int mPackErr;          /* stream package error */
    int mPrtclNumErrSet;   /* protocol num is not enough. eg: slice, pps, sps */
    int mRefErrSet;        /* refrence num is not enough */
    int mPicBufSizeErrSet; /* the buffer size of picture is not enough */
} VDEC_DECODE_ERROR_S;

typedef struct VDEC_CHN_STAT_S {
    PAYLOAD_TYPE_E mType;             /* video type to be decoded */
    unsigned int mLeftStreamBytes;    /* left stream bytes waiting for decode */
    unsigned int mLeftStreamFrames;   /* left frames waiting for decode,only valid for H264D_MODE_FRAME */
    unsigned int mLeftPics;           /* pics waiting for output */
    BOOL mbStartRecvStream;           /* had started recv stream? */
    unsigned int mRecvStreamFrames;   /* how many frames of stream has been received. valid when send by frame. */
    unsigned int mDecodeStreamFrames; /* how many frames of stream has been decoded. valid when send by frame. */
    unsigned int mLeftDecodeStreamFrames;   /* Number of frames to be decoded. This member is valid after AW_MPI_VDEC_StartRecvStreamEx is called.*/
    VDEC_DECODE_ERROR_S mVdecDecErr;  /* information about decode error */
} VDEC_CHN_STAT_S;

/*
 * static parameter: must set after stop sending stream and all stream is decoded.
 * dynamic parameter: can be set at any time.
 */
typedef struct VDEC_CHN_PARAM_S {
    int mChanErrThr; /* threshold for stream error process, 0: discard with any error, 100 : keep data with any error */
    int mChanStrmOFThr; /* threshold for stream overflow, 0~ , 0: nothing to do when stream is overflow */
    int mDecMode; /* decode mode , 0: deocde IPB frames, 1: only decode I frame & P frame , 2: only decode I frame */
    int mDecOrderOutput; /* frames output order ,0: the same with display order , 1: the same with decoder order */
    VIDEO_FORMAT_E mVideoFormat;
    COMPRESS_MODE_E mCompressMode;
} VDEC_CHN_PARAM_S;

typedef struct VDEC_PRTCL_PARAM_S {
    int mMaxSliceNum;     /* max  slice num support */
    int mMaxSpsNum;       /* max sps num support */
    int mMaxPpsNum;       /* max pps num support */
    int mDisplayFrameNum; /* display frame num */
} VDEC_PRTCL_PARAM_S;

//structure & varibale for jpegd decoder lib
/*********************************************************************************************************/
/* stream packet for jpeg */
typedef struct JPEGD_STREAM_PACKET_S {
    unsigned int PhyAddr[2];
    void* VirAddr[2];
    unsigned int Length[2];
    uint64_t Pts;
    unsigned int Index;

} USR_STREAM_PACKET_S;

/* description structure for memory record */
typedef struct USR_MEM_DESC_S {
    unsigned int u32PhyAddr;
    unsigned int u32Length;
} USR_MEM_DESC_S;

typedef enum JPEGD_DECODE_ERROR_E {
    JPEGD_ERR_NONE = 0,
    JPEGD_ERR_STREAM,
    JPEGD_ERR_PIC_SIZE,
    JPEGD_ERR_PICBUF_SIZE,
    JPEGD_ERR_FORMAT,
    VIDEO_ERR_BULL
} JPEGD_DECODE_ERROR_E;

typedef enum VDEC_JPG_COLOR_FMT_E {
    JPG_COLOR_FMT_YCBCR400,     /*YUV400*/
    JPG_COLOR_FMT_YCBCR420 = 3, /*YUV420*/
    JPG_COLOR_FMT_YCBCR422BHP,  /*YUV 422 2x1*/
    JPG_COLOR_FMT_YCBCR422BVP,  /*YUV 422 1x2*/
    JPG_COLOR_FMT_YCBCR444,     /*YUV 444*/
    JPG_COLOR_FMT_BUTT
} VDEC_JPG_COLOR_FMT_E;

typedef struct {
    VDEC_JPG_COLOR_FMT_E enFmt; /* YUV format */

    int s32IsFrameValid;
    uint64_t u64LuPixSum;
    uint64_t Pts;
    int s32IsSemiPlanar;

    int s32YWidth;
    int s32YHeight;

    int s32LumaPhyAddr;
    int s32LumaStride;

    int
      s32CbPhyAddr; /* semi-planar: this variable is PhyAddr of interlaced chroma; planar: this variable is PhyAddr of Cb */
    int s32CrPhyAddr; /* semi-planar: this variable is not value ; planar: this variable is PhyAddr of Cr */
    int s32ChromStride;
    int s32ChromCrStride;
} VDEC_FRAME_DESC_S;

/************************************************************************************************************************/

/* invlalid channel ID */
#define ERR_VDEC_INVALID_CHNID DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_VDEC_ILLEGAL_PARAM DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_VDEC_EXIST DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* using a NULL point */
#define ERR_VDEC_NULL_PTR DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define ERR_VDEC_NOT_CONFIG DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define ERR_VDEC_NOT_SURPPORT DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define ERR_VDEC_NOT_PERM DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the channle is not existed  */
#define ERR_VDEC_UNEXIST DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define ERR_VDEC_NOMEM DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_VDEC_NOBUF DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define ERR_VDEC_BUF_EMPTY DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define ERR_VDEC_BUF_FULL DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define ERR_VDEC_SYS_NOTREADY DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/*system busy*/
#define ERR_VDEC_BUSY DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* component state is same as user wanted */
#define ERR_VDEC_SAMESTATE DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_VDEC_INVALIDSTATE DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_VDEC_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_VDEC_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)

/* bad address,  eg. used for copy_from_user & copy_to_user   */
#define ERR_VDEC_BADADDR DEF_ERR(MOD_ID_VDEC, EN_ERR_LEVEL_ERROR, EN_ERR_BADADDR)

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __IPCLINUX_COMM_VDEC_H__ */

/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_mux.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/04/27
  Last Modified :
  Description   : common struct definition for MUX
  Function List :
  History       :
******************************************************************************/
#ifndef  __IPCLINUX_COMM_MUX_H__
#define  __IPCLINUX_COMM_MUX_H__
#include "plat_type.h"
#include "mm_common.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "FsWriter.h"

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

/*Define attributes of mux GROUP*/
typedef struct MUX_GRP_ATTR_S
{
    // video 
    int mHeight;
    int mWidth;
    int mVideoFrmRate;  // *1000
    int mCreateTime;
    int mMaxKeyInterval;
    PAYLOAD_TYPE_E mVideoEncodeType;   //VENC_CODEC_H264
    int mRotateDegree;  //0, 90, 180, 270

    // audio
    int mChannels;
    int mBitsPerSample; 
    int mSamplesPerFrame; //sample_cnt_per_frame
    int mSampleRate;    
    PAYLOAD_TYPE_E mAudioEncodeType;  //AUDIO_ENCODER_AAC_TYPE

    // text
    PAYLOAD_TYPE_E mTextEncodeType;  
}MUX_GRP_ATTR_S;

typedef struct MUX_GRP_PARAM_S
{
    int xxx;   //
}MUX_GRP_PARAM_S;

/*Define attributes of mux channel*/
typedef struct MUX_CHN_ATTR_S
{
    int mMuxerId;
    MEDIA_FILE_FORMAT_E mMediaFileFormat;
    int64_t mMaxFileDuration;   //unit:ms
    int64_t mMaxFileSizeBytes;  //unit:byte
    int     mFallocateLen;
    BOOL    mCallbackOutFlag;   //send data through callback.
    FSWRITEMODE mFsWriteMode;
    int         mSimpleCacheSize;
    BOOL        bBufFromCacheFlag;
}MUX_CHN_ATTR_S;

/*Define detailed params for mux channel*/
typedef struct MUX_CHN_PARAM_S
{
    int xxx;
} MUX_CHN_PARAM_S;

typedef struct CdxFdT
{
    int mFd;
    int mnFallocateLen;
    int mIsImpact;
    int mMuxerId;
}CdxFdT;

typedef struct ShutDownType
{
    int mMuxerId;   //-1:for all muxerId
    BOOL mbShutDownNowFlag;
}ShutDownType;

/************************************************************************************************************************/

/* invlalid channel ID */
#define ERR_MUX_INVALID_CHNID     DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_MUX_ILLEGAL_PARAM     DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_MUX_EXIST             DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* using a NULL point */
#define ERR_MUX_NULL_PTR          DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define ERR_MUX_NOT_CONFIG        DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define ERR_MUX_NOT_SUPPORT      DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define ERR_MUX_NOT_PERM          DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the channle is not existed  */
#define ERR_MUX_UNEXIST           DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define ERR_MUX_NOMEM             DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_MUX_NOBUF             DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* system is not ready,had not initialed or loaded*/
#define ERR_MUX_SYS_NOTREADY      DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/*system busy*/
#define ERR_MUX_BUSY			    DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* timeout */
#define ERR_MUX_TIMEOUT             DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_TIMEOUT)
/* component state is same as user wanted */
#define ERR_MUX_SAMESTATE         DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_MUX_INVALIDSTATE      DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_MUX_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_MUX_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_MUX, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __IPCLINUX_COMM_MUX_H__ */


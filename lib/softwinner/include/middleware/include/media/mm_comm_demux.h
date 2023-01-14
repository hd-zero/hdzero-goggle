/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_demux.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/04/25
  Last Modified :
  Description   : common struct definition for DEMUX
  Function List :
  History       :
******************************************************************************/
#ifndef  __IPCLINUX_COMM_DEMUX_H__
#define  __IPCLINUX_COMM_DEMUX_H__
#include "plat_type.h"
#include "mm_common.h"
#include "plat_errno.h"
#include "mm_comm_video.h"
#include "plat_defines.h"

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

// ref to cedarx_stream.h
typedef enum STREAMTYPE_E{
    STREAMTYPE_NETWORK,
    STREAMTYPE_LOCALFILE,
}STREAMTYPE_E;

typedef enum SOURCETYPE_E{
    SOURCETYPE_FD,
    SOURCETYPE_FILEPATH,
    SOURCETYPE_WRITER_CALLBACK = 6, //for recoder writer
}SOURCETYPE_E;

typedef enum CEDARX_MEDIA_TYPE{
	CEDARX_MEDIATYPE_NORMAL = 0 ,
	CEDARX_MEDIATYPE_RAWMUSIC   ,
	CEDARX_MEDIATYPE_3D_VIDEO   ,
	CEDARX_MEDIATYPE_DRM_VIDEO ,
	CEDARX_MEDIATYPE_DRM_WVM_VIDEO ,
	CEDARX_MEDIATYPE_DRM_ES_BASED_VIDEO,
	CEDARX_MEDIATYPE_DRM_CONTAINNER_BASED_VIDEO,
	CEDARX_MEDIATYPE_BD,
	CEDARX_SOURCE_MULTI_URL,
}CEDARX_MEDIA_TYPE;



#define MAX_AUDIO_STREAM_NUM (32)
#define MAX_VIDEO_STREAM_NUM (1)

typedef enum DEMUX_DISABLE_TRACKINFO {
	DEMUX_DISABLE_AUDIO_TRACK    = 0x01,
	DEMUX_DISABLE_VIDEO_TRACK    = 0x02,
	DEMUX_DISABLE_SUBTITLE_TRACK = 0x04,
} DEMUX_DISABLE_TRACKINFO;

typedef struct DEMUX_CHN_ATTR_S
{
    STREAMTYPE_E mStreamType;
    SOURCETYPE_E mSourceType;
	char* mSourceUrl;
    int mFd;
    int mDemuxDisableTrack;    //DEMUX_DISABLE_AUDIO_TRACK
}DEMUX_CHN_ATTR_S;

#define MAX_LANG_CHAR_SIZE (64)
typedef struct DEMUX_VIDEO_STREAM_INFO_S
{
    PAYLOAD_TYPE_E   mCodecType;
    int   mWidth;   //display width
    int   mHeight;
    int   mFrameRate;   // x1000
    int   mAvgBitsRate;
    int   mMaxBitsRate;
    int   nCodecSpecificDataLen;
    char* pCodecSpecificData;
}DEMUX_VIDEO_STREAM_INFO_S;


typedef struct DEMUX_AUDIO_STREAM_INFO_S
{
    PAYLOAD_TYPE_E mCodecType;
    int mChannelNum;
    int mBitsPerSample;
    int mSampleRate;
    int mAvgBitsRate;
    int mMaxBitsRate;
    unsigned char strLang[MAX_LANG_CHAR_SIZE];
}DEMUX_AUDIO_STREAM_INFO_S;

typedef struct DEMUX_SUBTITLE_STREAM_INFO_S
{
    int xxx;
    unsigned char strLang[MAX_LANG_CHAR_SIZE];
}DEMUX_SUBTITLE_STREAM_INFO_S;

#define DEMUX_MAX_AUDIO_STREAM_NUM 32
#define DEMUX_MAX_VIDEO_STREAM_NUM 1
#define DEMUX_MAX_SUBTITLE_STREAM_NUM 32
typedef struct DEMUX_MEDIA_INFO_S
{
    unsigned int mFileSize;
    unsigned int mDuration; //unit: ms
    int mAudioNum, mAudioIndex;
    int mVideoNum, mVideoIndex;
    int mSubtitleNum, mSubtitleIndex;

    DEMUX_AUDIO_STREAM_INFO_S    mAudioStreamInfo[DEMUX_MAX_AUDIO_STREAM_NUM];
    DEMUX_VIDEO_STREAM_INFO_S    mVideoStreamInfo[DEMUX_MAX_VIDEO_STREAM_NUM];
    DEMUX_SUBTITLE_STREAM_INFO_S mSubtitleStreamInfo[DEMUX_MAX_SUBTITLE_STREAM_NUM];
}DEMUX_MEDIA_INFO_S;

/************************************************************************************************************************/

/* invlalid channel ID */
#define ERR_DEMUX_INVALID_CHNID     DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_DEMUX_ILLEGAL_PARAM     DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_DEMUX_EXIST             DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* using a NULL point */
#define ERR_DEMUX_NULL_PTR          DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define ERR_DEMUX_NOT_CONFIG        DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported currently */
#define ERR_DEMUX_NOT_SUPPORT       DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change state attribute */
#define ERR_DEMUX_NOT_PERM          DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the channle is not existed  */
#define ERR_DEMUX_UNEXIST           DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define ERR_DEMUX_NOMEM             DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_DEMUX_NOBUF             DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* system is not ready,had not initialed or loaded*/
#define ERR_DEMUX_SYS_NOTREADY      DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/*system busy*/
#define ERR_DEMUX_BUSY			    DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* component state is same as user wanted */
#define ERR_DEMUX_SAMESTATE         DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_DEMUX_INVALIDSTATE      DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_DEMUX_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_DEMUX_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)
/* media file exception */
#define ERR_DEMUX_FILE_EXCEPTION    DEF_ERR(MOD_ID_DEMUX, EN_ERR_LEVEL_ERROR, EN_ERR_FILE_EXCEPTION)


#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __IPCLINUX_COMM_DEMUX_H__ */


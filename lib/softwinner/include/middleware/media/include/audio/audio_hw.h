/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : audio_hw.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/05/26
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef __AUDIO_HW_H__
#define __AUDIO_HW_H__

//ref platform headers
#include <plat_defines.h>
#include <plat_errno.h>
#include <plat_math.h>
#include <plat_type.h>

//media api headers to app
#include <alsa_interface.h>
#include <mm_comm_aio.h>
#include <mm_common.h>
#include <mm_component.h>
#include <tmessage.h>
#include <tsemaphore.h>

#define AIO_DEV_MAX_NUM (1)

typedef enum AI_CHANNEL_PORT_DEFINE_E {
    AI_CHN_PORT_INDEX_CAP_IN = 0,
    AI_CHN_PORT_INDEX_AO_IN,
    AI_CHN_PORT_INDEX_OUT_AENC,
    AI_CHN_PORT_INDEX_OUT_AO,
    AI_CHN_MAX_PORTS,
} AI_CHANNEL_PORT_DEFINE_E;

typedef enum AO_CHANNEL_PORT_DEFINE_E
{
    AO_CHN_PORT_INDEX_IN_CLK = 0,
    AO_CHN_PORT_INDEX_IN_PCM,
    AO_CHN_PORT_INDEX_OUT_PLAY,
    AO_CHN_PORT_INDEX_OUT_AI,
    AO_CHN_MAX_PORTS,
} AO_CHANNEL_PORT_DEFINE_E;

#define CHECK_AI_DEV_ID(id)                         \
    do {                                            \
        if (id < 0 || id > (AIO_DEV_MAX_NUM - 1)) { \
            aloge("Invalid AudioDevId %d!", id);    \
            return ERR_AI_INVALID_DEVID;            \
        }                                           \
    } while (0)

#define CHECK_AI_CHN_ID(id)                         \
    do {                                            \
        if (id < 0 || id > (AIO_MAX_CHN_NUM - 1)) { \
            aloge("Invalid AI channel ID %d!", id); \
            return ERR_AI_INVALID_CHNID;            \
        }                                           \
    } while (0)

#define CHECK_AO_DEV_ID(id)                         \
    do {                                            \
        if (id < 0 || id > (AIO_DEV_MAX_NUM - 1)) { \
            aloge("Invalid AudioDevId %d!", id);    \
            return ERR_AO_INVALID_DEVID;            \
        }                                           \
    } while (0)

#define CHECK_AO_CHN_ID(id)                         \
    do {                                            \
        if (id < 0 || id > (AIO_MAX_CHN_NUM - 1)) { \
            aloge("Invalid AI channel ID %d!", id); \
            return ERR_AO_INVALID_CHNID;            \
        }                                           \
    } while (0)

typedef struct AI_CHANNEL_S {
    AUDIO_DEV mDevId;
    AI_CHN mId;
    MM_COMPONENTTYPE *mpComp;
    cdx_sem_t mSemCompCmd;
    MPPCallbackInfo mCallbackInfo;
    struct list_head mList;
} AI_CHANNEL_S;

typedef struct AO_CHANNEL_S {
    AO_CHN mId;
    MM_COMPONENTTYPE *mpComp;
    cdx_sem_t mSemCompCmd;
    MPPCallbackInfo  mCallbackInfo;
    struct list_head mList;
} AO_CHANNEL_S;

#ifdef __cplusplus
extern "C"
{
#endif

ERRORTYPE audioHw_Construct(void);
ERRORTYPE audioHw_Destruct(void);

ERRORTYPE audioHw_AI_Dev_lock(AUDIO_DEV AudioDevId);
ERRORTYPE audioHw_AI_Dev_unlock(AUDIO_DEV AudioDevId);
ERRORTYPE audioHw_AI_searchChannel_l(AUDIO_DEV AudioDevId, AI_CHN AiChn, AI_CHANNEL_S **pChn);
ERRORTYPE audioHw_AI_searchChannel(AUDIO_DEV AudioDevId, AI_CHN AiChn, AI_CHANNEL_S **pChn);
ERRORTYPE audioHw_AI_AddChannel_l(AUDIO_DEV AudioDevId, AI_CHANNEL_S *pChn);
ERRORTYPE audioHw_AI_AddChannel(AUDIO_DEV AudioDevId, AI_CHANNEL_S *pChn);
ERRORTYPE audioHw_AI_RemoveChannel(AUDIO_DEV AudioDevId, AI_CHANNEL_S *pChn);
MM_COMPONENTTYPE *audioHw_AI_GetChnComp(PARAM_IN MPP_CHN_S *pMppChn);
ERRORTYPE audioHw_AI_SetPubAttr(AUDIO_DEV AudioDevId, const AIO_ATTR_S *pstAttr);
ERRORTYPE audioHw_AI_GetPubAttr(AUDIO_DEV AudioDevId, AIO_ATTR_S *pstAttr);
ERRORTYPE audioHw_AI_ClrPubAttr(AUDIO_DEV AudioDevId);
ERRORTYPE audioHw_AI_Enable(AUDIO_DEV AudioDevId);
ERRORTYPE audioHw_AI_Disable(AUDIO_DEV AudioDevId);
ERRORTYPE audioHw_AI_SetTrackMode(AUDIO_DEV AudioDevId, AUDIO_TRACK_MODE_E enTrackMode);
ERRORTYPE audioHw_AI_GetTrackMode(AUDIO_DEV AudioDevId, AUDIO_TRACK_MODE_E *penTrackMode);
ERRORTYPE audioHw_AI_GetPcmConfig(AUDIO_DEV AudioDevId, PCM_CONFIG_S **ppCfg);
ERRORTYPE audioHw_AI_GetAIOAttr(AUDIO_DEV AudioDevId, AIO_ATTR_S **ppAttr);
ERRORTYPE audioHw_AI_SetVolume(AUDIO_DEV AudioDevId, int s32VolumeDb);
ERRORTYPE audioHw_AI_GetVolume(AUDIO_DEV AudioDevId, int *ps32VolumeDb);
ERRORTYPE audioHw_AI_SetMute(AUDIO_DEV AudioDevId, int bEnable);
ERRORTYPE audioHw_AI_GetMute(AUDIO_DEV AudioDevId, int *pbEnable);
BOOL audioHw_AI_IsDevStarted(AUDIO_DEV AudioDevId);

ERRORTYPE audioHw_AO_Dev_lock(AUDIO_DEV AudioDevId);
ERRORTYPE audioHw_AO_Dev_unlock(AUDIO_DEV AudioDevId);
ERRORTYPE audioHw_AO_searchChannel_l(AUDIO_DEV AudioDevId, AO_CHN AoChn, AO_CHANNEL_S **pChn);
ERRORTYPE audioHw_AO_searchChannel(AUDIO_DEV AudioDevId, AO_CHN AoChn, AO_CHANNEL_S **pChn);
ERRORTYPE audioHw_AO_AddChannel_l(AUDIO_DEV AudioDevId, AO_CHANNEL_S *pChn);
ERRORTYPE audioHw_AO_AddChannel(AUDIO_DEV AudioDevId, AO_CHANNEL_S *pChn);
ERRORTYPE audioHw_AO_RemoveChannel(AUDIO_DEV AudioDevId, AO_CHANNEL_S *pChn);
MM_COMPONENTTYPE *audioHw_AO_GetChnComp(PARAM_IN MPP_CHN_S *pMppChn);
ERRORTYPE AudioHw_AO_SetPubAttr(AUDIO_DEV AudioDevId, const AIO_ATTR_S *pstAttr);
ERRORTYPE AudioHw_AO_GetPubAttr(AUDIO_DEV AudioDevId, AIO_ATTR_S *pstAttr);
ERRORTYPE audioHw_AO_ClrPubAttr(AUDIO_DEV AudioDevId);
ERRORTYPE audioHw_AO_SetTrackMode(AUDIO_DEV AudioDevId, AUDIO_TRACK_MODE_E enTrackMode);
ERRORTYPE audioHw_AO_GetTrackMode(AUDIO_DEV AudioDevId, AUDIO_TRACK_MODE_E *penTrackMode);
ERRORTYPE audioHw_AO_Enable(AUDIO_DEV AudioDevId);
ERRORTYPE audioHw_AO_Disable(AUDIO_DEV AudioDevId);
ERRORTYPE audioHw_AO_SetVolume(AUDIO_DEV AudioDevId, int s32VolumeDb);
ERRORTYPE audioHw_AO_GetVolume(AUDIO_DEV AudioDevId, int *ps32VolumeDb);
ERRORTYPE audioHw_AO_SetMute(AUDIO_DEV AudioDevId, BOOL bEnable, AUDIO_FADE_S *pstFade);
ERRORTYPE audioHw_AO_GetMute(AUDIO_DEV AudioDevId, BOOL *pbEnable, AUDIO_FADE_S *pstFade);
ERRORTYPE audioHw_AO_SetPA(AUDIO_DEV AudioDevId, BOOL bHighLevel);
ERRORTYPE audioHw_AO_GetPA(AUDIO_DEV AudioDevId, BOOL *pbHighLevel);

ERRORTYPE audioHw_AO_FillPcmRingBuf(AUDIO_DEV AudioDevId, void* pData, int Len);
ERRORTYPE audioHw_AO_DrainPcmRingBuf(AUDIO_DEV AudioDevId);

ERRORTYPE audioHw_AO_FeedPcmData(AUDIO_DEV AudioDevId, AUDIO_FRAME_S *pFrm);
ERRORTYPE audioHw_AO_GetPcmConfig(AUDIO_DEV AudioDevId, PCM_CONFIG_S **ppCfg);
ERRORTYPE audioHw_AO_GetAIOAttr(AUDIO_DEV AudioDevId, AIO_ATTR_S **ppAttr);
BOOL audioHw_AO_IsDevConfigured(AUDIO_DEV AudioDevId);
BOOL audioHw_AO_IsDevStarted(AUDIO_DEV AudioDevId);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_HW_H__ */

/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_ai.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/25
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_MPI_AI_H__
#define __IPCLINUX_MPI_AI_H__

#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"


#include "mm_common.h"
#include "mm_comm_aio.h"

#ifdef __cplusplus
extern "C"
{
#endif

ERRORTYPE AW_MPI_AI_SetPubAttr(AUDIO_DEV AudioDevId, const AIO_ATTR_S *pstAttr);
ERRORTYPE AW_MPI_AI_GetPubAttr(AUDIO_DEV AudioDevId, AIO_ATTR_S *pstAttr);

ERRORTYPE AW_MPI_AI_Enable(AUDIO_DEV AudioDevId);
ERRORTYPE AW_MPI_AI_Disable(AUDIO_DEV AudioDevId);

ERRORTYPE AW_MPI_AI_ResetChn(AUDIO_DEV AudioDevId, AI_CHN AiChn);

ERRORTYPE AW_MPI_AI_PauseChn(AUDIO_DEV AudioDevId, AI_CHN AiChn);
ERRORTYPE AW_MPI_AI_ResumeChn(AUDIO_DEV AudioDevId, AI_CHN AiChn);

ERRORTYPE AW_MPI_AI_CreateChn(AUDIO_DEV AudioDevId, AI_CHN AiChn);
ERRORTYPE AW_MPI_AI_DestroyChn(AUDIO_DEV AudioDevId, AI_CHN AiChn);

ERRORTYPE AW_MPI_AI_EnableChn(AUDIO_DEV AudioDevId, AI_CHN AiChn);
ERRORTYPE AW_MPI_AI_DisableChn(AUDIO_DEV AudioDevId, AI_CHN AiChn);

ERRORTYPE AW_MPI_AI_GetFrame(AUDIO_DEV AudioDevId, AI_CHN AiChn, AUDIO_FRAME_S *pstFrm, AEC_FRAME_S *pstAecFrm, int s32MilliSec);
ERRORTYPE AW_MPI_AI_ReleaseFrame(AUDIO_DEV AudioDevId, AI_CHN AiChn, AUDIO_FRAME_S *pstFrm, AEC_FRAME_S *pstAecFrm);

ERRORTYPE AW_MPI_AI_SetChnParam(AUDIO_DEV AudioDevId, AI_CHN AiChn, AI_CHN_PARAM_S *pstChnParam);
ERRORTYPE AW_MPI_AI_GetChnParam(AUDIO_DEV AudioDevId, AI_CHN AiChn, AI_CHN_PARAM_S *pstChnParam);

ERRORTYPE AW_MPI_AI_RegisterCallback(AUDIO_DEV AudioDevId, AI_CHN AiChn, MPPCallbackInfo *pCallback);

//ERRORTYPE AW_MPI_AI_SetVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn, AI_VQE_CONFIG_S *pstVqeConfig);
ERRORTYPE AW_MPI_AI_SetVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_VQE_CONFIG_S *pstVqeConfig); //
ERRORTYPE AW_MPI_AI_GetVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_VQE_CONFIG_S *pstVqeConfig);
ERRORTYPE AW_MPI_AI_EnableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);
ERRORTYPE AW_MPI_AI_DisableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);

ERRORTYPE AW_MPI_AI_EnableReSmp(AUDIO_DEV AudioDevId, AI_CHN AiChn, AUDIO_SAMPLE_RATE_E enOutSampleRate);
ERRORTYPE AW_MPI_AI_DisableReSmp(AUDIO_DEV AudioDevId, AI_CHN AiChn);

ERRORTYPE AW_MPI_AI_SetTrackMode(AUDIO_DEV AudioDevId, AUDIO_TRACK_MODE_E enTrackMode);
ERRORTYPE AW_MPI_AI_GetTrackMode(AUDIO_DEV AudioDevId, AUDIO_TRACK_MODE_E *penTrackMode);

ERRORTYPE AW_MPI_AI_SaveFile(AUDIO_DEV AudioDevId, AI_CHN AiChn, AUDIO_SAVE_FILE_INFO_S *pstSaveFileInfo);
ERRORTYPE AW_MPI_AI_QueryFileStatus(AUDIO_DEV AudioDevId, AI_CHN AiChn, AUDIO_SAVE_FILE_INFO_S *pstSaveFileInfo);

ERRORTYPE AW_MPI_AI_ClrPubAttr(AUDIO_DEV AudioDevId);

ERRORTYPE AW_MPI_AI_SetVqeVolume(AUDIO_DEV AudioDevId, AI_CHN AiChn, int s32VolumeDb);
ERRORTYPE AW_MPI_AI_GetVqeVolume(AUDIO_DEV AudioDevId, AI_CHN AiChn, int *ps32VolumeDb);

ERRORTYPE AW_MPI_AI_SetDevVolume(AUDIO_DEV AudioDevId, int s32VolumeDb);
ERRORTYPE AW_MPI_AI_GetDevVolume(AUDIO_DEV AudioDevId, int *ps32VolumeDb);
ERRORTYPE AW_MPI_AI_SetDevMute(AUDIO_DEV AudioDevId, int bEnableFlag);
ERRORTYPE AW_MPI_AI_GetDevMute(AUDIO_DEV AudioDevId, int *pbEnableFlag);

ERRORTYPE AW_MPI_AI_SetChnMute(AUDIO_DEV AudioDevId, AI_CHN AiChn, BOOL bMute);
ERRORTYPE AW_MPI_AI_GetChnMute(AUDIO_DEV AudioDevId, AI_CHN AiChn, BOOL* pbMute);

#ifdef __cplusplus
}
#endif

#endif /* __IPCLINUX_MPI_AI_H__ */


/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_ao.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/25
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_MPI_AO_H__
#define __IPCLINUX_MPI_AO_H__

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

ERRORTYPE AW_MPI_AO_SetPubAttr(AUDIO_DEV AudioDevId, const AIO_ATTR_S *pstAttr);
ERRORTYPE AW_MPI_AO_GetPubAttr(AUDIO_DEV AudioDevId, AIO_ATTR_S *pstAttr);
ERRORTYPE AW_MPI_AO_ClrPubAttr(AUDIO_DEV AudioDevId);

ERRORTYPE AW_MPI_AO_Enable(AUDIO_DEV AudioDevId);
ERRORTYPE AW_MPI_AO_Disable(AUDIO_DEV AudioDevId);

ERRORTYPE AW_MPI_AO_EnableChn(AUDIO_DEV AudioDevId, AO_CHN AoChn);
ERRORTYPE AW_MPI_AO_DisableChn(AUDIO_DEV AudioDevId, AO_CHN AoChn);

ERRORTYPE AW_MPI_AO_RegisterCallback(AUDIO_DEV AudioDevId, AO_CHN AoChn, MPPCallbackInfo *pCallback);

ERRORTYPE AW_MPI_AO_SetPcmCardType(AUDIO_DEV AudioDevId, AO_CHN AoChn, PCM_CARD_TYPE_E cardId);

ERRORTYPE AW_MPI_AO_StartChn(AUDIO_DEV AudioDevId, AO_CHN AoChn);
ERRORTYPE AW_MPI_AO_StopChn(AUDIO_DEV AudioDevId, AO_CHN AoChn);

ERRORTYPE AW_MPI_AO_SendFrame(AUDIO_DEV AudioDevId, AO_CHN AoChn, AUDIO_FRAME_S *pstData, int s32MilliSec);

ERRORTYPE AW_MPI_AO_EnableReSmp(AUDIO_DEV AudioDevId, AO_CHN AoChn, AUDIO_SAMPLE_RATE_E enInSampleRate);
ERRORTYPE AW_MPI_AO_DisableReSmp(AUDIO_DEV AudioDevId, AO_CHN AoChn);

ERRORTYPE AW_MPI_AO_QueryChnStat(AUDIO_DEV AudioDevId ,AO_CHN AoChn, AO_CHN_STATE_S *pstStatus);

ERRORTYPE AW_MPI_AO_PauseChn(AUDIO_DEV AudioDevId, AO_CHN AoChn);
ERRORTYPE AW_MPI_AO_ResumeChn(AUDIO_DEV AudioDevId, AO_CHN AoChn);
ERRORTYPE AW_MPI_AO_Seek(AUDIO_DEV AudioDevId, AO_CHN AoChn);

ERRORTYPE AW_MPI_AO_SetDevVolume(AUDIO_DEV AudioDevId, int s32VolumeDb);
ERRORTYPE AW_MPI_AO_GetDevVolume(AUDIO_DEV AudioDevId, int *ps32VolumeDb);

ERRORTYPE AW_MPI_AO_SetDevMute(AUDIO_DEV AudioDevId, BOOL bEnable, AUDIO_FADE_S *pstFade);
ERRORTYPE AW_MPI_AO_GetDevMute(AUDIO_DEV AudioDevId, BOOL *pbEnable, AUDIO_FADE_S *pstFade);

ERRORTYPE AW_MPI_AO_SetTrackMode(AUDIO_DEV AudioDevId, AUDIO_TRACK_MODE_E enTrackMode);
ERRORTYPE AW_MPI_AO_GetTrackMode(AUDIO_DEV AudioDevId, AUDIO_TRACK_MODE_E *penTrackMode);

ERRORTYPE AW_MPI_AO_SetVqeAttr(AUDIO_DEV AudioDevId, AO_CHN AoChn, AO_VQE_CONFIG_S *pstVqeConfig);
ERRORTYPE AW_MPI_AO_GetVqeAttr(AUDIO_DEV AudioDevId, AO_CHN AoChn, AO_VQE_CONFIG_S *pstVqeConfig);
ERRORTYPE AW_MPI_AO_EnableVqe(AUDIO_DEV AudioDevId, AO_CHN AoChn);
ERRORTYPE AW_MPI_AO_DisableVqe(AUDIO_DEV AudioDevId, AO_CHN AoChn);

ERRORTYPE AW_MPI_AO_SetStreamEof(AUDIO_DEV AudioDevId, AO_CHN AoChn, BOOL bEofFlag, BOOL bDrainFlag);

ERRORTYPE AW_MPI_AO_SaveFile(AUDIO_DEV AudioDevId, AO_CHN AoChn, AUDIO_SAVE_FILE_INFO_S *pstSaveFileInfo);
ERRORTYPE AW_MPI_AO_QueryFileStatus(AUDIO_DEV AudioDevId, AO_CHN AoChn, AUDIO_SAVE_FILE_INFO_S *pstSaveFileInfo);

ERRORTYPE AW_MPI_AO_SetPA(AUDIO_DEV AudioDevId, BOOL bLevelVal);
ERRORTYPE AW_MPI_AO_GetPA(AUDIO_DEV AudioDevId, BOOL *pbLevelVal);

#ifdef __cplusplus
}
#endif

#endif /* __IPCLINUX_MPI_AO_H__ */

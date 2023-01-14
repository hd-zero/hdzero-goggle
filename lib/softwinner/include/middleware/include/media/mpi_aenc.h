/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_aenc.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/12
  Last Modified :
  Description   : mpi functions declaration for audio encoder
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_MPI_AENC_H__
#define __IPCLINUX_MPI_AENC_H__

//ref platform headers
#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

//media api headers to app
#include "mm_common.h"
//#include "mm_comm_audio.h"
#include <mm_comm_aio.h>
#include "mm_comm_aenc.h"

//media internal common headers.
//#include "AudioEncApi.h"

#ifdef __cplusplus
extern "C" {
#endif


ERRORTYPE AW_MPI_AENC_CreateChn(AENC_CHN AeChn, const AENC_CHN_ATTR_S *pAttr);
ERRORTYPE AW_MPI_AENC_DestroyChn(AENC_CHN AeChn);

ERRORTYPE AW_MPI_AENC_ResetChn(AENC_CHN AeChn);

ERRORTYPE AW_MPI_AENC_Query(AENC_CHN AeChn, AENC_CHN_STAT_S *pStat);
ERRORTYPE AW_MPI_AENC_RegisterCallback(AENC_CHN AeChn, MPPCallbackInfo *pCallback);

ERRORTYPE AW_MPI_AENC_SetChnAttr(AENC_CHN AeChn, const AENC_CHN_ATTR_S *pAttr);
ERRORTYPE AW_MPI_AENC_GetChnAttr(AENC_CHN AeChn, AENC_CHN_ATTR_S *pAttr);

int AW_MPI_AENC_GetHandle(AENC_CHN AeChn);

ERRORTYPE AW_MPI_AENC_StartRecvPcm(AENC_CHN AeChn);
ERRORTYPE AW_MPI_AENC_StopRecvPcm(AENC_CHN AeChn);


ERRORTYPE AW_MPI_AENC_GetFirstFrmPts(AENC_CHN AeChn,long long *a_pts);

/*-1:bolck  0:nonblock   >0 : overtime   */
/* nMilliSec: -1 is block, 0 is no block, other positive number is timeout */
ERRORTYPE AW_MPI_AENC_SendFrame(AENC_CHN AeChn, AUDIO_FRAME_S *pFrame , AEC_FRAME_S* pAecFrm);
ERRORTYPE AW_MPI_AENC_GetStream(AENC_CHN AeChn, AUDIO_STREAM_S *pStream, int nMilliSec);
ERRORTYPE AW_MPI_AENC_ReleaseStream(AENC_CHN AeChn, AUDIO_STREAM_S *pStream);


#ifdef __cplusplus
}
#endif

#endif /* __IPCLINUX_MPI_AENC_H__ */


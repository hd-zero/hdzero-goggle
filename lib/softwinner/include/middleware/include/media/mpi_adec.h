/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_adec.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/12
  Last Modified :
  Description   : mpi functions declaration for video encoder
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_MPI_ADEC_H__
#define __IPCLINUX_MPI_ADEC_H__

//ref platform headers
#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

//media api headers to app
#include "mm_common.h"
//#include "mm_comm_audio.h"
#include <mm_comm_aio.h>
#include "mm_comm_adec.h"

//media internal common headers.
//#include "adecoder.h"

#ifdef __cplusplus
extern "C" {
#endif


ERRORTYPE AW_MPI_ADEC_CreateChn(ADEC_CHN ADecChn, const ADEC_CHN_ATTR_S *pAttr);
ERRORTYPE AW_MPI_ADEC_DestroyChn(ADEC_CHN ADecChn);

ERRORTYPE AW_MPI_ADEC_ResetChn(ADEC_CHN ADecChn);

ERRORTYPE AW_MPI_ADEC_RegisterCallback(ADEC_CHN ADecChn, MPPCallbackInfo *pCallback);

ERRORTYPE AW_MPI_ADEC_SetChnAttr(ADEC_CHN ADecChn, const ADEC_CHN_ATTR_S *pAttr);
ERRORTYPE AW_MPI_ADEC_GetChnAttr(ADEC_CHN ADecChn, ADEC_CHN_ATTR_S *pAttr);

ERRORTYPE AW_MPI_ADEC_StartRecvStream(ADEC_CHN AdChn);
ERRORTYPE AW_MPI_ADEC_StopRecvStream(ADEC_CHN AdChn);
ERRORTYPE AW_MPI_ADEC_SetStreamEof(ADEC_CHN AdChn, BOOL bEofFlag);

ERRORTYPE AW_MPI_ADEC_Pause(ADEC_CHN AdChn);
ERRORTYPE AW_MPI_ADEC_Seek(ADEC_CHN AdChn);

/* nMilliSec: -1 is block, 0 is no block, other positive number is timeout */
ERRORTYPE AW_MPI_ADEC_SendStream(ADEC_CHN AdChn, const AUDIO_STREAM_S *pStream, int nMilliSec);
ERRORTYPE AW_MPI_ADEC_GetFrame(ADEC_CHN AdChn, AUDIO_FRAME_S *pFrame, int nMilliSec);
ERRORTYPE AW_MPI_ADEC_ReleaseFrame(ADEC_CHN AdChn, AUDIO_FRAME_S *pFrame);


#ifdef __cplusplus
}
#endif

#endif /* __IPCLINUX_MPI_ADEC_H__ */


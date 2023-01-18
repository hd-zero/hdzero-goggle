/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_demux.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/04/25
  Last Modified :
  Description   : MPP Programe Interface for demux
  Function List :
  History       :
******************************************************************************/

#ifndef  __IPCLINUX_MPI_DEMUX_H__
#define  __IPCLINUX_MPI_DEMUX_H__

#include "mm_common.h"
#include "mm_comm_demux.h"

#include "EncodedStream.h"
#include "DemuxCompStream.h"


#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

ERRORTYPE AW_MPI_DEMUX_CreateChn(PARAM_IN DEMUX_CHN dmxChn, PARAM_IN const DEMUX_CHN_ATTR_S *pstAttr);
ERRORTYPE AW_MPI_DEMUX_DestroyChn(PARAM_IN DEMUX_CHN dmxChn);
ERRORTYPE AW_MPI_DEMUX_RegisterCallback(PARAM_IN DEMUX_CHN dmxChn, PARAM_IN MPPCallbackInfo *pCallback);
ERRORTYPE AW_MPI_DEMUX_SetChnAttr(PARAM_IN DEMUX_CHN dmxChn, PARAM_IN DEMUX_CHN_ATTR_S *pAttr);
ERRORTYPE AW_MPI_DEMUX_GetChnAttr(PARAM_IN DEMUX_CHN dmxChn, PARAM_OUT DEMUX_CHN_ATTR_S *pstAttr);
ERRORTYPE AW_MPI_DEMUX_GetMediaInfo(PARAM_IN DEMUX_CHN dmxChn, PARAM_OUT DEMUX_MEDIA_INFO_S *pMediaInfo);
ERRORTYPE AW_MPI_DEMUX_Start(PARAM_IN DEMUX_CHN dmxChn);
ERRORTYPE AW_MPI_DEMUX_Stop(PARAM_IN DEMUX_CHN dmxChn);
ERRORTYPE AW_MPI_DEMUX_Pause(PARAM_IN DEMUX_CHN dmxChn);
//ERRORTYPE AW_MPI_DEMUX_ResetChn(PARAM_IN DEMUX_CHN dmxChn);
ERRORTYPE AW_MPI_DEMUX_Seek(PARAM_IN DEMUX_CHN dmxChn, PARAM_IN int msec);

ERRORTYPE AW_MPI_DEMUX_getDmxOutPutBuf(PARAM_IN DEMUX_CHN dmxChn, PARAM_OUT EncodedStream *pDmxOutBuf, PARAM_IN int nMilliSec);
ERRORTYPE AW_MPI_DEMUX_releaseDmxBuf(PARAM_IN DEMUX_CHN dmxChn, PARAM_OUT EncodedStream *pDmxOutBuf);



#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __IPCLINUX_MPI_DEMUX_H__ */


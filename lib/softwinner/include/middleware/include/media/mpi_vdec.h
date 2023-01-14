/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_vdec.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/04/21
  Last Modified :
  Description   : MPP Programe Interface for video decode
  Function List :
  History       :
******************************************************************************/

#ifndef  __IPCLINUX_MPI_VDEC_H__
#define  __IPCLINUX_MPI_VDEC_H__

#include "mm_common.h"
#include "mm_comm_video.h"
#include "mm_comm_vdec.h"
#include "vdecoder.h"

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

ERRORTYPE AW_MPI_VDEC_CreateChn(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pAttr);
ERRORTYPE AW_MPI_VDEC_DestroyChn(VDEC_CHN VdChn);

ERRORTYPE AW_MPI_VDEC_GetChnAttr(VDEC_CHN VdChn, VDEC_CHN_ATTR_S *pAttr);

ERRORTYPE AW_MPI_VDEC_StartRecvStream(VDEC_CHN VdChn);
ERRORTYPE AW_MPI_VDEC_StartRecvStreamEx(VDEC_CHN VdChn, VDEC_DECODE_FRAME_PARAM_S *pDecodeParam);
ERRORTYPE AW_MPI_VDEC_StopRecvStream(VDEC_CHN VdChn);
ERRORTYPE AW_MPI_VDEC_Pause(VDEC_CHN VdChn);
ERRORTYPE AW_MPI_VDEC_Resume(VDEC_CHN VdChn);
ERRORTYPE AW_MPI_VDEC_Seek(VDEC_CHN VdChn);

ERRORTYPE AW_MPI_VDEC_Query(VDEC_CHN VdChn, VDEC_CHN_STAT_S *pStat);
ERRORTYPE AW_MPI_VDEC_RegisterCallback(VDEC_CHN VdChn, MPPCallbackInfo *pCallback);
ERRORTYPE AW_MPI_VDEC_SetStreamEof(VDEC_CHN VdChn, BOOL bEofFlag);


//ERRORTYPE AW_MPI_VDEC_GetFd(VDEC_CHN VdChn);

ERRORTYPE AW_MPI_VDEC_ResetChn(VDEC_CHN VdChn);

ERRORTYPE AW_MPI_VDEC_SetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S* pParam);
ERRORTYPE AW_MPI_VDEC_GetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S* pParam);

//ERRORTYPE AW_MPI_VDEC_SetProtocolParam(VDEC_CHN VdChn,VDEC_PRTCL_PARAM_S *pParam);
//ERRORTYPE AW_MPI_VDEC_GetProtocolParam(VDEC_CHN VdChn,VDEC_PRTCL_PARAM_S *pParam);

/* s32MilliSec: -1 is block£¬ 0 is no block£¬other positive number is timeout */
ERRORTYPE AW_MPI_VDEC_SendStream(VDEC_CHN VdChn, const VDEC_STREAM_S *pStream, int nMilliSec);

ERRORTYPE AW_MPI_VDEC_GetImage(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pFrameInfo,int nMilliSec);
ERRORTYPE AW_MPI_VDEC_ReleaseImage(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pFrameInfo);

ERRORTYPE AW_MPI_VDEC_GetDoubleImage(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pFrameInfo, VIDEO_FRAME_INFO_S *pSubFrameInfo,int nMilliSec);
ERRORTYPE AW_MPI_VDEC_ReleaseDoubleImage(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pFrameInfo, VIDEO_FRAME_INFO_S *pSubFrameInfo);

//ERRORTYPE AW_MPI_VDEC_GetUserData(VDEC_CHN VdChn, VDEC_USERDATA_S *pstUserData, int s32MilliSec);
//ERRORTYPE AW_MPI_VDEC_ReleaseUserData(VDEC_CHN VdChn, VDEC_USERDATA_S *pstUserData);

ERRORTYPE AW_MPI_VDEC_SetRotate(VDEC_CHN VdChn, ROTATE_E enRotate);
ERRORTYPE AW_MPI_VDEC_GetRotate(VDEC_CHN VdChn, ROTATE_E *penRotate);

//ERRORTYPE AW_MPI_VDEC_GetChnLuma(VDEC_CHN VdChn, VDEC_CHN_LUM_S *pLuma);

ERRORTYPE AW_MPI_VDEC_ReopenVideoEngine(VDEC_CHN VdChn);

//ERRORTYPE AW_MPI_VDEC_SetUserPic(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstUsrPic);
//ERRORTYPE AW_MPI_VDEC_EnableUserPic(VDEC_CHN VdChn, BOOL bInstant);
//ERRORTYPE AW_MPI_VDEC_DisableUserPic(VDEC_CHN VdChn);

//ERRORTYPE AW_MPI_VDEC_SetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E enDisplayMode);
//ERRORTYPE AW_MPI_VDEC_GetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E *penDisplayMode);

ERRORTYPE AW_MPI_VDEC_SetVEFreq(VDEC_CHN VeChn, int nFreq); //nFreq: MHz

ERRORTYPE AW_MPI_VDEC_SetVideoStreamInfo(VDEC_CHN VdChn, VideoStreamInfo *pVideoStreamInfo);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __IPCLINUX_MPI_VDEC_H__ */


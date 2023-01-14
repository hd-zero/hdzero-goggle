/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_venc.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : mpi functions declaration for video encoder
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_MPI_VENC_H__
#define __IPCLINUX_MPI_VENC_H__

//ref platform headers
#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

//media api headers to app
#include "mm_common.h"
#include "mm_comm_video.h"
#include "mm_comm_venc.h"

//media internal common headers.
#include "vencoder.h"
#include "ComponentCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

ERRORTYPE AW_MPI_VENC_CreateChn(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pAttr);
ERRORTYPE AW_MPI_VENC_DestroyChn(VENC_CHN VeChn);
//ERRORTYPE AW_MPI_VENC_SetChnlPriority(VENC_CHN VeChn, unsigned int nPriority);
//ERRORTYPE AW_MPI_VENC_GetChnlPriority(VENC_CHN VeChn, unsigned int *pPriority);
ERRORTYPE AW_MPI_VENC_ResetChn(VENC_CHN VeChn);

ERRORTYPE AW_MPI_VENC_StartRecvPic(VENC_CHN VeChn);
ERRORTYPE AW_MPI_VENC_StartRecvPicEx(VENC_CHN VeChn, VENC_RECV_PIC_PARAM_S *pRecvParam);
ERRORTYPE AW_MPI_VENC_StopRecvPic(VENC_CHN VeChn);

ERRORTYPE AW_MPI_VENC_Query(VENC_CHN VeChn, VENC_CHN_STAT_S *pStat);
ERRORTYPE AW_MPI_VENC_RegisterCallback(VENC_CHN VeChn, MPPCallbackInfo *pCallback);

ERRORTYPE AW_MPI_VENC_SetChnAttr(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pAttr);
ERRORTYPE AW_MPI_VENC_GetChnAttr(VENC_CHN VeChn, VENC_CHN_ATTR_S *pAttr);
/*-1:bolck  0:nonblock   >0 : overtime   */
ERRORTYPE AW_MPI_VENC_GetStream(VENC_CHN VeChn, VENC_STREAM_S *pStream, int nMilliSec);
ERRORTYPE AW_MPI_VENC_ReleaseStream(VENC_CHN VeChn, VENC_STREAM_S *pStream);

//ERRORTYPE AW_MPI_VENC_InsertUserData(VENC_CHN VeChn, unsigned char *pData, unsigned int nLen);
/*-1:bolck  0:nonblock   >0 : overtime   */
ERRORTYPE AW_MPI_VENC_SendFrame(VENC_CHN VeChn, VIDEO_FRAME_INFO_S *pFrame ,int nMilliSec);
//ERRORTYPE AW_MPI_VENC_GetUsedFrame(VENC_CHN VeChn, VIDEO_FRAME_INFO_S *pFrame ,int nMilliSec);

//ERRORTYPE AW_MPI_VENC_SetMaxStreamCnt(VENC_CHN VeChn, unsigned int u32MaxStrmCnt);
//ERRORTYPE AW_MPI_VENC_GetMaxStreamCnt(VENC_CHN VeChn, unsigned int *pu32MaxStrmCnt);

ERRORTYPE AW_MPI_VENC_RequestIDR(VENC_CHN VeChn, BOOL bInstant);

int AW_MPI_VENC_GetHandle(VENC_CHN VeChn);

ERRORTYPE AW_MPI_VENC_SetRoiCfg(VENC_CHN VeChn, VENC_ROI_CFG_S *pVencRoiCfg);
ERRORTYPE AW_MPI_VENC_GetRoiCfg(VENC_CHN VeChn, unsigned int nIndex, VENC_ROI_CFG_S *pVencRoiCfg);

//ERRORTYPE AW_MPI_VENC_SetRoiBgFrameRate(VENC_CHN VeChn, const VENC_ROIBG_FRAME_RATE_S *pstRoiBgFrmRate);
//ERRORTYPE AW_MPI_VENC_GetRoiBgFrameRate(VENC_CHN VeChn, VENC_ROIBG_FRAME_RATE_S *pstRoiBgFrmRate);

//ERRORTYPE AW_MPI_VENC_SetH264SliceSplit(VENC_CHN VeChn, const VENC_PARAM_H264_SLICE_SPLIT_S *pSliceSplit);
//ERRORTYPE AW_MPI_VENC_GetH264SliceSplit(VENC_CHN VeChn, VENC_PARAM_H264_SLICE_SPLIT_S *pSliceSplit);

//ERRORTYPE AW_MPI_VENC_SetH264InterPred(VENC_CHN VeChn, const VENC_PARAM_H264_INTER_PRED_S *pH264InterPred);
//ERRORTYPE AW_MPI_VENC_GetH264InterPred(VENC_CHN VeChn, VENC_PARAM_H264_INTER_PRED_S *pH264InterPred);

//ERRORTYPE AW_MPI_VENC_SetH264IntraPred(VENC_CHN VeChn, const VENC_PARAM_H264_INTRA_PRED_S *pH264IntraPred);
//ERRORTYPE AW_MPI_VENC_GetH264IntraPred(VENC_CHN VeChn, VENC_PARAM_H264_INTRA_PRED_S *pH264IntraPred);

//ERRORTYPE AW_MPI_VENC_SetH264Trans(VENC_CHN VeChn, const VENC_PARAM_H264_TRANS_S *pH264Trans);
//ERRORTYPE AW_MPI_VENC_GetH264Trans(VENC_CHN VeChn, VENC_PARAM_H264_TRANS_S *pH264Trans);

//ERRORTYPE AW_MPI_VENC_SetH264Entropy(VENC_CHN VeChn, const VENC_PARAM_H264_ENTROPY_S *pH264EntropyEnc);
//ERRORTYPE AW_MPI_VENC_GetH264Entropy(VENC_CHN VeChn, VENC_PARAM_H264_ENTROPY_S *pH264EntropyEnc);

//ERRORTYPE AW_MPI_VENC_SetH264Poc(VENC_CHN VeChn, const VENC_PARAM_H264_POC_S *pH264Poc);
//ERRORTYPE AW_MPI_VENC_GetH264Poc(VENC_CHN VeChn, VENC_PARAM_H264_POC_S *pH264Poc);

//ERRORTYPE AW_MPI_VENC_SetH264Dblk(VENC_CHN VeChn, const VENC_PARAM_H264_DBLK_S *pH264Dblk);
//ERRORTYPE AW_MPI_VENC_GetH264Dblk(VENC_CHN VeChn, VENC_PARAM_H264_DBLK_S *pH264Dblk);

//ERRORTYPE AW_MPI_VENC_SetH264Vui(VENC_CHN VeChn, const VENC_PARAM_H264_VUI_S *pH264Vui);
//ERRORTYPE AW_MPI_VENC_GetH264Vui(VENC_CHN VeChn, VENC_PARAM_H264_VUI_S *pH264Vui);

ERRORTYPE AW_MPI_VENC_GetH264SpsPpsInfo(VENC_CHN VeChn, VencHeaderData *pH264SpsPpsInfo);
ERRORTYPE AW_MPI_VENC_GetH265SpsPpsInfo(VENC_CHN VeChn, VencHeaderData *pH265SpsPpsInfo);

ERRORTYPE AW_MPI_VENC_SetJpegParam(VENC_CHN VeChn, const VENC_PARAM_JPEG_S *pJpegParam);
ERRORTYPE AW_MPI_VENC_GetJpegParam(VENC_CHN VeChn, VENC_PARAM_JPEG_S *pJpegParam);
ERRORTYPE AW_MPI_VENC_SetJpegExifInfo(VENC_CHN VeChn, const VENC_EXIFINFO_S *pJpegExifInfo);
ERRORTYPE AW_MPI_VENC_GetJpegExifInfo(VENC_CHN VeChn, VENC_EXIFINFO_S *pJpegExifInfo);
ERRORTYPE AW_MPI_VENC_GetJpegThumbBuffer(VENC_CHN VeChn, VENC_JPEG_THUMB_BUFFER_S *pThumbBuffer);

//ERRORTYPE AW_MPI_VENC_SetMjpegParam(VENC_CHN VeChn, const VENC_PARAM_MJPEG_S *pMjpegParam);
//ERRORTYPE AW_MPI_VENC_GetMjpegParam(VENC_CHN VeChn, VENC_PARAM_MJPEG_S *pMjpegParam);

ERRORTYPE AW_MPI_VENC_SetFrameRate(VENC_CHN VeChn, const VENC_FRAME_RATE_S *pFrameRate);
ERRORTYPE AW_MPI_VENC_GetFrameRate(VENC_CHN VeChn, VENC_FRAME_RATE_S *pFrameRate);

ERRORTYPE AW_MPI_VENC_SetTimeLapse(VENC_CHN VeChn, int64_t nTimeLapse); //unit:us
ERRORTYPE AW_MPI_VENC_GetTimeLapse(VENC_CHN VeChn, int64_t *pTimeLapse);

//ERRORTYPE AW_MPI_VENC_GetRcParam(VENC_CHN VeChn, VENC_RC_PARAM_S *pRcParam);
//ERRORTYPE AW_MPI_VENC_SetRcParam(VENC_CHN VeChn, const VENC_RC_PARAM_S *pRcParam);


//ERRORTYPE AW_MPI_VENC_SetRefParam(VENC_CHN VeChn, const VENC_PARAM_REF_S *pRefParam);
//ERRORTYPE AW_MPI_VENC_GetRefParam(VENC_CHN VeChn, VENC_PARAM_REF_S *pRefParam);

ERRORTYPE AW_MPI_VENC_SetColor2Grey(VENC_CHN VeChn, const VENC_COLOR2GREY_S *pChnColor2Grey);
ERRORTYPE AW_MPI_VENC_GetColor2Grey(VENC_CHN VeChn, VENC_COLOR2GREY_S *pChnColor2Grey);

ERRORTYPE AW_MPI_VENC_SetCrop(VENC_CHN VeChn, const VENC_CROP_CFG_S *pCropCfg);
ERRORTYPE AW_MPI_VENC_GetCrop(VENC_CHN VeChn, VENC_CROP_CFG_S *pCropCfg);

//ERRORTYPE AW_MPI_VENC_SetJpegSnapMode(VENC_CHN VeChn, VENC_JPEG_SNAP_MODE_E enJpegSnapMode);
//ERRORTYPE AW_MPI_VENC_GetJpegSnapMode(VENC_CHN VeChn, VENC_JPEG_SNAP_MODE_E *penJpegSnapMode);

//ERRORTYPE AW_MPI_VENC_EnableIDR(VENC_CHN VeChn, BOOL bEnableIDR);

//ERRORTYPE AW_MPI_VENC_SetH264IdrPicId( VENC_CHN VeChn, VENC_H264_IDRPICID_CFG_S* pstH264eIdrPicIdCfg );
//ERRORTYPE AW_MPI_VENC_GetH264IdrPicId( VENC_CHN VeChn, VENC_H264_IDRPICID_CFG_S* pstH264eIdrPicIdCfg );

ERRORTYPE AW_MPI_VENC_GetStreamBufInfo(VENC_CHN VeChn, VENC_STREAM_BUF_INFO_S *pStreamBufInfo);

//ERRORTYPE AW_MPI_VENC_SetRcPriority(VENC_CHN VeChn, VENC_RC_PRIORITY_E enRcPriority);
//ERRORTYPE AW_MPI_VENC_GetRcPriority(VENC_CHN VeChn, VENC_RC_PRIORITY_E *penRcPriority);


//ERRORTYPE AW_MPI_VENC_SetH265SliceSplit(VENC_CHN VeChn, const VENC_PARAM_H265_SLICE_SPLIT_S *pSliceSplit);
//ERRORTYPE AW_MPI_VENC_GetH265SliceSplit(VENC_CHN VeChn, VENC_PARAM_H265_SLICE_SPLIT_S *pSliceSplit);


//ERRORTYPE AW_MPI_VENC_SetH265PredUnit(VENC_CHN VeChn, const VENC_PARAM_H265_PU_S *pPredUnit);
//ERRORTYPE AW_MPI_VENC_GetH265PredUnit(VENC_CHN VeChn, VENC_PARAM_H265_PU_S *pPredUnit);


//ERRORTYPE AW_MPI_VENC_SetH265Trans(VENC_CHN VeChn, const VENC_PARAM_H265_TRANS_S *pH265Trans);
//ERRORTYPE AW_MPI_VENC_GetH265Trans(VENC_CHN VeChn, VENC_PARAM_H265_TRANS_S *pH265Trans);

//ERRORTYPE AW_MPI_VENC_SetH265Entropy(VENC_CHN VeChn, const VENC_PARAM_H265_ENTROPY_S *pH265Entropy);
//ERRORTYPE AW_MPI_VENC_GetH265Entropy(VENC_CHN VeChn, VENC_PARAM_H265_ENTROPY_S *pH265Entropy);

//ERRORTYPE AW_MPI_VENC_SetH265Dblk(VENC_CHN VeChn, const VENC_PARAM_H265_DBLK_S *pH265Dblk);
//ERRORTYPE AW_MPI_VENC_GetH265Dblk(VENC_CHN VeChn, VENC_PARAM_H265_DBLK_S *pH265Dblk);
//ERRORTYPE AW_MPI_VENC_SetH265Sao(VENC_CHN VeChn, const VENC_PARAM_H265_SAO_S *pH265Sao);
//ERRORTYPE AW_MPI_VENC_GetH265Sao(VENC_CHN VeChn, VENC_PARAM_H265_SAO_S *pH265Sao);


//ERRORTYPE AW_MPI_VENC_SetH265Timing(VENC_CHN VeChn, const VENC_PARAM_H265_TIMING_S *pH265Timing);
//ERRORTYPE AW_MPI_VENC_GetH265Timing(VENC_CHN VeChn, VENC_PARAM_H265_TIMING_S *pH265Timing);

//ERRORTYPE AW_MPI_VENC_SetFrameLostStrategy(VENC_CHN VeChn, const VENC_PARAM_FRAMELOST_S *pFrmLostParam);
//ERRORTYPE AW_MPI_VENC_GetFrameLostStrategy(VENC_CHN VeChn, VENC_PARAM_FRAMELOST_S *pFrmLostParam);


ERRORTYPE AW_MPI_VENC_SetSuperFrameCfg(VENC_CHN VeChn, const VENC_SUPERFRAME_CFG_S *pSuperFrmParam);
ERRORTYPE AW_MPI_VENC_GetSuperFrameCfg(VENC_CHN VeChn,VENC_SUPERFRAME_CFG_S *pSuperFrmParam);

ERRORTYPE AW_MPI_VENC_SetIntraRefresh(VENC_CHN VeChn, VENC_PARAM_INTRA_REFRESH_S *pIntraRefresh);
ERRORTYPE AW_MPI_VENC_GetIntraRefresh(VENC_CHN VeChn, VENC_PARAM_INTRA_REFRESH_S *pIntraRefresh);

ERRORTYPE AW_MPI_VENC_SetSmartP(VENC_CHN VeChn, VencSmartFun *pSmartPParam);
ERRORTYPE AW_MPI_VENC_GetSmartP(VENC_CHN VeChn, VencSmartFun *pSmartPParam);
/**
 * for h265
 */
ERRORTYPE AW_MPI_VENC_SetBrightness(VENC_CHN VeChn, VencBrightnessS *pBrightness);
ERRORTYPE AW_MPI_VENC_GetBrightness(VENC_CHN VeChn, VencBrightnessS *pBrightness);

ERRORTYPE AW_MPI_VENC_SetQPMAP(VENC_CHN VeChn, const VencMBModeCtrl *pQPMAP);
/**
 * set buffer before encode one frame.
 * after encode, buffer will be filled with MB info.
 * VencMBInfo->p_para need malloc by user.
 */
ERRORTYPE AW_MPI_VENC_SetQPMAPMBInfoOutput(VENC_CHN VeChn, const VencMBInfo *pQpMapMBInfo);
/**
 * after encode, get whole frame MB info
 */
ERRORTYPE AW_MPI_VENC_GetQPMAPMBSumInfoOutput(VENC_CHN VeChn, VencMBSumInfo *pQpMapMBSumInfo);

ERRORTYPE AW_MPI_VENC_SetVEFreq(VENC_CHN VeChn, int nFreq); //nFreq: MHz

/**
 * 3d nois reduce, 3dfilter.
 * @param b3DNRFlag [0, 6], 0(close, default), recommend:3, strongest: 6
 */
ERRORTYPE AW_MPI_VENC_Set3DNR(VENC_CHN VeChn, int b3DNRFlag);
ERRORTYPE AW_MPI_VENC_Get3DNR(VENC_CHN VeChn, int *b3DNRFlag);

ERRORTYPE AW_MPI_VENC_GetCacheState(VENC_CHN VeChn, CacheState *pCacheState);

ERRORTYPE AW_MPI_VENC_SetRefParam(VENC_CHN VeChn, const VENC_PARAM_REF_S *pstRefParam);
ERRORTYPE AW_MPI_VENC_GetRefParam(VENC_CHN VeChn, VENC_PARAM_REF_S *pstRefParam);

ERRORTYPE AW_MPI_VENC_SetHorizonFlip(VENC_CHN VeChn, BOOL bHorizonFlipFlag);
ERRORTYPE AW_MPI_VENC_GetHorizonFlip(VENC_CHN VeChn, BOOL *bpHorizonFlipFlag);

ERRORTYPE AW_MPI_VENC_SetAdaptiveIntraInP(VENC_CHN VeChn, BOOL bAdaptiveIntraInPFlag);

ERRORTYPE AW_MPI_VENC_EnableNullSkip(VENC_CHN VeChn, BOOL bEnable);
ERRORTYPE AW_MPI_VENC_EnablePSkip(VENC_CHN VeChn, BOOL bEnable);
/**
 * forbid mpi_venc to discard frame when encoding fail.
 */
ERRORTYPE AW_MPI_VENC_ForbidDiscardingFrame(VENC_CHN VeChn, BOOL bForbid);

/**
 * during encoding, command encoder to save bs file.
 */
ERRORTYPE AW_MPI_VENC_SaveBsFile(VENC_CHN VeChn, VencSaveBSFile *pSaveParam);

/**
 * open ve proc info to catch param.
 */
ERRORTYPE AW_MPI_VENC_SetProcSet(VENC_CHN VeChn, VeProcSet *pVeProcSet);

//ERRORTYPE AW_MPI_VENC_setOsdMaskRegions(VENC_CHN VeChn, VENC_OVERLAY_INFO *pOsdMaskRegion);
//ERRORTYPE AW_MPI_VENC_removeOsdMaskRegions(VENC_CHN VeChn);

#ifdef __cplusplus
}
#endif

#endif /* __IPCLINUX_MPI_VENC_H__ */


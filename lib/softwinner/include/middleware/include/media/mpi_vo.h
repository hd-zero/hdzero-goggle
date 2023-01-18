/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_vo.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/26
  Last Modified :
  Description   : mpi functions declaration
  Function List :
  History       :
******************************************************************************/


#ifndef _AW_VO_API_H_
#define _AW_VO_API_H_

/*********************************************************************************************************
*
*  Header File Include
**********************************************************************************************************/
#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"


//#include "mm_common.h"
#include "mm_comm_vo.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Device Settings */

ERRORTYPE AW_MPI_VO_Enable (VO_DEV VoDev);
ERRORTYPE AW_MPI_VO_Disable(VO_DEV VoDev);
ERRORTYPE AW_MPI_VO_SetPubAttr(VO_DEV VoDev, const VO_PUB_ATTR_S *pstPubAttr);
ERRORTYPE AW_MPI_VO_GetPubAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);
ERRORTYPE AW_MPI_VO_GetHdmiHwMode(VO_DEV VoDev, VO_INTF_SYNC_E *mode);
//ERRORTYPE AW_MPI_VO_CloseFd(HI_VOID);
//ERRORTYPE AW_MPI_VO_SetDevFrameRate(VO_DEV VoDev, unsigned int uFrameRate);
//ERRORTYPE AW_MPI_VO_GetDevFrameRate(VO_DEV VoDev, unsigned int *puFrameRate);

/* Video Settings */

ERRORTYPE AW_MPI_VO_EnableVideoLayer(VO_LAYER VoLayer);
ERRORTYPE AW_MPI_VO_DisableVideoLayer(VO_LAYER VoLayer);
/**
 * these two functions are special for miniGUI's ui layer.
 * Don't use these two functions unless you know what you want!
 */
ERRORTYPE AW_MPI_VO_AddOutsideVideoLayer(VO_LAYER VoLayer);
ERRORTYPE AW_MPI_VO_RemoveOutsideVideoLayer(VO_LAYER VoLayer);

ERRORTYPE AW_MPI_VO_OpenVideoLayer(VO_LAYER VoLayer);  //begin to show
ERRORTYPE AW_MPI_VO_CloseVideoLayer(VO_LAYER VoLayer);  //end to show

ERRORTYPE AW_MPI_VO_SetVideoLayerAttr(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
ERRORTYPE AW_MPI_VO_GetVideoLayerAttr(VO_LAYER VoLayer, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

//ERRORTYPE AW_MPI_VO_BindVideoLayer(VO_LAYER VoLayer, VO_DEV VoDev);
//ERRORTYPE AW_MPI_VO_UnBindVideoLayer(VO_LAYER VoLayer, VO_DEV VoDev);

ERRORTYPE AW_MPI_VO_SetVideoLayerPriority(VO_LAYER VoLayer, unsigned int uPriority);
ERRORTYPE AW_MPI_VO_GetVideoLayerPriority(VO_LAYER VoLayer, unsigned int *puPriority);

//ERRORTYPE AW_MPI_VO_SetVideoLayerCSC(VO_LAYER VoLayer, const VO_CSC_S *pstVideoCSC);
//ERRORTYPE AW_MPI_VO_GetVideoLayerCSC(VO_LAYER VoLayer, VO_CSC_S *pstVideoCSC);

//ERRORTYPE AW_MPI_VO_SetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E enPartMode);
//ERRORTYPE AW_MPI_VO_GetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E *penPartMode);

//ERRORTYPE AW_MPI_VO_SetAttrBegin(VO_LAYER VoLayer);
//ERRORTYPE AW_MPI_VO_SetAttrEnd  (VO_LAYER VoLayer);

//ERRORTYPE AW_MPI_VO_SetPlayToleration(VO_LAYER VoLayer, unsigned int uToleration);
//ERRORTYPE AW_MPI_VO_GetPlayToleration(VO_LAYER VoLayer, unsigned int *puToleration);

//ERRORTYPE AW_MPI_VO_GetScreenFrame    (VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVFrame, int nMilliSec);
//ERRORTYPE AW_MPI_VO_ReleaseScreenFrame(VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVFrame);

//ERRORTYPE AW_MPI_VO_SetDispBufLen(VO_LAYER VoLayer, unsigned int uBufLen);
//ERRORTYPE AW_MPI_VO_GetDispBufLen(VO_LAYER VoLayer, unsigned int *puBufLen);

ERRORTYPE AW_MPI_VO_SetVideoLayerAlpha(VO_LAYER VoLayer, VO_VIDEO_LAYER_ALPHA_S *pAlpha);
ERRORTYPE AW_MPI_VO_GetVideoLayerAlpha(VO_LAYER VoLayer, VO_VIDEO_LAYER_ALPHA_S *pAlpha);

/* General Operation of Channel */

ERRORTYPE AW_MPI_VO_EnableChn(VO_LAYER VoLayer, VO_CHN VoChn);
ERRORTYPE AW_MPI_VO_DisableChn(VO_LAYER VoLayer, VO_CHN VoChn);
ERRORTYPE AW_MPI_VO_RegisterCallback(VO_LAYER VoLayer, VO_CHN VoChn, MPPCallbackInfo *pCallback);

//ERRORTYPE AW_MPI_VO_SetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_ATTR_S *pstChnAttr);
//ERRORTYPE AW_MPI_VO_GetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_ATTR_S *pstChnAttr);

//ERRORTYPE AW_MPI_VO_SetChnField(VO_LAYER VoLayer, VO_CHN VoChn, const VO_DISPLAY_FIELD_E enField);
//ERRORTYPE AW_MPI_VO_GetChnField(VO_LAYER VoLayer, VO_CHN VoChn, VO_DISPLAY_FIELD_E *pField);
/**
 * vo channel cache display frame buf number. default to zero.
 */
ERRORTYPE AW_MPI_VO_SetChnDispBufNum(VO_LAYER VoLayer, VO_CHN VoChn, unsigned int uBufNum);
ERRORTYPE AW_MPI_VO_GetChnDispBufNum(VO_LAYER VoLayer, VO_CHN VoChn, unsigned int *puBufNum);
ERRORTYPE AW_MPI_VO_GetDisplaySize (VO_LAYER VoLayer, VO_CHN VoChn, SIZE_S *pDisplaySize);

//ERRORTYPE AW_MPI_VO_SetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, int nChnFrmRate);
//ERRORTYPE AW_MPI_VO_GetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, int *pChnFrmRate);

//ERRORTYPE AW_MPI_VO_GetChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstFrame, int nMilliSec);
//ERRORTYPE AW_MPI_VO_ReleaseChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, const VIDEO_FRAME_INFO_S *pstFrame);

ERRORTYPE AW_MPI_VO_StartChn (VO_LAYER VoLayer, VO_CHN VoChn);
ERRORTYPE AW_MPI_VO_StopChn (VO_LAYER VoLayer, VO_CHN VoChn);
ERRORTYPE AW_MPI_VO_PauseChn (VO_LAYER VoLayer, VO_CHN VoChn);
ERRORTYPE AW_MPI_VO_ResumeChn(VO_LAYER VoLayer, VO_CHN VoChn);
ERRORTYPE AW_MPI_VO_Seek(VO_LAYER VoLayer, VO_CHN VoChn);
ERRORTYPE AW_MPI_VO_SetStreamEof(VO_LAYER VoLayer, VO_CHN VoChn, BOOL bEofFlag);

//ERRORTYPE AW_MPI_VO_StepChn(VO_LAYER VoLayer, VO_CHN VoChn);
//ERRORTYPE AW_MPI_VO_RefreshChn( VO_LAYER VoLayer, VO_CHN VoChn);

ERRORTYPE AW_MPI_VO_ShowChn(VO_LAYER VoLayer, VO_CHN VoChn);
ERRORTYPE AW_MPI_VO_HideChn(VO_LAYER VoLayer, VO_CHN VoChn);

/** 
 * zoom in source picture frame. crop source frame to display in destination hd/sd device.
 *
 * @return SUCCESS.
 * @param pstZoomAttr zoom attribute.
 */
//ERRORTYPE AW_MPI_VO_SetZoomInWindow(VO_LAYER VoLayer, VO_CHN VoChn, const VO_ZOOM_ATTR_S *pstZoomAttr);
//ERRORTYPE AW_MPI_VO_GetZoomInWindow(VO_LAYER VoLayer, VO_CHN VoChn, VO_ZOOM_ATTR_S *pstZoomAttr);

/** 
 * zoom in destination video layer. all video channels on a video layer buffer,
 * then zoom this buffer to hd/sd display device.
 *
 * @return SUCCESS.
 * @param pstZoomAttr zoom attribute.
 * @param mode zoom mode, e.g. NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW
 */
ERRORTYPE AW_MPI_VO_SetVideoScalingMode(VO_LAYER VoLayer, VO_CHN VoChn, int mode);

ERRORTYPE AW_MPI_VO_GetChnPts   (VO_LAYER VoLayer, VO_CHN VoChn, uint64_t *pChnPts);

//ERRORTYPE AW_MPI_VO_QueryChnStat(VO_LAYER VoLayer, VO_CHN VoChn, VO_QUERY_STATUS_S *pstStatus);

ERRORTYPE AW_MPI_VO_SendFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVFrame, int nMilliSec);

//ERRORTYPE AW_MPI_VO_ClearChnBuffer(VO_LAYER VoLayer, VO_CHN VoChn, BOOL bClrAll);

//ERRORTYPE AW_MPI_VO_SetChnBorder(VO_LAYER VoLayer, VO_CHN VoChn, const VO_BORDER_S *pstBorder);
//ERRORTYPE AW_MPI_VO_GetChnBorder(VO_LAYER VoLayer, VO_CHN VoChn, VO_BORDER_S *pstBorder);

//ERRORTYPE AW_MPI_VO_SetChnReceiveThreshold(VO_LAYER VoLayer, VO_CHN VoChn, unsigned int uThreshold);
//ERRORTYPE AW_MPI_VO_GetChnReceiveThreshold(VO_LAYER VoLayer, VO_CHN VoChn, unsigned int *puThreshold);

//ERRORTYPE AW_MPI_VO_GetChnRegionLuma(VO_LAYER VoLayer, VO_CHN VoChn, VO_REGION_INFO_S *pstRegionInfo,
//                                                unsigned int *pu32LumaData, int nMilliSec);
/* GraphicLayer setting */

//ERRORTYPE AW_MPI_VO_BindGraphicLayer(GRAPHIC_LAYER GraphicLayer, VO_DEV VoDev);
//ERRORTYPE AW_MPI_VO_UnBindGraphicLayer(GRAPHIC_LAYER GraphicLayer, VO_DEV VoDev);
//ERRORTYPE AW_MPI_VO_SetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, const VO_CSC_S *pstCSC);
//ERRORTYPE AW_MPI_VO_GetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, VO_CSC_S *pstCSC);

ERRORTYPE AW_MPI_VO_Debug_StoreFrame(VO_LAYER VoLayer, VO_CHN VoChn, uint64_t framePts);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _AW_VO_API_H_ */


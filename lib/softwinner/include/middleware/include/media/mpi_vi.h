/*
 ******************************************************************************
 *
 * vi_api.h
 *
 * Hawkview ISP - vi_api.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 * 2.0		      yuanxianfeng   2016/08/01	    VIDEO INPUT
 *
 *****************************************************************************
 */

#ifndef _AW_VI_API_H_
#define _AW_VI_API_H_

#include "mm_comm_vi.h"
#include "mm_comm_region.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


/* Vi tunnel operations, Vipp & Osd/Mask & VirviChn Settings */
// AW_S32 AW_MPI_VI_Init(void);
// AW_S32 AW_MPI_VI_Exit(void);
AW_S32 AW_MPI_VI_CreateVipp(VI_DEV ViDev);
AW_S32 AW_MPI_VI_DestoryVipp(VI_DEV ViDev);
AW_S32 AW_MPI_VI_SetVippAttr(VI_DEV ViDev, VI_ATTR_S *pstAttr);
AW_S32 AW_MPI_VI_GetVippAttr(VI_DEV ViDev, VI_ATTR_S *pstAttr);
AW_S32 AW_MPI_VI_SetVippMirror(VI_DEV ViDev, int Value);
AW_S32 AW_MPI_VI_GetVippMirror(VI_DEV ViDev, int *Value);
AW_S32 AW_MPI_VI_SetVippFlip(VI_DEV ViDev, int Value);
AW_S32 AW_MPI_VI_GetVippFlip(VI_DEV ViDev, int *Value);
AW_S32 AW_MPI_VI_EnableVipp(VI_DEV ViDev);
AW_S32 AW_MPI_VI_DisableVipp(VI_DEV ViDev);
AW_S32 AW_MPI_VI_SetVippShutterTime(VI_DEV ViDev, VI_SHUTTIME_CFG_S *pTime);

AW_S32 AW_MPI_VI_CreateVirChn(VI_DEV ViDev, VI_CHN ViCh, void *pAttr);
AW_S32 AW_MPI_VI_DestoryVirChn(VI_DEV ViDev, VI_CHN ViCh);
AW_S32 AW_MPI_VI_GetVirChnAttr(VI_DEV ViDev, VI_CHN ViCh, void *pAttr);
AW_S32 AW_MPI_VI_SetVirChnAttr(VI_DEV ViDev, VI_CHN ViCh, void *pAttr);
AW_S32 AW_MPI_VI_EnableVirChn(VI_DEV ViDev, VI_CHN ViCh);
AW_S32 AW_MPI_VI_DisableVirChn(VI_DEV ViDev, VI_CHN ViCh);
/* VI non-tunnel operations */
AW_S32 AW_MPI_VI_GetFrame(VI_DEV ViDev, VI_CHN ViCh, VIDEO_FRAME_INFO_S *pstFrameInfo, AW_S32 s32MilliSec);
AW_S32 AW_MPI_VI_ReleaseFrame(VI_DEV ViDev, VI_CHN ViCh, VIDEO_FRAME_INFO_S *pstFrameInfo);
AW_S32 AW_MPI_VI_SetShutterTime(VI_DEV ViDev, VI_CHN ViChnMask, VI_SHUTTIME_CFG_S *pTime);

// Other Interface
AW_S32 AW_MPI_VI_SetVIFreq(VI_DEV ViDev, int nFreq); //nFreq: MHz
AW_S32 AW_MPI_VI_GetInfo(); /* get bind config */

//AW_S32 AW_MPI_VI_SetOsdMaskRegion(VI_DEV ViDev, VI_OsdMaskRegion *pstOsdMaskRegion);
//AW_S32 AW_MPI_VI_UpdateOsdMaskRegion(VI_DEV ViDev, AW_U32 OnOff);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /*_AW_VI_API_H_*/

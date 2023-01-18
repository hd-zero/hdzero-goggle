
/*
 ******************************************************************************
 *
 * MPI_ISE.h
 *
 * Hawkview ISP - mpi_ise.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   1.0		  yuanxianfeng   2016/04/01		ISE
 *
 *****************************************************************************
 */

#ifndef _AW_ISE_API_H_
#define _AW_ISE_API_H_

#include "plat_defines.h"
#include "plat_errno.h"
#include "plat_math.h"
#include "plat_type.h"

// media api headers to app
#include "mm_comm_ise.h"
//#include "mm_comm_mux.h"
#include "mm_common.h"

// media internal common headers.
// #include <vencoder.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


/* ISE tunnel operations, Group & Chn Settings */

ERRORTYPE AW_MPI_ISE_CreateGroup(ISE_GRP IseGrp, ISE_GROUP_ATTR_S *pGrpAttr);
ERRORTYPE AW_MPI_ISE_DestroyGroup(ISE_GRP IseGrp);
ERRORTYPE AW_MPI_ISE_SetGrpAttr(ISE_GRP IseGrp, ISE_GROUP_ATTR_S *pGrpAttr);
ERRORTYPE AW_MPI_ISE_GetGrpAttr(ISE_GRP IseGrp, ISE_GROUP_ATTR_S *pGrpAttr);

ERRORTYPE AW_MPI_ISE_CreatePort(ISE_GRP IseGrp, ISE_CHN IsePort, ISE_CHN_ATTR_S *pChnAttr); // Port
ERRORTYPE AW_MPI_ISE_DestroyPort(ISE_GRP IseGrp, ISE_CHN IsePort);
ERRORTYPE AW_MPI_ISE_GetPortAttr(ISE_GRP IseGrp, ISE_CHN IsePort, ISE_CHN_ATTR_S *pChnAttr);
ERRORTYPE AW_MPI_ISE_SetPortAttr(ISE_GRP IseGrp, ISE_CHN IsePort, ISE_CHN_ATTR_S *pChnAttr);

ERRORTYPE AW_MPI_ISE_Start(ISE_GRP IseGrp);
ERRORTYPE AW_MPI_ISE_Stop(ISE_GRP IseGrp);
/* ISE non-tunnel operations */
ERRORTYPE AW_MPI_ISE_GetData(ISE_GRP IseGrp, ISE_CHN IsePort, VIDEO_FRAME_INFO_S *pstVdaData, AW_S32 s32MilliSec);
ERRORTYPE AW_MPI_ISE_ReleaseData(ISE_GRP IseGrp, ISE_CHN IsePort, VIDEO_FRAME_INFO_S *pstVdaData);
ERRORTYPE AW_MPI_ISE_SendPic(ISE_GRP IseGrp, VIDEO_FRAME_INFO_S *pstUserFrame0, VIDEO_FRAME_INFO_S *pstUserFrame1,
                            AW_S32 s32MilliSec);
ERRORTYPE AW_MPI_ISE_RegisterCallback(ISE_GRP IseGrp, MPPCallbackInfo *pCallback);
ERRORTYPE AW_MPI_ISE_SetISEFreq(ISE_GRP IseGrp,int nFreq);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /*_AW_ISE_API_H_*/

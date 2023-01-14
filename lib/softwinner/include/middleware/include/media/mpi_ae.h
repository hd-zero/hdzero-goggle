
/*
 ******************************************************************************
 *
 * MPI_AE.h
 *
 * Hawkview ISP - mpi_ae.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   1.0		  yuanxianfeng   2016/04/01	    AE
 *
 *****************************************************************************
 */

#ifndef _AW_AE_API_H_
#define _AW_AE_API_H_

#include "mpi_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif  /* __cplusplus */

/****************************************************\
	AE
\****************************************************/

#if 0 /* not open customer */
AW_S32 AW_MPI_ISP_AE_SetWeightAttr(ISP_DEV IspDev, ISP_AE_WEIGHT_S *pWeight);
AW_S32 AW_MPI_ISP_AE_GetWeightAttr(ISP_DEV IspDev, ISP_AE_WEIGHT_S *pWeight);

AW_S32 AW_MPI_ISP_AE_SetExposureAttr(ISP_DEV IspDev, const ISP_AE_ATTR_PUB_S *pstExpAttr);
AW_S32 AW_MPI_ISP_AE_GetExposureAttr(ISP_DEV IspDev, ISP_AE_ATTR_PUB_S *pstExpAttr);
AW_S32 AW_MPI_ISP_AE_SetTableAttr(ISP_DEV IspDev, AW_S32 mode, ISP_AE_TABLE_S *pTable);
AW_S32 AW_MPI_ISP_AE_GetTableAttr(ISP_DEV IspDev, AW_S32 mode, ISP_AE_TABLE_S *pTable);
AW_S32 AW_MPI_ISP_AE_SetDelayAttr(ISP_DEV IspDev, ISP_AE_DELAY_S *pDelay);
AW_S32 AW_MPI_ISP_AE_GetDelayAttr(ISP_DEV IspDev, ISP_AE_DELAY_S *pDelay);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif  /* __cplusplus */

#endif /*_AW_AE_API_H_*/


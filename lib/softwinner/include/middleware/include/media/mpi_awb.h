
/*
 ******************************************************************************
 *
 * MPI_AWB.h
 *
 * Hawkview ISP - mpi_awb.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   1.0		  yuanxianfeng   2016/04/01	    AWB
 *
 *****************************************************************************
 */

#ifndef _AW_AWB_API_H_
#define _AW_AWB_API_H_

#include "mpi_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif  /* __cplusplus */


/****************************************************\
	AWB
\****************************************************/
#if 0 /* not open customer */
AW_S32 AW_MPI_ISP_AWB_SetSpeed(ISP_DEV IspDev, ISP_AWB_SPEED_S *pSpeed);
AW_S32 AW_MPI_ISP_AWB_GetSpeed(ISP_DEV IspDev, ISP_AWB_SPEED_S *pSpeed);
AW_S32 AW_MPI_ISP_AWB_SetTempRange(ISP_DEV IspDev, ISP_AWB_TEMP_RANGE_S *pTempRange);
AW_S32 AW_MPI_ISP_AWB_GetTempRange(ISP_DEV IspDev, ISP_AWB_TEMP_RANGE_S *pTempRange);
AW_S32 AW_MPI_ISP_AWB_SetLight(ISP_DEV IspDev, AW_S32 mode, ISP_AWB_TEMP_INFO_S *pTempInfo);
AW_S32 AW_MPI_ISP_AWB_GetLight(ISP_DEV IspDev, AW_S32 mode, ISP_AWB_TEMP_INFO_S *pTempInfo);
AW_S32 AW_MPI_ISP_AWB_SetFavor(ISP_DEV IspDev, ISP_AWB_FAVOR_S *pFavor);
AW_S32 AW_MPI_ISP_AWB_GetFavor(ISP_DEV IspDev, ISP_AWB_FAVOR_S *pFavor);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif  /* __cplusplus */

#endif /*_AW_AWB_API_H_*/


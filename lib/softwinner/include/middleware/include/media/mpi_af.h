
/*
 ******************************************************************************
 *
 * MPI_AF.h
 *
 * Hawkview ISP - mpi_af.h module
 *
 * Copyright (c) 2016 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version		  Author         Date		    Description
 *
 *   1.0		  yuanxianfeng   2016/04/01	    AF
 *
 *****************************************************************************
 */

#ifndef _AW_AF_API_H_
#define _AW_AF_API_H_

// #include "common_vi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif  /* __cplusplus */

/****************************************************\
	AF
\****************************************************/
#if 0 /* not open customer */
AW_S32 AW_MPI_ISP_AF_SetVcmCode(ISP_DEV IspDev, ISP_AF_VCM_CODE_S *pVcmCode);
AW_S32 AW_MPI_ISP_AF_GetVcmCode(ISP_DEV IspDev, ISP_AF_VCM_CODE_S *pVcmCode);
AW_S32 AW_MPI_ISP_AF_SetOtp(ISP_DEV IspDev, ISP_AF_OTP_S *pOtp);
AW_S32 AW_MPI_ISP_AF_GetOtp(ISP_DEV IspDev, ISP_AF_OTP_S *pOtp);
AW_S32 AW_MPI_ISP_AF_SetSpeed(ISP_DEV IspDev, ISP_AF_SPEED_S *pSpeed);
AW_S32 AW_MPI_ISP_AF_GetSpeed(ISP_DEV IspDev, ISP_AF_SPEED_S *pSpeed);
AW_S32 AW_MPI_ISP_AF_SetFineSearch(ISP_DEV IspDev, ISP_AF_FINE_SEARCH_S *pFineSearch);
AW_S32 AW_MPI_ISP_AF_GetFineSearch(ISP_DEV IspDev, ISP_AF_FINE_SEARCH_S *pFineSearch);
AW_S32 AW_MPI_ISP_AF_SetRefocus(ISP_DEV IspDev, ISP_AF_REFOCUS_S *pRefocus);
AW_S32 AW_MPI_ISP_AF_GetRefocus(ISP_DEV IspDev, ISP_AF_REFOCUS_S *pRefocus);
AW_S32 AW_MPI_ISP_AF_SetTolerance(ISP_DEV IspDev, ISP_AF_TOLERANCE_S *pTolerance);
AW_S32 AW_MPI_ISP_AF_GetTolerance(ISP_DEV IspDev, ISP_AF_TOLERANCE_S *pTolerance);
AW_S32 AW_MPI_ISP_AF_SetScene(ISP_DEV IspDev, ISP_AF_SCENE_S *pScene);
AW_S32 AW_MPI_ISP_AF_GetScene(ISP_DEV IspDev, ISP_AF_SCENE_S *pScene);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif  /* __cplusplus */

#endif /*_AW_AF_API_H_*/


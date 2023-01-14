/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_venc_common.c
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/05/23
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef _MPI_VENC_PRIVATE_H_
#define _MPI_VENC_PRIVATE_H_

#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

#include "mm_common.h"
#include "mm_component.h"
#include <mm_comm_region.h>

#ifdef __cplusplus
extern "C" {
#endif

ERRORTYPE VENC_Construct(void);
ERRORTYPE VENC_Destruct(void);
MM_COMPONENTTYPE* VENC_GetChnComp(MPP_CHN_S *pMppChn);

/**
 * Store region. If region is available to show, draw region immediately.
 */
ERRORTYPE AW_MPI_VENC_SetRegion(VENC_CHN VeChn, RGN_HANDLE RgnHandle, RGN_ATTR_S *pRgnAttr, const RGN_CHN_ATTR_S *pRgnChnAttr, BITMAP_S *pBmp);
ERRORTYPE AW_MPI_VENC_DeleteRegion(VENC_CHN VeChn, RGN_HANDLE RgnHandle);
/**
 * update immediately on next frame
 */
ERRORTYPE AW_MPI_VENC_UpdateOverlayBitmap(VENC_CHN VeChn, RGN_HANDLE RgnHandle, BITMAP_S *pBitmap);
ERRORTYPE AW_MPI_VENC_UpdateRegionChnAttr(VENC_CHN VeChn, RGN_HANDLE RgnHandle, const RGN_CHN_ATTR_S *pRgnChnAttr);

#ifdef __cplusplus
}
#endif

#endif  /* _MPI_VENC_PRIVATE_H_ */


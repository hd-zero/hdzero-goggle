/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_vi_common.c
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/05/23
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef _MPI_VI_PRIVATE_H_
#define _MPI_VI_PRIVATE_H_

#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

#include "mm_common.h"
#include "mm_component.h"
#include <mm_comm_vi.h>
#include <mm_comm_region.h>

#ifdef __cplusplus
extern "C" {
#endif

//ERRORTYPE VI_Construct(void);
//ERRORTYPE VI_Destruct(void);
AW_S32 AW_MPI_VI_Init(void);
AW_S32 AW_MPI_VI_Exit(void);
MM_COMPONENTTYPE *videoInputHw_GetChnComp(VI_DEV ViDev, VI_CHN ViChn);

/**
 * Store region. If region is available to show, draw region immediately.
 */
ERRORTYPE AW_MPI_VI_SetRegion(VI_DEV ViDev, RGN_HANDLE RgnHandle, RGN_ATTR_S *pRgnAttr, const RGN_CHN_ATTR_S *pRgnChnAttr, BITMAP_S *pBmp);
ERRORTYPE AW_MPI_VI_DeleteRegion(VI_DEV ViDev, RGN_HANDLE RgnHandle);

/**
 * draw this overlay immediately
 */
ERRORTYPE AW_MPI_VI_UpdateOverlayBitmap(VI_DEV ViDev, RGN_HANDLE RgnHandle, BITMAP_S *pBitmap);
ERRORTYPE AW_MPI_VI_UpdateRegionChnAttr(VI_DEV ViDev, RGN_HANDLE RgnHandle, const RGN_CHN_ATTR_S *pRgnChnAttr);

ERRORTYPE AW_MPI_VI_Debug_StoreFrame(VI_DEV ViDev, VI_CHN ViCh, const char* pDirPath); //pDirPath: e.g, "/mnt/extsd/VIDebug"

#ifdef __cplusplus
}
#endif

#endif  /* _MPI_VI_PRIVATE_H_ */


/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_region.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/04/25
  Last Modified :
  Description   : 
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_MPI_REGION_H__
#define __IPCLINUX_MPI_REGION_H__

#include "mm_comm_region.h"

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

ERRORTYPE AW_MPI_RGN_Create(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);
ERRORTYPE AW_MPI_RGN_Destroy(RGN_HANDLE Handle);

ERRORTYPE AW_MPI_RGN_GetAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRegion);
ERRORTYPE AW_MPI_RGN_SetAttr(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);

ERRORTYPE AW_MPI_RGN_SetBitMap(RGN_HANDLE Handle, const BITMAP_S *pstBitmap);

ERRORTYPE AW_MPI_RGN_AttachToChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);
ERRORTYPE AW_MPI_RGN_DetachFromChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn);

ERRORTYPE AW_MPI_RGN_SetDisplayAttr(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);
ERRORTYPE AW_MPI_RGN_GetDisplayAttr(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr);

//ERRORTYPE AW_MPI_RGN_SetAttachField(RGN_HANDLE Handle, VIDEO_FIELD_E enAttachField);
//ERRORTYPE AW_MPI_RGN_GetAttachField(RGN_HANDLE Handle, VIDEO_FIELD_E *penAttachField);

//ERRORTYPE AW_MPI_RGN_GetCanvasInfo(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstCanvasInfo);
//ERRORTYPE AW_MPI_RGN_UpdateCanvas(RGN_HANDLE Handle);


#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __IPCLINUX_MPI_REGION_H__ */


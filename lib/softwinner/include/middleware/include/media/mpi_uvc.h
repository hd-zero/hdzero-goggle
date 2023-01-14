/******************************************************************************
  Copyright (C), 2001-2017, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_uvc.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/11/30
  Last Modified :
  Description   : mpi functions declaration for uvc input component
  Function List :
  History       :
******************************************************************************/

#ifndef _MPI_UVC_H_
#define _MPI_UVC_H_

#pragma once

#include "mm_comm_uvc.h"
#include "mm_comm_video.h"
#include "aw_type.h"

#ifdef __cplusplus
extern "C" {
#endif

    ERRORTYPE AW_MPI_UVC_CreateDevice(UVC_DEV UvcDev);
    ERRORTYPE AW_MPI_UVC_DestroyDevice(UVC_DEV UvcDev);
    ERRORTYPE AW_MPI_UVC_GetDeviceAttr(UVC_DEV UvcDev, UVC_ATTR_S *pAttr);
    ERRORTYPE AW_MPI_UVC_SetDeviceAttr(UVC_DEV UvcDev, UVC_ATTR_S *pAttr);
    ERRORTYPE AW_MPI_UVC_EnableDevice(UVC_DEV UvcDev);
    ERRORTYPE AW_MPI_UVC_DisableDevice(UVC_DEV UvcDev);


    ERRORTYPE AW_MPI_UVC_CreateVirChn(UVC_DEV UvcDev, UVC_CHN UvcChn);
    ERRORTYPE AW_MPI_UVC_DestroyVirChn(UVC_DEV UvcDev, UVC_CHN UvcChn);
    ERRORTYPE AW_MPI_UVC_GetVirChnAttr(UVC_DEV UvcDev,UVC_CHN UvcChn);
    ERRORTYPE AW_MPI_UVC_SetVirChnAttr(UVC_DEV UvcDev, UVC_CHN UvcChn);


    ERRORTYPE AW_MPI_UVC_StartRecvPic(UVC_DEV UvcDev, UVC_CHN UvcChn);
    ERRORTYPE AW_MPI_UVC_StartRecvPicEx(UVC_DEV UvcDev, UVC_CHN UvcChn);
    ERRORTYPE AW_MPI_UVC_StopRecvPic(UVC_DEV UvcDev, UVC_CHN UvcChn);


    ERRORTYPE AW_MPI_UVC_GetFrame(UVC_DEV UvcDev, UVC_CHN UvcChn, VIDEO_FRAME_INFO_S *pstFrameInfo, AW_S32 s32MilliSec);
    ERRORTYPE AW_MPI_UVC_ReleaseFrame(UVC_DEV UvcDev, UVC_CHN UvcChn, VIDEO_FRAME_INFO_S *pstFrameInfo);


    //note: the control depend on UVC device.
    ERRORTYPE AW_MPI_UVC_SetBrightness(UVC_DEV UvcDev, int Value); //亮度
    ERRORTYPE AW_MPI_UVC_GetBrightness(UVC_DEV UvcDev, int *pValue);

    ERRORTYPE AW_MPI_UVC_SetContrast(UVC_DEV UvcDev, int Value); //对比度
    ERRORTYPE AW_MPI_UVC_GetContrast(UVC_DEV UvcDev, int *pValue);

    ERRORTYPE AW_MPI_UVC_SetHue(UVC_DEV UvcDev, int Value); //色调
    ERRORTYPE AW_MPI_UVC_GetHue(UVC_DEV UvcDev, int *pValue);

    ERRORTYPE AW_MPI_UVC_SetSaturation(UVC_DEV UvcDev, int Value); //饱和度
    ERRORTYPE AW_MPI_UVC_GetSaturation(UVC_DEV UvcDev, int *pValue);

    ERRORTYPE AW_MPI_UVC_SetSharpness(UVC_DEV UvcDev, int Value); //锐度
    ERRORTYPE AW_MPI_UVC_GetSharpness(UVC_DEV UvcDev, int *pValue);

#ifdef __cplusplus
}
#endif





#endif



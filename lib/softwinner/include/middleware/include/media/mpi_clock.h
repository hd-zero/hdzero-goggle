/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_clock.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/30
  Last Modified :
  Description   : MPP Programe Interface for video decode
  Function List :
  History       :
******************************************************************************/

#ifndef  __IPCLINUX_MPI_CLOCK_H__
#define  __IPCLINUX_MPI_CLOCK_H__

#include "mm_common.h"
#include "mm_comm_clock.h"

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

ERRORTYPE AW_MPI_CLOCK_CreateChn(CLOCK_CHN ClockChn, CLOCK_CHN_ATTR_S *pAttr);
ERRORTYPE AW_MPI_CLOCK_DestroyChn(CLOCK_CHN ClockChn);

ERRORTYPE AW_MPI_CLOCK_Start(CLOCK_CHN ClockChn);
ERRORTYPE AW_MPI_CLOCK_Stop(CLOCK_CHN ClockChn);
ERRORTYPE AW_MPI_CLOCK_Pause(CLOCK_CHN ClockChn);
ERRORTYPE AW_MPI_CLOCK_Seek(CLOCK_CHN ClockChn);
ERRORTYPE AW_MPI_CLOCK_RegisterCallback(CLOCK_CHN ClockChn, MPPCallbackInfo *pCallback);
/** 
 * get current media time.
 *
 * @return SUCCESS.
 * @param ClockChn clock channel number.
 * @param pMediaTime media time in millisecond.
 */
ERRORTYPE AW_MPI_CLOCK_GetCurrentMediaTime(CLOCK_CHN ClockChn, int* pMediaTime);


#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* __IPCLINUX_MPI_CLOCK_H__ */


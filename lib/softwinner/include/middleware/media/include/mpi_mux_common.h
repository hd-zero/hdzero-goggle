/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_mux_common.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/05/23
  Last Modified :
  Description   : mux module internal common header file.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_MPI_MUX_COMMON_H__
#define __IPCLINUX_MPI_MUX_COMMON_H__

//ref platform headers
#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

//media api headers to app
#include <mm_common.h>
#include <mm_comm_mux.h>

//media internal common headers.
#include <mm_component.h>

#ifdef __cplusplus
extern "C" {
#endif

ERRORTYPE MUX_Construct(void);
ERRORTYPE MUX_Destruct(void);
MM_COMPONENTTYPE *MUX_GetGroupComp(PARAM_IN MPP_CHN_S *pMppChn);

#ifdef __cplusplus
}
#endif

#endif /* __IPCLINUX_MPI_MUX_COMMON_H__ */


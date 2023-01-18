/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_demux_common.c
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/18
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef __MPI_DEMUX_COMMON_H__
#define __MPI_DEMUX_COMMON_H__

#include "plat_type.h"
#include "mm_common.h"
#include "mm_component.h"

ERRORTYPE DEMUX_Construct(void);
ERRORTYPE DEMUX_Destruct(void);
MM_COMPONENTTYPE* DEMUX_GetChnComp(MPP_CHN_S *pMppChn);

#endif /* __MPI_DEMUX_COMMON_H__ */


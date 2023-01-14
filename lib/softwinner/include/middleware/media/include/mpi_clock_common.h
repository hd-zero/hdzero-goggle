/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_clock_common.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/30
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef __MPI_CLOCK_COMMON_H__
#define __MPI_CLOCK_COMMON_H__

#include "plat_type.h"
#include "mm_common.h"
#include "mm_component.h"

ERRORTYPE CLOCK_Construct(void);
ERRORTYPE CLOCK_Destruct(void);
MM_COMPONENTTYPE* CLOCK_GetChnComp(MPP_CHN_S *pMppChn);

#endif /* __MPI_CLOCK_COMMON_H__ */


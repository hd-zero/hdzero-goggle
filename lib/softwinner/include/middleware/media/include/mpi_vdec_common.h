/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_vdec_common.c
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/18
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef __MPI_VDEC_COMMON_H__
#define __MPI_VDEC_COMMON_H__

#include "plat_type.h"
#include "mm_common.h"
#include "mm_component.h"

ERRORTYPE VDEC_Construct(void);
ERRORTYPE VDEC_Destruct(void);
MM_COMPONENTTYPE* VDEC_GetChnComp(MPP_CHN_S *pMppChn);

#endif /* __MPI_VDEC_COMMON_H__ */


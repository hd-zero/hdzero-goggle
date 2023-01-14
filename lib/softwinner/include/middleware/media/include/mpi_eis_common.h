/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_vda_common.c
  Version       : Initial Draft
  Author        : Allwinner PDC-PD5 Team - bohanhuang
  Created       : 2018/10/31
  Last Modified :
  Description   : mpi eis component functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef __MPI_EIS_COMMON_H__
#define __MPI_EIS_COMMON_H__

#include "plat_type.h"
#include "mm_common.h"
#include "mm_component.h"

ERRORTYPE EIS_Construct(void);
ERRORTYPE EIS_Destruct(void);
MM_COMPONENTTYPE *EIS_GetGroupComp(MPP_CHN_S *pMppChn);

#endif /* __MPI_EIS_COMMON_H__ */


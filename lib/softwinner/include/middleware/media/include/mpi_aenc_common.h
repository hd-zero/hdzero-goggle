/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_aenc_common.c
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/12
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef __MPI_AENC_COMMON_H__
#define __MPI_AENC_COMMON_H__

#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

#include "mm_common.h"
#include "mm_component.h"

ERRORTYPE AENC_Construct(void);
ERRORTYPE AENC_Destruct(void);
MM_COMPONENTTYPE *AENC_GetChnComp(MPP_CHN_S *pMppChn);

#endif /* __MPI_AENC_COMMON_H__ */

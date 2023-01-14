/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mpi_region_private.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/06/24
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef _MPI_REGION_PRIVATE_H_
#define _MPI_REGION_PRIVATE_H_

#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

#include "mm_common.h"
#include "mm_component.h"
#include "mm_comm_region.h"

#ifdef __cplusplus
extern "C" {
#endif

ERRORTYPE RegionManager_Construct(void);
ERRORTYPE RegionManager_Destruct(void);

#ifdef __cplusplus
}
#endif

#endif  /* _MPI_REGION_PRIVATE_H_ */


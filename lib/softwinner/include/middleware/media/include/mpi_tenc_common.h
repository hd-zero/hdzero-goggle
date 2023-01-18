#ifndef __MPI_TENC_COMMON_H__
#define __MPI_TENC_COMMON_H__


#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

#include "mm_common.h"
#include "mm_component.h"

ERRORTYPE TENC_Construct(void);
ERRORTYPE TENC_Destruct(void);
MM_COMPONENTTYPE *TENC_GetChnComp(MPP_CHN_S *pMppChn); 

#endif

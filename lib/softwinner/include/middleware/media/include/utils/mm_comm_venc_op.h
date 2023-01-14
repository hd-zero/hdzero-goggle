/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_venc.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : common struct definition for VENC
  Function List :
  History       :
******************************************************************************/

#ifndef _MM_COMM_VENC_OP_H_
#define _MM_COMM_VENC_OP_H_

#include "plat_type.h"
#include "plat_errno.h"
#include "mm_comm_venc.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

ERRORTYPE getPicSizeFromVENC_ATTR_S(VENC_ATTR_S *pVencAttr, SIZE_S *pPicSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _MM_COMM_VENC_OP_H_ */


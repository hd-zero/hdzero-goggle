/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : VencCompStream.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/12
  Last Modified :
  Description   : common struct definition in VdecComp and MPIVdec.
  Function List :
  History       :
******************************************************************************/

#ifndef _VENCCOMPSTREAM_H_
#define _VENCCOMPSTREAM_H_

#include <plat_type.h>
#include <vencoder.h>
#include <mm_comm_venc.h>

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

typedef struct VEncStream {
    VENC_STREAM_S *pStream;
    int nMilliSec;
} VEncStream;

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* _VENCCOMPSTREAM_H_ */


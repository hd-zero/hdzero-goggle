/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : AencCompStream.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/12/20
  Last Modified :
  Description   : common struct definition in AencComp and MPIAenc.
  Function List :
  History       :
******************************************************************************/

#ifndef _AENCCOMPSTREAM_H_
#define _AENCCOMPSTREAM_H_

#include <plat_type.h>

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

typedef struct AEncStream {
    AUDIO_STREAM_S *pStream;
    int nMilliSec;
} AEncStream;

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* _AENCCOMPSTREAM_H_ */


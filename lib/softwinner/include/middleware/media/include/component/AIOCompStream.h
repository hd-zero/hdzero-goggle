/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : AIOCompStream.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/01/13
  Last Modified :
  Description   : common struct definition in AI/AO Component.
  Function List :
  History       :
******************************************************************************/
#ifndef _AIOCOMPSTREAM_H_
#define _AIOCOMPSTREAM_H_

#include <plat_type.h>

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

typedef struct AudioFrame {
    AUDIO_FRAME_S *pFrame;
    AEC_FRAME_S *pAecFrame;
    int nMilliSec;
} AudioFrame;

typedef struct AudioStream {
    AUDIO_STREAM_S *pStream;
    int nMilliSec;
} AudioStream;

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* _AIOCOMPSTREAM_H_ */


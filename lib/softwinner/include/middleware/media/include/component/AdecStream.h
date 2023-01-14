/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : AdecStream.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/01/03
  Last Modified :
  Description   : common struct definition in AdecComp and MPIAdec.
  Function List :
  History       :
******************************************************************************/

#ifndef _ADECSTREAM_H_
#define _ADECSTREAM_H_

#include <mm_comm_adec.h>
#include <mm_comm_aio.h>

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */


typedef struct AdecInputStream
{
    AUDIO_STREAM_S *pStream;
    int nMilliSec;
}AdecInputStream;

typedef struct AdecOutputFrame
{
    AUDIO_FRAME_S *pFrame;
    int nMilliSec;
}AdecOutputFrame;

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* _ADECSTREAM_H_ */


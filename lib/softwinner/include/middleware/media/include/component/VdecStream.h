/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : VdecStream.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/12
  Last Modified :
  Description   : common struct definition in VdecComp and MPIVdec.
  Function List :
  History       :
******************************************************************************/

#ifndef _VDECSTREAM_H_
#define _VDECSTREAM_H_

#include <mm_comm_vdec.h>

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */


typedef struct VdecInputStream
{
    VDEC_STREAM_S *pStream;
    int nMilliSec;
}VdecInputStream;

typedef struct VdecOutFrame
{
    VIDEO_FRAME_INFO_S *pFrameInfo;
    VIDEO_FRAME_INFO_S *pSubFrameInfo;
    int nMilliSec;
}VdecOutFrame;

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* _VDECSTREAM_H_ */


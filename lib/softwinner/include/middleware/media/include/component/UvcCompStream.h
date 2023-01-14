/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : UvcCompStream.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/12/12
  Last Modified :
  Description   : common struct definition in UvcComp and MPIUvc.
  Function List :
  History       :
******************************************************************************/

#ifndef _UVCCOMPSTREAM_H_
#define _UVCCOMPSTREAM_H_

#include <plat_type.h>
#include <vdecoder.h>
#include "media/mm_comm_video.h"
#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */


/*typedef struct UvcCompOutputBuffer
{
    //VideoPicture *Decoderbuf;
    //V4L2BUF_t *v4l2_buf;
    //VideoPicture *pvbuf;
    //int nStreamIndex;
    VIDEO_FRAME_INFO_S *videobuf;
    VideoPicture *pvbuf;
    int mUsedRefCnt; 
    struct list_head app_info_node;

}UvcCompOutputBuffer;*/

typedef struct UvcStream {
    VIDEO_FRAME_INFO_S *pStream;
    int nMilliSec;
    int streamIdx;
} UvcStream;

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* _UVCCOMPSTREAM_H_ */


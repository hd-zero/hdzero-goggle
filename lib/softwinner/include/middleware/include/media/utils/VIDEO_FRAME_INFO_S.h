/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : VIDEO_FRAME_INFO_S.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : multimedia common function for VIDEO_FRAME_INFO_S.
  Function List :
  History       :
******************************************************************************/
#ifndef _VIDEO_FRAME_INFO_S_H_
#define _VIDEO_FRAME_INFO_S_H_

#include <mm_comm_video.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct VideoFrameBufferSizeInfo
{
    int mYSize;
    int mUSize;
    int mVSize;
} VideoFrameBufferSizeInfo;

ERRORTYPE getVideoFrameBufferSizeInfo(PARAM_IN VIDEO_FRAME_INFO_S *pFrame, PARAM_OUT VideoFrameBufferSizeInfo *pSizeInfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _VIDEO_FRAME_INFO_S_H_ */


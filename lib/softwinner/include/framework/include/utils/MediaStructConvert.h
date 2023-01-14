/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : MediaStructConvert.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/12/13
  Last Modified :
  Description   :
  Function List :
  History       :
******************************************************************************/
#ifndef __MEDIASTRUCTCONVERT_H__
#define __MEDIASTRUCTCONVERT_H__

#include <mm_comm_video.h>
#include <type_camera.h>

namespace EyeseeLinux {

void config_VIDEO_FRAME_INFO_S_by_VIDEO_FRAME_BUFFER_S(VIDEO_FRAME_INFO_S *pEncBuf, VIDEO_FRAME_BUFFER_S *pCameraFrameInfo);

}
#endif	// __MEDIASTRUCTCONVERT_H__


/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : EyeseeISECommon.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/10/18
  Last Modified :
  Description   : ISE module.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_EYESEEISECOMMON_H__
#define __IPCLINUX_EYESEEISECOMMON_H__

#include <EyeseeCamera.h>

namespace EyeseeLinux {

typedef enum ISE_MODE_E
{
    ISE_MODE_ONE_FISHEYE = 1,
    ISE_MODE_TWO_FISHEYE = 2,
    ISE_MODE_TWO_ISE = 3,
} ISE_MODE_E;

class CameraChannelInfo
{
public:
    //EyeseeCamera *mpCamera;
    CameraRecordingProxy *mpCameraRecordingProxy;
    int mnCameraChannel;
};

}; /* namespace EyeseeLinux */

#endif /* __IPCLINUX_EYESEEISECOMMON_H__ */


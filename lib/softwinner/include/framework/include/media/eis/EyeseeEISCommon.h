#ifndef __IPCLINUX_EYESEEEISCOMMON_H__
#define __IPCLINUX_EYESEEEISCOMMON_H__

#include <EyeseeCamera.h>

namespace EyeseeLinux {

class EIS_CameraChannelInfo
{
public:
	CameraRecordingProxy *mpCameraRecordingProxy;
	int mnCameraChannel;
};

};



#endif

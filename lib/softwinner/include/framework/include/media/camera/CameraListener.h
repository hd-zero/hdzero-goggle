/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : CameraListener.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/06/13
  Last Modified :
  Description   : camera wrap MPP components.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_CAMERA_LISTENER_H__
#define __IPCLINUX_CAMERA_LISTENER_H__

#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

#include "Errors.h"
#include <CameraParameters.h>
#include <CMediaMemory.h>

#include <memory>

namespace EyeseeLinux {

class CameraRecordingProxyListener
{
public:
    CameraRecordingProxyListener(){}
    virtual ~CameraRecordingProxyListener(){}
    virtual void dataCallbackTimestamp(const void *pData) = 0;
};

class CameraRecordingProxy
{
public:
    CameraRecordingProxy(){}
    virtual ~CameraRecordingProxy(){}
    virtual status_t        startRecording(int chnId, CameraRecordingProxyListener *pListener, int recorderId) = 0;
    virtual status_t        stopRecording(int chnId, int recorderId) = 0;
    virtual void            releaseRecordingFrame(int chnId, uint32_t frameIndex) = 0;
	virtual status_t        setChannelDisplay(int chnId, int hlay) = 0;
    virtual status_t        getParameters(int chnId, CameraParameters &param) = 0;
    virtual status_t        setParameters(int chnId, CameraParameters &param) = 0;
};

class DataListener
{
public:
    DataListener(){}
    virtual ~DataListener(){}
    virtual void postData(int msgType, int chnId, const std::shared_ptr<CMediaMemory>& dataPtr, size_t dataSize) = 0;
};


class NotifyListener
{
public:
    NotifyListener(){}
    virtual ~NotifyListener(){}
    virtual void notify(int msgType, int chnId, int ext1, int ext2) = 0;
};

}; /* namespace EyeseeLinux */

#endif /* __IPCLINUX_CAMERA_LISTENER_H__ */

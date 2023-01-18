/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : VIPPOSD.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/12/01
  Last Modified :
  Description   : some structure of VIPP OSD to share
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_VIPPOSD_H__
#define __IPCLINUX_VIPPOSD_H__

#include <string>

#include <Errors.h>
#include <mm_common.h>
#include <mm_comm_video.h>

namespace EyeseeLinux {
/*
typedef enum OSDType
{
    OSDType_Overlay = 0,
    OSDType_Cover,
}OSDType;

struct OSDRectInfo
{
    OSDType mType;
    PIXEL_FORMAT_E mFormat; //MM_PIXEL_FORMAT_RGB_8888, for overlay
    int mColor;  //for cover. ARGB
    int mPriority;  //for overlay [0, 63]; for cover [0,7];
    RECT_S mRect;
    std::string mComment;
    status_t AllocBuffer(int nBufSize);
    status_t FreeBuffer();
    void* GetBuffer();
    int GetBufferSize();
    OSDRectInfo(const OSDRectInfo& rhs);
    OSDRectInfo& operator= (const OSDRectInfo& rhs);
    OSDRectInfo();
    ~OSDRectInfo();
private:
    void *mpBuf;
    int mBufSize;
};
*/
}
#endif /* __IPCLINUX_VIPPOSD_H__ */


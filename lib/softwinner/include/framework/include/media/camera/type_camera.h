/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : type_camera.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/06/08
  Last Modified :
  Description   : camera wrap MPP components.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_TYPE_CAMERA_H__
#define __IPCLINUX_TYPE_CAMERA_H__

#include <vector>

#include <stdbool.h>
#include <mm_common.h>
#include <mm_comm_vi.h>

//#ifdef __cplusplus
//extern "C" {
//#endif

typedef struct VIDEO_FRAME_BUFFER_S
{
    VIDEO_FRAME_INFO_S mFrameBuf;
    //RECT_S mCrop;   //same as VIChannel->mRectCrop, indicate encode area in picBuf.
    int mColorSpace;
    int mRefCnt;
    //int mIsThumbAvailable;
    //int mThumbUsedForPhoto;
    int mRotation;  // anti-clock wise, 90, 180, 270
} VIDEO_FRAME_BUFFER_S;

typedef struct ISPGeometry
{
    ISP_DEV mISPDev;    //MOD_ID_ISP
    std::vector<VI_CHN> mScalerOutChns;    //MOD_ID_VIU
}ISPGeometry;

struct CameraInfo
{
    /**
     * The direction that the camera faces. It should be
     * CAMERA_FACING_BACK or CAMERA_FACING_FRONT.
     */
    enum
    {
        /**
         * The facing of the camera is opposite to that of the screen.
         */
        CAMERA_FACING_BACK = 0,
        /**
         * The facing of the camera is the same as that of the screen.
         */
        CAMERA_FACING_FRONT = 1,
    };
    int facing;  //enum CameraFacingDirection

    /**
     * <p>The orientation of the camera image. The value is the angle that the
     * camera image needs to be rotated clockwise so it shows correctly on
     * the display in its natural orientation. It should be 0, 90, 180, or 270.</p>
     *
     * <p>For example, suppose a device has a naturally tall screen. The
     * back-facing camera sensor is mounted in landscape. You are looking at
     * the screen. If the top side of the camera sensor is aligned with the
     * right edge of the screen in natural orientation, the value should be
     * 90. If the top side of a front-facing camera sensor is aligned with
     * the right of the screen, the value should be 270.</p>
     */
    int orientation;

    /**
     * <p>Whether the shutter sound can be disabled.</p>
     *
     * <p>On some devices, the camera shutter sound cannot be turned off
     * through {@link #enableShutterSound enableShutterSound}. This field
     * can be used to determine whether a call to disable the shutter sound
     * will succeed.</p>
     *
     * <p>If this field is set to true, then a call of
     * {@code enableShutterSound(false)} will be successful. If set to
     * false, then that call will fail, and the shutter sound will be played
     * when {@link Camera#takePicture takePicture} is called.</p>
     */
    bool canDisableShutterSound;

    
    enum CameraDevType
    {
        CAMERA_CSI = 0x00,
        CAMERA_USB = 0x01,
        CAMERA_TVD = 0x02,
        CAMERA_UNKNOWN = 0xFFFF,
    };
    CameraDevType mCameraDeviceType;
    /**
     * one camera can contain one or two csi input streams, stitch two of them as one picture
     * and output as one stream.
     * camera is construct by MPP modules through different combination.
     * So MPPGeometry describe this combination. 
     * This Structure is used to describe CAMERA_CSI. Other camera type such as CAMERA_USB maybe only use 
     * mScalerOutChns to describe output channels.
     * V40: only VIOutput.
     */
    struct MPPGeometry
    {
        int mCSIChn;    //MOD_ID_CSI
        std::vector<ISPGeometry> mISPGeometrys;
    } mMPPGeometry;
};

//#ifdef __cplusplus
//}
//#endif

#endif /* __IPCLINUX_TYPE_CAMERA_H__ */




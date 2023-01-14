/*
********************************************************************************
*                           eyesee framework module
*
*          (c) Copyright 2010-2018, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : EyeseeUSBCamera.h
* Version: V1.0
* By     : 
* Date   : 2018-01-11
* Description:
********************************************************************************
*/

#pragma once

#ifndef _EYESEEUSBCAMERA_H_
#define _EYESEEUSBCAMERA_H_

#include <string.h>
#include <vector>

#include <Errors.h>
#include <type_camera.h>
#include <OSAL_Queue.h>
#include <EyeseeMessageQueue.h>
#include <Mutex.h>
#include <Condition.h>

#include <mm_comm_uvc.h>
#include <mm_comm_vdec.h>

#include <CameraBufferReference.h>
#include <CameraListener.h>
#include <PreviewWindow.h>
#include <CallbackNotifier.h>
#include <CallbackDispatcher.h>
#include <UvcChnDefine.h>

namespace EyeseeLinux
{
class PreviewWindow;
class CallbackNotifier;
class EyeseeUSBCameraContext;
class UvcChannel;
class VdecFrameManager;

class EyeseeUSBCamera
{
    public: const int mCameraId;
    private: static int mCameraCounter;
public:
    EyeseeUSBCamera();
    ~EyeseeUSBCamera();
    EyeseeUSBCamera(const EyeseeUSBCamera &other) = delete; //do not allow to copy
    EyeseeUSBCamera& operator=(const EyeseeUSBCamera &other) = delete; // do not allow to operator=

    //UvcDevice
    struct USBCameraCaptureParam
    {
        std::string mpUsbCam_DevName;
        UVC_CAPTURE_FORMAT mUsbCam_CapPixelformat;
        unsigned int mUsbCam_CapWidth;
        unsigned int mUsbCam_CapHeight;
        unsigned int mUsbCam_CapFps;
        unsigned int mUsbCam_CapBufCnt;
    }; 
    status_t getUSBCameraCaptureParam(USBCameraCaptureParam &pCaptureParam) const;
    status_t setUSBCameraCaptureParam(const USBCameraCaptureParam &CaptureParam);
    
    //VdecDevice
    struct USBCameraVdecParam
    {
        unsigned int mUsbCam_VdecBufSize;
        unsigned int mUsbCam_VdecPriority;
        unsigned int mUsbCam_VdecPicWidth;
        unsigned int mUsbCam_VdecPicHeight;
        ROTATE_E mUsbCam_VdecInitRotation;
        PIXEL_FORMAT_E mUsbCam_VdecOutputPixelFormat;
        bool mUsbCam_VdecSubPicEnable;
        unsigned int mUsbCam_VdecSubPicWidth; 
        unsigned int mUsbCam_VdecSubPicHeight;
        PIXEL_FORMAT_E mUsbCam_VdecSubOutputPixelFormat; 

        unsigned int mUsbCam_VdecExtraFrameNum; //provide to user to config more output frames to vdeclib.
    };
    status_t getUSBCameraVdecParam(USBCameraVdecParam &VdecParam) const;
    status_t setUSBCameraVdecParam(const USBCameraVdecParam &VdecParam);

    status_t prepareDevice();
    status_t startDevice();
    status_t stopDevice();
    status_t releaseDevice();

    status_t openChannel(UvcChn chnId);
    status_t prepareChannel(UvcChn chnId);
    status_t startChannel(UvcChn chnId);
    status_t stopChannel(UvcChn chnId);
    status_t releaseChannel(UvcChn chnId);
    status_t closeChannel(UvcChn chnId);
    /**
     * get uvcChannel parameters after openChannel()
     */
    status_t setParameters(UvcChn chnId, CameraParameters &param);
    /**
     * set uvcChannel parameters after openChannel()
     */
    status_t getParameters(UvcChn chnId, CameraParameters &param);
    
    status_t setChannelDisplay(UvcChn chnId, VO_LAYER VoLayer); // 0: uvc main stream, 1: vdec main 2: vdec sub.
    bool previewEnabled(UvcChn chnId);
    /**
     * use hlay to show picture.
     * Or not show picture.
     */
    status_t startRender(UvcChn chnId);
    status_t stopRender(UvcChn chnId);

    void releaseRecordingFrame(UvcChn chnId, uint32_t index);
    status_t startRecording(UvcChn chnId, CameraRecordingProxyListener *pCb, int recorderId);
    status_t stopRecording(UvcChn chnId, int recorderId);

    class USBCameraProxy : public CameraRecordingProxy
    {
    public:
        USBCameraProxy(EyeseeUSBCamera *pUSBCameraParam);
        virtual status_t        startRecording(int chnId, CameraRecordingProxyListener *pListener, int recorderId) override;
        virtual status_t        stopRecording(int chnId, int recorderId) override;
        virtual void            releaseRecordingFrame(int chnId, uint32_t frameIndex) override;
        virtual status_t        setChannelDisplay(int chnId, int hlay) override;
        virtual status_t        getParameters(int chnId, CameraParameters &param) override;
        virtual status_t        setParameters(int chnId, CameraParameters &param) override;
    private:
        EyeseeUSBCamera *mpUSBCamera;
    };
    CameraRecordingProxy* getRecordingProxy();

//states
    enum class State 
    {
        ERROR        = 0,
        IDLE               = 1 << 0,
        //INITIALIZED        = 1 << 1,
        //PREPARING          = 1 << 2,
        PREPARED           = 1 << 3,
        STARTED            = 1 << 4,
        //STOPPED            = 1 << 5
    };
    /**
     * Callback interface for camera error notification.
     *
     * @see #setErrorCallback(ErrorCallback)
     */
    class ErrorCallback
    {
    public:
		ErrorCallback(){}
		virtual ~ErrorCallback(){}
        /**
         * Callback for camera errors.
         * @param error   error code:
         * <ul>
         * <li>{@link #CAMERA_ERROR_UNKNOWN}
         * <li>{@link #CAMERA_ERROR_SERVER_DIED}
         * </ul>
         * @param camera  the Camera service object
         */
        virtual void onError(UvcChn chnId, CameraMsgErrorType error, EyeseeUSBCamera *pCamera) = 0;
    };

    /**
     * Registers a callback to be invoked when an error occurs.
     * @param cb The callback to run
     */
    void setErrorCallback(ErrorCallback *pCb);

    class InfoCallback
    {
    public:
        /**
         * Called to indicate an info or a warning.
         *
         * @param pCamera      the camera the info pertains to.
         * @param what    the type of info or warning.
         * @param extra an extra code, specific to the info. Typically
         * implementation dependent.
         * @return True if the method handled the info, false if it didn't.
         */
		InfoCallback(){}
		virtual ~InfoCallback(){}  
        virtual bool onInfo(UvcChn chnId, CameraMsgInfoType what, int extra, EyeseeUSBCamera *pCamera) = 0;
    };
    /**
     * Register a callback to be invoked when an info/warning is available.
     *
     * @param pCb the callback that will be run
     */
    void setInfoCallback(InfoCallback *pCb);

    /**
     * Callback interface used to supply image data from a photo capture.
     *
     * @see #takePicture(ShutterCallback, PictureCallback, PictureCallback, PictureCallback)
     */
    class PictureCallback 
    {
    public:
		PictureCallback(){}
		virtual ~PictureCallback(){}
        /**
         * Called when image data is available after a picture is taken.
         * The format of the data depends on the context of the callback
         * and {@link Camera.Parameters} settings.
         *
         * @param data   a byte array of the picture data
         *          data = jpeg + thumbOffset(4 bytes) + thumbLen(4 bytes) + jpegSize(4 bytes)
         * @param pPictureROI these are faces region array, take picture for every face in the image.
         * @param camera the Camera service object
         */
        virtual void onPictureTaken(UvcChn chnId, const void *data, int size, EyeseeUSBCamera* pCamera) = 0;
    };
    status_t takePicture(UvcChn chnId, PictureCallback *pPostview, PictureCallback *pJpeg, PictureRegionCallback *pPicRegion);

    static void postEventFromNative(EyeseeUSBCamera* pC, int what, int arg1, int arg2, int arg3, const std::shared_ptr<CMediaMemory>* pDataPtr = NULL);

private:
    USBCameraCaptureParam mCaptureParam;
    bool mbCaptureFlag; //if set capture param.
    std::string mUvcDev;
    UVC_ATTR_S mUvcDevAttr;
    UVC_CHN mVirtualUvcChnForVdec;
    

    USBCameraVdecParam mVdecParam;
    bool mbVdecFlag; // set vdec param, vdec will exist.
    bool mbVdecSubOutputFlag;
    VDEC_CHN mVdecChn;
    VDEC_CHN_ATTR_S mVdecChnAttr;

    VdecFrameManager *mpVdecFrameManager;

    State mCurrentState;
    Mutex mLock;

    class EventHandler : public CallbackDispatcher
    {
    public:
        EventHandler(EyeseeUSBCamera *pC);
        virtual void handleMessage(const CallbackMessage &msg) override;
    private:
        EyeseeUSBCamera *mpCamera;
    };
    EventHandler    *mpEventHandler;
    ErrorCallback   *mpErrorCallback;
    InfoCallback    *mpInfoCallback;
    struct TakePictureCallback
    {
        PictureCallback *mpJpegCallback;
        PictureCallback *mpPostviewCallback;
        PictureRegionCallback *mpPictureRegionCallback;
     };
    std::map<UvcChn, TakePictureCallback> mTakePictureCallback;
    Mutex mSetTakePictureCallbackLock;   

    EyeseeUSBCameraContext *mpNativeContext;
    status_t openUSBCamera();
    status_t openVdecDevice();
    void clearConfig();
    UvcChannel *searchUvcChannel(UvcChn chnId);
    UvcChannel *searchUvcChannel_l(UvcChn chnId);
    
    struct UvcChannelInfo
    {
        UvcChn mChnId;
        UvcChannel *mpChannel;
    };
    std::list<UvcChannelInfo> mUvcChnList;
    Mutex mUvcChnListLock;

    static ERRORTYPE MPPVdecCallback(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData);
};

};

#endif


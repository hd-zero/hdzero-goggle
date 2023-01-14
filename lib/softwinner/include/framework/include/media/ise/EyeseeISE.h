/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : EyeseeISE.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/05
  Last Modified :
  Description   : ISE module.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_EYESEEISE_H__
#define __IPCLINUX_EYESEEISE_H__

#include <vector>

#include <plat_type.h>
#include <plat_errno.h>
#include <plat_defines.h>
#include <plat_math.h>
#include <Errors.h>

#include <utils/CallbackDispatcher.h>
#include <EyeseeISECommon.h>
#include <camera.h>
#include <CameraParameters.h>
#include <CameraListener.h>

#include <mm_comm_ise.h>

//#include <aw_ai_cve_dtca_interface.h>
//#include <aw_ai_cve_vlpr_interface.h>
//#include <aw_ai_eve_event_interface.h>
#include <PictureRegionCallback.h>


namespace EyeseeLinux {

class ISEDevice;
class EyeseeISEContext;
class EyeseeISE
{
public:
    EyeseeISE();
    ~EyeseeISE();

    static EyeseeISE *open();
    static void close(EyeseeISE *pIse);

    /**
     * @brief Prepares the ise device to begin capture channel data.
     * @param mode indicate ISE_MODE_E.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t prepareDevice(ISE_MODE_E mode,PIXEL_FORMAT_E pixelformat);

    /**
     * @brief Release the ise device. Call this after prepareDevice().
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t releaseDevice();

    /**
     * @brief Start the ise device.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t startDevice();

    status_t startChannel(int chnId);

    /**
     * @brief Stop the ise device. this method must be call after startDevice().
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t stopDevice();

    /**
     * @brief Open a ise channel.
     * @param pChnAttr the channel attributes.
     * @return ISE_CHN if channel open successfully, ISE_CHN>=0, otherwise return MM_INVALID_CHN.
     */
    ISE_CHN openChannel(ISE_CHN_ATTR_S *pChnAttr);

    /**
     * @brief close a ise channel opened by openChannel().
     * @param chnId the channel ID to be opened.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t closeChannel(ISE_CHN chnId);

    /**
     * @brief Set vi channel's parameters.
     * @param param parameters to be set.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t setParameters(int chnId, CameraParameters &param);

    /**
     * @brief Get vi channel's parameters.
     * @param param parameters to be get.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t getParameters(int chnId, CameraParameters &param);

    /**
     * @brief set ise channel's display surface.
     * @param chnId the channel to be set.
     * @param hlay display surface.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t setChannelDisplay(int chnId, int hlay);

    /**
     * Return current preview state.
     *
     * FIXME: Unhide before release
     * @hide
     */
    bool previewEnabled(int chnId);
    /**
     * use hlay to show picture.
     * Or not show picture.
     */
    status_t startRender(int chnId);
    status_t stopRender(int chnId);

    /**
     * @brief set ise MoAttr for ptz mode.
     * @param pAttr attr for ptz mode, whose pan, tilt and zoom is needed.
     * @return 0 if set successfully, otherwise return error code.
     */
    status_t setMoPortAttr(ISE_CHN_ATTR_S *pAttr);

    /**
     * @brief Release a recording frame.
     * @param chnId the channel opened by openChannel().
     * @param index frame buffer index.
     */
    void releaseRecordingFrame(int chnId, uint32_t index);

    /**
     * @brief start recording mode. 
     * @chnId the channel opened by openChannel().
     * @param pCb listener that allows the recorder to receive video frames during recording.
     * @param recorderId the recorder identifier.
     @ @return 0 if start record successfully, otherwise return error code.
     */
    status_t startRecording(int chnId, CameraRecordingProxyListener *pCb, int recorderId);

    /**
     * @brief stop recording mode. 
     * @chnId the channel opened by openChannel().
     * @param recorderId the recorder identifier.
     @ @return 0 if stop record successfully, otherwise return error code.
     */
    status_t stopRecording(int chnId, int recorderId);

    CameraRecordingProxy* getRecordingProxy();

    /**
     * Sets a Camera to use for image stitching or ptz decomposing. Must call after prepareDevice().
     *
     * @param pCam0 the first Camera to use for stitching or decomposing
     * @param nCam0Chn the first Camera source channel id
     * @param pCam1 the second Camera to use for stitching or decomposing
     * @param nCam1Chn the second Camera source channel id
     */

    status_t setCamera(std::vector<CameraChannelInfo>& cameraChannels);
    //status_t setCamera(EyeseeCamera *pCam0, int nCam0Chn, EyeseeCamera *pCam1, int nCam1Chn);

    /**
     * Callback interface used to signal the moment of actual image capture.
     *
     * @see #takePicture(ShutterCallback, PictureCallback, PictureCallback, PictureCallback)
     */
    class ShutterCallback
    {
    public:
        ShutterCallback(){}
        virtual ~ShutterCallback(){}
        /**
         * Called as near as possible to the moment when a photo is captured
         * from the sensor.  This is a good opportunity to play a shutter sound
         * or give other feedback of camera operation.  This may be some time
         * after the photo was triggered, but some time before the actual data
         * is available.
         */
        virtual void onShutter(int chnId) = 0;
    };

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
         * @param camera the Camera service object
         */
        virtual void onPictureTaken(int chnId, const void *data, int size, EyeseeISE* pISE) = 0;
    };

    /**
     * Equivalent to takePicture(shutter, raw, null, jpeg).
     *
     * @see #takePicture(ShutterCallback, PictureCallback, PictureCallback, PictureCallback)
     */
    status_t takePicture(int chnId, ShutterCallback *pShutter, PictureCallback *pRaw,
            PictureCallback *pJpeg, PictureRegionCallback *pPicReg);

    /**
     * Triggers an asynchronous image capture. The camera service will initiate
     * a series of callbacks to the application as the image capture progresses.
     * The shutter callback occurs after the image is captured. This can be used
     * to trigger a sound to let the user know that image has been captured. The
     * raw callback occurs when the raw image data is available (NOTE: the data
     * will be null if there is no raw image callback buffer available or the
     * raw image callback buffer is not large enough to hold the raw image).
     * The postview callback occurs when a scaled, fully processed postview
     * image is available (NOTE: not all hardware supports this). The jpeg
     * callback occurs when the compressed image is available. If the
     * application does not need a particular callback, a null can be passed
     * instead of a callback method.
     *
     * <p>This method is only valid when preview is active (after
     * {@link #startPreview()}).  Preview will be stopped after the image is
     * taken; callers must call {@link #startPreview()} again if they want to
     * re-start preview or take more pictures. This should not be called between
     * {@link android.media.MediaRecorder#start()} and
     * {@link android.media.MediaRecorder#stop()}.
     *
     * <p>After calling this method, you must not call {@link #startPreview()}
     * or take another picture until the JPEG callback has returned.
     *
     * @param shutter   the callback for image capture moment, or null
     * @param raw       the callback for raw (uncompressed) image data, or null
     * @param postview  callback with postview image data, may be null
     * @param jpeg      the callback for JPEG image data, or null
     */
    status_t takePicture(int chnId, ShutterCallback *pShutter, PictureCallback *pRaw,
            PictureCallback *pPostview, PictureCallback *pJpeg, PictureRegionCallback *pPicReg);

    /**
     * cancel continuous take picture which is going on.
     * @param chnId indicate ise channel.
     */
    status_t cancelContinuousPicture(int chnId);

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
        virtual void onError(int chnId, int error, EyeseeISE *pISE) = 0;
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
         * @param pIse:    the ise the info pertains to.
         * @param what:    the type of info or warning.
         * @param extra:   an extra code, specific to the info. Typically implementation dependent.
         * @return:        True if the method handled the info, false if it didn't.
         */
        InfoCallback(){}
        virtual ~InfoCallback(){}
        virtual bool onInfo(int chnId, CameraMsgInfoType what, int extra, EyeseeISE *pISE) = 0;
    };
    /**
     * Register a callback to be invoked when an info/warning is available.
     *
     * @param pCb the callback that will be run
     */
    void setInfoCallback(InfoCallback *pCb);

    /**
     * Callback interface used to deliver copies of preview frames as
     * they are displayed.
     */
    class PreviewCallback
    {
    public:
        PreviewCallback(){}
        virtual ~PreviewCallback(){}
        virtual void onPreviewFrame(const void *data, int size, EyeseeISE* pISE) = 0;
    };
    /**
     * <p>Installs a callback to be invoked for every preview frame in addition
     * to displaying them on the screen.  The callback will be repeatedly called
     * for as long as preview is active.  This method can be called at any time,
     * even while preview is live.  Any other preview callbacks are
     * overridden.</p>
     *
     * <p>If you are using the preview data to create video or still images,
     * strongly consider using {@link android.media.MediaActionSound} to
     * properly indicate image capture or recording start/stop to the user.</p>
     *
     * @param cb a callback object that receives a copy of each preview frame,
     *     or null to stop receiving callbacks.
     */
    void setPreviewCallback(PreviewCallback *pCb);


    static void postEventFromNative(EyeseeISE* pC, int what, int arg1, int arg2, int arg3, const std::shared_ptr<CMediaMemory>* pDataPtr=NULL);

private:
    class EventHandler : public CallbackDispatcher
    {
    public:
        EventHandler(EyeseeISE *pISE) : mpISE(pISE) {}
        virtual ~EventHandler(){};
        virtual void handleMessage(const CallbackMessage &msg);
    private:
        EyeseeISE *mpISE;
    };
    //void postDataCompleted(int chnId, const void *pData, int size);

    class ISEProxy : public CameraRecordingProxy    // recorder get this, it return frame to ise
    {
    public:
        ISEProxy(EyeseeISE *pISE) : mpISE(pISE) {}
        virtual ~ISEProxy() {};

        // CameraRecordingProxy interface
        virtual status_t        startRecording(int chnId, CameraRecordingProxyListener *pListener, int recorderId);
        virtual status_t        stopRecording(int chnId, int recorderId);
        virtual void            releaseRecordingFrame(int chnId, uint32_t frameIndex);
        virtual status_t        setChannelDisplay(int chnId, int hlay);
        virtual status_t        getParameters(int chnId, CameraParameters &param);
        virtual status_t        setParameters(int chnId, CameraParameters &param);
    private:
        EyeseeISE *mpISE;
    };

    static unsigned int gISEIdCounter;
    const unsigned int mISEId;
    ISEDevice *mpISEDevice;
    EyeseeISEContext *mpNativeContext;
    EventHandler *mpEventHandler;

    ShutterCallback *mpShutterCallback;
    PictureCallback *mpRawImageCallback;
    PictureCallback *mpJpegCallback;
    PreviewCallback *mpPreviewCallback;
    PictureCallback *mpPostviewCallback;
    ErrorCallback   *mpErrorCallback;
    InfoCallback    *mpInfoCallback;
    //PictureRegionCallback *mpPictureRegionCallback;
    struct TakePictureCallback
    {
        ShutterCallback *mpShutterCallback;
        PictureCallback *mpRawImageCallback;
        PictureCallback *mpJpegCallback;
        PictureCallback *mpPostviewCallback;
        PictureRegionCallback *mpPictureRegionCallback;
     };
    std::map<int, TakePictureCallback> mTakePictureCallback;
    Mutex mSetTakePictureCallbackLock;
}; /* class EyeseeISE */

}; /* namespace EyeseeLinux */

#endif /* __IPCLINUX_EYESEEISE_H__ */


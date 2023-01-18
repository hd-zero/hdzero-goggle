/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : EyeseeCamera.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/04/29
  Last Modified :
  Description   : camera wrap MPP components.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_EYESEECAMERA_H__
#define __IPCLINUX_EYESEECAMERA_H__

#include <stdbool.h>

#include <vector>
#include <map>

#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

#include "Errors.h"

#include <mm_common.h>
#include <mm_comm_vi.h>
#include <mm_comm_region.h>

#include <utils/CallbackDispatcher.h>
#include <camera.h>
#include <type_camera.h>
#include <CameraParameters.h>
#include <CameraListener.h>
//#include <VIPPOSD.h>
#include <ModData.h>
#include <PictureRegionCallback.h>

namespace EyeseeLinux {

class VIDevice;
class EyeseeCamera
{
public:
    /**
     * Returns the number of physical cameras available on this device.
     * <p class="caution"><b>Caution:</b>must first call {@link #configCameraWithMPPModules()} to config cameras,
     * then call this function to get correct number of cameras.
     */
    static int getNumberOfCameras();

    /**
     * Returns the information about a particular camera.
     * @param cameraId [in] user indicate camera using cameraId.
     * @param pCameraInfo [out]
     */
    static status_t getCameraInfo(int cameraId, CameraInfo *pCameraInfo);
    /**
     * Return all cameras info.
     */
    static status_t getCameraInfos(std::map<int, CameraInfo>& cameraInfos);
    /**
     * clear cameras configuration. 
     * If app want to reconfig camera, it must fisrt call this function to clean original
     * configuration, then call {@link #configCameraWithMPPModules()} to reconfig.
     */
    static status_t clearCamerasConfiguration();
    /**
     * camera can contain VideoInput, ISP, video scaler MPP modules.
     * So config these MPP modules to camera here.
     * @param cameraId user set its value to identify camera which he config.
     *      Strongly suggest cameraId start from 0 and increase contiguously.
     * @param pCameraInfo cameraConfiguration.
     */
    static status_t configCameraWithMPPModules(int cameraId, CameraInfo *pCameraInfo);

    /**
     * Creates a new Camera object to access a particular hardware camera. If
     * the same camera is opened by other applications, this will throw a
     * RuntimeException.
     *
     * <p>You must call {@link #release()} when you are done using the camera,
     * otherwise it will remain locked and be unavailable to other applications.
     *
     * <p>Your application should only have one Camera object active at a time
     * for a particular hardware camera.
     *
     * <p>Callbacks from other methods are delivered to the event loop of the
     * thread which called open().  If this thread has no event loop, then
     * callbacks are delivered to the main application event loop.  If there
     * is no main application event loop, callbacks are not delivered.
     *
     * <p class="caution"><b>Caution:</b> On some devices, this method may
     * take a long time to complete.  It is best to call this method from a
     * worker thread (possibly using {@link android.os.AsyncTask}) to avoid
     * blocking the main application UI thread.
     *
     * @param cameraId the hardware camera to access, between 0 and
     *     {@link #getNumberOfCameras()}-1.
     * @return a new Camera object, connected, locked and ready for use.
     * @throws RuntimeException if opening the camera fails (for example, if the
     *     camera is in use by another process or device policy manager has
     *     disabled the camera).
     */
    static EyeseeCamera* open(int cameraId);

    /**
     * @brief Destroy a EyeseeCamera created by open().
     * @param pEyeseeCam EyeseeCamera to be destroy.
     */
    static void close(EyeseeCamera* pEyeseeCam);

    EyeseeCamera(int cameraId);
    ~EyeseeCamera();

    /**
     * @brief Open a vi channel.
     * @param chnId the channel ID to be opened.
     * @param bForceRef force ISP use current channel param to try sensor param. If ISP has already adopt sensor param
     *          and can't try again, return failure.
     * @return 0 if channel open successfully, otherwise return error code.
     */
    status_t openChannel(int chnId, bool bForceRef);

    /**
     * @brief close a vi channel opened by openChannel().
     * @param chnId the channel ID to be opened.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t closeChannel(int chnId);

    /**
     * @brief set vi device attribute.
     * @param devAttr vi device attribute.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    //status_t setDeviceAttr(VI_DEV_ATTR_S *devAttr);

    /**
     * @brief get vi device attribute.
     * @param devAttr vi device attribute.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    //status_t getDeviceAttr(VI_DEV_ATTR_S *devAttr);

    /**
     * @brief set vi channel's display surface.
     * @param chnId the channel to be set.
     * @param hlay display surface.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t setChannelDisplay(int chnId, int hlay);

    /**
     * @brief Prepares the vi device to begin capture channel data. this method
     * must be call after setDeviceAttr(), but before prepareChannel().
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t prepareDevice();

    /**
     * @brief Release the vi device. Call this after prepareDevice().
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t releaseDevice();

    /**
     * @brief Prepares the vi channel to begin capture data. this method
     * must be call after openChannel(), but before startDevice().
     * @param chnId the channel opened by openChannel().
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t prepareChannel(int chnId);

    /**
     * @brief Release the vi channel. this method must be call after
     * prepareChannel(), but before closeChannel().
     * @param chnId the channel opened by openChannel().
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t releaseChannel(int chnId);

    /**
     * @brief Start the vi device. this method must be call after at least one
     * channel was preparedprepare.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t startDevice();

    /**
     * @brief Stop the vi device. this method must be call after startDevice().
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t stopDevice();

    /**
     * @brief Start capture channel data. Call this method after startDevice()
     * and prepareChannel().
     * @param chnId the channel opened by openChannel().
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t startChannel(int chnId);

    /**
     * @brief Start capture channel data. Call this method after startChannel().
     * @param chnId the channel opened by openChannel().
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t stopChannel(int chnId);
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
     * find a frame whose pts is near framePts, store to file.
     * for debug.
     */
    status_t storeDisplayFrame(int chnId, uint64_t framePts);
    
    /**
     * Motion Object Detection
     * @brief get current MOD params from designated channel.
     * @param chnId the channel opened.
     * @param pParamDTCA Detect, Track, Classify, Analysis parameters. can set to NULL, indicate don't want to get its value.
     * @param pParamCLBR calibration parameters, can set to NULL, indicate don't want to get its value.
     * @return NO_ERROR if successful.
     */
    status_t getMODParams(int chnId, MOTION_DETECT_ATTR_S *pParamMD);
    /**
     * Motion Object Detection
     * @brief set MOD params to designated channel. Call this method before start MOD detect.
     * @param chnId the channel opened.
     * @param pParamDTCA Detect, Track, Classify, Analysis parameters. It can be set to NULL, indicate don't want to set its value.
     * @param pParamCLBR calibration parameters. It can be set to NULL, indicate don't want to set its value.
     * @return NO_ERROR if successful.
     */
    status_t setMODParams(int chnId, MOTION_DETECT_ATTR_S pParamMD);
    /**
     * @brief start Motion Object Detection. Call this method after startChannel().
     * @param chnId the channel opened.
     * @return NO_ERROR if successful.
     */
    status_t startMODDetect(int chnId);
    /**
     * @brief stop Motion Object Detection. Call this method before stopChannel().
     * @param chnId the channel opened.
     * @return NO_ERROR if successful.
     */
    status_t stopMODDetect(int chnId);
    /**
     * @brief Set vi channel's parameters. Call this method after openChannel().
     * @param chnId the channel opened by openChannel().
     * @param param parameters to be set.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    /**
     * @brief Set vi channel's parameters. Call this method after openChannel().
     * @param chnId the channel opened by openChannel().
     * @param param parameters to be set.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t setParameters(int chnId, CameraParameters &param);

    
    status_t setPicCapMode(int chnId, uint32_t cap_mode_en); 
    status_t setFrmDrpThrForPicCapMode(int chnId,uint32_t frm_cnt);

    /**
     * @brief Get vi channel's parameters. Call this method after openChannel().
     * @param chnId the channel opened by openChannel().
     * @param param parameters to be get.
     * @return 0 if channel closed successfully, otherwise return error code.
     */
    status_t getParameters(int chnId, CameraParameters &param);

    /**
     * @brief set/get ISP parameters.
     * @param param parameters to be set/get.
     * @return NO_ERORR if channel closed successfully, otherwise return error code.
     */
    status_t setISPParameters(CameraParameters &param);
    status_t getISPParameters(CameraParameters &param);

    /* WATERMARK_ENABLE start*/
    //status_t  setWaterMark(int enable, const char *str);
	/* WATERMARK_ENABLE end*/
    /**
     * @param chnId VIPP id.
     */
//    status_t setOSDRects(int chnId, std::list<OSDRectInfo> &rects);
//    status_t getOSDRects(int chnId, std::list<OSDRectInfo> **ppRects);
//    status_t OSDOnOff(int chnId, bool bOnOff);

    RGN_HANDLE createRegion(const RGN_ATTR_S *pstRegion);
    status_t getRegionAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRgnAttr);
    status_t setRegionBitmap(RGN_HANDLE Handle, const BITMAP_S *pBitmap);
    status_t attachRegionToChannel(RGN_HANDLE Handle, int chnId, const RGN_CHN_ATTR_S *pstChnAttr);
    status_t detachRegionFromChannel(RGN_HANDLE Handle, int chnId);
    status_t setRegionDisplayAttr(RGN_HANDLE Handle, int chnId, const RGN_CHN_ATTR_S *pstChnAttr);
    status_t getRegionDisplayAttr(RGN_HANDLE Handle, int chnId, RGN_CHN_ATTR_S *pstChnAttr);
    status_t destroyRegion(RGN_HANDLE Handle);
    status_t getISPDMsg(int * exp, int * exp_line, int * gain, int * lv_idx, int * color_temp, int * rgain, int * bgain, int *grgain, int * gbgain);
    
    /**
     * @brief Release a recording frame.
     * @param chnId the channel opened by openChannel().
     * @param index frame buffer index.  */
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

    class CameraProxy : public CameraRecordingProxy
    {
    public:
        CameraProxy(EyeseeCamera *pCamera);

        // CameraRecordingProxy interface
        virtual status_t        startRecording(int chnId, CameraRecordingProxyListener *pListener, int recorderId);
        virtual status_t        stopRecording(int chnId, int recorderId);
        virtual void            releaseRecordingFrame(int chnId, uint32_t frameIndex);
        virtual status_t        setChannelDisplay(int chnId, int hlay);
        virtual status_t        getParameters(int chnId, CameraParameters &param);
        virtual status_t        setParameters(int chnId, CameraParameters &param);
    private:
        EyeseeCamera *mpCamera;
    };
    CameraRecordingProxy* getRecordingProxy();

    /**
     * Callback interface used to deliver copies of preview frames as
     * they are displayed.
     */
    class PreviewCallback
    {
    public:
        PreviewCallback(){}
        virtual ~PreviewCallback(){}
        virtual void onPreviewFrame(const void *data, int size, EyeseeCamera* pCamera) = 0;
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
    
    /**
     * <p>Installs a callback to be invoked for the next preview frame in
     * addition to displaying it on the screen.  After one invocation, the
     * callback is cleared. This method can be called any time, even when
     * preview is live.  Any other preview callbacks are overridden.</p>
     *
     * <p>If you are using the preview data to create video or still images,
     * strongly consider using {@link android.media.MediaActionSound} to
     * properly indicate image capture or recording start/stop to the user.</p>
     *
     * @param cb a callback object that receives a copy of the next preview frame,
     *     or null to stop receiving callbacks.
     */
    void setOneShotPreviewCallback(PreviewCallback *pCb);

    /**
     * <p>Installs a callback to be invoked for every preview frame, using
     * buffers supplied with {@link #addCallbackBuffer(byte[])}, in addition to
     * displaying them on the screen.  The callback will be repeatedly called
     * for as long as preview is active and buffers are available.  Any other
     * preview callbacks are overridden.</p>
     *
     * <p>The purpose of this method is to improve preview efficiency and frame
     * rate by allowing preview frame memory reuse.  You must call
     * {@link #addCallbackBuffer(byte[])} at some point -- before or after
     * calling this method -- or no callbacks will received.</p>
     *
     * <p>The buffer queue will be cleared if this method is called with a null
     * callback, {@link #setPreviewCallback(Camera.PreviewCallback)} is called,
     * or {@link #setOneShotPreviewCallback(Camera.PreviewCallback)} is
     * called.</p>
     *
     * <p>If you are using the preview data to create video or still images,
     * strongly consider using {@link android.media.MediaActionSound} to
     * properly indicate image capture or recording start/stop to the user.</p>
     *
     * @param cb a callback object that receives a copy of the preview frame,
     *     or null to stop receiving callbacks and clear the buffer queue.
     */
    void setPreviewCallbackWithBuffer(PreviewCallback *pCb);
    /**
     * Adds a pre-allocated buffer to the preview callback buffer queue.
     * Applications can add one or more buffers to the queue. When a preview
     * frame arrives and there is still at least one available buffer, the
     * buffer will be used and removed from the queue. Then preview callback is
     * invoked with the buffer. If a frame arrives and there is no buffer left,
     * the frame is discarded. Applications should add buffers back when they
     * finish processing the data in them.
     *
     * <p>For formats besides YV12, the size of the buffer is determined by
     * multiplying the preview image width, height, and bytes per pixel. The
     * width and height can be read from
     * {@link Camera.Parameters#getPreviewSize()}. Bytes per pixel can be
     * computed from {@link android.graphics.ImageFormat#getBitsPerPixel(int)} /
     * 8, using the image format from
     * {@link Camera.Parameters#getPreviewFormat()}.
     *
     * <p>If using the {@link android.graphics.ImageFormat#YV12} format, the
     * size can be calculated using the equations listed in
     * {@link Camera.Parameters#setPreviewFormat}.
     *
     * <p>This method is only necessary when
     * {@link #setPreviewCallbackWithBuffer(PreviewCallback)} is used. When
     * {@link #setPreviewCallback(PreviewCallback)} or
     * {@link #setOneShotPreviewCallback(PreviewCallback)} are used, buffers
     * are automatically allocated. When a supplied buffer is too small to
     * hold the preview frame data, preview callback will return null and
     * the buffer will be removed from the buffer queue.
     *
     * @param callbackBuffer the buffer to add to the queue. The size of the
     *   buffer must match the values described above.
     */
    void addCallbackBuffer(CMediaMemory &callbackBuffer);
    /**
     * Adds a pre-allocated buffer to the raw image callback buffer queue.
     * Applications can add one or more buffers to the queue. When a raw image
     * frame arrives and there is still at least one available buffer, the
     * buffer will be used to hold the raw image data and removed from the
     * queue. Then raw image callback is invoked with the buffer. If a raw
     * image frame arrives but there is no buffer left, the frame is
     * discarded. Applications should add buffers back when they finish
     * processing the data in them by calling this method again in order
     * to avoid running out of raw image callback buffers.
     *
     * <p>The size of the buffer is determined by multiplying the raw image
     * width, height, and bytes per pixel. The width and height can be
     * read from {@link Camera.Parameters#getPictureSize()}. Bytes per pixel
     * can be computed from
     * {@link android.graphics.ImageFormat#getBitsPerPixel(int)} / 8,
     * using the image format from {@link Camera.Parameters#getPreviewFormat()}.
     *
     * <p>This method is only necessary when the PictureCallbck for raw image
     * is used while calling {@link #takePicture(Camera.ShutterCallback,
     * Camera.PictureCallback, Camera.PictureCallback, Camera.PictureCallback)}.
     *
     * <p>Please note that by calling this method, the mode for
     * application-managed callback buffers is triggered. If this method has
     * never been called, null will be returned by the raw image callback since
     * there is no image callback buffer available. Furthermore, When a supplied
     * buffer is too small to hold the raw image data, raw image callback will
     * return null and the buffer will be removed from the buffer queue.
     *
     * @param callbackBuffer the buffer to add to the raw image callback buffer
     *     queue. The size should be width * height * (bits per pixel) / 8. An
     *     null callbackBuffer will be ignored and won't be added to the queue.
     */
    void addRawImageCallbackBuffer(CMediaMemory &callbackBuffer);

    static void postEventFromNative(EyeseeCamera* pC, int what, int arg1, int arg2, int arg3, const std::shared_ptr<CMediaMemory>* pDataPtr = NULL);

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
         *          data = jpeg + thumbOffset(4 bytes) + thumbLen(4 bytes) + jpegSize(4 bytes)
         * @param camera the Camera service object
         */
        virtual void onPictureTaken(int chnId, const void *data, int size, EyeseeCamera* pCamera) = 0;
    };

    /**
     * Equivalent to takePicture(shutter, raw, null, jpeg).
     *
     * @see #takePicture(ShutterCallback, PictureCallback, PictureCallback, PictureCallback)
     */
    status_t takePicture(int chnId, ShutterCallback *pShutter, PictureCallback *pRaw,
            PictureCallback *pJpeg, PictureRegionCallback *pPicReg = NULL);

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
     * @param chnId indicate camera channel.
     */
    status_t cancelContinuousPicture(int chnId);
    /**
     * <p>Enable or disable the default shutter sound when taking a picture.</p>
     *
     * <p>By default, the camera plays the system-defined camera shutter sound
     * when {@link #takePicture} is called. Using this method, the shutter sound
     * can be disabled. It is strongly recommended that an alternative shutter
     * sound is played in the {@link ShutterCallback} when the system shutter
     * sound is disabled.</p>
     *
     * <p>Note that devices may not always allow disabling the camera shutter
     * sound. If the shutter sound state cannot be set to the desired value,
     * this method will return false. {@link CameraInfo#canDisableShutterSound}
     * can be used to determine whether the device will allow the shutter sound
     * to be disabled.</p>
     *
     * @param enabled whether the camera should play the system shutter sound
     *                when {@link #takePicture takePicture} is called.
     * @return {@code true} if the shutter sound state was successfully
     *         changed. {@code false} if the shutter sound state could not be
     *         changed. {@code true} is also returned if shutter sound playback
     *         is already set to the requested state.
     */
    bool enableShutterSound(bool enabled);
    /**
     * For take picture speed, we can keep encoder.
     */
    status_t KeepPictureEncoder(int chnId, bool bKeep);
    status_t releasePictureEncoder(int chnId);

    /**
     * Callback interface used to supply MotionObjectDetection data from one camera channel.
     */
    class MODDataCallback
    {
    public:
		MODDataCallback(){}
		virtual ~MODDataCallback(){}
        /**
         * Called when vda data is available during frame capture.
         * The format of the data is AW_AI_CVE_DTCA_RESULT_S.
         *
         * @param chnId camera channel.
         * @param data MOD result struct.
         * @param camera the Camera service object
         */
		virtual void onMODData(int chnId, MotionDetectResult *p_sResult,EyeseeCamera* pCamera) = 0;
    };
    /**
     * <p>Installs a callback to be invoked for every MOD data which coming when generated.
     * The callback will be repeatedly called for as long as MOD detection is active.  
     * This method can be called at any time, even while MOD detect is live.  
     * Any other MOD callbacks are overridden.</p>
     *
     * <p>If you are using the MOD data, data will be valid during onMODData() is called.
     * After called, data will be invalid.</p>
     *
     * @param pCb a callback object that receives a MOD data object,
     *     or null to stop receiving callbacks.
     */
    void setMODDataCallback(MODDataCallback *pCb);

    // Error codes match the enum in include/ui/Camera.h
    enum {
        /**
         * Unspecified camera error.
         * @see Camera.ErrorCallback
         */
        CAMERA_ERROR_UNKNOWN = 1,

        /**
         * Media server died. In this case, the application must release the
         * Camera object and instantiate a new one.
         * @see Camera.ErrorCallback
         */
        CAMERA_ERROR_SERVER_DIED = 100,
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
        virtual void onError(int chnId, int error, EyeseeCamera *pCamera) = 0;
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
        virtual bool onInfo(int chnId, CameraMsgInfoType what, int extra, EyeseeCamera *pCamera) = 0;
    };
    /**
     * Register a callback to be invoked when an info/warning is available.
     *
     * @param pCb the callback that will be run
     */
    void setInfoCallback(InfoCallback *pCb);

private:
    class EventHandler : public CallbackDispatcher
    {
    public:
        EventHandler(EyeseeCamera *pC);
        virtual void handleMessage(const CallbackMessage &msg);
    private:
        EyeseeCamera *mpCamera;
    };
    void setHasPreviewCallback(bool installed, bool manualBuffer);
    void addCallbackBuffer(CMediaMemory &callbackBuffer, int msgType);
    void _addCallbackBuffer(CMediaMemory &callbackBuffer, int msgType);

    int             mCameraId;
    void            *mpNativeContext;
    EventHandler    *mpEventHandler;
    //ShutterCallback *mpShutterCallback;
    //PictureCallback *mpRawImageCallback;
    //PictureCallback *mpJpegCallback;
    PreviewCallback *mpPreviewCallback;
    //PictureCallback *mpPostviewCallback;
    MODDataCallback *mpMODDataCallback;
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

    bool mOneShot;
    bool mWithBuffer;

    VIDevice *mpVIDevice;

    /**
     * EyeseeCamera need to know cameras configuration in system, so use this fuction to get.
     * This function is called in appropriate member function.
     */
    static status_t initCamerasConfiguration();
    static std::map<int, CameraInfo> mCameraInfos;
    static Mutex mCameraInfosLock;
}; /* class EyeseeCamera */

}; /* namespace EyeseeLinux */

#endif /* __IPCLINUX_EYESEECAMERA_H__ */


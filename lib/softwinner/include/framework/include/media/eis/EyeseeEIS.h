#ifndef __IPCLINUX_EYESEEEIS_H__
#define __IPCLINUX_EYESEEEIS_H__

#include <vector>

#include <plat_type.h>
#include <plat_errno.h>
#include <plat_defines.h>
#include <plat_math.h>
#include <Errors.h>

#include <utils/CallbackDispatcher.h>
#include <camera.h>
#include <CameraParameters.h>
#include <CameraListener.h>

#include <mm_comm_eis.h>

#include <EyeseeEISCommon.h>

#include <PictureRegionCallback.h>


namespace EyeseeLinux{

class EISDevice;
class EyeseeEISContext;
class EyeseeEIS
{
public: 
    EyeseeEIS();
    ~EyeseeEIS(); 
	
    static EyeseeEIS *open();
    static void close(EyeseeEIS *peis);
	
    status_t prepareDevice(EIS_ATTR_S *pEisChnAttr);
    status_t releaseDevice(); 
	
    status_t startDevice();
    status_t stopDevice(); 
	
    status_t startChannel(int chnId); 
	
    EIS_CHN openChannel(EIS_ATTR_S *pChnAttr);
    status_t closeChannel(EIS_CHN chnId);

	
    status_t setParameters(int chnId, CameraParameters &param);
    status_t getParameters(int chnId, CameraParameters &param);

	
    status_t setChannelDisplay(int chnId, int hlay);
	
    bool previewEnabled(int chnId);

	
    status_t startRender(int chnId);
    status_t stopRender(int chnId); 
	
    status_t setEISChnAttr(EIS_ATTR_S *pAttr);

	
    void releaseRecordingFrame(int chnId, uint32_t index); 
    status_t startRecording(int chnId, CameraRecordingProxyListener *pCb, int recorderId);
    status_t stopRecording(int chnId, int recorderId); 
	
    CameraRecordingProxy* getRecordingProxy(); 
	
    status_t setCamera(std::vector<EIS_CameraChannelInfo>& cameraChannels);
	
//    status_t getMODParams(int chnId, AW_AI_CVE_DTCA_PARAM *pParamDTCA, AW_AI_CVE_CLBR_PARAM *pParamCLBR);
//    status_t setMODParams(int chnId, AW_AI_CVE_DTCA_PARAM *pParamDTCA, AW_AI_CVE_CLBR_PARAM *pParamCLBR);

////////////////////start->//////////////////////////////////

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
        virtual void onPictureTaken(int chnId, const void *data, int size, EyeseeEIS* pEIS) = 0;
    }; 
    status_t takePicture(int chnId, ShutterCallback *pShutter, PictureCallback *pRaw,
						            PictureCallback *pJpeg, PictureRegionCallback *pPicReg); 
    status_t takePicture(int chnId, ShutterCallback *pShutter, PictureCallback *pRaw,
            PictureCallback *pPostview, PictureCallback *pJpeg, PictureRegionCallback *pPicReg); 
    status_t cancelContinuousPicture(int chnId);


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
		virtual void onError(int chnId, int error, EyeseeEIS *pEIS) = 0;
	}; 
	void setErrorCallback(ErrorCallback *pCb);


	class InfoCallback
	{
	public:
		/**
		 * Called to indicate an info or a warning.
		 *
		 * @param pEis:    the eis the info pertains to.
		 * @param what:    the type of info or warning.
		 * @param extra:   an extra code, specific to the info. Typically implementation dependent.
		 * @return: 	   True if the method handled the info, false if it didn't.
		 */
		InfoCallback(){}
		virtual ~InfoCallback(){}
		virtual bool onInfo(int chnId, CameraMsgInfoType what, int extra, EyeseeEIS *pEIS) = 0;
	}; 
	void setInfoCallback(InfoCallback *pCb);

	class PreviewCallback
	{
	public:
		PreviewCallback(){}
		virtual ~PreviewCallback(){}
		virtual void onPreviewFrame(const void *data, int size, EyeseeEIS* pEIS) = 0;
	}; 
	void setPreviewCallback(PreviewCallback *pCb); 

	static void postEventFromNative(EyeseeEIS* pC, int what, int arg1, int arg2, int arg3, const std::shared_ptr<CMediaMemory>* pDataPtr=NULL);

	
////////////////////end->/////////////////////////////////////////

private:
    class EventHandler : public CallbackDispatcher
    {
    public:
        EventHandler(EyeseeEIS *pEIS) : mpEIS(pEIS) {}
        virtual ~EventHandler(){};
        virtual void handleMessage(const CallbackMessage &msg);
    private:
        EyeseeEIS *mpEIS;
    };

    class EISProxy : public CameraRecordingProxy    // recorder get this, it return frame to eis
    {
    public:
        EISProxy(EyeseeEIS *pEIS) : mpEIS(pEIS) {}
        virtual ~EISProxy() {};

        // CameraRecordingProxy interface
        virtual status_t        startRecording(int chnId, CameraRecordingProxyListener *pListener, int recorderId);
        virtual status_t        stopRecording(int chnId, int recorderId);
        virtual void            releaseRecordingFrame(int chnId, uint32_t frameIndex);
        virtual status_t        setChannelDisplay(int chnId, int hlay);
        virtual status_t        getParameters(int chnId, CameraParameters &param);
        virtual status_t        setParameters(int chnId, CameraParameters &param);
    private:
        EyeseeEIS *mpEIS;
    };


    static unsigned int gEISIdCounter;
    const unsigned int mEISId;					// <-- gEISIdCounter	Identification id for EIS

    EventHandler *mpEventHandler;				// for msg process 		samuel.zhou
    EyeseeEISContext *mpNativeContext;			// native context 		samuel.zhou
    EISDevice *mpEISDevice;						// point to EISDevice	samuel.zhou
	
    ErrorCallback   *mpErrorCallback;			// app set function			samuel.zhou
    InfoCallback    *mpInfoCallback;			// app set function			samuel.zhou
    
    ShutterCallback *mpShutterCallback;			// kinds of listener	samuel.zhou
    PreviewCallback *mpPreviewCallback;

#if 1
    PictureCallback *mpRawImageCallback;
    PictureCallback *mpJpegCallback;
    PictureCallback *mpPostviewCallback;
#endif /* 0 */

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


};



};



#endif

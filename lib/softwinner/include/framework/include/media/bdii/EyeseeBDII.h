/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : EyeseeBDII.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/05
  Last Modified :
  Description   : EyeseeBDII module.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_EYESEEBDII_H__
#define __IPCLINUX_EYESEEBDII_H__

#include <vector>

#include <plat_type.h>
#include <plat_errno.h>
#include <plat_defines.h>
#include <plat_math.h>
#include <Errors.h>

//#include <utils/CallbackDispatcher.h>
//#include <camera.h>
//#include <CameraParameters.h>
//#include <CameraListener.h>
//#include <CveBDII.h>

#include "EyeseeBDIICommon.h"


namespace EyeseeLinux {

class CveBDII;
class EyeseeBDIIContext;
class EyeseeBDII
{
public:
    EyeseeBDII();
    ~EyeseeBDII();

    static EyeseeBDII *open();
    static void close(EyeseeBDII *pBDII);

    status_t prepare();
    status_t start();
    status_t stop();

    status_t setDisplay(int hlay);
    status_t setPreviewRotation(int rotation);
    status_t setCamera(std::vector<BDIICameraChnInfo>& cameraChannels);

    status_t debugYuvDataOut(bool saveOut, std::vector<BDIIDebugFileOutString>&vec);

    status_t setBDIIConfigParams(AW_AI_CVE_BDII_INIT_PARA_S &Param);
    status_t getBDIIConfigParams(AW_AI_CVE_BDII_INIT_PARA_S &Param);

    class BDIIDataCallback
    {
    public:
        BDIIDataCallback(){}
        virtual ~BDIIDataCallback(){}
        /**
         * Called when FaceDetect data is available during frame capture.
         * The format of the data is .
         *
         * @param chnId reserve not use.
         * @param data   a byte array of the picture data
         * @param eyeseebdii the bdii service object
         */
        virtual void onBDIIData(int chnId, const CVE_BDII_RES_OUT_S *data, EyeseeBDII* pBDII) = 0;
    };
    void setBDIIDataCallback(BDIIDataCallback *pCb);

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
         * @param chnId reserve not use.
         * @param error error code
         * @param eyeseebdii the bdii service object

         */
        virtual void onError(int chnId, int error, EyeseeBDII *pBDII) = 0;
    };
    void setErrorCallback(ErrorCallback *pCb);

    static void postEventFromNative(EyeseeBDII* pC, int what, int arg1, int arg2, int arg3, const std::shared_ptr<CMediaMemory>* pDataPtr=NULL);

private:
    class EventHandler : public CallbackDispatcher
    {
    public:
        EventHandler(EyeseeBDII *pBDII) : mpBDII(pBDII) {}
        virtual ~EventHandler(){};
        virtual void handleMessage(const CallbackMessage &msg);
    private:
        EyeseeBDII *mpBDII;
    };

    static unsigned int gBDIIIdCounter;
    unsigned int mBDII_Id;
    CveBDII *mpCveBDII;
    EyeseeBDIIContext *mpNativeContext;
    EventHandler *mpEventHandler;
    BDIIDataCallback *mpBDIIDataCallback;
    ErrorCallback *mpErrorCallback;

}; /* class EyeseeBDII */

}; /* namespace EyeseeLinux */

#endif /* __IPCLINUX_EYESEEBDII_H__ */


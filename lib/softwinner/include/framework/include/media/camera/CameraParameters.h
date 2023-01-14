/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : CameraParameters.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/06/06
  Last Modified :
  Description   : camera wrap MPP components.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_CAMERA_PARAMETERS_H__
#define __IPCLINUX_CAMERA_PARAMETERS_H__

#include <mm_common.h>
#include <mm_comm_video.h>
#include <mm_comm_vi.h>
#include <mpi_isp.h>


#include <string>
#include <vector>
#include <map>

namespace EyeseeLinux {

enum TAKE_PICTURE_MODE_E
{
    TAKE_PICTURE_MODE_NULL = 0,
    TAKE_PICTURE_MODE_NORMAL,   // stream off -> stream on -> take picture -> stream off -> stream on
    TAKE_PICTURE_MODE_FAST,     // same as normal mode but do not need to stream off/on, use preview capture setting.
    TAKE_PICTURE_MODE_CONTINUOUS, //fast mode, take picture more times, need set PictureInterval(if 0, encode frame by frame), need set PictureNumber.
};

class CameraParameters
{
public:
    CameraParameters();
    ~CameraParameters();

    struct SensorParamSet
    {
        int mWidth;
        int mHeight;
        int mFps;   //unit: frame/s.
        SensorParamSet(int width, int height, int fps);
        bool operator== (const SensorParamSet& rhs);
    };

    void set(const char *key, const char *value);
    void set(const char *key, int value);
    void setFloat(const char *key, float value);
    const char *get(const char *key) const;
    int getInt(const char *key) const;
    float getFloat(const char *key) const;

    void setBrightnessValue(ISP_SINGLE_S& value);
    ISP_SINGLE_S& getBrightnessValue();
    void setContrastValue(ISP_CONTRAST_S& value);
    ISP_CONTRAST_S& getContrastValue();
    void setSaturationValue(ISP_SATURATION_S& value);
    ISP_SATURATION_S& getSaturationValue();
    void setHueValue(ISP_HUE_S& value);
    ISP_HUE_S& getHueValue();
    void setSharpnessValue(ISP_SHARPEN_ATTR_S& value);
    ISP_SHARPEN_ATTR_S& getSharpnessValue();

    void setAWB_WBAttrValue(ISP_WB_ATTR_S& value);
    ISP_WB_ATTR_S& getAWB_WBAttrValue();
    void setAWB_CCMAttrValue(ISP_COLORMATRIX_ATTR_S& value);
    ISP_COLORMATRIX_ATTR_S& getAWB_CCMAttrValue();
    void setAWB_SpeedValue(ISP_AWB_SPEED_S& value);
    ISP_AWB_SPEED_S& getAWB_SpeedValue();
    void setAWB_TempRangeValue(ISP_AWB_TEMP_RANGE_S& value);
    ISP_AWB_TEMP_RANGE_S& getAWB_TempRangeValue();
    void setAWB_LightValue(int mode, ISP_AWB_TEMP_INFO_S& value);
    ISP_AWB_TEMP_INFO_S* getAWB_LightValue(int mode);
    void setAWB_LightValues(std::map<int, ISP_AWB_TEMP_INFO_S>& value);
    std::map<int, ISP_AWB_TEMP_INFO_S>& getAWB_LightValues();
    void setAWB_FavorValue(ISP_AWB_FAVOR_S& value);
    ISP_AWB_FAVOR_S& getAWB_FavorValue();

    void setFlickerValue(ISP_FLICKER_S& value);
    ISP_FLICKER_S& getFlickerValue();

    void setNRAttrValue(int value);
    int getNRAttrValue();
    void set3NRAttrValue(int value);
    int get3NRAttrValue();
    void setPltmWDR(int value);
    int getPltmWDR();
    void setModuleOnOff(ISP_MODULE_ONOFF& value);
    ISP_MODULE_ONOFF& getModuleOnOff();


    void ChnIspAe_SetMode(int value);
    void ChnIspAe_SetExposureBias(int value);
    void ChnIspAe_SetExposure(int value);
    void ChnIspAe_SetGain(int value);
    void ChnIspAe_SetMetering(int value);
    void ChnIspAe_SetISOSensitive(int value);
    void ChnIspAwb_SetMode(int value);
    void ChnIspAwb_SetColorTemp(int value);
    void ChnIspAwb_SetRGain(int value);
    void ChnIspAwb_SetGrGain(int value);
    void ChnIspAwb_SetGbGain(int value);
    void ChnIspAwb_SetBGain(int value);
    void ChnIsp_SetFlicker(int value);
    void ChnIsp_SetBrightness(int value);
    void ChnIsp_SetContrast(int value);
    void ChnIsp_SetSaturation(int value);
    void ChnIsp_SetSharpness(int value);
    //void ChnIsp_SetHue(int value);

    int ChnIspAe_GetMode();
    int ChnIspAe_GetExposureBias();
    int ChnIspAe_GetExposure();
    int ChnIspAe_GetGain();
    int ChnIspAe_GetMetering();
    int ChnIspAe_GetISOSensitive();
    int ChnIspAwb_GetMode();
    int ChnIspAwb_GetColorTemp();
    int ChnIspAwb_GetRGain();
    int ChnIspAwb_GetGrGain();
    int ChnIspAwb_GetGbGain();
    int ChnIspAwb_GetBGain();
    int ChnIsp_GetFlicker();
    int ChnIsp_GetBrightness();
    int ChnIsp_GetContrast();
    int ChnIsp_GetSaturation();
    int ChnIsp_GetSharpness();
    //int ChnIsp_GetHue();

    void setZoom(int value);
    int getZoom();
private:
    void setZoomSupported(bool value);
public:
    bool isZoomSupported();
private:
    void setMaxZoom(int value);
public:
    int getMaxZoom();

    /**
     * mode: V4L2_MODE_VIDEO
     */
    void setCaptureMode(int mode);
    int getCaptureMode();
    void setVideoSize(SIZE_S &size);

    inline void getVideoSize(SIZE_S &size) const
    {
        size = mVideoSize;
    }
    void getVideoSizeOut(SIZE_S &size) const;
    void setVideoBufSizeOut(SIZE_S &size);
    void getVideoBufSizeOut(SIZE_S &size) const;

    inline void setPreviewFrameRate(int fps)
    {
        mFrameRate = fps;
    }

    inline int getPreviewFrameRate()
    {
        return mFrameRate;
    }

    /**
     * 0 means display every frame. unit: fps
     */
    inline void setDisplayFrameRate(int fps)
    {
        mDisplayFrameRate = fps;
    }

    inline int getDisplayFrameRate()
    {
        return mDisplayFrameRate;
    }

    void setSupportedSensorParamSets(std::vector<SensorParamSet> &SensorParamSets);
    void getSupportedSensorParamSets(std::vector<SensorParamSet> &SensorParamSets) const;

    inline void setPreviewRotation(int rotation)
    {
        mPreviewRotation = rotation;
    }

    inline int getPreviewRotation()
    {
        return mPreviewRotation;
    }

    inline void setPreviewFormat(PIXEL_FORMAT_E pixel_format)
    {
        mPixelFormat = pixel_format;
    }

    inline PIXEL_FORMAT_E getPreviewFormat()
    {
        return mPixelFormat;
    }
    inline void setColorSpace(enum v4l2_colorspace colorspace)
    {
        mColorSpace = colorspace;
    }

    inline enum v4l2_colorspace getColorSpace()
    {
        return mColorSpace;
    }

    inline void setVideoBufferNumber(int number)
    {
        mVideoBufferNum = number;
    }

    inline int getVideoBufferNumber()
    {
        return mVideoBufferNum;
    }

    inline void setPictureSize(const SIZE_S &size)
    {
        mPictureSize = size;
    }

    inline void getPictureSize(SIZE_S &size) const
    {
        size = mPictureSize;
    }

    inline void setJpegThumbnailSize(const SIZE_S &size)
    {
        mJpegThumbnailSize = size;
    }

    inline void getJpegThumbnailSize(SIZE_S &size) const
    {
        size = mJpegThumbnailSize;
    }

    inline void setJpegThumbnailQuality(int quality)
    {
        mJpegThumbnailQuality = quality;
    }

    inline int getJpegThumbnailQuality()
    {
        return mJpegThumbnailQuality;
    }

    inline void setJpegRotation(int rotation)
    {
        mJpegRotation = rotation;
    }

    inline int getJpegRotation()
    {
        return mJpegRotation;
    }

    inline void setJpegQuality(int quality)
    {
        mJpegQuality = quality;
    }

    inline int getJpegQuality()
    {
        return mJpegQuality;
    }

    inline void setPictureFormat(PIXEL_FORMAT_E pixel_format)
    {
        mPictureFromat = pixel_format;
    }

    inline PIXEL_FORMAT_E getPictureFormat()
    {
        return mPictureFromat;
    }

    inline void setPictureMode(TAKE_PICTURE_MODE_E mode)
    {
        mTakePictureMode = mode;
    }

    TAKE_PICTURE_MODE_E getPictureMode()
    {
        return mTakePictureMode;
    }

    inline void setGpsLatitude(double latitude)
    {
        mGpsLatitude = latitude;
    }

    inline double getGpsLatitude()
    {
        return mGpsLatitude;
    }

    inline void setGpsLongitude(double longitude)
    {
        mGpsLongitude = longitude;
    }

    inline double getGpsLongitude()
    {
        return mGpsLongitude;
    }

    inline void setGpsAltitude(double altitude)
    {
        mGpsAltitude = altitude;
    }

    inline double getGpsAltitude()
    {
        return mGpsAltitude;
    }

    inline void setGpsTimestamp(long timestamp)
    {
        mGpsTimestamp = timestamp;
    }

    inline long getGpsTimestamp()
    {
        return mGpsTimestamp;
    }

    void setGpsProcessingMethod(const char *processing_method);

    inline char* getGpsProcessingMethod()
    {
        return mpGpsProcessingMethod;
    }

    void removeGpsData();

    inline void setContinuousPictureNumber(int number)
    {
        mContinuousPicNum = number;
    }

    inline int getContinuousPictureNumber()
    {
        return mContinuousPicNum;
    }

    inline void setContinuousPictureIntervalMs(int timeMs)
    {
        mContinuousPicIntervalMs = timeMs;
    }

    inline int getContinuousPictureIntervalMs()
    {
        return mContinuousPicIntervalMs;
    }

    void SetMirror(int value);
    int GetMirror();
    void SetFlip(int value);
    int GetFlip();

    void setShutTime(VI_SHUTTIME_CFG_S &value);
    void getShutTime(VI_SHUTTIME_CFG_S &value) const;

private:
    friend class VIChannel;
    friend class VIDevice;
    friend class EyeseeUSBCamera;
    friend class UvcChannel;
    int mCaptureMode;   //V4L2_MODE_VIDEO
    SIZE_S mVideoSize;  //vi capture size.
    SIZE_S mVideoSizeOut;  //mpi_vi output frame size, in common ,== mVideoSize, but in anti-shake mode, < mVideoSize.
    SIZE_S mVideoBufSizeOut;  //vi capture buffer size.
    int mFrameRate;
    int mDisplayFrameRate;  //for preview window to display, 0 means display every frame. unit: fps
    int mPreviewRotation;   //indicate anti-clock wise of picture, 90, 180, 270; it means display system will rotate picture in clock wise to display normal.
    PIXEL_FORMAT_E mPixelFormat;
    enum v4l2_colorspace mColorSpace;
    int mVideoBufferNum;

    SIZE_S mPictureSize;
    SIZE_S mJpegThumbnailSize;
    int mJpegThumbnailQuality;
    int mJpegQuality;
    int mJpegRotation;
    PIXEL_FORMAT_E mPictureFromat;
    TAKE_PICTURE_MODE_E mTakePictureMode;
    double mGpsLatitude;
    double mGpsLongitude;
    double mGpsAltitude;
    long mGpsTimestamp;
    char *mpGpsProcessingMethod;
    int mContinuousPicNum;
    int mContinuousPicIntervalMs;

    std::vector<SensorParamSet> mSupportedSensorParamSets;

    ISP_SINGLE_S mBrightness;
    ISP_CONTRAST_S mContrast;
    ISP_SATURATION_S mSaturation;
    ISP_HUE_S mHue;
    ISP_SHARPEN_ATTR_S mSharpness;

    ISP_WB_ATTR_S mWBAttr;
    ISP_COLORMATRIX_ATTR_S mColorMatrixAttr;
    ISP_AWB_SPEED_S mAWBSpeed;
    ISP_AWB_TEMP_RANGE_S mAWBTempRange;
    ISP_AWB_TEMP_INFO_S mAWBTempInfo;
    std::map<int, ISP_AWB_TEMP_INFO_S> mAWBLight;
    ISP_AWB_FAVOR_S mAWBFavor;
    ISP_FLICKER_S mFlicker;

    int                mNRAttr;
    int                m3NRAttr;
    int                mPltmWDR;
    ISP_MODULE_ONOFF   mModuleOnOff;


    int mChnIspAe_Mode;
    int mChnIspAe_ExposureBias;
    int mChnIspAe_Exposure;
    int mChnIspAe_Gain;
    int mChnIspAe_ISOSensitive;
    int mChnIspAe_Metering;
    int mChnIspAwb_Mode;
    int mChnIspAwb_ColorTemp;
    int mChnIspAwb_RGain;
    int mChnIspAwb_GrGain;
    int mChnIspAwb_GbGain;
    int mChnIspAwb_BGain;
    int mChnIsp_Flicker;
    int mChnIsp_Brightness;
    int mChnIsp_Contrast;
    int mChnIsp_Saturation;
    int mChnIsp_Sharpness;
    //int mChnIsp_Hue;        

    int mZoom;  //0: normal, 1+mZoom/10 == zoomValue.
    int mMaxZoom;
    bool mbZoomSupported;
    int mMirror;    //0:no mirror, 1:mirror
    int mFlip;  //0:no flip; 1:flip
    VI_SHUTTIME_CFG_S mShutTime;

    std::map<std::string, std::string> mMap;
}; /* CameraParameters */

}; /* namespace EyeseeLinux */

#endif /* __IPCLINUX_CAMERA_PARAMETERS_H__ */

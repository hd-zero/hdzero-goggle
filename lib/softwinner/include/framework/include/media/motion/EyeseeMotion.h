/*
********************************************************************************
*                           eyesee framework module
*
*          (c) Copyright 2010-2018, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : EyeseeMotion.h
* Version: V1.0
* By     : zing huang
* Date   : 2018-06-28
* Description:
********************************************************************************
*/

#ifndef _EYESEEMOTION_H_
#define _EYESEEMOTION_H_

#include <string>
#include <memory>

#include <Errors.h>
#include <CMediaMemory.h>
#include <mm_common.h>
#include <mm_comm_vdec.h>
#include <mm_comm_demux.h>
#include <mm_comm_clock.h>
#include <Mutex.h>

#include <aw_ai_mt_def.h>
#include <aw_ai_core_def.h>

namespace EyeseeLinux
{
class EyeseeMotion
{
public:
    EyeseeMotion();
    ~EyeseeMotion();
    EyeseeMotion(const EyeseeMotion &other) = delete;
    EyeseeMotion& operator=(const EyeseeMotion &other) = delete;

    status_t setVideoFileSource(const char *url);
    status_t setVideoFileSource(int fd, int64_t offset, int64_t length);

    struct MotionPictureParam
    {
        int mPicWidth;
        int mPicHeight;
        int mStartTimeUs;
        int mSourceFrameNums;
        int mSourceFrameStep;
        int mObjectNums;
        std::string mTempBMPFileBasePath;
    };
    std::shared_ptr<CMediaMemory> getMotionJpeg(const MotionPictureParam &jpegParam);
    std::shared_ptr<CMediaMemory> getMotionBMP(const MotionPictureParam &bmpParam);

    status_t getMotionPictureConfig(MotionPictureParam *pictureParam) const
    {
        if(pictureParam)
        {
            pictureParam->mPicWidth = mPictureHeight;
            pictureParam->mPicHeight = mPictureHeight;
            pictureParam->mStartTimeUs = mStartTimeMs * 1000;
            pictureParam->mSourceFrameNums = mSourceFrameNums;
            pictureParam->mSourceFrameStep = mSourceFrameStep;
            pictureParam->mObjectNums = mObjectNums;
        }
        return NO_ERROR;
    }

    status_t reset();

    status_t getMediaInfo(DEMUX_MEDIA_INFO_S *pMediaInfo) const
    {
        if(pMediaInfo)
        {
            *pMediaInfo = mMediaInfo;
        }
        return NO_ERROR;
    }

	int GetmMotionTrailRet();

private:
    //mpp components
    DEMUX_CHN mDmxChn;
    DEMUX_CHN_ATTR_S mDmxChnAttr;
    DEMUX_MEDIA_INFO_S mMediaInfo;
    VDEC_CHN mVdecChn;
	VDEC_CHN_ATTR_S mVdecChnAttr; 
    CLOCK_CHN mClockChn;
    CLOCK_CHN_ATTR_S mClockChnAttr;

    //motion lib
    MT_Params mLpMtParam;
    AW_AI_HANDLE mMotionHandle;
    IMAGE_DATA_I mImageYUV;
    IMAGE_DATA_I mImageBGR;

    int mPictureWidth;
    int mPictureHeight;
    int mStartTimeMs;
    int mSourceFrameNums;
    int mSourceFrameStep;
    int mObjectNums;
	int mMotionTrailRet;

    std::string mTempBMPFileBasePath;


enum media_motion_states
{
    MEDIA_MOTION_STATE_ERROR        = 0,
    MEDIA_MOTION_IDLE               = 1 << 0,
    MEDIA_MOTION_INITIALIZED        = 1 << 1,
    MEDIA_MOTION_PREPARING          = 1 << 2,
    MEDIA_MOTION_PREPARED           = 1 << 3,
    MEDIA_MOTION_STARTED            = 1 << 4,
    MEDIA_MOTION_STOPPED            = 1 << 5,
    MEDIA_MOTION_FINISH             = 1 << 6
};
    media_motion_states mCurrentState;
    Mutex mLock;

    void ClearConfig();
    status_t prepare();
    status_t start();
    status_t getMotionTrail();
    status_t stop();
    status_t seekTo(); 

    std::shared_ptr<CMediaMemory> PictureAsJpeg(IMAGE_DATA_I &ImageYUV);
    std::shared_ptr<CMediaMemory> PictureAsBMP(IMAGE_DATA_I &ImageBGR);
};
};




#endif


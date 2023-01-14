/*
********************************************************************************
*                           eyesee framework module
*
*          (c) Copyright 2010-2017, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : thumbRetriever.h
* Version: V1.0
* By     : eric_wang
* Date   : 2017-10-27
* Description:
********************************************************************************
*/
#ifndef _EYESEETHUMBRETRIEVER_H_
#define _EYESEETHUMBRETRIEVER_H_

#include <Errors.h>
#include <vector>
#include <memory>
#include <CMediaMemory.h>
#include <mm_common.h>
#include <mm_comm_vdec.h>
#include <mm_comm_venc.h>
#include <mm_comm_demux.h>
#include <mm_comm_clock.h>

#include <Mutex.h>


namespace EyeseeLinux
{

class EyeseeThumbRetriever
{
public:
    EyeseeThumbRetriever();
    virtual ~EyeseeThumbRetriever();
    status_t setDataSource(const char *url);
    status_t setDataSource(int fd, int64_t offset, int64_t length);
    status_t reset();
    status_t getMediaInfo(DEMUX_MEDIA_INFO_S *pMediaInfo);
    std::shared_ptr<CMediaMemory> getJpegAtTime(int64_t timeUs, int reqWidth, int reqHeight);
	
private:
	//mpp components
	DEMUX_CHN mDmxChn;
	DEMUX_CHN_ATTR_S mDmxChnAttr;
	VDEC_CHN mVdecChn;
	VDEC_CHN_ATTR_S mVdecChnAttr;
	VENC_CHN mVecChn;
	VENC_CHN_ATTR_S mVecChnAttr;    
    CLOCK_CHN mClockChn;
    CLOCK_CHN_ATTR_S mClockChnAttr;

    //ThumbPicture
    //std::shared_ptr<CMediaMemory> mJpegBuf;
    bool bJpegSource;
    int mJpegWidth;
    int mJpegHeight;
    int mSeekStartPosition; //ms

enum media_thumb_states {
    MEDIA_THUMB_STATE_ERROR        = 0,
    MEDIA_THUMB_IDLE               = 1 << 0,
    MEDIA_THUMB_INITIALIZED        = 1 << 1,
    MEDIA_THUMB_PREPARING          = 1 << 2,
    MEDIA_THUMB_PREPARED           = 1 << 3,
    MEDIA_THUMB_STARTED            = 1 << 4,
    MEDIA_THUMB_STOPPED            = 1 << 5
};
    media_thumb_states mCurrentState;
    Mutex mLock;

    status_t prepare();
    status_t start();
    status_t stop();
    status_t seekTo();
    
};

};

#endif  /* _EYESEETHUMBRETRIEVER_H_ */


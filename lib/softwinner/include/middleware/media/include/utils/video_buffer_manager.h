/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : videoInputBufferManager.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/06/28
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef __VIDEOINPUT_BUFFER_MANAGER_H__
#define __VIDEOINPUT_BUFFER_MANAGER_H__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "mm_comm_video.h"

typedef struct VideoFrameListInfo {
    VIDEO_FRAME_INFO_S mFrame;
    struct list_head mList;
} VideoFrameListInfo;

typedef struct VideoBufferManager {
    struct list_head mFreeFrmList;
    struct list_head mValidFrmList;
    struct list_head mUsingFrmList;
    pthread_mutex_t mFrmListLock;
    pthread_cond_t mCondUsingFrmEmpty;
    int mFrameNodeNum;
    int mbWaitUsingFrmEmptyFlag;

    VIDEO_FRAME_INFO_S* (*GetOldestValidFrame)(struct VideoBufferManager *pMgr);
    VIDEO_FRAME_INFO_S* (*GetOldestUsingFrame)(struct VideoBufferManager *pMgr);
    VIDEO_FRAME_INFO_S* (*GetSpecUsingFrameWithAddr)(struct VideoBufferManager *pMgr, void *pVirAddr);
    VIDEO_FRAME_INFO_S* (*GetAllValidUsingFrame)(struct VideoBufferManager *pMgr);
    VIDEO_FRAME_INFO_S* (*getValidFrame)(struct VideoBufferManager *pMgr);
    int (*releaseFrame)(struct VideoBufferManager *pMgr, VIDEO_FRAME_INFO_S *pFrame);
    int (*pushFrame)(struct VideoBufferManager *pMgr, VIDEO_FRAME_INFO_S *pFrame);
    int (*usingFrmEmpty)(struct VideoBufferManager *pMgr);
    int (*waitUsingFrmEmpty)(struct VideoBufferManager *pMgr);
} VideoBufferManager;

/* Just return the oldest frame who was added into using frame list.
* there has no move operations. */
#define VideoBufMgrGetOldestUsingFrame(pMgr) pMgr->GetOldestUsingFrame(pMgr)

/* Just return the oldest frame who was added into valid frame list.
* there has no move operations. */
#define VideoBufMgrGetOldestValidFrame(pMgr) pMgr->GetOldestValidFrame(pMgr)

/* Use virtual address to get one using frame in using list, just get it's address,
* there has no move operations. */
#define VideoBufMgrGetSpecUsingFrameWithAddr(pMgr, pVirAddr) pMgr->GetSpecUsingFrameWithAddr(pMgr, pVirAddr)

/* Get all valid&using frame list's frames, and move to idle list.
* Return NULL means all has done. */
#define VideoBufMgrGetAllValidUsingFrame(pMgr) pMgr->GetAllValidUsingFrame(pMgr)

#define VideoBufMgrGetValidFrame(pMgr) pMgr->getValidFrame(pMgr)

/* You should know that if you release this frame, it pFrame->[VIDEO_FRAME_INFO_S] may be changed,
* so you must confirm that after invoke this function, you nnnnnnnnnnnnnnnnnever will use
* pFrame->[VIDEO_FRAME_INFO_S], or you may will get one wrong [VIDEO_FRAME_INFO_S], but not what you want. */
#define VideoBufMgrReleaseFrame(pMgr, pFrame) pMgr->releaseFrame(pMgr, pFrame)
#define VideoBufMgrPushFrame(pMgr, pFrame) pMgr->pushFrame(pMgr, pFrame)
#define VideoBufMgrUsingEmpty(pMgr) pMgr->usingFrmEmpty(pMgr)
#define VideoBufMgrWaitUsingEmpty(pMgr) pMgr->waitUsingFrmEmpty(pMgr)

/* This buffer manager module is used to manage buffers that
*the real buffer pool is not belong to ourselves, we just use the buffer header info to manage it.
*/
VideoBufferManager *VideoBufMgrCreate(int frmNum, int frmSize);
void VideoBufMgrDestroy(VideoBufferManager *pMgr);

#endif /* __PCM_BUFFER_MANAGER_H__ */

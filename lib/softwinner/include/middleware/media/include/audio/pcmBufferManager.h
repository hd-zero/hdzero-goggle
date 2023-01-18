/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : pcmBufferManager.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/06/28
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef __PCM_BUFFER_MANAGER_H__
#define __PCM_BUFFER_MANAGER_H__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <mm_comm_aio.h>


typedef struct PcmBufferManager
{
    struct list_head mFreeFrmList;
    struct list_head mValidFrmList;
    struct list_head mUsingFrmList;
    struct list_head mFillingFrmList;
    pthread_mutex_t mFreeFrmListLock;
    pthread_mutex_t mValidFrmListLock;
    pthread_mutex_t mUsingFrmListLock;
    pthread_mutex_t mFillingFrmListLock;
    int mFrameNodeNum;
    int mFrameSize; //bytes per audio pcm frame

    AUDIO_FRAME_S* (*getValidFrame)(struct PcmBufferManager *pMgr);
    void (*releaseFrame)(struct PcmBufferManager *pMgr, AUDIO_FRAME_S *pFrame);
    AUDIO_FRAME_S* (*getFreeFrame)(struct PcmBufferManager *pMgr);
    void (*pushFrame)(struct PcmBufferManager *pMgr, AUDIO_FRAME_S *pFrame);
    int (*usingFrmEmpty)(struct PcmBufferManager *pMgr);
    int (*fillingFrmEmpty)(struct PcmBufferManager *pMgr);
    int (*validFrmEmpty)(struct PcmBufferManager *pMgr);
    int (*fillingFrmCnt)(struct PcmBufferManager *pMgr);
    int (*validFrmCnt)(struct PcmBufferManager *pMgr);
    int (*usingFrmCnt)(struct PcmBufferManager *pMgr);
} PcmBufferManager;

PcmBufferManager *pcmBufMgrCreate(int frmNum, int frmSize);
void pcmBufMgrDestroy(PcmBufferManager *pMgr);

#endif /* __PCM_BUFFER_MANAGER_H__ */

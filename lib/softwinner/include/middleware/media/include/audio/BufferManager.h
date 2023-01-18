/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : BufferManager.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/06/28
  Last Modified :
  Description   : mpi functions implement
  Function List :
  History       :
******************************************************************************/
#ifndef __BUFFER_MANAGER_H__
#define __BUFFER_MANAGER_H__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <mm_comm_aio.h>

typedef struct BufferManager {
    char *mpStart;
    char *mpRead;
    char *mpWrite;
    size_t mTotalSize;
    size_t mDataSize;
    size_t mFreeSize;
    pthread_mutex_t mLock;

    ssize_t (*writeData)(struct BufferManager *pMgr, char *pInBuf, size_t inSize);
    ssize_t (*readData)(struct BufferManager *pMgr, char *pOutBuf, size_t reqSize);
    size_t (*getFreeSize)(struct BufferManager *pMgr);
} BufferManager;

BufferManager *bufferMgr_Create(size_t size);
void bufferMgr_Destroy(BufferManager *pMgr);

#endif /* __BUFFER_MANAGER_H__ */

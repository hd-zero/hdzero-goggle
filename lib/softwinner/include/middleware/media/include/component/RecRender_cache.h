#ifndef __RECRENDER_CACHE_H__
#define __RECRENDER_CACHE_H__

//ref platform headers
#include "plat_type.h"

//media internal common headers.
#include "mm_component.h"

typedef struct RsPacketCacheManager {
    struct list_head    mDataBufList;   //DynamicBuffer
    char*             mpWritePos;
    char*             mpReadPos;
    int             mUsedBufSize;
    int             mBufSize;
    int             mPacketCount;
    int64_t             mCacheTime; //unit:ms
    int64_t             mFirstPts;  //unit:us
    volatile BOOL   mIsFull;
    BOOL            mWaitReleaseUsingPacketFlag;
    int             mWaitReleaseUsingPacketId;
    pthread_cond_t      mCondReleaseUsingPacket;
    pthread_mutex_t     mPacketListLock;
    //struct list_head    mRSPacketBufList;   //DynamicBuffer, sizeof(RecSinkPacket)*PACKET_NUM_IN_RSPACKETBUF
    struct list_head    mIdlePacketList; //RecSinkPacket
    struct list_head    mReadyPacketList;
    struct list_head    mUsingPacketList;
    int             mPacketIdCounter;

    ERRORTYPE (*PushPacket)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN RecSinkPacket *pRSPacket);
    ERRORTYPE (*GetPacket)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT RecSinkPacket **ppRSPacket);
    ERRORTYPE (*RefPacket)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT int nId);
    ERRORTYPE (*ReleasePacket)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT int nId);
    BOOL (*IsReady)(
        PARAM_IN COMP_HANDLETYPE hComponent);
    int (*GetReadyPacketCount)(
        PARAM_IN COMP_HANDLETYPE hComponent);
    ERRORTYPE (*ControlCacheLevel)(
        PARAM_IN COMP_HANDLETYPE hComponent);
    ERRORTYPE (*QueryCacheState)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT CacheState *pState);
    ERRORTYPE (*LockPacketList)(
        PARAM_IN COMP_HANDLETYPE hComponent);
    ERRORTYPE (*UnlockPacketList)(
        PARAM_IN COMP_HANDLETYPE hComponent);
    ERRORTYPE (*GetUsingPacketList)(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT struct list_head **ppUsingPacketList);
}RsPacketCacheManager;

ERRORTYPE RsPacketCacheManagerInit(RsPacketCacheManager *pThiz, int64_t nCacheTime);//unit:ms
ERRORTYPE RsPacketCacheManagerDestroy(RsPacketCacheManager *pThiz);
RsPacketCacheManager *RsPacketCacheManagerConstruct(int64_t nCacheTime);//unit:ms
void RsPacketCacheManagerDestruct(RsPacketCacheManager *manager);

#endif


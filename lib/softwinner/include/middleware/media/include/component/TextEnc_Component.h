
#ifndef __TEXTENC_COMPONENT_H__
#define __TEXTENC_COMPONENT_H__

//ref platform headers
#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

//media api headers to app
#include "mm_common.h"
#include "mm_comm_tenc.h"
#include "mpi_tenc.h"
#include <TextEncApi.h>
//media internal common headers.
#include "media_common.h"
#include "mm_component.h"
#include "ComponentCommon.h"
#include "tmessage.h"
#include "tsemaphore.h"


#define MAX_TENCODER_PORTS (2)
#define TFIFO_LEVEL         TEXTENC_PACKET_CNT
#define TENC_FRAME_COUNT    (TFIFO_LEVEL+1)



typedef struct TEXTENCDATATYPE {
    COMP_STATETYPE state;
    pthread_mutex_t mStateLock;
    COMP_CALLBACKTYPE *pCallbacks;
    void* pAppData;
    COMP_HANDLETYPE hSelf;

    COMP_PORT_PARAM_TYPE sPortParam;
    COMP_PARAM_PORTDEFINITIONTYPE sInPortDef;
    COMP_PARAM_PORTDEFINITIONTYPE sOutPortDef;
    COMP_INTERNAL_TUNNELINFOTYPE sInPortTunnelInfo;
    COMP_INTERNAL_TUNNELINFOTYPE sOutPortTunnelInfo;

    COMP_PARAM_BUFFERSUPPLIERTYPE sPortBufSupplier[MAX_TENCODER_PORTS];
    BOOL mInputPortTunnelFlag;
    BOOL mOutputPortTunnelFlag;   //TRUE: tunnel mode; FALSE: non-tunnel mode.

    MPP_CHN_S mMppChnInfo;
    pthread_t thread_id;
    CompInternalMsgType eTCmd;
    message_queue_t  cmd_queue;
    volatile int mNoInputTextFlag; //1: aenclib:no input pcm data to be encoded.
    volatile int mNoOutFrameFlag; //1: aenclib:no out frame to store encoded audio data.

    __text_enc_info_t text_info;
    TEXTENCCONTENT_t *htext_enc; 


    int64_t mSendTextFailCnt;    // for update pts 

    
    int mOutputFrameNotifyPipeFds[2];

    struct list_head    mIdleOutFrameList;  //AEncCompOutputBuffer
    struct list_head    mReadyOutFrameList; //AEncCompOutputBuffer, for non-tunnel mode. when mOutputPortTunnelFlag == FALSE, use it to store encoded frames.
    struct list_head    mUsedOutFrameList; //AEncCompOutputBuffer, for non-tunnel mode. when mOutputPortTunnelFlag == FALSE, use it to store user occupied frames.
    int                 mFrameNodeNum;
    BOOL                mWaitOutFrameFullFlag;
    BOOL                mWaitOutFrameFlag;  //for non-tunnel mode, wait outFrame coming!
    pthread_mutex_t     mInputTextMutex;
    pthread_mutex_t     mOutFrameListMutex;
    pthread_cond_t      mOutFrameFullCondition;
    pthread_cond_t      mOutFrameCondition; //for non-tunnel mode, wait outFrame coming!
}TEXTENCDATATYPE;


ERRORTYPE TextEncComponentDeInit(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE TextEncComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);



#endif

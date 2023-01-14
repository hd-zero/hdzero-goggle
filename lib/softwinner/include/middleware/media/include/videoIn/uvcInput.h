#ifndef _UVC_INPUT_H_
#define _UVC_INPUT_H_

#pragma once

//ref platform headers
#include <plat_defines.h>
#include <plat_errno.h>
#include <plat_math.h>
#include <plat_type.h>

//media api headers to app
#include <mm_common.h>
#include <mm_component.h>
#include <tmessage.h>
#include <tsemaphore.h>
#include <mm_comm_uvc.h>
#include <mm_comm_video.h>
#include <linux/videodev2.h>

#define UVC_MAX_DEV_NAME (20)
#define UVC_MAX_DEV_NUM (4)
#define UVC_MAX_CHN_NUM (4)
#define BUFFER_COUNT (32)


typedef struct UVC_CHN_MAP_S
{
    UVC_CHN mUvcChn;
    MM_COMPONENTTYPE *mUvcComp;
    cdx_sem_t mSemCompCmd;
    MPPCallbackInfo  *mCallbackInfo;
    struct list_head mList;
}UVC_CHN_MAP_S;

typedef struct UvcChnManager
{
    char mUvcDevName[UVC_MAX_DEV_NAME];
    int mFd;
    UVC_ATTR_S mUvcAttr;
    pthread_mutex_t mLock;
    
    pthread_t mThread_uvc_capture;
    volatile int mUvcDevEnable;
    int mRefs[BUFFER_COUNT];
    VIDEO_FRAME_INFO_S mUvcCaptureFrame[BUFFER_COUNT];
    unsigned char *mpVideoBuffer[BUFFER_COUNT];
    
    unsigned int mBufferLength;
    struct v4l2_buffer mUvcV4l2BufferInfo[BUFFER_COUNT];
    pthread_mutex_t mRefsLock;    

    struct list_head mChnList;  //UVC_CHN_MAP_S
    
}UvcChnManager;

typedef struct UvcDevManager
{
    pthread_mutex_t mUvcDevManagerLock;
    UvcChnManager *mUvcChnManager[UVC_MAX_DEV_NUM];        
}UvcDevManager;

ERRORTYPE uvcInput_SearchExitDevVirChn(UVC_DEV UvcDev, UVC_CHN UvcChn, UVC_CHN_MAP_S **ppChn);

MM_COMPONENTTYPE *uvcInput_GetChnComp(UVC_DEV UvcDev, UVC_CHN UvcChn);

ERRORTYPE uvcInput_Construct(UVC_DEV UvcDev, int fd);

ERRORTYPE uvcInput_Destruct(UVC_DEV UvcDev);

ERRORTYPE UVC_Construct(void);

ERRORTYPE UVC_Destruct(void);

UVC_CHN_MAP_S *uvcInput_CHN_MAP_S_Construct();

void uvcInput_CHN_MAP_S_Destruct(UVC_CHN_MAP_S *pChnnel);

ERRORTYPE uvcInput_addChannel(UVC_DEV UvcDev, UVC_CHN_MAP_S *pChn);

ERRORTYPE uvcInput_removeChannel(UVC_DEV UvcDev, UVC_CHN_MAP_S *pChn);

ERRORTYPE uvcInput_Open_Video(UVC_DEV UvcDev);

ERRORTYPE uvcInput_Close_Video(UVC_DEV UvcDev);

ERRORTYPE uvcInput_SetDevEnable(UVC_DEV UvcDev);

ERRORTYPE uvcInput_SetDevDisable(UVC_DEV UvcDev);

ERRORTYPE uvcInput_GetDevAttr(UVC_DEV UvcDev, UVC_ATTR_S *pAttr);

ERRORTYPE uvcInput_SetDevAttr(UVC_DEV UvcDev, UVC_ATTR_S *pAttr);

ERRORTYPE uvcInput_ReleaseData2(UvcChnManager *pUvcChnManager, int buffer_index);

ERRORTYPE uvcInput_RefsReduceAndRleaseData2(UvcChnManager *pUvcChnManager, VIDEO_FRAME_INFO_S *pstFrameInfo);

ERRORTYPE uvcInput_SetControl(UVC_DEV UvcDev, int cmd_id, int set_value, const char *cmd_name);

ERRORTYPE uvcInput_GetControl(UVC_DEV UvcDev, int cmd_id, int *get_value, const char *cmd_name);

#endif

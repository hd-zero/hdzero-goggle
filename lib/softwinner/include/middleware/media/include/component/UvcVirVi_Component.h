/******************************************************************************
  Copyright (C), 2001-2017, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : UvcVirVi_Component.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/12/10
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/
#ifndef _UVCVIRVI_COMPONENT_H_
#define _UVCVIRVI_COMPONENT_H_

#pragma once

#include <uvcInput.h>

typedef enum UVC_CHANNEL_PORT_DEFINE_E {
    UVC_CHN_PORT_INDEX_NCOM_IN = 0, // no compress  uvc data
	//UVC_CHN_PORT_INDEX_COMP_IN, /* compress : Decode H264,mpeg data*/
	UVC_CHN_PORT_INDEX_DATA_OUT,
	UVC_CHN_MAX_PORTS,
} UVC_CHANNEL_PORT_DEFINE_E;

typedef struct UvcFrameInfo {
    VIDEO_FRAME_INFO_S VFrame;
    struct list_head mList;
} UvcFrameInfo;

typedef struct UVCDATATYPE
{
    UvcChnManager *pUvcChnManager; //
    
    COMP_STATETYPE state;
    pthread_mutex_t mStateLock;
    COMP_CALLBACKTYPE* pCallbacks;
    void* pAppData;
    COMP_HANDLETYPE hSelf;

    COMP_PORT_PARAM_TYPE sPortParam;
    COMP_PARAM_PORTDEFINITIONTYPE sPortDef[UVC_CHN_MAX_PORTS];
    COMP_INTERNAL_TUNNELINFOTYPE sPortTunnelInfo[UVC_CHN_MAX_PORTS];
    COMP_PARAM_BUFFERSUPPLIERTYPE sPortBufSupplier[UVC_CHN_MAX_PORTS];
    COMP_PARAM_PORTEXTRADEFINITIONTYPE  sPortExtraDef[UVC_CHN_MAX_PORTS];
    BOOL mInputPortTunnelFlag;
    BOOL mOutputPortTunnelFlag;  //TRUE: tunnel mode; FALSE: non-tunnel mode.
    
    MPP_CHN_S mMppChnInfo;
    pthread_t thread_id;
    message_queue_t cmd_queue;

    //input 
    volatile int mWaitingCapDataFlag;
    cdx_sem_t mSemWaitInputFrame;

    //output
    pthread_mutex_t mOutFrameLock;
    pthread_cond_t mWaiteUsedFrmeEmpty;    
    struct list_head    mIdleOutFrameList;
    struct list_head    mReadyOutFrameList;
    struct list_head    mUsedOutFrameList;
    
}UVCDATATYPE;

ERRORTYPE UvcComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);



#endif

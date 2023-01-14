/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : VideoRender_Component.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/29
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_VIDEORENDER_COMPONENT_H__
#define __IPCLINUX_VIDEORENDER_COMPONENT_H__

//ref platform headers
#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

//media api headers to app
#include "mm_common.h"
#include "mm_comm_video.h"
#include <mm_comm_vo.h>

//media internal common headers.
#include "media_common.h"
#include "mm_component.h"
#include "ComponentCommon.h"
#include "tmessage.h"
#include "tsemaphore.h"

typedef enum VIDEORENDER_PORT_INDEX_DEFINITION{
    VDR_PORT_INDEX_CLOCK = 0,
    VDR_PORT_INDEX_VIDEO = 1,
}VIDEORENDER_PORT_INDEX_DEFINITION;

ERRORTYPE VideoRenderGetMPPChannelInfo(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT MPP_CHN_S *pChn);
ERRORTYPE VideoRenderSetMPPChannelInfo(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN MPP_CHN_S *pChn);
ERRORTYPE VideoRenderSetChnAttr(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN VO_CHN_ATTR_S *pChnAttr);
ERRORTYPE VideoRenderGetChnAttr(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT VO_CHN_ATTR_S* pChnAttr);
ERRORTYPE VideoRenderGetPortDefinition(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_INOUT COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE VideoRenderGetCompBufferSupplier(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_INOUT COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE VideoRenderGetPortParam(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT COMP_PORT_PARAM_TYPE *pPortParam);
ERRORTYPE VideoRenderGetTimeCurrentMediaTime(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT COMP_TIME_CONFIG_TIMESTAMPTYPE *pComponentConfigStructure);
ERRORTYPE VideoRenderSetPortDefinition(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE VideoRenderSetCompBufferSupplier(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE VideoRenderSetTimeActiveRefClock(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN COMP_TIME_CONFIG_ACTIVEREFCLOCKTYPE *pRefClock);
ERRORTYPE VideoRenderSeek(PARAM_IN COMP_HANDLETYPE hComponent);    
ERRORTYPE VideoRenderSwitchAudio(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoRenderSetStreamEof(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoRenderClearStreamEof(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoRenderShow(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN BOOL bShowFlag);

/** 
 * init CDX_VideoRenderHAL
 *
 * @return SUCCESS.
 * @param hComponent videoRender component.
 * @param callback_info use it to notify VideoRenderComp something, not use it now.
 */
ERRORTYPE VideoRenderInitVideoRenderHAL(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN void *callback_info);
ERRORTYPE VideoRenderSetAVSync(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN char avSyncFlag);
ERRORTYPE VideoRenderSetVRenderMode(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN VideoRenderMode mode);
ERRORTYPE VideoRenderNotifyStartToRun(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoRenderResolutionChange(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoRenderSendCommand(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_COMMANDTYPE Cmd, 
        PARAM_IN unsigned int nParam1,
        PARAM_IN void* pCmdData);
ERRORTYPE VideoRenderGetState(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT COMP_STATETYPE* pState);
ERRORTYPE VideoRenderSetCallbacks(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_CALLBACKTYPE* pCallbacks, 
        PARAM_IN void* pAppData);
ERRORTYPE VideoRenderGetConfig(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_INDEXTYPE nIndex,
        PARAM_INOUT void* pComponentConfigStructure);
ERRORTYPE VideoRenderSetConfig(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_INDEXTYPE nIndex,
        PARAM_IN void* pComponentConfigStructure);
ERRORTYPE VideoRenderComponentTunnelRequest(
        PARAM_IN  COMP_HANDLETYPE hComponent,
        PARAM_IN  unsigned int nPort,
        PARAM_IN  COMP_HANDLETYPE hTunneledComp,
        PARAM_IN  unsigned int nTunneledPort,
        PARAM_INOUT  COMP_TUNNELSETUPTYPE* pTunnelSetup);
ERRORTYPE VideoRenderEmptyThisBuffer(
        PARAM_IN  COMP_HANDLETYPE hComponent,
        PARAM_IN  COMP_BUFFERHEADERTYPE* pBuffer);
ERRORTYPE VideoRenderComponentDeInit(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoRenderComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);

#endif  /* __IPCLINUX_VIDEORENDER_COMPONENT_H__ */


/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : VideoVI_Component.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/05/09
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_VIDEOVI_COMPONENT_H__
#define __IPCLINUX_VIDEOVI_COMPONENT_H__

//ref platform headers
#include "plat_defines.h"
#include "plat_errno.h"
#include "plat_math.h"
#include "plat_type.h"

//media api headers to app
#include "mm_comm_vi.h"
#include "mm_comm_video.h"
#include "mm_common.h"

//media internal common headers.
#include "ComponentCommon.h"
#include "media_common.h"
#include "mm_component.h"
#include "tmessage.h"
#include "tsemaphore.h"
#include "vencoder.h"

#include <VideoVirViCompPortIndex.h>

// ===========================================================================
#define MAX_VI_PORTS (2)
#define VI_FIFO_LEVEL 20

#define VI_MAXPATHSIZE (1024)

//private interface
ERRORTYPE VideoViGetPortDefinition(PARAM_IN COMP_HANDLETYPE hComponent,
                                   PARAM_INOUT COMP_PARAM_PORTDEFINITIONTYPE* pPortDef);
ERRORTYPE VideoViSetPortDefinition(PARAM_IN COMP_HANDLETYPE hComponent,
                                   PARAM_IN COMP_PARAM_PORTDEFINITIONTYPE* pPortDef);
ERRORTYPE VideoViGetCompBufferSupplier(PARAM_IN COMP_HANDLETYPE hComponent,
                                       PARAM_INOUT COMP_PARAM_BUFFERSUPPLIERTYPE* pPortBufSupplier);
ERRORTYPE VideoViSetCompBufferSupplier(PARAM_IN COMP_HANDLETYPE hComponent,
                                       PARAM_IN COMP_PARAM_BUFFERSUPPLIERTYPE* pPortBufSupplier);
ERRORTYPE VideoViGetMPPChannelInfo(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT MPP_CHN_S* pChn);
ERRORTYPE VideoViSetMPPChannelInfo(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN MPP_CHN_S* pChn);
//public interface
ERRORTYPE VideoViSendCommand(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_COMMANDTYPE Cmd,
                             PARAM_IN unsigned int nParam1, PARAM_IN void* pCmdData);
ERRORTYPE VideoViGetState(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT COMP_STATETYPE* pState);
ERRORTYPE VideoViSetCallbacks(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_CALLBACKTYPE* pCallbacks,
                              PARAM_IN void* pAppData);
ERRORTYPE VideoViGetConfig(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_INDEXTYPE nIndex,
                           PARAM_IN void* pComponentConfigStructure);
ERRORTYPE VideoViSetConfig(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_INDEXTYPE nIndex,
                           PARAM_IN void* pComponentConfigStructure);
ERRORTYPE VideoViComponentTunnelRequest(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN unsigned int nPort,
                                        PARAM_IN COMP_HANDLETYPE hTunneledComp, PARAM_IN unsigned int nTunneledPort,
                                        PARAM_INOUT COMP_TUNNELSETUPTYPE* pTunnelSetup);
ERRORTYPE VideoViEmptyThisBuffer(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_BUFFERHEADERTYPE* pBuffer);
ERRORTYPE VideoViFillThisBuffer(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_BUFFERHEADERTYPE* pBuffer);

ERRORTYPE VideoViComponentDeInit(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoViComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);

#endif /* __IPCLINUX_VIDEOVI_COMPONENT_H__ */

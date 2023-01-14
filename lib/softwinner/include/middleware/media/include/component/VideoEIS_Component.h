/******************************************************************************
  Copyright (C), 2018-2020, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : VideoEIS_Component.h
  Version       : Initial Draft
  Author        : Allwinner PDC-PD5 Team
  Created       : 2018/04/20
  Last Modified :
  Description   : EIS mpp component implement
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_VIDEOEIS_COMPONENT_H__
#define __IPCLINUX_VIDEOEIS_COMPONENT_H__

/* standard header files */
#include <stdbool.h>

/* ref platform headers */
#include "plat_defines.h"
#include "plat_errno.h"
#include "plat_math.h"
#include "plat_type.h"

/* media api headers to app */
#include "mm_comm_eis.h"
#include "mm_comm_video.h"
#include "mm_common.h"

/* media internal common headers */
#include "ComponentCommon.h"
#include "media_common.h"
#include "mm_component.h"
#include "tmessage.h"
#include "tsemaphore.h"

#define EIS_MAXPATHSIZE (256)

#define GYRO_DEV_NAME "/dev/iio:device0"
#define GYRO_DEV_DIR_PATH "/sys/bus/iio/devices/iio:device0"

/* private interface */
ERRORTYPE VideoEISGetPortDefinition(PARAM_IN COMP_HANDLETYPE hComponent,
                                   PARAM_INOUT COMP_PARAM_PORTDEFINITIONTYPE* pPortDef);
ERRORTYPE VideoEISSetPortDefinition(PARAM_IN COMP_HANDLETYPE hComponent,
                                   PARAM_IN COMP_PARAM_PORTDEFINITIONTYPE* pPortDef);
ERRORTYPE VideoEISGetCompBufferSupplier(PARAM_IN COMP_HANDLETYPE hComponent,
                                       PARAM_INOUT COMP_PARAM_BUFFERSUPPLIERTYPE* pPortBufSupplier);
ERRORTYPE VideoEISSetCompBufferSupplier(PARAM_IN COMP_HANDLETYPE hComponent,
                                       PARAM_IN COMP_PARAM_BUFFERSUPPLIERTYPE* pPortBufSupplier);
ERRORTYPE VideoEISGetMPPChannelInfo(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT MPP_CHN_S* pChn);
ERRORTYPE VideoEISSetMPPChannelInfo(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN MPP_CHN_S* pChn);

ERRORTYPE VideoEISSendCommand(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_COMMANDTYPE Cmd,
                             PARAM_IN unsigned int nParam1, PARAM_IN void* pCmdData);
ERRORTYPE VideoEISGetState(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT COMP_STATETYPE* pState);
ERRORTYPE VideoEISSetCallbacks(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_CALLBACKTYPE* pCallbacks,
                              PARAM_IN void* pAppData);
ERRORTYPE VideoEISGetConfig(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_INDEXTYPE nIndex,
                           PARAM_IN void* pComponentConfigStructure);
ERRORTYPE VideoEISSetConfig(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_INDEXTYPE nIndex,
                           PARAM_IN void* pComponentConfigStructure);
ERRORTYPE VideoEISComponentTunnelRequest(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN unsigned int nPort,
                                        PARAM_IN COMP_HANDLETYPE hTunneledComp, PARAM_IN unsigned int nTunneledPort,
                                        PARAM_INOUT COMP_TUNNELSETUPTYPE* pTunnelSetup);
ERRORTYPE VideoEISEmptyThisBuffer(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_BUFFERHEADERTYPE* pBuffer);
ERRORTYPE VideoEISFillThisBuffer(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_BUFFERHEADERTYPE* pBuffer);

/* public interface */
ERRORTYPE VideoEISComponentDeInit(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoEISComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);

#endif /* __IPCLINUX_VIDEOEIS_COMPONENT_H__ */

/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : Demux_Component.c
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/05
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_DEMUX_COMPONENT_H__
#define __IPCLINUX_DEMUX_COMPONENT_H__

//ref platform headers
#include "plat_defines.h"
#include "plat_errno.h"
#include "plat_math.h"
#include "plat_type.h"

//media api headers to app
#include "mm_comm_demux.h"  //"mm_comm_video.h"
#include "mm_common.h"
//#include "mpi_demux.h"          //"mpi_video.h"

//media internal common headers.
#include "ComponentCommon.h"
#include "aw_demux.h"
#include "cedarx_demux.h"
#include "media_common.h"
#include "mm_component.h"
#include "tmessage.h"
#include "tsemaphore.h"

//------------------------------------------------------------------------------------

#define DEMUX_MAX_PORT_COUNT 4

#define TS_PROCESS_PACKET_SIZE (256 * 1024)
#define MAX_CACHE_PERCENT 15

//private interface
ERRORTYPE DemuxGetPortDefinition(PARAM_IN COMP_HANDLETYPE hComponent,
                                 PARAM_INOUT COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE DemuxSetPortDefinition(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE DemuxGetCompBufferSupplier(PARAM_IN COMP_HANDLETYPE hComponent,
                                     PARAM_INOUT COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE DemuxSetCompBufferSupplier(PARAM_IN COMP_HANDLETYPE hComponent,
                                     PARAM_IN COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE DemuxGetMPPChannelInfo(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT MPP_CHN_S *pChn);
ERRORTYPE DemuxSetMPPChannelInfo(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN MPP_CHN_S *pChn);
ERRORTYPE DemuxGetChannelFd(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT int *pChnFd);
ERRORTYPE DemuxGetTunnelInfo(PARAM_IN COMP_HANDLETYPE hComponent,
                             PARAM_INOUT COMP_INTERNAL_TUNNELINFOTYPE *pTunnelInfo);
ERRORTYPE DemuxGetChnAttr(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT DEMUX_CHN_ATTR_S *pChnAttr);
ERRORTYPE DemuxSetChnAttr(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT DEMUX_CHN_ATTR_S *pChnAttr);

ERRORTYPE DemuxResetChannel(PARAM_IN COMP_HANDLETYPE hComponent);

//public interface
ERRORTYPE DemuxSendCommand(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_COMMANDTYPE Cmd,
                           PARAM_IN unsigned int nParam1, PARAM_IN void *pCmdData);
ERRORTYPE DemuxGetState(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT COMP_STATETYPE *pState);
ERRORTYPE DemuxSetCallbacks(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_CALLBACKTYPE *pCallbacks,
                            PARAM_IN void *pAppData);
ERRORTYPE DemuxGetConfig(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_INDEXTYPE nIndex,
                         PARAM_IN void *pComponentConfigStructure);
ERRORTYPE DemuxSetConfig(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_INDEXTYPE nIndex,
                         PARAM_IN void *pComponentConfigStructure);
ERRORTYPE DemuxComponentTunnelRequest(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN unsigned int nPort,
                                      PARAM_IN COMP_HANDLETYPE hTunneledComp, PARAM_IN unsigned int nTunneledPort,
                                      PARAM_INOUT COMP_TUNNELSETUPTYPE *pTunnelSetup);
ERRORTYPE DemuxEmptyThisBuffer(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_BUFFERHEADERTYPE *pBuffer);
ERRORTYPE DemuxFillThisBuffer(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_BUFFERHEADERTYPE *pBuffer);
ERRORTYPE DemuxComponentDeInit(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE DemuxComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);

#define COMP_TICKS_PER_SECOND 1000000

#endif /* __IPCLINUX_DEMUX_COMPONENT_H__ */

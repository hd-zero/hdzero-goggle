/*******************************************************************************
--                                                                            --
--                    CedarX Multimedia Framework                             --
--                                                                            --
--          the Multimedia Framework for Linux/Android System                 --
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                         Softwinner Products.                               --
--                                                                            --
--                   (C) COPYRIGHT 2011 SOFTWINNER PRODUCTS                   --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
*******************************************************************************/

#ifndef _CLOCK_COMPONENT_H_
#define _CLOCK_COMPONENT_H_

#include <mm_component.h>
#include <ComponentCommon.h>
#include <ClockCompPortIndex.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AVS_ADJUST_PERIOD (5*1000*1000) //us

ERRORTYPE ClockGetPortDefinition(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_INOUT COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE ClockGetCompBufferSupplier(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_INOUT COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE ClockGetClockState(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT COMP_TIME_CONFIG_CLOCKSTATETYPE *pClockState);
ERRORTYPE ClockGetCurrentWallTime(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT COMP_TIME_CONFIG_TIMESTAMPTYPE *pTimeStamp);
ERRORTYPE ClockGetCurrentMediaTime(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT COMP_TIME_CONFIG_TIMESTAMPTYPE *pTimeStamp);
ERRORTYPE ClockGetVps(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT int *pVps);
ERRORTYPE ClockSetPortDefinition(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE ClockSetCompBufferSupplier(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE ClockSeek(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE ClockSwitchAudio(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE ClockAdjustClock(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN COMP_TIME_CONFIG_TIMESTAMPTYPE *pTimeStamp);
ERRORTYPE ClockSetVps(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN int adjust_ratio);
ERRORTYPE ClockSetClockState(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN COMP_TIME_CONFIG_CLOCKSTATETYPE *pClockState);
ERRORTYPE ClockSetClientStartTime(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN COMP_TIME_CONFIG_TIMESTAMPTYPE *pRefTimeStamp);
ERRORTYPE ClockSetClientForceStartTime(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN COMP_TIME_CONFIG_TIMESTAMPTYPE *pRefTimeStamp);
ERRORTYPE ClockSetActiveRefClock(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN COMP_TIME_CONFIG_ACTIVEREFCLOCKTYPE *pRefClock);
ERRORTYPE ClockSetWallTimeBase(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN int64_t mediaTime);
ERRORTYPE ClockSetNotifyStartToRunInfo(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN CDX_NotifyStartToRunTYPE *pNotifyStartToRunInfo);
ERRORTYPE ClockSendCommand(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_COMMANDTYPE Cmd, PARAM_IN unsigned int nParam1, PARAM_IN void* pCmdData);
ERRORTYPE ClockGetState(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_OUT COMP_STATETYPE* pState);
ERRORTYPE ClockSetCallbacks(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_CALLBACKTYPE* pCallbacks, PARAM_IN void* pAppData);
ERRORTYPE ClockGetConfig(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_INDEXTYPE nIndex, PARAM_INOUT void* pComponentConfigStructure);
ERRORTYPE ClockSetConfig(PARAM_IN COMP_HANDLETYPE hComponent, PARAM_IN COMP_INDEXTYPE nIndex, PARAM_IN void* pComponentConfigStructure);
ERRORTYPE ClockComponentTunnelRequest(
        PARAM_IN  COMP_HANDLETYPE hComponent,
        PARAM_IN  unsigned int nPort,
        PARAM_IN  COMP_HANDLETYPE hTunneledComp,
        PARAM_IN  unsigned int nTunneledPort,
        PARAM_INOUT  COMP_TUNNELSETUPTYPE* pTunnelSetup);
ERRORTYPE ClockComponentDeInit(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE ClockComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _CLOCK_COMPONENT_H_ */


/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : VideoDec_Component.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/19
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_VIDEODEC_COMPONENT_H__
#define __IPCLINUX_VIDEODEC_COMPONENT_H__

//ref platform headers
#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

//media api headers to app
#include "mm_common.h"
#include "mm_comm_video.h"
#include <mm_comm_vdec.h>

//media internal common headers.
#include "media_common.h"
#include "mm_component.h"
#include "ComponentCommon.h"
#include "tmessage.h"
#include "tsemaphore.h"
#include <VdecCompStream.h>
#include <vdecoder.h>

typedef enum VIDEODECODER_PORT_INDEX_DEFINITION{
    VDEC_PORT_INDEX_DEMUX = 0x0,
    VDEC_OUT_PORT_INDEX_VRENDER = 0x1,
    VDEC_PORT_INDEX_CLOCK = 0x2,
}VIDEODECODER_PORT_INDEX_DEFINITION;

ERRORTYPE VideoDecGetMPPChannelInfo(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT MPP_CHN_S *pChn);
ERRORTYPE VideoDecGetPortDefinition(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_INOUT COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE VideoDecGetCompBufferSupplier(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_INOUT COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE VideoDecGetBufferState(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT COMP_BUFFERSTATE* pBufferState);
ERRORTYPE VideoDecGetFbmBufInfo(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT FbmBufInfo* pFbmBufInfo);
ERRORTYPE VideoDecGetChnAttr(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT VDEC_CHN_ATTR_S* pChnAttr);
ERRORTYPE VideoDecGetChnState(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_OUT VDEC_CHN_STAT_S *pChnStat);
ERRORTYPE VideoDecGetParam(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT VDEC_CHN_PARAM_S* pChnParam);
ERRORTYPE VideoDecGetProtocolParam(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT VDEC_PRTCL_PARAM_S *pProtocolParam);
/** 
 * get frame, used in non-tunnel mode.
 *
 * @return SUCCESS.
 * @param hComponent vdec component.
 * @param pFrameInfo store frame info, caller malloc.
 * @param nMilliSec 0:return immediately, <0:wait forever, >0:wait some time.
 */
ERRORTYPE VideoDecGetFrame(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT VIDEO_FRAME_INFO_S *pFrameInfo,
        PARAM_OUT VIDEO_FRAME_INFO_S *pSubFrameInfo,
        PARAM_IN int nMilliSec);
ERRORTYPE VideoDecGetRotate(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_OUT ROTATE_E *pRotate);
ERRORTYPE VideoDecSetPortDefinition(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN COMP_PARAM_PORTDEFINITIONTYPE *pPortDef);
ERRORTYPE VideoDecSetCompBufferSupplier(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN COMP_PARAM_BUFFERSUPPLIERTYPE *pPortBufSupplier);
ERRORTYPE VideoDecSetIonFd(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN int nIonFd);
ERRORTYPE VideoDecSeek(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoDecSetStreamEof(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoDecClearStreamEof(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoDecSetRotate(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN ROTATE_E eRotate);
ERRORTYPE VideoDecSetOutputPixelFormat(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN enum EPIXELFORMAT eVdecPixelFormat);
ERRORTYPE VideoDecSetDisplayFrameRequestMode(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN int nFrameRequest);
ERRORTYPE VideoDecSetMPPChannelInfo(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN MPP_CHN_S *pChn);
ERRORTYPE VideoDecSetChnAttr(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN VDEC_CHN_ATTR_S *pChnAttr);
ERRORTYPE VideoDecResetChannel(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoDecSetChnParam(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN VDEC_CHN_PARAM_S *pChnParam);
ERRORTYPE VideoDecSetProtocolParam(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN VDEC_PRTCL_PARAM_S *pPrtclParam);
/** 
 * release frame, used in non-tunnel mode.
 *
 * @return SUCCESS.
 * @param hComponent vdec component.
 * @param pFrameInfo frame info, caller malloc.
 */
ERRORTYPE VideoDecReleaseFrame(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN VIDEO_FRAME_INFO_S *pFrameInfo,
        PARAM_IN VIDEO_FRAME_INFO_S *pSubFrameInfo);
ERRORTYPE VideoDecSetFrameBuffersToVdecLib(
        PARAM_IN COMP_HANDLETYPE hComponent, 
        PARAM_IN VDecCompFrameBuffersParam *pParam);
ERRORTYPE VideoDecReopenVideoEngine(PARAM_IN COMP_HANDLETYPE hComponent);
ERRORTYPE VideoDecSendCommand(COMP_HANDLETYPE hComponent, COMP_COMMANDTYPE Cmd, unsigned int nParam1, void* pCmdData);
ERRORTYPE VideoDecGetState(COMP_HANDLETYPE hComponent, COMP_STATETYPE* pState);
ERRORTYPE VideoDecSetCallbacks(COMP_HANDLETYPE hComponent, COMP_CALLBACKTYPE* pCallbacks, void* pAppData);
ERRORTYPE VideoDecGetConfig(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_INDEXTYPE nIndex,
        PARAM_INOUT void* pComponentConfigStructure);
ERRORTYPE VideoDecSetConfig(
        PARAM_IN COMP_HANDLETYPE hComponent,
        PARAM_IN COMP_INDEXTYPE nIndex,
        PARAM_IN void* pComponentConfigStructure);
ERRORTYPE VideoDecComponentTunnelRequest(
        PARAM_IN  COMP_HANDLETYPE hComponent,
        PARAM_IN  unsigned int nPort,
        PARAM_IN  COMP_HANDLETYPE hTunneledComp,
        PARAM_IN  unsigned int nTunneledPort,
        PARAM_INOUT  COMP_TUNNELSETUPTYPE* pTunnelSetup);
ERRORTYPE VideoDecRequstBuffer(COMP_HANDLETYPE hComponent, unsigned int nPortIndex, COMP_BUFFERHEADERTYPE* pBuffer);
/** 
 * update vbs data to VdecLib, used in tunnel mode.
 *
 * @return SUCCESS.
 * @param hComponent vdec component.
 * @param nPortIndex indicate vbs inputPort index.
 * @param pBuffer contain vbs data struct.
 */
ERRORTYPE VideoDecReleaseBuffer(COMP_HANDLETYPE hComponent, unsigned int nPortIndex, COMP_BUFFERHEADERTYPE* pBuffer);
/** 
 * send stream to vdeclib, can used in tunnel/non-tunnel mode.
 *
 * @return SUCCESS
 * @param hComponent vdecComp handle.
 * @param pBuffer stream info.
 */
ERRORTYPE VideoDecEmptyThisBuffer(
        PARAM_IN  COMP_HANDLETYPE hComponent,
        PARAM_IN  COMP_BUFFERHEADERTYPE* pBuffer);
/** 
 * return frame to vdeclib in tunnel mode.
 *
 * @return SUCCESS
 * @param hComponent vdecComp handle.
 * @param pBuffer frame buffer info.
 */
ERRORTYPE VideoDecFillThisBuffer(
            PARAM_IN  COMP_HANDLETYPE hComponent,
            PARAM_IN  COMP_BUFFERHEADERTYPE* pBuffer);
ERRORTYPE VideoDecComponentDeInit(COMP_HANDLETYPE hComponent);
ERRORTYPE VideoDecComponentInit(PARAM_IN COMP_HANDLETYPE hComponent);

#endif  /* __IPCLINUX_VIDEODEC_COMPONENT_H__ */


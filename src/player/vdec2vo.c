//#define LOG_NDEBUG 0
#define LOG_TAG "vdec2vo"
#include <plat_log.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "vdec2vo.h"

enum CLOCK_COMP_PORT_INDEX {
    CLOCK_PORT_AUDIO = 0, //to audio render
    CLOCK_PORT_VIDEO = 1, //to video render
    CLOCK_PORT_DEMUX = 2, //to demux
    CLOCK_PORT_VDEC  = 3, //to vdec
};

static void vdec2vo_initContext(Vdec2VoContext_t* vvCtx)
{
    memset(vvCtx, 0, sizeof(Vdec2VoContext_t));

    vvCtx->params.initRotation = 0;
    vvCtx->params.pixelFormat = MM_PIXEL_FORMAT_YVU_PLANAR_420;
    //vvCtx->pixelFormat = MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    vvCtx->params.vdec.codecType = PT_H264;
    vvCtx->params.vdec.width = 1280;
    vvCtx->params.vdec.height = 720;

    vvCtx->params.vo.intfType = VO_INTF_LCD;
    vvCtx->params.vo.intfSync = VO_OUTPUT_NTSC;
    vvCtx->params.vo.width = 720;
    vvCtx->params.vo.height = 1280;
    vvCtx->params.vo.uiChn = 2;

    vvCtx->vdecChn = MM_INVALID_CHN;

    vvCtx->voDev = MM_INVALID_DEV;
    vvCtx->voLayer = MM_INVALID_CHN;
    vvCtx->voChn = MM_INVALID_CHN;
    vvCtx->uiLayer = MM_INVALID_CHN;
    vvCtx->flagEOF = 0;
}

static ERRORTYPE MPPCallbackWrapper(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    Vdec2VoContext_t *vvCtx = (Vdec2VoContext_t *)cookie;

    if (pChn->mModId == MOD_ID_VDEC) {
        switch (event) {
        case MPP_EVENT_NOTIFY_EOF:
            alogd("vdec to the end of file");
            if (vvCtx->voChn >= 0) {
                AW_MPI_VO_SetStreamEof(vvCtx->voLayer, vvCtx->voChn, 1);
            }
            vvCtx->flagEOF |= VDEC2VO_vdecEOF;
            break;

        default:
            break;
        }
    } else if (pChn->mModId == MOD_ID_VOU) {
        switch (event) {
        case MPP_EVENT_NOTIFY_EOF:
            alogd("vo to the end of file");
            vvCtx->flagEOF |= VDEC2VO_voEOF;
            break;

        case MPP_EVENT_RENDERING_START:
            alogd("vo start to rendering");
            break;

        default:
            break;
        }
    }

    return SUCCESS;
}

static ERRORTYPE vdec2vo_createClockChn(Vdec2VoContext_t *vvCtx, CLOCK_CHN_ATTR_S* clockChnAttr)
{
    ERRORTYPE ret;
    BOOL bSuccessFlag = FALSE;
    CLOCK_CHN clkChn = 0;

    vvCtx->params.clkChn = 0;
    while (clkChn < CLOCK_MAX_CHN_NUM) {
        ret = AW_MPI_CLOCK_CreateChn(clkChn, clockChnAttr);
        if (SUCCESS == ret) {
            bSuccessFlag = TRUE;
            alogd("create clock channel[%d] success!", clkChn);
            break;
        } else if (ERR_CLOCK_EXIST == ret) {
            alogd("clock channel[%d] is exist, find next!", clkChn);
            clkChn++;
        } else {
            alogd("create clock channel[%d] ret[0x%x]!", clkChn, ret);
            break;
        }
    }

    if (FALSE == bSuccessFlag) {
        vvCtx->params.clkChn = MM_INVALID_CHN;
        aloge("fatal error! create clock channel fail!");
        return FAILURE;
    }

    vvCtx->params.clkChn = clkChn;

    return SUCCESS;
}

static void vdec2vo_configVdecChnAttr(VDEC_CHN_ATTR_S* vdecChnAttr, Vdec2VoParams_t* config)
{
    memset(vdecChnAttr, 0, sizeof(VDEC_CHN_ATTR_S));
    vdecChnAttr->mPicWidth = config->vdec.width;
    vdecChnAttr->mPicHeight = config->vdec.height;
    vdecChnAttr->mInitRotation = config->initRotation;
    vdecChnAttr->mOutputPixelFormat = config->pixelFormat;
    vdecChnAttr->mType = config->vdec.codecType;
    vdecChnAttr->mVdecVideoAttr.mSupportBFrame = 0; //1
    vdecChnAttr->mVdecVideoAttr.mMode = VIDEO_MODE_FRAME;
}

static ERRORTYPE vdec2vo_createVdecChn(Vdec2VoContext_t *vvCtx, Vdec2VoParams_t* config)
{
    ERRORTYPE ret;
    BOOL nSuccessFlag = FALSE;
    VDEC_CHN_ATTR_S  vdecChnAttr;

    vdec2vo_configVdecChnAttr(&vdecChnAttr, config);
    vvCtx->vdecChn = 0;
    while (vvCtx->vdecChn < VDEC_MAX_CHN_NUM) {
        ret = AW_MPI_VDEC_CreateChn(vvCtx->vdecChn, &vdecChnAttr);
        if (SUCCESS == ret) {
            nSuccessFlag = TRUE;
            alogd("create vdec channel[%d] success!", vvCtx->vdecChn);
            break;
        } else if (ERR_VDEC_EXIST == ret) {
            alogd("vdec channel[%d] is exist, find next!", vvCtx->vdecChn);
            vvCtx->vdecChn++;
        } else {
            alogd("create vdec channel[%d] ret[0x%x]!", vvCtx->vdecChn, ret);
            break;
        }
    }

    if (FALSE == nSuccessFlag) {
        vvCtx->vdecChn = MM_INVALID_CHN;
        aloge("fatal error! create vdec channel fail!");
        return FAILURE;
    } else {
        alogd("add call back");
        MPPCallbackInfo cbInfo;
        cbInfo.cookie = (void*)vvCtx;
        cbInfo.callback = (MPPCallbackFuncType)&MPPCallbackWrapper;
        AW_MPI_VDEC_RegisterCallback(vvCtx->vdecChn, &cbInfo);
        return SUCCESS;
    }
}

static ERRORTYPE vdec2vo_setLayerTop(VO_LAYER voLayer)
{
    return AW_MPI_VO_SetVideoLayerPriority(voLayer, ZORDER_MAX);
}

static ERRORTYPE vdec2vo_setLayerAlpha(VO_LAYER voLayer, unsigned char alpha)
{
   VO_VIDEO_LAYER_ALPHA_S stAlpha;
    stAlpha.mAlphaMode = 0;
    stAlpha.mAlphaValue = alpha;

    ERRORTYPE ret = AW_MPI_VO_SetVideoLayerAlpha(voLayer, &stAlpha);

    if (ret != SUCCESS) {
        aloge("set failed, hlay: %d, alpha: %d", voLayer, alpha);
    }

    return ret;
}

static ERRORTYPE vdec2vo_createVoChn(Vdec2VoContext_t *vvCtx, Vdec2VoParams_t* config)
{
    ERRORTYPE ret;
    BOOL nSuccessFlag = FALSE;
    VO_VIDEO_LAYER_ATTR_S voLayerAttr;

    memset(&voLayerAttr, 0, sizeof(voLayerAttr));

    vvCtx->voDev = 0;
    vvCtx->uiLayer = HLAY(config->vo.uiChn, 0);//HLAY(2, 0);

    AW_MPI_VO_Enable(vvCtx->voDev);
    AW_MPI_VO_AddOutsideVideoLayer(vvCtx->uiLayer);
    //AW_MPI_VO_CloseVideoLayer(vvCtx->uiLayer);//close ui layer.

    //enable vo layer
    int hlay0 = 0;
    while (hlay0 < VO_MAX_LAYER_NUM) {
        if (SUCCESS == AW_MPI_VO_EnableVideoLayer(hlay0)) {
            break;
        }
        hlay0++;
    }

    if (hlay0 >= VO_MAX_LAYER_NUM) {
        aloge("fatal error! enable video layer fail!");
        vvCtx->voLayer = MM_INVALID_DEV;
        AW_MPI_VO_RemoveOutsideVideoLayer(vvCtx->uiLayer);
        AW_MPI_VO_Disable(vvCtx->voDev);
        return FAILURE;
    }

    VO_PUB_ATTR_S spPubAttr;
    AW_MPI_VO_GetPubAttr(vvCtx->voDev, &spPubAttr);
    spPubAttr.enIntfType = config->vo.intfType;
    spPubAttr.enIntfSync = config->vo.intfSync;
    AW_MPI_VO_SetPubAttr(vvCtx->voDev, &spPubAttr);

    vvCtx->voLayer = hlay0;
    //AW_MPI_VO_SetVideoLayerPriority(vvCtx->voLayer, 11);
    AW_MPI_VO_GetVideoLayerAttr(vvCtx->voLayer, &voLayerAttr);

    voLayerAttr.stDispRect.X = 0;
    voLayerAttr.stDispRect.Y = 0;
    voLayerAttr.stDispRect.Width = config->vo.width;//720;//1920;
    voLayerAttr.stDispRect.Height = config->vo.height;//1280;//1080;
    voLayerAttr.enPixFormat = config->pixelFormat;
    AW_MPI_VO_SetVideoLayerAttr(vvCtx->voLayer, &voLayerAttr);

    vvCtx->voChn = 0;
    while (vvCtx->voChn < VO_MAX_CHN_NUM) {
        ret = AW_MPI_VO_EnableChn(vvCtx->voLayer, vvCtx->voChn);
        if (SUCCESS == ret) {
            nSuccessFlag = TRUE;
            alogd("create vo channel[%d] success!", vvCtx->voChn);
            break;
        } else if(ERR_VO_CHN_NOT_DISABLE == ret) {
            alogd("vo channel[%d] is exist, find next!", vvCtx->voChn);
            vvCtx->voChn++;
        } else {
            alogd("create vo channel[%d] ret[0x%x]!", vvCtx->voChn, ret);
            break;
        }
    }

    if (FALSE == nSuccessFlag) {
        vvCtx->voChn = MM_INVALID_CHN;
        aloge("fatal error! create vo channel fail!");
        return FAILURE;
    } else {
        MPPCallbackInfo cbInfo;
        cbInfo.cookie = (void*)vvCtx;
        cbInfo.callback = (MPPCallbackFuncType)&MPPCallbackWrapper;
        AW_MPI_VO_RegisterCallback(vvCtx->voLayer, vvCtx->voChn, &cbInfo);
        AW_MPI_VO_SetChnDispBufNum(vvCtx->voLayer, vvCtx->voChn, 2);
        return SUCCESS;
    }
}

static void vdec2vo_destroy(Vdec2VoContext_t* vvCtx)
{
    alogd("begin");

    //vo stop/destroy must before vdec stop (when vo destroy, will return buffer to vdec(2 frames), just all buffer sync)
    if (vvCtx->voLayer >= 0) {
        if (vvCtx->voChn >= 0) {
            AW_MPI_VO_DisableChn(vvCtx->voLayer, vvCtx->voChn);
            vvCtx->voChn = MM_INVALID_CHN;
        }

        AW_MPI_VO_DisableVideoLayer(vvCtx->voLayer);
        vvCtx->voLayer = MM_INVALID_CHN;
        AW_MPI_VO_RemoveOutsideVideoLayer(vvCtx->uiLayer);
        AW_MPI_VO_Disable(vvCtx->voDev);
        vvCtx->voDev = MM_INVALID_DEV;
    }
    if (vvCtx->vdecChn >= 0) {
        AW_MPI_VDEC_DestroyChn(vvCtx->vdecChn);
    }

    alogd("done");
}

ERRORTYPE vdec2vo_prepare(Vdec2VoContext_t *vvCtx, Vdec2VoParams_t* params)
{
    ERRORTYPE ret;

    ret = vdec2vo_createVdecChn(vvCtx, params);
    if (ret != SUCCESS) {
        alogd("create vdec chn failed: %x", ret);
        goto failed;
    }

    ret = vdec2vo_createVoChn(vvCtx, params);
    if (ret == SUCCESS) {
        alogd("bind vdec & vo");
        MPP_CHN_S VdecChn = {MOD_ID_VDEC, 0, vvCtx->vdecChn};
        MPP_CHN_S VoChn = {MOD_ID_VOU, vvCtx->voLayer, vvCtx->voChn};
        MPP_CHN_S DmxChn = {MOD_ID_DEMUX, 0, params->dmxChn};
        MPP_CHN_S ClockChn = {MOD_ID_CLOCK, 0, params->clkChn};

        AW_MPI_SYS_Bind(&DmxChn, &VdecChn);
        AW_MPI_SYS_Bind(&VdecChn, &VoChn);
        AW_MPI_SYS_Bind(&ClockChn, &VoChn);
    } else {
        alogd("create vo chn fail");
        goto failed;
    }

    memcpy(&vvCtx->params, params, sizeof(Vdec2VoParams_t));
    return SUCCESS;

failed:
    vdec2vo_destroy(vvCtx);
    return ret;
}

ERRORTYPE vdec2vo_start(Vdec2VoContext_t *vvCtx)
{
    ERRORTYPE ret = SUCCESS;

    alogd("start stream");

    if (vvCtx->vdecChn >= 0) {
        ret = AW_MPI_VDEC_StartRecvStream(vvCtx->vdecChn);
    }

    if ((vvCtx->voLayer >= 0) && (vvCtx->voChn >= 0)) {
        ret = AW_MPI_VO_StartChn(vvCtx->voLayer, vvCtx->voChn);
    }

    vvCtx->flagEOF = 0;

    return ret;
}

ERRORTYPE vdec2vo_pause(Vdec2VoContext_t *vvCtx)
{
    ERRORTYPE ret = SUCCESS;

    if (vvCtx->vdecChn >= 0) {
        ret = AW_MPI_VDEC_Pause(vvCtx->vdecChn);
    }
    if ((ret==ERR_VDEC_INCORRECT_STATE_TRANSITION) && (vvCtx->flagEOF>0)) {
        ret = SUCCESS;
    }

    if ((vvCtx->voLayer >= 0) && (vvCtx->voChn >= 0)) {
        ret = AW_MPI_VO_PauseChn(vvCtx->voLayer, vvCtx->voChn);
    }
    if ((ret==ERR_VO_CHN_INCORRECT_STATE_TRANSITION) && (vvCtx->flagEOF>0)) {
        ret = SUCCESS;
    }

    return ret;
}

ERRORTYPE vdec2vo_stop(Vdec2VoContext_t *vvCtx)
{
    ERRORTYPE ret = SUCCESS;

//vo stop must before vdec stop (when vo stop, will return buffer to vdec, just all buffer sync)
    if ((vvCtx->voLayer >=0) && (vvCtx->voChn >= 0)) {
        alogd("stop vo chn");
        ret = AW_MPI_VO_StopChn(vvCtx->voLayer, vvCtx->voChn);
    }

    if (vvCtx->vdecChn >= 0) {
        ret = AW_MPI_VDEC_StopRecvStream(vvCtx->vdecChn);
    }

    return ret;
}

ERRORTYPE vdec2vo_seekTo(Vdec2VoContext_t *vvCtx)
{
    ERRORTYPE ret = SUCCESS;

    alogd("seek to");

    if (vvCtx->vdecChn >= 0) {
        ret = AW_MPI_VDEC_SetStreamEof(vvCtx->vdecChn, 0);
    }
    if ((vvCtx->voLayer >= 0) && (vvCtx->voChn >= 0)) {
        AW_MPI_VO_SetStreamEof(vvCtx->voLayer, vvCtx->voChn, 0);
    }

    if (vvCtx->vdecChn >= 0) {
        ret = AW_MPI_VDEC_Seek(vvCtx->vdecChn);
    }
    if ((vvCtx->voLayer >= 0) && (vvCtx->voChn >= 0)) {
        ret = AW_MPI_VO_Seek(vvCtx->voLayer, vvCtx->voChn);
    }

    return ret;
}

ERRORTYPE vdec2vo_currentMediaTime(Vdec2VoContext_t *vvCtx, int* mediaTime)
{
    ERRORTYPE ret = SUCCESS;

    if (vvCtx->params.clkChn >= 0) {
        ret = AW_MPI_CLOCK_GetCurrentMediaTime(vvCtx->params.clkChn, mediaTime);
    }

    return ret;
}

Vdec2VoContext_t* vdec2vo_initSys(void)
{
    Vdec2VoContext_t* vvCtx = (Vdec2VoContext_t*)malloc(sizeof(Vdec2VoContext_t));
    if( vvCtx != NULL ) {
        MPP_SYS_CONF_S mSysConf;
        mSysConf.nAlignWidth = 32;
        AW_MPI_SYS_SetConf(&mSysConf);
        AW_MPI_SYS_Init();

        vdec2vo_initContext(vvCtx);
    }

    return vvCtx;
}

void vdec2vo_deinitSys(Vdec2VoContext_t* vvCtx)
{
    if( vvCtx != NULL ) {
        vdec2vo_destroy(vvCtx);
        AW_MPI_SYS_Exit();
    }
}

ERRORTYPE vdec2vo_setVdecEof(Vdec2VoContext_t* vvCtx)
{
    ERRORTYPE ret = SUCCESS;

    if( !(vvCtx->flagEOF & VDEC2VO_vdecEOF_N) ) {
        if(vvCtx->vdecChn >= 0)
        {
            ret = AW_MPI_VDEC_SetStreamEof(vvCtx->vdecChn, TRUE);
        }

        vvCtx->flagEOF |= VDEC2VO_vdecEOF_N;
    }

    return ret;
}

ERRORTYPE vdec2vo_setVoEof(Vdec2VoContext_t* vvCtx)
{
    ERRORTYPE ret = SUCCESS;

    if( !(vvCtx->flagEOF & VDEC2VO_voEOF_N) ) {
        if(vvCtx->voChn >= 0)
        {
            ret = AW_MPI_VO_SetStreamEof(vvCtx->voLayer, vvCtx->voChn, TRUE);
        }

        vvCtx->flagEOF |= VDEC2VO_voEOF_N;
    }

    return ret;
}

ERRORTYPE vdec2vo_checkEof(Vdec2VoContext_t* vvCtx)
{
    ERRORTYPE ret = SUCCESS;

    ret = vdec2vo_setVdecEof(vvCtx);

    if( vvCtx->flagEOF & VDEC2VO_vdecEOF ) {
        vdec2vo_setVoEof(vvCtx);
    }

    return ret;
}

bool vdec2vo_isEOF(Vdec2VoContext_t *vvCtx)
{
    return vvCtx->flagEOF & VDEC2VO_voEOF;
}

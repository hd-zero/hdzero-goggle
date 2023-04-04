/******************************************************************************
  Copyright (C), 2001-2017, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : sample_virvi2venc.c
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/1/5
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/

//#define LOG_NDEBUG 0
#define LOG_TAG "vi2enc"
#include <log/log.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vi2venc.h"
#include "spspps_patch.h"

#define max(a, b)  ((a)>(b) ? (a) : (b))

static VencFrameType_e vi2venc_frameType(Vi2Venc_t* vv, VENC_PACK_S* vencPack)
{
    VencFrameType_e frameType = FRAME_typeO;

    switch(vv->veParams.codecType) {
    case PT_H264:
        switch (vencPack->mDataType.enH264EType) {
        case H264E_NALU_ISLICE:
            frameType = FRAME_typeI;
            break;
        case H264E_NALU_PSLICE:
            frameType = FRAME_typeP;
            break;
        default:
            break;
        }
        break;
    case PT_H265:
        switch (vencPack->mDataType.enH265EType) {
        case H265E_NALU_ISLICE:
            frameType = FRAME_typeI;
            break;
        case H265E_NALU_PSLICE:
            frameType = FRAME_typeP;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    return frameType;
}

static void* vi2venc_frameProc(void *arg)
{
    Vi2Venc_t* vv = (Vi2Venc_t*)arg;
    VI_DEV vipp_dev = vv->viDev;
    VI_CHN virvi_chn = vv->viChn;
    int ret = 0;
    int count = 0;

    VencFrameType_e frameType;
    VENC_CHN ve_chn = vv->veChn;
    VENC_STREAM_S VencFrame;
    VENC_PACK_S venc_pack;
    VencFrame.mPackCount = 1;
    VencFrame.mpPack = &venc_pack;

    LOGD("venc thread: dev[%d] chn[%d] ve[%d]", vipp_dev, virvi_chn, ve_chn);

    uint8_t* frameData = malloc(2*1024*1024);
    int frameLen = 0;

#if(0)
    VencHeaderData vencheader;
    if(PT_H264 == stContext.mConfigPara.EncoderType) {
        AW_MPI_VENC_GetH264SpsPpsInfo(ve_chn, &vencheader);
        if(vencheader.nLength) {
            //fwrite(vencheader.pBuffer,vencheader.nLength,1,OutputFile_Fd);
        }
    } else if(PT_H265 == stContext.mConfigPara.EncoderType) {
        AW_MPI_VENC_GetH265SpsPpsInfo(ve_chn, &vencheader);
        if(vencheader.nLength) {
        }
    }
#endif

    while( !vv->bExit ) {
        count++;
        if((ret = AW_MPI_VENC_GetStream(ve_chn,&VencFrame,4000)) < 0) { //6000(25fps) 4000(30fps)
            LOGE("get frame failed!");
            //continue;
            break;
        } else {
            if(VencFrame.mpPack != NULL && VencFrame.mpPack->mLen0) {
                memcpy(frameData, VencFrame.mpPack->mpAddr0, VencFrame.mpPack->mLen0);
                frameLen += VencFrame.mpPack->mLen0;
            }
            if(VencFrame.mpPack != NULL && VencFrame.mpPack->mLen1) {
                memcpy(frameData+frameLen, VencFrame.mpPack->mpAddr1, VencFrame.mpPack->mLen1);
                frameLen += VencFrame.mpPack->mLen1;
            }
            ret = AW_MPI_VENC_ReleaseStream(ve_chn,&VencFrame);
            if(ret < 0) {
                LOGW("release failed!");
            }

            if( frameLen > 0 ) {
                if( vv->cbOnFrame ) {
                    frameType = vi2venc_frameType(vv, VencFrame.mpPack);
                    //vv->cbOnFrame(vv, frameData, frameLen, frameType, count*30*90, vv->contextOfOnFrame);
                    vv->cbOnFrame(vv, frameData, frameLen, frameType, venc_pack.mPTS, vv->contextOfOnFrame);
                    //LOGD("%llu", venc_pack.mPTS);
                }
                frameLen = 0;
            }
        }
    }

    free(frameData);
    LOGD("venc thread exit");

    return NULL;
}

static ERRORTYPE MPPCallbackWrapper(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    LOGD("MPPCallbackWrapper: %d\n", event);

    if(MOD_ID_VENC == pChn->mModId)
    {
        switch(event)
        {
            case MPP_EVENT_RELEASE_VIDEO_BUFFER:
            {
                break;
            }
            default:
            {
                break;
            }
        }
    }

    return SUCCESS;
}

static ERRORTYPE vi2venc_initSys0(void)
{
    ERRORTYPE ret;

    MPP_SYS_CONF_S mSysConf;
    memset(&mSysConf, 0, sizeof(MPP_SYS_CONF_S));
    mSysConf.nAlignWidth = 32;

    AW_MPI_SYS_SetConf(&mSysConf);
    ret = AW_MPI_SYS_Init();

    LOGE("sys init: ret=%x", ret);

#if(0)
    if( ret == SUCCESS ) {
        //AW_MPI_VI_SetVIFreq(0,480000000);
        ret = AW_MPI_VENC_SetVEFreq(MM_INVALID_CHN, 632);
        LOGD("set vefreq %dMHz, ret=%d", 632, ret);
    }
#endif

    return ret;
}

Vi2Venc_t* vi2venc_initSys(CB_onFrame onFrame, void* context)
{
    Vi2Venc_t* vv = (Vi2Venc_t*)malloc( sizeof(Vi2Venc_t) );

    if( vv == NULL ) {
        LOGE("sys init: out of memory");
        return NULL;
    }

    memset( vv, 0, sizeof(Vi2Venc_t) );

    if( vi2venc_initSys0() != SUCCESS ) {
        free( vv );
        return NULL;
    }

    vv->cbOnFrame = onFrame;
    vv->contextOfOnFrame = context;
    vv->bExit = false;
    vv->viDev = MM_INVALID_DEV;
    vv->viChn = MM_INVALID_CHN;
    vv->veChn = MM_INVALID_CHN;
    vv->ispDev = MM_INVALID_DEV;

#if(VENC_spsppsPATCH)
    vv->spsppsBuff = malloc(VENC_spsppsLEN);
#endif

    return vv;
}

void vi2venc_deinitSys(Vi2Venc_t* vv)
{
    LOGD("begin");

    /* exit mpp systerm */
    AW_MPI_SYS_Exit();

    if( vv->spsppsBuff != NULL ) {
        free(vv->spsppsBuff);
    }

    free(vv);

    LOGD("done");
}

ERRORTYPE vi2venc_getSpsPpsInfo(Vi2Venc_t* vv, VencSpspps_t* spsppsInfo, bool patch)
{
    int ret = -1;
    VENC_CHN mVeChn = vv->veChn;
    PAYLOAD_TYPE_E codecType = vv->veParams.codecType;
    VencHeaderData veHeader = { NULL, 0 };
    int nLength = 0;
    uint8_t* spsppsData = vv->spsppsBuff;

    if(PT_H264 == codecType) {
        ret = AW_MPI_VENC_GetH264SpsPpsInfo(mVeChn, &veHeader);
        if( ret == SUCCESS ) {
            //LOGD("h264sps: %d, %p", veHeader.nLength, veHeader.pBuffer);
#if(VENC_spsppsPATCH)
            if ( patch ) {
                nLength = max(veHeader.nLength*2, VENC_spsppsLEN);

                memset(spsppsData, 0, nLength);
                memcpy(spsppsData, veHeader.pBuffer, veHeader.nLength);
                nLength = H264_Modify_SPS(spsppsData, veHeader.nLength-1, vv->veParams.fps);
                if( nLength < 0 ) {
                    nLength = veHeader.nLength;
                    spsppsData = veHeader.pBuffer;
                }
            } else
#endif
            {
                nLength = veHeader.nLength;
                spsppsData = veHeader.pBuffer;
            }
        }
    } else if(PT_H265 == codecType) {
        ret = AW_MPI_VENC_GetH265SpsPpsInfo(mVeChn, &veHeader);
        if( ret == SUCCESS ) {
#if(VENC_spsppsPATCH)
            if ( patch ) {
                nLength = max(veHeader.nLength*2, VENC_spsppsLEN);

                memset(spsppsData, 0, nLength);
                memcpy(spsppsData, veHeader.pBuffer, veHeader.nLength);
                nLength = H265_Modify_SPS(spsppsData, veHeader.nLength, vv->veParams.fps);
                if( nLength < 0 ) {
                    nLength = veHeader.nLength;
                    spsppsData = veHeader.pBuffer;
                }
            } else
#endif
            {
                nLength = veHeader.nLength;
                spsppsData = veHeader.pBuffer;
            }
        }
    }

    spsppsInfo->nLength = nLength;
    spsppsInfo->pBuffer = spsppsData;

    return ret;
}

char* vi2venc_getRcModeName(VencRateControlMode_e rcMode)
{
    if ( (rcMode < VENC_rcCBR) || (rcMode > VENC_rcQPMAP) ) {
        return "";
    }

    char* rcModeName[] = VE_rcModeNAME;
    return rcModeName[rcMode];
}

char* vi2venc_getProfileName(VencProfile_e profile, bool h265)
{
    if ( (profile < VENC_profileBASELINE) || (profile > VENC_profileSVC) ) {
        return "";
    }

    if( h265 ) {
        char* profileName[] = VE_h265profileNAME;
        return profileName[profile];
    }

    char* profileName[] = VE_h264profileNAME;
    return profileName[profile];
}

static ERRORTYPE vi2venc_configVencChnAttr(ViParams_t* viParams, VencParams_t* veParams, VENC_CHN_ATTR_S* veChnAttr)
{
    memset(veChnAttr, 0, sizeof(VENC_CHN_ATTR_S));

    veChnAttr->VeAttr.Type = veParams->codecType;
    veChnAttr->VeAttr.MaxKeyInterval = veParams->maxKeyItl;
    veChnAttr->VeAttr.SrcPicWidth  = viParams->width;
    veChnAttr->VeAttr.SrcPicHeight = viParams->height;
    veChnAttr->VeAttr.PixelFormat = veParams->picFormat;
    veChnAttr->VeAttr.Field = VIDEO_FIELD_FRAME;

    if (PT_H264 == veChnAttr->VeAttr.Type)
    {
        veChnAttr->VeAttr.AttrH264e.bByFrame = TRUE;
        veChnAttr->VeAttr.AttrH264e.Profile = veParams->veAttr.mAttrH264.mProfile;// 2;//0:base 1:main 2:high
        veChnAttr->VeAttr.AttrH264e.PicWidth  = veParams->width;
        veChnAttr->VeAttr.AttrH264e.PicHeight = veParams->height;
        switch (veParams->rcMode)
        {
        case VENC_rcVBR:
            veChnAttr->RcAttr.mRcMode = VENC_RC_MODE_H264VBR;
            veChnAttr->RcAttr.mAttrH264Vbr.mSrcFrmRate = veParams->fps;
            veChnAttr->RcAttr.mAttrH264Vbr.mMinQp = veParams->rcAttr.mAttrH264Vbr.mMinQp;//10;
            veChnAttr->RcAttr.mAttrH264Vbr.mMaxQp = veParams->rcAttr.mAttrH264Vbr.mMaxQp;//52;
            veChnAttr->RcAttr.mAttrH264Vbr.mMaxBitRate = veParams->rcAttr.mAttrH264Vbr.mMaxBitRate;
            break;
        case VENC_rcFIXQP:
            veChnAttr->RcAttr.mRcMode = VENC_RC_MODE_H264FIXQP;
            veChnAttr->RcAttr.mAttrH264FixQp.mSrcFrmRate = veParams->fps;
            veChnAttr->RcAttr.mAttrH264FixQp.mIQp = veParams->rcAttr.mAttrH264FixQp.mIQp;//28;
            veChnAttr->RcAttr.mAttrH264FixQp.mPQp = veParams->rcAttr.mAttrH264FixQp.mPQp;//28;
            break;
        case VENC_rcQPMAP:
            veChnAttr->RcAttr.mRcMode = VENC_RC_MODE_H264QPMAP;
            break;
        default:
            veChnAttr->RcAttr.mRcMode = VENC_RC_MODE_H264CBR;
            veChnAttr->RcAttr.mAttrH264Cbr.mSrcFrmRate = veParams->fps;
            veChnAttr->RcAttr.mAttrH264Cbr.mBitRate = veParams->bps;
            break;
        }
        if (veParams->fastEnc)
        {
            veChnAttr->VeAttr.AttrH264e.FastEncFlag = TRUE;
        }
    }
    else if (PT_H265 == veChnAttr->VeAttr.Type)
    {
        veChnAttr->VeAttr.AttrH265e.mbByFrame = TRUE;
        veChnAttr->VeAttr.AttrH265e.mProfile = veParams->veAttr.mAttrH265.mProfile;//0;//0:main 1:main10 2:sti11
        veChnAttr->VeAttr.AttrH265e.mPicWidth = veParams->width;
        veChnAttr->VeAttr.AttrH265e.mPicHeight = veParams->height;
        veChnAttr->RcAttr.mAttrH265Cbr.mBitRate = veParams->bps;
        switch (veParams->rcMode)
        {
        case VENC_rcVBR:
            veChnAttr->RcAttr.mRcMode = VENC_RC_MODE_H265VBR;
            veChnAttr->RcAttr.mAttrH265Vbr.mSrcFrmRate = veParams->fps;
            veChnAttr->RcAttr.mAttrH265Vbr.mMinQp = veParams->rcAttr.mAttrH265Vbr.mMinQp;//10;
            veChnAttr->RcAttr.mAttrH265Vbr.mMaxQp = veParams->rcAttr.mAttrH265Vbr.mMaxQp;//52;
            veChnAttr->RcAttr.mAttrH265Vbr.mMaxBitRate = veParams->rcAttr.mAttrH265Vbr.mMaxBitRate;
            break;
        case VENC_rcFIXQP:
            veChnAttr->RcAttr.mRcMode = VENC_RC_MODE_H265FIXQP;
            veChnAttr->RcAttr.mAttrH265FixQp.mSrcFrmRate = veParams->fps;
            veChnAttr->RcAttr.mAttrH265FixQp.mIQp = veParams->rcAttr.mAttrH265FixQp.mIQp;//28;
            veChnAttr->RcAttr.mAttrH265FixQp.mPQp = veParams->rcAttr.mAttrH265FixQp.mPQp;//28;
            break;
        case VENC_rcQPMAP:
            veChnAttr->RcAttr.mRcMode = VENC_RC_MODE_H265QPMAP;
            break;
        default:
            veChnAttr->RcAttr.mRcMode = VENC_RC_MODE_H265CBR;
            veChnAttr->RcAttr.mAttrH265Cbr.mSrcFrmRate = veParams->fps;
            veChnAttr->RcAttr.mAttrH265Cbr.mBitRate = veParams->bps;
            break;
        }
        if (veParams->fastEnc)
        {
            veChnAttr->VeAttr.AttrH265e.mFastEncFlag = TRUE;
        }
    }
    else if (PT_MJPEG == veChnAttr->VeAttr.Type)
    {
        veChnAttr->VeAttr.AttrMjpeg.mbByFrame = TRUE;
        veChnAttr->VeAttr.AttrMjpeg.mPicWidth = veParams->width;
        veChnAttr->VeAttr.AttrMjpeg.mPicHeight = veParams->height;
        switch (veParams->rcMode)
        {
        case VENC_rcFIXQP:
        case VENC_rcQPMAP:
            veChnAttr->RcAttr.mRcMode = VENC_RC_MODE_MJPEGFIXQP;
        default:
            veChnAttr->RcAttr.mRcMode = VENC_RC_MODE_MJPEGCBR;
            break;
        }
        veChnAttr->RcAttr.mAttrMjpegeCbr.mBitRate = veParams->bps;
    }

    LOGD("venc ste Rcmode=%d", veChnAttr->RcAttr.mRcMode);

    return SUCCESS;
}

static ERRORTYPE vi2venc_createVencChn(Vi2Venc_t* vv, ViParams_t* viParams, VencParams_t* veParams)
{
    ERRORTYPE ret;
    BOOL nSuccessFlag = FALSE;
    VENC_CHN_ATTR_S mVencChnAttr;
    VENC_CHN mVeChn;

    vi2venc_configVencChnAttr(viParams, veParams, &mVencChnAttr);
    mVeChn = 0;
    while (mVeChn < VENC_MAX_CHN_NUM)
    {
        ret = AW_MPI_VENC_CreateChn(mVeChn, &mVencChnAttr);
        if (SUCCESS == ret)
        {
            nSuccessFlag = TRUE;
            LOGD("create venc channel[%d] success!", mVeChn);
            break;
        }
        else if (ERR_VENC_EXIST == ret)
        {
            LOGD("venc channel[%d] is exist, find next!", mVeChn);
            mVeChn++;
        }
        else
        {
            LOGD("create venc channel[%d] ret[0x%x], find next!", mVeChn, ret);
            mVeChn++;
        }
    }

    if (nSuccessFlag == FALSE)
    {
        mVeChn = MM_INVALID_CHN;
        LOGE("fatal error! create venc channel fail!");
        return FAILURE;
    }
    else
    {
        VENC_FRAME_RATE_S stFrameRate;
        stFrameRate.SrcFrmRate = viParams->fps;
        stFrameRate.DstFrmRate = veParams->fps;

        LOGD("set venc framerate:%d", stFrameRate.DstFrmRate);
        AW_MPI_VENC_SetFrameRate(mVeChn, &stFrameRate);

#if( !VENC_spsppsPATCH )
        if(PT_H265 == mVencChnAttr.VeAttr.Type) {
            VENC_PARAM_H265_TIMING_S timing = { 0 };
            ret = AW_MPI_VENC_GetH265Timing(mVeChn, &timing);
            if( ret == SUCCESS ) {
                timing.timing_info_present_flag = 1;
                timing.num_units_in_tick = 1;
                timing.time_scale = veParams->fps << 1;
                timing.num_ticks_poc_diff_one = 1;

                ret = AW_MPI_VENC_SetH265Timing(mVeChn, &timing);
                if( ret == SUCCESS ) {
                    LOGD("h265 timing");
                    LOGD("timing_info_present_flag=%d", timing.timing_info_present_flag);
                    LOGD("num_units_in_tick=%d", timing.num_units_in_tick);
                    LOGD("time_scale=%d", timing.time_scale);
                    LOGD("num_ticks_poc_diff_one=%d", timing.num_ticks_poc_diff_one);
                }
            }
            else if ( ret == ERR_VENC_NOT_SUPPORT ) {
                LOGE("require h265 timing not support", ret);
            }
            else {
                LOGE("require h265 timing failed %x", ret);
            }
        }
        else if (PT_H264 == mVencChnAttr.VeAttr.Type) {
            VENC_PARAM_H264_VUI_S vui;
            memset(&vui, 0, sizeof(vui));

            ret = AW_MPI_VENC_GetH264Vui(mVeChn, &vui);
            if( ret == SUCCESS ) {
                VENC_PARAM_VUI_H264_TIME_INFO_S timing;

                timing.timing_info_present_flag = 1;
                timing.num_units_in_tick = 1;
                timing.time_scale = veParams->fps << 1;
                timing.fixed_frame_rate_flag = 1;

                vui.VuiTimeInfo = timing;
                ret = AW_MPI_VENC_SetH264Vui(mVeChn, &vui);
                if( ret == SUCCESS ) {
                    LOGD("h264 timing");
                    LOGD("timing_info_present_flag=%d", timing.timing_info_present_flag);
                    LOGD("num_units_in_tick=%d", timing.num_units_in_tick);
                    LOGD("time_scale=%d", timing.time_scale);
                    LOGD("fixed_frame_rate_flag=%d", timing.fixed_frame_rate_flag);
                }
            }
            else if ( ret == ERR_VENC_NOT_SUPPORT ) {
                LOGE("require h264 timing not support", ret);
            }
            else {
                LOGE("require h264 timing failed %x", ret);
            }
        }
#endif

        MPPCallbackInfo cbInfo;
        cbInfo.cookie = (void*)vv;
        cbInfo.callback = (MPPCallbackFuncType)&MPPCallbackWrapper;
        AW_MPI_VENC_RegisterCallback(mVeChn, &cbInfo);

        if ( ((PT_H264 == mVencChnAttr.VeAttr.Type) || (PT_H265 == mVencChnAttr.VeAttr.Type))
            && ((VENC_RC_MODE_H264QPMAP == mVencChnAttr.RcAttr.mRcMode) || (VENC_RC_MODE_H265QPMAP == mVencChnAttr.RcAttr.mRcMode))
           )
        {
            LOGE("fatal error! not support qpmap currently!");
            /*
            unsigned int width, heigth;
            int num;
            VENC_PARAM_H264_QPMAP_S QpMap;

            width = mVencChnAttr.VeAttr.AttrH264e.PicWidth;
            heigth = mVencChnAttr.VeAttr.AttrH264e.PicHeight;
            num = (ALIGN(width, 16) >> 4) * (ALIGN(heigth, 16) >> 4);

            QpMap.num = num;
            QpMap.p_info = (VENC_QPMAP_BLOCK_QP_INFO *)malloc(sizeof(VENC_QPMAP_BLOCK_QP_INFO) * num);
            if (QpMap.p_info == NULL)
            {
                LOGE("QPmap buffer malloc fail!!");
            }
            else
            {
                int i;
                for (i = 0; i < num / 2; i++)
                {
                    QpMap.p_info[i].mb_en = 1;
                    QpMap.p_info[i].mb_skip_flag = 0;
                    QpMap.p_info[i].mb_qp = 10;
                }
                for (; i < num; i++)
                {
                    QpMap.p_info[i].mb_en = 1;
                    QpMap.p_info[i].mb_skip_flag = 0;
                    QpMap.p_info[i].mb_qp = 42;
                }

                LOGD("set QPMAP");
                AW_MPI_VENC_SetH264QPMAP(mVeChn, (const VENC_PARAM_H264_QPMAP_S *)&QpMap);
                free(QpMap.p_info);
            }
            */
        }

        vv->veChn = mVeChn;
        memcpy(&vv->veParams, veParams, sizeof(VencParams_t));

        return SUCCESS;
    }
}

static ERRORTYPE vi2venc_createVipp(Vi2Venc_t* vv, ViParams_t* viParams)
{
    ERRORTYPE ret;
    VI_DEV  mViDev;
    ISP_DEV mIspDev;
    VI_ATTR_S mViAttr;

    //create vi channel
    mViDev = viParams->devNum;
    mIspDev = 0;

    vv->viDev = mViDev;
    vv->ispDev= mIspDev;
    memcpy(&vv->viParams, viParams, sizeof(ViParams_t));

    ret = AW_MPI_VI_CreateVipp(mViDev);
    if (ret != SUCCESS)
    {
        LOGE("fatal error! AW_MPI_VI CreateVipp failed: %x", ret);
        return ret;
    }

    memset(&mViAttr, 0, sizeof(VI_ATTR_S));
    mViAttr.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    mViAttr.memtype = V4L2_MEMORY_MMAP;
    //mViAttr.format.pixelformat = map_PIXEL_FORMAT_E_to_V4L2_PIX_FMT();
    mViAttr.format.pixelformat = V4L2_PIX_FMT_NV21M;
    mViAttr.format.field = V4L2_FIELD_NONE;
    //mViAttr.format.colorspace = V4L2_COLORSPACE_JPEG;
    mViAttr.format.width = viParams->width;
    mViAttr.format.height = viParams->height;
    mViAttr.nbufs = 7; //5
    LOGE("use %d v4l2 buffers!!!", mViAttr.nbufs);
    mViAttr.nplanes = 2;
    mViAttr.fps = viParams->fps;

    ret = AW_MPI_VI_SetVippAttr(mViDev, &mViAttr);
    if (ret != SUCCESS)
    {
        LOGE("fatal error! AW_MPI_VI SetVippAttr failed");
    }

    ret = AW_MPI_VI_EnableVipp(mViDev);
    if (ret != SUCCESS)
    {
        LOGE("fatal error! enableVipp fail!");
    }
    AW_MPI_ISP_Run(mIspDev);

    return ret;
}

static ERRORTYPE vi2venc_createViChn(Vi2Venc_t* vv)
{
    ERRORTYPE ret;
    VI_DEV  mViDev = vv->viDev;
    VI_CHN  mViChn = 0;
    BOOL nSuccessFlag = FALSE;

    mViChn = 0;
    while (mViChn < VIU_MAX_CHN_NUM)
    {
        ret = AW_MPI_VI_CreateVirChn(mViDev, mViChn, NULL);
        if (SUCCESS == ret)
        {
            nSuccessFlag = TRUE;
            LOGD("create vir channel[%d] success!", mViChn);
            break;
        }
        else if (ERR_VI_EXIST == ret)
        {
            LOGD("vir channel[%d] is exist, find next!", mViChn);
            mViChn++;
        }
        else
        {
            LOGD("create vir channel[%d] ret[0x%x], find next!", mViChn, ret);
            mViChn++;
        }
    }

    if (nSuccessFlag == FALSE)
    {
        mViChn = MM_INVALID_CHN;
        LOGE("fatal error! create vir channel fail!");
    }

    vv->viChn = mViChn;

    return ret;
}

static ERRORTYPE vi2venc_createVippChn(Vi2Venc_t* vv, ViParams_t* viParams)
{
    ERRORTYPE ret;
    VI_DEV  mViDev;
    ISP_DEV mIspDev;
    VI_CHN  mViChn;
    VI_ATTR_S mViAttr;

    //create vi channel
    mViDev = viParams->devNum;
    mIspDev = 0;
    mViChn = 0;

    ret = AW_MPI_VI_CreateVipp(mViDev);
    if (ret != SUCCESS)
    {
        LOGE("fatal error! AW_MPI_VI CreateVipp failed");
    }

    memset(&mViAttr, 0, sizeof(VI_ATTR_S));
    mViAttr.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    mViAttr.memtype = V4L2_MEMORY_MMAP;
    //mViAttr.format.pixelformat = map_PIXEL_FORMAT_E_to_V4L2_PIX_FMT();
    mViAttr.format.pixelformat = V4L2_PIX_FMT_NV21M;
    mViAttr.format.field = V4L2_FIELD_NONE;
    //mViAttr.format.colorspace = V4L2_COLORSPACE_JPEG;
    mViAttr.format.width = viParams->width;
    mViAttr.format.height = viParams->height;
    mViAttr.nbufs = 7; //5
    LOGD("use %d v4l2 buffers!!!", mViAttr.nbufs);
    mViAttr.nplanes = 2;
    mViAttr.fps = viParams->fps;

    ret = AW_MPI_VI_SetVippAttr(mViDev, &mViAttr);
    if (ret != SUCCESS)
    {
        LOGE("fatal error! AW_MPI_VI SetVippAttr failed");
    }

    ret = AW_MPI_VI_EnableVipp(mViDev);
    if (ret != SUCCESS)
    {
        LOGE("fatal error! enableVipp fail!");
    }
    AW_MPI_ISP_Run(mIspDev);

    ret = AW_MPI_VI_CreateVirChn(mViDev, mViChn, NULL);
    if (ret != SUCCESS)
    {
        LOGE("fatal error! createVirChn[%d] fail!", mViChn);
    }

    vv->viDev = mViDev;
    vv->viChn = mViChn;
    vv->ispDev= mIspDev;
    memcpy(&vv->viParams, viParams, sizeof(ViParams_t));

    return ret;
}

static ERRORTYPE vi2venc_configRoi(Vi2Venc_t* vv, VencParams_t* veParams)
{
#if(0)
    if (veParams->enableRoi)
    {
LOGD("------------------ROI begin-----------------------");
        VENC_ROI_CFG_S VencRoiCfg;

        VencRoiCfg.bEnable = TRUE;
        VencRoiCfg.Index = 0;
        VencRoiCfg.Qp = 10;
        VencRoiCfg.bAbsQp = 0;
        VencRoiCfg.Rect.X = 20;
        VencRoiCfg.Rect.Y = 0;
        VencRoiCfg.Rect.Width = 1280;
        VencRoiCfg.Rect.Height = 320;
        AW_MPI_VENC_SetRoiCfg(vv->veChn, &VencRoiCfg);

        VencRoiCfg.Index = 1;
        VencRoiCfg.Rect.X = 200;
        VencRoiCfg.Rect.Y = 600;
        VencRoiCfg.Rect.Width = 1000;
        VencRoiCfg.Rect.Height = 200;
        AW_MPI_VENC_SetRoiCfg(vv->veChn, &VencRoiCfg);

        VencRoiCfg.Index = 2;
        VencRoiCfg.Rect.X = 640;
        VencRoiCfg.Rect.Y = 384;
        VencRoiCfg.Rect.Width = 640;
        VencRoiCfg.Rect.Height = 360;
        VencRoiCfg.Qp = 40;
        VencRoiCfg.bAbsQp = 0;
        AW_MPI_VENC_SetRoiCfg(vv->veChn, &VencRoiCfg);

//        VencRoiCfg.Index = 4;
//        VencRoiCfg.Rect.X = 640;
//        VencRoiCfg.Rect.Y = 320;
//        VencRoiCfg.Rect.Width = 160;
//        VencRoiCfg.Rect.Height = 160;
//        AW_MPI_VENC_SetRoiCfg(vv->veChn, &VencRoiCfg);

        LOGD("------------------ROI end-----------------------");
    }
#endif

    return SUCCESS;
}

ERRORTYPE vi2venc_prepare(Vi2Venc_t* vv, ViParams_t* viParams, VencParams_t* veParams, VencParams_t* jpgParams)
{
    ERRORTYPE ret;

    ret = vi2venc_createVipp(vv, viParams);
    if (ret != SUCCESS)
    {
        LOGE("create vipp fail");
        //return ret;
    }

    ret = vi2venc_createViChn(vv);
    if (ret != SUCCESS)
    {
        LOGE("create vi chn fail");
        return ret;
    }

    ret = vi2venc_createVencChn(vv, viParams, veParams);
    if (ret != SUCCESS)
    {
        LOGE("create venc chn fail");
        return ret;
    }

    //config roi
    vi2venc_configRoi(vv, veParams);

    if ((vv->viDev >= 0 && vv->viChn >= 0) && vv->veChn >= 0)
    {
        MPP_CHN_S ViChn = {MOD_ID_VIU, vv->viDev, vv->viChn};
        MPP_CHN_S VeChn = {MOD_ID_VENC, 0, vv->veChn};

        ret = AW_MPI_SYS_Bind(&ViChn, &VeChn);
    }

    return ret;
}

ERRORTYPE vi2venc_start(Vi2Venc_t* vv, vi2venc_threadProc proc)
{
    ERRORTYPE ret = SUCCESS;

    LOGD("start");

    ret = AW_MPI_VI_EnableVirChn(vv->viDev, vv->viChn);
    if (ret != SUCCESS)
    {
        LOGE("VI enable error: %d", ret);
        return FAILURE;
    }

    if (vv->veChn >= 0)
    {
        ret = AW_MPI_VENC_StartRecvPic(vv->veChn);
        if( ret != SUCCESS ) {
            LOGE("start recv pic error: %d", ret);
            return FAILURE;
        }

        vv->bExit = false;
        if( proc == NULL ) {
            proc = vi2venc_frameProc;
        }
        ret = pthread_create(&vv->threadId, NULL, proc, (void *)vv);
        LOGD("venc pthread: dev[%d] chn[%d] veChn[%d]", vv->viDev, vv->viChn, vv->veChn);
    }

    return ret;
}

ERRORTYPE vi2venc_stop(Vi2Venc_t* vv, bool all)
{
    LOGD("stop");

    vv->bExit = true;
    if( vv->threadId > 0 ) {
        pthread_join(vv->threadId, NULL);
        vv->threadId = 0;
    }

    if (vv->viChn >= 0)
    {
        AW_MPI_VI_DisableVirChn(vv->viDev, vv->viChn);
    }

    if( vv->veChn >= 0 )
    {
        LOGD("stop venc");
        AW_MPI_VENC_StopRecvPic(vv->veChn);
    }

    if( vv->viChn >= 0 )
    {
        AW_MPI_VI_DestoryVirChn(vv->viDev, vv->viChn);
        vv->viChn = MM_INVALID_CHN;
    }

    if( vv->veChn >= 0 )
    {
        LOGD("destory venc");
        AW_MPI_VENC_ResetChn(vv->veChn);
        AW_MPI_VENC_DestroyChn(vv->veChn);
        vv->veChn = MM_INVALID_CHN;
    }

    if( all && (vv->viDev>=0) )
    {
        AW_MPI_VI_DisableVipp(vv->viDev);
        AW_MPI_VI_DestoryVipp(vv->viDev);
        AW_MPI_ISP_Stop(vv->ispDev);
        LOGD("destory vipp");
    }
    vv->viDev = MM_INVALID_DEV;
    vv->ispDev = MM_INVALID_DEV;

    return SUCCESS;
}

static int vi2venc_startVirvi(VI_DEV ViDev, VI_CHN ViCh, void *pAttr)
{
    int ret = -1;

    ret = AW_MPI_VI_CreateVirChn(ViDev, ViCh, pAttr);
    if(ret < 0)
    {
        LOGE("Create VI Chn failed,VIDev = %d,VIChn = %d",ViDev,ViCh);
        return ret ;
    }
    ret = AW_MPI_VI_SetVirChnAttr(ViDev, ViCh, pAttr);
    if(ret < 0)
    {
        LOGE("Set VI ChnAttr failed,VIDev = %d,VIChn = %d",ViDev,ViCh);
        return ret ;
    }
    ret = AW_MPI_VI_EnableVirChn(ViDev, ViCh);
    if(ret < 0)
    {
        LOGE("VI Enable VirChn failed,VIDev = %d,VIChn = %d",ViDev,ViCh);
        return ret ;
    }

    return 0;
}

static int vi2venc_stopVirvi(VI_DEV ViDev, VI_CHN ViCh)
{
    int ret = -1;
    ret = AW_MPI_VI_DisableVirChn(ViDev, ViCh);
    if(ret < 0)
    {
        LOGE("Disable VI Chn failed,VIDev = %d,VIChn = %d",ViDev,ViCh);
        return ret ;
    }
    ret = AW_MPI_VI_DestoryVirChn(ViDev, ViCh);
    if(ret < 0)
    {
        LOGE("Destory VI Chn failed,VIDev = %d,VIChn = %d",ViDev,ViCh);
        return ret ;
    }
    return 0;
}

//-------------------------------------------------------------
//-------------------------------------------------------------

ERRORTYPE vi2venc_setVencRateControlAttr(Vi2Venc_t* vv, VencRateControlAttr_t* RcAttr)
{
    ERRORTYPE ret = SUCCESS;
    VencRateControlAttr_t* mVEncRcAttr = &vv->veParams.rcAttr;
    VENC_CHN mVeChn = vv->veChn;

    //update bitRate when cbr.
    if(VENC_rcCBR == RcAttr->mRcMode)
    {
        if(PT_H264 == RcAttr->mVEncType)
        {
            if(mVEncRcAttr->mAttrH264Cbr.mBitRate != RcAttr->mAttrH264Cbr.mBitRate
                || mVEncRcAttr->mAttrH264Cbr.mMaxQp != RcAttr->mAttrH264Cbr.mMaxQp
                || mVEncRcAttr->mAttrH264Cbr.mMinQp != RcAttr->mAttrH264Cbr.mMinQp)
            {
                LOGD("need update h264 cbr bitRate[%d]->[%d], maxQp[%d]->[%d], minQp[%d]->[%d]",
                    mVEncRcAttr->mAttrH264Cbr.mBitRate, RcAttr->mAttrH264Cbr.mBitRate,
                    mVEncRcAttr->mAttrH264Cbr.mMaxQp, RcAttr->mAttrH264Cbr.mMaxQp,
                    mVEncRcAttr->mAttrH264Cbr.mMinQp, RcAttr->mAttrH264Cbr.mMinQp);
                mVEncRcAttr->mAttrH264Cbr.mBitRate = RcAttr->mAttrH264Cbr.mBitRate;
                mVEncRcAttr->mAttrH264Cbr.mMaxQp = RcAttr->mAttrH264Cbr.mMaxQp;
                mVEncRcAttr->mAttrH264Cbr.mMinQp = RcAttr->mAttrH264Cbr.mMinQp;
                VENC_CHN_ATTR_S attr;
                ret = AW_MPI_VENC_GetChnAttr(mVeChn, &attr);
                if(attr.RcAttr.mRcMode!=VENC_RC_MODE_H264CBR)
                {
                    LOGE("fatal error! check mpp_rcMode[0x%x]", attr.RcAttr.mRcMode);
                }
                attr.RcAttr.mAttrH264Cbr.mBitRate = RcAttr->mAttrH264Cbr.mBitRate;
                attr.RcAttr.mAttrH264Cbr.mMaxQp = RcAttr->mAttrH264Cbr.mMaxQp;
                attr.RcAttr.mAttrH264Cbr.mMinQp = RcAttr->mAttrH264Cbr.mMinQp;
                ret = AW_MPI_VENC_SetChnAttr(mVeChn, &attr);
            }
        }
        else if(PT_H265 == RcAttr->mVEncType)
        {
            if(mVEncRcAttr->mAttrH265Cbr.mBitRate != RcAttr->mAttrH265Cbr.mBitRate)
            {
                LOGD("need update h265 cbr bitRate[%d]->[%d]", mVEncRcAttr->mAttrH265Cbr.mBitRate, RcAttr->mAttrH265Cbr.mBitRate);
                mVEncRcAttr->mAttrH265Cbr.mBitRate = RcAttr->mAttrH265Cbr.mBitRate;
                VENC_CHN_ATTR_S attr;
                AW_MPI_VENC_GetChnAttr(mVeChn, &attr);
                if(attr.RcAttr.mRcMode!=VENC_RC_MODE_H265CBR)
                {
                    LOGE("fatal error! check mpp_rcMode[0x%x]", attr.RcAttr.mRcMode);
                }
                attr.RcAttr.mAttrH265Cbr.mBitRate = RcAttr->mAttrH265Cbr.mBitRate;
                ret = AW_MPI_VENC_SetChnAttr(mVeChn, &attr);
            }
        }
        else if(PT_MJPEG == RcAttr->mVEncType)
        {
            if(mVEncRcAttr->mAttrMjpegCbr.mBitRate != RcAttr->mAttrMjpegCbr.mBitRate)
            {
                LOGD("need update mjpeg cbr bitRate[%d]->[%d]", mVEncRcAttr->mAttrMjpegCbr.mBitRate, RcAttr->mAttrMjpegCbr.mBitRate);
                mVEncRcAttr->mAttrMjpegCbr.mBitRate = RcAttr->mAttrMjpegCbr.mBitRate;
                VENC_CHN_ATTR_S attr;
                AW_MPI_VENC_GetChnAttr(mVeChn, &attr);
                if(attr.RcAttr.mRcMode!=VENC_RC_MODE_MJPEGCBR)
                {
                    LOGE("fatal error! check mpp_rcMode[0x%x]", attr.RcAttr.mRcMode);
                }
                attr.RcAttr.mAttrMjpegeCbr.mBitRate = RcAttr->mAttrMjpegCbr.mBitRate;
                ret = AW_MPI_VENC_SetChnAttr(mVeChn, &attr);
            }
        }
        else
        {
            LOGE("fatal error! unsupport vencType[0x%x]", RcAttr->mVEncType);
        }
    }

    return ret;
}

ERRORTYPE vi2venc_setVideoFrameRate(Vi2Venc_t* vv, int srcRate, int rate)
{
    ERRORTYPE ret = SUCCESS;
    VENC_CHN mVeChn = vv->veChn;

    VENC_FRAME_RATE_S stFrameRate;
    stFrameRate.SrcFrmRate = srcRate;
    stFrameRate.DstFrmRate = rate;
    ret = AW_MPI_VENC_SetFrameRate(mVeChn, &stFrameRate);

    return ret;
}

ERRORTYPE vi2venc_setVideoEncodingIFramesNumberInterval(Vi2Venc_t* vv, int nMaxKeyItl)
{
    VENC_CHN_ATTR_S attr;
    VENC_CHN mVeChn = vv->veChn;

    AW_MPI_VENC_GetChnAttr(mVeChn, &attr);
    attr.VeAttr.MaxKeyInterval = nMaxKeyItl;
    return AW_MPI_VENC_SetChnAttr(mVeChn, &attr);
}

ERRORTYPE vi2venc_requestIFrame(Vi2Venc_t* vv)
{
    ERRORTYPE ret = SUCCESS;
    VENC_CHN mVeChn = vv->veChn;

    if(mVeChn >= 0)
    {
        ret = AW_MPI_VENC_RequestIDR(mVeChn, TRUE);
        LOGD("done");
    }

    return ret;
}

ERRORTYPE vi2venc_setVideoEncodingIntraRefresh(Vi2Venc_t* vv, VENC_PARAM_INTRA_REFRESH_S *pIntraRefresh)
{
    ERRORTYPE ret = SUCCESS;
    VENC_CHN mVeChn = vv->veChn;

    if(PT_H264 == vv->veParams.codecType || PT_H265 == vv->veParams.codecType)
    {
        ret = AW_MPI_VENC_SetIntraRefresh(mVeChn, pIntraRefresh);
    }
    else
    {
        LOGE("fatal error! encoder[0x%x] don't support IntraRefresh!", vv->veParams.codecType);
    }

    return ret;
}

ERRORTYPE vi2venc_setVideoEncodingSmartP(Vi2Venc_t* vv, VencSmartFun *pParam)
{
    ERRORTYPE ret = SUCCESS;
    VENC_CHN mVeChn = vv->veChn;

    if(PT_H264 == vv->veParams.codecType || PT_H265 == vv->veParams.codecType)
    {
        ret = AW_MPI_VENC_SetSmartP(mVeChn, pParam);
    }
    else
    {
        LOGE("fatal error! encoder[0x%x] don't support smartP!", vv->veParams.codecType);
    }

    return ret;
}

ERRORTYPE vi2venc_enableHorizonFlip(Vi2Venc_t* vv, bool enable)
{
    ERRORTYPE ret = SUCCESS;
    VENC_CHN mVeChn = vv->veChn;

    vv->veParams.enableHorizonfilp = enable;

    BOOL bHorizonFlipFlag;
    if(enable)
    {
        bHorizonFlipFlag = TRUE;
    }
    else
    {
        bHorizonFlipFlag = FALSE;
    }

    ret = AW_MPI_VENC_SetHorizonFlip(mVeChn, bHorizonFlipFlag);

    return ret;
}

 ERRORTYPE vi2venc_enable3DNR(Vi2Venc_t* vv, int nLevel)
 {
    ERRORTYPE ret = SUCCESS;
    VENC_CHN mVeChn = vv->veChn;

    vv->veParams.level3DNR = nLevel;

    ret = AW_MPI_VENC_Set3DNR(mVeChn, nLevel);
    return ret;
 }

ERRORTYPE vi2venc_setProcSet(Vi2Venc_t* vv, VeProcSet *pVeProcSet)
{
    ERRORTYPE ret = SUCCESS;
    VENC_CHN mVeChn = vv->veChn;

    //vv->veParams.procSet = *pVeProcSet;

    ret = AW_MPI_VENC_SetProcSet(mVeChn, pVeProcSet);
    return ret;
}

ERRORTYPE vi2venc_enableColor2Grey(Vi2Venc_t* vv, bool enable)
{
    ERRORTYPE ret = SUCCESS;
    VENC_CHN mVeChn = vv->veChn;

    vv->veParams.enableColor2Grey = enable;

    VENC_COLOR2GREY_S bColor2GreyFlag;
    if(enable)
    {
        bColor2GreyFlag.bColor2Grey = TRUE;
    }
    else
    {
        bColor2GreyFlag.bColor2Grey = FALSE;
    }
    ret = AW_MPI_VENC_SetColor2Grey(mVeChn, &bColor2GreyFlag);

    return ret;
}

ERRORTYPE vi2venc_enableAdaptiveIntraInp(Vi2Venc_t* vv, bool enable)
{
    ERRORTYPE ret = SUCCESS;
    VENC_CHN mVeChn = vv->veChn;

    vv->veParams.enableAdaptiveintrainp = enable;

    BOOL bAdaptiveIntraInpFlag;
    if(enable)
    {
        bAdaptiveIntraInpFlag = TRUE;
    }
    else
    {
        bAdaptiveIntraInpFlag = FALSE;
    }
    ret = AW_MPI_VENC_SetAdaptiveIntraInP(mVeChn, bAdaptiveIntraInpFlag);

    return ret;
}

ERRORTYPE vi2venc_setVencSuperFrameConfig(Vi2Venc_t* vv, VENC_SUPERFRAME_CFG_S *pSuperFrameConfig)
{
    ERRORTYPE ret = SUCCESS;
    VENC_CHN mVeChn = vv->veChn;

    //vv->veParams.mVencSuperFrameCfg = *pSuperFrameConfig;

    ret = AW_MPI_VENC_SetSuperFrameCfg(mVeChn, pSuperFrameConfig);

    return ret;
}

ERRORTYPE vi2venc_config_VENC_CHN_ATTR_S(Vi2Venc_t* vv, VENC_CHN_ATTR_S* veChnAttr)
{
    VENC_CHN_ATTR_S mVEncChnAttr;

    memset(veChnAttr, 0, sizeof(VENC_CHN_ATTR_S));
    memset(&mVEncChnAttr, 0, sizeof(VENC_CHN_ATTR_S));

    mVEncChnAttr.VeAttr.Type = vv->veParams.codecType;

    mVEncChnAttr.VeAttr.MaxKeyInterval = vv->veParams.maxKeyItl;
    SIZE_S frameBufSize = {
        .Width = vv->viParams.width,
        .Height= vv->viParams.height,
    };
    mVEncChnAttr.VeAttr.SrcPicWidth = frameBufSize.Width;
    mVEncChnAttr.VeAttr.SrcPicHeight = frameBufSize.Height;
    mVEncChnAttr.VeAttr.Field = VIDEO_FIELD_FRAME;
    mVEncChnAttr.VeAttr.PixelFormat = vv->veParams.picFormat;
    //mVEncChnAttr.VeAttr.mColorSpace = vv->veParams.colorSpace();

    if(PT_H264 == mVEncChnAttr.VeAttr.Type)
    {
        VencAttr_t mVEncAttr = {
            .mType = PT_H264,
            .mBufSize = 0,
            .mThreshSize = 0,
            {
                .mAttrH264 = {
                    .mProfile = 1,
                    .mLevel = H264_LEVEL_51,
                },
            },
        };

        mVEncChnAttr.VeAttr.AttrH264e.BufSize = mVEncAttr.mBufSize;
        mVEncChnAttr.VeAttr.AttrH264e.mThreshSize = mVEncAttr.mThreshSize;
        mVEncChnAttr.VeAttr.AttrH264e.Profile = mVEncAttr.mAttrH264.mProfile;
        mVEncChnAttr.VeAttr.AttrH264e.mLevel = mVEncAttr.mAttrH264.mLevel;
        mVEncChnAttr.VeAttr.AttrH264e.bByFrame = TRUE;
        mVEncChnAttr.VeAttr.AttrH264e.PicWidth = vv->veParams.width;
        mVEncChnAttr.VeAttr.AttrH264e.PicHeight = vv->veParams.height;
        mVEncChnAttr.VeAttr.AttrH264e.IQpOffset = 0;//vv->veParams.mIQpOffset;
       // mVEncChnAttr.VeAttr.AttrH264e.mbLongTermRef = vv->veParams.mbLongTermRef;
        mVEncChnAttr.VeAttr.AttrH264e.FastEncFlag = vv->veParams.fastEnc;
       // mVEncChnAttr.VeAttr.AttrH264e.mVirtualIFrameInterval = vv->veParams.mVirtualIFrameInterval;
        mVEncChnAttr.VeAttr.AttrH264e.mbPIntraEnable = TRUE;//vv->veParams.enablePIntra;
        LOGD("config venc bufSize[%d], threshSize[%d]", mVEncChnAttr.VeAttr.AttrH264e.BufSize, mVEncChnAttr.VeAttr.AttrH264e.mThreshSize);
    }
    else if(PT_H265 == mVEncChnAttr.VeAttr.Type)
    {
        VencAttr_t mVEncAttr = {
            .mType = PT_H265,
            .mBufSize = 0,
            .mThreshSize = 0,
            {
                .mAttrH265 = {
                    .mProfile = 1,
                    .mLevel = H265_LEVEL_51,
                },
            },
        };

        mVEncChnAttr.VeAttr.AttrH265e.mBufSize = mVEncAttr.mBufSize;
        mVEncChnAttr.VeAttr.AttrH265e.mThreshSize = mVEncAttr.mThreshSize;
        mVEncChnAttr.VeAttr.AttrH265e.mProfile = mVEncAttr.mAttrH265.mProfile;
        mVEncChnAttr.VeAttr.AttrH265e.mLevel = mVEncAttr.mAttrH265.mLevel;
        mVEncChnAttr.VeAttr.AttrH265e.mbByFrame = TRUE;
        mVEncChnAttr.VeAttr.AttrH265e.mPicWidth = vv->veParams.width;
        mVEncChnAttr.VeAttr.AttrH265e.mPicHeight = vv->veParams.height;
        mVEncChnAttr.VeAttr.AttrH265e.IQpOffset = 0;//vv->veParams.mIQpOffset;
        //mVEncChnAttr.VeAttr.AttrH265e.mbLongTermRef = vv->veParams.mbLongTermRef;
        mVEncChnAttr.VeAttr.AttrH265e.mFastEncFlag = vv->veParams.fastEnc;
        //mVEncChnAttr.VeAttr.AttrH265e.mVirtualIFrameInterval = vv->veParams.mVirtualIFrameInterval;
        mVEncChnAttr.VeAttr.AttrH265e.mbPIntraEnable = TRUE;//vv->veParams.enablePIntra;
    }
    else if(PT_MJPEG == mVEncChnAttr.VeAttr.Type)
    {
        mVEncChnAttr.VeAttr.AttrMjpeg.mBufSize = 0;
        mVEncChnAttr.VeAttr.AttrMjpeg.mbByFrame = TRUE;
        mVEncChnAttr.VeAttr.AttrMjpeg.mPicWidth = vv->veParams.width;
        mVEncChnAttr.VeAttr.AttrMjpeg.mPicHeight = vv->veParams.height;
    }
    else
    {
        LOGE("fatal error! unsupported temporary");
    }
    if(PT_H264 == mVEncChnAttr.VeAttr.Type)
    {
        switch(vv->veParams.rcMode)
        {
            case VENC_rcCBR:
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H264CBR;
                break;
            case VENC_rcVBR:
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H264VBR;
                break;
            case VENC_rcFIXQP:
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H264FIXQP;
                break;
            case VENC_rcABR:
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H264ABR;
                break;
            case VENC_rcQPMAP:
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H264QPMAP;
                break;
            default:
                LOGE("fatal error! unknown rcMode[%d]", vv->veParams.rcMode);
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H264CBR;
                break;
        }
        if(VENC_RC_MODE_H264CBR == mVEncChnAttr.RcAttr.mRcMode)
        {
            mVEncChnAttr.RcAttr.mAttrH264Cbr.mBitRate = vv->veParams.rcAttr.mAttrH264Cbr.mBitRate;
            mVEncChnAttr.RcAttr.mAttrH264Cbr.mMaxQp = vv->veParams.rcAttr.mAttrH264Cbr.mMaxQp;
            mVEncChnAttr.RcAttr.mAttrH264Cbr.mMinQp = vv->veParams.rcAttr.mAttrH264Cbr.mMinQp;
        }
        else if(VENC_RC_MODE_H264VBR == mVEncChnAttr.RcAttr.mRcMode)
        {
            mVEncChnAttr.RcAttr.mAttrH264Vbr.mMaxBitRate = vv->veParams.rcAttr.mAttrH264Vbr.mMaxBitRate;
            mVEncChnAttr.RcAttr.mAttrH264Vbr.mMaxQp = vv->veParams.rcAttr.mAttrH264Vbr.mMaxQp;
            mVEncChnAttr.RcAttr.mAttrH264Vbr.mMinQp = vv->veParams.rcAttr.mAttrH264Vbr.mMinQp;
            mVEncChnAttr.RcAttr.mAttrH264Vbr.mRatioChangeQp = vv->veParams.rcAttr.mAttrH264Vbr.mRatioChangeQp;
            mVEncChnAttr.RcAttr.mAttrH264Vbr.mQuality = vv->veParams.rcAttr.mAttrH264Vbr.mQuality;
        }
        else if(VENC_RC_MODE_H264FIXQP == mVEncChnAttr.RcAttr.mRcMode)
        {
            mVEncChnAttr.RcAttr.mAttrH264FixQp.mIQp = vv->veParams.rcAttr.mAttrH264FixQp.mIQp;
            mVEncChnAttr.RcAttr.mAttrH264FixQp.mPQp = vv->veParams.rcAttr.mAttrH264FixQp.mPQp;
        }
        else if(VENC_RC_MODE_H264ABR == mVEncChnAttr.RcAttr.mRcMode)
        {
            mVEncChnAttr.RcAttr.mAttrH264Abr.mMaxBitRate = vv->veParams.rcAttr.mAttrH264Abr.mMaxBitRate;
            mVEncChnAttr.RcAttr.mAttrH264Abr.mRatioChangeQp = vv->veParams.rcAttr.mAttrH264Abr.mRatioChangeQp;
            mVEncChnAttr.RcAttr.mAttrH264Abr.mQuality = vv->veParams.rcAttr.mAttrH264Abr.mQuality;
            mVEncChnAttr.RcAttr.mAttrH264Abr.mMinIQp = vv->veParams.rcAttr.mAttrH264Abr.mMinIQp;
            mVEncChnAttr.RcAttr.mAttrH264Abr.mMaxIQp = vv->veParams.rcAttr.mAttrH264Abr.mMaxIQp;
            mVEncChnAttr.RcAttr.mAttrH264Abr.mMaxQp = vv->veParams.rcAttr.mAttrH264Abr.mMaxQp;
            mVEncChnAttr.RcAttr.mAttrH264Abr.mMinQp = vv->veParams.rcAttr.mAttrH264Abr.mMinQp;
        }
        else if(VENC_RC_MODE_H264QPMAP == mVEncChnAttr.RcAttr.mRcMode)
        {
            LOGE("QPMap not support now!");
        }
        else
        {
            LOGE("fatal error! unknown rcMode[%d]", mVEncChnAttr.RcAttr.mRcMode);
        }
    }
    else if(PT_H265 == mVEncChnAttr.VeAttr.Type)
    {
        switch(vv->veParams.rcMode)
        {
            case VENC_rcCBR:
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H265CBR;
                break;
            case VENC_rcVBR:
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H265VBR;
                break;
            case VENC_rcFIXQP:
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H265FIXQP;
                break;
            case VENC_rcABR:
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H265ABR;
                break;
            case VENC_rcQPMAP:
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H265QPMAP;
                break;
            default:
                LOGE("fatal error! unknown rcMode[%d]", vv->veParams.rcMode);
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H265CBR;
                break;
        }
        if(VENC_RC_MODE_H265CBR == mVEncChnAttr.RcAttr.mRcMode)
        {
            mVEncChnAttr.RcAttr.mAttrH265Cbr.mBitRate = vv->veParams.rcAttr.mAttrH265Cbr.mBitRate;
            mVEncChnAttr.RcAttr.mAttrH265Cbr.mMaxQp = vv->veParams.rcAttr.mAttrH265Cbr.mMaxQp;
            mVEncChnAttr.RcAttr.mAttrH265Cbr.mMinQp = vv->veParams.rcAttr.mAttrH265Cbr.mMinQp;
        }
        else if(VENC_RC_MODE_H265VBR == mVEncChnAttr.RcAttr.mRcMode)
        {
            mVEncChnAttr.RcAttr.mAttrH265Vbr.mMaxBitRate = vv->veParams.rcAttr.mAttrH265Vbr.mMaxBitRate;
            mVEncChnAttr.RcAttr.mAttrH265Vbr.mMaxQp = vv->veParams.rcAttr.mAttrH265Vbr.mMaxQp;
            mVEncChnAttr.RcAttr.mAttrH265Vbr.mMinQp = vv->veParams.rcAttr.mAttrH265Vbr.mMinQp;
            mVEncChnAttr.RcAttr.mAttrH265Vbr.mRatioChangeQp = vv->veParams.rcAttr.mAttrH265Vbr.mRatioChangeQp;
            mVEncChnAttr.RcAttr.mAttrH265Vbr.mQuality = vv->veParams.rcAttr.mAttrH265Vbr.mQuality;
        }
        else if(VENC_RC_MODE_H265FIXQP == mVEncChnAttr.RcAttr.mRcMode)
        {
            mVEncChnAttr.RcAttr.mAttrH265FixQp.mIQp = vv->veParams.rcAttr.mAttrH265FixQp.mIQp;
            mVEncChnAttr.RcAttr.mAttrH265FixQp.mPQp = vv->veParams.rcAttr.mAttrH265FixQp.mPQp;
        }
        else if(VENC_RC_MODE_H265ABR == mVEncChnAttr.RcAttr.mRcMode)
        {
            mVEncChnAttr.RcAttr.mAttrH265Abr.mMaxBitRate = vv->veParams.rcAttr.mAttrH265Abr.mMaxBitRate;
            mVEncChnAttr.RcAttr.mAttrH265Abr.mRatioChangeQp = vv->veParams.rcAttr.mAttrH265Abr.mRatioChangeQp;
            mVEncChnAttr.RcAttr.mAttrH265Abr.mQuality = vv->veParams.rcAttr.mAttrH265Abr.mQuality;
            mVEncChnAttr.RcAttr.mAttrH265Abr.mMinIQp = vv->veParams.rcAttr.mAttrH265Abr.mMinIQp;
            mVEncChnAttr.RcAttr.mAttrH265Abr.mMaxIQp = vv->veParams.rcAttr.mAttrH265Abr.mMaxIQp;
            mVEncChnAttr.RcAttr.mAttrH265Abr.mMaxQp = vv->veParams.rcAttr.mAttrH265Abr.mMaxQp;
            mVEncChnAttr.RcAttr.mAttrH265Abr.mMinQp = vv->veParams.rcAttr.mAttrH265Abr.mMinQp;
        }
        else if(VENC_RC_MODE_H265QPMAP == mVEncChnAttr.RcAttr.mRcMode)
        {
            LOGE("QPMap not support now!");
        }
        else
        {
            LOGE("fatal error! unknown rcMode[%d]", mVEncChnAttr.RcAttr.mRcMode);
        }
    }
    else if(PT_MJPEG == mVEncChnAttr.VeAttr.Type)
    {
        switch(vv->veParams.rcMode)
        {
            case VENC_rcCBR:
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_MJPEGCBR;
                break;
            case VENC_rcFIXQP:
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_MJPEGFIXQP;
                break;
            default:
                LOGE("fatal error! unknown rcMode[%d]", vv->veParams.rcMode);
                mVEncChnAttr.RcAttr.mRcMode = VENC_RC_MODE_MJPEGCBR;
                break;
        }
        if(VENC_RC_MODE_MJPEGCBR == mVEncChnAttr.RcAttr.mRcMode)
        {
            mVEncChnAttr.RcAttr.mAttrMjpegeCbr.mBitRate = vv->veParams.rcAttr.mAttrMjpegCbr.mBitRate;
        }
        else if(VENC_RC_MODE_MJPEGFIXQP == mVEncChnAttr.RcAttr.mRcMode)
        {
            mVEncChnAttr.RcAttr.mAttrMjpegeFixQp.mQfactor = vv->veParams.rcAttr.mAttrMjpegFixQp.mQfactor;
        }
        else
        {
            LOGE("fatal error! unknown rcMode[%d]", mVEncChnAttr.RcAttr.mRcMode);
        }
    }
    else
    {
        LOGE("fatal error! unsupported temporary");
    }

    return SUCCESS;
}


ERRORTYPE vi2venc_prepare2(Vi2Venc_t* vv, ViParams_t* viParams, VencParams_t* veParams)
{
    //create venc channel.
    bool nSuccessFlag = false;
    ERRORTYPE ret;
    VENC_CHN_ATTR_S mVEncChnAttr;

    vi2venc_config_VENC_CHN_ATTR_S(vv, &mVEncChnAttr);

    VENC_CHN mVeChn = 0;
    while(mVeChn < VENC_MAX_CHN_NUM)
    {
        ret = AW_MPI_VENC_CreateChn(mVeChn, &mVEncChnAttr);
        if(SUCCESS == ret)
        {
            nSuccessFlag = true;
            LOGD("create venc channel[%d] success!", mVeChn);
            break;
        }
        else if(ERR_VENC_EXIST == ret)
        {
            LOGD("venc channel[%d] is exist, find next!", mVeChn);
            mVeChn++;
        }
        else
        {
            LOGD("create venc channel[%d] ret[0x%x], find next!", mVeChn, ret);
            mVeChn++;
        }
    }
    if(false == nSuccessFlag)
    {
        mVeChn = MM_INVALID_CHN;
        LOGE("fatal error! create venc channel fail!");
        goto _err0;
    }

    MPPCallbackInfo cbInfo;
    cbInfo.cookie = (void*)vv;
    cbInfo.callback = (MPPCallbackFuncType)&MPPCallbackWrapper;
    AW_MPI_VENC_RegisterCallback(mVeChn, &cbInfo);

    VENC_FRAME_RATE_S stFrameRate;
    stFrameRate.SrcFrmRate = vv->viParams.fps;
    stFrameRate.DstFrmRate = vv->veParams.fps;
    AW_MPI_VENC_SetFrameRate(mVeChn, &stFrameRate);
    if(veParams->timeLapseEnable)
    {
        AW_MPI_VENC_SetTimeLapse(mVeChn, veParams->timeBetweenFrameCapture);
    }
    //IntraRefresh
    VENC_PARAM_INTRA_REFRESH_S stIntraRefresh = {FALSE, FALSE, 0, 0};
    if(veParams->intraRefreshBlockNum > 0)
    {
        VENC_PARAM_INTRA_REFRESH_S newIntraRefresh = {TRUE, FALSE, (unsigned int)veParams->intraRefreshBlockNum, 0};
        stIntraRefresh = newIntraRefresh;
    }
    AW_MPI_VENC_SetIntraRefresh(mVeChn, &stIntraRefresh);
    //smartP
    if(PT_H264 == veParams->codecType || PT_H265 == veParams->codecType)
    {
        VencSmartFun smartParam;
        memset(&smartParam, 0, sizeof(VencSmartFun));

        if( veParams->enableSmartP )
        {
            smartParam.smart_fun_en = 1;
            smartParam.img_bin_en = 1;
            smartParam.img_bin_th = 0;
            smartParam.shift_bits = 2;
            LOGD("setSmartP shift_bits=[%d]", smartParam.shift_bits);
        }
        AW_MPI_VENC_SetSmartP(mVeChn, &smartParam);
    }

    //AW_MPI_VENC_SetVEFreq(mVeChn, 534);

    VENC_PARAM_REF_S RefParam;
    memset(&RefParam, 0, sizeof(RefParam));
#if(0)
    if(veParams->enableAdvanceRef)
    {
        RefParam.Base = veParam->nBase;
        RefParam.Enhance = veParam->nEnhance;
        RefParam.bEnablePred = veParam->bRefBaseEn;
    }
#endif
    AW_MPI_VENC_SetRefParam(mVeChn, &RefParam);

    AW_MPI_VENC_SetHorizonFlip(mVeChn, veParams->enableHorizonfilp);
    AW_MPI_VENC_SetAdaptiveIntraInP(mVeChn, veParams->enableAdaptiveintrainp);
    AW_MPI_VENC_Set3DNR(mVeChn, veParams->level3DNR);

    VENC_SUPERFRAME_CFG_S stSuperFrameConfig = {
#if(1)
        .enSuperFrmMode = SUPERFRM_NONE,
        .SuperIFrmBitsThr = 0,
        .SuperPFrmBitsThr = 0,
        .SuperBFrmBitsThr = 0,
#else
        .enSuperFrmMode = veParams->mSuperFrmMode,
        .SuperIFrmBitsThr = veParams->mSuperIFrmBitsThr,
        .SuperPFrmBitsThr = veParams->mSuperPFrmBitsThr,
        .SuperBFrmBitsThr = 0,
#endif
    };
    AW_MPI_VENC_SetSuperFrameCfg(mVeChn, &stSuperFrameConfig);

    VencSaveBSFile mSaveBSFileParam;
    memset(&mSaveBSFileParam, 0, sizeof(mSaveBSFileParam));
    AW_MPI_VENC_SaveBsFile(mVeChn, &mSaveBSFileParam);

    VeProcSet mVeProcSet;
    memset(&mVeProcSet, 0, sizeof(mVeProcSet));
#if(0)
    mVeProcSet.bProcEnable = 1;
    mVeProcSet.nProcFreq = 30;
    mVeProcSet.nStatisBitRateTime = 1000;
    mVeProcSet.nStatisFrRateTime = 1000;
#endif
    AW_MPI_VENC_SetProcSet(mVeChn, &mVeProcSet);

    VENC_COLOR2GREY_S  tColor2Grey;
    tColor2Grey.bColor2Grey = veParams->enableColor2Grey;
    AW_MPI_VENC_SetColor2Grey(mVeChn, &tColor2Grey);

    if(veParams->enableNullSkip)
    {
        AW_MPI_VENC_EnableNullSkip(mVeChn, (BOOL)veParams->enableNullSkip);
    }
    if(veParams->enablePSkip)
    {
        AW_MPI_VENC_EnablePSkip(mVeChn, (BOOL)veParams->enablePSkip);
    }

    vv->startTimestamp = -1;

#if(0)
    if (!veParams->timeLapseEnable && vv->aeParams.codecType != PT_MAX)
    {
        AUDIO_DEV mAiDev = 0;
        config_AIO_ATTR_S();
        AW_MPI_AI_SetPubAttr(mAiDev, &mAioAttr);

        //enable audio_hw_ai
        AW_MPI_AI_Enable(mAiDev);

        //create ai channel.
        ERRORTYPE ret;
        BOOL nSuccessFlag = FALSE;
        mAiChn = 0;
        while (mAiChn < AIO_MAX_CHN_NUM) {
            ret = AW_MPI_AI_CreateChn(mAiDev, mAiChn);
            if (SUCCESS == ret)
            {
                nSuccessFlag = TRUE;
                LOGD("create ai channel[%d] success!", mAiChn);
                break;
            }
            else if (ERR_AI_EXIST == ret)
            {
                alogv("ai channel[%d] exist, find next!", mAiChn);
                mAiChn++;
            }
            else if (ERR_AI_NOT_ENABLED == ret)
            {
                LOGE("audio_hw_ai not started!");
                break;
            }
            else
            {
                LOGE("create ai channel[%d] fail! ret[0x%x]!", mAiChn, ret);
                break;
            }
        }
        if(FALSE == nSuccessFlag)
        {
            mAiChn = MM_INVALID_CHN;
            LOGE("fatal error! create ai channel fail!");
            result = UNKNOWN_ERROR;
            goto _err0;
        }
        MPPCallbackInfo cbInfo;
        cbInfo.cookie = (void*)this;
        cbInfo.callback = (MPPCallbackFuncType)&MPPCallbackWrapper;
        AW_MPI_AI_RegisterCallback(mAiDev, mAiChn, &cbInfo);
        AW_MPI_AI_SetChnMute(mAiDev, mAiChn, mMuteMode?TRUE:FALSE);

        //create aenc channel.
        nSuccessFlag = FALSE;
        config_AENC_CHN_ATTR_S();
        mAeChn = 0;
        while(mAeChn < AENC_MAX_CHN_NUM)
        {
            ret = AW_MPI_AENC_CreateChn(mAeChn, &mAEncChnAttr);
            if(SUCCESS == ret)
            {
                nSuccessFlag = TRUE;
                LOGD("create aenc channel[%d] success!", mAeChn);
                break;
            }
            else if(ERR_AENC_EXIST == ret)
            {
                alogv("aenc channel[%d] exist, find next!", mAeChn);
                mAeChn++;
            }
            else
            {
                LOGD("create aenc channel[%d] ret[0x%x], find next!", mAeChn, ret);
                mAeChn++;
            }
        }
        if(FALSE == nSuccessFlag)
        {
            mAeChn = MM_INVALID_CHN;
            LOGE("fatal error! create aenc channel fail!");
            result = UNKNOWN_ERROR;
            goto _err0;
        }
        AW_MPI_AENC_RegisterCallback(mAeChn, &cbInfo);
    }
#endif

    return ret;
_err0:
    return ret;
}

ERRORTYPE vi2venc_start2(Vi2Venc_t* vv)
{
    ERRORTYPE ret = SUCCESS;
    VENC_CHN mVeChn = vv->veChn;

    if(mVeChn >= 0)
    {
        ret = AW_MPI_VENC_StartRecvPic(mVeChn);
        if(SUCCESS != ret)
        {
            LOGE("fatal error:%x rec AW_MPI_VENC_StartRecvPic",ret);
        }
    }

    return ret;
}

ERRORTYPE vi2venc_stop2(Vi2Venc_t* vv)
{
    ERRORTYPE ret = SUCCESS;
    VENC_CHN mVeChn = vv->veChn;

    if(mVeChn >= 0)
    {
        ret = AW_MPI_VENC_StopRecvPic(mVeChn);
    }
    if(mVeChn >= 0)
    {
        ret = AW_MPI_VENC_ResetChn(mVeChn);
        ret = AW_MPI_VENC_DestroyChn(mVeChn);
        mVeChn = MM_INVALID_CHN;
    }
    vv->veChn = mVeChn;

    return ret;
}

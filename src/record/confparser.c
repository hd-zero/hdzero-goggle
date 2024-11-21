//#define LOG_NDEBUG 0
#define LOG_TAG "CfgParser"
#include <log/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "confparser.h"
#include "minIni.h"

#define SEC_RECORD      "record"
#define SEC_VENC        "venc"
#define SEC_VI          "vi"
#define SEC_AENC        "aenc"
#define SEC_AI          "ai"
#define SEC_H264        "h264"
#define SEC_H265        "h265"
#define SEC_H264_LIVE   "h264_live"
#define SEC_H265_LIVE   "h265_live"
#define SEC_VENC_LIVE   "venc_live"

#define KEY_DISK        "disk"
#define KEY_DURATION    "duration"
#define KEY_TYPE        "type"
#define KEY_SIZE        "size"
#define KEY_FULL        "full"
#define KEY_AUDIO       "audio"
#define KEY_NAMING      "naming"

#define KEY_WIDTH       "width"
#define KEY_HEIGHT      "height"
#define KEY_FPS         "fps"
#define KEY_BPS         "bps"
#define KEY_KBPS        "kbps"
#define KEY_H265        "h265"
#define KEY_RC          "rc"
#define KEY_GOP         "gop"
#define KEY_minQP       "minQP"
#define KEY_maxQP       "maxQP"
#define KEY_ratioChangeQP "ratioChangeQP"
#define KEY_QUALITY     "quality"
#define KEY_IQP         "iQP"
#define KEY_PQP         "pQP"
#define KEY_minIQP      "minIQP"
#define KEY_PROFILE     "profile"
#define KEY_LEVEL       "level"

#define KEY_deviceNO    "device"
#define KEY_sampleRATE  "sample_rate"
#define KEY_sampleBITS  "sample_bits"
#define KEY_CHANNELS    "channels"
#define KEY_AAC         "aac"

#define sizearray(a)  (sizeof(a) / sizeof((a)[0]))
#define check_set(v, min, max)  ( ((v)<(min)) ? (min) : (((v)>(max)) ? (max) : (v)) )

void conf_loadViParams(char* confFile, ViParams_t* para)
{
    long lValue = 0;

    lValue = ini_getl(SEC_VI, KEY_WIDTH, VI_WIDTH, confFile);
    para->width = check_set(lValue, 640, 3840);

    lValue = ini_getl(SEC_VI, KEY_HEIGHT, VI_HEIGHT, confFile);
    para->height= check_set(lValue, 480, 2160);

    lValue = ini_getl(SEC_VI, KEY_FPS, VI_FPS, confFile);
    para->fps= check_set(lValue, 24, 90);
}

void conf_saveViParams(char* confFile, ViParams_t* para)
{
    ini_putl(SEC_VI, KEY_WIDTH, para->width, confFile);
    ini_putl(SEC_VI, KEY_HEIGHT, para->height, confFile);
    ini_putl(SEC_VI, KEY_FPS, para->fps, confFile);
}

void conf_loadVencParams(char* confFile, VencParams_t* para)
{
    long lValue = 0;
    bool b_h265 = false;

    lValue = ini_getl(SEC_VENC, KEY_WIDTH, VE_WIDTH, confFile);
    para->width = check_set(lValue, 640, 3840);

    lValue = ini_getl(SEC_VENC, KEY_HEIGHT, VE_HEIGHT, confFile);
    para->height= check_set(lValue, 480, 2160);

    lValue = ini_getl(SEC_VENC, KEY_FPS, VE_FPS, confFile);
    para->fps= check_set(lValue, 24, 90);

    lValue = ini_getl(SEC_VENC, KEY_KBPS, VE_KBPS, confFile);
    lValue = check_set(lValue, 32, 1024000);
    para->bps= lValue * 1024;   //kbps to bps

    lValue = ini_getbool(SEC_VENC, KEY_H265, (VE_ENCODER==PT_H265), confFile);
    para->codecType = lValue ? PT_H265 : PT_H264;
    b_h265 = (para->codecType == PT_H265);

    lValue = ini_getl(SEC_VENC, KEY_RC, VE_RC, confFile);
    para->rcMode = check_set(lValue, VENC_rcCBR, VENC_rcABR);

    lValue = ini_getl(SEC_VENC, KEY_GOP, VE_GOP, confFile);
    para->maxKeyItl = check_set(lValue, 0, 300);

    if( b_h265 ) {
        char* sSection = SEC_H265;

        lValue = ini_getl(sSection, KEY_PROFILE, VE_PROFILE, confFile);
        para->veAttr.mAttrH265.mProfile = check_set(lValue, VENC_profileBASELINE, VENC_profileMP);

        lValue = ini_getl(sSection, KEY_LEVEL, VE_LEVEL, confFile);
        para->veAttr.mAttrH265.mLevel = check_set(lValue, H265_LEVEL_1, H265_LEVEL_62);

        if( para->rcMode == VENC_rcCBR ) {
            lValue = ini_getl(sSection, KEY_minQP, VE_minQP, confFile);
            para->rcAttr.mAttrH265Cbr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_maxQP, VE_maxQP, confFile);
            para->rcAttr.mAttrH265Cbr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
            para->rcAttr.mAttrH265Cbr.mBitRate = para->bps;
        }
        else if( para->rcMode == VENC_rcVBR ) {
            lValue = ini_getl(sSection, KEY_minQP, VE_minQP, confFile);
            para->rcAttr.mAttrH265Vbr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_maxQP, VE_maxQP, confFile);
            para->rcAttr.mAttrH265Vbr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_ratioChangeQP, 0, confFile);
            para->rcAttr.mAttrH265Vbr.mRatioChangeQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_QUALITY, 0, confFile);
            para->rcAttr.mAttrH265Vbr.mQuality = check_set(lValue, 0, 13);
            para->rcAttr.mAttrH265Vbr.mMaxBitRate = para->bps;
        }
        else if( para->rcMode == VENC_rcABR ) {
            lValue = ini_getl(sSection, KEY_minQP, VE_minQP, confFile);
            para->rcAttr.mAttrH265Abr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_maxQP, VE_maxQP, confFile);
            para->rcAttr.mAttrH265Abr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_ratioChangeQP, 0, confFile);
            para->rcAttr.mAttrH265Abr.mRatioChangeQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_QUALITY, 0, confFile);
            para->rcAttr.mAttrH265Abr.mQuality = check_set(lValue, 0, 13);
            lValue = ini_getl(sSection, KEY_minIQP, 0, confFile);
            para->rcAttr.mAttrH265Abr.mMinIQp = check_set(lValue, 0, VE_qpMAX);
            para->rcAttr.mAttrH265Abr.mMaxBitRate = para->bps;
        }
        else if( para->rcMode == VENC_rcFIXQP ) {
            lValue = ini_getl(sSection, KEY_IQP, VE_iQP, confFile);
            para->rcAttr.mAttrH265Abr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_PQP, VE_pQP, confFile);
            para->rcAttr.mAttrH265Abr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
        }
    }
    else {
        char* sSection = SEC_H264;

        lValue = ini_getl(sSection, KEY_PROFILE, VE_PROFILE, confFile);
        para->veAttr.mAttrH264.mProfile = check_set(lValue, VENC_profileBASELINE, VENC_profileSVC);

        lValue = ini_getl(sSection, KEY_LEVEL, VE_LEVEL, confFile);
        para->veAttr.mAttrH264.mLevel = check_set(lValue, H264_LEVEL_1, H264_LEVEL_51);

        if( para->rcMode == VENC_rcCBR ) {
            lValue = ini_getl(sSection, KEY_minQP, VE_minQP, confFile);
            para->rcAttr.mAttrH264Cbr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_maxQP, VE_maxQP, confFile);
            para->rcAttr.mAttrH264Cbr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
            para->rcAttr.mAttrH264Cbr.mBitRate = para->bps;
        }
        else if( para->rcMode == VENC_rcVBR ) {
            lValue = ini_getl(sSection, KEY_minQP, VE_minQP, confFile);
            para->rcAttr.mAttrH264Vbr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_maxQP, VE_maxQP, confFile);
            para->rcAttr.mAttrH264Vbr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_ratioChangeQP, 0, confFile);
            para->rcAttr.mAttrH264Vbr.mRatioChangeQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_QUALITY, 0, confFile);
            para->rcAttr.mAttrH264Vbr.mQuality = check_set(lValue, 0, 13);
            para->rcAttr.mAttrH264Vbr.mMaxBitRate = para->bps;
        }
        else if( para->rcMode == VENC_rcABR ) {
            lValue = ini_getl(sSection, KEY_minQP, VE_minQP, confFile);
            para->rcAttr.mAttrH264Abr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_maxQP, VE_maxQP, confFile);
            para->rcAttr.mAttrH264Abr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_ratioChangeQP, 0, confFile);
            para->rcAttr.mAttrH264Abr.mRatioChangeQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_QUALITY, 0, confFile);
            para->rcAttr.mAttrH264Abr.mQuality = check_set(lValue, 0, 13);
            lValue = ini_getl(sSection, KEY_minIQP, 0, confFile);
            para->rcAttr.mAttrH264Abr.mMinIQp = check_set(lValue, 0, VE_qpMAX);
            para->rcAttr.mAttrH264Abr.mMaxBitRate = para->bps;
        }
        else if( para->rcMode == VENC_rcFIXQP ) {
            lValue = ini_getl(sSection, KEY_IQP, VE_iQP, confFile);
            para->rcAttr.mAttrH264Abr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_PQP, VE_pQP, confFile);
            para->rcAttr.mAttrH264Abr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
        }
    }
}

void conf_loadVencParamsForLive(char* confFile, VencParams_t* para)
{
    long lValue = 0;
    bool b_h265 = false;
    char* sSection = SEC_VENC_LIVE;

    lValue = ini_getl(sSection, KEY_WIDTH, VE_WIDTH, confFile);
    para->width = check_set(lValue, 640, 3840);

    lValue = ini_getl(sSection, KEY_HEIGHT, VE_HEIGHT, confFile);
    para->height= check_set(lValue, 480, 2160);

    lValue = ini_getl(sSection, KEY_FPS, VE_FPS, confFile);
    para->fps= check_set(lValue, 24, 90);

    lValue = ini_getl(sSection, KEY_KBPS, VE_KBPS_LIVE, confFile);
    lValue = check_set(lValue, 32, 100*1024);
    para->bps= lValue * 1024;   //kbps to bps

    lValue = ini_getbool(sSection, KEY_H265, (VE_ENCODER==PT_H265), confFile);
    para->codecType = lValue ? PT_H265 : PT_H264;
    b_h265 = (para->codecType == PT_H265);

    lValue = ini_getl(sSection, KEY_RC, VE_RC, confFile);
    para->rcMode = check_set(lValue, VENC_rcCBR, VENC_rcABR);

    lValue = ini_getl(sSection, KEY_GOP, VE_GOP, confFile);
    para->maxKeyItl = check_set(lValue, 0, 300);

    if( b_h265 ) {
        sSection = SEC_H265_LIVE;

        lValue = ini_getl(sSection, KEY_PROFILE, VE_PROFILE, confFile);
        para->veAttr.mAttrH265.mProfile = check_set(lValue, VENC_profileBASELINE, VENC_profileMP);

        lValue = ini_getl(sSection, KEY_LEVEL, VE_LEVEL, confFile);
        para->veAttr.mAttrH265.mLevel = check_set(lValue, H265_LEVEL_1, H265_LEVEL_62);

        if( para->rcMode == VENC_rcCBR ) {
            lValue = ini_getl(sSection, KEY_minQP, VE_minQP, confFile);
            para->rcAttr.mAttrH265Cbr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_maxQP, VE_maxQP, confFile);
            para->rcAttr.mAttrH265Cbr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
            para->rcAttr.mAttrH265Cbr.mBitRate = para->bps;
        }
        else if( para->rcMode == VENC_rcVBR ) {
            lValue = ini_getl(sSection, KEY_minQP, VE_minQP, confFile);
            para->rcAttr.mAttrH265Vbr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_maxQP, VE_maxQP, confFile);
            para->rcAttr.mAttrH265Vbr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_ratioChangeQP, 0, confFile);
            para->rcAttr.mAttrH265Vbr.mRatioChangeQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_QUALITY, 0, confFile);
            para->rcAttr.mAttrH265Vbr.mQuality = check_set(lValue, 0, 13);
            para->rcAttr.mAttrH265Vbr.mMaxBitRate = para->bps;
        }
        else if( para->rcMode == VENC_rcABR ) {
            lValue = ini_getl(sSection, KEY_minQP, VE_minQP, confFile);
            para->rcAttr.mAttrH265Abr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_maxQP, VE_maxQP, confFile);
            para->rcAttr.mAttrH265Abr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_ratioChangeQP, 0, confFile);
            para->rcAttr.mAttrH265Abr.mRatioChangeQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_QUALITY, 0, confFile);
            para->rcAttr.mAttrH265Abr.mQuality = check_set(lValue, 0, 13);
            lValue = ini_getl(sSection, KEY_minIQP, 0, confFile);
            para->rcAttr.mAttrH265Abr.mMinIQp = check_set(lValue, 0, VE_qpMAX);
            para->rcAttr.mAttrH265Abr.mMaxBitRate = para->bps;
        }
        else if( para->rcMode == VENC_rcFIXQP ) {
            lValue = ini_getl(sSection, KEY_IQP, VE_iQP, confFile);
            para->rcAttr.mAttrH265Abr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_PQP, VE_pQP, confFile);
            para->rcAttr.mAttrH265Abr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
        }
    }
    else {
        sSection = SEC_H264_LIVE;

        lValue = ini_getl(sSection, KEY_PROFILE, VE_PROFILE, confFile);
        para->veAttr.mAttrH264.mProfile = check_set(lValue, VENC_profileBASELINE, VENC_profileSVC);

        lValue = ini_getl(sSection, KEY_LEVEL, VE_LEVEL, confFile);
        para->veAttr.mAttrH264.mLevel = check_set(lValue, H264_LEVEL_1, H264_LEVEL_51);

        if( para->rcMode == VENC_rcCBR ) {
            lValue = ini_getl(sSection, KEY_minQP, VE_minQP, confFile);
            para->rcAttr.mAttrH264Cbr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_maxQP, VE_maxQP, confFile);
            para->rcAttr.mAttrH264Cbr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
            para->rcAttr.mAttrH264Cbr.mBitRate = para->bps;
        }
        else if( para->rcMode == VENC_rcVBR ) {
            lValue = ini_getl(sSection, KEY_minQP, VE_minQP, confFile);
            para->rcAttr.mAttrH264Vbr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_maxQP, VE_maxQP, confFile);
            para->rcAttr.mAttrH264Vbr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_ratioChangeQP, 0, confFile);
            para->rcAttr.mAttrH264Vbr.mRatioChangeQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_QUALITY, 0, confFile);
            para->rcAttr.mAttrH264Vbr.mQuality = check_set(lValue, 0, 13);
            para->rcAttr.mAttrH264Vbr.mMaxBitRate = para->bps;
        }
        else if( para->rcMode == VENC_rcABR ) {
            lValue = ini_getl(sSection, KEY_minQP, VE_minQP, confFile);
            para->rcAttr.mAttrH264Abr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_maxQP, VE_maxQP, confFile);
            para->rcAttr.mAttrH264Abr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_ratioChangeQP, 0, confFile);
            para->rcAttr.mAttrH264Abr.mRatioChangeQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_QUALITY, 0, confFile);
            para->rcAttr.mAttrH264Abr.mQuality = check_set(lValue, 0, 13);
            lValue = ini_getl(sSection, KEY_minIQP, 0, confFile);
            para->rcAttr.mAttrH264Abr.mMinIQp = check_set(lValue, 0, VE_qpMAX);
            para->rcAttr.mAttrH264Abr.mMaxBitRate = para->bps;
        }
        else if( para->rcMode == VENC_rcFIXQP ) {
            lValue = ini_getl(sSection, KEY_IQP, VE_iQP, confFile);
            para->rcAttr.mAttrH264Abr.mMinQp = check_set(lValue, 0, VE_qpMAX);
            lValue = ini_getl(sSection, KEY_PQP, VE_pQP, confFile);
            para->rcAttr.mAttrH264Abr.mMaxQp = check_set(lValue, 0, VE_qpMAX);
        }
    }
}

void conf_saveVencParams(char* confFile, VencParams_t* para)
{
    bool b_h265 = (para->codecType==PT_H265);

    ini_putl(SEC_VENC, KEY_WIDTH, para->width, confFile);
    ini_putl(SEC_VENC, KEY_HEIGHT, para->height, confFile);
    ini_putl(SEC_VENC, KEY_FPS, para->fps, confFile);
    ini_putl(SEC_VENC, KEY_KBPS, para->bps/1024, confFile);
    ini_putl(SEC_VENC, KEY_H265, b_h265, confFile);
    ini_putl(SEC_VENC, KEY_RC, para->rcMode, confFile);
    ini_putl(SEC_VENC, KEY_GOP, para->maxKeyItl, confFile);

    if( b_h265 ) {
        char* sSection = SEC_H265;
        ini_putl(sSection, KEY_PROFILE, para->veAttr.mAttrH265.mProfile, confFile);
        ini_putl(sSection, KEY_LEVEL, para->veAttr.mAttrH265.mLevel, confFile);

        if( para->rcMode == VENC_rcCBR ) {
            ini_putl(sSection, KEY_minQP, para->rcAttr.mAttrH265Cbr.mMinQp, confFile);
            ini_putl(sSection, KEY_maxQP, para->rcAttr.mAttrH265Cbr.mMaxQp, confFile);
        }
        else if( para->rcMode == VENC_rcVBR ) {
            ini_putl(sSection, KEY_minQP, para->rcAttr.mAttrH265Vbr.mMinQp, confFile);
            ini_putl(sSection, KEY_maxQP, para->rcAttr.mAttrH265Vbr.mMaxQp, confFile);
            ini_putl(sSection, KEY_ratioChangeQP, para->rcAttr.mAttrH265Vbr.mRatioChangeQp, confFile);
            ini_putl(sSection, KEY_QUALITY, para->rcAttr.mAttrH265Vbr.mQuality, confFile);
        }
        else if( para->rcMode == VENC_rcABR ) {
            ini_putl(sSection, KEY_minQP, para->rcAttr.mAttrH265Abr.mMinQp, confFile);
            ini_putl(sSection, KEY_maxQP, para->rcAttr.mAttrH265Abr.mMaxQp, confFile);
            ini_putl(sSection, KEY_minIQP, para->rcAttr.mAttrH265Abr.mMinIQp, confFile);
            ini_putl(sSection, KEY_ratioChangeQP, para->rcAttr.mAttrH265Abr.mRatioChangeQp, confFile);
            ini_putl(sSection, KEY_QUALITY, para->rcAttr.mAttrH265Abr.mQuality, confFile);
        }
        else if( para->rcMode == VENC_rcFIXQP ) {
            ini_putl(sSection, KEY_IQP, para->rcAttr.mAttrH265FixQp.mIQp, confFile);
            ini_putl(sSection, KEY_PQP, para->rcAttr.mAttrH265FixQp.mPQp, confFile);
        }
    }
    else {
        char* sSection = SEC_H264;

        ini_putl(sSection, KEY_PROFILE, para->veAttr.mAttrH264.mProfile, confFile);
        ini_putl(sSection, KEY_LEVEL, para->veAttr.mAttrH264.mLevel, confFile);

        if( para->rcMode == VENC_rcCBR ) {
            ini_putl(sSection, KEY_minQP, para->rcAttr.mAttrH264Cbr.mMinQp, confFile);
            ini_putl(sSection, KEY_maxQP, para->rcAttr.mAttrH264Cbr.mMaxQp, confFile);
        }
        else if( para->rcMode == VENC_rcVBR ) {
            ini_putl(sSection, KEY_minQP, para->rcAttr.mAttrH264Vbr.mMinQp, confFile);
            ini_putl(sSection, KEY_maxQP, para->rcAttr.mAttrH264Vbr.mMaxQp, confFile);
            ini_putl(sSection, KEY_ratioChangeQP, para->rcAttr.mAttrH265Vbr.mRatioChangeQp, confFile);
            ini_putl(sSection, KEY_QUALITY, para->rcAttr.mAttrH264Vbr.mQuality, confFile);
        }
        else if( para->rcMode == VENC_rcABR ) {
            ini_putl(sSection, KEY_minQP, para->rcAttr.mAttrH264Abr.mMinQp, confFile);
            ini_putl(sSection, KEY_maxQP, para->rcAttr.mAttrH264Abr.mMaxQp, confFile);
            ini_putl(sSection, KEY_minIQP, para->rcAttr.mAttrH264Abr.mMinIQp, confFile);
            ini_putl(sSection, KEY_ratioChangeQP, para->rcAttr.mAttrH264Abr.mRatioChangeQp, confFile);
            ini_putl(sSection, KEY_QUALITY, para->rcAttr.mAttrH264Abr.mQuality, confFile);
        }
        else if( para->rcMode == VENC_rcFIXQP ) {
            ini_putl(sSection, KEY_IQP, para->rcAttr.mAttrH264FixQp.mIQp, confFile);
            ini_putl(sSection, KEY_PQP, para->rcAttr.mAttrH264FixQp.mPQp, confFile);
        }
    }
}

void conf_loadRecordParams(char* confFile, RecordParams_t* para)
{
    char sTemp[MAX_pathLEN];

    long lValue = ini_gets(SEC_RECORD, KEY_DISK, REC_diskPATH, sTemp, sizearray(sTemp), confFile);
    if( lValue > 0 ) {
        memset(para->diskPath, 0, sizeof(para->packPath));
        strcpy(para->diskPath, sTemp);
        memset(para->packPath, 0, sizeof(para->packPath));
        snprintf(para->packPath, MAX_pathLEN, "%s%s", para->diskPath, REC_packPATH);
    }

    lValue = ini_gets(SEC_RECORD, KEY_TYPE, REC_packTYPE, sTemp, sizearray(sTemp), confFile);
    if( lValue > 0 && lValue < 4) {
        int i;
        char* sTypes[] = REC_packTYPES;
        for( i=0; i<REC_packTypesNUM; i++ ) {
            if( strcmp(sTemp, sTypes[i]) == 0 ) {
                break;
            }
        }

        memset(para->packType, 0, sizeof(para->packType));
        if( i < REC_packTypesNUM ) {
            strcpy(para->packType, sTemp);
        }
        else {
            /* unsupported type, set to default */
            strcpy(para->packType, REC_packTYPE);
        }
    }

    lValue = ini_getl(SEC_RECORD, KEY_NAMING, NAMING_CONTIGUOUS, confFile);
    lValue = check_set(lValue, NAMING_CONTIGUOUS, NAMING_DATE);
    para->fileNaming = lValue;

    lValue = ini_getl(SEC_RECORD, KEY_DURATION, REC_packDURATION, confFile);
    lValue = check_set(lValue, REC_minDURATION, REC_maxDURATION);
    para->packDuration = lValue * 60*1000;   //minutes to millseconds

    lValue = ini_getl(SEC_RECORD, KEY_SIZE, REC_packSIZE, confFile);
    lValue = check_set(lValue, REC_minSIZE, REC_maxSIZE);
    para->packSize = lValue * 1024*1024;   //MB to bytes

    lValue = ini_getl(SEC_RECORD, KEY_FULL, REC_diskFULL, confFile);
    para->minDiskSize = check_set(lValue, REC_minSIZE, REC_maxSIZE);

    lValue = ini_getbool(SEC_RECORD, KEY_AUDIO, TRUE, confFile);
    para->enableAudio = (lValue>0);
}

void conf_saveRecordParams(char* confFile, RecordParams_t* para)
{
    ini_puts(SEC_RECORD, KEY_DISK, para->diskPath, confFile);
    ini_puts(SEC_RECORD, KEY_TYPE, para->packType, confFile);
    ini_putl(SEC_RECORD, KEY_DURATION, para->packDuration/(60*1000), confFile);
    ini_putl(SEC_RECORD, KEY_SIZE, para->packSize/(1024*1024), confFile);
    ini_putl(SEC_RECORD, KEY_FULL, para->minDiskSize, confFile);
    ini_putl(SEC_RECORD, KEY_AUDIO, para->enableAudio, confFile);
}

void conf_loadAiParams(char* confFile, AiParams_t* para)
{
    long lValue = 0;

    lValue = ini_getl(SEC_AI, KEY_deviceNO, AI_devNO, confFile);
    para->aiDev = check_set(lValue, AI_devMIC1, AI_devI2S);

    lValue = ini_getl(SEC_AI, KEY_sampleRATE, AI_sampleRATE, confFile);
    para->sampleRate = check_set(lValue, AUDIO_SAMPLE_RATE_8000, AUDIO_SAMPLE_RATE_48000);

    lValue = ini_getl(SEC_AI, KEY_CHANNELS, AI_CHANNELS, confFile);
    para->channels = check_set(lValue, 1, 2);

    lValue = ini_getl(SEC_AI, KEY_sampleBITS, AI_sampleBITS, confFile);
    para->bitsPerSample = check_set(lValue, (AUDIO_BIT_WIDTH_8+1)*8, (AUDIO_BIT_WIDTH_32+1)*8);
}

void conf_saveAiParams(char* confFile, AiParams_t* para)
{
    ini_putl(SEC_AI, KEY_deviceNO, para->aiDev, confFile);
    ini_putl(SEC_AI, KEY_sampleRATE, para->sampleRate, confFile);
    ini_putl(SEC_AI, KEY_sampleBITS, para->bitsPerSample, confFile);
    ini_putl(SEC_AI, KEY_CHANNELS, para->channels, confFile);
}

void conf_loadAencParams(char* confFile, AencParams_t* para)
{
    long lValue = 0;

    lValue = ini_getbool(SEC_AENC, KEY_AAC, (AE_CODEC==PT_AAC), confFile);
    para->codecType = lValue ? PT_AAC : PT_MP3;

    lValue = ini_getl(SEC_AENC, KEY_BPS, AE_BPS, confFile);
    para->bitRate = check_set(lValue, 8000, 128000);
}

void conf_saveAencParams(char* confFile, AencParams_t* para)
{
    ini_putl(SEC_AENC, KEY_AAC, para->codecType==PT_AAC, confFile);
    ini_putl(SEC_AENC, KEY_BPS, para->bitRate, confFile);
}

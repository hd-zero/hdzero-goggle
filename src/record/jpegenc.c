#define LOG_NDEBUG 0
#define LOG_TAG "jpegenc"
#include <log/log.h>

#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include <mpi_vi.h>
#include "jpegenc.h"

typedef struct
{
    VENC_CHN mChn;
    //VENC_CHN_ATTR_S mChnAttr;
    //VENC_PARAM_JPEG_S mJpegParam;
    VENC_STREAM_S mOutStream;
    VENC_JPEG_THUMB_BUFFER_S mJpegThumbBuf;
    unsigned int mCurFrameId;
    bool         mSemFrameBack;

    JpegEncIO_t  io;
    pthread_t    threadId;
    int32_t      s32Timeout;    //milliseconds
    JpegEncConfig_t config;
} JpegEnc_t;

void jpegenc_init(JpegEnc_t* jenc)
{
    VENC_PACK_S *pEncPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S));
    memset(pEncPack, 0, sizeof(VENC_PACK_S));

    jenc->mChn = MM_INVALID_CHN;
    jenc->mCurFrameId = -1;
    jenc->mOutStream.mpPack = pEncPack;
    jenc->mOutStream.mPackCount = 1;
    jenc->mSemFrameBack = false;
}

void jpegenc_deinit(JpegEnc_t* jenc)
{
    if( jenc->mOutStream.mpPack != NULL ) {
        free(jenc->mOutStream.mpPack);
    }

    jenc->mOutStream.mpPack = NULL;
    jenc->mOutStream.mPackCount = 0;
    jenc->mSemFrameBack = false;

    if(jenc->io.sFile != NULL) {
        free(jenc->io.sFile);
        jenc->io.sFile = NULL;
    }
}

static JpegEnc_t* jpegenc_new(void)
{
    JpegEnc_t* jenc = (JpegEnc_t*)malloc(sizeof(JpegEnc_t));
    if( jenc != NULL ) {
        memset(jenc, 0, sizeof(JpegEnc_t));
        jpegenc_init(jenc);
    }
    else {
        LOGE("out of memory");
    }
    return jenc;
}

static void jpegenc_free(JpegEnc_t* jenc)
{
    if( jenc == NULL ) {
        return;
    }

    jpegenc_deinit(jenc);
    free(jenc);
}

ERRORTYPE jpegenc_mppCallback(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    ERRORTYPE ret = SUCCESS;
    JpegEnc_t *pThiz = (JpegEnc_t*)cookie;
    if(MOD_ID_VENC == pChn->mModId)
    {
        if(pChn->mChnId != pThiz->mChn)
        {
            LOGE("fatal error! VO chnId[%d]!=[%d]", pChn->mChnId, pThiz->mChn);
        }
        switch(event)
        {
            case MPP_EVENT_RELEASE_VIDEO_BUFFER:
            {
                VIDEO_FRAME_INFO_S *pVideoFrameInfo = (VIDEO_FRAME_INFO_S*)pEventData;
                if(pThiz->mCurFrameId != pVideoFrameInfo->mId)
                {
                    LOGE("fatal error! frameId is not match[%d]!=[%d]!", pThiz->mCurFrameId, pVideoFrameInfo->mId);
                }
                pThiz->mSemFrameBack = true;
                break;
            }
            case MPP_EVENT_VENC_BUFFER_FULL:
            {
                LOGD("jpeg encoder chn[%d] vbvBuffer full", pChn->mChnId);
                break;
            }
            default:
            {
                LOGE("fatal error! unknown event[0x%x] from channel[0x%x][0x%x][0x%x]!", event, pChn->mModId, pChn->mDevId, pChn->mChnId);
                ret = ERR_VENC_ILLEGAL_PARAM;
                break;
            }
        }
    }
    else
    {
        LOGE("fatal error! why modId[0x%x]?", pChn->mModId);
        ret = FAILURE;
    }
    return ret;
}

ERRORTYPE jpegenc_initialize(JpegEnc_t* jenc, VIDEO_FRAME_INFO_S* frame, JpegEncConfig_t * config)
{
    ERRORTYPE ret = SUCCESS;
    if(jenc->mChn >= 0)
    {
        LOGE("fatal error! already init jpeg encoder!");
    }

    uint16_t picWidth = config->width;
    uint16_t picHeight= config->height;
    uint16_t mPictureNum = 1;

    uint32_t minVbvBufSize = (picWidth * picHeight * 3/2 /2) + (picWidth*picHeight/3);
    uint32_t vbvBufSize = (picWidth * picHeight * 3/2 /10 * mPictureNum) + (picWidth*picHeight/3);
    if(vbvBufSize < minVbvBufSize)
    {
        vbvBufSize = minVbvBufSize;
    }
    if(vbvBufSize > 16*1024*1024)
    {
        LOGD("Be careful! vbvSize[%d]MB is too large, decrease to 16MB", vbvBufSize/(1024*1024));
        vbvBufSize = 16*1024*1024;
    }

    VENC_CHN mChn;
    VENC_CHN_ATTR_S mChnAttr;
    memset(&mChnAttr, 0, sizeof(VENC_CHN_ATTR_S));
    mChnAttr.VeAttr.Type = PT_JPEG;
    mChnAttr.VeAttr.AttrJpeg.MaxPicWidth = 0;
    mChnAttr.VeAttr.AttrJpeg.MaxPicHeight = 0;
    mChnAttr.VeAttr.AttrJpeg.BufSize = ALIGN(vbvBufSize, 1024);
    mChnAttr.VeAttr.AttrJpeg.bByFrame = TRUE;
    mChnAttr.VeAttr.AttrJpeg.PicWidth = picWidth;
    mChnAttr.VeAttr.AttrJpeg.PicHeight = picHeight;
    mChnAttr.VeAttr.AttrJpeg.bSupportDCF = FALSE;
    mChnAttr.VeAttr.MaxKeyInterval = 1;
    mChnAttr.VeAttr.SrcPicWidth = frame->VFrame.mWidth;
    mChnAttr.VeAttr.SrcPicHeight = frame->VFrame.mHeight;
    mChnAttr.VeAttr.Field = VIDEO_FIELD_FRAME;
    mChnAttr.VeAttr.PixelFormat = frame->VFrame.mPixelFormat;

    bool bSuccessFlag = false;
    mChn = 0;
    while(mChn < VENC_MAX_CHN_NUM)
    {
        ret = AW_MPI_VENC_CreateChn(mChn, &mChnAttr);
        if(SUCCESS == ret)
        {
            bSuccessFlag = true;
            LOGD("create venc channel[%d] success!", mChn);
            break;
        }
        else if(ERR_VENC_EXIST == ret)
        {
            LOGD("venc channel[%d] is exist, find next!", mChn);
            mChn++;
        }
        else
        {
            LOGD("create venc channel[%d] ret[0x%x], find next!", mChn, ret);
            mChn++;
        }
    }
    if(false == bSuccessFlag)
    {
        mChn = MM_INVALID_CHN;
        LOGE("fatal error! create venc channel fail!");
        ret = FAILURE;
        goto _err0;
    }

    MPPCallbackInfo cbInfo;
    cbInfo.cookie = (void*)jenc;
    cbInfo.callback = (MPPCallbackFuncType)&jpegenc_mppCallback;
    AW_MPI_VENC_RegisterCallback(mChn, &cbInfo);

    VENC_PARAM_JPEG_S mJpegParam;
    memset(&mJpegParam, 0, sizeof(VENC_PARAM_JPEG_S));
    mJpegParam.Qfactor = config->quality;
    AW_MPI_VENC_SetJpegParam(mChn, &mJpegParam);
    AW_MPI_VENC_ForbidDiscardingFrame(mChn, TRUE);

    jenc->mChn = mChn;

    ret = AW_MPI_VENC_StartRecvPic(mChn);
    if(SUCCESS != ret)
    {
        LOGE("fatal error:%x jpegEnc AW_MPI_VENC_StartRecvPic",ret);
    }

_err0:
    return ret;
}

ERRORTYPE jpegenc_destroy(JpegEnc_t* jenc)
{
    if( jenc->mChn != MM_INVALID_CHN ) {
        AW_MPI_VENC_StopRecvPic(jenc->mChn);
        //can't reset channel now, because in non-tunnel mode, mpi_venc will force release out frames, but we don't want
        //those out frames to be released before we return them.
        //AW_MPI_VENC_ResetChn(jenc->mChn);
        AW_MPI_VENC_DestroyChn(jenc->mChn);
        jenc->mChn = MM_INVALID_CHN;
    }

    return SUCCESS;
}

ERRORTYPE jpegenc_encode(JpegEnc_t* jenc, VIDEO_FRAME_INFO_S *frameInfo, VENC_EXIFINFO_S *exifInfo, int32_t s32MillSec)
{
    jenc->mCurFrameId = frameInfo->mId;

    if( exifInfo != NULL ) {
        AW_MPI_VENC_SetJpegExifInfo(jenc->mChn, exifInfo);
    }

    AW_MPI_VENC_SendFrame(jenc->mChn, frameInfo, 0);

    if( s32MillSec > 0 ) {
        while( s32MillSec-- > 0 ) {
            usleep(1000);
            if( jenc->mSemFrameBack ) {
                break;
            }
        }

        if(0 >= s32MillSec)
        {
            LOGD("Be careful! jpeg encode timeout");
            return FAILURE;
        }
    }

    return SUCCESS;
}

int jpegenc_getFrame(JpegEnc_t* jenc, int32_t s32Timeout)
{
    ERRORTYPE ret = AW_MPI_VENC_GetStream(jenc->mChn, &jenc->mOutStream, s32Timeout);
    if(ret != SUCCESS)
    {
        LOGE("fatal error! why get stream fail?");
        return -1;
    }
    else
    {
        AW_MPI_VENC_GetJpegThumbBuffer(jenc->mChn, &jenc->mJpegThumbBuf);
        return 0;
    }
}

int jpegenc_returnFrame(JpegEnc_t* jenc, VENC_STREAM_S *pVencStream)
{
    ERRORTYPE ret = AW_MPI_VENC_ReleaseStream(jenc->mChn, pVencStream);
    if(ret != SUCCESS)
    {
        LOGE("fatal error! why release stream fail?");
        return -1;
    }
    else
    {
        return 0;
    }
}

uint32_t jpegenc_getDataSize(JpegEnc_t* jenc)
{
    return jenc->mOutStream.mpPack[0].mLen0 +
           jenc->mOutStream.mpPack[0].mLen1 +
           jenc->mOutStream.mpPack[0].mLen2;
}

int jpegenc_getDataToBuffer(JpegEnc_t* jenc, void *buffer)
{
    char *p = (char *)buffer;
    memcpy(p, jenc->mOutStream.mpPack[0].mpAddr0, jenc->mOutStream.mpPack[0].mLen0);
    p += jenc->mOutStream.mpPack[0].mLen0;
    if(jenc->mOutStream.mpPack[0].mLen1 > 0)
    {
        memcpy(p, jenc->mOutStream.mpPack[0].mpAddr1, jenc->mOutStream.mpPack[0].mLen1);
    }
    return SUCCESS;
}

ERRORTYPE jpegenc_getThumbOffset(JpegEnc_t* jenc, off_t* offset, size_t* len)
{
    ERRORTYPE ret = SUCCESS;
    if(jenc->mJpegThumbBuf.ThumbLen > 0)
    {
        *len = jenc->mJpegThumbBuf.ThumbLen;
        //deduce thumb picture buffer in mOutStream
        if(NULL==jenc->mOutStream.mpPack[0].mpAddr0 || NULL==jenc->mOutStream.mpPack[0].mpAddr1 ||
           0==jenc->mOutStream.mpPack[0].mLen0 || 0==jenc->mOutStream.mpPack[0].mLen1)
        {
            LOGE("fatal error! check code!");
        }
        //uint8_t *pExifBufStart = jenc->mOutStream.mpPack[0].mpAddr0;
        if(jenc->mJpegThumbBuf.ThumbAddrVir >= jenc->mOutStream.mpPack[0].mpAddr0)
        {
            if(jenc->mJpegThumbBuf.ThumbAddrVir >= jenc->mOutStream.mpPack[0].mpAddr0+jenc->mOutStream.mpPack[0].mLen0)
            {
                LOGE("fatal error! check code![%p][%p][%d]", jenc->mJpegThumbBuf.ThumbAddrVir,
                      jenc->mOutStream.mpPack[0].mpAddr0, jenc->mOutStream.mpPack[0].mLen0);
            }
            *offset = jenc->mJpegThumbBuf.ThumbAddrVir - jenc->mOutStream.mpPack[0].mpAddr0;
        }
        else if(jenc->mJpegThumbBuf.ThumbAddrVir>=jenc->mOutStream.mpPack[0].mpAddr1)
        {
            if(jenc->mJpegThumbBuf.ThumbAddrVir >= jenc->mOutStream.mpPack[0].mpAddr1+jenc->mOutStream.mpPack[0].mLen1)
            {
                LOGE("fatal error! check code![%p][%p][%d]", jenc->mJpegThumbBuf.ThumbAddrVir,
                      jenc->mOutStream.mpPack[0].mpAddr1, jenc->mOutStream.mpPack[0].mLen1);
            }
            *offset = jenc->mOutStream.mpPack[0].mLen0 + (jenc->mJpegThumbBuf.ThumbAddrVir - jenc->mOutStream.mpPack[0].mpAddr1);
        }
        else
        {
            LOGE("fatal error! check code![%p][%p][%d][%p][%d]", jenc->mJpegThumbBuf.ThumbAddrVir,
                jenc->mOutStream.mpPack[0].mpAddr0, jenc->mOutStream.mpPack[0].mLen0,
                jenc->mOutStream.mpPack[0].mpAddr1, jenc->mOutStream.mpPack[0].mLen1);
            ret = FAILURE;
        }
    }
    else
    {
        LOGD("jpeg has no thumb picture");
        *offset = 0;
        *len = 0;
    }

    //LOGD("size of off_t[%d], size_t[%d]", sizeof(off_t), sizeof(size_t));
    //LOGD("thumbBuf [%p][%d], [%p][%d][%p][%d][%p][%d]", jenc->mJpegThumbBuf.ThumbAddrVir, jenc->mJpegThumbBuf.ThumbLen,
    //    jenc->mOutStream.mpPack[0].mpAddr0, jenc->mOutStream.mpPack[0].mLen0,
    //    jenc->mOutStream.mpPack[0].mpAddr1, jenc->mOutStream.mpPack[0].mLen1,
    //    jenc->mOutStream.mpPack[0].mpAddr2, jenc->mOutStream.mpPack[0].mLen2);
    //LOGD("offset[%jd], size[%d]", *offset, *len);

    return ret;
}

void getCurrentDateTime(char *buf, int bufsize)
{
    time_t t;
    struct tm *tm_t;
    time(&t);
    tm_t = localtime(&t);
    snprintf(buf, bufsize, "%4d:%02d:%02d %02d:%02d:%02d",
        tm_t->tm_year+1900, tm_t->tm_mon+1, tm_t->tm_mday,
        tm_t->tm_hour, tm_t->tm_min, tm_t->tm_sec);
}

int jpegenc_encodeFrame(VIDEO_FRAME_INFO_S* frame, JpegEncConfig_t* config, CB_onJpegFrame cbOnFrame, void* context)
{
    ERRORTYPE ret = SUCCESS;
    JpegEnc_t jenc;
    off_t     thumbOffset = 0;
    size_t    thumbLen = 0;

    jpegenc_init(&jenc);
    ret = jpegenc_initialize(&jenc, frame, config);
    if (ret != SUCCESS)
    {
        LOGE("CameraJpegEncoder initialize error!");
        goto JPEG_INIT_ERR;
    }
    LOGD("Picture size %dx%d, Thumb size %dx%d", config->width, config->height,
          config->thumbnailWidth, config->thumbnailHeight);

    jpegenc_encode(&jenc, frame, NULL, 50);
    jpegenc_getFrame(&jenc, 100);
    jpegenc_getThumbOffset(&jenc, &thumbOffset, &thumbLen);

    PictureBuffer_t buf;
    buf.mpData0 = jenc.mOutStream.mpPack[0].mpAddr0;
    buf.mpData1 = jenc.mOutStream.mpPack[0].mpAddr1;
    buf.mpData2 = jenc.mOutStream.mpPack[0].mpAddr2;
    buf.mLen0 = jenc.mOutStream.mpPack[0].mLen0;
    buf.mLen1 = jenc.mOutStream.mpPack[0].mLen1;
    buf.mLen2 = jenc.mOutStream.mpPack[0].mLen2;
    buf.mThumbOffset = thumbOffset;
    buf.mThumbLen = thumbLen;
    buf.mDataSize = buf.mLen0+buf.mLen1+buf.mLen2;

#if(0)
    uint8_t* p = NULL;
    PictureBuffer_t* pbuf = &buf;
    if(pbuf->mLen0 > 0)
    {
        memcpy(p, pbuf->mpData0, pbuf->mLen0);
        p += pbuf->mLen0;
    }
    if(pbuf->mLen1 > 0)
    {
        memcpy(p, pbuf->mpData1, pbuf->mLen1);
        p += pbuf->mLen1;
    }
    if(pbuf->mLen2 > 0)
    {
        memcpy(p, pbuf->mpData2, pbuf->mLen2);
        p += pbuf->mLen2;
    }

    off_t thumboffset = pbuf->mThumbOffset;
    size_t thumblen = pbuf->mThumbLen;
    memcpy(p, &thumboffset, sizeof(off_t));
    p += sizeof(off_t);
    memcpy(p, &thumblen, sizeof(size_t));
    p += sizeof(size_t);
    memcpy(p, &jpegsize, sizeof(size_t));
#endif

#if(0)
	int fd = -1;
	fd = open("/home/movies/0.jpg", O_RDWR | O_CREAT, 0666);
	if(fd >= 0) {
        if(buf.mLen0 > 0) {
            ret = write(fd, buf.mpData0, buf.mLen0);
            if (ret < 0) {
                LOGE("write data filed(%s)", strerror(errno));
            }
        }
        if(buf.mLen1 > 0) {
            ret = write(fd, buf.mpData1, buf.mLen1);
            if (ret < 0) {
                LOGE("write data filed(%s)", strerror(errno));
            }
        }
        if(buf.mLen2 > 0) {
            ret = write(fd, buf.mpData2, buf.mLen2);
            if (ret < 0) {
                LOGE("write data filed(%s)", strerror(errno));
            }
        }
	}
#endif

    if( cbOnFrame != NULL ) {
        cbOnFrame( &buf, context );
    }

    jpegenc_returnFrame(&jenc, &jenc.mOutStream);

JPEG_INIT_ERR:
    jpegenc_destroy(&jenc);
    jpegenc_deinit(&jenc);
    return 0;
}

static ERRORTYPE jpegenc_createViChn(VI_DEV ViDev, void *pAttr)
{
    ERRORTYPE ret;
    VI_DEV  mViDev = ViDev;
    VI_CHN  mViChn = 0;
    BOOL nSuccessFlag = FALSE;

    mViChn = 0;
    while (mViChn < VIU_MAX_CHN_NUM)
    {
        ret = AW_MPI_VI_CreateVirChn(mViDev, mViChn, pAttr);
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

    return mViChn;
}

static int jpegenc_startVirvi(VI_DEV ViDev, VI_CHN ViCh, void *pAttr)
{
    int ret = -1;

#if(0)
    ret = AW_MPI_VI_CreateVirChn(ViDev, ViCh, pAttr);
    if(ret < 0)
    {
        LOGE("Create VI Chn failed,VIDev = %d,VIChn = %d",ViDev,ViCh);
        return ret ;
    }
#endif
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

static int jpegenc_stopVirvi(VI_DEV ViDev, VI_CHN ViCh)
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

static void* jpegenc_frameProc(void *arg)
{
    JpegEnc_t* jenc = (JpegEnc_t*)arg;
    VI_DEV vipp_dev = jenc->io.viDev;
    VI_CHN virvi_chn = jenc->io.viChn;
    off_t     thumbOffset = 0;
    size_t    thumbLen = 0;
    int ret = 0;

    VIDEO_FRAME_INFO_S srcFrame;
    memset(&srcFrame, 0, sizeof(srcFrame));

    virvi_chn = jpegenc_createViChn(vipp_dev, NULL);
    if(virvi_chn < 0)
    {
        goto failed;
    }

    ret = jpegenc_startVirvi(vipp_dev, virvi_chn, NULL);
    if( ret != SUCCESS ) {
        LOGE("start vi chn[%d] failed: %x", vipp_dev, virvi_chn, ret);
        goto failed;
    }

    LOGD("jpeg thread: dev[%d] chn[%d]", vipp_dev, virvi_chn);

    ret = AW_MPI_VI_GetFrame(vipp_dev, virvi_chn, &srcFrame, jenc->s32Timeout);
    if( ret != SUCCESS ) {
        LOGE("get vi frame from dev[%d] chn[%d] failed: %x", vipp_dev, virvi_chn);
        goto failed;
    }

    ret = jpegenc_initialize(jenc, &srcFrame, &jenc->config);
    if( ret != SUCCESS ) {
        LOGE("start venc failed: %x", ret);
        goto failed;
    }

    VENC_EXIFINFO_S exifInfo;
    VENC_EXIFINFO_S *pExifInfo = NULL;
    memset(&exifInfo, 0, sizeof(exifInfo));
    if( jenc->io.bThumb ) {
        exifInfo.thumb_quality = jenc->config.thunbnailQuality;
        exifInfo.ThumbWidth = jenc->config.thumbnailWidth;
        exifInfo.ThumbHeight = jenc->config.thumbnailHeight;
        pExifInfo = &exifInfo;
    }

    ret = jpegenc_encode(jenc, &srcFrame, pExifInfo, jenc->s32Timeout);
    if( ret != SUCCESS ) {
        LOGE("encode failed: %x", ret);
        goto failed;
    }

    ret = jpegenc_getFrame(jenc, jenc->s32Timeout);
    if( ret != SUCCESS ) {
        LOGE("get encoded frame failed: %x", ret);
        goto failed;
    }

    jpegenc_getThumbOffset(jenc, &thumbOffset, &thumbLen);

    PictureBuffer_t buf;
    buf.mpData0 = jenc->mOutStream.mpPack[0].mpAddr0;
    buf.mpData1 = jenc->mOutStream.mpPack[0].mpAddr1;
    buf.mpData2 = jenc->mOutStream.mpPack[0].mpAddr2;
    buf.mLen0 = jenc->mOutStream.mpPack[0].mLen0;
    buf.mLen1 = jenc->mOutStream.mpPack[0].mLen1;
    buf.mLen2 = jenc->mOutStream.mpPack[0].mLen2;
    buf.mThumbOffset = thumbOffset;
    buf.mThumbLen = thumbLen;
    buf.mDataSize = buf.mLen0+buf.mLen1+buf.mLen2;

    jpegenc_returnFrame(jenc, &jenc->mOutStream);

    if( jenc->io.cbOnFrame != NULL ) {
        jenc->io.cbOnFrame( &buf, jenc->io.ctxOnFrame );
    }

    if( jenc->io.sFile != NULL ) {
        int fd = -1;

        LOGD("write file: %s", jenc->io.sFile);

        fd = open(jenc->io.sFile, O_RDWR | O_CREAT, 0666);
        if(fd >= 0) {
            if( jenc->io.bThumb ) {
                if( buf.mThumbLen > 0 ) {
                    ret = write(fd, jenc->mJpegThumbBuf.ThumbAddrVir, buf.mThumbLen);
                }
            }
            else {
                if(buf.mLen0 > 0) {
                    ret = write(fd, buf.mpData0, buf.mLen0);
                    if (ret < 0) {
                        LOGE("write data failed(%s)", strerror(errno));
                    }
                }
                if(buf.mLen1 > 0) {
                    ret = write(fd, buf.mpData1, buf.mLen1);
                    if (ret < 0) {
                        LOGE("write data failed(%s)", strerror(errno));
                    }
                }
                if(buf.mLen2 > 0) {
                    ret = write(fd, buf.mpData2, buf.mLen2);
                    if (ret < 0) {
                        LOGE("write data failed(%s)", strerror(errno));
                    }
                }
                if( ret > 0 ) {
                    ret = SUCCESS;
                }
            }
            close(fd);
        }
    }

failed:
    AW_MPI_VI_ReleaseFrame(vipp_dev, virvi_chn, &srcFrame);
    jpegenc_stopVirvi(vipp_dev, virvi_chn);
    jpegenc_destroy(jenc);
    jpegenc_free(jenc);

    LOGD("jpeg thread done: %x", ret);

    return NULL;
}

int jpegenc_takePicture(JpegEncIO_t* io, JpegEncConfig_t* config, int32_t s32Millisec)
{
    JpegEnc_t* jenc = jpegenc_new();
    if( jenc == NULL ) {
            LOGE("out of memory");
        return -1;
    }

    memcpy(&jenc->io, io, sizeof(JpegEncIO_t));
    if( io->sFile != NULL ) {
        int len = strlen(io->sFile);
        jenc->io.sFile = (char*)malloc(len+2);
        memset(jenc->io.sFile, 0, len+2);
        memcpy(jenc->io.sFile, io->sFile, len);
    }

    memcpy(&jenc->config, config, sizeof(JpegEncConfig_t));
    jenc->s32Timeout = s32Millisec;

    return pthread_create(&jenc->threadId, NULL, jpegenc_frameProc, jenc);
}

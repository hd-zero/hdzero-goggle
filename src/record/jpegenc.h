#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <mpi_venc.h>

#define PictureThumbQuality_DEFAULT 60

typedef struct JpegEncConfig
{
    uint16_t width;
    uint16_t height;
    uint16_t quality;
    uint16_t thumbnailWidth;
    uint16_t thumbnailHeight;
    uint16_t thunbnailQuality;
} JpegEncConfig_t;

typedef struct PictureBuffer
{
    uint8_t *mpData0;
    uint8_t *mpData1;
    uint8_t *mpData2;
    uint32_t mLen0;
    uint32_t mLen1;
    uint32_t mLen2;
    uint32_t mThumbOffset;
    uint32_t mThumbLen;
    size_t   mDataSize;
} PictureBuffer_t;

typedef int (*CB_onJpegFrame)(PictureBuffer_t* frameBuffer, void* context);

typedef struct JpegEncIO
{
    VI_DEV viDev;
    VI_CHN viChn;

    CB_onJpegFrame cbOnFrame;
    void*  ctxOnFrame;
    char*  sFile;
    bool   bThumb;
} JpegEncIO_t;

int jpegenc_encodeFrame(VIDEO_FRAME_INFO_S* frame, JpegEncConfig_t* config, CB_onJpegFrame cbOnFrame, void* context);
int jpegenc_takePicture(JpegEncIO_t* io, JpegEncConfig_t* config, int32_t s32Millisec);

#ifdef __cplusplus
}
#endif

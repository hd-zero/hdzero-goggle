#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "ffmpeg.h"

typedef enum AVMediaType FFMediaType_e;
typedef enum AVCodecID   FFCodecID_e;
typedef int (*CB_onData)(void* ffPtr, int streamIndex, uint8_t* pktData, int pktLen, void* context);

typedef struct tagVideoParameters
{
    int width;
    int height;
    int fps;        //frame rate
} FFVideoParameters_t;

typedef struct tagAudioParameters
{
    int channels;
    int sample_rate;
    int bits_per_sample;
} FFAudioParameters_t;

typedef struct tagDataParameters
{
    int encrypt_pos;
    int encrypt_len;
    int reserved;
} FFDataParameters_t;

typedef struct tagStreamParameters
{
    FFMediaType_e mediaType;
    FFCodecID_e   codecId;
    uint8_t*      spsData;
    uint32_t      spsLen;

    union {
        FFVideoParameters_t video;
        FFAudioParameters_t audio;
        FFDataParameters_t  data;
    };
} FFStreamParameters_t;

typedef struct tagFFPack
{
    AVFormatContext* ofmtContext;
    CB_onData   cbOnData;
    void*       cbContext;
    uint64_t    nbTotalSize;
} FFPack_t;

FFPack_t* ffpack_openFile(char* sName, void* context);
FFPack_t* ffpack_open(CB_onData fnOnData, void* context);
int  ffpack_newProgram(FFPack_t* ff, char* sName);
int  ffpack_newVideoStream(FFPack_t* ff, uint16_t programId, FFStreamParameters_t* param);
int  ffpack_newAudioStream(FFPack_t* ff, uint16_t programId, FFStreamParameters_t* param);
int  ffpack_newDataStream (FFPack_t* ff, uint16_t programId, FFStreamParameters_t* param);
int  ffpack_start(FFPack_t* ff);
int  ffpack_input(FFPack_t* ff, int streamIndex, uint8_t* frameData, int frameLen, bool keyFrame, uint64_t pts);
void ffpack_close(FFPack_t* ff);
int  ffpack_inputStream(FFPack_t* ff, int streamIndex, void* stream, void* pkt);

void ffpack_setParams(FFPack_t* ff, int streamIndex, void* param);
int  ffpack_setExtradata(FFPack_t* ff, int streamIndex, void* extradata, int extradataSize);

#ifdef __cplusplus
}
#endif

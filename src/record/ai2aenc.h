#ifndef __AI2AENC_H_
#define __AI2AENC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <plat_type.h>
#include <mm_common.h>
#include <mpi_sys.h>
#include <mm_comm_aenc.h>
#include <mm_comm_aio.h>
#include <mpi_ai.h>
#include <mpi_aenc.h>
#include <pthread.h>

#define ADTS_HEADER__LEN 7

typedef struct WaveHeader{
    int riff_id;
    int riff_sz;
    int riff_fmt;
    int fmt_id;
    int fmt_sz;
    short audio_fmt;
    short num_chn;
    int sample_rate;
    int byte_rate;
    short block_align;
    short bits_per_sample;
    int data_id;
    int data_sz;
} WaveHeader;

typedef enum
{
    AI_devMIC1  = 0,
    AI_devMIC2  = 1,
    AI_devLineIN= 2,
    AI_devI2S   = 3,
} AiDevices_e;

typedef struct
{
    int         aiDev;
    AI_CHN      aiChn;
    int         sampleRate;
    int         channels;
    int         bitsPerSample;
    int         mFrameSize;
} AiParams_t;

typedef AIO_ATTR_S  AioAttr_t;
typedef AENC_ATTR_S AencAttr_t;

typedef struct
{
    PAYLOAD_TYPE_E codecType;
    int bitRate;
} AencParams_t;

typedef int (*CB_onAencFrame)(void* aa, uint8_t* frameData, int frameLen, uint64_t pts, void* context);

typedef struct
{
    int         aiDev;
    AI_CHN      aiChn;
    AIO_ATTR_S  aiAttr;

    pthread_t   threadId;
    AENC_CHN    aeChn;
    bool        bExit;
    AENC_ATTR_S aeAttr;

    CB_onAencFrame  cbOnFrame;
    void*       contextOfOnFrame;

    int64_t     startTimestamp;
} Ai2Aenc_t;

Ai2Aenc_t* ai2aenc_initSys(CB_onAencFrame cbOnFrame, void* context);
void ai2aenc_deinitSys(Ai2Aenc_t* aa);

ERRORTYPE ai2aenc_prepare(Ai2Aenc_t* aa, AiParams_t* aiParams, AencParams_t* aeParams);
ERRORTYPE ai2aenc_start(Ai2Aenc_t* aa);
ERRORTYPE ai2aenc_stop(Ai2Aenc_t* aa, bool disableDev);

#ifdef __cplusplus
}
#endif

#endif  /* __AI2AENC_H_ */


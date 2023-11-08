#pragma once

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
#include <mpi_ao.h>
#include <mpi_clock.h>
#include <ClockCompPortIndex.h>
#include <pthread.h>

#include "ai2aenc.h"

typedef struct pcmFrameNode {
    AUDIO_FRAME_S frame;
    struct list_head mList;
} pcmFrameNode;

typedef struct
{
    int         aiDev;
    AI_CHN      aiChn;
    int         aoDev;
    AO_CHN      aoChn;
    AIO_ATTR_S  aioAttr;
    CLOCK_CHN   clkChn;
    CLOCK_CHN_ATTR_S clkChnAttr;

    pthread_t   threadId;
    bool        bExit;

    pthread_mutex_t pcmlock;
    struct list_head pcmIdleList;    // AUDIO_FRAME_INFO_S
    struct list_head pcmUsingList;   // AUDIO_FRAME_INFO_S

    bool   tunnelMode;
} Ai2Ao_t;

Ai2Ao_t* ai2ao_initSys(void);
void ai2ao_deinitSys(Ai2Ao_t* aa);

ERRORTYPE ai2ao_start(Ai2Ao_t* aa, AiParams_t* aiParams);
ERRORTYPE ai2ao_stop(Ai2Ao_t* aa, bool disableDev);
bool      ai2ao_playing(Ai2Ao_t* aa);

#ifdef __cplusplus
}
#endif

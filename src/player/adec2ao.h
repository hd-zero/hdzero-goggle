#ifndef __ADEC2AO_H_
#define __ADEC2AO_H_

#include <stdbool.h>
#include <plat_type.h>
#include <mm_common.h>
#include <mpi_sys.h>
#include <mm_comm_adec.h>
#include <mm_comm_aio.h>
#include <mpi_adec.h>
#include <mpi_ao.h>
#include <mpi_clock.h>
#include <ClockCompPortIndex.h>
#include <pthread.h>

#ifndef BIT
#define BIT(x) (1<<(x))
#endif

#define ADEC2AO_adecEOF     BIT(2)
#define ADEC2AO_aoEOF       BIT(3)

#define ADEC2AO_adecEOF_N   BIT(18)
#define ADEC2AO_aoEOF_N     BIT(19)

typedef struct pcmFrameNode {
    AUDIO_FRAME_S frame;
    struct list_head mList;
} pcmFrameNode;

typedef struct
{
    PAYLOAD_TYPE_E codecType;
    int         sampleRate;
    int         channels;
    int         bitsPerSample;

    DEMUX_CHN   dmxChn;
    CLOCK_CHN   clkChn;
} AdecParams_t;

typedef struct
{
    DEMUX_CHN   dmxChn;
    ADEC_CHN    adecChn;
    int         aoDev;
    AO_CHN      aoChn;
    CLOCK_CHN   clkChn;

    uint32_t    flagEOF;
} Adec2AoContext_t;

Adec2AoContext_t* adec2ao_initSys(void);
void adec2ao_deinitSys(Adec2AoContext_t* aa);

ERRORTYPE adec2ao_prepare(Adec2AoContext_t* aa, AdecParams_t* params);
ERRORTYPE adec2ao_start(Adec2AoContext_t* aa);
ERRORTYPE adec2ao_stop(Adec2AoContext_t* aa);
ERRORTYPE adec2ao_seekTo(Adec2AoContext_t *aa);
ERRORTYPE adec2ao_pause(Adec2AoContext_t *aa);
ERRORTYPE adec2ao_checkEof(Adec2AoContext_t *aa);
ERRORTYPE adec2ao_setAdecEof(Adec2AoContext_t* aa);
bool      adec2ao_isEOF(Adec2AoContext_t *aa);

#endif  /* __AI2AENC_H_ */


#ifndef __AWDMX_H_
#define __VDEC2VO_H_

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

#define MAX_FILE_PATH_LEN  (128)

#define AUDIO_defChannels   2
#define AUDIO_defSampleRate 48000
#define AUDIO_defSampleBits 16
#define AUDIO_defCodecType  PT_AACLC

typedef void (*CB_onDmxEof)(void* context);

typedef struct
{
    char srcFile[MAX_FILE_PATH_LEN];
    int  srcFd;
    int  seekTime;
    bool bEof;
    uint32_t msDuration; //unit: ms

    DEMUX_CHN dmxChn;
    CLOCK_CHN clkChn;

    int       videoNum;
    uint16_t  width;
    uint16_t  height;
    PAYLOAD_TYPE_E codecType;

    int       audioNum;
    uint16_t  channels;
    uint16_t  bitsPerSample;
    uint16_t  sampleRate;
    PAYLOAD_TYPE_E aCodecType;

    CB_onDmxEof cbOnEof;
    void*     cbOnEofContext;
} AwdmxContext_t;

AwdmxContext_t* awdmx_open(char* sFile, CB_onDmxEof cbOnEof, void* context);
void      awdmx_close(AwdmxContext_t* dmxCtx);
ERRORTYPE awdmx_start(AwdmxContext_t *dmxCtx);
ERRORTYPE awdmx_pause(AwdmxContext_t *dmxCtx);
ERRORTYPE awdmx_stop(AwdmxContext_t *dmxCtx);
ERRORTYPE awdmx_seekTo(AwdmxContext_t *dmxCtx, int seekTime);
bool      awdmx_isEOF(AwdmxContext_t *dmxCtx);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif //__AWDMX_H_


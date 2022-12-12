#ifndef __AWDMX_H_
#define __VDEC2VO_H_

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

#define MAX_FILE_PATH_LEN  (128)

typedef struct
{
    char srcFile[MAX_FILE_PATH_LEN];
    int  srcFd;
    int  seekTime;
    bool bEof;
    uint32_t msDuration; //unit: ms

    DEMUX_CHN dmxChn;
    VDEC_CHN  vdecChn;
    CLOCK_CHN clkChn;

    int       videoNum;
    uint16_t  width;
    uint16_t  height;
    PAYLOAD_TYPE_E codecType;
} AwdmxContext_t;

AwdmxContext_t* awdmx_open(char* sFile);
void      awdmx_close(AwdmxContext_t* dmxCtx);
ERRORTYPE awdmx_bindVdecAndClock(AwdmxContext_t* dmxCtx, VDEC_CHN vdecChn, CLOCK_CHN clkChn);
ERRORTYPE awdmx_start(AwdmxContext_t *dmxCtx);
ERRORTYPE awdmx_pause(AwdmxContext_t *dmxCtx);
ERRORTYPE awdmx_stop(AwdmxContext_t *dmxCtx);
ERRORTYPE awdmx_seekTo(AwdmxContext_t *dmxCtx, int seekTime);
bool      awdmx_isEOF(AwdmxContext_t *dmxCtx);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif //__AWDMX_H_


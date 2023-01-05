#ifndef __VDEC2VO_H_
#define __VDEC2VO_H_

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

#include <hwdisplay.h>
#include <mpi_clock.h>
#include <mpi_sys.h>
#include <mpi_vdec.h>
#include <mpi_vo.h>

#define BIT(x) (1 << (x))

#define VDEC2VO_vdecEOF BIT(0)
#define VDEC2VO_voEOF   BIT(1)

#define VDEC2VO_vdecEOF_N BIT(16)
#define VDEC2VO_voEOF_N   BIT(17)

typedef struct
{
    PAYLOAD_TYPE_E codecType;
    uint16_t width;
    uint16_t height;
} VdecParams_t;

typedef struct
{
    VO_INTF_TYPE_E intfType;
    VO_INTF_SYNC_E intfSync;
    uint16_t width;
    uint16_t height;
    uint16_t uiChn;
} VoParams_t;

typedef struct
{
    PIXEL_FORMAT_E pixelFormat;
    int initRotation;
    VdecParams_t vdec;
    VoParams_t vo;

    DEMUX_CHN dmxChn;
    CLOCK_CHN clkChn;
} Vdec2VoParams_t;

typedef struct
{
    VDEC_CHN vdecChn;

    VO_DEV voDev;
    VO_LAYER voLayer;
    VO_CHN voChn;
    int uiLayer;

    uint32_t flagEOF;
    Vdec2VoParams_t params;
} Vdec2VoContext_t;

Vdec2VoContext_t *vdec2vo_initSys(void);
void vdec2vo_deinitSys(Vdec2VoContext_t *vvCtx);
ERRORTYPE vdec2vo_prepare(Vdec2VoContext_t *vvCtx, Vdec2VoParams_t *params);
ERRORTYPE vdec2vo_seekTo(Vdec2VoContext_t *vvCtx);
ERRORTYPE vdec2vo_stop(Vdec2VoContext_t *vvCtx);
ERRORTYPE vdec2vo_pause(Vdec2VoContext_t *vvCtx);
ERRORTYPE vdec2vo_start(Vdec2VoContext_t *vvCtx);
ERRORTYPE vdec2vo_currentMediaTime(Vdec2VoContext_t *vvCtx, int *mediaTime);
ERRORTYPE vdec2vo_checkEof(Vdec2VoContext_t *vvCtx);
ERRORTYPE vdec2vo_setVdecEof(Vdec2VoContext_t *vvCtx);
bool vdec2vo_isEOF(Vdec2VoContext_t *vvCtx);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif //__VDEC2VO_H_

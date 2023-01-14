#ifndef __MPI_TENC_H__ 
#define __MPI_TENC_H__ 

//ref platform headers
#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

//media api headers to app
#include "mm_common.h"
#include "mm_comm_tenc.h"

#ifdef __cplusplus
extern "C" {
#endif


ERRORTYPE AW_MPI_TENC_CreateChn(TENC_CHN TeChn, const TENC_CHN_ATTR_S *pAttr);
ERRORTYPE AW_MPI_TENC_DestroyChn(TENC_CHN TeChn);

ERRORTYPE AW_MPI_TENC_ResetChn(TENC_CHN TeChn);

ERRORTYPE AW_MPI_TENC_RegisterCallback(TENC_CHN TeChn, MPPCallbackInfo *pCallback);

ERRORTYPE AW_MPI_TENC_SetChnAttr(TENC_CHN TeChn, const TENC_CHN_ATTR_S *pAttr);
ERRORTYPE AW_MPI_TENC_GetChnAttr(TENC_CHN TeChn, TENC_CHN_ATTR_S *pAttr);

int AW_MPI_TENC_GetHandle(TENC_CHN TeChn);

ERRORTYPE AW_MPI_TENC_StartRecvText(TENC_CHN TeChn);
ERRORTYPE AW_MPI_TENC_StopRecvText(TENC_CHN TeChn);

/*-1:bolck  0:nonblock   >0 : overtime   */
/* nMilliSec: -1 is block, 0 is no block, other positive number is timeout */
ERRORTYPE AW_MPI_TENC_SendFrame(TENC_CHN TeChn, TEXT_FRAME_S *pFrame);
ERRORTYPE AW_MPI_TENC_GetStream(TENC_CHN TeChn, TEXT_STREAM_S *pStream, int nMilliSec);
ERRORTYPE AW_MPI_TENC_ReleaseStream(TENC_CHN TeChn, TEXT_STREAM_S *pStream); 



#ifdef __cplusplus
}
#endif 

#endif

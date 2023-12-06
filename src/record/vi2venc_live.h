#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "vi2venc.h"

Vi2Venc_t* vi2live_initSys(CB_onFrame cbOnFrame, void* context);
void       vi2live_deinitSys(Vi2Venc_t* vv);
bool       vi2live_started(Vi2Venc_t* vv);

#ifdef __cplusplus
}
#endif

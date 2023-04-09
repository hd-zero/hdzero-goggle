#ifndef __VI2VENC_LIVE_H_
#define __VI2VENC_LIVE_H_

#include "vi2venc.h"

Vi2Venc_t* vi2live_initSys(CB_onFrame cbOnFrame, void* context);
void       vi2live_deinitSys(Vi2Venc_t* vv);
bool       vi2live_started(Vi2Venc_t* vv);

#endif  /* __VI2VENC_LIVE_H_ */


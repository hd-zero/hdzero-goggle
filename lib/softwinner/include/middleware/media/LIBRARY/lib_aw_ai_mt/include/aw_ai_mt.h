#ifndef __AW_AI_MT_H__
#define __AW_AI_MT_H__

#include "aw_ai_mt_def.h"

AW_AI_API  AW_AI_HANDLE  AW_AI_MT_Init(LP_MT_Params pMTParams);

AW_AI_API int AW_AI_MT_SendFrame(AW_AI_HANDLE hHandle, LP_IMAGE_DATA_I pImageIn);

AW_AI_API int AW_AI_MT_SetFrame(AW_AI_HANDLE hHandle, const char *pFramePath);

AW_AI_API int AW_AI_MT_GenMotionTrail(AW_AI_HANDLE hHandle, LP_IMAGE_DATA_I pImageOut);

AW_AI_API int AW_AI_MT_MotionMerge(LP_IMAGE_DATA_I pImageSrc1, LP_IMAGE_DATA_I pImageSrc2, LP_IMAGE_DATA_I pImageDst);

AW_AI_API int AW_AI_MT_Reset(AW_AI_HANDLE hHandle);

AW_AI_API int AW_AI_MT_UnInit(AW_AI_HANDLE hHandle);

#endif // __AW_AI_MT_H__

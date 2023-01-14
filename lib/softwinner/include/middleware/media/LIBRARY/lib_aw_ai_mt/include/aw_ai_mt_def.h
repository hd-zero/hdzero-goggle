#ifndef __AW_AI_MT_DEF_H__
#define __AW_AI_MT_DEF_H__

#include "aw_ai_core_def.h"

#define MT_PROCESSING_HEIGHT  (540)
#define MT_PROCESSING_WIDTH   (960)

#if ( defined _WIN64 || defined _WIN32 )
//#ifdef AW_AI_MT_EXPORTS
//#define AW_AI_API __declspec(dllexport)
//#else
//#define AW_AI_API __declspec(dllimport)
//#endif
//#else
#define AW_AI_API
#endif

typedef struct _MT_Params
{
	int Width;
	int Height;
	bool bAntishakeEnable;
}MT_Params, *LP_MT_Params;

#endif // __AW_AI_MT_DEF_H__

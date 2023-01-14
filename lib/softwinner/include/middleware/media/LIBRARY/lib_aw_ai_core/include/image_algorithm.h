#ifndef __IMAGE_ALGORITHM__
#define __IMAGE_ALGORITHM__

#include "aw_ai_core_def.h"

int AW_AI_API Image_GoodFeaturesToTrack(const LP_IMAGE_DATA_I pSrcImage, void* pFeatureParam);

int AW_AI_API Image_getPerspectiveTransform(std::vector<POINT_F> SrcPoints, std::vector<POINT_F> DstPoints, float *pWrapMatrix);

int AW_AI_API Image_FindHomography(LP_POINT_F pSrcPoints, LP_POINT_F pDstPoints, int iCount, float *pWrapMatrix, float fDistThresh = 5.0);

int AW_AI_API Image_WarpPerspective(const LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage, double *pMatrix);

#endif
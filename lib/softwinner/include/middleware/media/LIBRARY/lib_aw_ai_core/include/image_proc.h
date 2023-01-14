#include "aw_ai_core_def.h"

// 灰度图像二值化运算
int AW_AI_API Image_Threshold(const LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage, uint8_t iThresh, uint8_t iMaxValue);

// 二值图像腐蚀运算
int AW_AI_API Image_Erode(const LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage, int WinSize);

// 二值图像膨胀运算
int AW_AI_API Image_Dilate(const LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage, int WinSize);

// 二值图像与运算
int AW_AI_API Image_BitWise_And(const LP_IMAGE_DATA_I pSrcImage1, const LP_IMAGE_DATA_I pSrcImage2, LP_IMAGE_DATA_I pDstImage);

// 灰度图像减运算
int AW_AI_API Image_AbsDiff(const LP_IMAGE_DATA_I pSrcImage1, const LP_IMAGE_DATA_I pSrcImage2, LP_IMAGE_DATA_I pDstImage);

// 连通域标记
int AW_AI_API Image_Find_Boxes(const LP_IMAGE_DATA_I pSrcImage, std::list<BOX_T>& BoxList);

// 图像缩放
int AW_AI_API Image_Resize(const LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage);

// 高斯平滑
int AW_AI_API Image_GaussianBlur(const LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage, int Ksize, float sigma);

// 矩阵求逆
int AW_AI_API Matrix_Invert(double *pMatrix, double *pMatrixInverse, const int rank);


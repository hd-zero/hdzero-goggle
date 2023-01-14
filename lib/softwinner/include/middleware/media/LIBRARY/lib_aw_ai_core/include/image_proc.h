#include "aw_ai_core_def.h"

// �Ҷ�ͼ���ֵ������
int AW_AI_API Image_Threshold(const LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage, uint8_t iThresh, uint8_t iMaxValue);

// ��ֵͼ��ʴ����
int AW_AI_API Image_Erode(const LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage, int WinSize);

// ��ֵͼ����������
int AW_AI_API Image_Dilate(const LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage, int WinSize);

// ��ֵͼ��������
int AW_AI_API Image_BitWise_And(const LP_IMAGE_DATA_I pSrcImage1, const LP_IMAGE_DATA_I pSrcImage2, LP_IMAGE_DATA_I pDstImage);

// �Ҷ�ͼ�������
int AW_AI_API Image_AbsDiff(const LP_IMAGE_DATA_I pSrcImage1, const LP_IMAGE_DATA_I pSrcImage2, LP_IMAGE_DATA_I pDstImage);

// ��ͨ����
int AW_AI_API Image_Find_Boxes(const LP_IMAGE_DATA_I pSrcImage, std::list<BOX_T>& BoxList);

// ͼ������
int AW_AI_API Image_Resize(const LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage);

// ��˹ƽ��
int AW_AI_API Image_GaussianBlur(const LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage, int Ksize, float sigma);

// ��������
int AW_AI_API Matrix_Invert(double *pMatrix, double *pMatrixInverse, const int rank);


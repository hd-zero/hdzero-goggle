#include "aw_ai_core_def.h"

// BGR2YUV
int AW_AI_API cvtBGR2YUV(LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage, AW_PIXEL_FORMAT_E eType);

// YUV2BGR
int AW_AI_API cvtYUV2BGR(LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage);

// GBR2GRAY
int AW_AI_API cvtBGR2GRAY(LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage);

// YUV2GRAY
int AW_AI_API cvtYUV2GRAY(LP_IMAGE_DATA_I pSrcImage, LP_IMAGE_DATA_I pDstImage);

// ����BMPͼƬ
int AW_AI_API SaveBMP(LP_IMAGE_DATA_I pImage, char* szPath);

// ����BMPͼƬ
int AW_AI_API LoadBMP(char* szPath, LP_BMP_PIC_T pBMPImage);

// �ͷ�BMPͼƬ
int AW_AI_API ReleaseBMP(LP_BMP_PIC_T pBMPImage);

// ��ȡBMPͼƬ�ߴ�
int AW_AI_API GetBMPSize(char* szPath, int* pWidth, int* pHeight);

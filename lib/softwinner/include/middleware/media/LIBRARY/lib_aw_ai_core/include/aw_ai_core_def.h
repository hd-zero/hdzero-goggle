#ifndef __AW_CV_DEF__
#define __AW_CV_DEF__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h> 
#include <limits.h>
#include <float.h>

#ifdef ANDROID
#include <android/log.h>
#define LOG_TAG "LIB-AW-AI-XXXXXXXXX"
#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG ,  LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO  ,  LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN  ,  LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__)
#endif

#if ( defined _WIN64 || defined _WIN32 )
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <cmath>

#pragma pack(push)
#pragma pack(2)

#define AW_AI_PATH_LEN		(260)
#define AW_AI_NAME_LEN		(256)

#define ARRAY_SIZE(arr)     ( sizeof(arr) / sizeof((arr)[0]) )

#if ( defined _WIN64 || defined _WIN32 )
//#ifdef AW_AI_CORE_EXPORTS
//#define AW_AI_API __declspec(dllexport)
//#else
//#define AW_AI_API __declspec(dllimport)
//#endif
#define AW_AI_API
#else
#define AW_AI_API
#endif

#ifndef AW_AI_HANDLE
typedef void*  AW_AI_HANDLE;
#endif

typedef enum _AW_PIXEL_FORMAT_E
{
	PIXEL_FORMAT_UNKNOWN = 0,

	PIXEL_FORMAT_BGR_888 = 10,
	PIXEL_FORMAT_BGRA_8888,

	PIXEL_FORMAT_YUV_PLANAR_420 = 20,	//YU12
	PIXEL_FORMAT_YVU_PLANAR_420,		//YV12
	PIXEL_FORMAT_YUV_SEMIPLANAR_420,	//NV12
	PIXEL_FORMAT_YVU_SEMIPLANAR_420,	//NV21
}AW_PIXEL_FORMAT_E;

typedef enum _FILE_FORMAT_E
{
	FILE_FORMAT_DEFAULT = 0,
	FILE_FORMAT_MP4 = 0,
	FILE_FORMAT_TS,
	FILE_FORMAT_JPEG,
	FILE_FORMAT_RAW,
	FILE_FORMAT_UNKNOWN,
}FILE_FORMAT_E;

typedef struct _IMAGE_DATA_I
{
	AW_PIXEL_FORMAT_E mType;
	unsigned int   mWidth;
	unsigned int   mHeight;
	unsigned int   mChannel;
	unsigned char* mpData;
}IMAGE_DATA_I, *LP_IMAGE_DATA_I;

typedef struct _POINT_I
{
	unsigned int x;
	unsigned int y;
}POINT_I, *LP_POINT_I;

typedef struct _POINT_F
{
	float x;
	float y;
}POINT_F, *LP_POINT_F;

typedef struct _PIXEL_POS_T
{
	unsigned int row;
	unsigned int col;
}PIXEL_POS_T, *LP_PIXEL_POS_T;

typedef struct _BOX_T
{
	unsigned int left;
	unsigned int top;
	unsigned int right;
	unsigned int bottom;
}BOX_T, *LP_BOX_T;

typedef struct _BMP_FILE_HEADER_T
{
	uint16_t  bfType;
	uint32_t  bfSize;
	uint16_t  bfReserved1;
	uint16_t  bfReserved2;
	uint32_t  bfOffBits;
}BMP_FILE_HEADER_T; //14

typedef struct _BMP_INFO_HEADER_T
{
	uint32_t  biSize;
	int32_t   biWidth;
	int32_t   biHeight;
	uint16_t  biPlanes;
	uint16_t  biBitCount;
	uint32_t  biCompression;
	uint32_t  biSizeImage;
	int32_t   biXPelsPerMeter;
	int32_t   biYPelsPerMeter;
	uint32_t  biClrUsed;
	uint32_t  biClrImportant;
}BMP_INFO_HEADER_T; //40

typedef struct BMP_PIC_T
{
	BMP_FILE_HEADER_T  stFileHeader;
	BMP_INFO_HEADER_T  stInfoHeader;
	IMAGE_DATA_I       stImage;
}BMP_PIC_T, *LP_BMP_PIC_T;

#define AW_BUF_DUMP(buf, len) \
    do { \
        char *_buf = (char *)buf;\
        char str[1024] = {0};\
        unsigned int index = 0, _len;\
        _len = (unsigned int)len;\
        snprintf(str, 1024, ":%d:[", _len);\
        for (index = 0; index < _len; index++)\
        {\
            snprintf(str + strlen(str), 1024 - strlen(str), "%02hhx ", _buf[index]);\
        }\
        str[strlen(str) - 1] = ']';\
        printf("%s", str);\
    }while (0)

inline void CreateImage(LP_IMAGE_DATA_I pImage, unsigned int iWidth, unsigned int iHeight, unsigned int iChannel = 3, AW_PIXEL_FORMAT_E eType = PIXEL_FORMAT_UNKNOWN)
{
	assert(iChannel == 1 || iChannel == 3);

	pImage->mType = eType;
	pImage->mWidth = iWidth;
	pImage->mHeight = iHeight;
	pImage->mChannel = iChannel;
	pImage->mpData = (unsigned char*)malloc(iWidth * iHeight * iChannel);
	memset(pImage->mpData, 0, iWidth * iHeight * iChannel);
	return;
}

inline void CopyImage(LP_IMAGE_DATA_I pImageSrc, LP_IMAGE_DATA_I pImageDst)
{
	assert(pImageSrc->mpData != NULL && pImageDst->mpData != NULL);
	assert(pImageSrc->mWidth == pImageDst->mWidth);
	assert(pImageSrc->mHeight == pImageDst->mHeight);
	assert(pImageSrc->mChannel == pImageDst->mChannel);

	pImageDst->mType    = pImageSrc->mType;
	pImageDst->mWidth   = pImageSrc->mWidth;
	pImageDst->mHeight  = pImageSrc->mHeight;
	pImageDst->mChannel = pImageSrc->mChannel;
	memcpy(pImageDst->mpData, pImageSrc->mpData, pImageSrc->mWidth * pImageSrc->mHeight * pImageSrc->mChannel);
	return;
}

inline void DestroyImage(LP_IMAGE_DATA_I pImage)
{
	pImage->mType = PIXEL_FORMAT_UNKNOWN;
	pImage->mWidth = 0;
	pImage->mHeight = 0;
	pImage->mChannel = 0;
	if (pImage->mpData)
	{
		free(pImage->mpData);
		pImage->mpData = NULL;
	}
}

inline unsigned char GetPixel(LP_IMAGE_DATA_I pImage, int row, int col, int channel = 0)
{
	assert(row >= 0 && row < (int)pImage->mHeight);
	assert(col >= 0 && col < (int)pImage->mWidth);
	assert(channel >= 0 && channel < (int)pImage->mChannel);
	return pImage->mpData[row * pImage->mWidth * pImage->mChannel + col * pImage->mChannel + channel];
}

inline void SetPixel(LP_IMAGE_DATA_I pImage, int row, int col, int channel, unsigned char value)
{
	assert(row >= 0 && row < (int)pImage->mHeight);
	assert(col >= 0 && col < (int)pImage->mWidth);
	assert(channel >= 0 && channel < (int)pImage->mChannel);
	pImage->mpData[row * pImage->mWidth * pImage->mChannel + col * pImage->mChannel + channel] = value;
}

#pragma pack(pop)
#endif


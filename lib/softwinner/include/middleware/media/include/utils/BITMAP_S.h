#ifndef _BITMAP_S_H_
#define _BITMAP_S_H_

#include <plat_type.h>
#include <mm_comm_video.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum BMP_FLIP_FLAG_E {
    BMP_FLIP_NONE       = 0,
    BMP_FLIP_HFLIP      = 1<<0,
    BMP_FLIP_VFLIP      = 1<<1,
    BMP_FLIP_BOTH_FLIP  = (BMP_FLIP_HFLIP | BMP_FLIP_VFLIP),
    BMP_FLIP_BUTT,
}BMP_FLIP_FLAG_E;

int BITMAP_S_GetdataSize(PARAM_IN const BITMAP_S *pBitmap);
ERRORTYPE BITMAP_S_FlipData(PARAM_INOUT BITMAP_S *pBitmap, PARAM_IN BMP_FLIP_FLAG_E flip_dir);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _BITMAP_S_H_ */


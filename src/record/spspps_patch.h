#ifndef __SPSPPS_PATCH_H_
#define __SPSPPS_PATCH_H_

int H264_Modify_SPS(uint8_t* src, int srcSize, int fps);
int H265_Modify_SPS(uint8_t* src, int srcSize, int fps);

#endif /* __SPSPPS_PATCH_H_ */

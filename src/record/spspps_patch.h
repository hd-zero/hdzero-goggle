#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int H264_Modify_SPS(uint8_t* src, int srcSize, int fps);
int H265_Modify_SPS(uint8_t* src, int srcSize, int fps);

#ifdef __cplusplus
}
#endif

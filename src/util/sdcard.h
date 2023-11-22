#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define SD_BLOCK_DEVICE "/dev/mmcblk0"

bool sdcard_mounted();

bool sdcard_inserted();

#ifdef __cplusplus
}
#endif

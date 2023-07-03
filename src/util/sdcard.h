#pragma once

#include <stdbool.h>

#define SD_BLOCK_DEVICE "/dev/mmcblk0"

bool sdcard_mounted();

bool sdcard_inserted();
/*
 * include/linux/ion_sunxi.h
 *
 * Copyright(c) 2015-2020 Allwinnertech Co., Ltd.
 *      http://www.allwinnertech.com
 *
 * Author: Wim Hwang<haungwei@allwinnertech.com>
 *
 * sunxi ion header file
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _INCLUDE_LINUX_ION_SUNXI_H
#define _INCLUDE_LINUX_ION_SUNXI_H

#include "../../drivers/staging/android/ion/sunxi/sunxi_ion.h"
#include "../../drivers/staging/android/ion/ion_priv.h"

struct ion_mem_reserve_info {
	u32 size;
	u32 start;	/*physical start address*/
};

#endif

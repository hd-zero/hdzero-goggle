/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : VideoVirViCompPortIndex.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/01/20
  Last Modified :
  Description   : mpp component implement
  Function List :
  History       :
******************************************************************************/
#ifndef _VIDEOVIRVICOMPPORTINDEX_H_
#define _VIDEOVIRVICOMPPORTINDEX_H_

typedef enum VI_CHANNEL_PORT_DEFINE_E {
	VI_CHN_PORT_INDEX_CAP_IN = 0,
	VI_CHN_PORT_INDEX_FILE_IN, /* FILE_IN : Decode H26x,mpeg data, or yuv file input*/
	VI_CHN_PORT_INDEX_OUT,
	VI_CHN_MAX_PORTS,
} VI_CHANNEL_PORT_DEFINE_E;

#endif  /* _VIDEOVIRVICOMPPORTINDEX_H_ */


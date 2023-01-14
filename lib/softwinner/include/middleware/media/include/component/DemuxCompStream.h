/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : DemuxCompStream.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/08/12
  Last Modified :
  Description   : common struct definition in VdecComp and MPIVdec.
  Function List :
  History       :
******************************************************************************/
#ifndef _DEMUXCOMPSTREAM_H_
#define _DEMUXCOMPSTREAM_H_

#include <plat_type.h>
#include <EncodedStream.h>

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */


typedef struct dmxpkt_node_t
{
	EncodedStream stEncodedStream;
	struct list_head mList;
}DMXPKT_NODE_T;


typedef struct DemuxStream {
    EncodedStream *pEncodedStream;
    int nMilliSec;
} DemuxStream;


#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif  /* _DEMUXCOMPSTREAM_H_ */


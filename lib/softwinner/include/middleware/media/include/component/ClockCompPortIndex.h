/*******************************************************************************
--                                                                            --
--                    CedarX Multimedia Framework                             --
--                                                                            --
--          the Multimedia Framework for Linux/Android System                 --
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                         Softwinner Products.                               --
--                                                                            --
--                   (C) COPYRIGHT 2011 SOFTWINNER PRODUCTS                   --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
*******************************************************************************/

#ifndef _CLOCKCOMPPORTINDEX_H_
#define _CLOCKCOMPPORTINDEX_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum CLOCK_COMP_PORT_INDEX{
	CLOCK_PORT_INDEX_AUDIO = 0, //to audio render
	CLOCK_PORT_INDEX_VIDEO = 1, //to video render
	CLOCK_PORT_INDEX_DEMUX = 2, //to demux
	CLOCK_PORT_INDEX_VDEC  = 3, //to vdec
	CLOCK_PORT_INDEX_SUBTITLE  = 4, //to subtitle render component.
}CLOCK_COMP_PORT_INDEX;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _CLOCKCOMPPORTINDEX_H_ */


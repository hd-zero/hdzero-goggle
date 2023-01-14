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

#ifndef _DEMUXTYPEMAP_H_
#define _DEMUXTYPEMAP_H_

#include <CdxParser.h>
#include <CDX_fileformat.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CDX_MEDIA_FILE_FORMAT ParserType2DemuxType(enum CdxParserTypeE parserType);
enum CdxParserTypeE DemuxType2ParserType(CDX_MEDIA_FILE_FORMAT demuxType);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _DEMUXTYPEMAP_H_ */


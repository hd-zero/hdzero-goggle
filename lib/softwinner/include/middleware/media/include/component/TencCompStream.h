#ifndef __TENCCOMPSTREAM_H__
#define __TENCCOMPSTREAM_H__


#include <plat_type.h>
#include <vencoder.h>
#include <mm_comm_tenc.h>


#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */


typedef struct TEncStream {
    TEXT_STREAM_S *pStream;
    int nMilliSec;
} TEncStream; 


#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */



#endif

#ifndef __MM_COMM_TENC_H__
#define __MM_COMM_TENC_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

///////////////////////////

#include "plat_type.h"
#include "plat_errno.h" 
#include "mm_common.h"

/* invlalid channel ID */
#define ERR_TENC_INVALID_CHNID     DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_TENC_ILLEGAL_PARAM     DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_TENC_EXIST             DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel exists */
#define ERR_TENC_UNEXIST           DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define ERR_TENC_NULL_PTR          DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define ERR_TENC_NOT_CONFIG        DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define ERR_TENC_NOT_SUPPORT       DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define ERR_TENC_NOT_PERM          DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define ERR_TENC_NOMEM             DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_TENC_NOBUF             DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define ERR_TENC_BUF_EMPTY         DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define ERR_TENC_BUF_FULL          DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define ERR_TENC_SYS_NOTREADY      DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* system is busy*/
#define ERR_TENC_BUSY              DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* component state is same as user wanted */
#define ERR_TENC_SAMESTATE         DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_TENC_INVALIDSTATE      DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_TENC_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_TENC_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_TENC, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)


typedef struct TEXT_STREAM_S
{ 
    unsigned char       *pStream;       /* the virtual address of stream */
    unsigned int        mLen;           /* stream lenth, by bytes */
    unsigned long long  mTimeStamp;     /* frame time stamp */
    int                 mId;            /* frame id */
} TEXT_STREAM_S;

typedef struct TEXT_FRAME_S
{
    char                mpAddr[200];
    unsigned long long  mTimeStamp;                
    unsigned int        mLen;                      
    unsigned int        mId;
} TEXT_FRAME_S;

typedef struct TEXTINFO_t
{
    int enc_enable_type;                // bit0-gps; bit1-gsensor; bit2-adas; bit3-driver_id
    int text_enc_type;                  // CODEC_ID_GGAD
    int video_frame_rate;
    int gsensor_rate;       // set its value according to video_frame_rate
    int adas_rate;
}TEXTINFO_t;

typedef struct TENC_CHN_ATTR_S
{
    TEXTINFO_t tInfo;                           /*the attribution of audio encoder*/
}TENC_CHN_ATTR_S;



//////////////////////////

#ifdef __cplusplus
}
#endif /* __cplusplus */ 

#endif

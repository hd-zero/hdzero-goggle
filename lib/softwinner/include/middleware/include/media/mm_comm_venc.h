/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_venc.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : common struct definition for VENC
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_COMM_VENC_H__
#define __IPCLINUX_COMM_VENC_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "plat_type.h"
#include "plat_errno.h"

#include "mm_common.h"
#include "mm_comm_video.h"
#include "mm_comm_rc.h"

#include <linux/videodev2.h>

/* invlalid channel ID */
#define ERR_VENC_INVALID_CHNID     DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_VENC_ILLEGAL_PARAM     DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_VENC_EXIST             DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel exists */
#define ERR_VENC_UNEXIST           DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define ERR_VENC_NULL_PTR          DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define ERR_VENC_NOT_CONFIG        DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define ERR_VENC_NOT_SUPPORT       DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define ERR_VENC_NOT_PERM          DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define ERR_VENC_NOMEM             DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_VENC_NOBUF             DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define ERR_VENC_BUF_EMPTY         DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define ERR_VENC_BUF_FULL          DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define ERR_VENC_SYS_NOTREADY      DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
/* system is busy*/
#define ERR_VENC_BUSY              DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
/* encode timeout */
#define ERR_VENC_TIMEOUT           DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_TIMEOUT)
/* component state is same as user wanted */
#define ERR_VENC_SAMESTATE         DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_VENC_INVALIDSTATE      DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_VENC_INCORRECT_STATE_TRANSITION DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_VENC_INCORRECT_STATE_OPERATION DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)
/* efuse error. */
#define ERR_VENC_EFUSE_ERROR DEF_ERR(MOD_ID_VENC, EN_ERR_LEVEL_ERROR, EN_ERR_EFUSE_ERROR)

/*the nalu type of H264E*/
typedef enum H264E_NALU_TYPE_E
{
     H264E_NALU_PSLICE = 1,                         /*PSLICE types*/
     H264E_NALU_ISLICE = 5,                         /*ISLICE types*/
     H264E_NALU_SEI    = 6,                         /*SEI types*/
     H264E_NALU_SPS    = 7,                         /*SPS types*/
     H264E_NALU_PPS    = 8,                         /*PPS types*/
     H264E_NALU_IPSLICE = 9,
     H264E_NALU_BUTT        
} H264E_NALU_TYPE_E;

/*the nalu type of H265E*/
typedef enum H265E_NALU_TYPE_E
{
	 H265E_NALU_PSLICE = 1,                         /*P SLICE types*/
	 H265E_NALU_ISLICE = 19,                         /*I SLICE types*/
     H265E_NALU_VPS    = 32,                         /*VPS types*/
     H265E_NALU_SPS    = 33,                         /*SPS types*/
     H265E_NALU_PPS    = 34,                         /*PPS types*/
     H265E_NALU_SEI    = 39,                         /*SEI types*/
     
     H265E_NALU_BUTT        
} H265E_NALU_TYPE_E;

/*the reference type of H264E slice*/
typedef enum H264E_REFSLICE_TYPE_E
{
     H264E_REFSLICE_FOR_1X = 1,                     /*Reference slice for H264E_REF_MODE_1X*/
     H264E_REFSLICE_FOR_2X = 2,                     /*Reference slice for H264E_REF_MODE_2X*/
     H264E_REFSLICE_FOR_4X = 5,                     /*Reference slice for H264E_REF_MODE_4X*/
     H264E_REFSLICE_FOR_BUTT                        /* slice not for reference*/          
} H264E_REFSLICE_TYPE_E;

/*the pack type of JPEGE*/
typedef enum JPEGE_PACK_TYPE_E
{
     JPEGE_PACK_ECS = 5,                            /*ECS types*/
     JPEGE_PACK_APP = 6,                            /*APP types*/
     JPEGE_PACK_VDO = 7,                            /*VDO types*/
     JPEGE_PACK_PIC = 8,                            /*PIC types*/
     JPEGE_PACK_BUTT
} JPEGE_PACK_TYPE_E;

/*the pack type of MPEG4*/
typedef enum MPEG4E_PACK_TYPE_E
{
     MPEG4E_PACK_VOP_P = 1,                         /*P VOP packet types*/
     MPEG4E_PACK_VOP_I = 5,                         /*I VOP packet types*/
     MPEG4E_PACK_VOS   = 6,                         /*VOS types*/
     MPEG4E_PACK_VO    = 7,                         /*VO types*/
     MPEG4E_PACK_VOL   = 8,                         /*VOL types*/
     MPEG4E_PACK_GVOP  = 9                          /*GROUP of vop types */
} MPEG4E_PACK_TYPE_E;

/*the data type of VENC*/
typedef union VENC_DATA_TYPE_U
{
    H264E_NALU_TYPE_E    enH264EType;               /*H264E NALU types*/
    JPEGE_PACK_TYPE_E    enJPEGEType;               /*JPEGE pack types*/
    MPEG4E_PACK_TYPE_E   enMPEG4EType;              /*MPEG4E pack types*/
    H265E_NALU_TYPE_E    enH265EType;               /*H264E NALU types*/
}VENC_DATA_TYPE_U;

typedef struct VENC_PACK_INFO_S
{
	VENC_DATA_TYPE_U  mPackType;
    unsigned int mPackOffset;                          
    unsigned int mPackLength;                        
}VENC_PACK_INFO_S;


typedef struct VENC_PACK_S
{
    //unsigned int   mPhyAddr;                         /*the physics address of stream*/
    unsigned char   *mpAddr0;                            /*the virtual address of stream*/
    unsigned char   *mpAddr1;
    unsigned char   *mpAddr2;   //for jpeg encoder, jpeg encoder may use three buffer to store mainPicture and thumbPicture.
    unsigned int   mLen0;                             /*the length of stream*/
    unsigned int   mLen1;
    unsigned int   mLen2;
    
    uint64_t   mPTS;                                /*PTS*/
    BOOL  mbFrameEnd;                             /*frame end£¿*/
    
    VENC_DATA_TYPE_U  mDataType;                     /*the type of stream*/
    unsigned int   mOffset;

    unsigned int mDataNum;
    VENC_PACK_INFO_S mPackInfo[8];
}VENC_PACK_S;

typedef enum H264E_REF_TYPE_E
{
    BASE_IDRSLICE = 0,                              //the Idr frame at Base layer
    BASE_PSLICE_REFBYBASE,                          //the P frame at Base layer, referenced by other frames at Base layer
    BASE_PSLICE_REFBYENHANCE,                       //the P frame at Base layer, referenced by other frames at Enhance layer
    ENHANCE_PSLICE_REFBYENHANCE,                    //the P frame at Enhance layer, referenced by other frames at Enhance layer
    ENHANCE_PSLICE_NOTFORREF,                       //the P frame at Enhance layer ,not referenced
    ENHANCE_PSLICE_BUTT
} H264E_REF_TYPE_E;

typedef enum H264E_REF_TYPE_E H265E_REF_TYPE_E;

typedef struct VENC_STREAM_INFO_H264_S
{    
    unsigned int u32PicBytesNum;                          /* the coded picture stream byte number */
    unsigned int u32PSkipMbNum;                           /* the skip macroblock num */
    unsigned int u32IpcmMbNum;                            /* the ipcm macroblock num */
    unsigned int u32Inter16x8MbNum;                       /* the inter16x8 macroblock num */
    unsigned int u32Inter16x16MbNum;                      /* the inter16x16 macroblock num */
    unsigned int u32Inter8x16MbNum;                       /* the inter8x16 macroblock num */
    unsigned int u32Inter8x8MbNum;                        /* the inter8x8 macroblock num */
    unsigned int u32Intra16MbNum;                         /* the intra16x16 macroblock num */
    unsigned int u32Intra8MbNum;                          /* the intra8x8 macroblock num */
    unsigned int u32Intra4MbNum;                          /* the inter4x4 macroblock num */

    H264E_REFSLICE_TYPE_E enRefSliceType;           /*the reference type of H264E slice*/
    H264E_REF_TYPE_E      enRefType;                /*Type of encoded frames in advanced frame skipping reference mode*/
    unsigned int u32UpdateAttrCnt;                        /*Number of times that channel attributes or parameters (including RC parameters) are set*/	
	unsigned int u32StartQp;								/*StartQP Value*/
}VENC_STREAM_INFO_H264_S;

typedef struct VENC_STREAM_INFO_H265_S
{    
    unsigned int u32PicBytesNum;                          /* the coded picture stream byte number */
    unsigned int u32Inter64x64CuNum;                            /* the  num */
    unsigned int u32Inter32x32CuNum;
    unsigned int u32Inter16x16CuNum;
    unsigned int u32Inter8x8CuNum;
    unsigned int u32Intra32x32CuNum;
    unsigned int u32Intra16x16CuNum;
    unsigned int u32Intra8x8CuNum;
    unsigned int u32Intra4x4CuNum;
    H265E_REF_TYPE_E      enRefType;                /*Type of encoded frames in advanced frame skipping reference mode*/
    unsigned int u32UpdateAttrCnt;                        /*Number of times that channel attributes or parameters (including RC parameters) are set*/
	unsigned int u32StartQp;								/*StartQP Value*/
}VENC_STREAM_INFO_H265_S;

typedef struct VENC_STREAM_INFO_JPEG_S
{
    unsigned int u32PicBytesNum;                          /* the coded picture stream byte number */
    unsigned int u32UpdateAttrCnt;                        /*Number of times that channel attributes or parameters (including RC parameters) are set*/
    unsigned int u32Qfactor;                              /* image quality */
}VENC_STREAM_INFO_JPEG_S;

typedef struct VENC_STREAM_INFO_MPEG4_S
{
    unsigned int u32PicBytesNum;                          /* the coded picture stream byte number */
    unsigned int u32UpdateAttrCnt;                        /*Number of times that channel attributes or parameters (including RC parameters) are set*/
}VENC_STREAM_INFO_MPEG4_S;

typedef struct VENC_STREAM_S
{
    VENC_PACK_S       *mpPack;                           /*stream pack attribute*/
    unsigned int      mPackCount;                       /*the pack number of one frame stream*/
    unsigned int      mSeq;                             /*the list number of stream*/

    union
    {
        VENC_STREAM_INFO_H264_S  mH264Info;         /*the stream info of h264*/
        VENC_STREAM_INFO_JPEG_S  mJpegInfo;         /*the stream info of jpeg*/
        VENC_STREAM_INFO_MPEG4_S mMpeg4Info;       /*the stream info of mpeg4*/
        VENC_STREAM_INFO_H265_S  mH265Info;        /*the stream info of h265*/
    };
}VENC_STREAM_S;

typedef enum H264_LEVEL_E {
    H264_LEVEL_1   = 10,     /**< Level 1 */
    H264_LEVEL_11  = 11,     /**< Level 1.1 */
    H264_LEVEL_12  = 12,     /**< Level 1.2 */
    H264_LEVEL_13  = 13,     /**< Level 1.3 */
    H264_LEVEL_2   = 20,     /**< Level 2 */
    H264_LEVEL_21  = 21,     /**< Level 2.1 */
    H264_LEVEL_22  = 22,     /**< Level 2.2 */
    H264_LEVEL_3   = 30,     /**< Level 3 */
    H264_LEVEL_31  = 31,     /**< Level 3.1 */
    H264_LEVEL_32  = 32,     /**< Level 3.2 */
    H264_LEVEL_4   = 40,     /**< Level 4 */
    H264_LEVEL_41  = 41,     /**< Level 4.1 */
    H264_LEVEL_42  = 42,     /**< Level 4.2 */
    H264_LEVEL_5   = 50,     /**< Level 5 */
    H264_LEVEL_51  = 51,     /**< Level 5.1 */
}H264_LEVEL_E;

typedef struct VENC_ATTR_H264_S
{
    unsigned int  MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    unsigned int  MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/

    unsigned int  BufSize;                             /*stream buffer size*/
    unsigned int  Profile;                             /*0: baseline; 1:MP; 2:HP; 3: SVC-T [0,3]; */          
    BOOL bByFrame;                               /*get stream mode is slice mode or frame mode*/
	

    unsigned int  PicWidth;                            /*width of a picture to be encoded, in pixel*/
    unsigned int  PicHeight;                           /*height of a picture to be encoded, in pixel*/    

    unsigned int  BFrameNum;                           /* 0: not support B frame; >=1: number of B frames */
    unsigned int  RefNum;                              /* 0: default; number of refrence frame*/

    //BOOL mbLongTermRef; //enable long term reference IDR Frame 
    H264_LEVEL_E mLevel;   //VENC_H264LEVELTYPE
    BOOL FastEncFlag;   //for fast video encoder
    int IQpOffset;  //IQp offset value to offset I frame Qp to decrease I frame size.
    //int mVirtualIFrameInterval; /* (5, IDR_frame_interval), dynamic param */
    BOOL mbPIntraEnable;  //enalbe p frame intra
    unsigned int  mThreshSize;  /*threshSize in stream buffer, 0 means auto config*/
}VENC_ATTR_H264_S;

typedef enum H265_LEVEL_E {
    H265_LEVEL_1   = 30,     /**< Level 1 */
    H265_LEVEL_2  = 60,     /**< Level 2 */
    H265_LEVEL_21  = 63,     /**< Level 2.1 */
    H265_LEVEL_3  = 90,     /**< Level 3 */
    H265_LEVEL_31   = 93,     /**< Level 3.1 */
    H265_LEVEL_41  = 123,     /**< Level 4.1 */
    H265_LEVEL_5  = 150,     /**< Level 5 */
    H265_LEVEL_51   = 153,     /**< Level 5.1 */
    H265_LEVEL_52  = 156,     /**< Level 5.2 */
    H265_LEVEL_6  = 180,     /**< Level 6 */
    H265_LEVEL_61   = 183,     /**< Level 6.1 */
    H265_LEVEL_62  = 186     /**< Level 6.2 */
}H265_LEVEL_E;

typedef struct VENC_ATTR_H265_S
{
    unsigned int  mMaxPicWidth;     /*maximum width of a picture to be encoded, in pixel*/
    unsigned int  mMaxPicHeight;    /*maximum height of a picture to be encoded, in pixel*/
    
    unsigned int  mBufSize;         /*stream buffer size*/
    unsigned int  mProfile;         /*0: MP */          
    BOOL mbByFrame;           /*get stream mode is slice mode or frame mode*/

    
    unsigned int  mPicWidth;        /*width of a picture to be encoded, in pixel*/
    unsigned int  mPicHeight;       /*height of a picture to be encoded, in pixel*/
    
    unsigned int  mBFrameNum;                           /* 0: not support B frame; >=1: number of B frames */
    unsigned int  mRefNum;                              /* 0: default; number of refrence frame*/

    //BOOL mbLongTermRef; //enable long term reference IDR Frame 
    H265_LEVEL_E mLevel;   //VENC_H265LEVELTYPE
    BOOL mFastEncFlag;   //for fast video encoder
    int IQpOffset;  //IQp offset value to offset I frame Qp to decrease I frame size.
    //int mVirtualIFrameInterval;     /* (5, IDR_frame_interval), dynamic param */
    BOOL mbPIntraEnable;  //enalbe p frame intra
    unsigned int  mThreshSize;  /*threshSize in stream buffer, 0 means auto config*/
}VENC_ATTR_H265_S;

typedef struct VENC_ATTR_MJPEG_S
{
    unsigned int  mMaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    unsigned int  mMaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/
    unsigned int  mBufSize;                             /*stream buffer size*/
    BOOL mbByFrame;                               /*get stream mode is field mode  or frame mode*/

	
    unsigned int  mPicWidth;                            /*width of a picture to be encoded, in pixel*/
    unsigned int  mPicHeight;                           /*height of a picture to be encoded, in pixel*/ 
    
}VENC_ATTR_MJPEG_S;

typedef struct VENC_ATTR_JPEG_S 
{
    unsigned int  MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    unsigned int  MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/
    unsigned int  BufSize;                             /*stream buffer size, 0:adaptive to bitRate */
    BOOL bByFrame;                               /*get stream mode is field mode  or frame mode*/
	

    unsigned int  PicWidth;                            /*width of a picture to be encoded, in pixel*/
    unsigned int  PicHeight;                           /*height of a picture to be encoded, in pixel*/
    BOOL  bSupportDCF;                           /*support dcf*/
    
}VENC_ATTR_JPEG_S;

/* attribute of MPEG4*/
typedef struct  VENC_ATTR_MPEG4_S
{
    unsigned int  u32MaxPicWidth;                         /*maximum width of a picture to be encoded, in pixel*/
    unsigned int  u32MaxPicHeight;                        /*maximum height of a picture to be encoded, in pixel*/    

    unsigned int  u32BufSize;                             /*buffer size*/
    BOOL bByFrame;                               /*get stream mode is pack or frame*/


    unsigned int  u32PicWidth;                            /*width of a picture to be encoded, in pixel*/
    unsigned int  u32PicHeight;                           /*height of a picture to be encoded, in pixel*/
}VENC_ATTR_MPEG4_S;

typedef struct VENC_ATTR_S
{
    PAYLOAD_TYPE_E  Type;                         /*the type of payload*/
    union
    {
        VENC_ATTR_H264_S  AttrH264e;              /*attributes of h264*/
        VENC_ATTR_MJPEG_S AttrMjpeg;              /*attributes of mjpeg*/
        VENC_ATTR_JPEG_S  AttrJpeg;               /*attributes of jpeg*/
        VENC_ATTR_MPEG4_S AttrMpeg4;              /*attributes of mpeg4*/
        VENC_ATTR_H265_S  AttrH265e;              /*attributes of h265*/
    };
    int MaxKeyInterval;                           /* wanted key frame interval, dynamic param*/
    unsigned int  SrcPicWidth;                    /* source width of a picture buffer sent to venc channel, in pixel*/
    unsigned int  SrcPicHeight;                   /* source height of a picture buffer sent to venc channel, in pixel*/
    VIDEO_FIELD_E  Field;
    PIXEL_FORMAT_E PixelFormat;
    enum v4l2_colorspace mColorSpace;
    ROTATE_E Rotate;    /*encoder rotate angle.*/
}VENC_ATTR_S;

typedef enum VENC_GOP_MODE_E
{
    VENC_GOPMODE_NORMALP    = 0,
    VENC_GOPMODE_DUALP      = 1,        
    VENC_GOPMODE_SMARTP     = 2,
    VENC_GOPMODE_BIPREDB    = 3,
    VENC_GOPMODE_LOWDELAYB  = 4,
    
    VENC_GOPMODE_BUTT,
}VENC_GOP_MODE_E;


typedef struct VENC_GOP_NORMALP_S
{
   int   mIPQpDelta;
}VENC_GOP_NORMALP_S;

typedef struct VENC_GOP_DUALP_S
{
    unsigned int  mSPInterval;
    int mSPQpDelta;
	int mIPQpDelta;
}VENC_GOP_DUALP_S;

typedef struct VENC_GOP_SMARTP_S
{
	unsigned int   mBgInterval;
    int  mBgQpDelta;
	int  mViQpDelta;									/**/
    int  mVirtualIFrameInterval;
}VENC_GOP_SMARTP_S;


typedef struct VENC_GOP_BIPREDB_S
{
    unsigned int  mBFrmNum;										/*<=MAX_B_FRAME_NUM=3*/
    int mBQpDelta;										/**/
	int mIPQpDelta;
}VENC_GOP_BIPREDB_S;	

typedef struct VENC_GOP_ATTR_S
{
    VENC_GOP_MODE_E         enGopMode;
    union
    {
        VENC_GOP_NORMALP_S  stNormalP;              /*attributes of normal P*/
        VENC_GOP_DUALP_S    stDualP;                /*attributes of dual   P*/
        VENC_GOP_SMARTP_S   stSmartP;               /*attributes of Smart P*/
        VENC_GOP_BIPREDB_S  stBipredB;				/*attributes of b */
    };
    int mGopSize;
}VENC_GOP_ATTR_S;


typedef struct VENC_CHN_ATTR_S
{
    VENC_ATTR_S VeAttr;                           /*the attribute of video encoder*/
    VENC_RC_ATTR_S RcAttr;                        /*the attribute of rate  ctrl*/
    VENC_GOP_ATTR_S GopAttr;
}VENC_CHN_ATTR_S;

typedef struct VENC_CHN_STAT_S
{
    unsigned int mLeftPics;                             /*left picture number */
    unsigned int mLeftStreamBytes;                      /*left stream bytes*/
    unsigned int mLeftStreamFrames;                     /*left stream frames*/
    unsigned int mCurPacks;                             /*pack number of current frame*/
    unsigned int mLeftRecvPics;                         /*Number of frames to be received. This member is valid after AW_MPI_VENC_StartRecvPicEx is called.*/
    unsigned int mLeftEncPics;                          /*Number of frames to be encoded. This member is valid after AW_MPI_VENC_StartRecvPicEx is called.*/
}VENC_CHN_STAT_S;

typedef struct VENC_PARAM_H264_SLICE_SPLIT_S
{
    BOOL mbSplitEnable;                           /*slice split enable, TRUE:enable, FALSE:diable, default value:FALSE*/
    unsigned int  mSplitMode;                           /*0:bit number, 1:mb line number, >=2:no meaning*/
    unsigned int  mSliceSize;                           /*when the splitmode is 0, this value presents the bitsnum of one slice average, 
                                                      when the splitmode is 1, this value presents the mb num of one slice*/
} VENC_PARAM_H264_SLICE_SPLIT_S;

typedef struct VENC_PARAM_H264_INTER_PRED_S
{
    /* search window */
    unsigned int mHWSize;                              /* size of horizontal search window.
                                                      default value: differ with the picture size */
    unsigned int mVWSize;                              /* size of vertical search window.
                                                    default value: differ with the picture size */
                                    
    /* inter pred, one of the following 4 kinds of inter prediction mode must be enable */
    BOOL mbInter16x16PredEn;                     /*default: TRUE, enable 16x16 prediction*/
    BOOL mbInter16x8PredEn;                      /*default: TRUE*/
    BOOL mbInter8x16PredEn;                      /*default: TRUE*/
    BOOL mbInter8x8PredEn;                       /*default: TRUE*/
    BOOL mbExtedgeEn;                            /*default: TRUE*/
} VENC_PARAM_H264_INTER_PRED_S;

typedef struct VENC_PARAM_H264_INTRA_PRED_S
{
    /* intra pred, one of following 2 kinds of intra pred mode must be enable */                            
    BOOL bIntra16x16PredEn;                     /*default: TRUE, enable 16x16 intra prediction*/           
    BOOL bIntraNxNPredEn;                       /*default: TRUE, enable 4x4 and/or 8x8 prediction
                                                     Notes: this value must work with IntraTransMode*/
    unsigned int constrained_intra_pred_flag;            /*default: FALSE, see the H.264 protocol*/
    BOOL bIpcmEn;                               /*default: TRUE, enable ipcm*/
}VENC_PARAM_H264_INTRA_PRED_S;

typedef struct VENC_PARAM_H264_TRANS_S
{
    unsigned int IntraTransMode;                      /* 0: trans4x4, trans8x8; 1: trans4x4, 2: trans8x8 */
    unsigned int InterTransMode;                      /* 0: trans4x4, trans8x8; 1: trans4x4, 2: trans8x8 */
    
    BOOL bScalingListValid;                     /* default: FALSE */
                                                   /* Notes: Enable IntraScalingList8X8 and InterScalingList8X8 while bScalingListValid is TRUE */
    unsigned char  InterScalingList8X8[64];                
    unsigned char  IntraScalingList8X8[64];                
    
    int chroma_qp_index_offset;                 /* [-12,12],default value: 0*/
}VENC_PARAM_H264_TRANS_S;

typedef struct VENC_PARAM_H264_ENTROPY_S
{
    unsigned int EntropyEncModeI;                     /* 0:cavlc, 1:cabac */
    unsigned int EntropyEncModeP;                     /* 0:cavlc, 1:cabac */
    unsigned int cabac_stuff_en;                         /* default: 0*/
    unsigned int Cabac_init_idc;                         /* 0~2 */
}VENC_PARAM_H264_ENTROPY_S;

typedef struct VENC_PARAM_H264_POC_S
{
    unsigned int pic_order_cnt_type;                     /* default value: 2. {0,1,2} */

}VENC_PARAM_H264_POC_S;

typedef struct VENC_PARAM_H264_DBLK_S
{
    unsigned int disable_deblocking_filter_idc;          /* default value: 0. {0,1,2} */
    int slice_alpha_c0_offset_div2;             /* default value: 5. [-6,+6] */
    int slice_beta_offset_div2;                 /* default value: 5. [-6,+6] */
}VENC_PARAM_H264_DBLK_S;



typedef struct VENC_PARAM_H264_VUI_TIME_INFO_S
{
    unsigned char  timing_info_present_flag;               /* default value: 0. If 1, timing info belows will be encoded into vui. {0,1} */
    unsigned char  fixed_frame_rate_flag;                  /* default value: n/a. {0,1} */
    unsigned int num_units_in_tick;                      /* default value: n/a. > 0. */
    unsigned int time_scale;                             /* default value: n/a. > 0. */

}VENC_PARAM_VUI_H264_TIME_INFO_S;

typedef struct VENC_PARAM_VUI_ASPECT_RATIO_S
{
    unsigned char  aspect_ratio_info_present_flag;        /* default value: 0. If 1, aspectratio info belows will be encoded into vui. {0,1} */
    unsigned char  aspect_ratio_idc;                      /* default value: n/a. [0,255],17~254 is reserved. */
	unsigned char  overscan_info_present_flag;			  /* default value: 0,just be 0.If 1, oversacan info belows will be encoded into vui. {0,1} */
	unsigned char  overscan_appropriate_flag;			  /* default value: n/a. */
	unsigned short sar_width;                             /* default value: n/a. sar_width  and  sar_height  shall  be  relatively  prime. */
    unsigned short sar_height ;                  		  /* default value: n/a.  */
}VENC_PARAM_VUI_ASPECT_RATIO_S;


typedef struct VENC_PARAM_VUI_VIDEO_SIGNAL_S
{
    unsigned char  video_signal_type_present_flag ;        /* default value: 0. If 1, video singnal info will be encoded into vui. {0,1} */
    unsigned char  video_format ;                          /* default value: n/a. >= 0. */
    unsigned char  video_full_range_flag;                  /* default value: n/a. {0,1}. */
    unsigned char  colour_description_present_flag ;       /* default value: n/a. {0,1} */
	unsigned char  colour_primaries ;					   /* default value: n/a. [0,255],0/9~255 is reserved */
	unsigned char  transfer_characteristics;			   /* default value: n/a. [0,255],0/13~255 is reserved */
	unsigned char  matrix_coefficients; 				   /* default value: n/a. [0,255],9~255 is reserved */		
}VENC_PARAM_VUI_VIDEO_SIGNAL_S;



typedef struct VENC_PARAM_H264_VUI_S
{
	VENC_PARAM_VUI_ASPECT_RATIO_S VuiAspectRatio;
	VENC_PARAM_VUI_H264_TIME_INFO_S    VuiTimeInfo;
	VENC_PARAM_VUI_VIDEO_SIGNAL_S VuiVideoSignal;
}VENC_PARAM_H264_VUI_S;

//need keep same to vencoder.h, DATA_TIME_LENGTH...
#define  MM_DATA_TIME_LENGTH             24
#define  MM_INFO_LENGTH                 64
#define  MM_GPS_PROCESS_METHOD_LENGTH     100
typedef struct VENC_EXIFINFO_S //aw
{
    unsigned char  CameraMake[MM_INFO_LENGTH];
    unsigned char  CameraModel[MM_INFO_LENGTH];
    unsigned char  DateTime[MM_DATA_TIME_LENGTH];

    unsigned int   ThumbWidth;
    unsigned int   ThumbHeight;
    //unsigned char* ThumbAddrVir;
    //unsigned int   ThumbLen;

    int              Orientation;  //value can be 0,90,180,270 degree
    unsigned int     fr32ExposureTime; //tag 0x829A, FRACTION32()
    unsigned int     fr32FNumber; //tag 0x829D, FRACTION32()
    short           ISOSpeed;//tag 0x8827

    //srational_t    ShutterSpeedValue; //tag 0x9201
    //srational_t    BrightnessValue;   //tag 0x9203
    int    ExposureBiasValueNum; //tag 0x9204

    short           MeteringMode; //tag 0x9207, ExifMeteringModeType
    //short           LightSource; //tag 0x9208
    //short           FlashUsed;     //tag 0x9209
    unsigned int    fr32FocalLength; //tag 0x920A

    //rational_t       DigitalZoomRatio; // tag 0xA404
    short           WhiteBalance; //tag 0xA403
    //short           ExposureMode; //tag 0xA402

    // gps info
    int            enableGpsInfo;
    double         gps_latitude;
    double           gps_longitude;
    double         gps_altitude;
    long           gps_timestamp;
    unsigned char  gpsProcessingMethod[MM_GPS_PROCESS_METHOD_LENGTH];

    unsigned char  CameraSerialNum[128];     //tag 0xA431 (exif 2.3 version)
    short              FocalLengthIn35mmFilm;     // tag 0xA405

    unsigned char  ImageName[128];             //tag 0x010D
    unsigned char  ImageDescription[128];     //tag 0x010E
    //short            ImageWidth;                 //tag 0xA002
    //short            ImageHeight;             //tag 0xA003
    int             thumb_quality;  //[20, 100]
} VENC_EXIFINFO_S;

typedef struct VENC_JPEG_THUMB_BUFFER_S //aw
{
    unsigned char* ThumbAddrVir;
    unsigned int   ThumbLen;
} VENC_JPEG_THUMB_BUFFER_S;

typedef struct VENC_PARAM_JPEG_S
{
    unsigned int Qfactor;                             /*image quality :[1,99]*/

    unsigned char  YQt[64];                              /* y qt value */
    unsigned char  CbQt[64];                             /* cb qt value */
    unsigned char  CrQt[64];                             /* cr qt value */

    unsigned int MCUPerECS;                           /*default value: 0, MCU number of one ECS*/
} VENC_PARAM_JPEG_S;

typedef struct VENC_PARAM_MJPEG_S
{
    unsigned char YQt[64];                               /* y qt value */
    unsigned char CbQt[64];                              /* cb qt value */
    unsigned char CrQt[64];                              /* cr qt value */

    unsigned int MCUPerECS;                           /*default value: 0, MCU number of one ECS*/
} VENC_PARAM_MJPEG_S;



/* ROI struct */
typedef struct VENC_ROI_CFG_S
{
    unsigned int  Index;                              /* Index of an ROI. The system supports indexes ranging from 0 to 7 */
    BOOL bEnable;                               /* Whether to enable this ROI */
    BOOL bAbsQp;                                /* QP mode of an ROI.FALSE: relative QP.TURE: absolute QP.*/
    int  Qp;                                 /* QP value. */
    RECT_S  Rect;                                /* Region of an ROI*/
}VENC_ROI_CFG_S;


typedef struct VENC_ROIBG_FRAME_RATE_S
{
    int s32SrcFrmRate;                          /* Input frame rate of   Roi backgroud*/
    int s32DstFrmRate;                          /* Output frame rate of  Roi backgroud */
}VENC_ROIBG_FRAME_RATE_S;

/*
typedef enum VENC_OVERLAY_STYLE_E
{
    OVERLAY_STYLE_NORMAL   = 0, //normal overlay
    OVERLAY_STYLE_COVER,        //use the setting yuv to cover region
    OVERLAY_STYLE_LUMA_REVERSE, //normal overlay and luma reverse
}VENC_OVERLAY_STYLE_E;

typedef enum VENC_OVERLAY_BITMAP_COLOR_E
{
    BITMAP_COLOR_ARGB8888  = 0,
    BITMAP_COLOR_ARGB4444  = 1,
    BITMAP_COLOR_ARGB1555  = 2,
    BITMAP_COLOR_TYPE_MIN  = BITMAP_COLOR_ARGB8888,
    BITMAP_COLOR_TYPE_MAX  = BITMAP_COLOR_ARGB1555,
}VENC_OVERLAY_BITMAP_COLOR_E;

typedef struct VENC_OVERLAY_COVER_YUV_S
{
    unsigned char bCoverY; //the value of cover y
    unsigned char bCoverU; //the value of cover u
    unsigned char bCoverV; //the value of cover v
}VENC_OVERLAY_COVER_YUV_S;

typedef struct VENC_OVERLAY_REGION_INFO_S
{
    unsigned int bRegionID; //the ID of this region. reserve
    VENC_OVERLAY_STYLE_E bOverlayType; //reference define of VENC_OVERLAY_TYPE
    unsigned short nPriority;   //value in [0 -> VENC_OVERLAY_REGION_NUM_MAX]. The value is bigger,the region will on the topper relatively.
                                //all the region priority must not be the same
    RECT_S rect; //the rect of the region

    unsigned char extraAlphaFlag;  //0:no use extra_alpha; 1:use extra_alpha
    unsigned char extraAlphaVal;   //use argb's alpha + extraAlphaVal, range is [0, 15]

    VENC_OVERLAY_COVER_YUV_S coverYUV; //when use OVERLAY_STYLE_COVER should set the cover yuv value
    void *pBitMapAddr; //the vir addr of overlay string bitmap addr
    unsigned int nBitMapSize; //
}VENC_OVERLAY_REGION_INFO_S;

#define VENC_OVERLAY_MAX_REGION_NUM  (64)
typedef struct VENC_OVERLAY_INFO
{
    unsigned int regionNum;
    VENC_OVERLAY_BITMAP_COLOR_E nBitMapColorType; //all the regions to be set must be the same bitmap colortype;
    VENC_OVERLAY_REGION_INFO_S region[VENC_OVERLAY_MAX_REGION_NUM];
}VENC_OVERLAY_INFO;
*/

typedef struct VENC_COLOR2GREY_S
{
    BOOL bColor2Grey;                           /* Whether to enable Color2Grey.*/
}VENC_COLOR2GREY_S;

typedef struct VENC_CROP_CFG_S
{
    BOOL bEnable;                               /* Crop region enable */
    RECT_S  Rect;                                /* Crop region, note: X must be multi of 16 */
}VENC_CROP_CFG_S;


typedef struct VENC_FRAME_RATE_S
{
    int SrcFrmRate;                          /* Input frame rate of a  channel*/
    int DstFrmRate;                          /* Output frame rate of a channel*/
} VENC_FRAME_RATE_S;

typedef struct VENC_PARAM_REF_S
{
    unsigned int       Base;                          /*Base layer period*/
    unsigned int       Enhance;                       /*Enhance layer period*/
    BOOL      bEnablePred;                      /*Whether some frames at the base layer are referenced by other frames at the base layer. When bEnablePred is FALSE, all frames at the base layer reference IDR frames.*/
} VENC_PARAM_REF_S;


typedef enum H264E_IDR_PIC_ID_MODE_E
{
    H264E_IDR_PIC_ID_MODE_AUTO = 0,                /*auto mode */
    H264E_IDR_PIC_ID_MODE_USR,                     /*user mode */
    H264E_IDR_PIC_ID_MODE_BUTT,
}H264E_IDR_PIC_ID_MODE_E;

typedef struct VENC_H264_IDRPICID_CFG_S
{
    H264E_IDR_PIC_ID_MODE_E enH264eIdrPicIdMode;   /*the mode of idr_pic_id that is set*/
    unsigned int u32H264eIdrPicId;                       /*idr_pic_id value*/
} VENC_H264_IDRPICID_CFG_S;     

/* Jpeg snap mode */
typedef enum VENC_JPEG_SNAP_MODE_E
{
    JPEG_SNAP_ALL   = 0,                           /* Jpeg channel snap all the pictures when started. */
    JPEG_SNAP_FLASH = 1,                           /* Jpeg channel snap the flashed pictures when started. */
    JPEG_SNAP_BUTT,
    
}VENC_JPEG_SNAP_MODE_E;

typedef struct VENC_RECV_PIC_PARAM_S
{
    int mRecvPicNum;                          /*Number of frames received and encoded by the encoding channel*/
} VENC_RECV_PIC_PARAM_S;

typedef struct VENC_STREAM_BUF_INFO_S
{
    unsigned int   PhyAddr;
    void *pUserAddr;
    unsigned int   BufSize;    
} VENC_STREAM_BUF_INFO_S;


typedef struct VENC_PARAM_H265_SLICE_SPLIT_S
{
    BOOL bSplitEnable;   /* default value: FALSE, means no slice split*/
    unsigned int  SplitMode;   /* 0: by bits number; 1: by lcu line number*/
    unsigned int  SliceSize;   /* when splitmode is 0, this value presents bits number, 
                               when splitmode is 1, this value presents lcu line number */
    unsigned int  loop_filter_across_slices_enabled_flag;
} VENC_PARAM_H265_SLICE_SPLIT_S;

typedef struct VENC_PARAM_H265_INTER_PRED_S
{
    /* search window */
    unsigned int u32HWSize;                              /* size of horizontal search window.
                                                      default value: differ with the picture size */
    unsigned int u32VWSize;                              /* size of vertical search window.
                                                    default value: differ with the picture size */
                                    
    /* inter pred, one of the following 4 kinds of inter prediction mode must be enable */
    BOOL bInter16x16PredEn;                     /*default: TRUE, enable 16x16 prediction*/
    BOOL bInter16x8PredEn;                      /*default: TRUE*/
    BOOL bInter8x16PredEn;                      /*default: TRUE*/
    BOOL bInter8x8PredEn;                       /*default: TRUE*/
    BOOL bExtedgeEn;                            /*default: TRUE*/
} VENC_PARAM_H265_INTER_PRED_S;

typedef struct VENC_PARAM_H265_INTRA_PRED_S
{
    /* intra pred, one of following 2 kinds of intra pred mode must be enable */                            
    BOOL bIntra16x16PredEn;                     /*default: TRUE, enable 16x16 intra prediction*/           
    BOOL bIntraNxNPredEn;                       /*default: TRUE, enable 4x4 and/or 8x8 prediction
                                                     Notes: this value must work with IntraTransMode*/
    unsigned int  constrained_intra_pred_flag;            /*default: FALSE, see the H.264 protocol*/
    unsigned int  pcm_enabled_flag;               /*default: TRUE, enable ipcm*/
}VENC_PARAM_H265_INTRA_PRED_S;

typedef struct VENC_PARAM_H265_PU_S
{
    BOOL bPu32x32En;                             /*default: TRUE */  
    BOOL bPu16x16En;                             /*default: TRUE */           
    BOOL bPu8x8En;                               /*default: TRUE */ 
    BOOL bPu4x4En;                               /*default: TRUE */ 

    // intra pred
    unsigned int  constrained_intra_pred_flag;            /*default: FALSE*/
    unsigned int  strong_intra_smoothing_enabled_flag;
    unsigned int  pcm_enabled_flag;                       /*default: FALSE, enable ipcm*/
    unsigned int  pcm_loop_filter_disabled_flag;

    // inter pred
    unsigned int  MaxNumMergeCand;

}VENC_PARAM_H265_PU_S;


typedef struct VENC_PARAM_H265_TRANS_S
{
    unsigned int transquant_bypass_enabled_flag;
    unsigned int transform_skip_enabled_flag;
    int cb_qp_offset;
    int cr_qp_offset;
} VENC_PARAM_H265_TRANS_S;

typedef struct VENC_PARAM_H265_ENTROPY_S
{
    unsigned int cabac_init_flag;                         /* default: 0*/
}VENC_PARAM_H265_ENTROPY_S;

typedef struct VENC_PARAM_H265_DBLK_S
{
    unsigned int slice_deblocking_filter_disabled_flag;        /* default value: 0. {0,1} */
    int slice_beta_offset_div2;                 /* default value: 5. [-6,+6] */
    int slice_tc_offset_div2;             /* default value: 5. [-6,+6] */
}VENC_PARAM_H265_DBLK_S;

typedef struct VENC_PARAM_H265_SAO_S
{
    unsigned int  slice_sao_luma_flag;
    unsigned int  slice_sao_chroma_flag;
}VENC_PARAM_H265_SAO_S;


typedef struct VENC_PARAM_H265_TIMING_S
{
    int timing_info_present_flag;               /* default value: 0. If 1, timing info belows will be encoded into vui. {0,1} */
    int num_units_in_tick;                      /* default value: n/a. > 0. */
    int time_scale;                             /* default value: n/a. > 0. */
    unsigned int num_ticks_poc_diff_one;
}VENC_PARAM_H265_TIMING_S;

typedef struct VENC_PARAM_INTRA_REFRESH_S
{
    BOOL bRefreshEnable;
    BOOL bISliceEnable;
    unsigned int  RefreshLineNum;
    unsigned int  ReqIQp;
}VENC_PARAM_INTRA_REFRESH_S;



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __IPCLINUX_COMM_VENC_H__ */

/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_rc.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/15
  Last Modified :
  Description   : common struct definition for Rate control
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_MM_COMM_RC_H__
#define __IPCLINUX_MM_COMM_RC_H__

#include "plat_defines.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef enum VENC_RC_MODE_E
{
    VENC_RC_MODE_H264CBR = 1,    
    VENC_RC_MODE_H264VBR,        
    VENC_RC_MODE_H264ABR,        
    VENC_RC_MODE_H264FIXQP,
    VENC_RC_MODE_H264QPMAP,

    VENC_RC_MODE_MJPEGCBR,    
    VENC_RC_MODE_MJPEGVBR,        
    VENC_RC_MODE_MJPEGABR,        
    VENC_RC_MODE_MJPEGFIXQP,  

    VENC_RC_MODE_MPEG4CBR,    
    VENC_RC_MODE_MPEG4VBR,        
    VENC_RC_MODE_MPEG4ABR,        
    VENC_RC_MODE_MPEG4FIXQP,  

    VENC_RC_MODE_H265CBR,    
    VENC_RC_MODE_H265VBR,        
    VENC_RC_MODE_H265ABR,
    VENC_RC_MODE_H265FIXQP,
    VENC_RC_MODE_H265QPMAP,
    
    VENC_RC_MODE_BUTT,

}VENC_RC_MODE_E;


typedef struct VENC_ATTR_H264_FIXQP_S
{
    unsigned int      mGop;                                 /*the interval of ISLICE. */
    unsigned int      mSrcFrmRate;                          /* the input frame rate of the venc chnnel */
    unsigned int     fr32DstFrmRate ;                        /* the target frame rate of the venc chnnel */     
    unsigned int      mIQp;                                 /* qp of the i frame */
    unsigned int      mPQp;                                 /* qp of the p frame */
} VENC_ATTR_H264_FIXQP_S;

typedef struct VENC_ATTR_H264_CBR_S
{
    unsigned int      mGop;                                 /*the interval of ISLICE. */
    unsigned int      mStatTime;                            /* the rate statistic time, the unit is senconds(s) */
    unsigned int      mSrcFrmRate;                          /* the input frame rate of the venc chnnel */
    unsigned int      fr32DstFrmRate ;                        /* the target frame rate of the venc chnnel */ 
    unsigned int      mBitRate;                             /* average bitrate */
    unsigned int      mFluctuateLevel;                      /* level [0..5].scope of bitrate fluctuate. 1-5: 10%-50%. 0: SDK optimized, recommended; */
    unsigned int      mMaxQp;                               /* the max qp */
    unsigned int      mMinQp;                               /* the min qp */
} VENC_ATTR_H264_CBR_S;                                         
                                         
typedef struct VENC_ATTR_H264_VBR_S
{
    unsigned int      mGop;                                 /*the interval of ISLICE. */
    unsigned int      mStatTime;                            /* the rate statistic time, the unit is senconds(s) */
    unsigned int      mSrcFrmRate;                          /* the input frame rate of the venc chnnel */
    unsigned int     fr32DstFrmRate ;                        /* the target frame rate of the venc chnnel */     
    unsigned int      mMaxBitRate;                          /* the max bitrate */                      
    unsigned int      mMaxQp;                               /* the max qp */
    unsigned int      mMinQp;                               /* the min qp */
    unsigned int      mRatioChangeQp;                       /* range[50,100], default:85 */
    int               mQuality;                             /* range[1,13], 1:worst quality, 13:bestquality */
}VENC_ATTR_H264_VBR_S;

typedef struct VENC_ATTR_H264_ABR_S
{
    unsigned int      mGop;                                 /*the interval of ISLICE. */
    unsigned int      mStatTime;                            /* the rate statistic time, the unit is senconds(s) */
    unsigned int      mSrcFrmRate;                          /* the input frame rate of the venc chnnel */
    unsigned int     fr32DstFrmRate ;                        /* the target frame rate of the venc chnnel */    
    //unsigned int      mAvgBitRate;                          /* average bitrate */
    unsigned int      mMaxBitRate;                          /* the max bitrate */
    unsigned int      mMinIprop;      //no support
    unsigned int      mMaxIprop;      //no support
    int               mMaxReEncodeTimes; //no support, default use one time
    unsigned int      mRatioChangeQp;                       /* range[50,100], default:85 */
    int               mQuality;                             //range[1,13], 1:worst quality, 13:best quality, recommend:8.
    unsigned int      mMaxStaticIQp;  //no support
    unsigned int      mMinIQp;                              /* I frame qp lower limit */
    unsigned int      mMaxIQp;      //no support upper limit, I frame qp only use lower_limit(mMinIQp)
    unsigned int      mMaxQp;                               /* the max qp */
    unsigned int      mMinQp;                               /* the min qp */
}VENC_ATTR_H264_ABR_S;

typedef struct VENC_ATTR_H264_QPMAP_S
{
    unsigned int      mGop;                                 /* */
    unsigned int      mSrcFrmRate;                          /*  */
    unsigned int      fr32DstFrmRate ;                      /*  */
    void              *pQpMap;                             /* qp map */
} VENC_ATTR_H264_QPMAP_S;


typedef struct VENC_ATTR_H264_CBR_S   VENC_ATTR_H265_CBR_S;
typedef struct VENC_ATTR_H264_VBR_S   VENC_ATTR_H265_VBR_S;
typedef struct VENC_ATTR_H264_FIXQP_S VENC_ATTR_H265_FIXQP_S;
typedef struct VENC_ATTR_H264_ABR_S   VENC_ATTR_H265_ABR_S;
typedef struct VENC_ATTR_H264_QPMAP_S VENC_ATTR_H265_QPMAP_S;




typedef struct VENC_ATTR_MPEG4_FIXQP_S
{
    unsigned int      u32Gop;                                 /*the interval of ISLICE. */
    unsigned int      u32SrcFrmRate;                          /* the input frame rate of the venc chnnel */
    unsigned int     fr32DstFrmRate ;                        /* the target frame rate of the venc chnnel */     
    unsigned int      u32IQp;                                 /* qp of the i frame */
    unsigned int      u32PQp;                                 /* qp of the p frame */
}VENC_ATTR_MPEG4_FIXQP_S;

typedef struct VENC_ATTR_MPEG4_CBR_S
{
    unsigned int      u32Gop;                                 /*the interval of ISLICE. */
    unsigned int      u32StatTime;                            /* the rate statistic time, the unit is senconds(s) */
    unsigned int      u32SrcFrmRate;                          /* the input frame rate of the venc chnnel */
    unsigned int     fr32DstFrmRate ;                        /* the target frame rate of the venc chnnel */ 
    unsigned int      u32BitRate;                             /* average bitrate */
    unsigned int      u32FluctuateLevel;                      /* level [0..5].scope of bitrate fluctuate. 1-5: 10%-50%. 0: SDK optimized, recommended; */
}VENC_ATTR_MPEG4_CBR_S;

typedef struct VENC_ATTR_MPEG4_VBR_S
{
    unsigned int      u32Gop;                                 /* the interval of ISLICE. */
    unsigned int      u32StatTime;                            /* the rate statistic time, the unit is senconds(s) */
    unsigned int      u32SrcFrmRate;                          /* the input frame rate of the venc chnnel */
    unsigned int     fr32DstFrmRate ;                        /* the target frame rate of the venc chnnel */     
    unsigned int      u32MaxBitRate;                          /* the max bitrate */                      
    unsigned int      u32MaxQp;                               /* the max qp */
    unsigned int      u32MinQp;                               /* the min qp */
}VENC_ATTR_MPEG4_VBR_S;
 
typedef struct VENC_ATTR_MJPEG_FIXQP_S
{
    unsigned int      mSrcFrmRate;                          /* the input frame rate of the venc chnnel */
    unsigned int     fr32DstFrmRate;                         /* the target frame rate of the venc chnnel */
    unsigned int      mQfactor;                             /* image quality :[1,99]*/
}VENC_ATTR_MJPEG_FIXQP_S;

typedef struct VENC_ATTR_MJPEG_CBR_S
{
    unsigned int      mStatTime;                            /* the rate statistic time, the unit is senconds(s) */
    unsigned int      mSrcFrmRate;                          /* the input frame rate of the venc chnnel */
    unsigned int     fr32DstFrmRate ;                        /* the target frame rate of the venc chnnel */
    unsigned int      mBitRate;                             /* average bitrate */
    unsigned int      mFluctuateLevel;                      /* level [0..5].scope of bitrate fluctuate. 1-5: 10%-50%. 0: SDK optimized, recommended; */
} VENC_ATTR_MJPEG_CBR_S;

typedef struct VENC_ATTR_MJPEG_VBR_S
{
    unsigned int      u32StatTime;                            /* the rate statistic time, the unit is senconds(s) */
    unsigned int      u32SrcFrmRate;                          /* the input frame rate of the venc chnnel */
    unsigned int     fr32DstFrmRate;                         /* the target frame rate of the venc chnnel */
    unsigned int      u32MaxBitRate;                          /* max bitrate */
    unsigned int      u32MaxQfactor;                          /* max image quailty allowed */
    unsigned int      u32MinQfactor;                          /* min image quality allowed */
}VENC_ATTR_MJPEG_VBR_S;

typedef struct VENC_RC_ATTR_S
{
    VENC_RC_MODE_E mRcMode;                            /*the type of rc*/
    union
    {
        VENC_ATTR_H264_CBR_S    mAttrH264Cbr;
        VENC_ATTR_H264_VBR_S    mAttrH264Vbr;
        VENC_ATTR_H264_FIXQP_S  mAttrH264FixQp;
        VENC_ATTR_H264_ABR_S    mAttrH264Abr;
        VENC_ATTR_H264_QPMAP_S  mAttrH264QpMap;


        VENC_ATTR_MPEG4_CBR_S   mAttrMpeg4Cbr;
        VENC_ATTR_MPEG4_FIXQP_S mAttrMpeg4FixQp;
        VENC_ATTR_MPEG4_VBR_S   mAttrMpeg4Vbr;

        VENC_ATTR_MJPEG_CBR_S   mAttrMjpegeCbr;
        VENC_ATTR_MJPEG_FIXQP_S mAttrMjpegeFixQp;
        VENC_ATTR_MJPEG_VBR_S   mAttrMjpegeVbr;

        VENC_ATTR_H265_CBR_S    mAttrH265Cbr;
        VENC_ATTR_H265_VBR_S    mAttrH265Vbr;
        VENC_ATTR_H265_FIXQP_S  mAttrH265FixQp;
        VENC_ATTR_H265_ABR_S    mAttrH265Abr;
        VENC_ATTR_H265_QPMAP_S  mAttrH265QpMap;
    };   
    void*       pRcAttr ;                            /*the rc attribute which could be specified by user*/
}VENC_RC_ATTR_S;

typedef enum RC_SUPERFRM_MODE_E
{
    SUPERFRM_NONE,                                      /* sdk don't care super frame */
    SUPERFRM_DISCARD,                                   /* the super frame is discarded */
    SUPERFRM_REENCODE,                                  /* the super frame is re-encode */
    SUPERFRM_BUTT
}VENC_SUPERFRM_MODE_E;

typedef struct VENC_PARAM_H264_CBR_S
{
    unsigned int  MinIprop;                                /* the min ratio of i frame and p frame */           
    unsigned int  MaxIprop;                                /* the max ratio of i frame and p frame */
    unsigned int  MaxQp;                                   /* the max QP value */
    unsigned int  MinQp;                                   /* the min QP value */
    int  IPQPDelta;                               /* the qp difference between the i frame and the before gop avarage qp; == Qp(P) - Qp(I) */
    int  QualityLevel;                            /* quality of picture [1, 5] */
    int  MaxReEncodeTimes;                        /* max number of re-encode times [0, 3]*/ 
    unsigned int  MinIQp;                                  /* min qp for i frame */
}VENC_PARAM_H264_CBR_S;

typedef struct VENC_PARAM_H264_VBR_S
{
    int s32IPQPDelta;                                  /* the qp difference between the i frame and the before gop avarage qp; == Qp(P) - Qp(I) */
    int s32ChangePos;                                /* Indicates the ratio of the current bit rate to the maximum 
                                                            bit rate when the QP value starts to be adjusted */
    unsigned int  u32MinIprop;                                /* the min ratio of i frame and p frame */           
    unsigned int  u32MaxIprop;                                /* the max ratio of i frame and p frame */                                          
    unsigned int  u32MinIQP;                                  /* min qp for i frame */
}VENC_PARAM_H264_VBR_S;



typedef struct VENC_PARAM_MJPEG_CBR_S
{
    unsigned int  MaxQfactor;                              /* the max Qfactor value*/
    unsigned int  MinQfactor;                              /* the min Qfactor value */    
    unsigned int  RQRatio[RC_RQRATIO_SIZE];                              /* the rate stabilization weight, 
                                                            100-u32RQRatio[i] is the sequence quality stabilization weight */ 

}VENC_PARAM_MJPEG_CBR_S;

typedef struct VENC_PARAM_MJPEG_VBR_S
{
    int s32DeltaQfactor;                             /* Indicates the maximum change of Qfactor values of frames 
                                                            when the picture quality changes */
    int s32ChangePos;                                /* Indicates the ratio of the current bit rate to the maximum 
                                                            bit rate when the Qfactor value starts to be adjusted */                                               
}VENC_PARAM_MJPEG_VBR_S;

typedef struct VENC_PARAM_MPEG4_CBR_S
{
    unsigned int  u32MinIprop;                                /* the min ratio of i frame and p frame*/           
    unsigned int  u32MaxIprop;                                /* the max ratio of i frame and p frame */
     
    unsigned int  u32MaxQp;                                   /* the max QP value*/
    unsigned int  u32MinQp;                                   /* the min QP value */
    unsigned int  u32MaxPPDeltaQp;                            /* the max qp value difference between two successive P frame */
    unsigned int  u32MaxIPDeltaQp;                            /* the max qp value difference between p frame and the next i frame */    
    int s32IPQPDelta;                                /* the qp difference between the i frame and the before gop avarage qp*/

    unsigned int  u32RQRatio[RC_RQRATIO_SIZE];                              /* the rate stabilization weight, 
                                                            100-u32RQRatio[i] is the sequence quality stabilization weight */
}VENC_PARAM_MPEG4_CBR_S;

typedef struct VENC_PARAM_MPEG4_VBR_S
{
    int s32IPQPDelta;                                  /* the qp difference between the i frame and the before gop avarage qp*/
    
    int s32ChangePos;                                /* Indicates the ratio of the current bit rate to the maximum 
                                                            bit rate when the QP value starts to be adjusted */

    unsigned int  u32MinIprop;                                /* the min ratio of i frame and p frame */           
    unsigned int  u32MaxIprop;                                /* the max ratio of i frame and p frame */                                               
}VENC_PARAM_MPEG4_VBR_S;

typedef struct VENC_PARAM_H264_CBR_S   VENC_PARAM_H265_CBR_S; 
typedef struct VENC_PARAM_H264_VBR_S   VENC_PARAM_H265_VBR_S; 


typedef struct VENC_RC_PARAM_S
{     
    unsigned int ThrdI[RC_TEXTURE_THR_SIZE];                     /* just useful for h264/h265 and mpeg4 for now */
    unsigned int ThrdP[RC_TEXTURE_THR_SIZE];
    unsigned int RowQpDelta;
    union   //judge by VENC_CHN_ATTR_S->mVeAttr->mType
    {
        VENC_PARAM_H264_CBR_S     ParamH264Cbr;
        VENC_PARAM_H264_VBR_S     ParamH264VBR;
        VENC_PARAM_MJPEG_CBR_S    ParamMjpegCbr;
        VENC_PARAM_MJPEG_VBR_S    ParamMjpegVbr;
        VENC_PARAM_MPEG4_CBR_S    ParamMpeg4Cbr;
        VENC_PARAM_MPEG4_VBR_S    ParamMpeg4Vbr;
        VENC_PARAM_H265_CBR_S     ParamH265Cbr;
        VENC_PARAM_H265_VBR_S     ParamH265Vbr;        
    };

    void* pRcParam;                      /*RC parameter which could be specified by usrer*/
}VENC_RC_PARAM_S;



typedef enum VENC_FRAMELOST_MODE_E
{   
    FRMLOST_NORMAL,                                /*normal mode*/         
    FRMLOST_PSKIP,                                  /*pskip*/ 
    FRMLOST_BUTT,
}VENC_FRAMELOST_MODE_E;

typedef struct VENC_PARAM_FRAMELOST_S
{ 
    BOOL bFrmLostOpen;                           /* Indicates whether to discard frames to ensure 
                                                            stable bit rate when the instant bit rate is exceeded */
    unsigned int  FrmLostBpsThr;                       /* the instant bit rate threshold */
    VENC_FRAMELOST_MODE_E enFrmLostMode;            /*frame lost strategy*/
    unsigned int  EncFrmGaps;                          /*the gap of frame lost*/
}VENC_PARAM_FRAMELOST_S;


typedef struct VENC_SUPERFRAME_CFG_S
{ 
      VENC_SUPERFRM_MODE_E  enSuperFrmMode;               /* Indicates the mode of processing the super frame,[SUPERFRM_NONE,SUPERFRM_DISCARD,SUPERFRM_REENCODE]*/
      unsigned int  SuperIFrmBitsThr;                        /* Indicate the threshold of the super I frame 
                                                               for enabling the super frame processing mode */              
      unsigned int  SuperPFrmBitsThr;                        /* Indicate the threshold of the super P frame */                                                     
      unsigned int  SuperBFrmBitsThr;                        /* Indicate the threshold of the super B frame */                                                   
}VENC_SUPERFRAME_CFG_S;

typedef enum VENC_RC_PRIORITY_E
{
    VENC_RC_PRIORITY_BITRATE_FIRST = 1,    
    VENC_RC_PRIORITY_FRAMEBITS_FIRST,

    VENC_RC_PRIORITY_BUTT,
} VENC_RC_PRIORITY_E;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __IPCLINUX_MM_COMM_RC_H__ */

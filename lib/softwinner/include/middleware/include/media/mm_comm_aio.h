/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : mm_comm_aio.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/03/23
  Last Modified :
  Description   : common struct definition for AENC
  Function List :
  History       :
******************************************************************************/

#ifndef __IPCLINUX_COMM_AIO_H__
#define __IPCLINUX_COMM_AIO_H__

#include "plat_type.h"
#include "plat_errno.h"

#include "mm_common.h"



#ifdef __cplusplus
extern "C" {
#endif

#define MAX_AUDIO_FRAME_NUM    50       /*max count of audio frame in Buffer */
#define MAX_AUDIO_POINT_BYTES  4        /*max bytes of one sample point(now 32bit max)*/

#define MAX_VOICE_POINT_NUM    480      /*max sample per frame for voice encode */

#define MAX_AUDIO_POINT_NUM    2048     /*max sample per frame for all encoder(aacplus:2048)*/
#define MAX_AO_POINT_NUM       4096     /* from h3£»support 4096 framelen*/
#define MIN_AUDIO_POINT_NUM    80       /*min sample per frame*/
#define MAX_AI_POINT_NUM    2048     /*max sample per frame for all encoder(aacplus:2048)*/

/*max length of audio frame by bytes, one frame contain many sample point */
#define MAX_AUDIO_FRAME_LEN    (MAX_AUDIO_POINT_BYTES*MAX_AO_POINT_NUM)

/*max length of audio stream by bytes */
#define MAX_AUDIO_STREAM_LEN   MAX_AUDIO_FRAME_LEN

#define MAX_AI_USRFRM_DEPTH     30      /*max depth of user frame buf */

/*The VQE EQ Band num.*/
#define VQE_EQ_BAND_NUM 11


typedef enum AUDIO_SAMPLE_RATE_E
{
    AUDIO_SAMPLE_RATE_8000   = 8000,    /* 8K samplerate*/
    AUDIO_SAMPLE_RATE_12000  = 12000,   /* 12K samplerate*/    
    AUDIO_SAMPLE_RATE_11025  = 11025,   /* 11.025K samplerate*/
    AUDIO_SAMPLE_RATE_16000  = 16000,   /* 16K samplerate*/
    AUDIO_SAMPLE_RATE_22050  = 22050,   /* 22.050K samplerate*/
    AUDIO_SAMPLE_RATE_24000  = 24000,   /* 24K samplerate*/
    AUDIO_SAMPLE_RATE_32000  = 32000,   /* 32K samplerate*/
    AUDIO_SAMPLE_RATE_44100  = 44100,   /* 44.1K samplerate*/
    AUDIO_SAMPLE_RATE_48000  = 48000,   /* 48K samplerate*/
    AUDIO_SAMPLE_RATE_BUTT,
} AUDIO_SAMPLE_RATE_E;

typedef enum AUDIO_BIT_WIDTH_E
{
    AUDIO_BIT_WIDTH_8   = 0,   /* 8bit width */
    AUDIO_BIT_WIDTH_16  = 1,   /* 16bit width*/
    AUDIO_BIT_WIDTH_24  = 2,   /* 24bit width*/
    AUDIO_BIT_WIDTH_32  = 3,   /* 32bit width*/
    AUDIO_BIT_WIDTH_BUTT,
} AUDIO_BIT_WIDTH_E;

typedef enum AIO_MODE_E
{
    AIO_MODE_I2S_MASTER  = 0,   /* AIO I2S master mode */
    AIO_MODE_I2S_SLAVE,         /* AIO I2S slave mode */
    AIO_MODE_PCM_SLAVE_STD,     /* AIO PCM slave standard mode */
    AIO_MODE_PCM_SLAVE_NSTD,    /* AIO PCM slave non-standard mode */
    AIO_MODE_PCM_MASTER_STD,    /* AIO PCM master standard mode */
    AIO_MODE_PCM_MASTER_NSTD,   /* AIO PCM master non-standard mode */
    AIO_MODE_BUTT
} AIO_MODE_E;

typedef enum AIO_SOUND_MODE_E
{
    AUDIO_SOUND_MODE_MONO   =0,/*mono*/
    AUDIO_SOUND_MODE_STEREO =1,/*stereo*/
    AUDIO_SOUND_MODE_BUTT
} AUDIO_SOUND_MODE_E;

/*
An example of the packing scheme for G726-32 codewords is as shown, and bit A3 is the least significant bit of the first codeword:
RTP G726-32:
0                   1
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
|B B B B|A A A A|D D D D|C C C C| ...
|0 1 2 3|0 1 2 3|0 1 2 3|0 1 2 3|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

MEDIA G726-32:
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
|A A A A|B B B B|C C C C|D D D D| ...
|3 2 1 0|3 2 1 0|3 2 1 0|3 2 1 0|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*/
typedef enum G726_BPS_E
{
    G726_16K = 0,       /* G726 16kbps, see RFC3551.txt  4.5.4 G726-16 */
    G726_24K,           /* G726 24kbps, see RFC3551.txt  4.5.4 G726-24 */
    G726_32K,           /* G726 32kbps, see RFC3551.txt  4.5.4 G726-32 */
    G726_40K,           /* G726 40kbps, see RFC3551.txt  4.5.4 G726-40 */
    MEDIA_G726_16K,     /* G726 16kbps for ASF ... */
    MEDIA_G726_24K,     /* G726 24kbps for ASF ... */
    MEDIA_G726_32K,     /* G726 32kbps for ASF ... */
    MEDIA_G726_40K,     /* G726 40kbps for ASF ... */
    G726_BUTT,
} G726_BPS_E;

typedef enum ADPCM_TYPE_E
{
    /* see DVI4 diiffers in three respects from the IMA ADPCM at RFC3551.txt 4.5.1 DVI4 */

    ADPCM_TYPE_DVI4 = 0,    /* 32kbps ADPCM(DVI4) for RTP */
    ADPCM_TYPE_IMA,         /* 32kbps ADPCM(IMA),NOTICE:point num must be 161/241/321/481 */
    ADPCM_TYPE_ORG_DVI4,
    ADPCM_TYPE_BUTT,
} ADPCM_TYPE_E;

#define AI_EXPAND  0x01
#define AI_CUT     0x02

typedef enum PCM_CARD_TYPE_E
{
    PCM_CARD_TYPE_AUDIOCODEC = 0,    /* IC internal codec */
    PCM_CARD_TYPE_SNDHDMI = 1,    /* hdmi audio */
    PCM_CARD_TYPE_BUTT,
} PCM_CARD_TYPE_E;

typedef struct AIO_ATTR_S
{
    AUDIO_SAMPLE_RATE_E enSamplerate;   /* sample rate */
    AUDIO_BIT_WIDTH_E   enBitwidth;     /* bitwidth */
    AIO_MODE_E          enWorkmode;     /* master or slave mode */
    AUDIO_SOUND_MODE_E  enSoundmode;    /* momo or steror */
    unsigned int        u32EXFlag;      /* expand 8bit to 16bit,use AI_EXPAND(only valid for AI 8bit) */
    unsigned int        u32FrmNum;      /* frame num in buf[2,MAX_AUDIO_FRAME_NUM] */
    unsigned int        u32PtNumPerFrm; /* point num per frame (80/160/240/320/480/1024/2048)
                                                (ADPCM IMA should add 1 point, AMR only support 160) */
    unsigned int        u32ChnCnt;      /* channle number on FS, valid value:1/2/4/8 */
    unsigned int        u32ClkSel;      /* 0: AI and AO clock is separate 
                                                 1: AI and AO clock is inseparate, AI use AO's clock */
    PCM_CARD_TYPE_E     mPcmCardId;     /* audio card selection for play audio file */
} AIO_ATTR_S;

typedef struct AI_CHN_PARAM_S
{
    unsigned int u32UsrFrmDepth;
} AI_CHN_PARAM_S;

typedef struct AUDIO_FRAME_S
{
    AUDIO_SAMPLE_RATE_E mSamplerate;   /* sample rate */
    AUDIO_BIT_WIDTH_E   mBitwidth;     /*audio frame bitwidth*/
    AUDIO_SOUND_MODE_E  mSoundmode;    /*audio frame momo or stereo mode*/
    void                *mpAddr;
    unsigned long long  mTimeStamp;                /*audio frame timestamp*/
    unsigned int        mSeq;                      /*audio frame seq*/
    unsigned int        mLen;                      /*data lenth per channel in frame*/
    unsigned int        mId;

    int64_t tmp_pts;
} AUDIO_FRAME_S;

typedef struct AEC_FRAME_S
{
    AUDIO_FRAME_S   stRefFrame;    /* AEC reference audio frame */
    BOOL            bValid;        /* whether frame is valid */
	BOOL            bSysBind;        /* whether is sysbind */
} AEC_FRAME_S;

typedef struct AUDIO_FRAME_COMBINE_S
{
    AUDIO_FRAME_S stFrm;                /* audio frame */
    AEC_FRAME_S   stRefFrm;             /* AEC reference audio frame */
} AUDIO_FRAME_COMBINE_S;

typedef struct AUDIO_FRAME_INFO_S
{
    AUDIO_FRAME_S *mpAFrame;/*frame ptr*/
    unsigned int  mId;   /*frame id*/
} AUDIO_FRAME_INFO_S;

typedef struct AUDIO_STREAM_S
{ 
    unsigned char       *pStream;       /* the virtual address of stream */
    unsigned int        mLen;           /* stream lenth, by bytes */
    unsigned long long  mTimeStamp;     /* frame time stamp */
    int                 mId;            /* frame id */
} AUDIO_STREAM_S;

typedef struct AUDIO_RESAMPLE_ATTR_S
{
    unsigned int            u32InPointNum;      /* input point number of frame */
    AUDIO_SAMPLE_RATE_E     enInSampleRate;     /* input sample rate */
    AUDIO_SAMPLE_RATE_E     enOutSampleRate;    /* output sample rate */
} AUDIO_RESAMPLE_ATTR_S;

typedef struct AO_CHN_STATE_S
{
    unsigned int            u32ChnTotalNum;    /* total number of channel buffer */
    unsigned int            u32ChnFreeNum;     /* free number of channel buffer */
    unsigned int            u32ChnBusyNum;     /* busy number of channel buffer */
} AO_CHN_STATE_S;

typedef struct AIO_RESMP_INFO_S
{
    BOOL                    bReSmpEnable;      /* resample enable or disable */
    AUDIO_RESAMPLE_ATTR_S   stResmpAttr;
} AIO_RESMP_INFO_S;

typedef enum AUDIO_TRACK_MODE_E
{
    AUDIO_TRACK_NORMAL      = 0,
    AUDIO_TRACK_BOTH_LEFT   = 1,
    AUDIO_TRACK_BOTH_RIGHT  = 2,
    AUDIO_TRACK_EXCHANGE    = 3,
    AUDIO_TRACK_MIX         = 4,
    AUDIO_TRACK_LEFT_MUTE   = 5,
    AUDIO_TRACK_RIGHT_MUTE  = 6,
    AUDIO_TRACK_BOTH_MUTE   = 7,

    AUDIO_TRACK_BUTT
} AUDIO_TRACK_MODE_E;

typedef enum AUDIO_CLKDIR_E
{
    AUDIO_CLKDIR_RISE      = 0,
    AUDIO_CLKDIR_FALL       = 1,

    AUDIO_CLKDIR_BUTT
} AUDIO_CLKDIR_E;

typedef enum AUDIO_FADE_RATE_E
{
    AUDIO_FADE_RATE_1	= 0,
    AUDIO_FADE_RATE_2	= 1,
    AUDIO_FADE_RATE_4	= 2,
    AUDIO_FADE_RATE_8   = 3,
    AUDIO_FADE_RATE_16  = 4,
    AUDIO_FADE_RATE_32  = 5,
    AUDIO_FADE_RATE_64  = 6,
    AUDIO_FADE_RATE_128 = 7,

    AUDIO_FADE_RATE_BUTT
} AUDIO_FADE_RATE_E;

typedef struct AUDIO_FADE_S
{
    BOOL              bFade;
    AUDIO_FADE_RATE_E enFadeInRate;
    AUDIO_FADE_RATE_E enFadeOutRate;
} AUDIO_FADE_S;
typedef enum AUDIO_AEC_MODE_E
{
    AUDIO_AEC_MODE_CLOSE	 = 0,
    AUDIO_AEC_MODE_OPEN	     = 1,

    AUDIO_AEC_MODE_BUTT
} AUDIO_AEC_MODE_E;

/**Defines the configure parameters of AGC.*/
typedef struct AUDIO_AGC_CONFIG_S
{
    BOOL bUsrMode;          /* mode 0: auto¡ê?mode 1: manual.*/

    signed char s8TargetLevel;       /* target voltage level, range: [-40, -1]dB */
    signed char s8NoiseFloor;        /* noise floor, range: [-65, -20]dB */
    signed char s8MaxGain;           /* max gain, range: [0, 30]dB */
    signed char s8AdjustSpeed;       /* adjustable speed, range: [0, 10]dB/s */

    signed char s8ImproveSNR;        /* switch for improving SNR, range: [0:close, 1:upper limit 3dB, 2:upper limit 6dB] */
    signed char s8UseHighPassFilt;   /* switch for using high pass filt, range: [0:close, 1:80Hz, 2:120Hz, 3:150:Hz, 4:300Hz: 5:500Hz] */
    signed char s8OutputMode;        /* output mode, mute when lower than noise floor, range: [0:close, 1:open] */
    short       s16NoiseSupSwitch;  /* switch for noise suppression, range: [0:close, 1:open] */

    int s32Reserved;
} AUDIO_AGC_CONFIG_S;

typedef struct AUDIO_GAIN_CONFIG_S
{
    signed char s8GainValue;           /*Value range: -20 -> 20 */

    int s32Reserved;
} AUDIO_GAIN_CONFIG_S;


//////////////the following aec paras
/* dynamic range control parameters */
typedef struct
{
    int sampling_rate;  /* */
    int target_level;
    int max_gain;
    int min_gain;
    int attack_time;
    int release_time;
    int noise_threshold; /* */
}DRC_PARAMS_T;

/* bulk delay compensation parameters */
typedef struct
{
    int sampling_rate; /* sampling rate, should be equal to the SR of echo canceller */
    int	block_size;  /* processing block size, should be equal to the frame size of echo canceller */
    int max_delay;  /* max delay for the far end signal */
    int delay_margin; /* delay magin, this is used for delay estimation variation 
                         in some case, the variation could cause uncaulity
                         delay_margin shold be positive and litter than max_delay */
    int near_delay; /* delay for the near end signal */
}BDC_PARAMS_T;

/* noise suppression parameters */
typedef struct
{
    int sampling_rate;    /* sampling rate */
    int max_suppression;  /* maximum amount of suppression allowed for noise reduction */
    int overlap_percent;  /* overlap of window, value from 0 ->1, 0:50%, 1:75%*/
    int nonstat;  /* stationarity of noise, value from 0 -> 3 */
}NS_PARAMS_T;

typedef struct
{
    int G;  /* boost/cut gain in dB */
    int fc; /* cutoff/center frequency in Hz */
    int	Q;  /* quality factor */
    int type; /* filter type,value from 0->2 */
              /* 0: low pass shelving filter */
              /* 1: band pass peak filter */
              /* 2: high pass shelving filter */
}EQ_CORE_PARAMS_T;

typedef struct
{
    int biq_num;  /*num of items(biquad)*/
    int sampling_rate;  /* sampling rate */
    EQ_CORE_PARAMS_T* core_prms;  /* eq parameters for generate IIR coeff*/
}EQ_PARAMS_T;

/* Nonlinear Processing parameters parameters */
typedef struct
{
    /* nlp overlap of window */
    int nlp_overlap_percent;  //value from 0 -> 1
}NLP_PARAMS_T;

/* echo cancellation parameters */
typedef struct
{
    /* sampling rate */
    int sampling_rate;
    /* echo path length, Unit ms, should be times of 8 */
    int tail_length;
    /* processing frame size, Unit ms, only 8ms is supported until now */
    int frame_size;
    /* Nonlinear Processing switch */
    int enable_nlp;
    /* Nonlinear Processing parameters */
    NLP_PARAMS_T nlp_prms;
}AEC_PARAMS_T;

/* total init parameters for echo control */
typedef struct
{
    /* echo canceller switch */
    int enable_aec;
    /* echo canceller parameters */
    AEC_PARAMS_T aec_prms;
    /* bulk delay compensator switch */
    int enable_bdc;
    /* bulk delay estimator parameters */
    BDC_PARAMS_T bdc_prms;
    /* clock drift compensator switch */
    int enable_cdc;
    /* dynamic range control switch */
    int enable_txdrc;
    /* transmitter drc  parameters */
    DRC_PARAMS_T txdrc_prms;
    /* receiver drc */
    int	enable_rxdrc;
    /*receiver drc  parameters */
    DRC_PARAMS_T rxdrc_prms;
    /* transmitter equalizer */
    int enable_txeq;
    /* transmitter equalizer parameters */
    EQ_PARAMS_T txeq_prms;
    /* receiver equalizer */
    int enable_rxeq;
    /* receiver equalizer parameters */
    EQ_PARAMS_T rxeq_prms;
    /* noise suppressor switch */
    int enable_ns;
    /* noise suppression parameters */
    NS_PARAMS_T ns_prms;
    /* fade in to suppress the initial echo */
    int enable_txfade;
}EC_PARAMS_T;
/////////////////////end aec struct


/**Defines the configure parameters of AEC.*/
typedef struct AI_AEC_CONFIG_S
{
    EC_PARAMS_T prms;
} AI_AEC_CONFIG_S;

/**Defines the configure parameters of ANR.*/
typedef struct AUDIO_ANR_CONFIG_S
{
    BOOL bUsrMode;   /* mode 0: auto¡ê?mode 1: manual.*/

    short s16NrIntensity;       /* noise reduce intensity, range: [0, 25] */
    short s16NoiseDbThr;        /* noise threshold, range: [30, 60] */
    signed char s8SpProSwitch;        /* switch for music probe, range: [0:close, 1:open] */

    int s32Reserved;
} AUDIO_ANR_CONFIG_S;

/**Defines the configure parameters of HPF.*/
typedef enum AUDIO_HPF_FREQ_E
{
    AUDIO_HPF_FREQ_80   = 80,    /* 80Hz */
    AUDIO_HPF_FREQ_120  = 120,   /* 120Hz */
    AUDIO_HPF_FREQ_150  = 150,   /* 150Hz */
    AUDIO_HPF_FREQ_BUTT,
} AUDIO_HPF_FREQ_E;

typedef struct AUDIO_HPF_CONFIG_S
{
    BOOL bUsrMode;   /* mode 0: auto mode 1: mannual.*/
    AUDIO_HPF_FREQ_E enHpfFreq; /*freq to be processed*/
} AUDIO_HPF_CONFIG_S;

typedef struct AI_RNR_CONFIG_S
{
    int sMaxNoiseSuppression; /*max noise depress threshold(dB), range:[-30, -12], default:-12*/
    int sOverlapPercent;  /*window overlap percent, range:[0, 1], default: 0*/
                          /*0:repeat 50%    1:repeat 75%*/
    int sNonstat;  /*noise smoothness, range:[0, 3], default:3*/
                   /*0:normal 1:weak unsmoothness  2:middle unsmoothness  3:strong unsmoothness*/
} AI_RNR_CONFIG_S;

typedef struct AI_DRC_CONFIG_S
{
    int sTargetLevel;
    int sMaxGain;
    int sMinGain;
    int sAttackTime;
    int sReleaseTime;
    int sNoiseThreshold;
} AI_DRC_CONFIG_S;

typedef enum AUDIO_EQ_MODE_S
{
    AUDIO_EQ_TYPE_NORMAL=0,         /* nature */
    AUDIO_EQ_TYPE_DBB,              /* dbb */
    AUDIO_EQ_TYPE_POP,              /* pop  */
    AUDIO_EQ_TYPE_ROCK,             /* rock */
    AUDIO_EQ_TYPE_CLASSIC,          /* classic */
    AUDIO_EQ_TYPE_JAZZ,             /* jazz */
    AUDIO_EQ_TYPE_VOCAL,            /* vocal */
    AUDIO_EQ_TYPE_DANCE,            /* dance */
    AUDIO_EQ_TYPE_SOFT,             /* soft */
    AUDIO_EQ_TYPE_USR_MODE=0xFF,    /* user */
    
    AUDIO_EQ_TYPE_MAX
}AUDIO_EQ_MODE_S;

typedef struct AUDIO_EQ_CONFIG_S
{
    short s16GaindB[VQE_EQ_BAND_NUM]; /*only in usermode valid, from[1]->[10], frequncy include 31,62,125,250,500,1k,2k,4k,8k,16k, range:[-12, 12]*/
	                                  //s16GaindB[0]= 0xFF -> user mode, PostProInfo->s8GaindB[1]-PostProInfo->s8GaindB[10] data valid, range:[-12, 12], frequency difined above
                                      //s16GaindB[0] <= AUDIO_EQ_TYPE_SOFT, typic type above defined: AUDIO_EQ_MODE_S {0: nature 1:dbb 2:pop ---> 8:soft}
    int s32Reserved;
} AUDIO_EQ_CONFIG_S;


/**Defines the configure parameters of UPVQE work state.*/
typedef enum VQE_WORKSTATE_E
{
    VQE_WORKSTATE_COMMON  = 0,   /* common environment, Applicable to the family of voice calls. */
    VQE_WORKSTATE_MUSIC   = 1,   /* music environment , Applicable to the family of music environment. */
    VQE_WORKSTATE_NOISY   = 2,   /* noisy environment , Applicable to the noisy voice calls.  */
} VQE_WORKSTATE_E;

/**Defines the configure parameters of VQE.*/
typedef struct AI_VQE_CONFIG_S
{
    int              bHpfOpen;
    int              bAecOpen;
    int              bAnrOpen; /*Anr and Rnr can't enable at the same time,Anr used in voice noise reducing*/
    int              bRnrOpen;
    int              bAgcOpen;
    int              bEqOpen;
    int              bDrcOpen;

    int              s32WorkSampleRate;  /* Sample Rate£º8KHz/16KHz¡£default: 8KHz*/
    int              s32FrameSample; /* VQE frame length£º 80-4096 */
    VQE_WORKSTATE_E     enWorkstate;

    AUDIO_HPF_CONFIG_S  stHpfCfg;
    AI_AEC_CONFIG_S     stAecCfg;
    AUDIO_ANR_CONFIG_S  stAnrCfg;
    AI_RNR_CONFIG_S     stRnrCfg;
    AUDIO_AGC_CONFIG_S  stAgcCfg;
    AUDIO_EQ_CONFIG_S   stEqCfg;
    AI_DRC_CONFIG_S     stDrcCfg;
} AI_VQE_CONFIG_S;

typedef struct AO_VQE_CONFIG_S
{
    int              bHpfOpen;
    int              bAnrOpen;
    int              bAgcOpen;
    int              bEqOpen;
    int              bGainOpen;

    int              s32WorkSampleRate;  /* Sample Rate£º8KHz/16KHz/48KHz¡£default: 8KHz*/
    int              s32FrameSample; /* VQE frame length£º 80-4096 */
    VQE_WORKSTATE_E     enWorkstate;

    AUDIO_HPF_CONFIG_S stHpfCfg;
    AUDIO_ANR_CONFIG_S stAnrCfg;
    AUDIO_AGC_CONFIG_S stAgcCfg;
    AUDIO_EQ_CONFIG_S  stEqCfg;
    AUDIO_GAIN_CONFIG_S stGainCfg;
} AO_VQE_CONFIG_S;


typedef struct AI_VQE_INFO_S
{
    BOOL                 bVqeEnable;      /* vqe enable or disable */
	AI_VQE_CONFIG_S         stAiVqeCfg;
} AI_VQE_INFO_S;

typedef struct AO_VQE_INFO_S
{
    BOOL                 bVqeEnable;      /* vqe enable or disable */
	AO_VQE_CONFIG_S         stAoVqeCfg;
} AO_VQE_INFO_S;

/**Defines the state of inner codec.*/
typedef struct AI_INNER_CODEC_STATE_S
{
    BOOL             bMicInl;
    BOOL             bMicInr;
} AI_INNER_CODEC_STATE_S;

/*Defines the configure parameters of AI saving file.*/
typedef struct AUDIO_SAVE_FILE_INFO_S
{
    BOOL    bCfg;
    char  	mFilePath[256];
    char  	mFileName[256];
    unsigned int mFileSize;  /*in Bytes*/
} AUDIO_SAVE_FILE_INFO_S;

typedef enum EN_AIO_ERR_CODE_E
{
    AIO_ERR_CHN_NOT_ENABLE = 64,

} EN_ADEC_ERR_CODE_E;


/* invlalid device ID */
#define ERR_AI_INVALID_DEVID     DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define ERR_AI_INVALID_CHNID     DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_AI_ILLEGAL_PARAM     DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_AI_EXIST             DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel exists */
#define ERR_AI_UNEXIST           DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define ERR_AI_NULL_PTR          DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define ERR_AI_NOT_CONFIG        DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define ERR_AI_NOT_SUPPORT       DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define ERR_AI_NOT_PERM          DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the devide is not enabled  */
#define ERR_AI_NOT_ENABLED       DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define ERR_AI_NOMEM             DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_AI_NOBUF             DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define ERR_AI_BUF_EMPTY         DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define ERR_AI_BUF_FULL          DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define ERR_AI_SYS_NOTREADY      DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

#define ERR_AI_BUSY              DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* component state is same as user wanted */
#define ERR_AI_SAMESTATE         DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_AI_INVALIDSTATE      DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_AI_INCORRECT_STATE_TRANSITION   DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_AI_INCORRECT_STATE_OPERATION    DEF_ERR(MOD_ID_AI, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)


/* invlalid device ID */
#define ERR_AO_INVALID_DEVID     DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_DEVID)
/* invlalid channel ID */
#define ERR_AO_INVALID_CHNID     DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define ERR_AO_ILLEGAL_PARAM     DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
/* channel exists */
#define ERR_AO_EXIST             DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
/* channel exists */
#define ERR_AO_UNEXIST           DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* using a NULL point */
#define ERR_AO_NULL_PTR          DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define ERR_AO_NOT_CONFIG        DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define ERR_AO_NOT_SUPPORT       DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define ERR_AO_NOT_PERM          DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
/* the devide is not enabled  */
#define ERR_AO_NOT_ENABLED       DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
/* failure caused by malloc memory */
#define ERR_AO_NOMEM             DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOMEM)
/* failure caused by malloc buffer */
#define ERR_AO_NOBUF             DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
/* no data in buffer */
#define ERR_AO_BUF_EMPTY         DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
/* no buffer for new data */
#define ERR_AO_BUF_FULL          DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define ERR_AO_SYS_NOTREADY      DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)

#define ERR_AO_BUSY              DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/* component state is same as user wanted */
#define ERR_AO_SAMESTATE         DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_SAMESTATE)
/* component state is transit to invalid state */
#define ERR_AO_INVALIDSTATE      DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_INVALIDSTATE)
/* component current state can't transit to destination state */
#define ERR_AO_INCORRECT_STATE_TRANSITION   DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_TRANSITION)
/* Attempting a command that is not allowed during the present state. */
#define ERR_AO_INCORRECT_STATE_OPERATION    DEF_ERR(MOD_ID_AO, EN_ERR_LEVEL_ERROR, EN_ERR_INCORRECT_STATE_OPERATION)


#ifdef __cplusplus
}
#endif

#endif /* __IPCLINUX_COMM_AI_H__ */


#ifndef	__BsInfoType__
#define	__BsInfoType__
#include "adecoder.h"

typedef struct __AFRAME_INFO
{
    alib_uint8       *startAddr;
    alib_uint32       ptsValid;
    alib_int64        pts;
    alib_uint32       len;
    alib_uint32       readlen;
}aframe_info_t;

typedef struct __ASTREAM_FIFO
{
    aframe_info_t*    inFrames;
    alib_uint32       rdIdx;
    alib_uint32       wtIdx;
    unsigned int      prefetchIdx;
    alib_int32        ValidchunkCnt;
    alib_uint32       maxchunkNum;
    alib_int64        NowPTSTime;
	alib_int64        dLastValidPTS;
}astream_fifo_t;


typedef	struct __Ac320FileRead
{
    alib_uint32    FileLength;  //文件总长度
    alib_uint32    FileReadingOpsition; //文件下次要读出位置的偏移
    alib_uint32    FileWritingOpsition; //buffer下次要写入位置的偏移
    alib_uint32    need_offset;
    alib_uint16    updataoffsetflag;
    alib_uint16    BigENDFlag;  //1 big 0 little
    alib_uint8    *bufStart;  //buffer首地址
    alib_int32     BufToTalLen;  //总长度
    alib_uint8    *bufReadingPtr;//正在读的指针
    alib_int32     BufLen;  //有效数据长度

    alib_uint8     *bufWritingPtr;//正在读的指针
    alib_int32      BufValideLen;  //空余长度
    astream_fifo_t *frmFifo;
    //Legarcy rw apis....
    alib_int32     (*ShowAbsBits)(void *buf, alib_int32 Len, void *mem);
    void           (*FlushAbsBits)(alib_int32 Len, void *mem);
    alib_int32     (*FREAD_AC320)(void *buf, alib_int32 Len, void *mem);
    alib_int32     (*FSeek_AC320)(alib_int32 Len, void *mem);
    alib_int32     (*FSeek_AC320_Origin)(alib_int32 Len, void *mem, alib_int32 origin);
    alib_int32     (*BigLitEndianCheck)(void *mem, alib_int16 data);
    //extern rw apis....
    void           (*FlushPtsAtOnce)(void *mem);
    void           *tmpGlobalAudioDecData;
}Ac320FileRead;

#define	AUD_TAG_INF_SIZE 512
typedef  struct com_internal_struct
{
    //0x88
    alib_uint32             *pdecInfoSet;
    alib_uint32             *pBackupBufferPoint;
    alib_uint32              ulBitstreamAddPtr;
    alib_uint32              ulDspVersionNum;
    alib_uint32              ulPlayStartTime;
    alib_uint32              ulPlayEndTime;
    alib_uint32              ulBreakSentenceEnergy;
    alib_uint32              ulBreakSentenceTime;
    alib_uint32              ulFileSumData;
    alib_uint32              ulForwardBackSilenceTime;
    alib_uint32              ulForwardBackPlayTime;
    alib_uint32             *SptrumoutAdd;
    alib_uint32             *UserEqinputAdd;

    alib_uint16               ulBackupLength;

    alib_uint8                ulNormalPlay;               //=ulFadeInOutFinish
    alib_uint8                ulDecComm;
    alib_uint8                ulFadeinFlag;
    //0x8b
    alib_uint8                ulFadeInOutFinish;
    //0x89
    alib_uint32               ulTotalTimeMS;
    alib_uint32               ulAverageBit;
    alib_int32                ulSampleRate;               //?why not this data is fact data;
    //0x8a
    alib_uint8                ulExitMean;
    alib_uint32               ulDebugInfo;
    //0x60
    alib_uint32               ulNowTimeMS;
    alib_uint32               ulNowTimeMSAPoint;
    alib_uint32               ulBitrate;
    alib_uint8                ulMode;
    alib_uint8                ulFFREVDoing;
    //0x70
    alib_int8                 ulVpsSet;
    alib_uint32               ulFFREVStep;    
    alib_uint8                ulFFREVFlag;
    alib_uint8                ulABSet;
    alib_uint8                ulEQFlag;
    alib_uint16               ulUserEq[10];
    alib_uint8                ulspectrum;
    alib_uint16               Hmspetrum;//1-8
    alib_uint16               ulspectrumval[8][32];
    alib_uint8                ulChannelControl;
    alib_uint8                ulVolumeSet;
    //0x61 a sentence end
    //0x62 dsp mips limit
    //0x63 dsp alive minimum time
    //0x40 decoder need new bitstream to decode
    alib_uint32               ulReadBitstreamLen;
    alib_int32                ulSkipPageLength;
    alib_uint8                urFadeOutFlag;
    alib_uint8                urFileFlag;
    alib_uint32               urTrueReadLen;
    alib_uint32               ulFilecurrentpage;
    //0x41
    alib_uint8                ulIndex;
    alib_uint32               ulDestinationAdd;
    //0x42
    alib_uint8                ulIndex1;
    alib_uint32               ulDestinationAdd1;
    //0x43
    alib_uint32               bWithVideo;
    alib_uint32               ulDestinationAdd2;
    //0x21
    alib_uint32              *ulBufferAddress;
    alib_uint32               ulBufferLength;

    alib_uint16               NeedSetPlayTimeFlag;
    alib_uint32               framecount;

    alib_uint8                AudTagInfo[AUD_TAG_INF_SIZE];
	alib_int8                 n_foramet;
} com_internal;

typedef struct __AudioDEC_AC320
{
    Ac320FileRead  *FileReadInfo;
    BsInFor        *BsInformation;
    com_internal   *DecoderCommand;
    alib_int32      Decinitedflag;
    alib_int32     (*DecInit)(struct __AudioDEC_AC320 *p);
    alib_int32     (*DecFrame)(struct __AudioDEC_AC320 *p,alib_int8 *OutBuffer,alib_int32 *OutBuffLen);
    alib_int32     (*DecExit)(struct __AudioDEC_AC320 *p);
    alib_int32     (*DecIoCtrl)(struct __AudioDEC_AC320 *p, alib_int32 nCmd, alib_int32 aux, void* param);
}AudioDEC_AC320;



#endif

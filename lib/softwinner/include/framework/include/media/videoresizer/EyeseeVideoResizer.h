#ifndef __IPCLINUX_EYESEEVIDEORESIZE_H__
#define __IPCLINUX_EYESEEVIDEORESIZE_H__

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <CallbackDispatcher.h>
#include <EyeseeMessageQueue.h>

#include "mm_common.h"
#include "mm_comm_sys.h"
#include "mm_comm_clock.h"
#include "mm_comm_demux.h"
#include "mm_comm_vdec.h"
#include "mm_comm_venc.h"
#include "mm_comm_mux.h"
#include <VREncBuffer.h>

typedef struct ResizerOutputDest {
    char *url;
    int fd;
    int32_t width;  //output video width
    int32_t height; //output video height

    int32_t frameRate;
    int32_t bitRate;
} ResizerOutputDest;

typedef struct output_sink_info_s
{
    int mMuxerId;
    MEDIA_FILE_FORMAT_E mOutputFormat;
    int mOutputFd;
    int mFallocateLen;
    BOOL mCallbackOutFlag;
}OUTSINKINFO_S;

typedef struct mux_chn_info_s
{
    OUTSINKINFO_S mSinkInfo;
    MUX_CHN_ATTR_S mMuxChnAttr;
    MUX_CHN mMuxChn;
    struct list_head mList;
}MUX_CHN_INFO_S;

namespace EyeseeLinux {

#define ENC_BACKUP_BUFFER_NUM	(90)

enum VideoResizerStates {
    VIDEO_RESIZER_STATE_ERROR        = 0,
    VIDEO_RESIZER_IDLE               = 1 << 0,
    VIDEO_RESIZER_INITIALIZED        = 1 << 1,
    VIDEO_RESIZER_PREPARING          = 1 << 2,
    VIDEO_RESIZER_PREPARED           = 1 << 3,
    VIDEO_RESIZER_STARTED            = 1 << 4,
    VIDEO_RESIZER_PAUSED             = 1 << 5,
    VIDEO_RESIZER_STOPPED            = 1 << 6,
    VIDEO_RESIZER_PLAYBACK_COMPLETE  = 1 << 7
};

typedef struct ResizerOutputDest {
    char *url;
    int fd;
    int32_t width;  //output video width
    int32_t height; //output video height

    int32_t frameRate;
    int32_t bitRate;

    PAYLOAD_TYPE_E videoEncFmt;
    MEDIA_FILE_FORMAT_E mediaFileFmt;
} ResizerOutputDest;

class EyeseeVideoResizer
{
public:
    EyeseeVideoResizer();
    virtual     ~EyeseeVideoResizer();
    status_t    setDataSource(const char *url);
    status_t    setDataSource(int fd);
    status_t    setDataSource(int fd, int64_t offset, int64_t length);
    void        setVideoSize(int32_t width, int32_t height);
    status_t    setVideoEncFmt(PAYLOAD_TYPE_E format);
    status_t    addOutputFormatAndOutputSink(MEDIA_FILE_FORMAT_E format, const char* url, int FallocateLen, bool callback_out_flag);

    status_t    setFrameRate(int32_t framerate);
    status_t    setBitRate(int32_t bitrate);
    status_t    prepare();
    status_t    start();
    status_t    stop();
    status_t    pause();
    status_t    seekTo(int msec);
    status_t    reset();
    status_t    getDuration(int *msec);
    std::shared_ptr<VREncBuffer>  getPacket();
    bool        VRThread();

private:
    status_t  start_l();
    status_t  pause_l();
    status_t  stop_l();
    status_t  DoSeekTo(int msec);
    status_t  seekTo_l(int msec);

    //status_t    setListener(const sp<IMediaVideoResizerClient> &listener);
    //sp<IMemory> getEncDataHeader();
    //sp<IMemory> getOneBsFrame();

public:
    class OnErrorListener
    {
    public:
		OnErrorListener(){}
		virtual ~OnErrorListener(){};
        virtual bool onError(EyeseeVideoResizer *pMp, int what, int extra) = 0;
    };
    void setOnErrorListener(OnErrorListener *pListener);

    class OnMessageListener
    {
    public:
        OnMessageListener(){}
        virtual ~OnMessageListener(){};
        virtual bool onMessage(EyeseeVideoResizer *pMp, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData) = 0;
    };
    void setOnMessageListener(OnMessageListener *pListener);

private:
    OnErrorListener *mOnErrorListener;
    OnMessageListener *mOnMessageListener;


private:
    class DoVRThread : public Thread
    {
    public:
        DoVRThread(EyeseeVideoResizer *pVideoResizer);
        status_t startThread();
        void stopThread();
        virtual bool threadLoop();
        enum VideoResizerMsgType
        {
            MsgType_SeekTo = 0x200,
            MsgType_Stop,
            MsgType_Exit,
        };

    private:
        friend class EyeseeVideoResizer;
        Mutex mWaitLock;
        EyeseeMessageQueue mMsgQueue;
        EyeseeVideoResizer* const mpVideoResizer;
    };

    DoVRThread *mpDoVRThread;

    char mDstFilePath[256];
    VideoResizerStates mCurrentState;
    Mutex mLock;
    int mSeekStartPosition;    //unit:ms
    bool mbSeekStart;

    // Src file size
    int mSrcVideoWidth;
    int mSrcVideoHeight;
    PAYLOAD_TYPE_E mSrcCodecType;
    int mSrcFrameRate;
    DEMUX_MEDIA_INFO_S mDemuxMediaInfo;

    // demux chn
    DEMUX_CHN mDmxChn;
    DEMUX_CHN_ATTR_S mDmxChnAttr;

    // clock chn
    CLOCK_CHN mClockChn;
    CLOCK_CHN_ATTR_S mClockChnAttr;

    // video dec chn
    VDEC_CHN  mVdecChn;
    VDEC_CHN_ATTR_S mVdecChnAttr;

    // video enc chn
    VENC_CHN mVencChn;
    VENC_CHN_ATTR_S mVencChnAttr;

    // mux chn
    MUX_GRP mMuxGrp;
    MUX_CHN mMuxChn;
    MUX_CHN_INFO_S mMuxChnInfo;
    MUX_GRP_ATTR_S mMuxGrpAttr;

    ResizerOutputDest mOutDest;

    int mDstMediaDuration;
    int mDstVideoEncProfile;
    VENC_RC_MODE_E mDstRcMode;

    std::list<std::shared_ptr<VREncBuffer>> mBufList;

    bool create_MUX_GRP_CHN();
    bool create_MUX_CHN();
    bool create_CLOCK_CHN();
    bool create_VDEC_CHN();
    bool create_VENC_CHN();
    void config_VENC_CHN_ARRT_S();
    void config_MUX_CHN_ARRT_S();
    void config_VDEC_CHN_ATTR_S();
    status_t notify(MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData);
    static ERRORTYPE MPPCallbackWrapper(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData);
    status_t pushOneBsFrame(CDXRecorderBsInfo *frame);
};

}
#endif  //__IPCLINUX_EYESEEVIDEORESIZE_H__


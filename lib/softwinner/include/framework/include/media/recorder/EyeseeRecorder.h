/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Used to record audio and video. The recording control is based on a
 * simple state machine (see below).
 *
 * <p><img src="{@docRoot}images/mediarecorder_state_diagram.gif" border="0" />
 * </p>
 *
 * <p>A common case of using MediaRecorder to record audio works as follows:
 *
 * <pre>MediaRecorder recorder = new MediaRecorder();
 * recorder.setAudioSource(MediaRecorder.AudioSource.MIC);
 * recorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
 * recorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
 * recorder.setOutputFile(PATH_NAME);
 * recorder.prepare();
 * recorder.start();   // Recording is now started
 * ...
 * recorder.stop();
 * recorder.reset();   // You can reuse the object by going back to setAudioSource() step
 * recorder.release(); // Now the object cannot be reused
 * </pre>
 *
 * <p>Applications may want to register for informational and error
 * events in order to be informed of some internal update and possible
 * runtime errors during recording. Registration for such events is
 * done by setting the appropriate listeners (via calls
 * (to {@link #setOnInfoListener(OnInfoListener)}setOnInfoListener and/or
 * {@link #setOnErrorListener(OnErrorListener)}setOnErrorListener).
 * In order to receive the respective callback associated with these listeners,
 * applications are required to create MediaRecorder objects on threads with a
 * Looper running (the main UI thread by default already has a Looper running).
 *
 * <p><strong>Note:</strong> Currently, MediaRecorder does not work on the emulator.
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about how to use MediaRecorder for recording video, read the
 * <a href="{@docRoot}guide/topics/media/camera.html#capture-video">Camera</a> developer guide.
 * For more information about how to use MediaRecorder for recording sound, read the
 * <a href="{@docRoot}guide/topics/media/audio-capture.html">Audio Capture</a> developer guide.</p>
 * </div>
 */
 
#ifndef __IPCLINUX_EYESEERECORDER_H__
#define __IPCLINUX_EYESEERECORDER_H__

#include <vector>
#include <map>

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>

#include <utils/Mutex.h>
#include <RecorderMode.h>
#include <mm_comm_region.h>

//#include <EyeseeCamera.h>
//#include <MediaCallbackDispatcher.h>
#include <CameraListener.h>
#include <CallbackDispatcher.h>
#include <type_camera.h>

//#include <EyeseeISE.h>

#include <vencoder.h>
//#include <system/audio.h>
#include <mm_comm_aio.h>
#include <mm_comm_aenc.h>
#include <mm_comm_tenc.h>
#include <mm_comm_venc.h>
#include <mm_comm_mux.h>

struct RecAVSync;

namespace EyeseeLinux {

#define MAX_FILE_SIZE       (4*1024*1024*1024LL)
#define ENC_BACKUP_BUFFER_NUM	(90)

class VEncBuffer
{
public:
	int total_size; //no meaning, from CDXRecorderBsInfo->total_size
	int stream_type; /* RawPacketType; 0:video, 1:audio data, 128:video data header, 129:audio data header */
	int data_size;
	long long pts;  //unit: different muxer has different unit, raw muxer is us, ts muxer is ms.
    //for video frame.
    int CurrQp;
    int avQp;
	int nGopIndex;      //index of gop
	int nFrameIndex;    //index of current frame in gop.
	int nTotalIndex;    //index of current frame in whole encoded frames.
	char *data;
};
/*
typedef enum {
    OSD_TYPE_OVERLAY = 1 << 0, //normal overlay(osd)
    OSD_TYPE_COVER = 1 << 1,   //cover mode(mask)
    OSD_TYPE_OVERLAY_LUMA_REVERT = 1 << 2,   //normal overlay(osd), luma color revert
}VENC_OSD_TYPE;

typedef enum {
    OSD_BITMAP_ARGB8888 = 1 << 0,
    OSD_BITMAP_ARGB4444 = 1 << 1,
    OSD_BITMAP_ARGB1555 = 1 << 2,
}VENC_OSD_BITMAP_COLOR;


struct VEncOSDRectInfo
{
    RECT_S mRect;
    VENC_OSD_BITMAP_COLOR mFormat; //MM_PIXEL_FORMAT_RGB_8888
    void *mpBuf;
    unsigned int coverYUVColor; //when use cover mode should set the cover yuv value
                                //unsigned char: Y/U/V; value=(Y<<16)|(U<<8)|V
    VENC_OSD_TYPE mOsdType;
    unsigned int nPriority; //the region priority, from 0->64, the big value will on the top
    std::string mComment;
};
*/

struct OutputSinkInfo
{
    int mMuxerId;
    MEDIA_FILE_FORMAT_E mOutputFormat;
    int mOutputFd;
    int mFallocateLen;
    bool mCallbackOutFlag;
    OutputSinkInfo()
    {
        mMuxerId = -1;
        mOutputFormat = MEDIA_FILE_FORMAT_DEFAULT;
        mOutputFd = -1;
        mFallocateLen = 0;
        mCallbackOutFlag = false;
    }
};

struct SinkParam
{
    MEDIA_FILE_FORMAT_E mOutputFormat;
    int   mOutputFd;
    char* mOutputPath;
    int   mFallocateLen;
    int   mMaxDurationMs;
    bool  bCallbackOutFlag;
    bool  bBufFromCacheFlag;
};

/*
 * The state machine of the media_recorder.
 */
enum media_recorder_states {
    // Error state.
    MEDIA_RECORDER_ERROR                 =      0,

    // Recorder was just created.
    MEDIA_RECORDER_IDLE                  = 1 << 0,

    // Recorder has been initialized.
    MEDIA_RECORDER_INITIALIZED           = 1 << 1,

    // Configuration of the recorder has been completed.
    MEDIA_RECORDER_DATASOURCE_CONFIGURED = 1 << 2,

    // Recorder is ready to start.
    MEDIA_RECORDER_PREPARED              = 1 << 3,

    // Recording is in progress.
    MEDIA_RECORDER_RECORDING             = 1 << 4,
};

// The "msg" code passed to the listener in notify.
enum media_recorder_event_type {
    MEDIA_RECORDER_EVENT_LIST_START               = 1,
    MEDIA_RECORDER_EVENT_ERROR                    = 1,
    MEDIA_RECORDER_EVENT_INFO                     = 2,
    MEDIA_RECORDER_EVENT_LIST_END                 = 99,

    // Track related event types
//    MEDIA_RECORDER_TRACK_EVENT_LIST_START         = 100,
//    MEDIA_RECORDER_TRACK_EVENT_ERROR              = 100,
//    MEDIA_RECORDER_TRACK_EVENT_INFO               = 101,
//    MEDIA_RECORDER_TRACK_EVENT_LIST_END           = 1000,
//
//    MEDIA_RECORDER_VENDOR_EVENT_LIST_START        = 3000,
//    MEDIA_RECORDER_VENDOR_EVENT_EMPTY_BUFFER_ID   = 3000,
    MEDIA_RECORDER_VENDOR_EVENT_BSFRAME_AVAILABLE = 3001,
//    MEDIA_RECORDER_VENDOR_EVENT_LIST_END		  = 3100,
};

enum callback_out_data_type {
    CALLBACK_OUT_DATA_VIDEO_ONLY = 0,
    CALLBACK_OUT_DATA_VIDEO_AUDIO = 1,
    CALLBACK_OUT_DATA_AUDIO_ONLY = 2,
    CALLBACK_OUT_DATA_LIST_END,
};

class CameraFrameManager;
class DynamicBitRateControl;
class EyeseeRecorder
{
private:
    class EventHandler;
public:
    enum VideoEncodeRateControlMode
    {
        VideoRCMode_CBR = 0,
        VideoRCMode_VBR = 1,
        VideoRCMode_FIXQP = 2,
        VideoRCMode_ABR = 3,
        VideoRCMode_QPMAP = 4,
    };
    enum VEncProfile
    {
        VEncProfile_BaseLine = 0,
        VEncProfile_MP = 1,
        VEncProfile_HP = 2,
        VEncProfile_SVC = 3,
    };
    struct VEncBitRateControlAttr
    {
        PAYLOAD_TYPE_E mVEncType;
        VideoEncodeRateControlMode mRcMode;                            /*the type of rc*/
        struct VEncAttrH264Cbr
        {
            unsigned int      mBitRate;                             /* average bitrate */
            unsigned int      mMaxQp;                               /* the max qp */
            unsigned int      mMinQp;                               /* the min qp */
        };
        struct VEncAttrH264Vbr
        {
            unsigned int      mMaxBitRate;                          /* the max bitrate */                      
            unsigned int      mMaxQp;                               /* the max qp */
            unsigned int      mMinQp;                               /* the min qp */
            unsigned int      mRatioChangeQp;                       //range[50,100], default:85
            int               mQuality;                             /* range[1,13], 1:worst quality, 13:bestquality */
        };
        struct VEncAttrH264FixQp
        {
            unsigned int      mIQp;                                 /* qp of the i frame */
            unsigned int      mPQp;                                 /* qp of the p frame */
        };
        struct VEncAttrH264Abr
        {
            unsigned int      mMaxBitRate;                          /* the max bitrate */
            unsigned int      mRatioChangeQp;                       /* range[50,100], default:85 */
            int               mQuality;                             //range[1,13], 1:worst quality, 13:best quality, recommend:8.
            unsigned int      mMinIQp;                              /* I frame qp lower limit */
            unsigned int      mMaxIQp;      //no support upper limit, I frame qp only use lower_limit(mMinIQp)
            unsigned int      mMaxQp;                               /* the max qp */
            unsigned int      mMinQp;                               /* the min qp */
        };
        struct VEncAttrMjpegCbr
        {
            unsigned int      mBitRate;                             /* average bitrate */
        };
        struct VEncAttrMjpegFixQp
        {
            unsigned int      mQfactor;                             /* image quality :[1,99]*/
        };
        typedef struct VEncAttrH264Cbr VEncAttrH265Cbr;
        typedef struct VEncAttrH264Vbr VEncAttrH265Vbr;
        typedef struct VEncAttrH264FixQp VEncAttrH265FixQp;
        typedef struct VEncAttrH264Abr VEncAttrH265Abr;
        union
        {
            VEncAttrH264Cbr mAttrH264Cbr;
            VEncAttrH264Vbr mAttrH264Vbr;
            VEncAttrH264FixQp mAttrH264FixQp;
            VEncAttrH264Abr mAttrH264Abr;
            VEncAttrMjpegCbr mAttrMjpegCbr;
            VEncAttrMjpegFixQp mAttrMjpegFixQp;
            VEncAttrH265Cbr    mAttrH265Cbr;
            VEncAttrH265Vbr    mAttrH265Vbr;
            VEncAttrH265FixQp  mAttrH265FixQp;
            VEncAttrH265Abr    mAttrH265Abr;
        };   
    };
    struct VEncAttr
    {
        PAYLOAD_TYPE_E  mType;                         /*the type of payload*/
        unsigned int mBufSize;  /*stream buffer size, 0: use default */
        unsigned int mThreshSize;  /* threshSize in stream buffer, must < stream buffer size, 0 means auto config*/
        struct VEncAttrH264
        {
            unsigned int mProfile;                             /*0: baseline; 1:MP; 2:HP; 3: SVC-T [0,3]; */          
            H264_LEVEL_E mLevel;   //VENC_H264LEVELTYPE
        };
        struct VEncAttrH265
        {
            unsigned int  mProfile;         /*0: MP */
            H265_LEVEL_E mLevel;   //VENC_H264LEVELTYPE
        };
        union
        {
            VEncAttrH264 mAttrH264;
            VEncAttrH265 mAttrH265;
        };
    };
    struct BufferState
    {
        unsigned int mValidSizePercent;  // 0~100
        unsigned int mValidSize;         // unit:kB
        unsigned int mTotalSize;         // unit:kB
    };
    class OnErrorListener;
    class OnInfoListener;
    class OnDataListener;
    const unsigned int mRecorderId;
private:
    EventHandler *mEventHandler;        //I create, internal component call.
    OnErrorListener *mOnErrorListener;  //I call user.
    OnInfoListener *mOnInfoListener;
    OnDataListener *mOnDataListener;
    int mSurface;

    static Mutex msLock;

    class EventHandler : public CallbackDispatcher
    {
    private:
//      static const int MEDIA_RECORDER_EVENT_LIST_START = 1;
//      static const int MEDIA_RECORDER_EVENT_ERROR      = 1;
//      static const int MEDIA_RECORDER_EVENT_INFO       = 2;
//      static const int MEDIA_RECORDER_EVENT_LIST_END   = 99;
//      static const int MEDIA_RECORDER_TRACK_EVENT_LIST_START = 100;
//      static const int MEDIA_RECORDER_TRACK_EVENT_ERROR      = 100;
//      static const int MEDIA_RECORDER_TRACK_EVENT_INFO       = 101;
//      static const int MEDIA_RECORDER_TRACK_EVENT_LIST_END   = 1000;

        EyeseeRecorder *mpMediaRecorder;

    public:
        EventHandler(EyeseeRecorder *pC);
        ~EventHandler();
        virtual void handleMessage(const CallbackMessage &msg);
    };
    bool process_media_recorder_call(status_t opStatus, const char* message);

private:
    static unsigned int gRecorderIdCounter;
    volatile media_recorder_states mCurrentState;
    bool                        mIsAudioSourceSet;
    bool                        mIsVideoSourceSet;
    bool                        mIsAudioEncoderSet;
    bool                        mIsVideoEncoderSet;
    bool                        mIsOutputFileSet;
    Mutex                       mLock;
    Mutex                       mNotifyLock;
    Mutex                       mSendFrameLock;
    //camera
    CameraRecordingProxy *mpCameraProxy;
    int mCameraSourceChannel;   //camera source channel id.
    class CameraProxyListener: public CameraRecordingProxyListener  //I set to camera, camera call.
    {
    public:
        CameraProxyListener(EyeseeRecorder *recorder);
        virtual void dataCallbackTimestamp(const void *pdata);

    private:
        EyeseeRecorder * mRecorder;
    };

    //config
    int mVideoSource;   //VideoSource::CAMERA
    int mAudioSource;   //AudioSource::MIC
    bool mTimeLapseEnable;
    int64_t mTimeBetweenFrameCapture;   //in timeLapse mode, two selected encoded frame's original pts interval. if 0, means slow record mode. unit:us
    int mRotationDegrees;  // 0, 90, 180, 270. video frame anti-clockwise roration. It means vdeclib need Clockwise rotate to get normal picture.
    bool mGeoAvailable; //geography info is available.
    float mLatitude;
    float mLongitude;
    int mFrameRate; //e.g.: 25,30. dstFrameRate, decide encoder output frame rate.

    VEncAttr mVEncAttr;
    PAYLOAD_TYPE_E mVideoEncoder;
    int mVideoWidth;    //video size after hw encode.
    int mVideoHeight;
    int mVideoMaxKeyItl;
    int mIQpOffset;  //for h264 and h265
    int mFastEncFlag;   //for h264
    bool mbPIntraEnable;    //for h264,h265    
    VEncBitRateControlAttr mVEncRcAttr;
    VideoEncodeRateControlMode mVideoRCMode;
    //int mMaxQp; //if fixqp: PQp -> common param for all RC(except for FixQp)
    //int mMinQp; //if fixqp: IQp -> common param for all RC(except for FixQp)
    
    //bool mbLongTermRef; //for h264, h265
    bool mNullSkipEnable;
    bool mPSkipEnable;
    bool mbHorizonfilp;
    bool mbAdaptiveintrainp; //for h265
    int mb3DNR; //[0, 6], 0(close, default), recommend:3, strongest: 6
    VENC_SUPERFRAME_CFG_S mVencSuperFrameCfg;
    VencSaveBSFile mSaveBSFileParam;
    VeProcSet mVeProcSet;

    bool mbColor2Grey;
    //int mMaxVideoBitRate;    //for ABR and VBR
    //int mAbrRatioChangeQp; //for ABR. range[50,100], default:85
    //int mAbrQuality;       //for ABR. range[1,13], 1:worst quality, 13:best quality, recommend:8.
    //int mMaxIQp;    //for h265ABR -> not use anymore, ABR only use lower limit(mMinIQp)
    //int mMinIQp;    //for h265ABR and h264ABR
    //int mIQp;       // only for h264FixQp and h265FixQp
    //int mPQp;       // only for h264FixQp and h265FixQp
    VencSmartFun mSmartPParam;
    VENC_PARAM_INTRA_REFRESH_S mIntraRefreshParam;   
    bool mMuteMode; //true:mute; false:unmute.
    PAYLOAD_TYPE_E mAudioEncoder;
    int mSampleRate;
    int mAudioChannels; //number of audio channels.
    int mAudioBitRate;
    //volatile int mVideoBitRate;
    //int mVideoEncodingBufferTime;   //unit:ms
    bool mAttachAacHeaderFlag;
    std::vector<OutputSinkInfo> mSinkInfos;
    int64_t mMaxFileDuration; //unit:ms
    //int64_t mImpactFileDuration[2]; //unit:ms
    int mMuxCacheDuration;  //unit:ms
    int64_t mMaxFileSizeBytes;  //unit:byte
    int mMuxerIdCounter;
    callback_out_data_type mCallbackOutDataType;   //CALLBACK_OUT_DATA_VIDEO_ONLY
    Mutex mEncBufLock;
    std::list<VEncBuffer> mIdleEncBufList;
    std::list<VEncBuffer> mReadyEncBufList;
    std::list<VEncBuffer> mGetOutEncBufList;

    //MPP components
    VENC_CHN mVeChn;
    AUDIO_DEV mAiDev;
    AI_CHN mAiChn;
    AENC_CHN mAeChn;
    TENC_CHN mTeChn;

    int64_t last_frm_pts;
    int64_t frm_cnt;
    int gps_state;
    
    VENC_CHN_ATTR_S mVEncChnAttr;
    VENC_PARAM_REF_S mVEncRefParam;
    long long rec_start_timestamp;  // added to record the pts of the first audio frm sent to a enc
    
    AIO_ATTR_S mAioAttr;
    AENC_CHN_ATTR_S mAEncChnAttr;
    TENC_CHN_ATTR_S mTEncChnAttr;

    MUX_GRP mMuxGrp;
    MUX_GRP_ATTR_S mMuxGrpAttr;
    std::vector<MUX_CHN> mMuxChns;    //belong to mux group, match with mMuxChnAttrs
    std::vector<MUX_CHN_ATTR_S> mMuxChnAttrs;    //for addOutputFormatAndOutputSink(). match with mMuxChns
    std::map<int, RecordFileDurationPolicy> mPolicy;
    
    RecAVSync *mAVSync;
    CameraFrameManager *mpInputFrameManager;

    
    //std::list<VEncOSDRectInfo> mOSDRects;
    std::list<RGN_HANDLE> mRgnHandleList;
    Mutex mRgnLock;

    bool mEnableDBRC;
    friend class DynamicBitRateControl;
    DynamicBitRateControl *mpDBRC;

public:
    /**
     * Default constructor.
     */
    EyeseeRecorder();
    ~EyeseeRecorder();
    /**
     * Sets a Camera to use for recording. Use this function to switch
     * quickly between preview and capture mode without a teardown of
     * the camera object. {@link android.hardware.Camera#unlock()} should be
     * called before this. Must call before prepare().
     *
     * @param c the Camera to use for recording
     */
    //status_t setCamera(EyeseeCamera *pC);
    //status_t setCamera(EyeseeCamera *pC, int channelId);
    /**
     * Sets a ISE(Image Stitching Engine) to use for recording. 
     * Must call before prepare().
     * Camera and ISE can't be set at the same time.
     *
     * @param pISE the ISE to use for recording
     */
    //status_t setISE(EyeseeISE *pISE);
    //status_t setISE(EyeseeISE *pISE, int channelId);
    /**
     * Set Camera channel or ISE channel to use for recording. Use this function to select 
     * frame output channel of camera or ISE. Must call before prepare().
     *
     * @param channelId camera/ISE channel id.
     */
    //status_t setSourceChannel(int channelId);

    /**
     * Set Camera camera proxy for recording. Must call before prepare().
     * EyeseeRecorder has a responsible to free pCameraProxy. Caller don't free pCameraProxy.
     * @param channelId camera/ISE channel id.
     */
    status_t setCameraProxy(CameraRecordingProxy *pCameraProxy, int channelId);

    status_t setAudioVolume(int val);
    status_t getAudioVolume(int *pVal);
    /**
     * Defines the audio source. These constants are used with
     * {@link MediaRecorder#setAudioSource(int)}.
     */
    class AudioSource {
      /* Do not change these values without updating their counterparts
       * in system/core/include/system/audio.h!
       */
    private:
        AudioSource() {};
    public:

        /** Default audio source **/
        static const int DEFAULT = 0;

        /** Microphone audio source */
        static const int MIC = 1;

        /** Voice call uplink (Tx) audio source */
        static const int VOICE_UPLINK = 2;

        /** Voice call downlink (Rx) audio source */
        static const int VOICE_DOWNLINK = 3;

        /** Voice call uplink + downlink audio source */
        static const int VOICE_CALL = 4;

        /** Microphone audio source with same orientation as camera if available, the main
         *  device microphone otherwise */
        static const int CAMCORDER = 5;

        /** Microphone audio source tuned for voice recognition if available, behaves like
         *  {@link #DEFAULT} otherwise. */
        static const int VOICE_RECOGNITION = 6;

        /** Microphone audio source tuned for voice communications such as VoIP. It
         *  will for instance take advantage of echo cancellation or automatic gain control
         *  if available. It otherwise behaves like {@link #DEFAULT} if no voice processing
         *  is applied.
         */
        static const int VOICE_COMMUNICATION = 7;

        /**
         * @hide
         * Audio source for remote submix.
         */
        static const int REMOTE_SUBMIX_SOURCE = 8;
        
      /** FM audio source */
        static const int VOICE_FM = 9;
    };
    /**
     * Defines the video source. These constants are used with
     * {@link MediaRecorder#setVideoSource(int)}.
     */
    class VideoSource {
      /* Do not change these values without updating their counterparts
       * in include/media/mediarecorder.h!
       */
    private:
        VideoSource() {};
    public:
        static const int DEFAULT = 0;
        /** Camera video source */
        static const int CAMERA = 1;
        /** @hide */
        static const int GRALLOC_BUFFER = 2;
    };

    /**
     * Set video frame capture rate. This can be used to set a different video frame capture
     * rate than the recorded video's playback rate. This method also sets the recording mode
     * to time lapse. In time lapse video recording, only video is recorded. Audio related
     * parameters are ignored when a time lapse recording session starts, if an application
     * sets them.
     *
     * @param fps Rate at which frames should be captured in frames per second.
     * The fps can go as low as desired. However the fastest fps will be limited by the hardware.
     * For resolutions that can be captured by the video camera, the fastest fps can be computed using
     * {@link android.hardware.Camera.Parameters#getPreviewFpsRange(int[])}. For higher
     * resolutions the fastest fps may be more restrictive.
     * Note that the recorder cannot guarantee that frames will be captured at the
     * given rate due to camera/encoder limitations. However it tries to be as close as
     * possible.
     */
    status_t setCaptureRate(double fps);

    /**
     * enable slow record mode: reset every frame pts by dst frame rate.
     */
    status_t setSlowRecordMode(bool bEnable);
    /**
     * Sets the orientation hint for output video playback.
     * This method should be called before prepare(). This method will not
     * trigger the source video frame to rotate during video recording, but to
     * add a composition matrix containing the rotation angle in the output
     * video if the output format is OutputFormat.THREE_GPP or
     * OutputFormat.MPEG_4 so that a video player can choose the proper
     * orientation for playback. Note that some video players may choose
     * to ignore the compostion matrix in a video during playback.
     *
     * @param degrees the angle to be rotated clockwise in degrees.
     * The supported angles are 0, 90, 180, and 270 degrees.
     * @throws IllegalArgumentException if the angle is not supported.
     *
     */
    status_t setOrientationHint(int degrees);
    /**
     * Set and store the geodata (latitude and longitude) in the output file.
     * This method should be called before prepare(). The geodata is
     * stored in udta box if the output format is OutputFormat.THREE_GPP
     * or OutputFormat.MPEG_4, and is ignored for other output formats.
     * The geodata is stored according to ISO-6709 standard.
     *
     * @param latitude latitude in degrees. Its value must be in the
     * range [-90, 90].
     * @param longitude longitude in degrees. Its value must be in the
     * range [-180, 180].
     *
     * @throws IllegalArgumentException if the given latitude or
     * longitude is out of range.
     *
     */
    status_t setLocation(float latitude, float longitude);

    /**
     * Sets the format of the output file produced during recording. Call this
     * after setAudioSource()/setVideoSource() but before prepare().
     *
     * <p>It is recommended to always use 3GP format when using the H.263
     * video encoder and AMR audio encoder. Using an MPEG-4 container format
     * may confuse some desktop players.</p>
     *
     * @param output_format the output format to use. The output format
     * needs to be specified before setting recording-parameters or encoders.
     * @throws IllegalStateException if it is called after prepare() or before
     * setAudioSource()/setVideoSource().
     * @see android.media.MediaRecorder.OutputFormat
     */
    //status_t setOutputFormat(MEDIA_FILE_FORMAT_E output_format);

    /**
     * Sets the frame rate of the video to be encoded.  Must be called
     * after setVideoSource(). Call this after setOutFormat() but before
     * prepare().
     *
     * @param rate the number of frames per second of video to encode. It may be not equal to the camera capture rate.
     *      video encoder will take frame rate to calculate video frame pts if it is not set.
     * @throws IllegalStateException if it is called after
     * prepare() or before setOutputFormat().
     *
     * NOTE: On some devices that have auto-frame rate, this sets the
     * maximum frame rate, not a constant frame rate. Actual frame rate
     * will vary according to lighting conditions.
     */
    status_t setVideoFrameRate(int fps);

    status_t setVEncAttr(VEncAttr *pVEncAttr);
    /**
     * Sets the video encoder to be used for recording. If this method is not
     * called, the output file will not contain an video track. Call this after
     * setOutputFormat() and before prepare().
     *
     * @param video_encoder the video encoder to use.
     * @throws IllegalStateException if it is called before
     * setOutputFormat() or after prepare()
     * @see android.media.MediaRecorder.VideoEncoder
     */
    status_t setVideoEncoder(PAYLOAD_TYPE_E video_encoder);
    /**
     * Sets the width and height of the video to be captured.  Must be called
     * after setVideoSource(). Call this after setOutFormat() but before
     * prepare().
     * Caution: video size is not capture size, it indicate the size after hw encode.
     * 
     * @param width the width of the video to be captured
     * @param height the height of the video to be captured
     * @throws IllegalStateException if it is called after
     * prepare() or before setOutputFormat()
     */
    status_t setVideoSize(int width, int height);
    /**
     * set key frame interval to video encoder
     */
    status_t setVideoEncodingIFramesNumberInterval(int nMaxKeyItl);
    status_t setIQpOffset(int nIQpOffset);  //[0,10). default 0, if want to decrease I frame size, increase IQpOffset to 6 in common.
    status_t enableFastEncode(bool enable);
    status_t enableVideoEncodingPIntra(bool enable);
    /**
     * Set VideoEncoder BitRate Control attribute.
     * Must be called after setVideoEncoder() && setVideoEncodingRateControlMode().
     */
    status_t setVEncBitRateControlAttr(VEncBitRateControlAttr& RcAttr);
    status_t getVEncBitRateControlAttr(VEncBitRateControlAttr& RcAttr);
    status_t setVideoEncodingRateControlMode(VideoEncodeRateControlMode rcMode);
    status_t setGopAttr(const VENC_GOP_ATTR_S *pParam);
    status_t getGopAttr(VENC_GOP_ATTR_S *pParam);
    /**
     * Sets the video encoding bit rate for recording. Call this method before prepare().
     * Prepare() may perform additional checks on the parameter to make sure whether the
     * specified bit rate is applicable, and sometimes the passed bitRate will be
     * clipped internally to ensure the video recording can proceed smoothly based on
     * the capabilities of the platform.
     *
     * @param bitRate the video encoding bit rate in bits per second.
     */
    //status_t setVideoEncodingBitRate(int bitRate);
    /**
     * set buffer time to video encoder. unit:ms
     */
    //status_t setVideoEncodingBufferTime(int nBufferTime);
    //status_t setVideoEncodingBitRateSync(int bitRate);
    //status_t setVideoFrameRateSync(int frames_per_second);
    /**
 * Dynamic adjust vencoder param which list below:
 * 1.BitRate; 2.Qp; 3.IFrameInterval
 * Call this method only after start().
 */
    /**
     * for h264VBR: minqp ~ maxqp
     * for h264FIXQP: minqp is IQp, maxqp is PQp.
     * for mjpegFIXQP: minqp is Qfactor, maqqp mean nothing.
     * set H264 encoder max and min qp value, the value must be in the
     * interval[1,51], interval[5,45] is recommended.
     */
    //status_t SetVideoEncodingQpRange(int minqp, int maxqp);
    /**
     * set virtual I frame interval to video encoder, 0 or [5, IFrameInterval)
     * 0: disable virtual I frame.
     */
    //status_t setVirtualIFrameInterval(int nVirtualIFrameInterval);
    /**
     * force video encoder to encode a key frame immediately.
     */
    status_t reencodeIFrame();

    status_t SetVideoEncodingIntraRefresh(VENC_PARAM_INTRA_REFRESH_S *pIntraRefresh);

    /**
     * VencSmartFun->smart_fun_en = 1
     *              img_bin_en = 1;
     *              img_bin_th: take no effect.
     *              shift_bits = 2; [1,3]
     */
    status_t setVideoEncodingSmartP(VencSmartFun *pParam);//
    
    status_t setRefParam(const VENC_PARAM_REF_S * pstRefParam);
    status_t getRefParam(VENC_PARAM_REF_S * pstRefParam);

    status_t enableHorizonFlip(bool enable);
    status_t enable3DNR(int nLevel);    //nLevel: [0, 6], 0(close, default), recommend:3, strongest: 6
    status_t get3DNRLevel(int *p3DNRLevel);
    status_t enableSaveBsFile(VencSaveBSFile *pSaveParam);
    status_t setProcSet(VeProcSet *pVeProcSet);
    status_t enableColor2Grey(bool enable);
    status_t enableAdaptiveIntraInp(bool enable);
    
    status_t enableIframeFilter(bool enable);
    status_t enablePSkip(bool enable);
    status_t enableNullSkip(bool enable);
    //status_t enableVideoEncodingLongTermRef(bool enable);
    status_t setVideoEncodingMode(int Mode); /* VIDEO_CODING_MODE_FRAME:frame coding, VIDEO_CODING_MODE_FIELD:field coding, default value VIDEO_CODING_MODE_FRAME */
    status_t setVideoSliceHeight(int sliceHeight);
    //status_t setIQpRange(int maxIQp, int minIQp);  // for h264ABR and h265ABR, [20,40] is commanded.
    //status_t setIQpAndPQp(int nIQp, int nPQp);  // for h264FixQp and h265FixQp, [1,51] is commanded.
    //status_t setMaxVideoBitRate(int maxBitRate);  // for h265ABR
    //status_t setAbrRatioChangeQp(int nAbrRatioQp); // for ABR
    //status_t setAbrQuality(int nAbrQuality);  // for ABR, range[1,13], 1:worst quality, 13:best quality, recommend:8.

    /**
     * set h264,h265 encode profile.
     * This API's function will be merged into {@link #setVEncAttr(VEncAttr *pVEncAttr)}
     * @deprecated
     */
    //status_t setVEncProfile(VEncProfile nProfile);  //0:baseline; 1:main; 2:high
    status_t setRoiCfg(VENC_ROI_CFG_S *pVencRoiCfg);
    status_t getRoiCfg(unsigned int nIndex, VENC_ROI_CFG_S *pVencRoiCfg);
    status_t setVencSuperFrameConfig(VENC_SUPERFRAME_CFG_S *pSuperFrameConfig);

    status_t setMuteMode(bool mute);
    status_t enableDBRC(bool enable);

    /**
     * Sets the maximum duration (in ms) of the recording session.
     * Call this after setOutFormat() but before prepare().
     * After recording reaches the specified duration, a notification
     * will be sent to the {@link android.media.MediaRecorder.OnInfoListener}
     * with a "what" code of {@link #MEDIA_RECORDER_INFO_MAX_DURATION_REACHED}
     * and recording will be stopped. Stopping happens asynchronously, there
     * is no guarantee that the recorder will have stopped by the time the
     * listener is notified.
     *
     * @param max_duration_ms the maximum duration in ms (if zero or negative, disables the duration limit)
     *
     */
     //we change operation of mediaRecord, it will not stop after send a message, until apk command it to stop.
    status_t setMaxDuration(int max_duration_ms);
    status_t setMaxDuration(int muxer_id, int max_duration_ms);

    /**
     * Sets the maximum filesize (in bytes) of the recording session.
     * Call this after setOutFormat() but before prepare().
     * After recording reaches the specified filesize, a notification
     * will be sent to the {@link android.media.MediaRecorder.OnInfoListener}
     * with a "what" code of {@link #MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED}
     * and recording will be stopped. Stopping happens asynchronously, there
     * is no guarantee that the recorder will have stopped by the time the
     * listener is notified.
     *
     * @param max_filesize_bytes the maximum filesize in bytes (if zero or negative, disables the limit)
     *
     */
    /* we change operation of mediaRecord, it will not send MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED. 
     * it will send MEDIA_RECORDER_INFO_NEED_SET_NEXT_FD when max file size will be reached, 
     * it will end file when reach max file size, then send MEDIA_RECORDER_INFO_RECORD_FILE_DONE,
     * after that, it will keep encoding but not write to disk until a new file descriptor is set.
     */
    status_t setMaxFileSize(int64_t max_filesize_bytes);

    /**
     * Sets the maximum filesize (in bytes) of the recording session.
     * Call this after setOutFormat() but before prepare().
     * After recording reaches the specified filesize, a notification
     * will be sent to the {@link android.media.MediaRecorder.OnInfoListener}
     * with a "what" code of {@link #MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED}
     * and recording will be stopped. Stopping happens asynchronously, there
     * is no guarantee that the recorder will have stopped by the time the
     * listener is notified.
     *
     * @param max_filesize_bytes the maximum filesize in bytes (if zero or negative, disables the limit)
     *
     */
    //status_t setMaxFileSize(int64_t max_filesize_bytes);

    /**
     * Sets the audio encoder to be used for recording. If this method is not
     * called, the output file will not contain an audio track. Call this after
     * setOutputFormat() but before prepare().
     *
     * @param audio_encoder the audio encoder to use.
     * @throws IllegalStateException if it is called before
     * setOutputFormat() or after prepare().
     * @see android.media.MediaRecorder.AudioEncoder
     */
    status_t setAudioEncoder(PAYLOAD_TYPE_E audio_encoder);


    /**
     * Sets the audio sampling rate for recording. Call this method before prepare().
     * Prepare() may perform additional checks on the parameter to make sure whether
     * the specified audio sampling rate is applicable. The sampling rate really depends
     * on the format for the audio recording, as well as the capabilities of the platform.
     * For instance, the sampling rate supported by AAC audio coding standard ranges
     * from 8 to 96 kHz, the sampling rate supported by AMRNB is 8kHz, and the sampling
     * rate supported by AMRWB is 16kHz. Please consult with the related audio coding
     * standard for the supported audio sampling rate.
     *
     * @param samplingRate the sampling rate for audio in samples per second.
     */
    status_t setAudioSamplingRate(int samplingRate);

    /**
     * Sets the number of audio channels for recording. Call this method before prepare().
     * Prepare() may perform additional checks on the parameter to make sure whether the
     * specified number of audio channels are applicable.
     *
     * @param numChannels the number of audio channels. Usually it is either 1 (mono) or 2
     * (stereo).
     */
    status_t setAudioChannels(int numChannels);

    /**
     * Sets the audio encoding bit rate for recording. Call this method before prepare().
     * Prepare() may perform additional checks on the parameter to make sure whether the
     * specified bit rate is applicable, and sometimes the passed bitRate will be clipped
     * internally to ensure the audio recording can proceed smoothly based on the
     * capabilities of the platform.
     *
     * @param bitRate the audio encoding bit rate in bits per second.
     * Only useful when G726, it can be set 16k~40k.
     * There's 4 formats by G726 encoder: 2/3/4/5bits -- 16/24/32/40kbps
     */
    status_t setAudioEncodingBitRate(int bitRate);

    /**
     * Pass in the file descriptor of the file to be written. Call this after
     * setOutputFormat() but before prepare().
     *
     * @param fd an open file descriptor to be written into.
     * @throws IllegalStateException if it is called before
     * setOutputFormat() or after prepare()
     */
    //void setOutputFile(int fd);
    //void setOutputFile(int fd, int64_t length);

    /**
     * Sets the path of the output file to be produced. Call this after
     * setOutputFormat() but before prepare().
     *
     * @param path The pathname to use.
     * @throws IllegalStateException if it is called before
     * setOutputFormat() or after prepare()
     */
    //void setOutputFile(char* path);
    //void setOutputFile(char* path, int64_t length);

    /**
     * Sets the audio source to be used for recording. If this method is not
     * called, the output file will not contain an audio track. The source needs
     * to be specified before setting recording-parameters or encoders. Call
     * this only before setOutputFormat().
     *
     * @param audio_source the audio source to use
     * @throws IllegalStateException if it is called after setOutputFormat()
     * @see android.media.MediaRecorder.AudioSource
     *
     * MEDIA_RECORDER_IDLE->MEDIA_RECORDER_INITIALIZED
     */
    status_t setAudioSource(int audio_source);


    /**
     * Sets the video source to be used for recording. If this method is not
     * called, the output file will not contain an video track. The source needs
     * to be specified before setting recording-parameters or encoders. Call
     * this only before setOutputFormat().
     *
     * @param video_source the video source to use
     * @throws IllegalStateException if it is called after setOutputFormat()
     * @see android.media.MediaRecorder.VideoSource
     *
     * MEDIA_RECORDER_IDLE->MEDIA_RECORDER_INITIALIZED
     */
    status_t setVideoSource(int video_source);

    /** 
     * Add the out format and the path of the output file to be produced. The 
     * output sink can be file, or transport out by RTSP. The muxer can support 
     * more than one. Call this before prepare().
     * One output_format is one muxer.
     * Don't call setOutputFormat() and setOutputFile(), when use addOutputFormatAndOutputSink().
     *
     * @return muxerId>=0 if success. muxerId=-1 indicate fail.
     * @param output_format The muxer file format.
     *      @see android.media.MediaRecorder.OutputFormat
     * @param fd The fd of file to use. -1 indicate no fd.
     * @param callback_out_flag Indicate if transport out by RTSP. e.g, "http://mnt/extsd/HVGA_mp4_0.mp4"
     *
     * MEDIA_RECORDER_INITIALIZED->MEDIA_RECORDER_DATASOURCE_CONFIGURED
     */
    int addOutputFormatAndOutputSink(MEDIA_FILE_FORMAT_E output_format, int fd, int FallocateLen, bool callback_out_flag);
    int addOutputFormatAndOutputSink(MEDIA_FILE_FORMAT_E output_format, char* path, int FallocateLen, bool callback_out_flag);
    int addOutputSink(SinkParam *pSinkParam);

    /**
     *  To enable the packing feature of gps info to ts container.
     *  this api must be called before prepare() api,since some actions
     *  related with gps enc will be performed in the body of prepare() function 
     *  according to the setting of gpsInfoEn().
     *  
     * @param gps_en 1:enable gps info packing;0:disable gps info packing.
     */
    status_t gpsInfoEn(int gps_en);

    /**
     * To send gps info after the start of recorder.
     * @param gps_info the pointer of buffer that store the gps info.gps_info
     */
    status_t gpsInfoSend(void *gps_info);

    
    /**
     * Prepares the recorder to begin capturing and encoding data. This method
     * must be called after setting up the desired audio and video sources,
     * encoders, file format, etc., but before start().
     *
     * @throws IllegalStateException if it is called after
     * start() or before setOutputFormat().
     * @throws IOException if prepare fails otherwise.
     *
     * MEDIA_RECORDER_DATASOURCE_CONFIGURED->MEDIA_RECORDER_PREPARED
     */ 
    status_t prepare();

    /**
     * Begins capturing and encoding data to the file specified with
     * setOutputFile(). Call this after prepare().
     *
     * <p>Since API level 13, if applications set a camera via
     * {@link #setCamera(Camera)}, the apps can use the camera after this method
     * call. The apps do not need to lock the camera again. However, if this
     * method fails, the apps should still lock the camera back. The apps should
     * not start another recording session during recording.
     *
     * @throws IllegalStateException if it is called before
     * prepare().
     *
     * MEDIA_RECORDER_PREPARED->MEDIA_RECORDER_RECORDING
     */
    status_t start();

    /**
     * Stops recording. Call this after start(). Once recording is stopped,
     * you will have to configure it again as if it has just been constructed.
     * Note that a RuntimeException is intentionally thrown to the
     * application, if no valid audio/video data has been received when stop()
     * is called. This happens if stop() is called immediately after
     * start(). The failure lets the application take action accordingly to
     * clean up the output file (delete the output file, for instance), since
     * the output file is not properly constructed when this happens.
     *
     * @throws IllegalStateException if it is called before start()
     *
     * MEDIA_RECORDER_RECORDING->MEDIA_RECORDER_IDLE
     */
    status_t stop(bool bShutDownNowFlag = false);

    /**
     * Restarts the MediaRecorder to its idle state. After calling
     * this method, you will have to configure it again as if it had just been
     * constructed.
     *
     * MEDIA_RECORDER_PREPARED
     * /MEDIA_RECORDER_RECORDING
     * /MEDIA_RECORDER_DATASOURCE_CONFIGURED
     * /MEDIA_RECORDER_INITIALIZED
     * /MEDIA_RECORDER_ERROR -> MEDIA_RECORDER_IDLE
     */
    status_t reset();
private:
    /**
     * MEDIA_RECORDER_IDLE -> MEDIA_RECORDER_INITIALIZED
     */
    status_t init();
    /**
     * MEDIA_RECORDER_INITIALIZED -> MEDIA_RECORDER_IDLE
     */
    status_t close();
    /**
      * MEDIA_RECORDER_PREPARED
      * MEDIA_RECORDER_RECORDING
      * MEDIA_RECORDER_DATASOURCE_CONFIGURED
      * MEDIA_RECORDER_ERROR
      * -> MEDIA_RECORDER_INITIALIZED
     */
    status_t doReset();
    status_t stop_l(bool bShutDownNowFlag = false);
    status_t reset_l();

public:
    /* Do not change this value without updating its counterpart
     * in include/media/mediarecorder.h or mediaplayer.h!
     */
    enum
    {
        /** Unspecified media recorder error.
         * @see android.media.MediaRecorder.OnErrorListener
         */
        MEDIA_RECORDER_ERROR_UNKNOWN = 1,
        /** Media server died. In this case, the application must release the
         * MediaRecorder object and instantiate a new one.
         * @see android.media.MediaRecorder.OnErrorListener
         */
        MEDIA_ERROR_SERVER_DIED = 100,
        MEDIA_ERROR_VENC_TIMEOUT = 101, //uint64_t* pts
        MEDIA_ERROR_WRITE_DISK_ERROR = 102, //extra = muxerId
        MEDIA_ERROR_VENC_BUFFER_FULL, 
    };

    /**
     * Interface definition for a callback to be invoked when an error
     * occurs while recording.
     */
    class OnErrorListener
    {
    public:
        OnErrorListener()
        {
        }
        virtual ~OnErrorListener()
        {
        }
        /**
         * Called when an error occurs while recording.
         *
         * @param mr the MediaRecorder that encountered the error
         * @param what    the type of error that has occurred:
         * <ul>
         * <li>{@link #MEDIA_RECORDER_ERROR_UNKNOWN}
         * <li>{@link #MEDIA_ERROR_SERVER_DIED}
         * </ul>
         * @param extra   an extra code, specific to the error type
         */
        virtual void onError(EyeseeRecorder *pMr, int what, int extra) = 0;
    };

    /**
     * Register a callback to be invoked when an error occurs while
     * recording.
     *
     * @param l the callback that will be run
     */
    void setOnErrorListener(OnErrorListener *pl);

    /* Do not change these values without updating their counterparts
     * in include/media/mediarecorder.h!
     */
    enum
    {
        /** Unspecified media recorder error.
         * @see android.media.MediaRecorder.OnInfoListener
         */
        MEDIA_RECORDER_INFO_UNKNOWN              = 1,
        /** A maximum duration had been setup and has now been reached.
         * @see android.media.MediaRecorder.OnInfoListener
         */
        MEDIA_RECORDER_INFO_MAX_DURATION_REACHED = 800,
        /** A maximum filesize had been setup and has now been reached.
         * @see android.media.MediaRecorder.OnInfoListener
         */
        MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED = 801,

        /** informational events for individual tracks, for testing purpose.
         * The track informational event usually contains two parts in the ext1
         * arg of the onInfo() callback: bit 31-28 contains the track id; and
         * the rest of the 28 bits contains the informational event defined here.
         * For example, ext1 = (1 << 28 | MEDIA_RECORDER_TRACK_INFO_TYPE) if the
         * track id is 1 for informational event MEDIA_RECORDER_TRACK_INFO_TYPE;
         * while ext1 = (0 << 28 | MEDIA_RECORDER_TRACK_INFO_TYPE) if the track
         * id is 0 for informational event MEDIA_RECORDER_TRACK_INFO_TYPE. The
         * application should extract the track id and the type of informational
         * event from ext1, accordingly.
         *
         * FIXME:
         * Please update the comment for onInfo also when these
         * events are unhidden so that application knows how to extract the track
         * id and the informational event type from onInfo callback.
         *
         * {@hide}
         */
        MEDIA_RECORDER_TRACK_INFO_LIST_START        = 1000,
        /** Signal the completion of the track for the recording session.
         * {@hide}
         */
        MEDIA_RECORDER_TRACK_INFO_COMPLETION_STATUS = 1000,
        /** Indicate the recording progress in time (ms) during recording.
         * {@hide}
         */
        MEDIA_RECORDER_TRACK_INFO_PROGRESS_IN_TIME  = 1001,
        /** Indicate the track type: 0 for Audio and 1 for Video.
         * {@hide}
         */
        MEDIA_RECORDER_TRACK_INFO_TYPE              = 1002,
        /** Provide the track duration information.
         * {@hide}
         */
        MEDIA_RECORDER_TRACK_INFO_DURATION_MS       = 1003,
        /** Provide the max chunk duration in time (ms) for the given track.
         * {@hide}
         */
        MEDIA_RECORDER_TRACK_INFO_MAX_CHUNK_DUR_MS  = 1004,
        /** Provide the total number of recordd frames.
         * {@hide}
         */
        MEDIA_RECORDER_TRACK_INFO_ENCODED_FRAMES    = 1005,
        /** Provide the max spacing between neighboring chunks for the given track.
         * {@hide}
         */
        MEDIA_RECORDER_TRACK_INTER_CHUNK_TIME_MS    = 1006,
        /** Provide the elapsed time measuring from the start of the recording
         * till the first output frame of the given track is received, excluding
         * any intentional start time offset of a recording session for the
         * purpose of eliminating the recording sound in the recorded file.
         * {@hide}
         */
        MEDIA_RECORDER_TRACK_INFO_INITIAL_DELAY_MS  = 1007,
        /** Provide the start time difference (delay) betweeen this track and
         * the start of the movie.
         * {@hide}
         */
        MEDIA_RECORDER_TRACK_INFO_START_OFFSET_MS   = 1008,
        /** Provide the total number of data (in kilo-bytes) encoded.
         * {@hide}
         */
        MEDIA_RECORDER_TRACK_INFO_DATA_KBYTES       = 1009,
        /**
         * {@hide}
         */
        MEDIA_RECORDER_TRACK_INFO_LIST_END          = 2000,

    	//MEDIA_RECORDER_INFO_VENDOR_START            = 3000,
    	MEDIA_RECORDER_INFO_NEED_SET_NEXT_FD           = 3001,
    	MEDIA_RECORDER_INFO_RECORD_FILE_DONE           = 3002,
    	//MEDIA_RECORDER_INFO_DISK_SPEED_TOO_SLOW        = 3003,
    	//MEDIA_RECORDER_INFO_WRITE_DISK_ERROR           = 3004,
    	MEDIA_RECORDER_INFO_VENC_BUFFER_USAGE              = 3005,    //vbs buffer usage percentage
    };

    /**
     * Register a callback to be invoked when an informational event occurs while
     * recording.
     *
     * @param listener the callback that will be run
     */
    class OnInfoListener
    {
    public:
        OnInfoListener()
        {
        }
        virtual ~OnInfoListener()
        {
        }
        /**
         * Called when an error occurs while recording.
         *
         * @param mr the MediaRecorder that encountered the error
         * @param what    the type of error that has occurred:
         * <ul>
         * <li>{@link #MEDIA_RECORDER_INFO_UNKNOWN}
         * <li>{@link #MEDIA_RECORDER_INFO_MAX_DURATION_REACHED}
         * <li>{@link #MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED}
         * </ul>
         * @param extra   an extra code, specific to the error type
         */
        virtual void onInfo(EyeseeRecorder *pMr, int what, int extra) = 0;
    };

    /**
     * Register a callback to be invoked when an informational event occurs while
     * recording.
     *
     * @param listener the callback that will be run
     */
    void setOnInfoListener(OnInfoListener *pListener);

    /* for recorder data callback */
    class OnDataListener
    {
    public:
        OnDataListener()
        {
        }
        virtual ~OnDataListener()
        {
        }
        virtual void onData(EyeseeRecorder *pMr, int what, int extra) = 0;
    };
    void setOnDataListener(OnDataListener *pListener);
    /**
     * @param type callback_out_data_type.
     */
    status_t setBsFrameRawDataType(callback_out_data_type type); /* effected only when OutputFormat is OUTPUT_FORMAT_RAW */
    status_t getEncDataHeader(VencHeaderData *pEncDataHeader);
    VEncBuffer* getOneBsFrame();
    void freeOneBsFrame(VEncBuffer *pEncData);

    void dataCallbackTimestamp(const VIDEO_FRAME_BUFFER_S *pCameraFrameInfo);
        
     /** 
     *      AW extend
     * Remove one output_format sink of the muxer. If the muxer of output_format is removed all sink,
     * equivalent to destroy this muxer. If this muxer is the last one, return fail and 
     * don't remove it.
     *
     * @return if success.
     * @param muxerId.
     */
    status_t removeOutputFormatAndOutputSink(int muxerId);
    status_t setOutputFileSync(int fd, int64_t fallocateLength=0, int muxerId=0);
    status_t setOutputFileSync(char* path, int64_t fallocateLength=0, int muxerId=0);
    
    status_t setSdcardState(bool bExist);

    //status_t setOSDRects(std::list<VEncOSDRectInfo> &rects);
    //static bool compareOSDRectPriority(const VEncOSDRectInfo& first, const VEncOSDRectInfo& second);

    RGN_HANDLE createRegion(const RGN_ATTR_S *pstRegion);
    status_t setRegionBitmap(RGN_HANDLE Handle, const BITMAP_S *pBitmap);
    status_t attachRegionToVenc(RGN_HANDLE Handle, const RGN_CHN_ATTR_S *pstChnAttr);
    status_t detachRegionFromVenc(RGN_HANDLE Handle);
    status_t setRegionDisplayAttrOfVenc(RGN_HANDLE Handle, const RGN_CHN_ATTR_S *pstChnAttr);
    status_t getRegionDisplayAttrOfVenc(RGN_HANDLE Handle, RGN_CHN_ATTR_S *pstChnAttr);
    status_t destroyRegion(RGN_HANDLE Handle);

    //status_t setImpactFileDuration(int bfTimeMs, int afTimeMs);
    //status_t setImpactOutputFile(int fd, int64_t fallocateLength, int muxerId=0);
    //status_t setImpactOutputFile(char* path, int64_t fallocateLength, int muxerId=0);
    status_t setMuxCacheDuration(int nCacheMs);
    status_t switchFileNormal(int fd, int64_t fallocateLength, int muxerId=0);
    status_t switchFileNormal(char* path, int64_t fallocateLength, int muxerId=0);
    status_t setSwitchFileDurationPolicy(int muxerId, const RecordFileDurationPolicy ePolicy);
    status_t getSwitchFileDurationPolicy(int muxerId, RecordFileDurationPolicy *pPolicy) const;
    status_t enableDynamicBitRateControl(bool bEnable);
    status_t enableAttachAACHeader(bool enable); /* attach header for every frame, effect only when audio codec is AAC */
    status_t GetBufferState(BufferState &state);
    void notify(MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData); //process internal component callback.
private:
    void                    doCleanUp();
    static void postEventFromNative(EyeseeRecorder *pRecorder, int what, int arg1, int arg2, const std::shared_ptr<CMediaMemory>* pDataPtr = NULL);
    status_t config_AIO_ATTR_S();
    status_t config_AENC_CHN_ATTR_S();
    status_t config_TENC_CHN_ATTR_S();
    status_t config_VENC_CHN_ATTR_S();
    status_t config_MUX_GRP_ATTR_S();
    status_t pushOneBsFrame(CDXRecorderBsInfo *frame);
};

};

#endif  /* __IPCLINUX_EYESEERECORDER_H__ */


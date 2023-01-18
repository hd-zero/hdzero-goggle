/*
 * MediaStream.h
 *
 *  Created on: 2016年8月31日
 *      Author: liu
 */

#ifndef IPCPROGRAM_INTERFACE_MEDIASTREAM_H_
#define IPCPROGRAM_INTERFACE_MEDIASTREAM_H_

#include <stdint.h>
#include <time.h>
#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <condition_variable>

//#define SAVE_FILE
#define FILE_SUFFIX ".h264"

class TinyServer;
class Groupsock;
class H264or5VideoNaluFramer;
class UnicastVideoMediaSubsession;
class RTPSink;
class TinySource;
class RTCPInstance;

namespace Byl {
class FrameNaluParser;
}

class MediaStream {
  public:
    struct MediaStreamAttr {
        enum StreamType { STREAM_TYPE_UNICAST, STREAM_TYPE_MULTICAST };

        enum VideoType { VIDEO_TYPE_H265, VIDEO_TYPE_H264 };

        enum AudioType {
            AUDIO_TYPE_AAC,
        };

        VideoType videoType;
        AudioType audioType;
        StreamType streamType;
    };

    enum FrameDataType {
        FRAME_DATA_TYPE_I,
        FRAME_DATA_TYPE_B,
        FRAME_DATA_TYPE_P,
        FRAME_DATA_TYPE_HEADER,
        FRAME_DATA_TYPE_UNKNOW
    };

    MediaStream(std::string const &name, TinyServer &server, MediaStreamAttr attr);
    ~MediaStream();

    std::string streamURL();

    void appendVideoData(unsigned char *data, unsigned int size);
    void appendVideoData(unsigned char *data, unsigned int size, FrameDataType dataType);
    void appendVideoData(unsigned char *data, unsigned int size, uint64_t pts, FrameDataType dataType);
    void appendAudioData(unsigned char *data, unsigned int size, uint64_t pts);

    void setVideoFrameRate(int frameRate);

    typedef void(OnNewClientConnectFunc)(void *context);
    void setNewClientCallback(OnNewClientConnectFunc *function, void *context);

  private:
    TinySource *createVideoSource();
    TinySource *createAudioSource();
    RTPSink *createVideoSink();
    RTPSink *createAudioSink();
    void unicastDestructor();
    void multicastDestructor();

    static void unicastConstructor(void *context);
    static void multicastConstructor(void *context);
    static void disconnectAllClient(void *context);
    static void afterPlaying(MediaStream *stream);
    void play();
    void saveHeaderInfo(uint8_t *data, uint32_t size, uint64_t pts);
    void sendDataToQueue(uint8_t *data, uint32_t size, uint64_t pts);
    void checkDurationTime();

    TinyServer &_server;
    std::string _streamName;
    const MediaStreamAttr _mediaStreamAttr;
    struct timeval *_lastGetFrameTime;
    int _frameRate;

    Groupsock *_rtpGroupsock;
    Groupsock *_rtcpGroupsock;

    TinySource *_videoSource;
    TinySource *_audioSource;
    UnicastVideoMediaSubsession *_videoSubsession;
    H264or5VideoNaluFramer *_naluFramer;
    RTPSink *_videoSink;
    RTCPInstance *_rtcp;
    Byl::FrameNaluParser *_nal_parser;

    uint32_t _disconn_token;
    uint32_t _construct_token;
    std::mutex _task_mutex;
    std::condition_variable _task_complete;

    std::vector<uint8_t> _headerInfo;

    OnNewClientConnectFunc *_newRecvCallback;

#ifdef SAVE_FILE
    std::ofstream *_of;
#endif
};
#endif /* IPCPROGRAM_INTERFACE_MEDIASTREAM_H_ */

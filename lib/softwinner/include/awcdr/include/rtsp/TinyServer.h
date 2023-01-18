/*
 * TinyServer.h
 *
 *  Created on: 2016年6月29日
 *      Author: A
 */

#ifndef LIVE_IPCPROGRAM_TINYSERVER_H_
#define LIVE_IPCPROGRAM_TINYSERVER_H_

#include <pthread.h>
#include <string>
#include "MediaStream.h"

class TinySource;
class RTSPServer;
class Groupsock;
class H264VideoStreamDiscreteFramer;
class TaskScheduler;
class UsageEnvironment;
class RTPSink;
class RTCPInstance;
//forward

class TinyServer {
  public:
    static TinyServer* createServer();
    static TinyServer* createServer(const std::string& ip, int port);
    virtual ~TinyServer();

    MediaStream* createMediaStream(std::string const& name, MediaStream::MediaStreamAttr attr);
    RTSPServer* getRTSPServer();
    int runWithNewThread();
    void run();
    void stop();

  protected:
    TinyServer(const std::string ip, int port);

  private:
    void postConstructor();
    static void* loopThreadFunc(void* server);

    TaskScheduler* _scheduler;
    UsageEnvironment* _env;
    RTSPServer* _rtspServer;
    std::string _serverIp;
    int _port;

    char _runFlag;
    bool _loopRunInThread;
    pthread_t _loopThreadId;
};

#endif /* LIVE_IPCPROGRAM_TINYSERVER_H_ */

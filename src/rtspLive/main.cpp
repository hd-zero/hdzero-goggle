#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>

#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include "DynamicRTSPServer.hh"
#include "version.hh"
#include <GroupsockHelper.hh> // for "weHaveAnIPv*Address()"
#include "H264FramedLiveSource.hh"

#if(!USE_DynamicRTSPServer)
#include "H264LiveVideoServerMediaSubssion.hh"
#include "H265LiveVideoServerMediaSubssion.hh"
#endif

extern "C"
{
#include "gogglemsg.h"
#include "live_context.h"
#include "version.h"
#include "confparser.h"
#include "avshare.h"
}

void show_version(int argc, char* argv[])
{
    if( (argc > 1)  && (strcmp(argv[1], "-v") == 0) ) {
        alogd("version: %s(%s, %s)\n", VERSION_FULL, __DATE__, __TIME__);
        exit(0);
    }
}

uint32_t get_tickCount(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

/******************************************************************************
* funciton : signal handler
******************************************************************************/
void signal_pipe( int sig )
{
	aloge( "signal sigpipe\n" );

	return ;
}

void signal_segv(int sig)
{
	aloge( "signal sigsegv\n" );

	abort();
}

void dump_bytes(uint8_t* data, int len)
{
#define BYTES_perLINE 16
    int nLines = (len / BYTES_perLINE) + ((len & ~BYTES_perLINE) > 0);
    int i;

    alogd("addr=%p len=%d lines=%d\n", data, len, nLines);
    for(i=0; i<nLines; i++) {
        if( len >= BYTES_perLINE ) {
            alogd("%04x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                  i*BYTES_perLINE,
                  data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
                  data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]
                  );
            data += BYTES_perLINE;
            len -= BYTES_perLINE;
        }
        else {
            uint8_t dataLine[BYTES_perLINE];
            memset(dataLine, 0xFF, BYTES_perLINE);
            memcpy(dataLine, data, len);
            data = dataLine;
            alogd("%04x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                  i*BYTES_perLINE,
                  data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
                  data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]
                  );
            break;
        }
    }
}
/******************************************************************************
* funciton : lock file
******************************************************************************/
static int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return(fcntl(fd, F_SETLK, &fl));
}

/******************************************************************************
* funciton : check if application is already running
******************************************************************************/
static int already_running(void)
{
    int     fd;
    char    buf[16];

    fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
    if (fd < 0) {
        aloge( "can't open %s: %s\n", LOCKFILE, strerror(errno));
        exit(1);
    }
    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {

            close(fd);
            return(1);
        }
        aloge( "can't lock %s: %s\n", LOCKFILE, strerror(errno));
        exit(1);
    }
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf)+1);
    return(0);
}

/***********************************************************
 * check the file exist
***********************************************************/
bool disk_checkFile(const char* sPath)
{
	struct stat st;

	if(sPath == NULL) {
        return false;
	}

	stat(sPath, &st);
	return S_ISREG(st.st_mode);
}

void live_checkConf(LiveContext_t* liveCtx, char* confSet)
{
	if( disk_checkFile(confSet) )  {
        strcpy(liveCtx->confFile, confSet);
        return;
	}

    /* path where default confs is */
	if( disk_checkFile(REC_confDEFAULT) ) {
        strcpy(liveCtx->confFile, REC_confDEFAULT);
        return;
	}

    char  sTemp[MAX_pathLEN];
    char* p = sTemp;
    readlink("/proc/self/exe", sTemp, MAX_pathLEN);
    if (NULL != (p = strrchr(sTemp,'/'))){
        /* path where confs is */
        *p = '\0';
        strcat(sTemp, REC_confPathFILE);
        if( disk_checkFile(sTemp) ) {
            strcpy(liveCtx->confFile, sTemp);
            return;
        }

        /* path where record is */
        memset(sTemp, 0, MAX_pathLEN);
        readlink("/proc/self/exe", sTemp, MAX_pathLEN);
        p = strrchr(sTemp,'/');
        *p = '\0';
        sprintf(liveCtx->confFile, "%s/%s", sTemp, REC_confFILE);
    }
}

static void announceStream(RTSPServer* rtspServer, char const* streamName)//显示RTSP连接信息
{
  UsageEnvironment& env = rtspServer->envir();

  env << "LIVE555 Media Server\n";
  env << "\tversion " << MEDIA_SERVER_VERSION_STRING
       << " (LIVE555 Streaming Media library version "
       << LIVEMEDIA_LIBRARY_VERSION_STRING << ").\n";

  env << "Play streams from this server using the URL\n";
  if (weHaveAnIPv4Address(env)) {
    char* rtspURLPrefix = rtspServer->ipv4rtspURLPrefix();
    env << "\t" << rtspURLPrefix << "<streamname>\n";
    delete[] rtspURLPrefix;
    if (weHaveAnIPv6Address(env)) env << "or\n";
  }
  if (weHaveAnIPv6Address(env)) {
    char* rtspURLPrefix = rtspServer->ipv6rtspURLPrefix();
    env << "\t" << rtspURLPrefix << "<streamname>\n";
    delete[] rtspURLPrefix;
  }
  env << "\t\"" << streamName << "\" => a live stream\n";
  env << "See http://www.live555.com/mediaServer/ for additional documentation.\n";

  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP
  // port numbers (8000 and 8080).

  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
    env << "(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling).)\n";
  } else {
    env << "(RTSP-over-HTTP tunneling is not available.)\n";
  }
}

void announceURL(RTSPServer* rtspServer, ServerMediaSession* sms)
{
  if (rtspServer == NULL || sms == NULL) return; // sanity check

  UsageEnvironment& env = rtspServer->envir();

  env << LOG_PRIFIX << "Play this stream using the URL ";
  if (weHaveAnIPv4Address(env)) {
    char* url = rtspServer->ipv4rtspURL(sms);
    env << "\"" << url << "\"";
    delete[] url;
    if (weHaveAnIPv6Address(env)) env << " or ";
  }
  if (weHaveAnIPv6Address(env)) {
    char* url = rtspServer->ipv6rtspURL(sms);
    env << "\"" << url << "\"";
    delete[] url;
  }
  env << "\n";
}

void* live_multicast_loop(void *context)
{
  LiveContext_t* liveCtx = (LiveContext_t*)context;
  const char* streamName = liveCtx->streamName;
  portNumBits rtspServerPortNum = liveCtx->port;//8554;

  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

  // Create 'groupsocks' for RTP and RTCP:
  //char const* destinationAddressStr = "232.0.56.80";
  struct sockaddr_storage destinationAddress;
  destinationAddress.ss_family = AF_INET;
  ((struct sockaddr_in&)destinationAddress).sin_addr.s_addr = chooseRandomIPv4SSMAddress(*env); //inet_addr(destinationAddressStr);
  // Note: This is a multicast address.  If you wish instead to stream
  // using unicast, then you should use the "testOnDemandRTSPServer"
  // test program - not this test program - as a model.

  const unsigned short rtpPortNum = 18888;
  const unsigned short rtcpPortNum = rtpPortNum+1;
  const unsigned char ttl = 255;

  const Port rtpPort(rtpPortNum);
  const Port rtcpPort(rtcpPortNum);

  Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
  rtpGroupsock.multicastSendOnly(); // we're a SSM source
  Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
  rtcpGroupsock.multicastSendOnly(); // we're a SSM source

  // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
  RTPSink* videoSink;
  OutPacketBuffer::maxSize = 7208076;//100000;

  if( liveCtx->h265 ) {
    videoSink = H265VideoRTPSink::createNew(*env, &rtpGroupsock, 96);
  }
  else {
    videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);
  }

  // Create (and start) a 'RTCP instance' for this RTP sink:
  const unsigned estimatedSessionBandwidth = 500; // in kbps; for RTCP b/w share
  const unsigned maxCNAMElen = 100;
  unsigned char CNAME[maxCNAMElen+1];
  gethostname((char*)CNAME, maxCNAMElen);
  CNAME[maxCNAMElen] = '\0'; // just in case
  RTCPInstance* rtcp
  = RTCPInstance::createNew(*env, &rtcpGroupsock,
			    estimatedSessionBandwidth, CNAME,
			    videoSink, NULL /* we're a server */,
			    liveCtx->isSSM /* we're a SSM source */);
  // Note: This starts RTCP running automatically

  RTSPServer* rtspServer = RTSPServer::createNew(*env, rtspServerPortNum);
  if (rtspServer == NULL) {
    *env << LOG_PRIFIX << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }

  const char* descriptionString= "Session streamed by \"HDZERO\"";
  ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName, streamName, descriptionString, liveCtx->isSSM);
  sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
  rtspServer->addServerMediaSession(sms);
  announceURL(rtspServer, sms);
  liveCtx->server = rtspServer;

  // Start the streaming:
  *env << LOG_PRIFIX << "Beginning streaming...\n";

  FramedSource* videoES = H264FramedLiveSource::createNew(*env);
  H264or5VideoStreamFramer* videoSource;

  if( liveCtx->h265 ) {
    // Create a framer for the Video Elementary Stream:
    videoSource = H265VideoStreamFramer::createNew(*env, videoES);
  }
  else {
    // Create a framer for the Video Elementary Stream:
    videoSource = H264VideoStreamFramer::createNew(*env, videoES);
  }
  videoSink->startPlaying(*videoSource, NULL, NULL);

  env->taskScheduler().doEventLoop((char*)&liveCtx->bExit); // does not return

  return 0; // only to prevent compiler warning
}

void* live_loop(void *context)
{
	OutPacketBuffer::maxSize = 7208076;

	UsageEnvironment* env;
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	UserAuthenticationDatabase* authDB = NULL;
	LiveContext_t* liveCtx = (LiveContext_t*)context;
    const char* streamName = liveCtx->streamName;
    RTSPServer* rtspServer;
    portNumBits rtspServerPortNum = liveCtx->port;//8554;

    if( liveCtx->runDynamicRTSPServer ) {
        rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB, liveCtx->h265, liveCtx->streamName);
    }
    else {
        rtspServer = RTSPServer::createNew(*env, rtspServerPortNum, authDB);
        if( rtspServer != NULL ) {
            Boolean reuseFirstSource = True;
            const char* descriptionString= "Session streamed by \"HDZERO\"";
            ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName, streamName, descriptionString);

            if (liveCtx->h265) {
                sms->addSubsession(H265LiveVideoServerMediaSubssion::createNew(*env, reuseFirstSource));
            }
            else {
                sms->addSubsession(H264LiveVideoServerMediaSubssion::createNew(*env, reuseFirstSource));
            }
            rtspServer->addServerMediaSession(sms);
        }
    }

    if (rtspServer == NULL) {
        *env << LOG_PRIFIX << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
        exit(1);
    }

    announceStream(rtspServer, streamName);

    liveCtx->server = rtspServer;
    env->taskScheduler().doEventLoop((char*)&liveCtx->bExit);

	return 0;
}

void live_start(LiveContext_t* liveCtx)
{
    liveCtx->bExit = false;
    if( liveCtx->runThread ) {
        alogd("start thread\n");
        pthread_create(&liveCtx->threadId, NULL, liveCtx->IsMulticast?live_multicast_loop:live_loop, liveCtx);
    }
    else if( liveCtx->IsMulticast ) {
        live_multicast_loop(liveCtx);
    }
    else {
        live_loop(liveCtx);
    }
}

void live_stop(LiveContext_t* liveCtx)
{
    alogd("stop\n");

    //disconnect all
    RTSPServer* rtspServer = (RTSPServer*)liveCtx->server;

    if( rtspServer != NULL ) {
        rtspServer->closeAllClientSessionsForServerMediaSession(liveCtx->streamName);
        rtspServer->removeServerMediaSession(liveCtx->streamName);
        alogd("disconnected all\n");
    }

    liveCtx->bExit = true;
    if( liveCtx->threadId > 0 ) {
        pthread_join(liveCtx->threadId, NULL);
        liveCtx->threadId = 0;
    }

    liveCtx->server = NULL;
}

void live_getParams(LiveContext_t* liveCtx, int argc, char* argv[])
{
    liveCtx->h265 = false;
    liveCtx->runLive = true;
    liveCtx->runThread = true;
    liveCtx->runDynamicRTSPServer = false;

    //for multicast
    liveCtx->IsMulticast = true;
    liveCtx->isSSM = true;

    live_checkConf(liveCtx, NULL);
    alogd("conf: %s\n", liveCtx->confFile);

    conf_loadLiveParams(liveCtx);

    if( argc > 1 ) {
        if( strcmp(argv[1], "-nt") == 0 ) {
            liveCtx->runThread = false;
        }
        else if ( strcmp(argv[1], "-nl") == 0 ) {
            liveCtx->runLive = false;
        }
        else if ( strcmp(argv[1], "-ds") == 0 ) {
            liveCtx->runDynamicRTSPServer = true;
        }
    }
}

void main_loop(LiveContext_t* liveCtx)
{
    bool isExit = false;
    uint32_t tkIdle = get_tickCount();
    uint32_t tkNow = 0;

    alogd("wait messages\n");
    gogglemsg_init(false, MTYPE_LIVE);
    usleep(100);
    gogglemsg_flush(MTYPE_LIVE);

	while( !isExit ) {
	    int nMsg = gogglemsg_recv(MTYPE_LIVE);

		switch( nMsg ) {
		case MSG_cmdQUIT:
		    isExit = true;
			break;
		case MSG_cmdSTART:
		    avshare_start();
		    tkIdle = tkNow;
		    break;
		case MSG_cmdSTOP:
		    avshare_stop();
		    tkIdle = tkNow;
		    break;
		default:
            usleep(100*1000);
			break;
		}

		tkNow = get_tickCount();
		if( tkNow - tkIdle >= 500 ) {
            tkIdle = tkNow;
        }
	}

    gogglemsg_uninit();
}

int main(int argc, char *argv[])
{
    show_version(argc, argv);

    alogd("rtspLive start ...\n");

	signal(SIGPIPE, signal_pipe);
	signal(SIGSEGV, signal_segv);

	if( already_running() ) {
		aloge( "already runing... exit!\n");
		exit(1);
	}

    LiveContext_t liveCtx;
    ZeroMemory(&liveCtx, sizeof(liveCtx));
    live_getParams(&liveCtx, argc, argv);
    pthread_mutex_init(&liveCtx.mutex, NULL);

    live_start(&liveCtx);
    main_loop(&liveCtx);

failed:
    alogd("exit ...\n");

    live_stop(&liveCtx);
    avshare_stop();
    pthread_mutex_destroy(&liveCtx.mutex);

    alogd("exit done\n");

    return 0;
}

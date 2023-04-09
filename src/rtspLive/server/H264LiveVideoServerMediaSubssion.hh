#ifndef _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#define _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#include "H264VideoFileServerMediaSubsession.hh"

class H264LiveVideoServerMediaSubssion : public H264VideoFileServerMediaSubsession {

public:
	static H264LiveVideoServerMediaSubssion* createNew(UsageEnvironment& env, Boolean reuseFirstSource);

protected:
	H264LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource);
	~H264LiveVideoServerMediaSubssion();

protected:
	FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
};

#endif

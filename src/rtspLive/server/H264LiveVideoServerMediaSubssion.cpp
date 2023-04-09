#include "H264LiveVideoServerMediaSubssion.hh"
#include "H264FramedLiveSource.hh"
#include "H264VideoStreamFramer.hh"

H264LiveVideoServerMediaSubssion* H264LiveVideoServerMediaSubssion::createNew(UsageEnvironment& env, Boolean reuseFirstSource)
{
    return new H264LiveVideoServerMediaSubssion(env, reuseFirstSource);
}

H264LiveVideoServerMediaSubssion::H264LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource)
: H264VideoFileServerMediaSubsession(env, NULL, reuseFirstSource)
{
    printf("[MEDIA SERVER]H264 server media subssion construct\n");
}

H264LiveVideoServerMediaSubssion::~H264LiveVideoServerMediaSubssion()
{
    printf("[MEDIA SERVER]H264 server media subssion destruct\n");
}

FramedSource* H264LiveVideoServerMediaSubssion::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)
{
	estBitrate = 4000; //kbps, estimate

	H264FramedLiveSource* liveSource = H264FramedLiveSource::createNew(envir());
	if (liveSource == NULL)
	{
		return NULL;
	}

	// Create a framer for the Video Elementary Stream:
	return H264VideoStreamFramer::createNew(envir(), liveSource);
}

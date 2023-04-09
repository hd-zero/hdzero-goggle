#include "H265LiveVideoServerMediaSubssion.hh"
#include "H264FramedLiveSource.hh"
#include "H265VideoStreamFramer.hh"

H265LiveVideoServerMediaSubssion* H265LiveVideoServerMediaSubssion::createNew(UsageEnvironment& env, Boolean reuseFirstSource)
{
	return new H265LiveVideoServerMediaSubssion(env, reuseFirstSource);
}

H265LiveVideoServerMediaSubssion::H265LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource)
: H265VideoFileServerMediaSubsession(env, NULL, reuseFirstSource)
{
    printf("[MEDIA SERVER]H265 server media subssion construct\n");
}

H265LiveVideoServerMediaSubssion::~H265LiveVideoServerMediaSubssion()
{
    printf("[MEDIA SERVER]H265 server media subssion destruct\n");
}

FramedSource* H265LiveVideoServerMediaSubssion::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)
{
	estBitrate = 4000; //kbps, estimate

	H264FramedLiveSource* liveSource = H264FramedLiveSource::createNew(envir());
	if (liveSource == NULL)
	{
		return NULL;
	}

	// Create a framer for the Video Elementary Stream:
	return H265VideoStreamFramer::createNew(envir(), liveSource);
}

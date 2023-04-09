#ifndef _H264FRAMEDLIVESOURCE_HH
#define _H264FRAMEDLIVESOURCE_HH

#include <FramedSource.hh>

#define H264FramedLiveSource_SAVE_FILE  0

class H264FramedLiveSource : public FramedSource
{
public:
	static H264FramedLiveSource* createNew(UsageEnvironment& env);

protected:
	H264FramedLiveSource(UsageEnvironment& env);
	~H264FramedLiveSource();

protected:
	virtual void doGetNextFrame();

#if(H264FramedLiveSource_SAVE_FILE)
	FILE *_fileTest;
#endif
};

#endif

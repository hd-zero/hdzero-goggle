#pragma once

#include "H265VideoFileServerMediaSubsession.hh"

class H265LiveVideoServerMediaSubssion : public H265VideoFileServerMediaSubsession {

public:
	static H265LiveVideoServerMediaSubssion* createNew(UsageEnvironment& env, Boolean reuseFirstSource);

protected:
	H265LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource);
	~H265LiveVideoServerMediaSubssion();

protected:
	FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
};

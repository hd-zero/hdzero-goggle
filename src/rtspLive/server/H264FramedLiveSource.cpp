#include "H264FramedLiveSource.hh"

extern "C"
{
#include "avshare.h"
}

H264FramedLiveSource* H264FramedLiveSource::createNew(UsageEnvironment& env)
{
	H264FramedLiveSource* newSource = new H264FramedLiveSource(env);
	return newSource;
}

H264FramedLiveSource::H264FramedLiveSource(UsageEnvironment& env)
: FramedSource(env)
{
	printf("[MEDIA SERVER]live source opened\n");

#if(H264FramedLiveSource_SAVE_FILE)
	_fileTest = fopen("/mnt/extsd/test_live.264", "wb");
#endif

    avshare_start();
}

H264FramedLiveSource::~H264FramedLiveSource()
{
    printf("[MEDIA SERVER]live source closed\n");

    avshare_stop();

#if(H264FramedLiveSource_SAVE_FILE)
    if( _fileTest != NULL ) {
        fclose(_fileTest);
    }
#endif
}

void H264FramedLiveSource::doGetNextFrame()
{
    int nLen = avshare_readFrame(fTo, fMaxSize, &fPresentationTime, &fDurationInMicroseconds);

    //printf("[MEDIA SERVER]GetFrameData len = [%d],fMaxSize = [%d]\n",fFrameSize,fMaxSize);
    fFrameSize = (nLen>0) ? nLen : 0;

    if (fFrameSize > fMaxSize) {
        //fNumTruncatedBytes = fFrameSize - fMaxSize;
        fFrameSize = fMaxSize;
        printf("[MEDIA SERVER]Live source frame is overfload [%d/%d]\n",nLen,fMaxSize);
    }
    else {
        //fNumTruncatedBytes = 0;
        fDurationInMicroseconds = 0;
    }

#if(H264FramedLiveSource_SAVE_FILE)
    if( (fFrameSize > 0) && (_fileTest != NULL) ) {
        fwrite(fTo, fFrameSize, 1, _fileTest);
    }
#endif

    if(!fFrameSize){
		nextTask() = envir().taskScheduler().scheduleDelayedTask(1000,(TaskFunc*)FramedSource::afterGetting, this);
    }
    else{
		nextTask() = envir().taskScheduler().scheduleDelayedTask(0,(TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数
    }
}

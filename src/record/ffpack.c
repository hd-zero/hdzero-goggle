//#define LOG_NDEBUG 0
#define LOG_TAG "ffpack"
#include <log/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ffpack.h"

#define FFPack_bufSIZE  65535
#define FFPack(p)       ((FFPack_t*)(p))

void ff_printerr(char* sPrefix, int err)
{
	char strError[256] = {0};
	av_strerror(err,strError,256);
	if(sPrefix==NULL)
    {
        LOGD("ff-failed: %s", strError);
    }
    else
    {
        LOGD(sPrefix);
        LOGD("%d: %s", err, strError);
    }
}

int write_packet(void *opaque, uint8_t *buf, int buf_size)
{
    FFPack_t* ff = FFPack(opaque);
    ff->cbOnData(ff, 0, buf, buf_size, ff->cbContext);

	return 0;
}

FFPack_t* ffpack_open(CB_onData cbOnData, void* context)
{
    FFPack_t* ff = (FFPack_t*)malloc(sizeof(FFPack_t));
    if(ff == NULL)
    {
        return NULL;
    }

    memset(ff, 0, sizeof(FFPack_t));

    int nRet = avformat_alloc_output_context2(&ff->ofmtContext, NULL, "mpegts", NULL);
    //int nRet = avformat_alloc_output_context2(&ff->ofmtContext, NULL, "mp4", NULL);
    if(nRet < 0)
	{
		ff_printerr("failed: ", nRet);
		free(ff);
		return NULL;
	}

	uint8_t* buf = (uint8_t*)av_mallocz(sizeof(uint8_t)* FFPack_bufSIZE);
	AVIOContext* pb = avio_alloc_context(buf, FFPack_bufSIZE, 1, ff, NULL, write_packet, NULL);//注意：第3个参数赋值为1，否则write_packet回调将不能被成功调用

	ff->ofmtContext->pb = pb;//这个是关键，指定ps输出的方式
	ff->ofmtContext->flags |= AVFMT_FLAG_CUSTOM_IO;
	ff->ofmtContext->flags |= AVFMT_FLAG_FLUSH_PACKETS;
	ff->ofmtContext->flags |= AVFMT_NOFILE; //不生成文件

	ff->cbOnData = cbOnData;
	ff->cbContext= context;

	LOGD("format %s[%s]\n", ff->ofmtContext->oformat->name, ff->ofmtContext->oformat->long_name);

    return ff;
}

FFPack_t* ffpack_openFile(char* sName, void* context)
{
    FFPack_t* ff = (FFPack_t*)malloc(sizeof(FFPack_t));
    if(ff == NULL)
    {
        return NULL;
    }

    memset(ff, 0, sizeof(FFPack_t));

    int nRet = avformat_alloc_output_context2(&ff->ofmtContext, NULL, NULL, sName);
    //int nRet = avformat_alloc_output_context2(&ff->ofmtContext, NULL, "mp4", NULL);
    //int nRet = avformat_alloc_output_context2(&ff->ofmtContext, av_guess_format("mp4", NULL, "video/mp4"), NULL, NULL);
	if(nRet < 0)
	{
		ff_printerr("failed: ", nRet);
		free(ff);
		return NULL;
	}

    if(!(ff->ofmtContext->flags & AVFMT_NOFILE))
    {
        nRet = avio_open(&ff->ofmtContext->pb, sName, AVIO_FLAG_WRITE);
        if(nRet < 0)
        {
            ff_printerr( "pb failed: ", nRet);

            // pb open will failed if sdcard is read-only, treat it as fatal error
            // sdcard will be mounted as read-only sometimes by the following operations:
            // 1. shutdown the power while recording.
            // 2. power on the goggle.
            // once this happened, unmount sdcard will failed: Resource is busy.
            // unplug then plug sdcard, to restore it writable.
            avformat_free_context(ff->ofmtContext);
            free(ff);
            return NULL;
        }
    }

	ff->cbContext= context;

	LOGD("format %s[%s]\n", ff->ofmtContext->oformat->name, ff->ofmtContext->oformat->long_name);

    return ff;
}

void ffpack_close(FFPack_t* ff)
{
    LOGD("ff=%p\n", ff);

    //flush all data
    av_write_frame(ff->ofmtContext, NULL);

    //LOGD("ofmt=%p\n", ff->ofmtContext);

    //Write file trailer
    av_write_trailer(ff->ofmtContext);

    if( ff->ofmtContext != NULL )
    {
        if(!(ff->ofmtContext->flags & AVFMT_NOFILE))
        {
            //LOGD("close pb=%p\n", ff->ofmtContext->pb);
            avio_close(ff->ofmtContext->pb);
        }
        else {
            //LOGD("free pb=%p\n", ff->ofmtContext->pb);
            avio_context_free(&ff->ofmtContext->pb);
        }

#if(0)
        /* would cause an exception */
        int i;
        AVCodecParameters* cp;
        for( i=0; i<ff->ofmtContext->nb_streams; i++ ) {
            cp = ff->ofmtContext->streams[i]->codecpar;
            if( (cp->extradata_size > 0) && (cp->extradata != NULL) ) {
                av_free(cp->extradata);
            }
        }
#endif

        //LOGD("free ofmt\n");
        avformat_free_context(ff->ofmtContext);
    }

    free(ff);

    LOGD("done\n");
}

int ffpack_newProgram(FFPack_t* ff, char* sName)
{
    AVFormatContext* fmtContext = ff->ofmtContext;
    AVProgram *program = av_new_program(fmtContext, fmtContext->nb_programs + 1);

    if (program) {
        av_dict_set(&program->metadata, "service_name", sName, 0);
        //av_dict_set(&program->metadata, "title", "testTitle1", 0);
        av_dict_set(&program->metadata, "service_provider", "avsoon", 0);
        //av_dict_set(&program->metadata, "language", "English", 0);

        return program->id;
    }

    return 0;
}

int ffpack_newVideoStream(FFPack_t* ff, uint16_t programId, FFStreamParameters_t* param)
{
    if(param->mediaType != AVMEDIA_TYPE_VIDEO)
    {
        LOGD("unsupported type: %d", param->mediaType);
        return -1;
    }

    AVStream *stream = avformat_new_stream(ff->ofmtContext, NULL);
    if (!stream)
    {
        LOGD("failed!\n");
        return -1;
    }

    AVCodecParameters* cp = stream->codecpar;
    stream->r_frame_rate.num = 0;//param->video.fps;
    stream->r_frame_rate.den = 0;//1;
    stream->avg_frame_rate.num = param->video.fps;
    stream->avg_frame_rate.den = 1;
    stream->time_base.num = 1;//0;
    stream->time_base.den = param->video.fps;//100000;//1; //10us
    cp->codec_type = param->mediaType;
    cp->codec_id = param->codecId;
    cp->width = param->video.width;
    cp->height= param->video.height;

    //mp4 needed
    cp->codec_tag = 0;
    cp->format = 12;
    //cp->profile = 100;
    //cp->level = 51;

    if( param->codecId == AV_CODEC_ID_HEVC ) {
        //compatible with macos's quicktime
        cp->codec_tag = MKTAG('h', 'v', 'c', '1');
    }

    if( param->spsData != NULL ) {
        cp->extradata_size = param->spsLen;
        cp->extradata = av_mallocz(cp->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
        if (!cp->extradata)
        {
            LOGE("outof memory");
            return -1;
        }
        memcpy(cp->extradata, param->spsData, cp->extradata_size);
    }

    if(programId >=0)
    {
        av_program_add_stream_index(ff->ofmtContext, programId, stream->index);
    }

    return stream->index;
}

int  ffpack_newAudioStream(FFPack_t* ff, uint16_t programId, FFStreamParameters_t* param)
{
    if(param->mediaType != AVMEDIA_TYPE_AUDIO)
    {
        LOGD("unsupported type: %d", param->mediaType);
        return -1;
    }

    AVStream *stream = avformat_new_stream(ff->ofmtContext, NULL);
    if (!stream)
    {
        LOGD("failed");
        return -1;
    }

    AVCodecParameters* cp = stream->codecpar;
    stream->time_base.num = 1;
    stream->time_base.den = param->audio.sample_rate;
    cp->codec_type = param->mediaType;
    cp->codec_id = param->codecId;
    cp->channels = param->audio.channels;
    cp->sample_rate = param->audio.sample_rate;
    cp->codec_tag = 0;

    if(programId >=0)
    {
        av_program_add_stream_index(ff->ofmtContext, programId, stream->index);
    }

    return stream->index;
}

int  ffpack_newDataStream (FFPack_t* ff, uint16_t programId, FFStreamParameters_t* param)
{
    if(param->mediaType != AVMEDIA_TYPE_DATA)
    {
        LOGD("unsupported type: %d", param->mediaType);
        return -1;
    }

    AVStream *stream = avformat_new_stream(ff->ofmtContext, NULL);
    if (!stream)
    {
        LOGD("failed");
        return -1;
    }

    AVCodecParameters* cp = stream->codecpar;
    stream->time_base.num = 0;
    stream->time_base.den = 1;
    cp->codec_type = param->mediaType;
    cp->codec_id = param->codecId;

    av_program_add_stream_index(ff->ofmtContext, programId, stream->index);

    return stream->index;
}

bool ffpack_isVideoStream(AVStream* st)
{
    return st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO;
}

int ffpack_getFps(FFPack_t* ff)
{
    int i;

    for(i=0; i<ff->ofmtContext->nb_streams; i++)
    {
        if(ffpack_isVideoStream(ff->ofmtContext->streams[i]))
        {
            return ff->ofmtContext->streams[i]->r_frame_rate.num;
        }
    }

    return 0;
}

void ffpack_dumpTimebase(FFPack_t* ff)
{
    int i = 0;
    AVStream* st;

    for( i=0; i<ff->ofmtContext->nb_streams; i++ )
    {
        st = ff->ofmtContext->streams[i];
        LOGD("stream %d: time_base(num,den) = %d,%d", i, st->time_base.num, st->time_base.den);
        if( ffpack_isVideoStream(st) )
        {
            LOGD("stream %d: avg_framerate(num,den) = %d,%d", i, st->avg_frame_rate.num, st->avg_frame_rate.den);
            LOGD("stream %d: r_framerate(num,den) = %d,%d", i, st->r_frame_rate.num, st->r_frame_rate.den);
        }
    }
}

int ffpack_start(FFPack_t* ff)
{
    int ret = 0;

    //ffpack_dumpTimebase(ff);

    ret = avformat_write_header(ff->ofmtContext, NULL);
    if (ret < 0)
    {
        ff_printerr("failed: ", ret);
        return ret;
    }

    ffpack_dumpTimebase(ff);
    av_dump_format(ff->ofmtContext, 0, ff->ofmtContext->url, 1);

    LOGD("done\n");
    return ret;
}

AVStream* ffpack_findStream(FFPack_t* ff, int streamIndex)
{
    for(int i=0; i<ff->ofmtContext->nb_streams; i++)
    {
        if(ff->ofmtContext->streams[i]->index == streamIndex)
        {
            return ff->ofmtContext->streams[i];
        }
    }
    return NULL;
}

void ffpack_setParams(FFPack_t* ff, int streamIndex, void* param)
{
    AVStream* stream = ffpack_findStream(ff, streamIndex);
    AVCodecParameters* cp = stream->codecpar;

#if(1)
    int w = cp->width;
    int h = cp->height;

    avcodec_parameters_copy(stream->codecpar, (const AVCodecParameters*)param);

    if(cp->width == 0) cp->width = w;
    if(cp->height== 0) cp->height= h;
#else
    AVCodecParameters* src = (AVCodecParameters*)param;
    AVCodecParameters* dst = stream->codecpar;

    //for mp4
    dst->codec_tag = 0;//src->codec_tag;
    //dst->format = 12;//src->format;
    //dst->profile = 100;//src->profile;
    //dst->level = 51;//src->level;

    //for mpegts
    if(src->extradata)
    {
        dst->extradata = av_mallocz(src->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
        if (!dst->extradata)
            return;
        memcpy(dst->extradata, src->extradata, src->extradata_size);
        dst->extradata_size = src->extradata_size;
    }
#endif
}

int ffpack_input(FFPack_t* ff, int streamIndex, uint8_t* frameData, int frameLen, bool keyFrame, uint64_t pts)
{
	int nRet = 0;

	AVPacket pkt;

	av_init_packet(&pkt);

	pkt.stream_index = streamIndex;
	pkt.data = frameData;
	pkt.size = frameLen;
	pkt.pts = pkt.dts = pts;
	pkt.flags |= keyFrame ? AV_PKT_FLAG_KEY : 0;

#if(1)
	AVStream* st = ff->ofmtContext->streams[streamIndex];

	//LOGD("pts0: %lld", pkt.pts);
    AVRational time_base = (AVRational){1, 100000};
    av_packet_rescale_ts(&pkt, time_base, st->time_base);
	//LOGD("pts1: %lld", pkt.pts);
	//LOGD("codec_tag=%d", st->codecpar->codec_tag);
	//LOGD("extradata_size=%d", st->codecpar->extradata_size);
	//LOGD("format=%d", st->codecpar->format);
	//LOGD("sample_aspect_ratio=%d,%d", st->codecpar->sample_aspect_ratio.num, st->codecpar->sample_aspect_ratio.den);
	//LOGD("video_delay=%d", st->codecpar->video_delay);
#endif

	nRet = av_interleaved_write_frame(ff->ofmtContext, &pkt);
	//nRet = av_write_frame(ff->ofmtContext, &pkt);

	//av_packet_unref(&pkt);

	ff->nbTotalSize += frameLen;

	if(nRet != 0)
    {
        ff_printerr("ffpack_Input: ", nRet);
    }

	return nRet;
}

int ffpack_inputStream(FFPack_t* ff, int streamIndex, void* pstream, void* ppkt)
{
    static int nbFrames = 0;
	int nRet = 0;

	AVStream* in_stream = (AVStream*)pstream;
	AVPacket* pkt = (AVPacket*)ppkt;
	AVStream* stream = ffpack_findStream(ff, streamIndex);

#if(1)
    if(pkt->pts==AV_NOPTS_VALUE){
        //Write PTS
        AVRational time_base1=in_stream->time_base;
        //Duration between 2 frames (us)
        int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
        //Parameters
        pkt->pts=(double)(nbFrames*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
        pkt->dts=pkt->pts;
        pkt->duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
    }
    pkt->pts = av_rescale_q_rnd(pkt->pts, in_stream->time_base, stream->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
    pkt->dts = av_rescale_q_rnd(pkt->dts, in_stream->time_base, stream->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
    pkt->duration = av_rescale_q(pkt->duration, in_stream->time_base, stream->time_base);
    pkt->pos = -1;
#else
    pkt->pts = nbFrames * 30*90;
    pkt->dts = pkt->pts;
#endif
	pkt->stream_index = streamIndex;

	nRet = av_interleaved_write_frame(ff->ofmtContext, pkt);
	//nRet = av_write_frame(ff->ofmtContext, pkt);

	nbFrames++;
	ff->nbTotalSize += pkt->size;

	if(nRet != 0)
    {
        ff_printerr("ffpack_Input: ", nRet);
    }

	return nRet;
}

int  ffpack_setExtradata(FFPack_t* ff, int streamIndex, void* extradata, int extradataSize)
{
    int nRet = 0;

    AVCodecParameters* cp = ff->ofmtContext->streams[streamIndex]->codecpar;
    if( cp->extradata_size == 0 ) {
        cp->extradata_size = extradataSize;
        cp->extradata = (uint8_t*)av_mallocz(cp->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
        memcpy(cp->extradata, extradata, extradataSize);

        nRet = ffpack_start(ff);
    }

    return nRet;
}

/******************************************************************************

  Copyright (C), 2016-2017, Divimath Inc.

 ******************************************************************************
  File Name     : avshare.cpp
  Version       : Initial version
  Author        : Divimath Shenzhen
  Created       : 2017/01/06
  Description   :
  History       :
  1.Date        : 2017/01/06
    Author      : james
    Modification: Created file

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <pthread.h>

#include "avshare.h"
#include "live_context.h"

#define MEM_KEY 0x568066
#define largeStreamSize		10*1024	//0x100000
#define littleStreamSize	5*1024	//0x80000
#define audioStreamSize		1*1024	//0x40000
#define SHAREBUF_COUNT		56		//8*7

#define RESET_IsConnect(p, num)	    {gAvshare.ptrConnect=p; memset(gAvshare.ptrConnect, 0, num);}
#define CHECK_IsConnect(no)	        (gAvshare.ptrConnect[MEDIA_type2index(no)])
#define SET_IsConnect(no, x)	    {gAvshare.ptrConnect[MEDIA_type2index(no)] = x; }
#define MEDIA_type2index(t)         ((t)-MEDIA_VIDEO)
#define CHECK_IsInitialized(ret)    {if(gAvshare.bufShare == NULL) return (ret);}

#define FIFO_NAME    "rtspStream.fifo"//"/tmp/H264_fifo"
#define MEDIA_NUM     2
#define FIFO_EN       1

#define SAVE_FILE_TEST  0
#define SAVE_FILE_PATH  "/mnt/extsd/test_share_rx.264"

#pragma pack(push)
#pragma pack(1)

typedef struct {
	char wflag;     //ÊÇ·ñ¿ÉÐ´£¬µ±Îª0Ê±¿É¶Á²»¿ÉÐ´£¬¶ÁÈ¡½ø³Ì¶ÁÍêÖ®ºóÖÃ1£»µ±Îª1Ê±¿ÉÐ´²»¿É¶Á£¬Ð´Èë½ø³ÌÐ´ÍêºóÖÃ0¡£
	char frameType; //ÊÓÆµÖ¡ÀàÐÍ£º IÖ¡:1 PÖ¡:0
	char mediaType; //Ö¡ÀàÐÍ£ºvedio:0 audio:1
	char buff_type;
	uint64_t  timestamp;
	int  length;
} ShareStream_t;

#pragma pack(pop)

enum _BUFF_TYPE
{
	BUF_BEGIN,
	BUF_MIDDLE,
	BUF_END,
	BUF_BEG_END,
};

typedef struct
{
	int	id;
	uint8_t* bufShare;
	uint8_t* ptrConnect;

	ShareStream_t* bufStreamHead[MEDIA_NUM];
    char * streamBuf[MEDIA_NUM][SHAREBUF_COUNT];
	int idxNext[MEDIA_NUM];
	int posNext[MEDIA_NUM];
#if(!FIFO_EN)
	int fdPipe;
#endif
	//int readOffset;
	int nFrames;
	int nFrameI;
	int nSPS;

#if(SAVE_FILE_TEST)
	FILE*     fileTest;
    pthread_t threadId;
    bool      bExit;
#endif
} AvshareInfo_t;


//----------------------------------------------------------------------------------------------
// global variables
//----------------------------------------------------------------------------------------------
AvshareInfo_t gAvshare = { -1, NULL, NULL };

//----------------------------------------------------------------------------------------------
// avshare initialize
//----------------------------------------------------------------------------------------------
int avshare_init(void)
{
    int j = 0;
    int i = 0;

	int SHARE_CAPACITY = (largeStreamSize + audioStreamSize + sizeof(ShareStream_t)*MEDIA_NUM)*SHAREBUF_COUNT + MEDIA_NUM;

	if(gAvshare.id != -1)
    {
        //avshare_uninit();
        //avshare_flush();
        return 0;
    }

	memset(&gAvshare, 0, sizeof(gAvshare));
    memset(gAvshare.streamBuf, 0, sizeof(gAvshare.streamBuf));

	gAvshare.id = shmget( MEM_KEY, SHARE_CAPACITY, IPC_CREAT|0644 );
	if( gAvshare.id < 0)
	{
		alogd("failed to create share memory\n");
		return -1;
	}

	gAvshare.bufShare = (uint8_t*)shmat(gAvshare.id, NULL, 0);
	if (gAvshare.bufShare == (void*)-1)
	{
		alogd("get share mem address failed\n");
		gAvshare.bufShare = NULL;
		return -2;
	}

	RESET_IsConnect(gAvshare.bufShare, MEDIA_NUM);
 /*        ____________________________________________________________________________________________________
  format:|connect |        (video + audio) header           |          (video + audio) stream                  |
  bytes: |-2(v,a)-|-(sizeof(ShareStream_t)*2*SHAREBUF_COUNT-|-(largeStreamSize+audioStreamSize)*SHAREBUF_COUNT-|
 */

	for(i=0; i<MEDIA_NUM; i++)
	{
		gAvshare.bufStreamHead[i] = (ShareStream_t*)(gAvshare.bufShare + MEDIA_NUM + i*sizeof(ShareStream_t)*SHAREBUF_COUNT);
		for(j=0; j<SHAREBUF_COUNT; j++)
		{
			gAvshare.bufStreamHead[i][j].wflag = 1;
		}
	}

	for(j=0; j<SHAREBUF_COUNT; j++)  //video
	{
		gAvshare.streamBuf[0][j] = (char*)(gAvshare.bufShare + MEDIA_NUM + sizeof(ShareStream_t)*SHAREBUF_COUNT*MEDIA_NUM + j*largeStreamSize);
	}

	for(j=0; j<SHAREBUF_COUNT; j++)  //audio
	{
		gAvshare.streamBuf[1][j] = gAvshare.streamBuf[0][SHAREBUF_COUNT-1] + largeStreamSize + j*audioStreamSize;
	}

    for(i=0; i<MEDIA_NUM; i++)
    {
	    gAvshare.idxNext[i] = 0;
	    gAvshare.posNext[i] = 0;
    }

	//alogd("bufShare=%x, countOfBuffer=%d\n", gAvshare.bufShare, SHAREBUF_COUNT);
#if(!FIFO_EN)
    gAvshare.fdPipe = open(FIFO_NAME, O_RDONLY|O_NONBLOCK);
#endif

	return 0;
}

int avshare_uninit(void)
{
	CHECK_IsInitialized(0);

	if(shmdt(gAvshare.bufShare)==-1)
	{
		aloge("remove share mem failed\n");
	}

	if(0)//shmctl(gAvshare.id,IPC_RMID,NULL) == -1)//remove id
  	{
    	aloge("remove share mem ID failed\n");
 	}

#if(!FIFO_EN)
 	if(gAvshare.fdPipe >= 0)
    {
        close(gAvshare.fdPipe);
        gAvshare.fdPipe = -1;
    }
#endif

    gAvshare.id = -1;
    gAvshare.bufShare = NULL;

   	alogd("uninit share memory\n");

  	return 0;
}

//----------------------------------------------------------------------------------------------
// avshare server
//----------------------------------------------------------------------------------------------
/*
pstStream:流数据
len:流数据长度
frame_type: 1:I帧; 0:P帧，音频帧不生效，可默认赋值为0
millis: 时间戳
streamType: 0:大码流; 1:小码流; 2:音频流
*/

int avshare_add(uint8_t* pstStream, int len, FrameType_e frame_type, uint32_t millis, MediaType_e streamType)
{
    int buff_size[MEDIA_NUM];
	ShareStream_t* pStreamHead = NULL;
	int data_send = 0;
	int data_len = len;
    int connect_flag = 0;

	buff_size[0] = largeStreamSize;
	buff_size[1] = audioStreamSize;

	CHECK_IsInitialized(-2);

	while( data_len > 0)
	{
        connect_flag = MEDIA_type2index(streamType);
		pStreamHead = (ShareStream_t*)&gAvshare.bufStreamHead[connect_flag][gAvshare.idxNext[connect_flag]];

		if(!CHECK_IsConnect(streamType))
		{
			gAvshare.idxNext[connect_flag]= 0;
			return -3;
		}

		int buffer_len = data_len;
		if( buffer_len > buff_size[connect_flag] )
		{
			buffer_len = buff_size[connect_flag];
		}

		//alogd("debug --share_stream_header[%d][%d]---AddToShareStream-1--%d,%d,%d,%d\n",streamType,indexOfBuf[streamType],pShareStreamHeader->wflag,streamType,pShareStreamHeader->frameType,frame_type);
		//alogd("debug -----AddToShareStream-1--%d,%d,%d,%d\n",pShareStreamHeader->wflag,streamType,pShareStreamHeader->frameType,frame_type);
		if(pStreamHead->wflag == 1)
		{
			pStreamHead->frameType = frame_type;
			pStreamHead->timestamp = millis;
			pStreamHead->mediaType = streamType;

			memcpy((uint8_t*)gAvshare.streamBuf[connect_flag][gAvshare.idxNext[connect_flag]], pstStream+data_send, buffer_len);

			pStreamHead->length    = buffer_len;
			if( data_send == 0 )//begin
			{
				if( data_len > buff_size[connect_flag] )
				{
					pStreamHead->buff_type   = BUF_BEGIN;
                    //alogd("frame begin %d\n", gAvshare.idxNext);
				}
				else
				{
					pStreamHead->buff_type   = BUF_BEG_END;
                    //alogd("frame begin end %d\n", gAvshare.idxNext);
				}
			}
			else
			{
				if( data_len > buff_size[connect_flag] )
				{
					pStreamHead->buff_type   = BUF_MIDDLE;
				}
				else
				{
					pStreamHead->buff_type   = BUF_END;
                    //alogd("frame end %d\n", gAvshare.idxNext);
				}
			}

			pStreamHead->wflag = 0;
			gAvshare.idxNext[connect_flag] = (gAvshare.idxNext[connect_flag]+1) % SHAREBUF_COUNT;

			data_len -= buffer_len;
			data_send += buffer_len;
		}
		else  //对有连接却没读取数据的情况做处理
		{
			return 1;
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------------------------------------
void avshare_addaud( unsigned char * buf, int len, uint32_t millis )
{
#if(0)
	//alogd("debug --------------1\n");
	int ret=avshare_add(buf, len, 1, millis, 2); //  add to share memory, Audio stream
	if(ret==1)
	{
		//buffer fuul, dropped
		static int iDropped = 0;

		iDropped++;
		if(iDropped >= 100)
		{
			alogd("share buffer overflowed, dropped %d audio packets\n", iDropped);
		}
	}
#endif
}

void avshare_addvid(uint8_t* pstStream, int len, FrameType_e frameType, uint32_t millis )
{
	static int iDropped = 0;
	int ret = 0;

#if(SAVE_FILE_TEST)
	if( gAvshare.fileTest == NULL ) {
        gAvshare.fileTest = fopen(SAVE_FILE_PATH, "wb");
	}
	if( gAvshare.fileTest != NULL ) {
        fwrite(pstStream, len, 1, gAvshare.fileTest);
	}
#endif

	if(iDropped > 0)
	{
		if(frameType != FRAME_I)
		{
			iDropped++;
			return;
		}
		else
		{
			alogd("share buffer overflowed, dropped %d video packets\n", iDropped);
			iDropped = 0;
		}
	}

	ret = avshare_add(pstStream, len, frameType, millis, MEDIA_VIDEO);// add to share memory
	if( ret == 1 )
	{
		//buffer full, dropped a frame
		iDropped++;
	}
	else if (0 == ret )
	{
		//added a frame
	}
}

void avshare_flush(void)
{
	int j;
    int i;

	if(gAvshare.bufShare != NULL)
	{
        for(i=0; i<MEDIA_NUM; i++)
        {
		    for(j=0; j<SHAREBUF_COUNT; j++)
		    {
		    	gAvshare.bufStreamHead[i][j].wflag = 1;
		    }
		    gAvshare.idxNext[i] = 0;
            gAvshare.posNext[i] = 0;
        }
	}
}

//----------------------------------------------------------------------------------------------
// avshare client
//----------------------------------------------------------------------------------------------
int avshare_connect(int media_type)
{
	if(!CHECK_IsConnect(media_type))
	{
		SET_IsConnect(media_type, 1);
		gAvshare.idxNext[media_type] = 0;
	}

	alogd("connect stream %d successfully: %d\n", media_type, CHECK_IsConnect(media_type));

	return 0;
}

int avshare_disconnect(int media_type)
{
	int i=0;

	CHECK_IsInitialized(0);
	SET_IsConnect(media_type, 0);

	//sleep(1);
	usleep(100);//ä¿è¯writeä¸åç»§ç»­ï¼åå° wflagé½ç½®ä¸º1ï¼writeçindexOfBufå°è¢«ç½®ä¸º0;

	for(i=0; i<SHAREBUF_COUNT; i++)
	{
		gAvshare.bufStreamHead[media_type][i].wflag = 1;
	}

	alogd("disconnect stream %d successfully\n", media_type);

	return 0;
}

int avshare_readFrame(uint8_t*sFrameBuf, int nBufLen, struct timeval* timestamp, unsigned* uDurationInMicroseconds)
{
	ShareStream_t* pStreamHead = NULL;
	uint8_t* pStreamData = NULL;
	uint8_t  mediaIndex = MEDIA_type2index(MEDIA_VIDEO);
	FrameType_e frameType = FRAME_NUM;
	int frameLen = 0;
	int copyLen = 0;
	int idxBuf = gAvshare.idxNext[mediaIndex];
	uint32_t offset = gAvshare.posNext[mediaIndex];
    int i = 0;

	CHECK_IsInitialized(0);
    pStreamHead = (ShareStream_t*)&gAvshare.bufStreamHead[mediaIndex][idxBuf];

	if( pStreamHead->wflag==1 )
	{
		for(i=0; i<SHAREBUF_COUNT; i++)
        {
            idxBuf = (idxBuf+1) % SHAREBUF_COUNT;
            pStreamHead = (ShareStream_t*)&gAvshare.bufStreamHead[mediaIndex][idxBuf];
            if(pStreamHead->wflag != 1)
            {
                offset = 0;
                break;
            }
        }

		if( i>=SHAREBUF_COUNT )
        {
            gAvshare.idxNext[mediaIndex] = idxBuf;
            gAvshare.posNext[mediaIndex] = 0;
            return -1;
        }
	}

    timestamp->tv_sec = (pStreamHead->timestamp/1000);          //s
    timestamp->tv_usec= ((pStreamHead->timestamp%1000)*1000);   //us
    *uDurationInMicroseconds = 0;

    frameType = (FrameType_e)pStreamHead->frameType;
    pStreamData = (uint8_t*)gAvshare.streamBuf[mediaIndex][idxBuf];

	do
    {
        copyLen = pStreamHead->length - offset;
        if( frameLen + copyLen <= nBufLen )//MAXSTREAMBUF )
        {
            memcpy( sFrameBuf + frameLen, pStreamData + offset, copyLen );
            frameLen += copyLen;
        }
        else
        {
            copyLen = nBufLen - frameLen;
            memcpy( sFrameBuf + frameLen, pStreamData + offset, copyLen );
            frameLen += copyLen;
            offset += copyLen;
            break;
        }

        pStreamHead->wflag = 1;
        idxBuf = (idxBuf+1) % SHAREBUF_COUNT;
        pStreamHead = (ShareStream_t*)&gAvshare.bufStreamHead[mediaIndex][idxBuf];;
        pStreamData = (uint8_t*)gAvshare.streamBuf[mediaIndex][idxBuf];
        offset = 0;
    } while(pStreamHead->wflag != 1);

    gAvshare.idxNext[mediaIndex] = idxBuf;
    gAvshare.posNext[mediaIndex] = offset;

	return frameLen;
}

int avshare_readAudio(uint8_t*sFrameBuf, int nBufLen, struct timeval* timestamp, unsigned* uDurationInMicroseconds)
{
	ShareStream_t* pStreamHead = NULL;
	uint8_t* pStreamData = NULL;
	uint8_t  mediaIndex = MEDIA_type2index(MEDIA_AUDIO);
	int audioLen = 0;
	int idxBuf = gAvshare.idxNext[mediaIndex];
    int bFrameEnd = 0;
    int i = 0;

	CHECK_IsInitialized(0);
    pStreamHead = (ShareStream_t*)&gAvshare.bufStreamHead[mediaIndex][idxBuf];

	if( pStreamHead->wflag==1 )
	{
		for(i=0; i<SHAREBUF_COUNT; i++)
        {
            idxBuf = (idxBuf+1) % SHAREBUF_COUNT;
            pStreamHead = (ShareStream_t*)&gAvshare.bufStreamHead[mediaIndex][idxBuf];
            if(pStreamHead->wflag != 1)
            {
                break;
            }
        }

		if( i>=SHAREBUF_COUNT )
        {
            //alogd("No frame to be sent...\n");
            if(audioLen <= 0)
            {
                gAvshare.idxNext[mediaIndex] = idxBuf;
            }

            return -1;
        }
	}
	if( (pStreamHead->buff_type != BUF_BEGIN)  && (pStreamHead->buff_type != BUF_BEG_END) )
	{
		alogd("No audio frame start found... %d-%d\n", idxBuf, pStreamHead->buff_type);
        pStreamHead->wflag = 1;
        idxBuf = (idxBuf+1) % SHAREBUF_COUNT;
        if(audioLen <= 0)
        {
            gAvshare.idxNext[mediaIndex] = idxBuf;
        }

		return -2;
	}

    timestamp->tv_sec = (pStreamHead->timestamp/90000);     //s
    timestamp->tv_usec = (pStreamHead->timestamp*100/9);    //us
    *uDurationInMicroseconds = 0;

	while(bFrameEnd == 0)
    {

        pStreamData = (uint8_t*)gAvshare.streamBuf[mediaIndex][idxBuf];
        //sendBuf = streamBuf[stream][indexOfBuf[stream]];
        if( audioLen + pStreamHead->length <= nBufLen )//MAXSTREAMBUF )
        {
            memcpy( sFrameBuf + audioLen, pStreamData, pStreamHead->length );
            audioLen += pStreamHead->length;
        }
        else
        {
            alogd("polldata audio err, out of buf size size=%d", audioLen + pStreamHead->length );
        }

        if( (pStreamHead->buff_type == BUF_END) || (pStreamHead->buff_type == BUF_BEG_END) )
        {
            //a whole frame found
            //alogd("frame buffer %d - %d\n", gAvshare.idxNext, idxBuf);
            bFrameEnd = 1;
        }

        idxBuf = (idxBuf+1) % SHAREBUF_COUNT;
        if(!bFrameEnd)
        {
            pStreamHead = (ShareStream_t*)&gAvshare.bufStreamHead[mediaIndex][idxBuf];;
            if( pStreamHead->wflag==1 )
            {
                //not a complete frame
                bFrameEnd = 2;
            }
            else if(idxBuf == gAvshare.idxNext[mediaIndex])
            {
                //buffer empty
                bFrameEnd = 3;
            }
        }
    }

    if(bFrameEnd == 1)
    {
        //alogd("audioLen - %d, timestamp - %d / %d\n", audioLen, (timestamp->tv_usec-timestampLast.tv_usec), \
        //          (fTimeNow.tv_usec-fTimeLast.tv_usec));

        //release frame buffers
        if(idxBuf < gAvshare.idxNext[mediaIndex])
        {
            for(i=gAvshare.idxNext[mediaIndex]; i<SHAREBUF_COUNT; i++)
            {
                pStreamHead = (ShareStream_t*)&gAvshare.bufStreamHead[mediaIndex][i];
                pStreamHead->wflag = 1;
            }
            for(i=0; i<idxBuf; i++)
            {
                pStreamHead = (ShareStream_t*)&gAvshare.bufStreamHead[mediaIndex][i];
                pStreamHead->wflag = 1;
            }
        }
        else
        {
            for(i=gAvshare.idxNext[mediaIndex]; i<idxBuf; i++)
            {
                pStreamHead = (ShareStream_t*)&gAvshare.bufStreamHead[mediaIndex][i];
                pStreamHead->wflag = 1;
            }
        }

        gAvshare.idxNext[mediaIndex] = idxBuf;
    }
    else
    {
        audioLen = 0;
    }

	return audioLen;
}

#if(SAVE_FILE_TEST)
//----------------------------------------------------------------------------------------------
// avshare thread
//----------------------------------------------------------------------------------------------
void* avshare_task(void * param)
{
    AvshareInfo_t* shareInfo = (AvshareInfo_t*)param;
    int fMaxSize = 2*1024*1024;
    uint8_t* fTo = (uint8_t*)malloc(fMaxSize);
    struct timeval timeStamp;
    unsigned uDurationInMicroseconds;

    gAvshare.fileTest = fopen(SAVE_FILE_PATH, "wb");

    //成功绑定之后，便开始监听
    while(!shareInfo->bExit)
    {
        int nLen = avshare_readFrame(fTo, fMaxSize, &timeStamp, &uDurationInMicroseconds);
        if(nLen < 0)
        {
            usleep(10000);
        }
        else {
            if( gAvshare.fileTest != NULL ) {
                fwrite(fTo, nLen, 1, gAvshare.fileTest);
            }
        }
    }

    free(fTo);
    fclose(gAvshare.fileTest);

    return (void*)0;
}
#endif

bool avshare_start(void)
{
    avshare_init();
    avshare_connect(MEDIA_VIDEO);

#if(SAVE_FILE_TEST)
    gAvshare.bExit = false;
    pthread_create(&gAvshare.threadId, NULL, avshare_task, &gAvshare);
#endif

    return true;
}

void avshare_stop(void)
{
    if( gAvshare.bufShare == NULL ) {
        return;
    }

    avshare_disconnect(MEDIA_VIDEO);
#if(SAVE_FILE_TEST)
    if( gAvshare.threadId != 0 ) {
        gAvshare.bExit = true;
        pthread_join(gAvshare.threadId, NULL);
        gAvshare.threadId = 0;
    }
#endif
    avshare_uninit();
}

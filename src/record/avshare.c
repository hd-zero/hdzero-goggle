/******************************************************************************

  Copyright (C), 2016-2017, Divimath Inc.

 ******************************************************************************
  File Name     : avshare.c
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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "avshare.h"

#define MEM_KEY          0x568066
#define largeStreamSize  10 * 1024 // 0x100000
#define littleStreamSize 5 * 1024  // 0x80000
#define audioStreamSize  1 * 1024  // 0x40000
#define SHAREBUF_COUNT   56        // 8*7
#define FRAME_LenMAX     (3 * 1024 * 1024)
#define AUDIO_LenMAX     (32 * 1024)

#define RESET_IsConnect(p, num)              \
    {                                        \
        gAvshare.ptrConnect = p;             \
        memset(gAvshare.ptrConnect, 0, num); \
    }
#define CHECK_IsConnect(no) (gAvshare.ptrConnect[MEDIA_type2index(no)])
#define SET_IsConnect(no, x) \
    { gAvshare.ptrConnect[MEDIA_type2index(no)] = x; }
#define MEDIA_type2index(t) ((t)-MEDIA_VIDEO)

#define FIFO_NAME "rtspStream.fifo" //"/tmp/H264_fifo"

#define MEDIA_NUM 2
#define FIFO_EN   1

#define PRINT_INFO 1
#define PRINT_LINE 0

#if (PRINT_INFO)

#if (PRINT_LINE)
#define dpf(fmt, args...) printf("[airsink>%s:%d]" fmt, __FILE__, __LINE__, ##args)
#else
#define dpf printf
#endif

#else
#define dpf(x...)
#endif

#if (PRINT_LINE)
#define dperr(fmt, args...) fprintf(stderr, "[airsink>%s:%d]" fmt, __FILE__, __LINE__, ##args)
#else
#define dperr printf
#endif

#define SAVE_FILE_TEST 0
#define SAVE_FILE_PATH "/mnt/extsd/test_share_tx.264"

#pragma pack(push)
#pragma pack(1)

typedef struct {
    char wflag;     //\CAǷ\F1\BF\C9д\A3\AC\B5\B1Ϊ0ʱ\BFɶ\C1\B2\BB\BF\C9д\A3\AC\B6\C1ȡ\BD\F8\B3̶\C1\CD\EA֮\BA\F3\D6\C31\A3\BB\B5\B1Ϊ1ʱ\BF\C9д\B2\BB\BFɶ\C1\A3\ACд\C8\EB\BD\F8\B3\CCд\CD\EA\BA\F3\D6\C30\A1\A3
    char frameType; //\CA\D3Ƶ֡\C0\E0\D0ͣ\BA I֡:1 P֡:0
    char mediaType; // ֡\C0\E0\D0ͣ\BAvedio:0 audio:1
    char buff_type;
    uint64_t timestamp;
    int length;
} ShareStream_t;

#pragma pack(pop)

enum _BUFF_TYPE {
    BUF_BEGIN,
    BUF_MIDDLE,
    BUF_END,
    BUF_BEG_END,
};

typedef struct
{
    int id;
    uint8_t *bufShare;
    uint8_t *ptrConnect;

    ShareStream_t *bufStreamHead[MEDIA_NUM];
    uint8_t *streamBuf[MEDIA_NUM][SHAREBUF_COUNT];
    int idxNext[MEDIA_NUM];
#if (!FIFO_EN)
    int fdPipe;
#endif
    uint8_t *frameBuf;
    int frameLen;
    int pesLen;
    uint64_t timestamp;

    uint8_t *audioBuf;
    int audioLen;
    int audiopesLen;
    uint64_t audiotimestamp;

#if (SAVE_FILE_TEST)
    FILE *fileTest;
#endif
} AvshareInfo_t;

AvshareInfo_t gAvshare;

int avshare_init(void) {
    int j = 0;
    int i = 0;
    // system_exec("killall rtspServer");//kill rtspServer，通过app_deamon使rtspServer重启，保证rtspServer读取的顺序是正确的
    // sleep(1);

    int SHARE_CAPACITY = (largeStreamSize + audioStreamSize + sizeof(ShareStream_t) * MEDIA_NUM) * SHAREBUF_COUNT + MEDIA_NUM;

    memset(&gAvshare, 0, sizeof(gAvshare));
    memset(gAvshare.streamBuf, 0, sizeof(gAvshare.streamBuf));

    if ((gAvshare.id = shmget(MEM_KEY, SHARE_CAPACITY, IPC_CREAT | 0644)) == -1) {
        dpf("failed to create share memory\n");
        return -1;
    }

    if ((gAvshare.bufShare = shmat(gAvshare.id, NULL, 0)) == (void *)-1) {
        dpf("get share mem address failed\n");
        gAvshare.bufShare = NULL;
        return -2;
    }

    RESET_IsConnect(gAvshare.bufShare, MEDIA_NUM);
    // sleep(1);
    /*        ____________________________________________________________________________________________________
     format:|connect |        (video + audio) header           |          (video + audio) stream                  |
     bytes: |-2(v,a)-|-(sizeof(ShareStream_t)*2*SHAREBUF_COUNT-|-(largeStreamSize+audioStreamSize)*SHAREBUF_COUNT-|
    */

    for (i = 0; i < MEDIA_NUM; i++) {
        gAvshare.bufStreamHead[i] = (ShareStream_t *)(gAvshare.bufShare + MEDIA_NUM + i * sizeof(ShareStream_t) * SHAREBUF_COUNT);
        for (j = 0; j < SHAREBUF_COUNT; j++) {
            gAvshare.bufStreamHead[i][j].wflag = 1;
        }
    }

    for (j = 0; j < SHAREBUF_COUNT; j++) // video
    {
        gAvshare.streamBuf[0][j] = gAvshare.bufShare + MEDIA_NUM + sizeof(ShareStream_t) * SHAREBUF_COUNT * MEDIA_NUM + j * largeStreamSize;
    }

    for (j = 0; j < SHAREBUF_COUNT; j++) // audio
    {
        gAvshare.streamBuf[1][j] = gAvshare.streamBuf[0][SHAREBUF_COUNT - 1] + largeStreamSize + j * audioStreamSize;
    }

    for (i = 0; i < MEDIA_NUM; i++)
        gAvshare.idxNext[i] = 0;

        // dpf("bufShare=%x, countOfBuffer=%d\n", gAvshare.bufShare, SHAREBUF_COUNT);
#if (!FIFO_EN)
    if (access(FIFO_NAME, F_OK) == -1) {
        mkfifo(FIFO_NAME, 0777);
    }
    gAvshare.fdPipe = open(FIFO_NAME, O_WRONLY | O_NONBLOCK);
#endif
    gAvshare.frameBuf = (uint8_t *)malloc(FRAME_LenMAX);
    if (gAvshare.frameBuf == NULL) {
        avshare_uninit();
        return -1;
    }
    gAvshare.audioBuf = (uint8_t *)malloc(AUDIO_LenMAX);
    if (gAvshare.audioBuf == NULL) {
        avshare_uninit();
        return -1;
    }

    return 0;
}

/*
pstStream:流数据
len:流数据长度
frame_type: 1:I帧; 0:P帧，音频帧不生效，可默认赋值为0
millis: 时间戳
streamType: 0:大码流; 1:小码流; 2:音频流
*/

int avshare_add(uint8_t *pstStream, int len, FrameType_e frame_type, uint32_t millis, MediaType_e streamType) {
    int buff_size[MEDIA_NUM];
    ShareStream_t *pStreamHead = NULL;
    int data_send = 0;
    int data_len = len;
    int connect_flag = 0;

    buff_size[0] = largeStreamSize;
    buff_size[1] = audioStreamSize;

    if (gAvshare.bufShare == NULL)
        return -2;

    while (data_len > 0) {
        connect_flag = MEDIA_type2index(streamType);
        pStreamHead = (ShareStream_t *)&gAvshare.bufStreamHead[connect_flag][gAvshare.idxNext[connect_flag]];

        if (!CHECK_IsConnect(streamType)) {
            gAvshare.idxNext[connect_flag] = 0;
            return -3;
        }

        int buffer_len = data_len;
        if (buffer_len > buff_size[connect_flag]) {
            buffer_len = buff_size[connect_flag];
        }

        // printf("debug --share_stream_header[%d][%d]---AddToShareStream-1--%d,%d,%d,%d\n",streamType,indexOfBuf[streamType],pShareStreamHeader->wflag,streamType,pShareStreamHeader->frameType,frame_type);
        // printf("debug -----AddToShareStream-1--%d,%d,%d,%d\n",pShareStreamHeader->wflag,streamType,pShareStreamHeader->frameType,frame_type);
        if (pStreamHead->wflag == 1) {
            pStreamHead->frameType = frame_type;
            pStreamHead->timestamp = millis;
            pStreamHead->mediaType = streamType;

            memcpy((uint8_t *)gAvshare.streamBuf[connect_flag][gAvshare.idxNext[connect_flag]], pstStream + data_send, buffer_len);

            pStreamHead->length = buffer_len;
            if (data_send == 0) // begin
            {
                if (data_len > buff_size[connect_flag]) {
                    pStreamHead->buff_type = BUF_BEGIN;
                    // printf("frame begin %d\n", gAvshare.idxNext);
                } else {
                    pStreamHead->buff_type = BUF_BEG_END;
                    // printf("frame begin end %d\n", gAvshare.idxNext);
                }
            } else {
                if (data_len > buff_size[connect_flag]) {
                    pStreamHead->buff_type = BUF_MIDDLE;
                } else {
                    pStreamHead->buff_type = BUF_END;
                    // printf("frame end %d\n", gAvshare.idxNext);
                }
            }

            pStreamHead->wflag = 0;
            gAvshare.idxNext[connect_flag] = (gAvshare.idxNext[connect_flag] + 1) % SHAREBUF_COUNT;

            data_len -= buffer_len;
            data_send += buffer_len;
        } else // 对有连接却没读取数据的情况做处理
        {
            return 1;
        }
    }

    return 0;
}

int avshare_uninit(void) {
    if (shmdt(gAvshare.bufShare) == -1) {
        dperr("remove share mem failed\n");
    }

    if (shmctl(gAvshare.id, IPC_RMID, NULL) == -1) // remove id
    {
        dperr("remove share mem ID failed");
    }
#if (!FIFO_EN)
    if (gAvshare.fdPipe >= 0) {
        close(gAvshare.fdPipe);
        gAvshare.fdPipe = -1;
    }
#endif
    if (gAvshare.frameBuf != NULL) {
        free(gAvshare.frameBuf);
        gAvshare.frameBuf = NULL;
    }

    if (gAvshare.audioBuf != NULL) {
        free(gAvshare.audioBuf);
        gAvshare.audioBuf = NULL;
    }

    return 0;
}

//-----------------------------------------------------------------------------------------------------------
void avshare_addaud(unsigned char *buf, int len, uint32_t millis) {
#if (0)
    // printf("debug --------------1\n");
    int ret = avshare_add(buf, len, 1, millis, 2); //  add to share memory, Audio stream
    if (ret == 1) {
        // buffer fuul, dropped
        static int iDropped = 0;

        iDropped++;
        if (iDropped >= 100) {
            dpf("share buffer overflowed, dropped %d audio packets\n", iDropped);
        }
    }
#endif
}

void avshare_addvid(uint8_t *pstStream, int len, FrameType_e frameType, uint32_t millis) {
    static int iDropped = 0;
    int ret = 0;

#if (SAVE_FILE_TEST)
    if (gAvshare.fileTest == NULL) {
        gAvshare.fileTest = fopen(SAVE_FILE_PATH, "wb");
    }
    if (gAvshare.fileTest != NULL) {
        fwrite(pstStream, len, 1, gAvshare.fileTest);
    }
#endif

    if (iDropped > 0) {
        if (frameType != FRAME_I) {
            iDropped++;
            return;
        } else {
            dpf("share buffer overflowed, dropped %d video packets\n", iDropped);
            iDropped = 0;
        }
    }

    ret = avshare_add(pstStream, len, frameType, millis, MEDIA_VIDEO); // add to share memory
    if (ret == 1) {
        // buffer full, dropped a frame
        iDropped++;
    } else if (0 == ret) {
        // added a frame
    }
}

void avshare_flush(void) {
    int j;
    int i;

    if (gAvshare.bufShare != NULL) {
        for (i = 0; i < MEDIA_NUM; i++) {
            for (j = 0; j < SHAREBUF_COUNT; j++) {
                gAvshare.bufStreamHead[i][j].wflag = 1;
            }
            gAvshare.idxNext[i] = 0;
        }
    }
}

int avshare_addAudio(uint8_t *data, int len, uint64_t timestamp, int isNewFrame, int pesLen) {
    if (isNewFrame &&
        (gAvshare.audioLen > 0) && //(gAvshare.timestamp != timestamp) )
        ((gAvshare.audiotimestamp != timestamp) || ((gAvshare.audiopesLen < 65536) && (gAvshare.audioLen == gAvshare.audiopesLen)))) {
        int ret = avshare_add(gAvshare.audioBuf, gAvshare.audioLen, 0, gAvshare.audiotimestamp, 2);
        if (ret == 0) {
            // dpf("add audio %d/%d - %x\n", gAvshare.audiopesLen, gAvshare.audioLen, gAvshare.audioBuf[0]);
        } else if (ret != -3) {
            dpf("add audio %d failed %d\n", gAvshare.audioLen, ret);
        }
        gAvshare.audioLen = 0;
    }

    memcpy(gAvshare.audioBuf + gAvshare.audioLen, data, len);
    gAvshare.audioLen += len;
    gAvshare.audiopesLen = pesLen;
    gAvshare.audiotimestamp = timestamp;

    return 0;
}

int avshare_addPacket(uint8_t *data, int len, uint64_t timestamp, int isNewFrame, int pesLen) {
    if (isNewFrame &&
        (gAvshare.frameLen > 0) && //(gAvshare.timestamp != timestamp) )   65498
        ((gAvshare.timestamp != timestamp) || ((gAvshare.pesLen < 65536) && (gAvshare.frameLen == gAvshare.pesLen)))) {
        int ret = avshare_add(gAvshare.frameBuf, gAvshare.frameLen, FRAME_I, gAvshare.timestamp, MEDIA_VIDEO);
        if (ret == 0) {
            // dpf("add frame %d/%d - %x %x %x %x %x %x\n", gAvshare.pesLen, gAvshare.frameLen, gAvshare.frameBuf[0], gAvshare.frameBuf[1],
            //     gAvshare.frameBuf[2], gAvshare.frameBuf[3], gAvshare.frameBuf[4], gAvshare.frameBuf[5]);
        } else if (ret != -3) {
            dpf("add frame %d failed %d\n", gAvshare.frameLen, ret);
        }
        gAvshare.frameLen = 0;
    }

    memcpy(gAvshare.frameBuf + gAvshare.frameLen, data, len);
    gAvshare.frameLen += len;
    gAvshare.pesLen = pesLen;
    gAvshare.timestamp = timestamp;

    return 0;
}

#if (!FIFO_EN)
int avshare_write(uint8_t *pstStream, int len) {
    if (gAvshare.fdPipe < 0) {
        gAvshare.fdPipe = open(FIFO_NAME, O_WRONLY | O_NONBLOCK);
        if (gAvshare.fdPipe < 0) {
            return 0;
        }
    }

    len = write(gAvshare.fdPipe, pstStream, len);
    if (len < 0) {
        close(gAvshare.fdPipe);
        gAvshare.fdPipe = -1;
    }

    return len;
}
#endif

//----------------------------------------------------------------------------------------------
// avshare client
//----------------------------------------------------------------------------------------------
int avshare_connect(MediaType_e media_type) {
    if (!CHECK_IsConnect(media_type)) {
        SET_IsConnect(media_type, 1);
        gAvshare.idxNext[media_type] = 0;
    }

    dpf("connect stream %d successfully - %d\n", media_type, CHECK_IsConnect(media_type));

    return 0;
}

int avshare_disconnect(MediaType_e media_type) {
    int i = 0;

    SET_IsConnect(media_type, 0);

    usleep(500 * 1000); // ä¿è¯writeä¸åç»§ç»­ï¼åå° wflagé½ç½®ä¸º1ï¼writeçindexOfBufå°è¢«ç½®ä¸º0;

    for (i = 0; i < SHAREBUF_COUNT; i++) {
        gAvshare.bufStreamHead[media_type][i].wflag = 1;
    }

    dpf("disconnect stream %d successfully\n", media_type);

    return 0;
}

bool avshare_connected(MediaType_e streamType) {
    if (!CHECK_IsConnect(streamType)) {
        int streamIndex = MEDIA_type2index(streamType);
        gAvshare.idxNext[streamIndex] = 0;
        return false;
    }

    return true;
}

void avshare_reset(void) {
    dpf("avshare reset\n");

    // RESET_IsConnect(gAvshare.bufShare, MEDIA_NUM);
    // avshare_flush();

#if (SAVE_FILE_TEST)
    if (gAvshare.fileTest != NULL) {
        fclose(gAvshare.fileTest);
        gAvshare.fileTest = NULL;
    }
#endif
}

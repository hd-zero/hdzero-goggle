#if defined (__cplusplus)
extern "C" {
#endif

//#define LOG_NDEBUG 0
#define LOG_TAG "gogglemsg"
#if(THIS_MTYPE != MTYPE_NONE)
#include <log/log.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "gogglemsg.h"

#define GoggleMSG_KEY	    0x5680	//goggle message key

#define CommandMSG_SIZE()	(sizeof(CommandMsg_t)-sizeof(long))
#define DataMSG_SIZE(n)	    ((n)-sizeof(long))

#if(THIS_MTYPE == MTYPE_NONE)
#define LOGE printf
#define LOGD printf
#endif

/* Codec gogglemsg */
/**
* @defgroup global vars

* This contains global variables definition.
* @{
*/

/** @} */

int gGoggleMsg = -1;

/**
* @ingroup functions implementation
* @defgroup functions implementation

* Implements the functions
* @{
*/
int gogglemsg_init( int r )
{
	gGoggleMsg = msgget(GoggleMSG_KEY, 0);
	if( !r )
	{
		if( gGoggleMsg < 0 )
		{
			gGoggleMsg = msgget(GoggleMSG_KEY, IPC_CREAT|0666);
		}
	}
	if( gGoggleMsg < 0 )
	{
		LOGE("failed to require %x", GoggleMSG_KEY);
		return -1;
	}

	return 0;
}

int gogglemsg_uninit( void )
{
	msgctl( gGoggleMsg, IPC_RMID, NULL);

	LOGE("msgid %d free!", gGoggleMsg);

	return 0;
}

int gogglemsg_send( GoggleMsgType_e mtype, GoggleMsgCommand_e cmd )
{
	CommandMsg_t msg;

	msg.mtype = mtype;
	msg.cmd = cmd;

	return msgsnd( gGoggleMsg, &msg, CommandMSG_SIZE(), 0 );
}

bool gogglemsg_seek(int qid, GoggleMsgType_e mtype)
{
    int ret;

    if( (ret=msgrcv(qid, NULL, 0, mtype, IPC_NOWAIT)) == -1 ) {
        return (errno == E2BIG);
    }

    return false;
}

int gogglemsg_recv( GoggleMsgType_e mtype )
{
	CommandMsg_t msg = {0, 0};

	if( msgrcv(gGoggleMsg, &msg, CommandMSG_SIZE(), mtype, IPC_NOWAIT) < 0 )
	{
		return MSG_none;
	}

	return msg.cmd;
}

int gogglemsg_sendData( GoggleMsgType_e mtype, GoggleMsgCommand_e cmd, void* data, int len )
{
    size_t nSize = sizeof(CommandMsg_t) + len;
    uint8_t* msgBuff = (uint8_t*)malloc(nSize);
    if( msgBuff == NULL ) {
        return -1;
    }

    DataMsg_t* msgData = (DataMsg_t*)msgBuff;
    msgData->msg.mtype = mtype;
    msgData->msg.cmd = cmd;
    memcpy(msgData->data, data, len);

    //LOGD("send: %d, %d, %d", len, DataMSG_SIZE(nSize), nSize);

	nSize = msgsnd( gGoggleMsg, msgBuff, DataMSG_SIZE(nSize), 0 );
	free(msgBuff);

	return nSize;
}

int gogglemsg_recvData( GoggleMsgType_e mtype, void* data, int len )
{
	if( !gogglemsg_seek(gGoggleMsg, mtype) ) {
        return MSG_none;
	}

    size_t nSize = sizeof(CommandMsg_t) + len;
    uint8_t* msgBuff = (uint8_t*)malloc(nSize);
    if( msgBuff == NULL ) {
        return -1;
    }
    memset(msgBuff, 0, nSize);

	if( msgrcv(gGoggleMsg, msgBuff, DataMSG_SIZE(nSize), mtype, IPC_NOWAIT) < 0 )
	{
	    free(msgBuff);
		return MSG_none;
	}

    DataMsg_t* msgData = (DataMsg_t*)msgBuff;
    memcpy(data, msgData->data, len);
    nSize = msgData->msg.cmd;
    free(msgBuff);
	return nSize;
}

int gogglemsg_flush( GoggleMsgType_e mtype )
{
	CommandMsg_t msg = {0, 0};

	while( msgrcv(gGoggleMsg, &msg, CommandMSG_SIZE(), mtype, IPC_NOWAIT) == 0 )
	{
		msg.cmd = 0;
		msg.mtype = 0;
	}

	return 0;
}

#if defined (__cplusplus)
}
#endif


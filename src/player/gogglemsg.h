#ifndef __GOGGLEMSG_H_
#define __GOGGLEMSG_H_

#if defined (__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include "appmsg.h"

/**
* @defgroup data structures

* This contains structure definitions
* @{
*/

typedef struct
{
	long mtype;
	long cmd;
} CommandMsg_t;

typedef struct
{
	CommandMsg_t msg;
	uint8_t data[1];
} DataMsg_t;

/** @} */

/**
* @ingroup function declaration
* @defgroup function declaration

* This is the place to declare functions.
* @{
*/
int gogglemsg_init( int r );
int gogglemsg_uninit( void );
int gogglemsg_send( GoggleMsgType_e mtype, GoggleMsgCommand_e cmd );
int gogglemsg_recv( GoggleMsgType_e mtype );
int gogglemsg_sendData( GoggleMsgType_e mtype, GoggleMsgCommand_e cmd, void* data, int len );
int gogglemsg_recvData( GoggleMsgType_e mtype, void* data, int len );
int gogglemsg_flush( GoggleMsgType_e mtype );
/** @} */

#if defined (__cplusplus)
}
#endif

#endif //__GOGGLEMSG_H_


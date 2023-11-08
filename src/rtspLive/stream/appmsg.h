#pragma once

#if defined (__cplusplus)
extern "C" {
#endif

#define THIS_MTYPE  MTYPE_LIVE

/**
* @defgroup data structures

* This contains structure definitions
* @{
*/

typedef enum
{
    MTYPE_NONE   = 0,
    MTYPE_RECORD = 1,
    MTYPE_PLAYER = 2,
    MTYPE_LIVE   = 3,
    MTYPE_BUT,
} GoggleMsgType_e;

typedef enum {
	MSG_none	 = 0,
	MSG_cmdQUIT  = 1,
	MSG_cmdSTART,
	MSG_cmdSTOP,

	MSG_cmdBUTT,
} GoggleMsgCommand_e;

typedef enum {
	REC_cmdSAVE = MSG_cmdBUTT,

	AIO_cmdSTART,
	AIO_cmdSTOP,

	REC_cmdBUTT,
} RecordMsgCommand_e;

/** @} */

#if defined (__cplusplus)
}
#endif

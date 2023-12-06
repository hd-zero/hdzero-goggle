#pragma once

#if defined (__cplusplus)
extern "C" {
#endif

#define THIS_MTYPE  MTYPE_PLAYER

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

	REC_cmdBUTT,
} RecordMsgCommand_e;

typedef enum {
	PLAY_cmdPAUSE = MSG_cmdBUTT,
	PLAY_cmdSEEK,

	PLAY_cmdBUTT,
} PlayerMsgCommand_e;

typedef struct
{
	long value;
} PlayerMsgData_t;

/** @} */

#if defined (__cplusplus)
}
#endif

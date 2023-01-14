/*
********************************************************************************
*                           Android multimedia module
*
*          (c) Copyright 2010-2015, Allwinner Microelectronic Co., Ltd.
*                              All Rights Reserved
*
* File   : EyeseeMessageQueue.h
* Version: V1.0
* By     : eric_wang
* Date   : 2016-09-12
* Description:
********************************************************************************
*/
#ifndef _EYESEEMESSAGEQUEUE_H_
#define _EYESEEMESSAGEQUEUE_H_

#include <list>
#include <utils/Errors.h>
//#include <utils/RefBase.h>
#include <utils/Mutex.h>
#include <utils/Condition.h>
//#include <utils/List.h>

namespace EyeseeLinux {

class EyeseeMessage
{
public:
    int                 mWhoseMsg;  //user define it, e.g. enum WhoseMessage
	int 				mMsgType;   //PlayerCommandType
	int 				mPara0;
	int 				mPara1;
private:
    void*               mpData;
    int                 mDataSize;
public:
    status_t    setData(char* pData, int nDataSize);
    void*       getData(int* pDataSize = NULL);
    status_t    reset();
    EyeseeMessage(const EyeseeMessage& rhs);
    EyeseeMessage& operator= (const EyeseeMessage& rhs);
    //move constructor, move assignment, param rRef can't be const, because we will change rRef's member.
    EyeseeMessage(EyeseeMessage&& rRef);
    EyeseeMessage& operator=(EyeseeMessage&& rRef);
    EyeseeMessage();
    ~EyeseeMessage();
};

class EyeseeMessageQueue
{
public:
    EyeseeMessageQueue();
    ~EyeseeMessageQueue(){}
    status_t queueMessage(EyeseeMessage *pMsgIn);
    status_t queueMessage(EyeseeMessage&& rRefMsgIn);
    status_t dequeueMessage(EyeseeMessage *pMsgOut);   //return value is NOT_ENOUGH_DATA, UNKNOWN_ERROR, NO_ERROR
    status_t flushMessage();
    int waitMessage(unsigned int timeout = 0);  //return value is the message number.
    int getMessageCount();
private:
    Mutex       mLock;
    Condition   mCondMessageQueueChanged;
    bool        mWaitMessageFlag;
    int         mCount; //message count.
    std::list<EyeseeMessage>  mValidMessageList;
    std::list<EyeseeMessage>  mIdleMessageList;
};

};

#endif  /* _EYESEEMESSAGEQUEUE_H_ */


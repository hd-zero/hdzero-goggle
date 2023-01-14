/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : CallbackDispatcher.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2016/04/29
  Last Modified :
  Description   : camera wrap MPP components.
  Function List :
  History       :
******************************************************************************/
#ifndef __IPCLINUX_CALLBACK_DISPATCHER_H__
#define __IPCLINUX_CALLBACK_DISPATCHER_H__

#include "plat_type.h"
#include "plat_errno.h"
#include "plat_defines.h"
#include "plat_math.h"

#include "Errors.h"

#include <utils/Thread.h>
#include <utils/Mutex.h>
#include <utils/Condition.h>
#include <CMediaMemory.h>

#include <list>
#include <vector>
#include <memory>

namespace EyeseeLinux
{

/**
 * 
 * Defines a message containing a description and arbitrary data object that can be
 * sent to a {@link Handler}.  This object contains two extra int fields and an
 * extra object field that allow you to not do allocations in many cases.  
 *
 * <p class="note">While the constructor of Message is public, the best way to get
 * one of these is to call {@link #obtain Message.obtain()} or one of the
 * {@link Handler#obtainMessage Handler.obtainMessage()} methods, which will pull
 * them from a pool of recycled objects.</p>
 */
struct CallbackMessage
{
    /**
     * User-defined message code so that the recipient can identify 
     * what this message is about. Each {@link Handler} has its own name-space
     * for message codes, so you do not need to worry about yours conflicting
     * with other handlers.
     */
    int what;
    /**
     * arg1 and arg2 are lower-cost alternatives to using
     * {@link #setData(Bundle) setData()} if you only need to store a
     * few integer values.
     */
    int arg1;
    int arg2;
    /**
     * extArgs are used to store more args.
     */
    std::vector<int> extArgs;
    /**
     * An arbitrary object to send to the recipient.  When using
     * {@link Messenger} to send the message across processes this can only
     * be non-null if it contains a Parcelable of a framework class (not one
     * implemented by the application).   For other data transfer use
     * {@link #setData}.
     * 
     * <p>Note that Parcelable objects here are not supported prior to
     * the {@link android.os.Build.VERSION_CODES#FROYO} release.
     */
    //const void *obj;
    std::shared_ptr<const CMediaMemory> mDataPtr;
};

class CallbackDispatcher
{
public:
    CallbackDispatcher();

    void post(const CallbackMessage &msg);
    bool loop();
    void removeCallbacksAndMessages(CallbackMessage* token);

protected:
    virtual ~CallbackDispatcher();
protected:
    class CallbackDispatcherThread : public Thread
    {
    public:
        CallbackDispatcherThread(CallbackDispatcher *dispatcher)
            : mpDispatcher(dispatcher)
        {
        }
    private:
        virtual bool threadLoop();
    private:
        CallbackDispatcher *mpDispatcher;
    };
private:
    Mutex mLock;

    bool mbDone;
    Condition mQueueChanged;
    std::list<CallbackMessage> mQueue;
    CallbackDispatcherThread *mpDispatcherThread;

    void dispatch(const CallbackMessage &msg);
    virtual void handleMessage(const CallbackMessage &msg)=0;
};

}; /* namespace EyeseeLinux */

#endif  /* __IPCLINUX_CALLBACK_DISPATCHER_H__ */



/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation, nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <LocThread.h>
#include <string.h>
#include <pthread_loc.h>
#include <log_util.h>
#include <loc_log.h>
#include "stringl.h"

class LocThreadDelegate {
    LocRunnable* mRunnable;
    bool mJoinable;
    pthread_t mThandle;
    pthread_mutex_t mMutex;
    int mRefCount;
    ~LocThreadDelegate();
    LocThreadDelegate(LocThread::tCreate creator, const char* threadName,
                      LocRunnable* runnable, bool joinable);
    void destroy();
public:
    static LocThreadDelegate* create(LocThread::tCreate creator,
            const char* threadName, LocRunnable* runnable, bool joinable);
    void stop();
    // bye() is for the parent thread to go away. if joinable,
    // parent must stop the spawned thread, join, and then
    // destroy(); if detached, the parent can go straight
    // ahead to destroy()
    inline void bye() { mJoinable ? stop() : destroy(); }
    inline bool isRunning() { return (NULL != mRunnable); }
    static void* threadMain(void* arg);
};

// it is important to note that internal members must be
// initialized to values as if pthread_create succeeds.
// This is to avoid the race condition between the threads,
// once the thread is created, some of these values will
// be check in the spawned thread, and must set correctly
// then and there.
// However, upon pthread_create failure, the data members
// must be set to  indicate failure, e.g. mRunnable, and
// threashold approprietly for destroy(), e.g. mRefCount.
LocThreadDelegate::LocThreadDelegate(LocThread::tCreate creator,
        const char* threadName, LocRunnable* runnable, bool joinable) :
    mRunnable(runnable), mJoinable(joinable), mThandle(NULL),
//    mMutex((qurt_mutex_t)PTHREAD_MUTEX_INITIALIZER), mRefCount(2) {
    mRefCount(2) {

    LOC_LOGL0("Inside LocThreadDelegate::LocThreadDelegate");
    pthread_mutex_init(&mMutex, NULL);

    // set up thread name, if nothing is passed in
    if (!threadName) {
        threadName = "LocThread";
    }

    // create the thread here, then if successful
    // and a name is given, we set the thread name
    int ret = -1;
    if (creator) {
        LOC_LOGM0("Calling creator");
        mThandle = creator(threadName, threadMain, this);
    } else if (0 != (ret = pthread_create(&mThandle, NULL, threadMain, this))) {
        // pthread_create() failed
        LOC_LOGE1("Failed to create pthread. err %d", ret);
        mThandle = NULL;
    }

    if (mThandle) {
        // set thread name
        char lname[16];
        int len = sizeof(lname) - 1;
        memscpy(lname, sizeof(lname), threadName, MIN(len, strnlen(threadName, len)));
        lname[len] = 0;
        // detach, if not joinable
        if (!joinable) {
            pthread_detach(mThandle);
        }
        LOC_LOGL1("Created pthread %p", mThandle);
    } else {
        // must set these values upon failure
        mRunnable = NULL;
        mJoinable = false;
        mRefCount = 1;
    }
}

inline
LocThreadDelegate::~LocThreadDelegate() {
    pthread_mutex_destroy(&mMutex);
    // at this point nothing should need done any more
}

// factory method so that we could return NULL upon failure
LocThreadDelegate* LocThreadDelegate::create(LocThread::tCreate creator,
        const char* threadName, LocRunnable* runnable, bool joinable) {
    LocThreadDelegate* thread = NULL;
    if (runnable) {
        LOC_LOGL0("Calling LocThreadDelegate::LocThreadDelegate");
        thread = new LocThreadDelegate(creator, threadName, runnable, joinable);
        if (thread && !thread->isRunning()) {
            thread->destroy();
            thread = NULL;
        }
    }

    return thread;
}

// The order is importang
// NULLing mRunnalbe stops the while loop in threadMain()
// join() if mJoinble must come before destroy() call, as
// the obj must remain alive at this time so that mThandle
// remains valud.
void LocThreadDelegate::stop() {
    // mRunnable and mJoinable are reset on different triggers.
    // mRunnable may get nulled on the spawned thread's way out;
    //           or here.
    // mJouinable (if ever been true) gets falsed when client
    //            thread triggers stop, with either a stop()
    //            call or the client releases thread obj handle.
    LOC_LOGL1("LocThread stop mRunnable=%d", mRunnable);
    if (mRunnable) {
        mRunnable = NULL;
    }
    if (mJoinable) {
        mJoinable = false;
        LOC_LOGL0("LocThread calling  pthread_join");
        pthread_join(mThandle, NULL);
    }
    // call destroy() to possibly delete the obj
    destroy();
}

// method for clients to call to release the obj
// when it is a detached thread, the client thread
// and the spawned thread can both try to destroy()
// asynchronously. And we delete this obj when
// mRefCount becomes 0.
void LocThreadDelegate::destroy() {
    // else case shouldn't happen, unless there is a
    // leaking obj. But only our code here has such
    // obj, so if we test our code well, else case
    // will never happen
    LOC_LOGL1("LocThread destroy mRefCount=%d", mRefCount);
    if (mRefCount > 0) {
        // we need a flag on the stack
        bool callDelete = false;

        // critical section between threads
        pthread_mutex_lock(&mMutex);
        // last destroy() call
        callDelete = (1 == mRefCount--);
        pthread_mutex_unlock(&mMutex);

        // upon last destroy() call we delete this obj
        if (callDelete) {
            pthread_kill((TX_THREAD*)mThandle);
            LOC_LOGL1("Killed pthread %p", mThandle);
        }
    }
    LOC_LOGL0("LocThread destroy exits");
}

void* LocThreadDelegate::threadMain(void* arg) {
    LocThreadDelegate* locThread = (LocThreadDelegate*)(arg);

    if (locThread) {
        LocRunnable* runnable = locThread->mRunnable;

        if (runnable) {
            if (locThread->isRunning()) {
                runnable->prerun();
            }

            while (locThread->isRunning() && runnable->run());

            if (locThread->isRunning()) {
                runnable->postrun();
            }

            // at this time, locThread->mRunnable may or may not be NULL
            // NULL it just to be safe and clean, as we want the field
            // in the released memory slot to be NULL.
            locThread->mRunnable = NULL;
            delete runnable;
        }
        locThread->destroy();
    }
    LOC_LOGH0("LocThread threadMain exits");
    return NULL;
}

LocThread::~LocThread() {
    LOC_LOGL1("LocThread destructor mThread=%p", mThread);
    if (mThread) {
        mThread->bye();
        mThread = NULL;
    }
}

bool LocThread::start(tCreate creator, const char* threadName, LocRunnable* runnable, bool joinable) {
    bool success = false;
    if (!mThread) {
        LOC_LOGL0("Calling LocThreadDelegate::create");
        mThread = LocThreadDelegate::create(creator, threadName, runnable, joinable);
        // true only if thread is created successfully
        success = (NULL != mThread);
    }
    return success;
}

void LocThread::stop(bool abort) {
    if (mThread) {
        mThread->stop();
        if (abort) {
            mThread->bye();
        }
        mThread = NULL;
    }
}

#ifdef __LOC_DEBUG__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class LocRunnableTest1 : public LocRunnable {
    int mID;
public:
    LocRunnableTest1(int id) : LocRunnable(), mID(id) {}
    virtual bool run() {
        printf("LocRunnableTest1: %d\n", mID++);
        //sleep(1);
        qurt_timer_sleep( 1 * 1000000 );//arg in micro seconds
        return true;
    }
};

// on linux command line:
// compile: g++ -D__LOC_HOST_DEBUG__ -D__LOC_DEBUG__ -g -std=c++0x -I. -I../../../../vendor/qcom/proprietary/gps-internal/unit-tests/fakes_for_host -I../../../../system/core/include -lpthread LocThread.cpp
// test detached thread: valgrind ./a.out 0
// test joinable thread: valgrind ./a.out 1
int main(int argc, char** argv) {
    LocRunnableTest1 test(10);

    LocThread thread;
    thread.start("LocThreadTest", test, atoi(argv[1]));

    //sleep(10);
    qurt_timer_sleep( 10 * 1000000 );//arg in micro seconds

    thread.stop();

    //sleep(5);
    qurt_timer_sleep( 5 * 1000000 );//arg in micro seconds

    return 0;
}

#endif

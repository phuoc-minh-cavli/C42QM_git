/* Copyright (c) 2011-2013,2015 The Linux Foundation. All rights reserved.
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
#define LOG_NDDEBUG 0
#define LOG_TAG "LocSvc_MsgTask"

//#include <cutils/sched_policy.h>
//#include <unistd.h>
#include <MsgTask.h>
#include <msg_q.h>
#include <log_util.h>
#include <loc_log.h>

static void LocMsgDestroy(void* msg) {
    delete (LocMsg*)msg;
}

MsgTask::MsgTask(LocThread::tCreate tCreator,
                 const char* threadName, bool joinable) :
    mQ(msg_q_init2()), mThread(new LocThread()), mPendingMsgCount(0) {
    if (!mThread->start(tCreator, threadName, this, joinable)) {
        delete mThread;
        mThread = NULL;
    }
}

MsgTask::MsgTask(const char* threadName, bool joinable) :
    mQ(msg_q_init2()), mThread(new LocThread()), mPendingMsgCount(0) {
    if (!mThread->start(threadName, this, joinable)) {
        delete mThread;
        mThread = NULL;
    }
}

MsgTask::~MsgTask() {
    msg_q_flush((void*)mQ);
    msg_q_destroy((void**)&mQ);
}

void MsgTask::destroy(bool abort) {
    LOC_LOGL3("MsgTask::destroy(%p) mQ=%p mThread=%p", this, mQ, mThread);
    LocThread* thread = mThread;
    msg_q_unblock((void*)mQ);
    if (thread) {
        mThread = NULL;
        if(abort) {
            thread->stop(true);
        }
        delete thread;
    } else {
        delete this;
    }
}

void MsgTask::sendMsg(const LocMsg* msg) const {
    if (msg == NULL) {
        LOC_LOGE0("sendMsg(): NULL msg");
        return;
    }
    if (NULL == mThread) {
        LOC_LOGE1("sendMsg(): mThread NULL in MsgTask %p", this);
        return;
    }
    msg->mMsgTaskPtr = this;
    LOC_LOGL2("Sending LocMsg %p on MsgTask %p", msg, msg->mMsgTaskPtr);
    msg_q_snd((void*)mQ, (void*)msg, LocMsgDestroy);
    mPendingMsgCount++;
    if (mPendingMsgCount > 10) {
        LOC_LOGE2("Large pending msg count %d on MsgTask %p", mPendingMsgCount, msg->mMsgTaskPtr);
    }
}

bool MsgTask::run() {
    LOC_LOGL0("MsgTask::loop() listening ...");
    LocMsg* msg;
    msq_q_err_type result = msg_q_rcv((void*)mQ, (void **)&msg);
    if (eMSG_Q_SUCCESS != result) {
        LOC_LOGSH1("fail receiving msg: %s",
                      loc_get_msg_q_status(result));
        return false;
    }
    LOC_LOGL2("Received LocMsg %p on MsgTask %p", msg, msg->mMsgTaskPtr);
    if (msg->mMsgTaskPtr != this) {
        LOC_LOGE0("Invalid MsgTask, memory corruption !!");
        return false;
    }

    msg->log();
    // there is where each individual msg handling is invoked
    msg->proc();

    delete msg;
    mPendingMsgCount--;

    return true;
}

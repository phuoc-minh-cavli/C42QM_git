/* Copyright (c) 2011-2014, 2016, The Linux Foundation. All rights reserved.
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
#define LOG_TAG "LocSvc_DualCtx"

#include <LocDualContext.h>
#include <msg_q.h>
#include <log_util.h>
#include <loc_log.h>

namespace izat_core {

// nothing exclude for foreground
const LOC_API_ADAPTER_EVENT_MASK_T
LocDualContext::mFgExclMask = 0;
// excluded events for background clients
const LOC_API_ADAPTER_EVENT_MASK_T
LocDualContext::mBgExclMask =
    (LOC_API_ADAPTER_BIT_PARSED_POSITION_REPORT |
     LOC_API_ADAPTER_BIT_SATELLITE_REPORT |
     LOC_API_ADAPTER_BIT_NMEA_1HZ_REPORT |
     LOC_API_ADAPTER_BIT_NMEA_POSITION_REPORT |
     LOC_API_ADAPTER_BIT_IOCTL_REPORT |
     LOC_API_ADAPTER_BIT_STATUS_REPORT |
     LOC_API_ADAPTER_BIT_GEOFENCE_GEN_ALERT |
     LOC_API_ADAPTER_BIT_GNSS_MEASUREMENT);

MsgTask* LocDualContext::mMsgTask = NULL;
ContextBase* LocDualContext::mFgContext = NULL;
ContextBase* LocDualContext::mBgContext = NULL;
ContextBase* LocDualContext::mInjectContext = NULL;
// the name must be shorter than 15 chars
const char* LocDualContext::mLocationHalName = "LocHalWorker";
const char* LocDualContext::mLBSLibName = "liblbs_core.so";

//pthread_mutex_t LocDualContext::mGetLocContextMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t LocDualContext::mGetLocContextMutex;

MsgTask* LocDualContext::getMsgTask(LocThread::tCreate tCreator,
                                          const char* name, bool joinable)
{
    if (NULL == mMsgTask) {
        LOC_LOGL0("Calling MsgTask::MsgTask");
        mMsgTask = new MsgTask(tCreator, name, joinable);
    }
    return mMsgTask;
}

inline
const MsgTask* LocDualContext::getMsgTask(const char* name, bool joinable) {
    return getMsgTask((LocThread::tCreate)NULL, name, joinable);
}

ContextBase* LocDualContext::getLocFgContext(LocThread::tCreate tCreator,
            LocMsg* firstMsg, const char* name, bool joinable)
{
    LOC_LOGL0("LocDualContext::getLocFgContext");

    pthread_mutex_lock(&LocDualContext::mGetLocContextMutex);
    LOC_LOGL0("querying ContextBase with tCreator");
    if (NULL == mFgContext) {
        LOC_LOGL0("creating msgTask with tCreator");
        MsgTask* msgTask = getMsgTask(tCreator, name, joinable);
        mFgContext = new LocDualContext(msgTask,
                                        mFgExclMask);
    }
    if(NULL == mInjectContext) {
        LOC_LOGL0("mInjectContext is FgContext");
        mInjectContext = mFgContext;
        //injectFeatureConfig(mInjectContext);
    }

    pthread_mutex_unlock(&LocDualContext::mGetLocContextMutex);

    if (firstMsg) {
        mFgContext->sendMsg(firstMsg);
    }
    return mFgContext;
}

ContextBase* LocDualContext::getLocBgContext(LocThread::tCreate tCreator,
            LocMsg* firstMsg, const char* name, bool joinable)
{
    pthread_mutex_lock(&LocDualContext::mGetLocContextMutex);
    LOC_LOGL0("querying ContextBase with tCreator");
    if (NULL == mBgContext) {
        LOC_LOGL0("creating msgTask with tCreator");
        MsgTask* msgTask = getMsgTask(tCreator, name, joinable);
        mBgContext = new LocDualContext(msgTask,
                                        mBgExclMask);
    }
    if(NULL == mInjectContext) {
        LOC_LOGL0("mInjectContext is BgContext");
        mInjectContext = mBgContext;
        //injectFeatureConfig(mInjectContext);
    }
    pthread_mutex_unlock(&LocDualContext::mGetLocContextMutex);

    if (firstMsg) {
        mBgContext->sendMsg(firstMsg);
    }

    return mBgContext;
}

LocDualContext::LocDualContext(MsgTask* msgTask,
                               LOC_API_ADAPTER_EVENT_MASK_T exMask) :
    ContextBase(msgTask, exMask, mLBSLibName)
{
    LOC_LOGM0("LocDualContext constructor");
    pthread_mutex_init(&LocDualContext::mGetLocContextMutex, NULL);
}

}

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
#ifndef LOC_API_ADAPTER_BASE_H
#define LOC_API_ADAPTER_BASE_H

#include <gps_extended.h>
#include <ContextBase.h>

namespace loc_core {

class ContextBase;
class LocApiBase;

class LocAdapterBase {

protected:

    LOC_API_ADAPTER_EVENT_MASK_T mEvtMask;
    ContextBase* mContext;
    LocApiBase* mLocApi;
    const MsgTask* mMsgTask;

    inline LocAdapterBase(const MsgTask* msgTask) :
        mEvtMask(0),
        mContext(NULL),
        mLocApi(NULL),
        mMsgTask(msgTask) {}

public:
    virtual ~LocAdapterBase();
    LocAdapterBase(const LOC_API_ADAPTER_EVENT_MASK_T mask,
                   ContextBase* context);

    inline LOC_API_ADAPTER_EVENT_MASK_T checkMask(LOC_API_ADAPTER_EVENT_MASK_T mask) const {
        return mEvtMask & mask;
    }

    inline LOC_API_ADAPTER_EVENT_MASK_T getEvtMask() const {
        return mEvtMask;
    }

    ContextBase* getContext() const {
        return mContext;
    }

    inline void sendMsg(const LocMsg* msg) const {
        mMsgTask->sendMsg(msg);
    }

    inline void sendMsg(const LocMsg* msg) {
        mMsgTask->sendMsg(msg);
    }

    inline virtual bool isInSession() {
        return false;
    }

    virtual void handleEngineUpEvent();
    virtual void handleEngineDownEvent();
};

} // namespace loc_core

#endif //LOC_API_ADAPTER_BASE_H

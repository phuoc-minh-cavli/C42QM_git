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

#include <LocTimer.h>
#include <loc_util_log.h>
#include <timer.h>


// Define static members
MsgTask* LocTimer::mMsgTask = NULL;

// Common timer unit for all timers
timer_unit_type  LOC_TIMER_UNIT = T_MSEC;

// Native timer callback
void loc_timer_callback(timer_cb_data_type data){

    LOC_LOGM1("loc_timer_callback(): data %p", (void*)data);

    LocTimer* locTimer = static_cast<LocTimer*>((void*)data);

    // Send message to switch to Loc context
    struct MsgTimerExpire : public LocMsg {

        LocTimer* mLocTimer;
        inline MsgTimerExpire(LocTimer* locTimer) :
            LocMsg(), mLocTimer(locTimer) {}
        inline virtual void proc() const {

            // within loc msg processing, invoke
            // LocTimer client callback
            mLocTimer->timeOutCallback();
        }
    };

    LocTimer::mMsgTask->sendMsg(new MsgTimerExpire(locTimer));
    LOC_LOGM0("loc_timer_callback(): Sent MsgTimerExpire to LocTimer");
}

LocTimer::LocTimer() {

    // Define the msg task
    if (mMsgTask == NULL) {

        mMsgTask = new MsgTask("LocTimerMsgTask", false);
    }

    // Timer handle for easier access
    mTimerHandle = &mTimer;

    // Define the native timer
    timer_error_type ret =
        timer_def_osal(
            mTimerHandle,                // native timer handle
            NULL,                        // timer group
            TIMER_FUNC1_CB_TYPE,         // CB FN with 1 argument
            (void*)loc_timer_callback,          // CB FN
            (timer_cb_data_type)this);   // CB Data; instance handle
    if(ret != TE_SUCCESS){

        LOC_LOGE1("LocTimer(): timer_def_osal() failed, err %d", ret);
    }

    LOC_LOGM3(
        "LocTimer() instance created %p, mMsgTask %p, mTimerHandle %p",
        this, mMsgTask, mTimerHandle);
}

LocTimer::~LocTimer() {

    LOC_LOGM2("LocTimer()::~LocTimer() instance %p mTimerHandle %p",
                this, mTimerHandle);

    // Clear the native timer
    timer_clr(mTimerHandle, LOC_TIMER_UNIT);
}

bool LocTimer::start(unsigned int timeOutInMs) {

    // Check if already running
    LOC_LOGH1("Timer Handle %d",mTimerHandle);
    if(timer_is_active(mTimerHandle)){

        LOC_LOGE1("LocTimer::start(): timer %p already active !",
                    mTimerHandle);
        return false;
    }

    // Set the native timer
    timer_set(
            mTimerHandle,     // Native timer handle
            timeOutInMs,      // Time to first expiry
            timeOutInMs,      // Recurring time period
            LOC_TIMER_UNIT);  // Unit : millis

    LOC_LOGM2("LocTimer::start(): timer %p set for %d millis, recurring.",
                mTimerHandle, timeOutInMs);
    return true;
}

bool LocTimer::stop() {

    LOC_LOGM2("LocTimer()::stop() instance %p mTimerHandle %p",
                this, mTimerHandle);

    timer_clr(mTimerHandle, LOC_TIMER_UNIT);
    return true;
}

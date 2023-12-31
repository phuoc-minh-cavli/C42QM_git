/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
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

#ifndef __LOC_TIMER_CPP_H__
#define __LOC_TIMER_CPP_H__

#include <stddef.h>
#include <platform_lib_includes.h>
#include <timer.h>
#include <MsgTask.h>

// LocTimer client must extend this class and implementthe callback.
// start() / stop() methods are to arm / disarm timer.
class LocTimer
{
    // Native timer to be used
    struct timer_struct mTimer;
    timer_ptr_type mTimerHandle;

public:
    // Msg task to provider msg Q, sender and reader.
    static MsgTask* mMsgTask;

    LocTimer();
    virtual ~LocTimer();

    // timeOutInMs:  timeout delay in ms
    // wakeOnExpire: we don't wake up the CPU on timer expiry
    //               instead, timer callback is invoked whenever the
    //               CPU wakes up.
    // return:       true on success;
    //               false on failure, e.g. timer is already running.
    bool start(uint32_t timeOutInMs);

    // return:       true on success;
    //               false on failure, e.g. timer is not running.
    bool stop();

    //  LocTimer client Should implement this method.
    //  This method is used for timeout calling back to client. This method
    //  should be short enough (eg: send a message to your own thread).
    virtual void timeOutCallback() = 0;
};

#endif //__LOC_TIMER_CPP_H__

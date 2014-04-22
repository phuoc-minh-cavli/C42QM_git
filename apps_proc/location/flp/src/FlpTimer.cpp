/******************************************************************************
    Copyright (c) 2017 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#include <loc_util_log.h>
#include <FlpTimer.h>
#include <FlpLocationAdapter.h>

FlpTimer::FlpTimer(FlpLocationAdapter* flpAdapter, FlpTimerType timerType) :
        LocTimer(), mWaitTimeInMs(0), mStarted(false), mIsRecurring(false),
        mFlpAdapter(flpAdapter), mTimerType(timerType) {

    LOC_LOGM2("FlpSingleShotTimer created - adapter %p type %d",
              mFlpAdapter, mTimerType);
}

inline FlpTimer::~FlpTimer() {

    stop();
}

void FlpTimer::set(uint32_t waitTimeSec) {

    mWaitTimeInMs = waitTimeSec * 1000;
}

void FlpTimer::start() {

    LOC_LOGM1("SS timer start: %u millis", mWaitTimeInMs);

    if (mWaitTimeInMs == 0) {
        LOC_LOGE0("Invalid SS time wait duration.");
        return;
    }
    mStarted = true;
    LocTimer::start(mWaitTimeInMs);
}

void FlpTimer::setRecurring(bool isRecurring) {

    mIsRecurring = isRecurring;
}

void FlpTimer::stop() {

    LOC_LOGL0("SS timer stop");

    if (!mStarted) {
        LOC_LOGE0("SS timer not started");
        return;
    }
    LocTimer::stop();
    mStarted = false;
}

void FlpTimer::timeOutCallback() {

    LOC_LOGL0("SS timer timeout");

    if (!mIsRecurring) {
        LocTimer::stop();
        mStarted = false;
    }
    if (mFlpAdapter == NULL) {
        LOC_LOGE0("Flp Adapter null !");
        return;
    }

    struct MsgFlpSingleShotCb : public LocMsg {
        FlpLocationAdapter* mAdapter;
        FlpTimerType mTimerType;
        inline MsgFlpSingleShotCb(FlpLocationAdapter* adapter,
                                  FlpTimerType timerType) :
            LocMsg(),
            mAdapter(adapter),
            mTimerType(timerType) {}
        inline virtual ~MsgFlpSingleShotCb(){}
        inline virtual void proc() const {
            if (FLP_TIMER_TYPE_SINGLESHOT == mTimerType) {
                mAdapter->singleShotTimerCallback();
            } else {
                LOC_LOGE1("Invalid flp_timer_type %d", mTimerType);
            }
        }
    };

    mFlpAdapter->sendMsg(new MsgFlpSingleShotCb(mFlpAdapter, mTimerType));
}

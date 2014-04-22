/******************************************************************************
    Copyright (c) 2017 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#ifndef __FLP_SINGLE_SHOT_TIMER_H__
#define __FLP_SINGLE_SHOT_TIMER_H__

#include <LocTimer.h>

using namespace std;

class FlpLocationAdapter;

typedef enum {
    FLP_TIMER_TYPE_INVALID      = 0,
    FLP_TIMER_TYPE_SINGLESHOT   = 1,
} FlpTimerType;

class FlpTimer : public LocTimer {

    uint32_t mWaitTimeInMs;
    bool mStarted;
    bool mIsRecurring;
    FlpLocationAdapter* mFlpAdapter;
    FlpTimerType mTimerType;

public:
    FlpTimer(FlpLocationAdapter* flpAdapter, FlpTimerType timerType);
    virtual ~FlpTimer();

    void set(uint32_t waitTimeSec);
    void start();
    void setRecurring(bool isRecurring);
    inline void restart() { stop(); start(); }
    void stop();
    inline bool isStarted() { return mStarted; }

private:
    virtual void timeOutCallback();
};

#endif // __FLP_SINGLE_SHOT_TIMER_H__

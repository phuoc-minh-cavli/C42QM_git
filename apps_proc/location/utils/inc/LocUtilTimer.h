/******************************************************************************
    Copyright (c) 2019 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#ifndef __LOC_UTIL_TIMER_H__
#define __LOC_UTIL_TIMER_H__

#include <string>
#include <LocTimer.h>
#include <loc_misc_utils.h>

using namespace std;

class LocUtilTimer : public LocTimer {
    string mName;
    time_t mWaitTimeInMs;
    Runnable mRunnable;
    void* mClientDataPtr;
    bool mStarted;

public:
    LocUtilTimer(const string& name);
    virtual ~LocUtilTimer();

    void set(const time_t waitTimeSec, const Runnable& runnable, const void* clientDataPtr);

    void start();
    void start(const time_t waitTimeSec);
    inline void restart() { stop(); start(); }
    inline void restart(const time_t waitTimeSec) { stop(); start(waitTimeSec); }
    void stop();

    inline bool isStarted() {
        return mStarted;
    }

private:
    virtual void timeOutCallback();
};

#endif // __LOC_UTIL_TIMER_H__

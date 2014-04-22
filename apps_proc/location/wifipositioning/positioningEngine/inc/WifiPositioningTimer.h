/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#ifndef __WIFI_POSITIONING_TIMER_H__
#define __WIFI_POSITIONING_TIMER_H__

#include <string>
#include <LocTimer.h>
#include <loc_misc_utils.h>

using namespace std;

namespace izat_wifi_positioning {

class WifiPositioningIzatAdapter;
class WifiPositioningEngine;

class WifiPositioningTimer : public LocTimer {
    string mName;
    time_t mWaitTimeInMs;
    Runnable mRunnable;
    void* mClientDataPtr;
    bool mStarted;

public:
    WifiPositioningTimer(const string& name);
    virtual ~WifiPositioningTimer();

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
    //virtual void timeOutCallback() override // c++11 only;
    virtual void timeOutCallback();
};

} // end of namespace

#endif //__WIFI_POSITIONING_TIMER_H__

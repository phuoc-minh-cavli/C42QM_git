/******************************************************************************
    Copyright (c) 2017 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#ifndef __LOCATION_WIFI_INTERFACE_TIMER_H__
#define __LOCATION_WIFI_INTERFACE_TIMER_H__

#include <LocTimer.h>
#include <string>
#include <loc_misc_utils.h>
using namespace std;

namespace izat_location_wifi_interface {

class LocationWifiInterface;

class LocationWifiInterfaceTimer : public LocTimer {

    string mName;
    time_t mWaitTimeInMs;
    Runnable mRunnable;
    void* mClientDataPtr;
    bool mStarted;

public:
    LocationWifiInterfaceTimer(const string& name);
    virtual ~LocationWifiInterfaceTimer();

    void set(const time_t waitTimeSec, const Runnable& runnable, const void* clientDataPtr);
    void start();
    void start(const time_t waitTimeSec);
    inline void restart() { stop(); start(); }
    inline void restart(const time_t waitTimeSec) { stop(); start(waitTimeSec); }
    void stop();
    inline bool isStarted() { return mStarted; }

private:
    virtual void timeOutCallback();
};

}// end namespace

#endif // __LOCATION_WIFI_INTERFACE_TIMER_H__

/******************************************************************************
    Copyright (c) 2017 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#include <loc_util_log.h>
#include <LocationWifiInterfaceTimer.h>
#include <LocationWifiInterface.h>

namespace izat_location_wifi_interface {

LocationWifiInterfaceTimer::LocationWifiInterfaceTimer(const string& name) : LocTimer(), mName(name), mStarted(false)
{
  LOC_LOGSM1("Timer %s created.", mName.c_str());
}

inline LocationWifiInterfaceTimer::~LocationWifiInterfaceTimer() {

    stop();
}

void LocationWifiInterfaceTimer::set(const time_t waitTimeSec, const Runnable& runnable, const void* clientDataPtr) {

    mWaitTimeInMs = waitTimeSec * 1000;
    mRunnable = runnable;
    mClientDataPtr = clientDataPtr;
}

void LocationWifiInterfaceTimer::start() {

    LOC_LOGSM2("Start timer %s, waiting %ld ms...", mName.c_str(), mWaitTimeInMs);

    mStarted = true;
    LocTimer::start(mWaitTimeInMs);
}

void LocationWifiInterfaceTimer::stop() {

    if (!mStarted) {
        LOC_LOGSE1("Timer [%s] not started yet", mName.c_str());
        return;
    }

    LOC_LOGSM1("Stop timer %s ", mName.c_str());

    LocTimer::stop();
    mStarted = false;
}

void LocationWifiInterfaceTimer::timeOutCallback() {

    LOC_LOGSM1("timer %s timeout", mName.c_str());

    LocTimer::stop();
    mStarted = false;
    mRunnable(mClientDataPtr);
}


} // end namespace

/******************************************************************************
    Copyright (c) 2019 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#include <platform_lib_log_util.h>
#include "LocUtilTimer.h"

LocUtilTimer::LocUtilTimer(const string& name) : LocTimer(), mName(name), mStarted(false) {

    LOC_LOGSM1("LocUtilTimer %s created.", mName.c_str());
}

inline LocUtilTimer::~LocUtilTimer() {
    stop();
}

void LocUtilTimer::set(const time_t waitTimeSec, const Runnable& runnable, const void* clientDataPtr)
{
    mWaitTimeInMs = waitTimeSec * 1000;
    mRunnable = runnable;
    mClientDataPtr = clientDataPtr;
}

void LocUtilTimer::start()
{
    LOC_LOGSM2("start timer %s, waiting %ld ms...", mName.c_str(), mWaitTimeInMs);

    mStarted = true;
    LocTimer::start(mWaitTimeInMs);
}

void LocUtilTimer::start(const time_t waitTimeSec)
{
    mWaitTimeInMs = waitTimeSec * 1000;
    start();
}

void LocUtilTimer::stop()
{
    if (!mStarted) {
        LOC_LOGSE1("timer [%s] not started yet", mName.c_str());
        return;
    }

    LOC_LOGSM1("stop timer %s ", mName.c_str());

    LocTimer::stop();
    mStarted = false;
}

void LocUtilTimer::timeOutCallback() {

    LOC_LOGSM1("timer %s timeout", mName.c_str());

    LocTimer::stop();
    mStarted = false;
    mRunnable(mClientDataPtr);
}

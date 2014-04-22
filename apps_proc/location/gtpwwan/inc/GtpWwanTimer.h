/******************************************************************************
Copyright (c) 2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef __GTPWWAN_TIMER_H__
#define __GTPWWAN_TIMER_H__

#include <string>
#include <LocTimer.h>
#include <loc_misc_utils.h>

using namespace std;

namespace izat_gtpwwan {

  class GtpWwanClient;

  class GtpWwanTimer : public LocTimer {
    string mName;
    time_t mWaitTimeInMs;
    Runnable mRunnable;
    void* mClientDataPtr;
    bool mStarted;

    public:
    GtpWwanTimer(const string& name);
    virtual ~GtpWwanTimer();

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

#endif // __XTRA_TIMER_H__

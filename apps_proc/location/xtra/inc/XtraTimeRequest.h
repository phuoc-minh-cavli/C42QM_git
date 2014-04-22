/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#ifndef __XTRA_TIME_REQUEST__
#define __XTRA_TIME_REQUEST__

namespace izat_xtra {

class XtraClient;

typedef struct {
    uint64_t utcTime;
    uint64_t uncertainty;
    uint64_t reference;
} SntpTime;

class XtraTimeRequest {
public:
    XtraTimeRequest(XtraClient& client);
    ~XtraTimeRequest();

    void handleXtraTimeRequest();

    uint64_t getCacheAge();

    void onGpsTimeReceived(uint64_t gpsTime);
    bool calcUtcTimeMs(uint64_t& utcMs, uint64_t& refMs);

private:
    XtraClient& mXtraClient;

    SntpTime mCachedNtpTime;
    uint64_t mRefreshTimeSec;

    uint64_t mLatestGpsTimeMs;
    uint64_t mLatestGpsTimeRefMs;
};

} // end of namespace

#endif //__XTRA_TIME_REQUEST__

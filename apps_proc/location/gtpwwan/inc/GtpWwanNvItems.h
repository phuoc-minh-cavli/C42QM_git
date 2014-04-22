/******************************************************************************
Copyright (c) 2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#ifndef __GTP_WWAN_NV_ITEMS_H___
#define __GTP_WWAN_NV_ITEMS_H___

#include <comdef.h>
#include <stdint.h>
#include <time.h>
#include <string>

using namespace std;

namespace izat_gtpwwan {

#define INVALID_TIME (time_t)(-1)

class GtpWwanNvItems {
    public:
        GtpWwanNvItems();
        ~GtpWwanNvItems();
        void setRetryAfterTime(const uint64_t time);
        uint64_t getRetryAfterTime();
        void setRetryNumber(const uint64_t retrynumber);
        uint64_t getRetryNumber();
        uint64_t getBlackListTime();

        void setBlackListTime(const uint64_t time);

    private:
        void open();
        void close();
        void saveNvItem(const string& name, const string& value);
        string readNvItem(const string& name);

};

} // end of namespace

#endif // __GTP_WWAN_NV_ITEMS_H___

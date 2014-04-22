/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#ifndef __XTRA_NV_ITEMS_H___
#define __XTRA_NV_ITEMS_H___

#include <comdef.h>
#include <stdint.h>
#include <time.h>
#include <string>

using namespace std;

namespace izat_xtra {

#define INVALID_TIME (time_t)(-1)

enum XTRA_DOWNLOAD_TYPE {
    XTRA_DATA = 0,
    XTRA_INTEGRITY
};

class XtraNvItems {
public:
    XtraNvItems();
    ~XtraNvItems();

    // persist latest XTRA download time
    void saveLatestXtraDownloadTime(const time_t time);
    time_t getLatestXtraDownloadTime();

    // persist latest XTRA integrity download time
    void saveLatestXtraIntDownloadTime(const time_t time);
    time_t getLatestXtraIntDownloadTime();

    // persist image versions
    void saveUploadedSocVersions(const string& versions);
    string getUploadedSocVersions();

    // modem boot/reboot time
    void setModemBootTime(const int64_t time);
    int64_t getModemBootTime();

    // recent activity
    int getXtraRequestRateForToday(XTRA_DOWNLOAD_TYPE type);
    void incrementXtraRequestRateForToday(XTRA_DOWNLOAD_TYPE type);

    int getXtraRequestRetryForToday(const string& server, XTRA_DOWNLOAD_TYPE type);
    void incrementXtraRequestRetryForToday(const string& server, XTRA_DOWNLOAD_TYPE type);

    uint32_t getXtraIntSuccessfulDloadCount();
    void incrementXtraIntSuccessfulDloadCount();
    void resetXtraIntSuccessfulDloadCount();

private:
    void open();
    void close();
    void saveNvItem(const string& name, const string& value);
    string readNvItem(const string& name);
    int getCounterForToday(const string& name);
    void incrementCounterForToday(const string& name);

};

} // end of namespace

#endif // __XTRA_NV_ITEMS_H__

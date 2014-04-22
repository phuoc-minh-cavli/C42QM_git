/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#ifndef __XTRA_DATA_REQUEST__
#define __XTRA_DATA_REQUEST__
#include <string>
#include "XtraUserAgent.h"
#include <loc_misc_utils.h>

enum XTRA_DOWNLOAD_STATUS {
    XTRA_DOWNLOAD_UNKNOWN = -1,
    XTRA_DOWNLOAD_SUCCESS = 0,
    XTRA_DOWNLOAD_FAIL_URL,
    XTRA_DOWNLOAD_FAIL_HOST,
    XTRA_DOWNLOAD_FAIL_SOCKET,
    XTRA_DOWNLOAD_FAIL_CONNECTION,
    XTRA_DOWNLOAD_FAIL_REQUEST,
    XTRA_DOWNLOAD_FAIL_RESPONSE,
    XTRA_DOWNLOAD_FAIL_SKIPPED,
    XTRA_DOWNLOAD_FAIL_INSUFFICIENT_MEMORY,
    XTRA_DOWNLOAD_FAIL_SSL,
    XTRA_DOWNLOAD_FAIL_REDIRECT,
    XTRA_DOWNLOAD_REDIRECT_RESPONSE,
    XTRA_DOWNLOAD_4XX_RESPONSE,
    XTRA_DOWNLOAD_5XX_RESPONSE,
    XTRA_DOWNLOAD_FAIL_DISABLED
};

enum XTRA_REGION_SPECIFIC_PROTOCOL {
    XTRA_REGION_SPECIFIC_PROTOCOL_UNSPECIFIED = 0,
    XTRA_REGION_SPECIFIC_PROTOCOL_HTTP,
    XTRA_REGION_SPECIFIC_PROTOCOL_HTTPS,
};

using namespace std;

namespace izat_xtra {

class XtraClient;
class XtraConfig;

class XtraDataRequest {
public:
    XtraDataRequest(XtraClient& client);
    ~XtraDataRequest();

    void handleXtraDataRequest(XTRA_DOWNLOAD_TYPE type);

private:
    XtraClient& mXtraClient;
    XtraUserAgent mXtraUserAgent;

    XTRA_DOWNLOAD_STATUS doDownloadXtraData(
            const string& url, string& buffer, XTRA_DOWNLOAD_TYPE type,
            XTRA_REGION_SPECIFIC_PROTOCOL xtraIntProto = XTRA_REGION_SPECIFIC_PROTOCOL_UNSPECIFIED);
    XTRA_DOWNLOAD_STATUS doDownloadXtraDataOnce(
            const string& url, string& result, XTRA_DOWNLOAD_TYPE type,
            XTRA_REGION_SPECIFIC_PROTOCOL xtraIntProto = XTRA_REGION_SPECIFIC_PROTOCOL_UNSPECIFIED);

    void updateXtraFileNameForDuration(std::string& fileName);
    XTRA_REGION_SPECIFIC_PROTOCOL getXtraRegionSpecificProtocol(const std::string& path);
};

} // end of namespace

#endif //__XTRA_DATA_REQUEST__

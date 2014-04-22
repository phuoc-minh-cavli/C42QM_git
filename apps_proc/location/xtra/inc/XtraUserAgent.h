/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#ifndef __XTRA_USER_AGENT__
#define __XTRA_USER_AGENT__

#include <string>
//#include <unordered_map> // c++11 only
#include <map>
#include <XtraPlatform.h>

using namespace std;

namespace izat_xtra {

class XtraClient;
class XtraDataRequest;

class XtraUserAgent {
public:
    XtraUserAgent(XtraDataRequest& xtraDataReq, XtraClient& client);
    ~XtraUserAgent();

    string getUaString(string& versions) const;

private:
    XtraClient& mXtraClient;
    XtraDataRequest& mXtraDataRequest;

    XtraPlatform mXtraPlatform;

    void createLocXtraFolderIfNot() const;
    void saveUploadedSocVersions(const string& socVersions) const;
    string getSocVersions(const string& def) const;
    string getSocVersionsForUpload(const string& def) const;
    string getChipsetSerialNo(const string& def) const;

    // separator escaping
    //static const unordered_map<char, string> sEncodings; // c++11 only
    static map<char, string> initEncodingsMap();
    static const map<char, string> sEncodings;
    string encodeSeparators(const string& str) const;
};

} // end of namespace

#endif // __XTRA_USER_AGENT__

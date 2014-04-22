/******************************************************************************
    Copyright (c) 2016 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/

#ifndef __XTRA_IZAT_ADAPTER__
#define __XTRA_IZAT_ADAPTER__

#include <IzatAdapterBase.h>
#include <IzatApiBase.h>
#include <vector>
#include <string>
#include "LocationAPI.h"

using namespace std;
using namespace izat_core;

namespace izat_xtra {

class XtraClient;

class XtraIzatAdapter : public IzatAdapterBase {
public:
    XtraIzatAdapter(XtraClient& client);
    virtual ~XtraIzatAdapter();

    // Xtra version check
    void onSetXtraVersionCheck();

    // Xtra servers
    void onRequestXtraServersAndData(uint32_t reqSource);
    virtual bool onReceiveXtraServers(const char* server1, const char* server2, const char* server3);

    // XTRA data request
    virtual bool onRequestXtraData();
    void onInjectXtraData(const string& data);
    void onInjectXtraIntegrity(const string& data);

    // Xtra time requests
    virtual bool onRequestNtpTime();
    void onInjectNtpTime(const int64_t time, const int64_t reference, const int64_t uncertainty);

    // SSR
    virtual void handleEngineUpEvent();

    // Xtra config
    void requestXtraConfigInfo(uint32_t configReqMask, uint32_t configReqSource);
    virtual bool onReceiveXtraConfig(uint32_t status, uint32_t configReqSource,
                                     uint32_t report_mask, uint16_t absAge,
                                     uint64_t relAge, uint8_t timereqValid,
                                     uint16_t prefValidAgeHrs,
                                     const char* ntpServer1, const char* ntpServer2,
                                     const char* ntpServer3,
                                     const char* xtraServer1, const char* xtraServer2,
                                     const char* xtraServer3);

    // GPS time
    virtual bool reportLocation(const Location& location,
            const GpsLocationExtended& locationExtended, enum loc_sess_status status);

    // Integrity
    bool isMpIntegritySupported()
    {
        mMpIntegritySupported = true;
        LOC_LOGM1("isIntegritySupported: %d", mMpIntegritySupported);
        return mMpIntegritySupported;
    };

    /* Meta data commands */
    bool getBackoffTime(LocationMetaData& metaData);

    void forwardMetaData(const LocationMetaData& metaData);
    int xtraGetGnssTime(GnssTime& time);
    static int xtraGetGnssTimeCb(void *data, GnssTime& time);
    uint32_t requestNtpTimeCommand(LocationAPI* client);
    void reportResponse(
        LocationAPI* client, LocationError err, uint32_t sessionId);
    void saveClient(
            LocationAPI* client, const LocationCallbacks& callbacks);
    void eraseClient(LocationAPI* client);
    void addClientCommand(
            LocationAPI* client, const LocationCallbacks& callbacks);
    void removeClientCommand(LocationAPI* client);

private:
    XtraClient& mXtraClient;
    bool mMpIntegritySupported;
    string mTimeUncSecConfigItem;
    typedef std::map<LocationAPI*, LocationClientInfo> ClientDataMap;
    ClientDataMap mClientData;

    // Below static methods are callbacks : lambda can't be used : c++11 only
    static void sendMsgCbSetXtraVersionCheck(void* xtraIzatAdapterPtr);
    static void sendMsgCbInjectXtraData(void* cbDataPtr);
    static void sendMsgCbInjectNtpTime(void* cbDataPtr);
};

} // end of namespace

#endif //__XTRA_IZAT_ADAPTER__

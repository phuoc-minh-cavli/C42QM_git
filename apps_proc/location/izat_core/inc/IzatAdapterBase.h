/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015, 2018-20 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef IZAT_ADAPTER_BASE_H
#define IZAT_ADAPTER_BASE_H

#include <stdbool.h>
#include <string.h>
#include "MsgTask.h"
#include <ContextBase.h>
#include <geofence.h>
#include <LocationAPI.h>
#include <LocUtilTimer.h>
#include <map>
#include <vector>

/* Session Key used to identify each session uniquely */
typedef struct LocationSessionKey {
    LocationAPI* client;
    uint32_t id;
    inline LocationSessionKey(LocationAPI* _client, uint32_t _id) :
        client(_client), id(_id) {}
} LocationSessionKey;
inline bool operator <(LocationSessionKey const& left, LocationSessionKey const& right) {
    return left.id < right.id || (left.id == right.id && left.client < right.client);
}
inline bool operator ==(LocationSessionKey const& left, LocationSessionKey const& right) {
    return left.id == right.id && left.client == right.client;
}
inline bool operator !=(LocationSessionKey const& left, LocationSessionKey const& right) {
    return left.id != right.id || left.client != right.client;
}

typedef struct ExtendedLocationOptions {
    LocationOptions locationOptions;
    bool            isGnssDataRequested;
} ExtendedLocationOptions;

typedef std::map<LocationSessionKey, ExtendedLocationOptions> LocationSessionMap;
typedef std::map<LocationSessionKey, LocationMotionOptions> MotionSessionMap;

/* Location client info */
typedef struct
{
    LocationCallbacks callbacks;
    bool isEngineLoadSuccessReported;
    bool isEngineLoadFailureReported;
    uint64 firstWeakSvTimestamp;
    uint64 lastWeakSvTimestamp;

} LocationClientInfo;

namespace izat_core {

class IzatApiBase;
class ContextBase;

class IzatAdapterBase {
protected:
    /* Accessed from client threads.
     * Protected via LocationAPI mutex. */
    static uint32_t mSessionIdCounter;
    
    LOC_API_ADAPTER_EVENT_MASK_T mEvtMask;
    ContextBase* mContext;
    IzatApiBase* mIzatApi;
    const MsgTask* mMsgTask;

    std::vector<LocMsg*> mPendingMsgs;

    /* Timer for storing pending messages while waiting for XTRA Integrity download */
    LocUtilTimer mPendingMsgTimer;

    uint32_t mXtraIntDloadWaitTime;
    uint32_t mXtraIntDloadRetries;
    uint32_t mXtraIntDloadRetryLimit;

    IzatAdapterBase(const LOC_API_ADAPTER_EVENT_MASK_T mask,
                    ContextBase* context,
                    uint32_t xtraIntDloadWaitTime = 0,
                    uint32_t xtraIntDloadRetryLimit = 0);
    virtual ~IzatAdapterBase();

    uint32_t generateSessionId();

public:

    inline void sendMsg(const LocMsg* msg) const {
        mMsgTask->sendMsg(msg);
    }
    inline void sendMsg(const LocMsg* msg) {
        mMsgTask->sendMsg(msg);
    }
    inline void sendPendingMsg(LocMsg* msg) {
        msg->mIsPendingMsg = true;
        mPendingMsgs.push_back(msg);
    }
    inline void clearPendingMsgs() {
        for(std::vector<LocMsg*>::iterator it = mPendingMsgs.begin();
                it != mPendingMsgs.end(); ++it) {
            LocMsg* msg = *it;
            sendMsg(msg);
        }
        mPendingMsgs.clear();
    }
    inline LOC_API_ADAPTER_EVENT_MASK_T getEvtMask() const {
        return mEvtMask;
    }

    virtual bool reportLocations(Location* location,
                                 size_t count);
    virtual bool reportLocation(const Location& location,
            const GpsLocationExtended& locationExtended, enum loc_sess_status status);
    virtual bool reportWwanZppFix(const Location& location, LocationError err, bool isBestAvailPosition=false);
	#ifdef FEATURE_LOCATION_MCS
    virtual bool reportWwanScanList(void* ind);
	virtual bool handleReportfdclList(void* ind);
	#endif
    virtual bool reportWifiFix(const Location& location, LocationError err);

    virtual bool geofenceBreach(uint32_t id,
                                Location location,
                                GeofenceBreachType breachTypeMask,
                                uint64_t timestamp);
    virtual bool geofenceAlert(GeofenceAlertType alert);

    virtual bool reportNmea(const char* nmea, int length, NmeaSource source);

    virtual bool reportSv(GnssSvNotification& svInfo);

    virtual void handleEngineUpEvent();
    virtual void handleEngineDownEvent();

    // Meta data reporting
    virtual bool reportMetaData(const LocationMetaData& metaData, LocationAPI* client = NULL);

    // Report outgoing requests to GNSS engine to all adapters
    virtual void reportRequestStartTracking(
            const LocationOptions& options, bool isSingleShot, LocationAccuracyLevel accuracy);
    virtual void reportRequestStopTracking();
    virtual void reportRequestStartBatching(
            const LocationOptions& options, LocationAccuracyLevel accuracy);
    virtual void reportRequestStopBatching();
    virtual void reportRequestWwanFix();

    // Requests meant to be handled by one of the adapters
    virtual bool requestAdapterGetSingleshot(
            const SingleShotOptions& options, SingleshotRequestType requestType,
            IzatAdapterBase* requestorAdapter);

    // XTRA Client 2.0
    virtual bool onReceiveXtraServers(const char* server1, const char* server2, const char* server3);
    virtual bool onRequestXtraData();
    virtual bool onRequestNtpTime();
    /*========================================================================
      FUNCTION	onRequestBsObservedData

      DESCRIPTION 
      this function is invoked when we get fdcl event from TLE 

      PARAMETERS
      clientinfo - (IN) point to struct which contain client inforamtion which we get from TLE

      RETURN VALUE
      FALSE - this always return false if this function is called
      ==========================================================================*/	

    virtual bool onRequestBsObservedData(clientinformation* clientinfo);
    virtual bool onReceiveXtraConfig(uint32_t status, uint32_t configReqSource,
                                     uint32_t report_mask,
                                     uint16_t absAge, uint64_t relAge,
                                     uint8_t timereqValid, uint16_t prefValidAgeHrs,
                                     const char* ntpServer1, const char* ntpServer2,
                                     const char* ntpServer3, const char* xtraServer1,
                                     const char* xtraServer2, const char* xtraServer3);

    /* XTRA INT dload logic */
    void pendingMsgTimerCallback();
    uint32_t getXtraIntDloadWaitTimeSec();
    void triggerXtraIntDload();
};

}  // namespace izat_core

#endif /* IZAT_ADAPTER_BASE_H */

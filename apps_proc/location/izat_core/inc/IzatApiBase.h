/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015, 2018-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef IZAT_API_BASE_H
#define IZAT_API_BASE_H

#include <gps_extended.h>
#include "IzatAdapterBase.h"
#include <MsgTask.h>
#include <geofence.h>
#include <LocationAPI.h>
#include <vector>


#define MAX_ADAPTERS 10

#define TO_ALL_ADAPTERS(adapters, call)                                \
    for (int i = 0; i < MAX_ADAPTERS && NULL != (adapters)[i]; i++) {  \
        call;                                                          \
    }

#define TO_1ST_HANDLING_ADAPTER(adapters, call)                              \
    bool handled = false; \
    do { \
        for (int i = 0; i <MAX_ADAPTERS && NULL != (adapters)[i] && !handled; i++) { \
            handled = (call); \
        } \
    } while(0)

using namespace std;

namespace izat_core {
class ContextBase;
class IzatAdapterBase;
struct LocSsrMsg;
struct LocOpenMsg;

enum IzatApiReturnCode {
    IZAT_SUCCESS,
    IZAT_FAIL,
    IZAT_INVALID_ARGUMENTS,
    IZAT_GPS_LOCKED,
};

enum XtraVersionCheck {
    XTRA_VERSION_CHECK_DISABLED = 0,
    XTRA_VERSION_CHECK_AUTO,
    XTRA_VERSION_CHECK_XTRA2,
    XTRA_VERSION_CHECK_XTRA3
};

#define XTRA_MP_CONFIG_DISABLE_AUDO_DOWNLOD_TIMER (0x00000001)
#define XTRA_INFO_MASK_ABS_AGE (0x00000001) /**<  How many hours that the current XTRA information is valid  */
#define XTRA_INFO_MASK_REL_AGE (0x00000002) /**<  Last XTRA data download time  */
#define XTRA_INFO_MASK_XTRA_SERVER (0x00000004) /**<  XTRA server URLs  */
#define XTRA_INFO_MASK_NTP_SERVER (0x00000008) /**<  Network Time Protocol(NTP) server URLs  */
#define XTRA_INFO_MASK_TIME_REQUEST (0x00000010) /**<  Network Time Protocol(NTP) server URLs  */
#define XTRA_INFO_MASK_PREF_VALID_AGE (0x00000020) /**<  Network Time Protocol(NTP) server URLs  */

typedef enum {
    FLP_SESSION_TYPE_TRACKING,
    FLP_SESSION_TYPE_BATCHING
} FlpSessionType;

class IzatApiBase {
    
    friend struct LocSsrMsg;
    //LocOpenMsg calls open() which makes it necessary to declare
    //it as a friend
    friend struct LocOpenMsg;
    friend class ContextBase;
    const MsgTask* mMsgTask;
    IzatAdapterBase* mIzatAdapters[MAX_ADAPTERS];

    static bool mIsXtraInitialized;
	
    void initXtraIfNot(IzatAdapterBase* adapter);

protected:
    ContextBase *mContext;
    
public:
    IzatApiBase(const MsgTask* msgTask,
               LOC_API_ADAPTER_EVENT_MASK_T excludedMask,
               ContextBase* context = NULL);
    inline virtual ~IzatApiBase() {}
    void addAdapter(IzatAdapterBase* adapter);
    void removeAdapter(IzatAdapterBase* adapter);

    inline void sendMsg(const LocMsg* msg) const {
        mMsgTask->sendMsg(msg);
    }
    inline void sendMsg(const LocMsg* msg) {
        mMsgTask->sendMsg(msg);
    }

    virtual LocationError addGeofence(uint32_t& hwId,
                                      uint32_t clientId,
                                      const GeofenceOption& options,
                                      const GeofenceInfo& info,
                                      uint8_t confidence);
    virtual LocationError removeGeofence(uint32_t hwId,
                                         uint32_t clientId);
    virtual LocationError pauseGeofence(uint32_t hwId,
                                        uint32_t clientId);
    virtual LocationError resumeGeofence(uint32_t hwId,
                                         uint32_t clientId);
    virtual LocationError modifyGeofence(uint32_t hwId,
                                         uint32_t clientId,
                                         const GeofenceOption& options);

    virtual loc_api_adapter_err
        open(LOC_API_ADAPTER_EVENT_MASK_T mask);
    virtual loc_api_adapter_err
        close();

    virtual void setBatchSize(size_t size);
    virtual void resetStateAfterSSR();

    /* SINGLESHOT */
    virtual LocationError getSingleShot(const SingleShotOptions& options);

    /* TRACKING */
    virtual LocationError startTimeBasedTracking(
            const LocationOptions& options, LocationAccuracyLevel accuracy);
    virtual LocationError startTimeBasedTracking(
            uint8_t suplMode, LocationAccuracyLevel accuracy, uint32_t minInterval,
            bool isSingleShot, uint32_t singleShotTimeout);
    virtual LocationError updateTimeBasedTracking(uint32_t minInterval);
    virtual LocationError stopTimeBasedTracking();

    virtual LocationError startDistanceBasedTracking(uint32_t sessionId,
                                             const LocationOptions& options);
    virtual LocationError stopDistanceBasedTracking(uint32_t sessionId);

    /* BATCHING */
    virtual LocationError startBatching(uint32_t sessionId,
                                const LocationOptions& options,
                                LocationAccuracyLevel accuracy,
                                uint32_t timeout,
                                uint8_t batchAllPos);
    virtual LocationError stopBatching(uint32_t sessionId);
    virtual LocationError getBatchedLocations(size_t count);
    virtual LocationError releaseBatchBuffer();
    virtual LocationError setNMEATypes(uint32_t typesMask);

    /* SINGLESHOT */
    virtual LocationError getWwanZppFix(const GtpOptions& options);
    virtual LocationError getZppFix(Location& zppLoc);

    /* Report outgoing requests to GNSS engine to all adapters */
    void reportRequestStartTracking(
            const LocationOptions& options, bool isSingleShot, LocationAccuracyLevel accuracy);
    void reportRequestStopTracking();
    void reportRequestStartBatching(
            const LocationOptions& options, LocationAccuracyLevel accuracy);
    void reportRequestStopBatching();
    void reportRequestWwanFix();

    /* Request meant for adapters - not to be sent to QMI service */
    bool requestAdapterGetSingleshot(
            const SingleShotOptions& options, SingleshotRequestType requestType,
            IzatAdapterBase* requestorAdapter);

    /* Incoming Events from GNSS engine */
    void reportLocation(const Location& location,
            const GpsLocationExtended& locationExtended, enum loc_sess_status status);
    void reportLocations(Location* locations,
                       size_t count);
    void reportWwanZppFix(const Location& location, LocationError err, bool isBestAvailPosition=false);
	#ifdef FEATURE_LOCATION_MCS
    void reportWwanScanList(void* ind);
	void reportfdclList(void* ind);
	#endif
    void reportWifiFix(const Location& location, LocationError err);

    void geofenceBreach(uint32_t id,
                        Location location,
                        GeofenceBreachType breachTypeMask,
                        uint64_t timestamp);
    void geofenceAlert(GeofenceAlertType alert);

    void reportNmea(const char* nmea, int length, NmeaSource source);
    
    void reportSv(GnssSvNotification& svInfo);

    // upward calls
    void handleEngineUpEvent();
    void handleEngineDownEvent();

    // XTRA Client 2.0
    virtual int setXtraVersionCheck(enum XtraVersionCheck check);
    virtual int injectIzatPcid(uint64_t pcid);
    virtual int injectXtraData(const char* data, uint32_t len);
    virtual int injectNtpTime(const int64_t time, const int64_t reference, const int64_t uncertainty);
    virtual int requestXtraServers();
    virtual int requestXtraConfigInfo(const uint32_t configMask,const uint32_t configReqSource);
    virtual int enableGtpWwan();
    virtual int disableGtpWwan();

    // Control Client
    virtual LocationError setGpsLock(GnssConfigGpsLock lock);

    // Meta Data
    virtual LocationError getBackoffTime(LocationMetaData& metaData);
    virtual LocationError getGnssEnergyConsumed(LocationMetaData& metaData);

    virtual void reportMetaData(const LocationMetaData& metaData);
    virtual int getWwanScanData(uint8_t numOfCells,
                bool isAssisted, 
                uint8_t airInterfaceType);
	#ifdef FEATURE_LOCATION_MCS
	virtual int getBsFdclListAsync();
	#endif

    //gtpwwan client 1.0
    /*========================================================================
    FUNCTION	injectGtpWwanData

    DESCRIPTION 
    This function is invoke by gtp client to inject xtra-t data downloaded from server

    PARAMETERS
    fdclbslist - (IN) point to struct that contains xtra-t data downloaded from server

    RETURN VALUE
    int - return 0 on Success 
    ==========================================================================*/
    virtual int injectGtpWwanData(const Xt_Data* data);

	/*========================================================================
    FUNCTION	injectPosition

    DESCRIPTION 
    This function is invoke by FLP client to inject WIFI position from server

    PARAMETERS
    latitude - (IN) Position latitude
    longitude - (IN) Position longitude
    accuracy - (IN) Position accuracy

    RETURN VALUE
    loc_api_adapter_err - return LOC_API_ADAPTER_ERR_SUCCESS on Success 
    ==========================================================================*/
	virtual loc_api_adapter_err injectPosition(double latitude, double longitude, float accuracy);
	
    /*========================================================================
    FUNCTION	getBsObsList

    DESCRIPTION 
    This function is invoke by gtp client to get Bs obseved data from TLE

    PARAMETERS
    bsobsdata - (OUT) point to struct which will be fill by QMI call

    RETURN VALUE
    int - return 0 on Success 
    ==========================================================================*/
    virtual int getBsObsList(LocBsObsData* bsobsdata);
    /*========================================================================
    FUNCTION	getBsFdclList

    DESCRIPTION 
    This function is invoke by gtp client to get fdcl list data from TLE

    PARAMETERS
    fdclbslist - (OUT) point to struct which will be fill by QMI call

    RETURN VALUE
    int - return 0 on Success 
    ==========================================================================*/
    virtual int getBsFdclList(LocFdclBsList* fdclbslist);
	//gtpwwan
    /*========================================================================
    FUNCTION	handleRequestBsObservedDataService

    DESCRIPTION 
    this function is invoked when we get fdcl event from TLE 

    PARAMETERS
    clientinfo - (IN) point to struct which contain client inforamtion which we get from TLE

    RETURN VALUE
    None 
    ==========================================================================*/
    void handleRequestBsObservedDataService(clientinformation* clientinfo);
	virtual LocationError getGnssTime(GnssTime& time);
	virtual LocationError deleteAidingData(const GnssAidingData& data);

protected:
    LOC_API_ADAPTER_EVENT_MASK_T mMask;
    const LOC_API_ADAPTER_EVENT_MASK_T mExcludedMask;

    // XTRA Client 2.0
    void handleReceiveXtraServers(const char* server1, const char* server2, const char* server3);
    void handleRequestXtraData();
    void handleRequestNtpTime();
    void handleReceiveXtraConfigInfo(uint32_t status, uint32 configReqSource,
                                     uint32_t report_mask, uint16_t absAge,
                                     uint64_t relAge, uint8_t timereqValid,
                                     uint16_t prefValidAgeHrs,
                                     const char* ntpServer1, const char* ntpServer2,
                                     const char* ntpServer3, const char* xtraServer1,
                                     const char* xtraServer2, const char* xtraServer3);
};

}  // namespace izat_core

#endif //IZAT_API_BASE_H

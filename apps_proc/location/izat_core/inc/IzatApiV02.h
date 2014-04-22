/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015, 2018-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef IZAT_API_V02_H
#define IZAT_API_V02_H

#include "IzatApiBase.h"
#include "loc_api_v02_client.h"
#include <loc_util_log.h>
namespace izat_core {

struct MsgReportLocation : public LocMsg {
IzatApiBase& mIzatApi;
Location mLocation;
GpsLocationExtended mLocationExtended;
enum loc_sess_status mStatus;
inline MsgReportLocation(IzatApiBase& izatApi,
                         Location location,
                         GpsLocationExtended locationExtended,
                         enum loc_sess_status status) :
    LocMsg(),
    mIzatApi(izatApi),
    mLocation(location),
    mLocationExtended(locationExtended),
    mStatus(status) {}
    inline virtual void proc() const {
        LOC_LOGL0("MsgReportLocation");
        // calling the base class
        mIzatApi.reportLocation(mLocation, mLocationExtended, mStatus);
    }
};

struct MsgReportSv : public LocMsg {
IzatApiBase& mIzatApi;
const GnssSvNotification mSvNotify;
inline MsgReportSv(IzatApiBase& izatApi,
                   GnssSvNotification& svNotify) :
    LocMsg(),
    mIzatApi(izatApi),
    mSvNotify(svNotify) {}
    inline virtual void proc() const {
        LOC_LOGL0("MsgReportSv");
        mIzatApi.reportSv((GnssSvNotification&)mSvNotify);
    }
};

struct MsgReportGtpWwanDataService: public LocMsg {
IzatApiBase& mIzatApi;
clientinformation* mclientinfo;
inline MsgReportGtpWwanDataService(IzatApiBase& izatApi,
                         clientinformation* clientinfo) :
    LocMsg(),
    mIzatApi(izatApi),
    mclientinfo(clientinfo) {}
    inline virtual void proc() const {
        // calling the base class
        mIzatApi.handleRequestBsObservedDataService(mclientinfo);
        delete mclientinfo;
    }
};

struct MsgReportWwanZppFix : public LocMsg {
IzatApiBase& mIzatApi;
Location mLocation;
LocationError mErr;
inline MsgReportWwanZppFix(IzatApiBase& izatApi,
                           Location location,
                           LocationError err) :
    LocMsg(),
    mIzatApi(izatApi),
    mLocation(location),
    mErr(err) {}
    inline virtual void proc() const {
        LOC_LOGL0("MsgReportWwanZppFix");
        // calling the base class
        mIzatApi.reportWwanZppFix(mLocation, mErr);
    }
};
#ifdef FEATURE_LOCATION_MCS
struct MsgReportWwanScanList : public LocMsg {
IzatApiBase& mIzatApi;
qmiLocGetWwanDataIndMsgT_v02 mWwanScanList;
inline MsgReportWwanScanList(IzatApiBase& izatApi,
                           const qmiLocGetWwanDataIndMsgT_v02& scanlist) :
    LocMsg(),
    mIzatApi(izatApi),
    mWwanScanList(scanlist) {}
    inline virtual void proc() const {
        LOC_LOGL0("MsgReportWwanScanList");
        // calling the base class
        mIzatApi.reportWwanScanList((void*)&mWwanScanList);
    }
};
struct MsgReportFdcLList : public LocMsg {
IzatApiBase& mIzatApi;
qmiLocGetFdclBsListIndMsgT_v02 mFdclist;
inline MsgReportFdcLList(IzatApiBase& izatApi,
                           const qmiLocGetFdclBsListIndMsgT_v02 &ind) :
    LocMsg(),
    mIzatApi(izatApi),
    mFdclist(ind)
  	{}
    inline virtual void proc() const {
        LOC_LOGL0("MsgReportFdcLList proc");
        // calling the base class
        mIzatApi.reportfdclList((void*)&mFdclist);
    }
};

#endif

struct MsgGeofenceBreach : public LocMsg {
    IzatApiBase& mIzatApi;
    uint32_t mId;
    Location mLocation;
    GeofenceBreachType mBreachTypeMask;
    uint64_t mTimestamp;
    inline MsgGeofenceBreach(IzatApiBase& izatApi,
                             uint32_t id,
                             Location location,
                             GeofenceBreachType breachTypeMask,
                             uint64_t timestamp) :
        LocMsg(),
        mIzatApi(izatApi),
        mId(id),
        mLocation(location),
        mBreachTypeMask(breachTypeMask),
        mTimestamp(timestamp) {}
    inline virtual void proc() const {
        LOC_LOGL0("MsgGeofenceBreach");
        // call the base class
        mIzatApi.geofenceBreach(mId,
                                mLocation,
                                mBreachTypeMask,
                                mTimestamp);
    }
};

struct MsgGeofenceAlert : public LocMsg {
    IzatApiBase& mIzatApi;
    GeofenceAlertType mAlert;
    inline MsgGeofenceAlert(IzatApiBase& izatApi,
                            GeofenceAlertType alert) :
        LocMsg(),
        mIzatApi(izatApi),
        mAlert(alert) {}
    inline virtual void proc() const {
        LOC_LOGL0("MsgGeofenceAlert");
        mIzatApi.geofenceAlert(mAlert);
    }
};

struct MsgReportNmea : public LocMsg {
    IzatApiBase& mIzatApi;
    char* mNmea; 
    int mLength;
    inline MsgReportNmea(IzatApiBase& izatApi,
            const char* nmea,
            int length) :
        LocMsg(),
        mIzatApi(izatApi),
        mLength(length) {
            mNmea = new char[mLength + 1];
            if (NULL == mNmea) {
                LOC_LOGE0("new allocation failed, fatal error.");
                return;
            }
            strlcpy((char*)mNmea, nmea, mLength+1);
        }
    inline virtual ~MsgReportNmea()
    {
        delete[] mNmea;
    }
    inline virtual void proc() const {
        // calling the base class
        mIzatApi.reportNmea(mNmea, mLength, LOCATION_NMEA_SOURCE_GNSS_ENGINE);
    }
};

struct MsgReportRequestStartTracking : public LocMsg {

    IzatApiBase& mIzatApi;
    uint32_t mMinInterval;
    bool mIsSingleShot;
    LocationAccuracyLevel mAccuracy;

    inline MsgReportRequestStartTracking(IzatApiBase& izatApi,
            uint32_t minInterval, bool isSingleShot, LocationAccuracyLevel accuracy) :
            LocMsg(),
            mIzatApi(izatApi),
            mMinInterval(minInterval),
            mIsSingleShot(isSingleShot),
            mAccuracy(accuracy) {}
    inline virtual void proc() const {
        LocationOptions options = {};
        options.size = sizeof(LocationOptions);
        options.minInterval = mMinInterval;
        mIzatApi.reportRequestStartTracking(options, mIsSingleShot, mAccuracy);
    }
};

struct MsgReportRequestStopTracking : public LocMsg {

    IzatApiBase& mIzatApi;

    inline MsgReportRequestStopTracking(IzatApiBase& izatApi) :
            LocMsg(),
            mIzatApi(izatApi) {}
    inline virtual void proc() const {
        mIzatApi.reportRequestStopTracking();
    }
};

struct MsgReportRequestStartBatching : public LocMsg {

    IzatApiBase& mIzatApi;
    const LocationOptions& mOptions;
    LocationAccuracyLevel mAccuracy;

    inline MsgReportRequestStartBatching(IzatApiBase& izatApi,
            const LocationOptions& options, LocationAccuracyLevel accuracy) :
            LocMsg(),
            mIzatApi(izatApi),
            mOptions(options),
            mAccuracy(accuracy) {}
    inline virtual void proc() const {
        mIzatApi.reportRequestStartBatching(mOptions, mAccuracy);
    }
};

struct MsgReportRequestStopBatching : public LocMsg {

    IzatApiBase& mIzatApi;

    inline MsgReportRequestStopBatching(IzatApiBase& izatApi) :
            LocMsg(),
            mIzatApi(izatApi) {}
    inline virtual void proc() const {
        mIzatApi.reportRequestStopBatching();
    }
};

struct MsgReportRequestWwanFix : public LocMsg {

    IzatApiBase& mIzatApi;

    inline MsgReportRequestWwanFix(IzatApiBase& izatApi) :
            LocMsg(),
            mIzatApi(izatApi) {}
    inline virtual void proc() const {
        mIzatApi.reportRequestWwanFix();
    }
};

class IzatApiV02 : public IzatApiBase {
        
    size_t mBatchSize;
    size_t mDesiredBatchSize;
    bool mTimeBasedTrackingInProgress;
    bool mDistanceBasedTrackingInProgress;
    bool mBatchingInProgress;

    /* Convert event mask from loc eng to loc_api_v02 format */
    static locClientEventMaskType convertMask(LOC_API_ADAPTER_EVENT_MASK_T mask);

    /* Convert error from loc_api_v02 to loc eng format*/
    static loc_api_adapter_err convertErr(locClientStatusEnumType status);

    /* Convert GPS LOCK mask */
    // static qmiLocLockEnumT_v02 convertGpsLockMask(GnssConfigGpsLock lock);

    LocPosTechMask convertPosTechMask(qmiLocPosTechMaskT_v02 mask);
    LocNavSolutionMask convertNavSolutionMask(qmiLocNavSolutionMaskT_v02 mask);

    bool registerEventMask(locClientEventMaskType qmiMask);
    locClientEventMaskType adjustMaskForNoSession(locClientEventMaskType qmiMask);

    void readModemLocations(Location* pLocationPiece,
                            size_t count,
                            size_t& numbOfEntriess);
    LocationError queryBatchBufferOnModem(size_t size);
    
    void handleDbtPositionEvent(
        const qmiLocEventDbtPositionReportIndMsgT_v02* pDbtPosReport);
	
    void handlePositionEvent(
        const qmiLocEventPositionReportIndMsgT_v02 *location_report_ptr);
	
    void handleBatchFullEvent(size_t count);

    void handleGeofenceBreachEvent(
        const qmiLocEventGeofenceBreachIndMsgT_v02* batchedBreachInfo);

    void handleGeofenceGenAlertEvent(
        const qmiLocEventGeofenceGenAlertIndMsgT_v02* geofenceGenAlertEvent);

    void handleBsObservedDataServiceEvent(
        const qmiLocEventFdclServiceReqIndMsgT_v02 *fdclServiceEvent);

    /* convert NMEA report to loc eng format and send the converted
    report to loc eng */
    void handleNmeaEvent(const qmiLocEventNmeaIndMsgT_v02 *nmea_report_ptr);

    uint64 mLastInjectPosReqTime; /*millis since epoch*/ 
    
#define INJECT_POS_REQ_LIMIT_MS 1000 /* minimum allowed gap between requests from Modem*/


	void handleSvInfoEventIndication(const qmiLocEventGnssSvInfoIndMsgT_v02* svInfoEvent);

	float convertSignalTypeToCarrierFrequency(
	  	  qmiLocGnssSignalTypeMaskT_v02 signalType,
	  	  uint8_t gloFrequency);

	GnssSvType getLocApiSvSystemType (
		       qmiLocSvSystemEnumT_v02 qmiSvSystemType);
	GnssSignalTypeMask getLocApiSignalType (
		               qmiLocGnssSignalTypeMaskT_v02 gnssSignalType);
	svInfoValidMask getLocApiSvInfoMask(qmiLocSvInfoValidMaskT_v02 validMask);

    /* If Confidence value is less than 68%, then scale the accuracy value to 68%
       confidence.*/
    void scaleAccuracyTo68PercentConfidence(const uint8_t confidenceValue,
                                              Location &gpsLocation,
                                              const bool isCircularUnc);

protected:
    /* loc api v02 handle*/
    locClientHandleType clientHandle;
    locClientEventMaskType mQmiMask;

    virtual loc_api_adapter_err
      open(LOC_API_ADAPTER_EVENT_MASK_T mask);
    virtual loc_api_adapter_err
      close();
public:
    IzatApiV02(const MsgTask* msgTask,
                 LOC_API_ADAPTER_EVENT_MASK_T exMask,
                 ContextBase* context);

    virtual ~IzatApiV02();

    /* event callback registered with the loc_api v02 interface */
    virtual void eventCb(locClientHandleType client_handle,
                         uint32_t loc_event_id,
                         locClientEventIndUnionType loc_event_payload);

    /* error callback, this function handles the  service unavailable
       error */
    void errorCb(locClientHandleType handle,
                 locClientErrorEnumType errorId);

    /* Update Registration Mask */
    virtual int updateRegistrationMask(LOC_API_ADAPTER_EVENT_MASK_T event,
                                       loc_registration_mask_status isEnabled);

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
    virtual LocationError releaseBatchBuffer();

    virtual LocationError getBatchedLocations(size_t count);

    virtual LocationError addGeofence(uint32_t& hwId,
                                      uint32_t clientId,
                                      const GeofenceOption& options,
                                      const GeofenceInfo& info,
                                      uint8_t confidence);
    virtual LocationError removeGeofence(uint32_t hwId,
                                         uint32_t clientId);

    // Best Available Position
    virtual LocationError getWwanZppFix(const GtpOptions& options);
    void handleWwanZppFixIndication(
            const qmiLocGetAvailWwanPositionIndMsgT_v02& zpp_ind);
    virtual LocationError getZppFix(Location& zppLoc);

    // XTRA Client 2.0
    virtual int setXtraVersionCheck(enum XtraVersionCheck check);
    virtual int injectIzatPcid(uint64_t pcid);
    virtual int injectXtraData(const char* data, uint32_t len);
    virtual int injectNtpTime(const int64_t time, const int64_t reference, const int64_t uncertainty);
    virtual int requestXtraServers();
    virtual int requestXtraConfigInfo(const uint32_t configMask, const uint32_t configReqSource);

    /* Control Commands */
    virtual int enableGtpWwan();
    virtual int disableGtpWwan();

    /* Meta data requests */
    virtual LocationError getBackoffTime(LocationMetaData& metaData);
    virtual LocationError getGnssEnergyConsumed(LocationMetaData& metaData);
	virtual int getWwanScanData(uint8_t numOfCells,
                bool isAssisted, 
                uint8_t airInterfaceType);
	virtual void handleWwanDataIndication(const qmiLocGetWwanDataIndMsgT_v02 &wwan_ind);
	#ifdef FEATURE_LOCATION_MCS
	//LocFdclBsList mFdclBsList;
	virtual int getBsFdclListAsync();
	virtual int handleBsFdclListResponse(const qmiLocGetFdclBsListIndMsgT_v02 &ind);
	#endif

    //GtpWan client 
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
    FUNCTION	getGnssTime

    DESCRIPTION 
    Invoked by XTRA Adapter client to get GNSS Time from GNSS Engine

    PARAMETERS
    GnssTime - (OP) GNSS time

    RETURN VALUE
    loc_api_adapter_err - return LOC_API_ADAPTER_ERR_SUCCESS on Success 
    ==========================================================================*/
	virtual LocationError getGnssTime(GnssTime& time);
	virtual LocationError deleteAidingData(const GnssAidingData& data);
};

}  // namespace izat_core

#endif //IZAT_API_V02_H

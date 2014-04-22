/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015, 2018-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_NDDEBUG 0
#define LOG_TAG "LocSvc_IzatApiBase"

#include <log_util.h>
#include <IzatApiBase.h>
#include <XtraApis.h>
#include <LocOSIface.h>

using namespace izat_xtra;

namespace izat_core {

bool IzatApiBase::mIsXtraInitialized = false;

struct LocOpenMsg : public LocMsg {
    IzatApiBase* mIzatApi;
    LOC_API_ADAPTER_EVENT_MASK_T mMask;
    using LocMsg::log;
    inline LocOpenMsg(IzatApiBase* izatApi,
                      LOC_API_ADAPTER_EVENT_MASK_T mask) :
        LocMsg(), mIzatApi(izatApi), mMask(mask)
    {
        LOC_LOGM0("LocOpenMsg constructor");
    }
    inline virtual void proc() const {
        LOC_LOGM1("LocOpenMsg proc mIzatApi=%p", mIzatApi);
        if(NULL != mIzatApi) {
            mIzatApi->open(mMask);
        }
    }
    inline void locallog() {
        LOC_LOGL1("LocOpen Mask: %x", mMask);
    }
    inline virtual void log() {
        LOC_LOGM0("LocOpenMsg log");
        locallog();
    }
};

struct LocSsrMsg : public LocMsg {
    IzatApiBase* mIzatApi;
    using LocMsg::log;
    inline LocSsrMsg(IzatApiBase* izatApi) :
        LocMsg(), mIzatApi(izatApi)
    {
        locallog();
    }
    inline virtual void proc() const {
        LOC_LOGM1("LocSsrMsg proc mIzatApi=%p", mIzatApi);
        if(NULL != mIzatApi) {
            mIzatApi->close();
            mIzatApi->open(mIzatApi->mMask);
        }
    }
    inline void locallog() {
        LOC_LOGL0("LocSsrMsg");
    }
    inline virtual void log() {
        locallog();
    }
};

#define TO_ALL_IZATADAPTERS(call) TO_ALL_ADAPTERS(mIzatAdapters, (call))
#define TO_1ST_HANDLING_IZATADAPTERS(call) TO_1ST_HANDLING_ADAPTER(mIzatAdapters, (call))

IzatApiBase::IzatApiBase(const MsgTask* msgTask,
                       LOC_API_ADAPTER_EVENT_MASK_T excludedMask,
                       ContextBase* context) :
    mMsgTask(msgTask), mContext(context),
    mMask(0), mExcludedMask(excludedMask)
    
{
    memset(mIzatAdapters, 0, sizeof(mIzatAdapters));
    LOC_LOGL0("IzatApiBase created");
}

void IzatApiBase::initXtraIfNot(IzatAdapterBase* adapter)
{
    /* init XTRA Client 2.0 in the following conditions
       1. make singleton of XTRA Client 2.0;
       2. in system_server.
    */

    if (mIsXtraInitialized) {
        LOC_LOGL0("[XTRA2]: XTRA Client already created.");
        return;
    }
    mIsXtraInitialized = true;

    LOC_LOGL0("[XTRA2]: schedule to init XTRA in system_server.");
    initXtra();
}

void IzatApiBase::addAdapter(IzatAdapterBase* adapter)
{
    for (int i = 0; i < MAX_ADAPTERS && mIzatAdapters[i] != adapter; i++) {
        if (mIzatAdapters[i] == NULL) {
            mIzatAdapters[i] = adapter;
            LOC_LOGL2("this is %dth adapter %p", i, adapter);
  
            mMsgTask->sendMsg(new LocOpenMsg(this, adapter->getEvtMask()));
            break;
        }
    }

    LOC_LOGM0("Initialize XTRA");
    initXtraIfNot(adapter);
}

void IzatApiBase::removeAdapter(IzatAdapterBase* adapter)
{
    LOC_LOGL1("removeAdapter for %p", adapter);
    for (int i = 0;
         i < MAX_ADAPTERS && NULL != mIzatAdapters[i];
         i++) {
        if (mIzatAdapters[i] == adapter) {
            mIzatAdapters[i] = NULL;

            // shift the rest of the adapters up so that the pointers
            // in the array do not have holes.  This should be more
            // performant, because the array maintenance is much much
            // less frequent than event handlings, which need to linear
            // search all the adapters
            int j = i;
            while (++i < MAX_ADAPTERS && mIzatAdapters[i] != NULL);

            // i would be MAX_ADAPTERS or point to a NULL
            i--;
            // i now should point to a none NULL adapter within valid
            // range although i could be equal to j, but it won't hurt.
            // No need to check it, as it gains nothing.
            mIzatAdapters[j] = mIzatAdapters[i];
            // this makes sure that we exit the for loop
            mIzatAdapters[i] = NULL;
            LOC_LOGL1("%d adapters left", i);
        }
    }
}

LocationError
IzatApiBase::addGeofence(uint32_t& hwId,
                         uint32_t clientId,
                         const GeofenceOption& options,
                         const GeofenceInfo& info,
                         uint8_t confidence)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::removeGeofence(uint32_t hwId,
                            uint32_t clientId)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::pauseGeofence(uint32_t hwId,
                           uint32_t clientId)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::resumeGeofence(uint32_t hwId,
                            uint32_t clientId)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::modifyGeofence(uint32_t hwId,
                            uint32_t clientId,
                            const GeofenceOption& options)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

void IzatApiBase::handleEngineUpEvent()
{
    // loop through adapters, and deliver to all adapters.
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->handleEngineUpEvent());
}

void IzatApiBase::handleEngineDownEvent()
{
    // This will take care of renegotiating the loc handle
    mMsgTask->sendMsg(new LocSsrMsg(this));

    // loop through adapters, and deliver to all adapters.
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->handleEngineDownEvent());
}

// XTRA Client 2.0
int IzatApiBase::setXtraVersionCheck(enum XtraVersionCheck check)
DEFAULT_IMPL(0)

int IzatApiBase::injectIzatPcid(uint64_t pcid)
DEFAULT_IMPL(0)

int IzatApiBase::injectXtraData(const char* data, uint32_t len)
DEFAULT_IMPL(0)

int IzatApiBase::injectNtpTime(const int64_t time, const int64_t reference, const int64_t uncertainty)
DEFAULT_IMPL(0)

int IzatApiBase::requestXtraServers()
DEFAULT_IMPL(0)

int IzatApiBase::requestXtraConfigInfo(
        const uint32_t /*configMask*/, const uint32_t /*configReqSource*/)
DEFAULT_IMPL(0)

int IzatApiBase::enableGtpWwan()
DEFAULT_IMPL(0)

int IzatApiBase::disableGtpWwan()
DEFAULT_IMPL(0)

int IzatApiBase::injectGtpWwanData(const Xt_Data* data)
DEFAULT_IMPL(0)

void IzatApiBase::handleReceiveXtraServers(const char* server1, const char* server2, const char* server3)
{
    TO_1ST_HANDLING_IZATADAPTERS(mIzatAdapters[i]->onReceiveXtraServers(server1, server2, server3));
}

void IzatApiBase::handleRequestXtraData()
{
    TO_1ST_HANDLING_IZATADAPTERS(mIzatAdapters[i]->onRequestXtraData());
}

void IzatApiBase::handleRequestNtpTime()
{
    TO_1ST_HANDLING_IZATADAPTERS(mIzatAdapters[i]->onRequestNtpTime());
}

void IzatApiBase::handleRequestBsObservedDataService(clientinformation* clientinfo)
{
    TO_1ST_HANDLING_IZATADAPTERS(mIzatAdapters[i]->onRequestBsObservedData(clientinfo));
}

void IzatApiBase::handleReceiveXtraConfigInfo(uint32_t status, uint32 configReqSource,
                                              uint32_t report_mask,
                                              uint16_t absAge, uint64_t relAge,
                                              uint8_t timereqValid, uint16_t prefValidAgeHrs,
                                              const char* ntpServer1, const char* ntpServer2,
                                              const char* ntpServer3, const char* xtraServer1,
                                              const char* xtraServer2, const char* xtraServer3)
{
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->onReceiveXtraConfig(status, configReqSource,
                                                                       report_mask, absAge, relAge,
                                                                       timereqValid, prefValidAgeHrs,
                                                                       ntpServer1, ntpServer2,
                                                                       ntpServer3, xtraServer1,
                                                                       xtraServer2, xtraServer3));
}

loc_api_adapter_err
IzatApiBase::open(LOC_API_ADAPTER_EVENT_MASK_T mask)
DEFAULT_IMPL(LOC_API_ADAPTER_ERR_SUCCESS)

loc_api_adapter_err
IzatApiBase::close()
DEFAULT_IMPL(LOC_API_ADAPTER_ERR_SUCCESS)

void
IzatApiBase::setBatchSize(size_t size)
DEFAULT_IMPL()

void
IzatApiBase::resetStateAfterSSR()
DEFAULT_IMPL()

LocationError
IzatApiBase::startBatching(uint32_t sessionId,
                           const LocationOptions& options,
                           LocationAccuracyLevel accuracy,
                           uint32_t timeout,
                           uint8_t batchAllPos)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::stopBatching(uint32_t sessionId)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::getBatchedLocations(size_t count)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::releaseBatchBuffer()
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError 
IzatApiBase::setNMEATypes(uint32_t /*typesMask*/)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::getSingleShot(const SingleShotOptions& options)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

loc_api_adapter_err
IzatApiBase::injectPosition(double latitude, double longitude, float accuracy)
DEFAULT_IMPL(LOC_API_ADAPTER_ERR_SUCCESS)


LocationError
IzatApiBase::getWwanZppFix(const GtpOptions& options)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::getZppFix(Location&)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::startTimeBasedTracking(
        const LocationOptions& options, LocationAccuracyLevel accuracy)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::startTimeBasedTracking(
        uint8_t suplMode, LocationAccuracyLevel accuracy, uint32_t minInterval,
        bool isSingleShot, uint32_t singleShotTimeout)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::updateTimeBasedTracking(uint32_t minInterval)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::stopTimeBasedTracking()
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::startDistanceBasedTracking(uint32_t sessionId,
                                        const LocationOptions& options)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::stopDistanceBasedTracking(uint32_t sessionId)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::setGpsLock(GnssConfigGpsLock /*lock*/)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::getBackoffTime(LocationMetaData& metaData)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::getGnssEnergyConsumed(LocationMetaData& metaData)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError
IzatApiBase::deleteAidingData(const GnssAidingData& data)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

LocationError 
IzatApiBase::getGnssTime(GnssTime& time)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

int
IzatApiBase::getWwanScanData(uint8_t numOfCells,
                bool isAssisted, 
                uint8_t airInterfaceType)
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)

#ifdef FEATURE_LOCATION_MCS
int
IzatApiBase::getBsFdclListAsync()
DEFAULT_IMPL(LOCATION_ERROR_SUCCESS)
#endif

void
IzatApiBase::reportMetaData(const LocationMetaData& metaData)
{
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->reportMetaData(metaData));
}

void
IzatApiBase::reportLocation(const Location& location,
        const GpsLocationExtended& locationExtended, enum loc_sess_status status)
{
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->reportLocation(location, locationExtended, status));
}
    
void
IzatApiBase::reportLocations(Location* locations,
                             size_t count)
{
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->reportLocations(locations, count));
}

void
IzatApiBase::reportWwanZppFix(const Location& location, LocationError err, bool isBestAvailPosition)
{
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->reportWwanZppFix(location, err, isBestAvailPosition));
}
#ifdef FEATURE_LOCATION_MCS
void
IzatApiBase::reportWwanScanList(void* ind)
{
    TO_1ST_HANDLING_IZATADAPTERS(mIzatAdapters[i]->reportWwanScanList(ind));
}
void
IzatApiBase::reportfdclList(void* ind)
{
    TO_1ST_HANDLING_IZATADAPTERS(mIzatAdapters[i]->handleReportfdclList(ind));
}
#endif
void
IzatApiBase::reportWifiFix(const Location& location, LocationError err)
{
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->reportWifiFix(location, err));
}

void
IzatApiBase::geofenceBreach(uint32_t id,
                            Location location,
                            GeofenceBreachType breachTypeMask,
                            uint64_t timestamp)
{
    TO_1ST_HANDLING_IZATADAPTERS(mIzatAdapters[i]->geofenceBreach(id,
                                                                  location,
                                                                  breachTypeMask,
                                                                  timestamp));
}

void
IzatApiBase::geofenceAlert(GeofenceAlertType alert)
{
    TO_1ST_HANDLING_IZATADAPTERS(mIzatAdapters[i]->geofenceAlert(alert));
}

void
IzatApiBase::reportNmea(const char* nmea, int length, NmeaSource source)
{
    TO_1ST_HANDLING_IZATADAPTERS(mIzatAdapters[i]->reportNmea(nmea,length,source));
}

void
IzatApiBase::reportSv(GnssSvNotification& svInfo)
{
    TO_1ST_HANDLING_IZATADAPTERS(mIzatAdapters[i]->reportSv(svInfo));
}

int
IzatApiBase::getBsObsList(LocBsObsData* bsobsdata)
DEFAULT_IMPL(0)
	
int
IzatApiBase::getBsFdclList(LocFdclBsList* fdclbslist)
DEFAULT_IMPL(0)


// Outgoing requests to gnss engine
void
IzatApiBase::reportRequestStartTracking(
        const LocationOptions& options, bool isSingleShot, LocationAccuracyLevel accuracy)
{
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->reportRequestStartTracking(options, isSingleShot, accuracy));
}

void
IzatApiBase::reportRequestStopTracking()
{
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->reportRequestStopTracking());
}

void
IzatApiBase::reportRequestStartBatching(
        const LocationOptions& options, LocationAccuracyLevel accuracy)
{
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->reportRequestStartBatching(options, accuracy));
}

void
IzatApiBase::reportRequestStopBatching()
{
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->reportRequestStopBatching());
}

void
IzatApiBase::reportRequestWwanFix()
{
    TO_ALL_IZATADAPTERS(mIzatAdapters[i]->reportRequestWwanFix());
}

// Incoming request meant to be handled by one of the adapters
// Return value indicates if any adapter handled the request
bool
IzatApiBase::requestAdapterGetSingleshot(
        const SingleShotOptions& options, SingleshotRequestType requestType,
        IzatAdapterBase* requestorAdapter)
{
    TO_1ST_HANDLING_IZATADAPTERS(
            mIzatAdapters[i]->requestAdapterGetSingleshot(
                    options, requestType, requestorAdapter));
    return handled;
}

}  // namespace izat_core

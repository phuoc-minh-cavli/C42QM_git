/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015, 2018-2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_NDEBUG 0
#define LOG_NDDEBUG 1
#define LOG_TAG "LocSvc_IzatAdapterBase"

#include <log_util.h>
#include <IzatAdapterBase.h>
#include <IzatApiBase.h>
#include <XtraApis.h>
#include <XtraClient.h>

#define IZAT_ADAPTER_PENDING_MSG_TIMER "izat_adapter_pending_msg_timer"

using namespace izat_xtra;

namespace izat_core {

static void sPendingMsgTimerCallback(void* izatAdapterPtr){
    LOC_LOGH0("sPendingMsgTimerCallback()");
    static_cast<IzatAdapterBase*>(izatAdapterPtr)->
            pendingMsgTimerCallback();
}

IzatAdapterBase::IzatAdapterBase(const LOC_API_ADAPTER_EVENT_MASK_T mask,
                                 ContextBase* context,
                                 uint32_t xtraIntDloadWaitTime,
                                 uint32_t xtraIntDloadRetryLimit) :
    mEvtMask(mask),
    mContext(context),
    mIzatApi(context->getIzatApi()),
    mMsgTask(context->getMsgTask()),
    mPendingMsgTimer(IZAT_ADAPTER_PENDING_MSG_TIMER),
    mXtraIntDloadWaitTime(xtraIntDloadWaitTime),
    mXtraIntDloadRetries(0),
    mXtraIntDloadRetryLimit(xtraIntDloadRetryLimit)
{
    LOC_LOGM2("IzatAdapterBase constructor context=%p this=%p", context, this);
    mPendingMsgTimer.set(0, sPendingMsgTimerCallback, this);
    if(NULL != mIzatApi) {
        mIzatApi->addAdapter(this);
    }
}

IzatAdapterBase:: ~IzatAdapterBase()
{
    LOC_LOGL1("Inside ~IzatAdapterBase, calling removeAdapter for %p", this);
    if(NULL != mIzatApi) {
        mIzatApi->removeAdapter(this);
    } 
}

/* Session IDs shared across Adapters
 * Start with 2, 1 is reserved for Time Based Tracking dummy session id */
#define SESSION_ID_INITIAL_VALUE 2
uint32_t IzatAdapterBase::mSessionIdCounter(SESSION_ID_INITIAL_VALUE);
uint32_t
IzatAdapterBase::generateSessionId()
{
    if (++mSessionIdCounter == 0xFFFFFFFF)
        mSessionIdCounter = SESSION_ID_INITIAL_VALUE;

     return mSessionIdCounter;
}

bool
IzatAdapterBase::reportLocations(Location* location,
                                 size_t count)
DEFAULT_IMPL(false)

bool
IzatAdapterBase::reportLocation(const Location& location,
        const GpsLocationExtended& locationExtended, enum loc_sess_status status)
DEFAULT_IMPL(false)

bool
IzatAdapterBase::reportWwanZppFix(const Location& location, LocationError err, bool isBestAvailPosition)
DEFAULT_IMPL(false)

#ifdef FEATURE_LOCATION_MCS
bool
IzatAdapterBase::reportWwanScanList(void* ind)
DEFAULT_IMPL(false)
bool
IzatAdapterBase::handleReportfdclList(void* ind)
DEFAULT_IMPL(false)
#endif
bool
IzatAdapterBase::reportWifiFix(const Location& location, LocationError err)
DEFAULT_IMPL(false)

bool
IzatAdapterBase::geofenceBreach(uint32_t id,
                                Location location,
                                GeofenceBreachType breachTypeMask,
                                uint64_t timestamp)
DEFAULT_IMPL(false)

bool
IzatAdapterBase::geofenceAlert(GeofenceAlertType alert)
DEFAULT_IMPL(false)

bool
IzatAdapterBase::reportNmea(const char* nmea, int length, NmeaSource source)
DEFAULT_IMPL(false)

bool
IzatAdapterBase::reportSv(GnssSvNotification& svInfo)
DEFAULT_IMPL(false)

void IzatAdapterBase::
    handleEngineUpEvent()
DEFAULT_IMPL()

void IzatAdapterBase::
    handleEngineDownEvent()
DEFAULT_IMPL()

// Outgoing requests to GNSS engine
void
IzatAdapterBase::reportRequestStartTracking(
        const LocationOptions& options, bool isSingleShot, LocationAccuracyLevel accuracy)
DEFAULT_IMPL()

void
IzatAdapterBase::reportRequestStopTracking()
DEFAULT_IMPL()

void
IzatAdapterBase::reportRequestStartBatching(
        const LocationOptions& options, LocationAccuracyLevel accuracy)
DEFAULT_IMPL()

void
IzatAdapterBase::reportRequestStopBatching()
DEFAULT_IMPL()

void
IzatAdapterBase::reportRequestWwanFix()
DEFAULT_IMPL()

// Requests meant to be handled by one of the adapters
bool
IzatAdapterBase::requestAdapterGetSingleshot(
        const SingleShotOptions& options, SingleshotRequestType requestType,
        IzatAdapterBase* requestorAdapter)
DEFAULT_IMPL(false)


// XTRA Client 2.0
bool IzatAdapterBase::
    onReceiveXtraServers(const char* server1, const char* server2, const char* server3)
DEFAULT_IMPL(false)

bool IzatAdapterBase::
    onRequestXtraData()
DEFAULT_IMPL(false)

bool IzatAdapterBase::
    onRequestNtpTime()
DEFAULT_IMPL(false)
	
bool IzatAdapterBase::
	 onRequestBsObservedData(clientinformation* clientinfo)
DEFAULT_IMPL(false)

bool IzatAdapterBase::
     onReceiveXtraConfig(uint32_t /*status*/, uint32_t /*configReqSource*/,
             uint32_t /*report_mask*/, uint16_t /*absAge*/, uint64_t /*relAge*/,
             uint8_t /*timereqValid*/, uint16_t /*prefValidAgeHrs*/,
             const char* /*ntpServer1*/, const char* /*ntpServer2*/, const char* /*ntpServer3*/,
             const char* /*xtraServer1*/, const char* /*xtraServer2*/, const char* /*xtraServer3*/)
DEFAULT_IMPL(false)

bool
IzatAdapterBase::reportMetaData(const LocationMetaData& metaData, LocationAPI* client)
DEFAULT_IMPL(false)

/*------------------------------------------------
  XTRA INTEGRITY DL LOGIC
-------------------------------------------------*/
void IzatAdapterBase::pendingMsgTimerCallback() {

    LOC_LOGL0("pendingMsgTimerCallback()");
    mXtraIntDloadRetries++;
    clearPendingMsgs();
}

uint32_t IzatAdapterBase::getXtraIntDloadWaitTimeSec() {

    if (mXtraIntDloadRetries < mXtraIntDloadRetryLimit) {
        XtraClient* xtraClient = (XtraClient*)getXtraClient();
        if (NULL != xtraClient) {
            if (0 == xtraClient->getMinTimeToNextXtraIntDload()) {
                return mXtraIntDloadWaitTime;
            }
        }
    } else {
        LOC_LOGL1("Delayed GNSS session for INT DL %d times. Limit Reached.",
                  mXtraIntDloadRetries);
    }
    return 0;
}

void IzatAdapterBase::triggerXtraIntDload() {

    LOC_LOGL0("triggerXtraIntDload()");
    XtraClient* xtraClient = (XtraClient*)getXtraClient();
    if (NULL != xtraClient) {
        xtraClient->onRequestXtraIntegrity(false, REQ_SOURCE_AUTO_DOWNLOAD, true);
    }
}
}  // namespace izat_core

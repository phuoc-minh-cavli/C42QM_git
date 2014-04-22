/*=========================================================================
  Copyright (c) 2013-2018,2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#include <log_util.h>
#include "loc_util_log.h"

#include <LocDualContext.h>
#include <FlpLocationAdapter.h>
#include <IzatApiBase.h>
#include <LocOSIface.h>
#include <SystemStatus.h>
#include <XtraApis.h>
#include <XtraClient.h>
#include <loc_nmea.h>
#include <sstream>
#include "loc_misc_utils.h"
#include <LocDBIface.h>
#include <algorithm>

#define WEAK_GNSS_REPORTING_INTERVAL_MILLIS 60000
#define MAX_WEAK_GNSS_SV_REPORT_GAP_MILLIS 5000

extern pthread_cond_t gFlpCleanupSignal;
extern pthread_mutex_t gCleanupSignalMutex;

// Utility to parse sv info for weak gnss signals
static boolean _isWeakSignalStrength(GnssSvNotification& svInfo) {

    uint32_t count = 0;
    for (int i = 0; i < svInfo.count; i++)
    {
        // Consider only GPS for good signal conditions
        if (GNSS_SV_TYPE_GPS == svInfo.gnssSvs[i].type &&
                svInfo.gnssSvs[i].cN0Dbhz > 28)
        {
            count++;
        }
    }

    LOC_LOGM1("GPS SV count with cn0 > 28 = %d", count);
    return (count < 4)? TRUE: FALSE;
}

using namespace izat_core;
using namespace izat_xtra;

const LOC_API_ADAPTER_EVENT_MASK_T FLP_MASK =
    (LOC_API_ADAPTER_BIT_PARSED_POSITION_REPORT |
     LOC_API_ADAPTER_BIT_BATCHED_POSITION_REPORT |
     LOC_API_ADAPTER_BIT_NMEA_1HZ_REPORT |
     LOC_API_ADAPTER_BIT_BATCH_FULL |
     LOC_API_ADAPTER_BIT_NMEA_POSITION_REPORT);

/* Ctor: Invoked from the client thread */
FlpLocationAdapter::FlpLocationAdapter(LocationConfig config) :
    IzatAdapterBase(FLP_MASK,
                    LocDualContext::getLocFgContext(
                        NULL,
                        NULL,
                        LocDualContext::mLocationHalName,
                        false),
                    config.xtraIntDloadWaitTime, config.xtraIntDloadRetryLimit)
    ,mControlCallbacks()
    ,mGnssDataSessionCount(0)
    ,mGnssSvIdUsedInPosAvail(false)
    ,mIsSingleshotGnssCbPending(false)
    ,mIsSingleshotWwanCbPending(false)
    ,mIsSingleshotMcsCbPending(false)
    ,mIsSingleshotWifiCbPending(false)
    ,mActiveConstellationMask(0xFFFFFFFF)
    ,mConfig(config)
    ,mFirstEngineUpEventRecvd(false)
    ,mSingleShotTimer(this, FLP_TIMER_TYPE_SINGLESHOT)
    ,mSystemStatus(SystemStatus::getInstance(mMsgTask))
    ,mGtpWwanNvItems(NULL)
#ifndef SENSORS_DISABLED
    ,mSlimClient(NULL)
#endif
    ,mIsGnssLoaded(false)
{
    LOC_LOGH0("FlpLocationAdapter()");
    mSingleShotTimer.set(FLP_SINGLE_SHOT_TIMER_INTERVAL);
    mSingleShotTimer.setRecurring(true);
    configCommand();
}

/*------------------------------------------------
  Adapter init/cleanup
  Invoked from Client Thread
-------------------------------------------------*/
void
FlpLocationAdapter::initializeCommand(size_t size)
{
    struct FlpInitialize : public LocMsg {
        FlpLocationAdapter& mAdapter;
        size_t mSize;
        inline FlpInitialize(FlpLocationAdapter& adapter, size_t size) :
            LocMsg(),
            mAdapter(adapter),
            mSize(size) {}
        inline virtual void proc() const {
            mAdapter.setBatchSize(mSize);
        }
    };
    sendMsg(new FlpInitialize(*this, size));
}

void
FlpLocationAdapter::cleanUpCommand()
{
    LOC_LOGM0("cleanUp()");

    struct FlpCleanUp : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mIzatApi;
        inline FlpCleanUp(FlpLocationAdapter& adapter,
                          IzatApiBase& izatApi) :
                LocMsg(),
                mAdapter(adapter),
                mIzatApi(izatApi) {}
        inline virtual void proc() const {

            LocDBIface_log_efs("Before FLP cleanup stopAllClientSessions");
            mAdapter.stopAllClientSessions();
            LocDBIface_log_efs("Before FLP cleanup releaseBatchBuffer");
            mIzatApi.releaseBatchBuffer();
            LocDBIface_log_efs("After FLP cleanup");

            pthread_mutex_lock(&gCleanupSignalMutex);
            pthread_cond_signal(&gFlpCleanupSignal);
            pthread_mutex_unlock(&gCleanupSignalMutex);
        }
    };

    sendMsg(new FlpCleanUp(*this, *mIzatApi));
}

void
FlpLocationAdapter::configCommand()
{
    LOC_LOGM1("configCommand() mIzatApi=%p", mIzatApi);

    struct FlpConfig : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mIzatApi;
        inline FlpConfig(FlpLocationAdapter& adapter,
            IzatApiBase& izatApi) :
            LocMsg(),
            mAdapter(adapter),
            mIzatApi(izatApi) {}
        inline virtual void proc() const {
            // mIzatApi.setNMEATypes(LOC_NMEA_MASK_DEBUG_V02);
        }
    };

    sendMsg(new FlpConfig(*this, *mIzatApi));
}

/*------------------------------------------------
  Client Management APIs
  Invoked from Client Thread.
-------------------------------------------------*/
void
FlpLocationAdapter::addClientCommand(
        LocationAPI* client, const LocationCallbacks& callbacks)
{
    LOC_LOGM1("addClientCommand() %p", client);

    struct MsgAddClient : public LocMsg {
        FlpLocationAdapter& mAdapter;
        LocationAPI* mClient;
        const LocationCallbacks mCallbacks;
        inline MsgAddClient(FlpLocationAdapter& adapter,
                            LocationAPI* client,
                            const LocationCallbacks& callbacks) :
            LocMsg(),
            mAdapter(adapter),
            mClient(client),
            mCallbacks(callbacks) {}
        inline virtual void proc() const {
            mAdapter.saveClient(mClient, mCallbacks);
        }
    };

    sendMsg(new MsgAddClient(*this, client, callbacks));
}

void
FlpLocationAdapter::removeClientCommand(LocationAPI* client)
{
    LOC_LOGM1("removeClientCommand() %p", client);

    struct MsgRemoveClient : public LocMsg {
        FlpLocationAdapter& mAdapter;
        LocationAPI* mClient;
        inline MsgRemoveClient(FlpLocationAdapter& adapter,
                               LocationAPI* client) :
            LocMsg(),
            mAdapter(adapter),
            mClient(client) {}
        inline virtual void proc() const {
            mAdapter.stopClientSessions(mClient);
            mAdapter.eraseClient(mClient);
        }
    };

    sendMsg(new MsgRemoveClient(*this, client));
}

void
FlpLocationAdapter::setControlCallbacksCommand(LocationControlCallbacks& controlCallbacks)
{
    LOC_LOGL0("setControlCallbacksCommand()");
    struct MsgSetControlCallbacks : public LocMsg {
        FlpLocationAdapter& mAdapter;
        const LocationControlCallbacks mControlCallbacks;
        inline MsgSetControlCallbacks(FlpLocationAdapter& adapter,
            LocationControlCallbacks& controlCallbacks) :
            LocMsg(),
            mAdapter(adapter),
            mControlCallbacks(controlCallbacks) {}
        inline virtual void proc() const {
            mAdapter.setControlCallbacks(mControlCallbacks);
        }
    };

    sendMsg(new MsgSetControlCallbacks(*this, controlCallbacks));
}

void
FlpLocationAdapter::saveClient(
        LocationAPI* client, const LocationCallbacks& callbacks)
{
    LOC_LOGL1("saveClient() %p", client);

    LocationClientInfo info = { callbacks, false, false, 0, 0 };
    mClientData[client] = info;
}

void
FlpLocationAdapter::eraseClient(LocationAPI* client)
{
    LOC_LOGL1("eraseClient() %p", client);

    ClientDataMap::iterator it = mClientData.find(client);
    if (it != mClientData.end()) {
        mClientData.erase(it);
    }
    //updateClientsEventMask();
}

void
FlpLocationAdapter::updateClientLoadSuccess(
        LocationAPI* client, bool isEngineLoadSuccessReported)
{
    ClientDataMap::iterator it = mClientData.find(client);
    if (it != mClientData.end())
    {
        LocationClientInfo& clientInfo = it->second;
        clientInfo.isEngineLoadSuccessReported = isEngineLoadSuccessReported;
    }
}
void
FlpLocationAdapter::updateClientLoadFailure(
        LocationAPI* client, bool isEngineLoadFailureReported)
{
    ClientDataMap::iterator it = mClientData.find(client);
    if (it != mClientData.end())
    {
        LocationClientInfo& clientInfo = it->second;
        clientInfo.isEngineLoadFailureReported = isEngineLoadFailureReported;
    }
}

void
FlpLocationAdapter::stopClientSessions(LocationAPI* client)
{
    LOC_LOGH1("stopClientSessions() %p", client);

    for (LocationSessionMap::iterator it =
            mTimeBasedTrackingSessions.begin();
              it != mTimeBasedTrackingSessions.end(); /* no increment here*/) {
        if (client == it->first.client) {
            LocationError err =
                    stopTimeBasedTrackingMultiplex(it->first.client, it->first.id);
            if (LOCATION_ERROR_SUCCESS == err) {
                mTimeBasedTrackingSessions.erase(it++);
                continue;
            } else {
                LOC_LOGE1("stopTimeBasedTrackingMultiplex() failed, err %d", err);
            }
        }
        ++it; // increment only when not erasing an iterator
    }
    for (LocationSessionMap::iterator it =
            mDistanceBasedTrackingSessions.begin();
              it != mDistanceBasedTrackingSessions.end(); /* no increment here*/) {
        if (client == it->first.client) {
            LocationError err = mIzatApi->stopDistanceBasedTracking(it->first.id);
            if (LOCATION_ERROR_SUCCESS == err) {
                mDistanceBasedTrackingSessions.erase(it++);
                continue;
            } else {
                LOC_LOGE1("stopDistanceBasedTracking() failed, err %d", err);
            }
        }
        ++it; // increment only when not erasing an iterator
    }
    for (LocationSessionMap::iterator it = mBatchingSessions.begin();
              it != mBatchingSessions.end(); /* no increment here*/) {
        if (client == it->first.client) {
            LocationError err = mIzatApi->stopBatching(it->first.id);
            if (LOCATION_ERROR_SUCCESS == err) {
                mBatchingSessions.erase(it++);
                continue;
            } else {
                LOC_LOGE1("stopBatching() failed, err %d", err);
            }
        }
        ++it; // increment only when not erasing an iterator
    }
    for (MotionSessionMap::iterator it = mMotionTrackingSessions.begin();
              it != mMotionTrackingSessions.end(); /* no increment here*/) {

        if (client == it->first.client) {

            LocationMotionOptions options = it->second;
            mMotionTrackingSessions.erase(it++);

#ifndef SENSORS_DISABLED
            // Disable AMD only if no session is remaining
            if (!anyMotionTrackingSessionActive()) {
                mSlimClient->disableAmdDetection();
            }

            // Disable SMD if no interested client other than this one
            if (options.motionTypeMask & LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT) {
                if (!anyMotionTrackingSessionActive(NULL, LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT)) {
                    mSlimClient->disableSmdDetection();
                }
            }
#endif
        } else {
            ++it; // increment only when not erasing an iterator
        }
    }
}

void
FlpLocationAdapter::stopAllClientSessions() {

    LocationAPI* client = NULL;
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    LOC_LOGH0("stopAllClientSessions()");

    /* Stop Time based tracking session */
    if (anyTimeBasedTrackingSessionActive() || anySingleShotSessionActive()) {
        err = mIzatApi->stopTimeBasedTracking();
        if (LOCATION_ERROR_SUCCESS != err) {
            LOC_LOGE1("stopTimeBasedTracking failed, err %d", err);
        }
    }

    /* Stop all DBT and Batching sessions for all clients */
    for (ClientDataMap::iterator itr=mClientData.begin();
            itr != mClientData.end(); ++itr) {

        client = itr->first;

        /* Stop all DBT sessions */
        for (LocationSessionMap::iterator it =
                mDistanceBasedTrackingSessions.begin();
                  it != mDistanceBasedTrackingSessions.end(); ++it) {
            if (client == it->first.client) {
                LocationError err = mIzatApi->stopDistanceBasedTracking(it->first.id);
                if (LOCATION_ERROR_SUCCESS != err) {
                    LOC_LOGE2("stopDistanceBasedTracking() failed, "
                            "client %p err %d", client, err);
                }
            }
        }

        /* Stop all batching sessions */
        for (LocationSessionMap::iterator it = mBatchingSessions.begin();
                  it != mBatchingSessions.end(); ++it) {
            if (client == it->first.client) {
                LocationError err = mIzatApi->stopBatching(it->first.id);
                if (LOCATION_ERROR_SUCCESS != err) {
                    LOC_LOGE2("stopBatching() failed, client %p err %d",
                            client, err);
                }
            }
        }
    }

#ifndef SENSORS_DISABLED
    /* Stop Motion Tracking sessions if any active */
    if (anyMotionTrackingSessionActive()) {
        mSlimClient->disableAmdDetection();
    }
    if (anyMotionTrackingSessionActive(NULL, LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT)) {
        mSlimClient->disableSmdDetection();
    }
#endif
}

/*------------------------------------------------
  SINGLE SHOT APIs
  Invoked from Client Thread
-------------------------------------------------*/
uint32_t
FlpLocationAdapter::getSingleShotCommand(
        LocationAPI* client, const SingleShotOptions& options,
        IzatAdapterBase* requestorAdapter, SingleshotRequestType requestType) {

    uint32_t sessionId = generateSessionId();

    LOC_LOGH6("getSingleShotCommand(): "
            "client %p, id %u, power %u, accuracy %d, timeout %d adapter %p",
            client, sessionId, options.powerLevel, options.accuracyLevel,
            options.timeout, requestorAdapter);

    struct MsgGetSingleShot : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        SingleShotOptions mOptions;
        uint32_t mWwanAccuracyThreshold;
        IzatAdapterBase* mRequestorAdapter;
        SingleshotRequestType mRequestType;
        inline MsgGetSingleShot(FlpLocationAdapter& adapter,
                               IzatApiBase& api,
                               LocationAPI* client,
                               uint32_t sessionId,
                               SingleShotOptions options,
                               uint32_t wwanAccuracyThreshold,
                               IzatAdapterBase* requestorAdapter,
                               SingleshotRequestType requestType) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId),
            mOptions(options),
            mWwanAccuracyThreshold(wwanAccuracyThreshold),
            mRequestorAdapter(requestorAdapter),
            mRequestType(requestType) {}
        inline virtual ~MsgGetSingleShot(){}

        inline virtual void proc() const {

            /* Download XTRA Integrity before starting any GNSS session.
             * Only for the first time this message is posted, not for retries via sendPendingMsg. */
            if (!mIsPendingMsg) {
                if (LOCATION_POWER_HIGH == mOptions.powerLevel) {
                    uint32_t waitTime = mAdapter.getXtraIntDloadWaitTimeSec();
                    if (waitTime > 0) {
                        LOC_LOGH1("FLP Get Singleshot pending on XTRA INT, wait %d sec", waitTime);
                        mAdapter.sendPendingMsg(new MsgGetSingleShot(
                                mAdapter, mApi, mClient, mSessionId, mOptions, mWwanAccuracyThreshold,
                                mRequestorAdapter, mRequestType));
                        if (!mAdapter.mPendingMsgTimer.isStarted()) {
                            mAdapter.triggerXtraIntDload();
                            mAdapter.mPendingMsgTimer.start(waitTime);
                        } else  {
                            LOC_LOGH0("Pending msg timer already running");
                        }
                        return;
                    }
                }
            }

            mAdapter.getSingleShot(mClient, mSessionId, mOptions, mRequestorAdapter, mRequestType);
        }
    };

    sendMsg(new MsgGetSingleShot(*this, *mIzatApi, client, sessionId, options,
                                    mConfig.wwanFixAccuracyThreshold, requestorAdapter, requestType));
    return sessionId;
}

void
FlpLocationAdapter::getSingleShot(
        LocationAPI* client, uint32_t sessionId, SingleShotOptions options,
        IzatAdapterBase* requestorAdapter, SingleshotRequestType requestType) {

    LocationError err = LOCATION_ERROR_SUCCESS;

    if ((NULL == requestorAdapter && NULL == client) ||
            (NULL != requestorAdapter && NULL != client)) {
        LOC_LOGE0("Invalid requestor and client combination");
        err = LOCATION_ERROR_INVALID_PARAMETER;
    } else if (NULL == requestorAdapter && !hasSingleShotCallback(client)) {
        LOC_LOGE0("Single shot callback missing");
        err = LOCATION_ERROR_CALLBACK_MISSING;
    } else if (0 == options.size) {
        LOC_LOGE0("Options size 0");
        err = LOCATION_ERROR_INVALID_PARAMETER;
    } else if (0 == options.timeout) {
        LOC_LOGE0("Single shot timeout 0");
        err = LOCATION_ERROR_INVALID_PARAMETER;
    }

    Location zpp = {};
    bool reportZpp = false;

    if (LOCATION_ERROR_SUCCESS == err) {

        // Don't forward request to providers if a matching options
        // request is already in progress ..
        if (!anyMatchingSingleShotSessionActive(options)) {

            if (SINGLESHOT_REQUEST_TYPE_INVALID == requestType) {
                requestType = getSingleShotRequestType(options);
            }

            if (SINGLESHOT_REQUEST_TYPE_GTPWWAN_1_0 == requestType) {

                // Respond immediately if ZPP satisfies the threshold
                if (0 == mConfig.noZppBeforeWwan) {
                    err = mIzatApi->getZppFix(zpp);
                }
                if (LOCATION_ERROR_SUCCESS == err &&
                        meetsAccuracyThreshold(zpp, options)) {
                    reportZpp = true;
                } else {

                    // Else, request for wwan fix
                    GtpOptions gtpOptions = {};
                    gtpOptions.size = sizeof(GtpOptions);
                    gtpOptions.timeout = 0;
                    err = mIzatApi->getWwanZppFix(gtpOptions);
                    if (LOCATION_ERROR_SUCCESS == err) {
                        mIsSingleshotWwanCbPending = true;
                    }
                }

            } else if (SINGLESHOT_REQUEST_TYPE_GTPWWAN_2_0 == requestType) {

                if(mGtpWwanNvItems == NULL)
                {
                    mGtpWwanNvItems = new GtpWwanNvItems();
                    if(mGtpWwanNvItems ==  NULL)
                        return;
                }
                uint64_t currenttime = LocOSIface_getSecondsSinceEpoch();
                uint64_t blacklisttill = mGtpWwanNvItems->getBlackListTime();

                if((blacklisttill != INVALID_TIME) && (currenttime < blacklisttill))
                {
                    LOC_LOGE1("Device is blaclisted till %ld sec"
                              "abort GTP 2.0 request",blacklisttill - currenttime);
                    if (NULL != client) {
                        reportResponse(client, LOCATION_ERROR_THRESHOLD_REACHED, sessionId);
                    }
                    return;
                }
                GtpOptions gtpOptions = {};
                gtpOptions.size = sizeof(GtpOptions);
                if ((options.timeout/1000) > 255)
                {
                    gtpOptions.timeout = 255;
                }
                else
                {
                    gtpOptions.timeout = (options.timeout/1000);
                }
                LOC_LOGL1("gtpOptions timeout %d", gtpOptions.timeout);
                err = mIzatApi->getWwanZppFix(gtpOptions);
                if (LOCATION_ERROR_SUCCESS == err) {
                    mIsSingleshotMcsCbPending = true;
                }

            } else if (SINGLESHOT_REQUEST_TYPE_WIFI == requestType) {

                bool success = mIzatApi->requestAdapterGetSingleshot(
                        options, SINGLESHOT_REQUEST_TYPE_WIFI, this);
                if (success) {
                    mIsSingleshotWifiCbPending = true;
                }

            } else if (SINGLESHOT_REQUEST_TYPE_GNSS == requestType) {

                // Just trigger time based session
                // We don't add the session to time based tracking sessions
                // The session gets added to single shot sessions list only.
                SingleShotOptions opt = options;
                if(opt.accuracyLevel == LOCATION_ACCURACY_UNKNOWN) {
                    opt.accuracyLevel = mConfig.accuracy;
                }
                // Start session with slightly higher timeout
                // To avoid race condition between timeout timer expiry vs engine reported timeout.
                opt.timeout += 500;
                mIsGnssLoaded = false;
                err = mIzatApi->getSingleShot(opt);
                if (LOCATION_ERROR_SUCCESS == err) {
                    mIsSingleshotGnssCbPending = true;
                }
            } else {

                LOC_LOGE1("Invalid singleshot request type %d", requestType);
            }
        } else {

            LOC_LOGH0("Matching singleshot session already active.");
                }
            }

    if (LOCATION_ERROR_SUCCESS == err) {

        saveSingleShotSession(
                client, sessionId, options, requestorAdapter, requestType);
        // No need for timeout monitoring if reporting zpp
        // or for an internal request
        if (!reportZpp) {
            mSingleShotTimer.start();
        }
    } else {

        LOC_LOGE1("Failed to start singleshot session, err %d", err);
    }

    if (NULL != client) {
        reportResponse(client, err, sessionId);
        updateClientLoadSuccess(client, false);
        updateClientLoadFailure(client, false);
    }

    if (reportZpp) {
        reportWwanZppFix(zpp, LOCATION_ERROR_SUCCESS, true);
    }
}


void
FlpLocationAdapter::cancelSingleShotCommand(
        LocationAPI* client, uint32_t id) {

    LOC_LOGH2("cancelSingleShotCommand() client %p id %u", client, id);

    struct MsgCancelSingleShot : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        inline MsgCancelSingleShot(FlpLocationAdapter& adapter,
                               IzatApiBase& api,
                               LocationAPI* client,
                               uint32_t sessionId) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId) {}
        inline virtual void proc() const {

            LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
            SingleShotOptions options = {0};

            mAdapter.getSingleShotSessionOptions(mClient, mSessionId, options);

            if (0 == options.size) {
                LOC_LOGE1("No singleshot session with id: %d", mSessionId);
                mAdapter.reportResponse(mClient, LOCATION_ERROR_ID_UNKNOWN, mSessionId);
                return;
            }

            // Erase session before multiplexing,
            // So that we restart tracking if this is the last singleshot session.
            mAdapter.eraseSingleShotSession(mClient, mSessionId);
            err = LOCATION_ERROR_SUCCESS;

            if (!mAdapter.anySingleShotGnssSessionActive()) {
                if (mAdapter.anyTimeBasedTrackingSessionActive()) {
                    err = mAdapter.restoreTimeBasedTrackingMultiplex();
                } else {
                    err = mApi.stopTimeBasedTracking();
                }
            }

            // Stop timer if no more sessions
            if (!mAdapter.anySingleShotSessionActive()) {
                mAdapter.mSingleShotTimer.stop();
            }

            mAdapter.reportResponse(mClient, err, mSessionId);
        }
    };

    sendMsg(new MsgCancelSingleShot(*this, *mIzatApi, client, id));
}

bool
FlpLocationAdapter::requestAdapterGetSingleshot(
        const SingleShotOptions& options, SingleshotRequestType requestType,
        IzatAdapterBase* requestorAdapter) {

    LOC_LOGH2("requestAdapterGetSingleshot() :: type %d, requestor %p",
            requestType, requestorAdapter);

    if (SINGLESHOT_REQUEST_TYPE_GNSS == requestType) {

        SingleShotOptions opt = options;
        if (opt.timeout == 0) {
            opt.timeout = mConfig.singleShotTimeout;
        }
        getSingleShotCommand(NULL, opt, requestorAdapter);
        return true;
    }

    return false;
}

void FlpLocationAdapter::wifiAdapterSingleshotCb(
        const Location& location, LocationError err) {

    LOC_LOGSM7("%s: wifi pos: latitude: %d.%d,longitude: %d.%d hpe %d err %d", __FUNCTION__,
                         (int)location.latitude, ((int)(location.latitude*100000))%100000,
                         (int)location.longitude, ((int)(location.longitude*100000))%100000,
                         location.accuracy, err);

    mIsSingleshotWifiCbPending = false;

    // Fetch Best available position as well
    Location* selectedFix = &location;
    Location zpp = {};
    if (LOCATION_ERROR_SUCCESS == mIzatApi->getZppFix(zpp)) {
        if (zpp.accuracy > 0 && zpp.accuracy < location.accuracy) {
            selectedFix = &zpp;
        }
    } else if (LOCATION_ERROR_SUCCESS == err) {
        LOC_LOGE0("Failed to get ZPP fix, check WLAN fix..");
    } else {
        LOC_LOGE0("WLAN and ZPP both failed !");
        return;
    }

    // Check if accuracy is acceptable
    LOC_LOGL2("ZPP accuracy %d WIFI accuracy %d", (int)zpp.accuracy, (int)location.accuracy);

    // Iterate over all singleshot sessions
    for (SingleShotSessionMap::iterator it = mSingleShotSessions.begin();
            it != mSingleShotSessions.end(); /* Increment while erasing */) {

        // process the session
        LocationSessionKey key = it->first;
        SingleShotSessionInfo& info = it->second;
        singleShotCallback singleShotCb = getSingleShotCallback(key.client);

        // Report WIFI fix if requested by client
        if (LOCATION_POWER_MED == info.options.powerLevel) {

            info.lastRecvdLocation = *selectedFix;

            if (meetsAccuracyThreshold(*selectedFix, info.options)) {

                LOC_LOGM1("Accuracy acceptable: %d", (uint32_t)selectedFix->accuracy);
                reportMetaDataEngineStatus(LOCATION_ENGINE_STATUS_FIX_SUCCESS);
                if (singleShotCb) {
                    LOC_LOGL2("Single Shot callback for client %p session %d",
                               key.client, key.id);
                    singleShotCb(key.client, *selectedFix, err);
                }
                mSingleShotSessions.erase(it++);

            } else {
                reportMetaDataEngineStatus(LOCATION_ENGINE_STATUS_POS_ACC_CHECK_FAILED);
                ++it;
            }

        } else {
            ++it;
        }
    }
}

/*------------------------------------------------
  GET BEST AVAILABLE POSITION
  Invoked from Client Thread
-------------------------------------------------*/
uint32_t
FlpLocationAdapter::getBestAvailablePositionCommand(LocationAPI* client) {

    uint32_t sessionId = generateSessionId();

    LOC_LOGH2("getBestAvailablePositionCommand(): "
              "client %p, id %u", client, sessionId);

    struct MsgGetBestAvailablePosition : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        inline MsgGetBestAvailablePosition(FlpLocationAdapter& adapter,
                               IzatApiBase& api,
                               LocationAPI* client,
                               uint32_t sessionId) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId) {}
        inline virtual ~MsgGetBestAvailablePosition(){}

        inline virtual void proc() const {

            LocationError err = LOCATION_ERROR_SUCCESS;

            if (!mAdapter.hasSingleShotCallback(mClient)) {
                LOC_LOGE0("Single shot callback missing");
                err = LOCATION_ERROR_CALLBACK_MISSING;
            }

            Location location = {};
            if (LOCATION_ERROR_SUCCESS == err) {
                err = mApi.getZppFix(location);
                if (LOCATION_ERROR_SUCCESS != err) {
                    LOC_LOGE1("Failed to get ZPP fix, err %d", err);
                }
            }

                singleShotCallback singleShotCb = mAdapter.getSingleShotCallback(mClient);
            if (NULL == singleShotCb) {
                err = LOCATION_ERROR_CALLBACK_MISSING;
            }
            mAdapter.reportResponse(mClient, err, mSessionId);

            if (LOCATION_ERROR_SUCCESS != err) {
                err = LOCATION_ERROR_BEST_AVAIL_POS_INVALID;
            }

                if (NULL != singleShotCb) {
                    if (0 == location.timestamp && 0 != LocOSIface_getMillisUTC()) {
                        LOC_LOGM0("No timestamp in best avail pos, fill in system time");
                        location.timestamp = LocOSIface_getMillisUTC();
                    }
                    location.flags |= LOCATION_IS_BEST_AVAIL_POS_BIT;
                    singleShotCb(mClient, location, err);
                } else {
                LOC_LOGE0("Failed to get singleShotcb for client");
            }
        }
    };

    sendMsg(new MsgGetBestAvailablePosition(*this, *mIzatApi, client, sessionId));
    return sessionId;
}

/*------------------------------------------------
  TRACKING APIs
  Invoked from Client Thread
-------------------------------------------------*/
uint32_t
FlpLocationAdapter::startTrackingCommand(
        LocationAPI* client, 
        LocationOptions& options,
        bool isGnssDataRequested)
{
    uint32_t sessionId = generateSessionId();

    LOC_LOGH5("startTrackingCommand(): "
            "client %p id %u minInterval %u minDistance %u mode %u",
            client, sessionId, options.minInterval, options.minDistance,
            options.suplMode);

    struct MsgStartTracking : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        LocationOptions mOptions;
        bool mIsGnssDataRequested;
        inline MsgStartTracking(FlpLocationAdapter& adapter,
                               IzatApiBase& api,
                               LocationAPI* client,
                               uint32_t sessionId,
                               LocationOptions options,
                               bool isGnssDataRequested) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId),
            mOptions(options),
            mIsGnssDataRequested(isGnssDataRequested) {}
        inline virtual ~MsgStartTracking(){}

        inline virtual void proc() const {

            /* Download XTRA Integrity before starting any session.
             * Only for the first time this message is posted, not for retries via sendPendingMsg */
            if (!mIsPendingMsg) {
                uint32_t waitTime = mAdapter.getXtraIntDloadWaitTimeSec();
                if (waitTime > 0) {
                    LOC_LOGH1("FLP Start Tracking pending on XTRA INT, wait %d sec", waitTime);
                    mAdapter.sendPendingMsg(new MsgStartTracking(
                            mAdapter, mApi, mClient, mSessionId, mOptions, mIsGnssDataRequested));
                    if (!mAdapter.mPendingMsgTimer.isStarted()) {
                        mAdapter.triggerXtraIntDload();
                        mAdapter.mPendingMsgTimer.start(waitTime);
                    } else  {
                        LOC_LOGH0("Pending msg timer already running");
                    }
                    return;
                }
            }

            LocationError err = LOCATION_ERROR_SUCCESS;
            if (!mAdapter.hasTrackingCallback(mClient)) {
                LOC_LOGE0("No tracking callback.");
                err = LOCATION_ERROR_CALLBACK_MISSING;
            } else if (0 == mOptions.size) {
                LOC_LOGE0("Options size 0.");
                err = LOCATION_ERROR_INVALID_PARAMETER;
            }

            if (LOCATION_ERROR_SUCCESS == err) {
                if (mOptions.minDistance > 0) {
                    err =  mApi.startDistanceBasedTracking(mSessionId, mOptions);
                } else {
                    // Api doesn't support multiple clients for time based tracking, so mutiplex
                    if (mIsGnssDataRequested) {
                        mAdapter.incrementGnssDataSessionCount();
                    }
                    err = mAdapter.startTimeBasedTrackingMultiplex(mOptions);
                }

                ExtendedLocationOptions extLocOptions;
                extLocOptions.locationOptions = mOptions;
                extLocOptions.isGnssDataRequested = mIsGnssDataRequested;
                mAdapter.saveTrackingSession(mClient, mSessionId, extLocOptions);
            }

            mAdapter.reportResponse(mClient, err, mSessionId);

            // Set flag to report engine load status again
            mAdapter.updateClientLoadSuccess(mClient, false);
            mAdapter.updateClientLoadFailure(mClient, false);
        }
    };

    sendMsg(new MsgStartTracking(*this, *mIzatApi, client, sessionId, options, isGnssDataRequested));
    return sessionId;
}

LocationError
FlpLocationAdapter::startTimeBasedTrackingMultiplex(const LocationOptions& options)
{
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    LOC_LOGH0("startTimeBasedTrackingMultiplex()");

    if (anySingleShotGnssSessionActive()) {

        LOC_LOGH0("Single shot request active, ignore start multiplex request.");
        return LOCATION_ERROR_SUCCESS;
    }

    if (mTimeBasedTrackingSessions.empty()) {
        LOC_LOGM0("No existing sessions, just start.");
        mIsGnssLoaded = false;
        err = mIzatApi->startTimeBasedTracking(
                options, (LocationAccuracyLevel)mConfig.accuracy);
        if (LOCATION_ERROR_SUCCESS != err) {
            LOC_LOGE1("startTimeBasedTracking failed, err %d", err);
        }
    } else {
        LOC_LOGM0("Existing sessions, multiplex.");
        // get the LocationOptions that has the smallest interval, which should be the active one
        LocationOptions smallestIntervalOptions = {}; // size is zero until set for the first time
        LocationAccuracyLevel highestAccuracy = LOCATION_ACCURACY_UNKNOWN;
        for (LocationSessionMap::iterator it =
                mTimeBasedTrackingSessions.begin();
                  it != mTimeBasedTrackingSessions.end(); ++it) {
            if (0 == smallestIntervalOptions.size || //size of zero means we havent set it yet
                it->second.locationOptions.minInterval < smallestIntervalOptions.minInterval)
            {
                 smallestIntervalOptions = it->second.locationOptions;
            }
            // Find highest requested accuracy in all sessions
            if (it->second.locationOptions.accuracyLevel > highestAccuracy)
            {
                highestAccuracy = it->second.locationOptions.accuracyLevel;
            }
        }
        // if new session's minInterval is smaller than any in other sessions
        if (options.minInterval < smallestIntervalOptions.minInterval ||
                options.accuracyLevel > highestAccuracy)
        {
            // restart time based tracking with new options
            LocationOptions newOptions = options;
            newOptions.minInterval = std::min(options.minInterval, smallestIntervalOptions.minInterval);
            newOptions.accuracyLevel = std::max(options.accuracyLevel, highestAccuracy);

            LOC_LOGM3("Restarting tracking session, tbf %d dist %d acc %d",
                    newOptions.minInterval, newOptions.minDistance, newOptions.accuracyLevel);

            mIsGnssLoaded = false;
            err = mIzatApi->startTimeBasedTracking(
                    newOptions, (LocationAccuracyLevel)mConfig.accuracy);
        } else {
            err = LOCATION_ERROR_SUCCESS;
        }
    }

    return err;
}

LocationError
FlpLocationAdapter::updateTimeBasedTrackingMultiplex(
        const LocationOptions& oldOptions, const LocationOptions& newOptions)
{
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    LOC_LOGH0("updateTimeBasedTrackingMultiplex()");

    if (anySingleShotGnssSessionActive()) {

        LOC_LOGH0("Single shot request active, ignore update multiplex request.");
        return LOCATION_ERROR_SUCCESS;
    }

    // get the LocationOptions that has the smallest interval, which should be the active one
    LocationOptions smallestIntervalOptions = {}; // size is zero until set for the first time
    LocationAccuracyLevel highestAccuracy = LOCATION_ACCURACY_UNKNOWN;
    for (LocationSessionMap::iterator it = mTimeBasedTrackingSessions.begin();
              it != mTimeBasedTrackingSessions.end(); ++it)
    {
        if (0 == smallestIntervalOptions.size || //size of zero means we havent set it yet
            it->second.locationOptions.minInterval < smallestIntervalOptions.minInterval) {
             smallestIntervalOptions = it->second.locationOptions;
        }
        // Find highest requested accuracy in all sessions
        if (it->second.locationOptions.accuracyLevel > highestAccuracy)
        {
            highestAccuracy = it->second.locationOptions.accuracyLevel;
    }
    }
    // if new session's minInterval is smaller than any in other sessions
    if (smallestIntervalOptions.size == 0 ||
            newOptions.minInterval < smallestIntervalOptions.minInterval ||
            oldOptions.minInterval < smallestIntervalOptions.minInterval ||
            newOptions.accuracyLevel > highestAccuracy ||
            oldOptions.accuracyLevel > highestAccuracy)
    {
        // restart time based tracking with new options
        LocationOptions options = newOptions;
        if (smallestIntervalOptions.size > 0)
        {
            options.minInterval = std::min(options.minInterval, smallestIntervalOptions.minInterval);
        }
        options.accuracyLevel = std::max(options.accuracyLevel, highestAccuracy);

        LOC_LOGM3("Restarting tracking session, tbf %d dist %d acc %d",
                options.minInterval, options.minDistance, options.accuracyLevel);

        mIsGnssLoaded = false;
        err = mIzatApi->startTimeBasedTracking(
                options, (LocationAccuracyLevel)mConfig.accuracy);
    } else {
        err = LOCATION_ERROR_SUCCESS;
    }

    return err;
}

void
FlpLocationAdapter::updateTrackingOptionsCommand(
        LocationAPI* client, uint32_t id, LocationOptions& options)
{
    LOC_LOGH5("updateTrackingOptionsCommand() "
            "client %p id %u minInterval %u minDistance %u acc %u",
             client, id, options.minInterval, options.minDistance,
             options.accuracyLevel);

    struct MsgUpdateTracking : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        LocationOptions mOptions;
        inline MsgUpdateTracking(FlpLocationAdapter& adapter,
                                IzatApiBase& api,
                                LocationAPI* client,
                                uint32_t sessionId,
                                LocationOptions options) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId),
            mOptions(options) {}
        inline virtual void proc() const {

            if (mAdapter.isSingleShotSession(mClient, mSessionId)) {
                LOC_LOGE0("Update not allowed for singleshot session.");
                return;
            }

            bool isTimeBased = mAdapter.isTimeBasedTrackingSession(mClient, mSessionId);
            bool isDistanceBased = mAdapter.isDistanceBasedTrackingSession(mClient, mSessionId);

            if (isTimeBased || isDistanceBased) {

                LocationError err = LOCATION_ERROR_SUCCESS;
                if (0 == mOptions.size) {
                    err = LOCATION_ERROR_INVALID_PARAMETER;
                } else {
                    if (isTimeBased && mOptions.minDistance > 0) {

                        // switch from time based to distance based
                        // Api doesn't support multiple clients for time based tracking, so mutiplex
                        err = mAdapter.stopTimeBasedTrackingMultiplex(mClient, mSessionId);
                        if (LOCATION_ERROR_SUCCESS == err) {
                            mAdapter.eraseTrackingSession(mClient, mSessionId);
                            err = mApi.startDistanceBasedTracking(mSessionId, mOptions);
                        }

                    } else if (isDistanceBased && mOptions.minDistance == 0) {

                        // switch from distance based to time based
                        err = mApi.stopDistanceBasedTracking(mSessionId);
                        if (LOCATION_ERROR_SUCCESS == err) {
                            mAdapter.eraseTrackingSession(mClient, mSessionId);
                            // Api doesn't support multiple clients for time based tracking, so mutiplex
                            err = mAdapter.startTimeBasedTrackingMultiplex(mOptions);
                        }

                    } else if (isTimeBased) {

                        // Backup current session options, in case required for restore
                        LocationSessionKey key(mClient, mSessionId);
                        ExtendedLocationOptions oldExtOptions = {};
                        mAdapter.getTrackingSessionOptions(mClient, mSessionId, oldExtOptions);

                        // Delete current session so its not considered in multiplexing
                        mAdapter.eraseTrackingSession(mClient, mSessionId);

                        // Api doesn't support multiple clients for time based tracking, so mutiplex
                        err = mAdapter.updateTimeBasedTrackingMultiplex(
                                oldExtOptions.locationOptions, mOptions);

                        // Restore old options which we erased, if start tracking fails
                        if (err != LOCATION_ERROR_SUCCESS) {
                            LOC_LOGE1("updateTimeBasedTrackingMultiplex failed, err %d", err);
                            if (oldExtOptions.locationOptions.size > 0) {
                                mAdapter.saveTrackingSession(mClient, mSessionId, oldExtOptions);
                            }
                        }

                    } else if (isDistanceBased) {

                        // restart distance based tracking
                        err = mApi.stopDistanceBasedTracking(mSessionId);
                        if (LOCATION_ERROR_SUCCESS == err) {
                            err = mApi.startDistanceBasedTracking(mSessionId, mOptions);
                        }
                    }
                }
                if (LOCATION_ERROR_SUCCESS == err) {
                    ExtendedLocationOptions extOptions;
                    extOptions.locationOptions = mOptions;
                    extOptions.isGnssDataRequested = 0;
                    mAdapter.saveTrackingSession(mClient, mSessionId, extOptions);
                }
                mAdapter.reportResponse(mClient, err, mSessionId);
            } else {
                LOC_LOGE2("Unknown session ID %d for client %p", mSessionId, mClient);
                mAdapter.reportResponse(mClient, LOCATION_ERROR_ID_UNKNOWN, mSessionId);
            }
        }
    };

    sendMsg(new MsgUpdateTracking(*this, *mIzatApi, client, id, options));
}

void
FlpLocationAdapter::stopTrackingCommand(
    LocationAPI* client, uint32_t id, bool isGnssDataRequested)
{
    LOC_LOGH3("stopTrackingCommand() client %p id %u isGnssData %d", 
        client, id, isGnssDataRequested);

    struct MsgStopTracking : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        bool mIsGnssDataRequested;
        inline MsgStopTracking(FlpLocationAdapter& adapter,
                               IzatApiBase& api,
                               LocationAPI* client,
                               uint32_t sessionId,
                               bool isGnssDataRequested) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId),
            mIsGnssDataRequested(isGnssDataRequested) {}
        inline virtual void proc() const {
            bool isTimeBased = mAdapter.isTimeBasedTrackingSession(mClient, mSessionId);
            bool isDistanceBased = mAdapter.isDistanceBasedTrackingSession(mClient, mSessionId);
            if (isTimeBased || isDistanceBased) {
                LocationError err = LOCATION_ERROR_SUCCESS;

                if (isTimeBased) {
                    // Api doesn't support multiple clients for time based tracking, so mutiplex
                    err = mAdapter.stopTimeBasedTrackingMultiplex(mClient, mSessionId);
                    if (mIsGnssDataRequested) {
                        mAdapter.decrementGnssDataSessionCount();
                    }
                } else if (isDistanceBased) {
                    err = mApi.stopDistanceBasedTracking(mSessionId);
                }

                if (LOCATION_ERROR_SUCCESS == err) {
                    mAdapter.eraseTrackingSession(mClient, mSessionId);
                }
                mAdapter.reportResponse(mClient, err, mSessionId);
            } else {
                LOC_LOGE1("No session with id: %d", mSessionId);
                mAdapter.reportResponse(mClient, LOCATION_ERROR_ID_UNKNOWN, mSessionId);
            }
        }
    };

    sendMsg(new MsgStopTracking(*this, *mIzatApi, client, id, isGnssDataRequested));
}

LocationError
FlpLocationAdapter::stopTimeBasedTrackingMultiplex(LocationAPI* client, uint32_t id)
{
    LocationError err = LOCATION_ERROR_SUCCESS;

    LOC_LOGM0("stopTimeBasedTrackingMultiplex()");

    if (anySingleShotGnssSessionActive()) {

        LOC_LOGH0("Single shot request active, ignore stop multiplex request.");
        return LOCATION_ERROR_SUCCESS;
    }

    if (1 == mTimeBasedTrackingSessions.size()) {
        err = mIzatApi->stopTimeBasedTracking();
        if (LOCATION_ERROR_SUCCESS != err) {
            LOC_LOGE1("stopTimeBasedTracking failed, err %d", err);
        }
    } else {
        LocationSessionKey key(client, id);
        // find the next smallest interval, other than the session we are stopping
        LocationOptions smallestIntervalOptions; // size will be zero until set for the first time
        LocationAccuracyLevel highestAccuracy = LOCATION_ACCURACY_UNKNOWN;
        for (LocationSessionMap::iterator it =
                mTimeBasedTrackingSessions.begin();
                  it != mTimeBasedTrackingSessions.end(); ++it)
        {
            // if session is not the one we are stopping and either smallest interval is not set
            // or there is a new smallest interval, then set the new smallest interval
            if (it->first != key && (0 == smallestIntervalOptions.size ||
                it->second.locationOptions.minInterval < smallestIntervalOptions.minInterval)) {
                 smallestIntervalOptions = it->second.locationOptions;
            }
            // Find highest requested accuracy in all sessions
            if (it->first != key && it->second.locationOptions.accuracyLevel > highestAccuracy)
            {
                highestAccuracy = it->second.locationOptions.accuracyLevel;
            }
        }
        // get the session we are stopping
        LocationSessionMap::iterator it =
                mTimeBasedTrackingSessions.find(key);
        if (it != mTimeBasedTrackingSessions.end()) {
            // if session we are stopping has smaller interval then next smallest
            if (it->second.locationOptions.minInterval < smallestIntervalOptions.minInterval)
            {
                // restart time based tracking with next smallest interval
                LocationOptions newOptions = smallestIntervalOptions;
                newOptions.minInterval = std::min(newOptions.minInterval, smallestIntervalOptions.minInterval);
                newOptions.accuracyLevel = std::max(newOptions.accuracyLevel, highestAccuracy);

                LOC_LOGM3("Restarting tracking session, tbf %d dist %d acc %d",
                        newOptions.minInterval, newOptions.minDistance, newOptions.accuracyLevel);

                mIsGnssLoaded = false;
                err = mIzatApi->startTimeBasedTracking(
                        newOptions, (LocationAccuracyLevel)mConfig.accuracy);
            }
        }
    }

    return err;
}

LocationError
FlpLocationAdapter::restoreTimeBasedTrackingMultiplex()
{
    LOC_LOGH0("restoreTimeBasedTrackingMultiplex()");

    if (mTimeBasedTrackingSessions.empty()) {
        LOC_LOGM0("No existing sessions, nothing to do.");
        return LOCATION_ERROR_SUCCESS;
    }

    // get the LocationOptions that has the smallest interval, which should be the active one
    LocationOptions smallestIntervalOptions = {}; // size is zero until set for the first time
    LocationAccuracyLevel highestAccuracy = LOCATION_ACCURACY_UNKNOWN;
    for (LocationSessionMap::iterator it =
            mTimeBasedTrackingSessions.begin();
              it != mTimeBasedTrackingSessions.end(); ++it)
    {
        if (0 == smallestIntervalOptions.size || //size of zero means we havent set it yet
            it->second.locationOptions.minInterval < smallestIntervalOptions.minInterval) {
             smallestIntervalOptions = it->second.locationOptions;
        }
        // Find highest requested accuracy in all sessions
        if (it->second.locationOptions.accuracyLevel > highestAccuracy)
        {
            highestAccuracy = it->second.locationOptions.accuracyLevel;
    }
    }

    smallestIntervalOptions.accuracyLevel = std::max(smallestIntervalOptions.accuracyLevel, highestAccuracy);

    LOC_LOGM3("Restarting tracking session, tbf %d dist %d acc %d",
            smallestIntervalOptions.minInterval, smallestIntervalOptions.minDistance,
            smallestIntervalOptions.accuracyLevel);

    mIsGnssLoaded = false;
    return mIzatApi->startTimeBasedTracking(
            smallestIntervalOptions, (LocationAccuracyLevel)mConfig.accuracy);
}

/*------------------------------------------------
  TRACKING Utilities
  Invoked from Message Thread
-------------------------------------------------*/
bool
FlpLocationAdapter::hasTrackingCallback(LocationAPI* client)
{
    ClientDataMap::iterator it = mClientData.find(client);
    return (it != mClientData.end() && it->second.callbacks.trackingCb);
}

bool
FlpLocationAdapter::anyTrackingSessionActive(LocationAPI* client) {

    for (LocationSessionMap::iterator it = mTimeBasedTrackingSessions.begin();
            it != mTimeBasedTrackingSessions.end(); ++it) {
        if (client == it->first.client) {
            return true;
        }
    }
    for (LocationSessionMap::iterator it = mDistanceBasedTrackingSessions.begin();
            it != mDistanceBasedTrackingSessions.end(); ++it) {
        if (client == it->first.client) {
            return true;
        }
    }
    return false;
}

bool
FlpLocationAdapter::anyTimeBasedTrackingSessionActive() {

    return (mTimeBasedTrackingSessions.size() > 0);
}

bool
FlpLocationAdapter::isTimeBasedTrackingSession(
        LocationAPI* client, uint32_t sessionId)
{
    LocationSessionKey key(client, sessionId);
    return (mTimeBasedTrackingSessions.find(key) !=
            mTimeBasedTrackingSessions.end());
}

bool
FlpLocationAdapter::isDistanceBasedTrackingSession(
        LocationAPI* client, uint32_t sessionId)
{
    LocationSessionKey key(client, sessionId);
    return (mDistanceBasedTrackingSessions.find(key) !=
            mDistanceBasedTrackingSessions.end());
}

void
FlpLocationAdapter::saveTrackingSession(
        LocationAPI* client, uint32_t sessionId,
        const ExtendedLocationOptions& extOptions)
{
    LOC_LOGM2("saveTrackingSession() client %p id %u", client, sessionId);
    ExtendedLocationOptions localExtOptions;
    localExtOptions = extOptions;

    LocationSessionKey key(client, sessionId);

    if (extOptions.locationOptions.minDistance > 0) {
        localExtOptions.isGnssDataRequested = false;
        mDistanceBasedTrackingSessions[key] = localExtOptions;
    } else {
        mTimeBasedTrackingSessions[key] = localExtOptions;
    }
}

void
FlpLocationAdapter::eraseTrackingSession(
        LocationAPI* client, uint32_t sessionId)
{
    LOC_LOGM2("eraseTrackingSession() client %p id %u", client, sessionId);

    LocationSessionKey key(client, sessionId);
    LocationSessionMap::iterator it = mTimeBasedTrackingSessions.find(key);

    if (it != mTimeBasedTrackingSessions.end()) {
        mTimeBasedTrackingSessions.erase(it);
    } else {
        it = mDistanceBasedTrackingSessions.find(key);
        if (it != mDistanceBasedTrackingSessions.end()) {
            mDistanceBasedTrackingSessions.erase(it);
        }
    }
}

void
FlpLocationAdapter::getTrackingSessionOptions(
        LocationAPI* client, uint32_t sessionId, ExtendedLocationOptions& extOptions) {

    LOC_LOGM2("getTrackingSessionOptions() client %p id %u", client, sessionId);

    LocationSessionKey key(client, sessionId);
    LocationSessionMap::iterator it = mTimeBasedTrackingSessions.find(key);

    if (it != mTimeBasedTrackingSessions.end()) {
        extOptions = it->second;
    } else {
        it = mDistanceBasedTrackingSessions.find(key);
        if (it != mDistanceBasedTrackingSessions.end()) {
            extOptions = it->second;
        }
    }
}

/*------------------------------------------------
  SINGLESHOT Utilities
  Invoked from Message Thread
-------------------------------------------------*/
bool
FlpLocationAdapter::anySingleShotSessionActive() {

    return (mSingleShotSessions.size() > 0);
}

bool
FlpLocationAdapter::anySingleShotSessionActive(LocationAPI* client) {

    for (SingleShotSessionMap::iterator it = mSingleShotSessions.begin();
            it != mSingleShotSessions.end(); ++it) {
        if (client == it->first.client) {
            return true;
        }
    }
    return false;
}

bool
FlpLocationAdapter::anySingleShotGnssSessionActive() {

    for (SingleShotSessionMap::iterator it = mSingleShotSessions.begin();
            it != mSingleShotSessions.end(); ++it) {
        if (LOCATION_POWER_HIGH == it->second.options.powerLevel) {
            return true;
        }
    }
    return false;
}

bool
FlpLocationAdapter::anySingleShotWwanSessionActive() {

    for (SingleShotSessionMap::iterator it = mSingleShotSessions.begin();
            it != mSingleShotSessions.end(); ++it) {
        if (LOCATION_POWER_LOW == it->second.options.powerLevel) {
            return true;
        }
    }
    return false;
}

bool
FlpLocationAdapter::isSingleShotSession(
        LocationAPI* client, uint32_t sessionId)
{
    LocationSessionKey key(client, sessionId);
    SingleShotSessionMap::iterator it = mSingleShotSessions.find(key);
    return (it != mSingleShotSessions.end());
}

bool
FlpLocationAdapter::hasSingleShotCallback(LocationAPI* client)
{
    ClientDataMap::iterator it = mClientData.find(client);
    return (it != mClientData.end() && it->second.callbacks.singleShotCb);
}

singleShotCallback
FlpLocationAdapter::getSingleShotCallback(LocationAPI* client) {

    ClientDataMap::iterator it = mClientData.find(client);
    if (it == mClientData.end()) {
        return NULL;
    }
    return it->second.callbacks.singleShotCb;
}

bool
FlpLocationAdapter::isEngineLoadSuccessReported(LocationAPI* client)
{
    ClientDataMap::iterator it = mClientData.find(client);
    if (it == mClientData.end()) {
        return false;
    }
    return it->second.isEngineLoadSuccessReported;
}
bool
FlpLocationAdapter::isEngineLoadFailureReported(LocationAPI* client)
{
    ClientDataMap::iterator it = mClientData.find(client);
    if (it == mClientData.end()) {
        return false;
    }
    return it->second.isEngineLoadFailureReported;
}

void
FlpLocationAdapter::updateLastRecvdGnssLocation(Location location)
{
    for (SingleShotSessionMap::iterator it = mSingleShotSessions.begin();
            it != mSingleShotSessions.end(); ++it)
    {
        SingleShotSessionInfo& info = it->second;
        if (LOCATION_POWER_HIGH == info.options.powerLevel)
        {
            info.lastRecvdLocation = location;
        }
    }
}

void
FlpLocationAdapter::updateGnssSignalStrength(GnssSvNotification& svInfo)
{
    boolean isWeakGnssSignal = _isWeakSignalStrength(svInfo);

    // Update gnss signal status for all singleshot sessions
    for (SingleShotSessionMap::iterator it = mSingleShotSessions.begin();
            it != mSingleShotSessions.end(); ++it)
    {
        LocationSessionKey key = it->first;
        // Take info reference to edit the weak gnss field
        SingleShotSessionInfo& info = it->second;
        if (TRUE == info.isWeakGnssSignal)
        {
            info.isWeakGnssSignal = isWeakGnssSignal;
            LOC_LOGM3("client %p id %d isWeakGnss %d", key.client, key.id, info.isWeakGnssSignal);
        }
    }

    // Update gnss signal status for all tracking sessions
    uint64 now = LocOSIface_getMillisSincePowerup();

    // Report to all clients with meta data cb
    for (ClientDataMap::iterator it=mClientData.begin();
            it != mClientData.end(); ++it)
    {
        LocationAPI* client = it->first;
        LocationClientInfo& info = it->second;

        if (isWeakGnssSignal)
        {
            if (info.firstWeakSvTimestamp == 0)
            {
                info.firstWeakSvTimestamp = now;
            }

            if ((now - info.lastWeakSvTimestamp) > MAX_WEAK_GNSS_SV_REPORT_GAP_MILLIS)
            {
                // large gap from last weak SV info to this one, restart
                info.firstWeakSvTimestamp = 0;
            }
            else if ((now - info.firstWeakSvTimestamp) >= WEAK_GNSS_REPORTING_INTERVAL_MILLIS)
            {
                reportMetaDataEngineStatus(LOCATION_ENGINE_STATUS_WEAK_GNSS_SIGNALS, client);
                info.firstWeakSvTimestamp = 0;
            }

            info.lastWeakSvTimestamp = now;
        }
        else
        {
            // strong signals, restart
            info.firstWeakSvTimestamp = 0;
        }
    }
}

void
FlpLocationAdapter::saveSingleShotSession(
        LocationAPI* client, uint32_t sessionId,
        SingleShotOptions options, IzatAdapterBase* requestorAdapter,
        SingleshotRequestType requestType)
{
    LOC_LOGL4("saveSingleShotSession() client %p id %u requestor %p type %d",
            client, sessionId, requestorAdapter, requestType);

    LocationSessionKey key(client, sessionId);
    SingleShotSessionInfo info = {};
    uint64_t now = LocOSIface_getMillisSinceEpoch();
    info.sessionStartTime = now;
    info.options = options;
    info.requestorAdapter = requestorAdapter;
    info.requestType = requestType;
    info.isWeakGnssSignal = TRUE;
    mSingleShotSessions[key] = info;
}

void
FlpLocationAdapter::eraseSingleShotSession(
        LocationAPI* client, uint32_t sessionId)
{
    LOC_LOGL2("eraseSingleShotSession() client %p id %u", client, sessionId);

    LocationSessionKey key(client, sessionId);
    SingleShotSessionMap::iterator it = mSingleShotSessions.find(key);

    if (it != mSingleShotSessions.end()) {
        mSingleShotSessions.erase(it);
    }
}

void
FlpLocationAdapter::getSingleShotSessionOptions(
        LocationAPI* client, uint32_t sessionId, SingleShotOptions& options) {

    LOC_LOGM2("getSingleShotSessionOptions() client %p id %u", client, sessionId);

    LocationSessionKey key(client, sessionId);
    SingleShotSessionMap::iterator it = mSingleShotSessions.find(key);

    if (it != mSingleShotSessions.end()) {
        options = it->second.options;
    }
}

// Stop all single shot sessions across all clients
// This would be done once we have successfully invoked
// singleshot callbacks for all registered clients.
void FlpLocationAdapter::clearAllSingleShotSessions() {

    LOC_LOGL0("clearAllSingleShotSessions()");

    if (mSingleShotSessions.size() <= 0) {
        LOC_LOGL0("No singleshot sessions to clear.");
        return;
    }

    // Erase all sessions from the map
    mSingleShotSessions.clear();

    // Restart tracking sessions
    restoreTimeBasedTrackingMultiplex();

    // Kill the single shot timer
    mSingleShotTimer.stop();
}

void FlpLocationAdapter::singleShotTimerCallback() {

    LOC_LOGL0("singleShotTimerCallback()");

    // Flags derived based on state
    bool eraseSession = false;
    bool gnssSessionErased = false;

    // Iterate over all singleshot sessions
    for (SingleShotSessionMap::iterator it = mSingleShotSessions.begin();
            it != mSingleShotSessions.end(); /* Increment while erasing */) {

        // process the session
        LocationSessionKey key = it->first;
        SingleShotSessionInfo info = it->second;
        singleShotCallback singleShotCb = getSingleShotCallback(key.client);

        // Time elapsed since this session start
        uint64_t now = LocOSIface_getMillisSinceEpoch();
        uint64_t elapsed = now - info.sessionStartTime;

        uint32_t sessionTimeout = info.options.timeout;

        // If elapsed time is greater than timeout, terminate the session
        if (elapsed >= (uint64_t)sessionTimeout) {

            // Send callback to client
            if (singleShotCb) {

                // Check if ZPP satisfies the threshold
                Location zpp = {};
                if (LOCATION_ERROR_SUCCESS == mIzatApi->getZppFix(zpp) &&
                        meetsAccuracyThreshold(zpp, info.options))
                {
                    LOC_LOGH1("Providing best avail pos on singleshot timeout, "
                              "accuracy %d", zpp.accuracy);
                    singleShotCb(key.client, zpp, LOCATION_ERROR_SUCCESS);

                }
                else
                {
                    LOC_LOGE2("Single Shot timeout for client %p session %d",
                              key.client, key.id);

                    // Else, report Weak signal and timeout
                    if (SINGLESHOT_REQUEST_TYPE_GNSS == getSingleShotRequestType(info.options) &&
                            TRUE == info.isWeakGnssSignal && isEngineLoadSuccessReported(key.client))
                    {
                        reportMetaDataEngineStatus(LOCATION_ENGINE_STATUS_WEAK_GNSS_SIGNALS);
                    }
                    
                    if (info.lastRecvdLocation.size == sizeof(Location))
                    {
                        singleShotCb(key.client, info.lastRecvdLocation,
                                LOCATION_ERROR_TIMEOUT_ACC_CHECK_FAILED);
                    }
                    else
                    {
                        if (!mIsGnssLoaded && !isEngineLoadFailureReported(key.client))
                        {
                            reportMetaDataEngineStatus(LOCATION_ENGINE_STATUS_LOAD_FAILED);
                        }
                        singleShotCb(key.client, zpp, LOCATION_ERROR_TIMEOUT);
                    }
                }
            }

            // Delete this session
            eraseSession = true;
            if (LOCATION_POWER_HIGH == info.options.powerLevel) {
                gnssSessionErased = true;
            }
        }

        // Erase session if required
        if (eraseSession) {
            mSingleShotSessions.erase(it++);
        } else {
            ++it;
        }
        eraseSession = false;
    }

    // If there are no single shot GNSS sessions left,
    // restore time based tracking
    if (!anySingleShotGnssSessionActive()) {
        if (anyTimeBasedTrackingSessionActive()) {
            restoreTimeBasedTrackingMultiplex();
        } else if (gnssSessionErased) {
            // Did we erase the last GNSS session ?
            // Then send STOP request to engine.
            mIzatApi->stopTimeBasedTracking();
        }
    }

    // If all sessions removed, stop the timer
    if (!anySingleShotSessionActive()) {
        mSingleShotTimer.stop();
    }
}

// Handle timeout received from Modem
void FlpLocationAdapter::singleShotGnssTimeout() {

    LOC_LOGL0("singleShotGnssTimeout()");

    // Do we need to re-send single shot tracking request ?
    if (anySingleShotGnssSessionActive()) {

        SingleShotSessionMap::iterator it = mSingleShotSessions.begin();

        SingleShotOptions options = it->second.options;
        if(options.accuracyLevel == LOCATION_ACCURACY_UNKNOWN) {
            options.accuracyLevel = mConfig.accuracy;
        }
        // Start session with slightly higher timeout
        // To avoid race condition between timeout timer expiry vs engine reported timeout.
        options.timeout += 500;
        mIsGnssLoaded = false;
        mIzatApi->getSingleShot(options);
    }
}

// Validate position against accuracy threshold
bool FlpLocationAdapter::meetsAccuracyThreshold(
        const Location& location, const SingleShotOptions& options) {

    // Fail for invalid accuracy
    if (location.accuracy <= 0) {
        return false;
    }

    // No accuracy level configured : filter by power
    if (options.accuracyLevel == LOCATION_ACCURACY_UNKNOWN)
    {
        switch(options.powerLevel)
        {
        case LOCATION_POWER_LOW:
            return (mConfig.wwanFixAccuracyThreshold == 0 ||
                    location.accuracy < mConfig.wwanFixAccuracyThreshold);

        case LOCATION_POWER_MED:
            return (location.accuracy < 100);

        case LOCATION_POWER_HIGH:
            return (location.accuracy < 50);

        default:
            LOC_LOGE1("Invalid power level %d", options.powerLevel);
            return false;
        }
    }

    // Accuracy provided for the session, compare with individual thresholds
    switch(options.accuracyLevel) {
    case LOCATION_ACCURACY_LOW:
        return (location.accuracy < mConfig.lowAccuracyThreshold);

    case LOCATION_ACCURACY_MED:
        return (location.accuracy < mConfig.medAccuracyThreshold);

    case LOCATION_ACCURACY_HIGH:
        return (location.accuracy < mConfig.highAccuracyThreshold);

    default:
        LOC_LOGE1("Invalid accuracy level %d", options.accuracyLevel);
        break;
    }

    return false;
}

bool
FlpLocationAdapter::anyMatchingSingleShotSessionActive(const SingleShotOptions& options)
{
    bool sessionActive = false;
    for (SingleShotSessionMap::iterator it = mSingleShotSessions.begin();
            it != mSingleShotSessions.end(); ++it) {
        if (options.powerLevel == it->second.options.powerLevel &&
                options.accuracyLevel == it->second.options.accuracyLevel) {
            sessionActive = true;
            break;
        }
    }

    bool providerCbPending = false;
    SingleshotRequestType requestType = getSingleShotRequestType(options);
    if (SINGLESHOT_REQUEST_TYPE_GTPWWAN_1_0 == requestType) {
        providerCbPending = mIsSingleshotWwanCbPending;
    }
    if (SINGLESHOT_REQUEST_TYPE_GTPWWAN_2_0 == requestType) {
        providerCbPending = mIsSingleshotMcsCbPending;
    }
    if (SINGLESHOT_REQUEST_TYPE_WIFI == requestType) {
        providerCbPending = mIsSingleshotWifiCbPending;
    }
    if (SINGLESHOT_REQUEST_TYPE_GNSS == requestType) {
        providerCbPending = mIsSingleshotGnssCbPending;
    }

    if (sessionActive && !providerCbPending) {
        sessionActive = false;
    }

    return sessionActive;
}

SingleshotRequestType
FlpLocationAdapter::getSingleShotRequestType(const SingleShotOptions& options) {

    if (LOCATION_POWER_HIGH == options.powerLevel) {

        return SINGLESHOT_REQUEST_TYPE_GNSS;
    }

    if (LOCATION_POWER_MED == options.powerLevel) {

        if (mConfig.gtpMcsPositioningEnabled && mConfig.wifiPositioningEnabled) {

            if (LOCATION_ACCURACY_HIGH == options.accuracyLevel) {
                return SINGLESHOT_REQUEST_TYPE_WIFI;
            } else {
                return SINGLESHOT_REQUEST_TYPE_GTPWWAN_2_0;
            }
        }

        if (mConfig.gtpMcsPositioningEnabled && !mConfig.wifiPositioningEnabled) {
            return SINGLESHOT_REQUEST_TYPE_GTPWWAN_2_0;
        }

        if (!mConfig.gtpMcsPositioningEnabled && mConfig.wifiPositioningEnabled) {
            return SINGLESHOT_REQUEST_TYPE_WIFI;
        }

        if (!mConfig.gtpMcsPositioningEnabled && !mConfig.wifiPositioningEnabled) {
            return SINGLESHOT_REQUEST_TYPE_GTPWWAN_1_0;
        }
    }

    if (LOCATION_POWER_LOW == options.powerLevel) {

        if (!mConfig.gtpMcsPositioningEnabled ||
                LOCATION_ACCURACY_LOW == options.accuracyLevel) {
            return SINGLESHOT_REQUEST_TYPE_GTPWWAN_1_0;
        } else {
            return SINGLESHOT_REQUEST_TYPE_GTPWWAN_2_0;
        }
        }

    return SINGLESHOT_REQUEST_TYPE_INVALID;
    }

/*------------------------------------------------
  BATCHING APIs
  Invoked from client thread
-------------------------------------------------*/
uint32_t
FlpLocationAdapter::startBatchingCommand(
        LocationAPI* client, LocationOptions& options)
{
    uint32_t sessionId = generateSessionId();

    LOC_LOGH5("startBatchingCommand() "
              "client %p id %u interval %u dist %u mode %u",
              client, sessionId, options.minInterval, options.minDistance,
              options.suplMode);

    struct MsgStartBatching : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        LocationOptions mLocationOptions;
        inline MsgStartBatching(FlpLocationAdapter& adapter,
                               IzatApiBase& api,
                               LocationAPI* client,
                               uint32_t sessionId,
                               LocationOptions locationOptions) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId),
            mLocationOptions(locationOptions) {}
        inline virtual void proc() const {

            /* Download XTRA Integrity before starting any session.
             * Only for the first time this message is posted, not for retries via sendPendingMsg. */
            if (!mIsPendingMsg) {
                uint32_t waitTime = mAdapter.getXtraIntDloadWaitTimeSec();
                if (waitTime > 0) {
                    LOC_LOGH1("FLP Start Batching pending on XTRA INT, wait %d sec", waitTime);
                    mAdapter.sendPendingMsg(new MsgStartBatching(
                            mAdapter, mApi, mClient, mSessionId, mLocationOptions));
                    if (!mAdapter.mPendingMsgTimer.isStarted()) {
                        mAdapter.triggerXtraIntDload();
                        mAdapter.mPendingMsgTimer.start(waitTime);
                    } else  {
                        LOC_LOGH0("Pending msg timer already running");
                    }
                    return;
                }
            }

            LocationError err = LOCATION_ERROR_SUCCESS;

            if (!mAdapter.hasBatchingCallback(mClient)) {
                err = LOCATION_ERROR_CALLBACK_MISSING;
            } else if (0 == mLocationOptions.size) {
                err = LOCATION_ERROR_INVALID_PARAMETER;
            }
            if (LOCATION_ERROR_SUCCESS == err) {
                err =  mApi.startBatching(mSessionId, mLocationOptions,
                                          mAdapter.mConfig.accuracy,
                                          mAdapter.mConfig.batchingTimeout,
                                          mAdapter.mConfig.batchAllPos);
            }

            if (LOCATION_ERROR_SUCCESS == err) {
                mAdapter.saveBatchingSession(mClient, mSessionId, mLocationOptions);
            }
            mAdapter.reportResponse(mClient, err, mSessionId);

            // Set flag to report engine load status again
            mAdapter.updateClientLoadSuccess(mClient, false);
            mAdapter.updateClientLoadFailure(mClient, false);
        }
    };

    sendMsg(new MsgStartBatching(
            *this, *mIzatApi, client, sessionId, options));
    return sessionId;
}

void
FlpLocationAdapter::updateBatchingOptionsCommand(
        LocationAPI* client, uint32_t id, LocationOptions& options)
{
    LOC_LOGH5("updateBatchingOptionsCommand() "
              "client %p id %u interval %u dist %u mode %u",
              client, id, options.minInterval, options.minDistance,
              options.suplMode);

    struct MsgUpdateBatching : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        LocationOptions mLocationOptions;
        inline MsgUpdateBatching(FlpLocationAdapter& adapter,
                                IzatApiBase& api,
                                LocationAPI* client,
                                uint32_t sessionId,
                                LocationOptions locationOptions) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId),
            mLocationOptions(locationOptions) {}
        inline virtual void proc() const {
            LocationError err = LOCATION_ERROR_SUCCESS;
            if (!mAdapter.isBatchingSession(mClient, mSessionId)) {
                err = LOCATION_ERROR_ID_UNKNOWN;
            } else if (0 == mLocationOptions.size) {
                err = LOCATION_ERROR_INVALID_PARAMETER;
            }
            if (LOCATION_ERROR_SUCCESS == err) {
                mApi.stopBatching(mSessionId);
                err =  mApi.startBatching(mSessionId, mLocationOptions,
                        mAdapter.mConfig.accuracy,
                        mAdapter.mConfig.batchingTimeout,
                        mAdapter.mConfig.batchAllPos);
                if (LOCATION_ERROR_SUCCESS == err) {
                    mAdapter.saveBatchingSession(mClient, mSessionId, mLocationOptions);
                }
           }
           mAdapter.reportResponse(mClient, err, mSessionId);
        }
    };

    sendMsg(new MsgUpdateBatching(
            *this, *mIzatApi, client, id, options));
}

void
FlpLocationAdapter::stopBatchingCommand(LocationAPI* client, uint32_t id)
{
    LOC_LOGH2("stopBatchingCommand() client %p id %u", client, id);

    struct MsgStopBatching : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        inline MsgStopBatching(FlpLocationAdapter& adapter,
                               IzatApiBase& api,
                               LocationAPI* client,
                               uint32_t sessionId) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId) {}
        inline virtual void proc() const {
            LocationError err = LOCATION_ERROR_SUCCESS;
            if (!mAdapter.isBatchingSession(mClient, mSessionId)) {
                err = LOCATION_ERROR_ID_UNKNOWN;
            }
            if (LOCATION_ERROR_SUCCESS == err) {
                err = mApi.stopBatching(mSessionId);
            }
            if (LOCATION_ERROR_SUCCESS == err) {
                mAdapter.eraseBatchingSession(mClient, mSessionId);
            }
            mAdapter.reportResponse(mClient, err, mSessionId);
        }
    };

    sendMsg(new MsgStopBatching(*this, *mIzatApi, client, id));
}

void
FlpLocationAdapter::getBatchedLocationsCommand(
        LocationAPI* client, uint32_t id, size_t count)
{
    LOC_LOGH3("getBatchedLocationsCommand() client %p id %u count %u",
            client, id, count);

    struct MsgGetBatchedLocations : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        size_t mCount;
        inline MsgGetBatchedLocations(FlpLocationAdapter& adapter,
                                     IzatApiBase& api,
                                     LocationAPI* client,
                                     uint32_t sessionId,
                                     size_t count) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId),
            mCount(count) {}
        inline virtual void proc() const {
            LocationError err = LOCATION_ERROR_SUCCESS;
            if (!mAdapter.hasBatchingCallback(mClient)) {
                err = LOCATION_ERROR_CALLBACK_MISSING;
            } else if (!mAdapter.isBatchingSession(mClient, mSessionId)) {
                err = LOCATION_ERROR_ID_UNKNOWN;
            }
            if (LOCATION_ERROR_SUCCESS == err) {
                err = mApi.getBatchedLocations(mCount);
            }
            mAdapter.reportResponse(mClient, err, mSessionId);
        }
    };

    sendMsg(new MsgGetBatchedLocations(*this, *mIzatApi, client, id, count));
}

metaDataCallback
FlpLocationAdapter::getMetaDataCallback(LocationAPI* client) {

    ClientDataMap::iterator it = mClientData.find(client);
    if (it == mClientData.end()) {
        return NULL;
    }
    return it->second.callbacks.metaDataCb;
}

bool
FlpLocationAdapter::reportMetaData(
        const LocationMetaData& metaData, LocationAPI* client)
{
    LOC_LOGL1("reportMetaData() flags 0x%x", metaData.flags);

    // Report to all clients with meta data cb
    for (ClientDataMap::iterator it=mClientData.begin();
            it != mClientData.end(); ++it) {

        if ((NULL == client || it->first == client) &&
                NULL != it->second.callbacks.metaDataCb) {
            it->second.callbacks.metaDataCb(it->first, metaData);
        }
    }

    return true;
}

void
FlpLocationAdapter::reportMetaDataEngineStatus(
        LocationMetaDataEngineStatus status, LocationAPI* client)
{
    LocationMetaData metaData = {};
    metaData.size = sizeof(LocationMetaData);
    metaData.flags = LOCATION_META_DATA_ENGINE_STATUS;
    metaData.engineStatus = status;
    reportMetaData(metaData, client);
}

LocationMetaDataEngineStatus
FlpLocationAdapter::reportMetaDataForSessionStatus(enum loc_sess_status status)
{
    LOC_LOGL1("reportMetaData() status %d", status);

    LocationMetaData metaData = {};
    metaData.size = sizeof(LocationMetaData);
    metaData.flags = LOCATION_META_DATA_ENGINE_STATUS;
    metaData.engineStatus = LOCATION_ENGINE_STATUS_UNKNOWN;

    // Deduce engine status
    if (LOC_SESS_DDR_UNAVAILABLE == status) {
        metaData.engineStatus = LOCATION_ENGINE_STATUS_LOAD_FAILED;
        mIsGnssLoaded = false;
    } else if (LOC_SESS_FAILURE == status) {
        metaData.engineStatus = LOCATION_ENGINE_STATUS_FIX_FAILED;
        mIsGnssLoaded = false;
    } else if (LOC_SESS_INTERMEDIATE == status) {
        metaData.engineStatus = LOCATION_ENGINE_STATUS_LOAD_SUCCESS;
        mIsGnssLoaded = true;
    } else if (LOC_SESS_SUCCESS == status) {
        metaData.engineStatus = LOCATION_ENGINE_STATUS_FIX_SUCCESS;
    }

    // Send status cb to all clients with ongoing sessions
    if (LOCATION_ENGINE_STATUS_UNKNOWN != metaData.engineStatus) {

        for (ClientDataMap::iterator it=mClientData.begin();
                it != mClientData.end(); ++it) {

            LocationAPI* client = it->first;
            LocationClientInfo clientInfo = it->second;

            if (NULL != clientInfo.callbacks.metaDataCb) {

                if (anyTrackingSessionActive(client) ||
                        anySingleShotSessionActive(client) ||
                        anyBatchingSessionActive(client)) {

                    // If load failure received after load success, indicates unloaded
                    if (LOCATION_ENGINE_STATUS_LOAD_FAILED == metaData.engineStatus &&
                            clientInfo.isEngineLoadSuccessReported) {
                        metaData.engineStatus = LOCATION_ENGINE_STATUS_UNLOADED;
                    }

                    // See if we need to report load success
                    if (!clientInfo.isEngineLoadSuccessReported && mIsGnssLoaded)
                    {
                        LocationMetaData md = metaData;
                        md.engineStatus = LOCATION_ENGINE_STATUS_LOAD_SUCCESS;
                        clientInfo.callbacks.metaDataCb(client, md);
                        updateClientLoadSuccess(client, true);
                    }

                    switch(metaData.engineStatus)
                    {
                    case LOCATION_ENGINE_STATUS_LOAD_SUCCESS:
                        // already reported above if required
                        break;

                    case LOCATION_ENGINE_STATUS_LOAD_FAILED:
                        clientInfo.callbacks.metaDataCb(client, metaData);
                        updateClientLoadFailure(client, true);
                        break;

                    default:
                        clientInfo.callbacks.metaDataCb(client, metaData);
                        break;
                    }
                    }
                }
            }
        }

    return metaData.engineStatus;
    }

uint32_t
FlpLocationAdapter::getBackoffTimeCommand(LocationAPI* client)
{
    uint32_t sessionId = generateSessionId();

    LOC_LOGH2("getBackoffTimeCommand() client %p sessionId %d", client, sessionId);

    struct MsgGetBackoffTime : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        inline MsgGetBackoffTime(FlpLocationAdapter& adapter,
                                 IzatApiBase& api,
                                 LocationAPI* client,
                                 uint32_t sessionId) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId) {}
        inline virtual void proc() const {
            LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
            LocationMetaData metaData = {};
            err = mApi.getBackoffTime(metaData);
            mAdapter.reportResponse(mClient, err, mSessionId);
            if (LOCATION_ERROR_SUCCESS == err) {
                metaDataCallback metaDataCb = mAdapter.getMetaDataCallback(mClient);
                if (NULL != metaDataCb) {
                    metaDataCb(mClient, metaData);
                } else {
                    err = LOCATION_ERROR_CALLBACK_MISSING;
                    LOC_LOGE0("No metaDataCb to report backoff time.");
                }
            } else {
                LOC_LOGE1("Failed to get backoff time, err %d", err);
            }
        }
    };

    sendMsg(new MsgGetBackoffTime(*this, *mIzatApi, client, sessionId));
    return sessionId;
}

uint32_t
FlpLocationAdapter::getGnssEnergyConsumedCommand(LocationAPI* client)
{
    uint32_t sessionId = generateSessionId();

    LOC_LOGH2("getGnssEnergyConsumedCommand() client %p sessionId %d", client, sessionId);

    struct MsgGetGnssEnergyConsumed : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        inline MsgGetGnssEnergyConsumed(FlpLocationAdapter& adapter,
                                        IzatApiBase& api,
                                        LocationAPI* client,
                                        uint32_t sessionId) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId) {}
        inline virtual void proc() const {
            LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
            LocationMetaData metaData = {};
            err = mApi.getGnssEnergyConsumed(metaData);
            mAdapter.reportResponse(mClient, err, mSessionId);
            if (LOCATION_ERROR_SUCCESS == err) {
                metaDataCallback metaDataCb = mAdapter.getMetaDataCallback(mClient);
                if (NULL != metaDataCb) {
                    LOC_LOGL8("Reporting metaDataCb: Size: %u, Flags:%p, Backoff: %u, NrstGf: %u, "
                              "energy lastboot 0x%x%x frstboot 0x%x%x",
                              metaData.size, metaData.flags, metaData.backoffTimeMillis,
                              metaData.nearestGeofenceMeters,
                              (int)(metaData.energyConsumedSinceLastBoot >> 32),
                              (int)metaData.energyConsumedSinceLastBoot,
                              (int)(metaData.energyConsumedSinceFirstBoot >> 32),
                              (int)metaData.energyConsumedSinceFirstBoot);
                    metaDataCb(mClient, metaData);
                } else {
                    err = LOCATION_ERROR_CALLBACK_MISSING;
                    LOC_LOGE0("No metaDataCb to report gnss energy consumed.");
                }
            } else {
                LOC_LOGE1("Failed to get gnss energy consumed, err %d", err);
            }
        }
    };

    sendMsg(new MsgGetGnssEnergyConsumed(*this, *mIzatApi, client, sessionId));
    return sessionId;
}

void
FlpLocationAdapter::enableGtpWwanCommand(void)
{
    LOC_LOGH1("%s]: ", __func__);

    struct MsgEnableGtpWwan : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        inline MsgEnableGtpWwan(FlpLocationAdapter& adapter,
            IzatApiBase& api) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api) {}
        inline virtual void proc() const {
            int err;
            err = mApi.enableGtpWwan();
            if (IZAT_SUCCESS == err) {
                mAdapter.reportControlResponse(LOCATION_ERROR_SUCCESS);
            }
            else {
                mAdapter.reportControlResponse(LOCATION_ERROR_GENERAL_FAILURE);
            }
        }
    };

    if (mContext != NULL) {
        sendMsg(new MsgEnableGtpWwan(*this, *mIzatApi));
    }
}
void
FlpLocationAdapter::disableGtpWwanCommand(void)
{
    LOC_LOGH1("%s]: ", __func__);

    struct MsgDisableGtpWwan : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        inline MsgDisableGtpWwan(FlpLocationAdapter& adapter,
            IzatApiBase& api) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api) {}
        inline virtual void proc() const {
            int err;
            err = mApi.disableGtpWwan();
            if (IZAT_SUCCESS == err) {
                mAdapter.reportControlResponse(LOCATION_ERROR_SUCCESS);
            }
            else {
                mAdapter.reportControlResponse(LOCATION_ERROR_GENERAL_FAILURE);
            }
        }
    };

    if (mContext != NULL) {
        sendMsg(new MsgDisableGtpWwan(*this, *mIzatApi));
    }
}

void
FlpLocationAdapter::enableCommand(LocationTechnologyType techType)
{
    LOC_LOGH1("enableCommand() techType %u", techType);

    struct MsgEnableGnss : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationTechnologyType mTechType;
        inline MsgEnableGnss(FlpLocationAdapter& adapter,
                             IzatApiBase& api,
                             LocationTechnologyType techType) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mTechType(techType) {}
        inline virtual void proc() const {
            LocationError err = LOCATION_ERROR_SUCCESS;
            if (LOCATION_TECHNOLOGY_TYPE_GNSS == mTechType) {
                // mApi.setGpsLock(GNSS_CONFIG_GPS_LOCK_NONE);
                XtraClient* xtraClient = (XtraClient*)getXtraClient();
                if (NULL != xtraClient) {
                    getXtraInfoCache().updateGpsLockStatus(0);
                }
            } else if (LOCATION_TECHNOLOGY_TYPE_GTP_WWAN == mTechType) {
                if (IZAT_SUCCESS == mApi.enableGtpWwan()) {
                    err = LOCATION_ERROR_SUCCESS;
                } else {
                    LOC_LOGE0("Failed to enable GTP WWAN");
                }
            } else {
                err = LOCATION_ERROR_INVALID_PARAMETER;
            }
            mAdapter.reportControlResponse(err);
        }
    };

    sendMsg(new MsgEnableGnss(*this, *mIzatApi, techType));
}

void
FlpLocationAdapter::disableCommand(LocationTechnologyType techType)
{
    LOC_LOGH1("disableCommand() techType %u", techType);

    struct MsgDisableGnss : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationTechnologyType mTechType;
        inline MsgDisableGnss(FlpLocationAdapter& adapter,
                              IzatApiBase& api,
                              LocationTechnologyType techType) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mTechType(techType) {}
        inline virtual void proc() const {
            LocationError err = LOCATION_ERROR_SUCCESS;
            if (LOCATION_TECHNOLOGY_TYPE_GNSS == mTechType) {
                // mApi.setGpsLock(GNSS_CONFIG_GPS_LOCK_MO_AND_NI);
                XtraClient* xtraClient = (XtraClient*)getXtraClient();
                if (NULL != xtraClient) {
                    getXtraInfoCache().updateGpsLockStatus(1);
                }
            } else if (LOCATION_TECHNOLOGY_TYPE_GTP_WWAN == mTechType) {
                err = LOCATION_ERROR_NOT_SUPPORTED;
            } else {
                err = LOCATION_ERROR_INVALID_PARAMETER;
            }
            mAdapter.reportControlResponse(err);
        }
    };

    sendMsg(new MsgDisableGnss(*this, *mIzatApi, techType));
}

void
FlpLocationAdapter::gnssDeleteAidingDataCommand(GnssAidingData& data) {
    
    struct MsgDeleteAidingData : public LocMsg {
        FlpLocationAdapter& mAdapter;
        GnssAidingData mData;
        
        inline MsgDeleteAidingData(FlpLocationAdapter& adapter,
                                    GnssAidingData& data) : LocMsg(),
                                                            mAdapter(adapter),
                                                            mData(data) {}
        inline virtual void proc() const {
            mAdapter.deleteAidingData(mData);
        }
    };

    sendMsg(new MsgDeleteAidingData(*this, data));
}

void 
FlpLocationAdapter::deleteAidingData(GnssAidingData data) {

    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    err = mIzatApi->deleteAidingData(data);
    if (LOCATION_ERROR_SUCCESS == err) {
        LOC_LOGM0("Delete Aiding data command sent successfully");
    } 
    else {
        LOC_LOGE1("Failed to send Delete Aiding data command, err %d", err);
    }
    
    reportControlResponse(err);
}

/*------------------------------------------------
  BATCHING Utilities
  Invoked from Message Thread
-------------------------------------------------*/
bool
FlpLocationAdapter::hasBatchingCallback(LocationAPI* client)
{
    ClientDataMap::iterator it = mClientData.find(client);
    return (it != mClientData.end() && it->second.callbacks.batchingCb);
}

bool
FlpLocationAdapter::anyBatchingSessionActive(LocationAPI* client) {

    for (LocationSessionMap::iterator it = mBatchingSessions.begin();
            it != mBatchingSessions.end(); ++it) {
        if (client == it->first.client) {
            return true;
        }
    }
    return false;
}

bool
FlpLocationAdapter::isBatchingSession(LocationAPI* client, uint32_t sessionId)
{
    LocationSessionKey key(client, sessionId);
    return (mBatchingSessions.find(key) != mBatchingSessions.end());
}

void
FlpLocationAdapter::saveBatchingSession(LocationAPI* client, uint32_t sessionId,
                                const LocationOptions& options)
{
    LocationSessionKey key(client, sessionId);

    LOC_LOGL2("saveBatchingSession() client %p id %u", client, sessionId);

    ExtendedLocationOptions extOptions;
    extOptions.locationOptions = options;
    extOptions.isGnssDataRequested = 0;
    mBatchingSessions[key] = extOptions;
}

void
FlpLocationAdapter::eraseBatchingSession(LocationAPI* client, uint32_t sessionId)
{
    LocationSessionKey key(client, sessionId);
    LocationSessionMap::iterator it = mBatchingSessions.find(key);

    LOC_LOGL2("eraseBatchingSession() client %p id %u", client, sessionId);

    if (it != mBatchingSessions.end()) {
        mBatchingSessions.erase(it);
    }
}

/*------------------------------------------------
  RESPONSE APIs
  Invoked from Message Thread
-------------------------------------------------*/
void
FlpLocationAdapter::reportResponse(
        LocationAPI* client, LocationError err, uint32_t sessionId)
{
    LOC_LOGH3("reportResponse() client %p id %u err %u",
            client, sessionId, err);

    ClientDataMap::iterator it = mClientData.find(client);
    if (it != mClientData.end() &&
        it->second.callbacks.responseCb != NULL) {
        it->second.callbacks.responseCb(it->first, err, sessionId);
    } else {
        LOC_LOGE1("Client %p not found !", client);
    }
}

void
FlpLocationAdapter::reportControlResponse(LocationError err)
{
    LOC_LOGE2("%s]: err %u", __func__, err);

    if (mControlCallbacks.size > 0 && NULL != mControlCallbacks.responseControlCb) {
        mControlCallbacks.responseControlCb(err);
    }
    else {
        LOC_LOGE1("%s]: control client response callback not found", __func__);
    }
}

bool
FlpLocationAdapter::reportLocations(Location* locations, size_t count)
{
    LOC_LOGL1("reportLocations() count %d", count);

    for (ClientDataMap::iterator it=mClientData.begin();
            it != mClientData.end(); ++it) {
        if (NULL != it->second.callbacks.batchingCb &&
                anyBatchingSessionActive(it->first)) {
            it->second.callbacks.batchingCb(it->first, count, locations);
        }
    }
    return true;
}

bool
FlpLocationAdapter::reportLocation(const Location& location,
        const GpsLocationExtended& locationExtended, enum loc_sess_status status)
{
    NmeaSource source = LOCATION_NMEA_SOURCE_UNKNOWN;
    mGnssSvIdUsedInPosAvail = false;
    bool nmeaCbRegistered = false;
    
    LOC_LOGL1("reportLocation() status %d", status);

    if (LOC_SESS_INTERMEDIATE != status) {
        mIsSingleshotGnssCbPending = false;
    }

    if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_GNSS_SV_USED_DATA) {
        mGnssSvIdUsedInPosAvail = true;
        mGnssSvIdUsedInPosition = locationExtended.gnss_sv_used_ids;
    }

    // Report engine status meta data
    LocationMetaDataEngineStatus statusReported = reportMetaDataForSessionStatus(status);

    // Inject time to system
    if (LOC_SESS_SUCCESS == status) {
        LocOSIface_injectTime(location.timestamp, LOC_OS_IFACE_TIME_SOURCE_GPS);
    }

    // Check for errors
    if (LOC_SESS_TIMEOUT == status) {
        updateLastRecvdGnssLocation(location);
        singleShotGnssTimeout();
        return true;
    }
    if (LOC_SESS_DDR_UNAVAILABLE == status || LOC_SESS_FAILURE == status) {
        LOC_LOGL1("Ignoring position report with status %d", status);
        return true;
    }

    // check if we need to report position (in case this is
    // just a session for gnssData then we don't need to report
    if ((getGnssDataSessionCount() > 0 && mTimeBasedTrackingSessions.size() <= 1) &&
            !anySingleShotSessionActive()) {
        LOC_LOGL2("Ignoring position report, gnssDataSessionCount %d, tracking sessions %d",
                getGnssDataSessionCount(), mTimeBasedTrackingSessions.size());
        return true;
    }

    /* NMEA generation */ 
    for (ClientDataMap::iterator it=mClientData.begin();
            it != mClientData.end(); ++it) {
        if (NULL != it->second.callbacks.gnssNmeaCb) {
            nmeaCbRegistered = true;
        }
    }
    if (mIsGnssLoaded && FLP_NMEA_PROVIDER_AP == mConfig.nmeaProvider && nmeaCbRegistered &&
            (!mTimeBasedTrackingSessions.empty() || !mSingleShotSessions.empty()) &&
            (status != LOC_SESS_INTERMEDIATE || mConfig.nmeaForIntermediatePos != 0)) {
        /*Only BlankNMEA sentence needs to be processed and sent, if both lat, long is 0 */
        bool blank_fix = (((0 == location.latitude) &&
                           (0 == location.longitude) &&
                           (LOC_RELIABILITY_NOT_SET_V02 == locationExtended.horizontal_reliability)) || 
                          ((LOC_SESS_INTERMEDIATE == status) && 
                           (0 == mConfig.intermediatePos)));
        uint8_t generate_nmea = (LOC_SESS_FAILURE != status && !blank_fix);
        std::vector<std::string> nmeaArraystr;
        loc_nmea_generate_pos(location, locationExtended, generate_nmea, nmeaArraystr,
                              mConfig.disableNmeaTimeCheck, mActiveConstellationMask);

        std::stringstream ss;
        for (int i = 0; i < nmeaArraystr.size(); i++) {
            LOC_LOGSM1("NMEA %s", nmeaArraystr[i].c_str());
            ss << nmeaArraystr[i];
        }

        if(LOC_SESS_SUCCESS == status) {
            source = LOCATION_NMEA_SOURCE_POSITION_FINAL;
        }
        else if(LOC_SESS_INTERMEDIATE == status) {
            source = LOCATION_NMEA_SOURCE_POSITION_INTERMEDIATE;
        }
        string s = ss.str();
        if (s.length() > 0) {
            reportNmea(s.c_str(), s.length(), source);
        }
    }
    else {
        LOC_LOGE4("Not generating NMEAs. provider %d tracking session %d singleshot session %d CB registered %d",
            mConfig.nmeaProvider,
            mTimeBasedTrackingSessions.size(),
            mSingleShotSessions.size(),
            nmeaCbRegistered);
    }

    // Check if we report intermediate positions
    if (status != LOC_SESS_INTERMEDIATE || mConfig.intermediatePos != 0) {

        updateLastRecvdGnssLocation(location);

        // Report for all tracking sessions
        for (ClientDataMap::iterator it=mClientData.begin();
                it != mClientData.end(); ++it) {

            if (LOC_SESS_SUCCESS == status || LOC_SESS_INTERMEDIATE == status) {

                if (NULL != it->second.callbacks.trackingCb &&
                        anyTrackingSessionActive(it->first)) 
                {
                    it->second.callbacks.trackingCb(it->first, location);
                }
            }
        }

        // Report for all singleshot sessions
        for (SingleShotSessionMap::iterator it = mSingleShotSessions.begin();
                it != mSingleShotSessions.end(); /* Increment inside loop */) {

            // process the session
            LocationSessionKey key = it->first;
            SingleShotSessionInfo info = it->second;
            singleShotCallback singleShotCb = getSingleShotCallback(key.client);

            if (meetsAccuracyThreshold(location, info.options)) {

                if (LOCATION_ENGINE_STATUS_FIX_SUCCESS != statusReported)
                {
                    reportMetaDataEngineStatus(LOCATION_ENGINE_STATUS_FIX_SUCCESS);
        }
                if (singleShotCb != NULL) {
                    singleShotCb(key.client, location, LOCATION_ERROR_SUCCESS);
                }
                mSingleShotSessions.erase(it++);

            } else {

                LOC_LOGL1("Location fails accuracy criteria, acc %d",
                           (uint32_t)location.accuracy);
                reportMetaDataEngineStatus(LOCATION_ENGINE_STATUS_POS_ACC_CHECK_FAILED);
                ++it;
            }
        }
        }

    return true;
}

bool
FlpLocationAdapter::reportWwanZppFix(
        const Location& wwanLocation, LocationError wwanErr, bool isBestAvailPosition) {

    LOC_LOGL0("reportWwanZppFix()");

    mIsSingleshotWwanCbPending = false;

    // Fetch Best available position as well
    Location* selectedFix = &wwanLocation;
    Location zppLocation = {};
    LocationError zppErr = LOCATION_ERROR_GENERAL_FAILURE;
    if (!isBestAvailPosition && 0 == mConfig.noZppBeforeWwan) {
        // Fetch ZPP only if wwanLocation is not already zpp position
        zppErr = mIzatApi->getZppFix(zppLocation);
    }
    if (LOCATION_ERROR_SUCCESS != wwanErr && LOCATION_ERROR_SUCCESS != zppErr) {
        LOC_LOGE0("WWAN and ZPP both failed !");
        return false;
    }
    if (LOCATION_ERROR_SUCCESS != wwanErr ||
            (LOCATION_ERROR_SUCCESS == zppErr &&
                    zppLocation.accuracy > 0 &&
                    zppLocation.accuracy < wwanLocation.accuracy)) {
        selectedFix = &zppLocation;
    }

    // Check if accuracy is acceptable
    if (isBestAvailPosition) {
        LOC_LOGL1("ZPP accuracy %d WWAN accuracy NA", (int)wwanLocation.accuracy);
    } else {
        LOC_LOGL2("ZPP accuracy %d WWAN accuracy %d",
                  (int)zppLocation.accuracy, (int)wwanLocation.accuracy);
    }

    // Iterate over all singleshot sessions
    for (SingleShotSessionMap::iterator it = mSingleShotSessions.begin();
            it != mSingleShotSessions.end(); /* Increment while erasing */) {

        // process the session
        LocationSessionKey key = it->first;
        SingleShotSessionInfo& info = it->second;
        singleShotCallback singleShotCb = getSingleShotCallback(key.client);

        // Report WWAN fix if requested by client
        SingleshotRequestType requestType = getSingleShotRequestType(info.options);
        if (SINGLESHOT_REQUEST_TYPE_GTPWWAN_1_0 == requestType ||
                SINGLESHOT_REQUEST_TYPE_GTPWWAN_2_0 == requestType) {

            info.lastRecvdLocation = *selectedFix;

            if (meetsAccuracyThreshold(*selectedFix, info.options)) {

                LOC_LOGM1("Accuracy acceptable: %d", (uint32_t)selectedFix->accuracy);
                reportMetaDataEngineStatus(LOCATION_ENGINE_STATUS_FIX_SUCCESS);
            if (singleShotCb) {
                LOC_LOGH2("Single Shot callback for client %p session %d",
                           key.client, key.id);
                singleShotCb(key.client, *selectedFix, LOCATION_ERROR_SUCCESS);
            }
            mSingleShotSessions.erase(it++);

            } else {
                reportMetaDataEngineStatus(LOCATION_ENGINE_STATUS_POS_ACC_CHECK_FAILED);
                ++it;
            }

        } else {
            ++it;
        }
    }

    return true;
}

bool
FlpLocationAdapter::reportWifiFix(
        const Location& location, LocationError err) {

    LOC_LOGL0("reportWifiFix()");

    /* Process position from Wifi and pass it to client */
    wifiAdapterSingleshotCb(location, err);

    /* Inject position to Modem as CP
     * Hard-code accuracy for WIFI fix to 1000 meters for injection */
    if (LOCATION_ERROR_SUCCESS == err) {
        mIzatApi->injectPosition(location.latitude, location.longitude, 1000);
    }

    return true;
}

bool
FlpLocationAdapter::reportNmea(const char* nmea, int length, NmeaSource source)
{
    GnssData gnssData;
    bool ret = false;
    GnssNmeaNotification nmeaNotification = {};
    SystemStatus* s = mSystemStatus;
    memset(&gnssData, 0, sizeof(gnssData));
    gnssData.size = sizeof(gnssData);

    if (NULL != s) {
        ret = s->setNmeaString(nmea, length);
        SystemStatusReports reports = {};
        s->getReport(reports, true);
        if ((!reports.mRfAndParams.empty())) {
            gnssData.jammerInd[GNSS_SV_TYPE_GPS] = (double)reports.mRfAndParams.back().mJammerGps;
            gnssData.jammerInd[GNSS_SV_TYPE_GLONASS] = (double)reports.mRfAndParams.back().mJammerGlo;
            gnssData.jammerInd[GNSS_SV_TYPE_BEIDOU] = (double)reports.mRfAndParams.back().mJammerBds;
            gnssData.jammerInd[GNSS_SV_TYPE_GALILEO] = (double)reports.mRfAndParams.back().mJammerGal;
            LOC_LOGL2("reportNmea() jammer[GPS]=%d jammer[GLONASS]=%d",
                (int)gnssData.jammerInd[GNSS_SV_TYPE_GPS],
                (int)gnssData.jammerInd[GNSS_SV_TYPE_GLONASS]);
        }
    }
    if (getGnssDataSessionCount() > 0) {
        reportJammer(gnssData);
    }

    /*Report NMEA sentence to registered clients */
    nmeaNotification.size = sizeof(GnssNmeaNotification);
    nmeaNotification.timestamp = LocOSIface_getMillisSinceEpoch();
    nmeaNotification.nmea = nmea;
    nmeaNotification.length = (size_t)length;
    nmeaNotification.source = source;

    for (ClientDataMap::iterator it = mClientData.begin();
        it != mClientData.end(); ++it) {
        if (NULL != it->second.callbacks.gnssNmeaCb) {
            LOC_LOGL1("NMEA report for client %p", it->first);
            it->second.callbacks.gnssNmeaCb(it->first, nmeaNotification);
        }
    }

    return ret;
}

bool
FlpLocationAdapter::reportSv(GnssSvNotification& svInfo)
{
    // LocationError err = LOCATION_ERROR_SUCCESS;
    LOC_LOGL0("reportSv()");

    int numSv = svInfo.count;
    int16_t gnssSvId = 0;
    uint64_t svUsedIdMask = 0;

    mIsGnssLoaded = true;

    // Update signal strength flag in all sessions
    updateGnssSignalStrength(svInfo);

    for (int i=0; i < numSv; i++)
    {
        svUsedIdMask = 0;
        gnssSvId = svInfo.gnssSvs[i].svId;
        switch (svInfo.gnssSvs[i].type) {
            case GNSS_SV_TYPE_GPS:
                if (mGnssSvIdUsedInPosAvail) {
                    svUsedIdMask = mGnssSvIdUsedInPosition.gps_sv_used_ids_mask;
                }
                break;
            case GNSS_SV_TYPE_GLONASS:
                if (mGnssSvIdUsedInPosAvail) {
                    svUsedIdMask = mGnssSvIdUsedInPosition.glo_sv_used_ids_mask;
                }
                break;
            case GNSS_SV_TYPE_BEIDOU:
                if (mGnssSvIdUsedInPosAvail) {
                    svUsedIdMask = mGnssSvIdUsedInPosition.bds_sv_used_ids_mask;
                }
                break;
            case GNSS_SV_TYPE_GALILEO:
                if (mGnssSvIdUsedInPosAvail) {
                    svUsedIdMask = mGnssSvIdUsedInPosition.gal_sv_used_ids_mask;
                }
                break;
            case GNSS_SV_TYPE_QZSS:
                if (mGnssSvIdUsedInPosAvail) {
                    svUsedIdMask = mGnssSvIdUsedInPosition.qzss_sv_used_ids_mask;
                }
                break;
            default:
                svUsedIdMask = 0;
                break;
        }

        // If SV ID was used in previous position fix, then set USED_IN_FIX
        // flag, else clear the USED_IN_FIX flag.
        if (svUsedIdMask & (1 << (gnssSvId - 1))) {
            svInfo.gnssSvs[i].gnssSvOptionsMask =
                    svInfo.gnssSvs[i].gnssSvOptionsMask | GNSS_SV_OPTIONS_USED_IN_FIX_BIT;
        }
    }

    bool nmeaCbRegistered = false;
    for (ClientDataMap::iterator it=mClientData.begin();
            it != mClientData.end(); ++it) {
        if (NULL != it->second.callbacks.gnssNmeaCb) {
            nmeaCbRegistered = true;
        }
    }

    /* NMEA generation */
    if ((FLP_NMEA_PROVIDER_AP == mConfig.nmeaProvider) &&
        (!mTimeBasedTrackingSessions.empty() || !mSingleShotSessions.empty()) &&
         nmeaCbRegistered) {

        std::vector<std::string> nmeaArraystr;
        loc_nmea_generate_sv(svInfo, nmeaArraystr, mActiveConstellationMask);

        std::stringstream ss;
        for (int i = 0; i < nmeaArraystr.size(); i++) {
            LOC_LOGSM1("%s", nmeaArraystr[i].c_str());
            ss << nmeaArraystr[i];
        }
        string s = ss.str();
        if (s.length() > 0) {
            reportNmea(s.c_str(), s.length(), LOCATION_NMEA_SOURCE_SV_INFO);
        }
    } else {

        LOC_LOGE4("Not generating NMEAs. provider %d tracking session %d singleshot session %d CB registered %d",
            mConfig.nmeaProvider,
            mTimeBasedTrackingSessions.size(),
            mSingleShotSessions.size(),
            nmeaCbRegistered);
    }

    mGnssSvIdUsedInPosAvail = false;
    return true;
}

bool
FlpLocationAdapter::reportJammer(GnssData gnssData)
{
    LOC_LOGL0("reportJammer()");

    for (ClientDataMap::iterator it = mClientData.begin();
        it != mClientData.end(); ++it) {
        if (NULL != it->second.callbacks.gnssDataCb &&
            anyTrackingSessionActive(it->first)) {
            it->second.callbacks.gnssDataCb(it->first, gnssData);
        }
    }

    return true;
}

/*------------------------------------------------
  MOTION TRACKING APIs
  Invoked from Client Thread
-------------------------------------------------*/
uint32_t
FlpLocationAdapter::startMotionTrackingCommand(
        LocationAPI* client,
        LocationMotionOptions& options)
{
    uint32_t sessionId = generateSessionId();

    LOC_LOGH3("startMotionTrackingCommand(): client %p id %u mask %p",
              client, sessionId, options.motionTypeMask);

    struct MsgStartMotionTracking : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        LocationMotionOptions mOptions;
        inline MsgStartMotionTracking(FlpLocationAdapter& adapter,
                               IzatApiBase& api,
                               LocationAPI* client,
                               uint32_t sessionId,
                               LocationMotionOptions options) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId),
            mOptions(options) {}
        inline virtual ~MsgStartMotionTracking(){}

        inline virtual void proc() const {

            LOC_LOGH0("MsgStartMotionTracking::proc()");

            LocationError err = LOCATION_ERROR_SUCCESS;

#ifndef SENSORS_DISABLED

            // Validate options
            if (!mAdapter.hasMotionTrackingCallback(mClient)) {

                LOC_LOGE0("No motion tracking callback.");
                err = LOCATION_ERROR_CALLBACK_MISSING;

            } else if (0 == mOptions.size) {

                LOC_LOGE0("Options size 0.");
                err = LOCATION_ERROR_INVALID_PARAMETER;

            } else if (0 == (mOptions.motionTypeMask & LOCATION_MOTION_TYPE_MOVE_ABSOLUTE) &&
                    0 == (mOptions.motionTypeMask & LOCATION_MOTION_TYPE_REST) &&
                    0 == (mOptions.motionTypeMask & LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT)) {

                LOC_LOGE1("Invalid motion type mask %p", mOptions.motionTypeMask);
                err = LOCATION_ERROR_INVALID_PARAMETER;
            }

            if (LOCATION_ERROR_SUCCESS == err) {

                if (NULL == mAdapter.mSlimClient) {
                    LOC_LOGH0("Creating SLIM Client Instance..");
                    mAdapter.mSlimClient = new FlpSlimClient(&mAdapter);
                }

                // Enable AMD
                if ((mOptions.motionTypeMask & LOCATION_MOTION_TYPE_MOVE_ABSOLUTE) ||
                        (mOptions.motionTypeMask & LOCATION_MOTION_TYPE_REST) ||
                        (mOptions.motionTypeMask & LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT)) {

                    // AMD is needed for any type of motion since we are always
                    // interested in REST event
                    err = mAdapter.mSlimClient->enableAmdDetection();
                }
            }

            if (LOCATION_ERROR_SUCCESS == err) {

                // Enable SMD
                if (mOptions.motionTypeMask & LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT) {

                    err = mAdapter.mSlimClient->enableSmdDetection();
                }
            }

#else
            err = LOCATION_ERROR_NOT_SUPPORTED;
#endif

            if (LOCATION_ERROR_SUCCESS == err) {

                mAdapter.saveMotionTrackingSession(mClient, mSessionId, mOptions);
            }

            mAdapter.reportResponse(mClient, err, mSessionId);
        }
    };

    sendMsg(new MsgStartMotionTracking(*this, *mIzatApi, client, sessionId, options));
    return sessionId;
}

void
FlpLocationAdapter::stopMotionTrackingCommand(
    LocationAPI* client, uint32_t id)
{
    LOC_LOGH2("stopMotionTrackingCommand() client %p id %u", client, id);

    struct MsgStopMotionTracking : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mApi;
        LocationAPI* mClient;
        uint32_t mSessionId;
        inline MsgStopMotionTracking(FlpLocationAdapter& adapter,
                               IzatApiBase& api,
                               LocationAPI* client,
                               uint32_t sessionId) :
            LocMsg(),
            mAdapter(adapter),
            mApi(api),
            mClient(client),
            mSessionId(sessionId) {}
        inline virtual void proc() const {

            LocationError err = LOCATION_ERROR_SUCCESS;

#ifndef SENSORS_DISABLED

            LocationMotionOptions options = {};

            mAdapter.getMotionTrackingSessionOptions(mClient, mSessionId, options);
            if (options.size == 0) {
                err = LOCATION_ERROR_ID_UNKNOWN;
            }

            if (LOCATION_ERROR_SUCCESS == err) {

                mAdapter.eraseMotionTrackingSession(mClient, mSessionId);

                // Disable AMD only if no session is remaining
                if (!mAdapter.anyMotionTrackingSessionActive()) {

                    err = mAdapter.mSlimClient->disableAmdDetection();
                }

                // Disable SMD if no interested client
                if (options.motionTypeMask & LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT) {

                    if (!mAdapter.anyMotionTrackingSessionActive(NULL, LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT)) {

                        err = mAdapter.mSlimClient->disableSmdDetection();
                    }
                }
            }

#else
            err = LOCATION_ERROR_NOT_SUPPORTED;
#endif

            mAdapter.reportResponse(mClient, err, mSessionId);
        }
    };

    sendMsg(new MsgStopMotionTracking(*this, *mIzatApi, client, id));
}

void FlpLocationAdapter::reportMotionStateEvent(const LocationMotionInfo motionInfo)
{
    LOC_LOGH0("reportMotionStateEvent()");

    struct MsgReportMotionState : public LocMsg {
        FlpLocationAdapter& mAdapter;
        const LocationMotionInfo mMotionInfo;
        inline MsgReportMotionState(FlpLocationAdapter& adapter,
                                    const LocationMotionInfo motionInfo) :
            LocMsg(),
            mAdapter(adapter),
            mMotionInfo(motionInfo) {}
        inline virtual void proc() const {

            mAdapter.reportMotionState(mMotionInfo);
        }
    };

    sendMsg(new MsgReportMotionState(*this, motionInfo));
}

void FlpLocationAdapter::reportMotionState(const LocationMotionInfo& motionInfo)
{
    LOC_LOGL3("reportMotionState() type %d timestamp 0x%x%x", motionInfo.motionType,
              (int)(motionInfo.timestamp >> 32), (int)motionInfo.timestamp);

#ifndef SENSORS_DISABLED

    for (ClientDataMap::iterator it=mClientData.begin();
            it != mClientData.end(); ++it) {
        if (NULL != it->second.callbacks.motionTrackingCb &&
                anyMotionTrackingSessionActive(it->first, motionInfo.motionType)) {
            it->second.callbacks.motionTrackingCb(it->first, motionInfo);
        }
    }

    // SMD needs to be disabled and re-enabled after each report
    // Disable on receiving SMD, re-enable on receiving REST event
    if (LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT == motionInfo.motionType &&
            anyMotionTrackingSessionActive(NULL, LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT)) {

        LocationError err = LOCATION_ERROR_SUCCESS;

        err = mSlimClient->disableSmdDetection();
        if (LOCATION_ERROR_SUCCESS != err) {
            LOC_LOGE1("Failed to disable SMD on recvng report, err %d", err);
        }
    }
    if (LOCATION_MOTION_TYPE_REST == motionInfo.motionType &&
            anyMotionTrackingSessionActive(NULL, LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT)) {

        if (!mSlimClient->isSmdEnabled()) {

            LocationError err = mSlimClient->enableSmdDetection();
            if (LOCATION_ERROR_SUCCESS != err) {
                LOC_LOGE1("Failed to re-enable SMD on recvng report, err %d", err);
            }
        }
    }

#endif
}

void FlpLocationAdapter::motionTrackingServiceChangeEvent(LocationMotionType serviceType, bool isAvailable) {

    LOC_LOGH0("motionTrackingServiceChangeEvent()");

    struct MsgReportMotionTrackingServiceChange : public LocMsg {
        FlpLocationAdapter& mAdapter;
        LocationMotionType mServiceType;
        bool mIsAvailable;
        inline MsgReportMotionTrackingServiceChange(
                FlpLocationAdapter& adapter,
                LocationMotionType serviceType,
                bool isAvailable) :
            LocMsg(),
            mAdapter(adapter),
            mServiceType(serviceType),
            mIsAvailable(isAvailable) {}
        inline virtual void proc() const {

            mAdapter.motionTrackingServiceChange(mServiceType, mIsAvailable);
        }
    };

    sendMsg(new MsgReportMotionTrackingServiceChange(*this, serviceType, isAvailable));
}

void FlpLocationAdapter::motionTrackingServiceChange(LocationMotionType serviceType, bool isAvailable) {

    LOC_LOGL0("motionTrackingServiceChange()");

#ifndef SENSORS_DISABLED
    if (isAvailable && anyMotionTrackingSessionActive(NULL, serviceType)) {

        if (serviceType == LOCATION_MOTION_TYPE_MOVE_ABSOLUTE) {

            mSlimClient->enableAmdDetection();
        }
        if (serviceType == LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT) {

            mSlimClient->enableSmdDetection();
        }
    }
#endif
}


/*------------------------------------------------
  MOTION TRACKING Utilities
  Invoked from Message Thread
-------------------------------------------------*/
bool
FlpLocationAdapter::hasMotionTrackingCallback(LocationAPI* client)
{
    ClientDataMap::iterator it = mClientData.find(client);
    return (it != mClientData.end() && it->second.callbacks.motionTrackingCb);
}

bool
FlpLocationAdapter::anyMotionTrackingSessionActive(
        LocationAPI* client, LocationMotionTypeMask mask) {

    for (MotionSessionMap::iterator it = mMotionTrackingSessions.begin();
            it != mMotionTrackingSessions.end(); ++it) {
        LocationAPI* itClient = it->first.client;
        LocationMotionTypeMask itMask = it->second.motionTypeMask;
        if ((client == it->first.client || client == NULL) &&
                ((mask & itMask) || mask == 0)) {
            return true;
        }
    }
    return false;
}

bool
FlpLocationAdapter::isMotionTrackingSession(
        LocationAPI* client, uint32_t sessionId)
{
    LocationSessionKey key(client, sessionId);
    return (mMotionTrackingSessions.find(key) !=
            mMotionTrackingSessions.end());
}

void
FlpLocationAdapter::saveMotionTrackingSession(
        LocationAPI* client, uint32_t sessionId,
        const LocationMotionOptions& options)
{
    LOC_LOGM2("saveMotionTrackingSession() client %p id %u", client, sessionId);
    LocationSessionKey key(client, sessionId);
    mMotionTrackingSessions[key] = options;
}

void
FlpLocationAdapter::eraseMotionTrackingSession(
        LocationAPI* client, uint32_t sessionId)
{
    LOC_LOGM2("eraseMotionTrackingSession() client %p id %u", client, sessionId);

    LocationSessionKey key(client, sessionId);
    MotionSessionMap::iterator it = mMotionTrackingSessions.find(key);

    if (it != mMotionTrackingSessions.end()) {
        mMotionTrackingSessions.erase(it);
    }
}

void
FlpLocationAdapter::getMotionTrackingSessionOptions(
        LocationAPI* client, uint32_t sessionId, LocationMotionOptions& options) {

    LOC_LOGM2("getMotionTrackingSessionOptions() client %p id %u", client, sessionId);

    LocationSessionKey key(client, sessionId);
    MotionSessionMap::iterator it = mMotionTrackingSessions.find(key);

    if (it != mMotionTrackingSessions.end()) {
        options = it->second;
    }
}

/*------------------------------------------------
  Engine up/down Events
  Invoked from QMI Thread
-------------------------------------------------*/
void
FlpLocationAdapter::handleEngineUpEvent()
{
    LOC_LOGL0("handleEngineUpEvent()");

    struct MsgRestartSessions : public LocMsg {
        FlpLocationAdapter& mAdapter;
        IzatApiBase& mIzatApi;

        inline MsgRestartSessions(FlpLocationAdapter& adapter,
                                  IzatApiBase& izatApi) :
            LocMsg(),
            mAdapter(adapter),
            mIzatApi(izatApi) {}
        inline virtual void proc() const {
            if (mAdapter.mFirstEngineUpEventRecvd) {
                //restart state of Izat Api before starting sessions
                mIzatApi.resetStateAfterSSR();
                mAdapter.restartSessions();
            } else {
                mAdapter.mFirstEngineUpEventRecvd = true;
            }
        }
    };

    sendMsg(new MsgRestartSessions(*this, *mIzatApi));
}

void
FlpLocationAdapter::handleEngineDownEvent()
{
    LOC_LOGL0("handleEngineDownEvent()");
}

void
FlpLocationAdapter::restartSessions()
{
    LOC_LOGL0("restartSessions()");

    if (!mTimeBasedTrackingSessions.empty()) {
        // get the LocationOptions that has the smallest interval
        LocationOptions smallestIntervalOptions = {};
        LocationAccuracyLevel highestAccuracy = LOCATION_ACCURACY_UNKNOWN;
        for (LocationSessionMap::iterator it =
                mTimeBasedTrackingSessions.begin();
                  it != mTimeBasedTrackingSessions.end(); ++it) {
            //size of zero means we havent set it yet
            if (0 == smallestIntervalOptions.size ||
                it->second.locationOptions.minInterval < smallestIntervalOptions.minInterval) {
                smallestIntervalOptions = it->second.locationOptions;
            }
            // Find highest requested accuracy in all sessions
            if (it->second.locationOptions.accuracyLevel > highestAccuracy)
            {
                highestAccuracy = it->second.locationOptions.accuracyLevel;
            }
        }
        mIsGnssLoaded = false;
        smallestIntervalOptions.accuracyLevel = highestAccuracy;
        mIzatApi->startTimeBasedTracking(
                smallestIntervalOptions, (LocationAccuracyLevel)mConfig.accuracy);
    }

    for (LocationSessionMap::iterator it =
            mDistanceBasedTrackingSessions.begin();
              it != mDistanceBasedTrackingSessions.end(); ++it) {
        mIzatApi->startDistanceBasedTracking(it->first.id, it->second.locationOptions);
    }
    for (LocationSessionMap::iterator it = mBatchingSessions.begin();
              it != mBatchingSessions.end(); ++it) {
        mIzatApi->startBatching(it->first.id, it->second.locationOptions,
                                mConfig.accuracy, mConfig.batchingTimeout, mConfig.batchAllPos);
    }
}

/*------------------------------------------------
  Utility Methods
  Invoked from Client Thread
-------------------------------------------------*/
void
FlpLocationAdapter::setBatchSize(size_t size)
{
    LOC_LOGL1("Modem batching size desired by the client is %u", size);
    mIzatApi->setBatchSize(size);
}

bool FlpLocationAdapter::onReceiveXtraConfig(uint32_t status, uint32_t configReqSource,
                                          uint32_t report_mask,
                                          uint16_t absAge, uint64_t relAge,
                                          uint8_t timereqValid, uint16_t prefValidAgeHrs,
                                          const char* ntpServer1, const char* ntpServer2,
                                          const char* ntpServer3,
                                          const char* xtraServer1, const char* xtraServer2,
                                          const char* xtraServer3) {
                                          
    /*Extract active constellation info from XTRA URL, Ignore other parama*/
    uint32_t mask1 = getActiveConstellationFromXtraUrl(string(xtraServer1));
    uint32_t mask2 = getActiveConstellationFromXtraUrl(string(xtraServer2));
    uint32_t mask3 = getActiveConstellationFromXtraUrl(string(xtraServer3));
    uint32_t mask = (mask1 | mask2 | mask3);
    if (0 != mask) {
        mActiveConstellationMask = mask;
    }
    LOC_LOGM1("Active constellation mask %x", mActiveConstellationMask);
	return true;
}

uint32_t FlpLocationAdapter::getActiveConstellationFromXtraUrl(const std::string& path)
{
    uint32_t constellationMask = 0;

    /*Check characters after xtra3 and before .bin*/
    size_t posXtra3 = path.find("xtra3");
    size_t posDot = path.find_last_of('.');

    if (string::npos == posXtra3 || string::npos == posDot ||
            posDot <= posXtra3) {
        LOC_LOGSE1("path unexpected [%s]", path.c_str());
        return constellationMask;
    }

    /*Iterate over constellation specific characters 
*/
    for (int i = (posXtra3 + 5); i < posDot; i++) {
        
        if ('g' == path.at(i) || 'G' == path.at(i))
            constellationMask = constellationMask|LOC_GNSS_CONSTELLATION_MASK_GPS;
        if ('c' == path.at(i) || 'C' == path.at(i) || 'b' == path.at(i) || 'B' == path.at(i))
            constellationMask = constellationMask|LOC_GNSS_CONSTELLATION_MASK_BEIDOU;
        if ('e' == path.at(i) || 'E' == path.at(i))
            constellationMask = constellationMask|LOC_GNSS_CONSTELLATION_MASK_GALILEO;
        if ('j' == path.at(i) || 'J' == path.at(i))
            constellationMask = constellationMask|LOC_GNSS_CONSTELLATION_MASK_QZSS;
        if ('r' == path.at(i) || 'R' == path.at(i))
            constellationMask = constellationMask|LOC_GNSS_CONSTELLATION_MASK_GLONASS;
        if ('i' == path.at(i) || 'I' == path.at(i))
            constellationMask = constellationMask|LOC_GNSS_CONSTELLATION_MASK_NAVIC;
    }

    LOC_LOGSH1("XTRA URL : %s", path.c_str());
    LOC_LOGM1("Active constelation mask for the current URL %x", constellationMask);

    return constellationMask;
}



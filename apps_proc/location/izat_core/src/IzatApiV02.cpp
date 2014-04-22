/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015, 2018 - 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_NDDEBUG 0
#define LOG_TAG "LocSvc_IzatApiV02"

#include "stringl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dlfcn.h>
#include <location_service_iot_v02.h>
#include "IzatApiV02.h"

#include "loc_api_v02_log.h"

#include "loc_api_sync_req.h"
#include "loc_util_log.h"

#include <math.h>
#include <algorithm>

#include <platform_lib_time.h>
#include <LocOSIface.h>
#include "GtpProtocol.h"

/* Time offset in millis between GPS time(jan 6, 1980) and UTC time(Jan 1, 1970)*/
#define UTC_GPS_TIME_OFFSET     315964800000L

#define LAT_LONG_TO_RADIANS .000005364418
#define LOC_API_V02_DEF_SESSION_ID (1)

#define XT_SYNC_REQUEST_TIMEOUT  (1000) // in msec
#define FLP_BATCHING_MINIMUN_INTERVAL_ADJUSTED  (5000) // in msec
#define FLP_BATCHING_MAX_WAIT_TIME              (60*10*1000) // 10 minutes in msec

#define DUMMY_TIME_BASED_TRACKING_SESSION_ID 1

using namespace std;
using namespace izat_gtpwwan;

namespace izat_core {

/* Gaussian 2D scaling table - scale from x% to 68% confidence */
struct conf_scaler_to_68_pair {
    uint8_t confidence;
    float scaler_to_68;
};
/* length of confScalers array */
#define CONF_SCALER_ARRAY_MAX   (3)
const struct conf_scaler_to_68_pair confScalers[CONF_SCALER_ARRAY_MAX] = {
    {39, 1.517}, // 0 - 39 . Index 0
    {50, 1.287}, // 40 - 50. Index 1
    {63, 1.072}, // 51 - 63. Index 2
};

static uint32_t wwanZppFixTransactionId = 1;


/* global event callback, call the eventCb function in loc api adapter v02
   instance */
static void globalEventCb(locClientHandleType clientHandle,
                          uint32_t eventId,
                          const locClientEventIndUnionType eventPayload,
                          void*  pClientCookie)
{
    MODEM_LOG_CALLFLOW(%s, loc_get_v02_event_name(eventId));
    IzatApiV02 *locApiV02Instance =
        (IzatApiV02 *)pClientCookie;

    LOC_LOGL3("client = %p, event id = %d, client cookie ptr = %p",
          clientHandle, eventId, pClientCookie);

    // return if null is passed
    if( NULL == locApiV02Instance)
    {
        LOC_LOGL2("NULL object passed : client = %p, event id = %d",
              clientHandle, eventId);
        return;
    }

    locApiV02Instance->eventCb(clientHandle, eventId, eventPayload);
}

/* global response callback, it calls the sync request process
   indication function to unblock the request that is waiting on this
   response indication*/
static void globalRespCb(locClientHandleType clientHandle,
                         uint32_t respId,
                         const locClientRespIndUnionType respPayload,
                         void*  pClientCookie)
{
    MODEM_LOG_CALLFLOW(%s, loc_get_v02_event_name(respId));
    IzatApiV02 *locApiV02Instance =
        (IzatApiV02 *)pClientCookie;

    LOC_LOGL3("client = %p, resp id = %d, client cookie ptr = %p",
          clientHandle, respId, pClientCookie);

    if( NULL == locApiV02Instance)
    {
        LOC_LOGL2("NULL object passed : client = %p, resp id = %d",
            clientHandle, respId);
        return;
    }

    switch(respId) 
    {
        case QMI_LOC_GET_AVAILABLE_WWAN_POSITION_IND_V02:
        {
            if (NULL != respPayload.pGetAvailWwanPositionInd) 
            {
                locApiV02Instance->handleWwanZppFixIndication(
                        *respPayload.pGetAvailWwanPositionInd);
            }
            break;
        }
        case QMI_LOC_GET_WWAN_DATA_IND_V02:
        {
            if (NULL != respPayload.pLocWwanDataInd)
            {
                locApiV02Instance->handleWwanDataIndication(
                *respPayload.pLocWwanDataInd);
            }
            break;
        }
        #ifdef FEATURE_LOCATION_MCS
        case QMI_LOC_GET_FDCL_BS_LIST_IND_V02:
        {
            if (NULL != respPayload.pGetFdclBsListInd)
            {
                locApiV02Instance->handleBsFdclListResponse(
                *respPayload.pGetFdclBsListInd);
            }
            break;
        }
        #endif
        default:
        {
            // process the sync call
            // use pInjectUtcTimeInd as a dummy pointer
            loc_sync_process_ind(clientHandle, respId,
                                 (void *)respPayload.pInjectUtcTimeInd);
            break;
        }
    }
}

/* global error callback, it will call the handle service down
   function in the loc api adapter instance. */
static void globalErrorCb (locClientHandleType clientHandle,
                           locClientErrorEnumType errorId,
                           void *pClientCookie)
{
    IzatApiV02 *locApiV02Instance =
        (IzatApiV02 *)pClientCookie;

    LOC_LOGL3("client = %p, error id = %d, client cookie ptr = %p",
          clientHandle, errorId, pClientCookie);
    if( NULL == locApiV02Instance)
    {
        LOC_LOGH2("NULL object passed : client = %p, error id = %d",
              clientHandle, errorId);
        return;
    }
    locApiV02Instance->errorCb(clientHandle, errorId);
}

/* global structure containing the callbacks */
locClientCallbacksType globalCallbacks =
{
    sizeof(locClientCallbacksType),
    globalEventCb,
    globalRespCb,
    globalErrorCb
};

IzatApiV02::IzatApiV02(const MsgTask* msgTask,
                       LOC_API_ADAPTER_EVENT_MASK_T exMask,
                       ContextBase* context) :
    IzatApiBase(msgTask, exMask, context),
    mBatchSize(0),
    mDesiredBatchSize(0),
    mTimeBasedTrackingInProgress(false),
    mDistanceBasedTrackingInProgress(false),
    mBatchingInProgress(false),
    mLastInjectPosReqTime(0),
    clientHandle(LOC_CLIENT_INVALID_HANDLE_VALUE),
    mQmiMask(0)
{
    LOC_LOGL1("IzatApiV02 created. izatApiV02:%p",
          this);
    // initialize loc_sync_req interface
    loc_sync_req_init();
}

/* Destructor for IzatApiV02 */
IzatApiV02 :: ~IzatApiV02()
{
    close();
}


/* Initialize a loc api v02 client AND
   check which loc message are supported by modem */
loc_api_adapter_err
IzatApiV02 :: open(LOC_API_ADAPTER_EVENT_MASK_T mask)
{
    loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;
    LOC_API_ADAPTER_EVENT_MASK_T newMask = mMask | (mask & ~mExcludedMask);
    locClientEventMaskType qmiMask = convertMask(newMask);
    LOC_LOGL5("Enter mMask: %x; mask: %x; newMask: %x mQmiMask: %lld qmiMask: %lld",
          mMask, mask, newMask, mQmiMask, qmiMask);
    /* If the client is already open close it first */
    if (LOC_CLIENT_INVALID_HANDLE_VALUE == clientHandle)
    {
        locClientStatusEnumType status = eLOC_CLIENT_SUCCESS;

        LOC_LOGL1("reference to this = %p passed in", this);
        /* initialize the loc api v02 interface, note that
           the locClientOpen() function will block if the
           service is unavailable for a fixed time out */


        // it is important to cap the mask here, because not all LocApi's
        // can enable the same bits, e.g. foreground and bckground.
        status = locClientOpen(adjustMaskForNoSession(qmiMask), &globalCallbacks,
                               &clientHandle, (void *)this);
        mMask = newMask;
        mQmiMask = qmiMask;
        if (eLOC_CLIENT_SUCCESS == status &&
            clientHandle != LOC_CLIENT_INVALID_HANDLE_VALUE ) {

            // Query for supported feature list
            locClientReqUnionType req_union;
            locClientStatusEnumType status = eLOC_CLIENT_SUCCESS;
            qmiLocGetSupportedFeatureReqMsgT_v02 getSupportedFeatureList_req;
            qmiLocGetSupportedFeatureIndMsgT_v02 getSupportedFeatureList_ind;

            memset(&getSupportedFeatureList_req, 0, sizeof(getSupportedFeatureList_req));
            memset(&getSupportedFeatureList_ind, 0, sizeof(getSupportedFeatureList_ind));

            req_union.pGetSupportedFeatureReq = &getSupportedFeatureList_req;
            status = loc_sync_send_req(clientHandle,
                                       QMI_LOC_GET_SUPPORTED_FEATURE_REQ_V02,
                                       req_union,
                                       LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                                       QMI_LOC_GET_SUPPORTED_FEATURE_IND_V02,
                                       &getSupportedFeatureList_ind);
            if (eLOC_CLIENT_SUCCESS != status) {
                LOC_LOGE1("Failed to get features supported: st %d", status);
            } else {
                LOC_LOGH1("Got list of features supported of length: %d",
                          getSupportedFeatureList_ind.feature_len);
                for (uint32_t i = 0; i < getSupportedFeatureList_ind.feature_len; i++) {
                    LOC_LOGH2("Bit-mask of supported features at index:%d is %d",
                              i, getSupportedFeatureList_ind.feature[i]);
                }
            }

            // cache the mpss engine capabilities
            mContext->setEngineCapabilities(0,
                (getSupportedFeatureList_ind.feature_len != 0 ? getSupportedFeatureList_ind.feature:
                 NULL), false);

            // broadcast handle engine up event
            handleEngineUpEvent();

        } else {
            mMask = 0;
            mQmiMask = 0;
            LOC_LOGSH1("locClientOpen failed, status = %s",
                          loc_get_v02_client_status_name(status));
            rtv = LOC_API_ADAPTER_ERR_FAILURE;
        }
    } else if (newMask != mMask) {
        // it is important to cap the mask here, because not all LocApi's
        // can enable the same bits, e.g. foreground and background.
        if (!registerEventMask(qmiMask)) {
            // we do not update mMask here, because it did not change
            // as the mask update has failed.
            rtv = LOC_API_ADAPTER_ERR_FAILURE;
        }
        else {
            mMask = newMask;
            mQmiMask = qmiMask;
        }
    }
    LOC_LOGL4("Exit mMask: %x; mask: %x mQmiMask: %llx qmiMask: %llx",
          mMask, mask, mQmiMask, qmiMask);

    return rtv;
}

bool
IzatApiV02::registerEventMask(locClientEventMaskType qmiMask)
{
    if (!mTimeBasedTrackingInProgress) {
        qmiMask = adjustMaskForNoSession(qmiMask);
    }
    LOC_LOGL2("mQmiMask=%lld qmiMask=%lld",
          mQmiMask, qmiMask);
    return locClientRegisterEventMask(clientHandle, qmiMask);
}

locClientEventMaskType
IzatApiV02::adjustMaskForNoSession(locClientEventMaskType qmiMask)
{
    LOC_LOGL1("before qmiMask=%lld", qmiMask);
    locClientEventMaskType clearMask = QMI_LOC_EVENT_MASK_POSITION_REPORT_V02 |
        QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02 |
        QMI_LOC_EVENT_MASK_NMEA_V02 |
        QMI_LOC_EVENT_MASK_ENGINE_STATE_V02 |
        QMI_LOC_EVENT_MASK_BATCHING_STATUS_V02;

    qmiMask = qmiMask & ~clearMask;
    LOC_LOGL1("after qmiMask=%lld", qmiMask);
    return qmiMask;
}

loc_api_adapter_err IzatApiV02 :: close()
{
    loc_api_adapter_err rtv =
        // success if either client is already invalid, or
        // we successfully close the handle
        (LOC_CLIENT_INVALID_HANDLE_VALUE == clientHandle ||
         eLOC_CLIENT_SUCCESS == locClientClose(&clientHandle)) ?
        LOC_API_ADAPTER_ERR_SUCCESS : LOC_API_ADAPTER_ERR_FAILURE;

    mMask = 0;
    clientHandle = LOC_CLIENT_INVALID_HANDLE_VALUE;

    return rtv;
}
/* Convert event mask from loc eng to loc_api_v02 format */
locClientEventMaskType IzatApiV02 :: convertMask(
    LOC_API_ADAPTER_EVENT_MASK_T mask)
{
    locClientEventMaskType eventMask = 0;
    LOC_LOGL1("adapter mask = %u", mask);

    if (mask & LOC_API_ADAPTER_BIT_PARSED_POSITION_REPORT)
        eventMask |= QMI_LOC_EVENT_MASK_POSITION_REPORT_V02;

    if (mask & LOC_API_ADAPTER_BIT_SATELLITE_REPORT)
        eventMask |= QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02;

    /* treat NMEA_1Hz and NMEA_POSITION_REPORT the same*/
    if ((mask & LOC_API_ADAPTER_BIT_NMEA_POSITION_REPORT) ||
        (mask & LOC_API_ADAPTER_BIT_NMEA_1HZ_REPORT) )
        eventMask |= QMI_LOC_EVENT_MASK_NMEA_V02;

    if (mask & LOC_API_ADAPTER_BIT_ASSISTANCE_DATA_REQUEST)
    {
        // TBD: This needs to be decoupled in the HAL
        eventMask |= QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ_V02;
        eventMask |= QMI_LOC_EVENT_MASK_INJECT_TIME_REQ_V02;
        eventMask |= QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ_V02;
    }

    if (mask & LOC_API_ADAPTER_BIT_STATUS_REPORT)
    {
        eventMask |= (QMI_LOC_EVENT_MASK_ENGINE_STATE_V02);
    }

    if (mask & LOC_API_ADAPTER_GEOFENCE_GEN_ALERT)
        eventMask |= QMI_LOC_EVENT_MASK_GEOFENCE_GEN_ALERT_V02;

    if (mask & LOC_API_ADAPTER_REPORT_GENFENCE_BREACH)
        eventMask |= QMI_LOC_EVENT_MASK_GEOFENCE_BREACH_NOTIFICATION_V02;

    if (mask & LOC_API_ADAPTER_BATCHED_GENFENCE_BREACH_REPORT)
        eventMask |= QMI_LOC_EVENT_MASK_GEOFENCE_BATCH_BREACH_NOTIFICATION_V02;

    if(mask & LOC_API_ADAPTER_BIT_BATCH_FULL)
        eventMask |= QMI_LOC_EVENT_MASK_BATCH_FULL_NOTIFICATION_V02;

    if(mask & LOC_API_ADAPTER_BIT_BATCHED_POSITION_REPORT)
        eventMask |= QMI_LOC_EVENT_MASK_LIVE_BATCHED_POSITION_REPORT_V02;

    if(mask & LOC_API_ADAPTER_BIT_REQUEST_GTPWWAN_DATA)
        eventMask |= QMI_LOC_EVENT_MASK_FDCL_SERVICE_REQ_V02;

    return eventMask;
}

/* Convert error from loc_api_v02 to loc eng format*/
loc_api_adapter_err IzatApiV02 :: convertErr(
    locClientStatusEnumType status)
{
    switch( status)
    {
    case eLOC_CLIENT_SUCCESS:
        return LOC_API_ADAPTER_ERR_SUCCESS;

    case eLOC_CLIENT_FAILURE_GENERAL:
        return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;

    case eLOC_CLIENT_FAILURE_UNSUPPORTED:
        return LOC_API_ADAPTER_ERR_UNSUPPORTED;

    case eLOC_CLIENT_FAILURE_INVALID_PARAMETER:
        return LOC_API_ADAPTER_ERR_INVALID_PARAMETER;

    case eLOC_CLIENT_FAILURE_ENGINE_BUSY:
        return LOC_API_ADAPTER_ERR_ENGINE_BUSY;

    case eLOC_CLIENT_FAILURE_PHONE_OFFLINE:
        return LOC_API_ADAPTER_ERR_PHONE_OFFLINE;

    case eLOC_CLIENT_FAILURE_TIMEOUT:
        return LOC_API_ADAPTER_ERR_TIMEOUT;

    case eLOC_CLIENT_FAILURE_INVALID_HANDLE:
        return LOC_API_ADAPTER_ERR_INVALID_HANDLE;

    case eLOC_CLIENT_FAILURE_SERVICE_NOT_PRESENT:
        return LOC_API_ADAPTER_ERR_SERVICE_NOT_PRESENT;

    case eLOC_CLIENT_FAILURE_INTERNAL:
        return LOC_API_ADAPTER_ERR_INTERNAL;

    default:
        return LOC_API_ADAPTER_ERR_FAILURE;
    }
}


void
IzatApiV02::eventCb(locClientHandleType clientHandle,
                    uint32_t eventId,
                    locClientEventIndUnionType eventPayload)

{
    LOC_LOGL1("event id = 0x%X", eventId);

    switch(eventId) {
        case QMI_LOC_EVENT_BATCH_FULL_NOTIFICATION_IND_V02:
        {
            if (eventPayload.pBatchCount!= NULL) {
                LOC_LOGL1("Got QMI_LOC_EVENT_BATCH_FULL_NOTIFICATION_IND_V02"
                      " Count is %d", eventPayload.pBatchCount->batchCount);
                handleBatchFullEvent(eventPayload.pBatchCount->batchCount);
            }
            break;
        }
        case QMI_LOC_EVENT_DBT_POSITION_REPORT_IND_V02:
        {
            LOC_LOGL0("Got QMI_LOC_EVENT_DBT_POSITION_REPORT_IND_V02");
            handleDbtPositionEvent(eventPayload.pDbtPositionReportEvent);
            break;
        }
        case QMI_LOC_EVENT_POSITION_REPORT_IND_V02:
        {
            LOC_LOGL0("Got QMI_LOC_EVENT_POSITION_REPORT_IND_V02");
            handlePositionEvent(eventPayload.pPositionReportEvent);
            break;
        }
        case QMI_LOC_EVENT_GEOFENCE_BREACH_NOTIFICATION_IND_V02:
        {
            LOC_LOGL0("Got QMI_LOC_EVENT_GEOFENCE_BREACH_NOTIFICATION_IND_V02");
            handleGeofenceBreachEvent(eventPayload.pGeofenceBreachEvent);
            break;
        }
        case QMI_LOC_EVENT_GEOFENCE_GEN_ALERT_IND_V02:
        {
            LOC_LOGL0("Got QMI_LOC_EVENT_GEOFENCE_GEN_ALERT_IND_V02");
            handleGeofenceGenAlertEvent(eventPayload.pGeofenceGenAlertEvent);
            break;
        }
        /*NMEA*/ 
        case QMI_LOC_EVENT_NMEA_IND_V02:
          handleNmeaEvent(eventPayload.pNmeaReportEvent);
          break;
        /*XTRA request*/ 
        case QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_IND_V02:
        {
            LOC_LOGL0("XTRA download request");

            const char* servers[] = {"","",""};
            qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02* pMsg =
                    eventPayload.pInjectPredictedOrbitsReqEvent;
            for (unsigned int idx = 0; idx < MIN(pMsg->serverList.serverList_len, 3); idx++) {
                servers[idx] = pMsg->serverList.serverList[idx].serverUrl;
            }
            handleReceiveXtraServers(servers[0], servers[1], servers[2]);
            handleRequestXtraData();
            break;
        }
        /*time request*/ 
        case QMI_LOC_EVENT_INJECT_TIME_REQ_IND_V02:
        {
            LOC_LOGL0("Time request");
            handleRequestNtpTime();
            break;
        }
        case QMI_LOC_EVENT_FDCL_SERVICE_REQ_IND_V02:{
          LOC_LOGH0("Got GTP WWAN service Req");
          handleBsObservedDataServiceEvent(eventPayload.pfdclserviceEvent);
          break;
        }
        /*SV info indication*/
        case QMI_LOC_EVENT_GNSS_SV_INFO_IND_V02:{
            LOC_LOGH0("Got SV Info");
            handleSvInfoEventIndication(eventPayload.svInfoEvent);
            break;
        }
        default:
        {
            break;
        }
    }
}

void IzatApiV02 :: errorCb(locClientHandleType handle,
                           locClientErrorEnumType errorId)
{
    if(errorId == eLOC_CLIENT_ERROR_SERVICE_UNAVAILABLE)
    {
        LOC_LOGH0("Service unavailable error");

        handleEngineDownEvent();

        /* immediately send the engine up event so that
           the loc engine re-initializes the adapter and the
           izat-api_v02 interface */

        handleEngineUpEvent();
    }
}

/*
  Returns
  0: update the gps reporting event successfully
  -1: on failure
*/
int IzatApiV02 :: updateRegistrationMask(LOC_API_ADAPTER_EVENT_MASK_T event,
                                         loc_registration_mask_status isEnabled)
{
    LOC_LOGL0("Enter");

    return open((isEnabled == LOC_REGISTRATION_MASK_ENABLED)?(mMask|event):(mMask&~event));
}

/** called from the qmi thread.*/
void
IzatApiV02::handleBatchFullEvent(size_t count)
{
    ENTRY_LOG();

    struct FlpGetBatchedLocations : public LocMsg {
        IzatApiBase& mIzatApi;
        size_t mCount;
        inline FlpGetBatchedLocations(IzatApiBase& izatApi,
                                      size_t count) :
            LocMsg(),
            mIzatApi(izatApi),
            mCount(count) {}
        inline virtual void proc() const {
            LOC_LOGL0("FlpGetBatchedLocation struct");
            mIzatApi.getBatchedLocations(mCount);
        }
    };

    sendMsg(new FlpGetBatchedLocations(*this, count));
}

/* inject position into the position engine */
loc_api_adapter_err IzatApiV02::injectPosition(
        double latitude, double longitude, float accuracy)
{
  locClientReqUnionType req_union;
  locClientStatusEnumType status;
  qmiLocInjectPositionReqMsgT_v02 inject_pos_msg;
  qmiLocInjectPositionIndMsgT_v02 inject_pos_ind = { 0 };

  memset(&inject_pos_msg, 0, sizeof(inject_pos_msg));

  inject_pos_msg.latitude_valid = 1;
  inject_pos_msg.latitude = latitude;

  inject_pos_msg.longitude_valid = 1;
  inject_pos_msg.longitude = longitude;

  inject_pos_msg.horUncCircular_valid = 1;

  inject_pos_msg.horUncCircular = accuracy; //meters assumed

  inject_pos_msg.horConfidence_valid = 1;

  inject_pos_msg.horConfidence = 63; // 63% (1 std dev assumed)

    /* Log */
  LOC_LOGM6("injectLocation: LAT: %d.%05d LON: %d.%05d ACC: %d.%02d",
          (int)inject_pos_msg.latitude, (abs((int)(inject_pos_msg.latitude*100000)))%100000,
          (int)inject_pos_msg.longitude, (abs((int)(inject_pos_msg.longitude*100000)))%100000,
          (int)inject_pos_msg.horUncCircular, (abs((int)(inject_pos_msg.horUncCircular*100)))%100);


  req_union.pInjectPositionReq = &inject_pos_msg;

  status = loc_sync_send_req(clientHandle,
                             QMI_LOC_INJECT_POSITION_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_INJECT_POSITION_IND_V02,
                             &inject_pos_ind);

  if (status != eLOC_CLIENT_SUCCESS ||
      eQMI_LOC_SUCCESS_V02 != inject_pos_ind.status)
  {
    LOC_LOGSE2("error! status = %s, inject_pos_ind.status = %s\n",
              loc_get_v02_client_status_name(status),
              loc_get_v02_qmi_status_name(inject_pos_ind.status));

    return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}

LocationError IzatApiV02::getZppFix(Location& zppLoc)
{
  locClientReqUnionType req_union;
  qmiLocGetBestAvailablePositionIndMsgT_v02 zpp_ind;
  qmiLocGetBestAvailablePositionReqMsgT_v02 zpp_req;

  memset(&zpp_ind, 0, sizeof(zpp_ind));
  memset(&zpp_req, 0, sizeof(zpp_req));
  memset(&zppLoc, 0, sizeof(zppLoc));

  req_union.pGetBestAvailablePositionReq = &zpp_req;

  LOC_LOGM0("Get ZPP Fix");

  locClientStatusEnumType status = loc_sync_send_req(clientHandle,
                             QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND_V02,
                             &zpp_ind);

  if (status != eLOC_CLIENT_SUCCESS ||
      eQMI_LOC_SUCCESS_V02 != zpp_ind.status)
  {
      LOC_LOGE2("error! status = %d, zpp_ind.status = %d", status, zpp_ind.status);
      return LOCATION_ERROR_GENERAL_FAILURE;
  }

  LOC_LOGM4("Got Zpp fix location validity (lat:%d, lon:%d, timestamp:%d accuracy:%d)",
                   zpp_ind.latitude_valid,
                   zpp_ind.longitude_valid,
                   zpp_ind.timestampUtc_valid,
                   zpp_ind.horUncCircular_valid);

  LOC_LOGL8("LAT: %d.%05d LON: %d.%05d ACC: %d.%02d TS: 0x%x%x",
        (int)zpp_ind.latitude, (abs((int)(zpp_ind.latitude * 100000))) % 100000,
        (int)zpp_ind.longitude, (abs((int)(zpp_ind.longitude * 100000))) % 100000,
        (int)zpp_ind.horUncCircular, (abs((int)(zpp_ind.horUncCircular * 100))) % 100,
        zpp_ind.timestampUtc >> 32, zpp_ind.timestampUtc);

  zppLoc.size = sizeof(Location);
  if (zpp_ind.timestampUtc_valid) {
    zppLoc.timestamp = zpp_ind.timestampUtc;
  }

  if (zpp_ind.latitude_valid &&
      zpp_ind.longitude_valid &&
      zpp_ind.horUncCircular_valid)
  {
      zppLoc.flags |= (LOCATION_HAS_LAT_LONG_BIT | LOCATION_HAS_ACCURACY_BIT);
      zppLoc.latitude = zpp_ind.latitude;
      zppLoc.longitude = zpp_ind.longitude;
      zppLoc.accuracy = zpp_ind.horUncCircular;

      // If horCircularConfidence_valid is true, and horCircularConfidence value
      // is less than 68%, then scale the accuracy value to 68% confidence.
      if (zpp_ind.horCircularConfidence_valid)
      {
          scaleAccuracyTo68PercentConfidence(zpp_ind.horCircularConfidence,
                                             zppLoc, true);
      }

  } else {
      LOC_LOGE0("One of lat/long/accuracy missing.");
      return LOCATION_ERROR_BEST_AVAIL_POS_INVALID;
  }

  if (zpp_ind.altitudeWrtEllipsoid_valid) {
      zppLoc.flags |= LOCATION_HAS_ALTITUDE_BIT;
      zppLoc.altitude = zpp_ind.altitudeWrtEllipsoid;
  }

  if (zpp_ind.altitudeWrtMeanSeaLevel_valid) {
      zppLoc.flags |= LOCATION_HAS_ALTITUDE_MSL_BIT;
      zppLoc.altitudeMeanSeaLevel = zpp_ind.altitudeWrtMeanSeaLevel;
  }

  if (zpp_ind.horSpeed_valid) {
      zppLoc.flags |= LOCATION_HAS_SPEED_BIT;
      zppLoc.speed = zpp_ind.horSpeed;
  }

  if (zpp_ind.horSpeedUnc_valid) {
      zppLoc.flags |= LOCATION_HAS_SPEED_ACCURACY_BIT;
      zppLoc.speedAccuracy = zpp_ind.horSpeedUnc;
  }

  if (zpp_ind.heading_valid) {
      zppLoc.flags |= LOCATION_HAS_BEARING_BIT;
      zppLoc.bearing = zpp_ind.heading;
  }

  if (zpp_ind.headingUnc_valid) {
      zppLoc.flags |= LOCATION_HAS_BEARING_ACCURACY_BIT;
      zppLoc.bearingAccuracy = zpp_ind.headingUnc;
  }

  if (zpp_ind.vertUnc_valid) {
      zppLoc.flags |= LOCATION_HAS_VERTICAL_ACCURACY_BIT;
      zppLoc.verticalAccuracy = zpp_ind.vertUnc;
  }

  return LOCATION_ERROR_SUCCESS;
}

LocationError IzatApiV02::getGnssTime(GnssTime& time) {
    
    locClientReqUnionType req_union;
    qmiLocSystemInfoReqMsgT_v02 timeReq;
    qmiLocSystemInfoIndMsgT_v02 timeInd;

    memset(&time, 0, sizeof(time));
    memset(&timeReq, 0, sizeof(timeReq));
    memset(&timeInd, 0, sizeof(timeInd));

    timeReq.systemInfo = eQMI_LOC_NEXT_LEAP_SECOND_INFO_V02;
    req_union.pGnssTimeReq = &timeReq;

    LOC_LOGM0("GetGnssTime");

    locClientStatusEnumType status = loc_sync_send_req(clientHandle,
                             QMI_LOC_SYSTEM_INFO_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_SYSTEM_INFO_IND_V02,
                             &timeInd);

    if (eLOC_CLIENT_SUCCESS != status) {
        LOC_LOGE1("Send request failed status = %d", status);
        return LOCATION_ERROR_GENERAL_FAILURE;
    }

    if(eQMI_LOC_NEXT_LEAP_SECOND_INFO_V02 != timeInd.systemInfo || 
        0 == timeInd.nextLeapSecondInfo.gpsTimeCurrent_valid || 
        0 == timeInd.nextLeapSecondInfo_valid) {

        LOC_LOGE3("GNSS time not available, SystemInfo type %d, nextLeapSecondInfo_valid = %d, gpsTimeCurrent_valid = %d",
                    timeInd.systemInfo,
                    timeInd.nextLeapSecondInfo_valid,
                    timeInd.nextLeapSecondInfo.gpsTimeCurrent_valid);
      
        return LOCATION_ERROR_GENERAL_FAILURE;
    }

    time.gpsWeek = timeInd.nextLeapSecondInfo.gpsTimeCurrent.gpsWeek;
    time.gpsTimeOfWeekMs = timeInd.nextLeapSecondInfo.gpsTimeCurrent.gpsTimeOfWeekMs;
    time.leapSecondsCurrent = timeInd.nextLeapSecondInfo.leapSecondsCurrent;
    time.leapSecondsCurrent_valid = timeInd.nextLeapSecondInfo.leapSecondsCurrent_valid;
    time.gpsTimeUncMs_valid = 0;
    
    LOC_LOGM1("time gpsTimeUncMs_valid %d", timeInd.gpsTimeUncMs_valid);
    if (timeInd.gpsTimeUncMs_valid)
    {
        time.gpsTimeUncMs_valid = 1;
        time.gpsTimeUncMs = timeInd.gpsTimeUncMs;
        LOC_LOGM1("Got GNSS Time Unc of %d Ms", time.gpsTimeUncMs);
    }
    LOC_LOGM4("Got GNSS Time. Week : %d, TimeOfTheWeek : %d, currentLS_Valid : %d, currentLS : %d",
                   time.gpsWeek,
                   time.gpsTimeOfWeekMs,
                   time.leapSecondsCurrent_valid,
                   time.leapSecondsCurrent);

    return LOCATION_ERROR_SUCCESS;
}

LocationError IzatApiV02::getWwanZppFix(const GtpOptions& options)
{
    locClientReqUnionType req_union;
    qmiLocGetAvailWwanPositionReqMsgT_v02 zpp_req;
    memset(&zpp_req, 0, sizeof(zpp_req));

    zpp_req.transactionId = wwanZppFixTransactionId++;
    if (options.timeout == 0) {
        zpp_req.gtpLocMask_valid = 1;
        zpp_req.timeOut_valid = 0;
        zpp_req.gtpLocMask = QMI_LOC_GTP_BASIC_MASK_V02;
    } else {
        zpp_req.gtpLocMask_valid = 1;
        zpp_req.timeOut_valid = 1;
        zpp_req.gtpLocMask = QMI_LOC_GTP_ENHANCED_MASK_V02;
    }
	  zpp_req.timeOut = options.timeout;

    req_union.pGetAvailWwanPositionReq = &zpp_req;
    LOC_LOGM1("Get ZPP Fix from available WWAN position %d", zpp_req.gtpLocMask);

    locClientStatusEnumType status =
        locClientSendReq(clientHandle,
                         QMI_LOC_GET_AVAILABLE_WWAN_POSITION_REQ_V02,
                         req_union);

    if (eLOC_CLIENT_SUCCESS == status) {

        sendMsg(new MsgReportRequestWwanFix(*this));
        return LOCATION_ERROR_SUCCESS;

    } else if (eLOC_CLIENT_FAILURE_UNSUPPORTED == status){
        return LOCATION_ERROR_LOCATION_DISABLED;
    } else {
        return LOCATION_ERROR_GENERAL_FAILURE;
    }
}

void IzatApiV02::handleWwanZppFixIndication(
        const qmiLocGetAvailWwanPositionIndMsgT_v02& zppInd)
{
    Location zppLoc;
    memset(&zppLoc, 0, sizeof(zppLoc));

    LOC_LOGM5("WWAN ZPP fix: status: %d, isValid: "
              "(lat:%d, lon:%d, ts:%d unc:%d)", zppInd.status,
              zppInd.latitude_valid, zppInd.longitude_valid,
              zppInd.timestampUtc_valid, zppInd.horUncCircular_valid);

    LOC_LOGM8("WWAN ZPP Fix: vals: LAT: %d.%05d LON: %d.%05d UNC: %d.%02d TS: 0x%x%x",
            (int)zppInd.latitude, (abs((int)(zppInd.latitude*100000)))%100000,
            (int)zppInd.longitude, (abs((int)(zppInd.longitude*100000)))%100000,
            (int)zppInd.horUncCircular, (abs((int)(zppInd.horUncCircular*100)))%100,
            zppInd.timestampUtc >> 32, zppInd.timestampUtc);

    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    if (eQMI_LOC_SUCCESS_V02 == zppInd.status) {

        if ((false == zppInd.latitude_valid) ||
            (false == zppInd.longitude_valid) ||
            (false == zppInd.horUncCircular_valid)) {

            LOC_LOGE3(" Location not valid lat=%u lon=%u unc=%u",
                    zppInd.latitude_valid,
                    zppInd.longitude_valid,
                    zppInd.horUncCircular_valid);

            err = LOCATION_ERROR_GENERAL_FAILURE;

        } else {

            zppLoc.size = sizeof(Location);

            if (zppInd.timestampUtc_valid) {
                zppLoc.timestamp = zppInd.timestampUtc;
            }

            zppLoc.flags = LOCATION_HAS_LAT_LONG_BIT | LOCATION_HAS_ACCURACY_BIT;
            zppLoc.latitude = zppInd.latitude;
            zppLoc.longitude = zppInd.longitude;
            zppLoc.accuracy = zppInd.horUncCircular;

            // If horCircularConfidence_valid is true, and horCircularConfidence value
            // is less than 68%, then scale the accuracy value to 68% confidence.
            if (zppInd.horCircularConfidence_valid)
            {
                scaleAccuracyTo68PercentConfidence(zppInd.horCircularConfidence,
                                                   zppLoc, true);
            }

            if (zppInd.altitudeWrtEllipsoid_valid) {
                zppLoc.flags |= LOCATION_HAS_ALTITUDE_BIT;
                zppLoc.altitude = (double)zppInd.altitudeWrtEllipsoid;
            }

            if (zppInd.vertUnc_valid) {
                zppLoc.flags |= LOCATION_HAS_VERTICAL_ACCURACY_BIT;
                zppLoc.verticalAccuracy = zppInd.vertUnc;
            }

            err = LOCATION_ERROR_SUCCESS;
        }
    } else if (eQMI_LOC_UNSUPPORTED_V02 == zppInd.status) {

        err = LOCATION_ERROR_LOCATION_DISABLED;
    }

    sendMsg(new MsgReportWwanZppFix(*this, zppLoc, err));
}

int IzatApiV02::getWwanScanData(uint8_t numOfCells,
                bool isAssisted, 
                uint8_t airInterfaceType)
{
    locClientReqUnionType req_union;
    qmiLocGetWwanDataReqMsgT_v02 data_req;
    memset(&data_req, 0, sizeof(data_req));

    data_req.numOfCells = numOfCells;
    data_req.isAssisted = isAssisted;
    data_req.airInterface = airInterfaceType;

    req_union.pWwanDataReq = &data_req;

    LOC_LOGM0("Get WWAN scan Data from modem");

    locClientStatusEnumType status =
        locClientSendReq(clientHandle,
                         QMI_LOC_GET_WWAN_DATA_REQ_V02,
                         req_union);
return status;
}

void IzatApiV02::handleWwanDataIndication(
  const qmiLocGetWwanDataIndMsgT_v02 &ind)
{
#ifdef FEATURE_LOCATION_MCS
  LOC_LOGE0("handleWwanDataIndication");
  sendMsg(new MsgReportWwanScanList(*this, ind));
#endif
}

/* If Confidence value is less than 68%, then scale the accuracy value to
   68%.confidence.*/
void IzatApiV02::scaleAccuracyTo68PercentConfidence(
                                                const uint8_t confidenceValue,
                                                Location &gpsLocation,
                                                const bool isCircularUnc)
{
    if (confidenceValue < 68)
    {
        // Circular uncertainty is at 63%.confidence. Scale factor should be
        // 1.072(from 63% -> 68%)
        uint8_t realConfidence = (isCircularUnc) ? 63:confidenceValue;
        // get scaling value based on 2D% confidence scaling table
        for (uint8_t iter = 0; iter < CONF_SCALER_ARRAY_MAX; iter++)
        {
            if (realConfidence <= confScalers[iter].confidence)
            {
                LOC_LOGL3("Confidence: %d, Scaler value:%d.%d",
                            realConfidence, (int)(confScalers[iter].scaler_to_68),
                            (int)(confScalers[iter].scaler_to_68 * 10) % 10);
                gpsLocation.accuracy *= confScalers[iter].scaler_to_68;
                break;
            }
        }
    }
}

/** called from the qmi thread.*/
void
IzatApiV02::handlePositionEvent (
  const qmiLocEventPositionReportIndMsgT_v02 *pPosReport)
{
    if (NULL == pPosReport)
        return;

    Location location = {0};
    location.size = sizeof(Location);

    GpsLocationExtended locationExtended = {0};
    locationExtended.size = sizeof(locationExtended);

    // Process the position from final and intermediate reports
    LOC_LOGL2("Position report with sess status = %d fix id = %u",
          pPosReport->sessionStatus, pPosReport->fixId);
    
    if( (pPosReport->sessionStatus == eQMI_LOC_SESS_STATUS_SUCCESS_V02) ||
        (pPosReport->sessionStatus == eQMI_LOC_SESS_STATUS_IN_PROGRESS_V02)) 
    {
        // Latitude & Longitude
        if( (pPosReport->latitude_valid == 1 ) &&
            (pPosReport->longitude_valid == 1) )
        {
            location.flags  |= LOCATION_HAS_LAT_LONG_BIT;
            location.latitude  = pPosReport->latitude;
            location.longitude = pPosReport->longitude;
            // Time stamp (UTC)
            if(pPosReport->timestampUtc_valid == 1)
            {
                location.timestamp = pPosReport->timestampUtc;
            }

            // Altitude
            if(pPosReport->altitudeWrtEllipsoid_valid == 1  )
            {
                location.flags  |= LOCATION_HAS_ALTITUDE_BIT;
                location.altitude = pPosReport->altitudeWrtEllipsoid;
            }
            if(pPosReport->altitudeWrtMeanSeaLevel_valid == 1)
            {
                location.flags |= LOCATION_HAS_ALTITUDE_MSL_BIT;
                location.altitudeMeanSeaLevel = pPosReport->altitudeWrtMeanSeaLevel;
            }

            // Speed
            if(pPosReport->speedHorizontal_valid == 1)
            {
                location.flags  |= LOCATION_HAS_SPEED_BIT;
                location.speed = pPosReport->speedHorizontal;
            }

            // Heading
            if(pPosReport->heading_valid == 1)
            {
                location.flags  |= LOCATION_HAS_BEARING_BIT;
                location.bearing = pPosReport->heading;
            }

            // Uncertainty (circular)
            if (pPosReport->horUncCircular_valid)
            {
                location.flags |= LOCATION_HAS_ACCURACY_BIT;
                location.accuracy = pPosReport->horUncCircular;

                // If horConfidence_valid is true, and horConfidence value is less than 68%
                // then scale the accuracy value to 68% confidence.
                if (pPosReport->horConfidence_valid)
                {
                    scaleAccuracyTo68PercentConfidence(pPosReport->horConfidence,
                                                       location, true);
                }

            } else if (pPosReport->horUncEllipseSemiMinor_valid &&
                       pPosReport->horUncEllipseSemiMajor_valid) {
                location.flags |= LOCATION_HAS_ACCURACY_BIT;
                location.accuracy =
                    sqrt((pPosReport->horUncEllipseSemiMinor *
                          pPosReport->horUncEllipseSemiMinor) +
                         (pPosReport->horUncEllipseSemiMajor *
                          pPosReport->horUncEllipseSemiMajor));
            }

            // Vertical Uncertainty
            if (pPosReport->vertUnc_valid) {
                location.flags |= LOCATION_HAS_VERTICAL_ACCURACY_BIT;
                location.verticalAccuracy = pPosReport->vertUnc;
            }

            // Speed Uncertainty
            if (pPosReport->speedUnc_valid) {
                location.flags |= LOCATION_HAS_SPEED_ACCURACY_BIT;
                location.speedAccuracy = pPosReport->speedUnc;
            }

            // Bearing Uncertainty
            if (pPosReport->headingUnc_valid) {
                location.flags |= LOCATION_HAS_BEARING_ACCURACY_BIT;
                location.bearingAccuracy = pPosReport->headingUnc;
            }

            /** Populate Extended Location **/

            if (pPosReport->altitudeWrtMeanSeaLevel_valid)
            {
                locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL;
                locationExtended.altitudeMeanSeaLevel = pPosReport->altitudeWrtMeanSeaLevel;
            }
            if (pPosReport->DOP_valid)
            {
                locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_DOP;
                locationExtended.pdop = pPosReport->DOP.PDOP;
                locationExtended.hdop = pPosReport->DOP.HDOP;
                locationExtended.vdop = pPosReport->DOP.VDOP;
            }
            if (pPosReport->magneticDeviation_valid)
            {
                locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_MAG_DEV;
                locationExtended.magneticDeviation = pPosReport->magneticDeviation;
            }
            if (pPosReport->vertUnc_valid)
            {
               locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_VERT_UNC;
               locationExtended.vert_unc = pPosReport->vertUnc;
            }
            if (pPosReport->speedUnc_valid)
            {
               locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_SPEED_UNC;
               locationExtended.speed_unc = pPosReport->speedUnc;
            }
            if (pPosReport->headingUnc_valid)
            {
               locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_BEARING_UNC;
               locationExtended.bearing_unc = pPosReport->headingUnc;
            }
            if (pPosReport->horUncEllipseSemiMajor_valid)
            {
                locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MAJOR;
                locationExtended.horUncEllipseSemiMajor = pPosReport->horUncEllipseSemiMajor;
            }
            if (pPosReport->horUncEllipseSemiMinor_valid)
            {
                locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MINOR;
                locationExtended.horUncEllipseSemiMinor = pPosReport->horUncEllipseSemiMinor;
            }
            if (pPosReport->horUncEllipseOrientAzimuth_valid)
            {
                locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_AZIMUTH;
                locationExtended.horUncEllipseOrientAzimuth = pPosReport->horUncEllipseOrientAzimuth;
            }
            if (pPosReport->horReliability_valid)
            {
               locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_HOR_RELIABILITY;
               switch(pPosReport->horReliability)
               {
                  case eQMI_LOC_RELIABILITY_NOT_SET_V02 :
                    locationExtended.horizontal_reliability = LOC_RELIABILITY_NOT_SET_V02;
                    break;
                  case eQMI_LOC_RELIABILITY_VERY_LOW_V02 :
                    locationExtended.horizontal_reliability = LOC_RELIABILITY_VERY_LOW_V02;
                    break;
                  case eQMI_LOC_RELIABILITY_LOW_V02 :
                    locationExtended.horizontal_reliability = LOC_RELIABILITY_LOW_V02;
                    break;
                  case eQMI_LOC_RELIABILITY_MEDIUM_V02 :
                    locationExtended.horizontal_reliability = LOC_RELIABILITY_MEDIUM_V02;
                    break;
                  case eQMI_LOC_RELIABILITY_HIGH_V02 :
                    locationExtended.horizontal_reliability = LOC_RELIABILITY_HIGH_V02;
                    break;
                  default:
                    locationExtended.horizontal_reliability = LOC_RELIABILITY_NOT_SET_V02;
                    break;
               }
            }
            if (pPosReport->vertReliability_valid)
            {
               locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_VERT_RELIABILITY;
               switch(pPosReport->vertReliability)
               {
                  case eQMI_LOC_RELIABILITY_NOT_SET_V02 :
                    locationExtended.vertical_reliability = LOC_RELIABILITY_NOT_SET_V02;
                    break;
                  case eQMI_LOC_RELIABILITY_VERY_LOW_V02 :
                    locationExtended.vertical_reliability = LOC_RELIABILITY_VERY_LOW_V02;
                    break;
                  case eQMI_LOC_RELIABILITY_LOW_V02 :
                    locationExtended.vertical_reliability = LOC_RELIABILITY_LOW_V02;
                    break;
                  case eQMI_LOC_RELIABILITY_MEDIUM_V02 :
                    locationExtended.vertical_reliability = LOC_RELIABILITY_MEDIUM_V02;
                    break;
                  case eQMI_LOC_RELIABILITY_HIGH_V02 :
                    locationExtended.vertical_reliability = LOC_RELIABILITY_HIGH_V02;
                    break;
                  default:
                    locationExtended.vertical_reliability = LOC_RELIABILITY_NOT_SET_V02;
                    break;
               }
            }

            if (((pPosReport->expandedGnssSvUsedList_valid) &&
                    (pPosReport->expandedGnssSvUsedList_len != 0)) ||
                    ((pPosReport->gnssSvUsedList_valid) &&
                    (pPosReport->gnssSvUsedList_len != 0)))
            {
                uint32_t idx=0;
                uint32_t gnssSvUsedList_len = 0;
                uint16_t gnssSvIdUsed = 0;
                const uint16_t *svUsedList;
//                bool multiBandTypesAvailable = false;

                if (pPosReport->expandedGnssSvUsedList_valid)
                {
                    gnssSvUsedList_len = pPosReport->expandedGnssSvUsedList_len;
                    svUsedList = pPosReport->expandedGnssSvUsedList;
                } else if (pPosReport->gnssSvUsedList_valid)
                {
                    gnssSvUsedList_len = pPosReport->gnssSvUsedList_len;
                    svUsedList = pPosReport->gnssSvUsedList;
                }

                LOC_LOGL1("sv length %d ", gnssSvUsedList_len);

                for (idx = 0; idx < gnssSvUsedList_len; idx++)
                {
                    gnssSvIdUsed = svUsedList[idx];
                    if (gnssSvIdUsed <= GPS_SV_PRN_MAX)
                    {
                        locationExtended.gnss_sv_used_ids.gps_sv_used_ids_mask |=
                            (1 << (gnssSvIdUsed - GPS_SV_PRN_MIN));
                    }
                    else if ((gnssSvIdUsed >= GLO_SV_PRN_MIN) && (gnssSvIdUsed <= GLO_SV_PRN_MAX))
                    {
                        locationExtended.gnss_sv_used_ids.glo_sv_used_ids_mask |=
                            (1 << (gnssSvIdUsed - GLO_SV_PRN_MIN));
                    }
                    else if ((gnssSvIdUsed >= BDS_SV_PRN_MIN) && (gnssSvIdUsed <= BDS_SV_PRN_MAX))
                    {
                        locationExtended.gnss_sv_used_ids.bds_sv_used_ids_mask |=
                            (1 << (gnssSvIdUsed - BDS_SV_PRN_MIN));
                    }
                    else if ((gnssSvIdUsed >= GAL_SV_PRN_MIN) && (gnssSvIdUsed <= GAL_SV_PRN_MAX))
                    {
                        locationExtended.gnss_sv_used_ids.gal_sv_used_ids_mask |=
                            (1 << (gnssSvIdUsed - GAL_SV_PRN_MIN));
                    }
                    else if ((gnssSvIdUsed >= QZSS_SV_PRN_MIN) && (gnssSvIdUsed <= QZSS_SV_PRN_MAX))
                    {
                        locationExtended.gnss_sv_used_ids.qzss_sv_used_ids_mask |=
                            (1 << (gnssSvIdUsed - QZSS_SV_PRN_MIN));
                    }
                }
                locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_GNSS_SV_USED_DATA;
            }

            if (pPosReport->navSolutionMask_valid)
            {
               locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_NAV_SOLUTION_MASK;
               locationExtended.navSolutionMask = convertNavSolutionMask(pPosReport->navSolutionMask);
            }

            if (pPosReport->technologyMask_valid) {
                locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_POS_TECH_MASK;
                locationExtended.tech_mask = convertPosTechMask(pPosReport->technologyMask);
            }

            sendMsg(new MsgReportLocation(*this, location, locationExtended,
                    (pPosReport->sessionStatus ==
                            eQMI_LOC_SESS_STATUS_IN_PROGRESS_V02 ?
                                    LOC_SESS_INTERMEDIATE : LOC_SESS_SUCCESS)));
        }

    } else if (eQMI_LOC_SESS_STATUS_TIMEOUT_V02 == pPosReport->sessionStatus) {

        sendMsg(new MsgReportLocation(*this, location, locationExtended, LOC_SESS_TIMEOUT));

    } else if (eQMI_LOC_SESS_STATUS_GNSS_SERVICE_NOT_AVAILABLE_V02 == pPosReport->sessionStatus) {

        sendMsg(new MsgReportLocation(*this, location, locationExtended, LOC_SESS_DDR_UNAVAILABLE));

    } else if (eQMI_LOC_SESS_STATUS_GENERAL_FAILURE_V02 == pPosReport->sessionStatus) {

        sendMsg(new MsgReportLocation(*this, location, locationExtended, LOC_SESS_FAILURE));

    } else {

        LOC_LOGL2("Ignoring position report with sess status = %d "
            "fix id = %u", pPosReport->sessionStatus, pPosReport->fixId );
    }
}

void
IzatApiV02::handleBsObservedDataServiceEvent (
        const qmiLocEventFdclServiceReqIndMsgT_v02 *fdclServiceEvent)
{
  clientinformation* clientinfo = new clientinformation();

  if(clientinfo == NULL)
  {
    LOC_LOGE0("Failed to allocate memory for clientinfo");
    return;
  }

  if(fdclServiceEvent != NULL){
      clientinfo->gtpClientSwVer_valid = fdclServiceEvent->gtpClientSwVer_valid;
      if (fdclServiceEvent->gtpClientSwVer_valid){
          LOC_LOGL2("gtpClientSwMajorVersion = %d "
                  "gtpClientSwMinorVersion= %d", fdclServiceEvent->gtpClientSwVer.gtpClientSwMajorVersion, fdclServiceEvent->gtpClientSwVer.gtpClientSwMinorVersion);
          clientinfo->gtpClientSwMajorVersion = fdclServiceEvent->gtpClientSwVer.gtpClientSwMajorVersion;
          clientinfo->gtpClientSwMinorVersion =  fdclServiceEvent->gtpClientSwVer.gtpClientSwMinorVersion;
      }
      if(fdclServiceEvent->modemSwVersion_valid){
          clientinfo->modemSwVersion_valid = 1;
          LOC_LOGL1("modemSwVersion Is Valid  and string length %d",strlen(fdclServiceEvent->modemSwVersion));
          memscpy(clientinfo->modemSwVersion, sizeof(clientinfo->modemSwVersion),
                 fdclServiceEvent->modemSwVersion, sizeof(clientinfo->modemSwVersion));
      }


      if(fdclServiceEvent->homeCarrierId_valid){
          LOC_LOGL0("homeCarrierId Is Valid ");
          clientinfo->homeMccId = fdclServiceEvent->homeCarrierId.homeMcc;
          clientinfo->homeMncValid = fdclServiceEvent->homeCarrierId.homeMncValid;
          if(clientinfo->homeMncValid){
              clientinfo->homeMncId = fdclServiceEvent->homeCarrierId.homeMnc;
          }

      } 
      if(fdclServiceEvent->cdmaRealm_valid){
          LOC_LOGL0("cdmaRealm Is Valid ");
          //LOC_LOGSL1("gtpClientSwMajorVersion = %s",clientinfo->cdmaRealm);
          memscpy(clientinfo->cdmaRealm, sizeof(clientinfo->cdmaRealm),
                  fdclServiceEvent->cdmaRealm,sizeof(clientinfo->cdmaRealm));
      }

      if(fdclServiceEvent->gtpMaxNumRecordsAccepted_valid){
          clientinfo->gtpMaxNumRecordsAccepted_valid =1;
          LOC_LOGL1("gtpMaxNumRecordsAccepted Is Valid and value is %d",fdclServiceEvent->gtpMaxNumRecordsAccepted);
          clientinfo->gtpMaxNumRecordsAccepted = fdclServiceEvent->gtpMaxNumRecordsAccepted;
      }    
      if(fdclServiceEvent->gpsWeek_valid){
          LOC_LOGL0("gpsWeek Is Valid ");
          clientinfo->gpsWeek= fdclServiceEvent->gpsWeek;
      }
      if(fdclServiceEvent->gpsMilliseconds_valid){
          LOC_LOGL0("gpsMilliseconds Is Valid ");

          clientinfo->gpsMilliseconds= fdclServiceEvent->gpsMilliseconds;
      }

      if(fdclServiceEvent->currentCellId_valid){
          LOC_LOGL0("currentCellId Is Valid ");
          LOC_LOGSL4("id1,2,3,4 = %d,%d,%d,%d",fdclServiceEvent->currentCellId.id1,fdclServiceEvent->currentCellId.id2,fdclServiceEvent->currentCellId.id3,fdclServiceEvent->currentCellId.id4);

          clientinfo->currentCellId_valid = 1;
          clientinfo->currentCellId.id1 = fdclServiceEvent->currentCellId.id1;
          clientinfo->currentCellId.id2 = fdclServiceEvent->currentCellId.id2;
          clientinfo->currentCellId.id3 = fdclServiceEvent->currentCellId.id3;
          clientinfo->currentCellId.id4 = fdclServiceEvent->currentCellId.id4;
          clientinfo->currentCellId.airInterfaceType = fdclServiceEvent->currentCellId.airInterfaceType;

      }
      if(fdclServiceEvent->requestedDataTypes_valid){
          LOC_LOGL1("is valid requestedDataTypes_valid %d",fdclServiceEvent->requestedDataTypes);
          clientinfo->requestedDataTypes= fdclServiceEvent->requestedDataTypes;
          clientinfo->requestedDataTypes_valid = 1;
      }
      if(fdclServiceEvent->restrictedRegionVersion_valid){
          LOC_LOGL0("is valid restrictedRegionVersion_valid ");
          clientinfo->restrictedRegionVersion_valid = 1;
          clientinfo->restrictedRegionVersion =fdclServiceEvent->restrictedRegionVersion;
      }	
      if(fdclServiceEvent->bsBestPosition_valid){
          LOC_LOGL0("is valid bsBestPosition_valid ");
          clientinfo->bsBestPosition_valid = 1;
          clientinfo->bestPosition.latitude = fdclServiceEvent->bsBestPosition.latitude ;
          clientinfo->bestPosition.longitude = fdclServiceEvent->bsBestPosition.longitude ;
          clientinfo->bestPosition.horizontalUnc = fdclServiceEvent->bsBestPosition.horizontalUnc;
          clientinfo->bestPosition.altitude = fdclServiceEvent->bsBestPosition.altitude;
          clientinfo->bestPosition.altitudeUnc = fdclServiceEvent->bsBestPosition.altitudeUnc;
          clientinfo->bestPosition.latestPositionSources.unspecifiedInternal= fdclServiceEvent->bsBestPosition.latestPositionSources.unspecifiedInternal;
          clientinfo->bestPosition.latestPositionSources.unspecifiedExternal = fdclServiceEvent->bsBestPosition.latestPositionSources.unspecifiedExternal;
          clientinfo->bestPosition.latestPositionSources.gnss = fdclServiceEvent->bsBestPosition.latestPositionSources.gnss;
          clientinfo->bestPosition.latestPositionSources.sensors = fdclServiceEvent->bsBestPosition.latestPositionSources.sensors;
          clientinfo->bestPosition.latestPositionSources.wifi = fdclServiceEvent->bsBestPosition.latestPositionSources.wifi;
          clientinfo->bestPosition.latestPositionSources.wwan = fdclServiceEvent->bsBestPosition.latestPositionSources.wwan;	 	
      }	 
#ifdef FEATURE_LOCATION_MCS
      if(fdclServiceEvent->rfBandsLte_valid)
      {
        clientinfo->bandMask |= RF_BANDS_LTE;
        clientinfo->rfBandLTESubBandMask = fdclServiceEvent->rfBandsLte.bandMask;
        memscpy (&(clientinfo->rfBandsLTEMask), 
                QMI_LOC_RF_BAND_LENGTH_V02,
                &(fdclServiceEvent->rfBandsLte.rfBandsLTE), 
                QMI_LOC_RF_BAND_LENGTH_V02);
      }
      if(fdclServiceEvent->rfBandsGSM_valid)
      {
        clientinfo->bandMask |= RF_BANDS_GSM;
        clientinfo->rfBandsGSM = fdclServiceEvent->rfBandsGSM;
      }
      if(fdclServiceEvent->capabilitiesEnabled_valid)
      {
        if(fdclServiceEvent->capabilitiesEnabled & QMI_LOC_CAP_MASK_GTP_V02)
          clientinfo->capabilityMask |= GTP20_SUPPORTED;
        if(fdclServiceEvent->capabilitiesEnabled & QMI_LOC_CAP_MASK_GNSS_V02)
          clientinfo->capabilityMask |= GNSS_SUPPORTED;
        if(fdclServiceEvent->capabilitiesEnabled & QMI_LOC_CAP_MASK_GTP_CACHE_V02)
          clientinfo->capabilityMask |= GTP_20_CACHE_FIX;
      }
      if (fdclServiceEvent->scanAssistId_valid)
      {
        clientinfo->scanAssistId_valid = 1;
        clientinfo->scanAssistId = fdclServiceEvent->scanAssistId;
      }
      if (fdclServiceEvent->configId_valid)
      {
        clientinfo->configId_valid = 1;
        clientinfo->configId = fdclServiceEvent->configId;
      }
#endif
  }

  sendMsg(new MsgReportGtpWwanDataService(*this, clientinfo));

}

/** called from the qmi thread.*/
void IzatApiV02::handleDbtPositionEvent(
    const qmiLocEventDbtPositionReportIndMsgT_v02* pDbtPosReport)
{
    if (NULL == pDbtPosReport)
        return;

    Location location = {0};
    location.size = sizeof(Location);
    const qmiLocDbtPositionStructT_v02 *pReport = &pDbtPosReport->dbtPosition;

    GpsLocationExtended locationExtended = {0};

    // time stamp
    location.timestamp = pReport->timestampUtc;
    // latitude & longitude
    location.flags |= LOCATION_HAS_LAT_LONG_BIT;
    location.latitude  = pReport->latitude;
    location.longitude = pReport->longitude;
    // Altitude
    if (pReport->altitudeWrtEllipsoid_valid == 1) {
        location.flags |= LOCATION_HAS_ALTITUDE_BIT;
        location.altitude = pReport->altitudeWrtEllipsoid;
    }
    // Speed
    if (pReport->speedHorizontal_valid == 1) {
        location.flags |= LOCATION_HAS_SPEED_BIT;
        location.speed = pReport->speedHorizontal;
    }
    // Heading
    if (pReport->heading_valid == 1) {
        location.flags |= LOCATION_HAS_BEARING_BIT;
        location.bearing = pReport->heading;
    }
    // Accuracy
    location.flags |= LOCATION_HAS_ACCURACY_BIT;
    location.accuracy = sqrt(pReport->horUncEllipseSemiMinor*
                             pReport->horUncEllipseSemiMinor +
                             pReport->horUncEllipseSemiMajor*
                             pReport->horUncEllipseSemiMajor);

    sendMsg(new MsgReportLocation(
            *this, location, locationExtended, LOC_SESS_SUCCESS));
}

/** called from the qmi thread.*/
void
IzatApiV02::handleGeofenceBreachEvent(
    const qmiLocEventGeofenceBreachIndMsgT_v02* breachInfo)
{
    if (NULL == breachInfo)
        return;

    int64_t timestamp = LocOSIface_getSecondsSinceEpoch(); // get the current time
    Location location = {0};
    location.size = sizeof(Location);
    uint32_t id = breachInfo->geofenceId;

    if (breachInfo->geofencePosition_valid) {
        // Latitude & Longitude
        location.flags  |= LOCATION_HAS_LAT_LONG_BIT;
        if (breachInfo->geofencePosition.latitude >= -90 &&
            breachInfo->geofencePosition.latitude <= 90 &&
            breachInfo->geofencePosition.longitude >= -180 &&
            breachInfo->geofencePosition.longitude <= 180) {
            // latitude and longitude look to be in the expected format
            location.latitude  = breachInfo->geofencePosition.latitude;
            location.longitude = breachInfo->geofencePosition.longitude;
        }
        else {
            // latitdue and longitude must be in wrong format, so convert
            location.latitude  = breachInfo->geofencePosition.latitude *
                                 LAT_LONG_TO_RADIANS;
            location.longitude = breachInfo->geofencePosition.longitude *
                                 LAT_LONG_TO_RADIANS;
        }

        // Time stamp (UTC)
        location.timestamp = breachInfo->geofencePosition.timestampUtc;

        // Altitude
        location.flags  |= LOCATION_HAS_ALTITUDE_BIT;
        location.altitude = breachInfo->geofencePosition.altitudeWrtEllipsoid;

        // Speed
        if (breachInfo->geofencePosition.speedHorizontal_valid == 1) {
            location.flags |= LOCATION_HAS_SPEED_BIT;
            location.speed = breachInfo->geofencePosition.speedHorizontal;
        }

        // Heading
        if (breachInfo->geofencePosition.heading_valid == 1) {
            location.flags |= LOCATION_HAS_BEARING_BIT;
            location.bearing = breachInfo->geofencePosition.heading;
        }

        // Uncertainty (circular)
        location.flags  |= LOCATION_HAS_ACCURACY_BIT;
        location.accuracy = sqrt(
            (breachInfo->geofencePosition.horUncEllipseSemiMinor *
            breachInfo->geofencePosition.horUncEllipseSemiMinor) +
            (breachInfo->geofencePosition.horUncEllipseSemiMajor *
            breachInfo->geofencePosition.horUncEllipseSemiMajor));

        LOC_LOGL4("Location lat=%d.%d long=%d.%d ",
                      (int)location.latitude, ((int)(location.latitude*100))%100,
                      (int)location.longitude, ((int)(location.longitude*100))%100);
    } else {
       LOC_LOGH0("NO Location ");
    }

    GeofenceBreachType breachType;
    switch (breachInfo->breachType) {
    case eQMI_LOC_GEOFENCE_BREACH_TYPE_ENTERING_V02:
        breachType = GEOFENCE_BREACH_ENTER;
        break;
    case eQMI_LOC_GEOFENCE_BREACH_TYPE_LEAVING_V02:
        breachType = GEOFENCE_BREACH_EXIT;
        break;
    default:
        breachType = GEOFENCE_BREACH_UNKNOWN;
        break;
    }

    sendMsg(new MsgGeofenceBreach(*this,
                                  id,
                                  location,
                                  breachType,
                                  timestamp));
}

/** called from the qmi thread.*/
void IzatApiV02::handleGeofenceGenAlertEvent(
    const qmiLocEventGeofenceGenAlertIndMsgT_v02* geofenceGenAlertEvent)
{
    if (NULL == geofenceGenAlertEvent)
    {
        LOC_LOGE0("NULL indication info");
        return;
    }

    GeofenceAlertType alert = GEOFENCE_ALERT_INVALID;
    switch (geofenceGenAlertEvent->geofenceAlert)
    {
    case eQMI_LOC_GEOFENCE_GEN_ALERT_GNSS_UNAVAILABLE_V02:
        alert = GEOFENCE_ALERT_GNSS_UNAVAILABLE;
        break;
    case eQMI_LOC_GEOFENCE_GEN_ALERT_GNSS_AVAILABLE_V02:
        alert = GEOFENCE_ALERT_GNSS_AVAILABLE;
        break;
    case eQMI_LOC_GEOFENCE_GEN_ALERT_OOS_V02:
        alert = GEOFENCE_ALERT_OOS;
        break;
    case eQMI_LOC_GEOFENCE_GEN_ALERT_TIME_INVALID_V02:
        alert = GEOFENCE_ALERT_TIME_INVALID;
        break;
    case eQMI_LOC_GEOFENCE_GEN_ALERT_MGP_UNAVAILABLE_V02:
        alert = GEOFENCE_ALERT_MGP_UNAVAILABLE;
        break;
    default:
        alert = GEOFENCE_ALERT_INVALID;
        break;
    }

    sendMsg(new MsgGeofenceAlert(*this, alert));
}

void
IzatApiV02::resetStateAfterSSR()
{
    LOC_LOGL0("resetStateAfterSSR");

  /* When SSR happens, modem will release the
     batching memory allocated, so need to query
     the memory again.*/
    mBatchSize = 0;
    mTimeBasedTrackingInProgress = false;
    mDistanceBasedTrackingInProgress = false;
    mBatchingInProgress = false;
}

void
IzatApiV02::setBatchSize(size_t size)
{
    LOC_LOGL1("mDesiredBatchSize %u", size);
    mDesiredBatchSize = size;
}

LocationError
IzatApiV02::queryBatchBufferOnModem(size_t size)
{
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    LOC_LOGL1("The batch size desired is %u", size);
    qmiLocGetBatchSizeReqMsgT_v02 batchSizeReq;
    memset(&batchSizeReq, 0, sizeof(batchSizeReq));
    batchSizeReq.transactionId = 1;
    batchSizeReq.batchSize = size;
    LOC_SEND_SYNC_REQ(GetBatchSize,
                      GET_BATCH_SIZE,
                      batchSizeReq,
                      clientHandle);
    if (rv) {
        mBatchSize = ind.batchSize;
        LOC_LOGL1("get batching size succeeded. The modem batch size is %u", mBatchSize);
        if (mBatchSize != 0) {
            err = LOCATION_ERROR_SUCCESS;
        }
    } else {
        mBatchSize = 0;
        LOC_LOGH0("get batching size failed.");
    }

    return err;
}

LocationError
IzatApiV02::addGeofence(uint32_t& hwId,
                        uint32_t clientId,
                        const GeofenceOption& options,
                        const GeofenceInfo& info,
                        uint8_t confidence)
{
    LOC_LOGH6("addGeofence(): lat: %d.%d lon: %d.%d radius: %d.%d",
                  (int)info.latitude, ((int)(info.latitude*10000))%10000,
                  (int)info.longitude, ((int)(info.longitude*10000))%10000,
                  (int)info.radius, ((int)(info.radius*100))%100);
    LOC_LOGH3("breach=%u respon=%u dwell=%u",
            options.breachTypeMask, options.responsiveness, options.dwellTime);
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    qmiLocAddCircularGeofenceReqMsgT_v02 addReq;
    memset(&addReq, 0, sizeof(addReq));

    if (options.breachTypeMask & GEOFENCE_BREACH_ENTER_BIT)
        addReq.breachMask |= QMI_LOC_GEOFENCE_BREACH_ENTERING_MASK_V02;
    if (options.breachTypeMask & GEOFENCE_BREACH_EXIT_BIT)
        addReq.breachMask |= QMI_LOC_GEOFENCE_BREACH_LEAVING_MASK_V02;

    // confidence
    addReq.confidence_valid = true;
    switch (confidence) {
    case GF_CONFIDENCE_LOW:
        addReq.confidence = eQMI_LOC_GEOFENCE_CONFIDENCE_LOW_V02;
        break;
    case GF_CONFIDENCE_MED:
        addReq.confidence = eQMI_LOC_GEOFENCE_CONFIDENCE_MED_V02;
        break;
    case GF_CONFIDENCE_HIGH:
        addReq.confidence = eQMI_LOC_GEOFENCE_CONFIDENCE_HIGH_V02;
        break;
    default:
        addReq.confidence = eQMI_LOC_GEOFENCE_CONFIDENCE_LOW_V02;
        break;
    }

    // custom responsiveness
    addReq.responsiveness_valid = 1;
    addReq.responsiveness = eQMI_LOC_GEOFENCE_RESPONSIVENESS_CUSTOM_V02;
    addReq.customResponsivenessValue_valid = true;

    #define MILLIS_PER_SEC 1000
    // The min custom responsiveness we support is 1 second
    if(options.responsiveness < MILLIS_PER_SEC){
        addReq.customResponsivenessValue = 1;
    }
    else{
        // The max custom responsiveness we support is 65535 (in seconds)
        addReq.customResponsivenessValue = options.responsiveness / MILLIS_PER_SEC & 0xFFFF;
    }

    // dwell time
    if (options.dwellTime > 0) {
        addReq.dwellTime_valid = 1;
        addReq.dwellTime = options.dwellTime;
        addReq.dwellTypeMask_valid = 1;
        if (options.breachTypeMask & GEOFENCE_BREACH_DWELL_IN_BIT) {
            addReq.dwellTypeMask |= QMI_LOC_GEOFENCE_DWELL_TYPE_INSIDE_MASK_V02;
        }
        if (options.breachTypeMask & GEOFENCE_BREACH_DWELL_OUT_BIT) {
            addReq.dwellTypeMask |= QMI_LOC_GEOFENCE_DWELL_TYPE_OUTSIDE_MASK_V02;
        }
    }
    addReq.circularGeofenceArgs.latitude = info.latitude;
    addReq.circularGeofenceArgs.longitude = info.longitude;
    addReq.circularGeofenceArgs.radius = info.radius;
    addReq.includePosition = true;
    addReq.transactionId = clientId;

    LOC_SEND_SYNC_REQ(AddCircularGeofence,
        ADD_CIRCULAR_GEOFENCE,
        addReq, clientHandle);

    LOC_LOGL2("rv is %d, ind.geofenceId_valid is %d",
          rv, ind.geofenceId_valid);
    if (rv && ind.geofenceId_valid != 0) {
        LOC_LOGL0("add geofence succeed");
        hwId = ind.geofenceId;
        err = LOCATION_ERROR_SUCCESS;
    }

    EXIT_LOG(%u, err);
    return err;
}

LocationError
IzatApiV02::removeGeofence(uint32_t hwId, uint32_t clientId)
{
    LOC_LOGH2("removeGeofence() clientId %p hwid %u", clientId, hwId);
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    qmiLocDeleteGeofenceReqMsgT_v02 deleteReq;
    memset(&deleteReq, 0, sizeof(deleteReq));

    deleteReq.geofenceId = hwId;
    deleteReq.transactionId = clientId;

    LOC_SEND_SYNC_REQ(DeleteGeofence,
        DELETE_GEOFENCE,
        deleteReq, clientHandle);
    LOC_LOGL1("removeGeofence- rv is %d", rv);
    if (rv) {
        err = LOCATION_ERROR_SUCCESS;
    }

    EXIT_LOG(%u, err);
    return err;
}

LocationError
IzatApiV02::getSingleShot(const SingleShotOptions& options) {

    return startTimeBasedTracking(
            0, options.accuracyLevel, 0, true, options.timeout);
}

LocationError
IzatApiV02::startTimeBasedTracking(
        const LocationOptions& options, LocationAccuracyLevel accuracy) {

    if (options.accuracyLevel != LOCATION_ACCURACY_UNKNOWN)
    {
        accuracy = options.accuracyLevel;
    }
    return startTimeBasedTracking(
            0, accuracy, options.minInterval, false, 0);
}

LocationError
IzatApiV02::startTimeBasedTracking(
        uint8_t suplMode, LocationAccuracyLevel accuracy, uint32_t minInterval,
        bool isSingleShot, uint32_t singleShotTimeout)
{
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    LOC_LOGM3("Starting time based tracking id %u minInterval %u accuracy %u",
          DUMMY_TIME_BASED_TRACKING_SESSION_ID, minInterval, accuracy);

    mTimeBasedTrackingInProgress = true;
    /** register now so that QMI_LOC_EVENT_MASK_POSITION_REPORT_V02 gets registered */
    if (!registerEventMask(mQmiMask)) {
        LOC_LOGH0("update flp session on fix mask failed");
        mTimeBasedTrackingInProgress = false;
        return err;
    }

    /** start time based tracking session as flp session on fix*/
    locClientStatusEnumType status;
    locClientReqUnionType req_union;
    qmiLocStartReqMsgT_v02 start_msg;
    memset(&start_msg, 0, sizeof(start_msg));

    /* Only one session supported, pass in dummy session id */
    start_msg.sessionId = DUMMY_TIME_BASED_TRACKING_SESSION_ID;

    // interval
    if (!isSingleShot) {
        /*set interval for intermediate fixes*/
        start_msg.minIntermediatePositionReportInterval_valid = 1;
        start_msg.minIntermediatePositionReportInterval = minInterval;
        /*set interval for final fixes*/
        start_msg.minInterval_valid = 1;
        start_msg.minInterval = minInterval;
    }

    // accuracy
    start_msg.horizontalAccuracyLevel_valid = 1;
    switch(accuracy) {
    case LOCATION_ACCURACY_LOW:
        start_msg.horizontalAccuracyLevel = eQMI_LOC_ACCURACY_LOW_V02;
        break;
    case LOCATION_ACCURACY_MED:
        start_msg.horizontalAccuracyLevel = eQMI_LOC_ACCURACY_MED_V02;
        break;
    case LOCATION_ACCURACY_HIGH:
        start_msg.horizontalAccuracyLevel = eQMI_LOC_ACCURACY_HIGH_V02;
        break;
    default:
        start_msg.horizontalAccuracyLevel = eQMI_LOC_ACCURACY_HIGH_V02;
    }
    LOC_LOGL1("Tracking accuracy level is %d", start_msg.horizontalAccuracyLevel);

    // recurrence
    start_msg.fixRecurrence_valid = 1;
    if (isSingleShot) {
        start_msg.fixRecurrence = eQMI_LOC_RECURRENCE_SINGLE_V02;
        start_msg.positionReportTimeout_valid = 1;
        start_msg.positionReportTimeout = singleShotTimeout;
    } else {
        start_msg.fixRecurrence = eQMI_LOC_RECURRENCE_PERIODIC_V02;
    }

    // altitude assumed
    start_msg.configAltitudeAssumed_valid = 1;
    start_msg.configAltitudeAssumed =
        eQMI_LOC_ALTITUDE_ASSUMED_IN_GNSS_SV_INFO_DISABLED_V02;

    // intermediate fix
    start_msg.intermediateReportState_valid = 1;
    start_msg.intermediateReportState = eQMI_LOC_INTERMEDIATE_REPORTS_ON_V02;

    req_union.pStartReq = &start_msg;
    status = locClientSendReq(clientHandle,
                              QMI_LOC_START_REQ_V02,
                              req_union);

    if (status == eLOC_CLIENT_SUCCESS) {
        LOC_LOGM0("start time-based tracking succeeded.");
        sendMsg(new MsgReportRequestStartTracking(
                *this, minInterval, isSingleShot, accuracy));
        err = LOCATION_ERROR_SUCCESS;
    } else {
        mTimeBasedTrackingInProgress = false;
        LOC_LOGH0("start time-based tracking failed.");
    }
    return err;
}

LocationError 
IzatApiV02::startDistanceBasedTracking(uint32_t sessionId,
                                       const LocationOptions& options)
{
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    // start distance-based tracking.
    LOC_LOGM3("Starting distance based tracking id %u minInterval %u minDistance %u",
          sessionId, options.minInterval, options.minDistance);

    /** start distance based tracking session*/
    locClientStatusEnumType status;
    locClientReqUnionType req_union;
    qmiLocStartDbtReqMsgT_v02 start_dbt_req;
    qmiLocStartDbtIndMsgT_v02 start_dbt_ind;
    memset(&start_dbt_req, 0, sizeof(start_dbt_req));
    memset(&start_dbt_ind, 0, sizeof(start_dbt_ind));

    // request id
    start_dbt_req.reqId = sessionId;

    // distance
    start_dbt_req.minDistance = options.minDistance;

    // time
    if (options.minInterval > 0) {
        start_dbt_req.maxLatency_valid = 1;
        start_dbt_req.maxLatency = options.minInterval/1000; //in seconds
        if (0 == start_dbt_req.maxLatency)
        {
            start_dbt_req.maxLatency = 1; //in seconds
        }
    } else {
        LOC_LOGL0("no time set in this DBT request.");
    }
    // type
    start_dbt_req.distanceType = eQMI_LOC_DBT_DISTANCE_TYPE_STRAIGHT_LINE_V02;

    /* original location disabled by default, as the original location is
       the one cached in the modem buffer and its timestamps is not fresh.*/
    start_dbt_req.needOriginLocation = 0;

    start_dbt_req.usageType_valid = 1;
    start_dbt_req.usageType = eQMI_LOC_DBT_USAGE_NAVIGATION_V02;
    req_union.pStartDbtReq = &start_dbt_req;

    status = loc_sync_send_req(clientHandle,
                               QMI_LOC_START_DBT_REQ_V02,
                               req_union,
                               LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                               QMI_LOC_START_DBT_IND_V02,
                               &start_dbt_ind);

    if (status != eLOC_CLIENT_SUCCESS || eQMI_LOC_SUCCESS_V02 != start_dbt_ind.status ||
        start_dbt_ind.reqId != start_dbt_req.reqId) {
        LOC_LOGH4("start distance-based flp session on fix failed status = %d, "
              "ind.status = %d, start_dbt_ind.reqId is %d, start_dbt_req.reqId is %d",
              status, start_dbt_ind.status, start_dbt_ind.reqId, start_dbt_req.reqId);
    } else {
        LOC_LOGM0("start distance-based flp session on fix succeeded.");
        mDistanceBasedTrackingInProgress = true;
        err = LOCATION_ERROR_SUCCESS;
    }
    return err;
}

LocationError
IzatApiV02::startBatching(uint32_t sessionId,
                          const LocationOptions& options,
                          LocationAccuracyLevel accuracy,
                          uint32_t timeout,
                          uint8_t batchAllPos)
{
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    LOC_LOGM3("id %u minInternal %u minDistance %u",
          sessionId, options.minInterval, options.minDistance);

    //get batch size if needs
    if (mBatchSize == 0) {
        if (LOCATION_ERROR_SUCCESS != queryBatchBufferOnModem(mDesiredBatchSize))
            return err;
    }

    /** start flp session*/
    qmiLocStartBatchingReqMsgT_v02 startBatchReq;
    memset(&startBatchReq, 0, sizeof(startBatchReq));

    // interval
    if (options.minInterval != 0xFFFFFFFF) {
        startBatchReq.minInterval_valid = 1;
        startBatchReq.minInterval = options.minInterval;
        LOC_LOGM1("Batching interval is %d.", startBatchReq.minInterval);
    }

    // distance
    if (options.minDistance != 0xFFFFFFFF) {
        startBatchReq.minDistance_valid = 1;
        startBatchReq.minDistance = options.minDistance;
        LOC_LOGM1("Batching distance is %d.", startBatchReq.minDistance);
    }

    // accuracy
    startBatchReq.horizontalAccuracyLevel_valid = 1;
    switch(accuracy) {
    case LOCATION_ACCURACY_LOW:
        startBatchReq.horizontalAccuracyLevel = eQMI_LOC_ACCURACY_LOW_V02;
        break;
    case LOCATION_ACCURACY_MED:
        startBatchReq.horizontalAccuracyLevel = eQMI_LOC_ACCURACY_MED_V02;
        break;
    case LOCATION_ACCURACY_HIGH:
        startBatchReq.horizontalAccuracyLevel = eQMI_LOC_ACCURACY_HIGH_V02;
        break;
    default:
        startBatchReq.horizontalAccuracyLevel = eQMI_LOC_ACCURACY_LOW_V02;
    }
    LOC_LOGL1("Batching accuracy level is %d.",
          startBatchReq.horizontalAccuracyLevel);

    // time out
    if (timeout > 0) {
        startBatchReq.fixSessionTimeout_valid = 1;
        startBatchReq.fixSessionTimeout = timeout;
        LOC_LOGL1("Batching time out is set to %d.",
              startBatchReq.fixSessionTimeout);
    } else {
        // modem will use the default time out (20 seconds)
        startBatchReq.fixSessionTimeout_valid = 0;
    }

    // batch request id
    startBatchReq.requestId_valid = 1;
    startBatchReq.requestId = sessionId;

    // batch all fixes always
    startBatchReq.batchAllPos_valid = 1;
    startBatchReq.batchAllPos = batchAllPos;

    LOC_SEND_SYNC_REQ(StartBatching,
                      START_BATCHING,
                      startBatchReq,
                      clientHandle);
    if (rv) {
        LOC_LOGM0("start batching succeeded.");
        mBatchingInProgress = true;
        err = LOCATION_ERROR_SUCCESS;
    } else {
        LOC_LOGH0("start batching failed.");
    }
    return err;
}

LocationError IzatApiV02::releaseBatchBuffer()
{
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    // Close the batching interface if required
    if (mBatchSize != 0) {
        qmiLocReleaseBatchReqMsgT_v02 releaseLocatonReq;
        memset(&releaseLocatonReq, 0, sizeof(releaseLocatonReq));
        releaseLocatonReq.transactionId = 1;
        LOC_SEND_SYNC_REQ(ReleaseBatch,
                          RELEASE_BATCH,
                          releaseLocatonReq,
                          clientHandle);
        if (rv) {
            LOC_LOGL0("Release Batch successful.");
            mBatchSize = 0;
            err = LOCATION_ERROR_SUCCESS;
        } else {
            LOC_LOGH0("Release Batch failed !");
        }
    } else {
        LOC_LOGL0("The batching API in modem has not been queried");
    }

    return err;
}

LocationError
IzatApiV02::stopTimeBasedTracking()
{
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    locClientStatusEnumType status;
    locClientReqUnionType req_union;
    qmiLocStopReqMsgT_v02 stop_msg;
    memset(&stop_msg, 0, sizeof(stop_msg));
    stop_msg.sessionId = DUMMY_TIME_BASED_TRACKING_SESSION_ID;
    req_union.pStopReq = &stop_msg;

    status = locClientSendReq(clientHandle,
                              QMI_LOC_STOP_REQ_V02,
                              req_union);
    if( eLOC_CLIENT_SUCCESS != status) {
        LOC_LOGH0("stop time-based tracking failed.");
    } else {
        mTimeBasedTrackingInProgress = false;
        /** register so that QMI_LOC_EVENT_MASK_POSITION_REPORT_V02 is not registered */
        if (!registerEventMask(mQmiMask)) {
            LOC_LOGH0("update flp session on fix mask failed");
            return err;
        }        
        err = LOCATION_ERROR_SUCCESS;
        LOC_LOGL0("stop time-based tracking succeeded.");

        sendMsg(new MsgReportRequestStopTracking(*this));
    }
    return err;
}

LocationError
IzatApiV02::stopDistanceBasedTracking(uint32_t sessionId)
{
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    locClientStatusEnumType status;
    locClientReqUnionType req_union;
    qmiLocStopDbtReqMsgT_v02 stop_dbt_req;
    qmiLocStopDbtIndMsgT_v02 stop_dbt_Ind;
    memset(&stop_dbt_req, 0, sizeof(stop_dbt_req));
    memset(&stop_dbt_Ind, 0, sizeof(stop_dbt_Ind));

    stop_dbt_req.reqId = sessionId;

    req_union.pStopDbtReq = &stop_dbt_req;

    status = loc_sync_send_req(clientHandle,
                               QMI_LOC_STOP_DBT_REQ_V02,
                               req_union,
                               LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                               QMI_LOC_STOP_DBT_IND_V02,
                               &stop_dbt_Ind);

    if (status != eLOC_CLIENT_SUCCESS || eQMI_LOC_SUCCESS_V02 != stop_dbt_Ind.status) {
        LOC_LOGL2("Stop distance-based tracking failed status = %d ind.status = %d",
              status, stop_dbt_Ind.status);
    } else {
        mDistanceBasedTrackingInProgress = false;
        err = LOCATION_ERROR_SUCCESS;
        LOC_LOGL0("stop distance-based tracking succeeded.");
    }
    return err;
}

LocationError
IzatApiV02::stopBatching(uint32_t sessionId)
{
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    // locClientStatusEnumType status;
    qmiLocStopBatchingReqMsgT_v02 stopBatchingReq;
    memset(&stopBatchingReq, 0, sizeof(stopBatchingReq));

    stopBatchingReq.requestId_valid = 1;
    stopBatchingReq.requestId = sessionId;

    LOC_SEND_SYNC_REQ(StopBatching,
                      STOP_BATCHING,
                      stopBatchingReq,
                      clientHandle);
    if (rv) {               
        mBatchingInProgress = false;
        err = LOCATION_ERROR_SUCCESS;
        LOC_LOGL0("stop batching on full succeeded.");

        sendMsg(new MsgReportRequestStopBatching(*this));

    } else {
        LOC_LOGH0("stop batching on full failed.");
    }
    return err;
}


LocationError
IzatApiV02::getBatchedLocations(size_t count)
{
    LocationError err = LOCATION_ERROR_SUCCESS;

    LOC_LOGL1("count is %u.", count);

    size_t entriesToReadInTotal = std::min(mBatchSize,count);
    if (entriesToReadInTotal == 0) {
        LOC_LOGL0("No batching memory allocated in modem or nothing to read");
        // calling the base class
        reportLocations(NULL, 0);
    } else {
        size_t entriesToRead = std::min(entriesToReadInTotal, (size_t)QMI_LOC_READ_FROM_BATCH_MAX_SIZE_V02);
        size_t entriesGotInTotal = 0;
        size_t entriesGotInEachTime = 0;
        //allocate +1 just in case we get one more location added during read
        Location* pLocationsFromModem = new Location[entriesToReadInTotal+1]();
        Location* tempLocationP = new Location[QMI_LOC_READ_FROM_BATCH_MAX_SIZE_V02]();
        do {
            memset(tempLocationP, 0, QMI_LOC_READ_FROM_BATCH_MAX_SIZE_V02*sizeof(Location));
            readModemLocations(tempLocationP,
                               entriesToRead,
                               entriesGotInEachTime);
            for (int i = 0; i<entriesGotInEachTime; i++) {
                int index = entriesToReadInTotal-entriesGotInTotal-i;
                LOC_LOGL2("i:%d and index:%d", i, index);
                
                // make sure index is not too large or small to fit in the array
                if (index <= entriesToReadInTotal && index >= 0) {
                    pLocationsFromModem[index] = tempLocationP[i];
                } else {
                    LOC_LOGH0("dropped an unexpected location.");
                }
            }
            entriesGotInTotal += entriesGotInEachTime;
            entriesToRead = std::min(entriesToReadInTotal - entriesGotInTotal,
                                     (size_t)QMI_LOC_READ_FROM_BATCH_MAX_SIZE_V02);
        } while (entriesGotInEachTime > 0 && entriesToRead > 0);
        delete[] tempLocationP;

        LOC_LOGL1("Read out %u batched locations from modem in total.",
              entriesGotInTotal);

        // offset is by default 1 because we allocated one extra location during read
        uint32_t offset = 1;
        if (entriesToReadInTotal > entriesGotInTotal) {
            offset = 1 + entriesToReadInTotal - entriesGotInTotal;
            LOC_LOGL1("offset %u", offset);
        } else if (entriesGotInTotal > entriesToReadInTotal) {
            // offset is zero because we need one extra slot for the extra location
            offset = 0;
            LOC_LOGM1("Read %u extra location(s) than expected.",
                  entriesGotInTotal - entriesToReadInTotal);
            // we got one extra location added during modem read, so one location will
            // be out of order and needs to be found and put in order
            uint64_t currentTimeStamp = pLocationsFromModem[entriesToReadInTotal].timestamp;
            for (int i=entriesToReadInTotal-1; i >= 0; i--) {
                // find the out of order location
                if (currentTimeStamp < pLocationsFromModem[i].timestamp) {
                    LOC_LOGM2("Out of order location is index %d timestamp %llu.",
                          i, tempLocationP[i].timestamp);
                    // save the out of order location to be put at end of array
                    Location tempLocation(pLocationsFromModem[i]);
                    // shift the remaining locations down one
                    for (int j=i; j<entriesToReadInTotal; j++) {
                        pLocationsFromModem[j] = pLocationsFromModem[j+1];
                    }
                    // put the out of order location at end of array now
                    pLocationsFromModem[entriesToReadInTotal] = tempLocation;
                    break;
                } else {
                    currentTimeStamp = pLocationsFromModem[i].timestamp;
                }
            }
        }

        LOC_LOGL2("Calling reportLocations with count:%u and entriesGotInTotal:%d",
              count, entriesGotInTotal);

        // calling the base class
        reportLocations(pLocationsFromModem+offset,
                        entriesGotInTotal);
        delete[] pLocationsFromModem;
    }

    return err;
}

void
IzatApiV02::readModemLocations(Location* pLocationPiece,
                               size_t count,
                               size_t& numbOfEntries)
{
    LOC_LOGL1("count is %u.", count);
    numbOfEntries = 0;
    qmiLocReadFromBatchReqMsgT_v02 getBatchLocatonReq;
    memset(&getBatchLocatonReq, 0, sizeof(getBatchLocatonReq));

    if (count != 0) {
        getBatchLocatonReq.numberOfEntries = count;
        getBatchLocatonReq.transactionId = 1;
    }

    LOC_SEND_SYNC_REQ(ReadFromBatch,
                      READ_FROM_BATCH,
                      getBatchLocatonReq,
                      clientHandle);

    if (!rv) {
        LOC_LOGH0("Reading batched locations from modem failed.");
        return;
    }
    if (ind.numberOfEntries_valid != 0 &&
        ind.batchedReportList_valid != 0) {
        for (int i =0; i<ind.numberOfEntries; i++) {
            Location temp = {0};
            temp.size = sizeof(Location);
            if ((ind.batchedReportList[i].validFields &
                 QMI_LOC_BATCHED_REPORT_MASK_VALID_LATITUDE_V02) &&
                (ind.batchedReportList[i].validFields &
                 QMI_LOC_BATCHED_REPORT_MASK_VALID_LONGITUDE_V02)) {
                temp.latitude = ind.batchedReportList[i].latitude;
                temp.longitude = ind.batchedReportList[i].longitude;
                temp.flags |= LOCATION_HAS_LAT_LONG_BIT;
            }
            if (ind.batchedReportList[i].validFields &
                QMI_LOC_BATCHED_REPORT_MASK_VALID_ALT_WRT_ELP_V02) {
                temp.altitude = ind.batchedReportList[i].altitudeWrtEllipsoid;
                temp.flags |= LOCATION_HAS_ALTITUDE_BIT;
            }
            if (ind.batchedReportList[i].validFields &
                QMI_LOC_BATCHED_REPORT_MASK_VALID_TIMESTAMP_UTC_V02) {
                temp.timestamp = ind.batchedReportList[i].timestampUtc;
            }
            if (ind.batchedReportList[i].validFields &
                QMI_LOC_BATCHED_REPORT_MASK_VALID_HOR_CIR_UNC_V02) {
                temp.accuracy = ind.batchedReportList[i].horUncCircular;
                temp.flags |= LOCATION_HAS_ACCURACY_BIT;
            }
            if (ind.batchedReportList[i].validFields &
                QMI_LOC_BATCHED_REPORT_MASK_VALID_HEADING_V02) {
                temp.bearing = ind.batchedReportList[i].heading;
                temp.flags |= LOCATION_HAS_BEARING_BIT;
            }
            if (ind.batchedReportList[i].validFields &
                QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_HOR_V02) {
                temp.speed = ind.batchedReportList[i].speedHorizontal;
                temp.flags |= LOCATION_HAS_SPEED_BIT;
            }
            pLocationPiece[i] = temp;
        }
        numbOfEntries = ind.numberOfEntries;
        LOC_LOGL1("Read out %u batched locations from modem.",
              numbOfEntries);
    } else {
        LOC_LOGL0("Modem does not return batched location.");
    }
}

// XTRA Client 2.0
int IzatApiV02::setXtraVersionCheck(enum XtraVersionCheck check)
{
    LOC_LOGL1("entering IzatApiV02::setXtraVersionCheck(check=%llu)",
          check);

    if (check != XTRA_VERSION_CHECK_DISABLED && check != XTRA_VERSION_CHECK_AUTO
            && check != XTRA_VERSION_CHECK_XTRA2 && check != XTRA_VERSION_CHECK_XTRA3 ) {
        return IZAT_INVALID_ARGUMENTS;
    }

    qmiLocSetXtraVersionCheckReqMsgT_v02 req;
    memset(&req, 0, sizeof(req));

    switch (check) {
        case XTRA_VERSION_CHECK_DISABLED:
            req.xtraVersionCheckMode = eQMI_LOC_XTRA_VERSION_CHECK_DISABLE_V02;
            break;

        case XTRA_VERSION_CHECK_AUTO:
            req.xtraVersionCheckMode = eQMI_LOC_XTRA_VERSION_CHECK_AUTO_V02;
            break;

        case XTRA_VERSION_CHECK_XTRA2:
            req.xtraVersionCheckMode = eQMI_LOC_XTRA_VERSION_CHECK_XTRA2_V02;
            break;

        case XTRA_VERSION_CHECK_XTRA3:
            req.xtraVersionCheckMode = eQMI_LOC_XTRA_VERSION_CHECK_XTRA3_V02;
            break;

        default:
            // cannot reach here
            break;
    };

    LOC_SEND_SYNC_REQ(
            SetXtraVersionCheck,
            SET_XTRA_VERSION_CHECK,
            req, clientHandle);

    LOC_LOGL2("st is %d ind.status is %d", st, ind.status);

    EXIT_LOG(%d, rv);
    return rv ? IZAT_SUCCESS : IZAT_FAIL;
}

int IzatApiV02::injectIzatPcid(uint64_t pcid)
{
    LOC_LOGL1("entering IzatApiV02::injectIzatPcid(pcid=%llu)", pcid);

    if (pcid == 0) {
        return IZAT_INVALID_ARGUMENTS;
    }

    qmiLocInjectXtraPcidReqMsgT_v02 req;
    memset(&req, 0, sizeof(req));

    req.xtraPcid = pcid;

    LOC_SEND_SYNC_REQ(
            InjectXtraPcid, INJECT_XTRA_PCID,
            req, clientHandle);

    LOC_LOGL2("st is %d ind.stasus is %d", st, ind.status);

    EXIT_LOG(%d, rv);
    return rv ? IZAT_SUCCESS : IZAT_FAIL;
}

int IzatApiV02::injectXtraData(const char* data, uint32_t length)
{
    if (data == NULL || length < 1) {
        LOC_LOGL2("invalid argument data = %p length = %u",
              data, length);
        return IZAT_INVALID_ARGUMENTS;
    }

    qmiLocInjectXtraDataReqMsgT_v02 req;
    req.formatType_valid = 1;
    req.formatType =  eQMI_LOC_XTRA_DATA_V02;
    req.totalSize = length;

    int totalParts = ((length - 1) / QMI_LOC_MAX_XTRA_PART_LEN_V02) + 1;
    req.totalParts = totalParts;
    int lenInjected = 0; // O bytes injected

    for (int part = 1; part <= totalParts; part++) {
        req.partNum = part;
        req.partData_len =
             MIN(QMI_LOC_MAX_XTRA_PART_LEN_V02, (int)length - lenInjected);

        if (req.partData_len > QMI_LOC_MAX_XTRA_PART_LEN_V02) {
            // this can never happen, since we limited with MIN above
            // but Klockwork complains, so...
            LOC_LOGE1("req.partData_len %d is too long!", req.partData_len);
            return IZAT_FAIL;
        }
        // copy data into the message
        memscpy(req.partData, req.partData_len,
                data + lenInjected, req.partData_len);

        LOC_LOGL4("part %d/%d, len = %d, total injected = %d",
              req.partNum, totalParts, req.partData_len, lenInjected);

        LOC_SEND_SYNC_REQ(
                InjectXtraData, INJECT_XTRA_DATA,
                req, clientHandle);

        if (st != eLOC_CLIENT_SUCCESS || rv == false ||
              ind.status != eQMI_LOC_SUCCESS_V02 ||
              req.partNum != ind.partNum)
        {
            LOC_LOGE4("failed status = %d, ind.status = %d, part num = %d, ind.partNum = %d",
                  st, ind.status, req.partNum, ind.partNum);

            EXIT_LOG(%d, rv);
            return IZAT_FAIL;
        }

        lenInjected += req.partData_len;
        LOC_LOGL1("XTRA accumulated injected length: %d", lenInjected);
    }

    return IZAT_SUCCESS;
}

int IzatApiV02::injectNtpTime(const int64_t time, const int64_t reference, const int64_t uncertainty)
{
    LOC_LOGEVENT("IzatApiV02::injectNtpTime");

    qmiLocInjectUtcTimeReqMsgT_v02 req;
    memset(&req, 0, sizeof(req));

    if(0 == time) {
        LOC_LOGH0("Injecting NTP failure indication into the modem");

        /*Zero time in GPS format*/
        req.timeUtc = UTC_GPS_TIME_OFFSET;
        req.timeUnc = 0;

    }
    else {
        req.timeUtc = time;

        // get elapsed time in ms
        timeval tv = timeval();
        LocOSIface_gettimeofday(&tv, NULL);
        //gettimeofday(&tv, NULL);
        uint64_t elapsedTimeMs = (uint64_t)tv.tv_sec*1000 + (uint64_t)tv.tv_usec/1000;

        // we might not have got valid timeofday, check
        if(elapsedTimeMs > reference){
            req.timeUtc += elapsedTimeMs - reference;
        }
        else{
            LOC_LOGE2("Invalid elapsedTimeMs 0x%x%x", elapsedTimeMs>>32, elapsedTimeMs);
        }

        req.timeUnc = uncertainty;

        LocOSIface_injectTime(req.timeUtc, LOC_OS_IFACE_TIME_SOURCE_NTP);
    }
    
    LOC_SEND_SYNC_REQ(
            InjectUtcTime, INJECT_UTC_TIME,
            req,clientHandle);
    
    LOC_LOGL2("status is %d ind.status is %d", st, ind.status);

    EXIT_LOG(%d, rv);
    return rv ? IZAT_SUCCESS : IZAT_FAIL;
}

int IzatApiV02::requestXtraServers()
{
    LOC_LOGEVENT("IzatApiV02::requestXtraServers()");

    locClientStatusEnumType st = eLOC_CLIENT_SUCCESS;
    locClientReqUnionType req_union;
    qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02 ind;

    memset(&ind, 0, sizeof(ind));

    st = loc_sync_send_req(clientHandle,
                           QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02,
                           req_union,
                           LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                           QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_IND_V02,
                           &ind);

    LOC_LOGL4("status is %d, ind.status is %d"
          " ind.serverList_valid is %d ind.serverList.serverList_len is %d",
          st, ind.status, ind.serverList_valid, ind.serverList.serverList_len);

    if (st == eLOC_CLIENT_SUCCESS &&
            ind.status == eQMI_LOC_SUCCESS_V02 &&
            ind.serverList_valid &&
            ind.serverList.serverList_len != 0)
    {
        const char* servers[] = {"","",""};
        for (int nserver = 0; nserver < MIN(ind.serverList.serverList_len, 3); nserver++) {
            servers[nserver] = ind.serverList.serverList[nserver].serverUrl;
        }

        handleReceiveXtraServers(servers[0], servers[1], servers[2]);
    }

    return (st == eLOC_CLIENT_SUCCESS && ind.status == eQMI_LOC_SUCCESS_V02) ?
               IZAT_SUCCESS : IZAT_FAIL;
}

int IzatApiV02::requestXtraConfigInfo(
        const uint32_t configMask, const uint32_t configReqSource)
{
    locClientStatusEnumType st = eLOC_CLIENT_SUCCESS;
    uint32_t returnStatus = eLOC_CLIENT_FAILURE_GENERAL;
    uint32_t report_mask = 0;
    locClientReqUnionType reqUnion;
    qmiLocQueryXtraInfoReqMsgT_v02 req;
    qmiLocQueryXtraInfoIndMsgT_v02 ind;
    const char* xtraServers[] = {"","",""};
    const char* ntpServers[] = {"","",""};
    uint16_t absAgeHrs = 0 ,prefValidAgeHrs = 0;
    uint64_t relAgeUtc = 0;
    uint8_t timeReqValid = 0;

    LOC_LOGL2("IzatApiV02::requestXtraConfigInfo() configMask:0x%x source:%d",
              configMask, configReqSource);

    memset(&ind, 0, sizeof(ind));
    memset(&req, 0, sizeof(req));

    if ( configMask & XTRA_MP_CONFIG_DISABLE_AUDO_DOWNLOD_TIMER )
    {
      req.xtraConfig |= QMI_LOC_XTRA_CONFIG_DISABLE_AUTO_DOWNLOAD_TIMER_V02;
    }
    reqUnion.pQueryXtraInfoReq  = &req;

    st = loc_sync_send_req(clientHandle,
                           QMI_LOC_QUERY_XTRA_INFO_REQ_V02,
                           reqUnion,
                           (LOC_ENGINE_SYNC_REQUEST_TIMEOUT),
                           QMI_LOC_QUERY_XTRA_INFO_IND_V02,
                           &ind);

    LOC_LOGL8("Reportmask:0x%x AbsAge:%d RelAge:0x%x%x "
              "TimeReq:%d PrefAge:%d XtraSrvLen:%d NtpSrvLen:%d",
              ind.xtraInfo.reportMask, ind.xtraInfo.absAgeHrs,
              (int)(ind.xtraInfo.relAgeInUTC >> 32),
              (int)ind.xtraInfo.relAgeInUTC,
              ind.xtraInfo.timeRequest,
              ind.xtraInfo.preferedValidAgeHrs,
              ind.xtraInfo.xtraServerInfo.serverList_len,
              ind.xtraInfo.ntpServerInfo.timeServerList_len);

    if (st == eLOC_CLIENT_SUCCESS && ind.status == eQMI_LOC_SUCCESS_V02 &&
        0 != ind.xtraInfo.reportMask )
    {
        returnStatus = IZAT_SUCCESS ;
        if ( ind.xtraInfo.reportMask & QMI_LOC_XTRA_INFO_MASK_ABS_AGE_V02 )
        {
            report_mask = report_mask | XTRA_INFO_MASK_ABS_AGE ;
            absAgeHrs = ind.xtraInfo.absAgeHrs ;
        }
        if ( ind.xtraInfo.reportMask & QMI_LOC_XTRA_INFO_MASK_REL_AGE_V02 )
        {
            report_mask = report_mask | XTRA_INFO_MASK_REL_AGE;
            relAgeUtc = ind.xtraInfo.relAgeInUTC;
        }
        if ( ind.xtraInfo.reportMask & QMI_LOC_XTRA_INFO_MASK_XTRA_SERVER_V02 )
        {
            report_mask = report_mask | XTRA_INFO_MASK_XTRA_SERVER;
            for (unsigned int nserver = 0;
                    nserver < MIN(ind.xtraInfo.xtraServerInfo.serverList_len, 3); nserver++) {
                xtraServers[nserver] = ind.xtraInfo.xtraServerInfo.serverList[nserver].serverUrl;
            }
        }
        if ( ind.xtraInfo.reportMask & QMI_LOC_XTRA_INFO_MASK_NTP_SERVER_V02 )
        {
            report_mask = report_mask | XTRA_INFO_MASK_NTP_SERVER ;
            for (unsigned int nserver = 0; nserver <
                    MIN(ind.xtraInfo.ntpServerInfo.timeServerList_len, 3); nserver++) {
                ntpServers[nserver] = ind.xtraInfo.ntpServerInfo.timeServerList[nserver].serverUrl;
            }
        }
        if ( ind.xtraInfo.reportMask & QMI_LOC_XTRA_INFO_MASK_TIME_REQUEST_V02 )
        {
            report_mask = report_mask | XTRA_INFO_MASK_TIME_REQUEST;
            timeReqValid = ind.xtraInfo.timeRequest ;
        }

        if ( ind.xtraInfo.reportMask & QMI_LOC_XTRA_INFO_MASK_PREF_VALID_AGE_V02 )
        {
            report_mask = report_mask | XTRA_INFO_MASK_PREF_VALID_AGE;
            prefValidAgeHrs = ind.xtraInfo.preferedValidAgeHrs;
        }
    }
    else {
        if (st == eLOC_CLIENT_SUCCESS && ind.status == eQMI_LOC_GNSS_DISABLED_V02 ){
            returnStatus = IZAT_GPS_LOCKED;
            LOC_LOGE0("Request failed: GPS LOCKED");
        }
        else {
            returnStatus = IZAT_FAIL;
            LOC_LOGE2("Request failed: st %d ind.status %d", st, ind.status);
        }
    }
    handleReceiveXtraConfigInfo(returnStatus, configReqSource, report_mask,
                                 absAgeHrs, relAgeUtc, timeReqValid,
                                 prefValidAgeHrs,
                                 ntpServers[0],
                                 ntpServers[1],
                                 ntpServers[2],
                                 xtraServers[0],
                                 xtraServers[1],
                                 xtraServers[2]);

    return (st == eLOC_CLIENT_SUCCESS && ind.status == eQMI_LOC_SUCCESS_V02) ?
                 IZAT_SUCCESS : IZAT_FAIL;
}

int IzatApiV02::enableGtpWwan()
{
    LOC_LOGEVENT("IzatApiV02::enableGtpWwan()");

    #define REQUEST_ARR_SIZE 5
    uint8_t requestArray[REQUEST_ARR_SIZE] = {'A', 'R', 'T', 'X', 1};
    locClientStatusEnumType st = eLOC_CLIENT_SUCCESS;
    locClientReqUnionType req_union;
    qmiLocSetXtraTSessionControlReqMsgT_v02 req;
    qmiLocSetXtraTSessionControlIndMsgT_v02 ind;


    for (int i = 0; i < REQUEST_ARR_SIZE; i++) {
        memset(&ind, 0, sizeof(ind));
        memset(&req, 0, sizeof(req));

        req_union.pLocSetXtraTSessionControlReq = &req;
        req.xtraTSessionControl = requestArray[i];

        st = loc_sync_send_req(clientHandle,
            QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02,
            req_union,
            LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
            QMI_LOC_SET_XTRA_T_SESSION_CONTROL_IND_V02,
            &ind);

        LOC_LOGL2("enableGtpWwan status is %d, ind.status is %d %X", 
                st, ind.status);

        if (st != eLOC_CLIENT_SUCCESS ||
            ind.status != eQMI_LOC_SUCCESS_V02) {
            return IZAT_FAIL;
        }
    }
    return IZAT_SUCCESS;
}

int IzatApiV02::disableGtpWwan()
{
    LOC_LOGEVENT("IzatApiV02::disableGtpWwan()");

    #define REQUEST_ARR_SIZE 5
    uint8_t requestArray[REQUEST_ARR_SIZE] = {'A', 'R', 'T', 'X', 0};
    locClientStatusEnumType st = eLOC_CLIENT_SUCCESS;
    locClientReqUnionType req_union;
    qmiLocSetXtraTSessionControlReqMsgT_v02 req;
    qmiLocSetXtraTSessionControlIndMsgT_v02 ind;


    for (int i = 0; i < REQUEST_ARR_SIZE; i++) {
        memset(&ind, 0, sizeof(ind));
        memset(&req, 0, sizeof(req));

        req_union.pLocSetXtraTSessionControlReq = &req;
        req.xtraTSessionControl = requestArray[i];

        st = loc_sync_send_req(clientHandle,
            QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02,
            req_union,
            LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
            QMI_LOC_SET_XTRA_T_SESSION_CONTROL_IND_V02,
            &ind);

        LOC_LOGL2("disableGtpWwan status is %d, ind.status is %d %X", 
                st, ind.status);

        if (st != eLOC_CLIENT_SUCCESS ||
            ind.status != eQMI_LOC_SUCCESS_V02) {
            return IZAT_FAIL;
        }
    }
    return IZAT_SUCCESS;
}

int IzatApiV02::getBsObsList(LocBsObsData* bsobsdata)
{
    qmiLocGetBsObsDataReqMsgT_v02 req;
    qmiLocGetBsObsDataIndMsgT_v02* ind = new qmiLocGetBsObsDataIndMsgT_v02;
    locClientReqUnionType req_union;
    locClientStatusEnumType st = eLOC_CLIENT_SUCCESS;
    memset(&req, 0, sizeof(req));
    req_union.pGetBsObsDataReq = &req;
    req.numCellsUpload_valid = 1;
    req.numCellsUpload = 1; 

    st = loc_sync_send_req(clientHandle,
            QMI_LOC_GET_BS_OBS_DATA_REQ_V02,
            req_union,
            LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
            QMI_LOC_GET_BS_OBS_DATA_IND_V02,
            ind);

    if (st != eLOC_CLIENT_SUCCESS ||
            ind->status != eQMI_LOC_SUCCESS_V02) {
        return IZAT_FAIL;
    }
    bsobsdata->bsData_len = ind->bsData_len;
    for (uint32_t i = 0; i< ind->bsData_len; i++) {
        bsobsdata->bsData[i].posDeltaTime=ind->bsData[i].posDeltaTime;
        bsobsdata->bsData[i].validMask=ind->bsData[i].validMask;
        bsobsdata->bsData[i].bsLocalTimestamp=ind->bsData[i].bsLocalTimestamp;
        bsobsdata->bsData[i].cellid.airInterfaceType = ind->bsData[i].cellId.airInterfaceType>3?ind->bsData[i].cellId.airInterfaceType+2:ind->bsData[i].cellId.airInterfaceType;
        bsobsdata->bsData[i].cellid.id1 =ind->bsData[i].cellId.id1;
        bsobsdata->bsData[i].cellid.id2 =ind->bsData[i].cellId.id2;
        bsobsdata->bsData[i].cellid.id3 =ind->bsData[i].cellId.id3;
        bsobsdata->bsData[i].cellid.id4 =ind->bsData[i].cellId.id4;
        if(ind->bsData[i].validMask & QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_LATITUDE_V02)
        {
            bsobsdata->bsData[i].bestPosition.latitude = ind->bsData[i].bsBestPosition.latitude;
        }
        if(ind->bsData[i].validMask & QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_LONGITUDE_V02)
        {
            bsobsdata->bsData[i].bestPosition.longitude = ind->bsData[i].bsBestPosition.longitude;
        }
        if(ind->bsData[i].validMask & QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_HOR_UNC_V02)
        {
          bsobsdata->bsData[i].bestPosition.horizontalUnc = ind->bsData[i].bsBestPosition.horizontalUnc;
        }
        if(ind->bsData[i].validMask & QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_ALTITUDE_V02)
        {
          bsobsdata->bsData[i].bestPosition.altitude = ind->bsData[i].bsBestPosition.altitude;
        }
        if(ind->bsData[i].validMask & QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_ALT_UNC_V02)
        {
          bsobsdata->bsData[i].bestPosition.altitudeUnc = ind->bsData[i].bsBestPosition.altitudeUnc;
        }
        bsobsdata->bsData[i].bestPosition.latestPositionSources.unspecifiedInternal=ind->bsData[i].bsBestPosition.latestPositionSources.unspecifiedInternal;
        bsobsdata->bsData[i].bestPosition.latestPositionSources.unspecifiedExternal=ind->bsData[i].bsBestPosition.latestPositionSources.unspecifiedExternal;
        bsobsdata->bsData[i].bestPosition.latestPositionSources.gnss=ind->bsData[i].bsBestPosition.latestPositionSources.gnss;
        bsobsdata->bsData[i].bestPosition.latestPositionSources.sensors=ind->bsData[i].bsBestPosition.latestPositionSources.sensors;
        bsobsdata->bsData[i].bestPosition.latestPositionSources.wifi=ind->bsData[i].bsBestPosition.latestPositionSources.wifi;
        bsobsdata->bsData[i].bestPosition.latestPositionSources.wwan=ind->bsData[i].bsBestPosition.latestPositionSources.wwan;	

        if(ind->bsData[i].validMask & QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_GPSWEEK_V02){
            bsobsdata->bsData[i].gpsWeek =ind->bsData[i].gpsWeek;
        }
        if(ind->bsData[i].validMask & QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_GPSMSEC_V02) {
            bsobsdata->bsData[i].gpsMilliseconds =ind->bsData[i].gpsMilliseconds;
        }  
    }
    delete(ind);
    return IZAT_SUCCESS;
}

#ifdef FEATURE_LOCATION_MCS
int IzatApiV02::getBsFdclListAsync()
{     
    qmiLocGetFdclBsListReqMsgT_v02 req;
    locClientReqUnionType req_union;
    locClientStatusEnumType st = eLOC_CLIENT_SUCCESS;
    LOC_LOGE0("getBsFdclList: get FDCL list aync");
    memset(&req, 0, sizeof(req));
    req_union.pLocGetFdclBsListReq = &req;
    req.maxNumBs_valid = 1;
    req.maxNumBs = 1;
    st = locClientSendReq(clientHandle,
                         QMI_LOC_GET_FDCL_BS_LIST_REQ_V02,
                         req_union);
    LOC_LOGE1("getBsFdclList: get FDCL list async return %d", st);
    return st;
}

int IzatApiV02::handleBsFdclListResponse(const qmiLocGetFdclBsListIndMsgT_v02 &ind)
{     
  LOC_LOGE0("handleBsFdclListResponse");
  sendMsg(new MsgReportFdcLList(*this, ind));
  return IZAT_SUCCESS;
}
#endif

int IzatApiV02::getBsFdclList(LocFdclBsList* fdclbslist)
{     
  qmiLocGetFdclBsListReqMsgT_v02 req;
  qmiLocGetFdclBsListIndMsgT_v02 ind;
  locClientReqUnionType req_union;
  locClientStatusEnumType st = eLOC_CLIENT_SUCCESS;
  LOC_LOGE0("getBsFdclList: get FDCL list");
    memset(&ind, 0, sizeof(ind));
    memset(&req, 0, sizeof(req));
    req_union.pLocGetFdclBsListReq = &req;
    req.maxNumBs_valid = 1;
    req.maxNumBs = 1;

    st = loc_sync_send_req(clientHandle,
            QMI_LOC_GET_FDCL_BS_LIST_REQ_V02 ,
            req_union,
            LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
            QMI_LOC_GET_FDCL_BS_LIST_IND_V02 ,
            &ind);
    LOC_LOGE1("getBsFdclList: get FDCL list return %d", st);
    if (st != eLOC_CLIENT_SUCCESS ||
            ind.status != eQMI_LOC_SUCCESS_V02) {
        return IZAT_FAIL;
    }

    if(ind.bsBestPosition_valid){
        fdclbslist->bsBestPosition_valid = ind.bsBestPosition_valid;
        fdclbslist->bsBestPosition.latestPositionSources.unspecifiedInternal =ind.bsBestPosition.latestPositionSources.unspecifiedInternal;
        fdclbslist->bsBestPosition.latestPositionSources.unspecifiedExternal =ind.bsBestPosition.latestPositionSources.unspecifiedExternal;
        fdclbslist->bsBestPosition.latestPositionSources.gnss =ind.bsBestPosition.latestPositionSources.gnss;
        fdclbslist->bsBestPosition.latestPositionSources.sensors =ind.bsBestPosition.latestPositionSources.sensors;
        fdclbslist->bsBestPosition.latestPositionSources.wifi =ind.bsBestPosition.latestPositionSources.wifi;
        fdclbslist->bsBestPosition.latestPositionSources.wwan =ind.bsBestPosition.latestPositionSources.wwan;
        fdclbslist->bsBestPosition.latitude =ind.bsBestPosition.latitude;
        fdclbslist->bsBestPosition.longitude =ind.bsBestPosition.longitude;
        fdclbslist->bsBestPosition.horizontalUnc =ind.bsBestPosition.horizontalUnc;
        fdclbslist->bsBestPosition.altitude =ind.bsBestPosition.altitude;
        fdclbslist->bsBestPosition.altitudeUnc =ind.bsBestPosition.altitudeUnc;
    }
    for (uint32_t i = 0; i<ind.BsList_len; i++) {
        fdclbslist->BsList[i].airInterfaceType = ind.BsList[i].airInterfaceType>3?ind.BsList[i].airInterfaceType+2:ind.BsList[i].airInterfaceType;
        fdclbslist->BsList[i].id1 = ind.BsList[i].id1;
        fdclbslist->BsList[i].id2 = ind.BsList[i].id2;
        fdclbslist->BsList[i].id3 = ind.BsList[i].id3;
        fdclbslist->BsList[i].id4 = ind.BsList[i].id4;
    }
    fdclbslist->BsList_len = ind.BsList_len;
//     fdclbslist->bsLocalTimestampList_valid = ind.bsLocalTimestampList_valid;
//     if(ind.bsLocalTimestampList_valid){
//        fdclbslist->bsLocalTimestampList_len = ind.bsLocalTimestampList_len;
//        for (uint32_t i = 0; i<ind.bsLocalTimestampList_len; i++) {
//            fdclbslist->bsLocalTimestampList[i]=ind.bsLocalTimestampList[i];
//        }
//    }
  return IZAT_SUCCESS;
}

int IzatApiV02::injectGtpWwanData(const Xt_Data* data)
{

  //code to write contact modem 
  qmiLocInjectFdclDataReqMsgT_v02 req;
  qmiLocInjectFdclDataIndMsgT_v02 ind;
  locClientReqUnionType req_union;
  locClientStatusEnumType st = eLOC_CLIENT_SUCCESS;

  memset(&ind, 0, sizeof(ind));
  memset(&req, 0, sizeof(req));
  req_union.pLocInjectFdclDataReq = &req;
  if(data != NULL){
      req.status = data->status;
      if(data->cellRecordTypeList_valid){
          req.cellRecordTypeList_valid=1;
          req.cellRecordTypeList_len = data->cellRecordTypeList_len;
      }
      if(data->clientControls_valid){
          req.clientControls_valid =1;
          req.clientControls = data->clientControls;
      }
      if(data->maxNumCellsToUpload_valid){
          req.maxNumCellsToUpload_valid =1;
          req.maxNumCellsToUpload=data->maxNumCellsToUpload;
      }
      if(data->minSecsBtwRequests_valid){
          req.minSecsBtwRequests_valid=1;
          req.minSecsBtwRequests=data->minSecsBtwRequests;
      }
      if(data->maxNumCellsToDownload_valid){
          req.maxNumCellsToDownload_valid=1;
          req.maxNumCellsToDownload=data->maxNumCellsToDownload;
      }
      if(data->maxGtpCellRequestsPerDay_valid){
          req.maxGtpCellRequestsPerDay_valid=1;
          req.maxGtpCellRequestsPerDay=data->maxGtpCellRequestsPerDay;
      }
      if(data->posDeltaTimeThreshold_valid){
          req.posDeltaTimeThreshold_valid=1;
          req.posDeltaTimeThreshold=data->posDeltaTimeThreshold;
      }

        if(data->restrictedRegionVersion_valid){
            req.restrictedRegionVersion_valid=1;
            req.restrictedRegionVersion=data->restrictedRegionVersion;
        }
        if(data->restrictedRegions_valid){
            uint32_t itr;
            req.restrictedRegions_valid=1;
            req.restrictedRegions_len = data->restrictedRegions_len;
            for(itr=0;itr<data->restrictedRegions_len;itr++){
                if(data->restrictedRegions[itr].homeMncValid){
                    req.restrictedRegions[itr].homeMncValid = data->restrictedRegions[itr].homeMncValid;
                    req.restrictedRegions[itr].homeMnc = data->restrictedRegions[itr].homeMnc;
                }
                req.restrictedRegions[itr].homeMcc = data->restrictedRegions[itr].homeMcc;
            }
        }
        req.cellPosList_len = data->cellPosList_len;
        req.cellRecordTypeList_valid = 1;
        req.cellRecordTypeList_len = data->cellPosList_len;

        for(int i =0; i< data->cellPosList_len ;i++){
            req.cellRecordTypeList[i] = data->cellRecordTypeList[i];
            req.cellPosList[i].validMask = data->cellPosList[i].validMask;
            req.cellPosList[i].cellId.airInterfaceType = data->cellPosList[i].cellId.airInterfaceType>5?data->cellPosList[i].cellId.airInterfaceType-2:data->cellPosList[i].cellId.airInterfaceType;
            req.cellPosList[i].cellId.id1 = data->cellPosList[i].cellId.id1;
            req.cellPosList[i].cellId.id2 = data->cellPosList[i].cellId.id2;
            req.cellPosList[i].cellId.id3 = data->cellPosList[i].cellId.id3;
            req.cellPosList[i].cellId.id4 = data->cellPosList[i].cellId.id4;
            req.cellPosList[i].latitude = data->cellPosList[i].latitude;
            req.cellPosList[i].longitude = data->cellPosList[i].longitude;
            req.cellPosList[i].horCoverageRadius = data->cellPosList[i].horCoverageRadius;

            if(data->cellPosList[i].validMask & FDCL_CELL_POS_MASK_VALID_ALTITUDE_V02)
            {
                req.cellPosList[i].altitude = data->cellPosList[i].altitude;
            }
            if(data->cellPosList[i].validMask & FDCL_CELL_POS_MASK_VALID_ALT_UNC_V02)
            {
                req.cellPosList[i].altUnc = data->cellPosList[i].altUnc;
            }
            if(data->cellPosList[i].validMask & FDCL_CELL_POS_MASK_VALID_ALT_CONFIDENCE_V02)
            {
                req.cellPosList[i].altConfidence = data->cellPosList[i].altConfidence;
            }
            if(data->cellPosList[i].validMask & FDCL_CELL_POS_MASK_VALID_ALT_RELIABILITY_V02)
            {
                req.cellPosList[i].altReliability = data->cellPosList[i].altReliability;
            }
        }
		if (data->positionResponse_valid)
		{
			req.responsePos_valid = 1;
			req.responsePos.latitude = data->positionResponse.latitude;
			req.responsePos.longitude = data->positionResponse.longitude;
			req.responsePos.horCoverageRadius = data->positionResponse.horCoverageRadius;
			req.responsePos.validMask = data->positionResponse.validMask;
			if (data->positionResponse.validMask & FDCL_CELL_POS_MASK_VALID_ALTITUDE_V02)
			{
				req.responsePos.altitude = data->cellPosList[0].altitude;
			}
			if (data->positionResponse.validMask & FDCL_CELL_POS_MASK_VALID_ALT_UNC_V02)
			{
				req.responsePos.altUnc = data->positionResponse.altUnc;
			}
			if (data->positionResponse.validMask & FDCL_CELL_POS_MASK_VALID_ALT_CONFIDENCE_V02)
			{
				req.responsePos.altConfidence = data->positionResponse.altConfidence;
			}
			if (data->positionResponse.validMask & FDCL_CELL_POS_MASK_VALID_ALT_RELIABILITY_V02)
			{
                req.responsePos.altReliability = data->positionResponse.altReliability;
            }
			LOC_LOGH2("inject scan position: LATITUDE:%d.%05d", (int)req.responsePos.latitude, (abs((int)(req.responsePos.latitude * 100000))) % 100000);
			LOC_LOGH2("inject scan position: LONGITUDE:%d.%05d", (int)req.responsePos.longitude, (abs((int)(req.responsePos.longitude * 100000))) % 100000);
			LOC_LOGH2("inject scan position: HORIZONTAL_UNC:%d.%02d", (int)req.responsePos.horCoverageRadius, (abs((int)(req.responsePos.horCoverageRadius * 100))) % 100);
        }
#ifdef FEATURE_LOCATION_MCS
    if(data->gtpMCSresponse_valid)
    {
      req.responseBuffer_valid = 1;
      if (QMI_LOC_INJ_DATA_MAX_SIZE_V02 < sizeof(GtpMCSResponse))
      {
        LOC_LOGH1("buffer size less than expected  %d",
          sizeof(GtpMCSResponse));
          return IZAT_INVALID_ARGUMENTS;
      }
      memset(&(req.responseBuffer), 0, QMI_LOC_INJ_DATA_MAX_SIZE_V02);
      memscpy(&(req.responseBuffer), sizeof(GtpMCSResponse),
        &(data->gtpMCSResponse), sizeof(GtpMCSResponse));
      req.responseBuffer_len = sizeof(GtpMCSResponse);
     }
#endif
    }
    st = loc_sync_send_req(clientHandle,
            QMI_LOC_INJECT_FDCL_DATA_REQ_V02  ,
            req_union,
            LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
            QMI_LOC_INJECT_FDCL_DATA_IND_V02 ,
            &ind);

    LOC_LOGH1("return value  %d",st);
    /*TODO::findout with kin how that there is no horcoverage member variable in asn struct */

    return IZAT_SUCCESS;
}

/* convert NMEA report to loc eng format and send the converted
report to loc eng */
void IzatApiV02::handleNmeaEvent(
    const qmiLocEventNmeaIndMsgT_v02 *nmea_report_ptr)
{
    if (NULL == nmea_report_ptr) {
        return;
    }

    const char* p_nmea = NULL;
    uint32_t q_nmea_len = 0;

    if (nmea_report_ptr->expandedNmea_valid) {
        p_nmea = nmea_report_ptr->expandedNmea;
        q_nmea_len = strlen(nmea_report_ptr->expandedNmea);
        if (q_nmea_len > QMI_LOC_EXPANDED_NMEA_STRING_MAX_LENGTH_V02) {
            q_nmea_len = QMI_LOC_EXPANDED_NMEA_STRING_MAX_LENGTH_V02;
        }
    }
    else
    {
        p_nmea = nmea_report_ptr->nmea;
        q_nmea_len = strlen(nmea_report_ptr->nmea);
        if (q_nmea_len > QMI_LOC_NMEA_STRING_MAX_LENGTH_V02) {
            q_nmea_len = QMI_LOC_NMEA_STRING_MAX_LENGTH_V02;
        }
    }

    if ((NULL != p_nmea) && (q_nmea_len > 0)) {
        LOC_LOGSL1("Sending NMEA string: %s", p_nmea);
        sendMsg(new MsgReportNmea(*this, p_nmea, q_nmea_len));
    }
}

LocationError IzatApiV02::getBackoffTime(LocationMetaData& metaData)
{
    qmiLocGetLocationBackoffPeriodReqMsgT_v02 getBackoffTimeReq;
    qmiLocGetLocationBackoffPeriodIndMsgT_v02 getBackoffTimeInd;
    locClientReqUnionType req_union;
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    locClientStatusEnumType st = eLOC_CLIENT_SUCCESS;

    LOC_LOGH0("Get Backoff Time Req");
    getBackoffTimeReq.transactionId = 1;
    req_union.pLocGetLocationBackoffPeriod = &getBackoffTimeReq;
    memset(&getBackoffTimeInd, 0, sizeof(getBackoffTimeInd));
    memset(&metaData, 0, sizeof(metaData));

    st = loc_sync_send_req(clientHandle,
            QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_REQ_V02 ,
            req_union,
            LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
            QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_IND_V02 ,
            &getBackoffTimeInd);

    if(st == eLOC_CLIENT_SUCCESS && getBackoffTimeInd.status == eQMI_LOC_SUCCESS_V02) {

        err = LOCATION_ERROR_SUCCESS;
        if (getBackoffTimeInd.locationBackoffPeriodMs_valid) {
            metaData.flags |= LOCATION_META_DATA_HAS_BACKOFF_TIME;
            metaData.backoffTimeMillis = getBackoffTimeInd.locationBackoffPeriodMs;
            LOC_LOGL1("Got Backoff Time %d", metaData.backoffTimeMillis);
        }
        if (getBackoffTimeInd.distanceToNearestGeofence_valid) {
            metaData.flags |= LOCATION_META_DATA_HAS_DISTANCE_TO_NEAREST_GEOFENCE;
            metaData.nearestGeofenceMeters = getBackoffTimeInd.distanceToNearestGeofence;
            LOC_LOGL1("Got dist to nearest gf %d", metaData.nearestGeofenceMeters);
        }
        if (!getBackoffTimeInd.locationBackoffPeriodMs_valid &&
                !getBackoffTimeInd.distanceToNearestGeofence_valid) {
            LOC_LOGE0("Neither backoff nor distance to gf valid");
        }

    } else {
        LOC_LOGSE2("Get Backoff Time failed. status: %s, ind status:%s\n",
                   loc_get_v02_client_status_name(status),
                   loc_get_v02_qmi_status_name(getBackoffTimeInd.status));
        err = LOCATION_ERROR_GENERAL_FAILURE;
    }

    return err;
}

LocationError IzatApiV02::getGnssEnergyConsumed(LocationMetaData& metaData)
{
    qmiLocQueryGNSSEnergyConsumedIndMsgT_v02 queryGnssEnergyConsumedInd;
    locClientReqUnionType req_union;
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    locClientStatusEnumType st = eLOC_CLIENT_SUCCESS;

    LOC_LOGH0("Query GNSS Energy Consumed Req");
    memset(&queryGnssEnergyConsumedInd, 0, sizeof(queryGnssEnergyConsumedInd));
    memset(&metaData, 0, sizeof(metaData));

    st = loc_sync_send_req(clientHandle,
            QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_REQ_V02 ,
            req_union,
            LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
            QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_IND_V02 ,
            &queryGnssEnergyConsumedInd);

    if(st == eLOC_CLIENT_SUCCESS) {

        metaData.flags |= LOCATION_META_DATA_ENERGY_CONSUMED_SINCE_LAST_BOOT;
        metaData.energyConsumedSinceLastBoot =
                queryGnssEnergyConsumedInd.energyConsumedSinceLastBoot;
        metaData.flags |= LOCATION_META_DATA_ENERGY_CONSUMED_SINCE_FIRST_BOOT;
        metaData.energyConsumedSinceFirstBoot =
                queryGnssEnergyConsumedInd.energyConsumedSinceFirstBoot;
        LOC_LOGL4("energy since last boot 0x%x%x, first boot 0x%x%x",
                  (int)(metaData.energyConsumedSinceLastBoot >> 32),
                  (int)metaData.energyConsumedSinceLastBoot,
                  (int)(metaData.energyConsumedSinceFirstBoot >> 32),
                  (int)metaData.energyConsumedSinceFirstBoot);
        err = LOCATION_ERROR_SUCCESS;

    } else {
        LOC_LOGSE1("Query GNSS Energy Consumed Req failed. status: %s",
                   loc_get_v02_client_status_name(status));
        err = LOCATION_ERROR_GENERAL_FAILURE;
    }

    return err;
}

LocPosTechMask IzatApiV02 :: convertPosTechMask(
  qmiLocPosTechMaskT_v02 mask)
{
   LocPosTechMask locTechMask = LOC_POS_TECH_MASK_DEFAULT;

   if (mask & QMI_LOC_POS_TECH_MASK_SATELLITE_V02)
      locTechMask |= LOC_POS_TECH_MASK_SATELLITE;

   if (mask & QMI_LOC_POS_TECH_MASK_CELLID_V02)
      locTechMask |= LOC_POS_TECH_MASK_CELLID;

   if (mask & QMI_LOC_POS_TECH_MASK_WIFI_V02)
      locTechMask |= LOC_POS_TECH_MASK_WIFI;

   if (mask & QMI_LOC_POS_TECH_MASK_SENSORS_V02)
      locTechMask |= LOC_POS_TECH_MASK_SENSORS;

   if (mask & QMI_LOC_POS_TECH_MASK_REFERENCE_LOCATION_V02)
      locTechMask |= LOC_POS_TECH_MASK_REFERENCE_LOCATION;

   if (mask & QMI_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION_V02)
      locTechMask |= LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION;

   if (mask & QMI_LOC_POS_TECH_MASK_AFLT_V02)
      locTechMask |= LOC_POS_TECH_MASK_AFLT;

   if (mask & QMI_LOC_POS_TECH_MASK_HYBRID_V02)
      locTechMask |= LOC_POS_TECH_MASK_HYBRID;

   return locTechMask;
}

LocNavSolutionMask IzatApiV02 :: convertNavSolutionMask(
  qmiLocNavSolutionMaskT_v02 mask)
{
   LocNavSolutionMask locNavMask = 0;

   if (mask & QMI_LOC_NAV_MASK_SBAS_CORRECTION_IONO_V02)
      locNavMask |= LOC_NAV_MASK_SBAS_CORRECTION_IONO;

   if (mask & QMI_LOC_NAV_MASK_SBAS_CORRECTION_FAST_V02)
      locNavMask |= LOC_NAV_MASK_SBAS_CORRECTION_FAST;

   if (mask & QMI_LOC_POS_TECH_MASK_WIFI_V02)
      locNavMask |= LOC_POS_TECH_MASK_WIFI;

   if (mask & QMI_LOC_NAV_MASK_SBAS_CORRECTION_LONG_V02)
      locNavMask |= LOC_NAV_MASK_SBAS_CORRECTION_LONG;

   if (mask & QMI_LOC_NAV_MASK_SBAS_INTEGRITY_V02)
      locNavMask |= LOC_NAV_MASK_SBAS_INTEGRITY;

   return locNavMask;
}

/*convert signal type to carrier frequency*/
float IzatApiV02 :: convertSignalTypeToCarrierFrequency(
    qmiLocGnssSignalTypeMaskT_v02 signalType,
    uint8_t gloFrequency)
{
    float carrierFrequency = 0.0;

    LOC_LOGL1("glo frequency %d", gloFrequency);
    
    switch (signalType) {
    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1CA_V02:
        carrierFrequency = GPS_L1CA_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1C_V02:
        carrierFrequency = GPS_L1C_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L2C_L_V02:
        carrierFrequency = GPS_L2C_L_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L5_Q_V02:
        carrierFrequency = GPS_L5_Q_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G1_V02:
        carrierFrequency = GLONASS_G1_CARRIER_FREQUENCY;
        if ((gloFrequency >= 1 && gloFrequency <= 14)) {
            carrierFrequency += ((gloFrequency - 8) * 562500);
        }
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G2_V02:
        carrierFrequency = GLONASS_G2_CARRIER_FREQUENCY;
        if ((gloFrequency >= 1 && gloFrequency <= 14)) {
            carrierFrequency += ((gloFrequency - 8) * 437500);
        }
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E1_C_V02:
        carrierFrequency = GALILEO_E1_C_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5A_Q_V02:
        carrierFrequency = GALILEO_E5A_Q_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5B_Q_V02:
        carrierFrequency = GALILEO_E5B_Q_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1_I_V02:
        carrierFrequency = BEIDOU_B1_I_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1C_V02:
        carrierFrequency = BEIDOU_B1C_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2_I_V02:
        carrierFrequency = BEIDOU_B2_I_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2A_I_V02:
        carrierFrequency = BEIDOU_B2A_I_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1CA_V02:
        carrierFrequency = QZSS_L1CA_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1S_V02:
        carrierFrequency = QZSS_L1S_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L2C_L_V02:
        carrierFrequency = QZSS_L2C_L_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L5_Q_V02:
        carrierFrequency = QZSS_L5_Q_CARRIER_FREQUENCY;
        break;

    case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_SBAS_L1_CA_V02:
        carrierFrequency = SBAS_L1_CA_CARRIER_FREQUENCY;
        break;

    default:
        LOC_LOGL2("Unsupported signalType 0x%x%x", (uint32_t)(signalType>>32), (uint32_t)(signalType));
        break;
    }
    return carrierFrequency;
} 

GnssSvType IzatApiV02 :: getLocApiSvSystemType (qmiLocSvSystemEnumT_v02 qmiSvSystemType) {
    GnssSvType locSvType = GNSS_SV_TYPE_UNKNOWN;

    switch (qmiSvSystemType) {
        case eQMI_LOC_SV_SYSTEM_GPS_V02:
            locSvType = GNSS_SV_TYPE_GPS;
            break;

        case eQMI_LOC_SV_SYSTEM_GALILEO_V02:
            locSvType = GNSS_SV_TYPE_GALILEO;
            break;

        case eQMI_LOC_SV_SYSTEM_SBAS_V02:
            locSvType = GNSS_SV_TYPE_SBAS;
            break;

        case eQMI_LOC_SV_SYSTEM_GLONASS_V02:
            locSvType = GNSS_SV_TYPE_GLONASS;
            break;

        case eQMI_LOC_SV_SYSTEM_BDS_V02:
        case eQMI_LOC_SV_SYSTEM_COMPASS_V02:
            locSvType = GNSS_SV_TYPE_BEIDOU;
            break;

        case eQMI_LOC_SV_SYSTEM_QZSS_V02:
            locSvType = GNSS_SV_TYPE_QZSS;
            break;

        default:
            LOC_LOGE1("Unsupported System type  = %d",qmiSvSystemType);
            break;
    }

    return locSvType;
}

GnssSignalTypeMask IzatApiV02 :: getLocApiSignalType (qmiLocGnssSignalTypeMaskT_v02 gnssSignalType) {
    GnssSignalTypeMask locSignalType = 0;

    switch (gnssSignalType) {
        case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1CA_V02:
            locSignalType = GNSS_SIGNAL_GPS_L1CA;
            break;

        case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L1C_V02:
            locSignalType = GNSS_SIGNAL_GPS_L1C;
            break;

        case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L2C_L_V02:
            locSignalType = GNSS_SIGNAL_GPS_L2;
            break;

        case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GPS_L5_Q_V02:
            locSignalType = GNSS_SIGNAL_GPS_L5;
            break;

        case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G1_V02:
            locSignalType = GNSS_SIGNAL_GLONASS_G1;
            break;
            
        case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GLONASS_G2_V02:
            locSignalType = GNSS_SIGNAL_GLONASS_G2;
            break;

        case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E1_C_V02:
            locSignalType = GNSS_SIGNAL_GALILEO_E1;
            break;
            
        case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5A_Q_V02:
            locSignalType = GNSS_SIGNAL_GALILEO_E5A;
            break;
            
        case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_GALILEO_E5B_Q_V02:
            locSignalType = GNSS_SIGNAL_GALILEO_E5B;
            break;
            
        case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1_I_V02:
            locSignalType = GNSS_SIGNAL_BEIDOU_B1I;
            break;
            
         case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B1C_V02:
            locSignalType = GNSS_SIGNAL_BEIDOU_B1C;
            break;
            
         case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2_I_V02:
            locSignalType = GNSS_SIGNAL_BEIDOU_B2I;
            break;
            
         case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_BEIDOU_B2A_I_V02:
            locSignalType = GNSS_SIGNAL_BEIDOU_B2AI;
            break;
            
         case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1CA_V02:
            locSignalType = GNSS_SIGNAL_QZSS_L1CA;
            break;
            
         case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L1S_V02:
            locSignalType = GNSS_SIGNAL_QZSS_L1S;
            break;
            
         case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L2C_L_V02:
             locSignalType = GNSS_SIGNAL_QZSS_L2;
            break;
            
         case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_QZSS_L5_Q_V02:
            locSignalType = GNSS_SIGNAL_QZSS_L5;
            break;
            
         case QMI_LOC_MASK_GNSS_SIGNAL_TYPE_SBAS_L1_CA_V02:
            locSignalType = GNSS_SIGNAL_SBAS_L1;
            break;

        default:
            LOC_LOGL2("signalType 0x%x%x", (uint32_t)(gnssSignalType>>32), (uint32_t)(gnssSignalType));
            break;
    }

    return locSignalType;
}

svInfoValidMask IzatApiV02::getLocApiSvInfoMask(qmiLocSvInfoValidMaskT_v02 validMask) {

    svInfoValidMask svInfoMask = 0;

    if(validMask & QMI_LOC_SV_INFO_MASK_VALID_SYSTEM_V02) {
        svInfoMask |= GNSS_SV_INFO_MASK_VALID_SYSTEM;
    }

    if(validMask & QMI_LOC_SV_INFO_MASK_VALID_GNSS_SVID_V02) {
        svInfoMask |= GNSS_SV_INFO_MASK_VALID_GNSS_SVID;
    }

    if(validMask & QMI_LOC_SV_INFO_MASK_VALID_HEALTH_STATUS_V02) {
        svInfoMask |= GNSS_SV_INFO_MASK_VALID_HEALTH_STATUS;
    }

    if(validMask & QMI_LOC_SV_INFO_MASK_VALID_PROCESS_STATUS_V02) {
        svInfoMask |= GNSS_SV_INFO_MASK_VALID_PROCESS_STATUS;
    }

    if(validMask & QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02) {
        svInfoMask |= GNSS_SV_INFO_MASK_VALID_SVINFO_MASK;
    }

    if(validMask & QMI_LOC_SV_INFO_MASK_VALID_ELEVATION_V02) {
        svInfoMask |= GNSS_SV_INFO_MASK_VALID_ELEVATION;
    }

    if(validMask & QMI_LOC_SV_INFO_MASK_VALID_AZIMUTH_V02) {
        svInfoMask |= GNSS_SV_INFO_MASK_VALID_AZIMUTH;
    }

    if(validMask & QMI_LOC_SV_INFO_MASK_VALID_SNR_V02) {
        svInfoMask |= GNSS_SV_INFO_MASK_VALID_SNR;
    }

    return svInfoMask;
}

void IzatApiV02 :: handleSvInfoEventIndication(
    const qmiLocEventGnssSvInfoIndMsgT_v02* svInfoEvent){

    uint8_t gloFrequency = 0; /*Dummy param, Applicable only with expanded SV list, Not needed for now*/
    GnssSvNotification* svNotifyPtr = NULL;
        
    if(NULL == svInfoEvent) {
        LOC_LOGE0("Empty SV info reported");
        return;
    }
    
    if((FALSE == svInfoEvent->svList_valid) || 
       (svInfoEvent->svList_len >= GNSS_SV_MAX )) {

        LOC_LOGE2("Invalid SV info indication reported SVList_Valid = %d SVList_Len = %d", 
                   svInfoEvent->svList_valid, svInfoEvent->svList_len);
        return;
    }

    svNotifyPtr = new GnssSvNotification();
    if(NULL == svNotifyPtr) {
        LOC_LOGE0("Memory allocation failed for SV info");
        return;
    }

    LOC_LOGM2("SV info indication reported SVList_Valid = %d SVList_Len = %d", 
               svInfoEvent->svList_valid, svInfoEvent->svList_len);
    
    svNotifyPtr->size = sizeof(*svNotifyPtr);
    svNotifyPtr->count = svInfoEvent->svList_len;
    for(int i = 0; i < (svInfoEvent->svList_len); i++) {

        svNotifyPtr->gnssSvs[i].size =                             sizeof(GnssSv);
        svNotifyPtr->gnssSvs[i].svInfoMask =                       getLocApiSvInfoMask(svInfoEvent->svList[i].validMask);
        svNotifyPtr->gnssSvs[i].azimuth =                          svInfoEvent->svList[i].azimuth;
        svNotifyPtr->gnssSvs[i].carrierFrequencyHz =               convertSignalTypeToCarrierFrequency(
                                                                      svInfoEvent->svList[i].gnssSignalType,
                                                                      gloFrequency); /*Dummy param*/
        svNotifyPtr->gnssSvs[i].cN0Dbhz =                          svInfoEvent->svList[i].snr;
        svNotifyPtr->gnssSvs[i].elevation =                        svInfoEvent->svList[i].elevation;
        svNotifyPtr->gnssSvs[i].gnssSignalTypeMask =               getLocApiSignalType(svInfoEvent->svList[i].gnssSignalType);
        svNotifyPtr->gnssSvs[i].gnssSvOptionsMask =                svInfoEvent->svList[i].svInfoMask;
        svNotifyPtr->gnssSvs[i].svId =                             svInfoEvent->svList[i].gnssSvId;
        svNotifyPtr->gnssSvs[i].type =                             getLocApiSvSystemType(svInfoEvent->svList[i].system);
        LOC_LOGE4("SV Id %d, SV Type %d, SV info mask reported %d , returned %d", 
                      (svNotifyPtr->gnssSvs[i].svId), 
                      (svNotifyPtr->gnssSvs[i].type),
                      (svInfoEvent->svList[i].validMask),
                      (svNotifyPtr->gnssSvs[i].svInfoMask));
    }

    sendMsg(new MsgReportSv(*this, *svNotifyPtr));
    if(svNotifyPtr) {
        free(svNotifyPtr);
    }
}

/* delete assistance date */
LocationError
IzatApiV02::deleteAidingData(const GnssAidingData& data){

    locClientReqUnionType req_union;
    locClientStatusEnumType status = eLOC_CLIENT_FAILURE_UNSUPPORTED;
    LocationError err = LOCATION_ERROR_SUCCESS;

    // Use the new API first
    qmiLocDeleteGNSSServiceDataReqMsgT_v02 delete_gnss_req;
    qmiLocDeleteGNSSServiceDataIndMsgT_v02 delete_gnss_resp;

    memset(&delete_gnss_req, 0, sizeof(delete_gnss_req));
    memset(&delete_gnss_resp, 0, sizeof(delete_gnss_resp));
    
    if (data.deleteAll) {
        delete_gnss_req.deleteAllFlag = true;
    } 
    else {
        if (GNSS_AIDING_DATA_SV_EPHEMERIS_BIT & data.sv.svMask) {
            delete_gnss_req.deleteSatelliteData_valid = 1;
            delete_gnss_req.deleteSatelliteData.deleteSatelliteDataMask |=
            QMI_LOC_DELETE_DATA_MASK_EPHEMERIS_V02;
        }
        if (GNSS_AIDING_DATA_SV_ALMANAC_BIT & data.sv.svMask) {
            delete_gnss_req.deleteSatelliteData_valid = 1;
            delete_gnss_req.deleteSatelliteData.deleteSatelliteDataMask |=
            QMI_LOC_DELETE_DATA_MASK_ALMANAC_V02;
        }
        if (GNSS_AIDING_DATA_SV_HEALTH_BIT & data.sv.svMask) {
            delete_gnss_req.deleteSatelliteData_valid = 1;
            delete_gnss_req.deleteSatelliteData.deleteSatelliteDataMask |=
            QMI_LOC_DELETE_DATA_MASK_SVHEALTH_V02;
        }
        if (GNSS_AIDING_DATA_SV_DIRECTION_BIT & data.sv.svMask) {
            delete_gnss_req.deleteSatelliteData_valid = 1;
            delete_gnss_req.deleteSatelliteData.deleteSatelliteDataMask |=
            QMI_LOC_DELETE_DATA_MASK_SVDIR_V02;
        }
        if (GNSS_AIDING_DATA_SV_STEER_BIT & data.sv.svMask) {
            delete_gnss_req.deleteSatelliteData_valid = 1;
            delete_gnss_req.deleteSatelliteData.deleteSatelliteDataMask |=
            QMI_LOC_DELETE_DATA_MASK_SVSTEER_V02;
        }
        if (GNSS_AIDING_DATA_SV_ALMANAC_CORR_BIT & data.sv.svMask) {
            delete_gnss_req.deleteSatelliteData_valid = 1;
            delete_gnss_req.deleteSatelliteData.deleteSatelliteDataMask |=
            QMI_LOC_DELETE_DATA_MASK_ALM_CORR_V02;
        }
        if (GNSS_AIDING_DATA_SV_BLACKLIST_BIT & data.sv.svMask) {
            delete_gnss_req.deleteSatelliteData_valid = 1;
            delete_gnss_req.deleteSatelliteData.deleteSatelliteDataMask |=
            QMI_LOC_DELETE_DATA_MASK_BLACKLIST_V02;
        }
        if (GNSS_AIDING_DATA_SV_SA_DATA_BIT & data.sv.svMask) {
            delete_gnss_req.deleteSatelliteData_valid = 1;
            delete_gnss_req.deleteSatelliteData.deleteSatelliteDataMask |=
            QMI_LOC_DELETE_DATA_MASK_SA_DATA_V02;
        }
        if (GNSS_AIDING_DATA_SV_NO_EXIST_BIT & data.sv.svMask) {
            delete_gnss_req.deleteSatelliteData_valid = 1;
            delete_gnss_req.deleteSatelliteData.deleteSatelliteDataMask |=
            QMI_LOC_DELETE_DATA_MASK_SV_NO_EXIST_V02;
        }
        if (GNSS_AIDING_DATA_SV_IONOSPHERE_BIT & data.sv.svMask) {
            delete_gnss_req.deleteSatelliteData_valid = 1;
            delete_gnss_req.deleteSatelliteData.deleteSatelliteDataMask |=
            QMI_LOC_DELETE_DATA_MASK_IONO_V02;
        }
        if (GNSS_AIDING_DATA_SV_TIME_BIT & data.sv.svMask) {
            delete_gnss_req.deleteSatelliteData_valid = 1;
            delete_gnss_req.deleteSatelliteData.deleteSatelliteDataMask |=
            QMI_LOC_DELETE_DATA_MASK_TIME_V02;
        }
        if (GNSS_AIDING_DATA_SV_MB_DATA & data.sv.svMask) {
            delete_gnss_req.deleteSatelliteData_valid = 1;
            delete_gnss_req.deleteSatelliteData.deleteSatelliteDataMask |=
            QMI_LOC_DELETE_DATA_MASK_MB_DATA_V02;
        }
        if (delete_gnss_req.deleteSatelliteData_valid) {
            if (GNSS_AIDING_DATA_SV_TYPE_GPS_BIT & data.sv.svTypeMask) {
                delete_gnss_req.deleteSatelliteData.system |= QMI_LOC_SYSTEM_GPS_V02;
            }
            if (GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT & data.sv.svTypeMask) {
                delete_gnss_req.deleteSatelliteData.system |= QMI_LOC_SYSTEM_GLO_V02;
            }
            if (GNSS_AIDING_DATA_SV_TYPE_QZSS_BIT & data.sv.svTypeMask) {
                delete_gnss_req.deleteSatelliteData.system |= QMI_LOC_SYSTEM_BDS_V02;
            }
            if (GNSS_AIDING_DATA_SV_TYPE_BEIDOU_BIT & data.sv.svTypeMask) {
                delete_gnss_req.deleteSatelliteData.system |= QMI_LOC_SYSTEM_GAL_V02;
            }
            if (GNSS_AIDING_DATA_SV_TYPE_GALILEO_BIT & data.sv.svTypeMask) {
                delete_gnss_req.deleteSatelliteData.system |= QMI_LOC_SYSTEM_QZSS_V02;
            }
            #if 0
            if (GNSS_AIDING_DATA_SV_TYPE_NAVIC_BIT & data.sv.svTypeMask) {
                delete_gnss_req.deleteSatelliteData.system |= QMI_LOC_SYSTEM_NAVIC_V02;
            }
            #endif
        }
        if (GNSS_AIDING_DATA_COMMON_POSITION_BIT & data.common.mask) {
            delete_gnss_req.deleteCommonDataMask_valid = 1;
            delete_gnss_req.deleteCommonDataMask |= QMI_LOC_DELETE_COMMON_MASK_POS_V02;
        }
        if (GNSS_AIDING_DATA_COMMON_TIME_BIT & data.common.mask) {
            delete_gnss_req.deleteCommonDataMask_valid = 1;
            delete_gnss_req.deleteCommonDataMask |= QMI_LOC_DELETE_COMMON_MASK_TIME_V02;
        }
        if (GNSS_AIDING_DATA_COMMON_UTC_BIT & data.common.mask) {
            delete_gnss_req.deleteCommonDataMask_valid = 1;
            delete_gnss_req.deleteCommonDataMask |= QMI_LOC_DELETE_COMMON_MASK_UTC_V02;
        }
        if (GNSS_AIDING_DATA_COMMON_RTI_BIT & data.common.mask) {
            delete_gnss_req.deleteCommonDataMask_valid = 1;
            delete_gnss_req.deleteCommonDataMask |= QMI_LOC_DELETE_COMMON_MASK_RTI_V02;
        }
        if (GNSS_AIDING_DATA_COMMON_FREQ_BIAS_EST_BIT & data.common.mask) {
            delete_gnss_req.deleteCommonDataMask_valid = 1;
            delete_gnss_req.deleteCommonDataMask |= QMI_LOC_DELETE_COMMON_MASK_FREQ_BIAS_EST_V02;
        }
        if (GNSS_AIDING_DATA_COMMON_CELLDB_BIT & data.common.mask) {
            delete_gnss_req.deleteCellDbDataMask_valid = 1;
            delete_gnss_req.deleteCellDbDataMask =
                        (QMI_LOC_MASK_DELETE_CELLDB_POS_V02 |
                         QMI_LOC_MASK_DELETE_CELLDB_LATEST_GPS_POS_V02 |
                         QMI_LOC_MASK_DELETE_CELLDB_OTA_POS_V02 |
                         QMI_LOC_MASK_DELETE_CELLDB_EXT_REF_POS_V02 |
                         QMI_LOC_MASK_DELETE_CELLDB_TIMETAG_V02 |
                         QMI_LOC_MASK_DELETE_CELLDB_CELLID_V02 |
                         QMI_LOC_MASK_DELETE_CELLDB_CACHED_CELLID_V02 |
                         QMI_LOC_MASK_DELETE_CELLDB_LAST_SRV_CELL_V02 |
                         QMI_LOC_MASK_DELETE_CELLDB_CUR_SRV_CELL_V02 |
                         QMI_LOC_MASK_DELETE_CELLDB_NEIGHBOR_INFO_V02);
        }
    }
    
    req_union.pDeleteGNSSServiceDataReq = &delete_gnss_req;
    status = loc_sync_send_req(clientHandle,
                               QMI_LOC_DELETE_GNSS_SERVICE_DATA_REQ_V02,
                               req_union,
                               LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                               QMI_LOC_DELETE_GNSS_SERVICE_DATA_IND_V02,
                               &delete_gnss_resp);
    
    if (status != eLOC_CLIENT_SUCCESS ||
        eQMI_LOC_SUCCESS_V02 != delete_gnss_resp.status) {
        
        LOC_LOGE2("Status %d, Error %d", status, delete_gnss_resp.status);
        err = LOCATION_ERROR_GENERAL_FAILURE;
    }
    return err;
}
}  // namespace izat_core


/*****************************************************************************
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gnss_worker_thread.h"
#include "gnss_worker_sm.h"
#include "gnss_worker_utils.h"
#include "gnss_worker_tracking_sm.h"
#include "gnss_worker_geofence_sm.h"
#include "app_thread.h"
#include "app_utils_misc.h"
#include "app_cfg.h"
#include "app_utils_misc.h"
#include "controller_thread.h"

/* Thread Constants */
static char* _GnssWorkerTaskName = "gnss_worker";
static app_thread_id _GnssWorkerTaskId = APP_THREAD_ID_GNSS_WORKER;

static TX_MUTEX* _GnssWorkerMutex;
static TX_EVENT_FLAGS_GROUP* _GnssWorkerSingleshotSignal;

int gnss_worker_get_task_id()
{
    return _GnssWorkerTaskId;
}
char* gnss_worker_get_task_name()
{
    return _GnssWorkerTaskName;
}

static qapi_Location_t* _BestAvailLocationOutPtr;

// INTERNAL FUNCTIONS
static void gnss_worker_send_msg_request_best_avail_location();

// Synchronous wrappers on top of message processing in this thread
void gnss_worker_sync_get_best_avail_location(qapi_Location_t* locationOutPtr)
{
    app_utils_mutex_get(_GnssWorkerMutex);

    APP_LOG_GNSS_WORKER_REQUEST("Get Best Available Location");

    // Set output memory
    _BestAvailLocationOutPtr = locationOutPtr;
    if (_BestAvailLocationOutPtr == NULL)
    {
        APP_LOGE("Null Location pointer to fetch best avail pos");
        return;
    }

    _BestAvailLocationOutPtr->size = 0;

    // Send the message
    gnss_worker_send_msg_request_best_avail_location();

    // Now block on the best available position fetched signal
    // This signal will be sent by gnss_worker thread when best available position
    // is fetched and populated in the _BestAvailLocationOutPtr
    // Then we allow this function to return.
    // For the caller, this is a blocking function which fetches the best available position.
    app_utils_timedwait_on_signal(
            _GnssWorkerSingleshotSignal, _GnssWorkerMutex, MILLIS_TO_TICKS(1000));

    if (_BestAvailLocationOutPtr->size != 0)
    {
        app_utils_log_location_report(*_BestAvailLocationOutPtr);
    }
    else
    {
        APP_LOGE("Failed to get best avail pos.");
    }

    app_utils_mutex_put(_GnssWorkerMutex);
}

void gnss_worker_report_best_avail_location(qapi_Location_t location)
{
    APP_LOGM("gnss_worker_report_best_avail_location");

    app_utils_mutex_get(_GnssWorkerMutex);

    memcpy(_BestAvailLocationOutPtr, &location, sizeof(qapi_Location_t));

    // Set the singleshot callback signal
    app_utils_set_signal(_GnssWorkerSingleshotSignal, _GnssWorkerMutex);

    // Also send the location to controller thread
    local_controller_send_msg_report_location(location);

    app_utils_mutex_put(_GnssWorkerMutex);
}

void gnss_worker_report_singleshot_location(qapi_Location_t location)
{
    APP_LOGM("gnss_worker_report_singleshot_location");

    // Send the location to controller thread
    local_controller_send_msg_report_location(location);
}

// Internal struct to allow passing message data to this thread
// message handlers
typedef struct gnss_worker_msg
{
    qapi_Location_t* locationOutPtr;

    // gnss_worker_send_msg_request_delayed_location
    uint32 reportDelaySec;
    qapi_Location_Power_Level_t maxPower;
    qapi_Location_Accuracy_Level_t minAccuracy;
    uint32 allowPriorityInversion;

    // gnss_worker_send_msg_cb_singleshot
    qapi_Location_t location;
    qapi_Location_Error_t err;

    // gnss_worker_send_msg_cb_meta_data
    qapi_Location_Meta_Data_t metaData;

    // gnss_worker_send_msg_cb_motion
    qapi_Location_Motion_Info_t motionInfo;

    // gnss_worker_send_msg_cb_nmea
    qapi_Gnss_Nmea_t nmea;

    // gnss_worker_send_msg_cb_geofence_breach
    qapi_Location_t geofenceBreachLocation;
    qapi_Geofence_Breach_t geofenceBreachType;
    uint64_t geofenceBreachTimestamp;

    // Callback to process in thread context
    gnss_worker_thread_cb_type callback;
    uint32 cbData;

    // Tracking start request
    uint32 trackingTbf;
    uint32 trackingDbf;
    qapi_Location_Accuracy_Level_t trackingAccuracy;

    // Geofence add request
    // Geofence parameters
    double geofenceLatitude;
    double geofenceLongitude;
    double geofenceRadius;
    uint32 geofenceMask;
    uint32 geofenceResponsivenessMillis;
    uint32 geofenceDwellMillis;

    // WWAN sleep callback
    uint32 wwanSleepDuration;

} gnss_worker_msg;


/***************************************************************************
    TRACKING MODE APIs - START
****************************************************************************/
void gnss_worker_send_msg_request_tracking_start(
        uint32 tbf, uint32 dbf, qapi_Location_Accuracy_Level_t accuracy,
        uint32 allowPriorityInversion)
{
    APP_LOG_GNSS_WORKER_REQUEST("Start tracking session tbf %d dbf %d accuracy %d",
            tbf, dbf, accuracy);

    gnss_worker_msg workerMsg = {0};
    workerMsg.trackingTbf = tbf;
    workerMsg.trackingDbf = dbf;
    workerMsg.trackingAccuracy = accuracy;
    workerMsg.allowPriorityInversion = allowPriorityInversion;

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_REQUEST_TRACKING_START,
            &workerMsg, sizeof(gnss_worker_msg));
}

static void _gnss_worker_handle_req_tracking_start(void* msg, size_t size)
{
    // validate
    gnss_worker_msg* workerMsg = (gnss_worker_msg*)msg;
    if (workerMsg == NULL || size != sizeof(gnss_worker_msg))
    {
        APP_LOGE("Invalid msg");
        return;
    }

    // Process the request in State Machine
    gnss_worker_tracking_sm_request_tracking_start(
            workerMsg->trackingTbf, workerMsg->trackingDbf,
            workerMsg->trackingAccuracy, workerMsg->allowPriorityInversion);
}

void gnss_worker_send_msg_cb_tracking(qapi_Location_t location)
{
    gnss_worker_msg workerMsg = {0};
    size_t workerMsgSize = sizeof(gnss_worker_msg);

    memcpy(&workerMsg.location, &location, sizeof(qapi_Location_t));

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_CB_TRACKING,
            &workerMsg, workerMsgSize);
}

static void _gnss_worker_handle_cb_tracking(void* msg, size_t size)
{
    // validate
    gnss_worker_msg* workerMsg = (gnss_worker_msg*)msg;
    if (workerMsg == NULL || size != sizeof(gnss_worker_msg))
    {
        APP_LOGE("Invalid msg");
        return;
    }

    // notify the state machine
    gnss_worker_tracking_sm_tracking_cb(workerMsg->location);
}

void gnss_worker_send_msg_request_tracking_stop()
{
    APP_LOG_GNSS_WORKER_REQUEST("Stop tracking session");

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_REQUEST_TRACKING_STOP,
            NULL, 0);
}

static void _gnss_worker_handle_req_tracking_stop(void* msg, size_t size)
{
    // Process the request in State Machine
    gnss_worker_tracking_sm_request_tracking_stop();
}
/***************************************************************************
    TRACKING MODE APIs - END
****************************************************************************/

/***************************************************************************
    GEOFENCE MODE APIs - START
****************************************************************************/
void gnss_worker_send_msg_request_add_geofence(
        double latitude, double longitude, double radius,
        qapi_Geofence_Breach_Mask_t mask, uint32 responsiveness, uint32 dwellTime,
        uint32 allowPriorityInversion)
{
    APP_LOG_GNSS_WORKER_REQUEST(
            "Add Geofence lat %d.%d, long %d.%d, rad %d, mask %d, resp %d, dwell %d",
            (int)latitude, (abs((int)(latitude * 100000))) % 100000,
            (int)longitude, (abs((int)(longitude * 100000))) % 100000,
            radius, mask, responsiveness, dwellTime);

    gnss_worker_msg workerMsg = {0};
    workerMsg.geofenceLatitude = latitude;
    workerMsg.geofenceLongitude = longitude;
    workerMsg.geofenceRadius = radius;
    workerMsg.geofenceMask = mask;
    workerMsg.geofenceResponsivenessMillis = responsiveness;
    workerMsg.geofenceDwellMillis = dwellTime;

    workerMsg.allowPriorityInversion = allowPriorityInversion;

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_REQUEST_GEOFENCE_ADD,
            &workerMsg, sizeof(gnss_worker_msg));
}

static void _gnss_worker_handle_req_geofence_add(void* msg, size_t size)
{
    // validate
    gnss_worker_msg* workerMsg = (gnss_worker_msg*)msg;
    if (workerMsg == NULL || size != sizeof(gnss_worker_msg))
    {
        APP_LOGE("Invalid msg");
        return;
    }

    // Process the request in State Machine
    gnss_worker_geofence_sm_request_add_geofence(
            workerMsg->geofenceLatitude, workerMsg->geofenceLongitude, workerMsg->geofenceRadius,
            workerMsg->geofenceMask, workerMsg->geofenceResponsivenessMillis, workerMsg->geofenceDwellMillis,
            workerMsg->allowPriorityInversion);
}

void gnss_worker_send_msg_cb_geofence_breach(
        qapi_Location_t location, qapi_Geofence_Breach_t type, uint64_t timestamp)
{
    gnss_worker_msg workerMsg = {0};
    size_t workerMsgSize = sizeof(gnss_worker_msg);

    memcpy(&workerMsg.geofenceBreachLocation, &location, sizeof(qapi_Location_t));
    workerMsg.geofenceBreachType = type;
    workerMsg.geofenceBreachTimestamp = timestamp;

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_CB_GEOFENCE_BREACH,
            &workerMsg, workerMsgSize);
}

static void _gnss_worker_handle_cb_geofence_breach(void* msg, size_t size)
{
    // validate
    gnss_worker_msg* workerMsg = (gnss_worker_msg*)msg;
    if (workerMsg == NULL || size != sizeof(gnss_worker_msg))
    {
        APP_LOGE("Invalid msg");
        return;
    }

    // notify the state machine
    gnss_worker_geofence_sm_breach_cb(
            workerMsg->geofenceBreachLocation, workerMsg->geofenceBreachType,
            workerMsg->geofenceBreachTimestamp);
}

void gnss_worker_send_msg_request_remove_geofence()
{
    APP_LOG_GNSS_WORKER_REQUEST("Remove Geofence");

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_REQUEST_GEOFENCE_REMOVE,
            NULL, 0);
}

static void _gnss_worker_handle_req_geofence_remove(void* msg, size_t size)
{
    // Process the request in State Machine
    gnss_worker_geofence_sm_request_remove_geofence();
}
/***************************************************************************
    GEOFENCE MODE APIs - END
****************************************************************************/


/* Utility methods for external threads to post messages to this thread */
void gnss_worker_send_msg_request_best_avail_location()
{
    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_REQUEST_BEST_AVAIL_LOCATION,
            NULL, 0);
}

void gnss_worker_send_msg_request_delayed_location(
        uint32 reportDelaySec, qapi_Location_Power_Level_t maxPower,
        qapi_Location_Accuracy_Level_t minAccuracy, uint32 allowPriorityInversion)
{
    APP_LOG_GNSS_WORKER_REQUEST("Need location after %d seconds", reportDelaySec);

    gnss_worker_msg workerMsg = {0};
    size_t workerMsgSize = sizeof(gnss_worker_msg);

    workerMsg.reportDelaySec = reportDelaySec;
    workerMsg.maxPower = maxPower;
    workerMsg.minAccuracy = minAccuracy;
    workerMsg.allowPriorityInversion = allowPriorityInversion;

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_REQUEST_DELAYED_LOCATION,
            &workerMsg, workerMsgSize);
}

void gnss_worker_send_msg_cb_singleshot(
        qapi_Location_t location, qapi_Location_Error_t err)
{
    gnss_worker_msg workerMsg = {0};
    size_t workerMsgSize = sizeof(gnss_worker_msg);

    memcpy(&workerMsg.location, &location, sizeof(qapi_Location_t));
    workerMsg.err = err;

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_CB_SINGLESHOT,
            &workerMsg, workerMsgSize);
}

void gnss_worker_send_msg_cb_meta_data(qapi_Location_Meta_Data_t metaData)
{
    gnss_worker_msg workerMsg = {0};
    size_t workerMsgSize = sizeof(gnss_worker_msg);

    memcpy(&workerMsg.metaData, &metaData, sizeof(qapi_Location_Meta_Data_t));

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_CB_META_DATA,
            &workerMsg, workerMsgSize);
}

void gnss_worker_send_msg_cb_motion(qapi_Location_Motion_Info_t motionInfo)
{
    gnss_worker_msg workerMsg = {0};
    size_t workerMsgSize = sizeof(gnss_worker_msg);

    memcpy(&workerMsg.motionInfo, &motionInfo, sizeof(qapi_Location_Motion_Info_t));

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_CB_MOTION_TRACKING,
            &workerMsg, workerMsgSize);
}

void gnss_worker_send_msg_cb_nmea(qapi_Gnss_Nmea_t nmea)
{
    gnss_worker_msg workerMsg = {0};
    size_t workerMsgSize = sizeof(gnss_worker_msg);

    memcpy(&workerMsg.nmea, &nmea, sizeof(qapi_Gnss_Nmea_t));

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_CB_NMEA,
            &workerMsg, workerMsgSize);
}

void gnss_worker_send_msg_cb_wwan_sleep(uint32 sleepDuration)
{
    gnss_worker_msg workerMsg = {0};
    size_t workerMsgSize = sizeof(gnss_worker_msg);

    workerMsg.wwanSleepDuration = sleepDuration;

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_CB_WWAN_SLEEP,
            &workerMsg, workerMsgSize);
}

void gnss_worker_send_msg_process_cb(gnss_worker_thread_cb_type callback, uint32 cbData)
{
    gnss_worker_msg workerMsg = {0};
    size_t workerMsgSize = sizeof(gnss_worker_msg);

    workerMsg.callback = callback;
    workerMsg.cbData = cbData;

    app_thread_send_msg(
            _GnssWorkerTaskId,
            GNSS_WORKER_MSG_ID_PROCESS_CB,
            &workerMsg, workerMsgSize);
}

/* Message Handlers for this thread */
static void _gnss_worker_handle_req_best_avail_location(void* msg, size_t size)
{
    // validate
    gnss_worker_msg* workerMsg = (gnss_worker_msg*)msg;
    if (workerMsg != NULL || size != 0)
    {
        APP_LOGE("Invalid msg, no payload expected for best avail request");
        return;
    }

    // Process
    gnss_worker_sm_request_best_avail_location();
}

static void _gnss_worker_handle_req_delayed_location(void* msg, size_t size)
{
    // Message parameters
    uint32 reportDelaySec = 0;
    qapi_Location_Power_Level_t maxPower = 0;
    qapi_Location_Accuracy_Level_t minAccuracy = 0;
    uint32 allowPriorityInversion = 0;

    // validate
    gnss_worker_msg* workerMsg = (gnss_worker_msg*)msg;
    if (workerMsg == NULL || size != sizeof(gnss_worker_msg))
    {
        APP_LOGE("Invalid msg");
        return;
    }

    // Extract parameters
    reportDelaySec = workerMsg->reportDelaySec;
    maxPower = workerMsg->maxPower;
    minAccuracy = workerMsg->minAccuracy;
    allowPriorityInversion = workerMsg->allowPriorityInversion;

    // Process
    gnss_worker_sm_request_delayed_location(
            reportDelaySec, maxPower, minAccuracy, allowPriorityInversion);
}

/* QAPI Callbacks */
static void _gnss_worker_handle_cb_singleshot(void* msg, size_t size)
{
    // Message parameters
    qapi_Location_t location = {};
    qapi_Location_Error_t err = QAPI_LOCATION_ERROR_GENERAL_FAILURE;

    // validate
    gnss_worker_msg* workerMsg = (gnss_worker_msg*)msg;
    if (workerMsg == NULL || size != sizeof(gnss_worker_msg))
    {
        APP_LOGE("Invalid msg");
        return;
    }

    // Extract parameters
    location = workerMsg->location;
    err = workerMsg->err;

    // notify the state machine
    gnss_worker_sm_singleshot_cb(location, err);
}

static void _gnss_worker_handle_cb_metadata(void* msg, size_t size)
{
    // Message parameters
    qapi_Location_Meta_Data_t metaData = {};

    // validate
    gnss_worker_msg* workerMsg = (gnss_worker_msg*)msg;
    if (workerMsg == NULL || size != sizeof(gnss_worker_msg))
    {
        APP_LOGE("Invalid msg");
        return;
    }

    // Extract parameters
    metaData = workerMsg->metaData;

    // notify the state machines
    switch(APP_CONFIG.LOCATION_MODE)
    {
    case 0:
        gnss_worker_sm_meta_data_cb(metaData);
        break;
    case 1:
        gnss_worker_tracking_sm_meta_data_cb(metaData);
        break;
    case 2:
        gnss_worker_geofence_sm_meta_data_cb(metaData);
        break;
    default:
        break;
    }
}

static void _gnss_worker_handle_cb_motion(void* msg, size_t size)
{
    // Message parameters
    qapi_Location_Motion_Info_t motionInfo = {};

    // validate
    gnss_worker_msg* workerMsg = (gnss_worker_msg*)msg;
    if (workerMsg == NULL || size != sizeof(gnss_worker_msg))
    {
        APP_LOGE("Invalid msg");
        return;
    }

    // Extract parameters
    motionInfo = workerMsg->motionInfo;

    // notify the state machines
    switch(APP_CONFIG.LOCATION_MODE)
    {
    case 0:
        gnss_worker_sm_motion_info_cb(motionInfo);
        break;
    case 1:
        gnss_worker_tracking_sm_motion_info_cb(motionInfo);
        break;
    default:
        break;
    }
}

static void _gnss_worker_handle_cb_nmea(void* msg, size_t size)
{
    // validate
    gnss_worker_msg* workerMsg = (gnss_worker_msg*)msg;
    if (workerMsg == NULL || size != sizeof(gnss_worker_msg))
    {
        APP_LOGE("Invalid msg");
        return;
    }

    // notify the state machines
    switch(APP_CONFIG.LOCATION_MODE)
    {
    case 1:
        // Only needed for tracking SM
        gnss_worker_tracking_sm_nmea_cb(workerMsg->nmea);
        break;
    default:
        break;
    }
}

static void _gnss_worker_handle_cb_wwan_sleep(void* msg, size_t size)
{
    // validate
    gnss_worker_msg* workerMsg = (gnss_worker_msg*)msg;
    if (workerMsg == NULL || size != sizeof(gnss_worker_msg))
    {
        APP_LOGE("Invalid msg");
        return;
    }

    // notify the state machines
    switch(APP_CONFIG.LOCATION_MODE)
    {
    case 0:
        gnss_worker_sm_wwan_sleep_cb(workerMsg->wwanSleepDuration);
        break;
    default:
        break;
    }
}

static void _gnss_worker_handle_process_cb(void* msg, size_t size)
{
    // validate
    gnss_worker_msg* workerMsg = (gnss_worker_msg*)msg;
    if (workerMsg == NULL || size != sizeof(gnss_worker_msg))
    {
        APP_LOGE("Invalid msg");
        return;
    }

    // Message parameters
    gnss_worker_thread_cb_type callback = workerMsg->callback;
    uint32 cbData = workerMsg->cbData;

    // Invoke the callback
    if (callback != NULL)
    {
        callback(cbData);
    }
}

// Message router
static void _gnss_worker_process_msg(int msgId, void *msg, size_t msgSize)
{
    APP_LOGM("Processing msgId %d", msgId);

    switch (msgId)
    {
    case GNSS_WORKER_MSG_ID_REQUEST_BEST_AVAIL_LOCATION:
        APP_LOGM("Recvd REQUEST_BEST_AVAIL_LOCATION msgSize %d", msgSize);
        _gnss_worker_handle_req_best_avail_location(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_REQUEST_DELAYED_LOCATION:
        APP_LOGM("Recvd REQUEST_DELAYED_LOCATION msgSize %d", msgSize);
        _gnss_worker_handle_req_delayed_location(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_REQUEST_TRACKING_START:
        APP_LOGM("Recvd REQUEST_TRACKING_START msgSize %d", msgSize);
        _gnss_worker_handle_req_tracking_start(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_REQUEST_TRACKING_STOP:
        APP_LOGM("Recvd REQUEST_TRACKING_STOP msgSize %d", msgSize);
        _gnss_worker_handle_req_tracking_stop(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_REQUEST_GEOFENCE_ADD:
        APP_LOGM("Recvd REQUEST_GEOFENCE_ADD msgSize %d", msgSize);
        _gnss_worker_handle_req_geofence_add(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_REQUEST_GEOFENCE_REMOVE:
        APP_LOGM("Recvd REQUEST_GEOFENCE_REMOVE msgSize %d", msgSize);
        _gnss_worker_handle_req_geofence_remove(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_CB_TRACKING:
        APP_LOGM("Recvd CB_TRACKING msgSize %d", msgSize);
        _gnss_worker_handle_cb_tracking(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_CB_SINGLESHOT:
        APP_LOGM("Recvd CB_SINGLESHOT msgSize %d", msgSize);
        _gnss_worker_handle_cb_singleshot(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_CB_META_DATA:
        APP_LOGM("Recvd CB_META_DATA msgSize %d", msgSize);
        _gnss_worker_handle_cb_metadata(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_CB_MOTION_TRACKING:
        APP_LOGM("Recvd CB_MOTION_TRACKING msgSize %d", msgSize);
        _gnss_worker_handle_cb_motion(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_CB_NMEA:
        APP_LOGM("Recvd CB_NMEA msgSize %d", msgSize);
        _gnss_worker_handle_cb_nmea(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_CB_GEOFENCE_BREACH:
        APP_LOGM("Recvd CB_GEOFENCE_BREACH msgSize %d", msgSize);
        _gnss_worker_handle_cb_geofence_breach(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_CB_WWAN_SLEEP:
        APP_LOGM("Recvd CB_WWAN_SLEEP msgSize %d", msgSize);
        _gnss_worker_handle_cb_wwan_sleep(msg, msgSize);
        break;

    case GNSS_WORKER_MSG_ID_PROCESS_CB:
        APP_LOGM("Recvd PROCESS_CB msgSize %d", msgSize);
        _gnss_worker_handle_process_cb(msg, msgSize);
        break;

    default:
        APP_LOGM("Incorrect msgId %d", msgId);
    }
}

/*This is the entry point for Location task */
void gnss_worker_task_entry(ULONG args) {

    APP_LOGH("GNSS Worker Thread started. TCB %x TID %d", tx_thread_identify(), args);

    // Initializations
    app_utils_mutex_init(&_GnssWorkerMutex, "_GnssWorkerMutex");
    app_utils_init_signal(&_GnssWorkerSingleshotSignal, "_GnssWorkerSingleshotSignal");

    gnss_worker_utils_init();

    if (APP_CONFIG.LOCATION_MODE == 0)
    {
        gnss_worker_sm_init();
    }
    else if (APP_CONFIG.LOCATION_MODE == 1)
    {
        gnss_worker_tracking_sm_init();
    }
    else if (APP_CONFIG.LOCATION_MODE == 2)
    {
        gnss_worker_geofence_sm_init();
    }
    else
    {
        APP_LOGE("Invalid location mode %d", APP_CONFIG.LOCATION_MODE);
    }

    AppMsg *payload = NULL;
    while (0 == app_thread_rcv_msg(args, (void**) &payload))
    {
        _gnss_worker_process_msg(payload->msgId, payload->msg, payload->msgSize);
        if (payload->msg != NULL)
        {
            app_utils_byte_release(payload->msg);
        }
        app_utils_byte_release(payload);
    }
}


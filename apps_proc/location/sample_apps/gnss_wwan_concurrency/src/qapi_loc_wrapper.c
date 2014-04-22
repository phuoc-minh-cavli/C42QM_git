/*****************************************************************************
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *****************************************************************************/
#include "qapi_loc_wrapper.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "app_types.h"
#include "app_thread.h"
#include "app_utils_misc.h"
#include "qapi_timer.h"
#include "qflog_utils.h"
#include "txm_module.h"
#include "qapi_location.h"

// User buffer provided to location QAPI for callbacks
#define USER_BUFFER_SIZE 4096
static uint8_t _UserBuffer[USER_BUFFER_SIZE];

// Callbacks provided to location QAPI
static qapi_Location_Callbacks_t _LocationCallbacks;

// client id obtained from loc init call
static qapi_loc_client_id _ClientId = (qapi_loc_client_id)-1;
static uint32 _SingleshotSessionId = (uint32)-1;
static uint32 _TrackingSessionId = 0;
static uint32 _BestAvailSessionId = 0;
static uint32 _MotionTrackingSessionId = 0;
static uint32* _GeofenceIdArray = NULL;

// Mutex and signals
TX_MUTEX* _MutexResponse = NULL;
TX_EVENT_FLAGS_GROUP* _SignalResponse = NULL;
TX_EVENT_FLAGS_GROUP* _SignalCollectiveResponse = NULL;

// These 2 APIs are for TIMED wait on response callback
static int _wait_for_response_cb()
{
    uint32 timeout = 1000;

    APP_LOGM("Waiting for response callback");
    app_utils_mutex_get(_MutexResponse);
    uint32 ret = app_utils_timedwait_on_signal(
            _SignalResponse, _MutexResponse, MILLIS_TO_TICKS(timeout));
    app_utils_mutex_put(_MutexResponse);

    if (ret != 0)
    {
        APP_LOGE("No response cb for %d millis", timeout);
    }
    return ret == 0? 0:-1;
}

// These 2 APIs are for TIMED wait on response callback
static int _wait_for_collective_response_cb()
{
    uint32 timeout = 1000;

    APP_LOGM("Waiting for collective response callback");
    app_utils_mutex_get(_MutexResponse);
    uint32 ret = app_utils_timedwait_on_signal(
            _SignalCollectiveResponse, _MutexResponse, MILLIS_TO_TICKS(timeout));
    app_utils_mutex_put(_MutexResponse);

    if (ret != 0)
    {
        APP_LOGE("No response cb for %d millis", timeout);
    }
    return ret == 0? 0:-1;
}

/*--------------------------------------------------------------------------
 * Must be invoked before any other API in this file
 *-------------------------------------------------------------------------*/
void qapi_loc_wrapper_setup()
{
    app_utils_init_signal(&_SignalResponse, "_SignalResponse");
    app_utils_init_signal(&_SignalCollectiveResponse, "_SignalCollectiveResponse");
    app_utils_mutex_init(&_MutexResponse, "_MutexResponse");
}

/*--------------------------------------------------------------------------
 * LOC INIT - DEINIT APIs
 *-------------------------------------------------------------------------*/
int qapi_loc_wrapper_init()
{
    memset(&_LocationCallbacks, 0, sizeof(qapi_Location_Callbacks_t));
    _LocationCallbacks.size = sizeof(qapi_Location_Callbacks_t);

    _LocationCallbacks.capabilitiesCb        = qapi_loc_wrapper_capabilities_cb;
    _LocationCallbacks.responseCb            = qapi_loc_wrapper_response_cb;
    _LocationCallbacks.collectiveResponseCb  = qapi_loc_wrapper_collective_response_cb;
    _LocationCallbacks.trackingCb            = qapi_loc_wrapper_tracking_cb;
    _LocationCallbacks.batchingCb            = qapi_loc_wrapper_batching_cb;
    _LocationCallbacks.geofenceBreachCb      = qapi_loc_wrapper_geofence_breach_cb;
    _LocationCallbacks.singleShotCb          = qapi_loc_wrapper_single_shot_cb;
    _LocationCallbacks.gnssDataCb            = qapi_loc_wrapper_gnss_data_cb;
    _LocationCallbacks.metaDataCb            = qapi_loc_wrapper_gnss_meta_data_cb;
    _LocationCallbacks.gnssNmeaCb            = qapi_loc_wrapper_gnss_nmea_cb;
    _LocationCallbacks.motionTrackingCb      = qapi_loc_wrapper_motion_tracking_cb;

    APP_LOG_QAPI("qapi_Loc_Init");
    qapi_Location_Error_t ret = qapi_Loc_Init(&_ClientId, &_LocationCallbacks);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS)
    {
        APP_LOGE("LOC INIT returned failure ret %d", ret);
        return -1;
    }

    APP_LOG_QAPI("qapi_Loc_Set_User_Buffer");
    ret = qapi_Loc_Set_User_Buffer(_ClientId, (uint8_t*)_UserBuffer, (size_t)USER_BUFFER_SIZE);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS)
    {
        APP_LOGE("Set user buffer failed ret %d", ret);
        return -1;
    }

    return 0;
}

int qapi_loc_wrapper_deinit(uint32 clientIdx)
{
    APP_LOG_QAPI("qapi_Loc_Deinit");
    qapi_Location_Error_t ret = qapi_Loc_Deinit(_ClientId);

    if (ret == QAPI_LOCATION_ERROR_SUCCESS)
    {
        _ClientId = (qapi_loc_client_id)-1;
    }
    else
    {
        APP_LOGE("LOC DEINIT returned failure ret %d", ret);
        return -1;
    }

    return 0;
}


/*--------------------------------------------------------------------------
 * SINGLESHOT APIs
 *-------------------------------------------------------------------------*/
int qapi_loc_wrapper_get_single_shot(
        uint32 powerLevel, uint32 accuracy, uint32 timeout)
{
    qapi_Singleshot_Options_t singleshotOptions = { 0 };

    singleshotOptions.size = sizeof(qapi_Singleshot_Options_t);
    singleshotOptions.powerLevel = powerLevel;
    singleshotOptions.accuracyLevel = accuracy;
    singleshotOptions.timeout = timeout;

    APP_LOG_QAPI("qapi_Loc_Get_Single_Shot_V2");
    qapi_Location_Error_t ret =
            qapi_Loc_Get_Single_Shot_V2(_ClientId, &singleshotOptions, &_SingleshotSessionId);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS)
    {
        APP_LOGE("Get singleshot returned failure ret %d", ret);
        return -1;
    }

    int waitRet = _wait_for_response_cb();

    return waitRet;
}

int qapi_loc_wrapper_cancel_single_shot()
{
    APP_LOG_QAPI("qapi_Loc_Cancel_Single_Shot");
    qapi_Location_Error_t ret =
            qapi_Loc_Cancel_Single_Shot(_ClientId, _SingleshotSessionId);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS)
    {
        APP_LOGE("QAPI invocation failed, ret %d", ret);
        return -1;
    }

    int waitRet = _wait_for_response_cb();

    return waitRet;
}


/*--------------------------------------------------------------------------
 * TRACKING APIs
 *-------------------------------------------------------------------------*/
int qapi_loc_wrapper_tracking_start(
        uint32 tbf, uint32 dbf, uint32 accuracy)
{
    qapi_Location_Options_t trackingOptions = { 0 };
    trackingOptions.size = sizeof(qapi_Location_Options_t);
    trackingOptions.minInterval = tbf;
    trackingOptions.minDistance = dbf;
    trackingOptions.accuracyLevel = accuracy;

    APP_LOG_QAPI("qapi_Loc_Start_Tracking");
    qapi_Location_Error_t ret = qapi_Loc_Start_Tracking(
            _ClientId, &trackingOptions, &_TrackingSessionId);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS)
    {
        APP_LOGE("QAPI invocation failed, ret %d", ret);
        return -1;
    }

    int waitRet = _wait_for_response_cb();

    return waitRet;
}

int qapi_loc_wrapper_tracking_stop()
{
    APP_LOG_QAPI("qapi_Loc_Stop_Tracking");
    qapi_Location_Error_t ret =
            qapi_Loc_Stop_Tracking(_ClientId, _TrackingSessionId);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS)
    {
        APP_LOGE("QAPI invocation failed, ret %d", ret);
        return -1;
    }

    int waitRet = _wait_for_response_cb();

    return waitRet;
}

/*--------------------------------------------------------------------------
 * META DATA APIs
 *-------------------------------------------------------------------------*/
int qapi_loc_wrapper_get_best_available_position()
{
    APP_LOG_QAPI("qapi_Loc_Get_Best_Available_Position");
    qapi_Location_Error_t ret =
           qapi_Loc_Get_Best_Available_Position(_ClientId, &_BestAvailSessionId);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS)
    {
       APP_LOGE("QAPI invocation failed, ret %d", ret);
       _BestAvailSessionId = (uint32)-1;
       return -1;
    }

    int waitRet = _wait_for_response_cb();

    return waitRet;
}

/*--------------------------------------------------------------------------
 * MOTION APIs
 *-------------------------------------------------------------------------*/
int qapi_loc_wrapper_start_motion_tracking(
        qapi_Location_Motion_Type_Mask_t mask)
{
    qapi_Location_Motion_Options_t motionOptions={0};

    motionOptions.size=sizeof(qapi_Location_Motion_Options_t);
    motionOptions.motionTypeMask = mask;

    APP_LOG_QAPI("qapi_Loc_Start_Motion_Tracking");
    qapi_Location_Error_t ret =
            qapi_Loc_Start_Motion_Tracking(_ClientId, &motionOptions, &_MotionTrackingSessionId);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS)
    {
       APP_LOGE("QAPI invocation failed, ret %d", ret);
       return -1;
    }

    int waitRet = _wait_for_response_cb();

    return waitRet;
}

int qapi_loc_wrapper_stop_motion_tracking()
{
    APP_LOG_QAPI("qapi_Loc_Stop_Motion_Tracking");
    qapi_Location_Error_t ret =
            qapi_Loc_Stop_Motion_Tracking(_ClientId, _MotionTrackingSessionId);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS)
    {
       APP_LOGE("QAPI invocation failed, ret %d", ret);
       return -1;
    }

    int waitRet = _wait_for_response_cb();

    return waitRet;
}

/*--------------------------------------------------------------------------
 * GEOFENCE APIs
 *-------------------------------------------------------------------------*/
int qapi_loc_wrapper_add_geofence(
        double latitude, double longitude, double radius,
        uint16 breachTypeMask, uint32 responsivenessMillis, uint32 dwellTimeMillis)
{
    qapi_Geofence_Option_t options = { 0 };
    qapi_Geofence_Info_t info = {0};
    options.size = sizeof(qapi_Geofence_Option_t);
    info.size = sizeof(qapi_Geofence_Info_t);

    options.breachTypeMask = breachTypeMask;
    options.responsiveness = responsivenessMillis;
    options.dwellTime = dwellTimeMillis;

    info.latitude = latitude;
    info.longitude = longitude;
    info.radius = radius;

    uint32* idArray = NULL;
    APP_LOG_QAPI("qapi_Loc_Add_Geofences");
    qapi_Location_Error_t ret = qapi_Loc_Add_Geofences(
            _ClientId, 1, &options, &info, &_GeofenceIdArray);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS)
    {
        APP_LOGE("QAPI invocation failed, ret %d", ret);
        return -1;
    }

    int waitRet = _wait_for_collective_response_cb();

    return waitRet;
}

int qapi_loc_wrapper_remove_geofence()
{
    APP_LOG_QAPI("qapi_Loc_Remove_Geofences");
    qapi_Location_Error_t ret =  qapi_Loc_Remove_Geofences(
            _ClientId, 1, _GeofenceIdArray);

    if (ret != QAPI_LOCATION_ERROR_SUCCESS)
    {
        APP_LOGE("QAPI invocation failed, ret %d", ret);
        return -1;
    }

    int waitRet = _wait_for_collective_response_cb();

    return waitRet;
}

/*****************************************************************************
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gnss_worker_geofence_sm.h"
#include "gnss_worker_utils.h"

#include "app_utils_misc.h"
#include "qapi_loc_wrapper.h"

#include "qapi_timer.h"

#define MAX_BACKOFF_COUNT           1
#define BACKOFF_DELAY_SEC           60

/***************************************************************************
    INTERNAL STATE DATA
****************************************************************************/
typedef struct gnss_worker_geofence_sm_request_params
{
    double latitude;
    double longitude;
    double radius;
    qapi_Geofence_Breach_Mask_t mask;
    uint32 responsiveness;
    uint32 dwellTime;

    uint32 allowPriorityInversion;

} gnss_worker_geofence_sm_request_params;

typedef struct gnss_worker_geofence_sm_state_data
{
    gnss_worker_state state;

    uint8 isLocInitDone;
    uint8 isBackoffTimerRunning;
    uint8 isAdDlCompleteTimerRunning;

    // Internal Timers
    void* backoffTimerHandle;
    void* adDlCompleteTimerHandle;

    // Mutex for blocking
    TX_MUTEX* mutex;
    TX_EVENT_FLAGS_GROUP* signal;

    // Ongoing request parameters
    gnss_worker_geofence_sm_request_params geofenceRequestParams;

    uint8 backoffCount;

    gnss_worker_ad_dl_state xtraDlState;
    gnss_worker_ad_dl_state ntpDlState;
    gnss_worker_ad_dl_state gtpDlState;

} gnss_worker_geofence_sm_state_data;

// Internal state
static gnss_worker_geofence_sm_state_data _StateData = {};

/***************************************************************************
    Internal Method Declarations
****************************************************************************/
static void _gnss_worker_geofence_sm_request_add_geofence();

static void gnss_worker_geofence_sm_backoff_timer_cb_in_timer_context(uint32 cbData);

// State change methods
// Return 0 if event is handled, -1 otherwise
static int _gnss_worker_geofence_sm_state_idle(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_geofence_sm_state_waiting_for_gnss_load(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_geofence_sm_state_waiting_for_ad_dl_complete(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_geofence_sm_state_ad_dl_complete(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_geofence_sm_state_gnss_loaded(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_geofence_sm_state_backoff_bad_wwan_connection(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_geofence_sm_state_backoff_weak_gnss_signals(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_geofence_sm_state_backoff_wwan_high_priority(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_geofence_sm_state_gnss_retry(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_geofence_sm_state_gnss_fix_generated(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);

// Returns 0 if event is handled by some state, or -1 otherwise
static int _gnss_worker_geofence_sm_report_event(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);

static void _gnss_worker_geofence_sm_start_backoff_timer(uint32 durationSec);
static void _gnss_worker_geofence_sm_cancel_backoff_timer();

static void _gnss_worker_geofence_sm_enter_state(gnss_worker_state state);

/***************************************************************************
    INTERNAL METHODS
****************************************************************************/
static void _gnss_worker_geofence_sm_backoff_timer_cb(uint32 cbData)
{
    // stop the recurring timer
    _gnss_worker_geofence_sm_cancel_backoff_timer();

    // Report backoff expiry to be handled in current backoff state
    _gnss_worker_geofence_sm_report_event(
            GNSS_WORKER_EVENT_BACKOFF_TIMER_EXPIRY, 0);
}

static void _gnss_worker_geofence_sm_start_backoff_timer(uint32 durationSec)
{
    // Create timer if it doesn't exist
    if (_StateData.backoffTimerHandle == NULL)
    {
        app_utils_timer_init(
                &_StateData.backoffTimerHandle,
                gnss_worker_geofence_sm_backoff_timer_cb_in_timer_context,
                (uint32)&_StateData);
    }

    if (_StateData.isBackoffTimerRunning)
    {
        _gnss_worker_geofence_sm_cancel_backoff_timer();
    }

    app_utils_timer_start(_StateData.backoffTimerHandle, durationSec);
    _StateData.isBackoffTimerRunning = 1;
}

static void _gnss_worker_geofence_sm_cancel_backoff_timer()
{
    app_utils_timer_stop(_StateData.backoffTimerHandle);
    _StateData.isBackoffTimerRunning = 0;
}

void _gnss_worker_geofence_sm_update_dl_ongoing(qapi_Location_Meta_Data_Engine_Status_t status)
{
    switch (status)
    {
    case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_START:
        _StateData.xtraDlState = GNSS_WORKER_AD_DL_STATE_ONGOING;
        break;
    case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_SUCCESS:
        _StateData.xtraDlState = GNSS_WORKER_AD_DL_STATE_SUCCESS;
        break;
    case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_FAILED:
        _StateData.xtraDlState = GNSS_WORKER_AD_DL_STATE_FAILED;
        break;

    case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_START:
        _StateData.ntpDlState = GNSS_WORKER_AD_DL_STATE_ONGOING;
        break;
    case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_SUCCESS:
        _StateData.ntpDlState = GNSS_WORKER_AD_DL_STATE_SUCCESS;
        break;
    case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_FAILED:
        _StateData.ntpDlState = GNSS_WORKER_AD_DL_STATE_FAILED;
        break;

    case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_START:
        _StateData.gtpDlState = GNSS_WORKER_AD_DL_STATE_ONGOING;
        break;
    case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_SUCCESS:
        _StateData.gtpDlState = GNSS_WORKER_AD_DL_STATE_SUCCESS;
        break;
    case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_FAILED:
        _StateData.gtpDlState = GNSS_WORKER_AD_DL_STATE_FAILED;
        break;

    default:
        break;
    }
}

uint8 _gnss_worker_geofence_sm_is_any_ad_dl_ongoing()
{
    return (_StateData.xtraDlState == GNSS_WORKER_AD_DL_STATE_ONGOING ||
            _StateData.ntpDlState == GNSS_WORKER_AD_DL_STATE_ONGOING ||
            _StateData.gtpDlState == GNSS_WORKER_AD_DL_STATE_ONGOING) ? 1 : 0;
}
uint8 _gnss_worker_geofence_sm_is_any_ad_dl_failed()
{
    return (_StateData.xtraDlState == GNSS_WORKER_AD_DL_STATE_FAILED ||
            _StateData.ntpDlState == GNSS_WORKER_AD_DL_STATE_FAILED ||
            _StateData.gtpDlState == GNSS_WORKER_AD_DL_STATE_FAILED) ? 1 : 0;
}
uint8 _gnss_worker_geofence_sm_is_xtra_or_ntp_dl_ongoing()
{
    return (_StateData.xtraDlState == GNSS_WORKER_AD_DL_STATE_ONGOING ||
            _StateData.ntpDlState == GNSS_WORKER_AD_DL_STATE_ONGOING) ? 1 : 0;
}
uint8 _gnss_worker_geofence_sm_is_xtra_or_ntp_dl_failed()
{
    return (_StateData.xtraDlState == GNSS_WORKER_AD_DL_STATE_FAILED ||
            _StateData.ntpDlState == GNSS_WORKER_AD_DL_STATE_FAILED) ? 1 : 0;
}

// TRIGGER TRACKING SESSION
static void _gnss_worker_geofence_sm_request_add_geofence()
{
    APP_LOGEVENT_STR("Adding geofence");

    qapi_loc_wrapper_add_geofence(
            _StateData.geofenceRequestParams.latitude,
            _StateData.geofenceRequestParams.longitude,
            _StateData.geofenceRequestParams.radius,
            _StateData.geofenceRequestParams.mask,
            _StateData.geofenceRequestParams.responsiveness,
            _StateData.geofenceRequestParams.dwellTime);

    _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_WAITING_FOR_GNSS_LOAD);
}

// CANCEL SINGLESHOT SESSION
static void _gnss_worker_geofence_sm_request_geofence_remove()
{
    APP_LOGEVENT("Removing geofence");

    qapi_loc_wrapper_remove_geofence();
}

/***************************************************************************
    STATE CHANGE/HANDLING METHODS
****************************************************************************/
static int _gnss_worker_geofence_sm_state_idle(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_IDLE;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        gnss_worker_utils_restore_default_app_priority();
        _StateData.backoffCount = 0;

        ret = 0;
    }
    else if (event == GNSS_WORKER_EVENT_GEOFENCE_ADD_REQUEST)
    {
        APP_LOGEVENT("Adding geofence in IDLE state");
        _gnss_worker_geofence_sm_request_add_geofence();
        ret = 0;
    }

    return ret;
}

static int _gnss_worker_geofence_sm_state_waiting_for_gnss_load(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_WAITING_FOR_GNSS_LOAD;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        ret = 0;
    }

    // Transitions
    if (event == GNSS_WORKER_EVENT_ENGINE_STATUS_UPDATE)
    {
        ret = 0;
        switch(status)
        {
        case QAPI_LOCATION_ENGINE_STATUS_LOAD_FAILED:
            if (gnss_worker_utils_is_gnss_high_priority())
            {
                APP_LOGEVENT("Load failed in GNSS High priority, GNSS Engine blocked for AD DL.");
                _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION);
            }
            else
            {
                _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY);
            }
            break;

        case QAPI_LOCATION_ENGINE_STATUS_LOAD_SUCCESS:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_GNSS_LOADED);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_THROTTLED:
            APP_LOGE("XTRA Throttling not expected in production, please check if Modem XTRA is somehow deleted ?");
            break;

        case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_START:
        case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_START:
        case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_START:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_WAITING_FOR_AD_DL_COMPLETE);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_FIX_SUCCESS:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_GNSS_FIX_GENERATED);
            break;

        default:
            ret = -1;
            break;
        }
    }

    return ret;
}

static int _gnss_worker_geofence_sm_state_waiting_for_ad_dl_complete(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_WAITING_FOR_AD_DL_COMPLETE;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        ret = 0;
    }

    // Transitions
    if (event == GNSS_WORKER_EVENT_ENGINE_STATUS_UPDATE)
    {
        ret = 0;
        switch(status)
        {
        case QAPI_LOCATION_ENGINE_STATUS_LOAD_FAILED:
            if (_gnss_worker_geofence_sm_is_xtra_or_ntp_dl_ongoing() ||
                    _gnss_worker_geofence_sm_is_xtra_or_ntp_dl_failed())
            {
                APP_LOGEVENT("Load failed while AD DL incomplete, GNSS Engine blocked for AD DL.");
                _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION);
            }
            else if (gnss_worker_utils_is_gnss_high_priority())
            {
                APP_LOGEVENT("Load failed in GNSS High priority, GNSS Engine blocked for AD DL.");
                _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION);
            }
            else
            {
                _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY);
            }
            break;

        case QAPI_LOCATION_ENGINE_STATUS_LOAD_SUCCESS:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_GNSS_LOADED);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_DATA_CALL_FAILED:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_FIX_SUCCESS:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_GNSS_FIX_GENERATED);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_THROTTLED:
            APP_LOGE("XTRA Throttling not expected in production, please check if Modem XTRA is somehow deleted ?");
            break;

        case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_SUCCESS:
        case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_SUCCESS:
        case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_SUCCESS:
            if (!_gnss_worker_geofence_sm_is_any_ad_dl_ongoing() &&
                    !_gnss_worker_geofence_sm_is_any_ad_dl_failed())
            {
                _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_AD_DL_COMPLETE);
            }
            break;

        default:
            ret = -1;
            break;
        }
    }

    return ret;
}

static int _gnss_worker_geofence_sm_state_ad_dl_complete(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_AD_DL_COMPLETE;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        ret = 0;
    }

    // Transitions
    if (event == GNSS_WORKER_EVENT_ENGINE_STATUS_UPDATE)
    {
        ret = 0;
        switch(status)
        {
        case QAPI_LOCATION_ENGINE_STATUS_LOAD_FAILED:
            if (gnss_worker_utils_is_gnss_high_priority())
            {
                APP_LOGEVENT("Load failed in GNSS High priority, GNSS Engine blocked for AD DL.");
                _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION);
            }
            else
            {
                _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY);
            }
            break;

        case QAPI_LOCATION_ENGINE_STATUS_LOAD_SUCCESS:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_GNSS_LOADED);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_FIX_SUCCESS:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_GNSS_FIX_GENERATED);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_THROTTLED:
            APP_LOGE("XTRA Throttling not expected in production, please check if Modem XTRA is somehow deleted ?");
            break;

        case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_START:
        case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_START:
        case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_START:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_WAITING_FOR_AD_DL_COMPLETE);
            break;

        default:
            ret = -1;
            break;
        }
    }

    return ret;
}

static int _gnss_worker_geofence_sm_state_gnss_loaded(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_GNSS_LOADED;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        // nothing to do on entry
        ret = 0;
    }

    // Transitions
    if (event == GNSS_WORKER_EVENT_ENGINE_STATUS_UPDATE)
    {
        ret = 0;
        switch(status)
        {
        case QAPI_LOCATION_ENGINE_STATUS_WEAK_GNSS_SIGNALS:
        case QAPI_LOCATION_ENGINE_STATUS_FIX_FAILED:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WEAK_GNSS_SIGNALS);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_UNLOADED:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_FIX_SUCCESS:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_GNSS_FIX_GENERATED);
            break;

        default:
            ret = -1;
            break;
        }
    }

    return ret;
}

static int _gnss_worker_geofence_sm_state_backoff_bad_wwan_connection(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        APP_LOGEVENT("Backoff for %d seconds.", BACKOFF_DELAY_SEC);
        _gnss_worker_geofence_sm_start_backoff_timer(BACKOFF_DELAY_SEC);

        ret = 0;
    }

    if (event == GNSS_WORKER_EVENT_BACKOFF_TIMER_EXPIRY)
    {
        if (_StateData.backoffCount > 0)
        {
            APP_LOGEVENT("Backoff ongoing for bad wwan..");
        }
        _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_GNSS_RETRY);
        ret = 0;
    }

    // Transitions
    if (event == GNSS_WORKER_EVENT_ENGINE_STATUS_UPDATE)
    {
        ret = 0;
        switch(status)
        {
        case QAPI_LOCATION_ENGINE_STATUS_LOAD_SUCCESS:
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_GNSS_LOADED);
            break;

        default:
            ret = -1;
            break;
        }
    }

    return ret;
}

static int _gnss_worker_geofence_sm_state_backoff_weak_gnss_signals(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_BACKOFF_WEAK_GNSS_SIGNALS;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        APP_LOGEVENT("Backoff for %d seconds.", BACKOFF_DELAY_SEC);
        _gnss_worker_geofence_sm_start_backoff_timer(BACKOFF_DELAY_SEC);

        qapi_loc_wrapper_start_motion_tracking(QAPI_LOCATION_MOTION_TYPE_MOVE_ABSOLUTE);
        ret = 0;
    }

    if (event == GNSS_WORKER_EVENT_BACKOFF_TIMER_EXPIRY ||
            event == GNSS_WORKER_EVENT_ABSOLUTE_MOTION)
    {
        _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_GNSS_RETRY);
        qapi_loc_wrapper_stop_motion_tracking();
        ret = 0;
    }

    return ret;
}

static int _gnss_worker_geofence_sm_state_backoff_wwan_high_priority(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        // Check if we can switch priority to gnss
        if (_StateData.geofenceRequestParams.allowPriorityInversion)
        {
            gnss_worker_utils_set_gnss_high_priority();
        }

        _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_GNSS_RETRY);

        ret = 0;
    }

    return ret;
}

static int _gnss_worker_geofence_sm_state_gnss_retry(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_GNSS_RETRY;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY ||
            event == GNSS_WORKER_EVENT_BACKOFF_TIMER_EXPIRY)
    {
        // if max retries done, send to idle state
        if (_StateData.backoffCount >= MAX_BACKOFF_COUNT)
        {
            APP_LOGEVENT("Max retries (%d) done, going back to idle.", MAX_BACKOFF_COUNT);
            _StateData.backoffCount = 0;
            gnss_worker_utils_restore_default_app_priority();
            _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_WAITING_FOR_GNSS_LOAD);
            return 0;
        }
        else
        {
            _StateData.backoffCount++;
            APP_LOGEVENT("Backoff for %d seconds.", BACKOFF_DELAY_SEC);
            _gnss_worker_geofence_sm_start_backoff_timer(BACKOFF_DELAY_SEC);
        }

        ret = 0;
    }

    return ret;
}

static int _gnss_worker_geofence_sm_state_gnss_fix_generated(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_GNSS_FIX_GENERATED;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_WAITING_FOR_GNSS_LOAD);
        ret = 0;
    }

    return ret;
}

/*  ---- STATE HANDLING METHODS COMPLETE ----  */

static int _gnss_worker_geofence_sm_report_event(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    int ret = -1;

    // Let's update AD DL status before event handling
    _gnss_worker_geofence_sm_update_dl_ongoing(status);

    switch(_StateData.state)
    {
    case GNSS_WORKER_STATE_IDLE:
        ret = _gnss_worker_geofence_sm_state_idle(event, status);
        break;

    case GNSS_WORKER_STATE_WAITING_FOR_GNSS_LOAD:
        ret = _gnss_worker_geofence_sm_state_waiting_for_gnss_load(event, status);
        break;

    case GNSS_WORKER_STATE_WAITING_FOR_AD_DL_COMPLETE:
        ret = _gnss_worker_geofence_sm_state_waiting_for_ad_dl_complete(event, status);
        break;

    case GNSS_WORKER_STATE_AD_DL_COMPLETE:
        ret = _gnss_worker_geofence_sm_state_ad_dl_complete(event, status);
        break;

    case GNSS_WORKER_STATE_GNSS_LOADED:
        ret = _gnss_worker_geofence_sm_state_gnss_loaded(event, status);
        break;

    case GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION:
        ret = _gnss_worker_geofence_sm_state_backoff_bad_wwan_connection(event, status);
        break;

    case GNSS_WORKER_STATE_BACKOFF_WEAK_GNSS_SIGNALS:
        ret = _gnss_worker_geofence_sm_state_backoff_weak_gnss_signals(event, status);
        break;

    case GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY:
        ret = _gnss_worker_geofence_sm_state_backoff_wwan_high_priority(event, status);
        break;

    case GNSS_WORKER_STATE_GNSS_RETRY:
        ret = _gnss_worker_geofence_sm_state_gnss_retry(event, status);
        break;

    case GNSS_WORKER_STATE_GNSS_FIX_GENERATED:
        ret = _gnss_worker_geofence_sm_state_gnss_fix_generated(event, status);
        break;

    default:
        break;
    }

    return ret;
}

static void _gnss_worker_geofence_sm_enter_state(gnss_worker_state state)
{
    switch(state)
    {
    case GNSS_WORKER_STATE_IDLE:
        _gnss_worker_geofence_sm_state_idle(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_WAITING_FOR_GNSS_LOAD:
        _gnss_worker_geofence_sm_state_waiting_for_gnss_load(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_WAITING_FOR_AD_DL_COMPLETE:
        _gnss_worker_geofence_sm_state_waiting_for_ad_dl_complete(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_AD_DL_COMPLETE:
        _gnss_worker_geofence_sm_state_ad_dl_complete(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_GNSS_LOADED:
        _gnss_worker_geofence_sm_state_gnss_loaded(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION:
        _gnss_worker_geofence_sm_state_backoff_bad_wwan_connection(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_BACKOFF_WEAK_GNSS_SIGNALS:
        _gnss_worker_geofence_sm_state_backoff_weak_gnss_signals(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY:
        _gnss_worker_geofence_sm_state_backoff_wwan_high_priority(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_GNSS_FIX_GENERATED:
        _gnss_worker_geofence_sm_state_gnss_fix_generated(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_GNSS_RETRY:
        _gnss_worker_geofence_sm_state_gnss_retry(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    default:
        APP_LOGE("Invalid state requested %d", state);
    }
}

/***************************************************************************
    EXTERNAL METHODS
****************************************************************************/
// INIT
void gnss_worker_geofence_sm_init()
{
    if (_StateData.isLocInitDone)
    {
        APP_LOGE("GNSS worker SM init already done.");
        return;
    }

    memset(&_StateData, 0, sizeof(_StateData));

    app_utils_init_signal(&_StateData.signal, "_StateData.signal");
    app_utils_mutex_init(&_StateData.mutex, "_StateData.mutex");

    qapi_loc_wrapper_setup();
    qapi_loc_wrapper_init(0, 255);

    _StateData.isLocInitDone = 1;
    _StateData.state = GNSS_WORKER_STATE_IDLE;
}

// REQUEST HANDLER :: Add Geofence
void gnss_worker_geofence_sm_request_add_geofence(
        double latitude, double longitude, double radius,
        qapi_Geofence_Breach_Mask_t mask, uint32 responsiveness, uint32 dwellTime,
        uint32 allowPriorityInversion)
{
    _StateData.geofenceRequestParams.latitude = latitude;
    _StateData.geofenceRequestParams.longitude = longitude;
    _StateData.geofenceRequestParams.radius = radius;
    _StateData.geofenceRequestParams.mask = mask;
    _StateData.geofenceRequestParams.responsiveness = responsiveness;
    _StateData.geofenceRequestParams.dwellTime = dwellTime;

    _StateData.geofenceRequestParams.allowPriorityInversion = allowPriorityInversion;

    int ret = _gnss_worker_geofence_sm_report_event(GNSS_WORKER_EVENT_GEOFENCE_ADD_REQUEST, 0);
    if (ret != 0)
    {
        APP_LOGEVENT_STR("Ignoring new geofence add in current state [%s], one request already ongoing",
                app_utils_gnss_worker_state_str(_StateData.state));
    }
}

void gnss_worker_geofence_sm_request_remove_geofence()
{
    memset(&_StateData.geofenceRequestParams, 0, sizeof(_StateData.geofenceRequestParams));

    _gnss_worker_geofence_sm_request_geofence_remove();
    _gnss_worker_geofence_sm_enter_state(GNSS_WORKER_STATE_IDLE);
}

/***************************************************************************
    Event handlers called after context switch from GNSS Worker thread
****************************************************************************/
void gnss_worker_geofence_sm_breach_cb(
        qapi_Location_t location, qapi_Geofence_Breach_t breachType, uint64_t timestamp)
{
    _gnss_worker_geofence_sm_report_event(
            GNSS_WORKER_EVENT_GEOFENCE_BREACH_CB, 0);
}

void gnss_worker_geofence_sm_meta_data_cb(qapi_Location_Meta_Data_t metaData)
{
    APP_LOGM("gnss_worker_geofence_sm_meta_data_cb flags %p engineStatus %d",
            metaData.flags, metaData.engineStatus);

    if ((metaData.flags & QAPI_LOCATION_META_DATA_HAS_ENGINE_STATUS) &&
            metaData.engineStatus != 0)
    {
        _gnss_worker_geofence_sm_report_event(
                GNSS_WORKER_EVENT_ENGINE_STATUS_UPDATE, metaData.engineStatus);
    }
}

void gnss_worker_geofence_sm_motion_info_cb(qapi_Location_Motion_Info_t motionInfo)
{
    if (motionInfo.motionType == QAPI_LOCATION_MOTION_TYPE_MOVE_ABSOLUTE)
    {
        _gnss_worker_geofence_sm_report_event(
                GNSS_WORKER_EVENT_ABSOLUTE_MOTION, 0);
    }
}

/***************************************************************************
    Out of context calls - not in GNSS Worker thread context
****************************************************************************/
void gnss_worker_geofence_sm_backoff_timer_cb_in_timer_context(uint32 cbData)
{
    APP_LOGM("Backoff timer expiry");

    gnss_worker_send_msg_process_cb(_gnss_worker_geofence_sm_backoff_timer_cb, cbData);
}

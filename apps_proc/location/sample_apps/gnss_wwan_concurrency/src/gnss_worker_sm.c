/*****************************************************************************
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gnss_worker_sm.h"
#include "gnss_worker_utils.h"

#include "app_utils_misc.h"
#include "qapi_loc_wrapper.h"

#include "qapi_timer.h"
#include "app_cfg.h"

#define BEST_AVAIL_ACC_THRESHOLD_METERS      500
#define SINGLESHOT_SESSION_TIMEOUT_SEC       120
#define MIN_REPORT_DELAY_TIMER_DURATION_SEC  60
#define MAX_BACKOFF_COUNT                    2
#define BACKOFF_DELAY_SEC                    60
#define AD_DL_COMPLETE_WAIT_ENABLED          0
#define AD_DL_COMPLETE_WAIT_SEC              10

// Refresh the position few seconds before report is needed
#define POSITION_REFRESH_INTERVAL_SEC        15

// WWAN sleep retry threshold
#define MIN_WWAN_SLEEP_FOR_GNSS_RETRY_SEC    15

/***************************************************************************
    INTERNAL STATE DATA
****************************************************************************/
typedef struct gnss_worker_sm_singleshot_request_params
{
    qapi_Location_Power_Level_t maxPower;
    qapi_Location_Accuracy_Level_t minAccuracy;
    uint32 allowPriorityInversion;

} gnss_worker_sm_singleshot_request_params;

typedef struct gnss_worker_sm_state_data
{
    gnss_worker_state state;

    uint8 isLocInitDone;
    uint8 isBestAvailRequestPending;
    uint8 isBestAvailSyncRequestPending;
    uint8 isReportDelayTimerRunning;
    uint8 isBackoffTimerRunning;
    uint8 isPositionRefreshTimerRunning;
    uint8 isAdDlCompleteTimerRunning;

    // Fetch best avail pos in sync
    qapi_Location_t bestAvailLocation;

    // Internal Timers
    void* reportDelayTimerHandle;
    void* backoffTimerHandle;
    void* positionRefreshTimerHandle;
    void* adDlCompleteTimerHandle;

    // Mutex for blocking
    TX_MUTEX* gnssWorkerSmMutex;
    TX_EVENT_FLAGS_GROUP* gnssWorkerSmSingleshotSignal;

    // Delayed request parameters
    gnss_worker_sm_singleshot_request_params delayedRequestParams;

    // Incoming request parameters
    gnss_worker_sm_singleshot_request_params newRequestParams;

    // Ongoing request parameters
    gnss_worker_sm_singleshot_request_params ongoingRequestParams;

    // Queued request parameters
    // - to be handled when app moves to IDLE state
    uint8 isRequestQueuedForIdleState;
    gnss_worker_sm_singleshot_request_params queuedRequestParams;

    uint8 backoffCount;

    gnss_worker_ad_dl_state xtraDlState;
    gnss_worker_ad_dl_state ntpDlState;
    gnss_worker_ad_dl_state gtpDlState;

    uint32 wwanSleepDuration;

} gnss_worker_sm_state_data;

// Internal state
static gnss_worker_sm_state_data _StateData = {};

/***************************************************************************
    Internal Method Declarations
****************************************************************************/
static void _gnss_worker_sm_request_singleshot(
        qapi_Location_Power_Level_t maxPower, qapi_Location_Accuracy_Level_t minAccuracy,
        uint32 allowPriorityInversion);
static void _gnss_worker_sm_cancel_singleshot();

static void gnss_worker_sm_report_delay_timer_cb_in_timer_context(uint32 cbData);
static void gnss_worker_sm_backoff_timer_cb_in_timer_context(uint32 cbData);
static void gnss_worker_sm_position_refresh_timer_cb_in_timer_context(uint32 cbData);
static void gnss_worker_sm_ad_dl_complete_timer_cb_in_timer_context(uint32 cbData);

// State change methods
// Return 0 if event is handled, -1 otherwise
static int _gnss_worker_sm_state_idle(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_sm_state_waiting_for_gnss_load(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_sm_state_waiting_for_ad_dl_complete(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_sm_state_ad_dl_complete(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_sm_state_gnss_loaded(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_sm_state_backoff_bad_wwan_connection(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_sm_state_backoff_weak_gnss_signals(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_sm_state_backoff_wwan_high_priority(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_sm_state_gnss_retry(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_sm_state_gnss_fix_generated(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);
static int _gnss_worker_sm_state_waiting_for_non_gnss_position(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);

// Returns 0 if event is handled by some state, or -1 otherwise
static int _gnss_worker_sm_report_event(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status);

static void _gnss_worker_sm_start_report_delay_timer(
        uint32 delaySec, qapi_Location_Power_Level_t maxPower,
        qapi_Location_Accuracy_Level_t minAccuracy, uint32 allowPriorityInversion);
static void _gnss_worker_sm_cancel_report_delay_timer();

static void _gnss_worker_sm_start_backoff_timer(uint32 durationSec);
static void _gnss_worker_sm_cancel_backoff_timer();

static void _gnss_worker_sm_start_position_refresh_timer(uint32 durationSec);
static void _gnss_worker_sm_cancel_position_refresh_timer();

static void _gnss_worker_sm_start_ad_dl_complete_timer(uint32 durationSec);
static void _gnss_worker_sm_cancel_ad_dl_complete_timer();

static void _gnss_worker_sm_enter_state(gnss_worker_state state);

/***************************************************************************
    INTERNAL METHODS
****************************************************************************/
static void _gnss_worker_sm_report_delay_timer_cb(uint32 cbData)
{
    // stop the recurring timer
    _gnss_worker_sm_cancel_report_delay_timer();

    // Report event to be handled if in idle state
    _gnss_worker_sm_report_event(
            GNSS_WORKER_EVENT_REPORT_DELAY_TIMER_EXPIRY, 0);
}

static void _gnss_worker_sm_backoff_timer_cb(uint32 cbData)
{
    // stop the recurring timer
    _gnss_worker_sm_cancel_backoff_timer();

    // Report backoff expiry to be handled in current backoff state
    _gnss_worker_sm_report_event(
            GNSS_WORKER_EVENT_BACKOFF_TIMER_EXPIRY, 0);
}

static void _gnss_worker_sm_position_refresh_timer_cb(uint32 cbData)
{
    // stop the recurring timer
    _gnss_worker_sm_cancel_position_refresh_timer();

    // Report backoff expiry to be handled in current backoff state
    int res = _gnss_worker_sm_report_event(
            GNSS_WORKER_EVENT_POSITION_REFRESH_TIMER_EXPIRY, 0);
}

static void _gnss_worker_sm_ad_dl_complete_timer_cb(uint32 cbData)
{
    // stop the recurring timer
    _gnss_worker_sm_cancel_ad_dl_complete_timer();

    // Report expiry to be handled in current state
    int res = _gnss_worker_sm_report_event(
            GNSS_WORKER_EVENT_AD_DL_COMPLETE_TIMER_EXPIRY, 0);
}

// Internal method to get best avail position synchronously
static int _gnss_worker_sm_get_best_avail_loc_sync()
{
    app_utils_mutex_get(_StateData.gnssWorkerSmMutex);

    _StateData.bestAvailLocation.size = 0;
    _StateData.isBestAvailSyncRequestPending = 1;

    // Send request to qapi layer
    qapi_loc_wrapper_get_best_available_position();

    app_utils_timedwait_on_signal(
            _StateData.gnssWorkerSmSingleshotSignal, _StateData.gnssWorkerSmMutex, MILLIS_TO_TICKS(1000));

    _StateData.isBestAvailSyncRequestPending = 0;
    app_utils_mutex_put(_StateData.gnssWorkerSmMutex);

    return (_StateData.bestAvailLocation.size != 0)? 0: -1;
}

static void _gnss_worker_sm_start_report_delay_timer(
        uint32 delaySec, qapi_Location_Power_Level_t maxPower,
        qapi_Location_Accuracy_Level_t minAccuracy, uint32 allowPriorityInversion)
{
    // Create timer if it doesn't exist
    if (_StateData.reportDelayTimerHandle == NULL)
    {
        app_utils_timer_init(
                &_StateData.reportDelayTimerHandle,
                gnss_worker_sm_report_delay_timer_cb_in_timer_context,
                (uint32)&_StateData);
    }

    APP_LOGEVENT("Delaying singleshot start by %d sec", delaySec);

    _StateData.delayedRequestParams.maxPower = maxPower;
    _StateData.delayedRequestParams.minAccuracy = minAccuracy;
    _StateData.delayedRequestParams.allowPriorityInversion = allowPriorityInversion;

    if (_StateData.isReportDelayTimerRunning)
    {
        _gnss_worker_sm_cancel_report_delay_timer();
    }

    app_utils_timer_start(_StateData.reportDelayTimerHandle, delaySec);
    _StateData.isReportDelayTimerRunning = 1;
}

static void _gnss_worker_sm_cancel_report_delay_timer()
{
    app_utils_timer_stop(_StateData.reportDelayTimerHandle);
    _StateData.isReportDelayTimerRunning = 0;
}

static void _gnss_worker_sm_start_backoff_timer(uint32 durationSec)
{
    // Create timer if it doesn't exist
    if (_StateData.backoffTimerHandle == NULL)
    {
        app_utils_timer_init(
                &_StateData.backoffTimerHandle,
                gnss_worker_sm_backoff_timer_cb_in_timer_context,
                (uint32)&_StateData);
    }

    if (_StateData.isBackoffTimerRunning)
    {
        _gnss_worker_sm_cancel_backoff_timer();
    }

    app_utils_timer_start(_StateData.backoffTimerHandle, durationSec);
    _StateData.isBackoffTimerRunning = 1;
}

static void _gnss_worker_sm_cancel_backoff_timer()
{
    app_utils_timer_stop(_StateData.backoffTimerHandle);
    _StateData.isBackoffTimerRunning = 0;
}

static void _gnss_worker_sm_start_position_refresh_timer(uint32 durationSec)
{
    // Create timer if it doesn't exist
    if (_StateData.positionRefreshTimerHandle == NULL)
    {
        app_utils_timer_init(
                &_StateData.positionRefreshTimerHandle,
                gnss_worker_sm_position_refresh_timer_cb_in_timer_context,
                (uint32)&_StateData);
    }

    if (_StateData.isPositionRefreshTimerRunning)
    {
        _gnss_worker_sm_cancel_position_refresh_timer();
    }

    APP_LOGH("Starting refresh timer..");
    app_utils_timer_start(_StateData.positionRefreshTimerHandle, durationSec);
    _StateData.isPositionRefreshTimerRunning = 1;
}

static void _gnss_worker_sm_cancel_position_refresh_timer()
{
    APP_LOGH("Canceling refresh timer..");
    app_utils_timer_stop(_StateData.positionRefreshTimerHandle);
    _StateData.isPositionRefreshTimerRunning = 0;
}

static void _gnss_worker_sm_start_ad_dl_complete_timer(uint32 durationSec)
{
    if (!AD_DL_COMPLETE_WAIT_ENABLED)
    {
        APP_LOGE("AD DL Complete Wait logic is disabled, can't start timer.");
        return;
    }

    // Create timer if it doesn't exist
    if (_StateData.adDlCompleteTimerHandle == NULL)
    {
        app_utils_timer_init(
                &_StateData.adDlCompleteTimerHandle,
                gnss_worker_sm_ad_dl_complete_timer_cb_in_timer_context,
                (uint32)&_StateData);
    }

    if (_StateData.isAdDlCompleteTimerRunning)
    {
        _gnss_worker_sm_cancel_ad_dl_complete_timer();
    }

    app_utils_timer_start(_StateData.adDlCompleteTimerHandle, durationSec);
    _StateData.isAdDlCompleteTimerRunning = 1;
}

static void _gnss_worker_sm_cancel_ad_dl_complete_timer()
{
    app_utils_timer_stop(_StateData.adDlCompleteTimerHandle);
    _StateData.isAdDlCompleteTimerRunning = 0;
}

void _gnss_worker_sm_update_dl_ongoing(qapi_Location_Meta_Data_Engine_Status_t status)
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

uint8 _gnss_worker_sm_is_any_ad_dl_ongoing()
{
    return (_StateData.xtraDlState == GNSS_WORKER_AD_DL_STATE_ONGOING ||
            _StateData.ntpDlState == GNSS_WORKER_AD_DL_STATE_ONGOING ||
            _StateData.gtpDlState == GNSS_WORKER_AD_DL_STATE_ONGOING) ? 1 : 0;
}
uint8 _gnss_worker_sm_is_any_ad_dl_failed()
{
    return (_StateData.xtraDlState == GNSS_WORKER_AD_DL_STATE_FAILED ||
            _StateData.ntpDlState == GNSS_WORKER_AD_DL_STATE_FAILED ||
            _StateData.gtpDlState == GNSS_WORKER_AD_DL_STATE_FAILED) ? 1 : 0;
}
uint8 _gnss_worker_sm_is_xtra_or_ntp_dl_ongoing()
{
    return (_StateData.xtraDlState == GNSS_WORKER_AD_DL_STATE_ONGOING ||
            _StateData.ntpDlState == GNSS_WORKER_AD_DL_STATE_ONGOING) ? 1 : 0;
}
uint8 _gnss_worker_sm_is_xtra_or_ntp_dl_failed()
{
    return (_StateData.xtraDlState == GNSS_WORKER_AD_DL_STATE_FAILED ||
            _StateData.ntpDlState == GNSS_WORKER_AD_DL_STATE_FAILED) ? 1 : 0;
}

// TRIGGER SINGLESHOT SESSION
static void _gnss_worker_sm_request_singleshot(
        qapi_Location_Power_Level_t maxPower, qapi_Location_Accuracy_Level_t minAccuracy,
        uint32 allowPriorityInversion)
{
    APP_LOGEVENT_STR("Triggering Singleshot request with [%s] [%s] [timeout %d sec]",
            app_utils_power_level_str(maxPower), app_utils_accuracy_level_str(minAccuracy),
            SINGLESHOT_SESSION_TIMEOUT_SEC);

    _StateData.ongoingRequestParams.maxPower = maxPower;
    _StateData.ongoingRequestParams.minAccuracy = minAccuracy;
    _StateData.ongoingRequestParams.allowPriorityInversion = allowPriorityInversion;

    qapi_loc_wrapper_get_single_shot(maxPower, minAccuracy, SINGLESHOT_SESSION_TIMEOUT_SEC*1000);

    if (maxPower == QAPI_LOCATION_POWER_HIGH)
    {
        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_WAITING_FOR_GNSS_LOAD);
    }
    else
    {
        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_WAITING_FOR_NON_GNSS_POSITION);
    }
}

// CANCEL SINGLESHOT SESSION
static void _gnss_worker_sm_cancel_singleshot()
{
    APP_LOGEVENT("Canceling Singleshot Session");

    qapi_loc_wrapper_cancel_single_shot();
}

/***************************************************************************
    STATE CHANGE/HANDLING METHODS
****************************************************************************/
static int _gnss_worker_sm_state_idle(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_IDLE;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        gnss_worker_utils_restore_default_app_priority();
        _StateData.backoffCount = 0;

        // handle any queued request
        if (_StateData.isRequestQueuedForIdleState)
        {
            APP_LOGEVENT("Trigger queued request in IDLE state");
            _StateData.isRequestQueuedForIdleState = 0;
            _gnss_worker_sm_request_singleshot(
                    _StateData.queuedRequestParams.maxPower,
                    _StateData.queuedRequestParams.minAccuracy,
                    _StateData.queuedRequestParams.allowPriorityInversion);
        }

        ret = 0;
    }
    else if (event == GNSS_WORKER_EVENT_SINGLESHOT_REQUEST)
    {
        APP_LOGEVENT("Trigger new request in IDLE state");
        _gnss_worker_sm_request_singleshot(
                _StateData.newRequestParams.maxPower,
                _StateData.newRequestParams.minAccuracy,
                _StateData.newRequestParams.allowPriorityInversion);
        ret = 0;
    }
    else if (event == GNSS_WORKER_EVENT_REPORT_DELAY_TIMER_EXPIRY)
    {
        APP_LOGEVENT("Trigger delayed report request in IDLE state");
        _gnss_worker_sm_request_singleshot(
                _StateData.delayedRequestParams.maxPower,
                _StateData.delayedRequestParams.minAccuracy,
                _StateData.delayedRequestParams.allowPriorityInversion);
        ret = 0;
    }
    else if (event == GNSS_WORKER_EVENT_POSITION_REFRESH_TIMER_EXPIRY)
    {
        APP_LOGEVENT("Trigger position refresh request in IDLE state");
        _gnss_worker_sm_request_singleshot(
                _StateData.newRequestParams.maxPower,
                _StateData.newRequestParams.minAccuracy,
                _StateData.newRequestParams.allowPriorityInversion);
        ret = 0;
    }


    return ret;
}

static int _gnss_worker_sm_state_waiting_for_gnss_load(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_WAITING_FOR_GNSS_LOAD;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        // if we don't see a load event soon, means wwan is high priority
        if (AD_DL_COMPLETE_WAIT_ENABLED)
        {
            _gnss_worker_sm_start_ad_dl_complete_timer(AD_DL_COMPLETE_WAIT_SEC);
        }

        ret = 0;
    }

    // Transitions
    if (event == GNSS_WORKER_EVENT_ENGINE_STATUS_UPDATE)
    {
        ret = 0;
        switch(status)
        {
        case QAPI_LOCATION_ENGINE_STATUS_LOAD_FAILED:
        case QAPI_LOCATION_ENGINE_STATUS_UNLOADED:
            if (gnss_worker_utils_is_gnss_high_priority())
            {
                APP_LOGEVENT("Load failed in GNSS High priority, GNSS Engine blocked for AD DL.");
                _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION);
            }
            else
            {
                _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY);
            }
            break;

        case QAPI_LOCATION_ENGINE_STATUS_LOAD_SUCCESS:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_LOADED);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_THROTTLED:
            APP_LOGE("XTRA Throttling not expected in production, please check if Modem XTRA is somehow deleted ?");
            break;

        case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_START:
        case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_START:
        case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_START:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_WAITING_FOR_AD_DL_COMPLETE);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_FIX_SUCCESS:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_FIX_GENERATED);
            break;

        default:
            ret = -1;
            break;
        }
    }

    if (event == GNSS_WORKER_EVENT_AD_DL_COMPLETE_TIMER_EXPIRY)
    {
        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY);
        ret = 0;
    }

    return ret;
}

static int _gnss_worker_sm_state_waiting_for_ad_dl_complete(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_WAITING_FOR_AD_DL_COMPLETE;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        _gnss_worker_sm_cancel_ad_dl_complete_timer();
        ret = 0;
    }

    // Transitions
    if (event == GNSS_WORKER_EVENT_ENGINE_STATUS_UPDATE)
    {
        ret = 0;
        switch(status)
        {
        case QAPI_LOCATION_ENGINE_STATUS_LOAD_FAILED:
        case QAPI_LOCATION_ENGINE_STATUS_UNLOADED:
            if (_gnss_worker_sm_is_xtra_or_ntp_dl_ongoing() ||
                    _gnss_worker_sm_is_xtra_or_ntp_dl_failed())
            {
                APP_LOGEVENT("Load failed while AD DL incomplete, GNSS Engine blocked for AD DL.");
                _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION);
            }
            else if (gnss_worker_utils_is_gnss_high_priority())
            {
                APP_LOGEVENT("Load failed in GNSS High priority, GNSS Engine blocked for AD DL.");
                _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION);
            }
            else
            {
                _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY);
            }
            break;

        case QAPI_LOCATION_ENGINE_STATUS_LOAD_SUCCESS:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_LOADED);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_DATA_CALL_FAILED:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_FIX_SUCCESS:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_FIX_GENERATED);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_THROTTLED:
            APP_LOGE("XTRA Throttling not expected in production, please check if Modem XTRA is somehow deleted ?");
            break;

        case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_SUCCESS:
        case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_SUCCESS:
        case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_SUCCESS:
            if (!_gnss_worker_sm_is_any_ad_dl_ongoing() &&
                    !_gnss_worker_sm_is_any_ad_dl_failed())
            {
                _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_AD_DL_COMPLETE);
            }
            break;

        default:
            ret = -1;
            break;
        }
    }

    if (event == GNSS_WORKER_EVENT_SINGLESHOT_TIMEOUT)
    {
        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION);
        ret = 0;
    }

    return ret;
}

static int _gnss_worker_sm_state_ad_dl_complete(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_AD_DL_COMPLETE;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        ret = 0;

        // if we don't see a load event soon, means wwan is high priority
        if (AD_DL_COMPLETE_WAIT_ENABLED)
        {
            _gnss_worker_sm_start_ad_dl_complete_timer(AD_DL_COMPLETE_WAIT_SEC);
        }
    }

    // Transitions
    if (event == GNSS_WORKER_EVENT_ENGINE_STATUS_UPDATE)
    {
        ret = 0;
        switch(status)
        {
        case QAPI_LOCATION_ENGINE_STATUS_LOAD_FAILED:
        case QAPI_LOCATION_ENGINE_STATUS_UNLOADED:
            if (gnss_worker_utils_is_gnss_high_priority())
            {
                APP_LOGEVENT("Load failed in GNSS High priority, GNSS Engine blocked for AD DL.");
                _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION);
            }
            else
            {
                _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY);
            }
            break;

        case QAPI_LOCATION_ENGINE_STATUS_LOAD_SUCCESS:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_LOADED);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_FIX_SUCCESS:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_FIX_GENERATED);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_THROTTLED:
            APP_LOGE("XTRA Throttling not expected in production, please check if Modem XTRA is somehow deleted ?");
            break;

        case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_START:
        case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_START:
        case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_START:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_WAITING_FOR_AD_DL_COMPLETE);
            break;

        default:
            ret = -1;
            break;
        }
    }

    if (event == GNSS_WORKER_EVENT_SINGLESHOT_TIMEOUT)
    {
        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY);
        ret = 0;
    }

    if (event == GNSS_WORKER_EVENT_AD_DL_COMPLETE_TIMER_EXPIRY)
    {
        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY);
        ret = 0;
    }

    return ret;
}

static int _gnss_worker_sm_state_gnss_loaded(
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
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WEAK_GNSS_SIGNALS);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_UNLOADED:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_FIX_SUCCESS:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_FIX_GENERATED);
            break;

        default:
            ret = -1;
            break;
        }
    }

    if (event == GNSS_WORKER_EVENT_SINGLESHOT_TIMEOUT)
    {
        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_BACKOFF_WEAK_GNSS_SIGNALS);
        ret = 0;
    }

    return ret;
}

static int _gnss_worker_sm_state_backoff_bad_wwan_connection(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        APP_LOGEVENT("Backoff for %d seconds.", BACKOFF_DELAY_SEC);
        _gnss_worker_sm_start_backoff_timer(BACKOFF_DELAY_SEC);
        ret = 0;
    }

    if (event == GNSS_WORKER_EVENT_BACKOFF_TIMER_EXPIRY)
    {
        if (_StateData.backoffCount > 0)
        {
            APP_LOGEVENT("Backoff ongoing for bad wwan..");
        }
        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_RETRY);
        ret = 0;
    }

    // Transitions
    if (event == GNSS_WORKER_EVENT_ENGINE_STATUS_UPDATE)
    {
        ret = 0;
        switch(status)
        {
        case QAPI_LOCATION_ENGINE_STATUS_LOAD_SUCCESS:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_LOADED);
            break;

        default:
            ret = -1;
            break;
        }
    }

    return ret;
}

static int _gnss_worker_sm_state_backoff_weak_gnss_signals(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_BACKOFF_WEAK_GNSS_SIGNALS;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        APP_LOGEVENT("Backoff for %d seconds.", BACKOFF_DELAY_SEC);
        _gnss_worker_sm_start_backoff_timer(BACKOFF_DELAY_SEC);
        qapi_loc_wrapper_start_motion_tracking(QAPI_LOCATION_MOTION_TYPE_MOVE_ABSOLUTE);
        ret = 0;
    }

    if (event == GNSS_WORKER_EVENT_BACKOFF_TIMER_EXPIRY ||
            event == GNSS_WORKER_EVENT_ABSOLUTE_MOTION)
    {
        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_RETRY);
        qapi_loc_wrapper_stop_motion_tracking();
        ret = 0;
    }

    return ret;
}

static int _gnss_worker_sm_state_backoff_wwan_high_priority(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        APP_LOGEVENT("Backoff for %d seconds.", BACKOFF_DELAY_SEC);
        _gnss_worker_sm_start_backoff_timer(BACKOFF_DELAY_SEC);
        ret = 0;
    }

    if (event == GNSS_WORKER_EVENT_BACKOFF_TIMER_EXPIRY)
    {
        // Check if we can switch priority to gnss
        if (_StateData.ongoingRequestParams.allowPriorityInversion)
        {
            gnss_worker_utils_set_gnss_high_priority();
        }

        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_RETRY);

        ret = 0;
    }

    if (event == GNSS_WORKER_EVENT_WWAN_SLEEP_CB &&
            _StateData.wwanSleepDuration >= MIN_WWAN_SLEEP_FOR_GNSS_RETRY_SEC)
    {
        APP_LOGEVENT("WWAN Sleep CB (%d sec) in Backoff WWAN High Priority state, retry right away.",
                _StateData.wwanSleepDuration);
        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_RETRY);
    }

    return ret;
}

static int _gnss_worker_sm_state_gnss_retry(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_GNSS_RETRY;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        // if max retries done, try lower power mode or send to idle state
        if (_StateData.backoffCount > MAX_BACKOFF_COUNT)
        {
            if (_StateData.ongoingRequestParams.maxPower == QAPI_LOCATION_POWER_HIGH)
            {
                APP_LOGEVENT("Fallback from High power mode to Medium power mode");
                _StateData.ongoingRequestParams.maxPower = QAPI_LOCATION_POWER_MED;
            }
            else if (_StateData.ongoingRequestParams.maxPower == QAPI_LOCATION_POWER_MED)
            {
                APP_LOGEVENT("Fallback from Medium power mode to Low power mode");
                _StateData.ongoingRequestParams.maxPower = QAPI_LOCATION_POWER_LOW;
            }
            else
            {
                APP_LOGEVENT("Max retries (%d) done, going back to idle.", MAX_BACKOFF_COUNT);
                _gnss_worker_sm_cancel_singleshot();
                _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_IDLE);
                return 0;
            }
        }
        else
        {
            _StateData.backoffCount++;
        }

        // Cancel if session is running and restart
        _gnss_worker_sm_cancel_singleshot();
        _gnss_worker_sm_request_singleshot(
                _StateData.ongoingRequestParams.maxPower,
                _StateData.ongoingRequestParams.minAccuracy,
                _StateData.ongoingRequestParams.allowPriorityInversion);

        ret = 0;
    }

    return ret;
}

static int _gnss_worker_sm_state_waiting_for_non_gnss_position(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_WAITING_FOR_NON_GNSS_POSITION;
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
        case QAPI_LOCATION_ENGINE_STATUS_DATA_CALL_FAILED:
        case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_FAILED:
        case QAPI_LOCATION_ENGINE_STATUS_WIFI_POS_FAILED:
        case QAPI_LOCATION_ENGINE_STATUS_POS_ACC_CHECK_FAILED:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_RETRY);
            break;

        case QAPI_LOCATION_ENGINE_STATUS_FIX_SUCCESS:
            _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_FIX_GENERATED);
            break;

        default:
            ret = -1;
            break;
        }
    }

    if (event == GNSS_WORKER_EVENT_SINGLESHOT_TIMEOUT)
    {
        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_GNSS_RETRY);
        ret = 0;
    }

    return ret;
}

static int _gnss_worker_sm_state_gnss_fix_generated(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    _StateData.state = GNSS_WORKER_STATE_GNSS_FIX_GENERATED;
    gnss_worker_utils_log_state_event(_StateData.state, event, status);

    int ret = -1;

    if (event == GNSS_WORKER_EVENT_STATE_ENTRY)
    {
        _gnss_worker_sm_enter_state(GNSS_WORKER_STATE_IDLE);
        ret = 0;
    }

    return ret;
}

/*  ---- STATE HANDLING METHODS COMPLETE ----  */

static int _gnss_worker_sm_report_event(
        gnss_worker_event event, qapi_Location_Meta_Data_Engine_Status_t status)
{
    int ret = -1;

    // Let's update AD DL status before event handling
    _gnss_worker_sm_update_dl_ongoing(status);

    switch(_StateData.state)
    {
    case GNSS_WORKER_STATE_IDLE:
        ret = _gnss_worker_sm_state_idle(event, status);
        break;

    case GNSS_WORKER_STATE_WAITING_FOR_GNSS_LOAD:
        ret = _gnss_worker_sm_state_waiting_for_gnss_load(event, status);
        break;

    case GNSS_WORKER_STATE_WAITING_FOR_AD_DL_COMPLETE:
        ret = _gnss_worker_sm_state_waiting_for_ad_dl_complete(event, status);
        break;

    case GNSS_WORKER_STATE_AD_DL_COMPLETE:
        ret = _gnss_worker_sm_state_ad_dl_complete(event, status);
        break;

    case GNSS_WORKER_STATE_GNSS_LOADED:
        ret = _gnss_worker_sm_state_gnss_loaded(event, status);
        break;

    case GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION:
        ret = _gnss_worker_sm_state_backoff_bad_wwan_connection(event, status);
        break;

    case GNSS_WORKER_STATE_BACKOFF_WEAK_GNSS_SIGNALS:
        ret = _gnss_worker_sm_state_backoff_weak_gnss_signals(event, status);
        break;

    case GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY:
        ret = _gnss_worker_sm_state_backoff_wwan_high_priority(event, status);
        break;

    case GNSS_WORKER_STATE_GNSS_RETRY:
        ret = _gnss_worker_sm_state_gnss_retry(event, status);
        break;

    case GNSS_WORKER_STATE_WAITING_FOR_NON_GNSS_POSITION:
        ret = _gnss_worker_sm_state_waiting_for_non_gnss_position(event, status);
        break;

    case GNSS_WORKER_STATE_GNSS_FIX_GENERATED:
        ret = _gnss_worker_sm_state_gnss_fix_generated(event, status);
        break;

    default:
        break;
    }

    return ret;
}

static void _gnss_worker_sm_enter_state(gnss_worker_state state)
{
    switch(state)
    {
    case GNSS_WORKER_STATE_IDLE:
        _gnss_worker_sm_state_idle(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_WAITING_FOR_GNSS_LOAD:
        _gnss_worker_sm_state_waiting_for_gnss_load(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_WAITING_FOR_AD_DL_COMPLETE:
        _gnss_worker_sm_state_waiting_for_ad_dl_complete(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_AD_DL_COMPLETE:
        _gnss_worker_sm_state_ad_dl_complete(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_GNSS_LOADED:
        _gnss_worker_sm_state_gnss_loaded(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_BACKOFF_BAD_WWAN_CONNECTION:
        _gnss_worker_sm_state_backoff_bad_wwan_connection(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_BACKOFF_WEAK_GNSS_SIGNALS:
        _gnss_worker_sm_state_backoff_weak_gnss_signals(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_BACKOFF_WWAN_HIGH_PRIORITY:
        _gnss_worker_sm_state_backoff_wwan_high_priority(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_GNSS_FIX_GENERATED:
        _gnss_worker_sm_state_gnss_fix_generated(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_WAITING_FOR_NON_GNSS_POSITION:
        _gnss_worker_sm_state_waiting_for_non_gnss_position(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    case GNSS_WORKER_STATE_GNSS_RETRY:
        _gnss_worker_sm_state_gnss_retry(GNSS_WORKER_EVENT_STATE_ENTRY, 0);
        break;
    default:
        APP_LOGE("Invalid state requested %d", state);
    }
}

/***************************************************************************
    EXTERNAL METHODS
****************************************************************************/
// INIT
void gnss_worker_sm_init()
{
    if (_StateData.isLocInitDone)
    {
        APP_LOGE("GNSS worker SM init already done.");
        return;
    }

    memset(&_StateData, 0, sizeof(_StateData));

    app_utils_init_signal(&_StateData.gnssWorkerSmSingleshotSignal, "_StateData.gnssWorkerSmSingleshotSignal");
    app_utils_mutex_init(&_StateData.gnssWorkerSmMutex, "_StateData.gnssWorkerSmMutex");

    qapi_loc_wrapper_setup();
    qapi_loc_wrapper_init(0, 255);

    if (APP_CONFIG.WWAN_SLEEP_EVENT_ENABLED)
    {
        gnss_worker_utils_set_wwan_sleep_callback();
    }

    _StateData.isLocInitDone = 1;
    _StateData.state = GNSS_WORKER_STATE_IDLE;
}

// REQUEST HANDLER :: Best Available Location
void gnss_worker_sm_request_best_avail_location()
{
    _StateData.isBestAvailRequestPending = 1;
    qapi_loc_wrapper_get_best_available_position();
}

void gnss_worker_sm_request_delayed_location(
        uint32 reportDelaySec, qapi_Location_Power_Level_t maxPower,
        qapi_Location_Accuracy_Level_t minAccuracy, uint32 allowPriorityInversion)
{
    // If there is a previous request running delay timer, cancel it
    if (_StateData.isReportDelayTimerRunning)
    {
        _gnss_worker_sm_cancel_report_delay_timer();
    }

    // Cancel running refresh timer
    if (_StateData.isPositionRefreshTimerRunning)
    {
        _gnss_worker_sm_cancel_position_refresh_timer();
    }

    // Fetch the best available position
    _gnss_worker_sm_get_best_avail_loc_sync();

    // Save incoming parameters
    _StateData.newRequestParams.maxPower = maxPower;
    _StateData.newRequestParams.minAccuracy = minAccuracy;
    _StateData.newRequestParams.allowPriorityInversion = allowPriorityInversion;

    // Whether to start session right away
    if (_StateData.bestAvailLocation.size == 0 ||
            _StateData.bestAvailLocation.accuracy > BEST_AVAIL_ACC_THRESHOLD_METERS ||
            reportDelaySec < (SINGLESHOT_SESSION_TIMEOUT_SEC + MIN_REPORT_DELAY_TIMER_DURATION_SEC))
    {
        int ret = _gnss_worker_sm_report_event(GNSS_WORKER_EVENT_SINGLESHOT_REQUEST, 0);
        if (ret != 0)
        {
            if (_StateData.isRequestQueuedForIdleState)
            {
                APP_LOGEVENT_STR("Ignoring new location request in current state [%s], one request already queued",
                        app_utils_gnss_worker_state_str(_StateData.state));
            }
            else
            {
                _StateData.queuedRequestParams = _StateData.newRequestParams;
                _StateData.isRequestQueuedForIdleState = 1;
                APP_LOGEVENT_STR("Queuing new location request (for idle state), not handled in current state [%s]",
                        app_utils_gnss_worker_state_str(_StateData.state));
            }
        }
    }
    else
    {
        _gnss_worker_sm_start_report_delay_timer(
                (reportDelaySec - (SINGLESHOT_SESSION_TIMEOUT_SEC + MIN_REPORT_DELAY_TIMER_DURATION_SEC)),
                maxPower, minAccuracy, allowPriorityInversion);
    }

    // Start position refresh timer aligned with the report delay
    if (reportDelaySec > MIN_REPORT_DELAY_TIMER_DURATION_SEC &&
            reportDelaySec > POSITION_REFRESH_INTERVAL_SEC)
    {
        _gnss_worker_sm_start_position_refresh_timer(
                reportDelaySec - POSITION_REFRESH_INTERVAL_SEC);
    }
}

/***************************************************************************
    Event handlers called after context switch from GNSS Worker thread
****************************************************************************/
void gnss_worker_sm_singleshot_cb(
        qapi_Location_t location, qapi_Location_Error_t err)
{
    if (QAPI_LOCATION_ERROR_SUCCESS == err)
    {
        if (_StateData.isBestAvailRequestPending)
        {
            gnss_worker_report_best_avail_location(location);
            _StateData.isBestAvailRequestPending = 0;
        }
        else
        {
            gnss_worker_report_singleshot_location(location);
        }
    }

    if (QAPI_LOCATION_ERROR_TIMEOUT == err ||
            QAPI_LOCATION_ERROR_TIMEOUT_ACC_CHECK_FAILED == err)
    {
        _gnss_worker_sm_report_event(
                GNSS_WORKER_EVENT_SINGLESHOT_TIMEOUT, 0);
    }
}

void gnss_worker_sm_meta_data_cb(qapi_Location_Meta_Data_t metaData)
{
    APP_LOGM("gnss_worker_sm_meta_data_cb flags %p engineStatus %d",
            metaData.flags, metaData.engineStatus);

    if ((metaData.flags & QAPI_LOCATION_META_DATA_HAS_ENGINE_STATUS) &&
            metaData.engineStatus != 0)
    {
        _gnss_worker_sm_report_event(
                GNSS_WORKER_EVENT_ENGINE_STATUS_UPDATE, metaData.engineStatus);
    }
}

void gnss_worker_sm_motion_info_cb(qapi_Location_Motion_Info_t motionInfo)
{
    if (motionInfo.motionType == QAPI_LOCATION_MOTION_TYPE_MOVE_ABSOLUTE)
    {
        _gnss_worker_sm_report_event(
                GNSS_WORKER_EVENT_ABSOLUTE_MOTION, 0);
    }
}

void gnss_worker_sm_wwan_sleep_cb(uint32 sleepDuration)
{
    _StateData.wwanSleepDuration = sleepDuration;
    _gnss_worker_sm_report_event(
            GNSS_WORKER_EVENT_WWAN_SLEEP_CB, 0);
}

/***************************************************************************
    Out of context calls - not in GNSS Worker thread context
****************************************************************************/
// This one only used for synchronized waiting for best avail position
// This one gets invoked in location callback context
void gnss_worker_sm_singleshot_cb_in_loc_context(
        qapi_Location_t location, qapi_Location_Error_t err)
{
    app_utils_mutex_get(_StateData.gnssWorkerSmMutex);

    // In case this is meant to be sent back for get best avail request
    if (QAPI_LOCATION_ERROR_SUCCESS == err && _StateData.isBestAvailSyncRequestPending)
    {
         memcpy(&_StateData.bestAvailLocation, &location, sizeof(qapi_Location_t));
    }

    // Set the singleshot callback signal
    // Passing NULL mutex since we are doing mutex get/put here directly
    app_utils_set_signal(
            _StateData.gnssWorkerSmSingleshotSignal, _StateData.gnssWorkerSmMutex);

    app_utils_mutex_put(_StateData.gnssWorkerSmMutex);
}

void gnss_worker_sm_report_delay_timer_cb_in_timer_context(uint32 cbData)
{
    APP_LOGM("Report delay timer expiry");

    gnss_worker_send_msg_process_cb(_gnss_worker_sm_report_delay_timer_cb, cbData);
}

void gnss_worker_sm_backoff_timer_cb_in_timer_context(uint32 cbData)
{
    APP_LOGM("Backoff timer expiry");

    gnss_worker_send_msg_process_cb(_gnss_worker_sm_backoff_timer_cb, cbData);
}

void gnss_worker_sm_position_refresh_timer_cb_in_timer_context(uint32 cbData)
{
    APP_LOGM("Position Refresh timer expiry");

    gnss_worker_send_msg_process_cb(_gnss_worker_sm_position_refresh_timer_cb, cbData);
}

void gnss_worker_sm_ad_dl_complete_timer_cb_in_timer_context(uint32 cbData)
{
    APP_LOGM("AD DL Complete timer expiry");

    gnss_worker_send_msg_process_cb(_gnss_worker_sm_ad_dl_complete_timer_cb, cbData);
}

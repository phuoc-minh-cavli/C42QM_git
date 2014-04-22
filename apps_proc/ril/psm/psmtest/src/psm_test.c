/******************************************************************************
#@file    psm_test.c
#@brief   Provides interface for testing PSM functionality
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stringl/stringl.h>

#include "network_access_service_v01.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_target_ext.h"

#include "psm_client.h"
#include "ril_port.h"

#include "qapi_location.h"
#include "qapi_socket.h"
#include "qapi_netservices.h"
#include "ULog.h"
#include "ULogFront.h"

//Include the headers for Module Manager Usage
#include "fs_public.h"
#include "tx_api.h"
#include "txm_module.h"
#include "qurt_system.h"



#include "utils_common.h"

/*External declarations for MMU APIs*/
extern qurt_txm_module_manager_setup_module_mmu(TXM_MODULE_INSTANCE *module);

extern int qurt_mapping_create_with_asid(qurt_addr_t vaddr, qurt_addr_t paddr, qurt_size_t size,
                         qurt_mem_cache_mode_t cache_attribs, qurt_perm_t perm, uint32 asid);

/*===========================================================================

                           DEFINES

===========================================================================*/

#define PSM_KERNEL_APP_FULL        0

#define PSM_AUTOMATE_TEST_FILE        "/ril/psm/enable_auto_test"
#define PSM_LEGACY_TEST_FILE          "/ril/psm/enable_legacy_test"
#define PSM_DAMTEST_MODULE_FILE       "/ril/psm/dam_psmtest.bin"
#define PSM_LLVM_DAMTEST_MODULE_FILE  "/ril/psm/dam_psmtest_llvm.bin"

#define PSM_AUTOMATE_TEST_COUNT_FILE  "/ril/psm/psm_legacycount"

#define TRUE                              1
#define FALSE                             0
#define SUCCESS                           0
#define FAIL                             -1
#define PSM_DEFAULT_DATA_EVENT_FREQUENCY  1
#define PSM_DEFAULT_TEST_ACTIVE_TIMER     0
#define PSM_DEFAULT_TEST_PSM_TIME        (150)
#define PSM_DEFAULT_PING_PACKET_SIZE      1000
#define PSM_PING_MAX_PACKET_SIZE         (1500)
#define PSM_DEFAULT_GPS_TIME_INTERVAL    (30)
#define PSM_DEFAULT_GPS_MIN_DISTANCE     (0)
#define PSM_DEFAULT_GPS_SAMPLE_COUNT     (5)
#define PSM_GPS_USER_BUFFER_SIZE         (128)
#define PSM_DEFAULT_DEEP_SLEEP_TIME      (50)

#define PSM_TEST_LOG_DEBUG(...)   RIL_LOG_MSG(MSG_LEGACY_HIGH, __VA_ARGS__ )
#define PSM_TEST_LOG_ERROR(...)   RIL_LOG_MSG(MSG_LEGACY_ERROR, __VA_ARGS__ )
#define PSM_TEST_LOG_FATAL(...)   RIL_LOG_MSG(MSG_LEGACY_ERROR, __VA_ARGS__ )
#define PSM_TEST_LOG_VERBOSE(...) RIL_LOG_MSG(MSG_LEGACY_LOW, __VA_ARGS__ )
#define PSM_TEST_LOG_INFO(...)    RIL_LOG_MSG(MSG_LEGACY_MED, __VA_ARGS__ )

#define PSM_QMI_TEST_TIMEOUT (5000)
#define PSM_DATA_EVENT_PIPE_MAX_ELEMENTS 4
#define PSM_TEST_ULOG_FILE_NAME  "PSM_TEST"
#define PSM_TEST_ULOG_BUF_SIZE   16384

/*===========================================================================

                           GLOBALS

===========================================================================*/

int               client_id = -1;
int               is_modem_loaded = FALSE;
int               health_check_fail = FALSE;
int               backoff = FALSE;
int               psm_complete = FALSE;
int               nw_full_service = FALSE;
int               data_connected = FALSE;
int               psm_init_complete = FALSE; /* Flag to record Init call*/
int               gps_samples = 0;
int               psm_cycles_count = 0;
int               fake_psm_time = -1;
qmi_client_type   qmi_nas_handle;
#if PSM_KERNEL_APP_FULL
dss_hndl_t        dss_handle;
#endif
static ULogHandle psm_test_log_handle = NULL;
#if PSM_KERNEL_APP_FULL
static uint8_t    psmGpsUserBuffer[PSM_GPS_USER_BUFFER_SIZE];

qapi_Location_Callbacks_t sLocationCallbacks;
qapi_loc_client_id gpsid;
#endif
ril_platform_thread_mutex_t   data_connection_mutex;

ril_platform_thread_mutex_t   mutex;
ril_platform_thread_cond_t    cond;

ril_platform_thread_mutex_t   c_mutex;
ril_platform_thread_cond_t    c_cond;
#if PSM_KERNEL_APP_FULL
ril_platform_thread_mutex_t   gpsresp_mutex;
ril_platform_thread_mutex_t   gpstrack_mutex;
ril_platform_thread_cond_t    gpsresp_cond;
ril_platform_thread_cond_t    gpstrack_cond;

ril_platform_pipe_t         data_test_pipe;
#endif

TXM_MODULE_INSTANCE   dam_psmtest;

typedef struct
{
  int    err;
  char * err_name;
}psm_err_map_type;

#if PSM_KERNEL_APP_FULL
typedef struct
{
  dss_net_evt_t         evt;
  void                 *data;
  dss_hndl_t            hndl;
} psm_data_test_event_params_type;
#endif

typedef struct
{
  int               valid;             /* Is the configuration valid */
#if PSM_KERNEL_APP_FULL
  char              apn[128];          /* APN to use with Data call */
  char              ip_addr[128];      /* IP to send the Ping/Data to */
  int               data_event_freq;   /* Nth cycle in which Data events are performed */
  int               is_data_event_cycle; /* Is current cycle involves app event */
#endif
  int               test_active_timer; /* Active Time for PSM Voting */
  int               time_in_psm;       /* Time in PSM  */
  int               is_modem_required; /* Is modem required from next cycle onwards */
#if PSM_KERNEL_APP_FULL
  int               is_data_success_required; /* Is data event SUCCESS mandatory */
  int               ping_size;         /* Size of Ping */
  int               is_location_scenario; /* Is current cycle for Location event */
  int               gps_min_distance;  /* Min GPS distance for tracking */
  int               gps_time_interval; /* Is the configuration valid */
  int               gps_samples;       /* Number of GPS Samples to take in current cycle */
  int               usecase;           /* Use case number. Refer to Use cases table below */
  int               fake_time_in_psm;  /* Fake time to send APP to sleep in Deepsleep cases */
#endif
  int               gps_debug_sleep;   /* Flag to enable time delays in GPS Usecases for Debugging*/

} psm_test_configuration_type;

/*
    Usecases table
    0 - Norma Ping + PSM Scenario
    1 - Normal - Default number of readings and then to PSM
    2 - DeepSleep - 2 DeepSleep cycles and then PSM
    3 - No PSM - 2 No PSM Vote cycles and then no-PSM standby
    4 - Late PSM - 2 No PSM Vote cycles and then PSM enter
*/

const psm_err_map_type error_string[] = {
    {PSM_ERR_NONE, "NONE"},
    {PSM_ERR_FAIL, "FAIL"},
    {PSM_ERR_GENERIC_FAILURE, "GENERIC FAILURE"},
    {PSM_ERR_APP_NOT_REGISTERED, "APP NOT REGISTERED"},
    {PSM_ERR_WRONG_ARGUMENTS, "WRONG VALUES PASSED"},
    {PSM_ERR_IPC_FAILURE, "FAILED TO COMMUNICATE WITH SERVER"},
    {PSM_ERR_INVALID_ACTIVE_TIME, "INVALID ACTIVE TIME PASSED"}
};


const psm_err_map_type reject_string[] = {
    {PSM_REJECT_REASON_NONE, "NONE"},
    {PSM_REJECT_REASON_NOT_ENABLED, "PSM FEATURE NOT ENABLED"},
    {PSM_REJECT_REASON_MODEM_NOT_READY, "MODEM NOT READY FOR PSM"},
    {PSM_REJECT_REASON_DURATION_TOO_SHORT, "ENTERED PSM DURATION IS TOO SHORT"},
    {PSM_REJECT_REASON_INCORRECT_USAGE, "INCORRECT PSM USAGE"},
};

const psm_err_map_type status_string[] = {
    {PSM_STATUS_NONE,               "NONE"},
    {PSM_STATUS_REJECT,             "REJECT"},
    {PSM_STATUS_READY,              "READY TO ENTER PSM MODE"},
    {PSM_STATUS_NOT_READY,          "NOT READY TO ENTER PSM MODE"},
    {PSM_STATUS_COMPLETE,           "ENTERED INTO PSM MODE"},
    {PSM_STATUS_DISCONNECTED,       "COMMUNICATION WITH SERVER LOST"},
    {PSM_STATUS_MODEM_LOADED,       "MODEM LOADED"},
    {PSM_STATUS_MODEM_NOT_LOADED,   "MODEM NOT LOADED"},
    {PSM_STATUS_NW_OOS,             "NW OOS"},
    {PSM_STATUS_NW_LIMITED_SERVICE, "NW LIMITED SERVICE"},
    {PSM_STATUS_HEALTH_CHECK,       "HEALT CHECK"},
    {PSM_STATUS_FEATURE_ENABLED,    "PSM FEATURE ENABLED"},
    {PSM_STATUS_FEATURE_DISABLED,   "PSM FEATURE DISABLED"},
};

psm_test_configuration_type  app_configuration;


/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/
#if PSM_KERNEL_APP_FULL
void psm_test_net_event_cb
(
    dss_hndl_t hndl,
    void * user_data,
    dss_net_evt_t evt,
    dss_evt_payload_t *payload_ptr
);
#endif

#if PSM_KERNEL_APP_FULL
extern pm_rtc_app_init(void);

void psm_test_evaluate_update_psm_cycle(void);
#endif

/*===========================================================================

                           FUNCTIONS

===========================================================================*/

void psm_test_critical_ulog( int arg_count, const char *format, ... )
{
    va_list args;
    
    va_start(args, format);
    ULogFront_RealTimeVprintf(psm_test_log_handle, arg_count, format, args);
    va_end(args);

}


void psm_test_critical_ulog_init( void )
{
#if defined(USES_QNX) && defined(QNP_POWERMAN_PROC)
        ULOG_LOCK_TYPE log_lock = ULOG_LOCK_NONE;
#else
        ULOG_LOCK_TYPE log_lock = ULOG_LOCK_OS;
#endif

    if( !psm_test_log_handle )
    {
        /* Creating a log handle */
        ULogFront_RealTimeInit(&psm_test_log_handle, PSM_TEST_ULOG_FILE_NAME,
                        PSM_TEST_ULOG_BUF_SIZE, ULOG_MEMORY_LOCAL, log_lock);
        ULogCore_Enable(psm_test_log_handle);
    }
}

#if PSM_KERNEL_APP_FULL
/*========================================================================

                CALLBACKS FOR GPS MEASUREMENT <START>

=========================================================================*/

static void psm_test_gps_capabilities_callback(qapi_Location_Capabilities_Mask_t capabilitiesMask)
{
    PSM_TEST_LOG_VERBOSE("GPS API CALLBACK : CAPABILITIES (mask 0x%x)", capabilitiesMask);
}

static void psm_test_gps_response_callback(qapi_Location_Error_t err, uint32_t id)
{
    PSM_TEST_LOG_VERBOSE("GPS API CALLBACK : RESPONSE (err=%u id=%u)", err, id);
    ril_platform_thread_mutex_lock(&gpsresp_mutex);
    ril_platform_thread_cond_signal(&gpsresp_cond);
    ril_platform_thread_mutex_unlock(&gpsresp_mutex);
}

static void psm_test_gps_geofence_response_callback(size_t count,
    qapi_Location_Error_t* err,
    uint32_t* ids)
{
    uint32_t i = 0;
    PSM_TEST_LOG_VERBOSE("GPS API CALLBACK : GEOFENCE RESPONSE (count %d)", count);

    for (i = 0; i < count; ++i) {
        PSM_TEST_LOG_VERBOSE("ID[%d] = %d, err[%d] = %d", i, ids[i], i, err[i]);
    }
}

static void psm_test_gps_tracking_callback(qapi_Location_t location)
{
    PSM_TEST_LOG_VERBOSE("GPS API CALLBACK : TRACKING");

    PSM_TEST_LOG_VERBOSE("LAT: %d.%d LON: %d.%d ALT: %d.%d ACC: %d.%d",
        (int)location.latitude, ((int)(location.latitude * 100000)) % 100000,
        (int)location.longitude, ((int)(location.longitude * 100000)) % 100000,
        (int)location.altitude, ((int)(location.altitude * 100)) % 100,
        (int)location.accuracy, ((int)(location.accuracy * 100)) % 100);
    psm_test_critical_ulog(8,"LAT: %d.%d LON: %d.%d ALT: %d.%d ACC: %d.%d",
        (int)location.latitude, ((int)(location.latitude * 100000)) % 100000,
        (int)location.longitude, ((int)(location.longitude * 100000)) % 100000,
        (int)location.altitude, ((int)(location.altitude * 100)) % 100,
        (int)location.accuracy, ((int)(location.accuracy * 100)) % 100);

    PSM_TEST_LOG_VERBOSE("SPEED: %d.%d BEAR: %d.%d TIME: 0x%x%x FLAGS: %u",
        (int)location.speed, ((int)(location.speed * 100)) % 100,
        (int)location.bearing, ((int)(location.bearing * 100)) % 100,
        location.timestamp >> 32, location.timestamp, location.flags);

    ril_platform_thread_mutex_lock(&gpstrack_mutex);

    gps_samples--;
    PSM_TEST_LOG_VERBOSE("GPS samples remaining: %d",gps_samples);
    psm_test_critical_ulog(1,"GPS samples remaining: %d",gps_samples);
    if(gps_samples == NIL)
    {
        ril_platform_thread_cond_signal(&gpstrack_cond);
    }

    ril_platform_thread_mutex_unlock(&gpstrack_mutex);
}

qapi_Location_Error_t psm_test_gps_init()
{
    qapi_Location_Error_t ret = QAPI_LOCATION_ERROR_GENERAL_FAILURE;

    sLocationCallbacks.size = sizeof(qapi_Location_Callbacks_t);
    sLocationCallbacks.capabilitiesCb = psm_test_gps_capabilities_callback;
    sLocationCallbacks.responseCb = psm_test_gps_response_callback;
    sLocationCallbacks.collectiveResponseCb = psm_test_gps_geofence_response_callback;
    sLocationCallbacks.trackingCb = psm_test_gps_tracking_callback;
    sLocationCallbacks.batchingCb = NULL;
    sLocationCallbacks.geofenceBreachCb = NULL;

    do
    {
        //Init GPS
        PSM_TEST_LOG_VERBOSE("Invoking qapi_Loc_Init()");
        ret = (qapi_Location_Error_t)qapi_Loc_Init(&gpsid,&sLocationCallbacks);
        if (ret != QAPI_LOCATION_ERROR_SUCCESS) 
        {
            PSM_TEST_LOG_VERBOSE("Init Returned failure ! (ret %d)", ret);
            break;
        }

        ret = (qapi_Location_Error_t)qapi_Loc_Set_User_Buffer(gpsid, psmGpsUserBuffer, PSM_GPS_USER_BUFFER_SIZE);
        PSM_TEST_LOG_VERBOSE("Init Returned success");
    } while(0);

    return ret;

}

qapi_Location_Error_t psm_test_gps_deinit()
{
    qapi_Location_Error_t ret = QAPI_LOCATION_ERROR_GENERAL_FAILURE;

    do
    {
        //DeInit GPS
        PSM_TEST_LOG_VERBOSE("Invoking qapi_Loc_Deinit()");
        ret = (qapi_Location_Error_t)qapi_Loc_Deinit(gpsid);
        if (ret != QAPI_LOCATION_ERROR_SUCCESS) {
            PSM_TEST_LOG_VERBOSE("Deinit Returned failure ! (ret %d)", ret);
            break;
        }
        PSM_TEST_LOG_VERBOSE("Deinit Returned success");

    } while(0);

    return ret;
}

qapi_Location_Error_t psm_test_gps_start_tracking(int* id)
{
    qapi_Location_Options_t locationOptions;
    qapi_Location_Error_t ret = QAPI_LOCATION_ERROR_GENERAL_FAILURE;

    do
    {
        if(!id)
        {
            PSM_TEST_LOG_VERBOSE("Invalid id passed");
            break;
        }

        //Start Tracking GPS
        PSM_TEST_LOG_VERBOSE("GPS Start tracking");
        PSM_TEST_LOG_VERBOSE("Interval %d seconds, Distance %d",
                                        app_configuration.gps_time_interval,
                                        app_configuration.gps_min_distance);

        locationOptions.size = sizeof(qapi_Location_Options_t);
        locationOptions.minInterval = (app_configuration.gps_time_interval)*1000;
        locationOptions.minDistance = (app_configuration.gps_min_distance);
        PSM_TEST_LOG_VERBOSE("Invoking qapi_Loc_Start_Tracking()");
        ret = (qapi_Location_Error_t)qapi_Loc_Start_Tracking(gpsid, &locationOptions, (uint32_t*)id);
        
        if (ret != QAPI_LOCATION_ERROR_SUCCESS) 
        {
            PSM_TEST_LOG_VERBOSE("Returned failure for start tracking ! (ret %d)", ret);
            break;
        }
        PSM_TEST_LOG_VERBOSE("Returned success for start tracking, id %d", *id);
        PSM_TEST_LOG_VERBOSE("Waiting for response callback...");
        //Wait for Response Callback
        ril_platform_thread_mutex_lock(&gpsresp_mutex);
        ril_platform_thread_cond_wait_and_clear(&gpsresp_cond, &gpsresp_mutex);
        ril_platform_thread_mutex_unlock(&gpsresp_mutex);
        PSM_TEST_LOG_VERBOSE("REQUEST SUCCESS start track");
    } while(0);

    return ret;
}

qapi_Location_Error_t psm_test_gps_stop_tracking(int id)
{
    qapi_Location_Error_t ret = QAPI_LOCATION_ERROR_GENERAL_FAILURE;

    do
    {
        //Stop Tracking GPS
        PSM_TEST_LOG_VERBOSE("GPS stop tracking");
        PSM_TEST_LOG_VERBOSE("Invoking qapi_Loc_Stop_Tracking() : id %d", id);
        ret = (qapi_Location_Error_t)qapi_Loc_Stop_Tracking(gpsid, id);
        if (ret != QAPI_LOCATION_ERROR_SUCCESS)
        {
            PSM_TEST_LOG_VERBOSE("Returned failure for stop tracking ! (ret %d)", ret);
            break;
        }
        PSM_TEST_LOG_VERBOSE("Returned success for stop tracking");
        PSM_TEST_LOG_VERBOSE("Waiting on Callback...");
        //Wait for Response Callback
        ril_platform_thread_mutex_lock(&gpsresp_mutex);
        ril_platform_thread_cond_wait_and_clear(&gpsresp_cond, &gpsresp_mutex);
        ril_platform_thread_mutex_unlock(&gpsresp_mutex);
        PSM_TEST_LOG_VERBOSE("REQUEST SUCCESS stop track");
    } while(0);

    return ret;
}

void psm_test_gps_sample_and_wait()
{

    do
    {
        //Wait for requested number of GPS samples
        ril_platform_thread_mutex_lock(&gpstrack_mutex);
        ril_platform_thread_cond_wait_and_clear(&gpstrack_cond, &gpstrack_mutex);
        ril_platform_thread_mutex_unlock(&gpstrack_mutex);
    } while(0);

}
#endif
/*========================================================================

                CALLBACKS FOR GPS MEASUREMENT <END>

=========================================================================*/

void psm_test_initialize_configuration (void)
{
    /* Initialize the structure with default configuration values*/
    app_configuration.valid = FALSE;
    app_configuration.test_active_timer = -1;
    app_configuration.time_in_psm = PSM_DEFAULT_TEST_PSM_TIME;
    app_configuration.is_modem_required = TRUE;
#if PSM_KERNEL_APP_FULL
    app_configuration.is_data_event_cycle = TRUE;
    app_configuration.is_data_success_required = FALSE;
    app_configuration.ping_size = PSM_DEFAULT_PING_PACKET_SIZE;
    app_configuration.data_event_freq = PSM_DEFAULT_DATA_EVENT_FREQUENCY;
    app_configuration.is_location_scenario = FALSE;
    app_configuration.gps_min_distance = PSM_DEFAULT_GPS_MIN_DISTANCE;
    app_configuration.gps_time_interval = PSM_DEFAULT_GPS_TIME_INTERVAL;
    app_configuration.gps_samples = PSM_DEFAULT_GPS_SAMPLE_COUNT;
    app_configuration.usecase = 0;
    app_configuration.fake_time_in_psm = PSM_DEFAULT_DEEP_SLEEP_TIME;
#endif
    app_configuration.gps_debug_sleep = 0;
}

void psm_test_app_call_back_func(psm_status_msg_type *psm_status)
{
    if ( psm_status )
    {
        // to avoid fludding of logs, don't print health check.
        if ( psm_status->status != PSM_STATUS_HEALTH_CHECK &&
             psm_status->status != PSM_STATUS_NONE )
        {
            PSM_TEST_LOG_VERBOSE("PSM STATUS:    %s", status_string[psm_status->status].err_name);
            PSM_TEST_LOG_VERBOSE("REJECT REASON: %s\n", reject_string[psm_status->reason].err_name);
        }

        switch(psm_status->status)
        {
#if PSM_KERNEL_APP_FULL
            // special handling for some cases like modem loaded, not loaded etc.
            case PSM_STATUS_MODEM_LOADED:
                is_modem_loaded = TRUE;
                break;

            case PSM_STATUS_MODEM_NOT_LOADED:
                is_modem_loaded = FALSE;
                break;
#endif
            case PSM_STATUS_HEALTH_CHECK:
#if PSM_KERNEL_APP_FULL
                if ( !health_check_fail )
#endif
                {
                    psm_client_health_check_ack(client_id);
                }
                break;
#if PSM_KERNEL_APP_FULL
            case PSM_STATUS_NW_OOS:
                if ( backoff )
                {
                    psm_client_enter_backoff(client_id);
                }
                break;

            case PSM_STATUS_NW_LIMITED_SERVICE:
                if ( backoff )
                {
                    psm_client_enter_backoff(client_id);
                }
                break;
#endif
            case PSM_STATUS_COMPLETE:
                PSM_TEST_LOG_VERBOSE("Received PSM complete");
                break;

            default:
                break;
        }
    }
}

void psm_test_nas_unsol_ind_cb(qmi_client_type user_handle,
                               unsigned int message_id,
                               void *ind_buf,
                               unsigned int ind_buf_len,
                               void *ind_cb_data)
{
    void*                     decoded_payload;
    uint32_t                  decoded_payload_len;
    nas_sys_info_ind_msg_v01 *sys_info_ind;

    qmi_idl_get_message_c_struct_len(nas_get_service_object_v01(),
                                     QMI_IDL_INDICATION,
                                     message_id,
                                     &decoded_payload_len);

    if(decoded_payload_len)
    {
        decoded_payload = malloc(decoded_payload_len);
        if(decoded_payload)
        {
            memset(decoded_payload, 0, decoded_payload_len);
            qmi_client_message_decode(user_handle,
                                      QMI_IDL_INDICATION,
                                      message_id,
                                      ind_buf,
                                      ind_buf_len,
                                      decoded_payload,
                                      (int)decoded_payload_len);

            ril_platform_thread_mutex_lock(&mutex);
            switch(message_id)
            {
                case QMI_NAS_SYS_INFO_IND_MSG_V01:
                    sys_info_ind = (nas_sys_info_ind_msg_v01*)decoded_payload;
                    if ( sys_info_ind->lte_srv_status_info_valid == 1 &&
                         sys_info_ind->lte_srv_status_info.srv_status == SYS_SRV_STATUS_SRV_V01 )
                    {
                        PSM_TEST_LOG_VERBOSE("LTE IN-SERVICE");
                        nw_full_service = TRUE;
                    }
                    else if ( sys_info_ind->wcdma_srv_status_info_valid == 1 &&
                         sys_info_ind->wcdma_srv_status_info.srv_status == SYS_SRV_STATUS_SRV_V01 )
                    {
                        PSM_TEST_LOG_VERBOSE("WCDMA IN-SERVICE");
                        nw_full_service = TRUE;
                    }
                    else if ( sys_info_ind->gsm_srv_status_info_valid == 1 &&
                         sys_info_ind->gsm_srv_status_info.srv_status == SYS_SRV_STATUS_SRV_V01 )
                    {
                        PSM_TEST_LOG_VERBOSE("GSM IN-SERVICE");
                        nw_full_service = TRUE;
                    }
                    else
                    {
                        PSM_TEST_LOG_VERBOSE("NO-SERVICE");
                    }
                break;

                default:
                    break;
            }

            if ( nw_full_service == TRUE )
            {
                ril_platform_thread_cond_signal(&cond);
            }
            ril_platform_thread_mutex_unlock(&mutex);
        }
        free(decoded_payload);
    }

}

void psm_test_fetch_network_state(void)
{
    qmi_client_error_type                         transport_error = QMI_INTERNAL_ERR;
    nas_get_sys_info_req_msg_v01                  sys_info_req;
    nas_get_sys_info_resp_msg_v01                 sys_info_resp;

    memset(&sys_info_req, 0, sizeof(sys_info_req));
    memset(&sys_info_resp, 0, sizeof(sys_info_resp));

    transport_error = qmi_client_send_msg_sync(qmi_nas_handle,
                                               QMI_NAS_GET_SYS_INFO_REQ_MSG_V01,
                                               &sys_info_req,
                                               sizeof(sys_info_req),
                                               &sys_info_resp,
                                               sizeof(sys_info_resp),
                                               PSM_QMI_TEST_TIMEOUT);
    if ( transport_error != QMI_NO_ERR )
    {
        PSM_TEST_LOG_VERBOSE("Failed to get sys info - %d", transport_error);
    }
    else
    {
         PSM_TEST_LOG_VERBOSE("get sys info response: error - result: %d, error: %d",
                              sys_info_resp.resp.result,
                              sys_info_resp.resp.error);
    }

    ril_platform_thread_mutex_lock(&mutex);
    if ( sys_info_resp.lte_srv_status_info_valid == 1 &&
         sys_info_resp.lte_srv_status_info.srv_status == SYS_SRV_STATUS_SRV_V01 )
    {
        PSM_TEST_LOG_VERBOSE("LTE IN-SERVICE");
        nw_full_service = TRUE;
    }
    else if ( sys_info_resp.wcdma_srv_status_info_valid == 1 &&
              sys_info_resp.wcdma_srv_status_info.srv_status == SYS_SRV_STATUS_SRV_V01 )
    {
        PSM_TEST_LOG_VERBOSE("WCDMA IN-SERVICE");
        nw_full_service = TRUE;
    }
    else if ( sys_info_resp.gsm_srv_status_info_valid == 1 &&
         sys_info_resp.gsm_srv_status_info.srv_status == SYS_SRV_STATUS_SRV_V01 )
    {
        PSM_TEST_LOG_VERBOSE("GSM IN-SERVICE");
        nw_full_service = TRUE;
    }
    else
    {
        PSM_TEST_LOG_VERBOSE("NO-SERVICE");
    }

    if ( nw_full_service == TRUE )
    {
        ril_platform_thread_cond_signal(&cond);
        psm_test_critical_ulog(1,"Network Full Service: %d", nw_full_service);
    }
    ril_platform_thread_mutex_unlock(&mutex);
}

void psm_test_init(void)
{
    qmi_client_os_params                          os_params;
    qmi_client_error_type                         transport_error = QMI_INTERNAL_ERR;
    nas_indication_register_req_msg_v01           nwreg_ind_req;
    nas_indication_register_resp_msg_v01          nwreg_ind_resp;

    memset(&nwreg_ind_req, 0, sizeof(nwreg_ind_req));
    memset(&nwreg_ind_resp, 0, sizeof(nwreg_ind_resp));

    do
    {
        transport_error = qmi_client_init_instance(nas_get_service_object_v01(),
                                                   QMI_CLIENT_INSTANCE_ANY,
                                                   psm_test_nas_unsol_ind_cb,
                                                   NULL,
                                                   &os_params,
                                                   0,
                                                   &qmi_nas_handle);

        if ( transport_error == QMI_NO_ERR )
        {
            break;
        }
        PSM_TEST_LOG_VERBOSE("Failed to connect to QMI NAS.. retrying");

    }while(1);

    nwreg_ind_req.sys_info_valid = 1;
    nwreg_ind_req.sys_info       = 1;

    nwreg_ind_req.req_serving_system_valid = 1;
    nwreg_ind_req.req_serving_system = 0;

    transport_error = qmi_client_send_msg_sync(qmi_nas_handle,
                                               QMI_NAS_INDICATION_REGISTER_REQ_MSG_V01,
                                               &nwreg_ind_req,
                                               sizeof(nwreg_ind_req),
                                               &nwreg_ind_resp,
                                               sizeof(nwreg_ind_resp),
                                               PSM_QMI_TEST_TIMEOUT);
    if ( transport_error != QMI_NO_ERR )
    {
        PSM_TEST_LOG_VERBOSE("Failed to register for nas indications - %d", transport_error);
    }
    else
    {
         PSM_TEST_LOG_VERBOSE("register nas indication response: error - result: %d, error: %d",
                              nwreg_ind_resp.resp.result,
                              nwreg_ind_resp.resp.error);
    }

    /* Fetch Sys-Info and update the global flag of Full Service */
    psm_test_fetch_network_state();

#if PSM_KERNEL_APP_FULL
    if( TRUE == app_configuration.is_data_event_cycle )
    {
        PSM_TEST_LOG_VERBOSE("just before dss_init");

        if (DSS_SUCCESS !=  dss_init(DSS_MODE_GENERAL))
        {
            PSM_TEST_LOG_VERBOSE("%s","dss_init failed !!");
        }
        else
        {
            PSM_TEST_LOG_VERBOSE("dss_init successful");
        }

        do
        {
            psm_test_critical_ulog(0,"Registering Callback DSS_Handle");
            /* obtain data service handle */
            dss_handle = dss_get_data_srvc_hndl(psm_test_net_event_cb, NULL);
            PSM_TEST_LOG_VERBOSE("dss_handle %d", dss_handle);
            if ( dss_handle != NULL )
            {
                break;
            }
            ril_platform_thread_sleep(SLEEP_10MS);
        }while(1);
    }
    else
    {
        PSM_TEST_LOG_VERBOSE("Data service handle registration not required");
    }
#endif
}


static int psm_test_register_application(void)
{
    psm_error_type_e  result = PSM_ERR_NONE;
    do
    {
        result = psm_client_register(&client_id, psm_test_app_call_back_func);
        if ( result == PSM_ERR_NONE )
        {
            break;
        }
        ril_platform_thread_sleep(SLEEP_10MS);
    }while(1);
    PSM_TEST_LOG_VERBOSE("Result: %s", result ? "FAIL": "SUCCESS");
    PSM_TEST_LOG_VERBOSE("Application Id: %d", client_id);
    return result;
}

int psm_test_enter_psm_auto(void)
{
    int             result = PSM_ERR_NONE;
    int             psm_voting = TRUE;
    psm_info_type   psm_info;

    psm_test_critical_ulog(0,"Enter PSM process executing. Test app work is done.");

    fake_psm_time = -1; //Initialize it so that it is normal scenario
    memset(&psm_info, 0, sizeof(psm_info));
    psm_info.psm_time_info.time_format_flag     = PSM_TIME_IN_SECS;
    psm_info.psm_wakeup_type = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;

    if( app_configuration.test_active_timer >= 0 )
    {
        psm_info.active_time_in_secs = app_configuration.test_active_timer;
    }
    else
    {
        psm_info.active_time_in_secs = PSM_DEFAULT_TEST_ACTIVE_TIMER;
    }

    psm_info.psm_time_info.psm_duration_in_secs = app_configuration.time_in_psm;

#if PSM_KERNEL_APP_FULL
    if((app_configuration.usecase == 2) && (psm_cycles_count < 2))
    {
        /* Need to vote less than Threshold for 2 cycles - DeepSleep Usecase*/
        psm_info.psm_time_info.psm_duration_in_secs = app_configuration.fake_time_in_psm; //Safe value less than 60 seconds
        fake_psm_time = app_configuration.fake_time_in_psm;
    }
    else if((app_configuration.usecase == 3) && (psm_cycles_count < 2))
    {
        /*No need to set time since no voting of PSM. Fake time so it sleeps till then */
        fake_psm_time = app_configuration.fake_time_in_psm;
        psm_voting = FALSE;
    }
    else if((app_configuration.usecase == 4) && (psm_cycles_count < 2))
    {
        /*
            No need to set time since no voting of PSM for 2 cycles.
            Fake time so it sleeps till then.
        */
        fake_psm_time = app_configuration.fake_time_in_psm;
        psm_voting = FALSE;
    }
    else if(app_configuration.usecase == 3)
    {
        /* No need to vote for PSM in entire 3rd scenario */
        psm_voting = FALSE;
    }
#endif
    if( app_configuration.is_modem_required == FALSE )
    {
        psm_info.psm_wakeup_type = PSM_WAKEUP_MEASUREMENT_ONLY;
    }

    PSM_TEST_LOG_VERBOSE("PSM Voting for this cycle: %d", psm_voting);
    psm_test_critical_ulog(1,"PSM Voting for this cycle: %d", psm_voting);
    if(psm_voting == TRUE)
    {
        PSM_TEST_LOG_VERBOSE("Enter into Power save Mode");
        result = psm_client_enter_power_save(client_id, &psm_info);
    }

    PSM_TEST_LOG_VERBOSE("Result: %s", result ? "FAIL":"SUCCESS");
    if ( result != PSM_ERR_NONE )
    {
        PSM_TEST_LOG_VERBOSE("Reason: %s", error_string[result].err_name);
    }

    /* 
        Test app utility is complete at this point
        App has to sleep for the voted time. So signal main thread exit.
    */
    ril_platform_thread_mutex_lock(&c_mutex);
    psm_complete = TRUE;
    ril_platform_thread_cond_signal(&c_cond);
    ril_platform_thread_mutex_unlock(&c_mutex);

    return 0;
}

#if PSM_KERNEL_APP_FULL
void psm_test_net_event_cb( dss_hndl_t hndl,
                                     void * user_data,
                                     dss_net_evt_t evt,
                                     dss_evt_payload_t *payload_ptr )
{

    int                              datalen = 0;
    psm_data_test_event_params_type *params;

    params = malloc(sizeof(*params));
    if(params != NULL)
    {
        memset(params, 0, sizeof(psm_data_test_event_params_type));

        datalen = sizeof(user_data);
        params->evt = evt;
        params->hndl = hndl;

        if( user_data && datalen > 0 )
        {
            params->data = malloc(datalen);
            if( params->data != NULL )
            {
                memset(params->data, 0, datalen);
                memscpy(params->data, datalen, user_data, datalen);
            }
        }


        ril_platform_pipe_send(data_test_pipe, params);
        PSM_TEST_LOG_VERBOSE("Data test event sent to data test thread.");
        free(params);
    }

}

int psm_test_perform_ping(void)
{
    int                   rc = FAIL;
    int                   count_full_size = 0;
    int                   partial_ping = FALSE;
    int                   partial_ping_size = 0;
    struct                ip46addr addr;


    PSM_TEST_LOG_VERBOSE("Enter Perform ping function");

    PSM_TEST_LOG_VERBOSE("Setting ip route");
    addr.type = AF_INET;
    PSM_TEST_LOG_VERBOSE("IP Address to ping: %s",app_configuration.ip_addr);

    if (inet_pton(AF_INET, app_configuration.ip_addr, &addr.a) == 0)
    {
        count_full_size = (app_configuration.ping_size/PSM_PING_MAX_PACKET_SIZE);
        if( NIL != ((app_configuration.ping_size)%PSM_PING_MAX_PACKET_SIZE) )
        {
            partial_ping = TRUE;
            partial_ping_size = ((app_configuration.ping_size)%PSM_PING_MAX_PACKET_SIZE);
        }

        PSM_TEST_LOG_VERBOSE("Number of Pings based upon packet size : %d %d (%d %d) ",
                             (app_configuration.ping_size), count_full_size, partial_ping, partial_ping_size);
        psm_test_critical_ulog(4,"Number of Pings based upon packet size : %d %d (%d %d) ",
                                (app_configuration.ping_size), count_full_size, partial_ping, partial_ping_size);

        while(count_full_size--)
        {
            do
            {
                rc = qapi_Net_Ping(addr.a.addr4, PSM_PING_MAX_PACKET_SIZE);
                PSM_TEST_LOG_VERBOSE("Full packet Ping[%d]: Result: %d", count_full_size+1, rc);

                if( SUCCESS == rc  || FALSE == app_configuration.is_data_success_required )
                {
                    break;
                }

                ril_platform_thread_sleep(SLEEP_10MS);

            }while(1);
        }

        PSM_TEST_LOG_VERBOSE("Ping packet size: %d", partial_ping_size);
        psm_test_critical_ulog(1,"Ping packet Size: %d", partial_ping_size);

        if( TRUE == partial_ping )
        {
            do
            {
                rc = qapi_Net_Ping(addr.a.addr4, partial_ping_size);
                if( SUCCESS == rc  || FALSE == app_configuration.is_data_success_required )
                {
                    break;
                }

                ril_platform_thread_sleep(SLEEP_10MS);
            }while(1);
        }

        PSM_TEST_LOG_VERBOSE("Ping: %d. IP: %s\n", rc, app_configuration.ip_addr);
        psm_test_critical_ulog(1,"Ping: %d.\n", rc);
    }

    PSM_TEST_LOG_VERBOSE("Exit ping function with return value: %d", rc);
    psm_test_critical_ulog(1,"Exit ping function with return value: %d", rc);
    return rc;
}

void psm_test_perform_location_measure()
{
    qapi_Location_Error_t ret = QAPI_LOCATION_ERROR_GENERAL_FAILURE;
    int                   id = -1;

    do
    {
        /* Set the global variable with configured number gps_samples count */
        gps_samples = app_configuration.gps_samples;

        ret = psm_test_gps_init();
        if(ret != QAPI_LOCATION_ERROR_SUCCESS)
        {
            PSM_TEST_LOG_VERBOSE("GPS Init failed, exiting: %d", ret);
            break;
        }
        PSM_TEST_LOG_VERBOSE("GPS Init success");
        psm_test_critical_ulog(0,"GPS Init success");

        ret = psm_test_gps_start_tracking(&id);
        if(ret != QAPI_LOCATION_ERROR_SUCCESS)
        {
            PSM_TEST_LOG_VERBOSE("GPS tracking req failed, exiting: %d", ret);
            break;
        }
        PSM_TEST_LOG_VERBOSE("GPS Track req success");
        psm_test_critical_ulog(0,"GPS Track req success");

        // Wait for requested number of samples
        PSM_TEST_LOG_VERBOSE("Wait for configured no. of GPS measurements");
        psm_test_gps_sample_and_wait();
        PSM_TEST_LOG_VERBOSE("Wait for configured no. of GPS measurements COMPLETE");
        psm_test_critical_ulog(0,"Wait for configured no. of GPS measurements COMPLETE");

        PSM_TEST_LOG_VERBOSE("Ping final Location info..");
        psm_test_perform_ping();
        ret = psm_test_gps_stop_tracking(id);
        if(ret != QAPI_LOCATION_ERROR_SUCCESS)
        {
            PSM_TEST_LOG_VERBOSE("GPS deinit failed, exiting: %d", ret);
            break;
        }
        PSM_TEST_LOG_VERBOSE("GPS deinit success");

        ret = psm_test_gps_deinit();
    } while(0);

}

void psm_test_perform_app_event(void)
{
    /* Handler for performing configured event. */
    /* Check if Location Event */
    if(TRUE == app_configuration.is_location_scenario)
    {
        psm_test_perform_location_measure();
    }
    else
    {
        psm_test_perform_ping();
    }
}

#if PSM_KERNEL_APP_FULL
void psm_test_process_data_test_event(psm_data_test_event_params_type *params)
{
    dss_net_evt_t         evt;

    evt = params->evt;

    if (evt > DSS_EVT_INVALID && evt < DSS_EVT_MAX)
    {
        switch( evt )
        {
            case DSS_EVT_NET_NO_NET:
                PSM_TEST_LOG_VERBOSE("DSS_EVT_NET_NO_NET");

                /* Obtain mutex and set No Data state */
                ril_platform_thread_mutex_lock(&data_connection_mutex);
                data_connected = FALSE;
                ril_platform_thread_mutex_unlock(&data_connection_mutex);
                psm_test_critical_ulog(0,"Data call got disconnected");
                break;

            case DSS_EVT_NET_IS_CONN:
                PSM_TEST_LOG_VERBOSE("DSS_EVT_NET_IS_CONN");
                psm_test_critical_ulog(0,"DSS Event Connected. Attempting Ping");

                /* Obtain mutex and set Data connected state*/
                ril_platform_thread_mutex_lock(&data_connection_mutex);
                data_connected = TRUE;
                ril_platform_thread_mutex_unlock(&data_connection_mutex);

                psm_test_perform_app_event();
                psm_test_enter_psm_auto();
                break;

            default:
                break;
        }
    }
}
#endif

int psm_test_start_lte_data_call(void)
{
    dss_call_param_value_t param_info;

    psm_test_critical_ulog(0,"LTE Data call function entered");

    if ( dss_handle != NULL )
    {
        /* set data call param */
        param_info.buf_val = NULL;
        param_info.num_val = DSS_RADIO_TECH_UNKNOWN;
        PSM_TEST_LOG_VERBOSE("Setting tech to Automatic");
        dss_set_data_call_param(dss_handle, DSS_CALL_INFO_TECH_PREF, &param_info);

        param_info.buf_val = app_configuration.apn;
        param_info.num_val = strlen(app_configuration.apn);
        PSM_TEST_LOG_VERBOSE("Setting APN - %s", app_configuration.apn);
        dss_set_data_call_param(dss_handle, DSS_CALL_INFO_APN_NAME, &param_info);

        param_info.buf_val = NULL;
        param_info.num_val = DSS_IP_VERSION_4;
        PSM_TEST_LOG_VERBOSE("Setting family to IP");
        dss_set_data_call_param(dss_handle, DSS_CALL_INFO_IP_VERSION, &param_info);

        /* set data call param for UMTS*/
        param_info.buf_val = NULL;
        param_info.num_val = 1;
        PSM_TEST_LOG_VERBOSE("setting profile number to: %d", 1);
        dss_set_data_call_param(dss_handle, DSS_CALL_INFO_UMTS_PROFILE_IDX, &param_info);

        dss_start_data_call(dss_handle);
        psm_test_critical_ulog(0,"Start data call executed");
    }
    else
    {
        PSM_TEST_LOG_ERROR("dss_handle is NULL");
    }
    return 0;
}
#endif
void psm_test_read_config_file(void)
{
    void *config_list  = NULL;
    int  fp_auto       = INVALID_FILE_HANDLE;
    int  fp_legacyauto = INVALID_FILE_HANDLE;
    fp_auto =  efs_open( PSM_AUTOMATE_TEST_FILE, O_RDONLY);
    fp_legacyauto =  efs_open( PSM_LEGACY_TEST_FILE, O_RDONLY);
    if(fp_legacyauto != INVALID_FILE_HANDLE)
    {
        efs_close(fp_legacyauto);
        if(fp_auto != INVALID_FILE_HANDLE)
        {
            efs_close(fp_auto);
        }
        config_list = util_config_load(PSM_LEGACY_TEST_FILE);    
    }
    else if(fp_auto != INVALID_FILE_HANDLE)
    {
        efs_close(fp_auto);
        config_list = util_config_load(PSM_AUTOMATE_TEST_FILE); 
    }    
    
    util_config_get_value(config_list, "ACTIVE_TIMER", INTEGER, &app_configuration.test_active_timer);
    util_config_get_value(config_list, "TIME_IN_PSM", INTEGER, &app_configuration.time_in_psm);
    util_config_get_value(config_list, "MODEM_REQUIRED", INTEGER, &app_configuration.is_modem_required);
#if PSM_KERNEL_APP_FULL
    util_config_get_value(config_list, "IP", STRING, app_configuration.ip_addr);
    util_config_get_value(config_list, "APN", STRING, app_configuration.apn);
    util_config_get_value(config_list, "DATA_EVENT_FREQUENCY", INTEGER, &app_configuration.data_event_freq);
    util_config_get_value(config_list, "DATA_SUCCESS_REQUIRED", INTEGER, &app_configuration.is_data_success_required);
    util_config_get_value(config_list, "PING_SIZE", INTEGER, &app_configuration.ping_size);
    util_config_get_value(config_list, "GPS_MEASURE", INTEGER, &app_configuration.is_location_scenario);
    util_config_get_value(config_list, "GPS_INTERVAL", INTEGER, &app_configuration.gps_time_interval);
    util_config_get_value(config_list, "GPS_DISTANCE", INTEGER, &app_configuration.gps_min_distance);
    util_config_get_value(config_list, "GPS_SAMPLES", INTEGER, &app_configuration.gps_samples);
    util_config_get_value(config_list, "GPS_USECASE", INTEGER, &app_configuration.usecase);
    util_config_get_value(config_list, "FAKE_TIME_IN_PSM", INTEGER, &app_configuration.fake_time_in_psm);
#endif
    util_config_get_value(config_list, "GPS_DEBUG", INTEGER, &app_configuration.gps_debug_sleep);

#if PSM_KERNEL_APP_FULL
    PSM_TEST_LOG_VERBOSE("Data Event Frequency from Config file is %d",app_configuration.data_event_freq);
#endif
    PSM_TEST_LOG_VERBOSE("Active Timer from Config file is %d",app_configuration.test_active_timer);
    PSM_TEST_LOG_VERBOSE("GPS Debug Sleep from Config file is %d",app_configuration.gps_debug_sleep);
    util_config_unload(config_list);

#if PSM_KERNEL_APP_FULL
    if(app_configuration.usecase > 0)
    {
        /* 1,2,3 and 4 correspond to GPS Usecases */
        app_configuration.is_location_scenario = TRUE;
    }
#endif
    /* Set the valid flag True */
    app_configuration.valid = TRUE;
}

#if PSM_KERNEL_APP_FULL
void psm_test_evaluate_update_psm_cycle(void)
{
    int         count = 0;
    int         fp = INVALID_FILE_HANDLE;
    char        buffer[32] = {0};
    char        str[32] = {0};

    if ( app_configuration.data_event_freq > 0 )
    {
        PSM_TEST_LOG_VERBOSE("Valid Data event frequency: %d", app_configuration.data_event_freq);

        fp =  efs_open( PSM_AUTOMATE_TEST_COUNT_FILE, O_RDONLY);
        if ( INVALID_FILE_HANDLE == fp )
        {
            PSM_TEST_LOG_VERBOSE("No count file.. Starting counter");
            count = 1; //First PSM cycle after creating the count file;
        }
        else
        {
            efs_read(fp, buffer, 32);
            count = atoi(buffer);
            if(count <= 0)
            {
                count = 1;
            }
        }
        efs_close(fp);
        fp = INVALID_FILE_HANDLE;
    
        if ( count == app_configuration.data_event_freq )
        {
            app_configuration.is_data_event_cycle = TRUE;
            count = 0;
        }
        else
        {
            app_configuration.is_data_event_cycle = FALSE;
        }
    
        efs_unlink(PSM_AUTOMATE_TEST_COUNT_FILE);
        fp = efs_open(PSM_AUTOMATE_TEST_COUNT_FILE, O_WRONLY|O_CREAT);
        if ( INVALID_FILE_HANDLE != fp )
        {
            snprintf(str, sizeof(str), "%d", count+1);
            efs_write(fp, str, strlen(str));
        }
        efs_close(fp);
        fp = INVALID_FILE_HANDLE;
    }
    else
    {
        app_configuration.data_event_freq = PSM_DEFAULT_DATA_EVENT_FREQUENCY;
        app_configuration.is_data_event_cycle = TRUE;
        PSM_TEST_LOG_VERBOSE("Invalid count. Setting default: %d", app_configuration.data_event_freq);
    }
}
#endif

void psm_test_check_wait_for_service(void)
{
    PSM_TEST_LOG_INFO("Wait for service: Entry");
    psm_test_critical_ulog(0,"Wait for service: Entry");

    /* Check and wait for Network Full Service here */
    do
    {
        ril_platform_thread_mutex_lock(&mutex);
        if ( nw_full_service == TRUE )
        {
            ril_platform_thread_mutex_unlock(&mutex);
            break;
        }
        else
        {
            // Wait here till service is available.
            ril_platform_thread_cond_wait(&cond, &mutex);
        }
        ril_platform_thread_mutex_unlock(&mutex);
    }while (1);

    psm_test_critical_ulog(0,"Wait for service: Exit");
    PSM_TEST_LOG_INFO("Wait for service: Exit");
}

int psm_test_auto(void)
{
    int             result = FAIL;

    PSM_TEST_LOG_VERBOSE("\n\n========= Automated Test Application =========");
    psm_test_critical_ulog(0,"Auto test app entered");
    PSM_TEST_LOG_VERBOSE("Cycle Number: %d ", psm_cycles_count+1);
    psm_test_critical_ulog(1,"Cycle Number: %d ", psm_cycles_count+1);

    if(!app_configuration.valid)
    {
        /* Bootup cycle. Need to read configurations */
        psm_test_read_config_file();
    }

    if(client_id < 0)
    {
        /* Bootup cycle. Need to register with PSM Daemon */
        result = psm_test_register_application();
        if(app_configuration.gps_debug_sleep)
        {
            PSM_TEST_LOG_VERBOSE("GPS Debug Sleep bootup delay before start of PSM Application");
            PSM_TEST_LOG_VERBOSE("===GPS Debug Sleep start===");
            ril_platform_thread_sleep((app_configuration.gps_debug_sleep)*SLEEP_1S);
            PSM_TEST_LOG_VERBOSE("===GPS Debug Sleep end===");
        }

        PSM_TEST_LOG_VERBOSE("Register application: %d", result);
        psm_test_critical_ulog(0,"Auto test app registered");
    }

#if PSM_KERNEL_APP_FULL
    psm_test_evaluate_update_psm_cycle();
    if( TRUE == app_configuration.is_modem_required )
    {
#endif
        PSM_TEST_LOG_VERBOSE("Check and Perform PSM_Test Initialization");
        if(psm_init_complete == FALSE)
        {
            /* Once per bootup registrations*/
            psm_test_init(); /* Registers for QMI Indications and Data handles */
            psm_init_complete = TRUE;
        }
        else
        {
            /* Non Bootup cycle. Just GET the sys-info */
            psm_test_fetch_network_state();
        }

        PSM_TEST_LOG_VERBOSE("Wait for Device to acquire LTE service");
        psm_test_check_wait_for_service();

#if PSM_KERNEL_APP_FULL
        if( TRUE == app_configuration.is_data_event_cycle )
        {
            PSM_TEST_LOG_VERBOSE("Data event cycle");
            PSM_TEST_LOG_VERBOSE("Cycle Number: %d and Data Connection state: %d", psm_cycles_count+1, data_connected);
            psm_test_critical_ulog(2,"Cycle Number: %d and Data Connection state: %d", psm_cycles_count+1, data_connected);
            if(data_connected == FALSE)
            {
                psm_test_start_lte_data_call();
            }
            else
            {
                /* Data connection already available in this cycle */
                psm_test_perform_app_event();
                psm_test_enter_psm_auto();
            }
        }
        else
        {
#endif
            PSM_TEST_LOG_VERBOSE("Not a Data event cycle. No Data call");
            psm_test_enter_psm_auto();
#if PSM_KERNEL_APP_FULL
        }
    }
    else
    {
        PSM_TEST_LOG_VERBOSE("Modem not required in this cycle. Send ready");
        psm_test_enter_psm_auto();
    }
#endif

    while (!psm_complete)
    {
        ril_platform_thread_cond_wait(&c_cond, &c_mutex);
    }
    PSM_TEST_LOG_VERBOSE("Exiting as PSM is complete");
    return result;
}

int psm_test_task_entry
(
    dword ignored
)
{
    int         ret  = 0;
    int         fp_auto = INVALID_FILE_HANDLE;
    int         fp_legacyauto = INVALID_FILE_HANDLE;
    int         fp_dam = INVALID_FILE_HANDLE;
    int         fp_llvm_dam = INVALID_FILE_HANDLE;
    qurt_addr_t module_phys_base = NULL;
    char *      dam_bin_path =     NULL;
    /* Inform RC_INIT that task started */
    rcinit_handshake_startup();

    psm_test_critical_ulog_init();
    psm_test_initialize_configuration();
    ril_platform_thread_mutex_init(&mutex, NULL);
    ril_platform_thread_mutex_init(&c_mutex, NULL);
    ril_platform_thread_mutex_init(&data_connection_mutex, NULL);
#if PSM_KERNEL_APP_FULL
    ril_platform_thread_mutex_init(&gpsresp_mutex, NULL);
    ril_platform_thread_mutex_init(&gpstrack_mutex, NULL);
#endif
    ril_platform_thread_cond_init(&cond, 0);
    ril_platform_thread_cond_init(&c_cond, 0);
#if PSM_KERNEL_APP_FULL
    ril_platform_thread_cond_init(&gpsresp_cond, 0);
    ril_platform_thread_cond_init(&gpstrack_cond, 0);
#endif
    fp_dam = efs_open( PSM_DAMTEST_MODULE_FILE, O_RDONLY);
    fp_llvm_dam = efs_open( PSM_LLVM_DAMTEST_MODULE_FILE, O_RDONLY);
    // efs_access is not supported on EFS2, so use efs_open
    fp_auto =  efs_open( PSM_AUTOMATE_TEST_FILE, O_RDONLY);
    fp_legacyauto =  efs_open( PSM_LEGACY_TEST_FILE, O_RDONLY);
    if(fp_dam != INVALID_FILE_HANDLE || fp_llvm_dam != INVALID_FILE_HANDLE)
    {
        if(fp_auto != INVALID_FILE_HANDLE || fp_legacyauto != INVALID_FILE_HANDLE)
        {
            psm_test_critical_ulog(0,"Auto damtest app enabled");
            PSM_TEST_LOG_VERBOSE("Auto damtest app enabled");
        }
        else
        {
            /*neither dam_test_app nor legacy_test_app can start*/
            psm_test_critical_ulog(1,"PSM Test application is not enabled in EFS. Exiting");
            PSM_TEST_LOG_VERBOSE("PSM Test application is not enabled in EFS. Exiting");
            return 0;
        }

        if(fp_dam != INVALID_FILE_HANDLE)
        {
            dam_bin_path = PSM_DAMTEST_MODULE_FILE;
            efs_close(fp_dam);
            if(fp_llvm_dam != INVALID_FILE_HANDLE)
            {
                efs_close(fp_llvm_dam);
            }
        }
        else
        {
            dam_bin_path = PSM_LLVM_DAMTEST_MODULE_FILE;
            efs_close(fp_llvm_dam);    
        }

        if(fp_auto != INVALID_FILE_HANDLE)
        {
            efs_close(fp_legacyauto); 
        }

        if(fp_legacyauto != INVALID_FILE_HANDLE)
        {
            efs_close(fp_legacyauto);    
        }

        /* load the module from PSM DAMTEST FILE */
        psm_test_critical_ulog(0,"Starting DAM PSMTEST");
        PSM_TEST_LOG_VERBOSE("Dam bin path %s",dam_bin_path);
        psm_test_critical_ulog(0,"Dam bin path %s",dam_bin_path);
        ret = _txm_module_manager_file_load(&dam_psmtest, "PSM DAM TEST", NULL, dam_bin_path);
        if (ret != TX_SUCCESS)
        {
            psm_test_critical_ulog(1,"Exiting since module load return: %d", ret);
            return 0;
        }

        psm_test_critical_ulog(1,"return value for DAM load: %d", ret);

// BSP Provided Customization as is
#ifdef TX_DAM_QC_CUSTOMIZATIONS
        ret = qurt_txm_module_manager_setup_module_mmu(&dam_psmtest);
        if(ret != QURT_EOK)
        {
            psm_test_critical_ulog(1,"Exiting. Unable to setup MMU: %d", ret);
            return 0;
        }

        {
            qurt_size_t size = dam_psmtest.txm_module_instance_total_ram_usage;
            /* Align to page size of 4KB */
            size = (size + 0xfff) & ~0xfff;
            module_phys_base = qurt_lookup_physaddr((qurt_addr_t)dam_psmtest.txm_module_instance_phys_base);
            ret = qurt_mapping_create_with_asid(
                        (qurt_addr_t)dam_psmtest.txm_module_instance_virt_base,
                         module_phys_base, size, QURT_MEM_CACHE_WRITEBACK,
                         QURT_PERM_FULL,dam_psmtest.txm_module_instance_asid);

            if(ret != QURT_EOK)
            {
                psm_test_critical_ulog(1,"Exiting. Unable to map vrtl memory: %d", ret);
                return 0;
            }

            ret = qurt_mapping_create( (qurt_addr_t)dam_psmtest.txm_module_instance_virt_base,
                                     module_phys_base,
                                     size, QURT_MEM_CACHE_WRITEBACK, QURT_PERM_FULL);

            if(ret != QURT_EOK)
            {
                psm_test_critical_ulog(1,"Exiting. Unable to create memory map for DAM: %d", ret);
                return 0;
            }
        }
#endif

        psm_test_critical_ulog(0,"Executing start module");
        /* Start the module.  */
        ret = txm_module_manager_start(&dam_psmtest);
        if (ret != TX_SUCCESS)
        {
            psm_test_critical_ulog(1,"Exiting since module start return: %d", ret);
            return 0;
        }

        psm_test_critical_ulog(1,"Return value for start: %d", ret);

    }
    else if (fp_auto != INVALID_FILE_HANDLE || fp_legacyauto != INVALID_FILE_HANDLE )
    {
        psm_test_critical_ulog(0,"Auto test app enabled");
        if(fp_legacyauto != INVALID_FILE_HANDLE)
        {
             efs_close(fp_legacyauto);
        }
        
        if(fp_auto != INVALID_FILE_HANDLE)
        {
            efs_close(fp_auto);
        }

        do
        {
            psm_test_auto();

            /* Sleep for the voted amount of time */
            if(app_configuration.time_in_psm <= 0)
            {
                app_configuration.time_in_psm = PSM_DEFAULT_TEST_PSM_TIME;
            }
            if(fake_psm_time <= 0)
            {
                ril_platform_thread_sleep((app_configuration.time_in_psm)*SLEEP_1S);
            }
            else
            {
                /* Sleeping in GPS cases for Fake PSM Times */
                ril_platform_thread_sleep((fake_psm_time)*SLEEP_1S);
            }

            /* Post wakeup, cancel the previous stale PSM Vote */
            psm_cancel_power_save(client_id);
            psm_cycles_count++;
        } while(TRUE); /* Run this in loop for Reject & Deepsleep cases */

    }
    else
    {
        psm_test_critical_ulog(1,"PSM Test application is not enabled in EFS. Exiting");
    }

    return 0;
}

/*========================================================================================
FUNCTION psm_data_test_task_entry

Entry function for PSM test data event thread.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_data_test_task_entry
(
    dword ignored
)
{
#if PSM_KERNEL_APP_FULL

    ril_platform_pipe_attr_t attr;
    psm_data_test_event_params_type    buffer;
#endif
    rcinit_handshake_startup();
#if PSM_KERNEL_APP_FULL
    do
    {
        /* Create a pipe */
        ril_platform_pipe_attr_init(&attr);
        ril_platform_pipe_attr_set_elements(&attr, PSM_DATA_EVENT_PIPE_MAX_ELEMENTS);
        ril_platform_pipe_attr_set_element_size(&attr, sizeof(buffer));
        if ( 0 != ril_platform_pipe_create (&data_test_pipe, &attr) )
        {
            PSM_TEST_LOG_INFO("Error creating data test pipe");
            break;
        }

        while(1)
        {
            /* Block waiting for a event to be put on the queue */
            ril_platform_pipe_receive (data_test_pipe, &buffer);
            psm_test_process_data_test_event(&buffer);
        }

    } while(FALSE);
#endif
    return 0;
}


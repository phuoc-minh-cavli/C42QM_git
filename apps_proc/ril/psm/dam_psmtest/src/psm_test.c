/******************************************************************************
#@file    psm_test.c
#@brief   Provides DAM interface for testing PSM functionality
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
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
#include <stdarg.h>
#include <stringl/stringl.h>
#include <stdbool.h>

#include "qapi_psm.h"
#include "qapi_psm_types.h"
#include "qapi_psm_status.h"
#include "qapi_fs.h"

#include "qapi_dss.h"
#include "qapi_socket.h"
#include "qapi_netservices.h"
#include "qapi_diag.h"
#include "qapi_diag_codes.h"
#include "qapi_diag_types.h"
#include "qapi_timer.h"
#include "qapi_location.h"
#include "qapi_tlmm.h"
#include "qapi_device_info.h"

/*===========================================================================

                           DEFINES

===========================================================================*/

#define PSM_AUTOMATE_TEST_FILE           "/ril/psm/enable_auto_test"
#define PSM_LEGACY_TEST_FILE             "/ril/psm/enable_legacy_test"
#define PSM_AUTOMATE_TEST_COUNT_FILE     "/ril/psm/psm_count"
#define PSM_DAM_TEST_LOG_FILE            "/ril/psm/damlog.txt"

#define TRUE                              1
#define FALSE                             0
#define SUCCESS                           0
#define FAIL                             -1
#define NIL                               0

#define SLEEP_10MS  (10)
#define SLEEP_100MS (10*SLEEP_10MS)
#define SLEEP_1S    (100*SLEEP_10MS)


#define PSM_DEFAULT_DATA_EVENT_FREQUENCY  1
#define PSM_DEFAULT_TEST_ACTIVE_TIMER     0
#define PSM_DEFAULT_TEST_PSM_TIME        (150)
#define PSM_DEFAULT_PING_PACKET_SIZE      1000
#define PSM_PING_MAX_PACKET_SIZE         (1500)
#define PSM_MAX_LOG_MSG_SIZE              100
#define PSM_TEST_DEFAULT_GPIO_PIN         (52)

#define PSM_GPS_USER_BUFFER_SIZE         (128)
#define PSM_DEFAULT_GPS_TIME_INTERVAL    (30)
#define PSM_DEFAULT_GPS_MIN_DISTANCE     (0)
#define PSM_DEFAULT_GPS_SAMPLE_COUNT     (5)
#define PSM_DEFAULT_DEEP_SLEEP_TIME      (50)



#define PSM_TEST_LOG_DEBUG(...)   RIL_LOG_MSG(MSG_LEGACY_HIGH, __VA_ARGS__ )
#define PSM_TEST_LOG_ERROR(...)   RIL_LOG_MSG(MSG_LEGACY_ERROR, __VA_ARGS__ )
#define PSM_TEST_LOG_FATAL(...)   RIL_LOG_MSG(MSG_LEGACY_ERROR, __VA_ARGS__ )
#define PSM_TEST_LOG_VERBOSE(...) RIL_LOG_MSG(MSG_LEGACY_LOW, __VA_ARGS__ )
#define PSM_TEST_LOG_INFO(...)    RIL_LOG_MSG(MSG_LEGACY_MED, __VA_ARGS__ )

#define PSM_QMI_TEST_TIMEOUT             (5000)
#define PSM_DATA_EVENT_PIPE_MAX_ELEMENTS  4
#define PSM_TEST_ULOG_FILE_NAME          "PSM_TEST"
#define PSM_TEST_ULOG_BUF_SIZE            16384
#define INVALID_FILE_HANDLE              (-1)
#define PSM_DATA_STACK_SIZE               4098
#define PSMT_BYTE_POOL_SIZE               17408
#define PSMT_NO_NET_MASK                  0x00000001
#define PSMT_CONN_NET_MASK                0x00000010

void psm_format_log_msg
(
    char *buf_ptr,
    int buf_size,
    char *fmt,
    ...
);


/* Log message to Diag */
#define RIL_LOG_MSG( lvl, fmt, ... )                                           \
    {                                                                            \
        char log_fmt[ PSM_MAX_LOG_MSG_SIZE ]; \
        char log_buf[ PSM_MAX_LOG_MSG_SIZE ]; \
        int written = 0;\
        strlcpy(log_fmt, "DAM_%s: ", sizeof(log_fmt));\
        strlcat(log_fmt, fmt, sizeof(log_fmt));\
        psm_format_log_msg( log_buf, PSM_MAX_LOG_MSG_SIZE, log_fmt, __func__, ##__VA_ARGS__ );\
        QAPI_MSG_SPRINTF( MSG_SSID_ANDROID_QCRIL, lvl, "%s", log_buf );\
        strlcat(log_buf, "\n", sizeof(log_buf));\
        if( file_log_fd > -1 )\
        {\
            qapi_FS_Write(file_log_fd,log_buf,strlen(log_buf),&written);\
        }\
    }

/*===========================================================================

                           GLOBALS

===========================================================================*/

int               client_id = -1;
int               file_log_fd = -1;
int               is_modem_loaded = FALSE;
int               health_check_fail = FALSE;
int               backoff = FALSE;
int               psm_complete = FALSE;
int               nw_full_service = FALSE;
int               psm_init_complete = FALSE;              /* Flag to record Init call*/
int               fake_psm_time=-1;
int               psm_cycles_count = 0;
int               data_connected = FALSE;
int               gps_samples = 0;

int                psm_dam_test_gpio_initialized = FALSE;
qapi_GPIO_ID_t     psm_dam_test_gpio_id;
qapi_TLMM_Config_t psm_dam_test_tlmm_config;

qapi_DSS_Hndl_t           dss_handle;
qapi_Location_Callbacks_t sLocationCallbacks;
qapi_loc_client_id        gpsid;


static uint8_t      psmGpsUserBuffer[PSM_GPS_USER_BUFFER_SIZE];
int                 ping_size = PSM_DEFAULT_PING_PACKET_SIZE;
psm_status_msg_type cb_msg;

TX_MUTEX               *mutex;
TX_EVENT_FLAGS_GROUP   *cond;

TX_MUTEX               *c_mutex;
TX_EVENT_FLAGS_GROUP   *c_cond;

TX_MUTEX               *data_mutex;
TX_EVENT_FLAGS_GROUP   *data_cond;

TX_THREAD              *psm_data_thread;

TX_MUTEX               *gpsresp_mutex;
TX_MUTEX               *gpstrack_mutex;

TX_EVENT_FLAGS_GROUP   *gpsresp_cond;
TX_EVENT_FLAGS_GROUP   *gpstrack_cond;

TX_MUTEX               *data_connection_mutex;



/*===========================================================================

                           Static & global Variable Declarations

===========================================================================*/

UCHAR         psmt_stack[PSM_DATA_STACK_SIZE];
TX_THREAD     psm_data_thread_handle;
ULONG         free_memory_psmt[PSMT_BYTE_POOL_SIZE];
ULONG         free_memory_psmt_data[PSMT_BYTE_POOL_SIZE];
TX_BYTE_POOL *byte_pool_psmt;
TX_BYTE_POOL *byte_pool_psmt_data;

qapi_Device_Info_Hndl_t device_handle = {NULL};
typedef struct
{
  int    err;
  char * err_name;
}psm_err_map_type;

typedef struct
{
  qapi_DSS_Net_Evt_t         evt;
  void                 *data;
  qapi_DSS_Hndl_t            hndl;
} psm_data_test_event_params_type;

typedef struct
{
  int               valid;             /* Is the configuration valid */
  char              apn[128];          /* APN to use with Data call */
  char              ip_addr[128];      /* IP to send the Ping/Data to */
  int               data_event_freq;   /* Nth cycle in which Data events are performed */
  int               is_data_event_cycle; /* Is current cycle involves app event */
  int               test_active_timer; /* Active Time for PSM Voting */
  int               time_in_psm;       /* Time in PSM  */
  int               is_modem_required; /* Is modem required from next cycle onwards */
  int               is_data_success_required; /* Is data event SUCCESS mandatory */
  int               ping_size;         /* Size of Ping */
  int               is_location_scenario; /* Is current cycle for Location event */
  int               gps_min_distance;  /* Min GPS distance for tracking */
  int               gps_time_interval; /* Is the configuration valid */
  int               gps_samples;       /* Number of GPS Samples to take in current cycle */
  int               gps_usecase;           /* Use case number. Refer to Use cases table below */
  int               fake_time_in_psm;  /* Fake time to send APP to sleep in Deepsleep cases */
  int               gps_debug_sleep;   /* Flag to enable time delays in GPS Usecases for Debugging*/
  int               gpio_enabled;      /* Flag to enalbe GPIO toggling for Data events*/
  int               gpio_pin;          /* GPIO Pin number for toggling */
  int               is_ipv6;           /* IP address type*/
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


#define DAM_INTEGER 0
#define DAM_STRING 1
#define ASSERT_FATAL 1

unsigned long __stack_chk_guard __attribute__((weak)) = 0xDEADDEAD;

psm_test_configuration_type  app_configuration;

void __attribute__((weak)) __stack_chk_fail (void)
{
    return;
}

int myAtoi(char *str)
{
    int res = 0;
    int i = 0;

    for (i = 0; (str[i] != '\0') && (str[i] <= '9') && (str[i] >= '0'); ++i)
        res = res*10 + str[i] - '0';

    return res;
}


UINT memscpy(
              void   *dst,
              UINT   dst_size,
              const  void  *src,
              UINT   src_size
              )
    {
      UINT  copy_size = (dst_size <= src_size)? dst_size : src_size;
      memcpy(dst, src, copy_size);
      return copy_size;
    }

int dam_config_load(const char *file)
{
    int fd = -1;

    qapi_FS_Open(file, QAPI_FS_O_RDONLY_E, &fd);

    return fd;
}

void dam_config_get_value(int fd, const char *identifier, int type, void* value)
{
    //PSM_TEST_LOG_VERBOSE("Identifier %s",identifier);

    char file_contents[1000] = "";
    char * token;
    char * needle;
    char *saveptr;
    char sub_value[128] = "";
    int  read_length = 0;
    qapi_FS_Offset_t seek_status = 0;
    int i=0;

    do
    {
        if(fd < 0 )
        {
            break;
        }

        qapi_FS_Seek(fd,0, QAPI_FS_SEEK_SET_E, &seek_status);

        qapi_FS_Read(fd, file_contents, 1000, &read_length);
        token = strtok_r(file_contents, "\n", &saveptr);

        while( token != NULL )
        {
            if( strstr(token, identifier) != NULL )
            {
                needle = strstr(token, identifier);
                needle = needle + strlen(identifier);
                //PSM_TEST_LOG_VERBOSE("Needle  %s",needle);
                strlcpy(sub_value,needle,128);
                switch(type)
                {
                    case DAM_STRING:
                        for(i=0;i<sizeof(sub_value);i++)
                            if((int)sub_value[i] < 32)
                                sub_value[i] = 0;
                        strlcpy(value, sub_value, sizeof(sub_value));

                        break;

                    case DAM_INTEGER:
                       *((int*)value) = myAtoi(sub_value);
                        //PSM_TEST_LOG_VERBOSE("Identifier %s Val %s",identifier,sub_value);
                        break;
                }
                break;
            }

            token = strtok_r(NULL, "\n", &saveptr);
        }



    } while(false);

}

void dam_config_unload(int fd)
{
    qapi_FS_Close(fd);
}

int dam_thread_cond_wait_and_clear(TX_MUTEX *mutex, TX_EVENT_FLAGS_GROUP *cond)
{
    int ret = 0;
    ULONG   actual_flags;

    tx_mutex_put(mutex);
    ret = tx_event_flags_get(cond, 0x1, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
    tx_mutex_get(mutex,TX_WAIT_FOREVER);

    return ret;
}

void psm_dam_test_read_config_file(void)
{
    int fd = -1;
    fd = dam_config_load(PSM_AUTOMATE_TEST_FILE);
    if(fd == -1)
    {
        fd = dam_config_load(PSM_LEGACY_TEST_FILE);
        PSM_TEST_LOG_VERBOSE("Loading PSM_LEGACY_TEST_FILE file");
        if(fd == -1)
        {
            PSM_TEST_LOG_VERBOSE("Dam auto test file not opened");
        }
    }

    PSM_TEST_LOG_VERBOSE("loading PSM_AUTOMATE_TEST_FILE file");
    dam_config_get_value(fd, "IP=", DAM_STRING, app_configuration.ip_addr);
    dam_config_get_value(fd, "APN=", DAM_STRING, app_configuration.apn);
    dam_config_get_value(fd, "DATA_EVENT_FREQUENCY=", DAM_INTEGER, &app_configuration.data_event_freq);
    dam_config_get_value(fd, "ACTIVE_TIMER=", DAM_INTEGER, &app_configuration.test_active_timer);
    dam_config_get_value(fd, "TIME_IN_PSM=", DAM_INTEGER, &app_configuration.time_in_psm);
    dam_config_get_value(fd, "MODEM_REQUIRED=", DAM_INTEGER, &app_configuration.is_modem_required);
    dam_config_get_value(fd, "DATA_SUCCESS_REQUIRED=", DAM_INTEGER, &app_configuration.is_data_success_required);
    dam_config_get_value(fd, "PING_SIZE=", DAM_INTEGER, &app_configuration.ping_size);
    dam_config_get_value(fd, "GPS_MEASURE=", DAM_INTEGER, &app_configuration.is_location_scenario);
    dam_config_get_value(fd, "GPS_INTERVAL=", DAM_INTEGER, &app_configuration.gps_time_interval);
    dam_config_get_value(fd, "GPS_DISTANCE=", DAM_INTEGER, &app_configuration.gps_min_distance);
    dam_config_get_value(fd, "GPS_SAMPLES=", DAM_INTEGER, &app_configuration.gps_samples);
    dam_config_get_value(fd, "GPS_USECASE=", DAM_INTEGER, &app_configuration.gps_usecase);
    dam_config_get_value(fd, "FAKE_TIME_IN_PSM=", DAM_INTEGER, &app_configuration.fake_time_in_psm);
    dam_config_get_value(fd, "GPS_DEBUG=", DAM_INTEGER, &app_configuration.gps_debug_sleep);
    dam_config_get_value(fd, "GPIO_ENABLE=", DAM_INTEGER, &app_configuration.gpio_enabled);
    dam_config_get_value(fd, "GPIO_PIN=", DAM_INTEGER, &app_configuration.gpio_pin);
    dam_config_get_value(fd, "IS_IPV6=", DAM_INTEGER, &app_configuration.is_ipv6);


    PSM_TEST_LOG_VERBOSE("Data Event Frequency from Config file is %d",app_configuration.data_event_freq);
    PSM_TEST_LOG_VERBOSE("Active Timer from Config file is %d",app_configuration.test_active_timer);
    PSM_TEST_LOG_VERBOSE("IP Address from Config file is %s",app_configuration.ip_addr);
    PSM_TEST_LOG_VERBOSE("APN from Config file is %s",app_configuration.apn);
    PSM_TEST_LOG_VERBOSE("Time in PSM configured is %d",app_configuration.time_in_psm);
    PSM_TEST_LOG_VERBOSE("Modem required configured is %d",app_configuration.is_modem_required);
    PSM_TEST_LOG_VERBOSE("Data success configured is %d",app_configuration.is_data_success_required);
    PSM_TEST_LOG_VERBOSE("Ping size from Config file is %d",app_configuration.ping_size);
    PSM_TEST_LOG_VERBOSE("GPS Measure from Config file is %d",app_configuration.is_location_scenario);
    PSM_TEST_LOG_VERBOSE("GPS Interval from Config file is %d",app_configuration.gps_time_interval);
    PSM_TEST_LOG_VERBOSE("GPS Distance from Config file is %d",app_configuration.gps_min_distance);
    PSM_TEST_LOG_VERBOSE("GPS Samples from Config file is %d",app_configuration.gps_samples);
    PSM_TEST_LOG_VERBOSE("GPS Usecase from Config file is %d",app_configuration.gps_usecase);
    PSM_TEST_LOG_VERBOSE("Fake time in psm from Config file is %d",app_configuration.fake_time_in_psm);
    PSM_TEST_LOG_VERBOSE("GPS Debug Sleep Flag is %d",app_configuration.gps_debug_sleep);
    PSM_TEST_LOG_VERBOSE("GPIO Enabled is %d",app_configuration.gpio_enabled);
    PSM_TEST_LOG_VERBOSE("GPIO Pin is %d",app_configuration.gpio_pin);
    PSM_TEST_LOG_VERBOSE("Is it ipv6 address %d",app_configuration.is_ipv6);

    dam_config_unload(fd);

    if(app_configuration.gps_usecase > 0)
    {
        /* 1,2,3 and 4 correspond to GPS Usecases */
        PSM_TEST_LOG_VERBOSE("Location Scenario");
        app_configuration.is_location_scenario = TRUE;
    }

    if(app_configuration.gpio_enabled < 0 || app_configuration.gpio_enabled > 1)
    {
        PSM_TEST_LOG_VERBOSE("Invalid value for gpio enablement. Disabling.");
        app_configuration.gpio_enabled = FALSE;
    }

    /* Set the valid flag True */
    app_configuration.valid = TRUE;

    //qapi_FS_Unlink(PSM_AUTOMATE_DAMTEST_FILE);
}

/*** GPIO Functions BEGIN ***/

void psm_dam_test_gpio_init(void)
{
    qapi_Status_t status = QAPI_OK;

    PSM_TEST_LOG_VERBOSE("Entered GPIO Initialization");

    if((TRUE == app_configuration.gpio_enabled) &&
                            (psm_dam_test_gpio_initialized == FALSE))
    {
        psm_dam_test_tlmm_config.pin = app_configuration.gpio_pin;
        psm_dam_test_tlmm_config.func = 0x0;
        psm_dam_test_tlmm_config.dir = QAPI_GPIO_OUTPUT_E;
        psm_dam_test_tlmm_config.pull= QAPI_GPIO_NO_PULL_E;
        psm_dam_test_tlmm_config.drive = QAPI_GPIO_2MA_E;

        status = qapi_TLMM_Get_Gpio_ID(&psm_dam_test_tlmm_config, &psm_dam_test_gpio_id);
        if(QAPI_OK == status)
        {
            status = qapi_TLMM_Config_Gpio(psm_dam_test_gpio_id, &psm_dam_test_tlmm_config);
            PSM_TEST_LOG_INFO("Configuring PSM GPIO result: %d", status);
            if(QAPI_OK == status)
            {
                psm_dam_test_gpio_initialized = TRUE;
            }
        }
    }
}

void psm_dam_test_gpio_set(qapi_GPIO_Value_t eValue)
{
    qapi_Status_t status = QAPI_OK;


    PSM_TEST_LOG_VERBOSE("Entered GPIO Set");

    if((TRUE == app_configuration.gpio_enabled) && (psm_dam_test_gpio_initialized))
    {
        status = qapi_TLMM_Drive_Gpio(psm_dam_test_gpio_id, app_configuration.gpio_pin, eValue);
        PSM_TEST_LOG_INFO("Setting PSM GPIO value: %d, result: %d", eValue, status);
    }
}


/*** GPIO Functions END***/

int psm_dam_test_start_lte_data_call(void)
{
    qapi_DSS_Call_Param_Value_t param_info;
    int temp;
    PSM_TEST_LOG_VERBOSE("LTE Data call function entered");

    if ( dss_handle != NULL )
    {
        /* set data call param */
        param_info.buf_val = NULL;
        param_info.num_val = QAPI_DSS_RADIO_TECH_UNKNOWN;
        PSM_TEST_LOG_VERBOSE("Setting tech to Automatic");
        qapi_DSS_Set_Data_Call_Param(dss_handle, QAPI_DSS_CALL_INFO_TECH_PREF_E, &param_info);


        param_info.buf_val = app_configuration.apn;
        param_info.num_val = strlen(app_configuration.apn);
        PSM_TEST_LOG_VERBOSE("Setting APN - %s", app_configuration.apn);
        PSM_TEST_LOG_VERBOSE("APN length - %d", param_info.num_val);
        qapi_DSS_Set_Data_Call_Param(dss_handle, QAPI_DSS_CALL_INFO_APN_NAME_E, &param_info);

        param_info.buf_val = NULL;
        if(app_configuration.is_ipv6==0){
            param_info.num_val = QAPI_DSS_IP_VERSION_4;
        }
        if(app_configuration.is_ipv6==1){
            param_info.num_val = QAPI_DSS_IP_VERSION_6;
        }
        PSM_TEST_LOG_VERBOSE("Setting family to IP");
        qapi_DSS_Set_Data_Call_Param(dss_handle, QAPI_DSS_CALL_INFO_IP_VERSION_E, &param_info);

        /* set data call param for UMTS*/
        param_info.buf_val = NULL;
        param_info.num_val = 1;
        PSM_TEST_LOG_VERBOSE("setting profile number to: %d", 1);
        qapi_DSS_Set_Data_Call_Param(dss_handle, QAPI_DSS_CALL_INFO_UMTS_PROFILE_IDX_E, &param_info);

        PSM_TEST_LOG_VERBOSE("Toggling GPIO before starting LTE Data call");
        psm_dam_test_gpio_set(QAPI_GPIO_HIGH_VALUE_E);

        temp = qapi_DSS_Start_Data_Call(dss_handle);
        PSM_TEST_LOG_VERBOSE("Start data call executed: %d", temp);
    }
    else
    {
        PSM_TEST_LOG_ERROR("dss_handle is NULL");
    }
    return 0;
}

void psm_dam_test_app_call_back_func(psm_status_msg_type *psm_status)
{
    PSM_TEST_LOG_INFO("Callback Entered");
    if ( psm_status )
    {
        // to avoid fludding of logs, don't print health check.
//        if ( psm_status->status != PSM_STATUS_HEALTH_CHECK &&
//             psm_status->status != PSM_STATUS_NONE )
//        {
            PSM_TEST_LOG_VERBOSE("PSM STATUS:    %s", status_string[psm_status->status].err_name);
            PSM_TEST_LOG_VERBOSE("REJECT REASON: %s\n", reject_string[psm_status->reason].err_name);
//        }

        switch(psm_status->status)
        {
            // special handling for some cases like modem loaded, not loaded etc.
            case PSM_STATUS_MODEM_LOADED:
                is_modem_loaded = TRUE;
                break;

            case PSM_STATUS_MODEM_NOT_LOADED:
                is_modem_loaded = FALSE;
                break;

            case PSM_STATUS_HEALTH_CHECK:
                if ( !health_check_fail )
                {
                    qapi_PSM_Client_Hc_Ack(client_id);
                }
                break;

            case PSM_STATUS_NW_OOS:
                if ( backoff )
                {
                    qapi_PSM_Client_Enter_Backoff(client_id);
                }
                break;

            case PSM_STATUS_NW_LIMITED_SERVICE:
                if ( backoff )
                {
                    qapi_PSM_Client_Enter_Backoff(client_id);
                }
                break;

            case PSM_STATUS_COMPLETE:
                tx_mutex_get(c_mutex, TX_WAIT_FOREVER);
                psm_complete = TRUE;
                tx_event_flags_set(c_cond, 0x1, TX_OR);
                tx_mutex_put(c_mutex);
                PSM_TEST_LOG_VERBOSE("Received PSM complete");
                break;

            default:
                break;
        }
    }
    PSM_TEST_LOG_INFO("Callback Exit");
}


void psm_dam_test_net_event_cb( qapi_DSS_Hndl_t hndl,
                                     void * user_data,
                                     qapi_DSS_Net_Evt_t evt,
                                     qapi_DSS_Evt_Payload_t *payload_ptr )
{

    psm_data_test_event_params_type  params;

    memset(&params, 0, sizeof(params));
    params.evt = evt;
    params.hndl = hndl;

    tx_mutex_get(data_mutex, TX_WAIT_FOREVER);

    switch( params.evt )
    {
        case QAPI_DSS_EVT_NET_NO_NET_E:
            tx_event_flags_set(data_cond, PSMT_NO_NET_MASK, TX_OR);
            break;

        case QAPI_DSS_EVT_NET_IS_CONN_E:
            tx_event_flags_set(data_cond, PSMT_CONN_NET_MASK, TX_OR);
            break;

        default:
            break;
    }
    tx_mutex_put(data_mutex);

    PSM_TEST_LOG_VERBOSE("Data test event: %d sent to data test thread.",params.evt);

}

static void psm_dam_test_gps_capabilities_callback(qapi_Location_Capabilities_Mask_t capabilitiesMask)
{
    PSM_TEST_LOG_VERBOSE("GPS API CALLBACK : CAPABILITIES (mask 0x%x)", capabilitiesMask);
}

static void psm_dam_test_gps_response_callback(qapi_Location_Error_t err, uint32_t id)
{
    PSM_TEST_LOG_VERBOSE("GPS API CALLBACK : RESPONSE (err=%u id=%u)", err, id);
    tx_mutex_get(gpsresp_mutex, TX_WAIT_FOREVER);
    tx_event_flags_set(gpsresp_cond, 0x1, TX_OR);
    tx_mutex_put(gpsresp_mutex);
}

static void psm_dam_test_gps_geofence_response_callback(size_t count,
    qapi_Location_Error_t* err,
    uint32_t* ids)
{
    uint32_t i = 0;
    PSM_TEST_LOG_VERBOSE("GPS API CALLBACK : GEOFENCE RESPONSE (count %d)", count);

    for (i = 0; i < count; ++i) {
        PSM_TEST_LOG_VERBOSE("ID[%d] = %d, err[%d] = %d", i, ids[i], i, err[i]);
    }
}

static void psm_dam_test_gps_tracking_callback(qapi_Location_t location)
{
    PSM_TEST_LOG_VERBOSE("GPS API CALLBACK : TRACKING");

    PSM_TEST_LOG_VERBOSE("LAT: %d.%d LON: %d.%d ALT: %d.%d ACC: %d.%d",
        (int)location.latitude, ((int)(location.latitude * 100000)) % 100000,
        (int)location.longitude, ((int)(location.longitude * 100000)) % 100000,
        (int)location.altitude, ((int)(location.altitude * 100)) % 100,
        (int)location.accuracy, ((int)(location.accuracy * 100)) % 100);
    /*psm_test_critical_ulog(8,"LAT: %d.%d LON: %d.%d ALT: %d.%d ACC: %d.%d",
        (int)location.latitude, ((int)(location.latitude * 100000)) % 100000,
        (int)location.longitude, ((int)(location.longitude * 100000)) % 100000,
        (int)location.altitude, ((int)(location.altitude * 100)) % 100,
        (int)location.accuracy, ((int)(location.accuracy * 100)) % 100);*/

    PSM_TEST_LOG_VERBOSE("SPEED: %d.%d BEAR: %d.%d TIME: 0x%x%x FLAGS: %u",
        (int)location.speed, ((int)(location.speed * 100)) % 100,
        (int)location.bearing, ((int)(location.bearing * 100)) % 100,
        location.timestamp >> 32, location.timestamp, location.flags);

    tx_mutex_get(gpstrack_mutex, TX_WAIT_FOREVER);

    gps_samples--;
    PSM_TEST_LOG_VERBOSE("GPS samples remaining: %d",gps_samples);

    if(gps_samples == NIL)
    {
        tx_event_flags_set(gpstrack_cond, 0x1, TX_OR);
    }

    tx_mutex_put(gpstrack_mutex);
}

/**************************************************
*  Gets the device information for specified ID   *
**************************************************/
void psm_dam_test_fetch_network_state(){
    
    int ret= QAPI_OK;
    qapi_Device_Info_t *info = NULL;

    if(NULL == device_handle)
    {
        PSM_TEST_LOG_VERBOSE("Device info not initialized, Please do init first \n");
        return;
    }
    tx_byte_allocate(byte_pool_psmt, (VOID **)&info, sizeof(qapi_Device_Info_t), TX_NO_WAIT);
    if(info == NULL)
    {
        PSM_TEST_LOG_VERBOSE("get_device_info_service_state : malloc failed while allocating qapi_Device_Info_t");
        return;
    }
    memset(info, 0, sizeof(qapi_Device_Info_t));
    ret = qapi_Device_Info_Get_v2(device_handle, QAPI_DEVICE_INFO_SERVICE_STATE_E, info);
    if(ret != QAPI_OK)
    {
        PSM_TEST_LOG_VERBOSE("get_device_info_service_state : returned error %d\n", ret);
        return;
    }
    else
    {
        PSM_TEST_LOG_VERBOSE("get_device_info_service_state: %lld \n",info->u.valueint);
    }

    tx_mutex_get(mutex, TX_WAIT_FOREVER);
    if(info->u.valueint==QAPI_DEVICE_INFO_SRV_STATE_SRV)
    {
        PSM_TEST_LOG_VERBOSE("SERVICE AVAILABLE");
        nw_full_service = TRUE;
    }
    else
    {
        PSM_TEST_LOG_VERBOSE("NO-SERVICE");
    }

    if ( nw_full_service == TRUE )
    {
        tx_event_flags_set(cond, 0x1, TX_OR);
        PSM_TEST_LOG_VERBOSE("Network Full Service: %d", nw_full_service);
    }
    tx_mutex_put(mutex);

    return;
}

/*****************************************
* Callback to handle network indications *
*****************************************/
void device_info_callback(qapi_Device_Info_Hndl_t hndl, const qapi_Device_Info_t *device_info)
{
    if(NULL == device_info || hndl == NULL)
    {
        PSM_TEST_LOG_VERBOSE("device_info_callback: NULL == device_info || hndl == NULL\n");
        return;
    }
    if(device_info->id == QAPI_DEVICE_INFO_NETWORK_IND_E)
        PSM_TEST_LOG_VERBOSE("device_info->id:%d -QAPI_DEVICE_INFO_NETWORK_IND_E \n", device_info->id);
    if(device_info->info_type == QAPI_DEVICE_INFO_TYPE_INTEGER_E)
        PSM_TEST_LOG_VERBOSE("device_info_callback: QAPI_DEVICE_INFO_TYPE_INTEGER_E Val: %d\n\n", device_info->u.valueint);

    tx_mutex_get(mutex, TX_WAIT_FOREVER);
    if(device_info->u.valueint==QAPI_DEVICE_INFO_NW_IND_SRV)
    {
        PSM_TEST_LOG_VERBOSE("SERVICE AVAILABLE");
        nw_full_service=TRUE;
    }
    else
    {
        PSM_TEST_LOG_VERBOSE("NO-SERVICE");
    }
    if ( nw_full_service == TRUE )
    {
        tx_event_flags_set(cond, 0x1, TX_OR);
    }
    tx_mutex_put(mutex);

    return;
}

/*************************************************************
*  Registers for Network Indication and Data handles         *
*************************************************************/
int psm_dam_test_devinfo_init2(void)
{
	int ret = QAPI_OK;

	ret = qapi_Device_Info_Init_v2(&device_handle);
	if(ret != QAPI_OK)
	{
		PSM_TEST_LOG_VERBOSE("devinfo init returned with error %d\n", ret);
		return ret;
	}

    ret = qapi_Device_Info_Pass_Pool_Ptr(device_handle, byte_pool_psmt);
    if(ret != QAPI_OK)
	{
		PSM_TEST_LOG_VERBOSE("init_device_info2 : qapi_Device_Info_Pass_Pool_Ptr returned error %d\n", ret);
		return ret;
	}

	ret = qapi_Device_Info_Set_Callback_v2(device_handle, QAPI_DEVICE_INFO_NETWORK_IND_E, device_info_callback);
    if(ret!=QAPI_OK)
    {
		PSM_TEST_LOG_VERBOSE("init_device_info2 : qapi_Device_Info_Set_Callback2 returned error %d\n", ret);
        return  ret;
	}

    PSM_TEST_LOG_VERBOSE("init_device_info2 : INIT successfull\n");
    
    /* Checking for network availablity */
    psm_dam_test_fetch_network_state();
    
    /* If persent cycle required Data, intialize the data handle */
    if( TRUE == app_configuration.is_data_event_cycle )
    {
        PSM_TEST_LOG_VERBOSE("just before dss_init");

        if (QAPI_DSS_SUCCESS !=  qapi_DSS_Init(QAPI_DSS_MODE_GENERAL))
        {
            PSM_TEST_LOG_VERBOSE("%s","dss_init failed !!");
        }
        else
        {
            PSM_TEST_LOG_VERBOSE("dss_init successful");
        }

        do
        {
            /* obtain data service handle */
            PSM_TEST_LOG_VERBOSE("Registering Callback DSS_Handle");
            qapi_DSS_Get_Data_Srvc_Hndl(psm_dam_test_net_event_cb, NULL, &dss_handle);
            PSM_TEST_LOG_VERBOSE("dss_handle %d", dss_handle);

            if ( dss_handle != NULL )
            {
                PSM_TEST_LOG_VERBOSE("Got valid Data handle. Break.");
/*                if(qapi_DSS_Pass_Pool_Ptr(dss_handle,byte_pool_psmt_data) != QAPI_OK)
                {
                    PSM_TEST_LOG_VERBOSE("Pass pool api failed");
                }
                else
                {
                    PSM_TEST_LOG_VERBOSE("Pass pool api Success");
                }*/

                break;
            }
            qapi_Timer_Sleep(100, QAPI_TIMER_UNIT_MSEC, true);

        }while(1);
    }
    else
    {
        PSM_TEST_LOG_VERBOSE("Data service handle registration not required");
    }

    return ret;
}

unsigned int psm_dam_get_MTU_size()
{
    qapi_Status_t result=QAPI_ERROR;
    int ip_family=-1;
    unsigned int mtu=0;

    PSM_TEST_LOG_VERBOSE("Setting IP Family");
    if(app_configuration.is_ipv6==0)
    {
        ip_family = QAPI_DSS_IP_FAMILY_V4_E;
    }
    else
    {
        ip_family = QAPI_DSS_IP_FAMILY_V6_E;
    }

    PSM_TEST_LOG_VERBOSE("IP Family is: %d",ip_family);
    PSM_TEST_LOG_VERBOSE("Getting MTU");
    result = qapi_DSS_Get_Link_Mtu_per_family(dss_handle, ip_family, &mtu);

    if(result==QAPI_OK)
    {
        PSM_TEST_LOG_VERBOSE("MTU is %d", mtu);
    }
    else
    {
        PSM_TEST_LOG_VERBOSE("Failed to obtain the MTU size!!");
    }
    return mtu;
}

int psm_dam_test_perform_ping(void)
{
    qapi_Net_Ping_V4_R2_t *ping_buf = NULL;             /*IPv4 request buffer*/
    qapi_Ping_Info_Resp_R2_t *ping_resp;                /*IPv4/IPv6 response buffer*/
    int                   rc = QAPI_ERROR;
    int                   count_full_size = 0;          /*Number of packets*/
    int                   partial_ping = FALSE;         /*If we have partial packet*/
    int                   partial_ping_size = 0;        /*Size of partial packet*/
    unsigned int          max_ping_payload_size;        /*Max value of a packet can be transfer*/
    struct                ip46addr addr;
    struct                ip46addr addr1;
    uint32_t timeout = 1000;                            /*Timeout value (in ms)*/
    uint8_t ttl = 64;                                   /*Time to live or hop limit*/
       
    /*IPv6 ping variables*/
    CHAR                  *ifname = NULL;               /*Interface name*/
    uint8_t               *ip6addr = NULL;              /*Destination to ping*/
    uint8_t               *ip6src = NULL;               /*Source address*/
    qapi_Net_Ping_V6_R2_t *ping6_buf = NULL;            /*IPv6 request buffer*/
    
    memset(&addr, 0x00, sizeof(struct ip46addr));
    memset(&addr1, 0x00, sizeof(struct ip46addr));
    
    max_ping_payload_size = psm_dam_get_MTU_size();

    if(max_ping_payload_size==0)
    {
        PSM_TEST_LOG_VERBOSE("MTU is invalid, ping event failed");
        return rc;
    }
    else
    {
        /*Reducing the payload size to ignore the fragmentation */
        max_ping_payload_size = max_ping_payload_size-100;
    }

    PSM_TEST_LOG_VERBOSE("Toggling GPIO before starting Ping");
    psm_dam_test_gpio_set(QAPI_GPIO_HIGH_VALUE_E);

    PSM_TEST_LOG_VERBOSE("Enter Perform ping function");
    
    //IPv4 Ping (Valid IPv4 adrress)
    if (inet_pton(AF_INET, app_configuration.ip_addr, &addr.a) == 0)
    {
        PSM_TEST_LOG_VERBOSE("IPV4 Max Payload Size is %d",max_ping_payload_size);
        PSM_TEST_LOG_VERBOSE("Setting ip route");
        addr.type = AF_INET;
        
        tx_byte_allocate(byte_pool_psmt, (VOID **)&ping_buf, sizeof(qapi_Net_Ping_V4_R2_t), TX_NO_WAIT);
        if(ping_buf == NULL)
        {
            PSM_TEST_LOG_ERROR("Failed to allocate memory");
            return rc;
        }
        memset(ping_buf, 0x00, sizeof(qapi_Net_Ping_V4_R2_t));
        
        tx_byte_allocate(byte_pool_psmt, (VOID **)&ping_resp, sizeof(qapi_Ping_Info_Resp_R2_t), TX_NO_WAIT);
        if(ping_resp == NULL)
        {
          PSM_TEST_LOG_ERROR("Failed to allocate memory");
          return rc;
        }
        memset(ping_resp, 0x00, sizeof(qapi_Ping_Info_Resp_R2_t));
        
        ping_buf->ipv4_addr = addr.a.addr4;
        ping_buf->bitmask |= QAPI_NET_PING_V4_DST_ADDR_MASK;
        ping_buf->ipv4_src = addr1.a.addr4;
        ping_buf->bitmask |= QAPI_NET_PING_V4_SRC_ADDR_MASK;
        ping_buf->size     = max_ping_payload_size;
        ping_buf->bitmask |= QAPI_NET_PING_V4_PKT_SIZE_MASK;
        ping_buf->timeout  = timeout;
        ping_buf->bitmask |= QAPI_NET_PING_V4_TIMEOUT_MASK;
        ping_buf->ttl      = ttl;
        ping_buf->bitmask |= QAPI_NET_PING_V4_TTL_MASK;
        
        count_full_size = (app_configuration.ping_size/max_ping_payload_size);
        if( 0 != (app_configuration.ping_size% max_ping_payload_size))
        {
            partial_ping = TRUE;
            partial_ping_size = (app_configuration.ping_size%max_ping_payload_size);
        }

        PSM_TEST_LOG_VERBOSE("Number of Pings based upon packet size : %d %d (%d %d) ",
                             app_configuration.ping_size, count_full_size, partial_ping, partial_ping_size);

        PSM_TEST_LOG_VERBOSE("performing IPV4 Ping");
        while(count_full_size--)
        {
            do
            {
                rc = qapi_Net_Ping_3(ping_buf, ping_resp);
                PSM_TEST_LOG_VERBOSE("Full packet Ping[%d]: Result: %d", count_full_size+1, rc);

                if( QAPI_OK == rc  || FALSE == app_configuration.is_data_success_required )
                {
                    break;
                }
                if(rc != QAPI_OK)
                {
                    if(ping_resp->ptype > 0)
                    {
                        PSM_TEST_LOG_ERROR("ICMP Error is %s",
                           ping_resp->perror);
                    }
                    else if(ping_resp->ptype == -2)
                    {
                        PSM_TEST_LOG_ERROR("Bind failed for provided src.");
                    }
                    else if(ping_resp->ptype == -1)
                    {
                        PSM_TEST_LOG_ERROR("Request timed out");
                    }            
                }

                qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_MSEC, true);
            }while(1);
        }

        PSM_TEST_LOG_VERBOSE("Ping packet size: %d", partial_ping_size);

        if( TRUE == partial_ping )
        {
            ping_buf->size     = partial_ping_size;
            ping_buf->bitmask |= QAPI_NET_PING_V4_PKT_SIZE_MASK;
            do
            {
                rc = qapi_Net_Ping_3(ping_buf, ping_resp);
                PSM_TEST_LOG_VERBOSE("Partial ping packet status: %d",rc);
                if( QAPI_OK == rc  || FALSE == app_configuration.is_data_success_required )
                {
                    break;
                }
                if(rc != QAPI_OK)
                {
                    if(ping_resp->ptype > 0)
                    {
                        PSM_TEST_LOG_ERROR("ICMP Error is %s\n",
                           ping_resp->perror);
                    }
                    else if(ping_resp->ptype == -2)
                    {
                        PSM_TEST_LOG_ERROR("Bind failed for provided src. \n");
                    }
                    else if(ping_resp->ptype == -1)
                    {
                        PSM_TEST_LOG_ERROR("Request timed out\n");
                    }            
                }

                qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_MSEC, true);
            }while(1);
        }

        PSM_TEST_LOG_VERBOSE("Ping: %d. IP: %s\n", rc, app_configuration.ip_addr);
        if(ping_buf)
        {
            tx_byte_release(ping_buf);
            ping_buf = NULL;
        }

        if(ping_resp)
        {
            tx_byte_release(ping_resp);
            ping_resp = NULL;
        }
    }
    //IPv6 Ping (Valid IPv6 adrress)
    else if (inet_pton(AF_INET6, app_configuration.ip_addr, &addr.a) == 0)
    {
        ttl = 255;
        PSM_TEST_LOG_VERBOSE("IPv6 Max Payload Size is %d",max_ping_payload_size);
		PSM_TEST_LOG_VERBOSE("Setting ip route");
		addr.type = AF_INET6;
		ip6addr = (uint8_t *)&addr.a;
        ip6src = (uint8_t *)&addr1.a;
        count_full_size = (app_configuration.ping_size/max_ping_payload_size);
        
        PSM_TEST_LOG_VERBOSE("Total number of packets: %d",count_full_size);
        tx_byte_allocate(byte_pool_psmt, (VOID **)&ping6_buf, sizeof(qapi_Net_Ping_V6_R2_t), TX_NO_WAIT);
        if(ping6_buf == NULL)
        {
            PSM_TEST_LOG_ERROR("Failed to allocate memory");
            return rc;
        }
        
        memset(ping6_buf, 0x00, sizeof(qapi_Net_Ping_V6_R2_t));

        tx_byte_allocate(byte_pool_psmt, (VOID **)&ping_resp, sizeof(qapi_Ping_Info_Resp_R2_t), TX_NO_WAIT);
        if(ping_resp == NULL)
        {
            PSM_TEST_LOG_ERROR("Failed to allocate memory");
            return rc;
        }
       
        memset(ping_resp, 0x00, sizeof(qapi_Ping_Info_Resp_R2_t));

        memcpy(ping6_buf->ipv6_addr,ip6addr, sizeof(struct in6_addr));
        ping6_buf->bitmask |= QAPI_NET_PING_V6_DST_ADDR_MASK;
        memcpy(ping6_buf->ipv6_src,ip6src, sizeof(struct in6_addr));
        ping6_buf->bitmask |= QAPI_NET_PING_V6_SRC_ADDR_MASK;
        ping6_buf->size = max_ping_payload_size;
        ping6_buf->bitmask |= QAPI_NET_PING_V6_PKT_SIZE_MASK;
        ping6_buf->timeout = timeout;
        ping6_buf->bitmask |= QAPI_NET_PING_V6_TIMEOUT_MASK;
        ping6_buf->ifname = ifname;
        ping6_buf->bitmask |= QAPI_NET_PING_V6_IF_NAME_MASK;
        ping6_buf->ttl = ttl;
        ping6_buf->bitmask |= QAPI_NET_PING_V6_TTL_MASK;
        
        if( 0 != (app_configuration.ping_size%max_ping_payload_size))
        {
            partial_ping = TRUE;
            partial_ping_size = (app_configuration.ping_size%max_ping_payload_size);
        }

        PSM_TEST_LOG_VERBOSE("Number of Pings based upon packet size : %d %d (%d %d) ",
                             app_configuration.ping_size, count_full_size, partial_ping, partial_ping_size);

		PSM_TEST_LOG_VERBOSE("performing IPV6 Ping");
        
        
        
        while(count_full_size--)
        {
            do
            {
				rc = qapi_Net_Ping6_3(ping6_buf, ping_resp);
                PSM_TEST_LOG_VERBOSE("Full packet Ping[%d]: Result: %d", count_full_size+1, rc);

                if( QAPI_OK == rc  || FALSE == app_configuration.is_data_success_required )
                {
                    break;
                }
				if(rc != QAPI_OK){
                    if(ping_resp->ptype > 0)
                    {
                        PSM_TEST_LOG_ERROR("ICMP Error is %s\n",
                           ping_resp->perror);
                    }
                    else if(ping_resp->ptype == -2)
                    {
                        PSM_TEST_LOG_ERROR("Bind failed for provided src. \n");
                    }
                    else if(ping_resp->ptype == -1)
                    {
                        PSM_TEST_LOG_ERROR("Request timed out\n");
                    }
                }
                qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_MSEC, true);
            }while(1);
        }

        PSM_TEST_LOG_VERBOSE("Ping packet size: %d", partial_ping_size);

        if( TRUE == partial_ping )
        {
            ping6_buf->size = partial_ping_size;
            ping6_buf->bitmask |= QAPI_NET_PING_V6_PKT_SIZE_MASK;
            do
            {
				rc = qapi_Net_Ping6_3(ping6_buf, ping_resp);
                PSM_TEST_LOG_VERBOSE("Partial ping packet status: %d",rc);
                if( SUCCESS == rc  || FALSE == app_configuration.is_data_success_required )
                {
                    break;
                }
				if(rc != QAPI_OK){
                    if(ping_resp->ptype > 0)
                    {
                        PSM_TEST_LOG_ERROR("ICMP Error is %s\n",
                           ping_resp->perror);
                    }
                    else if(ping_resp->ptype == -2)
                    {
                        PSM_TEST_LOG_ERROR("Bind failed for provided src. \n");
                    }
                    else if(ping_resp->ptype == -1)
                    {
                        PSM_TEST_LOG_ERROR("Request timed out\n");
                    }
                }

                qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_MSEC, true);
            }while(1);
        }

        PSM_TEST_LOG_VERBOSE("Ping: %d. IP: %s\n", rc, app_configuration.ip_addr);
    }
    else
    {
        PSM_TEST_LOG_ERROR("Invalid ping address");
    }

    PSM_TEST_LOG_VERBOSE("Exit ping function with return value: %d", rc);
    PSM_TEST_LOG_VERBOSE("Toggling GPIO after Ping");
    psm_dam_test_gpio_set(QAPI_GPIO_LOW_VALUE_E);

    return rc;
}

static int psm_dam_test_register_application(void)
{
    qapi_Status_t  result = QAPI_OK;
    do
    {
        result = qapi_PSM_Client_Register(&client_id, psm_dam_test_app_call_back_func, &cb_msg);
        if ( result == QAPI_OK )
        {
            break;
        }
        qapi_Timer_Sleep(100, QAPI_TIMER_UNIT_MSEC, true);
    }while(1);

    PSM_TEST_LOG_VERBOSE("MSG Address: %x",&cb_msg);

    PSM_TEST_LOG_VERBOSE("Result: %s", result ? "FAIL": "SUCCESS");
    PSM_TEST_LOG_VERBOSE("Application Id: %d", client_id);
    return result;
}

int psm_dam_test_enter_psm_auto(void)
{
    int             result = FAIL;
    psm_info_type   psm_info;
    int             psm_voting = TRUE;

    PSM_TEST_LOG_VERBOSE("Enter PSM process executing. Test app work is done.");

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

    if( app_configuration.is_modem_required == FALSE )
    {
        psm_info.psm_wakeup_type = PSM_WAKEUP_MEASUREMENT_ONLY;
    }

    if((app_configuration.gps_usecase == 2) && (psm_cycles_count < 2))
    {
        /* Need to vote less than Threshold for 2 cycles - DeepSleep Usecase*/
        psm_info.psm_time_info.psm_duration_in_secs = app_configuration.fake_time_in_psm; //Safe value less than 60 seconds
        fake_psm_time = app_configuration.fake_time_in_psm;
    }
    else if((app_configuration.gps_usecase == 3) && (psm_cycles_count < 2))
    {
        /*No need to set time since no voting of PSM. Fake time so it sleeps till then */
        fake_psm_time = app_configuration.fake_time_in_psm;
        psm_voting = FALSE;
    }
    else if((app_configuration.gps_usecase == 4) && (psm_cycles_count < 2))
    {
        /*
            No need to set time since no voting of PSM for 2 cycles.
            Fake time so it sleeps till then.
        */
        fake_psm_time = app_configuration.fake_time_in_psm;
        psm_voting = FALSE;
    }
    else if(app_configuration.gps_usecase == 3)
    {
        /* No need to vote for PSM in entire 3rd scenario */
        psm_voting = FALSE;
    }

    if( app_configuration.is_modem_required == FALSE )
    {
        psm_info.psm_wakeup_type = PSM_WAKEUP_MEASUREMENT_ONLY;
    }


    PSM_TEST_LOG_VERBOSE("PSM Voting for this cycle: %d", psm_voting);
    //psm_test_critical_ulog(1,"PSM Voting for this cycle: %d", psm_voting);
    if(psm_voting == TRUE)
    {
        PSM_TEST_LOG_VERBOSE("Enter into Power save Mode");
        result = qapi_PSM_Client_Enter_Psm(client_id, &psm_info);
    }

    //PSM_TEST_LOG_VERBOSE("Enter into Power save Mode");
    //result = qapi_PSM_Client_Enter_Psm(client_id, &psm_info);

    PSM_TEST_LOG_VERBOSE("Result: %s", result ? "FAIL":"SUCCESS");
    if ( result != QAPI_OK )
    {
        PSM_TEST_LOG_VERBOSE("Reason: %d",result);
    }

    /*
        Test app utility is complete at this point
        App has to sleep for the voted time. So signal main thread exit.
    */

    tx_mutex_get(c_mutex, TX_WAIT_FOREVER);
    psm_complete = TRUE;
    tx_event_flags_set(c_cond, 0x1, TX_OR);
    tx_mutex_put(c_mutex);

    return 0;
}

void psm_dam_test_evaluate_update_psm_cycle(void)
{
    int         count = 0;
    int         fp = INVALID_FILE_HANDLE;
    char        buffer[32] = {0};
    char        str[32] = {0};
    int         read_len = 0;

    if ( app_configuration.data_event_freq > 0 )
    {
        PSM_TEST_LOG_VERBOSE("Valid Data event frequency: %d", app_configuration.data_event_freq);

        qapi_FS_Open( PSM_AUTOMATE_TEST_COUNT_FILE, QAPI_FS_O_RDONLY_E, &fp);
        if ( INVALID_FILE_HANDLE == fp )
        {
            PSM_TEST_LOG_VERBOSE("No count file.. Starting counter");
            count = 1; //First PSM cycle after creating the count file;
        }
        else
        {
            qapi_FS_Read(fp, buffer, 32, &read_len);
            count = myAtoi(buffer);
            if(count <= 0)
            {
                count = 1;
            }
        }
        qapi_FS_Close(fp);
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

        qapi_FS_Unlink(PSM_AUTOMATE_TEST_COUNT_FILE);
        qapi_FS_Open(PSM_AUTOMATE_TEST_COUNT_FILE, QAPI_FS_O_WRONLY_E|QAPI_FS_O_CREAT_E, &fp);
        if ( INVALID_FILE_HANDLE != fp )
        {
            PSM_TEST_LOG_VERBOSE("Extra: Count File created");
            snprintf(str, sizeof(str), "%d", count+1);
            qapi_FS_Write(fp, str, strlen(str), &read_len);
        }
        qapi_FS_Close(fp);
        fp = INVALID_FILE_HANDLE;
        PSM_TEST_LOG_VERBOSE("Extra: PSM test evaluate function returned sucessfully");
    }
    else
    {
        app_configuration.data_event_freq = PSM_DEFAULT_DATA_EVENT_FREQUENCY;
        app_configuration.is_data_event_cycle = TRUE;
        PSM_TEST_LOG_VERBOSE("Invalid count. Setting default: %d", app_configuration.data_event_freq);
    }
}

/*========================================================================

                CALLBACKS FOR GPS MEASUREMENT <START>

=========================================================================*/

void psm_dam_test_gps_sample_and_wait()
{

    do
    {
        //Wait for requested number of GPS samples
        tx_mutex_get(gpstrack_mutex, TX_WAIT_FOREVER);
        dam_thread_cond_wait_and_clear(gpstrack_mutex,gpstrack_cond);
        tx_mutex_put(gpstrack_mutex);

    } while(0);

}

qapi_Location_Error_t psm_dam_test_gps_init()
{
    qapi_Location_Error_t ret = QAPI_LOCATION_ERROR_GENERAL_FAILURE;

    sLocationCallbacks.size = sizeof(qapi_Location_Callbacks_t);
    sLocationCallbacks.capabilitiesCb = psm_dam_test_gps_capabilities_callback;
    sLocationCallbacks.responseCb = psm_dam_test_gps_response_callback;
    sLocationCallbacks.collectiveResponseCb = psm_dam_test_gps_geofence_response_callback;
    sLocationCallbacks.trackingCb = psm_dam_test_gps_tracking_callback;
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

qapi_Location_Error_t psm_dam_test_gps_deinit()
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

qapi_Location_Error_t psm_dam_test_gps_start_tracking(int* id)
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
        tx_mutex_get(gpsresp_mutex, TX_WAIT_FOREVER);
        dam_thread_cond_wait_and_clear(gpsresp_mutex, gpsresp_cond);
        tx_mutex_put(gpsresp_mutex);

        PSM_TEST_LOG_VERBOSE("REQUEST SUCCESS start track");
    } while(0);

    return ret;
}

qapi_Location_Error_t psm_dam_test_gps_stop_tracking(int id)
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
        tx_mutex_get(gpsresp_mutex, TX_WAIT_FOREVER);
        dam_thread_cond_wait_and_clear(gpsresp_mutex,gpsresp_cond);
        tx_mutex_put(gpsresp_mutex);

        PSM_TEST_LOG_VERBOSE("REQUEST SUCCESS stop track");
    } while(0);

    return ret;
}


/*========================================================================

                CALLBACKS FOR GPS MEASUREMENT <END>

=========================================================================*/

void psm_dam_test_initialize_configuration (void)
{
    /* Initialize the structure with default configuration values*/
    PSM_TEST_LOG_VERBOSE("initialize config");
    app_configuration.valid = FALSE;
    app_configuration.is_data_event_cycle = TRUE;
    app_configuration.test_active_timer = -1;
    app_configuration.time_in_psm = PSM_DEFAULT_TEST_PSM_TIME;
    app_configuration.is_modem_required = TRUE;
    app_configuration.is_data_success_required = FALSE;
    app_configuration.ping_size = PSM_DEFAULT_PING_PACKET_SIZE;
    app_configuration.data_event_freq = PSM_DEFAULT_DATA_EVENT_FREQUENCY;
    app_configuration.is_location_scenario = FALSE;
    app_configuration.gps_min_distance = PSM_DEFAULT_GPS_MIN_DISTANCE;
    app_configuration.gps_time_interval = PSM_DEFAULT_GPS_TIME_INTERVAL;
    app_configuration.gps_samples = PSM_DEFAULT_GPS_SAMPLE_COUNT;
    app_configuration.gps_usecase = 0;
    app_configuration.fake_time_in_psm = PSM_DEFAULT_DEEP_SLEEP_TIME;
    app_configuration.gps_debug_sleep = 0;
    app_configuration.gpio_enabled = FALSE;
    app_configuration.gpio_pin = PSM_TEST_DEFAULT_GPIO_PIN;
    app_configuration.is_ipv6=0;
}

void psm_dam_test_perform_location_measure()
{
    qapi_Location_Error_t ret = QAPI_LOCATION_ERROR_GENERAL_FAILURE;
    int                   id = -1;

    do
    {
        /* Set the global variable with configured number gps_samples count */
        gps_samples = app_configuration.gps_samples;

        ret = psm_dam_test_gps_init();
        if(ret != QAPI_LOCATION_ERROR_SUCCESS)
        {
            PSM_TEST_LOG_VERBOSE("GPS Init failed, exiting: %d", ret);
            break;
        }
        PSM_TEST_LOG_VERBOSE("GPS Init success");

        ret = psm_dam_test_gps_start_tracking(&id);
        if(ret != QAPI_LOCATION_ERROR_SUCCESS)
        {
            PSM_TEST_LOG_VERBOSE("GPS tracking req failed, exiting: %d", ret);
            break;
        }
        PSM_TEST_LOG_VERBOSE("GPS Track req success");

        // Wait for requested number of samples
        PSM_TEST_LOG_VERBOSE("Wait for configured no. of GPS measurements");


        psm_dam_test_gps_sample_and_wait();
        PSM_TEST_LOG_VERBOSE("Wait for configured no. of GPS measurements COMPLETE");

        PSM_TEST_LOG_VERBOSE("Ping final Location info..");
        psm_dam_test_perform_ping();
        ret = psm_dam_test_gps_stop_tracking(id);
        if(ret != QAPI_LOCATION_ERROR_SUCCESS)
        {
            PSM_TEST_LOG_VERBOSE("GPS deinit failed, exiting: %d", ret);
            break;
        }
        PSM_TEST_LOG_VERBOSE("GPS deinit success");

        ret = psm_dam_test_gps_deinit();
    } while(0);

}

void psm_dam_test_perform_app_event()
{
    /* Handler for performing configured event. */

    /* Check if Location Event */
    if(TRUE == app_configuration.is_location_scenario)
    {
        psm_dam_test_perform_location_measure();
    }
    else
    {
        psm_dam_test_perform_ping();
    }

}

int psm_dam_test_auto(void)
{
    int    result = FAIL;
    ULONG   actual_flags;
    ULONG   c_actual_flags;

    PSM_TEST_LOG_VERBOSE("\n\n========= Automated Test Application =========");
    PSM_TEST_LOG_VERBOSE("Cycle Number: %d ", psm_cycles_count+1);

    if(!app_configuration.valid)
    {
        PSM_TEST_LOG_VERBOSE("Read config file");
        /* Bootup cycle. Need to read configurations */
        psm_dam_test_read_config_file();
        if(app_configuration.gpio_enabled)
        {
            psm_dam_test_gpio_init();
        }
    }

    if(client_id < 0)
    {
        /* Bootup cycle. Need to register with PSM Daemon */
        result = psm_dam_test_register_application();

        PSM_TEST_LOG_VERBOSE("Register application: %d", result);
        if(app_configuration.gps_debug_sleep)
        {
            PSM_TEST_LOG_VERBOSE("GPS Debug Sleep bootup delay before start of PSM Application");
            PSM_TEST_LOG_VERBOSE("===GPS Debug Sleep start===");
            qapi_Timer_Sleep(app_configuration.gps_debug_sleep, QAPI_TIMER_UNIT_SEC, true);
            PSM_TEST_LOG_VERBOSE("===GPS Debug Sleep end===");
        }

    }

    psm_dam_test_evaluate_update_psm_cycle();

    PSM_TEST_LOG_VERBOSE("Extra: Cycle Evaluation completed");
    if( TRUE == app_configuration.is_modem_required )
    {
        PSM_TEST_LOG_VERBOSE("Check and Perform PSM_Test Initialization");
        if(psm_init_complete == FALSE)
        {
            /* Once per bootup registrations*/
            psm_dam_test_devinfo_init2(); /* Registers for QMI Indications and Data handles */
            psm_init_complete = TRUE;
        }
        else
        {
            /* Non Bootup cycle. Just GET the sys-info */
            psm_dam_test_fetch_network_state();
        }

        PSM_TEST_LOG_VERBOSE("Wait for Device to acquire LTE service");
        do
        {
            if ( nw_full_service == TRUE )
            {
                break;
            }
            else
            {
                // Wait here till service is available.
                tx_event_flags_get(cond, 0x1, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
            }
        }while (1);

        if( TRUE == app_configuration.is_data_event_cycle )
        {
            PSM_TEST_LOG_VERBOSE("Data event cycle");
            PSM_TEST_LOG_VERBOSE("Cycle Number: %d and Data Connection state: %d", psm_cycles_count+1, data_connected);

            /* Added 8 */
            if(data_connected == FALSE)
            {
                psm_dam_test_start_lte_data_call();
            }
            else
            {
                /* Data connection already available in this cycle */
                psm_dam_test_perform_app_event();
                psm_dam_test_enter_psm_auto();
            }
        }
        else
        {
            PSM_TEST_LOG_VERBOSE("Not a Data event cycle. No Data call");
            psm_dam_test_enter_psm_auto();
        }

    }
    else
    {
        PSM_TEST_LOG_VERBOSE("Modem not required in this cycle. Send ready");
        psm_dam_test_enter_psm_auto();
    }

    do
    {
        PSM_TEST_LOG_VERBOSE("Wait for PSM");
        tx_event_flags_get(c_cond, 0x1, TX_OR_CLEAR, &c_actual_flags, TX_WAIT_FOREVER);
    }while (!psm_complete);

    PSM_TEST_LOG_VERBOSE("Exiting as PSM is complete");
    qapi_FS_Close(file_log_fd);

    return result;
}

void psm_data_damtest_task_entry
(
    ULONG thread_input
)
{

    ULONG   actual_flags = NULL;
    UINT    status;

    PSM_TEST_LOG_VERBOSE("psm_data_damtest_task_entry task started");

    do
    {

        status = tx_event_flags_get(data_cond, 0x00000011, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);

        if( status != TX_SUCCESS )
        {
            break;
        }

        switch(actual_flags)
        {
            case PSMT_NO_NET_MASK:
                PSM_TEST_LOG_VERBOSE("DSS_EVT_NET_NO_NET");

                /* Obtain mutex and set No Data state */
                tx_mutex_get(data_connection_mutex, TX_WAIT_FOREVER);
                data_connected = FALSE;
                tx_mutex_put(data_connection_mutex);
                //qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);
                //psm_dam_test_start_lte_data_call();
                break;

            case PSMT_CONN_NET_MASK:
                PSM_TEST_LOG_VERBOSE("Toggling GPIO before on Data call connected");
                psm_dam_test_gpio_set(QAPI_GPIO_LOW_VALUE_E);
                PSM_TEST_LOG_VERBOSE("DSS Event Connected. Attempting Ping");

                tx_mutex_get(data_connection_mutex, TX_WAIT_FOREVER);
                data_connected = TRUE;
                tx_mutex_put(data_connection_mutex);

                psm_dam_test_perform_app_event();
                psm_dam_test_enter_psm_auto();
                break;

            default:
                break;
        }

    } while(TRUE);

    //return 0;
}


int psm_damtest_task_entry
(
    void
)
{
    CHAR *data_thread_stack_pointer = NULL;
    int   result;
    struct qapi_FS_Stat_Type_s log_file;

    //qapi_Timer_Sleep(15, QAPI_TIMER_UNIT_SEC, true);
    //Hardcoded Mechanism to limit the logfile to 10KB or reset the log
    memset(&log_file, 0, sizeof(log_file));
    result = qapi_FS_Stat(PSM_DAM_TEST_LOG_FILE,&log_file);
    if( QAPI_OK == result )
    {
        if( log_file.st_size > 10000)
        {
            qapi_FS_Unlink(PSM_DAM_TEST_LOG_FILE);
        }
    }
    qapi_FS_Open(PSM_DAM_TEST_LOG_FILE, QAPI_FS_O_WRONLY_E|QAPI_FS_O_APPEND_E|QAPI_FS_O_CREAT_E, &file_log_fd);

    psm_dam_test_initialize_configuration();
    txm_module_object_allocate((void **)&mutex, sizeof(TX_MUTEX));
    tx_mutex_create(mutex, "PSM Test Mutex", TX_INHERIT);

    txm_module_object_allocate((void **)&c_mutex, sizeof(TX_MUTEX));
    tx_mutex_create(c_mutex, "PSM Test conditional Mutex", TX_INHERIT);

    txm_module_object_allocate((void **)&data_mutex, sizeof(TX_MUTEX));
    tx_mutex_create(data_mutex, "PSM Data Mutex", TX_INHERIT);

    txm_module_object_allocate((void **)&cond, sizeof(TX_EVENT_FLAGS_GROUP));
    tx_event_flags_create(cond, "event flag PSMT 1");

    txm_module_object_allocate((void **)&c_cond, sizeof(TX_EVENT_FLAGS_GROUP));
    tx_event_flags_create(c_cond, "event flag PSMT 2");

    txm_module_object_allocate((void **)&data_cond, sizeof(TX_EVENT_FLAGS_GROUP));
    tx_event_flags_create(data_cond, "event flag PSMT 3");

/*    txm_module_object_allocate(&byte_pool_psmt_data, sizeof(TX_BYTE_POOL));
    tx_byte_pool_create(byte_pool_psmt_data, "byte pool PSM Data", free_memory_psmt_data, PSMT_BYTE_POOL_SIZE);
*/
    txm_module_object_allocate((void **)&byte_pool_psmt, sizeof(TX_BYTE_POOL));
    tx_byte_pool_create(byte_pool_psmt, "byte pool PSM", free_memory_psmt, PSMT_BYTE_POOL_SIZE);

    /* Mutex and conditions for GPS*/
    txm_module_object_allocate((void **)&gpsresp_mutex, sizeof(TX_MUTEX));
    tx_mutex_create(gpsresp_mutex, "PSM Test gpsresp Mutex", TX_INHERIT);

    txm_module_object_allocate((void **)&gpstrack_mutex, sizeof(TX_MUTEX));
    tx_mutex_create(gpstrack_mutex, "PSM Test gpstrack Mutex", TX_INHERIT);

    txm_module_object_allocate((void **)&gpsresp_cond, sizeof(TX_EVENT_FLAGS_GROUP));
    tx_event_flags_create(gpsresp_cond, "event flag PSMT 4");

    txm_module_object_allocate((void **)&gpstrack_cond, sizeof(TX_EVENT_FLAGS_GROUP));
    tx_event_flags_create(gpstrack_cond, "event flag PSMT 5");

    /* Allocate the stack for data thread.  */
    txm_module_object_allocate((void **)&psm_data_thread, sizeof(TX_THREAD));
    tx_byte_allocate(byte_pool_psmt, (VOID **) &data_thread_stack_pointer, PSM_DATA_STACK_SIZE, TX_NO_WAIT);

    /* Create the data thread.  */
    tx_thread_create(psm_data_thread, "PSMT Data Thread", psm_data_damtest_task_entry,
                          0, data_thread_stack_pointer, PSM_DATA_STACK_SIZE,
                          148, 148, TX_NO_TIME_SLICE, TX_AUTO_START);


    do
    {
        psm_dam_test_auto();

        /* Sleep for the voted amount of time */
        if(app_configuration.time_in_psm <= 0)
        {
            app_configuration.time_in_psm = PSM_DEFAULT_TEST_PSM_TIME;
        }
        if(fake_psm_time <= 0)
        {
            qapi_Timer_Sleep((app_configuration.time_in_psm*SLEEP_1S), QAPI_TIMER_UNIT_MSEC, true);
        }
        else
        {
            /* Sleeping in GPS cases for Fake PSM Times */
           qapi_Timer_Sleep(fake_psm_time*SLEEP_1S, QAPI_TIMER_UNIT_MSEC, true);
        }

        /* Post wakeup, cancel the previous stale PSM Vote */
        qapi_PSM_Client_Cancel_Psm(client_id);

        psm_cycles_count++;
        psm_complete = FALSE;
    } while(TRUE); /* Run this in loop for Reject & Deepsleep cases */

}


void psm_format_log_msg
(
    char *buf_ptr,
    int buf_size,
    char *fmt,
    ...
)
{
    va_list ap;
    va_start( ap, fmt );

    if ( NULL != buf_ptr && buf_size > 0 )
    {
        vsnprintf( buf_ptr, buf_size, fmt, ap );
    }

    va_end( ap );
} /* ril_format_log_msg */

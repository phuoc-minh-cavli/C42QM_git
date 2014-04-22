/**************************************************************************************************************
 * Copyright (C) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * @file  psm_app.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the implementation for the Power Saving Mode demo application
 *
 **************************************************************************************************************
 *
 *   PSM Sample Application
 *   ======================
 *
 *   This is a reference application for using the Power Saving Mode(PSM) QAPIs
 *   provided as part of the SDK. 
 *
 *   We simulate a simple usecase,
 *      1. Device Wakes up
 *      2. Registers with the PSM Daemon
 *      3. Performs its operations/measurements
 *      4. Communicates the data over the network to a server
 *      5. Shuts down for a stipulated amount of time
 *      6. Repeat.
 *
 *   Any application that fits the above use case can utilize QAPI PSM to optimize the
 *   power consumption of the device.
 *   We use a simple Ping to simulate communication over the network to the server(4)
 *   Also, a sleep of 40 seconds is done before executing this application, to ensure there are
 *   no surprises to the user (Since PSM does shutdown the device abruptly/no other app registered)
 *
 *   This sample application further includes lots of customizations as listed and explained below
 *      1. IP - Destination server with which the application tries to communicate over network
 *      2. APN - Access Point Name(optional) to use while connecting to the network
 *      2. PING_SIZE - Size of the ping packet in bytes.
 *      3. MODEM_REQUIRED - If modem is required in the next bootup.
 *      4. TIME_IN_PSM - Time voted by the Sample App for PSM. This should be set to the 
 *                       time after which each server communication event is expected
 *      5. ACTIVE_TIMER - Time for which the sample application anticipates any 
 *                        downlink data after voting for PSM(if any)
 *      6. DATA_EVENT_FREQUENCY - The Nth cycle in which communication with the server is
 *                                desired. The data event happens every Nth, 2*Nth, 3*Nth cycle and so on
 *      7. DATA_SUCCESS_REQUIRED - If the acknowledgement to network event is required.
 *
 **************************************************************************************************************/


#include <math.h>
#include <stdio.h>
#include <float.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/* QAPI Headers Inclusion */

#include "qapi_fs.h"
#include "qapi_cli.h"
#include "qapi_psm.h"
#include "qapi_dss.h"
#include "qapi_diag.h"
#include "qapi_timer.h"
#include "qapi_socket.h"
#include "qflog_utils.h"
#include "qapi_netservices.h"


/* Application Specific Headers Inclusion */
#include "psm_app_data.h"
#include "psm_app_common.h"


/*===========================================================================

                           CONSTANTS DEFINITION

===========================================================================*/

// Below 2 are unused Constants for now.
// We will revisit once File Operations are in place.
#define PSM_APP_DATA_FILE                "psm_app_data"
#define PSM_APP_CYCLE_COUNT_FILE         "psm_app_count"

#define SUCCESS                           0
#define FAIL                             -1
#define PSM_APP_DATA_EVENT_FREQUENCY      1
#define PSM_APP_DEFAULT_ACTIVE_TIMER      0
#define PSM_APP_DEFAULT_PSM_TIME         (150)
#define PSM_APP_DEFAULT_PING_PACKET_SIZE  1000
#define PSM_APP_PING_MAX_PACKET_SIZE     (1500)
#define PSM_APP_INVALID_FILE_HANDLE      (-1)
#define PSM_APP_DATA_STACK_SIZE           4098
#define PSM_APP_BYTE_POOL_SIZE            17408 
#define PSM_APP_NO_NET_MASK               0x00000001
#define PSM_APP_CONN_NET_MASK             0x00000010

/*===========================================================================

                           Globals

===========================================================================*/

int               psm_app_client_id = -1;
int               psm_app_modem_loaded = FALSE;
int               psm_app_health_check_fail = FALSE;
int               psm_app_backoff = FALSE;
int               psm_app_psm_complete = FALSE;
int               psm_app_nw_full_service = TRUE; /* Assume Network Full service already */
qapi_DSS_Hndl_t   psm_app_dss_handle;
char              psm_app_apn[128];
char              psm_app_ip_addr[128];
int               psm_app_data_event_freq;
int               psm_app_is_data_event_cycle = TRUE;
int               psm_app_active_timer = -1;
int               psm_app_time_in_psm = PSM_APP_DEFAULT_PSM_TIME;
int               psm_app_is_modem_required = TRUE;
int               psm_app_is_data_success_required = FALSE;
int               psm_app_ping_size = PSM_APP_DEFAULT_PING_PACKET_SIZE;
int               psm_app_data_event_complete = FALSE;

psm_status_msg_type psm_app_cb_msg;

TX_MUTEX               *psm_app_mutex;
TX_EVENT_FLAGS_GROUP   *psm_app_cond;

TX_MUTEX               *psm_app_c_mutex;
TX_EVENT_FLAGS_GROUP   *psm_app_c_cond;

TX_MUTEX               *psm_app_data_mutex;
TX_EVENT_FLAGS_GROUP   *psm_app_data_cond;

TX_THREAD              *psm_app_data_thread;

/*===========================================================================

                           Static & global Variable Declarations

===========================================================================*/

UCHAR         psm_app_stack[PSM_APP_DATA_STACK_SIZE];
TX_THREAD     psm_data_thread_handle;
ULONG         free_memory_psm_app[PSM_APP_BYTE_POOL_SIZE];
TX_BYTE_POOL *byte_pool_psm_app;


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
} psm_data_event_params_type;

/*===========================================================================

                Map for Readable PSM Status and Error Messages

===========================================================================*/

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

#define PSM_APP_INTEGER 0
#define PSM_APP_STRING 1
#define ASSERT_FATAL 1

/*===========================================================================

                Utility Methods

===========================================================================*/

/*
    Converts a string of number into an int
*/
int myAtoi(char *str)
{
    int res = 0;
    int i = 0;  

    for (i = 0; (str[i] != '\0') && (str[i] <= '9') && (str[i] >= '0'); ++i)
        res = res*10 + str[i] - '0';

    return res;
}

/*
    Load the file specfied with input path
*/
int config_load(const char *file)
{
    int fd = -1;

    qapi_FS_Open(file, QAPI_FS_O_RDONLY_E, &fd);

    return fd;
}

/*
    Fetch the value specified against a provided key in a loaded configuration file
*/
void config_get_value(int fd, const char *identifier, int type, void* value)
{
    char file_contents[1000] = "";
    char * token;
    char * needle;
    char *saveptr;
    char sub_value[128] = "";
    int  read_length = 0;
    qapi_FS_Offset_t seek_status = 0;

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

                strlcpy(sub_value,needle,128);
                switch(type)
                {
                    case PSM_APP_STRING:
                        strlcpy(value, sub_value, sizeof(sub_value));
                        break;

                    case PSM_APP_INTEGER:
                       *((int*)value) = myAtoi(sub_value);
                        break;
                }
                break;
            }
            token = strtok_r(NULL, "\n", &saveptr);
        }
    } while(false);

}

/*
    Close the file specfied with input file descriptor
*/
void config_unload(int fd)
{
    qapi_FS_Close(fd);
}


/*============================================================================
                psm_app_read_config_file
============================================================================*/
/**
 *  Reads all the configuration parameters specified in file at PSM_APP_DATA_FILE
 *  and stores it against the respective global variables.
 *
 *  Unused at the moment. We will revisit this method once File Operations
 *  are supported.
 */
void psm_app_read_config_file(void)
{
    int fd = -1;
    fd = config_load(PSM_APP_DATA_FILE);
    config_get_value(fd, "IP=", PSM_APP_STRING, psm_app_ip_addr);
    config_get_value(fd, "APN=", PSM_APP_STRING, psm_app_apn);
    config_get_value(fd, "DATA_EVENT_FREQUENCY=", PSM_APP_INTEGER, &psm_app_data_event_freq);
    config_get_value(fd, "ACTIVE_TIMER=", PSM_APP_INTEGER, &psm_app_active_timer);
    config_get_value(fd, "TIME_IN_PSM=", PSM_APP_INTEGER, &psm_app_time_in_psm);
    config_get_value(fd, "MODEM_REQUIRED=", PSM_APP_INTEGER, &psm_app_is_modem_required);
    config_get_value(fd, "DATA_SUCCESS_REQUIRED=", PSM_APP_INTEGER, &psm_app_is_data_success_required);
    config_get_value(fd, "PING_SIZE=", PSM_APP_INTEGER, &psm_app_ping_size);
    LOG_INFO("Data Event Frequency from Config file is %d",psm_app_data_event_freq);
    LOG_INFO("Active Timer from Config file is %d",psm_app_active_timer);
    LOG_INFO("IP Address from Config file is %s",psm_app_ip_addr);
    LOG_INFO("APN from Config file is %s",psm_app_apn);
    LOG_INFO("Time in PSM configured is %d",psm_app_time_in_psm);
    LOG_INFO("Modem required configured is %d",psm_app_is_modem_required);
    LOG_INFO("Data success configured is %d",psm_app_is_data_success_required);
    LOG_INFO("Ping size from Config file is %d",psm_app_ping_size);

    config_unload(fd);
}

/*============================================================================
                psm_app_set_config_parameters
============================================================================*/
/**
 *  Uses the configuration parameters specified in psm_app_data.h
 *  and stores it against the respective global variables.
 */
void psm_app_set_config_parameters(void)
{
    strlcpy(psm_app_ip_addr,IP,sizeof(psm_app_ip_addr));
    strlcpy(psm_app_apn,APN,sizeof(psm_app_apn));
    psm_app_data_event_freq = DATA_EVENT_FREQUENCY;
    psm_app_active_timer = ACTIVE_TIMER;
    psm_app_time_in_psm = TIME_IN_PSM;
    psm_app_is_modem_required = MODEM_REQUIRED;
    psm_app_is_data_success_required = DATA_SUCCESS_REQUIRED;
    psm_app_ping_size = PING_SIZE;

    LOG_INFO("Data Event Frequency from Config file is %d",psm_app_data_event_freq);
    LOG_INFO("Active Timer from Config file is %d",psm_app_active_timer);
    LOG_INFO("IP Address from Config file is %s",psm_app_ip_addr);
    LOG_INFO("APN from Config file is %s",psm_app_apn);
    LOG_INFO("Time in PSM configured is %d",psm_app_time_in_psm);
    LOG_INFO("Modem required configured is %d",psm_app_is_modem_required);
    LOG_INFO("Data success configured is %d",psm_app_is_data_success_required);
    LOG_INFO("Ping size from Config file is %d",psm_app_ping_size);
}

/*============================================================================
                psm_app_start_lte_data_call
============================================================================*/
/**
 *  Triggers an LTE Data call using the QAPI_DSS APIs.
 *  This data call will result in an event fired against the callback registered 
 *  as part of Initialization with QAPI_DSS.
 */
int psm_app_start_lte_data_call(void)
{
    qapi_DSS_Call_Param_Value_t param_info;

    LOG_INFO("LTE Data call function entered");

    if ( psm_app_dss_handle != NULL )
    {
        /* set data call param */
        param_info.buf_val = NULL;
        param_info.num_val = QAPI_DSS_RADIO_TECH_UNKNOWN;
        LOG_INFO("Setting tech to Automatic");
        qapi_DSS_Set_Data_Call_Param(psm_app_dss_handle, QAPI_DSS_CALL_INFO_TECH_PREF_E, &param_info);


        param_info.buf_val = psm_app_apn;
        param_info.num_val = strlen(psm_app_apn);
        LOG_INFO("Setting APN - %s", psm_app_apn);
        qapi_DSS_Set_Data_Call_Param(psm_app_dss_handle, QAPI_DSS_CALL_INFO_APN_NAME_E, &param_info);

        param_info.buf_val = NULL;
        param_info.num_val = QAPI_DSS_IP_VERSION_4;
        LOG_INFO("Setting family to IP");
        qapi_DSS_Set_Data_Call_Param(psm_app_dss_handle, QAPI_DSS_CALL_INFO_IP_VERSION_E, &param_info);

        /* set data call param for UMTS*/
        param_info.buf_val = NULL;
        param_info.num_val = 1;
        LOG_INFO("setting profile number to: %d", 1);
        qapi_DSS_Set_Data_Call_Param(psm_app_dss_handle, QAPI_DSS_CALL_INFO_UMTS_PROFILE_IDX_E, &param_info);

        qapi_DSS_Start_Data_Call(psm_app_dss_handle);
        LOG_INFO("Start data call executed");

    }
    else
    {
        LOG_ERROR("psm_app_dss_handle is NULL");
    }
    return 0;
}

/*============================================================================
                psm_app_call_back_func
============================================================================*/
/**
 *  This is the callback function registered with the QAPI PSM framework.
 *  All PSM events are communicated with our application through this 
 *  call back.
 *
 *  Ideally this call back function must post an event to the application's
 *  main thread and handled there. Here we demonstrate the utility simply.
 *
 *  All events posted through this callback can be referred at psm_status_type_e
 */
void psm_app_call_back_func(psm_status_msg_type *psm_status)
{
    if ( psm_status )
    {
        // to avoid fludding of logs, don't print health check.
        if ( psm_status->status != PSM_STATUS_HEALTH_CHECK &&
             psm_status->status != PSM_STATUS_NONE )
        {
            LOG_INFO("PSM STATUS:    %s", status_string[psm_status->status].err_name);
            LOG_INFO("REJECT REASON: %s\n", reject_string[psm_status->reason].err_name);
        }

        switch(psm_status->status)
        {
            // special handling for some cases like modem loaded, not loaded etc.
            case PSM_STATUS_MODEM_LOADED:
                psm_app_modem_loaded = TRUE;
                break;

            case PSM_STATUS_MODEM_NOT_LOADED:
                psm_app_modem_loaded = FALSE;
                break;

            case PSM_STATUS_HEALTH_CHECK:
                if ( !psm_app_health_check_fail )
                {
                    qapi_PSM_Client_Hc_Ack(psm_app_client_id);
                }
                break;

            case PSM_STATUS_NW_OOS:
                if ( psm_app_backoff )
                {
                    qapi_PSM_Client_Enter_Backoff(psm_app_client_id);
                }
                break;

            case PSM_STATUS_NW_LIMITED_SERVICE:
                if ( psm_app_backoff )
                {
                    qapi_PSM_Client_Enter_Backoff(psm_app_client_id);
                }
                break;

            case PSM_STATUS_COMPLETE:
                tx_mutex_get(psm_app_c_mutex, TX_WAIT_FOREVER);
                psm_app_psm_complete = TRUE;
                tx_event_flags_set(psm_app_c_cond, 0x1, TX_OR);
                tx_mutex_put(psm_app_c_mutex);
                LOG_INFO("Received PSM complete");
                break;

            default:
                break;
        }
    }
}

/*============================================================================
                psm_app_net_event_cb
============================================================================*/
/**
 *  This is the callback function registered with the QAPI DSS framework.
 *  All DSS/Data events are communicated to our application through this 
 *  call back.
 *
 *  This call back function sets the condition variable that our Data thread is 
 *  waiting indefinitely for.
 *
 *  Condition variable psm_app_data_cond is the trigger for Data thread to perform
 *  its data event. It is a Network Ping in our sample application
 */
void psm_app_net_event_cb( qapi_DSS_Hndl_t hndl,
                                     void * user_data,
                                     qapi_DSS_Net_Evt_t evt,
                                     qapi_DSS_Evt_Payload_t *payload_ptr )
{

    psm_data_event_params_type  params;

    memset(&params, 0, sizeof(params));
    params.evt = evt;
    params.hndl = hndl;

    tx_mutex_get(psm_app_data_mutex, TX_WAIT_FOREVER);

    switch( params.evt )
    {
        case QAPI_DSS_EVT_NET_NO_NET_E:
            tx_event_flags_set(psm_app_data_cond, PSM_APP_NO_NET_MASK, TX_OR);
            break;

        case QAPI_DSS_EVT_NET_IS_CONN_E:
            tx_event_flags_set(psm_app_data_cond, PSM_APP_CONN_NET_MASK, TX_OR);
            break;

        default:
            break;
    }
    tx_mutex_put(psm_app_data_mutex);

    LOG_INFO("Data event: %d sent to data thread.",params.evt);

}

/*============================================================================
                psm_app_init
============================================================================*/
/**
 *  Initializes the application's conditions.
 *  Here we register our callback with QAPI DSS for all data events.
 *
 *  We can optionally wait here for Network Full service.
 */
void psm_app_init(void)
{

    if ( psm_app_nw_full_service == TRUE )
    {
        tx_event_flags_set(psm_app_cond, 0x1, TX_OR);
        LOG_INFO("Network Full Service: %d", psm_app_nw_full_service);
    }
    tx_mutex_put(psm_app_mutex);

    if( TRUE == psm_app_is_data_event_cycle )
    {
        LOG_INFO("just before dss_init");

        if (QAPI_DSS_SUCCESS !=  qapi_DSS_Init(QAPI_DSS_MODE_GENERAL))
        {
            LOG_INFO("%s","dss_init failed !!");
        }
        else
        {
            LOG_INFO("dss_init successful");
        }

        do
        {
            /* obtain data service handle */
            LOG_INFO("Registering Callback DSS_Handle");
            qapi_DSS_Get_Data_Srvc_Hndl(psm_app_net_event_cb, NULL, &psm_app_dss_handle);
            LOG_INFO("psm_app_dss_handle %d", psm_app_dss_handle);

            if ( psm_app_dss_handle != NULL )
            {
                LOG_INFO("Got valid Data handle. Break.");
                break;
            }
            qapi_Timer_Sleep(100, QAPI_TIMER_UNIT_MSEC, true);

        }while(1);
    }
    else
    {
        LOG_INFO("Data service handle registration not required");
    }
}

/*============================================================================
                psm_app_register_application
============================================================================*/
/**
 *  Wrapper over the QAPI PSM's application register method.
 *  This makes the sample application known to the PSM Daemon running in the device.
 * 
 *  Client id procured in this call is required for all further PSM operations
 */
static int psm_app_register_application(void)
{
    qapi_Status_t  result = QAPI_OK;
    do
    {
        result = qapi_PSM_Client_Register(&psm_app_client_id, psm_app_call_back_func, &psm_app_cb_msg);
        if ( result == QAPI_OK )
        {
            break;
        }
        qapi_Timer_Sleep(100, QAPI_TIMER_UNIT_MSEC, true);
    }while(1);
    LOG_INFO("Result: %s", result ? "FAIL": "SUCCESS");
    LOG_INFO("Application Id: %d", psm_app_client_id);
    return result;
}

/*============================================================================
                psm_app_enter_psm
============================================================================*/
/**
 *  Wrapper over the QAPI PSM's method to vote for PSM.
 * 
 *  Enter PSM method requires Active Time, PSM Time and Measurement type for
 *  next bootup.
 *
 */
int psm_app_enter_psm(void)
{
    int             result = FAIL;
    psm_info_type   psm_info;

    LOG_INFO("Enter PSM process executing. PSM app work is done.");

    memset(&psm_info, 0, sizeof(psm_info));
    psm_info.psm_time_info.time_format_flag     = PSM_TIME_IN_SECS;
    psm_info.psm_wakeup_type = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;

    if( psm_app_active_timer >= 0 )
    {
        psm_info.active_time_in_secs = psm_app_active_timer;
    }
    else
    {
        psm_info.active_time_in_secs = PSM_APP_DEFAULT_ACTIVE_TIMER;
    }

    psm_info.psm_time_info.psm_duration_in_secs = psm_app_time_in_psm;
    if( psm_app_is_modem_required == FALSE )
    {
        psm_info.psm_wakeup_type = PSM_WAKEUP_MEASUREMENT_ONLY;
    }

    LOG_INFO("Enter into Power save Mode");
    result = qapi_PSM_Client_Enter_Psm(psm_app_client_id, &psm_info);

    LOG_INFO("Result: %s", result ? "FAIL":"SUCCESS");
    if ( result != PSM_ERR_NONE )
    {
        LOG_INFO("Reason: %s", error_string[result].err_name);
    }

    return 0;
}

/*============================================================================
                psm_app_evaluate_update_psm_cycle
============================================================================*/
/**
 *  Method to find the current number of PSM Cycle and its related properties
 *
 *  The count of PSM Cycle is useful for scenarios which require the 
 *  application to perform Network Activity in a particular cycle only.
 *  With this count, we will be able to deduce if the next cycle is Measure-only
 *  bootup or a Network required bootup.
 *
 *  Say, Sample app must ping a server every 5th Cycle
 */
void psm_app_evaluate_update_psm_cycle(void)
{
    int         count = 0;
    int         fp = PSM_APP_INVALID_FILE_HANDLE;
    char        buffer[32] = {0};
    char        str[32] = {0};
    int         read_len = 0;

    if ( psm_app_data_event_freq > 0 )
    {
        LOG_INFO("Valid Data event frequency: %d", psm_app_data_event_freq);

        qapi_FS_Open( PSM_APP_CYCLE_COUNT_FILE, QAPI_FS_O_RDONLY_E, &fp);
        if ( PSM_APP_INVALID_FILE_HANDLE == fp )
        {
            LOG_INFO("No count file.. Starting counter");
            count = 1; //First PSM cycle after creating the count file;
        }
        else
        {
            qapi_FS_Read(fp, buffer, 32, &read_len);
            count = atoi(buffer);
            if(count <= 0)
            {
                count = 1;
            }
        }
        qapi_FS_Close(fp);
        fp = PSM_APP_INVALID_FILE_HANDLE;
    
        if ( count == psm_app_data_event_freq )
        {
            psm_app_is_data_event_cycle = TRUE;
            count = 0;
        }
        else
        {
            psm_app_is_data_event_cycle = FALSE;
        }
    
        qapi_FS_Unlink(PSM_APP_CYCLE_COUNT_FILE);
        qapi_FS_Open(PSM_APP_CYCLE_COUNT_FILE, QAPI_FS_O_WRONLY_E|QAPI_FS_O_CREAT_E, &fp);
        if ( PSM_APP_INVALID_FILE_HANDLE != fp )
        {
            snprintf(str, sizeof(str), "%d", count+1);
            qapi_FS_Write(fp, str, strlen(str), &read_len);
        }
        qapi_FS_Close(fp);
        fp = PSM_APP_INVALID_FILE_HANDLE;
    }
    else
    {
        psm_app_data_event_freq = PSM_APP_DATA_EVENT_FREQUENCY;
        psm_app_is_data_event_cycle = TRUE;
        LOG_INFO("Invalid count. Setting default: %d", psm_app_data_event_freq);
    }
}

/*============================================================================
                psm_app_evaluate_psm_cycle_random
============================================================================*/
/**
 *  Method to find the current number of PSM Cycle and its related properties
 *
 *  This is functionally similar to psm_app_evaluate_update_psm_cycle but 
 *  does not use storage to enumerate the current cycle. A random number
 *  generator is used to assume the cycle number and if Data event is required.
 */
void psm_app_evaluate_psm_cycle_random(void)
{
    int         count = 0;
    qapi_time_get_t current_time;
    qapi_Status_t  result = QAPI_OK;

    if ( psm_app_data_event_freq > 0 )
    {
        LOG_INFO("Valid Data event frequency: %d", psm_app_data_event_freq);
        result = qapi_time_get(QAPI_TIME_SECS,&current_time);
        LOG_INFO("Current Time: %d", current_time.time_secs);

        if(result == QAPI_OK)
        {
            srand(current_time.time_secs);
        }
        else
        {
            srand(0);
        }

        count = (rand()%(psm_app_data_event_freq)) + 1;
        if ( count == psm_app_data_event_freq )
        {
            psm_app_is_data_event_cycle = TRUE;
            count = 0;
        }
        else
        {
            psm_app_is_data_event_cycle = FALSE;
        }
    }
    else
    {
        psm_app_data_event_freq = PSM_APP_DATA_EVENT_FREQUENCY;
        psm_app_is_data_event_cycle = TRUE;
        LOG_INFO("Invalid count. Setting default: %d", psm_app_data_event_freq);
    }
    LOG_INFO("Is Data cycle: %d", psm_app_is_data_event_cycle);

}

/*============================================================================
                psm_app_run
============================================================================*/
/**
 *  Main thread method of the PSM Sample app
 *
 *  This essentially performs the operations in following order:-
 *
 *  1. Read configuration parameters
 *  2. Register the sample application with PSM Daemon
 *  3. Evaluate and update the Cycle count, if data event needs to be performed 
 *     and if modem is required in the next bootup.
 *  4. Initialize and register with QAPI DSS
 *  5. Perform a LTE Data call
 *  6. Wait till Data Call connection is confirmed and perform a Network Event
 *  7. Based upon Configuration, retry Network event till success.
 *  8. Vote for PSM
 *  9. Exit
 */
int psm_app_run(void)
{
    int    result = FAIL;
    ULONG   actual_flags;
    ULONG   c_actual_flags;

    psm_app_set_config_parameters();
    result = psm_app_register_application();

    LOG_INFO("Register application: %d", result);

    psm_app_evaluate_psm_cycle_random();

    if( TRUE == psm_app_is_modem_required )
    {
        LOG_INFO("Wait for Device to acquire LTE service");
        psm_app_init();

        do
        {
            if ( psm_app_nw_full_service == TRUE )
            {
                break;
            }
            else
            {
                // Wait here till service is available.
                tx_event_flags_get(psm_app_cond, 0x1, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
            }
        }while (1);

        if( TRUE == psm_app_is_data_event_cycle )
        {
            LOG_INFO("Data event cycle. Performing Data call.");
            psm_app_start_lte_data_call();
        }
        else
        {
            LOG_INFO("Not a Data event cycle. No Data call");
            psm_app_enter_psm();
        }

    }
    else
    {
        LOG_INFO("Modem not required in this cycle. Send ready");
        psm_app_enter_psm();
    }


    do
    {
           tx_event_flags_get(psm_app_c_cond, 0x1, TX_OR_CLEAR, &c_actual_flags, TX_WAIT_FOREVER);
    }while (!psm_app_psm_complete);

    LOG_INFO("Exiting as PSM is complete");

    return result;
}

/*============================================================================
                psm_app_perform_ping
============================================================================*/
/**
 *  Method to perform a ping with the configured parameters.
 *  Optionally we will retry Ping until success.
 *
 *  We take care that pings are sent out with the permitted maximum
 *  packet size. If size exceeds it, we divide it into multiple ping packets.
 */
int psm_app_perform_ping(void)
{
    int                   rc = FAIL;
    int                   count_full_size = 0;
    int                   partial_ping = FALSE;
    int                   partial_ping_size = 0;
    struct                ip46addr addr;


    LOG_INFO("Enter Perform ping function");

    LOG_INFO("Setting ip route");
    addr.type = AF_INET;

    if (inet_pton(AF_INET, psm_app_ip_addr, &addr.a) == 0)
    {
        count_full_size = (psm_app_ping_size/PSM_APP_PING_MAX_PACKET_SIZE);
        if( 0 != (psm_app_ping_size%PSM_APP_PING_MAX_PACKET_SIZE) )
        {
            partial_ping = TRUE;
            partial_ping_size = (psm_app_ping_size%PSM_APP_PING_MAX_PACKET_SIZE);
        }

        LOG_INFO("Number of Pings based upon packet size : %d %d (%d %d) ",
                             psm_app_ping_size, count_full_size, partial_ping, partial_ping_size);

        while(count_full_size--)
        {
            do
            {
                rc = qapi_Net_Ping(addr.a.addr4, PSM_APP_PING_MAX_PACKET_SIZE);
                LOG_INFO("Full packet Ping[%d]: Result: %d", count_full_size+1, rc);

                if( SUCCESS == rc  || FALSE == psm_app_is_data_success_required )
                {
                    break;
                }

                //ril_platform_thread_sleep(SLEEP_10MS);

            }while(1);
        }

        LOG_INFO("Ping packet size: %d", partial_ping_size);

        if( TRUE == partial_ping )
        {
            do
            {
                rc = qapi_Net_Ping(addr.a.addr4, partial_ping_size);
                if( SUCCESS == rc  || FALSE == psm_app_is_data_success_required )
                {
                    break;
                }

                //ril_platform_thread_sleep(SLEEP_10MS);
            }while(1);
        }

        LOG_INFO("Ping: %d. IP: %s\n", rc, psm_app_ip_addr);
    }

    LOG_INFO("Exit ping function with return value: %d", rc);
    return rc;
}


/*============================================================================
                psm_app_data_task_entry
============================================================================*/
/**
 *  Entry function for the Data thread.
 * 
 *  Indefinitely waits for the conditional variable to be set. Once set, it
 *  triggers the corresponding data events and then votes for PSM
 *
 *  If DSS informs us that data connection could not be established, we sleep for 1 second
 *  and retry the LTE Data Call.
 */
int psm_app_data_task_entry
(
    ULONG thread_input
)
{

    ULONG   actual_flags = 0;
    UINT    status;

    do
    {
        LOG_INFO("Wait for Data condition event");
        status = tx_event_flags_get(psm_app_data_cond, 0x00000011, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);

        if( status != TX_SUCCESS )
        {
            break;
        }

        switch(actual_flags)
        {
            case PSM_APP_NO_NET_MASK:
                LOG_INFO("DSS Event No Net. Data Event complete status: %d", psm_app_data_event_complete);
                if(psm_app_data_event_complete == FALSE)
                {
                    LOG_INFO("Sleep for 1 second before retry");
                    qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);
                    LOG_INFO("Attempting Data call again");
                    psm_app_start_lte_data_call();
                }
                break;

            case PSM_APP_CONN_NET_MASK:
                LOG_INFO("DSS Event Connected. Attempting Ping");

                psm_app_perform_ping();
                psm_app_data_event_complete = TRUE;
                psm_app_enter_psm();

                break;

            default:
                break;
        }

    } while(TRUE);

    return 0;
}

/*============================================================================
                psm_app
============================================================================*/
/**
 *  Entry function for the PSM App.
 */
int psm_app
(
    void
)
{
    CHAR *data_thread_stack_pointer = NULL;
    int   result;

   /*
        The sample application broadly runs as 2 threads.
        1. The main thread
           This handles all the PSM application related operations such as
           Initialization, registration and PSM Entry.

        2. Data Event thread.
           This handles the data operations to be done as part of the application.
           We perform a Ping over the network which is handled in this thread.
    */


    /* Create and Allocate memory for Mutexes and Conditional Variables */

    txm_module_object_allocate(&psm_app_mutex, sizeof(TX_MUTEX));
    tx_mutex_create(psm_app_mutex, "PSM Sample App Mutex", TX_INHERIT);

    txm_module_object_allocate(&psm_app_c_mutex, sizeof(TX_MUTEX));
    tx_mutex_create(psm_app_c_mutex, "PSM Sample App conditional Mutex", TX_INHERIT);

    txm_module_object_allocate(&psm_app_data_mutex, sizeof(TX_MUTEX));
    tx_mutex_create(psm_app_data_mutex, "PSM Sample App Data Mutex", TX_INHERIT);

    txm_module_object_allocate(&psm_app_cond, sizeof(TX_EVENT_FLAGS_GROUP));
    tx_event_flags_create(psm_app_cond, "event flag PSM Sample App 1");

    txm_module_object_allocate(&psm_app_c_cond, sizeof(TX_EVENT_FLAGS_GROUP));
    tx_event_flags_create(psm_app_c_cond, "event flag PSM Sample App 2");

    txm_module_object_allocate(&psm_app_data_cond, sizeof(TX_EVENT_FLAGS_GROUP));
    tx_event_flags_create(psm_app_data_cond, "event flag PSM Sample App 3");

    txm_module_object_allocate(&byte_pool_psm_app, sizeof(TX_BYTE_POOL));
    tx_byte_pool_create(byte_pool_psm_app, "byte pool PSM Sample App",
                                        free_memory_psm_app, PSM_APP_BYTE_POOL_SIZE);

    /*==================================================================*/


    /* Allocate the stack for data thread.  */

    txm_module_object_allocate(&psm_app_data_thread, sizeof(TX_THREAD));
    tx_byte_allocate(byte_pool_psm_app, (VOID **) &data_thread_stack_pointer,
                                                PSM_APP_DATA_STACK_SIZE, TX_NO_WAIT);

    /*==================================================================*/


    /* Create the data event thread */

    tx_thread_create(psm_app_data_thread, "PSM Sample App Data Thread", psm_app_data_task_entry,
                          0, data_thread_stack_pointer, PSM_APP_DATA_STACK_SIZE,
                          148, 148, TX_NO_TIME_SLICE, TX_AUTO_START);

    /*==================================================================*/


    /* Run the operations to be handled in main thread */

    psm_app_run();

    /*==================================================================*/


    /* Final fallback when all operations are complete. We remain here till shutdown */

    while(1)
    {
        //Loop forever so that App responds to health check if PSM rejected.
        qapi_Timer_Sleep(2000, QAPI_TIMER_UNIT_MSEC, true);
    }

    /*==================================================================*/
}



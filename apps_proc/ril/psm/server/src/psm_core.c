/******************************************************************************
#@file    psm_core.c
#@brief   PSM server core functionality.
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ril_port.h"

#include "psm_qmi.h"
#include "psm_rtc.h"
#include "psm_core.h"
#include "psm_client.h"
#include "psm_common.h"
#include "psm_list.h"
#include "pm_pon.h"

#include "utils_common.h"

#include "DALDeviceId.h"
#include "TlmmDefs.h"
#include "DDITlmm.h"
#include "DDIChipPower.h"

#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

#define PSM_DIR                 "/ril/psm"
#define PSM_SERVER_CONFIG_FILE  "/ril/psm/server_config"

#define PSM_PIPE_MAX_ELEMENTS                24
#define PSM_BOOTUP_DELAY                    (13)
#define PSM_GPIO_DEFAULT_PIN                 10
#define PSM_GPIO_DEFAULT_STATE               FALSE
#define PSM_HIBERNATION_THRESHOLD           (0)
#define PSM_DEFAULT_INACTIVITY_TIMER_STATE   TRUE
#define PSM_DEFAULT_INACTIVITY_TIMER        (0)
#define PSM_PULL_DEFAULT_DURATION                   (1800)

#ifdef  FEATURE_PSM_SENSOR_PRODUCT
#define PSM_PON_SUPPRESS_ENABLED_DEFAULT_VALUE FALSE
#else
#define PSM_PON_SUPPRESS_ENABLED_DEFAULT_VALUE TRUE
#endif

typedef int (*psm_req_handler_type)(psm_event_params_type *);

typedef struct
{
    psm_event_type_e       event_id;
    psm_req_handler_type   handler;
} psm_dispatch_table_entry_type;

int                   rcvd_auto_ready = FALSE;
int                   pending_shutdown = FALSE;
int                   psm_gpio_enabled = PSM_GPIO_DEFAULT_STATE;
int                   psm_gpio_pin = PSM_GPIO_DEFAULT_PIN;
int                   psm_bootup_delay = PSM_BOOTUP_DELAY;
int                   psm_hibernation_threshold = PSM_HIBERNATION_THRESHOLD;
int                   psm_inactivity_enabled = PSM_DEFAULT_INACTIVITY_TIMER_STATE;
int                   psm_inactivity_timer_value = PSM_DEFAULT_INACTIVITY_TIMER;
int                   psm_pull_duration_time     = PSM_PULL_DEFAULT_DURATION;
int                   psm_pon_suppress_enabled = PSM_PON_SUPPRESS_ENABLED_DEFAULT_VALUE;
psm_modem_load_status modem_load_status = PSM_MODEM_NOT_LOADED;
ril_platform_timer_t  active_timer_id;
ril_platform_timer_t  server_hc_timerid;
ril_platform_timer_t  psm_inactivity_timerid;
int                   randomization_time = 0;
int                   psm_server_hc_started = FALSE;
psm_state_type_e      psm_state;
DalDeviceHandle      *dev_hndl = NULL;
DalDeviceHandle      *psm_power_hndl = NULL;
DALGpioSignalType     gpioConfig;

ril_platform_pipe_t         event_pipe;
ril_platform_pipe_t         server_pipe;
ril_platform_pipe_t         client_pipe;

static psm_dispatch_table_entry_type qcril_event_table[] =
{
    { PSM_EVT_PSM_REGISTER,            psm_core_handle_register_request},
    { PSM_EVT_PSM_READY,               psm_core_handle_ready_request},
    { PSM_EVT_PSM_NOT_READY,           psm_core_handle_not_ready_request},
    { PSM_EVT_PSM_UNREGISTER,          psm_core_handle_unregister_request},
    { PSM_EVT_QMI_IND,                 psm_qmi_handle_indication},
    { PSM_EVT_LOAD_MODEM,              psm_core_handle_load_modem_request},
    { PSM_EVT_CLIENT_HEALTH_CHECK,     psm_core_handle_client_health_check_request},
    { PSM_EVT_SERVER_HEALTH_CHECK,     psm_core_handle_server_health_check_request},
    { PSM_EVT_AUTO_READY,              psm_core_handle_auto_ready},
    { PSM_EVT_ACTIVE_TIMER_EXPIRY,     psm_core_handle_event_active_timer_expiry},
    { PSM_EVT_CLIENT_BACKOFF,          psm_core_handle_backoff_request},
    { PSM_EVT_INACTIVITY_TIMER_EXPIRY, psm_core_handle_inactivity_timer_expiry},
};

extern void sys_m_shutdown(void);
extern void sys_m_hibernation(unsigned int);
/*========================================================================================
FUNCTION psm_core_get_psm_state

Query current PSM state.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
inline psm_state_type_e psm_core_get_psm_state(void)
{
    return psm_state;
}

/*========================================================================================
FUNCTION psm_core_set_psm_state

Set PSM state.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
void psm_core_set_psm_state(psm_state_type_e state)
{
    PSM_LOG_FUNC_ENTRY("psm_core_set_psm_state: function entry");

    if ( PSM_STATE_LAST >= state )
    {
        // Only if there is state transition send status to clients.
        if ( state != psm_state )
        {
            psm_state = state;

            if ( state == PSM_STATE_COMPLETE )
            {
                psm_core_send_psm_status_to_allclients(PSM_STATUS_COMPLETE, PSM_REJECT_REASON_NONE);
            }
        }
    }

    PSM_LOG_FUNC_RETURN("psm_core_set_psm_state: function return");
}

/*========================================================================================
FUNCTION psm_core_gpio_init

Init PSM GPIO configuration

ARGUMENTS
    void

RETURN VALUE
    void
=========================================================================================*/
void psm_core_gpio_init(void)
{
    DALResult eDalResult = DAL_SUCCESS;

    PSM_LOG_FUNC_ENTRY("psm_core_gpio_init: function entry");

    if( TRUE == psm_gpio_enabled )
    {
        eDalResult = DAL_DeviceAttach(DALDEVICEID_TLMM, &dev_hndl);
        if(( DAL_SUCCESS == eDalResult ) && ( NULL != dev_hndl ))
        {
            gpioConfig = (DALGpioSignalType)DAL_GPIO_CFG(psm_gpio_pin, 0x0,
                             DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA);
            
            eDalResult = DalTlmm_ConfigGpio(dev_hndl, gpioConfig, DAL_TLMM_GPIO_ENABLE);
            PSM_LOG_INFO1("psm_core_gpio_init: Configuring PSM GPIO result: %d", eDalResult);
        }
    }

    PSM_LOG_FUNC_RETURN("psm_core_gpio_init: function return");
}

/*========================================================================================
FUNCTION psm_core_gpio_set

Set PSM GPIO to given DALGpioValue

ARGUMENTS
    DALGpioValueType  eValue

RETURN VALUE
    void
=========================================================================================*/
void psm_core_gpio_set(DALGpioValueType eValue)
{
    DALResult eDalResult = DAL_SUCCESS;

    PSM_LOG_FUNC_ENTRY("psm_core_gpio_set: function entry");

    if(( TRUE == psm_gpio_enabled ) && ( NULL != dev_hndl ))
    {
        eDalResult = DalTlmm_GpioOut(dev_hndl, gpioConfig, eValue);
        PSM_LOG_INFO2("psm_core_gpio_set: Setting PSM GPIO value: %d, result: %d", eValue, eDalResult);
    }

    PSM_LOG_FUNC_RETURN("psm_core_gpio_set: function return");
}

/*========================================================================================
FUNCTION psm_event_thread

Entry function for PSM event thread.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_event_task_entry
(
    dword ignored
)
{
    ril_platform_pipe_attr_t attr;
    psm_event_params_type    buffer;

    PSM_LOG_FUNC_ENTRY("psm_event_task_entry: function entry");

    rcinit_handshake_startup();

    do
    {
        /* Create a pipe */
        ril_platform_pipe_attr_init(&attr);
        ril_platform_pipe_attr_set_elements(&attr, PSM_PIPE_MAX_ELEMENTS);
        ril_platform_pipe_attr_set_element_size(&attr, sizeof(buffer));
        if ( 0 != ril_platform_pipe_create (&event_pipe, &attr) )
        {
            PSM_LOG_INFO("psm_event_task_entry: Error creating pipe");
            break;
        }

        while(1)
        {
            /* Block waiting for a event to be put on the queue */
            ril_platform_pipe_receive (event_pipe, &buffer);
            psm_core_process_event(&buffer);
        }

    } while(FALSE);

    PSM_LOG_FUNC_RETURN("psm_event_task_entry: function return");
    return 0;
}

/*========================================================================================
FUNCTION psm_core_check_if_ready_for_shutdown

Shutdown device if all apps are ready to enter PSM mode.

ARGUMENTS
    uint8_t updated periodic time valid
    uint32_t updated periodic time value

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_check_if_ready_for_shutdown
(
    uint8_t     updated_time_valid,
    uint32_t    updated_time,
    uint8_t     modem_only_time_valid,
    uint32_t    modem_only_time
)
{
    int      rc = 0;
    uint32   time_in_psm = 0;
    PSM_LOG_FUNC_ENTRY("psm_core_check_if_ready_for_shutdown: function entry");

    if(modem_only_time_valid)
    {
        /* For AP-only boot enabled flavors, this info is important
           Updated Time -> Next Wakeup time for Modem + AP
           Modem only Time -> Next Wakeup time for the Modem
        */
        psm_list_add_auto_ready_info(modem_only_time);
        psm_qmi_update_periodic_timer_info(modem_only_time);
        PSM_CORE_ULOG_MSG("Modem-only Time: %d", modem_only_time);
        PSM_LOG_INFO1("psm_core_check_if_ready_for_shutdown: Modem-only Time: %d", modem_only_time);
    }

    if( PSM_MODEM_LOAD_IN_PROGRESS != modem_load_status )
    {
        pending_shutdown = FALSE;
        do
        {
            PSM_CORE_ULOG_MSG("Ready for shutdown function called");
            rc = psm_list_get_psm_time(&time_in_psm, NULL);
            if ( rc != 0 || time_in_psm <= 0 )
            {
                PSM_LOG_INFO("psm_core_check_if_ready_for_shutdown: Failed to retrieve psm time");
                break;
            }

            if( (TRUE == updated_time_valid) &&
                    (time_in_psm > (int) updated_time) && (0 < (int) updated_time) )
            {
                time_in_psm = (int) updated_time;
                PSM_LOG_INFO(
                  "psm_core_check_if_ready_for_shutdown: Updating psm time received in complete indication");
            }
            rc = psm_core_adjust_psm_bootup_time(&time_in_psm);
            if( rc != 0 || time_in_psm <= 0 )
            {
                PSM_LOG_INFO("psm_core_check_if_ready_for_shutdown: Failed to adjust psm bootup time");
                break;
            }
        
            rc = psm_core_adjust_psm_randomization_time(&time_in_psm);
            if( rc != 0 || time_in_psm <= 0 )
            {
                PSM_LOG_INFO("psm_core_check_if_ready_for_shutdown: Failed to adjust psm randomization time");
                break;
            }

            psm_core_shutdown_device(time_in_psm);
        }while(FALSE);
    }
    else
    {
        pending_shutdown = TRUE;
        PSM_LOG_INFO("psm_core_check_if_ready_for_shutdown: Postpone shutdown. Modem load in progress");
        PSM_CORE_ULOG_MSG("Postpone shutdown. Modem load in progress");
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_core_handle_event_active_timer_expiry

Handle active time expiry event.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_event_active_timer_expiry(psm_event_params_type *parms)
{
    int rc = 0;

    PSM_LOG_FUNC_ENTRY("psm_core_handle_event_active_timer_expiry: function entry");

    rc = psm_core_set_psm_ready();
    if ( rc != 0 )
    {
        PSM_LOG_INFO("Failed to send Ready to enter PSM mode request to modem");
    }

    PSM_LOG_FUNC_RETURN("psm_core_handle_event_active_timer_expiry: function return");
    return rc;
}


/*========================================================================================
FUNCTION psm_active_timer_expiry_cb

Active time expiry call back.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
void psm_active_timer_expiry_cb(void *timer_expiry_cb_data)
{

    PSM_LOG_FUNC_ENTRY("psm_active_timer_expiry_cb: function entry");

    ril_platform_timer_delete(&active_timer_id);
    PSM_CORE_ULOG_MSG("Active timer expired");
    psm_queue_event(PSM_EVT_ACTIVE_TIMER_EXPIRY,
                    NIL,
                    NULL,
                    NIL);

    PSM_LOG_FUNC_RETURN("psm_active_timer_expiry_cb: function return");
}


/*========================================================================================
FUNCTION psm_core_check_if_enter_psm_ready

Check if all apps ready to Enter PSM mode.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_check_if_enter_psm_ready(void)
{
    int     rc = 0;
    uint32  max_active_time = 0;

    PSM_LOG_FUNC_ENTRY("psm_core_check_if_enter_psm_ready :function entry");

    do
    {
        rc = psm_list_check_all_clients_ready();
        if ( rc != 0 )
        {
            PSM_LOG_INFO("psm_core_check_if_enter_psm_ready: One of the client or modem is not ready to enter PSM mode");
            PSM_LOG_MSG_CRITICAL("psm_core_check_if_enter_psm_ready: One of the client or modem is not ready to enter PSM mode");
            break;
        }
        PSM_CORE_ULOG_MSG("All clients ready to enter PSM");

        rc = psm_core_send_psm_status_to_allclients(PSM_STATUS_READY, PSM_REJECT_REASON_NONE);
        if ( rc != 0 )
        {
            PSM_LOG_INFO("psm_core_check_if_enter_psm_ready: Failed to send status to clients");
        }

        if( PSM_MODEM_NOT_LOADED == modem_load_status )
        {
            psm_core_set_psm_state(PSM_STATE_COMPLETE);
            psm_core_check_if_ready_for_shutdown(FALSE, NULL, FALSE, NULL);
        }
        else
        {
            rc = psm_list_get_max_active_time(&max_active_time);
            if ( rc != 0 )
            {
                // Ideally should not enter here.. just a safety check.
                PSM_LOG_INFO("psm_core_check_if_enter_psm_ready: Failed to retrieve max active time");
            }
            PSM_CORE_ULOG_MSG("Active timer: %d", max_active_time);

            rc = psm_qmi_set_psm_active_time(max_active_time);
            if ( rc != 0 )
            {
                PSM_LOG_INFO("psm_core_check_if_enter_psm_ready: Failed to set active time");
            }

            if ( (max_active_time > 0) && (rcvd_auto_ready == FALSE) )
            {
                PSM_LOG_INFO("psm_core_check_if_enter_psm_ready: Start active timer");
                ril_platform_timer_start_oneshot(&active_timer_id, psm_active_timer_expiry_cb, NULL, TIMER_1S * max_active_time);
                PSM_CORE_ULOG_MSG("Active timer started");
            }
            else
            {
                rc = psm_core_set_psm_ready();
                if ( rc != 0 )
                {
                    PSM_LOG_INFO("Failed to send Ready to enter PSM mode request to modem");
                }
            }
        }
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_core_set_psm_ready

Send PSM ready request to modem.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_set_psm_ready(void)
{
    int    rc = 0;
    int    accept = TRUE;
    int    send_enter_psm = TRUE;
    uint32 least_psm_time = 0;

    PSM_LOG_FUNC_ENTRY("psm_core_set_psm_ready: function entry");

    do
    {
        PSM_CORE_ULOG_MSG("Setting PSM ready");
        rc = psm_list_check_all_clients_ready();
        if ( rc != 0 )
        {
            PSM_LOG_INFO("some apps psm time might have been expired.. Send not ready to clients.");
            psm_core_send_psm_status_to_allclients(PSM_STATUS_NOT_READY, PSM_REJECT_REASON_NONE);
        }
        else
        {
            rc = psm_list_get_psm_time(&least_psm_time, NULL);
            PSM_CORE_ULOG_MSG("PSM Time: %u", least_psm_time);
            if ( rc !=0 )
            {
                PSM_LOG_INFO("psm_core_set_psm_ready: Failed to retrieve psm time");
                psm_core_send_psm_status_to_allclients(PSM_STATUS_NOT_READY, PSM_REJECT_REASON_NONE);
            }
        }

        if(rc)
        {
            send_enter_psm = FALSE;
            /* For Modem controlled PSM, we need to send REJECT to modem if the current 
               state is PSM_STATE_MODEM_READY_AP_AWAITED */
            if(psm_core_get_psm_state() == PSM_STATE_MODEM_READY_AP_AWAITED)
            {
                PSM_CORE_ULOG_MSG("Sending Enter PSM with reject");
                PSM_LOG_INFO("psm_core_set_psm_ready: Sending Enter PSM with reject");
                accept = FALSE;
                send_enter_psm = TRUE;
                psm_core_set_psm_state(PSM_STATE_MODEM_READY_AP_REJECTED);
            }
        }

        if(send_enter_psm)
        {
            rc = psm_qmi_set_psm_ready(least_psm_time, accept);
            if ( rc != 0 )
            {
                PSM_LOG_INFO("psm_core_set_psm_ready: Failed to send PSM READY to modem");
                break;
            }
            if(accept)
            {
                PSM_CORE_ULOG_MSG("Sent request to modem for entering PSM. Waiting for Complete indication: %d", rc);
                PSM_LOG_INFO1("Sent request to modem for entering PSM. Waiting for Complete indication: %d", rc);
                PSM_LOG_MSG_CRITICAL("Sent request to modem for entering PSM. Waiting for Complete indication: %d", rc);
            }
        }

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_core_set_no_client_oos

Handler for OOS/Limited Service with no clients

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_set_no_client_oos(void)
{
    int   oos_time = 0;
    int   rc = FAIL;

    PSM_LOG_FUNC_ENTRY("psm_core_set_no_client_oos: function entry");

    psm_core_stop_inactivity_timer();

    oos_time = psm_qmi_get_backoff_psmtime();
    psm_list_add_oos_info(oos_time, TRUE);

    rc = psm_core_set_psm_ready();
    if ( rc != 0 )
    {
        PSM_LOG_INFO("psm_core_set_no_client_oos: Failed to send Ready to enter PSM mode request to modem");
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_core_inactivity_timer_expiry_cb

Handler for inactivity timer expiry

ARGUMENTS
    void

RETURN VALUE
    void
=========================================================================================*/

void psm_core_inactivity_timer_expiry_cb(void *timer_expiry_cb_data)
{
        PSM_LOG_FUNC_ENTRY("psm_core_inactivity_timer_expiry_cb: function entry");

        PSM_CORE_ULOG_MSG("Inactivity timer expired");
        psm_queue_event(PSM_EVT_INACTIVITY_TIMER_EXPIRY,
                        NIL,
                        NULL,
                        NIL);

        PSM_LOG_FUNC_RETURN("psm_core_inactivity_timer_expiry_cb: function entry");
}

/*========================================================================================
FUNCTION psm_core_handle_inactivity_timer_expiry

Handle Inactivity Timer expiry request.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_inactivity_timer_expiry(psm_event_params_type *parms)
{
    int   oos_time = 0;
    int   rc = FAIL;
    int   load_modem;

    PSM_LOG_FUNC_ENTRY("psm_core_handle_inactivity_timer_expiry: function entry");

    psm_core_stop_inactivity_timer();

    oos_time = psm_qmi_get_backoff_psmtime();

    if( PSM_MODEM_LOADED == modem_load_status )
    {
        load_modem = TRUE;
    }
    else
    {
        load_modem = FALSE;
    }
    psm_list_add_oos_info(oos_time, load_modem);

    rc = psm_core_set_psm_ready();
    if ( rc != 0 )
    {
        PSM_LOG_INFO("Failed to send Ready to enter PSM mode request to modem");
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_core_start_inactivity_timer

Start the PSM inactivity timer.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/

int psm_core_start_inactivity_timer(void)
{
    int   rc = FAIL;
    PSM_LOG_FUNC_ENTRY("psm_core_start_inactivity_timer: function entry");

    if(( TRUE == psm_inactivity_enabled ) && ( 0 < psm_inactivity_timer_value ) && 
                     ( psm_inactivity_timerid.timer_state == TIMER_UNDEF_FLAG ))
    {
        PSM_LOG_INFO1("psm_core_start_inactivity_timer: Starting Inactivity timer for: %d", psm_inactivity_timer_value);
        PSM_CORE_ULOG_MSG("Starting Inactivity timer for: %d", psm_inactivity_timer_value);
        ril_platform_timer_start_oneshot(&psm_inactivity_timerid,
            psm_core_inactivity_timer_expiry_cb, NULL, TIMER_1S * psm_inactivity_timer_value);
        rc = SUCCESS;
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_core_stop_inactivity_timer

Stop the PSM inactivity timer.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/

int psm_core_stop_inactivity_timer(void)
{
    int   rc = FAIL;

    if ( psm_inactivity_timerid.timer_state != TIMER_UNDEF_FLAG )
    {
        ril_platform_timer_stop(&psm_inactivity_timerid);
        ril_platform_timer_delete(&psm_inactivity_timerid);
        rc = SUCCESS;
        PSM_LOG_INFO("psm_core_stop_inactivity_timer: Stopped/Deleted Inactivity timer.");
        PSM_CORE_ULOG_MSG("Stopped Inactivity timer");
    }
    psm_list_remove_client(PSM_LIST_OOS_CLIENT_FD);

    return rc;
}

/*========================================================================================
FUNCTION psm_core_process_event

Event thread entry function. Event thread is the only thread which does processing.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_process_event(psm_event_params_type *parms)
{
    int                   iter_map = 0;
    int                   event_table_len = 0;
    int                   rc = 0;
    int                   event_id;
    psm_req_handler_type  handler = NULL;

    PSM_LOG_FUNC_ENTRY("psm_core_process_event: function entry");

    do
    {
        if ( parms != NULL )
        {
            event_id = parms->event_id;

            PSM_LOG_INFO1("psm_core_process_event: Processing event %d",event_id);

            if ( event_id >= PSM_EVT_FIRST && event_id <= PSM_EVT_LAST )
            {
                event_table_len = sizeof(qcril_event_table)/sizeof(qcril_event_table[0]);

                for(iter_map = 0; iter_map < event_table_len; iter_map++)
                {
                    if(qcril_event_table[iter_map].event_id == event_id)
                    {
                        handler = qcril_event_table[iter_map].handler;
                        break;
                    }
                }
            }

            if ( handler )
            {
                (*handler)(parms);
            }
            if ( parms && parms->data )
            {
                free(parms->data);
                parms->data = NULL;
            }
        }
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_queue_event

Post request for event thread processing.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_queue_event(int event, int client_fd, void *data, size_t data_len)
{
    int                    rc = 0;
    psm_event_params_type *parms = NULL;

    PSM_LOG_FUNC_ENTRY("psm_queue_event: function entry");

    parms = malloc(sizeof(*parms));
    if ( parms != NULL )
    {
        memset(parms, 0, sizeof(psm_event_params_type));

        parms->event_id  = event;
        parms->client_fd = client_fd;

        if ( data && data_len > 0 )
        {
            parms->data = malloc(data_len);
            if ( parms->data != NULL )
            {
                memset(parms->data, 0, data_len);
                memscpy(parms->data, data_len, data, data_len);
            }
        }

        ril_platform_pipe_send(event_pipe, parms);
        if (parms) {
            free(parms);
            parms = NULL;
        }
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_core_send_psm_status_to_allclients

Sends psm status to all registered clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_send_psm_status_to_allclients
(
    psm_status_type_e        status,
    psm_reject_reason_type_e reason
)
{
    return psm_list_send_psmstatus_to_allclients(status, reason);
}


/*========================================================================================
FUNCTION psm_core_send_psm_status

Sends psm status to a particular client.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_send_psm_status(int client_fd, psm_status_type_e status, psm_reject_reason_type_e reason)
{
    int           rc = 0;
    psm_status_msg_type resp_msg;

    PSM_LOG_FUNC_ENTRY("psm_core_send_psm_status: function entry");

    memset(&resp_msg, 0, sizeof(resp_msg));
    resp_msg.client_id = client_fd;
    resp_msg.status    = status;
    resp_msg.reason    = reason;

    PSM_LOG_INFO3("psm_core_send_psm_status: Send psm status - %d with reason - %d to client with fd - %d",
                                                            status, reason, client_fd);

    rc = ril_platform_pipe_send(server_pipe, &resp_msg);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_core_adjust_psm_bootup_time

Adjust psm time to consider the bootup time delay

ARGUMENTS
    time in psm

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_core_adjust_psm_bootup_time(uint32 *time_in_psm)
{
    int      rc = FAIL;
    int      bootup_delay_time;
    uint32   adjusted_time;

    PSM_LOG_FUNC_ENTRY("psm_core_adjust_psm_bootup_time: function entry");

    // First check if bootup delay from NV
    bootup_delay_time = psm_qmi_get_bootup_delay_from_configuration();
    
    if( bootup_delay_time < 0)
    {
        // Bootup delay not in NV, fallback to EFS or Defaults
        PSM_LOG_INFO("Bootup delay not in NV, fallback to EFS/Default values");
        if( psm_bootup_delay >= 0 )
        {
            bootup_delay_time = psm_bootup_delay;
        }
        else
        {
            bootup_delay_time = PSM_BOOTUP_DELAY;
        }
    }
    adjusted_time = *time_in_psm - bootup_delay_time;
    PSM_LOG_INFO3("psm_core_adjust_psm_bootup_time: PSM time: %u, Bootup delay: %u, Adjusted PSM time: %u",
                                *time_in_psm, bootup_delay_time, adjusted_time);

    *time_in_psm = adjusted_time;

    if( *time_in_psm > 0 )
    {
        rc = SUCCESS;
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_core_adjust_psm_randomization_time

Adjust psm time to consider the bootup time delay

ARGUMENTS
    time in psm

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_core_adjust_psm_randomization_time(uint32 *time_in_psm)
{
    int     rc = FAIL;
    uint32  adjusted_time;

    PSM_LOG_FUNC_ENTRY("psm_core_adjust_psm_randomization_time: function entry");

    adjusted_time = *time_in_psm - randomization_time;
    PSM_LOG_INFO3("PSM time:%u, Randomization time:%u, Adjusted PSM time:%u",
                                *time_in_psm, randomization_time, adjusted_time);

    *time_in_psm = adjusted_time;

    if( *time_in_psm > 0 )
    {
        rc = SUCCESS;
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_core_handle_register_request

Handle register request.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_register_request(psm_event_params_type *parms)
{
    int                 rc = 0;
    //int                 psm_supported = FALSE;
    char                client_fd;
    int                 clients_before_register = 0;
    psm_status_type_e   psm_status;

    PSM_LOG_FUNC_ENTRY("psm_core_handle_register_request: function entry");

    if ( parms != NULL )
    {
         client_fd  = parms->client_fd;
        clients_before_register = psm_list_get_number_of_clients(FALSE);

        /* Irrespective of PSM enabled/disabled add entry as
        ** with rel 12 changes, PSM can be dynamically enabled/disabled.
        ** We need to have client handle to inform dynamic enable/disable. */
        rc = psm_list_add_newclient(client_fd);
        if( SUCCESS == rc && NIL == clients_before_register )
        {
            PSM_LOG_INFO("Atleast one client registered. Request to stop inactivity timer");
            psm_core_stop_inactivity_timer();
        }
        if ( psm_server_hc_started == FALSE )
        {
            ril_platform_timer_start_oneshot(&server_hc_timerid,psm_core_healthcheck_timer_cb, (void*)client_fd, PSM_SERVER_HC_TIMEOUT);
            psm_server_hc_started = TRUE;
        }

        //psm_qmi_is_psm_supported(&psm_supported);
        //if ( psm_supported == FALSE )
        if(psm_qmi_is_psm_supported() == FALSE)
        {
            psm_core_send_status(client_fd, PSM_STATUS_REJECT, PSM_REJECT_REASON_NOT_ENABLED);
        }

        /* Inform newly registered clients whether modem is loaded or not. */
        if ( PSM_MODEM_LOADED == modem_load_status )
        {
            psm_core_send_status(client_fd,PSM_STATUS_MODEM_LOADED,PSM_REJECT_REASON_NONE);

            // Send network status to newly registered client.
            psm_status = psm_qmi_get_psm_oos_status();
            if ( psm_status != PSM_STATUS_NONE )
            {
                psm_core_send_status(client_fd, psm_status, PSM_REJECT_REASON_NONE);
            }
        }
        else
        {
            psm_core_send_status(client_fd,PSM_STATUS_MODEM_NOT_LOADED,PSM_REJECT_REASON_NONE);
        }

    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_core_handle_unregister_request

Handle unregister request.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_unregister_request(psm_event_params_type *parms)
{
    int                 rc = 0;
    int                 remaining_clients;
    char                client_fd;

    PSM_LOG_FUNC_ENTRY("psm_core_handle_unregister_request: function entry");

    if ( parms != NULL )
    {
        client_fd  = parms->client_fd;

        if ( active_timer_id.timer_state != TIMER_UNDEF_FLAG )
        {
            PSM_LOG_INFO("psm_core_handle_unregister_request: Cancel PSM active timer");
            ril_platform_timer_stop (&active_timer_id);
            ril_platform_timer_delete (&active_timer_id);
        }

        rc = psm_list_remove_client(client_fd);
        if ( rc != 0 )
        {
            PSM_LOG_INFO("psm_core_handle_unregister_request: Failed to remove client");
        }

        remaining_clients = psm_list_get_number_of_clients(FALSE);
        if( NIL == remaining_clients )
        {
            PSM_LOG_INFO("No clients left. Start inactivity timer.");
            psm_core_start_inactivity_timer();
        }

        rc = psm_core_check_if_enter_psm_ready();
        if ( rc != 0 )
        {
            PSM_LOG_INFO("psm_core_handle_unregister_request: Not ready to enter PSM mode");
        }
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_core_send_status

Send status to clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_send_status(int fd, psm_status_type_e status, psm_reject_reason_type_e reason)
{
    return psm_core_send_psm_status( fd, status, reason);
}

/*========================================================================================
FUNCTION psm_core_handle_ready_request

Handle ready request.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_ready_request(psm_event_params_type *parms)
{
    int                 rc = -1;
	int                 start_time_in_psm = 0;
	int                 end_time_in_psm = 0;
	int                 active_time = 0;
	int                 nw_access = 0;
    //int                 psm_supported = FALSE;
    char                client_fd;
    psm_req_msg_type    *client_msg;

    PSM_LOG_FUNC_ENTRY("psm_core_handle_ready_request: function entry");

    do
    {
        if ( parms == NULL )
        {
            PSM_LOG_INFO("psm_core_handle_ready_request: wrong arguments");
            break;
        }
        PSM_CORE_ULOG_MSG("Ready request entered");

        client_fd   = parms->client_fd;
        //psm_qmi_is_psm_supported(&psm_supported);
        //if ( psm_supported == TRUE )
        if(psm_qmi_is_psm_supported() == TRUE)
        {
            client_msg  = parms->data;
            nw_access   = client_msg->req_info.nw_access;
            active_time = client_msg->req_info.active_timer;
            start_time_in_psm = client_msg->req_info.start_timer;
            end_time_in_psm = client_msg->req_info.end_timer;

            PSM_LOG_INFO1("psm_core_handle_ready_request: active time - %d", active_time );
            PSM_LOG_INFO2("psm_core_handle_ready_request: Start PSM time - %d End PSM time %d",
                           start_time_in_psm, end_time_in_psm);
            PSM_LOG_MSG_CRITICAL("psm_core_handle_ready_request: Start PSM time - %d End PSM time %d",
                           start_time_in_psm, end_time_in_psm);

            if ((start_time_in_psm < 0 || end_time_in_psm < 0) || 
                (start_time_in_psm < active_time))
            {
                // Too short PSM time.
                rc = psm_core_send_status(client_fd, PSM_STATUS_REJECT,
                                           PSM_REJECT_REASON_DURATION_TOO_SHORT);
                if ( rc != 0 )
                {
                    PSM_LOG_INFO1("failed to send psm status to client with fd - %d", client_fd);
                }
                break;
            }

            if(TRUE == psm_list_check_client_voted(client_fd))
            {
                PSM_LOG_INFO1("Client with fd: %d already voted with valid time. Exiting", client_fd);
                PSM_CORE_ULOG_MSG("Client with fd: %d already voted with valid time. Exiting", client_fd);
                rc = psm_core_send_status(client_fd, PSM_STATUS_REJECT,
                                                PSM_REJECT_REASON_INCORRECT_USAGE);
                if ( rc != 0 )
                {
                    PSM_LOG_INFO1("failed to send psm status to client with fd - %d", client_fd);
                }
                break;
            }

            rc = psm_list_update_info(client_fd, active_time,
                       start_time_in_psm, end_time_in_psm, nw_access);
            if ( rc != 0 )
            {
                PSM_LOG_INFO1("psm_core_handle_ready_request: Failed to find client with fd - %d", client_fd);
                break;
            }

            rc = psm_core_check_if_enter_psm_ready();
            if ( rc != 0 )
            {
                PSM_LOG_INFO("psm_core_handle_ready_request: Not yet ready to enter PSM mode");
                break;
            }
        }
        else
        {
            psm_core_send_status(client_fd, PSM_STATUS_REJECT, PSM_REJECT_REASON_NOT_ENABLED);
        }


    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_core_handle_backoff_request

Handle ready request.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_backoff_request(psm_event_params_type *parms)
{
    int                 rc = -1;
    int                 time_in_psm = 0;
    int                 active_time = 0;
    int                 nw_access = 0;
    //int                 psm_enabled = FALSE;
    char                client_fd;

    PSM_LOG_FUNC_ENTRY("psm_core_handle_backoff_request: function entry");

    do
    {
        //psm_qmi_is_psm_supported(&psm_enabled);
        //if( FALSE == psm_enabled )
        if(psm_qmi_is_psm_supported() == FALSE)
        {
            PSM_LOG_INFO("PSM not enabled. Not handling backoff");
            break; 
        }

        if ( parms == NULL )
        {
            PSM_LOG_INFO("psm_core_handle_backoff_request: wrong arguments");
            break;
        }

        client_fd   = parms->client_fd;
        nw_access   = 1;
        active_time = 0;
        time_in_psm = psm_qmi_get_backoff_psmtime();

         rc = psm_list_update_info(client_fd, active_time, time_in_psm, time_in_psm, nw_access);
        if ( rc != 0 )
        {
            PSM_LOG_INFO1("Failed to find client with fd - %d", client_fd);
            break;
        }

        rc = psm_core_check_if_enter_psm_ready();
        if ( rc != 0 )
        {
            PSM_LOG_INFO("psm_core_handle_backoff_request: Not yet ready to enter PSM mode");
            break;
        }

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}


/*========================================================================================
FUNCTION psm_core_handle_not_ready_request

Handle not ready request.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_not_ready_request(psm_event_params_type *parms)
{
    //int                 rc = 0;
    char                client_fd;

    PSM_LOG_FUNC_ENTRY("psm_core_handle_not_ready_request: function entry");

    if ( parms != NULL )
    {
        PSM_LOG_INFO("reset PSM time info as application is not ready to enter PSM");
        client_fd  = parms->client_fd;
        psm_list_reset_timeinfo(client_fd);

        if ( active_timer_id.timer_state != TIMER_UNDEF_FLAG )
        {
            PSM_LOG_INFO("Cancel PSM active timer");
            ril_platform_timer_stop (&active_timer_id);
            ril_platform_timer_delete (&active_timer_id);
            psm_core_set_psm_ready();
        }
    }

    PSM_LOG_FUNC_RETURN("psm_core_handle_not_ready_request: function return");
    //return rc;
    return SUCCESS;
}

/*==============================================================================
FUNCTION psm_core_healthcheck_timer_cb

Server health check call back

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
void psm_core_healthcheck_timer_cb(void *timer_expiry_cb_data)
{
    PSM_LOG_FUNC_ENTRY("psm_core_healthcheck_timer_cb: function entry");

    psm_queue_event(PSM_EVT_SERVER_HEALTH_CHECK, -1, NULL, NIL);

    PSM_LOG_FUNC_RETURN("psm_core_healthcheck_timer_cb: function return");
}

/*========================================================================================
FUNCTION psm_core_handle_server_health_check_request

Handle health check.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_server_health_check_request(psm_event_params_type *parms)
{
    int                 rc = 0;
    int                 ready_for_psm = FALSE;

    PSM_LOG_FUNC_ENTRY("psm_core_handle_server_health_check_request: function entry");

    if ( parms != NULL )
    {
        do
        {
            rc = psm_list_handle_healthcheck();
            if ( rc != 0 )
            {
                rc = psm_core_check_if_enter_psm_ready();
                if ( rc != 0 )
                {
                    PSM_LOG_INFO("Not yet ready to enter PSM mode");
                }
                else
                {
                    ready_for_psm = TRUE;
                }
            }

            /* If ready to enter PSM mode, don't start timer again */
            if ( !ready_for_psm )
            {
                psm_list_reset_healthcheck_flag();
                ril_platform_timer_restart(&server_hc_timerid, PSM_SERVER_HC_TIMEOUT);
            }
       }while(FALSE);
    }

    PSM_LOG_FUNC_RETURN("psm_core_handle_server_health_check_request: function return");
    return 0;
}


/*========================================================================================
FUNCTION psm_core_handle_client_health_check_request

Handle health check from clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_client_health_check_request(psm_event_params_type *parms)
{
    char                client_fd;

    PSM_LOG_FUNC_ENTRY("psm_core_handle_client_health_check_request: function entry");

    if ( parms != NULL )
    {
        do
        {
            client_fd  = parms->client_fd;
            PSM_LOG_INFO1("health check from client with fd - %d", client_fd);
            psm_list_client_healthcheck(client_fd);

        }while(FALSE);
    }

    PSM_LOG_FUNC_RETURN("psm_core_handle_client_health_check_request: function return");
    return 0;
}

/*========================================================================================
FUNCTION psm_core_handle_load_modem_request

Handle request to load modem.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_load_modem_request(psm_event_params_type *parms)
{
    int                 rc = 0;
    char                client_fd;
    psm_boot_type_e     boot_type = OUT_OF_RESET;

    PSM_LOG_FUNC_ENTRY("psm_core_handle_load_modem_request: function entry");

    if ( parms != NULL )
    {
        do
        {
            client_fd  = parms->client_fd;

            PSM_LOG_INFO1("client with fd - %d has requested for modem", client_fd);
/* Hibernation is not supported in 9x05 so no need of checking boot type for now */
#ifndef FEATURE_PSM_TARGET_9205
            psm_qmi_get_boot_type(&boot_type);
#endif


            if( PSM_MODEM_NOT_LOADED == modem_load_status )
            {
                psm_qmi_start_modem_subsystem(boot_type);
            }
            else if( PSM_MODEM_LOADED == modem_load_status )
            {
                psm_core_send_psm_status(client_fd,PSM_STATUS_MODEM_LOADED,PSM_REJECT_REASON_NONE);
            }

        }while(FALSE);
    }

    PSM_LOG_FUNC_RETURN("psm_core_handle_load_modem_request: function return");
    return rc;
}

/*========================================================================================
FUNCTION psm_core_handle_auto_ready

Handles READY and AUTOREADY received from modem.

Detailed:
    In modem controlled PSM version :-
        1. AUTOREADY and READY can be sent with periodic_update_timer filled optionally.

    In AP controlled PSM Version :-
        1. AUTOREADY indication has to include the periodic_update_timer
        2. READY indication may include periodic_update_timer (i.e optional)

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_auto_ready(psm_event_params_type *parms)
{
    int                               rc = 0;
    psm_qmi_ready_indication_payload *tau_ptr = NULL;
    PSM_LOG_FUNC_ENTRY("psm_core_handle_auto_ready: function entry");

    if ( parms != NULL )
    {
        do
        {
            tau_ptr = (psm_qmi_ready_indication_payload*)parms->data;
            if(!tau_ptr)
            {
                PSM_LOG_INFO("Ready indication payload is NULL");
                break;
            }

            PSM_LOG_INFO("Deleting Inactivity Timer since READY/AUTOREADY Recieved");
            rcvd_auto_ready = TRUE;
            psm_core_stop_inactivity_timer();

            /* Fill with the default Modem vote as highest possible time for 9205 */
                tau_ptr->periodic_update_timer_valid = 1;
                tau_ptr->periodic_update_timer = 0xFFFFFFFF;

            if(tau_ptr->periodic_update_timer_valid)
            {
                PSM_LOG_INFO1("psm_core_handle_auto_ready: tau timer - %d", tau_ptr->periodic_update_timer);
                PSM_CORE_ULOG_MSG("tau timer: %d", tau_ptr->periodic_update_timer);

                psm_list_add_auto_ready_info(tau_ptr->periodic_update_timer);
                psm_qmi_update_periodic_timer_info(tau_ptr->periodic_update_timer);
                PSM_LOG_INFO("psm_core_handle_auto_ready: Updated the tau timer in the backup");
            }

            if ( active_timer_id.timer_state != TIMER_UNDEF_FLAG )
            {
                ril_platform_timer_stop(&active_timer_id);
                ril_platform_timer_delete(&active_timer_id);
            }

            rc = psm_core_set_psm_ready();
            if ( rc != 0 )
            {
                PSM_LOG_INFO("Failed to send Ready to enter PSM mode request to modem");
            }
        }while(FALSE);
    }

    PSM_LOG_FUNC_RETURN("psm_core_handle_auto_ready: function return");
    return 0;
}

/*========================================================================================
FUNCTION psm_core_read_config_file

Read server config file

ARGUMENTS
    void

RETURN VALUE
    void
=========================================================================================*/
void psm_core_read_config_file(void)
{
    void *config_list = NULL;
    config_list = util_config_load(PSM_SERVER_CONFIG_FILE);
    util_config_get_value(config_list, "GPIO_ENABLED", INTEGER, &psm_gpio_enabled);
    util_config_get_value(config_list, "GPIO_PIN_NUMBER", INTEGER, &psm_gpio_pin);
    util_config_get_value(config_list, "BOOTUP_DELAY", INTEGER, &psm_bootup_delay);
    util_config_get_value(config_list, "INACTIVITY_TIMER", INTEGER, &psm_inactivity_timer_value);
    util_config_get_value(config_list, "HIBERNATION_THRESHOLD", INTEGER, &psm_hibernation_threshold);
    util_config_get_value(config_list, "PSM_PULL_DEFAULT_DURATION", INTEGER, &psm_pull_duration_time);
    util_config_get_value(config_list, "PON_SUPPRESS", INTEGER, &psm_pon_suppress_enabled);
    util_config_unload(config_list);

    PSM_CORE_ULOG_MSG("Inactivity timer: %d", psm_inactivity_timer_value);
    PSM_CORE_ULOG_MSG("Hibernation threshold: %d", psm_hibernation_threshold);

    //Handle initializations
    if( 0 > psm_gpio_enabled || 1 < psm_gpio_enabled )
    {
        psm_gpio_enabled = PSM_GPIO_DEFAULT_STATE;
    }
    if( 0 > psm_gpio_pin )
    {
        psm_gpio_pin = PSM_GPIO_DEFAULT_PIN;
    }

    if( 0 < psm_inactivity_timer_value )
    {
        psm_inactivity_enabled = TRUE;
    }
    else
    {
        psm_inactivity_enabled = FALSE;
    }

    if(psm_pon_suppress_enabled != TRUE && psm_pon_suppress_enabled != FALSE )
    {
        psm_pon_suppress_enabled = TRUE;
    }

}

/*==============================================================================
FUNCTION psm_core_init

Initializes psm core

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_core_init(void)
{
    int            rc = -1;

    do
    {
        psm_core_read_config_file();
        psm_core_gpio_init();

        /*To-do: Should optimize this to not mkdir if already present*/
        efs_mkdir("/ril", 0777);
        efs_mkdir("/ril/psm", 0777);
        rc = psm_rtc_enable_rtc();
        if ( rc != 0 )
        {
            PSM_LOG_INFO("psm_core_init: Failed to enable RTC alarm functionality");
            PSM_CORE_ULOG_MSG("RTC Enable Failed");
            break;
        }

        psm_core_stop_inactivity_timer(); //As a safe check to delete previous timers set for future.
        rc = psm_qmi_init();
        if ( rc != 0 )
        {
            PSM_LOG_INFO("psm_core_init: Failed to initialize qmi clients");
        }

#if defined(FEATURE_PSM_TARGET_9205) && !defined(PSM_APPS_ONLY_BOOT_SUPPORTED)
            PSM_LOG_INFO("Disable Inactivity timer. Modem Controlled PSM Enabled");
            PSM_CORE_ULOG_MSG("Disable Inactivity timer. Modem Controlled PSM Enabled");
            psm_inactivity_enabled = FALSE;
#else
            psm_core_start_inactivity_timer();
#endif
        if(psm_qmi_is_psm_supported() == TRUE)
        {
            if(psm_pon_suppress_enabled == TRUE)
            {
                pm_pon_trigger_enable(0x0, PM_PON_TRIGGER_PON1, PSM_USB_PON_ENABLE); 
                PSM_LOG_INFO("psm_core_init: Triggered USB PON api enable");
		    }
            else
            {
                PSM_LOG_INFO("psm_core_init: USB PON Suppression set to FALSE. Skipping..");
            }
        }
        else
        {
            psm_core_stop_inactivity_timer();
        }

        // fetch at bootup and use same randomization time for all clients.
        randomization_time = psm_qmi_get_randomization_time();
        PSM_LOG_INFO1("psm_core_init: randomization time - %d", randomization_time);
        PSM_CORE_ULOG_MSG("Core Init completed");
        PSM_LOG_MSG_CRITICAL("Core Init completed");

    } while (FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_core_server_init

PSM socket initialization function.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_server_init(void)
{
    int                      ret = SUCCESS;
    ril_platform_pipe_attr_t attr;

    PSM_CRITICAL_ULOG_INIT();
    do
    {

        /* Create a pipe */
        ril_platform_pipe_attr_init(&attr);
        ril_platform_pipe_attr_set_elements(&attr, PSM_PIPE_MAX_ELEMENTS);
        ril_platform_pipe_attr_set_element_size(&attr, sizeof(psm_status_msg_type));
        if ( 0 != ril_platform_pipe_create (&server_pipe, &attr) )
        {
            PSM_LOG_INFO("psm_core_server_init: Error creating server pipe");
            ret = FAIL;
            break;
        }
        ril_platform_pipe_attr_init(&attr);
        ril_platform_pipe_attr_set_elements(&attr, PSM_PIPE_MAX_ELEMENTS);
        ril_platform_pipe_attr_set_element_size(&attr, sizeof(psm_req_msg_type));
        if ( 0 != ril_platform_pipe_create (&client_pipe, &attr) )
        {
            PSM_LOG_INFO("psm_core_server_init: Error creating client pipe");
            ret = FAIL;
            break;
        }
        PSM_CORE_ULOG_MSG("Core server init completed");
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(ret);
    return ret;
}

/*========================================================================================
FUNCTION psm_do_listen_client

Seperate thread function for monitoring clients and recieving client data.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
========================================================================================*/
void psm_do_listen_client(void)
{
    psm_req_msg_type req;

    while(1)
    {
        /* Block waiting for a event to be put on the queue */
        ril_platform_pipe_receive (client_pipe, &req);
        psm_core_process_request_from_client(&req);
    }

}


/*========================================================================================
FUNCTION psm_core_process_request_from_client

Post requests received from clients to event thread.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
========================================================================================*/
int psm_core_process_request_from_client(psm_req_msg_type *client_msg)
{
    int              rc = 0;
    int              event = -1;

    PSM_LOG_FUNC_ENTRY("psm_core_process_request_from_client: function entry");

    do
    {
        if ( client_msg == NULL )
        {
            rc = -1;
            break;
        }

        switch ( client_msg->request )
        {
            case PSM_REQUEST_PSM_REGISTER:
                event = PSM_EVT_PSM_REGISTER;
                break;

            case PSM_REQUEST_PSM_READY:
                event = PSM_EVT_PSM_READY;
                break;

            case PSM_REQUEST_PSM_NOT_READY:
                event = PSM_EVT_PSM_NOT_READY;
                break;

            case PSM_REQUEST_PSM_UNREGISTER:
                event = PSM_EVT_PSM_UNREGISTER;
                break;

            case PSM_REQUEST_LOAD_MODEM:
                event = PSM_EVT_LOAD_MODEM;
                break;

            case PSM_REQUEST_HEALTH_CHECK:
                event = PSM_EVT_CLIENT_HEALTH_CHECK;
                break;

            case PSM_REQUEST_BACKOFF:
                event = PSM_EVT_CLIENT_BACKOFF;
                break;

            default:
                PSM_LOG_INFO1("psm_core_process_request_from_client: unhandled request - %d", client_msg->request);
                break;
        }

        if ( event != -1 )
        {
            rc = psm_queue_event(event, client_msg->client_id, client_msg,sizeof(*client_msg));
        }
    } while(FALSE);

    PSM_LOG_FUNC_RETURN("psm_core_process_request_from_client: function return");
    return rc;
}

int psm_core_read_modem_loading_flag(int *load_modem)
{
    int         rc = 0;
    int         fd = -1;
    char        buffer[2];

    PSM_LOG_FUNC_ENTRY("psm_qmi_read_modem_loading_flag :function entry");

    do
    {
        if ( load_modem == NULL )
        {
            rc = -1;
            break;
        }

        fd = efs_open (PSM_MODEM_FLAG_FILE, O_RDONLY);
        if ( fd < 0 )
        {
            PSM_LOG_INFO_STR("psm_qmi_read_modem_loading_flag: Failed to open modem loading flag file %s",
                              strerror(errno));
            *load_modem = -1;
            rc = -1;
            break;
        }

        efs_read(fd, buffer, 2);
        *load_modem = atoi(buffer);
        PSM_CORE_ULOG_MSG("Load modem flag in file: %d", *load_modem);
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_core_shutdown_device

Shutdown device.

ARGUMENTS
    psm_time The time requested to be in PSM

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_core_shutdown_device(int psm_time)
{
    int       rc = 0;
    int      fd = INVALID_FILE_HANDLE;
    char      buffer[2];
    int       modem_at_bootup = 1;
    boolean update_file = TRUE;
    int load_status = -1;

    PSM_LOG_FUNC_ENTRY("psm_core_shutdown_device: function entry");

    psm_list_get_psm_time(NULL, &modem_at_bootup);
    PSM_LOG_INFO1("psm_core_shutdown_device: load modem at next boot  - %d", modem_at_bootup);

    if(psm_core_read_modem_loading_flag(&load_status) == 0) {
      if (load_status == modem_at_bootup) {
        PSM_LOG_INFO1("psm_core_shutdown_device: same status in cache %d",load_status);
        update_file = FALSE;
      }
    }

    if (update_file) {
      efs_mkdir(PSM_MODEM_FLAG_DIR, 0666);
      fd = efs_open(PSM_MODEM_FLAG_FILE, O_WRONLY|O_CREAT);

      if(fd != INVALID_FILE_HANDLE)
      {
        strlcpy(buffer, (modem_at_bootup)?"1":"0", sizeof(buffer));
        efs_write(fd, buffer, sizeof(buffer));
        efs_close(fd);
      }
    }
    PSM_LOG_INFO("psm_core_shutdown_device: Shutting down device");
    if(psm_qmi_is_psm_supported())
    {
        psm_core_gpio_set(PSM_DAEMON_END);
        if(psm_pon_suppress_enabled)
        {
            pm_pon_trigger_enable(0x0, PM_PON_TRIGGER_PON1, PSM_USB_PON_DISABLE); 
            PSM_LOG_INFO("USB PON disabled");
        }
        else
        {
            PSM_LOG_INFO("USB PON Suppression set to FALSE. Skipping..");
        }
    }

// TODO:- We will need alternative hibernation mechanism in NO-MODEM case

    if((psm_time <= psm_hibernation_threshold) && (psm_hibernation_threshold != NIL))
    {
        PSM_CORE_ULOG_MSG("Within Hibernation threshold, Using RBSp");
        PSM_LOG_INFO("Within Hibernation threshold, Using RBSp");
        sys_m_hibernation(psm_time*TIMER_1S);
    }
    else
    {

        PSM_CORE_ULOG_MSG("Normal PSM Shutdown");
        PSM_LOG_INFO("psm_core_shutdown_device: Normal PSM Shutdown");

#ifdef PSM_APPS_ONLY_BOOT_SUPPORTED
        /* RTC Must be set by PSMD if AP-only boot feature is supported */
            PSM_CORE_ULOG_MSG("Set PMIC Alarm for %d seconds", psm_time);
            PSM_LOG_INFO1("psm_core_shutdown_device: Set PMIC Alarm for %d seconds", psm_time);

            rc = psm_rtc_set_alarm(psm_time);
            if ( rc != 0 )
            {
                PSM_LOG_INFO("Failed to configure RTC alarm");
                return rc;
            }
#else
        PSM_CORE_ULOG_MSG("Modem setting RTC");
        PSM_LOG_INFO("psm_core_shutdown_device: Modem setting RTC");
#endif
        if( PSM_MODEM_NOT_LOADED == modem_load_status)
        {
            PSM_CORE_ULOG_MSG("Dal Init and shutdown. No modem.");
            rc = DAL_DeviceAttach(DALDEVICEID_CHIPPOWER, &psm_power_hndl);
            if ( DAL_SUCCESS != rc || NULL == psm_power_hndl )
            {
                PSM_CORE_ULOG_MSG("sys_m_shutdown Executed. PSMD exit");
                sys_m_shutdown();
            }
            else
            {
                DalChipPower_PowerOff(psm_power_hndl);
            }
        }
        else
        {
            PSM_CORE_ULOG_MSG("sys_m_shutdown Executed. PSMD exit");
            PSM_LOG_MSG_CRITICAL("sys_m_shutdown Executed. PSMD exit");
            sys_m_shutdown();
        }
    }

    PSM_CORE_ULOG_MSG("PSMD exit");
    PSM_LOG_FUNC_RETURN("psm_core_shutdown_device: function return");
    return rc;
}

/*========================================================================================
FUNCTION  psm_core_server_release

Function to release socket fd

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
========================================================================================*/
void psm_core_server_release(void)
{
    PSM_LOG_FUNC_ENTRY("psm_core_server_release: function entry");

    PSM_LOG_FUNC_RETURN("psm_core_server_release: function return");
}

/*========================================================================================
FUNCTION  psm_core_log_msg

Function for formatting log buffer.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
========================================================================================*/
void psm_core_log_msg
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
} /* psm_core_log_msg */




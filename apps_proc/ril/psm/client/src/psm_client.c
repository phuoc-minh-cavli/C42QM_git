/******************************************************************************
#@file    psm_client.c
#@brief   Provides interface for clients to access PSM functionality
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include "psm_client.h"
#include "psm_common.h"
#include "utils_common.h"
#include "qapi/qapi_psm.h"
#include "qurt_txm_qapi_fwk.h"
#include "txm_module.h"
#include "tx_thread.h"
#include "qurt_txm_uid.h"

#define PSM_DIR                     "/ril/psm"
#define PSM_LOG_FILE                "/ril/psm/psm_log.txt"
#define PSM_CLIENT_MAX_LOG_MSG_SIZE 512
#define PSM_CLIENT_MAX_MSGS         16
#define PSM_MAX_CLIENT_IDS          20

typedef enum {
    PSM_CLIENT_KERNELSPACE,    /**< Kernel space client.*/
    PSM_CLIENT_USERSPACE     /**< User space client. MMU compliance.*/
} psm_client_origin;

typedef struct
{
    int is_filled;
    ril_platform_timer_t   client_hc_timer_id;
    psm_client_cb_type     cb_func;
    psm_client_origin      origin;
    psm_status_msg_type*   msg;
    uint32_t module_id;
    uint32_t module_instance;
} psm_client_info_type;

psm_client_info_type psm_client_info[PSM_MAX_CLIENT_IDS];

int                       client_hc_timer_id = 0;
int                       client_shutdown = 0;
int                       pipe_fds[2] = {-1, -1};
int                       cbreadfd;
int                       cbwritefd;
int                       sockfd_client = -1;
psm_client_cb_type        client_cb;
util_sync_data_type       psm_client_sync_data;
ril_platform_thread_mutex_t log_lock_mutex;

extern ril_platform_pipe_t client_pipe;
extern ril_platform_pipe_t server_pipe;

int32_t psm_client_get_module_info(uint32_t *module_id, uint32_t *m_instance, int client_id) {
    int32_t status = QURT_EOK;

#ifdef FEATURE_PSM_USE_DAM_CONTAINMENT_CHECK
    TXM_MODULE_INSTANCE *module_inst = NULL;

    /* Validate the input parameters */
    if ((NULL == module_id) || (NULL == m_instance)) {
        return QURT_EINVALID;
    }

    /* Retrieve the module instance information */
    if ((status = qurt_txm_module_instance_get(&module_inst)) != QURT_EOK)
    {
        PSM_LOG_INFO("psm_client_get_module_info: Retrieving module instance information failed");
        return status;
    }

    /* Retrieve the module ID information */
    if ((status = qurt_txm_module_uid_get(module_inst, module_id)) != QURT_EOK)
    {
        PSM_LOG_INFO("psm_client_get_module_info: Retrieving module ID information failed");
        return status;
  }

  *m_instance = (uint32_t)module_inst;

#else
    //set module_id same as client_id for backward compatibility
    *module_id = (uint32_t)client_id;
    *m_instance = (uint32_t)client_id;
#endif

    return status;
}

boolean psm_client_validate_module_uid(int cur_uid) {
    int32_t status = FALSE;
    uint32_t module_id;
    uint32_t module_instance;

    //get the current caller module UID and compare it with stored UID
    if(psm_client_get_module_info(&module_id, &module_instance, cur_uid) == QURT_EOK) {
        if (cur_uid == module_id) {
            PSM_LOG_INFO("psm_client_validate_module_uid: Received request on correct module");
            status = TRUE;
        }
    }
    return status;
}

/*==============================================================================
FUNCTION psm_client_log_init

Initializes psm logging.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
void psm_client_log_init(void)
{
    ril_platform_thread_mutex_init(&log_lock_mutex, NULL);
}

/*========================================================================================
FUNCTION  psm_client_log_msg

Function for formatting log buffer.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
========================================================================================*/
void psm_client_log_msg
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
} /* psm_client_log_msg */


/*=========================================================================
  FUNCTION:  psm_client_send_msg

===========================================================================*/
/*!
    @brief
    Send message to psm server.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_send_msg(psm_req_msg_type *msg)
{
    psm_error_type_e result = PSM_ERR_NONE;

    PSM_LOG_FUNC_ENTRY("psm_client_send_msg: function entry");

    if ( SUCCESS != ril_platform_pipe_send(client_pipe, msg) )
    {
        result = PSM_ERR_GENERIC_FAILURE;
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
}

/*=========================================================================
  FUNCTION:  psm_client_is_active_time_compliant

===========================================================================*/
/*!
    @brief
    Checks if the entered Active time value is valid as per Spec.
    Refer to spec sections TS 24.008 Table 10.5.172 & Table 10.5.163a
    for valid Active time values.

    @return
    FAIL if invalid. SUCCESS if it is valid.
*/
/*=========================================================================*/
int psm_client_is_active_time_compliant(int active_time)
{
    if(((active_time <= 62) && (active_time % 2 == 0)) ||
       ((active_time >= 120 && active_time <= 1860) && (active_time % 60 == 0)) ||
       ((active_time >= 2160 && active_time <= 11160) && (active_time % 360 == 0)))
    {
        PSM_LOG_INFO1("psm_client_is_active_time_compliant: Valid Active time passed: %d", active_time);
        return SUCCESS;
    }
    else
    {
        PSM_LOG_INFO1("psm_client_is_active_time_compliant: Invalid Active time passed: %d", active_time);
        return FAIL;
    }
}

/*=========================================================================
  FUNCTION:  psm_client_cancel_health_check_timer

===========================================================================*/
/*!
    @brief
    Cancel health check timer.

    @return
    None.
*/
/*=========================================================================*/
void psm_client_cancel_health_check_timer(int i)
{
    if ( psm_client_info[i].client_hc_timer_id.timer_state != TIMER_UNDEF_FLAG )
    {
        ril_platform_timer_stop(&psm_client_info[i].client_hc_timer_id);
        ril_platform_timer_delete(&psm_client_info[i].client_hc_timer_id);
    }
}

#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
/**
 * migrate to new time_services api
**/
void psm_client_migrate_time_information(time_julian_type *out,pm_rtc_julian_type *in) {
    if (out && in)
    {
        PSM_LOG_INFO("psm_client_migrate_time_information: migrate_time_info ");
        out->year = in->year;
        out->month = in->month;
        out->day = in->day;
        out->hour = in->hour;
        out->minute = in->minute;
        out->second = in->second;
        out->day_of_week = in->day_of_week;
    }
}
#endif



/*=========================================================================
  FUNCTION:  psm_client_handle_recv_msg

===========================================================================*/
/*!
    @brief
    Handle status from psm server.

    @return
    None.
*/
/*=========================================================================*/
void psm_client_handle_recv_msg(psm_status_msg_type *resp)
{
    int                  client_id;
    psm_status_msg_type  cb_data;
    int rc = -1;

    memset(&cb_data, 0, sizeof(cb_data));
    client_id      = resp->client_id;
    cb_data.client_id = resp->client_id;
    cb_data.status = resp->status;
    cb_data.reason = resp->reason;

    /* If PSM feature not enabled, stop health check timer */
    if ( (resp->status == PSM_STATUS_REJECT) &&
         (resp->reason == PSM_REJECT_REASON_NOT_ENABLED) )
    {
        psm_client_cancel_health_check_timer(client_id);
    }

    if ( (client_id < PSM_MAX_CLIENT_IDS) &&
            (psm_client_info[client_id].is_filled == TRUE) &&
            (psm_client_info[client_id].cb_func ))
    {
        switch (psm_client_info[client_id].origin)
        {
            case PSM_CLIENT_KERNELSPACE:
                if(psm_client_info[client_id].msg != NULL)
                {
                    /* Kernel space app with passed msg*/
                    memscpy(
                        psm_client_info[client_id].msg, sizeof(psm_status_msg_type),
                        &cb_data, sizeof(psm_status_msg_type));
                    (*psm_client_info[client_id].cb_func)(psm_client_info[client_id].msg);
                }
                else
                {
                    /* Kernel space app without passed msg*/
                    (*psm_client_info[client_id].cb_func)(&cb_data);
                }
                break;
            case PSM_CLIENT_USERSPACE:
                /*Safe copy kernel memory contents to Userspace memory*/
                //copy data to user-space
#ifdef FEATURE_PSM_USE_DAM_CONTAINMENT_CHECK
                rc = qurt_copy_to_user_ext(
                    (TXM_MODULE_INSTANCE *) psm_client_info[client_id].module_instance,
                    (psm_status_msg_type *) psm_client_info[client_id].msg,
                    (qurt_size_t) sizeof(psm_status_msg_type),
                    (psm_status_msg_type *) &cb_data,
                    (qurt_size_t) sizeof(psm_status_msg_type));
                PSM_LOG_INFO1("psm_client_handle_recv_msg: copy response %d ",rc);
#else
                memscpy(
                   (void *) psm_client_info[client_id].msg,
                    sizeof(psm_status_msg_type),
                   (void *) &cb_data,
                   sizeof(psm_status_msg_type));
#endif
                (*psm_client_info[client_id].cb_func)(psm_client_info[client_id].msg);
                break;
        }
        PSM_LOG_INFO1("psm_client_handle_recv_msg: Client CB - %x", psm_client_info[client_id].cb_func);
    }

}

/*=========================================================================
  FUNCTION:  psm_client_callback_entry

===========================================================================*/
/*!
    @brief
    Entry function for call back thread. Callback thread is the one which
    informs to clients when ever there are events from psm server.

    @return
    None.
*/
/*=========================================================================*/
int psm_client_callback_entry
(
    dword ignored
)
{
    psm_status_msg_type      recv_msg;

    PSM_LOG_FUNC_ENTRY("psm_client_callback_entry: function entry");
    qurt_qapi_register(TXM_QAPI_PSM_BASE, TXM_QAPI_PSM_NUM_IDS, qapi_PSM_Handler);
    rcinit_handshake_startup();

    do
    {
       while(1)
       {
           ril_platform_pipe_receive(server_pipe, &recv_msg);
           psm_client_handle_recv_msg(&recv_msg);
       }
    } while(FALSE);

}

/*=========================================================================
  FUNCTION:  psm_healthcheck_timer_cb

===========================================================================*/
/*!
    @brief
    Health check call back.

    @return
    None.
*/
/*=========================================================================*/
void psm_healthcheck_timer_cb(void *cb_data)
{
    psm_status_msg_type psm_data;

    PSM_LOG_INFO("psm_healthcheck_timer_cb: Health Check timer expired");

    if ( server_pipe != 0 )
    {
        psm_data.client_id = (int)cb_data;
        psm_data.status = PSM_STATUS_HEALTH_CHECK;
        psm_data.reason = PSM_REJECT_REASON_NONE;
        ril_platform_pipe_send(server_pipe, &psm_data);
        ril_platform_timer_restart(&psm_client_info[psm_data.client_id].client_hc_timer_id,
                                   PSM_CLIENT_HC_TIMEOUT);
    }
}


/*=========================================================================
  FUNCTION:  psm_client_register

===========================================================================*/
/*!
    @brief
    Makes the app known to psmd as a PSM-aware app.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_register(int *client_id, psm_client_cb_type cb_func)
{
    int                         i=0;
    psm_error_type_e            result = PSM_ERR_NONE;
    psm_req_msg_type            req;

    PSM_LOG_FUNC_ENTRY("psm_client_register: function entry");

    do
    {
        if ( client_id == NULL || cb_func == NULL )
        {
            result = PSM_ERR_WRONG_ARGUMENTS;
            break;
        }

        psm_client_log_init();
        for(i=0;i<PSM_MAX_CLIENT_IDS; i++)
        {
            if (psm_client_info[i].is_filled == FALSE )
            {
                break;
            }
        }

        if ( i < PSM_MAX_CLIENT_IDS )
        {
            psm_client_info[i].is_filled = TRUE;
            psm_client_info[i].cb_func = cb_func;
            psm_client_info[i].origin = PSM_CLIENT_KERNELSPACE;
            psm_client_info[i].msg = NULL; /*Initialize with NULL*/
            *client_id = i;

            memset(&req.req_info, 0, sizeof(req.req_info));
            req.client_id = i;
            req.request = PSM_REQUEST_PSM_REGISTER;
            psm_client_send_msg(&req);

            ril_platform_timer_start_oneshot(&psm_client_info[i].client_hc_timer_id,
                                             psm_healthcheck_timer_cb,
                                             (void*)i,
                                             PSM_CLIENT_HC_TIMEOUT);

            PSM_LOG_INFO1("psm_client_register: PSM Registered successfully. client_id - %d", *client_id);
        }
        else
        {
            result = PSM_ERR_GENERIC_FAILURE;
        }
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
}

/*=========================================================================
  FUNCTION:  psm_client_register_with_msg

===========================================================================*/
/*!
    @brief
    Register with the PSM Daemon along with a supplied memory to use with
    Callbacks.

    Userspace applications have to mandatorily use this call.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_register_with_msg
(
    int                 *client_id,
    psm_client_cb_type   cb_func,
    psm_status_msg_type *msg
)
{
    int                         i=0;
    psm_error_type_e            result = PSM_ERR_NONE;
    psm_req_msg_type            req;
    int rc = -1;
    PSM_LOG_FUNC_ENTRY("psm_client_register_with_msg: function entry");

    do
    {
        if ( client_id == NULL || cb_func == NULL )
        {
            result = PSM_ERR_WRONG_ARGUMENTS;
            break;
        }

        /* 
         If register request is from a Userspace application, then
         the Callback msg data cannot be NULL. This is enforced because of
         Kernel/Userspace MMU seperation. Kernel space address cannot be passed
         on to the Userspace application callback
        */
        if(FALSE == qurt_check_if_kernel_address((qurt_addr_t *) client_id) && 
                                                                        NULL == msg)
        {
            result = PSM_ERR_WRONG_ARGUMENTS;
            break;
        }


        for(i=0;i<PSM_MAX_CLIENT_IDS; i++)
        {
            if (psm_client_info[i].is_filled == FALSE )
            {
                break;
            }
        }

        if ( i < PSM_MAX_CLIENT_IDS )
        {

            psm_client_info[i].is_filled = TRUE;
            psm_client_info[i].cb_func = cb_func;
            psm_client_info[i].msg = NULL; /*Initialize with NULL*/

            if(TRUE == qurt_check_if_kernel_address((qurt_addr_t *) client_id))
            {
                /* Register call from kernel space application. msg is optional */
               *client_id = i;
                psm_client_info[i].origin = PSM_CLIENT_KERNELSPACE;

                if(NULL != msg)
                {
                    memset(msg, 0, sizeof(psm_status_msg_type));
                    psm_client_info[i].msg = msg;
                }
            }
            else
            {
                /* Register call from User space application. msg is Mandatory */
                rc = qurt_copy_to_user(
                    (qurt_addr_t *) client_id, (qurt_size_t)   sizeof(int),
                    (qurt_addr_t *) &i,        (qurt_size_t)   sizeof(int));
                PSM_LOG_INFO1("psm_client_register_with_msg: Copy return: %d", rc);
                psm_client_info[i].origin = PSM_CLIENT_USERSPACE;
                psm_client_info[i].msg = msg;

                //store the module_uid
                if (psm_client_get_module_info(&(psm_client_info[i].module_id),
                    &(psm_client_info[i].module_instance), i) == QURT_EOK) {
                       PSM_LOG_INFO("psm_client_register_with_msg: Received module info successfully");
                } else {
                    PSM_LOG_INFO("psm_client_register_with_msg: Not able to query Module UID Info ");
                    result = PSM_ERR_GENERIC_FAILURE;
                    break;
                }
            }
            memset(&req.req_info, 0, sizeof(req.req_info));
            req.client_id = i;
            req.request = PSM_REQUEST_PSM_REGISTER;
            psm_client_send_msg(&req);

            ril_platform_timer_start_oneshot(&psm_client_info[i].client_hc_timer_id,
                                             psm_healthcheck_timer_cb,
                                             (void*)i,
                                             PSM_CLIENT_HC_TIMEOUT);

            PSM_LOG_INFO1("psm_client_register_with_msg: PSM Registered successfully. client_id - %d", *client_id);
            PSM_LOG_MSG_CRITICAL("psm_client_register_with_msg: PSM Registered successfully. client_id - %d", *client_id);
        }
        else
        {
            result = PSM_ERR_GENERIC_FAILURE;
        }
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
}

/*=========================================================================
  FUNCTION:  psm_client_unregister

===========================================================================*/
/*!
    @brief
    Unregister a psm aware app.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_unregister(int client_id)
{
    psm_error_type_e result = PSM_ERR_NONE;
    psm_req_msg_type req;

    PSM_LOG_FUNC_ENTRY("psm_client_unregister: function entry");

    do
    {
        memset(&req, 0, sizeof(req));
        if ( client_id < 0 || psm_client_info[client_id].is_filled == FALSE )
        {
            PSM_LOG_INFO1("psm_client_unregister: invalid client_id - %d", client_id);
            result = PSM_ERR_APP_NOT_REGISTERED;
            break;
        }

        // Cancel health check timer
        psm_client_cancel_health_check_timer(client_id);
        psm_client_info[client_id].is_filled = FALSE;
        psm_client_info[client_id].cb_func   = NULL;

        req.request = PSM_REQUEST_PSM_UNREGISTER;
        req.client_id = client_id;
        psm_client_send_msg(&req);

        PSM_LOG_INFO("psm_client_unregister: PSM Unregistered successfully..");
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
}

/*=========================================================================
  FUNCTION:  psm_client_enter_power_save

===========================================================================*/
/*!
    @brief
    Used by app to notify its desire to enter PSM mode.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_enter_power_save
(
    int                 client_id,
    psm_info_type      *psm_info
)
{
    psm_req_msg_type       req_msg;
    psm_error_type_e       result = PSM_ERR_NONE;
#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
    time_julian_type       current_time_julian;
#else
    pm_rtc_julian_type     current_time_julian;
#endif
    uint32                 curr_time_in_secs;
    uint32                 alarm_time_in_secs;

    PSM_LOG_FUNC_ENTRY("psm_client_enter_power_save: function entry");

    do
    {
        if ( client_id < 0 || psm_client_info[client_id].is_filled == FALSE )
        {
            result = PSM_ERR_APP_NOT_REGISTERED;
            break;
        }

        if( (psm_info == NULL) ||
            (psm_info->active_time_in_secs < 0) )
        {
            PSM_LOG_INFO("psm_client_enter_power_save: wrong arguments");
            result = PSM_ERR_WRONG_ARGUMENTS;
            break;
        }

        if( SUCCESS != psm_client_is_active_time_compliant(psm_info->active_time_in_secs) )
        {
            PSM_LOG_ERROR("psm_client_enter_power_save: Rejecting enter request. Unsupported Active time passed");
            result = PSM_ERR_INVALID_ACTIVE_TIME;
            break;
        }

        memset(&req_msg, 0, sizeof(req_msg));
        req_msg.client_id              = client_id;
        req_msg.request                = PSM_REQUEST_PSM_READY;
        req_msg.req_info.nw_access     = psm_info->psm_wakeup_type;
        req_msg.req_info.active_timer  = psm_info->active_time_in_secs;

        if ( psm_info->psm_time_info.time_format_flag == PSM_TIME_IN_TM )
        {
#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
            time_julian_type wakeup_time_julian = {0};
            psm_client_migrate_time_information(&wakeup_time_julian, &psm_info->psm_time_info.wakeup_time);
            time_get_julian(&current_time_julian);
            curr_time_in_secs = time_jul_to_secs(&current_time_julian);
            alarm_time_in_secs = time_jul_to_secs(&wakeup_time_julian);
#else
            pm_rtc_rw_cmd(PM_RTC_GET_CMD, &current_time_julian);
            curr_time_in_secs  = pm_rtc_app_rtc_julian_to_secs(&current_time_julian);
            alarm_time_in_secs = pm_rtc_app_rtc_julian_to_secs(&psm_info->psm_time_info.wakeup_time);
#endif

            req_msg.req_info.start_timer = alarm_time_in_secs - curr_time_in_secs;
            req_msg.req_info.end_timer = alarm_time_in_secs - curr_time_in_secs;
        }
        else
        {
            req_msg.req_info.start_timer = psm_info->psm_time_info.psm_duration_in_secs;
            req_msg.req_info.end_timer = psm_info->psm_time_info.psm_duration_in_secs;
        }

        PSM_LOG_INFO3("psm_client_enter_power_save: active time - %d psm time - %d wake up type - %d",
                                                    req_msg.req_info.active_timer,
                                                    req_msg.req_info.start_timer,
                                                    req_msg.req_info.nw_access );

        result = psm_client_send_msg(&req_msg);

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
}

/*=========================================================================
  FUNCTION:  psm_client_enter_backoff

===========================================================================*/
/*!
    @brief
    Used by app to notify its desire to enter PSM mode when OOS, Limited service
    or PSM server not reachable.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_enter_backoff
(
    int client_id
)
{
    psm_req_msg_type       req_msg;
    psm_error_type_e       result = PSM_ERR_NONE;

    PSM_LOG_FUNC_ENTRY("psm_client_enter_backoff: function entry");

    do
    {
        if ( client_id < 0 || psm_client_info[client_id].is_filled == FALSE )
        {
            PSM_LOG_INFO("psm_client_enter_backoff: application not registered");
            result = PSM_ERR_WRONG_ARGUMENTS;
            break;
        }

        memset(&req_msg, 0, sizeof(req_msg));
        req_msg.client_id = client_id;
        req_msg.request   = PSM_REQUEST_BACKOFF;
        result = psm_client_send_msg(&req_msg);

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
}


/*=========================================================================
  FUNCTION:  psm_cancel_power_save

===========================================================================*/
/*!
    @brief
    Used by app to notify its desire to withdraw a previous request to enter PSM mode.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_cancel_power_save(int client_id)
{
    psm_req_msg_type       req_msg;
    psm_error_type_e       result = PSM_ERR_NONE;

    PSM_LOG_FUNC_ENTRY("psm_cancel_power_save: function entry");

    do
    {
        if ( client_id < 0 || psm_client_info[client_id].is_filled == FALSE )
        {
            PSM_LOG_INFO("psm_cancel_power_save: application not registered");
            result = PSM_ERR_APP_NOT_REGISTERED;
            break;
        }

        memset(&req_msg, 0, sizeof(req_msg));
        req_msg.client_id = client_id;
        req_msg.request   = PSM_REQUEST_PSM_NOT_READY;
        result = psm_client_send_msg(&req_msg);

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
}

/*=========================================================================
  FUNCTION:  psm_client_load_modem

===========================================================================*/
/*!
    @brief
    Application should inform its intention to use modem services through this API.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_load_modem(int client_id)
{
    psm_req_msg_type       req_msg;
    psm_error_type_e       result = PSM_ERR_NONE;

    PSM_LOG_FUNC_ENTRY("psm_client_load_modem: function entry");

    do
    {
        if ( client_id < 0 || psm_client_info[client_id].is_filled == FALSE )
        {
            PSM_LOG_INFO("psm_client_load_modem: application not registered");
            result = PSM_ERR_APP_NOT_REGISTERED;
            break;
        }

        memset(&req_msg, 0, sizeof(req_msg));
        req_msg.client_id = client_id;
        req_msg.request   = PSM_REQUEST_LOAD_MODEM;
        result = psm_client_send_msg(&req_msg);

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
}

/*=========================================================================
  FUNCTION:  psm_client_health_check_ack

===========================================================================*/
/*!
    @brief
    Application health check API.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_health_check_ack(int client_id)
{
    psm_req_msg_type       req_msg;
    psm_error_type_e       result = PSM_ERR_NONE;

    PSM_LOG_FUNC_ENTRY("psm_client_health_check_ack: function entry");

    do
    {
        if ( client_id < 0 || psm_client_info[client_id].is_filled == FALSE )
        {
            PSM_LOG_INFO("psm_client_health_check_ack: application not registered");
            result = PSM_ERR_APP_NOT_REGISTERED;
            break;
        }

        memset(&req_msg, 0, sizeof(req_msg));
        req_msg.client_id = client_id;
        req_msg.request   = PSM_REQUEST_HEALTH_CHECK;
        result = psm_client_send_msg(&req_msg);

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
}

/*=========================================================================
  FUNCTION:  psm_client_compare_intervals

===========================================================================*/
/*!
    @brief
    Utility Function to compare intervals used by psm_client_optimal_interval .

    @return
    None.
*/
/*=========================================================================*/
int psm_client_compare_intervals(const void * a, const void * b) 
{ 
    psm_time_interval_type *ia = (psm_time_interval_type *)a;
    psm_time_interval_type *ib = (psm_time_interval_type *)b;
    if(ia->start.psm_duration_in_secs > ib->start.psm_duration_in_secs)
    {
        return 1; 
    }
    else if (ia->start.psm_duration_in_secs == ib->start.psm_duration_in_secs)
    {
        return 0;
    }
    else
    {
        return -1;
    }
} 


/*=========================================================================
  FUNCTION:  psm_client_sort_intervals

===========================================================================*/
/*!
    @brief
    Utility Function to sort intervals used by psm_client_optimal_interval .

    @return
    None.
*/
/*=========================================================================*/
void psm_client_sort_intervals(psm_time_interval_group_type *interval_group)
{   
    int rc = 0;
    int n  = 0;
    PSM_LOG_FUNC_ENTRY("psm_client_sort_intervals: function entry");
    
    do
    {
        if(!interval_group)
        {
            PSM_LOG_INFO("psm_client_sort_intervals: Invalid group intervals passed");
            rc=-1;
            break;
        }

        n = interval_group->number_of_intervals;
        qsort
        (
          interval_group->intervals, n, 
          sizeof(psm_time_interval_type),
          psm_client_compare_intervals
        );    
    }while (FALSE);
    
    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
}

/*=========================================================================
  FUNCTION:  psm_client_convert_julian_sec

===========================================================================*/
/*!
    @brief
    Utility Function to convert the time format to seconds before 
    sorting intervals used by psm_client_optimal_interval.

    @return
    None.
*/
/*=========================================================================*/
void psm_client_convert_julian_sec(psm_time_interval_group_type *interval_group)
{
    int                    i;
    int                    rc = 0;
#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
    time_julian_type       current_time_julian;
#else
    pm_rtc_julian_type     current_time_julian;
#endif
    uint32                 curr_time_in_secs;
    uint32                 alarm_time_in_secs;

    PSM_LOG_FUNC_ENTRY("psm_client_sort_intervals: function entry");
    do
    {
        if(!interval_group)
        {
            PSM_LOG_INFO("psm_client_sort_intervals: Invalid group intervals passed");
            rc=-1;
            break;
        }
        
        for (i = 0; i < interval_group->number_of_intervals; i++)       
        {
            if ((interval_group->intervals[i].start.time_format_flag == PSM_TIME_IN_TM) || 
                (interval_group->intervals[i].end.time_format_flag == PSM_TIME_IN_TM))
            {
#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
                time_get_julian(&current_time_julian);
                curr_time_in_secs = time_jul_to_secs(&current_time_julian);
#else
                 pm_rtc_rw_cmd(PM_RTC_GET_CMD,&current_time_julian);
                curr_time_in_secs  = pm_rtc_app_rtc_julian_to_secs(&current_time_julian);
#endif
                if(interval_group->intervals[i].start.time_format_flag == PSM_TIME_IN_TM)
                {
#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
                    time_julian_type wakeup_time_julian = {0};
                    psm_client_migrate_time_information(&wakeup_time_julian, &interval_group->intervals[i].start.wakeup_time);
                    alarm_time_in_secs = time_jul_to_secs(&wakeup_time_julian);
#else
                    alarm_time_in_secs =
                        pm_rtc_app_rtc_julian_to_secs(&interval_group->intervals[i].start.wakeup_time);
#endif
                    alarm_time_in_secs = alarm_time_in_secs - curr_time_in_secs;
                    interval_group->intervals[i].start.psm_duration_in_secs = alarm_time_in_secs;
                    interval_group->intervals[i].start.time_format_flag = PSM_TIME_IN_SECS;
                } 

                if(interval_group->intervals[i].end.time_format_flag == PSM_TIME_IN_TM)
                {
#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
                    time_julian_type wakeup_time_julian = {0};
                    psm_client_migrate_time_information(&wakeup_time_julian, &interval_group->intervals[i].end.wakeup_time);
                    alarm_time_in_secs = time_jul_to_secs(&wakeup_time_julian);
#else
                    alarm_time_in_secs =
                        pm_rtc_app_rtc_julian_to_secs(&interval_group->intervals[i].end.wakeup_time);
#endif
                    alarm_time_in_secs = alarm_time_in_secs - curr_time_in_secs;
                    interval_group->intervals[i].end.psm_duration_in_secs = alarm_time_in_secs;
                    interval_group->intervals[i].end.time_format_flag = PSM_TIME_IN_SECS;
                }
            }
        }
    }while (FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
}
/*=========================================================================
  FUNCTION:  psm_client_optimal_interval

===========================================================================*/
/*!
    @brief
    Used by app to find optimal PSM interval from a group of time intervas.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_optimal_interval
(
    int client_id,
    psm_time_interval_group_type *interval_group,
    psm_time_interval_type *opt_interval,
    boolean request_from_app
)
{
    psm_error_type_e       result = PSM_ERR_NONE;
    int                    audit = TRUE;
    int                    i = 0;
    psm_time_interval_type *optimal_interval = NULL;
    PSM_LOG_FUNC_ENTRY("psm_client_optimal_interval: function entry");

    do
    {
        if(!opt_interval || (!interval_group))
        {
            result = PSM_ERR_INVALID_INTERVAL;
            break;
        }

        if(!interval_group && interval_group->number_of_intervals <=0)
        {
            result = PSM_ERR_INVALID_INTERVAL;
            break;
        }
        
        for(i=0; i<interval_group->number_of_intervals;i++)
        {
            if(interval_group->intervals[i].end.psm_duration_in_secs <
                        interval_group->intervals[i].start.psm_duration_in_secs)
            {
                PSM_LOG_INFO3("psm_client_optimal_interval: Interval number %d passed invalid interval. Start: %d, End: %d",i,
                        interval_group->intervals[i].start.psm_duration_in_secs,
                        interval_group->intervals[i].end.psm_duration_in_secs);
                audit = FALSE;
                break;
            }
        }
        if(!audit)
        {
            result = PSM_ERR_INVALID_INTERVAL;
            break;
        }

        // convert intervals time to sec format if in julian format
        psm_client_convert_julian_sec(interval_group);
        //sort intervals based on start time
        psm_client_sort_intervals(interval_group);

        //Print Sorted Intervals
        for (i = 0; i < interval_group->number_of_intervals; i++)       
        {
             PSM_LOG_INFO2("psm_client_optimal_interval: Interval number %d Start: %d",
             i,interval_group->intervals[i].start.psm_duration_in_secs);
        }

        
        optimal_interval = (psm_time_interval_type*)malloc(sizeof(psm_time_interval_type));
        if ( optimal_interval == NULL )
        {
            PSM_LOG_INFO("psm_client_optimal_interval: Failed to allocate memory for optimal_interval struct");
            break;
        }
        memset(optimal_interval,0,sizeof(optimal_interval));
        
        //Assume first interval as optimal interval     
        optimal_interval->start.psm_duration_in_secs = 
        interval_group->intervals[0].start.psm_duration_in_secs;
        optimal_interval->start.time_format_flag    = 
        interval_group->intervals[0].start.time_format_flag;
        optimal_interval->end.psm_duration_in_secs  = 
        interval_group->intervals[0].end.psm_duration_in_secs;
        optimal_interval->end.time_format_flag      = 
        interval_group->intervals[0].end.time_format_flag;
        optimal_interval->psm_wakeup_type           = 
        interval_group->intervals[0].psm_wakeup_type;
        
        for (i = 1; i < interval_group->number_of_intervals; i++)       
        {
            if( optimal_interval->end.psm_duration_in_secs > 
                interval_group->intervals[i].start.psm_duration_in_secs)
            {
                optimal_interval->start.psm_duration_in_secs = interval_group->intervals[i].start.psm_duration_in_secs;
                optimal_interval->end.psm_duration_in_secs = 
                (optimal_interval->end.psm_duration_in_secs < interval_group->intervals[i].end.psm_duration_in_secs) ? 
                 optimal_interval->end.psm_duration_in_secs : interval_group->intervals[i].end.psm_duration_in_secs;

                //Update Wakeup type
                if(interval_group->intervals[i].psm_wakeup_type == 
                    PSM_WAKEUP_MEASUREMENT_NW_ACCESS)
                {
                    optimal_interval->psm_wakeup_type = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
                }
            }
            else
            {
                break;
            }
        }

        memscpy(opt_interval, sizeof(psm_time_interval_type), optimal_interval, sizeof(psm_time_interval_type));


        PSM_LOG_INFO3("psm_client_optimal_interval: Optimal interval: PSM: (%u,%u) and wakeup type: %d",
                                    opt_interval->start.psm_duration_in_secs,
                                    opt_interval->end.psm_duration_in_secs,
                                    opt_interval->psm_wakeup_type); 
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(result);
    return result;
}

/*=========================================================================
  FUNCTION:  psm_client_enter_psm_with_range

===========================================================================*/
/*!
    @brief
   Used by app to notify its desire to enter PSM mode with interval.

    @return
    None.
*/
/*=========================================================================*/
psm_error_type_e psm_client_enter_psm_with_range
(
    int                 client_id,
    psm_info_interval_type      *psm_info
)
{
    psm_req_msg_type       req_msg;
    psm_error_type_e       result = PSM_ERR_NONE;
#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
    time_julian_type       current_time_julian;
#else
    pm_rtc_julian_type     current_time_julian;
#endif

    uint32                 curr_time_in_secs;
    uint32                 alarm_time_in_secs;

    PSM_LOG_FUNC_ENTRY("psm_client_enter_psm_with_range: function entry");

    do
    {
        if ( client_id < 0 || psm_client_info[client_id].is_filled == FALSE )
        {
            result = PSM_ERR_APP_NOT_REGISTERED;
            break;
        }

        if( (psm_info == NULL) || (psm_info->active_time_in_secs < 0) )
        {
            PSM_LOG_INFO("psm_client_enter_psm_with_range: wrong arguments");
            result = PSM_ERR_WRONG_ARGUMENTS;
            break;
        }
        PSM_LOG_INFO3("psm_client_enter_psm_with_range: Start time - %d End time - %d  Active Time - %d",
                                                    psm_info->psm_time_info.start.psm_duration_in_secs,
                                                    psm_info->psm_time_info.end.psm_duration_in_secs,
                                                    psm_info->active_time_in_secs);
       
        if( SUCCESS != psm_client_is_active_time_compliant(psm_info->active_time_in_secs) )
        {
            PSM_LOG_ERROR("psm_client_enter_psm_with_range: Rejecting enter request. Unsupported Active time passed");
            result = PSM_ERR_INVALID_ACTIVE_TIME;
            break;
        }

        if(psm_info->psm_time_info.end.psm_duration_in_secs < 
            psm_info->psm_time_info.start.psm_duration_in_secs)
        {
            result = PSM_ERR_INVALID_INTERVAL;
            break;
        }
        
        memset(&req_msg, 0, sizeof(req_msg));
        req_msg.client_id              = client_id;
        req_msg.request                = PSM_REQUEST_PSM_READY;
        req_msg.req_info.nw_access     = psm_info->psm_time_info.psm_wakeup_type;
        req_msg.req_info.active_timer  = psm_info->active_time_in_secs;
                   
        PSM_LOG_INFO2("psm_client_enter_psm_with_range: Start time format  %u end time format %u ",
                psm_info->psm_time_info.start.time_format_flag,
                psm_info->psm_time_info.end.time_format_flag);
                                               
        if ((psm_info->psm_time_info.start.time_format_flag == PSM_TIME_IN_TM) || 
            (psm_info->psm_time_info.end.time_format_flag== PSM_TIME_IN_TM))
        {
            PSM_LOG_INFO("psm_client_enter_psm_with_range: Start/END time format in TM");
#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
            time_get_julian(&current_time_julian);
            curr_time_in_secs = time_jul_to_secs(&current_time_julian);
#else
            pm_rtc_rw_cmd(PM_RTC_GET_CMD,&current_time_julian);
            curr_time_in_secs  = pm_rtc_app_rtc_julian_to_secs(&current_time_julian);
#endif
        }
        
        if(psm_info->psm_time_info.start.time_format_flag == PSM_TIME_IN_TM)
        {
#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
            time_julian_type wakeup_time_julian = {0};
            PSM_LOG_INFO("psm_client_enter_psm_with_range: Start time format in TM");
            psm_client_migrate_time_information(&wakeup_time_julian, &psm_info->psm_time_info.start.wakeup_time);
            alarm_time_in_secs = time_jul_to_secs(&wakeup_time_julian);
#else
            PSM_LOG_INFO("psm_client_enter_psm_with_range: Start time format in TM");
            alarm_time_in_secs =
                pm_rtc_app_rtc_julian_to_secs(&psm_info->psm_time_info.start.wakeup_time);
#endif
            alarm_time_in_secs = alarm_time_in_secs - curr_time_in_secs;
            psm_info->psm_time_info.start.psm_duration_in_secs = alarm_time_in_secs;
        }

        if(psm_info->psm_time_info.end.time_format_flag == PSM_TIME_IN_TM)
        {
#ifdef FEATURE_PSM_USE_NEW_PMIC_RTC_API
            time_julian_type wakeup_time_julian = {0};
            PSM_LOG_INFO("psm_client_enter_psm_with_range: End time format in TM");
            psm_client_migrate_time_information(&wakeup_time_julian, &psm_info->psm_time_info.end.wakeup_time);
            alarm_time_in_secs = time_jul_to_secs(&wakeup_time_julian);
#else
            PSM_LOG_INFO("psm_client_enter_psm_with_range: End time format in TM");
            alarm_time_in_secs =
                pm_rtc_app_rtc_julian_to_secs(&psm_info->psm_time_info.end.wakeup_time);
#endif

            alarm_time_in_secs = alarm_time_in_secs - curr_time_in_secs;
            psm_info->psm_time_info.end.psm_duration_in_secs = alarm_time_in_secs;
        } 

        req_msg.req_info.start_timer   = psm_info->psm_time_info.start.psm_duration_in_secs;
        req_msg.req_info.end_timer     = psm_info->psm_time_info.end.psm_duration_in_secs;
        
        PSM_LOG_INFO4("Req msg active time - %u start psm time - %u  end psm time %u wake up type - %u",
                                                    req_msg.req_info.active_timer,
                                                    req_msg.req_info.start_timer,
                                                    req_msg.req_info.end_timer,
                                                    req_msg.req_info.nw_access );

        result = psm_client_send_msg(&req_msg);

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(result);
    return result;

}

psm_error_type_e psm_client_validate_module(int client_id) {
  psm_error_type_e result = PSM_ERR_NONE;

  PSM_LOG_FUNC_ENTRY("psm_client_validate_module: function entry");

  do {
    if ( client_id < 0 || psm_client_info[client_id].is_filled == FALSE )
    {
      PSM_LOG_INFO1("psm_client_validate_module: invalid client_id - %d", client_id);
      result = PSM_ERR_APP_NOT_REGISTERED;
      break;
    }
    //check whether request is received from correct module
    if (!psm_client_validate_module_uid(psm_client_info[client_id].module_id)) {
      PSM_LOG_INFO("psm_client_validate_module: Request from unauthorized module");
      result = PSM_ERR_FAIL;
      break;
    }
  } while (FALSE);

  return result;

}

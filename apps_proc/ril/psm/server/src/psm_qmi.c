/******************************************************************************
#@file    psm_qmi.c
#@brief   Provides API to exercise modem functionality.
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
#include <stringl/stringl.h>

#include <ril_port.h>

#include "qmi_client.h"
#include "common_v01.h"
#include "device_management_service_v01.h"

#include "psm_qmi.h"
#include "psm_rtc.h"
#include "psm_core.h"
#include "psm_common.h"
#include "psm_list.h"
#include "utils_common.h"
#include "pm_pon.h"
#include "subsys.h"

#define PSM_QMI_TIMEOUT              (1000)
#define PSM_QMI_MIN_THRESHOLD        (50)
#define PSM_BOOT_REGISTER_VALUE      (200)
#define PSM_HIBERNATE_REGISTER_VALUE (100)
#define PSM_CNF_LEN                  (512)
#define PSM_MODEM_NODE         "/dev/subsys_modem"
#define PSM_NV_BACKUP_FILE     "/ril/psm/psm_nv_backup.txt"
#define SSCTL_INSTANCE_ID      (SSCTL_QMI_SERVICE_INSTANCE_MPSS_V02 + QMI_SSCTL_SERVICE_INSTANCE_OFFSET_V02)

#define WONCE_REG_ADDRESS  (0x0193D010)
#define WONCE_BOOT_ADDRESS (0x0193D03C)

#define PSM_ENABLED_KEY              "PSM_ENABLED"
#define PSM_THRESHOLD_KEY            "PSM_THRESHOLD"
#define PSM_OOS_DURATION_KEY         "PSM_OOS_DURATION"
#define PSM_RANDMIZATION_WINDOW_KEY  "PSM_RANDOMIZATION_WINDOW"
#define PSM_PERIODIC_TAU_TIMER       "PSM_PERIODIC_TAU_TIMER"
#define PSM_PERIODIC_TAU_RCVD_TIME   "PSM_PERIODIC_TAU_RCVD_TIME"
#define PSM_APP_ACTIVE_TIME          "PSM_APP_ACTIVE_TIME"
#define PSM_BOOTUP_DELAY             "PSM_BOOTUP_DELAY"
#define PSM_MODEM_CAP_VERSION        "PSM_MODEM_CAP_VERSION"

typedef struct
{
  qmi_idl_service_object_type service_objects[PSM_QMI_CLIENT_MAX];
  qmi_client_type             qmi_client_handles[PSM_QMI_CLIENT_MAX];
  qmi_client_os_params        os_params[PSM_QMI_CLIENT_MAX];
} psm_qmi_client_info_type;

typedef struct
{
    uint8_t     psm_supported_valid;
    uint32_t    psm_supported;
    uint8_t     psm_threshold_value_valid;
    uint32_t    psm_threshold_value;
    uint8_t     duration_due_to_oos_valid;
    uint32_t    duration_due_to_oos;
    uint8_t     randomization_window_valid;
    uint32_t    randomization_window;
    uint8_t     periodic_tau_timer_valid;
    uint32_t    periodic_tau_timer;
    uint8_t     periodic_tau_rcvd_time_valid;
    uint32_t    periodic_tau_rcvd_time;
    uint32_t    psm_app_active_time_valid;
    uint32_t    psm_app_active_time;
    uint8_t     early_wakeup_time_valid;
    uint32_t    early_wakeup_time;
    uint8_t     modem_capability_version_valid;
    dms_psm_version_capability_v01 modem_capability_version;
}psm_qmi_cache_type;

/* Declaration for IMEM Cookie structure and enums
   Refer to boot_shared_imem_cookie.h file from BOOT image
   for further details about its contents.

   Usage brief:-
   1. Use IMEM START ADDRESS(Physical address) to dereference 
   this struct from memory (Target specific)
   2. Check if the shared_imem_magic is correct (Target specific)
   3. Verify fota_info_cookie to know if it is FOTA or a Normal Boot
*/
enum
{
    APPS_IMAGE_INDEX,
    MPSS_IMAGE_INDEX,
    //all more image before it
    LAST_IMAGE_INDEX	
};

enum 
{
    BOOT_FROM_MAIN_PARTITION =1,
    BOOT_FROM_FOTA_PARTITION,
    BOOT_CANCEL_LOAD
};

typedef struct
{
    uint32_t    magic;
    uint32_t    version;
    uint64_t    etb_buf_addr;
    uint64_t   *l2_cache_dump_buff_ptr;
    uint32_t    a64_pointer_padding;
    uint32_t    uefi_ram_dump_magic;
    uint32_t    ddr_training_cookie;
    uint32_t    abnormal_reset_occurred;
    uint32_t    reset_status_register;
    uint8_t     fota_cookie[LAST_IMAGE_INDEX];
}imem_cookie_type;

int                      load_modem_at_powerup = TRUE;
int                      modem_online = FALSE;
psm_qmi_client_info_type client_info;
psm_qmi_cache_type       psm_qmi_cache;
psm_status_type_e        psm_oos_status;
ril_platform_timer_t     tau_timer_id;

extern psm_modem_load_status modem_load_status;
extern int                   pending_shutdown;
extern int                   psm_pon_suppress_enabled;
extern void sys_m_shutdown(void);
extern void sys_m_reset(void);

void psm_qmi_backup_configuration_info(void);
void psm_modem_start_cb_fn(void * cb_data, subsys_start_status_type start_status);

/*========================================================================================
FUNCTION psm_qmi_get_psm_oos_status

Get network status.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
psm_status_type_e psm_qmi_get_psm_oos_status()
{
    return psm_oos_status;
}


/*========================================================================================
FUNCTION psm_qmi_unsol_ind_cb

Handle modem indications.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
void psm_qmi_unsol_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   message_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
)
{
    qmi_ind_callback_type qmi_callback;

    PSM_LOG_FUNC_ENTRY("psm_qmi_unsol_ind_cb: function entry");

    memset(&qmi_callback, 0 , sizeof(qmi_callback));
    qmi_callback.data_buf = malloc(ind_buf_len);

    if( qmi_callback.data_buf )
    {
        qmi_callback.user_handle   = user_handle;
        qmi_callback.msg_id        = message_id;
        qmi_callback.data_buf_len  = ind_buf_len;
        qmi_callback.cb_data       = ind_cb_data;
        memscpy(qmi_callback.data_buf, ind_buf_len, ind_buf,ind_buf_len);

        psm_queue_event(PSM_EVT_QMI_IND,
                        NIL,
                        (void*) &qmi_callback,
                        sizeof(qmi_callback));
    }
    else
    {
        PSM_LOG_INFO("psm_qmi_unsol_ind_cb: malloc failed");
    }

    PSM_LOG_FUNC_RETURN("psm_qmi_unsol_ind_cb: function return");
}

/*========================================================================================
FUNCTION psm_qmi_handle_indication

Handle any qmi indications received from modem.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_handle_indication(psm_event_params_type *parms)
{
    int                     event;
    int                     srv_index;
    qmi_ind_callback_type  *qmi_cb_data = NULL;

    PSM_LOG_FUNC_ENTRY("psm_qmi_handle_indication: function entry");

    do
    {
        if ( parms == NULL )
        {
            break;
        }
        event       = parms->event_id;
        qmi_cb_data = (qmi_ind_callback_type*)parms->data;
        if (qmi_cb_data == NULL)
        {
            break;
        }
        srv_index   = (int)qmi_cb_data->cb_data;

        PSM_LOG_INFO2("psm_qmi_handle_indication: event - %d srv_index - %d", event, srv_index);
        if ( event == PSM_EVT_QMI_IND )
        {
            switch(srv_index)
            {
                case PSM_QMI_CLIENT_DMS:
                    psm_qmi_handle_dms_indication(qmi_cb_data);
                    break;

                default:
                    break;
            }
        }

    }while(FALSE);

    if ( qmi_cb_data && qmi_cb_data->data_buf )
    {
        free(qmi_cb_data->data_buf);
        qmi_cb_data->data_buf = NULL;
    }

    PSM_LOG_FUNC_RETURN("psm_qmi_handle_indication: function return");
    return 0;
}

void psm_qmi_register_oprt_mode_ind(void)
{
    int                                  rc = 0;
    dms_set_event_report_req_msg_v01     qmi_request_power;
    dms_set_event_report_resp_msg_v01    qmi_response_power;
    qmi_client_error_type                qmi_transport_error;

    PSM_LOG_FUNC_ENTRY("psm_qmi_register_oprt_mode_ind: function entry");

    do
    {
        memset( &qmi_request_power, 0, sizeof( qmi_request_power ) );
        memset( &qmi_request_power, 0, sizeof( qmi_request_power ) );

        qmi_request_power.report_oprt_mode_state_valid = TRUE;
        qmi_request_power.report_oprt_mode_state = 1;

        qmi_transport_error = qmi_client_send_msg_sync(client_info.qmi_client_handles[PSM_QMI_CLIENT_DMS],
                                                       QMI_DMS_SET_EVENT_REPORT_REQ_V01,
                                                       &qmi_request_power,
                                                       sizeof(qmi_request_power),
                                                       &qmi_response_power,
                                                       sizeof(qmi_response_power),
                                                       PSM_QMI_TIMEOUT);

        if ( qmi_transport_error != QMI_NO_ERR )
        {
            PSM_LOG_INFO1("Failed to send sync qmi msg, error - %d", qmi_transport_error);
            PSM_QMI_ULOG_MSG("Failed to send sync qmi msg, error - %d", qmi_transport_error);
            rc = -1;
            break;
        }
        else
        {
            PSM_LOG_INFO2("Oprt mode event report req qmi response error - result: %d, error: %d",
                          qmi_response_power.resp.result,
                          qmi_response_power.resp.error);
            PSM_QMI_ULOG_MSG("Oprt mode event report req qmi response error - result: %d, error: %d",
                              qmi_response_power.resp.result,
                              qmi_response_power.resp.error);

            if (QMI_RESULT_SUCCESS_V01 != qmi_response_power.resp.result)
            {
                rc = -1;
                //Removing break for allowing tiny modem debug cases
                //where modem dms does not support all the requests but is active.
                //break;
            }
        }
    } while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
}

/*==============================================================================
FUNCTION psm_qmi_dms_client_init

Initializes qmi dms client

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_dms_client_init(void)
{
    int                                  rc = 0;
    qmi_client_error_type                qmi_transport_error;
    dms_indication_register_req_msg_v01  qmi_request;
    dms_indication_register_resp_msg_v01 qmi_response;

    PSM_LOG_FUNC_ENTRY("psm_qmi_dms_client_init: function entry");

    do
    {
        memset( &qmi_request, 0, sizeof( qmi_request ) );
        memset( &qmi_response, 0, sizeof( qmi_response ) );

        qmi_request.report_psm_status_valid    = TRUE;
        qmi_request.report_psm_status          = 1;

        qmi_request.report_psm_cfg_change_valid = TRUE;
        qmi_request.report_psm_cfg_change       = 1;

        qmi_transport_error = qmi_client_send_msg_sync(client_info.qmi_client_handles[PSM_QMI_CLIENT_DMS],
                                                       QMI_DMS_INDICATION_REGISTER_REQ_V01,
                                                       &qmi_request,
                                                       sizeof(qmi_request),
                                                       &qmi_response,
                                                       sizeof(qmi_response),
                                                       PSM_QMI_TIMEOUT);

        if ( qmi_transport_error != QMI_NO_ERR )
        {
            PSM_LOG_INFO1("Failed to send sync qmi msg, error - %d", qmi_transport_error);
            PSM_QMI_ULOG_MSG("Failed to send sync qmi msg, error - %d", qmi_transport_error);
            rc = -1;
            break;
        }
        else
        {
            PSM_LOG_INFO2("DMS Ind register qmi response error - result: %d, error: %d", qmi_response.resp.result, qmi_response.resp.error);
            PSM_QMI_ULOG_MSG("DMS Ind register  - result: %d, error: %d", 
                        qmi_response.resp.result, qmi_response.resp.error);

            if (QMI_RESULT_SUCCESS_V01 != qmi_response.resp.result)
            {
                rc = -1;
                break;
            }
        }

        PSM_LOG_INFO("psm_qmi_dms_client_init: Successfully registered for PSM status indications");
        PSM_QMI_ULOG_MSG("DMS Client Registered");

        /* There is no more dependency of settings based upon modem operating mode */
        //psm_qmi_register_oprt_mode_ind();
        //psm_qmi_fetch_modem_operational_status();
        psm_qmi_fetch_psm_configuration();

        if(rc == SUCCESS)
        {
            //psm_qmi_is_psm_supported(&psm_supported);
            //if(psm_supported == TRUE)
            if(psm_qmi_is_psm_supported() == TRUE)
            {
                psm_core_gpio_set(PSM_DAEMON_START);
                PSM_LOG_INFO("Successfully set configured GPIO to High since DMS Init complete");
                PSM_QMI_ULOG_MSG("Successfully configured GPIO to High since DMS Init complete");
            }
        }

        PSM_LOG_INFO("psm_qmi_dms_client_init: Successfully registered for radio status indications");
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

void psm_qmi_fetch_backup_psm_configuration(void )
{
    void *config_list = NULL;
    int   bootup_delay = -1;
    int   modem_cap_version = -1;

    PSM_LOG_FUNC_ENTRY("psm_qmi_fetch_backup_psm_configuration: function entry");

    config_list = util_config_load(PSM_NV_BACKUP_FILE);
    if ( config_list != NULL )
    {
        psm_qmi_cache.psm_supported_valid = TRUE;
        util_config_get_value(config_list,
                              PSM_ENABLED_KEY,
                              INTEGER,
                              &psm_qmi_cache.psm_supported);

        psm_qmi_cache.psm_threshold_value_valid = TRUE;
        util_config_get_value(config_list,
                              PSM_THRESHOLD_KEY,
                              INTEGER,
                              &psm_qmi_cache.psm_threshold_value);

        psm_qmi_cache.duration_due_to_oos_valid = TRUE;
        util_config_get_value(config_list,
                              PSM_OOS_DURATION_KEY,
                              INTEGER,
                              &psm_qmi_cache.duration_due_to_oos);

        if ( psm_qmi_cache.duration_due_to_oos == NIL )
        {
            psm_qmi_cache.duration_due_to_oos = 2*psm_qmi_cache.psm_threshold_value;
        }

        psm_qmi_cache.randomization_window_valid = TRUE;
        util_config_get_value(config_list,
                              PSM_RANDMIZATION_WINDOW_KEY,
                              INTEGER,
                              &psm_qmi_cache.randomization_window);

        psm_qmi_cache.periodic_tau_timer_valid = TRUE;
        util_config_get_value(config_list,
                              PSM_PERIODIC_TAU_TIMER,
                              INTEGER,
                              &psm_qmi_cache.periodic_tau_timer);

        psm_qmi_cache.periodic_tau_rcvd_time_valid = TRUE;
        util_config_get_value(config_list,
                              PSM_PERIODIC_TAU_RCVD_TIME,
                              INTEGER,
                              &psm_qmi_cache.periodic_tau_rcvd_time);

        psm_qmi_cache.psm_app_active_time_valid = TRUE;
        util_config_get_value(config_list,
                              PSM_APP_ACTIVE_TIME,
                              INTEGER,
                              &psm_qmi_cache.psm_app_active_time);

        util_config_get_value(config_list,
                              PSM_BOOTUP_DELAY,
                              INTEGER,
                              &bootup_delay);

        if( bootup_delay >= 0)
        {
            psm_qmi_cache.early_wakeup_time_valid= TRUE;
            psm_qmi_cache.early_wakeup_time = bootup_delay;
        }

        util_config_get_value(config_list,
                              PSM_MODEM_CAP_VERSION,
                              INTEGER,
                              &modem_cap_version);

        if( modem_cap_version >= 0)
        {
            psm_qmi_cache.modem_capability_version_valid = TRUE;
            psm_qmi_cache.modem_capability_version = 
                                        (dms_psm_version_capability_v01)modem_cap_version;
        }

        util_config_unload(config_list);
        config_list = NULL;
    }

    PSM_LOG_FUNC_RETURN("psm_qmi_fetch_backup_psm_configuration: function return");
}

/*==============================================================================
FUNCTION psm_qmi_modem_timer_expiry_cb

Callback to handle auto modem bring up in case of tau timer expiry

ARGUMENTS
    void

RETURN VALUE
    void
==============================================================================*/
void psm_qmi_modem_timer_expiry_cb(void* timer_expiry_data)
{
    PSM_LOG_FUNC_ENTRY("psm_qmi_modem_timer_expiry_cb: function entry");

    ril_platform_timer_delete(&tau_timer_id);
    psm_queue_event(PSM_EVT_LOAD_MODEM, -1, NULL, NIL);

    PSM_LOG_FUNC_RETURN("psm_qmi_modem_timer_expiry_cb: function return");
}

/*==============================================================================
FUNCTION psm_qmi_auto_start_modem

Start timer to auto power on modem if tau timer expires

ARGUMENTS
    tau time

RETURN VALUE
    void
==============================================================================*/
void psm_qmi_auto_start_modem(uint32  tau_time)
{
    PSM_LOG_FUNC_ENTRY("psm_qmi_auto_start_modem: function entry");

    psm_core_adjust_psm_bootup_time(&tau_time);
    if( tau_time <= 0 )
    {
        tau_time = 0;
    }
    PSM_LOG_INFO1("psm_qmi_auto_start_modem: Modem will start in %d seconds if not started till then", tau_time);
    ril_platform_timer_start_oneshot(&tau_timer_id, psm_qmi_modem_timer_expiry_cb, NULL, TIMER_1S * tau_time);

    PSM_LOG_FUNC_RETURN("psm_qmi_auto_start_modem: function return");
}

/*==============================================================================
FUNCTION psm_qmi_tau_timer_evaluate_load_modem

Load modem, initialize qmi clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_tau_timer_evaluate_load_modem(int *load_modem)
{
    uint32 curr_time = 0;
    uint32 elapsed_time = 0;
    uint32 remaining_tau_time = 0;
    int    rc = FAIL;

    PSM_LOG_FUNC_ENTRY("psm_qmi_tau_timer_evaluate_load_modem: function entry");

    if ( psm_qmi_cache.periodic_tau_timer_valid == TRUE &&
         psm_qmi_cache.periodic_tau_rcvd_time_valid == TRUE &&
                  ((int) psm_qmi_cache.periodic_tau_timer > 0) )
    {
        psm_rtc_get_time(&curr_time);
        elapsed_time = curr_time - psm_qmi_cache.periodic_tau_rcvd_time;
        remaining_tau_time = (psm_qmi_cache.periodic_tau_timer - elapsed_time);

        PSM_LOG_INFO3("psm_qmi_tau_timer_evaluate_load_modem: curr time - %d, psm tau time - %d, tau rcvd time - %d",
                                                curr_time,
                                                psm_qmi_cache.periodic_tau_timer,
                                                psm_qmi_cache.periodic_tau_rcvd_time );

        // If tau timer expired or less than PSM threshold value, start modem.
        if( psm_qmi_cache.psm_threshold_value_valid == TRUE )
        {
            if ( (int)remaining_tau_time < (int)psm_qmi_cache.psm_threshold_value )
            {
                PSM_LOG_INFO("Load modem as periodic tau timer expired or below threshold value");
                *load_modem = TRUE;
                PSM_QMI_ULOG_MSG("Modem loading because of Tau");
            }
            else
            {
                PSM_LOG_INFO("psm_qmi_tau_timer_evaluate_load_modem: Add remaining tau info to list");
                PSM_QMI_ULOG_MSG("Modem not loaded");
                *load_modem = FALSE;
                psm_qmi_cache.periodic_tau_timer_valid = TRUE;
                psm_qmi_cache.periodic_tau_timer = remaining_tau_time;
                psm_qmi_cache.periodic_tau_rcvd_time_valid = TRUE;
                psm_qmi_cache.periodic_tau_rcvd_time = curr_time;

                psm_list_add_auto_ready_info(psm_qmi_cache.periodic_tau_timer);
                psm_qmi_auto_start_modem(remaining_tau_time);
                psm_qmi_backup_configuration_info();
            }
        }
        else
        {
            *load_modem = TRUE;
        }

        rc = SUCCESS;
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}


/*==============================================================================
FUNCTION psm_qmi_modify_modem_load_based_on_boot

Load modem, initialize qmi clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_modify_modem_load_based_on_boot(int* load_modem)
{
    int               rc = 0;
    int               started = TRUE;
    qurt_addr_t       v_addr;

    PSM_LOG_FUNC_ENTRY("psm_qmi_modify_modem_load_based_on_boot: function entry");

    rc = qurt_mem_map_static_query(&v_addr, WONCE_REG_ADDRESS, 4, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE);
    if (rc == QURT_EOK)
    {
        if (v_addr)
        {
            started = *((int*)v_addr);
        }

        if (FALSE != started)
        {
            *load_modem = TRUE;
            PSM_QMI_ULOG_MSG("Modem loaded as part of Bootloader");
        }
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;

}

/*==============================================================================
FUNCTION psm_qmi_check_is_fota

Check if current bootup is FOTA bootup

ARGUMENTS
    type - Address of int flag that will contain TRUE/FALSE
           indicating if it is FOTA Boot or not.

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_check_is_fota(int* is_fota)
{
    int                   rc = FAIL;
    int                   itr = 0;
    imem_cookie_type     *cookie;

    PSM_LOG_FUNC_ENTRY("psm_qmi_check_is_fota: function entry");

    /* Set fota flag to FALSE initialized */
    *is_fota = FALSE;

    do
    {
        /* Attempt to Typecast IMEM shared memory to struct */
        cookie = ( imem_cookie_type *)ril_platform_get_imem_cookie_addr();
        PSM_QMI_ULOG_MSG("boot_shared_imem_cookie_ptr->shared_imem_magic: %zu", cookie->magic);
        PSM_QMI_ULOG_MSG("boot_shared_imem_cookie_ptr->shared_imem_version: %u",cookie->version);
        PSM_QMI_ULOG_MSG("boot_shared_imem_cookie_ptr->fota_info_cookie[0]: %u",cookie->fota_cookie[0]);
        PSM_QMI_ULOG_MSG("boot_shared_imem_cookie_ptr->fota_info_cookie[1]: %u",cookie->fota_cookie[1]);

        if(cookie->magic != ril_platform_get_imem_magic_num())
        {
            PSM_QMI_ULOG_MSG("IMEM magic doesn't match.");
            break;
        }

        for(itr=APPS_IMAGE_INDEX; itr<LAST_IMAGE_INDEX;itr++)
        {
            if(cookie->fota_cookie[itr] == BOOT_FROM_FOTA_PARTITION)
            {
                PSM_QMI_ULOG_MSG("FOTA Bootup of Image: %d Do not load modem",itr);
               *is_fota = TRUE;
                break;
            }
        }

        PSM_QMI_ULOG_MSG("is_fota flag: %d",*is_fota);
        rc = SUCCESS;
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_qmi_get_boot_type

Get if boot type is Hibernation exit or PSM exit

ARGUMENTS
    type - Address of enum to assign

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_get_boot_type(psm_boot_type_e* type)
{
    int               rc = FAIL;
    int               reg_value = NULL;
    int               is_fota = FALSE;
    qurt_addr_t       v_addr;

    PSM_LOG_FUNC_ENTRY("psm_qmi_get_boot_type: function entry");

    do
    {
        rc = psm_qmi_check_is_fota(&is_fota);
        if(rc == SUCCESS && is_fota == TRUE)
        {
            *type = FOTA_BOOTUP;
            PSM_QMI_ULOG_MSG("FOTA Boot Type detected. Skip modem loading");
            PSM_LOG_INFO("psm_qmi_get_boot_type: FOTA Boot Type detected. Skip modem loading");
            break;
        }

        rc = qurt_mem_map_static_query(&v_addr, WONCE_BOOT_ADDRESS, 4, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE);
        if (rc == QURT_EOK)
        {
            if (v_addr)
            {
                reg_value = *((int*)v_addr);
            }

            if (PSM_HIBERNATE_REGISTER_VALUE == reg_value)
            {
                *type = HIBERNATION_EXIT;
                PSM_QMI_ULOG_MSG("Bootup was after Hibernation");
            }
            else if(PSM_BOOT_REGISTER_VALUE == reg_value)
            {
                *type = OUT_OF_RESET;
                PSM_QMI_ULOG_MSG("Bootup was after PSM");
            }
            else
            {
                *type = OUT_OF_RESET;
                PSM_QMI_ULOG_MSG("Wonce register value: %d Assume PSM Bootup", reg_value);
            }
            rc = SUCCESS;
        }

        PSM_QMI_ULOG_MSG("PSM Boot Type: %d, Mem lookup call result: %d", *type, rc);
        PSM_LOG_INFO2("PSM Boot Type: %d, Mem lookup call result: %d", type, rc);

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;

}


/*==============================================================================
FUNCTION psm_qmi_init

Load modem, initialize qmi clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_init()
{
    int             rc = 0;
    int             load_modem = FALSE;
    psm_boot_type_e bootup_type = OUT_OF_RESET;

    PSM_LOG_FUNC_ENTRY("psm_qmi_init: function entry");
    PSM_CRITICAL_ULOG_INIT();

    psm_qmi_fetch_backup_psm_configuration();

#if defined(FEATURE_PSM_TARGET_9205) && !defined(PSM_APPS_ONLY_BOOT_SUPPORTED)
    (void)load_modem;
    (void)bootup_type;
    PSM_QMI_ULOG_MSG("Modem loaded as part of bootup");
    rc = psm_qmi_broadcast_and_qmi_init();

#else
    psm_qmi_read_modem_loading_flag(&load_modem);
    psm_qmi_modify_modem_load_based_on_boot(&load_modem);

/* Hibernation is not supported in 9x05 so no need of checking boot type for now */
#ifndef FEATURE_PSM_TARGET_9205
    psm_qmi_get_boot_type(&bootup_type);
#endif
    /* Modem loading can be true due to two reasons.
       1) Applications wanted to use modem
       2) PSM wakeup is due to modem tau timer.
       In case load_modem = FALSE, we need to consider
       modem tau timer in next PSM sleep to ensure that target
       connects to network before tau timer expires. */
    if ( load_modem == TRUE )
    {
        rc = psm_qmi_start_modem_subsystem(bootup_type);
        PSM_QMI_ULOG_MSG("Modem loaded");
    }
    else
    {
        PSM_LOG_INFO("psm_qmi_init: No need to start modem subsystem.. Check with threshold");
        PSM_LOG_INFO("psm_qmi_init: Evaluate and consider tau timer for next PSM");
        rc = psm_qmi_tau_timer_evaluate_load_modem(&load_modem);
        if ( load_modem == TRUE )
        {
            rc = psm_qmi_start_modem_subsystem(bootup_type);
        }
    }

#endif

    PSM_QMI_ULOG_MSG("qmi Init completed");
    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}


/*==============================================================================
FUNCTION psm_qmi_client_init

Initializes qmi clients

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_client_init()
{
    int                   i = 0;
    int                   rc = FAIL;
    int                   max_retry = 100;
    int                   time_out = 4;
    qmi_client_error_type client_err = 0;

    PSM_LOG_FUNC_ENTRY("psm_qmi_client_init: function entry");

    do
    {

        PSM_LOG_DEBUG( "psm_qmi_client_init: Client connecting to QMI FW start" );
        PSM_QMI_ULOG_MSG("Client connecting to QMI FW start");
        client_info.service_objects[PSM_QMI_CLIENT_DMS]   = dms_get_service_object_v01();

        /* ensure that modem is up and DMS client is connected before proceeding further. */
        do
        {
            client_err = qmi_client_init_instance(client_info.service_objects[PSM_QMI_CLIENT_DMS],
                                                  QMI_CLIENT_INSTANCE_ANY,
                                                  psm_qmi_unsol_ind_cb,
                                                  (void *)PSM_QMI_CLIENT_DMS,
                                                  &client_info.os_params[PSM_QMI_CLIENT_DMS],
                                                  time_out,
                                                  &client_info.qmi_client_handles[PSM_QMI_CLIENT_DMS]);

            PSM_LOG_INFO2("qmi_client_init_instance returned (%d) for DMS. Retry attempt - %d", client_err, i);
            if ( client_err == QMI_NO_ERR )
            {
                rc = SUCCESS;
                break;
            }
            ril_platform_thread_sleep(10*SLEEP_10MS);
            i++;

        }while(i <= max_retry);

        PSM_LOG_DEBUG1("psm_qmi_client_init: Client connecting to QMI FW completed: Attempts: %d", i);
        PSM_QMI_ULOG_MSG("Client connecting to QMI FW completed: Attempts: %d", i);

        if ( client_err != QMI_NO_ERR )
        {
            rc = -1;
            break;
        }

        psm_qmi_dms_client_init();
    } while (FALSE);


    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_qmi_client_release

Releases qmi clients handles.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_client_release()
{
    int        rc = 0;
    int        idx = 0;

    PSM_LOG_FUNC_ENTRY("psm_qmi_client_release: function entry");

    for (idx = PSM_QMI_CLIENT_FIRST; idx < PSM_QMI_CLIENT_MAX; idx++)
    {
       if(client_info.qmi_client_handles[idx])
       {
          qmi_client_release(client_info.qmi_client_handles[idx]);
          client_info.qmi_client_handles[idx] = NULL;
       }
    }

    PSM_LOG_FUNC_RETURN("psm_qmi_client_release: function return");
    return rc;
}

/*==============================================================================
FUNCTION psm_qmi_get_threshold_time

Get PSM threshold value.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_get_threshold_time(int *threshold_time)
{
    int rc = -1;

    PSM_LOG_FUNC_ENTRY("psm_qmi_get_threshold_time: function entry");

    if ( psm_qmi_cache.psm_threshold_value_valid )
    {
        *threshold_time = psm_qmi_cache.psm_threshold_value;
        rc = 0;
    }
    else
    {
        psm_qmi_fetch_psm_configuration();
        if ( psm_qmi_cache.psm_supported_valid )
        {
            *threshold_time = psm_qmi_cache.psm_threshold_value;
            rc = 0;
        }
    }

    PSM_LOG_INFO1("PSM Threshold time - %d",psm_qmi_cache.psm_threshold_value);
    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_qmi_update_apps_active_time_info

Update Periodic timer information in cache.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_update_apps_active_time_info(int active_time)
{
    int rc = 0;

    PSM_LOG_FUNC_ENTRY("psm_qmi_update_apps_active_time_info: function entry");

    psm_qmi_cache.psm_app_active_time_valid = TRUE;
    psm_qmi_cache.psm_app_active_time       = active_time;

    psm_qmi_backup_configuration_info();

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_qmi_update_periodic_timer_info

Update Periodic timer information in cache.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_update_periodic_timer_info(int periodic_tau)
{
    int rc = 0;

    PSM_LOG_FUNC_ENTRY("psm_qmi_update_periodic_timer_info: function entry");

    psm_qmi_cache.periodic_tau_timer_valid = TRUE;
    psm_qmi_cache.periodic_tau_timer       = periodic_tau;

    psm_qmi_cache.periodic_tau_rcvd_time_valid = TRUE;
    psm_rtc_get_time((uint32*)&psm_qmi_cache.periodic_tau_rcvd_time);

    psm_qmi_backup_configuration_info();

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_qmi_backup_configuration_info

Backup PSM configuration to be used in case of modem is not started in next boot.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
void psm_qmi_backup_configuration_info(void)
{
    int   fp_backup = INVALID_FILE_HANDLE;
    char  str[PSM_CNF_LEN];

    PSM_LOG_FUNC_ENTRY("psm_qmi_backup_configuration_info: function entry");

    /* Delete if file already exist */
    efs_unlink(PSM_NV_BACKUP_FILE);

    fp_backup = efs_open(PSM_NV_BACKUP_FILE, O_WRONLY|O_CREAT);
    if ( fp_backup != INVALID_FILE_HANDLE )
    {
        if ( psm_qmi_cache.psm_supported_valid )
        {
            snprintf(str, PSM_CNF_LEN, "%s=%d\n", PSM_ENABLED_KEY, psm_qmi_cache.psm_supported);
        }
        else
        {
            snprintf(str, PSM_CNF_LEN, "%s=%d\n", PSM_ENABLED_KEY, 0);
        }

        if ( psm_qmi_cache.psm_threshold_value_valid )
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_THRESHOLD_KEY, psm_qmi_cache.psm_threshold_value);
        }
        else
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_THRESHOLD_KEY, 0);
        }

        if ( psm_qmi_cache.duration_due_to_oos_valid )
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_OOS_DURATION_KEY, psm_qmi_cache.duration_due_to_oos);
        }
        else
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_OOS_DURATION_KEY, psm_qmi_cache.duration_due_to_oos);
        }

        if ( psm_qmi_cache.randomization_window_valid )
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_RANDMIZATION_WINDOW_KEY, psm_qmi_cache.randomization_window);
        }
        else
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_RANDMIZATION_WINDOW_KEY, 0);
        }

        if ( psm_qmi_cache.periodic_tau_timer_valid && ((int)psm_qmi_cache.periodic_tau_timer > 0) )
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_PERIODIC_TAU_TIMER, psm_qmi_cache.periodic_tau_timer);
        }
        else
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_PERIODIC_TAU_TIMER, 0);
        }

        if ( psm_qmi_cache.periodic_tau_rcvd_time_valid )
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_PERIODIC_TAU_RCVD_TIME, psm_qmi_cache.periodic_tau_rcvd_time);
        }
        else
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_PERIODIC_TAU_RCVD_TIME, 0);
        }

        if ( psm_qmi_cache.psm_app_active_time_valid )
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_APP_ACTIVE_TIME, psm_qmi_cache.psm_app_active_time);
        }
        else
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_APP_ACTIVE_TIME, 0);
        }

        if ( psm_qmi_cache.early_wakeup_time_valid )
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_BOOTUP_DELAY, (uint32_t)psm_qmi_cache.early_wakeup_time);
        }
        else
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_BOOTUP_DELAY, -1);
        }

        if ( psm_qmi_cache.modem_capability_version_valid)
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_MODEM_CAP_VERSION, psm_qmi_cache.modem_capability_version);
        }
        else
        {
            snprintf(str + strlen(str), PSM_CNF_LEN - strlen(str), "%s=%d\n", PSM_MODEM_CAP_VERSION, -1);
        }

        efs_write(fp_backup, str, strlen(str));

        efs_close(fp_backup);
        fp_backup = INVALID_FILE_HANDLE;
    }

    PSM_LOG_FUNC_RETURN("psm_qmi_backup_configuration_info: function return");
}


/*==============================================================================
FUNCTION psm_qmi_fetch_modem_operational_status

Fetch Modem operating status.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_fetch_modem_operational_status(void)
{
    int                                 rc = FAIL;
    int                                 i = 0;
    int                                 retry_count = 25;
    qmi_client_error_type               err = QMI_NO_ERR;
    dms_get_operating_mode_resp_msg_v01 qmi_resp;


    PSM_LOG_FUNC_ENTRY("psm_qmi_fetch_modem_operational_status: function entry");

    memset(&qmi_resp, 0, sizeof(qmi_resp));

    do
    {
        err = qmi_client_send_msg_sync(client_info.qmi_client_handles[PSM_QMI_CLIENT_DMS],
                                       QMI_DMS_GET_OPERATING_MODE_REQ_V01,
                                       NULL,
                                       NIL,
                                       &qmi_resp,
                                       sizeof(qmi_resp),
                                       PSM_QMI_TIMEOUT);

        PSM_LOG_INFO2("result - %d error - %d", qmi_resp.resp.result, qmi_resp.resp.error);
        PSM_QMI_ULOG_MSG("result - %d error - %d", qmi_resp.resp.result, qmi_resp.resp.error);

        if ( (err == QMI_NO_ERR) && (qmi_resp.resp.result == SUCCESS) )
        {
            rc = SUCCESS;
            break;
        }

        if(qmi_resp.resp.error == QMI_ERR_NOT_SUPPORTED_V01)
        {
            //For Page Only Image, this request is not supported.
            PSM_QMI_ULOG_MSG("Modem oprt mode req not supported. Setting Modem to Online");
            modem_online = TRUE;
            break;
        }

        i++;
        ril_platform_thread_sleep(50*SLEEP_10MS);

    }while( i <= retry_count );

    if ( err == QMI_NO_ERR )
    {
        if ( qmi_resp.resp.result == QMI_RESULT_SUCCESS_V01 )
        {

            PSM_LOG_INFO1("Modem oprt mode - %d", qmi_resp.operating_mode);
            PSM_QMI_ULOG_MSG("Modem oprt mode - %d", qmi_resp.operating_mode);

            if ( qmi_resp.operating_mode == DMS_OP_MODE_ONLINE_V01 )
            {
                modem_online = TRUE;
            }
            else
            {
                modem_online = FALSE;
            }
        }
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}


/*==============================================================================
FUNCTION psm_qmi_fetch_psm_configuration

Fetch PSM configuration information from modem.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_fetch_psm_configuration(void)
{
    int                                 rc = SUCCESS;
    qmi_client_error_type               err = QMI_NO_ERR;
    dms_psm_get_cfg_params_resp_msg_v01 qmi_resp;
    dms_get_capability_req_msg_v01      qmi_request_capability;
    dms_get_capability_resp_msg_v01     qmi_response_capability;

    PSM_LOG_FUNC_ENTRY("psm_qmi_fetch_psm_configuration: function entry");

    memset(&qmi_resp, 0, sizeof(qmi_resp));

    err = qmi_client_send_msg_sync(client_info.qmi_client_handles[PSM_QMI_CLIENT_DMS],
                                   QMI_DMS_PSM_GET_CFG_PARAMS_REQ_V01,
                                   NULL,
                                   NIL,
                                   &qmi_resp,
                                   sizeof(qmi_resp),
                                   PSM_QMI_TIMEOUT);

    PSM_LOG_INFO2("result - %d error - %d", qmi_resp.resp.result, qmi_resp.resp.error);

    if ( err == QMI_NO_ERR )
    {
        if ( qmi_resp.resp.result == QMI_RESULT_SUCCESS_V01 )
        {
            if ( qmi_resp.psm_enabled_valid )
            {
                PSM_LOG_INFO1("psm_qmi_fetch_psm_configuration: psm enabled - %d", qmi_resp.psm_enabled);
                PSM_QMI_ULOG_MSG("psm enabled - %d", qmi_resp.psm_enabled);
                psm_qmi_cache.psm_supported_valid = TRUE;
                psm_qmi_cache.psm_supported = qmi_resp.psm_enabled;
            }
            if ( qmi_resp.duration_threshold_valid )
            {
                PSM_LOG_INFO1("psm_qmi_fetch_psm_configuration: psm threshold - %d", qmi_resp.duration_threshold);
                PSM_QMI_ULOG_MSG("psm threshold - %d", qmi_resp.duration_threshold);
                psm_qmi_cache.psm_threshold_value_valid = TRUE;
                psm_qmi_cache.psm_threshold_value = qmi_resp.duration_threshold;
            }
            if ( qmi_resp.randomization_window_valid )
            {
                PSM_LOG_INFO1("psm_qmi_fetch_psm_configuration: randomization window - %d", qmi_resp.randomization_window);
                PSM_QMI_ULOG_MSG("randomization window - %d", qmi_resp.randomization_window);
                psm_qmi_cache.randomization_window_valid = TRUE;
                psm_qmi_cache.randomization_window = qmi_resp.randomization_window;
            }
            if ( qmi_resp.early_wakeup_time_valid )
            {
                PSM_LOG_INFO1("psm_qmi_fetch_psm_configuration: Bootup Adjustment - %d", qmi_resp.early_wakeup_time);
                PSM_QMI_ULOG_MSG("Bootup Adjustment - %d", qmi_resp.early_wakeup_time);
                psm_qmi_cache.early_wakeup_time_valid = TRUE;
                psm_qmi_cache.early_wakeup_time = qmi_resp.early_wakeup_time;
            }

            PSM_LOG_INFO1("Duration due to OOS valid - %d", qmi_resp.duration_due_to_oos_valid);
            if ( !qmi_resp.duration_due_to_oos_valid ||
                  qmi_resp.duration_due_to_oos < (2*psm_qmi_cache.psm_threshold_value) )
            {
                psm_qmi_cache.duration_due_to_oos_valid = TRUE;
                psm_qmi_cache.duration_due_to_oos = 2*qmi_resp.duration_threshold;
                PSM_LOG_INFO1("Update duration due to OOS to - %d", psm_qmi_cache.duration_due_to_oos);
                PSM_QMI_ULOG_MSG("Update duration due to OOS to - %d", psm_qmi_cache.duration_due_to_oos);
            }
            else
            {
                PSM_LOG_INFO1("Duration due to OOS - %d", qmi_resp.duration_due_to_oos);
                PSM_QMI_ULOG_MSG("Duration due to OOS - %d", qmi_resp.duration_due_to_oos);
                psm_qmi_cache.duration_due_to_oos_valid = TRUE;
                psm_qmi_cache.duration_due_to_oos = qmi_resp.duration_due_to_oos;
            }
        }
    }

    /* Fetch the version of the PSM and update */
    memset( &qmi_request_capability, 0, sizeof( qmi_request_capability ) );
    memset( &qmi_response_capability, 0, sizeof( qmi_response_capability ) );

    qmi_request_capability.te_psm_capability_valid = 1;
    qmi_request_capability.te_psm_capability = DMS_PSM_VERSION_2_V01; //RIL is capable of handling both;
    err = qmi_client_send_msg_sync(client_info.qmi_client_handles[PSM_QMI_CLIENT_DMS],
                                                   QMI_DMS_GET_CAPABILITY_REQ_V01,
                                                   &qmi_request_capability,
                                                   sizeof(qmi_request_capability),
                                                   &qmi_response_capability,
                                                   sizeof(qmi_response_capability),
                                                   PSM_QMI_TIMEOUT);

    if ( err != QMI_NO_ERR )
    {
        PSM_LOG_INFO1("Failed to send sync qmi msg, error - %d", err);
        PSM_QMI_ULOG_MSG("Failed to send sync qmi msg, error - %d", err);
        rc = -1;
    }
    else
    {
        PSM_LOG_INFO2("DMS Get capabilityresp - result: %d, error: %d", qmi_response_capability.resp.result, qmi_response_capability.resp.error);
        PSM_QMI_ULOG_MSG("DMS Get capabilityresp - result: %d, error: %d", 
                    qmi_response_capability.resp.result, qmi_response_capability.resp.error);

        if (QMI_RESULT_SUCCESS_V01 != qmi_response_capability.resp.result)
        {
            rc = -1;
        }

        else if(qmi_response_capability.psm_capability_valid)
        {
            rc = 0;
            psm_qmi_cache.modem_capability_version_valid = 1;
            psm_qmi_cache.modem_capability_version = qmi_response_capability.psm_capability;
            PSM_LOG_INFO1("Modem PSM Version capability: %d", psm_qmi_cache.modem_capability_version);
            PSM_QMI_ULOG_MSG("Modem PSM Version capability: %d", psm_qmi_cache.modem_capability_version);
            if(qmi_response_capability.psm_capability == DMS_PSM_VERSION_2_V01)
            {
                /* Since this is modem controlled PSM, we will need a compulsory 
                   READY or AUTOREADY indication from modem. So ADD modem as a client */
                   psm_list_add_newclient(PSM_LIST_MODEM_CLIENT_FD);
            }
        }
    }

    psm_qmi_backup_configuration_info();
    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}


/*==============================================================================
FUNCTION psm_qmi_is_psm_supported

Check if modem supports PSM feature.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
/*int psm_qmi_is_psm_supported(int *psm_supported)
{
    int rc = FAIL;

    PSM_LOG_FUNC_ENTRY();

    if ( psm_qmi_cache.psm_supported_valid )
    {
        *psm_supported = psm_qmi_cache.psm_supported;
        rc = SUCCESS;
    }
    else
    {
        psm_qmi_fetch_psm_configuration();
        if ( psm_qmi_cache.psm_supported_valid )
        {
            *psm_supported = psm_qmi_cache.psm_supported;
            rc = SUCCESS;
        }
    }

    PSM_QMI_ULOG_MSG("PSM Supported: %d", *psm_supported);
    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}*/

int psm_qmi_is_psm_supported(void)
{
   int psm_supported = FALSE;

    PSM_LOG_FUNC_ENTRY("psm_qmi_is_psm_supported: function entry");

    if ( psm_qmi_cache.psm_supported_valid )
    {
        psm_supported = psm_qmi_cache.psm_supported;
    }
    else
    {
        psm_qmi_fetch_psm_configuration();
        if ( psm_qmi_cache.psm_supported_valid )
        {
            psm_supported = psm_qmi_cache.psm_supported;
        }
    }

    PSM_QMI_ULOG_MSG("PSM Supported: %d", psm_supported);
    PSM_LOG_FUNC_RETURN_WITH_RET(psm_supported);
    return psm_supported;
}

/*==============================================================================
FUNCTION psm_qmi_is_modem_controlled_psm

Check if the Modem is controlling entity.
If modem is the controlling entity then the following are changed:-
    1. RTC is set by modem in case of PSM
    2. Enter PSM cannot be called by PSMD independently. Needs a ready/autoready
       from Modem.
    3. Modem voted time must not be used to calculate time sent in Enter Request

ARGUMENTS
    Pointer to the value

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_is_modem_controlled_psm(int *modem_controlled)
{
    int rc = FAIL;

    PSM_LOG_FUNC_ENTRY("psm_qmi_is_modem_controlled_psm: function entry");

    if (!psm_qmi_cache.modem_capability_version_valid)
    {
        /* Attempt to fetch fresh from DMS */
        psm_qmi_fetch_psm_configuration();
    }

    if ((psm_qmi_cache.modem_capability_version_valid))
    {
        if(DMS_PSM_VERSION_2_V01 == psm_qmi_cache.modem_capability_version)
        {
            *modem_controlled = TRUE;
            rc = SUCCESS;
        }
        else if(DMS_PSM_VERSION_1_V01 == psm_qmi_cache.modem_capability_version)
        {
            *modem_controlled = FALSE;
            rc = SUCCESS;
        }
    }

    PSM_QMI_ULOG_MSG("Modem Control: %d", *modem_controlled);
    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}


/*==============================================================================
FUNCTION psm_qmi_set_psm_active_time

Set PSM active time.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_set_psm_active_time(int time_in_secs)
{
    int                                 rc = SUCCESS;
    int                                 update_active_time = TRUE;
    qmi_client_error_type               err = QMI_NO_ERR;
    dms_psm_set_cfg_params_req_msg_v01  qmi_req;
    dms_psm_set_cfg_params_resp_msg_v01 qmi_resp;

    PSM_LOG_FUNC_ENTRY("psm_qmi_set_psm_active_time: function entry");


    PSM_LOG_INFO2("previous active time  valid - %d, previous active time - %d",
        psm_qmi_cache.psm_app_active_time_valid, psm_qmi_cache.psm_app_active_time);

    if ( psm_qmi_cache.psm_app_active_time_valid &&
         psm_qmi_cache.psm_app_active_time == time_in_secs )
    {
        update_active_time = FALSE;
    }

    PSM_LOG_INFO2("psm_qmi_set_psm_active_time: active time - %d, update decision - %d",
            psm_qmi_cache.psm_app_active_time, update_active_time);

    if (update_active_time == TRUE )
    {
        memset(&qmi_req, 0, sizeof(qmi_req));
        memset(&qmi_resp, 0, sizeof(qmi_resp));

        qmi_req.active_timer_valid = TRUE;
        qmi_req.active_timer       = time_in_secs;

        err = qmi_client_send_msg_sync(client_info.qmi_client_handles[PSM_QMI_CLIENT_DMS],
                                       QMI_DMS_PSM_SET_CFG_PARAMS_REQ_V01,
                                       &qmi_req,
                                       sizeof(qmi_req),
                                       &qmi_resp,
                                       sizeof(qmi_resp),
                                       PSM_QMI_TIMEOUT);

        PSM_LOG_INFO1("psm_qmi_set_psm_active_time: update modem with active time - %d", time_in_secs);
        PSM_LOG_INFO3("transport err - %d result - %d error - %d",
                        err, qmi_resp.resp.result, qmi_resp.resp.error);
        PSM_QMI_ULOG_MSG("transport err - %d result - %d error - %d",
                        err, qmi_resp.resp.result, qmi_resp.resp.error);
        if ( err == QMI_NO_ERR )
        {
            if ( qmi_resp.resp.result == QMI_RESULT_SUCCESS_V01 )
            {
                rc = SUCCESS;
                /* Once we update active time to modem, store this to ensure that
                   we don't send same active time once again */
                psm_qmi_update_apps_active_time_info(time_in_secs);
            }
            else
            {
                rc = FAIL;
            }
        }
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_qmi_set_psm_ready

Inform modem that APPS is ready to enter PSM mode.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_set_psm_ready(int time_in_secs, int accept)
{
    int                          rc = FAIL;
    qmi_client_error_type        err = QMI_NO_ERR;
    dms_psm_enter_req_msg_v01    qmi_req;
    dms_psm_enter_resp_msg_v01   qmi_resp;

    PSM_LOG_FUNC_ENTRY("psm_qmi_set_psm_ready: function entry");
    /*Previously we were skipping this and directly shutting down device
      if modem was not Online. But this behavior is deprecated */
        memset(&qmi_req, 0, sizeof(qmi_req));
        memset(&qmi_resp, 0, sizeof(qmi_resp));

        if(accept)
        {
            qmi_req.duration_in_psm_valid = TRUE;
            qmi_req.duration_in_psm       = time_in_secs;
        }
        qmi_req.status_valid = TRUE;
        qmi_req.status = accept;
        PSM_LOG_INFO1("psm_qmi_set_psm_ready: Posting Enter PSM with status: %d", accept);
        PSM_QMI_ULOG_MSG("Posting Enter PSM with status: %d", accept);
        err = qmi_client_send_msg_sync(client_info.qmi_client_handles[PSM_QMI_CLIENT_DMS],
                                       QMI_DMS_PSM_ENTER_REQ_V01,
                                       &qmi_req,
                                       sizeof(qmi_req),
                                       &qmi_resp,
                                       sizeof(qmi_resp),
                                       PSM_QMI_TIMEOUT);

        PSM_LOG_INFO4("Enter PSM for - %u secs transport err - %d result - %d error - %d",
                     time_in_secs,
                     err,
                     qmi_resp.resp.result,
                     qmi_resp.resp.error);
        PSM_QMI_ULOG_MSG("Enter PSM for - %u secs transport err - %d result - %d error - %d",
                     time_in_secs,
                     err,
                     qmi_resp.resp.result,
                     qmi_resp.resp.error);
        PSM_LOG_MSG_CRITICAL(
            "Enter PSM for - %u secs with status[%d] transport err - %d result - %d error - %d",
                     time_in_secs,
                     qmi_req.status,
                     err,
                     qmi_resp.resp.result,
                     qmi_resp.resp.error);


        if ( err == QMI_NO_ERR )
        {
            if ( qmi_resp.resp.result == QMI_RESULT_SUCCESS_V01 )
            {
                rc = SUCCESS;
            }
        }


    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_qmi_convert_reject_reason

Convert modem PSM reject reason to LE PSM reject reason.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_convert_reject_reason(dms_psm_status_ind_msg_v01 *qmi_ind, psm_reject_reason_type_e *reject_reason)
{
    int          rc = 0;

    PSM_LOG_FUNC_ENTRY("psm_qmi_convert_reject_reason: function entry");

    if ( reject_reason && qmi_ind->reject_reason_valid )
    {
        PSM_LOG_INFO1("psm_qmi_convert_reject_reason: Modem reject reason - %d",
                       qmi_ind->reject_reason);

        switch(qmi_ind->reject_reason)
        {
            case DMS_PSM_REJECT_REASON_DURATION_TOO_SHORT_V01:
                *reject_reason = PSM_REJECT_REASON_DURATION_TOO_SHORT;
                break;

            case DMS_PSM_REJECT_REASON_MODEM_NOT_READY_V01:
                *reject_reason = PSM_REJECT_REASON_MODEM_NOT_READY;
                break;

            case DMS_PSM_REJECT_REASON_NOT_ENABLED_V01:
                *reject_reason = PSM_REJECT_REASON_NOT_ENABLED;
                break;

            case DMS_PSM_REJECT_REASON_NO_ERROR_V01:
                *reject_reason = PSM_REJECT_REASON_NONE;
                break;

            default:
                PSM_LOG_INFO1("Invalid reject reason - %d", qmi_ind->reject_reason);
                rc = -1;
                break;
        }
    }
    else
    {
        rc = -1;
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_qmi_handle_auto_ready

Handle PSM auto ready.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_handle_auto_ready(dms_psm_status_ind_msg_v01 *qmi_ind)
{
    psm_qmi_ready_indication_payload payload;
    memset(&payload, NIL, sizeof(payload));
    PSM_LOG_FUNC_ENTRY("psm_qmi_handle_auto_ready: function entry");

    if(qmi_ind)
    {
        payload.periodic_update_timer_valid = qmi_ind->periodic_update_timer_valid;
        payload.periodic_update_timer = qmi_ind->periodic_update_timer;
        payload.psm_status = qmi_ind->psm_status;
        psm_queue_event(PSM_EVT_AUTO_READY,
                        NIL,
                        (void*) &payload,
                        sizeof(payload));
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(0);
    return 0;
}


/*========================================================================================
FUNCTION psm_qmi_handle_dms_psm_cfg_change_ind

Handle PSM configuration change indication.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_handle_dms_psm_cfg_change_ind(dms_psm_cfg_params_change_ind_msg_v01 *qmi_ind)
{
    int                      rc = 0;
    psm_reject_reason_type_e reason = PSM_REJECT_REASON_NONE;
    boolean update_active_timer = FALSE;
    PSM_LOG_FUNC_ENTRY("psm_qmi_handle_dms_psm_cfg_change_ind: function entry");

    if ( qmi_ind )
    {
        if ( qmi_ind->psm_enabled_valid == TRUE )
        {
            PSM_LOG_INFO1("psm_qmi_handle_dms_psm_cfg_change_ind: psm_enabled - %d", qmi_ind->psm_enabled);
            if ( qmi_ind->psm_enabled == 1 &&
                 ( psm_qmi_cache.psm_supported_valid && psm_qmi_cache.psm_supported == FALSE ) )
            {
                // Send status to all clients
                psm_core_send_psm_status_to_allclients(PSM_STATUS_FEATURE_ENABLED, reason);
                psm_qmi_fetch_psm_configuration();
            }
            else if ( qmi_ind->psm_enabled == 0 &&
                      ( psm_qmi_cache.psm_supported_valid && psm_qmi_cache.psm_supported == TRUE ) )
            {
                // Send status to all clients
                psm_core_send_psm_status_to_allclients(PSM_STATUS_FEATURE_DISABLED, reason);
                psm_list_reset_all_clients_timeinfo();
                psm_qmi_fetch_psm_configuration();
            }

            PSM_LOG_INFO1("psm_qmi_handle_dms_psm_cfg_change_ind: periodic_update_timer_valid - %d",
                          qmi_ind->periodic_update_timer_valid);
            if ( qmi_ind->periodic_update_timer_valid == TRUE )
            {
                // handle change in periodic timer
            }

            PSM_LOG_INFO1("psm_qmi_handle_dms_psm_cfg_change_ind: active_timer_valid - %d",
                           qmi_ind->active_timer_valid);
            if ( qmi_ind->active_timer_valid == TRUE )
            {
                // handle change in active timer
                PSM_LOG_INFO1("psm_qmi_handle_dms_psm_cfg_change_ind: active_timer - %d",
                           qmi_ind->active_timer);
                update_active_timer = TRUE;
                if ( psm_qmi_cache.psm_app_active_time_valid &&
                    psm_qmi_cache.psm_app_active_time == qmi_ind->active_timer )
                {
                    update_active_timer = FALSE;
                }
                if (update_active_timer == TRUE)
                {
                    psm_qmi_update_apps_active_time_info(qmi_ind->active_timer);
                }
            }
        }
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}


/*========================================================================================
FUNCTION psm_qmi_handle_dms_modem_status_ind

Handle Modem status change indication.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_handle_dms_modem_status_ind(dms_event_report_ind_msg_v01 *qmi_ind)
{
    int                      rc = 0;

    PSM_LOG_FUNC_ENTRY("psm_qmi_handle_dms_modem_status_ind: function entry");

    if ( qmi_ind )
    {
        if ( qmi_ind->operating_mode_valid )
        {
            PSM_LOG_INFO1("psm_qmi_handle_dms_modem_status_ind: Modem oprt mode - %d",
                           qmi_ind->operating_mode);
            PSM_QMI_ULOG_MSG("Modem oprt mode - %d", qmi_ind->operating_mode);

            if ( qmi_ind->operating_mode == DMS_OP_MODE_ONLINE_V01 )
            {
                modem_online = TRUE;
            }
            else
            {
                modem_online = FALSE;
            }
        }
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}



/*========================================================================================
FUNCTION psm_qmi_handle_dms_psm_status_ind

Handle PSM status indication from modem.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_handle_dms_psm_status_ind(dms_psm_status_ind_msg_v01 *qmi_ind)
{
    int                      rc = 0;
    int                      clients_count = 0;
    uint8_t                  updated_time_valid = FALSE;
    uint32_t                 updated_time = NULL;
    uint8_t                  modem_only_time_valid = FALSE;
    uint32_t                 modem_only_time = NULL;

    psm_reject_reason_type_e reason = PSM_REJECT_REASON_NONE;

    PSM_LOG_FUNC_ENTRY("psm_qmi_handle_dms_psm_status_ind: function entry");

    do
    {
        //psm_qmi_is_psm_supported(&psm_enabled);
        //if( FALSE == psm_enabled )
        if(psm_qmi_is_psm_supported() == FALSE)
        {
            PSM_LOG_INFO("PSM not enabled. Skipping indication handling");
            break;
        }
        if ( qmi_ind )
        {

            clients_count = psm_list_get_number_of_clients(FALSE);
            if ( qmi_ind->reject_reason_valid )
            {
                psm_qmi_convert_reject_reason(qmi_ind, &reason);
            }

            PSM_QMI_ULOG_MSG("psm status DMS indication rcvd: %d", qmi_ind->psm_status);
            switch(qmi_ind->psm_status)
            {
                /* DMS_PSM_STATUS_AUTO_READY_V01 will come when NAS active timer expires.*/
                case DMS_PSM_STATUS_AUTO_READY_V01:
                    /* READY and AUTO READY Handling is now same */
                case DMS_PSM_STATUS_READY_V01:
                    PSM_LOG_INFO("psm_qmi_handle_dms_psm_status_ind: Modem is ready to enter PSM mode");
                    PSM_LOG_MSG_CRITICAL("psm_qmi_handle_dms_psm_status_ind: Modem is ready to enter PSM mode");
                    psm_core_set_psm_state(PSM_STATE_MODEM_READY_AP_AWAITED);
                    psm_qmi_handle_auto_ready(qmi_ind);
                    break;

                case DMS_PSM_STATUS_COMPLETED_V01:
                    PSM_LOG_INFO("psm_qmi_handle_dms_psm_status_ind: Modem entered PSM mode");
                    PSM_LOG_MSG_CRITICAL("psm_qmi_handle_dms_psm_status_ind: Modem entered PSM mode");
                    if ( qmi_ind->periodic_update_timer_valid == TRUE )
                    {
                        PSM_LOG_INFO1(
                        "psm_qmi_handle_dms_psm_status_ind: Updated time in complete indication - %u",
                        qmi_ind->periodic_update_timer);
                        updated_time = qmi_ind->periodic_update_timer;
                        updated_time_valid = qmi_ind->periodic_update_timer_valid;
                    }
                    else
                    {
                        updated_time = NULL;
                        updated_time_valid = FALSE;
                    }
                    if ( qmi_ind->modem_only_timer_valid == TRUE )
                    {
                        PSM_LOG_INFO1(
                        "psm_qmi_handle_dms_psm_status_ind: Modem only timer in complete indication - %u",
                        qmi_ind->modem_only_timer);
                        modem_only_time = qmi_ind->modem_only_timer;
                        modem_only_time_valid = qmi_ind->modem_only_timer_valid;
                    }
                    else
                    {
                        modem_only_time = NULL;
                        modem_only_time_valid = FALSE;
                    }

                    psm_core_set_psm_state(PSM_STATE_COMPLETE);
                    if(qmi_ind->reject_reason_valid)
                    {
                        PSM_LOG_INFO1("Debug Usecase for Tinymodem is set: %d", qmi_ind->reject_reason);
                        PSM_QMI_ULOG_MSG("Debug Usecase for Tinymodem is set: %d", qmi_ind->reject_reason);
                        if(psm_pon_suppress_enabled == TRUE)
                        {
                            pm_pon_trigger_enable(0x0, PM_PON_TRIGGER_PON1, PSM_USB_PON_DISABLE);
                            PSM_LOG_INFO("Triggered USB PON api disable");
                        }
                        else
                        {
                            PSM_LOG_INFO("USB PON Suppression set to FALSE. Skipping..");
                        }

                        if(qmi_ind->reject_reason == DMS_PSM_REJECT_REASON_DURATION_TOO_SHORT_V01)
                        {
                            sys_m_shutdown();
                        }
                        else if(qmi_ind->reject_reason == DMS_PSM_REJECT_REASON_PSM_DURATION_CHANGED_V01)
                        {
                            sys_m_reset();
                        }
                    }

                    psm_core_check_if_ready_for_shutdown(updated_time_valid, updated_time,
                                                         modem_only_time_valid, modem_only_time );
                    break;

                case DMS_PSM_STATUS_REJECTED_V01:
                    PSM_LOG_INFO("psm_qmi_handle_dms_psm_status_ind: Modem rejected PSM mode");
                    /* Modem will send PSM READY again after REJECT. So revoke modem vote */
                        psm_list_reset_timeinfo(PSM_LIST_MODEM_CLIENT_FD);
                    psm_core_send_psm_status_to_allclients(PSM_STATUS_REJECT, reason);
                    break;

                case DMS_PSM_STATUS_OOS_V01:
                    PSM_LOG_INFO("psm_qmi_handle_dms_psm_status_ind: Network OOS");
                    psm_oos_status = PSM_STATUS_NW_OOS;
                    psm_core_send_psm_status_to_allclients(PSM_STATUS_NW_OOS, reason);
                    if( NIL == clients_count )
                    {
                        PSM_LOG_INFO("psm_qmi_handle_dms_psm_status_ind: No clients. Handle OOS");
                        psm_core_set_no_client_oos();
                    }

                    break;

                case DMS_PSM_STATUS_LIMITED_SERVICE_V01:
                    PSM_LOG_INFO("psm_qmi_handle_dms_psm_status_ind: Network Limited service");
                    psm_oos_status = PSM_STATUS_NW_LIMITED_SERVICE;
                    psm_core_send_psm_status_to_allclients(PSM_STATUS_NW_LIMITED_SERVICE, reason);
                    if( NIL == clients_count )
                    {
                        PSM_LOG_INFO("psm_qmi_handle_dms_psm_status_ind: No clients. Handle Limited Service");
                        psm_core_set_no_client_oos();
                    }

                    break;

                default:
                    PSM_LOG_INFO("psm_qmi_handle_dms_psm_status_ind: Invalid psm status");
                    rc = -1;
                    break;
            }
        }
    } while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_qmi_decode_indication

Decode QMI indications.

ARGUMENTS
    void

RETURN VALUE
    NA.
=========================================================================================*/
void psm_qmi_decode_indication
(
    qmi_ind_callback_type *qmi_data,
    int                    service_id,
    void                  **decoded_payload,
    uint32_t              *decoded_payload_len
)
{
    qmi_client_type             client_handle;
    qmi_client_error_type       qmi_err;
    qmi_idl_service_object_type srv_object;
    int                         process_indication = FALSE;

    PSM_LOG_FUNC_ENTRY("psm_qmi_decode_indication: function entry");

    do
    {

       if( !qmi_data || !decoded_payload )
       {
          PSM_LOG_INFO("psm_qmi_decode_indication: qmi_data or decoded_payload is NULL");
          break;
       }

       PSM_LOG_INFO1("psm_qmi_decode_indication: qmi indication - 0x%x", (int) qmi_data->msg_id);

       switch(service_id)
       {
            case PSM_QMI_CLIENT_DMS:
                client_handle = client_info.qmi_client_handles[PSM_QMI_CLIENT_DMS];
                srv_object    = dms_get_service_object_v01();
                process_indication = TRUE;
            break;

            default:
                process_indication = FALSE;
                PSM_LOG_VERBOSE1("psm_qmi_decode_indication: Unhandled service id: %d", service_id);
            break;
       }

       if(TRUE == process_indication)
       {
            qmi_err = qmi_idl_get_message_c_struct_len( srv_object,
                                                        QMI_IDL_INDICATION,
                                                        qmi_data->msg_id,
                                                        decoded_payload_len);
             if ( qmi_err != QMI_NO_ERR )
             {
                 PSM_LOG_INFO1("psm_qmi_decode_indication: Failed to process qmi message w/%d",
                                qmi_err);
                 break;
             }
         
             if( !(*decoded_payload_len) )
             {
             }
             else
             {
                 *decoded_payload = malloc( *decoded_payload_len );
                 if ( *decoded_payload == NULL )
                 {
                     PSM_LOG_INFO("Failed to alloc memory for decoded payload");
                     break;
                 }
                 memset( *decoded_payload, 0, *decoded_payload_len );
         
                 qmi_err = qmi_client_message_decode(
                             client_handle,
                             QMI_IDL_INDICATION,
                             qmi_data->msg_id,
                             qmi_data->data_buf,
                             qmi_data->data_buf_len,
                             *decoded_payload,
                             (int)(*decoded_payload_len) );
         
                 if (qmi_err != QMI_NO_ERR)
                 {
                      PSM_LOG_INFO1("psm_qmi_decode_indication: Failed to process qmi message w/%d",
                                    qmi_err);
                      free(*decoded_payload);
                      break;
                 }
             }
        }
    }while(FALSE);

    PSM_LOG_FUNC_RETURN("psm_qmi_decode_indication: function return");
}


/*========================================================================================
FUNCTION psm_qmi_handle_dms_indication

Handle all DMS indications.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
void psm_qmi_handle_dms_indication(qmi_ind_callback_type *qmi_data)
{
    void                 *decoded_payload = NULL;
    uint32_t              decoded_payload_len;

    PSM_LOG_FUNC_ENTRY("psm_qmi_handle_dms_indication: function entry");

    do
    {

        if(!qmi_data )
        {
            PSM_LOG_INFO("psm_qmi_handle_dms_indication: qmi_data is NULL");
            break;
        }

        PSM_LOG_INFO1("psm_qmi_handle_dms_indication: qmi indication - 0x%x", (int) qmi_data->msg_id);
        psm_qmi_decode_indication(qmi_data,PSM_QMI_CLIENT_DMS,&decoded_payload,&decoded_payload_len);

        switch(qmi_data->msg_id)
        {
            case QMI_DMS_PSM_STATUS_IND_V01:
                psm_qmi_handle_dms_psm_status_ind(decoded_payload);
                break;

            case QMI_DMS_EVENT_REPORT_IND_V01:
                psm_qmi_handle_dms_modem_status_ind(decoded_payload);
                break;

            case QMI_DMS_PSM_CFG_PARAMS_CHANGE_IND_V01:
                psm_qmi_handle_dms_psm_cfg_change_ind(decoded_payload);
                break;

            default:
                PSM_LOG_INFO1("Unhandled DMS indication - %d", qmi_data->msg_id);
                break;
        }

    }while(FALSE);

    if ( decoded_payload )
    {
        free(decoded_payload);
        decoded_payload = NULL;
    }

    PSM_LOG_FUNC_RETURN("psm_qmi_handle_dms_indication: function return");

}

/*========================================================================================
FUNCTION psm_qmi_read_modem_loading_flag

Read whether to start modem at bootup or not.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_read_modem_loading_flag(int *load_modem)
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
            *load_modem = TRUE;
            break;
        }

        efs_read(fd, buffer, 2);
        *load_modem = atoi(buffer);
        PSM_QMI_ULOG_MSG("Load modem flag in file: %d", *load_modem);
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}


/*========================================================================================
FUNCTION psm_modem_start_cb_fn

Load Modem.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
void psm_modem_start_cb_fn(void * cb_data, subsys_start_status_type start_status)
{
    int   rc = 0;
    if ( start_status == SUBSYS_START_SUCCESS )
    {
        modem_load_status = PSM_MODEM_LOADED;
        psm_core_send_psm_status_to_allclients(PSM_STATUS_MODEM_LOADED,PSM_REJECT_REASON_NONE);
        rc = psm_qmi_client_init();
        if ( rc != 0 )
        {
            PSM_LOG_INFO("psm_modem_start_cb_fn: Failed to initialize qmi clients - ");
        }
        else
        {
            PSM_LOG_INFO("psm_modem_start_cb_fn: loaded modem successfully");
        }

#ifdef FEATURE_PSM_TARGET_9205
    /* In 9205, Master is modem, so remove any outstanding fake votes on behalf of Modem 
       This will force PSMD to wait for READY from modem again in use case as below
       1. Device Started in AP-Only
       2. Modem is required at T=100
       3. Some client application is loading Modem at T=30
       4. Client app is voting for PSM at T=50
       In 9206, we can proceed for PSM by asking modem to Enter so it worked then.
       In 9205 we cannot.
    */
    psm_list_reset_timeinfo(PSM_LIST_MODEM_CLIENT_FD);
#endif

        if( TRUE == pending_shutdown)
        {
            rc = psm_core_set_psm_ready();
            if ( rc != 0 )
            {
                PSM_LOG_INFO("psm_modem_start_cb_fn: Failed to send Ready to enter PSM mode request to modem");
            }
        }
    }
}

/*========================================================================================
FUNCTION psm_qmi_broadcast_and_qmi_init

Broadcast Modem Loaded to all registered clients. Also trigger QMI Init

ARGUMENTS
    None

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_broadcast_and_qmi_init(void)
{
    int               rc = 0;

    PSM_LOG_FUNC_ENTRY("psm_qmi_broadcast_and_qmi_init: function entry");

    psm_core_send_psm_status_to_allclients(PSM_STATUS_MODEM_LOADED,PSM_REJECT_REASON_NONE);
    rc = psm_qmi_client_init();
    if ( rc != 0 )
    {
        PSM_LOG_INFO("psm_qmi_broadcast_and_qmi_init: Failed to initialize qmi clients - ");
        PSM_QMI_ULOG_MSG("Failed to initialize qmi clients - ");
    }
    else
    {
        PSM_LOG_INFO("psm_qmi_broadcast_and_qmi_init: Moving modem load status to Complete");
        modem_load_status = PSM_MODEM_LOADED;
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}
/*========================================================================================
FUNCTION psm_qmi_start_modem_subsystem

Shutdown Modem.

ARGUMENTS
    psm_boot_type_e for loading Modem post RBSp or Out of Reset

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_start_modem_subsystem(psm_boot_type_e boot_type)
{
    int               rc = 0;
    int               modem_started = TRUE;
    qurt_addr_t       v_addr;
    subsys_error_type err;
    subsys_start_options_type pil_option;

    PSM_LOG_FUNC_ENTRY("psm_qmi_start_modem_subsystem :function entry");

    rc = qurt_mem_map_static_query(&v_addr, WONCE_REG_ADDRESS, 4, QURT_MEM_CACHE_DEVICE_SHARED, QURT_PERM_READ_WRITE);
    if (rc == QURT_EOK)
    {
        if (v_addr)
        {
            modem_started = *((int*)v_addr);
        }

        if (modem_started == FALSE)
        {
            modem_load_status = PSM_MODEM_LOAD_IN_PROGRESS;
            subsys_start_options_init(pil_option);

            switch (boot_type)
            {
                case OUT_OF_RESET:
                    PSM_QMI_ULOG_MSG("Modem PIL Loading after Out of Reset");
                    pil_option.hibernate_boot = FALSE; //BOOT after reset
                    err = subsys_start(SUBSYS_MODEM_NAME,&pil_option,psm_modem_start_cb_fn, NULL);
                    break;

                case HIBERNATION_EXIT:
                    PSM_QMI_ULOG_MSG("Modem PIL Loading after Hibernation");
                    pil_option.hibernate_boot = TRUE; //BOOT after Hibernation
                    err = subsys_start(SUBSYS_MODEM_NAME,&pil_option,psm_modem_start_cb_fn, NULL);
                    break;

                case FOTA_BOOTUP:
                    PSM_QMI_ULOG_MSG("Skip Modem Loading. FOTA Boot");
                    err = SUBSYS_NOT_ALLOWED;
                    break;

                default:
                    /* We shouldn't be reaching here. All cases had been labelled */
                    PSM_QMI_ULOG_MSG("Unknown Boot type: %d", boot_type);
                    err = SUBSYS_NOT_ALLOWED;
                    break;
            }


            if ( err != SUBSYS_SUCCESS )
            {
                PSM_LOG_INFO1("psm_qmi_start_modem_subsystem: Failed to load modem - %d", err);
                rc = -1;
                modem_load_status = PSM_MODEM_NOT_LOADED;
            }
        }
        else
        {
            PSM_LOG_INFO("psm_qmi_start_modem_subsystem: Modem already loaded by SBL");
            rc = psm_qmi_broadcast_and_qmi_init();
        }
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*========================================================================================
FUNCTION psm_qmi_get_backoff_psmtime

Get backoff time.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_get_backoff_psmtime()
{
    int    time_in_secs = 0;
    if ( psm_qmi_cache.duration_due_to_oos_valid )
    {
        time_in_secs = psm_qmi_cache.duration_due_to_oos;
    }

    return time_in_secs;
}

/*========================================================================================
FUNCTION psm_qmi_get_bootup_delay_from_configuration

Get backoff time.

ARGUMENTS
    void

RETURN VALUE
    int - bootup_delay , -1 for failure.
=========================================================================================*/
int psm_qmi_get_bootup_delay_from_configuration(void)
{
    int    time_in_secs;
    if ( psm_qmi_cache.early_wakeup_time_valid )
    {
        time_in_secs = psm_qmi_cache.early_wakeup_time;
    }
    else
    {
        time_in_secs = -1;
    }

    return time_in_secs;
}

/*========================================================================================
FUNCTION psm_qmi_get_randomization_time

Get backoff time.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_get_randomization_time()
{
    uint32 time_in_secs = 0;
    int    random_time = 0;

    PSM_LOG_FUNC_ENTRY("psm_qmi_get_randomization_time: function entry");

    if ( psm_qmi_cache.randomization_window_valid &&
         psm_qmi_cache.randomization_window != 0 )
    {
        psm_rtc_get_time(&time_in_secs);
        random_time = (time_in_secs % psm_qmi_cache.randomization_window);
        if ( ( random_time != 0 ) && (random_time % 2 ) )
        {
            random_time = (-1)*random_time;
        }
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(random_time);
    return random_time;
}



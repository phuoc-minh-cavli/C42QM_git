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

#ifndef PSM_CORE_H
#define PSM_CORE_H

#include "psm_common.h"
#include "TlmmDefs.h"

#define PSM_DAEMON_START                     DAL_GPIO_HIGH_VALUE
#define PSM_DAEMON_END                       DAL_GPIO_LOW_VALUE
#define PSM_USB_PON_ENABLE                   1
#define PSM_USB_PON_DISABLE                  0

typedef enum
{
    PSM_EVT_FIRST,
    PSM_EVT_PSM_REGISTER,
    PSM_EVT_PSM_READY,
    PSM_EVT_PSM_NOT_READY,
    PSM_EVT_PSM_UNREGISTER,
    PSM_EVT_LOAD_MODEM,
    PSM_EVT_CLIENT_HEALTH_CHECK,
    PSM_EVT_SERVER_HEALTH_CHECK,
    PSM_EVT_QMI_IND,
    PSM_EVT_AUTO_READY,
    PSM_EVT_ACTIVE_TIMER_EXPIRY,
    PSM_EVT_CLIENT_BACKOFF,
    PSM_EVT_INACTIVITY_TIMER_EXPIRY,
    PSM_EVT_LAST = PSM_EVT_INACTIVITY_TIMER_EXPIRY
}psm_event_type_e;

typedef enum
{
    PSM_STATE_FIRST,
    PSM_STATE_NONE = PSM_STATE_FIRST,
    PSM_STATE_MODEM_READY_AP_AWAITED,
    PSM_STATE_MODEM_READY_AP_REJECTED,
    PSM_STATE_MODEM_READY_AP_READY,
    PSM_STATE_COMPLETE,
    PSM_STATE_LAST = PSM_STATE_COMPLETE,
} psm_state_type_e;

typedef enum
{
    PSM_MODEM_STATE_NONE,
    PSM_MODEM_NOT_LOADED,
    PSM_MODEM_LOAD_IN_PROGRESS,
    PSM_MODEM_LOADED,
    PSM_MODEM_STATE_MAX,
} psm_modem_load_status;


typedef struct
{
  int     event_id;
  int     client_fd;
  void   *data;
  size_t  datalen;
} psm_event_params_type;

/*========================================================================================
FUNCTION psm_core_set_psm_ready

Send PSM ready request to modem.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_set_psm_ready(void);

/*========================================================================================
FUNCTION psm_core_set_no_client_oos

Handler for OOS/Limited Service with no clients

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_set_no_client_oos(void);

/*========================================================================================
FUNCTION psm_core_get_psm_state

Query current PSM state.

ARGUMENTS
    none

RETURN VALUE
    int - PSM state.
=========================================================================================*/
psm_state_type_e psm_core_get_psm_state(void);

/*========================================================================================
FUNCTION psm_core_gpio_init

Init PSM GPIO configuration

ARGUMENTS
    void

RETURN VALUE
    void
=========================================================================================*/
void psm_core_gpio_init(void);

/*========================================================================================
FUNCTION psm_core_gpio_set

Set PSM GPIO to given DALGpioValue

ARGUMENTS
    DALGpioValueType  eValue

RETURN VALUE
    void
=========================================================================================*/
void psm_core_gpio_set(DALGpioValueType eValue);

/*========================================================================================
FUNCTION psm_core_set_psm_state

Set PSM state.

ARGUMENTS
    int - PSM state

RETURN VALUE
    void
=========================================================================================*/
void psm_core_set_psm_state(psm_state_type_e state);

/*========================================================================================
FUNCTION psm_core_send_status

Send status to clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_send_status(int fd, psm_status_type_e status, psm_reject_reason_type_e reason);

/*========================================================================================
FUNCTION psm_event_process

Entry function for PSM event thread.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
=========================================================================================*/
void psm_event_process(void *);

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
);

/*========================================================================================
FUNCTION psm_core_check_if_enter_psm_ready

Check if all apps ready to Enter PSM mode.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
=========================================================================================*/
int psm_core_check_if_enter_psm_ready(void);

/*========================================================================================
FUNCTION psm_core_inactivity_timer_expiry_cb

Handler for inactivity timer expiry

ARGUMENTS
    void

RETURN VALUE
    void
=========================================================================================*/

void psm_core_inactivity_timer_expiry_cb(void *timer_expiry_cb_data);

/*========================================================================================
FUNCTION psm_core_handle_inactivity_timer_expiry

Handle Inactivity Timer expiry request.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_inactivity_timer_expiry(psm_event_params_type *parms);

/*========================================================================================
FUNCTION psm_core_start_inactivity_timer

Start the PSM inactivity timer.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/

int psm_core_start_inactivity_timer(void);

/*========================================================================================
FUNCTION psm_core_stop_inactivity_timer

Stop the PSM inactivity timer.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/

int psm_core_stop_inactivity_timer(void);

/*========================================================================================
FUNCTION psm_core_process_event

Event thread entry function. Event thread is the only thread which does processing.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
=========================================================================================*/
int psm_core_process_event(psm_event_params_type *parms);

/*========================================================================================
FUNCTION psm_queue_event

Post request for event thread processing.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
=========================================================================================*/
int psm_queue_event(int event, int client_fd, void *data, size_t data_len);

/*========================================================================================
FUNCTION psm_core_send_psm_status_to_allclients

Sends psm status to all registered clients.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
=========================================================================================*/
int psm_core_send_psm_status_to_allclients
(
    psm_status_type_e        status,
    psm_reject_reason_type_e reason
);

/*========================================================================================
FUNCTION psm_core_send_psm_status

Sends psm status to a particular client.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
=========================================================================================*/
int psm_core_send_psm_status(int client_fd, psm_status_type_e status, psm_reject_reason_type_e reason);

/*==============================================================================
FUNCTION psm_core_adjust_psm_bootup_time

Adjust psm time to consider the bootup time delay

ARGUMENTS
    time in psm

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_core_adjust_psm_bootup_time(uint32 *time_in_psm);

/*==============================================================================
FUNCTION psm_core_adjust_psm_randomization_time

Adjust psm time to consider the bootup time delay

ARGUMENTS
    time in psm

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_core_adjust_psm_randomization_time(uint32 *time_in_psm);

/*========================================================================================
FUNCTION psm_core_handle_register_request

Handle register request.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
=========================================================================================*/
int psm_core_handle_register_request(psm_event_params_type *parms);

/*========================================================================================
FUNCTION psm_core_handle_unregister_request

Handle unregister request.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
=========================================================================================*/
int psm_core_handle_unregister_request(psm_event_params_type *parms);

/*========================================================================================
FUNCTION psm_core_handle_ready_request

Handle ready request.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
=========================================================================================*/
int psm_core_handle_ready_request(psm_event_params_type *parms);

/*========================================================================================
FUNCTION psm_core_handle_not_ready_request

Handle not ready request.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
=========================================================================================*/
int psm_core_handle_not_ready_request(psm_event_params_type *parms);

/*========================================================================================
FUNCTION psm_core_handle_load_modem_request

Handle request to load modem.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_load_modem_request(psm_event_params_type *parms);

/*========================================================================================
FUNCTION psm_core_handle_health_check_request

Handle health check from clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_health_check_request(psm_event_params_type *parms);

/*========================================================================================
FUNCTION psm_core_handle_client_health_check_request

Handle health check from clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_client_health_check_request(psm_event_params_type *parms);

/*========================================================================================
FUNCTION psm_core_handle_server_health_check_request

Handle health check.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_server_health_check_request(psm_event_params_type *parms);

/*========================================================================================
FUNCTION psm_qmi_handle_indication

Handle any qmi indications received from modem.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
=========================================================================================*/
int psm_qmi_handle_indication(psm_event_params_type *parms);

/*========================================================================================
FUNCTION psm_core_handle_auto_ready

Handle auto ready received from modem.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_auto_ready(psm_event_params_type *parms);

/*=======================================================================================
FUNCTION psm_core_read_config_file

Read server config file

ARGUMENTS
    void

RETURN VALUE
    void
=========================================================================================*/
void psm_core_read_config_file(void);

/*==============================================================================
FUNCTION psm_core_init

Initializes psm core

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_core_init(void);

/*========================================================================================
FUNCTION psm_core_server_init

PSM socket initialization function.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
=========================================================================================*/
int psm_core_server_init(void);

/*========================================================================================
FUNCTION psm_do_listen_client

Seperate thread function for monitoring clients and recieving client data.

ARGUMENTS
    data - data, not used

RETURN VALUE
    void
========================================================================================*/
void psm_do_listen_client(void);

/*========================================================================================
FUNCTION psm_core_process_request_from_client

Post requests received from clients to event thread.

ARGUMENTS
    none

RETURN VALUE
    0 on success, negative on failure.
========================================================================================*/
int psm_core_process_request_from_client(psm_req_msg_type *client_msg);

/*==============================================================================
FUNCTION psm_core_shutdown_device

Shutdown device.

ARGUMENTS
    psm_time Time requested to be in PSM

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_core_shutdown_device(int psm_time);

/*==============================================================================
FUNCTION psm_core_healthcheck_timer_cb

Server health check call back

ARGUMENTSs
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
void psm_core_healthcheck_timer_cb(void *cb_data);

/*==============================================================================
FUNCTION psm_core_handle_event_active_timer_expiry

Handle active timer expiry event

ARGUMENTSs
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_core_handle_event_active_timer_expiry(psm_event_params_type *parms);

/*========================================================================================
FUNCTION psm_core_handle_backoff_request

Handle ready request.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_core_handle_backoff_request(psm_event_params_type *parms);

#endif //PSM_CORE_H


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

#ifndef PSM_QMI_H
#define PSM_QMI_H

#include "qmi_client.h"
#include "device_management_service_v01.h"

#include "psm_core.h"

#define PSM_MODEM_FLAG_DIR           "/ril/psm"
#define PSM_MODEM_FLAG_FILE          "/ril/psm/modem_flag"

typedef enum {
    OUT_OF_RESET = 0,
    HIBERNATION_EXIT,
    FOTA_BOOTUP
}psm_boot_type_e;


typedef enum
{
    PSM_QMI_CLIENT_FIRST,
    PSM_QMI_CLIENT_DMS,
    PSM_QMI_CLIENT_MAX
}psm_qmi_client_info_type_e;

typedef struct
{
  qmi_client_type                user_handle;
  unsigned long                  msg_id;
  unsigned char                  *data_buf;
  int                            data_buf_len;
  void                           *cb_data;
} qmi_ind_callback_type;

typedef struct
{
  uint8_t periodic_update_timer_valid;
  uint32_t periodic_update_timer;
  dms_psm_status_enum_v01 psm_status;
} psm_qmi_ready_indication_payload;

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
);

/*==============================================================================
FUNCTION psm_qmi_dms_client_init

Initializes qmi dms client

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_dms_client_init(void);

/*==============================================================================
FUNCTION psm_qmi_client_init

Initializes qmi clients

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_client_init(void);

/*==============================================================================
FUNCTION psm_qmi_init

Load modem, initialize qmi clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_init(void);

/*========================================================================================
FUNCTION psm_qmi_client_release

Releases qmi clients handles.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_client_release(void);

/*==============================================================================
FUNCTION psm_qmi_get_threshold_time

Get PSM threshold value.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_get_threshold_time(int *threshold_time);

/*==============================================================================
FUNCTION psm_qmi_fetch_psm_configuration

Fetch PSM configuration information from modem.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_fetch_psm_configuration(void);

/*==============================================================================
FUNCTION psm_qmi_modem_timer_expiry_cb

Callback to handle auto modem bring up in case of tau timer expiry

ARGUMENTS
    void

RETURN VALUE
    void
==============================================================================*/
void psm_qmi_modem_timer_expiry_cb(void* timer_expiry_data);

/*==============================================================================
FUNCTION psm_list_update_auto_modem_start

Start timer to auto power on modem if tau timer expires

ARGUMENTS
    tau time

RETURN VALUE
    void
==============================================================================*/
void psm_qmi_auto_start_modem(uint32  tau_time);

/*==============================================================================
FUNCTION psm_qmi_fetch_modem_operational_status

Fetch Modem operating status.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_fetch_modem_operational_status(void);

/*==============================================================================
FUNCTION psm_qmi_is_psm_supported

Check if modem supports PSM feature.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_is_psm_supported(void);

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
int psm_qmi_is_modem_controlled_psm(int *modem_controlled);

/*==============================================================================
FUNCTION psm_qmi_set_psm_active_time

Set PSM active time.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_set_psm_active_time(int time_in_secs);

/*==============================================================================
FUNCTION psm_qmi_set_psm_ready

Inform modem that APPS is ready to enter PSM mode.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_set_psm_ready(int time_in_secs, int accept);

/*========================================================================================
FUNCTION psm_qmi_handle_dms_modem_status_ind

Handle Modem status change indication.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_handle_dms_modem_status_ind(dms_event_report_ind_msg_v01 *qmi_ind);

/*========================================================================================
FUNCTION psm_qmi_broadcast_and_qmi_init

Broadcast Modem Loaded to all registered clients. Also trigger QMI Init

ARGUMENTS
    None

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_broadcast_and_qmi_init(void);

/*========================================================================================
FUNCTION psm_qmi_start_modem

Shutdown Modem.

ARGUMENTS
    psm_boot_type_e for loading Modem post RBSp or Out of Reset

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_start_modem_subsystem(psm_boot_type_e boot_type);

/*========================================================================================
FUNCTION psm_qmi_handle_dms_psm_status_ind

Handle PSM status indication from modem.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_handle_dms_psm_status_ind(dms_psm_status_ind_msg_v01 *qmi_ind);

/*========================================================================================
FUNCTION psm_qmi_decode_indication

Decode QMI indications.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
void psm_qmi_decode_indication
(
    qmi_ind_callback_type *qmi_data,
    int                    service_id,
    void                  **decoded_payload,
    uint32_t              *decoded_payload_len
);

/*========================================================================================
FUNCTION psm_qmi_handle_dms_indication

Handle all DMS indications.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
void psm_qmi_handle_dms_indication(qmi_ind_callback_type *qmi_data);

/*========================================================================================
FUNCTION psm_qmi_handle_indication

Handle any qmi indications received from modem.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_handle_indication(psm_event_params_type *parms);

/*==============================================================================
FUNCTION psm_qmi_update_periodic_timer_info

Update Periodic timer information in cache.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_update_periodic_timer_info(int periodic_tau);

/*========================================================================================
FUNCTION psm_qmi_read_modem_loading_flag

Read whether to start modem at bootup or not.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_read_modem_loading_flag(int *load_modem);

/*==============================================================================
FUNCTION psm_qmi_modify_modem_load_based_on_boot

Load modem, initialize qmi clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_modify_modem_load_based_on_boot(int* load_modem);

/*========================================================================================
FUNCTION psm_qmi_get_backoff_psmtime

Get backoff time.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_get_backoff_psmtime(void);

/*========================================================================================
FUNCTION psm_qmi_get_bootup_delay_from_configuration

Get backoff time.

ARGUMENTS
    void

RETURN VALUE
    int - bootup_delay , -1 for failure.
=========================================================================================*/
int psm_qmi_get_bootup_delay_from_configuration(void);

/*========================================================================================
FUNCTION psm_qmi_get_randomization_time

Get backoff time.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
int psm_qmi_get_randomization_time(void);

/*========================================================================================
FUNCTION psm_qmi_get_psm_oos_status

Get network status.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure.
=========================================================================================*/
psm_status_type_e psm_qmi_get_psm_oos_status(void);

/*==============================================================================
FUNCTION psm_qmi_get_boot_type

Get if boot type is Hibernation exit or PSM exit

ARGUMENTS
    type - Address of enum to assign

RETURN VALUE
    int - 0 for success, -1 for failure.
==============================================================================*/
int psm_qmi_get_boot_type(psm_boot_type_e* type);


#endif // PSM_QMI_H


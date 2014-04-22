/******************************************************************************
#@file    psm_list.h
#@brief   Book keeping of client connections.
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#ifndef PSM_CLIENT_LIST_H
#define PSM_CLIENT_LIST_H


#define PSM_LIST_MODEM_CLIENT_FD   (-1)
#define PSM_LIST_OOS_CLIENT_FD     (-2)
#define PSM_LIST_SPECIAL_TAU_TIME  (0)

/*==============================================================================
FUNCTION psm_list_add_newclient

Add new client

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_add_newclient(int   client_fd);

/*==============================================================================
FUNCTION psm_list_update_auto_ready_info

Add a node in list for auto ready

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_add_auto_ready_info(uint32 time_in_psm);

/*==============================================================================
FUNCTION psm_list_add_oos_info

Add a node in list for OOS info. Used with inactivity timer

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_add_oos_info(int oos_time, int nw_access);

/*==============================================================================
FUNCTION psm_list_update_info

Update existing client node with time info

ARGUMENTS
        void

RETURN VALUE
        int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_update_info
(
    int client_fd, int active_time, int start_time_in_psm, int end_time_in_psm, int nw_access
);

/*==============================================================================
FUNCTION psm_list_remove_client

Remove client from list.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_remove_client(int client_fd);

/*==============================================================================
FUNCTION psm_list_reset_all_clients_timeinfo

Reset active, psm time of all clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_reset_all_clients_timeinfo(void);

/*==============================================================================
FUNCTION psm_list_get_number_of_clients

Get total no. of clients registered.

ARGUMENTS
    void

RETURN VALUE
    int - Number of clients registered
==============================================================================*/
int psm_list_get_number_of_clients(int all_clients);

/*==============================================================================
FUNCTION psm_list_make_interval_group

Make structure of psm_time_interval_group_type from list.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_make_interval_group(psm_time_interval_group_type *interval_group);

/*==============================================================================
FUNCTION psm_list_get_psm_time

Get psm time.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_get_psm_time(uint32 *time_in_psm_mode, int *nw_access);

/*==============================================================================
FUNCTION psm_list_check_all_clients_ready

Check if all clients are ready to enter psm mode.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_check_all_clients_ready(void);

/*==============================================================================
FUNCTION psm_list_check_client_voted

Check if the client has already voted to enter PSM mode.

ARGUMENTS
    int - clientfd

RETURN VALUE
    int - 1 for already voted with valid time, 0 else.
==============================================================================*/
int psm_list_check_client_voted(int client_fd);

/*==============================================================================
FUNCTION psm_list_healthcheck_failure

Check if all clients health check ack received or not.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_handle_healthcheck(void);

/*==============================================================================
FUNCTION psm_list_client_healthcheck

Handle client health check ack.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_client_healthcheck(int client_fd);

/*==============================================================================
FUNCTION psm_list_reset_healthcheck_flag

Reset all clients health check flag.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_reset_healthcheck_flag(void);

/*==============================================================================
FUNCTION psm_list_reset_timeinfo

Reset active, psm time.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_reset_timeinfo(int client_fd);

/*==============================================================================
FUNCTION psm_list_send_psmstatus_to_allclients

Send psm status to all registered clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_send_psmstatus_to_allclients
    (
        psm_status_type_e status,
        psm_reject_reason_type_e reason
    );

/*==============================================================================
FUNCTION psm_list_get_max_active_time

Get max active time.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
    int psm_list_get_max_active_time(uint32 *active_time);

/*==============================================================================
FUNCTION psm_list_healthcheck_failure

Handle client health check failure.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_healthcheck_failure(int client_fd);

#endif // PSM_CLIENT_LIST_H



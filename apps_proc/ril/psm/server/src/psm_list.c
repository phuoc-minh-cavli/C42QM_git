/******************************************************************************
#@file    psm_list.c
#@brief   Book keeping of client connections.
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

#include "psm_core.h"
#include "psm_qmi.h"
#include "psm_rtc.h"
#include "psm_common.h"
#include "psm_list.h"
extern int psm_pull_duration_time;
typedef struct psm_core_client_node_type
{
    int    client_fd;
    int    nw_access;
    uint32 active_timer;
    uint32 start_psm_alarm_time;
    uint32 end_psm_alarm_time;
    uint32 adj_psm_start_alarm_time;
    uint32 adj_psm_end_alarm_time;
    uint32 ready_req_arrival_time;
    int    dirty;
    struct psm_core_client_node_type *next;
} psm_core_client_node_type;
    
    

psm_core_client_node_type   *list_head = NULL;

/*==============================================================================
FUNCTION psm_list_add_newclient

Add new client to list

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_add_newclient(int  client_fd)
{
    int                               rc = -1;
    struct psm_core_client_node_type *itr_node = NULL;
    struct psm_core_client_node_type *newclient_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_add_newclient: function entry");

    do
    {
        itr_node = list_head;
        while(itr_node != NULL)
        {
            if ( itr_node->client_fd == client_fd )
            {
                PSM_LOG_INFO1("client with client_fd - %d already exist", client_fd);
                break;
            }
            itr_node = itr_node->next;
        }

        // new client, add to list
        if ( itr_node == NULL )
        {
            newclient_node = malloc(sizeof(*newclient_node));

            if ( newclient_node == NULL )
            {
                PSM_LOG_INFO("failed to allocate memory for newclient node");
                break;
            }
            memset(newclient_node, 0, sizeof(*newclient_node));
            newclient_node->client_fd = client_fd;
            newclient_node->dirty = FALSE;

            if (list_head == NULL)
            {
                list_head = newclient_node;
            }
            else
            {
                newclient_node->next = list_head;
                list_head = newclient_node;
            }
        }

        rc = 0;

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_list_update_auto_ready_info

Add a node in list for auto ready

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_add_auto_ready_info(uint32 time_in_psm)
{
    int                        rc = SUCCESS;
    psm_core_client_node_type *itr_node = NULL;
    psm_core_client_node_type *newclient_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_add_auto_ready_info: function entry");
    PSM_LOG_INFO1("psm_list_add_auto_ready_info: Time in PSM recieved in indication is: %d",
                  time_in_psm);

    if( PSM_LIST_SPECIAL_TAU_TIME == time_in_psm )
    {
        PSM_LOG_INFO1("Zero PSM Time, Special case. Skip adding in list %d", time_in_psm);
        //Remove Modem client if already existing in list
        psm_list_remove_client(PSM_LIST_MODEM_CLIENT_FD);
        return rc;
    }

    do
    {

        itr_node = list_head;
        while(itr_node != NULL)
        {
            if ( itr_node->client_fd == -1 )
            {
                PSM_LOG_INFO("psm_list_add_auto_ready_info: Auto ready entry already available");
                break;
            }
            itr_node = itr_node->next;
        }
       if ( itr_node != NULL )
        {
            itr_node->start_psm_alarm_time = time_in_psm;
            itr_node->end_psm_alarm_time   = time_in_psm;
            itr_node->nw_access            = TRUE;
			psm_rtc_get_time(&itr_node->ready_req_arrival_time);
	    }
        else
        {
            newclient_node = malloc(sizeof(*newclient_node));

            if ( newclient_node == NULL )
            {
                PSM_LOG_INFO("failed to allocate memory for newclient node");
                break;
            }
            memset(newclient_node, 0, sizeof(*newclient_node));
            newclient_node->client_fd = -1;
            newclient_node->dirty = FALSE;

            newclient_node->nw_access          = TRUE;
            newclient_node->active_timer       = 0;
            newclient_node->start_psm_alarm_time     = time_in_psm;
            newclient_node->end_psm_alarm_time       = time_in_psm;
            psm_rtc_get_time(&newclient_node->ready_req_arrival_time);

            if (list_head == NULL)
            {
                list_head = newclient_node;
            }
            else
            {
                newclient_node->next = list_head;
                list_head = newclient_node;
            }
        }
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_list_add_oos_info

Add a node in list for OOS info. Used with inactivity timer

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_add_oos_info(int oos_time, int nw_access)
{
    int                        rc = SUCCESS;
    psm_core_client_node_type *itr_node = NULL;
    psm_core_client_node_type *newclient_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_add_oos_info: function entry");

    do
    {

        itr_node = list_head;
        while(itr_node != NULL)
        {
            if ( itr_node->client_fd == PSM_LIST_OOS_CLIENT_FD )
            {
                PSM_LOG_INFO("OOS information already available. Updating");
                break;
            }
            itr_node = itr_node->next;
        }

        if ( itr_node != NULL )
        {
            itr_node->start_psm_alarm_time = oos_time;
            itr_node->end_psm_alarm_time   = oos_time;
            itr_node->nw_access            = nw_access;
            psm_rtc_get_time(&itr_node->ready_req_arrival_time);
        }
        else
        {
            newclient_node = malloc(sizeof(*newclient_node));

            if ( newclient_node == NULL )
            {
                PSM_LOG_INFO("failed to allocate memory for OOS node");
                break;
            }
            memset(newclient_node, 0, sizeof(*newclient_node));
            newclient_node->client_fd = PSM_LIST_OOS_CLIENT_FD;
            newclient_node->dirty = FALSE;
            newclient_node->nw_access             = nw_access;
            newclient_node->active_timer          = 0;
			newclient_node->start_psm_alarm_time  = oos_time;
            newclient_node->end_psm_alarm_time    = oos_time;

            psm_rtc_get_time(&newclient_node->ready_req_arrival_time);

            if (list_head == NULL)
            {
                list_head = newclient_node;
            }
            else
            {
                newclient_node->next = list_head;
                list_head = newclient_node;
            }
        }
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

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
)
{
    int                        rc = FAIL;
    psm_core_client_node_type *itr_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_update_info: function entry");

    do
    {
        itr_node = list_head;

        while(itr_node != NULL)
        {
            if ( itr_node->client_fd == client_fd )
            {
                PSM_LOG_INFO1("psm_list_update_info: client with client_fd found", client_fd);
                rc = SUCCESS;
                break;
            }
            itr_node = itr_node->next;
        }

        // Update node
        if ( itr_node != NULL )
        {
            itr_node->nw_access              = nw_access;
            itr_node->active_timer           = active_time;
            itr_node->start_psm_alarm_time   = start_time_in_psm;
            itr_node->end_psm_alarm_time     = end_time_in_psm;

            psm_rtc_get_time(&itr_node->ready_req_arrival_time);
            PSM_LOG_INFO4(
            "psm_list_update_info: Updated active time %d, start_psm time %d end_psm_time %d arrival time %d",
            active_time, start_time_in_psm, end_time_in_psm, itr_node->ready_req_arrival_time);
		}

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_list_reset_all_clients_timeinfo

Reset active, psm time of all clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_reset_all_clients_timeinfo(void)
{
    int                               rc = SUCCESS;
    psm_core_client_node_type        *itr_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_reset_all_clients_timeinfo: function entry");

    do
    {
        itr_node = list_head;

        while(itr_node != NULL)
        {
            itr_node->nw_access               = 0;
            itr_node->active_timer            = 0;
            itr_node->start_psm_alarm_time    = 0;
            itr_node->end_psm_alarm_time      = 0;
            itr_node->ready_req_arrival_time  = 0;
            itr_node = itr_node->next;
        }
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}


/*==============================================================================
FUNCTION psm_list_reset_timeinfo

Reset active, psm time.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_reset_timeinfo(int client_fd)
{
    int                               rc = SUCCESS;
    psm_core_client_node_type        *itr_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_reset_timeinfo: function entry");

    do
    {
        itr_node = list_head;

        while(itr_node != NULL)
        {
            if ( itr_node->client_fd == client_fd )
            {
                PSM_LOG_INFO1("psm_list_reset_timeinfo: client with client_fd found", client_fd);
                break;
            }
            itr_node = itr_node->next;
        }

        // Update node
        if ( itr_node != NULL )
        {
            itr_node->nw_access               = 0;
            itr_node->active_timer            = 0;
            itr_node->start_psm_alarm_time    = 0;
            itr_node->end_psm_alarm_time      = 0;
            itr_node->ready_req_arrival_time  = 0;
        }

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_list_adjust_psm_time

Adjust psm time

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_adjust_psm_time()
{
    int    rc = 0;
    uint32 curr_time_in_secs;
    uint32 delta_psm_time = 0;
    //uint32 adj_psm_time = 0;
    uint32 adj_start_psm_time = 0;
    uint32 adj_end_psm_time = 0;
    psm_core_client_node_type *itr_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_adjust_psm_time: function entry");

    do
    {
        itr_node = list_head;

        if ( list_head == NULL )
        {
            rc = -1;
            break;
        }

        rc = psm_rtc_get_time(&curr_time_in_secs);

        while(itr_node != NULL)
        {
             if ( itr_node->start_psm_alarm_time > 0 && itr_node->end_psm_alarm_time > 0)
            {
                delta_psm_time = curr_time_in_secs - itr_node->ready_req_arrival_time;
                if(itr_node->start_psm_alarm_time > delta_psm_time && 
                    itr_node->end_psm_alarm_time > delta_psm_time)                
                {
                    /* Adjust the time only if PSM Time has not expired */
                    adj_start_psm_time = itr_node->start_psm_alarm_time - delta_psm_time;
                    adj_end_psm_time = itr_node->end_psm_alarm_time - delta_psm_time;
                }

                if ( adj_start_psm_time > 0 && adj_end_psm_time > 0)
                {
                     itr_node->adj_psm_start_alarm_time = adj_start_psm_time;
                    itr_node->adj_psm_end_alarm_time = adj_end_psm_time;
                   
                }
                else
                {
                    //itr_node->psm_alarm_time         = 0;
                    itr_node->start_psm_alarm_time     = 0;
                    itr_node->end_psm_alarm_time       = 0;
                    itr_node->adj_psm_start_alarm_time = 0;
                    itr_node->adj_psm_end_alarm_time   = 0;
                    itr_node->active_timer             = 0;
                    itr_node->ready_req_arrival_time   = 0;
                }
            }
            itr_node = itr_node->next;
        }
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}


/*==============================================================================
FUNCTION psm_list_get_number_of_clients

Get total no. of clients registered.

ARGUMENTS
    void

RETURN VALUE
    int - Number of clients registered
==============================================================================*/
int psm_list_get_number_of_clients(int all_clients)
{
    int                               clients_count = 0;
    struct psm_core_client_node_type *itr_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_get_number_of_clients: function entry");

    do
    {
        itr_node = list_head;

        while(itr_node)
        {
            if ( all_clients || itr_node->client_fd >= 0 )
            {
                clients_count++;
            }
            itr_node = itr_node->next;
        }

        PSM_LOG_INFO1("psm_list_get_number_of_clients: Count of clients - %d", clients_count);
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(clients_count);
    return clients_count;
}

/*==============================================================================
FUNCTION psm_list_make_interval_group

Utility function     to get   a structure of type psm_time_interval_group_type from the list.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_make_interval_group(psm_time_interval_group_type *interval_group)
{
    int                               res             = 0;
    int                               i               = 0;                                 
    struct psm_core_client_node_type  *itr_node       = NULL;
    int                               num_clients     = 0;
    psm_time_interval_type            *list_intervals = NULL;
    
    PSM_LOG_FUNC_ENTRY("psm_list_make_interval_group: function entry");
    do
    {
        num_clients = psm_list_get_number_of_clients(TRUE);
        if(!num_clients)
        {
            PSM_LOG_ERROR("psm_list_make_interval_group:psm_list_make_interval_group:No clients present.. Exit");
            res = -1;
            break;
        }

        list_intervals=(psm_time_interval_type *)malloc(sizeof(psm_time_interval_type)*num_clients);
        if(!list_intervals)
        {
            PSM_LOG_ERROR("psm_list_make_interval_group: Failed to allocate memory for list_intervals array");
            res = -1;
            break;
        }
        memset(list_intervals, 0, sizeof(list_intervals));
        interval_group->number_of_intervals = num_clients;

        itr_node=list_head;
        for(i = 0; i < num_clients; i++)
        {
            list_intervals[i].start.psm_duration_in_secs = itr_node->adj_psm_start_alarm_time;
            list_intervals[i].end.psm_duration_in_secs = itr_node->adj_psm_end_alarm_time;
            list_intervals[i].start.time_format_flag = PSM_TIME_IN_SECS;
            list_intervals[i].end.time_format_flag = PSM_TIME_IN_SECS;
            PSM_LOG_INFO2("psm_list_make_interval_group: Client: %d, NW-Access: %d",
                          itr_node->client_fd, itr_node->nw_access);
            if( itr_node->nw_access == 0 )
            {
                list_intervals[i].psm_wakeup_type = PSM_WAKEUP_MEASUREMENT_ONLY ;
            }
            else
            {
                list_intervals[i].psm_wakeup_type = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
            }

            itr_node=itr_node->next;
        }
        interval_group->intervals = list_intervals;
    }while (FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(res);
    return res;
}
/*==============================================================================
FUNCTION psm_list_get_psm_time

Get psm time.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_get_psm_time(uint32 *time_in_psm_mode, int *nw_access)
{
    int                               rc                     = -1;
    int                               i                      = 0;
    int                               temp_nw_access         = 0;
    uint32                            least_psm_time         = 0;
    uint32                            curr_time_in_secs      = 0;
    psm_time_interval_type            *optimal_interval      = NULL;
    psm_time_interval_type            *next_wakeup_interval  = NULL;
    psm_time_interval_group_type      *interval_group        = NULL;
    psm_error_type_e                  result                 = PSM_ERR_NONE;

    PSM_LOG_FUNC_ENTRY("psm_list_get_psm_time: function entry");

    do
    {

        psm_list_adjust_psm_time();
        if ( list_head == NULL )
        {
            break;
        }

        interval_group = (psm_time_interval_group_type *)malloc(sizeof(psm_time_interval_group_type));
        optimal_interval = (psm_time_interval_type*)malloc(sizeof(psm_time_interval_type));
        if (!interval_group || !optimal_interval)
        {
            PSM_LOG_ERROR("psm_list_get_psm_time:Failed to allocate memory for interval structs");
            rc = -1;
            break;
        }
        memset(interval_group,0,sizeof(interval_group));
        memset(optimal_interval,0,sizeof(optimal_interval));

        rc = psm_list_make_interval_group(interval_group);
        if(rc || !interval_group->intervals)
        {
            PSM_LOG_ERROR1("psm_list_get_psm_time: PSM client get optimal interval return value %d",
                            result);
            break;
        }

        /*client id not used, passing 0*/
        result= psm_client_optimal_interval(0,interval_group,optimal_interval, FALSE);
        PSM_LOG_INFO1("psm_list_get_psm_time: PSM client get optimal interval return value %d", result);

        temp_nw_access = optimal_interval->psm_wakeup_type == PSM_WAKEUP_MEASUREMENT_ONLY ? 0 : 1 ;

        /*Find next wakeup interval */
        for(i=0; i<interval_group->number_of_intervals;i++)
        {
            if(interval_group->intervals[i].start.psm_duration_in_secs >
                        optimal_interval->start.psm_duration_in_secs)
            {
                next_wakeup_interval = &interval_group->intervals[i];
                break;
            }
        }

        if(next_wakeup_interval)
        {
            PSM_LOG_INFO2("psm_list_get_psm_time: Next wakeup interval start %u end %u",
            next_wakeup_interval->start.psm_duration_in_secs,
            next_wakeup_interval->end.psm_duration_in_secs);
        }
        else
        {
            PSM_LOG_INFO("psm_list_get_psm_time: No next wakeup interval");
        }

        /* Calculate PSM Duration */
        if ((next_wakeup_interval != NULL) && 
            ((next_wakeup_interval->start.psm_duration_in_secs - optimal_interval->end.psm_duration_in_secs) <= 
             psm_pull_duration_time ) )
        {
            least_psm_time = optimal_interval->end.psm_duration_in_secs;
        }
        else
        {
            least_psm_time = optimal_interval->start.psm_duration_in_secs;
        }
        PSM_LOG_INFO1("psm_list_get_psm_time: least psm time before bootup adjustment- %u", least_psm_time);

        rc = psm_rtc_get_time(&curr_time_in_secs);

        if ( rc != 0 )
        {
            PSM_LOG_ERROR("psm_list_get_psm_time: Failed to retrieve RTC time");
            break;
        }

        if ( least_psm_time > 0 )
        {
            if ( time_in_psm_mode != NULL )
            {
                *time_in_psm_mode = least_psm_time;
            }
            if ( nw_access != NULL )
            {
                *nw_access = temp_nw_access;
            }
        }
        else
        {
            rc = -1;
            PSM_LOG_INFO("psm_list_get_psm_time: Adjusted psm time is negative");
        }

        if(!optimal_interval)
        {
            free(optimal_interval);
        }
        if(!interval_group)
        {
            free(interval_group->intervals);
            free(interval_group);
        }
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_list_get_max_active_time

Get max active time.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_get_max_active_time(uint32 *active_time)
{
    int                               rc = 0;
    int                               temp_active_time = 0;
    struct psm_core_client_node_type *itr_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_get_max_active_time: function entry");

    do
    {
        itr_node = list_head;

        if ( active_time == NULL || list_head == NULL )
        {
            rc = -1;
            break;
        }

        temp_active_time = itr_node->active_timer;

        while(itr_node != NULL)
        {
            if ( itr_node->active_timer > 0 )
            {
                if ( temp_active_time < itr_node->active_timer )
                {
                    temp_active_time = itr_node->active_timer;
                }
            }
            itr_node = itr_node->next;
        }

        *active_time = temp_active_time;

        PSM_LOG_INFO1("psm_list_get_max_active_time: max active time - %d", temp_active_time);
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_list_check_all_clients_ready

Check if all clients are ready to enter PSM mode.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_check_all_clients_ready(void)
{
    int                               rc = 0;
    struct psm_core_client_node_type *itr_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_check_all_clients_ready: function entry");

    do
    {
        if( list_head == NULL )
        {
            PSM_LOG_INFO("No clients registered... No need to enter PSM mode");
            rc = -1;
            break;
        }

        psm_list_adjust_psm_time();

        itr_node = list_head;
        while(itr_node != NULL)
        {
            if ( itr_node->start_psm_alarm_time <= 0 || itr_node->end_psm_alarm_time <=0 )  
            {
                rc = -1;
                PSM_LOG_INFO1("psm_list_check_all_clients_ready: client_fd - %d is not yet ready to enter PSM mode",
                               itr_node->client_fd);
                break;
            }
            itr_node = itr_node->next;
        }

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_list_check_client_voted

Check if the client has already voted to enter PSM mode.

ARGUMENTS
    int - clientfd

RETURN VALUE
    int - 1 for already voted with valid time, 0 else.
==============================================================================*/
int psm_list_check_client_voted(int client_fd)
{
    int                               voted = FALSE;
    struct psm_core_client_node_type *itr_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_check_client_voted: function entry");

    do
    {
        if(list_head == NULL)
        {
            PSM_LOG_INFO("No clients registered... Client did not vote previously");
            voted = FALSE;
            break;
        }

        psm_list_adjust_psm_time();

        itr_node = list_head;
        while(itr_node != NULL)
        {
            if ((itr_node->client_fd == client_fd) && (itr_node->ready_req_arrival_time > 0))
            {
                voted = TRUE;
                PSM_LOG_INFO2("psm_list_check_client_voted: client_fd - %d already voted valid PSM at %d",
                            itr_node->client_fd, itr_node->ready_req_arrival_time);
                break;
            }
            itr_node = itr_node->next;
        }
    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(voted);
    return voted;
}

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
)
{
    int                               rc = 0;
    struct psm_core_client_node_type *itr_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_send_psmstatus_to_allclients: function entry");

    do
    {
        if( list_head == NULL )
        {
            PSM_LOG_INFO("No clients registered... No need to send psm status");
            break;
        }

        itr_node = list_head;
        while( itr_node != NULL )
        {
            if( itr_node->client_fd >= 0 )
            {
                rc = psm_core_send_psm_status( itr_node->client_fd, status, reason);
                if ( rc != 0 )
                {
                    PSM_LOG_INFO1("Failed to update status to client with fd - %d", itr_node->client_fd);
                }
            }
            itr_node = itr_node->next;
        }

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_list_remove_client

Remove client from list.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_remove_client(int client_fd)
{
    int                        rc = FAIL;
    psm_core_client_node_type *client = NULL;
    psm_core_client_node_type *prev_client = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_remove_client: function entry");

    for (client = list_head; client != NULL; client = client->next)
    {
        if (client->client_fd != client_fd)
        {
            prev_client = client;
            continue;
        }
        if (client == list_head)
        {
            list_head = list_head->next;
            rc = SUCCESS;
            break;
        }
        else
        {
            prev_client->next = client->next;
            rc = SUCCESS;
            break;
        }
    }
    if(NULL != client)
    {
        free(client);
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_list_healthcheck_failure

Handle client health check failure.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_healthcheck_failure(int client_fd)
{
    int     rc = 0;
    int     time_in_psm = 0;

    PSM_LOG_FUNC_ENTRY("psm_list_healthcheck_failure: function entry");

    do
    {
        time_in_psm = psm_qmi_get_backoff_psmtime();
        rc = psm_list_update_info(client_fd, 0, time_in_psm, time_in_psm, 1);
        if ( rc != 0 )
        {
            PSM_LOG_INFO1("psm_list_healthcheck_failure: Failed to find client with fd - %d", client_fd);
            break;
        }

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_list_handle_healthcheck

Check if health check ack received from all clients.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_handle_healthcheck()
{
    int                               rc = 0;
    struct psm_core_client_node_type *itr_node = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_handle_healthcheck: function entry");

    do
    {
        if( list_head == NULL )
        {
            PSM_LOG_INFO("psm_list_handle_healthcheck: No clients registered...");
            break;
        }

        itr_node = list_head;
        while(itr_node != NULL)
        {
            // client_fd check is necessary to avoid any issues due to auto ready node in list.
            if ( ( itr_node->dirty == TRUE ) && ( itr_node->client_fd >= 0 ) )
            {
                PSM_LOG_INFO1("psm_list_handle_healthcheck:: client_fd - %d health check failed",
                              itr_node->client_fd);
                psm_list_healthcheck_failure(itr_node->client_fd);
                rc = -1;
            }
            itr_node = itr_node->next;
        }

    }while(FALSE);

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}


/*==============================================================================
FUNCTION psm_list_client_healthcheck

Handle client health check ack.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_client_healthcheck(int client_fd)
{
    int                        rc = 0;
    psm_core_client_node_type *client = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_client_healthcheck: function entry");

    for (client = list_head; client != NULL; client = client->next)
    {
        if (client->client_fd != client_fd)
        {
            continue;
        }
        client->dirty = FALSE;
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}

/*==============================================================================
FUNCTION psm_list_reset_healthcheck_flag

Reset all clients health check flag.

ARGUMENTS
    void

RETURN VALUE
    int - 0 for success, -1 for failure
==============================================================================*/
int psm_list_reset_healthcheck_flag(void)
{
    int                        rc = 0;
    psm_core_client_node_type *client = NULL;

    PSM_LOG_FUNC_ENTRY("psm_list_reset_healthcheck_flag: function entry");

    for (client = list_head; client != NULL; client = client->next)
    {
        // Apps which already sent PSM READY don't need to send health check request.
        if ( client->start_psm_alarm_time == 0 && client->end_psm_alarm_time == 0 ) 
        {
            client->dirty = TRUE;
        }
    }

    PSM_LOG_FUNC_RETURN_WITH_RET(rc);
    return rc;
}



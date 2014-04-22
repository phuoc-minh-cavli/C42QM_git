/*
 * Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*!
  @file
  tx_cpu_profile.c

  @brief
  Implementation of ThreadX profile command handler functionality.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include "qurt_types.h"
#include "qcli.h"
#include "qcli_api.h"
#include "qapi.h"
#include "qapi_netservices.h"
#include "qurt_thread.h"
#include "timer.h"
#include "qapi_ns_utils.h"
#include "time_svc.h"
#include "stringl/stringl.h"
#include "tx_api.h"

#define MAX_TH_SIZE 20
#define TH_NAME_SIZE 30

typedef struct _apps_tx_thread_list
{
    char *thread_name;
    bool is_profile_needed;
    qurt_thread_t module_thread_id;
    double cpu_per_val;
}apps_tx_thread_list;

apps_tx_thread_list tx_thread_list[MAX_TH_SIZE];

extern int server_quit;
extern int test_for_quit_s(void);
extern QCLI_Group_Handle_t qcli_net_handle; /* Handle for Net Command Group. */
#define PROFILE_PRINTF(...) QCLI_Printf(qcli_net_handle, __VA_ARGS__)

#ifdef MQTT_ENABLE
extern qurt_thread_t get_mqtt_thread_id(void);
#endif

extern qurt_thread_t get_netmain_thread_id(void);
extern qurt_thread_t get_ping_thread_id(void);
extern qurt_thread_t get_tcptick_thread_id(void);
extern qurt_thread_t get_timertick_thread_id(void);
extern qurt_thread_t get_ipfragtick_thread_id(void);
extern qurt_thread_t get_arpnattick_thread_id(void);
extern qurt_thread_t get_dns_proxy_thread_id(void);
extern qurt_thread_t get_dnstick_thread_id(void);
#ifdef ENABLE_COAP
extern qurt_thread_t get_coap_thread_id(void);
#endif
#ifdef LWM2M_ENABLE
extern qurt_thread_t get_lwm2m_thread_Handle(void);
#endif
extern qurt_thread_t get_atfwd_task_thread_id(void);
extern qurt_thread_t get_qti_task_thread_id(void);
extern qurt_thread_t get_qti_bridge_task_thread_id(void);

bool is_profile_started = false;
uint8_t tx_max_th = 0;

static size_t int_to_text(int64_t data, uint8_t *string, size_t length)
{
    int index = 0;
    bool minus = false;
    size_t result = 0;
    if(string == NULL)
    {
        return 0;
    }

    if (data < 0)
    {
        minus = true;
        data = 0 - data;
    }
    else
    {
        minus = false;
    }

    index = length - 1;
    do
    {
        string[index] = '0' + data%10;
        data /= 10;
        index --;
    } while(index >= 0 && data > 0);

    if(data > 0)
        return 0;

    if(minus == true)
    {
        if(index == 0)
            return 0;
        string[index] = '-';
    }
    else
    {
        index++;
    }

    result = length - index;

    if (result < length)
    {
        memscpy(string, length, string + index, result);
    }

    return result;
}

static size_t float_to_text(double data, uint8_t *string, size_t length)
{
    size_t int_length = 0;
    size_t dec_length = 0;
    int64_t int_part = 0;
    double dec_part = 0;

    if(string == NULL)
    {
        return 0;
    }

    if(data <= (double)INT64_MIN || data >= (double)INT64_MAX)
    {
        return 0;
    }

    int_part = (int64_t)data;
    dec_part = data - int_part;
    if(dec_part < 0)
    {
        dec_part = 1 - dec_part;
    }
    else
    {
        dec_part = 1 + dec_part;
    }

    if(dec_part <= 1 + FLT_EPSILON)
    {
        dec_part = 0;
    }

    if (int_part == 0 && data < 0)
    {
        // deal with numbers between -1 and 0
        if(length < 4) return 0;
        string[0] = '-';
        string[1] = '0';
        int_length = 2;
    }
    else
    {
        int_length = int_to_text(int_part, string, length);
        if(int_length == 0) return 0;
    }
    dec_length = 0;
    if(dec_part >= FLT_EPSILON)
    {
        int i;
        double floor;

        if(int_length >= length - 1) return 0;

        i = 0;
        floor = FLT_EPSILON;
        do
        {
            dec_part *= 10;
            floor *= 10;
            i++;
        }while(dec_part - (int64_t)dec_part > floor);

        dec_length = int_to_text(dec_part, string + int_length, length - int_length);
        if(dec_length <= 1) return 0;

        // replace the leading 1 with a dot
        string[int_length] = '.';
    }

    return int_length + dec_length;
}

static size_t float64_to_text(double data, uint8_t **buffer)
{
    uint8_t string[32 * 2];
    size_t length = 0;
    if(buffer== NULL )
    {
        return 0;
    }
    length = float_to_text(data, string, 32 * 2);
    if(length == 0)
    {
        return 0;
    }

    if(*buffer == NULL)
    {
        return 0;
    }

    memscpy(*buffer, length, string, length);
    return length;
}

static void tx_swap(apps_tx_thread_list *xp, apps_tx_thread_list *yp)
{
    apps_tx_thread_list temp = *xp;
    *xp = *yp;
    *yp = temp;
}

static void tx_thread_sort(apps_tx_thread_list arr[])
{
    uint8_t i, j = 0;
    bool swapped = false;

    for (i = 0; i < tx_max_th-1; i++)
    {
        swapped = false;
        for (j = 0; j < tx_max_th-i-1; j++)
        {
            if (arr[j].cpu_per_val < arr[j+1].cpu_per_val)
            {
                tx_swap(&arr[j], &arr[j+1]);
                swapped = true;
            }
        }

        if (swapped == false)
            break;
    }
}

void display_tx_cpu_per(apps_tx_thread_list arr[])
{
    uint8_t i;
    uint8_t *buffer = NULL;
    
    buffer = (uint8_t *) malloc(TH_NAME_SIZE);
	
	if ( buffer == NULL )
	{
		return;
	}
    
    PROFILE_PRINTF("Name\t\t\t CPU%\n");
    for (i = 0; i < tx_max_th; i++)
    {
        memset(buffer, 0, TH_NAME_SIZE);
        float64_to_text(arr[i].cpu_per_val, &buffer);
        PROFILE_PRINTF("%s \t\t %s \n", arr[i].thread_name,buffer);
    }
    PROFILE_PRINTF("********************************************************\n");
    free(buffer);
}


void profile_configure_thread_id()
{
    uint8_t curr_th_index = 0;
    qurt_thread_t tx_thread_id;

    tx_thread_list[curr_th_index].thread_name = "IDLE_TH          ";
    tx_thread_list[curr_th_index].module_thread_id = 65535; /*For Idle mode thread id is not required, this thread id is ignored*/
    tx_thread_list[curr_th_index].cpu_per_val = 0.0;
    tx_thread_list[curr_th_index].is_profile_needed = true;
    curr_th_index++;

    if((tx_thread_id = get_netmain_thread_id()) != 0)
    {
        tx_thread_list[curr_th_index].thread_name = "NETMAIN_TH   ";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
    if((tx_thread_id = get_ping_thread_id()) != 0)
    {
        tx_thread_list[curr_th_index].thread_name = "PING_TH      ";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
    if((tx_thread_id = get_tcptick_thread_id()) != 0)
    {
        tx_thread_list[curr_th_index].thread_name = "TCPTICK_TH   ";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
    if((tx_thread_id = get_timertick_thread_id()) != 0)
    {
        tx_thread_list[curr_th_index].thread_name = "TIMERTICK_TH ";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
    if((tx_thread_id = get_arpnattick_thread_id()) != 0)
    {
        tx_thread_list[curr_th_index].thread_name = "ARPNAT_TICK_TH";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
    if((tx_thread_id = get_ipfragtick_thread_id()) != 0)
    {
        tx_thread_list[curr_th_index].thread_name = "IPFRAG_TICK_TH";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
    if((tx_thread_id = get_dns_proxy_thread_id()) != 0)
    {
        tx_thread_list[curr_th_index].thread_name = "DNS_PROXY_TH  ";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
    if((tx_thread_id = get_dnstick_thread_id()) != 0)
    {
        tx_thread_list[curr_th_index].thread_name = "DNS_TICK_TH   ";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
#ifdef MQTT_ENABLE
    if((tx_thread_id = get_mqtt_thread_id())!= 0)
    {
        tx_thread_list[curr_th_index].thread_name = "MQTT_TH      ";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
#endif
	
#ifdef ENABLE_COAP
    if((tx_thread_id = get_coap_thread_id()) != 0)
    {
        tx_thread_list[curr_th_index].thread_name = "COAP_TH       ";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
#endif

#ifdef LWM2M_ENABLE
    if((tx_thread_id = get_lwm2m_thread_Handle())!= 0)
    {
        tx_thread_list[curr_th_index].thread_name = "LWM2M_TH      ";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
#endif

#ifdef QTI_BRIDGE_ENABLE
    if((tx_thread_id = get_atfwd_task_thread_id())!= 0)
    {
        tx_thread_list[curr_th_index].thread_name = "ATFWD_TH      ";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
#endif
#ifdef QTI_ENABLE
    if((tx_thread_id = get_qti_task_thread_id())!= 0)
    {
        tx_thread_list[curr_th_index].thread_name = "QTITASK_TH    ";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
#endif
#ifdef QTI_BRIDGE_ENABLE
    if((tx_thread_id = get_qti_bridge_task_thread_id())!= 0)
    {
        tx_thread_list[curr_th_index].thread_name = "QTIBRIDGE     ";
        tx_thread_list[curr_th_index].module_thread_id = tx_thread_id;
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index++;
    }
#endif
    {
        tx_thread_list[curr_th_index].thread_name = "QCLI_TH          ";
        tx_thread_list[curr_th_index].module_thread_id = qurt_thread_get_id();
        tx_thread_list[curr_th_index].cpu_per_val = 0.0;
        tx_thread_list[curr_th_index].is_profile_needed = false;
        curr_th_index ++;
    }

    tx_max_th = curr_th_index;
}

void tx_get_profile_status(void)
{
    double idl_time, idl_total_time = 0.0;
    uint8_t i;

    //update the CPU % for idle thread
    idl_time =qurt_get_profiling_time_Idle();
    idl_total_time =qurt_profile_getTotalTime_IdleThread();
    if(idl_total_time > 0)
    {
        tx_thread_list[0].cpu_per_val = (double)(idl_time)/idl_total_time * 100;
    }

    if((int)tx_thread_list[0].cpu_per_val > 100)
    {
        /* After certain period of time the current time > the total times
         * So, below code will reset automatically and start the CPU profiling
         */
        qurt_start_profiling_Idle_thread();
        tx_thread_sleep(10);
        idl_time =qurt_get_profiling_time_Idle();
        idl_total_time =qurt_profile_getTotalTime_IdleThread();
        if(idl_total_time > 0)
        {
            tx_thread_list[0].cpu_per_val = (double)(idl_time)/idl_total_time * 100;
        }
    }
   
    /*Profiling for non-idle threads*/
    for (i = 1; i < tx_max_th; i++)
    {
        if(true == tx_thread_list[i].is_profile_needed)
        {
            idl_time = qurt_get_profiling_time(tx_thread_list[i].module_thread_id);
            idl_total_time = qurt_profile_getTotalTime(tx_thread_list[i].module_thread_id);
            if(idl_total_time > 0)
            {
                tx_thread_list[i].cpu_per_val = (double)(idl_time)/idl_total_time * 100;
            }

            if((int)tx_thread_list[i].cpu_per_val > 100)
            {
                /* After certain period of time the current time > the total times
                 * So, below code will reset automatically and start the CPU profiling
                 */
                qurt_start_profiling_thread(tx_thread_list[i].module_thread_id);
                tx_thread_sleep(10);
                idl_time = qurt_get_profiling_time(tx_thread_list[i].module_thread_id);
                idl_total_time = qurt_profile_getTotalTime(tx_thread_list[i].module_thread_id);
                if(idl_total_time > 0)
                {
                    tx_thread_list[i].cpu_per_val = (double)(idl_time)/idl_total_time * 100;
                }
            }
        }
    }

    tx_thread_sort(tx_thread_list);
    display_tx_cpu_per(tx_thread_list);
}

void profile_start()
{
    uint8_t i;

    PROFILE_PRINTF("Start profiling\n");

    /*Start profling fir idle th*/
    qurt_start_profiling_Idle_thread();

    for (i = 1; i < tx_max_th; i++)
    {
        if (true == tx_thread_list[i].is_profile_needed)
        {
            qurt_start_profiling_thread(tx_thread_list[i].module_thread_id);
        }
    }

    while(1)
    {
        tx_thread_sleep(120);
        if(test_for_quit_s())
        {
            break;
        }
        tx_get_profile_status();
    }
}

QCLI_Command_Status_t profile_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    uint8_t j,i=0;
    QCLI_Command_Status_t ret = QCLI_STATUS_ERROR_E; 
    server_quit = 0; 
    if(Parameter_Count < 1)
    {
        PROFILE_PRINTF("Wrong usage Parameter_Count %d\n",Parameter_Count);
        return QCLI_STATUS_USAGE_E;
    }

    if (0 == strcmp((char*)Parameter_List[i].String_Value, "start"))
    {
        i++;
        memset(tx_thread_list, 0, sizeof(apps_tx_thread_list));
        if (i < Parameter_Count)
        {
            profile_configure_thread_id();
            is_profile_started = true;
            if (0 == strcmp((char*)Parameter_List[i].String_Value, "all"))
            {
                for (j = 1; j < tx_max_th; j++)
                {
                    tx_thread_list[j].is_profile_needed = true;
                }
            }
            else
            {
                while(i < Parameter_Count)
                {
                    for (j=1; j < tx_max_th; j++)
                    {
                        if (0 == strncasecmp((char*)Parameter_List[i].String_Value, tx_thread_list[j].thread_name,4))
                        {
                            tx_thread_list[j].is_profile_needed = true;
                            break;
                        }
                    }
                    i++;
                }
            }
            profile_start();
            ret = QCLI_STATUS_SUCCESS_E;
        }
        else
        {
            PROFILE_PRINTF("Wrong usage\n");
            ret = QCLI_STATUS_USAGE_E;
        }
    }
    else if (0 == strcmp((char*)Parameter_List[i].String_Value, "status") && (true == is_profile_started))
    {
        while(1)
        {
            tx_thread_sleep(120);
            if(test_for_quit_s())
            {
                break;
            }
            tx_get_profile_status();
        }
    }
    else if (0 == strcmp((char*)Parameter_List[i].String_Value, "reset") && (true == is_profile_started))
    {
        i++;
        if (i < Parameter_Count)
        {
            if (0 == strcmp((char*)Parameter_List[i].String_Value, "all"))
            {
                for (j = 1; j < tx_max_th; j++)
                {
                    tx_thread_list[j].cpu_per_val = 0.0;
                    tx_thread_list[j].is_profile_needed = false;
                }
            }
            else
            {
                while(i < Parameter_Count)
                {
                    for (j = 1; j < tx_max_th; j++)
                    {
                        if (0 == strncasecmp((char*)Parameter_List[i].String_Value, tx_thread_list[j].thread_name,4))
                        {
                            tx_thread_list[j].cpu_per_val = 0.0;
                            tx_thread_list[j].is_profile_needed = false;
                            break;
                        }
                    }
                    i++;
                }
            }
            display_tx_cpu_per(tx_thread_list);
            ret = QCLI_STATUS_SUCCESS_E;
        }
        else
        {
            PROFILE_PRINTF("Wrong usage\n");
            ret = QCLI_STATUS_USAGE_E;
        }
    }
    else
    {
        PROFILE_PRINTF("Wrong usage\n");
        ret = QCLI_STATUS_USAGE_E;
    }

    return ret;
}


/*!
  @file
  dss_netapp.c

  @brief
  DSS netctrl test application functionality to interact with underlying 
  data control plane. 
*/

/*===========================================================================

  Copyright (c) 2017-2021 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "qurt_types.h"
#include "qcli.h"
#include "qcli_api.h"
#include "dss_netapp.h"
#include "qapi_dss.h"
#include "timer.h"
#include "qapi_ns_utils.h"
#include "netutils.h"
#include "qapi_ssl.h"
#include "multi_app_test.h"

/* Forward declarations */
static QCLI_Command_Status_t dss_get_call_handle(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_release_call_handle(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_test_set_param(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_test_net_up(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_test_net_down(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
#ifndef MINI_QCLI
static QCLI_Command_Status_t dss_test_list_handle(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
#endif
static QCLI_Command_Status_t dss_lib_init(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
#ifndef MINI_QCLI
static QCLI_Command_Status_t dss_test_get_pkt_stats(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_test_reset_pkt_stats(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_test_get_call_end_reason(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_test_get_call_tech(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_test_get_current_data_bearer_tech(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_test_get_device_name(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_test_get_qmi_port_name(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
#endif
static QCLI_Command_Status_t dss_test_get_ip_addr_count(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_test_get_ip_addr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
#ifndef MINI_QCLI
static QCLI_Command_Status_t dss_test_get_link_mtu(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
#endif
static QCLI_Command_Status_t dss_test_send_nipd_data(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_test_add_filters(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_test_remove_filters(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
#ifndef MINI_QCLI
static QCLI_Command_Status_t nipd_data_tx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t nipd_data_rx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
#endif
static QCLI_Command_Status_t dss_set_data_transfer_status(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dss_set_data_transfer_status_v2(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

const QCLI_Command_t dss_netapp_cmd_list[] =
{
    /* Command_Function, Start_Thread, Command_String, Usage_String, Description */
    {dss_lib_init, false, "InitTest", "[-app <app id: 0/1>]", "init test library",NULL},
    {dss_get_call_handle, false, "GetHandle", "[-app <app id: 0/1>]", "Get call handle",NULL},
    {dss_release_call_handle, false, "ReleaseHandle", "[-app <app id: 0/1>] <call index>", "Release call handle",NULL},
    {dss_test_set_param, false, "SetParams", "[-app <app id: 0/1>] <call index> <tech> <family> <profile> <apn>", "Set parameter for the handle",NULL},
    {dss_test_net_up, false, "StartCall", "[-app <app id: 0/1>] <call index> <optional family>", "Start the data call",NULL},
    {dss_test_net_down, false, "StopCall", "[-app <app id: 0/1>] <call index>", "Stop the data call",NULL},
#ifndef MINI_QCLI
    {dss_test_list_handle, false, "ListHandles", "[-app <app id: 0/1>]", "Display all the handles",NULL},
    {dss_test_get_pkt_stats, false, "GetPktStats", "[-app <app id: 0/1>] <call index>", "Display packet stats",NULL},
    {dss_test_reset_pkt_stats, false, "ResetPktStats", "[-app <app id: 0/1>] <call index>", "Reset packet stats",NULL},
    {dss_test_get_call_end_reason, false, "CallEndReason", "[-app <app id: 0/1>] <call index> <family>", "Display call end reason",NULL},
    {dss_test_get_call_tech, false, "CallTech", "[-app <app id: 0/1>] <call index>", "Display call technology",NULL},
    {dss_test_get_current_data_bearer_tech, false, "BearerTech", "[-app <app id: 0/1>] <call index>", "Display current bearer technology",NULL},
    {dss_test_get_device_name, false, "DeviceName", "[-app <app id: 0/1>] <call index>", "Display device name",NULL},
    {dss_test_get_qmi_port_name, false, "QMIPortName", "[-app <app id: 0/1>] <call index>", "Display qmi port name",NULL},
#endif
    {dss_test_get_ip_addr_count, false, "IPAddressCnt", "[-app <app id: 0/1>] <call index>", "Display count of ip address",NULL},
    {dss_test_get_ip_addr, false, "GetIPAddress", "[-app <app id: 0/1>] <call index>", "Display all ip address",NULL},
#ifndef MINI_QCLI
    {dss_test_get_link_mtu, false, "MTUSize", "[-app <app id: 0/1>] <call index> <family>", "Display link mtu ",NULL},
#endif
    {dss_test_send_nipd_data, false, "SendNIPData", "[-app <app id: 0/1>] <call index> <packets> <data length> <exception type>","Send non-ip UL data",NULL},
    {dss_test_add_filters, false, "AddEXIPFilters", "[-app <app id: 0/1>] <call index> <-v4/-v6> <src address> <src mask/prefix length> <dst address> <dst mask/prefix length> <-upd/-tcp> <src port> <src range> <dst port> <dst range>\n",
                                                    "Add MO exception IP filters",NULL},
    {dss_test_remove_filters, false, "DelEXIPFilters", "[-app <app id: 0/1>] <call index> <filter index>","Remove MO exception IP filters",NULL},
#ifndef MINI_QCLI
    {nipd_data_tx , false, "nipdtx", "[-app <app id: 0/1>] <call index> <packets> <data length> <exception type> <sec/no_sec>","Send non-ip UL data",NULL},
    {nipd_data_rx , false, "nipdrx", "[-app <app id: 0/1>] <call index> ","Recieve non-ip DL data",NULL},
#endif
    {dss_set_data_transfer_status, false, "Set_transfer_status","[-app <app id: 0/1>] <call index> <status> ", "Set data transfer status",NULL},
    {dss_set_data_transfer_status_v2, false, "Set_transfer_status_v2","[-app <app id: 0/1>] <call index> [-s status] [-t next_activity_time] ", "Set data transfer status_v2",NULL}
};

const QCLI_Command_Group_t dss_netapp_cmd_group =
{
    "dss_netapp",              /* Group_String: will display cmd prompt as "Net> " */
    sizeof(dss_netapp_cmd_list)/sizeof(dss_netapp_cmd_list[0]),   /* Command_Count */
    dss_netapp_cmd_list        /* Command_List */
};

QCLI_Group_Handle_t qcli_dss_netapp_handle;     /* Handle for Net Command Group. */


/*****************************************************************************/

#define DSS_PROFILE_3GPP2_OFFSET  1000
#define CE_MAX 9
#define CT_MAX 4
#define QMI_WDS_APN_NAME_MAX_V01 150
#define NETMGR_CALL_INFO_DEVICE_NAME_MAX_LEN 12
#define AF_INET 2
#define AF_INET6 3
#define NIDD_PSK_FILE             "nidd_psk.bin"



// qcli_ds_platform_pthread_cond_t dss_test_cnd_var; /* Not being used anywhere */
qapi_DSS_Call_Param_Value_t call_params[NO_OF_APPS];
qapi_DSS_Call_Param_Identifier_t iden[NO_OF_APPS];
int index[NO_OF_APPS];

extern bool thrd_running[NO_OF_APPS];

#define QCLI_SET_STATE(st,state)                                 \
    qurt_mutex_lock( &(st)->mutex );               \
    (st)->call_status = state;                              \
    qurt_mutex_unlock( &(st)->mutex );


enum dss_test_call_status_e
{
  dss_test_call_status_idle,
  dss_test_call_status_connecting,
  dss_test_call_status_connected,
  dss_test_call_status_disconnecting
};

struct status_strings_s
{
  enum dss_test_call_status_e status;
  const char* str;
};

struct status_strings_s status_string_tbl[] =
{
  {dss_test_call_status_idle, "idle"},
  {dss_test_call_status_connecting, "connecting"},
  {dss_test_call_status_connected, "connected"},
  {dss_test_call_status_disconnecting, "disconnecting"}
};


typedef struct 
{
  qapi_DSS_Hndl_t  call_hndl;
  char tech[8];
  char family[10];
  int profile;
  char apn[QMI_WDS_APN_NAME_MAX_V01+1];
  enum dss_test_call_status_e call_status;
  qcli_ds_platform_pthread_mutex_t mutex;
  qapi_DSS_Add_MO_Exception_Filters_Rsp_t filter_stats;
} dss_test_call_info_t;

dss_test_call_info_t dss_net_hndl[NO_OF_APPS][QAPI_DSS_MAX_DATA_CALLS];


boolean dss_lib_inited[NO_OF_APPS] = {FALSE};

struct event_strings_s
{
  qapi_DSS_Net_Evt_t evt;
  char * str;
};

struct event_strings_s event_string_tbl[QAPI_DSS_EVT_MAX_E] =
{
  { QAPI_DSS_EVT_INVALID_E, " INVALID" },
  { QAPI_DSS_EVT_NET_IS_CONN_E, "NET_IS_CONN" },
  { QAPI_DSS_EVT_NET_NO_NET_E, "NET_NO_NET" },
  { QAPI_DSS_EVT_NET_RECONFIGURED_E, "NET_RECONFIGURED" },
  { QAPI_DSS_EVT_NET_NEWADDR_E, "NET_NEWADDR" },
  { QAPI_DSS_EVT_NET_DELADDR_E, "NET_DELADDR" },
  { QAPI_DSS_EVT_NIPD_DL_DATA_E, "NET_NIPD_DL_DATA" }
};

struct ce_status_strings_s
{
  qapi_DSS_CE_Reason_Type_t call_end;
  char *str;
};

struct ce_status_strings_s ce_status_tbl[CE_MAX] = 
{
  {QAPI_DSS_CE_TYPE_UNINIT_E,"Uninit type" },
  {QAPI_DSS_CE_TYPE_INVALID_E,"Invalid type"},
  {QAPI_DSS_CE_TYPE_MOBILE_IP_E,"Mobile IP type"},
  {QAPI_DSS_CE_TYPE_INTERNAL_E,"Internal type"},
  {QAPI_DSS_CE_TYPE_CALL_MANAGER_DEFINED_E,"Call manager defined type"},
  {QAPI_DSS_CE_TYPE_3GPP_SPEC_DEFINED_E,"3GPP spec defined type"},
  {QAPI_DSS_CE_TYPE_PPP_E,"PPP type"},
  {QAPI_DSS_CE_TYPE_EHRPD_E,"EHPRD type"},
  {QAPI_DSS_CE_TYPE_IPV6_E,"IPV6 type"}
};

struct call_tech_status_strings_s
{
  qapi_DSS_Call_Tech_Type_t call_tech;
  char *str;
};

struct call_tech_status_strings_s call_tech_status_tbl[CT_MAX] = 
{
  {QAPI_DSS_CALL_TECH_INVALID_E,"Invalid "},
  {QAPI_DSS_CALL_TECH_CDMA_E,"CDMA "},
  {QAPI_DSS_CALL_TECH_UMTS_E,"UMTS "},
};

struct data_bearer_status_strings_s
{
  qapi_DSS_Data_Bearer_Tech_t bearer_tech;
  char *str;
};

struct data_bearer_status_strings_s data_bearer_status_tbl[QAPI_DSS_DATA_BEARER_TECH_MAX_E] = 
{
  {QAPI_DSS_DATA_BEARER_TECH_UNKNOWN_E,"Unknown"},
  {QAPI_DSS_DATA_BEARER_TECH_CDMA_1X_E,"1X"},
  {QAPI_DSS_DATA_BEARER_TECH_EVDO_REV0_E,"CDMA Rev 0"},
  {QAPI_DSS_DATA_BEARER_TECH_EVDO_REVA_E,"CDMA Rev A"},
  {QAPI_DSS_DATA_BEARER_TECH_EVDO_REVB_E,"CDMA Rev B"},
  {QAPI_DSS_DATA_BEARER_TECH_EHRPD_E,"EHPRD"},
  {QAPI_DSS_DATA_BEARER_TECH_FMC_E,"FMC"},
  {QAPI_DSS_DATA_BEARER_TECH_HRPD_E,"HRPD"},
  {QAPI_DSS_DATA_BEARER_TECH_3GPP2_WLAN_E,"IWLAN"},
  {QAPI_DSS_DATA_BEARER_TECH_WCDMA_E,"WCDMA "},
  {QAPI_DSS_DATA_BEARER_TECH_GPRS_E,"GPRS"},
  {QAPI_DSS_DATA_BEARER_TECH_HSDPA_E,"HSDPA"},
  {QAPI_DSS_DATA_BEARER_TECH_HSUPA_E,"HSUPA"},
  {QAPI_DSS_DATA_BEARER_TECH_EDGE_E,"EDGE"},
  {QAPI_DSS_DATA_BEARER_TECH_LTE_E,"LTE"},
  {QAPI_DSS_DATA_BEARER_TECH_HSDPA_PLUS_E,"HSDPA+"},
  {QAPI_DSS_DATA_BEARER_TECH_DC_HSDPA_PLUS_E,"DC_HSDPA+"},
  {QAPI_DSS_DATA_BEARER_TECH_HSPA_E,"HSPA"},
  {QAPI_DSS_DATA_BEARER_TECH_64_QAM_E,"64_QAM"},
  {QAPI_DSS_DATA_BEARER_TECH_TDSCDMA_E,"TD-SCDMA"},
  {QAPI_DSS_DATA_BEARER_TECH_GSM_E,"GSM"},
  {QAPI_DSS_DATA_BEARER_TECH_3GPP_WLAN_E,"IWLAN"}
};



int dss_get_data_call_handle_of_index(int app_id ,int ind)
{

   return (int)dss_net_hndl[app_id][ind].call_hndl;

}

int qcli_ds_platform_pthread_mutex_init
(
  qcli_ds_platform_pthread_mutex_t *mutex,
  void *attr
)
{
  qurt_mutex_init(mutex);
  return 0;
}

void dss_test_cb(qapi_DSS_Hndl_t         hndl,
                        void              *user_data,
                        qapi_DSS_Net_Evt_t      evt,
                        qapi_DSS_Evt_Payload_t *payload_ptr)
{
  if (evt > QAPI_DSS_EVT_INVALID_E && evt < QAPI_DSS_EVT_MAX_E)
  {
    int i;
    dss_test_call_info_t* call = (dss_test_call_info_t*) user_data;

    for( i = 0; i < QAPI_DSS_EVT_MAX_E; i++)
    {
      if(event_string_tbl[i].evt == evt)
      {
        QCLI_Printf(qcli_dss_netapp_handle, "dss_test_cb %s evt!!\n",event_string_tbl[i].str);
        break;
      }
    }

    switch(evt)
    {
      case QAPI_DSS_EVT_NET_NO_NET_E:
      {
        QCLI_SET_STATE(call,dss_test_call_status_idle);
        break;
      } 

      case QAPI_DSS_EVT_NET_IS_CONN_E:
      {
        QCLI_SET_STATE(call,dss_test_call_status_connected);
        break;
      }

      case QAPI_DSS_EVT_NIPD_DL_DATA_E:
      {
        if (payload_ptr->data)
        {
          free(payload_ptr->data);
          QCLI_Printf(qcli_dss_netapp_handle, "dss_test_cb received NIPD DL DATA of length %d bytes\n", payload_ptr->data_len);
        }
       
        break;
      }

      case QAPI_DSS_EVT_NET_RECONFIGURED_E:
      case QAPI_DSS_EVT_NET_NEWADDR_E:
      case QAPI_DSS_EVT_NET_DELADDR_E:
        /* Do nothing */
        break;

      default:
        QCLI_Printf(qcli_dss_netapp_handle,"Received unsupported event [%d]", evt);
        return;
    }
  }
}

static QCLI_Command_Status_t 
dss_lib_init(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_LIB_INIT_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 4;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_lib_init(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if(dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib already initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  
  memset( dss_net_hndl[app_id], 0x0, sizeof(dss_net_hndl[app_id]) );
  
  for (i = 0; i < QAPI_DSS_MAX_DATA_CALLS; i++)
  {
    (void)qcli_ds_platform_pthread_mutex_init( &dss_net_hndl[app_id][i].mutex, NULL );
  }
   qapi_DSS_Init((int) QAPI_DSS_MODE_GENERAL);
  dss_lib_inited[app_id] = TRUE;

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_get_call_handle(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_GET_CALL_HANDLE_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 5;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_get_call_handle(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (Parameter_Count > 0)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  for (i = 0; i < QAPI_DSS_MAX_DATA_CALLS; i++)
  {
    if (dss_net_hndl[app_id][i].call_hndl == NULL)
    {
      index[app_id] = i;
      break;
    }
  }

  if (i == QAPI_DSS_MAX_DATA_CALLS)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Max call handle already allocated.");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  qapi_DSS_Get_Data_Srvc_Hndl((qapi_DSS_Net_Ev_CB_t)dss_test_cb, (void*) &dss_net_hndl[app_id][index[app_id]], &(dss_net_hndl[app_id][index[app_id]].call_hndl));
  
  if (dss_net_hndl[app_id][index[app_id]].call_hndl)
  {
    strlcpy(dss_net_hndl[app_id][index[app_id]].tech, "",sizeof(dss_net_hndl[app_id][index[app_id]].tech));
    strlcpy(dss_net_hndl[app_id][index[app_id]].family, "",sizeof(dss_net_hndl[app_id][index[app_id]].family));
    dss_net_hndl[app_id][index[app_id]].profile= NULL;
    strlcpy(dss_net_hndl[app_id][index[app_id]].apn, "",sizeof(dss_net_hndl[app_id][index[app_id]].apn));
    QCLI_SET_STATE(&dss_net_hndl[app_id][index[app_id]], dss_test_call_status_idle);
    QCLI_Printf(qcli_dss_netapp_handle, "Call handle is created at index %d!!\n",index[app_id]+1);
    QCLI_Printf(qcli_dss_netapp_handle, "Set parameter for this handle!!\n");
    index[app_id]++;

    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Call handle is not created \n");

    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_release_call_handle(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_RELEASE_CALL_HANDLE_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 6;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_release_call_handle(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index = -1;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
     QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
     result = QCLI_STATUS_ERROR_E;
     goto end;
  }

  if (Parameter_Count != 1 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }

  if (call_index>=0 && call_index<QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl)
  {
    qapi_DSS_Rel_Data_Srvc_Hndl((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl);
    dss_net_hndl[app_id][call_index].call_hndl = NULL;
    strlcpy(dss_net_hndl[app_id][index[app_id]].tech, "",sizeof(dss_net_hndl[app_id][index[app_id]].tech));
    strlcpy(dss_net_hndl[app_id][index[app_id]].family, "",sizeof(dss_net_hndl[app_id][index[app_id]].family));
    dss_net_hndl[app_id][call_index].profile= NULL;
    strlcpy(dss_net_hndl[app_id][index[app_id]].apn, "",sizeof(dss_net_hndl[app_id][index[app_id]].apn));

    QCLI_Printf(qcli_dss_netapp_handle, "Call handle is released!!\n");
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Call handle is not released!!\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_test_set_param(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_SET_PARAM_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 7;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_set_param(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int prof = 0;
  int call_index = -1;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 5 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Call index not valid!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if ((call_index >= 0) && (call_index < QAPI_DSS_MAX_DATA_CALLS) && (dss_net_hndl[app_id][call_index].call_hndl))
  {
    if (strcmp((char*)Parameter_List[1].String_Value,"umts")==0)
    {
      strlcpy(dss_net_hndl[app_id][call_index].tech,"UMTS",sizeof(dss_net_hndl[app_id][call_index].tech));
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_RADIO_TECH_UMTS;
      iden[app_id] = QAPI_DSS_CALL_INFO_TECH_PREF_E;
    }
    else if (strcmp((char*)Parameter_List[1].String_Value,"cdma")==0)
    {
      strlcpy(dss_net_hndl[app_id][call_index].tech,"CDMA",sizeof(dss_net_hndl[app_id][call_index].tech));
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_RADIO_TECH_CDMA;
      iden[app_id] = QAPI_DSS_CALL_INFO_TECH_PREF_E;
    }
    else if (strcmp((char*)Parameter_List[1].String_Value,"auto")==0)
    {
      strlcpy(dss_net_hndl[app_id][call_index].tech,"AUTO",sizeof(dss_net_hndl[app_id][call_index].tech));
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_RADIO_TECH_UNKNOWN;
      iden[app_id] = QAPI_DSS_CALL_INFO_TECH_PREF_E;
    }
    else 
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Invalid call technology requested, failed to set the call technology!");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
    
    if (QAPI_DSS_SUCCESS!= qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl , 
		(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    QCLI_Printf(qcli_dss_netapp_handle, "Setting tech to %s!!\n",dss_net_hndl[app_id][call_index].tech);
      
    if (strcmp((char*)Parameter_List[2].String_Value,"v4")==0)
    {
      strlcpy(dss_net_hndl[app_id][call_index].family,"IPV4",sizeof(dss_net_hndl[app_id][call_index].family));
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_IP_VERSION_4;
      iden[app_id] = QAPI_DSS_CALL_INFO_IP_VERSION_E;
    }
    else if (strcmp((char*)Parameter_List[2].String_Value,"v6")==0)
    {
      strlcpy(dss_net_hndl[app_id][call_index].family,"IPV6",sizeof(dss_net_hndl[app_id][call_index].family));
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_IP_VERSION_6;
      iden[app_id] = QAPI_DSS_CALL_INFO_IP_VERSION_E;
    }
    else if (strcmp((char*)Parameter_List[2].String_Value,"v4v6")==0)
    {
      strlcpy(dss_net_hndl[app_id][call_index].family,"IPV4V6",sizeof(dss_net_hndl[app_id][call_index].family));
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_IP_VERSION_4_6;
      iden[app_id] = QAPI_DSS_CALL_INFO_IP_VERSION_E;
    }
    else if (strcmp((char*)Parameter_List[2].String_Value,"nonip")==0)
    {
      strlcpy(dss_net_hndl[app_id][call_index].family,"NONIP",sizeof(dss_net_hndl[app_id][call_index].family));
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_EXT_RADIO_TECH_NONIP;
      iden[app_id] = QAPI_DSS_CALL_INFO_EXT_TECH_E;
    }
    else if (strcmp((char*)Parameter_List[2].String_Value,"ex_v4")==0)
    {
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_MO_EXCEPTION_IP_DATA;
      iden[app_id] = QAPI_DSS_CALL_INFO_MO_EXCEPTION_DATA_E;

      if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
	  	(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
      {
        QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      strlcpy(dss_net_hndl[app_id][call_index].family,"IPV4",sizeof(dss_net_hndl[app_id][call_index].family));
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_IP_VERSION_4;
      iden[app_id] = QAPI_DSS_CALL_INFO_IP_VERSION_E;
    }
    else if (strcmp((char*)Parameter_List[2].String_Value,"ex_v6")==0)
    {
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_MO_EXCEPTION_IP_DATA;
      iden[app_id] = QAPI_DSS_CALL_INFO_MO_EXCEPTION_DATA_E;

      if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
	  	(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
      {
        QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      strlcpy(dss_net_hndl[app_id][call_index].family,"IPV6",sizeof(dss_net_hndl[app_id][call_index].family));
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_IP_VERSION_6;
      iden[app_id] = QAPI_DSS_CALL_INFO_IP_VERSION_E;
    }
    else if (strcmp((char*)Parameter_List[2].String_Value,"ex_v4v6")==0)
    {
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_MO_EXCEPTION_IP_DATA;
      iden[app_id] = QAPI_DSS_CALL_INFO_MO_EXCEPTION_DATA_E;
        
      if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
	  	(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
      {
        QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      strlcpy(dss_net_hndl[app_id][call_index].family,"IPV4V6",sizeof(dss_net_hndl[app_id][call_index].family));
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_IP_VERSION_4_6;
      iden[app_id] = QAPI_DSS_CALL_INFO_IP_VERSION_E;
    }
    else if (strcmp((char*)Parameter_List[2].String_Value,"ex_nonip")==0)
    {
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_MO_EXCEPTION_NONIP_DATA;
      iden[app_id] = QAPI_DSS_CALL_INFO_MO_EXCEPTION_DATA_E;

      if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
	  	(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
      {
        QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      strlcpy(dss_net_hndl[app_id][call_index].family,"EX_NONIP",sizeof(dss_net_hndl[app_id][call_index].family));
      call_params[app_id].buf_val = NULL;
      call_params[app_id].num_val = QAPI_DSS_EXT_RADIO_TECH_NONIP;
      iden[app_id] = QAPI_DSS_CALL_INFO_EXT_TECH_E;
    }
    else 
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Invalid family type requested, failed to set the family!");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
		(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    QCLI_Printf(qcli_dss_netapp_handle, "Setting family to %s!!\n",dss_net_hndl[app_id][call_index].family);
      
    if ((Parameter_List[3].Integer_Is_Valid) && ((prof = Parameter_List[3].Integer_Value) > 0))
    {
      char profile_str[10]; 

      if (prof > DSS_PROFILE_3GPP2_OFFSET)
      {
        dss_net_hndl[app_id][call_index].profile = prof - DSS_PROFILE_3GPP2_OFFSET;
        call_params[app_id].num_val = prof - DSS_PROFILE_3GPP2_OFFSET;
        iden[app_id] = QAPI_DSS_CALL_INFO_CDMA_PROFILE_IDX_E;   

        strlcpy(profile_str, "3GPP2", sizeof(profile_str));     
      }
      else
      {
        dss_net_hndl[app_id][call_index].profile = prof;
        call_params[app_id].num_val = prof;
        iden[app_id] = QAPI_DSS_CALL_INFO_UMTS_PROFILE_IDX_E;
        
        strlcpy(profile_str, "3GPP", sizeof(profile_str));
      }

      if (QAPI_DSS_SUCCESS!= qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
	  	(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
      {
        QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
         
      QCLI_Printf(qcli_dss_netapp_handle, "Setting %s profile to %d!!\n", profile_str, dss_net_hndl[app_id][call_index].profile);
    }
  
    if (strlen((char*)Parameter_List[4].String_Value) > QMI_WDS_APN_NAME_MAX_V01)
    {
      QCLI_Printf(qcli_dss_netapp_handle, "APN length should be less than 150 !!");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
    else
    {
      strlcpy(dss_net_hndl[app_id][call_index].apn,(char*)Parameter_List[4].String_Value,sizeof(dss_net_hndl[app_id][call_index].apn));
      call_params[app_id].buf_val = (char*)Parameter_List[4].String_Value;
      call_params[app_id].num_val = strlen((char*)Parameter_List[4].String_Value);
      iden[app_id] = QAPI_DSS_CALL_INFO_APN_NAME_E;

      if (QAPI_DSS_SUCCESS!= qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
	  	(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
      {
        QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
        result = QCLI_STATUS_ERROR_E;
      goto end;
      }
             
      QCLI_Printf(qcli_dss_netapp_handle, "Setting apn to %s!!\n",dss_net_hndl[app_id][call_index].apn);      
    }

    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_test_net_down(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_NET_DOWN_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 9;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_net_down(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index = -1;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
    call_index = Parameter_List[0].Integer_Value-1;

  if (call_index >= 0 && call_index<QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl )
  {
    QCLI_SET_STATE(&dss_net_hndl[app_id][call_index], dss_test_call_status_disconnecting);

    if (!qapi_DSS_Stop_Data_Call((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl))
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Call Stopped !!\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Call not Stopped !!\n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Call not Stopped !!\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}


/*****************************************************************************
 * This function is used to register the Net Command Group with QCLI.
 *****************************************************************************/
void Initialize_Dss_Netapp_Demo(void)
{
  /* Attempt to reqister the Command Groups with the qcli framework.*/
  qcli_dss_netapp_handle = QCLI_Register_Command_Group(NULL, &dss_netapp_cmd_group);

  if (qcli_dss_netapp_handle)
    QCLI_Printf(qcli_dss_netapp_handle, "DSS_NetApp Registered\n");

  return;
}

static QCLI_Command_Status_t 
dss_test_net_up(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_NET_UP_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 8;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_net_up(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index = -1;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL || Parameter_Count > 2)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
    call_index = Parameter_List[0].Integer_Value - 1;

  if (call_index >= 0 && call_index<QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "DSS_TEST_MAIN !!\n");
    QCLI_SET_STATE(&dss_net_hndl[app_id][call_index], dss_test_call_status_connecting);

    if (Parameter_Count == 2)
    {
      if (strcmp((char*)Parameter_List[1].String_Value,"v4")==0)
      {
        strlcpy(dss_net_hndl[app_id][call_index].family,"IPV4",sizeof(dss_net_hndl[app_id][call_index].family));
        call_params[app_id].buf_val = NULL;
        call_params[app_id].num_val = QAPI_DSS_IP_VERSION_4;
        iden[app_id] = QAPI_DSS_CALL_INFO_IP_VERSION_E;
      }
      else if (strcmp((char*)Parameter_List[1].String_Value,"v6")==0)
      {
        strlcpy(dss_net_hndl[app_id][call_index].family,"IPV6",sizeof(dss_net_hndl[app_id][call_index].family));
        call_params[app_id].buf_val = NULL;
        call_params[app_id].num_val = QAPI_DSS_IP_VERSION_6;
        iden[app_id] = QAPI_DSS_CALL_INFO_IP_VERSION_E;
      }
      else if (strcmp((char*)Parameter_List[1].String_Value,"v4v6")==0)
      {
        strlcpy(dss_net_hndl[app_id][call_index].family,"IPV4V6",sizeof(dss_net_hndl[app_id][call_index].family));
        call_params[app_id].buf_val = NULL;
        call_params[app_id].num_val = QAPI_DSS_IP_VERSION_4_6;
        iden[app_id] = QAPI_DSS_CALL_INFO_IP_VERSION_E;
      }
      else if (strcmp((char*)Parameter_List[1].String_Value,"nonip")==0)
      {
        strlcpy(dss_net_hndl[app_id][call_index].family,"NONIP",sizeof(dss_net_hndl[app_id][call_index].family));
        call_params[app_id].buf_val = NULL;
        call_params[app_id].num_val = QAPI_DSS_EXT_RADIO_TECH_NONIP;
        iden[app_id] = QAPI_DSS_CALL_INFO_EXT_TECH_E;
      }
      else if (strcmp((char*)Parameter_List[1].String_Value,"ex_v4")==0)
      {
        call_params[app_id].buf_val = NULL;
        call_params[app_id].num_val = QAPI_DSS_MO_EXCEPTION_IP_DATA;
        iden[app_id] = QAPI_DSS_CALL_INFO_MO_EXCEPTION_DATA_E;
        
        if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
			(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
        {
          QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
          result = QCLI_STATUS_ERROR_E;
          goto end;
        }
        
        strlcpy(dss_net_hndl[app_id][call_index].family,"IPV4",sizeof(dss_net_hndl[app_id][call_index].family));
        call_params[app_id].buf_val = NULL;
        call_params[app_id].num_val = QAPI_DSS_IP_VERSION_4;
        iden[app_id] = QAPI_DSS_CALL_INFO_IP_VERSION_E;
      }
      else if (strcmp((char*)Parameter_List[1].String_Value,"ex_v6")==0)
      {
        call_params[app_id].buf_val = NULL;
        call_params[app_id].num_val = QAPI_DSS_MO_EXCEPTION_IP_DATA;
        iden[app_id] = QAPI_DSS_CALL_INFO_MO_EXCEPTION_DATA_E;
        
        if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
			(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
        {
          QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
          result = QCLI_STATUS_ERROR_E;
          goto end;
        }
        
        strlcpy(dss_net_hndl[app_id][call_index].family,"IPV6",sizeof(dss_net_hndl[app_id][call_index].family));
        call_params[app_id].buf_val = NULL;
        call_params[app_id].num_val = QAPI_DSS_IP_VERSION_6;
        iden[app_id] = QAPI_DSS_CALL_INFO_IP_VERSION_E;
      }
      else if (strcmp((char*)Parameter_List[1].String_Value,"ex_v4v6")==0)
      {
        call_params[app_id].buf_val = NULL;
        call_params[app_id].num_val = QAPI_DSS_MO_EXCEPTION_IP_DATA;
        iden[app_id] = QAPI_DSS_CALL_INFO_MO_EXCEPTION_DATA_E;
        
        if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
			(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
        {
          QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
          result = QCLI_STATUS_ERROR_E;
          goto end;
        }
        
        strlcpy(dss_net_hndl[app_id][call_index].family,"IPV4V6",sizeof(dss_net_hndl[app_id][call_index].family));
        call_params[app_id].buf_val = NULL;
        call_params[app_id].num_val = QAPI_DSS_IP_VERSION_4_6;
        iden[app_id] = QAPI_DSS_CALL_INFO_IP_VERSION_E;
      }
      else if (strcmp((char*)Parameter_List[1].String_Value,"ex_nonip")==0)
      {
        call_params[app_id].buf_val = NULL;
        call_params[app_id].num_val = QAPI_DSS_MO_EXCEPTION_NONIP_DATA;
        iden[app_id] = QAPI_DSS_CALL_INFO_MO_EXCEPTION_DATA_E;
        
        if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
			(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
        {
          QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
          result = QCLI_STATUS_ERROR_E;
          goto end;
        }
        
        strlcpy(dss_net_hndl[app_id][call_index].family,"EX_NONIP",sizeof(dss_net_hndl[app_id][call_index].family));
        call_params[app_id].buf_val = NULL;
        call_params[app_id].num_val = QAPI_DSS_EXT_RADIO_TECH_NONIP;
        iden[app_id] = QAPI_DSS_CALL_INFO_EXT_TECH_E;
      }
      else 
      {
        QCLI_Printf(qcli_dss_netapp_handle, "Invalid family type requested, failed to set the family!");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
        
      if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Data_Call_Param((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
	  	(qapi_DSS_Call_Param_Identifier_t)iden[app_id], (qapi_DSS_Call_Param_Value_t *)&call_params[app_id]))
      {
         QCLI_Printf(qcli_dss_netapp_handle,"Failed to set call parameters");
         result =  QCLI_STATUS_ERROR_E;
         goto end;
      }
        
      QCLI_Printf(qcli_dss_netapp_handle, "Setting family to %s!!\n",dss_net_hndl[app_id][call_index].family);
    }

    if (!qapi_DSS_Start_Data_Call((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl))
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Call Created !!\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Call not Created !!\n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Call not Created !!\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}
#ifndef MINI_QCLI
static QCLI_Command_Status_t dss_test_list_handle(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_LIST_HANDLE_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 10;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_list_handle(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int cur_ind, call_up = 0;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  for (cur_ind = 0; cur_ind < 20; cur_ind++)
  {
    if (dss_net_hndl[app_id][cur_ind].call_hndl)
    {
      QCLI_Printf(qcli_dss_netapp_handle ," %d Tech: %s   Family: %s Profile: %d APN %s Call status [%s]\n",
                  (cur_ind + 1),
                  dss_net_hndl[app_id][cur_ind].tech,
                  dss_net_hndl[app_id][cur_ind].family,
                  dss_net_hndl[app_id][cur_ind].profile,
                  dss_net_hndl[app_id][cur_ind].apn,
                  status_string_tbl[dss_net_hndl[app_id][cur_ind].call_status].str);
      
      call_up=1;
    }
  }
  
  if (!call_up)
  {
     QCLI_Printf(qcli_dss_netapp_handle, "No handles !!\n");
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_test_reset_pkt_stats(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_RESET_PKT_STATS_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 12;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_reset_pkt_stats(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index =-1;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }

  if (call_index >= 0 && call_index<QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl )
  {
    if (qapi_DSS_Reset_Pkt_Stats((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl) == 0)
    {
      QCLI_Printf(qcli_dss_netapp_handle," Packet stats reset successfully\n");
    }
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Error while resetting packet stats \n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_test_get_pkt_stats(uint32_t Parameter_Count, QCLI_Parameter_t * Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_GET_PKT_STATS_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 11;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_get_pkt_stats(uint32_t Parameter_Count, QCLI_Parameter_t * Parameter_List, int app_id)
{
  int call_index = -1;
  qapi_DSS_Data_Pkt_Stats_t pkt_stats;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }

  memset(&pkt_stats,0,sizeof(qapi_DSS_Data_Pkt_Stats_t));

  if (call_index >= 0 && call_index<QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl )
  {
    if (qapi_DSS_Get_Pkt_Stats( (qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
		(qapi_DSS_Data_Pkt_Stats_t *)&pkt_stats) == 0)
    {
      QCLI_Printf(qcli_dss_netapp_handle," Packet stats for bytes_rx = %lli bytes_tx = %lli pkts_dropped_rx = %lu pkts_dropped_tx = %lu pkts_rx = %lu pkts_tx = %lu \n",
                                           pkt_stats.bytes_rx,
                                           pkt_stats.bytes_tx,
                                           pkt_stats.pkts_dropped_rx,
                                           pkt_stats.pkts_dropped_tx,
                                           pkt_stats.pkts_rx,
                                           pkt_stats.pkts_tx);
    }
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Error while getting packet stats \n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_test_get_call_end_reason(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_GET_CALL_END_REASON_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 13;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_get_call_end_reason(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index =-1 ,i;
  qapi_DSS_CE_Reason_t ce_reason;
  qapi_DSS_IP_Family_t ip_family = (qapi_DSS_IP_Family_t) - 1;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 2 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }
  if ((strcasecmp((char*)Parameter_List[1].String_Value,"v4")==0) || (strcasecmp((char*)Parameter_List[1].String_Value,"nonip")==0))
  {
    ip_family = QAPI_DSS_IP_FAMILY_V4_E;
  }
  else if(strcasecmp((char*)Parameter_List[1].String_Value,"v6")==0)
  {
    ip_family = QAPI_DSS_IP_FAMILY_V6_E;
  }
  else if(strcasecmp((char*)Parameter_List[1].String_Value,"v4v6")==0)
  {
    ip_family = QAPI_DSS_IP_FAMILY_V4_E;
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Not a valid family type \n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  memset(&ce_reason,0,sizeof(qapi_DSS_CE_Reason_t));

  if (call_index >= 0 && call_index<QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl)
  {
    if (qapi_DSS_Get_Call_End_Reason((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
		(qapi_DSS_CE_Reason_t *)&ce_reason, (qapi_DSS_IP_Family_t)ip_family)==0)
    {
      for (i = 0; i < CE_MAX; i++)
      {
        if (ce_status_tbl[i].call_end == ce_reason.reason_type)
        {
          QCLI_Printf(qcli_dss_netapp_handle, "Call end reason type %s and reason code %d \n",ce_status_tbl[i].str,ce_reason.reason_code);
          break;
        }
      }
    }

    if (strcmp((char*)Parameter_List[1].String_Value,"v4v6") == 0)
    {
      ip_family = QAPI_DSS_IP_FAMILY_V6_E;
      memset(&ce_reason,0,sizeof(qapi_DSS_CE_Reason_t));
   
      if (qapi_DSS_Get_Call_End_Reason((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
	  	(qapi_DSS_CE_Reason_t *)&ce_reason, (qapi_DSS_IP_Family_t)ip_family) == 0)
      {
        for (i = 0; i < CE_MAX; i++)
        {
          if (ce_status_tbl[i].call_end == ce_reason.reason_type)
          {
            QCLI_Printf(qcli_dss_netapp_handle, "Call end reason type %s and reason code %d \n",ce_status_tbl[i].str,ce_reason.reason_code);
            break;
          }
        }
      }
    }
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_test_get_call_tech(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_GET_CALL_TECH_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 14;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_get_call_tech(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index = -1, i  = 0;
  qapi_DSS_Call_Tech_Type_t call_tech;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }

  memset(&call_tech,0,sizeof( qapi_DSS_Call_Tech_Type_t));

  if (call_index >= 0 && call_index<QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl)
  {
    if (qapi_DSS_Get_Call_Tech((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
		(qapi_DSS_Call_Tech_Type_t  *)&call_tech)==0)
    {
      for (i = 0; i < CT_MAX; i++)
      {
        if (call_tech_status_tbl[i].call_tech == call_tech)
        {
          QCLI_Printf(qcli_dss_netapp_handle, "Call technology type is  %s\n",call_tech_status_tbl[i].str);
          break;
        }
      }
    }
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Error while getting call technology \n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_test_get_current_data_bearer_tech(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_GET_CURRENT_DATA_BEARER_TECH_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 15;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_get_current_data_bearer_tech(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index = -1, i;
  qapi_DSS_Data_Bearer_Tech_t bearer_tech = QAPI_DSS_DATA_BEARER_TECH_UNKNOWN_E;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }

  if (call_index >= 0 && call_index<QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl)
  {
    qapi_DSS_Get_Current_Data_Bearer_Tech((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl,
    									  &bearer_tech);

    for (i = 0; i < QAPI_DSS_DATA_BEARER_TECH_MAX_E;i++)
    {
      if (data_bearer_status_tbl[i].bearer_tech == bearer_tech)
      {
        QCLI_Printf(qcli_dss_netapp_handle, "Current data bearer tech is  %s\n",data_bearer_status_tbl[i].str);
        break;
      }
    }
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_test_get_device_name(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_GET_DEVICE_NAME_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 16;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_get_device_name(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index =-1;
  char *buf = NULL;
  int  len = 15;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if(!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }

  if (call_index >= 0 && call_index<QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl )
  {
    buf = (char *)malloc(sizeof(char) * len);
    if (!buf)
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Failed to allocate memory for device name buffer!\n",buf);
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    if (qapi_DSS_Get_Device_Name((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, buf, len) == 0)
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Device name is %s \n",buf);
 
      if (buf != NULL)
        free(buf);
    }
    else
    {
      if (buf != NULL)
        free(buf);

      QCLI_Printf(qcli_dss_netapp_handle,"Error while getting device name \n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_test_get_qmi_port_name(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_GET_QMI_PORT_NAME_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 17;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_get_qmi_port_name(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index =-1;
  char *buf = NULL;
  int  len = 15;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }

  if (call_index >= 0 && call_index<QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl)
  {    
    buf = (char *)malloc(sizeof(char) * len);
    if (!buf)
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Failed to allocate memory for port name buffer!\n",buf);
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    if (qapi_DSS_Get_Qmi_Port_Name((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, buf,len) == 0)
    {
      QCLI_Printf(qcli_dss_netapp_handle,"QMI port name is %s \n",buf);

      if (buf != NULL)
        free(buf);
    }
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Error while getting qmi port name \n");

      if(buf != NULL)
        free(buf);

      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}
#endif
static QCLI_Command_Status_t 
dss_test_get_ip_addr_count(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_GET_IP_ADDR_COUNT_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
#ifndef MINI_QCLI
  cmd_parser_temp->Command_Index = 18;
#else
  cmd_parser_temp->Command_Index = 10;
#endif

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_get_ip_addr_count(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index =-1;
  unsigned int cnt = 0;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }

  if (call_index >= 0 && call_index<QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl )
  {
    qapi_DSS_Get_IP_Addr_Count((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, &cnt);
    QCLI_Printf(qcli_dss_netapp_handle, "IP address count is %u\n",cnt);
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_test_get_ip_addr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_GET_IP_ADDR_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
#ifndef MINI_QCLI
  cmd_parser_temp->Command_Index = 19;
#else
  cmd_parser_temp->Command_Index = 11;
#endif

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_get_ip_addr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index =-1;
  qapi_DSS_Addr_Info_t *info_ptr = NULL;
  char ip_v4[16];
  char ip_v6[130];
  char ip_v4_sub[16];
  char gt_v4[16];
  char gt_v6[130];
  char dp_v4[16];
  char ds_v4[16];
  char dp_v6[130];
  char ds_v6[130];
  int len;
  int ip_family = 0;
  qapi_DSS_Addr_Info_t *v4_info_ptr = NULL; 
  qapi_DSS_Addr_Info_t *v6_info_ptr = NULL; 
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }

  if (call_index >=0 && call_index < QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl)
  {
    qapi_DSS_Get_IP_Addr_Count((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, (unsigned int*)&len);
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  info_ptr = (qapi_DSS_Addr_Info_t *)malloc(sizeof(qapi_DSS_Addr_Info_t)*len);
  if (NULL == info_ptr) 
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Allocating memory for IP address information failed!!\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  memset(info_ptr, 0, (sizeof(qapi_DSS_Addr_Info_t)*len));

  if (qapi_DSS_Get_IP_Addr((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl,
                           (qapi_DSS_Addr_Info_t  *)info_ptr, len) == 0)
  {
    boolean default_family = TRUE; 

    /* IPv4 address is valid */ 
    v4_info_ptr = info_ptr + ip_family;
    if ((v4_info_ptr->iface_addr_s.valid_addr) &&  
        ((strcmp(dss_net_hndl[app_id][call_index].family,"IPV4")==0) || (strcmp(dss_net_hndl[app_id][call_index].family,"IPV4V6") == 0)))
    {
      ip_family++;
      default_family = FALSE;        

      QCLI_Printf(qcli_dss_netapp_handle,"V4 ip address is %s \n V4 subnet mask address %s \n V4 gateway address %s \n Primary DNS V4 %s \n Secondary DNS V4 %s \n"
                                        ,inet_ntop(AF_INET,&v4_info_ptr->iface_addr_s.addr.v4,ip_v4,sizeof(ip_v4))
                                        ,inet_ntop(AF_INET,&v4_info_ptr->iface_mask,ip_v4_sub,sizeof(ip_v4_sub))
                                        ,inet_ntop(AF_INET,&v4_info_ptr->gtwy_addr_s.addr.v4,gt_v4,sizeof(gt_v4))
                                        ,inet_ntop(AF_INET,&v4_info_ptr->dnsp_addr_s.addr.v4,dp_v4,sizeof(dp_v4))
                                        ,inet_ntop(AF_INET,&v4_info_ptr->dnss_addr_s.addr.v4,ds_v4,sizeof(ds_v4)));
    }
 
    v6_info_ptr = info_ptr + ip_family; 
    if ((v6_info_ptr->iface_addr_s.valid_addr) &&
        ((strcmp(dss_net_hndl[app_id][call_index].family,"IPV6")==0) || (strcmp(dss_net_hndl[app_id][call_index].family,"IPV4V6") == 0)))
    {
      default_family = FALSE;        
      QCLI_Printf(qcli_dss_netapp_handle,"V6 address is %s \n V6 prefix length %u \n V6 gateway address %s \n v6 gateway length %u \n Primary DNS V6 %s \n Secondary DNS V6 %s \n"
                                        ,inet_ntop(AF_INET6,&v6_info_ptr->iface_addr_s.addr.v6_addr64,ip_v6,sizeof(ip_v6))
                                        ,info_ptr->iface_mask
                                        ,inet_ntop(AF_INET6,&v6_info_ptr->gtwy_addr_s.addr.v6_addr64,gt_v6,sizeof(gt_v6))
                                        ,info_ptr->gtwy_mask
                                        ,inet_ntop(AF_INET6,&v6_info_ptr->dnsp_addr_s.addr.v6_addr64,dp_v6,sizeof(dp_v6))
                                        ,inet_ntop(AF_INET6,&v6_info_ptr->dnss_addr_s.addr.v6_addr64,ds_v6,sizeof(ds_v6)));       
    }

    /* Default IP family might be configured - IPv4 family by default */
    if (default_family)
    {
      /* IPv4 family is configured by default */ 
      v4_info_ptr = info_ptr + ip_family;
      if (v4_info_ptr->iface_addr_s.valid_addr)
      {
        QCLI_Printf(qcli_dss_netapp_handle,"V4 ip address is %s \n V4 subnet mask address %s \n V4 gateway address %s \n Primary DNS V4 %s \n Secondary DNS V4 %s \n"
                                          ,inet_ntop(AF_INET,&v4_info_ptr->iface_addr_s.addr.v4,ip_v4,sizeof(ip_v4))
                                          ,inet_ntop(AF_INET,&v4_info_ptr->iface_mask,ip_v4_sub,sizeof(ip_v4_sub))
                                          ,inet_ntop(AF_INET,&v4_info_ptr->gtwy_addr_s.addr.v4,gt_v4,sizeof(gt_v4))
                                          ,inet_ntop(AF_INET,&v4_info_ptr->dnsp_addr_s.addr.v4,dp_v4,sizeof(dp_v4))
                                          ,inet_ntop(AF_INET,&v4_info_ptr->dnss_addr_s.addr.v4,ds_v4,sizeof(ds_v4)));
      }
    }
  }
  else
  {
    if (info_ptr != NULL)
      free(info_ptr);
     
    QCLI_Printf(qcli_dss_netapp_handle, "Cannot obtain ip address");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  
  if (info_ptr != NULL)
    free(info_ptr);

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}
#ifndef MINI_QCLI
static QCLI_Command_Status_t 
dss_test_get_link_mtu(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_GET_LINK_MTU_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 20;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_get_link_mtu(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index = -1;
  int ip_family = -1;
  unsigned int mtu = 0;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }
  if (Parameter_Count > 1)
  {
    if (strcmp((char*)Parameter_List[1].String_Value,"V4")==0)
    {
      ip_family = QAPI_DSS_IP_FAMILY_V4_E;
    }
    else if (strcmp((char*)Parameter_List[1].String_Value,"V6")==0)
    {
      ip_family = QAPI_DSS_IP_FAMILY_V6_E;
    }
    else if (strcmp((char*)Parameter_List[1].String_Value,"NON_IP")==0)
    {
      ip_family = QAPI_DSS_NON_IP_FAMILY_E;
    }
    else 
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Invalid IP family!");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
  }

  if (call_index >= 0 && call_index<QAPI_DSS_MAX_DATA_CALLS && dss_net_hndl[app_id][call_index].call_hndl)
  {
    if(ip_family == -1)
    {
      if (qapi_DSS_Get_Link_Mtu(dss_net_hndl[app_id][call_index].call_hndl, &mtu) == 0)
      {
        QCLI_Printf(qcli_dss_netapp_handle, "MTU is %u \n", mtu);
      }
      else
      {
        QCLI_Printf(qcli_dss_netapp_handle, "Failed to obtain the MTU information!!");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
    else
    {
      if (qapi_DSS_Get_Link_Mtu_per_family(dss_net_hndl[app_id][call_index].call_hndl, ip_family, &mtu) == 0)
      {
        QCLI_Printf(qcli_dss_netapp_handle, "MTU is %u \n", mtu);
      }
      else
      {
        QCLI_Printf(qcli_dss_netapp_handle, "Failed to obtain the MTU information!!");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}
#endif
static QCLI_Command_Status_t 
dss_test_send_nipd_data(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_SEND_NIPD_DATA_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
#ifndef MINI_QCLI
  cmd_parser_temp->Command_Index = 21;
#else
  cmd_parser_temp->Command_Index = 12;
#endif

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_send_nipd_data(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index = -1;
  int input_data_len = 0;
  int i = 0, cnt = 0;
  unsigned int nipdMTU = 0;
  char *data = NULL;
  uint32_t packets = 0;
  uint8_t ex_data = 0; 
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 4 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }
  
  if ((call_index >= 0) && (call_index < QAPI_DSS_MAX_DATA_CALLS) && (dss_net_hndl[app_id][call_index].call_hndl))
  {
    if (Parameter_List[1].Integer_Is_Valid)
    {
      input_data_len = Parameter_List[2].Integer_Value;
    }
  
    if (qapi_DSS_Get_Link_Mtu((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl,
                               (unsigned int *)&nipdMTU) == 0)
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Maximum non-ip data capability is %d bytes\n", nipdMTU);
    }
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Querying maximum non-ip data capability failed!\n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
  
    if (input_data_len > nipdMTU)
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Input data length greater than maximum allowed limit of %d bytes\n", nipdMTU);
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    /* Non-IP packet data type */
    if (strcmp((char*)Parameter_List[3].String_Value,"ex_nonip")==0)
    {
      ex_data = QAPI_DSS_MO_EXCEPTION_NONIP_DATA;      
    }   
    else if (strcmp((char*)Parameter_List[3].String_Value,"nonip")==0)
    {
      ex_data = QAPI_DSS_MO_EXCEPTION_NONE;      
    }   
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Invalid option for non-ip exception type!!\n");
      result = QCLI_STATUS_ERROR_E;    
      goto end;
    }

    /* Number of non-ip data packets */
    packets = Parameter_List[1].Integer_Value;

    data = (char *)malloc(input_data_len * sizeof(char));
    if (!data)
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Allocating memory for non-ip data buffer failed!\n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
  
    for (i = 0; i < input_data_len; i++)
    {
      if (cnt == 9)
        cnt = 0; 
  
      data[i]= cnt++;
    }

    for (cnt = 0; cnt < packets; cnt++)
    {
      if (qapi_DSS_Nipd_Send((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
	  	(void *)data, input_data_len, ex_data) == 0)
      {
        QCLI_Printf(qcli_dss_netapp_handle, "Non-ip UL data sent successfully!\n");
      }
      else
      {
        if (data != NULL)
          free(data);

        QCLI_Printf(qcli_dss_netapp_handle, "Failed to send non-ip UL data(packet id :%d)!\n", cnt + 1);
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
  
    if (data != NULL)
      free(data);
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid!\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
dss_test_add_filters(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_ADD_FILTERS_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
#ifndef MINI_QCLI
  cmd_parser_temp->Command_Index = 22;
#else
  cmd_parser_temp->Command_Index = 13;
#endif

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_add_filters(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index = -1;
  QCLI_Command_Status_t reslt = QCLI_STATUS_ERROR_E;
  qapi_DSS_Add_MO_Exception_Filters_Req_t *filter_req = NULL; 
  qapi_DSS_Add_MO_Exception_Filters_Rsp_t *filter_rsp = NULL;
  int i;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    goto filter_fail;
  }

  if (Parameter_Count < 11 || Parameter_List == NULL)
  {
    reslt = QCLI_STATUS_USAGE_E;
    goto filter_fail;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }
   
  if ((call_index >= 0) && (call_index < QAPI_DSS_MAX_DATA_CALLS) && (dss_net_hndl[app_id][call_index].call_hndl))
  {
    if (dss_net_hndl[app_id][call_index].filter_stats.filter_handles_len >= QAPI_DSS_MAX_EXCEPTION_FILTERS)
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Adding filter rule not allowed - maximum limit reached\n");
      goto filter_fail;
    }

    /* Allocate resources for the filter requests */
    filter_req = (qapi_DSS_Add_MO_Exception_Filters_Req_t *)malloc(sizeof(qapi_DSS_Add_MO_Exception_Filters_Req_t));
    if (!filter_req)
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Failed to allocate memory for adding filter requests!\n");
      goto filter_fail;
    }

    filter_rsp = (qapi_DSS_Add_MO_Exception_Filters_Rsp_t *)malloc(sizeof(qapi_DSS_Add_MO_Exception_Filters_Rsp_t));
    if (!filter_rsp)
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Failed to allocate memory for adding filter response!\n");
      goto filter_fail;
    }

    memset(filter_req, 0x00, sizeof(qapi_DSS_Add_MO_Exception_Filters_Req_t));
    memset(filter_rsp, 0x00, sizeof(qapi_DSS_Add_MO_Exception_Filters_Rsp_t));
    
    /* Add MO exception IP filters - support for adding only one rule at a time */ 
    filter_req->filter_rules_valid = TRUE; 
    filter_req->filter_rules_len = 0x01;

    /* IPv4 */ 
    if (strcmp((char*)Parameter_List[1].String_Value, "-v4")==0)
    {
      uint32_t ipv4_addr = 0;

      filter_req->filter_rules[0].ip_info.ip_version = QAPI_DSS_IP_VERSION_4;

      /* Source address information */ 
      if (atoi((char*)Parameter_List[2].String_Value) != 0)
      {
        /* IPv4 address (network order) */
        if ((inet_pton(AF_INET, (const char *)Parameter_List[2].String_Value, &ipv4_addr)) != 0)
        {
          QCLI_Printf(qcli_dss_netapp_handle, "Incorrect IPv4 source address %s\n", Parameter_List[2].String_Value);
          goto filter_fail;
        }

        filter_req->filter_rules[0].ip_info.v4_info.src_addr.ipv4_addr = ntohl(ipv4_addr);

        /* IPv4 subnet mask (network order) */
        ipv4_addr = 0; 
        if ((inet_pton(AF_INET, (const char *)Parameter_List[3].String_Value, &ipv4_addr)) != 0)
        {
          QCLI_Printf(qcli_dss_netapp_handle, "Incorrect IPv4 source subnet mask %s\n", Parameter_List[3].String_Value);
          goto filter_fail;
        } 

        filter_req->filter_rules[0].ip_info.v4_info.src_addr.subnet_mask = ntohl(ipv4_addr);
        filter_req->filter_rules[0].ip_info.v4_info.valid_params |= QAPI_DSS_IPV4_FILTER_MASK_SRC_ADDR;
      }

      /* Destination address information */ 
      if (atoi((char*)Parameter_List[4].String_Value) != 0)
      {
        /* IPv4 address (network order) */
        ipv4_addr = 0;
        if ((inet_pton(AF_INET, (const char *)Parameter_List[4].String_Value, &ipv4_addr)) != 0)
        {
          QCLI_Printf(qcli_dss_netapp_handle, "Incorrect IPv4 destination address %s\n", Parameter_List[4].String_Value);
          goto filter_fail;
        }

        filter_req->filter_rules[0].ip_info.v4_info.dest_addr.ipv4_addr = ntohl(ipv4_addr);

        /* IPv4 subnet mask (network order) */ 
        ipv4_addr = 0;
        if ((inet_pton(AF_INET, (const char *)Parameter_List[5].String_Value, &ipv4_addr)) != 0)
        {
          QCLI_Printf(qcli_dss_netapp_handle, "Incorrect IPv4 destination subnet mask %s\n", Parameter_List[5].String_Value);
          goto filter_fail;
        }

        filter_req->filter_rules[0].ip_info.v4_info.dest_addr.subnet_mask = ntohl(ipv4_addr);
        filter_req->filter_rules[0].ip_info.v4_info.valid_params |= QAPI_DSS_IPV4_FILTER_MASK_DEST_ADDR;
      }
    }
    /* IPv6 */ 
    else if (strcmp((char *)Parameter_List[1].String_Value, "-v6")==0)
    {
      uint8_t cnt = 0; 
      uint8_t ipv6_addr[QAPI_DSS_IPV6_ADDR_LEN];

      filter_req->filter_rules[0].ip_info.ip_version = QAPI_DSS_IP_VERSION_6;

      /* Source address information */ 
      if (strcmp((char *)Parameter_List[2].String_Value, "0")!=0)
      {
        memset(ipv6_addr, 0x00, sizeof(ipv6_addr));  

        /* IPv6 source address (network order) */
        if ((inet_pton(AF_INET6, (const char *)Parameter_List[2].String_Value, &ipv6_addr)) != 0)
        {
          QCLI_Printf(qcli_dss_netapp_handle, "Incorrect IPv6 source address %s\n", Parameter_List[2].String_Value);
          goto filter_fail;
        }

        /* IPv6 source address (host order) */
        for (cnt = 0; cnt < QAPI_DSS_IPV6_ADDR_LEN; cnt++)
        {
          filter_req->filter_rules[0].ip_info.v6_info.src_addr.ipv6_address[cnt] = ipv6_addr[cnt];
        }
        
        /* IPv6 source prefix length */ 
        filter_req->filter_rules[0].ip_info.v6_info.src_addr.prefix_len = atoi((char*)Parameter_List[3].String_Value);
        filter_req->filter_rules[0].ip_info.v6_info.valid_params |= QAPI_DSS_IPV6_FILTER_MASK_SRC_ADDR;
      }     

      /* Destination address information */       
      if (strcmp((char *)Parameter_List[4].String_Value, "0")!=0)
      {
        memset(ipv6_addr, 0x00, sizeof(ipv6_addr));  

        /* IPv6 destination address (network order) */
        if ((inet_pton(AF_INET6, (const char *)Parameter_List[4].String_Value, &ipv6_addr)) != 0)
        {
          QCLI_Printf(qcli_dss_netapp_handle, "Incorrect IPv6 destination address %s\n", Parameter_List[2].String_Value);
          goto filter_fail;
        }
    
        /* IPv6 destination address (host order) */
        for (cnt = 0; cnt < QAPI_DSS_IPV6_ADDR_LEN; cnt++)
        {
          filter_req->filter_rules[0].ip_info.v6_info.dest_addr.ipv6_address[cnt] = ipv6_addr[cnt];
        }
     
        /* IPv6 destination prefix length */ 
        filter_req->filter_rules[0].ip_info.v6_info.dest_addr.prefix_len = atoi((char*)Parameter_List[5].String_Value);
        filter_req->filter_rules[0].ip_info.v6_info.valid_params |= QAPI_DSS_IPV6_FILTER_MASK_DEST_ADDR;
      }
    }
    else 
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Invalid option - not supported!!\n");
      
      reslt = QCLI_STATUS_USAGE_E;
      goto filter_fail;
    }
  
    /* TCP */ 
    if (strcmp((char*)Parameter_List[6].String_Value, "-tcp")==0)
    {
      filter_req->filter_rules[0].xport_info.xport_protocol = QAPI_DSS_XPORT_PROTO_TCP;

      /* Source port information */ 
      if (atoi((char*)Parameter_List[7].String_Value) != 0)
      {
        filter_req->filter_rules[0].xport_info.tcp_info.src_port_info.port = atoi((char*)Parameter_List[7].String_Value);
        filter_req->filter_rules[0].xport_info.tcp_info.src_port_info.range = atoi((char*)Parameter_List[8].String_Value);
        filter_req->filter_rules[0].xport_info.tcp_info.valid_params |= QAPI_DSS_PORT_INFO_FILTER_MASK_SRC_PORT;
      }      

      /* Destination port information */ 
      if (atoi((char*)Parameter_List[9].String_Value) != 0)
      {
        filter_req->filter_rules[0].xport_info.tcp_info.dest_port_info.port = atoi((char*)Parameter_List[9].String_Value);
        filter_req->filter_rules[0].xport_info.tcp_info.dest_port_info.range = atoi((char*)Parameter_List[10].String_Value);
        filter_req->filter_rules[0].xport_info.tcp_info.valid_params |= QAPI_DSS_PORT_INFO_FILTER_MASK_DEST_PORT;
      }  
    }
    /* UDP */ 
    else if (strcmp((char*)Parameter_List[6].String_Value, "-udp")==0)
    {
      filter_req->filter_rules[0].xport_info.xport_protocol = QAPI_DSS_XPORT_PROTO_UDP;
     
      /* Source port information */ 
      if (atoi((char*)Parameter_List[7].String_Value) != 0)
      {
        filter_req->filter_rules[0].xport_info.udp_info.src_port_info.port = atoi((char*)Parameter_List[7].String_Value);
        filter_req->filter_rules[0].xport_info.udp_info.src_port_info.range = atoi((char*)Parameter_List[8].String_Value);
        filter_req->filter_rules[0].xport_info.udp_info.valid_params |= QAPI_DSS_PORT_INFO_FILTER_MASK_SRC_PORT;
      }     
     
      /* Destination port information */ 
      if (atoi((char*)Parameter_List[9].String_Value) != 0)
      {
        filter_req->filter_rules[0].xport_info.udp_info.dest_port_info.port = atoi((char*)Parameter_List[9].String_Value);
        filter_req->filter_rules[0].xport_info.udp_info.dest_port_info.range = atoi((char*)Parameter_List[10].String_Value);
        filter_req->filter_rules[0].xport_info.udp_info.valid_params |= QAPI_DSS_PORT_INFO_FILTER_MASK_DEST_PORT;
      }   
    }
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Invalid option - not supported!!\n");
      
      reslt = QCLI_STATUS_USAGE_E;
      goto filter_fail;
    }

    /* Add MO exception IP filters */ 
    if (qapi_DSS_Add_MO_Exception_IPdata_Filters((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl, 
		(qapi_DSS_Add_MO_Exception_Filters_Req_t *)filter_req, (qapi_DSS_Add_MO_Exception_Filters_Rsp_t *)filter_rsp) == 0)
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Added filters for MO exception IP filters\n");

      /* Error adding MO exception IP filters */ 
      if (filter_rsp->filter_rule_error_valid)
      {
        QCLI_Printf(qcli_dss_netapp_handle,"Failed to add MO exception IP filters(0x%x)!!\n", filter_rsp->filter_rule_error[0]);
        goto filter_fail;
      }
      
      /* Added MO exception IP filters successfully */ 
      if (filter_rsp->filter_handles_valid)
      {
        uint32_t cnt = 0;

        dss_net_hndl[app_id][call_index].filter_stats.filter_handles_valid = TRUE; 

        /* Find a valid slot for adding the filter indentifier */
        for (cnt = 0; cnt < QAPI_DSS_MAX_EXCEPTION_FILTERS; cnt++)
        {
          /* Valid slot found for adding the filter identifier */ 
          if (!dss_net_hndl[app_id][call_index].filter_stats.filter_handles[cnt])
          {
            QCLI_Printf(qcli_dss_netapp_handle,"Added MO exception IP filter rule at index(0x%x)!!\n", cnt);

            dss_net_hndl[app_id][call_index].filter_stats.filter_handles[cnt] = filter_rsp->filter_handles[0];
            dss_net_hndl[app_id][call_index].filter_stats.filter_handles_len++;
            break;
          }
        }
      }
    }
    else 
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Failed to add MO exception IP filters!!\n");
      goto filter_fail;
    }

    reslt = QCLI_STATUS_SUCCESS_E;
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid!\n");
    goto filter_fail;
  }

filter_fail:
  if (filter_req)
    free(filter_req);

  if (filter_rsp)
    free(filter_rsp);

  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return reslt;
}

static QCLI_Command_Status_t 
dss_test_remove_filters(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_REMOVE_FILTERS_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
#ifndef MINI_QCLI
  cmd_parser_temp->Command_Index = 23;
#else
  cmd_parser_temp->Command_Index = 14;
#endif

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_test_remove_filters(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index = -1;
  QCLI_Command_Status_t reslt = QCLI_STATUS_ERROR_E;
  qapi_DSS_Remove_MO_Exception_Filters_t *filter_req = NULL;
  int i;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    goto filter_fail;
  }

  if (Parameter_Count < 2 || Parameter_List == NULL)
  {
    reslt = QCLI_STATUS_USAGE_E;
    goto filter_fail;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }
   
  if ((call_index >= 0) && (call_index < QAPI_DSS_MAX_DATA_CALLS) && (dss_net_hndl[app_id][call_index].call_hndl))
  {
    int32_t filter_indx = atoi((char*)Parameter_List[1].String_Value);

    if ((filter_indx < 0) || (filter_indx >= QAPI_DSS_MAX_EXCEPTION_FILTERS) || (!dss_net_hndl[app_id][call_index].filter_stats.filter_handles[filter_indx])) 
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Invalid filter index passed to remove filter rule(%d)!!\n", filter_indx);
      goto filter_fail;
    }

    filter_req = (qapi_DSS_Remove_MO_Exception_Filters_t *)malloc(sizeof(qapi_DSS_Remove_MO_Exception_Filters_t));
    if (!filter_req)
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Failed to allocate memory for adding filter response!\n");
      goto filter_fail;
    }

    memset(filter_req, 0x00, sizeof(qapi_DSS_Remove_MO_Exception_Filters_t));

    /* Remove MO exception IP filters - support for removing only one rule at a time */ 
    filter_req->filter_handles_len = 0x01;
    filter_req->filter_handles[0] = dss_net_hndl[app_id][call_index].filter_stats.filter_handles[filter_indx];
    if (qapi_DSS_Remove_MO_Exception_IPdata_Filters((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl,
		(qapi_DSS_Remove_MO_Exception_Filters_t *)filter_req) == 0)
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Removed filters for MO exception IP filters at index (%d)\n", filter_indx);

      dss_net_hndl[app_id][call_index].filter_stats.filter_handles[filter_indx] = 0x00;
      dss_net_hndl[app_id][call_index].filter_stats.filter_handles_len--; 
    }
    else 
    {
      QCLI_Printf(qcli_dss_netapp_handle,"Failed to remove MO exception IP filters rule!!\n");
      goto filter_fail;
    }

    reslt = QCLI_STATUS_SUCCESS_E;
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid!\n");
    goto filter_fail;
  }

filter_fail:
  if (filter_req)
    free(filter_req);

  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return reslt;
}
#ifndef MINI_QCLI
static QCLI_Command_Status_t 
nipd_data_tx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = NIPD_DATA_TX_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 24;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_nipd_data_tx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  
  int call_index = -1;
  int input_data_len = 0;
  int i = 0, cnt = 0;
  unsigned int nipdMTU = 0;
  char *data = NULL;
  uint32_t packets = 0;
  uint8_t ex_data = 0;
  uint8_t secure_conn = 0;
  int sock_hndl;
  int bytes_sent;
  int err;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  int dss_hndl;
  int sock_err;

  fd_set readfds;
  int num_ready_fds;
  int wait_duration = 30000;
  int bytes_recvd = 0;
  char * rcvd_data = NULL;

  qapi_Net_SSL_PSKTable_t psk;
  qapi_Net_SSL_Config_t cfg;
  qapi_Net_SSL_Obj_Hdl_t sslctx;
  qapi_Net_SSL_Con_Hdl_t sslconn;

  memset(&cfg, 0, sizeof(qapi_Net_SSL_Config_t) );
  cfg.verify.domain =  0;
  cfg.verify.match_Name[0] = '\0';
  cfg.verify.time_Validity = 1;
  cfg.verify.send_Alert = 1;
  cfg.max_Frag_Len = 1024;

  cfg.protocol = QAPI_NET_SSL_PROTOCOL_DTLS_1_2;
  cfg.cipher[0] = QAPI_NET_TLS_PSK_WITH_AES_128_CCM_8;
  psk = NIDD_PSK_FILE;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 5 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }
  
  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }
  
  if ((call_index >= 0) && (call_index < QAPI_DSS_MAX_DATA_CALLS) && (dss_net_hndl[app_id][call_index].call_hndl))
  {
    if (Parameter_List[1].Integer_Is_Valid)
    {
      input_data_len = Parameter_List[2].Integer_Value;
    }
  
    if (qapi_DSS_Get_Link_Mtu((qapi_DSS_Hndl_t)dss_net_hndl[app_id][call_index].call_hndl,
	                           (unsigned int *)&nipdMTU) == 0)
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Maximum non-ip data capability is %d bytes\n", nipdMTU);
    }
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Querying maximum non-ip data capability failed!\n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
  
    if (input_data_len > nipdMTU)
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Input data length greater than maximum allowed limit of %d bytes\n", nipdMTU);
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    /* Non-IP packet data type */
    if (strcmp((char*)Parameter_List[3].String_Value,"ex_nonip")==0)
    {
      ex_data = QAPI_DSS_MO_EXCEPTION_NONIP_DATA;      
    }   
    else if (strcmp((char*)Parameter_List[3].String_Value,"nonip")==0)
    {
      ex_data = QAPI_DSS_MO_EXCEPTION_NONE;      
    }   
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Invalid option for non-ip exception type!!\n");
      result = QCLI_STATUS_ERROR_E;    
      goto end;
    }
	if (strcmp((char*)Parameter_List[4].String_Value,"sec")==0)
    {
      secure_conn = 1;      
    }   
    else if (strcmp((char*)Parameter_List[4].String_Value,"no_sec")==0)
    {
      secure_conn = 0;       
    }   
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Invalid option for security type!!\n");
      result = QCLI_STATUS_ERROR_E;    
      goto end;
    }

	QCLI_Printf(qcli_dss_netapp_handle, "ex_data = %d\n", ex_data);
	QCLI_Printf(qcli_dss_netapp_handle, "secure_conn = %d\n", secure_conn);

    /* Number of non-ip data packets */
    packets = Parameter_List[1].Integer_Value;

    data = (char *)malloc(input_data_len * sizeof(char));
    if (!data)
    {
      QCLI_Printf(qcli_dss_netapp_handle, "Allocating memory for non-ip data buffer failed!\n");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

	rcvd_data = (char *)malloc(2048);
    if(rcvd_data == NULL)
    {
	  QCLI_Printf(qcli_dss_netapp_handle, "Failed to allocate memory\n");
	  result = QCLI_STATUS_ERROR_E;
	  free(data);
	  goto end;
    }

    for (i = 0; i < input_data_len; i++)
    {
      if (cnt == 8)
        cnt = 0; 
  
      data[i]= 'A' + cnt;
	  cnt++;
    }

	sock_hndl = qapi_socket(AF_NONIP, SOCK_NONIP, 0);
	if(sock_hndl == -1)
	{
	  QCLI_Printf(qcli_dss_netapp_handle, "Failed to create socket\n");
      free(data);
	  result = QCLI_STATUS_ERROR_E;
	  goto end;
	}

	dss_hndl = (int)dss_net_hndl[app_id][call_index].call_hndl;
	err = qapi_setsockopt(sock_hndl, SOL_SOCKET, SO_NONIP_DSSHNDL, &dss_hndl, sizeof(int *));
    if(err != 0)
	{
	  QCLI_Printf(qcli_dss_netapp_handle, "Failed to set socket option SO_NONIP_DSSHNDL\n");
	  err = qapi_getsockopt(sock_hndl, SOL_SOCKET, SO_ERROR, &sock_err, sizeof(int));
	  QCLI_Printf(qcli_dss_netapp_handle, "Get socket option SO_NONIP_DSSHNDL returned %d sock_err = %d\n", err, sock_err);
	  free(data);
	  qapi_socketclose(sock_hndl);
	  result = QCLI_STATUS_ERROR_E;
	  goto end;
	}
	
	err = qapi_setsockopt(sock_hndl, SOL_SOCKET, SO_NONIP_EXTYPE, &ex_data, sizeof(uint8_t));
    if(err == -1)
	{
	  QCLI_Printf(qcli_dss_netapp_handle, "Failed to set socket option SO_NONIP_EXTYPE\n");
      free(data);
	  qapi_socketclose(sock_hndl);
	  result = QCLI_STATUS_ERROR_E;
	  goto end;
	}

    if(secure_conn)
    {
      sslctx = qapi_Net_SSL_Obj_New(QAPI_NET_SSL_CLIENT_E);
      if(sslctx == QAPI_NET_SSL_INVALID_HANDLE) 
	  {
        QCLI_Printf(qcli_dss_netapp_handle, "qapi_Net_SSL_Obj_New Invalid Handle\n");
        free(data);
	    qapi_socketclose(sock_hndl);
	    result = QCLI_STATUS_ERROR_E;
        goto end;
      }
	  QCLI_Printf(qcli_dss_netapp_handle, "Loading cert %s\n", psk);
	  if(qapi_Net_SSL_Cert_Load(sslctx, QAPI_NET_SSL_PSK_TABLE_E, (char *)psk) < QAPI_OK)
	  {
	    QCLI_Printf(qcli_dss_netapp_handle, "qapi_Net_SSL_Cert_Load failed\n");
	    result = QCLI_STATUS_ERROR_E;
        goto error_ssl_conn;
	  }
	  
	  sslconn = qapi_Net_SSL_Con_New(sslctx, QAPI_NET_SSL_DTLS_E);

      if(sslconn == QAPI_NET_SSL_INVALID_HANDLE)
      {
        QCLI_Printf(qcli_dss_netapp_handle, "qapi_Net_SSL_Con_New Invalid Handle\n");
	    result = QCLI_STATUS_ERROR_E;
        goto error_ssl_conn;
      }
	  err = qapi_Net_SSL_Configure(sslconn, &cfg);
      if(err < QAPI_OK)
      {
        QCLI_Printf(qcli_dss_netapp_handle,"SSL configure failed (%d)\r\n", err);
        qapi_Net_SSL_Shutdown(sslconn);
		result = QCLI_STATUS_ERROR_E;
        goto error_ssl_conn;
      }

      err = qapi_Net_SSL_Fd_Set(sslconn, sock_hndl);      

      if(err < QAPI_OK)
      {
        QCLI_Printf(qcli_dss_netapp_handle,"qapi_Net_SSL_Fd_Set Failed! closing socket %d\n",sock_hndl);
        qapi_Net_SSL_Shutdown(sslconn);
		result = QCLI_STATUS_ERROR_E;
        goto error_ssl_conn;
      }
      err = qapi_Net_SSL_Connect(sslconn);
      if(err < QAPI_OK)
      {
        if(err == QAPI_SSL_OK_HS)
        {
          QCLI_Printf(qcli_dss_netapp_handle,"The certificate is trusted\n");
        }
        else if (err == QAPI_ERR_SSL_CERT_CN)
        {
          /** The peer's SSL certificate is trusted, CN matches the host name, time is expired */
          QCLI_Printf(qcli_dss_netapp_handle,"The certificate is expired,closing socket %d\n",sock_hndl);
          qapi_Net_SSL_Shutdown(sslconn);
		  result = QCLI_STATUS_ERROR_E;
          goto error_ssl_conn;
        }
        else if (err == QAPI_ERR_SSL_CERT_TIME)
        {
          /** The peer's SSL certificate is trusted, CN does NOT match the host name, time is valid */
          QCLI_Printf(qcli_dss_netapp_handle,"The certificate is trusted, but the host name is not valid,closing socket %d\n",sock_hndl);
          qapi_Net_SSL_Shutdown(sslconn);
		  result = QCLI_STATUS_ERROR_E;
          goto error_ssl_conn;
        }
        else if (err == QAPI_ERR_SSL_CERT_NONE)
        {
          /** The SSL certificate of the peer is not trusted */
          QCLI_Printf(qcli_dss_netapp_handle,"The SSL certificate of the peer is not trusted, closing socket %d\n",sock_hndl);
          qapi_Net_SSL_Shutdown(sslconn);
		  result = QCLI_STATUS_ERROR_E;
          goto error_ssl_conn;
        }
        else
        {
          QCLI_Printf(qcli_dss_netapp_handle,"SSL connect failed (%d),closing socket %d\n", err,sock_hndl);
          qapi_Net_SSL_Shutdown(sslconn);
		  result = QCLI_STATUS_ERROR_E;
          goto error_ssl_conn;
		  
        }
      }

	  for (cnt = 0; cnt < packets; cnt++)
      {
        if ((err = qapi_Net_SSL_Write(sslconn, (void *)data, input_data_len)) < QAPI_OK)
        {
          QCLI_Printf(qcli_dss_netapp_handle, "Failed to send non-ip SSL UL data(packet id :%d ERR %d)! Closing socket\n", cnt + 1,err);
          result = QCLI_STATUS_ERROR_E;
          goto error_ssl_conn;
        }
        else
        {
          QCLI_Printf(qcli_dss_netapp_handle, "\nSSL Non-ip UL data(packet id :%d) sent successfully!\n", cnt + 1);
		  //app_msec_delay(100);

		  FD_ZERO(&readfds);
	      FD_SET(sock_hndl, &readfds);
  
		  num_ready_fds = qapi_select(&readfds, NULL, NULL, wait_duration);
	      if(num_ready_fds == 0)
	      {
	        QCLI_Printf(qcli_dss_netapp_handle,"Select timedout. Send next packet and Wait on select again\n");
	      }
	      else if (num_ready_fds < 0)
	      {
	        QCLI_Printf(qcli_dss_netapp_handle,"Select Error\n");
			qapi_Net_SSL_Shutdown(sslconn);
		    result = QCLI_STATUS_ERROR_E;
            goto error_ssl_conn;
	      }
	      else
	      {
	         QCLI_Printf(qcli_dss_netapp_handle,"Select Unblocked. Recieve data\n");
		     memset(rcvd_data, 0, 2048);
		     if (FD_ISSET(sock_hndl, &readfds))
		     {
		       bytes_recvd = qapi_Net_SSL_Read(sslconn, rcvd_data, 2048);
		     }
		     if(bytes_recvd < 0)
		     {
		       QCLI_Printf(qcli_dss_netapp_handle,"Error:Recieve data len < 0. Closing socket\n");
		       qapi_Net_SSL_Shutdown(sslconn);
		       result = QCLI_STATUS_ERROR_E;
               goto error_ssl_conn;
		     }
		     else if(bytes_recvd == 0)
		     {
		       QCLI_Printf(qcli_dss_netapp_handle,"Error:Recieve data len = 0.Dropping\n");
		       qapi_recv(sock_hndl, rcvd_data, 2048,0);
		     }
		     else
		     {
		       QCLI_Printf(qcli_dss_netapp_handle,"Recieve data len = %d.\n", bytes_recvd);
		       QCLI_Printf(qcli_dss_netapp_handle,"%s\n\n", rcvd_data);
		     }
	       }
        }
      }
	  
	  qapi_Net_SSL_Shutdown(sslconn);
	  
	  error_ssl_conn:
          qapi_Net_SSL_Obj_Free(sslctx);
          qapi_socketclose(sock_hndl);
          sslctx = QAPI_NET_SSL_INVALID_HANDLE;
          free(data);
	  
    }
    else
    {
      for (cnt = 0; cnt < packets; cnt++)
      {
        if ((bytes_sent = qapi_send(sock_hndl, (void *)data, input_data_len, 0)) != input_data_len)
        {
          free(data);
          QCLI_Printf(qcli_dss_netapp_handle, "Failed to send non-ip UL data(packet id :%d)! Closing socket\n", cnt + 1);
		  qapi_socketclose(sock_hndl);
          result = QCLI_STATUS_ERROR_E;
          goto end;
        }
        else
        {
          QCLI_Printf(qcli_dss_netapp_handle, "%d bytes Non-ip UL data(packet id :%d) sent successfully!\n", bytes_sent, cnt + 1);
		  //app_msec_delay(100);
		  
          num_ready_fds = qapi_select(&readfds, NULL, NULL, wait_duration);
	      if(num_ready_fds == 0)
	      {
	        QCLI_Printf(qcli_dss_netapp_handle,"Select timedout. Send next packet and Wait on select again\n");
	      }
	      else if (num_ready_fds < 0)
	      {
	        QCLI_Printf(qcli_dss_netapp_handle,"Select Error\n");
			qapi_socketclose(sock_hndl);
	        result = QCLI_STATUS_ERROR_E;
		  	free(data);
	        goto end;
	      }
	      else
	      {
	         QCLI_Printf(qcli_dss_netapp_handle,"Select Unblocked. Recieve data\n");
		     memset(rcvd_data, 0, 2048);
		     if (FD_ISSET(sock_hndl, &readfds))
		     {
		       bytes_recvd = qapi_recv(sock_hndl, rcvd_data, 2048,0);
		     }
		     if(bytes_recvd < 0)
		     {
		       QCLI_Printf(qcli_dss_netapp_handle,"Error:Recieve data len < 0. Closing socket\n");
		       qapi_socketclose(sock_hndl);
	           result = QCLI_STATUS_ERROR_E;
		  	   free(data);
	           goto end;
		     }
		     else if(bytes_recvd == 0)
		     {
		       QCLI_Printf(qcli_dss_netapp_handle,"Error:Recieve data len = 0.Dropping\n");
		       qapi_recv(sock_hndl, rcvd_data, 2048,0);
		     }
		     else
		     {
		       QCLI_Printf(qcli_dss_netapp_handle,"Recieve data len > 0.\n");
		       QCLI_Printf(qcli_dss_netapp_handle,"\n%s\n", rcvd_data);
		     }
	       }
        }
      }
  
      free(data);
	  qapi_socketclose(sock_hndl);
    }
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid!\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }


end:
	if(rcvd_data != NULL)
	free(rcvd_data);
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t 
nipd_data_rx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = NIPD_DATA_RX_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 25;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_nipd_data_rx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  
  int call_index = -1;
  int sock_hndl;
  fd_set readfds; 
  int err;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  int num_ready_fds;
  int cnt = 5;
  int wait_duration = 10000;
  int bytes_recvd = 0;
  char * data = NULL;
  int dss_hndl;
  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }
  
  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }
  
  if ((call_index >= 0) && (call_index < QAPI_DSS_MAX_DATA_CALLS) && (dss_net_hndl[app_id][call_index].call_hndl))
  {
	sock_hndl = qapi_socket(AF_NONIP, SOCK_NONIP, 0);
	if(sock_hndl == -1)
	{
	  QCLI_Printf(qcli_dss_netapp_handle, "Failed to create socket\n");
	  result = QCLI_STATUS_ERROR_E;
	  goto end;
	}

	dss_hndl = (int)dss_net_hndl[app_id][call_index].call_hndl;
	err = qapi_setsockopt(sock_hndl, SOL_SOCKET, SO_NONIP_DSSHNDL, &dss_hndl, sizeof(int *));
    if(err == -1)
	{
	  QCLI_Printf(qcli_dss_netapp_handle, "Failed to set socket option\n");
	  qapi_socketclose(sock_hndl);
	  result = QCLI_STATUS_ERROR_E;
	  goto end;
	}

	FD_ZERO(&readfds);
    FD_SET(sock_hndl, &readfds);

    data = (char *)malloc(2048);

	if(data == NULL)
	{
	  QCLI_Printf(qcli_dss_netapp_handle, "Failed to allocate memory\n");
	  qapi_socketclose(sock_hndl);
	  result = QCLI_STATUS_ERROR_E;
	  goto end;
	}
	
	while(cnt)
	{
      num_ready_fds = qapi_select(&readfds, NULL, NULL, wait_duration);
	  if(num_ready_fds == 0)
	  {
	    QCLI_Printf(qcli_dss_netapp_handle,"Select timedout. Wait on select again\n");
	  }
	  else if (num_ready_fds < 0)
	  {
	    QCLI_Printf(qcli_dss_netapp_handle,"Select Error\n");
	  }
	  else
	  {
	    QCLI_Printf(qcli_dss_netapp_handle,"Select Unblocked. Recieve data\n");
		memset(data, 0, 2048);
		if (FD_ISSET(sock_hndl, &readfds))
		{
		  bytes_recvd = qapi_recv(sock_hndl, data, 2048,0);
		}
		if(bytes_recvd < 0)
		{
		  QCLI_Printf(qcli_dss_netapp_handle,"Error:Recieve data len < 0. Closing socket\n");
		  qapi_socketclose(sock_hndl);
	      result = QCLI_STATUS_ERROR_E;
		  free(data);
	      goto end;
		}
		else if(bytes_recvd == 0)
		{
		  QCLI_Printf(qcli_dss_netapp_handle,"Error:Recieve data len = 0.Dropping\n");
		  qapi_recv(sock_hndl, data, 2048,0);
		}
		else
		{
		  QCLI_Printf(qcli_dss_netapp_handle,"Recieve data len > 0.\n");
		  QCLI_Printf(qcli_dss_netapp_handle,"%s\n", data);
		}
		cnt--;
	  }
	}
    free(data);
	qapi_socketclose(sock_hndl);
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid!\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}
#endif


static QCLI_Command_Status_t 
dss_set_data_transfer_status_v2(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_SET_TRANSFER_STAT_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 27;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_set_data_transfer_status_v2(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{

  int call_index = -1;
  int i;
  int status = 0;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  qapi_Set_transfer_Status_Info_t status_info[3];
  int length = 0;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 2 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }
  
  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  } 
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Call index not valid!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  for (i = 1 ; i < Parameter_Count; i++)
  {
     if(strcmp((char*)Parameter_List[i].String_Value, "-s" ) == 0)
     {
       ++i;
       if(Parameter_List[i].Integer_Is_Valid)
       {
          status = Parameter_List[i].Integer_Value;
          status_info[length].id = QAPI_SET_TRANSFER_STATUS_RAI; 
          status_info[length].info_type = QAPI_SET_STATUS_TYPE_INTEGER_E;
          status_info[length].u.valueint = status;
          length ++;
       }
       else
       {
         QCLI_Printf(qcli_dss_netapp_handle , "Invalid Message id value\n");
         result = QCLI_STATUS_USAGE_E;
         goto end;
       }
     }
     else if (strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
     {
       ++i;
       if(Parameter_List[i].Integer_Is_Valid)
       {
		  status_info[length].id = QAPI_SET_NEXT_ACTIVITY_TIMER; 
		  status_info[length].info_type = QAPI_SET_STATUS_TYPE_INTEGER_E;
		  status_info[length].u.valueint = Parameter_List[i].Integer_Value;
		  length ++;
       }
       else
       {
         QCLI_Printf(qcli_dss_netapp_handle , "Invalid Message id value\n");
         result = QCLI_STATUS_USAGE_E;
         goto end;
       }
     }
   }

   if (QAPI_DSS_SUCCESS != qapi_DSS_Set_Transfer_Status_v2(dss_net_hndl[app_id][call_index].call_hndl , status_info , length))
   {
     QCLI_Printf(qcli_dss_netapp_handle,"Failed to set the status\n");
     result = QCLI_STATUS_ERROR_E;
     goto end;
   }
   else
   {
     QCLI_Printf(qcli_dss_netapp_handle,"Successfully set the status\n");
     result = QCLI_STATUS_SUCCESS_E;
     goto end;
   }

end:

if (Parameter_List != NULL)
{
   for (i = 0; i<Parameter_Count; i++)
   {
     if (Parameter_List[i].String_Value != NULL)
     {
       free(Parameter_List[i].String_Value);
       Parameter_List[i].String_Value = NULL;
     }
   }
}

return result;

}

static QCLI_Command_Status_t 
dss_set_data_transfer_status(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DSS_TEST_SET_TRANSFER_STAT_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&dss_netapp_cmd_list[0];
  cmd_parser_temp->Command_Index = 26;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_dss_set_data_transfer_status(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int call_index = -1;
  int i;
  int status = 0;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (!dss_lib_inited[app_id])
  {
    QCLI_Printf(qcli_dss_netapp_handle, "dss lib not initialized!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if (Parameter_Count < 2 || Parameter_List == NULL)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    call_index = Parameter_List[0].Integer_Value-1;
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle, "Call index not valid!! \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if ((call_index >= 0) && (call_index < QAPI_DSS_MAX_DATA_CALLS) && (dss_net_hndl[app_id][call_index].call_hndl))
  {
    if (Parameter_List[1].Integer_Is_Valid)
    {
      status = Parameter_List[1].Integer_Value;
      if((status < 0)|| (status > 2))
      {
         QCLI_Printf(qcli_dss_netapp_handle,"Invalid Status \n %d ",status);
         result = QCLI_STATUS_ERROR_E;
         goto end;
      }
    }

    if (QAPI_DSS_SUCCESS!= qapi_DSS_Set_Transfer_Status(dss_net_hndl[app_id][call_index].call_hndl ,status))
    {
      QCLI_Printf(qcli_dss_netapp_handle,"\n Failed to set the status");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }
    else
    {
      QCLI_Printf(qcli_dss_netapp_handle,"\n Successfully set the status \n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }
  }
  else
  {
    QCLI_Printf(qcli_dss_netapp_handle,"Call index is invalid or call handle is invalid\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

end:
  if (Parameter_List!= NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}


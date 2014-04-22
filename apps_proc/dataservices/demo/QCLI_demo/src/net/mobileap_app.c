/*!
  @file
  mobileap_app.c

  @brief
  MobileAP client functionality
*/

/*===========================================================================

  Copyright (c) 2021 Qualcomm Technologies, Inc. All Rights Reserved

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
#include "mobileap_app.h"
#include "qcmap.h"
#include "qcmap_api.h"
#include "timer.h"
#include "qapi_ns_utils.h"

#define MAX_STATUS      2
#define MAX_RESULT      2
#define MAX_ERROR       7
#define MAX_CONN_STATUS 12
#define MAX_CE          7

static QCLI_Command_Status_t mobileap_client_reg(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t enable_mobile_ap(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t disable_mobile_ap(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t enable_backhaul(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t disable_backhaul(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t get_autoconnect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t set_autoconnect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t get_roaming(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t set_roaming(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t set_loopback(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t delete_DMZ(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t set_DMZ(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t set_dhcp(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);



 extern client_data_s *client_data_t;
 extern qcmap_file qcmap_file_t;
 extern qcmap_connection_manager *qc_mgr;
 extern qcmap_backhaul_wwan *qc_bck_wan;
 extern unsigned int modem_loopback_mode;
 boolean client_reg = false;
 boolean client_reg_cb = FALSE;
 boolean mobileap_enable = FALSE;

 mobile_ap_resp mobile_ap_type;

const QCLI_Command_t mobileap_cmd_list[] =
{
    /* Command_Function, Start_Thread, Command_String, Usage_String, Description, Abort_function */
    {mobileap_client_reg, false, "regclient", "", "Registers the client with QCMAP ",NULL},
    {enable_mobile_ap, false, "enableap", "", "Enable Mobile AP",NULL},
    {disable_mobile_ap, false, "disableap", "", "Disable Mobile AP",NULL},
    {enable_backhaul, false, "enablebackhaul", " <family> ", "Enable Backhaul ",NULL},
    {disable_backhaul, false, "disablebackhaul", " <family> ", "Disable Backhaul ",NULL},
    {get_autoconnect, false, "getautoconnect", "", "Get autoconnect flag",NULL},
    {set_autoconnect, false, "setautoconnect", "<0/1>", "Set autoconnect flag",NULL},
    {get_roaming, false, "getroaming", "", "Get roaming flag",NULL},
    {set_roaming, false, "setroaming", "<0/1>", "Set roaming flag",NULL},
    {set_DMZ, false, "setDMZIP", "<DMZ IP>", "Set DMZ IP",NULL},
    {delete_DMZ, false, "deleteDMZIP", "<DMZ IP>", "Delete DMZ IP",NULL},
    {set_loopback, false, "setloopback", "<0/1>", "Set/clear loopback mode",NULL},
    {set_dhcp, false, "setdhcp", "<0/1>", "Disable/Enable DHCP",NULL},

};

extern client_data_s *client_data_t;
const QCLI_Command_Group_t mobileap_cmd_group =
{
    "mobileap",              /* Group_String: will display cmd prompt as "Mobileap> " */
    sizeof(mobileap_cmd_list)/sizeof(mobileap_cmd_list[0]),   /* Command_Count */
    mobileap_cmd_list        /* Command_List */
};

QCLI_Group_Handle_t qcli_mobileap_handle;     /* Handle for Net Command Group. */

/*****************************************************************************
 * This function is used to register the MobileAP Command Group with QCLI.
 *****************************************************************************/
void Initialize_MobileAP_Demo(void)
{
    /* Attempt to reqister the Command Groups with the qcli framework.*/
    qcli_mobileap_handle = QCLI_Register_Command_Group(NULL, &mobileap_cmd_group);
    if (qcli_mobileap_handle)
    {
      QCLI_Printf(qcli_mobileap_handle, "MobileAP Registered\n");
    }

    return;
}

struct mobile_ap_status_strings_s
{
  qcmap_msgr_mobile_ap_status_enum_v01 status;
  const char* str;
};

struct mobile_ap_status_strings_s mobile_ap_status_string_tbl[MAX_STATUS] =
{
  {QCMAP_MSGR_MOBILE_AP_STATUS_CONNECTED_V01, "Connected"},
  {QCMAP_MSGR_MOBILE_AP_STATUS_DISCONNECTED_V01, "Disconnected"}
};

struct mobile_ap_result_strings_s
{
  qcmap_result_type result;
  const char* str;
};

struct mobile_ap_result_strings_s mobile_ap_result_string_tbl[MAX_RESULT] =
{
  {QCMAP_RESULT_SUCCESS, "Success"},
  {QCMAP_RESULT_FAILURE, "Failure"}
};

struct mobile_ap_error_strings_s
{
  qcmap_error_type error;
  const char* str;
};

struct mobile_ap_error_strings_s mobile_ap_error_string_tbl[MAX_ERROR] =
{
  {QCMAP_ERR_NONE, "No Error"},
  {QCMAP_ERR_INTERNAL, "Internal error"},
  {QCMAP_ERR_INVALID_HANDLE, "Invalid Handle"},
  {QCMAP_ERR_NO_NETWORK_FOUND, "No network found"},
  {QCMAP_ERR_NO_EFFECT, "No effect"},
  {QCMAP_ERR_INTERFACE_NOT_FOUND, "Interface not found"},
  {QCMAP_ERR_INVALID_IP_FAMILY_PREF, "Invalid ip family prefernce"}
};

struct mobile_ap_conn_status_strings_s
{
  qcmap_msgr_wwan_status_enum_v01 conn_status;
  const char* str;
};

struct mobile_ap_conn_status_strings_s mobile_ap_conn_status_string_tbl[MAX_CONN_STATUS] =
{
  {QCMAP_MSGR_WWAN_STATUS_CONNECTING_V01, "IPV4 Connecting state"},
  {QCMAP_MSGR_WWAN_STATUS_CONNECTING_FAIL_V01, "IPV4 Connecting failed"},
  {QCMAP_MSGR_WWAN_STATUS_CONNECTED_V01, "IPV4 Connected state"},
  {QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_V01, "IPV4 Disconnected state"},
  {QCMAP_MSGR_WWAN_STATUS_DISCONNECTING_FAIL_V01, "IPV4 Disconnecting failed"},
  {QCMAP_MSGR_WWAN_STATUS_DISCONNECTED_V01, "IPV4 Disconnected state"},
  {QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_V01, "IPV6 Connecting state"},
  {QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTING_FAIL_V01, "IPV6 Connecting failed"},
  {QCMAP_MSGR_WWAN_STATUS_IPV6_CONNECTED_V01, "IPV6 Connected state"},
  {QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_V01, "IPV6 Disconnected state"},
  {QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTING_FAIL_V01, "IPV6 Disconnecting failed"},
  {QCMAP_MSGR_WWAN_STATUS_IPV6_DISCONNECTED_V01, "IPV6 Disconnected state"}
};

struct mobile_ap_call_end_strings_s
{
  qcmap_msgr_wwan_call_end_type_enum_v01 call_end;
  const char* str;
};

struct mobile_ap_call_end_strings_s mobile_ap_call_end_string_tbl[MAX_CE] =
{
  {QCMAP_MSGR_WWAN_CALL_END_TYPE_MOBILE_IP_V01,"Mobile IP type"},
  {QCMAP_MSGR_WWAN_CALL_END_TYPE_INTERNAL_V01,"Internal type"},
  {QCMAP_MSGR_WWAN_CALL_END_TYPE_CALL_MANAGER_DEFINED_V01,"Call manager defined type"},
  {QCMAP_MSGR_WWAN_CALL_END_TYPE_3GPP_SPEC_DEFINED_V01,"3GPP spec defined type"},
  {QCMAP_MSGR_WWAN_CALL_END_TYPE_PPP_V01,"PPP type"},
  {QCMAP_MSGR_WWAN_CALL_END_TYPE_EHRPD_V01,"EHPRD type"},
  {QCMAP_MSGR_WWAN_CALL_END_TYPE_IPV6_V01,"IPV6 type"}
};

void qcmap_client_cb(uint8 evt, void* data, uint8 idx)
{
  int i;
  qcmap_msgr_mobile_ap_enable_resp_msg_v01 *enable_resp;
  qcmap_msgr_mobile_ap_status_ind_msg_v01  *enable_ind;
  qcmap_msgr_mobile_ap_disable_resp_msg_v01 *disable_resp;
  qcmap_msgr_bring_up_wwan_resp_msg_v01 *wwan_bringup;
  qcmap_msgr_tear_down_wwan_resp_msg_v01 *wwan_teardown;
  qcmap_msgr_bring_up_wwan_ind_msg_v01  *wwan_bringup_ind;
  qcmap_msgr_tear_down_wwan_ind_msg_v01  *wwan_teardown_ind;
  qcmap_msgr_set_roaming_pref_resp_msg_v01 *set_roaming;
  qcmap_msgr_get_roaming_pref_resp_msg_v01 *get_roaming;
  qcmap_msgr_get_auto_connect_resp_msg_v01 *get_autoconnect;
  qcmap_msgr_set_auto_connect_resp_msg_v01 *set_autoconnect;
  qcmap_msgr_set_dmz_resp_msg_v01          *set_dmz;
  qcmap_msgr_delete_dmz_resp_msg_v01       *delete_dmz;
  qcmap_msgr_set_dhcp_resp_msg_v01 *set_dhcp;

  switch(evt)
  {
   case QCMAP_MOBILEAP_ENABLE_RESP_EVT:
        enable_resp = (qcmap_msgr_mobile_ap_enable_resp_msg_v01 *)data;
        mobile_ap_type.resp.result = enable_resp->resp.result;
        mobile_ap_type.resp.error  = enable_resp->resp.error;
        for(i=0; i< MAX_RESULT; i++)
        {
          if(mobile_ap_result_string_tbl[i].result == mobile_ap_type.resp.result)
          {
            QCLI_Printf(qcli_mobileap_handle, "Mobile AP enable result is %s \n ",
                        mobile_ap_result_string_tbl[i].str);
             break;
          }
        }
        if(mobile_ap_type.resp.result == QCMAP_RESULT_FAILURE)
        {
          for(i=0; i< MAX_ERROR; i++)
          {
            if(mobile_ap_error_string_tbl[i].error == mobile_ap_type.resp.error)
            {
              QCLI_Printf(qcli_mobileap_handle, "Error in Mobile AP enable is %s \n ",
                          mobile_ap_error_string_tbl[i].str);
               break;
            }
          }
        }

        if(mobile_ap_type.resp.result == QCMAP_RESULT_SUCCESS)
        {
          mobileap_enable = TRUE;
        }

        mobile_ap_type.mobile_ap_handle_valid = TRUE;
        mobile_ap_type.mobile_ap_handle = enable_resp->mobile_ap_handle;
        QCLI_Printf(qcli_mobileap_handle, "Mobile ap handle is %x for client %d \n"
                    ,mobile_ap_type.mobile_ap_handle, mobile_ap_type.conn_handle);

        break;

   case QCMAP_MOBILEAP_ENABLE_IND_EVT:
         enable_ind = (qcmap_msgr_mobile_ap_status_ind_msg_v01 *)data;
         mobile_ap_type.mobile_ap_handle = enable_ind->mobile_ap_handle;
         mobile_ap_type.mobile_ap_status = enable_ind->mobile_ap_status;
         for(i=0; i< MAX_STATUS; i++)
         {
           if(mobile_ap_status_string_tbl[i].status == mobile_ap_type.mobile_ap_status)
           {
             QCLI_Printf(qcli_mobileap_handle, "Mobile AP enable status is %s \n ",
                         mobile_ap_status_string_tbl[i].str);
              break;
           }
         }

        break;

   case QCMAP_MOBILEAP_DISABLE_RESP_EVT:
        disable_resp = (qcmap_msgr_mobile_ap_disable_resp_msg_v01 *)data;
        mobile_ap_type.resp.result = disable_resp->resp.result;
        mobile_ap_type.resp.error  = disable_resp->resp.error;
        for(i=0; i< MAX_RESULT; i++)
        {
          if(mobile_ap_result_string_tbl[i].result == mobile_ap_type.resp.result)
          {
            QCLI_Printf(qcli_mobileap_handle, "Mobile AP disable result is %s \n ",
                        mobile_ap_result_string_tbl[i].str);
             break;
          }
        }
        if(mobile_ap_type.resp.result == QCMAP_RESULT_FAILURE)
        {
          for(i=0; i< MAX_ERROR; i++)
          {
            if(mobile_ap_error_string_tbl[i].error == mobile_ap_type.resp.error)
            {
              QCLI_Printf(qcli_mobileap_handle, "Error in Mobile AP disable is %s \n ",
                          mobile_ap_error_string_tbl[i].str);
               break;
            }
          }
        }

        if(mobile_ap_type.resp.result == QCMAP_RESULT_SUCCESS)
        {
          mobileap_enable = FALSE;
        }

       break;

   case QCMAP_MOBILEAP_BRING_UP_WWAN_RESP_EVT:
        wwan_bringup = (qcmap_msgr_bring_up_wwan_resp_msg_v01 *)data;
        mobile_ap_type.resp.result = wwan_bringup->resp.result;
        mobile_ap_type.resp.error  = wwan_bringup->resp.error;
        mobile_ap_type.conn_status_valid = TRUE;
        mobile_ap_type.conn_status = wwan_bringup->conn_status;
        for(i=0; i< MAX_CONN_STATUS; i++)
        {
          if(mobile_ap_conn_status_string_tbl[i].conn_status== mobile_ap_type.conn_status)
          {
            QCLI_Printf(qcli_mobileap_handle, "Backhaul connection is in %s \n ",
                        mobile_ap_conn_status_string_tbl[i].str);
             break;
          }
        }

       break;

   case QCMAP_MOBILEAP_TEAR_DOWN_WWAN_RESP_EVT:
        wwan_teardown = (qcmap_msgr_tear_down_wwan_resp_msg_v01 *)data;
        mobile_ap_type.resp.result = wwan_teardown->resp.result;
        mobile_ap_type.resp.error  = wwan_teardown->resp.error;
        mobile_ap_type.conn_status_valid = TRUE;
        mobile_ap_type.conn_status = wwan_teardown->conn_status;
        for(i=0; i< MAX_CONN_STATUS; i++)
        {
          if(mobile_ap_conn_status_string_tbl[i].conn_status== mobile_ap_type.conn_status)
          {
            QCLI_Printf(qcli_mobileap_handle, "Backhaul connection is in %s \n ",
                        mobile_ap_conn_status_string_tbl[i].str);
             break;
          }
        }

       break;

   case QCMAP_MOBILEAP_BRING_UP_WWAN_IND_EVT:
        wwan_bringup_ind = (qcmap_msgr_bring_up_wwan_ind_msg_v01 *)data;
        mobile_ap_type.mobile_ap_handle = wwan_bringup_ind->mobile_ap_handle;
        mobile_ap_type.conn_status = wwan_bringup_ind->conn_status;
        mobile_ap_type.wwan_call_end_reason_valid = TRUE;
        mobile_ap_type.wwan_call_end_reason = wwan_bringup_ind->wwan_call_end_reason;
        for(i=0; i< MAX_CONN_STATUS; i++)
        {
          if(mobile_ap_conn_status_string_tbl[i].conn_status== mobile_ap_type.conn_status)
          {
            QCLI_Printf(qcli_mobileap_handle, "Backhaul connection is in %s \n ",
                        mobile_ap_conn_status_string_tbl[i].str);
             break;
          }
        }
        if(wwan_bringup_ind->wwan_call_end_reason_valid)
        {
          for(i=0; i< MAX_CE; i++)
          {
            if(mobile_ap_call_end_string_tbl[i].call_end == mobile_ap_type.wwan_call_end_reason.wwan_call_end_reason_type)
            {
              QCLI_Printf(qcli_mobileap_handle, "Call end reason in Backhaul connection is %s \n ",
                          mobile_ap_call_end_string_tbl[i].str);
               break;
            }
          }
        }

       break;

   case QCMAP_MOBILEAP_TEAR_DOWN_WWAN_IND_EVT:
        wwan_teardown_ind = (qcmap_msgr_tear_down_wwan_ind_msg_v01 *)data;
        mobile_ap_type.mobile_ap_handle = wwan_teardown_ind->mobile_ap_handle;
        mobile_ap_type.conn_status = wwan_teardown_ind->conn_status;
        mobile_ap_type.wwan_call_end_reason_valid = TRUE;
        mobile_ap_type.wwan_call_end_reason = wwan_teardown_ind->wwan_call_end_reason;
        for(i=0; i< MAX_CONN_STATUS; i++)
        {
          if(mobile_ap_conn_status_string_tbl[i].conn_status== mobile_ap_type.conn_status)
          {
            QCLI_Printf(qcli_mobileap_handle, "Backhaul connection is in %s \n ",
                        mobile_ap_conn_status_string_tbl[i].str);
             break;
          }
        }
        if(wwan_teardown_ind->wwan_call_end_reason_valid)
        {
          for(i=0; i< MAX_CE; i++)
          {
            if(mobile_ap_call_end_string_tbl[i].call_end == mobile_ap_type.wwan_call_end_reason.wwan_call_end_reason_type)
            {
              QCLI_Printf(qcli_mobileap_handle, "Call end reason in Backhaul connection is %s \n ",
                          mobile_ap_call_end_string_tbl[i].str);
               break;
            }
          }
        }

       break;

   case QCMAP_MOBILEAP_SET_AUTOCONNECT_RESP_EVT:
        set_autoconnect = (qcmap_msgr_set_auto_connect_resp_msg_v01 *)data;
        mobile_ap_type.resp.result = set_autoconnect->resp.result;
        mobile_ap_type.resp.error  = set_autoconnect->resp.error;
        if(mobile_ap_type.resp.result == QCMAP_RESULT_SUCCESS)
        {
          QCLI_Printf(qcli_mobileap_handle, "Set auto connect successful\n ");
        }

      break;

   case QCMAP_MOBILEAP_GET_AUTOCONNECT_RESP_EVT:
        get_autoconnect = (qcmap_msgr_get_auto_connect_resp_msg_v01 *)data;
        mobile_ap_type.resp.result = get_autoconnect->resp.result;
        mobile_ap_type.resp.error  = get_autoconnect->resp.error;
        if(mobile_ap_type.resp.result == QCMAP_RESULT_SUCCESS)
        {
          QCLI_Printf(qcli_mobileap_handle, "Auto connect flag is set to %d \n ",
                          qc_mgr->auto_connect);
        }

       break;

   case QCMAP_MOBILEAP_SET_ROAMING_RESP_EVT:
        set_roaming= (qcmap_msgr_set_roaming_pref_resp_msg_v01 *)data;
        mobile_ap_type.resp.result = set_roaming->resp.result;
        mobile_ap_type.resp.error  = set_roaming->resp.error;
        if(mobile_ap_type.resp.result == QCMAP_RESULT_SUCCESS)
        {
          QCLI_Printf(qcli_mobileap_handle, "Set roaming successful\n ");
        }

      break;

   case QCMAP_MOBILEAP_GET_ROAMING_RESP_EVT:
        get_roaming = (qcmap_msgr_get_roaming_pref_resp_msg_v01 *)data;
        mobile_ap_type.resp.result = get_roaming->resp.result;
        mobile_ap_type.resp.error  = get_roaming->resp.error;
        if(mobile_ap_type.resp.result == QCMAP_RESULT_SUCCESS)
        {
          QCLI_Printf(qcli_mobileap_handle, "Roaming flag is set to %d \n ",
                          qc_bck_wan->wwan_cfg.roaming );
        }

       break;
    case QCMAP_MOBILEAP_SET_DMZIP_RESP_EVT:
       set_dmz = (qcmap_msgr_set_dmz_resp_msg_v01 *)data;
       mobile_ap_type.resp.result = set_dmz->resp.result;
       mobile_ap_type.resp.error  = set_dmz->resp.error;

       if(mobile_ap_type.resp.result == QCMAP_RESULT_SUCCESS)
       {
         QCLI_Printf(qcli_mobileap_handle, "DMZ IP set successful \n ");
       }
       else if (mobile_ap_type.resp.result == QCMAP_RESULT_FAILURE)
       {
         QCLI_Printf(qcli_mobileap_handle, "DMZ IP set unsuccessful \n ");
       }
       break;

     case QCMAP_MOBILEAP_DELETE_DMZIP_RESP_EVT:
       delete_dmz = (qcmap_msgr_delete_dmz_resp_msg_v01 *)data;
       mobile_ap_type.resp.result = delete_dmz->resp.result;
       mobile_ap_type.resp.error  = delete_dmz->resp.error;

       if(mobile_ap_type.resp.result == QCMAP_RESULT_SUCCESS)
       {
         QCLI_Printf(qcli_mobileap_handle, "DMZ IP delete successful \n ");
       }
       else if (mobile_ap_type.resp.result == QCMAP_RESULT_FAILURE)
       {
         QCLI_Printf(qcli_mobileap_handle, "DMZ IP delete unsuccessful \n ");
       }
       break;

       case QCMAP_MOBILEAP_SET_DHCP_RESP_EVT:
        set_dhcp = (qcmap_msgr_set_dhcp_resp_msg_v01 *)data;
        mobile_ap_type.resp.result = set_dhcp->resp.result;
        mobile_ap_type.resp.error  = set_dhcp->resp.error;
        if(mobile_ap_type.resp.result == QCMAP_RESULT_SUCCESS)
        {
          QCLI_Printf(qcli_mobileap_handle, "Set dhcp successful\n ");
        }

      break;
  }

}


static QCLI_Command_Status_t mobileap_client_reg(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

  uint8 *connection_handle;

  if(!client_reg_cb)
   {

     connection_handle = (uint8 *)malloc(sizeof(uint8));
      if (connection_handle == NULL)
      {
        QCMAP_LOG_MSG_ERROR("connection_handle == NULL", 0, 0, 0);
        return QCLI_STATUS_ERROR_E;
      }
     memset(connection_handle, 0, sizeof(uint8));
     qcmap_client_reg_cb((qcmap_client_resp_ind_cb )qcmap_client_cb, connection_handle);

     if(connection_handle !=NULL)
     {
      client_reg_cb = TRUE;
      mobile_ap_type.conn_handle = *connection_handle;
      client_reg                 = true;
      QCLI_Printf(qcli_mobileap_handle, "\n CLient registered with connection handle %d ",*connection_handle);
     }
   
     else
     {
      client_reg_cb = FALSE;
      client_reg    = false;
      QCLI_Printf(qcli_mobileap_handle, "\n CLient not registered Try again ");
     }
    
     if(connection_handle != NULL)
     {
       free(connection_handle);
       connection_handle = NULL;
     }
   }

  else
   {
     QCLI_Printf(qcli_mobileap_handle, " CLient already registered");
      return QCLI_STATUS_SUCCESS_E;
   }

  return QCLI_STATUS_SUCCESS_E;

}

static QCLI_Command_Status_t 
enable_mobile_ap(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

  if(!client_reg)
  {
   QCLI_Printf(qcli_mobileap_handle, "Client not registered  Please register client");
   return QCLI_STATUS_ERROR_E;
  }

  if(mobileap_enable)
  {
   QCLI_Printf(qcli_mobileap_handle, "Mobile AP already enabled by same client.");
   return QCLI_STATUS_ERROR_E;
  }

  QCMAP_LOG_MSG_HIGH("qcli enabling qcmap ", 0, 0, 0);

  qcmap_evt_handler_cb(QCMAP_MOBILEAP_ENABLE_REQ_EVT, mobile_ap_type.data, mobile_ap_type.conn_handle);

  return QCLI_STATUS_SUCCESS_E;

}

static QCLI_Command_Status_t 
disable_mobile_ap(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  qcmap_msgr_mobile_ap_disable_req_msg_v01 *data;

  if(!client_reg)
  {
   QCLI_Printf(qcli_mobileap_handle, "Client not registered  Please register client");
   return QCLI_STATUS_ERROR_E;
  }

  if(!mobileap_enable)
  {
   QCLI_Printf(qcli_mobileap_handle, "Mobile AP not enabled.");
   return QCLI_STATUS_ERROR_E;
  }

  data = (qcmap_msgr_mobile_ap_disable_req_msg_v01 *)malloc(sizeof(qcmap_msgr_mobile_ap_disable_req_msg_v01));
   if (data == NULL)
  {
     QCMAP_LOG_MSG_ERROR("data == NULL", 0, 0, 0);
      return QCLI_STATUS_ERROR_E;
  }
  memset(data, 0, sizeof(qcmap_msgr_mobile_ap_disable_req_msg_v01));

  data->mobile_ap_handle = mobile_ap_type.mobile_ap_handle;

  QCMAP_LOG_MSG_HIGH("qcli disabling qcmap ", 0, 0, 0);

  qcmap_evt_handler_cb(QCMAP_MOBILEAP_DISABLE_REQ_EVT, (void*) data, mobile_ap_type.conn_handle);

  return QCLI_STATUS_SUCCESS_E;

}

static QCLI_Command_Status_t 
enable_backhaul(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  
  qcmap_msgr_bring_up_wwan_req_msg_v01 *data;

  if(!client_reg)
  {
   QCLI_Printf(qcli_mobileap_handle, "Client not registered  Please register client");
   return QCLI_STATUS_ERROR_E;
  }

  if(!mobileap_enable)
  {
   QCLI_Printf(qcli_mobileap_handle, "Mobile AP not enabled.");
   return QCLI_STATUS_ERROR_E;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    return QCLI_STATUS_USAGE_E;
  }

  data = (qcmap_msgr_bring_up_wwan_req_msg_v01 *)malloc(sizeof(qcmap_msgr_bring_up_wwan_req_msg_v01));
   if (data == NULL)
  {
     QCMAP_LOG_MSG_ERROR("data == NULL", 0, 0, 0);
      return QCLI_STATUS_ERROR_E;
  }
  memset(data, 0, sizeof(qcmap_msgr_bring_up_wwan_req_msg_v01));

  if(strcasecmp((char*)Parameter_List[0].String_Value,"v4")==0)
  {
     data->call_type_valid = TRUE;
     data->call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
  }
  else if(strcasecmp((char*)Parameter_List[0].String_Value,"v6")==0)
  {
     data->call_type_valid = TRUE;
     data->call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01;
  }
  else
  {
    QCMAP_LOG_MSG_ERROR("Not a valid family type ", 0, 0, 0);
    QCLI_Printf(qcli_mobileap_handle, "Not a valid family type ");
    return QCLI_STATUS_ERROR_E;
  }

  data->mobile_ap_handle = mobile_ap_type.mobile_ap_handle;

  QCMAP_LOG_MSG_HIGH("qcli enabling backhaul ", 0, 0, 0);

  qcmap_evt_handler_cb(QCMAP_MOBILEAP_BRING_UP_WWAN_REQ_EVT, (void*) data, mobile_ap_type.conn_handle);

  return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t 
disable_backhaul(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  qcmap_msgr_tear_down_wwan_req_msg_v01  *data;

  if(!client_reg)
  {
   QCLI_Printf(qcli_mobileap_handle, "Client not registered  Please register client");
   return QCLI_STATUS_ERROR_E;
  }

  if(!mobileap_enable)
  {
   QCLI_Printf(qcli_mobileap_handle, "Mobile AP not enabled.");
   return QCLI_STATUS_ERROR_E;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    return QCLI_STATUS_USAGE_E;
  }

  data = (qcmap_msgr_tear_down_wwan_req_msg_v01 *)malloc(sizeof(qcmap_msgr_tear_down_wwan_req_msg_v01));
   if (data == NULL)
  {
     QCMAP_LOG_MSG_ERROR("data == NULL", 0, 0, 0);
      return QCLI_STATUS_ERROR_E;
  }
  memset(data, 0, sizeof(qcmap_msgr_tear_down_wwan_req_msg_v01));

  if(strcasecmp((char*)Parameter_List[0].String_Value,"v4")==0)
  {
     data->call_type_valid = TRUE;
     data->call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01;
  }
  else if(strcasecmp((char*)Parameter_List[0].String_Value,"v6")==0)
  {
     data->call_type_valid = TRUE;
     data->call_type = QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01;
  }
  else
  {
    QCMAP_LOG_MSG_ERROR("Not a valid family type ", 0, 0, 0);
    QCLI_Printf(qcli_mobileap_handle, "Not a valid family type ");
    return QCLI_STATUS_ERROR_E;
  }

  data->mobile_ap_handle = mobile_ap_type.mobile_ap_handle;

  QCMAP_LOG_MSG_HIGH("qcli disabling backhaul ", 0, 0, 0);

  qcmap_evt_handler_cb(QCMAP_MOBILEAP_TEAR_DOWN_WWAN_REQ_EVT, (void *)data, mobile_ap_type.conn_handle);

  return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t 
get_autoconnect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  qcmap_msgr_get_auto_connect_req_msg_v01 *data;

  if(!client_reg)
  {
   QCLI_Printf(qcli_mobileap_handle, "Client not registered  Please register client");
   return QCLI_STATUS_ERROR_E;
  }

  if(!mobileap_enable)
  {
   QCLI_Printf(qcli_mobileap_handle, "Mobile AP not enabled.");
   return QCLI_STATUS_ERROR_E;
  }

  data = (qcmap_msgr_get_auto_connect_req_msg_v01 *)malloc(sizeof(qcmap_msgr_get_auto_connect_req_msg_v01));
   if (data == NULL)
  {
     QCMAP_LOG_MSG_ERROR("data == NULL", 0, 0, 0);
      return QCLI_STATUS_ERROR_E;
  }
  memset(data, 0, sizeof(qcmap_msgr_get_auto_connect_req_msg_v01));

  data->mobile_ap_handle = mobile_ap_type.mobile_ap_handle;

  QCMAP_LOG_MSG_HIGH("qcli get autoconnect ", 0, 0, 0);

  qcmap_evt_handler_cb(QCMAP_MOBILEAP_GET_AUTOCONNECT_REQ_EVT, (void *)data, mobile_ap_type.conn_handle);

  return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t 
set_autoconnect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  qcmap_msgr_set_auto_connect_req_msg_v01  *data;

  if(!client_reg)
  {
   QCLI_Printf(qcli_mobileap_handle, "Client not registered  Please register client");
   return QCLI_STATUS_ERROR_E;
  }

  if(!mobileap_enable)
  {
   QCLI_Printf(qcli_mobileap_handle, "Mobile AP not enabled.");
   return QCLI_STATUS_ERROR_E;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    return QCLI_STATUS_USAGE_E;
  }

  data = (qcmap_msgr_set_auto_connect_req_msg_v01 *)malloc(sizeof(qcmap_msgr_set_auto_connect_req_msg_v01));
   if (data == NULL)
  {
     QCMAP_LOG_MSG_ERROR("data == NULL", 0, 0, 0);
      return QCLI_STATUS_ERROR_E;
  }
  memset(data, 0, sizeof(qcmap_msgr_set_auto_connect_req_msg_v01));

  if(Parameter_List[0].Integer_Is_Valid && (Parameter_List[0].Integer_Value == 0 || Parameter_List[0].Integer_Value == 1))
  {
   data->enable = Parameter_List[0].Integer_Value;
  }

  else
  {
    QCLI_Printf(qcli_mobileap_handle, "Not a valid parameter for auto connect Check usage.");
    return QCLI_STATUS_USAGE_E;
  }

  data->mobile_ap_handle = mobile_ap_type.mobile_ap_handle;

  QCMAP_LOG_MSG_HIGH("qcli set autoconnect ", 0, 0, 0);

  qcmap_evt_handler_cb(QCMAP_MOBILEAP_SET_AUTOCONNECT_REQ_EVT, (void *)data, mobile_ap_type.conn_handle);

  return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t 
get_roaming(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  qcmap_msgr_get_roaming_pref_req_msg_v01  *data;

  if(!client_reg)
  {
   QCLI_Printf(qcli_mobileap_handle, "Client not registered  Please register client");
   return QCLI_STATUS_ERROR_E;
  }

  if(!mobileap_enable)
  {
   QCLI_Printf(qcli_mobileap_handle, "Mobile AP not enabled.");
   return QCLI_STATUS_ERROR_E;
  }

  data = (qcmap_msgr_get_roaming_pref_req_msg_v01 *)malloc(sizeof(qcmap_msgr_get_roaming_pref_req_msg_v01));
   if (data == NULL)
  {
     QCMAP_LOG_MSG_ERROR("data == NULL", 0, 0, 0);
      return QCLI_STATUS_ERROR_E;
  }
  memset(data, 0, sizeof(qcmap_msgr_get_roaming_pref_req_msg_v01));

  data->mobile_ap_handle = mobile_ap_type.mobile_ap_handle;

  QCMAP_LOG_MSG_HIGH("qcli get roaming ", 0, 0, 0);

  qcmap_evt_handler_cb(QCMAP_MOBILEAP_GET_ROAMING_REQ_EVT, (void *)data, mobile_ap_type.conn_handle);

  return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t 
set_roaming(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  qcmap_msgr_set_roaming_pref_req_msg_v01 *data;

  if(!client_reg)
  {
   QCLI_Printf(qcli_mobileap_handle, "Client not registered  Please register client");
   return QCLI_STATUS_ERROR_E;
  }

  if(!mobileap_enable)
  {
   QCLI_Printf(qcli_mobileap_handle, "Mobile AP not enabled.");
   return QCLI_STATUS_ERROR_E;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    return QCLI_STATUS_USAGE_E;
  }

  data = (qcmap_msgr_set_roaming_pref_req_msg_v01 *)malloc(sizeof(qcmap_msgr_set_roaming_pref_req_msg_v01));
   if (data == NULL)
  {
     QCMAP_LOG_MSG_ERROR("data == NULL", 0, 0, 0);
      return QCLI_STATUS_ERROR_E;
  }
  memset(data, 0, sizeof(qcmap_msgr_set_roaming_pref_req_msg_v01));

  if(Parameter_List[0].Integer_Is_Valid && (Parameter_List[0].Integer_Value == 0 || Parameter_List[0].Integer_Value == 1))
  {
   data->allow_wwan_calls_while_roaming = Parameter_List[0].Integer_Value;
  }

  else
  {
    QCLI_Printf(qcli_mobileap_handle, "Not a valid parameter for roaming Check usage.");
    return QCLI_STATUS_USAGE_E;
  }

  data->mobile_ap_handle               = mobile_ap_type.mobile_ap_handle;

  QCMAP_LOG_MSG_HIGH("qcli set roaming ", 0, 0, 0);

  qcmap_evt_handler_cb(QCMAP_MOBILEAP_SET_ROAMING_REQ_EVT, (void *)data, mobile_ap_type.conn_handle);

  return QCLI_STATUS_SUCCESS_E;
}


static QCLI_Command_Status_t 
set_DMZ(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  char ipaddr[20] = {0};
  struct ip46addr dmz_addr;
  qcmap_msgr_set_dmz_req_msg_v01 *data;

  if(!client_reg)
  {
   QCLI_Printf(qcli_mobileap_handle, "Client not registered  Please register client");
   return QCLI_STATUS_ERROR_E;
  }

  if(!mobileap_enable)
  {
   QCLI_Printf(qcli_mobileap_handle, "Mobile AP not enabled.");
   return QCLI_STATUS_ERROR_E;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    return QCLI_STATUS_USAGE_E;
  }

  strlcpy((char*)&ipaddr, (char*)Parameter_List[0].String_Value,sizeof(ipaddr));

  if (inet_pton(AF_INET, ipaddr, &dmz_addr.a) != 0)   /* not in xxx.xxx.xxx.xxx format */
  {
    if (strlen(ipaddr) > 32)
    {
      QCMAP_LOG_MSG_HIGH(" DMZ ip should not be more than 32 bytes ", 0, 0, 0);
      return QCLI_STATUS_ERROR_E;
    }
  }

  data = (qcmap_msgr_set_dmz_req_msg_v01 *)malloc(sizeof(qcmap_msgr_set_dmz_req_msg_v01));
  if (data == NULL)
  {
     QCMAP_LOG_MSG_ERROR("data == NULL", 0, 0, 0);
      return QCLI_STATUS_ERROR_E;
  }
  memset(data, 0, sizeof(qcmap_msgr_set_dmz_req_msg_v01));

  if(!Parameter_List[0].Integer_Is_Valid)
  {
    data->dmz_ip_addr = dmz_addr.a.addr4;
  }

  else
  {
    QCLI_Printf(qcli_mobileap_handle, "Not a valid parameter for DMZ usage.");
    return QCLI_STATUS_USAGE_E;
  }

  data->mobile_ap_handle               = mobile_ap_type.mobile_ap_handle;

  QCMAP_LOG_MSG_HIGH("qcli set DMZ ", 0, 0, 0);

  qcmap_evt_handler_cb(QCMAP_MOBILEAP_SET_DMZIP_REQ_EVT, (void *)data, mobile_ap_type.conn_handle);

  return QCLI_STATUS_SUCCESS_E;


}

static QCLI_Command_Status_t 
delete_DMZ(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  char ipaddr[20] = {0};
  struct ip46addr dmz_addr;
  qcmap_msgr_delete_dmz_req_msg_v01 *data;

  QCMAP_LOG_MSG_HIGH("qcli Delete DMZ ", 0, 0, 0);

  if(!client_reg)
  {
   QCLI_Printf(qcli_mobileap_handle, "Client not registered  Please register client");
   return QCLI_STATUS_ERROR_E;
  }

  if(!mobileap_enable)
  {
   QCLI_Printf(qcli_mobileap_handle, "Mobile AP not enabled.");
   return QCLI_STATUS_ERROR_E;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    return QCLI_STATUS_USAGE_E;
  }

  strlcpy((char*)&ipaddr, (char*)Parameter_List[0].String_Value,sizeof(ipaddr));

  if (inet_pton(AF_INET, ipaddr, &dmz_addr.a) != 0)   /* not in xxx.xxx.xxx.xxx format */
  {
    if (strlen(ipaddr) > 32)
    {
      QCMAP_LOG_MSG_ERROR(" DMZ ip should not be more than 32 bytes ", 0, 0, 0);
      return QCLI_STATUS_ERROR_E;
    }
  }

  data = (qcmap_msgr_delete_dmz_req_msg_v01 *)malloc(sizeof(qcmap_msgr_delete_dmz_req_msg_v01));
  if (data == NULL)
  {
     QCMAP_LOG_MSG_ERROR("data == NULL", 0, 0, 0);
      return QCLI_STATUS_ERROR_E;
  }
  memset(data, 0, sizeof(qcmap_msgr_delete_dmz_req_msg_v01));

  if(!Parameter_List[0].Integer_Is_Valid)
  {
    data->dmz_ip_addr = dmz_addr.a.addr4;
  }

  else
  {
    QCLI_Printf(qcli_mobileap_handle, "Not a valid parameter for DMZ usage.");
    return QCLI_STATUS_USAGE_E;
  }

  data->mobile_ap_handle               = mobile_ap_type.mobile_ap_handle;

  qcmap_evt_handler_cb(QCMAP_MOBILEAP_DELETE_DMZIP_REQ_EVT, (void *)data, mobile_ap_type.conn_handle);

  return QCLI_STATUS_SUCCESS_E;
}


static QCLI_Command_Status_t 
set_loopback(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    return QCLI_STATUS_USAGE_E;
  }
  
  if(Parameter_List[0].Integer_Is_Valid && (Parameter_List[0].Integer_Value == 0 || Parameter_List[0].Integer_Value == 1))
  {
   modem_loopback_mode = Parameter_List[0].Integer_Value;
  }

  else
  {
    QCLI_Printf(qcli_mobileap_handle, "Not a valid parameter for looback mode Check usage.");
    return QCLI_STATUS_USAGE_E;
  }

  QCLI_Printf(qcli_mobileap_handle, "Loopback mode is set to %d \n ",
                          modem_loopback_mode );
  return QCLI_STATUS_SUCCESS_E;
}

static QCLI_Command_Status_t 
set_dhcp(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  qcmap_msgr_set_dhcp_req_msg_v01  *data;

  if(!client_reg)
  {
   QCLI_Printf(qcli_mobileap_handle, "Client not registered  Please register client");
   return QCLI_STATUS_ERROR_E;
  }

  if(!mobileap_enable)
  {
   QCLI_Printf(qcli_mobileap_handle, "Mobile AP not enabled.");
   return QCLI_STATUS_ERROR_E;
  }

  if (Parameter_Count < 1 || Parameter_List == NULL)
  {
    return QCLI_STATUS_USAGE_E;
  }

  data = (qcmap_msgr_set_dhcp_req_msg_v01 *)malloc(sizeof(qcmap_msgr_set_dhcp_req_msg_v01));
   if (data == NULL)
  {
     QCMAP_LOG_MSG_ERROR("data == NULL", 0, 0, 0);
      return QCLI_STATUS_ERROR_E;
  }
  memset(data, 0, sizeof(qcmap_msgr_set_dhcp_req_msg_v01));

  if(Parameter_List[0].Integer_Is_Valid && (Parameter_List[0].Integer_Value == 0 || Parameter_List[0].Integer_Value == 1))
  {
   data->dhcp_enable = Parameter_List[0].Integer_Value;
  }

  else
  {
    QCLI_Printf(qcli_mobileap_handle, "Not a valid parameter for dhcp command Check usage.");
    return QCLI_STATUS_USAGE_E;
  }

  data->mobile_ap_handle = mobile_ap_type.mobile_ap_handle;

  QCMAP_LOG_MSG_HIGH("qcli set dhcp ", 0, 0, 0);

  qcmap_evt_handler_cb(QCMAP_MOBILEAP_SET_DHCP_REQ_EVT, (void *)data, mobile_ap_type.conn_handle);

  return QCLI_STATUS_SUCCESS_E;
}



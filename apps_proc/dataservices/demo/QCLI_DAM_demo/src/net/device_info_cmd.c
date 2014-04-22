/*!
  @file
  device_info_cmd.c

  @brief
  Implementation of Device Info command handler functionality.
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
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef QAPI_TXM_MODULE
#else
#include "stringl/stringl.h"
#endif

#include "qcli_api.h"
#include "qcli.h"
#include "pal.h"

#include "qapi_device_info.h"
#include "bench.h"

qapi_Device_Info_Hndl_t app_devinfo_cli_ctx[NO_OF_APPS] = {NULL};


extern QCLI_Command_t net_cmd_list[];
extern QCLI_Group_Handle_t qcli_net_handle; /* Handle for Net Command Group. */
#define DEVINFO_PRINTF(...) QCLI_Printf(qcli_net_handle, ##__VA_ARGS__)

#ifdef QAPI_TXM_MODULE
extern TX_BYTE_POOL *byte_pool_qcli;
#define MALLOC_MEMORY(a, b) tx_byte_allocate(byte_pool_qcli, (VOID **)&a, b, TX_NO_WAIT);
#define FREE_MEMORY(a)      tx_byte_release(a)
extern uint32_t memscpy(void   *dst, uint32_t   dst_size, const  void  *src,uint32_t   src_size);
#else
#define MALLOC_MEMORY(a, b) a = malloc(b);
#define FREE_MEMORY(a)      free(a)
#endif

typedef void (*PrintDeviceInfoFunc)(qapi_Device_Info_t *info, const char* testcase);
typedef QCLI_Command_Status_t (*cmd_handler)(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

typedef struct dev_info_command_s {
  const char* cmd_1;
  const char* cmd_2;
  cmd_handler func;
}dev_info_command_t;

typedef struct dev_info_get_command_s {
  const char* cmd_1;
  const char* cmd_2;
  qapi_Device_Info_ID_t info_id;
  const char* testcase;
  PrintDeviceInfoFunc print_func;
}dev_info_get_command_t;

typedef struct dev_info_set_command_s {
  const char* cmd_1;
  const char* cmd_2;
  qapi_Device_Info_ID_t info_id;
  const char* testcase;
}dev_info_set_command_t;

typedef struct dev_info_request_command_s {
  const char* cmd_1;
  const char* cmd_2;
  cmd_handler print_func;
}dev_info_request_command_t;

struct id_str {
  	 int16_t id;
	 const char* str;
};

#define ID_STR(id)  {id, #id},

void print_device_info_int(qapi_Device_Info_t *info, const char* testcase)
{
    if(info == NULL) return;
    DEVINFO_PRINTF("%s: %lld \n", testcase, info->u.valueint);
}

void print_device_info_bool(qapi_Device_Info_t *info, const char* testcase)
{
    if(info == NULL) return;
    DEVINFO_PRINTF("%s: %d \n", testcase, info->u.valuebool);
}

void print_device_info_str(qapi_Device_Info_t *info, const char* testcase)
{
    if(info == NULL) return;
    DEVINFO_PRINTF("%s:%s \n",testcase, info->u.valuebuf.buf);
}
void print_device_info_intbuf(qapi_Device_Info_t *info, const char* testcase)
{
  int p;

  if(info == NULL) return;

  for(p=0; p< info->u.valint.len; p++)
  {
      DEVINFO_PRINTF("%s[%d]: %d \n", testcase, p,info->u.valint.buf[p]);
  }
}
void print_device_info_hexbuf(qapi_Device_Info_t *info, const char* testcase)
{
  int p;

  if(info == NULL) return;

  for(p=0; p< info->u.valint.len; p++)
  {
      DEVINFO_PRINTF("%s[%d]: 0x%08x \n", testcase, p,info->u.valint.buf[p]);
  }
}

void print_device_info_battery(qapi_Device_Info_t *info, const char* testcase)
{
    if(info == NULL) return;

    if(info->u.valueint == 0)
       DEVINFO_PRINTF("%s:%d - Battery over_voltage\n",testcase, info->u.valueint);
    else if(info->u.valueint == 1)
       DEVINFO_PRINTF("%s:%d - low battery voltage\n", testcase, info->u.valueint);
    else if(info->u.valueint == 2)
       DEVINFO_PRINTF("%s:%d - battery is missing \n", testcase, info->u.valueint);
    else if(info->u.valueint == 3)
       DEVINFO_PRINTF("%s:%d - battery voltage is good\n",testcase, info->u.valueint);
}

void print_device_info_rccs(qapi_Device_Info_t *info, const char* testcase)
{
    if(info == NULL) return;

    if(info->u.valueint == 0)
       DEVINFO_PRINTF("%s: %lld - NAS_RRC_IDLE\n", testcase, info->u.valueint);
    else if(info->u.valueint == 1)
       DEVINFO_PRINTF("%s: %lld - NAS_RRC_CONNECTED\n", testcase, info->u.valueint);
}

void print_device_info_network_bearer(qapi_Device_Info_t *info, const char* testcase)
{
    if(info == NULL) return;

    if(info->u.valueint == 2)
        DEVINFO_PRINTF("%s:GSM - %lld \n", testcase, info->u.valueint);
    else if(info->u.valueint == 3)
        DEVINFO_PRINTF("%s:WCDMA - %lld \n", testcase, info->u.valueint);
    else if(info->u.valueint == 5)
        DEVINFO_PRINTF("%s: IOTAPP_LTE_TDD - %lld \n", testcase, info->u.valueint);
    else if(info->u.valueint == 6)
        DEVINFO_PRINTF("%s: IOTAPP_LTE_FDD - %lld \n", testcase, info->u.valueint);
    else
        DEVINFO_PRINTF("%s: %lld \n", testcase, info->u.valueint);
}

void print_device_info_geran_cell(qapi_Device_Info_t *info, const char* testcase)
{
     int i = 0;
     qapi_Cell_Info *c_info = NULL;
     if(info == NULL) return;

     c_info = (qapi_Cell_Info *)info->u.device_info_extn;
	 if(c_info == NULL) return;

     DEVINFO_PRINTF("cinfo_len[%d]\n", c_info->cinfo_len);
     for (i = 0; i < c_info->cinfo_len ;i++)
     {
        DEVINFO_PRINTF("ARFCN[%d]: %d\n",i, c_info->g_cinfo[i].arfcn);
        DEVINFO_PRINTF("BSIC[%d]: %d\n",i, c_info->g_cinfo[i].bsic);
        DEVINFO_PRINTF("LAC[%d]: %d\n",i, c_info->g_cinfo[i].lac);
        DEVINFO_PRINTF("MCC[%d]: %d\n",i, c_info->g_cinfo[i].mcc);
        DEVINFO_PRINTF("MNC[%d]: %d\n",i, c_info->g_cinfo[i].mnc);
        DEVINFO_PRINTF("Cell_Barred[%d]: %d\n",i, c_info->g_cinfo[i].cell_barred);
        DEVINFO_PRINTF("Cell_Id[%d]: %d\n",i, c_info->g_cinfo[i].cell_identity);
        DEVINFO_PRINTF("RxLev[%d]: %d\n",i, c_info->g_cinfo[i].rssi);
        DEVINFO_PRINTF("GPRS[%d]: %d\n",i, c_info->g_cinfo[i].gprs_support);
     }
     DEVINFO_PRINTF("get_device_info_gsm: success \n");

     if(c_info)
     {
       FREE_MEMORY(c_info);
       c_info = NULL;
     }

}

void print_device_info_lte_m1_n1_cell(qapi_Device_Info_t *info, const char* testcase)
{
     int i = 0,j=0;
     qapi_Cell_Info *c_info = NULL;
     if(info == NULL) return;

     c_info = (qapi_Cell_Info*)info->u.device_info_extn;
	 if(c_info == NULL) return;
     DEVINFO_PRINTF("cinfo_len[%d]\n", c_info->cinfo_len);
     for (i = 0; i < c_info->cinfo_len ;i++)
     {
        DEVINFO_PRINTF("EARFCN[%d]: %d\n",i, c_info->lte_cinfo[i].earfcn);
        DEVINFO_PRINTF("phy_cell_id[%d]: %d\n",i, c_info->lte_cinfo[i].phy_cell_id);
        DEVINFO_PRINTF("cell_id[%d]: %d\n",i, c_info->lte_cinfo[i].cell_id);
        DEVINFO_PRINTF("tac[%d]: %d\n",i, c_info->lte_cinfo[i].tac);
        DEVINFO_PRINTF("cell_barred[%d]: %d\n",i, c_info->lte_cinfo[i].cell_barred);
        DEVINFO_PRINTF("rsrp[%d]: %d\n",i, c_info->lte_cinfo[i].rsrp);
        DEVINFO_PRINTF("rsrq[%d]: %d\n",i, c_info->lte_cinfo[i].rsrq);
        DEVINFO_PRINTF("rssi[%d]: %d\n",i, c_info->lte_cinfo[i].rssi);
        DEVINFO_PRINTF("bandwidth[%d]: %d\n",i, c_info->lte_cinfo[i].bandwidth);
        DEVINFO_PRINTF("plmn_len[%d]\n", c_info->lte_cinfo[i].plmn_len);
        for(j = 0; j< c_info->lte_cinfo[i].plmn_len ; j++)
        {
           DEVINFO_PRINTF("mcc:plmn[%d][%d]: %d\n",i,j,c_info->lte_cinfo[i].mcc[j]);
           DEVINFO_PRINTF("mnc:plmn[%d][%d]: %d\n",i,j,c_info->lte_cinfo[i].mnc[j]);
        }
    }
    DEVINFO_PRINTF("%s: success \n", testcase);

    if(c_info)
    {
       FREE_MEMORY(c_info);
       c_info = NULL;
    }

}

void print_device_info_app_priority(qapi_Device_Info_t *info, const char* testcase)
{
    if(info == NULL) return;

    if(info->u.valueint == QAPI_DEVICE_INFO_GNSS_PRIORITY)
        DEVINFO_PRINTF("%s:GNSS clients application prioritized on the modem\n", testcase);
    else if(info->u.valueint == QAPI_DEVICE_INFO_WWAN_PRIORITY)
        DEVINFO_PRINTF("%s:WWAN clients application prioritized on the modem\n", testcase);
    else
        DEVINFO_PRINTF("%s:Invalid client group \n", testcase);
}

void print_device_info_loaded_tech(qapi_Device_Info_t *info, const char* testcase)
{
    if(info == NULL) return;

    if(info->u.valueint == QAPI_DEVICE_INFO_NONE)
       DEVINFO_PRINTF("%s: No technology in loaded/pending state\n", testcase);
    else if(info->u.valueint == QAPI_DEVICE_INFO_WWAN_PENDING)
       DEVINFO_PRINTF("%s: WWAN technology in pending state\n\n", testcase);
    else if(info->u.valueint == QAPI_DEVICE_INFO_GPS_PENDING)
       DEVINFO_PRINTF("%s: GPS technology in pending state\n", testcase);
    else if(info->u.valueint == QAPI_DEVICE_INFO_WWAN_LOADED)
       DEVINFO_PRINTF("%s: WWAN technology in loaded state\n", testcase);
    else if(info->u.valueint == QAPI_DEVICE_INFO_GPS_LOADED)
       DEVINFO_PRINTF("%s: GPS technology in loaded state\n", testcase);
    else
       DEVINFO_PRINTF("%s: Invalid state \n", testcase);
}

void print_device_info_nz_bsr(qapi_Device_Info_t *info, const char* testcase)
{
    if(info == NULL) return;

    if(info->u.valueint == 0)
        DEVINFO_PRINTF("%s:NZ BSR disabled\n", testcase);
    else if(info->u.valueint == 1)
        DEVINFO_PRINTF("%s:NZ BSR enabled\n", testcase);
    else
        DEVINFO_PRINTF("%s:Invalid NZ BSR value \n", testcase);
}

void printf_device_info_edrx(const char* str_print, const qapi_Device_Info_t *device_info)
{
  if(NULL == device_info)
  {
    QAPI_MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,"NULL == device_info");
    DEVINFO_PRINTF("%s: NULL == device_info\n", str_print);
    return;
  }
  if(device_info->id == QAPI_DEVICE_INFO_EDRX_E && device_info->u.device_info_extn != NULL)
  {
    qapi_Device_Info_Rsp_EDRX_Get_t * edrx_info = (qapi_Device_Info_Rsp_EDRX_Get_t *)(device_info->u.device_info_extn);
    DEVINFO_PRINTF("device_info->id:[%d][QAPI_DEVICE_INFO_EDRX_E]\n", QAPI_DEVICE_INFO_EDRX_E);
    DEVINFO_PRINTF("%s: QAPI_DEVICE_INFO_TYPE_EDRX_INFO_E Val: edrx_mode[%d], act_type[%d], edrx_value[%d], ptw[%d]\n\n", str_print,
                   edrx_info->edrx_mode, edrx_info->act_type, edrx_info->edrx_value, edrx_info->paging_time_window);
  }
  return;
}

void printf_device_info_jamming_status(qapi_Device_Info_Jamming_Status_Get_Rsp_t  *             jamming_status)
{
  if(jamming_status== NULL)
 {

    DEVINFO_PRINTF("%s: NULL argument\n", __func__);
     return;
 }
 else
 {
    if(jamming_status->jammer_status_valid)
    {
      if(jamming_status->jammer_status == QAPI_DEVICE_INFO_JAMMING_NOTFOUND)
      {
         DEVINFO_PRINTF("%s: jamming-Status NOT FOUND\n", __func__);
      }
      else if(jamming_status->jammer_status == QAPI_DEVICE_INFO_JAMMING_FOUND)
      {
         DEVINFO_PRINTF("%s: jamming-Status FOUND\n", __func__ );
      }
      else if(jamming_status->jammer_status == QAPI_DEVICE_INFO_JAMMING_UNKNOWN)
      {
         DEVINFO_PRINTF("%s: jamming-Status UNKWOWN\n", __func__);
      }
    }
    if(jamming_status->soft_jammer_flag_valid)
    {
      DEVINFO_PRINTF("%s: soft jamming-Status %d\n", __func__, jamming_status->soft_jammer_flag);
    }
  }
  return;
}

struct id_str reg_status_str[] =
{
  ID_STR(QAPI_DEVICE_INFO_IMS_IOT_STATUS_MIN_V01)
  ID_STR(QAPI_DEVICE_INFO_IMS_IOT_STATUS_NOT_REGISTERED_V01)
  ID_STR(QAPI_DEVICE_INFO_IMS_IOT_STATUS_REGISTERING_V01)
  ID_STR(QAPI_DEVICE_INFO_IMS_IOT_STATUS_REGISTERED_V01)
  ID_STR(QAPI_DEVICE_INFO_IMS_IOT_STATUS_LIMITED_REGISTERED_V01)
  ID_STR(QAPI_DEVICE_INFO_IMS_IOT_STATUS_MAX_V01)
};

void printf_device_info_ims_reg_status(const char* str_print, const qapi_Device_Info_t *device_info)
{
  if(NULL == device_info)
  {
    QAPI_MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,"NULL == device_info");
    DEVINFO_PRINTF("%s: NULL == device_info\n", str_print);
    return;
  }
  if(device_info->id == QAPI_DEVICE_INFO_IMS_IOT_REGISTRATION_STATUS_IND_E && device_info->u.device_info_extn != NULL)
  {
    qapi_Device_Info_IMS_Registration_Status_Rsp_t * ims_reg_status = (qapi_Device_Info_IMS_Registration_Status_Rsp_t *)(device_info->u.device_info_extn);
    DEVINFO_PRINTF("device_info->id:[%d][QAPI_DEVICE_INFO_IMS_IOT_REGISTRATION_STATUS_IND_E]\n", QAPI_DEVICE_INFO_IMS_IOT_REGISTRATION_STATUS_IND_E);
    if(ims_reg_status != NULL &&
       ims_reg_status->registration_status <= QAPI_DEVICE_INFO_IMS_IOT_STATUS_MAX_V01)
      DEVINFO_PRINTF("ims registration status: [%s]\n",reg_status_str[ims_reg_status->registration_status].str);
  }
  return;
}

struct id_str call_state_str[] =
{
  ID_STR(QAPI_DEVICE_INFO_CALL_STATE_ORIGINATION)
  ID_STR(QAPI_DEVICE_INFO_CALL_STATE_INCOMING)
  ID_STR(QAPI_DEVICE_INFO_CALL_STATE_CONVERSATION)
  ID_STR(QAPI_DEVICE_INFO_CALL_STATE_CC_IN_PROGRESS)
  ID_STR(QAPI_DEVICE_INFO_CALL_STATE_ALERTING)
  ID_STR(QAPI_DEVICE_INFO_CALL_STATE_HOLD)
  ID_STR(QAPI_DEVICE_INFO_CALL_STATE_WAITING)
  ID_STR(QAPI_DEVICE_INFO_CALL_STATE_DISCONNECTING)
  ID_STR(QAPI_DEVICE_INFO_CALL_STATE_END)
  ID_STR(QAPI_DEVICE_INFO_CALL_STATE_SETUP)
};

void printf_device_info_call_status(const char* str_print, const qapi_Device_Info_t *device_info)
{
  if(NULL == device_info)
  {
    QAPI_MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,"NULL == device_info");
    DEVINFO_PRINTF("%s: NULL == device_info\n", str_print);
    return;
  }
  if(device_info->id == QAPI_DEVICE_INFO_VOICE_ALL_CALL_STATUS_IND_E && device_info->u.device_info_extn != NULL)
  {
    qapi_Device_Info_Call_Status_Rsp_t  *call_status = (qapi_Device_Info_Call_Status_Rsp_t *)(device_info->u.device_info_extn);
    DEVINFO_PRINTF("%s: Call id is : %d\n", str_print, call_status->call_id);
    if(call_status->call_state_valid)
    {
      if(QAPI_DEVICE_INFO_CALL_STATE_ORIGINATION <= call_status->call_state &&
         call_status->call_state <= QAPI_DEVICE_INFO_CALL_STATE_SETUP)
      {
        DEVINFO_PRINTF("call_state: [%s]\n",call_state_str[call_status->call_state - 1].str);
      }
      else
      {
        DEVINFO_PRINTF("Invalid call_state: [%d]\n",call_status->call_state);
      }
    }
    if(call_status->direction_valid)
    {
      if(call_status->direction == QAPI_DEVICE_INFO_CALL_DIRECTION_MO)
        DEVINFO_PRINTF("Call direction is MO\n");
      else if(call_status->direction == QAPI_DEVICE_INFO_CALL_DIRECTION_MT)
        DEVINFO_PRINTF("Call direction is MT\n");
      else
        DEVINFO_PRINTF("Invalid call direction\n");
    }
  }
  return;
}

void printf_device_info_mt_sms(const char* str_print, const qapi_Device_Info_t *device_info)
{
  if(NULL == device_info)
  {
    QAPI_MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,"NULL == device_info");
    DEVINFO_PRINTF("%s: NULL == device_info\n", str_print);
    return;
  }
  if(device_info->id == QAPI_DEVICE_INFO_INCOMING_SMS_E && device_info->u.device_info_extn != NULL)
  {
    qapi_Device_Info_Incoming_SMS_Ind_t *mt_sms = (qapi_Device_Info_Incoming_SMS_Ind_t *)(device_info->u.device_info_extn);
    DEVINFO_PRINTF("%s: Storage type is : %d\n", str_print, mt_sms->storage_type);
    DEVINFO_PRINTF("%s: Storage index is : %d\n", str_print, mt_sms->storage_index);
  }
  return;
}

void printf_device_info_mt_route_sms(const char* str_print, const qapi_Device_Info_t *device_info)
{
  if(NULL == device_info)
  {
    QAPI_MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,"NULL == device_info");
    DEVINFO_PRINTF("%s: NULL == device_info\n", str_print);
    return;
  }
  if(device_info->id == QAPI_DEVICE_INFO_ROUTE_INCOMING_SMS_E && device_info->u.device_info_extn != NULL)
  {
    qapi_Device_Info_Route_Incoming_SMS_Ind_t *mt_sms = (qapi_Device_Info_Route_Incoming_SMS_Ind_t *)(device_info->u.device_info_extn);
    DEVINFO_PRINTF("%s: Ack indicator is: %d\n", str_print, mt_sms->ack_indicator);
    DEVINFO_PRINTF("%s: Transaction id is : %u\n", str_print, mt_sms->transaction_id);
    if(mt_sms->smsc_number.length)
    {
      DEVINFO_PRINTF("%s: SMSC type is: %d\n", str_print, mt_sms->smsc_number.number_type);
      DEVINFO_PRINTF("%s: SMSC number is: %s\n", str_print, mt_sms->smsc_number.number);
    }
    DEVINFO_PRINTF("%s: First octet is : %d\n", str_print, mt_sms->first_octet);
    if(mt_sms->sender_number.length)
    {
      DEVINFO_PRINTF("%s: Sender number type is: %d\n", str_print, mt_sms->sender_number.number_type);
      DEVINFO_PRINTF("%s: Sender number is: %s\n", str_print, mt_sms->sender_number.number);
    }
    DEVINFO_PRINTF("%s: Protocol identifier is : %d\n", str_print, mt_sms->pid);
    DEVINFO_PRINTF("%s: Data coding scheme is : %d\n", str_print, mt_sms->dcs);
    if(mt_sms->sms_payload_length)
    {
      DEVINFO_PRINTF("%s: SMS content is: %s\n", str_print, mt_sms->sms_payload);
    }
    DEVINFO_PRINTF("%s: Message format is : %d\n", str_print, mt_sms->format);
  }
  return;
}

void printf_device_info_sms_status_report(const char* str_print, const qapi_Device_Info_t *device_info)
{
  if(NULL == device_info)
  {
    QAPI_MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,"NULL == device_info");
    DEVINFO_PRINTF("%s: NULL == device_info\n", str_print);
    return;
  }
  if(device_info->id == QAPI_DEVICE_INFO_SMS_STATUS_REPORT_E && device_info->u.device_info_extn != NULL)
  {
    qapi_Device_Info_SMS_Staus_Report_t *mt_sms = (qapi_Device_Info_SMS_Staus_Report_t *)(device_info->u.device_info_extn);
    DEVINFO_PRINTF("%s: Ack indicator is: %d\n", str_print, mt_sms->ack_indicator);
    DEVINFO_PRINTF("%s: Transaction id is : %u\n", str_print, mt_sms->transaction_id);
    if(mt_sms->smsc_number.length)
    {
      DEVINFO_PRINTF("%s: SMSC type is: %d\n", str_print, mt_sms->smsc_number.number_type);
      DEVINFO_PRINTF("%s: SMSC number is: %s\n", str_print, mt_sms->smsc_number.number);
    }
    DEVINFO_PRINTF("%s: First octet is : %d\n", str_print, mt_sms->first_octet);
    DEVINFO_PRINTF("%s: Message reference is : %d\n", str_print, mt_sms->msg_ref);
    DEVINFO_PRINTF("%s: Status is : %d\n", str_print, mt_sms->status);
    if(mt_sms->reciever_number.length)
    {
      DEVINFO_PRINTF("%s: Reciever number type is: %d\n", str_print, mt_sms->reciever_number.number_type);
      DEVINFO_PRINTF("%s: Reciever number is: %s\n", str_print, mt_sms->reciever_number.number);
    }
    if(mt_sms->pid_is_valid)
      DEVINFO_PRINTF("%s: Protocol identifier is : %d\n", str_print, mt_sms->pid);
    if(mt_sms->dcs_is_valid)
      DEVINFO_PRINTF("%s: Data coding scheme is : %d\n", str_print, mt_sms->dcs);
    if(mt_sms->sms_payload_length)
    {
      DEVINFO_PRINTF("%s: SMS content is: %s\n", str_print, mt_sms->sms_payload);
    }
    DEVINFO_PRINTF("%s: Message format is : %d\n", str_print, mt_sms->format);
  }
  return;
}


void printf_device_info_send_sms_response(const char* str_print, const qapi_Device_Info_t *device_info)
{
  if(NULL == device_info)
  {
    QAPI_MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,"NULL == device_info");
    DEVINFO_PRINTF("%s: NULL == device_info\n", str_print);
    return;
  }
  if(device_info->id == QAPI_DEVICE_INFO_SEND_SMS_RSP_E && device_info->u.device_info_extn != NULL)
  {
    qapi_Device_Info_Send_SMS_Rsp_t *mt_sms = (qapi_Device_Info_Send_SMS_Rsp_t *)(device_info->u.device_info_extn);
    if(mt_sms->result.result == 0)
    {
      DEVINFO_PRINTF("%s: SMS send success\n", str_print);
      DEVINFO_PRINTF("%s: SMS send message id %d\n", str_print, mt_sms->message_id);
    }
    else
    {
      DEVINFO_PRINTF("%s: SMS send failure\n", str_print);
      DEVINFO_PRINTF("%s: SMS send failure error: %d\n", str_print, mt_sms->result.error);
    }
    if(mt_sms->cause_code_valid)
    {
      DEVINFO_PRINTF("%s: SMS send cause code: %d\n", str_print, mt_sms->cause_code);
    }
    if(mt_sms->error_class_valid)
    {
      DEVINFO_PRINTF("%s: SMS send error class: %d\n", str_print, mt_sms->error_class);
    }
    if(mt_sms->rp_cause_info_valid)
    {
      DEVINFO_PRINTF("%s: SMS send rp cause code: %d\n", str_print, mt_sms->rp_cause);
    }
    if(mt_sms->tp_cause_info_valid)
    {
      DEVINFO_PRINTF("%s: SMS send tp cause code: %d\n", str_print, mt_sms->tp_cause);
    }
    if(mt_sms->message_delivery_failure_type_valid)
    {
      DEVINFO_PRINTF("%s: SMS send msg failure type: %d\n", str_print, mt_sms->msg_failure_type);
    }
    if(mt_sms->message_delivery_failure_cause_valid)
    {
      DEVINFO_PRINTF("%s: SMS send msg failure cause: %d\n", str_print, mt_sms->msg_failure_cause);
    }
    if(mt_sms->call_control_modified_info_valid)
    {
      if(mt_sms->call_modified_info.alpha_id_len)
        DEVINFO_PRINTF("%s: SMS send call control modified: %s\n", str_print, mt_sms->call_modified_info.alpha_id);
    }
  }
  return;
}

struct id_str device_info_id_str[] =
{
    ID_STR(QAPI_DEVICE_INFO_NETWORK_IND_E)
    ID_STR(QAPI_DEVICE_INFO_NETWORK_BEARER_E)
    ID_STR(QAPI_DEVICE_INFO_MCC_E)
    ID_STR(QAPI_DEVICE_INFO_MNC_E)
    ID_STR(QAPI_DEVICE_INFO_CELL_ID_E)
    ID_STR(QAPI_DEVICE_INFO_4G_SIG_STRENGTH_E)
    ID_STR(QAPI_DEVICE_INFO_LINK_QUALITY_E)
    ID_STR(QAPI_DEVICE_INFO_TX_BYTES_E)
    ID_STR(QAPI_DEVICE_INFO_RX_BYTES_E)
    ID_STR(QAPI_DEVICE_INFO_NETWORK_IND_E)
    ID_STR(QAPI_DEVICE_INFO_ROAMING_E)
    ID_STR(QAPI_DEVICE_INFO_LTE_OP_MODE_E)
    ID_STR(QAPI_DEVICE_INFO_LTE_OP_MODE_E)
    ID_STR(QAPI_DEVICE_INFO_SIM_STATE_E)
    ID_STR(QAPI_DEVICE_INFO_JAMMER_IND_E)
    ID_STR(QAPI_DEVICE_INFO_IMS_IOT_REGISTRATION_STATUS_IND_E)
    ID_STR(QAPI_DEVICE_INFO_VOICE_ALL_CALL_STATUS_IND_E)
    ID_STR(QAPI_DEVICE_INFO_MODEM_SERVICE_STATE_E)
};

extern void iotapp_log
(
  const char* file_name, uint32_t line_number, uint32_t mask,
  uint8_t argnum,
  const char *fmt,
  ...
);

#define IOTAPP_LOG_INFO(format,...) iotapp_log( __MODULE__, __LINE__, MSG_LEGACY_HIGH, MSG_NARG(format, ## __VA_ARGS__), format, ##__VA_ARGS__)


void printf_device_info_id(const char* str_print, const qapi_Device_Info_t *device_info)
{
  int i = 0;

  if(NULL == device_info)
  {
    QAPI_MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,"NULL == device_info");
    DEVINFO_PRINTF("%s: NULL == device_info\n", str_print);
    return;
  }

  for(i = 0; i<sizeof(device_info_id_str)/sizeof(struct id_str); i++)
  {
    if(device_info_id_str[i].id == device_info->id)
    {
      QAPI_MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,"id=%d", device_info_id_str[i].id);
      DEVINFO_PRINTF("device_info->id:[%d][%s]\n", device_info->id, device_info_id_str[i].str);
      break;
    }
  }

   QAPI_MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,"info_type=%d", device_info->info_type);


  if(device_info->info_type == QAPI_DEVICE_INFO_TYPE_BUFFER_E)
    DEVINFO_PRINTF("%s: QAPI_DEVICE_INFO_TYPE_BUFFER_E buf: %s\n\n", str_print, device_info->u.valuebuf.buf);

  else if(device_info->info_type == QAPI_DEVICE_INFO_TYPE_BOOLEAN_E)
    DEVINFO_PRINTF("%s: QAPI_DEVICE_INFO_TYPE_BOOLEAN_E Val: %d\n\n", str_print, device_info->u.valuebool);

  else if(device_info->info_type == QAPI_DEVICE_INFO_TYPE_INTEGER_E)
  {
    QAPI_MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,"val=%d", device_info->u.valueint);
    QAPI_MSG(MSG_SSID_LINUX_DATA, MSG_LEGACY_ERROR,"%s: INTEGER_E Val: %d" , str_print , (int32_t)device_info->u.valueint);
    DEVINFO_PRINTF("%s: QAPI_DEVICE_INFO_TYPE_INTEGER_E Val: %lld\n\n", str_print, device_info->u.valueint);
  }
  else if(device_info->info_type == QAPI_DEVICE_INFO_TYPE_JAMMING_STATUS_E)
  {
    qapi_Device_Info_Jamming_Status_Get_Rsp_t * jamming_status = (qapi_Device_Info_Jamming_Status_Get_Rsp_t *)(device_info->u.device_info_extn);
    printf_device_info_jamming_status(jamming_status);
  }

  return;
}

boolean is_setting_band_mask(qapi_Device_Info_ID_t info_id)
{
  switch (info_id)
  {
    case QAPI_DEVICE_INFO_LTE_M1_BAND_PREF_E:
    case QAPI_DEVICE_INFO_LTE_NB1_BAND_PREF_E:
    case QAPI_DEVICE_INFO_LTE_M1_BAND_PREF_EXT_E:
    case QAPI_DEVICE_INFO_LTE_NB1_BAND_PREF_EXT_E:
    case QAPI_DEVICE_INFO_BAND_LTE_M1_CELL_E:
    case QAPI_DEVICE_INFO_BAND_LTE_NB1_CELL_E:
    case QAPI_DEVICE_INFO_BAND_GSM_CELL_E:
      return TRUE;
    default:
      return FALSE;
  }
}

/*****************************************************************************
 set_device_info_ciot_mode
 *****************************************************************************/
QCLI_Command_Status_t set_device_info_common(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id, dev_info_set_command_t* set_cmd)
{
 int i=0;
 int ret = QAPI_ERROR;
 int len = 0;
 int val[2]={0};
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

 if(NULL == app_devinfo_cli_ctx[app_id] || set_cmd == NULL)
 {
    if(set_cmd != NULL)
    {
      DEVINFO_PRINTF("%s : Device info not initialized, Please do init first \n", set_cmd->testcase);
    }
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
 }

 if((Parameter_Count <5) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("%s : Usage: [-app <app_id>] %s %s[-l <len>] [-v1 <val[0]>] [[-v2 <val[1]>]], Device Info CLI SET command\n", set_cmd->testcase, set_cmd->cmd_1, set_cmd->cmd_2);
    result = QCLI_STATUS_USAGE_E;
    goto end;
 }

 for (i = 0; i < Parameter_Count; i++)
 {
  if(strcmp((char*)Parameter_List[i].String_Value, "-l") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("size is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    len = Parameter_List[i].Integer_Value;
   }
  }

  else if(strcmp((char*)Parameter_List[i].String_Value, "-v1") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("value1 is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    val[0] = Parameter_List[i].Integer_Value;
   }
   else if(is_setting_band_mask(set_cmd->info_id))
   {
    DEVINFO_PRINTF("value1 is invalid\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
  }

  else if(strcmp((char*)Parameter_List[i].String_Value, "-v2") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("value2 is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    val[1] = Parameter_List[i].Integer_Value;
   }
   else if(is_setting_band_mask(set_cmd->info_id))
   {
    DEVINFO_PRINTF("value2 is invalid\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
  }
 }

 ret = qapi_Device_Info_Set(app_devinfo_cli_ctx[app_id], set_cmd->info_id, len,val);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("%s : qapi_Device_Info_Set returned error %d\n", set_cmd->testcase, ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
    DEVINFO_PRINTF("%s : SET successfull\n", set_cmd->testcase);

end:

  return result;
}

/*****************************************************************************
 * get_device_info_base_station_id
 *****************************************************************************/
QCLI_Command_Status_t get_device_info_common(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id,
                                                      dev_info_get_command_t* get_cmd)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_t *info = NULL;

  if(NULL == app_devinfo_cli_ctx[app_id])
  {
    DEVINFO_PRINTF("%s : Device info not initialized, Please do init first \n", get_cmd->testcase);
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if((Parameter_Count > 1) || (Parameter_List[1].String_Value != NULL))
  {
    DEVINFO_PRINTF("%s : Usage: [-app <app_id>] %s %s, Device Info CLI GET2 command\n", get_cmd->cmd_1, get_cmd->cmd_2,  get_cmd->testcase);
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  MALLOC_MEMORY(info, sizeof(qapi_Device_Info_t));
  if(info == NULL)
  {
    DEVINFO_PRINTF("%s : malloc failed while allocating qapi_Device_Info_t\n", get_cmd->testcase);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  memset(info, 0, sizeof(qapi_Device_Info_t));
  ret = qapi_Device_Info_Get_v2(app_devinfo_cli_ctx[app_id], get_cmd->info_id, info);
  if(ret != QAPI_OK)
  {
    DEVINFO_PRINTF("%s : qapi_Device_Info_Get2 returned error %d\n", get_cmd->testcase, ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    get_cmd->print_func(info, get_cmd->testcase);
  }

end:
  if(info)
    FREE_MEMORY(info);
  return result;
}

/*****************************************************************************
 * get_device_info_base_station_id
 *****************************************************************************/
QCLI_Command_Status_t get_device_info_common_cell_info(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id,
                                                      dev_info_get_command_t* get_cmd)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_t *info = NULL;
  qapi_Cell_Info *c_info = NULL;

  if(NULL == app_devinfo_cli_ctx[app_id])
  {
    DEVINFO_PRINTF("%s : Device info not initialized, Please do init first \n", get_cmd->testcase);
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if((Parameter_Count > 1) || (Parameter_List[1].String_Value != NULL))
  {
    DEVINFO_PRINTF("%s : Usage: [-app <app_id>] %s %s, Device Info CLI GET2 command\n", get_cmd->cmd_1, get_cmd->cmd_2,  get_cmd->testcase);
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  MALLOC_MEMORY(info, sizeof(qapi_Device_Info_t));
  if(info == NULL)
  {
    DEVINFO_PRINTF("%s : malloc failed while allocating qapi_Device_Info_t\n", get_cmd->testcase);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  memset(info, 0, sizeof(qapi_Device_Info_t));

  MALLOC_MEMORY(c_info, sizeof(qapi_Cell_Info));
  if(c_info == NULL)
  {
    DEVINFO_PRINTF("%s : malloc failed while allocating qapi_Cell_Info\n", get_cmd->testcase);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  memset(c_info, 0, sizeof(qapi_Cell_Info));
  info->u.device_info_extn = (void *)c_info;

  ret = qapi_Device_Info_Get_v2(app_devinfo_cli_ctx[app_id], get_cmd->info_id, info);
  if(ret != QAPI_OK)
  {
    DEVINFO_PRINTF("%s : qapi_Device_Info_Get2 returned error %d\n", get_cmd->testcase, ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    get_cmd->print_func(info, get_cmd->testcase);
  }

end:
  if(info)
    FREE_MEMORY(info);
  return result;
}

/*
 * @fn - qcli_device_info_cb
 * @brief - device info callback .
 * @param[In/Out] - device_info - qapi_Devivce_Info_t type
 */
void qcli_device_info_cb_app1(const qapi_Device_Info_t *device_info)
{
  printf_device_info_id(__func__, device_info);
}

/*
 * @fn - qcli_device_info_cb
 * @brief - device info callback .
 * @param[In/Out] - device_info - qapi_Devivce_Info_t type
 */
void qcli_device_info_cb2_app1(qapi_Device_Info_Hndl_t hndl, const qapi_Device_Info_t *device_info)
{
  printf_device_info_id(__func__, device_info);
  printf_device_info_edrx(__func__, device_info);
  printf_device_info_ims_reg_status(__func__, device_info);
  printf_device_info_call_status(__func__, device_info);
  printf_device_info_mt_sms(__func__, device_info);
  printf_device_info_mt_route_sms(__func__, device_info);
  printf_device_info_send_sms_response(__func__, device_info);
  printf_device_info_sms_status_report(__func__, device_info);
}

/*
 * @fn - qcli_device_info_cb
 * @brief - device info callback .
 * @param[In/Out] - device_info - qapi_Devivce_Info_t type
 */
void qcli_device_info_cb_app2(const qapi_Device_Info_t *device_info)
{
  printf_device_info_id(__func__, device_info);
}

/*
 * @fn - qcli_device_info_cb
 * @brief - device info callback .
 * @param[In/Out] - device_info - qapi_Devivce_Info_t type
 */
void qcli_device_info_cb2_app2(qapi_Device_Info_Hndl_t hndl, const qapi_Device_Info_t *device_info)
{
  printf_device_info_id(__func__, device_info);
  printf_device_info_edrx(__func__, device_info);
  printf_device_info_ims_reg_status(__func__, device_info);
  printf_device_info_call_status(__func__, device_info);
  printf_device_info_mt_sms(__func__, device_info);
  printf_device_info_mt_route_sms(__func__, device_info);
  printf_device_info_send_sms_response(__func__, device_info);
  printf_device_info_sms_status_report(__func__, device_info);
}

/*****************************************************************************
 set_device_info_callback
 *****************************************************************************/
QCLI_Command_Status_t set_device_info_callback(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id, dev_info_set_command_t* set_cmd)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

 if(NULL == app_devinfo_cli_ctx[app_id] || set_cmd == NULL)
 {
    if(set_cmd != NULL)
    {
      DEVINFO_PRINTF("%s : Device info not initialized, Please do init first \n", set_cmd->testcase);
    }
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
 }

 if((Parameter_Count <1) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("%s : Usage: [-app <app_id>] %s %s, Device Info SET ind command\n", set_cmd->testcase, set_cmd->cmd_1, set_cmd->cmd_2);
    result = QCLI_STATUS_USAGE_E;
    goto end;
 }

  if(app_id == 0)
  {
    ret = qapi_Device_Info_Set_Callback_v2(app_devinfo_cli_ctx[app_id], set_cmd->info_id,qcli_device_info_cb2_app1);
  }
  else if(app_id == 1)
  {
    ret = qapi_Device_Info_Set_Callback_v2(app_devinfo_cli_ctx[app_id], set_cmd->info_id,qcli_device_info_cb2_app2);
  }
  if(ret != QAPI_OK)
  {
     DEVINFO_PRINTF("%s : qapi_Device_Info_Set_Callback_v2 returned error %d\n", set_cmd->testcase, ret);
     result = QCLI_STATUS_ERROR_E;
     goto end;
  }
 else
    DEVINFO_PRINTF("%s : SET callback successfull\n", set_cmd->testcase);

end:

  return result;
}

/*****************************************************************************
 clear_device_info_callback
 *****************************************************************************/
QCLI_Command_Status_t clear_device_info_callback(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id, dev_info_set_command_t* set_cmd)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

 if(NULL == app_devinfo_cli_ctx[app_id] || set_cmd == NULL)
 {
    if(set_cmd != NULL)
    {
      DEVINFO_PRINTF("%s : Device info not initialized, Please do init first \n", set_cmd->testcase);
    }
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
 }

  if((Parameter_Count <1) || (Parameter_List == NULL))
  {
      DEVINFO_PRINTF("%s : Usage: [-app <app_id>] %s %s, Device Info clear ind command\n", set_cmd->testcase, set_cmd->cmd_1, set_cmd->cmd_2);
     result = QCLI_STATUS_USAGE_E;
     goto end;
  }

  ret = qapi_Device_Info_Clear_Callback_v2(app_devinfo_cli_ctx[app_id], set_cmd->info_id);

  if(ret != QAPI_OK)
  {
     DEVINFO_PRINTF("%s : qapi_Device_Info_Clear_Callback_v2 returned error %d\n", set_cmd->testcase, ret);
     result = QCLI_STATUS_ERROR_E;
     goto end;
  }
 else
    DEVINFO_PRINTF("%s : CLear callback successfull\n", set_cmd->testcase);

end:

  return result;
}

/*************************************************************************************************************
 *             [0]
 * help_device_info
 **************************************************************************************************************/
QCLI_Command_Status_t help_device_info(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

    DEVINFO_PRINTF("[-app <app id: 0/1>] [help|init2|get2|release2|reset2|shutdown]\n");
    DEVINFO_PRINTF("[get <lte|lac|mode|ciot|m1|nb1|acq|model|base|roam|imei|imsi|os|manu|bat_stat|percent|iccid|time|4g|srv|mnc>]\n");
    DEVINFO_PRINTF("[get <cell|rccs|emms|rsrq|pci|earfcn|chip|power|sim|psm|active|net|bearer|build|mcc|mdn|tac|apn|cm1|cnb1|gsm|app_priority|loaded_tech|opmode|plmn|cemode|modem_state|sleep_threshold|m1_ext|nb1_ext|sinr|ce_level>]\n");
    DEVINFO_PRINTF("[set <mode|ciot|m1|nb1|acq|psm|active|ps_attach|ps_detach|m1band|nb1band|gsmband|app_priority|opmode|plmn|cemode|enable_sub|sleep_threshold|m1_ext|nb1_ext>]\n");
    DEVINFO_PRINTF("[request <apdu|readplmn|writeplmn|verifypin|changepin|unblockpin|protectpin|setfdn|getfdn|ps_detach|readapp|getedrx|setedrx|jamming_status|setimsindreg|imsregstatus|dialcall|endcall|answercall|get_qcpdpp|set_qcpdpp|get_cgdcont|get_qRxlevmin|get_srxlev"
                   "|set_cgdcont|get_qcpdpimscfge|set_qcpdpimscfge|get_qcpdpcfge|set_qcpdpcfge|fsk_start|fsk_stop|fsk_data|set_fsk_debug|get_fsk_debug|set_fsk_pcl|get_fsk_pcl|set_fsk_hoptable|get_fsk_hoptable|set_sig_threshold|send_sms|write_sms|read_sms|delete_sms|list_sms|send_ack_sms|set_smsc|get_smsc|set_sms_routes|get_sms_routes >]\n");
    DEVINFO_PRINTF("[set_id <any|nw_ind|nw_bearer|mcc|mnc|cell_id|4g_sig|link_qual|tx_bytes|rx_bytes|roaming|lte_op|active|psm|wms|sim_state|edrx|jammer|ims_status|voice_call| modem_state|sleep_info|mt_sms|mt_route_sms|sms_status_report>]\n");
    DEVINFO_PRINTF("[clear_id <any|nw_ind|nw_bearer|mcc|mnc|cell_id|4g_sig|link_qual|tx_bytes|rx_bytes|roaming|lte_op|active|psm|wms|sim_state|edrx|jammer|ims_status|voice_call| modem_state|sleep_info|mt_sms|mt_route_sms|sms_status_report>]\n");

  return result;
}


/*************************************************************************************************************
 *             [0]
 * init_device_info2
 **************************************************************************************************************/

QCLI_Command_Status_t init_device_info2(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
	int ret = QAPI_ERROR;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

    if((Parameter_Count > 0) || (Parameter_List[0].String_Value != NULL))
    {
        DEVINFO_PRINTF("init_device_info : Usage: [-app <app_id>] init2 , Device Info CLI INIT2 command\n");
        result = QCLI_STATUS_USAGE_E;
        goto end;
    }

    ret = qapi_Device_Info_Init_v2(&app_devinfo_cli_ctx[app_id]);
	if(ret != QAPI_OK)
	{
		DEVINFO_PRINTF("init_device_info2 : qapi_Device_Info_Init2 returned error %d\n", ret);
		result = QCLI_STATUS_ERROR_E;
        goto end;
	}

#ifdef QAPI_TXM_MODULE
	ret = qapi_Device_Info_Pass_Pool_Ptr(app_devinfo_cli_ctx[app_id], byte_pool_qcli);
	if(ret != QAPI_OK)
	{
		DEVINFO_PRINTF("init_device_info2 : qapi_Device_Info_Pass_Pool_Ptr returned error %d\n", ret);
		result = QCLI_STATUS_ERROR_E;
        goto end;
	}
#endif
/*	if(app_id == 0)
	  ret = qapi_Device_Info_Set_Callback_v2(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_ANY, qcli_device_info_cb2_app1);
      //ret = qapi_Device_Info_Set_Callback_v2(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_EDRX_E, qcli_device_info_cb2_app1); //no need, register of one ID, all ID will be reported
	if(app_id == 1)
	  ret = qapi_Device_Info_Set_Callback_v2(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_ANY, qcli_device_info_cb2_app2);

	if(ret != QAPI_OK)
	{
		DEVINFO_PRINTF("init_device_info2 : qapi_Device_Info_Set_Callback2 returned error %d\n", ret);
		result = QCLI_STATUS_ERROR_E;
        goto end;
	}*/
	DEVINFO_PRINTF("init_device_info2 : INIT successfull\n");
end:

  return result;
}



/***********************************************
 *                 [0]
 * release_device_info2
 ***********************************************/

QCLI_Command_Status_t release_device_info2(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  if(NULL == app_devinfo_cli_ctx[app_id])
  {
    DEVINFO_PRINTF("release_device_info2 : Device info not initialized, Please do init first \n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if((Parameter_Count > 0) || (Parameter_List[0].String_Value != NULL))
  {
    DEVINFO_PRINTF("release_device_info2 : Usage: [-app <app_id>] get , Device Info CLI RELEASE2 command\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  ret = qapi_Device_Info_Release_v2(app_devinfo_cli_ctx[app_id]);
  if(ret != QAPI_OK)
  {
	DEVINFO_PRINTF("release_device_info2 : qapi_Device_Info_Release2 returned error %d\n", ret);
	result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  app_devinfo_cli_ctx[app_id] = NULL;
  DEVINFO_PRINTF("release_device_info2 : RELEASED Successfully\n");

end:

  return result;
}


/***********************************************
 *                 [0]
 * reset_device_info
 ***********************************************/

QCLI_Command_Status_t reset_device_info2(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  if(NULL == app_devinfo_cli_ctx[app_id])
  {
    DEVINFO_PRINTF("reset_device_info2 : Device info not initialized, Please do init first  \n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if((Parameter_Count > 0) || (Parameter_List[0].String_Value != NULL))
  {
    DEVINFO_PRINTF("reset_device_info2 : Usage: [-app <app_id>] get , Device Info CLI RESET2 command\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  ret = qapi_Device_Info_Reset_v2(app_devinfo_cli_ctx[app_id]);
  if(ret != QAPI_OK)
  {
	DEVINFO_PRINTF("reset_device_info2 : qapi_Device_Info_Reset2 returned error %d\n", ret);
	result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  DEVINFO_PRINTF("reset_device_info2 : RESET Successfully\n");

end:

  return result;
}


/***********************************************
 * shutdown_device_info
 ***********************************************/
QCLI_Command_Status_t shutdown_device_info(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  if(NULL == app_devinfo_cli_ctx[app_id])
  {
    DEVINFO_PRINTF("shutdown_device_info : Device info not initialized, Please do init first  \n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if((Parameter_Count > 0) || (Parameter_List[0].String_Value != NULL))
  {
    DEVINFO_PRINTF("shutdown_device_info : Usage: [-app <app_id>] shutdown , Device Info CLI SHUTDOWN command\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }
  ret = qapi_Device_Info_Set(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_SHUTDOWN_E, 0, NULL);
  if(ret != QAPI_OK)
  {
	DEVINFO_PRINTF("shutdown_device_info : qapi_Device_Info_Reset2 returned error %d\n", ret);
	result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  DEVINFO_PRINTF("shutdown_device_info : Successfully\n");

end:
  return result;
}
/***********************************************
  set_device_info_acq_order
 ***********************************************/
QCLI_Command_Status_t set_device_info_acq_order(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int i=0;
 int ret = QAPI_ERROR;
 int val[MAX_LEN_VAL]={0};
 int len = 0;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

 if((Parameter_Count <4) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("set_device_info : Usage: [-app <app_id>] set acq [-l <len>] [-v1 <val[0]>] [-v2 < val[1]>] [-v3 <val[2]>] [-v4 <val[3]>] [-v5 <val[4]>], Device Info CLI SET command\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
 }

 for (i = 0; i < Parameter_Count; i++)
 {
  if(strcmp((char*)Parameter_List[i].String_Value, "-l") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("size is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    len = Parameter_List[i].Integer_Value;
   }
  }

  else if(strcmp((char*)Parameter_List[i].String_Value, "-v1") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("value1 is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    val[0] = Parameter_List[i].Integer_Value;
   }
  }

  else  if(strcmp((char*)Parameter_List[i].String_Value, "-v2") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("value is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    val[1] = Parameter_List[i].Integer_Value;
   }
  }

  else  if(strcmp((char*)Parameter_List[i].String_Value, "-v3") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("value is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    val[2] = Parameter_List[i].Integer_Value;
   }
  }

  else  if(strcmp((char*)Parameter_List[i].String_Value, "-v4") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("value is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    val[3] = Parameter_List[i].Integer_Value;
   }
  }

  else  if(strcmp((char*)Parameter_List[i].String_Value, "-v5") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("value is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    val[4] = Parameter_List[i].Integer_Value;
   }
  }
 }

 ret = qapi_Device_Info_Set(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_RAT_ACQ_PREF_E, len, val);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("Set_device_info : qapi_Device_Info_Set returned error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
    DEVINFO_PRINTF("set_device_info_acq_order: SET successfull\n");

end:
  return result;
}
/*****************************************************************************
 * get_device_info_plmn_list
 *****************************************************************************/
QCLI_Command_Status_t get_device_info_plmn_list(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_t *info = NULL;
  qapi_Device_Info_PLMN_Info_List_t *plmn_info = NULL;

  if((Parameter_Count > 1) || (Parameter_List[1].String_Value != NULL))
  {
    DEVINFO_PRINTF("get_device_info2 : Usage: [-app <app_id>] get plmn , Device Info CLI GET2 command\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  MALLOC_MEMORY(info, sizeof(qapi_Device_Info_t));
  if(info == NULL)
  {
    DEVINFO_PRINTF("get_device_info_plmn_list : malloc failed while allocating qapi_Device_Info_t\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  memset(info, 0, sizeof(qapi_Device_Info_t));
  MALLOC_MEMORY(plmn_info, sizeof(qapi_Device_Info_PLMN_Info_List_t));
  if(plmn_info == NULL)
  {
    DEVINFO_PRINTF("get_device_info_plmn_list : malloc failed while allocating qapi_Device_Info_PLMN_Info_List_t\n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  memset(plmn_info, 0, sizeof(qapi_Device_Info_PLMN_Info_List_t));
  info->u.device_info_extn = (void *)plmn_info;
  ret = qapi_Device_Info_Get_v2(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_PLMN_LIST_E, info);
  if(ret != QAPI_OK)
  {
    DEVINFO_PRINTF("get_device_info_plmn_list : returned error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
     int i = 0;
     plmn_info = (qapi_Device_Info_PLMN_Info_List_t *)info->u.device_info_extn;
     DEVINFO_PRINTF("plmn_list_len[%d]\n", plmn_info->list_len);
     for (i = 0; i < plmn_info->list_len ;i++)
     {
        DEVINFO_PRINTF("PLMN index[%d]: mcc=%d, mnc=%d, act=%d\n",i, plmn_info->plmn_list[i].mcc, plmn_info->plmn_list[i].mnc, plmn_info->plmn_list[i].Act);
     }
  }

end:
  if(plmn_info)
  {
     FREE_MEMORY(plmn_info);
     plmn_info = NULL;
  }
  if(info)
  {
    FREE_MEMORY(info);
    info = NULL;
  }
  return result;
}

/***********************************************
    set_device_info_plmn_select
 ***********************************************/
QCLI_Command_Status_t set_device_info_plmn_select(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int i=0;
 int ret = QAPI_ERROR;
 int len = 0;
 int val[3]={0};
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

 if((Parameter_Count < 5) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("set_device_info : Usage: [-app <app_id>] set plmn [-l <len>] [-v1 <val[0]>] [-v2 <val[1]>] [-v3 <val[2]>], Device Info CLI SET command\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
 }

 for (i = 0; i < Parameter_Count; i++)
 {
  if(strcmp((char*)Parameter_List[i].String_Value, "-l") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("size is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    len = Parameter_List[i].Integer_Value;
   }
  }
  else if(strcmp((char*)Parameter_List[i].String_Value, "-v1") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("value1 is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    val[0] = Parameter_List[i].Integer_Value;
   }
  }
  else if(strcmp((char*)Parameter_List[i].String_Value, "-v2") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("value1 is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    val[1] = Parameter_List[i].Integer_Value;
   }
  }
  else if(strcmp((char*)Parameter_List[i].String_Value, "-v3") == 0)
  {
   ++i;
   if(i > Parameter_Count)
   {
    DEVINFO_PRINTF("value1 is required\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else if(Parameter_List[i].Integer_Is_Valid)
   {
    val[2] = Parameter_List[i].Integer_Value;
   }
  }
 }

 ret = qapi_Device_Info_Set(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_PLMN_SELECT_E, len, val);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("Set_device_info : qapi_Device_Info_Set returned error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
    DEVINFO_PRINTF("set_device_info_plmn_select: SET successfull\n");

end:
  return result;
}
/***********************************************
  set_device_info_m1_band_ext
 ***********************************************/
QCLI_Command_Status_t set_device_info_m1_band_ext(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int i=0;
 int ret = QAPI_ERROR;
 int val[MAX_LEN_VAL]={0};
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

 if((Parameter_Count < 16) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("set_device_info : Usage: [-app <app_id>] set m1_ext -v1 <val[0]> -v2 < val[1]> -v3 <val[2]> -v4 <val[3]> -v5 <val[4]> -v6 <val[5]> -v7 < val[6]> -v8 <val[7]>, Device Info CLI SET command\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
 }

 for (i = 0; i < Parameter_Count; i++)
 {
  if(strcmp((char*)Parameter_List[i].String_Value, "-v1") == 0)
  {
   ++i;
   if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
   {
    DEVINFO_PRINTF("value1 is invalid or not inputted\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else
   {
    val[0] = Parameter_List[i].Integer_Value;
   }
  }

  else  if(strcmp((char*)Parameter_List[i].String_Value, "-v2") == 0)
  {
   ++i;
   if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
   {
    DEVINFO_PRINTF("value2 is invalid or not inputted\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else
   {
    val[1] = Parameter_List[i].Integer_Value;
   }
  }

  else  if(strcmp((char*)Parameter_List[i].String_Value, "-v3") == 0)
  {
   ++i;
   if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
   {
    DEVINFO_PRINTF("value3 is invalid or not inputted\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else
   {
    val[2] = Parameter_List[i].Integer_Value;
   }
  }

  else  if(strcmp((char*)Parameter_List[i].String_Value, "-v4") == 0)
  {
   ++i;
   if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
   {
    DEVINFO_PRINTF("value4 is invalid or not inputted\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else
   {
    val[3] = Parameter_List[i].Integer_Value;
   }
  }

  else  if(strcmp((char*)Parameter_List[i].String_Value, "-v5") == 0)
  {
   ++i;
   if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
   {
    DEVINFO_PRINTF("value5 is invalid or not inputted\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else
   {
    val[4] = Parameter_List[i].Integer_Value;
   }
  }

 else  if(strcmp((char*)Parameter_List[i].String_Value, "-v6") == 0)
 {
  ++i;
  if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
  {
   DEVINFO_PRINTF("value6 is invalid or not inputted\n");
   result = QCLI_STATUS_USAGE_E;
   goto end;
  }
  else
  {
   val[5] = Parameter_List[i].Integer_Value;
  }
 }

 else  if(strcmp((char*)Parameter_List[i].String_Value, "-v7") == 0)
 {
  ++i;
  if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
  {
   DEVINFO_PRINTF("value7 is invalid or not inputted\n");
   result = QCLI_STATUS_USAGE_E;
   goto end;
  }
  else
  {
   val[6] = Parameter_List[i].Integer_Value;
  }
 }

 else  if(strcmp((char*)Parameter_List[i].String_Value, "-v8") == 0)
 {
  ++i;
  if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
  {
   DEVINFO_PRINTF("value8 is invalid or not inputted\n");
   result = QCLI_STATUS_USAGE_E;
   goto end;
  }
  else
  {
   val[7] = Parameter_List[i].Integer_Value;
  }
 }

 }

 ret = qapi_Device_Info_Set(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_LTE_M1_BAND_PREF_EXT_E, 8, val);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("Set_device_info : qapi_Device_Info_Set returned error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
    DEVINFO_PRINTF("set_device_info_m1_band_ext: SET successfull\n");

end:
  return result;
}

/***********************************************
  set_device_info_nb1_band_ext
 ***********************************************/
QCLI_Command_Status_t set_device_info_nb1_band_ext(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int i=0;
 int ret = QAPI_ERROR;
 int val[MAX_LEN_VAL]={0};
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

 if((Parameter_Count < 16) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("set_device_info : Usage: [-app <app_id>] set nb1_ext -v1 <val[0]> -v2 < val[1]> -v3 <val[2]> -v4 <val[3]> -v5 <val[4]> -v6 <val[5]> -v7 < val[6]> -v8 <val[7]>, Device Info CLI SET command\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
 }

 for (i = 0; i < Parameter_Count; i++)
 {
  if(strcmp((char*)Parameter_List[i].String_Value, "-v1") == 0)
  {
   ++i;
   if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
   {
    DEVINFO_PRINTF("value1 is invalid or not inputted\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else
   {
    val[0] = Parameter_List[i].Integer_Value;
   }
  }

  else  if(strcmp((char*)Parameter_List[i].String_Value, "-v2") == 0)
  {
   ++i;
   if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
   {
    DEVINFO_PRINTF("value2 is invalid or not inputted\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else
   {
    val[1] = Parameter_List[i].Integer_Value;
   }
  }

  else  if(strcmp((char*)Parameter_List[i].String_Value, "-v3") == 0)
  {
   ++i;
   if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
   {
    DEVINFO_PRINTF("value3 is invalid or not inputted\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else
   {
    val[2] = Parameter_List[i].Integer_Value;
   }
  }

  else  if(strcmp((char*)Parameter_List[i].String_Value, "-v4") == 0)
  {
   ++i;
   if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
   {
    DEVINFO_PRINTF("value4 is invalid or not inputted\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else
   {
    val[3] = Parameter_List[i].Integer_Value;
   }
  }

  else  if(strcmp((char*)Parameter_List[i].String_Value, "-v5") == 0)
  {
   ++i;
   if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
   {
    DEVINFO_PRINTF("value5 is invalid or not inputted\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
   }
   else
   {
    val[4] = Parameter_List[i].Integer_Value;
   }
  }

 else  if(strcmp((char*)Parameter_List[i].String_Value, "-v6") == 0)
 {
  ++i;
  if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
  {
   DEVINFO_PRINTF("value6 is invalid or not inputted\n");
   result = QCLI_STATUS_USAGE_E;
   goto end;
  }
  else
  {
   val[5] = Parameter_List[i].Integer_Value;
  }
 }

 else  if(strcmp((char*)Parameter_List[i].String_Value, "-v7") == 0)
 {
  ++i;
  if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
  {
   DEVINFO_PRINTF("value7 is invalid or not inputted\n");
   result = QCLI_STATUS_USAGE_E;
   goto end;
  }
  else
  {
   val[6] = Parameter_List[i].Integer_Value;
  }
 }

 else  if(strcmp((char*)Parameter_List[i].String_Value, "-v8") == 0)
 {
  ++i;
  if(i > Parameter_Count || Parameter_List[i].Integer_Is_Valid != true)
  {
   DEVINFO_PRINTF("value8 is invalid or not inputted\n");
   result = QCLI_STATUS_USAGE_E;
   goto end;
  }
  else
  {
   val[7] = Parameter_List[i].Integer_Value;
  }
 }

 }

 ret = qapi_Device_Info_Set(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_LTE_NB1_BAND_PREF_EXT_E, 8, val);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("Set_device_info : qapi_Device_Info_Set returned error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
    DEVINFO_PRINTF("set_device_info_nb1_band_ext: SET successfull\n");

end:
  return result;
}

static boolean util_get_hex_value(const char str, uint8_t* number)
{
  if(str >= '0' && (str <= '9'))
  	*number = str - '0';
  else if(str >= 'a' && (str <= 'f'))
    *number = 10 + str - 'a';
  else if(str >= 'A' && (str <= 'F'))
    *number = 10 + str - 'A';
  else
  	return false;

  return true;
}
static boolean util_string_to_byte(const char *String_Buffer, uint8_t **hex_value)
{
  int length = 0;
  int i=0;

  length = strlen(String_Buffer);

  if(length % 2 != 0) {
  	DEVINFO_PRINTF("Wrong HEX string length\n");
	return false;
  }

  length = length/2;
  MALLOC_MEMORY(*hex_value, length);

  if(*hex_value == NULL)
  	return false;

  for(i=0; i<length; i++)
  {
     uint8_t value;
	 if(util_get_hex_value(String_Buffer[2*i], &value) == false)
	 {
  	   DEVINFO_PRINTF("Wrong HEX string\n");
  	   return false;
	 }
     (*hex_value)[i] = value << 4;
     if(util_get_hex_value(String_Buffer[2*i+1], &value) == false)
     {
 	     DEVINFO_PRINTF("Wrong HEX string\n");
 	     return false;
     }
     (*hex_value)[i] += value ;
  }

  return true;
}

void printf_uim_error_code(qapi_Device_Info_UIM_Response_code error)
{
  int i = 0;
  struct id_str uim_error_str[] =
  {
    {QAPI_DEVICE_INFO_UIM_NO_ERROR,         "No Error"},
    {QAPI_DEVICE_INFO_UIM_INCORRECT_PIN,    "Incorrect PIN"},
    {QAPI_DEVICE_INFO_UIM_PIN_BLOCKED,      "Pin Blocked"},
    {QAPI_DEVICE_INFO_UIM_PIN_PERM_BLOCKED, "Pin Perm Blocked"},
    {QAPI_DEVICE_INFO_UIM_ACCESS_DENIED,    "Access Denied"},
    {QAPI_DEVICE_INFO_UIM_INTERNAL_ERROR,   "Internal Error"},
  };
  DEVINFO_PRINTF("  Uim Error Code: \n");

  for(i=0; i<sizeof(uim_error_str)/sizeof(struct id_str); i++)
  {
    if(uim_error_str[i].id == error)
    {
      DEVINFO_PRINTF("    %s\n", uim_error_str[i].str);
	  break;
    }
  }
  if(i == sizeof(uim_error_str)/sizeof(struct id_str))
  {
      DEVINFO_PRINTF("    %s\n", "Unknown Error\n");
  }

}

void printf_pdp_error_code(qapi_Device_Info_PDP_Response_code error)
{
  int i = 0;
  struct id_str pdp_error_str[] =
  {
    {QAPI_DEVICE_INFO_PDP_NO_ERROR,         "No Error"},
    {QAPI_DEVICE_INFO_PDP_FAIL,    "Internal Error"},
    {QAPI_DEVICE_INFO_PDP_ERR_INVAL_IDENT,      "Invalid identifier."},
    {QAPI_DEVICE_INFO_PDP_ERR_INVAL, "Invalid parameter"},
    {QAPI_DEVICE_INFO_PDP_ERR_OUT_OF_PROFILES,    "Out of profiles"},
    {QAPI_DEVICE_INFO_PDP_ERR_OUT_OF_MEMORY,   "Out of memory"},
    {QAPI_DEVICE_INFO_PDP_ERR_FILE_ACCESS,   "File access error"},
    {QAPI_DEVICE_INFO_PDP_ERR_EOF,   "End of file error."},
    {QAPI_DEVICE_INFO_PDP_PROFILE_ALREADY_PRESENT,   "Profile already present."},
  };
  DEVINFO_PRINTF("  Profile response: \n");

  for(i=0; i<sizeof(pdp_error_str)/sizeof(struct id_str); i++)
  {
    if(pdp_error_str[i].id == error)
    {
      DEVINFO_PRINTF("    %s\n", pdp_error_str[i].str);
      break;
    }
  }
  if(i == sizeof(pdp_error_str)/sizeof(struct id_str))
  {
      DEVINFO_PRINTF("    %s\n", "Internal Error\n");
  }
}

void printf_uim_response(qapi_Device_Info_UIM_Response_t* uim_resp)
{
  if(uim_resp == NULL) return ;

  DEVINFO_PRINTF("Uim Response: \n");
  printf_uim_error_code(uim_resp->error);
  DEVINFO_PRINTF("  card-sw1:%02x, card-sw2:%02x\n", uim_resp->card_result_sw1, uim_resp->card_result_sw2 );

}

void printf_uim_pin_response(qapi_Device_Info_Pin_Response_t* pin_resp)
{
  if(pin_resp == NULL) return ;

  printf_uim_response(&(pin_resp->uim_response));

  if(pin_resp->retries_left_valid == true)
     DEVINFO_PRINTF("  verify-left:%d, unblock-left:%d\n", pin_resp->verify_left, pin_resp->unblock_left);
}

QCLI_Command_Status_t request_device_apdu(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 int i=0;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_APDU_Command_t   apdu_cmd_req;
 qapi_Device_Info_APDU_Response_t  apdu_cmd_rsp;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&apdu_cmd_req,  0, sizeof(qapi_Device_Info_APDU_Command_t));
 memset(&apdu_cmd_rsp, 0, sizeof(qapi_Device_Info_APDU_Response_t));

 if((Parameter_Count != 2) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("request apdu : Usage: [-app <app_id>] request apdu <HEX_APDU_DATA> , Device Info CLI Request command\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
 }

 if(util_string_to_byte((const char*)Parameter_List[1].String_Value, &(apdu_cmd_req.apdu_cmd_data)) == false)
 {
   goto end;
 }

 apdu_cmd_req.apdu_cmd_length  = strlen((const char*)Parameter_List[1].String_Value)/2;
 info_request.req.apdu_cmd_req  = &apdu_cmd_req;
 info_response.resp.apdu_cmd_rsp = &apdu_cmd_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SEND_APDU_E, &info_request, &info_response);
 if(ret != QAPI_OK || apdu_cmd_rsp.apdu_resp_data == NULL)
 {
    DEVINFO_PRINTF("request apdu: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    if(apdu_cmd_rsp.uim_response.error != QAPI_DEVICE_INFO_UIM_NO_ERROR)
    {
 	  printf_uim_response(&(apdu_cmd_rsp.uim_response));
	  result = QCLI_STATUS_ERROR_E;
	  goto end;
    }

	DEVINFO_PRINTF(" APDU Response: ");
    for(i=0; i<apdu_cmd_rsp.apdu_resp_length; i++)
      DEVINFO_PRINTF("%02x", apdu_cmd_rsp.apdu_resp_data[i]);

    DEVINFO_PRINTF("\n");
 }
end:

  if(apdu_cmd_req.apdu_cmd_data)
    FREE_MEMORY(apdu_cmd_req.apdu_cmd_data);

  return result;
}

QCLI_Command_Status_t request_device_read_plmn(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Preferred_PLMN_List_ID_t list_id;
 qapi_Device_Info_Preferred_PLMN_Read_Req_t   preferred_plmn_read_req;
 qapi_Device_Info_Preferred_PLMN_Read_Rsp_t  preferred_plmn_read_rsp;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&preferred_plmn_read_req,  0, sizeof(qapi_Device_Info_Preferred_PLMN_Read_Req_t));
 memset(&preferred_plmn_read_rsp, 0, sizeof(qapi_Device_Info_Preferred_PLMN_Read_Rsp_t));


 if((Parameter_Count != 2) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 if(strcasecmp((const char*)Parameter_List[1].String_Value, "plmn") == 0)
 	 list_id = QAPI_DEVICE_INFO_PREFERRED_PLMN_LIST_PLMNWACT;
 else if(strcasecmp((const char*)Parameter_List[1].String_Value, "oplmn") == 0)
 	 list_id = QAPI_DEVICE_INFO_PREFERRED_PLMN_LIST_OPLMNWACT;
 else if(strcasecmp((const char*)Parameter_List[1].String_Value, "hplmn") == 0)
 	 list_id = QAPI_DEVICE_INFO_PREFERRED_PLMN_LIST_HPLMNWACT;
 else
 {
    goto usage_end;
 }

 preferred_plmn_read_req.list_id = list_id;

 info_request.req.preferred_plmn_read_req  = &preferred_plmn_read_req;
 info_response.resp.preferred_plmn_read_rsp = &preferred_plmn_read_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_READ_PREFERRED_PLMN_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request plmn: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    qapi_Device_Info_Preferred_PLMN_Read_Rsp_t  *prefered_plmn_read_rsp = &preferred_plmn_read_rsp;
    qapi_Device_Info_Preferred_PLMN_t *plmn_item = prefered_plmn_read_rsp->plmn_list_head;
    qapi_Device_Info_Preferred_PLMN_t *plmn_item_tmp = NULL;

    DEVINFO_PRINTF("QAPI success: \n");

    if(prefered_plmn_read_rsp->uim_response.error != QAPI_DEVICE_INFO_UIM_NO_ERROR)
    {
 	  printf_uim_response(&(prefered_plmn_read_rsp->uim_response));
	  result = QCLI_STATUS_ERROR_E;
	  goto end;
    }

    DEVINFO_PRINTF("response:total number[%d], valid number[%d]\n", prefered_plmn_read_rsp->total_plmn_number, prefered_plmn_read_rsp->valid_plmn_number);
    while(plmn_item != NULL)
    {
       plmn_item_tmp = plmn_item;
       DEVINFO_PRINTF("Record %d:\n", plmn_item->index);
       DEVINFO_PRINTF("  plmn %02x%02x%02x\n", plmn_item->plmn[0], plmn_item->plmn[1], plmn_item->plmn[2]);
       DEVINFO_PRINTF("  gsm_act %d\n", plmn_item->gsm_act);
       DEVINFO_PRINTF("  gsm_compact_act %d\n", plmn_item->gsm_compact_act);
       DEVINFO_PRINTF("  utran_act %d\n",  plmn_item->utran_act);
       DEVINFO_PRINTF("  eutran_act %d\n", plmn_item->eutran_act);
       plmn_item = plmn_item->next;
       FREE_MEMORY(plmn_item_tmp);
    }
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
    DEVINFO_PRINTF("request plmn: Usage: [-app <app_id>] read plmn <plmn/oplmn/hplmn>\n");
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_read_app(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 int i=0;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_UICC_Application_Rsp_t     uicc_apps_read_rsp;

 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&uicc_apps_read_rsp, 0, sizeof(qapi_Device_Info_UICC_Application_Rsp_t));

 if((Parameter_Count != 1) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

  info_response.resp.uicc_apps_read_rsp  = &uicc_apps_read_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_READ_UICC_APPLICATIONS_E, NULL, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request plmn: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    qapi_Device_Info_UICC_Application_Rsp_t  *apps_rsp= &uicc_apps_read_rsp;

    DEVINFO_PRINTF("QAPI success: \n");

    if(apps_rsp->uim_response.error != QAPI_DEVICE_INFO_UIM_NO_ERROR)
    {
 	  printf_uim_response(&(apps_rsp->uim_response));
	  result = QCLI_STATUS_ERROR_E;
	  goto end;
    }

    DEVINFO_PRINTF("response:total apps number[%d]\n", apps_rsp->application_rec_number);
	for(i=0; i<apps_rsp->application_rec_number; i++)
    {
       int j=0;
       DEVINFO_PRINTF("Apps %d:\n  ", i);
	   for(j=0; j<apps_rsp->application_rec_length; j++ )
         DEVINFO_PRINTF("%02x", apps_rsp->application_recs[i][j]);
	   DEVINFO_PRINTF("\n");
    }
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
    DEVINFO_PRINTF("request plmn: Usage: [-app <app_id>] read plmn <plmn/oplmn/hplmn>\n");
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}


QCLI_Command_Status_t request_device_write_plmn(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_reponse;
 qapi_Device_Info_Preferred_PLMN_List_ID_t list_id;
 qapi_Device_Info_Preferred_PLMN_Set_Req_t    preferred_plmn_set_req;
 qapi_Device_Info_UIM_Response_t             preferred_plmn_set_rsp;
 uint8_t* hex_value = NULL;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_reponse,  0, sizeof(qapi_Device_Info_Response_t));
 memset(&preferred_plmn_set_req,  0, sizeof(qapi_Device_Info_Preferred_PLMN_Set_Req_t));
 memset(&preferred_plmn_set_rsp,  0, sizeof(qapi_Device_Info_UIM_Response_t));


 if((Parameter_Count < 2) || (Parameter_Count > 8) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 if(strcasecmp((const char*)Parameter_List[1].String_Value, "plmn") == 0)
 	list_id = QAPI_DEVICE_INFO_PREFERRED_PLMN_LIST_PLMNWACT;
 else if(strcasecmp((const char*)Parameter_List[1].String_Value, "oplmn") == 0)
 	list_id = QAPI_DEVICE_INFO_PREFERRED_PLMN_LIST_OPLMNWACT;
 else if(strcasecmp((const char*)Parameter_List[1].String_Value, "hplmn") == 0)
 	list_id = QAPI_DEVICE_INFO_PREFERRED_PLMN_LIST_HPLMNWACT;
 else
 {
    goto usage_end;
 }

 preferred_plmn_set_req.list_id = list_id;
 if(strlen((const char*)Parameter_List[3].String_Value) != 6)
 	goto usage_end;

 if(util_string_to_byte((const char*)Parameter_List[3].String_Value, &hex_value) == false)
 {
    goto usage_end;
 }
 if(hex_value == NULL)
    goto usage_end;

 memscpy(preferred_plmn_set_req.plmn_data.plmn, 3, hex_value, 3);

 preferred_plmn_set_req.plmn_data.index = Parameter_List[2].Integer_Value;
 preferred_plmn_set_req.plmn_data.gsm_act         = (Parameter_List[4].Integer_Value != 0);
 preferred_plmn_set_req.plmn_data.gsm_compact_act = (Parameter_List[5].Integer_Value != 0);
 preferred_plmn_set_req.plmn_data.utran_act       = (Parameter_List[6].Integer_Value != 0);
 preferred_plmn_set_req.plmn_data.eutran_act      = (Parameter_List[7].Integer_Value != 0);

 info_request.req.preferred_plmn_set_req  = &preferred_plmn_set_req;
 info_reponse.resp.preferred_plmn_set_rsp = &preferred_plmn_set_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_WRITE_PREFERRED_PLMN_E, &info_request, &info_reponse);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request writeplmn: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    if(preferred_plmn_set_rsp.error != QAPI_DEVICE_INFO_UIM_NO_ERROR)
    {
 	  printf_uim_response(&(preferred_plmn_set_rsp));
	  result = QCLI_STATUS_ERROR_E;
	  goto end;
    }

    result = QCLI_STATUS_SUCCESS_E;
    goto end;
 }


usage_end:
    DEVINFO_PRINTF("request writeplmn: Usage: [-app <app_id>] request writeplmn <plmn/oplmn/hplmn> <index> <plmn-id> <gsm-act> <gsm-compact-act> <utran-act> <eutran-act> \n");
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_verify_pin(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Req_Verify_Pin_t            pin_verify_req;
 qapi_Device_Info_Pin_Response_t              pin_resp;
 uint8_t max_pin_value_len = sizeof(pin_verify_req.pin_value) - 1;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&pin_verify_req,  0, sizeof(qapi_Device_Info_Req_Verify_Pin_t));
 memset(&pin_resp, 0, sizeof(qapi_Device_Info_Pin_Response_t));

 if((Parameter_Count != 3) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("request_device_verify_pin : Parameter_Count[%d], expect 3\n", Parameter_Count);
    goto usage_end;
 }

 if(Parameter_List[1].Integer_Value != QAPI_DEVICE_INFO_PIN_ID_PIN1 &&
 	Parameter_List[1].Integer_Value != QAPI_DEVICE_INFO_PIN_ID_PIN2 &&
 	Parameter_List[1].Integer_Value != QAPI_DEVICE_INFO_PIN_ID_UNIVERSER_PIN)
 {
    DEVINFO_PRINTF("request_device_verify_pin : PIN-id[%d], expect 0/1/2\n", Parameter_List[1].Integer_Value);
    goto usage_end;
 }

 if(strlen((const char*)Parameter_List[2].String_Value) == 0 ||
 	strlen((const char*)Parameter_List[2].String_Value) > max_pin_value_len)
 {
    DEVINFO_PRINTF("request_device_verify_pin : PIN-Lenth[%d], expect (0,8]\n", strlen((const char*)Parameter_List[2].String_Value));
 	goto usage_end;
 }


 pin_verify_req.pin_id = (qapi_Device_Info_Pin_Id_t)Parameter_List[1].Integer_Value;
 strlcpy(pin_verify_req.pin_value,(const char*) Parameter_List[2].String_Value, max_pin_value_len+1);

 info_request.req.pin_verify_req  = &pin_verify_req;
 info_response.resp.pin_resp = &pin_resp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_VERIFY_PIN_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {

    DEVINFO_PRINTF("request_device_verify_pin: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    printf_uim_pin_response(&pin_resp);
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
    DEVINFO_PRINTF("request_device_verify_pin: Usage: [-app <app_id>] request verifypin <pin-id> <pin-value>\n");
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_change_pin(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Req_Change_Pin_t             pin_change_req;
 qapi_Device_Info_Pin_Response_t pin_resp;
 uint8_t max_pin_value_len = sizeof(pin_change_req.new_pin_value) - 1;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&pin_change_req,  0, sizeof(qapi_Device_Info_Req_Change_Pin_t));
 memset(&pin_resp, 0, sizeof(qapi_Device_Info_Pin_Response_t));

 if((Parameter_Count != 4) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("request_device_change_pin : unexpected params number(%d), expect(%d) \n", Parameter_Count, 4);
    goto usage_end;
 }

 if(Parameter_List[1].Integer_Value != QAPI_DEVICE_INFO_PIN_ID_PIN1  &&
 	Parameter_List[1].Integer_Value != QAPI_DEVICE_INFO_PIN_ID_PIN2  &&
 	Parameter_List[1].Integer_Value != QAPI_DEVICE_INFO_PIN_ID_UNIVERSER_PIN )
 {
    DEVINFO_PRINTF("request_device_change_pin : unexpected pin-id(%d), expect[%d-%d] \n", Parameter_List[1].Integer_Value, QAPI_DEVICE_INFO_PIN_ID_PIN1, QAPI_DEVICE_INFO_PIN_ID_UNIVERSER_PIN);
    goto usage_end;
 }

 if(strlen((const char*)Parameter_List[2].String_Value) == 0 ||
 	strlen((const char*)Parameter_List[2].String_Value) > max_pin_value_len ||
 	strlen((const char*)Parameter_List[3].String_Value) == 0 ||
 	strlen((const char*)Parameter_List[3].String_Value) > max_pin_value_len)
 {
    DEVINFO_PRINTF("request_device_change_pin : unexpected pin-lenth(%d), expec(0,8] \n", strlen((const char*)Parameter_List[2].String_Value));
    DEVINFO_PRINTF("request_device_change_pin : unexpected pin-lenth(%d), expec(0,8] \n", strlen((const char*)Parameter_List[3].String_Value));
 	goto usage_end;
 }


 pin_change_req.pin_id = (qapi_Device_Info_Pin_Id_t)Parameter_List[1].Integer_Value;
 strlcpy(pin_change_req.old_pin_value, (const char*)Parameter_List[2].String_Value, max_pin_value_len+1);
 strlcpy(pin_change_req.new_pin_value, (const char*)Parameter_List[3].String_Value, max_pin_value_len+1);

 info_request.req.pin_change_req  = &pin_change_req;
 info_response.resp.pin_resp = &pin_resp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_CHANGE_PIN_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {

    DEVINFO_PRINTF("request_device_change_pin: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    printf_uim_pin_response(&pin_resp);
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
    DEVINFO_PRINTF("request_device_change_pin: Usage: [-app <app_id>] request changepin <pin-id> <old-pin-value> <new-pin-value>\n");
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_unblock_pin(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Req_Unblock_Pin_t           pin_unblock_req;
 qapi_Device_Info_Pin_Response_t  pin_resp ;
 uint8_t max_pin_value_len = sizeof(pin_unblock_req.new_pin_value) - 1;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&pin_unblock_req,  0, sizeof(qapi_Device_Info_Req_Unblock_Pin_t));
 memset(&pin_resp, 0, sizeof(qapi_Device_Info_Pin_Response_t));


 if((Parameter_Count != 4) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 if(Parameter_List[1].Integer_Value != QAPI_DEVICE_INFO_PIN_ID_PIN1  &&
 	Parameter_List[1].Integer_Value != QAPI_DEVICE_INFO_PIN_ID_PIN2  &&
 	Parameter_List[1].Integer_Value != QAPI_DEVICE_INFO_PIN_ID_UNIVERSER_PIN )
    goto usage_end;

 if(strlen((const char*)Parameter_List[2].String_Value) == 0 ||
 	strlen((const char*)Parameter_List[2].String_Value) > max_pin_value_len ||
 	strlen((const char*)Parameter_List[3].String_Value) == 0 ||
 	strlen((const char*)Parameter_List[3].String_Value) > max_pin_value_len)
 	goto usage_end;


 pin_unblock_req.pin_id = (qapi_Device_Info_Pin_Id_t)Parameter_List[1].Integer_Value;
 strlcpy(pin_unblock_req.puk_value,     (const char*)Parameter_List[2].String_Value, max_pin_value_len+1);
 strlcpy(pin_unblock_req.new_pin_value, (const char*)Parameter_List[3].String_Value, max_pin_value_len+1);

 info_request.req.pin_unblock_req  = &pin_unblock_req;
 info_response.resp.pin_resp = &pin_resp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_UNBLOCK_PIN_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {

    DEVINFO_PRINTF("request_device_unblock_pin: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    printf_uim_pin_response(&pin_resp);
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
    DEVINFO_PRINTF("request_device_unblock_pin: Usage: [-app <app_id>] request unblockpin <pin-id> <puk-value> <new-pin-value>\n");
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_protect_pin(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Req_Protect_Pin_t           pin_protect_req;
 qapi_Device_Info_Pin_Response_t pin_resp;
 uint8_t max_pin_value_len = sizeof(pin_protect_req.pin_value) - 1;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&pin_protect_req,  0, sizeof(qapi_Device_Info_Req_Protect_Pin_t));
 memset(&pin_resp, 0, sizeof(qapi_Device_Info_Pin_Response_t));

 if((Parameter_Count != 4) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("%s : Parameter_Count[%d], expect 3\n", __func__, Parameter_Count);
    goto usage_end;
 }

 if(Parameter_List[1].Integer_Value != QAPI_DEVICE_INFO_PIN_ID_PIN1 &&
 	Parameter_List[1].Integer_Value != QAPI_DEVICE_INFO_PIN_ID_PIN2 &&
 	Parameter_List[1].Integer_Value != QAPI_DEVICE_INFO_PIN_ID_UNIVERSER_PIN)
 {
    DEVINFO_PRINTF("%s : PIN-id[%d], expect 0/1/2\n", __func__, Parameter_List[1].Integer_Value);
    goto usage_end;
 }

 if(strlen((const char*)Parameter_List[2].String_Value) == 0 ||
 	strlen((const char*)Parameter_List[2].String_Value) > max_pin_value_len)
 {
    DEVINFO_PRINTF("%s : PIN-Lenth[%d], expect (0,8]\n", __func__, strlen((const char*)Parameter_List[2].String_Value));
 	goto usage_end;
 }

 if(Parameter_List[3].Integer_Value != 0 && Parameter_List[3].Integer_Value != 1)
 {
    DEVINFO_PRINTF("%s : expect [0,1] on the last parameter\n", __func__);
 	goto usage_end;
 }

 pin_protect_req.pin_id = (qapi_Device_Info_Pin_Id_t)Parameter_List[1].Integer_Value;
 strlcpy(pin_protect_req.pin_value,(const char*) Parameter_List[2].String_Value, max_pin_value_len+1);
 pin_protect_req.isEnablePin = Parameter_List[3].Integer_Value;

 info_request.req.pin_protect_req  = &pin_protect_req;
 info_response.resp.pin_resp = &pin_resp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_PROTECT_PIN_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {

    DEVINFO_PRINTF("%s: qapi_Device_Info_Request error %d\n", __func__, ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    printf_uim_pin_response(&pin_resp);
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
    DEVINFO_PRINTF("%s: Usage: [-app <app_id>] request protectpin <pin-id> <pin-value> <pin-status>\n", __func__);
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}


QCLI_Command_Status_t request_device_set_fdn_status(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Req_Set_FDN_t    set_fdn_req;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&set_fdn_req,  0, sizeof(qapi_Device_Info_Req_Set_FDN_t));

 if((Parameter_Count != 2) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("%s : Parameter_Count[%d], expect 3\n", __func__, Parameter_Count);
    goto usage_end;
 }

 if(Parameter_List[1].Integer_Value != 0 &&
 	Parameter_List[1].Integer_Value != 1)
 {
    DEVINFO_PRINTF("%s : expect [0,1] on the last parameter\n", __func__);
    goto usage_end;
 }

 set_fdn_req.isEnableFDN = Parameter_List[1].Integer_Value;

 info_request.req.set_fdn_req  = &set_fdn_req;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SET_FDN_STATUS_E, &info_request, NULL);
 if(ret != QAPI_OK)
 {

    DEVINFO_PRINTF("%s: qapi_Device_Info_Request error %d\n", __func__ , ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
    DEVINFO_PRINTF("%s: Usage: [-app <app_id>] set fdn <fdn-status>\n", __func__);
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_get_fdn_status(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Get_FDN_Rsponse_t          get_fdn_rsp;

 memset(&info_response,  0, sizeof(qapi_Device_Info_Response_t));
 memset(&get_fdn_rsp, 0, sizeof(qapi_Device_Info_Get_FDN_Rsponse_t));

 if((Parameter_Count != 1) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("%s : Parameter_Count[%d], expect no more parameter\n", __func__,Parameter_Count);
    goto usage_end;
 }

 info_response.resp.get_fdn_rsp = &get_fdn_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_FDN_STATUS_E, NULL, &info_response);
 if(ret != QAPI_OK)
 {

    DEVINFO_PRINTF("%s: qapi_Device_Info_Request error %d\n", __func__ , ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("%s: FDN-Status %d\n", __func__, get_fdn_rsp.fdn_status);
    DEVINFO_PRINTF("QAPI success: \n");
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
    DEVINFO_PRINTF("%s: Usage: [-app <app_id>] request get fdn\n", __func__);
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_jamming_status(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Jamming_Status_Get_Req_t   jamming_status_req;
 qapi_Device_Info_Jamming_Status_Get_Rsp_t   jamming_status_rsp;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&jamming_status_req,  0, sizeof(qapi_Device_Info_Jamming_Status_Get_Req_t));
 memset(&jamming_status_rsp, 0, sizeof(qapi_Device_Info_Jamming_Status_Get_Rsp_t));

 if(NULL == app_devinfo_cli_ctx[app_id])
 {
    DEVINFO_PRINTF("%s : Device info not initialized, Please do init first \n", __func__);
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
 }

 if((Parameter_Count != 2) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("%s : Parameter_Count[%d], expect no more parameter\n", __func__, Parameter_Count);
    goto usage_end;
 }

 if(strcasecmp((const char*)Parameter_List[1].String_Value, "nb1") == 0)
     jamming_status_req.rat_type = QAPI_DEVICE_INFO_LTE_NB1;
 else if(strcasecmp((const char*)Parameter_List[1].String_Value, "m1") == 0)
     jamming_status_req.rat_type = QAPI_DEVICE_INFO_LTE_M1;
 else if(strcasecmp((const char*)Parameter_List[1].String_Value, "gsm") == 0)
 	 jamming_status_req.rat_type = QAPI_DEVICE_INFO_GSM;
 else
 {
    goto usage_end;
 }
 info_request.req.jamming_status_req = &jamming_status_req;
 info_response.resp.jamming_status_rsp = &jamming_status_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_JAMMER_STATUS_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {

    DEVINFO_PRINTF("%s: qapi_Device_Info_Request error %d\n", __func__ , ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    printf_device_info_jamming_status(&jamming_status_rsp);
    DEVINFO_PRINTF("QAPI success: \n");
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
    DEVINFO_PRINTF("%s: Usage: [-app <app_id>] request jamming_status <nb1/m1/gsm>\n", __func__);
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_set_ims_iot_ind_register(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int                                         ret         = QAPI_ERROR;
  QCLI_Command_Status_t                       result      = QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t                  info_request;
  qapi_Device_Info_IMS_IOT_Ind_Register_Req_t ind_reg_req;

  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&ind_reg_req,  0, sizeof(qapi_Device_Info_IMS_IOT_Ind_Register_Req_t));

  if((Parameter_Count != 2) || (Parameter_List == NULL))
  {
    DEVINFO_PRINTF("%s : Parameter_Count[%d], expect 3\n", __func__, Parameter_Count);
    goto usage_end;
  }

  if(Parameter_List[1].Integer_Value != 0 &&
  Parameter_List[1].Integer_Value != 1)
  {
    DEVINFO_PRINTF("%s : expect [0,1] on the last parameter\n", __func__);
    goto usage_end;
  }

  ind_reg_req.srv_status_change_ind = Parameter_List[1].Integer_Value;

  info_request.req.ind_register_req  = &ind_reg_req;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SET_IMS_IOT_IND_REGISTER_E, &info_request, NULL);
  if(ret != QAPI_OK)
  {

    DEVINFO_PRINTF("%s: qapi_Device_Info_Request error %d\n", __func__ , ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    DEVINFO_PRINTF("QAPI success: \n");
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

usage_end:
  DEVINFO_PRINTF("%s: Usage: [-app <app_id>] request setimsindreg <enable_ims_iot_ind: 0|1>\n", __func__);
  result = QCLI_STATUS_USAGE_E;

end:
  return result;
}

QCLI_Command_Status_t request_device_get_ims_register_status(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Response_t info_response;
  qapi_Device_Info_IMS_Registration_Status_Rsp_t ims_reg_status_rsp;

  if(NULL == app_devinfo_cli_ctx[app_id])
  {
    DEVINFO_PRINTF("%s : Device info not initialized, Please do init first \n", __func__);
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if((Parameter_Count != 1) || (Parameter_List == NULL))
  {
    DEVINFO_PRINTF("%s : Parameter_Count[%d], expect no more parameter\n", __func__);
    goto usage_end;
  }

  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
  memset(&ims_reg_status_rsp, 0, sizeof(qapi_Device_Info_IMS_Registration_Status_Rsp_t));

  info_response.resp.ims_registration_rsp = &ims_reg_status_rsp;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id],
                                QAPI_DEVICE_INFO_REQ_GET_IMS_REGISTRATION_E,
                                NULL, &info_response);
  if(ret != QAPI_OK)
  {

    DEVINFO_PRINTF("%s: qapi_Device_Info_Request error %d\n", __func__ , ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    if(ims_reg_status_rsp.registration_status <= QAPI_DEVICE_INFO_IMS_IOT_STATUS_MAX_V01)
      DEVINFO_PRINTF("%s:ims registration status: [%s]\n",__func__, reg_status_str[ims_reg_status_rsp.registration_status].str);
    else
      DEVINFO_PRINTF("%s: IMS register status [%d] is invalid\n", __func__ , ims_reg_status_rsp.registration_status);
    DEVINFO_PRINTF("QAPI success: \n");
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

  usage_end:
    DEVINFO_PRINTF("%s: Usage: [-app <app_id>] request imsregstatus\n", __func__);
    result = QCLI_STATUS_USAGE_E;

  end:

  return result;
}

QCLI_Command_Status_t request_device_dial_call(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int                           ret = QAPI_ERROR;
  QCLI_Command_Status_t      result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Response_t info_response;
  qapi_Device_Info_Call_Req_t   call_req;
  qapi_Device_Info_Call_Status_Rsp_t   call_rsp;

  if(NULL == app_devinfo_cli_ctx[app_id])
  {
    DEVINFO_PRINTF("%s : Device info not initialized, Please do init first \n", __func__);
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if((Parameter_Count != 2) || (Parameter_List == NULL))
  {
    DEVINFO_PRINTF("%s : Parameter_Count[%d], expect no more parameter\n", __func__, Parameter_Count);
    goto usage_end;
  }

  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
  memset(&call_req,  0, sizeof(qapi_Device_Info_Call_Req_t));
  memset(&call_rsp, 0, sizeof(qapi_Device_Info_Call_Status_Rsp_t));

  call_req.call_req_type = QAPI_DEVICE_INFO_VOICE_DIAL_CALL_REQ;

  if(strlen((const char*)Parameter_List[1].String_Value) > MAX_CALL_NUM_LENGTH)
    goto usage_end;

  memscpy(call_req.callReq.calling_number, MAX_CALL_NUM_LENGTH + 1,
          (const char*)Parameter_List[1].String_Value, strlen((const char*)Parameter_List[1].String_Value));

  info_request.req.call_req = &call_req;
  info_response.resp.call_status_rsp = &call_rsp;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_VOLTE_CALL_E, &info_request, &info_response);
  if(ret != QAPI_OK)
  {
    DEVINFO_PRINTF("%s: qapi_Device_Info_Request error %d\n", __func__ , ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    DEVINFO_PRINTF("Call id is: %d\n", call_rsp.call_id);
    DEVINFO_PRINTF("QAPI success: \n");
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

  usage_end:
    DEVINFO_PRINTF("%s: Usage: [-app <app_id>] request dialcall <calling_number>\n", __func__);
    result = QCLI_STATUS_USAGE_E;

  end:
    return result;
}

QCLI_Command_Status_t request_device_end_call(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int                           ret = QAPI_ERROR;
  QCLI_Command_Status_t      result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Response_t info_response;
  qapi_Device_Info_Call_Req_t   call_req;
  qapi_Device_Info_Call_Status_Rsp_t   call_rsp;

  if(NULL == app_devinfo_cli_ctx[app_id])
  {
    DEVINFO_PRINTF("%s : Device info not initialized, Please do init first \n", __func__);
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if(((Parameter_Count != 2) && (Parameter_Count != 3))
      || (Parameter_List == NULL))
  {
    DEVINFO_PRINTF("%s : Parameter_Count[%d], expect no more parameter\n", __func__, Parameter_Count);
    goto usage_end;
  }

  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
  memset(&call_req,  0, sizeof(qapi_Device_Info_Call_Req_t));
  memset(&call_rsp, 0, sizeof(qapi_Device_Info_Call_Status_Rsp_t));

  call_req.call_req_type = QAPI_DEVICE_INFO_VOICE_END_CALL_REQ;

  if(0xFF<= Parameter_List[1].Integer_Value || Parameter_List[1].Integer_Value <0)
    goto usage_end;

  call_req.callReq.endCallReq.call_id = Parameter_List[1].Integer_Value;

  if(3 == Parameter_Count)
  {
    if(strcasecmp((const char*)Parameter_List[2].String_Value, "user_busy") == 0)
      call_req.callReq.endCallReq.reject_cause = QAPI_DEVICE_INFO_Call_REJECT_CAUSE_USER_BUSY;
    else if(strcasecmp((const char*)Parameter_List[2].String_Value, "user_reject") == 0)
      call_req.callReq.endCallReq.reject_cause = QAPI_DEVICE_INFO_Call_REJECT_CAUSE_USER_REJECT;
    else if(strcasecmp((const char*)Parameter_List[2].String_Value, "low_battery") == 0)
      call_req.callReq.endCallReq.reject_cause = QAPI_DEVICE_INFO_Call_REJECT_CAUSE_LOW_BATTERY;
    else if(strcasecmp((const char*)Parameter_List[2].String_Value, "blacklist") == 0)
      call_req.callReq.endCallReq.reject_cause = QAPI_DEVICE_INFO_Call_REJECT_CAUSE_BLACKLISTED_CALL_ID;
    else if(strcasecmp((const char*)Parameter_List[2].String_Value, "dead_battery") == 0)
      call_req.callReq.endCallReq.reject_cause = QAPI_DEVICE_INFO_Call_REJECT_CAUSE_DEAD_BATTERY;
    else if(strcasecmp((const char*)Parameter_List[2].String_Value, "unwanted_call") == 0)
      call_req.callReq.endCallReq.reject_cause = QAPI_DEVICE_INFO_Call_REJECT_CAUSE_UNWANTED_CALL;
    else
    {
       goto usage_end;
    }
  }

  info_request.req.call_req = &call_req;
  info_response.resp.call_status_rsp = &call_rsp;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_VOLTE_CALL_E, &info_request, &info_response);
  if(ret != QAPI_OK)
  {
    DEVINFO_PRINTF("%s: qapi_Device_Info_Request error %d\n", __func__ , ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    DEVINFO_PRINTF("Call id is: %d\n", call_rsp.call_id);
    DEVINFO_PRINTF("QAPI success: \n");
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

  usage_end:
    DEVINFO_PRINTF("%s: Usage: [-app <app_id>] request endcall <call_id> <reject_cause: user_busy|user_reject|low_battery|low_battery|blacklist|dead_battery|unwanted_call>\n", __func__);
    result = QCLI_STATUS_USAGE_E;

  end:
    return result;
}

QCLI_Command_Status_t request_device_answer_call(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int                           ret = QAPI_ERROR;
  QCLI_Command_Status_t      result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Response_t info_response;
  qapi_Device_Info_Call_Req_t   call_req;
  qapi_Device_Info_Call_Status_Rsp_t   call_rsp;

  if(NULL == app_devinfo_cli_ctx[app_id])
  {
    DEVINFO_PRINTF("%s : Device info not initialized, Please do init first \n", __func__);
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if(((Parameter_Count != 2) && (Parameter_Count != 3) && (Parameter_Count != 4))
      || (Parameter_List == NULL))
  {
    DEVINFO_PRINTF("%s : Parameter_Count[%d], expect no more parameter\n", __func__, Parameter_Count);
    goto usage_end;
  }

  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
  memset(&call_req,  0, sizeof(qapi_Device_Info_Call_Req_t));
  memset(&call_rsp, 0, sizeof(qapi_Device_Info_Call_Status_Rsp_t));

  call_req.call_req_type = QAPI_DEVICE_INFO_VOICE_ANSWER_CALL_REQ;

  if(0xFF<= Parameter_List[1].Integer_Value || Parameter_List[1].Integer_Value <0)
    goto usage_end;

  call_req.callReq.answerCallReq.call_id = Parameter_List[1].Integer_Value;

  if(Parameter_Count > 2)
  {
    if(1 == Parameter_List[2].Integer_Value)
    {
      call_req.callReq.answerCallReq.reject_call = TRUE;
      if(4== Parameter_Count)
      {
        if(strcasecmp((const char*)Parameter_List[3].String_Value, "user_busy") == 0)
          call_req.callReq.answerCallReq.reject_cause = QAPI_DEVICE_INFO_Call_REJECT_CAUSE_USER_BUSY;
        else if(strcasecmp((const char*)Parameter_List[3].String_Value, "user_reject") == 0)
          call_req.callReq.answerCallReq.reject_cause = QAPI_DEVICE_INFO_Call_REJECT_CAUSE_USER_REJECT;
        else if(strcasecmp((const char*)Parameter_List[3].String_Value, "low_battery") == 0)
          call_req.callReq.answerCallReq.reject_cause = QAPI_DEVICE_INFO_Call_REJECT_CAUSE_LOW_BATTERY;
        else if(strcasecmp((const char*)Parameter_List[3].String_Value, "blacklist") == 0)
          call_req.callReq.answerCallReq.reject_cause = QAPI_DEVICE_INFO_Call_REJECT_CAUSE_BLACKLISTED_CALL_ID;
        else if(strcasecmp((const char*)Parameter_List[3].String_Value, "dead_battery") == 0)
          call_req.callReq.answerCallReq.reject_cause = QAPI_DEVICE_INFO_Call_REJECT_CAUSE_DEAD_BATTERY;
        else if(strcasecmp((const char*)Parameter_List[3].String_Value, "unwanted_call") == 0)
          call_req.callReq.answerCallReq.reject_cause = QAPI_DEVICE_INFO_Call_REJECT_CAUSE_UNWANTED_CALL;
        else
        {
           goto usage_end;
        }
      }
    }
    else if(0 == Parameter_List[2].Integer_Value)
    {
      call_req.callReq.answerCallReq.reject_call = FALSE;
      //ignore following prarameter(s)...
    }
    else
    {
      goto usage_end;
    }
  }

  info_request.req.call_req = &call_req;
  info_response.resp.call_status_rsp = &call_rsp;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_VOLTE_CALL_E, &info_request, &info_response);
  if(ret != QAPI_OK)
  {
    DEVINFO_PRINTF("%s: qapi_Device_Info_Request error %d\n", __func__ , ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    DEVINFO_PRINTF("Call id is: %d\n", call_rsp.call_id);
    DEVINFO_PRINTF("QAPI success: \n");
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

  usage_end:
    DEVINFO_PRINTF("%s: Usage: [-app <app_id>] request answercall <call_id> <reject call:0|1> <reject_cause: user_busy|user_reject|low_battery|low_battery|blacklist|dead_battery|unwanted_call>\n", __func__);
    result = QCLI_STATUS_USAGE_E;

  end:
    return result;
}

/*****************************************************************************
 * set_device_info_app_priority
 *****************************************************************************/
QCLI_Command_Status_t set_device_info_app_priority(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int p;
  int val[1]={0};
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;


  if(NULL == app_devinfo_cli_ctx[app_id])
  {
    DEVINFO_PRINTF("set_device_info_app_priority : Device info not initialized, Please do init first \n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if((Parameter_Count < 3) || (Parameter_List == NULL))
  {
    DEVINFO_PRINTF("set_device_info_app_priority : Usage: [-app <app_id>] set app_priority -g <0/1> , Device Info CLI GET2 command\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }
  for (p = 0; p < Parameter_Count ; p++)
  {
     if(strcmp((char*)Parameter_List[p].String_Value, "-g") == 0)
     {
        ++p;
        if(p > Parameter_Count)
        {
           DEVINFO_PRINTF("Client group is required\n");
           result = QCLI_STATUS_USAGE_E;
           goto end;
        }
        else
        {
           val[0] = Parameter_List[p].Integer_Value;
        }
     }
  }
  ret = qapi_Device_Info_Set(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_SET_APP_PRIORITY_E, 1, val);
  if(ret != QAPI_OK)
  {
    DEVINFO_PRINTF("set_device_info_app_priority: returned error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
     DEVINFO_PRINTF("set_device_info_app_priority: success \n");
  }

end:
  return result;
}

/*****************************************************************************
 * set_device_info_nz_bsr
 *****************************************************************************/
QCLI_Command_Status_t set_device_info_nz_bsr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int p;
  int val[1]={0};
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;


  if(NULL == app_devinfo_cli_ctx[app_id])
  {
    DEVINFO_PRINTF("set_device_info_nz_bsr : Device info not initialized, Please do init first \n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if((Parameter_Count < 3) || (Parameter_List == NULL))
  {
    DEVINFO_PRINTF("set_device_info_nz_bsr : Usage: [-app <app_id>] set nz_bsr -o <0/1> , Device Info CLI GET2 command\n");
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }
  for (p = 0; p < Parameter_Count ; p++)
  {
     if(strcmp((char*)Parameter_List[p].String_Value, "-o") == 0)
     {
        ++p;
        if(p > Parameter_Count)
        {
           DEVINFO_PRINTF("Client BSR optimization is required\n");
           result = QCLI_STATUS_USAGE_E;
           goto end;
        }
        else
        {
           val[0] = Parameter_List[p].Integer_Value;
        }
     }
  }
  ret = qapi_Device_Info_Set(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_SET_NZ_BSR_E, 1, val);
  if(ret != QAPI_OK)
  {
    DEVINFO_PRINTF("set_device_info_nz_bsr: returned error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
     DEVINFO_PRINTF("set_device_info_nz_bsr: success \n");
  }

end:
  return result;
}

/***********************************************
    request_device_get_edrx
 ***********************************************/
QCLI_Command_Status_t request_device_get_edrx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Req_EDRX_Get_t get_edrx_req;
 qapi_Device_Info_Rsp_EDRX_Get_t get_edrx_rsp;

 if((Parameter_Count != 2) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&get_edrx_req,  0, sizeof(qapi_Device_Info_Req_EDRX_Get_t));
 memset(&get_edrx_rsp,  0, sizeof(qapi_Device_Info_Rsp_EDRX_Get_t));

 if(Parameter_List[1].Integer_Is_Valid &&
    (Parameter_List[1].Integer_Value == QAPI_DEVICE_INFO_EDRX_ACT_LTE_M1 || Parameter_List[1].Integer_Value == QAPI_DEVICE_INFO_EDRX_ACT_LTE_NB1))
 {
    get_edrx_req.act_type = (qapi_Device_Info_EDRX_Act_Type_t)Parameter_List[1].Integer_Value;
 }
 else
 {
    DEVINFO_PRINTF("%s: AcT Type need to be M1(4) or NB1(5)\n", __func__);
    goto usage_end;
 }

 info_request.req.get_edrx_req = &get_edrx_req;
 info_response.resp.get_edrx_rsp = &get_edrx_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_EDRX_GET_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("%s: qapi_Device_Info_Request returned error %d\n",__func__, ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("%s: REQUEST success\n", __func__);
    DEVINFO_PRINTF("  eDRX mode = %d\n", get_edrx_rsp.edrx_mode);
    DEVINFO_PRINTF("  AcT Type = %d\n",  get_edrx_rsp.act_type);
    DEVINFO_PRINTF("  eDRX value = %d\n",get_edrx_rsp.edrx_value);
    DEVINFO_PRINTF("  eDRX ptw = %d\n",  get_edrx_rsp.paging_time_window);
    result = QCLI_STATUS_SUCCESS_E;
   goto end;
 }

usage_end:
 DEVINFO_PRINTF("%s: Usage: [-app <app_id>] request getedrx <AcT>\n", __func__);
 result = QCLI_STATUS_USAGE_E;

end:
 return result;
}
/***********************************************
    request_device_set_edrx
 ***********************************************/
QCLI_Command_Status_t request_device_set_edrx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Req_EDRX_Set_t set_edrx_req;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&set_edrx_req, 0, sizeof(qapi_Device_Info_Req_EDRX_Set_t));

 if((Parameter_Count != 3 && Parameter_Count != 4) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 set_edrx_req.edrx_value = QAPI_DEVICE_INFO_EDRX_VALUE_NOT_USED;

 if(Parameter_List[1].Integer_Is_Valid &&
    (Parameter_List[1].Integer_Value == 0 || Parameter_List[1].Integer_Value == 1))
 {
    set_edrx_req.edrx_mode = (Parameter_List[1].Integer_Value == 1)? TRUE : FALSE;
 }
 else
 {
    goto usage_end;
 }
 if(Parameter_List[2].Integer_Is_Valid &&
    (Parameter_List[2].Integer_Value == QAPI_DEVICE_INFO_EDRX_ACT_LTE_M1 || Parameter_List[2].Integer_Value == QAPI_DEVICE_INFO_EDRX_ACT_LTE_NB1))
 {
   set_edrx_req.act_type = (qapi_Device_Info_EDRX_Act_Type_t)Parameter_List[2].Integer_Value;
 }
 else
 {
    goto usage_end;
 }
 if(Parameter_Count == 4)
 {
    if(Parameter_List[1].Integer_Value == 1 && Parameter_List[3].Integer_Is_Valid)
    {
       set_edrx_req.edrx_value = (uint8_t)Parameter_List[3].Integer_Value;
    }
    else
    {
       DEVINFO_PRINTF("%s: only allow <edrx_value> when enable edrx\n", __func__);
       goto usage_end;
    }
 }

 info_request.req.set_edrx_req = &set_edrx_req;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_EDRX_SET_E, &info_request, NULL);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("%s : qapi_Device_Info_Request returned error %d\n", __func__, ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("%s: REQUEST success\n", __func__);
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
 }

usage_end:
 DEVINFO_PRINTF("%s: Usage: [-app <app_id>] request setedrx <edrx_enable> <AcT> [<edrx_value>]\n", __func__);
 result = QCLI_STATUS_USAGE_E;

end:
  return result;
}

QCLI_Command_Status_t request_device_get_qcpdpp(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Get_QCPDPP_Req_t   qcpdpp_req;
 qapi_Device_Info_Get_QCPDPP_Rsp_t  qcpdpp_rsp;
 int i = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&qcpdpp_req,  0, sizeof(qapi_Device_Info_Get_QCPDPP_Req_t));
 memset(&qcpdpp_rsp, 0, sizeof(qapi_Device_Info_Get_QCPDPP_Rsp_t));

 if((Parameter_Count != 1) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 info_request.req.get_qcpdpp_req  = &qcpdpp_req;
 info_response.resp.get_qcpdpp_rsp = &qcpdpp_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_QCPDPP_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request qcpdpp: qapi_Device_Info_Request error %d\n", ret);
    printf_pdp_error_code((qcpdpp_rsp.response));
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    qapi_Device_Info_Get_QCPDPP_t *get_list = NULL;
    get_list = qcpdpp_rsp.list;
    DEVINFO_PRINTF("QAPI success: \n");

    if(qcpdpp_rsp.response != QAPI_DEVICE_INFO_PDP_NO_ERROR)
    {
      printf_pdp_error_code((qcpdpp_rsp.response));
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    DEVINFO_PRINTF("response:total number[%d]", qcpdpp_rsp.count);
    for(i = 0; i< qcpdpp_rsp.count && get_list; i++)
    {
       DEVINFO_PRINTF("\n PDP type %d:\n", get_list[i].pdp_type);
       if(get_list[i].apn_name_valid)
         DEVINFO_PRINTF("  APN name %s\n", get_list[i].apn_name);
       if(get_list[i].auth_type_valid)
         DEVINFO_PRINTF("  Authentication preference %d\n", get_list[i].auth_type);
       if(get_list[i].username_valid)
         DEVINFO_PRINTF("  Username %s\n", get_list[i].username);
    }
    if(get_list)
      FREE_MEMORY(get_list);
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request qcpdpp: Usage: [-app <app_id>] get_qcpdpp \n");
  result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_get_cgdcont(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Get_CGDCONT_Req_t   cgdcont_req;
 qapi_Device_Info_Get_CGDCONT_Rsp_t  cgdcont_rsp;
 int i = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&cgdcont_req,  0, sizeof(qapi_Device_Info_Get_CGDCONT_Req_t));
 memset(&cgdcont_rsp, 0, sizeof(qapi_Device_Info_Get_CGDCONT_Rsp_t));

 if((Parameter_Count != 1) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 info_request.req.get_cgdcont_req  = &cgdcont_req;
 info_response.resp.get_cgdcont_rsp = &cgdcont_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_CGDCONT_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request cgdcont: qapi_Device_Info_Request error %d\n", ret);
    printf_pdp_error_code((cgdcont_rsp.response));
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    qapi_Device_Info_Get_CGDCONT_t *get_list = NULL;
    get_list = cgdcont_rsp.list;
    DEVINFO_PRINTF("QAPI success: \n");

    if(cgdcont_rsp.response != QAPI_DEVICE_INFO_PDP_NO_ERROR)
    {
      printf_pdp_error_code((cgdcont_rsp.response));
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    DEVINFO_PRINTF("response:total number[%d]", cgdcont_rsp.count);
    for(i = 0; i< cgdcont_rsp.count && get_list != NULL; i++)
    {
       DEVINFO_PRINTF("\n PDP type %d:\n", get_list[i].addr_info.pdp_type);
       if(get_list[i].addr_info.apn_name_valid)
         DEVINFO_PRINTF("  APN name %s\n", get_list[i].addr_info.apn_name);
       if(get_list[i].d_comp_valid)
         DEVINFO_PRINTF("  Data compression type %d\n", get_list[i].d_comp);
       if(get_list[i].h_comp_valid)
         DEVINFO_PRINTF("  Header compression type  %d\n", get_list[i].h_comp);
       if(get_list[i].ipv4AddrAlloc_valid)
         DEVINFO_PRINTF(" IPv4 address alloc type  %d\n", get_list[i].ipv4AddrAlloc);
    }
    if(get_list)
      FREE_MEMORY(get_list);

 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request cgdcont: Usage: [-app <app_id>] get_cgdcont \n");
  result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_get_qcpdpimscfge(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Get_QCPDPIMSCFGE_Req_t   qcpdpimscfge_req;
 qapi_Device_Info_Get_QCPDPIMSCFGE_Rsp_t  qcpdpimscfge_rsp;
 int i = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&qcpdpimscfge_req,  0, sizeof(qapi_Device_Info_Get_QCPDPIMSCFGE_Req_t));
 memset(&qcpdpimscfge_rsp, 0, sizeof(qapi_Device_Info_Get_QCPDPIMSCFGE_Rsp_t));

 if((Parameter_Count != 1) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 info_request.req.get_qcpdpimscfg_req  = &qcpdpimscfge_req;
 info_response.resp.get_qcpdpimscfg_rsp = &qcpdpimscfge_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_QCPDPIMSCFG_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request qcpdpimscfge: qapi_Device_Info_Request error %d\n", ret);
    printf_pdp_error_code((qcpdpimscfge_rsp.response));
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    qapi_Device_Info_Get_QCPDPIMSCFGE_t *get_list = NULL;
    get_list = qcpdpimscfge_rsp.list;
    DEVINFO_PRINTF("QAPI success: \n");

    if(qcpdpimscfge_rsp.response != QAPI_DEVICE_INFO_PDP_NO_ERROR)
    {
      printf_pdp_error_code((qcpdpimscfge_rsp.response));
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    DEVINFO_PRINTF("response:total number[%d]", qcpdpimscfge_rsp.count);
    for(i = 0; i < qcpdpimscfge_rsp.count && get_list != NULL; i++)
    {
       DEVINFO_PRINTF("\n PDP type %d:\n", get_list[i].pdp_type);
       if( get_list[i].apn_name_valid)
         DEVINFO_PRINTF("  APN name %s\n",  get_list[i].apn_name);
       if( get_list[i].address_flag_valid)
         DEVINFO_PRINTF("  Address flag %d\n",  get_list[i].address_flag);
       if( get_list[i].dhcp_flag_valid)
         DEVINFO_PRINTF("  DHCP flag %d\n",  get_list[i].dhcp_flag);
       if( get_list[i].cn_flag_valid)
         DEVINFO_PRINTF("  CN flag %d\n",  get_list[i].cn_flag);
    }
    if(get_list)
      FREE_MEMORY(get_list);
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request qcpdpimscfge: Usage: [-app <app_id>] qcpdpimscfge \n");
  result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_get_qcpdpcfge(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Get_QCPDPCFGE_Req_t   qcpdpcfge_req;
 qapi_Device_Info_Get_QCPDPCFGE_Rsp_t  qcpdpcfge_rsp;
 int i = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&qcpdpcfge_req,  0, sizeof(qapi_Device_Info_Get_QCPDPCFGE_Req_t));
 memset(&qcpdpcfge_rsp, 0, sizeof(qapi_Device_Info_Get_QCPDPCFGE_Rsp_t));

 if((Parameter_Count != 1) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 info_request.req.get_qcpdpcfg_req  = &qcpdpcfge_req;
 info_response.resp.get_qcpdpcfg_rsp = &qcpdpcfge_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_QCPDPCFG_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request qcpdpcfge: qapi_Device_Info_Request error %d\n", ret);
    printf_pdp_error_code((qcpdpcfge_rsp.response));
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    qapi_Device_Info_Get_QCPDPCFGE_t *get_list = NULL;
    get_list = qcpdpcfge_rsp.list;
    DEVINFO_PRINTF("QAPI success: \n");

    if(qcpdpcfge_rsp.response != QAPI_DEVICE_INFO_PDP_NO_ERROR)
    {
      printf_pdp_error_code((qcpdpcfge_rsp.response));
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    DEVINFO_PRINTF("response:total number[%d]", qcpdpcfge_rsp.count);
    for(i = 0; i< qcpdpcfge_rsp.count && get_list != NULL; i++)
    {
       DEVINFO_PRINTF("\n PDP type %d:\n", get_list[i].pdp_type);
       if(get_list[i].apn_name_valid)
         DEVINFO_PRINTF("  APN name %s\n", get_list[i].apn_name);
       if(get_list[i].apn_disable_flag_valid)
         DEVINFO_PRINTF("  APN disable flag %d\n", get_list[i].apn_disable_flag);
       if(get_list[i].timer_value_valid)
         DEVINFO_PRINTF("  Timer value %d\n", get_list[i].timer_value);
       if(get_list[i].apn_class_valid)
         DEVINFO_PRINTF("  APN class %d\n", get_list[i].apn_class);
       if(get_list[i].apn_bearer_valid)
         DEVINFO_PRINTF("  APN bearer %d\n", get_list[i].apn_bearer);
       if(get_list[i].max_pdn_conn_per_block_valid)
         DEVINFO_PRINTF("  Max PDN conn per block %d\n", get_list[i].max_pdn_conn_per_block);
       if(get_list[i].max_pdn_conn_timer_valid)
         DEVINFO_PRINTF("  Max PDN conn timer %d\n", get_list[i].max_pdn_conn_timer);
       if(get_list[i].pdn_req_wait_timer_valid)
         DEVINFO_PRINTF("  PDN request wait timer %d\n", get_list[i].pdn_req_wait_timer);
       if(get_list[i].operator_reserved_pco_valid)
         DEVINFO_PRINTF("  Operator reserved PCO %d\n", get_list[i].operator_reserved_pco);
       if(get_list[i].mcc_valid)
         DEVINFO_PRINTF("  MCC %d\n", get_list[i].mcc);
       if(get_list[i].mnc_valid )
       {
         if(get_list[i].mnc.mnc < 99 && get_list[i].mnc.mnc_includes_pcs_digit == TRUE)
           DEVINFO_PRINTF("  MNC 0%d\n", get_list[i].mnc.mnc);
         else
           DEVINFO_PRINTF("  MNC %d\n", get_list[i].mnc.mnc);
       }
    }
    if(get_list)
      FREE_MEMORY(get_list);
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request qcpdpp: Usage: [-app <app_id>] get_qcpdpp \n");
  result = QCLI_STATUS_USAGE_E;

end:

  return result;
}
QCLI_Command_Status_t request_device_set_qcpdpp(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_reponse;
 qapi_Device_Info_Set_QCPDPP_Req_t    qcpdpp_set_req;
 qapi_Device_Info_Set_QCPDPP_Rsp_t    qcpdpp_set_rsp;
 uint32_t flag = 0, bitmask = 1;
 int i = 2;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_reponse,  0, sizeof(qapi_Device_Info_Response_t));
 memset(&qcpdpp_set_req,  0, sizeof(qapi_Device_Info_Set_QCPDPP_Req_t));
 memset(&qcpdpp_set_rsp,  0, sizeof(qapi_Device_Info_Set_QCPDPP_Rsp_t));

 if((Parameter_Count < 4) || (Parameter_Count > 7) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 if(Parameter_List[1].Integer_Value < 3)
 {
   goto usage_end;
 }

 flag = Parameter_List[1].Integer_Value;

 do
 {
   switch(Parameter_List[1].Integer_Value & bitmask)
   {
     case 1:
     {
       qcpdpp_set_req.pdp_type = (qapi_Device_Info_PDP_Type_e)Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 2:
     {
       if(Parameter_List[i].String_Value)
       {
         qcpdpp_set_req.apn_name_valid = TRUE;
         memscpy(qcpdpp_set_req.apn_name, sizeof(qcpdpp_set_req.apn_name), Parameter_List[i].String_Value,
                  strlen((const char *)Parameter_List[i].String_Value));
       }
       i++;
     }
     break;
     case 4:
     {
       qcpdpp_set_req.auth_type_valid = TRUE;
       qcpdpp_set_req.auth_type = (qapi_Device_Info_Auth_Type_e)Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 8:
     {
       if(Parameter_List[i].String_Value)
       {
         qcpdpp_set_req.username_valid = TRUE;
         memscpy(qcpdpp_set_req.username, sizeof(qcpdpp_set_req.username), Parameter_List[i].String_Value,
                  strlen((const char *)Parameter_List[i].String_Value));
       }
       i++;
     }
     break;
     case 16:
     {
       if(Parameter_List[i].String_Value)
       {
         qcpdpp_set_req.password_valid = TRUE;
         memscpy(qcpdpp_set_req.password, sizeof(qcpdpp_set_req.password), Parameter_List[i].String_Value,
                  strlen((const char *)Parameter_List[i].String_Value));
       }
       i++;
     }
     break;
     default:
     break;
   }
   bitmask <<= 1;
   flag >>= 1;
 }while(flag);

 info_request.req.set_qcpdpp_req  = &qcpdpp_set_req;
 info_reponse.resp.set_qcpdpp_rsp = &qcpdpp_set_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SET_QCPDPP_E, &info_request, &info_reponse);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request qcpdpp: qapi_Device_Info_Request error %d\n", ret);
    printf_pdp_error_code((qcpdpp_set_rsp.response));
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    if(qcpdpp_set_rsp.response != QAPI_DEVICE_INFO_PDP_NO_ERROR)
    {
      printf_pdp_error_code((qcpdpp_set_rsp.response));
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    result = QCLI_STATUS_SUCCESS_E;
    goto end;
 }


usage_end:
    DEVINFO_PRINTF("request qcpdpp: Usage: [-app <app_id>] request set_qcpdpp <bitmask> <pdp type (0-4)> <apn name>"
                    "<auth type (0-3)> <username> <password>\n");
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_set_cgdcont(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_reponse;
 qapi_Device_Info_Set_CGDCONT_Req_t    cgdcont_set_req;
 qapi_Device_Info_Set_CGDCONT_Rsp_t    cgdcont_set_rsp;
 uint32_t flag = 0, bitmask = 1;
 int i = 2;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_reponse,  0, sizeof(qapi_Device_Info_Response_t));
 memset(&cgdcont_set_req,  0, sizeof(qapi_Device_Info_Set_CGDCONT_Req_t));
 memset(&cgdcont_set_rsp,  0, sizeof(qapi_Device_Info_Set_CGDCONT_Rsp_t));

 if((Parameter_Count < 4) || (Parameter_Count > 10) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 if(Parameter_List[1].Integer_Value < 3)
 {
   goto usage_end;
 }

 flag = Parameter_List[1].Integer_Value;

 do
 {
   switch(Parameter_List[1].Integer_Value & bitmask)
   {
     case 1:
     {
       cgdcont_set_req.addr_info.pdp_type = (qapi_Device_Info_PDP_Type_e)Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 2:
     {
       if(Parameter_List[i].String_Value)
       {
         cgdcont_set_req.addr_info.apn_name_valid = TRUE;
         memscpy(cgdcont_set_req.addr_info.apn_name, sizeof(cgdcont_set_req.addr_info.apn_name), Parameter_List[i].String_Value,
                  strlen((const char *)Parameter_List[i].String_Value));
       }
       i++;
     }
     break;
     case 4:
     {
       // Ignore for now.
       i++;
     }
     break;
     case 8:
     {
       // Ignore for now.
       i++;
     }
     break;
     case 16:
     {
       cgdcont_set_req.d_comp_valid = TRUE;
       cgdcont_set_req.d_comp = (qapi_Device_Info_PDP_Data_Compr_Type_e)Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 32:
     {
       cgdcont_set_req.h_comp_valid = TRUE;
       cgdcont_set_req.h_comp = (qapi_Device_Info_PDP_Hdr_Compr_Type_e)Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 64:
     {
       cgdcont_set_req.ipv4AddrAlloc_valid = TRUE;
       cgdcont_set_req.ipv4AddrAlloc = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 128:
     {
       cgdcont_set_req.delete_profile_valid = TRUE;
       cgdcont_set_req.delete_profile = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     default:
     break;
   }
   bitmask <<= 1;
   flag >>= 1;
 }while(flag);

 info_request.req.set_cgdcont_req  = &cgdcont_set_req;
 info_reponse.resp.set_cgdcont_rsp = &cgdcont_set_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SET_CGDCONT_E, &info_request, &info_reponse);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request cgdcont: qapi_Device_Info_Request error %d\n", ret);
    printf_pdp_error_code((cgdcont_set_rsp.response));
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    if(cgdcont_set_rsp.response != QAPI_DEVICE_INFO_PDP_NO_ERROR)
    {
      printf_pdp_error_code((cgdcont_set_rsp.response));
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    result = QCLI_STATUS_SUCCESS_E;
    goto end;
 }


usage_end:
    DEVINFO_PRINTF("request cgdcont: Usage: [-app <app_id>] request set_cgdcont <bitmask> <pdp type (0-4)> <apn name>"
                    "<ipv4 address> <ipv6 address> <data compression type(0-2)> <header compression type(0-4)> "
                    "<ipv4addrAlloc (0-1)> <delete profile (0-1)>\n");
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_set_qcpdpimscfge(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_reponse;
 qapi_Device_Info_Set_QCPDPIMSCFGE_Req_t    qcpdpimscfge_set_req;
 qapi_Device_Info_Set_QCPDPIMSCFGE_Rsp_t    qcpdpimscfge_set_rsp;
 uint32_t flag = 0, bitmask = 1;
 int i = 2;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_reponse,  0, sizeof(qapi_Device_Info_Response_t));
 memset(&qcpdpimscfge_set_req,  0, sizeof(qapi_Device_Info_Set_QCPDPIMSCFGE_Req_t));
 memset(&qcpdpimscfge_set_rsp,  0, sizeof(qapi_Device_Info_Set_QCPDPIMSCFGE_Rsp_t));

 if((Parameter_Count < 4) || (Parameter_Count > 7) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 if(Parameter_List[1].Integer_Value < 3)
 {
   goto usage_end;
 }

 flag = Parameter_List[1].Integer_Value;

 do
 {
   switch(Parameter_List[1].Integer_Value & bitmask)
   {
     case 1:
     {
       qcpdpimscfge_set_req.pdp_type = (qapi_Device_Info_PDP_Type_e)Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 2:
     {
       if(Parameter_List[i].String_Value)
       {
         qcpdpimscfge_set_req.apn_name_valid = TRUE;
         memscpy(qcpdpimscfge_set_req.apn_name, sizeof(qcpdpimscfge_set_req.apn_name), Parameter_List[i].String_Value,
                  strlen((const char *)Parameter_List[i].String_Value));
       }
       i++;
     }
     break;
     case 4:
     {
       qcpdpimscfge_set_req.address_flag_valid = TRUE;
       qcpdpimscfge_set_req.address_flag = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 8:
     {
       qcpdpimscfge_set_req.dhcp_flag_valid = TRUE;
       qcpdpimscfge_set_req.dhcp_flag = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 16:
     {
       qcpdpimscfge_set_req.cn_flag_valid = TRUE;
       qcpdpimscfge_set_req.cn_flag = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     default:
     break;
   }
   bitmask <<= 1;
   flag >>= 1;
 }while(flag);

 info_request.req.set_qcpdpimscfg_req  = &qcpdpimscfge_set_req;
 info_reponse.resp.set_qcpdpimscfg_rsp = &qcpdpimscfge_set_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SET_QCPDPIMSCFG_E, &info_request, &info_reponse);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request qcpdpimscfge: qapi_Device_Info_Request error %d\n", ret);
    printf_pdp_error_code((qcpdpimscfge_set_rsp.response));
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    if(qcpdpimscfge_set_rsp.response != QAPI_DEVICE_INFO_PDP_NO_ERROR)
    {
      printf_pdp_error_code((qcpdpimscfge_set_rsp.response));
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    result = QCLI_STATUS_SUCCESS_E;
    goto end;
 }


usage_end:
    DEVINFO_PRINTF("request qcpdpimscfge: Usage: [-app <app_id>] request set_qcpdpimscfge <bitmask> <pdp type (0-4)> <apn name>"
                   "<address flag (0-1)> <dhcp flag (0-1)> <cn flag (0-1)>\n ");
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_set_qcpdpcfge(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_reponse;
 qapi_Device_Info_Set_QCPDPCFGE_Req_t    qcpdpcfge_set_req;
 qapi_Device_Info_Set_QCPDPCFGE_Rsp_t    qcpdpcfge_set_rsp;
 uint32_t flag = 0, bitmask = 1;
 int i = 2;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_reponse,  0, sizeof(qapi_Device_Info_Response_t));
 memset(&qcpdpcfge_set_req,  0, sizeof(qapi_Device_Info_Set_QCPDPCFGE_Req_t));
 memset(&qcpdpcfge_set_rsp,  0, sizeof(qapi_Device_Info_Set_QCPDPCFGE_Rsp_t));

 if((Parameter_Count < 4) || (Parameter_Count > 15) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 if(Parameter_List[1].Integer_Value < 3)
 {
   goto usage_end;
 }

 flag = Parameter_List[1].Integer_Value;

 do
 {
   switch(Parameter_List[1].Integer_Value & bitmask)
   {
     case 1:
     {
       qcpdpcfge_set_req.pdp_type = (qapi_Device_Info_PDP_Type_e)Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 2:
     {
       if(Parameter_List[i].String_Value)
       {
         qcpdpcfge_set_req.apn_name_valid = TRUE;
         memscpy(qcpdpcfge_set_req.apn_name, sizeof(qcpdpcfge_set_req.apn_name), Parameter_List[i].String_Value,
                  strlen((const char *)Parameter_List[i].String_Value));
       }
       i++;
     }
     break;
     case 4:
     {
       qcpdpcfge_set_req.apn_disable_flag_valid = TRUE;
       qcpdpcfge_set_req.apn_disable_flag = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 8:
     {
       qcpdpcfge_set_req.timer_value_valid = TRUE;
       qcpdpcfge_set_req.timer_value = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 16:
     {
       qcpdpcfge_set_req.apn_class_valid = TRUE;
       qcpdpcfge_set_req.apn_class = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 32:
     {
       qcpdpcfge_set_req.apn_bearer_valid = TRUE;
       qcpdpcfge_set_req.apn_bearer = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 64:
     {
       qcpdpcfge_set_req.max_pdn_conn_per_block_valid = TRUE;
       qcpdpcfge_set_req.max_pdn_conn_per_block = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 128:
     {
       qcpdpcfge_set_req.max_pdn_conn_timer_valid = TRUE;
       qcpdpcfge_set_req.max_pdn_conn_timer = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 256:
     {
       qcpdpcfge_set_req.pdn_req_wait_timer_valid = TRUE;
       qcpdpcfge_set_req.pdn_req_wait_timer = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 512:
     {
       qcpdpcfge_set_req.emergency_calls_supported_valid = TRUE;
       qcpdpcfge_set_req.emergency_calls_supported = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 1024:
     {
       qcpdpcfge_set_req.operator_reserved_pco_valid = TRUE;
       qcpdpcfge_set_req.operator_reserved_pco = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 2048:
     {
       qcpdpcfge_set_req.mcc_valid = TRUE;
       qcpdpcfge_set_req.mcc = Parameter_List[i].Integer_Value;
       i++;
     }
     break;
     case 4096:
     {
       qcpdpcfge_set_req.mnc_valid = TRUE;
       qcpdpcfge_set_req.mnc.mnc = Parameter_List[i].Integer_Value;
       qcpdpcfge_set_req.mnc.mnc_includes_pcs_digit = TRUE;
       i++;
     }
     break;
     default:
     break;
   }
   bitmask <<= 1;
   flag >>= 1;
 }while(flag);

 info_request.req.set_qcpdpcfg_req  = &qcpdpcfge_set_req;
 info_reponse.resp.set_qcpdpcfg_rsp = &qcpdpcfge_set_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SET_QCPDPCFG_E, &info_request, &info_reponse);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request qcpdpcfge: qapi_Device_Info_Request error %d\n", ret);
    printf_pdp_error_code((qcpdpcfge_set_rsp.response));
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    if(qcpdpcfge_set_rsp.response != QAPI_DEVICE_INFO_PDP_NO_ERROR)
    {
      printf_pdp_error_code((qcpdpcfge_set_rsp.response));
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

    result = QCLI_STATUS_SUCCESS_E;
    goto end;
 }


usage_end:
    DEVINFO_PRINTF("request qcpdpcfge:Usage: [-app <app_id>] request set_qcpdpcfge <bitmask> <pdp type (0-4)> <apn name>"
                   "<apn disable flag (0-1)> <timer value (0-122820)> <apn class (0-16)> <apn bearer (1-15, 255)>"
                   "<max_pdn_conn_per_block(0-1023> <max_pdn_conn_timer(0-3600) <pdn_req_wait_timer (0-1023>"
                   "emergency call (0-1)> <operater pco (65280-65535)> <mcc(0-999)<mnc(0-999)>\n ");
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

/***********************************************
    request_scan_config
 ***********************************************/
QCLI_Command_Status_t request_scan_config(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Scan_Config_Rsp_t  scan_cfg_rsp;

 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&scan_cfg_rsp, 0, sizeof(qapi_Device_Info_Scan_Config_Rsp_t));

 if((Parameter_Count != 1) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("request scan_config : Usage: [-app <app_id>] request scan_config , Device Info CLI Request command\n");
    result = QCLI_STATUS_USAGE_E;
    return result;
 }

 info_response.resp.scan_config_rsp = &scan_cfg_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_SCAN_CONFIG_E, NULL, &info_response);

 if(ret != QAPI_OK )
 {
    DEVINFO_PRINTF("request scan_config: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    return result;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    if(scan_cfg_rsp.scan_counter_valid)
		DEVINFO_PRINTF("Scan counter is %d\n",scan_cfg_rsp.scan_counter);
	else
		DEVINFO_PRINTF("Invalid Scan counter\n");

    if(scan_cfg_rsp.power_save_duration_min_valid)
		DEVINFO_PRINTF("Power save duration min is %d\n",scan_cfg_rsp.power_save_duration_min);
	else
		DEVINFO_PRINTF("Invalid Power save duration min\n");

    if(scan_cfg_rsp.power_save_duration_inc_type_valid)
		DEVINFO_PRINTF("Power save duration inc type %d\n",scan_cfg_rsp.power_save_duration_inc_type);
	else
		DEVINFO_PRINTF("Invalid Power save duration inc type \n");

    if(scan_cfg_rsp.power_save_duration_max_valid)
		DEVINFO_PRINTF("Power save duration max %d\n",scan_cfg_rsp.power_save_duration_max);
	else
		DEVINFO_PRINTF("Invalid Power save duration max\n");

    if(scan_cfg_rsp.hplmn_scan_interval_valid)
		DEVINFO_PRINTF("HPLMN Scan Interval%d\n",scan_cfg_rsp.hplmn_scan_interval);
	else
		DEVINFO_PRINTF("Invalid HPLMN Scan Interval\n");

    if(scan_cfg_rsp.ciot_pref_rat_scan_interval_valid)
		DEVINFO_PRINTF("CIOT Pref RAT Scan Interval is %d\n",scan_cfg_rsp.ciot_pref_rat_scan_interval);
	else
		DEVINFO_PRINTF("Invalid CIOT Pref RAT Scan Interval \n");
  }

  return result;


}

QCLI_Command_Status_t set_fdn_status(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Req_Set_FDN_t    set_fdn_req;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&set_fdn_req,  0, sizeof(qapi_Device_Info_Req_Set_FDN_t));

 if((Parameter_Count != 2) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("%s : Parameter_Count[%d], expect 3\n", __func__, Parameter_Count);
    goto usage_end;
 }

 if(Parameter_List[1].Integer_Value != 0 &&
 	Parameter_List[1].Integer_Value != 1)
 {
    DEVINFO_PRINTF("%s : expect [0,1] on the last parameter\n", __func__);
    goto usage_end;
 }

 set_fdn_req.isEnableFDN = Parameter_List[1].Integer_Value;

 info_request.req.set_fdn_req  = &set_fdn_req;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SET_FDN_STATUS_E, &info_request, NULL);
 if(ret != QAPI_OK)
 {

    DEVINFO_PRINTF("%s: qapi_Device_Info_Request error %d\n", __func__ , ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
    DEVINFO_PRINTF("%s: Usage: [-app <app_id>] set fdn <fdn-status>\n", __func__);
    result = QCLI_STATUS_USAGE_E;

end:

  return result;
}


QCLI_Command_Status_t enable_uim_sub(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int val[1]={0};
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;


  if(NULL == app_devinfo_cli_ctx[app_id])
  {
    DEVINFO_PRINTF("enable_uim_sub : Device info not initialized, Please do init first \n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if((Parameter_Count < 2) || (Parameter_List == NULL))
  {
    DEVINFO_PRINTF("enable_uim_sub : Usage: [-app <app_id>] set enable_uim_sub <0/1> , Device Info CLI Set command\n");
    goto end;
  }

  val[0] = Parameter_List[1].Integer_Value;
  ret = qapi_Device_Info_Set(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_SUBS_ENABLE_E, 1, val);
  if(ret != QAPI_OK)
  {
    DEVINFO_PRINTF("enable_uim_sub: returned error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
     DEVINFO_PRINTF("enable_uim_sub: success \n");
  }

end:
  return result;
}

QCLI_Command_Status_t request_device_fsk_start(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Response_t info_response;
  qapi_Device_Info_FSK_Start_Req_t fsk_start_req;
  uint32_t flag = 0, bitmask = 1;
  int i = 2;

  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
  memset(&fsk_start_req,  0, sizeof(qapi_Device_Info_FSK_Start_Req_t));

 if((Parameter_Count < 2) || (Parameter_Count > 5) || (Parameter_List == NULL))
  {
    goto usage_end;
  }
  flag = Parameter_List[1].Integer_Value;

  do
  {
    switch(Parameter_List[1].Integer_Value & bitmask)
    {
      case 1:
      {
        fsk_start_req.baudrate_valid = TRUE;
        fsk_start_req.baudrate       = Parameter_List[i].Integer_Value;
        i++;
      }
      break;
      case 2:
      {
        fsk_start_req.gaptime_valid = TRUE;
        fsk_start_req.gaptime       = Parameter_List[i].Integer_Value;
        i++;
      }
      break;
      case 4:
      {
        fsk_start_req.restart_mode_valid = TRUE;
        fsk_start_req.restart_mode       = Parameter_List[i].Integer_Value;
        i++;
      }
      default:
      break;
    }
    bitmask <<= 1;
    flag >>= 1;
  }while(flag);

  info_request.req.fsk_start_req  = &fsk_start_req;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_FSK_START_E, &info_request, &info_response);
  if(ret == QAPI_OK)
  {
    DEVINFO_PRINTF("FSK start successful. \n");
  }
  else if(ret == QAPI_ERR_INVALID_STATE)
  {
    DEVINFO_PRINTF("Device should be in low power mode. \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    DEVINFO_PRINTF("request fsk_start: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

usage_end:
  DEVINFO_PRINTF("request fsk_start: Usage: [-app <app_id>] request fsk_start <bitmask> <baud rate (0-2)> "
                 "<gap time (baud rate = 0 gap time multiple of 16 till 1024, baud rate = 1 gap time multiple of 8 till 1024 baud rate = 2 gap time multiple of 4 till 1024>"
                 "<restart mode (0/1)> \n");
  result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_fsk_stop(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Response_t info_response;

  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));

 if((Parameter_Count != 1) || (Parameter_List == NULL))
  {
    goto usage_end;
  }

  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_FSK_STOP_E, &info_request, &info_response);
  if(ret == QAPI_OK)
  {
    DEVINFO_PRINTF("FSK stop successful. \n");
  }
  else if(ret == QAPI_ERR_INVALID_STATE)
  {
    DEVINFO_PRINTF("Device should be in low power mode. \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    DEVINFO_PRINTF("request fsk_stop: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

usage_end:
  DEVINFO_PRINTF("request fsk_stop: Usage: [-app <app_id>] request fsk_stop \n");
  result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_fsk_data(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Response_t info_response;
  qapi_Device_Info_FSK_Data_Req_t fsk_data_req;
  int i = 0;

  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
  memset(&fsk_data_req,  0, sizeof(qapi_Device_Info_FSK_Data_Req_t));

  if((Parameter_Count < 5) || (Parameter_List == NULL))
  {
    goto usage_end;
  }

  for(i = 1; i < Parameter_Count; i++)
  {
    if(strcmp((char*)Parameter_List[i].String_Value, "-l") == 0)
    {
      ++i;
      fsk_data_req.data_len = Parameter_List[i].Integer_Value;
    }
    else if(strcmp((char*)Parameter_List[i].String_Value, "-v") == 0)
    {
      ++i;
      if(Parameter_List[i].String_Value)
      {
        memcpy(fsk_data_req.data, ( void *)Parameter_List[i].String_Value, QAPI_DEVICE_INFO_FSK_MAX_BYTES_TO_SEND);
      }
    }
  }
  info_request.req.fsk_data_req  = &fsk_data_req;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_FSK_DATA_E, &info_request, &info_response);
  if(ret == QAPI_OK)
  {
    DEVINFO_PRINTF("FSK data set successful. \n");
  }
  else
  {
    DEVINFO_PRINTF("request fsk_data: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

usage_end:
  DEVINFO_PRINTF("request fsk_data: Usage: [-app <app_id>] request fsk_data -l <length> -v <value>\n");
  result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_set_fsk_pcl(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Response_t info_response;
  qapi_Device_Info_Set_FSK_PCL_Req_t set_fsk_pcl_req;
  uint32_t flag = 0, bitmask = 1;
  int i = 2;

  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
  memset(&set_fsk_pcl_req,  0, sizeof(qapi_Device_Info_Set_FSK_PCL_Req_t));

 if((Parameter_Count < 2) || (Parameter_Count > 5) || (Parameter_List == NULL))
  {
    goto usage_end;
  }
  flag = Parameter_List[1].Integer_Value;

  do
  {
    switch(Parameter_List[1].Integer_Value & bitmask)
    {
      case 1:
      {
        set_fsk_pcl_req.pcl_gsm_valid = TRUE;
        set_fsk_pcl_req.pcl_gsm = Parameter_List[i].Integer_Value;
        i++;
      }
      break;
      case 2:
      {
        set_fsk_pcl_req.pcl_dcs_valid = TRUE;
        set_fsk_pcl_req.pcl_dcs       = Parameter_List[i].Integer_Value;
        i++;
      }
      break;
      case 4:
      {
        set_fsk_pcl_req.pcl_pcs_valid = TRUE;
        set_fsk_pcl_req.pcl_pcs       = Parameter_List[i].Integer_Value;
        i++;
      }
      default:
      break;
    }
    bitmask <<= 1;
    flag >>= 1;
  }while(flag);

  info_request.req.set_fsk_pcl_req   = &set_fsk_pcl_req;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SET_FSK_PCL_E, &info_request, &info_response);
  if(ret == QAPI_OK)
  {
    DEVINFO_PRINTF("FSK set PCL successful. \n");
  }
  else if(ret == QAPI_ERR_INVALID_STATE)
  {
    DEVINFO_PRINTF("Device should be in low power mode. \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    DEVINFO_PRINTF("request set_fsk_pcl: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

usage_end:
  DEVINFO_PRINTF("request set_fsk_pcl: Usage: [-app <app_id>] request set_fsk_pcl <bitmask> <pcl gsm (5-19,99)> "
                 "<pcl dcs (0-15,99)> <pcl pcs (0-15,99)> \n");

  result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_set_fsk_debug(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Response_t info_response;
  qapi_Device_Info_Set_FSK_Debug_Req_t set_fsk_debug_req;
  uint32_t flag = 0, bitmask = 1;
  int i = 2;

  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
  memset(&set_fsk_debug_req,  0, sizeof(qapi_Device_Info_Set_FSK_Debug_Req_t));

 if((Parameter_Count < 2) || (Parameter_Count > 5) || (Parameter_List == NULL))
  {
    goto usage_end;
  }
  flag = Parameter_List[1].Integer_Value;

  do
  {
    switch(Parameter_List[1].Integer_Value & bitmask)
    {
      case 1:
      {
        set_fsk_debug_req.freq       = Parameter_List[i].Integer_Value;
        i++;
      }
      break;
      case 2:
      {
        set_fsk_debug_req.delta_valid = TRUE;
        set_fsk_debug_req.delta       = Parameter_List[i].Integer_Value;
        i++;
      }
      break;
      case 4:
      {
        set_fsk_debug_req.baudrate_valid = TRUE;
        set_fsk_debug_req.baudrate       = Parameter_List[i].Integer_Value;
        i++;
      }
      default:
      break;
    }
    bitmask <<= 1;
    flag >>= 1;
  }while(flag);

  info_request.req.set_fsk_debug_req  = &set_fsk_debug_req;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SET_FSK_DEBUG_E, &info_request, &info_response);
  if(ret == QAPI_OK)
  {
    DEVINFO_PRINTF("Set FSK debug successful. \n");
  }
  else if(ret == QAPI_ERR_INVALID_STATE)
  {
    DEVINFO_PRINTF("Device should be in low power mode. \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    DEVINFO_PRINTF("request set_fsk_debug: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

usage_end:
  DEVINFO_PRINTF("request set_fsk_debug: Usage: [-app <app_id>] request set_fsk_debug <bitmask> <freq (902000-928000, 966037-967957)> "
                 "<delta (1,2,3,5)> <baud rate(0-2)> \n");
  result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_set_fsk_hoptable(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Response_t info_response;
  qapi_Device_Info_Set_FSK_Hoptable_Req_t set_fsk_hoptable_req;
  int i = 0, j = 0;

  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
  memset(&set_fsk_hoptable_req,  0, sizeof(qapi_Device_Info_Set_FSK_Hoptable_Req_t));

 if((Parameter_Count < 4) || (Parameter_List == NULL))
  {
    goto usage_end;
  }

  set_fsk_hoptable_req.index = Parameter_List[1].Integer_Value;
  set_fsk_hoptable_req.fsk_hop_table.delta = Parameter_List[2].Integer_Value;
  set_fsk_hoptable_req.fsk_hop_table.freq_len = Parameter_List[3].Integer_Value;
  for(i = 0, j= 4; (i < Parameter_List[3].Integer_Value && i < QAPI_DEVICE_INFO_FSK_MAX_FREQS && j < Parameter_Count) ; i++, j++)
  {
    set_fsk_hoptable_req.fsk_hop_table.freq[i] = Parameter_List[j].Integer_Value;
  }

  info_request.req.set_fsk_hoptable_req  = &set_fsk_hoptable_req;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SET_FSK_HOP_TABLE_E, &info_request, &info_response);
  if(ret == QAPI_OK)
  {
    DEVINFO_PRINTF("FSK set hop table successful. \n");
  }
  else
  {
    DEVINFO_PRINTF("request set_fsk_hoptable: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

usage_end:
  DEVINFO_PRINTF("request set_fsk_hoptable: Usage: [-app <app_id>] request set_fsk_hoptable <index (0-2)> "
                 "<delta (1,2,4,5)> <freq_len> <freq....> \n");

  result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_device_get_fsk_pcl(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Response_t info_response;
  qapi_Device_Info_Get_FSK_PCL_Rsp_t get_fsk_pcl_rsp;

  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
  memset(&get_fsk_pcl_rsp,  0, sizeof(qapi_Device_Info_Get_FSK_PCL_Rsp_t));
  if((Parameter_Count != 1) || (Parameter_List == NULL))
  {
     goto usage_end;
  }
  info_response.resp.get_fsk_pcl_rsp   = &get_fsk_pcl_rsp;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_FSK_PCL_E, &info_request, &info_response);
  if(ret == QAPI_OK)
  {
    DEVINFO_PRINTF("QAPI success: \n");
    if(get_fsk_pcl_rsp.pcl_gsm_valid)
      DEVINFO_PRINTF("PCL GSM %d ", get_fsk_pcl_rsp.pcl_gsm);
    if(get_fsk_pcl_rsp.pcl_dcs_valid)
      DEVINFO_PRINTF("PCL DCS %d ", get_fsk_pcl_rsp.pcl_dcs);
    if(get_fsk_pcl_rsp.pcl_pcs_valid)
      DEVINFO_PRINTF("PCL PCS %d ", get_fsk_pcl_rsp.pcl_pcs);
  }
  else if(ret == QAPI_ERR_INVALID_STATE)
  {
    DEVINFO_PRINTF("Device should be in low power mode. \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    DEVINFO_PRINTF("request get_fsk_pcl: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

usage_end:
  DEVINFO_PRINTF("request get_fsk_pcl: Usage: [-app <app_id>] get_fsk_pcl \n");
  result = QCLI_STATUS_USAGE_E;

end:
  return result;
}

QCLI_Command_Status_t request_device_get_fsk_debug(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Response_t info_response;
  qapi_Device_Info_Get_FSK_Debug_Rsp_t get_fsk_debug_rsp;

  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
  memset(&get_fsk_debug_rsp,  0, sizeof(qapi_Device_Info_Get_FSK_Debug_Rsp_t));
  if((Parameter_Count != 1) || (Parameter_List == NULL))
  {
     goto usage_end;
  }
  info_response.resp.get_fsk_debug_rsp   = &get_fsk_debug_rsp;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_FSK_DEBUG_E, &info_request, &info_response);
  if(ret == QAPI_OK)
  {
    DEVINFO_PRINTF("QAPI success: \n");
    if(get_fsk_debug_rsp.delta_valid)
      DEVINFO_PRINTF("Delta %d ", get_fsk_debug_rsp.delta);
    if(get_fsk_debug_rsp.freq_valid)
      DEVINFO_PRINTF("Freq %d ", get_fsk_debug_rsp.freq);
    if(get_fsk_debug_rsp.baudrate_valid)
      DEVINFO_PRINTF("Baudrate %d ", get_fsk_debug_rsp.baudrate);
  }
  else if(ret == QAPI_ERR_INVALID_STATE)
  {
    DEVINFO_PRINTF("Device should be in low power mode. \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    DEVINFO_PRINTF("request get_fsk_debug: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

usage_end:
  DEVINFO_PRINTF("request get_fsk_debug: Usage: [-app <app_id>] get_fsk_debug \n");
  result = QCLI_STATUS_USAGE_E;

end:
  return result;
}

QCLI_Command_Status_t request_device_get_fsk_hoptable(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Response_t info_response;
  qapi_Device_Info_Get_FSK_Hoptable_Rsp_t get_fsk_hoptable_rsp;
  int i = 0, j = 0;
  memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
  memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
  memset(&get_fsk_hoptable_rsp,  0, sizeof(qapi_Device_Info_Get_FSK_Hoptable_Rsp_t));
  if((Parameter_Count != 1) || (Parameter_List == NULL))
  {
     goto usage_end;
  }
  info_response.resp.get_fsk_hoptable_rsp   = &get_fsk_hoptable_rsp;
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_FSK_HOP_TABLE_E, &info_request, &info_response);
  if(ret == QAPI_OK)
  {
    DEVINFO_PRINTF("QAPI success: \n");
    if(get_fsk_hoptable_rsp.fsk_hop_table_valid)
    {
      for (i = 0; i< get_fsk_hoptable_rsp.fsk_hop_table_len; i++)
      {
        DEVINFO_PRINTF("Delta %d ", get_fsk_hoptable_rsp.fsk_hop_table[i].delta);
        for (j = 0; j< get_fsk_hoptable_rsp.fsk_hop_table[i].freq_len ; j++)
        {
          DEVINFO_PRINTF("Freq[%d]  %d ",(j+1), get_fsk_hoptable_rsp.fsk_hop_table[i].freq);
        }
      }
    }
  }
  else
  {
    DEVINFO_PRINTF("request get_fsk_hoptable: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

usage_end:
  DEVINFO_PRINTF("request get_fsk_hoptable: Usage: [-app <app_id>] get_fsk_hoptable \n");
  result = QCLI_STATUS_USAGE_E;

end:
  return result;
}

QCLI_Command_Status_t request_device_get_qRxlevmin(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Get_QRXLEVMIN_Req_t   qrxlevmin_req;
 qapi_Device_Info_Get_QRXLEVMIN_Rsp_t  qrxlevmin_rsp;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&qrxlevmin_req,  0, sizeof(qapi_Device_Info_Get_QRXLEVMIN_Req_t));
 memset(&qrxlevmin_rsp, 0, sizeof(qapi_Device_Info_Get_QRXLEVMIN_Rsp_t));

 if((Parameter_Count != 1) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 info_request.req.get_qRxlevmin_req  = &qrxlevmin_req;
 info_response.resp.get_qRxlevmin_rsp = &qrxlevmin_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_QRXLEVMIN_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request qRxlevmin: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    if(qrxlevmin_rsp.q_rxlevmin_valid)
    {
      DEVINFO_PRINTF("%s: q_rxlevmin = %d\n", __func__,qrxlevmin_rsp.q_rxlevmin );
    }
    if(qrxlevmin_rsp.qrxlevmin_ce_valid)
    {
      DEVINFO_PRINTF("%s: qrxlevmin_ce = %d\n", __func__,qrxlevmin_rsp.qrxlevmin_ce );
    }
    if(qrxlevmin_rsp.qrxlevmin_ce1_valid)
    {
      DEVINFO_PRINTF("%s: qrxlevmin_ce1 = %d\n", __func__,qrxlevmin_rsp.qrxlevmin_ce1 );
    }
    if(qrxlevmin_rsp.rxlev_access_min_valid)
    {
      DEVINFO_PRINTF("%s: rxlev_access_min = %d\n", __func__,qrxlevmin_rsp.rxlev_access_min );
    }

  }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request qRxlevmin: Usage: [-app <app_id>] get_qRxlevmin \n");
  result = QCLI_STATUS_USAGE_E;

end:

  return result;
}


QCLI_Command_Status_t request_device_get_Srxlev(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 int iter_freq, iter_cell;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Get_Srxlev_Req_t  srxlev_req;
 qapi_Device_Info_Get_Srxlev_Rsp_t  srxlev_rsp;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&srxlev_req,  0, sizeof(qapi_Device_Info_Get_Srxlev_Req_t));
 memset(&srxlev_rsp, 0, sizeof(qapi_Device_Info_Get_Srxlev_Rsp_t));


 if((Parameter_Count != 2) || (Parameter_List == NULL))
 {
    DEVINFO_PRINTF("%s : Parameter_Count[%d], expect no more parameter\n", __func__, Parameter_Count);
    goto usage_end;
 }

 if(strcasecmp((const char*)Parameter_List[1].String_Value, "intra") == 0)
     srxlev_req.rat_type = QAPI_DEVICE_INFO_INTRA_FREQ;
 else if(strcasecmp((const char*)Parameter_List[1].String_Value, "inter") == 0)
     srxlev_req.rat_type = QAPI_DEVICE_INFO_INTER_FREQ;
 else if(strcasecmp((const char*)Parameter_List[1].String_Value, "lte_gsm") == 0)
 	 srxlev_req.rat_type = QAPI_DEVICE_INFO_LTE_GSM;
 else if(strcasecmp((const char*)Parameter_List[1].String_Value, "lte_wcdma") == 0)
 	 srxlev_req.rat_type = QAPI_DEVICE_INFO_LTE_WCDMA;
 else if(strcasecmp((const char*)Parameter_List[1].String_Value, "wcdma_lte") == 0)
 	 srxlev_req.rat_type = QAPI_DEVICE_INFO_WCDMA_LTE;
 else
 {
    DEVINFO_PRINTF("request srxlev: invalid rat type.\n");
    goto usage_end;
 }
 
 info_request.req.get_srxlev_req  = &srxlev_req;
 info_response.resp.get_srxlev_rsp = &srxlev_rsp;
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_SRXLEV_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("request srxlev: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
	if(srxlev_rsp.freq_count == 0)
	{
		DEVINFO_PRINTF("No cells found in the RAT type \n");
	}
	for(iter_freq = 0; iter_freq < srxlev_rsp.freq_count; iter_freq++)
	{
		DEVINFO_PRINTF("%s: ARFCN = %d\n", __func__,srxlev_rsp.freqs[iter_freq].arfcn );
		for(iter_cell = 0; iter_cell < srxlev_rsp.freqs[iter_freq].cell_count; iter_cell++)
		{
			DEVINFO_PRINTF("%s: Cell id = %d \t Srxlev = %d\n", __func__,srxlev_rsp.freqs[iter_freq].pci[iter_cell] , srxlev_rsp.freqs[iter_freq].srxlev[iter_cell]);
		}
	}
  }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request srxlev: Usage: [-app <app_id>] get_srxlev <intra/inter/lte_gsm/lte_wcdma/wcdma_lte>\n");
  result = QCLI_STATUS_USAGE_E;

end:

  return result;
}

QCLI_Command_Status_t request_set_sig_strength_threshold(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  qapi_Device_Info_Request_t  info_request;
  qapi_Device_Info_Config_Signal_strength_Req_t sig_config_req;
  int i,j, current_param = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&sig_config_req,  0, sizeof(qapi_Device_Info_Config_Signal_strength_Req_t));

 if((Parameter_Count < 5) || (Parameter_List == NULL))
 {
    goto usage_end;
 }
 
  sig_config_req.option_value_len = Parameter_List[1].Integer_Value;
 
  MALLOC_MEMORY(sig_config_req.opt_value, sig_config_req.option_value_len*sizeof(qapi_Device_Info_Signal_Strength_Option_Value_t));

  if(sig_config_req.opt_value == NULL)
  {
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  memset(sig_config_req.opt_value,  0, sizeof(sig_config_req.option_value_len*sizeof(qapi_Device_Info_Signal_Strength_Option_Value_t)));
  current_param = 2;
  for(i = 0; i < Parameter_List[1].Integer_Value && current_param < Parameter_Count ; i++)
  {
    
    sig_config_req.opt_value[i].sig_strength_opt = (qapi_Device_Info_Signal_Strength_Option_t)Parameter_List[current_param++].Integer_Value;

	if(current_param < Parameter_Count && Parameter_List[current_param].Integer_Value == 1)
    {
      current_param ++;
      sig_config_req.opt_value[i].value_type = QAPI_DEVICE_INFO_DELTA_VALUE;
      sig_config_req.opt_value[i].u.delta.value = Parameter_List[current_param++].Integer_Value;
   
    }
	else if(current_param < Parameter_Count && Parameter_List[current_param].Integer_Value == 2)
    {
      current_param ++;
      sig_config_req.opt_value[i].value_type = QAPI_DEVICE_INFO_THRESHOLD_LIST;
      sig_config_req.opt_value[i].u.threshold.list_len = Parameter_List[current_param++].Integer_Value;
	  for(j= 0; j < sig_config_req.opt_value[i].u.threshold.list_len && current_param < Parameter_Count ; j++)
	  {
         sig_config_req.opt_value[i].u.threshold.list[j] = Parameter_List[current_param++].Integer_Value;
	  }
    }
	
  }
  
  info_request.req.sig_config_req  = &sig_config_req;
  
  ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_CONFIG_SIGNAL_STRENGTH_E, &info_request, NULL);

  if(ret != QAPI_OK)
  {
    DEVINFO_PRINTF("request_set_sig_strength_threshold: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  else
  {
    DEVINFO_PRINTF("QAPI success: \n");

  }
  result = QCLI_STATUS_SUCCESS_E;
  goto end;

usage_end:
  DEVINFO_PRINTF("request set_sig_threshold Usage: [-app <app_id>] set_sig_threshold option_length(1-4) nw_option(1-GSM RSSI, 2- WCDMA RSSI, 3-LTE RSRP , 4- RSRQ)"
  	              "<for Delta: 1 delta_value><for threshold_list 2 list_len list> ....  \n");
  result = QCLI_STATUS_USAGE_E;

end:
  if(sig_config_req.opt_value)
    FREE_MEMORY(sig_config_req.opt_value);
  return result;
}

#define TYPE_OF_ADDRESS_ISDN 0x81
#define TYPE_OF_ADDRESS_INTERNATIONAL 0x91

QCLI_Command_Status_t request_device_send_sms(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Send_SMS_Req_t send_sms_req;
 int i = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&send_sms_req,  0, sizeof(qapi_Device_Info_Send_SMS_Req_t));

 if((Parameter_Count < 7) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 for(i = 1; i < Parameter_Count; i++)
 {
   if(strcmp((char*)Parameter_List[i].String_Value, "-smsc") == 0)
   {
     uint8_t *String_Value = Parameter_List[++i].String_Value;
     send_sms_req.smsc_number_is_valid = TRUE;

     if(strchr((const char *)String_Value, '+') != NULL)
     {
       send_sms_req.smsc_number.number_type = TYPE_OF_ADDRESS_INTERNATIONAL;
       String_Value = String_Value + 1;
     }
     else
     {
       send_sms_req.smsc_number.number_type = TYPE_OF_ADDRESS_ISDN;
     }
     send_sms_req.smsc_number.length = strlen((const char *)String_Value);
     MALLOC_MEMORY(send_sms_req.smsc_number.number, send_sms_req.smsc_number.length + 1);
     if(send_sms_req.smsc_number.number == NULL)
     {
       goto end;
     }
     memset(send_sms_req.smsc_number.number, 0x00, send_sms_req.smsc_number.length + 1);
     strlcpy(send_sms_req.smsc_number.number, (const char *)String_Value, send_sms_req.smsc_number.length + 1);
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-dest") == 0)
   {
     uint8_t *String_Value = Parameter_List[++i].String_Value;
     if(strchr((const char *)String_Value, '+') != '\0')
     {
       send_sms_req.dest_number.number_type = TYPE_OF_ADDRESS_INTERNATIONAL;
       String_Value = String_Value + 1;
     }
     else
     {
       send_sms_req.dest_number.number_type = TYPE_OF_ADDRESS_ISDN;
     }
     send_sms_req.dest_number.length = strlen((const char *)String_Value);
     MALLOC_MEMORY(send_sms_req.dest_number.number, send_sms_req.dest_number.length + 1);
     if(send_sms_req.dest_number.number == NULL)
     {
       goto end;
     }
     memset(send_sms_req.dest_number.number, 0x00, send_sms_req.dest_number.length + 1);
     strlcpy(send_sms_req.dest_number.number, (const char *)String_Value, send_sms_req.dest_number.length + 1);
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-format") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       send_sms_req.message_format = (qapi_Device_Info_WMS_Message_Format_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-fo") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       send_sms_req.first_octet_is_valid = TRUE;
       send_sms_req.first_octet = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-msg_ref") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       send_sms_req.message_ref_is_valid = TRUE;
       send_sms_req.message_ref = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-pid") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       send_sms_req.pid_is_valid = TRUE;
       send_sms_req.pid  = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-dcs") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       send_sms_req.dcs_is_valid = TRUE;
       send_sms_req.dcs  = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-validity") == 0)
   {
     int j = 0;
     i++;
     if(strcmp((char*)Parameter_List[i].String_Value, "-l") == 0)
     {
       i++;
       send_sms_req.validity.length =  Parameter_List[i++].Integer_Value;
     }
     if(strcmp((char*)Parameter_List[i].String_Value, "-v") == 0)
     {
     send_sms_req.validity_is_valid = TRUE;
       i++;
     for(j = 0; j < send_sms_req.validity.length && j < 7; j++)
     {
         send_sms_req.validity.validity[j] = Parameter_List[i++].Integer_Value;
       }
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-ims") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       send_sms_req.sms_on_ims_is_valid= TRUE;
       send_sms_req.sms_on_ims = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-payload") == 0)
   {
     i++;
     send_sms_req.sms_payload_length = strlen((const char *)Parameter_List[i].String_Value);
     MALLOC_MEMORY(send_sms_req.sms_payload, send_sms_req.sms_payload_length + 1);
     if(send_sms_req.sms_payload == NULL)
     {
       goto end;
     }
     memset(send_sms_req.sms_payload, 0x00, send_sms_req.sms_payload_length + 1);
     strlcpy(send_sms_req.sms_payload, (const char *)Parameter_List[i].String_Value, send_sms_req.sms_payload_length + 1);
   }
 }

 info_request.req.send_sms_req  = &send_sms_req;
 
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SEND_SMS_E, &info_request, NULL);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("send sms: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request send_sms: Usage: [-app <app_id>] send_sms <-smsc> <smsc number> (optional)\n");
  DEVINFO_PRINTF("<-dest> <destination number> <-payload> <payload> <-format> <format value> \n");
  DEVINFO_PRINTF("<-fo> <first octet>(optional) <-pid> <pid value>(optional) <-msg_ref> <message refernce value>(optional) \n");
  DEVINFO_PRINTF("<-dcs> <dcs value>(optional) <-ims> <sms on ims>(optional) <-validity><-l><length><-v> <validity value (1-7 bytes)>(optional) \n");
  result = QCLI_STATUS_USAGE_E;

end:
  if(send_sms_req.sms_payload)
    FREE_MEMORY(send_sms_req.sms_payload);
  if(send_sms_req.smsc_number.number)
    FREE_MEMORY(send_sms_req.smsc_number.number);
  if(send_sms_req.dest_number.number)
    FREE_MEMORY(send_sms_req.dest_number.number);
  return result;
}

QCLI_Command_Status_t request_device_write_sms(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Write_SMS_Req_t write_sms_req;
 qapi_Device_Info_Write_SMS_Rsp_t write_sms_rsp;
 
 int i = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&write_sms_req,  0, sizeof(qapi_Device_Info_Write_SMS_Req_t));
 memset(&write_sms_rsp,  0, sizeof(qapi_Device_Info_Write_SMS_Rsp_t));

 if((Parameter_Count < 9) || (Parameter_List == NULL))
 {
   goto usage_end;
 }

 for(i = 1; i < Parameter_Count; i++)
 {
   if(strcmp((char*)Parameter_List[i].String_Value, "-smsc") == 0)
   {
     uint8_t *String_Value = Parameter_List[++i].String_Value;
     write_sms_req.smsc_number_is_valid = TRUE;
     if(strchr((const char *)String_Value, '+') != NULL)
     {
       write_sms_req.smsc_number.number_type = TYPE_OF_ADDRESS_INTERNATIONAL;
       String_Value = String_Value+1;
     }
     else
     {
       write_sms_req.smsc_number.number_type = TYPE_OF_ADDRESS_ISDN;
     }
     write_sms_req.smsc_number.length = strlen((const char *)String_Value);
     MALLOC_MEMORY(write_sms_req.smsc_number.number, write_sms_req.smsc_number.length + 1);
     if(write_sms_req.smsc_number.number == NULL)
     {
       goto end;
     }
     memset(write_sms_req.smsc_number.number, 0x00, write_sms_req.smsc_number.length + 1);
     strlcpy(write_sms_req.smsc_number.number, (const char *)String_Value, write_sms_req.smsc_number.length + 1);
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-dest") == 0)
   {
     uint8_t *String_Value = Parameter_List[++i].String_Value;
     if(strchr((const char *)String_Value, '+') != NULL)
     {
       write_sms_req.dest_number.number_type = TYPE_OF_ADDRESS_INTERNATIONAL;
       String_Value = String_Value+1;
     }
     else
     {
       write_sms_req.dest_number.number_type = TYPE_OF_ADDRESS_ISDN;
     }
     write_sms_req.dest_number.length = strlen((const char *)String_Value);
     MALLOC_MEMORY(write_sms_req.dest_number.number, write_sms_req.dest_number.length + 1);
     if(write_sms_req.dest_number.number == NULL)
     {
       goto end;
     }
     memset(write_sms_req.dest_number.number, 0x00, write_sms_req.dest_number.length + 1);
     strlcpy(write_sms_req.dest_number.number, (const char *)String_Value, write_sms_req.dest_number.length + 1);
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-format") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       write_sms_req.message_format = (qapi_Device_Info_WMS_Message_Format_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-fo") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       write_sms_req.first_octet_is_valid = TRUE;
       write_sms_req.first_octet = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-msg_ref") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       write_sms_req.message_ref_is_valid = TRUE;
       write_sms_req.message_ref = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-pid") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       write_sms_req.pid_is_valid = TRUE;
       write_sms_req.pid  = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-dcs") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       write_sms_req.dcs_is_valid = TRUE;
       write_sms_req.dcs  = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-validity") == 0)
   {
     int j = 0;
     i++;
     if(strcmp((char*)Parameter_List[i].String_Value, "-l") == 0)
     {
       i++;
       write_sms_req.validity.length =  Parameter_List[i++].Integer_Value;
     }
     if(strcmp((char*)Parameter_List[i].String_Value, "-v") == 0)
     {
     write_sms_req.validity_is_valid = TRUE;
       i++;
     for(j = 0; j < write_sms_req.validity.length && j < 7; j++)
     {
         write_sms_req.validity.validity[j] = Parameter_List[i++].Integer_Value;
       }
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-tag") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       write_sms_req.tag_type_is_valid = TRUE;
       write_sms_req.tag_type = (qapi_Device_Info_WMS_Message_Tag_Type_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-storage") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       write_sms_req.storage_type = (qapi_Device_Info_WMS_Storage_Type_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-payload") == 0)
   {
     i++;
     write_sms_req.sms_payload_length = strlen((const char *)Parameter_List[i].String_Value);
     MALLOC_MEMORY(write_sms_req.sms_payload, write_sms_req.sms_payload_length + 1);
     if(write_sms_req.sms_payload == NULL)
     {
       goto end;
     }
     memset(write_sms_req.sms_payload, 0x00, write_sms_req.sms_payload_length+  1);
     strlcpy(write_sms_req.sms_payload, (const char *)Parameter_List[i].String_Value, write_sms_req.sms_payload_length + 1);
   }
 }

 info_request.req.write_sms_req  = &write_sms_req;
 info_response.resp.write_sms_rsp  = &write_sms_rsp;
 
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_WRITE_SMS_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("write sms: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    DEVINFO_PRINTF("Storage index %u \n",info_response.resp.write_sms_rsp->storage_index);
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request write_sms: Usage: [-app <app_id>] write_sms <-smsc> <smsc number> (optional)\n");
  DEVINFO_PRINTF("<-dest> <destination number> <-payload> <payload> <-format> <format value> <-storage> <storage value>\n");
  DEVINFO_PRINTF("<-fo> <first octet>(optional) <-pid> <pid value>(optional) <-msg_ref> <message refernce value>(optional) \n");
  DEVINFO_PRINTF("<-dcs> <dcs value>(optional) <-tag> <tag type>(optional) <-validity><-l> <length><-v> <validity value (1-7 bytes)>(optional) \n");
  result = QCLI_STATUS_USAGE_E;

end:
  if(write_sms_req.sms_payload)
    FREE_MEMORY(write_sms_req.sms_payload);
  if(write_sms_req.smsc_number.number)
    FREE_MEMORY(write_sms_req.smsc_number.number);
  if(write_sms_req.dest_number.number)
    FREE_MEMORY(write_sms_req.dest_number.number);
  return result;
}

QCLI_Command_Status_t request_device_read_sms(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Read_SMS_Req_t read_sms_req;
 qapi_Device_Info_Read_SMS_Rsp_t read_sms_rsp;
 
 int i = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&read_sms_req,  0, sizeof(qapi_Device_Info_Read_SMS_Req_t));
 memset(&read_sms_rsp,  0, sizeof(qapi_Device_Info_Read_SMS_Rsp_t));

 if((Parameter_Count < 5) || (Parameter_List == NULL))
 {
   goto usage_end;
 }

 for(i = 1; i < Parameter_Count; i++)
 {
   if(strcmp((char*)Parameter_List[i].String_Value, "-storage") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       read_sms_req.storage_type = (qapi_Device_Info_WMS_Storage_Type_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-index") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       read_sms_req.storage_index = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-mode") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       read_sms_req.message_mode_is_valid= TRUE;
       read_sms_req.message_mode = (qapi_Device_Info_WMS_Message_Mode_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-ims") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       read_sms_req.sms_on_ims_is_valid = TRUE;
       read_sms_req.sms_on_ims = Parameter_List[i].Integer_Value;
     }
   }
 }

 info_request.req.read_sms_req  = &read_sms_req;
 info_response.resp.read_sms_rsp  = &read_sms_rsp;
 
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_READ_SMS_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("read sms: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    if(info_response.resp.read_sms_rsp->smsc_number.length)
    {
      DEVINFO_PRINTF("SMSC Number address type %u \n",info_response.resp.read_sms_rsp->smsc_number.number_type);
      DEVINFO_PRINTF("SMSC Number address %s \n",info_response.resp.read_sms_rsp->smsc_number.number);
    }
    DEVINFO_PRINTF("First octet %u \n",info_response.resp.read_sms_rsp->first_octet);
    if(info_response.resp.read_sms_rsp->sender_number.length)
    {
      DEVINFO_PRINTF("Sender Number address type %u \n",info_response.resp.read_sms_rsp->sender_number.number_type);
      DEVINFO_PRINTF("Sender Number address %s \n",info_response.resp.read_sms_rsp->sender_number.number);
    }
    DEVINFO_PRINTF("Protocol identifier %u \n",info_response.resp.read_sms_rsp->pid);
    DEVINFO_PRINTF("Data coding scheme %u \n",info_response.resp.read_sms_rsp->dcs);
    if(info_response.resp.read_sms_rsp->sms_payload_length)
    {
      DEVINFO_PRINTF("SMS content length %d \n",info_response.resp.read_sms_rsp->sms_payload_length);
      DEVINFO_PRINTF("SMS content %s \n",info_response.resp.read_sms_rsp->sms_payload);
    }
    DEVINFO_PRINTF("Message format %u \n",info_response.resp.read_sms_rsp->format);
    DEVINFO_PRINTF("Tag type %u \n",info_response.resp.read_sms_rsp->tag_type);
    if(read_sms_rsp.message_ref_is_valid)
    {
      DEVINFO_PRINTF("Message refernce is: %d\n", read_sms_rsp.message_ref);
    }
    if(read_sms_rsp.validity_is_valid)
    {
      for(i = 0; i < read_sms_rsp.validity.length ; i++)
      {
        DEVINFO_PRINTF("Validity period: %d\n", read_sms_rsp.validity.validity[i]);
      }
    }
    if(read_sms_rsp.sms_payload)
      FREE_MEMORY(read_sms_rsp.sms_payload);
    if(read_sms_rsp.smsc_number.number)
      FREE_MEMORY(read_sms_rsp.smsc_number.number);
    if(read_sms_rsp.sender_number.number)
      FREE_MEMORY(read_sms_rsp.sender_number.number);
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request read_sms: Usage: [-app <app_id>] read_sms <-storage> <storage type> \n");
  DEVINFO_PRINTF("<-index> <message index> <-mode> <mode value>(optional) <-ims> <sms on ims>(optional) \n");
  result = QCLI_STATUS_USAGE_E;

end:
  return result;
}

QCLI_Command_Status_t request_device_delete_sms(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Delete_SMS_Req_t delete_sms_req;
 
 int i = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&delete_sms_req,  0, sizeof(qapi_Device_Info_Delete_SMS_Req_t));

 if((Parameter_Count < 3) || (Parameter_List == NULL))
 {
   goto usage_end;
 }

 for(i = 1; i < Parameter_Count; i++)
 {
   if(strcmp((char*)Parameter_List[i].String_Value, "-storage") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       delete_sms_req.storage_type = (qapi_Device_Info_WMS_Storage_Type_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-index") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       delete_sms_req.storage_index_is_valid = TRUE;
       delete_sms_req.storage_index = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-mode") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       delete_sms_req.message_mode_is_valid= TRUE;
       delete_sms_req.message_mode = (qapi_Device_Info_WMS_Message_Mode_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-tag") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       delete_sms_req.tag_type_is_valid = TRUE;
       delete_sms_req.tag_type = (qapi_Device_Info_WMS_Message_Tag_Type_e)Parameter_List[i].Integer_Value;
     }
   }
 }

 info_request.req.delete_sms_req  = &delete_sms_req;
 
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_DELETE_SMS_E, &info_request, NULL);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("delete sms: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request delete_sms: Usage: [-app <app_id>] delete_sms <-storage> <storage type> \n");
  DEVINFO_PRINTF("<-index> <message index>(optional) <-mode> <mode value>(optional) <-tag> <tag value>(optional) \n");
  result = QCLI_STATUS_USAGE_E;

end:
  return result;
}

QCLI_Command_Status_t request_device_list_sms(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_List_SMS_Req_t list_sms_req;
 qapi_Device_Info_List_SMS_Rsp_t list_sms_rsp;
 
 int i = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&list_sms_req,  0, sizeof(qapi_Device_Info_List_SMS_Req_t));
 memset(&list_sms_rsp,  0, sizeof(qapi_Device_Info_List_SMS_Rsp_t));

 if((Parameter_Count < 3) || (Parameter_List == NULL))
 {
   goto usage_end;
 }

 for(i = 1; i < Parameter_Count; i++)
 {
   if(strcmp((char*)Parameter_List[i].String_Value, "-storage") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       list_sms_req.storage_type = (qapi_Device_Info_WMS_Storage_Type_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-mode") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       list_sms_req.message_mode_is_valid= TRUE;
       list_sms_req.message_mode = (qapi_Device_Info_WMS_Message_Mode_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-tag") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       list_sms_req.tag_type_is_valid = TRUE;
       list_sms_req.tag_type = (qapi_Device_Info_WMS_Message_Tag_Type_e)Parameter_List[i].Integer_Value;
     }
   }
 }

 info_request.req.list_sms_req  = &list_sms_req;
 info_response.resp.list_sms_rsp  = &list_sms_rsp;
 
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_LIST_SMS_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("list sms: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    for(i = 0; i < list_sms_rsp.message_tuple_len && list_sms_rsp.message_tuple; i++)
    {
      DEVINFO_PRINTF("Number %d\n",i+1);
      DEVINFO_PRINTF("Message index %u \n",list_sms_rsp.message_tuple[i].message_index);
      DEVINFO_PRINTF("Tag type %u \n",list_sms_rsp.message_tuple[i].tag_type);
    }
    if(list_sms_rsp.message_tuple)
      FREE_MEMORY(list_sms_rsp.message_tuple);
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request list_sms: Usage: [-app <app_id>] list_sms <-storage> <storage type> \n");
  DEVINFO_PRINTF("<-mode> <mode value>(optional) <-tag> <tag type>(optional) \n");
  result = QCLI_STATUS_USAGE_E;

end:
  return result;
}

QCLI_Command_Status_t request_device_send_sms_ack(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Send_SMS_Ack_Req_t send_sms_ack_req;
 qapi_Device_Info_Send_SMS_Ack_Rsp_t send_sms_ack_rsp;
 
 int i = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&send_sms_ack_req,  0, sizeof(qapi_Device_Info_Send_SMS_Ack_Req_t));
 memset(&send_sms_ack_rsp,  0, sizeof(qapi_Device_Info_Send_SMS_Ack_Rsp_t));

 if((Parameter_Count < 7) || (Parameter_List == NULL))
 {
   goto usage_end;
 }

 for(i = 1; i < Parameter_Count; i++)
 {
   
   if(strcmp((char*)Parameter_List[i].String_Value, "-id") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       send_sms_ack_req.transaction_id = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-prot") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       send_sms_ack_req.message_protocol = (qapi_Device_Info_WMS_Message_Protocol_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-success") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       send_sms_ack_req.success = Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-rp_cause") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       send_sms_ack_req.rp_cause_is_valid = TRUE;
       send_sms_ack_req.rp_cause = (qapi_Device_Info_WMS_RP_Cause_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-tp_cause") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       send_sms_ack_req.tp_cause_is_valid = TRUE;
       send_sms_ack_req.tp_cause = (qapi_Device_Info_WMS_TP_Cause_e)Parameter_List[i].Integer_Value;
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-ims") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       send_sms_ack_req.sms_on_ims_is_valid = TRUE;
       send_sms_ack_req.sms_on_ims = Parameter_List[i].Integer_Value;
     }
   }
 }

 info_request.req.send_sms_ack_req  = &send_sms_ack_req;
 info_response.resp.send_sms_ack_rsp  = &send_sms_ack_rsp;
 
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SEND_SMS_ACK_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("send sms ack: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    if(send_sms_ack_rsp.failure_cause_valid)
    {
      DEVINFO_PRINTF("Failure cause %u \n",send_sms_ack_rsp.failure_cause);
    }
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request send_ack_sms: Usage: [-app <app_id>] send_ack_sms <-id> <transcation id> \n");
  DEVINFO_PRINTF("<-prot> <protocol value> <-success> <sucess/failure> \n");
  DEVINFO_PRINTF("<-tp_cause> <tp cause value> (optional)<-rp_cause> <rp cause value> (optional) <-ims> <sms on ims> (optional)\n");
  result = QCLI_STATUS_USAGE_E;

end:
  return result;
}

QCLI_Command_Status_t request_device_get_sms_routes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Get_SMS_Routes_Rsp_t get_sms_routes_rsp;
 
 int i = 0;

 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&get_sms_routes_rsp,  0, sizeof(qapi_Device_Info_List_SMS_Rsp_t));

 if((Parameter_Count < 1) || (Parameter_List == NULL))
 {
   goto usage_end;
 }

 info_response.resp.get_sms_route_rsp  = &get_sms_routes_rsp;
 
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_SMS_ROUTES_E, NULL, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("get sms route: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    for(i = 0; i < get_sms_routes_rsp.route_list_cnt && get_sms_routes_rsp.route_list; i++)
    {
      DEVINFO_PRINTF("Number %d\n",i+1);
      DEVINFO_PRINTF("Message type %u \n",get_sms_routes_rsp.route_list[i].msg_type);
      DEVINFO_PRINTF("Message class %u \n",get_sms_routes_rsp.route_list[i].msg_class);
      DEVINFO_PRINTF("Storage type %u \n",get_sms_routes_rsp.route_list[i].storage_type);
      DEVINFO_PRINTF("Receipt action %u \n",get_sms_routes_rsp.route_list[i].receipt_action);
    }
    if(get_sms_routes_rsp.transfer_ind_valid)
    {
      DEVINFO_PRINTF("Transfer ind %u \n",get_sms_routes_rsp.transfer_ind);
    }
    if(get_sms_routes_rsp.route_list)
      FREE_MEMORY(get_sms_routes_rsp.route_list);
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  result = QCLI_STATUS_USAGE_E;

end:
  return result;
}

QCLI_Command_Status_t request_device_set_sms_routes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Set_SMS_Routes_Req_t set_sms_routes_req;
 int i = 0, j = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&set_sms_routes_req,  0, sizeof(qapi_Device_Info_Set_SMS_Routes_Req_t));

 if((Parameter_Count < 8) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 for(i = 1; i < Parameter_Count; i++)
 {
   if(strcmp((char*)Parameter_List[i].String_Value, "-count") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       set_sms_routes_req.route_list_cnt = Parameter_List[i].Integer_Value;
     }
     MALLOC_MEMORY(set_sms_routes_req.route_list, set_sms_routes_req.route_list_cnt * sizeof(qapi_Device_Info_WMS_Route_t));
     if(set_sms_routes_req.route_list == NULL)
       goto end;
     memset(set_sms_routes_req.route_list, 0x00, set_sms_routes_req.route_list_cnt * sizeof(qapi_Device_Info_WMS_Route_t));
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-val") == 0)
   {
     if(set_sms_routes_req.route_list)
     {
       for(j = 0; j< set_sms_routes_req.route_list_cnt; j++)
       {
         if(Parameter_List[i].Integer_Is_Valid)
         {
           set_sms_routes_req.route_list[j].msg_type = (qapi_Device_Info_WMS_Message_Type_e)Parameter_List[++i].Integer_Value;
         }
         if(Parameter_List[i].Integer_Is_Valid)
         {
           set_sms_routes_req.route_list[j].msg_class = (qapi_Device_Info_WMS_Message_Class_e)Parameter_List[++i].Integer_Value;
         }
         if(Parameter_List[i].Integer_Is_Valid)
         {
           set_sms_routes_req.route_list[j].storage_type= (qapi_Device_Info_WMS_Storage_Type_e)Parameter_List[++i].Integer_Value;
         }
         if(Parameter_List[i].Integer_Is_Valid)
         {
           set_sms_routes_req.route_list[j].receipt_action= (qapi_Device_Info_WMS_Receipt_Action_e)Parameter_List[++i].Integer_Value;
         }
       }
     }
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-transfer") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       set_sms_routes_req.transfer_ind_is_valid = TRUE;
       set_sms_routes_req.transfer_ind= (qapi_Device_Info_WMS_Transfer_Ind_e)Parameter_List[i].Integer_Value;
     }
   }
 }

 info_request.req.set_sms_route_req  = &set_sms_routes_req;
 
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SET_SMS_ROUTES_E, &info_request, NULL);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("set sms routes: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request set_sms_routes: Usage: [-app <app_id>] set_sms_routes <-count> <no of routes> \n");
  DEVINFO_PRINTF("<-val> <msg_type> <msg_class> <storage type> <receipt action> <-transfer> <transfer ind value> (optional) \n");
  result = QCLI_STATUS_USAGE_E;

end:
  if(set_sms_routes_req.route_list)
    FREE_MEMORY(set_sms_routes_req.route_list);
  return result;
}

QCLI_Command_Status_t request_device_get_smsc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Response_t info_response;
 qapi_Device_Info_Get_SMSC_Req_t get_smsc_req;
 qapi_Device_Info_Get_SMSC_Rsp_t get_smsc_rsp;
 
 int i = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&info_response, 0, sizeof(qapi_Device_Info_Response_t));
 memset(&get_smsc_req,  0, sizeof(qapi_Device_Info_Get_SMSC_Req_t));
 memset(&get_smsc_rsp,  0, sizeof(qapi_Device_Info_Get_SMSC_Rsp_t));

 if((Parameter_Count < 1) || (Parameter_List == NULL))
 {
   goto usage_end;
 }

 for(i = 1; i < Parameter_Count; i++)
 {
   if(strcmp((char*)Parameter_List[i].String_Value, "-index") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       get_smsc_req.index_is_valid = TRUE;
       get_smsc_req.index = Parameter_List[i].Integer_Value;
     }
   }
 }

 info_request.req.get_smsc_req  = &get_smsc_req;
 info_response.resp.get_smsc_rsp  = &get_smsc_rsp;
 
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_GET_SMSC_E, &info_request, &info_response);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("get smsc: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
    if(get_smsc_rsp.smsc_address_digits_len)
    {
      DEVINFO_PRINTF("SMSC address digit %s\n",get_smsc_rsp.smsc_address_digits);
    }
    DEVINFO_PRINTF("SMSC address type %s\n",get_smsc_rsp.smsc_address_type);
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request get_smsc: Usage: [-app <app_id>] get_smsc <-index> <index value> (optional) \n");
  result = QCLI_STATUS_USAGE_E;

end:
  return result;
}

QCLI_Command_Status_t request_device_set_smsc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
 int ret = QAPI_ERROR;
 QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
 qapi_Device_Info_Request_t  info_request;
 qapi_Device_Info_Set_SMSC_Req_t set_smsc_req;
 int i = 0;

 memset(&info_request,  0, sizeof(qapi_Device_Info_Request_t));
 memset(&set_smsc_req,  0, sizeof(qapi_Device_Info_Set_SMSC_Req_t));

 if((Parameter_Count < 3) || (Parameter_List == NULL))
 {
    goto usage_end;
 }

 for(i = 1; i < Parameter_Count; i++)
 {
   if(strcmp((char*)Parameter_List[i].String_Value, "-smsc_address") == 0)
   {
     i++;
     memcpy(set_smsc_req.smsc_address_digits, Parameter_List[i].String_Value, strlen((const char *)Parameter_List[i].String_Value)+1);
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-smsc_type") == 0)
   {
     i++;
     set_smsc_req.smsc_address_type_is_valid = TRUE;
     memcpy(set_smsc_req.smsc_address_type, Parameter_List[i].String_Value, strlen((const char *)Parameter_List[i].String_Value)+1);
   }
   else if(strcmp((char*)Parameter_List[i].String_Value, "-index") == 0)
   {
     i++;
     if(Parameter_List[i].Integer_Is_Valid)
     {
       set_smsc_req.index_is_valid = TRUE;
       set_smsc_req.index = Parameter_List[i].Integer_Value;
     }
   }
 }

 info_request.req.set_smsc_req  = &set_smsc_req;
 
 ret = qapi_Device_Info_Request(app_devinfo_cli_ctx[app_id], QAPI_DEVICE_INFO_REQ_SET_SMSC_E, &info_request, NULL);
 if(ret != QAPI_OK)
 {
    DEVINFO_PRINTF("set smsc: qapi_Device_Info_Request error %d\n", ret);
    result = QCLI_STATUS_ERROR_E;
    goto end;
 }
 else
 {
    DEVINFO_PRINTF("QAPI success: \n");
 }
 result = QCLI_STATUS_SUCCESS_E;
 goto end;

usage_end:
  DEVINFO_PRINTF("request set_smsc: Usage: [-app <app_id>] set_smsc <-smsc_address> <smsc address> \n");
  DEVINFO_PRINTF("<-smsc_type> <smsc type> (optional) <-index> <index value>(optional) \n");
  result = QCLI_STATUS_USAGE_E;

end:
  return result;
}



/***********************************************
 device_info_command_handler
 ***********************************************/
QCLI_Command_Status_t device_info_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "device_info_command_handler : Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DEVICE_INFO_COMMAND_HANDLER_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
#ifdef QAPI_TXM_MODULE
  cmd_parser_temp->Command_Index = 21;
#else
  cmd_parser_temp->Command_Index = 24;
#endif
  result = multi_app_cmd_fill_func(cmd_parser_temp);

  return result;
}
/***********************************************
 proc_device_info_command_handle
 ***********************************************/
QCLI_Command_Status_t
proc_device_info_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int i;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

/* command that no common function to handle */
  dev_info_command_t cmd_list[] =
  {
    {"help",      NULL,   help_device_info                                  },
    {"init2",     NULL,   init_device_info2                                 },
    {"release2",  NULL,   release_device_info2                              },
    {"reset2",    NULL,   reset_device_info2                                },
    {"shutdown",  NULL,   shutdown_device_info                              },
    {"set",       "acq",           set_device_info_acq_order                },
    {"set",       "app_priority",  set_device_info_app_priority             },
    {"get",       "plmn",          get_device_info_plmn_list                },
    {"set",       "plmn",          set_device_info_plmn_select              },
    {"set",       "enable_sub",    enable_uim_sub                           },
    {"set", 	  "m1_ext",        set_device_info_m1_band_ext              },
    {"set",       "nb1_ext",       set_device_info_nb1_band_ext             },
    {"set",       "nz_bsr",        set_device_info_nz_bsr             },

    {"request",  "apdu",            request_device_apdu                     },
    {"request",  "readplmn",        request_device_read_plmn                },
    {"request",  "writeplmn",       request_device_write_plmn               },
    {"request",  "verifypin",       request_device_verify_pin               },
    {"request",  "changepin",       request_device_change_pin               },
    {"request",  "unblockpin",      request_device_unblock_pin              },
    {"request",  "protectpin",      request_device_protect_pin              },
    {"request",  "setfdn",          request_device_set_fdn_status           },
    {"request",  "getfdn",          request_device_get_fdn_status           },
    {"request",  "readapp",         request_device_read_app                 },
    {"request",  "getedrx",         request_device_get_edrx                 },
    {"request",  "setedrx",         request_device_set_edrx                 },
    {"request",  "jamming_status",  request_device_jamming_status           },
    {"request",  "setimsindreg",    request_device_set_ims_iot_ind_register },
    {"request",  "imsregstatus",    request_device_get_ims_register_status  },
    {"request",  "dialcall",        request_device_dial_call                },
    {"request",  "endcall",         request_device_end_call                 },
    {"request",  "answercall",      request_device_answer_call              },
    {"request",  "get_qcpdpp",    request_device_get_qcpdpp  },
    {"request",  "set_qcpdpp",    request_device_set_qcpdpp  },
    {"request",  "get_cgdcont",    request_device_get_cgdcont  },
    {"request",  "set_cgdcont",    request_device_set_cgdcont  },
    {"request",  "get_qcpdpimscfge",    request_device_get_qcpdpimscfge  },
    {"request",  "set_qcpdpimscfge",    request_device_set_qcpdpimscfge  },
    {"request",  "get_qcpdpcfge",    request_device_get_qcpdpcfge  },
    {"request",  "set_qcpdpcfge",    request_device_set_qcpdpcfge  },
    {"request",  "scan_config",     request_scan_config                },
    {"request",  "fsk_start",    request_device_fsk_start},
    {"request",  "fsk_stop",     request_device_fsk_stop},
    {"request",  "fsk_data",    request_device_fsk_data},
    {"request",  "set_fsk_debug",    request_device_set_fsk_debug},
    {"request",  "get_fsk_debug",    request_device_get_fsk_debug},
    {"request",  "set_fsk_hoptable",    request_device_set_fsk_hoptable},
    {"request",  "get_fsk_hoptable",    request_device_get_fsk_hoptable},
    {"request",  "set_fsk_pcl",    request_device_set_fsk_pcl},
    {"request",  "get_fsk_pcl",    request_device_get_fsk_pcl},
    {"request",  "get_qRxlevmin",  request_device_get_qRxlevmin           },
    {"request",  "set_sig_threshold",  request_set_sig_strength_threshold },
    {"request",  "get_srxlev",  request_device_get_Srxlev},
    {"request",  "send_sms",       request_device_send_sms},
    {"request",  "write_sms",       request_device_write_sms},
    {"request",  "read_sms",       request_device_read_sms},
    {"request",  "delete_sms",       request_device_delete_sms},
    {"request",  "list_sms",       request_device_list_sms},
    {"request",  "send_ack_sms",       request_device_send_sms_ack},
    {"request",  "set_smsc",       request_device_set_smsc},
    {"request",  "get_smsc",       request_device_get_smsc},
    {"request",  "set_sms_routes",       request_device_set_sms_routes},
    {"request",  "get_sms_routes",       request_device_get_sms_routes},
    };

  /* get command that call  */
  dev_info_get_command_t get_cmd_list[] =
  {
    {"get2", NULL,       QAPI_DEVICE_INFO_BUILD_ID_E,              "get_device_info2",                   print_device_info_str           },
    {"get",  "ciot",     QAPI_DEVICE_INFO_CIOT_LTE_OP_MODE_PREF_E, "get_device_info_ciot_mode",          print_device_info_int           },
    {"get",  "mode",     QAPI_DEVICE_INFO_MODE_PREF_E,             "get_device_info_mode",               print_device_info_intbuf        },
    {"get",  "m1",       QAPI_DEVICE_INFO_LTE_M1_BAND_PREF_E,      "get_device_info_m1band",             print_device_info_hexbuf        },
    {"get",  "nb1",      QAPI_DEVICE_INFO_LTE_NB1_BAND_PREF_E,     "get_device_info_nb1band",            print_device_info_hexbuf        },
    {"get",  "acq",      QAPI_DEVICE_INFO_RAT_ACQ_PREF_E,          "get_device_info_acq_order",          print_device_info_intbuf        },
    {"get",  "model",    QAPI_DEVICE_INFO_MODEL_ID_E,              "get_device_info_model_id",           print_device_info_str           },
    {"get",  "roam",     QAPI_DEVICE_INFO_ROAMING_E,               "get_device_info_roaming_info",       print_device_info_int           },
    {"get",  "imei",     QAPI_DEVICE_INFO_IMEI_E,                  "get_device_info_imei",               print_device_info_str           },
    {"get",  "imsi",     QAPI_DEVICE_INFO_IMSI_E,                  "get_device_info_imsi",               print_device_info_str           },
    {"get",  "os",       QAPI_DEVICE_INFO_OS_VERSION_E,            "get_device_info_os",                 print_device_info_str           },
    {"get",  "manu",     QAPI_DEVICE_INFO_MANUFACTURER_E,          "get_device_info_manufacturer",       print_device_info_str           },
    {"get",  "bat_stat", QAPI_DEVICE_INFO_BATTERY_STATUS_E,        "get_device_info_battery_stat",       print_device_info_battery       },
    {"get",  "percent",  QAPI_DEVICE_INFO_BATTERY_PERCENTAGE_E,    "get_device_info_battery_percentage", print_device_info_str           },
    {"get",  "iccid",    QAPI_DEVICE_INFO_ICCID_E,                 "get_device_info_iccid",              print_device_info_str           },
    {"get",  "time",     QAPI_DEVICE_INFO_TIME_ZONE_E,             "get_device_info_time_zone",          print_device_info_int           },
    {"get",  "4g",       QAPI_DEVICE_INFO_4G_SIG_STRENGTH_E,       "get_device_info_4g_sig_strength",    print_device_info_int           },
    {"get",  "base",     QAPI_DEVICE_INFO_BASE_STATION_ID_E,       "get_device_info_base_station_id",    print_device_info_int           },
    {"get",  "mnc",      QAPI_DEVICE_INFO_MNC_E,                   "get_device_info_mnc",                print_device_info_str           },
    {"get",  "mcc",      QAPI_DEVICE_INFO_MCC_E,                   "get_device_info_mcc",                print_device_info_str           },
    {"get",  "mdn",      QAPI_DEVICE_INFO_MDN_E,                   "get_device_info_mdn",                print_device_info_str           },
    {"get",  "srv",      QAPI_DEVICE_INFO_SERVICE_STATE_E,         "get_device_info_service_state",      print_device_info_int           },
    {"get",  "tac",      QAPI_DEVICE_INFO_TAC_E,                   "get_device_info_tac",                print_device_info_int           },
    {"get",  "cell",     QAPI_DEVICE_INFO_CELL_ID_E,               "get_device_info_cell_id",            print_device_info_int           },
    {"get",  "rccs",     QAPI_DEVICE_INFO_RCCS_E,                  "get_device_info_rccs",               print_device_info_rccs          },
    {"get",  "emms",     QAPI_DEVICE_INFO_EMMS_E,                  "get_device_info_emms",               print_device_info_int           },
    {"get",  "rsrq",     QAPI_DEVICE_INFO_SERVING_RSRQ_E,          "get_device_info_rsrq",               print_device_info_int           },
    {"get",  "pci",      QAPI_DEVICE_INFO_SERVING_PCI_E,           "get_device_info_pci",                print_device_info_int           },
    {"get",  "earfcn",   QAPI_DEVICE_INFO_SERVING_EARFCN_E,        "get_device_info_earfcn",             print_device_info_int           },
    {"get",  "sim",      QAPI_DEVICE_INFO_SIM_STATE_E,             "get_device_info_sim_state",          print_device_info_int           },
    {"get",  "chip",     QAPI_DEVICE_INFO_CHIPID_STRING_E,         "get_device_info_chip_id",            print_device_info_str           },
    {"get",  "power",    QAPI_DEVICE_INFO_LAST_POWER_ON_E,         "get_device_info_last_power_on",      print_device_info_str           },
    {"get",  "net",      QAPI_DEVICE_INFO_NETWORK_IND_E,           "get_device_info_network_indication", print_device_info_int           },
    {"get",  "bearer",   QAPI_DEVICE_INFO_NETWORK_BEARER_E,        "get_device_info_network_bearer",     print_device_info_network_bearer},
    {"get",  "build",    QAPI_DEVICE_INFO_BUILD_ID_E,              "get_device_info_build_id",           print_device_info_str           },
    {"get",  "psm",      QAPI_DEVICE_INFO_PSM_TIMER_E,             "get_device_info_psm_timer",          print_device_info_int           },
    {"get",  "active",   QAPI_DEVICE_INFO_ACTIVE_TIMER_E,          "get_device_info_active_timer",       print_device_info_int           },
    {"get",  "lte",      QAPI_DEVICE_INFO_LTE_OP_MODE_E,           "get_device_info_lte_op_mode",        print_device_info_int           },
    {"get",  "lac",      QAPI_DEVICE_INFO_LAC_E,                   "get_device_info_lac",                print_device_info_int           },
    {"get",  "app_priority", QAPI_DEVICE_INFO_GET_APP_PRIORITY_E,  "get_device_info_app_priority",       print_device_info_app_priority  },
    {"get",  "loaded_tech",  QAPI_DEVICE_INFO_GET_LOADED_TECH_E,   "get_device_info_loaded_tech",        print_device_info_loaded_tech   },
    {"get",  "opmode",   QAPI_DEVICE_INFO_OPERATING_MODE_E,        "get_device_info_op_mode",            print_device_info_int           },
    {"get",  "cemode",   QAPI_DEVICE_INFO_CEMODE_E,                "get_device_info_cemode",             print_device_info_int           },
    {"get",  "modem_state", QAPI_DEVICE_INFO_MODEM_SERVICE_STATE_E,"get_device_info_modem_state",        print_device_info_bool          },
    {"get",  "sleep_threshold", QAPI_DEVICE_INFO_WWAN_SLEEP_THRESHOLD_E,     "get_device_info_wwan_sleep_threshold",    print_device_info_int           },
    {"get",  "m1_ext",   QAPI_DEVICE_INFO_LTE_M1_BAND_PREF_EXT_E,  "get_device_info_m1_band_ext",        print_device_info_hexbuf        },
    {"get",  "nb1_ext",  QAPI_DEVICE_INFO_LTE_NB1_BAND_PREF_EXT_E, "get_device_info_nb1_band_ext",       print_device_info_hexbuf        },
    {"get",  "sinr", 	QAPI_DEVICE_INFO_SINR_E,     	   		   "get_device_info_sinr",    			 print_device_info_int           },
	{"get",  "ce_level",QAPI_DEVICE_INFO_CE_E,     		   		   "get_device_info_ce_level",           print_device_info_int           },
    {"get",  "nz_bsr",   QAPI_DEVICE_INFO_GET_NZ_BSR_E,            "get_device_info_nz_bsr",             print_device_info_nz_bsr  },
  };

  dev_info_get_command_t get_cmd_ext_cell_info_list[] =
  {
    {"get",  "gsm",     QAPI_DEVICE_INFO_RAT_TYPE_GSM_CELL_E,     "get_device_info_gsm",                print_device_info_geran_cell     },
    {"get",  "cm1",     QAPI_DEVICE_INFO_RAT_TYPE_LTE_M1_CELL_E,  "get_device_info_cm1",                print_device_info_lte_m1_n1_cell },
    {"get",  "cnb1",    QAPI_DEVICE_INFO_RAT_TYPE_LTE_NB1_CELL_E, "get_device_info_cnb1",               print_device_info_lte_m1_n1_cell },
  };
  dev_info_set_command_t set_cmd_list[] =
  {
    {"set",  "mode",      QAPI_DEVICE_INFO_MODE_PREF_E,             "set_device_info_mode"         },
    {"set",  "ciot",      QAPI_DEVICE_INFO_CIOT_LTE_OP_MODE_PREF_E, "set_device_info_ciot_mode"    },
    {"set",  "m1",        QAPI_DEVICE_INFO_LTE_M1_BAND_PREF_E,      "set_device_info_band_m1"      },
    {"set",  "nb1",       QAPI_DEVICE_INFO_LTE_NB1_BAND_PREF_E,     "set_device_info_band_nb1"     },
    {"set",  "ps_attach", QAPI_DEVICE_INFO_PS_ATTACH_E,             "set_device_info_ps_attach"    },
    {"set",  "ps_detach", QAPI_DEVICE_INFO_PS_DETACH_E,             "set_device_info_ps_detach"    },
    {"set",  "psm",       QAPI_DEVICE_INFO_PSM_TIMER_E,             "set_device_info_psm_timer"    },
    {"set",  "active",    QAPI_DEVICE_INFO_ACTIVE_TIMER_E,          "set_device_info_active_timer" },
    {"set",  "m1band",    QAPI_DEVICE_INFO_BAND_LTE_M1_CELL_E,      "set_device_info_m1band"       },
    {"set",  "nb1band",   QAPI_DEVICE_INFO_BAND_LTE_NB1_CELL_E,     "set_device_info_nb1band"      },
    {"set",  "gsmband",   QAPI_DEVICE_INFO_BAND_GSM_CELL_E,         "set_device_info_gsmband"      },
    {"set",  "opmode",    QAPI_DEVICE_INFO_OPERATING_MODE_E,        "set_device_info_op_mode"      },
    {"set",  "cemode",    QAPI_DEVICE_INFO_CEMODE_E,                "set_device_info_cemode"       },
    {"set",  "sleep_threshold", QAPI_DEVICE_INFO_WWAN_SLEEP_THRESHOLD_E,      "set_device_info_wwan_sleep_threshold"},
  };

  dev_info_set_command_t set_cmd_list_ind[] =
  {
    {"set_id",  "any",       QAPI_DEVICE_INFO_ANY,                                "set_device_info_callback"       },
    {"set_id",  "nw_ind",    QAPI_DEVICE_INFO_NETWORK_IND_E,                      "set_device_info_callback"       },
    {"set_id",  "nw_bearer", QAPI_DEVICE_INFO_NETWORK_BEARER_E,                   "set_device_info_callback"       },
    {"set_id",  "mcc",       QAPI_DEVICE_INFO_MCC_E,                              "set_device_info_callback"       },
    {"set_id",  "mnc",       QAPI_DEVICE_INFO_MNC_E,                              "set_device_info_callback"       },
    {"set_id",  "cell_id",   QAPI_DEVICE_INFO_CELL_ID_E,                          "set_device_info_callback"       },
    {"set_id",  "4g_sig",    QAPI_DEVICE_INFO_4G_SIG_STRENGTH_E,                  "set_device_info_callback"       },
    {"set_id",  "link_qual", QAPI_DEVICE_INFO_LINK_QUALITY_E,                     "set_device_info_callback"       },
    {"set_id",  "tx_bytes",  QAPI_DEVICE_INFO_TX_BYTES_E,                         "set_device_info_callback"       },
    {"set_id",  "rx_bytes",  QAPI_DEVICE_INFO_RX_BYTES_E,                         "set_device_info_callback"       },
    {"set_id",  "roaming",   QAPI_DEVICE_INFO_ROAMING_E,                          "set_device_info_callback"       },
    {"set_id",  "lte_op",    QAPI_DEVICE_INFO_LTE_OP_MODE_E,                      "set_device_info_callback"       },
    {"set_id",  "active",    QAPI_DEVICE_INFO_ACTIVE_TIMER_E,                     "set_device_info_callback"       },
    {"set_id",  "psm",       QAPI_DEVICE_INFO_PSM_TIMER_E,                        "set_device_info_callback"       },
    {"set_id",  "wms",       QAPI_DEVICE_INFO_WMS_EVENT_REPORT_E,                 "set_device_info_callback"       },
    {"set_id",  "sim_state", QAPI_DEVICE_INFO_SIM_STATE_E,                        "set_device_info_callback"       },
    {"set_id",  "edrx",      QAPI_DEVICE_INFO_EDRX_E,                             "set_device_info_callback"       },
    {"set_id",  "jammer",    QAPI_DEVICE_INFO_JAMMER_IND_E,                       "set_device_info_callback"       },
    {"set_id",  "ims_status",QAPI_DEVICE_INFO_IMS_IOT_REGISTRATION_STATUS_IND_E,  "set_device_info_callback"       },
    {"set_id",  "voice_call",QAPI_DEVICE_INFO_VOICE_ALL_CALL_STATUS_IND_E,        "set_device_info_callback"       },
    {"set_id",  "modem_state",QAPI_DEVICE_INFO_MODEM_SERVICE_STATE_E,             "set_device_info_callback"       },
    {"set_id",  "sleep_info",QAPI_DEVICE_INFO_WWAN_SLEEP_INFO_E,                  "set_device_info_callback"       },
    {"set_id",  "mt_sms",    QAPI_DEVICE_INFO_INCOMING_SMS_E,                     "set_device_info_callback"       },
    {"set_id",  "mt_route_sms",QAPI_DEVICE_INFO_ROUTE_INCOMING_SMS_E,             "set_device_info_callback"       },
    {"set_id",  "sms_status_report",QAPI_DEVICE_INFO_SMS_STATUS_REPORT_E,         "set_device_info_callback"       },
  };

  dev_info_set_command_t clear_cmd_list_ind[] =
  {
    {"clear_id",  "any",       QAPI_DEVICE_INFO_ANY,                                "clear_device_info_callback"       },
    {"clear_id",  "nw_ind",    QAPI_DEVICE_INFO_NETWORK_IND_E,                      "clear_device_info_callback"       },
    {"clear_id",  "nw_bearer", QAPI_DEVICE_INFO_NETWORK_BEARER_E,                   "clear_device_info_callback"       },
    {"clear_id",  "mcc",       QAPI_DEVICE_INFO_MCC_E,                              "clear_device_info_callback"       },
    {"clear_id",  "mnc",       QAPI_DEVICE_INFO_MNC_E,                              "clear_device_info_callback"       },
    {"clear_id",  "cell_id",   QAPI_DEVICE_INFO_CELL_ID_E,                          "clear_device_info_callback"       },
    {"clear_id",  "4g_sig",    QAPI_DEVICE_INFO_4G_SIG_STRENGTH_E,                  "clear_device_info_callback"       },
    {"clear_id",  "link_qual", QAPI_DEVICE_INFO_LINK_QUALITY_E,                     "clear_device_info_callback"       },
    {"clear_id",  "tx_bytes",  QAPI_DEVICE_INFO_TX_BYTES_E,                         "clear_device_info_callback"       },
    {"clear_id",  "rx_bytes",  QAPI_DEVICE_INFO_RX_BYTES_E,                         "clear_device_info_callback"       },
    {"clear_id",  "roaming",   QAPI_DEVICE_INFO_ROAMING_E,                          "clear_device_info_callback"       },
    {"clear_id",  "lte_op",    QAPI_DEVICE_INFO_LTE_OP_MODE_E,                      "clear_device_info_callback"       },
    {"clear_id",  "active",    QAPI_DEVICE_INFO_ACTIVE_TIMER_E,                     "clear_device_info_callback"       },
    {"clear_id",  "psm",       QAPI_DEVICE_INFO_PSM_TIMER_E,                        "clear_device_info_callback"       },
    {"clear_id",  "wms",       QAPI_DEVICE_INFO_WMS_EVENT_REPORT_E,                 "clear_device_info_callback"       },
    {"clear_id",  "sim_state", QAPI_DEVICE_INFO_SIM_STATE_E,                        "clear_device_info_callback"       },
    {"clear_id",  "edrx",      QAPI_DEVICE_INFO_EDRX_E,                             "clear_device_info_callback"       },
    {"clear_id",  "jammer",    QAPI_DEVICE_INFO_JAMMER_IND_E,                       "clear_device_info_callback"       },
    {"clear_id",  "ims_status",QAPI_DEVICE_INFO_IMS_IOT_REGISTRATION_STATUS_IND_E,  "clear_device_info_callback"       },
    {"clear_id",  "voice_call",QAPI_DEVICE_INFO_VOICE_ALL_CALL_STATUS_IND_E,        "clear_device_info_callback"       },
    {"clear_id",  "modem_state",QAPI_DEVICE_INFO_MODEM_SERVICE_STATE_E,             "clear_device_info_callback"       },
    {"clear_id",  "sleep_info",QAPI_DEVICE_INFO_WWAN_SLEEP_INFO_E,                  "clear_device_info_callback"       },
    {"clear_id",  "mt_sms",    QAPI_DEVICE_INFO_INCOMING_SMS_E,                     "clear_device_info_callback"       },
    {"clear_id",  "mt_route_sms",QAPI_DEVICE_INFO_ROUTE_INCOMING_SMS_E,             "clear_device_info_callback"       },
    {"clear_id",  "sms_status_report",QAPI_DEVICE_INFO_SMS_STATUS_REPORT_E,         "clear_device_info_callback"       }
  };

  if(Parameter_Count < 1)
  {
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }

  /* qapi_Device_Info_xxx API test */
  for(i=0; i<sizeof(cmd_list)/sizeof(dev_info_command_t); i++)
  {
    if (0 == strcmp((char*)Parameter_List[0].String_Value, cmd_list[i].cmd_1))
    {
      if(cmd_list[i].cmd_2 == NULL)
      {
        result = cmd_list[i].func(Parameter_Count - 1, &Parameter_List[1], app_id);
        goto end;
      }
      else if(Parameter_Count > 1 && 0 == strcmp((char*)Parameter_List[1].String_Value, cmd_list[i].cmd_2))
      {
        if(NULL == app_devinfo_cli_ctx[app_id])
          {
              DEVINFO_PRINTF("Device info not initialized, Please do init first\n");
              result = QCLI_STATUS_SUCCESS_E;
              goto end;
          }
          result = cmd_list[i].func(Parameter_Count - 1, &Parameter_List[1], app_id);
          goto end;
      }
    }
  }
  /* common qapi_Device_Info_Get_v2 API test */
  for(i=0; i<sizeof(get_cmd_list)/sizeof(dev_info_get_command_t); i++)
  {
    if (0 == strcmp((char*)Parameter_List[0].String_Value, get_cmd_list[i].cmd_1))
    {
      if((get_cmd_list[i].cmd_2 == NULL && Parameter_Count == 1) || /*get_device_info2 cmd*/
         (Parameter_Count > 1 && 0 == strcmp((char*)Parameter_List[1].String_Value, get_cmd_list[i].cmd_2)))
      {
        result = get_device_info_common(Parameter_Count - 1, &Parameter_List[1], app_id, get_cmd_list+i);
        goto end;
      }
    }
  }
  /* qapi_Device_Info_Get_v2 API on cell info test */
  for(i=0; i<sizeof(get_cmd_ext_cell_info_list)/sizeof(dev_info_get_command_t); i++)
  {
    if (0 == strcmp((char*)Parameter_List[0].String_Value, get_cmd_ext_cell_info_list[i].cmd_1))
    {
      if((get_cmd_ext_cell_info_list[i].cmd_2 == NULL && Parameter_Count == 1) || /*get_device_info2 cmd*/
         (Parameter_Count > 1 && 0 == strcmp((char*)Parameter_List[1].String_Value, get_cmd_ext_cell_info_list[i].cmd_2)))
      {
        result = get_device_info_common_cell_info(Parameter_Count - 1, &Parameter_List[1], app_id, get_cmd_ext_cell_info_list+i);
        goto end;
      }
    }
  }

  /* common qapi_Device_Info_Set API test */
  for(i=0; i<sizeof(set_cmd_list)/sizeof(dev_info_set_command_t); i++)
  {
    if (0 == strcmp((char*)Parameter_List[0].String_Value, set_cmd_list[i].cmd_1))
    {
      if(Parameter_Count > 1 && 0 == strcmp((char*)Parameter_List[1].String_Value, set_cmd_list[i].cmd_2))
      {
        result = set_device_info_common(Parameter_Count - 1, &Parameter_List[1], app_id, set_cmd_list+i);
        goto end;
      }
    }
  }

  for(i=0; i<sizeof(set_cmd_list_ind)/sizeof(dev_info_set_command_t); i++)
  {
    if (0 == strcmp((char*)Parameter_List[0].String_Value, set_cmd_list_ind[i].cmd_1))
    {
      if(Parameter_Count > 1 && 0 == strcmp((char*)Parameter_List[1].String_Value, set_cmd_list_ind[i].cmd_2))
      {
        result = set_device_info_callback(Parameter_Count - 1, &Parameter_List[1], app_id, set_cmd_list_ind+i);
        goto end;
      }
    }
  }

  for(i=0; i<sizeof(clear_cmd_list_ind)/sizeof(dev_info_set_command_t); i++)
  {
    if (0 == strcmp((char*)Parameter_List[0].String_Value, clear_cmd_list_ind[i].cmd_1))
    {
      if(Parameter_Count > 1 && 0 == strcmp((char*)Parameter_List[1].String_Value, clear_cmd_list_ind[i].cmd_2))
      {
        result = clear_device_info_callback(Parameter_Count - 1, &Parameter_List[1], app_id, clear_cmd_list_ind+i);
        goto end;
      }
    }
  }

result = QCLI_STATUS_USAGE_E;

end:
if (Parameter_List != NULL)
{
  for (i = 0; i<Parameter_Count; i++)
  {
    if (Parameter_List[i].String_Value != NULL)
    {
      FREE_MEMORY(Parameter_List[i].String_Value);
      Parameter_List[i].String_Value = NULL;
    }
  }
}
return result;
}

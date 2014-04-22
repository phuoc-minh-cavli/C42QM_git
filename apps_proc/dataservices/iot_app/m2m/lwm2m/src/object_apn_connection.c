/******************************************************************************

  @file    object_apn_connection.c
  @brief   APN connection profile object source file
           This file implements API's related to apn connection object 
           This object specifies resources to enable a device to connect to an APN.
  ---------------------------------------------------------------------------
  Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

#include "liblwm2m.h"
#include "internals.h"
#include "lwm2mclient.h"
#include "object_apn_connection.h"
#include "qapi_ns_utils.h"
#include "qapi_timer.h"
#include "wireless_data_service_v01.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lwm2m_rx.h"
#include "ds_log.h"


#ifdef ENABLE_APN_CONN_OBJ

/* Default resource values */
#define DEFAULT_PROFILE_NAME "QCOM_PROFILE"
#define DEFAULT_APN_NAME     "internet.15.234"
#define DEFAULT_USER_NAME    "admin"
#define DEFAULT_SECRET       "password"
#define DEFAULT_RECONNECT    "10,60,600,3600,86400"
#define DEFAULT_IP_ADDRESS   "0.0.0.0"
#define DEFAULT_PREFIX       "64"
#define DEFAULT_SUBNET       "255.255.255.255"
#define DEFAULT_GATEWAY      "0.0.0.0"
#define DEFAULT_PRI_DNS      "0.0.0.0"
#define DEFAULT_SEC_DNS      "0.0.0.0"
#define GPS_TO_UNIX_TIME_OFFSET   315964800

#define PDN_TYPE_NON_IP  0x00
#define PDN_TYPE_IPv4    0x01
#define PDN_TYPE_IPv6    0x02
#define PDN_TYPE_IPv4v6  0x03

typedef enum 
{
  CREATE_REQUEST = 0x01,
  WRITE_REQUEST  = 0x02,
  SET_REQUEST    = 0x03,
}operation_request_enum;

/* Used to check whether free object is called from load persistent function or not */
bool persist_call = FALSE;

static bool create_operation = LWM2M_FALSE;

extern lwm2m_pending_pkt pending_dl_pkt;
extern bool enable_apn_conn_obj;
typedef struct
{
  int  obj11_val;
  int  qmi_val;
}wds_map_type;

const wds_map_type pdp_type_table[ ] = 
{
  {PDN_TYPE_NON_IP,  IOTAPP_WDS_PDP_TYPE_PDP_NON_IP_V01},
  {PDN_TYPE_IPv4,  IOTAPP_WDS_PDP_TYPE_PDP_IPV4_V01},
  {PDN_TYPE_IPv6,  IOTAPP_WDS_PDP_TYPE_PDP_IPV6_V01},
  {PDN_TYPE_IPv4v6,  IOTAPP_WDS_PDP_TYPE_PDP_IPV4V6_V01},
};

const wds_map_type auth_type_table[ ] = 
{
  {APN_AUTH_PREF_PAP_ONLY_ALLOWED,  QAPI_DSS_AUTH_PREF_PAP_ONLY_ALLOWED_E},
  {APN_AUTH_PREF_CHAP_ONLY_ALLOWED,  QAPI_DSS_AUTH_PREF_CHAP_ONLY_ALLOWED_E},
  {APN_AUTH_PREF_PAP_CHAP_BOTH_ALLOWED,  QAPI_DSS_AUTH_PREF_PAP_CHAP_BOTH_ALLOWED_E},
  {APN_AUTH_PREF_PAP_CHAP_NOT_ALLOWED,  QAPI_DSS_AUTH_PREF_PAP_CHAP_NOT_ALLOWED_E},
};

/* This function will either create/modify the data profile based on the profile_
   action_enum.
   If the request is from Create/Write formulate the response message and fill the
   status code based on QMI return value. Send the response message to the server.
   If the request is from SET no need to formulate and send response message.
   One special case where disable APN is set by the server using the same APN name
   then we send the response first and then call QMI message.*/
int lwm2m_profile_action(void *userData)
{
  profile_params_t *params = (profile_params_t *)userData;
  int rc = -1;
  qapi_Coap_Packet_t *message = NULL; 
  qapi_Coap_Message_Params_t tx_message;
  lwm2m_server_t *server = NULL;
  uint8_t result = LWM2M_NO_ERROR;
  bool bs_server = FALSE;
  wds_mandatory_res_changed_t c_info = {0};

  if(!enable_apn_conn_obj)
  {
    LOG_DEBUG("APN OBJ DISABLED");
    return rc ;
  }
  
  if(params)
  {
    if(params->req_type != SET_REQUEST)
    {
      if(data.lwm2mH)
      {
        server = data.lwm2mH->bootstrapServer;
        if(server)
        {
          /* Short server ID match - server context found */
          if (server->shortID != params->ssid)
            server = NULL; 
        }

        if(server == NULL)
        {
          server = (lwm2m_server_t *)LWM2M_LIST_FIND(data.lwm2mH->serverList, params->ssid);
        }
        else
        {
          bs_server = TRUE;
        }
        if(server == NULL)
        goto end;

        memset(&tx_message, 0x00, sizeof(qapi_Coap_Message_Params_t));
        tx_message.msgUsrData = (void *)server;

        if (QAPI_ERROR == qapi_Coap_Init_Message(server->coapHandle,&message, QAPI_COAP_TYPE_NON, LWM2M_NO_ERROR))
        {
          goto end;
        }

        qapi_Coap_Set_Header(server->coapHandle,message, QAPI_COAP_TOKEN, params->token, params->token_len);

        if(params->delay_timer)
        {
          result = LWM2M_204_CHANGED;
          qapi_Coap_Set_Header(server->coapHandle,message, QAPI_COAP_SET_STATUS, &result,0);

          qapi_Coap_Send_Message(server->coapHandle ,message, &tx_message);

          qapi_Coap_Free_Message(server->coapHandle,message);
        }
      }
    }

    switch(params->type)
    {
      case PROFILE_NO_ACTION:
      rc = iotapp_qmi_wds_modify_profile_lite(params, REQ_TYPE_OBJ_11, c_info);
      break;
  
      case PROFILE_MODIFY:
      {
        if(params->modified_pdp == TRUE)
        {
          c_info.pdp_type_valid = TRUE;
          c_info.pdp_type       = (qapi_Device_Info_PDP_Type_e)params->pdp_type;
          params->pdp_type      = (iotapp_wds_pdp_type_enum_v01)params->old_pdp_type;
        }
        rc = iotapp_qmi_wds_modify_profile_lite(params, REQ_TYPE_OBJ_11, c_info);
      }
      break;
  
      case PROFILE_CREATE:
      rc = iotapp_qmi_wds_create_profile_lite(params, REQ_TYPE_OBJ_11);
      break;
  
      default:
      break;
    }
  
    ds_iot_ulog_high1("Profile action result %d", rc);

    if(rc != 0)
    {
      result = LWM2M_400_BAD_REQUEST;
      if(params->req_type == CREATE_REQUEST)
      {
        lwm2m_remove_object_instance(LWM2M_APN_CONN_PROFILE_OBJECT_ID, params->instanceID, true);
      }
    }
    else if(params->req_type == CREATE_REQUEST && bs_server == FALSE)
    {
      result = LWM2M_201_CREATED;
    }
    else if(params->req_type == WRITE_REQUEST || 
            (params->req_type == CREATE_REQUEST && bs_server == TRUE))
    {
      result = LWM2M_204_CHANGED;
    }

    if(!params->delay_timer && params->req_type != SET_REQUEST && data.lwm2mH && server != NULL)
    {
      qapi_Coap_Set_Header(server->coapHandle,message, QAPI_COAP_SET_STATUS, &result,0);

      qapi_Coap_Send_Message(server->coapHandle ,message, &tx_message);

      qapi_Coap_Free_Message(server->coapHandle,message);
    }
end:
    lwm2m_free(params);
  }
  return rc;
}

/**
* @brief  API to check whether object instance of APN link info exists or not..
*
* @param[in]  apn_link  Object link resource value (Object_id:Instance_id)
*
* @return On success - true, on failure - false
*/
bool find_apn_link_info(objlink_t apn_link)
{
  uint16_t obj_id = 0xFFFF;
  uint16_t inst_id = 0xFFFF;
  lwm2m_object_t *apn_object_list = NULL;
  apn_conn_profile_instance_t *instanceP = NULL;

  obj_id  = apn_link >> 16;
  inst_id = apn_link & 0xFFFF;

  if(obj_id != LWM2M_APN_CONN_PROFILE_OBJECT_ID || inst_id == 0xFFFF)
  {
    /* Invalid parameters passed. */
    return false;
  }

  if(data.lwm2mH == NULL)
  {
    LOG_DEBUG("%s context",nullError);
    return false;
  }
  apn_object_list = (lwm2m_object_t *)LWM2M_LIST_FIND(data.lwm2mH->objectList, LWM2M_APN_CONN_PROFILE_OBJECT_ID);
  if(apn_object_list == NULL)
  {
    /* Object 11 not found */
    return false;
  }

  instanceP = (apn_conn_profile_instance_t *)LWM2M_LIST_FIND(apn_object_list->instanceList, 
                                                             inst_id);
  if (NULL == instanceP)
  {
    /* Instance id not found */
    return false;
  }

  return true;
}

/* Utility function to find the resource instance ID corresponding to resource id
   passed 
   Return resource instance ID (if found) otherwise return NULL*/
static resource_instance_conn_list_t * find_conn_resource_inst(resource_instance_conn_list_t * head,
                                                               uint16_t id, uint16_t res_id)
{
  while (NULL != head && head->resInstId < id)
  {
    head = head->next;
  }

  if (NULL != head && head->resInstId == id)
  {
    switch(res_id)
    {
      case RES_O_CONN_ESTB_TIME:
      {
        if(head->conn_estblsh_time_isValid) return head;
        else return NULL;
      }

      case RES_O_CONN_ESTB_RESULT:
      {
        if(head->conn_estblsh_result_isValid) return head;
        else return NULL;
      }

      case RES_O_CONN_ESTB_REJECT_CAUSE:
      {
        if(head->conn_reject_cause_isValid) return head;
        else return NULL;
      }

      case RES_O_CONN_END_TIME:
      {
        if(head->conn_end_time_isValid) return head;
        else return NULL;
      }
    
      default:
      break;
    }
  }

  return NULL;
}

/* Utility function to remove the connection stats based on resource instance id
   passed.
   Return true for success false for failure.*/
static bool prv_delete_resource_resource_conn_val(resource_instance_conn_list_t** rsList,
                                                  uint16_t resId)
{
  bool ret = false;
  resource_instance_conn_list_t *resInst = NULL;

  if(*rsList == NULL)
  {
    return ret;
  }

  *rsList = (resource_instance_conn_list_t*)LWM2M_LIST_RM(*rsList, resId, &resInst);

  if (resInst==NULL)
  {
    return ret;
  }
  else
  {
    lwm2m_free(resInst);
    resInst = NULL;
    ret = true;
  }

  return ret;
}

/* Utility function to create entry in Connection stats list.
   Based on "Connection count" config item limits the no of entries.
   Newest entry would always start with 0.
   Return true for success false for failure.*/
static bool prv_write_resource_conn_val(resource_instance_conn_list_t** rsList,
                                        uint8_t family, time_t resValue)
{
  bool ret = false;
  resource_instance_conn_list_t *resInst = NULL, *temp = *rsList, *prev = NULL;

  resInst = (resource_instance_conn_list_t *)lwm2m_malloc(sizeof(resource_instance_conn_list_t));
  if (resInst==NULL)
  {
    return ret;
  }
  else
  {
    memset(resInst, 0, sizeof(resource_instance_conn_list_t));
    resInst->resInstId                 = 0;
    resInst->family                    = family;
    resInst->conn_estblsh_time_isValid = LWM2M_TRUE;
    resInst->conn_estblsh_time         = resValue;

    /* Increment the resource instance id of old entries. */
    while(temp)
    {
      temp->resInstId++;
      prev = temp;
      temp = temp->next;
    }

    /* Remove the oldest entry */
    if(prev && prev->resInstId == data.cfg_info.connection_count)
    {
      prv_delete_resource_resource_conn_val(rsList, prev->resInstId);
    }

    *rsList = (resource_instance_conn_list_t*)LWM2M_LIST_ADD(*rsList, resInst);
    ret = true;
  }

  return ret;
}

/**
 * @fn uint8_t store_apn_info_persistent_info()
 * @brief This function is used to store the apn connection profile into persistent storage
 * 
 * @param targetP pointer to APN conn object instance
 * @param store   to check whether instance stored is valid or not.
 * 
 * @return return error code
 */
int store_apn_info_persistent_info(apn_conn_profile_instance_t * targetP, bool store)
{
  int fd = 0, ret = 0, index = 0;
  apn_conn_persistent_instance_t *info = NULL;
  apn_conn_persistent_instance_t *conn_ctx = NULL;

  if(NULL == targetP)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }

  info = (apn_conn_persistent_instance_t *)malloc(sizeof(apn_conn_persistent_instance_t));
  if(info == NULL)
  {
    return EFAILURE;
  }
  conn_ctx = (apn_conn_persistent_instance_t *)malloc(sizeof(apn_conn_persistent_instance_t));
  if(conn_ctx == NULL)
  {
    free(info);
    return EFAILURE;
  }

  /* open apn connection info persistent file.If not already present, create it*/
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_APN_CONN_PERSISTENCE_FILE);
  fd = efs_open(LWM2M_APN_CONN_PERSISTENCE_FILE, O_CREAT | O_RDWR, 00644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err(" failed to open apn conn persistence file");
    ret = EFAILURE;
    goto end;
  }

  while (1)
  {
    memset(info, 0x00, sizeof(apn_conn_persistent_instance_t));
    ret = efs_read(fd, info, sizeof(apn_conn_persistent_instance_t));
    if (ret <= 0)
    {
      break;
    }
    if(info->instanceId == targetP->instanceId)
    {
      break;
    }
    index++;
  }

  memset(conn_ctx, 0x00, sizeof(apn_conn_persistent_instance_t));

  LWM2M_STRCPY(conn_ctx->profile_name, targetP->profile_name, strlen(targetP->profile_name) +1);
  conn_ctx->apn_name_valid = targetP->apn_name_valid;
  LWM2M_STRCPY(conn_ctx->apn_name, targetP->apn_name, strlen(targetP->apn_name) +1);
  conn_ctx->auth_type = targetP->auth_type;
  LWM2M_STRCPY(conn_ctx->username, targetP->username, strlen(targetP->username) +1);
  LWM2M_STRCPY(conn_ctx->secret, targetP->secret, strlen(targetP->secret) +1);
  LWM2M_STRCPY(conn_ctx->reconn_schedule, targetP->reconn_schedule, strlen(targetP->reconn_schedule) +1);
  conn_ctx->pdn_type = targetP->pdn_type;
  conn_ctx->params_set = targetP->params_set;
  conn_ctx->instanceId = targetP->instanceId;
  conn_ctx->enable_status = targetP->enable_status;
  if(store)
  {
    conn_ctx->isValid = LWM2M_TRUE; 
  }
  else
  {
    conn_ctx->isValid = LWM2M_FALSE; 
  }

  /* lseek to reqd index and write the apn_persist_info structure in efs */
  efs_lseek(fd, index * sizeof(apn_conn_persistent_instance_t), SEEK_SET);
  ds_iot_log_strf_med("EFS_OPT: File write %s",LWM2M_APN_CONN_PERSISTENCE_FILE);
  if (EFAILURE == efs_write(fd, conn_ctx, sizeof(apn_conn_persistent_instance_t)))
  {
    ds_iot_ulog_err(" failed to store persistent value of apn conn instance");
    efs_close(fd);
    ret = EFAILURE;
    goto end;
  }

  efs_close(fd);
  ret = ESUCCESS;

end:
  free(info);
  free(conn_ctx);
  return ret;
}

/**
 * @fn uint8_t load_apn_info_persistent_info()
 * @brief This function is used to fetch the apn connection profile from persistent storage
 * 
 * @param objectP pointer to APN conn object
 * 
 * @return return error code
 */
int load_apn_info_persistent_info(lwm2m_object_t * objectP)
{
  int fd = 0, ret = 0;

  if (NULL == objectP)
  {
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_APN_CONN_PERSISTENCE_FILE);
  fd = efs_open(LWM2M_APN_CONN_PERSISTENCE_FILE, O_RDONLY);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err(" failed to open apn conn persistent storage");
    return EFAILURE;
  }

  while(1)
  {
    apn_conn_persistent_instance_t info;
    apn_conn_profile_instance_t *instanceP = NULL;

    memset(&info, 0, sizeof(info));
    ret = efs_read(fd, &info, sizeof(info));
    if (EFAILURE == ret || 0 == ret)
      break;

    /*Check if the structure entry in file is valid*/
    if(LWM2M_FALSE == info.isValid)
    {
      continue;
    }

    instanceP = (apn_conn_profile_instance_t *)lwm2m_malloc(sizeof(apn_conn_profile_instance_t));
    if(instanceP == NULL)
    {
      ret = -1;
      break;
    }

    memset(instanceP, 0, sizeof(apn_conn_profile_instance_t));
    prv_apn_conn_default_values(instanceP);

    LWM2M_STRCPY(instanceP->profile_name, info.profile_name, strlen(info.profile_name) +1);
    instanceP->apn_name_valid = info.apn_name_valid;
    LWM2M_STRCPY(instanceP->apn_name, info.apn_name, strlen(info.apn_name) +1);
    instanceP->auth_type = info.auth_type;
    LWM2M_STRCPY(instanceP->username,info.username, strlen(info.username) +1);
    LWM2M_STRCPY(instanceP->secret, info.secret, strlen(info.secret) +1);
    LWM2M_STRCPY(instanceP->reconn_schedule, info.reconn_schedule, strlen(info.reconn_schedule) +1);
    instanceP->pdn_type = info.pdn_type;
    instanceP->params_set = info.params_set;
    instanceP->instanceId = info.instanceId;
    instanceP->enable_status = info.enable_status;

    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, instanceP);
  }
  if (ret < 0)
  {
    efs_close(fd);
    persist_call = TRUE;
    clean_apn_conn_object(objectP);
    lwm2m_free(objectP);
    objectP = NULL;

    persist_call = FALSE;
    return EFAILURE;
  }
  efs_close(fd);
  return ESUCCESS;
}


/**
 * @brief This function is used to write the resources into object instance
 *        while creating the object instance.or got write request from server.

 * @param   objectP (IN)   handle to object
 * @param   instanceP (IN/OUT)   handle to object instance
 * @param   numData (IN)        number of resources to be written
 * @param   dataArray (IN)      array of resource values
 * @param resInstId Resource instance id or INVALID_RES_INST_ID 
 *                  if resource instance id is not passed.
 * @param write_method To distinguish between partial write or replace.
 * @param req_type Source of Request (Create/Write/Set API)
 * @return  LWM2M response type
 */
static uint8_t prv_apn_conn_write_resources(lwm2m_object_t * objectP,
                                            apn_conn_profile_instance_t *instanceP,
                                            int numData,
                                            lwm2m_data_t *dataArray,
                                            uint16_t resInstId, uint8_t write_method,
                                            operation_request_enum req_type)
{
  int i = 0;
  uint8_t result = LWM2M_204_CHANGED;
  bool delay_timer = FALSE;
  bool val_changed = FALSE;
  if( instanceP == NULL  || dataArray == NULL )
  {
    LOG_DEBUG(" %s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  
  do
  {
    switch (dataArray[i].id)
    {
      case RES_M_PROFILE_NAME:
      {
        if(lwm2m_strcmp(instanceP->profile_name, (char *)(dataArray + i)->value.asBuffer.buffer) != 0)
        {
          val_changed = TRUE;
        }
        if(((dataArray + i)->value.asBuffer.length) && ((dataArray + i)->value.asBuffer.buffer))
        {
          LWM2M_STRCPY(instanceP->profile_name, (const char *)((dataArray + i)->value.asBuffer.buffer),
                      ((dataArray + i)->value.asBuffer.length) + 1);
          instanceP->params_set |= PROFILE_NAME_SET;
        }
        else
        {
          return LWM2M_400_BAD_REQUEST;
        }
      }
      break;

      case RES_O_APN:
      {
        if(lwm2m_strcmp(instanceP->apn_name, (char *)(dataArray + i)->value.asBuffer.buffer) != 0)
        {
          val_changed = TRUE;
        }
        if(((dataArray + i)->value.asBuffer.length) && ((dataArray + i)->value.asBuffer.buffer))
        {
          instanceP->apn_name_valid = TRUE;
          LWM2M_STRCPY(instanceP->apn_name, (const char *)((dataArray + i)->value.asBuffer.buffer),
                    ((dataArray + i)->value.asBuffer.length) + 1);
          instanceP->params_set |= APN_NAME_SET;
        }
        /* Support for Null APN passed from server */
        else if(((dataArray + i)->value.asBuffer.length) == 0)
        {
          instanceP->apn_name_valid = TRUE;
          LWM2M_STRCPY(instanceP->apn_name, "",
                    ((dataArray + i)->value.asBuffer.length) + 1);
          instanceP->params_set |= APN_NAME_SET;
        }
      }
      break;

      case RES_O_AUTO_SELECT_APN:
      {
        bool auto_select = instanceP->auto_apn;
        if(1 != lwm2m_data_decode_bool(dataArray+i, &instanceP->auto_apn))
        {
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(auto_select != instanceP->auto_apn)
          {
            val_changed = TRUE;
          }
          instanceP->params_set |= AUTO_SELECT_SET;
        }
      }
      break;

      case RES_O_ENABLE_STATUS:
      {
        bool enab_status = instanceP->enable_status;
        if(1 != lwm2m_data_decode_bool(dataArray+i, &instanceP->enable_status))
        {
          return LWM2M_400_BAD_REQUEST;
        }
        if(enab_status != instanceP->enable_status)
        {
          val_changed = TRUE;
        }
        if((enab_status != instanceP->enable_status) && 
            instanceP->enable_status == LWM2M_FALSE)
        {
          delay_timer = LWM2M_TRUE;
        }
      }
      break;

      case RES_M_AUTH_TYPE:
      {
        int64_t value = 0;
        if(1 != lwm2m_data_decode_int(dataArray+i, &value))
        {
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if((value < 0 || value > 3) && (value < '0' || value > '3'))
          {
            return LWM2M_400_BAD_REQUEST;
          }
          else
          {
            if(instanceP->auth_type != value)
            {
              val_changed = TRUE;
            }
            instanceP->auth_type = value;
            instanceP->params_set |= AUTH_TYPE_SET;
          }
        }
      }
      break;

      case RES_O_USER_NAME:
      {
        if(lwm2m_strcmp(instanceP->username, (char *)(dataArray + i)->value.asBuffer.buffer) != 0)
        {
          val_changed = TRUE;
        }
        if(((dataArray + i)->value.asBuffer.length) && ((dataArray + i)->value.asBuffer.buffer))
        {
          LWM2M_STRCPY(instanceP->username, (const char *)((dataArray + i)->value.asBuffer.buffer),
                    ((dataArray + i)->value.asBuffer.length) + 1);
          instanceP->params_set |= USER_NAME_SET;
        }
        else
        {
          return LWM2M_400_BAD_REQUEST;
        }
      }
      break;

      case RES_O_SECRET:
      {
        if(lwm2m_strcmp(instanceP->secret, (char *)(dataArray + i)->value.asBuffer.buffer) != 0)
        {
          val_changed = TRUE;
        }
        if(((dataArray + i)->value.asBuffer.length) && ((dataArray + i)->value.asBuffer.buffer))
        {
          LWM2M_STRCPY(instanceP->secret, (const char *)((dataArray + i)->value.asBuffer.buffer),
                    ((dataArray + i)->value.asBuffer.length) + 1);
          instanceP->params_set |= SECRET_SET;
        }
        else
        {
          return LWM2M_400_BAD_REQUEST;
        }
      }
      break;

      case RES_O_RECONN_SCHEDULE:
      {
        if(lwm2m_strcmp(instanceP->reconn_schedule, (char *)(dataArray + i)->value.asBuffer.buffer) != 0)
        {
          val_changed = TRUE;
        }
        if(((dataArray + i)->value.asBuffer.length) && ((dataArray + i)->value.asBuffer.buffer))
        {
          LWM2M_STRCPY(instanceP->reconn_schedule, (const char *)((dataArray + i)->value.asBuffer.buffer),
                    ((dataArray + i)->value.asBuffer.length) + 1);
          instanceP->params_set |= RECONNECT_SET;
        }
        else
        {
          return LWM2M_400_BAD_REQUEST;
        }
      }
      break;

      case RES_O_PDN_TYPE:
      {
        int64_t value = 0;
        if(1 != lwm2m_data_decode_int(dataArray+i, &value))
        {
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if((value < 0 || value > 3) && (value < '0' || value > '3'))
          {
            return LWM2M_400_BAD_REQUEST;
          }
          else
          {
            if(instanceP->pdn_type != value)
            {
              val_changed = TRUE;
            }
            instanceP->pdn_type = value;
            instanceP->params_set |= PDN_TYPE_SET;
          }
        }
      }
      break;

      case RES_O_VENDOR_EXT:
      {
        uint16_t obj_id = 0;
        uint16_t inst_id = 0;
        objlink_t obj_link = 0x00;

        if (0x00 == lwm2m_data_decode_objLink(&dataArray[i], &obj_link))
        {
          return LWM2M_400_BAD_REQUEST;
        }

        obj_id = (obj_link >> 16);
        inst_id = (obj_link & 0xFFFF);
        if(obj_id > 0xFFFF || inst_id > 0xFFFF)
        {
          return LWM2M_400_BAD_REQUEST;
        }
        if(instanceP->vendor_ext != obj_link)
        {
          val_changed = TRUE;
        }
        instanceP->vendor_ext = obj_link;
      }
      break;

      default:
        /*If create/write have one more one resource to write we write on unknown/notsupported resource*/
        if(numData > 1)
          result = LWM2M_204_CHANGED;
        else
          result = LWM2M_404_NOT_FOUND;
    }
    i++;
  } while (i < numData && result == LWM2M_204_CHANGED);

  /* Update the persistent file */
  if (LWM2M_204_CHANGED == result)
  {
    int rc = -1;
    
    if(instanceP->apn_name_valid)
    {
      profile_params_t *params = NULL;
      wds_mandatory_res_t req;
      profile_search_response_t response = {0};

      req.pdp_type = (qapi_Device_Info_PDP_Type_e)pdp_type_table[instanceP->pdn_type].qmi_val;
      req.apn_name_valid = TRUE;
      memscpy(req.apn_name, sizeof(req.apn_name), instanceP->apn_name,
               sizeof(instanceP->apn_name));

      rc = iotapp_qmi_wds_search_profile_lite((void *)&req,&response);

      ds_iot_ulog_high1(" Search profile result %d", rc); 
      if(rc != 0)
      {
        result = LWM2M_400_BAD_REQUEST;
        return result;
      }
      else
      {
        lwm2m_context_t *contextP = NULL;

        params = lwm2m_malloc(sizeof(profile_params_t));
        if(params == NULL)
        {
          result = LWM2M_500_INTERNAL_SERVER_ERROR;
          return result;
        }
        memset(params, 0x00, sizeof(profile_params_t));
        LWM2M_STRCPY(params->apn_name, instanceP->apn_name,
                    (strlen(instanceP->apn_name) + 1));
        params->enable_status = instanceP->enable_status;
        if(instanceP->params_set & USER_NAME_SET)
        {
          params->username_valid = TRUE;
          LWM2M_STRCPY(params->username, instanceP->username,
                      (strlen(instanceP->username) + 1));
        }
        if(instanceP->params_set & SECRET_SET)
        {
          params->password_valid = TRUE;
          LWM2M_STRCPY(params->password, instanceP->secret,
                      (strlen(instanceP->secret) + 1));
        }
        if(instanceP->params_set & AUTH_TYPE_SET)
        {
          params->authentication_preference_valid = TRUE;
          params->authentication_preference = auth_type_table[instanceP->auth_type].qmi_val;
        }
        params->req_type = req_type;
        params->delay_timer = delay_timer;
        if(req_type != SET_REQUEST)
        {
          contextP = get_lwm2m_context();
          if(contextP && contextP->activeServerP)
          {
            params->ssid = contextP->activeServerP->shortID;
          }
          if(pending_dl_pkt.dl_pkt != NULL)
          {
            params->token_len = pending_dl_pkt.dl_pkt->token_len;
            memcpy(params->token, pending_dl_pkt.dl_pkt->token, params->token_len);
          }
        }
      }
      switch(response.search_result)
      {
        case PROFILE_NO_ACTION:
        {
          params->type = PROFILE_NO_ACTION;
          params->pdp_type = (iotapp_wds_pdp_type_enum_v01)pdp_type_table[instanceP->pdn_type].qmi_val;
          params->old_pdp_type = (iotapp_wds_pdp_type_enum_v01)response.old_pdp_type;
          params->modified_pdp = FALSE;
        }
        break;
        
        case PROFILE_MODIFY:
        {
          params->type = PROFILE_MODIFY;
          params->pdp_type = (iotapp_wds_pdp_type_enum_v01)pdp_type_table[instanceP->pdn_type].qmi_val;
          params->old_pdp_type = (iotapp_wds_pdp_type_enum_v01)response.old_pdp_type;
          params->modified_pdp = response.old_pdp_type_valid;
        }
        break;
        
        case PROFILE_CREATE:
        {
          params->type = PROFILE_CREATE;
          params->pdp_type = (iotapp_wds_pdp_type_enum_v01)pdp_type_table[instanceP->pdn_type].qmi_val;
        }
        break;

        default:
        {
          result = LWM2M_400_BAD_REQUEST;
          lwm2m_free(params);
          return result;
        }
      }

      if(req_type == SET_REQUEST)
      {
        rc = lwm2m_profile_action((void *)params);
      }
      else
      {
        lwm2m_pkt_t *pkt = NULL;
        pkt = lwm2m_get_pkt_buffer();

        if(pkt == NULL)
        {
          ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED");
          result = LWM2M_400_BAD_REQUEST;
          lwm2m_free(params);
          return result;
        }
        memset(&pkt->cmd_data, 0x00, sizeof(lwm2m_pkt_data_t));
        memset(&pkt->cmd_hdr, 0x00, sizeof(lwm2m_pkt_hdr_t));
        pkt->cmd_hdr.cmd_id = LWM2M_PROFILE_PROCESSING;
        pkt->cmd_data.clientData = (void *)params;
        lwm2m_enqueue_pkt(pkt);
        lwm2m_set_signal();
        rc = 0;
      }
      
      if(rc != 0)
      {
        return LWM2M_400_BAD_REQUEST;
      }
    }
    if(create_operation == TRUE || val_changed == TRUE)
    {
      store_apn_info_persistent_info(instanceP, LWM2M_TRUE);
    }
  }

  return result;
}


/** 
* @fn static uint8_t prv_apn_conn_create_write_check() 
*
* @brief This function verifies and write the resource's into  
*        APN connection profile object 
*
* @param instanceId Instance of the device cap object to be create/write 
* @param numDataP number of resources in the array 
* @param dataArrayP resource value array 
* @param create_flag - if true creating instance else write on instance 
*
* @return LWM2M response type 
*
*/

/***************************************************************************************************

Under section 8.2.2 Operation on Object Instance of OMA 1.1

- For the "Write" operation on an Object Instance, the LwM2M Client MUST perform the operation, 
if all the Resources conveyed in the operation are allowed to perform the "Write" operation. 
If any Resource does not support the "Write" operation, the LwM2M Client MUST inform the LwM2M 
Server that the Object Instance cannot perform the requested "Write" operation in sending a 
"Operation is not supported" error code.

*************************************************************************************************************/ 

static uint8_t prv_apn_conn_create_write_check(int numData,
                                               lwm2m_data_t * dataArray,
                                               bool create_flag)
{ 
  int16_t i = 0; 
  uint8_t result = LWM2M_NO_ERROR; 
  if((NULL == dataArray))
  { 
    LOG_DEBUG(" %s",nullError); 
    return LWM2M_500_INTERNAL_SERVER_ERROR; 
  } 
   
  do
  {
    switch(dataArray[i].id)
    {
      case RES_O_CONN_ESTB_TIME:
      case RES_O_CONN_ESTB_RESULT:
      case RES_O_CONN_ESTB_REJECT_CAUSE:
      case RES_O_CONN_END_TIME:
      case RES_O_TOT_BYTES_SENT:
      case RES_O_TOT_PKTS_SENT:
      case RES_O_TOT_BYTES_RECV:
      case RES_O_IP_ADDR:
      case RES_O_PREFIX_LEN:
      case RES_O_SUBNET_MASK:
      case RES_O_GATEWAY:
      case RES_O_PRI_DNS:
      case RES_O_SEC_DNS:
      case RES_O_VENDOR_EXT:
      case RES_O_APN_CONTROL:
      if (!create_flag)
      {
        result = LWM2M_405_METHOD_NOT_ALLOWED;
        break;
      }
      case RES_M_PROFILE_NAME:
      case RES_O_APN:
      case RES_O_AUTO_SELECT_APN:
      case RES_O_ENABLE_STATUS:
      case RES_M_AUTH_TYPE:
      case RES_O_USER_NAME:
      case RES_O_SECRET:
      case RES_O_RECONN_SCHEDULE:
      case RES_O_PDN_TYPE:
        result = LWM2M_204_CHANGED;
        break;
      default:
      if(!create_flag)
      {
        result = LWM2M_404_NOT_FOUND;
        break;	   
      }
      else
      {
        result = LWM2M_204_CHANGED;
      }
    }
  }while(++i < numData && result == LWM2M_204_CHANGED);

  if(result != LWM2M_204_CHANGED)
  {
     ds_iot_ulog_high1("failed for resource id %d",dataArray[i-1].id);
     return result;
  }
   return result;
}

/**
 * @fn static uint8_t prv_apn_conn_write()
 *
 * @brief This fucntion implements the write operation performed on 
 *        APN connection profile
 *
 * @param instanceId Instance ID of the object to be read
 * @param numData number of resources in the array
 * @param dataArray resource value array
 * @param objectP  Object to be utilized
 * @param resInstId Resource instance id or INVALID_RES_INST_ID 
 *                  if resource instance id is not passed.
 * @param write_method To distinguish between partial write or replace.
 *
 * @return LWM2M response type
 */
static uint8_t prv_apn_conn_write(uint16_t instanceId,
                                  int numData,
                                  lwm2m_data_t * dataArray,
                                  lwm2m_object_t * objectP,
                                  uint16_t resInstId, uint8_t write_method, bool bs_true)
{
  apn_conn_profile_instance_t * targetP = NULL;
  uint8_t result = LWM2M_205_CONTENT;
  bool enable_status_new = TRUE, enable_status_old = TRUE;

  if( dataArray == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err(" LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  targetP = (apn_conn_profile_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
  if (NULL == targetP)
  {
    return LWM2M_404_NOT_FOUND;
  }

  /* Check all resources to be written are writable or not */
  result = prv_apn_conn_create_write_check(numData, dataArray, bs_true);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err ("Write operation failed due to write on Readable resource. ");
    return result;
  }

  enable_status_old = targetP->enable_status;
  /* Write the resource values */
  result = prv_apn_conn_write_resources(objectP, targetP, numData, dataArray,
                                        resInstId, write_method, WRITE_REQUEST);

  enable_status_new = targetP->enable_status;
  if(result == LWM2M_204_CHANGED && enable_status_old != enable_status_new)
  {
    carrier_apn_cfg_t *apn_info = NULL;
    lwm2m_server_t *server_list = NULL;
    uint16_t instance_id = 0xFFFF;
    apn_info = get_lwm2m_apn_cfg();
    while(apn_info)
    {
      instance_id = (apn_info->apn_link & 0xFFFF);
      if(instance_id == instanceId)
      {
        server_list = (lwm2m_server_t *)LWM2M_LIST_FIND(data.lwm2mH->serverList, apn_info->short_server_id);
        if(server_list)
        {
          LWM2M_SERVER_SET_UPDATE_PROFILE_CHANGE(server_list->update_disable_check);
        }
      }
      apn_info = apn_info->next;
    }
  }

  if(result == LWM2M_204_CHANGED && targetP->apn_name_valid == TRUE)
  {
    result = LWM2M_NO_ERROR;
  }

  return result;
}

/**
 * @fn static uint8_t prv_apn_conn_delete()
 *
 * @brief This function is used to delete APN connection profile object
 *
 * @param id Instance ID of the object to be deleted
 * @param objectP Object to be deleted
 *
 * @return LWM2M response type
 */
static uint8_t prv_apn_conn_delete(uint16_t id,
                                   lwm2m_object_t * objectP,
                                   bool persist_file)
{
  apn_conn_profile_instance_t * apnInstance = NULL;

  if(objectP == NULL)
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  /* Remove the instance from object instance list */
  objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&apnInstance);
  if(NULL == apnInstance)
  {
    return LWM2M_404_NOT_FOUND;
  }

  /* Update the persistent file */
  if(persist_file)
    store_apn_info_persistent_info(apnInstance, LWM2M_FALSE);

  /* Free mult-instance resources (if any) */
  if(apnInstance->connection_stats)
  {
    LWM2M_LIST_FREE(apnInstance->connection_stats);
    apnInstance->connection_stats = NULL;
  }

  if(apnInstance->ip_addr)
  {
    LWM2M_LIST_FREE(apnInstance->ip_addr);
    apnInstance->ip_addr = NULL;
  }

  if(apnInstance->prefix_len)
  {
    LWM2M_LIST_FREE(apnInstance->prefix_len);
    apnInstance->prefix_len = NULL;
  }

  if(apnInstance->subnet_mask)
  {
    LWM2M_LIST_FREE(apnInstance->subnet_mask);
    apnInstance->subnet_mask = NULL;
  }

  if(apnInstance->gateway)
  {
    LWM2M_LIST_FREE(apnInstance->gateway);
    apnInstance->gateway = NULL;
  }

  if(apnInstance->pri_dns_addr)
  {
    LWM2M_LIST_FREE(apnInstance->pri_dns_addr);
    apnInstance->pri_dns_addr = NULL;
  }

  if(apnInstance->sec_dns_addr)
  {
    LWM2M_LIST_FREE(apnInstance->sec_dns_addr);
    apnInstance->sec_dns_addr = NULL;
  }

  /* Finally free the instance */
  lwm2m_free(apnInstance);
  apnInstance = NULL;

  return LWM2M_202_DELETED;
}

/**
* @brief  Utility API to retrieve packet statistics,
*
* @param[in]  handle     Handle received after linking successfully with APN Link resource.
* @param[in]  pdn_type   PDN type 
* @param[out] tx_bytes   Total tx bytes.
* @param[out] tx_pkts    Total tx packets.
* @param[out] rx_bytes   Total rx bytes.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK (0) is returned. Other value on error.
*/
qapi_Status_t get_packet_stats(void *hndl, int pdn_type, int family,
                               unsigned long long *tx_bytes, unsigned long long *tx_pkts,
                               unsigned long long *rx_bytes)
{
  qapi_Status_t result = QAPI_ERROR;
  qapi_DSS_Data_Pkt_Stats_t pktStats;

  if(hndl == NULL || tx_bytes == NULL || tx_pkts == NULL || rx_bytes == NULL )
  {
    return result;
  }

  if((pdn_type == PDN_TYPE_IPv4 || pdn_type == PDN_TYPE_IPv4v6 || 
     pdn_type == PDN_TYPE_NON_IP ) && family == PDN_TYPE_IPv4)
  {
    memset(&pktStats, 0x00, sizeof(qapi_DSS_Data_Pkt_Stats_t));
    result = get_packet_info_by_apn_info(hndl, &pktStats);
    if(result == QAPI_OK)
    {
      *tx_bytes = pktStats.bytes_tx;
      *tx_pkts  = pktStats.pkts_tx;
      *rx_bytes = pktStats.bytes_rx;
    }
    else
    {
      return result;
    }
  }
  if((pdn_type == PDN_TYPE_IPv6 || pdn_type == PDN_TYPE_IPv4v6) && family == PDN_TYPE_IPv6)
  {
    memset(&pktStats, 0x00, sizeof(qapi_DSS_Data_Pkt_Stats_t));
    result = get_packet_info_by_apn_info(hndl, &pktStats);
    if(result == QAPI_OK)
    {
      *tx_bytes = pktStats.bytes_tx;
      *tx_pkts  = pktStats.pkts_tx;
      *rx_bytes = pktStats.bytes_rx;
    }
    else
    {
      return result;
    }
  }

  return result;
}

/**
 * @fn static uint8_t prv_set_value()
 * @brief This function is used to populate the values of the resource
 * to be provided to the server during the read operation on the object.
 * @param dataP (OUT) data array for the resource 
 * @param apnConnP (IN) apn connection profile object data
 * @return LWM2M response type
 */
static uint8_t prv_set_value(lwm2m_data_t * dataP,
                             apn_conn_profile_instance_t * apnConnP,
                             uint16_t resInstId)
{
  if( dataP == NULL || apnConnP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  } 

  switch (dataP->id)
  {
    case RES_M_PROFILE_NAME:
      lwm2m_data_encode_string(apnConnP->profile_name, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_APN:
      lwm2m_data_encode_string(apnConnP->apn_name, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_AUTO_SELECT_APN:
      lwm2m_data_encode_bool(apnConnP->auto_apn, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_ENABLE_STATUS:
      {
        if(apnConnP->apn_name_valid)
        {
          apnConnP->enable_status = !(iotapp_qmi_wds_get_apn_disable_status(pdp_type_table[apnConnP->pdn_type].qmi_val,
                                                                          apnConnP->apn_name));
        }
        lwm2m_data_encode_bool(apnConnP->enable_status, dataP);
        return LWM2M_205_CONTENT;
      }

    case RES_M_AUTH_TYPE:
      lwm2m_data_encode_int(apnConnP->auth_type, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_USER_NAME:
      lwm2m_data_encode_string(apnConnP->username, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_SECRET:
      lwm2m_data_encode_string(apnConnP->secret, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_RECONN_SCHEDULE:
      lwm2m_data_encode_string(apnConnP->reconn_schedule, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_CONN_ESTB_TIME:
      {
        if (resInstId != INVALID_RES_INST_ID)
        {
          resource_instance_conn_list_t *resInst;
          resInst = find_conn_resource_inst(apnConnP->connection_stats ,resInstId, RES_O_CONN_ESTB_TIME);
          if(resInst != NULL)
          {
            lwm2m_data_encode_int(resInst->conn_estblsh_time, dataP);
            return LWM2M_205_CONTENT;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          int ri = 0;
          resource_instance_conn_list_t* resInst = NULL;
          for(resInst = apnConnP->connection_stats;
              resInst !=NULL;
              resInst = resInst->next)
          {
            if(resInst->conn_estblsh_time_isValid)
            ri++;
          }

          if (ri==0)  // no values!
          {
            lwm2m_data_t* subTlvP = NULL;
            lwm2m_data_encode_instances(subTlvP, 0, dataP);
            return LWM2M_205_CONTENT;
          }
          else
          {
            int ri_save = 0;
            lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
            if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;

            for (resInst = apnConnP->connection_stats, ri = 0;
                resInst!= NULL;
                resInst = resInst->next, ri++)
            {
              if(resInst->conn_estblsh_time_isValid)
              {
                subTlvP[ri_save].id = ri;
                lwm2m_data_encode_int(resInst->conn_estblsh_time, &subTlvP[ri_save]);
                ri_save++;
              }
            }

            lwm2m_data_encode_instances(subTlvP, ri_save, dataP);

            return LWM2M_205_CONTENT;
          }
        } /* resInstId == INVALID_RES_INST_ID */
      }

    case RES_O_CONN_ESTB_RESULT:
      {
        if (resInstId != INVALID_RES_INST_ID)
        {
          resource_instance_conn_list_t *resInst;
          resInst = find_conn_resource_inst(apnConnP->connection_stats ,resInstId, RES_O_CONN_ESTB_RESULT);
          if(resInst != NULL)
          {
            lwm2m_data_encode_int(resInst->conn_estblsh_result, dataP);
            return LWM2M_205_CONTENT;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          int ri = 0;
          resource_instance_conn_list_t* resInst = NULL;
          for(resInst = apnConnP->connection_stats;
              resInst !=NULL;
              resInst = resInst->next)
          {
            if(resInst->conn_estblsh_result_isValid)
            ri++;
          }

          if (ri==0)  // no values!
          {
            lwm2m_data_t* subTlvP = NULL;
            lwm2m_data_encode_instances(subTlvP, 0, dataP);
            return LWM2M_205_CONTENT;
          }
          else
          {
            int ri_save = 0;
            lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
            if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;

            for (resInst = apnConnP->connection_stats, ri = 0;
                resInst!= NULL;
                resInst = resInst->next, ri++)
            {
              if(resInst->conn_estblsh_result_isValid)
              {
                subTlvP[ri_save].id = ri;
                lwm2m_data_encode_int(resInst->conn_estblsh_result, &subTlvP[ri_save]);
                ri_save++;
              }
            }

            lwm2m_data_encode_instances(subTlvP, ri_save, dataP);

            return LWM2M_205_CONTENT;
          }
        } /* resInstId == INVALID_RES_INST_ID */
      }

    case RES_O_CONN_ESTB_REJECT_CAUSE:
      {
        if (resInstId != INVALID_RES_INST_ID)
        {
          resource_instance_conn_list_t *resInst;
          resInst = find_conn_resource_inst(apnConnP->connection_stats ,resInstId, RES_O_CONN_ESTB_REJECT_CAUSE);
          if(resInst != NULL)
          {
            lwm2m_data_encode_int(resInst->conn_reject_cause, dataP);
            return LWM2M_205_CONTENT;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          int ri = 0;
          resource_instance_conn_list_t* resInst = NULL;
          for(resInst = apnConnP->connection_stats;
              resInst !=NULL;
              resInst = resInst->next)
          {
            if(resInst->conn_reject_cause_isValid)
            ri++;
          }

          if (ri==0)  // no values!
          {
            lwm2m_data_t* subTlvP = NULL;
            lwm2m_data_encode_instances(subTlvP, 0, dataP);
            return LWM2M_205_CONTENT;
          }
          else
          {
            int ri_save = 0;
            lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
            if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;

            for (resInst = apnConnP->connection_stats, ri = 0;
                resInst!= NULL;
                resInst = resInst->next, ri++)
            {
              if(resInst->conn_reject_cause_isValid)
              {
                subTlvP[ri_save].id = ri;
                lwm2m_data_encode_int(resInst->conn_reject_cause, &subTlvP[ri_save]);
                ri_save++;
              }
            }

            lwm2m_data_encode_instances(subTlvP, ri_save, dataP);

            return LWM2M_205_CONTENT;
          }
        } /* resInstId == INVALID_RES_INST_ID */
      } 

    case RES_O_CONN_END_TIME:
      {
        if (resInstId != INVALID_RES_INST_ID)
        {
          resource_instance_conn_list_t *resInst;
          resInst = find_conn_resource_inst(apnConnP->connection_stats ,resInstId, RES_O_CONN_END_TIME);
          if(resInst != NULL)
          {
            lwm2m_data_encode_int(resInst->conn_end_time, dataP);
            return LWM2M_205_CONTENT;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          int ri = 0;
          resource_instance_conn_list_t* resInst = NULL;
          for(resInst = apnConnP->connection_stats;
              resInst !=NULL;
              resInst = resInst->next)
          {
            if(resInst->conn_end_time_isValid)
            ri++;
          }

          if (ri==0)  // no values!
          {
            lwm2m_data_t* subTlvP = NULL;
            lwm2m_data_encode_instances(subTlvP, 0, dataP);
            return LWM2M_205_CONTENT;
          }
          else
          {
            int ri_save = 0;
            lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
            if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;

            for (resInst = apnConnP->connection_stats, ri = 0;
                resInst!= NULL;
                resInst = resInst->next, ri++)
            {
              if(resInst->conn_end_time_isValid)
              {
                subTlvP[ri_save].id = ri;
                lwm2m_data_encode_int(resInst->conn_end_time, &subTlvP[ri_save]);
                ri_save++;
              }
            }

            lwm2m_data_encode_instances(subTlvP, ri_save, dataP);

            return LWM2M_205_CONTENT;
          }
        } /* resInstId == INVALID_RES_INST_ID */
      }

    case RES_O_TOT_BYTES_SENT:
      {
        qapi_Status_t result = QAPI_ERROR;
        unsigned long long tx_bytes_v4 = 0;
        unsigned long long tx_pkts_v4 = 0;
        unsigned long long rx_bytes_v4 = 0;
        unsigned long long tx_bytes_v6 = 0;
        unsigned long long tx_pkts_v6 = 0;
        unsigned long long rx_bytes_v6 = 0;

        if(apnConnP->apn_hndl_v4 == NULL && apnConnP->apn_hndl_v6 == NULL)
        {
          lwm2m_data_encode_int(apnConnP->tot_bytes_sent, dataP);
          return LWM2M_205_CONTENT;
        }
        if(apnConnP->pdn_type == PDN_TYPE_IPv4 || apnConnP->pdn_type == PDN_TYPE_NON_IP
           || (apnConnP->pdn_type == PDN_TYPE_IPv4v6 && apnConnP->apn_hndl_v4 != NULL))
        {
          result = get_packet_stats(apnConnP->apn_hndl_v4, apnConnP->pdn_type, PDN_TYPE_IPv4,
                                    &tx_bytes_v4, &tx_pkts_v4, &rx_bytes_v4);
        }
        if(apnConnP->pdn_type == PDN_TYPE_IPv6 || (apnConnP->pdn_type == PDN_TYPE_IPv4v6 && apnConnP->apn_hndl_v6 != NULL))
        {
          result = get_packet_stats(apnConnP->apn_hndl_v6, apnConnP->pdn_type, PDN_TYPE_IPv6,
                                    &tx_bytes_v6, &tx_pkts_v6, &rx_bytes_v6);
        }

        if(result == QAPI_OK)
        {
          apnConnP->tot_bytes_sent = tx_bytes_v4 + tx_bytes_v6;
          lwm2m_data_encode_int(apnConnP->tot_bytes_sent, dataP);
          return LWM2M_205_CONTENT;
        }
        else
        {
          //lwm2m_data_encode_int(apnConnP->tot_bytes_sent, dataP);
          return LWM2M_500_INTERNAL_SERVER_ERROR;
        }
      }

    case RES_O_TOT_PKTS_SENT:
      {
        qapi_Status_t result = QAPI_ERROR;
        unsigned long long tx_bytes_v4 = 0;
        unsigned long long tx_pkts_v4 = 0;
        unsigned long long rx_bytes_v4 = 0;
        unsigned long long tx_bytes_v6 = 0;
        unsigned long long tx_pkts_v6 = 0;
        unsigned long long rx_bytes_v6 = 0;
        
        if(apnConnP->apn_hndl_v4 == NULL && apnConnP->apn_hndl_v6 == NULL)
        {
          lwm2m_data_encode_int(apnConnP->tot_pkts_sent, dataP);
          return LWM2M_205_CONTENT;
        }

        if(apnConnP->pdn_type == PDN_TYPE_IPv4 || apnConnP->pdn_type == PDN_TYPE_NON_IP
           || (apnConnP->pdn_type == PDN_TYPE_IPv4v6 && apnConnP->apn_hndl_v4 != NULL))
        {
          result = get_packet_stats(apnConnP->apn_hndl_v4, apnConnP->pdn_type, PDN_TYPE_IPv4,
                                    &tx_bytes_v4, &tx_pkts_v4, &rx_bytes_v4);
        }
        if(apnConnP->pdn_type == PDN_TYPE_IPv6 || (apnConnP->pdn_type == PDN_TYPE_IPv4v6 && apnConnP->apn_hndl_v6 != NULL))
        {
          result = get_packet_stats(apnConnP->apn_hndl_v6, apnConnP->pdn_type, PDN_TYPE_IPv6,
                                    &tx_bytes_v6, &tx_pkts_v6, &rx_bytes_v6);
        }

        if(result == QAPI_OK)
        {
          apnConnP->tot_pkts_sent = tx_pkts_v4 + tx_pkts_v6;
          lwm2m_data_encode_int(apnConnP->tot_pkts_sent, dataP);
          return LWM2M_205_CONTENT;
        }
        else
        {
          //lwm2m_data_encode_int(apnConnP->tot_pkts_sent, dataP);
          return LWM2M_500_INTERNAL_SERVER_ERROR;
        }
      }

    case RES_O_TOT_BYTES_RECV:
      {
        qapi_Status_t result = QAPI_ERROR;
        unsigned long long tx_bytes_v4 = 0;
        unsigned long long tx_pkts_v4 = 0;
        unsigned long long rx_bytes_v4 = 0;
        unsigned long long tx_bytes_v6 = 0;
        unsigned long long tx_pkts_v6 = 0;
        unsigned long long rx_bytes_v6 = 0;

        if(apnConnP->apn_hndl_v4 == NULL && apnConnP->apn_hndl_v6 == NULL)
        {
          lwm2m_data_encode_int(apnConnP->tot_bytes_recv, dataP);
          return LWM2M_205_CONTENT;
        }
        if(apnConnP->pdn_type == PDN_TYPE_IPv4 || apnConnP->pdn_type == PDN_TYPE_NON_IP
           || (apnConnP->pdn_type == PDN_TYPE_IPv4v6 && apnConnP->apn_hndl_v4 != NULL))
        {
          result = get_packet_stats(apnConnP->apn_hndl_v4, apnConnP->pdn_type, PDN_TYPE_IPv4,
                                    &tx_bytes_v4, &tx_pkts_v4, &rx_bytes_v4);
        }
        if(apnConnP->pdn_type == PDN_TYPE_IPv6 || (apnConnP->pdn_type == PDN_TYPE_IPv4v6 && apnConnP->apn_hndl_v6 != NULL))
        {
          result = get_packet_stats(apnConnP->apn_hndl_v6, apnConnP->pdn_type, PDN_TYPE_IPv6,
                                    &tx_bytes_v6, &tx_pkts_v6, &rx_bytes_v6);
        }

        if(result == QAPI_OK)
        {
          apnConnP->tot_bytes_recv = rx_bytes_v4 + rx_bytes_v6;
          lwm2m_data_encode_int(apnConnP->tot_bytes_recv, dataP);
          return LWM2M_205_CONTENT;
        }
        else
        {
          //lwm2m_data_encode_int(apnConnP->tot_bytes_recv, dataP);
          return LWM2M_500_INTERNAL_SERVER_ERROR;
        }
      }

    case RES_O_IP_ADDR:
      {
        if (resInstId != INVALID_RES_INST_ID)
        {
          resource_instance_string_list_t *resInst = (resource_instance_string_list_t *)LWM2M_LIST_FIND(apnConnP->ip_addr,resInstId);
          if(resInst != NULL)
          {
            lwm2m_data_encode_string(resInst->InstValue, dataP);
            return LWM2M_205_CONTENT;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          int ri = 0;
          resource_instance_string_list_t* resInst = NULL;
          for(resInst = apnConnP->ip_addr, ri=0;
              resInst !=NULL;
              resInst = resInst->next, ri++);

          if (ri==0)  // no values!
          {
            lwm2m_data_t* subTlvP = NULL;
            lwm2m_data_encode_instances(subTlvP, 0, dataP);
            return LWM2M_205_CONTENT;
          }
          else
          {
            lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
            if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
            for (resInst = apnConnP->ip_addr, ri = 0;
                resInst!= NULL;
                resInst = resInst->next, ri++)
            {
              subTlvP[ri].id = ri;
              lwm2m_data_encode_string(resInst->InstValue, &subTlvP[ri]);
            }

            lwm2m_data_encode_instances(subTlvP, ri, dataP);

            return LWM2M_205_CONTENT;
          }
        } /* resInstId == INVALID_RES_INST_ID */
      }

    case RES_O_PREFIX_LEN:
      {
        if (resInstId != INVALID_RES_INST_ID)
        {
          resource_instance_string_list_t *resInst = (resource_instance_string_list_t *)LWM2M_LIST_FIND(apnConnP->prefix_len,resInstId);
          if(resInst != NULL)
          {
            lwm2m_data_encode_string(resInst->InstValue, dataP);
            return LWM2M_205_CONTENT;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          int ri = 0;
          resource_instance_string_list_t* resInst = NULL;
          for(resInst = apnConnP->prefix_len, ri=0;
              resInst !=NULL;
              resInst = resInst->next, ri++);

          if (ri==0)  // no values!
          {
            lwm2m_data_t* subTlvP = NULL;
            lwm2m_data_encode_instances(subTlvP, 0, dataP);
            return LWM2M_205_CONTENT;
          }
          else
          {
            lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
            if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
            for (resInst = apnConnP->prefix_len, ri = 0;
                resInst!= NULL;
                resInst = resInst->next, ri++)
            {
              subTlvP[ri].id = ri;
              lwm2m_data_encode_string(resInst->InstValue, &subTlvP[ri]);
            }

            lwm2m_data_encode_instances(subTlvP, ri, dataP);

            return LWM2M_205_CONTENT;
          }
        } /* resInstId == INVALID_RES_INST_ID */
      }

    case RES_O_SUBNET_MASK:
      {
        if (resInstId != INVALID_RES_INST_ID)
        {
          resource_instance_string_list_t *resInst = (resource_instance_string_list_t *)LWM2M_LIST_FIND(apnConnP->subnet_mask,resInstId);
          if(resInst != NULL)
          {
            lwm2m_data_encode_string(resInst->InstValue, dataP);
            return LWM2M_205_CONTENT;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          int ri = 0;
          resource_instance_string_list_t* resInst = NULL;
          for(resInst = apnConnP->subnet_mask, ri=0;
              resInst !=NULL;
              resInst = resInst->next, ri++);

          if (ri==0)  // no values!
          {
            lwm2m_data_t* subTlvP = NULL;
            lwm2m_data_encode_instances(subTlvP, 0, dataP);
            return LWM2M_205_CONTENT;
          }
          else
          {
            lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
            if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
            for (resInst = apnConnP->subnet_mask, ri = 0;
                resInst!= NULL;
                resInst = resInst->next, ri++)
            {
              subTlvP[ri].id = ri;
              lwm2m_data_encode_string(resInst->InstValue, &subTlvP[ri]);
            }

            lwm2m_data_encode_instances(subTlvP, ri, dataP);

            return LWM2M_205_CONTENT;
          }
        } /* resInstId == INVALID_RES_INST_ID */
      }

    case RES_O_GATEWAY:
      {
        if (resInstId != INVALID_RES_INST_ID)
        {
          resource_instance_string_list_t *resInst = (resource_instance_string_list_t *)LWM2M_LIST_FIND(apnConnP->gateway,resInstId);
          if(resInst != NULL)
          {
            lwm2m_data_encode_string(resInst->InstValue, dataP);
            return LWM2M_205_CONTENT;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          int ri = 0;
          resource_instance_string_list_t* resInst = NULL;
          for(resInst = apnConnP->gateway, ri=0;
              resInst !=NULL;
              resInst = resInst->next, ri++);

          if (ri==0)  // no values!
          {
            lwm2m_data_t* subTlvP = NULL;
            lwm2m_data_encode_instances(subTlvP, 0, dataP);
            return LWM2M_205_CONTENT;
          }
          else
          {
            lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
            if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
            for (resInst = apnConnP->gateway, ri = 0;
                resInst!= NULL;
                resInst = resInst->next, ri++)
            {
              subTlvP[ri].id = ri;
              lwm2m_data_encode_string(resInst->InstValue, &subTlvP[ri]);
            }

            lwm2m_data_encode_instances(subTlvP, ri, dataP);

            return LWM2M_205_CONTENT;
          }
        } /* resInstId == INVALID_RES_INST_ID */
      }

    case RES_O_PRI_DNS:
      {
        if (resInstId != INVALID_RES_INST_ID)
        {
          resource_instance_string_list_t *resInst = (resource_instance_string_list_t *)LWM2M_LIST_FIND(apnConnP->pri_dns_addr,resInstId);
          if(resInst != NULL)
          {
            lwm2m_data_encode_string(resInst->InstValue, dataP);
            return LWM2M_205_CONTENT;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          int ri = 0;
          resource_instance_string_list_t* resInst = NULL;
          for(resInst = apnConnP->pri_dns_addr, ri=0;
              resInst !=NULL;
              resInst = resInst->next, ri++);

          if (ri==0)  // no values!
          {
            lwm2m_data_t* subTlvP = NULL;
            lwm2m_data_encode_instances(subTlvP, 0, dataP);
            return LWM2M_205_CONTENT;
          }
          else
          {
            lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
            if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
            for (resInst = apnConnP->pri_dns_addr, ri = 0;
                resInst!= NULL;
                resInst = resInst->next, ri++)
            {
              subTlvP[ri].id = ri;
              lwm2m_data_encode_string(resInst->InstValue, &subTlvP[ri]);
            }

            lwm2m_data_encode_instances(subTlvP, ri, dataP);

            return LWM2M_205_CONTENT;
          }
        } /* resInstId == INVALID_RES_INST_ID */
      }

    case RES_O_SEC_DNS:
      {
        if (resInstId != INVALID_RES_INST_ID)
        {
          resource_instance_string_list_t *resInst = (resource_instance_string_list_t *)LWM2M_LIST_FIND(apnConnP->sec_dns_addr,resInstId);
          if(resInst != NULL)
          {
            lwm2m_data_encode_string(resInst->InstValue, dataP);
            return LWM2M_205_CONTENT;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          int ri = 0;
          resource_instance_string_list_t* resInst = NULL;
          for(resInst = apnConnP->sec_dns_addr, ri=0;
              resInst !=NULL;
              resInst = resInst->next, ri++);

          if (ri==0)  // no values!
          {
            lwm2m_data_t* subTlvP = NULL;
            lwm2m_data_encode_instances(subTlvP, 0, dataP);
            return LWM2M_205_CONTENT;
          }
          else
          {
            lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
            if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
            for (resInst = apnConnP->sec_dns_addr, ri = 0;
                resInst!= NULL;
                resInst = resInst->next, ri++)
            {
              subTlvP[ri].id = ri;
              lwm2m_data_encode_string(resInst->InstValue, &subTlvP[ri]);
            }

            lwm2m_data_encode_instances(subTlvP, ri, dataP);

            return LWM2M_205_CONTENT;
          }
        } /* resInstId == INVALID_RES_INST_ID */
      }

    case RES_O_PDN_TYPE:
      lwm2m_data_encode_int(apnConnP->pdn_type, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_VENDOR_EXT:
      lwm2m_data_encode_objLink(apnConnP->vendor_ext, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_APN_CONTROL:
    {
       qapi_Status_t result = QAPI_ERROR;
       qapi_DSS_Apn_Rate_Control_Info_t apn_rate_control_info;

       if(apnConnP->apn_hndl_v4 == NULL && apnConnP->apn_hndl_v6 == NULL)
       {
         apnConnP->apn_rate_control = -1;
         lwm2m_data_encode_int(apnConnP->apn_rate_control, dataP);
         return LWM2M_205_CONTENT;
       }

       memset(&apn_rate_control_info, 0x00, sizeof(qapi_DSS_Apn_Rate_Control_Info_t));
       if(apnConnP->pdn_type == PDN_TYPE_IPv4 || apnConnP->pdn_type == PDN_TYPE_NON_IP
           || (apnConnP->pdn_type == PDN_TYPE_IPv4v6 && apnConnP->apn_hndl_v4 != NULL))
       {
          result = get_apn_rate_control(apnConnP->apn_hndl_v4, &apn_rate_control_info);
       }
       if((result !=QAPI_OK) && (apnConnP->pdn_type == PDN_TYPE_IPv6 || (apnConnP->pdn_type == PDN_TYPE_IPv4v6 && apnConnP->apn_hndl_v6 != NULL)))
       {
          result = get_apn_rate_control(apnConnP->apn_hndl_v6, &apn_rate_control_info);
       }

       if(result == QAPI_OK)
       {
          apnConnP->apn_rate_control = apn_rate_control_info.max_uplink_rate;
          lwm2m_data_encode_int(apnConnP->apn_rate_control, dataP);
          return LWM2M_205_CONTENT;
       }
       else if(result == QAPI_ERR_NOT_SUPPORTED)
       {
          apnConnP->apn_rate_control = -1;
          lwm2m_data_encode_int(apnConnP->apn_rate_control, dataP);
          return LWM2M_205_CONTENT;
       }
       else
       {
         return LWM2M_500_INTERNAL_SERVER_ERROR;
       }
    }

    case RES_O_QCI:  // TODO check support
    case RES_O_VALIDITY:
      return LWM2M_405_METHOD_NOT_ALLOWED;
    default:
      return LWM2M_404_NOT_FOUND;
  }
}

static uint8_t apn_info_instance_read(apn_conn_profile_instance_t *instanceP,
                                      lwm2m_data_t ** dataArrayP,
                                      int * numDataP)
{
  int i=0, nbRes = 0;
  uint16_t resList[] = {
    RES_O_ENABLE_STATUS,
    RES_O_TOT_BYTES_SENT,
    RES_O_TOT_PKTS_SENT,
    RES_O_TOT_BYTES_RECV,
  //RES_O_QCI,// TODO check support
    RES_O_VENDOR_EXT,
    RES_O_APN_CONTROL,
    RES_M_PROFILE_NAME,
    RES_O_APN,
    RES_O_AUTO_SELECT_APN,
    RES_M_AUTH_TYPE,
    RES_O_USER_NAME,
    RES_O_SECRET,
    RES_O_RECONN_SCHEDULE,
    RES_O_PDN_TYPE,
    RES_O_CONN_ESTB_TIME,
    RES_O_CONN_ESTB_RESULT,
    RES_O_CONN_ESTB_REJECT_CAUSE,
    RES_O_CONN_END_TIME,
    RES_O_IP_ADDR,
    RES_O_PREFIX_LEN,
    RES_O_SUBNET_MASK,
    RES_O_GATEWAY,
    RES_O_PRI_DNS, 
    RES_O_SEC_DNS
  };
  if(instanceP == NULL || dataArrayP == NULL || numDataP == NULL)
  {
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  nbRes = sizeof(resList)/sizeof(uint16_t);

  *dataArrayP = lwm2m_data_new(nbRes);
  if (*dataArrayP == NULL)
  {
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  *numDataP = nbRes;
  for (i = 0 ; i < nbRes ; i++)
  {
    (*dataArrayP)[i].id = resList[i];
  }

  return QAPI_OK;
}

/**
 * @brief This function implements the read operation performed on APN connection
 * profile object
 * @param instanceId (IN) Instance ID of the object to be read
 * @param numDataP (OUT) number of resources in the array
 * @param dataArrayP (OUT) resource value array
 * @param objectP (IN) Object to be read
 * @param resInstId (IN)  Resource instance id or INVALID_RES_INST_ID 
 *                        if resource instance id is not passed.
 * @return LWM2M response type
 */ 
static uint8_t prv_apn_conn_read(uint16_t instanceId,
                                 int * numDataP,
                                 lwm2m_data_t ** dataArrayP,
                                 lwm2m_object_t * objectP,
                                 uint16_t resInstId, lwm2m_server_t *serverP)
{
  uint8_t result = LWM2M_404_NOT_FOUND;
  int i = 0;
  apn_conn_profile_instance_t *instanceP = NULL;

  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  instanceP = (apn_conn_profile_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, 
                                                             instanceId);
  if (NULL == instanceP)
  {
    return result;
  }

  if(*numDataP == 0)
  {
    result = apn_info_instance_read(instanceP, dataArrayP, numDataP);
    if(result != QAPI_OK)
    {
      return result;
    }
  }

  i = 0;
  do
  {
    ds_iot_ulog_high1("APN Connection profile READ ON  Resource ID %d ",((*dataArrayP)+i)->id);  
    result = prv_set_value((*dataArrayP)+i, instanceP, resInstId);
    i++;
  }while (i < *numDataP && result == LWM2M_205_CONTENT);

  return result;
}

/**
 * @fn static uint8_t prv_apn_conn_discover()
 *
 * @brief This function implements the discover operation performed on the
 *       APN profile connection object.
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_apn_conn_discover(uint16_t instanceId,
                                     int * numDataP,
                                     lwm2m_data_t ** dataArrayP,
                                     lwm2m_object_t * objectP, lwm2m_server_t *serverP)
{
  uint8_t result = LWM2M_205_CONTENT;
  apn_conn_profile_instance_t *targetP = NULL;
  int i = 0;

  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  targetP = (apn_conn_profile_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);

  if (NULL == targetP)
  {
    return LWM2M_404_NOT_FOUND;
  }

  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
  uint16_t resList[] = {
      RES_O_ENABLE_STATUS,
      RES_O_CONN_ESTB_TIME,
      RES_O_CONN_ESTB_RESULT,
      RES_O_CONN_ESTB_REJECT_CAUSE,
      RES_O_CONN_END_TIME,
      RES_O_TOT_BYTES_SENT,
      RES_O_TOT_PKTS_SENT,
      RES_O_TOT_BYTES_RECV,
      RES_O_IP_ADDR,
      RES_O_PREFIX_LEN,
      RES_O_SUBNET_MASK,
      RES_O_GATEWAY,
      RES_O_PRI_DNS,
      RES_O_SEC_DNS,
      //RES_O_QCI,        // TODO check support
      RES_O_VENDOR_EXT,
      RES_O_APN_CONTROL,
      RES_M_PROFILE_NAME,
      RES_O_APN,
      RES_O_AUTO_SELECT_APN,
      RES_M_AUTH_TYPE,
      RES_O_USER_NAME,
      RES_O_SECRET,
      RES_O_RECONN_SCHEDULE,
      RES_O_PDN_TYPE,
    };
    int nbRes = sizeof(resList) / sizeof(uint16_t);

   *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL) 
      return LWM2M_500_INTERNAL_SERVER_ERROR;

   *numDataP = nbRes;

    for (i = 0 ; i < nbRes ; i++)
    {
      (*dataArrayP)[i].id = resList[i];
    }
  }

  for (i = 0; i < *numDataP && result == LWM2M_205_CONTENT; i++)
  {
    switch ((*dataArrayP)[i].id)
    {
      case RES_O_CONN_ESTB_TIME:
      {
        uint16_t cntr = 0;
        resource_instance_conn_list_t* resInst = targetP->connection_stats;

        while (resInst)
        {
          if(resInst->conn_estblsh_time_isValid)
          {
            cntr++;
          }
          resInst = resInst->next;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = cntr;
      }
      break;

      case RES_O_CONN_ESTB_RESULT:
      {
        uint16_t cntr = 0;
        resource_instance_conn_list_t* resInst = targetP->connection_stats;

        while (resInst)
        {
          if(resInst->conn_estblsh_result_isValid)
          {
            cntr++;
          }
          resInst = resInst->next;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = cntr;
      }
      break;

      case RES_O_CONN_ESTB_REJECT_CAUSE:
      {
        uint16_t cntr = 0;
        resource_instance_conn_list_t* resInst = targetP->connection_stats;

        while (resInst)
        {
          if(resInst->conn_reject_cause_isValid)
          {
            cntr++;
          }
          resInst = resInst->next;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = cntr;
      }
      break;

      case RES_O_CONN_END_TIME:
      {
        uint16_t cntr = 0;
        resource_instance_conn_list_t* resInst = targetP->connection_stats;

        while (resInst)
        {
          if(resInst->conn_end_time_isValid)
          {
            cntr++;
          }
          resInst = resInst->next;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = cntr;
      }
      break;

      case RES_O_IP_ADDR:
      {
        uint16_t cntr = 0;
        resource_instance_string_list_t* resInst = targetP->ip_addr;

        while (resInst)
        {
          resInst = resInst->next;
          cntr++;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = cntr;
      }
      break;

      case RES_O_PREFIX_LEN:
      {
        uint16_t cntr = 0;
        resource_instance_string_list_t* resInst = targetP->prefix_len;

        while (resInst)
        {
          resInst = resInst->next;
          cntr++;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = cntr;
      }
      break;

      case RES_O_SUBNET_MASK:
      {
        uint16_t cntr = 0;
        resource_instance_string_list_t* resInst = targetP->subnet_mask;

        while (resInst)
        {
          resInst = resInst->next;
          cntr++;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = cntr;
      }
      break;

      case RES_O_GATEWAY:
      {
        uint16_t cntr = 0;
        resource_instance_string_list_t* resInst = targetP->gateway;

        while (resInst)
        {
          resInst = resInst->next;
          cntr++;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = cntr;
      }
      break;

      case RES_O_PRI_DNS:
      {
        uint16_t cntr = 0;
        resource_instance_string_list_t* resInst = targetP->pri_dns_addr;

        while (resInst)
        {
          resInst = resInst->next;
          cntr++;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = cntr;
      }
      break;

      case RES_O_SEC_DNS:
      {
        uint16_t cntr = 0;
        resource_instance_string_list_t* resInst = targetP->sec_dns_addr;

        while (resInst)
        {
          resInst = resInst->next;
          cntr++;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = cntr;
      }

      case RES_O_ENABLE_STATUS:
      case RES_O_TOT_BYTES_SENT:
      case RES_O_TOT_PKTS_SENT:
      case RES_O_TOT_BYTES_RECV:
      //case RES_O_QCI:
      case RES_O_VENDOR_EXT:
      case RES_O_APN_CONTROL:
      case RES_M_PROFILE_NAME:
      case RES_O_APN:
      case RES_O_AUTO_SELECT_APN:
      case RES_M_AUTH_TYPE:
      case RES_O_USER_NAME:
      case RES_O_SECRET:
      case RES_O_RECONN_SCHEDULE:
      case RES_O_PDN_TYPE:
      break;
      default:
        result = LWM2M_404_NOT_FOUND;
    }
  }

  return result;
}

/**
 * @brief This function sets default values for APN connection
 * profile object instance 
 * @param instanceP (IN/OUT) Instance whose values to be set.
 * @return none
 */ 
void prv_apn_conn_default_values(apn_conn_profile_instance_t *instanceP)
{
  if(instanceP == NULL)
  {
    LOG_DEBUG("%s",nullError);
    return;
  }

  LWM2M_STRCPY(instanceP->profile_name, DEFAULT_PROFILE_NAME, 
               strlen(DEFAULT_PROFILE_NAME) + 1);
  instanceP->apn_name_valid = FALSE;
  LWM2M_STRCPY(instanceP->apn_name, DEFAULT_APN_NAME, 
               strlen(DEFAULT_APN_NAME) + 1);
  instanceP->auto_apn = FALSE;
  instanceP->enable_status = TRUE;
  instanceP->auth_type = APN_AUTH_PREF_PAP_CHAP_NOT_ALLOWED;
  LWM2M_STRCPY(instanceP->username, DEFAULT_USER_NAME, 
               strlen(DEFAULT_USER_NAME) + 1);
  LWM2M_STRCPY(instanceP->secret, DEFAULT_SECRET, 
               strlen(DEFAULT_SECRET) + 1);
  LWM2M_STRCPY(instanceP->reconn_schedule, DEFAULT_RECONNECT, 
               strlen(DEFAULT_RECONNECT) + 1);
  instanceP->tot_pkts_sent = 0;
  instanceP->tot_bytes_sent = 0;
  instanceP->tot_bytes_recv = 0;
  instanceP->vendor_ext = 0xFFFFFFFF;
  instanceP->params_set = 0;
}

#if 0
/**
 * @brief This function implements the execute operation performed on APN 
 * connection profile object.
 * @param instanceId (IN) Instance ID of the object to be read
 * @param resourceId (IN) Resource ID of the resource to be executed
 * @param buffer (IN) message payload from server fro execute operation
 * @param length (IN) message payload length
 * @param objectP Object on which the operation needs to be executed
 * @return LWM2M response type
 */ 
static uint8_t prv_apn_conn_execute(uint16_t instanceId,
                                    uint16_t resourceId,
                                    uint8_t * buffer,
                                    int length,
                                    lwm2m_object_t * objectP)
{
  apn_conn_profile_instance_t *instanceP = NULL;

  if ( objectP == NULL )
  {
    LOG_ERROR("%s",nullError);
    return LWM2M_400_BAD_REQUEST;
  }

  instanceP = (apn_conn_profile_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, instanceId);

  if (NULL == instanceP)
  {
    return LWM2M_404_NOT_FOUND;
  }

  switch(resourceId)
  {
    case RES_O_ENABLE_STATUS:
    case RES_O_VALIDITY:
    case RES_O_CONN_ESTB_TIME:
    case RES_O_CONN_ESTB_RESULT:
    case RES_O_CONN_ESTB_REJECT_CAUSE:
    case RES_O_CONN_END_TIME:
    case RES_O_TOT_BYTES_SENT:
    case RES_O_TOT_PKTS_SENT:
    case RES_O_TOT_BYTES_RECV:
    case RES_O_IP_ADDR:
    case RES_O_PREFIX_LEN:
    case RES_O_SUBNET_MASK:
    case RES_O_GATEWAY:
    case RES_O_PRI_DNS:
    case RES_O_SEC_DNS:
    case RES_O_QCI:
    case RES_O_VENDOR_EXT:
    case RES_O_APN_CONTROL:
    case RES_M_PROFILE_NAME:
    case RES_O_APN:
    case RES_O_AUTO_SELECT_APN:
    case RES_M_AUTH_TYPE:
    case RES_O_USER_NAME:
    case RES_O_SECRET:
    case RES_O_RECONN_SCHEDULE:
    case RES_O_PDN_TYPE:
    return LWM2M_405_METHOD_NOT_ALLOWED;
    default:
      return LWM2M_404_NOT_FOUND;
  }
}
#endif
/**
 * @fn static uint8_t prv_apn_conn_create()
 * @brief   This function implements the create operation performed
 * on the APN connection profile object
 * @param   instanceId (IN) Instance ID of the object instance to be created
 * @param   numData (IN)    number of resources in the array i.e. dataArray
 * @param   dataArray (IN)  resource value array
 * @param   objectP (IN)    Object whose instance is to be created
 * return   COAP response type
 */

/***************************************************************************************************

 Under section 8.2.3 Operation on Object of OMA 1.1

- If the "Create" operation is permitted, the LwM2M Client MUST perform the instantiation on the 
Object only if all the mandatory Resources are present in the "New Value" parameter (see Section 5). 
If all the mandatory Resources are not present, the LwM2M Client MUST send a "Bad Request" error code 
to the LwM2M Server


*************************************************************************************************************/

static uint8_t prv_apn_conn_create(uint16_t instanceId,
                                   int numData,
                                   lwm2m_data_t* dataArray,
                                   lwm2m_object_t* objectP, bool apiCall)
{
  apn_conn_profile_instance_t *instanceP = NULL;
  uint8_t result = LWM2M_500_INTERNAL_SERVER_ERROR;
  uint16_t mandatory_resource[] = {RES_M_PROFILE_NAME, RES_M_AUTH_TYPE};
  uint8_t size = sizeof(mandatory_resource)/sizeof(mandatory_resource[0]);
  if(dataArray == NULL || objectP == NULL )
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  if(apiCall == TRUE)
  {
     goto skip_check;
  }

  result = prv_mandatory_check(numData, dataArray, mandatory_resource, size);
  if(result != LWM2M_NO_ERROR)
    return result;
  /* Check all resources to be written are writable or not */
  result = prv_apn_conn_create_write_check(numData, dataArray, TRUE);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err ("Creating instance failed ");
    return result;
  }

skip_check:
  instanceP = (apn_conn_profile_instance_t *)lwm2m_malloc(sizeof(apn_conn_profile_instance_t));

  if (instanceP != NULL)
  {
    memset(instanceP, 0, sizeof(apn_conn_profile_instance_t));
    instanceP->instanceId = instanceId;
    /* Add instance into object instance list */
    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, instanceP);

    /* Assign default values to all resources */
    prv_apn_conn_default_values(instanceP);

    create_operation = TRUE;
    /* Write the resource values */
    result = prv_apn_conn_write_resources(objectP, instanceP, numData, dataArray, 
                                          INVALID_RES_INST_ID, QAPI_COAP_POST, CREATE_REQUEST);

    create_operation = FALSE;
    if (result == LWM2M_204_CHANGED && instanceP->apn_name_valid == TRUE)
    {
      //result = LWM2M_201_CREATED;
      result = LWM2M_NO_ERROR;
    }
    else if(result == LWM2M_204_CHANGED)
    {
      result = LWM2M_201_CREATED;
    }
    else
    {
      /* Delete the instance in case of failure */
      prv_apn_conn_delete(instanceId, objectP,false); 
      // Issue in creating object instance so retuning Bad request error.
      result = LWM2M_400_BAD_REQUEST;
    }
  }
  return result;

}

/*
 * @fn      static int32_t prv_apn_conn_getvalue ()
 * @brief   This function is used to get the particular resource values for given instance 
 * @param   id_info      - indicates what values need to be filled whether resource or instance or object level 
 * @param   lwm2m_dataP  - pointer to lwm2m object information to be filled 
 * @param   targetP      - pointer to lwm2m object 
 * @return  on success  -  0
            on failure   - -1
 */

int32_t prv_apn_conn_getvalue(lwm2m_id_info_t id_info, lwm2m_object_data_t **lwm2m_dataP ,
                              lwm2m_object_t * targetP, lwm2m_resource_info_t **resource_info)
{
  int8_t result = -1;
  lwm2m_data_t *dataArrayP = NULL;
  //lwm2m_resource_info_t *resource_info = NULL;
  int numData = 0;
  uint16_t instance_id = 0;

  if(lwm2m_dataP == NULL)
    return -1;

  if((result = prv_init_getvalue(id_info,*lwm2m_dataP, resource_info, &dataArrayP, &numData))==-1)
    return result;
  instance_id = id_info.instance_ID;
  result = prv_apn_conn_read(instance_id, &numData, &dataArrayP, targetP,INVALID_RES_INST_ID, NULL);

  if (result != LWM2M_205_CONTENT)
  {
    result = -1;
    goto GET_ERROR;
  }
  if((result = prv_getvalue_resource_populate(lwm2m_dataP, dataArrayP, resource_info, numData))==-1)
    goto GET_ERROR;
  result = 0;

GET_ERROR:
  /* Resources allocated for lwm2m_dataP are cleaned-up by the caller */
  if (dataArrayP)
    lwm2m_free(dataArrayP);

  return result;
}

/*
 * @fn      static int32_t prv_apn_conn_setvalue()
 * @brief   This function is used to set the particular resource value from application
 * @param   lwm2m_data - pointer to lwm2m object information
 * @param    targetP   - pointer to lwm2m object 
 * @return  on success - 1
            on error   - 0
 */
static int32_t prv_apn_conn_setvalue(lwm2m_object_data_t *lwm2m_data, lwm2m_object_t *targetP)
{
  uint8_t res = LWM2M_204_CHANGED;
  apn_conn_profile_instance_t* instanceP = NULL;
  lwm2m_data_t *dataArray = NULL;
  bool enable_status_new = TRUE, enable_status_old = TRUE;

  if(lwm2m_data == NULL || targetP == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }

  if((lwm2m_data->no_instances != 0x01)|| 
         (!lwm2m_data->instance_info) ||
         ((lwm2m_data->instance_info->no_resources) &&
         (!lwm2m_data->instance_info->resource_info)))
    return 0;
  
  instanceP = (apn_conn_profile_instance_t *)LWM2M_LIST_FIND(targetP->instanceList, lwm2m_data->instance_info->instance_ID);

  if(NULL == instanceP)
  {
    return 0;
  }

  if(QAPI_OK != lwm2m_parse_application_object_info(lwm2m_data->instance_info,
                                                    lwm2m_data->instance_info->resource_info,
                                                    &dataArray))
  {
    return 0;
  }

  enable_status_old = instanceP->enable_status;

  res = prv_apn_conn_write_resources(targetP, instanceP, 1, 
                                     dataArray,
                                     INVALID_RES_INST_ID, QAPI_COAP_PUT, SET_REQUEST);

  enable_status_new = instanceP->enable_status;
  
  lwm2m_data_free_ext(lwm2m_data->instance_info->no_resources,dataArray);

  if(enable_status_old != enable_status_new)
  {
    carrier_apn_cfg_t *apn_info = NULL;
    lwm2m_server_t *server_list = NULL;
    uint16_t instance_id = 0xFFFF;
    apn_info = get_lwm2m_apn_cfg();
    while(apn_info)
    {
      instance_id = (apn_info->apn_link & 0xFFFF);
      if(instance_id == instanceP->instanceId)
      {
        server_list = (lwm2m_server_t *)LWM2M_LIST_FIND(data.lwm2mH->serverList, apn_info->short_server_id);
        if(server_list)
        {
          LWM2M_SERVER_SET_UPDATE_PROFILE_CHANGE(server_list->update_disable_check);
        }
      }
      apn_info = apn_info->next;
    }
  }
  
  if(res != LWM2M_204_CHANGED)
  {
    return 0;
  }
  else
  {
    resource_value_change(lwm2m_data);
    return 1;
  }
}

/**
 * @brief This function is used to get the information regarding the APN 
 * connection profile object during client registration.
 * @param void
 * @return APN connection profile object 
 */ 
lwm2m_object_t * get_object_apn_conn()
{
  lwm2m_object_t * apnConnObj = NULL;

  apnConnObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

  if(NULL != apnConnObj)
  {
    memset(apnConnObj, 0, sizeof(lwm2m_object_t));

    /* It assigns his unique ID. The 11 is the standard ID for the mandatory 
       object "APN Connection Profile".*/
    apnConnObj->objID = LWM2M_APN_CONN_PROFILE_OBJECT_ID;

    /*
     * And the private function that will access the object.
     * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
     * know the resources of the object, only the server does.
     */
    apnConnObj->readFunc     = prv_apn_conn_read;
    apnConnObj->discoverFunc = prv_apn_conn_discover;
    apnConnObj->writeFunc    = prv_apn_conn_write;
    apnConnObj->executeFunc  = NULL;
    apnConnObj->createFunc   = prv_apn_conn_create;
    apnConnObj->deleteFunc   = prv_apn_conn_delete;
    apnConnObj->get_Value_Func = prv_apn_conn_getvalue;
    apnConnObj->setValueFunc   = prv_apn_conn_setvalue;
  }

  return apnConnObj;
}

/**
 * @brief This function frees the APN connection profile object allocated
 * @param ObjectP (IN) Object to free
 * @return void
 */ 
void clean_apn_conn_object(lwm2m_object_t * objectP)
{
  apn_conn_profile_instance_t *instances = NULL;

  if(objectP == NULL)
  {
    LOG_DEBUG("%s objectP",nullError);
    return;
  }

  instances = (apn_conn_profile_instance_t *)objectP->instanceList;

  while(NULL != instances)
  {
	/* don't update persistent file */
    prv_apn_conn_delete(instances->instanceId, objectP,false);
    instances = (apn_conn_profile_instance_t *)objectP->instanceList;
  }

  objectP->instanceList = NULL;

}

/**
* @brief  API to retrieve parameters required for Data Call bring up.
*
* @param[in]  value   Object link resource value (Object_id:Instance_id)
* @param[out] params  Data call input parameters.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK (0) is returned. Other value on error.
*/
int8_t get_apn_conn_params(objlink_t value, 
                           apn_conn_input_params_t *params)
{
  int8_t result = QAPI_ERROR;
  lwm2m_object_t *apn_object_list = NULL;
  apn_conn_profile_instance_t *instanceP = NULL;
  char *num = NULL;
  uint16_t obj_id = 0;
  uint16_t instance_id = 0;
  int i =0;

  if(params == NULL)
  {
    LOG_DEBUG("%s params",nullError);
    return QAPI_ERR_INVALID_PARAM;
  }

  /* Parse object id and instance id. */
  obj_id  = value >> 16;
  instance_id = value & 0xFFFF;

  if(obj_id != LWM2M_APN_CONN_PROFILE_OBJECT_ID)
  {
    LOG_DEBUG("QAPI_Invalid input parameters ");
    return QAPI_ERR_INVALID_PARAM;
  }

  if(data.lwm2mH == NULL)
  {
    LOG_DEBUG("%s LwM2M ctxt",nullError);
    return QAPI_ERR_INVALID_PARAM;
  }

  apn_object_list = (lwm2m_object_t *)LWM2M_LIST_FIND(data.lwm2mH->objectList, LWM2M_APN_CONN_PROFILE_OBJECT_ID);
  if (apn_object_list == NULL)
  {
    ds_iot_ulog_err("QAPI_Object 11 not found.");
    result = QAPI_ERR_NO_ENTRY;
    return result;
  }

  instanceP = (apn_conn_profile_instance_t *)LWM2M_LIST_FIND(apn_object_list->instanceList, 
                                                             instance_id);
  if (NULL == instanceP)
  {
    ds_iot_ulog_err1("QAPI_Instance id %d not found ",instance_id);
    result = QAPI_ERR_NO_ENTRY;
    return result;
  }

  /* Scenario 1 "APN name" Set "Auto enable" True ----> Use Object 11 info.
     Scenario 2 "APN name" Set "Auto enable" False ----> Use Object 11 info.
     Scenario 3 "APN name" Unset "Auto enable" True ----> Use legacy info
     Scenario 4 "APN name" Unset "Auto enable" False ----> Use legacy info */
  if(instanceP->apn_name_valid == FALSE )
  {
    ds_iot_ulog_high("QAPI_No APN name configured. Use pre configured values.");
    /* Returning this error so that call management will use legacy APN info to
       bring up call */
    return QAPI_ERR_NO_ENTRY;
  }

  /* Update parameters set for the resources whose values are updated from server
     or application */
  if(instanceP->params_set & APN_NAME_SET)
  {
    LWM2M_STRCPY(params->apn_name , instanceP->apn_name, strlen(instanceP->apn_name) + 1);
  }
  if(instanceP->params_set & AUTH_TYPE_SET)
  {
    params->auth_type = (qapi_DSS_Auth_Pref_t)((instanceP->auth_type + 1 ) % 4);
  }
  if(instanceP->params_set & USER_NAME_SET)
  {
    LWM2M_STRCPY(params->username, instanceP->username, strlen(instanceP->username) + 1);
  }
  if(instanceP->params_set & SECRET_SET)
  {
    LWM2M_STRCPY(params->secret, instanceP->secret, strlen(instanceP->secret) + 1);
  }

  if(instanceP->params_set & RECONNECT_SET)
  {
    /* Convert string "10,60,600,3600,86400" into 10 60 600 3600 86400 */
    char *reconn_schedule = NULL;
    char *saveptr = NULL;

    reconn_schedule = malloc(sizeof(instanceP->reconn_schedule));
    if(reconn_schedule == NULL)
    return result;
    memset(reconn_schedule, 0x00, sizeof(instanceP->reconn_schedule));

    memcpy(reconn_schedule, instanceP->reconn_schedule, sizeof(instanceP->reconn_schedule));

    num = strtok_r(reconn_schedule, ",", &saveptr);
    while(num)
    {
      i++;
      num = strtok_r(NULL, ",", &saveptr);
    }
    params->reconn_count = i;

    if(i == 0) 
    {
      free(reconn_schedule);
      return result;
    }

    params->reconn_schedule = (uint32_t *)malloc(i * sizeof(uint32_t));
    if(params->reconn_schedule == NULL)
    {
      free(reconn_schedule);
      return result;
    }
    memset(params->reconn_schedule, 0x00, i * sizeof(uint32_t));
    memset(reconn_schedule, 0x00, sizeof(instanceP->reconn_schedule));

    memcpy(reconn_schedule, instanceP->reconn_schedule, sizeof(instanceP->reconn_schedule));
    i = 0;
    num = strtok_r(reconn_schedule, ",", &saveptr);
    while(num)
    {
      params->reconn_schedule[i++]=atoi(num);
      num = strtok_r(NULL, ",", &saveptr);
    }
    free(reconn_schedule);
  }

  if (instanceP->params_set & PDN_TYPE_SET)
  {
    if (0x00 == instanceP->pdn_type)
      params->pdn_type = AF_NONIP;    
    else if (0x01 == instanceP->pdn_type)
      params->pdn_type = AF_INET;
    else if (0x02 == instanceP->pdn_type)
      params->pdn_type = AF_INET6;
    else if (0x03 == instanceP->pdn_type)
      params->pdn_type = AF_INET_DUAL46;
  }

  params->params_set = instanceP->params_set;

  ds_iot_ulog_high("QAPI_Input params passed successfully.");
  return QAPI_OK;
}

void apn_info_register_util(apn_conn_profile_instance_t *instance_list, 
                                 int family, void *hndl, uint16_t instance_id)
{
  apn_conn_profile_instance_t *instanceP = NULL;

  instanceP = instance_list;
  while(instanceP != NULL)
  {
    if(family == APN_PDN_TYPE_IPv4 || family == APN_PDN_TYPE_NON_IP)
    {
      if(instanceP->apn_hndl_v4 == hndl)
      {
        /* Entry found corresponding to handle */
        break;
      }
    }
    else if(family == APN_PDN_TYPE_IPv6)
    {
      if(instanceP->apn_hndl_v6 == hndl)
      {
        /* Entry found corresponding to handle */
        break;
      }
    }
    instanceP = instanceP->next;
  }
  
  if(instanceP != NULL)
  {
    if(instanceP->instanceId != instance_id)
    {
      apn_conn_profile_instance_t *instance = NULL;
      instance = (apn_conn_profile_instance_t *)LWM2M_LIST_FIND(instance_list, 
                                                                instance_id);
      if (NULL == instance)
      {
        ds_iot_ulog_err1("QAPI_Instance id %d not found ",instance_id);
        return;
      }
      else
      {
        apn_info_event_notify(hndl, 0xFFFFFFFF, LWM2M_DATA_CALL_DEREGISTERED, family);
        if(family == APN_PDN_TYPE_IPv4 || family == APN_PDN_TYPE_NON_IP)
        {
          instance->apn_hndl_v4 = hndl;  // Link associated with handle updated.
          instanceP->apn_hndl_v4 = NULL;  // Mark old instance  handle as NULL
        }
        else if(family == APN_PDN_TYPE_IPv6 )
        {
          instance->apn_hndl_v6 = hndl;  // Link associated with handle updated.
          instanceP->apn_hndl_v6 = NULL;  // Mark old instance  handle as NULL
        }
      }
    }
    else
    {
      apn_info_event_notify(hndl, 0xFFFFFFFF, LWM2M_DATA_CALL_DEREGISTERED, family);
      if(family == APN_PDN_TYPE_IPv4 || family == APN_PDN_TYPE_NON_IP)
      {
        instanceP->apn_hndl_v4 = hndl;  // Handle associated with instance updated.
      }
      else if(family == APN_PDN_TYPE_IPv6 )
      {
        instanceP->apn_hndl_v6 = hndl;  // Handle associated with instance updated.
      }
    }
  }
  else
  {
    instanceP = (apn_conn_profile_instance_t *)LWM2M_LIST_FIND(instance_list, 
                                                               instance_id);
    if (NULL == instanceP)
    {
      ds_iot_ulog_err1("QAPI_Instance id %d not found ",instance_id);
      return;
    }
    if(family == APN_PDN_TYPE_IPv4 || family == APN_PDN_TYPE_NON_IP)
    {
      instanceP->apn_hndl_v4 = hndl;     // Data call registration handle
    }
    else if(family == APN_PDN_TYPE_IPv6 )
    {
      instanceP->apn_hndl_v6 = hndl;     // Data call registration handle
    }
  }
}

/**
* @brief  API for receiving different data call management events.
*
* @param[in]  handle  Handle will be valid only for register and deregister event.
* @param[in]  value   Object link resource value (Object_id:Instance_id)
* @param[in]  event   Call management event.
* @param[in]  family  IP type (IPv4 / IPv6 ) or Non-IP
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK (0) is returned. Other value on error.
*/
void apn_info_event_notify(void *hndl, objlink_t value,
                           lwm2m_data_call_event_t event, int family)
{
  lwm2m_object_t *apn_object_list = NULL;
  apn_conn_profile_instance_t *instanceP = NULL;
  uint16_t instance_id = 0;
  bool val_change = false;

  if(hndl == NULL)
  {
    return;
  }

  /* Parse object id and instance id. */
  if(value != 0xFFFF)
  {
    instance_id = value & 0xFFFF;
  }

  if(data.lwm2mH == NULL)
  {
    LOG_DEBUG("%s LwM2M ctxt",nullError);
    return ;
  }

  apn_object_list = (lwm2m_object_t *)LWM2M_LIST_FIND(data.lwm2mH->objectList, LWM2M_APN_CONN_PROFILE_OBJECT_ID);
  if (apn_object_list == NULL || apn_object_list->instanceList == NULL)
  {
    ds_iot_ulog_err("QAPI_Object 11 not found.");
    return;
  }
  
  ds_iot_ulog_high3("Handle [0x%x] event %d, family %d ",hndl,event,family);
  if(family == APN_PDN_TYPE_NON_IP || family == APN_PDN_TYPE_IPv4)
  {
    instanceP = (apn_conn_profile_instance_t *)apn_object_list->instanceList;
    while(instanceP != NULL)
    {
      if(instanceP->apn_hndl_v4 == hndl)
      {
        /* Entry found corresponding to handle */
        break;
      }
      instanceP = instanceP->next;
    }
  }
  else if(family == APN_PDN_TYPE_IPv6)
  {
    instanceP = (apn_conn_profile_instance_t *)apn_object_list->instanceList;
    while(instanceP != NULL)
    {
      if(instanceP->apn_hndl_v6 == hndl)
      {
        /* Entry found corresponding to handle */
        break;
      }
      instanceP = instanceP->next;
    }
  }

  if((event != LWM2M_DATA_CALL_REGISTERED) && (NULL == instanceP))
  {
    ds_iot_ulog_err("QAPI_Instance id not found ");
    return;
  }

  switch(event)
  {
    case LWM2M_DATA_CALL_START:
      {
        ds_iot_ulog_high("QAPI_Data call attempted .");

        prv_write_resource_conn_val(&instanceP->connection_stats, family, lwm2m_gettime()+GPS_TO_UNIX_TIME_OFFSET);

      }
    break;

   case LWM2M_DATA_CALL_STOP:
      {
        uint16_t res_id = 0xFFFF;
        qapi_Status_t result = QAPI_ERROR;
        resource_instance_conn_list_t *conn_list = instanceP->connection_stats;
        unsigned long long tx_bytes_v4 = 0;
        unsigned long long tx_pkts_v4 = 0;
        unsigned long long rx_bytes_v4 = 0;
        unsigned long long tx_bytes_v6 = 0;
        unsigned long long tx_pkts_v6 = 0;
        unsigned long long rx_bytes_v6 = 0;

        ds_iot_ulog_high("QAPI_Data call stopped .");
        if(family == APN_PDN_TYPE_NON_IP || family == APN_PDN_TYPE_IPv4)
        {
          res_id = 0;
        }

        else if(family == APN_PDN_TYPE_IPv6)
        {
          res_id = 1;
        }

        while(conn_list)
        {
          if(conn_list->family == family)
          {
            conn_list->conn_end_time_isValid = LWM2M_TRUE;
            conn_list->conn_end_time         = lwm2m_gettime()+GPS_TO_UNIX_TIME_OFFSET;
            break;
          }
          conn_list = conn_list->next;
        }

        if(instanceP->pdn_type == PDN_TYPE_IPv4 || instanceP->pdn_type == PDN_TYPE_NON_IP
           || (instanceP->pdn_type == PDN_TYPE_IPv4v6 && instanceP->apn_hndl_v4 != NULL))
        {
          result = get_packet_stats(instanceP->apn_hndl_v4, instanceP->pdn_type, PDN_TYPE_IPv4,
                                    &tx_bytes_v4, &tx_pkts_v4, &rx_bytes_v4);
        }
        if(instanceP->pdn_type == PDN_TYPE_IPv6 || (instanceP->pdn_type == PDN_TYPE_IPv4v6 && instanceP->apn_hndl_v6 != NULL))
        {
          result = get_packet_stats(instanceP->apn_hndl_v6, instanceP->pdn_type, PDN_TYPE_IPv6,
                                    &tx_bytes_v6, &tx_pkts_v6, &rx_bytes_v6);
        }

        if(result == QAPI_OK)
        {
          instanceP->tot_bytes_sent = tx_bytes_v4 + tx_bytes_v6;
          instanceP->tot_bytes_recv = rx_bytes_v4 + rx_bytes_v6;
          instanceP->tot_pkts_sent  = tx_pkts_v4 + tx_pkts_v6;
        }
        /* IP address need to be updated */

        if(family == APN_PDN_TYPE_IPv4v6)
        {
           res_id = 2;
        }

        if(res_id == 0 || res_id == 1)
        {
          prv_delete_string_resource_inst_val(&instanceP->ip_addr, res_id);
          prv_delete_string_resource_inst_val(&instanceP->prefix_len, res_id);
          prv_delete_string_resource_inst_val(&instanceP->subnet_mask, res_id);
          prv_delete_string_resource_inst_val(&instanceP->gateway, res_id);
          prv_delete_string_resource_inst_val(&instanceP->pri_dns_addr, res_id);
          prv_delete_string_resource_inst_val(&instanceP->sec_dns_addr, res_id);
        }
      }
    break;

   case LWM2M_DATA_CALL_CONNECTED:
   case LWM2M_DATA_CALL_RECONFIGURED:
      {
        qapi_DSS_Addr_Info_t  *addr_info = NULL;
        qapi_Status_t result = QAPI_ERROR;
        char ip_addr_str[48];
        int8_t ri = 0;
        resource_instance_conn_list_t *conn_list = instanceP->connection_stats;

        ds_iot_ulog_high("QAPI_Data call successful .");

        addr_info = (qapi_DSS_Addr_Info_t *)malloc(sizeof(qapi_DSS_Addr_Info_t));
        if(addr_info == NULL)
        {
          return;
        }

        memset(addr_info, 0x00, sizeof(qapi_DSS_Addr_Info_t));

        while(conn_list)
        {
          if(conn_list->family == family)
          {
            conn_list->conn_estblsh_result_isValid = LWM2M_TRUE;
            conn_list->conn_estblsh_result         = LWM2M_FALSE;
            break;
          }
          conn_list = conn_list->next;
        }

        if(family == APN_PDN_TYPE_NON_IP || family == APN_PDN_TYPE_IPv4)
        {
          result = get_ip_info_by_apn_info(hndl, addr_info, APN_PDN_TYPE_IPv4);
          if(result == QAPI_OK)
          {
            if(family == APN_PDN_TYPE_IPv4)
            {
              ri = 0;
              if(addr_info->iface_addr_s.valid_addr)
              {
                inet_ntop(AF_INET, &addr_info->iface_addr_s.addr.v4, ip_addr_str, INET4_ADDRSTRLEN);
                prv_write_string_resource_inst_val(&(instanceP->ip_addr),ri, ip_addr_str, &val_change);
              }
              if(addr_info->gtwy_addr_s.valid_addr)
              {
                inet_ntop(AF_INET, &addr_info->gtwy_addr_s.addr.v4, ip_addr_str, INET4_ADDRSTRLEN);
                prv_write_string_resource_inst_val(&(instanceP->gateway),ri, ip_addr_str, &val_change);
              }
              memset(ip_addr_str, 0x00, sizeof(ip_addr_str));
              prv_write_string_resource_inst_val(&(instanceP->prefix_len),ri, ip_addr_str, &val_change);

              memset(ip_addr_str, 0x00, sizeof(ip_addr_str));
              inet_ntop(AF_INET, &addr_info->iface_mask, ip_addr_str, INET4_ADDRSTRLEN);
              prv_write_string_resource_inst_val(&(instanceP->subnet_mask),ri, ip_addr_str, &val_change);

              if(addr_info->dnsp_addr_s.valid_addr)
              {
                inet_ntop(AF_INET, &addr_info->dnsp_addr_s.addr.v4, ip_addr_str, INET4_ADDRSTRLEN);
                prv_write_string_resource_inst_val(&(instanceP->pri_dns_addr),ri, ip_addr_str, &val_change);
              }
              if(addr_info->dnss_addr_s.valid_addr)
              {
                inet_ntop(AF_INET, &addr_info->dnss_addr_s.addr.v4, ip_addr_str, INET4_ADDRSTRLEN);
                prv_write_string_resource_inst_val(&(instanceP->sec_dns_addr),ri, ip_addr_str, &val_change);
              }
            }
          }
          if(family == APN_PDN_TYPE_NON_IP)
          {
            ri = 0;
            memset(ip_addr_str, 0x00, sizeof(ip_addr_str));
            prv_write_string_resource_inst_val(&(instanceP->ip_addr),ri, ip_addr_str, &val_change);
            prv_write_string_resource_inst_val(&(instanceP->gateway),ri, ip_addr_str, &val_change);
            prv_write_string_resource_inst_val(&(instanceP->prefix_len),ri, ip_addr_str, &val_change);
            prv_write_string_resource_inst_val(&(instanceP->subnet_mask),ri, ip_addr_str, &val_change);
            prv_write_string_resource_inst_val(&(instanceP->pri_dns_addr),ri, ip_addr_str, &val_change);
            prv_write_string_resource_inst_val(&(instanceP->sec_dns_addr),ri, ip_addr_str, &val_change);
          }
        }

        else if(family == APN_PDN_TYPE_IPv6)
        {
          result = get_ip_info_by_apn_info(hndl, addr_info, APN_PDN_TYPE_IPv6);
          if(result == QAPI_OK)
          {
            if(family == APN_PDN_TYPE_IPv6)
            {
              ri = 1;

              if(addr_info->iface_addr_s.valid_addr)
              {
                inet_ntop(AF_INET6, addr_info->iface_addr_s.addr.v6_addr8, ip_addr_str, INET6_ADDRSTRLEN);
                prv_write_string_resource_inst_val(&(instanceP->ip_addr),ri, ip_addr_str, &val_change);
              }

              if(addr_info->gtwy_addr_s.valid_addr)
              {
                inet_ntop(AF_INET6, addr_info->gtwy_addr_s.addr.v6_addr8, ip_addr_str, INET6_ADDRSTRLEN);
                prv_write_string_resource_inst_val(&(instanceP->gateway),ri, ip_addr_str, &val_change);
              }

              memset(ip_addr_str, 0x00, sizeof(ip_addr_str));
              prv_write_string_resource_inst_val(&(instanceP->subnet_mask),ri, ip_addr_str, &val_change);

              memset(ip_addr_str, 0x00, sizeof(ip_addr_str));
              snprintf(ip_addr_str, sizeof(uint32_t), "%u", addr_info->iface_mask);
              prv_write_string_resource_inst_val(&(instanceP->prefix_len),ri, ip_addr_str, &val_change);

              if(addr_info->dnsp_addr_s.valid_addr)
              {
                inet_ntop(AF_INET6, addr_info->dnsp_addr_s.addr.v6_addr8, ip_addr_str, INET6_ADDRSTRLEN);
                prv_write_string_resource_inst_val(&(instanceP->pri_dns_addr),ri, ip_addr_str, &val_change);
              }

              if(addr_info->dnss_addr_s.valid_addr)
              {
                inet_ntop(AF_INET6, addr_info->dnss_addr_s.addr.v6_addr8, ip_addr_str,INET6_ADDRSTRLEN);
                prv_write_string_resource_inst_val(&(instanceP->sec_dns_addr),ri, ip_addr_str, &val_change);
              }
            }
          }
        }

        free(addr_info);
        addr_info = NULL;

      }
    break;

   case LWM2M_DATA_CALL_FAILURE:
      {
        qapi_DSS_CE_Reason_t *conn_reject_cause = NULL;
        qapi_Status_t result = QAPI_ERROR;
        resource_instance_conn_list_t *conn_list = instanceP->connection_stats;
        uint16_t res_id = 0xFFFF;

        ds_iot_ulog_high("QAPI_Data call un-successful .");

        conn_reject_cause = (qapi_DSS_CE_Reason_t *)malloc(sizeof(qapi_DSS_CE_Reason_t));
        if(conn_reject_cause == NULL)
        {
          return;
        }

        memset(conn_reject_cause, 0x00, sizeof(qapi_DSS_CE_Reason_t));

        while(conn_list)
        {
          if(conn_list->family == family)
          {
            conn_list->conn_estblsh_result_isValid = LWM2M_TRUE;
            conn_list->conn_estblsh_result         = LWM2M_TRUE;
            conn_list->conn_end_time_isValid = LWM2M_TRUE;
            conn_list->conn_end_time         = lwm2m_gettime()+GPS_TO_UNIX_TIME_OFFSET;
            result = get_apn_conn_failure_reason(hndl, conn_reject_cause, family);
            if(result == QAPI_OK)
            {
              if(conn_reject_cause->reason_code >= 0 && conn_reject_cause->reason_code <= 111)
              {
                conn_list->conn_reject_cause_isValid = LWM2M_TRUE;
                conn_list->conn_reject_cause         = conn_reject_cause->reason_code;
              }
              else
              {
                conn_list->conn_reject_cause_isValid = LWM2M_TRUE;
                conn_list->conn_reject_cause         = 0xFF;
              }
            }
            break;
          }
          conn_list = conn_list->next;
        }

        if(family == APN_PDN_TYPE_IPv4 || family == APN_PDN_TYPE_NON_IP)
        {
          res_id = 0;
        }

        else if(family == APN_PDN_TYPE_IPv6)
        {
          res_id = 1;
        }

        /* IP address need to be updated */

        if(family == APN_PDN_TYPE_IPv4v6)
        {
          res_id = 2;
        }

        if(res_id == 0 || res_id == 1)
        {
          prv_delete_string_resource_inst_val(&instanceP->ip_addr, res_id);
          prv_delete_string_resource_inst_val(&instanceP->prefix_len, res_id);
          prv_delete_string_resource_inst_val(&instanceP->subnet_mask, res_id);
          prv_delete_string_resource_inst_val(&instanceP->gateway, res_id);
          prv_delete_string_resource_inst_val(&instanceP->pri_dns_addr, res_id);
          prv_delete_string_resource_inst_val(&instanceP->sec_dns_addr, res_id);
        }
        free(conn_reject_cause);
        conn_reject_cause = NULL;

      }
    break;

    case LWM2M_DATA_CALL_REGISTERED:
      {
        /* Link the associated DSS call handle for Object 11 instance .*/
        ds_iot_ulog_high1("QAPI_Register event received for handle [0x%x]", hndl);
        apn_info_register_util((apn_conn_profile_instance_t *)apn_object_list->instanceList, family, hndl, instance_id);
      }
    break;

    case LWM2M_DATA_CALL_DEREGISTERED:
      {
      /* De link the associated DSS call handle for Object 11 instance .*/
        ds_iot_ulog_high1("QAPI_De-Register event received for handle [0x%x]", hndl);
        if(family == APN_PDN_TYPE_IPv4 || family == APN_PDN_TYPE_NON_IP)
        {
          apn_info_event_notify(hndl, 0xFFFFFFFF, LWM2M_DATA_CALL_STOP, family);
          instanceP->apn_hndl_v4 = NULL;
        }
        else if(family == APN_PDN_TYPE_IPv6 )
        {
          apn_info_event_notify(hndl, 0xFFFFFFFF, LWM2M_DATA_CALL_STOP, family);
          instanceP->apn_hndl_v6 = NULL;
        }
      }
    break;
  }

  return;
}

/**
* @brief  API to retrieve packets sent/ received on particular interface.
*
* @param[in]  handle  Handle received after linking successfully with APN Link resource.
* @param[out] pktStats  Packet statistics .
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK (0) is returned. Other value on error.
*/
qapi_Status_t get_packet_info_by_apn_info(void *hndl,
                                          qapi_DSS_Data_Pkt_Stats_t *pktStats)
{
  qapi_DSS_Hndl_t *curr_hndl = NULL;

  if(hndl == NULL)
  {
    ds_iot_ulog_err(" LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  curr_hndl = (qapi_DSS_Hndl_t *)hndl;

  memset(pktStats, 0x00, sizeof(qapi_DSS_Data_Pkt_Stats_t));

    if(curr_hndl != NULL)
    {
    if (QAPI_OK != qapi_DSS_Get_Pkt_Stats(curr_hndl, pktStats))
      {
        LOG_DEBUG("Failed to get the packet statistics for hndl[%p]", curr_hndl);
        return QAPI_ERROR;
      }
    }

  return QAPI_OK;
}

/**
* @brief  API to retrieve IP address info for particular interface.
*
* @param[in]  handle  Handle received after linking successfully with APN Link resource.
* @param[out] addr_info  IP Address information .
* @param[in] family  IP family type (IPv4 or IPv6) For Dual IP and NonIP error will be returned.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK (0) is returned. Other value on error.
*/
qapi_Status_t get_ip_info_by_apn_info(void *hndl,
                                      qapi_DSS_Addr_Info_t *addr_info,
                                      int family)
{
  qapi_DSS_Hndl_t *curr_hndl = NULL;

  if(hndl == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  curr_hndl = (qapi_DSS_Hndl_t *)hndl;

  memset(addr_info, 0x00, sizeof(qapi_DSS_Addr_Info_t));

  if(family == APN_PDN_TYPE_IPv4 )
  {
    if(curr_hndl != NULL)
    {
      if (QAPI_OK != qapi_DSS_Get_IP_Addr_Per_Family(curr_hndl, addr_info, QAPI_DSS_IP_VERSION_4))
      {
        LOG_DEBUG("Failed to get the IP address statistics for hndl[%p]", curr_hndl);
        return QAPI_ERROR;
      }
    }
  }

  else if(family == APN_PDN_TYPE_IPv6 )
  {
    if(curr_hndl != NULL)
    {
      if (QAPI_OK != qapi_DSS_Get_IP_Addr_Per_Family(curr_hndl, addr_info, QAPI_DSS_IP_VERSION_6))
      {
         LOG_DEBUG("Failed to get the IP address statistics for hndl[%p]", curr_hndl);
        return QAPI_ERROR;
      }
    }
  }

  else if(family == APN_PDN_TYPE_IPv4v6|| family == APN_PDN_TYPE_NON_IP)
  {
    ds_iot_ulog_err("Non IP or dual IP family specified.");
    return QAPI_ERROR;
  }

  return QAPI_OK;
}

/**
* @brief  API to retrieve connection failure parameters for particular interface.
*
* @param[in]  handle  Handle received after linking successfully with APN Link resource.
* @param[out] info  Connection related information .(Call end reason and type)
* @param[in] family  IP family type (IPv4/ IPv6 / Non-IP)
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK (0) is returned. Other value on error.
*/
qapi_Status_t get_apn_conn_failure_reason(void *hndl,
                                          qapi_DSS_CE_Reason_t *conn_reject_cause,
                                           int family)
{
  qapi_DSS_Hndl_t *curr_hndl = NULL;

  if(hndl == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  curr_hndl = (qapi_DSS_Hndl_t *)hndl;

  memset(conn_reject_cause, 0x00, sizeof(qapi_DSS_CE_Reason_t));

  if(family == APN_PDN_TYPE_IPv4 || family == APN_PDN_TYPE_NON_IP)
  {
    if (QAPI_OK != qapi_DSS_Get_Call_End_Reason(curr_hndl, conn_reject_cause, QAPI_DSS_IP_FAMILY_V4_E))
    {
      LOG_DEBUG("Failed to get the call end reason for hndl[%p]", curr_hndl);
      return QAPI_ERROR;
    }
  }

  else if(family == APN_PDN_TYPE_IPv6 )
  {
    if (QAPI_OK != qapi_DSS_Get_Call_End_Reason(curr_hndl, conn_reject_cause, QAPI_DSS_IP_FAMILY_V6_E))
    {
      LOG_DEBUG("Failed to get the call end reason for hndl[%p]", curr_hndl);
      return QAPI_ERROR;
    }
  }

  else if(family == APN_PDN_TYPE_IPv4v6)
  {
    ds_iot_ulog_err("Dual IP family specified.");
    return QAPI_ERROR;
  }

  return QAPI_OK;
}


/**
* @brief  API to retrieve apn rate control for particular interface.
*
* @param[in]  handle  Handle received after linking successfully with APN Link resource.
* @param[out] apn_rate_control  apn rate control related information 
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK (0) is returned. Other value on error.
*/
qapi_Status_t get_apn_rate_control(void *hndl,
                                          qapi_DSS_Apn_Rate_Control_Info_t *apn_rate_control_info)
{
  qapi_DSS_Hndl_t *curr_hndl = NULL;
  qapi_Status_t ret = QAPI_ERROR;

  if(hndl == NULL || apn_rate_control_info == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return QAPI_ERR_INVALID_PARAM;
  }

  curr_hndl = (qapi_DSS_Hndl_t *)hndl;

  if(curr_hndl != NULL)
  {
    ret = qapi_DSS_Get_Apn_Rate_Control(curr_hndl, apn_rate_control_info);
    if (QAPI_OK != ret && QAPI_ERR_NOT_SUPPORTED != ret)
    {
      ds_iot_ulog_err1("Failed to get the apn rate control for hndl[%p]", curr_hndl);
    }
  }
  return ret;
}
#else
int8_t get_apn_conn_params(objlink_t value, 
                           apn_conn_input_params_t *params)
{
	return QAPI_ERR_NO_ENTRY;
}
void apn_info_event_notify(void *hndl, objlink_t value,
                           lwm2m_data_call_event_t event, int family)
{
	return;
}
#endif // ENABLE_APN_CONN_OBJ

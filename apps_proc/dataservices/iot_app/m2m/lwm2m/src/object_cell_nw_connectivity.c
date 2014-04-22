/******************************************************************************

  @file    object_cell_nw_connectivity.c
  @brief   Cellular Network connectivity source file
           This file implements API's related to Cellular network connectivity object 
           This object specifies resources to enable a device to connect to a 3GPP or 3GPP2 bearer, 
           including GPRS/EDGE, UMTS, LTE, NB-IoT, SMS. 
   
  ---------------------------------------------------------------------------
  Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

#include "liblwm2m.h"
#include "internals.h"
#include "lwm2mclient.h"
#include "lwm2m_signals.h"
#include "object_cell_nw_connectivity.h"
#include "qapi_ns_utils.h"
#include "iotapp_qmi_internal.h"
#include "power_save_mode_service_v01.h"
#include "object_apn_connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iotapp_dss_util.h>
#include "ds_log.h"


#ifdef ENABLE_CELL_NW_CONN_OBJ

extern timer_group_type lwm2mTimerGroup; 
extern qurt_signal_t lwm2m_signal_Handle;

static bool create_operation = LWM2M_FALSE;
/**
 * @brief This function sets default values for Cellular Network connectivity
 * object instance 
 * @param instanceP (IN/OUT) Instance whose values to be set.
 * @return none
 */ 
void prv_cell_nw_conn_default_values(cell_nw_conn_instance_t *instanceP)
{
  if(instanceP == NULL)
  {
    LOG_DEBUG("%s",nullError);
    return;
  }
  instanceP->smsc_addr[0] = '\0';
  instanceP->disable_radio_period = 0;
  instanceP->psm_timer = 0;
  instanceP->active_timer = 0;
  instanceP->vendor_ext = 0xFFFFFFFF;
  instanceP->edrx_param_nbs1_mode = 0;
  instanceP->edrx_param_wbs1_mode = 0;
  instanceP->act_prof_name = NULL;
}

/**
 * @fn uint8_t load_cell_nw_conn_persistent_info()
 * @brief This function is used to fetch the cell nw connectivity from persistent storage
 * 
 * @param objectP pointer to Cell NW conn object
 * 
 * @return return error code
 */
int load_cell_nw_conn_persistent_info(lwm2m_object_t * objectP)
{
  int fd = 0, ret = 0;
  cell_nw_conn_persistent_info_t info;
  cell_nw_conn_instance_t *instanceP = NULL;

  if (NULL == objectP)
  {
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_CELL_NW_CONN_PERSISTENCE_FILE);
  fd = efs_open(LWM2M_CELL_NW_CONN_PERSISTENCE_FILE, O_RDONLY);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open cell nw conn persistent storage");
    return EFAILURE;
  }

  memset(&info, 0, sizeof(info));
  ret = efs_read(fd, &info, sizeof(info));
  if (EFAILURE == ret || 0 == ret)
    goto end;

  /*Check if the structure entry in file is valid*/
  if(LWM2M_FALSE == info.isValid)
  {
    ret = -1;
    goto end;
  }

  instanceP = (cell_nw_conn_instance_t *)lwm2m_malloc(sizeof(cell_nw_conn_instance_t));
  if(instanceP == NULL)
  {
    ret = -1;
    goto end;
  }

  memset(instanceP, 0, sizeof(cell_nw_conn_instance_t));
  prv_cell_nw_conn_default_values(instanceP);

  LWM2M_STRCPY(instanceP->smsc_addr, info.smsc_addr,WMS_ADDRESS_DIGIT_MAX_V01 +1);
  instanceP->disable_radio_period = info.disable_radio_period;
  instanceP->vendor_ext = info.vendor_ext;
  instanceP->psm_timer = info.psm_timer;
  instanceP->active_timer = info.active_timer;
  instanceP->edrx_param_nbs1_mode = info.edrx_param_nbs1_mode;
  instanceP->edrx_param_wbs1_mode = info.edrx_param_wbs1_mode;;

  objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, instanceP);
  objectP->userData = instanceP;

end:
  if (ret < 0)
  {
    efs_close(fd);
    return EFAILURE;
  }
  efs_close(fd);
  return ESUCCESS;
}


/**
 * @fn uint8_t store_cell_nw_conn_info_persistent_info()
 * @brief This function is used to store the cell nw connectivity into persistent storage
 * 
 * @param targetP pointer to Cell nw conn object instance
 * @param store   to check whether instance stored is valid or not.
 * 
 * @return return error code
 */
int store_cell_nw_conn_info_persistent_info(cell_nw_conn_instance_t * targetP, bool store)
{
  int fd = 0, ret = 0;
  cell_nw_conn_persistent_info_t *conn_ctx = NULL;
  int32 start_index = 0;

  if(NULL == targetP)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }

  conn_ctx = (cell_nw_conn_persistent_info_t *)malloc(sizeof(cell_nw_conn_persistent_info_t));
  if(conn_ctx == NULL)
  {
    return EFAILURE;
  }
  	

  /* open cell nw conn persistent file.If not already present, create it*/
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_CELL_NW_CONN_PERSISTENCE_FILE);
  fd = efs_open(LWM2M_CELL_NW_CONN_PERSISTENCE_FILE, O_CREAT | O_RDWR, 00644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open cell nw conn persistence file");
    ret = EFAILURE;
    goto end;
  }

  memset(conn_ctx, 0x00, sizeof(cell_nw_conn_persistent_info_t));

  LWM2M_STRCPY(conn_ctx->smsc_addr, targetP->smsc_addr, WMS_ADDRESS_DIGIT_MAX_V01 +1);
  conn_ctx->disable_radio_period = targetP->disable_radio_period;
  conn_ctx->vendor_ext = targetP->vendor_ext;
  conn_ctx->psm_timer = targetP->psm_timer;
  conn_ctx->active_timer = targetP->active_timer;
  conn_ctx->edrx_param_nbs1_mode = targetP->edrx_param_nbs1_mode;
  conn_ctx->edrx_param_wbs1_mode = targetP->edrx_param_wbs1_mode;;

  if(store)
  {
    conn_ctx->isValid = LWM2M_TRUE; 
  }
  else
  {
    conn_ctx->isValid = LWM2M_FALSE; 
  }

  /* lseek to reqd index and write the cell_nw_conn structure in efs */
  efs_lseek(fd, start_index, SEEK_SET);
  ds_iot_log_strf_med("EFS_OPT: File write %s",LWM2M_CELL_NW_CONN_PERSISTENCE_FILE);
  if (EFAILURE == efs_write(fd, conn_ctx, sizeof(cell_nw_conn_persistent_info_t)))
  {
    ds_iot_ulog_err("failed to store persistent value of cell nw conn instance");
    efs_close(fd);
    ret = EFAILURE;
    goto end;
  }
  efs_close(fd);
  ret = ESUCCESS;

end:
  free(conn_ctx);
  return ret;
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
                             cell_nw_conn_instance_t * cell_nw_conn_Obj,
                             uint16_t resInstId)
{
  char * smsc_addr = NULL;
  psm_get_cfg_params_resp_msg_v01 *psm_cfg_params = NULL;
  uint8_t edrx_params;

  if( dataP == NULL || cell_nw_conn_Obj == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  } 

  switch (dataP->id)
  {
    case RES_O_SMSC_ADDR:
      {
        smsc_addr = qmi_wms_get_smsc_address();
        if(smsc_addr)
        {
          strlcpy(cell_nw_conn_Obj->smsc_addr,smsc_addr,strlen(smsc_addr)+1);
          free(smsc_addr);
        }
        else
        {
           return LWM2M_500_INTERNAL_SERVER_ERROR;
        }
        lwm2m_data_encode_string(cell_nw_conn_Obj->smsc_addr, dataP);
      }
      return LWM2M_205_CONTENT;

    case RES_O_DISABLE_RADIO_PERIOD:
      lwm2m_data_encode_int(cell_nw_conn_Obj->disable_radio_period, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_OBJ_10_VENDOR_EXT:
      lwm2m_data_encode_objLink(cell_nw_conn_Obj->vendor_ext, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_PSM_TIMER:
      psm_cfg_params = (psm_get_cfg_params_resp_msg_v01 *)get_psm_params();
      if(psm_cfg_params)
      {
        cell_nw_conn_Obj->psm_timer = psm_cfg_params->periodic_update_timer;
        lwm2m_data_encode_int(cell_nw_conn_Obj->psm_timer, dataP);
        free(psm_cfg_params);
        return LWM2M_205_CONTENT;
      }
      else
      {
       return LWM2M_500_INTERNAL_SERVER_ERROR;
      }
    case RES_O_ACTIVE_TIMER:
      psm_cfg_params = (psm_get_cfg_params_resp_msg_v01 *)get_psm_params();
      if(psm_cfg_params)
      {
        cell_nw_conn_Obj->active_timer = psm_cfg_params->active_timer;
        lwm2m_data_encode_int(cell_nw_conn_Obj->active_timer, dataP);
        free(psm_cfg_params);
        return LWM2M_205_CONTENT;
       }
      else
      {
       return LWM2M_500_INTERNAL_SERVER_ERROR;
      }

    case RES_O_SERVING_PLMN_RATE_CONTROL:
    {
       uint32_t splmn_rate_control = 0;
       qapi_Status_t result = QAPI_ERROR;
       lwm2m_server_t *server_list = NULL;
       lwm2m_server_t *oldest_registered_sever = NULL;
       carrier_apn_cfg_t *apn_cfg = get_lwm2m_apn_cfg();

       if(data.lwm2mH == NULL)
          return LWM2M_500_INTERNAL_SERVER_ERROR;

       server_list = data.lwm2mH->serverList;
       if(server_list == NULL)
       {
         return LWM2M_500_INTERNAL_SERVER_ERROR;
       }
       while(server_list !=NULL && server_list->status != STATE_REGISTERED)
       {
          server_list = server_list->next;
       }
       if(server_list !=NULL)
       {
          oldest_registered_sever = server_list;
       }
       else
       {
         return LWM2M_500_INTERNAL_SERVER_ERROR;
       }
       while(server_list->next != NULL)
       {
          server_list = server_list->next;
          if(server_list->status == STATE_REGISTERED && (oldest_registered_sever->registration > server_list->registration))
          {
             oldest_registered_sever = server_list;
          }
       }
       if (NULL == apn_cfg)
       {
          return LWM2M_500_INTERNAL_SERVER_ERROR;
       } 
       apn_cfg = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(apn_cfg, oldest_registered_sever->shortID);
       if (NULL == apn_cfg)
       {
          return LWM2M_500_INTERNAL_SERVER_ERROR;
       } 
       
       if(apn_cfg->transport_binding == AF_INET || AF_NONIP == apn_cfg->transport_binding 
	                                            || apn_cfg->transport_binding == AF_INET_DUAL46)
       {
          result = get_splmn_rate_control_by_apn_info(get_data_call_hndl(apn_cfg->callHdl_v4_nip), &splmn_rate_control);
       }
       if((result !=QAPI_OK) && (apn_cfg->transport_binding == AF_INET6 
	                                      || apn_cfg->transport_binding == AF_INET_DUAL46))
       {
          result = get_splmn_rate_control_by_apn_info(get_data_call_hndl(apn_cfg->callHdl_v6) , &splmn_rate_control);
       }
       if(result == QAPI_OK)
       {
          cell_nw_conn_Obj->splmn_rate_control = splmn_rate_control;
          lwm2m_data_encode_int(cell_nw_conn_Obj->splmn_rate_control, dataP);
          return LWM2M_205_CONTENT;
       }
       else
       {
         return LWM2M_500_INTERNAL_SERVER_ERROR;
       }
    }

    case RES_O_EDRX_NB_S1:

    if(iotapp_qmi_nas_get_erdx_params(&edrx_params, IOTAPP_CIOT_MODE_NB1) != ESUCCESS)
    {
      return LWM2M_500_INTERNAL_SERVER_ERROR;
    }
    cell_nw_conn_Obj->edrx_param_nbs1_mode = edrx_params;
    lwm2m_data_encode_opaque(&(cell_nw_conn_Obj->edrx_param_nbs1_mode), 1, dataP);
    return LWM2M_205_CONTENT;

    case RES_O_EDRX_WB_S1:

    if(iotapp_qmi_nas_get_erdx_params(&edrx_params, IOTAPP_CIOT_MODE_CATM1) != ESUCCESS)
    {
      return LWM2M_500_INTERNAL_SERVER_ERROR;
    }
    cell_nw_conn_Obj->edrx_param_wbs1_mode = edrx_params;
    lwm2m_data_encode_opaque(&(cell_nw_conn_Obj->edrx_param_wbs1_mode), 1, dataP);
    return LWM2M_205_CONTENT;

    case RES_M_ACTIVATED_PROF_NAME:
    {
      uint32_t result                                = LWM2M_NO_ERROR;
      lwm2m_data_t                         *res_inst = NULL;
      uint32_t                                  cntr = 0x00;
      act_profile_name_info_s     *curr_profile_info = cell_nw_conn_Obj->act_prof_name;
      act_profile_name_info_s     *head_profile_info = NULL;
      act_profile_name_info_s     *prev_profile_info = NULL;
      lwm2m_object_t                   *apn_info_obj = NULL;
      apn_conn_profile_instance_t* apn_info_obj_inst = NULL;
      objlink_t                               valueP = 0xFFFFFFFF;

      //free list of active apn profile info
      while(curr_profile_info)
      {
        prev_profile_info = curr_profile_info->next;
        lwm2m_free(curr_profile_info);
        curr_profile_info = prev_profile_info;
      }
      curr_profile_info = NULL;
      cell_nw_conn_Obj->act_prof_name = NULL;

      qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

      apn_info_obj = (lwm2m_object_t *)LWM2M_LIST_FIND(data.lwm2mH->objectList , LWM2M_APN_CONN_PROFILE_OBJECT_ID);
      if (apn_info_obj == NULL)
      {
        res_inst = lwm2m_data_new(0x01);
        if (NULL == res_inst) 
        {
          result = LWM2M_500_INTERNAL_SERVER_ERROR;
        }
        else
        {
          res_inst->id = cntr++;
          lwm2m_data_encode_objLink(0xFFFFFFFF, res_inst);
          lwm2m_data_encode_instances(res_inst, cntr, dataP);
          result = LWM2M_205_CONTENT;
        }
        goto READ_PROFILE_NAME_ERROR;
      }
      apn_info_obj_inst = (apn_conn_profile_instance_t *)apn_info_obj->instanceList;
      if (apn_info_obj_inst == NULL)
      {
        res_inst = lwm2m_data_new(0x01);
        if (NULL == res_inst) 
        {
          result = LWM2M_500_INTERNAL_SERVER_ERROR;
        }
        else
        {
          res_inst->id = cntr++;
          lwm2m_data_encode_objLink(0xFFFFFFFF, res_inst);
          lwm2m_data_encode_instances(res_inst, cntr, dataP);
          result = LWM2M_205_CONTENT;
        }
        goto READ_PROFILE_NAME_ERROR;
      }

      while(apn_info_obj_inst)
      {
         if(apn_info_obj_inst->ip_addr != NULL)
         {
          if(head_profile_info == NULL)
          {
            head_profile_info = (act_profile_name_info_s *)lwm2m_malloc(sizeof(act_profile_name_info_s));
            if (NULL == head_profile_info) 
            {
              result = LWM2M_500_INTERNAL_SERVER_ERROR;
              goto READ_PROFILE_NAME_ERROR;
            }
            valueP = (LWM2M_APN_CONN_PROFILE_OBJECT_ID<<16)|(apn_info_obj_inst->instanceId);
            head_profile_info->link_info = valueP;
            head_profile_info->resInstId = cntr++;
            head_profile_info->next = NULL;
            prev_profile_info = head_profile_info;
          }
          else
          {
            curr_profile_info = (act_profile_name_info_s *)lwm2m_malloc(sizeof(act_profile_name_info_s));
            if (NULL == curr_profile_info)
            {
             result = LWM2M_500_INTERNAL_SERVER_ERROR;
             goto READ_PROFILE_NAME_ERROR;
            }
            valueP = (LWM2M_APN_CONN_PROFILE_OBJECT_ID<<16)|(apn_info_obj_inst->instanceId);
            curr_profile_info->link_info = valueP;
            curr_profile_info->resInstId = cntr++;
            curr_profile_info->next = NULL;
            prev_profile_info->next = curr_profile_info;
            prev_profile_info = prev_profile_info->next;
          }
        }
        apn_info_obj_inst = apn_info_obj_inst->next;
      }

      if(head_profile_info == NULL || cntr == 0)
      {
         res_inst = lwm2m_data_new(0x01);
         if (NULL == res_inst) 
         {
            result = LWM2M_500_INTERNAL_SERVER_ERROR;
         }
         else
         {
            res_inst->id = cntr++;
            lwm2m_data_encode_objLink(0xFFFFFFFF, res_inst);
            lwm2m_data_encode_instances(res_inst, cntr, dataP);
            result = LWM2M_205_CONTENT;
         }
         goto READ_PROFILE_NAME_ERROR;
      }
      cell_nw_conn_Obj->act_prof_name = head_profile_info;
      if (resInstId != INVALID_RES_INST_ID)
      {
        cntr = 0x00;
        curr_profile_info = cell_nw_conn_Obj->act_prof_name;
        while (curr_profile_info)
        {
          if(resInstId == cntr)
          {
            res_inst = lwm2m_data_new(0x01);
            if (NULL == res_inst) 
            {
              result = LWM2M_500_INTERNAL_SERVER_ERROR;
              goto READ_PROFILE_NAME_ERROR;
            }
            res_inst->id = cntr;
            lwm2m_data_encode_objLink(curr_profile_info->link_info, res_inst);
            break;
          }
         curr_profile_info = curr_profile_info->next;
         cntr++;
        }
        if(resInstId != cntr)
        {
          result = LWM2M_404_NOT_FOUND;
          goto READ_PROFILE_NAME_ERROR;
        }
        else
          cntr = 1;
      }
      else
      {
        res_inst = lwm2m_data_new(cntr);
        if (NULL == res_inst) 
        {
          result = LWM2M_500_INTERNAL_SERVER_ERROR;
          goto READ_PROFILE_NAME_ERROR;
        }

        cntr = 0x00;
        curr_profile_info = cell_nw_conn_Obj->act_prof_name;
        while (curr_profile_info)
        {
          res_inst[cntr].id = cntr;
          lwm2m_data_encode_objLink(curr_profile_info->link_info, &res_inst[cntr]);
        
         curr_profile_info = curr_profile_info->next;
         cntr++;
        }
      }
      lwm2m_data_encode_instances(res_inst, cntr, dataP);
      result = LWM2M_205_CONTENT;

READ_PROFILE_NAME_ERROR:
      qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

      if(result == LWM2M_500_INTERNAL_SERVER_ERROR)
      {
         while(head_profile_info)
         {
          prev_profile_info = head_profile_info->next;
          lwm2m_free(head_profile_info);
          head_profile_info = prev_profile_info;
         }
      }
        return result;
      }

    default:
      return LWM2M_404_NOT_FOUND;
  }
}


/**
 * @brief This function implements the read operation performed on Cell N/W
 * Connectivity object
 * @param instanceId (IN) Instance ID of the object to be read
 * @param numDataP (OUT) number of resources in the array
 * @param dataArrayP (OUT) resource value array
 * @param objectP (IN) Object to be read
 * @param resInstId (IN)  Resource instance id or INVALID_RES_INST_ID 
 *                        if resource instance id is not passed.
 * @return LWM2M response type
 */ 
static uint8_t prv_cell_nw_conn_read(uint16_t instanceId,
                                 int * numDataP,
                                 lwm2m_data_t ** dataArrayP,
                                 lwm2m_object_t * objectP,
                                 uint16_t resInstId, lwm2m_server_t *serverP)
{
  uint8_t result = LWM2M_404_NOT_FOUND;
  int i = 0;
  cell_nw_conn_instance_t *instanceP = NULL;

  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  instanceP = (cell_nw_conn_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, 
                                                             instanceId);
  if (NULL == instanceP)
  {
    return result;
  }

  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_O_SMSC_ADDR,
      RES_O_DISABLE_RADIO_PERIOD,
      RES_O_OBJ_10_VENDOR_EXT,
      RES_O_PSM_TIMER,
      RES_O_ACTIVE_TIMER,
      RES_O_SERVING_PLMN_RATE_CONTROL,
      RES_O_EDRX_WB_S1,
      RES_O_EDRX_NB_S1,
      RES_M_ACTIVATED_PROF_NAME,
    };
    int nbRes = sizeof(resList) / sizeof(uint16_t);

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;
    *numDataP = nbRes;
    for (i = 0; i < nbRes; i++)
    {
      (*dataArrayP)[i].id = resList[i];
    }
  }

  i = 0;
  do
  {
    ds_iot_ulog_high1("cell nw conn READ ON  Resource ID %d ",((*dataArrayP)+i)->id);  
    result = prv_set_value((*dataArrayP)+i, instanceP, resInstId);
    i++;
  }while (i < *numDataP && result == LWM2M_205_CONTENT);

  return result;
}
/**
 * @fn static uint8_t prv_cell_nw_conn_discover()
 *
 * @brief This function implements the discover operation performed on the
 *       Cell N/W Connectivity object.
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_cell_nw_conn_discover(uint16_t instanceId,
                                     int * numDataP,
                                     lwm2m_data_t ** dataArrayP,
                                     lwm2m_object_t * objectP, lwm2m_server_t *serverP)
{
  uint8_t result = LWM2M_205_CONTENT;
  cell_nw_conn_instance_t *targetP = NULL;
  int i = 0;

  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  targetP = (cell_nw_conn_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);

  if (NULL == targetP)
  {
    return LWM2M_404_NOT_FOUND;
  }

  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_O_SMSC_ADDR,
      RES_O_DISABLE_RADIO_PERIOD,
      RES_O_OBJ_10_VENDOR_EXT,
      RES_O_PSM_TIMER,
      RES_O_ACTIVE_TIMER,
      RES_O_SERVING_PLMN_RATE_CONTROL,
      RES_O_EDRX_WB_S1,
      RES_O_EDRX_NB_S1,
      RES_M_ACTIVATED_PROF_NAME,
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
      case RES_M_ACTIVATED_PROF_NAME:
      {
        uint32_t cntr = 0;
        act_profile_name_info_s *act_prof_name = targetP->act_prof_name;

        while (act_prof_name)
        {
          act_prof_name = act_prof_name->next;
          cntr++;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = cntr;
      }
      break;

      case RES_O_SMSC_ADDR:
      case RES_O_DISABLE_RADIO_PERIOD:
	  case RES_O_OBJ_10_VENDOR_EXT:
      case RES_O_PSM_TIMER:
      case RES_O_ACTIVE_TIMER:
      case RES_O_SERVING_PLMN_RATE_CONTROL:
      case RES_O_EDRX_NB_S1:
      case RES_O_EDRX_WB_S1:
      break;
      default:
        result = LWM2M_404_NOT_FOUND;
    }
  }

  return result;
}


/** 
* @fn static uint8_t prv_cell_nw_conn_create_write_check() 
*
* @brief This function verifies and write the resource's into  
*        Cellular Network connectivity object 
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

- For the �Write� operation on an Object Instance, the LwM2M Client MUST perform the operation, 
if all the Resources conveyed in the operation are allowed to perform the �Write� operation. 
If any Resource does not support the �Write� operation, the LwM2M Client MUST inform the LwM2M 
Server that the Object Instance cannot perform the requested �Write� operation in sending a 
�Operation is not supported� error code.

*************************************************************************************************************/ 

static uint8_t prv_cell_nw_conn_create_write_check(int numData,
                                               lwm2m_data_t * dataArray,
                                               bool create_flag)
{ 
  int16_t i = 0; 
  uint8_t result = LWM2M_NO_ERROR; 
  if((NULL == dataArray))
  { 
    LOG_DEBUG("%s",nullError); 
    return LWM2M_500_INTERNAL_SERVER_ERROR; 
  } 
   
  do
  {
    switch(dataArray[i].id)
    {
      case RES_M_ACTIVATED_PROF_NAME:
      case RES_O_OBJ_10_VENDOR_EXT:
      case RES_O_SERVING_PLMN_RATE_CONTROL:
      if (!create_flag)
      {
        result = LWM2M_405_METHOD_NOT_ALLOWED;
        break;
      }
      case RES_O_SMSC_ADDR:
      case RES_O_DISABLE_RADIO_PERIOD:
	  case RES_O_MOD_ACTIVATION_CODE:
      case RES_O_PSM_TIMER:
      case RES_O_ACTIVE_TIMER:
      case RES_O_EDRX_IU:
      case RES_O_EDRX_WB_S1:
      case RES_O_EDRX_NB_S1:
      case RES_O_EDRX_AGB:
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
 * @brief This function is used to write the resources into object instance
 *        while creating the object instance.or got write request from server.

 * @param   objectP (IN)   handle to object
 * @param   instanceP (IN/OUT)   handle to object instance
 * @param   numData (IN)        number of resources to be written
 * @param   dataArray (IN)      array of resource values
 * @param resInstId Resource instance id or INVALID_RES_INST_ID 
 *                  if resource instance id is not passed.
 * @param write_method To distinguish between partial write or replace.
 * @return  LWM2M response type
 */
static uint8_t prv_cell_nw_conn_write_resources(lwm2m_object_t * objectP,
                                            cell_nw_conn_instance_t *instanceP,
                                            int numData,
                                            lwm2m_data_t *dataArray,
                                            uint16_t resInstId, uint8_t write_method)
{
  int i = 0;
  uint8_t result = LWM2M_204_CHANGED;
  psm_set_cfg_params_req_msg_v01  psm_cfg_params;
  char temp_smsc_addr[WMS_ADDRESS_DIGIT_MAX_V01 + 1];
  bool val_change = false;

  if( instanceP == NULL  || dataArray == NULL )
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  do
  {
    switch (dataArray[i].id)
    {
      case RES_O_SMSC_ADDR:
	  	if((dataArray + i)->value.asBuffer.buffer == NULL || (dataArray + i)->value.asBuffer.length > WMS_ADDRESS_DIGIT_MAX_V01)
	  	{
	  	   return LWM2M_400_BAD_REQUEST;
	  	}
	  	LWM2M_STRCPY(temp_smsc_addr, (const char *)((dataArray + i)->value.asBuffer.buffer),
                    ((dataArray + i)->value.asBuffer.length) + 1);
        if(qmi_wms_set_smsc_address(temp_smsc_addr) != ESUCCESS)
        {
          return LWM2M_400_BAD_REQUEST;
        }
        if(lwm2m_strcmp(instanceP->smsc_addr, temp_smsc_addr) != 0)
	{
          val_change = true;
	}
        LWM2M_STRCPY(instanceP->smsc_addr, (const char *)((dataArray + i)->value.asBuffer.buffer),
                    ((dataArray + i)->value.asBuffer.length) + 1); 
	  
        break;

      case RES_O_DISABLE_RADIO_PERIOD:
      {
        int64_t value = 0;
        if(1 != lwm2m_data_decode_int(dataArray+i, &value))
        {
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(value < 0 || value > 65535)
          {
            return LWM2M_400_BAD_REQUEST;
          }
          if(instanceP->disable_radio_period != value)
          {
            val_change = true;
          }
          instanceP->disable_radio_period = value;
          qurt_signal_set(&lwm2m_signal_Handle, LWM2M_PS_DETACH);
        }
      }
      break;

      case RES_O_OBJ_10_VENDOR_EXT:
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
          val_change = true;
        }
        instanceP->vendor_ext = obj_link;
      }
      break;

      case RES_O_PSM_TIMER:
      {
        int64_t value = 0;
        if(1 != lwm2m_data_decode_int(dataArray+i, &value))
        {
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(value < 600 || value > 85708800)
          {
            return LWM2M_400_BAD_REQUEST;
          }
          else
          {
            memset(&psm_cfg_params, 0x0, sizeof(psm_cfg_params));
			psm_cfg_params.psm_enabled_valid = TRUE;
			psm_cfg_params.psm_enabled = 1;
			psm_cfg_params.periodic_update_timer_valid = TRUE;
            psm_cfg_params.periodic_update_timer = value;
            if (set_psm_params((void *)&psm_cfg_params) != ESUCCESS)
            {
               return LWM2M_400_BAD_REQUEST;
            }
            if(instanceP->psm_timer != value)
            {
              val_change = true;
            }
            instanceP->psm_timer = value;
          }
        } 
      }
      break;

      case RES_O_ACTIVE_TIMER:
      {
        int64_t value = 0;
        if(1 != lwm2m_data_decode_int(dataArray+i, &value))
        {
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(value < 2 || value > 1860)
          {
            return LWM2M_400_BAD_REQUEST;
          }
          else
          {
            memset(&psm_cfg_params, 0x0, sizeof(psm_cfg_params));
			psm_cfg_params.psm_enabled_valid = TRUE;
			psm_cfg_params.psm_enabled = 1;
			psm_cfg_params.active_timer_valid = TRUE;
            psm_cfg_params.active_timer = value;
			if (set_psm_params((void *)&psm_cfg_params) != ESUCCESS)
            {
               return LWM2M_400_BAD_REQUEST;
            }
            if(instanceP->active_timer != value)
            {
              val_change = true;
            }
            instanceP->active_timer = value;
          }
        }
      }
      break;

      case RES_O_EDRX_NB_S1:
        {
		uint8_t value = 0;
		if(((dataArray+i)->value.asBuffer.length != 1)|| ((dataArray+i)->value.asBuffer.buffer[0] > 255))
		{
          return LWM2M_400_BAD_REQUEST;		
        }
        else
        {
		   value = (dataArray+i)->value.asBuffer.buffer[0];
		}
		
          if (iotapp_qmi_nas_set_erdx_params(value, IOTAPP_CIOT_MODE_NB1) != ESUCCESS)
          {
               return LWM2M_400_BAD_REQUEST;
          }
        if(instanceP->edrx_param_nbs1_mode != value)
        {
          val_change = true;
        }
        instanceP->edrx_param_nbs1_mode = value; 
      }
        break;

      case RES_O_EDRX_WB_S1:
      {
        uint8_t value = 0;
        if(((dataArray+i)->value.asBuffer.length != 1)|| ((dataArray+i)->value.asBuffer.buffer[0] > 255))
        {
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          value = (dataArray+i)->value.asBuffer.buffer[0];
        }

        if (iotapp_qmi_nas_set_erdx_params(value, IOTAPP_CIOT_MODE_CATM1) != ESUCCESS)
        {
          return LWM2M_400_BAD_REQUEST;
        }
        if(instanceP->edrx_param_wbs1_mode != value)
        {
          val_change = true;
        }
        instanceP->edrx_param_wbs1_mode = value;
      }
      break;
      case RES_M_ACTIVATED_PROF_NAME:
      {
        act_profile_name_info_s *new_prof_info = NULL;
        act_profile_name_info_s **head_prof_info = &(instanceP->act_prof_name);
        act_profile_name_info_s *curr_prof_info = *head_prof_info; 
    
        /* Skip duplicate links */    
        while (curr_prof_info)
        {
          if (curr_prof_info->link_info == dataArray->value.asObjLink)
            return LWM2M_204_CHANGED;
          curr_prof_info = curr_prof_info->next;
        }

        curr_prof_info = *head_prof_info;

        new_prof_info = lwm2m_malloc(sizeof(act_profile_name_info_s));  
        if (!new_prof_info)
        {
          ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED for activated prof name resource");
          return LWM2M_404_NOT_FOUND;
        }
    
        memset(new_prof_info, 0x00, sizeof(act_profile_name_info_s));
        new_prof_info->link_info = dataArray->value.asObjLink;
    
        if (!curr_prof_info)
        {
          *head_prof_info = new_prof_info;
        }
        else 
        {
          act_profile_name_info_s *prev_prof_info = NULL;
    
          while (curr_prof_info)
          {
            prev_prof_info = curr_prof_info;
            curr_prof_info = curr_prof_info->next;
          }
    
          if (prev_prof_info)
            prev_prof_info->next = new_prof_info;
        }
        val_change = true;
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
  if (LWM2M_204_CHANGED == result && (create_operation == TRUE || val_change == TRUE))
    store_cell_nw_conn_info_persistent_info(instanceP, LWM2M_TRUE);

  return result;
}

/**
 * @fn static uint8_t prv_cell_nw_conn_write()
 *
 * @brief This fucntion implements the write operation performed on 
 *        Cell NW Conn object
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
static uint8_t prv_cell_nw_conn_write(uint16_t instanceId,
                                  int numData,
                                  lwm2m_data_t * dataArray,
                                  lwm2m_object_t * objectP,
                                  uint16_t resInstId, uint8_t write_method, bool bs_true)
{
  cell_nw_conn_instance_t * targetP = NULL;
  uint8_t result = LWM2M_205_CONTENT;

  if( dataArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  targetP = (cell_nw_conn_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
  if (NULL == targetP)
  {
    return LWM2M_404_NOT_FOUND;
  }

  /* Check all resources to be written are writable or not */
  result = prv_cell_nw_conn_create_write_check(numData, dataArray, bs_true);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err ("Write operation failed due to write on Readable resource. ");
    return result;
  }

  /* Write the resource values */
  result = prv_cell_nw_conn_write_resources(objectP, targetP, numData, dataArray,
                                        resInstId, write_method);

  return result;
}


/**
 * @fn static uint8_t prv_cell_nw_conn_delete()
 *
 * @brief This fucntion is used to delete Cell NW Conn object
 *
 * @param id Instance ID of the object to be deleted
 * @param objectP Object to be deleted
 *
 * @return LWM2M response type
 */
static uint8_t prv_cell_nw_conn_delete(uint16_t id,
    lwm2m_object_t * objectP, 
    bool persist_file)
{
  cell_nw_conn_instance_t * cell_nw_conn_Obj = NULL;

  if( objectP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  
  objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&cell_nw_conn_Obj);

  if(cell_nw_conn_Obj != NULL && cell_nw_conn_Obj->ps_attach_timer.timer_state != TIMER_UNDEF_FLAG)
  {
  timer_undef(&(cell_nw_conn_Obj->ps_attach_timer));
  }
  if (NULL == cell_nw_conn_Obj) return LWM2M_404_NOT_FOUND;

  /*  Update single instance object persistence */

  store_cell_nw_conn_info_persistent_info(cell_nw_conn_Obj, LWM2M_FALSE);

  if (NULL != objectP->userData)
  {
     lwm2m_free(objectP->userData);
     objectP->userData = NULL;
  }

  

  return LWM2M_202_DELETED;

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
static uint8_t prv_cell_nw_conn_execute(uint16_t instanceId,
                                    uint16_t resourceId,
                                    uint8_t * buffer,
                                    int length,
                                    lwm2m_object_t * objectP)
{
  cell_nw_conn_instance_t *instanceP = NULL;

  if ( objectP == NULL )
  {
    LOG_ERROR("%s",nullError);
    return LWM2M_400_BAD_REQUEST;
  }

  instanceP = (cell_nw_conn_instance_t *)objectP->userData;

  if (NULL == instanceP)
  {
    return LWM2M_404_NOT_FOUND;
  }

  switch(resourceId)
  {
     
    case RES_O_SMSC_ADDR:
    case RES_O_DISABLE_RADIO_PERIOD:
    case RES_O_MOD_ACTIVATION_CODE:
    case RES_O_OBJ_10_VENDOR_EXT:
    case RES_O_PSM_TIMER:
    case RES_O_ACTIVE_TIMER:
    case RES_O_SERVING_PLMN_RATE_CONTROL:
    case RES_O_EDRX_IU:
    case RES_O_EDRX_WB_S1:
    case RES_O_EDRX_NB_S1:
    case RES_O_EDRX_AGB:
    case RES_M_ACTIVATED_PROF_NAME:
    return LWM2M_405_METHOD_NOT_ALLOWED;
    default:
      return LWM2M_404_NOT_FOUND;
  }
}
#endif

/**
 * @fn static uint8_t prv_cell_nw_conn_create()
 * @brief   This function implements the create operation performed
 * on the Cellular network connectivity object
 * @param   instanceId (IN) Instance ID of the object instance to be created
 * @param   numData (IN)    number of resources in the array i.e. dataArray
 * @param   dataArray (IN)  resource value array
 * @param   objectP (IN)    Object whose instance is to be created
 * return   COAP response type
 */

/***************************************************************************************************

 Under section 8.2.3 Operation on Object of OMA 1.1

- If the �Create� operation is permitted, the LwM2M Client MUST perform the instantiation on the 
Object only if all the mandatory Resources are present in the �New Value� parameter (see Section 5). 
If all the mandatory Resources are not present, the LwM2M Client MUST send a �Bad Request� error code 
to the LwM2M Server


*************************************************************************************************************/

static uint8_t prv_cell_nw_conn_create(uint16_t instanceId,
                                   int numData,
                                   lwm2m_data_t* dataArray,
                                   lwm2m_object_t* objectP, bool apiCall)
{
  cell_nw_conn_instance_t *instanceP = NULL;
  uint8_t result = LWM2M_500_INTERNAL_SERVER_ERROR;
  uint16_t mandatory_resource[] = {RES_M_ACTIVATED_PROF_NAME};
  uint8_t size = sizeof(mandatory_resource)/sizeof(mandatory_resource[0]);
  if(dataArray == NULL || objectP == NULL )
  {
    LOG_DEBUG("%s",nullError);
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
  result = prv_cell_nw_conn_create_write_check(numData, dataArray, TRUE);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err ("Creating instance failed ");
    return result;
  }
skip_check:

  /*check if already one instance is created */
  if(objectP->instanceList != NULL)
  {
    ds_iot_ulog_err("Cell N/W Conn object instance exist");
    return LWM2M_400_BAD_REQUEST;
  }

  instanceP = (cell_nw_conn_instance_t *)lwm2m_malloc(sizeof(cell_nw_conn_instance_t));

  if (instanceP != NULL)
  {
    memset(instanceP, 0, sizeof(cell_nw_conn_instance_t));
    instanceP->instanceId = instanceId;
    /* Add instance into object instance list */
    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, instanceP);
    objectP->userData = instanceP;

    /* Assign default values to all resources */
    prv_cell_nw_conn_default_values(instanceP);

    timer_def_osal(&instanceP->ps_attach_timer, &lwm2mTimerGroup,
          TIMER_NATIVE_OS_SIGNAL_TYPE, &lwm2m_signal_Handle, LWM2M_PS_ATTACH);

    /* Write the resource values */
    create_operation = LWM2M_TRUE;
    result = prv_cell_nw_conn_write_resources(objectP, instanceP, numData, dataArray, 
                                          INVALID_RES_INST_ID, QAPI_COAP_POST);
    create_operation = LWM2M_FALSE;
    if (result == LWM2M_204_CHANGED)
    {
      result = LWM2M_201_CREATED;
    }
    else
    {
      /* Delete the instance in case of failure 
	  3rd param Doesn't matter for Multi instance or Mandotary Objects,
	  only applicable for single instance optional objects.*/
      prv_cell_nw_conn_delete(instanceId, objectP,false); 
      /* Issue in creating object instance so retuning Bad request error.*/
      result = LWM2M_400_BAD_REQUEST;
    }
  }
  return result;

}
/*
 * @fn      static int32_t prv_cell_nw_conn_setvalue()
 * @brief   This function is used to set the particular resource value from application
 * @param   lwm2m_data - pointer to lwm2m object information
 * @param    targetP   - pointer to lwm2m object 
 * @return  on success - 1
            on error   - 0
 */
static int32_t prv_cell_nw_conn_setvalue(lwm2m_object_data_t *lwm2m_data, lwm2m_object_t *targetP)
{
  uint8_t res = LWM2M_204_CHANGED;
  cell_nw_conn_instance_t* instanceP = NULL;
  lwm2m_data_t *dataArray = NULL;

  if(lwm2m_data == NULL || targetP == NULL)
  {
    LOG_DEBUG("%s",nullError);
    return 0;
  }

  instanceP = (cell_nw_conn_instance_t *)targetP->userData;

  if(NULL == instanceP)
  {
    return 0;
  }

  if((lwm2m_data->no_instances != 0x01)|| 
         (!lwm2m_data->instance_info) ||
         ((lwm2m_data->instance_info->no_resources) &&
         (!lwm2m_data->instance_info->resource_info)))
    return 0;

  if(QAPI_OK != lwm2m_parse_application_object_info(lwm2m_data->instance_info,
                                                    lwm2m_data->instance_info->resource_info,
                                                    &dataArray))
  {
    return 0;
  }

  res = prv_cell_nw_conn_write_resources(targetP, instanceP, 1, 
                                     dataArray,
                                     INVALID_RES_INST_ID, QAPI_COAP_PUT);
  LOG_DEBUG("Result %u.%2u",(res & 0xFF) >> 5, (res & 0x1F) );
  lwm2m_data_free_ext(lwm2m_data->instance_info->no_resources, dataArray);
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

/*
 * @fn      static int32_t prv_cell_nw_conn_getvalue ()
 * @brief   This function is used to get the particular resource values for given instance 
 * @param   id_info      - indicates what values need to be filled whether resource or instance or object level 
 * @param   lwm2m_dataP  - pointer to lwm2m object information to be filled 
 * @param   targetP      - pointer to lwm2m object 
 * @return  on success  -  0
            on failure   - -1
 */

int32_t prv_cell_nw_conn_getvalue(lwm2m_id_info_t id_info, lwm2m_object_data_t **lwm2m_dataP ,
                              lwm2m_object_t * targetP,lwm2m_resource_info_t **resource_info)
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
  result = -1;

	result = prv_cell_nw_conn_read(instance_id, &numData, &dataArrayP, targetP,INVALID_RES_INST_ID, NULL);

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

lwm2m_object_t * get_object_cell_nw_conn(void)
{
  lwm2m_object_t * nwconnobj = NULL;
  cell_nw_conn_instance_t *instanceP = NULL;
  int fd = 0;

  nwconnobj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

  if(NULL != nwconnobj)
  {
    register_psm_cfg_change();
    memset(nwconnobj, 0, sizeof(lwm2m_object_t));

    /* It assigns his unique ID. The 11 is the standard ID for the mandatory 
       object "APN Connection Profile".*/
    nwconnobj->objID = LWM2M_CELL_NW_CONN_OBJECT_ID;

    /*
     * And the private function that will access the object.
     * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
     * know the resources of the object, only the server does.
     */
    nwconnobj->readFunc     = prv_cell_nw_conn_read;
    nwconnobj->discoverFunc = prv_cell_nw_conn_discover;
    nwconnobj->writeFunc    = prv_cell_nw_conn_write;
    nwconnobj->executeFunc  = NULL;
    nwconnobj->createFunc   = prv_cell_nw_conn_create;
    nwconnobj->deleteFunc   = prv_cell_nw_conn_delete;
    nwconnobj->get_Value_Func = prv_cell_nw_conn_getvalue;
    nwconnobj->setValueFunc   = prv_cell_nw_conn_setvalue;

    /*  Check if device_obj_single_instance_pr file exist */
    ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_CELL_NW_CONN_PERSISTENCE_FILE);
    fd = efs_open(LWM2M_CELL_NW_CONN_PERSISTENCE_FILE, O_RDONLY);
    if(fd != EFAILURE)
    {
       nwconnobj->userData = NULL;
       efs_close(fd);
       return nwconnobj;
    }
  
    instanceP = (cell_nw_conn_instance_t *)lwm2m_malloc(sizeof(cell_nw_conn_instance_t));
    if (instanceP != NULL)
    {
      memset(instanceP, 0, sizeof(cell_nw_conn_instance_t));
      instanceP->instanceId = 0;
	  nwconnobj->userData = instanceP;
      prv_cell_nw_conn_default_values(instanceP);


      timer_def_osal(&instanceP->ps_attach_timer, &lwm2mTimerGroup,
        TIMER_NATIVE_OS_SIGNAL_TYPE, &lwm2m_signal_Handle, LWM2M_PS_ATTACH);

	  nwconnobj->instanceList = LWM2M_LIST_ADD(nwconnobj->instanceList, instanceP);

    } 
    else
    { 
      lwm2m_free(nwconnobj);
	  nwconnobj = NULL;
    }
  }
  return nwconnobj;
}

/**
 * @brief This fucntion frees the Cell NW connectivity object allocated
 * @param ObjectP (IN) Object to free
 * @return void
 */ 
void free_object_cell_nw_conn(lwm2m_object_t * objectP)
{
  if( objectP == NULL )
  { 
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return ;
  }

  if(NULL != objectP->userData)
  {
    cell_nw_conn_instance_t * instanceP = (cell_nw_conn_instance_t *)objectP->userData;
    timer_clr(&instanceP->ps_attach_timer, T_MIN);

    timer_undef(&instanceP->ps_attach_timer);
    lwm2m_free(objectP->userData);
    objectP->userData = NULL;
  }
  lwm2m_free(objectP);
}


/**
 * @fn uint8_t cell_nw_change()
 *
 * @brief This fucntion is used to populate the values of the resource
 *        to be provided to the server during the read operation on the object.
 * @param dataArray data array for the resource 
 * @param objectP handle to device object 
 * @return LWM2M response type
 */
uint8_t cell_nw_change(lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP)
{
  uint8_t result = LWM2M_NO_ERROR;
  int64_t value = 0;
  if( dataArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  switch (dataArray->id)
  {
    case RES_O_PSM_TIMER:
      {

        if (1 == lwm2m_data_decode_int(dataArray, &value))
        {
          if ((600 <= value) && (85708800 >= value) && (value != ((cell_nw_conn_instance_t*)(objectP->userData))->psm_timer) )
          {
            ((cell_nw_conn_instance_t*)(objectP->userData))->psm_timer = value;
            ds_iot_ulog_high("PSM timer changed");
            result = LWM2M_204_CHANGED;
          }
          else
          {
            result = LWM2M_400_BAD_REQUEST;
          }
        }
        else
        {
          result = LWM2M_400_BAD_REQUEST;
        }
      }
      break;
    case RES_O_ACTIVE_TIMER:

		if (1 == lwm2m_data_decode_int(dataArray, &value))
		{
           if ((2 <= value) && (1860 >= value) && (value != ((cell_nw_conn_instance_t*)(objectP->userData))->active_timer) )
           {
              ((cell_nw_conn_instance_t*)(objectP->userData))->active_timer = value;
               ds_iot_ulog_high("Active timer changed");
               result = LWM2M_204_CHANGED;
           }
           else
           {
              result = LWM2M_400_BAD_REQUEST;
           }
	   }
	   else
       {
         result = LWM2M_400_BAD_REQUEST;
       }
      break;
	default:
      result = LWM2M_405_METHOD_NOT_ALLOWED;
      break;
  }

  return result;
 }


 /**
 * @brief  API to retrieve splmn_rate_control info for particular interface.
 *
 * @param[in]  handle  Handle received after sucessful conection 
 * @param[out] splmn_rate_control  splmn_rate_control info .
 *
 * @return See Section @xref{hdr:QAPIStatust}. \n
 * On success, QAPI_OK (0) is returned. Other value on error.
 */

 qapi_Status_t get_splmn_rate_control_by_apn_info(void *hndl,
                                                                 uint32_t *splmn_rate_control)
 {
   qapi_DSS_Hndl_t *curr_hndl = NULL;
   if(hndl == NULL || splmn_rate_control == NULL)
   {
      ds_iot_ulog_err(" LWM2M_LOG: NULL Error");
      return LWM2M_500_INTERNAL_SERVER_ERROR;
   }
 
   curr_hndl = (qapi_DSS_Hndl_t *)hndl;
 
   if(curr_hndl != NULL)
   {
     if (QAPI_OK != qapi_DSS_Get_Splmn_Rate_Control(curr_hndl, splmn_rate_control))
    {
       LOG_DEBUG("Failed to get the SPLMN rate control for hndl[%p]", curr_hndl);
       return QAPI_ERROR;
    }
   }
 
   return QAPI_OK;
 
 }

#endif //ENABLE_CELL_NW_CONN_OBJ


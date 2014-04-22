/******************************************************************************

  @file    object_server.c

  ---------------------------------------------------------------------------

  Copyright (c) 2016-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Julien Vermillard, Sierra Wireless
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    
 *******************************************************************************/

/*
 *  Resources:
 *
 *          Name         | ID | Operations | Instances | Mandatory |  Type   |  Range  | Units |
 *  Short ID             |  0 |     R      |  Single   |    Yes    | Integer | 1-65535 |       |
 *  Lifetime             |  1 |    R/W     |  Single   |    Yes    | Integer |         |   s   |
 *  Default Min Period   |  2 |    R/W     |  Single   |    No     | Integer |         |   s   |
 *  Default Max Period   |  3 |    R/W     |  Single   |    No     | Integer |         |   s   |
 *  Disable              |  4 |     E      |  Single   |    No     |         |         |       |
 *  Disable Timeout      |  5 |    R/W     |  Single   |    No     | Integer |         |   s   |
 *  Notification Storing |  6 |    R/W     |  Single   |    Yes    | Boolean |         |       |
 *  Binding              |  7 |    R/W     |  Single   |    Yes    | String  |         |       |
 *  Registration Update  |  8 |     E      |  Single   |    Yes    |         |         |       |
 *  APN link             | 10 |    R/W     |  Single   |    No     | Objlink |         |       |
 *  Trigger              | 21 |    R/W     |  Single   |    No     | Boolean |         |       |
 *  Preferred Transport  | 22 |    R/W     |  Single   |    No     | String  |         |       |
 * 
 */

#include "liblwm2m.h"
#include "internals.h"
#ifdef LWM2M_SERVER_OBJECT
#include "lwm2mclient.h"
#endif
#include "lwm2m_signals.h"
#include "object_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qapi_coap.h"
#include "math.h"
#include "ds_log.h"


extern bool g_carrier_cfg; 
extern qurt_signal_t lwm2m_signal_Handle;
extern time_t gRegRetryInitialTimeout;
extern time_t gRegRetrytimeoutExponent;
extern time_t gRegRetryMaxTimeout;
extern bool persis_regstatus_feat_enabled;
extern bool enable_apn_conn_obj;

typedef struct _server_persistent_info_
{
  char        binding[MAX_BINDING_SIZE];
  uint32_t    lifetime;
  uint32_t    defaultMinPeriod;
  uint32_t    defaultMaxPeriod;
  uint32_t    disableTimeout;
  uint16_t    instanceId;            // matches lwm2m_list_t::id
  uint16_t    shortServerId;
  bool        storing;
  bool        isValid;
  objlink_t   apnLink; 
  bool        trigger;
  char        prefTransport[MAX_PREFERRED_TRANSPORT_SIZE]; 
  pers_resource_instance_int_list_t custom30000List[MAX_CUST_30000_RES_INSTANCES];
  uint8_t     res_set;            // Flag to check which resources are set.
  uint16_t    priority;           // Server priority 
  uint32_t    initial_delay_timer;// Time to wait before attempting first registration 
  bool        reg_failure;        // Blocking or Non-Blocking server
  bool        bs_on_failure;      // To perform rebootstraping or not in case of failure
  uint8_t     retry_count;        // No of attempts in one communication sequence.
  uint32_t    retry_timer;        // Initial timeout used for backoffs.
  uint32_t    seq_delay_timer;    // Delay between two communication sequence.
  uint8_t     seq_retry_count;    // No of communication sequence to be attempted.
} server_persistent_info_t;

#define REG_RETRY_NEW_RESOURCES          11
static bool create_operation = LWM2M_FALSE;

int load_server_persistent_info(lwm2m_object_t *serObjP)
{
  int fd = 0, ret = 0, index = 0;
  server_instance_t *targetP = NULL;
  server_persistent_info_t info;
  resource_instance_int_list_t *resList = NULL;

  if (NULL == serObjP)
  {
    LOG_DEBUG("%s", nullError);
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",SERVER_PERSISTENCE_FILE);
  fd = efs_open(SERVER_PERSISTENCE_FILE, 00);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open server persistence file");
    return EFAILURE;
  }

  while (1)
  {
    memset(&info, 0x00, sizeof(info));
    targetP = NULL;

    ret = efs_read(fd, &info, sizeof(info));
    if (ret <= 0)
    {
      break;
    }
    if (LWM2M_FALSE == info.isValid)
    {
      continue;
    }

    targetP = lwm2m_calloc(1, sizeof(server_instance_t));
    if (NULL == targetP)
    {
      ret = -1;
      break;
    }

    targetP->lifetime = info.lifetime;
    targetP->defaultMinPeriod = info.defaultMinPeriod;
    targetP->defaultMaxPeriod = info.defaultMaxPeriod;
    targetP->disableTimeout = info.disableTimeout;
    targetP->instanceId = info.instanceId;
    targetP->shortServerId = info.shortServerId;
    targetP->storing = info.storing;
    targetP->apnLink = info.apnLink;
    targetP->trigger = info.trigger;
    targetP->res_set = info.res_set;
    targetP->priority = info.priority;
    targetP->initial_delay_timer = info.initial_delay_timer;
    targetP->reg_failure = info.reg_failure;
    targetP->bs_on_failure = info.bs_on_failure;
    targetP->retry_count = info.retry_count;
    targetP->retry_timer = info.retry_timer;
    targetP->seq_delay_timer = info.seq_delay_timer;
    targetP->seq_retry_count = info.seq_retry_count;
     
    strlcpy(targetP->binding, info.binding, MAX_BINDING_SIZE); 
    strlcpy(targetP->prefTransport, info.prefTransport, MAX_PREFERRED_TRANSPORT_SIZE);

    ds_iot_ulog_high2(" Lifetime of SSID: %d is %d",targetP->shortServerId,targetP->lifetime);
    if(g_carrier_cfg) 
    {
      for (index = 0; index < MAX_CUST_30000_RES_INSTANCES; index++)
      {
        resList = NULL;
        if (LWM2M_FALSE == info.custom30000List[index].isValid)
          continue;
        resList = lwm2m_calloc(1, sizeof(resource_instance_int_list_t));
        if (NULL == resList)
        {
          ret = -1;
          break;
        }
        resList->resInstId = info.custom30000List[index].resInstId;
        resList->InstValue= info.custom30000List[index].InstValue;
        targetP->custom30000List = (resource_instance_int_list_t *)LWM2M_LIST_ADD(targetP->custom30000List, resList);
      }
    }

    serObjP->instanceList = LWM2M_LIST_ADD(serObjP->instanceList, targetP);
  }

  if (ret < 0)
  {
    efs_close(fd);
    clean_server_object(serObjP);
    return EFAILURE;
  }

  efs_close(fd);
  return ESUCCESS;
}


int store_server_persistent_info(server_instance_t *targetP,  boolean store)
{
  int fd = 0, index=0;
  server_persistent_info_t info;
  resource_instance_int_list_t *resList;
 

  if (NULL == targetP)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",SERVER_PERSISTENCE_FILE);
  fd = efs_open(SERVER_PERSISTENCE_FILE, O_CREAT | O_WRONLY, 00644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open server persistence file");
    return EFAILURE;
  }

  memset(&info, 0x00, sizeof(info));
 
  info.lifetime = targetP->lifetime;
  info.defaultMinPeriod = targetP->defaultMinPeriod;
  info.defaultMaxPeriod = targetP->defaultMaxPeriod;
  info.disableTimeout = targetP->disableTimeout;
  info.instanceId = targetP->instanceId;
  info.shortServerId = targetP->shortServerId;
  info.storing = targetP->storing;
  info.apnLink = targetP->apnLink;
  info.trigger = targetP->trigger;
  strlcpy(info.binding, targetP->binding, MAX_BINDING_SIZE);
  strlcpy(info.prefTransport, targetP->prefTransport, MAX_PREFERRED_TRANSPORT_SIZE); 
  info.res_set = targetP->res_set;
  info.priority = targetP->priority;
  info.initial_delay_timer = targetP->initial_delay_timer;
  info.reg_failure = targetP->reg_failure;
  info.bs_on_failure = targetP->bs_on_failure;
  info.retry_count = targetP->retry_count;
  info.retry_timer = targetP->retry_timer;
  info.seq_delay_timer = targetP->seq_delay_timer;
  info.seq_retry_count = targetP->seq_retry_count;

  if(store)
  {
    info.isValid = LWM2M_TRUE; 
  }
  else
  {
    info.isValid = LWM2M_FALSE; 
  }

  if(g_carrier_cfg)
  {
    resList = targetP->custom30000List;
    while (resList)
    {
      info.custom30000List[index].isValid = LWM2M_TRUE;
      info.custom30000List[index].resInstId = resList->resInstId;
      info.custom30000List[index++].InstValue = resList->InstValue;
      resList = resList->next;
    }
    while (index < MAX_CUST_30000_RES_INSTANCES)
    {
      info.custom30000List[index++].isValid = LWM2M_FALSE;
    }
  }

  efs_lseek(fd, info.instanceId * sizeof(info), SEEK_SET);
  ds_iot_log_strf_med("EFS_OPT: File write %s",SERVER_PERSISTENCE_FILE);
  if (EFAILURE == efs_write(fd, &info, sizeof(info)))
  {
    ds_iot_ulog_err1("failed to store persistent value of server instance %u", info.instanceId);
    efs_close(fd);
    return EFAILURE;
  }
  efs_close(fd);
  return ESUCCESS;
}

/** 
* @fn static uint8_t prv_server_create_write_check() 
*
* @brief This fucntion verifies and write the resource's into  
*        Server object 
*
* @param instanceId Instance of the server object to be create/write 
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
static uint8_t prv_server_create_write_check(int numData,
                                               lwm2m_data_t * dataArray,
                                               bool create_flag, bool version)
{ 
  int16_t i = 0;
  uint8_t result = LWM2M_NO_ERROR; 
  if((NULL == dataArray) )
  { 
    LOG_DEBUG("%s",nullError); 
    return LWM2M_500_INTERNAL_SERVER_ERROR; 
  } 
  i = 0;
  do
  {
    switch(dataArray[i].id)
    {
      case LWM2M_SERVER_DISABLE_ID:
      case LWM2M_SERVER_UPDATE_ID:
      case LWM2M_SERVER_SHORT_ID_ID:
      if (!create_flag)
      {
         result = LWM2M_405_METHOD_NOT_ALLOWED;
         break;
      }
      else
      {
         result = LWM2M_204_CHANGED;
         break;
      }
      case LWM2M_SERVER_REG_PRI_ORD_ID:
      case LWM2M_SERVER_INITIAL_DELAY_TIMER_ID:
      case LWM2M_SERVER_REG_FAILURE_BLOCK_ID:
      case LWM2M_SERVER_BS_ON_REG_FAILURE_ID:
      if (!create_flag)
      {
        if(version == TRUE)
          result = LWM2M_405_METHOD_NOT_ALLOWED;
        else
          result = LWM2M_204_CHANGED;
        break;
      }
      else
      {
         result = LWM2M_204_CHANGED;
         break;
      }

      case LWM2M_SERVER_RESOURCE_30000:
      if(!g_carrier_cfg)
      {
        result = LWM2M_405_METHOD_NOT_ALLOWED;
        break;
      }
      case LWM2M_SERVER_LIFETIME_ID:
      case LWM2M_SERVER_MIN_PERIOD_ID:
      case LWM2M_SERVER_MAX_PERIOD_ID:
      case LWM2M_SERVER_TIMEOUT_ID:
      case LWM2M_SERVER_STORING_ID:
      case LWM2M_SERVER_BINDING_ID:
      case LWM2M_SERVER_APN_LINK_ID:
      case LWM2M_SERVER_TRIGGER_ID: 
      case LWM2M_SERVER_PREFERRED_TRANSPORT_ID:
      case LWM2M_SERVER_COMM_RETRY_COUNT_ID:
      case LWM2M_SERVER_COMM_RETRY_TIMER_ID:
      case LWM2M_SERVER_COMM_SEQ_DELAY_TIMER_ID:
      case LWM2M_SERVER_COMM_SEQ_RETRY_COUNT_ID:
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
 * @fn static uint8_t prv_get_value()
 *
 * @brief This fucntion is used to populate the values of the resource
 *        to be provided to the server during the read operation on the object.
 * @param dataP data array for the resource 
 * @param targetP Security object data
 * @return LWM2M response type
 */
static uint8_t prv_get_value(lwm2m_data_t * dataP,
    server_instance_t * targetP, uint16_t resInstId, bool read_instance, float version)
{
  if( dataP == NULL || targetP == NULL ) 
  {
    LOG_DEBUG("fun [%s] file [%s]  %s",__func__,__FILE__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  switch (dataP->id)
  {
    case LWM2M_SERVER_SHORT_ID_ID:
      lwm2m_data_encode_int(targetP->shortServerId, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SERVER_LIFETIME_ID:
    {
      lwm2m_data_encode_int(targetP->lifetime, dataP);
      ds_iot_ulog_high2(" Lifetime of SSID: %d is %d",targetP->shortServerId,targetP->lifetime);
      return LWM2M_205_CONTENT; 
    }

    case LWM2M_SERVER_MIN_PERIOD_ID:
      lwm2m_data_encode_int(targetP->defaultMinPeriod, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SERVER_MAX_PERIOD_ID:
      lwm2m_data_encode_int(targetP->defaultMaxPeriod, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SERVER_DISABLE_ID:
      return LWM2M_405_METHOD_NOT_ALLOWED;

    case LWM2M_SERVER_TIMEOUT_ID:
      lwm2m_data_encode_int(targetP->disableTimeout, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SERVER_STORING_ID:
      lwm2m_data_encode_bool(targetP->storing, dataP);
      return LWM2M_205_CONTENT;

    case LWM2M_SERVER_BINDING_ID:
      lwm2m_data_encode_string(targetP->binding, dataP);
      return LWM2M_205_CONTENT;
    
    case LWM2M_SERVER_APN_LINK_ID:
    { 
      /* Internal consumption when we refresh the server list
       * Read is performed at object/instance level for LwM2Mv1.0 client
       * Read is performed at object/instance/resource level for LwM2Mv1.1 client or higher
       */    
       if(version == (float)LWM2M_V_1_0)
         return LWM2M_404_NOT_FOUND;
       else
       {
         lwm2m_data_encode_objLink(targetP->apnLink, dataP);
         return LWM2M_205_CONTENT;
       }
    }

    case LWM2M_SERVER_TRIGGER_ID:
    {
      /* Internal consumption when we refresh the server list
       * Read is performed at object/instance level for LwM2Mv1.0 client
       * Read is performed at object/instance/resource level for LwM2Mv1.1 client or higher
       */
       if(version == (float)LWM2M_V_1_0)
         return LWM2M_404_NOT_FOUND;
       else
       {
         lwm2m_data_encode_bool(targetP->trigger, dataP);
         return LWM2M_205_CONTENT;
       }
    }

    case LWM2M_SERVER_PREFERRED_TRANSPORT_ID:
    {
      /* Internal consumption when we refresh the server list
       * Read is performed at object/instance level for LwM2Mv1.0 client
       * Read is performed at object/instance/resource level for LwM2Mv1.1 client or higher
       */
       if(version == (float)LWM2M_V_1_0)
         return LWM2M_404_NOT_FOUND;
       else
       {
         lwm2m_data_encode_string(targetP->prefTransport, dataP);
         return LWM2M_205_CONTENT;
       }
    }

    case LWM2M_SERVER_UPDATE_ID:
      return LWM2M_405_METHOD_NOT_ALLOWED;

    case LWM2M_SERVER_RESOURCE_30000 :
      {
        if(g_carrier_cfg){
          if (resInstId != INVALID_RES_INST_ID)
          {
            resource_instance_int_list_t *resInst = (resource_instance_int_list_t *)LWM2M_LIST_FIND(targetP->custom30000List,resInstId);
            if(resInst != NULL)
            {
              lwm2m_data_encode_int(resInst->InstValue, dataP);
              return LWM2M_205_CONTENT;
            }
            else
              return LWM2M_404_NOT_FOUND;
          }
          else {
            int ri = 0;
            resource_instance_int_list_t* custom30000RI = NULL;
            for (custom30000RI = targetP->custom30000List, ri=0;
                custom30000RI!=NULL;
                custom30000RI = custom30000RI->next, ri++);

            if (ri==0)  // no values!
            {
              return LWM2M_205_CONTENT;
            }
            else
            {
              lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
              if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
              for (custom30000RI = targetP->custom30000List, ri = 0;
                  custom30000RI!= NULL;
                  custom30000RI = custom30000RI->next, ri++)
              {
                subTlvP[ri].id = (uint16_t)custom30000RI->resInstId;

                lwm2m_data_encode_int(custom30000RI->InstValue, &subTlvP[ri]);
              }
              lwm2m_data_encode_instances(subTlvP, ri, dataP);
              return LWM2M_205_CONTENT;
            }
          }
        }
        else
          return LWM2M_404_NOT_FOUND;
      }

      case LWM2M_SERVER_REG_PRI_ORD_ID:
      {
        if(version == (float)LWM2M_V_1_0)
         return LWM2M_404_NOT_FOUND;
        else
        {
          if(targetP->priority == 0xFFFF)
            return LWM2M_404_NOT_FOUND;
          else
          {
            lwm2m_data_encode_int(targetP->priority, dataP);
            return LWM2M_205_CONTENT;
          }
        }
      }

      case LWM2M_SERVER_INITIAL_DELAY_TIMER_ID:
      {
        if(version == (float)LWM2M_V_1_0)
         return LWM2M_404_NOT_FOUND;
        else
        {
          lwm2m_data_encode_int(targetP->initial_delay_timer, dataP);
          return LWM2M_205_CONTENT;
        }
      }

      case LWM2M_SERVER_REG_FAILURE_BLOCK_ID:
      {
        if(version == (float)LWM2M_V_1_0)
         return LWM2M_404_NOT_FOUND;
        else
        {
          lwm2m_data_encode_bool(targetP->reg_failure, dataP);
          return LWM2M_205_CONTENT;
        }
      }

      case LWM2M_SERVER_BS_ON_REG_FAILURE_ID:
      {
        if(version == (float)LWM2M_V_1_0)
         return LWM2M_404_NOT_FOUND;
        else
        {
          lwm2m_data_encode_bool(targetP->bs_on_failure, dataP);
          return LWM2M_205_CONTENT;
        }
      }

      case LWM2M_SERVER_COMM_RETRY_COUNT_ID:
      {
        if(version == (float)LWM2M_V_1_0)
         return LWM2M_404_NOT_FOUND;
        else
        {
          lwm2m_data_encode_int(targetP->retry_count, dataP);
          return LWM2M_205_CONTENT;
        }
      }

      case LWM2M_SERVER_COMM_RETRY_TIMER_ID:
      {
        if(version == (float)LWM2M_V_1_0)
         return LWM2M_404_NOT_FOUND;
        else
        {
          lwm2m_data_encode_int(targetP->retry_timer, dataP);
          return LWM2M_205_CONTENT;
        }
      }

      case LWM2M_SERVER_COMM_SEQ_DELAY_TIMER_ID:
      {
        if(version == (float)LWM2M_V_1_0)
         return LWM2M_404_NOT_FOUND;
        else
        {
          lwm2m_data_encode_int(targetP->seq_delay_timer, dataP);
          return LWM2M_205_CONTENT;
        }
      }

      case LWM2M_SERVER_COMM_SEQ_RETRY_COUNT_ID:
      {
        if(version == (float)LWM2M_V_1_0)
         return LWM2M_404_NOT_FOUND;
        else
        {
          lwm2m_data_encode_int(targetP->seq_retry_count, dataP);
          return LWM2M_205_CONTENT;
        }
      }

    default:
      return LWM2M_404_NOT_FOUND;
  }
}


/**
 * @fn static uint8_t prv_server_read()
 *
 * @brief This function implements the read operation performed on Server Object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_server_read(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP,
    uint16_t resInstId, lwm2m_server_t *serverP)
{
  server_instance_t * targetP = NULL;
  uint8_t result = 0;
  int i = 0, ri = 0, pri = 0;
  bool read_instance = false;
  resource_instance_int_list_t *custom30000RI;
  lwm2m_context_t *contextP = NULL;
  lwm2m_server_t *target_serlistP = NULL;

  if( dataArrayP == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_server_read] LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
  if (NULL == targetP) return 
    LWM2M_404_NOT_FOUND;

  // is the server asking for the full instance ?

  contextP = get_lwm2m_context();
  if (NULL != contextP)
  {
    target_serlistP = (lwm2m_server_t *)LWM2M_LIST_FIND(contextP->serverList, targetP->shortServerId);
  
    if(target_serlistP == NULL)
      target_serlistP = (lwm2m_server_t *)LWM2M_LIST_FIND(contextP->bootstrapServer, targetP->shortServerId);
  }
    
  if (*numDataP == 0)
  {
    uint16_t resList[] = {
      LWM2M_SERVER_SHORT_ID_ID,
      LWM2M_SERVER_LIFETIME_ID,
      LWM2M_SERVER_MIN_PERIOD_ID,
      LWM2M_SERVER_MAX_PERIOD_ID,
      LWM2M_SERVER_TIMEOUT_ID,
      LWM2M_SERVER_STORING_ID,
      LWM2M_SERVER_BINDING_ID
    };
    int nbRes = sizeof(resList)/sizeof(uint16_t);
    int temp = nbRes;
      
    read_instance = true; 

    if(target_serlistP)
    {
      if(target_serlistP->version > (float)LWM2M_V_1_0)
      {
        if(target_serlistP->pri_order != UNDEFINED_PRIORITY)
        {
          nbRes += REG_RETRY_NEW_RESOURCES;
          pri++;
        }
        else
        {
          nbRes += (REG_RETRY_NEW_RESOURCES - 1);
        }
      }
    }
    if(g_carrier_cfg) 
    {
      for (custom30000RI = targetP->custom30000List, ri=0;
          custom30000RI!=NULL;
          custom30000RI = custom30000RI->next, ri++);
      if(ri!=0)
        nbRes++;
    }

    *dataArrayP = lwm2m_data_new(nbRes);

    if (*dataArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;

    *numDataP = nbRes;

    for (i = 0 ; i < temp ; i++)
    {
      (*dataArrayP)[i].id = resList[i];
    }

    if(target_serlistP)
    {
      if(target_serlistP->version > (float)LWM2M_V_1_0)
      {
        (*dataArrayP)[temp++].id = LWM2M_SERVER_APN_LINK_ID;
        if(pri != 0)
        {
          (*dataArrayP)[temp++].id = LWM2M_SERVER_REG_PRI_ORD_ID;
        }
        (*dataArrayP)[temp++].id = LWM2M_SERVER_INITIAL_DELAY_TIMER_ID;
        (*dataArrayP)[temp++].id = LWM2M_SERVER_REG_FAILURE_BLOCK_ID;
        (*dataArrayP)[temp++].id = LWM2M_SERVER_BS_ON_REG_FAILURE_ID;
        (*dataArrayP)[temp++].id = LWM2M_SERVER_COMM_RETRY_COUNT_ID;
        (*dataArrayP)[temp++].id = LWM2M_SERVER_COMM_RETRY_TIMER_ID;
        (*dataArrayP)[temp++].id = LWM2M_SERVER_COMM_SEQ_DELAY_TIMER_ID;
        (*dataArrayP)[temp++].id = LWM2M_SERVER_COMM_SEQ_RETRY_COUNT_ID;
        (*dataArrayP)[temp++].id = LWM2M_SERVER_TRIGGER_ID;
        (*dataArrayP)[temp++].id = LWM2M_SERVER_PREFERRED_TRANSPORT_ID;
      }
    }
    if ((g_carrier_cfg) && (ri!=0))
    {
      (*dataArrayP)[temp].id = LWM2M_SERVER_RESOURCE_30000 ;
    }

  }

  i = 0;
  do
  {
    if(target_serlistP)
    {
      result = prv_get_value((*dataArrayP) + i, targetP, resInstId, read_instance, target_serlistP->version);
    }
    else
    {
      result = prv_get_value((*dataArrayP) + i, targetP, resInstId, read_instance, (float)LWM2M_V_1_1);
    }
    i++;
  } while (i < *numDataP && result == LWM2M_205_CONTENT);

  return result;
}


/**
 * @fn static uint8_t prv_server_discover()
 *
 * @brief This fucntion implements the discover operation performed on the
 *     Server object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_server_discover(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP, lwm2m_server_t *serverP)
{
  uint8_t result = LWM2M_205_CONTENT;
  server_instance_t * targetP;
  resource_instance_int_list_t*    custResource;
  int i = 0, ri = 0, pri = 0;
  lwm2m_context_t *contextP = NULL;
  lwm2m_server_t *target_serlistP = NULL;

  if ( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  targetP = (server_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, instanceId);
  if (NULL == targetP) return  LWM2M_404_NOT_FOUND;
  contextP = get_lwm2m_context();
  if (NULL != contextP)
  {
    target_serlistP = (lwm2m_server_t *)LWM2M_LIST_FIND(contextP->serverList, targetP->shortServerId);
  
    if(target_serlistP == NULL)
      target_serlistP = (lwm2m_server_t *)LWM2M_LIST_FIND(contextP->bootstrapServer, targetP->shortServerId);
  }
  if(target_serlistP && (target_serlistP->version > (float)LWM2M_V_1_0))
  {
    if(target_serlistP->pri_order != 0xFFFF)
    pri++;
  }
  
  if(g_carrier_cfg)
  {
    if (targetP != NULL)
    {
      for (custResource = targetP->custom30000List, ri=0;
        custResource!=NULL;
        custResource = custResource->next, ri++);
    }
  }

  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {
      LWM2M_SERVER_SHORT_ID_ID,
      LWM2M_SERVER_LIFETIME_ID,
      LWM2M_SERVER_MIN_PERIOD_ID,
      LWM2M_SERVER_MAX_PERIOD_ID,
      LWM2M_SERVER_DISABLE_ID,
      LWM2M_SERVER_TIMEOUT_ID,
      LWM2M_SERVER_STORING_ID,
      LWM2M_SERVER_BINDING_ID,
      LWM2M_SERVER_UPDATE_ID
    };
    int nbRes = sizeof(resList) / sizeof(uint16_t);

    int temp = nbRes;

    if(target_serlistP && (target_serlistP->version > (float)LWM2M_V_1_0))
    {
      if(pri != 0)
        nbRes+= REG_RETRY_NEW_RESOURCES;
      else
        nbRes+= (REG_RETRY_NEW_RESOURCES - 1);
    }
    if (ri != 0)  // no values!
      nbRes++;

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;
    *numDataP = nbRes;
    for (i = 0; i < temp; i++)
    {
      (*dataArrayP)[i].id = resList[i];
    }
    if(target_serlistP && (target_serlistP->version > (float)LWM2M_V_1_0))
    {
      (*dataArrayP)[temp++].id = LWM2M_SERVER_APN_LINK_ID;
      if(pri != 0)
      {
        (*dataArrayP)[temp++].id = LWM2M_SERVER_REG_PRI_ORD_ID;
      }
      (*dataArrayP)[temp++].id = LWM2M_SERVER_INITIAL_DELAY_TIMER_ID;
      (*dataArrayP)[temp++].id = LWM2M_SERVER_REG_FAILURE_BLOCK_ID;
      (*dataArrayP)[temp++].id = LWM2M_SERVER_BS_ON_REG_FAILURE_ID;
      (*dataArrayP)[temp++].id = LWM2M_SERVER_COMM_RETRY_COUNT_ID;
      (*dataArrayP)[temp++].id = LWM2M_SERVER_COMM_RETRY_TIMER_ID;
      (*dataArrayP)[temp++].id = LWM2M_SERVER_COMM_SEQ_DELAY_TIMER_ID;
      (*dataArrayP)[temp++].id = LWM2M_SERVER_COMM_SEQ_RETRY_COUNT_ID;
      (*dataArrayP)[temp++].id = LWM2M_SERVER_TRIGGER_ID;
      (*dataArrayP)[temp++].id = LWM2M_SERVER_PREFERRED_TRANSPORT_ID;
    }

    if ((g_carrier_cfg) && (ri != 0))  // no values!
    {
      (*dataArrayP)[temp].id = LWM2M_SERVER_RESOURCE_30000;
      (*dataArrayP)[temp].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
      (*dataArrayP)[temp].value.asChildren.count = ri;
    }

  }
  else
  {
    for (i = 0; i < *numDataP && result == LWM2M_205_CONTENT; i++)
    {
      switch ((*dataArrayP)[i].id)
      {
        case LWM2M_SERVER_RESOURCE_30000:
        {
          if (g_carrier_cfg)
          {
            (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
            (*dataArrayP)[i].value.asChildren.count = ri;
          }
          else
            result = LWM2M_404_NOT_FOUND;
          break;
        }
        case LWM2M_SERVER_SHORT_ID_ID:
        case LWM2M_SERVER_LIFETIME_ID:
        case LWM2M_SERVER_MIN_PERIOD_ID:
        case LWM2M_SERVER_MAX_PERIOD_ID:
        case LWM2M_SERVER_DISABLE_ID:
        case LWM2M_SERVER_TIMEOUT_ID:
        case LWM2M_SERVER_STORING_ID:
        case LWM2M_SERVER_BINDING_ID:
        case LWM2M_SERVER_UPDATE_ID:
          break;
        case LWM2M_SERVER_REG_PRI_ORD_ID:
          if(!(target_serlistP && (target_serlistP->version > (float)LWM2M_V_1_0 )&& pri != 0))
          result = LWM2M_404_NOT_FOUND;
        break;
        case LWM2M_SERVER_INITIAL_DELAY_TIMER_ID:
        case LWM2M_SERVER_REG_FAILURE_BLOCK_ID:
        case LWM2M_SERVER_BS_ON_REG_FAILURE_ID:
        case LWM2M_SERVER_COMM_RETRY_COUNT_ID:
        case LWM2M_SERVER_COMM_RETRY_TIMER_ID:
        case LWM2M_SERVER_COMM_SEQ_DELAY_TIMER_ID:
        case LWM2M_SERVER_COMM_SEQ_RETRY_COUNT_ID:
        case LWM2M_SERVER_APN_LINK_ID:
        case LWM2M_SERVER_TRIGGER_ID:
        case LWM2M_SERVER_PREFERRED_TRANSPORT_ID:
          if(target_serlistP && (target_serlistP->version == (float)LWM2M_V_1_0))
            result = LWM2M_404_NOT_FOUND;
        break;
        default:
          result = LWM2M_404_NOT_FOUND;
      }
    }
  }

  return result;
}


/**
 * @fn static uint8_t prv_set_int_value()
 *
 * @brief This fucntion is used to set an integer value from server object
 *
 * @param dataArray handle to resource value
 * @param data pointer to store the resource value
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_set_int_value(lwm2m_data_t * dataArray, uint32_t * data, bool *val_changed) 
{

  uint8_t result = LWM2M_205_CONTENT;
  int64_t value = 0;  
  if( dataArray == NULL || data == NULL  || val_changed == NULL) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  if (1 == lwm2m_data_decode_int(dataArray, &value))
  {
    if (value >= 0 && value <= 0xFFFFFFFF)
    {
      if(LWM2M_SERVER_LIFETIME_ID == dataArray->id && 0 == value)
        result = LWM2M_400_BAD_REQUEST;
      else
      {
        if(*data != value)
        {
          *val_changed = true;
        }
        *data = value;
        result = LWM2M_204_CHANGED;
      }
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
  return result;
}


/**
 * @brief This function is used to write the resources into server object instance
 *        while creating the object instance.or got write request from server.

 * @param   objectP (IN)   handle to object
 * @param   instanceP (IN/OUT)   handle to object instance
 * @param   numData (IN)        number of resources to be written
 * @param   dataArray (IN)      array of resource values
 * @param resInstId Resource instance id or INVALID_RES_INST_ID 
 *                  if resource instance id is not passed.
 * @param write_method To distinguish between partial write or replace.
 * @return LWM2M response type
 */
static uint8_t prv_server_write_resources(lwm2m_object_t * objectP,
                                            server_instance_t *targetP,
    int numData,
                                            lwm2m_data_t *dataArray,
    uint16_t resInstId, uint8_t write_method)
{
  int i = 0;
  uint8_t result = LWM2M_205_CONTENT;
  lwm2m_server_t *target_serlistP = NULL;
  lwm2m_context_t *contextP = NULL;
  bool val_changed = false;

  if ((NULL == dataArray) || 
      (NULL == objectP) || (NULL == targetP)) 
  {
    LOG_DEBUG("Invalid parameters - dataArray[0x%x], objectP[0x%x]", dataArray, objectP);
    return LWM2M_500_INTERNAL_SERVER_ERROR;     
  }

  /* Don't throw any errors if the server list is not valid 
			*       - Context and server list information is not populated on device boot-up
   *       - Bootstrap server may delete all the existing factory provisioned servers in the bootstrap phase 
   */
  contextP = get_lwm2m_context();
  if (NULL != contextP)
    target_serlistP = (lwm2m_server_t *)lwm2m_list_find((lwm2m_list_t *)contextP->serverList, targetP->shortServerId);
   
  do
  {
    switch (dataArray[i].id)
    {
      case LWM2M_SERVER_SHORT_ID_ID:
      {
        uint32_t value = targetP->shortServerId;

        result = prv_set_int_value(dataArray + i, &value, &val_changed);
        if (LWM2M_204_CHANGED == result)
        {
          if (0 < value && 0xFFFF > value)
            targetP->shortServerId = value;
          else
          {
            result = LWM2M_400_BAD_REQUEST;
            ds_iot_ulog_high2(" Invalid SSID %d result - %d", value, result);
          }
        }

        break;
      }
      
      case LWM2M_SERVER_LIFETIME_ID:
      {
        qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
        result = prv_set_int_value(dataArray + i, (uint32_t *)&(targetP->lifetime), &val_changed);

        if(result != LWM2M_204_CHANGED)
        {
          qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
          ds_iot_ulog_high1(" Couldn't set Lifetime result - %d", result);
          break;
        }

        ds_iot_ulog_high2(" Lifetime of SSID: %d is %d", targetP->shortServerId, targetP->lifetime);

        if ((target_serlistP) && 
            (target_serlistP->lifetime != targetP->lifetime))
        {
          target_serlistP->lifetime = targetP->lifetime;
            
#ifdef LWM2M_SERVER_OBJECT
          {
            LWM2M_SERVER_SET_UPDATE_ID(updt_disabl_flag);
            LWM2M_SERVER_SET_UPDATE_LIFETIME(updt_disabl_flag);
            short_serverId = targetP->shortServerId;
          }
#endif        
          qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
  
          /* Indicate the application that the server life time has been updated */ 
          if (lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_LIFETIME_UPDATE_E) != QAPI_OK)
            ds_iot_ulog_err("Indicating the applications of the life time update from server failed.");          
  
          ds_iot_ulog_high("disable signal set for lifetime");
          qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
        }
        else
        {
#ifdef LWM2M_SERVER_OBJECT
            short_serverId = targetP->shortServerId;
#endif
          qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
          ds_iot_ulog_high2(" Lifetime of SSID: %d is %d is same as previous.",targetP->shortServerId,targetP->lifetime);
        }

        break;
      }

      case LWM2M_SERVER_MIN_PERIOD_ID:
      {
        int64_t value = 0;  

        lwm2m_data_decode_int(dataArray+i, &value);
      
        result = prv_set_int_value(dataArray + i, &(targetP->defaultMinPeriod), &val_changed);
        if (result != LWM2M_204_CHANGED)
        {
          ds_iot_ulog_high2(" Couldn't set MIN period %d result - %d", value, result);
          break;
        }

        if (target_serlistP)
          target_serlistP->defaultMinPeriod = targetP->defaultMinPeriod;
 
#ifdef LWM2M_SERVER_OBJECT
        short_serverId = targetP->shortServerId; 
#endif
        qurt_signal_set(&lwm2m_signal_Handle, LOCATION_PMIN_UPDATED);        
        break;
      }

      case LWM2M_SERVER_MAX_PERIOD_ID:
      {
        int64_t value = 0; 

        lwm2m_data_decode_int(dataArray + i, &value);

        result = prv_set_int_value(dataArray + i, &(targetP->defaultMaxPeriod), &val_changed);
        if (result != LWM2M_204_CHANGED)
          break;

        if (target_serlistP)
          target_serlistP->defaultMaxPeriod = targetP->defaultMaxPeriod; 
        
#ifdef LWM2M_SERVER_OBJECT
        short_serverId = targetP->shortServerId; 
#endif 
        qurt_signal_set(&lwm2m_signal_Handle, LOCATION_PMIN_UPDATED);        
        break;
      }

      case LWM2M_SERVER_DISABLE_ID:
      {
        result = LWM2M_405_METHOD_NOT_ALLOWED;
        break;
      }

      case LWM2M_SERVER_TIMEOUT_ID:
      {
        result = prv_set_int_value(dataArray + i, &(targetP->disableTimeout), &val_changed);
        if(result != LWM2M_204_CHANGED)
        {
          ds_iot_ulog_high1(" Couldn't set server timeout result - %d", result);
          break;
        }

        if (target_serlistP)
          target_serlistP->disable_timeOut = targetP->disableTimeout; 

        break;
      }  

      case LWM2M_SERVER_STORING_ID:
      {
        bool value = false;

        if (!lwm2m_data_decode_bool(dataArray + i, &value))
        {
          ds_iot_ulog_high(" Couldn't set server storing id 400 result ");

          return LWM2M_400_BAD_REQUEST;
        }
        if(targetP->storing != value)
        {
          val_changed = true;
        }
        targetP->storing = value;

        if (target_serlistP)
          target_serlistP->storing = value; 

        result = LWM2M_204_CHANGED;        
        break;
      }
        
      case LWM2M_SERVER_BINDING_ID:
      {
        if (((dataArray[i].type == LWM2M_TYPE_STRING) || 
             (dataArray[i].type == LWM2M_TYPE_OPAQUE)) &&
            ((dataArray[i].value.asBuffer.length > 0) && 
             (dataArray[i].value.asBuffer.length < MAX_BINDING_SIZE)))
        {     
          lwm2m_binding_t binding_mode = BINDING_UNKNOWN;
          
          /* Retrieve the bindings information */ 
          binding_mode |= utils_stringToBinding(dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);
     
          /* Skip validation for bootstrap.ini config. Server list is not populated when initializing object information 
           * from boostrap.ini on device boot-up (use that as a hint).
           *
           * Validate the bindings received from the registration server.   
           */  
          if (target_serlistP)  
          { 
            if ((BINDING_UNKNOWN == binding_mode) ||
                (((float)LWM2M_V_1_0 == target_serlistP->version) &&
                 ((binding_mode & BINDING_N) ||
                  (binding_mode & BINDING_T))) ||
                ((target_serlistP->version >= (float)LWM2M_V_1_1) &&
                 (binding_mode & BINDING_Q)))
            {
              ds_iot_ulog_high(" Couldn't set server binding 400 result ");

              return LWM2M_400_BAD_REQUEST;
            }
          }

          /* Binding changes */ 
          if ((target_serlistP) &&
              (target_serlistP->binding == binding_mode))
          {
            result = LWM2M_204_CHANGED;

            short_serverId = targetP->shortServerId;

            ds_iot_ulog_high2(" Binding mode of SSID[%d] is %d - same as previous value", targetP->shortServerId, targetP->binding);
            break;
          }

          val_changed = true;
#ifdef LWM2M_SERVER_OBJECT
          /* Send the "Register Update" on bindings change only for servers supporting version LwM2Mv1.0 */
          if ((target_serlistP) &&
              (float)LWM2M_V_1_0 == target_serlistP->version) 
          {
            {
              LWM2M_SERVER_SET_UPDATE_ID(updt_disabl_flag);
              LWM2M_SERVER_SET_UPDATE_BINDING(updt_disabl_flag);

              short_serverId = targetP->shortServerId;
            }
          }
#endif   
          memset(targetP->binding, 0x00, MAX_BINDING_SIZE);
 
          /* Only UDP and Non-IP transports are supported, but allow other transports to be configured as well. It is read/write 
           * resource, so the server can perform read operations. Skip internally prior to performing the transport selection.  
           */                              
          LWM2M_STRCPY(targetP->binding, (char*)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length + 1);           
          if (target_serlistP) 
            target_serlistP->binding = binding_mode; 
          
          ds_iot_ulog_high2(" Binding mode for SSID[%d] is configured successfully to [%d]", targetP->shortServerId, binding_mode);
          result = LWM2M_204_CHANGED;           
        }
        else
        {
          ds_iot_ulog_high(" Invalid binding 400 result-");

          result = LWM2M_400_BAD_REQUEST;
        }

        break;
      }

      case LWM2M_SERVER_APN_LINK_ID:
      {
        objlink_t apn_link = 0x00;

        if(target_serlistP && (target_serlistP->version == (float)LWM2M_V_1_0)
            && numData == 1)
        {
          LOG_DEBUG("APN link setting not allowed from v1.0 server");
          return LWM2M_404_NOT_FOUND;
        }
        
        /* Decode the object link information */
        if (0x00 == lwm2m_data_decode_objLink(&dataArray[i], &apn_link))
        { 
          ds_iot_ulog_err1("Decoding APN link for SSID[%d] failed", targetP->shortServerId);
          return LWM2M_400_BAD_REQUEST;
        } 
        
        /* Validate the link information to ensure that the object and instances are present (Object 11) and then only 
         * update the APN link information internally. Applicable for registration servers.   
         *
         * Skip validation for bootstrap.ini config. Server list is not populated when initializing object information 
         * from boostrap.ini on device boot-up (use that as a hint).
         *
         */  
        if ((target_serlistP) &&
            (apn_link != 0xFFFFFFFF))
        {
          lwm2m_object_t *apn_conn_obj = NULL;
          uint16_t object_id = (apn_link >> 16);
          uint16_t instance_id = (apn_link & 0xFFFF);
          lwm2m_list_t *apn_instance_lst = NULL;

          /* Invalid object ID in the object link */ 
          if (object_id != LWM2M_APN_CONN_PROFILE_OBJECT_ID)
          {
            ds_iot_ulog_high1(" Invalid object ID - %d 400 result ", object_id);

            return LWM2M_400_BAD_REQUEST; 
          }
  
          /* Ensure that information of object 11 and its associated instances are present prior to updating link info */
          apn_conn_obj = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, LWM2M_APN_CONN_PROFILE_OBJECT_ID);
          if (NULL == apn_conn_obj)
            return LWM2M_400_BAD_REQUEST; 
        
          apn_instance_lst = apn_conn_obj->instanceList;
          while (apn_instance_lst)
          {
            if (apn_instance_lst->id == instance_id)
               break;  

            apn_instance_lst = apn_instance_lst->next;  
          }       
        
          /* Object instance ID in the object link is not valid */  
          if (!apn_instance_lst)
            return LWM2M_400_BAD_REQUEST;
        }
        if(targetP->apnLink != apn_link)
        {
          val_changed = true;
        }
#ifdef ENABLE_APN_CONN_OBJ
        /* Update the APN link information (format 0xFFFFFFFF) */
        targetP->apnLink = (apn_link && enable_apn_conn_obj) ? apn_link : 0xFFFFFFFF;
        if (target_serlistP)
          target_serlistP->apn_link = (apn_link && enable_apn_conn_obj) ? apn_link : 0xFFFFFFFF;
#else
		targetP->apnLink = 0xFFFFFFFF;
        if (target_serlistP)
          target_serlistP->apn_link = 0xFFFFFFFF;
#endif

        ds_iot_ulog_high2("APN link[0x%x] for SSID[%d] configured successfully", targetP->shortServerId, apn_link);
        result = LWM2M_204_CHANGED;
        break;
      }

      case LWM2M_SERVER_TRIGGER_ID:
      {
        bool value = false;

        if(target_serlistP && (target_serlistP->version == (float)LWM2M_V_1_0)
            && numData == 1)
        {
          LOG_DEBUG("Trigger id setting not allowed from v1.0 server");
          return LWM2M_404_NOT_FOUND;
        }

        if (!lwm2m_data_decode_bool(dataArray + i, &value))
          return LWM2M_400_BAD_REQUEST;
        if(targetP->trigger!= value)
        {
          val_changed = true;
        }
        targetP->trigger = value;
        if (target_serlistP)
          target_serlistP->trigger = value;

        result = LWM2M_204_CHANGED;
        break;
      }

      case LWM2M_SERVER_PREFERRED_TRANSPORT_ID:
      { 
        if(target_serlistP && (target_serlistP->version == (float)LWM2M_V_1_0)
            && numData == 1)
        {
          LOG_DEBUG("Preferred transport setting not allowed from v1.0 server");
          return LWM2M_404_NOT_FOUND;
        }
        
        if (((LWM2M_TYPE_STRING == dataArray[i].type) || 
             (LWM2M_TYPE_OPAQUE == dataArray[i].type)) && 
            ((dataArray[i].value.asBuffer.length > 0) && 
             (dataArray[i].value.asBuffer.length < MAX_PREFERRED_TRANSPORT_SIZE)))
        { 
          lwm2m_binding_t preferred_mode = INVALID_TRANSPORT;

          /* Retrieve the bindings information */ 
          preferred_mode = utils_stringToBinding(dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);

          /* Validate the preferred transport received from the registration servers. 
           *
           * Skip validation for bootstrap.ini config. Server list is not populated when initializing object information 
           * from boostrap.ini on device boot-up (use that as a hint).
           */ 
          if (target_serlistP)
          {
            if ((INVALID_TRANSPORT == preferred_mode) ||
                (BINDING_Q == preferred_mode))
              return LWM2M_400_BAD_REQUEST;
          }

          if(lwm2m_strcmp(targetP->prefTransport, (char *)dataArray[i].value.asBuffer.buffer) != 0)
          {
            val_changed = true;
          }
          memset(&targetP->prefTransport, 0x00, MAX_PREFERRED_TRANSPORT_SIZE);
           
          /* Only UDP and Non-IP transports are supported, but allow other transports to be configured as well. It is read/write 
           * resource, so the server can perform read operations. Skip internally prior to performing the transport selection.  
           */  
          LWM2M_STRCPY(targetP->prefTransport, (char*)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length + 1); 
          if (target_serlistP)
            target_serlistP->pref_transport = preferred_mode;
          
          ds_iot_ulog_high2("Preferred transport for SSID[%d] configured successfully to [%d]", targetP->shortServerId, preferred_mode); 
          result = LWM2M_204_CHANGED;
        }
        else 
        {
          result = LWM2M_400_BAD_REQUEST;
        }

        break;
      }

      case LWM2M_SERVER_UPDATE_ID:
      {
        result = LWM2M_405_METHOD_NOT_ALLOWED;
        break;
      }

      case  LWM2M_SERVER_RESOURCE_30000:
      {
        if (g_carrier_cfg) 
        {
          if (resInstId != INVALID_RES_INST_ID)
          {
            int64_t value = 0; 

            lwm2m_data_decode_int(dataArray + i, &value);

            if (!prv_write_resource_inst_val(&(targetP->custom30000List),resInstId, value, &val_changed))
              result = LWM2M_500_INTERNAL_SERVER_ERROR;
            else
              result = LWM2M_204_CHANGED;
          }
          else 
          {
            int ri = 0;
            int64_t value = 0;
            lwm2m_data_t* subTlvArray = dataArray[i].value.asChildren.array;
            resource_instance_int_list_t* custom30000ListSave = targetP->custom30000List;

            if (dataArray[i].type!= LWM2M_TYPE_MULTIPLE_RESOURCE)
              return LWM2M_400_BAD_REQUEST;
            
            /* Re-initialize the custom list as we have stored the reference */    
            targetP->custom30000List = NULL;
            
            if (0x00 == dataArray[i].value.asChildren.count)
            {
              val_changed = true;
              result = LWM2M_204_CHANGED;
            }
            else if (NULL == subTlvArray)
            {
              result = LWM2M_400_BAD_REQUEST;
            }
            else
            {
              for (ri = 0; ri < dataArray[i].value.asChildren.count; ri++)
              {
                if (!lwm2m_data_decode_int(&subTlvArray[ri], &value))
                {
                  result = LWM2M_400_BAD_REQUEST;
                  break;
                }
                else if (value < 0 || value > 0xFFFF)
                {
                  result = LWM2M_400_BAD_REQUEST;
                  break;
                }
                else if (!prv_write_resource_inst_val(&(targetP->custom30000List), subTlvArray[ri].id, (uint16_t)value, &val_changed))
                {
                  result = LWM2M_500_INTERNAL_SERVER_ERROR;
                  break;
                }
                else
                {
                  result = LWM2M_204_CHANGED;
                  val_changed = true;
                }                 
 
                LOG_DEBUG("Custom value is %d, iteration %d", value, i);
              }
            }

            if (result != LWM2M_204_CHANGED)
            {
              /* Release the resources assocaiated with the partial created new ones */
              LWM2M_LIST_FREE(targetP->custom30000List);
              /* Restore the old values */
              targetP->custom30000List = custom30000ListSave;
            }
            else
            {
              /* Release the resources associated with the saved value list */
              LWM2M_LIST_FREE(custom30000ListSave);
            }
          }
        }         

        break;
      }

      case LWM2M_SERVER_REG_PRI_ORD_ID:
      {
        int64_t value = 0;

        if(target_serlistP && numData == 1)
        {
          LOG_DEBUG("Couldn't set priority order 404 result ");
          return LWM2M_404_NOT_FOUND;
        }
        
        if(1 != lwm2m_data_decode_int(dataArray+i, &value))
        {
          ds_iot_ulog_high("Couldn't set priority order 400 result ");
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(value < 0 || value > 65534)
          {
            return LWM2M_400_BAD_REQUEST;
          }
          else
          {
            if(targetP->priority != value)
            {
              val_changed = true;
            }
            targetP->priority = value;
            targetP->res_set |= REG_PRIORITY_SET;
            result = LWM2M_204_CHANGED;
          }
        }
        break;
      }

      case LWM2M_SERVER_INITIAL_DELAY_TIMER_ID:
      {
        int64_t value = 0;

        if(target_serlistP && numData == 1)
        {
          LOG_DEBUG("Couldn't set initial delay timer 404 result ");
          return LWM2M_404_NOT_FOUND;
        }

        if(1 != lwm2m_data_decode_int(dataArray+i, &value))
        {
          LOG_DEBUG("Couldn't set intial delay timer 400 result ");
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(value < 0 || value > 0xFFFFFFFF)
          {
            return LWM2M_400_BAD_REQUEST;
          }
          else
          {
            if(targetP->initial_delay_timer != value)
            {
              val_changed = true;
            }
            targetP->initial_delay_timer = value;
            targetP->res_set |= INITIAL_DELAY_TIMER_SET;
            result = LWM2M_204_CHANGED;
          }
        }
        break;
      }

      case LWM2M_SERVER_REG_FAILURE_BLOCK_ID:
      {
        bool value = FALSE;

        if(target_serlistP && numData == 1)
        {
          LOG_DEBUG("Couldn't set reg failure block 404 result ");
          return LWM2M_404_NOT_FOUND;
        }

        if(1 != lwm2m_data_decode_bool(dataArray+i, &value))
        {
           LOG_DEBUG("Couldn't set registration failure block 400 result ");
           return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(targetP->reg_failure != value)
          {
            val_changed = true;
          }
          targetP->reg_failure = value;
          targetP->res_set |= REG_FAILURE_SET;
          result = LWM2M_204_CHANGED;
        }
        break;
      }

      case LWM2M_SERVER_BS_ON_REG_FAILURE_ID:
      {
        bool value = FALSE;
        if(target_serlistP && numData == 1)
        {
          LOG_DEBUG("Couldn't set bs on reg failure 404 result ");
          return LWM2M_404_NOT_FOUND;
        }
        if(1 != lwm2m_data_decode_bool(dataArray+i, &value))
        {
           LOG_DEBUG("Couldn't set bs on reg failure block 400 result ");
           return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(targetP->bs_on_failure != value)
          {
            val_changed = true;
          }
          targetP->bs_on_failure = value;
          targetP->res_set |= BS_ON_REG_FAILURE_SET;
          result = LWM2M_204_CHANGED;
        }
        break;
      }

      case LWM2M_SERVER_COMM_RETRY_COUNT_ID:
      {
        int64_t value = 0;

        if(target_serlistP && (target_serlistP->version == (float)LWM2M_V_1_0)
            && numData == 1)
        {
          LOG_DEBUG("Couldn't set retry count 404 result ");
          return LWM2M_404_NOT_FOUND;
        }

        if(1 != lwm2m_data_decode_int(dataArray+i, &value))
        {
          LOG_DEBUG("Couldn't set retry count 400 result ");
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(targetP->retry_count != value)
          {
            val_changed = true;
          }
          targetP->retry_count = value;
          targetP->res_set |= CONN_RETRY_COUNT_SET; // TODO: error in case of big values?
          result = LWM2M_204_CHANGED;
        }
        break;
      }

      case LWM2M_SERVER_COMM_RETRY_TIMER_ID:
      {
        int64_t value = 0;

        if(target_serlistP && (target_serlistP->version == (float)LWM2M_V_1_0)
            && numData == 1)
        {
          LOG_DEBUG("Couldn't set retry timer 404 result ");
          return LWM2M_404_NOT_FOUND;
        }
        
        if(1 != lwm2m_data_decode_int(dataArray+i, &value))
        {
          LOG_DEBUG("Couldn't set retry timer 400 result ");
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(targetP->retry_timer != value)
          {
            val_changed = true;
          }
          targetP->retry_timer = value;
          targetP->res_set |= CONN_RETRY_TIMER_SET; // TODO: error in case of big values?
          result = LWM2M_204_CHANGED;
        }
        break;
      }

      case LWM2M_SERVER_COMM_SEQ_DELAY_TIMER_ID:
      {
        int64_t value = 0;

        if(target_serlistP && (target_serlistP->version == (float)LWM2M_V_1_0)
            && numData == 1)
        {
          LOG_DEBUG("Couldn't set seq delay timer 404 result ");
          return LWM2M_404_NOT_FOUND;
        }
        
        if(1 != lwm2m_data_decode_int(dataArray+i, &value))
        {
          LOG_DEBUG("Couldn't set seq delay timer 400 result ");
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(targetP->seq_delay_timer != value)
          {
            val_changed = true;
          }
          targetP->seq_delay_timer = value;
          targetP->res_set |= CONN_SEQ_DELAY_TIMER_SET; // TODO: error in case of big values?
          result = LWM2M_204_CHANGED;
        }
        break;
      }

      case LWM2M_SERVER_COMM_SEQ_RETRY_COUNT_ID:
      {
        int64_t value = 0;

        if(target_serlistP && (target_serlistP->version == (float)LWM2M_V_1_0)
            && numData == 1)
        {
          LOG_DEBUG("Couldn't set seq retry count 404 result ");
          return LWM2M_404_NOT_FOUND;
        }
        
        if(1 != lwm2m_data_decode_int(dataArray+i, &value))
        {
          LOG_DEBUG("Couldn't set seq retry count 400 result ");
          return LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(targetP->seq_retry_count != value)
          {
            val_changed = true;
          }
          targetP->seq_retry_count = value;
          targetP->res_set |= CONN_SEQ_DELAY_COUNT_SET; // TODO: error in case of big values?
          result = LWM2M_204_CHANGED;
        }
        break;
      }

      default:
      {
        /* If create/write have one more one resource to write we write on unknown/notsupported resource*/
        if (numData > 1)
          result = LWM2M_204_CHANGED;
        else
          result = LWM2M_404_NOT_FOUND;

        break;  
      }
    }

    i++;
  } while (i < numData && result == LWM2M_204_CHANGED);

  if (LWM2M_204_CHANGED == result && (create_operation == true || val_changed == true))
    store_server_persistent_info(targetP, LWM2M_TRUE);

  return result;
}



/**
 * @fn static uint8_t prv_server_write()
 *
 * @brief This fucntion implements the write operation performed on 
 *        Server object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numData number of resources in the array
 * @param dataArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
uint8_t prv_server_write(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP,
    uint16_t resInstId, uint8_t write_method, bool bs_true)
{
  server_instance_t * targetP = NULL;
  uint8_t result = LWM2M_205_CONTENT;
  lwm2m_context_t *contextP = NULL;
  lwm2m_server_t *target_serlistP = NULL;
  bool version = FALSE;
  if( dataArray == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("LWM2M_LOG: fun [prv_server_write]: LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;     
  }

  targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
  if (NULL == targetP)
  {
    return LWM2M_404_NOT_FOUND;
  }
  
  /* Check all resources to be written are writable or not */
  contextP = get_lwm2m_context();
  if (NULL != contextP)
  {
    target_serlistP = (lwm2m_server_t *)LWM2M_LIST_FIND(contextP->serverList, targetP->shortServerId);
    if(target_serlistP && (target_serlistP->version >= (float)LWM2M_V_1_1))
      version = TRUE;
  }
  result = prv_server_create_write_check(numData, dataArray, bs_true, version);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err("Write operation failed due to write on non-writable resource");
    return result;
  }

  result = prv_server_write_resources(objectP, targetP, numData, dataArray,
                                        resInstId, write_method);

  if(result == LWM2M_204_CHANGED)
  {
    contextP = get_lwm2m_context();
    if (NULL != contextP)
    {
      target_serlistP = (lwm2m_server_t *)LWM2M_LIST_FIND(contextP->serverList, targetP->shortServerId);
      if(target_serlistP && (target_serlistP->version >= (float)LWM2M_V_1_1))
        get_1_1_retry_info(objectP, instanceId, target_serlistP);
    }
  }
  return result;

}



/**
 * @fn static uint8_t prv_server_execute()
 *
 * @brief This fucntion implements the execute operation performed on 
 *        Server object
 *
 * @param instanceId Instance ID of the object to be read
 * @param resourceId Resource ID of the resource to be executed
 * @param buffer message payload from server fro execute operation
 * @param length message payload length
 * @param objectP Object on which the operation needs to be executed
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_server_execute(uint16_t instanceId,
    uint16_t resourceId,
    uint8_t * buffer,
    int length,
    lwm2m_object_t * objectP)
{
  server_instance_t * targetP = NULL;
  lwm2m_server_t *target_serlistP = NULL;
  lwm2m_context_t *contextP = NULL;

  if (NULL == objectP)
  {
    LOG_DEBUG("%s objectP", nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;     
  }

  targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
  if (NULL == targetP) 
    return LWM2M_404_NOT_FOUND;

  contextP = get_lwm2m_context();
  if (NULL == contextP) 
    return LWM2M_500_INTERNAL_SERVER_ERROR;

  target_serlistP = (lwm2m_server_t *)lwm2m_list_find((lwm2m_list_t *)contextP->serverList, targetP->shortServerId);
  if (NULL == target_serlistP)
    return LWM2M_500_INTERNAL_SERVER_ERROR;      

  switch (resourceId)
  {
    case LWM2M_SERVER_DISABLE_ID:
    {                
#ifdef LWM2M_SERVER_OBJECT
      LWM2M_SERVER_SET_DISABLE_ID(updt_disabl_flag);
      short_serverId = targetP->shortServerId;  
#endif
      return LWM2M_204_CHANGED;
    }

    case LWM2M_SERVER_UPDATE_ID:
    {  
      bool profile_change = false; 

      if ((target_serlistP == NULL) || (target_serlistP->status != STATE_REGISTERED))
        return LWM2M_400_BAD_REQUEST;
     
      /* For servers supporting LwM2Mv1.1 and higher, when registration update trigger (with param) is received validate
       * the APN profile configuration to check if there is any change in the existing transport bindings.
       */ 
      if (target_serlistP->version >= (float)LWM2M_V_1_1)
      {
        lwm2m_binding_t trigger_param = BINDING_UNKNOWN;

        /* Validate the register update with parameter */
        if ((length) && (length >= MAX_REG_TRIGGER_PARAM_SIZE)) 
          return LWM2M_400_BAD_REQUEST;
        
        /* Validate the data call parameters to see if we need to tear-down and re-establish the data call */ 
        if (length)
        {
          /* Trigger information is a one-time transient configuration that doesn't persist over reboot etc. It is used by 
           * the transport selection logic only when there is an assoicated parameter otherwise it is dummy. We would clear 
           * trigger related information immediately after applying the transport selection logic.      
           */
          trigger_param = utils_stringToBinding((uint8_t *)buffer, length);
          if (BINDING_UNKNOWN == trigger_param)
            return LWM2M_400_BAD_REQUEST;

          memset(targetP->regTriggerParam, 0x00, MAX_REG_TRIGGER_PARAM_SIZE);
                    
          /* LwM2Mv1.0 client doesn't need to perform transport selection, so we don't need this register trigger flag.
           * Reset the trigger flag and associated param later if there is no transport change. 
           */ 
          targetP->regTrigger = true;          
          targetP->regTriggerParam[0] = buffer[0];
        }
          
        /* Detect and apply the new connection profile changes (registration trigger param validation done internally) */   
        profile_change = data_call_conn_profile_change(targetP->shortServerId);
        if (profile_change)          
        {
          carrier_apn_cfg_t *apn_info = NULL; 

          /* Set the flag indicating profile change */
          profile_change = true; 

          /* Retrieve APN information associated with the short server ID. Indicate that the registration update needs 
           * to be sent at a later point after the data call is re-established due to profile change.
           */ 
          apn_info = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(data.lwm2mH->apn_cfg, targetP->shortServerId);  
          if (apn_info)            
            apn_info->send_update = true; 
          
          /* Handle the profile change as part of the LwM2M main thread and not in the RX thread. It would allow us to
           * send the ACK for the CON register update trigger message prior to updating the transport due to the profile
           * change that was detected. 
           *
           * Set the associated short server ID for which profile change was detected 
           */  
          short_serverId = targetP->shortServerId;
          LWM2M_SERVER_SET_UPDATE_PROFILE_CHANGE(updt_disabl_flag);    
        }
      }   

      /* Send the the register update if there is no profile or transport change */
      if (!profile_change)
      {
        /* Set the associated short server ID for which we need to send an update */
        short_serverId = targetP->shortServerId;
        LWM2M_SERVER_SET_UPDATE_ID(updt_disabl_flag);        

        /* Reset the trigger information */
        targetP->regTrigger = false;        
        memset(targetP->regTriggerParam, 0x00, MAX_REG_TRIGGER_PARAM_SIZE);
      }

      return LWM2M_204_CHANGED;         
    }   

    case LWM2M_SERVER_APN_LINK_ID:
    case LWM2M_SERVER_REG_PRI_ORD_ID:
    case LWM2M_SERVER_INITIAL_DELAY_TIMER_ID:
    case LWM2M_SERVER_REG_FAILURE_BLOCK_ID:
    case LWM2M_SERVER_BS_ON_REG_FAILURE_ID:
    case LWM2M_SERVER_COMM_RETRY_COUNT_ID:
    case LWM2M_SERVER_COMM_RETRY_TIMER_ID:
    case LWM2M_SERVER_COMM_SEQ_DELAY_TIMER_ID:
    case LWM2M_SERVER_COMM_SEQ_RETRY_COUNT_ID:
    case LWM2M_SERVER_TRIGGER_ID:
    case LWM2M_SERVER_PREFERRED_TRANSPORT_ID:
    {
      if ((target_serlistP == NULL))
        return LWM2M_500_INTERNAL_SERVER_ERROR;
      if (target_serlistP->version >= (float)LWM2M_V_1_1)
        return LWM2M_405_METHOD_NOT_ALLOWED;
      else
        return LWM2M_404_NOT_FOUND;
    }
    case LWM2M_SERVER_SHORT_ID_ID:
    case LWM2M_SERVER_LIFETIME_ID:
    case LWM2M_SERVER_MIN_PERIOD_ID:
    case LWM2M_SERVER_MAX_PERIOD_ID:

    case LWM2M_SERVER_TIMEOUT_ID:
    case LWM2M_SERVER_STORING_ID:
    case LWM2M_SERVER_BINDING_ID:
      return LWM2M_405_METHOD_NOT_ALLOWED;

    default:
      return LWM2M_404_NOT_FOUND;
  }
}


/**
 * @fn static uint8_t prv_server_delete()
 *
 * @brief This fucntion is used to delete Server object
 *
 * @param id Instance ID of the object to be deleted
 * @param objectP Object to be deleted
 *
 * @return LWM2M response type
 */
static uint8_t prv_server_delete(uint16_t id,
    lwm2m_object_t * objectP,
    bool persist_file)
{
  server_instance_t * serverInstance = NULL;
  if( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&serverInstance);
  if (NULL == serverInstance) return LWM2M_404_NOT_FOUND;

  /* Persist the server instance */ 
  store_server_persistent_info(serverInstance, LWM2M_FALSE);
  /* Release the associated server instance */
  if(g_carrier_cfg || serverInstance->custom30000List != NULL) 
  {
      LWM2M_LIST_FREE(serverInstance->custom30000List);
  }
  lwm2m_free(serverInstance);

  return LWM2M_202_DELETED;
}


/**
 * @fn static uint8_t prv_server_create()
 *
 * @brief This fucntion is used to delete Server object
 *
 * @param instanceId Instance ID of the object to be created
 * @param numData number of resources in the array
 * @param dataArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */

/***************************************************************************************************

 Under section 8.2.3 Operation on Object of OMA 1.1

- If the �Create� operation is permitted, the LwM2M Client MUST perform the instantiation on the 
Object only if all the mandatory Resources are present in the �New Value� parameter (see Section 5). 
If all the mandatory Resources are not present, the LwM2M Client MUST send a �Bad Request� error code 
to the LwM2M Server


*************************************************************************************************************/

static uint8_t prv_server_create(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP, bool apiCall)
{
  server_instance_t * serverInstance =NULL;
  uint8_t result = LWM2M_500_INTERNAL_SERVER_ERROR;
  uint16_t mandatory_resource[] = {LWM2M_SERVER_SHORT_ID_ID, LWM2M_SERVER_LIFETIME_ID, LWM2M_SERVER_STORING_ID, LWM2M_SERVER_BINDING_ID};
  uint8_t size = sizeof(mandatory_resource)/sizeof(mandatory_resource[0]);
  if( dataArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
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
  result = prv_server_create_write_check(numData, dataArray, TRUE, FALSE);
  if (result != LWM2M_204_CHANGED )
  {
     ds_iot_ulog_err("Creating instance failed ");
     return result;
  }

skip_check:
  serverInstance = (server_instance_t *)lwm2m_malloc(sizeof(server_instance_t));
  if (NULL == serverInstance) return LWM2M_500_INTERNAL_SERVER_ERROR;
  memset(serverInstance, 0, sizeof(server_instance_t));

  /* Assign the server instance ID */
  serverInstance->instanceId = instanceId; 
  /* Initialize the APN link to NULL (default) */
  serverInstance->apnLink = 0xFFFFFFFF;
  /* Trigger resource set to allowed (default) */
  serverInstance->trigger = true;   

  serverInstance->priority = UNDEFINED_PRIORITY;
  serverInstance->initial_delay_timer = DEFAULT_INITIAL_DELAY_TIMER;
  serverInstance->reg_failure = DEFAULT_REG_FAILURE;
  serverInstance->bs_on_failure = DEFAULT_BS_ON_REG_FAILURE_1_1;
  serverInstance->retry_count = DEFAULT_REG_RETRY_COUNT;
  serverInstance->retry_timer = DEFAULT_REG_RETRY_TIMER;
  serverInstance->seq_delay_timer = DEFAULT_REG_SEQ_DELAY_TIMER;
  serverInstance->seq_retry_count = DEFAULT_REG_SEQ_DELAY_COUNT_1_1;

  objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, serverInstance);
  create_operation = LWM2M_TRUE;
  result = prv_server_write_resources(objectP, serverInstance, numData, dataArray, 
                                          INVALID_RES_INST_ID, QAPI_COAP_POST);
  create_operation = LWM2M_FALSE;
  if (result != LWM2M_204_CHANGED)
  {
    /* Issue in creating object instanace so returning "Bad Request" error */
    result = LWM2M_400_BAD_REQUEST;
	/* 3rd param Doesn't matter for Multi instance or Mandotary Objects,
	only applicable for single instance optional objects.*/
    (void)prv_server_delete(instanceId, objectP,false);
  }
  else
  {
    result = LWM2M_201_CREATED;
  }

  return result;
}

/**
 * @fn int populate_lower_version_retry_values()
 * @brief This function is used to update lower version registration retry values
 *        to server object instance resources
 * @param shortId    LwM2M server short server id
 *
 * @return     0 for success,-1 for failure
 */
int populate_lower_version_retry_values(uint16_t shortId)
{
  server_instance_t * serverInstance =NULL;
  lwm2m_object_t *objectP = NULL;
  lwm2m_context_t *contextP = NULL;
  carrier_apn_cfg_t *apn_cfg = NULL;
  carrier_apn_cfg_t *server_apn_cfg = NULL;
  lwm2m_active_server_t *serverP = NULL;

  contextP = get_lwm2m_context();

  if(contextP == NULL)
  {
    return -1;
  }

  objectP =  (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, LWM2M_SERVER_OBJECT_ID);
  if(objectP != NULL)
  {
    serverInstance = (server_instance_t *)prv_findServerInstance(objectP, shortId);
    if(serverInstance != NULL)
    {
      apn_cfg = get_lwm2m_apn_cfg();

      server_apn_cfg = (carrier_apn_cfg_t *) LWM2M_LIST_FIND(apn_cfg, shortId);

      if(apn_cfg != NULL && server_apn_cfg != NULL)
      {
        serverInstance->bs_on_failure = server_apn_cfg->bs_if_reg_fails;
      }
      else
      {
        serverInstance->bs_on_failure = DEFAULT_BS_ON_REG_FAILURE_1_0;
      }

      if(g_carrier_cfg)
      {
        lwm2m_object_t * serv_obj = NULL;
        serv_obj =  (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList , LWM2M_SERVER_OBJECT_ID);
        if (serv_obj == NULL )
        {
          LOG_DEBUG ("No server object found");
        }
        serverInstance->initial_delay_timer = get_serverobj_hold_off_time(serv_obj, shortId);
      }
      else
      {
        serverInstance->initial_delay_timer = DEFAULT_INITIAL_DELAY_TIMER;
      }

      serverInstance->priority = UNDEFINED_PRIORITY;
      serverInstance->reg_failure = DEFAULT_REG_FAILURE;
      serverInstance->retry_count = (((log(gRegRetryMaxTimeout) - log(gRegRetryInitialTimeout)) / log(gRegRetrytimeoutExponent)) +(double)(0.5)) + 1;
      serverInstance->retry_timer = gRegRetryInitialTimeout;
      serverInstance->seq_delay_timer = data.cfg_info.wait_time_for_rereg;
      serverInstance->seq_retry_count = DEFAULT_REG_SEQ_DELAY_COUNT_1_0; //TODO : can increase this value

      serverP = (lwm2m_active_server_t *)LWM2M_LIST_FIND(contextP->activeServerList, shortId);
      if(serverP != NULL)
      {
        serverP->pri_order = UNDEFINED_PRIORITY;
        if(persis_regstatus_feat_enabled == LWM2M_TRUE)
        {
          sync_activeserverlist_to_persistence(contextP);
        }
      }
      return 0;
    }
  }
  return -1;
}

#ifdef FACTORY_BOOTSTRAPPING
/**
 * @fn lwm2m_object_t * get_server_object()
 *
 * @brief This function is used to get the information regarding the Security
 *        object during client registartion
 *
 * @param void
 *
 * @return Server object  
 *
 */
lwm2m_object_t * get_server_object()
#else
  /**
   * @fn lwm2m_object_t * get_server_object()
   *
   * @brief This function is used to get the information regarding the Server
   *        object during client registartion
   *
   * @param serverId server id 
   * @param binding type of binding
   * @param lifetime server lifetime
   * @param storing storing value
   *
   * @return Server object  
   *
   */
lwm2m_object_t * get_server_object(int serverId,
    const char* binding,
    int lifetime,
    bool storing)
#endif
{
  lwm2m_object_t * serverObj = NULL;
#ifndef FACTORY_BOOTSTRAPPING
  if( binding == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return NULL;
  }
#endif 

  serverObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

  if (NULL != serverObj)
  {
#ifndef FACTORY_BOOTSTRAPPING
    server_instance_t * serverInstance;
#endif

    memset(serverObj, 0, sizeof(lwm2m_object_t));

    serverObj->objID = 1;

#ifndef FACTORY_BOOTSTRAPPING
    // Manually create an hardcoded server
    serverInstance = (server_instance_t *)lwm2m_malloc(sizeof(server_instance_t));
    if (NULL == serverInstance)
    {
      lwm2m_free(serverObj);
      return NULL;
    }

    memset(serverInstance, 0, sizeof(server_instance_t));
    serverInstance->instanceId = 0;
    serverInstance->shortServerId = serverId;
    serverInstance->lifetime = lifetime;
    serverInstance->storing = storing;
    LWM2M_MEMCPY (serverInstance->binding, binding, strlen(binding)+1);
    serverObj->instanceList = LWM2M_LIST_ADD(serverObj->instanceList, serverInstance);
#endif

    serverObj->readFunc = prv_server_read;
    serverObj->discoverFunc = prv_server_discover;
    serverObj->writeFunc = prv_server_write;
    serverObj->createFunc = prv_server_create;
    serverObj->deleteFunc = prv_server_delete;
    serverObj->executeFunc = prv_server_execute;
  }

  return serverObj;
}


/**
 * @fn void clean_server_object()
 *
 * @brief This fucntion is used to free server instance
 *
 * @param object handle to server object
 *
 * @return void
 *
 */
void clean_server_object(lwm2m_object_t * object)
{
  if( object == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return ;
  }

  while (object->instanceList != NULL)
  {
    server_instance_t * serverInstance = (server_instance_t *)object->instanceList;
    object->instanceList = object->instanceList->next;
	if(g_carrier_cfg || serverInstance->custom30000List != NULL) 
    {
      LWM2M_LIST_FREE(serverInstance->custom30000List);
	}
    lwm2m_free(serverInstance);
  }
}


/* 
 * @fn     bool update_isRegistered_for_server()
 * @brief  This function is used to update isRegistered  resource to server object instance 
 * @param  serv_objectP - pointer to security object 
 serverObjInstID - Id of security object instance 
 * @return void     
 */ 
void update_isRegistered_for_server (lwm2m_object_t * serv_object, uint16_t ssid, uint8 value)
{ 
  server_instance_t * targetP = NULL;
  lwm2m_list_t *temp_list;
  uint8_t inst = 0, max_inst = 0;
  bool val_changed = false;
  if (serv_object== NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return ;
  }
  temp_list = serv_object->instanceList;
  for(max_inst = 0;temp_list !=NULL;max_inst++)
  {
    max_inst = temp_list->id;
    temp_list = temp_list->next;
  }
  do
  {
    targetP = (server_instance_t *)LWM2M_LIST_FIND(serv_object->instanceList, inst);
    inst++;
    if (targetP == NULL)
    {
      continue;
    }
    if(targetP->shortServerId == ssid)
    {
      prv_write_resource_inst_val(&(targetP->custom30000List),LWM2M_SERVER_RESOURCE_INST_IS_REGISTERED, value, &val_changed);
      if(val_changed == true)
      {
        store_server_persistent_info(targetP, LWM2M_TRUE);
      }
      break;
    }
  }while(inst < max_inst);
  return ;

}
/*
 * @fn     uint32_t get_serverobj_hold_off_time()
 * @brief  This function is used to get client hold off timer resource from server object
 * @param  objectP - pointer to server object
 *         shortID - id of server object instance
 * @return uint32_t value
 */
uint32_t get_serverobj_hold_off_time(lwm2m_object_t * objectP, uint16_t shortID)
{
  server_instance_t * targetP = NULL;
  if( objectP == NULL )
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return 0;
  }

  targetP = (server_instance_t *)prv_findServerInstance(objectP, shortID);

  if (targetP == NULL)
  {
    ds_iot_ulog_high1("No server instance found for short server ID -> %d", shortID);
  }

  if (NULL != targetP)
  {
    resource_instance_int_list_t *resInst = (resource_instance_int_list_t *)LWM2M_LIST_FIND(targetP->custom30000List, LWM2M_SERVER_RESOURCE_INST_IS_CLIENT_HOLD_OFF_TIMER);
    if(resInst != NULL)
      return resInst->InstValue;
    else
      return 0;
  }

  return 0 ;
}

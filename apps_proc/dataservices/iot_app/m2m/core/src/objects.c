/******************************************************************************

  @file    objects.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/

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
 *    Fabien Fleutot - Please refer to git log
 *    Toby Jaffey - Please refer to git log
 *    Benjamin Cabé - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    
 *******************************************************************************/

/*
   Copyright (c) 2013, 2014 Intel Corporation

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 * Neither the name of Intel Corporation nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>

*/
#include "internals.h"
#include "ds_log.h"


#ifdef LWM2M_CLIENT_MODE


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "liblwm2m.h"
#include "lwm2m_signals.h"
#include "object_security.h"
#include "object_server.h"
#include "lwm2m_carrier.h"
#include "math.h"
#include "lwm2mclient.h"
#ifdef AUTHORIZATION_SUPPORT
#include "object_access_control.h"
#endif
extern bool g_carrier_cfg; // flag to check  carrier config is enabled or not 
extern bool persis_bootstrap_feat_enabled;
extern bool persis_regstatus_feat_enabled;  
extern bool lwm2m_psm_enable;               // LWM2M PSM is enabled or not.
extern time_t gRegRetryInitialTimeout;
extern time_t gRegRetrytimeoutExponent;
extern time_t gRegRetryMaxTimeout;
extern bool isBootstrappedflag; 
extern bool enable_acl_obj;
extern bool enable_bs_server_replace;

#ifdef LWM2M_SERVER_OBJECT
extern uint8_t updt_disabl_flag;
extern uint16_t short_serverId;
#endif
extern qurt_signal_t lwm2m_signal_Handle;

extern qapi_Status_t lwm2m_load_application_object_info 
(
  lwm2m_app_obj_info_t **load_obj_info
);


#ifdef LWM2M_SERVER_OBJECT
/**
 * @fn int object_UpdateServerList()
 * @brief API is to update server list form context object list
 *
 * @param contextP          pointer to lwm2m_context_t
 * @param short_serverid    lwm2m_server short server id
 *
 * @return      0 on success
 *              -1 on failure 
 */                                   
int object_UpdateServerList(lwm2m_context_t * contextP, 
    uint16_t short_serverid)
{                                                                               
  lwm2m_object_t *serverObjP = NULL;                                        
  lwm2m_server_t *server_list = NULL;                                       
  lwm2m_list_t *serverInstP = NULL;                                          
#ifdef PSM_LWM2M
  bool          value_changed = FALSE;
#endif
  LOG_DEBUG("Entering");                                                            

  if (contextP == NULL) 
  { 
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return EFAILURE;
  }

  server_list = (lwm2m_server_t *)LWM2M_LIST_FIND(contextP->serverList, short_serverid); 
  if (NULL == server_list) 
  {
    ds_iot_ulog_err1("Server %d id not found", short_serverid);
    return EFAILURE;
  }    

  serverObjP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, LWM2M_SERVER_OBJECT_ID); 
  if (NULL == serverObjP)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error ServerObjP");
    return EFAILURE;
  }

  serverInstP = serverObjP->instanceList;

  while (NULL != serverInstP)                                                  
  {                                                                          
    int size = 0;                                                          
    lwm2m_data_t *dataP;                                                  
    int64_t value = 0; 
    #ifdef PSM_LWM2M
      int64_t prev_value = 0;
    #endif
    bool storing = true;

    if (serverObjP->readFunc(serverInstP->id, &size, &dataP, serverObjP, INVALID_RES_INST_ID, server_list) != LWM2M_205_CONTENT)                                        
    {       
      lwm2m_data_free(size, dataP);                                  
      return EFAILURE;                                                     
    }  
                                                                
    if (lwm2m_data_decode_int(dataP + 0,&value ) == 0)                  
    {                                                                  
      lwm2m_data_free(size, dataP);                              
      return EFAILURE;                                                 
    }

    /* Short server ID match */
    if (value == short_serverId)
    {                                                                  
      if (lwm2m_data_decode_int(dataP + 1, &value) == 0)              
      {                                                          
        lwm2m_data_free(size,dataP);                           
        return EFAILURE;                                             
      }                                                          
      #ifdef PSM_LWM2M
        prev_value = server_list->lifetime;
      #endif
      server_list->lifetime = value;
      #ifdef PSM_LWM2M
        if(prev_value != value)
        {
          value_changed = TRUE;
        }
      #endif

      if (lwm2m_data_decode_int(dataP + 4, &value) == 0)
      {
        lwm2m_data_free(size, dataP);
        return EFAILURE;
      }
      #ifdef PSM_LWM2M
        prev_value = server_list->disable_timeOut;
      #endif
      server_list->disable_timeOut= value;
      #ifdef PSM_LWM2M
        if(prev_value != value)
        {
          value_changed = TRUE;
        }
      #endif
      if(lwm2m_data_decode_int(dataP + 2, &value) == 0 )  //  Updating default minimum period
      {
        lwm2m_data_free(size, dataP);
        return EFAILURE;
      }
      #ifdef PSM_LWM2M
        prev_value = server_list->defaultMinPeriod;
      #endif
      server_list->defaultMinPeriod= value;
      #ifdef PSM_LWM2M
        if(prev_value != value)
        {
          value_changed = TRUE;
        }
      #endif
      LOG_DEBUG("defaultMinPeriod: %d",server_list->defaultMinPeriod);
      qurt_signal_set(&lwm2m_signal_Handle, LOCATION_PMIN_UPDATED);

      if (lwm2m_data_decode_int(dataP + 3, &value) == 0 )  //  Updating default maximum period
      {
        lwm2m_data_free(size, dataP);
        return EFAILURE;
      }
      #ifdef PSM_LWM2M
        prev_value = server_list->defaultMaxPeriod;
      #endif
      server_list->defaultMaxPeriod= value;
      #ifdef PSM_LWM2M
        if(prev_value != value)
        {
          value_changed = TRUE;
        }
      #endif
      LOG_DEBUG("defaultMaxPeriod: %d",server_list->defaultMaxPeriod);
      qurt_signal_set(&lwm2m_signal_Handle, LOCATION_PMIN_UPDATED);

      server_list->isFallbackV1_0 = FALSE;
    
      if (LWM2M_SERVER_IS_UPDATE_SET(updt_disabl_flag))
      {
        LWM2M_SERVER_SET_UPDATE_ID(server_list->update_disable_check);
        LWM2M_SERVER_RESET_UPDATE_ID(updt_disabl_flag);
        qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
      }

      if (LWM2M_SERVER_IS_UPDATE_SET_LIFETIME(updt_disabl_flag))
      {
        LWM2M_SERVER_SET_UPDATE_LIFETIME(server_list->update_disable_check);
        LWM2M_SERVER_RESET_UPDATE_LIFETIME(updt_disabl_flag);
        qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
      }

      if (LWM2M_SERVER_IS_UPDATE_SET_BINDING(updt_disabl_flag))
      {
        LWM2M_SERVER_SET_UPDATE_BINDING(server_list->update_disable_check);
        LWM2M_SERVER_RESET_UPDATE_BINDING(updt_disabl_flag);
        qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
      }

      if (LWM2M_SERVER_IS_DISABLE_SET(updt_disabl_flag))
      {
        LWM2M_SERVER_SET_DISABLE_ID(server_list->update_disable_check);
        LWM2M_SERVER_RESET_SERVER_ID(updt_disabl_flag);
        qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
      }

      if (LWM2M_SERVER_IS_UPDATE_PROFILE_CHANGE_SET(updt_disabl_flag))
      {
        LWM2M_SERVER_SET_UPDATE_PROFILE_CHANGE(server_list->update_disable_check);
        LWM2M_SERVER_RESET_UPDATE_PROFILE_CHANGE(updt_disabl_flag);
        qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
      } 

      if (lwm2m_data_decode_bool(dataP + 5, &storing) == 0 )
      {
        lwm2m_data_free(size, dataP);
        return EFAILURE;
      }

      server_list->storing = storing;
      #ifdef PSM_LWM2M
	  if(value_changed)
	  	lwm2m_psm_operation(contextP, server_list->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UPDATE, PSM_MODIFY_NODE);

      #endif
      //Updating the registration status to configuration file
      if (persis_regstatus_feat_enabled == LWM2M_TRUE)  
        lwm2m_carrier_update_regstatus(server_list);
      short_serverId = 0;
	     lwm2m_data_free(size, dataP);
      break;                                                         
    }

    lwm2m_data_free(size, dataP);
    serverInstP = serverInstP->next;  
  }

  return ESUCCESS;
}                                                                              
#endif            

#ifdef AUTHORIZATION_SUPPORT
/**
 * @fn acc_ctrl_oi_t * get_acc_cl_objectinst()
 * @brief This function is used to get the access control object 
 *        instance for given uri
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to lwm2m_uri_t
 *
 * @return    pointer to the access control object on success
 *        NULL on failure
 */
acc_ctrl_oi_t * get_acc_cl_objectinst(lwm2m_context_t *contextP,
    lwm2m_uri_t *uriP)
{
  if(!enable_acl_obj)
  	return NULL;
  
  if ( contextP == NULL || uriP == NULL ) 
  {
    ds_iot_ulog_err(" fun [get_acc_cl_objectinst] LWM2M_LOG: NULL Error");
    return NULL;
  }
  if(uriP->flag > LWM2M_URI_FLAG_OBJECT_ID)
  {
    acc_ctrl_oi_t *acc_ctrl_oi = NULL;
    acc_ctrl_oi = (acc_ctrl_oi_t *)contextP->acObjInstList;
    while(NULL != acc_ctrl_oi)
    {
      if((acc_ctrl_oi->objectId == uriP->objectId) && (acc_ctrl_oi->objectInstId == uriP->instanceId))
        break;
      acc_ctrl_oi = acc_ctrl_oi->next;
    }
    return acc_ctrl_oi;
  }
  return NULL;
}

/**
 * @fn acc_ctrl_oi_t * get_acc_cl_object()
 * @brief This function is used to get the access control object for given uri
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to lwm2m_uri_t
 *
 * @return    pointer to the access control object instance on success
 *        NULL on failure
 */
acc_ctrl_oi_t * get_acc_cl_object(lwm2m_context_t *contextP, 
    lwm2m_uri_t *uriP) 
{

  acc_ctrl_oi_t *acc_ctrl_oi = NULL;

  if(!enable_acl_obj)
  	return NULL;
  
  if ( contextP == NULL || uriP == NULL ) 
  {
    ds_iot_ulog_err(" fun [get_acc_cl_object] LWM2M_LOG: NULL Error");
    return NULL;
  }
  acc_ctrl_oi = (acc_ctrl_oi_t *)contextP->acObjInstList;

  while(NULL != acc_ctrl_oi)
  {
    if((acc_ctrl_oi->objectId == uriP->objectId) && (acc_ctrl_oi->accCtrlOwner == LWM2M_MAX_ID))
      break;
    acc_ctrl_oi = acc_ctrl_oi->next;
  }

  return acc_ctrl_oi;
}

/**
 * @fn acc_ctrl_ri_t *get_acc_cl_resourceinst()
 * @brief This function is used to get the access control resource instance 
 *        for give access control object instance
 *
 * @param contextP      pointer to lwm2m_context_t
 * @param acc_ctrl_oi   pointer to access control object instance
 *
 * @return    pointer to the access control resource instance on success
 *        NULL on failure
 */
acc_ctrl_ri_t *get_acc_cl_resourceinst(lwm2m_context_t *contextP, 
    acc_ctrl_oi_t *acc_ctrl_oi)
{
  acc_ctrl_ri_t *acc_ctrl_ri = NULL;

  if(!enable_acl_obj)
  	return NULL;
  
  if ( contextP == NULL || acc_ctrl_oi == NULL ) 
  {
    ds_iot_ulog_err(" fun [get_acc_cl_resourceinst] LWM2M_LOG: NULL Error");
    return NULL;
  } 
  acc_ctrl_ri = (acc_ctrl_ri_t *)acc_ctrl_oi->accCtrlValList;
  while((acc_ctrl_ri != NULL) && 
      !(acc_ctrl_ri->resInstId == contextP->activeServerP->shortID))
    acc_ctrl_ri = acc_ctrl_ri->next;
  if(NULL != acc_ctrl_ri)
    return acc_ctrl_ri;
  else if ((acc_ctrl_oi->accCtrlValList != NULL) && 
      (acc_ctrl_oi->accCtrlValList->resInstId == 0))
  {
    return acc_ctrl_oi->accCtrlValList;
  }
  else 
  {
    return NULL;
  }
}

#endif

/**
 * @fn uint8_t object_isObjOrObjInstanceExist()
 * @brief This function is used to check whether object or object instance level
 *        of given uri exists or not
 *
 * @param contextP      pointer to lwm2m_context_t
 * @param uriP          pointer to lwm2m_uri_t
 *
 * @return    true for exist
 *            false for not exist
 */
bool object_isObjOrObjInstanceExist(lwm2m_context_t * contextP, lwm2m_uri_t * uriP)
{

  lwm2m_object_t   *targetP = NULL;
  lwm2m_list_t    *instanceP = NULL;

  if(contextP == NULL || uriP == NULL)
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return false;
  }

  ds_iot_ulog_high("Operation on");
  PRINT_URI(uriP);

  if(!LWM2M_URI_IS_SET_OBJECT(uriP))
    return true;

  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);

  /* URI on object level */
  if (NULL == targetP)
    return false;
  else if(!LWM2M_URI_IS_SET_INSTANCE(uriP))
    return true;

  /* URI on object instance level */
  instanceP = lwm2m_list_find(targetP->instanceList, uriP->instanceId);
  if (NULL == instanceP)
    return false;

  /* leave check of URI on object instance resource level to each Object self*/

  return true;

}

/**
 * @fn uint8_t object_checkReadable()
 * @brief This function is used to check whether given uri is readalbe or not
 *
 * @param contextP      pointer to lwm2m_context_t
 * @param uriP          pointer to lwm2m_uri_t
 *
 * @return  LWM2M response type
 *        
 */
uint8_t object_checkReadable(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP, lwm2m_server_t * serverP)
{
  uint8_t result;
  lwm2m_object_t * targetP = NULL;
  lwm2m_data_t * dataP = NULL;
  int size;


  if ( contextP == NULL || uriP == NULL ) 
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return LWM2M_400_BAD_REQUEST;
  }
  LOG_URI(uriP);
  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
  if (NULL == targetP) return LWM2M_404_NOT_FOUND;
  if (NULL == targetP->readFunc) return LWM2M_405_METHOD_NOT_ALLOWED;

  if (!LWM2M_URI_IS_SET_INSTANCE(uriP)) return LWM2M_205_CONTENT;

  if (NULL == lwm2m_list_find(targetP->instanceList, uriP->instanceId)) return LWM2M_404_NOT_FOUND;

  if (!LWM2M_URI_IS_SET_RESOURCE(uriP)) return LWM2M_205_CONTENT;

  size = 1;
  dataP = lwm2m_data_new(1);
  if (dataP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;

  dataP->id = uriP->resourceId;

  result = (uint8_t) targetP->readFunc(uriP->instanceId, &size, &dataP, targetP, INVALID_RES_INST_ID, serverP);
  lwm2m_data_free(1, dataP);

  return result;
}

/**
 * @fn uint8_t object_checkNumeric()
 * @brief This function is used to check whether given uri has numeric 
 *        type or not
 *
 * @param contextP      pointer to lwm2m_context_t
 * @param uriP          pointer to lwm2m_uri_t
 *
 * @return  LWM2M response type
 *        
 */
uint8_t object_checkNumeric(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP, lwm2m_server_t * serverP)
{
  uint8_t result;
  lwm2m_object_t * targetP = NULL;
  lwm2m_data_t * dataP = NULL;
  int size;

  LOG_URI(uriP);
  if ( contextP == NULL || uriP == NULL ) 
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return LWM2M_400_BAD_REQUEST;
  }
  if (!LWM2M_URI_IS_SET_RESOURCE(uriP)) return LWM2M_405_METHOD_NOT_ALLOWED;

  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
  if (NULL == targetP) return LWM2M_404_NOT_FOUND;
  if (NULL == targetP->readFunc) return LWM2M_405_METHOD_NOT_ALLOWED;

  size = 1;
  dataP = lwm2m_data_new(1);
  if (dataP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;    
  dataP->id = uriP->resourceId;

  result = (uint8_t) targetP->readFunc(uriP->instanceId, &size, &dataP, targetP, INVALID_RES_INST_ID, serverP);
  if (result == LWM2M_205_CONTENT)
  {
    switch (dataP->type)
    {
      case LWM2M_TYPE_INTEGER:
      case LWM2M_TYPE_FLOAT:
        break;
      default:
        result = (uint8_t) LWM2M_405_METHOD_NOT_ALLOWED;
    }
  }

  lwm2m_data_free(1, dataP);

  return result;
}

#ifdef AUTHORIZATION_SUPPORT
qapi_Coap_Status_t checkAuthForRead(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP)
{
  acc_ctrl_oi_t     *acc_ctrl_oi = NULL;
  acc_ctrl_ri_t     *acc_ctrl_ri = NULL;
  lwm2m_object_t    *targetP = NULL;
  lwm2m_list_t      *instanceP = NULL;
  int                server_count = 0;

  if(!enable_acl_obj)
    return QAPI_CHANGED_2_04;

  if (contextP == NULL || uriP == NULL)
  {
    ds_iot_ulog_err(" fun [checkAuthForRead] LWM2M_LOG: NULL Error");
    return QAPI_BAD_REQUEST_4_00;
  }

  server_count = lwm2m_get_server_count(contextP, FALSE);

  /* activeServerP would be null only when called from observe_step, 
     hence there is no need for permission check aspermission would
     be checked while processing observe operation made by server */
  if (server_count > 1 && contextP->activeServerP != NULL && uriP->objectId != LWM2M_ACL_OBJECT_ID && uriP->objectId != 10255)  //work around for host device object. Later Remove 10255 
  {
    if (LWM2M_URI_IS_SET_INSTANCE(uriP))
    {
      acc_ctrl_oi = get_acc_cl_objectinst(contextP, uriP);
      acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
      if((NULL == acc_ctrl_ri) && (NULL != acc_ctrl_oi) && (contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner))
      {
        return QAPI_UNAUTHORIZED_4_01;
      }
      if (NULL != acc_ctrl_ri)
      {
        if(ACC_CTRL_PERM_READ != (ACC_CTRL_CAN_READ(acc_ctrl_ri->accCtrlValue))) 
        {
          if (!(acc_ctrl_ri->resInstId == 0 
                && contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner))
            return QAPI_UNAUTHORIZED_4_01;
        }
      }
    }
    else
    {
      lwm2m_uri_t uri ={0};
      uri.objectId = uriP->objectId;
      uri.flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID;
      targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
      if(targetP == NULL)
        return QAPI_CHANGED_2_04;
      instanceP = targetP->instanceList;
      while(instanceP != NULL)
      {
        uri.instanceId = instanceP->id;
        acc_ctrl_oi = get_acc_cl_objectinst(contextP, &uri);
        acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
        if((NULL == acc_ctrl_ri) && (NULL != acc_ctrl_oi) && (contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner))
        {
          return QAPI_UNAUTHORIZED_4_01;
        }
        if (NULL != acc_ctrl_ri)
        {
          if(ACC_CTRL_PERM_READ != (ACC_CTRL_CAN_READ(acc_ctrl_ri->accCtrlValue)))
          {
           if (!(acc_ctrl_ri->resInstId == 0
                 && contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner))
             return QAPI_UNAUTHORIZED_4_01;
          }
        }
        instanceP = instanceP->next; 
      }
    }
  }
  return QAPI_CHANGED_2_04;
}

#endif

/**
 * @fn uint8_t object_readData()
 * @brief This function is used to read object data
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to lwm2m_uri_t
 * @param sizeP     size of dataP pointer
 * @param dataP     double pointer to lwm2m_data_t
 *
 * @return  LWM2M response type
 *        
 */
uint8_t object_readData(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    int * sizeP,
    lwm2m_data_t ** dataP, lwm2m_server_t *serverP)
{
  uint8_t result;
  lwm2m_object_t * targetP = NULL;
#ifdef AUTHORIZATION_SUPPORT
  acc_ctrl_oi_t *acc_ctrl_oi = NULL;
  acc_ctrl_ri_t *acc_ctrl_ri = NULL;
  int            server_count = 0;
  int       *readable_inst_id = NULL;
#endif
  if ( contextP == NULL || uriP == NULL || sizeP == NULL || dataP == NULL ) 
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return LWM2M_400_BAD_REQUEST;
  }
  LOG_URI(uriP);

  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);

  if (NULL == targetP) return (uint8_t) LWM2M_404_NOT_FOUND;
  if (NULL == targetP->readFunc) return (uint8_t) LWM2M_405_METHOD_NOT_ALLOWED;
  if (targetP->instanceList == NULL) return (uint8_t) LWM2M_404_NOT_FOUND;
#ifdef AUTHORIZATION_SUPPORT
  server_count = lwm2m_get_server_count(contextP, FALSE);
#endif
  if (LWM2M_URI_IS_SET_INSTANCE(uriP))
  {
    if (NULL == lwm2m_list_find(targetP->instanceList, uriP->instanceId)) return (uint8_t) LWM2M_404_NOT_FOUND;
#ifdef AUTHORIZATION_SUPPORT
    /* activeServerP would be null only when called from observe_step, 
       hence there is no need for permission check as permission would
       be checked while processing observe operation made by server */
    if (server_count > 1 && contextP->activeServerP != NULL && uriP->objectId != LWM2M_ACL_OBJECT_ID && uriP->objectId != 10255)  //work around for host device object. Later Remove 10255 
    {
      acc_ctrl_oi = get_acc_cl_objectinst(contextP, uriP);
      if(!enable_acl_obj || NULL == acc_ctrl_oi)
      {
        ds_iot_ulog_high2("enable_acl_obj[%d] or no ACL object instance[%d], operation with full permission", enable_acl_obj, uriP->objectId);
        goto next;
      }  
      acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
      if(NULL == acc_ctrl_ri && contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner)
      {
        return QAPI_UNAUTHORIZED_4_01;
      }
      if (NULL != acc_ctrl_ri)
      {
        if(ACC_CTRL_PERM_READ != (ACC_CTRL_CAN_READ(acc_ctrl_ri->accCtrlValue))) 
        {
          if (!(acc_ctrl_ri->resInstId == 0 
                && contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner))
            return QAPI_UNAUTHORIZED_4_01;
        }
      }
    }
next:
#endif

    // single instance read
    if (LWM2M_URI_IS_SET_RESOURCE(uriP))
    {
      *sizeP = 1;
      *dataP = lwm2m_data_new(*sizeP);
      if (*dataP == NULL) return (uint8_t) LWM2M_500_INTERNAL_SERVER_ERROR;

      (*dataP)->id = uriP->resourceId;
    }
    if (LWM2M_URI_IS_SET_RESOURCE_INST(uriP))
      result = (uint8_t) targetP->readFunc(uriP->instanceId, sizeP, dataP, targetP, uriP->resourceInstId, serverP);
    else
      result = (uint8_t) targetP->readFunc(uriP->instanceId, sizeP, dataP, targetP, INVALID_RES_INST_ID, serverP);
  }
  else
  {
    // multiple object instances read
    lwm2m_list_t * instanceP;
    int i;
#ifdef AUTHORIZATION_SUPPORT
    int j;
    lwm2m_uri_t uri ={0};
    uri.objectId = uriP->objectId;
    uri.flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID;
#endif

    i = 0;
    //get the count of all instances as i
    for (instanceP = targetP->instanceList; instanceP != NULL ; instanceP = instanceP->next, i++);

    if (0 == i) return (uint8_t) LWM2M_404_NOT_FOUND;

#ifdef AUTHORIZATION_SUPPORT
    if(server_count > 1 && contextP->activeServerP != NULL && uriP->objectId != LWM2M_ACL_OBJECT_ID)
    {
      readable_inst_id = lwm2m_malloc(i * sizeof(int));
      if(NULL == readable_inst_id)
         return (uint8_t)LWM2M_500_INTERNAL_SERVER_ERROR;

      for(j = 0; j < i; j++)
        readable_inst_id[j] = -1; //initialize invalid instanceID -1

      for (i =0, instanceP = targetP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
      {
        /* activeServerP would be null only when called from observe_step, 
           hence there is no need for permission check aspermission would
           be checked while processing observe operation made by server */
        uri.instanceId = instanceP->id;
        acc_ctrl_oi = get_acc_cl_objectinst(contextP, &uri);
        if(!enable_acl_obj || NULL == acc_ctrl_oi)
        {
          ds_iot_ulog_high2("enable_acl_obj[%d] or no ACL object instance[%d], operation with full permission", enable_acl_obj, uriP->objectId);
          readable_inst_id[i++] = instanceP->id;
          continue;
        }
        acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
        /* 8.2.1. Obtaining Access Right of OMA1.1
          If the LwM2M Server is declared as the Access Control Owner of this
          Object Instance and there is no ACL Resource Instance for that LwM2M
          Server, then the LwM2M Client gets full access right.
        */
        if(NULL == acc_ctrl_ri) 
        {
          if(contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner)
          {
            readable_inst_id[i++] = instanceP->id;
          }
          continue;
        }
        else
        {
          /* 8.2.1. Obtaining Access Right of OMA1.1
             If the LwM2M Client has an ACL Resource Instance for the LwM2M Server,
             the LwM2M Client gets the access right from that ACL Resource Instance
          */
          if(acc_ctrl_ri->resInstId != 0)
          {
            if(ACC_CTRL_PERM_READ == (ACC_CTRL_CAN_READ(acc_ctrl_ri->accCtrlValue)))
              readable_inst_id[i++] = instanceP->id;
            continue;
          }
          else
          {
            /* 8.2.1. Obtaining Access Right of OMA1.1
               1) if the server is access control owner (resInstId 0 is for other servers)
                 If the LwM2M Server is declared as the Access Control Owner of this
                 Object Instance and there is no ACL Resource Instance for that LwM2M
                 Server, then the LwM2M Client gets full access right.
               2) if the server is not access control owner
                 If the LwM2M Server is not declared as the Access Control Owner
                 of this Object Instance and the LwM2M Client does not have the
                 ACL Resource Instance for that Server, the LwM2M Client gets
                 the access right from the ACL Resource Instance (ID:0) containing
                 the default access rights if it exists.
            */
            if(contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner ||
               (contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner &&
                 ACC_CTRL_PERM_READ == (ACC_CTRL_CAN_READ(acc_ctrl_ri->accCtrlValue))))
              readable_inst_id[i++] = instanceP->id;
            continue;
          }
        }
      }
      if(0 == i)
      {
        if(readable_inst_id != NULL)
        {
          lwm2m_free(readable_inst_id);
          readable_inst_id = NULL;
        }
        return QAPI_UNAUTHORIZED_4_01;
      }
    }
#endif

    *sizeP = i;
    *dataP = lwm2m_data_new(*sizeP);
    if (*dataP == NULL)
    {
#ifdef AUTHORIZATION_SUPPORT
      if(readable_inst_id != NULL)
      {
        lwm2m_free(readable_inst_id);
        readable_inst_id = NULL;
      }
#endif
      return (uint8_t)LWM2M_500_INTERNAL_SERVER_ERROR;
    }

    result = (uint8_t) LWM2M_205_CONTENT;
    instanceP = targetP->instanceList;
    i = 0;
    while (instanceP != NULL && result == LWM2M_205_CONTENT )
    {
#ifdef AUTHORIZATION_SUPPORT
      if(server_count > 1 && contextP->activeServerP != NULL && uriP->objectId != LWM2M_ACL_OBJECT_ID)
      {
        for(j = 0; j < *sizeP; j++)
         if(readable_inst_id[j] == instanceP->id) break;

        if(j == *sizeP)
        {
         instanceP = instanceP->next;
         continue;
        }
      }
#endif
      result = (uint8_t) targetP->readFunc(instanceP->id,
                                            (int*)&((*dataP)[i].value.asChildren.count),
                                            &((*dataP)[i].value.asChildren.array),
                                            targetP,
                                            INVALID_RES_INST_ID,
                                            serverP);
      (*dataP)[i].type = LWM2M_TYPE_OBJECT_INSTANCE;
      (*dataP)[i].id = instanceP->id;
      i++;
      instanceP = instanceP->next;
    }
  }
#ifdef AUTHORIZATION_SUPPORT
  if(readable_inst_id != NULL)
  {
    lwm2m_free(readable_inst_id);
    readable_inst_id = NULL;
  }
#endif
  LOG_DEBUG("result: %u.%2u, size: %d", (result & 0xFF) >> 5, (result & 0x1F), *sizeP);
  return result;
}

/**
 * @fn uint8_t object_read()
 * @brief This function is used to read object data
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to lwm2m_uri_t
 * @param formatP   pointer to media type
 * @param bufferP   double pointer to buffer
 * @param lengthP   pointer to the length 
 * @param dataP     double pointer to lwm2m_data_t
 * @param size      size of dataP pointer
 *
 * @return  LWM2M response type
 *        
 */
uint8_t object_read(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    lwm2m_media_type_t * formatP,
    uint8_t ** bufferP,
    size_t * lengthP,
    lwm2m_data_t **dataP,
    int *size,
    lwm2m_server_t *serverP
  )
{
  uint8_t result;
  if (contextP == NULL || uriP == NULL || formatP == NULL || bufferP == NULL
      || lengthP == NULL ) 
  { 
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return LWM2M_400_BAD_REQUEST;
  } 
  LOG_URI(uriP);
  ds_iot_ulog_high("Read operation on");
  PRINT_URI(uriP);
  result = object_readData(contextP, uriP, size, dataP, serverP);

  if (result == LWM2M_205_CONTENT)
  {
    *lengthP = lwm2m_data_serialize(uriP, *size, *dataP, formatP, bufferP);
    if (*lengthP == 0)
    {
      if(*formatP == LWM2M_CONTENT_OPAQUE)
        result = (uint8_t) LWM2M_406_NOT_ACCEPTABLE;
      else if (*formatP != LWM2M_CONTENT_TEXT
          || *size != 1
          || (*dataP)->type != LWM2M_TYPE_STRING
          || (*dataP)->value.asBuffer.length != 0)
      {
        result = (uint8_t) LWM2M_400_BAD_REQUEST;
      }
    }
  }

  ds_iot_ulog_high2("Sending Response %u.%2u for read on",(result & 0xFF) >> 5, (result & 0x1F));
  PRINT_URI(uriP);
  return result;
}

/**
 * @fn uint8_t object_write()
 * @brief This function is used to write object data
 *
 * @param contextP pointer to lwm2m_context_t
 * @param uriP     pointer to lwm2m_uri_t
 * @param format   pointer to media type
 * @param buffer   double pointer to buffer
 * @param lengtP   pointer to the length
 * @param write_method   PUT/POST
 *
 * @return  LWM2M response type
 *        
 */
uint8_t object_write(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    lwm2m_media_type_t format,
    qapi_Coap_Packet_t* buffer,
    size_t length,
    uint8_t write_method, bool bs_true)
{
  uint8_t result = QAPI_NO_ERROR;
  lwm2m_object_t * targetP;
  lwm2m_data_t * dataP = NULL;
  int size = 0;
#ifdef AUTHORIZATION_SUPPORT
  lwm2m_client_state_t lwm2m_state;
  acc_ctrl_oi_t *acc_ctrl_oi = NULL;
  acc_ctrl_ri_t *acc_ctrl_ri = NULL;
  int            server_count = 0;
#endif
  if( contextP == NULL || uriP == NULL || buffer == NULL ) 
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return LWM2M_400_BAD_REQUEST; 
  }
  LOG_URI(uriP);
  ds_iot_ulog_high("Write operation on");
  PRINT_URI(uriP);
#ifdef AUTHORIZATION_SUPPORT
#ifdef LWM2M_BOOTSTRAP
  if ( !( (contextP->bootstrapServer!=NULL && contextP->activeServerP->secObjInstID == contextP->bootstrapServer->secObjInstID)&& 
        (uriP->objectId == LWM2M_SECURITY_OBJECT_ID || uriP->objectId == LWM2M_SERVER_OBJECT_ID) )) 
#endif         
  {
    server_count = lwm2m_get_server_count(contextP, FALSE);
    if ( server_count > 1 && LWM2M_ACL_OBJECT_ID != uriP->objectId)
    {
      acc_ctrl_oi = get_acc_cl_objectinst(contextP, uriP);
      if(NULL == acc_ctrl_oi)
      {
        ds_iot_ulog_high1("No ACL object instance[%d], operation with full permission", uriP->objectId);
        goto next;
      }  
      acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
      if(NULL == acc_ctrl_ri && contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner) 
      {
        return QAPI_UNAUTHORIZED_4_01;
      }
      if (NULL != acc_ctrl_ri)
      {
        if(ACC_CTRL_PERM_WRITE != (ACC_CTRL_CAN_WRITE(acc_ctrl_ri->accCtrlValue))) 
        {
          if (!(acc_ctrl_ri->resInstId == 0 
                && contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner))
            return QAPI_UNAUTHORIZED_4_01;
        }
      }
    }
    else if(LWM2M_ACL_OBJECT_ID == uriP->objectId)
    {
      targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
      if (targetP == NULL) {
        return  (uint8_t) LWM2M_404_NOT_FOUND;
      }
      acc_ctrl_oi     =  (acc_ctrl_oi_t *)LWM2M_LIST_FIND(targetP->instanceList, uriP->instanceId);
      if (acc_ctrl_oi == NULL )
      {
        return  (uint8_t) LWM2M_404_NOT_FOUND;
      }
      lwm2m_state = lwm2m_get_client_state();
      if( (lwm2m_state == STATE_BOOTSTRAP_REQUIRED) || (lwm2m_state == STATE_BOOTSTRAPPING))
        goto next;
      if( contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner) {
        return QAPI_UNAUTHORIZED_4_01;
      }
    }  
  }

next:
#endif
  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
  if (NULL == targetP)
  {
    result = (uint8_t) LWM2M_404_NOT_FOUND;
  }
  else if (NULL == targetP->writeFunc)
  {
    result = (uint8_t) LWM2M_405_METHOD_NOT_ALLOWED;
  }
  else
  {
    size = lwm2m_data_parse(uriP, buffer->payload, length, format, &dataP);
    if (size == 0)
    {
      result = (uint8_t) LWM2M_400_BAD_REQUEST;
    }
  }
  if (result == QAPI_NO_ERROR)
  {
   if(dataP)
   {
      /* Copy blockwise transfer information to give it to user */
       dataP->value.asBuffer.block1_more = buffer->block1_more;
       dataP->value.asBuffer.block1_num =    buffer->block1_num;
       dataP->value.asBuffer.block1_size =buffer->block1_size;
       dataP->value.asBuffer.block1_offset =buffer->block1_offset;
       dataP->value.asBuffer.size1 = buffer->size1;
   }

    if (LWM2M_URI_IS_SET_RESOURCE_INST(uriP))
      result = (uint8_t) targetP->writeFunc(uriP->instanceId, size, dataP, targetP, uriP->resourceInstId, write_method, bs_true);
    else 
      result = (uint8_t) targetP->writeFunc(uriP->instanceId, size, dataP, targetP, INVALID_RES_INST_ID, write_method, bs_true);
    lwm2m_data_free(size, dataP);
  }

  ds_iot_ulog_high2("Sending Response %u.%2u for write on",(result & 0xFF) >> 5, (result & 0x1F));
  return result;
}

/**
 * @fn coap_status_t object_execute()
 * @brief This function is used to execute resource of an object
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to lwm2m_uri_t
 * @param buffer    pointer to buffer
 * @param length    length of buffer
 *
 * @return  LWM2M response type
 *        
 */
uint8_t object_execute(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    uint8_t * buffer,
    size_t length)
{
  lwm2m_object_t * targetP = NULL;
#ifdef AUTHORIZATION_SUPPORT
  acc_ctrl_oi_t *acc_ctrl_oi = NULL;
  acc_ctrl_ri_t *acc_ctrl_ri = NULL;
  int            server_count = 0;
#endif

  LOG_URI(uriP);
  if ( contextP == NULL || uriP == NULL )     // buffer can be NULL, no need to check for NULL
  { 
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return LWM2M_400_BAD_REQUEST;
  }
#ifdef AUTHORIZATION_SUPPORT
  server_count = lwm2m_get_server_count(contextP, FALSE);
  if (server_count > 1)
  {
    acc_ctrl_oi = get_acc_cl_objectinst(contextP, uriP);
    if(!enable_acl_obj || NULL == acc_ctrl_oi)
    {
	  ds_iot_ulog_high2("enable_acl_obj[%d] or no ACL object instance[%d], operation with full permission", enable_acl_obj, uriP->objectId);
      goto next;
    }  
    acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
    if(NULL == acc_ctrl_ri && contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner)
    {
      return QAPI_UNAUTHORIZED_4_01;
    }
    if (NULL != acc_ctrl_ri)
    {
      if(ACC_CTRL_PERM_EXECUTE != (ACC_CTRL_CAN_EXEC(acc_ctrl_ri->accCtrlValue))) 
      {
        if (!(acc_ctrl_ri->resInstId == 0 
              && contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner))
          return QAPI_UNAUTHORIZED_4_01;
      }
    }
  }
next:
#endif  
  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
  if (NULL == targetP) return (uint8_t) LWM2M_404_NOT_FOUND;
  if (NULL == targetP->executeFunc) return (uint8_t) LWM2M_405_METHOD_NOT_ALLOWED;
  if (!LWM2M_URI_IS_SET_INSTANCE(uriP)) return (uint8_t) LWM2M_405_METHOD_NOT_ALLOWED;
  if (targetP->instanceList == NULL) return (uint8_t) LWM2M_404_NOT_FOUND;
  if (LWM2M_URI_IS_SET_INSTANCE(uriP))
  {
    if (NULL == lwm2m_list_find(targetP->instanceList, uriP->instanceId))
      return (uint8_t)LWM2M_404_NOT_FOUND;
    if (!LWM2M_URI_IS_SET_RESOURCE(uriP))
      return (uint8_t)LWM2M_405_METHOD_NOT_ALLOWED;

    return (uint8_t) targetP->executeFunc(uriP->instanceId, uriP->resourceId, buffer, length, targetP);
  }
  return (uint8_t) LWM2M_405_METHOD_NOT_ALLOWED;
}

/**
 * @fn coap_status_t object_create()
 * @brief This function is used to create an object
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to lwm2m_uri_t
 * @param format    media type format
 * @param buffer    pointer to buffer
 * @param length    length of buffer
 *
 * @return  LWM2M response type
 *        
 */
uint8_t object_create(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    lwm2m_media_type_t format,
    uint8_t * buffer,
    size_t length)
{
  lwm2m_object_t * targetP = NULL;
  lwm2m_data_t * dataP = NULL;
  int size = 0;
  uint8_t result;
#ifdef AUTHORIZATION_SUPPORT 
  acc_ctrl_oi_t *acc_ctrl_oi = NULL;
  acc_ctrl_ri_t *acc_ctrl_ri = NULL;
    int          server_count = 0;
#endif  
  if( contextP == NULL || uriP == NULL || buffer == NULL ) 
  { 
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return LWM2M_400_BAD_REQUEST; 
  }

  LOG_URI(uriP);
  ds_iot_ulog_high("Create operation on");
  PRINT_URI(uriP);
  if (length == 0)
  {
    return (uint8_t) LWM2M_400_BAD_REQUEST;
  }
#ifdef AUTHORIZATION_SUPPORT
#ifdef LWM2M_BOOTSTRAP
  if ( !( (contextP->bootstrapServer!=NULL && contextP->activeServerP->secObjInstID == contextP->bootstrapServer->secObjInstID)&&
        (uriP->objectId == LWM2M_SECURITY_OBJECT_ID || uriP->objectId == LWM2M_SERVER_OBJECT_ID) )) 
#endif      
  {
    server_count = lwm2m_get_server_count(contextP, FALSE);
    /*multiple lwm2m servers*/
    if ( server_count > 1 && LWM2M_ACL_OBJECT_ID != uriP->objectId)
    {
      acc_ctrl_oi = get_acc_cl_object(contextP, uriP);
      if(NULL == acc_ctrl_oi)
      {
        ds_iot_ulog_high1("No ACL object instance[%d], operation with full permission", uriP->objectId);
        goto next;
      }
      acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
      if (NULL == acc_ctrl_ri || 0 == acc_ctrl_ri->resInstId ||
          (ACC_CTRL_PERM_CREATE != (ACC_CTRL_CAN_CREATE(acc_ctrl_ri->accCtrlValue))))
      {
        ds_iot_ulog_high("No ACL instance or not support Create");
        return QAPI_UNAUTHORIZED_4_01;
      }
    }
  }
next:
#endif
  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
  if (NULL == targetP) return (uint8_t) LWM2M_404_NOT_FOUND;
  if (NULL == targetP->createFunc) return (uint8_t) LWM2M_405_METHOD_NOT_ALLOWED;

  size = lwm2m_data_parse(uriP, buffer, length, format, &dataP);
  if (size <= 0 || NULL == dataP) return (uint8_t) LWM2M_400_BAD_REQUEST;

  switch (dataP[0].type)
  {
    case LWM2M_TYPE_OBJECT:
      result = LWM2M_400_BAD_REQUEST;
      goto exit;

    case LWM2M_TYPE_OBJECT_INSTANCE:
      if (size != 1 || (dataP[0].id >= LWM2M_MAX_ID))
      {
        result = LWM2M_400_BAD_REQUEST;
        goto exit;
      }
      if (NULL != lwm2m_list_find(targetP->instanceList, dataP[0].id))
      {
        // Instance already exists
        result = LWM2M_400_BAD_REQUEST;
        goto exit;
      }
      result = targetP->createFunc(dataP[0].id, dataP[0].value.asChildren.count, dataP[0].value.asChildren.array, targetP, FALSE);
      uriP->instanceId = dataP[0].id;
      uriP->flag |= LWM2M_URI_FLAG_INSTANCE_ID;
      break;

    default:
#ifdef LWM2M_BOOTSTRAP
      if ( !((contextP->bootstrapServer!=NULL && contextP->activeServerP->secObjInstID == contextP->bootstrapServer->secObjInstID) && (uriP->objectId == LWM2M_SECURITY_OBJECT_ID || uriP->objectId == LWM2M_SERVER_OBJECT_ID ||
              uriP->objectId == LWM2M_APN_CONN_PROFILE_OBJECT_ID) && (NULL == lwm2m_list_find(targetP->instanceList,uriP->instanceId )))) 
#endif  
      {
        uriP->instanceId = lwm2m_list_newId(targetP->instanceList);
        if( uriP->instanceId >= LWM2M_MAX_ID)
        {
           result = LWM2M_400_BAD_REQUEST;
           goto exit;
        }
      }
      uriP->flag |= LWM2M_URI_FLAG_INSTANCE_ID;
      result = targetP->createFunc(uriP->instanceId, size, dataP, targetP, FALSE);
      break;
  }

exit:
  lwm2m_data_free(size, dataP);

  LOG_DEBUG("result: %u.%2u", (result & 0xFF) >> 5, (result & 0x1F));

  return (uint8_t) result;
}

/**
 * @fn coap_status_t object_delete()
 * @brief This function is used to delete an object
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to lwm2m_uri_t
 *
 * @return  LWM2M response type
 *        
 */
uint8_t object_delete(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP)
{
  lwm2m_object_t * objectP = NULL;
  uint8_t result;
#ifdef AUTHORIZATION_SUPPORT
  acc_ctrl_oi_t *acc_ctrl_oi = NULL;
  acc_ctrl_ri_t *acc_ctrl_ri = NULL;
  lwm2m_uri_t    acc_ctrl_uri;
  int            server_count = 0;
#endif
  lwm2m_server_data_t lwm2m_info;
  if(contextP == NULL || uriP == NULL)
  { 
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return LWM2M_400_BAD_REQUEST;
  } 

  LOG_URI(uriP);
  ds_iot_ulog_high("Delete operation on");
  PRINT_URI(uriP);
#ifdef AUTHORIZATION_SUPPORT
  memset(&acc_ctrl_uri, 0x00, sizeof(acc_ctrl_uri));
  server_count = lwm2m_get_server_count(contextP, FALSE);
  if (
#ifdef LWM2M_BOOTSTRAP
      !( (contextP->bootstrapServer!=NULL && contextP->activeServerP->secObjInstID == contextP->bootstrapServer->secObjInstID)&& 
        (uriP->objectId == LWM2M_SECURITY_OBJECT_ID || uriP->objectId == LWM2M_SERVER_OBJECT_ID) ) &&
#endif 
      (uriP->objectId != LWM2M_ACL_OBJECT_ID))
  {
    acc_ctrl_oi = get_acc_cl_objectinst(contextP, uriP);
        /*multiple lwm2m servers*/
    if(server_count > 1)
    {
      if(NULL == acc_ctrl_oi)
      {
		ds_iot_ulog_high2("No ACL object instance[%d], operation with full permission", enable_acl_obj, uriP->objectId);
        goto next;
      }

      acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
      if(NULL == acc_ctrl_ri && contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner)
      {
        return QAPI_UNAUTHORIZED_4_01;
      }

      if (NULL != acc_ctrl_ri)
      {
        if(ACC_CTRL_PERM_DELETE != (ACC_CTRL_CAN_DELETE(acc_ctrl_ri->accCtrlValue))) 
        {
          if (!(acc_ctrl_ri->resInstId == 0
                && contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner))
            return QAPI_UNAUTHORIZED_4_01;
        }
      }
    }
    if(acc_ctrl_oi != NULL && contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner)
    {
      acc_ctrl_uri.objectId = LWM2M_ACL_OBJECT_ID;
      acc_ctrl_uri.instanceId = acc_ctrl_oi->objInstId;
      acc_ctrl_uri.flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID;
    }
  } 
  else if (uriP->objectId == LWM2M_ACL_OBJECT_ID)
  {
    acc_ctrl_oi = (acc_ctrl_oi_t *)contextP->acObjInstList;
    if(acc_ctrl_oi == NULL) return LWM2M_404_NOT_FOUND;
    acc_ctrl_oi = (acc_ctrl_oi_t*)LWM2M_LIST_FIND(acc_ctrl_oi , uriP->instanceId);
    if(acc_ctrl_oi == NULL ) return LWM2M_404_NOT_FOUND;
    if(acc_ctrl_oi ->accCtrlOwner != contextP->activeServerP->shortID)
      return QAPI_UNAUTHORIZED_4_01;
    if(contextP->acObjInstList == (lwm2m_list_t *)acc_ctrl_oi)
      contextP->acObjInstList = (lwm2m_list_t *)acc_ctrl_oi->next;
  }


next:
#endif
  objectP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
  if(NULL == objectP) return (uint8_t) LWM2M_404_NOT_FOUND;
  if(NULL == objectP->deleteFunc) return (uint8_t) LWM2M_405_METHOD_NOT_ALLOWED;


  if(LWM2M_URI_IS_SET_INSTANCE(uriP))
  {
	  /* Since object_delete is called only from server requests.*/
    result = (uint8_t) objectP->deleteFunc(uriP->instanceId, objectP,true);
    if(result == (uint8_t)LWM2M_202_DELETED)
    {
      ds_iot_ulog_high2("Delete on %d / %d ",objectP->objID,uriP->instanceId);
      observe_delete(contextP, uriP);
      /* Forward "delete" request to the extensible OEM application */
      memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t));
      lwm2m_info.msg_type = QAPI_NET_LWM2M_DELETE_REQ_E;

      /* Object info */
      lwm2m_info.obj_info.obj_id = uriP->objectId;
      lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
      
      /* Object instance info */
      lwm2m_info.obj_info.obj_inst_id = uriP->instanceId;
      lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_INSTANCE_ID_E;

      lwm2m_client_application_notification(&lwm2m_info);
#ifdef AUTHORIZATION_SUPPORT
      if(LWM2M_URI_IS_SET_INSTANCE(&acc_ctrl_uri))
      {
        object_delete(contextP, &acc_ctrl_uri);
      }
#endif
    }
  }
  else
  {
    lwm2m_list_t * instanceP;

    result = (uint8_t) LWM2M_202_DELETED;
    instanceP = objectP->instanceList;
    while (NULL != instanceP
        && result == LWM2M_202_DELETED)
    {
	  /* Since object_delete is called only from server requests.*/
      result = (uint8_t) objectP->deleteFunc(instanceP->id, objectP,true);
      if(result == (uint8_t)LWM2M_202_DELETED)
      {
        uriP->flag |= LWM2M_URI_FLAG_INSTANCE_ID;
        observe_delete(contextP, uriP);
        uriP->flag &= ~LWM2M_URI_FLAG_INSTANCE_ID;
        /* Forward "delete" request to the extensible OEM application */
        memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t));
        lwm2m_info.msg_type = QAPI_NET_LWM2M_DELETE_REQ_E;
        
        /* Object info */
        lwm2m_info.obj_info.obj_id = uriP->objectId;
        lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
        
        /* Object instance info */
        lwm2m_info.obj_info.obj_inst_id = instanceP->id;
        lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_INSTANCE_ID_E;
        
        lwm2m_client_application_notification(&lwm2m_info);
      }
      ds_iot_ulog_high2("Delete on %d / %d ",objectP->objID,instanceP->id);
      instanceP = objectP->instanceList;
    }
  }

  return result;
}

/**
 * @fn coap_status_t object_delete_bootstrap()
 * @brief This function is used to delete an object during bootstrap operation
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to lwm2m_uri_t
 *
 * @return  LWM2M response type
 *
 */
uint8_t object_delete_bootstrap(lwm2m_context_t * contextP,
                                lwm2m_uri_t * uriP)
{
  lwm2m_object_t * objectP = NULL;
  uint8_t result;
  server_instance_t *targetP = NULL;
  security_instance_t *secP = NULL;
  lwm2m_list_t *temp_list;
  uint8_t inst = 0, max_inst = 0;
  uint16_t ssid = 0;
  lwm2m_server_data_t lwm2m_info;

  if( contextP == NULL || uriP == NULL ) 
  { 
    LOG_DEBUG(" fun [%s] %s",__func__, nullError);
    return LWM2M_400_BAD_REQUEST;
  }

  LOG_URI(uriP);

  objectP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
  if (NULL == objectP) return (uint8_t) LWM2M_400_BAD_REQUEST;
  if (NULL == objectP->deleteFunc) return (uint8_t) LWM2M_400_BAD_REQUEST;

  LOG_DEBUG("Entering");

  if (LWM2M_URI_IS_SET_INSTANCE(uriP))
  {
    secP = (security_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, uriP->instanceId);
    if(secP != NULL)
    {
      ssid = secP->shortID;
    }
    else
      return (uint8_t) LWM2M_400_BAD_REQUEST;

	/* Since delete from Bootstrap server should also be persisted.*/
    result = (uint8_t) objectP->deleteFunc(uriP->instanceId, objectP, true);
    if(result == (uint8_t)LWM2M_202_DELETED)
    {
      observe_delete(contextP, uriP);
      /* Forward "delete" request to the extensible OEM application */
      memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t));
      lwm2m_info.msg_type = QAPI_NET_LWM2M_DELETE_REQ_E;
      
      /* Object info */
      lwm2m_info.obj_info.obj_id = uriP->objectId;
      lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
      
      /* Object instance info */
      lwm2m_info.obj_info.obj_inst_id = uriP->instanceId;
      lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_INSTANCE_ID_E;
      
      lwm2m_client_application_notification(&lwm2m_info);
    }
    ds_iot_ulog_high2(" Delete on %d / %d ",objectP->objID,uriP->instanceId);
  }
  else
  {
    lwm2m_list_t * instanceP;

    result = (uint8_t) LWM2M_202_DELETED;
    instanceP = objectP->instanceList;
    while (NULL != instanceP && result == LWM2M_202_DELETED)
    {
	  /* Since delete from Bootstrap server should also be persisted.*/
      result = (uint8_t) objectP->deleteFunc(instanceP->id, objectP, true);

      if(result == (uint8_t)LWM2M_202_DELETED)
      {
        uriP->flag |= LWM2M_URI_FLAG_INSTANCE_ID;
        observe_delete(contextP, uriP);
        uriP->flag &= ~LWM2M_URI_FLAG_INSTANCE_ID;
        /* Forward "delete" request to the extensible OEM application */
        memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t));
        lwm2m_info.msg_type = QAPI_NET_LWM2M_DELETE_REQ_E;
        
        /* Object info */
        lwm2m_info.obj_info.obj_id = uriP->objectId;
        lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
        
        /* Object instance info */
        lwm2m_info.obj_info.obj_inst_id = instanceP->id;
        lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_INSTANCE_ID_E;
        
        lwm2m_client_application_notification(&lwm2m_info);
      }
      ds_iot_ulog_high2(" Delete on %d / %d ",objectP->objID,instanceP->id);
      instanceP = objectP->instanceList;
    }
  }

  if(uriP->objectId == LWM2M_SECURITY_OBJECT_ID)
  {
    objectP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, LWM2M_SERVER_OBJECT_ID);
    if (NULL == objectP) return (uint8_t) LWM2M_400_BAD_REQUEST;
    if (NULL == objectP->deleteFunc) return (uint8_t) LWM2M_400_BAD_REQUEST;

    temp_list = objectP->instanceList;
    for(max_inst = 0;temp_list !=NULL;max_inst++)
    {
      max_inst = temp_list->id;
      temp_list = temp_list->next;
    }
    do
    {
      targetP = (server_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, inst);
      inst++;
      if (targetP == NULL)
      {
        continue;
      }
      if(targetP->shortServerId == ssid)
      {
        lwm2m_remove_object_instance(LWM2M_SERVER_OBJECT_ID, inst-1,false);
        ds_iot_ulog_high2(" Delete on %d / %d ",LWM2M_SERVER_OBJECT_ID,inst-1);
        break;
      }
    }while(inst < max_inst);
  }

  LOG_DEBUG("result: %u.%2u", (result & 0xFF) >> 5, (result & 0x1F));
  return result;
}

/**
 * @fn coap_status_t object_discover()
 * @brief This function is used to perform discover operation an object
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to lwm2m_uri_t
 * @param bufferP   double pointer to buffer
 * @param lengthP   pointer to the length
 *
 * @return  LWM2M response type
 *        
 */
uint8_t object_discover(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    uint8_t ** bufferP,
    size_t * lengthP, lwm2m_server_t *serverP)
{
  uint8_t result = LWM2M_NO_ERROR;
  lwm2m_object_t * targetP = NULL;
  lwm2m_data_t * dataP = NULL;
  int size = 0;
  if( contextP == NULL || uriP == NULL || bufferP == NULL 
      || lengthP == NULL ) 
  { 
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return LWM2M_400_BAD_REQUEST; 
  }
  LOG_URI(uriP);
  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
  if (NULL == targetP) return (uint8_t) LWM2M_404_NOT_FOUND;
  if (NULL == targetP->discoverFunc) return (uint8_t)LWM2M_405_METHOD_NOT_ALLOWED;
  if (!LWM2M_URI_IS_SET_INSTANCE(uriP) && (targetP->instanceList == NULL)) 
  {
    int len = -1;

    len = discover_serialize_wo_inst(contextP, uriP, bufferP);
    if (len <= 0) result = (uint8_t)LWM2M_500_INTERNAL_SERVER_ERROR;
    else
    {
      *lengthP = len;
      result = LWM2M_205_CONTENT;
    }
    return result;
  }
  else if (targetP->instanceList == NULL)
  {
    return (uint8_t) LWM2M_404_NOT_FOUND;
  }

  if (LWM2M_URI_IS_SET_INSTANCE(uriP))
  {
    if (NULL == lwm2m_list_find(targetP->instanceList, uriP->instanceId)) return (uint8_t) LWM2M_404_NOT_FOUND;
    // single instance read
    if (LWM2M_URI_IS_SET_RESOURCE(uriP))
    {
      size = 1;
      dataP = lwm2m_data_new(size);
      if (dataP == NULL) return (uint8_t) LWM2M_500_INTERNAL_SERVER_ERROR;

      dataP->id = uriP->resourceId;
    }

    result = (uint8_t) targetP->discoverFunc(uriP->instanceId, &size, &dataP, targetP, serverP);
  }
  else
  {
    // multiple object instances read
    lwm2m_list_t * instanceP = NULL;
    int i;

    size = 0;
    for (instanceP = targetP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
    {
      size++;
    }

    dataP = lwm2m_data_new(size);
    if (dataP == NULL) return (uint8_t) LWM2M_500_INTERNAL_SERVER_ERROR;
    result = (uint8_t) LWM2M_205_CONTENT;
    instanceP = targetP->instanceList;
    i = 0;
    while (instanceP != NULL && result == LWM2M_205_CONTENT)
    {
      result = (uint8_t) targetP->discoverFunc(instanceP->id, (int*)&(dataP[i].value.asChildren.count), &(dataP[i].value.asChildren.array), targetP, serverP);
      dataP[i].type = LWM2M_TYPE_OBJECT_INSTANCE;
      dataP[i].id = instanceP->id;
      i++;
      instanceP = instanceP->next;
    }
  }

  if (result == LWM2M_205_CONTENT)
  {
    int len;

    len = discover_serialize(contextP, uriP, size, dataP, bufferP);
    if (len <= 0) result = (uint8_t)LWM2M_500_INTERNAL_SERVER_ERROR;
    else *lengthP = len;
  }
  lwm2m_data_free(size, dataP);

  LOG_DEBUG("result: %u.%2u", (result & 0xFF) >> 5, (result & 0x1F));

  return result;
}

/**
 * @fn validate_ex_object_permissions()
 * @brief Function is used to validate the permissions of extensible objects.
 *
 * @param contextP  LWM2M context 
 * @param uriP      URI information 
 * @param msg_type  DL message type 
 *
 * @return  LWM2M response type
 *        
 */
uint8_t validate_ex_object_permissions(lwm2m_context_t *contextP,
    lwm2m_uri_t * uriP,
    qapi_Net_LWM2M_DL_Msg_t msg_type)
{
#ifdef AUTHORIZATION_SUPPORT
  acc_ctrl_oi_t *acc_ctrl_oi = NULL;
  acc_ctrl_ri_t *acc_ctrl_ri = NULL;
  int            server_count = 0;

  if(!enable_acl_obj)
  	return QAPI_NO_ERROR;
  
  if ((contextP == NULL) || 
      (uriP == NULL))
  { 
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return QAPI_BAD_REQUEST_4_00;
  }

  server_count = lwm2m_get_server_count(contextP, FALSE);
  if(server_count > 1)
  {
    switch (msg_type)
    {
      case QAPI_NET_LWM2M_OBSERVE_REQ_E:
      case QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E:
      case QAPI_NET_LWM2M_READ_REQ_E:
      {
        /* Object instance level */ 
        acc_ctrl_oi = get_acc_cl_objectinst(contextP, uriP);
        if (NULL == acc_ctrl_oi)
        {
          ds_iot_ulog_high1("No ACL object instance[%d], operation with full permission", uriP->objectId);
          return QAPI_NO_ERROR;      
        }
    
        /* Resource instance level */ 
        acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
        if ((NULL == acc_ctrl_ri) && 
            (contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner))
          return QAPI_UNAUTHORIZED_4_01;
        
        /* Read ACL permissions */ 
        if (NULL != acc_ctrl_ri)
        {
          if (ACC_CTRL_PERM_READ != (ACC_CTRL_CAN_READ(acc_ctrl_ri->accCtrlValue))) 
          {
            if (!((acc_ctrl_ri->resInstId == 0) && 
                  (contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner)))
              return QAPI_UNAUTHORIZED_4_01;
           }
         }
  
        break;
      }        
    
      case QAPI_NET_LWM2M_WRITE_PARTIAL_UPDATE_REQ_E:
      case QAPI_NET_LWM2M_WRITE_REPLACE_REQ_E:
      {
        /* Object instance level */ 
        acc_ctrl_oi = get_acc_cl_objectinst(contextP, uriP);
        if (NULL == acc_ctrl_oi)
        {
          ds_iot_ulog_high1("No ACL object instance[%d], operation with full permission", uriP->objectId);
          return QAPI_NO_ERROR;      
        }
  
        /* Resource instance level */ 
        acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
        if ((NULL == acc_ctrl_ri) && 
            (contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner)) 
          return QAPI_UNAUTHORIZED_4_01;
         
        /* Write ACL permissions */  
        if (NULL != acc_ctrl_ri)
        {
          if (ACC_CTRL_PERM_WRITE != (ACC_CTRL_CAN_WRITE(acc_ctrl_ri->accCtrlValue))) 
          {
            if (!((acc_ctrl_ri->resInstId == 0) &&
                  (contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner)))
              return QAPI_UNAUTHORIZED_4_01;
          }
        }
       
        break;
      }        
  
      case QAPI_NET_LWM2M_EXECUTE_REQ_E:
      {
        /* Object instance level */
         acc_ctrl_oi = get_acc_cl_objectinst(contextP, uriP);
         if (NULL == acc_ctrl_oi)
         {
           ds_iot_ulog_high1("No ACL object instance[%d], operation with full permission", uriP->objectId);
           return QAPI_NO_ERROR;      
         }
  
         /* Resource instance level */  
         acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
         if ((NULL == acc_ctrl_ri) && 
             (contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner))
           return QAPI_UNAUTHORIZED_4_01;
         
         /* Execute ACL permissions */
         if (NULL != acc_ctrl_ri)
         {
           if (ACC_CTRL_PERM_EXECUTE != (ACC_CTRL_CAN_EXEC(acc_ctrl_ri->accCtrlValue))) 
           {
             if (!((acc_ctrl_ri->resInstId == 0) && 
                   (contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner)))
               return QAPI_UNAUTHORIZED_4_01;
           }
         }

         break; 
      }
  
      case QAPI_NET_LWM2M_CREATE_REQ_E:
      {
       /* Object instance level - No permissions(allow all servers) */ 
       acc_ctrl_oi = get_acc_cl_object(contextP, uriP);
       if (NULL == acc_ctrl_oi)
         return QAPI_NO_ERROR; 

       /* Resource instance level */  
       acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
       if ((NULL == acc_ctrl_ri) && 
           (contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner))
         return QAPI_UNAUTHORIZED_4_01;

       /* Create ACL permissions */
       if (NULL != acc_ctrl_ri)
       {
         if (ACC_CTRL_PERM_CREATE != (ACC_CTRL_CAN_CREATE(acc_ctrl_ri->accCtrlValue)))
         {
           if (!((acc_ctrl_ri->resInstId == 0) && 
                   (contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner)))
             return QAPI_UNAUTHORIZED_4_01;
         }
       }

       break;
      }

      case QAPI_NET_LWM2M_DELETE_REQ_E:
      {
       /* Object instance level - No permissions(allow all servers) */ 
       acc_ctrl_oi = get_acc_cl_objectinst(contextP, uriP);
       if (NULL == acc_ctrl_oi)
         return QAPI_NO_ERROR;

       /* Resource instance level */ 
       acc_ctrl_ri = get_acc_cl_resourceinst(contextP, acc_ctrl_oi);
       if ((NULL == acc_ctrl_ri) && 
           (contextP->activeServerP->shortID != acc_ctrl_oi->accCtrlOwner))
         return QAPI_UNAUTHORIZED_4_01;

       /* Delete ACL permissions */  
       if (NULL != acc_ctrl_ri)
       {
         if (ACC_CTRL_PERM_DELETE != (ACC_CTRL_CAN_DELETE(acc_ctrl_ri->accCtrlValue))) 
         {
           if (!((acc_ctrl_ri->resInstId == 0) && 
                 (contextP->activeServerP->shortID == acc_ctrl_oi->accCtrlOwner)))
             return QAPI_UNAUTHORIZED_4_01;
         }
       }

       break;
      }

      default: 
        return QAPI_BAD_REQUEST_4_00;
    }
  }
#endif
  return QAPI_NO_ERROR;
}

/**
 * @fn bool object_isInstanceNew()
 * @brief This function is used to find the instance of a given object id
 *
 * @param contextP      pointer to lwm2m_context_t
 * @param objectId      object id to search in object list
 * @param instanceId    instance id to search in instance list
 *
 * @return    true on success
 *            false on failure
 */
bool object_isInstanceNew(lwm2m_context_t *contextP,
    uint16_t objectId,
    uint16_t instanceId)
{
  lwm2m_object_t * targetP = NULL;

  LOG_DEBUG("Entering");
  if( contextP == NULL )
  { 
    ds_iot_ulog_err(" fun [object_isInstanceNew] LWM2M_LOG: NULL Error");
    return LWM2M_400_BAD_REQUEST;
  }

  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, objectId);
  if (targetP != NULL)
  {
    if (NULL != lwm2m_list_find(targetP->instanceList, instanceId))
    {
      return false;
    }
  }

  return true;
}

/**
 * @fn static int prv_getObjectTemplate()
 * @brief This function is used to get the template of the object
 *
 * @param buffer    pointer to buffer
 * @param length    length of buffer
 * @param id        object id 
 *
 * @return    index on success
 *              -1 on failure
 *        
 */
static int prv_getObjectTemplate(uint8_t * buffer,
    size_t length,
    uint16_t id)
{
  int index;
  int result;

  if (buffer == NULL || length < REG_OBJECT_MIN_LEN) return -1;

  buffer[0] = '<';
  buffer[1] = '/';
  index = 2;

  result = utils_intCopy((char *)buffer + index, length - index, id);
  if (result < 0) return -1;
  index += result;

  if (length - index < REG_OBJECT_MIN_LEN - 3) return -1;
  buffer[index] = '/';
  index++;

  return index;
}

/**
 * @fn int get_registerPayload_size()
 * @brief This function is used to get register payload size
 *
 * @param contextP  pointer to lwm2m_context_t
 *
 * @return    size of payload on success
 *              0 on failure
 *        
 */

int get_registerPayload_size(lwm2m_context_t * contextP)
{
  size_t size = 0;
  int result = 0;
  lwm2m_object_t * objectP = NULL;
  size_t objectTemplateLen = 0;
  lwm2m_handle_info_t *curr_handle = NULL;
  
  if ( contextP == NULL) 
  {
    LOG_DEBUG(" fun [%s] %s\n",__func__,nullError);
    return 0;
  }

  if ((contextP->altPath != NULL) && (contextP->altPath[0] != 0))
  {
    result = strlen(contextP->altPath);
  }
  else
  {
    result = REG_DEFAULT_PATH_SIZE;
  }
  if (result < 0) return 0;

  size = REG_START_SIZE + result + REG_LWM2M_RESOURCE_TYPE_LEN;
  
  /* Standard Objects */ 
  for (objectP = contextP->objectList; objectP != NULL; objectP = objectP->next)
  {
    if (objectP->objID == LWM2M_SECURITY_OBJECT_ID) continue;

    objectTemplateLen = LINK_ITEM_START_SIZE + LINK_URI_SEPARATOR_SIZE + utils_numDigit(objectP->objID) + LINK_URI_SEPARATOR_SIZE;
    if (objectP->instanceList == NULL)
    {
      size += objectTemplateLen - LINK_URI_SEPARATOR_SIZE + REG_PATH_END_SIZE;
    }
    else
    {
      lwm2m_list_t * targetP;
      for (targetP = objectP->instanceList ; targetP != NULL ; targetP = targetP->next)
      {
        size += objectTemplateLen + utils_numDigit(targetP->id) + REG_PATH_END_SIZE;
      }
    }
  }  
      
  qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
  curr_handle = data.glwm2m_app_ctx.handles;  

  /* Extensible Objects */ 
  while (curr_handle)
  {    
    lwm2m_app_obj_info_t *curr_obj_list = curr_handle->obj_info; 

    while (curr_obj_list)
    {
      lwm2m_list_t *curr_inst_list = curr_obj_list->instance_list; 

      if(lwm2m_is_standard_object(curr_obj_list->object_ID)) 
      {
        curr_obj_list = curr_obj_list->next;
        continue;
      }
      
      objectTemplateLen = LINK_ITEM_START_SIZE + LINK_URI_SEPARATOR_SIZE  + utils_numDigit(curr_obj_list->object_ID) + LINK_URI_SEPARATOR_SIZE ; 
      if (curr_inst_list == NULL )
      {
        size += objectTemplateLen - LINK_URI_SEPARATOR_SIZE + REG_PATH_END_SIZE;
      }
      else
      {
        while (curr_inst_list )
        {
           size += objectTemplateLen + utils_numDigit(curr_inst_list->id) + REG_PATH_END_SIZE;
           curr_inst_list = curr_inst_list->next; 
        }
      }
      curr_obj_list = curr_obj_list->next; 
    }
    curr_handle = curr_handle->next;
  }
  
  qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
  if (size > 0)
  {
    size = size - 1;  // remove trailing ','
  }
  return size;
}


/**
 * @fn int object_getRegisterPayload()
 * @brief This function is used to get register payload
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param buffer    pointer to buffer
 * @param length    length of buffer
 *
 * @return    index on success
 *              0 on failure
 *        
 */
int object_getRegisterPayload(lwm2m_context_t * contextP, lwm2m_server_t * server,
    uint8_t * buffer,
    size_t bufferLen)
{
  // index can not be greater than bufferLen
  size_t index = 0;
  int result = 0;
  lwm2m_object_t * objectP = NULL;
  size_t start = 0;
  size_t length = 0;
  lwm2m_handle_info_t *curr_handle = NULL;
  
  LOG_DEBUG("Entering");
  if ( contextP == NULL || buffer == NULL || server == NULL) 
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return 0;
  }

  result = utils_stringCopy((char *)buffer, bufferLen, REG_START);
  if (result < 0) return 0;
  index += result;

  if ((contextP->altPath != NULL)
      && (contextP->altPath[0] != 0))
  {
    result = utils_stringCopy((char *)buffer + index, bufferLen - index, contextP->altPath);
  }
  else
  {
    result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_DEFAULT_PATH);
  }
  if (result < 0) return 0;
  index += result;

  result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_LWM2M_RESOURCE_TYPE);
  if (result < 0) return 0;
  index += result;

  /* Standard Objects */ 
  for (objectP = contextP->objectList; objectP != NULL; objectP = objectP->next)
  {
    if (objectP->objID == LWM2M_SECURITY_OBJECT_ID) continue;

    start = index;
    result = prv_getObjectTemplate(buffer + index, bufferLen - index, objectP->objID);
    if (result < 0) return 0;
    length = result;
    index += length;

    if (objectP->instanceList == NULL)
    {
      index--;
      result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_PATH_END);
      if (result < 0) return 0;
      index += result;
    }
    else
    {
      lwm2m_list_t * targetP;

      for (targetP = objectP->instanceList ; targetP != NULL ; targetP = targetP->next)
      {

        if (index != start + length)
        {
          if (bufferLen - index <= length) return 0;
          LWM2M_MEMCPY(buffer + index, bufferLen - index, buffer + start, length);
          index += length;
        }

        result = utils_intCopy((char *)buffer + index, bufferLen - index, targetP->id);
        if (result < 0) return 0;
        index += result;

        result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_PATH_END);
        if (result < 0) return 0;
        index += result;
      }
    }
  }  
      
  qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

  /* Populate the extensible object information in the registration update */  
  curr_handle = data.glwm2m_app_ctx.handles;
  storeRegisteredExtObj(server, false); /*ensure registering object is cleanup before new register */
  
  while (curr_handle)
  {    
    lwm2m_app_obj_info_t *curr_obj_list = curr_handle->obj_info; 

    /* Retrieve the object information for the handle and traverse through the object list (custom objects only) */
    while (curr_obj_list)
    {
      lwm2m_list_t *curr_inst_list = curr_obj_list->instance_list; 
      
      if (curr_inst_list == NULL && (!(lwm2m_is_standard_object(curr_obj_list->object_ID))))
      {
        /* Retrieve the object information from the object list and populate only object id */
        start = index;
        result = prv_getObjectTemplate(buffer + index, bufferLen - index, curr_obj_list->object_ID);
        if (result < 0) return 0;
        length = result;
        index += length;
        storeRegisteringExtObj( server, curr_obj_list->object_ID, 0, false);
        index--;
        result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_PATH_END);
        if (result < 0) return 0;
        index += result;
      }
      else
      {
        /* Retrieve the object instance list information from the object list and populate the update information */
        while ((curr_inst_list) && 
               (!(lwm2m_is_standard_object(curr_obj_list->object_ID))))
        {
          start = index;
          result = prv_getObjectTemplate(buffer + index, bufferLen - index, curr_obj_list->object_ID);
          if (result < 0) return 0;
          length = result;
          index += length;

          if (index != start + length)
          {
            if (bufferLen - index <= length) return 0;
            LWM2M_MEMCPY(buffer + index, bufferLen - index, buffer + start, length);
            index += length;
          }

          storeRegisteringExtObj( server, curr_obj_list->object_ID, curr_inst_list->id, true);
   
          result = utils_intCopy((char *)buffer + index, bufferLen - index, curr_inst_list->id);
          if (result < 0) return 0;
          index += result;
   
          result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_PATH_END);
          if (result < 0) return 0;
          index += result;
  
          curr_inst_list = curr_inst_list->next; 
        }
      }
      curr_obj_list = curr_obj_list->next; 
    }
    
    curr_handle = curr_handle->next;
  }

  qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

  if (index > 0)
  {
    index = index - 1;  // remove trailing ','
  }

  buffer[index] = 0;

  return index;
}

/**
 * @fn static lwm2m_list_t * prv_findServerInstance()
 * @brief This function is used to find server instance in instance list
 *
 * @param objectP   pointer to lwm2m_object_t
 * @param shortID   id of server to be searched
 *
 * @return    server instance pointer on success
 *              NULL on failure
 *        
 */
lwm2m_list_t * prv_findServerInstance(lwm2m_object_t * objectP, uint16_t shortID)
{
  lwm2m_list_t * instanceP = NULL;

  if (objectP == NULL )  
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return NULL;
  } 

  instanceP = objectP->instanceList;
  while (NULL != instanceP)
  {
    int64_t value;
    lwm2m_data_t * dataP;
    int size;

    size = 1;
    dataP = lwm2m_data_new(size);
    if (dataP == NULL) return NULL;
    dataP->id = LWM2M_SERVER_SHORT_ID_ID;

    if (objectP->readFunc(instanceP->id, &size, &dataP, objectP, INVALID_RES_INST_ID, NULL) !=  LWM2M_205_CONTENT)
    {
      lwm2m_data_free(size, dataP);
      return NULL;
    }

    if (1 == lwm2m_data_decode_int(dataP, &value))
    {
      if (value == shortID)
      {
        lwm2m_data_free(size, dataP);
        break;
      }
    }
    lwm2m_data_free(size, dataP);
    instanceP = instanceP->next;
  }

  return instanceP;
}

/**
 * @fn static int prv_getDefMinMaxPeriod()
 * @brief This function us used to get Def MinPeriod and Def Max Period 
 * 
 * @param objectP       pointer to lwm2m_object_t
 * @param instanceID    instance id
 *  @param targetP      pointer to lwm2m_server_t
 *
 * @return    0 on success
 *              -1 on failure
 *        
 */
static int prv_getDefMinMaxPeriod(lwm2m_object_t * objectP,
    uint16_t instanceID,
    lwm2m_server_t * targetP)
{
  lwm2m_data_t * dataP = NULL;
  int size = 0;
  int64_t value = 0;
  if ( objectP == NULL || targetP == NULL ) 
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return EFAILURE;
  }     
  size = 2; // we are going to read 2 resources 
  dataP = lwm2m_data_new(size);
  if (dataP == NULL) return EFAILURE;
  dataP[0].id = LWM2M_SERVER_MIN_PERIOD_ID;
  dataP[1].id = LWM2M_SERVER_MAX_PERIOD_ID;

  if (objectP->readFunc(instanceID, &size, &dataP, objectP, INVALID_RES_INST_ID, targetP) != LWM2M_205_CONTENT) 
  {
    lwm2m_data_free(size, dataP);
    return EFAILURE;
  }

  if (0 == lwm2m_data_decode_int(dataP, &value)
      || value < 0 || value >0xFFFFFFFF)             // This is an implementation limit
  {
    lwm2m_data_free(size, dataP);
    return EFAILURE;
  }
  targetP->defaultMinPeriod = value;
  if (0 == lwm2m_data_decode_int(&dataP[1], &value)
      || value < 0 || value >0xFFFFFFFF)             // This is an implementation limit
  {
    lwm2m_data_free(size, dataP);
    return EFAILURE;
  }
  targetP->defaultMaxPeriod = value;

  lwm2m_data_free(size, dataP);
  return ESUCCESS;  
}

/**
 * @fn static int prv_getMandatoryInfo()
 * @brief This function is used to get mandatory information
 *
 * @param objectP       pointer to lwm2m_object_t
 * @param instanceID    instance id
 *  @param targetP      pointer to lwm2m_server_t
 *
 * @return    0 on success
 *              -1 on failure
 *        
 */
static int prv_getMandatoryInfo(lwm2m_object_t * objectP,
    uint16_t instanceID,
    lwm2m_server_t * targetP)
{
  lwm2m_data_t * dataP = NULL;
  int size = 0;
  int64_t value = 0;

  if ( objectP == NULL || targetP == NULL ) 
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return -1;
  }     

  size = 2;
  dataP = lwm2m_data_new(size);
  if (dataP == NULL) return -1;

  dataP[0].id = LWM2M_SERVER_LIFETIME_ID;
  dataP[1].id = LWM2M_SERVER_BINDING_ID;

  if (objectP->readFunc(instanceID, &size, &dataP, objectP, INVALID_RES_INST_ID, targetP) != LWM2M_205_CONTENT) 
  {
    lwm2m_data_free(size, dataP);
    return -1;
  }

  /* Lifetime */ 
  if ((0 == lwm2m_data_decode_int(dataP, &value)) ||
      (value < 0) || 
      (value > 0xFFFFFFFF))             // This is an implementation limit
  {
    lwm2m_data_free(size, dataP);
    return -1;
  }
  targetP->lifetime = value;
  ds_iot_ulog_high2(" Lifetime of SSID: %d is %d",targetP->shortID,targetP->lifetime);

  /* Bindings */
  targetP->binding = utils_stringToBinding(dataP[1].value.asBuffer.buffer, dataP[1].value.asBuffer.length);
  if (targetP->binding == BINDING_UNKNOWN)
  {
    lwm2m_data_free(size, dataP);
    return -1;
  }
  lwm2m_data_free(size, dataP);
  return 0;
}

 int prv_getServerInfo(lwm2m_object_t * objectP,
    uint16_t instanceID,
    lwm2m_server_t * targetP)
{
  lwm2m_data_t * dataP = NULL;
  int size = 0;
  objlink_t obj_link = 0;

  if ( objectP == NULL || targetP == NULL ) 
  {
    LOG_DEBUG(" fun [%s] file [%s]  %s",__func__,__FILE__,nullError);
    return -1;
  }     

  size = 2;
  dataP = lwm2m_data_new(size);
  if (dataP == NULL) return -1;

  dataP[0].id = LWM2M_SERVER_PREFERRED_TRANSPORT_ID;
  dataP[1].id = LWM2M_SERVER_APN_LINK_ID;

  if (objectP->readFunc(instanceID, &size, &dataP, objectP, INVALID_RES_INST_ID, targetP) != LWM2M_205_CONTENT) 
  {
    lwm2m_data_free(size, dataP);
    return -1;
  }

  /* Preferred transport (optional) */
  targetP->pref_transport = utils_stringToBinding(dataP[0].value.asBuffer.buffer, dataP[0].value.asBuffer.length);

  /* APN link (optional) */
  if (0 == lwm2m_data_decode_objLink(&dataP[1], &obj_link))
  {
    lwm2m_data_free(size, dataP);
    return -1;
  }
  targetP->apn_link = (obj_link) ? obj_link : 0xFFFFFFFF; 

  lwm2m_data_free(size, dataP);
  return 0;
}


/* Utility to fetch the registration retries parameters for Version 1.0 servers */
void get_1_0_retry_info(lwm2m_server_t * targetP)
{
  carrier_apn_cfg_t *apn_cfg = NULL;
  carrier_apn_cfg_t *server_apn_cfg = NULL;

  if (targetP == NULL )
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return;
  }

  apn_cfg = get_lwm2m_apn_cfg();

  server_apn_cfg = (carrier_apn_cfg_t *) LWM2M_LIST_FIND(apn_cfg, targetP->shortID);

  // Bs on reg failure needs to be fetched from carrier_apn_cfg values.
  if(apn_cfg != NULL && server_apn_cfg != NULL)
  {
    targetP->bs_if_reg_fails = server_apn_cfg->bs_if_reg_fails;
  }
  else
  {
    targetP->bs_if_reg_fails = DEFAULT_BS_ON_REG_FAILURE_1_0;
  }

  /* Initial delay timer for VZW case needs to be fetched from /1/x/30000/1 resource
     if configured else default 1.0 resource value needs to be used.*/

  targetP->intial_delay_time = DEFAULT_INITIAL_DELAY_TIMER;
  if(g_carrier_cfg)
  {
    lwm2m_object_t * serv_obj = NULL;
    if(data.lwm2mH)
    {
      serv_obj =  (lwm2m_object_t *)LWM2M_LIST_FIND(data.lwm2mH->objectList , LWM2M_SERVER_OBJECT_ID);
      if (serv_obj == NULL ) 
      {
        LOG_DEBUG("No server object found") ;
      }
      targetP->intial_delay_time = get_serverobj_hold_off_time(serv_obj, targetP->shortID);
    }
  }

  // Version 1.0 server registration retry values.
  targetP->pri_order = UNDEFINED_PRIORITY;
  targetP->reg_failure = DEFAULT_REG_FAILURE;
  targetP->retry_count =(((log(gRegRetryMaxTimeout) - log(gRegRetryInitialTimeout)) / log(gRegRetrytimeoutExponent)) +(double)(0.5)) + 1;
  targetP->retry_timer = gRegRetryInitialTimeout;
  targetP->seq_delay_timer = data.cfg_info.wait_time_for_rereg;
  targetP->seq_retry_count = DEFAULT_REG_SEQ_DELAY_COUNT_1_0; //TODO : can increase this value
}

/* Utility to fetch the registration retries parameters for Version 1.1 servers */
void get_1_1_retry_info(lwm2m_object_t * objectP,
                             uint16_t instanceID,
                             lwm2m_server_t * targetP)
{
  server_instance_t *instanceP = NULL;
  
  if (targetP == NULL )
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return;
  }

  instanceP = (server_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, instanceID);

  if(instanceP == NULL)
  {
    ds_iot_ulog_err1("Unable to find the server instance for SSID %d", instanceID);
    return;
  }

  /* For registration retry resources, if the resource value is configured then
     use that value otherwise default 1.1 resource values need to be used*/
  if(instanceP->res_set & REG_PRIORITY_SET)
  {
    targetP->pri_order = instanceP->priority;
  }
  else
  {
    targetP->pri_order = UNDEFINED_PRIORITY;
  }

  // For VZW case check either /1/x/14 or /1/x/30000/1 is configured or not.
  if(instanceP->res_set & INITIAL_DELAY_TIMER_SET)
  {
    targetP->intial_delay_time = instanceP->initial_delay_timer;
  }
  else if(g_carrier_cfg)
  {
    lwm2m_object_t * serv_obj = NULL;
    if(data.lwm2mH)
    {
      serv_obj =  (lwm2m_object_t *)LWM2M_LIST_FIND(data.lwm2mH->objectList , LWM2M_SERVER_OBJECT_ID);
      if (serv_obj == NULL ) 
      {
        LOG_DEBUG("No server object found") ;
      }
      targetP->intial_delay_time = get_serverobj_hold_off_time(serv_obj, targetP->shortID);
    }
    else
    {
      targetP->intial_delay_time = DEFAULT_INITIAL_DELAY_TIMER;
    }
  }
  else
  {
    targetP->intial_delay_time = DEFAULT_INITIAL_DELAY_TIMER;
  }

  if(instanceP->res_set & REG_FAILURE_SET)
  {
    targetP->reg_failure = instanceP->reg_failure;
  }
  else
  {
    targetP->reg_failure = DEFAULT_REG_FAILURE;
  }

  if(instanceP->res_set & BS_ON_REG_FAILURE_SET)
  {
    targetP->bs_if_reg_fails = instanceP->bs_on_failure;
  }
  else
  {
    targetP->bs_if_reg_fails = DEFAULT_BS_ON_REG_FAILURE_1_1;
  }

  if(instanceP->res_set & CONN_RETRY_COUNT_SET)
  {
    targetP->retry_count = instanceP->retry_count;
  }
  else
  {
    targetP->retry_count = DEFAULT_REG_RETRY_COUNT;
  }

  if(instanceP->res_set & CONN_RETRY_TIMER_SET)
  {
    targetP->retry_timer = instanceP->retry_timer;
  }
  else
  {
    targetP->retry_timer = DEFAULT_REG_RETRY_TIMER;
  }

  if(instanceP->res_set & CONN_SEQ_DELAY_TIMER_SET)
  {
    targetP->seq_delay_timer = instanceP->seq_delay_timer;
  }
  else
  {
    targetP->seq_delay_timer = DEFAULT_REG_SEQ_DELAY_TIMER;
  }

  if(instanceP->res_set & CONN_SEQ_DELAY_COUNT_SET)
  {
    targetP->seq_retry_count = instanceP->seq_retry_count;
  }
  else
  {
    targetP->seq_retry_count = DEFAULT_REG_SEQ_DELAY_COUNT_1_1;
  }

  return;
}

/* Utility to fetch the registration retries parameters for LwM2M servers*/
int prv_getRetryInfo(lwm2m_object_t * objectP,
                             uint16_t instanceID,
                             lwm2m_server_t * targetP)
{
  if (targetP == NULL )
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return -1;
  }

  if(targetP->version == (float)LWM2M_V_1_0)
  {
    get_1_0_retry_info(targetP);
  }
  else
  {
    get_1_1_retry_info(objectP, instanceID, targetP);
  }

  return 0;
}

/**
 * @fn int object_getServers()
 * @brief This function is used to get servers
 *
 * @param contextP  pointer to lwm2m_context_t
 *
 * @return    0 on success
 *              -1 on failure
 *        
 */
int object_getServers(lwm2m_context_t * contextP)
{
  lwm2m_object_t         *securityObjP = NULL;
  lwm2m_object_t           *serverObjP = NULL;
  security_instance_t   *securityInstP = NULL;
  
  bool                   isServerError = FALSE;
  client_data_t              *userData = NULL;
  float                        version = 0.0;
  lwm2m_list_t            *serverInstP = NULL;

  if (contextP == NULL)
  {
    LOG_DEBUG("%s contextP", nullError);
    return -1;
  }

  userData = (client_data_t *)contextP->userData;
  if(userData !=NULL)
  {
    securityObjP = userData->securityObjP;
    serverObjP   = userData->serverObject;
  }

  if ((NULL == serverObjP) || (NULL == securityObjP))
  {
    ds_iot_ulog_err2("Cannot find either server [0x%x] or security [0x%x] object information", serverObjP, securityObjP);
    return -1;
  }  

  securityInstP = (security_instance_t *)securityObjP->instanceList;
  while (securityInstP != NULL)
  {
    lwm2m_server_t *targetP = NULL;

    /* Bootstrap and registration server list has all information associated with the configured security instances.
     * If not, create the server information and add to the appropriate list. 
     */
    {
      lwm2m_server_t *dmSerLst = contextP->serverList;

      /* Bootstrap server doesn't always have a short ID - use the security instance ID for mapping */
      dmSerLst = contextP->serverList;
      while (dmSerLst)
      {
        if (dmSerLst->secObjInstID == securityInstP->instanceId)
        {
          //DM server is changed to BS server
          if(securityInstP->isBootstrap)
          {
            lwm2m_server_t * nextP = NULL;

            nextP = dmSerLst->next;
            dmSerLst->next = NULL;

            ds_iot_ulog_high("Deleting the BS server");
            prv_deleteServer(dmSerLst, contextP, TRUE);
            dmSerLst = nextP;
          }
          goto next_instance;
        }
        dmSerLst = dmSerLst->next;
      }
    }

    if(securityInstP->isBootstrap)
      goto next_instance;

    targetP = (lwm2m_server_t *)lwm2m_malloc(sizeof(lwm2m_server_t));
    if (targetP == NULL)
    {
      return -1;
    }
     
    memset(targetP, 0, sizeof(lwm2m_server_t));

    /* Security instance ID mapped to the server */ 
    targetP->secObjInstID = securityInstP->instanceId;

    if(securityInstP->shortID < 1 || 0xFFFF < securityInstP->shortID)
    {
      lwm2m_free(targetP);
      return -1;
    }
    targetP->shortID = securityInstP->shortID;

    /* Short server ID */ 
    serverInstP = prv_findServerInstance(serverObjP, targetP->shortID);
    if (serverInstP == NULL)
    {
      lwm2m_free(targetP);
      isServerError = TRUE;
      goto next_instance;
    }

    /* Mandatory information */ 
    if (0 != prv_getMandatoryInfo(serverObjP, serverInstP->id, targetP))
    {
      lwm2m_free(targetP);
      isServerError = TRUE;
      goto next_instance;
    }

    /* Default min-max period */ 
    if (0 != prv_getDefMinMaxPeriod(serverObjP, serverInstP->id, targetP))
    {
      lwm2m_free(targetP);
      isServerError = TRUE;
      goto next_instance;
    }

    /* Default LwM2M version from the LwM2M config, gets overwritten if there is a persisted value for the server */
    targetP->version = data.cfg_info.lwm2m_version;
    /* Retrieve the LwM2M version from the persistence file. In case, there is no entry associated with the server in the
     * persistence file then use default LwM2M version in LwM2M config.
     */
    get_persisted_lwm2m_server_version(targetP->shortID, &version);
    if (version)
      targetP->version = version;

    targetP->isFallbackV1_0 = FALSE;

    /* Add the registration server to the server list */
    contextP->serverList = (lwm2m_server_t*)LWM2M_LIST_ADD(contextP->serverList, targetP);
    /* Populate the registration retry resource values from server instance or config files*/
    prv_getRetryInfo(serverObjP, serverInstP->id, targetP);
    /* Populate APN link and preferred transport resources */
    prv_getServerInfo(serverObjP, serverInstP->id, targetP);

    /* Set the default server state to de-registered */
    targetP->status = STATE_DEREGISTERED;

next_instance:
    securityInstP = securityInstP->next;
  }

  if(isServerError) return -1;

  return 0;
}

/**
 * @fn int object_getBSServer()
 * @brief This function is used to get BS server
 *
 * @param contextP  pointer to lwm2m_context_t
 *
 * @return    0 on success
 *              -1 on failure
 *        
 */
  int object_getBSServer(lwm2m_context_t * contextP)

{
  lwm2m_object_t         *securityObjP = NULL;
  security_instance_t   *securityInstP = NULL;
  client_data_t              *userData = NULL;
  lwm2m_server_t             *bsSerLst = NULL;

  if (contextP == NULL)
  {
    LOG_DEBUG("Invalid input parameters - contextP[0x%x]", contextP);
    return -1;
  }

  bsSerLst =  contextP->bootstrapServer;

  if(bsSerLst != NULL)
  {
    ds_iot_ulog_err1 ("1 BS server already exists: /0/%d", bsSerLst->secObjInstID);
    return 0;
  }

  userData = (client_data_t *)contextP->userData;
  if(userData !=NULL)
  {
    securityObjP = userData->securityObjP;
  }


  if (NULL == securityObjP)
  {
    ds_iot_ulog_err1 ("Cannot find security [0x%x] object information", securityObjP);
    return -1;
  }  

  securityInstP = (security_instance_t *)securityObjP->instanceList;
  
  while (securityInstP != NULL && NULL == contextP->bootstrapServer)
  {
    lwm2m_server_t *targetP = NULL;
    if(!securityInstP->isBootstrap)
      goto next_instance;

    targetP = (lwm2m_server_t *)lwm2m_malloc(sizeof(lwm2m_server_t));
    if (targetP == NULL)
    {
      return -1;
    }
     
    memset(targetP, 0, sizeof(lwm2m_server_t));

    targetP->secObjInstID = securityInstP->instanceId;
    if(securityInstP->shortID > 0xFFFF)
    {
      lwm2m_free(targetP);
      return -1;
    }
    targetP->shortID = securityInstP->shortID;

    /* Populate bootstrap server specific information */

    targetP->lifetime = securityInstP->clientHoldOffTime;
    ds_iot_ulog_high2("Lifetime of BS account /0/%d is %d", securityInstP->instanceId, targetP->lifetime);

    /* Version */
    targetP->version = data.cfg_info.lwm2m_version;
    if(enable_bs_server_replace)
      targetP->isBSServerAccountUpdated = FALSE;
    /* Add the bootstrap server to the bootstrap server list */
    contextP->bootstrapServer = targetP;
    #ifdef PSM_LWM2M
      lwm2m_psm_operation(contextP, targetP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UNCHANGED, PSM_ADD_NODE);
    #endif

    /* Set the default server state to de-registered */
    targetP->status = STATE_DEREGISTERED;

#if defined (LWM2M_BOOTSTRAP) 
    if (persis_bootstrap_feat_enabled)
    {
      /* LwM2M client bootstrapped state information with carrier config enabled */ 
      if (g_carrier_cfg)
      {
        int64_t isBootstrapped = 0x00;
        lwm2m_data_t* subTlvArray = NULL;
        uint8 index = 0;
        int size = 1;
        lwm2m_data_t *dataP = NULL;

            dataP = lwm2m_data_new(size);
            if (dataP == NULL) return -1;

            dataP[0].id = LWM2M_SECURITY_RESOURCE_30000;

            if (securityObjP->readFunc(securityInstP->instanceId, &size, &dataP, securityObjP, INVALID_RES_INST_ID, NULL) != LWM2M_205_CONTENT)
            {
              lwm2m_data_free(size, dataP);
              return -1;
            }

        subTlvArray = dataP[0].value.asChildren.array;
  
        if ((dataP[0].value.asChildren.count != 0x00) || (subTlvArray != NULL))
        {
          for (index = 0; index < dataP[0].value.asChildren.count;index++)
          {
            if (subTlvArray[index].id == LWM2M_SECURITY_RESOURCE_INST_IS_BOOTSTRAPPED)
              break;
          }

          if (0x01 != lwm2m_data_decode_int(&subTlvArray[index], &isBootstrapped))
            ds_iot_ulog_high ("Decoding \"isBootstrapped\" resource failed - defaulting to FALSE") ;
        }
        targetP->isBootstrapped = isBootstrapped;
        lwm2m_data_free(size, dataP);
      }
      else
      {
        /* Retrieve the bootstrapped information state from the custom3000 resource (carrier config disabled) */ 
        targetP->isBootstrapped = get_serverobj_isbootstrapped(securityObjP, securityInstP->instanceId);
      }
    }
#endif

next_instance:
    securityInstP = securityInstP->next;
  }

  return 0;
}

uint8_t object_createInstance(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    lwm2m_data_t * dataP)
{
  lwm2m_object_t * targetP = NULL;
  uint16_t inst_ID = 0xFFFF;

  if( contextP == NULL || uriP == NULL || dataP == NULL )
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError); 
    return LWM2M_400_BAD_REQUEST;
  }
  ds_iot_ulog_high("Create Instance on ");
  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
  if (NULL == targetP) return (uint8_t) LWM2M_404_NOT_FOUND;

  if (NULL == targetP->createFunc)
  {
    return (uint8_t) LWM2M_405_METHOD_NOT_ALLOWED;
  }
  inst_ID = lwm2m_list_newId(targetP->instanceList);
  if (inst_ID >= LWM2M_MAX_ID)
  {
     return (uint8_t) LWM2M_400_BAD_REQUEST;
  }

  return (uint8_t) targetP->createFunc(inst_ID, dataP->value.asChildren.count, dataP->value.asChildren.array, targetP, FALSE);
}

size_t get_bs_discover_size(lwm2m_context_t * contextP,
                                   uint16_t  objectId)
{
   size_t length = 0;
   int result = 0;
   lwm2m_object_t * objectP = NULL;
   lwm2m_handle_info_t *curr_handle = NULL; 
   size_t objectTemplateLen = 0;

   if ( contextP == NULL ) 
   {
      LOG_DEBUG(" fun [%s] %s",__func__, nullError);
      return 0;
   }
   /* Bootstrap enabler version */
   length = LWM2M_ENABLER_VERSION_SIZE;
   for (objectP = contextP->objectList; objectP != NULL; objectP = objectP->next)
   {
      if((objectId != INVALID_OBJECT_ID) && (objectP->objID != objectId))  continue;

      objectTemplateLen = LINK_ITEM_START_SIZE + LINK_URI_SEPARATOR_SIZE + utils_numDigit(objectP->objID) + LINK_URI_SEPARATOR_SIZE;   // for </objID/
      if (objectP->instanceList == NULL)
      {
         length += objectTemplateLen - LINK_URI_SEPARATOR_SIZE + BS_DISC_PATH_END_SIZE + BS_DISC_COMMA_SIZE;
      }
      else
      {
         lwm2m_list_t * targetP;
         security_instance_t *secObj;
         server_instance_t *serverObj;
         for (targetP = objectP->instanceList ; targetP != NULL ; targetP = targetP->next)
         {
            length += objectTemplateLen + utils_numDigit(targetP->id) + BS_DISC_PATH_END_SIZE;
            if (objectP->objID == LWM2M_SECURITY_OBJECT_ID)
            {
              secObj = (security_instance_t *)targetP;
              /*
               * LwM2Mv1.0 - </0/1>;ssid=101,"
               * LwM2Mv1.1 - </0/1>;ssid=101;uri="coaps://server_1.example.com:5684",              
               */ 
              if (secObj->isBootstrap == false)
              {
                 char *uri_info = NULL;
  
                 length += BS_DISC_SSID_SIZE + utils_numDigit(secObj->shortID);
  
                 /* Bootstrap URI supported for servers with enabler greater than LWM2Mv1.0 */
                 if (data.cfg_info.lwm2m_version > (float)LWM2M_V_1_0)
                 {
                    length += BS_DISC_URI_START_SIZE;
                    /* Retrieve the server URI information associated with the security instance */
                    uri_info = get_server_uri(objectP, secObj->instanceId);
                    if (!uri_info)   return 0; 
                    result = strlen (uri_info);
                    free(uri_info);  
                    if (result < 0) return 0;
                    length += result + BS_DISC_URI_END_SIZE;
                 }
               }
           }
           if (objectP->objID == LWM2M_SERVER_OBJECT_ID)
           {
              serverObj = (server_instance_t *)targetP;
              length += BS_DISC_SSID_SIZE + utils_numDigit(serverObj->shortServerId);
           }
           length += BS_DISC_COMMA_SIZE;
        }
      }
   }
   qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
   curr_handle = data.glwm2m_app_ctx.handles;  
  
   /* Extensible Objects */ 
   while (curr_handle)
   {
      lwm2m_app_obj_info_t *curr_obj_list = NULL; 
      for (curr_obj_list = curr_handle->obj_info; curr_obj_list != NULL; curr_obj_list = curr_obj_list->next)
      {
         if((objectId != INVALID_OBJECT_ID) && (curr_obj_list->object_ID != objectId)) continue;

          objectTemplateLen = LINK_ITEM_START_SIZE + LINK_URI_SEPARATOR_SIZE + utils_numDigit(curr_obj_list->object_ID) + LINK_URI_SEPARATOR_SIZE ; 
          if (curr_obj_list->instance_list == NULL)
          {
             length += objectTemplateLen - LINK_URI_SEPARATOR_SIZE + BS_DISC_PATH_END_SIZE + BS_DISC_COMMA_SIZE;
          }
          else
          {
             lwm2m_list_t *curr_inst_list = NULL;
             for (curr_inst_list = curr_obj_list->instance_list; curr_inst_list != NULL; curr_inst_list = curr_inst_list->next)
             {
                length += objectTemplateLen + utils_numDigit(curr_inst_list->id) + BS_DISC_PATH_END_SIZE + BS_DISC_COMMA_SIZE;
             }
          }
       }
       curr_handle = curr_handle->next;
   }
   qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

   if (length > 0)
   {
     length = length - 1;  // remove trailing ','
   }
   return length;
}


uint8_t object_bs_discover(lwm2m_context_t * contextP,
                                   uint16_t  objectId,
                                   uint8_t** payload,
                                   size_t*   payloadLen)
{
  uint8_t  *bufferP = NULL;
  size_t index = 0;
  int result = 0;
  lwm2m_object_t * objectP = NULL;
  bool objFound = false;  
  size_t start = 0;
  size_t length = 0;
  lwm2m_handle_info_t *curr_handle = NULL; 
  size_t buffer_length = 0;
  
  LOG_DEBUG("Entering");
  if ( contextP == NULL ) 
  {
    LOG_DEBUG(" fun [%s] %s",__func__, nullError);
    return 0;
  }


  buffer_length = get_bs_discover_size(contextP, objectId);
  if(buffer_length <= 0)
    return LWM2M_500_INTERNAL_SERVER_ERROR;

  /* Adding padding of size two because 
  we are using 2 extra unit to prepare payload  */
  buffer_length += BUFFER_PADDING_LENGTH;
  
  bufferP = (uint8_t *) malloc(buffer_length);

  if(bufferP == NULL)
  {
     ds_iot_ulog_err("no memory for bootstrap payload");
     return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  memset(bufferP, 0x00, buffer_length);

  /* Bootstrap enabler version */
  if ((float)LWM2M_V_1_0 == data.cfg_info.lwm2m_version)
    result = utils_stringCopy((char *)bufferP, buffer_length, LWM2M_ENABLER_VERSION_1_0);
  else if ((float)LWM2M_V_1_1 == data.cfg_info.lwm2m_version)
    result = utils_stringCopy((char *)bufferP, buffer_length, LWM2M_ENABLER_VERSION_1_1);
  
  if (result < 0) 
  {
     goto err2;
  }
  index += result;

  for (objectP = contextP->objectList; objectP != NULL; objectP = objectP->next)
  {
    start = index;

    if((objectId != INVALID_OBJECT_ID) && (objectP->objID != objectId))
      continue;

    objFound = true;

    result = prv_getObjectTemplate(bufferP+ index, buffer_length - index, objectP->objID);
    if (result < 0) 
    {
       goto err2;
    }
    length = result;
    index += length;

    if (objectP->instanceList == NULL)
    {
      index--;

      result = utils_stringCopy((char *)bufferP + index, buffer_length - index, BS_DISC_PATH_END);
      if (result < 0) 
      {
        goto err2;
      }
        index += result;

      result = utils_stringCopy((char *)bufferP + index, buffer_length - index, BS_DISC_COMMA);
      if (result < 0) 
      {
        goto err2;
      }
        index += result;
    }
    else
    {
      lwm2m_list_t *targetP;
      security_instance_t *secObj;
      server_instance_t *serverObj;

      for (targetP = objectP->instanceList; targetP != NULL ; targetP = targetP->next)
      {
        if (index != start + length)
        {
          if (buffer_length - index <= length) 
          {
             goto err2;
          }
          LWM2M_MEMCPY(bufferP + index, buffer_length - index, bufferP + start, length);
          index += length;
        }

        result = utils_intCopy((char *)bufferP + index, buffer_length - index, targetP->id);
        if (result < 0) 
        {
           goto err2;
        }
        index += result;

        result = utils_stringCopy((char *)bufferP+ index, buffer_length - index, BS_DISC_PATH_END);
        if (result < 0) 
        {
           goto err2;
        }
          index += result;

        if (objectP->objID == LWM2M_SECURITY_OBJECT_ID)
        {
          secObj = (security_instance_t *)targetP;

          if (secObj->isBootstrap == false)
          {
            char *uri_info = NULL;

            /*
             * LwM2Mv1.0 - </0/1>;ssid=101,"
             * LwM2Mv1.1 - </0/1>;ssid=101;uri="coaps://server_1.example.com:5684",              
             */ 
            result = utils_stringCopy((char *)bufferP + index, buffer_length - index, BS_DISC_SSID);
            if (result < 0) 
            {
               goto err2;
            }
            index += result;

            /* Bootstrap URI supported for servers with enabler greater than LWM2Mv1.0 */
            if (data.cfg_info.lwm2m_version > (float)LWM2M_V_1_0)
            {
              result = utils_intCopy((char *)bufferP + index, buffer_length - index, secObj->shortID);
              if (result < 0) 
              {
                goto err2;
              }
              index += result;
  
              result = utils_stringCopy((char *)bufferP + index, buffer_length - index, BS_DISC_URI_START);
              if (result < 0) 
              {
                  goto err2;
              }
              index += result;
  
              /* Retrieve the server URI information associated with the security instance */
              uri_info = get_server_uri(objectP, secObj->instanceId);
              if (!uri_info)
              {
                  goto err2;
              }
  
              result = utils_stringCopy((char *)bufferP + index, buffer_length - index, uri_info);            
              free(uri_info);  
              if (result < 0) {
                  goto err2;
              }
              index += result;
  
              result = utils_stringCopy((char *)bufferP + index, buffer_length - index, BS_DISC_URI_END);
              if (result < 0) 
              {
                  goto err2;
              }
              index += result;
            }
            else 
            {
              /* LwM2Mv1.0 */
              result = utils_intCopy((char *)bufferP + index, buffer_length - index, secObj->shortID);
              if (result < 0) 
              {
                  goto err2;
              }
              index += result;
            }         
          }
        }

        if (objectP->objID == LWM2M_SERVER_OBJECT_ID)
        {
          result = utils_stringCopy((char *)bufferP + index, buffer_length - index, BS_DISC_SSID);
          if (result < 0) 
          {
             goto err2;
          }
          index += result;
  
          serverObj = (server_instance_t *)targetP;
          result = utils_intCopy((char *)bufferP + index, buffer_length - index, serverObj->shortServerId);
          if (result < 0) 
          {
            goto err2;
          }
          index += result;
        }

        result = utils_stringCopy((char *)bufferP + index, buffer_length - index, BS_DISC_COMMA);
        if (result < 0) 
        {
           goto err2;
        }
          index += result;
      }
    }
  }

  qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

  /* Populate the extensible object information in the registration update */  
  curr_handle = data.glwm2m_app_ctx.handles;
  while (curr_handle)
  {    
    lwm2m_app_obj_info_t *curr_obj_list = NULL; 

    /* Retrieve the object information for the handle and traverse through the object list */
    for (curr_obj_list = curr_handle->obj_info; curr_obj_list != NULL; curr_obj_list = curr_obj_list->next)
    {
      start = index;
      if((objectId != INVALID_OBJECT_ID) && (curr_obj_list->object_ID != objectId))
        continue;
  
      objFound = true;

      result = prv_getObjectTemplate(bufferP+ index, buffer_length - index, curr_obj_list->object_ID);
      if (result < 0) 
      {
         goto err1;
      }
      length = result;
      index += length;
  
      if (curr_obj_list->instance_list == NULL)
      {
        index--;
        result = utils_stringCopy((char *)bufferP + index, buffer_length - index, BS_DISC_PATH_END);
        if (result < 0) 
        {
           goto err1;
        }
          index += result;
        result = utils_stringCopy((char *)bufferP + index, buffer_length - index, BS_DISC_COMMA);
        if (result < 0) 
        {
          goto err1;
        }
          index += result;
      }
      else
      {
        lwm2m_list_t *curr_inst_list = NULL;  
  
        /* Retrieve the object instance list information from the object list and populate the update information */ 
        for (curr_inst_list = curr_obj_list->instance_list; curr_inst_list != NULL; curr_inst_list = curr_inst_list->next)
        {
          if (index != start + length)
          {
            if (buffer_length - index <= length) 
            {
              goto err1;
            }
            LWM2M_MEMCPY(bufferP + index, buffer_length - index, bufferP + start, length);
            index += length;
          }
  
          result = utils_intCopy((char *)bufferP + index, buffer_length - index, curr_inst_list->id);
          if (result < 0) 
          {
            goto err1;
          }
          index += result;
  
          result = utils_stringCopy((char *)bufferP+ index, buffer_length - index, BS_DISC_PATH_END);
          if (result < 0) 
          {
            goto err1;
          }
            index += result;

          result = utils_stringCopy((char *)bufferP + index, buffer_length - index, BS_DISC_COMMA);
          if (result < 0) 
          {
            goto err1;
          }
            index += result;
        }
      }
    }
    
    curr_handle = curr_handle->next;
  }

  qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

  if (index > 0)
  {
    index = index - 1;  // remove trailing ','
  }

  bufferP[index] = 0;

  if (true == objFound)
  {
    *payloadLen = index;
    *payload = (uint8_t *)lwm2m_malloc((*payloadLen)*sizeof(uint8_t));
    if (NULL == *payload)  
     {
        if (bufferP) free(bufferP);
      return LWM2M_500_INTERNAL_SERVER_ERROR;
     }
     memset(*payload, 0x00, index);
    utils_stringCopy((char *)*payload, *payloadLen, (char *)bufferP);
     if (bufferP) free(bufferP);
    return LWM2M_205_CONTENT;
  }
  else
  {
    if (bufferP) free(bufferP);
    return LWM2M_404_NOT_FOUND;
  }

err1:
   qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
err2:
   if (bufferP)
     free(bufferP);
   return 0;
  
}

/**
 * @fn void * lwm2m_list_pri_add()
 * @brief This function is used to add the node to list sorted by priority order
 *
 * @param head       head pointer
 * @param node       node to be added
 *
 * @return     head pointer
 */
void * lwm2m_list_pri_add(void * head,
    void * node)
{
  lwm2m_list_t * target = NULL;
  lwm2m_list_t * ahead = NULL;
  lwm2m_list_t * anode = NULL;

  if (NULL == head) return node;

  ahead = (lwm2m_list_t *)head;
  anode = (lwm2m_list_t *)node;
  if (((lwm2m_server_t *)ahead)->pri_order > ((lwm2m_server_t *)anode)->pri_order)
  {
    anode->next = ahead;
    return anode;
  }

  target = ahead;
  while (NULL != target->next && ((lwm2m_server_t *)target->next)->pri_order <= ((lwm2m_server_t *)anode)->pri_order)
  {
    target = target->next;
  }

  anode->next = target->next;
  target->next = anode;

  return ahead;

}

/**
 * @fn void lwm2m_sort_servers_by_pri()
 * @brief This function is used to sort the either the server list or active 
 *        server list based on LwM2M server priority order.
 * @param server_lst server list to be sorted
 *
 * @return     None
 */
void lwm2m_sort_servers_by_pri(void **server_lst)
{
  lwm2m_list_t * targetP = NULL;
  lwm2m_list_t * nextP = NULL;

  if(*server_lst == NULL)
  {
    ds_iot_ulog_high("Server list passed is empty. Returning.");
    return;
  }

  targetP = (lwm2m_list_t * )*server_lst;
  *server_lst = NULL;

  while (targetP != NULL)
  {
    nextP = targetP->next;
    targetP->next = NULL;

    *server_lst = lwm2m_list_pri_add((lwm2m_list_t *)*server_lst, (lwm2m_list_t *)targetP);

    targetP = nextP;
  }

  return;
}

void lwm2m_create_active_server_node(lwm2m_context_t *contextP,
                                                   lwm2m_server_t *targetP,
                                                   bool ordered)
{
  lwm2m_active_server_t *serverP = NULL;
  serverP = (lwm2m_active_server_t *)lwm2m_malloc(sizeof(lwm2m_active_server_t));
  if(serverP == NULL)
  {
    LOG_DEBUG("%s",mallocError);
    return;
  }
  memset(serverP, 0x00, sizeof(lwm2m_active_server_t));
  serverP->ssid = targetP->shortID;
  targetP->initial_holdoff_time = lwm2m_gettime() + targetP->intial_delay_time;
  serverP->initial_holdoff_time = targetP->initial_holdoff_time;
  contextP->activeServerList = (lwm2m_active_server_t *)LWM2M_LIST_ADD(contextP->activeServerList, serverP);
  if(ordered)
  {
    contextP->lastActiveServer = serverP;
  }
  serverP->pri_order = targetP->pri_order;
  ds_iot_ulog_high1("Server made active for short id [%d]", serverP->ssid);
  #ifdef PSM_LWM2M
    if(lwm2m_psm_enable == TRUE)
    {
      lwm2m_uri_t uri;
      /* Add PSM node associated with DM server .*/
      memset(&uri, 0x00, sizeof(lwm2m_uri_t));
      lwm2m_psm_add_node(contextP, targetP->shortID, uri, PSM_NODE_SERVER);
    }
  #endif
  if(persis_regstatus_feat_enabled == LWM2M_TRUE)
  {
    sync_activeserverlist_to_persistence(contextP);
  }
}

/**
 * @fn void lwm2m_create_active_server_list()
 * @brief This function is used to create the active server list. Active servers
 *        are those which are ready for registration procedure.
 * @param contextP LwM2M context
 *
 * @return     None
 */
void lwm2m_create_active_server_list(lwm2m_context_t *contextP)
{
  lwm2m_server_t *targetP = NULL;
  lwm2m_server_t *nextP = NULL;

  if (contextP == NULL)
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return;
  }

  if(contextP->serverList == NULL)
  {
    ds_iot_ulog_high("ServerList is NULL");
#ifdef LWM2M_BOOTSTRAP        
          isBootstrappedflag = LWM2M_FALSE;
#endif 
    contextP->state = STATE_BOOTSTRAP_REQUIRED;
  
   /* Stop the data calls with the APNs associated with the bootstrap and registration servers */ 
    lwm2m_datacall_stop(contextP, LWM2M_STOP_BOOTSTRAP_DATA_CALL, NULL, false);  
    lwm2m_datacall_stop(contextP, LWM2M_STOP_REGISTRATION_DATA_CALL, NULL, false);
  
    /* Start the data calls with the APNs associated with the bootstrap servers */ 
    if (-1 == lwm2m_datacall_start(contextP, LWM2M_START_BOOTSTRAP_DATA_CALL, NULL, true))
      return;
    
    m2m_log_printf("State: %s", STR_STATE(contextP->state));
  }

  // Sort the contextP->serverList based on priority order.
  lwm2m_sort_servers_by_pri((void **)&contextP->serverList);
  targetP = contextP->serverList;

  /* Loop over the DM servers configured. Server is made active if it meets any 
     of the below criteria
     1) If the server is unordered then mark server as Active server.
     2) If the server has highest priority then mark server as Active server.
     3) If the last ordered server added was blocking then wait for last server 
        to move to Registerd state then mark server as Active server.
     4) If the last ordered server added was non-blocking then either wait for last 
        server to move to Registerd state or wait for one communication retry sequence 
        and then mark server as Active server.
     5) If the last ordered server added is trying with lower version 
        then mark server as Active server.
  */

  /* DM servers are added to PSM node list when they are moved to active server 
     list. Also, initial holdoff time is calculated here.*/
  while(targetP)
  {
    lwm2m_active_server_t *serverP = NULL;

    if(targetP->pri_order == UNDEFINED_PRIORITY)
    {
      serverP = (lwm2m_active_server_t *)LWM2M_LIST_FIND(contextP->activeServerList,targetP->shortID);
      if(serverP == NULL)
      {
        lwm2m_create_active_server_node(contextP, targetP, FALSE);
      }
    }
    else
    {
      lwm2m_server_t *lastactive = NULL;

      if(contextP->lastActiveServer != NULL)
      {
        lastactive = lwm2m_get_server(contextP->lastActiveServer->ssid);
      }
      if(contextP->activeServerList == NULL || contextP->lastActiveServer == NULL ||
         (lastactive &&
         ((lastactive->reg_failure == TRUE && 
          lastactive->status == STATE_REGISTERED) || 
          (lastactive->reg_failure == FALSE && 
          (lastactive->status == STATE_REGISTERED ||
          (lastactive->status == STATE_REG_FAILED && 
           lastactive->seq_retry_attempt == 1) || 
          (lastactive->status == STATE_REG_FAILED && 
           lastactive->seq_retry_attempt == 0 && g_carrier_cfg 
           && lastactive->last_code == LWM2M_503_SERVICE_UNAVAILABLE &&
           data.cfg_info.wait_time_for_error))) ||
           (lastactive->pri_order == UNDEFINED_PRIORITY))))
      {
        serverP = (lwm2m_active_server_t *)LWM2M_LIST_FIND(contextP->activeServerList,targetP->shortID);
        if(serverP == NULL)
        {
          lwm2m_create_active_server_node(contextP, targetP, TRUE);
        }
      }
    }
    targetP = targetP->next;
  }

  /* contextP->serverList is resorted based on short server ID */ 
  targetP = contextP->serverList;
  contextP->serverList = NULL;
  while (targetP != NULL)
  {
    nextP = targetP->next;
    targetP->next = NULL;
    contextP->serverList = (lwm2m_server_t *)LWM2M_LIST_ADD(contextP->serverList, targetP);
    targetP = nextP;
  }
}

/* Utility to move the node at the end of the active server list */
void lwm2m_server_end(lwm2m_context_t *contextP, uint16_t ssid)
{
  lwm2m_active_server_t *targetP = NULL;
  lwm2m_active_server_t *serverP = NULL;
  if (contextP == NULL || contextP->activeServerList == NULL)
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return;
  }

  contextP->activeServerList = (lwm2m_active_server_t *)LWM2M_LIST_RM(contextP->activeServerList, ssid, &targetP);

  if(targetP != NULL)
  {
    serverP = contextP->activeServerList;
    if(serverP == NULL)
    {
      contextP->activeServerList = targetP;
      targetP->next = NULL;
      return;
    }
    else
    {
      while(serverP->next != NULL)
      {
        serverP = serverP->next;
      }
  
      serverP->next = targetP;
      targetP->next = NULL;
    }
  }
  
  return;
}

/**
 * @fn void lwm2m_sort_active_server_list()
 * @brief This function is used to sort the active server list. Active servers
 *        are those which are ready for registration procedure.
 * @param contextP LwM2M context
 *
 * @return     None
 */
void lwm2m_sort_active_server_list(lwm2m_context_t *contextP)
{
  lwm2m_active_server_t *serverList = NULL;
  uint16_t *shortid_list = NULL;
  uint8_t count = 0, i = 0;
  
  if (contextP == NULL || contextP->activeServerList == NULL || contextP->serverList == NULL)
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return;
  }

  /* Sort the active server list by following criteria
     1) Based on the priority order.
     2) Based on Blocking/Non-blocking servers, non-blocking servers are moved to end.
     3) Based on Registration status, not registered servers are moved to end.
  */

  // Sort the contextP->activeServerList based on priority order.
  lwm2m_sort_servers_by_pri((void **)&contextP->activeServerList);

  serverList = contextP->activeServerList;

  count = LWM2M_LIST_COUNT(serverList);

  if(count != 0)
  {
    i = 0;
    shortid_list = lwm2m_calloc(count, sizeof(uint16_t));
    if(shortid_list == NULL)
    {
      LOG_DEBUG("%s",mallocError);
      return;
    }
    memset(shortid_list, 0x00, count * sizeof(uint16_t));

    serverList = contextP->activeServerList;
    while(serverList != NULL)
    {
      shortid_list[i] = serverList->ssid;
      i++;
      serverList = serverList->next;
    }
  }

  serverList = contextP->activeServerList;

  for(i = 0; i < count; i++)
  {
    lwm2m_server_t *serverP = NULL;
    serverP = lwm2m_get_server(shortid_list[i]);
    if(serverP && serverP->reg_failure == FALSE)
    {
      lwm2m_server_end(contextP, shortid_list[i]);
    }
  }

  serverList = contextP->activeServerList;
  for(i = 0; i < count; i++)
  {
    lwm2m_server_t *serverP = NULL;
    serverP = lwm2m_get_server(shortid_list[i]);
    if(serverP && serverP->status != STATE_REGISTERED)
    {
      lwm2m_server_end(contextP, shortid_list[i]);
    }
  }

  if(shortid_list != NULL)
  {
    lwm2m_free(shortid_list);
    shortid_list = NULL;
  }
}

/**
 * @fn bool object_checkACLConsistence()
 * @brief This function is used to do ACL sanity check.
 *
 * @param contextP      pointer to lwm2m_context_t
 * @param isClientInitBS  it's Client Initialized BS or Factory BS.
 *
 * @return    true on success
 *            false on failure
 */
 #ifdef AUTHORIZATION_SUPPORT
bool object_checkACLConsistence(lwm2m_context_t * contextP, bool isClientInitBS)
{
  acc_ctrl_oi_t           *acc_ctrl_oi = NULL;
  acc_ctrl_ri_t           *acc_ctrl_ri = NULL;
  lwm2m_object_t              *targetP = NULL;
  lwm2m_list_t               *objInstP = NULL;
  lwm2m_server_t              *serverP = NULL;
  lwm2m_app_obj_info_t  *load_obj_info = NULL;
  lwm2m_app_obj_info_t *obj_info_match = NULL;
  bool                is_check_ext_obj = FALSE;
  bool                          result = FALSE;

  if(contextP == NULL)
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return FALSE;
  }
  LOG_DEBUG("enter [%s]", __func__);

  /* no need to check ACL if there is less one server
     or there is no AC Obj instance at all.
  */
  if(lwm2m_get_server_count(contextP, FALSE) <= 1 ||
     contextP->acObjInstList == NULL)
    return TRUE;

  // may need to check ext obj if ACL targets on it when CIB.
  if(isClientInitBS)
    lwm2m_load_application_object_info(&load_obj_info);

  acc_ctrl_oi = (acc_ctrl_oi_t *)contextP->acObjInstList;
  while(NULL != acc_ctrl_oi)
  {
    is_check_ext_obj = FALSE;
    if(/*1. Object ID validity check*/
     (acc_ctrl_oi->objectId == LWM2M_SECURITY_OBJECT_ID ||
      acc_ctrl_oi->objectId == LWM2M_ACL_OBJECT_ID ||
      acc_ctrl_oi->objectId == LWM2M_MAX_ID) ||
     /*2. Obj instance ID and AC owner consistence check*/
     ((acc_ctrl_oi->objectInstId == LWM2M_MAX_ID && acc_ctrl_oi->accCtrlOwner !=LWM2M_MAX_ID) ||
      (acc_ctrl_oi->objectInstId != LWM2M_MAX_ID && acc_ctrl_oi->accCtrlOwner ==LWM2M_MAX_ID)))
    {
      ds_iot_ulog_err(" fun [object_checkACLConsistence]: invalid object ID/instance ID/acOwner");
      result = FALSE;
      goto END;
    }

    if(lwm2m_is_standard_object(acc_ctrl_oi->objectId))
    {
      targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, acc_ctrl_oi->objectId);
      if(targetP == NULL)
      {
        ds_iot_ulog_err2("fun [object_checkACLConsistence]: no target object ID %d for ACL /2/%d",
                    acc_ctrl_oi->objectId, acc_ctrl_oi->objInstId);
        result = FALSE;
        goto END;
      }
    }
    else
    {
      if(isClientInitBS && load_obj_info != NULL)
      {
        obj_info_match = (lwm2m_app_obj_info_t * ) LWM2M_LIST_FIND(load_obj_info, acc_ctrl_oi->objectId);
        if (obj_info_match == NULL)
        {
          ds_iot_ulog_err2("fun [object_checkACLConsistence]: no target object ID %d for ACL /2/%d",
                      acc_ctrl_oi->objectId, acc_ctrl_oi->objInstId);
          result = FALSE;
          goto END;
        }
        is_check_ext_obj = TRUE;
      }
      else
      {
        acc_ctrl_oi = acc_ctrl_oi->next;
        continue;
      }
    }

    /*for ACL on Object Instance Level Check*/
    if(acc_ctrl_oi->objectInstId != LWM2M_MAX_ID && acc_ctrl_oi->accCtrlOwner !=LWM2M_MAX_ID)
    {
      if(is_check_ext_obj)
        objInstP = LWM2M_LIST_FIND(obj_info_match->instance_list, acc_ctrl_oi->objectInstId);
      else
        objInstP = LWM2M_LIST_FIND(targetP->instanceList, acc_ctrl_oi->objectInstId);

      if(objInstP == NULL)
      {
        ds_iot_ulog_err3("fun [object_checkACLConsistence]: no target object instance ID /%d/%d for ACL /2/%d",
                    acc_ctrl_oi->objectId, acc_ctrl_oi->objectInstId, acc_ctrl_oi->objInstId);
        result = FALSE;
        goto END;
      }

      serverP = (lwm2m_server_t *)LWM2M_LIST_FIND(contextP->serverList, acc_ctrl_oi->accCtrlOwner);
      if(serverP == NULL)
      {
        ds_iot_ulog_err2("fun [object_checkACLConsistence]: no server with ssid %d for ACL /2/%d", 
                    acc_ctrl_oi->accCtrlOwner, acc_ctrl_oi->objInstId);
        result = FALSE;
        goto END;
      }
    }

    /*for ACL on both Object and Object instance level check*/
    acc_ctrl_ri = (acc_ctrl_ri_t *)acc_ctrl_oi->accCtrlValList;
    while(acc_ctrl_ri != NULL)
    {
      if(acc_ctrl_ri->resInstId == 0)
      {
        /*ACL instance ID 0 is invalid when AC on object level*/
        if(acc_ctrl_oi->objectInstId == LWM2M_MAX_ID && acc_ctrl_oi->accCtrlOwner ==LWM2M_MAX_ID)
        {
          ds_iot_ulog_err1("fun [object_checkACLConsistence]: ACL inst ID 0 is invalid when AC on obj level for ACL /2/%d", acc_ctrl_oi->objInstId);
          result = FALSE;
          goto END;
        }

        /*ACL instance ID 0 is valid when AC on object instance level*/
        acc_ctrl_ri = acc_ctrl_ri->next;
        continue;
      }

      serverP = (lwm2m_server_t *)LWM2M_LIST_FIND(contextP->serverList, acc_ctrl_ri->resInstId);
      if(serverP == NULL)
      {
        ds_iot_ulog_err2("fun [object_checkACLConsistence]: no server with ssid %d for ACL /2/%d",
                    acc_ctrl_ri->resInstId, acc_ctrl_oi->objInstId);
        result = FALSE;
        goto END;
      }
      else
      {
        acc_ctrl_ri = acc_ctrl_ri->next;
      }
    }
    acc_ctrl_oi = acc_ctrl_oi->next;
  }
  result = TRUE;
END:
  /* Clean-up the loaded persistence object list */ 
  while (load_obj_info)
  {
    lwm2m_app_obj_info_t *next_obj_info = load_obj_info->next;

    LWM2M_LIST_FREE(load_obj_info->instance_list);
    lwm2m_free(load_obj_info);
    load_obj_info = next_obj_info;
  }
  return result;
}
#endif

/**
 * @fn bool object_checkSecurityObjInstConsistence()
 * @brief This function is used to do security Obj Inst sanity check.
 *
 * @param contextP        pointer to lwm2m_context_t
 *
 * @return      TRUE  success
 *              FALSE failure
 */
bool object_checkSecurityObjInstConsistence(lwm2m_context_t * contextP)
{
  client_data_t          *dataP = NULL;
  security_instance_t  *secInst = NULL;
  uint8_t        DMSecInstCount = 0;
  uint8_t        BSSecInstCount = 0;

  if(contextP == NULL)
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return FALSE;
  }

  LOG_DEBUG("enter [%s]", __func__);

  dataP = (client_data_t *)contextP->userData;
  if(dataP !=NULL && dataP->securityObjP != NULL)
    secInst = (security_instance_t *)dataP->securityObjP->instanceList;
  if(secInst == NULL)
  {
    ds_iot_ulog_err("fun [object_checkSecurityObjInstConsistence]: there is no security object instance");
    return FALSE;
  }

  while(secInst != NULL)
  {
    if(secInst->isBootstrap)
    {
      if(secInst->uri == NULL)
      {
        ds_iot_ulog_err1("fun [object_checkSecurityObjInstConsistence]: there is no URI in security obj instance /0/%d",   secInst->instanceId);
        return FALSE;
      }
      else
        BSSecInstCount++;
    }
    else
    {
      if(CHECK_SHORT_SERVER_ID_RANGE(secInst->shortID))
      {
        DMSecInstCount++;
      }
      //ignore the invalid ones
    }
    secInst = secInst->next;
  }

  if(BSSecInstCount > 1 || DMSecInstCount < 1)
  {
     ds_iot_ulog_err1("fun [object_checkSecurityObjInstConsistence]: there are [%d] BS(at most one) [%d] DM(at least one) security obj instance",  BSSecInstCount);
    return FALSE;
  }

  return TRUE;
}

/**
 * @fn bool object_checkServerObjInstConsistence()
 * @brief This function is used to do server Obj Inst sanity check.
 *
 * @param contextP        pointer to lwm2m_context_t
 *
 * @return      TRUE  success
 *              FALSE failure
 */
bool object_checkServerObjInstConsistence(lwm2m_context_t * contextP)
{
  client_data_t              *dataP = NULL;
  server_instance_t       *servInst = NULL;
  uint8_t           DMServInstCount = 0;

  if(contextP == NULL)
  {
    LOG_DEBUG(" fun [%s] %s",__func__,nullError);
    return FALSE;
  }

  LOG_DEBUG("enter [%s]", __func__);

  dataP = (client_data_t *)contextP->userData;
  if(dataP !=NULL && dataP->serverObject != NULL)
    servInst = (server_instance_t *)dataP->serverObject->instanceList;
  if(servInst == NULL)
  {
    ds_iot_ulog_err("fun [object_checkServerObjInstConsistence]: no server instance, at least one.");
    return FALSE;
  }


  while(servInst != NULL)
  {
    if(CHECK_SHORT_SERVER_ID_RANGE(servInst->shortServerId))
    {
      DMServInstCount++;
    }
    servInst = servInst->next;
  }

  if(0 == DMServInstCount)
  {
    ds_iot_ulog_err("fun [object_checkServerObjInstConsistence]: no server instance, at least one.");
    return FALSE;
  }

  return TRUE;
}


/**
 * @fn bool object_checkServerListConsistence()
 * @brief This function is used to do server list sanity check.
 *
 * @param contextP        pointer to lwm2m_context_t
 *
 * @return      TRUE  success
 *              FALSE failure
 */
bool object_checkServerListConsistence(lwm2m_context_t * contextP)
{
  client_data_t          *dataP = NULL;
  lwm2m_server_t    *serverList = NULL;
  lwm2m_object_t  *securityObjP = NULL;
  char                     *uri = NULL;

  if(contextP == NULL || contextP->serverList == NULL)
  {
    LOG_DEBUG(" fun [object_checkServerListConsistence] %s", nullError);
    return FALSE;
  }

  LOG_DEBUG(" enter [object_checkServerListConsistence]");

  dataP = (client_data_t *)contextP->userData;
  if(dataP !=NULL && dataP->securityObjP != NULL)
      securityObjP = dataP->securityObjP;
  if(securityObjP == NULL)
  {
    ds_iot_ulog_err(" fun [object_checkServerListConsistence]: there is no security object instance");
    return FALSE;
  }

  serverList = contextP->serverList;

  while(serverList != NULL)
  {
    if(!CHECK_SHORT_SERVER_ID_RANGE(serverList->shortID))
    {
      ds_iot_ulog_err2(" fun [object_checkServerListConsistence]: invalid ssid %d for /0/%d",  serverList->shortID, serverList->secObjInstID);
      return FALSE;
    }

    uri = get_server_uri(securityObjP, serverList->secObjInstID);
    if (uri == NULL)
    {
      ds_iot_ulog_err1(" fun [object_checkServerListConsistence]: URI is NULL for ssid %d", serverList->shortID);
      return FALSE;
    }
    if(uri != NULL)
    {
      lwm2m_free(uri);
      uri = NULL;
    }
    serverList = serverList->next;
  }

  return TRUE;
}
/**
 * @fn bool object_isSSIDUnique()
 * @brief This function is used to check if short server ID is unique in security/server
 *        object instance list and in server list.
 *
 * @param contextP        pointer to lwm2m_context_t
 * @param isClientInitBS  it's Client Initialized BS or Factory BS.
 *
 * @return      TRUE  success
 *              FALSE failure
 */
bool object_isSSIDUnique(lwm2m_context_t * contextP)
{
  client_data_t            *dataP = NULL;
  security_instance_t  *secInst_i = NULL;
  security_instance_t  *secInst_j = NULL;
  server_instance_t    *serInst_i = NULL;
  server_instance_t    *serInst_j = NULL;
  lwm2m_server_t       *serList_i = NULL;
  lwm2m_server_t       *serList_j = NULL;

  if(contextP == NULL)
  {
    LOG_DEBUG(" fun [object_isSSIDUnique] %s",nullError);
    return FALSE;
  }

  LOG_DEBUG(" enter [object_isSSIDUnique]");

  dataP = (client_data_t *)contextP->userData;
  if(dataP != NULL)
  {
    //check SSID unique in security object instance list
    if(dataP->securityObjP != NULL)
      secInst_i = (security_instance_t *)dataP->securityObjP->instanceList;
    while(secInst_i != NULL)
    {
      if(secInst_i->isBootstrap)
      {
        secInst_i = secInst_i->next;
        continue;
      }
      for(secInst_j = secInst_i->next; secInst_j != NULL; secInst_j = secInst_j->next)
      {
        if(secInst_j->shortID == secInst_i->shortID)
        {
          ds_iot_ulog_err3(" fun [object_isSSIDUnique]: security instance /0/%d and /0/%d with the same shortID %d",
                      secInst_i->instanceId, secInst_j->instanceId, secInst_i->shortID);
          return FALSE;
        }
      }
      secInst_i = secInst_i->next;
    }

    //check SSID unique in server object instance list
    if(dataP->serverObject != NULL)
      serInst_i = (server_instance_t *)dataP->serverObject->instanceList;
    while(serInst_i != NULL)
    {
      for(serInst_j = serInst_i->next; serInst_j != NULL; serInst_j = serInst_j->next)
      {
        if(serInst_j->shortServerId == serInst_i->shortServerId)
        {
          ds_iot_ulog_err3(" fun [object_isSSIDUnique]: server instance /1/%d and /1/%d with the same shortID %d",
                      serInst_i->instanceId, serInst_j->instanceId, serInst_i->shortServerId);
          return FALSE;
        }
      }
      serInst_i = serInst_i->next;
    }
  }

  serList_i = contextP->serverList;
  while(serList_i != NULL)
  {
    //serverList is in order by shortID, just need to compare the 2 neighbour ones.
    serList_j = serList_i->next;
    if(serList_j != NULL)
    {
      if(serList_j->shortID == serList_i->shortID)
      {
        ds_iot_ulog_err3(" fun [object_isSSIDUnique]: servers /0/%d and /0/%d with the same shortID %d",
                    serList_i->secObjInstID, serList_j->secObjInstID, serList_i->shortID);
        return FALSE;
      }
    }
    serList_i = serList_j;
  }

  return TRUE;
}

uint8_t prv_mandatory_check(int numData, lwm2m_data_t * dataArray, uint16_t *mandatory_resource, uint8_t size)
{
  uint8_t i = 0, j = 0;

  if(mandatory_resource == NULL)
   return LWM2M_500_INTERNAL_SERVER_ERROR;

  /*check all mendatory resources present */
  for(i = 0; i < size; i++)
  {
    for(j = 0; j < numData; j++)
    {
      if(mandatory_resource[i] == dataArray[j].id)
      break;
    }
    if(j == numData)
    {
      ds_iot_ulog_high1("LWM2M_LOG: failed for mandatory resource id %d ", mandatory_resource[i]);
      return LWM2M_400_BAD_REQUEST;
    }
  }
  return LWM2M_NO_ERROR;
}

#endif


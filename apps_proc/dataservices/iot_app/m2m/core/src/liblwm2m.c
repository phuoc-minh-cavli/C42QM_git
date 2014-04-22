/******************************************************************************

  @file    liblwm2m.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2023 Qualcomm Technologies, Inc.
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
 *    Simon Bernard - Please refer to git log
 *    Toby Jaffey - Please refer to git log
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

#include "qapi_dss.h"
#include "internals.h"
#include "lwm2m_signals.h"
#include "lwm2m_carrier.h"
#include "lwm2mclient.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "object_access_control.h"
#include "object_server.h"
#include "object_security.h"


#include <qapi_ns_utils.h>
#include "iotapp_dss_util.h"
#include "iotapp_qmi_internal.h"
#include "log_utils.h"
#include "math.h"
#include "ds_log.h"


#define EP_NAME_IMEI_MSISDN     "urn:imei-msisdn:000000000000000-000000000000000"

#define MSISDN_LEN 10
#define MAX_MSISDN_LEN  15
#define MAX_IMSI_LEN    15

#ifdef LWM2M_BOOTSTRAP
bool isBootstrappedflag = LWM2M_FALSE; 
#endif 

extern bool lwm2m_in_sleep_state;
extern timer_group_type lwm2mTimerGroup;
qurt_signal_t lwm2m_signal_Handle;
static bool is_MSISDN_set_from_UIM = false;
extern time_t gRegRetryInitialTimeout;
extern time_t gRegRetrytimeoutExponent;
extern time_t gRegRetryMaxTimeout;
extern bool g_carrier_cfg;
extern bool lwm2m_psm_enable;  // LWM2M PSM is enabled or not.
extern bool enable_bs_server_replace;

extern const char * inet_ntop(int af, const void *addr, char *str, size_t size);
extern client_data_t data;

extern bool persis_regstatus_feat_enabled;
extern bool enable_location;
extern bool enable_dev_cap_obj;
extern bool enable_conn_m_obj;
extern bool enable_soft_mgnt_obj;
extern bool enable_firmware_obj;
extern bool enable_conn_stat_obj;
extern bool enable_cell_nw_conn_obj;
extern bool enable_apn_conn_obj;
extern bool enable_host_dev_obj;
extern bool enable_acl_obj;

const char* nullError = "LWM2M_LOG: NULL Error";
const char* mallocError = "LWM2M_LOG: Memory Allocation FAILED";

timer_type extObjCreateWindowTimer;


/* Supported LWM2M versions */
float supported_lwm2m_ver [LWM2M_MAX_SUPPORTED_VERSIONS] =
{
  /* Sorted in descending order */          
  LWM2M_V_1_1,  /* Version 1.1 */
  LWM2M_V_1_0   /* Version 1.0 */
};


void lwm2m_unlink_node(void **head, void *node)
{
  lwm2m_list_t *list_head = NULL;
  if (head == NULL || *head == NULL || node == NULL ) 
  {
    LOG_DEBUG("fun [lwm2m_unlink_node]:NULL Error");
    return;
  }

  list_head = (lwm2m_list_t *)*head;
  if (list_head == node)
  {
    list_head= list_head->next;
  }
  else
  {
    lwm2m_list_t *list = NULL;
  
    list = list_head;
    while (list->next != NULL && list->next != node)
    {
      list = list->next;
    }
  
    if(list->next != NULL)
    {
      list->next = list->next->next;
    }
  }
  *head = (void *)list_head;
}

void reset_global_flags(bool val)
{ 
  isBootstrappedflag = val;
  is_MSISDN_set_from_UIM = val;
  ds_iot_ulog_high("Resetting flags");
  return;  
}
/**
 * @fn lwm2m_context_t * lwm2m_init()
 * @brief This function is used to initialise lwm2m context
 *
 * @param userData  void pointer
 *
 * @return    lwm2m context pointer on success
 *        NULL on failure
 */
lwm2m_context_t * lwm2m_init(void * userData)
{
  lwm2m_context_t * contextP = NULL;

  LOG_DEBUG("Entering");
  if ( NULL == userData ) 
  { 
    LOG_DEBUG("fun [reset_global_flags] NULL Error");
    return NULL;
  }
  contextP = (lwm2m_context_t *)lwm2m_malloc(sizeof(lwm2m_context_t));
  if (NULL != contextP)
  {
    memset(contextP, 0, sizeof(lwm2m_context_t));
    contextP->userData = userData;
    srand(lwm2m_gettime());
    ds_iot_ulog_high("lwm2m_init successful");
  }
  return contextP;
}

#ifdef LWM2M_CLIENT_MODE
/**
 * @fn void lwm2m_deregister()
 * @brief This function is used to deregister lwm2m context
 *
 * @param context  pointer to lwm2m context
 *
 * @return    void
 *        
 */
void lwm2m_deregister(lwm2m_context_t * context)
{
  lwm2m_server_t * server = NULL;
  if( context== NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return;
  }

  server = context->serverList;

  LOG_DEBUG("Entering");
  while (NULL != server)
  {
    registration_deregister(context, server);
    if (persis_regstatus_feat_enabled == LWM2M_TRUE)
      lwm2m_carrier_update_regstatus(server);
    ds_iot_ulog_high1 ("Deregistering for short server ID %d ",server->shortID);
    server = server->next;
  }
}

/**
  * @fn static void remove_observed_for_server()
  * @brief This function is used to remove the pending observed 
  * 	   items for deleted server.
  *
  * @param serverP	pointer to server to be deleted 
  * @param context	pointer to lwM2M2 context 
  *
  * @return 	 void
  * 			 
  */
static void remove_observed_for_server(const lwm2m_server_t * serverP, lwm2m_context_t * context)
{
  lwm2m_observed_t * observedP;

  if(context == NULL || serverP == NULL) 
  { 
    LOG_DEBUG("fun [%s] Passed  Argument is NULL",__func__);
    return ;
  }

  for (observedP = context->observedList;
       observedP != NULL;
       observedP = observedP->next)
   {
    lwm2m_watcher_t *targetP = NULL;

    if(observedP->watcherList->server->shortID == serverP->shortID)
    {
      targetP = observedP->watcherList;
      observedP->watcherList = observedP->watcherList->next;
    }
    else
    {
      lwm2m_watcher_t * parentP;

      parentP = observedP->watcherList;
      while (parentP->next != NULL
          && (parentP->next->server != NULL)&& (parentP->next->server->shortID != serverP->shortID))
      {
        parentP = parentP->next;
      }
      if(parentP->next != NULL)
      {
        targetP = parentP->next;
        parentP->next = parentP->next->next;
      }
    }
    if(targetP != NULL)
    {
      if(targetP->parameters != NULL)
      {
        lwm2m_free(targetP->parameters);
        targetP->parameters = NULL;
      }
      lwm2m_free(targetP);
      targetP = NULL;

      if(observedP->watcherList == NULL)
      {
        lwm2m_unlink_node((void **)&context->observedList, (void *)observedP);
        lwm2m_free(observedP);
        observedP = NULL;
      }
      // Sync the latest observation info to persistent storage
      if(persis_regstatus_feat_enabled == LWM2M_TRUE)
      {
        sync_observations_to_persistence(context);
      }
      qurt_signal_set(&lwm2m_signal_Handle, LOCATION_PMIN_UPDATED);
      return;
    }
  }
}

 /**
  * @fn static void update_acl_for_removed_server()
  * @brief This function is used to update the ACL objects for
  * 	   deleted server per OMA Spec (Unbootstrapping)
  *
  * @param serverP	pointer to server to be deleted 
  * @param context	pointer to lwM2M2 context 
  *
  * @return 	 void
  * 			 
  */
#ifdef AUTHORIZATION_SUPPORT 
 static void update_acl_for_removed_server(const lwm2m_server_t * serverP, lwm2m_context_t * context)
 {
   acc_ctrl_oi_t * acc_ctrl_oi = NULL;
   acc_ctrl_ri_t* next_acl = NULL;
   acc_ctrl_ri_t* current_acl = NULL;
   acc_ctrl_oi_t * next_acc_ctrl_oi = NULL;
   uint16_t acl_counter = 0;
   uint16_t shortID_highest_access = 0;
   uint16_t max_access_right_sum = 0;
   uint16_t acl_access_right_sum = 0;

   if(!enable_acl_obj)
   return;

   if (context == NULL || serverP == NULL) 
   { 
     LOG_DEBUG("fun [%s] Passed  Argument is NULL",__func__);
     return;
   }
 
   acc_ctrl_oi = (acc_ctrl_oi_t *)context->acObjInstList;
 
   while (acc_ctrl_oi != NULL) 
   {
     ds_iot_ulog_high2("Current ACL Object for object %d instance %d", acc_ctrl_oi->objectId, acc_ctrl_oi->objectInstId); 
     next_acc_ctrl_oi = acc_ctrl_oi->next;
 
     //the Server is Access Control Owner and the LwM2M Server can access the Object Instance only 
     //in an Access Control Object Instance), the Object Instance and the corresponding Access 
     //Control Object Instance MUST be deleted 
 
     current_acl = acc_ctrl_oi->accCtrlValList;
     acl_counter = 0;
     shortID_highest_access = 0;
     max_access_right_sum = 0;

     while (current_acl != NULL) 
     {
       next_acl = current_acl->next;

       if (current_acl->resInstId == serverP->shortID) 
       {
         ds_iot_ulog_high1("REMOVED ACL resource instance %d", current_acl->resInstId); 
         prv_remove_ac_val(acc_ctrl_oi, current_acl->resInstId);
       }
       else
       {
         /*if server has no access right, it should NOT be set as the new Access Control Owner.
           and only LSB 1st~4th of access control value make sense.
         */
         if((current_acl->accCtrlValue & ACC_CTRL_PERM_FULL_RIGHT) > 0)
         {
           acl_counter++;
           /*E.1.3 Unbootstrapping of OMA 1.0
             The Client MUST choose the Server who has highest sum of each number
             assigned to an access right for the Access Control Owner.If two or
             more Servers have the same sum, the Client MUST choose one of them
             as the new Access Control Owner.
           */
           acl_access_right_sum = get_acl_access_right_sum(current_acl->accCtrlValue);
           if (acl_access_right_sum > max_access_right_sum)
           {
             max_access_right_sum = acl_access_right_sum;
             shortID_highest_access = current_acl->resInstId;
           }
         }
       }
       current_acl = next_acl;
     }
 
     if (acc_ctrl_oi->accCtrlOwner == serverP->shortID)
     {
       if (acl_counter == 0 )
       {
         //remove the object instance 
         ds_iot_ulog_high2("REMOVED object %d instance %d" , acc_ctrl_oi->objectId, acc_ctrl_oi->objectInstId);
         lwm2m_remove_object_instance(acc_ctrl_oi->objectId, acc_ctrl_oi->objectInstId,false);
 
         //remove the ACL object instance
         lwm2m_remove_object_instance(LWM2M_ACL_OBJECT_ID, acc_ctrl_oi->objInstId,false);
       }
       else 
       {
         //Choose a new server owner. Server who has highest sum of each number assigned to an access right.
         acc_ctrl_oi->accCtrlOwner = shortID_highest_access;
       }
     }
     acc_ctrl_oi = next_acc_ctrl_oi; 
   }
 }
#endif
/**
 * @fn static void prv_deleteServer()
 * @brief This function is used to delete server
 *
 * @param serverP  pointer to server to be deleted
 * @param context  pointer to lwm2m context
 * @param isUpdateACL if need to update ACL and related object instance
 *
 * @return    void
 *
 */
void prv_deleteServer(lwm2m_server_t * serverP, lwm2m_context_t * context, bool isUpdateACL)
{
  lwm2m_server_t *server = NULL;
  if( serverP == NULL  || context == NULL ) 
  {
    LOG_DEBUG("fun [%s]  %s",__func__,nullError);
    return;
  }
  ds_iot_ulog_high1("GOT DELETE for SERVER ID %d", serverP->shortID); 

  //Remove pendign CoAP transaction
  lwm2m_clean_coap_session(serverP);
  timer_clr(&serverP->regOpTimer, T_SEC);

  remove_observed_for_server(serverP, context);
#ifdef AUTHORIZATION_SUPPORT
  if(isUpdateACL)
    update_acl_for_removed_server(serverP, context);
#endif

  /* If the server is getting deleted remove the corresponding entry from active 
     server list too */
  context->activeServerList = (lwm2m_active_server_t *)LWM2M_LIST_RM(context->activeServerList, serverP->shortID, &server);
  if(server != NULL)
  {
    if(context->lastActiveServer && (context->lastActiveServer->ssid == server->shortID))
    {
      context->lastActiveServer = NULL;
    }
    lwm2m_free(server);
    server = NULL;
  }
  if(persis_regstatus_feat_enabled == LWM2M_TRUE)
  {
    sync_activeserverlist_to_persistence(context);
  }
#ifdef PSM_LWM2M
lwm2m_psm_operation(context, serverP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UNCHANGED, PSM_REMOVE_NODE);

#endif
  if (NULL != serverP->registered_extobj_info)
  {
    deleteExtObjInfoList(serverP->registered_extobj_info);
	serverP->registered_extobj_info = NULL;
  }

  if (NULL != serverP->registering_extobj_info)
  {
    deleteExtObjInfoList(serverP->registering_extobj_info);
	serverP->registering_extobj_info = NULL;
  }
  
  if (NULL != serverP->location)
  {
    lwm2m_free(serverP->location);
    serverP->location = NULL;
  }
  if(serverP)
  {
    lwm2m_free(serverP);
    serverP = NULL;
  }
}

/* Retrieve the server based on short server ID */
lwm2m_server_t *lwm2m_get_server(uint16_t shortId)
{
  lwm2m_context_t *contextP = NULL;
  lwm2m_server_t *serverP = NULL;

  contextP = get_lwm2m_context();

  if(contextP == NULL || contextP->serverList == NULL)
  {
    return NULL;
  }

  serverP = (lwm2m_server_t *)LWM2M_LIST_FIND(contextP->serverList, shortId);

  return serverP;
}

/**
 * @fn static void prv_deleteServerList()
 * @brief This function is used to delete entire server list
 *
 * @param context  pointer to lwm2m context
 *
 * @return    void
 *        
 */
static void prv_deleteServerList(lwm2m_context_t * context)
{
  if( context == NULL )
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return ;
  }

  while (NULL != context->serverList)
  {
    lwm2m_server_t * server;
    server = context->serverList;
    context->serverList = server->next;
    prv_deleteServer(server, context, FALSE);
  }
}

/**
 * @fn static void prv_deleteBootstrapServer()
 * @brief This function is used to delete entire bootstrap server list
 *
 * @param contextP  pointer to lwm2m context
 *
 * @return    void
 *        
 */
static void prv_deleteBootstrapServer(lwm2m_context_t * contextP)
{
  if( contextP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return ;
  }

  LWM2M_LIST_FREE(contextP->bootstrapServer);
  contextP->bootstrapServer = NULL;
}

/**
 * @fn static void prv_deleteObservedList()
 * @brief This function is used to delete observed list
 *
 * @param contextP  pointer to lwm2m context
 *
 * @return    void
 *        
 */
static void prv_deleteObservedList(lwm2m_context_t * contextP)
{
  if( contextP == NULL ) 
  { 
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return ;
  }

  while (NULL != contextP->observedList)
  {
    lwm2m_observed_t * targetP;
    lwm2m_watcher_t * watcherP;

    targetP = contextP->observedList;
    contextP->observedList = contextP->observedList->next;

    for (watcherP = targetP->watcherList ; watcherP != NULL ; watcherP = watcherP->next)
    {
      if (watcherP->parameters != NULL) lwm2m_free(watcherP->parameters);
    }
    LWM2M_LIST_FREE(targetP->watcherList);

    lwm2m_free(targetP);
  }
}
#endif

/**
 * @fn void lwm2m_close()
 * @brief This function is used to close lwm2m context
 *
 * @param contextP  pointer to lwm2m context
 *
 * @return    void
 *        
 */
void lwm2m_close(lwm2m_context_t * contextP)
{
#ifdef LWM2M_CLIENT_MODE
  lwm2m_server_t *  serverList =  NULL;
  lwm2m_server_t *  Bootstrapserver =  NULL;
  if( contextP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return ;
  } 
  serverList = contextP->serverList;
  Bootstrapserver = contextP->bootstrapServer;
  LOG_DEBUG("Entering");
  while (serverList) 
  { 
    lwm2m_clean_coap_session(serverList);
    timer_clr(&serverList->regOpTimer, T_SEC);
    serverList = serverList->next;
  }
  if (Bootstrapserver != NULL)
  {
    lwm2m_clean_coap_session(Bootstrapserver);
    timer_clr(&Bootstrapserver->regOpTimer, T_SEC);     
  }
  prv_deleteServerList(contextP);
  prv_deleteBootstrapServer(contextP);
  prv_deleteObservedList(contextP);

  lwm2m_free(contextP->bootstrapEpName);
  contextP->bootstrapEpName = NULL;

  lwm2m_free(contextP->endpointName);
  contextP->endpointName = NULL;
  if (contextP->msisdn != NULL)
  {
    lwm2m_free(contextP->msisdn);
    contextP->msisdn = NULL;
  }
  if (contextP->altPath != NULL)
  {
    lwm2m_free(contextP->altPath);
    contextP->altPath = NULL;
  }

#endif
  deinit_value_change();
  lwm2m_free(contextP);
  contextP = NULL;

  reset_global_flags(false);
}


/**
 * @fn void lwm2m_clean_coap_session()
 *
 * @brief This function is used to close CoAP connection and destroy CoAP session
 *
 * @param serverP   poiter to lwm2m-server.
 *
 * @return    void
 *
 */
void lwm2m_clean_coap_session(lwm2m_server_t *serverP)
{
  if(serverP == NULL || serverP->coapHandle == NULL)
    return;

  if(serverP->sessionH != NULL) 
  {
    qapi_Coap_Close_Connection(serverP->coapHandle);
    ds_iot_ulog_high1("Closing COAP connection to shortID %d", serverP->shortID);
    serverP->sessionH = NULL;
  }
  if(serverP->coapHandle != NULL)
  {
    qapi_Coap_Destroy_Session(serverP->coapHandle);
    ds_iot_ulog_high1("Deregistering the coap context for shortID %d",serverP->shortID);
    serverP->coapHandle = NULL;
  }
  return;
}

#ifdef LWM2M_CLIENT_MODE 
/**
 * @fn int prv_refreshServerList()
 * @brief This function is used to refresh server list
 *
 * @param contextP  handle to lwm2m context
 *
 * @return   -1 on failure
 *            0 on success
 *        
 */
int prv_refreshServerList(lwm2m_context_t * contextP)
{
  lwm2m_server_t * targetP = NULL;
  lwm2m_server_t * nextP = NULL;

  /* Remove all servers marked as dirty */
  if (NULL == contextP) 
  {
    ds_iot_ulog_err1("Invalid input paramters - contextP[0x%x]", contextP);
    return -1;
  }

  /* Registration server list */ 
  targetP = contextP->serverList;
  contextP->serverList = NULL;
  while (targetP != NULL)
  {
    nextP = targetP->next;
    targetP->next = NULL;

    if (!targetP->dirty)
    {
      // TODO: Should we revert the status to STATE_DEREGISTERED ?
      contextP->serverList = (lwm2m_server_t *)LWM2M_LIST_ADD(contextP->serverList, targetP);
    }
    else
    {  
      ds_iot_ulog_high1("Deleting the server with short id -> %d ",targetP->shortID); 
      prv_deleteServer(targetP, contextP, TRUE);
    }

    targetP = nextP;
  }

  /* Get the updated bootstrap and registration server list */
  object_getServers(contextP);
  if(data.lwm2mH)
  {
    lwm2m_object_t           *serverObjP = NULL;
    server_instance_t        *serverInst = NULL;
    serverObjP = (lwm2m_object_t *)LWM2M_LIST_FIND(data.lwm2mH->objectList, LWM2M_SERVER_OBJECT_ID);
    if(serverObjP == NULL)
      return 0;

    targetP = contextP->serverList;
    while (targetP != NULL)
    {
      /*get the server instance to get the server instance ID*/
      serverInst = (server_instance_t *)prv_findServerInstance(serverObjP, targetP->shortID);
      if(serverInst != NULL)
      {
        /* Populate the registration retry resource values from server instance or config files*/
        prv_getRetryInfo(serverObjP, serverInst->instanceId, targetP);
        /* Populate APN link and preferred transport resources */
        prv_getServerInfo(serverObjP, serverInst->instanceId, targetP);
      }
      targetP = targetP->next;
    }
  }
  return 0;
}

/**
 * @fn int prv_refreshBSServer()
 * @brief This function is used to refresh BS server
 *
 * @param contextP          handle to lwm2m context

 * @return   -1 on failure
 *            0 on success
 *        
 */
int prv_refreshBSServer(lwm2m_context_t * contextP)
{
  if (NULL == contextP) 
  {
    ds_iot_ulog_err1("Invalid input paramters - contextP[0x%x]", contextP);
    return -1;
  }

  if(contextP->bootstrapServer != NULL)
  {
    if(enable_bs_server_replace && contextP->bootstrapServer->isBSServerAccountUpdated)
    {
      ds_iot_ulog_high("Deleting the BS server");
      prv_deleteServer(contextP->bootstrapServer, contextP, TRUE);
      contextP->bootstrapServer = NULL;
    }
    else
    {
      LOG_DEBUG("no need to refresh BS server");
      return 0;
    }
  }

  /* Get the updated bootstrap server*/
  object_getBSServer(contextP);

  return 0;
}

/**
 * @fn int lwm2m_configure()
 * @brief This function is used to configure lwm2m 
 *
 * @param contextP      pointer to lwm2m context
 * @param endpointName  pointer to end point name
 * @param msisdn        pointer to msisdn
 * @param altPath       pointer to alt path
 * @param numObject     number of objects
 * @param objectList[]  list of objects
 *
 * @return LWM2M response type 
 *        
 */
int lwm2m_configure(lwm2m_context_t * contextP,
    const char * altPath,
    uint16_t numObject,
    lwm2m_object_t * objectList[])
{
  int i = 0;
  uint8_t found = 0;
  int wms_res = EFAILURE;

  if ( contextP == NULL || objectList == NULL) 
  { 
    ds_iot_ulog_err("lwm2m_configure:NULL ARGS"); 
    return LWM2M_400_BAD_REQUEST;   
  }
  // This API can be called only once for now
  if ( (contextP->objectList != NULL) ) return LWM2M_400_BAD_REQUEST;
  //if (endpointName == NULL) return LWM2M_400_BAD_REQUEST;
  if (numObject < 3) return LWM2M_400_BAD_REQUEST;

  // Check that mandatory objects are present
  found = 0;

  for (i = 0 ; i < numObject ; i++)
  {
    if (objectList[i]->objID == LWM2M_SECURITY_OBJECT_ID) found |= 0x01;
    if (objectList[i]->objID == LWM2M_SERVER_OBJECT_ID) found |= 0x02;
    if (objectList[i]->objID == LWM2M_DEVICE_OBJECT_ID) found |= 0x04;
  }
  if (found != 0x07) return LWM2M_400_BAD_REQUEST;

  if (altPath != NULL)
  {
    if (0 == utils_isAltPathValid(altPath))
    {
      return LWM2M_400_BAD_REQUEST;
    }
    if (altPath[1] == 0)
    {
      altPath = NULL;
    }
  }

  if (altPath != NULL)
  {
    contextP->altPath = lwm2m_strdup(altPath);
    if (contextP->altPath == NULL)
    {
      return LWM2M_500_INTERNAL_SERVER_ERROR;
    }
  }

  for (i = 0; i < numObject; i++)
  {
    if (((objectList[i]->objID == LWM2M_LOCATION_OBJECT_ID) && !enable_location)
          || ((objectList[i]->objID == LWM2M_DEVICE_CAP_OBJECT_ID) && !enable_dev_cap_obj)
          || ((objectList[i]->objID == LWM2M_CONN_MONITOR_OBJECT_ID) && !enable_conn_m_obj)
          || ((objectList[i]->objID == LWM2M_SOFTWARE_MGNT_OBJECT_ID) && !enable_soft_mgnt_obj)
          || ((objectList[i]->objID == LWM2M_FIRMWARE_UPDATE_OBJECT_ID) && !enable_firmware_obj)
          || ((objectList[i]->objID == LWM2M_CONN_STATS_OBJECT_ID) && !enable_conn_stat_obj)
          || ((objectList[i]->objID == LWM2M_CELL_NW_CONN_OBJECT_ID) && !enable_cell_nw_conn_obj)
          || ((objectList[i]->objID == LWM2M_APN_CONN_PROFILE_OBJECT_ID) && !enable_apn_conn_obj)
          || ((objectList[i]->objID == LWM2M_HOST_DEVICE_OBJECT_ID) && !enable_host_dev_obj)
          || ((objectList[i]->objID == LWM2M_ACL_OBJECT_ID) && !enable_acl_obj)) continue;
    objectList[i]->next = NULL;
    contextP->objectList = (lwm2m_object_t *)LWM2M_LIST_ADD(contextP->objectList, objectList[i]);
  }
#ifdef AUTHORIZATION_SUPPORT
  update_acl_specific_list(contextP, objectList[LWM2M_ACL_OBJECT_ID]);
#endif
  wms_res = qmi_wms_set_event_report_req();
  if(wms_res == EFAILURE)
  {
    ds_iot_ulog_high(" QMI_WMS SET Event Report req failed ...!!");
  }
  else 
  {
    LOG_DEBUG(" QMI_WMS SET Event Report req Success ...!!");
  }

  LOG_DEBUG("LWM2M Configuration Successful");
  return LWM2M_NO_ERROR;
}
#endif

/**
 * @fn void lwm2m_get_server_count()
 * @brief This function is used to get the BS or DM server count
 *
 * @param context    pointer to lwm2m context
 * @param isBSServer it's BS server or DM server
 *
 * @return    the count of BS or DM server(s)
 *
 */
  int lwm2m_get_server_count(lwm2m_context_t * contextP, bool isBSServer)
{

  lwm2m_server_t        *server = NULL;
  int                    count  = 0;
  security_instance_t  *secInst = NULL;
  client_data_t          *dataP = NULL;

  if(contextP == NULL || (!isBSServer && contextP->serverList == NULL))
    return 0;

  if(isBSServer)
  {
    dataP = (client_data_t *)contextP->userData;
    if(dataP !=NULL && dataP->securityObjP != NULL)
      secInst = (security_instance_t *)dataP->securityObjP->instanceList;
    if(secInst == NULL)
    {
      return 0;
    }

    while(secInst != NULL)
    {
      if(secInst->isBootstrap)
      {
        ++count;
      }
      secInst = secInst->next;
    }
  }
  else
    for(count = 0, server = contextP->serverList; NULL != server; server = server->next)
      ++count;

  return count;
}

/**
 * @fn int trigger_retry()
 * @brief This function is used to rtrigger retry logic
 *
 * @param targetP  pointer to server context
 *
 * @return      LWM2M response   
 *        
 */
void trigger_retry(lwm2m_server_t *targetP)
{
  time_t tv_sec = lwm2m_gettime();
  
  if ((tv_sec == 0 ) || (tv_sec > 0))
    targetP->registration = tv_sec;

  /* If the retry attempts in one sequence are not exhausted then perform exponential backoff.
     If the retry attempts in one sequence are exhausted and all sequence are not exhausted
     then wait for sequence delay timer. If all the sequences are also exhausted then 
     treat this server as failure.*/
  if(targetP->retry_attempt < targetP->retry_count)
  {
    targetP->status = STATE_REG_RETRYING;
    targetP->prevAttemptTime = tv_sec;

    targetP->retry_attempt++;

    if(targetP->version == (float)LWM2M_V_1_0)
    {
      targetP->retryTimeout = (targetP->retry_timer * (pow(gRegRetrytimeoutExponent ,(targetP->retry_attempt - 1))));
    }
    else
    {
      targetP->retryTimeout = (targetP->retry_timer * (pow(2,(targetP->retry_attempt - 1))));
    }

    timer_set(&targetP->regOpTimer, targetP->retryTimeout, 0, T_SEC);
    ds_iot_ulog_high3("Re-trying \"registration request\" for SSID[%d] with retryTimeout[%d] retry attempt[%d]", 
              targetP->shortID, targetP->retryTimeout, targetP->retry_attempt);
  }
  else
  {
    if(targetP->seq_retry_attempt < targetP->seq_retry_count && targetP->seq_delay_timer != 0xFFFFFFFF)
    {
      targetP->seq_retry_attempt++;
      targetP->status = STATE_REG_FAILED;
      targetP->retryTimeout = 0;
      targetP->retry_attempt = 0;
      targetP->prevAttemptTime = 0;

      targetP->next_cycle_for_registration = lwm2m_gettime() + targetP->seq_delay_timer;
    }
    else
    {
      targetP->seq_retry_attempt = targetP->seq_retry_count;
      targetP->retry_attempt = targetP->retry_count;
      targetP->status = STATE_REG_FAILED;
      targetP->next_cycle_for_registration = 0;
    }
    ds_iot_ulog_err3 ("Max \"registration request\" retries reached for SSID[%d] with next attempt after[%d] for sequence [%d]",
                targetP->shortID,targetP->seq_delay_timer, targetP->seq_retry_attempt);
  }
  if (persis_regstatus_feat_enabled == LWM2M_TRUE)
    lwm2m_carrier_update_regstatus(targetP);
#ifdef PSM_LWM2M
  lwm2m_psm_operation(data.lwm2mH, targetP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UPDATE, PSM_MODIFY_NODE);

#endif

}

void trigger_factory_reset_util(bool is_immediate, qapi_Net_LWM2M_Event_t event)
{
  time_t bs_wait_time = 0, current_time =0;
  int fd = 0;
  qurt_signal_set(&lwm2m_signal_Handle, LWM2M_FACTORY_RESET_SET);
  /* Indicate the applications that the LWM2M client has failed to register with the server */

  if(event == QAPI_NET_LWM2M_BOOTSTRAP_FAILED_E)
  {
    lwm2m_send_internal_ind_to_application(event);
    ds_iot_ulog_high1("Rebootstrapping due to consistency check fail after %u sec", data.cfg_info.wait_time_for_rereg);
  }
  else if(is_immediate)
  {
    ds_iot_ulog_high("Rebootstrapping due to change in MSISDN");
  }
  else
  {
    lwm2m_send_internal_ind_to_application(event);
    ds_iot_ulog_high1("Rebootstrapping due to blocking server failure after %u sec", data.cfg_info.wait_time_for_rereg);
  }

  qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
  /* Indicate that the application extended object creation window has been extended due to re-bootstrapping.
   * Any applications that failed to create objects within the window are given another chance to register. 
   * Don't purge any persisted object list at this point, only additions can be made to the list. */
  data.glwm2m_app_ctx.bs_performed = FALSE;
  data.glwm2m_app_ctx.purged_app_obj_persistence = TRUE;  
  qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex); 

  /* Indicate the applications that the LWM2M client is re-bootstrapping with the server */
  lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_DEVICE_REBOOTSTRAPPING_E);
  LOG_DEBUG("EFS_OPT: File open %s",BOOTSTRAP_STATUS_PERSISTENCE_FILE);
  fd = efs_open(BOOTSTRAP_STATUS_PERSISTENCE_FILE, O_CREAT | O_WRONLY, 00644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open bs status reg file");
  }
  else
  {
    current_time = lwm2m_gettime();
    if(!is_immediate)
      bs_wait_time = current_time + data.cfg_info.wait_time_for_rereg;
    else
      bs_wait_time = current_time;
    LOG_DEBUG("EFS_OPT: File write %s",BOOTSTRAP_STATUS_PERSISTENCE_FILE);
    if (EFAILURE == efs_write(fd, &bs_wait_time, sizeof(time_t)))
    {
      ds_iot_ulog_err("failed to store persistent value of bs status file");
    }
    efs_close(fd);
  }
}
/**
 * @fn static bool prv_write_resource_inst_val()
 * @brief Function to create a resource instance or overwrite existing resource instance for int
 * @return true on success, 
 *         false on failure
 *
 */
bool prv_write_resource_inst_val(resource_instance_int_list_t** rsList,
    uint16_t resId, uint16_t resValue, bool *val_change)
{
  bool ret = false;
  resource_instance_int_list_t *resInst = NULL, *temp = *rsList;

  if(val_change == NULL)
  {
    return ret;
  }
  while(temp)
  {
    if(temp->resInstId == resId)
    {
      if(temp->InstValue != resValue)
      {
        *val_change = true;
      }
      temp->InstValue = resValue;
      return true;
    }
    temp = temp->next;
  }

  resInst = (resource_instance_int_list_t *)lwm2m_malloc(sizeof(resource_instance_int_list_t));
  if (resInst==NULL)
  {
    return ret;
  }
  else
  {
    memset(resInst, 0, sizeof(resource_instance_int_list_t));
    resInst->resInstId   = resId;
    resInst->InstValue   = resValue;

    *rsList = (resource_instance_int_list_t*)LWM2M_LIST_ADD(*rsList, resInst);
    *val_change = true;
    ret = true;
  }

  return ret;
}

/**
 * @fn static bool prv_write_string_resource_inst_val()
 * @brief Function to create a resource instance or overwrite existing resource instance for string
 * @return true on success, 
 *         false on failure
 *
 */
bool prv_write_string_resource_inst_val(resource_instance_string_list_t** rsList,
    uint16_t resId, char* resValue, bool *val_change)
{
  bool ret = false;
  resource_instance_string_list_t *resInst = NULL, *temp = *rsList;
  if(val_change == NULL)
  {
    return ret;
  }
  while(temp)
  {
    if(temp->resInstId == resId)
    {
      if(lwm2m_strcmp(temp->InstValue, resValue) != 0)
      {
        *val_change = true;
      }
      LWM2M_STRCPY(temp->InstValue,resValue, MAX_STRING_LEN);
      return true;
    }
    temp = temp->next;
  }

  resInst = (resource_instance_string_list_t *)lwm2m_malloc(sizeof(resource_instance_string_list_t));
  if (resInst==NULL)
  {
    return ret;
  }
  else
  {
    memset(resInst, 0, sizeof(resource_instance_string_list_t));
    resInst->resInstId   = resId;
    LWM2M_STRCPY(resInst->InstValue,resValue, MAX_STRING_LEN);
    *rsList = (resource_instance_string_list_t*)LWM2M_LIST_ADD(*rsList, resInst);
    *val_change = true;
    ret = true;
  }

  return ret;
}

int lwm2m_strcmp(char *dst, char *src)
{
  if(dst == NULL && src == NULL)
  {
    return 0;
  }

  if(dst == NULL)
    return -1;

  if(src == NULL)
    return -1;

  return strcmp(dst,src);
}

bool prv_write_string_resource_val(resource_instance_string_list_t** rsList,
                                   char* resValue)
{
  bool ret = false;
  uint16_t resId = 0;
  resource_instance_string_list_t *resInst = NULL, *temp = *rsList;
  while(temp)
  {
    if(!strcasecmp(temp->InstValue,resValue))
    {
      return true;
    }
    temp = temp->next;
  }

  temp = *rsList;

  resId = lwm2m_list_newId((lwm2m_list_t *)temp);

  resInst = (resource_instance_string_list_t *)lwm2m_malloc(sizeof(resource_instance_string_list_t));
  if (resInst==NULL)
  {
    return ret;
  }
  else
  {
    memset(resInst, 0, sizeof(resource_instance_string_list_t));
    resInst->resInstId   = resId;
    LWM2M_STRCPY(resInst->InstValue,resValue, MAX_STRING_LEN);
    *rsList = (resource_instance_string_list_t*)LWM2M_LIST_ADD(*rsList, resInst);
    ret = true;
  }

  return ret;
}

bool prv_delete_string_resource_inst_val(resource_instance_string_list_t** rsList,
                                         uint16_t resId)
{
  bool ret = false;
  resource_instance_string_list_t *resInst = NULL;

  if(*rsList == NULL)
  {
    return ret;
  }

  *rsList = (resource_instance_string_list_t*)LWM2M_LIST_RM(*rsList, resId, &resInst);

  if (resInst==NULL)
  {
    return ret;
  }
  else
  {
    free(resInst);
    resInst = NULL;
    ret = true;
  }

  return ret;
}



/** 
 * @fn     int get_mtu_for_carrier_apn_cfg()
 * @brief  This function is used to get MTU info based on carrier apn config 
 * @param  carrier_apn_cfg[IN]  - pointer to carrier apn config 
 *         targetP[IN]          - pointer to lwm2m server list 
           mtu_size[OUT]        - pointer to store MTU size
 * @return on success  - 0
 on error    - LWM2M_400_BAD_REQUEST
 */  
int get_mtu_for_carrier_apn_cfg(carrier_apn_cfg_t *carrier_apn_cfg, lwm2m_server_t *targetP, uint32_t *mtu_size)
{

   carrier_apn_cfg_t *apn_cfg = apn_cfg = carrier_apn_cfg;
   int ret = 0;
   
   if ( carrier_apn_cfg == NULL || targetP == NULL || mtu_size == NULL) 
   {
      LOG_DEBUG("Invalid params - server_info[0x%x], mtu_size[0x%x], carrier_apn_cfg[0x%x]", targetP, mtu_size, carrier_apn_cfg);
      return LWM2M_400_BAD_REQUEST;
   }
  /* Retrieve APN configuration by short ID */
  apn_cfg = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(apn_cfg, targetP->shortID);  
  if (NULL == apn_cfg)
  {
    LOG_DEBUG("Unable to find the information associated with server[%d]", targetP->shortID);    
     return LWM2M_400_BAD_REQUEST;
  }
   
  /* Non-IP or IPv4*/ 
  if (AF_NONIP == apn_cfg->transport_binding || AF_INET == apn_cfg->transport_binding)
  {
    ret = iot_dss_get_mtu_by_hndl(apn_cfg->callHdl_v4_nip, mtu_size);
  }
  /* IPv6 */ 
  else if (AF_INET6 == apn_cfg->transport_binding)
  {
    ret = iot_dss_get_mtu_by_hndl(apn_cfg->callHdl_v6, mtu_size);
  }

  /* Dual-IP */ 
  else if (AF_INET_DUAL46 == apn_cfg->transport_binding)
  {
     if(targetP->conn_failed_with_v6 == LWM2M_FALSE)
     {
        ret = iot_dss_get_mtu_by_hndl(apn_cfg->callHdl_v6, mtu_size);
     }
     if(targetP->conn_failed_with_v6 == LWM2M_TRUE || ret == -1)
     {
        ret = iot_dss_get_mtu_by_hndl(apn_cfg->callHdl_v4_nip, mtu_size);
     }
  }
  if(-1 == ret)
  {
       ds_iot_ulog_err2("Failed to get MTU for SSID %d, IP_FAMILY %d", apn_cfg->short_server_id, apn_cfg->ip_family);
       return LWM2M_400_BAD_REQUEST;
  }

  ds_iot_ulog_high2 ("MTU for Short Server ID[%d] is %d",targetP->shortID, *mtu_size) ;
  return 0;
}


/** 
 * @fn     int get_ip_for_carrier_apn_cfg()
 * @brief  This function is used to get ip adress based on carrier apn config 
 * @param  carrier_apn_cfg - pointer to carrier apn config 
 *         server          - pointer to lwm2m server list 
           server_ip       - pointer to store ip address
 * @return on success  - 0
 on error    - LWM2M_400_BAD_REQUEST
 */  
int get_ip_for_carrier_apn_cfg(carrier_apn_cfg_t *carrier_apn_cfg, lwm2m_server_t *server, char *server_ip)
{
  int ip_count = 0;
  struct in_addr  ipaddr;
  struct in6_addr ip6addr;
  qapi_DSS_Addr_Info_t *info_ptr = NULL; 
  lwm2m_server_t server_info;
  int result = LWM2M_400_BAD_REQUEST; 
  carrier_apn_cfg_t *apn_cfg = carrier_apn_cfg;
 
  if ((server == NULL) || 
      (server_ip == NULL)   || 
      (carrier_apn_cfg == NULL))
  {
    ds_iot_ulog_err3("Invalid params - server_info[0x%x], server_ip[0x%x], carrier_apn_cfg[0x%x]", server, server_ip, carrier_apn_cfg); 
    return LWM2M_400_BAD_REQUEST;
  }

  /* Copy the server info and update the copy. Only on success, update the original server info. */ 
  server_info = *server;

  /* Retrieve APN configuration by short ID */
  apn_cfg = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(apn_cfg, server_info.shortID);  
  if (NULL == apn_cfg)
  {
    ds_iot_ulog_err1("Unable to find the information associated with server[%d]", server_info.shortID);    
    goto carrier_apn_err;
  }
   
  /* Copy the config options from the APN config to the server resource */ 
  if(server_info.version == (float)LWM2M_V_1_0)
  server_info.bs_if_reg_fails       = apn_cfg->bs_if_reg_fails;
  server_info.ip_family_configured  = apn_cfg->transport_binding;
  server_info.local_ip.sa_family    = apn_cfg->transport_binding;  /* Gets over-written for Dual-IP scenario to IPv6 or IPv4 */

  /* Non-IP */ 
  if (AF_NONIP == apn_cfg->transport_binding)
  {
    uint32_t mtu_size = 0x00; 

    /* Non-IP transport doesn't have any IP address associated. Instead, check if the handle has associated MTU info */
    qapi_DSS_Get_Link_Mtu(get_data_call_hndl(apn_cfg->callHdl_v4_nip), &mtu_size);
    if (!mtu_size)
      return result;

    /* Update the server resource with the updated information (as it is successful) */ 
    //*server = server_info;
    if(server_info.version == (float)LWM2M_V_1_0)
    server->bs_if_reg_fails = server_info.bs_if_reg_fails;
    server->ip_family_configured = server_info.ip_family_configured;
    server->local_ip.sa_family = server_info.local_ip.sa_family;     
    return 0;
  }
  /* IPv4 */
  else if (AF_INET == apn_cfg->transport_binding)
  {
    if (NULL == (info_ptr = iot_dss_get_ip_info_by_hndl(apn_cfg->callHdl_v4_nip, &ip_count)))
    {
      ds_iot_ulog_err1("Retrieving IPv4 information for SSID[%d] failed", apn_cfg->short_server_id);
      goto carrier_apn_err;
    }    
  }
  /* IPv6 */ 
  else if (AF_INET6 == apn_cfg->transport_binding)
  {
    if (NULL == (info_ptr = iot_dss_get_ip_info_by_hndl(apn_cfg->callHdl_v6, &ip_count)))
    {
      ds_iot_ulog_err1("Retrieving IPv6 information for SSID[%d] failed", apn_cfg->short_server_id);
      goto carrier_apn_err;
    }      
  }
  /* Dual-IP */ 
  else if (AF_INET_DUAL46 == apn_cfg->transport_binding)
  {
    /* IPv6 connection validity */
    if (server_info.conn_failed_with_v6 == LWM2M_FALSE)
    {
      /* Update the IP family information to IPv6 which might get overridden by IPv4 in case of any failure */ 
      server_info.local_ip.sa_family = AF_INET6;
      if (NULL == (info_ptr = iot_dss_get_ip_info_by_hndl(apn_cfg->callHdl_v6, &ip_count)))
      {
        ds_iot_ulog_err1("Retrieving IPv6 information for SSID[%d] failed... trying IPv4 (Dual-IP)", apn_cfg->short_server_id);
    
        /* Update the IP family information to IPv4 */ 
        server_info.local_ip.sa_family = AF_INET;
        if (NULL == (info_ptr = iot_dss_get_ip_info_by_hndl(apn_cfg->callHdl_v4_nip, &ip_count)))
        {
          ds_iot_ulog_err1("Retrieving IPv4 information for SSID[%d] failed (Dual-IP)", apn_cfg->short_server_id);
          goto carrier_apn_err;
        }       
      }
    } 
    else 
    {
      /* Dual-IP scenario IPv6 failed to connect to the server. Re-try with IPv4 before giving-up */ 
      ds_iot_ulog_high("Dual-IP scenario IPv6 failed. Re-try with IPv4 before giving-up");
      
      /* Update the IP family information to IPv4 */ 
      server_info.local_ip.sa_family = AF_INET;
      if (NULL == (info_ptr = iot_dss_get_ip_info_by_hndl(apn_cfg->callHdl_v4_nip, &ip_count)))
      {
        ds_iot_ulog_err1("Retrieving IPv4 information for SSID[%d] failed (Dual-IP)", apn_cfg->short_server_id);
        goto carrier_apn_err;
      }
    } 
  }
  else
  {
    ds_iot_ulog_err1("Transport binding for SSID[%d] is not valid", apn_cfg->short_server_id);
    goto carrier_apn_err;
  }

  /* Valid IP information */ 
  if (info_ptr->iface_addr_s.valid_addr)
  {
    /* IPv4 */ 
    if (AF_INET == server_info.local_ip.sa_family)
    {
      memset(&ipaddr, 0, sizeof(ipaddr));
      memscpy(&ipaddr, sizeof(struct in_addr), &info_ptr->iface_addr_s.addr.v4, sizeof(struct in_addr));

      if (NULL == inet_ntop(AF_INET, &ipaddr, server_ip, PRV_ADDRSTRLEN))
      {
        ds_iot_ulog_err1("Fetching IPv4 failed for SSID[%d]", apn_cfg->short_server_id);
        result = LWM2M_400_BAD_REQUEST;
    
        goto carrier_apn_err;
      }
    }

    /* IPv6 */
    if (AF_INET6 == server_info.local_ip.sa_family)
    {
      memset(&ip6addr, 0, sizeof(ip6addr));
      memscpy(&ip6addr, sizeof(struct in6_addr), &info_ptr->iface_addr_s.addr.v6_addr8, sizeof(struct in6_addr));

      if (NULL == inet_ntop(AF_INET6, &ip6addr, server_ip, PRV_ADDRSTRLEN))
      {
        ds_iot_ulog_err1("Fetching IPv6 failed for SSID[%d]", apn_cfg->short_server_id);
        result = LWM2M_400_BAD_REQUEST;
    
        goto carrier_apn_err;
      }
    }
  }
  else 
  {
    ds_iot_ulog_err1("Valid address not found for SSID[%d]", apn_cfg->short_server_id);
    result = LWM2M_400_BAD_REQUEST;
    
    goto carrier_apn_err;
  }
  
  ds_iot_log_strf_high("Found IP[%s] associated with APN[%s] for SSID[%d]", server_ip, apn_cfg->apn_name, server_info.shortID);

  /* Update the server resource with the updated information (as it is successful) */ 
  //*server = server_info;
  if(server_info.version == (float)LWM2M_V_1_0)
  server->bs_if_reg_fails = server_info.bs_if_reg_fails;
  server->ip_family_configured = server_info.ip_family_configured;
  server->local_ip.sa_family = server_info.local_ip.sa_family; 
  result = 0x00;

carrier_apn_err: 
  if (info_ptr)
    lwm2m_free(info_ptr);
  
  return result;
}


qapi_Status_t lwm2m_send_internal_ind_to_application(qapi_Net_LWM2M_Event_t event)
{
  lwm2m_server_data_t lwm2m_info;
  memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t ));
  lwm2m_info.msg_type = QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E;
  lwm2m_info.event    = event;
  return lwm2m_client_application_notification(&lwm2m_info);
}

char* prv_get_end_point_name(lwm2m_context_t * contextP, int endpoint_type)
{
  char* ep_name  = NULL;
  char* imei_buf = NULL;
  char* imsi_buf = NULL;
  uint8_t ep_name_len = 0;

  imei_buf = (char*)get_device_serial_number();
  /*always need IMEI for end point name*/
  if(NULL == imei_buf) 
  {
    ds_iot_ulog_err("Fetching IMEI failed, hence quit the application");
    return NULL;    
  }

  if(endpoint_type == LWM2M_IMEI_MSISDN_URN && NULL == contextP->msisdn)
  {
    ds_iot_ulog_err("Fetching MSISDN failed, hence quit the application");
    goto end_handling;    
  }
  
  if(endpoint_type == LWM2M_IMEI_IMSI_URN || endpoint_type == LWM2M_IMEI_IMSI)
  {
    imsi_buf = (char*)get_imsi_frm_physical_slot();
    if(NULL == imsi_buf)
    {
      ds_iot_ulog_err("Fetching IMSI failed, hence quit the application");
      goto end_handling;    
    }
  }

  ep_name_len = strlen(EP_NAME_IMEI_MSISDN);     //EP_NAME_IMEI_MSISDN is max length of endpoint
  ep_name = lwm2m_calloc(1, ep_name_len + 1);
  if(ep_name == NULL)
  {
    goto end_handling;
  }
  
  switch(endpoint_type)
  {
   case LWM2M_IMEI_MSISDN_URN:
     snprintf(ep_name, ep_name_len + 1, "urn:imei-msisdn:%s-%s", imei_buf, contextP->msisdn);
     break;
   case LWM2M_IMEI_IMSI_URN:
     snprintf(ep_name, ep_name_len + 1, "urn:imei-imsi:%s-%s",   imei_buf, imsi_buf);
     break;
   case LWM2M_IMEI_IMSI:
     snprintf(ep_name, ep_name_len + 1, "%s;%s",                 imei_buf, imsi_buf);
     break;
   case LWM2M_IMEI_URN:
   default:
     snprintf(ep_name, ep_name_len + 1, "urn:imei:%s",           imei_buf);   	
     break;
  }

  end_handling: 
  if(imei_buf)
    lwm2m_free(imei_buf);
  if(imsi_buf)
    lwm2m_free(imsi_buf);
  
  return ep_name;
}
/**
 * @fn int lwm2m_step()
 * @brief This function is used to periodically perform update 
 *        registration, notification and set timeout value
 *
 * @param contextP   handle to lwm2m context
 * @param timeoutP   handle to timeout value
 *
 * @return    LWM2M response type
 *        
 */
int lwm2m_step(lwm2m_context_t * contextP, time_t * timeoutP)
{
  time_t tv_sec, current_time;
  int result = 0;
  lwm2m_server_t *serverP = NULL; 
  char * name = NULL;
  char * bootstrap_name = NULL;
  const char * msisdn_buf = NULL;
  char msisdn_buf_pr[MAX_MSISDN_LEN + 1] = {0};
  int res = -1;
  char msisdn[MAX_MSISDN_LEN + 1] ={0};
  char ServerIp[PRV_ADDRSTRLEN] = {0};
  bool msisdn_rereg = false;

  current_time = lwm2m_gettime();
  if( contextP == NULL || timeoutP == NULL ) 
  { 
    ds_iot_ulog_err("fun [lwm2m_step]  NULL Error"); 
    return LWM2M_400_BAD_REQUEST;
  }
  LOG_DEBUG("timeoutP: %" PRId64, *timeoutP);
  tv_sec = lwm2m_gettime();
  //Warning Fix: pointless comparison of unsigned integer with zero

#ifdef LWM2M_CLIENT_MODE
#ifdef LWM2M_WITH_LOGS
  LOG_DEBUG("State: %s", STR_STATE(contextP->state));
  LOG_TO_CONSOLE("State: %d", STR_STATE(contextP->state));
#endif
  // state can also be modified in bootstrap_handleCommand().

next_step:
  switch (contextP->state)
  {
    case STATE_INITIAL:
      if (0 != prv_refreshServerList(contextP) || 0 != prv_refreshBSServer(contextP))
        return LWM2M_503_SERVICE_UNAVAILABLE;

#ifdef LWM2M_BOOTSTRAP

      if (contextP->bootstrapServer != NULL) 
      {
        serverP = contextP->bootstrapServer;
        if(serverP != NULL)
        {
          if (serverP->isBootstrapped == LWM2M_TRUE) 
          {
            #ifdef PSM_LWM2M
            lwm2m_psm_operation(contextP, serverP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UNCHANGED , PSM_REMOVE_NODE);
            #endif
            serverP = serverP->next ; 
            isBootstrappedflag = LWM2M_TRUE ;
            ds_iot_ulog_high("Bootstrap procedure already completed");
          }
          else 
          {
            isBootstrappedflag = LWM2M_FALSE;
            ds_iot_ulog_high("Bootstrap procedure required") ;
          }
        }
      } 
      else
      {
        ds_iot_ulog_high ("BS server list NULL , going with DM servers");
        isBootstrappedflag = LWM2M_TRUE;
        /* For rebootstrap case create a PSM node and vote for PSM.
           When wait time for rebootstrap is finished, remove the node from PSM node list*/
         if(current_time < contextP->time_for_bs)
         {
            lwm2m_uri_t uri;
           if (*timeoutP > (contextP->time_for_bs - current_time))
           {
              *timeoutP = contextP->time_for_bs - current_time;
           }
  
          #ifdef PSM_LWM2M
          if(lwm2m_psm_enable == TRUE)
          {
            /* Update PSM node associated with BS server due to change in server
               object fields.*/
            lwm2m_psm_scheduler_t *psm_node = NULL;
            memset(&uri, 0x00, sizeof(lwm2m_uri_t));
            psm_node = lwm2m_psm_find_node(contextP, 0x00, uri, PSM_NODE_REBOOTSTRAP);
            if(psm_node == NULL)
            {
              lwm2m_psm_operation(contextP, 0x00, &uri, PSM_NODE_REBOOTSTRAP, PSM_NODE_UNCHANGED, PSM_ADD_NODE);
            }
            else
            {
              lwm2m_psm_operation(contextP, 0x00, &uri, PSM_NODE_REBOOTSTRAP, PSM_NODE_UNCHANGED, PSM_MODIFY_NODE);
            }
          }
          #endif
  
           break;
        }
        else
        {
          #ifdef PSM_LWM2M
          lwm2m_psm_operation(contextP, 0x00, NULL, PSM_NODE_REBOOTSTRAP, PSM_NODE_UNCHANGED, PSM_REMOVE_NODE);
          #endif
        }
      }

      if (is_MSISDN_set_from_UIM == FALSE)
      {
        is_MSISDN_set_from_UIM = TRUE;

        msisdn_buf = (const char *)get_msisdn_frm_uim();
        if(!msisdn_buf)
        {
          contextP->msisdn = NULL;
        }
        else
        {
           if (data.cfg_info.truncate_msisdn)
           {
              if (strlen (msisdn_buf) > MSISDN_LEN)
              {
                 strlcpy(msisdn, msisdn_buf+(strlen (msisdn_buf) - MSISDN_LEN), MSISDN_LEN+1);
              }
              else
              {
                 strlcpy(msisdn, msisdn_buf, MSISDN_LEN+1);
              }
           }
           else
           {
              strlcpy(msisdn, msisdn_buf, MAX_MSISDN_LEN+1);
           }
          contextP->msisdn = lwm2m_strdup(msisdn);
        } 

        res = get_msisdn_from_persistence(msisdn_buf_pr);
        if(lwm2m_strcmp(msisdn_buf_pr, msisdn))
        {
          store_msisdn_to_persistence(msisdn);
        }
        ds_iot_log_strf_high("msisdn: \"%s\", Msisdn Old:\"%s\"",msisdn, msisdn_buf_pr);

        if(res== 0)
        {
          if(!strncmp(msisdn_buf_pr, msisdn, MAX_MSISDN_LEN)){
            goto msisdn_check_pass;
          }
          if(data.cfg_info.reg_endpoint_name == LWM2M_IMEI_MSISDN_URN)
          {
            if(contextP->bootstrapServer != NULL){
              trigger_factory_reset_util(true, QAPI_NET_LWM2M_STATE_INITIAL_E);
              break;
            }
            else{
              msisdn_rereg = true;
            }
          }
        }

msisdn_check_pass:
// Populate the Registration endpoint name// Populate the Registration endpoint name
        name = prv_get_end_point_name(contextP, data.cfg_info.reg_endpoint_name);
        if(NULL == name)
        { 
            m2m_log_printf("LWM2M_LOG: Populating %s EP name failed", "Reg");
            ds_iot_log_strf_err("LWM2M_LOG: Populating %s EP name failed", "Reg");
            return LWM2M_503_SERVICE_UNAVAILABLE;
        }

        //Populate the bootstrap endpoint name
        bootstrap_name = prv_get_end_point_name(contextP, data.cfg_info.bootstrap_endpoint_name);
        if (NULL == bootstrap_name)
        { 
            m2m_log_printf("LWM2M_LOG: Populating %s EP name failed", "Bootstrap");
            ds_iot_log_strf_err("LWM2M_LOG: Populating %s EP name failed", "Bootstrap");
            return LWM2M_503_SERVICE_UNAVAILABLE;
        }
        
        ds_iot_log_strf_high("LWM2M_LOG: endpointName: %s, bootstrap_name: %s", name, bootstrap_name) ;
        m2m_log_printf ("LWM2M_LOG: endpointName: %s, bootstrap_name: %s", name, bootstrap_name);
        contextP->endpointName    = name;
        contextP->bootstrapEpName = bootstrap_name;

        lwm2m_free((void *)msisdn_buf);
        name = NULL;
        bootstrap_name = NULL;
        msisdn_buf = NULL;
      }

      if (isBootstrappedflag == LWM2M_FALSE)
      {
        if (contextP->bootstrapServer != NULL) 
        {
          lwm2m_uri_t uri;
          /* For rebootstrap case create a PSM node and vote for PSM.
           When wait time for rebootstrap is finished, remove the node from PSM node list*/
          if(current_time < contextP->time_for_bs)
          {
            if (*timeoutP > (contextP->time_for_bs - current_time))
            {
              *timeoutP = contextP->time_for_bs - current_time;
            }
  
            #ifdef PSM_LWM2M
            if(lwm2m_psm_enable == TRUE)
            {
              /* Update PSM node associated with BS server due to change in server
                 object fields.*/
              lwm2m_psm_scheduler_t *psm_node = NULL;
              memset(&uri, 0x00, sizeof(lwm2m_uri_t));
              psm_node = lwm2m_psm_find_node(contextP, 0x00, uri, PSM_NODE_REBOOTSTRAP);
              if(psm_node == NULL)
              {
                lwm2m_psm_operation(contextP, 0x00, &uri, PSM_NODE_REBOOTSTRAP, PSM_NODE_UNCHANGED, PSM_ADD_NODE);
              }
              else
              {
                lwm2m_psm_operation(contextP, 0x00, &uri, PSM_NODE_REBOOTSTRAP, PSM_NODE_UNCHANGED, PSM_MODIFY_NODE);
              }
            }
            #endif
  
            break;
          }
          else
          {
            #ifdef PSM_LWM2M
            lwm2m_psm_operation(contextP, 0x00, NULL, PSM_NODE_REBOOTSTRAP, PSM_NODE_UNCHANGED, PSM_REMOVE_NODE);
            #endif
          }

          contextP->state = STATE_BOOTSTRAP_REQUIRED;

          /* Stop the data calls with the APNs associated with the registration servers */ 
          lwm2m_datacall_stop(contextP, LWM2M_STOP_REGISTRATION_DATA_CALL, NULL, false);

          /* Start the data calls with the APNs associated with the bootstrap servers */ 
          if (-1 == lwm2m_datacall_start(contextP, LWM2M_START_BOOTSTRAP_DATA_CALL, NULL, true))
            return LWM2M_503_SERVICE_UNAVAILABLE;
        } 
        else 
        {
          ds_iot_ulog_err("No BS server configured.");
          return LWM2M_400_BAD_REQUEST;
        }

        goto next_step;
      } 
      else if (isBootstrappedflag == LWM2M_TRUE)  
#endif                                                                         
      {
        ds_iot_ulog_high ("BS completed, Registration starts ");

        if (contextP->serverList != NULL)
        {
          serverP = contextP->serverList;
          contextP->state = STATE_REGISTER_REQUIRED;
          m2m_log_printf("State: %s", STR_STATE(contextP->state));

          /* Stop the data calls with the APNs associated with the bootstrap servers */ 
          lwm2m_datacall_stop(contextP, LWM2M_STOP_BOOTSTRAP_DATA_CALL, NULL, true);

          /* Start the data calls with the APNs associated with the registration servers */ 
          if (-1 == lwm2m_datacall_start(contextP, LWM2M_START_REGISTRATION_DATA_CALL, NULL, true))
            return LWM2M_503_SERVICE_UNAVAILABLE;

          while(serverP)
          {
            if ((STATE_REGISTERED == serverP->status) &&
                (NULL == serverP->sessionH))
            {
              if (get_ip_for_carrier_apn_cfg(contextP->apn_cfg, serverP, ServerIp) != LWM2M_400_BAD_REQUEST)
              {
                serverP->sessionH = lwm2m_connect_server(serverP, contextP);
                if (0 == serverP->sessionH)
                {
                  trigger_retry(serverP);
                  return LWM2M_500_INTERNAL_SERVER_ERROR;
                }
                else
                {
                  if(msisdn_rereg){
                    serverP->trigger_registration = true;
                    ds_iot_ulog_high1("Re-Reg: Change in MSISDN for ssid[%d]", serverP->shortID);
                    qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
                  }
                  else if (data.cfg_info.reg_update_on_reconnect== LWM2M_TRUE)
                  {
                    lwm2m_update_registration(contextP,serverP->shortID, false);
                  }
                }
              }
              else
              {
                ds_iot_ulog_high1("No IP address for Server id[%d] so not connecting  ",serverP->shortID);
              }
            }
            serverP = serverP->next;
          }
        }
        else
        {
          ds_iot_ulog_high ("ServerList is NULL ");
#ifdef LWM2M_BOOTSTRAP        
          isBootstrappedflag = LWM2M_FALSE;
#endif 
          contextP->state = STATE_BOOTSTRAP_REQUIRED;
  
          /* Stop the data calls with the APNs associated with the bootstrap and registration servers */ 
          lwm2m_datacall_stop(contextP, LWM2M_STOP_BOOTSTRAP_DATA_CALL, NULL, false);  
          lwm2m_datacall_stop(contextP, LWM2M_STOP_REGISTRATION_DATA_CALL, NULL, false);

          /* Start the data calls with the APNs associated with the bootstrap servers */ 
          if (-1 == lwm2m_datacall_start(contextP, LWM2M_START_BOOTSTRAP_DATA_CALL, NULL, true))
            return LWM2M_503_SERVICE_UNAVAILABLE;
          
          m2m_log_printf("State: %s", STR_STATE(contextP->state));
        }
      }
      goto next_step;
      /* commented the break to address the dead code, as this case is currently not supported 
         break; */

    case STATE_BOOTSTRAP_REQUIRED:
    {
#ifdef LWM2M_BOOTSTRAP
      if (contextP->bootstrapServer != NULL)
      {
        qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
        /* Extensible object creation window during bootstrapping process has started */        
        data.glwm2m_app_ctx.lwm2m_state = STATE_BOOTSTRAP_REQUIRED;        
        //qurt_signal_set(&data.glwm2m_app_ctx.lwm2m_app_global_signal, LWM2M_BOOTSTRAP_REQUIRED);            
        qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

        /* Indicate the applications that the LWM2M client module requires bootstrapping */  
        lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_BOOTSTRAP_REQUIRED_E);

        /* Start the bootstrap process */ 
        bootstrap_start(contextP);
        contextP->state = STATE_BOOTSTRAPPING;
        m2m_log_printf("State: %s", STR_STATE(contextP->state));
        ds_iot_log_strf_high("State: %s ", STR_STATE(contextP->state));
        bootstrap_step(contextP, tv_sec, timeoutP);
        break;
      }
#endif
      {
        LOG_DEBUG("STATE_BOOTSTRAP_REQUIRED returning 503");
        return LWM2M_503_SERVICE_UNAVAILABLE;
      }
      /* commented the break to address the dead code, as this case is currently not supported 
         break; */
    }

#ifdef LWM2M_BOOTSTRAP
    case STATE_BOOTSTRAPPING:
    {
      switch (bootstrap_getStatus(contextP))
      {
        case STATE_BS_FINISHED:
        {
          contextP->state = STATE_INITIAL;
          m2m_log_printf("State: %s", STR_STATE(contextP->state));
          ds_iot_log_strf_high ("State: %s ", STR_STATE(contextP->state));

          /* Closing bootstrap server connection as soon as bootstrapping is done*/
          if(contextP->bootstrapServer != NULL) 
          {
            serverP = contextP->bootstrapServer;
            if(serverP != NULL)
            {
              if (serverP->sessionH != NULL) 
              {
                ds_iot_ulog_high("Closing BS session"); 
                qapi_Coap_Close_Connection(serverP->coapHandle);
                #ifdef PSM_LWM2M
                lwm2m_psm_operation(contextP, serverP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UNCHANGED, PSM_REMOVE_NODE);
                #endif
                serverP->sessionH = NULL;
              }
            }
          }
          /* Indicate the applications that the LWM2M client has successfully completed bootstrapping with the server */
          lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_BOOTSTRAP_COMPLETED_E);

          goto next_step;
          /* commented the break to address the dead code, as this case is currently not supported 
             break; */
        }

#if 0
        case STATE_BS_FAILED:
        {
          LOG_ERROR("Response 503_SERVICE_UNAVAILABLE setting  for Bootstrap request as STATE_BS_FAILED ");

          /* Indicate the applications that the bootstrap procedure has failed */
		  lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_BOOTSTRAP_FAILED_E);
          return LWM2M_503_SERVICE_UNAVAILABLE;
        }
#endif
        default:
        {
          // keep on waiting
          LOG_DEBUG("default case, bootstrapping");
          bootstrap_step(contextP, tv_sec, timeoutP);
          break;
        }
      }
      
      break;
    }
#endif
    case STATE_REGISTER_REQUIRED:      
    {    

      lwm2m_create_active_server_list(contextP);
      lwm2m_sort_active_server_list(contextP);
      qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
      /* Registration process has started */        
      data.glwm2m_app_ctx.lwm2m_state = STATE_REGISTER_REQUIRED;        
      //qurt_signal_set(&data.glwm2m_app_ctx.lwm2m_app_global_signal, LWM2M_REGISTRATION_REQUIRED); 
      qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
      if(IsInExtObjCreateWindow() == true)
      {
        ds_iot_ulog_err("Timer is running to wait ExtObj create, no register action now");
        break;
      }	  
      result = registration_start(contextP, NULL,timeoutP);
      if (LWM2M_NO_ERROR != result) 
      {
        LOG_DEBUG("Response %u.%02u recieved for initial registration req, Retrying", (result & 0xFF) >> 5, (result & 0x1F));
      }
      contextP->state = STATE_REGISTERING;
      m2m_log_printf("State: %s", STR_STATE(contextP->state));
      ds_iot_log_strf_high ("State: %s ", STR_STATE(contextP->state));

      break;
    }

    case STATE_REGISTERING:
    {
      bool is_blocking = FALSE;
      /* Create the active server list from the lwm2m context server list.*/
      lwm2m_create_active_server_list(contextP);

      /* Sort the active server list .*/
      lwm2m_sort_active_server_list(contextP);
      switch (registration_getStatus(contextP, &is_blocking))
      {
        case STATE_REGISTERED:
        {
          contextP->state = STATE_READY;
          m2m_log_printf("State: %s", STR_STATE(contextP->state));
          ds_iot_log_strf_high ("State: %s ", STR_STATE(contextP->state));

          lwm2m_in_sleep_state = FALSE;

          qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
          data.glwm2m_app_ctx.lwm2m_state = STATE_READY;
          qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

          /* Indicate the applications that the LWM2M client has successfully completed registering with the server */
          lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_REGISTERTION_COMPELTED_E);
          break;
        }

        case STATE_REG_FAILED:
        {
          /* If the blocking server exhausted all the possible registration attempts
             then we will studown LwM2M */
          if(is_blocking == TRUE)
          {
            /* Indicate the applications that the LWM2M client has failed to register with the server */
            lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_BLOCKING_REGISTRATION_FAILURE_E);
            return LWM2M_503_SERVICE_UNAVAILABLE;
          }
          else
          {
            trigger_factory_reset_util(false, QAPI_NET_LWM2M_REGISTRATION_FAILED_E);
          }
          break;
        }

        case STATE_REG_PENDING:
        default:
          // keep on waiting
          break;
      }

      break;
    }
    
    case STATE_READY:
    {
      bool is_blocking = FALSE;
      /* Create the active server list from the lwm2m context server list.*/
      lwm2m_create_active_server_list(contextP);

      /* Sort the active server list .*/
      lwm2m_sort_active_server_list(contextP);
      /* Update the servers with the LwM2M versions they need to operate under */
      if (contextP->serverList)
      {
        serverP = contextP->serverList;
       
        while (serverP != NULL)
        {
          if (!serverP->version)
          {
            float version = 0.0;
      
            /* Default LwM2M version from the LwM2M config, gets overwritten if there is a persisted value for the server */
            serverP->version = data.cfg_info.lwm2m_version;           
              
            /* Retrieve the LwM2M version from the persistence file. In case, there is no entry associated with the server in the 
             * persistence file then use default LwM2M version in LwM2M config.
             */
            get_persisted_lwm2m_server_version(serverP->shortID, &version);
            if (!version)
              serverP->version = version;
          }
        
          serverP = serverP->next; 
        }
      }

      if (registration_getStatus(contextP, &is_blocking) == STATE_REG_FAILED)
      {
        /* If the blocking server exhausted all the possible registration attempts
           then we will studown LwM2M */
        if(is_blocking == TRUE)
        {
          /* Indicate the applications that the LWM2M client has failed to register with the server */
          lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_BLOCKING_REGISTRATION_FAILURE_E);
          return LWM2M_503_SERVICE_UNAVAILABLE;
        }
        else{
          trigger_factory_reset_util(false, QAPI_NET_LWM2M_REGISTRATION_FAILED_E);
        }
        break;
      }
	  
      break;
    }

    default:
      // do nothing
      break;
  }

#endif

  if(msisdn_buf!=NULL)
    lwm2m_free((void*)msisdn_buf);



  if(IsInExtObjCreateWindow() == true)
  {
    ds_iot_ulog_err("Timer is running to wait ExtObj create. Return.");
    return 0;
  }


  /* Registration step to be called before observe step. Refresh the server 
     status by visiting the registration step before visiting the observe step
     where we check server status. */
  if ((result = registration_step(contextP, tv_sec, timeoutP)) != 0x00)
    return result;

  tv_sec = lwm2m_gettime();     // Current time.
  observe_step(contextP, tv_sec, timeoutP); 
     
#ifdef PSM_LWM2M
  if (lwm2m_psm_enable == TRUE)
    psm_step(contextP, tv_sec, timeoutP); 
#endif

  ds_iot_ulog_high1("Exiting lwm2m_step with timeoutP: %d", *timeoutP);
#ifdef LWM2M_CLIENT_MODE
  LOG_DEBUG("Final state: %s", STR_STATE(contextP->state));
#endif
  return 0;
}


/**
 * @fn bool write_Tlv_to_file()
 * @brief This function is used to write a TagLengthValue to file
 *
 * @param[in] fd     file to write
 * @param[in] tag    TAG to write
 * @param[in] length length of data to write
 * @param[in] value  data to write
 *
 * @return    True   Write to file success
              False  Write to file failure 
 *        
 */
bool write_Tlv_to_file(int fd, uint16 tag, uint8 length, uint8* value)
{
  uint8_t *payload = NULL;
  uint16_t act_length = 0;
  uint16_t curr_ptr = 0;

  if (EFAILURE == fd || NULL == value)
  {
    LOG_DEBUG("%s",nullError);
	return LWM2M_FALSE;
  }

  act_length = sizeof(tag) + sizeof(length) + length;

  if(act_length != 0)
  {
    payload = lwm2m_malloc(act_length);
    if(payload == NULL)
	return LWM2M_FALSE;

    memset(payload, 0x00, act_length);
  }	

  memcpy(payload, &tag, sizeof(tag));
  curr_ptr += sizeof(tag);

  memcpy(payload + curr_ptr, &length, sizeof(length));
  curr_ptr += sizeof(length);

  memcpy(payload + curr_ptr, value, length);
  ds_iot_log_strf_med("EFS_OPT: File write %s",__FUNCTION__);
  if(EFAILURE == efs_write(fd, payload, act_length))
  {
    ds_iot_ulog_err("LWM2M_LOG: failed to store persistent value ");
    lwm2m_free(payload);
    return LWM2M_FALSE;
  }	

  lwm2m_free(payload);
  return LWM2M_TRUE;
}

/** 
 * @fn     read_Tlv_from_file()
 * @brief  This function is used to read resource value from persistence file
 * @param[in]  fd          - file descripter
   @param[out] tag         - tag
   @param[out] length	   - length of value
   @param[out] value	   - value
 * @return on 1    -  read success
           on 0    -  reach end of file
           on EFAILURE - read failure
*/ 
int read_Tlv_from_file(int fd, uint16* tag, uint8* length, uint8** value)
{
  int ret    = 0;

  if (EFAILURE == fd || NULL == tag || NULL == length)
  {
    LOG_DEBUG("LWM2M_LOG: NULL Error");
    return 0;
  }

  ret = efs_read(fd, tag, sizeof(uint16));
  if(0 == ret)
  { //reach end of file
    return 0;
  }
  else if(ret != sizeof(uint16))
  { //error happen
	ds_iot_ulog_err("failed to read persistent value of res/resource_id");
    return EFAILURE;
  }
    
  if (sizeof(uint8) != efs_read(fd, length, sizeof(uint8)))
  {
	ds_iot_ulog_err("failed to read persistent value of device res/length");
	return EFAILURE;
  }
  *value = lwm2m_malloc((*length) + 1);
  if(NULL == *value)
  {
	ds_iot_ulog_err("read_stringTlv_from_file/memory allocate failure");
	return EFAILURE;    
  }
  
  memset(*value, 0, (*length)+1);
  if (*length != efs_read(fd, *value, *length))
  {
    lwm2m_free(*value);
	*value = NULL;
	ds_iot_ulog_err("failed to read persistent value of device res/value");
	return EFAILURE;
  }	
  return 1;
}

/** 
 * @fn     storeRegisteringExtObj()
 * @brief  This function is used to buffer the extensible obj/ins that is registering;
 * @param[in] server   - lwm2m-server that is registering
   @param[in] objID    - extensible object-id
   @param[in] insID    - instance-id ( 0 in case of no instance list)
   @param[in] instance - instance list is present or not.
 * @return on 0    -  success
           on -1   -  failure
*/ 
int storeRegisteringExtObj(lwm2m_server_t* server, uint16_t objID, uint16_t insID, bool instance)
{
  lwm2m_app_obj_info_t *objInfo   = NULL;
  lwm2m_list_t         *insIdList = NULL;
  lwm2m_app_obj_info_t *matched_extobj_info = NULL; 
  
  if(server == NULL)
  {
    return -1;
  }
 
  insIdList = (lwm2m_list_t*)lwm2m_malloc(sizeof(lwm2m_list_t)); 
  if(insIdList == NULL)
  {   
	return -1; 
  }
  memset(insIdList, 0, sizeof(lwm2m_list_t));
  
  ds_iot_ulog_high2("storeRegisteringExtObj, /%d/%d", objID, insID);

  /* anytime that register do happen, stop the timer window
     this could happen when UE bootup and lifetime will expire, 
     so let registraion firstly to maintain the register session
  */
  StopExtObjCreateWindow();

  /* If instance list is present then store object and instance info else store 
     only object information */
  if(instance)
  {
    matched_extobj_info = (lwm2m_app_obj_info_t *)LWM2M_LIST_FIND(server->registering_extobj_info, objID);
    if(matched_extobj_info)
    {
      insIdList->id   = insID;
      matched_extobj_info->instance_list = (lwm2m_list_t *)LWM2M_LIST_ADD(matched_extobj_info->instance_list, insIdList);
    }
    else
    {
      objInfo = (lwm2m_app_obj_info_t*)lwm2m_malloc(sizeof(lwm2m_app_obj_info_t));
      if(objInfo == NULL)
      {
        lwm2m_free(insIdList);
        return -1;
      }
      memset(objInfo, 0, sizeof(lwm2m_app_obj_info_t));
      insIdList->id	= insID;
      objInfo->instance_list = (lwm2m_list_t *)LWM2M_LIST_ADD(objInfo->instance_list, insIdList);
      objInfo->object_ID = objID;
      server->registering_extobj_info = (lwm2m_app_obj_info_t*)LWM2M_LIST_ADD(server->registering_extobj_info,objInfo);
    }
  }
  else
  {
    matched_extobj_info = (lwm2m_app_obj_info_t *)LWM2M_LIST_FIND(server->registering_extobj_info, objID);
    if(matched_extobj_info)
    {
      lwm2m_free(insIdList);
    }
    else
    {
      objInfo = (lwm2m_app_obj_info_t*)lwm2m_malloc(sizeof(lwm2m_app_obj_info_t));
      if(objInfo == NULL)
      {
        lwm2m_free(insIdList);
        return -1;
      }
      memset(objInfo, 0, sizeof(lwm2m_app_obj_info_t));
      objInfo->object_ID = objID;
      server->registering_extobj_info = (lwm2m_app_obj_info_t*)LWM2M_LIST_ADD(server->registering_extobj_info,objInfo);
      lwm2m_free(insIdList);
    }
  }
  return 0;
}

/** 
 * @fn     storeRegisteredExtObj()
 * @brief  This function is used to store the extensible obj/ins that is registered;
 * @param[in] server            - lwm2m-server that is registering
   @param[in] bRegisterSuccess  - register is success or not
 * @return 
*/ 
void storeRegisteredExtObj(lwm2m_server_t* serverP, bool bRegisterSuccess)
{
  ds_iot_ulog_high1("storeRegisteredExtObj, registersuccess=%d", bRegisterSuccess);

  if(bRegisterSuccess)
  {
    if(serverP->registering_extobj_info != NULL)
    {
      /*register success, the registering-extobjinfo now change to registered-objinfo*/

	  /*cleanup old registered_extobj_info*/
      if(serverP->registered_extobj_info)
      {
   	    /*delete old registered extobj info*/
	    deleteExtObjInfoList(serverP->registered_extobj_info);
	  }
	  serverP->registered_extobj_info  = serverP->registering_extobj_info;	
	  serverP->registering_extobj_info = NULL;

	  persistRegisterdExtObj();
    }
	else
	{
	  ds_iot_ulog_high("storeRegisteredExtObj, register update without ExtObjects, no need persist extobj");	
	}
  }
  else
  {  
    /*register failure or will retry, clean the registering extobjinfo*/	  
    if(serverP->registering_extobj_info)
    {    
      deleteExtObjInfoList(serverP->registering_extobj_info);
	  serverP->registering_extobj_info = NULL;
	}

  }
  
}

/** 
 * @fn     deleteExtObjInfoList()
 * @brief  This function is utility function to delete the obj-info list of extensible object;
 * @param[in] obj_info     - obj-info that is to delelted, it is a list, and each item have also a list for instance-id, both need delete;

 * @return 
*/ 
void deleteExtObjInfoList(lwm2m_app_obj_info_t* obj_info)
{
  ds_iot_ulog_high("deleteExtObjInfoList");

  while(obj_info != NULL)
  {
    lwm2m_app_obj_info_t *next_obj  = obj_info->next;
	LWM2M_LIST_FREE(obj_info->instance_list);

	lwm2m_free(obj_info);
	obj_info = next_obj;
  }
}

/** 
 * @fn     cleanExtObjInfoList()
 * @brief  This function is to delete the all registered obj-info list of extensible object in the server ;
 * @param[in] serverP     - lwm2m-server that to be clean on registed ext-obj-info;
 * @return 
*/ 
void cleanExtObjInfoList(lwm2m_server_t* serverP)
{
  ds_iot_ulog_high("cleanExtObjInfoList");

  if(serverP == NULL || serverP->registered_extobj_info == NULL)
  	 return;
  
  deleteExtObjInfoList(serverP->registered_extobj_info);
  serverP->registered_extobj_info = NULL;
  persistRegisterdExtObj();	  
}

uint16_t registered_ext_obj_length()
{
  lwm2m_server_t* serverP = NULL;
  lwm2m_app_obj_info_t *objInfo = NULL;
  uint16_t total_length = 0;

  if(data.lwm2mH == NULL)
    return 0;
  
  serverP = data.lwm2mH->serverList;

  while(serverP != NULL)
  {
    total_length += 4;  // SSID (2 bytes) + Length of obj/obj_instances (2 bytes)

    objInfo = serverP->registered_extobj_info;

    while(objInfo != NULL)
    {
      uint8_t  no_insId = 0;
      lwm2m_list_t *insList = objInfo->instance_list;

      total_length += 3;  // Object id (2 bytes) + No of obj_instances (1 byte)
      while(insList)
      {
        no_insId++;
        insList = insList->next;
      }
      total_length += (2*no_insId);  // Object instance id(2 bytes * count)
      objInfo = objInfo->next;
    }
    serverP = serverP->next;
  }
  return total_length;
}


/** 
 * @fn     persistRegisterdExtObj()
 * @brief  This function is persis the registered extended obj/instance info file of all server;
 *         persist file use TLV value to store all objid/insid of each server, like below:
 *         <short-server-id-1><length>[<objid1><lengh><insid1><insid2>...<objid2><lengh><insid1><insid2>...]
 *	       <short-server-id-2><length>[<objid1><lengh><insid1><insid2>...<objid2><lengh><insid1><insid2>...]
 * @return on ESUCCESS   -  success
           on EFAILURE   -  failure
*/ 
int persistRegisterdExtObj()
{
  int fd = 0; 
  lwm2m_server_t* serverP = NULL; 
  lwm2m_app_obj_info_t *objInfo = NULL;
  uint16_t total_length = 0;
  uint8_t *payload = NULL;
  uint16_t curr_ptr = 0;

  if(data.lwm2mH == NULL)
  	return EFAILURE;
  
  total_length = registered_ext_obj_length();
  if(total_length == 0)
    return ESUCCESS;
  else
  {
    payload = lwm2m_malloc(total_length);
    if(payload == NULL)
    {
      ds_iot_ulog_err("Memory allocation failure.");
      return EFAILURE;
    }
    memset(payload, 0x00, total_length);
  }
  
  serverP = data.lwm2mH->serverList;

  ds_iot_ulog_high("persistRegisterdExtObj");
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_REGISTERED_EXTOBJ_PERSISTENCE_FILE);
  fd = efs_open(LWM2M_REGISTERED_EXTOBJ_PERSISTENCE_FILE, O_CREAT | O_TRUNC | O_WRONLY, 00644);
  if (EFAILURE == fd)
  {
	ds_iot_ulog_err("failed to open registered extend obj persistence file");
    goto write_file_failure;
  }

  while(serverP != NULL)
  {
    uint16_t length_ptr = 0;
    uint16_t total_length_of_server = 0;
    objInfo = serverP->registered_extobj_info;

    memcpy(payload, &serverP->shortID, sizeof(serverP->shortID));
    curr_ptr += sizeof(serverP->shortID);

    length_ptr = curr_ptr;
	/*Length: place holder of the length, will write back when got the length value later*/
    curr_ptr += sizeof(length_ptr);

	while(objInfo != NULL)
	{
	  uint8  no_insId = 0;
      uint16_t obj_ptr = 0;
      lwm2m_list_t *insList = objInfo->instance_list;
      ds_iot_ulog_err1("LWM2M_LOG: persistRegisterdExtObj objid=%d\n", objInfo->object_ID);
      ds_iot_ulog_err("LWM2M_LOG: persistRegisterdExtObj insid=:");

      /*Tag: object-id*/
      memcpy(payload + curr_ptr, &objInfo->object_ID, sizeof(objInfo->object_ID));
      curr_ptr += sizeof(objInfo->object_ID);
      total_length_of_server += sizeof(objInfo->object_ID);
			
      obj_ptr = curr_ptr;
      /*Length: place holder of the length, will write back when got the length value later*/
      curr_ptr += sizeof(uint8);
      total_length_of_server += sizeof(uint8);

      while(insList != NULL)
	  {
        ds_iot_ulog_err1("%d,", insList->id);
        /*Tag: object instance -id*/
        memcpy(payload + curr_ptr, &insList->id, sizeof(insList->id));
        curr_ptr += sizeof(insList->id);
        total_length_of_server += sizeof(insList->id);
	    no_insId++;
	    insList = insList->next;
	  }
      payload[obj_ptr] = 2 * no_insId;   // Update the actual instance count
      objInfo = objInfo->next;
	  }

    payload[length_ptr] = total_length_of_server; // Update the actual length of the server.
    serverP = serverP->next;
    }
  ds_iot_log_strf_med("EFS_OPT: File write %s",LWM2M_REGISTERED_EXTOBJ_PERSISTENCE_FILE);
  if(EFAILURE == efs_write(fd, payload, total_length))
	{
	  goto write_file_failure;
	}
  ds_iot_ulog_high1("EFS_opt: file_write %s",LWM2M_REGISTERED_EXTOBJ_PERSISTENCE_FILE);
  efs_close(fd);
  if(payload)
  {
    free(payload);
  }
  return ESUCCESS;

  write_file_failure:  	
  ds_iot_ulog_err("LWM2M_LOG: failed to store persistent value of RegsiterdExtObj");
  if(payload)
  {
    free(payload);
  }
	efs_close(fd);
	efs_unlink(LWM2M_REGISTERED_EXTOBJ_PERSISTENCE_FILE);
	return EFAILURE;
}

/** 
 * @fn     loadRegisterdExtObj()
 * @brief  This function is load the registered extended obj/instance info from persistent files;
 * @param[in] lwm2mH     - lwm2m-context, that used to get all lwM2M server to init registred ext-obj-info 
 * @return on ESUCCESS   -  success
           on EFAILURE   -  failures 
*/
int loadRegisterdExtObj(lwm2m_context_t *lwm2mH)
{
  int fd  = 0 ; 
  int res = 0;
  int i   = 0;
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_REGISTERED_EXTOBJ_PERSISTENCE_FILE);
  fd = efs_open(LWM2M_REGISTERED_EXTOBJ_PERSISTENCE_FILE, O_RDONLY);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open RegisterdExtObj persistence file ");
    return EFAILURE;
  }

  while(1)
  {
    lwm2m_server_t       *serverP = NULL;  
	lwm2m_app_obj_info_t *objInfo = NULL;	
	uint16 serverid = 0;
    uint16  data_length_of_current_server = 0;
	
	res = efs_read(fd, &serverid, sizeof(serverid));
	if(res == 0)
	{//end of file
	  ds_iot_ulog_err("loadRegisterdExtObj: reach end of file");
	  break;
	}
	ds_iot_ulog_err1("loadRegisterdExtObj : serverid=%d", serverid);

	/*read the length of all data belong to this server*/
	res = efs_read(fd, &data_length_of_current_server, sizeof(data_length_of_current_server));
	if(res != sizeof(data_length_of_current_server))
	{
	  goto read_file_error;	
	}

	/*loop to read each objid/insidlist,*/
	while(data_length_of_current_server > 0)
	{
      uint16  objid	  = 0;
	  uint8  length   = 0;
	  uint16 *value   = 0;

	  lwm2m_app_obj_info_t *objInfo_temp = (lwm2m_app_obj_info_t*)lwm2m_malloc(sizeof(lwm2m_app_obj_info_t));
	  
      if(objInfo_temp == NULL)
      {
		  goto read_value_error;
      }
      objInfo_temp->next = NULL;
	  objInfo_temp->instance_list = NULL;

	  /*read objid and insidlist*/
      res = read_Tlv_from_file(fd, &objid, &length, (uint8**)&value);	  
      if(res != 1)
      {
	  	goto read_value_error;
      }
      objInfo_temp->object_ID = (uint16_t)objid;
      ds_iot_ulog_err1("loadRegisterdExtObj : objid=%d", objInfo_temp->object_ID);
	
      ds_iot_ulog_err("insid=:");
      for(i=0; i<length/2; i++)
      {
        lwm2m_list_t *insList = (lwm2m_list_t*)lwm2m_malloc(sizeof(lwm2m_list_t));

		if(insList == NULL)
        {
          lwm2m_free(value);
          value = NULL;
          goto read_value_error;
		}
        memset(insList, 0, sizeof(lwm2m_list_t));		
        insList->id   = value[i];

        ds_iot_ulog_err1("%d,", value[i]);
		objInfo_temp->instance_list = (lwm2m_list_t*)LWM2M_LIST_ADD(objInfo_temp->instance_list, insList);
      }
	  
	  lwm2m_free(value);
	  value = NULL;

      //insert obj into objinfo list
	  objInfo = (lwm2m_app_obj_info_t *)LWM2M_LIST_ADD(objInfo,objInfo_temp);

      data_length_of_current_server -= 2 + 1 + length; /*Tag(16)+Length(8)+value*/
	  continue;

	  read_value_error:
	  	deleteExtObjInfoList(objInfo_temp);
		objInfo_temp = NULL;		
		deleteExtObjInfoList(objInfo);
		objInfo = NULL;
		goto read_file_error;
    }

    /*find out the server*/
     serverP = lwm2mH->serverList; 
     while(serverP != NULL)
     {
       if(serverP->shortID == serverid )
       {
         if(serverP->status == STATE_REGISTERED)
         {
		   serverP->registered_extobj_info = objInfo;
		   objInfo = NULL;
         }
		 else
		 {
		   ds_iot_ulog_err1("server %d is not registered, don't recover registered_extobj_info", serverid);		 
		 }
         break;
       }
       serverP = serverP->next;	 
     }
     if(objInfo != NULL)	 
     { 
       /*objinfo was not assigned to any server*/
       deleteExtObjInfoList(objInfo);
     }	 
	 objInfo = NULL;
  }

  efs_close(fd);
  return ESUCCESS;

  read_file_error:
	efs_close(fd);
	efs_unlink(LWM2M_REGISTERED_EXTOBJ_PERSISTENCE_FILE);
  	return EFAILURE;
	 
}

/** 
 * @fn     StartExtObjCreateWindow()
 * @brief  This function is to start the window for waiting application create extensible obj/instance if need;
 *         if UE bootup with bootstrap status was persistented, then need start the window to wait extobj create;
 *         (if UE bootup with bootstrap not persistened, lwm2m will start the max_bootstrap_wait timer otherwhere)
 * @param[in] lwm2mH     - lwm2m-context, that used to get all lwM2M server to check whether need start the ExtObjCreate Window 
 * @return 
*/
void StartExtObjCreateWindow(lwm2m_context_t *lwm2mH)
{
  lwm2m_server_t* serverP = NULL;
  bool bTimerNeedStartForRegister = LWM2M_TRUE;

  if(lwm2mH == NULL)
  {
  	ds_iot_ulog_high("LWM2M_LOG: NULL Error,StartExtObjCreateWindow");  
  	return;
  }

  /* if need force reg update on reconnect, 
   * then we don't need start the window since reg-update is mandatary
   */
  if(data.cfg_info.reg_update_on_reconnect == true)
  {
    return;
  }

  /* if UE don't need do the BootStrap, then need start timer here to wait Ext-Obj create for  register;
     if UE need do the BootStrap, then lwm2m will request start timer otherwhere(lwm2m_step)
   */
  if (lwm2mH->bootstrapServer != NULL) 
  {
    serverP = lwm2mH->bootstrapServer;
    //any bootstrap server are in not-registered state, then bootstrap part will start timer for wait
    if (serverP)
    { 
      if (serverP->isBootstrapped == LWM2M_FALSE) 
      {
        bTimerNeedStartForRegister  = LWM2M_FALSE ;
      }
    }
  } 

  
  if(bTimerNeedStartForRegister == LWM2M_TRUE)
  {
	timer_error_type timer_result = TE_FAIL; 
	  
    timer_result = timer_def_osal(&extObjCreateWindowTimer, &lwm2mTimerGroup, 
		  TIMER_FUNC1_CB_TYPE,extObjCreateWindowTimercb, NULL);
	if (timer_result == TE_SUCCESS)
    {
      //reuse the timer value setting for bootstrap wait
   	  timer_set(&extObjCreateWindowTimer , data.cfg_info.max_bootstrap_wait, 0, T_SEC);
  	  ds_iot_ulog_high("StartExtObjCreateWindow");
	}
	else
	{
	  ds_iot_ulog_high("StartExtObjCreateWindow failure");
	}
  }	
  else
  {
	ds_iot_ulog_high("StartExtObjCreateWindow don't start since same timer will run during bootstrap");
  }
  
}

/** 
 * @fn     IsInExtObjCreateWindow()
 * @brief  This function is to return whether the ExtObjCreate wait timer is running
 * 
 * @return 
*/
bool IsInExtObjCreateWindow(void)
{
  bool ret = timer_is_active(&extObjCreateWindowTimer);
  ds_iot_ulog_high1("IsInExtObjCreateWindow, %d", ret);
  return ret;
}

/** 
 * @fn     IsInExtObjCreateWindow()
 * @brief  This function is to stop the ExtObjCreate wait timer if it is running
 * 
 * @return 
*/
void StopExtObjCreateWindow(void)
{
  if(timer_is_active(&extObjCreateWindowTimer))
  {
    ds_iot_ulog_high("StopExtObjCreateWindow");  
    timer_clr(&extObjCreateWindowTimer, T_SEC);
  }
}

#define MAX_INSTANCE_NO 50  /*maximum number of instance in each object*/
#define MAX_OBJECT_NO   50  /*maximum number of object*/


/** 
 * @fn     extObjRegUpdateCheck()
 * @brief  This function is to trigger register update on the server if registered-ext-obj-info is different with app-ext-obj-info
 *
 * @param[in] serverP 	        - lwm2m server
 * @return 
*/
void extObjRegUpdateCheck(lwm2m_server_t* serverP)
{
  lwm2m_app_obj_info_t *matched_extobj_info = NULL; 
  lwm2m_list_t * matched_inst_info = NULL;
  lwm2m_app_obj_info_t *registered_extobj_info = NULL; 	
  lwm2m_handle_info_t  *appHandle = data.glwm2m_app_ctx.handles;	
  uint8 app_obj_info_count = 0;
  
  if(serverP == NULL)
    return;
  registered_extobj_info = serverP->registered_extobj_info;
  ds_iot_ulog_high1 ("extObjRegUpdateCheck in with server id %d", serverP->shortID);
  while(appHandle != NULL)
  {
	lwm2m_app_obj_info_t * app_obj_info = appHandle->obj_info;
	while((app_obj_info != NULL))
	{
	  if(!lwm2m_is_standard_object(app_obj_info->object_ID))
	  {
	  	app_obj_info_count++;
		ds_iot_ulog_high1 ("extObjRegUpdateCheck, find registered_extobj_info for obj %d", app_obj_info->object_ID);
	    matched_extobj_info = (lwm2m_app_obj_info_t *)LWM2M_LIST_FIND(registered_extobj_info, app_obj_info->object_ID);
	    if(matched_extobj_info)
	    {
		  lwm2m_list_t * app_ins_list = app_obj_info->instance_list;
		  uint8 app_ins_list_count = 0;
		  while(app_ins_list != NULL)
		  {
		    app_ins_list_count++;
			ds_iot_ulog_high1 ("extObjRegUpdateCheck, find registered_extobj_info for ins %d",  app_ins_list->id);
			matched_inst_info = (lwm2m_list_t *)LWM2M_LIST_FIND(matched_extobj_info->instance_list, app_ins_list->id);
			if(matched_inst_info)
			{
              ds_iot_ulog_high2 ("extObjRegUpdateCheck, current extobj id: %d, instance id: %d, matched in regsitered_extobj_list.", app_obj_info->object_ID,app_ins_list->id);
			}
		    else
			{
			  ds_iot_ulog_high2 ("extObjRegUpdateCheck, new instance find, obj id %d, ins id %d",  app_obj_info->object_ID,app_ins_list->id);
              goto register_update_need;				
			}
			app_ins_list = app_ins_list->next;
		  }
		  if(app_ins_list_count != LWM2M_LIST_COUNT(matched_extobj_info->instance_list))
		  {
		    ds_iot_ulog_high1 ("extObjRegUpdateCheck, instance count not match for object %d",  app_obj_info->object_ID);
            goto register_update_need;			    
		  }
	    }
        else
	    {			  
	      ds_iot_ulog_high1 ("extObjRegUpdateCheck, new obj find, obj id %d", app_obj_info->object_ID);
          goto register_update_need;	  
	    }
	  }
	  app_obj_info = app_obj_info->next;
	} 
    appHandle = appHandle->next;	
  }
  if(app_obj_info_count != LWM2M_LIST_COUNT(registered_extobj_info))
  {
	ds_iot_ulog_high ("extObjRegUpdateCheck, object count not match");
    goto register_update_need;  
  }
  ds_iot_ulog_high ("extObjRegUpdateCheck, No need trigger Register Update since No ExtObj chagne");
  return;

register_update_need:
  //clean the registered-obj-info before register update
  cleanExtObjInfoList(serverP);
  
  /* Perform registration updates as part of the extended object creation */ 
  ds_iot_ulog_high ("extObjRegUpdateCheck, trigger Register Update");
  lwm2m_update_registration(data.lwm2mH, serverP->shortID, TRUE);	  	  
}

/**
 * @fn     void extObjCreateWindowTimercb
 * @brief  This function is timer callback function, 
            to trigger register update on any lwm2mserver that have different registered-ext-obj-info with app-ext-obj-info
 * @param  userData - lwm2m context passed by timer 
 * @return void
 */
void extObjCreateWindowTimercb(  int32 time_ms, timer_cb_data_type userData )
{
  lwm2m_server_t* serverP = NULL; 

  ds_iot_ulog_high ("extObjCreateWindowTimercb");

  qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex); 

  if(data.lwm2mH != NULL)
  {
    serverP = data.lwm2mH->serverList;
    while(serverP != NULL)
    {
      if(serverP->status == STATE_REGISTERED)
      {
        extObjRegUpdateCheck(serverP);
      }
	  else
	  {
	    qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
	  }
	  serverP = serverP->next;
    }
  }
  else
  {
	ds_iot_ulog_err ("extObjCreateWindowTimercb is called while context is not initilized");
  }

  qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
  
}

int prv_init_getvalue
(
 lwm2m_id_info_t id_info,
 lwm2m_object_data_t *lwm2m_dataP,
 lwm2m_resource_info_t **resource_info,
 lwm2m_data_t ** dataArrayP,
 int* numData)
{
  int result =-1;

  if (lwm2m_dataP == NULL || resource_info == NULL || dataArrayP == NULL || numData == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return result;
  }

  if (id_info.id_set & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    ds_iot_ulog_high ("RESOURCE REQUESTED ");

    *numData = 1;
    if(lwm2m_dataP->instance_info == NULL)
      return result;

#if 0
    *resource_info = lwm2m_dataP->instance_info->resource_info;
    if (*resource_info == NULL)
    {
      LOG_ERROR ("%s FOR RESOURCE",nullError);
      return result;
    }
#endif
    *dataArrayP = lwm2m_data_new(*numData);
    if (*dataArrayP == NULL) 
      return result;

    //(*dataArrayP)->id = (*resource_info)->resource_ID;
    (*dataArrayP)->id = id_info.resource_ID;
  }
  else if (id_info.id_set & QAPI_NET_LWM2M_INSTANCE_ID_E)
  {
    ds_iot_ulog_high ("INSTANCE IS REQUESTED ");

    *numData = 0;

    if (lwm2m_dataP->instance_info == NULL)
    {
      ds_iot_ulog_err ("NULL Error FOR INSTANCE ");
      return result;    
    }
#if 0
    *resource_info = (lwm2m_resource_info_t *)lwm2m_malloc(sizeof (lwm2m_resource_info_t)); 
    if (*resource_info == NULL)
    {
      LOG_ERROR ("%s",mallocError);
      return result;
    }

    memset(*resource_info, 0x00, sizeof(lwm2m_resource_info_t));
    lwm2m_dataP->instance_info->resource_info = *resource_info; 
#endif
  }
  else
  {
    ds_iot_ulog_err ("OBJECT INFO IS REQUESTED ");
    return result;
  }
  lwm2m_dataP->instance_info->resource_info = *resource_info; 
  return 0;
}

int prv_getvalue_resource_populate
(
  lwm2m_object_data_t **lwm2m_data,
  lwm2m_data_t * dataArrayP,
  lwm2m_resource_info_t **resource_info,
  int numData
)
{
  int i=0;
  if(lwm2m_data == NULL || *lwm2m_data == NULL || (*lwm2m_data)->instance_info == NULL || resource_info == NULL ||dataArrayP == NULL || numData == 0)
    return -1;

  *resource_info= (*lwm2m_data)->instance_info->resource_info;
  (*lwm2m_data)->instance_info->no_resources = numData;

  for (i = 0; i < numData; i++)
  {
    lwm2m_resource_info_t *resource_infoP = NULL;
    resource_infoP = (lwm2m_resource_info_t *)lwm2m_malloc(sizeof(lwm2m_resource_info_t));
    if(resource_infoP == NULL)
    {
      ds_iot_ulog_high ("LWM2M_LOG: Memory Allocation FAILED");
      LWM2M_LIST_FREE(*resource_info);
      (*lwm2m_data)->instance_info->resource_info = NULL;
      return -1;
    }
    memset(resource_infoP, 0x00, sizeof(lwm2m_resource_info_t));

    resource_infoP->resource_ID = dataArrayP->id;
    resource_infoP->type = (lwm2m_value_type_t)dataArrayP->type;
    switch (dataArrayP->type)
    {
      case LWM2M_TYPE_STRING:
        resource_infoP->value.asBuffer.buffer = dataArrayP->value.asBuffer.buffer;
        resource_infoP->value.asBuffer.length = dataArrayP->value.asBuffer.length;
        break;
      case LWM2M_TYPE_INTEGER:
        resource_infoP->value.asInteger = dataArrayP->value.asInteger;
        break;
      case LWM2M_TYPE_BOOLEAN:
        resource_infoP->value.asBoolean = dataArrayP->value.asBoolean;
        break;
      case LWM2M_TYPE_OBJECT_LINK:
        resource_infoP->value.asObjLink = dataArrayP->value.asObjLink;
        break;
      case LWM2M_TYPE_MULTIPLE_RESOURCE:
        resource_infoP->value.asChildren.count = dataArrayP->value.asChildren.count;
        resource_infoP->value.asChildren.array = (qapi_Net_LWM2M_Flat_Data_t *)dataArrayP->value.asChildren.array;
        break;
      default:
        break;
    }

    (*lwm2m_data)->instance_info->resource_info = (lwm2m_resource_info_t *)
                                                   LWM2M_LIST_ADD((*lwm2m_data)->instance_info->resource_info, resource_infoP);
    dataArrayP = dataArrayP + 1;
  }

  return 0;
}

uint8_t prv_init_resources
(
  lwm2m_object_data_t* lwm2m_data,
  uint8_t* id_set,
  uint16_t object_id,
  uint16_t instanceId,
  uint16_t resource_id
)
{
  if(lwm2m_data ==NULL || id_set == NULL)
    return LWM2M_400_BAD_REQUEST;

  memset(lwm2m_data, 0, sizeof( lwm2m_object_data_t));
  *id_set = 0;
  *id_set = (*id_set | QAPI_NET_LWM2M_OBJECT_ID_E) | (*id_set | QAPI_NET_LWM2M_INSTANCE_ID_E) | (*id_set | QAPI_NET_LWM2M_RESOURCE_ID_E);
  
  lwm2m_data->object_ID = object_id;
  lwm2m_data->no_instances++;
  lwm2m_data->instance_info = (lwm2m_instance_info_t *)lwm2m_malloc(sizeof(lwm2m_instance_info_t));
  if(NULL != lwm2m_data->instance_info)
  {
    memset(lwm2m_data->instance_info, 0, sizeof(lwm2m_instance_info_t)); 
    lwm2m_data->instance_info->instance_ID = instanceId;
    lwm2m_data->instance_info->no_resources++;
    lwm2m_data->instance_info->resource_info = (lwm2m_resource_info_t *)lwm2m_malloc(sizeof(lwm2m_resource_info_t));
    if(NULL != lwm2m_data->instance_info->resource_info)
    {
      memset(lwm2m_data->instance_info->resource_info, 0, sizeof(lwm2m_resource_info_t));	  
      lwm2m_data->instance_info->resource_info->resource_ID = resource_id;
      lwm2m_data->instance_info->resource_info->next = NULL;
    } 
    else
    {
      ds_iot_ulog_err ("LWM2M_LOG: Memory Allocation FAILED");
      lwm2m_free_instance_info(lwm2m_data->instance_info);
      return LWM2M_500_INTERNAL_SERVER_ERROR;
    }
  }
  else
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  return 0;
}
uint8_t prv_write_init_resources
(
	lwm2m_object_data_t* lwm2m_data,
	uint8_t* id_set,
	lwm2m_data_t * data,
	int object_id,
	int instanceId,
	lwm2m_value_type_t data_type
)
{
	int result = LWM2M_400_BAD_REQUEST;
	if(data == NULL || (0 != (result = prv_init_resources(lwm2m_data, id_set, object_id, instanceId, data->id))))
		return result;

	if(lwm2m_data->instance_info == NULL || lwm2m_data->instance_info->resource_info == NULL)
		return result;
	
	switch(data_type){
		case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
			lwm2m_data->instance_info->resource_info->type = QAPI_NET_LWM2M_TYPE_BOOLEAN_E;
			if(1 != lwm2m_data_decode_bool(data, &lwm2m_data->instance_info->resource_info->value.asBoolean))
	        {
			    result = LWM2M_400_BAD_REQUEST;
				goto MALLOC_ERROR;
	        }
		break;
		
		case QAPI_NET_LWM2M_TYPE_STRING_E :
		case QAPI_NET_LWM2M_TYPE_OPAQUE_E :
			lwm2m_data->instance_info->resource_info->type = (lwm2m_value_type_t)data->type;
			lwm2m_data->instance_info->resource_info->value.asBuffer.length	= data->value.asBuffer.length;
			if (lwm2m_data->instance_info->resource_info->value.asBuffer.length )
			{
			  lwm2m_data->instance_info->resource_info->value.asBuffer.buffer = lwm2m_malloc (lwm2m_data->instance_info->resource_info->value.asBuffer.length);
			  if (lwm2m_data->instance_info->resource_info->value.asBuffer.buffer == NULL )
			  {
				result= LWM2M_500_INTERNAL_SERVER_ERROR;
				goto MALLOC_ERROR;

			  }
			  memset(lwm2m_data->instance_info->resource_info->value.asBuffer.buffer, 0x00, lwm2m_data->instance_info->resource_info->value.asBuffer.length);
			  memscpy (lwm2m_data->instance_info->resource_info->value.asBuffer.buffer,lwm2m_data->instance_info->resource_info->value.asBuffer.length, \
				  data->value.asBuffer.buffer ,lwm2m_data->instance_info->resource_info->value.asBuffer.length);				  
			}
			break;
		case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
			if(1 != lwm2m_data_decode_objLink(data, &lwm2m_data->instance_info->resource_info->value.asObjLink))
	        {
			    result = LWM2M_400_BAD_REQUEST;
				goto MALLOC_ERROR;
	        }
			break;
		case QAPI_NET_LWM2M_TYPE_INTEGER_E:
			if(1 != lwm2m_data_decode_int(data, &lwm2m_data->instance_info->resource_info->value.asInteger))
	        {
			    result = LWM2M_400_BAD_REQUEST;
				goto MALLOC_ERROR;
	        }
			break;
		default :
			return LWM2M_501_NOT_IMPLEMENTED;
	}

	return 0;

	MALLOC_ERROR:
		lwm2m_free_instance_info(lwm2m_data->instance_info);
		return result;
		
}

qapi_Status_t lwm2m_parse_application_object_info 
(
  qapi_Net_LWM2M_Instance_Info_v2_t *instance_info,
  qapi_Net_LWM2M_Resource_Info_t *resource_info,
  lwm2m_data_t **parsed_data
)
{
  int32_t result = QAPI_ERROR;
  lwm2m_data_t *data_array = NULL;
  lwm2m_data_t *data_arrayP = NULL;
  uint32_t res_cnt = 0;

  /* Validate the input parameters */ 
  if ((!instance_info) ||
      ((instance_info->no_resources) &&
       (!resource_info)) ||
       (!parsed_data))
  {
    ds_iot_ulog_err("Invalid input parameters - lwm2m_data, instance_info");
    return QAPI_ERROR; 
  }

  /* Resources were not configured as part of the object nothing to parse */ 
  if (!instance_info->no_resources)
  {
    *parsed_data = NULL;
     return QAPI_OK;
  }

  /* Allocate resources for the resource information */
  data_array    = lwm2m_malloc(sizeof(lwm2m_data_t) * instance_info->no_resources);
  if(data_array == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED Res info");
    return QAPI_ERR_NO_MEMORY;
  }
  memset(data_array, 0x00, sizeof(lwm2m_data_t) * instance_info->no_resources);

  data_arrayP = data_array;
  while ((resource_info) && (res_cnt++ < instance_info->no_resources))
  {
    data_array->type = (lwm2m_data_type_t)resource_info->type; 
    data_array->id = resource_info->resource_ID;  

    switch (resource_info->type)
    {
      case QAPI_NET_LWM2M_TYPE_STRING_E:
      case QAPI_NET_LWM2M_TYPE_OPAQUE_E:
      {
        size_t length = 0;
        data_array->value.asBuffer.length = resource_info->value.asBuffer.length;
        length = resource_info->value.asBuffer.length;
        data_array->value.asBuffer.buffer = NULL;

        if (length)
        {
          data_array->value.asBuffer.buffer = (uint8_t *)lwm2m_malloc(length);
          if (!data_array->value.asBuffer.buffer)
          {
            /* Release the associated resources */
            ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED Res info"); 
            result = QAPI_ERR_NO_MEMORY; 
            goto PARSE_ERROR;
          }
          memscpy(data_array->value.asBuffer.buffer, length, resource_info->value.asBuffer.buffer, length); 
        }
        break;
      }

      case QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE:
      {
        qapi_Net_LWM2M_Flat_Data_t  *in_array  = resource_info->value.asChildren.array;
        uint32_t  in_array_size                = resource_info->value.asChildren.count;
        qapi_Net_LWM2M_Flat_Data_t **out_array = &data_array->value.asChildren.array;
        uint32_t *out_array_size               = (uint32_t *)&data_array->value.asChildren.count;

        *out_array = NULL;
        *out_array_size = 0x00;
        if(QAPI_OK != lwm2m_flat_data_copy_to_uspace((size_t)in_array_size, in_array, out_array))
        {
          result = QAPI_ERR_NO_MEMORY;
          goto PARSE_ERROR;
        }
        *out_array_size = in_array_size;
        break;
      }

      case QAPI_NET_LWM2M_TYPE_INTEGER_E:
      {
        data_array->value.asInteger = resource_info->value.asInteger ;
        break;
      }

      case QAPI_NET_LWM2M_TYPE_FLOAT_E:
      {
        data_array->value.asFloat = resource_info->value.asFloat ;
        break;
      }

      case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
      {
        data_array->value.asBoolean = resource_info->value.asBoolean;
        break;
      }

      case QAPI_NET_LWM2M_TYPE_OBJECT_LINK:
      {
        data_array->value.asObjLink = resource_info->value.asObjLink;
        break;
      }

      default:
      {
        ds_iot_ulog_err("Unsupported object resource type");
        result = QAPI_ERR_NOT_SUPPORTED; 

        goto PARSE_ERROR;
      }
    }

    data_array = data_array + 1;
    resource_info = resource_info->next;
  }

 *parsed_data = data_arrayP;  
  return QAPI_OK;

PARSE_ERROR:
  /* Perform resource clean-up */
  lwm2m_data_free_ext(instance_info->no_resources, data_arrayP);
  return result;
}
int get_msisdn_from_persistence(char* msisdn_buf){
	int read_bytes = 0;
	int cfg_fd = 0;
	char* msisdn_buf_pr = NULL;
	
	if(msisdn_buf == NULL)
		goto end;
    ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_CONFIG_PERSISTENCE_FILE);
	msisdn_buf_pr = process_cfg_utility_open_file(LWM2M_CONFIG_PERSISTENCE_FILE, &read_bytes, &cfg_fd);
	if (cfg_fd >= 0)
    {
      efs_close(cfg_fd);
      cfg_fd = -1;
    }
	if(msisdn_buf_pr == NULL)
		goto end;
	strlcpy(msisdn_buf, msisdn_buf_pr, read_bytes+1);
	lwm2m_free(msisdn_buf_pr);
	return ESUCCESS;

	end:
		ds_iot_ulog_err("Failed to read MSISDN from persistence");
		return EFAILURE;
}

int store_msisdn_to_persistence(char* msisdn_buf){
	int fd;
	char msisdn_buf_pr[MAX_MSISDN_LEN+2]= {0};
	int buff_len =0;
	
	if(msisdn_buf == NULL)
		goto end;
	
	buff_len = strlen(msisdn_buf);

	if (buff_len > MAX_MSISDN_LEN)
		goto end;
	ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_CONFIG_PERSISTENCE_FILE);
	fd = efs_open(LWM2M_CONFIG_PERSISTENCE_FILE, O_CREAT | O_RDWR, 00644);
	
	if (EFAILURE == fd)
		goto end;

	strlcpy(msisdn_buf_pr, msisdn_buf, buff_len+1);
	msisdn_buf_pr[buff_len] = '\0';
	ds_iot_log_strf_med("EFS_OPT: File write %s",LWM2M_CONFIG_PERSISTENCE_FILE);
	if (EFAILURE == efs_write(fd, msisdn_buf_pr, (buff_len +2)))
	{
		efs_close(fd);
		goto end;
	}
	efs_close(fd);
	return ESUCCESS;

	end:
		ds_iot_ulog_err("Failed to write MSISDN to persistence");
		return EFAILURE;
}

bool lwm2m_is_standard_object(uint16_t object_id)
{
  if(((int32_t)object_id >= 0x00 && (object_id <= 0x07)) ||
      (QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E == object_id) || 
      (QAPI_NET_LWM2M_CELL_NW_CONN_OBJECT_ID_E == object_id) || 
      (QAPI_NET_LWM2M_APN_CONN_OBJECT_ID_E == object_id) || 
      (QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E == object_id))
  return TRUE;

  else
    return FALSE;
}

/**
 * @fn get_blockSize()
 * @brief This function is used to get block size based on MTU and configured value
 *
 * @serverP[IN]: server list pointer
 *
 * @return blocksize
 */

uint16_t get_blockSize(lwm2m_server_t * serverP)
{
    uint16_t blocksize = data.cfg_info.blockSize;
    if (serverP && serverP->mtu_size >= 16 )
    {
       while (blocksize > serverP->mtu_size)
       {
          blocksize >>= 1; 
       }
    }
    return blocksize;
    
}



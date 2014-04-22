/******************************************************************************

  @file    observe.c

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
 *    Toby Jaffey - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
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
#include <stdio.h>
#include "lwm2m_signals.h"
#include "ds_log.h"


#define DEV_CAP_OBJECT_ID 15
#define RES_O_ATTACHED    3
#define RES_M_ENABLED   4 
#define PROCESSING_DELAY 0

extern uint32_t active_time;
extern bool lwm2m_in_sleep_state;
extern bool persis_regstatus_feat_enabled;
extern qurt_signal_t lwm2m_signal_Handle;
extern uint32_t trackingSessionId;
extern bool lwm2m_psm_enable;  // LWM2M PSM is enabled or not.

#define ATTR_FLAG_LT_GT (LWM2M_ATTR_FLAG_GREATER_THAN | LWM2M_ATTR_FLAG_LESS_THAN)

uint32_t get_pmin_value(lwm2m_context_t * contextP, lwm2m_watcher_t * watcherP,
                               lwm2m_uri_t uri)
{
  lwm2m_watcher_t *watcher = NULL;
  uint32_t min_period = 0;

  if(contextP == NULL || watcherP == NULL)
    return 0;

  if(watcherP->parameters == NULL || (watcherP->parameters != NULL 
      && (watcherP->parameters->toSet & LWM2M_ATTR_FLAG_MIN_PERIOD) == 0))
  {
    watcher = prv_findInheritedWatcher(contextP, &uri, watcherP->server, LWM2M_ATTR_FLAG_MIN_PERIOD);
    if(watcher == NULL)
    {
      watcher = watcherP;
    }
  }
  else
  {
    watcher = watcherP;
  }

  if(watcher && watcher->parameters != NULL 
      && ((watcher->parameters->toSet & LWM2M_ATTR_FLAG_MIN_PERIOD) != 0))
  {
    min_period = watcher->parameters->minPeriod;
  }
  else
  {
    min_period = watcher->server->defaultMinPeriod;
  }
  return min_period;
}

uint32_t get_pmax_value(lwm2m_context_t * contextP, lwm2m_watcher_t * watcherP,
                               lwm2m_uri_t uri)
{
  lwm2m_watcher_t *watcher = NULL;
  uint32_t max_period = 0;

  if(contextP == NULL || watcherP == NULL)
    return 0;

  if(watcherP->parameters == NULL || (watcherP->parameters != NULL 
      && (watcherP->parameters->toSet & LWM2M_ATTR_FLAG_MAX_PERIOD) == 0))
  {
    watcher = prv_findInheritedWatcher(contextP, &uri, watcherP->server, LWM2M_ATTR_FLAG_MAX_PERIOD);
    if(watcher == NULL)
    {
      watcher = watcherP;
    }
  }
  else
  {
    watcher = watcherP;
  }
  if(watcher && watcher->parameters != NULL 
      && ((watcher->parameters->toSet & LWM2M_ATTR_FLAG_MAX_PERIOD) != 0))
  {
    max_period = watcher->parameters->maxPeriod;
  }
  else
  {
    max_period = watcher->server->defaultMaxPeriod;
  }
  return max_period;
}

extern bool lwm2m_coap_sessionhandle_is_equal(void * handle1,
    void * handle2,
    void * userData);


#ifdef LWM2M_CLIENT_MODE

void update_timeout_val_with_period(lwm2m_watcher_t *watcherP, time_t *timeout,
                                                   time_t currentTime, uint32_t period)
{
  //time_t timeout = 0;
  if(watcherP->lastTime + period <= currentTime)
  {
    *timeout = 0;
    ds_iot_ulog_high1("Updating timeout to: %d", *timeout);
  } 
  else if (*timeout > ( watcherP->lastTime + period - currentTime ))
  {
    ds_iot_ulog_high4("timeout = %u watcherP->lastTime = %u period = %u currenTime = %u", *timeout,watcherP->lastTime,period, currentTime );
    *timeout = (watcherP->lastTime + period- currentTime);
    ds_iot_ulog_high1("timeout > interval Updating timeout to : %d", *timeout);
  }
}

/**
 * @fn static lwm2m_observed_t * prv_findObserved()
 * @brief This function is used to find a object in the observed list
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to uri
 *
 * @return    pointer to the observed node in the observed list on success
 *        NULL on failure
 */
static lwm2m_observed_t * prv_findObserved(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP)
{
  lwm2m_observed_t * targetP = NULL;
  if( contextP == NULL || uriP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return NULL;
  }

  targetP = contextP->observedList;
  while (targetP != NULL
      && (targetP->uri.objectId != uriP->objectId
        || targetP->uri.flag != uriP->flag
        || (LWM2M_URI_IS_SET_INSTANCE(uriP) && targetP->uri.instanceId != uriP->instanceId)
        || (LWM2M_URI_IS_SET_RESOURCE(uriP) && targetP->uri.resourceId != uriP->resourceId)))
  {
    targetP = targetP->next;
  }

  if(targetP != NULL)
  {
    ds_iot_ulog_high1("found a matching observedP for objID: %d",targetP->uri.objectId);
	LOG_DEBUG("%p",targetP);// Debug log for address.
    if(LWM2M_URI_IS_SET_INSTANCE(uriP))
    ds_iot_ulog_high1("And InstanceID: %d",targetP->uri.instanceId);
    if(LWM2M_URI_IS_SET_RESOURCE(uriP))
    ds_iot_ulog_high1("And ResourceID: %d",targetP->uri.resourceId);
  }

  return targetP;
}

/**
 * @fn static void prv_unlinkObserved()
 * @brief This function is used to remove node from observed list
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param observedP pointer to observed list
 *
 * @return    void
 *        
 */
 #if 0
static void prv_unlinkObserved(lwm2m_context_t * contextP,
    lwm2m_observed_t * observedP)
{
  if( contextP == NULL || observedP == NULL ) 
  {
    LOG_ERROR("fun [%s] file [%s]  %s",__func__,__FILE__,nullError);
    return ;
  } 
  if (contextP->observedList == observedP)
  {
    contextP->observedList = contextP->observedList->next;
  }
  else
  {
    lwm2m_observed_t * parentP = NULL;

    parentP = contextP->observedList;
    while (parentP->next != NULL
        && parentP->next != observedP)
    {
      parentP = parentP->next;
    }
    if (parentP->next != NULL)
    {
      parentP->next = parentP->next->next;
    }
  }
}
#endif

/**
 * @fn static lwm2m_watcher_t * prv_findWatcher()
 * @brief This function is used to find watcher node in watcher list
 *
 * @param observedP pointer to observed list
 * @param serverP   pointer to server to be searched
 *
 * @return    pointer to watcher node on success
 *        NULL on failure
 */
static lwm2m_watcher_t * prv_findWatcher(lwm2m_observed_t * observedP,
    lwm2m_server_t * serverP)
{
  lwm2m_watcher_t * targetP = NULL;
  if(observedP == NULL || serverP == NULL ) 
  { 
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return NULL;
  }

  targetP = observedP->watcherList;
  while (targetP != NULL
      && targetP->server != serverP)
  {
    targetP = targetP->next;
  }

  if(targetP)
  ds_iot_ulog_high("Found WatcherP");
  LOG_DEBUG("%p",targetP);// Address.

  return targetP;
}

/**
 * @fn lwm2m_watcher_t * prv_getWatcher()
 * @brief This function is used to get watcher 
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to uri
 * @param serverP   pointer to server to be searched
 *
 * @return    pointer to watcher node on success
 *        NULL on failure
 */
lwm2m_watcher_t * prv_getWatcher(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    lwm2m_server_t * serverP)
{
  lwm2m_observed_t * observedP = NULL ;
  bool allocatedObserver = LWM2M_FALSE;
  lwm2m_watcher_t * watcherP = NULL;


  if( contextP == NULL || uriP == NULL || serverP == NULL )
  { 
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return NULL;
  }

  observedP = prv_findObserved(contextP, uriP);
  if (observedP == NULL)
  {
    observedP = (lwm2m_observed_t *)lwm2m_malloc(sizeof(lwm2m_observed_t));
    if (observedP == NULL) return NULL;
    allocatedObserver = true;
    memset(observedP, 0, sizeof(lwm2m_observed_t));
    LWM2M_MEMCPY(&(observedP->uri), sizeof(lwm2m_uri_t), uriP, sizeof(lwm2m_uri_t));
    observedP->next = contextP->observedList;
    contextP->observedList = observedP;
  }

  watcherP = prv_findWatcher(observedP, serverP);
  if (watcherP == NULL)
  {
    watcherP = (lwm2m_watcher_t *)lwm2m_malloc(sizeof(lwm2m_watcher_t));
    if (watcherP == NULL)
    {
      if (allocatedObserver == true)
      {
        lwm2m_free(observedP);
      }
      return NULL;
    }
    memset(watcherP, 0, sizeof(lwm2m_watcher_t));
    watcherP->active = false;
    watcherP->server = serverP;
    watcherP->next = observedP->watcherList;
    observedP->watcherList = watcherP;
    #ifdef PSM_LWM2M
	lwm2m_psm_operation(contextP, serverP->shortID, uriP, PSM_NODE_OBSERVATION, PSM_NODE_UNCHANGED, PSM_ADD_NODE);
    #endif
  }

  return watcherP;
}

/**
 * @fn lwm2m_watcher_t * prv_findInheritedWatcher()
 * @brief This function is used to get watcher of higher level according to 
 *        precedence rule.
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to uri
 * @param serverP   pointer to server to be searched
 * @param flag      attribute to be checked
 * 
 * @return    pointer to watcher node on success
 *        NULL on failure
 */
lwm2m_watcher_t * prv_findInheritedWatcher(lwm2m_context_t * contextP,
                                                  lwm2m_uri_t *uriP,
                                                  lwm2m_server_t *serverP,
                                                  uint8_t flag)
{
  lwm2m_watcher_t *watcher = NULL;
  lwm2m_observed_t *observed = NULL;

  if(contextP == NULL || serverP == NULL || uriP == NULL ) 
  { 
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return NULL;
  }

  if(LWM2M_URI_IS_SET_RESOURCE(uriP))
  {
    uriP->flag &= ~LWM2M_URI_FLAG_RESOURCE_ID;
    observed = prv_findObserved(contextP, uriP);
    if(observed == NULL)
    {
      uriP->flag &= ~LWM2M_URI_FLAG_INSTANCE_ID;
      observed = prv_findObserved(contextP, uriP);
      if(observed == NULL)
      {
        LOG_DEBUG("Observed list not found for URI ");
      }
      else
      {
        watcher = prv_findWatcher(observed, serverP);
      }
      uriP->flag |= LWM2M_URI_FLAG_INSTANCE_ID;
    }
    else
    {
      watcher = prv_findWatcher(observed, serverP);
      if(watcher != NULL && watcher->parameters != NULL)
      {
        if((watcher->parameters->toSet & flag) == 0)
        {
          uriP->flag &= ~LWM2M_URI_FLAG_INSTANCE_ID;
          observed = prv_findObserved(contextP, uriP);
          if(observed == NULL)
          {
            watcher = NULL;
            LOG_DEBUG("Observed list not found for URI ");
          }
          else
          {
            watcher = prv_findWatcher(observed, serverP);
          }
          uriP->flag |= LWM2M_URI_FLAG_INSTANCE_ID;
        }
      }
      // If inherited watcher has no attributes set, move to object level
      else if(watcher != NULL && watcher->parameters == NULL)
      {
        uriP->flag &= ~LWM2M_URI_FLAG_INSTANCE_ID;
        observed = prv_findObserved(contextP, uriP);
        if(observed == NULL)
        {
          watcher = NULL;
          LOG_DEBUG("Observed list not found for URI ");
        }
        else
        {
          watcher = prv_findWatcher(observed, serverP);
        }
        uriP->flag |= LWM2M_URI_FLAG_INSTANCE_ID;
      }
    }
    uriP->flag |= LWM2M_URI_FLAG_RESOURCE_ID;
  }
  else if(LWM2M_URI_IS_SET_INSTANCE(uriP))
  {
    uriP->flag &= ~LWM2M_URI_FLAG_INSTANCE_ID;
    observed = prv_findObserved(contextP, uriP);
    if(observed == NULL)
    {
      LOG_DEBUG("Observed list not found for URI ");
    }
    else
    {
      watcher = prv_findWatcher(observed, serverP);
    }
    uriP->flag |= LWM2M_URI_FLAG_INSTANCE_ID;
  }

  return watcher;
}

/**
 * @fn uint8_t sync_observations_to_persistence()
 * @brief This function is used to store the observations into persistent storage
 * 
 * @param contextP pointer to lwm2m_context_t
 * 
 * @return return error code
 */
int sync_observations_to_persistence(lwm2m_context_t * contextP)
{
  lwm2m_observed_t *observed = NULL;
  int fd = 0;
  lwm2m_persistent_watcher_t *payload = NULL;
  uint8_t length = 0;

  if (NULL == contextP )
    return EFAILURE;

  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_PERSITENT_OBSERVATION_STORAGE);
  fd = efs_open(LWM2M_PERSITENT_OBSERVATION_STORAGE, O_CREAT | O_TRUNC 
      | O_WRONLY, 0644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("Failed to open observation persistent storage");
    return EFAILURE;
  }
  if ( contextP->observedList == NULL )
  {
    efs_close(fd);
    return ESUCCESS;
  }   

  observed = contextP->observedList;
  while (observed)
  {
    lwm2m_watcher_t *watcher = observed->watcherList;
    while(watcher)
    {
      length++;
      watcher = watcher->next;
    }
    observed= observed->next;
  }

  if(length != 0)
  {
    payload = lwm2m_malloc(length * sizeof(lwm2m_persistent_watcher_t));
    if(payload == NULL)
      return EFAILURE;
    memset(payload, 0x00, length * sizeof(lwm2m_persistent_watcher_t));
  }
  if(payload == NULL)
      return EFAILURE;
  length = 0;
  observed = contextP->observedList;
  while (observed)
  {
    lwm2m_watcher_t *watcher = observed->watcherList;
    LOG_DEBUG("observed[%x]", observed);
    while (watcher)
    {
      lwm2m_persistent_watcher_t info;

      memset(&info, 0 , sizeof(info));
      info.active = watcher->active;
      info.counter = watcher->counter;
      info.lastMid = watcher->lastMid;
      info.lastTime = watcher->lastTime;
      info.lastValue.asFloat = watcher->lastValue.asFloat;
      info.serverID = watcher->server->shortID;
      info.format = watcher->format;
      info.pmin_wait_count = watcher->pmin_wait_count;
      if (NULL != watcher->parameters)
      {
        LWM2M_MEMCPY(&info.parameters, sizeof(info.parameters), watcher->parameters, sizeof(lwm2m_attributes_t));
        info.isParameterSet = LWM2M_TRUE;
      }
      LWM2M_MEMCPY(&info.token, sizeof(info.token), watcher->token, watcher->tokenLen);
      info.tokenLen = watcher->tokenLen;
      LWM2M_MEMCPY(&info.uri, sizeof(lwm2m_uri_t), &observed->uri, sizeof(lwm2m_uri_t));
      memcpy((void *)(payload +length), &info, sizeof(info));
      length++;
      watcher = watcher->next;
    }
    observed = observed->next;
        }
  ds_iot_log_strf_med("EFS_OPT: File write %s",LWM2M_PERSITENT_OBSERVATION_STORAGE);
  if (EFAILURE == efs_write(fd, payload, length *sizeof(lwm2m_persistent_watcher_t)))
        {
    ds_iot_ulog_err("Failed to update the obseravation persistence.");
        }

  if(payload)
        {
    lwm2m_free(payload);
  }
  efs_close(fd);
  return ESUCCESS;
}

/**
 * @fn uint8_t sync_observations_from_persistence(lwm2m_context_t contextP)
 * @brief This function is used fetch stored observations from persistent storage
 * 
 * @param contextP pointer to lwm2m_context_t
 * 
 * @return return error code
 */
int sync_observations_from_persistence(lwm2m_context_t *contextP)
{
  int fd = 0;

  if (NULL == contextP || NULL == contextP->serverList)
  {
    return EFAILURE;
  }

  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_PERSITENT_OBSERVATION_STORAGE);
  fd = efs_open(LWM2M_PERSITENT_OBSERVATION_STORAGE, O_RDONLY);
  if (EFAILURE == fd)
  {
    return EFAILURE;
  }

  while (1)
  {
    lwm2m_persistent_watcher_t info;
    lwm2m_server_t *serverP = NULL;
    lwm2m_watcher_t *watcherP = NULL;
    int ret = 0;

    memset(&info, 0, sizeof(info));
    ret = efs_read(fd, &info, sizeof(info));
    if (EFAILURE == ret || 0 == ret)
      break;

    // Search for the watching server from the serverList
    serverP = contextP->serverList;
    while (serverP)
    {
      if (info.serverID == serverP->shortID)
        break;
      serverP = serverP->next;
    }

    //! Server not found, drop the observation
    if (NULL == serverP)
      continue;

    watcherP = prv_getWatcher(contextP, &info.uri, serverP);
    if (NULL == watcherP)
    {
      ds_iot_ulog_err1("failed to restore a observation by %hu", serverP->shortID);
      continue;
    }

    watcherP->active = info.active;
    watcherP->counter = info.counter;
    watcherP->lastMid = info.lastMid;
    watcherP->lastTime = info.lastTime;
    watcherP->lastValue.asFloat = info.lastValue.asFloat;
    watcherP->server = serverP;
    watcherP->tokenLen = info.tokenLen;
    watcherP->update = LWM2M_FALSE;
    watcherP->format = info.format;
    watcherP->pmin_wait_count = info.pmin_wait_count;
    LWM2M_MEMCPY(watcherP->token, TOKEN_LEN, info.token, info.tokenLen);
    if (LWM2M_TRUE == info.isParameterSet)
    {
      watcherP->parameters = (lwm2m_attributes_t *)lwm2m_calloc(1, 
          sizeof(lwm2m_attributes_t));
    }
    if (NULL != watcherP->parameters)
    {
      LWM2M_MEMCPY(watcherP->parameters, sizeof(lwm2m_attributes_t), 
          &info.parameters, sizeof(info.parameters));
    }
    #ifdef PSM_LWM2M
    if(watcherP->active == FALSE)
		lwm2m_psm_operation(contextP, watcherP->server->shortID, &(info.uri), PSM_NODE_OBSERVATION, PSM_NODE_UNCHANGED, PSM_REMOVE_NODE);
    #endif
  }
  efs_close(fd);
  return ESUCCESS;
}
/**
 * @fn uint8_t observe_handleRequest()
 * @brief This function is used to handle observe requests
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uriP      pointer to uri
 * @param serverP   pointer to server to be searched
 * @param dataP     handle t lwm2m data
 * @param message   handle to coap packet
 * @param response  handle to coap response packet
 *
 * @return  LWM2M response type
 */
uint8_t observe_handleRequest(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    lwm2m_server_t * serverP,
    int size,
    lwm2m_data_t * dataP,
    qapi_Coap_Packet_t * message,
    qapi_Coap_Packet_t * response)
{
  lwm2m_watcher_t * watcherP = NULL;
  uint32_t count_len = 0;
  uint32_t *count = NULL;
  uint8_t result = LWM2M_400_BAD_REQUEST;

  if ( contextP == NULL || uriP == NULL || serverP == NULL
      || dataP == NULL || message == NULL || response == NULL )
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_400_BAD_REQUEST;
  }

  count = lwm2m_calloc(1, sizeof(uint32_t));
  if(count == NULL)
    return LWM2M_500_INTERNAL_SERVER_ERROR;

  LOG_DEBUG("Code: %02X, server status: %s", message->code, STR_STATUS(serverP->status));
  LOG_URI(uriP);

  qapi_Coap_Get_Header(serverP->coapHandle,message , QAPI_COAP_OBSERVE ,(void **) &count , (size_t *)&count_len);

  switch(*count)
  {
    case 0:
      if ((!LWM2M_URI_IS_SET_INSTANCE(uriP) && LWM2M_URI_IS_SET_RESOURCE(uriP))
           || message->token_len == 0)
      {
        result = (uint8_t)LWM2M_400_BAD_REQUEST;
        goto end;
      }
      watcherP = prv_getWatcher(contextP, uriP, serverP);
      if (watcherP == NULL)
      {
        result = LWM2M_500_INTERNAL_SERVER_ERROR;
        goto end;
      }

      watcherP->tokenLen = message->token_len;
      LWM2M_MEMCPY(watcherP->token, TOKEN_LEN, message->token, message->token_len);
      watcherP->active = true;
      watcherP->lastTime = lwm2m_gettime();
      if(uriP->objectId == LWM2M_LOCATION_OBJECT_ID)
        qurt_signal_set(&lwm2m_signal_Handle, LOCATION_PMIN_UPDATED);

      ds_iot_ulog_high2("Made %p WatcherP active at %u time",watcherP,watcherP->lastTime);
      qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);  //Set signal to visit process signal immediately
      if (QAPI_IS_OPTION(message, QAPI_COAP_OPTION_ACCEPT))
      {
        watcherP->format = utils_convertMediaType(message->accept[0]);
      }
      else
      {
        watcherP->format = LWM2M_CONTENT_TEXT;
      }
      if(watcherP->format == LWM2M_CONTENT_UNSUPPORTED)
      {
        result = LWM2M_406_NOT_ACCEPTABLE;
        goto end;
      }

      if (LWM2M_URI_IS_SET_RESOURCE(uriP))
      {
        switch (dataP->type)
        {
          case LWM2M_TYPE_INTEGER:
            if (1 != lwm2m_data_decode_int(dataP, &(watcherP->lastValue.asInteger))) 
            {
              result = LWM2M_500_INTERNAL_SERVER_ERROR;
              goto end;
            }
            break;
          case LWM2M_TYPE_FLOAT:
            if (1 != lwm2m_data_decode_float(dataP, &(watcherP->lastValue.asFloat)))
            {
              result = LWM2M_500_INTERNAL_SERVER_ERROR; 
              goto end;
            }
            break;
          default:
            break;
        }
      }
      if (!(uriP->objectId == LWM2M_LOCATION_OBJECT_ID && !trackingSessionId))
      {
      qapi_Coap_Set_Header(serverP->coapHandle,response ,QAPI_COAP_OBSERVE , &(watcherP->counter) , 0 ) ;
        watcherP->counter++;
      }

    #ifdef PSM_LWM2M
	lwm2m_psm_operation(contextP, serverP->shortID, uriP, PSM_NODE_OBSERVATION, PSM_NODE_UNCHANGED, PSM_ADD_NODE);
    #endif
      result = LWM2M_205_CONTENT;
      break;

    case 1:
      // cancellation
      ds_iot_ulog_high("Cancel observation for ");
      PRINT_URI(uriP);
      observe_cancel(contextP, LWM2M_MAX_ID, serverP->coapHandle, uriP);
      result = LWM2M_205_CONTENT;

      break;

    default:
      result = LWM2M_400_BAD_REQUEST;
  }
  end:
      lwm2m_free(count);
      count = NULL;
  return result;
}

void observe_delete(lwm2m_context_t *contextP,
                    lwm2m_uri_t *uriP)
{
  lwm2m_observed_t *observedP = NULL;


  if(contextP == NULL || uriP == NULL) 
  { 
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return;
  }

  LOG_URI(uriP);

  observedP = contextP->observedList;
  while(observedP != NULL)
  {
    if(observedP->uri.objectId == uriP->objectId
       &&(LWM2M_URI_IS_SET_INSTANCE(uriP) == false
       || observedP->uri.instanceId == uriP->instanceId))
    {
      lwm2m_observed_t *nextP;
      lwm2m_watcher_t *watcherP;

      nextP = observedP->next;

      for(watcherP = observedP->watcherList; watcherP != NULL; watcherP = watcherP->next)
      {
        if(watcherP->parameters != NULL)
        {
          lwm2m_free(watcherP->parameters);
          watcherP->parameters= NULL;
        }
        #ifdef PSM_LWM2M
		lwm2m_psm_operation(contextP, watcherP->server->shortID, uriP, PSM_NODE_OBSERVATION, PSM_NODE_UNCHANGED, PSM_REMOVE_NODE);
        #endif
      }
      LWM2M_LIST_FREE(observedP->watcherList);

      lwm2m_unlink_node((void **)&contextP->observedList, (void *)observedP);
      //prv_unlinkObserved(contextP, observedP);
      ds_iot_ulog_high1("Removed observedP  %p from list",observedP);
      lwm2m_free(observedP);
      observedP = NULL;

      observedP = nextP;
    }
    else
    {
      observedP = observedP->next;
    }
  }
  // Sync the latest observation info to persistent storage
  if(persis_regstatus_feat_enabled == LWM2M_TRUE)
  {
    sync_observations_to_persistence(contextP);
  }
  if(uriP->objectId == LWM2M_LOCATION_OBJECT_ID)
    qurt_signal_set(&lwm2m_signal_Handle, LOCATION_PMIN_UPDATED);
}

/**
 * @fn void observe_cancel()
 * @brief This function is used to cancel an observation
 *
 * @param contextP      pointer to lwm2m_context_t
 * @param mid           mid value of watcher list
 * @param fromSessionH  void pointer to session
 *
 * @return    void
 *
 */
void observe_cancel(lwm2m_context_t * contextP,
    uint16_t mid,
    void * fromCoapHandleH,
    lwm2m_uri_t *uri)
{
  lwm2m_observed_t * observedP = NULL;
  lwm2m_watcher_t * targetP = NULL;

  if( contextP == NULL || fromCoapHandleH == NULL ) 
  { 
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return ;
  } 

  LOG_DEBUG("mid: %d", mid);
  ds_iot_ulog_high1("Observe cancel request with mid %d",mid);

  // Handling cancel observe with OBSERVE flag set to 1
  if(LWM2M_MAX_ID == mid && uri != NULL)
  {
    observedP = prv_findObserved(contextP,uri);
    if(observedP != NULL)
    {
      targetP = observedP->watcherList;
      while (targetP != NULL && (!lwm2m_coap_sessionhandle_is_equal(targetP->server->coapHandle, fromCoapHandleH, contextP->userData)))
      {
        targetP = targetP->next;
      }
      if(targetP)
      {
        targetP->active = false;
        #ifdef PSM_LWM2M
		lwm2m_psm_operation(contextP, targetP->server->shortID, uri, PSM_NODE_OBSERVATION, PSM_NODE_UNCHANGED, PSM_REMOVE_NODE);
        #endif
      }
    }
  }

  // Handling cancel observe came through RESET message
  else if(uri == NULL)
  {
    for(observedP = contextP->observedList; observedP != NULL; observedP = observedP->next)
    {
      lwm2m_watcher_t * parentP = NULL;
      parentP = observedP->watcherList;
      while (parentP != NULL
             && (parentP->lastMid != mid
             || (! lwm2m_coap_sessionhandle_is_equal(parentP->server->coapHandle, fromCoapHandleH, contextP->userData))))
      {
        parentP = parentP->next;
      }
      if (parentP != NULL)
      {
        targetP = parentP;
        targetP->active = false;
        #ifdef PSM_LWM2M
		lwm2m_psm_operation(contextP, targetP->server->shortID, &(observedP->uri), PSM_NODE_OBSERVATION, PSM_NODE_UNCHANGED, PSM_REMOVE_NODE);
        #endif
        break;
      }
    }
  }
  // Sync the latest observation info to persistent storage
  if(persis_regstatus_feat_enabled == LWM2M_TRUE)
  {
    sync_observations_to_persistence(contextP);
  }
  qurt_signal_set(&lwm2m_signal_Handle, LOCATION_PMIN_UPDATED);
}

/**
 * @fn qapi_coap_status_t observe_setParameters()
 * @brief This function is used to observe attributes of given uri
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uri       pointer to uri
 * @param serverP   pointer to lwm2m_server_t
 * @param attrP     pointer to attribute structure
 *
 * @return  LWM2M response type
 *
 */
uint8_t observe_setParameters(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    lwm2m_server_t * serverP,
    lwm2m_attributes_t * attrP)
{
  uint8_t result = LWM2M_NO_ERROR;
  lwm2m_watcher_t * watcherP = NULL;
  if( contextP == NULL || uriP == NULL || serverP == NULL 
      || attrP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_400_BAD_REQUEST;
  }

  if (!LWM2M_URI_IS_SET_INSTANCE(uriP) && LWM2M_URI_IS_SET_RESOURCE(uriP)) return (uint8_t)LWM2M_400_BAD_REQUEST; //LWM2M_400_BAD_REQUEST;

  result = object_checkReadable(contextP, uriP, serverP);
  if (LWM2M_205_CONTENT != result) return (uint8_t) result;

  if (0 != (attrP->toSet & ATTR_FLAG_NUMERIC))
  {
    result = object_checkNumeric(contextP, uriP, serverP);
    if (LWM2M_205_CONTENT != result) return (uint8_t) result;
  }

  watcherP = prv_getWatcher(contextP, uriP, serverP);
  if (watcherP == NULL) return (uint8_t) LWM2M_500_INTERNAL_SERVER_ERROR;
  
  #ifdef PSM_LWM2M
  if(watcherP->active == FALSE)
  {
    lwm2m_psm_operation(contextP, serverP->shortID, uriP, PSM_NODE_OBSERVATION, PSM_NODE_UNCHANGED, PSM_REMOVE_NODE);
  }
  #endif 

  ds_iot_ulog_high("Found/Alloc watcher: %p for URI"); 
  PRINT_URI(uriP);

  // Check rule “lt” value + 2*”stp” values < “gt” value and lt value < gt value
  if ((((attrP->toSet | (watcherP->parameters?watcherP->parameters->toSet:0)) & ~attrP->toClear) & ATTR_FLAG_NUMERIC) >= ATTR_FLAG_LT_GT)
  {
    float gt = 0.0;
    float lt = 0.0;
    float stp = 0.0;
	bool  isGtSet = false;
	bool  isLtSet = false;
	bool  isStSet = false;

    if (0 != (attrP->toSet & LWM2M_ATTR_FLAG_GREATER_THAN))
    {
      gt = attrP->greaterThan;
	  isGtSet = true;
    }
    else if (NULL != watcherP->parameters)
    {
      gt = watcherP->parameters->greaterThan;
	  if( (watcherP->parameters->toSet & LWM2M_ATTR_FLAG_GREATER_THAN) && !(attrP->toClear & LWM2M_ATTR_FLAG_GREATER_THAN))
	  {//Gt ever set, and it is not clear here;
	    isGtSet = true;
	  }	   
    }
    else
    {
      //Do nothing
    }
    if (0 != (attrP->toSet & LWM2M_ATTR_FLAG_LESS_THAN))
    {
      lt = attrP->lessThan;
	  isLtSet = true;
    }
    else if (NULL != watcherP->parameters)
    {
      lt = watcherP->parameters->lessThan;
	  if( (watcherP->parameters->toSet & LWM2M_ATTR_FLAG_LESS_THAN) && !(attrP->toClear & LWM2M_ATTR_FLAG_LESS_THAN))
	  {//Lt ever set, and it is not clear here;
	    isLtSet = true;
	  }	   	  
    }
    else
    {
      //Do nothing
    }
    if (0 != (attrP->toSet & LWM2M_ATTR_FLAG_STEP))
    {
      stp = attrP->step;
	  isStSet = true;	  
    }
    else if (NULL != watcherP->parameters)
    {
      stp = watcherP->parameters->step;
	  if( (watcherP->parameters->toSet & LWM2M_ATTR_FLAG_STEP) && !(attrP->toClear & LWM2M_ATTR_FLAG_STEP))
	  {//St ever set, and it is not clear here;
	    isStSet = true;
	  }	   	  	  
    }
    else
    {
      //Do nothing
    }

	if(isGtSet && isLtSet && isStSet)
	{
      if (lt + (2 * stp) >= gt) return (uint8_t)  LWM2M_400_BAD_REQUEST; 
	}
	else if(isGtSet && isLtSet)
	{
	  if (lt >= gt) return (uint8_t)	LWM2M_400_BAD_REQUEST; 
	}
  }

  if (watcherP->parameters == NULL)
  {
    if (attrP->toSet != 0)
    {
      watcherP->parameters = (lwm2m_attributes_t *)lwm2m_malloc(sizeof(lwm2m_attributes_t));
      if (watcherP->parameters == NULL) return (uint8_t)LWM2M_500_INTERNAL_SERVER_ERROR;
      memset(watcherP->parameters, 0, sizeof(lwm2m_attributes_t));
      LWM2M_MEMCPY(watcherP->parameters, sizeof(lwm2m_attributes_t), attrP, sizeof(lwm2m_attributes_t));
    }
  }
  else
  {
    watcherP->parameters->toSet &= ~attrP->toClear;
    watcherP->parameters->toSet |= attrP->toSet;
    ds_iot_ulog_high1("toSet: %d", watcherP->parameters->toSet);
    ds_iot_ulog_high1("toClear: %d", attrP->toClear);
    if (attrP->toSet & LWM2M_ATTR_FLAG_MIN_PERIOD)
    {
      watcherP->parameters->minPeriod = attrP->minPeriod;
      ds_iot_ulog_high1("minPeriod: %d", watcherP->parameters->minPeriod);
    }
    if (attrP->toSet & LWM2M_ATTR_FLAG_MAX_PERIOD)
    {
      watcherP->parameters->maxPeriod = attrP->maxPeriod;
      ds_iot_ulog_high1("maxPeriod: %d", watcherP->parameters->maxPeriod);
    }
    if (attrP->toSet & LWM2M_ATTR_FLAG_GREATER_THAN)
    {
      watcherP->parameters->greaterThan = attrP->greaterThan;
      ds_iot_ulog_high1("greaterThan: %d",(int)watcherP->parameters->greaterThan);
    }
    if (attrP->toSet & LWM2M_ATTR_FLAG_LESS_THAN)
    {
      watcherP->parameters->lessThan = attrP->lessThan;
      ds_iot_ulog_high1("lessThan: %d",(int)watcherP->parameters->lessThan);
    }
    if (attrP->toSet & LWM2M_ATTR_FLAG_STEP)
    {
      watcherP->parameters->step = attrP->step;
      ds_iot_ulog_high1("step: %d",(int)watcherP->parameters->step);
    }
    if (attrP->toClear & LWM2M_ATTR_FLAG_MIN_PERIOD)
    {
      watcherP->parameters->minPeriod = 0;
      ds_iot_ulog_high("minPeriod: Cleared ");
    }
    if (attrP->toClear & LWM2M_ATTR_FLAG_MAX_PERIOD)
    {
      watcherP->parameters->maxPeriod = 0;
      ds_iot_ulog_high("maxPeriod: Cleared");
    }
    if (attrP->toClear & LWM2M_ATTR_FLAG_GREATER_THAN)
    {
      watcherP->parameters->greaterThan = 0;
      ds_iot_ulog_high("greaterThan: Cleared");
    }
    if (attrP->toClear & LWM2M_ATTR_FLAG_LESS_THAN)
    {
      watcherP->parameters->lessThan = 0;
      ds_iot_ulog_high("lessThan: Cleared");
    }
    if (attrP->toClear & LWM2M_ATTR_FLAG_STEP)
    {
      watcherP->parameters->step = 0;
      ds_iot_ulog_high("step: Cleared");
    }
  }

  if ((uriP->objectId == LWM2M_LOCATION_OBJECT_ID) && (attrP->toSet & LWM2M_ATTR_FLAG_MIN_PERIOD) && (attrP->toSet & LWM2M_ATTR_FLAG_MAX_PERIOD))
  	qurt_signal_set(&lwm2m_signal_Handle, LOCATION_PMIN_UPDATED);
  
  #ifdef PSM_LWM2M
  lwm2m_psm_operation(contextP, serverP->shortID, uriP, PSM_NODE_OBSERVATION, PSM_NODE_UPDATE, PSM_MODIFY_NODE);

  #endif
  return (uint8_t) LWM2M_204_CHANGED; 
}

/**
 * @fn lwm2m_observed_t * observe_findByUri()
 * @brief This function is used to get observe object by searching uri
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uri       pointer to uri
 *
 * @return    pointer to observe node on success
 *              NULL on failure
 *
 */
lwm2m_observed_t * observe_findByUri(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP)
{
  lwm2m_observed_t * targetP = NULL;
  if( contextP == NULL || uriP == NULL ) 
  { 
    LOG_DEBUG("fun [%s] %s",__func__,nullError); 
    return NULL; 
  }

  LOG_URI(uriP);
  targetP = contextP->observedList;
  while (targetP != NULL)
  {
    if (targetP->uri.objectId == uriP->objectId)
    {
      if ((!LWM2M_URI_IS_SET_INSTANCE(uriP) && !LWM2M_URI_IS_SET_INSTANCE(&(targetP->uri)))
          || (LWM2M_URI_IS_SET_INSTANCE(uriP) && LWM2M_URI_IS_SET_INSTANCE(&(targetP->uri)) && (uriP->instanceId == targetP->uri.instanceId)))
      {
        if ((!LWM2M_URI_IS_SET_RESOURCE(uriP) && !LWM2M_URI_IS_SET_RESOURCE(&(targetP->uri)))
            || (LWM2M_URI_IS_SET_RESOURCE(uriP) && LWM2M_URI_IS_SET_RESOURCE(&(targetP->uri)) && (uriP->resourceId == targetP->uri.resourceId)))
        {
          LOG_DEBUG("Found one with%s observers.", targetP->watcherList ? "" : " no");
          LOG_URI(&(targetP->uri));
          return targetP;
        }
      }
    }
    targetP = targetP->next;
  }

  LOG_DEBUG("Found nothing");
  return NULL;
}

/**
 * @fn void lwm2m_resource_value_changed()
 * @brief This function is used to change the value of resource
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param uri       pointer to uri
 *
 * @return    void
 *
 */
void lwm2m_resource_value_changed(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP)
{
  lwm2m_observed_t * targetP = NULL;


  if(contextP == NULL || uriP == NULL )
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError); 
    return ;
  } 
  LOG_URI(uriP);
  targetP = contextP->observedList;
  // Make watcher update true for watcher tagged with resource
  while (targetP != NULL)
  {
    if (targetP->uri.objectId == uriP->objectId)
    {
      if (LWM2M_URI_IS_SET_INSTANCE(uriP)
          && (targetP->uri.flag & LWM2M_URI_FLAG_INSTANCE_ID) != 0
          && uriP->instanceId == targetP->uri.instanceId)
      {
        if (LWM2M_URI_IS_SET_RESOURCE(uriP)
            && (targetP->uri.flag & LWM2M_URI_FLAG_RESOURCE_ID) != 0
            && uriP->resourceId == targetP->uri.resourceId)
        {
          lwm2m_watcher_t * watcherP;

          LOG_DEBUG("Found an observation");
          LOG_URI(&(targetP->uri));

          for (watcherP = targetP->watcherList ; watcherP != NULL ; watcherP = watcherP->next)
          {
            if (watcherP->active == true)
            {
              LOG_DEBUG("Tagging a watcher");
              watcherP->update = true;
              qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE); 
              #ifdef PSM_LWM2M
			  lwm2m_psm_operation(contextP, watcherP->server->shortID, uriP,
                                               PSM_NODE_OBSERVATION, PSM_NODE_UPDATE, PSM_MODIFY_NODE);

              #endif
            }
          }
        }
      }
    }
    targetP = targetP->next;
  }
}

/**
 * @fn void observer_add_notify_to_pending_list()
 * @brief This function is used to add notify to be sent in sleep state 
 *        into the list
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param watcherP watcher List
 * @param format lwm2m media format for sending the request
 * @param length length of the message to be sent
 *
 * @return    void
 *
 */
int observer_add_notify_to_pending_list(lwm2m_context_t * contextP, lwm2m_watcher_t * watcherP,
    lwm2m_media_type_t format, uint8_t * buffer, size_t length)
{
  lwm2m_pending_notify_t * watcherNode = NULL;
  lwm2m_pending_notify_t *notifyP = NULL;
  qapi_Coap_Packet_t *message = NULL;  
  lwm2m_pending_notify_t * node = NULL;
  lwm2m_pending_notify_t *prev_node = NULL;

  if(contextP == NULL || watcherP == NULL || buffer == NULL)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }  
  
  /* Loop over pending notify list and store the latest entry */
  node = contextP->notifypendingList;
  while(node != NULL && node->watcher_info != NULL)
  {
    /* Store the last "n" notifications based on config item NOTIFICATIONS_STORE
       Remove the old notifications and store latest "n" notifications */
    if(node->watcher_info == watcherP && node->message && 
       node->watcher_info->store_count == data.cfg_info.store_notifications)
    {
      node->watcher_info->store_count--;
      if(prev_node != NULL)
      {
        prev_node->next = node->next;
      }
      else 
      {
        contextP->notifypendingList = node->next;
      }
      qapi_Coap_Free_Message(watcherP->server->coapHandle,node->message);
      node->message = NULL;
      node->watcher_info = NULL; 
      lwm2m_free(node);
      node = NULL;
      break;
    }
    else 
    {
      prev_node = node;
      node = node->next;
    }
  }

  notifyP = (lwm2m_pending_notify_t *) lwm2m_malloc(sizeof(lwm2m_pending_notify_t));
  if(NULL == notifyP)
  {
    LOG_DEBUG("%s",mallocError);
    return EFAILURE;
  }

  memset(notifyP, 0, sizeof(lwm2m_pending_notify_t));
  notifyP->watcher_info = watcherP;

  qapi_Coap_Init_Message(watcherP->server->coapHandle,&message,QAPI_COAP_TYPE_NON,LWM2M_205_CONTENT);
  if(message == NULL)
  {
    lwm2m_free(notifyP);
    return EFAILURE;
  }

  notifyP->message = message;
  notifyP->next = NULL;
  qapi_Coap_Set_Header(watcherP->server->coapHandle,message ,QAPI_COAP_CONTENT_TYPE,&format,0);
  qapi_Coap_Set_Header(watcherP->server->coapHandle,message ,QAPI_COAP_TOKEN ,watcherP->token, watcherP->tokenLen); 
  qapi_Coap_Set_Header(watcherP->server->coapHandle,message ,QAPI_COAP_OBSERVE , &(watcherP->counter) , 0 ) ;
 
  
  watcherP->counter++; 
  qapi_Coap_Set_Payload(watcherP->server->coapHandle,message, buffer, length);
 
  
  if(NULL == contextP->notifypendingList)
  { 
    notifyP->next = contextP->notifypendingList;
    contextP->notifypendingList = notifyP;
  } 
  else 
  {
    for(watcherNode = contextP->notifypendingList; (watcherNode != NULL && watcherNode->next != NULL) ; watcherNode = watcherNode->next);
    // if(!watcherNode) return EFAILURE; commented to fix KW
    watcherNode->next = notifyP;
  }

  return ESUCCESS;  
}

/**
 * @fn void observe_step()
 * @brief This function is used to, periodically perform notify
 *        and update timeout 
 *
 * @param contextP  pointer to lwm2m_context_t
 * @param currentTime  Current time 
 * @param timeoutP  handle to timeout value
 *
 * @return    void
 *
 */
void observe_step(lwm2m_context_t * contextP,
    time_t currentTime,
    time_t * timeoutP)
{
  lwm2m_observed_t * targetP = NULL;
  qapi_Coap_Message_Params_t message_param;
  bool efs_call = false;
  LOG_DEBUG("Entering");
  
  if ( contextP == NULL || timeoutP == NULL ) 
  {
    ds_iot_ulog_err("fun [observe_step] LWM2M_LOG: NULL Error");
    return;
  }
   memset(&message_param,0,sizeof(message_param));
  ds_iot_ulog_high3(" In Obs_step @ %u time, %u timeout and sleepstate: %d", currentTime, *timeoutP,lwm2m_in_sleep_state);

  //Send Pending Notifications
  if (!lwm2m_in_sleep_state && NULL != contextP->notifypendingList)
  {
    lwm2m_pending_notify_t *node = NULL;
    void *payload = NULL;
    node = contextP->notifypendingList;


    while ( node && (node->watcher_info) && (node->watcher_info->server))
    { 
      if(node->watcher_info->server->status == STATE_DEREGISTERED &&  
          LWM2M_SERVER_IS_DISABLE_SET(node->watcher_info->server->update_disable_check))
      {
        /* Do not send pending notifications if the server has executed disable resource      
         * and the registration state is still de-registered.
         * Need to send these stored notifies after disable timeout has elapsed.
         */
        ds_iot_ulog_high1("server: %d is in disable timeout ",node->watcher_info->server->shortID);
        node = node->next; 
      }
      else{
        //Sending the pedning notifies stored either when in sleep or after disbale timeout elapses  
        ds_iot_ulog_high2("Sending pending notify for watcherP: %p to server : %d",node->watcher_info,node->watcher_info->server->shortID);
        if(STATE_REGISTERED == node->watcher_info->server->status)
        {   
          contextP->notifypendingList = node->next;
          if (!node->watcher_info || !node->message)
          {
            if (node->message) 
            {
              if (((qapi_Coap_Packet_t *)node->message)->payload)
                lwm2m_free(((qapi_Coap_Packet_t *)node->message)->payload);
              lwm2m_free(node->message);
            }
            lwm2m_free(node);
            node = contextP->notifypendingList;
            continue;
          }
          if(node->watcher_info->store_count > 0)
            node->watcher_info->store_count--;
          node->watcher_info->lastTime = currentTime;

          message_param.lastmid = &node->watcher_info->lastMid ;
          message_param.msgUsrData = node->watcher_info->server->coapHandle ;

          payload = ((qapi_Coap_Packet_t *)node->message)->payload;

          if (qapi_Coap_Send_Message(node->watcher_info->server->coapHandle,node->message, &message_param) != QAPI_OK)
          {
            lwm2m_clean_coap_session(node->watcher_info->server);
          }

          node->watcher_info->pmin_wait_count = 0;   // Reset counter
          node->watcher_info->update = false;
          lwm2m_free(payload);
          lwm2m_free(node);
          node = contextP->notifypendingList;      
          // Sync the latest observation info to persistent storage
          if(persis_regstatus_feat_enabled == LWM2M_TRUE)
          {
            sync_observations_to_persistence(contextP);
          }      
        }
        else
        {
		  node = node->next; 
        }
      }
    }
  } //End of Send Pending Notifications

  //Process observation List
  for (targetP = contextP->observedList ; targetP != NULL ; targetP = targetP->next)
  {
    lwm2m_watcher_t * watcherP = NULL;
    uint8_t * buffer = NULL;
    size_t length = 0;
    lwm2m_data_t * dataP = NULL;
    int size = 0;
    double floatValue = 0;
    int64_t integerValue = 0;
    bool storeValue = false;
    qapi_Coap_Packet_t * message = NULL;
	qapi_Coap_Message_Params_t message_params;
    lwm2m_object_t * object = NULL;
    int result = 0;
    bool notifyen_val = false;
    time_t min_period = 0, max_period = 0;
    lwm2m_uri_t uri;
    bool continue_flag = FALSE;

    memset(&uri, 0x00, sizeof(lwm2m_uri_t));
    memset(&message_params, 0, sizeof(qapi_Coap_Message_Params_t));

    //Check for device capability object and if the NotifyEn resource is true prior to sending the notify for Resources Enabled or Attached
    if((DEV_CAP_OBJECT_ID == targetP->uri.objectId) && (targetP->uri.resourceId == RES_O_ATTACHED || targetP->uri.resourceId == RES_M_ENABLED) )
    {
      object = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, targetP->uri.objectId);
      if ((NULL != object) && (object->getValueFunc))
      {
        result = object->getValueFunc(targetP->uri.instanceId, object, &notifyen_val);
        if((LWM2M_204_CHANGED == result) && (false == notifyen_val))
          continue;  
      }
    }

    // Process watcher list
    for (watcherP = targetP->watcherList ; watcherP != NULL && watcherP->active && continue_flag == FALSE; watcherP = watcherP->next)
      {
        bool notify = false; //Flga that decides if notify needs to be sent
        storeValue = false;
		size = 0;

    /* If observation is performed on resource level, fetch the current value of the resource 
     * to check if the resource value has changed from previous notification.
     * The flag storeValue is set as true, to enable value change check prior to sending notify  
     */
    if (LWM2M_URI_IS_SET_RESOURCE(&targetP->uri))
    {
      // Read resource to value to check value change
           if (LWM2M_205_CONTENT != object_readData(contextP, &targetP->uri, &size, &dataP, watcherP->server))
      {
        continue_flag = TRUE;
        goto hdl_error;
      }
      if (NULL == dataP)
      {
        continue_flag = TRUE;
       	goto hdl_error;
      } 
      switch (dataP->type)
      {
        case LWM2M_TYPE_INTEGER:
               if (1 != lwm2m_data_decode_int(dataP, &integerValue))
			   {
			      continue_flag = TRUE;
       			   	   goto hdl_error;
               }
          if(integerValue != watcherP->lastValue.asInteger)
          {
            watcherP->update = true;
          }
          else
          {
            watcherP->update = false;
          }
          storeValue = true;
          break;
        case LWM2M_TYPE_FLOAT:
               if (1 != lwm2m_data_decode_float(dataP, &floatValue)) 
			   {
			      continue_flag = TRUE;
       			   	goto hdl_error;
               }
              if(ABS_DIFF(floatValue, (watcherP->lastValue.asFloat)) >= EPSILON)
              {
                watcherP->update = true;
              }
              else
              {
		watcherP->update = false;
              }
          storeValue = true;
              break;
        default:
          break;
      }  
        ds_iot_ulog_high3("storeValue[%d], dataP->type [%d], Value Changed[%d]", storeValue, dataP->type, watcherP->update);
          }

    // check if the resource value has changed and set watcher->update flag to true to trigger notify
		LOG_DEBUG("Watcher: %p",watcherP);

        if (LWM2M_URI_IS_SET_INSTANCE(&(targetP->uri)))             
        {                                                     
            if (LWM2M_URI_IS_SET_RESOURCE(&(targetP->uri)))          
            {                                             
            	
                ds_iot_ulog_high3("Watcher is for :/%d/%d/%d",targetP->uri.objectId, targetP->uri.instanceId, targetP->uri.resourceId); 
            }                                                 
            else                                              
            {                                                 
                       ds_iot_ulog_high2("Watcher is for :/%d/%d",targetP->uri.objectId, targetP->uri.instanceId); 
            }                                                 
        }                                                     
        else                                                  
        {                                                     
                   ds_iot_ulog_high1("Watcher is for :/%d",targetP->uri.objectId);
        }                                                     
    
        // Get Pmin and Pmax period and if pmin > pmax, notify on Pmax period
        memset(&uri, 0x00, sizeof(lwm2m_uri_t));
        memcpy(&uri, &targetP->uri, sizeof(lwm2m_uri_t));
        // Is the Maximum Period reached ?
        // Get the inherited Pmax for watcher if Pmax for current watcher is not set.
        max_period = get_pmax_value(contextP, watcherP, uri);

        memset(&uri, 0x00, sizeof(lwm2m_uri_t));
        memcpy(&uri, &targetP->uri, sizeof(lwm2m_uri_t));
        // Get the inherited Pmin for watcher if Pmin for current watcher is not set.
        min_period = get_pmin_value(contextP, watcherP, uri);

      /*if PMAX < PMIN, then it is invalid value that need to ignore*/
	  if(max_period < min_period)
	  {
	    max_period  = 0;
	  }

      /*
       *possible valid scenrio for pmin/pmax
       *1. 0 <  pmin < pmax,    if there is value changes, send Notify per pMin;    no value change, send notify per pMax	   
       *2. 0 == pmin <  pmax,   if there is value changes, send Notify immediatly;  no value change, send notify per pMax
       *3. 0 == pmin, 0 == pmax,if there is value changes, send Notify immediatly;  no value change, no notify; 
       *4. 0  < pmin, 0 == pmax,if there is value changes, send Notify per pMin;    no value change, no notify;
	   */
      if((min_period < max_period) || //cover 1/2 scenario
         (max_period == 0))           //cover 3/4 scenario
      {
        // If notify needs to be sent due to resource value change
        if (watcherP->update == true) 
        {
          // value changed, should we notify the server ?
          // If no parameters are set, wait for pMin time
          if (watcherP->parameters == NULL || watcherP->parameters->toSet == 0)
          {
            // no conditions
            notify = true;
            ds_iot_ulog_high("Notify with no conditions for watcher");
			LOG_DEBUG("%p",watcherP);
//            PRINT_URI(&(targetP->uri));
          }

          // If lt, gt and st parameters are set
          if (notify == false
              && watcherP->parameters != NULL
              && (watcherP->parameters->toSet & ATTR_FLAG_NUMERIC) != 0
              && dataP != NULL)
          {
            // check for less than parameter
            if ((watcherP->parameters->toSet & LWM2M_ATTR_FLAG_LESS_THAN) != 0)
            {
              LOG_DEBUG("Checking lower treshold");
              // Did we cross the lower treshold ?
              switch (dataP->type)
              {
                case LWM2M_TYPE_INTEGER:
                  if ((integerValue < watcherP->parameters->lessThan
                        && watcherP->lastValue.asInteger > watcherP->parameters->lessThan)
                      || (integerValue > watcherP->parameters->lessThan
                        && watcherP->lastValue.asInteger < watcherP->parameters->lessThan))
                  {
                    ds_iot_ulog_high("Notify on lower threshold crossing for watcher ");
					LOG_DEBUG(":%p",watcherP);
                    notify = true;
                  }
                  break;
                case LWM2M_TYPE_FLOAT:
                  if ((floatValue < watcherP->parameters->lessThan
                        && watcherP->lastValue.asFloat > watcherP->parameters->lessThan)
                      || (floatValue > watcherP->parameters->lessThan
                        && watcherP->lastValue.asFloat < watcherP->parameters->lessThan))
                  {
                    ds_iot_ulog_high("Notify on lower threshold crossing for watcher ");
					LOG_DEBUG(":%p",watcherP);
                    notify = true;
                  }
                  break;
                default:
                  break;
              }
            }
            // check for greater than parameter
            if ((watcherP->parameters->toSet & LWM2M_ATTR_FLAG_GREATER_THAN) != 0)
            {
              LOG_DEBUG("Checking upper treshold");
              // Did we cross the upper treshold ?
              switch (dataP->type)
              {
                case LWM2M_TYPE_INTEGER:
                  if ((integerValue < watcherP->parameters->greaterThan
                        && watcherP->lastValue.asInteger > watcherP->parameters->greaterThan)
                      || (integerValue > watcherP->parameters->greaterThan
                        && watcherP->lastValue.asInteger < watcherP->parameters->greaterThan))
                  {
                    ds_iot_ulog_high("Notify on upper threshold crossing for watcher ");
					LOG_DEBUG(":%p",watcherP);
                    notify = true;
                  }
                  break;
                case LWM2M_TYPE_FLOAT:
                  if ((floatValue < watcherP->parameters->greaterThan
                        && watcherP->lastValue.asFloat > watcherP->parameters->greaterThan)
                      || (floatValue > watcherP->parameters->greaterThan
                        && watcherP->lastValue.asFloat < watcherP->parameters->greaterThan))
                  {
                    ds_iot_ulog_high("Notify on upper threshold crossing for watcher");
					LOG_DEBUG(":%p",watcherP);
                    notify = true;
                  }
                  break;
                default:
                  break;
              }
            }
            // check for step parameter
            if ((watcherP->parameters->toSet & LWM2M_ATTR_FLAG_STEP) != 0)
            {
              LOG_DEBUG("Checking step");

              switch (dataP->type)
              {
                case LWM2M_TYPE_INTEGER:
                  {
                    int64_t diff;

                    diff = integerValue - watcherP->lastValue.asInteger;
                    if ((diff < 0 && (0 - diff) >= watcherP->parameters->step)
                        || (diff >= 0 && diff >= watcherP->parameters->step))
                    {
                      ds_iot_ulog_high("Notify on step condition for watcher ");
					  LOG_DEBUG(":%p",watcherP);
                      notify = true;
                    }
                  }
                  break;
                case LWM2M_TYPE_FLOAT:
                  {
                    double diff;

                    diff = floatValue - watcherP->lastValue.asFloat;
                    if ((diff < 0 && (0 - diff) >= watcherP->parameters->step)
                        || (diff >= 0 && diff >= watcherP->parameters->step))
                    {
                      ds_iot_ulog_high("Notify on step condition for watcher");
					  LOG_DEBUG(":%p",watcherP);
                      notify = true;
                    }
                  }
                  break;
                default:
                  break;
              }
            }
          }
          // Set notify to true if either Pmin/Pmax is set and Lt/gt/step is not set.
          if(notify == false && ((watcherP->parameters != NULL)&&((watcherP->parameters->toSet & LWM2M_ATTR_FLAG_MIN_PERIOD) || 
            (watcherP->parameters->toSet & LWM2M_ATTR_FLAG_MAX_PERIOD))))
          {
            if((watcherP->parameters->toSet & ATTR_FLAG_NUMERIC) == 0)
            {
              notify = true;
            }
          }
          if(notify == true)
          {
            if (watcherP->lastTime + min_period > currentTime)
            {
              // Update the timeout to be aligned to next Pmin time 
              update_timeout_val_with_period(watcherP, timeoutP, currentTime, min_period);
              notify = false;
              #ifdef PSM_LWM2M
			  result = lwm2m_psm_operation(contextP, watcherP->server->shortID, &uri, 
                                               PSM_NODE_OBSERVATION, PSM_NODE_UNCHANGED, PSM_MODIFY_NODE);

              #endif
            }
            else
            {
              ds_iot_ulog_high("Notify on minimal period for watcher ");
			  LOG_DEBUG(":%p",watcherP);
              notify = true;			    
            }
          }
        } // watcher->update = true
    }

        ds_iot_ulog_high1("Max period is %d",max_period);
        if(notify == false && max_period > 0)
        {
          if ((watcherP->lastTime + max_period <= currentTime))
          {
            ds_iot_ulog_high("Notify on maximal period for watcher ");
			LOG_DEBUG(":%p",watcherP);
            notify = true;
          }
          if(min_period <= max_period)
          {
            update_timeout_val_with_period(watcherP, timeoutP, currentTime, min_period);
          }
          else
          {
            update_timeout_val_with_period(watcherP, timeoutP, currentTime, max_period);
          }
        }

        #ifdef PSM_LWM2M
		result = lwm2m_psm_operation(contextP, watcherP->server->shortID,
                                           &uri, PSM_NODE_OBSERVATION, PSM_NODE_UNCHANGED, PSM_MODIFY_NODE);

        #endif

        if (notify == true)
        {
          if (buffer == NULL)
          {
            if (dataP != NULL)
            {
              length = lwm2m_data_serialize(&targetP->uri, size, dataP, &(watcherP->format), &buffer);
              if (length == 0) 
			  {
			     if(dataP)
                 {
                    lwm2m_data_free(size, dataP);
                    dataP = NULL;
                 }
			     break;
              }
            }
            else
            {
              if (LWM2M_205_CONTENT != object_read(contextP, &targetP->uri, &(watcherP->format), &buffer, &length, &dataP, &size, watcherP->server))
              {
                if(dataP)
                {
                  lwm2m_data_free(size, dataP);
                  dataP = NULL;
                }
                buffer = NULL;
                break;
              }
              if(dataP == NULL) break;
            }

            result = qapi_Coap_Init_Message(watcherP->server->coapHandle,&message ,QAPI_COAP_TYPE_NON , LWM2M_205_CONTENT);

            if (result == QAPI_ERROR)
			{
               goto hdl_error;
            }
            qapi_Coap_Set_Header(watcherP->server->coapHandle,message ,QAPI_COAP_CONTENT_TYPE,&(watcherP->format),0);
            qapi_Coap_Set_Payload(watcherP->server->coapHandle,message, buffer, length);
          }

          if (NULL == message) 
		  {
              goto hdl_error;
          } 
          //check if the device is in disable timeout period
          if( watcherP->server->status == STATE_DEREGISTERED && LWM2M_SERVER_IS_DISABLE_SET(watcherP->server->update_disable_check) )
          {
            if(watcherP->server->storing){
              // If the observation request is for resource level update the last value.
              if(storeValue)
              {
                switch (dataP->type)
                {
                  case LWM2M_TYPE_INTEGER:
                  if(integerValue != watcherP->lastValue.asInteger)
                  {
                    watcherP->lastValue.asInteger = integerValue;
                  }
                  break;
                  case LWM2M_TYPE_FLOAT:
                  if(floatValue != watcherP->lastValue.asFloat)
                  {
                    watcherP->lastValue.asFloat = floatValue;
                  }
                  break;
                }
				ds_iot_ulog_high3("storeValue[%d], dataP->type [%d], Value Changed[%d]", storeValue, dataP->type, watcherP->update);
              }
              ds_iot_ulog_high("Adding notifications into pending list for disable timeout");
              observer_add_notify_to_pending_list(contextP, watcherP, watcherP->format, buffer, length);
              watcherP->store_count++; // Increment the store notify count
              watcherP->lastTime = currentTime; //To avoid observe_step from being called immediately
              qapi_Coap_Free_Message(watcherP->server->coapHandle,message);
              message = NULL;
            }
            else
            {
              ds_iot_ulog_high("Skip sending observe operation as device is disabled");
              qapi_Coap_Free_Message(watcherP->server->coapHandle,message);
              message = NULL;
			  goto hdl_error;
            }
          }
          // check if the device is in sleep state
          else if(lwm2m_in_sleep_state)
          {
            // If the observation request is for resource level update the last value.
            if(storeValue)
            {
              switch (dataP->type)
              {
                case LWM2M_TYPE_INTEGER:
                if(integerValue != watcherP->lastValue.asInteger)
                {
                  watcherP->lastValue.asInteger = integerValue;
                }
                break;
                case LWM2M_TYPE_FLOAT:
                if(floatValue != watcherP->lastValue.asFloat)
                {
                  watcherP->lastValue.asFloat = floatValue;
                }
                break;
              }
			  ds_iot_ulog_high3("storeValue[%d], dataP->type [%d], Value Changed[%d]", storeValue, dataP->type, watcherP->update);
            }
            //Add into notify pending list
            ds_iot_ulog_high1("Adding notifications of watcher %p into pending list for sleep state and send Reg update",watcherP);
            observer_add_notify_to_pending_list(contextP, watcherP, watcherP->format, buffer, length);
            watcherP->store_count++;  // Increment the store notify count
            //Send out registration update to move device out of sleep state.
            if (!watcherP->server->is_update_for_wakeup )
            {
              ds_iot_ulog_high("Sending Reg update");
              lwm2m_update_registration(contextP, watcherP->server->shortID, LWM2M_FALSE);
              watcherP->server->is_update_for_wakeup = LWM2M_TRUE;
            }
            watcherP->lastTime = currentTime; //To avoid observe_step from being called immediately
            qapi_Coap_Free_Message(watcherP->server->coapHandle,message);
            message = NULL;
          } 
          else
          {
            // Send notifications
            if(STATE_REGISTERED == watcherP->server->status)
            {       
              watcherP->lastTime = currentTime;
              #ifdef PSM_LWM2M
			  result = lwm2m_psm_operation(contextP, watcherP->server->shortID, &uri,
                                               PSM_NODE_OBSERVATION, PSM_NODE_UPDATE, PSM_MODIFY_NODE);

              #endif

              qapi_Coap_Set_Header(watcherP->server->coapHandle,message ,QAPI_COAP_TOKEN ,watcherP->token, watcherP->tokenLen); 
              qapi_Coap_Set_Header(watcherP->server->coapHandle,message ,QAPI_COAP_OBSERVE , &(watcherP->counter) , 0 ) ;

              watcherP->counter++; 

              message_params.lastmid = &watcherP->lastMid ;

              ds_iot_ulog_high2("Sending notify on watcher: %p to %d server",watcherP,watcherP->server->shortID);

              if (qapi_Coap_Send_Message(watcherP->server->coapHandle,message, &message_params) != 0)
              {
                lwm2m_clean_coap_session(watcherP->server);
              }

              active_time = lwm2m_gettime();
              ds_iot_ulog_high1("Last active time - %u", active_time);

              qapi_Coap_Free_Message(watcherP->server->coapHandle,message);
              message = NULL;
              watcherP->update = false;
              watcherP->pmin_wait_count = 0;  // Reset counter.
              #ifdef PSM_LWM2M
			  result = lwm2m_psm_operation(contextP, 0, &uri, PSM_NODE_TYPE_MIN, PSM_NODE_UPDATE_ALL, PSM_MODIFY_NODE);

              #endif
              // Sync the latest observation info to persistent storage
              if(persis_regstatus_feat_enabled == LWM2M_TRUE)
              {
                sync_observations_to_persistence(contextP);
              }

              if ( (notify == true) && (storeValue == true) )
              {
                switch (dataP->type)
                {
                  case LWM2M_TYPE_INTEGER:
                    watcherP->lastValue.asInteger = integerValue;
                    break;
                  case LWM2M_TYPE_FLOAT:
                    watcherP->lastValue.asFloat = floatValue;
                    break;
                  default:
                    break;
                }
              }

              update_timeout_val_with_period(watcherP, timeoutP, currentTime, min_period);

            }
            else
            {
              qapi_Coap_Free_Message(watcherP->server->coapHandle,message);
              message = NULL;
            }
          }
         
        } // notify == true

        /* For all observed resources store Pmin interval count. This field would
           be used in PSM time computation.
           For ex 
           If Pmin is 10 then at 24th second pmin_wait_count would be 2 implying 
           2 times pmin interval crossed.*/
        if((notify == false) && (watcherP->update == false))
        {
          if(currentTime > watcherP->lastTime)
          {
            uint8_t pmin_cnt = watcherP->pmin_wait_count;
            watcherP->pmin_wait_count = ((currentTime - watcherP->lastTime)/min_period);
            LOG_DEBUG("pmin_wait_count is %d",watcherP->pmin_wait_count);
            if(lwm2m_psm_enable && contextP->lwm2m_psm_info->psm_enabled == TRUE && 
               contextP->lwm2m_psm_info->pmin_pmax_awake == FALSE)
            {
              if(pmin_cnt != watcherP->pmin_wait_count)
            {
                efs_call = true;
              }
            }
          }
        }
hdl_error:
        if (buffer != NULL) 
        {   
            lwm2m_free(buffer);
            buffer = NULL;
        }

		if (dataP != NULL)
		{	
		   lwm2m_data_free(size, dataP);
		   dataP = NULL;
		}   
		
        memset(&uri, 0x00, sizeof(lwm2m_uri_t));
        memcpy(&uri, &targetP->uri, sizeof(lwm2m_uri_t));
        // Get the inherited Pmin for watcher if Pmin for current watcher is not set.
        min_period = get_pmin_value(contextP, watcherP, uri);
         ds_iot_ulog_high1("Min period is %d",min_period);

        update_timeout_val_with_period(watcherP, timeoutP, currentTime, min_period);
    } //for watcher list
  } // End of //Process observation List

  // Sync the latest observation info to persistent storage
  if(efs_call == true && persis_regstatus_feat_enabled == LWM2M_TRUE)
  {
    ds_iot_ulog_high("Change in 1 of the pmin wait count.");
    sync_observations_to_persistence(contextP);
  }
}

#endif


/******************************************************************************

  @file    management.c

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
 *    domedambrosio - Please refer to git log
 *    Toby Jaffey - Please refer to git log
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
#include <stdio.h>
#include "lwm2m_signals.h"
#include "ds_log.h"



#ifdef AUTHORIZATION_SUPPORT
#include "object_access_control.h"
#endif

#define LOCATION_PATH_LEN 13

#ifdef LWM2M_SERVER_OBJECT
extern uint16_t short_serverId;
#endif

extern bool enable_acl_obj;
extern uint32_t active_time;
bool gInboundFirwareTransferComplete = false;
extern bool persis_regstatus_feat_enabled;
extern uint32_t lh_location_single_shot(void);
extern bool active_single_shot;
extern uint32_t trackingSessionId;
extern q_type lwm2m_location_pending_read_observe;
extern qurt_signal_t lwm2m_signal_Handle;
extern bool enable_location;



#ifdef LWM2M_CLIENT_MODE
/**
 * @fn static int prv_readAttributes()
 * @brief This function is used to read attributes
 *
 * @param query pointer to multi_option_t
 * @param attrP pointer to attribute structure
 *
 * @return    0 on success
 *        -1 on failure
 */
static int prv_readAttributes(qapi_Multi_Option_t * query,
    lwm2m_attributes_t * attrP)
{
  int64_t intValue = 0;
  double floatValue = 0;

  if(attrP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return -1;
  }
  memset(attrP, 0, sizeof(lwm2m_attributes_t));

  while (query != NULL)
  {
    if (lwm2m_strncmp((char *)query->data, ATTR_MIN_PERIOD_STR, ATTR_MIN_PERIOD_LEN) == 0)
    {
      if (0 != ((attrP->toSet | attrP->toClear) & LWM2M_ATTR_FLAG_MIN_PERIOD)) return -1;
      if (query->len == ATTR_MIN_PERIOD_LEN) return -1;

      if (1 != utils_plainTextToInt64(query->data + ATTR_MIN_PERIOD_LEN, query->len - ATTR_MIN_PERIOD_LEN, &intValue)) return -1;
      if (intValue < 0) return -1;

      attrP->toSet |= LWM2M_ATTR_FLAG_MIN_PERIOD;
      attrP->minPeriod = intValue;
    }
    else if (lwm2m_strncmp((char *)query->data, ATTR_MIN_PERIOD_STR, ATTR_MIN_PERIOD_LEN - 1) == 0)
    {
      if (0 != ((attrP->toSet | attrP->toClear) & LWM2M_ATTR_FLAG_MIN_PERIOD)) return -1;
      if (query->len != ATTR_MIN_PERIOD_LEN - 1) return -1;

      attrP->toClear |= LWM2M_ATTR_FLAG_MIN_PERIOD;
    }
    else if (lwm2m_strncmp((char *)query->data, ATTR_MAX_PERIOD_STR, ATTR_MAX_PERIOD_LEN) == 0)
    {
      if (0 != ((attrP->toSet | attrP->toClear) & LWM2M_ATTR_FLAG_MAX_PERIOD)) return -1;
      if (query->len == ATTR_MAX_PERIOD_LEN) return -1;

      if (1 != utils_plainTextToInt64(query->data + ATTR_MAX_PERIOD_LEN, query->len - ATTR_MAX_PERIOD_LEN, &intValue)) return -1;
      if (intValue < 0) return -1;

      attrP->toSet |= LWM2M_ATTR_FLAG_MAX_PERIOD;
      attrP->maxPeriod = intValue;
    }
    else if (lwm2m_strncmp((char *)query->data, ATTR_MAX_PERIOD_STR, ATTR_MAX_PERIOD_LEN - 1) == 0)
    {
      if (0 != ((attrP->toSet | attrP->toClear) & LWM2M_ATTR_FLAG_MAX_PERIOD)) return -1;
      if (query->len != ATTR_MAX_PERIOD_LEN - 1) return -1;

      attrP->toClear |= LWM2M_ATTR_FLAG_MAX_PERIOD;
    }
    else if (lwm2m_strncmp((char *)query->data, ATTR_GREATER_THAN_STR, ATTR_GREATER_THAN_LEN) == 0)
    {
      if (0 != ((attrP->toSet | attrP->toClear) & LWM2M_ATTR_FLAG_GREATER_THAN)) return -1;
      if (query->len == ATTR_GREATER_THAN_LEN) return -1;

      if (1 != utils_plainTextToFloat64(query->data + ATTR_GREATER_THAN_LEN, query->len - ATTR_GREATER_THAN_LEN, &floatValue)) return -1;

      attrP->toSet |= LWM2M_ATTR_FLAG_GREATER_THAN;
      attrP->greaterThan = floatValue;
    }
    else if (lwm2m_strncmp((char *)query->data, ATTR_GREATER_THAN_STR, ATTR_GREATER_THAN_LEN - 1) == 0)
    {
      if (0 != ((attrP->toSet | attrP->toClear) & LWM2M_ATTR_FLAG_GREATER_THAN)) return -1;
      if (query->len != ATTR_GREATER_THAN_LEN - 1) return -1;

      attrP->toClear |= LWM2M_ATTR_FLAG_GREATER_THAN;
    }
    else if (lwm2m_strncmp((char *)query->data, ATTR_LESS_THAN_STR, ATTR_LESS_THAN_LEN) == 0)
    {
      if (0 != ((attrP->toSet | attrP->toClear) & LWM2M_ATTR_FLAG_LESS_THAN)) return -1;
      if (query->len == ATTR_LESS_THAN_LEN) return -1;

      if (1 != utils_plainTextToFloat64(query->data + ATTR_LESS_THAN_LEN, query->len - ATTR_LESS_THAN_LEN, &floatValue)) return -1;

      attrP->toSet |= LWM2M_ATTR_FLAG_LESS_THAN;
      attrP->lessThan = floatValue;
    }
    else if (lwm2m_strncmp((char *)query->data, ATTR_LESS_THAN_STR, ATTR_LESS_THAN_LEN - 1) == 0)
    {
      if (0 != ((attrP->toSet | attrP->toClear) & LWM2M_ATTR_FLAG_LESS_THAN)) return -1;
      if (query->len != ATTR_LESS_THAN_LEN - 1) return -1;

      attrP->toClear |= LWM2M_ATTR_FLAG_LESS_THAN;
    }
    else if (lwm2m_strncmp((char *)query->data, ATTR_STEP_STR, ATTR_STEP_LEN) == 0)
    {
      if (0 != ((attrP->toSet | attrP->toClear) & LWM2M_ATTR_FLAG_STEP)) return -1;
      if (query->len == ATTR_STEP_LEN) return -1;

      if (1 != utils_plainTextToFloat64(query->data + ATTR_STEP_LEN, query->len - ATTR_STEP_LEN, &floatValue)) return -1;
      if (floatValue < 0) return -1;

      attrP->toSet |= LWM2M_ATTR_FLAG_STEP;
      attrP->step = floatValue;
    }
    else if (lwm2m_strncmp((char *)query->data, ATTR_STEP_STR, ATTR_STEP_LEN - 1) == 0)
    {
      if (0 != ((attrP->toSet | attrP->toClear) & LWM2M_ATTR_FLAG_STEP)) return -1;
      if (query->len != ATTR_STEP_LEN - 1) return -1;

      attrP->toClear |= LWM2M_ATTR_FLAG_STEP;
    }
    else return -1;

    query = query->next;
  }

  return 0;
}

/**
 * @fn uint8_t dm_handleRequest()
 * @brief This function is used to handle read, write,observe and 
 *        notify requests
 *
 * @param contextP handle to lwm2m context
 * @param uriP  handle to resource uri
 * @param serverP  handle to lwm2m server
 * @param message  handle to coap packet
 * @param response  handle to coap response packet
 *
 * @return  LWM2M response type
 */
uint8_t dm_handleRequest(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    lwm2m_server_t * serverP,
    qapi_Coap_Packet_t * message,
    qapi_Coap_Packet_t * response)
{
  lwm2m_media_type_t format;
  bool               cont_type_set = false;
  uint8_t            result        = LWM2M_NO_ERROR;

  if ( contextP == NULL || uriP == NULL || serverP == NULL
      || message == NULL || response == NULL )
  { 
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  LOG_DEBUG("Code: %02X, server status: %s", message->code, STR_STATUS(serverP->status));
  LOG_URI(uriP);

  if(QAPI_IS_OPTION(message, QAPI_COAP_OPTION_CONTENT_TYPE))
  {
    cont_type_set = true;
  }

  format = utils_convertMediaType(message->content_type);


  /* OMA v1.1:
   * The LwM2M Client MUST ignore LwM2M Server operations on this interface
   * until the registration procedure with the respective LwM2M Server is completed.
   */
  if (serverP->status != STATE_REGISTERED
      && serverP->status != STATE_REG_UPDATE_PENDING)
  {
    return (lwm2m_status_t)LWM2M_IGNORE;
  }

  if(!LWM2M_URI_IS_SET_OBJECT(uriP))
      return LWM2M_405_METHOD_NOT_ALLOWED;

  /* OMA v1.1:
     The LwM2M Client MUST reject any LwM2M Server operation on the Security
     Object (ID: 0) with an "4.01 Unauthorized" response code
   */
  if (uriP->objectId == LWM2M_SECURITY_OBJECT_ID)
  {
    return (uint8_t) LWM2M_401_UNAUTHORIZED;
  }

  /* OMA v1.1:
  * 1. 6.3.7. Delete Operation:
       The "Delete" operation is used for LwM2M Server to delete an Object
       Instance within the LwM2M Client.
  * 2. 6.3.5. Execute Operation:
       The "Execute" operation is used by the LwM2M Server to initiate some action,
       and can only be performed on individual Resources. A LwM2M Client MUST
       return an error when the "Execute" operation is received for an Object
       Instance(s) or Resource Instance(s).
  * 3. 8.2.3. Operation on Object
       If a given LwM2M Server targets an Object with a "Write", "Execute", or
       "Delete" operation, the LwM2M Client MUST NOT perform such an operation
       and MUST send a "Method Not Allowed" error code to the LwM2M Server.
 */
  //DELETE on object level or resource level
  if(message->code == QAPI_COAP_DELETE &&
     (!LWM2M_URI_IS_SET_INSTANCE(uriP) || LWM2M_URI_IS_SET_RESOURCE(uriP)))
      return LWM2M_405_METHOD_NOT_ALLOWED;

  //EXECUTE on object level or object instance level or resource instence level
  if(message->code == QAPI_COAP_POST && 
     !(cont_type_set && (!LWM2M_URI_IS_SET_INSTANCE(uriP) ||   /*create*/
                         !LWM2M_URI_IS_SET_RESOURCE(uriP))))   /*write*/
  {
    if(!LWM2M_URI_IS_SET_INSTANCE(uriP) ||    /*object level*/
       !LWM2M_URI_IS_SET_RESOURCE(uriP) ||    /*object instance level*/
        LWM2M_URI_IS_SET_RESOURCE_INST(uriP)) /*resource instance level*/
      return LWM2M_405_METHOD_NOT_ALLOWED;
  }

  if(!object_isObjOrObjInstanceExist(contextP, uriP))
    return LWM2M_404_NOT_FOUND;

  // activerserverP is used to check permission of the server.
  contextP->activeServerP = serverP;
  switch (message->code)
  {
    case QAPI_COAP_GET:
      {
        uint8_t * buffer = NULL;
        size_t length = 0;

        if (QAPI_IS_OPTION(message, QAPI_COAP_OPTION_OBSERVE))
        {
          lwm2m_data_t * dataP = NULL;
          int size = 0;
          int *obs_type = (int *)lwm2m_calloc(1,sizeof(int));
          int obs_type_len = 0;
          lwm2m_media_type_t accept = LWM2M_CONTENT_UNSUPPORTED;

          if(!obs_type)
          {
            return LWM2M_500_INTERNAL_SERVER_ERROR;
          }

          qapi_Coap_Get_Header(serverP->coapHandle, message , QAPI_COAP_OBSERVE , (void **)&obs_type , (size_t *)&obs_type_len);

          if(obs_type)
          {
            ds_iot_ulog_high1("dm_handleRequest: Observation request: obs_type %d ", *obs_type);
          }
          result = object_readData(contextP, uriP, &size, &dataP, serverP);
          if (LWM2M_205_CONTENT == result)
          {
            ds_iot_ulog_high(" Observation for ");
            PRINT_URI(uriP);
            result = observe_handleRequest(contextP, uriP, serverP, size, dataP, message, response);
            ds_iot_ulog_high2(" Response %u.%02u recieved for observation on",(result & 0xFF) >> 5, (result & 0x1F));
            if (LWM2M_205_CONTENT == result)
            {
              if (QAPI_IS_OPTION(message, QAPI_COAP_OPTION_ACCEPT))
              {
                format = utils_convertMediaType(message->accept[0]);
                accept = utils_convertMediaType(message->accept[0]);
              }
              if(format == LWM2M_CONTENT_UNSUPPORTED)
              {
                if(dataP)
                lwm2m_data_free(size, dataP);
                if(obs_type)
                {
                    lwm2m_free(obs_type);
                    obs_type = NULL;
                }
                return LWM2M_406_NOT_ACCEPTABLE;
              }
              length = lwm2m_data_serialize(uriP, size, dataP, &format, &buffer);
              if (length == 0)
              {
                if(format == LWM2M_CONTENT_OPAQUE)
                  result = (uint8_t) LWM2M_406_NOT_ACCEPTABLE;
                else 
                 result = (uint8_t) LWM2M_500_INTERNAL_SERVER_ERROR;
              }
              else if (dataP && (accept == LWM2M_CONTENT_TEXT
                                 || accept == LWM2M_CONTENT_OPAQUE))
              {
                if (size != 1
                    || !LWM2M_URI_IS_SET_RESOURCE(uriP)
                    || dataP->type == LWM2M_TYPE_OBJECT
                    || dataP->type == LWM2M_TYPE_OBJECT_INSTANCE
                    || dataP->type == LWM2M_TYPE_MULTIPLE_RESOURCE)
                {
                  result = (uint8_t) LWM2M_406_NOT_ACCEPTABLE;
                }
              }
              else
              {
                LOG_DEBUG("Observe Request[/%d/%d/%d]: %.*s", uriP->objectId, uriP->instanceId, uriP->resourceId, length, buffer);
                if (obs_type && uriP->objectId == LWM2M_LOCATION_OBJECT_ID && *obs_type == 0)
                {
                  if(!enable_location)
                  {
                     result = LWM2M_404_NOT_FOUND;
                  }
                  else
                  {
                    lwm2m_location_read_observe_pending_t *obs_pend = NULL;
                    ds_iot_ulog_high("Location observe request  Queuing observe request");
                    obs_pend = (lwm2m_location_read_observe_pending_t *)lwm2m_malloc(sizeof(lwm2m_location_read_observe_pending_t));
                    if(obs_pend == NULL)
                    {
                       LOG_DEBUG("%s queue obs request ",mallocError);
                       if(dataP)
                       lwm2m_data_free(size, dataP);
                       if(obs_type)
                       {
                         lwm2m_free(obs_type);
                         obs_type = NULL;
                       }
                       return LWM2M_500_INTERNAL_SERVER_ERROR;
                    }
                    memset(obs_pend, 0, sizeof(lwm2m_location_read_observe_pending_t));
                    obs_pend->observe_req = 1;
                    memcpy(&obs_pend->uriP, uriP,sizeof(lwm2m_uri_t));
                    obs_pend->format=format;
                    obs_pend->accept = accept;
                    obs_pend->ssid = serverP->shortID;
                    obs_pend->token_len = message->token_len;
                    memcpy(obs_pend->token, message->token, obs_pend->token_len);
        
                    q_put(&lwm2m_location_pending_read_observe, &(obs_pend->link));
  
                    qurt_signal_set(&lwm2m_signal_Handle, LOCATION_FETCH);
                    result = LWM2M_NO_ERROR;
                  }
                }

                 if(obs_type)
                 {
                    lwm2m_free(obs_type);
                    obs_type = NULL;
                 }
                // Sync the latest observation info to persistent storage
                if(persis_regstatus_feat_enabled == LWM2M_TRUE)
                {
                  sync_observations_to_persistence(contextP);
                }
              }
            }
            //lwm2m_data_free(size, dataP); //To fix memory leak issue
          }
          lwm2m_data_free(size, dataP);
        }
        else if (QAPI_IS_OPTION(message, QAPI_COAP_OPTION_ACCEPT)
            && message->accept_num == 1
            && message->accept[0] == QAPI_APPLICATION_LINK_FORMAT)
        {
          format = (lwm2m_media_type_t) QAPI_APPLICATION_LINK_FORMAT;
          ds_iot_ulog_high("Discover for");
          PRINT_URI(uriP);

          result = object_discover(contextP, uriP, &buffer, &length, serverP);

          //LOG_INFO(" Response %u.%02u recieved for discover on",(result & 0xFF) >> 5, (result & 0x1F));// Already printed in process pkt
          //PRINT_URI(uriP);This log already printed before object_discover
        }
        else
        {
          lwm2m_data_t * dataP = NULL;
          int size = 0;
          lwm2m_media_type_t accept = LWM2M_CONTENT_UNSUPPORTED;
          if (QAPI_IS_OPTION(message, QAPI_COAP_OPTION_ACCEPT))
          {
            format = utils_convertMediaType(message->accept[0]);
            accept = utils_convertMediaType(message->accept[0]);
          }
          if(format == LWM2M_CONTENT_UNSUPPORTED)
            return LWM2M_406_NOT_ACCEPTABLE;

          if (uriP->objectId == LWM2M_LOCATION_OBJECT_ID)
          {
            if(!enable_location)
            {
               result = LWM2M_404_NOT_FOUND;
            }
            else
            {
#ifdef AUTHORIZATION_SUPPORT
              result =  checkAuthForRead(contextP,uriP);
#endif
              if(result != LWM2M_401_UNAUTHORIZED)
              {
                lwm2m_location_read_observe_pending_t *read_pend = NULL;
                ds_iot_ulog_high("Queuing read request");
                read_pend = (lwm2m_location_read_observe_pending_t *)lwm2m_malloc(sizeof(lwm2m_location_read_observe_pending_t));
                if(read_pend == NULL)
                {
                   LOG_DEBUG(" %s queue read request ", mallocError);
                   return LWM2M_500_INTERNAL_SERVER_ERROR;
                }
                memset(read_pend, 0, sizeof(lwm2m_location_read_observe_pending_t));
                memcpy(&read_pend->uriP, uriP,sizeof(lwm2m_uri_t));
                read_pend->format=format;
                if (QAPI_IS_OPTION(message, QAPI_COAP_OPTION_ACCEPT))
                {
                  read_pend->accept = utils_convertMediaType(message->accept[0]);
                }
                else
                {
                  read_pend->accept = LWM2M_CONTENT_UNSUPPORTED;
                }
                read_pend->ssid = serverP->shortID;
                read_pend->token_len = message->token_len;
                memcpy(read_pend->token, message->token, read_pend->token_len);
    
                q_put(&lwm2m_location_pending_read_observe, &(read_pend->link));
  
                qurt_signal_set(&lwm2m_signal_Handle, LOCATION_FETCH);
                result = LWM2M_NO_ERROR;
              }
            }
          }
          else
          {
            result = object_read(contextP, uriP, &format, &buffer, &length, &dataP, &size, serverP);
            if(dataP == NULL)
            {
              ds_iot_ulog_err("Object read failed ");
            }
            if (dataP && (accept == LWM2M_CONTENT_TEXT
                          || accept == LWM2M_CONTENT_OPAQUE))
            {
              if (size != 1
                  || !LWM2M_URI_IS_SET_RESOURCE(uriP)
                  || dataP->type == LWM2M_TYPE_OBJECT
                  || dataP->type == LWM2M_TYPE_OBJECT_INSTANCE
                  || dataP->type == LWM2M_TYPE_MULTIPLE_RESOURCE)
              {
                result = LWM2M_406_NOT_ACCEPTABLE;
              }
            }
            if(dataP)
            {
              lwm2m_data_free(size, dataP);
              dataP = NULL;
            }
          }
        }

        if (LWM2M_205_CONTENT == result)
        {
          qapi_Coap_Set_Header(serverP->coapHandle,response ,QAPI_COAP_CONTENT_TYPE,&format,0);
          qapi_Coap_Set_Payload(serverP->coapHandle, response, buffer, length);
        }
        lwm2m_free(buffer);
      }
      break;

    case QAPI_COAP_POST: 
      {
        if ((!LWM2M_URI_IS_SET_INSTANCE(uriP)) && cont_type_set)
        {
          if(format == LWM2M_CONTENT_UNSUPPORTED)
            result = (uint8_t) LWM2M_415_UNSUPPORTED_CONTENT_FORMAT;
          else
          {
#ifdef AUTHORIZATION_SUPPORT
           acc_ctrl_oi_t     *targetP = NULL;
           lwm2m_object_t    *objectP = NULL;
           int                server_count = 0;
#endif
           result = object_create(contextP, uriP, format, message->payload, message->payload_len);
           if (result == LWM2M_201_CREATED ||
               (uriP->objectId == LWM2M_APN_CONN_PROFILE_OBJECT_ID && LWM2M_NO_ERROR == result))
           {
             //longest uri is /65535/65535 = 12 + 1 (null) chars
             char location_path[LOCATION_PATH_LEN] = "";
             //instanceId expected
             if ((uriP->flag & LWM2M_URI_FLAG_INSTANCE_ID) == 0)
             {
               result = (uint8_t) LWM2M_500_INTERNAL_SERVER_ERROR;                   
               break;
             }
             if (snprintf(location_path, LOCATION_PATH_LEN, "/%d/%d", uriP->objectId, uriP->instanceId) < 0)
             {
               result = (uint8_t) LWM2M_500_INTERNAL_SERVER_ERROR;
               break;
             }
#ifdef AUTHORIZATION_SUPPORT
             /*For multiple server scenario:
               <<OMA 1.1>> 6.3.6. Create
               "When there is no reference to Object Instance in the TLV/CBOR/JSON
               payload of the "Create" command, the LwM2M Client MUST assigns the
               ID of the created Object Instance. If a new Object Instance is created
               through that operation and the LwM2M Client has more than one LwM2M
               Server Account, then the LwM2M Client creates an Access Control Object
               Instance for the created Object Instance (section 8. Access Control)
                  * Access Control Owner MUST be the LwM2M Server
                  * The LwM2M Server MUST have full access rights"
             */
             do
             {
               if(LWM2M_ACL_OBJECT_ID == uriP->objectId) break;
               server_count = lwm2m_get_server_count(contextP, FALSE);
               if(server_count > 1)
               {
                 objectP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, LWM2M_ACL_OBJECT_ID);
                 if(NULL == objectP) break;
                 targetP = acl_create_obj_inst_with_access_right(objectP,
                                                lwm2m_list_newId(objectP->instanceList),
                                                uriP->objectId,
                                                uriP->instanceId,
                                                contextP->activeServerP->shortID,
                                                true,
                                                ACC_CTRL_PERM_FULL_RIGHT);

                 if (NULL == targetP) break;
                 update_acl_specific_list(contextP, objectP);
               }
             }while(0);
#endif

             qapi_Coap_Set_Header(serverP->coapHandle ,response , QAPI_COAP_LOCATION_PATH ,location_path ,0);
             lwm2m_update_registration(contextP, 0, true);
           }
          }
        }
        else if ((!LWM2M_URI_IS_SET_RESOURCE(uriP)) && cont_type_set)
        {
          if(format == LWM2M_CONTENT_UNSUPPORTED)
            result = (uint8_t) LWM2M_415_UNSUPPORTED_CONTENT_FORMAT;
          else
          {
          result = object_write(contextP, uriP, format, message, message->payload_len, message->code, FALSE);
          if (LWM2M_204_CHANGED == result || 
              (uriP->objectId == LWM2M_APN_CONN_PROFILE_OBJECT_ID && LWM2M_NO_ERROR == result))
            lwm2m_resource_value_changed(contextP, uriP);
#ifdef LWM2M_SERVER_OBJECT
            object_UpdateServerList(contextP, short_serverId);
#endif
          }
        }
        else
        {
          ds_iot_ulog_high("Execute operation on");
          PRINT_URI(uriP);
          result = object_execute(contextP, uriP, message->payload, message->payload_len);
#ifdef LWM2M_SERVER_OBJECT
          object_UpdateServerList(contextP, short_serverId);
#endif        
        }
      }
      break;

    case QAPI_COAP_PUT:
      {
        if (QAPI_IS_OPTION(message, QAPI_COAP_OPTION_URI_QUERY))
        {
          lwm2m_attributes_t attr;

          if (0 != prv_readAttributes(message->uri_query, &attr))
          {
            result = (uint8_t) LWM2M_400_BAD_REQUEST;
          }
          else
          {
#ifdef AUTHORIZATION_SUPPORT
            if(LWM2M_URI_IS_SET_INSTANCE(uriP))
              result = checkAuthForRead(contextP,uriP);
#endif
            if(result != LWM2M_401_UNAUTHORIZED){
              ds_iot_ulog_high("SET ATTRIBUTES on");
              PRINT_URI(uriP);
              result = observe_setParameters(contextP, uriP, serverP, &attr);
            }
          }
        }
        else if (LWM2M_URI_IS_SET_INSTANCE(uriP) && cont_type_set)
        {
          if(format == LWM2M_CONTENT_UNSUPPORTED)
            result = (uint8_t) LWM2M_415_UNSUPPORTED_CONTENT_FORMAT;
          else
          {
           if ( (LWM2M_FIRMWARE_UPDATE_OBJECT_ID == uriP->objectId 
                 && LWM2M_TRUE != message->block1_more) || (LWM2M_SOFTWARE_MGNT_OBJECT_ID == uriP->objectId 
                   && LWM2M_TRUE != message->block1_more))
           {
             gInboundFirwareTransferComplete = true;
           }
          result = object_write(contextP, uriP, format, message, message->payload_len, message->code, FALSE);
          if (LWM2M_204_CHANGED == result ||
              (uriP->objectId == LWM2M_APN_CONN_PROFILE_OBJECT_ID && LWM2M_NO_ERROR == result))
            lwm2m_resource_value_changed(contextP, uriP);
#ifdef LWM2M_SERVER_OBJECT
           object_UpdateServerList(contextP, short_serverId);
#endif
          }
        }
        else
        {
          result = (uint8_t) LWM2M_400_BAD_REQUEST;
        }
      }
      break;

    case QAPI_COAP_DELETE:
      {
        result = object_delete(contextP, uriP);
        if (result == LWM2M_202_DELETED)
        {
          lwm2m_update_registration(contextP, 0, true);
        }
      }
      break;

    default:
      result = (uint8_t) LWM2M_400_BAD_REQUEST;
      break;
  }
  contextP->activeServerP = NULL;
  return result;
}

/**
 * @fn uint8_t dm_handleRequestExObj()
 * @brief Function is used to handle read, write,observe and 
 *        notify requests for extensible objects. 
 *
 * @param contextP handle to lwm2m context
 * @param uriP  handle to resource uri
 * @param serverP  handle to lwm2m server
 * @param message  handle to coap packet
 * @param response  handle to coap response packet
 *
 * @return  LWM2M response type
 */
uint8_t dm_handleRequest_ExObj(lwm2m_context_t *contextP,
    lwm2m_uri_t *uriP,
    lwm2m_server_t *serverP,
    qapi_Coap_Packet_t *message,
    qapi_Coap_Packet_t *response)
{
  uint8_t result = LWM2M_NO_ERROR;
  lwm2m_server_data_t lwm2m_info;
  bool cont_type_set = false;

  /* Validate the input paramters */
  if ((contextP == NULL) ||
      (uriP == NULL)     ||
      (serverP == NULL)  ||
      (message == NULL)  ||
      (response == NULL))
  { 
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  LOG_DEBUG("Code: %02X, server status: %s", message->code, STR_STATUS(serverP->status));
  LOG_URI(uriP);

  if ((serverP->status != STATE_REGISTERED) &&
      (serverP->status != STATE_REG_UPDATE_PENDING))
  {
    return (lwm2m_status_t)LWM2M_IGNORE;
  }

  if(QAPI_IS_OPTION(message, QAPI_COAP_OPTION_CONTENT_TYPE))
  {
    cont_type_set = true;
  }
  /* activerserverP is used to check permission of the server */
  contextP->activeServerP = serverP;

  memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t));

  /* Object info */
  if (uriP->flag & LWM2M_URI_FLAG_OBJECT_ID)
  {
    lwm2m_info.obj_info.obj_id = uriP->objectId;
    lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_OBJECT_ID_E;
  }

  /* Object instance info */
  if (uriP->flag & LWM2M_URI_FLAG_INSTANCE_ID)
  {
    lwm2m_info.obj_info.obj_inst_id = uriP->instanceId;
    lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_INSTANCE_ID_E;
  }

  /* Resource info */
  if (uriP->flag & LWM2M_URI_FLAG_RESOURCE_ID)
  {
    lwm2m_info.obj_info.res_id = uriP->resourceId;
    lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_RESOURCE_ID_E;
  }

  /* Resource instance info */
  if (uriP->flag & LWM2M_URI_FLAG_RESOURCE_INST_ID)
  {
    lwm2m_info.obj_info.res_inst_id = uriP->resourceInstId;
    lwm2m_info.obj_info.obj_mask |= QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E;
  }

  /* Message ID comprises of Token ID + Short Server ID */
  lwm2m_info.msg_id_len = message->token_len;
  memcpy(lwm2m_info.msg_id, message->token, message->token_len);
  memcpy(&lwm2m_info.msg_id[lwm2m_info.msg_id_len], &serverP->shortID, 0x02);
  lwm2m_info.msg_id_len += 0x02;
 
  /* LWM2M payload content type */ 
  lwm2m_info.content_type = (qapi_Net_LWM2M_Content_Type_t)message->content_type;
  if ((message->code == QAPI_COAP_GET) && (QAPI_IS_OPTION(message, QAPI_COAP_OPTION_ACCEPT)))
  {
    lwm2m_info.content_type = (qapi_Net_LWM2M_Content_Type_t)message->accept[0];
  }

  /* LWM2M accept type */ 
  if((QAPI_IS_OPTION(message, QAPI_COAP_OPTION_ACCEPT)))
  {
    lwm2m_info.accept_is_valid = TRUE;
    lwm2m_info.accept = (qapi_Net_LWM2M_Content_Type_t)message->accept[0];
  }

  /* LWM2M payload length */
  lwm2m_info.payload_len = message->payload_len;
  /* LWM2M payload */
  lwm2m_info.payload = message->payload; 

  /* LWM2M message request type */ 
  switch (message->code)
  {
    case QAPI_COAP_GET:
    {
      /* Observe option */ 
      if (QAPI_IS_OPTION(message, QAPI_COAP_OPTION_OBSERVE))
      {
        uint32_t  * count = (uint32_t *)lwm2m_calloc(1,sizeof(uint32_t));
        uint32_t count_len = 0;
        if(!count)
        return LWM2M_500_INTERNAL_SERVER_ERROR;

        ds_iot_ulog_high("Observation operation on");
        PRINT_URI(uriP);

        /* Observe option from the header */ 
         qapi_Coap_Get_Header(serverP->coapHandle, message, QAPI_COAP_OBSERVE, (void **)&count, (size_t *)&count_len);

        /* Validate the observe permission information */
        if(count)
        {
         lwm2m_info.msg_type = (*count) ? QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E : QAPI_NET_LWM2M_OBSERVE_REQ_E;
        }

        if(count)
        {
           lwm2m_free(count);
           count = NULL;
        }
        /* Set the notification/message ID */
        lwm2m_info.notification_id = message->mid;
        result = validate_ex_object_permissions(contextP, uriP, lwm2m_info.msg_type);
      }
      /* Discover option */
      else if ((QAPI_IS_OPTION(message, QAPI_COAP_OPTION_ACCEPT)) &&
               (0x01 == message->accept_num) &&
               (message->accept[0] == QAPI_APPLICATION_LINK_FORMAT))
      {
        ds_iot_ulog_high("Discover operation on");
        PRINT_URI(uriP);
        lwm2m_info.msg_type = QAPI_NET_LWM2M_DISCOVER_REQ_E;
        lwm2m_info.content_type = (qapi_Net_LWM2M_Content_Type_t)QAPI_APPLICATION_LINK_FORMAT;
      }
      else
      {
        /* Read operation */
        ds_iot_ulog_high("Read operation on");
        PRINT_URI(uriP);

        /* Validate the read permission information */        
        lwm2m_info.msg_type = QAPI_NET_LWM2M_READ_REQ_E;
        result = validate_ex_object_permissions(contextP, uriP, lwm2m_info.msg_type);            
      }
      
      break;
    }

    case QAPI_COAP_POST: 
    {
      /* Create operation */ 
      if (!LWM2M_URI_IS_SET_INSTANCE(uriP) && cont_type_set)
      {
        ds_iot_ulog_high("Create operation on");
        PRINT_URI(uriP);

        /* Validate the create permission information */        
        lwm2m_info.msg_type = QAPI_NET_LWM2M_CREATE_REQ_E;
        result = validate_ex_object_permissions(contextP, uriP, lwm2m_info.msg_type);        
      }
      /* Write operation - Partial update */ 
      else if (!LWM2M_URI_IS_SET_RESOURCE(uriP) && cont_type_set)
      {
        ds_iot_ulog_high("Write (partial update) operation on");
        PRINT_URI(uriP);

        /* Validate the write(partial update) permission information */
        lwm2m_info.msg_type = QAPI_NET_LWM2M_WRITE_PARTIAL_UPDATE_REQ_E;
        result = validate_ex_object_permissions(contextP, uriP, lwm2m_info.msg_type);

#ifdef LWM2M_SERVER_OBJECT
        object_UpdateServerList(contextP, short_serverId);
#endif
      }
      else
      {
        if(!LWM2M_URI_IS_SET_INSTANCE(uriP) ||    /*object level*/
           !LWM2M_URI_IS_SET_RESOURCE(uriP) ||    /*object instance level*/
            LWM2M_URI_IS_SET_RESOURCE_INST(uriP)) /*resource instance level*/
        {
          result = (uint8_t)QAPI_METHOD_NOT_ALLOWED_4_05;
        }
        else
        {
          /* Execute operation */ 
          ds_iot_ulog_high("Execute operation on");
          PRINT_URI(uriP);

          /* Validate the execute permission information */
          lwm2m_info.msg_type = QAPI_NET_LWM2M_EXECUTE_REQ_E;
          result = validate_ex_object_permissions(contextP, uriP, lwm2m_info.msg_type);

#ifdef LWM2M_SERVER_OBJECT
          object_UpdateServerList(contextP, short_serverId);
#endif
        }
      }
      
      break;
    } 

    case QAPI_COAP_PUT:
    {
      /* Write attributes operation */
      if (QAPI_IS_OPTION(message, QAPI_COAP_OPTION_URI_QUERY))
      {
        lwm2m_attributes_t attr;

        ds_iot_ulog_high("Write attributes operation on");
        PRINT_URI(uriP)

        memset(&attr, 0x00, sizeof(lwm2m_attributes_t));

        lwm2m_info.msg_type = QAPI_NET_LWM2M_WRITE_ATTR_REQ_E;
        if (0 != prv_readAttributes(message->uri_query, &attr))
        { 
          result = (uint8_t) QAPI_BAD_REQUEST_4_00;         
        }
        else 
        {
          /* Forward the write attributes information to the application */
          lwm2m_info.lwm2m_attr = (qapi_Net_LWM2M_Attributes_t *)lwm2m_malloc(sizeof(qapi_Net_LWM2M_Attributes_t));
          if (!lwm2m_info.lwm2m_attr)
          {
            result = (uint8_t) QAPI_BAD_REQUEST_4_00;     
          }
          else 
          {
            memset(lwm2m_info.lwm2m_attr, 0x00, sizeof(qapi_Net_LWM2M_Attributes_t));
            /* Set the object information */
            memcpy(&lwm2m_info.lwm2m_attr->obj_info, &lwm2m_info.obj_info, sizeof(qapi_Net_LWM2M_Obj_Info_t)); 
  
            /* Clear the attribute information */
            if (attr.toClear)
            {
              lwm2m_info.lwm2m_attr->clr_attr_mask = (qapi_Net_LWM2M_Write_Attr_t)attr.toClear;
#if 0
              /* Minimum period */ 
              if (attr.toClear & LWM2M_ATTR_FLAG_MIN_PERIOD)
                lwm2m_info.lwm2m_attr->clr_attr_mask |= QAPI_NET_LWM2M_MIN_PERIOD_E;
                      
              /* Maximum period */           
              if (attr.toClear & LWM2M_ATTR_FLAG_MAX_PERIOD)
                lwm2m_info.lwm2m_attr->clr_attr_mask |= QAPI_NET_LWM2M_MAX_PERIOD_E; 
                      
              /* Greater than */           
              if (attr.toClear & LWM2M_ATTR_FLAG_GREATER_THAN)
                lwm2m_info.lwm2m_attr->clr_attr_mask |= QAPI_NET_LWM2M_GREATER_THAN_E;  
                      
              /* Lesser than */           
              if (attr.toClear & LWM2M_ATTR_FLAG_LESS_THAN)
                lwm2m_info.lwm2m_attr->clr_attr_mask |= QAPI_NET_LWM2M_LESS_THAN_E;
            
              /* Step size */           
              if (attr.toClear & LWM2M_ATTR_FLAG_STEP)
                lwm2m_info.lwm2m_attr->clr_attr_mask |= QAPI_NET_LWM2M_STEP_E;
#endif
            } 
  
            /* Set the attribute information */
            if (attr.toSet)
            {
              lwm2m_info.lwm2m_attr->set_attr_mask = (qapi_Net_LWM2M_Write_Attr_t)attr.toSet;
              /* Minimum period */ 
              //if (attr.toSet & LWM2M_ATTR_FLAG_MIN_PERIOD)
              //{
                lwm2m_info.lwm2m_attr->minPeriod = attr.minPeriod;
              // lwm2m_info.lwm2m_attr->set_attr_mask |= QAPI_NET_LWM2M_MIN_PERIOD_E;
              //}
            
              /* Maximum period */           
              //if (attr.toSet & LWM2M_ATTR_FLAG_MAX_PERIOD)
              //{
                lwm2m_info.lwm2m_attr->maxPeriod = attr.maxPeriod;
             //   lwm2m_info.lwm2m_attr->set_attr_mask |= QAPI_NET_LWM2M_MAX_PERIOD_E; 
             // }
            
              /* Greater than */           
             // if (attr.toSet & LWM2M_ATTR_FLAG_GREATER_THAN)
              //{ 
                lwm2m_info.lwm2m_attr->greaterThan = attr.greaterThan;
             //   lwm2m_info.lwm2m_attr->set_attr_mask |= QAPI_NET_LWM2M_GREATER_THAN_E;  
              //} 
            
              /* Lesser than */           
              //if (attr.toSet & LWM2M_ATTR_FLAG_LESS_THAN)
              //{ 
                lwm2m_info.lwm2m_attr->lessThan = attr.lessThan;
              //  lwm2m_info.lwm2m_attr->set_attr_mask |= QAPI_NET_LWM2M_LESS_THAN_E;
              //} 
  
              /* Step size */           
             // if (attr.toSet & LWM2M_ATTR_FLAG_STEP)
              //{ 
                lwm2m_info.lwm2m_attr->step = attr.step;
              //  lwm2m_info.lwm2m_attr->set_attr_mask |= QAPI_NET_LWM2M_STEP_E;
             // } 
            }
          }
        }
      }
      /* Write operation - Replace */ 
      else if (LWM2M_URI_IS_SET_INSTANCE(uriP) && cont_type_set)
      {
        ds_iot_ulog_high("Write (replace) operation on");
        PRINT_URI(uriP);

        /* Validate the write(replace) permission information */
        lwm2m_info.msg_type = QAPI_NET_LWM2M_WRITE_REPLACE_REQ_E;            
        result = validate_ex_object_permissions(contextP, uriP, lwm2m_info.msg_type);
      
#ifdef LWM2M_SERVER_OBJECT
        object_UpdateServerList(contextP, short_serverId);
#endif
      }
      else
        result = (uint8_t)QAPI_BAD_REQUEST_4_00;

      break;
    }
      
    case QAPI_COAP_DELETE:
    {
      if (!LWM2M_URI_IS_SET_INSTANCE(uriP) || LWM2M_URI_IS_SET_RESOURCE(uriP))
      {
        result = (uint8_t)QAPI_METHOD_NOT_ALLOWED_4_05;
      }
      else
      {
        /* Delete operation */ 
        ds_iot_ulog_high("Delete operation on");
        PRINT_URI(uriP)

        /* Validate the write(replace) permission information */
        lwm2m_info.msg_type = QAPI_NET_LWM2M_DELETE_REQ_E;
        result = validate_ex_object_permissions(contextP, uriP, lwm2m_info.msg_type);
      }

      break;
    }

    default:
    {
      result = (uint8_t)QAPI_BAD_REQUEST_4_00;
      break;
    }
  }

  contextP->activeServerP = NULL;

  /* Notify the applications using the register callbacks of the request message */
  if (LWM2M_NO_ERROR == result)
  {
    qapi_Status_t status = QAPI_NO_ERROR;
    status = lwm2m_client_application_notification(&lwm2m_info);
    if (status == QAPI_ERR_NO_ENTRY)
      result = (uint8_t)QAPI_NOT_FOUND_4_04;
    else if (status)
      result = (uint8_t)QAPI_INTERNAL_SERVER_ERROR_5_00;
  } 
  else 
  {
    /* Skip the ACL failures */ 
    if (result != QAPI_UNAUTHORIZED_4_01)
    {
      lwm2m_info.msg_type = QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E;
      lwm2m_info.event = QAPI_NET_LWM2M_RX_MESSAGE_INTERNAL_FAILURE_E;

      /* Ignore the error from the registered callback, as there was already an error */
      lwm2m_client_application_notification(&lwm2m_info);
    }
  }

  if (lwm2m_info.lwm2m_attr)
  {
    lwm2m_free(lwm2m_info.lwm2m_attr);
    lwm2m_info.lwm2m_attr = NULL;
  }
  return result;
}


#endif


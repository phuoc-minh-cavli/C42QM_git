/******************************************************************************

  @file    registration.c

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
 *    domedambrosio - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Simon Bernard - Please refer to git log
 *    Toby Jaffey - Please refer to git log
 *    Manuel Sangoi - Please refer to git log
 *    Julien Vermillard - Please refer to git log
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
#include "liblwm2m.h"
#include "lwm2m_signals.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lwm2m_carrier.h"
#include "object_server.h"
#include "qapi_coap.h"
#include "lwm2mclient.h"
#include "lwm2m_rx.h"
#include "ds_log.h"


#define MAX_LOCATION_LENGTH 10      // strlen("/rd/65534") + 1
#define PRV_BINDING_MODE_VERSION_1_0  "UQS"
#define PRV_BINDING_MODE_VERSION_1_1  "UN"

#ifdef LWM2M_CLIENT_MODE
#ifdef LWM2M_SERVER_OBJECT
extern uint16_t short_ServerId;
extern bool update_server;
extern bool disable_server;
extern time_t disable_TimeOut;
#endif

#include "timer.h"
#include "qurt/qurt_signal.h"

extern qurt_signal_t lwm2m_signal_Handle;
extern bool g_carrier_cfg;  // flag to check  carrier config is enabled or not 
extern bool lwm2m_psm_enable;  // LWM2M PSM is enabled or not.

#define LWM2M_TRIGGER_EARLYREGUPDATE_SEC 15
#define MAX_DNS_ATTEMPT 1
/* Considering the processing delay of client as 5secs, for proper handling of registration update */
#define LWM2M_PROCESSING_DELAY   5 
/* As per design registration update should be triggered after completion of 90% of life time */
#define LWM2M_10PERCENT_OF_LIFETIME     ((float)10/100)
extern uint32_t active_time;
extern bool lwm2m_in_sleep_state;
extern void sys_m_reset(void);
extern bool persis_regstatus_feat_enabled;
extern void trigger_retry(lwm2m_server_t *targetP);


time_t gRegRetryInitialTimeout;
time_t gRegRetrytimeoutExponent;
time_t gRegRetryMaxTimeout;



typedef struct version_map_s
{
  uint16_t short_id;
  float version;
} version_map_t;


uint8_t persist_lwm2m_server_version(uint16_t short_id, float version)
{
  int fd = 0;
  version_map_t v_map;

  /* Only the LwM2M versions of the registration servers are persisted */
  if ((0x00 == version) || 
      (0x00 == short_id))
  {
    LOG_DEBUG("Invalid parameters - short_id[%d], version[%f]", short_id, version);
    return 1;
  }
  LOG_DEBUG("EFS_OPT: File open %s",LWM2M_VERSION_PERSISTENCE_FILE);
  fd = efs_open(LWM2M_VERSION_PERSISTENCE_FILE, O_APPEND | O_CREAT | O_WRONLY, 00644);
  if (EFAILURE == fd)
  {
    ds_iot_log_strf_err("Opening LwM2M version file to persist the version[%f] failed for SSID[%d]", version, short_id);
    return 1;
  }

  memset(&v_map, 0x00, sizeof(version_map_t));
  
  v_map.short_id = short_id;
  v_map.version  = version;
  LOG_DEBUG("EFS_OPT: File write %s",LWM2M_VERSION_PERSISTENCE_FILE);
  if (EFAILURE == efs_write(fd, &v_map, sizeof(version_map_t)))
  {
    ds_iot_log_strf_err("Persisting version in the LwM2M version[%f] failed for SSID[%d]", version, short_id);
    efs_close(fd);
    return 1;
  }
  
  efs_close(fd);
  return 0; 
}


uint8_t get_persisted_lwm2m_server_version(uint16_t short_id, float *version)
{
  int fd = 0;
  version_map_t v_map;
  
  /* Only the LwM2M versions of the registration servers are persisted */
  if ((NULL == version) || 
      (0x00 == short_id))
  {
    LOG_DEBUG("Invalid parameters - short_id[%d], version[0x%x]", short_id, version);
    return 1;
  }
  LOG_DEBUG("EFS_OPT: File open %s",LWM2M_VERSION_PERSISTENCE_FILE);
  fd = efs_open(LWM2M_VERSION_PERSISTENCE_FILE, O_RDONLY, 00644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err1("Opening LwM2M version persistence file for SSID[%d] failed", short_id);
    return 1;
  }

  while (true)
  {
    memset(&v_map, 0x00, sizeof(version_map_t));

	   /* Search for the server associated with the short server ID and retrieve the version */	  
    if (efs_read(fd, &v_map, sizeof(version_map_t)))
    {
      if (v_map.short_id == short_id)
        *version = v_map.version;
	
      continue;
    }

    break;
  }
  
  efs_close(fd);
  return 0; 
}


/**
 * @fn prv_deregister()
 * @brief This function is used to send de-registartion to all the 
 *     servers and perform a reboot
 * @param contextP lwm2m context
 * @return  void
 */
void prv_deregister(lwm2m_context_t * contextP)
{
  lwm2m_server_t *serverP = NULL;

  if(NULL == contextP)
  {
    LOG_DEBUG("ContextP is NULL");
    return;
  }

  /*stop the ext object create window*/
  StopExtObjCreateWindow();

  serverP = contextP->serverList;
  while(serverP != NULL)
  {
    registration_deregister(contextP, serverP);
    serverP = serverP->next;
  }

  return;
}

/**
 * @fn prv_getRegistrationQuery()
 * @brief This function is used to get the registration request buffer to be sent to the n/w 
 * @param contextP lwm2m context
 * @param server server context to which registration is to be triggerred
 * @param buffer output buffer 
 * @param length buffer length
 * @return 0 on failure
 *     length of the registration query on success
 */
static int prv_getRegistrationQuery(lwm2m_context_t * contextP,
    lwm2m_server_t * server,
    char * buffer,
    size_t length)
{
  char *version_ptr = NULL; 
  int index = 0;
  int res = 0;
  uint8_t bindings_cntr = 3;
  uint8_t binding_buf[MAX_BINDING_SIZE + 3] = {'&', 'b', '=', '\0', '\0', '\0', '\0', '\0'};  /* Bindings - "&b=UQS" or "&b=UNTS" */

  if ( contextP == NULL || server == NULL || buffer == NULL ) 
  { 
    LOG_DEBUG("%s",nullError);
    return 0;
  } 

  /* End-point name */
  index = utils_stringCopy(buffer, length, "?ep=");
  if (index < 0) return 0;

  res = utils_stringCopy(buffer + index, length - index, contextP->endpointName);
  if (res < 0) return 0;
  index += res;
  
  /* Version */ 
  res = utils_stringCopy(buffer+index, length - index, QUERY_DELIMITER QUERY_VERSION);
  if (res < 0) return 0;
  index += res;

  /* Add other supported LwM2M versions going forward */ 
  if ((float)LWM2M_V_1_1 == server->version) 
    version_ptr = QUERY_VERSION_1_1;
  else 
    version_ptr = QUERY_VERSION_1_0;

  res = utils_stringCopy(buffer + index, length - index, version_ptr); 
  if (res < 0) return 0;
  index += res;

  /* SMS */ 
  if (NULL != contextP->msisdn)
  {
    res = utils_stringCopy(buffer + index, length - index, QUERY_DELIMITER QUERY_SMS);
    if (res < 0) return 0;
    index += res;
    res = utils_stringCopy(buffer + index, length - index, contextP->msisdn);
    if (res < 0) return 0;
    index += res;
  }

  /* Bindings */
  if ((float)LWM2M_V_1_0 == server->version)
  {
    /* Servers that support LwM2Mv1.0 would encode bindings from the "server bindings" resource */
    if(server->isFallbackV1_0)
    {
      memcpy((void *)&binding_buf[bindings_cntr], PRV_BINDING_MODE_VERSION_1_0, 3);
      bindings_cntr += 3;
    }
    else
    {
      /* UDP binding */
      if (server->binding & BINDING_U)
        binding_buf[bindings_cntr++] = 'U';

      /* QUEUE mode (encoded only for LwM2Mv1.0 as part of bindings) */
      if (server->binding & BINDING_Q)
        binding_buf[bindings_cntr++] = 'Q';

      /* SMS binding */
      if (server->binding & BINDING_S)
        binding_buf[bindings_cntr++] = 'S';
    }
  }
  else
  {
    /* Servers that support LwM2Mv1.1 or higher would encode the bindings from the "device bindings" 
     * resource which is hard-coded to "U" and "N" (supported by our LwM2M client). 
     */
    memcpy((void *)&binding_buf[bindings_cntr], PRV_BINDING_MODE_VERSION_1_1, 2);
    bindings_cntr += 2;
  }

  /* Encode the bindings information */
  res = utils_stringCopy(buffer + index, length - index, (const char *)binding_buf);
  if (res < 0) return 0;
  index += res;

  /* Queue mode (supported by servers with LwM2Mv1.1 and higher) - always true */
  if (server->version >= (float)LWM2M_V_1_1)
  {
    res = utils_stringCopy(buffer + index, length - index, QUERY_DELIMITER QUERY_QUEUE_MODE);
    if (res < 0) return 0;
      index = index + res; 
  }
    
  return index;
}


/**
 * @fn prv_getRegistrationUpdateQuery()
 * @brief This function is used to get the registration update buffer to be sent to the n/w 
 * @param contextP lwm2m context
 * @param server server context to which registration update is to be triggerred
 * @param buffer output buffer 
 * @param length buffer length
 * @return 0 on failure
 *     length of the registration update query on success
 */
static int prv_getRegistrationUpdateQuery(lwm2m_context_t * contextP,
    lwm2m_server_t * server,
    char * buffer,
    size_t length,
    uint8_t setFlag)
{
  int index = 0;
  int res = 0;
  uint8_t bindings_cntr = 3;
  uint8_t binding_buf[MAX_BINDING_SIZE + 3] = {'&', 'b', '=', '\0', '\0', '\0', '\0', '\0'};  /* Bindings - "&b=UQS" */

  ds_iot_ulog_high1("Set Flag %d", setFlag);

  if ( contextP == NULL || server == NULL || buffer == NULL ) 
  { 
    LOG_DEBUG("%s",nullError);
    return 0;
  } 

  index = utils_stringCopy(buffer, length, "?");
  if (index < 0) return 0;

  /* Life time */ 
  if (LWM2M_SERVER_IS_UPDATE_SET_LIFETIME(setFlag))
  {
    ds_iot_ulog_high("Include Life Time");
    
    res = utils_stringCopy(buffer + index, length - index, QUERY_DELIMITER QUERY_LIFETIME);
    if (res < 0) return 0;
    index += res;
    
    res = utils_intCopy(buffer + index, length - index, server->lifetime);
    if (res < 0) return 0;
    index += res;
  }

  /* Bindings */ 
  if (LWM2M_SERVER_IS_UPDATE_SET_BINDING(setFlag))
  { 
    ds_iot_ulog_high("Include Binding mode");

    /* Encode the bindings information from the server object instance for servers supporting LwM2Mv1.0. 
     * For servers supporting LwM2Mv1.1, we need to encode the device bindings instead of server bindings. 
     * As there would be no change in the device bindings of the LwM2M client, the bindings are omitted.
     */
    if ((float)LWM2M_V_1_0 == server->version)
    {
      /* Bindings information - UDP */
      if (server->binding & BINDING_U)
        binding_buf[bindings_cntr++] = 'U';
  
     	/* Bindings information - QUEUE */
      if (server->binding & BINDING_Q)
        binding_buf[bindings_cntr++] = 'Q';  
  
      /* Bindings information - SMS */
      if (server->binding & BINDING_S)
        binding_buf[bindings_cntr++] = 'S';  
   
      /* Encode the bindings information */
      res = utils_stringCopy(buffer + index, length - index, (const char *)binding_buf);
      if (res < 0) return 0;
        index = index + res;  
    }
  }

  return index;
}

void lwm2m_cb_util(qapi_Coap_Session_Hdl_t hdl,
     qapi_Coap_Transaction_t * transacP,
     qapi_Coap_Packet_t * rsp_pkt,
     uint8_t cmd_id)
{
  lwm2m_pkt_t *pkt = NULL;
  transaction_cb_info_t *transactP = NULL;

  if ((NULL == transacP) || 
      (NULL == hdl))
  {
    ds_iot_ulog_err2("Invalid input params - transacP[0x%x], hdl[0x%x]", transacP, hdl);
    return;
  }

  transactP = (transaction_cb_info_t *) malloc(sizeof(transaction_cb_info_t));
  if(transactP == NULL)
  {
    LOG_DEBUG("failed to allocate memory");
    return;
  }
  memset(transactP, 0, sizeof(transaction_cb_info_t));

  transactP->message = transacP->message;
  transactP->userData = transacP->userData;
  if(cmd_id == LWM2M_EXT_APPLICATION_CB)
  {
    transactP->ack_received = transacP->ack_received;
    transactP->retrans_counter = transacP->retrans_counter;
  }
  pkt = lwm2m_get_pkt_buffer();
  if(pkt == NULL)
  {
    if(transactP) free(transactP);
    return;
  }
  memset(pkt, 0x00, sizeof(lwm2m_pkt_t));
  pkt->cmd_hdr.cmd_id = (lwm2m_pkt_id_e)cmd_id;
  pkt->cmd_data.cHandle = (void *)hdl;
  pkt->cmd_data.message = (void *)rsp_pkt;
  pkt->cmd_data.clientData = (void *)transactP;

  lwm2m_enqueue_pkt(pkt);

  lwm2m_set_signal();
  return;
}

/**
 * @fn lwm2m_registration_cb()
 * @brief Callback function to handle Registartion reply message
 * @param transacP transaction handle
 * @param message message received
 * @return  void
 */
static void lwm2m_registration_cb(qapi_Coap_Session_Hdl_t hdl,
     qapi_Coap_Transaction_t * transacP,
     qapi_Coap_Packet_t * rsp_pkt)
{
  lwm2m_cb_util(hdl, transacP,rsp_pkt, LWM2M_REGISTRATION_CB);
}

void prv_registration_failed(lwm2m_server_t * targetP)
{
  targetP->status = STATE_REG_FAILED;
  targetP->retryTimeout = 0;
  targetP->retry_attempt = 0;
  targetP->prevAttemptTime = 0;
  /* If 5.03 error code is received so for VzW case if wait_time_for_error is configured
     this time will be given preference over next cycle timer.
     Also, this cycle attempt won't be counted.*/
  if(g_carrier_cfg && data.cfg_info.wait_time_for_error
     && targetP->last_code == LWM2M_503_SERVICE_UNAVAILABLE)
  {
    targetP->next_cycle_for_registration =  lwm2m_gettime() + data.cfg_info.wait_time_for_error;
    ds_iot_ulog_high1("Next registration cycle will start after %d sec", data.cfg_info.wait_time_for_error);
  }
  else
  {
    targetP->seq_retry_attempt++;
    targetP->next_cycle_for_registration = lwm2m_gettime() + targetP->seq_delay_timer;
    ds_iot_ulog_high1("Next registration cycle will start after %d sec", targetP->seq_delay_timer);
  }
}

/**
 * @fn prv_handleRegistrationReply()
 * @brief function to handle Registartion reply message
 * @param transacP transaction handle
 * @param message message received
 * @return  void
 */
void prv_handleRegistrationReply(qapi_Coap_Session_Hdl_t hdl,
     transaction_cb_info_t * transacP,
     qapi_Coap_Packet_t * rsp_pkt)
{
  lwm2m_server_t * targetP = NULL;
  char *location = NULL;


  ds_iot_ulog_high("Inside prv_handleRegistrationReply");

  if ((NULL == transacP) || 
      (NULL == hdl))
  { 
    LOG_DEBUG("Invalid input params - transacP[0x%x], hdl[0x%x]", transacP, hdl);
    return;
  }

  targetP = (lwm2m_server_t *)(transacP->userData);
  if (targetP == NULL)
  {
    return;
  }
  if (targetP->status == STATE_REG_PENDING)
  {
    lwm2m_server_data_t lwm2m_info;
    time_t tv_sec = lwm2m_gettime();

    if(LWM2M_IS_INVALIDE_PACKETS(rsp_pkt))
    {
       LOG_DEBUG("Registration Reply:  recevied invalid packet");
       targetP->last_code = LWM2M_IGNORE;
       prv_registration_failed(targetP);
       goto end;
    }

    if ((tv_sec == 0 ) || (tv_sec > 0))
      targetP->registration = tv_sec;
    
    /* Successful registration */
    if ((rsp_pkt != NULL) && 
        (LWM2M_201_CREATED == rsp_pkt->code))
    {
      float version = 0.0;
      ds_iot_log_strf_high("Registration successful for SSID[%d] with version[%f]", targetP->shortID, targetP->version); 
      targetP->last_code = LWM2M_201_CREATED;
      /* Persist the negotiated LwM2M server version. Continue using this persisted version from now for that paritcular server 
       * until the cached persisted version file is removed.
       */ 
      get_persisted_lwm2m_server_version(targetP->shortID, &version);

      if(!version || version != targetP->version)
      {
        persist_lwm2m_server_version(targetP->shortID, targetP->version);
      }

      /* Re-set the APN link information if the negotiated version is v1.0 instead of the configured v1.1 */
      if (((float)LWM2M_V_1_0 == targetP->version) && 
          (targetP->apn_link != 0xFFFFFFFF))
      { 
        lwm2m_object_t *server_obj = NULL;
        server_instance_t *server_inst = NULL;        

        targetP->apn_link = 0xFFFFFFFF; 
     
        server_obj = (lwm2m_object_t *)LWM2M_LIST_FIND(data.lwm2mH->objectList, LWM2M_SERVER_OBJECT_ID);
        if (NULL == server_obj)
        {
          LOG_DEBUG("Server object is not found in the device");
          return;
        }

        server_inst = (server_instance_t *)server_obj->instanceList;
        while (server_inst)
        {
          if (server_inst->shortServerId == targetP->shortID)
          {
            server_inst->apnLink = 0xFFFFFFFF;
            break;
          }

          server_inst = server_inst->next; 
        }

        if (NULL == server_inst)
        {
          LOG_DEBUG("Server instance associated with the SSID[%d] is not found on the device", targetP->shortID);
          return;
        }
      }
 
      if (NULL != targetP->location)
        lwm2m_free(targetP->location);
 
      /* Location information */   
      qapi_Coap_Get_Header(hdl, (qapi_Coap_Packet_t *)rsp_pkt, QAPI_COAP_LOCATION_PATH, (void **)&location, 0);
      targetP->location = location;       

      /* Set the "register update" for lifetime expiry */
      targetP->reg_update_for_lifetime_expiry = false;
 
      if (LWM2M_TRUE == targetP->is_update_for_wakeup)
        targetP->is_update_for_wakeup = LWM2M_FALSE;

      /* Set the state to registered */ 
      targetP->status = STATE_REGISTERED;
      targetP->retryTimeout = 0;
      targetP->retry_attempt = 0;
      targetP->seq_retry_attempt = 0;

      memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t));

      /* Indicate OEM applications that the device is awake */
      if(!data.glwm2m_app_ctx.device_active)
      {
        lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_WAKEUP_E);
      }

      memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t));
      /* Indicate OEM applications of the registration successful with SSID. */
      memcpy(&lwm2m_info.msg_id[lwm2m_info.msg_id_len], &targetP->shortID, 0x02);
      lwm2m_info.msg_id_len = 2;
      lwm2m_info.msg_type = QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E;
      lwm2m_info.event = QAPI_NET_LWM2M_REGISTER_UPDATE_E;
      lwm2m_client_application_notification(&lwm2m_info);

    }
    /* Version negotiation */
    else if ((rsp_pkt != NULL) && 
             (LWM2M_412_PRECONDITION_FAILED == rsp_pkt->code))
    {
      uint8_t cntr = 0x00; 
      uint8_t version_cnt = sizeof(supported_lwm2m_ver)/sizeof(float);
      float curr_version = targetP->version;
      
      ds_iot_log_strf_err("Received error 4.12 (pre-condition failure) for SSID[%d] with version[%f]", targetP->shortID, targetP->version);
      targetP->last_code = LWM2M_412_PRECONDITION_FAILED;
      /* Get the next lower LwM2M version to send the "Registration Request" */
      for (cntr = 0; cntr < version_cnt; cntr++)
      {
        if (supported_lwm2m_ver[cntr] == curr_version)
        {
          cntr += 0x01;
          break;
        }  
      }

      if (cntr < version_cnt)
      {
        time_t tv_sec = lwm2m_gettime();
        
        /* Lower down the version and retry sending the "registration request" */
        targetP->version = supported_lwm2m_ver[cntr];
        if((float)LWM2M_V_1_0 == targetP->version)
          targetP->isFallbackV1_0 = TRUE;

        /* Setup the registration related information prior to performing the retry */ 
        targetP->status = STATE_REG_RETRYING;
        targetP->prevAttemptTime = tv_sec;
        targetP->registration = tv_sec;
        targetP->retry_attempt = 0;
        targetP->seq_retry_attempt = 0;

        /* Update the registration retry resources based on lower LwM2M version.*/
        populate_lower_version_retry_values(targetP->shortID);
        get_1_0_retry_info(targetP);

        /* Perform retry operation to send the "registration request" with the lower version */ 
        timer_set(&targetP->regOpTimer, 500, 0, T_MSEC);
      }
      else 
      {
        targetP->last_code = LWM2M_IGNORE;
        prv_registration_failed(targetP);
      }
    }
    else if((rsp_pkt != NULL) && 
             (LWM2M_503_SERVICE_UNAVAILABLE== rsp_pkt->code))
    {
      ds_iot_ulog_err2 ("Received error response [0x%x] for \"registration request\" for SSID[%d]", rsp_pkt->code, targetP->shortID);
      targetP->last_code = LWM2M_503_SERVICE_UNAVAILABLE;
      prv_registration_failed(targetP);
      
    }
    else if(rsp_pkt == NULL)
    {
      ds_iot_ulog_err1("Received NULL response for \"registration request\" for SSID[%d]", targetP->shortID);
      targetP->last_code = LWM2M_IGNORE;
      /* Perform "registration request" retry */
      trigger_retry(targetP);      
    }
    else
    {
      ds_iot_ulog_err2 ("Received error response [0x%x] for \"registration request\" for SSID[%d]", rsp_pkt->code, targetP->shortID);
      targetP->last_code = rsp_pkt->code;
      prv_registration_failed(targetP);
    }
end: 
    if (persis_regstatus_feat_enabled == LWM2M_TRUE)
      lwm2m_carrier_update_regstatus(targetP);
 
    if (transacP->message)
    {
      qapi_Coap_Free_Message(hdl,transacP->message);
      transacP->message = NULL;
    }
    if(transacP)
      free(transacP);
 
    if(rsp_pkt)
      qapi_Coap_Free_Message(hdl, rsp_pkt);

    qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
  }
 
  if (targetP->status == STATE_REGISTERED)
  {
    targetP->register_success = TRUE;
  }
  else
  {
    storeRegisteredExtObj(targetP, false);
  }

  /* Update the active time for LWM2M client */
  active_time = lwm2m_gettime();
#ifdef PSM_LWM2M
lwm2m_psm_operation(data.lwm2mH, 0, NULL, PSM_NODE_TYPE_MIN, PSM_NODE_UPDATE_ALL, PSM_MODIFY_NODE);


#endif
}


#define PRV_QUERY_BUFFER_LENGTH 200

/**
 * @fn prv_register() 
 * @brief This function is used to sens registration for particular 
 *     server
 * @param contextP lwm2m context
 * @param server server information to which registration needs to be sent
 * @return  COAP failure error on failure
 *      COAP_NO_ERROR on success
 */
uint8_t prv_register(lwm2m_context_t * contextP,
     lwm2m_server_t * server)
{
  char query[200];
  int query_length = 0;
  uint8_t *payload = NULL;
  int payload_length = 0;
  qapi_Coap_Packet_t *messageP = NULL;
  unsigned int link_type = 0;
  qapi_Status_t result = QAPI_ERROR;
  qapi_Coap_Message_Params_t message_param;
  qapi_Coap_Block_Wise_Options_t blocktype = QAPI_COAP_BLOCK_OPTION_NONE;
  uint16_t blocksize = 0;


  if ( contextP == NULL || server == NULL ) 
  { 
    LOG_DEBUG("Invalid input params - contextP[0x%x], server[0x%x]", contextP, server);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
 
  memset(&message_param, 0x00, sizeof(message_param));

  payload_length = get_registerPayload_size(contextP);
  if(payload_length <= 0)
    return LWM2M_500_INTERNAL_SERVER_ERROR;

  /* Adding padding of size two because 
  we are using 2 extra unit to prepare payload  */
  payload_length += BUFFER_PADDING_LENGTH;

  payload = (uint8_t *) malloc(payload_length);

  if(payload == NULL)
  {
     ds_iot_ulog_err("no memory for registeration payload");
     return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  memset(payload, 0x00, payload_length);

  payload_length = object_getRegisterPayload(contextP, server, payload, payload_length);
  if(payload_length == 0) 
  {
     free(payload);
     return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
 
  query_length = prv_getRegistrationQuery(contextP, server, query, sizeof(query));
  if (query_length == 0)
  {
    free(payload);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
 
  if (0 != server->lifetime)
  {
    int res = 0;
 
    res = utils_stringCopy(query + query_length, PRV_QUERY_BUFFER_LENGTH - query_length, QUERY_DELIMITER QUERY_LIFETIME);
    if (res < 0) 
    {
       free(payload);
       return LWM2M_500_INTERNAL_SERVER_ERROR;
    }
    query_length += res;
    res = utils_intCopy(query + query_length, PRV_QUERY_BUFFER_LENGTH - query_length, server->lifetime);
    if (res < 0) 
    {
       free(payload);
       return LWM2M_500_INTERNAL_SERVER_ERROR;
    }
    query_length += res;
  }
 
  if (0 == server->sessionH)
  {
    server->sessionH = lwm2m_connect_server(server, contextP);
    ds_iot_ulog_high1("PRV REGISTER server session[0x%x]", server->sessionH);
  }
 
  if ((NULL == server->coapHandle) || 
      (0 == server->sessionH)) 
  {
    trigger_retry(server);
    free(payload);
    return LWM2M_503_SERVICE_UNAVAILABLE;
  }
  
  result = qapi_Coap_Init_Message(server->coapHandle,&messageP, (qapi_Coap_Message_Type_t)QAPI_COAP_TYPE_CON, (qapi_Coap_Method_t)QAPI_COAP_POST);
  if (result != QAPI_OK)
  {
    free(payload);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
 
  result= qapi_Coap_Set_Header(server->coapHandle, messageP , QAPI_COAP_URI_PATH, "/"URI_REGISTRATION_SEGMENT , 0);
  if (result != QAPI_OK) 
  {
    ds_iot_ulog_err("Setting the URI path in the CoAP header failed");
    qapi_Coap_Free_Message(server->coapHandle, messageP);
    free(payload);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

   result = qapi_Coap_Set_Header(server->coapHandle,messageP , QAPI_COAP_URI_QUERY, query, 0);  
   if (result != QAPI_OK)
   {
     ds_iot_ulog_err("Setting the URI query in the CoAP header failed");
     qapi_Coap_Free_Message(server->coapHandle, messageP);
     free(payload);
     return LWM2M_500_INTERNAL_SERVER_ERROR;
   }

   link_type = LWM2M_CONTENT_LINK; 
   result = qapi_Coap_Set_Header(server->coapHandle ,messageP, QAPI_COAP_CONTENT_TYPE, &link_type, 0);
   if (result != QAPI_OK)
   {
     ds_iot_ulog_err("Setting the content type in the CoAP header failed");
     qapi_Coap_Free_Message(server->coapHandle, messageP);
     free(payload);
     return LWM2M_500_INTERNAL_SERVER_ERROR;
   }
   
   result = qapi_Coap_Set_Payload(server->coapHandle,messageP, payload, payload_length);
   if (result != QAPI_OK)
   {
     ds_iot_ulog_err("Setting the registration request payload in the CoAP message failed");
     qapi_Coap_Free_Message(server->coapHandle, messageP);
     free(payload);
     return LWM2M_500_INTERNAL_SERVER_ERROR;
   }
    
   /* Update the active time for LwM2M client */
   active_time = lwm2m_gettime();
   lwm2m_in_sleep_state = LWM2M_FALSE;
 
   message_param.lastmid = 0;
   message_param.msg_cb = lwm2m_registration_cb;
   message_param.token= NULL;
   message_param.token_len=0;
   message_param.msgUsrData = (void *)server;

   blocksize = get_blockSize(server);
   if(blocksize >= 16 && messageP->payload_len > blocksize)
   { 
       blocktype  = QAPI_COAP_BLOCK1_OPTION;
   }
   else
   {
       blocksize = 0;
       blocktype  = QAPI_COAP_BLOCK_OPTION_NONE;
   }
   if(LWM2M_SERVER_IS_SERVER_IP_CHANGE_SET(server->update_disable_check))
   {
     LWM2M_SERVER_RESET_SERVER_IP_CHANGE_CHECK(server->update_disable_check);
   }
   if(qapi_Coap_Send_Message_v2(server->coapHandle , messageP, &message_param, blocktype, blocksize) != QAPI_OK)
   {
     ds_iot_ulog_high1("Sending the \"register request\" failed for SSID[%d]", server->shortID);
     qapi_Coap_Free_Message(server->coapHandle, messageP);
     lwm2m_clean_coap_session(server);
     free(payload);
     return LWM2M_500_INTERNAL_SERVER_ERROR;
   }
  
   /* Set the status to registration pending */
   server->status = STATE_REG_PENDING;
   server->prevAttemptTime = lwm2m_gettime();
   ds_iot_ulog_high1("Sent the \"registration request\" for SSID[%d]", server->shortID);

#ifdef PSM_LWM2M
lwm2m_psm_operation(contextP, 0, NULL, PSM_NODE_TYPE_MIN, PSM_NODE_UPDATE_ALL, PSM_MODIFY_NODE);

#endif

   free(payload);
   return LWM2M_NO_ERROR;
}

/**
 * @fn lwm2m_registration_update_cb()
 * @brief Callback function to handle registration update reply
 * @param transacP transaction handle
 * @param message message received
 * @return  void
 */
static void lwm2m_registration_update_cb(qapi_Coap_Session_Hdl_t hdl, 
    qapi_Coap_Transaction_t * transacP,
    qapi_Coap_Packet_t * rsp_pkt)
{
  lwm2m_cb_util(hdl, transacP,rsp_pkt, LWM2M_REGISTRATION_UPDATE_CB);
}

/**
 * @fn prv_handleRegistrationUpdateReply()
 * @brief function to handle registration update reply
 * @param transacP transaction handle
 * @param message message received
 * @return  void
 */
void prv_handleRegistrationUpdateReply(qapi_Coap_Session_Hdl_t hdl, 
    transaction_cb_info_t * transacP,
    qapi_Coap_Packet_t * rsp_pkt)
{
  lwm2m_server_t * targetP = NULL;

   if ((NULL == transacP) || 
       (NULL == hdl))
  { 
    LOG_DEBUG("Invalid input params - transacP[0x%x], hdl[0x%x]", transacP, hdl);
    return;
  }

  targetP = (lwm2m_server_t *)(transacP->userData);
  if (targetP == NULL)
  {
    return;
  }
  if (targetP->status == STATE_REG_UPDATE_PENDING)
  {
    time_t tv_sec = lwm2m_gettime();

    if(LWM2M_IS_INVALIDE_PACKETS(rsp_pkt))
    {
      LOG_DEBUG(" Registration Update Reply:  recevied invalid packet");
      goto end;
    }

    if ((rsp_pkt != NULL) && (rsp_pkt->code == LWM2M_204_CHANGED)) 
    {
      lwm2m_server_data_t lwm2m_info;
      targetP->status = STATE_REGISTERED;
      targetP->isRegupdate_for_sms = false;

      if ((tv_sec == 0) || (tv_sec > 0))
        targetP->registration = tv_sec;

      ds_iot_ulog_high1("Registration update successful for SSID[%d]", targetP->shortID);

      lwm2m_in_sleep_state = LWM2M_FALSE;
      if (LWM2M_TRUE == targetP->is_update_for_wakeup)
      {
        targetP->is_update_for_wakeup = LWM2M_FALSE;
        qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
        LOG_DEBUG("Update response received! Waking up!! ");
      }

      targetP->reg_update_for_lifetime_expiry = false;

      /* Indicate OEM applications that the device is awake */
      if(!data.glwm2m_app_ctx.device_active)
      {
        lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_WAKEUP_E);
      }

      memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t));
      /* Indicate OEM applications of the registration successful with SSID. */
      memcpy(&lwm2m_info.msg_id[lwm2m_info.msg_id_len], &targetP->shortID, 0x02);
      lwm2m_info.msg_id_len = 2;
      lwm2m_info.msg_type = QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E;
      lwm2m_info.event = QAPI_NET_LWM2M_REGISTER_UPDATE_E;
      lwm2m_client_application_notification(&lwm2m_info);
      
      targetP->register_success = TRUE;
    }
    else
end:
    {
      targetP->status = STATE_REGISTERED;

      if (rsp_pkt == NULL)
      {
        ds_iot_ulog_err("Received Null response for registration update");
        LWM2M_SERVER_SET_SERVER_IP_CHANGE_CHECK(targetP->update_disable_check);
      }
      else if (LWM2M_400_BAD_REQUEST <= rsp_pkt->code &&  rsp_pkt->code <= LWM2M_503_SERVICE_UNAVAILABLE) 
      {
        targetP->status = STATE_DEREGISTERED;
        ds_iot_ulog_err("Received the 404 NOT FOUND response code for registration update");  
      }

      if (LWM2M_TRUE == targetP->is_update_for_wakeup)
        targetP->is_update_for_wakeup = LWM2M_FALSE;
      
      ds_iot_ulog_err1("Registration update failed for SSID[%d]", targetP->shortID);	  
      storeRegisteredExtObj(targetP, false);
    }
      //Updating the registration status to configuration file
      if (persis_regstatus_feat_enabled == LWM2M_TRUE)  
        lwm2m_carrier_update_regstatus(targetP);

      qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
  }

  if (transacP->message)
  {
    qapi_Coap_Free_Message(hdl, transacP->message);
    transacP->message = NULL;
  }
  if(transacP)
      free(transacP);

  if (rsp_pkt)
    qapi_Coap_Free_Message(hdl, rsp_pkt);

  /* Update the active time for LwM2M client */
  active_time = lwm2m_gettime();
#ifdef PSM_LWM2M
lwm2m_psm_operation(data.lwm2mH, 0, NULL, PSM_NODE_TYPE_MIN, PSM_NODE_UPDATE_ALL, PSM_MODIFY_NODE);

#endif
}


/**
 * @fn prv_updateRegistration()
 * @brief Function to send registration update to the server
 * @param contextP lwm2m context
 * @param server server information to which update needs to be sent
 * @param withObjects indicates if update with objects or not
 * @return COAP_NO_ERROR on success 
 *         COAP error on failure
 */
static int prv_updateRegistration(lwm2m_context_t * contextP,
     lwm2m_server_t * server,
     bool withObjects)
{
  uint8_t *payload = NULL;
  int payload_length = 0;
  qapi_Coap_Packet_t* messageP = NULL;
  qapi_Status_t result = QAPI_ERROR;
  qapi_Coap_Message_Params_t message_param;
  qapi_Coap_Block_Wise_Options_t blocktype = QAPI_COAP_BLOCK_OPTION_NONE;
  uint16_t blocksize = 0;

  if ( contextP == NULL || server == NULL ) 
  {
    LOG_DEBUG("Invalid input params - contextP[0x%x], server[0x%x]", contextP, server);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }   
 
  memset(&message_param, 0x00, sizeof(message_param));
 
  if (withObjects == true)
  {
    payload_length = get_registerPayload_size(contextP);
    if(payload_length <= 0)
      return LWM2M_500_INTERNAL_SERVER_ERROR;

    /* Adding padding of size two because 
    we are using 2 extra unit to prepare payload  */
    payload_length += BUFFER_PADDING_LENGTH;
  
    payload = (uint8_t *) malloc(payload_length);
  
    if(payload == NULL)
    {
       ds_iot_ulog_err("no memory for registeration payload");
       return LWM2M_500_INTERNAL_SERVER_ERROR;
    }
  
    memset(payload, 0x00, payload_length);
    
    payload_length = object_getRegisterPayload(contextP, server, payload, payload_length);
    if(payload_length == 0) 
    {
      lwm2m_free(payload);
      return LWM2M_500_INTERNAL_SERVER_ERROR;
    }
  } 
 
  result = qapi_Coap_Init_Message(server->coapHandle,&messageP, (qapi_Coap_Message_Type_t)QAPI_COAP_TYPE_CON, (qapi_Coap_Method_t)QAPI_COAP_POST);
  if (result != QAPI_OK)
  {
    lwm2m_free(payload);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  result = qapi_Coap_Set_Header(server->coapHandle, messageP, QAPI_COAP_URI_PATH, server->location, 0);
  if (result != QAPI_OK) 
  {
    ds_iot_ulog_err("Setting the URI path in the CoAP header failed");
    qapi_Coap_Free_Message(server->coapHandle, messageP);
    lwm2m_free(payload);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
    
  if (payload_length != 0) 
  {
    result = qapi_Coap_Set_Payload(server->coapHandle,messageP, payload, payload_length);
    if (result != QAPI_OK)
    {
      ds_iot_ulog_err("Setting the registration update payload in the CoAP message failed");
      qapi_Coap_Free_Message(server->coapHandle, messageP);
      lwm2m_free(payload);
      return LWM2M_500_INTERNAL_SERVER_ERROR;
    }
  }

  message_param.lastmid = 0;
  message_param.msg_cb = lwm2m_registration_update_cb;
  message_param.token= NULL;
  message_param.token_len=0;
  message_param.msgUsrData = (void *)server;

  blocksize = get_blockSize(server);
  if(blocksize >= 16 && messageP->payload_len > blocksize)
  { 
     blocktype  = QAPI_COAP_BLOCK1_OPTION;
  }
  else
  {
     blocksize = 0;
     blocktype  = QAPI_COAP_BLOCK_OPTION_NONE;
  }
  /* Update the active time for LWM2M client */
  active_time = lwm2m_gettime();
  /* Set the status to registration update pending */
  server->status = STATE_REG_UPDATE_PENDING;
 
  result = qapi_Coap_Send_Message_v2(server->coapHandle , messageP, &message_param, blocktype, blocksize);
  if (result != QAPI_OK)
  {
    ds_iot_ulog_high1("Sending \"registration update\" for SSID[%d] failed",server->shortID);
    qapi_Coap_Free_Message(server->coapHandle, messageP);
    server->status = STATE_DEREGISTERED;
    if(result != QAPI_NET_COAP_BLOCK_SESSION_IN_PROGRESS)
      lwm2m_clean_coap_session(server);
    lwm2m_free(payload);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  server->prevAttemptTime = lwm2m_gettime();
  ds_iot_ulog_high1("Sent \"registration update\" for SSID[%d]", server->shortID);

#ifdef PSM_LWM2M
lwm2m_psm_operation(contextP, 0, NULL, PSM_NODE_TYPE_MIN, PSM_NODE_UPDATE_ALL, PSM_MODIFY_NODE);


#endif
  lwm2m_free(payload);
  return LWM2M_NO_ERROR;
}


/**
 * @fn prv_updateRegistration()
 * @brief Function to send registration update to the server explicitly
 * @param contextP lwm2m context
 * @param shortServerID server information to which update needs to be sent
 * @param withObjects indicates if update with objects or not
 * @return LWM2M response type
 */
int lwm2m_update_registration(lwm2m_context_t * contextP,
    uint16_t shortServerID,
    bool withObjects)
{
  lwm2m_server_t * targetP = NULL;
  uint8_t result = LWM2M_NO_ERROR;

  ds_iot_ulog_high1("Sending the register update for SSID[%d]",shortServerID);

  if (contextP == NULL) 
  {
    LOG_DEBUG(" %s",nullError);
    return LWM2M_404_NOT_FOUND;
  }

  LOG_DEBUG("State: %s, shortServerID: %d", STR_STATE(contextP->state), shortServerID);

  if (IsInExtObjCreateWindow() == true)
  {
    ds_iot_ulog_err("Timer is running to wait ExtObj create");
    return result;
  }

  targetP = contextP->serverList;
  if (targetP == NULL)
  {
    object_getServers(contextP);
    targetP = contextP->serverList;
    if (targetP == NULL)
    {
      ds_iot_ulog_err("Retrieving the updated server information failed from the server object"); 
      return LWM2M_404_NOT_FOUND;
    }
  }

  while (targetP != NULL && result == LWM2M_NO_ERROR)
  {
    /* Send register update request to a specific short server ID */
    if (shortServerID != 0)
    { 
      if (targetP->shortID == shortServerID)
      {
        /* Client needs to be registered in order to send the register update */ 
        if (targetP->status != STATE_REGISTERED)
        {
          ds_iot_log_strf_err("Register update request failed due to invalid server id[%d] -> state[%s]",targetP->shortID, STR_STATUS(targetP->status));  
          return LWM2M_400_BAD_REQUEST;
        }
        
        LWM2M_SERVER_SET_UPDATE_ID(targetP->update_disable_check);

        /* Update request with object information */ 
        if (withObjects == TRUE)
          LWM2M_SERVER_SET_UPDATE_WITH_OBJECTS(targetP->update_disable_check);

        break;
      }
    }
    else 
    {
      /* Send register update requests to all the servers that are registered */ 
      if (targetP->status == STATE_REGISTERED)
      {
        LWM2M_SERVER_SET_UPDATE_ID(targetP->update_disable_check);
         
        /* Update request with object information */ 
        if (withObjects == TRUE)
          LWM2M_SERVER_SET_UPDATE_WITH_OBJECTS(targetP->update_disable_check);        
      
      }
    }

    targetP = targetP->next;
  }

  /* Server not found for the requested short ID */ 
  if ((shortServerID != 0) && (targetP == NULL))
  {
    ds_iot_ulog_err1("Server instance associated with the SSID[%d] not found in the server list", shortServerID); 
    return LWM2M_404_NOT_FOUND;
  }

  qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
  return result;
}


/**
 * @fn lwm2m_update_registration_for_write_on_server()
 * @brief Function to send registration update to the server explicitly
 * @param contextP lwm2m context
 * @param setflag flag to indicate the parameter that has been changed
 * @return LWM2M response type
 */
int lwm2m_update_registration_for_write_on_server(lwm2m_context_t * contextP,
    uint16_t shortServerID,
    uint8_t setflag)
{
  lwm2m_server_t * targetP = NULL;
  qapi_Status_t result = LWM2M_NO_ERROR;
  qapi_Coap_Message_Params_t message_param;
  qapi_Coap_Block_Wise_Options_t blocktype = QAPI_COAP_BLOCK_OPTION_NONE;
  uint16_t blocksize = 0;

  if ( contextP == NULL ) 
  {
    LOG_DEBUG(" %s",nullError);
    return LWM2M_404_NOT_FOUND;
  }

  memset(&message_param, 0x00, sizeof(message_param)); 

  targetP = contextP->serverList;
  if (targetP == NULL)
  {
    object_getServers(contextP);
    targetP = contextP->serverList;
    if (targetP == NULL)
    {
      ds_iot_ulog_err("Retrieving the updated server information failed from the server object"); 
      return LWM2M_404_NOT_FOUND;
    }
  }

  /* Retrieve the server information associated with the short server ID */ 
  targetP = (lwm2m_server_t *)LWM2M_LIST_FIND(contextP->serverList, shortServerID);
  if (NULL == targetP)
  {
    ds_iot_ulog_err1("Server instance associated with the SSID[%d] not found in the server list", shortServerID); 
    return LWM2M_404_NOT_FOUND;
  }

  if ((targetP != NULL) && (targetP->status == STATE_REGISTERED))
  {
    qapi_Coap_Packet_t * messageP = NULL;
    char query[30];
    int query_length = 0;

    query_length = prv_getRegistrationUpdateQuery(contextP, targetP, query, sizeof(query), setflag);

    if (query_length == 0) return LWM2M_500_INTERNAL_SERVER_ERROR;

    result = qapi_Coap_Init_Message(targetP->coapHandle,&messageP, (qapi_Coap_Message_Type_t)QAPI_COAP_TYPE_CON, (qapi_Coap_Method_t)QAPI_COAP_POST);
    if (result != QAPI_OK) 
    {
      return LWM2M_500_INTERNAL_SERVER_ERROR;
    }

    result = qapi_Coap_Set_Header(targetP->coapHandle ,messageP , QAPI_COAP_URI_PATH,targetP->location , 0);
    if (result != QAPI_OK)
	{
	  qapi_Coap_Free_Message(targetP->coapHandle ,messageP);
	  return LWM2M_500_INTERNAL_SERVER_ERROR;
    }

    result = qapi_Coap_Set_Header(targetP->coapHandle,messageP , QAPI_COAP_URI_QUERY, query , 0);

	if (result != QAPI_OK)
	{
	  qapi_Coap_Free_Message(targetP->coapHandle ,messageP);
	  return LWM2M_500_INTERNAL_SERVER_ERROR;
    }
	

    message_param.lastmid = 0;
    message_param.msg_cb = lwm2m_registration_update_cb;
    message_param.token = NULL;
    message_param.token_len = 0;
    message_param.msgUsrData = (void *)targetP;
    
    blocksize = get_blockSize(targetP);
    if(blocksize >= 16 && messageP->payload_len > blocksize)
    { 
       blocktype  = QAPI_COAP_BLOCK1_OPTION;
    }
    else
    {
       blocksize = 0;
       blocktype  = QAPI_COAP_BLOCK_OPTION_NONE;
    }
    // Update the active time for LWM2M client
    active_time = lwm2m_gettime();

    targetP->status = STATE_REG_UPDATE_PENDING;
    if (qapi_Coap_Send_Message_v2(targetP->coapHandle , messageP,&message_param, blocktype, blocksize ) != QAPI_OK)
    {
      qapi_Coap_Free_Message(targetP->coapHandle,messageP);
      if(result != QAPI_NET_COAP_BLOCK_SESSION_IN_PROGRESS)
        lwm2m_clean_coap_session(targetP);
      targetP->status = STATE_DEREGISTERED;
      return LWM2M_500_INTERNAL_SERVER_ERROR;
    }
    #ifdef PSM_LWM2M
	lwm2m_psm_operation(contextP, 0, NULL, PSM_NODE_TYPE_MIN, PSM_NODE_UPDATE_ALL, PSM_MODIFY_NODE);

    #endif
    ds_iot_ulog_high1("Registration Updating for server id: %d",targetP->shortID);
    result = LWM2M_NO_ERROR;
  }

  return result;
}


/**
 * @fn prv_updateRegistration()
 * @brief Function to send registration update to the server explicitly
 * @param contextP lwm2m context
 * @param shortServerID server information to which update needs to be sent
 * @param withObjects indicates if update with objects or not
 * @return LWM2M response type
 */
int lwm2m_update_registration_for_server(lwm2m_context_t * contextP,
 lwm2m_server_t * targetP,
    bool withObjects)
{
  if (targetP == NULL)
  {
    LOG_DEBUG ("In fun [%s] targetP is NULL",__func__);
    return LWM2M_404_NOT_FOUND;
  }

  targetP->isRegupdate_for_sms = true;
  
  if (LWM2M_SERVER_IS_UPDATE_SET(updt_disabl_flag))
  {
    LWM2M_SERVER_RESET_UPDATE_ID(updt_disabl_flag);
  }

  if (targetP->status == STATE_REGISTERED){
  	ds_iot_ulog_high("Reg Update:lwm2m_update_registration_for_server");
    return prv_updateRegistration(contextP, targetP, withObjects);
  }
  else
    return LWM2M_400_BAD_REQUEST;
}


/**
 * @fn registration_start()
 * @brief This function is used to start the registration procedure 
 *     for all the servers
 * @param contextP lwm2m context
 * @return LWM2M response type
 */
uint8_t registration_start(lwm2m_context_t * contextP, lwm2m_server_t *targetP, time_t * timeoutP)
{
  time_t currentTime = 0;
  uint8_t result = LWM2M_IGNORE;
  char ServerIp[48] = {0};
  bool per_server = LWM2M_FALSE;
  lwm2m_active_server_t *serverP = NULL;

  if ( contextP == NULL || contextP->activeServerList == NULL) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  LOG_DEBUG("State: %s", STR_STATE(contextP->state));

  if(targetP == NULL)
  {
    serverP = contextP->activeServerList;
  }
  else
  {
    serverP = contextP->activeServerList;
    serverP = (lwm2m_active_server_t *)LWM2M_LIST_FIND(serverP, targetP->shortID);
    per_server = LWM2M_TRUE;
  }

  /* Loop over only active servers.*/
  while (serverP != NULL )
  {
    result = LWM2M_IGNORE;  
    currentTime = lwm2m_gettime();
    /* Fetch the server form information from LwM2M context server list*/
    targetP = lwm2m_get_server(serverP->ssid);
    if(targetP == NULL)
    {
      if (per_server == LWM2M_TRUE)
      break;
      else
      {
        serverP = serverP->next;
        continue;
      }
    }

    /* If the state is disabled timestamp is not zero we need to wait 
     * until disable timesout, hence register only if disabled is 0
     */ 
    if ((targetP->status == STATE_DEREGISTERED && targetP->disabled == 0))
    {
      /* Wait for client hold off timer to expire then only attempt registration.*/
      if(currentTime >= targetP->initial_holdoff_time)
      {
        if(get_ip_for_carrier_apn_cfg(contextP->apn_cfg,targetP, ServerIp) != LWM2M_400_BAD_REQUEST)
        {
          result = prv_register(contextP, targetP);
          if(result == LWM2M_NO_ERROR)
          {
            targetP->retryTimeout = 0;
            targetP->retry_attempt = 0;
            //targetP->seq_retry_attempt = 0;

            if (persis_regstatus_feat_enabled == LWM2M_TRUE)
              lwm2m_carrier_update_regstatus(targetP);
          }                    
        }
        else
        {
          ds_iot_ulog_high1("No IP found yet for short id %d",targetP->shortID);
        }
      }
      else if (*timeoutP > (targetP->initial_holdoff_time - currentTime))
      {
         *timeoutP = targetP->initial_holdoff_time - currentTime;
          ds_iot_ulog_high1("Initial hold off time didn't expire for short server id %d Waiting.",targetP->shortID);
      }
      else
      {
        ds_iot_ulog_high1("Initial hold off time didn't expire for short server id %d Waiting ",targetP->shortID);
      }
    }

    if (per_server == LWM2M_TRUE)
      break;
    else
      serverP = serverP->next;
  }

  return result;
}

/**
 * @fn lwm2m_status_t get_current_regstatus()
 * @brief This function is used to get the current registration state  
 *     of the servers
 * @param contextP lwm2m context
 * @return LWM2M response type
 */

lwm2m_status_t get_current_regstatus(lwm2m_context_t * contextP)
{  
  lwm2m_server_t * targetP = NULL;
  lwm2m_status_t reg_status = STATE_REG_FAILED;
  if ( contextP == NULL )
  {
    LOG_DEBUG(" %s-> %s",nullError,__FUNCTION__);
    return reg_status;
  }
  targetP = contextP->serverList;
  while (targetP != NULL)
  {
    if(targetP->status == STATE_REGISTERED || targetP->status == STATE_REG_UPDATE_PENDING )
    {
      reg_status = targetP->status;

      break;
    }
    targetP = targetP->next;
  }
  return reg_status;
}
/**
 * @fn registration_getStatus()
 * @brief This function is used to get the registration status
 * @param contextP lwm2m context
 * Returns STATE_REG_PENDING if at least one registration is still pending
 * Returns STATE_REGISTERED if no registration is pending and there is at 
 *         least one server the client is registered to
 * Returns STATE_REG_FAILED if all registration failed.
 */
lwm2m_status_t registration_getStatus(lwm2m_context_t * contextP, bool *blocking)
{
  lwm2m_server_t * targetP = NULL;
  lwm2m_status_t reg_status;
  lwm2m_active_server_t *serverP = NULL;
  bool per_server = FALSE;

  if ( contextP == NULL || blocking == NULL)
  {
    LOG_DEBUG(" %s",nullError);
    return STATE_DEREGISTERED;
  }
  LOG_DEBUG("State: %s", STR_STATE(contextP->state));

  /* Loop over only active servers.*/
  serverP = contextP->activeServerList;
  reg_status = STATE_DEREGISTERED;

  while (serverP != NULL)
  {
    /* Fetch the server form information from LwM2M context server list*/
    targetP = lwm2m_get_server(serverP->ssid);
    if(targetP == NULL)
  {
      serverP = serverP->next;
      continue;
    }
    
    switch (targetP->status)
    {
      case STATE_REGISTERED:
      case STATE_REG_UPDATE_PENDING:
      {
        reg_status = STATE_REGISTERED;
        per_server = TRUE;
      }
        break;

      case STATE_REG_PENDING:
        if(targetP->bs_if_reg_fails == 1)
          reg_status = STATE_REG_PENDING;
        break;
      case STATE_REG_RETRYING:
        if(targetP->bs_if_reg_fails == 1)
          reg_status = STATE_REG_PENDING;
        break;
      case STATE_REG_FAILED:
      {
        /* If 5.03 error code is received so for VzW case if wait_time_for_error is configured
           then we skip below logic.*/
        if(g_carrier_cfg && targetP->last_code == LWM2M_503_SERVICE_UNAVAILABLE
           && data.cfg_info.wait_time_for_error)
        {
          //do nothing
        }
        else
        {
          if(targetP->version == (float)LWM2M_V_1_0)
          {
            if(targetP->bs_if_reg_fails == 1)
              reg_status = STATE_REG_FAILED;
          }
          else
          {
            /* When all the communication sequence are exhausted then perform 
               rebootstraping.*/
            if(targetP->bs_if_reg_fails == TRUE && 
               targetP->seq_retry_attempt >= targetP->seq_retry_count)
            {
              reg_status = STATE_REG_FAILED;
            }
            /* When all the communication sequence are exhausted for ordered 
               blocking server return Reg failed and blocking as TRUE.
               We will shutdown LwM2M in this scenario.*/
            else if(targetP->bs_if_reg_fails == FALSE && 
               targetP->seq_retry_attempt >= targetP->seq_retry_count
               && targetP->pri_order != UNDEFINED_PRIORITY && targetP->reg_failure == TRUE)
            {
              reg_status = STATE_REG_FAILED;
              *blocking = TRUE;
            }
          }
        }
        break;
      }
      case STATE_DEREG_PENDING:
      case STATE_DEREGISTERED:
        if(targetP->bs_if_reg_fails == 1)
          reg_status = STATE_DEREGISTERED;

        break;
      default:
        break;
    }
    LOG_DEBUG("reg_status: %s", STR_STATUS(reg_status));

    if (reg_status == STATE_REG_FAILED)
    {
      ds_iot_ulog_err1 ("Registration failed for server id %d",targetP->shortID);
      break;
    }
    serverP = serverP->next;
  }

  // If any of the server is move into Registered state and none of the server
  // state is Reg Failed then return STATE_REGISTERED
  if(per_server == TRUE && reg_status != STATE_REG_FAILED)
  {
    reg_status = STATE_REGISTERED;
  }
  return reg_status;
}

/**
 * @fn lwm2m_deregistration_cb()
 * @brief Callback function to handle de-registration response
 * @param transacP transaction pointer
 * @param message response message
 * @return  void
 */
static void lwm2m_deregistration_cb(qapi_Coap_Session_Hdl_t hdl ,qapi_Coap_Transaction_t * transacP,
    qapi_Coap_Packet_t * rsp_pkt)
{
  lwm2m_cb_util(hdl, transacP,rsp_pkt, LWM2M_DEREGISTRATION_CB);
}

/**
 * @fn prv_handleDeregistrationReply()
 * @brief function to handle de-registration response
 * @param transacP transaction pointer
 * @param message response message
 * @return  void
 */
void prv_handleDeregistrationReply(qapi_Coap_Session_Hdl_t hdl ,transaction_cb_info_t * transacP,
    qapi_Coap_Packet_t * rsp_pkt)
{
  lwm2m_server_t * targetP = NULL;

  if( transacP == NULL || hdl == NULL ) 
  {
    LOG_DEBUG(" %s",nullError);
    return;
  } 

  targetP = (lwm2m_server_t *)(transacP->userData);
  if (NULL != targetP)
  {
    if (targetP->status == STATE_DEREG_PENDING)
    {
      targetP->status = STATE_DEREGISTERED;

      ds_iot_ulog_high1("Deregistration successful for server id: %d",targetP->shortID);
    }
  }

  qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
  if(transacP->message)
  {
    qapi_Coap_Free_Message(hdl,transacP->message);
    transacP->message = NULL;
  }
  if(transacP)
    free(transacP);

  if(rsp_pkt)
   qapi_Coap_Free_Message(hdl,rsp_pkt);

  // Update the active time for LWM2M client
  active_time = lwm2m_gettime();
#ifdef PSM_LWM2M
lwm2m_psm_operation(data.lwm2mH, 0, NULL, PSM_NODE_TYPE_MIN, PSM_NODE_UPDATE_ALL, PSM_MODIFY_NODE);


#endif
}

/**
 * @fn registration_deregister()
 * @brief Function to send de-registration to the server
 * @param contextP lwm2m context
 * @param serverP server information to which update needs to be sent
 * @return void
 */
void registration_deregister(lwm2m_context_t * contextP,
    lwm2m_server_t * serverP)
{
  lwm2m_status_t init_status;
  qapi_Coap_Packet_t * messageP = NULL ;
  qapi_Status_t result = QAPI_ERROR;
  qapi_Coap_Message_Params_t message_param;


  if ( contextP == NULL || serverP == NULL ) 
  { 
    LOG_DEBUG(" %s",nullError);
    return ;
  }

  memset(&message_param,0,sizeof(message_param));
  init_status = serverP->status;

  LOG_DEBUG("State: %s, serverP->status: %s", STR_STATE(contextP->state), STR_STATUS(serverP->status));

  if (serverP->status == STATE_DEREGISTERED
      || serverP->status == STATE_REG_PENDING
      || serverP->status == STATE_DEREG_PENDING
      || serverP->status == STATE_REG_FAILED
      || serverP->location == NULL)
  {
    return;
  }

  result = qapi_Coap_Init_Message(serverP->coapHandle,&messageP, (qapi_Coap_Message_Type_t)QAPI_COAP_TYPE_CON, (qapi_Coap_Method_t)QAPI_COAP_DELETE);
  if (result != QAPI_OK) 
  {
    return ;
  }

  result = qapi_Coap_Set_Header(serverP->coapHandle,messageP , QAPI_COAP_URI_PATH,serverP->location , 0);
  if (result != QAPI_OK) 
  {
     qapi_Coap_Free_Message(serverP->coapHandle,messageP);
     return ;
  }
  message_param.lastmid = 0;
  message_param.msg_cb = lwm2m_deregistration_cb;
  message_param.token= NULL;
  message_param.token_len=0;
  message_param.msgUsrData = (void *)serverP;
  
  // Update the active time for LWM2M client
  active_time = lwm2m_gettime();

  serverP->status = STATE_DEREG_PENDING;

  if (qapi_Coap_Send_Message(serverP->coapHandle , messageP, &message_param) != QAPI_OK)
  {
    qapi_Coap_Free_Message(serverP->coapHandle,messageP);
    serverP->status = init_status;
  }
  
  #ifdef PSM_LWM2M
  lwm2m_psm_operation(contextP, 0, NULL, PSM_NODE_TYPE_MIN, PSM_NODE_UPDATE_ALL, PSM_MODIFY_NODE);

  #endif
  serverP->prevAttemptTime = lwm2m_gettime();
  ds_iot_ulog_high1("Deregistration for server id: %d",serverP->shortID);
}

#endif

/**
 * @fn registration_step()
 * @brief This function is used to perform registartion related
 *     functionalities 
 *     for each server update the registration if needed
 *     for each client check if the registration expired
 * @param contextP lwm2m context input   
 * @param currentTime input param needed to calculate the timeout value   
 * @param timeoutP output param which contains the actual timeout for
 *     select function  
 * @return  void
 */
int registration_step(lwm2m_context_t * contextP, time_t currentTime, time_t * timeoutP)
{
#ifdef LWM2M_CLIENT_MODE
  lwm2m_server_t * targetP = contextP->serverList;
  bool handle_reregister = false;
  lwm2m_object_t *serv_obj;
  char ServerIp[48] = {0};
  uint32_t coap_exchange_lifetime = 0;

  lwm2m_active_server_t *serverP = NULL;
  LOG_DEBUG("State: %s", STR_STATE(contextP->state));

  serverP = contextP->activeServerList;
  while(serverP != NULL)
  {
    ds_iot_ulog_high1("Active server short id [%d]", serverP->ssid);
    serverP = serverP->next;
  }

  /* Loop over only active servers.*/
  serverP = contextP->activeServerList;
  while (serverP != NULL)
  {
    /* Fetch the server form information from LwM2M context server list*/
    targetP = lwm2m_get_server(serverP->ssid);
    if(targetP == NULL)
    {
      serverP = serverP->next;
      continue;
    }
    switch (targetP->status)
    {
      case STATE_REGISTERED:
        {
          time_t nextUpdate = 0;
          time_t interval = 0;
          uint32_t disable_timeout = 0;
          int update_result = LWM2M_NO_ERROR;
          ds_iot_ulog_high1 ("registration_step() STATE_REGISTERED ShortID %d", targetP->shortID);
          if((g_carrier_cfg) && (targetP->isRgistered == 0))
          {
            serv_obj =  (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, LWM2M_SERVER_OBJECT_ID);
            if (serv_obj != NULL ) 
            {
              update_isRegistered_for_server(serv_obj,targetP->shortID,1);
              targetP->isRgistered = 1;
            }
          }
          /* Update the persistent file having extensible object associated with server.*/
          if(targetP->register_success == TRUE)
          {
            storeRegisteredExtObj(targetP, true);
            extObjRegUpdateCheck(targetP);
            targetP->register_success = FALSE;
          }

#ifdef LWM2M_SERVER_OBJECT
next_step:
          if (LWM2M_SERVER_IS_UPDATE_PROFILE_CHANGE_SET(targetP->update_disable_check))
          {
            carrier_apn_cfg_t *apn_info = NULL;        
        
            /* Retrieve the APN config associated with the server */
            if (NULL != (apn_info = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(data.lwm2mH->apn_cfg, targetP->shortID)))
            {
              bool send_reg_update = apn_info->send_update; 

              /* Close any existing CoAP related connection for the server at this point, as we are certain that the 
               * data call is going to be re-established. When the data call is re-established successfully, we would
               * automatically perform the CoAP connections for that particular data call.  
               */
              if (targetP->coapHandle != NULL)
              { 
                qapi_Coap_Close_Connection(targetP->coapHandle);
                targetP->sessionH = 0x00; 
              }
              
              /* Stop the data call associated with the short server ID (cleans-up all the internal information) */
              lwm2m_datacall_stop(data.lwm2mH, LWM2M_STOP_REGISTRATION_DATA_CALL, &targetP->shortID, false);  
  
              /* Restart the data call associated with the short server ID (generates the new context) */
              if (-1 == lwm2m_datacall_start(data.lwm2mH, LWM2M_START_REGISTRATION_DATA_CALL, &targetP->shortID, true))
                return LWM2M_503_SERVICE_UNAVAILABLE;

              /* As part of starting the data call, APN configuration is updated (use the latest configuration) */ 
              if (NULL != (apn_info = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(data.lwm2mH->apn_cfg, targetP->shortID)))
              {
                if ((LWM2M_APN_CLASS2 == apn_info->apn_class) && (true == send_reg_update))
                {
                  /* Primary APN is already connected at this point. Setting the re-connect flag at this point indicates that the
                   * we need to tear-down the existing connection and perform re-connection for the associated CoAP handle.
                   *
                   * Any "register update" that needs to be sent on the new transport or profile to the server is delayed until the 
                   * re-connection is performed.
                   */
                  apn_info->reconnect   = true;
                  apn_info->send_update = send_reg_update;

                  lwm2m_process_profile_change(data.lwm2mH, targetP);
                }
                else
                {   
                  /* Setting the re-connect flag at this point indicates that after the data call is up with the new configuration
                   * we need to tear-down the existing connection and perform re-connection for the associated CoAP handle.
                   *
                   * Any "register update" that needs to be sent on the new transport or profile to the server is delayed until the 
                   * re-connection is performed.
                   */   
                  apn_info->reconnect   = true;
                  apn_info->send_update = send_reg_update;
                }
              }             
            }

            LWM2M_SERVER_RESET_UPDATE_PROFILE_CHANGE(targetP->update_disable_check);
            goto next_step;
          }
          else if (LWM2M_SERVER_IS_UPDATE_SET(targetP->update_disable_check))
          {
            if (get_ip_for_carrier_apn_cfg(contextP->apn_cfg, targetP, ServerIp)!= LWM2M_400_BAD_REQUEST)
            {
              if (NULL == targetP->sessionH)
              {
                 targetP->sessionH = lwm2m_connect_server(targetP, contextP);
              }
              if(NULL != targetP->sessionH)
              {
                ds_iot_ulog_high1("Sending a registration update for SSID %d",targetP->shortID);
     
                if ((LWM2M_SERVER_IS_UPDATE_SET_LIFETIME(targetP->update_disable_check)) || 
                    (LWM2M_SERVER_IS_UPDATE_SET_BINDING(targetP->update_disable_check)))
                {
                  ds_iot_ulog_high("Reg Update,reg_step: Since SET_LIFETIME or SET_BINDING");
                     update_result = lwm2m_update_registration_for_write_on_server(contextP, targetP->shortID, targetP->update_disable_check);
                     if( update_result != LWM2M_500_INTERNAL_SERVER_ERROR)
                     {
                       if (LWM2M_SERVER_IS_UPDATE_SET_LIFETIME(targetP->update_disable_check))
                         LWM2M_SERVER_RESET_UPDATE_LIFETIME(targetP->update_disable_check);
            
                       if (LWM2M_SERVER_IS_UPDATE_SET_BINDING(targetP->update_disable_check))
                         LWM2M_SERVER_RESET_UPDATE_BINDING(targetP->update_disable_check);
                     }
                }
                else if (LWM2M_SERVER_IS_UPDATE_SET_OBJECTS(targetP->update_disable_check))
                {
                  ds_iot_ulog_high("Reg Update, reg_step: Since SET_OBJECTS");
                     update_result = prv_updateRegistration(contextP, targetP, true);
                     if ( update_result != LWM2M_500_INTERNAL_SERVER_ERROR)
                        LWM2M_SERVER_RESET_UPDATE_SET_OBJECTS(targetP->update_disable_check);
                }
                else
                {
                  ds_iot_ulog_high("Reg Update, reg_step");
                     update_result = prv_updateRegistration(contextP, targetP, false);
                }
                if ( update_result != LWM2M_500_INTERNAL_SERVER_ERROR)
                  LWM2M_SERVER_RESET_UPDATE_ID(targetP->update_disable_check);
              }
            }
          }          
          else if(LWM2M_SERVER_IS_SERVER_IP_CHANGE_SET(targetP->update_disable_check))
          {
            uint8_t res = 0;
            if(targetP->dns_count < MAX_DNS_ATTEMPT)
            {
              /* OEM needs to have a exponential backoff logic to call
                 lwm2m_check_server_ip_change() to check server IP changes or not.
                 After retries are exhausted, no need to call the lwm2m_check_server_ip_change().
              */
              //if(last_retry_time + backoff_timer > current_time)
              {
                res = lwm2m_check_server_ip_change(targetP, contextP);
                targetP->dns_count++;
                /* API will return 1 if Server IP address is changed, for all other 
                   cases 0 would be returned.*/
                if(res == 1)
                {
                  targetP->dns_count = 0;
                  LWM2M_SERVER_RESET_SERVER_IP_CHANGE_CHECK(targetP->update_disable_check);
                }
              }
            }
            else
            {
              targetP->dns_count = 0;
              LWM2M_SERVER_RESET_SERVER_IP_CHANGE_CHECK(targetP->update_disable_check);
            }
          }
          else if(LWM2M_SERVER_IS_DISABLE_SET(targetP->update_disable_check))
          {
            disable_timeout = (0 == targetP->disable_timeOut) ? LWM2M_SERVER_DEFAULT_DISABLE_TIMEOUT : targetP->disable_timeOut;
            ds_iot_ulog_high2("Trigerring Dereg since Disable Set ssid[%d], timeout[%d]",targetP->shortID, disable_timeout);
            registration_deregister(contextP, targetP);                            
            targetP->dirty = true;
            targetP->disabled = lwm2m_gettime();
            #ifdef PSM_LWM2M
              lwm2m_psm_operation(contextP, targetP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UPDATE, PSM_MODIFY_NODE);
            #endif
            timer_set(&targetP->regOpTimer, disable_timeout, 0, T_SEC);           
          } 
          else if(targetP->trigger_registration == TRUE)
          {
            if ((g_carrier_cfg) && (targetP->isRgistered == 1))
            {
              serv_obj =  (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList , LWM2M_SERVER_OBJECT_ID);
              if (serv_obj != NULL ) 
              {
                update_isRegistered_for_server(serv_obj,targetP->shortID,0);
                targetP->isRgistered = 0;
              }
            }
             targetP->trigger_registration = FALSE;
             ds_iot_ulog_high1("Trigger registration for %d", targetP->shortID); 
             prv_register(contextP, targetP);
          }
          else 
#endif  
          {   
            if (NULL == targetP->sessionH)
            {
              if (get_ip_for_carrier_apn_cfg(contextP->apn_cfg, targetP, ServerIp) != LWM2M_400_BAD_REQUEST)
              {
                targetP->sessionH = lwm2m_connect_server(targetP, contextP);
                if(0 == targetP->sessionH)
                {
                  ds_iot_ulog_err1("Connect for %d Server id failed ",targetP->shortID);
                }
                else
                {
                  LOG_DEBUG("Registration step server session %p",targetP->sessionH);

                  if (data.cfg_info.reg_update_on_reconnect== LWM2M_TRUE)
                    lwm2m_update_registration(contextP, targetP->shortID, false);
                }
              }
              else
              {
                ds_iot_ulog_high1("No IP address for Server id so not connecting %d ",targetP->shortID);
              }
            }

            nextUpdate = targetP->lifetime;

            ds_iot_ulog_high3("In reg step nextupdate %d : currenttime %d for short id -> %d ",nextUpdate,currentTime,targetP->shortID);

            #ifdef PSM_LWM2M
			lwm2m_psm_operation(contextP, targetP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UNCHANGED, PSM_MODIFY_NODE);

             #endif
             if((targetP->registration + nextUpdate) <= currentTime)
             {
               if(nextUpdate != 0 && (get_ip_for_carrier_apn_cfg(contextP->apn_cfg, targetP, ServerIp) != LWM2M_400_BAD_REQUEST))
               {
                 ds_iot_ulog_high("Re-registration flag is true so registration triggered"); 
 
                 targetP->reg_update_for_lifetime_expiry = false;
                 prv_register(contextP, targetP);
               }
             }
             else
             {
               interval = targetP->registration + nextUpdate - currentTime;
               ds_iot_ulog_high3("In reg step interval : %d : last registration %d timeout %d ",interval, targetP->registration, *timeoutP);
               if((interval <=  (int)(LWM2M_10PERCENT_OF_LIFETIME * nextUpdate)) || (interval <= LWM2M_PROCESSING_DELAY))
               {
                 if ((get_ip_for_carrier_apn_cfg(contextP->apn_cfg,targetP, ServerIp) != LWM2M_400_BAD_REQUEST) &&
                     (targetP->reg_update_for_lifetime_expiry == false))
                 {
                   ds_iot_ulog_high("Reg Update, reg_step: 10% lifetime");
                   prv_updateRegistration(contextP, targetP, false);
                   targetP->reg_update_for_lifetime_expiry = true;
                 }
                 else
                 {
                    ds_iot_ulog_high1("Not able to fetch IP for SSID : %d", targetP->shortID);
                 }
               }
             }

            /* Comparing the interval and timeoutP , so that which ever is less that value is taken for processing signal 
               i.e timeoutP, so regupdate will happen properly. Here interval is time remaining to send registration update */
            if ((interval > 0) && ((interval  < (*timeoutP + LWM2M_PROCESSING_DELAY))))
            {
              if (interval > LWM2M_PROCESSING_DELAY) 
                *timeoutP =  interval - LWM2M_PROCESSING_DELAY;
              else 
                *timeoutP  = interval;

              ds_iot_ulog_high1("timeout changed to :%d",*timeoutP);
            }
          }
        }
        break;

      case STATE_REG_FAILED:
#ifdef LWM2M_SERVER_OBJECT 
      case STATE_DEREGISTERED:
#endif
        ds_iot_ulog_err1 ("STATE_REG_FAILED/STATE_DEREGISTERED ShortID %d", targetP->shortID);

        cleanExtObjInfoList(targetP);
        if(IsInExtObjCreateWindow() == true)
        {
	      ds_iot_ulog_err("Timer is running to wait ExtObj create, no register action now");
          break;
        }

        if((g_carrier_cfg) && (targetP->isRgistered == 1))
        {
          serv_obj =  (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList , LWM2M_SERVER_OBJECT_ID);
          if (serv_obj != NULL ) 
          {
            update_isRegistered_for_server(serv_obj,targetP->shortID,0);
            targetP->isRgistered = 0;
          }
        }

        if (targetP->sessionH != NULL)
        {
          qapi_Coap_Close_Connection(targetP->coapHandle);
          targetP->sessionH = NULL;
          /* VZW_REQ_OTADMLWM2M_41031
           * Updating the registration status to configuration file
           */
          if (persis_regstatus_feat_enabled == LWM2M_TRUE) 
            lwm2m_carrier_update_regstatus(targetP);
        }
#ifdef LWM2M_SERVER_OBJECT
        if(LWM2M_SERVER_IS_DISABLE_SET(targetP->update_disable_check))
        {
          uint32_t disable_timeout = 0;
          uint32 current_time = 0;
          disable_timeout = (0 == targetP->disable_timeOut) ? LWM2M_SERVER_DEFAULT_DISABLE_TIMEOUT : targetP->disable_timeOut;
          #ifdef PSM_LWM2M
		  lwm2m_psm_operation(contextP, targetP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UNCHANGED, PSM_MODIFY_NODE);

          #endif

          current_time = lwm2m_gettime();
          if(current_time >= (targetP->disabled + disable_timeout))
          {                                                                      
            // if the timer is just about to expire
            timer_clr(&targetP->regOpTimer, T_SEC);
			ds_iot_ulog_high1("Sending REG REQ for disabled ssid[%d] after disable timeout", targetP->shortID);
            prv_register(contextP, targetP);
            // Disabling sleep after disable timeout expires
            if(LWM2M_TRUE == lwm2m_in_sleep_state)
              lwm2m_in_sleep_state = LWM2M_FALSE;     
            LWM2M_SERVER_RESET_SERVER_ID(targetP->update_disable_check);
          }
          else if (*timeoutP > ((targetP->disabled + disable_timeout) - currentTime))
          {
             *timeoutP = (targetP->disabled + disable_timeout) - currentTime;
          }
        }
        else
        {
          if((contextP->state != STATE_INITIAL) && (contextP->state != STATE_BOOTSTRAP_REQUIRED) && (contextP->state != STATE_BOOTSTRAPPING))
          {
            if(targetP->status == STATE_DEREGISTERED)
            {
              registration_start(contextP, targetP,timeoutP);
            }
            else if(targetP->status == STATE_REG_FAILED)
            {
              #ifdef PSM_LWM2M
			  lwm2m_psm_operation(contextP, targetP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UNCHANGED, PSM_MODIFY_NODE);

              #endif
              /* If all the communication sequence are exhausted move the server
                 to Reg Failed state and set next attempt to zero.*/
              if(targetP->seq_retry_attempt >= targetP->seq_retry_count)
              {
                targetP->seq_retry_attempt = targetP->seq_retry_count;
                targetP->retry_attempt = targetP->retry_count;
                targetP->status = STATE_REG_FAILED;
                targetP->next_cycle_for_registration = 0;
              }
              if((targetP->next_cycle_for_registration > 0) && (currentTime >= targetP->next_cycle_for_registration ))
              {
                targetP->status = STATE_DEREGISTERED;
                registration_start(contextP, targetP,timeoutP);
                //targetP->next_cycle_for_registration  = 0;
                if (persis_regstatus_feat_enabled == LWM2M_TRUE)
                  lwm2m_carrier_update_regstatus(targetP);
              }
              else if ((targetP->next_cycle_for_registration > 0) && (*timeoutP > (targetP->next_cycle_for_registration - currentTime)))
              {
                *timeoutP = targetP->next_cycle_for_registration - currentTime;
              }

              /* If all the communication sequence are exhausted the server is 
                 removed from LwM2M context server list.*/
              if(targetP->next_cycle_for_registration == 0 && targetP->bs_if_reg_fails == FALSE 
                 && ((targetP->reg_failure == FALSE) || (targetP->reg_failure == TRUE && targetP->pri_order == UNDEFINED_PRIORITY)))
              {
                lwm2m_server_t *server = NULL;
                contextP->serverList = (lwm2m_server_t *)LWM2M_LIST_RM(contextP->serverList, targetP->shortID, &server);
                ds_iot_ulog_high("Removing non-blocking server as all attempts are exhauted.");
                prv_deleteServer(server, contextP, TRUE);
                qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);
                return 0;
              }
            }
          }
        }
#endif      
        break;

      case STATE_REG_RETRYING:
        {
          ds_iot_ulog_high1("LWM2M_LOG; registration_step() STATE_REG_RETRYING ShortID %d", targetP->shortID);
          #ifdef PSM_LWM2M
          lwm2m_psm_operation(contextP, targetP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UNCHANGED, PSM_MODIFY_NODE);
          #endif
          if (currentTime >= (targetP->prevAttemptTime + targetP->retryTimeout))
          {
            if (get_ip_for_carrier_apn_cfg(contextP->apn_cfg,targetP, ServerIp) != LWM2M_400_BAD_REQUEST)
            {
              prv_register(contextP, targetP);
              if(targetP->isFallbackV1_0)
                targetP->isFallbackV1_0 = FALSE;
            }
          }
          else if (*timeoutP > ((targetP->prevAttemptTime + targetP->retryTimeout) - currentTime))
          {
             *timeoutP = (targetP->prevAttemptTime + targetP->retryTimeout) - currentTime;
          }
        }
        break;

      case STATE_REG_UPDATE_PENDING:
      {
        qapi_Coap_Get_Exchange_Lifetime(targetP->coapHandle, &coap_exchange_lifetime);
        ds_iot_ulog_high1("STATE_REG_UPDATE_PENDING Exchange Lifetime : %d",coap_exchange_lifetime);
        if (LWM2M_SERVER_IS_UPDATE_PROFILE_CHANGE_SET(targetP->update_disable_check))
        {
          carrier_apn_cfg_t *apn_info = NULL;        
        
          /* Retrieve the APN config associated with the server */
          if (NULL != (apn_info = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(data.lwm2mH->apn_cfg, targetP->shortID)))
          {
            bool send_reg_update = apn_info->send_update; 
        
            /* Close any existing CoAP related connection for the server at this point, as we are certain that the 
             * data call is going to be re-established. When the data call is re-established successfully, we would
             * automatically perform the CoAP connections for that particular data call.  
             */
            if (targetP->coapHandle != NULL)
            { 
              qapi_Coap_Close_Connection(targetP->coapHandle);
              targetP->sessionH = 0x00; 
            }
            
            /* Stop the data call associated with the short server ID (cleans-up all the internal information) */
            lwm2m_datacall_stop(data.lwm2mH, LWM2M_STOP_REGISTRATION_DATA_CALL, &targetP->shortID, false);  
        
            /* Restart the data call associated with the short server ID (generates the new context) */
            if (-1 == lwm2m_datacall_start(data.lwm2mH, LWM2M_START_REGISTRATION_DATA_CALL, &targetP->shortID, true))
              return LWM2M_503_SERVICE_UNAVAILABLE;
        
            /* As part of starting the data call, APN configuration is updated (use the latest configuration) */ 
            if (NULL != (apn_info = (carrier_apn_cfg_t *)LWM2M_LIST_FIND(data.lwm2mH->apn_cfg, targetP->shortID)))
            {
              if ((LWM2M_APN_CLASS2 == apn_info->apn_class) && (true == send_reg_update))
              {
                /* Primary APN is already connected at this point. Setting the re-connect flag at this point indicates that the
                 * we need to tear-down the existing connection and perform re-connection for the associated CoAP handle.
                 *
                 * Any "register update" that needs to be sent on the new transport or profile to the server is delayed until the 
                 * re-connection is performed.
                 */
                apn_info->reconnect   = true;
                apn_info->send_update = send_reg_update;
        
                lwm2m_process_profile_change(data.lwm2mH, targetP);
              }
              else
              {   
                /* Setting the re-connect flag at this point indicates that after the data call is up with the new configuration
                 * we need to tear-down the existing connection and perform re-connection for the associated CoAP handle.
                 *
                 * Any "register update" that needs to be sent on the new transport or profile to the server is delayed until the 
                 * re-connection is performed.
                 */   
                apn_info->reconnect   = true;
                apn_info->send_update = send_reg_update;
              }
            }             
          }
        
          LWM2M_SERVER_RESET_UPDATE_PROFILE_CHANGE(targetP->update_disable_check);
        }
		else
		{
          if((currentTime >= (targetP->prevAttemptTime + coap_exchange_lifetime)))
          {
             targetP->status = STATE_REGISTERED;
          }
          else if (*timeoutP > (targetP->prevAttemptTime + coap_exchange_lifetime - currentTime))
          {
            *timeoutP = targetP->prevAttemptTime + coap_exchange_lifetime - currentTime;
          }
		}

        break; 
      }

	  case STATE_REG_PENDING:
	  {
        qapi_Coap_Get_Exchange_Lifetime(targetP->coapHandle, &coap_exchange_lifetime);
        ds_iot_ulog_high1("STATE_REG_PENDING Exchange Lifetime : %d",coap_exchange_lifetime);

        if((currentTime >= (targetP->prevAttemptTime + coap_exchange_lifetime)))
        {
          trigger_retry(targetP);
        }
        else if (*timeoutP > (targetP->prevAttemptTime + coap_exchange_lifetime - currentTime))
        {
          *timeoutP = targetP->prevAttemptTime + coap_exchange_lifetime - currentTime;
        }
        break;
	  }
	  case STATE_DEREG_PENDING:
	  {
        qapi_Coap_Get_Exchange_Lifetime(targetP->coapHandle, &coap_exchange_lifetime);
        ds_iot_ulog_high1("STATE_DEREG_PENDING Exchange Lifetime : %d",coap_exchange_lifetime);

        if((currentTime >= (targetP->prevAttemptTime + coap_exchange_lifetime)))
        {
          if(LWM2M_SERVER_IS_DISABLE_SET(targetP->update_disable_check))
          {
            targetP->status = STATE_REGISTERED;
          }
		  else
		  {
		    targetP->status = STATE_DEREGISTERED;
		  }
        }
        else if (*timeoutP > (targetP->prevAttemptTime + coap_exchange_lifetime - currentTime))
        {
          *timeoutP = targetP->prevAttemptTime + coap_exchange_lifetime - currentTime;
        }
        break;
	  }
      default:
        break;
    }
    serverP = serverP->next;
  }

  if(handle_reregister == LWM2M_TRUE)
    contextP->state = STATE_REGISTER_REQUIRED;

#endif

  return 0; 
}


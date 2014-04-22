/******************************************************************************

  @file    bootstrap.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/

/*******************************************************************************
 *
 * Copyright (c) 2015 Sierra Wireless and others.
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
 *    Pascal Rieux - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    David Navarro, Intel Corporation - Please refer to git log
 *
 *******************************************************************************/

#include "internals.h"
#include "object_security.h"
#include "lwm2m_carrier.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "object_server.h"
#include "lwm2m_signals.h"
#include "lwm2mclient.h"
#include "lwm2m_rx.h"
#include "ds_log.h"


#ifdef LWM2M_CLIENT_MODE
#ifdef LWM2M_BOOTSTRAP

#define PRV_QUERY_BUFFER_LENGTH 200
extern uint32_t active_time;
extern bool lwm2m_in_sleep_state;

// Bootstrap Registration Retry Information
extern time_t gRegRetryInitialTimeout;
extern time_t gRegRetrytimeoutExponent;
extern time_t gRegRetryMaxTimeout;
extern bool persis_regstatus_feat_enabled;
extern qurt_signal_t lwm2m_signal_Handle;
extern bool enable_bs_server_replace;
/**
 * @fn static void prv_bootstrapFailed()
 * @brief This function is used to handle bootstrap failure
 *
 * @param bootstrapServer   lwm2m_server_t server pointer
 *
 * @return      void
 */
static void prv_bootstrapFailed(lwm2m_server_t * bootstrapServer)
{
  bootstrapServer->status = STATE_BS_FAILED;
  bootstrapServer->retryTimeout = 0;
  bootstrapServer->prevAttemptTime = 0;
  bootstrapServer->next_cycle_for_registration = lwm2m_gettime() + data.cfg_info.wait_time_for_rereg;
#ifdef PSM_LWM2M
  lwm2m_psm_operation(data.lwm2mH, bootstrapServer->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UPDATE, PSM_MODIFY_NODE);
#endif
}

/**
 * @fn static void trigger_bs_retry()
 * @brief This function is used to handle bootstrap failure
 *
 * @param bootstrapServer   lwm2m_server_t server pointer
 *
 * @return      void
 */
void trigger_bs_retry(lwm2m_server_t* bootstrapServer)
{
  if(bootstrapServer == NULL)
    return;
  if(bootstrapServer->retryTimeout < gRegRetryMaxTimeout)
  {
    bootstrapServer->status = STATE_BS_REG_RETRYING;
    bootstrapServer->prevAttemptTime = lwm2m_gettime();

    if(0 == bootstrapServer->retryTimeout)
    {
      bootstrapServer->retryTimeout = gRegRetryInitialTimeout;
    }
    else
    {
      bootstrapServer->retryTimeout *= gRegRetrytimeoutExponent ;
    }
    timer_set(&bootstrapServer->regOpTimer, bootstrapServer->retryTimeout, 0, T_SEC);
    ds_iot_ulog_high1("BS retry after %d secs", bootstrapServer->retryTimeout);
  }
  else
  {
    ds_iot_ulog_err("BS Connection retry count exceed, state changing to STATE_BS_FAILED");
    prv_bootstrapFailed(bootstrapServer);
  }

  if (persis_regstatus_feat_enabled == LWM2M_TRUE) 
    lwm2m_carrier_update_regstatus(bootstrapServer);

  #ifdef PSM_LWM2M
  lwm2m_psm_operation(data.lwm2mH, bootstrapServer->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UPDATE, PSM_MODIFY_NODE);
  #endif
}

/**
 * @fn static void prv_handleResponse()
 * @brief This function is used to handle bootstrap failure
 *
 * @param bootstrapServer   lwm2m_server_t server pointer
 * @param message           coap_packet_t message pointer
 *
 * @return      void
 */
static void prv_handleResponse(lwm2m_server_t * bootstrapServer,
    qapi_Coap_Packet_t * message)
{
  if (LWM2M_204_CHANGED == message->code)
  {
    ds_iot_ulog_high("Received ACK/2.04, Bootstrap pending, waiting for DEL/PUT from BS server...");
    bootstrapServer->status = STATE_BS_PENDING;
  }
  else
  {
    prv_bootstrapFailed(bootstrapServer);
  }
}

/**
 * @fn void lwm2m_Bootstrap_cb()
 * @brief This function is used to handle bootstrap reply
 *
 * @param transaction   coap_transaction_t pointer
 * @param message       void message pointer
 *
 * @return      void
 */
static void lwm2m_Bootstrap_cb(qapi_Coap_Session_Hdl_t hdl ,qapi_Coap_Transaction_t * transaction,
    qapi_Coap_Packet_t * rsp_pkt)
{
  lwm2m_cb_util(hdl, transaction ,rsp_pkt, LWM2M_BOOTSTRAP_CB);
}


/**
 * @fn void prv_handleBootstrapReply()
 * @brief This function is used to handle bootstrap reply
 *
 * @param transaction   coap_transaction_t pointer
 * @param message       void message pointer
 *
 * @return      void
 */
void prv_handleBootstrapReply(qapi_Coap_Session_Hdl_t hdl ,transaction_cb_info_t *transaction,
    qapi_Coap_Packet_t * rsp_pkt)
{
  lwm2m_server_t * bootstrapServer = NULL;
  qapi_Coap_Packet_t * coapMessage;
  
  if((hdl == NULL) || (transaction == NULL))
  return;

  bootstrapServer = (lwm2m_server_t *)transaction->userData;
  if (bootstrapServer == NULL)
  {
    return;
  }
  coapMessage = (qapi_Coap_Packet_t *)rsp_pkt;

  if(LWM2M_IS_INVALIDE_PACKETS(rsp_pkt))
  {
    LOG_DEBUG("BS reply: received invalid packet");
    prv_bootstrapFailed(bootstrapServer);
    if(persis_regstatus_feat_enabled == LWM2M_TRUE) 
      lwm2m_carrier_update_regstatus(bootstrapServer);
    goto end;
  }

  ds_iot_ulog_high("BS reply received from Server");
  if (bootstrapServer->status == STATE_BS_INITIATED)
  {
    if ((NULL == coapMessage || LWM2M_403_FORBIDDEN == coapMessage->code || LWM2M_400_BAD_REQUEST == coapMessage->code ) && (bootstrapServer->retryTimeout < gRegRetryMaxTimeout)) 
    {
      if (coapMessage == NULL)
      {
        ds_iot_ulog_err("Received NULL reponse for BS request");
      }
      else
      {
        ds_iot_ulog_err("Received 403 forbidden reponse for BS request");
      }

      LOG_DEBUG("BS RETRY SET:BSReply");
      trigger_bs_retry(bootstrapServer);
    }
    else
    { 
      if (NULL != coapMessage && QAPI_COAP_TYPE_RST != coapMessage->type)
      {
        prv_handleResponse(bootstrapServer, coapMessage);
        bootstrapServer->last_BS_Operation_Time = lwm2m_gettime();
      }
      else
      {
        prv_bootstrapFailed(bootstrapServer);
      }
      
      if(persis_regstatus_feat_enabled == LWM2M_TRUE) 
        lwm2m_carrier_update_regstatus(bootstrapServer);
    }
  }
end:
  if(NULL != transaction->message)
  {
    qapi_Coap_Free_Message(hdl,transaction->message);
    transaction->message = NULL;
  }
  if(NULL != rsp_pkt)
    qapi_Coap_Free_Message(hdl,rsp_pkt);
  if(transaction)
    free(transaction);
}

/**
 * @fn static void prv_requestBootstrap()
 * @brief This function is used to start a device initiated bootstrap
 *
 * @param contextP          lwm2m_context_t pointer
 * @param bootstrapServer   lwm2m_server_t server pointer
 *
 * @return      void
 */
static void prv_requestBootstrap(lwm2m_context_t * context,
    lwm2m_server_t * bootstrapServer)
{
  char query[PRV_QUERY_BUFFER_LENGTH];
  qapi_Coap_Packet_t *messageP = NULL;
  qapi_Coap_Message_Params_t qapi_message;
  int query_length = 0;
  int res;

  memset(&qapi_message, 0, sizeof(qapi_Coap_Message_Params_t));

  if (context == NULL || bootstrapServer == NULL ) 
  {
    ds_iot_ulog_err(" NULL Error");
    return ;
  }

  query_length = utils_stringCopy(query, PRV_QUERY_BUFFER_LENGTH, "?ep=");
  if (query_length < 0)
  {
    bootstrapServer->status = STATE_BS_FAILED;
    return;
  }
  if( NULL == context->bootstrapEpName)
  {
    ds_iot_ulog_err("Bootstrap EP name is NULL");
    return;
  }
  res = utils_stringCopy(query + query_length, PRV_QUERY_BUFFER_LENGTH - query_length, context->bootstrapEpName);
  if (res < 0)
  {
    bootstrapServer->status = STATE_BS_FAILED;
    return;
  }
  query_length += res;

  if (bootstrapServer->sessionH == 0)
  {
    bootstrapServer->sessionH = lwm2m_connect_server(bootstrapServer , context); 
    LOG_DEBUG (" New session for BS server");
  }

  if (bootstrapServer->sessionH  == 1)
  {
    LOG_DEBUG("Bootstrap server connection opened");
    if (QAPI_OK != qapi_Coap_Init_Message(bootstrapServer->coapHandle ,&messageP, QAPI_COAP_TYPE_CON ,QAPI_COAP_POST ))
    {
      return;
    }
    if(QAPI_OK != qapi_Coap_Set_Header(bootstrapServer->coapHandle,messageP , QAPI_COAP_URI_PATH, "/"URI_BOOTSTRAP_SEGMENT , 0))
    {
      qapi_Coap_Free_Message(bootstrapServer->coapHandle,messageP);
      return;
    }
     if (qapi_Coap_Set_Header(bootstrapServer->coapHandle,messageP , QAPI_COAP_URI_QUERY, query , 0) != QAPI_OK)
     {
       qapi_Coap_Free_Message(bootstrapServer->coapHandle,messageP);
       return;
     }

    // Update the active time for LWM2M client
    active_time = lwm2m_gettime();
    lwm2m_in_sleep_state = LWM2M_FALSE;

    qapi_message.msgUsrData = (void *)bootstrapServer;
    qapi_message.msg_cb = lwm2m_Bootstrap_cb;
    qapi_message.token = NULL;
    qapi_message.token_len = 0;

    ds_iot_ulog_high("Sending Bootstrap request");
    if(qapi_Coap_Send_Message(bootstrapServer->coapHandle,messageP,&qapi_message ) == 0)
    {
      bootstrapServer->status = STATE_BS_INITIATED;
      bootstrapServer->prevAttemptTime = lwm2m_gettime();
      if (persis_regstatus_feat_enabled == LWM2M_TRUE) 
        lwm2m_carrier_update_regstatus(bootstrapServer);
    } 
    else 
    {
      qapi_Coap_Free_Message(bootstrapServer->coapHandle,messageP);
      lwm2m_clean_coap_session(bootstrapServer);
      ds_iot_ulog_err("Sending BS request failed");
      /* Exponentially retry before bailing out */
      trigger_bs_retry(bootstrapServer);
      return;
    }
  }
  else
  {
    LOG_DEBUG(" Connecting bootstrap server failed");
    trigger_bs_retry(bootstrapServer);
  }
}

/**
 * @fn void bootstrap_step()
 * @brief This function performs action based on the current bootstrap state 
 *
 * @param contextP    lwm2m_context_t pointer
 * @param currentTime current time  
 * @param timeoutP    handle to timeout value
 *
 * @return      void
 */
void bootstrap_step(lwm2m_context_t * contextP,
    uint32_t currentTime,
    time_t* timeoutP)
{
  lwm2m_server_t * targetP = NULL;
  uint32_t coap_exchange_lifetime;
  
  if (contextP == NULL || timeoutP == NULL ) 
  {
    ds_iot_ulog_err("bootstrap_step: NULL Error");
    return;
  }

  LOG_DEBUG("entering");
  targetP = contextP->bootstrapServer;
  if (targetP == NULL) {
    ds_iot_ulog_err("Bootstrap server is NULL");
    return;
  }

  ds_iot_log_strf_high("bs_step:%s",STR_STATUS(targetP->status));
  coap_exchange_lifetime = 0;
  switch (targetP->status)
  {
    case STATE_DEREGISTERED:
    {
      uint32_t bootstrap_wait_time = targetP->lifetime; 

      /* We do configure the "maximum bootstrap wait" period in the lwm2m config file. In addition to that, the OEMs 
       * can provide the value as part of the server object. In order to increase the window for application object 
       * creation we do take the maximum of these values allowing the device to report to accurately all the newly 
       * created extensible objects in the bootstrap process and get the relevant permissions for these objects. */
      if (data.cfg_info.max_bootstrap_wait > bootstrap_wait_time) 
        bootstrap_wait_time = data.cfg_info.max_bootstrap_wait;

      /* Update the bootstrap registration time */  
      targetP->registration = currentTime + bootstrap_wait_time;
      
      /* Update the time-out value to be within the configured boot-strap wait time */
      if (*timeoutP > bootstrap_wait_time)
        *timeoutP = bootstrap_wait_time;

      targetP->status = STATE_BS_HOLD_OFF;
      break;
    }

    case STATE_BS_HOLD_OFF:
    {
      if (targetP->registration <= currentTime)
      {
        qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
        /* Indicate that the bootstrap window has been closed and no further extensible object creations are accepted */ 
        data.glwm2m_app_ctx.bs_performed = TRUE;
        /* Indicate that the application extended object creation window during bootstrapping process is complete.
         * Any further extended object creations result in a failure to the caller. */ 
        data.glwm2m_app_ctx.lwm2m_state = STATE_BOOTSTRAPPING;
        qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex); 

        /* Indicate the applications that the LWM2M client has successfully completed bootstrapping with the server */
        lwm2m_send_internal_ind_to_application(QAPI_NET_LWM2M_BOOTSTRAP_START_E);

        prv_requestBootstrap(contextP, targetP);
      }
      else if (*timeoutP > (targetP->registration - currentTime))
      {
        *timeoutP = targetP->registration - currentTime;
      }

      break;
    }

    case STATE_BS_INITIATED:
      qapi_Coap_Get_Exchange_Lifetime(targetP->coapHandle,&coap_exchange_lifetime);

      if((currentTime >= (targetP->prevAttemptTime + coap_exchange_lifetime)))
      {
        trigger_bs_retry(targetP);
      }
      else if (*timeoutP > (targetP->prevAttemptTime + coap_exchange_lifetime - currentTime))
      {
        *timeoutP = targetP->prevAttemptTime + coap_exchange_lifetime - currentTime;
      }
      break;

    case STATE_BS_PENDING:
       qapi_Coap_Get_Exchange_Lifetime(targetP->coapHandle,&coap_exchange_lifetime);
       
      if((targetP->last_BS_Operation_Time > 0) && (currentTime >= (targetP->last_BS_Operation_Time + coap_exchange_lifetime)))
      {
        trigger_bs_retry(targetP);
      }
      else if (*timeoutP > (targetP->last_BS_Operation_Time + coap_exchange_lifetime - currentTime))
      {
        *timeoutP = targetP->last_BS_Operation_Time + coap_exchange_lifetime - currentTime;
      }
      break;

    case STATE_BS_FINISHED:
        //if (targetP->sessionH != NULL)
        //{
        //qapi_coap_close_connection(targetP->sessionH, targetP->coapHandle);
        //targetP->sessionH = NULL;
        //}
      break;

    case STATE_BS_FAILED:
    {
      if (targetP->sessionH != NULL)
      {
        qapi_Coap_Close_Connection(targetP->coapHandle);
        targetP->sessionH = NULL;
      }
      if((targetP->next_cycle_for_registration > 0) && (currentTime > targetP->next_cycle_for_registration ))
      {
        targetP->status = STATE_DEREGISTERED;
        prv_requestBootstrap(contextP, targetP);
        targetP->next_cycle_for_registration  = 0;
        if (persis_regstatus_feat_enabled == LWM2M_TRUE)
          lwm2m_carrier_update_regstatus(targetP);
      }
      else if((targetP->next_cycle_for_registration > 0) && (*timeoutP > (targetP->next_cycle_for_registration - currentTime)))
      {
        *timeoutP = targetP->next_cycle_for_registration - currentTime;
      }
    }
    break;

    case STATE_BS_REG_RETRYING:
      {
        #ifdef PSM_LWM2M
        lwm2m_psm_operation(contextP, targetP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UNCHANGED, PSM_MODIFY_NODE);
        #endif

        if(enable_bs_server_replace && targetP->isBSServerAccountUpdated)
        {
          uint16_t old_ssid = targetP->shortID;
          ds_iot_ulog_high("BS server account is updated, trigger BS again immediately");
          if (targetP->sessionH != NULL)
          {
            qapi_Coap_Close_Connection(targetP->coapHandle);
            targetP->coapHandle = NULL;
            targetP->sessionH = NULL;
          }

          prv_refreshBSServer(contextP);

          targetP = contextP->bootstrapServer;

          if(targetP != NULL)
          {
            update_bs_apn_info(contextP, old_ssid, targetP->shortID);
            targetP->status = STATE_DEREGISTERED;
            prv_requestBootstrap(contextP, targetP);
            targetP->next_cycle_for_registration = 0;
            if (persis_regstatus_feat_enabled == LWM2M_TRUE)
              lwm2m_carrier_update_regstatus(targetP);
          }
          else
          {
            ds_iot_ulog_err("Bootstrap server is NULL");
            return;
          }
        }
        else if(currentTime >= (targetP->prevAttemptTime + targetP->retryTimeout))
        {
          if (targetP->sessionH != NULL)
          {
            LOG_DEBUG("close CoAP connection before doing BS retry");
            qapi_Coap_Close_Connection(targetP->coapHandle);
            targetP->coapHandle = NULL;
            targetP->sessionH = NULL;
          }
          LOG_DEBUG("prv_requestBootstrap");
          prv_requestBootstrap(contextP, targetP);
        }
        else if (*timeoutP > (targetP->prevAttemptTime +  targetP->retryTimeout - currentTime))
        {
          *timeoutP = targetP->prevAttemptTime +  targetP->retryTimeout - currentTime;
        }
      }
      break;
    default:
      break;
  }
  LOG_DEBUG("Final status: %s", STR_STATUS(targetP->status));
}

/**
 * @fn coap_status_t bootstrap_handleFinish()
 * @brief This function is used to set the bootstrap server status to finished
 *
 * @param context handle to lwm2m context
 * @param fromSessionH handle to session
 *
 * @return  COAP response type
 */
uint8_t bootstrap_handleFinish(lwm2m_context_t * context, void * fromSessionH)
{
  lwm2m_server_t   * bootstrapServer = NULL;
  lwm2m_server_t    *target_serlistP = NULL;
  lwm2m_object_t            *sec_obj = NULL;
  void                  *coap_handle = NULL;

  ds_iot_ulog_high(" Bootstrap FINISH");

  if(context == NULL || fromSessionH == NULL )
  {
    LOG_DEBUG(" Passed  Argument is NULL");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  bootstrapServer = utils_findBootstrapServer(context, fromSessionH);
  if (bootstrapServer != NULL
      && bootstrapServer->status == STATE_BS_PENDING)
  {
    LOG_DEBUG("Bootstrap server status changed to STATE_BS_FINISHED");
    sec_obj = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SECURITY_OBJECT_ID);
    if(sec_obj == NULL)
    {
      LOG_DEBUG("NULL Error for Security Obj");
    }
    bootstrapServer->last_BS_Operation_Time = lwm2m_gettime();
    
    #ifdef PSM_LWM2M
      lwm2m_psm_operation(context, bootstrapServer->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UPDATE, PSM_MODIFY_NODE);
    #endif

    coap_handle = bootstrapServer->coapHandle;
    prv_refreshServerList(context);

    //re-bootstap will be done if there is 1 BS server account and it's updated.
    if(enable_bs_server_replace && bootstrapServer->isBSServerAccountUpdated && (1 == lwm2m_get_server_count(context, TRUE)))
    {
      bootstrapServer = context->bootstrapServer;
      bootstrapServer->isBootstrapped = FALSE;
      bootstrapServer->retryTimeout = 0;
      bootstrapServer->prevAttemptTime = 0;
      trigger_bs_retry(bootstrapServer);
    }
    else
    {
      if(!bootstrap_checkAllConsistence(context, TRUE))
      {
        trigger_factory_reset_util(FALSE, QAPI_NET_LWM2M_BOOTSTRAP_FAILED_E);
        if(enable_bs_server_replace && bootstrapServer->isBSServerAccountUpdated)
          bootstrapServer->isBSServerAccountUpdated = FALSE;

        return LWM2M_406_NOT_ACCEPTABLE;
      }

      if(context->bootstrapServer!= NULL)
      {
        bootstrapServer = context->bootstrapServer;
        bootstrapServer->coapHandle = coap_handle;
        bootstrapServer->status = STATE_BS_FINISHED;
        bootstrapServer->isBootstrapped = TRUE;
        if (persis_regstatus_feat_enabled == LWM2M_TRUE)
          lwm2m_carrier_update_regstatus(bootstrapServer);
        update_isbootstrap_field_for_security_instance(sec_obj, bootstrapServer->secObjInstID);
      }
    }

    target_serlistP  = context->serverList;
    if (target_serlistP != NULL)
    {
      if (persis_regstatus_feat_enabled == LWM2M_TRUE)
      {
        while (target_serlistP != NULL)
        {
          lwm2m_carrier_update_regstatus(target_serlistP);
          target_serlistP = target_serlistP->next;
        }
      }
    }

    qurt_signal_set(&lwm2m_signal_Handle, LWM2M_TRIGGER_DISABLE);

    return  LWM2M_204_CHANGED;
  }

  return LWM2M_IGNORE;
}

/*
 * Reset the bootstrap servers statuses
 *
 * TODO: handle LWM2M Servers the client is registered to ?
 *
 */
/**
 * @fn void bootstrap_start()
 * @brief This function is used to start bootstrap
 *
 * @param contextP  lwm2m_context_t pointer
 *
 * @return      void
 */
void bootstrap_start(lwm2m_context_t * contextP)
{
  lwm2m_server_t * targetP = NULL;

  if(contextP == NULL)
  {
    LOG_DEBUG("Passed  Argument is NULL");
    return;
  }

  LOG_DEBUG("Entering");
  targetP = contextP->bootstrapServer;
  if(targetP != NULL)
  {
    targetP->status = STATE_DEREGISTERED;
    if (targetP->sessionH != 0) {
      LOG_DEBUG("Bootstrap server consists already session");
      qapi_Coap_Close_Connection(targetP->coapHandle);
      targetP->sessionH = 0;
    }
  }
  return;
}

/*
 * Returns STATE_BS_PENDING if at least one bootstrap is still pending
 * Returns STATE_BS_FINISHED if at least one bootstrap succeeded and no bootstrap is pending
 * Returns STATE_BS_FAILED if all bootstrap failed.
 */
/**
 * @fn lwm2m_status_t bootstrap_getStatus()
 * @brief This function is used to get the bootstrap status
 *
 * @param contextP  lwm2m_context_t pointer
 *
 * @return      STATE_BS_PENDING if at least one bootstrap is still pending
 *              STATE_BS_FINISHED if at least one bootstrap succeeded and 
 *              no bootstrap is pending
 *              STATE_BS_FAILED if all bootstrap failed
 */
lwm2m_status_t bootstrap_getStatus(lwm2m_context_t * contextP)
{
  lwm2m_server_t * targetP;
  lwm2m_status_t bs_status;

  LOG_DEBUG("Entering");
  if ( contextP == NULL ) 
  {
    LOG_DEBUG(" Passed  Argument is NULL");
    return STATE_BS_FAILED;
  }
  targetP = contextP->bootstrapServer;
  if(NULL == targetP)
  {
     ds_iot_ulog_high("BS server is NULL, regard it as BS finished");
    return STATE_BS_FINISHED;
  }
  bs_status = STATE_BS_FAILED;

  switch (targetP->status)
  {
    case STATE_BS_FINISHED:
      if (bs_status == STATE_BS_FAILED)
      {
        bs_status = STATE_BS_FINISHED;
      }
      break;

    case STATE_BS_HOLD_OFF:
    case STATE_BS_INITIATED:
    case STATE_BS_PENDING:
    case STATE_BS_REG_RETRYING :
    case STATE_BS_CONN_RETRYING :
    case STATE_DEREGISTERED:
      bs_status = STATE_BS_PENDING;
      break;

    default:
      break;
  }

  if (targetP)
    ds_iot_log_strf_high("BS id %d status: %s",targetP->shortID, STR_STATUS(targetP->status));

  return bs_status;
}

/**
 * @fn static coap_status_t prv_checkServerStatus()
 * @brief This function is used to check the status of server
 *
 * @param serverP   lwm2m_server_t server pointer
 *
 * @return COAP response type
 */
static uint8_t prv_checkServerStatus(lwm2m_server_t * serverP)
{
  if( serverP == NULL ) 
  {
    LOG_DEBUG("prv_checkServerStatus:NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  LOG_DEBUG("Initial status: %s", STR_STATUS(serverP->status));

  switch (serverP->status)
  {
    case STATE_BS_HOLD_OFF:
      serverP->status = STATE_BS_PENDING;
      LOG_DEBUG("Status changed to: %s", STR_STATUS(serverP->status));
      break;

    case STATE_BS_INITIATED:
      // The ACK was probably lost
      serverP->status = STATE_BS_PENDING;
      LOG_DEBUG("Status changed to: %s", STR_STATUS(serverP->status));
      break;

    case STATE_DEREGISTERED:
      // server initiated bootstrap
    case STATE_BS_PENDING:
      // do nothing
      break;

    case STATE_BS_FINISHED:
    case STATE_BS_FAILED:
    default:
      LOG_DEBUG("Returning LWM2M_IGNORE");
      return LWM2M_IGNORE;
  }

  return  LWM2M_NO_ERROR;
}
//TODO: complete brief
/**
 * @fn static void prv_tagServer()
 * @brief This function is used to find server and 
 *
 * @param contextP handle to lwm2m context
 * @param id Server id to be tagged
 *
 * @return void
 */
static void prv_tagServer(lwm2m_context_t * contextP, uint16_t id)
{
  lwm2m_server_t * targetP = NULL;
  if(contextP == NULL) 
  { 
    LOG_DEBUG("prv_tagServer:NULL Error");
    return;
  }

  if(id == 0xFFFF)
  {
    targetP = contextP->serverList;
    while (targetP != NULL)
    {
      targetP->dirty = true;
      targetP = targetP->next;
    }
  }
  else
  {
    targetP = (lwm2m_server_t *)LWM2M_LIST_FIND(contextP->serverList, id);
    if (targetP != NULL)
    {
      ds_iot_ulog_high1("Marking ssid[%d] as invalid",id);
      targetP->dirty = true;
    }
  }
  return;
}

/**
 * @fn coap_status_t bootstrap_handleCommand()
 * @brief This function is used to handle coap commands
 *
 * @param contextP handle to lwm2m context
 * @param uriP handle to the resource
 * @param serverP handle to server 
 * @param message handle to coap packet
 * @param response handle to coap packet
 *
 * @return COAP response type
 */
uint8_t bootstrap_handleCommand(lwm2m_context_t * contextP,
    lwm2m_uri_t * uriP,
    lwm2m_server_t * serverP,
    qapi_Coap_Packet_t * message,
    qapi_Coap_Packet_t * response)
{
  uint8_t result ;
  lwm2m_media_type_t        format;
  lwm2m_object_t          *objectP = NULL;
  security_instance_t   *instanceP = NULL;
  uint16_t                    ssid = 0;

  if( contextP == NULL || uriP == NULL || serverP == NULL
      || message == NULL || response == NULL ) 
  { 
    LOG_DEBUG("bootstrap_handleCommand:NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  LOG_DEBUG("Code: %02X", message->code);
  LOG_URI(uriP);

  format = utils_convertMediaType(message->content_type);

  result = prv_checkServerStatus(serverP);

  if (result != LWM2M_NO_ERROR) return result;

  contextP->activeServerP =  serverP ;

  serverP->last_BS_Operation_Time = lwm2m_gettime();
  if (persis_regstatus_feat_enabled == LWM2M_TRUE) 
   lwm2m_carrier_update_regstatus(serverP);
  switch (message->code)
  {
    case QAPI_COAP_PUT:
      {
        if(format == LWM2M_CONTENT_UNSUPPORTED)
          return LWM2M_415_UNSUPPORTED_CONTENT_FORMAT;
        if (LWM2M_URI_IS_SET_INSTANCE(uriP))
        {
          if (object_isInstanceNew(contextP, uriP->objectId, uriP->instanceId))
          {
            result = object_create(contextP, uriP, format, message->payload, message->payload_len);
            if (LWM2M_201_CREATED == result)
            {
              result = LWM2M_204_CHANGED;
            }
            else if(result != LWM2M_415_UNSUPPORTED_CONTENT_FORMAT &&
                    result != LWM2M_NO_ERROR)
            {
              result = LWM2M_400_BAD_REQUEST;
            }
          }
          else
          {
            result = object_write(contextP, uriP, format, message, message->payload_len, message->code, TRUE);

            if(uriP->objectId == LWM2M_SECURITY_OBJECT_ID &&
               uriP->instanceId == serverP->secObjInstID &&
               result == LWM2M_204_CHANGED &&
               enable_bs_server_replace)
            {
              serverP->isBSServerAccountUpdated = TRUE;
            }
            if (result != LWM2M_204_CHANGED &&
                result != LWM2M_415_UNSUPPORTED_CONTENT_FORMAT &&
                result != LWM2M_NO_ERROR)
            {
              result = LWM2M_400_BAD_REQUEST;
            }
          }
        }
        else
        {
          lwm2m_data_t * dataP = NULL;
          int size = 0;
          int i;

          if (message->payload_len == 0 || message->payload == 0)
          {
            result =  LWM2M_400_BAD_REQUEST;
          }
          else
          {
            size = lwm2m_data_parse(uriP, message->payload, message->payload_len, format, &dataP);
            if ( ( size == 0 ) || ( NULL == dataP ) )
            {
              result = LWM2M_500_INTERNAL_SERVER_ERROR;
              break;
            }

            for (i = 0 ; i < size ; i++)
            {
              if(dataP[i].type == LWM2M_TYPE_OBJECT_INSTANCE)
              {
                result = object_createInstance(contextP, uriP, &dataP[i]);
                if (LWM2M_201_CREATED == result)
                {
                  result =  LWM2M_204_CHANGED;
                }
                else if(LWM2M_NO_ERROR == result)
                {
                  result = LWM2M_NO_ERROR;
                }
                if(result != LWM2M_204_CHANGED && result != LWM2M_NO_ERROR) // Stop object create or write when result is error
                {
                  if(result != LWM2M_415_UNSUPPORTED_CONTENT_FORMAT)
                  {
                    result = LWM2M_400_BAD_REQUEST;
                  }
                  break;
                }
              }
              else
              {
                result =  LWM2M_400_BAD_REQUEST;
              }
            }
            lwm2m_data_free(size, dataP);
          }
        }
      }
      break;

    case QAPI_COAP_DELETE:
      {
        if (LWM2M_URI_IS_SET_RESOURCE(uriP))
        {
          result = (qapi_Coap_Status_t) LWM2M_400_BAD_REQUEST;
        }
        else
        {
          if(uriP->objectId == LWM2M_DEVICE_OBJECT_ID)
          {
            result = LWM2M_400_BAD_REQUEST;
            goto end;
          }

          if(uriP->objectId == LWM2M_SECURITY_OBJECT_ID)
          {
            if(LWM2M_URI_IS_SET_INSTANCE(uriP))
            {
              if(uriP->instanceId == serverP->secObjInstID)
              {
                result  = LWM2M_400_BAD_REQUEST;
                goto end;
              }

              objectP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
              if(NULL == objectP)
              {
                result = LWM2M_400_BAD_REQUEST;
                goto end;
              }

              instanceP = (security_instance_t *)LWM2M_LIST_FIND(objectP->instanceList, uriP->instanceId);
              if(NULL == instanceP)
              {
                result = LWM2M_400_BAD_REQUEST;
                goto end;
              }

              ssid = instanceP->shortID;

              result = object_delete_bootstrap(contextP, uriP);
            }
            else
            {
              for(objectP = contextP->objectList; objectP != NULL; objectP = objectP->next)
              {
                if(objectP->objID == LWM2M_SECURITY_OBJECT_ID)
                {
                  result = LWM2M_202_DELETED;
                  instanceP = (security_instance_t *)objectP->instanceList;
                  while (NULL != instanceP && result == LWM2M_202_DELETED)
                  {
                   if (instanceP->instanceId == serverP->secObjInstID)
                   {
                      instanceP = instanceP->next;
                   }
                   else
                   {
                     uriP->flag |= LWM2M_URI_FLAG_INSTANCE_ID;
                     uriP->instanceId = instanceP->instanceId;
                     result = object_delete_bootstrap(contextP, uriP);
                     instanceP = (security_instance_t *)objectP->instanceList;
                   }
                  }
                  uriP->flag &= ~LWM2M_URI_FLAG_INSTANCE_ID;
                }
              }
            }
          }
          else
          {
           /* Standard objects processing */ 
           if (lwm2m_is_standard_object(uriP->objectId))
           {
             result = object_delete_bootstrap(contextP, uriP);
           }
           else 
           {
             lwm2m_server_data_t lwm2m_info;

             /* Forward "bootstrap delete" request to the extensible OEM application */
             memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t));
             lwm2m_info.msg_type = QAPI_NET_LWM2M_DELETE_REQ_E;

             /* Message ID comprises of Token ID + Short Server ID */
             lwm2m_info.msg_id_len = message->token_len;
             memcpy(lwm2m_info.msg_id, message->token, message->token_len);
             memcpy(&lwm2m_info.msg_id[lwm2m_info.msg_id_len], &serverP->shortID, 0x02);
             lwm2m_info.msg_id_len += 0x02;

             /* LWM2M payload content type */ 
             lwm2m_info.content_type = (qapi_Net_LWM2M_Content_Type_t)message->content_type;
             /* LWM2M payload length */
             lwm2m_info.payload_len = message->payload_len;
             /* LWM2M payload */
             lwm2m_info.payload = message->payload;

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

             result = lwm2m_client_application_notification(&lwm2m_info);
             if (result)
               result = (uint8_t)LWM2M_500_INTERNAL_SERVER_ERROR;
           }
          }
          if ((uriP->objectId == LWM2M_SECURITY_OBJECT_ID )
             && (result == LWM2M_202_DELETED) )
          {
           if (LWM2M_URI_IS_SET_INSTANCE(uriP))
           {
             prv_tagServer(contextP, ssid);
           }
           else
           {
             prv_tagServer(contextP, 0xFFFF);
           }
          }
        }
        prv_refreshServerList(contextP);
      }
      break;

    case QAPI_COAP_GET:
      {
        uint8_t *buffer = NULL;
        size_t length = 0;

        if ((LWM2M_URI_IS_SET_INSTANCE(uriP)) || (LWM2M_URI_IS_SET_RESOURCE(uriP)))
        {
          result = (qapi_Coap_Status_t) LWM2M_400_BAD_REQUEST;
          return result;
        }
        if (QAPI_IS_OPTION(message, QAPI_COAP_OPTION_ACCEPT)
                       && message->accept_num == 1
                       && message->accept[0] == QAPI_APPLICATION_LINK_FORMAT)
        {
          ds_iot_ulog_high("BS Discover for");
          format = (lwm2m_media_type_t) QAPI_APPLICATION_LINK_FORMAT;
          result = object_bs_discover(contextP, uriP->objectId, &buffer, &length);
        }
        else
          result = (uint8_t) LWM2M_400_BAD_REQUEST;

        if (LWM2M_205_CONTENT == result)
        {
          qapi_Coap_Set_Header(serverP->coapHandle,response ,QAPI_COAP_CONTENT_TYPE,&format,0);
          qapi_Coap_Set_Payload(serverP->coapHandle,response, buffer, length);
        }

        if(buffer)
        lwm2m_free(buffer);

        break;
      }
    case QAPI_COAP_POST:
    default:
      result = LWM2M_400_BAD_REQUEST;
      break;
  }
end:
  if (result == LWM2M_202_DELETED
      || result == LWM2M_204_CHANGED)
  {
    #ifdef PSM_LWM2M
      lwm2m_psm_operation(contextP, serverP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UPDATE, PSM_MODIFY_NODE);
    #endif
    if (serverP->status != STATE_BS_PENDING)
    {
      serverP->status = STATE_BS_PENDING;
      contextP->state = STATE_BOOTSTRAPPING;
    }
  }
  LOG_DEBUG("Server status: %s", STR_STATUS(serverP->status));

  return result;
}

/**
 * @fn coap_status_t bootstrap_handleDeleteAll()
 * @brief This function is used to delete objects
 *
 * @param contextP handle to lwm2m context
 * @param fromSessionH handle to session
 *
 * @return COAP response type
 */
uint8_t bootstrap_handleDeleteAll(lwm2m_context_t * contextP,
    void * fromSessionH)
{
  lwm2m_server_t        *serverP = NULL;
  uint8_t                 result;
  lwm2m_object_t        *objectP = NULL;
  lwm2m_server_data_t lwm2m_info;


  LOG_DEBUG("Entering");
  if( contextP == NULL || fromSessionH == NULL )
  {
    LOG_DEBUG(" Passed  Argument is NULL");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  serverP = utils_findBootstrapServer(contextP, fromSessionH);
  if (serverP == NULL) return LWM2M_IGNORE;
  result = prv_checkServerStatus(serverP);
  if (result != LWM2M_NO_ERROR) return result;

  /* Forward "bootstrap delete all" request to the extensible OEM application */
  memset(&lwm2m_info, 0x00, sizeof(lwm2m_server_data_t));
  lwm2m_info.msg_type = QAPI_NET_LWM2M_DELETE_ALL_REQ_E;
  result = lwm2m_client_application_notification(&lwm2m_info);
  if (result)
    return (uint8_t)LWM2M_500_INTERNAL_SERVER_ERROR; 

  contextP->activeServerP = serverP;

  result = LWM2M_202_DELETED;
  serverP->last_BS_Operation_Time = lwm2m_gettime();
  if (persis_regstatus_feat_enabled == LWM2M_TRUE) 
   lwm2m_carrier_update_regstatus(serverP);

  for (objectP = contextP->objectList; objectP != NULL; objectP = objectP->next)
  {
    lwm2m_uri_t uri;
    if (objectP->objID == LWM2M_ACL_OBJECT_ID )
    {
      if(contextP->acObjInstList != NULL)
      {
        contextP->acObjInstList = NULL;
      }
    }

    memset(&uri, 0, sizeof(lwm2m_uri_t));
    uri.flag = LWM2M_URI_FLAG_OBJECT_ID;
    uri.objectId = objectP->objID;

    if (objectP->objID == LWM2M_SECURITY_OBJECT_ID)
    {
      security_instance_t *instanceP;

      instanceP = (security_instance_t *)objectP->instanceList;
      while (NULL != instanceP && result == LWM2M_202_DELETED)
      {
        if (instanceP->instanceId == serverP->secObjInstID)
        {
          instanceP = instanceP->next;
        }
        else
        {
          uri.flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID;
          uri.instanceId = instanceP->instanceId;
          result = object_delete_bootstrap(contextP, &uri);
          instanceP = (security_instance_t *)objectP->instanceList;
        }
      }
      if (result == LWM2M_202_DELETED)
      {
        prv_tagServer(contextP, 0xFFFF);
      }
    }
    else
    {
      if ((objectP->objID == LWM2M_SERVER_OBJECT_ID ))
      {
        server_instance_t* instanceP = NULL;

        instanceP = (server_instance_t *)objectP->instanceList;
        while (NULL != instanceP && result == LWM2M_202_DELETED)
        {
          uri.flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID;
          uri.instanceId = instanceP->instanceId;
          result = object_delete_bootstrap(contextP, &uri);
          instanceP = (server_instance_t *)objectP->instanceList;
        }
      }
      else
      {
        result = object_delete_bootstrap(contextP, &uri);
        if (result == LWM2M_400_BAD_REQUEST)
        {
          // Fake a successful deletion for static objects like the Device object.
          result = LWM2M_202_DELETED;
        }
      }
    }
  }

  #ifdef PSM_LWM2M
  lwm2m_psm_operation(contextP, serverP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UPDATE, PSM_MODIFY_NODE);
  #endif

  prv_refreshServerList(contextP);
  return result;
}

/**
 * @fn bool bootstrap_checkBSserverAccount()
 * @brief This function is used to do BS server account check, make sure there is 
 *        no more than 1 account.
 *
 * @param contextP        pointer to lwm2m_context_t
 * @param isClientInitBS  it's Client Initialized BS or Factory BS.
 *
 * @return      TRUE  success
 *              FALSE failure
 */
bool bootstrap_checkBSserverAccount(lwm2m_context_t * contextP, bool isClientInitBS)
{
  client_data_t          *dataP = NULL;
  lwm2m_server_t      *BSserver = NULL;
  lwm2m_object_t  *securityObjP = NULL;
  char                     *uri = NULL;

  if(contextP == NULL)
  {
    ds_iot_ulog_err(" bootstrap_checkBSserverAccount:  NULL Error");
    return FALSE;
  }

  dataP = (client_data_t *)contextP->userData;
  securityObjP = dataP->securityObjP;
  BSserver = contextP->bootstrapServer;

  if(BSserver != NULL)
  {
    uri = get_server_uri(securityObjP, BSserver->secObjInstID);
    if (uri == NULL)
    {
      ds_iot_ulog_err1("uri of instance /0/%d is NULL", BSserver->secObjInstID);
      return FALSE;
    }
  }

  if(uri != NULL)
  {
    lwm2m_free(uri);
  }
  if(!isClientInitBS && BSserver != NULL)
  {
    ds_iot_ulog_err("it's factory BS but there is BS server account");
    return FALSE;
  }

  return TRUE;
}

/**
 * @fn bool bootstrap_checkAllConsistence()
 * @brief This function is used to do BS consistence check after reveiving BS FINISH
 *        under Client initialized BS or after factory BS.
 *
 * @param contextP          lwm2m_context_t pointer
 * @param isClientInitBS   it's Client Initialized BS(TRUE) or Factory BS(FALSE).
 *
 * @return      TRUE  success
 *              FALSE failure
 */
bool bootstrap_checkAllConsistence(lwm2m_context_t *contextP, bool isClientInitBS)
{
  if (contextP == NULL)
  {
    ds_iot_ulog_err1("Invalid input parameters - contextP[0x%x]", contextP);
    return FALSE;
  }

  if(!object_checkSecurityObjInstConsistence(contextP)         ||
     !object_checkServerObjInstConsistence(contextP)           ||
#ifdef AUTHORIZATION_SUPPORT
     !object_checkACLConsistence(contextP, isClientInitBS)     ||
#endif
     !object_checkServerListConsistence(contextP)              ||
     !bootstrap_checkBSserverAccount(contextP, isClientInitBS) ||
     !object_isSSIDUnique(contextP))
  {
    ds_iot_ulog_err1("Bootstrap consistence check failure - contextP[0x%x]", contextP);
    return FALSE;
  }

  return TRUE;
}
#endif
#endif


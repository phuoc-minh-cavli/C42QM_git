/*!
  @file
  lwm2m_psm.c

  @brief
  LWM2M PSM file to interact with the PSM Daemon.. 
*/

/*===========================================================================

  Copyright (c) 2019-2021 Qualcomm Technologies, Inc. All Rights Reserved

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifdef PSM_LWM2M

#include "liblwm2m.h"
#include "qapi_psm.h"
#include "qapi_psm_types.h"
#include "qapi_psm_status.h"
#include "lwm2m_rx.h"
#include "qapi_coap.h"
#include "time_jul.h"
#include "psm_client.h"
#include "ds_log.h"


/* Global declarations */ 
bool psm_inited = FALSE;             // LwM2M PSM is inited or not.
extern uint32_t active_time;         // LwM2M Active time
extern time_t sleepAckTimeout;       // LwM2M Ack timeout
extern uint32_t trackingSessionId;   // Location tracking session id
extern bool lwm2m_psm_enable;

static bool validate_timeout_value(time_t *interval, time_t value,
                                             time_t delta)
{
  if(interval == NULL)
    return FALSE;

  if(value >= delta)
  {
    *interval = value - delta;
    return TRUE;
  }
  else
  {
    ds_iot_ulog_high("PSM node made inactive due to invalid timeout");
    return FALSE;
  }
}

#define LOCATION_TIMEOUT_FETCH  65 
static bool rebootstrap_psm = LWM2M_FALSE;

/* LwM2M PSM Function definition */

/* LwM2M PSM registered callback */
void lwm2m_psm_cb(psm_status_msg_type *psm_status)
{
  lwm2m_pkt_t *pkt = NULL;

  pkt = lwm2m_get_pkt_buffer();

  if(pkt == NULL)
  {
    LOG_DEBUG("%s",mallocError);
    return;
  }

  memset(pkt, 0x00, sizeof(lwm2m_pkt_t));

  pkt->cmd_hdr.cmd_id = LWM2M_PSM_CB;

  pkt->cmd_data.clientData = (void *)psm_status;

  lwm2m_enqueue_pkt(pkt);

  lwm2m_set_signal();

  return;
}

/**
 * @fn void lwm2m_psm_event_handle()
 *
 * @brief This function is used to handle different events recieved from PSM daemon.
 *
 * @param psm_status  PSM status message contains status and reject reason (if any)
 *
 * @return None
 */
void lwm2m_psm_event_handle(psm_status_msg_type *psm_status)
{
  if(data.lwm2mH && data.lwm2mH->lwm2m_psm_info)
  {
    if(psm_status)
    {
      if(psm_status->status != PSM_STATUS_HEALTH_CHECK &&
         psm_status->status != PSM_STATUS_NONE)
      {
        ds_iot_ulog_high1(" PSM STATUS:    %d", psm_status->status);
        ds_iot_ulog_high1(" REJECT REASON: %d", psm_status->reason);
      }

      switch(psm_status->status)
      {
        // special handling for some cases like modem loaded, not loaded etc.
        case PSM_STATUS_MODEM_LOADED:
        {
          data.lwm2mH->lwm2m_psm_info->is_modem_loaded = TRUE;
        }
        break;
  
        case PSM_STATUS_MODEM_NOT_LOADED:
        {
          data.lwm2mH->lwm2m_psm_info->is_modem_loaded = FALSE;
        }
        break;
  
        case PSM_STATUS_REJECT:
        {
          data.lwm2mH->lwm2m_psm_info->psm_msg.status = psm_status->status;
          data.lwm2mH->lwm2m_psm_info->psm_msg.reason = psm_status->reason;
          switch(psm_status->reason)
          {
            case PSM_REJECT_REASON_NOT_ENABLED:
            {
              data.lwm2mH->lwm2m_psm_info->psm_enabled = FALSE;
            }
            break;

            case PSM_REJECT_REASON_MODEM_NOT_READY:
            break;
  
            case PSM_REJECT_REASON_DURATION_TOO_SHORT:
            case PSM_REJECT_REASON_INCORRECT_USAGE:
            {
              /* Cancel vote for PSM and state is moved back to computation 
                 required state */
              lwm2m_cancel_vote_for_psm(data.lwm2mH);
            }
            break;
          }
        }
        break;
  
        case PSM_STATUS_READY:
        case PSM_STATUS_NOT_READY:
        case PSM_STATUS_COMPLETE:
        case PSM_STATUS_DISCONNECTED:
        break;
  
        case PSM_STATUS_FEATURE_ENABLED:
        {
          data.lwm2mH->lwm2m_psm_info->psm_enabled = TRUE;
        }
        break;
  
        case PSM_STATUS_FEATURE_DISABLED:
        {
          data.lwm2mH->lwm2m_psm_info->psm_enabled = FALSE;
        }
        break;
  
        case PSM_STATUS_HEALTH_CHECK:
        {
          qapi_PSM_Client_Hc_Ack(data.lwm2mH->lwm2m_psm_info->client_id);
        }
        break;
  
        case PSM_STATUS_NW_OOS:
        case PSM_STATUS_NW_LIMITED_SERVICE:
        {
          qapi_PSM_Client_Enter_Backoff(data.lwm2mH->lwm2m_psm_info->client_id);
        }
        break;
        default:
        break;
      }
    }
  }
}

/**
 * @fn lwm2m_psm_scheduler_t * lwm2m_psm_find_node()
 *
 * @brief This function is used to find PSM node based on server short server id,
 *        uri ( for watcher node ).and type of LwM2M PSM node.
 *
 * @param contextP  LwM2M client context
 * @param ssid      Short server id
 * @param uri       Uri of observed watcher
 * @param node_type Type of LwM2M PSM node
 *
 * @return Node if found or NULL is returned
 */
lwm2m_psm_scheduler_t * lwm2m_psm_find_node(lwm2m_context_t * contextP,
                                            uint16_t ssid,
                                            lwm2m_uri_t uri,
                                            lwm2m_psm_node_type_e node_type)
{
  lwm2m_psm_scheduler_t *node_list = NULL;

  if((contextP == NULL) || (contextP->lwm2m_psm_info == NULL))
  {
    LOG_DEBUG("LWM2M_LOG: NULL Error");
    return NULL;
  }

  qurt_mutex_lock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);

  node_list = contextP->lwm2m_psm_info->psm_schedule_list;

  switch(node_type)
  {
    case PSM_NODE_SERVER:
    {
      while(node_list != NULL)
      {
        if(node_list->ssid == ssid)
        {
          break;
        }
        node_list = node_list->next;
      }
    }
    break;

    case PSM_NODE_OBSERVATION:
    {
      while(node_list != NULL
            && (node_list->uriP.objectId != uri.objectId
            ||  node_list->uriP.flag != uri.flag
            || (LWM2M_URI_IS_SET_INSTANCE(&uri) && node_list->uriP.instanceId != uri.instanceId)
            || (LWM2M_URI_IS_SET_RESOURCE(&uri) && node_list->uriP.resourceId != uri.resourceId)
            || (node_list->ssid != ssid)))
      {
        node_list = node_list->next;
      }
    }
    break;

    case PSM_NODE_REBOOTSTRAP:
    {
      while(node_list != NULL)
      {
        if(node_list->node_type == PSM_NODE_REBOOTSTRAP)
        {
          break;
        }
        node_list = node_list->next;
      }
      break;
    }

    default:
    {
      node_list = NULL;
    }
    break;
  }

  if(node_list)
  {
    ds_iot_ulog_high1("PSM Found node with ssid %d",ssid);
    if(node_type == PSM_NODE_OBSERVATION)
    LOG_URI(&uri);
  }

  qurt_mutex_unlock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);

  return node_list; 
}

/**
 * @fn void lwm2m_psm_fill_psm_info()
 *
 * @brief This function is used to fill different fields realted to LwM2M PSM node
 *
 * @param contextP  LwM2M client context
 * @param ssid      Short server id
 * @param uri       Uri of observed watcher
 * @param node_type Type of LwM2M PSM node
 * @param node      LwM2M PSM node which needs to be updated.
 *
 * @return None
 */
void lwm2m_psm_fill_psm_info(lwm2m_context_t *contextP,
                             uint16_t ssid,
                             lwm2m_uri_t uri,
                             lwm2m_psm_node_type_e node_type,
                             lwm2m_psm_scheduler_t *node)
{
  lwm2m_server_t *targetP = NULL;
  lwm2m_active_server_t *serverP = NULL;
  time_t currentTime = lwm2m_gettime();

  if(contextP == NULL || node == NULL || contextP->lwm2m_psm_info == NULL)
  {
    LOG_DEBUG("%s",nullError);
    return;
  }

  ds_iot_ulog_high2("Node ssid %d node type %d ", ssid, node_type);
  LOG_DEBUG("node %p.", node);
  ds_iot_ulog_high3("curr time %lu active time %lu timeout %lu.", currentTime, active_time, sleepAckTimeout);
  switch(node_type)
  {
    case PSM_NODE_REBOOTSTRAP:
    {
   /* When LwM2M client triggers rebootstraping vote for PSM upto 
      time_for_bs duration. */
      if(contextP->lwm2m_psm_info->psm_voted == FALSE 
         && contextP->time_for_bs > currentTime)
      {
        node->node_start_interval = contextP->time_for_bs;
        node->node_end_interval   = contextP->time_for_bs;
        node->node_wakeup_type    = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
        node->node_active_time    = 0;
        node->node_active         = TRUE;
        rebootstrap_psm           = TRUE;
      }
      else
      {
        node->node_active         = FALSE;
        rebootstrap_psm           = FALSE;
      }
    }
    break;

    case PSM_NODE_SERVER:
    {
      /* For Bootstrap server fill information */
      targetP = contextP->bootstrapServer;
      if(targetP)
      {
        if(targetP->shortID == ssid)
        {
          switch(targetP->status)
          {
            case STATE_DEREGISTERED:
            case STATE_BS_HOLD_OFF:
            case STATE_BS_INITIATED:
            case STATE_BS_PENDING:
             node->node_active         = FALSE;
            break;

            case STATE_BS_REG_RETRYING:
            {
              if(contextP->lwm2m_psm_info->psm_voted == FALSE)
              {
                node->node_start_interval = targetP->retryTimeout;
                node->node_end_interval   = targetP->retryTimeout;
                node->node_wakeup_type    = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
                node->node_active_time    = targetP->prevAttemptTime;
                node->node_active         = TRUE;
              }
              else
              {
                node->node_active         = FALSE;
              }
            }
            break;

            case STATE_BS_FAILED:
            {
              if(contextP->lwm2m_psm_info->psm_voted == FALSE)
              {
                node->node_start_interval = targetP->next_cycle_for_registration;
                node->node_end_interval   = targetP->next_cycle_for_registration;
                node->node_wakeup_type    = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
                node->node_active_time    = 0 ;
                node->node_active         = TRUE;
              }
              else
              {
                node->node_active         = FALSE;
              }
            }
            break;
          }
        }
        else
          targetP = NULL;
      }

      if(targetP == NULL)
      {
        /* For DM server fill information Loop over only active servers.*/
        serverP = contextP->activeServerList;
        
        while(serverP)
        {
          /* Fetch the server form information from LwM2M context server list*/
          targetP = lwm2m_get_server(serverP->ssid);
          if(targetP == NULL)
          {
            serverP = serverP->next;
            continue;
          }
          
          if(targetP->shortID == ssid)
          {
            switch(targetP->status)
            {
              case STATE_DEREGISTERED:
              {
                /* If Server is in Disabled state */
                if(LWM2M_SERVER_IS_DISABLE_SET(targetP->update_disable_check))
                {
                  if(contextP->lwm2m_psm_info->psm_voted == FALSE)
                  {
                    uint32_t disable_timeout = 0;
                    disable_timeout = (0 == targetP->disable_timeOut) ? LWM2M_SERVER_DEFAULT_DISABLE_TIMEOUT : targetP->disable_timeOut;
                    node->node_start_interval = disable_timeout;
                    node->node_end_interval   = disable_timeout;
                    node->node_wakeup_type    = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
                    node->node_active_time    = targetP->disabled;
                    node->node_active         = TRUE;
                  }
                  else
                  {
                    node->node_active         = FALSE;
                  }
                }
                else if(targetP->initial_holdoff_time >= currentTime)
                {
                  if(contextP->lwm2m_psm_info->psm_voted == FALSE)
                  {
                    node->node_start_interval = targetP->initial_holdoff_time;
                    node->node_end_interval   = targetP->initial_holdoff_time;
                    node->node_wakeup_type    = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
                    node->node_active_time    = 0;
                    node->node_active         = TRUE;
                  }
                  else
                  {
                    node->node_active         = FALSE;
                  }
                }
              }
              break;

              case STATE_REG_FAILED:
              {
                if(contextP->lwm2m_psm_info->psm_voted == FALSE)
                {
                  node->node_start_interval = targetP->next_cycle_for_registration;
                  node->node_end_interval   = targetP->next_cycle_for_registration;
                  node->node_wakeup_type    = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
                  node->node_active_time    = 0 ;
                  node->node_active         = TRUE;
                }
                else
                {
                  node->node_active         = FALSE;
                }
              }
              break;

              case STATE_REG_RETRYING:
              {
                if(contextP->lwm2m_psm_info->psm_voted == FALSE)
                {
                  node->node_start_interval = targetP->retryTimeout;
                  node->node_end_interval   = targetP->retryTimeout;
                  node->node_wakeup_type    = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
                  node->node_active_time    = targetP->prevAttemptTime;
                  node->node_active         = TRUE;
                }
                else
                {
                  node->node_active         = FALSE;
                }
              }
              break;

              case STATE_REGISTERED:
              {
                /* Wait for Ack timeout */
                if((currentTime - active_time) >= sleepAckTimeout)
                {
                  if(contextP->lwm2m_psm_info->psm_voted == FALSE)
                  {
                    node->node_delta          = (int)(((float)contextP->lwm2m_psm_info->lifetime_delta_percentage
                                                      /100) * targetP->lifetime);
                    node->node_active = validate_timeout_value(&node->node_start_interval,
                                                               targetP->lifetime, node->node_delta);
                    if(node->node_active == FALSE)
                    {
                      break;
                    }
                    node->node_active = validate_timeout_value(&node->node_end_interval,
                                                               targetP->lifetime, contextP->lwm2m_psm_info->transport_guard);
                    node->node_wakeup_type    = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
                    node->node_active_time    = targetP->registration;
                  }
                  else
                  {
                    node->node_active         = FALSE;
                  }
                }
                else
                {
                  node->node_active         = FALSE;
                }
              }
              break;
            }
            break;
          }
          serverP = serverP->next;
        }
      }
      
      if(targetP == NULL)
      {
        ds_iot_ulog_err("No node found for passed argument.");
        return;
      }
    }
    break;

    case PSM_NODE_OBSERVATION:
    {
      lwm2m_watcher_t *watcher = NULL;
      time_t max_period = 0, min_period = 0;
      targetP = contextP->serverList;
      while(targetP)
      {
        if(targetP->shortID == ssid)
        {
          break;
        }
        targetP = targetP->next;
      }

      if(targetP == NULL)
      {
        ds_iot_ulog_err("No node found for passed argument.");
        return;
      }

      watcher = prv_getWatcher(contextP, &uri, targetP);

      if(watcher == NULL)
      {
        ds_iot_ulog_err("Watcher not found.");
        return;
      }

      min_period = get_pmin_value(contextP, watcher, uri);
      max_period = get_pmax_value(contextP, watcher, uri);

      if(max_period < min_period)
      {
        max_period  = 0;
      }

      /* For location object handle PSM seperately. */
      if(uri.objectId == LWM2M_LOCATION_OBJECT_ID)
      {
        if(trackingSessionId)
        {
          node->node_active         = FALSE;
        }
      }
      /* For object, object instance level notification */
      else if(!LWM2M_URI_IS_SET_RESOURCE(&uri))
      {
        /* Wait for Ack timeout and check whether max period is valid or not.*/
        if(watcher->active && max_period > 0 && ((currentTime - active_time) >= sleepAckTimeout) )
        {
          if(((uri.objectId == LWM2M_LOCATION_OBJECT_ID && max_period > LOCATION_TIMEOUT_FETCH)
               || (uri.objectId != LWM2M_LOCATION_OBJECT_ID)) && (contextP->lwm2m_psm_info->psm_voted == FALSE) )
          {
            node->node_delta          = (int)(((float)contextP->lwm2m_psm_info->pmax_delta_percentage / 100)
                                                    * max_period);
            if(uri.objectId == LWM2M_LOCATION_OBJECT_ID)
            {
              node->node_active = validate_timeout_value(&node->node_start_interval,
                                                           max_period, (node->node_delta + LOCATION_TIMEOUT_FETCH));
              if(node->node_active == FALSE)
              {
                break;
              }
              node->node_active = validate_timeout_value(&node->node_end_interval,
                                                           max_period, LOCATION_TIMEOUT_FETCH);
            }
            else
            {
              node->node_active = validate_timeout_value(&node->node_start_interval,
                                    max_period, node->node_delta);
              node->node_end_interval   = max_period;
            }
            node->node_wakeup_type    = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
            node->node_active_time    = watcher->lastTime;
          }
          else
          {
            node->node_active         = FALSE;
          }
        }
        else
        {
          node->node_active         = FALSE;
        }
      }
      /* For resource level notification */
      else
      {
        /* Wait for Ack timeout and check whether max period is valid or not.*/
        if(watcher->active && ((currentTime - active_time) >= sleepAckTimeout))
        {
          /* If value already changed before Pmin expiry wakeup for NW access
             with delta = 0, else wakeup for measurement only */ 
          if(watcher->update)
          {
            node->node_wakeup_type    = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
            node->node_delta          = 0;
          }
          else
          {
            node->node_wakeup_type    = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
            node->node_delta          = (int)(((float)(((float)contextP->lwm2m_psm_info->pmin_delta_percentage/100)))
                                               * min_period);
          }
          if(contextP->lwm2m_psm_info->psm_voted == FALSE)
          {
            if(contextP->lwm2m_psm_info->pmin_pmax_awake == FALSE || watcher->pmin_wait_count == 0)
            {
              if(max_period > 0 && (((watcher->pmin_wait_count + 1) * min_period) < max_period))
              {
                if(uri.objectId == LWM2M_LOCATION_OBJECT_ID)
                {
                  node->node_active = validate_timeout_value(&node->node_start_interval,
                                                               (watcher->pmin_wait_count + 1) * min_period,
                                                                LOCATION_TIMEOUT_FETCH);
                  if(node->node_active == FALSE)
                  {
                    break;
                  }
                  node->node_active = validate_timeout_value(&node->node_end_interval,
                                                             (watcher->pmin_wait_count + 1) * min_period + node->node_delta,
                                                              LOCATION_TIMEOUT_FETCH);
                }
                else
                {
                  node->node_start_interval = (watcher->pmin_wait_count + 1) * min_period ;
                  node->node_end_interval   = (watcher->pmin_wait_count + 1) * min_period + node->node_delta;
                  node->node_active         = TRUE;
                }
                node->node_active_time    = watcher->lastTime;
              }
              else
              {
                node->node_delta          = (int)(((float)contextP->lwm2m_psm_info->pmax_delta_percentage / 100)
                                                    * max_period);
                if(uri.objectId == LWM2M_LOCATION_OBJECT_ID)
                {
                  node->node_active = validate_timeout_value(&node->node_start_interval,
                                          max_period, node->node_delta + LOCATION_TIMEOUT_FETCH);
                  if(node->node_active == FALSE)
                  {
                    break;
                  }
                  node->node_active = validate_timeout_value(&node->node_end_interval,
                                          max_period ,LOCATION_TIMEOUT_FETCH);
                }
                else
                {
                  node->node_active = validate_timeout_value(&node->node_start_interval,
                                          max_period, node->node_delta);
                  node->node_end_interval   = max_period;
                }
                node->node_wakeup_type    = PSM_WAKEUP_MEASUREMENT_NW_ACCESS;
                node->node_active_time    = watcher->lastTime;
              }
            }
            else
            {
              node->node_active         = FALSE;
            }
          }
          else
          {
            node->node_active         = FALSE;
          }
        }
        else
        {
          node->node_active         = FALSE;
        }
      }
    }
    break;
  }
}

/**
 * @fn qapi_Status_t lwm2m_psm_add_node()
 *
 * @brief This function is used to add PSM node based on server short server id,
 *        uri ( for watcher node ).and type of LwM2M PSM node in global LwM2M PSM 
 *        node list.
 *
 * @param contextP  LwM2M client context
 * @param ssid      Short server id
 * @param uri       Uri of observed watcher
 * @param node_type Type of LwM2M PSM node
 *
 * @return See Section @xref{hdr:QAPIStatust}. \n
 * On success, QAPI_OK (0) is returned. Other value on error.
 */
qapi_Status_t lwm2m_psm_add_node(lwm2m_context_t *contextP, 
                                 uint16_t ssid,
                                 lwm2m_uri_t uri,
                                 lwm2m_psm_node_type_e node_type)
{
  lwm2m_psm_scheduler_t *node_list = NULL;
  lwm2m_psm_scheduler_t *node = NULL;
  qapi_Status_t result = QAPI_ERROR;

  if((contextP == NULL) || (contextP->lwm2m_psm_info == NULL))
  {
    return QAPI_ERR_INVALID_PARAM;
  }

  node_list = lwm2m_psm_find_node(contextP, ssid, uri, node_type);

  if(node_list == NULL && ((node_type == PSM_NODE_SERVER) || (node_type == PSM_NODE_OBSERVATION)
      || (node_type == PSM_NODE_REBOOTSTRAP)))
  {
    node_list = (lwm2m_psm_scheduler_t *)lwm2m_malloc(sizeof(lwm2m_psm_scheduler_t));
    if(node_list == NULL)
    {
      return QAPI_ERR_NO_MEMORY;
    }
    memset(node_list, 0x00, sizeof(lwm2m_psm_scheduler_t));

    qurt_mutex_lock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);

    if(NULL == contextP->lwm2m_psm_info->psm_schedule_list)
    {
      node_list->next = contextP->lwm2m_psm_info->psm_schedule_list;
      contextP->lwm2m_psm_info->psm_schedule_list = node_list;
    }
    else
    {
      for(node = contextP->lwm2m_psm_info->psm_schedule_list;
         (node != NULL && node->next != NULL) ; node = node->next);
      node->next = node_list;
    }

    qurt_mutex_unlock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);

    node_list->node_active = FALSE;
    node_list->ssid = ssid;
    memcpy((void *)&(node_list->uriP), (void *)&uri, sizeof(lwm2m_uri_t));
    node_list->node_type = node_type;

    lwm2m_psm_fill_psm_info(contextP, ssid, uri, node_type, node_list);
    node_list->node_mask |= PSM_NODE_ADDED;
    contextP->lwm2m_psm_info->psm_node_count++;
    result = QAPI_OK;
  }
  else
  {
    result = lwm2m_psm_modify_node(contextP, ssid, uri, node_type, PSM_NODE_UPDATE);
  }

  return result;
}

/**
 * @fn qapi_Status_t lwm2m_psm_remove_node()
 *
 * @brief This function is used to remove PSM node based on server short server id,
 *        uri ( for watcher node ).and type of LwM2M PSM node in global LwM2M PSM 
 *        node list.
 *
 * @param contextP  LwM2M client context
 * @param ssid      Short server id
 * @param uri       Uri of observed watcher
 * @param node_type Type of LwM2M PSM node
 *
 * @return See Section @xref{hdr:QAPIStatust}. \n
 * On success, QAPI_OK (0) is returned. Other value on error.
 */
qapi_Status_t lwm2m_psm_remove_node(lwm2m_context_t *contextP, 
                                    uint16_t ssid,
                                    lwm2m_uri_t uri,
                                    lwm2m_psm_node_type_e node_type)
{
  lwm2m_psm_scheduler_t *node_list = NULL;
  qapi_Status_t result = QAPI_ERROR;

  if((contextP == NULL) || (contextP->lwm2m_psm_info == NULL))
  {
    return QAPI_ERR_INVALID_PARAM;
  }

  node_list = lwm2m_psm_find_node(contextP, ssid, uri, node_type);

  if(node_list == NULL)
  {
    result = QAPI_ERR_NO_ENTRY;
  }
  else
  {
    lwm2m_unlink_node((void**)&(contextP->lwm2m_psm_info->psm_schedule_list), (void *)node_list);
    lwm2m_free(node_list);
    node_list = NULL;
    qurt_mutex_lock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);
    contextP->lwm2m_psm_info->psm_node_count--;
    contextP->lwm2m_psm_info->compute_state = PSM_COMPUTATION_REQUIRED;
    ds_iot_ulog_high1(" PSM total active count %d ",contextP->lwm2m_psm_info->psm_node_count);
    qurt_mutex_unlock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);
    result = QAPI_OK;
  }

  return result;
}

/**
 * @fn qapi_Status_t lwm2m_psm_modify_node()
 *
 * @brief This function is used to modify PSM node based on server short server id,
 *        uri ( for watcher node ).and type of LwM2M PSM node in global LwM2M PSM 
 *        node list.
 *        If all_nodes is set to TRUE then all nodes in LwM2M PSM node list need 
 *        to be updated.
 *
 * @param contextP  LwM2M client context
 * @param ssid      Short server id
 * @param uri       Uri of observed watcher
 * @param node_type Type of LwM2M PSM node
 * @param update_type No update, single node update or all node update .
 *
 * @return See Section @xref{hdr:QAPIStatust}. \n
 * On success, QAPI_OK (0) is returned. Other value on error.
 */
qapi_Status_t lwm2m_psm_modify_node(lwm2m_context_t *contextP, 
                                    uint16_t ssid,
                                    lwm2m_uri_t uri,
                                    lwm2m_psm_node_type_e node_type,
                                    lwm2m_psm_modify_node_type_e update_type)
{
  lwm2m_psm_scheduler_t *node_list = NULL;
  qapi_Status_t result = QAPI_ERROR;

  if((contextP == NULL) || (contextP->lwm2m_psm_info == NULL))
  {
    return QAPI_ERR_INVALID_PARAM;
  }

  if(update_type == PSM_NODE_UPDATE_ALL)
  {
    lwm2m_cancel_vote_for_psm(contextP);

    qurt_mutex_lock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);
    node_list = contextP->lwm2m_psm_info->psm_schedule_list;
    while(node_list)
    {
      node_list->node_mask |= PSM_NODE_MODIFIED;
      lwm2m_psm_fill_psm_info(contextP, node_list->ssid, node_list->uriP, node_list->node_type, node_list);
      node_list = node_list->next;
      result = QAPI_OK;
    }

    qurt_mutex_unlock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);
  }
  else
  {
    node_list = lwm2m_psm_find_node(contextP, ssid, uri, node_type);
  
    if(node_list == NULL)
    {
      result = QAPI_ERR_NO_ENTRY;
    }
    else
    {
      if(update_type == PSM_NODE_UPDATE)
      {
        lwm2m_cancel_vote_for_psm(contextP);
        node_list->node_mask |= PSM_NODE_MODIFIED;
      }

      lwm2m_psm_fill_psm_info(contextP, ssid, uri, node_type, node_list);
      result = QAPI_OK;
    }
  }
  return result;
}

/**
 * @fn qapi_Status_t lwm2m_psm_operation()
 *
 * @brief This function is used as a wrapper to call lwm2m_psm_add_node, lwm2m_psm_remove_node
 *        or lwm2m_psm_modify_node on the bases of lwm2m_psm_operation_e.
 *
 * @param contextP  LwM2M client context
 * @param ssid      Short server id
 * @param uri       Uri of observed watcher
 * @param node_type Type of LwM2M PSM node
 * @param update_type No update, single node update or all node update in case of Modify, ignored otherwise .
 * @param psm_operation Type of PSM operation to be performed(Add, Modify, Remove)
 *
 * @return See Section @xref{hdr:QAPIStatust}. \n
 * On success, QAPI_OK (0) is returned. Other value on error.
 */

qapi_Status_t lwm2m_psm_operation(lwm2m_context_t *contextP, 
                                    uint16_t ssid,
                                    lwm2m_uri_t *uri,
                                    lwm2m_psm_node_type_e node_type,
                                    lwm2m_psm_modify_node_type_e update_type,
                                    lwm2m_psm_operation_e psm_operation
                                    )
{
  qapi_Status_t result = QAPI_OK;
  lwm2m_uri_t uriP;

  if(lwm2m_psm_enable == FALSE)
    return QAPI_ERR_NOT_INITIALIZED;

  if(uri == NULL) 
  {
    memset(&uriP, 0x00, sizeof(lwm2m_uri_t));
    uri = &uriP;
  }

  switch(psm_operation){
  case PSM_ADD_NODE:
    result = lwm2m_psm_add_node(contextP, ssid, *uri, node_type);
  break;
  
  case PSM_MODIFY_NODE:
    result = lwm2m_psm_modify_node(contextP, ssid, *uri, node_type, update_type);
  break;
  
  case PSM_REMOVE_NODE:
    result = lwm2m_psm_remove_node(contextP, ssid, *uri, node_type); 
  break;
  
  default:
    result = QAPI_ERR_INVALID_PARAM;
   break;
  }

  ds_iot_ulog_high1("PSM result  %d", result);
  return result;
}


/**
 * @fn qapi_Status_t lwm2m_psm_init()
 *
 * @brief This function is used to allocate LwM2M PSM context and initialize the
 *        LwM2M PSM related information Configuration items are poulated.
 *
 * @param lwm2mP  LwM2M client context
 *
 * @return See Section @xref{hdr:QAPIStatust}. \n
 * On success, QAPI_OK (0) is returned. Other value on error.
 */
qapi_Status_t lwm2m_psm_init(lwm2m_context_t *lwm2mP)
{
  lwm2m_psm_context_t *psm_info = NULL;

  if(lwm2mP == NULL)
  {
    LOG_DEBUG("LWM2M_LOG: NULL Error");
    return QAPI_ERR_INVALID_PARAM;
  }

  if(psm_inited == FALSE)
  {
    psm_info = (lwm2m_psm_context_t *)lwm2m_malloc(sizeof(lwm2m_psm_context_t));
    if(psm_info == NULL)
    {
      return QAPI_ERR_NO_MEMORY;
    }
    memset(psm_info, 0x00, sizeof(lwm2m_psm_context_t));

    /* Initialize the PSM mutex */
    qurt_mutex_init(&psm_info->lwm2m_psm_mutex); 

    psm_info->lifetime_delta_percentage = data.cfg_info.lifetime_percentage;
    psm_info->pmin_delta_percentage = data.cfg_info.pmin_percentage;
    psm_info->pmax_delta_percentage = data.cfg_info.pmax_percentage;
    psm_info->transport_guard       = data.cfg_info.transport_guard;
    psm_info->default_psm_delta     = data.cfg_info.default_psm_delta;
    psm_info->psm_enabled = TRUE;
    psm_info->pmin_pmax_awake = data.cfg_info.pmin_pmax_awake;

    lwm2mP->lwm2m_psm_info = psm_info;
    psm_inited = TRUE;
  }
  return QAPI_OK;
}

/**
 * @fn qapi_Status_t lwm2m_psm_deinit()
 *
 * @brief This function is used to de-allocate LwM2M PSM context and free 
 *        LwM2M PSM realted information .
 *
 * @param lwm2mP  LwM2M client context
 *
 * @return See Section @xref{hdr:QAPIStatust}. \n
 * On success, QAPI_OK (0) is returned. Other value on error.
 */
qapi_Status_t lwm2m_psm_deinit(lwm2m_context_t *lwm2mP)
{
  lwm2m_psm_context_t *psm_info = NULL;

  if(lwm2mP == NULL)
  {
    LOG_DEBUG("Null argument passed ");
    return QAPI_ERR_INVALID_PARAM;
  }

  if(psm_inited == TRUE)
  {
    psm_info = lwm2mP->lwm2m_psm_info;
    if(psm_info)
    {
      qurt_mutex_destroy(&psm_info->lwm2m_psm_mutex);

      LWM2M_LIST_FREE(psm_info->psm_schedule_list);
      lwm2m_free(psm_info);
      lwm2mP->lwm2m_psm_info = NULL;
    }

    psm_inited = FALSE;
  }
  return QAPI_OK;
}

/**
 * @fn qapi_Status_t lwm2m_cancel_vote_for_psm()
 *
 * @brief This function is used to cancel PSM vote by LwM2M client.
 *
 * @param lwm2mP  LwM2M client context
 *
 * @return See Section @xref{hdr:QAPIStatust}. \n
 * On success, QAPI_OK (0) is returned. Other value on error.
 */
qapi_Status_t lwm2m_cancel_vote_for_psm(lwm2m_context_t *contextP)
{
  lwm2m_psm_context_t *psm_info = NULL;
  qapi_Status_t result = QAPI_ERROR;

  if(contextP == NULL || contextP->lwm2m_psm_info == NULL)
  {
    LOG_DEBUG("PSM context is not defined.");
    return QAPI_ERR_INVALID_PARAM;
  }

  psm_info = contextP->lwm2m_psm_info;

  /* PSM vote will be cancelled and LwM2M PSM compute state would be moved back 
     to compuation required state.and previous vote would be nullified.*/
  if(psm_info->psm_voted == FALSE)
  {
    ds_iot_ulog_high("LWM2M client didn't vote for PSM.");
    return QAPI_OK;
  }

  else
  {
    result = qapi_PSM_Client_Cancel_Psm(psm_info->client_id);
    if(result == QAPI_OK)
    { 
      ds_iot_ulog_high("LWM2M vote for PSM cancelled successfully.");
      psm_info->psm_voted = FALSE;
      psm_info->last_psm_vote_time = 0;
      psm_info->compute_state = PSM_COMPUTATION_REQUIRED;
      return result;
    }
    else
    {
      ds_iot_ulog_err1("LWM2M PSM cancel vote returned with error %d .", result);
      return result;
    }
  }

}

/**
 * @fn qapi_Status_t lwm2m_vote_for_psm()
 *
 * @brief This function is used to vote for PSM by LwM2M client.
 *
 * @param lwm2mP  LwM2M client context
 *
 * @return See Section @xref{hdr:QAPIStatust}. \n
 * On success, QAPI_OK (0) is returned. Other value on error.
 */
qapi_Status_t lwm2m_vote_for_psm(lwm2m_context_t *contextP)
{
  lwm2m_psm_context_t *psm_info = NULL;
  psm_info_interval_type *interval = NULL;
  lwm2m_psm_scheduler_t *node_list = NULL;
  qapi_Status_t result = QAPI_ERROR;

  if(contextP == NULL || contextP->lwm2m_psm_info == NULL)
  {
    LOG_DEBUG(" PSM context is not defined.");
    return QAPI_ERR_INVALID_PARAM;
  }

  psm_info = contextP->lwm2m_psm_info;

  interval = (psm_info_interval_type *)lwm2m_malloc(sizeof(psm_info_interval_type));
  if(interval == NULL)
  {
    return QAPI_ERR_NO_MEMORY;
  }
  memset(interval, 0x00, sizeof(psm_info_interval_type));

  /* Cancel the old PSM vote and vote again with new calculated time period.*/
  if(psm_info->psm_voted == TRUE)
  {
    result = lwm2m_cancel_vote_for_psm(contextP);
    if(result != QAPI_OK)
    {
      ds_iot_ulog_err("LWM2M cancel vote failed.");
      goto end;
    }
  }

  ds_iot_ulog_high1("PSM total active count %d ",psm_info->psm_node_count);

  /* Vote for PSM with calculated time period and type of wakeup.
     Clear mask of all the LwM2M PSM nodes after succesful vote. */
  if(psm_info->psm_voted == FALSE)
  {
    interval->psm_time_info.start.time_format_flag = PSM_TIME_IN_SECS;
    interval->psm_time_info.start.psm_duration_in_secs= psm_info->lwm2m_wakeup_start_time;
    interval->psm_time_info.end.time_format_flag   = PSM_TIME_IN_SECS;
    interval->psm_time_info.end.psm_duration_in_secs= psm_info->lwm2m_wakeup_end_time;
    interval->psm_time_info.psm_wakeup_type        = psm_info->lwm2m_wakeup_type;

    result = qapi_PSM_Client_Enter_Psm_With_Range(psm_info->client_id, interval);
    if(result == QAPI_OK)
    {
      ds_iot_ulog_high("LWM2M voted for PSM successfully.");
      psm_info->psm_voted = TRUE;
      psm_info->last_psm_vote_time = lwm2m_gettime();

      node_list = psm_info->psm_schedule_list;
      while(node_list)
      {
        if(node_list->node_mask)
        {
          node_list->node_mask = 0;
          node_list->node_active = FALSE;
        }
        node_list = node_list->next;
      }
      qurt_mutex_lock(&psm_info->lwm2m_psm_mutex);
      psm_info->compute_state = PSM_COMPUTATION_NOT_REQUIRED;
      qurt_mutex_unlock(&psm_info->lwm2m_psm_mutex);
      goto end;
    }

    else
    {
      ds_iot_ulog_err1("LWM2M PSM vote returned with error %d .", result);
      goto end;
    }
  }

end:
  if(interval)
  {
    lwm2m_free(interval);
    interval = NULL;
  }
  return result;
}

/**
 * @fn bool lwm2m_psm_node_modified()
 *
 * @brief This function is used to check that any PSM node is modified or not.
 *
 * @param lwm2mP  LwM2M client context
 *
 * @return TRUE if any node is modified else false
 */
bool lwm2m_psm_node_modified(lwm2m_context_t *contextP)
{
  lwm2m_psm_context_t *psm_info = NULL;
  lwm2m_psm_scheduler_t *node_list = NULL;
  bool result = FALSE;

  if(contextP == NULL || contextP->lwm2m_psm_info == NULL)
  {
    LOG_DEBUG(" PSM context is not defined.");
    return QAPI_ERR_INVALID_PARAM;
  }

  psm_info = contextP->lwm2m_psm_info;

  qurt_mutex_lock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);

  node_list = psm_info->psm_schedule_list;

  while(node_list)
  {
    if(node_list->node_mask & PSM_NODE_ADDED)
    {
      result = TRUE;
      break;
    }
    else if(node_list->node_mask & PSM_NODE_REMOVED)
    {
      result = TRUE;
      break;
    }
    else if((node_list->node_mask & PSM_NODE_MODIFIED))
    {
      result = TRUE;
      break;
    } 
    node_list= node_list->next;
  }

  qurt_mutex_unlock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);
  return result;
}

/**
 * @fn qapi_Status_t lwm2m_psm_compute_time()
 *
 * @brief This function is used to compute optimal time to vote for PSM by LwM2M
 *        client based on differnt nodes configured..
 *
 * @param lwm2mP  LwM2M client context
 *
 * @return See Section @xref{hdr:QAPIStatust}. \n
 * On success, QAPI_OK (0) is returned. Other value on error.
 */
qapi_Status_t lwm2m_psm_compute_time(lwm2m_context_t *contextP)
{
  lwm2m_psm_context_t *psm_info = NULL;
  lwm2m_psm_scheduler_t *node_list = NULL;
  psm_time_interval_type *optimal_interval = NULL;
  psm_time_interval_group_type *interval_group = NULL;
  qapi_Status_t result = QAPI_ERROR;
  uint16_t active_count = 0;
  time_t currentTime = lwm2m_gettime();

  if(contextP == NULL || contextP->lwm2m_psm_info == NULL)
  {
    LOG_DEBUG(" PSM context is not defined.");
    return QAPI_ERR_INVALID_PARAM;
  }

  psm_info = contextP->lwm2m_psm_info;

  qurt_mutex_lock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);

  node_list = psm_info->psm_schedule_list;

  /* Loop through all the LwM2M PSM nodes.
     Calculate the count of PSM nodes which are ready for PSM vote.*/
  while(node_list)
  {
    if(node_list->node_active)
    {
      active_count++;
    }
    node_list = node_list->next;
  }

  ds_iot_ulog_high1("PSM active count %d",active_count);
  /* Check whether all the LwM2M PSM nodes are ready for PSM computation, then 
     only proceed for optimal interval calculation.
     In case of Bootstraping state, DM servers would also be part of PSM node list
     but we need to ignore those and only consider BS server node is active or not.
     In case of rebootstrapping only BS node would be active so LwM2M would vote for
     PSM in this case also.*/
  if(active_count && ((active_count == psm_info->psm_node_count) || 
                     ((contextP->state == STATE_BOOTSTRAPPING) && active_count == 1))
                     || (active_count == 1 && rebootstrap_psm == TRUE))
  {
    int i = 0;
    node_list = psm_info->psm_schedule_list;
    interval_group = (psm_time_interval_group_type *)lwm2m_malloc(sizeof(psm_time_interval_group_type));
    if(interval_group == NULL)
    {
      qurt_mutex_unlock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);
      return QAPI_ERR_NO_MEMORY;
    }
    memset(interval_group, 0x00, sizeof(psm_time_interval_group_type));

    interval_group->intervals = (psm_time_interval_type *)lwm2m_calloc(active_count,
                                                                       sizeof(psm_time_interval_type));
    if(interval_group->intervals == NULL)
    {
      result = QAPI_ERR_NO_MEMORY;
      goto end;
    }
    memset(interval_group->intervals, 0x00, active_count*sizeof(psm_time_interval_type));

    while(node_list)
    {
      if(node_list->node_active)
      {
        /* If either of node start interval or node end interval is less than 
           current time or Node start interval is greater than node end interval 
           then make node inactive and don't do optimal interval calculation..*/
        if(node_list->node_start_interval + node_list->node_active_time <= currentTime ||
           node_list->node_end_interval + node_list->node_active_time <= currentTime ||
           node_list->node_start_interval > node_list->node_end_interval)
        {
          ds_iot_ulog_high1("PSM node made inactive %p", node_list);
          node_list->node_active = FALSE;
          goto end;
        }
        interval_group->intervals[i].start.time_format_flag = PSM_TIME_IN_SECS;
        interval_group->intervals[i].start.psm_duration_in_secs= (node_list->node_start_interval + node_list->node_active_time
                                                                - currentTime);
        interval_group->intervals[i].end.time_format_flag = PSM_TIME_IN_SECS;
        interval_group->intervals[i].end.psm_duration_in_secs = (node_list->node_end_interval + node_list->node_active_time
                                                               - currentTime);
        interval_group->intervals[i].psm_wakeup_type = node_list->node_wakeup_type;
        ds_iot_ulog_high2("PSM enter wakeup start time %lu end time %lu ",interval_group->intervals[i].start.psm_duration_in_secs,
                                                         interval_group->intervals[i].end.psm_duration_in_secs);

        i++;
      }
      node_list = node_list->next;
    }
    interval_group->number_of_intervals = active_count;

    optimal_interval = (psm_time_interval_type *)lwm2m_malloc(sizeof(psm_time_interval_type));
    if(optimal_interval == NULL)
    {
      result = QAPI_ERR_NO_MEMORY;
      goto end;
    }
    memset(optimal_interval, 0x00, sizeof(psm_time_interval_type));

    result = qapi_PSM_Client_Get_Optimal_Interval(psm_info->client_id,
                                                  interval_group,
                                                  optimal_interval);

    if(result == QAPI_OK && optimal_interval)
    {
      if(optimal_interval->start.time_format_flag == PSM_TIME_IN_TM)
      {
        psm_info->lwm2m_wakeup_start_time = time_jul_to_secs((const time_julian_type *)&optimal_interval->start.wakeup_time);
      }
      else if(optimal_interval->start.time_format_flag == PSM_TIME_IN_SECS)
      {
        psm_info->lwm2m_wakeup_start_time = optimal_interval->start.psm_duration_in_secs;
      }
      if(optimal_interval->end.time_format_flag == PSM_TIME_IN_TM)
      {
        psm_info->lwm2m_wakeup_end_time= time_jul_to_secs((const time_julian_type *)&optimal_interval->end.wakeup_time);
      }
      else if(optimal_interval->end.time_format_flag == PSM_TIME_IN_SECS)
      {
        psm_info->lwm2m_wakeup_end_time = optimal_interval->end.psm_duration_in_secs;
      }
      psm_info->lwm2m_wakeup_type = optimal_interval->psm_wakeup_type;
      ds_iot_ulog_high2("PSM qapi wakeup start time %lu end time %lu",optimal_interval->start.psm_duration_in_secs,
                                                         optimal_interval->end.psm_duration_in_secs);

      /* Adjust the boot_up_sleep time */
      psm_info->lwm2m_wakeup_start_time = optimal_interval->start.psm_duration_in_secs - data.cfg_info.boot_up_sleep;
      psm_info->lwm2m_wakeup_end_time =  optimal_interval->end.psm_duration_in_secs - data.cfg_info.boot_up_sleep;

      /* After adjusting bootsleep time check whether wakeup start and end time
         are in synchronization. */
      if(psm_info->lwm2m_wakeup_start_time > 0 && psm_info->lwm2m_wakeup_end_time > 0 
         && psm_info->lwm2m_wakeup_start_time <= psm_info->lwm2m_wakeup_end_time)
      {
        if(optimal_interval->start.psm_duration_in_secs > data.cfg_info.boot_up_sleep)
        {
          goto end;
        }
      }

      ds_iot_ulog_high("PSM vote cancelled due to wrong interval");
      result = QAPI_ERROR;
    }
    else
    {
      ds_iot_ulog_err1("Optimal interval calculation failed with error %d", result);
    }
  }

end:
  if(interval_group)
  {
    if(interval_group->intervals)
    {
      lwm2m_free(interval_group->intervals);
      interval_group->intervals = NULL;
    }
    lwm2m_free(interval_group);
    interval_group = NULL;
  }

  if(optimal_interval)
  {
    lwm2m_free(optimal_interval);
    optimal_interval = NULL;
  }

  qurt_mutex_unlock(&contextP->lwm2m_psm_info->lwm2m_psm_mutex);
  rebootstrap_psm = FALSE;
  return result;
}

void psm_step(lwm2m_context_t *contextP,
              time_t currentTime,
              time_t * timeoutP)
{
  lwm2m_psm_context_t *psm_info = contextP->lwm2m_psm_info;
  qapi_Status_t result = QAPI_ERROR;

  if(psm_info == NULL)
  {
    LOG_DEBUG("PSM context is not defined");
    return;
  }

  /* If PSM is enabled from modem/NW side then only do computation, else consider
     client as PSM unaware.*/

  if(psm_info->psm_enabled)
  {
    next_step:
    switch(psm_info->compute_state)
    {
      case PSM_COMPUTATION_REQUIRED:
      {
        ds_iot_ulog_high("Going to PSM compute state");
        /* Compute the LwM2M PSM vote time and if computation results in success,
           then LwM2M client is ready for PSM vote. Move the state to computation
           not required state if successful. If API returns error continue to be 
           in compute required state and don't vote for PSM.*/
        result = lwm2m_psm_compute_time(contextP);
        if(result == QAPI_OK)
        {
          qurt_mutex_lock(&psm_info->lwm2m_psm_mutex);
          psm_info->compute_state = PSM_COMPUTATION_NOT_REQUIRED;
          qurt_mutex_unlock(&psm_info->lwm2m_psm_mutex);
        }
      }
      break;
      case PSM_COMPUTATION_NOT_REQUIRED:
      {
        /* LwM2M client voted for PSM but due to some reason UE didn't go to PSM,
           we will wait for LwM2M wakeup start time + boot up sleep time and move 
           the state to computation required state.*/
        if(psm_info->psm_voted)
        {
           if((currentTime >= (psm_info->last_psm_vote_time + 
                                   psm_info->lwm2m_wakeup_start_time + 
                                   data.cfg_info.boot_up_sleep)))
        {
          ds_iot_ulog_high("PSM unvote due to PSM time expiry.");
          psm_info->psm_voted = FALSE;
          psm_info->last_psm_vote_time = 0;
          qurt_mutex_lock(&psm_info->lwm2m_psm_mutex);
          psm_info->compute_state = PSM_COMPUTATION_REQUIRED;
          qurt_mutex_unlock(&psm_info->lwm2m_psm_mutex);
          goto next_step;
        }
           else
           {
              time_t interval = 0;
              interval = (psm_info->last_psm_vote_time + 
                                      psm_info->lwm2m_wakeup_start_time + 
                                      data.cfg_info.boot_up_sleep ) - currentTime;
              if(*timeoutP > interval )
              {
                 *timeoutP = interval;
              }
          }
        }
        /* LwM2M PSM needs to be moved to computation required state if new LwM2M
           node is added, removed or modifed.*/
        if(lwm2m_psm_node_modified(contextP) == TRUE)
        {
          qurt_mutex_lock(&psm_info->lwm2m_psm_mutex);
          psm_info->compute_state = PSM_COMPUTATION_REQUIRED;
          qurt_mutex_unlock(&psm_info->lwm2m_psm_mutex);
          goto next_step;
        }
      }
      break;
    }
    if(currentTime < (active_time + sleepAckTimeout))
    {
       if(*timeoutP > ((active_time + sleepAckTimeout) - currentTime))
       {
          *timeoutP = (active_time + sleepAckTimeout) - currentTime;
       }
    }

    /* LwM2M client is ready to vote for PSM only when psm compute time API 
       was successful. Else no need to vote for PSM and continue to be in 
       compuation required state.*/
    if(result == QAPI_OK)
    {
      lwm2m_vote_for_psm(contextP);
    }
  }
  else
  {
    ds_iot_ulog_high("PSM is not enabled");
    return;
  }
}

#endif

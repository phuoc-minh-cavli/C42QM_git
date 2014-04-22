/******************************************************************************

  @file    lwm2m_carrier.c
  @brief   Interface File Containing Interfaces and DS w.r.t carrier specific
           implementations

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/
#include <time.h>

#include "liblwm2m.h"
#include "lwm2m_signals.h"
#include "lwm2mclient.h"
#include "fs_public.h"
#include "log_utils.h"
#include "iotapp_dss_util.h" 
#include "ds_log.h"

#define MAX_LOC_PATH_SIZE   128
#include<stdbool.h>

extern bool lwm2m_psm_enable;  // LWM2M PSM is enabled or not.

extern uint32_t active_time;

/**
 * @brief This enum defines the different registration status
 */
typedef enum {
  LWM2M_CARRIER_REGSTATUS_REGISTERED = 1,
  LWM2M_CARRIER_REGSTATUS_DISABLED,
  LWM2M_CARRIER_REGSTATUS_REG_RETRYING,
  LWM2M_CARRIER_REGSTATUS_REGFAILED,
  LWM2M_CARRIER_REGSTATUS_NOTREGISTERED,
  LWM2M_CARRIER_BS_STATUS_HOLD_OFF,
  LWM2M_CARRIER_BS_STATUS_INITIATED,
  LWM2M_CARRIER_BS_STATUS_PENDING,
  LWM2M_CARRIER_BS_STATUS_FINISHED,
  LWM2M_CARRIER_BS_STATUS_FAILED,
  LWM2M_CARRIER_BS_STATUS_RETRYING,
  LWM2M_CARRIER_BS_STATUS_DEREGISTERED,
} lwm2m_carrier_eregstatus;

/**
 * @brief This structure consists of registration status information for persistance
 */
typedef struct {
  uint8_t  locationPath[MAX_LOC_PATH_SIZE];
  time_t   stateTimeStamp; //! Timestamp when the status changed.
  time_t   lifetime; //! Lifetime value to be considered if status is registered
  time_t   disableTimeout; //! Disable Timeout value to be considered if the status is disabled
  time_t   next_cycle_for_registration; //! Next cycle for registration to be considered if status is regfailed
  time_t   prevAttemptTime;  //! Previous registration attempted time to be considered if status is reg_retrying or regfailed
  time_t   retryTimeout; //Retry timeout to be considered if status is reg_retrying
  time_t   last_BS_Operation_Time; // Last BS operation performed time
  uint16_t shortID; //! Short server id of the server to which this info belongs
  lwm2m_carrier_eregstatus  regStatus; //! Registration status
  uint8_t  retry_attempt; //! Current retry count 
  uint8_t  seq_retry_attempt; //! Current communication sequence retry count 
  time_t   initial_holdoff_time; //! Holdoff time to wait before 1st registration attempt.
#if defined (LWM2M_BOOTSTRAP) 
  bool     isBootstrapped;
#endif  
} lwm2m_carrier_regstatus_t;

/**
 * @brief This structure consists of active server information for persistance
 */
typedef struct {
  uint16_t ssid;
  time_t   initial_holdoff_time;
  bool last_server;
  uint16_t pri_order;
} lwm2m_active_server_list_t;

/**
 * @fn lwm2m_carrier_init_regstatus(lwm2m_context_t *lwm2mH)
 * @brief This function is used to fill in the registration status from config file to server List
 * @param lwm2mH This is the handle to lwm2m context
 * @return On success, ESUCCESS; On failure, EFAILURE.
 */
int lwm2m_carrier_init_regstatus(lwm2m_context_t *lwm2mH)
{
  lwm2m_server_t *serverP = NULL;
  lwm2m_carrier_regstatus_t status;
  uint32_t count = 0;
  int fd = 0;

  if (NULL == lwm2mH)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",REGISTRATION_STATUS_PERSISTENCE_FILE);
  fd = efs_open(REGISTRATION_STATUS_PERSISTENCE_FILE, O_RDONLY);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open m2m_regstatus file ");
    return EFAILURE;
  }

  if (sizeof(uint32_t) != efs_read(fd, &count, sizeof(uint32_t)))
  {
    ds_iot_ulog_err("Unabled to read count from regstatus cfg");
    efs_close(fd);
    return EFAILURE;
  }

  while (count--)
  {
    serverP = lwm2mH->bootstrapServer;

    memset(&status, 0, sizeof(lwm2m_carrier_regstatus_t));
    if (sizeof(lwm2m_carrier_regstatus_t) != efs_read(fd, &status, sizeof(lwm2m_carrier_regstatus_t)))
    {
      if(serverP)
      ds_iot_ulog_err1("Failed to read regstatus for %u", serverP->shortID);
      efs_close(fd);
      return EFAILURE;
    }

    if(serverP !=  NULL)
    {
      if (status.shortID != serverP->shortID)
        serverP = NULL;
    }

    if(NULL == serverP)
    {
      serverP = lwm2mH->serverList;
      while (serverP)
      {
        if (status.shortID == serverP->shortID)
          break;
        serverP = serverP->next;
      }
      if( NULL == serverP)
      {
        LOG_DEBUG("Server not found ");
        continue;
      }
    }

#if defined (LWM2M_BOOTSTRAP)      
    serverP->isBootstrapped = status.isBootstrapped ;
#endif  
    switch(status.regStatus)
    {
      case LWM2M_CARRIER_REGSTATUS_REGISTERED:
      {
        serverP->status = STATE_REGISTERED;
        serverP->registration = status.stateTimeStamp;
        serverP->lifetime = status.lifetime;
        serverP->location = lwm2m_strdup((char *)status.locationPath);
        break;
      }

      case LWM2M_CARRIER_REGSTATUS_DISABLED:
      {
        serverP->status = STATE_DEREGISTERED;
        LWM2M_SERVER_SET_DISABLE_ID(serverP->update_disable_check);
        serverP->disabled = status.stateTimeStamp;
        serverP->disable_timeOut = status.disableTimeout;
        break;
      }

      case LWM2M_CARRIER_REGSTATUS_REG_RETRYING:
      {
        serverP->status = STATE_REG_RETRYING;
        serverP->retryTimeout = status.retryTimeout;
        serverP->prevAttemptTime = status.prevAttemptTime;
        serverP->next_cycle_for_registration = status.next_cycle_for_registration;
        serverP->retry_attempt = status.retry_attempt;
        serverP->seq_retry_attempt = status.seq_retry_attempt;
        break;
      }

      case LWM2M_CARRIER_REGSTATUS_REGFAILED:
      {
        serverP->status = STATE_REG_FAILED;
        serverP->retryTimeout = status.retryTimeout;
        serverP->prevAttemptTime = status.prevAttemptTime;
        serverP->next_cycle_for_registration = status.next_cycle_for_registration;
        serverP->retry_attempt = status.retry_attempt;
        serverP->seq_retry_attempt = status.seq_retry_attempt;
        break;
      }

      case LWM2M_CARRIER_REGSTATUS_NOTREGISTERED:
      case LWM2M_CARRIER_BS_STATUS_DEREGISTERED:
      {
        serverP->initial_holdoff_time = status.initial_holdoff_time;
        serverP->status = STATE_DEREGISTERED;
        break;
      }

      case LWM2M_CARRIER_BS_STATUS_HOLD_OFF:
      {
        serverP->status = STATE_BS_HOLD_OFF;
        serverP->lifetime = status.lifetime;
        serverP->registration = status.stateTimeStamp;
        break;
      }

      case LWM2M_CARRIER_BS_STATUS_INITIATED:
      {
        serverP->status = STATE_BS_INITIATED;
        serverP->prevAttemptTime = status.prevAttemptTime;
        break;
      }

      case LWM2M_CARRIER_BS_STATUS_PENDING:
      {
        serverP->status = STATE_BS_PENDING;
        serverP->last_BS_Operation_Time = status.last_BS_Operation_Time;
        break;
      }

      case LWM2M_CARRIER_BS_STATUS_FINISHED:
      {
        serverP->status = STATE_BS_FINISHED;
        break;
      }

      case LWM2M_CARRIER_BS_STATUS_FAILED:
      {
        serverP->status = STATE_BS_FAILED;
        serverP->retryTimeout = status.retryTimeout;
        serverP->prevAttemptTime = status.prevAttemptTime;
        serverP->next_cycle_for_registration = status.next_cycle_for_registration;
        break;
      }

      case LWM2M_CARRIER_BS_STATUS_RETRYING:
      {
        serverP->status = STATE_BS_REG_RETRYING;
        serverP->retryTimeout = status.retryTimeout;
        serverP->prevAttemptTime = status.prevAttemptTime;
        serverP->next_cycle_for_registration = status.next_cycle_for_registration;
        break;
      }

      default:
      {
        ds_iot_ulog_high("regstatus cfg file is tainted");
        efs_close(fd);
        return EFAILURE;
      }
    }

    #ifdef PSM_LWM2M
	lwm2m_psm_operation(lwm2mH, serverP->shortID, NULL, PSM_NODE_SERVER, PSM_NODE_UPDATE, PSM_MODIFY_NODE);

    #endif
  }

  efs_close(fd);
  return ESUCCESS;
}

void lwm2m_update_server_status(lwm2m_carrier_regstatus_t *status, lwm2m_server_t *serverP)
{
  if(status == NULL || serverP == NULL)
  {
    return;
  }
  if (STATE_REGISTERED == serverP->status)
  {
    status->regStatus = LWM2M_CARRIER_REGSTATUS_REGISTERED;
    status->stateTimeStamp = serverP->registration;
    status->lifetime = serverP->lifetime;
    LWM2M_STRCPY((char *)status->locationPath,serverP->location, MAX_LOC_PATH_SIZE);
  }
  else if (STATE_DEREGISTERED == serverP->status)
  {
    if (LWM2M_SERVER_IS_DISABLE_SET(serverP->update_disable_check))
    {
      status->regStatus = LWM2M_CARRIER_REGSTATUS_DISABLED;
      status->stateTimeStamp = serverP->disabled;
      status->disableTimeout = serverP->disable_timeOut;
    }
    else
    {
      status->regStatus = LWM2M_CARRIER_REGSTATUS_NOTREGISTERED;
      status->initial_holdoff_time = serverP->initial_holdoff_time;
    }
  }
  else if(STATE_REG_RETRYING == serverP->status)
  {
     status->regStatus = LWM2M_CARRIER_REGSTATUS_REG_RETRYING;
     status->retryTimeout = serverP->retryTimeout;
     status->prevAttemptTime = serverP->prevAttemptTime;
     status->next_cycle_for_registration = serverP->next_cycle_for_registration;
     status->retry_attempt = serverP->retry_attempt;
     status->seq_retry_attempt = serverP->seq_retry_attempt;
  }
  else if(STATE_REG_FAILED == serverP->status)
  {
     status->regStatus = LWM2M_CARRIER_REGSTATUS_REGFAILED;
     status->retryTimeout = serverP->retryTimeout;
     status->prevAttemptTime = serverP->prevAttemptTime;
     status->next_cycle_for_registration = serverP->next_cycle_for_registration;
     status->retry_attempt = serverP->retry_attempt;
     status->seq_retry_attempt = serverP->seq_retry_attempt;
  }
  else if(STATE_BS_HOLD_OFF == serverP->status)
  {
    status->regStatus = LWM2M_CARRIER_BS_STATUS_HOLD_OFF;
    status->lifetime = serverP->lifetime;
    status->stateTimeStamp = serverP->registration;
  }
  else if(STATE_BS_INITIATED == serverP->status)
  {
    status->regStatus = LWM2M_CARRIER_BS_STATUS_INITIATED;
    status->prevAttemptTime = serverP->prevAttemptTime;
  }
  else if(STATE_BS_PENDING == serverP->status)
  {
    status->regStatus = LWM2M_CARRIER_BS_STATUS_PENDING;
    status->last_BS_Operation_Time = serverP->last_BS_Operation_Time;
  }
  else if(STATE_BS_FINISHED == serverP->status)
  {
    status->regStatus = LWM2M_CARRIER_BS_STATUS_FINISHED;
  }
  else if(STATE_BS_FAILED == serverP->status)
  {
    status->regStatus = LWM2M_CARRIER_BS_STATUS_FAILED;
    status->retryTimeout = serverP->retryTimeout;
    status->prevAttemptTime = serverP->prevAttemptTime;
    status->next_cycle_for_registration = serverP->next_cycle_for_registration;
  }
  else if(STATE_BS_REG_RETRYING == serverP->status)
  {
    status->regStatus = LWM2M_CARRIER_BS_STATUS_RETRYING;
    status->retryTimeout = serverP->retryTimeout;
    status->prevAttemptTime = serverP->prevAttemptTime;
    status->next_cycle_for_registration = serverP->next_cycle_for_registration;
  }
  else
  {
    status->regStatus = LWM2M_CARRIER_REGSTATUS_NOTREGISTERED;
  }
#if defined (LWM2M_BOOTSTRAP)
  status->isBootstrapped = serverP->isBootstrapped ;
#endif  
}

/**
 * @brief lwm2m_carrier_update_regstatus(lwm2m_server_t *serverP)
 * @brief This function is used to update registration status of the given server into cfg file
 * @param serverP handle to the server info
 * @return void
 */
void lwm2m_carrier_update_regstatus(lwm2m_server_t *serverP)
{
  lwm2m_carrier_regstatus_t *statusP = NULL;
  int fd = 0;
  uint32_t count = 0, tmp = 0;
  uint8_t *payload = NULL;
  uint16_t length = 0;
  lwm2m_carrier_regstatus_t status;

  if (NULL == serverP)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",REGISTRATION_STATUS_PERSISTENCE_FILE);
  fd = efs_open(REGISTRATION_STATUS_PERSISTENCE_FILE, O_RDONLY | O_CREAT, 00744);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open m2m_regstatus file ");
    return;
  }
  if (sizeof(uint32_t) == efs_read(fd, &count, sizeof(uint32_t)))
  {
    statusP = (lwm2m_carrier_regstatus_t *)lwm2m_calloc(count ? count : 1, sizeof(lwm2m_carrier_regstatus_t));
    if (NULL == statusP)
    {
      ds_iot_ulog_err("Failed to update regstatus");
      efs_close(fd);
      return;
    }

    if ((sizeof(lwm2m_carrier_regstatus_t) * count) != efs_read(fd, statusP, sizeof(lwm2m_carrier_regstatus_t) * count))
    {
      ds_iot_ulog_err("Failed to read regstatus cfg while updating");
      lwm2m_free(statusP);
      efs_close(fd);
      return;
    }
  }
  else
  {
    LOG_DEBUG("Unabled to read count from regstatus cfg, rewriting...");
  }
  efs_close(fd);
  ds_iot_log_strf_med("EFS_OPT: File open %s",REGISTRATION_STATUS_PERSISTENCE_FILE);
  fd = efs_open(REGISTRATION_STATUS_PERSISTENCE_FILE, O_WRONLY);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open m2m_regstatus file ");
    lwm2m_free(statusP);
    return;
  }

  if (NULL == statusP && count != 0)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return;
  }
  while (tmp < count)
  {
    if (statusP[tmp].shortID == serverP->shortID)
    {
      break;
    }
    tmp++;
  }

  if (tmp != count)
  {
    lwm2m_update_server_status(&statusP[tmp], serverP);
  }
  else
  {
    memset(&status, 0, sizeof(status));
    status.shortID = serverP->shortID;

    lwm2m_update_server_status(&status, serverP);
  }
  if (tmp != count)
    {
    length = (count * sizeof(lwm2m_carrier_regstatus_t) + 4);
    payload = lwm2m_malloc(length);
    if(payload == NULL)
      goto end;
    memset(payload, 0x00, length);

    memcpy(payload, &count, sizeof(uint32_t));
    if(statusP)
    memcpy(payload + 4, statusP, count * sizeof(lwm2m_carrier_regstatus_t));
    }
  else
    {
    count++;
    length = (count * sizeof(lwm2m_carrier_regstatus_t) + 4);
    payload = lwm2m_malloc(length);
    if(payload == NULL)
      goto end;
    memset(payload, 0x00, length);

    memcpy(payload, &count, sizeof(uint32_t));
    if(statusP)
    memcpy(payload + 4, statusP, (count - 1)* sizeof(lwm2m_carrier_regstatus_t));
    memcpy(payload + 4 + ((count - 1)* sizeof(lwm2m_carrier_regstatus_t)),
           &status, sizeof(lwm2m_carrier_regstatus_t));
  }
  ds_iot_log_strf_med("EFS_OPT: File write %s",REGISTRATION_STATUS_PERSISTENCE_FILE);
  efs_write(fd, payload, length);

  efs_close(fd);
end:
  if(statusP)
      {
        lwm2m_free(statusP);
      }
  if(payload)
  {
    lwm2m_free(payload);
    }
      return;
    }

/**
 * @brief sync_activeserverlist_to_persistence(lwm2m_context_t *contextP)
 * @brief This function is used to update the active server list into cfg file
 * @param contextP handle to the lwm2m context 
 * @return On success, ESUCCESS; On failure, EFAILURE.
 */
int sync_activeserverlist_to_persistence(lwm2m_context_t *contextP)
{
  int fd = 0;
  lwm2m_active_server_t *serverP = NULL;
  lwm2m_active_server_list_t *info = NULL;
  uint8_t count = 0, i = 0;

  if (NULL == contextP )
    return EFAILURE;
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_ACTIVE_SERVER_LIST);
  fd = efs_open(LWM2M_ACTIVE_SERVER_LIST, O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("Failed to open activeserverlist persistent storage");
    return EFAILURE;
  }
  if( contextP->activeServerList == NULL )
  {
    efs_close(fd);
    return ESUCCESS;  
  }


  serverP = contextP->activeServerList;
  while(serverP != NULL)
  {
    count++;
    serverP = serverP->next;
  }

  info = lwm2m_malloc(count * sizeof(lwm2m_active_server_list_t));
  if(info == NULL)
  {
    efs_close(fd);
    return EFAILURE;
  }
  serverP = contextP->activeServerList;
  memset(info, 0x00, count * sizeof(lwm2m_active_server_list_t));
  while(serverP != NULL)
  {
    info[i].ssid = serverP->ssid;
    info[i].initial_holdoff_time = serverP->initial_holdoff_time;
    info[i].pri_order = serverP->pri_order;
    if(contextP->lastActiveServer && (serverP->ssid == contextP->lastActiveServer->ssid))
    {
      info[i].last_server = TRUE;
    }
    else
    {
      info[i].last_server = FALSE;
    }
    i++;
    serverP = serverP->next;
    }

  ds_iot_log_strf_med("EFS_OPT: File write %s",LWM2M_ACTIVE_SERVER_LIST);
  if (EFAILURE == efs_write(fd, info, count * sizeof(lwm2m_active_server_list_t)))
    {
    ds_iot_ulog_err("Failed to write active server in EFS file ");
    }
  if(info)
  {
    free(info);
    info = NULL;
  }

  efs_close(fd);
  return ESUCCESS;
}

/**
 * @fn sync_activeserverlist_from_persistence(lwm2m_context_t *lwm2mH)
 * @brief This function is used to restore the active servers from config file to active server List
 * @param lwm2mH This is the handle to lwm2m context
 * @return On success, ESUCCESS; On failure, EFAILURE.
 */
int sync_activeserverlist_from_persistence(lwm2m_context_t *contextP)
{
  int fd = 0;

  if (NULL == contextP )
  {
    return EFAILURE;
  }

  fd = efs_open(LWM2M_ACTIVE_SERVER_LIST, O_RDONLY);
  if (EFAILURE == fd)
  {
    return EFAILURE;
  }

  while(1)
  {
    lwm2m_active_server_list_t info;
    lwm2m_server_t *serverP = NULL;
    lwm2m_active_server_t *targetP = NULL;
    int ret = 0;

    memset(&info, 0, sizeof(lwm2m_active_server_list_t));
    ret = efs_read(fd, &info, sizeof(lwm2m_active_server_list_t));
    if (EFAILURE == ret || 0 == ret)
      break;

    targetP = (lwm2m_active_server_t *)lwm2m_malloc(sizeof(lwm2m_active_server_t));
    if(targetP == NULL)
    {
      LOG_DEBUG("%s",mallocError);
      continue;
    }
    memset(targetP, 0x00, sizeof(lwm2m_active_server_t));
    targetP->ssid = info.ssid;
    targetP->pri_order = info.pri_order;
    contextP->activeServerList = (lwm2m_active_server_t *)LWM2M_LIST_ADD(contextP->activeServerList, targetP);
    serverP = lwm2m_get_server(targetP->ssid);
    if(serverP)
    {
      serverP->initial_holdoff_time = info.initial_holdoff_time;
    }
    targetP->initial_holdoff_time = info.initial_holdoff_time;
    if(info.last_server)
    {
      contextP->lastActiveServer = targetP;
    }
    #ifdef PSM_LWM2M
      if(lwm2m_psm_enable == TRUE)
      {
        lwm2m_uri_t uri;
        /* Add PSM node associated with DM server .*/
        memset(&uri, 0x00, sizeof(lwm2m_uri_t));
        lwm2m_psm_add_node(contextP, targetP->ssid, uri, PSM_NODE_SERVER);
      }
    #endif

  }
  efs_close(fd);
  return ESUCCESS;
}


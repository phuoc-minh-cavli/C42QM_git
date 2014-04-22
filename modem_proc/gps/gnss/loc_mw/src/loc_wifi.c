/******************************************************************************
  @file: loc_wifi.c
  @brief: Location API WiFi positioning module

  DESCRIPTION
   Qualcomm Location API WiFi Positioning Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
 
  Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
  All Rights Reserved.
  QCA Proprietary and Confidential. 

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
 
$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_wifi.c#1 $
$DateTime: 2020/07/21 21:23:44 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
01/11/16   yh       LocWifi cleanup
09/05/14   ssu      GM LOWI Integration
07/24/09   dx       Initial version
======================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "msg.h"

#include "pdapi.h"
#include "loc_api_2.h"

#include "loc_client.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"
#include "loc_utils.h"
#include "loc_qmi_shim.h"

// Prototypes
static void locWifi_EventCb (
   void *p_UserData,
   pdsm_pd_event_type q_PdEvent,
   const pdsm_pd_info_s_type *pz_PdInfo
);

/*---------------------------------------------------------------------------
@brief
   One time initializations for the WiFi module. This function is to be called 
   only once.

@param[in] None

@retval    TRUE  if the Init was successful 
@retval    FALSE  if the Init failed
---------------------------------------------------------------------------*/
boolean locWifi_Init()
{
   boolean                       v_RetVal = FALSE; /* default: not successful */
   pdsm_client_status_e_type     e_Status = PDSM_CLIENT_OK;

   LOC_MSG_MED("locWifi_Init: entered", 0, 0, 0);

   do { /* one time loop */

      // Check if this is valid when QWiP is updated
      LOC_MSG_HIGH("locWifi_Init: installing a hook in PD client", 0, 0, 0);
      e_Status = loc_pd_install_event_cb_hook(locWifi_EventCb);
      if (e_Status != PDSM_CLIENT_OK)
      {
         LOC_MSG_ERROR ("loc_pd_install_event_cb_hook failed, error code = %d", e_Status, 0, 0);
         break;
      }

      loc_middleware_data.wifi_client_id = pdsm_client_init(PDSM_CLIENT_TYPE_WIPER);
      if (loc_middleware_data.wifi_client_id == -1)
      {
         LOC_MSG_HIGH("locWifi_Init: pdsm_client_init for WiFi failed", 0, 0, 0);
         // break;
      }


      /* Initialize the Critical Section */
      os_MutexInit( &loc_middleware_data.wifi_crit_sec, MUTEX_DATA_ONLY_CONTEXT );


      LOC_MSG_HIGH("locWifi_Init: pdsm_client_init for WiFi successful, client id=%d",
            (int) loc_middleware_data.wifi_client_id, 0, 0);

      // Check if WiFi module needs callbacks
      /*
      e_Status = pdsm_client_pd(wifi)_reg(loc_middleware_data.wifi_client_id,
                                    NULL,
                                    locWifi_EventCb,
                                    PDSM_CLIENT_EVENT_REG,
                                    PDSM_PD_EVENT_WPS_NEEDED,
                                    NULL);
      if (e_Status != PDSM_CLIENT_OK)
      {
         pdsm_client_release(loc_middleware_data.wifi_client_id);
         LOC_MSG_ERROR ("pdsm_client_pd_reg failed, error code = %d", e_Status, 0, 0);
         break;
      }

      e_Status = pdsm_client_act(loc_middleware_data.wifi_client_id);
      if (e_Status != PDSM_CLIENT_OK)
      {
         pdsm_client_release(loc_middleware_data.wifi_client_id);
         LOC_MSG_ERROR ("pdsm_client_act failed, error code =%d", e_Status, 0, 0);
         break;
      }
      */

      // Reaching here means success
      v_RetVal = TRUE;
   } while (0); /* one time loop */

   LOC_MSG_MED ("locWifi_Init: returned %d", v_RetVal, 0, 0);
   return v_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  This function processes wiper position report injection callback

@param[in] 
  p_UserData        Pointer to client provided data block 
  e_WiperCmd        Indicate which command is this error e_Status for
  e_WiperCmdErr     Command error code, indicating whether command
                    is rejected and the reason

@retval    None
---------------------------------------------------------------------------*/
static void locWifi_CmdCb (
   void                      *p_UserData,
   pdsm_wiper_cmd_e_type      e_WiperCmd,
   pdsm_wiper_cmd_err_e_type  e_WiperCmdErr
)
{
   LOC_MSG_HIGH ("locWifi_CmdCb: cmd=%d, err=%d\n", e_WiperCmd, e_WiperCmd, 0);
}

/*---------------------------------------------------------------------------
@brief
  This function processes wiper position report injection

@param[in] 
  l_ClientHandle                   LocMW client handle
  pz_NotifyWifiEnabledStatus       QMI_LOC input pointer

@retval    Loc API return values (e.g., LOC_API_SUCCESS (or 0) for success)
---------------------------------------------------------------------------*/
static int locWifi_InjectWifiPos
(
  loc_client_handle_type                            l_ClientHandle,
  const qmiLocInjectWifiPositionReqMsgT_v02*        pz_InjectWifiPos
)
{
   t_wiper_position_report_ex_struct_type*   pz_PosReport= NULL;
   uint32                                    q_RetVal = LOC_API_GENERAL_FAILURE;

   /* Function arguments sanity check */
   if ((NULL == pz_InjectWifiPos) || (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle))
   {
      LOC_MSG_ERROR("locWifi_InjectWifiPos: NULL parameters", 0, 0, 0);
      return LOC_API_INVALID_PARAMETER;
   }

   do
   {
      pz_PosReport = (t_wiper_position_report_ex_struct_type *)loc_calloc(sizeof(t_wiper_position_report_ex_struct_type));
      if (NULL == pz_PosReport)
      {
         LOC_MSG_ERROR("locWifi_InjectWifiPos: calloc return NULL", 0, 0, 0);
         break;
      }

      q_RetVal = LOC_API_SUCCESS;
      /***************************
       *  Copying the data       *
       ***************************/
      if(pz_InjectWifiPos->wifiFixTime_valid)
      {
         pz_PosReport->wiper_pos_report.wiper_valid_info_flag |= WIPER_LOG_TIME_VALID;
         pz_PosReport->wiper_pos_report.wiper_fix_time.slow_clock_count = pz_InjectWifiPos->wifiFixTime.wifiPositionTime;
      }

      if(pz_InjectWifiPos->wifiFixPosition_valid)
      {
         pz_PosReport->wiper_pos_report.wiper_valid_info_flag |= WIPER_LOG_POS_VALID;
         pz_PosReport->wiper_pos_report.wiper_fix_position.lat = pz_InjectWifiPos->wifiFixPosition.lat;
         pz_PosReport->wiper_pos_report.wiper_fix_position.lon = pz_InjectWifiPos->wifiFixPosition.lon;
         pz_PosReport->wiper_pos_report.wiper_fix_position.HEPE = pz_InjectWifiPos->wifiFixPosition.hepe;
         pz_PosReport->wiper_pos_report.wiper_fix_position.num_of_aps_used = pz_InjectWifiPos->wifiFixPosition.numApsUsed;
         pz_PosReport->wiper_pos_report.wiper_fix_position.fix_error_code = pz_InjectWifiPos->wifiFixPosition.fixErrorCode;
      }

      if( ( pz_InjectWifiPos->apInfo_valid ) &&
         ( pz_InjectWifiPos->wifiApSsidInfo_valid )  &&
         ( pz_InjectWifiPos->apInfo_len  != pz_InjectWifiPos->wifiApSsidInfo_len)
      )
      {
         LOC_MSG_ERROR("locWifi_InjectWifiPos: Ap info length %d and SSID info length %dnot matching",
                        pz_InjectWifiPos->apInfo_len,pz_InjectWifiPos->wifiApSsidInfo_len,0);
         q_RetVal = LOC_API_INVALID_PARAMETER;
         break;
      }

      if(pz_InjectWifiPos->apInfo_valid)
      {
         uint8 u_ApCntr;
         pz_PosReport->wiper_pos_report.wiper_valid_info_flag |= WIPER_LOG_AP_SET_VALID;
         pz_PosReport->wiper_pos_report.wiper_ap_set.num_of_aps = 
                   (uint8) MIN ( QMI_LOC_WIFI_MAX_REPORTED_APS_PER_MSG_V02,
                                 pz_InjectWifiPos->apInfo_len );
         for ( u_ApCntr = 0;
               u_ApCntr < pz_PosReport->wiper_pos_report.wiper_ap_set.num_of_aps;
               u_ApCntr++ )
         {

           pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].meas_age = -1;
           pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].ap_qualifier =
                                 pz_InjectWifiPos->apInfo[u_ApCntr].apQualifier;

           pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].rssi = 
                                 pz_InjectWifiPos->apInfo[u_ApCntr].rssi;

           pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].channel =
                                 pz_InjectWifiPos->apInfo[u_ApCntr].channel;

           memscpy(pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].mac_addr,
              sizeof(pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].mac_addr),
              pz_InjectWifiPos->apInfo[u_ApCntr].macAddr,
              sizeof(pz_InjectWifiPos->apInfo[u_ApCntr].macAddr));

           if(pz_InjectWifiPos->wifiApSsidInfo_valid)
           {
             GNSS_STRLCPY((char *)pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].ssid,
                          pz_InjectWifiPos->wifiApSsidInfo[u_ApCntr].ssid,
                          sizeof(pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].ssid));
           }
         }
      }

      if(pz_InjectWifiPos->horizontalReliability_valid)
      {
         switch(pz_InjectWifiPos->horizontalReliability)
         {
            case eQMI_LOC_RELIABILITY_NOT_SET_V02:
               pz_PosReport->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_NOT_SET;
               break;
            case eQMI_LOC_RELIABILITY_VERY_LOW_V02:
               pz_PosReport->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_VERY_LOW;
               break;
            case eQMI_LOC_RELIABILITY_LOW_V02:
               pz_PosReport->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_LOW;
               break;
            case eQMI_LOC_RELIABILITY_MEDIUM_V02:
               pz_PosReport->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_MEDIUM;
               break;
            case eQMI_LOC_RELIABILITY_HIGH_V02:
               pz_PosReport->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_HIGH;
               break;
            default:
               pz_PosReport->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_NOT_SET;
               break;
         }
      }

      /* Note: rawHepe has not been used for WIFI position yet */

      /***************************
       *  Makes the PDSM call    *
       ***************************/
      // XXX check if the client_id should be wifi_client_id
      if(LOC_API_SUCCESS == q_RetVal)
      {
         pdsm_send_wiper_position_report_ex(locWifi_CmdCb,
                loc_middleware_data.wifi_client_id,      // Client id of user
                NULL,                                    // Pointer to client data block
                pz_PosReport);
      }

      if (!locMW_SendQmiIndicationProxy(
                    l_ClientHandle,
                    QMI_LOC_INJECT_WIFI_POSITION_IND_V02, 
                    NULL, q_RetVal))
      {
         LOC_MSG_ERROR("locWifi_InjectWifiPos: Could not send IPC to LocMW", 0, 0, 0);
         q_RetVal = LOC_API_GENERAL_FAILURE;
      }
   }while(0);

   if(pz_PosReport)
   {
      loc_free(pz_PosReport);
   }
   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  This function processes wiper e_Status report

@param[in] 
  l_ClientHandle                   LocMW client handle
  pz_NotifyWifiEnabledStatus       QMI_LOC input pointer

@retval    Loc API return values (e.g., LOC_API_SUCCESS (or 0) for success)
---------------------------------------------------------------------------*/
static int locWifi_NotifyWifiStatus
(
   loc_client_handle_type                          l_ClientHandle,
   const qmiLocNotifyWifiStatusReqMsgT_v02*        pz_NotifyWifiStatus
)
{
   uint32       q_RetVal = LOC_API_GENERAL_FAILURE;
   boolean      v_WiperStatus = FALSE;

  /* Function arguments sanity check */
  if ((NULL == pz_NotifyWifiStatus) || (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle))
  {
    LOC_MSG_ERROR("locWifi_NotifyWifiStatus: NULL parameters", 0, 0, 0);
    return LOC_API_INVALID_PARAMETER;
  }

   /***************************
      *  Makes the PDSM call    *
      ***************************/
   q_RetVal = LOC_API_SUCCESS;
   switch (pz_NotifyWifiStatus->wifiStatus)
   {
      case eQMI_LOC_WIFI_STATUS_AVAILABLE_V02:
         v_WiperStatus = TRUE;
         break;
      case eQMI_LOC_WIFI_STATUS_UNAVAILABLE_V02:
         v_WiperStatus = FALSE;
         break;
      default:
         q_RetVal = LOC_API_INVALID_PARAMETER;
         break;
   }

   if(LOC_API_SUCCESS == q_RetVal)
   {
      pdsm_notify_wiper_status(v_WiperStatus);
   }

   /* Send IPC message to LocMW task to handle QMI_LOC indication */
   if (!locMW_SendQmiIndicationProxy(
                  l_ClientHandle,
                  QMI_LOC_NOTIFY_WIFI_STATUS_IND_V02, 
                  NULL, q_RetVal))
   {
      LOC_MSG_ERROR("locWifi_NotifyWifiStatus: Could not send IPC to LocMW", 0, 0, 0);
      q_RetVal = LOC_API_GENERAL_FAILURE;
   }

   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  This function processes wiper attachment e_Status report

@param[in] 
  l_ClientHandle                   LocMW client handle
  pz_NotifyWifiEnabledStatus       QMI_LOC input pointer

@retval    Loc API return values (e.g., LOC_API_SUCCESS (or 0) for success)
---------------------------------------------------------------------------*/
static int locWifi_NotifyWifiAttachStatus
(
   loc_client_handle_type                              l_ClientHandle,
   const qmiLocNotifyWifiAttachmentStatusReqMsgT_v02*  pz_NotifyWifiAttachStatus
)
{
   uint32              q_RetVal = LOC_API_GENERAL_FAILURE;
   pdsm_wifi_attachment_status_ind_info z_WifiAttachStatusIndInfo = {0};

   /* Function arguments sanity check */
   if ((NULL == pz_NotifyWifiAttachStatus) || (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle))
   {
      LOC_MSG_ERROR("locWifi_NotifyWifiAttachStatus: NULL parameters", 0, 0, 0);
      return LOC_API_INVALID_PARAMETER;
   }

   /***************************
      *  Makes the PDSM call    *
      ***************************/
   q_RetVal = LOC_API_SUCCESS;
   switch (pz_NotifyWifiAttachStatus->attachState)
   {
      case eQMI_LOC_WIFI_ACCESS_POINT_ATTACHED_V02:
         z_WifiAttachStatusIndInfo.wifi_attachment_status_ind_type = WIFI_ATTACHMENT_STATUS_CONNECTED;
         break;
      case eQMI_LOC_WIFI_ACCESS_POINT_DETACHED_V02:
         z_WifiAttachStatusIndInfo.wifi_attachment_status_ind_type = WIFI_ATTACHMENT_STATUS_DISCONNECTED;
         break;
      case eQMI_LOC_WIFI_ACCESS_POINT_HANDOVER_V02:
         z_WifiAttachStatusIndInfo.wifi_attachment_status_ind_type = WIFI_ATTACHMENT_STATUS_HANDOVER;
         break;
      default:
         q_RetVal = LOC_API_INVALID_PARAMETER;
         break;
   }

   if(TRUE == pz_NotifyWifiAttachStatus->accessPointMacAddress_valid)
   {
      memscpy(z_WifiAttachStatusIndInfo.u.wifi_ap_info.mac_addr,
            sizeof(z_WifiAttachStatusIndInfo.u.wifi_ap_info.mac_addr),
            pz_NotifyWifiAttachStatus->accessPointMacAddress,
            sizeof(pz_NotifyWifiAttachStatus->accessPointMacAddress));
   }

   if(TRUE == pz_NotifyWifiAttachStatus->wifiApSsid_valid)
   {
      GNSS_STRLCPY((char *)z_WifiAttachStatusIndInfo.u.wifi_ap_info.ssid,
                     pz_NotifyWifiAttachStatus->wifiApSsid,
                     sizeof(z_WifiAttachStatusIndInfo.u.wifi_ap_info.ssid));
   }

   if(LOC_API_SUCCESS == q_RetVal)
   {
      pdsm_wifi_attachment_status_ind_report(&z_WifiAttachStatusIndInfo);
   }

   /* Send IPC message to LocMW task to handle QMI_LOC indication */
   if (!locMW_SendQmiIndicationProxy(
                  l_ClientHandle,
                  QMI_LOC_NOTIFY_WIFI_ATTACHMENT_STATUS_IND_V02, 
                  NULL, q_RetVal))
   {
      LOC_MSG_ERROR("locWifi_NotifyWifiAttachStatus: Could not send IPC to LocMW", 0, 0, 0);
      q_RetVal = LOC_API_GENERAL_FAILURE;
   }

   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  This function processes wifi enabled e_Status

@param[in] 
  l_ClientHandle                   LocMW client handle
  pz_NotifyWifiEnabledStatus       QMI_LOC input pointer

@retval    Loc API return values (e.g., LOC_API_SUCCESS (or 0) for success)
---------------------------------------------------------------------------*/
static int locWifi_NotifyWifiEnabledStatus
(
   loc_client_handle_type                            l_ClientHandle,
   const qmiLocNotifyWifiEnabledStatusReqMsgT_v02*   pz_NotifyWifiEnabledStatus
)
{
   uint32             q_RetVal = LOC_API_GENERAL_FAILURE;
   pdsm_wifi_enabled_status_ind_type e_WifiEnabledStatus;

   /* Function arguments sanity check */
   if ((NULL == pz_NotifyWifiEnabledStatus) || (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle))
   {
      LOC_MSG_ERROR("locWifi_NotifyWifiEnabledStatus: NULL parameters", 0, 0, 0);
      return LOC_API_INVALID_PARAMETER;
   }

   /***************************
      *  Makes the PDSM call    *
      ***************************/
   q_RetVal = LOC_API_SUCCESS;
   switch (pz_NotifyWifiEnabledStatus->enabledStatus)
   {
      case eQMI_LOC_WIFI_ENABLED_TRUE_V02:
         e_WifiEnabledStatus = WIFI_ENABLED_STATUS_TRUE;
         break;
      case eQMI_LOC_WIFI_ENABLED_FALSE_V02:
         e_WifiEnabledStatus = WIFI_ENABLED_STATUS_FALSE;
         break;
      default:
         q_RetVal = LOC_API_INVALID_PARAMETER;
         break;
   }

   if(LOC_API_SUCCESS == q_RetVal)
   {
      pdsm_wifi_enabled_status_report(e_WifiEnabledStatus);
   }

   /* Send IPC message to LocMW task to handle QMI_LOC indication */
   if (!locMW_SendQmiIndicationProxy(
                  l_ClientHandle,
                  QMI_LOC_NOTIFY_WIFI_ENABLED_STATUS_IND_V02, 
                  NULL, q_RetVal))
   {
      LOC_MSG_ERROR("locWifi_NotifyWifiEnabledStatus: Could not send IPC to LocMW", 0, 0, 0);
      q_RetVal = LOC_API_GENERAL_FAILURE;
   }

   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  PD event callback, this is where WPS_NEEDED event will come

@param[in] 
  p_UserData      user data pointer 
  q_PdEvent       Indicate PD event
  pz_PdInfo       PD info pointer

@retval    None
---------------------------------------------------------------------------*/
static void locWifi_EventCb
(
   void                           *p_UserData,
   pdsm_pd_event_type             q_PdEvent,
   const pdsm_pd_info_s_type      *pz_PdInfo
)
{
   qmiLocEventWifiReqIndMsgT_v02*   pz_WifiReqIndMsg = NULL;
   boolean                          v_RetVal = TRUE;

   LOC_MSG_MED("locWifi_EventCb entered: psdm client id=%d, q_PdEvent=0x%x",
         (int) pz_PdInfo->client_id,
         (int) q_PdEvent, 0);

   // WPS needed event
   if (q_PdEvent & PDSM_PD_EVENT_WPS_NEEDED)
   {
      // Send an IPC to LOC_MW task and then call the callback in LOC QMI SHIM 
      // instead of directly calling the callback in LOC QMI SHIM.
      LOC_MSG_MED ("locWifi_EventCb forwards PDSM_PD_EVENT_WPS_NEEDED request, type=%d",
                  (int) pz_PdInfo->pd_info.qwip_data.request_type, 0, 0);
      pz_WifiReqIndMsg = (qmiLocEventWifiReqIndMsgT_v02 *)loc_calloc(sizeof(*pz_WifiReqIndMsg));

      if(NULL != pz_WifiReqIndMsg)
      {
         pz_WifiReqIndMsg->tbfInMs_valid = TRUE;
         pz_WifiReqIndMsg->tbfInMs = pz_PdInfo->pd_info.qwip_data.tbf_ms;
         pz_WifiReqIndMsg->e911Mode_valid = TRUE;
         pz_WifiReqIndMsg->e911Mode = pz_PdInfo->pd_info.qwip_data.v_is_emergency_mode;
         switch (pz_PdInfo->pd_info.qwip_data.request_type)
         {
            case PDSM_PD_QWIP_START_PERIODIC_HI_FREQ_FIXES:
               pz_WifiReqIndMsg->requestType = 
                                    eQMI_LOC_WIFI_START_PERIODIC_HI_FREQ_FIXES_V02;
              break;
            case PDSM_PD_QWIP_START_PERIODIC_KEEP_WARM:
              pz_WifiReqIndMsg->requestType = eQMI_LOC_WIFI_START_PERIODIC_KEEP_WARM_V02;
              break;
            case PDSM_PD_QWIP_STOP_PERIODIC_FIXES:
              pz_WifiReqIndMsg->requestType = eQMI_LOC_WIFI_STOP_PERIODIC_FIXES_V02;
              break;
            case PDSM_PD_QWIP_SUSPEND:
              pz_WifiReqIndMsg->requestType = eQMI_LOC_WIFI_STOP_PERIODIC_FIXES_V02;
              break;

            default:
            {
              LOC_MSG_ERROR( "locWifi_EventCb invalid request_type: %d",
                              pz_PdInfo->pd_info.qwip_data.request_type,0,0);
              v_RetVal = FALSE;

            }
            break;
         }

         if(TRUE == v_RetVal)
         {
            (void)locMW_SendLocCmdProxy(LOC_HANDLE_REGISTERED_CLIENTS,
                                        LOC_CMD_TYPE_NOTIFY_CLIENT,
                                        LOC_EVENT_WPS_NEEDED_REQUEST,
                                        (void *)pz_WifiReqIndMsg);
         }
         else
         {
            loc_free(pz_WifiReqIndMsg);
         }
      }// end of if(NULL != pz_WifiClntData) 
      else
      {
         LOC_MSG_ERROR("locWifi_EventCb failed at loc_calloc ",
                        0, 0, 0);
      }
   }// end of if (q_PdEvent & PDSM_PD_EVENT_WPS_NEEDED)
}

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC WIFI related requests

@param[in] 
  l_ClientHandle      Loc API client handle
  q_QmiLocMsgId       QMI_LOC Msg Id
  p_QmiLocMsgData     QMI_LOC Msg Data
  q_QmiLocMsgLen      QMI_LOC Msg Length

@retval   TRUE/FALSE    If process is successful or failed
---------------------------------------------------------------------------*/
boolean locWifi_ProcessQmiRequest
(
   loc_client_handle_type l_ClientHandle,
   uint32                 q_QmiLocMsgId,
   const void*            p_QmiLocMsgData,
   uint32                 q_QmiLocMsgLen
)
{
   locWifi_ModuleDataType* pz_LocWifiData = &(loc_middleware_data.z_locWifi_Data);
   uint32                  q_RetVal      = LOC_API_UNSUPPORTED;

   /* Function arguments sanity check */
   if ((NULL == p_QmiLocMsgData) || (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle))
   {
      LOC_MSG_ERROR("locWifi_ProcessQmiRequest: NULL pointer", 0,0,0);
      return FALSE;
   }

   LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.wifi_crit_sec);

   pz_LocWifiData->l_ClientHandle      = l_ClientHandle;
   pz_LocWifiData->q_QmiLocMsgId       = q_QmiLocMsgId;

   LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.wifi_crit_sec);

   switch (q_QmiLocMsgId)
   {
      case QMI_LOC_INJECT_WIFI_POSITION_REQ_V02:
      {
         q_RetVal = locWifi_InjectWifiPos(l_ClientHandle, (qmiLocInjectWifiPositionReqMsgT_v02 *)p_QmiLocMsgData);
         break;
      }

      case QMI_LOC_NOTIFY_WIFI_STATUS_REQ_V02: 
      {
         q_RetVal = locWifi_NotifyWifiStatus(l_ClientHandle, (qmiLocNotifyWifiStatusReqMsgT_v02 *)p_QmiLocMsgData);
         break;
      }

      case QMI_LOC_NOTIFY_WIFI_ATTACHMENT_STATUS_REQ_V02:
      {
         q_RetVal = locWifi_NotifyWifiAttachStatus(l_ClientHandle, (qmiLocNotifyWifiAttachmentStatusReqMsgT_v02 *)p_QmiLocMsgData);
         break;
      }

      case QMI_LOC_NOTIFY_WIFI_ENABLED_STATUS_REQ_V02:
      {
         q_RetVal = locWifi_NotifyWifiEnabledStatus(l_ClientHandle, (qmiLocNotifyWifiEnabledStatusReqMsgT_v02 *)p_QmiLocMsgData);
         break;
      }

      default:
         q_RetVal = LOC_API_UNSUPPORTED;
         break;
   }

   if( LOC_API_SUCCESS == q_RetVal)
   {
      return TRUE;
   }
   else
   {
      LOC_MSG_ERROR("locWifi_ProcessQmiRequest: QMI_LOC 0X%04x returns error=%d", q_QmiLocMsgId, q_RetVal, 0);
      return FALSE;
   }
}

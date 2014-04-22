/******************************************************************************
  @file:  loc_ni.c
  @brief: Location Middleware NI module

  DESCRIPTION
  This module handles the NI Call flows and mediates them to PDAPI.
  
  
  -----------------------------------------------------------------------------
  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  Copyright (c) 2013-2014 QUALCOMM Atheros Incorporated.
  All Rights Reserved. QUALCOMM Atheros Proprietary and Confidential
  Copyright (c) 2015-2018 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
 ******************************************************************************/


/*=============================================================================
$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_ni.c#1 $
$DateTime: 2020/07/21 21:23:44 $
$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  01/16/18     sum corrected typo, clientName.formattedString_len not getting filled.
  06/15/17     yh  crash from locNi_ConvertToQmiNiFormat
  05/25/17     yh  valid_flags bug
  03/31/17     yh  Loc_Ni cleanup
=============================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "loc_qmi_shim.h"
#include "loc_ni.h"
#include "loc_sm_interface.h"
#include "loc_utils.h"

/* Mapping of Data-Coding-Scheme */
static const struct 
{
  qmiLocNiDataCodingSchemeEnumT_v02                     e_QmiNiDataCodingScheme;
  pdsm_lcs_notify_verify_datacoding_scheme_e_type       e_PdapiDataCodingScheme;
}z_NiDataCodingSchemeMap[]=
{
  { eQMI_LOC_NI_SS_GERMAN_V02,                   PDSM_SS_GERMAN                        },

  { eQMI_LOC_NI_SS_ENGLISH_V02,                  PDSM_SS_ENGLISH                       },

  { eQMI_LOC_NI_SS_ITALIAN_V02,                  PDSM_SS_ITALIAN                       },
    
  { eQMI_LOC_NI_SS_FRENCH_V02,                   PDSM_SS_FRENCH                        },
    
  { eQMI_LOC_NI_SS_SPANISH_V02,                  PDSM_SS_SPANISH                       },
  
  { eQMI_LOC_NI_SS_DUTCH_V02,                    PDSM_SS_DUTCH                         },
    
  { eQMI_LOC_NI_SS_SWEDISH_V02,                  PDSM_SS_SWEDISH                       },
  
  { eQMI_LOC_NI_SS_DANISH_V02,                   PDSM_SS_DANISH                        },
  
  { eQMI_LOC_NI_SS_PORTUGUESE_V02,               PDSM_SS_PORTUGUESE                    },
  
  { eQMI_LOC_NI_SS_FINNISH_V02,                  PDSM_SS_FINNISH                       },
  
  { eQMI_LOC_NI_SS_NORWEGIAN_V02,                PDSM_SS_NORWEGIAN                     },

  { eQMI_LOC_NI_SS_GREEK_V02,                    PDSM_SS_GREEK                         },
    
  { eQMI_LOC_NI_SS_TURKISH_V02,                  PDSM_SS_TURKISH                       },
    
  { eQMI_LOC_NI_SS_HUNGARIAN_V02,                PDSM_SS_HUNGARIAN                     },
  
  { eQMI_LOC_NI_SS_POLISH_V02,                   PDSM_SS_POLISH                        },
    
  { eQMI_LOC_NI_SS_LANGUAGE_UNSPEC_V02,          PDSM_SS_LANGUAGE_UNSPEC               },
  
  { eQMI_LOC_NI_SUPL_UTF8_V02,                   PDSM_SUPL_UTF8                        },
  
  { eQMI_LOC_NI_SUPL_UCS2_V02,                   PDSM_SUPL_UCS2                        },
  
  { eQMI_LOC_NI_SUPL_GSM_DEFAULT_V02,            PDSM_SUPL_GSM_DEFAULT                 }

};

/* Mapping of SUPL format */
static const struct
{
   qmiLocNiSuplFormatEnumType_v02          e_QmiNiFormatType;
   pdsm_lcs_supl_format_indicator_e_type   e_PdapiNiFormatType;
}z_NiSuplFormatMap[]=
{
  { eQMI_LOC_NI_SUPL_FORMAT_LOGICAL_NAME_V02,
    PDSM_SUPL_FORMAT_LOGICAL_NAME },

  { eQMI_LOC_NI_SUPL_FORMAT_EMAIL_ADDRESS_V02,
    PDSM_SUPL_FORMAT_EMAIL_ADDRESS },

  { eQMI_LOC_NI_SUPL_FORMAT_MSISDN_V02,
    PDSM_SUPL_FORMAT_MSISDN },

  { eQMI_LOC_NI_SUPL_FORMAT_URL_V02,
    PDSM_SUPL_FORMAT_URL },

  { eQMI_LOC_NI_SUPL_FORMAT_SIP_URL_V02,
    PDSM_SUPL_FORMAT_SIP_URL },

  { eQMI_LOC_NI_SUPL_FORMAT_MIN_V02,
    PDSM_SUPL_FORMAT_MIN },

  { eQMI_LOC_NI_SUPL_FORMAT_MDN_V02,
    PDSM_SUPL_FORMAT_MDN },

  { eQMI_LOC_NI_SUPL_FORMAT_IMSPUBLIC_IDENTITY_V02,
    PDSM_SUPL_FORMAT_IMSPUBLIC_IDENTITY },

  { eQMI_LOC_NI_SUPL_FORMAT_OSS_UNKNOWN_V02,
    PDSM_SUPL_FORMAT_OSS_UNKNOWN }
};



/* local variable to temporary store the notification text for UMTS CP */
static char umts_cp_notification_text[QMI_LOC_NI_MAX_CLIENT_NAME_LENGTH_V02];


/* table mapping the QMI_LOC SUPL 2 GNSS tech masks to PDAPI
   pos tech mask */
static const struct 
{
  qmiLocNiSuplVer2ExtGnssTypeMaskT_v02 qmiLoc_supl_ver2_ext_mask;
  uint32 pdapi_supl_ver2_ext_mask;
}z_NiGnssTypeMap[] =
{
  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_GPS_V02, 
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_GPS },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_GLONASS_V02,
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_GLONASS },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_GALILEO_V02,
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_GALILEO },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_SBAS_V02,
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_SBAS },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_QZSS_V02,
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_QZSS },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_MODERN_GPS_V02,
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_MODERN_GPS },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_BDS_V02,
    PDAPI_SUPLINIT_GNSS_POS_TECH_BITMAP_BDS }
};

/* table mapping the QMI_LOC supported networks masks to PDAPI
   supported networks masks */
static const struct
{
  qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02 qmiLoc_supl_ver2_ext_mask;
  uint32 pdapi_supl_ver2_ext_mask;
}z_NiSupportedNetworkMaskMap[] = 
{
  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WLAN_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_WLAN },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_GSM_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_GSM }, 

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WCDMA_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_WCDMA },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_CDMA_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_CDMA },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_HRDP_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_HRDP },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_UMB_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_UMB },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_LTE_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_LTE },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WIMAX_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_WIMAX },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_HISTORIC_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_HISTORIC },

  { QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_NONSVRV_V02,
    PDAPI_SUPLINIT_SUPPORTED_NTWK_BITMAP_NONSVRV }
};

#define QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_ALL \
    (QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WLAN_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_GSM_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WCDMA_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_CDMA_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_HRDP_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_UMB_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_LTE_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WIMAX_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_HISTORIC_V02 | \
     QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_NONSVRV_V02)

static void locNi_EventCb
(
      void *user_data,
      pdsm_lcs_event_type  l_LcsEvent,
      const pdsm_lcs_info_s_type *lcs_info_ptr
);

static void locNi_CmdCb
(
      void                      *data_block_ptr,
      pdsm_lcs_cmd_e_type        lcs_cmd,
      pdsm_lcs_cmd_err_e_type    lcs_cmd_error
);

// NI module used to process user reponse to the NI request
static int locNi_ProcessUserResponse
(
   loc_client_handle_type                    l_ClientHandle,
   const qmiLocNiUserRespReqMsgT_v02*        pz_NiUserRespReq
);

// Translate V1/V2 NI request from PDAPI to QMI_LOC API
static void locNi_XlateVxReqFromPdapiToQmi
(
   const pdsm_lcs_vx_notify_verify_req_s_type*  pz_PdapiVxReq,
   qmiLocEventNiNotifyVerifyReqIndMsgT_v02*     pz_NiNotifyVerifyReqInd
);

// Translate SUPL NI request from PDAPI to QMI_LOC API
static void locNi_XlateSuplReqFromPdapiToQmi
(
   const pdsm_lcs_supl_notify_verify_req_s_type*  pz_PdapiSuplReq,
   qmiLocEventNiNotifyVerifyReqIndMsgT_v02*       pz_NiNotifyVerifyReqInd
);

// Translate UMTS NI request from PDAPI tO QMI_LOC API
static void locNi_XlateUmtsReqFromPdapiToQmi
(
   const pdsm_lcs_umts_cp_notify_verify_req_s_type*  pz_PdapiUmtsReq,
   qmiLocEventNiNotifyVerifyReqIndMsgT_v02*          pz_NiNotifyVerifyReqInd
);

// Translate V1/V2 NI request from QMI_LOC API to PDAPI
static boolean locNi_XlateVxReqFromQmiToPdapi
(
   const qmiLocNiVxNotifyVerifyStructT_v02*  pz_NiVxPayload,
   pdsm_lcs_vx_notify_verify_req_s_type*     pz_RespInfo
);

// Translate UMTS NI request from QMI_LOC API to PDAPI
static boolean locNi_XlateUmtsCpFromQmiToPdapi
(
   const qmiLocNiUserRespReqMsgT_v02*         pz_NiUserRespReq,
   pdsm_lcs_umts_cp_notify_verify_req_s_type* pz_RespInfo
);

static boolean locNi_XlateSuplPayloadFromQmiToPdapi
(
   const qmiLocNiUserRespReqMsgT_v02*        pz_NiUserRespReq,
   pdsm_lcs_notification_resp_s_type*        pz_RespInfo
);

static boolean locNi_XlateSuplVer2ExtPayloadFromQmiToPdapi
(
   const qmiLocNiUserRespReqMsgT_v02*        pz_NiUserRespReq,
   pdsm_lcs_notification_resp_s_type*        pz_RespInfo
);

static boolean locNi_ConvertToPdapiNotificationType
(
   qmiLocNiNotifyVerifyEnumT_v02    e_QmiNotificationType,
   pdsm_lcs_notify_verify_e_type*   pe_PdapiNotifyVerifyType
);

static boolean locNi_ConvertToQmiNiFormat
(
  pdsm_lcs_supl_format_indicator_e_type  e_PdapiNiFormatType,
  qmiLocNiSuplFormatEnumType_v02*        pe_QmiNiFormatType
);

/*---------------------------------------------------------------------------
@brief
  Function to initialize location NI module
  This function will be called when first location client registers for NI event.

@retval    TRUE   Called successfully
@retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locNi_Init()
{
   boolean                     v_RetVal = FALSE;
   pdsm_client_status_e_type   e_Status = PDSM_CLIENT_OK;

   do
   {
      loc_middleware_data.ni_client_id = pdsm_client_init(PDSM_CLIENT_TYPE_NI);

      if (loc_middleware_data.ni_client_id == -1)
      {
         LOC_MSG_ERROR ( "pdsm_client_init for NI failed", 0, 0, 0);
         break;
      }

      e_Status= pdsm_client_act(loc_middleware_data.ni_client_id);

      if(e_Status!= PDSM_CLIENT_OK)
      {
         LOC_MSG_ERROR ("pdsm_client_act failed, error code =%d", v_RetVal, 0, 0);
         break;
      }

      e_Status= pdsm_client_lcs_reg(loc_middleware_data.ni_client_id,
                                    NULL,
                                    locNi_EventCb,
                                    PDSM_CLIENT_EVENT_REG,
                                    PDSM_LCS_EVENT_MASK,
                                    NULL
                                    );

      if(e_Status!= PDSM_CLIENT_OK)
      {
         LOC_MSG_ERROR ("pdsm_client_lcs_reg failed, error code = %d", v_RetVal, 0, 0);
         break;
      }

      v_RetVal = TRUE;
   } while (0);

   // NI module data has been zero-initialized in the loc_task, no more special initialization is needed

   LOC_MSG_HIGH ("locNi_Init returns =%d", v_RetVal, 0, 0);

   return v_RetVal;
}

// Route user response for NI request to modem
static int locNi_ProcessUserResponse
(
   loc_client_handle_type                    l_ClientHandle,
   const qmiLocNiUserRespReqMsgT_v02*        pz_NiUserRespReq
)
{
   boolean                             v_RetVal = TRUE;
   pdsm_lcs_notification_resp_s_type   *pz_RespInfo = NULL;

   pz_RespInfo = (pdsm_lcs_notification_resp_s_type *)loc_calloc(sizeof(*pz_RespInfo));
   if(NULL == pz_RespInfo)
   {
      LOC_MSG_ERROR ("locNi_ProcessUserResponse no memory", 0, 0, 0);
      return LOC_API_GENERAL_FAILURE;
   }

   /* Input parameter check are done in locNi_ProcessQmiRequest */
   switch(pz_NiUserRespReq->userResp)
   {
      case eQMI_LOC_NI_LCS_NOTIFY_VERIFY_ACCEPT_V02:
          pz_RespInfo->resp = PDSM_LCS_NOTIFY_VERIFY_ACCEPT;
      break;

      case eQMI_LOC_NI_LCS_NOTIFY_VERIFY_DENY_V02:
         pz_RespInfo->resp = PDSM_LCS_NOTIFY_VERIFY_DENY;
      break;

      case eQMI_LOC_NI_LCS_NOTIFY_VERIFY_NORESP_V02:
         pz_RespInfo->resp = PDSM_LCS_NOTIFY_VERIFY_NORESP;
      break;

      default:
         pz_RespInfo->resp = PDSM_LCS_NOTIFY_VERIFY_NORESP;
         v_RetVal = FALSE;
      break;
   }

   if(TRUE == pz_NiUserRespReq->NiVxPayload_valid)
   {
      pz_RespInfo->lcs_info_cmd_type = PDSM_LCS_CMD_TYPE_VX_REQ;
      /* do notification type convert here on purpose */
      v_RetVal = locNi_ConvertToPdapiNotificationType(pz_NiUserRespReq->notificationType,
                                 &(pz_RespInfo->lcs_info.vx_req.notification_priv_type));
      if(TRUE == v_RetVal)
      {
         v_RetVal = locNi_XlateVxReqFromQmiToPdapi (&(pz_NiUserRespReq->NiVxPayload),
                                      &(pz_RespInfo->lcs_info.vx_req));
      }
      else
      {
         LOC_MSG_ERROR("locNi_ProcessUserResponse NiVxPayload notificationType convert error", 0, 0, 0);
      }
   }
   else if(TRUE == pz_NiUserRespReq->NiSuplPayload_valid)
   {
      pz_RespInfo->lcs_info_cmd_type = PDSM_LCS_CMD_TYPE_SUPL_REQ;
      v_RetVal = locNi_XlateSuplPayloadFromQmiToPdapi (pz_NiUserRespReq,
                                      pz_RespInfo);
   }
   else if(TRUE == pz_NiUserRespReq->NiUmtsCpPayload_valid)
   {
      pz_RespInfo->lcs_info_cmd_type = PDSM_LCS_CMD_TYPE_UMTS_CP_REQ;
      v_RetVal = locNi_XlateUmtsCpFromQmiToPdapi (pz_NiUserRespReq,
                                      &(pz_RespInfo->lcs_info.umts_cp_req));
   }
   else if(TRUE == pz_NiUserRespReq->NiVxServiceInteractionPayload_valid)
   {
      pz_RespInfo->lcs_info_cmd_type = PDSM_LCS_CMD_TYPE_VX_SERVICE_INTERACTION;
      /* do notification type convert here on purpose */
      v_RetVal = locNi_ConvertToPdapiNotificationType(pz_NiUserRespReq->notificationType,
                                 &(pz_RespInfo->lcs_info.vx_req.notification_priv_type));
      if(TRUE == v_RetVal)
      {
         v_RetVal = locNi_XlateVxReqFromQmiToPdapi (&(pz_NiUserRespReq->NiVxServiceInteractionPayload.niVxReq),
                                      &(pz_RespInfo->lcs_info.vx_req));
      }
      else
      {
         LOC_MSG_ERROR("locNi_ProcessUserResponse NiVxServiceInteractionPayload notificationType convert error", 0, 0, 0);
      }

   }

   if (v_RetVal == TRUE) 
   {
      // Change summary Service interaction for SUPL MT PO and SUPL MT
      // SUPL MT PO:
      // 1: loc mw need to cancel the on-going MO session to allow SUPL MT PO to proceed. 
      // 2: If a SUPL MT is in progress, this will be killed by LSM
      // 3: LSM layer will make sure that in progress SUPL MT PO can not be killed
      //    by other incoming SUPL MT session or MO sessions   
      loc_pd_handle_service_interaction (pz_RespInfo);

      v_RetVal = pdsm_lcs_agent_client_response(locNi_CmdCb,
                                               (void *)l_ClientHandle,
                                               pz_RespInfo,
                                               loc_middleware_data.ni_client_id
                                               );

      LOC_MSG_MED ("pdsm_lcs_agent_client_response returned %d", v_RetVal, 0, 0);
   }

   loc_free(pz_RespInfo);

   if (v_RetVal == TRUE)
   {
      return LOC_API_SUCCESS;
   }
   else
   {
      return LOC_API_GENERAL_FAILURE;
   }
}

// NI event cb function: this is where the NI notify/verify request comes in
// Process the NI event (NI notification and verification event)
static void locNi_EventCb
(
   void *user_data,
   pdsm_lcs_event_type  l_LcsEvent,
   const pdsm_lcs_info_s_type *lcs_info_ptr
)
{
   boolean                             v_Ret = FALSE, v_SendLocCmd = FALSE;
   pdsm_lcs_notification_resp_s_type*  pz_LcsResp = NULL;
   loc_client_handle_type              l_ClientHandle = LOC_HANDLE_REGISTERED_CLIENTS;
   LocMW_LocCmdEnumT                   e_LocCmdType;
   qmiLocEventNiNotifyVerifyReqIndMsgT_v02*    pz_NiNotifyVerifyReqIndMsg = NULL;
   void*                               p_LocCmdData = NULL;

   LOC_MSG_HIGH ("locNi_EventCb, l_LcsEvent = 0x%x ", l_LcsEvent, 0, 0);

   // Save the ni module data, to be used in following service interaction
   loc_middleware_data.z_locNi_Data.l_LcsEvent = l_LcsEvent;
   loc_middleware_data.z_locNi_Data.y_LcsInfo = lcs_info_ptr->lcs_info;
   do
   {
      e_LocCmdType = LOC_CMD_TYPE_NOTIFY_CLIENT;
      if( (l_LcsEvent == PDSM_LCS_EVENT_VX_NOTIFY_VERIFY_REQ) ||
          (l_LcsEvent == PDSM_LCS_EVENT_SUPL_NOTIFY_VERIFY_REQ) ||
          (l_LcsEvent == PDSM_LCS_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ) ||
          (l_LcsEvent == PDSM_LCS_VX_EVENT_USER_NOTIFY_SERVICE_INTERACTION) )
      {
         pz_NiNotifyVerifyReqIndMsg = (qmiLocEventNiNotifyVerifyReqIndMsgT_v02 *)loc_calloc(sizeof(*pz_NiNotifyVerifyReqIndMsg));
         if(NULL == pz_NiNotifyVerifyReqIndMsg)
         {
            // memory allocation error
            LOC_MSG_ERROR("locNi_EventCb, loc_calloc failed ", 0, 0, 0);
            v_SendLocCmd = FALSE;
            break;
         }

         p_LocCmdData = (void *)pz_NiNotifyVerifyReqIndMsg;
         if (l_LcsEvent == PDSM_LCS_EVENT_VX_NOTIFY_VERIFY_REQ)
         {
            locNi_XlateVxReqFromPdapiToQmi (&(lcs_info_ptr->lcs_info.vx_req),
                                             pz_NiNotifyVerifyReqIndMsg);
            v_SendLocCmd = TRUE;
         }
         else if (l_LcsEvent == PDSM_LCS_EVENT_SUPL_NOTIFY_VERIFY_REQ)
         {
            loc_supl_req_e_type  supl_result = LOC_SUPL_REQ_DENY;
            locNi_XlateSuplReqFromPdapiToQmi(&(lcs_info_ptr->lcs_info.supl_req),
                                             pz_NiNotifyVerifyReqIndMsg);

            supl_result = loc_pd_continue_processing_supl_request(&(lcs_info_ptr->lcs_info.supl_req));
            if(LOC_SUPL_REQ_DENY == supl_result)
            {
               // no need to continue processing this SUPL request
               // just ignore the whole event
               LOC_MSG_HIGH ("locNi_EventCb, silently deny SUPL NI request", 0, 0, 0);

               // instead of notifying the client, let's send an event to ourselves and deny it 
               // (switching thread context to avoid calling PDSM in a callback context, which could lead to deadlock) 
               pz_LcsResp = (pdsm_lcs_notification_resp_s_type *)loc_calloc(sizeof(pdsm_lcs_notification_resp_s_type));
               if(NULL != pz_LcsResp)
               {
                  // change command type from notifying the user to sending message back to SM
                  // just for this case
                  e_LocCmdType = LOC_CMD_TYPE_SILENTLY_DENY_NI_REQUEST;
                  p_LocCmdData = (void *)pz_LcsResp;

                  pz_LcsResp->resp = PDSM_LCS_NOTIFY_VERIFY_DENY;
                  pz_LcsResp->lcs_info_cmd_type = lcs_info_ptr->lcs_cb_cmd_type;
                  pz_LcsResp->lcs_info = lcs_info_ptr->lcs_info;

                  v_SendLocCmd = TRUE;
                }
                else
                {
                    // we are unable to reject the request, so SM probably will accept it by default after timeout
                    // there is nothing we can do
                    // memory allocation error
                }
            }
            else if(LOC_SUPL_REQ_ACCEPT == supl_result)
            {
               LOC_MSG_HIGH ("locNi_EventCb, continue processing SUPL NI event", 0, 0, 0);
               v_SendLocCmd = TRUE;
            }
            else
            {
               // LOC_SUPL_REQ_DROP, drop second SUPL silently
               LOC_MSG_HIGH ("locNi_EventCb, continue SUPL result = %d, drop SUPL", supl_result, 0, 0);
               v_SendLocCmd = FALSE;
            }
         }
         else if (l_LcsEvent == PDSM_LCS_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ)
         {
            locNi_XlateUmtsReqFromPdapiToQmi(&(lcs_info_ptr->lcs_info.umts_cp_req),
                                             pz_NiNotifyVerifyReqIndMsg);

            v_SendLocCmd = TRUE;
         }
         else if (l_LcsEvent == PDSM_LCS_VX_EVENT_USER_NOTIFY_SERVICE_INTERACTION)
         {
            // In current LSM, the on-going vx ni event is not popuated properly, so we use the previously saved info
            if (loc_middleware_data.z_locNi_Data.l_LcsEvent == PDSM_LCS_EVENT_VX_NOTIFY_VERIFY_REQ)
            {
               locNi_XlateVxReqFromPdapiToQmi ( &(loc_middleware_data.z_locNi_Data.y_LcsInfo.vx_req),
                                                pz_NiNotifyVerifyReqIndMsg);
            }

            // Now copy the info that are being populated properly in the event, this includes the notification/verification type and user response timeout
            switch(lcs_info_ptr->lcs_info.vx_req.notification_priv_type)
            {
               case PDSM_LCS_USER_NOTIFY_ONLY:
                  pz_NiNotifyVerifyReqIndMsg->notificationType = eQMI_LOC_NI_USER_NOTIFY_ONLY_V02;
               break;

               case PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
                  pz_NiNotifyVerifyReqIndMsg->notificationType = eQMI_LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP_V02;
               break;

               case PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP:
                  pz_NiNotifyVerifyReqIndMsg->notificationType = eQMI_LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP_V02;
               break;

               default:
                  pz_NiNotifyVerifyReqIndMsg->notificationType = eQMI_LOC_NI_USER_NOTIFY_ONLY_V02;
               break;
            }
            pz_NiNotifyVerifyReqIndMsg->NiVxInd.userRespTimerInSeconds
               = lcs_info_ptr->lcs_info.vx_req.user_resp_timer_val;

            v_SendLocCmd = TRUE;
         }
      }
      /* In both cases, relay the vx-Config to the client, 
         The client ( QMI-LOC ) can verify if it the operation succeeded,
         Note: Had to do this as this is a limitation on PDAPI */
      else if( l_LcsEvent == PDSM_LCS_EVENT_CLIENT_VX_CONFIG )
      {
         qmiLocGetProtocolConfigParametersIndMsgT_v02* pz_GetProtocolConfigParametersInd = 
            (qmiLocGetProtocolConfigParametersIndMsgT_v02 *)loc_calloc(sizeof(*pz_GetProtocolConfigParametersInd));
         uint32   q_RetVal = LOC_API_GENERAL_FAILURE;
         if (NULL == pz_GetProtocolConfigParametersInd)
         {
            LOC_MSG_ERROR("locNi_EventCb: calloc return NULL", 0, 0, 0);
         }
         else
         {
            if (l_LcsEvent == PDSM_LCS_EVENT_CLIENT_VX_CONFIG)
            {
               pdsm_lcs_agent_version_e_type e_VxVersion = lcs_info_ptr->lcs_info.vx_config.vxVersion;

               switch ( e_VxVersion)
               {
               case PDSM_LCS_AGENT_VERSION_V1_ONLY :
                  pz_GetProtocolConfigParametersInd->vxVersion = eQMI_LOC_VX_VERSION_V1_ONLY_V02;
                  pz_GetProtocolConfigParametersInd->vxVersion_valid = TRUE;
                  q_RetVal = LOC_API_SUCCESS;
                  break;
               case PDSM_LCS_AGENT_VERSION_V2_ONLY:
                  pz_GetProtocolConfigParametersInd->vxVersion = eQMI_LOC_VX_VERSION_V2_ONLY_V02;
                  pz_GetProtocolConfigParametersInd->vxVersion_valid = TRUE;
                  q_RetVal = LOC_API_SUCCESS;
                  break;
               default:
                  LOC_MSG_ERROR ( "could not translate vxVersion %d", 
                                 e_VxVersion, 0, 0);
                  break;
               }
            }
            else
            {
               /* There is no status, We are gauranteed to get a callback,
                  if we get a callback, we pass the value that is returned,
                  and hence reporting a success */
               q_RetVal = LOC_API_SUCCESS;
            }
            v_Ret = locMW_SendQmiIndicationProxy((loc_client_handle_type) user_data,
                                       QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_IND_V02,
                                       pz_GetProtocolConfigParametersInd, q_RetVal);
            /* This is a special call flow, 
               get VX version triaged by loc_pa then passed to loc_ni */
            locPa_ResetModule();
         }
      }
      else if ( l_LcsEvent == PDSM_LCS_EVENT_CLIENT_SET_VX_CONFIG )
      {
         qmiLocSetProtocolConfigParametersIndMsgT_v02* pz_SetProtocolConfigParametersInd = 
            (qmiLocSetProtocolConfigParametersIndMsgT_v02 *)loc_calloc(sizeof(*pz_SetProtocolConfigParametersInd));

         if (NULL == pz_SetProtocolConfigParametersInd)
         {
            LOC_MSG_ERROR("locNi_EventCb: calloc return NULL", 0, 0, 0);
         }
         else
         {
            v_Ret = locMW_SendQmiIndicationProxy((loc_client_handle_type) user_data,
                                       QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_IND_V02,
                                       pz_SetProtocolConfigParametersInd, LOC_API_SUCCESS);
            /* This is a special call flow, 
               set VX version triaged by loc_pa then passed to loc_ni */
            locPa_ResetModule();
         }
      }
      else
      {
         LOC_MSG_ERROR ("locNi_EventCb, unrecognized l_LcsEvent = %d ", l_LcsEvent, 0, 0);
         // Exit out of the loop
         break;
      }

      // Queue the cmd to notify the client who registers for those NI events
      if(TRUE == v_SendLocCmd)
      {
         v_Ret = locMW_SendLocCmdProxy(
                     LOC_HANDLE_REGISTERED_CLIENTS,
                     e_LocCmdType,
                     LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST,
                     p_LocCmdData);
      }
   }while (0);

   if(TRUE != v_Ret)
   {
      LOC_MSG_ERROR ("locNi_EventCb, error", 0, 0, 0);
   }
}

// NI cmd callback
static void locNi_CmdCb ( void                      *data_block_ptr,
                           pdsm_lcs_cmd_e_type        lcs_cmd,
                           pdsm_lcs_cmd_err_e_type    lcs_cmd_error)
{
   LOC_MSG_HIGH ("locNi_CmdCb, cmd %d, cmd_err %d", lcs_cmd, lcs_cmd_error, 0);

   if (lcs_cmd == PDSM_LCS_CMD_NOTIFY_VERIFY_RESP)
   {
      if (lcs_cmd_error != PDSM_LCS_CMD_ERR_NOERR)
      {
         locMW_SendQmiIndicationProxy((loc_client_handle_type) data_block_ptr,
                                     QMI_LOC_NI_USER_RESPONSE_IND_V02,
                                     NULL, LOC_API_GENERAL_FAILURE);
      }
      else
      {
         locMW_SendQmiIndicationProxy((loc_client_handle_type) data_block_ptr,
                                     QMI_LOC_NI_USER_RESPONSE_IND_V02,
                                     NULL,LOC_API_SUCCESS);
      }
   }
}

// Translate V1/V2 NI request from PDAPI to QMI_LOC API
static void locNi_XlateVxReqFromPdapiToQmi
(
   const pdsm_lcs_vx_notify_verify_req_s_type*  pz_PdapiVxReq,
   qmiLocEventNiNotifyVerifyReqIndMsgT_v02*     pz_NiNotifyVerifyReqInd
)
{
   /* Input parameter sanity check is done in LocNi_EventCb */
   qmiLocNiVxNotifyVerifyStructT_v02* pz_NiVxInd = &(pz_NiNotifyVerifyReqInd->NiVxInd);

   pz_NiNotifyVerifyReqInd->NiVxInd_valid = TRUE;
   switch(pz_PdapiVxReq->notification_priv_type)
   {
      case PDSM_LCS_USER_NOTIFY_ONLY:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NOTIFY_ONLY_V02;
      break;

      case PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP_V02;
      break;

      case PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP_V02;
      break;

      default:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NOTIFY_ONLY_V02;
      break;
   }

   pz_NiVxInd->posQosIncl = pz_PdapiVxReq->pos_qos_incl;
   pz_NiVxInd->posQos     = pz_PdapiVxReq->pos_qos;
   pz_NiVxInd->numFixes   = pz_PdapiVxReq->num_fixes;
   pz_NiVxInd->timeBetweenFixes  = pz_PdapiVxReq->tbf;

   switch(pz_PdapiVxReq->pos_mode)
   {
     case PDSM_LCS_MS_ASSISTED_ONLY:
         pz_NiVxInd->posMode = eQMI_LOC_NI_VX_MS_ASSISTED_ONLY_V02;
     break;

     case PDSM_LCS_MS_BASED_ONLY:
         pz_NiVxInd->posMode = eQMI_LOC_NI_VX_MS_BASED_ONLY_V02;
     break;

     case PDSM_LCS_MS_ASSISTED_PREF_MSBASED_ALLWD:
         pz_NiVxInd->posMode = eQMI_LOC_NI_VX_MS_ASSISTED_PREFERRED_MS_BASED_ALLOWED_V02;
     break;

     case PDSM_LCS_MS_BASED_PREF_ASSISTED_ALLWD:
         pz_NiVxInd->posMode = eQMI_LOC_NI_VX_MS_BASED_PREFERRED_MS_ASSISTED_ALLOWED_V02;
     break;

     default:
         pz_NiVxInd->posMode = eQMI_LOC_NI_VX_MS_ASSISTED_ONLY_V02;
     break;
   }

   switch(pz_PdapiVxReq->encoding_scheme)
   {
      case PDSM_VX_OCTET:
         pz_NiVxInd->encodingScheme = eQMI_LOC_NI_VX_OCTET_V02;
      break;
      PDSM_VX_EXN_PROTOCOL_MSG:
         pz_NiVxInd->encodingScheme = eQMI_LOC_NI_VX_EXN_PROTOCOL_MSG_V02;
      break;
      PDSM_VX_ASCII:
         pz_NiVxInd->encodingScheme = eQMI_LOC_NI_VX_ASCII_V02;
      break;
      PDSM_VX_IA5:
         pz_NiVxInd->encodingScheme = eQMI_LOC_NI_VX_IA5_V02;
      break;
      PDSM_VX_UNICODE:
         pz_NiVxInd->encodingScheme = eQMI_LOC_NI_VX_UNICODE_V02;
      break;
      PDSM_VX_SHIFT_JIS:
         pz_NiVxInd->encodingScheme = eQMI_LOC_NI_VX_SHIFT_JIS_V02;
      break;
      PDSM_VX_KOREAN:
         pz_NiVxInd->encodingScheme = eQMI_LOC_NI_VX_KOREAN_V02;
      break;
      PDSM_VX_LATIN_HEBREW:
         pz_NiVxInd->encodingScheme = eQMI_LOC_NI_VX_LATIN_HEBREW_V02;
      break;
      PDSM_VX_LATIN:
         pz_NiVxInd->encodingScheme = eQMI_LOC_NI_VX_LATIN_V02;
      break;
      PDSM_VX_GSM:
         pz_NiVxInd->encodingScheme = eQMI_LOC_NI_VX_GSM_V02;
      break;
      default:
         pz_NiVxInd->encodingScheme = eQMI_LOC_NI_VX_OCTET_V02;
      break;
   }

   pz_NiVxInd->requestorId_len = 
      pz_PdapiVxReq->requester_id.requester_id_length;

   memscpy(pz_NiVxInd->requestorId,
          sizeof(pz_NiVxInd->requestorId),
          pz_PdapiVxReq->requester_id.requester_id,
          sizeof(pz_PdapiVxReq->requester_id.requester_id));

   pz_NiVxInd->userRespTimerInSeconds =
      pz_PdapiVxReq->user_resp_timer_val;
}

static boolean locNi_ConvertToQmiNiFormat
(
  pdsm_lcs_supl_format_indicator_e_type  e_PdapiNiFormatType,
  qmiLocNiSuplFormatEnumType_v02*        pe_QmiNiFormatType
)
{
   boolean v_RetVal = FALSE;
   uint32  q_ctr = 0;
   uint32  q_size = (sizeof(z_NiSuplFormatMap))/sizeof(z_NiSuplFormatMap[0]);

   if(NULL == pe_QmiNiFormatType)
   {
      LOC_MSG_ERROR("locNi_ConvertToQmiNiFormat NULL pointer", 0, 0, 0);
      return v_RetVal;
   }

  /* Not doing check of inputs as this is called from within the module,
     and this will be called with proper parameters */
   for(; q_ctr < q_size; q_ctr++)
   {
      if(z_NiSuplFormatMap[q_ctr].e_PdapiNiFormatType == e_PdapiNiFormatType)
      {
         LOC_MSG_LOW ("locNi_ConvertToQmiNiFormat q_ctr %d PDAPI %d QMI %d ",
                         q_ctr, e_PdapiNiFormatType,
                         z_NiSuplFormatMap[q_ctr].e_QmiNiFormatType);
         *pe_QmiNiFormatType = z_NiSuplFormatMap[q_ctr].e_QmiNiFormatType;
         v_RetVal = TRUE;
         break;
      }

   }

   return v_RetVal;
}

static boolean locNi_ConvertToQmiDataCodingScheme
(
   pdsm_lcs_notify_verify_datacoding_scheme_e_type   e_PdapiDataCodingScheme,
   qmiLocNiDataCodingSchemeEnumT_v02*    pe_QmiNiDataCodingScheme
)
{
   boolean v_RetVal = FALSE;
   uint32  q_ctr = 0;
   uint32  q_size = (sizeof(z_NiDataCodingSchemeMap))/sizeof(z_NiDataCodingSchemeMap[0]);

   if(NULL == pe_QmiNiDataCodingScheme)
   {
      LOC_MSG_ERROR("locNi_ConvertToQmiDataCodingScheme NULL pointer", 0, 0, 0);
      return v_RetVal;
   }

   /* Input parameter sanity check is done in LocNi_EventCb */
   for(; q_ctr < q_size; q_ctr++)
   {
      if(z_NiDataCodingSchemeMap[q_ctr].e_PdapiDataCodingScheme == e_PdapiDataCodingScheme)
      {
         LOC_MSG_LOW ("locNi_ConvertToQmiDataCodingScheme q_ctr %d PDAPI %d QMI %d ",
                         q_ctr, e_PdapiDataCodingScheme,
                         z_NiDataCodingSchemeMap[q_ctr].e_QmiNiDataCodingScheme);
         *pe_QmiNiDataCodingScheme = z_NiDataCodingSchemeMap[q_ctr].e_QmiNiDataCodingScheme;
         v_RetVal = TRUE;
         break;
      }

   }

   return v_RetVal;
}

//Translate SUPL 2 extensions from PDAPI to QMI_LOC
static boolean locNi_XlateSuplVer2FromPdapiToQmi
(
   const pdsm_lcs_supl_suplinit_ver2_ext_s_type*  pz_PdapiSuplVer2,
   qmiLocEventNiNotifyVerifyReqIndMsgT_v02*       pz_NiNotifyVerifyReqInd
)
{
   uint32   q_idx = 0;
   uint32   q_size = 0;

   /* Input parameter sanity check is done in LocNi_EventCb */
   qmiLocNiSuplVer2ExtStructT_v02*     pz_NiSuplVer2ExtInd = &(pz_NiNotifyVerifyReqInd->NiSuplVer2ExtInd);
   pz_NiNotifyVerifyReqInd->NiSuplVer2ExtInd_valid = TRUE;

   /* convert the supported network masks */
   q_size = sizeof(z_NiSupportedNetworkMaskMap) / 
      sizeof(z_NiSupportedNetworkMaskMap[0]);

   /* if the supported networks validity bit is valid,
     copy the mask from PDAPI structure
     else indicate support for the defined networks */
   if(pz_PdapiSuplVer2->validity &   
      PDAPI_SUPLINIT_VER2_EXT_BITMAP_SUPTDNTWK_VALID)
   {
      for( q_idx = 0; q_idx < q_size; q_idx++ )
      {
         if( pz_PdapiSuplVer2->supl_supported_ntwkinfo & 
               z_NiSupportedNetworkMaskMap[q_idx].pdapi_supl_ver2_ext_mask )
         {
            pz_NiSuplVer2ExtInd->supportedNetworksMask |= 
               z_NiSupportedNetworkMaskMap[q_idx].qmiLoc_supl_ver2_ext_mask;
         }
      }
   }
   else
   {
      pz_NiSuplVer2ExtInd->supportedNetworksMask = 
      QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_ALL;
   }

   /* if trigger type validity bit is set  then copy the information
     otherwise set the default trigger type of single shot*/
   if(pz_PdapiSuplVer2->validity & PDAPI_SUPLINIT_VER2_EXT_BITMAP_TRIGTYPE_VALID )
   {
      switch(pz_PdapiSuplVer2->trigger_type)
      {
         case PDAPI_SUPLINIT_TRIGGER_TYPE_PERIODIC:
         {
            pz_NiSuplVer2ExtInd->triggerType = 
               eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_PERIODIC_V02;
            break;
         }

         case PDAPI_SUPLINIT_TRIGGER_TYPE_AREA_EVENT:
         {
            pz_NiSuplVer2ExtInd->triggerType = 
               eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_AREA_EVENT_V02;
            break;
         }

         default:
         {
            LOC_MSG_ERROR("copy_supl_ver_2_ext_from_pdsm: Unknown trigger type %d \n",
                           pz_PdapiSuplVer2->trigger_type, 0, 0);
            return FALSE;
         }
      }
   }
   else
   {
      pz_NiSuplVer2ExtInd->triggerType = 
         eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_SINGLE_SHOT_V02;
   }

   /* convert the gnss type masks */
   q_size = sizeof(z_NiGnssTypeMap) / 
      sizeof(z_NiGnssTypeMap[0]);

   /* if the gnss pos tech validity bit is valid,
      copy the mask from PDAPI structure
      else indicate support for GPS only  */

   if(pz_PdapiSuplVer2->validity & 
      PDAPI_SUPLINIT_VER2_EXT_BITMAP_GNSSPOSTECH_VALID)
   {
      for( q_idx = 0; q_idx < q_size; q_idx++ )
      {
         if( pz_PdapiSuplVer2->gnss_pos_tech & 
               z_NiGnssTypeMap[q_idx].pdapi_supl_ver2_ext_mask )
         {
            pz_NiSuplVer2ExtInd->gnssType |= 
               z_NiGnssTypeMap[q_idx].qmiLoc_supl_ver2_ext_mask;
         }
      }
   }
   else
   {
      pz_NiSuplVer2ExtInd->gnssType |= 
      QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_GPS_V02;
   }

   return TRUE;
}

// Translate SUPL NI request from PDAPI to QMI_LOC API
static void locNi_XlateSuplReqFromPdapiToQmi
(
   const pdsm_lcs_supl_notify_verify_req_s_type*  pz_PdapiSuplReq,
   qmiLocEventNiNotifyVerifyReqIndMsgT_v02*       pz_NiNotifyVerifyReqInd
)
{
   /* Input parameter sanity check is done in LocNi_EventCb */
   qmiLocNiSuplNotifyVerifyStructT_v02* pz_NiSuplInd = &(pz_NiNotifyVerifyReqInd->NiSuplInd);
   pz_NiNotifyVerifyReqInd->NiSuplInd_valid = TRUE;

   pz_NiSuplInd->valid_flags = 0;
   switch(pz_PdapiSuplReq->notification_priv_type)
   {
      case PDSM_LCS_NO_NOTIFY_NO_VERIFY:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NO_NOTIFY_NO_VERIFY_V02;
      break;

      case PDSM_LCS_USER_NOTIFY_ONLY:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NOTIFY_ONLY_V02;
      break;

      case PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP_V02;
      break;

      case PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP_V02;
      break;

      case PDSM_LCS_PRIVACY_OVERRIDE:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NOTIFY_VERIFY_PRIVACY_OVERRIDE_V02;
      break;

      default:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NO_NOTIFY_NO_VERIFY_V02;
      break;
   }

   if(pz_PdapiSuplReq->supl_slp_session_id.presence)
   {
      pz_NiSuplInd->valid_flags |= QMI_LOC_SUPL_SESSION_ID_MASK_V02;
      pz_NiSuplInd->valid_flags |= QMI_LOC_SUPL_SERVER_INFO_MASK_V02;

      pz_NiSuplInd->suplServerInfo.suplServerAddrTypeMask = 0;
      memscpy(pz_NiSuplInd->suplSessionId,
               sizeof(pz_NiSuplInd->suplSessionId),
               pz_PdapiSuplReq->supl_slp_session_id.session_id,
               sizeof(pz_PdapiSuplReq->supl_slp_session_id.session_id));
      if(pz_PdapiSuplReq->supl_slp_session_id.slp_address.choice == PDSM_SERVER_TYPE_IPV4_ADDRESS)
      {
         pz_NiSuplInd->suplServerInfo.suplServerAddrTypeMask |= 
                        QMI_LOC_SERVER_ADDR_TYPE_IPV4_MASK_V02;
         pz_NiSuplInd->suplServerInfo.ipv4Addr.addr =
            pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs;
         pz_NiSuplInd->suplServerInfo.ipv4Addr.port =
            (uint16) pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id;
      }
      else if(pz_PdapiSuplReq->supl_slp_session_id.slp_address.choice == PDSM_SERVER_TYPE_IPV6_ADDRESS)
      {
         pz_NiSuplInd->suplServerInfo.suplServerAddrTypeMask |= 
                        QMI_LOC_SERVER_ADDR_TYPE_IPV6_MASK_V02;
         memscpy(pz_NiSuplInd->suplServerInfo.ipv6Addr.addr,
                 sizeof(pz_NiSuplInd->suplServerInfo.ipv6Addr.addr),
                 pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs,
                 sizeof(pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs));
         pz_NiSuplInd->suplServerInfo.ipv6Addr.port =
            (uint16) pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id;
      }
      else if(pz_PdapiSuplReq->supl_slp_session_id.slp_address.choice == PDSM_SERVER_TYPE_URL_ADDRESS)
      {
         // note: it's not clear if url_length can be safely used as indicator of how many bytes should be copied
         memscpy(pz_NiSuplInd->suplServerInfo.urlAddr,
               sizeof(pz_NiSuplInd->suplServerInfo.urlAddr),
               pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url,
               sizeof(pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url));
         pz_NiSuplInd->suplServerInfo.suplServerAddrTypeMask |= 
                  QMI_LOC_SERVER_ADDR_TYPE_URL_MASK_V02;
      }
   }

   pz_NiSuplInd->valid_flags |= QMI_LOC_SUPL_HASH_MASK_V02;
   memscpy(pz_NiSuplInd->suplHash, sizeof(pz_NiSuplInd->suplHash),
           pz_PdapiSuplReq->supl_hash, sizeof(pz_PdapiSuplReq->supl_hash));

   switch ( pz_PdapiSuplReq->pos_method )
   {
      case POSMETHOD_AGPS_SETASSISTED:
         pz_NiSuplInd->posMethod = eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETASSISTED_V02;
      break;

      case POSMETHOD_AGPS_SETBASED:
         pz_NiSuplInd->posMethod = eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETBASED_V02;
         break;

      case POSMETHOD_AGPS_SETASSISTED_PREF:
         pz_NiSuplInd->posMethod = eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETASSISTED_PREF_V02;
         break;

      case POSMETHOD_AGPS_SETBASED_PREF:
         pz_NiSuplInd->posMethod = eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETBASED_PREF_V02;
         break;

      case POSMETHOD_AUTONOMOUS_GPS:
         pz_NiSuplInd->posMethod = eQMI_LOC_NI_SUPL_POSMETHOD_AUTONOMOUS_GPS_V02;
         break;

      case POSMETHOD_AFLT:
         pz_NiSuplInd->posMethod = eQMI_LOC_NI_SUPL_POSMETHOD_AFLT_V02;
         break;

      case POSMETHOD_ECID:
         pz_NiSuplInd->posMethod = eQMI_LOC_NI_SUPL_POSMETHOD_ECID_V02;
         break;

      case POSMETHOD_EOTD:
         pz_NiSuplInd->posMethod = eQMI_LOC_NI_SUPL_POSMETHOD_EOTD_V02;
         break;

      case POSMETHOD_OTDOA:
         pz_NiSuplInd->posMethod = eQMI_LOC_NI_SUPL_POSMETHOD_OTDOA_V02;
         break;

      case POSMETHOD_NO_POSITION:
         pz_NiSuplInd->posMethod = eQMI_LOC_NI_SUPL_POSMETHOD_NO_POSITION_V02;
         break;

      default:
         LOC_MSG_ERROR("locNi_XlateSuplReqFromPdapiToQmi invalid pos method: %d", 
                        pz_PdapiSuplReq->pos_method, 0, 0);
         break;
   }
   if (0 != pz_NiSuplInd->posMethod)
   {
      pz_NiSuplInd->valid_flags |= QMI_LOC_SUPL_POS_METHOD_MASK_V02;
   }
   LOC_MSG_LOW("locNi_XlateSuplReqFromPdapiToQmi pdapi pos method %d, QMI pos method %d",
                  pz_PdapiSuplReq->pos_method, pz_NiSuplInd->posMethod, 0);

   if( TRUE == locNi_ConvertToQmiDataCodingScheme(
      pz_PdapiSuplReq->datacoding_scheme,
      &pz_NiSuplInd->dataCodingScheme))
   {
      pz_NiSuplInd->valid_flags |= QMI_LOC_SUPL_DATA_CODING_SCHEME_MASK_V02;
   }

   LOC_MSG_LOW("locNi_XlateSuplReqFromPdapiToQmi: requestorId len=%d", pz_PdapiSuplReq->requestor_id.string_len, 0, 0);
   if( 0 != pz_PdapiSuplReq->requestor_id.string_len)
   {
      uint8 u_length = pz_PdapiSuplReq->requestor_id.string_len;
      if( TRUE == locNi_ConvertToQmiNiFormat(
                  (pdsm_lcs_supl_format_indicator_e_type)(pz_PdapiSuplReq->requestor_id.data_coding_scheme),
                  &(pz_NiSuplInd->requestorId.formatType)))
      {
         if((u_length <= sizeof(pz_NiSuplInd->requestorId.formattedString)) && 
            (u_length <= sizeof(pz_PdapiSuplReq->requestor_id.requestor_id_string)))
         {
            pz_NiSuplInd->requestorId.formattedString_len = 
               memscpy(pz_NiSuplInd->requestorId.formattedString,
                       sizeof(pz_NiSuplInd->requestorId.formattedString),
                       pz_PdapiSuplReq->requestor_id.requestor_id_string,
                       u_length);
            pz_NiSuplInd->valid_flags |= QMI_LOC_SUPL_REQUESTOR_ID_MASK_V02;
         }
      }
   }

   LOC_MSG_LOW("locNi_XlateSuplReqFromPdapiToQmi: clientName len=%d", pz_PdapiSuplReq->client_name.string_len, 0, 0);
   if( 0 != pz_PdapiSuplReq->client_name.string_len)
   {
      uint8 u_length = pz_PdapiSuplReq->client_name.string_len;
      if( TRUE == locNi_ConvertToQmiNiFormat(
                  (pdsm_lcs_supl_format_indicator_e_type)(pz_PdapiSuplReq->client_name.data_coding_scheme),
                  &(pz_NiSuplInd->clientName.formatType)))
      {
         if((u_length <= sizeof(pz_NiSuplInd->clientName.formattedString)) && 
            (u_length <= sizeof(pz_PdapiSuplReq->client_name.client_name_string)))
         {
            pz_NiSuplInd->clientName.formattedString_len = 
               memscpy(pz_NiSuplInd->clientName.formattedString,
                        sizeof(pz_NiSuplInd->clientName.formattedString),
                        pz_PdapiSuplReq->client_name.client_name_string,
                        u_length);
            pz_NiSuplInd->valid_flags |= QMI_LOC_SUPL_CLIENT_NAME_MASK_V02;
         }
      }
   }

   pz_NiSuplInd->suplQop.validMask = 0;
   pz_NiSuplInd->suplQop.horizontalAccuracy = pz_PdapiSuplReq->supl_qop.horacc;
   pz_NiSuplInd->suplQop.validMask |= QMI_LOC_NI_SUPL_MASK_QOP_HORZ_ACC_VALID_V02;
   pz_NiSuplInd->suplQop.verticalAccuracy = pz_PdapiSuplReq->supl_qop.veracc;
   pz_NiSuplInd->suplQop.validMask |= QMI_LOC_NI_SUPL_MASK_QOP_VER_ACC_VALID_V02;
   pz_NiSuplInd->suplQop.maxLocAge = pz_PdapiSuplReq->supl_qop.maxLocAge;
   pz_NiSuplInd->suplQop.validMask |= QMI_LOC_NI_SUPL_MASK_QOP_MAXAGE_VALID_V02;
   pz_NiSuplInd->suplQop.delay = pz_PdapiSuplReq->supl_qop.delay;
   pz_NiSuplInd->suplQop.validMask |= QMI_LOC_NI_SUPL_MASK_QOP_DELAY_VALID_V02;
   pz_NiSuplInd->valid_flags |= QMI_LOC_SUPL_QOP_MASK_V02;

   pz_NiSuplInd->userResponseTimer = pz_PdapiSuplReq->user_response_timer;
   pz_NiSuplInd->valid_flags |= QMI_LOC_SUPL_USER_RESP_TIMER_MASK_V02;

   LOC_MSG_LOW("locNi_XlateSuplReqFromPdapiToQmi: flags = 0x%x",pz_PdapiSuplReq->flags, 0, 0);
   if(pz_PdapiSuplReq->flags & PDSM_LCS_SUPLINIT_VER2_EXT_PRESENT)
   {
      /* copy SUPL 2 extension from PDAPI to QMI_LOC */
      if( TRUE == locNi_XlateSuplVer2FromPdapiToQmi(
                     &(pz_PdapiSuplReq->supl_init_ver2_ext),
                     pz_NiNotifyVerifyReqInd ))
      {
         pz_NiNotifyVerifyReqInd->NiSuplVer2ExtInd_valid = TRUE;
         LOC_MSG_LOW("locNi_XlateSuplReqFromPdapiToQmi: supl extensions,trigger type = %d,"
                        "supported Networks = %u, gnss type = %u\n", 
                        pz_NiNotifyVerifyReqInd->NiSuplVer2ExtInd.triggerType,
                        pz_NiNotifyVerifyReqInd->NiSuplVer2ExtInd.supportedNetworksMask,
                        pz_NiNotifyVerifyReqInd->NiSuplVer2ExtInd.gnssType);
      }
   }

   if(pz_PdapiSuplReq->flags & PDSM_LCS_SUPLINIT_VER2_EXT_EMERGENCY_PRESENT)
   {
      pz_NiNotifyVerifyReqInd->suplEmergencyNotification_valid = TRUE;
      if(pz_PdapiSuplReq->supl_init_ver2_ext_emergency.b_eslp_address_present &&
         PDSM_SERVER_TYPE_URL_ADDRESS == pz_PdapiSuplReq->supl_init_ver2_ext_emergency.e_slp_address.choice)
      {
         memscpy(pz_NiNotifyVerifyReqInd->suplEmergencyNotification.eslpUrl,
            sizeof(pz_NiNotifyVerifyReqInd->suplEmergencyNotification.eslpUrl),
            pz_PdapiSuplReq->supl_init_ver2_ext_emergency.e_slp_address.supl_slp_address.server_adrs_url.url,
            sizeof(pz_PdapiSuplReq->supl_init_ver2_ext_emergency.e_slp_address.supl_slp_address.server_adrs_url.url));
      }
   }
}

// Translate UMTS NI request from PDAPI to QMI_LOC API
static void locNi_XlateUmtsReqFromPdapiToQmi
(
   const pdsm_lcs_umts_cp_notify_verify_req_s_type*  pz_PdapiUmtsReq,
   qmiLocEventNiNotifyVerifyReqIndMsgT_v02*          pz_NiNotifyVerifyReqInd
)
{
   /* Input parameter sanity check is done in LocNi_EventCb */
   qmiLocNiUmtsCpNotifyVerifyStructT_v02* pz_NiUmtsInd = &(pz_NiNotifyVerifyReqInd->NiUmtsCpInd);
   pz_NiNotifyVerifyReqInd->NiUmtsCpInd_valid = TRUE;

   switch(pz_PdapiUmtsReq->notification_priv_type)
   {
      case PDSM_LCS_USER_NOTIFY_ONLY:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NOTIFY_ONLY_V02;
      break;

      case PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP_V02;
      break;

      case PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP_V02;
      break;

      default:
         pz_NiNotifyVerifyReqInd->notificationType = eQMI_LOC_NI_USER_NOTIFY_ONLY_V02;
      break;
   }

   pz_NiUmtsInd->valid_flags = 0;
   pz_NiUmtsInd->valid_flags |= QMI_LOC_UMTS_CP_INVOKE_ID_MASK_V02;
   pz_NiUmtsInd->invokeId = pz_PdapiUmtsReq->invoke_id;

   if(pz_PdapiUmtsReq->flags & PDSM_LCS_ENCODING_TYPE_PRESENT)
   {
      if(TRUE == locNi_ConvertToQmiDataCodingScheme(
                   pz_PdapiUmtsReq->datacoding_scheme,
                  &pz_NiUmtsInd->dataCodingScheme))
      {
         pz_NiUmtsInd->valid_flags |= QMI_LOC_UMTS_CP_DATA_CODING_SCHEME_MASK_V02;
      }
   }

   if((pz_PdapiUmtsReq->flags & PDSM_LCS_CLIENT_NAME_PRESENT) &&
      (pz_PdapiUmtsReq->notification_text != NULL))
   {
      pz_NiUmtsInd->notificationText_len = 
         memscpy (pz_NiUmtsInd->notificationText,
                  sizeof(pz_NiUmtsInd->notificationText),
                  pz_PdapiUmtsReq->notification_text,
                  pz_PdapiUmtsReq->notification_length);
      pz_NiUmtsInd->valid_flags |= QMI_LOC_UMTS_CP_NOTIFICATION_TEXT_MASK_V02;
   }

   if(pz_PdapiUmtsReq->flags & PDSM_LCS_CLIENT_EXTADDR_PRESENT)
   {
      pz_NiUmtsInd->clientAddress_len = 
         memscpy (pz_NiUmtsInd->clientAddress,
                  sizeof(pz_NiUmtsInd->clientAddress),
                  pz_PdapiUmtsReq->ext_client_address_data.ext_client_address,
                  sizeof(pz_PdapiUmtsReq->ext_client_address_data.ext_client_address));
      pz_NiUmtsInd->valid_flags |= QMI_LOC_UMTS_CP_CLIENT_ADDRESS_MASK_V02;
   }

   pz_NiUmtsInd->valid_flags |= QMI_LOC_UMTS_CP_LOCATION_TYPE_MASK_V02;
   switch ( pz_PdapiUmtsReq->location_type)
   {
      case PDSM_LOCATIONTYPE_CURRENT_LOCATION:
         pz_NiUmtsInd->locationType = eQMI_LOC_NI_LOCATIONTYPE_CURRENT_LOCATION_V02;
      break;
      case PDSM_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION:
         pz_NiUmtsInd->locationType = eQMI_LOC_NI_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION_V02;
      break;
      case PDSM_LOCATIONTYPE_INITIAL_LOCATION:
         pz_NiUmtsInd->locationType = eQMI_LOC_NI_LOCATIONTYPE_INITIAL_LOCATION_V02;
      break;
      default:
         LOC_MSG_HIGH ( "locNi_XlateUmtsReqFromPdapiToQmi could not translate location_type %d", 
                        pz_PdapiUmtsReq->location_type, 0, 0);
         pz_NiUmtsInd->valid_flags &= ~QMI_LOC_UMTS_CP_LOCATION_TYPE_MASK_V02;
      break;
   }

   if(pz_PdapiUmtsReq->flags & PDSM_LCS_REQUESTOR_ID_PRESENT)
   {
      LOC_MSG_LOW("locNi_XlateUmtsReqFromPdapiToQmi: requestor ID len=%d", pz_PdapiUmtsReq->requestor_id.string_len, 0, 0);
      if( 0 != pz_PdapiUmtsReq->requestor_id.string_len)
      {
         uint8 u_length = pz_PdapiUmtsReq->requestor_id.string_len;
         if( TRUE == locNi_ConvertToQmiDataCodingScheme(
            (pdsm_lcs_notify_verify_datacoding_scheme_e_type)(pz_PdapiUmtsReq->requestor_id.data_coding_scheme),
            &pz_NiUmtsInd->requestorId.dataCodingScheme))
         {
            if((u_length <= sizeof(pz_NiUmtsInd->requestorId.codedString)) && 
               (u_length <= sizeof(pz_PdapiUmtsReq->requestor_id.requestor_id_string)))
            {
               pz_NiUmtsInd->requestorId.codedString_len = 
                  memscpy(pz_NiUmtsInd->requestorId.codedString,
                          sizeof(pz_NiUmtsInd->requestorId.codedString),
                          pz_PdapiUmtsReq->requestor_id.requestor_id_string,
                          u_length);
               pz_NiUmtsInd->valid_flags |= QMI_LOC_UMTS_CP_REQUESTOR_ID_MASK_V02;
            }
         }
      }
   }

   if(pz_PdapiUmtsReq->flags & PDSM_LCS_CODEWORD_PRESENT)
   {
      LOC_MSG_LOW("locNi_XlateUmtsReqFromPdapiToQmi: codeword_string len=%d", pz_PdapiUmtsReq->codeword_string.string_len, 0, 0);
      if( 0 != pz_PdapiUmtsReq->codeword_string.string_len)
      {
         uint8 u_length = pz_PdapiUmtsReq->codeword_string.string_len;
         if( TRUE == locNi_ConvertToQmiDataCodingScheme(
            (pdsm_lcs_notify_verify_datacoding_scheme_e_type)(pz_PdapiUmtsReq->codeword_string.data_coding_scheme),
            &pz_NiUmtsInd->codewordString.dataCodingScheme))
         {
            if((u_length <= sizeof(pz_NiUmtsInd->codewordString.codedString)) && 
               (u_length <= sizeof(pz_PdapiUmtsReq->codeword_string.lcs_codeword_string)))
            {
               pz_NiUmtsInd->codewordString.codedString_len = 
                  memscpy(pz_NiUmtsInd->codewordString.codedString,
                          sizeof(pz_NiUmtsInd->codewordString.codedString),
                          pz_PdapiUmtsReq->codeword_string.lcs_codeword_string,
                          u_length);
               pz_NiUmtsInd->valid_flags |= QMI_LOC_UMTS_CP_CODEWORD_STRING_MASK_V02;
            }
         }
      }
   }

   if(pz_PdapiUmtsReq->flags & PDSM_LCS_SERVICE_TYPE_ID_PRESENT)
   {
      pz_NiUmtsInd->lcsServiceTypeId =
         pz_PdapiUmtsReq->service_type_id.lcs_service_type_id;
      pz_NiUmtsInd->valid_flags |= QMI_LOC_UMTS_CP_SERVICE_TYPE_MASK_V02;
   }

   pz_NiUmtsInd->userResponseTimer =
      pz_PdapiUmtsReq->user_response_timer;
   pz_NiUmtsInd->valid_flags |= QMI_LOC_UMTS_CP_USER_RESP_TIMER_MASK_V02;
}

// Translate V1/V2 NI request from QMI_Loc API to PD API
static boolean locNi_XlateVxReqFromQmiToPdapi
(
   const qmiLocNiVxNotifyVerifyStructT_v02*  pz_NiVxPayload,
   pdsm_lcs_vx_notify_verify_req_s_type*     pz_RespInfo
)
{
   boolean v_RetVal = TRUE;

   /* Input parameter sanity check is done in locNi_ProcessUserResponse */
   switch(pz_NiVxPayload->posMode)
   {
      case eQMI_LOC_NI_VX_MS_ASSISTED_ONLY_V02:
         pz_RespInfo->pos_mode = PDSM_LCS_MS_ASSISTED_ONLY;
      break;

      case eQMI_LOC_NI_VX_MS_BASED_ONLY_V02:
         pz_RespInfo->pos_mode = PDSM_LCS_MS_BASED_ONLY;
      break;

      case eQMI_LOC_NI_VX_MS_ASSISTED_PREFERRED_MS_BASED_ALLOWED_V02:
         pz_RespInfo->pos_mode = PDSM_LCS_MS_ASSISTED_PREF_MSBASED_ALLWD;
      break;

      case eQMI_LOC_NI_VX_MS_BASED_PREFERRED_MS_ASSISTED_ALLOWED_V02:
         pz_RespInfo->pos_mode = PDSM_LCS_MS_BASED_PREF_ASSISTED_ALLWD;
      break;

      default:
         v_RetVal = FALSE;
         LOC_MSG_ERROR("locNi_XlateVxReqFromQmiToPdapi, invalid posMode=%d",
                        pz_NiVxPayload->posMode, 0, 0);
      break;
   }

   if( TRUE == v_RetVal )
   {
      switch(pz_NiVxPayload->encodingScheme)
      {
         case eQMI_LOC_NI_VX_OCTET_V02:
            pz_RespInfo->encoding_scheme = PDSM_VX_OCTET;
         break;

         case eQMI_LOC_NI_VX_EXN_PROTOCOL_MSG_V02:
            pz_RespInfo->encoding_scheme = PDSM_VX_EXN_PROTOCOL_MSG;
         break;

         case eQMI_LOC_NI_VX_ASCII_V02:
            pz_RespInfo->encoding_scheme = PDSM_VX_ASCII;
         break;

         case eQMI_LOC_NI_VX_IA5_V02:
            pz_RespInfo->encoding_scheme = PDSM_VX_IA5;
         break;

         case eQMI_LOC_NI_VX_UNICODE_V02:
            pz_RespInfo->encoding_scheme = PDSM_VX_UNICODE;
         break;

         case eQMI_LOC_NI_VX_SHIFT_JIS_V02:
            pz_RespInfo->encoding_scheme = PDSM_VX_SHIFT_JIS;
         break;

         case eQMI_LOC_NI_VX_KOREAN_V02:
            pz_RespInfo->encoding_scheme = PDSM_VX_KOREAN;
         break;

         case eQMI_LOC_NI_VX_LATIN_HEBREW_V02:
            pz_RespInfo->encoding_scheme = PDSM_VX_LATIN_HEBREW;
         break;

         case eQMI_LOC_NI_VX_LATIN_V02:
            pz_RespInfo->encoding_scheme = PDSM_VX_LATIN;
         break;

         case eQMI_LOC_NI_VX_GSM_V02:
            pz_RespInfo->encoding_scheme = PDSM_VX_GSM;
         break;

         default:
            v_RetVal = FALSE;
            LOC_MSG_ERROR("locNi_XlateVxReqFromQmiToPdapi, invalid encodingScheme=%d",
                           pz_NiVxPayload->encodingScheme, 0, 0);
         break;
      }
   }

   if( TRUE == v_RetVal )
   {
      if(TRUE == LOC_MW_IS_VALID_BOOLEAN(pz_NiVxPayload->posQosIncl))
      {
         pz_RespInfo->pos_qos_incl = pz_NiVxPayload->posQosIncl;
      }
      else
      {
         v_RetVal = FALSE;
         LOC_MSG_ERROR("locNi_XlateVxReqFromQmiToPdapi, invalid posQosIncl=%d",
                        pz_NiVxPayload->posQosIncl, 0, 0);
      }
   }

   if( TRUE == v_RetVal )
   {
      pz_RespInfo->requester_id.requester_id_length = 
      memscpy((void *)&(pz_RespInfo->requester_id.requester_id[0]),
              sizeof (pz_RespInfo->requester_id.requester_id),
              (void *)&(pz_NiVxPayload->requestorId),
              pz_NiVxPayload->requestorId_len);

      pz_RespInfo->pos_qos      = pz_NiVxPayload->posQos;
      pz_RespInfo->num_fixes    = pz_NiVxPayload->numFixes;
      pz_RespInfo->tbf          = pz_NiVxPayload->timeBetweenFixes;
      pz_RespInfo->user_resp_timer_val = pz_NiVxPayload->userRespTimerInSeconds;
   }

   return v_RetVal;
}

static boolean locNi_ConvertToPdapiNiFormat
(
  qmiLocNiSuplFormatEnumType_v02          e_QmiNiFormatType,
  pdsm_lcs_supl_format_indicator_e_type*  pe_PdapiNiFormatType
)
{
   boolean v_RetVal = FALSE;
   uint32  q_ctr = 0;
   uint32  q_size = (sizeof(z_NiSuplFormatMap))/sizeof(z_NiSuplFormatMap[0]);

   if(NULL == pe_PdapiNiFormatType)
   {
      LOC_MSG_ERROR("locNi_ConvertToPdapiNiFormat NULL pointer", 0, 0, 0);
      return v_RetVal;
   }

   /* Input parameter sanity check is done in locNi_ProcessUserResponse */
   for(; q_ctr < q_size; q_ctr++)
   {
      if(z_NiSuplFormatMap[q_ctr].e_QmiNiFormatType == e_QmiNiFormatType)
      {
         LOC_MSG_LOW ("locNi_ConvertToPdapiNiFormat q_ctr %d QMI %d PDAPI %d ",
                         q_ctr, e_QmiNiFormatType,
                         z_NiSuplFormatMap[q_ctr].e_PdapiNiFormatType);
         *pe_PdapiNiFormatType = z_NiSuplFormatMap[q_ctr].e_PdapiNiFormatType;
         v_RetVal = TRUE;
         break;
      }

   }

   return v_RetVal;
}

static boolean locNi_ConvertToPdapiDataCodingScheme
(
   qmiLocNiDataCodingSchemeEnumT_v02    e_QmiNiDataCodingScheme,
   pdsm_lcs_notify_verify_datacoding_scheme_e_type*   pe_PdapiDataCodingScheme
)
{
   boolean v_RetVal = FALSE;
   uint32  q_ctr = 0;
   uint32  q_size = (sizeof(z_NiDataCodingSchemeMap))/sizeof(z_NiDataCodingSchemeMap[0]);

   if(NULL == pe_PdapiDataCodingScheme)
   {
      LOC_MSG_ERROR("locNi_ConvertToPdapiDataCodingScheme NULL pointer", 0, 0, 0);
      return v_RetVal;
   }

   /* Input parameter sanity check is done in locNi_ProcessUserResponse */
   for(; q_ctr < q_size; q_ctr++)
   {
      if(z_NiDataCodingSchemeMap[q_ctr].e_QmiNiDataCodingScheme == e_QmiNiDataCodingScheme)
      {
         LOC_MSG_LOW ("locNi_ConvertToPdapiDataCodingScheme q_ctr %d QMI %d PDAPI %d ",
                         q_ctr, e_QmiNiDataCodingScheme,
                         z_NiDataCodingSchemeMap[q_ctr].e_PdapiDataCodingScheme);
         *pe_PdapiDataCodingScheme = z_NiDataCodingSchemeMap[q_ctr].e_PdapiDataCodingScheme;
         v_RetVal = TRUE;
         break;
      }
   }

   return v_RetVal;
}

static boolean locNi_ConvertToPdapiNotificationType
(
   qmiLocNiNotifyVerifyEnumT_v02    e_QmiNotificationType,
   pdsm_lcs_notify_verify_e_type*   pe_PdapiNotifyVerifyType
)
{
   boolean v_RetVal = TRUE;

   if(NULL == pe_PdapiNotifyVerifyType)
   {
      LOC_MSG_ERROR("locNi_ConvertToPdapiNotificationType, NULL pe_PdapiNotifyVerifyType",
                     0, 0, 0);
      return FALSE;
   }

   /* Input parameter sanity check is done in locNi_ProcessUserResponse */
   switch(e_QmiNotificationType)
   {
      case eQMI_LOC_NI_USER_NO_NOTIFY_NO_VERIFY_V02:
         *pe_PdapiNotifyVerifyType = PDSM_LCS_NO_NOTIFY_NO_VERIFY;
      break;

      case eQMI_LOC_NI_USER_NOTIFY_ONLY_V02:
         *pe_PdapiNotifyVerifyType = PDSM_LCS_USER_NOTIFY_ONLY;
      break;

      case eQMI_LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP_V02:
         *pe_PdapiNotifyVerifyType = PDSM_LCS_USER_NOTIFY_VERIFY_ALLOW_NO_RESP;
      break;

      case eQMI_LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP_V02:
         *pe_PdapiNotifyVerifyType = PDSM_LCS_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP;
      break;

      case eQMI_LOC_NI_USER_NOTIFY_VERIFY_PRIVACY_OVERRIDE_V02:
         *pe_PdapiNotifyVerifyType = PDSM_LCS_PRIVACY_OVERRIDE;
      break;

      default:
         v_RetVal = FALSE;
         LOC_MSG_ERROR("locNi_ConvertToPdapiNotificationType, invalid e_QmiNotificationType=%d",
                        e_QmiNotificationType, 0, 0);
      break;
   }
   return v_RetVal;
}

static boolean locNi_XlateSuplPayloadFromQmiToPdapi
(
   const qmiLocNiUserRespReqMsgT_v02*        pz_NiUserRespReq,
   pdsm_lcs_notification_resp_s_type*        pz_RespInfo
)
{
   boolean v_RetVal = TRUE;
   const qmiLocNiSuplNotifyVerifyStructT_v02  *pz_QmiSuplPayload = 
                                       &(pz_NiUserRespReq->NiSuplPayload);
   pdsm_lcs_supl_notify_verify_req_s_type*   pz_PdapiSuplReq = 
                                       &(pz_RespInfo->lcs_info.supl_req);

   /* Input parameter sanity check is done in locNi_ProcessUserResponse */
   v_RetVal = locNi_ConvertToPdapiNotificationType(pz_NiUserRespReq->notificationType,
                                 &(pz_PdapiSuplReq->notification_priv_type));

   pz_PdapiSuplReq->flags = 0;
   if ( TRUE == v_RetVal )
   {
      /* 1 Network Initiated SUPL Request */
      if(pz_QmiSuplPayload->valid_flags & QMI_LOC_SUPL_SESSION_ID_MASK_V02 )
      {
         pz_PdapiSuplReq->supl_slp_session_id.presence = TRUE;
         memscpy ( pz_PdapiSuplReq->supl_slp_session_id.session_id, sizeof(pz_PdapiSuplReq->supl_slp_session_id.session_id),
                  pz_QmiSuplPayload->suplSessionId, sizeof(pz_QmiSuplPayload->suplSessionId)); 

         if ( pz_QmiSuplPayload->valid_flags & QMI_LOC_SUPL_SERVER_INFO_MASK_V02 )
         {
            if ( pz_QmiSuplPayload->suplServerInfo.suplServerAddrTypeMask & 
                        QMI_LOC_SERVER_ADDR_TYPE_IPV4_MASK_V02 ) 
            {
               pz_PdapiSuplReq->supl_slp_session_id.slp_address.choice =
                                PDSM_SERVER_TYPE_IPV4_ADDRESS;
               pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs =
                                       pz_QmiSuplPayload->suplServerInfo.ipv4Addr.addr;
               pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id= 
                                       pz_QmiSuplPayload->suplServerInfo.ipv4Addr.port;
            }
            else if ( pz_QmiSuplPayload->suplServerInfo.suplServerAddrTypeMask & 
                        QMI_LOC_SERVER_ADDR_TYPE_IPV6_MASK_V02 ) 
            {
               pz_PdapiSuplReq->supl_slp_session_id.slp_address.choice =
                                PDSM_SERVER_TYPE_IPV6_ADDRESS;
               memscpy ( pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs,
                 sizeof(pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs),
                 pz_QmiSuplPayload->suplServerInfo.ipv6Addr.addr,
                 sizeof(pz_QmiSuplPayload->suplServerInfo.ipv6Addr.addr));
            
               pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id = 
                                       pz_QmiSuplPayload->suplServerInfo.ipv6Addr.port;
            }
            else if ( pz_QmiSuplPayload->suplServerInfo.suplServerAddrTypeMask & 
                        QMI_LOC_SERVER_ADDR_TYPE_URL_MASK_V02 ) 
            {
               size_t suplServerurl_len = 0;
               pz_PdapiSuplReq->supl_slp_session_id.slp_address.choice =
                                PDSM_SERVER_TYPE_URL_ADDRESS;

               /* Bounded Buffer Copy. Length of src string is returned */
               suplServerurl_len = GNSS_STRLCPY ( 
                  (char*)pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url,
                  (char*)pz_QmiSuplPayload->suplServerInfo.urlAddr,
                  sizeof(pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url) );

               /* Check if copy was truncated. Adjust length accordingly */
               if ( suplServerurl_len >= sizeof(pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url))
               {
                  pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url_length = \
                           sizeof(pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url) - 1;
                  LOC_MSG_MED("locNi_XlateSuplPayloadFromQmiToPdapi:SUPL srvr URL too long: %d, truncated",      \
                                 suplServerurl_len, 0, 0);
               }
               else
               {
                  pz_PdapiSuplReq->supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url_length = \
                           suplServerurl_len;
               }
            }
         }/* if ( pz_QmiSuplPayload->valid_flags & QMI_LOC_SUPL_SERVER_INFO_MASK_V02 ) */
      }/* if ( pz_QmiSuplPayload->valid_flags & QMI_LOC_SUPL_SESSION_ID_MASK_V02 ) */

      /* No flag required on inbound */
      if ( pz_QmiSuplPayload->valid_flags & QMI_LOC_SUPL_HASH_MASK_V02 )
      {
         memscpy( pz_PdapiSuplReq->supl_hash, sizeof(pz_PdapiSuplReq->supl_hash),
                  pz_QmiSuplPayload->suplHash, sizeof(pz_QmiSuplPayload->suplHash));
      }

      if( TRUE == v_RetVal )
      {
         if ( pz_QmiSuplPayload->valid_flags & QMI_LOC_SUPL_POS_METHOD_MASK_V02 )
         {
            switch(pz_QmiSuplPayload->posMethod)
            {
               case eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETASSISTED_V02:
                  pz_PdapiSuplReq->pos_method = POSMETHOD_AGPS_SETASSISTED;
                  break;
               case eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETBASED_V02:
                  pz_PdapiSuplReq->pos_method = POSMETHOD_AGPS_SETBASED;
                  break;
               case eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETASSISTED_PREF_V02:
                  pz_PdapiSuplReq->pos_method = POSMETHOD_AGPS_SETASSISTED_PREF;
                  break;
               case eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETBASED_PREF_V02:
                  pz_PdapiSuplReq->pos_method = POSMETHOD_AGPS_SETBASED_PREF;
                  break;
               case eQMI_LOC_NI_SUPL_POSMETHOD_AUTONOMOUS_GPS_V02:
                  pz_PdapiSuplReq->pos_method = POSMETHOD_AUTONOMOUS_GPS;
                  break;
               case eQMI_LOC_NI_SUPL_POSMETHOD_AFLT_V02:
                  pz_PdapiSuplReq->pos_method = POSMETHOD_AFLT;
                  break;
               case eQMI_LOC_NI_SUPL_POSMETHOD_ECID_V02:
                  pz_PdapiSuplReq->pos_method = POSMETHOD_ECID;
                  break;
               case eQMI_LOC_NI_SUPL_POSMETHOD_EOTD_V02:
                  pz_PdapiSuplReq->pos_method = POSMETHOD_EOTD;
                  break;
               case eQMI_LOC_NI_SUPL_POSMETHOD_OTDOA_V02:
                  pz_PdapiSuplReq->pos_method = POSMETHOD_OTDOA;
                  break;
               case eQMI_LOC_NI_SUPL_POSMETHOD_NO_POSITION_V02:
                  pz_PdapiSuplReq->pos_method = POSMETHOD_NO_POSITION;
                  break;
               default:
                  v_RetVal = FALSE;
                  LOC_MSG_ERROR("locNi_XlateSuplPayloadFromQmiToPdapi:invalid posMethod=%d,",\
                                 pz_QmiSuplPayload->posMethod, 0, 0);
            }
         }
         else
         {
            v_RetVal = FALSE;
            LOC_MSG_ERROR("locNi_XlateSuplPayloadFromQmiToPdapi:posMethod flag not set",0, 0, 0);
         }
      }

      if ( pz_QmiSuplPayload->valid_flags & QMI_LOC_SUPL_DATA_CODING_SCHEME_MASK_V02 )
      {
        v_RetVal = locNi_ConvertToPdapiDataCodingScheme ( 
                          pz_QmiSuplPayload->dataCodingScheme,
                          &(pz_PdapiSuplReq->datacoding_scheme));

        if( TRUE == v_RetVal )
        {
          pz_PdapiSuplReq->flags |= PDSM_LCS_ENCODING_TYPE_PRESENT;
        }
      }

      if ( (  TRUE == v_RetVal ) &&
         ( pz_QmiSuplPayload->valid_flags & QMI_LOC_SUPL_REQUESTOR_ID_MASK_V02 ) )
      {
         pdsm_lcs_supl_format_indicator_e_type e_FormatIndicatior;
         v_RetVal = locNi_ConvertToPdapiNiFormat(
                     pz_QmiSuplPayload->requestorId.formatType,
                     &e_FormatIndicatior);

         if ( TRUE == v_RetVal )
         {
            pz_PdapiSuplReq->requestor_id.string_len = memscpy( 
                        (void *)pz_QmiSuplPayload->requestorId.formattedString,
                        sizeof(pz_QmiSuplPayload->requestorId.formattedString),
                        pz_PdapiSuplReq->requestor_id.requestor_id_string,
                        sizeof(pz_PdapiSuplReq->requestor_id.requestor_id_string));

            pz_PdapiSuplReq->requestor_id.data_coding_scheme = (byte)e_FormatIndicatior;
            pz_PdapiSuplReq->flags |= PDSM_LCS_REQUESTOR_ID_PRESENT;
         }
      }

      if ( ( TRUE == v_RetVal ) &&
         ( pz_QmiSuplPayload->valid_flags & QMI_LOC_SUPL_CLIENT_NAME_MASK_V02 ) )
      {
         pdsm_lcs_supl_format_indicator_e_type e_FormatIndicatior;
         v_RetVal = locNi_ConvertToPdapiNiFormat(
                  pz_QmiSuplPayload->clientName.formatType,
                  &e_FormatIndicatior);

         if ( TRUE == v_RetVal )
         {
            pz_PdapiSuplReq->client_name.string_len = memscpy(
                        (void *)pz_QmiSuplPayload->clientName.formattedString,
                        sizeof(pz_QmiSuplPayload->clientName.formattedString),
                        pz_PdapiSuplReq->client_name.client_name_string,
                        sizeof(pz_PdapiSuplReq->client_name.client_name_string));

            pz_PdapiSuplReq->client_name.data_coding_scheme = (byte)e_FormatIndicatior;
            pz_PdapiSuplReq->flags |= PDSM_LCS_CLIENT_NAME_PRESENT;
         }
      }

      if ( ( TRUE == v_RetVal ) &&
         ( pz_QmiSuplPayload->valid_flags & QMI_LOC_SUPL_QOP_MASK_V02 ) )
      {
         pz_PdapiSuplReq->flags |= PDSM_LCS_SUPL_QOP_VALID;
      
         /* No mask present in Loc-API so assuming it is always provided */
         if ( pz_QmiSuplPayload->suplQop.validMask & 
                     QMI_LOC_NI_SUPL_MASK_QOP_HORZ_ACC_VALID_V02 )
         {
            pz_PdapiSuplReq->supl_qop.horacc = pz_QmiSuplPayload->suplQop.horizontalAccuracy;
         }
         else
         {
            LOC_MSG_ERROR("locNi_XlateSuplPayloadFromQmiToPdapi:Hor Acc not set",0, 0, 0);
            v_RetVal = FALSE;
         }
      
         if ( pz_QmiSuplPayload->suplQop.validMask & 
                     QMI_LOC_NI_SUPL_MASK_QOP_MAXAGE_VALID_V02 )
         {
            pz_PdapiSuplReq->supl_qop.maxLocAge = pz_QmiSuplPayload->suplQop.maxLocAge;
            pz_PdapiSuplReq->supl_qop.bit_mask   |=  PDSM_LCS_SUPL_QOP_MAXAGE_VALID;
         }
         if ( pz_QmiSuplPayload->suplQop.validMask & 
                     QMI_LOC_NI_SUPL_MASK_QOP_DELAY_VALID_V02 )
         {
            pz_PdapiSuplReq->supl_qop.delay = pz_QmiSuplPayload->suplQop.delay;
            pz_PdapiSuplReq->supl_qop.bit_mask   |=  PDSM_LCS_SUPL_QOP_DELAY_VALID;
         }

         if ( pz_QmiSuplPayload->suplQop.validMask & 
                     QMI_LOC_NI_SUPL_MASK_QOP_VER_ACC_VALID_V02 )
         {
            pz_PdapiSuplReq->supl_qop.veracc = pz_QmiSuplPayload->suplQop.verticalAccuracy;
            pz_PdapiSuplReq->supl_qop.bit_mask   |=  PDSM_LCS_SUPL_QOP_VERACC_VALID;
         }
      }

      if ( ( TRUE == v_RetVal ) &&
         ( pz_QmiSuplPayload->valid_flags & 
                  QMI_LOC_SUPL_USER_RESP_TIMER_MASK_V02 ) )
      {
         pz_PdapiSuplReq->user_response_timer = pz_QmiSuplPayload->userResponseTimer;
      }

      /* 2 Network Initiated SUPL Version 2 Extension */
      if( ( TRUE == v_RetVal ) &&
         (TRUE == pz_NiUserRespReq->NiSuplVer2ExtPayload_valid ) )
      {
         pz_PdapiSuplReq->flags|= PDSM_LCS_SUPLINIT_VER2_EXT_PRESENT;

         v_RetVal = locNi_XlateSuplVer2ExtPayloadFromQmiToPdapi(
            pz_NiUserRespReq,pz_RespInfo);

      }

      /* 3 SUPL Emergency Notification */
      if( ( TRUE == v_RetVal ) &&
         (TRUE == pz_NiUserRespReq->suplEmergencyNotification_valid ) )
      {
         pz_PdapiSuplReq->flags|= PDSM_LCS_SUPLINIT_VER2_EXT_EMERGENCY_PRESENT;
         pz_PdapiSuplReq->supl_init_ver2_ext_emergency.b_eslp_address_present = FALSE;

         if('\0' != pz_NiUserRespReq->suplEmergencyNotification.eslpUrl[0])
         {
            pz_PdapiSuplReq->supl_init_ver2_ext_emergency.b_eslp_address_present = TRUE;
            pz_PdapiSuplReq->supl_init_ver2_ext_emergency.e_slp_address.choice = PDSM_SERVER_TYPE_URL_ADDRESS;
            pz_PdapiSuplReq->supl_init_ver2_ext_emergency.e_slp_address.supl_slp_address.server_adrs_url.url_length = 
               GNSS_STRLEN(pz_NiUserRespReq->suplEmergencyNotification.eslpUrl);
            (void)GNSS_STRLCPY(pz_PdapiSuplReq->supl_init_ver2_ext_emergency.e_slp_address.supl_slp_address.server_adrs_url.url,
               pz_NiUserRespReq->suplEmergencyNotification.eslpUrl,
               sizeof(pz_PdapiSuplReq->supl_init_ver2_ext_emergency.e_slp_address.supl_slp_address.server_adrs_url.url));
         }
      }/* (TRUE == pz_NiUserRespReq->suplEmergencyNotification_valid ) */
   }/* if ( TRUE == v_RetVal ) */

   return v_RetVal;
}

// Translate UMTS NI request from QMI_LOC API to PD API
static boolean locNi_XlateUmtsCpFromQmiToPdapi
(
   const qmiLocNiUserRespReqMsgT_v02*         pz_NiUserRespReq,
   pdsm_lcs_umts_cp_notify_verify_req_s_type* pz_RespInfo
)
{
   boolean v_RetVal = TRUE;

   /* Input parameter sanity check is done in locNi_ProcessUserResponse */
   const qmiLocNiUmtsCpNotifyVerifyStructT_v02  *pz_QmiNiUmtsCpPayload =
                            &(pz_NiUserRespReq->NiUmtsCpPayload);

   if ( pz_QmiNiUmtsCpPayload->valid_flags & QMI_LOC_UMTS_CP_INVOKE_ID_MASK_V02 )
   {
      pz_RespInfo->invoke_id = pz_QmiNiUmtsCpPayload->invokeId;
   }
   else
   {
      v_RetVal = FALSE;
      LOC_MSG_ERROR( "locNi_XlateUmtsCpFromQmiToPdapi Invoke ID not specified: ", 
                     0, 0, 0 );
   }

   if ( ( TRUE == v_RetVal ) && 
      ( pz_QmiNiUmtsCpPayload->valid_flags & QMI_LOC_UMTS_CP_DATA_CODING_SCHEME_MASK_V02 ) )
   {
      v_RetVal = locNi_ConvertToPdapiDataCodingScheme ( 
                        pz_QmiNiUmtsCpPayload->dataCodingScheme,
                        &(pz_RespInfo->datacoding_scheme));
      
      if( TRUE == v_RetVal )
      {
         pz_RespInfo->flags |= PDSM_LCS_ENCODING_TYPE_PRESENT;
      }
   }

   if ( TRUE == v_RetVal )
   {
      if ( pz_QmiNiUmtsCpPayload->valid_flags & QMI_LOC_UMTS_CP_NOTIFICATION_TEXT_MASK_V02 )
      {
         memset (umts_cp_notification_text, 0, 
                 sizeof (umts_cp_notification_text));

         memscpy (umts_cp_notification_text, sizeof (umts_cp_notification_text),
                 pz_QmiNiUmtsCpPayload->notificationText, sizeof (pz_QmiNiUmtsCpPayload->notificationText));
           
         pz_RespInfo->notification_length = pz_QmiNiUmtsCpPayload->notificationText_len;
         if (pz_RespInfo->notification_length > sizeof (umts_cp_notification_text)) 
         {
            pz_RespInfo->notification_length = sizeof (umts_cp_notification_text);
         }
         pz_RespInfo->notification_text = umts_cp_notification_text;
         pz_RespInfo->flags |= PDSM_LCS_CLIENT_NAME_PRESENT;
      }

      if ( pz_QmiNiUmtsCpPayload->valid_flags & QMI_LOC_UMTS_CP_CLIENT_ADDRESS_MASK_V02)
      {
         memscpy( pz_RespInfo->ext_client_address_data.ext_client_address,
                  sizeof(pz_RespInfo->ext_client_address_data.ext_client_address),
                  pz_QmiNiUmtsCpPayload->clientAddress,
                  pz_QmiNiUmtsCpPayload->clientAddress_len );

         pz_RespInfo->ext_client_address_data.ext_client_address_len = pz_QmiNiUmtsCpPayload->clientAddress_len;

         if(pz_RespInfo->ext_client_address_data.ext_client_address_len > sizeof(pz_RespInfo->ext_client_address_data.ext_client_address))
         {
            pz_RespInfo->ext_client_address_data.ext_client_address_len = sizeof(pz_RespInfo->ext_client_address_data.ext_client_address);
         }
         pz_RespInfo->flags |= PDSM_LCS_CLIENT_EXTADDR_PRESENT;
      }

      if ( pz_QmiNiUmtsCpPayload->valid_flags & QMI_LOC_UMTS_CP_LOCATION_TYPE_MASK_V02)
      {
        switch ( pz_QmiNiUmtsCpPayload->locationType )
        {
          case eQMI_LOC_NI_LOCATIONTYPE_CURRENT_LOCATION_V02:
            pz_RespInfo->location_type = 
               PDSM_LOCATIONTYPE_CURRENT_LOCATION;
            break;
          case eQMI_LOC_NI_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION_V02:
            pz_RespInfo->location_type = 
               PDSM_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION;
            break;
          case eQMI_LOC_NI_LOCATIONTYPE_INITIAL_LOCATION_V02:
            pz_RespInfo->location_type =
               PDSM_LOCATIONTYPE_INITIAL_LOCATION;
            break;
            
          default:
          {
            v_RetVal  = FALSE;
            LOC_MSG_ERROR( "locNi_XlateUmtsCpFromQmiToPdapi, Invalid locationType: %d", 
                            pz_QmiNiUmtsCpPayload->locationType, 0, 0 );
          }  
          break;
        }
        /* No flag for location type */
      }
      else
      {
        v_RetVal = FALSE;
        LOC_MSG_ERROR( "locNi_XlateUmtsCpFromQmiToPdapi, Location Type not specified: ", 
                        0, 0, 0 );
      }
   }

   if ( ( TRUE == v_RetVal ) &&
      ( pz_QmiNiUmtsCpPayload->valid_flags & QMI_LOC_UMTS_CP_REQUESTOR_ID_MASK_V02 ) )
   {
      if ( pz_QmiNiUmtsCpPayload->requestorId.codedString_len <= 
           sizeof(pz_QmiNiUmtsCpPayload->requestorId.codedString) )
      {
         pdsm_lcs_notify_verify_datacoding_scheme_e_type e_DataCodingScheme;

         pz_RespInfo->requestor_id.string_len = memscpy(
            pz_RespInfo->requestor_id.requestor_id_string,
            sizeof(pz_RespInfo->requestor_id.requestor_id_string),
            pz_QmiNiUmtsCpPayload->requestorId.codedString,
            pz_QmiNiUmtsCpPayload->requestorId.codedString_len);

         v_RetVal = locNi_ConvertToPdapiDataCodingScheme(
            pz_QmiNiUmtsCpPayload->requestorId.dataCodingScheme,
            &e_DataCodingScheme);

         /* Unfortunate the data coding scheme in requestor ID is byte */
         if (TRUE == v_RetVal)
         {
            pz_RespInfo->requestor_id.data_coding_scheme = (byte)e_DataCodingScheme;
         }

         pz_RespInfo->flags |= PDSM_LCS_REQUESTOR_ID_PRESENT;
      }
      else
      {
         v_RetVal = FALSE;
         LOC_MSG_ERROR("locNi_XlateUmtsCpFromQmiToPdapi Invalid requestorId.codedString_len: %d",
                        pz_QmiNiUmtsCpPayload->requestorId.codedString_len, 0, 0);
      }
   }

   if ( ( TRUE == v_RetVal ) &&
      ( pz_QmiNiUmtsCpPayload->valid_flags & QMI_LOC_UMTS_CP_CODEWORD_STRING_MASK_V02) )
   {
      if ( pz_QmiNiUmtsCpPayload->codewordString.codedString_len <= 
           sizeof(pz_QmiNiUmtsCpPayload->codewordString.codedString) )
      {
         pdsm_lcs_notify_verify_datacoding_scheme_e_type e_DataCodingScheme;

         pz_RespInfo->codeword_string.string_len = memscpy(
            pz_RespInfo->codeword_string.lcs_codeword_string,
            sizeof(pz_RespInfo->codeword_string.lcs_codeword_string),
            pz_QmiNiUmtsCpPayload->codewordString.codedString,
            pz_QmiNiUmtsCpPayload->codewordString.codedString_len);

         v_RetVal = locNi_ConvertToPdapiDataCodingScheme(
            pz_QmiNiUmtsCpPayload->codewordString.dataCodingScheme,
            &e_DataCodingScheme);

         /* Unfortunate the data coding scheme in codeword string is byte */
         if (TRUE == v_RetVal)
         {
            pz_RespInfo->codeword_string.data_coding_scheme = (byte)e_DataCodingScheme;
         }

         pz_RespInfo->flags |= PDSM_LCS_CODEWORD_PRESENT;
      }
      else
      {
         v_RetVal = FALSE;
         LOC_MSG_ERROR("locNi_XlateUmtsCpFromQmiToPdapi Invalid codewordString.codedString_len: %d",
                        pz_QmiNiUmtsCpPayload->requestorId.codedString_len, 0, 0);
      }
   }

   if ( ( TRUE == v_RetVal ) &&
      ( pz_QmiNiUmtsCpPayload->valid_flags & QMI_LOC_UMTS_CP_SERVICE_TYPE_MASK_V02 ) )
   {
      pz_RespInfo->service_type_id.lcs_service_type_id = 
                        pz_QmiNiUmtsCpPayload->lcsServiceTypeId;
      pz_RespInfo->flags |= PDSM_LCS_SERVICE_TYPE_ID_PRESENT;
   }

   if ( ( TRUE == v_RetVal ) &&
      ( pz_QmiNiUmtsCpPayload->valid_flags & QMI_LOC_UMTS_CP_USER_RESP_TIMER_MASK_V02 ) )
   {
      pz_RespInfo->user_response_timer = pz_QmiNiUmtsCpPayload->userResponseTimer;
   }

   return v_RetVal;
}

static boolean locNi_XlateSuplVer2ExtPayloadFromQmiToPdapi
(
   const qmiLocNiUserRespReqMsgT_v02*        pz_NiUserRespReq,
   pdsm_lcs_notification_resp_s_type*        pz_RespInfo
)
{
   boolean v_RetVal = TRUE;
   int idx = 0;
   uint32 size = 0;
   /* Input parameter sanity check is done in locNi_ProcessUserResponse */
   const qmiLocNiSuplVer2ExtStructT_v02  *pz_QmiSuplVer2ExtPayload =
                            &(pz_NiUserRespReq->NiSuplVer2ExtPayload);
   pdsm_lcs_supl_suplinit_ver2_ext_s_type*   pz_PdapiSuplInitVer2Ext = 
                            &(pz_RespInfo->lcs_info.supl_req.supl_init_ver2_ext);

   /* clear all the fields */
   memset(pz_PdapiSuplInitVer2Ext, 0, sizeof(*pz_PdapiSuplInitVer2Ext));

  /* convert trigger type to PDSM */
  switch(pz_QmiSuplVer2ExtPayload->triggerType)
  {
    case eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_PERIODIC_V02:
    {
      pz_PdapiSuplInitVer2Ext->trigger_type = 
        PDAPI_SUPLINIT_TRIGGER_TYPE_PERIODIC;
      pz_PdapiSuplInitVer2Ext->validity |=  
        PDAPI_SUPLINIT_VER2_EXT_BITMAP_TRIGTYPE_VALID;
      break;
    }

    case eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_AREA_EVENT_V02:
    {
      pz_PdapiSuplInitVer2Ext->trigger_type = 
        PDAPI_SUPLINIT_TRIGGER_TYPE_AREA_EVENT;
      pz_PdapiSuplInitVer2Ext->validity |=  
        PDAPI_SUPLINIT_VER2_EXT_BITMAP_TRIGTYPE_VALID;
      break;
    }

    case eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_SINGLE_SHOT_V02:
    {
      /* pdsm infers single shot if the the valdity bit is not set,
         just log and break */
      LOC_MSG_MED("locNi_XlateSuplVer2ExtPayloadFromQmiToPdapi: single shot trigger type",
                  0,0,0);
      break;
    }

    default:
    {
      LOC_MSG_ERROR("locNi_XlateSuplVer2ExtPayloadFromQmiToPdapi: Unknown trigger type %d \n",
                    pz_QmiSuplVer2ExtPayload->triggerType, 0, 0);
      return FALSE;
    }
  }
  
  /* convert the gnss type masks from QMI_LOC to PDAPI */
  size = sizeof(z_NiGnssTypeMap) / 
      sizeof(z_NiGnssTypeMap[0]);

  pz_PdapiSuplInitVer2Ext->validity |= 
    PDAPI_SUPLINIT_VER2_EXT_BITMAP_GNSSPOSTECH_VALID;
  for( idx = 0; idx < size; idx++ )
  {
    if( pz_QmiSuplVer2ExtPayload->gnssType & 
        z_NiGnssTypeMap[idx].qmiLoc_supl_ver2_ext_mask)
    {
      pz_PdapiSuplInitVer2Ext->gnss_pos_tech |= 
        z_NiGnssTypeMap[idx].pdapi_supl_ver2_ext_mask ;
    }
  }

  /* convert the supported network masks */
  size = sizeof(z_NiSupportedNetworkMaskMap) / 
      sizeof(z_NiSupportedNetworkMaskMap[0]);

  /* if the supported networks mask is set to ALL networks,
     do not set the validity bit for PDAPI supported networks,
     else convert the masks to PDAPI */

  if(QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_ALL != 
     pz_QmiSuplVer2ExtPayload->supportedNetworksMask)
  {
    pz_PdapiSuplInitVer2Ext->validity |= 
      PDAPI_SUPLINIT_VER2_EXT_BITMAP_SUPTDNTWK_VALID;
    for( idx = 0; idx < size; idx++ )
    {
      if(pz_QmiSuplVer2ExtPayload->supportedNetworksMask  & 
          z_NiSupportedNetworkMaskMap[idx].qmiLoc_supl_ver2_ext_mask )
      {
         pz_PdapiSuplInitVer2Ext->supl_supported_ntwkinfo |= 
           z_NiSupportedNetworkMaskMap[idx].pdapi_supl_ver2_ext_mask ;
      }
    }

  }
  return TRUE;
}

int locNi_ProcessCmdRequest(const LocMW_LocCmdStructT* pz_Cmd)
{
   boolean     v_Status = FALSE;
   uint32      q_RetVal = LOC_API_GENERAL_FAILURE;

   if(NULL != pz_Cmd)
   {
      LocMW_LocCmdEnumT e_CmdType = pz_Cmd->e_CmdType;
      loc_client_handle_type l_ClientHandle = pz_Cmd->l_ClientHandle;
      pdsm_lcs_notification_resp_s_type* pz_Resp = (pdsm_lcs_notification_resp_s_type *)pz_Cmd->p_QmiEventIndMsg;

      switch (e_CmdType)
      {
         case LOC_CMD_TYPE_SILENTLY_DENY_NI_REQUEST:
            if(NULL != pz_Resp)
            {
                v_Status = pdsm_lcs_agent_client_response(locNi_CmdCb,
                                                       (void *) l_ClientHandle,
                                                       pz_Resp,
                                                       loc_middleware_data.ni_client_id );
                LOC_MSG_MED ("pdsm_lcs_agent_client_response returned %d", v_Status, 0, 0);
                if(TRUE == v_Status)
                {
                    q_RetVal = LOC_API_SUCCESS;
                }
                else
                {
                    // something wrong with the response handling
                    q_RetVal = LOC_API_GENERAL_FAILURE;
                }
            }
            else
            {
                // internal error
                q_RetVal = LOC_API_GENERAL_FAILURE;
            }
            break;
        default:
            // internal error
            q_RetVal = LOC_API_UNSUPPORTED;
            break;
      }
   }
   else
   {
       // internal error
       q_RetVal = LOC_API_GENERAL_FAILURE;
   }

   if(LOC_API_SUCCESS != q_RetVal)
   {
       LOC_MSG_ERROR ("locNi_ProcessCmdRequest internal error", 0, 0, 0);
   }
   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC request from client that belongs to NI module

@param[in] l_ClientHandle  : Loc API client handle
@param[in] q_QmiLocMsgId   : QMI_LOC Request Message ID
@param[in] p_QmiLocMsgData : QMI_LOC Request Message Data
@param[in] q_QmiLocMsgLen  : QMI_LOC Request Message Data Length

@retval    TRUE   Called successfully
@retval    FALSE  Called unsuccessfully
---------------------------------------------------------------------------*/
boolean locNi_ProcessQmiRequest
(
  loc_client_handle_type l_ClientHandle,
  uint32                 q_QmiLocMsgId,
  const void*            p_QmiLocMsgData,
  uint32                 q_QmiLocMsgLen
)
{
   uint32                 q_RetVal = LOC_API_GENERAL_FAILURE;

   /* Function arguments sanity check */
   if ((NULL == p_QmiLocMsgData) || (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle))
   {
      LOC_MSG_ERROR("locNi_ProcessQmiRequest: NULL pointer or invalid l_ClientHandle %d",
                     l_ClientHandle,0,0);
      return FALSE;
   }

   switch (q_QmiLocMsgId)
   {
      case QMI_LOC_NI_USER_RESPONSE_REQ_V02:
      {
         q_RetVal = locNi_ProcessUserResponse(l_ClientHandle, (qmiLocNiUserRespReqMsgT_v02*)p_QmiLocMsgData);
         break;
      }

      case QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02:
      {
         pdsm_lcs_agent_set_vx_config_s_type z_SetVxConfig = {0};
         qmiLocSetProtocolConfigParametersReqMsgT_v02* p_QmiLocSetProtReq =
               (qmiLocSetProtocolConfigParametersReqMsgT_v02*) p_QmiLocMsgData;

         switch ( p_QmiLocSetProtReq->vxVersion )
         {
         case eQMI_LOC_VX_VERSION_V1_ONLY_V02:
            z_SetVxConfig.vxVersion= PDSM_LCS_AGENT_VERSION_V1_ONLY;
            q_RetVal = LOC_API_SUCCESS;
            break;
      
         case eQMI_LOC_VX_VERSION_V2_ONLY_V02:
            z_SetVxConfig.vxVersion= PDSM_LCS_AGENT_VERSION_V2_ONLY;
            q_RetVal = LOC_API_SUCCESS;
            break;

         default:
            LOC_MSG_ERROR ( "locNi_ProcessQmiRequest, could not parse VxVersion %d", 
                           p_QmiLocSetProtReq->vxVersion, 0, 0);
            q_RetVal = LOC_API_INVALID_PARAMETER;
            break;
         }

         if( LOC_API_SUCCESS == q_RetVal )
         {
            if ( FALSE == pdsm_lcs_set_vx_config(
                           PDSM_CLIENT_TYPE_NI,
                           z_SetVxConfig ) )
            {
               q_RetVal = LOC_API_GENERAL_FAILURE;
               LOC_MSG_ERROR("locNi_ProcessQmiRequest Could not set Vx Config",0,0,0);
            }
         }
         break;
      }

      case QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02:
      {
         if ( TRUE == pdsm_lcs_query_vx_config(
                        PDSM_CLIENT_TYPE_NI) )
         {
            q_RetVal = LOC_API_SUCCESS;
         }
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

   LOC_MSG_ERROR("locNi_ProcessQmiRequest: QMI_LOC 0X%04x returns error=%d", q_QmiLocMsgId, q_RetVal, 0);
   return FALSE;
}

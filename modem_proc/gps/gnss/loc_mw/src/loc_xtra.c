/******************************************************************************
  @file: loc_xtra.c
  @brief:   This module handles gpsone XTRA related features .
  

  DESCRIPTION
   Qualcomm Location API XTRA Module

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
 
$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_xtra.c#1 $
$DateTime: 2020/07/21 21:23:44 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
05/23/17   yh       XTRA client 2.5
12/09/16   yh       Fix for over boundary memory reading in memescpy
11/15/16   yh       Remove big local varialbe in locXtra_SendQmiIndicationProxy
07/05/16   yh       Report Constellation Mask
06/18/16   yh       Fix UTC time convert bug
05/19/16   yh       LocMW refactoring
04/15/14   yh       XTRA version check added   
04/27/11   spn      Updated for Loc-Api 2.0
======================================================================*/



#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "msg.h"
#include "queue.h"

#include "aries_os_api.h"
#include "pdapi_lite.h"
#include "pdsm_atl.h"
#include "loc_api_2.h"

#include "loc_client.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_sm_interface.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"
#include "loc_qmi_shim.h"
#include "gnss_common.h"  //GNSS_XTRA_INJ_MASK
#include "loc_utils.h"

// Assembly xtra file
#define MAX_XTRA_FILE_SIZE   (100*1024)
#define MAX_XTRA_PART_SIZE   (10*1024)

// Number of seconds between January 1, 1970 and January 6, 1980
#define DELTA_UTC_TO_GPS_SECONDS        (3657 * 24 * 60 * 60)
#define XTRA_OPER_DEFAULT_TIMEOUT       (1 * 1000)  // 1 second
#define XTRA_DATA_INJECT_TIMEOUT        (60 * 1000)  // 60 seconds

static void locXtra_EventCb
(
   void                        *user_data,
   pdsm_lite_xtra_event_type         xtra_event,
   const pdsm_lite_xtra_info_s_type *xtra_info_ptr
);

static void locXtra_CmdCb
(
   void                     *user_data,
   pdsm_lite_xtra_cmd_e_type      xtra_cmd,
   pdsm_lite_xtra_cmd_err_e_type  xtra_cmd_err
);

static void locXtra_ProcessDownloadEvent
(
   const pdsm_lite_xtra_download_info_s_type* pz_XtraDloadInfo
);

#if 0
static void locXtra_ProcessDataValidityEvent
(
   const pdsm_lite_xtra_download_info_s_type* pz_XtraDloadInfo
);
#endif

static void locXtra_ProcessTimeDownloadEvent
(
   const pdsm_lite_xtra_time_info_req_s_type*  pz_XtraTimeInfo
);

static void locXtra_ProcessStatusEvent
(
   const pdsm_lite_xtra_info_s_type* pz_XtraInfo
);

static int locXtra_InjectData
(
   loc_client_handle_type                            l_ClientHandle,
   const qmiLocInjectXtraDataReqMsgT_v02*            pz_XtraData
);

static int locXtra_GetDataSrc      (void);
#if 0
static int locXtra_GetDataValidity (void);
#endif
static int locXtra_GetConfigInfo   (void);

static int locXtra_InjectUtcTime
(
  loc_client_handle_type                l_ClientHandle,
  const qmiLocInjectUtcTimeReqMsgT_v02* pz_UtcTime
);

static void locXtra_QueueDataInjectRequest
(
  const pdsm_lite_xtra_info_s_type* pz_XtraInfo
);

static void locXtra_ResetModule (void);

static int locXtra_QueryXtraInfo
(
  loc_client_handle_type                l_ClientHandle,
  const qmiLocQueryXtraInfoReqMsgT_v02* pz_QueryXtraInfo
);

static void locXtra_ProcessConfigEvent
(
  const pdsm_lite_xtra_config_info_s_type*  pz_XtraConfigInfo
);


// Initialize the xtra module
boolean locXtra_Init (void)
{
   boolean                       v_RetVal = FALSE;
   pdsm_lite_client_status_e_type     status = PDSM_LITE_CLIENT_OK;
   locXtra_ModuleDataType       *xtra_module_data_ptr = NULL;

   do
   {
      loc_middleware_data.xtra_client_id = pdsm_lite_client_init(PDSM_LITE_CLIENT_TYPE_XTRA);

      if (loc_middleware_data.xtra_client_id == -1)
      {
         LOC_MSG_ERROR("locXtra_Init: pdsm_lite_client_init for XTRA failed", 0, 0, 0);
         break;
      }

      status = pdsm_lite_client_xtra_reg(loc_middleware_data.xtra_client_id,
                                     NULL,
                                     locXtra_EventCb,
                                     PDSM_LITE_CLIENT_EVENT_REG,
                                     PDSM_LITE_XTRA_EVENT_MASK,
                                     NULL);
      if (status != PDSM_LITE_CLIENT_OK)
      {
         LOC_MSG_ERROR("locXtra_Init: pdsm_lite_client_xtra_reg failed, error code = %d", status, 0, 0);
         break;
      }

      status = pdsm_lite_client_act(loc_middleware_data.xtra_client_id);
      if (status != PDSM_LITE_CLIENT_OK)
      {
         LOC_MSG_ERROR ("locXtra_Init: pdsm_lite_client_act for xtra failed, error code =%d", status, 0, 0);
         break;
      }

      // xtra module data has been zero-initialized in loc_task.c
      xtra_module_data_ptr = &(loc_middleware_data.z_locXtra_Data);

      // Allocate the memory for the XTRA data when needed and free it when not needed.
      xtra_module_data_ptr->pb_XtraData = NULL;

      // Have a timer to reset xtra state if expected callback does not come back
      xtra_module_data_ptr->pz_XtraProcessTimer = os_TimerCreate(LOC_MIDDLEWARE_TIMER_ID_XTRA, THREAD_ID_LOC_MIDDLEWARE);

      locXtra_ResetModule();
      v_RetVal = TRUE;
   } while (0);

   LOC_MSG_MED("locXtra_Init: returned %d", v_RetVal, 0, 0);
   return v_RetVal;
}

/* Function to process QMI_LOC XTRA related requests */
boolean locXtra_ProcessQmiRequest
(
   loc_client_handle_type l_ClientHandle,
   uint32                 q_QmiLocMsgId,
   const void*            p_QmiLocMsgData,
   uint32                 q_QmiLocMsgLen
)
{
   locXtra_ModuleDataType* pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);
   uint32                  q_RetVal      = LOC_API_UNSUPPORTED;

   LOC_MSG_MED("locXtra_ProcessQmiRequest: ClientHandle=%d, QmiLocMsgId=0x%04X, QmiLocMsgLen=%d", l_ClientHandle, q_QmiLocMsgId, q_QmiLocMsgLen);

   /* Function arguments sanity check */
   if (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle)
   {
      LOC_MSG_ERROR("locXtra_ProcessQmiRequest: invalid clientHandle", 0,0,0);
      return FALSE;
   }

   LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);
   if ((!pz_LocXtraData->v_XtraCmdInProgress) ||
      ((pz_LocXtraData->e_XtraCmd == PDSM_LITE_XTRA_CMD_SET_DATA) &&
         (QMI_LOC_INJECT_XTRA_DATA_REQ_V02 == q_QmiLocMsgId)))
   {
      pz_LocXtraData->v_XtraCmdInProgress = TRUE;
      pz_LocXtraData->l_ClientHandle      = l_ClientHandle;
      pz_LocXtraData->q_QmiLocMsgId       = q_QmiLocMsgId;

      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.xtra_crit_sec);

      switch (q_QmiLocMsgId)
      {
         case QMI_LOC_INJECT_XTRA_DATA_REQ_V02: /* XTRA file injection */
         {
            if(NULL != p_QmiLocMsgData)
            {
               pz_LocXtraData->e_XtraCmd = PDSM_LITE_XTRA_CMD_SET_DATA;
               q_RetVal = locXtra_InjectData(l_ClientHandle, (qmiLocInjectXtraDataReqMsgT_v02*)p_QmiLocMsgData);
            }
            else
            {
               LOC_MSG_ERROR("locXtra_ProcessQmiRequest QMI_LOC_INJECT_XTRA_DATA_REQ NULL p_QmiLocMsgData",0,0,0);
            }
            break;
         }

#if 0
         case QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02: /* Query XTRA data validity */
         {
            pz_LocXtraData->e_XtraCmd = PDSM_LITE_XTRA_CMD_QUERY_DATA_VALIDITY;
            q_RetVal = locXtra_GetDataValidity();
            break;
         }
#endif
         case QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02: /* Query XTRA server and other xtra file info (file size, maximum size can be injected in one call) */
         {
            pz_LocXtraData->e_XtraCmd = PDSM_LITE_XTRA_CMD_INITIATE_DOWNLOAD;
            q_RetVal = locXtra_GetDataSrc();
            break;
         }

         case QMI_LOC_INJECT_UTC_TIME_REQ_V02: /* Inject UTC time */
         {
            if(NULL != p_QmiLocMsgData)
            {
               pz_LocXtraData->e_XtraCmd = PDSM_LITE_XTRA_CMD_TIME_INFO_INJ;
               q_RetVal = locXtra_InjectUtcTime(l_ClientHandle, (qmiLocInjectUtcTimeReqMsgT_v02*)p_QmiLocMsgData);
            }
            else
            {
               LOC_MSG_ERROR("locXtra_ProcessQmiRequest QMI_LOC_INJECT_UTC_TIME_REQ NULL p_QmiLocMsgData",0,0,0);
            }
            break;
         }

         case QMI_LOC_QUERY_XTRA_INFO_REQ_V02:
         {
            if(NULL != p_QmiLocMsgData)
            {
               pz_LocXtraData->e_XtraCmd = PDSM_LITE_XTRA_CMD_QUERY_XTRA_CFG_INFO;
               q_RetVal = locXtra_QueryXtraInfo(l_ClientHandle, (qmiLocQueryXtraInfoReqMsgT_v02*)p_QmiLocMsgData);
            }
            else
            {
               LOC_MSG_ERROR("locXtra_ProcessQmiRequest QMI_LOC_QUERY_XTRA_INFO_REQ NULL p_QmiLocMsgData",0,0,0);
            }
            break;
         }

         case QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02: /* Query XTRA data validity */
         default:
            q_RetVal = LOC_API_UNSUPPORTED;
            break;
      }

      if (q_RetVal != LOC_API_SUCCESS)
      {
         locXtra_ResetModule();
      }
   }
   else
   {
      LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);

      LOC_MSG_ERROR("locXtra_ProcessQmiRequest: XTRA cmd=%d in progress", pz_LocXtraData->e_XtraCmd, 0, 0);
      q_RetVal = LOC_API_ENGINE_BUSY;
   }

   if( LOC_API_SUCCESS == q_RetVal)
   {
      return TRUE;
   }
   else
   {
      LOC_MSG_ERROR("locXtra_ProcessQmiRequest: QMI_LOC 0x%04x returns error=%d", q_QmiLocMsgId, q_RetVal, 0);
      return FALSE;
   }
}

// Function to process ioctl calls from the location middleware task.
int locXtra_ProcessCmdRequest (const LocMW_LocCmdStructT * pz_Cmd)
{
   boolean                      v_Status = FALSE;
   uint32                       q_RetVal = LOC_API_GENERAL_FAILURE;
   uint32                       q_XtraDataLen = 0;
   locXtra_ModuleDataType*      pz_XtraModuleData = NULL;

   if(NULL == pz_Cmd)
   {
      LOC_MSG_ERROR ("locXtra_ProcessCmdRequest NULL cmd", 0, 0, 0);
      return q_RetVal;
   }

   if( LOC_CMD_TYPE_INJECT_XTRA_DATA != pz_Cmd->e_CmdType)
   {
      LOC_MSG_ERROR ("locXtra_ProcessCmdRequest unsupported CmdType = %d", 
                     pz_Cmd->e_CmdType, 0, 0);
      return q_RetVal;
   }

   pz_XtraModuleData =
      (locXtra_ModuleDataType *)os_MemAlloc(
         sizeof(*pz_XtraModuleData),OS_MEM_SCOPE_TASK);

   if (NULL != pz_XtraModuleData)
   {
      //make a local copy of the xtra_module data 
      LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.xtra_crit_sec);

      memscpy(pz_XtraModuleData, sizeof(*pz_XtraModuleData),
               &(loc_middleware_data.z_locXtra_Data), sizeof(loc_middleware_data.z_locXtra_Data));

      LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.xtra_crit_sec);

      if ((pz_XtraModuleData->v_XtraCmdInProgress) &&
            (pz_XtraModuleData->e_XtraCmd == PDSM_LITE_XTRA_CMD_SET_DATA))
      {
         q_XtraDataLen = pz_XtraModuleData->q_NumOfBytesReceived -
            pz_XtraModuleData->q_NumOfBytesInjected;

         if (q_XtraDataLen > MAX_XTRA_PART_SIZE)
         {
            q_XtraDataLen = MAX_XTRA_PART_SIZE;
         }

         LOC_MSG_MED ("locXtra_ProcessCmdRequest, data inject: "
                        "total part = %d, part number = %d, num of bytes = %d",
                        pz_XtraModuleData->q_TotalPartsToInject, 
                        pz_XtraModuleData->q_NumOfPartsInjected, 
                        q_XtraDataLen);
         v_Status = pdsm_lite_xtra_set_data (locXtra_CmdCb,
                                       loc_middleware_data.xtra_client_id,
                                       (void *) pz_Cmd->l_ClientHandle,
                                       q_XtraDataLen,
                                       (unsigned char *)(pz_XtraModuleData->pb_XtraData +
                                       pz_XtraModuleData->q_NumOfBytesInjected),
                                       pz_XtraModuleData->q_NumOfPartsInjected,
                                       pz_XtraModuleData->q_TotalPartsToInject,
                                       PDSM_LITE_XTRA_STATUS_SUCCESS);

         // In case of failure, nofity this through IOCTL status
         if (FALSE == v_Status)
         {
            qmiLocInjectXtraDataIndMsgT_v02* pz_InjectXtraDataInd =
               (qmiLocInjectXtraDataIndMsgT_v02 *)loc_calloc(sizeof(*pz_InjectXtraDataInd));

            if (NULL == pz_InjectXtraDataInd)
            {
               LOC_MSG_ERROR("locXtra_ProcessCmdRequest: calloc return NULL", 0, 0, 0);
            }
            else
            {
               pz_InjectXtraDataInd->partNum_valid = TRUE;
               pz_InjectXtraDataInd->partNum = pz_XtraModuleData->q_NumOfPartsInjected;
               locMW_SendQmiIndicationProxy(
                  pz_XtraModuleData->l_ClientHandle,
                  pz_XtraModuleData->q_QmiLocMsgId, 
                  pz_InjectXtraDataInd,
                  LOC_API_GENERAL_FAILURE);
            }
            locXtra_ResetModule();
            q_RetVal = LOC_API_GENERAL_FAILURE;
         }
         else
         {
            LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.xtra_crit_sec);
            // parts index starts at 1
            loc_middleware_data.z_locXtra_Data.q_NumOfPartsInjected++;
            loc_middleware_data.z_locXtra_Data.q_NumOfBytesInjected += q_XtraDataLen;
            q_RetVal= LOC_API_SUCCESS;
            LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.xtra_crit_sec);
         }
      }
      os_MemFree((void**) &pz_XtraModuleData);
   }

   if(LOC_API_SUCCESS != q_RetVal)
   {
      LOC_MSG_ERROR ("locXtra_ProcessCmdRequest internal error q_RetVal =%u", q_RetVal, 0, 0);
   }
   return q_RetVal;
}

/* Process QMI_LOC Request for XTRA (Predicted Orbits) data injection. */
static int locXtra_InjectData
(
  loc_client_handle_type                            l_ClientHandle,
  const qmiLocInjectXtraDataReqMsgT_v02*            pz_XtraData
)
{
   locXtra_ModuleDataType* pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);
   LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);
  /* Function arguments sanity check */
   if(NULL == pz_XtraData)
   {
      LOC_MSG_ERROR("locXtra_InjectData: NULL pz_XtraDaa", 0,0,0);
	  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);
	  return LOC_API_INVALID_PARAMETER;
   }

   LOC_MSG_MED("locXtra_InjectData: TotalParts=%d, PartNum=%d, PartDataLenBytes=%d",
               pz_XtraData->totalParts, pz_XtraData->partNum, pz_XtraData->partData_len);

   if ((pz_LocXtraData->v_XtraCmdInProgress) &&
      (pz_LocXtraData->e_XtraCmd == PDSM_LITE_XTRA_CMD_SET_DATA))
   {
      if (NULL == pz_LocXtraData->pb_XtraData)
      {
         /* The buffer for XTRA data is not yet allocated.
            Allocate the buffer for the XTRA data in LOC_MW to copy the injected XTRA file */
         pz_LocXtraData->pb_XtraData = (char*)os_MemAlloc(MAX_XTRA_FILE_SIZE, OS_MEM_SCOPE_TASK);
         if (NULL == pz_LocXtraData->pb_XtraData)
         {
            /* Memory allocation failed */
            LOC_MSG_ERROR("locXtra_InjectData: Allocating buffer for xtra file failed", 0, 0, 0);
            LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);
            return LOC_API_GENERAL_FAILURE;
         }
      }
      /* First part injection, initialize the variables */
      if (pz_XtraData->partNum == 1)
      {
         pz_LocXtraData->q_TotalPartsToReceive = pz_XtraData->totalParts;
         pz_LocXtraData->q_TotalBytesToReceive = pz_XtraData->totalSize;
         pz_LocXtraData->q_NumOfPartsReceived  = 0;
         pz_LocXtraData->q_NumOfBytesReceived  = 0;
         memset(pz_LocXtraData->pb_XtraData, 0, MAX_XTRA_FILE_SIZE);

         /* Set up the timer, all parts of the file need to be injected before the timeout period expires */
         os_TimerStart(pz_LocXtraData->pz_XtraProcessTimer, XTRA_DATA_INJECT_TIMEOUT, 0);
      }

      /* Return error for out of order bytes or xtra file is too big for the buffer */
      if ((pz_LocXtraData->q_NumOfPartsReceived  != (pz_XtraData->partNum-1)) ||
         (pz_LocXtraData->q_TotalBytesToReceive != pz_XtraData->totalSize)   ||
         ((pz_LocXtraData->q_NumOfBytesReceived + pz_XtraData->partData_len) > MAX_XTRA_FILE_SIZE))
      {
         LOC_MSG_ERROR("locXtra_InjectData: Cannot handle out of order part or xtra file too big", 0, 0, 0);
         LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);
         return LOC_API_INVALID_PARAMETER;
      }
      else
      {
         memscpy(pz_LocXtraData->pb_XtraData + pz_LocXtraData->q_NumOfBytesReceived,
                  pz_XtraData->partData_len, pz_XtraData->partData, pz_XtraData->partData_len);
         pz_LocXtraData->q_NumOfBytesReceived += pz_XtraData->partData_len;
         pz_LocXtraData->q_NumOfPartsReceived++;

         /* We have received all parts, queue the request to inject the file to the engine */
         if (pz_LocXtraData->q_TotalPartsToReceive == pz_LocXtraData->q_NumOfPartsReceived)
         {
            if (pz_LocXtraData->q_TotalBytesToReceive == pz_LocXtraData->q_NumOfBytesReceived)
            {
               pz_LocXtraData->q_TotalPartsToInject = pz_LocXtraData->q_NumOfBytesReceived / MAX_XTRA_PART_SIZE;
               if ((pz_LocXtraData->q_NumOfBytesReceived % MAX_XTRA_PART_SIZE) != 0)
               {
                  pz_LocXtraData->q_TotalPartsToInject++;
               }

               loc_middleware_data.z_locXtra_Data.q_NumOfPartsInjected = 1;
               loc_middleware_data.z_locXtra_Data.q_NumOfBytesInjected = 0;
               locXtra_QueueDataInjectRequest(NULL);
            }
            else
            {
               LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);
               return LOC_API_INVALID_PARAMETER;
            }
         }
         /* Generate Indications for intermediate Parts */
         else
         {
            qmiLocInjectXtraDataIndMsgT_v02* pz_InjectXtraDataInd =
               (qmiLocInjectXtraDataIndMsgT_v02 *)loc_calloc(sizeof(*pz_InjectXtraDataInd));

            if (NULL == pz_InjectXtraDataInd)
            {
               LOC_MSG_ERROR("locXtra_InjectData: calloc return NULL", 0, 0, 0);
            }
            else
            {
               pz_InjectXtraDataInd->partNum_valid = TRUE;
               pz_InjectXtraDataInd->partNum = pz_XtraData->partNum;
               /* Send IPC message to LocMW task to handle QMI_LOC indication */
               locMW_SendQmiIndicationProxy(l_ClientHandle,
                                             QMI_LOC_INJECT_XTRA_DATA_IND_V02,
                                             pz_InjectXtraDataInd, LOC_API_SUCCESS);
               /* call locXtra_ResetModule() only for the last QMI_LOC_INJECT_XTRA_DATA_IND_V02 */
            }
         }
      }
   }

   LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);
   return LOC_API_SUCCESS;
}

/*---------------------------------------------------------------------------
@brief
  Function to send IPC to LocMW task to
  either inject the next part of xtra file to SM
  or send the QMI_LOC_INJECT_XTRA_DATA_IND message to the client when 
  all parts of xtra files have been injected

@param[in] pz_XtraInfo : SM callback data for xtra file injection

@retval    None
---------------------------------------------------------------------------*/
static void locXtra_QueueDataInjectRequest
(
  const pdsm_lite_xtra_info_s_type* pz_XtraInfo
)
{
   locXtra_ModuleDataType* pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);

   LOC_MSG_HIGH ("locXtra_QueueDataInjectRequest, bytes injected = %d, total bytes =%d\n",
                 pz_LocXtraData->q_NumOfBytesInjected, pz_LocXtraData->q_NumOfBytesReceived, 0);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.xtra_crit_sec);

   if ((pz_LocXtraData->v_XtraCmdInProgress) &&
       (pz_LocXtraData->e_XtraCmd == PDSM_LITE_XTRA_CMD_SET_DATA))
   {
      if (pz_LocXtraData->q_NumOfBytesInjected < pz_LocXtraData->q_NumOfBytesReceived)
      {
         // no resource to reclaim if this operation failed
         locMW_SendLocCmdProxy(pz_LocXtraData->l_ClientHandle,
                                 LOC_CMD_TYPE_INJECT_XTRA_DATA,
                                 0,
                                 NULL);
      }
      else
      {
         /* All parts have been injected successfully */
         /* Send IPC message to LocMW task to handle QMI_LOC indication */
         qmiLocInjectXtraDataIndMsgT_v02*   pz_InjectXtraDataInd =
            (qmiLocInjectXtraDataIndMsgT_v02 *)loc_calloc(sizeof(*pz_InjectXtraDataInd));

         if (NULL == pz_InjectXtraDataInd)
         {
            LOC_MSG_ERROR("locXtra_QueueDataInjectRequest: calloc return NULL", 0, 0, 0);
         }
         else
         {
            pz_InjectXtraDataInd->partNum_valid = TRUE;
            pz_InjectXtraDataInd->partNum = pz_LocXtraData->q_NumOfPartsReceived;
            if( pz_XtraInfo && 
                (XTRA_DOWNLOAD_STATUS_SUCCESS == pz_XtraInfo->xtra_union.xtra_download_info.status))
            {
               uint32 q_XtraInjMask = pz_XtraInfo->xtra_union.xtra_download_info.value;
               pz_InjectXtraDataInd->ConstellationMask_valid = TRUE;
               if(q_XtraInjMask & GNSS_XTRA_INJ_MASK_GPS)
               {
                  pz_InjectXtraDataInd->ConstellationMask |= eQMI_SYSTEM_GPS_V02;
               }
               if(q_XtraInjMask & GNSS_XTRA_INJ_MASK_GLO)
               {
                  pz_InjectXtraDataInd->ConstellationMask |= eQMI_SYSTEM_GLO_V02;
               }
               if(q_XtraInjMask & GNSS_XTRA_INJ_MASK_BDS)
               {
                  pz_InjectXtraDataInd->ConstellationMask |= eQMI_SYSTEM_BDS_V02;
               }
               if(q_XtraInjMask & GNSS_XTRA_INJ_MASK_GAL)
               {
                  pz_InjectXtraDataInd->ConstellationMask |= eQMI_SYSTEM_GAL_V02;
               }
               if(q_XtraInjMask & GNSS_XTRA_INJ_MASK_QZSS)
               {
                  pz_InjectXtraDataInd->ConstellationMask |= eQMI_SYSTEM_QZSS_V02;
               }
            }
            locMW_SendQmiIndicationProxy(pz_LocXtraData->l_ClientHandle,
                                          QMI_LOC_INJECT_XTRA_DATA_IND_V02,
                                          pz_InjectXtraDataInd, LOC_API_SUCCESS);
         }

         // Release the XTRA buffer in LOC_MW allocated to the XTRA file .It is allocated when it is needed. 
         LOC_MSG_HIGH ("locXtra_QueueDataInjectRequest, releasing XTRA buffer in LOC_MW: %p ",
                        pz_LocXtraData->pb_XtraData,
                        0,
                        0);
         os_MemFree((void**)&pz_LocXtraData->pb_XtraData);
         pz_LocXtraData->pb_XtraData = NULL;
         // Reset the state since xtra injection is now completed
         locXtra_ResetModule();
      }
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.xtra_crit_sec);

}

/* Process QMI_LOC Request for injecting UTC Time. */
int locXtra_InjectUtcTime
(
  loc_client_handle_type                l_ClientHandle,
  const qmiLocInjectUtcTimeReqMsgT_v02* p_UtcTime
)
{
   locXtra_ModuleDataType*  pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);
   pdsm_lite_xtra_time_info_type z_XtraTimeInfo = {0};
   uint64                   t_temp = DELTA_UTC_TO_GPS_SECONDS;
   uint32                   q_lowTime=0, q_highTime=0;

  /* UTC time conversion */
   t_temp *= 1000;
   z_XtraTimeInfo.d_TimeMsec     = p_UtcTime->timeUtc - t_temp;
   z_XtraTimeInfo.f_TimeUncMsec  = p_UtcTime->timeUnc;
   z_XtraTimeInfo.b_RefToUtcTime = TRUE;
   z_XtraTimeInfo.b_ForceFlag    = FALSE;

   q_lowTime = (uint32)(z_XtraTimeInfo.d_TimeMsec);
   q_highTime = (uint32)(z_XtraTimeInfo.d_TimeMsec >> 32);

   LOC_MSG_LOW("locXtra_InjectUtcTime, q_highTime=%u, q_lowTime=%u, unc msec=%u",
                q_highTime, q_lowTime,z_XtraTimeInfo.f_TimeUncMsec);
   os_TimerStart(pz_LocXtraData->pz_XtraProcessTimer, XTRA_OPER_DEFAULT_TIMEOUT, 0);

   if (pdsm_lite_xtra_inject_time_info(locXtra_CmdCb,
                                  loc_middleware_data.xtra_client_id,
                                  (void*)pz_LocXtraData->l_ClientHandle,
                                  &z_XtraTimeInfo))
   {
     return LOC_API_SUCCESS;
   }
   else
   {
     return LOC_API_GENERAL_FAILURE;
   }
}

/* Process QMI_LOC Request for XTRA (Predicted Orbits) data validity. */
#if 0
static int locXtra_GetDataValidity (void)
{
  locXtra_ModuleDataType* pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);

  LOC_MSG_MED("locXtra_GetDataValidity called", 0, 0, 0);
  os_TimerStart(pz_LocXtraData->pz_XtraProcessTimer, XTRA_OPER_DEFAULT_TIMEOUT, 0);

  if (pdsm_lite_xtra_query_data_validity(locXtra_CmdCb,
                                    loc_middleware_data.xtra_client_id,
                                    (void*)pz_LocXtraData->l_ClientHandle))
  {
    return LOC_API_SUCCESS;
  }
  else
  {
    return LOC_API_GENERAL_FAILURE;
  }
}
#endif

/* Process QMI_LOC Request for XTRA (Predicted Orbits) data validity. */
static int locXtra_GetConfigInfo (void)
{
  locXtra_ModuleDataType* pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);
    pdsm_lite_xtra_config_req_info_type z_CfgReqInfo = {0};
  LOC_MSG_MED("locXtra_GetDataValidity called", 0, 0, 0);


  z_CfgReqInfo.config_mask = 0x01;
  LOC_MSG_MED("locXtra_GetDataValidity called", 0, 0, 0);
  os_TimerStart(pz_LocXtraData->pz_XtraProcessTimer, XTRA_OPER_DEFAULT_TIMEOUT, 0);

  if (pdsm_lite_xtra_query_config_info(locXtra_CmdCb,
                                    loc_middleware_data.xtra_client_id,
                                    ((void*)pz_LocXtraData->l_ClientHandle),
                                    &z_CfgReqInfo))
  {
    return LOC_API_SUCCESS;
  }
  else
  {
    return LOC_API_GENERAL_FAILURE;
  }
}

/* Process QMI_LOC Request for XTRA (Predicted Orbits) file download. */
static int locXtra_GetDataSrc (void)
{
  locXtra_ModuleDataType* pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);

  LOC_MSG_MED("locXtra_GetDataSrc called", 0, 0, 0);
  os_TimerStart(pz_LocXtraData->pz_XtraProcessTimer, XTRA_OPER_DEFAULT_TIMEOUT, 0);

  if (pdsm_lite_xtra_client_initiate_download_request(locXtra_CmdCb,
                                                 loc_middleware_data.xtra_client_id,
                                                 (void*)pz_LocXtraData->l_ClientHandle))
  {
    return LOC_API_SUCCESS;
  }
  else
  {
    return LOC_API_GENERAL_FAILURE;
  }
}

static uint32 locXtra_ConvertXtraStatusToLocStatus(pdsm_lite_xtra_status_e_type e_XtraCmdStatus)
{
   uint32                  q_RetVal;

   switch(e_XtraCmdStatus)
   {
      case XTRA_DOWNLOAD_STATUS_SUCCESS:
         q_RetVal = LOC_API_SUCCESS;
         break;
      case XTRA_DOWNLOAD_STATUS_GPS_BUSY:
         q_RetVal = LOC_API_ENGINE_BUSY;
         break;

      case XTRA_DOWNLOAD_STATUS_BAD_CRC:
      case XTRA_DOWNLOAD_STATUS_BAD_BUFFER_LENGTH:
      case XTRA_DOWNLOAD_STATUS_TOA_BAD_RANGE:
         q_RetVal = LOC_API_INVALID_PARAMETER;
         break;

      case XTRA_DOWNLOAD_STATUS_VERSION_MISMATCH:
         q_RetVal =LOC_API_XTRA_VERSION_CHECK_FAILURE;
         break;

      case XTRA_DOWNLOAD_STATUS_GPS_LOCK:
         q_RetVal =LOC_API_GNSS_DISABLED;
         break;

      default:
         q_RetVal = LOC_API_GENERAL_FAILURE;
         break;
   }
   return q_RetVal;
}

// Process xtra callback regarding xtra data and time injection.
static void locXtra_ProcessStatusEvent (  const pdsm_lite_xtra_info_s_type* pz_XtraInfo)
{
   locXtra_ModuleDataType* pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);
   uint32                  q_RetVal;
   pdsm_lite_xtra_status_e_type e_XtraCmdStatus;

   if(NULL == pz_XtraInfo)
   {
      LOC_MSG_ERROR("NULL pointer",0,0,0);
      return;
   }

   e_XtraCmdStatus = pz_XtraInfo->xtra_union.xtra_download_info.status;
   LOC_MSG_HIGH("locXtra_ProcessStatusEvent, e_XtraCmd = %d, status = %d",
                 pz_LocXtraData->e_XtraCmd, e_XtraCmdStatus, 0);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.xtra_crit_sec);
   if (!pz_LocXtraData->v_XtraCmdInProgress)
   {
      LOC_MSG_MED ("locXtra_ProcessStatusEvent, status event received when no xtra cmd is active", 0, 0, 0);
   }
   else
   {
      // Special case for xtra file injection
      if ((pz_LocXtraData->e_XtraCmd == PDSM_LITE_XTRA_CMD_SET_DATA) &&
          (e_XtraCmdStatus == XTRA_DOWNLOAD_STATUS_SUCCESS))
      {
         locXtra_QueueDataInjectRequest (pz_XtraInfo);
      }
      else
      {
         qmiLocInjectXtraDataIndMsgT_v02* pz_InjectXtraDataInd =
            (qmiLocInjectXtraDataIndMsgT_v02 *)loc_calloc(sizeof(*pz_InjectXtraDataInd));

         if (NULL == pz_InjectXtraDataInd)
         {
            LOC_MSG_ERROR("locXtra_ProcessStatusEvent: calloc return NULL", 0, 0, 0);
         }
         else
         {
            q_RetVal = locXtra_ConvertXtraStatusToLocStatus(e_XtraCmdStatus);

            /* QMI_LOC_INJECT_XTRA_DATA_REQ_V02
            QMI_LOC_INJECT_UTC_TIME_REQ_V02 Indication sent back here */
            pz_InjectXtraDataInd->partNum_valid = TRUE;
            pz_InjectXtraDataInd->partNum = pz_LocXtraData->q_NumOfPartsInjected;
            locMW_SendQmiIndicationProxy(
                       pz_LocXtraData->l_ClientHandle,
                       pz_LocXtraData->q_QmiLocMsgId, 
                       pz_InjectXtraDataInd, q_RetVal);
         }
         locXtra_ResetModule();
      }
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.xtra_crit_sec);
}

static void locXtra_ProcessQmiDownloadEvent
(
  const pdsm_lite_xtra_download_info_s_type* p_XtraDloadInfo
)
{
   locXtra_ModuleDataType*                        pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);
   int                                            i;
   qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02* pz_XtraDataSrcInd =
      (qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02 *)loc_calloc(sizeof(*pz_XtraDataSrcInd));

   if(NULL != pz_XtraDataSrcInd)
   {
      pz_XtraDataSrcInd->allowedSizes.maxFileSizeInBytes = p_XtraDloadInfo->maxFileSize;
      pz_XtraDataSrcInd->allowedSizes.maxPartSize        = p_XtraDloadInfo->maxFilePartSize;

      pz_XtraDataSrcInd->serverList_valid          = TRUE;
      pz_XtraDataSrcInd->serverList.serverList_len = QMI_LOC_MAX_PREDICTED_ORBITS_SERVERS_V02;

      GNSS_STRLCPY(
            pz_XtraDataSrcInd->serverList.serverList[0].serverUrl,
            p_XtraDloadInfo->xtra_server_primary,
            sizeof(pz_XtraDataSrcInd->serverList.serverList[0].serverUrl));

      GNSS_STRLCPY(
            pz_XtraDataSrcInd->serverList.serverList[1].serverUrl,
            p_XtraDloadInfo->xtra_server_secondary,
            sizeof(pz_XtraDataSrcInd->serverList.serverList[1].serverUrl));

      GNSS_STRLCPY(
            pz_XtraDataSrcInd->serverList.serverList[2].serverUrl,
            p_XtraDloadInfo->xtra_server_tertiary,
            sizeof(pz_XtraDataSrcInd->serverList.serverList[2].serverUrl));

      /* Send IPC message to LocMW task to handle QMI_LOC indication */
      locMW_SendQmiIndicationProxy(pz_LocXtraData->l_ClientHandle,
                                 QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_IND_V02,
                                 pz_XtraDataSrcInd, LOC_API_SUCCESS);
  }
  else
  {
    LOC_MSG_ERROR("locXtra_ProcessQmiDownloadEvent: Failed to allocate memory for QMI_LOC Indication", 0, 0, 0);
  }

  locXtra_ResetModule();

}

// Process the modem request to download xtra file. This can be either:
// (1) This is a response to client issue query xtra download info
// (2) Modem requests xtra file to be downloaded
static void locXtra_ProcessDownloadEvent
(
  const pdsm_lite_xtra_download_info_s_type* p_XtraDloadInfo
)
{
   locXtra_ModuleDataType*                   pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);
   int                                       i;
   boolean                                   result = FALSE;
   qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02 *pz_InjectPredictedOrbitsReqIndMsg = NULL;

   LOC_MSG_HIGH("locXtra_ProcessDownloadEvent: cmd is in progress = %d, pdsm cmd = %d",
                pz_LocXtraData->v_XtraCmdInProgress, pz_LocXtraData->e_XtraCmd, 0);

   LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);
   /* Special case for client query XTRA download server */
   if ((pz_LocXtraData->v_XtraCmdInProgress) &&
      (pz_LocXtraData->e_XtraCmd == PDSM_LITE_XTRA_CMD_INITIATE_DOWNLOAD))
   {
      LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);
      return locXtra_ProcessQmiDownloadEvent(p_XtraDloadInfo);
   }

   pz_InjectPredictedOrbitsReqIndMsg =
      (qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02 *)loc_calloc(sizeof(*pz_InjectPredictedOrbitsReqIndMsg));
   /* Modem triggers xtra file injection */
   if (NULL != pz_InjectPredictedOrbitsReqIndMsg)
   {
      // Modem triggers xtra file injection
      pz_InjectPredictedOrbitsReqIndMsg->allowedSizes.maxFileSizeInBytes =
         p_XtraDloadInfo->maxFileSize;
      pz_InjectPredictedOrbitsReqIndMsg->allowedSizes.maxPartSize =
         p_XtraDloadInfo->maxFilePartSize;

      pz_InjectPredictedOrbitsReqIndMsg->serverList_valid          = TRUE;
      pz_InjectPredictedOrbitsReqIndMsg->serverList.serverList_len = QMI_LOC_MAX_PREDICTED_ORBITS_SERVERS_V02;

      GNSS_STRLCPY(
         pz_InjectPredictedOrbitsReqIndMsg->serverList.serverList[0].serverUrl,
         p_XtraDloadInfo->xtra_server_primary,
         sizeof(pz_InjectPredictedOrbitsReqIndMsg->serverList.serverList[0].serverUrl));

      GNSS_STRLCPY(
         pz_InjectPredictedOrbitsReqIndMsg->serverList.serverList[1].serverUrl,
         p_XtraDloadInfo->xtra_server_secondary,
         sizeof(pz_InjectPredictedOrbitsReqIndMsg->serverList.serverList[1].serverUrl));

      GNSS_STRLCPY(
         pz_InjectPredictedOrbitsReqIndMsg->serverList.serverList[2].serverUrl,
         p_XtraDloadInfo->xtra_server_tertiary,
         sizeof(pz_InjectPredictedOrbitsReqIndMsg->serverList.serverList[2].serverUrl));

      if(TRUE != locMW_SendLocCmdProxy(LOC_HANDLE_REGISTERED_CLIENTS,
                                       LOC_CMD_TYPE_NOTIFY_CLIENT,
                                       QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ_V02,
                                       (void *)pz_InjectPredictedOrbitsReqIndMsg))
      {
         result = FALSE;
      }
      else
      {
         // IPC sent, we're done.
         result = TRUE;
      }

      LOC_MSG_HIGH("locXtra_ProcessDownloadEvent: queue LOC_EVENT_INJECT_XTRA_DATA_REQUEST ", 0, 0, 0);
   }
  else
  {
    // memory allocation error
    result = FALSE;
  }

  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);

  if (!result)
  {
    // TODO: what else can we do?
    LOC_MSG_ERROR("locXtra_ProcessDownloadEvent: processing failed", 0, 0, 0);
  }
}

// Process the modem request to query xtra data validity.
// If the XTRA data is not present, then the start_utc_time and valid_duration_hours will all be 0.
#if 0
static void locXtra_ProcessDataValidityEvent
(
  const pdsm_lite_xtra_download_info_s_type* p_XtraDloadInfo
)
{
  locXtra_ModuleDataType* pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);
  qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02 *pz_XtraDataValidInd =
     (qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02 *)loc_calloc(sizeof(*pz_XtraDataValidInd));

    LOC_MSG_HIGH("locXtra_ProcessDataValidityEvent: start_gps_week=%d, start_gps_minute=%d, valid_duration=%d",
                p_XtraDloadInfo->startGpsWeek, p_XtraDloadInfo->startGpsMinutes, p_XtraDloadInfo->validDurationHours);

  LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);

  if ((pz_LocXtraData->v_XtraCmdInProgress) &&
      (pz_LocXtraData->e_XtraCmd == PDSM_LITE_XTRA_CMD_QUERY_DATA_VALIDITY))
  {
    if(NULL != pz_XtraDataValidInd)
    {
      pz_XtraDataValidInd->validityInfo_valid = TRUE;
      pz_XtraDataValidInd->validityInfo.durationHours  = p_XtraDloadInfo->validDurationHours;
      pz_XtraDataValidInd->validityInfo.startTimeInUTC = DELTA_UTC_TO_GPS_SECONDS + p_XtraDloadInfo->startGpsWeek * 7 * 24 * 60 * 60 +
                                                       p_XtraDloadInfo->startGpsMinutes * 60;

      /* Send IPC message to LocMW task to handle QMI_LOC indication */
      locMW_SendQmiIndicationProxy(pz_LocXtraData->l_ClientHandle,
                                     QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_IND_V02,
                                     pz_XtraDataValidInd, LOC_API_SUCCESS);
    }
    else
    {
      LOC_MSG_ERROR("locXtra_ProcessDataValidityEvent: Failed to allocate memory for QMI_LOC Indication", 0, 0, 0);
    }
    locXtra_ResetModule();
  }
  else
  {
    LOC_MSG_ERROR("locXtra_ProcessDataValidityEvent should not be received, XtraCmdInProgress=%d, e_XtraCmd=%d",
                   pz_LocXtraData->v_XtraCmdInProgress, pz_LocXtraData->e_XtraCmd, 0);
  }
  LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);
}
#endif

/* Process time download event. SM triggers this event when 
   It needs time to be injected to the location engine.
   This event will be formulated as LOC_EVENT_INJECT_TIME_REQUEST and pass it to the loc client. */
static void locXtra_ProcessTimeDownloadEvent (const pdsm_lite_xtra_time_info_req_s_type*  pz_XtraTimeInfo)
{

   qmiLocEventInjectTimeReqIndMsgT_v02* pz_TimeInjInd = NULL;

   if(NULL == pz_XtraTimeInfo)
   {
      LOC_MSG_ERROR ("locXtra_ProcessTimeDownloadEvent, NULL pz_XtraTimeInfo \n",0, 0, 0);
      return;
   }
   if(XTRA_COMMAND_RETRIEVE_TIME_INFO != pz_XtraTimeInfo->command)
   {
      LOC_MSG_ERROR ("locXtra_ProcessTimeDownloadEvent, invalid pdsm_lite_xtra_time_info \n",
                    pz_XtraTimeInfo->command, 0, 0);
      return;
   }

   pz_TimeInjInd = (qmiLocEventInjectTimeReqIndMsgT_v02 *)loc_calloc(sizeof(*pz_TimeInjInd));
   if(NULL != pz_TimeInjInd)
   {
      pz_TimeInjInd->timeServerInfo_valid = TRUE;
      pz_TimeInjInd->timeServerInfo.delayThreshold = pz_XtraTimeInfo->oneway_delay_failover_thresh;

      GNSS_STRLCPY(
         pz_TimeInjInd->timeServerInfo.timeServerList[0].serverUrl,
         pz_XtraTimeInfo->xtra_server_primary,
         sizeof(pz_TimeInjInd->timeServerInfo.timeServerList[0].serverUrl));

      GNSS_STRLCPY(
         pz_TimeInjInd->timeServerInfo.timeServerList[1].serverUrl,
         pz_XtraTimeInfo->xtra_server_secondary,
         sizeof(pz_TimeInjInd->timeServerInfo.timeServerList[1].serverUrl));

      GNSS_STRLCPY(
         pz_TimeInjInd->timeServerInfo.timeServerList[2].serverUrl,
         pz_XtraTimeInfo->xtra_server_tertiary,
         sizeof(pz_TimeInjInd->timeServerInfo.timeServerList[2].serverUrl));

      pz_TimeInjInd->timeServerInfo.timeServerList_len = QMI_LOC_MAX_NTP_SERVERS_V02;

      (void)locMW_SendLocCmdProxy(LOC_HANDLE_REGISTERED_CLIENTS,
                                 LOC_CMD_TYPE_NOTIFY_CLIENT,
                                 QMI_LOC_EVENT_MASK_INJECT_TIME_REQ_V02,
                                 (void *)pz_TimeInjInd);

   }
   else
   {
   // memory allocation error
      LOC_MSG_ERROR ("locXtra_ProcessTimeDownloadEvent, malloc failed 2", 0, 0, 0);
   }
}

// XTRA event callback.
static void locXtra_EventCb (void                        *user_data,
                               pdsm_lite_xtra_event_type         xtra_event,
                               const pdsm_lite_xtra_info_s_type *xtra_info_ptr)
{
   LOC_MSG_HIGH ("locXtra_EventCb, event = %d flags = 0x%x status = %d \n",
                 xtra_event, xtra_info_ptr->xtra_union.xtra_download_info.flags,
                 xtra_info_ptr->xtra_union.xtra_download_info.status);

   switch (xtra_event)
   {
      case PDSM_LITE_XTRA_EVENT_STATUS:
      {
#if 0
         if (((xtra_info_ptr->xtra_union.xtra_download_info.flags & XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_WEEK) &&
               (xtra_info_ptr->xtra_union.xtra_download_info.flags & XTRA_DOWNLOAD_INFO_FLAGS_START_GPS_MINS) &&
               (xtra_info_ptr->xtra_union.xtra_download_info.flags & XTRA_DOWNLOAD_INFO_FLAGS_VALID_DURATION))!= 0)
         {
            locXtra_ProcessDataValidityEvent(&(xtra_info_ptr->xtra_union.xtra_download_info));
         }
         else if ((xtra_info_ptr->xtra_union.xtra_download_info.flags & XTRA_DOWNLOAD_INFO_FLAGS_STATUS) != 0)
#endif
         if ((xtra_info_ptr->xtra_union.xtra_download_info.flags & XTRA_DOWNLOAD_INFO_FLAGS_STATUS) != 0)
         {
            locXtra_ProcessStatusEvent (xtra_info_ptr);
         }
         break;
      }

      case PDSM_LITE_XTRA_EVENT_DOWNLOAD_REQ:
      {
         locXtra_ProcessDownloadEvent(&(xtra_info_ptr->xtra_union.xtra_download_info));
         break;
      }

      case PDSM_LITE_XTRA_EVENT_TIME_REQ:
      {
         locXtra_ProcessTimeDownloadEvent (&(xtra_info_ptr->xtra_union.xtra_time_info));
         break;
      }

      case PDSM_LITE_XTRA_EVENT_CFG_REQ:
      {
         locXtra_ProcessConfigEvent(&(xtra_info_ptr->xtra_union.xtra_cfg_info));
         break;
      }

      default:
      {
         LOC_MSG_ERROR("locXtra_EventCb = %d not expected\n", xtra_event, 0, 0);
      }
   }
}

// XTRA cmd callback. If xtra cmd callback reports error, no further processing of xtra
// cmd will be performed.
static void locXtra_CmdCb (void                     *user_data,
                           pdsm_lite_xtra_cmd_e_type      xtra_cmd_cb,
                           pdsm_lite_xtra_cmd_err_e_type  xtra_cmd_err)
{
   locXtra_ModuleDataType* xtra_module_data_ptr = &(loc_middleware_data.z_locXtra_Data);
   pdsm_lite_xtra_cmd_e_type    xtra_cmd_expected = xtra_module_data_ptr->e_XtraCmd;
   uint32                  q_RetVal = LOC_API_GENERAL_FAILURE;
   void*                   p_XtraIndMsg = NULL;

   LOC_MSG_HIGH ("locXtra_CmdCb: cmd=%d, err=%d, expected cmd=%d\n",
                 xtra_cmd_cb, xtra_cmd_err, xtra_cmd_expected);

   LOC_MW_ENTER_CRIT_SECTION (&loc_middleware_data.xtra_crit_sec);
   if ((!xtra_module_data_ptr->v_XtraCmdInProgress) ||
       (xtra_cmd_expected != xtra_cmd_cb) ||
       (xtra_module_data_ptr->l_ClientHandle != (loc_client_handle_type) user_data))
   {
      LOC_MSG_ERROR ("locXtra_CmdCb unexpected, xtra_in_progress = %d, active client = %d, user_data = %d\n",
                      xtra_module_data_ptr->v_XtraCmdInProgress, xtra_module_data_ptr->l_ClientHandle, user_data);
   }
   else
   {
      if (xtra_cmd_err == PDSM_LITE_XTRA_CMD_ERR_NOERR)
      {
         LOC_MSG_MED ("loc_xra_cmd_cb success, wait for event cb", 0, 0, 0);
      }
      else
      {
         LOC_MSG_ERROR("Error : %d encountered for MsgId : %d", xtra_cmd_err, (xtra_module_data_ptr->q_QmiLocMsgId), 0);
         switch (xtra_cmd_err)
         {
            case PDSM_LITE_XTRA_CMD_ERR_GPS_SUBSYSTEM_BUSY:
            q_RetVal = LOC_API_ENGINE_BUSY;
            break;

            case PDSM_LITE_XTRA_CMD_ERR_BAD_CRC:
            case PDSM_LITE_XTRA_CMD_ERR_TIME_OF_VALIDITY_OUT_OF_RANGE:
            case PDSM_LITE_XTRA_CMD_ERR_DOWNLOAD_INTRVL_OUT_OF_RANGE:
            case PDSM_LITE_XTRA_CMD_ERR_TIME_INFO_ERROR:
            q_RetVal = LOC_API_INVALID_PARAMETER;
            break;

            case PDSM_LITE_XTRA_CMD_ERR_GPS_LOCK_S:
            q_RetVal = LOC_API_GNSS_DISABLED;
            break;

            default:
            q_RetVal = LOC_API_GENERAL_FAILURE;
            break;
         }

         switch (xtra_module_data_ptr->q_QmiLocMsgId)
         {
            case QMI_LOC_INJECT_XTRA_DATA_REQ_V02:
               p_XtraIndMsg = loc_calloc(sizeof(qmiLocInjectXtraDataIndMsgT_v02));
               if (NULL != p_XtraIndMsg)
               {
                  ((qmiLocInjectXtraDataIndMsgT_v02 *)p_XtraIndMsg)->partNum_valid = TRUE;
                  ((qmiLocInjectXtraDataIndMsgT_v02 *)p_XtraIndMsg)->partNum = xtra_module_data_ptr->q_NumOfPartsInjected;
               }
               break;
            
            case QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02:
               p_XtraIndMsg = loc_calloc(sizeof(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02));
               break;

            case QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02:
               p_XtraIndMsg = loc_calloc(sizeof(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02));
               break;

            case QMI_LOC_INJECT_UTC_TIME_REQ_V02:
               /* Do not allocate memory on purpose, because
               qmiLocInjectUtcTimeIndMsgT_v02 doesn't contain
               any data, Only status is reported
               Generic indication will be created in LocQmiShim*/
               break;

            case QMI_LOC_QUERY_XTRA_INFO_REQ_V02:
               p_XtraIndMsg = loc_calloc(sizeof(qmiLocQueryXtraInfoIndMsgT_v02));
               break;

            default:
               LOC_MSG_ERROR("Invalid MsgId encountered in LocMw : %d",(xtra_module_data_ptr->q_QmiLocMsgId), 0, 0);
               break;
         }

         if((NULL != p_XtraIndMsg) || (QMI_LOC_INJECT_UTC_TIME_REQ_V02 == xtra_module_data_ptr->q_QmiLocMsgId))
         {
            locMW_SendQmiIndicationProxy(xtra_module_data_ptr->l_ClientHandle,
                                         xtra_module_data_ptr->q_QmiLocMsgId, 
                                         p_XtraIndMsg, q_RetVal);
         }
         else
         {
            LOC_MSG_ERROR("Calloc Failed or Invalid MsgId encountered in LocMw : %d",(xtra_module_data_ptr->q_QmiLocMsgId), 0, 0);
         }
         locXtra_ResetModule();
      }
   }
   LOC_MW_LEAVE_CRIT_SECTION (&loc_middleware_data.xtra_crit_sec);
}

// When the xtra timer is fired, it means that we have not received response from GPS engine
// notify the client about the timeout value
void locXtra_ProcessTimerCb ( void )
{
   locXtra_ModuleDataType* pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);
   void*                   p_XtraIndMsg = NULL;

   LOC_MSG_ERROR("LocXtra time out for QMI msg 0x%4x ",pz_LocXtraData->q_QmiLocMsgId, 0, 0);

   if (pz_LocXtraData->v_XtraCmdInProgress)
   {
      switch(pz_LocXtraData->q_QmiLocMsgId)
      {
         case QMI_LOC_INJECT_XTRA_DATA_REQ_V02:
            p_XtraIndMsg = loc_calloc(sizeof(qmiLocInjectXtraDataIndMsgT_v02));
            if (NULL != p_XtraIndMsg)
            {
               ((qmiLocInjectXtraDataIndMsgT_v02 *)p_XtraIndMsg)->partNum_valid = TRUE;
               ((qmiLocInjectXtraDataIndMsgT_v02 *)p_XtraIndMsg)->partNum = pz_LocXtraData->q_NumOfPartsInjected;
            }
            break;
         case QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02:
            p_XtraIndMsg = loc_calloc(sizeof(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02));
            break;
         case QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02:
            p_XtraIndMsg = loc_calloc(sizeof(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02));
            break;
         case QMI_LOC_INJECT_UTC_TIME_REQ_V02:
            /* Do not allocate memory on purpose, because 
            qmiLocInjectUtcTimeIndMsgT_v02 doesn't contain real data */
            break;
         case QMI_LOC_QUERY_XTRA_INFO_REQ_V02:
            p_XtraIndMsg = loc_calloc(sizeof(qmiLocQueryXtraInfoIndMsgT_v02));
            break;
         default:
            LOC_MSG_ERROR("locXtra_ProcessTimerCb: q_QmiLocMsgId 0x%4x is not valid",
               pz_LocXtraData->q_QmiLocMsgId, 0, 0);
            locXtra_ResetModule();
            return;
      }

      if( NULL == p_XtraIndMsg && 
          (QMI_LOC_INJECT_UTC_TIME_REQ_V02 != pz_LocXtraData->q_QmiLocMsgId))
      {
         LOC_MSG_ERROR("locXtra_ProcessTimerCb: calloc return NULL", 0, 0, 0);
      }
      else
      {
         // Notify client about the timeout
         locMW_SendQmiIndicationProxy(
                       pz_LocXtraData->l_ClientHandle,
                       pz_LocXtraData->q_QmiLocMsgId, 
                       p_XtraIndMsg, LOC_API_TIMEOUT);
      }
      locXtra_ResetModule();
   }
}

// Reset the xtra module. This function is called when the in-progress xtra cmds finishes.
static void locXtra_ResetModule (void)
{
   locXtra_ModuleDataType* pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);

   LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);

   if (pz_LocXtraData->v_XtraCmdInProgress)
   {
      /* Clear the timer */
      os_TimerStop(pz_LocXtraData->pz_XtraProcessTimer);

      pz_LocXtraData->v_XtraCmdInProgress = FALSE;
      pz_LocXtraData->e_XtraCmd           = PDSM_LITE_XTRA_CMD_NONE;
      pz_LocXtraData->l_ClientHandle      = LOC_CLIENT_HANDLE_INVALID;
      pz_LocXtraData->q_QmiLocMsgId       = 0;

      pz_LocXtraData->q_TotalPartsToReceive = 0;
      pz_LocXtraData->q_TotalBytesToReceive = 0;
      pz_LocXtraData->q_NumOfPartsReceived  = 0;
      pz_LocXtraData->q_NumOfBytesReceived  = 0;

      pz_LocXtraData->q_TotalPartsToInject  = 0;
      pz_LocXtraData->q_NumOfPartsInjected  = 0;
      pz_LocXtraData->q_NumOfBytesInjected  = 0;
   }

   LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.xtra_crit_sec);

   LOC_MSG_MED("locXtra_ResetModule called", 0, 0, 0);
}

/* Process QMI_LOC Request for query XTRA info */
static int locXtra_QueryXtraInfo
(
  loc_client_handle_type                l_ClientHandle,
  const qmiLocQueryXtraInfoReqMsgT_v02* pz_QueryXtraInfo
)
{
   locXtra_ModuleDataType* pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);
   pdsm_lite_xtra_config_req_info_type   z_XtraConfigReqInfo = {0};

   LOC_MSG_MED("locXtra_QueryXtraInfo called", 0, 0, 0);
   os_TimerStart(pz_LocXtraData->pz_XtraProcessTimer, XTRA_OPER_DEFAULT_TIMEOUT, 0);

   z_XtraConfigReqInfo.config_mask = pz_QueryXtraInfo->xtraConfig;

   if (pdsm_lite_xtra_query_config_info(locXtra_CmdCb,
                                    loc_middleware_data.xtra_client_id,
                                    (void*)pz_LocXtraData->l_ClientHandle,
                                    &z_XtraConfigReqInfo))
   {
      return LOC_API_SUCCESS;
   }
   else
   {
      return LOC_API_GENERAL_FAILURE;
   }
}

/* Process config event */
static void locXtra_ProcessConfigEvent
(
  const pdsm_lite_xtra_config_info_s_type*  pz_XtraConfigInfo
)
{
   qmiLocQueryXtraInfoIndMsgT_v02*  pz_QueryXtraInfoInd = NULL;
   locXtra_ModuleDataType*          pz_LocXtraData = &(loc_middleware_data.z_locXtra_Data);
   uint32                           q_RetVal;
   boolean                          v_Ret;

   if(NULL == pz_XtraConfigInfo)
   {
      LOC_MSG_ERROR ("locXtra_ProcessConfigEvent, NULL pz_XtraConfigInfo \n",0, 0, 0);
      return;
   }

   pz_QueryXtraInfoInd = (qmiLocQueryXtraInfoIndMsgT_v02 *)loc_calloc(sizeof(*pz_QueryXtraInfoInd));
   if(NULL != pz_QueryXtraInfoInd)
   {
      q_RetVal = locXtra_ConvertXtraStatusToLocStatus(pz_XtraConfigInfo->status);

      if( pz_XtraConfigInfo->report_mask & XTRA_CFG_INFO_MASK_ABS_AGE )
      {
         pz_QueryXtraInfoInd->xtraInfo.reportMask |= QMI_LOC_XTRA_INFO_MASK_ABS_AGE_V02;
         pz_QueryXtraInfoInd->xtraInfo.absAgeHrs = pz_XtraConfigInfo->abs_age_hrs;
      }

      if( pz_XtraConfigInfo->report_mask & XTRA_CFG_INFO_MASK_REL_AGE )
      {
         pz_QueryXtraInfoInd->xtraInfo.reportMask |= QMI_LOC_XTRA_INFO_MASK_REL_AGE_V02;
         pz_QueryXtraInfoInd->xtraInfo.relAgeInUTC = pz_XtraConfigInfo->rel_age_utc;
      }

      if( pz_XtraConfigInfo->report_mask & XTRA_CFG_INFO_MASK_XTRA_SERVER )
      {
         pz_QueryXtraInfoInd->xtraInfo.reportMask |= QMI_LOC_XTRA_INFO_MASK_XTRA_SERVER_V02;
         GNSS_STRLCPY(
            pz_QueryXtraInfoInd->xtraInfo.xtraServerInfo.serverList[0].serverUrl,
            pz_XtraConfigInfo->xtra_server_primary,
            sizeof(pz_QueryXtraInfoInd->xtraInfo.xtraServerInfo.serverList[0].serverUrl));

         GNSS_STRLCPY(
            pz_QueryXtraInfoInd->xtraInfo.xtraServerInfo.serverList[1].serverUrl,
            pz_XtraConfigInfo->xtra_server_secondary,
            sizeof(pz_QueryXtraInfoInd->xtraInfo.xtraServerInfo.serverList[1].serverUrl));

         GNSS_STRLCPY(
            pz_QueryXtraInfoInd->xtraInfo.xtraServerInfo.serverList[2].serverUrl,
            pz_XtraConfigInfo->xtra_server_tertiary,
            sizeof(pz_QueryXtraInfoInd->xtraInfo.xtraServerInfo.serverList[2].serverUrl));

         pz_QueryXtraInfoInd->xtraInfo.xtraServerInfo.serverList_len = QMI_LOC_MAX_PREDICTED_ORBITS_SERVERS_V02;
      }

      if( pz_XtraConfigInfo->report_mask & XTRA_CFG_INFO_MASK_NTP_SERVER )
      {
         pz_QueryXtraInfoInd->xtraInfo.reportMask |= QMI_LOC_XTRA_INFO_MASK_NTP_SERVER_V02;
         GNSS_STRLCPY(
            pz_QueryXtraInfoInd->xtraInfo.ntpServerInfo.timeServerList[0].serverUrl,
            pz_XtraConfigInfo->xtra_time_server_primary,
            sizeof(pz_QueryXtraInfoInd->xtraInfo.ntpServerInfo.timeServerList[0].serverUrl));

         GNSS_STRLCPY(
            pz_QueryXtraInfoInd->xtraInfo.ntpServerInfo.timeServerList[1].serverUrl,
            pz_XtraConfigInfo->xtra_time_server_secondary,
            sizeof(pz_QueryXtraInfoInd->xtraInfo.ntpServerInfo.timeServerList[1].serverUrl));

         GNSS_STRLCPY(
            pz_QueryXtraInfoInd->xtraInfo.ntpServerInfo.timeServerList[2].serverUrl,
            pz_XtraConfigInfo->xtra_time_server_tertiary,
            sizeof(pz_QueryXtraInfoInd->xtraInfo.ntpServerInfo.timeServerList[2].serverUrl));

         pz_QueryXtraInfoInd->xtraInfo.ntpServerInfo.timeServerList_len = QMI_LOC_MAX_NTP_SERVERS_V02;
      }

      if( pz_XtraConfigInfo->report_mask & XTRA_CFG_INFO_MASK_TIME_REQUEST )
      {
         pz_QueryXtraInfoInd->xtraInfo.reportMask |= QMI_LOC_XTRA_INFO_MASK_TIME_REQUEST_V02;
         pz_QueryXtraInfoInd->xtraInfo.timeRequest = pz_XtraConfigInfo->time_request;
      }

      if( pz_XtraConfigInfo->report_mask & XTRA_CFG_INFO_MASK_PREF_VALID_AGE )
      {
         pz_QueryXtraInfoInd->xtraInfo.reportMask |= QMI_LOC_XTRA_INFO_MASK_PREF_VALID_AGE_V02;
         pz_QueryXtraInfoInd->xtraInfo.preferedValidAgeHrs = pz_XtraConfigInfo->prefered_valid_age_hrs;
      }

      v_Ret = locMW_SendQmiIndicationProxy(
                       pz_LocXtraData->l_ClientHandle,
                       QMI_LOC_QUERY_XTRA_INFO_IND_V02,
                       pz_QueryXtraInfoInd, q_RetVal);
      if ( TRUE != v_Ret )
      {
        LOC_MSG_ERROR ("locXtra_ProcessConfigEvent, failed to send msg to LocMW", 0, 0, 0);
      }
   }
   else
   {
      // memory allocation error
      LOC_MSG_ERROR ("locXtra_ProcessConfigEvent, malloc failed", 0, 0, 0);
   }
   locXtra_ResetModule();
}

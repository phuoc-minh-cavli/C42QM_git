/******************************************************************************
  @file: loc_gdt.c
  @brief:   This module handles gpsone GDT library
  

  DESCRIPTION
   Qualcomm Location API GDT Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  Copyright (c) 2014 QUALCOMM Atheros, Inc.
  All Rights Reserved.
  QCA Proprietary and Confidential. 
 
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
 
$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_gdt.c#1 $
$DateTime: 2020/07/21 21:23:44 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
09/15/14   ah       Intial version
======================================================================*/

#include "loc_api_2.h"
#include "loc_api_internal.h"
#include "loc_utils.h"
#include "loc_qmi_shim.h"
#include "gnss_gdt.h"
#include "tle_api.h"


/*===========================================================================
FUNCTION loc_gdt_SendBeginIndProxy

DESCRIPTION
  This function sends a GDT Begin indication to AP. In other words, MP is
  requesting to start a session by sending this indication.
 
PARAMETERS 
 
  client_handle - LocAPI client handle  
  (input)

  transaction_id  - Transaction id 
  (input)
 
  p_ind  - Points to the QMI indication
  (output)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
 
SIDE EFFECTS

===========================================================================*/
static uint32 loc_gdt_SendBeginIndProxy(const int32 client_handle, 
                                        const uint32 transaction_id, 
                                        const qmiLocEventGdtUploadBeginStatusReqIndMsgT_v02 * const p_ind)
{
  uint32 error_code = 1;
  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_GDT_BEGIN_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_gdt_SendBeginIndProxy: error code:%lu", 
                    error_code, 0, 0);
    }
  }
  return error_code; 
}

/*===========================================================================
FUNCTION loc_gdt_SendEndIndProxy

DESCRIPTION
  This function sends a GDT end indication to AP. In other words, MP is
  requesting to end a session by sending this indication.
 
PARAMETERS 
 
  client_handle - LocAPI client handle  
  (input)

  transaction_id  - Transaction id 
  (input)
 
  p_ind  - Points to the QMI indication
  (output)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
 
SIDE EFFECTS

===========================================================================*/
static uint32 loc_gdt_SendEndIndProxy(const int32 client_handle, 
                                      const uint32 transaction_id, 
                                      const qmiLocEventGdtUploadEndReqIndMsgT_v02 * const p_ind)
{
  uint32 error_code = 1;
  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_GDT_END_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_gdt_SendEndIndProxy: error code:%lu", 
                    error_code, 0, 0);
    }
  }
  return error_code; 
}

/*===========================================================================
FUNCTION loc_gdt_BeginReqIndProxy

DESCRIPTION
  This function sends gdt begin request status indication.
  MP has recievd a request from AP and now this function sends the corresponding
  indication back to AP.
 
PARAMETERS 
 
  client_handle - LocAPI client handle
  (input)

  transaction_id - Same transaction id passed in earlier by the request
  (input)

  p_ind   - Points to the The QMI indication
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
SIDE EFFECTS

===========================================================================*/
uint32 loc_gdt_BeginReqIndProxy
(
  const int32 client_handle, 
  const uint32 transaction_id, 
  const qmiLocGdtUploadBeginStatusIndMsgT_v02 * const p_ind
)
{
  uint32 error_code = 1;
  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_GDT_BEGIN_REQ_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_gdt_BeginReqIndProxy: "
                    "error code: %u", error_code, 0, 0);
    }
  }
  return error_code;
}



/*===========================================================================
FUNCTION loc_gdt_UploadEndStatusReq

DESCRIPTION
  This function handles QMI command to process gdt upload end status request.
  MP has sent a session end to AP, and now the response from AP is recieved
  through this request
 
PARAMETERS 
 
  cpz_Client - Points to the LocAPI client
  (input)

  cpz_Req   - Points to the QMI Request message received from AP
  (input)

  pz_Resp   - Points to the generated response
  (output)
 
DEPENDENCIES

RETURN VALUE 
  LOC_API_SUCCESS:          If the command has been handled successfully
  LOC_API_GENERAL_FAILURE : Otherwise
SIDE EFFECTS

===========================================================================*/
int32 loc_gdt_UploadEndStatusReq(const locQmiShimCltInfoStructT *const cpz_Client,
                                 const qmiLocGdtUploadEndReqMsgT_v02 *const cpz_Req,
                                 qmiLocGenRespMsgT_v02 *const pz_Resp)
{
  int32 result = LOC_API_GENERAL_FAILURE;
  e_GDTReturnCodesType e_GdtResult;

  if ((NULL != cpz_Client) && (NULL != cpz_Req) && (NULL != pz_Resp))
  {
    gdt_EndResponseType z_EndResponse;
    gdt_EndArgumentType z_Response;

    memset(&z_EndResponse, 0x00, sizeof(z_EndResponse));
    z_Response.pz_EndResponse = &z_EndResponse;
    
    LOC_MSG_MED("loc_qmiloc_gdt_upload_end_status_req. client:%d, serv ID:%lu, status:%d",
                 cpz_Client->l_LocClientHandle, cpz_Req->serviceId, (int) cpz_Req->gdtEndStatus);

    memset(pz_Resp, 0, sizeof(*pz_Resp));
    switch (cpz_Req->gdtEndStatus)
    {
      case eQMI_LOC_GDT_ACK_SUCCESS_V02:
        z_EndResponse.e_EndStatus = e_GDT_ACK_SUCCESS;
        break;

      case eQMI_LOC_GDT_ACK_FAILED_V02:
        z_EndResponse.e_EndStatus = e_GDT_ACK_FAILED;
        break;

      case eQMI_LOC_GDT_ACK_INVALID_V02:
      default:
        z_EndResponse.e_EndStatus = e_GDT_ACK_FAILED;
    }
    
    switch (cpz_Req->serviceId)
    {
      case eQMI_LOC_GDT_SERVICE_WWAN_V02:
        z_EndResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_GTP_WWAN;
        break;

#ifdef FEATURE_GNSS_TDP_20
      case eQMI_LOC_GDT_SERVICE_CSM_UL_V02:
        z_EndResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_GTP_CSM_UL;
        break;
#endif /* FEATURE_GNSS_TDP_20 */

      default:
        z_EndResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_INVALID;
    }
     
    z_EndResponse.z_SessionInfo.q_SessId = cpz_Req->sessionId;
    z_EndResponse.l_Client_handle = cpz_Client->l_LocClientHandle;
    z_EndResponse.q_TransactionId = loc_qmiloc_get_transaction_id();

    e_GdtResult = gdt_RcvdSendEndResponse(z_Response);
    if ( e_GDT_SUCCESS == e_GdtResult)
    {
      LOC_MSG_MED("End response send to GDT client",
                  0, 0, 0);
      pz_Resp->resp.error = QMI_ERR_NONE_V01;
      pz_Resp->resp.result = QMI_RESULT_SUCCESS_V01;
    }
    else
    {
      LOC_MSG_HIGH("Failed to send end response to GDT client. err:%d",
                   (int32) e_GdtResult, 0, 0);

      pz_Resp->resp.error = QMI_ERR_GENERAL_V01;
      pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
    }

    result = LOC_API_SUCCESS;
  }
  else
  {
    // this null pointer reference shall never happen
    // the error code indicates pResp is not filled
    result = LOC_API_GENERAL_FAILURE;
  }

  LOC_MSG_MED("GDT End response result:%d", result, 0, 0);
  return result;
}

/*===========================================================================
FUNCTION loc_gdt_BeginIndicationHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocEventGdtUploadBeginStatusReqIndMsgT_v02.
  MP is sending a GDT begin message as an indication to AP.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if ipc has been processed successfully  
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_BeginIndicationHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_gdt_BeginIndicationHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocEventGdtUploadBeginStatusReqIndMsgT_v02 z_Ind;

    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_EVENT_GDT_UPLOAD_BEGIN_STATUS_REQ_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC] loc_gdt_BeginIndicationHandler, client: %d, tx id: %lu",
                   l_ClientHandle, q_TransactionId, 0);

      LOC_MSG_MED("[LOC] loc_gdt_BeginIndicationHandler, serviceId:%d, sessId:%lu, filePathLen:%lu",
                   (int32) z_Ind.serviceId, z_Ind.sessionId, z_Ind.filePath_len);

      
      // send indication through QCSI
      if (locQmiShimBroadcastInd(QMI_LOC_EVENT_MASK_GDT_UPLOAD_BEGIN_REQ_V02, 
                                 &z_IndInfo))
      {
        // done
        q_ErrorCode = 0;
      }
      else
      {
        // failed
        q_ErrorCode = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      q_ErrorCode = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    q_ErrorCode = 4;
  }

  if (0 != q_ErrorCode)
  {
    LOC_MSG_ERROR("[LOC] loc_gdt_BeginIndicationHandler, error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
}

/*===========================================================================
FUNCTION loc_gdt_EndIndicationHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtUploadEndIndMsgT_v02
  MP is sending a GDT end message as an indication to AP.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_EndIndicationHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 l_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_gdt_EndIndicationHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocEventGdtUploadEndReqIndMsgT_v02 z_Ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_EVENT_GDT_UPLOAD_END_REQ_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC] loc_gdt_EndIndicationHandler, client: %d, tx id: %u",
                   l_ClientHandle, q_TransactionId, 0);

      LOC_MSG_MED("[LOC] loc_gdt_EndIndicationHandler, serviceId: %d, sessId: %lu, status: %d",
                   (int32) z_Ind.serviceId, z_Ind.sessionId, (int32) z_Ind.endStatus);

      // send indication through QCSI
      if (locQmiShimBroadcastInd(QMI_LOC_EVENT_MASK_GDT_UPLOAD_END_REQ_V02, &z_IndInfo))
      {
        // done
        l_ErrorCode = 0;
      }
      else
      {
        // failed
        l_ErrorCode = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      l_ErrorCode = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    l_ErrorCode = 4;
  }

  if (0 != l_ErrorCode)
  {
    LOC_MSG_ERROR("[LOC] loc_gdt_EndIndicationHandler, error code: %u", l_ErrorCode, 0, 0);
  }
}

/*===========================================================================
FUNCTION loc_gdt_SendBeginRequest

DESCRIPTION
  This function sends a GDT begin indication to AP.
 
PARAMETERS 
 
  e_ServiceId - GDT Client service ID
  (input)
 
  q_SessionId - GDt session ID
  (input)
 
  q_FilePathLen - The number valid chars in file path
  (input)
 
  c_FilePath  - Points to the file path
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the request, and sent the access request
                 to AP to get the permission
 
  e_GDT_FAIL: Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType loc_gdt_SendBeginRequest(e_GDTServiceIDType e_ServiceId,
                                              uint32 q_SessionId, uint32 q_FilePathLen,
                                              const char * c_FilePath)
{
  qmiLocEventGdtUploadBeginStatusReqIndMsgT_v02 z_BeginInd;
  int32 client_handle = 0;
  uint32 transaction_id = 0;

  if ((NULL == c_FilePath) ||
      (q_FilePathLen >= QMI_LOC_MAX_GDT_PATH_LEN_V02))
  {
    return e_GDT_FAIL;
  }

  z_BeginInd.filePath_len = q_FilePathLen;

  switch (e_ServiceId)
  {
  case e_GDT_SERVICE_GTP_WWAN:
    z_BeginInd.serviceId = eQMI_LOC_GDT_SERVICE_WWAN_V02;
    break;

#ifdef FEATURE_GNSS_TDP_20
  case e_GDT_SERVICE_GTP_CSM_UL:
    z_BeginInd.serviceId = eQMI_LOC_GDT_SERVICE_CSM_UL_V02;
    break;
#endif /* FEATURE_GNSS_TDP_20 */

  default:
    LOC_MSG_HIGH("[LOC] loc_gdt_SendBeginRequest, Invalid serviceId:%d.", e_ServiceId, 0, 0);
    return e_GDT_FAIL;
  }

  memscpy(z_BeginInd.filePath, sizeof(z_BeginInd.filePath), 
          c_FilePath, q_FilePathLen);

  z_BeginInd.sessionId = q_SessionId;

  /* call an API in loc_task */
  if( loc_gdt_SendBeginIndProxy(client_handle, transaction_id, &z_BeginInd) == 0)
  {
    return e_GDT_SUCCESS;
  }

  return e_GDT_FAIL;
}

/*===========================================================================
FUNCTION loc_gdt_SendEndRequest

DESCRIPTION
  This function sends a GDT END indication to AP.
 
PARAMETERS 
 
  e_ServiceId - GDT Client service ID
  (input)
 
  q_SessionId - GDT session ID
  (input)
 
  e_Status   - End Status (session succeeded or aborted)
  (input)
 
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the request, and sent the indication
                 to AP
 
  e_GDT_FAIL: Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType loc_gdt_SendEndRequest(e_GDTServiceIDType e_ServiceId,
                                            uint32 q_SessionId, 
                                            e_GDTResponseType e_Status)
{
  qmiLocEventGdtUploadEndReqIndMsgT_v02 z_EndInd;
  int32  l_ClientHandle = 0;
  uint32 q_Transaction_id = 0;

  z_EndInd.sessionId = q_SessionId;
  switch (e_ServiceId)
  {
    case e_GDT_SERVICE_GTP_WWAN:
      z_EndInd.serviceId = eQMI_LOC_GDT_SERVICE_WWAN_V02;
      break;

#ifdef FEATURE_GNSS_TDP_20
    case e_GDT_SERVICE_GTP_CSM_UL:
      z_EndInd.serviceId = eQMI_LOC_GDT_SERVICE_CSM_UL_V02;
      break;
#endif /* FEATURE_GNSS_TDP_20 */

  default:
    LOC_MSG_MED("[LOC] loc_gdt_SendEndRequest, Invalid serviceId:%d.", e_ServiceId, 0, 0);
      return e_GDT_FAIL;
  }

  switch (e_Status)
  {
    case e_GDT_ACCESS_ALLOWED:
      z_EndInd.endStatus = eQMI_LOC_GDT_SUCCESS_V02;
      break;

    default:
      z_EndInd.endStatus = eQMI_LOC_GDT_FAILED_V02;
  }

  /* call an API in loc_task */
  if( loc_gdt_SendEndIndProxy(l_ClientHandle, q_Transaction_id, &z_EndInd) == 0)
  {
    return e_GDT_SUCCESS;
  }

  return e_GDT_FAIL;

}

/*===========================================================================
FUNCTION loc_gdt_BeginReqIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtUploadBeginStatusIndMsgT_v02
  MP has received the request for GDT begin response from AP, now it is
  sending the indication for the request.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_BeginReqIndHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_gdt_BeginReqIndHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocGdtUploadBeginStatusIndMsgT_v02 z_Ind;

    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_GDT_UPLOAD_BEGIN_STATUS_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC] loc_gdt_BeginReqIndHandler. client: %d, tx id: %lu, status: %d",
                   l_ClientHandle, q_TransactionId, (int32) z_Ind.status);

      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(l_ClientHandle, &z_IndInfo))
      {
        // done
        q_ErrorCode = 0;
      }
      else
      {
        // failed
        q_ErrorCode = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      q_ErrorCode = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    q_ErrorCode = 4;
  }

  if (0 != q_ErrorCode)
  {
    LOC_MSG_ERROR("[LOC] loc_gdt_BeginReqIndHandler: error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
}

/*===========================================================================
FUNCTION loc_gdt_EndReqIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtUploadEndIndMsgT_v02 
  MP has received the request for GDT end response from AP, now it is
  sending the indication for the request.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_EndReqIndHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_gdt_EndReqIndHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocGdtUploadEndIndMsgT_v02 z_Ind;
    
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_GDT_UPLOAD_END_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC] loc_gdt_EndReqIndHandler. client: %d, tx id: %lu, status: %d",
                   l_ClientHandle, q_TransactionId, (int32) z_Ind.status);

      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(l_ClientHandle, &z_IndInfo))
      {
        // done
        q_ErrorCode = 0;
      }
      else
      {
        // failed
        q_ErrorCode = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      q_ErrorCode = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    q_ErrorCode = 4;
  }

  if (0 != q_ErrorCode)
  {
    LOC_MSG_ERROR("[LOC] loc_gdt_EndReqIndHandler: error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
}

/*===========================================================================
FUNCTION loc_gdt_DownloadReadyReq

DESCRIPTION
  This function receives a TDP download ready request from AP and forwards the
  request status to GDT MP. 
 
PARAMETERS 
 
  cpz_Client
  (input)
 
  cpz_Req - Points to the download ready request message
  (input)
 
  pz_Resp - 
  (output)
DEPENDENCIES

RETURN VALUE 
  LOC_API_SUCCESS: If the status is delivered to GDT MP
 
  LOC_API_GENERAL_FAILURE: Otherwise
 
SIDE EFFECTS

===========================================================================*/
int32 loc_gdt_DownloadReadyReq(const locQmiShimCltInfoStructT * cpz_Client,
                               const qmiLocGdtDownloadReadyStatusReqMsgT_v02 *const cpz_Req,
                               qmiLocGenRespMsgT_v02 *const pz_Resp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

#ifdef FEATURE_GNSS_TDP_20
  if ((NULL != cpz_Client) && (NULL != cpz_Req) && (NULL != pz_Resp) &&
      (cpz_Req->filePath_len < QMI_LOC_MAX_GDT_PATH_LEN_V02) )
  {
    gdt_ReceiveReadyResponseType z_Response = {0}; 
    gdt_ReceiveReadyArgumentType z_ReadyArg;
    uint32 q_TransactionId = loc_qmiloc_get_transaction_id();
    int32  l_ClientHandle = cpz_Client->l_LocClientHandle;

    z_ReadyArg.pz_ReadyResponse = &z_Response;

    LOC_MSG_MED("loc_gdt_DownloadReadyReq. client:%d, serv ID:%lu, status:%d",
                 cpz_Client->l_LocClientHandle, cpz_Req->serviceId, (int) cpz_Req->status);

    memset(pz_Resp, 0, sizeof(*pz_Resp));

    z_Response.q_FilePathLen = cpz_Req->filePath_len;

    memscpy(z_Response.RcvFilePath, sizeof(z_Response.RcvFilePath),
            cpz_Req->filePath, sizeof(cpz_Req->filePath));


    z_Response.z_SessionInfo.q_SessId = cpz_Req->sessionId;
    switch (cpz_Req->status)
    {
       case eQMI_LOC_SUCCESS_V02:
         z_Response.e_StartStatus = e_GDT_ACCESS_ALLOWED;
         break;

       default:
         z_Response.e_StartStatus = e_GDT_ACCESS_NOT_ALLOWED;
    }
    
    switch (cpz_Req->serviceId)
    {
       case eQMI_LOC_GDT_SERVICE_WWAN_V02:
         z_Response.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_GTP_WWAN;
        break;

       case eQMI_LOC_GDT_SERVICE_WWAN_DL_V02:
         z_Response.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_GTP_WWAN_DL;
        break;

      default:
        z_Response.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_INVALID; 
    }

    z_Response.l_ClientHandle = cpz_Client->l_LocClientHandle;
    z_Response.q_TransactionId = q_TransactionId;

    if (e_GDT_SUCCESS == gdt_RcvdReceiveReadyResponse(z_ReadyArg))
    {
      result = LOC_API_SUCCESS;
    }
  }
  else
  {
    // this null pointer reference shall never happen
    // the error code indicates pResp is not filled
    result = LOC_API_GENERAL_FAILURE;
  }
#endif /* FEATURE_GNSS_TDP_20 */
  
  LOC_MSG_MED("Download ready result:%d", result, 0, 0);
  return result;
}

/*===========================================================================
FUNCTION loc_gdt_RcvDoneReqIndProxy

DESCRIPTION
  This function sends a receive done indication from MP into loc task to
  be sent to AP.
 
PARAMETERS 
 
  p_ind  - Points to the QMI indication
  (output)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
 
SIDE EFFECTS

===========================================================================*/
static uint32 loc_gdt_RcvDoneReqIndProxy(const qmiLocEventGdtReceiveDoneIndMsgT_v02 * const p_ind)
{
  uint32 error_code = 1;
  int32 client_handle = 0;
  uint32 transaction_id = 0;
   
  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_GDT_RECEIVE_DONE_REQ_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_gdt_RcvDoneIndProxy: error code:%lu", 
                    error_code, 0, 0);
    }
  }
  return error_code; 
}

/*===========================================================================
FUNCTION loc_gdt_ReceiveDoneReqInd

DESCRIPTION
  This function is called by GDT-MP to send a receive done event to AP
 
PARAMETERS 
 
  e_ServiceId - Service Id
  (input)
 
  q_SessionId - GDT session ID
  (input)
 
  e_Status - Status of the close
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS - if ipc has been sent to loc_task
  e_GDT_FAIL    - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType loc_gdt_ReceiveDoneReqInd(e_GDTServiceIDType e_ServiceId,
                                               uint32 q_SessionId,
                                               e_GDTTransferStatusType e_Status)
{
#ifdef FEATURE_GNSS_TDP_20
  qmiLocEventGdtReceiveDoneIndMsgT_v02 z_RcvDoneInd = {0};

  z_RcvDoneInd.sessionId = q_SessionId;
  switch (e_ServiceId)
  {
    case e_GDT_SERVICE_GTP_WWAN:
      z_RcvDoneInd.serviceId = eQMI_LOC_GDT_SERVICE_WWAN_V02;
      break;

    case e_GDT_SERVICE_GTP_WWAN_DL:
      z_RcvDoneInd.serviceId = eQMI_LOC_GDT_SERVICE_WWAN_DL_V02;
      break;

    default:
      return e_GDT_FAIL;
  }

  switch (e_Status)
  {
    case e_GDT_TRANSFER_INCOMPLETE:
      z_RcvDoneInd.status = eQMI_LOC_GTP_RCV_STATUS_CONTINUE_V02;
      break;

     case e_GDT_TRANSFER_COMPLETE:
       z_RcvDoneInd.status = eQMI_LOC_GTP_RCV_STATUS_DONE_V02;
       break;

    default:
      return e_GDT_FAIL;
  }

  if( loc_gdt_RcvDoneReqIndProxy(&z_RcvDoneInd) == 0)
  {
    return e_GDT_SUCCESS;
  }
#endif /* FEATURE_GNSS_TDP_20 */

  return e_GDT_FAIL;
}

/*===========================================================================
FUNCTION loc_gdt_RcvDoneStatusReq

DESCRIPTION
  This function receives a TDP receive done status request from AP and forwards the
  request status to GDT MP. 
 
PARAMETERS 
 
  cpz_Client
  (input)
 
  cpz_Req - Points to the receive done status request message
  (input)
 
  pz_Resp - 
  (output)
DEPENDENCIES

RETURN VALUE 
  LOC_API_SUCCESS: If the status is delivered to GDT MP
 
  LOC_API_GENERAL_FAILURE: Otherwise
 
SIDE EFFECTS

===========================================================================*/
int32 loc_gdt_RcvDoneStatusReq(const locQmiShimCltInfoStructT *cpz_Client,
                               const qmiLocGdtReceiveDoneStatusReqMsgT_v02 *const cpz_Req,
                               qmiLocGenRespMsgT_v02 *const pz_Resp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

#ifdef FEATURE_GNSS_TDP_20
  if ((NULL != cpz_Client) && (NULL != cpz_Req) && (NULL != pz_Resp))
  {
    gdt_CloseResponseType z_CloseResponse = {0}; 
    gdt_CloseArgumentType z_CloseArg;

    z_CloseArg.pz_CloseResponse = &z_CloseResponse;

    LOC_MSG_MED("loc_gdt_RcvDoneStatusReq. client:%d, serv ID:%lu, status:%d",
                 cpz_Client->l_LocClientHandle, cpz_Req->serviceId, (int) cpz_Req->status);

    memset(pz_Resp, 0, sizeof(*pz_Resp));
    z_CloseResponse.l_ClientHandle = cpz_Client->l_LocClientHandle;
    z_CloseResponse.q_TransactionId = loc_qmiloc_get_transaction_id();

    z_CloseResponse.z_SessionInfo.q_SessId = cpz_Req->sessionId;
    switch (cpz_Req->status)
    {
       case eQMI_LOC_SUCCESS_V02:
         z_CloseResponse.e_CloseStatus = e_GDT_SUCCESS;
         break;

       default:
         z_CloseResponse.e_CloseStatus = e_GDT_FAIL;
    }
    
    switch (cpz_Req->serviceId)
    {
       case eQMI_LOC_GDT_SERVICE_WWAN_V02:
         z_CloseResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_GTP_WWAN;
         break;

       case eQMI_LOC_GDT_SERVICE_WWAN_DL_V02:
         z_CloseResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_GTP_WWAN_DL;
         break;

      default:
        z_CloseResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_INVALID; 
    }

    if (e_GDT_SUCCESS == gdt_RcvdReceiveCloseResponse(z_CloseArg))
    {
      result = LOC_API_SUCCESS;
    }
  }
  else
  {
    // this null pointer reference shall never happen
    // the error code indicates pResp is not filled
    result = LOC_API_GENERAL_FAILURE;
  }
#endif /* FEATURE_GNSS_TDP_20 */ 
  
  LOC_MSG_MED("Rcv done ready result:%d", result, 0, 0);
  return result;
}

/*===========================================================================
FUNCTION loc_gdt_DownlodEndStatusReq

DESCRIPTION
  This function receives a TDP download end status request from AP and forwards the
  request status to GDT MP. 
 
PARAMETERS 
 
  cpz_Client
  (input)
 
  cpz_Req - Points to the download end status request message
  (input)
 
  pz_Resp - 
  (output)
DEPENDENCIES

RETURN VALUE 
  LOC_API_SUCCESS: If the status is delivered to GDT MP
 
  LOC_API_GENERAL_FAILURE: Otherwise
 
SIDE EFFECTS

===========================================================================*/
int32 loc_gdt_DownlodEndStatusReq(const locQmiShimCltInfoStructT *cpz_Client,
                                  const qmiLocGdtDownloadEndStatusReqMsgT_v02 *const cpz_Req,
                                  qmiLocGenRespMsgT_v02 *const pz_Resp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

#ifdef FEATURE_GNSS_TDP_20
  if ((NULL != cpz_Client) && (NULL != cpz_Req) && (NULL != pz_Resp))
  {
    gdt_EndResponseType        z_EndResponse = {0}; 
    gdt_ReceiveEndArgumentType z_EndArg;

    z_EndArg.pz_EndResponse = &z_EndResponse;

    LOC_MSG_MED("loc_gdt_DownlodEndStatusReq client:%d, serv ID:%lu, status:%d",
                 cpz_Client->l_LocClientHandle, cpz_Req->serviceId, (int) cpz_Req->status);

    memset(pz_Resp, 0, sizeof(*pz_Resp));

    z_EndResponse.l_Client_handle = cpz_Client->l_LocClientHandle;

    z_EndResponse.z_SessionInfo.q_SessId = cpz_Req->sessionId;
    z_EndResponse.q_TransactionId = loc_qmiloc_get_transaction_id();
    switch (cpz_Req->status)
    {
    case eQMI_LOC_SUCCESS_V02:
      z_EndResponse.e_EndStatus = e_GDT_ACK_SUCCESS;
      break;

    default:
      z_EndResponse.e_EndStatus = e_GDT_ACK_FAILED;
    }

    switch (cpz_Req->serviceId)
    {
       case eQMI_LOC_GDT_SERVICE_WWAN_V02:
         z_EndResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_GTP_WWAN;
        break;

       case eQMI_LOC_GDT_SERVICE_WWAN_DL_V02:
         z_EndResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_GTP_WWAN_DL;
        break;

      default:
        z_EndResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_INVALID; 
    }

    if (e_GDT_SUCCESS == gdt_RcvdReceiveEndResponse(z_EndArg))
    {
      result = LOC_API_SUCCESS;
    }
  }
  else
  {
    // this null pointer reference shall never happen
    // the error code indicates pResp is not filled
    result = LOC_API_GENERAL_FAILURE;
  }
#endif /* FEATURE_GNSS_TDP_20 */

  LOC_MSG_MED("Rcv end result:%d", result, 0, 0);
  return result;
}

/*===========================================================================
FUNCTION loc_gdt_DownloadEndReqIndProxy

DESCRIPTION
  This function sends a download end indication from MP into loc task to
  be sent to AP.
 
PARAMETERS 
 
  p_ind  - Points to the QMI indication
  (output)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
 
SIDE EFFECTS

===========================================================================*/
static uint32 loc_gdt_DownloadEndReqIndProxy(const qmiLocEventGdtDownloadEndReqIndMsgT_v02 * const p_ind)
{
  uint32 error_code = 1;
  if (NULL != p_ind)
  {
    int32 client_handle = 0; 
    uint32 transaction_id = 0; 
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_GDT_DOWNLOAD_END_REQ_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_gdt_DownloadEndIndProxy: error code:%lu", 
                    error_code, 0, 0);
    }
  }
  return error_code; 
}

/*===========================================================================
FUNCTION loc_gdt_DownloadEndReqInd

DESCRIPTION
  This function is called by GDT-MP to send a session end event to AP
 
PARAMETERS 
 
  e_ServiceId - Service Id
  (input)
 
  q_SessionId - GDT session ID
  (input)
 
  e_Status - Status of the session end
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS - if ipc has been sent to loc_task
  e_GDT_FAIL    - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType loc_gdt_DownloadEndReqInd(e_GDTServiceIDType e_ServiceId,
                                               uint32 q_SessionId,
                                               e_GDTTransferStatusType e_Status)
{
#ifdef FEATURE_GNSS_TDP_20
  qmiLocEventGdtDownloadEndReqIndMsgT_v02 z_RcvEndInd = {0}; 

  z_RcvEndInd.sessionId = q_SessionId;
  switch (e_ServiceId)
  {
    case e_GDT_SERVICE_GTP_WWAN:
      z_RcvEndInd.serviceId = eQMI_LOC_GDT_SERVICE_WWAN_V02;
      break;

     case e_GDT_SERVICE_GTP_WWAN_DL:
       z_RcvEndInd.serviceId = eQMI_LOC_GDT_SERVICE_WWAN_DL_V02;
       break;

    default:
      return e_GDT_FAIL;
  }

  switch (e_Status)
  {
     case e_GDT_TRANSFER_COMPLETE:
       z_RcvEndInd.status = eQMI_LOC_SUCCESS_V02;
       break;

     default:
       z_RcvEndInd.status = eQMI_LOC_GENERAL_FAILURE_V02;
  }

  /* call an API in loc_task */
  if( loc_gdt_DownloadEndReqIndProxy(&z_RcvEndInd) == 0)
  {
    return e_GDT_SUCCESS;
  }
#endif /* FEATURE_GNSS_TDP_20 */

  return e_GDT_FAIL;
}

/*===========================================================================
FUNCTION loc_gdt_RcvDoneReqIndicationHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocEventGdtReceiveDoneIndMsgT_v02.
  GDT MP is informing GDT AP that MP has received the downloaded file.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_RcvDoneReqIndicationHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_gdt_RcvDoneReqIndicationHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;

    qmiLocEventGdtReceiveDoneIndMsgT_v02   z_Ind; 
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_EVENT_GDT_RECEIVE_DONE_IND_V02, NULL, (int32)sizeof(z_Ind) };

    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC] loc_gdt_RcvDoneReqIndicationHandler, client: %d, tx id: %lu",
                   l_ClientHandle, q_TransactionId, 0);

      LOC_MSG_MED("[LOC] loc_gdt_RcvDoneReqIndicationHandler, serviceId:%d, sessId:%lu, status:%lu",
                   (int32) z_Ind.serviceId, z_Ind.sessionId, z_Ind.status);
      
      if(locQmiShimSendTdpInd(&z_IndInfo))
      {
        q_ErrorCode = 0;
      }
      else
      {
        q_ErrorCode = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      q_ErrorCode = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    q_ErrorCode = 4;
  }
  if (0 != q_ErrorCode)
  {
    LOC_MSG_ERROR("[LOC] loc_gdt_RcvDoneReqIndicationHandler: error code: %u", q_ErrorCode, 0, 0);
  }
}

/*===========================================================================
FUNCTION loc_gdt_DownloadEndReqIndicationHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocEventGdtReceiveDoneIndMsgT_v02.
  GDT MP is informing GDT AP that MP has received the downloaded file.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_DownloadEndReqIndicationHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_gdt_DownloadEndReqIndicationHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;

    
    qmiLocEventGdtDownloadEndReqIndMsgT_v02   z_Ind; 
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_EVENT_GDT_DOWNLOAD_END_REQ_IND_V02, NULL, (int32)sizeof(z_Ind) };

    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC] loc_gdt_DownloadEndReqIndicationHandler, client: %d, tx id: %lu",
                   l_ClientHandle, q_TransactionId, 0);

      LOC_MSG_MED("[LOC] loc_gdt_DownloadEndReqIndicationHandler, serviceId:%d, sessId:%lu, status:%lu",
                   (int32) z_Ind.serviceId, z_Ind.serviceId, z_Ind.status);
      
      
      if(locQmiShimSendTdpInd(&z_IndInfo))
      {
        q_ErrorCode = 0;
      }
      else
      {
        q_ErrorCode = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      q_ErrorCode = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    q_ErrorCode = 4;
  }
  if (0 != q_ErrorCode)
  {
    LOC_MSG_ERROR("[LOC] loc_gdt_DownloadEndReqIndicationHandler: error code: %u", q_ErrorCode, 0, 0);
  }
}

/*===========================================================================
FUNCTION loc_gdt_DownloadReadyStatusReqProxy

DESCRIPTION
  This function sends download ready status request indication.
  MP has recievd a download ready request from AP and now this function sends the
  corresponding indication back to AP.
 
PARAMETERS 
 
  client_handle - LocAPI client handle
  (input)

  transaction_id - Same transaction id passed in earlier by the request
  (input)

  p_ind   - Points to the The QMI indication
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
SIDE EFFECTS

===========================================================================*/
uint32 loc_gdt_DownloadReadyStatusReqProxy
(
  const int32 client_handle, 
  const uint32 transaction_id, 
  const qmiLocGdtDownloadReadyStatusIndMsgT_v02 * const p_ind
)
{
  uint32 error_code = 1;

  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_GDT_DOWNLOAD_READY_STATUS_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_gdt_DownloadReadyStatusReqProxy: , error code: %lu",
                    error_code, 0, 0);
    }
  }
  return error_code;
}

/*===========================================================================
FUNCTION loc_gdt_DownloadReadyStatusIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtDownloadReadyStatusIndMsgT_v02 
  MP has received a download ready status request from AP, now it is
  sending the indication for the request.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_DownloadReadyStatusIndHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_gdt_DownloadReadyStatusIndHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    
    qmiLocGdtDownloadReadyStatusIndMsgT_v02 z_Ind;
                                           
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_GDT_DOWNLOAD_READY_STATUS_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC] loc_gdt_DownloadReadyStatusIndHandler. client: %d, tx id: %lu, status: %d",
                   l_ClientHandle, q_TransactionId, (int32) z_Ind.status);

      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(l_ClientHandle, &z_IndInfo))
      {
        // done
        q_ErrorCode = 0;
      }
      else
      {
        // failed
        q_ErrorCode = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      q_ErrorCode = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    q_ErrorCode = 4;
  }

  if (0 != q_ErrorCode)
  {
    LOC_MSG_ERROR("[LOC] loc_gdt_DownloadReadyStatusIndHandler: error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
}

/*===========================================================================
FUNCTION loc_gdt_ReceiveDoneStatusIndProxy

DESCRIPTION
  This function sends receive done status request indication.
  MP has recievd a receive done status request from AP and now this function
  sends the corresponding indication back to AP.
 
PARAMETERS 
 
  client_handle - LocAPI client handle
  (input)

  transaction_id - Same transaction id passed in earlier by the request
  (input)

  p_ind   - Points to the The QMI indication
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
SIDE EFFECTS

===========================================================================*/
uint32 loc_gdt_ReceiveDoneStatusIndProxy
(
  const int32 client_handle, 
  const uint32 transaction_id, 
  const qmiLocGdtReceiveDoneStatusIndMsgT_v02 * const p_ind
)
{
  uint32 error_code = 1;

  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_GDT_RECEIVE_DONE_STATUS_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_gdt_ReceiveDoneStatusIndProxy: , error code: %lu",
                    error_code, 0, 0);
    }
  }
  return error_code;
}

/*===========================================================================
FUNCTION loc_gdt_ReceiveDoneStatusIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtReceiveDoneStatusIndMsgT_v02 
  MP has received a GDT receive done status request from AP, now it is
  sending the indication for the request.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_ReceiveDoneStatusIndHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_gdt_ReceiveDoneStatusIndHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    
    qmiLocGdtReceiveDoneStatusIndMsgT_v02 z_Ind;
                                           
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_GDT_RECEIVE_DONE_STATUS_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC] loc_gdt_ReceiveDoneStatusIndHandler. client: %d, tx id: %lu, status: %d",
                   l_ClientHandle, q_TransactionId, (int32) z_Ind.status);

      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(l_ClientHandle, &z_IndInfo))
      {
        // done
        q_ErrorCode = 0;
      }
      else
      {
        // failed
        q_ErrorCode = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      q_ErrorCode = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    q_ErrorCode = 4;
  }

  if (0 != q_ErrorCode)
  {
    LOC_MSG_ERROR("[LOC] loc_gdt_ReceiveDoneStatusIndHandler: error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
}


/*===========================================================================
FUNCTION loc_gdt_DownloadEndStatusIndProxy

DESCRIPTION
  This function sends receive done status request indication.
  MP has recievd a receive done status request from AP and now this function
  sends the corresponding indication back to AP.
 
PARAMETERS 
 
  client_handle - LocAPI client handle
  (input)

  transaction_id - Same transaction id passed in earlier by the request
  (input)

  p_ind   - Points to the The QMI indication
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
SIDE EFFECTS

===========================================================================*/
uint32 loc_gdt_DownloadEndStatusIndProxy
(
  const int32 client_handle, 
  const uint32 transaction_id, 
  const qmiLocGdtDownloadEndStatusIndMsgT_v02 * const p_ind
)
{
  uint32 error_code = 1;

  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_GDT_DOWNLOAD_END_STATUS_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_gdt_DownloadEndStatusIndProxy: , error code: %lu",
                    error_code, 0, 0);
    }
  }
  return error_code;
}

/*===========================================================================
FUNCTION loc_gdt_DownloadEndStatusIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtDownloadEndStatusIndMsgT_v02 
  MP has received a GDT download end status request from AP, now it is
  sending the indication for the request.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_DownloadEndStatusIndHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_gdt_DownloadEndStatusIndHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    
    qmiLocGdtDownloadEndStatusIndMsgT_v02 z_Ind;
                                           
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_GDT_DOWNLOAD_END_STATUS_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC] loc_gdt_DownloadEndStatusIndHandler. client: %d, tx id: %lu, status: %d",
                   l_ClientHandle, q_TransactionId, (int32) z_Ind.status);

      // send indication through QCSI
      if (locQmiShimSendTdpInd(&z_IndInfo))
      {
        // done
        q_ErrorCode = 0;
      }
      else
      {
        // failed
        q_ErrorCode = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      q_ErrorCode = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    q_ErrorCode = 4;
  }

  if (0 != q_ErrorCode)
  {
    LOC_MSG_ERROR("[LOC] loc_gdt_DownloadEndStatusIndHandler: error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
}


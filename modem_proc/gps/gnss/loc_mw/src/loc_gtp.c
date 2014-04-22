/******************************************************************************
  @file: loc_gtp.c
  @brief:   This module handles gpsone GTP related features .
  

  DESCRIPTION
   Qualcomm Location API GTP Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2014 QUALCOMM Atheros, Inc.
  All Rights Reserved.
  QCA Proprietary and Confidential. 

  Copyright (c) 2015-2020 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_gtp.c#1 $
$DateTime: 2020/07/21 21:23:44 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
08/28/14   ah       Intial version
======================================================================*/
#include "loc_gtp.h"
#include "loc_api_2.h"
#include "loc_api_internal.h"
#include "tle_api.h"
#include "loc_utils.h"
#include "loc_qmi_shim.h"
#include "loc_for_gtp.h"


/*===========================================================================
FUNCTION loc_gtp_ClientDownloadedDataIndProxy

DESCRIPTION
  This function Sends gtp ClientDownloadedData indication 
 
PARAMETERS 
 
  client_handle - LocAPI client handle  
  (input)

  transaction_id  - Same transaction id passed in earlier by the request
  (input)
 
  p_ind  - The QMI indication
  (output)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
 
SIDE EFFECTS

===========================================================================*/
uint32 loc_gtp_ClientDownloadedDataIndProxy(const int32 client_handle, 
                                            const uint32 transaction_id, 
                                            const qmiLocInjectGtpClientDownloadedDataIndMsgT_v02 * const p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_GTP_CLIENT_DLD_DATA_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[LOC] loc_gtp_clientDownloadedData_ind_proxy: "
                  "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/*===========================================================================
FUNCTION loc_gtp_InjectGtpClientDownloadedDataReq

DESCRIPTION
  This function handles QMI command to inject GTP client downloaded data 
 
PARAMETERS 
 
  cpz_Client - Points to LocAPI client 
  (input)

  cpz_Req  - Points to QMI request
  (input)
 
  pz_Resp  - Points to QMI response
  (output)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
int32 loc_gtp_InjectGtpClientDownloadedDataReq(const locQmiShimCltInfoStructT *const cpz_Client,
                                               const qmiLocInjectGtpClientDownloadedDataReqMsgT_v02 *const cpz_Req,
                                               qmiLocGenRespMsgT_v02 *const pz_Resp)
{
  int32 result = LOC_API_GENERAL_FAILURE;
  tle_GtpClientDownloadedReqType z_ClientDownloadedData;

  if ((NULL != cpz_Client) && (NULL != cpz_Req) && (NULL != pz_Resp))
  {
    memscpy(&z_ClientDownloadedData.z_ClienDownloadedReq, 
            sizeof(z_ClientDownloadedData.z_ClienDownloadedReq),
            cpz_Req, sizeof(*cpz_Req));

    z_ClientDownloadedData.l_Client_handle = cpz_Client->l_LocClientHandle;
    z_ClientDownloadedData.q_TransactionId = loc_qmiloc_get_transaction_id();

            
    LOC_MSG_MED("loc_gtp_InjectGtpClientDownloadedDataReq. client:%d, data_len:%lu",
                 cpz_Client->l_LocClientHandle, cpz_Req->ClientDownloadedData_len, 0);

    memset(pz_Resp, 0, sizeof(*pz_Resp));
    #ifdef FEATURE_CGPS_XTRA_T
    if ((cpz_Req->ClientDownloadedData_len < QMI_LOC_MAX_GTP_WWAN_CLIENT_DOWNLOADED_DATA_LEN_V02) && 
        tle_InjectGtpWwanClientData(&z_ClientDownloadedData))
    {
      pz_Resp->resp.error = QMI_ERR_NONE_V01;
      pz_Resp->resp.result = QMI_RESULT_SUCCESS_V01;
    }
    else
    #endif /* FEATURE_CGPS_XTRA_T */
    {
      pz_Resp->resp.error = QMI_ERR_GENERAL_V01;
      pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
    }

    // the result of this function is success as long as pResp is filled
    result = LOC_API_SUCCESS;
  }
  else
  {
    // this null pointer reference shall never happen
    // the error code indicates pResp is not filled
    result = LOC_API_GENERAL_FAILURE;
  }
  return result;
}


/*===========================================================================
FUNCTION loc_gtp_WwanClientDloadedDataIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocInjectGtpClientDownloadedDataIndMsgT_v02 
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gtp_WwanClientDloadedDataIndHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_gtp_WwanClientDloadedDataIndHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocInjectGtpClientDownloadedDataIndMsgT_v02 z_Ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC] loc_gtp_WwanClientDloadedData_ind_handler. client: %d, tx id: %u, status: %d",
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
    LOC_MSG_ERROR("[LOC] loc_gtp_WwanClientDloadedData_ind_handler: error code: %u", q_ErrorCode, 0, 0);
  }
} 

/*===========================================================================
FUNCTION loc_gtp_DownloadBeginReqInd

DESCRIPTION
  This function sends a TDP Start Download indication to AP. MP attempts to
  initiate a TDP download.
 
PARAMETERS 
 
  cpz_Req - Points to the start download reuest message
  (input)
 
DEPENDENCIES

RETURN VALUE 
  TRUE: If successful
 
  FALSE: Otherwise
 
SIDE EFFECTS

===========================================================================*/
boolean loc_gtp_DownloadBeginReqInd(const loc_gtp_DownldStartRequestType *cpz_Req)
{
  
  return FALSE;
}

/*===========================================================================
FUNCTION loc_tle_RequestFdclService

DESCRIPTION
   This functionis called by TLE module to  indicate the loc_middleware 
   to send an QMI indication to App layer, requesting for
   FDCL service.
 
PARAMETERS 
 qmiLocEventFdclServiceReqIndMsgT_v02* p_ind

DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
uint32 loc_tle_RequestFdclService(qmiLocEventFdclServiceReqIndMsgT_v02* p_ind)
{
  int32 client_handle=0;
  uint32 transaction_id=0;
  uint32 error_code = 1;

  LOC_MSG_MED("[LOC]  loc_tle_RequestFdclService", 0, 0, 0);
  IND_PROXY(LM_MIDDLEWARE_MSG_ID_SEND_FDCL_SERVICE_REQ_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[LOC] loc_tle_RequestFdclService: "
                   "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/*===========================================================================
FUNCTION loc_tle_sendFDCLServiceReqInd

DESCRIPTION
   This function handles the request, made by TLE module, to send QMI Indication to start
   FDCL service.
 
PARAMETERS 
    cpz_IpcMsg-Points to the recieved IPC message
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
uint32 loc_tle_sendFDCLServiceReqInd(const os_IpcMsgType * cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_tle_sendFDCLServiceReqInd.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocEventFdclServiceReqIndMsgT_v02 z_Ind={'\0'};
       
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_EVENT_FDCL_SERVICE_REQ_IND_V02, NULL, (uint32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC] loc_tle_sendFDCLServiceReqInd, client: %d, tx id: %lu",
                   l_ClientHandle, q_TransactionId, 0);

      // send indication through QCSI
      if (locQmiShimBroadcastInd(QMI_LOC_EVENT_MASK_FDCL_SERVICE_REQ_V02, 
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
    LOC_MSG_ERROR("[LOC] loc_tle_sendFDCLServiceReqInd, error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
  return q_ErrorCode;
}
/*===========================================================================
FUNCTION loc_tle_GetFdclBsListIndProxy

DESCRIPTION
  This function is called  by TLE to  indicates the LOC_MW to send an QMI indication t QMI client 
  with list of all the Base stations for which FDCL data is required.
 
PARAMETERS 
 
  client_handle   - Client Id of the Client which had made the 
                   request, in response to which indication is 
                   being sent.
  transaction_id-Transaction id of the request in response to which 
                   indication is being sent
  p_ind-Points to the indication  message that has to be sent
  (input)
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/

uint32 loc_tle_GetFdclBsListIndProxy(const int32 client_handle,
                                         const uint32 transaction_id,
                                         const qmiLocGetFdclBsListIndMsgT_v02 * p_ind)
{
  uint32 error_code = 1;
  LOC_MSG_MED("[LOC]  loc_tle_GetFdclBsListIndProxy", 0, 0, 0);
  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_SEND_GET_FDCL_BS_LIST_IND)
  
    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_tle_GetFdclBsListIndProxy: "
                    "error code: %u", error_code, 0, 0);
    }
  }
  return error_code;
}
/*===========================================================================
FUNCTION loc_tle_sendGetFdclBsListInd

DESCRIPTION
 This function handles the request made by TLE to  .
 to send an QMI indication t QMI client with list of all the Base stations 
 for which FDCL data is required.
PARAMETERS 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/	
void loc_tle_sendGetFdclBsListInd(const os_IpcMsgType * cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_tle_sendGetFdclBsListInd", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocGetFdclBsListIndMsgT_v02 z_Ind;

    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_GET_FDCL_BS_LIST_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC]loc_tle_sendGetFdclBsListInd. client: %d, tx id: %lu ",
                   l_ClientHandle, q_TransactionId,0);

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
    LOC_MSG_ERROR("[LOC]loc_tle_sendGetFdclBsListInd: error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
}
/*===========================================================================
FUNCTION loc_tle_InjectFdclDataIndProxy

DESCRIPTION
 This function is called by TLE module to indicate locatiion middleware to send a QMI Indication, 
 with the status. of Injection of FDCL data.
 
PARAMETERS 
 
  client_handle - Handle of the client to which indication is to be sent
   transaction_id - Transaction id of QMI_LOC_INJECT_FDCL_DATA_REQ_V02
     p_ind - Indication message to be sent.
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/ 
uint32 loc_tle_InjectFdclDataIndProxy(const int32 client_handle,
                                         const uint32 transaction_id,
                                         const qmiLocInjectFdclDataIndMsgT_v02 * p_ind)
{
  uint32 error_code = 1;
  LOC_MSG_MED("[LOC]  loc_tle_InjectFdclDataIndProxy", 0, 0, 0);
  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_SEND_INJECT_FDCL_DATA_IND)
  
    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_tle_InjectFdclDataIndProxy: "
                    "error code: %u", error_code, 0, 0);
    }
  }
  return error_code;
}

/*===========================================================================
FUNCTION loc_tle_sendInjectFdclDataInd

DESCRIPTION
 This function indicates locatiion middleware to send a QMI Indication, to 
  Qmi client, with the status. of Injection of FDCL data.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/	
void loc_tle_sendInjectFdclDataInd(const os_IpcMsgType * cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_tle_sendInjectFdclDataInd", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocInjectFdclDataIndMsgT_v02 z_Ind;

    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_INJECT_FDCL_DATA_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC]loc_tle_sendInjectFdclDataInd. client: %d, tx id: %lu, status: %d",
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
    LOC_MSG_ERROR("[LOC]loc_tle_sendInjectFdclDataInd: error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
}
/*===========================================================================
FUNCTION loc_tle_GetFdclBsListReq

DESCRIPTION
  This function handles QMI command to get the list of BS for which FDCL data
   is required.
     
PARAMETERS 
 
  cpz_Client - Points to the LocAPI client
  (input)

  cpz_Req   - Points to the QMI Request message received from AP
  (input)

  pz_Resp   - Points to the generated response
  (output)
 
DEPENDENCIES

RETURN VALUE 
  TRUE  : If the command has been handled successfully
  FALSE : Otherwise

SIDE EFFECTS

===========================================================================*/

boolean loc_tle_GetFdclBsListReq(const locQmiShimCltInfoStructT * cpz_Client,
                                 const qmiLocGetFdclBsListReqMsgT_v02 * cpz_Req,
                                 qmiLocGenRespMsgT_v02 * pz_Resp)
{
  tle_FdclBsListReqType z_bsListReq;
  boolean v_RetVal = FALSE;
  memset(&z_bsListReq,0, sizeof(tle_FdclBsListReqType));
  memset(pz_Resp,0, sizeof(qmiLocGenRespMsgT_v02));
  if(NULL != cpz_Req && NULL != cpz_Client)
  {
#ifdef FEATURE_CGPS_XTRA_T
    (void)memscpy(&(z_bsListReq.z_Msg),sizeof(z_bsListReq.z_Msg) , cpz_Req, sizeof(*cpz_Req));
    z_bsListReq.l_Client_handle = cpz_Client->l_LocClientHandle;
    z_bsListReq.q_TransactionId = loc_qmiloc_get_transaction_id();
    v_RetVal = tle_GetFdclBsList(&z_bsListReq);
    if(TRUE == v_RetVal)
    {
      pz_Resp->resp.result = QMI_RESULT_SUCCESS_V01;
      pz_Resp->resp.error = QMI_ERR_NONE_V01;
    }
    else
#endif /* FEATURE_CGPS_XTRA_T */
    {
      pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
      pz_Resp->resp.error = QMI_ERR_INTERNAL_V01;
    }
  }
  else
  {

    pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
    pz_Resp->resp.error = QMI_ERR_MALFORMED_MSG_V01;
  }
  return v_RetVal;
}
/*===========================================================================
FUNCTION loc_tle_InjectFdclDataReq

DESCRIPTION
  This function handles QMI command to inject FDCL Data 
   is required.
     
PARAMETERS 
 
  cpz_Client - Points to the LocAPI client
  (input)

  cpz_Req   - Points to the QMI Request message received from AP
  (input)

  pz_Resp   - Points to the generated response
  (output)
 
DEPENDENCIES

RETURN VALUE 
  TRUE  : If the command has been handled successfully
  FALSE : Otherwise

SIDE EFFECTS

===========================================================================*/
boolean loc_tle_InjectFdclDataReq(const locQmiShimCltInfoStructT * cpz_Client,
                                        const qmiLocInjectFdclDataReqMsgT_v02 * cpz_Req,
                                   qmiLocGenRespMsgT_v02 * pz_Resp)
{
  tle_FdclInjectDataReqType z_FdclInjectDataReq;
  boolean v_RetVal = FALSE;
  memset(&z_FdclInjectDataReq,0, sizeof(z_FdclInjectDataReq));
  memset(pz_Resp,0, sizeof(*pz_Resp));
  if(NULL != cpz_Req  && NULL != cpz_Client)
  {
#ifdef FEATURE_CGPS_XTRA_T
    (void)memscpy(&(z_FdclInjectDataReq.z_Msg),sizeof(z_FdclInjectDataReq.z_Msg) , 
                  cpz_Req, sizeof(*cpz_Req)); //both qmiLocInjectFdclDataReqMsgT_v02
    z_FdclInjectDataReq.l_Client_handle = cpz_Client->l_LocClientHandle;
    z_FdclInjectDataReq.q_TransactionId = loc_qmiloc_get_transaction_id();
    v_RetVal = tle_InjectFdclData(&z_FdclInjectDataReq);
    if(TRUE == v_RetVal)
    {
      pz_Resp->resp.result = QMI_RESULT_SUCCESS_V01;
      pz_Resp->resp.error = QMI_ERR_NONE_V01;
    }
    else
#endif /* FEATURE_CGPS_XTRA_T */
    {
      pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
      pz_Resp->resp.error = QMI_ERR_INTERNAL_V01;
    }
  }
  else
  {
    pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
    pz_Resp->resp.error = QMI_ERR_MALFORMED_MSG_V01;
  }
  return v_RetVal;
}

/*===========================================================================
FUNCTION loc_tle_Fdcl2_BsObsDataServiceReqIndProxy

DESCRIPTION
This function is called by TLE module indicating loc_middleware
(via IPC LM_MIDDLEWARE_MSG_ID_SEND_FDCL2_SERVICE_REQ_IND) 
to send an indication QMI_LOC_EVENT_BS_OBS_DATA_SERVICE_REQ_IND to AP
requesting for upload of crowdsourced data. The upload of crowdsourced data 
shall be initiated by AP by sending QMI_LOC_GET_BS_OBS_DATA_REQ

PARAMETERS
qmiLocEventBsObsDataServiceReqIndMsgT_v02* p_ind

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
uint32 loc_tle_Fdcl2_BsObsDataServiceReqIndProxy(qmiLocEventBsObsDataServiceReqIndMsgT_v02* p_ind)
{
  int32 client_handle = 0;
  uint32 transaction_id = 0;
  uint32 error_code = 1;

  LOC_MSG_MED_0("[LOC] loc_tle_Fdcl2_BsObsDataServiceReqIndProxy");
  IND_PROXY(LM_MIDDLEWARE_MSG_ID_SEND_FDCL2_SERVICE_REQ_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR_1("[LOC] loc_tle_Fdcl2_BsObsDataServiceReqIndProxy: "
                    "error code: %u", error_code);
    }
  return error_code;
}

/*===========================================================================
FUNCTION loc_tle_Fdcl2_SendBsObsDataServiceReqInd

DESCRIPTION
IPC Handler for LM_MIDDLEWARE_MSG_ID_SEND_FDCL2_SERVICE_REQ_IND.
This function handles the request, made by TLE module, and sends Indication 
QMI_LOC_EVENT_BS_OBS_DATA_SERVICE_REQ_IND_V02 to client to start initiation 
of crowdsourced data upload

PARAMETERS
cpz_IpcMsg Points to the received IPC message

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
uint32 loc_tle_Fdcl2_SendBsObsDataServiceReqInd(const os_IpcMsgType * cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED_0("[LOC] loc_tle_Fdcl2_SendBsObsDataServiceReqInd");

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocEventBsObsDataServiceReqIndMsgT_v02 z_Ind = {'\0'};

    locQmiShimIndInfoStructT z_IndInfo = {QMI_LOC_EVENT_BS_OBS_DATA_SERVICE_REQ_IND_V02, NULL, (uint32)sizeof(z_Ind)};
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED_2("[LOC] loc_tle_Fdcl2_SendBsObsDataServiceReqInd, client: %d, tx id: %lu",
                  l_ClientHandle, q_TransactionId);

      // send indication through QCSI
      if (locQmiShimBroadcastInd(QMI_LOC_EVENT_MASK_BS_OBS_DATA_SERVICE_REQ_V02,
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
    LOC_MSG_ERROR_1("[LOC] loc_tle_Fdcl2_SendBsObsDataServiceReqInd, error code: %lu",
                  q_ErrorCode);
  }
  return q_ErrorCode;
}

/*===========================================================================
FUNCTION loc_tle_Fdcl2_GetBsObsDataReq

DESCRIPTION
This function handles QMI request QMI_LOC_GET_BS_OBS_DATA_REQ_V02 from AP
requesting the upload of crowdsourced base station data via indication
QMI_LOC_GET_BS_OBS_DATA_IND_V02

PARAMETERS

cpz_Client - Points to the LocAPI client
(input)

cpz_Req   - Points to the QMI Request message received from AP
(input)

pz_Resp   - Points to the generated response
(output)

DEPENDENCIES

RETURN VALUE
TRUE  : If the command has been handled successfully
FALSE : Otherwise

SIDE EFFECTS
===========================================================================*/
boolean loc_tle_Fdcl2_GetBsObsDataReq(const locQmiShimCltInfoStructT * cpz_Client,
                                     const qmiLocGetBsObsDataReqMsgT_v02 * cpz_Req,
                                     qmiLocGenRespMsgT_v02 * pz_Resp)
{
  boolean v_RetVal = FALSE;
#ifdef FEATURE_GNSS_TDP_20
  tle_FdclBsObsDataReqType z_bsObsDataReq = {0}; //nikhill. change to BS Obs Data Req type in TLE

  if (NULL == pz_Resp)
  {
    LOC_MSG_FATAL_0("loc_tle_Fdcl2_GetBsObsDataReq: pz_Resp is NULL");
    return v_RetVal;
  }
  memset(pz_Resp, 0, sizeof(*pz_Resp));

  if (NULL != cpz_Req && NULL != cpz_Client)
  {
#ifdef FEATURE_CGPS_XTRA_T
    (void)memscpy(&(z_bsObsDataReq.z_Msg), sizeof(z_bsObsDataReq.z_Msg), cpz_Req, sizeof(*cpz_Req));
    z_bsObsDataReq.l_Client_handle = cpz_Client->l_LocClientHandle;
    z_bsObsDataReq.q_TransactionId = loc_qmiloc_get_transaction_id();
    v_RetVal = tle_GetFdcl2BsObsData(&z_bsObsDataReq); //nikhill. replace with new API for Get BS Obs Data
    if (TRUE == v_RetVal)
    {
      pz_Resp->resp.result = QMI_RESULT_SUCCESS_V01;
      pz_Resp->resp.error = QMI_ERR_NONE_V01;
    }
    else
#endif /* FEATURE_CGPS_XTRA_T */
    {
      pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
      pz_Resp->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
    }
  }
  else
  {
    LOC_MSG_FATAL_2("loc_tle_Fdcl2_GetBsObsDataReq: NULL Ptr %d %d. Send Error Resp",
      (NULL == cpz_Req), (NULL == cpz_Client));

    pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
    pz_Resp->resp.error = QMI_ERR_MALFORMED_MSG_V01;
  }
#endif /* FEATURE_GNSS_TDP_20 */

  return v_RetVal;
}

/*===========================================================================
FUNCTION loc_tle_Fdcl2_GetBsObsDataIndProxy

DESCRIPTION
This function is called by TLE indicating LOC_MW 
(via IPC LM_MIDDLEWARE_MSG_ID_SEND_GET_FDCL2_BS_OBS_DATA_IND) to send a indication 
QMI_LOC_GET_BS_OBS_DATA_IND_V02 to AP with the crowdsourced base station
data that has to be uploaded

PARAMETERS

client_handle   - Client Id of the Client which had made the
request, in response to which indication is
being sent.
transaction_id-Transaction id of the request in response to which
indication is being sent
p_ind-Points to the indication  message that has to be sent
(input)
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
uint32 loc_tle_Fdcl2_GetBsObsDataIndProxy(const int32 client_handle,
                                     const uint32 transaction_id,
                                     const qmiLocGetBsObsDataIndMsgT_v02 * p_ind)
{
  uint32 error_code = 1;

  LOC_MSG_MED_0("[LOC]  loc_tle_Fdcl2_GetBsObsDataIndProxy");
  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_SEND_GET_FDCL2_BS_OBS_DATA_IND)

      if (0 != error_code)
      {
        LOC_MSG_ERROR_1("[LOC] loc_tle_Fdcl2_GetBsObsDataIndProxy: "
                      "error code: %u", error_code);
      }
  }
  return error_code;
}

/*===========================================================================
FUNCTION loc_tle_Fdcl2_sendGetBsObsDataInd

DESCRIPTION
IPC handler for LM_MIDDLEWARE_MSG_ID_SEND_GET_FDCL2_BS_OBS_DATA_IND.
This function handles the request made by TLE and sends indication 
QMI_LOC_GET_BS_OBS_DATA_IND_V02 to client with the crowdsourced base station
data that has to be uploaded.

PARAMETERS
cpz_IpcMsg - Points to the received IPC message
(input)

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void loc_tle_Fdcl2_sendGetBsObsDataInd(const os_IpcMsgType * cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED_0("[LOC] loc_tle_Fdcl2_sendGetBsObsDataInd");

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocGetBsObsDataIndMsgT_v02 z_Ind = {0};

    locQmiShimIndInfoStructT z_IndInfo = {QMI_LOC_GET_BS_OBS_DATA_IND_V02, NULL, (int32)sizeof(z_Ind)};
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED_2("[LOC]loc_tle_Fdcl2_sendGetBsObsDataInd. client: %d, tx id: %lu ",
                  l_ClientHandle, q_TransactionId);

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
    LOC_MSG_ERROR_1("[LOC]loc_tle_Fdcl2_sendGetBsObsDataInd: error code: %lu",
                  q_ErrorCode);
  }
}
/*===========================================================================
FUNCTION loc_tle_GetWwanDataIndProxy

DESCRIPTION
  This function is called  by TLE to  indicates the LOC_MW to send an QMI indication
  QMI client with wwan data
 
PARAMETERS 
 
  client_handle   - Client Id of the Client which had made the 
                   request, in response to which indication is 
                   being sent.
  transaction_id-Transaction id of the request in response to which 
                   indication is being sent
  p_ind-Points to the indication  message that has to be sent
  (input)
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/

uint32 loc_tle_GetWwanDataIndProxy(const int32 client_handle,
                                         const uint32 transaction_id,
                                         const qmiLocGetWwanDataIndMsgT_v02 * p_ind)
{
  uint32 error_code = 1;
  LOC_MSG_MED("[LOC]  loc_tle_GetWwanDataIndProxy", 0, 0, 0);
  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_SEND_GET_WWAN_DATA_IND)
  
    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_tle_GetWwanDataIndProxy: "
                    "error code: %u", error_code, 0, 0);
    }
  }
  return error_code;
}

/*===========================================================================
FUNCTION loc_tle_sendGetWwanDataInd

DESCRIPTION
 This function handles the request made by TLE to  .
 to send an QMI indication QMI client with wwan data
PARAMETERS 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/	
void loc_tle_sendGetWwanDataInd(const os_IpcMsgType * cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("[LOC] loc_tle_sendGetWwanDataInd", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocGetWwanDataIndMsgT_v02 z_Ind;

    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_GET_WWAN_DATA_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC]loc_tle_sendGetFdclBsListInd. client: %d, tx id: %lu ",
                   l_ClientHandle, q_TransactionId,0);

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
    LOC_MSG_ERROR("[LOC]loc_tle_sendGetWwanDataInd: error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
}

/*===========================================================================
FUNCTION loc_tle_GetWwanDataReq

DESCRIPTION
  This function handles QMI command to get wwan data
     
PARAMETERS 
 
  cpz_Client - Points to the LocAPI client
  (input)

  cpz_Req   - Points to the QMI Request message received from AP
  (input)

  pz_Resp   - Points to the generated response
  (output)
 
DEPENDENCIES

RETURN VALUE 
  TRUE  : If the command has been handled successfully
  FALSE : Otherwise

SIDE EFFECTS

===========================================================================*/

boolean loc_tle_GetWwanDataReq(const locQmiShimCltInfoStructT * cpz_Client,
                                 const qmiLocGetWwanDataReqMsgT_v02 * cpz_Req,
                                 qmiLocGenRespMsgT_v02 * pz_Resp)
{
  tle_WwanDataReqType z_wwanDataReq;
  boolean v_RetVal = FALSE;
  memset(&z_wwanDataReq,0, sizeof(tle_WwanDataReqType));
  memset(pz_Resp,0, sizeof(qmiLocGenRespMsgT_v02));
  if(NULL != cpz_Req && NULL != cpz_Client)
  {
    (void)memscpy(&(z_wwanDataReq.z_Msg),sizeof(z_wwanDataReq.z_Msg) , cpz_Req, sizeof(*cpz_Req));
    z_wwanDataReq.l_Client_handle = cpz_Client->l_LocClientHandle;
    z_wwanDataReq.q_TransactionId = loc_qmiloc_get_transaction_id();
    v_RetVal = tle_GetWwanData(&z_wwanDataReq);
    if(TRUE == v_RetVal)
    {
      pz_Resp->resp.result = QMI_RESULT_SUCCESS_V01;
      pz_Resp->resp.error = QMI_ERR_NONE_V01;
    }
    else
    {
      pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
      pz_Resp->resp.error = QMI_ERR_INTERNAL_V01;
    }
  }
  else
  {

    pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
    pz_Resp->resp.error = QMI_ERR_MALFORMED_MSG_V01;
  }
  return v_RetVal;
}




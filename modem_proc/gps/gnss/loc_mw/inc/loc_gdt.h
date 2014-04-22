/******************************************************************************
  @file: loc_gtp.h
  @brief:   This module handles interface between location middleware and GTP.
  

  DESCRIPTION
   Qualcomm Location API for GTP feature

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
Copyright (c) 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
Copyright (c) 2015 Qualcomm Technologies, Inc. 
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/inc/loc_gdt.h#1 $
$DateTime: 2020/07/21 21:23:44 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
09/11/14   ah      Initial version
======================================================================*/

#ifndef LOC_GDT_H
#define LOC_GDT_H

#include "loc_client.h"
#include "aries_os_api.h"
#include "gnss_gdt.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*===========================================================================
FUNCTION loc_gdt_UploadEndStatusReq

DESCRIPTION
  This function handles QMI command to process gdt upload end status request 
 
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
                                 qmiLocGenRespMsgT_v02 *const pz_Resp);

/*===========================================================================
FUNCTION loc_gdt_SendBeginRequest

DESCRIPTION
  This function sends a GDT begin indication to AP.
 
PARAMETERS 
 
  e_ServiceId - GDT Client service ID
  (input)
 
  q_SessionId - GDT session ID
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
                                              const char * c_FilePath);

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
                                            e_GDTResponseType e_Status);

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
void loc_gdt_BeginIndicationHandler(const os_IpcMsgType *const cpz_IpcMsg);

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
void loc_gdt_EndIndicationHandler(const os_IpcMsgType *const cpz_IpcMsg);


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
void loc_gdt_BeginReqIndHandler(const os_IpcMsgType *const cpz_IpcMsg);

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
void loc_gdt_EndReqIndHandler(const os_IpcMsgType *const cpz_IpcMsg);

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
uint32 loc_gdt_BeginReqIndProxy(const int32 client_handle, const uint32 transaction_id,
                                const qmiLocGdtUploadBeginStatusIndMsgT_v02 *const p_ind);


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
int32 loc_gdt_DownloadReadyReq(const locQmiShimCltInfoStructT *cpz_Client,
                               const qmiLocGdtDownloadReadyStatusReqMsgT_v02 *const cpz_Req,
                               qmiLocGenRespMsgT_v02 *const pz_Resp);

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
                                               e_GDTTransferStatusType e_Status);

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
                               qmiLocGenRespMsgT_v02 *const pz_Resp);

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
                                  qmiLocGenRespMsgT_v02 *const pz_Resp);

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
                                               e_GDTTransferStatusType e_Status);

/*===========================================================================
FUNCTION loc_gdt_RcvDoneIndHandler

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
void loc_gdt_RcvDoneIndHandler(const os_IpcMsgType *const cpz_IpcMsg);

/*===========================================================================
FUNCTION loc_gdt_RcvDoneReqIndicationHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocEventGdtUploadEndReqIndMsgT_v02.
  GDT MP is informing GDT AP that MP has received the downloaded file.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if ipc has been processed successfully  
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_RcvDoneReqIndicationHandler(const os_IpcMsgType *const cpz_IpcMsg);

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
void loc_gdt_DownloadReadyStatusIndHandler(const os_IpcMsgType *const cpz_IpcMsg);

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
void loc_gdt_ReceiveDoneStatusIndHandler(const os_IpcMsgType *const cpz_IpcMsg);

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
void loc_gdt_DownloadEndStatusIndHandler(const os_IpcMsgType *const cpz_IpcMsg);

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
void loc_gdt_DownloadEndReqIndicationHandler(const os_IpcMsgType *const cpz_IpcMsg);

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
uint32 loc_gdt_DownloadReadyStatusReqProxy(const int32 client_handle,
                                           const uint32 transaction_id,
                                           const qmiLocGdtDownloadReadyStatusIndMsgT_v02 *const p_ind);

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
uint32 loc_gdt_ReceiveDoneStatusIndProxy(const int32 client_handle,
                                         const uint32 transaction_id,
                                         const qmiLocGdtReceiveDoneStatusIndMsgT_v02 *const p_ind);

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
uint32 loc_gdt_DownloadEndStatusIndProxy(const int32 client_handle,
                                         const uint32 transaction_id,
                                         const qmiLocGdtDownloadEndStatusIndMsgT_v02 *const p_ind);

#ifdef __cplusplus
}
#endif
#endif /* LOC_GDT_H */

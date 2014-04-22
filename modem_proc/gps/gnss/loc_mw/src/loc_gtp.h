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

Copyright (c) 2015-2020 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.


$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_gtp.h#1 $
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

#ifndef LOC_GTP_H
#define LOC_GTP_H

#include "loc_client.h"
#include "gnss_gdt.h"
#include "aries_os_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
                                               qmiLocGenRespMsgT_v02 *const pz_Resp);

/*===========================================================================
FUNCTION loc_gtp_WwanClientDloadedDataIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocInjectGtpClientDownloadedDataIndMsgT_v02 
 
PARAMETERS 
 
  p_ipc_msg_in - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gtp_WwanClientDloadedDataIndHandler(const os_IpcMsgType *const p_ipc_msg_in);


/*===========================================================================
FUNCTION loc_gtp_DownloadBeginStatusIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtDownloadBeginStatusIndMsgT_v02 
  MP has received a download begin status request from AP, now it is
  sending the indication for the request.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gtp_DownloadBeginStatusIndHandler(const os_IpcMsgType *const cpz_IpcMsg);


/*===========================================================================
FUNCTION loc_tle_sendFDCLServiceReqInd

DESCRIPTION
   This function handles the request, made by TLE module, to send QMI Indication to start
   FDCL service.
 
PARAMETERS 
    cpz_IpcMsg -Points to the recieved IPC message
DEPENDENCIES
 
RETURN VALUE 
     0 if message is sent successfully.
SIDE EFFECTS

===========================================================================*/
uint32 loc_tle_sendFDCLServiceReqInd(const os_IpcMsgType * cpz_IpcMsg);

/*===========================================================================
FUNCTION loc_tle_GetFdclBsListReq

DESCRIPTION
  This function handles QMI command to get the list of BS for which FDCL data
   is required.
 
PARAMETERS 
 
  cpz_Client - Points to the LocAPI client
  (input)

  cpz_Req -  Points to the QMI Request message received from AP
  (input)
 
  pz_Resp  - Points to the generated response
  (output)
 
DEPENDENCIES

RETURN VALUE 
  TRUE  : If the command has been handled successfully
  FALSE : Otherwise

SIDE EFFECTS

===========================================================================*/	
	
boolean loc_tle_GetFdclBsListReq(const locQmiShimCltInfoStructT  * cpz_Client,
                                 const qmiLocGetFdclBsListReqMsgT_v02 * cpz_Req,
                                 qmiLocGenRespMsgT_v02 * pz_Resp);

/*===========================================================================
FUNCTION loc_tle_sendGetFdclBsListInd

DESCRIPTION
  This function is called by LOC_MW task,  .to send an QMI indication 
  to QMI client with list of all the Base stations for which FDCL 
  data is required.
PARAMETERS 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
SIDE EFFECTS

===========================================================================*/
void loc_tle_sendGetFdclBsListInd(const os_IpcMsgType * cpz_IpcMsg);

/*===========================================================================
FUNCTION loc_tle_InjectFdclDataReq

DESCRIPTION
  This function handles QMI command to inject FDCL Data 
   is required.
 
PARAMETERS 
   cpz_Client - Points to the LocAPI client
  (input)
   cpz_Req - Points to the QMI Request message received from AP
  (input)

   pz_Resp - Points to the generated response
   (output)
 
DEPENDENCIES

RETURN VALUE 
  TRUE  : If the command has been handled successfully
  FALSE : Otherwise

SIDE EFFECTS

===========================================================================*/
boolean loc_tle_InjectFdclDataReq(const locQmiShimCltInfoStructT  * cpz_Client,
                                  const qmiLocInjectFdclDataReqMsgT_v02 * cpz_Req,
                                  qmiLocGenRespMsgT_v02 * pz_Resp);


/*===========================================================================
FUNCTION loc_tle_GetFdclBsListReq

DESCRIPTION
  This function handles QMI command to get the list of BS for which FDCL data
   is required.

PARAMETERS 
 
  cpz_Client - Points to the LocAPI client
  (input)

  cpz_Req -  Points to the QMI Request message received from AP
  (input)

  pz_Resp  - Points to the generated response
  (output)
 
DEPENDENCIES

RETURN VALUE 
  TRUE  : If the command has been handled successfully
  FALSE : Otherwise

SIDE EFFECTS

===========================================================================*/	
	
boolean loc_tle_GetFdclBsListReq(const locQmiShimCltInfoStructT * cpz_Client,
                                 const qmiLocGetFdclBsListReqMsgT_v02 * cpz_Req,
                                 qmiLocGenRespMsgT_v02 * pz_Resp);

/*===========================================================================
FUNCTION loc_tle_InjectFdclDataReq
	
DESCRIPTION
  This function handles QMI command to inject FDCL Data 
   is required.
		 
PARAMETERS	 
   cpz_Client - Points to the LocAPI client
   (input)
   cpz_Req - Points to the QMI Request message received from AP
   (input)
   
   pz_Resp - Points to the generated response
   (output)
 
DEPENDENCIES

RETURN VALUE 
  TRUE  : If the command has been handled successfully
  FALSE : Otherwise

SIDE EFFECTS

===========================================================================*/
boolean loc_tle_InjectFdclDataReq(const locQmiShimCltInfoStructT * cpz_Client,
                                        const qmiLocInjectFdclDataReqMsgT_v02 * cpz_Req,
                                        qmiLocGenRespMsgT_v02 * pz_Resp);

/*===========================================================================
FUNCTION loc_tle_sendInjectFdclDataInd

DESCRIPTION
 This function is called by LOC_MW task,  .to send an QMI indication 
  to QMI client Qmi client, with the status. of Injection of FDCL data.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
   
SIDE EFFECTS

===========================================================================*/
void loc_tle_sendInjectFdclDataInd(const os_IpcMsgType *cpz_IpcMsg);

/*===========================================================================
FUNCTION loc_tle_Fdcl2_SendBsObsDataServiceReqInd

DESCRIPTION
IPC Handler for LM_MIDDLEWARE_MSG_ID_SEND_FDCL2_SERVICE_REQ_IND.
This function handles the request, made by TLE module, and sends Indication
QMI_LOC_EVENT_BS_OBS_DATA_SERVICE_REQ_IND to client to start initiation
of crowdsourced data upload

PARAMETERS
cpz_IpcMsg Points to the received IPC message
===========================================================================*/

uint32 loc_tle_Fdcl2_SendBsObsDataServiceReqInd(const os_IpcMsgType * cpz_IpcMsg);

/*===========================================================================
FUNCTION loc_tle_Fdcl2_GetBsObsDataReq

DESCRIPTION
This function handles QMI request QMI_LOC_GET_BS_OBS_DATA_REQ from AP
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
=================================================================================*/
boolean loc_tle_Fdcl2_GetBsObsDataReq(const locQmiShimCltInfoStructT * cpz_Client,
                                      const qmiLocGetBsObsDataReqMsgT_v02 * cpz_Req,
                                      qmiLocGenRespMsgT_v02 * pz_Resp);

/*===========================================================================
FUNCTION loc_tle_Fdcl2_sendGetBsObsDataInd

DESCRIPTION
IPC handler for LM_MIDDLEWARE_MSG_ID_SEND_GET_FDCL2_BS_OBS_DATA_IND.
This function handles the request made by TLE and sends indication
QMI_LOC_GET_BS_OBS_DATA_IND to client with the crowdsourced base station
data that has to be uploaded.

PARAMETERS
cpz_IpcMsg - Points to the received IPC message
(input)
==========================================================================*/
void loc_tle_Fdcl2_sendGetBsObsDataInd(const os_IpcMsgType * cpz_IpcMsg);

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
                                         const qmiLocGetWwanDataIndMsgT_v02 * p_ind);

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

void loc_tle_sendGetWwanDataInd(const os_IpcMsgType * cpz_IpcMsg);

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
                                   qmiLocGenRespMsgT_v02 * pz_Res);

#ifdef __cplusplus
}

#endif

#endif /* LOC_GTP_H */

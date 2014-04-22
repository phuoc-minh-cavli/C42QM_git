/******************************************************************************
  @file: loc_for_tle.h
  @brief:   This module handles interface between location middleware and GTP.
  

  DESCRIPTION
   Qualcomm Location API for TLE feature

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
Copyright (c) 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
Copyright (c) 2015 Qualcomm Technologies, Inc. 
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/inc/loc_for_gtp.h#1 $
$DateTime: 2020/07/21 21:23:44 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
10/11/17   akaspand      Initial version
======================================================================*/
#ifndef LOC_FOR_TLE_H
#define LOC_FOR_TLE_H
#include "loc_client.h"
#include "aries_os_api.h"
#include "gnss_gdt.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* The following data structure is used by gtp client on MP before starting
   a download session. The data structre provides information about the 
   current status of the modem */ 
typedef struct
{
  e_GDTServiceIDType e_ServiceId;
  uint32 q_SessionId;
  uint32 q_RespTimeoutInterval;
  uint32 q_FilePathLen;
  char *c_FilePath;
  uint32 q_ClientInfoLen; 
  char *c_ClientInfo; 
  uint32 q_MobileStatusLen; 
  char *c_MobileStatus;
  boolean b_PowerAllowanceValid; 
  uint32 q_PowerBudgetAllowance; 
  boolean b_PowerBudgetInfoValid; 
  uint32 q_PowerBudgetInfo;
  boolean b_DownloadRequestMaskValid;
  uint32 q_DownloadRequestMask;
} loc_gtp_DownldStartRequestType;

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
boolean loc_gtp_DownloadBeginReqInd(const loc_gtp_DownldStartRequestType *cpz_Req);



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
                                            const qmiLocInjectGtpClientDownloadedDataIndMsgT_v02 * const p_ind);

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
    0 if message is sent successfully.
SIDE EFFECTS

===========================================================================*/
uint32 loc_tle_RequestFdclService(qmiLocEventFdclServiceReqIndMsgT_v02* p_ind);
								
/*===========================================================================
FUNCTION loc_tle_GetFdclBsListIndProxy

DESCRIPTION
  This function is called  by TLE to  indicates the LOC_MW to send an QMI indication to QMI client 
  with list of all the base stations for which FDCL data is required.
 
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
    0 if message is sent successfully.
SIDE EFFECTS

===========================================================================*/
uint32 loc_tle_GetFdclBsListIndProxy(const int32 client_handle,
                                                 const uint32 transaction_id,
                                                 const qmiLocGetFdclBsListIndMsgT_v02 *const p_ind);

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
0 if message is sent successfully. 
SIDE EFFECTS

===========================================================================*/
uint32 loc_tle_InjectFdclDataIndProxy(const int32 client_handle,
                                                 const uint32 transaction_id,
                                                 const qmiLocInjectFdclDataIndMsgT_v02 * p_ind);

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
uint32 loc_tle_Fdcl2_BsObsDataServiceReqIndProxy(qmiLocEventBsObsDataServiceReqIndMsgT_v02* p_ind);

/*===========================================================================
FUNCTION loc_tle_Fdcl2_GetBsObsDataIndProxy

DESCRIPTION
This function is called by TLE indicating LOC_MW
(via IPC LM_MIDDLEWARE_MSG_ID_SEND_GET_FDCL2_BS_OBS_DATA_IND) to send a indication
QMI_LOC_GET_BS_OBS_DATA_IND to AP with the crowdsourced base station
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
                                          const qmiLocGetBsObsDataIndMsgT_v02 * p_ind);
#ifdef __cplusplus
}
#endif
#endif 


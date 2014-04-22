/*============================================================================
 @file loc_aon.h

 loc MW AlwaysOn service module

 GENERAL DESCRIPTION

 This file defines the AlwaysOn service module in loc middleware.

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2014 Qualcomm Technologies Incorporated.
 All Rights Reserved
 Qualcomm Confidential and Proprietary
 Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
 Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_aon.h#1 $
 $DateTime: 2020/07/21 21:23:44 $
 $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 04/06/15   sj      LB 2.0 integration
 09/22/14   sj      Initial version

 =============================================================================*/

#ifndef _LOC_AON_H_
#define _LOC_AON_H_
#include "aries_os_api.h"
#include "location_service_iot_v02.h"

//Min itnerval b/w fixes that are batched
#define LOC_AON_BATCHING_MINIMUM_MININTERVAL (1000)

typedef enum 
{
  eLOC_AON_SUCCESS = 0,
  eLOC_AON_ERR_START_PENDING = 1,
  eLOC_AON_ERR_AON_REQ_NOT_FOUND = 2
}locAonStatusEnumT;

typedef enum 
{
  eLOC_AON_REQ_DBT,
  eLOC_AON_REQ_BATCH,
  eLOC_AON_REQ_OTB,
}locAonReqTypeEnumT;

/** locAonForceStopAll
* Stop all batching requests from the specified client 
* @return : none.
*/
void locAonForceStopAll
(  
  loc_client_handle_type locClientHandle
);

/** locAonInit
 * initialize loc MW AlwaysOn service module and register the callbacks with GM
 * module.
 * @return : TRUE ; loc AlwaysOn module was initialized correctly
 *         : FALSE; loc AlwaysOn module could not be initialized.
 */
boolean locAonInit(void);

/** locAonBatchStartReq
* Start batching request, stops the existing request if the same client already
  has batching ongoing. Restarts batching with new params.
* @return None
*/

void locAonBatchStartReq 
(
  loc_client_handle_type locClientHandle,
  const qmiLocStartBatchingReqMsgT_v02 *pStartBatchingReq,
  qmiLocGenRespMsgT_v02 *pResp
);

/** locAonBatchStopReq
* Stop batching request.
* @return None
*/
void locAonBatchStopReq
(
  loc_client_handle_type locClientHandle,
  const qmiLocStopBatchingReqMsgT_v02 *pStopBatchingReq,
  qmiLocGenRespMsgT_v02 *pResp
);

/** locAonDbtStartReq
* Starts DBT request.
* @return None
*/
boolean locAonDbtStartReq
(
  loc_client_handle_type locClientHandle,
  const qmiLocStartDbtReqMsgT_v02 *pStartDbtReq,
  qmiLocGenRespMsgT_v02 *pResp
);

/** locAonDbtStopReq
* Stop DBT request.
* @return None
*/
boolean locAonDbtStopReq
( 
  loc_client_handle_type locClientHandle,
  const qmiLocStopDbtReqMsgT_v02 *pStopDbtReq,
  qmiLocGenRespMsgT_v02 *pResp
);

/** locAonIpcHandler
* Forwards the AON IPC's to aon code
* @return None
*/
boolean locAonIpcHandler(os_IpcMsgType* p_msg );

/** locAonTimerHandler
* Hnadles timer expirys set by AON 
* @return None 
*/
void locAonTimerHandler(uint32 timerId, uint32 timerParam);

/** locAonCheckIfLocClientPresent
* Determine if the given locClientHandle is present in the 
* request queue 
* @return : TRUE: Present
*           FALSE: Absent
*/
boolean locAonCheckIfLocClientPresent
(
  loc_client_handle_type locClientHandle,
  locAonReqTypeEnumT reqType
);

/** locAonOtbStartReq
* Starts Outdoor Trip Batching request.
* @return None
*/
boolean locAonOtbStartReq
( 
   loc_client_handle_type l_ClientHandle,
   const qmiLocStartOutdoorTripBatchingReqMsgT_v02 *pz_StartOtbgReq,
   qmiLocGenRespMsgT_v02 *pz_Resp
);

/** locAonOtbStopReq
* Stop Outdoor trip batching request.
* @return None
*/
void locAonOtbStopReq
(
  loc_client_handle_type               l_ClientHandle,
  const qmiLocStopBatchingReqMsgT_v02 *pz_StopBatchingReq,
  qmiLocGenRespMsgT_v02               *pz_Resp
);

/** locAonSendStartBatchingInd
* Send QMI_LOC batching indication messages.
* @return None
*/
void locAonSendStartBatchingInd
(
   loc_client_handle_type        l_ClientHandle,
   uint32                        q_LocReqId,
   qmiLocStatusEnumT_v02         e_QmiLocStatus,
   qmiLocBatchingTypeEnumT_v02   e_QmiLocBatchType
);


#endif // _LOC_AON_H_

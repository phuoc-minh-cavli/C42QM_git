/******************************************************************************
  @file: loc_csm.c
  @brief:   This module handles interface for crowd source manager


  DESCRIPTION
   Qualcomm Location API Crowd Source Manager Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_csm.c#1 $
$DateTime: 2020/07/21 21:23:44 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
09/20/16   ss       Intial version
======================================================================*/
#include "loc_csm.h"
#include "loc_api_2.h"
#include "loc_api_internal.h"
#include "loc_utils.h"
#include "loc_qmi_shim.h"
#include "tm_api.h"


/**
 * Function to handle QMI command from control point to
 * configure crowd source manager
 *
 * @param pz_ClientInfo LocAPI client handle
 * @param pz_Req        QMI request
 * @param pz_Resp       QMI response
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
boolean locCsm_ControlReq(loc_client_info_s_type*    const pz_ClientInfo,
                          const qmiLocCrowdSourceManagerControlReqMsgT_v02*  const pz_Req,
                          qmiLocGenRespMsgT_v02*     const pz_Resp)
{
  fusion_csm_ControlReqType z_csmControlReq = {0};
  if( (NULL == pz_ClientInfo) ||
      (NULL == pz_Req) ||
      (NULL == pz_Resp)
    )
  {

    LOC_MSG_ERROR("locCsm_ControlReq Null pointer received",0,0,0);
    return FALSE;
  }

  z_csmControlReq.l_Client_handle = pz_ClientInfo->client_handle;
  z_csmControlReq.q_TransactionId = loc_qmiloc_get_transaction_id();

  z_csmControlReq.z_csmControlReq = *(pz_Req);

  return tm_fusion_csm_handle_control_req(&z_csmControlReq);
}

/**
 * Function to handle QMI command from control point to 
 * read crowd sourced data
 *
 * @param pz_ClientInfo LocAPI client handle
 * @param pz_Req        QMI request
 * @param pz_Resp       QMI response
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
boolean locCsm_ReadCsDataReq(loc_client_info_s_type*    const pz_ClientInfo,
                             const qmiLocCrowdSourceManagerReadDataReqMsgT_v02*  const pz_Req,
                             qmiLocGenRespMsgT_v02*     const pz_Resp)
{
  return tm_fusion_csm_handle_read_cs_data_req((qmiLocCrowdSourceManagerReadDataReqMsgT_v02 *)pz_Req);
}
/**
 * Function to handle crowd source manager control indication from Location engine to
 * control point
 *
 * @param int32  client handle
 * @param transaction_id       Transanction Id
 * @param p_ind       QMI Indication
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
boolean locCsm_ControlInd(const int32 client_handle,
                          const uint32 transaction_id,
                          const qmiLocCrowdSourceManagerControlIndMsgT_v02*  const p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_CROWDSOURCE_MANAGER_CONTROL_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("locCsm_ControlInd: "
                  "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
 * This function is called by LocMW task loop to handle ipc message for QMI
 * indication qmiLocCrowdSourceManagerControlIndMsgT_v02.
 *
 * @param cpz_IpcMsg       Pointer to IPC message
 *
 * @return Void
 */
void locCsm_ControlReqIndicationdHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_MED("locCsm_ControlReqIndicationdHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocCrowdSourceManagerControlIndMsgT_v02 z_Ind;


    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_CROWDSOURCE_MANAGER_CONTROL_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_MED("[LOC] locCsm_ControlReqIndicationdHandler. client: %d, tx id: %u, status: %d",
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
    LOC_MSG_ERROR("[LOC] locCsm_ControlReqIndicationdHandler: error code: %u", q_ErrorCode, 0, 0);
  }
}


/**
 * Function to handle QMI command from control point to
 * set external power config
 *
 * @param pz_Req The external power configuration information
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
boolean locCsm_HandleSetExternalPowerConfigReq(const qmiLocSetExternalPowerConfigReqMsgT_v02 *pz_Req)
{
  return(tm_fusion_csm_handle_set_external_power_config_req(pz_Req));
}

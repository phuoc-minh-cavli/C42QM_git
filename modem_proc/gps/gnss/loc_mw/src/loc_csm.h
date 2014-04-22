/******************************************************************************
  @file: loc_csm.h
  @brief:   This module handles interface between location middleware and Crowd Source Manager.
  

  DESCRIPTION
   Qualcomm Location API for Crowd Source Manage

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
Copyright (c) 2016 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.


$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_csm.h#1 $
$DateTime: 2020/07/21 21:23:44 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
09/20/16   ss      Initial version
======================================================================*/

#ifndef LOC_CSM_H
#define LOC_CSM_H

#include "loc_client.h"

#ifdef __cplusplus
extern "C" {
#endif


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
                          qmiLocGenRespMsgT_v02*     const pz_Resp);

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
                             qmiLocGenRespMsgT_v02*     const pz_Resp);

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
boolean locCsm_ControlInd(const int32 client_handl,
                          const uint32 transaction_id, 
                          const qmiLocCrowdSourceManagerControlIndMsgT_v02*  const p_ind);

/**
 * This function is called by LocMW task loop to handle ipc message for QMI 
 * indication qmiLocCrowdSourceManagerControlIndMsgT_v02.
 *
 * @param cpz_IpcMsg       Pointer to IPC message
 *
 * @return Void
 */
void locCsm_ControlReqIndicationdHandler(const os_IpcMsgType *const cpz_IpcMsg);

/**
 * Function to handle QMI command from control point to
 * set external power config
 *
 * @param pz_Req The external power configuration information
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
boolean locCsm_HandleSetExternalPowerConfigReq(const qmiLocSetExternalPowerConfigReqMsgT_v02 *pz_Req);

#ifdef __cplusplus
}
#endif


#endif /*LOC_CSM_H*/

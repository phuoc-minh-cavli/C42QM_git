/*======================================================================

              Fusion Crowd Source Manager  API header file

 GENERAL DESCRIPTION
  This file contains API declarations for Fusion Crowd Source Manager module

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2016 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/fusion_csm/api/fusion_csm_api.h#1 $

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
12/18/16   saig	 Adding On-Demand triggers to start and stop
09/20/16   ss    Initial creation of file.
======================================================================*/

#ifndef FUSION_CSM_API_H
#define FUSION_CSM_API_H
#include "comdef.h"
#include "customer.h"
#include "gps_common.h"
#include "gnss_common.h"
#include "gnss_gdt.h"
#include "location_service_iot_v02.h"
#include "sm_api.h"
#include "pdapi.h"
#include "gps_nw_common.h"

#ifdef __cplusplus
extern "C" {
#endif


#define FUSION_CSM_WIFI_CS_VERSION_MAJOR 2
#define FUSION_CSM_WIFI_CS_VERSION_MINOR 0

#define FUSION_CSM_WIFI_CS_OPERATION_MODE_PURE 0x0
#define FUSION_CSM_WIFI_CS_OPERATION_MODE_HYBRID 0x1

typedef struct
{
  qmiLocCrowdSourceManagerControlReqMsgT_v02 z_csmControlReq;
  int             l_Client_handle;
  uint32          q_TransactionId;
} fusion_csm_ControlReqType;

/********************************************************************
*
* fusion_csm_HandleControlReq
*
* Function description:
* API to forward csm config data to fusion csm
*
* Parameters:
* fusion_csm_ControlReqType : csm config.
* Return:
* boolean pass/fail
*
*********************************************************************
*/
boolean fusion_csm_HandleControlReq(const fusion_csm_ControlReqType* p_csmControlReq);

/** API to be called when GNSS fix needs to be given to fusion csm **/
boolean fusion_csm_gnssPosFixCbk(const sm_FixInfoStructType *pGnssFix, uint32 b_errorType, uint32 q_powerConsumed);

/** Call back function to report wifi scans **/
#if 0
boolean fusion_csm_wifiDiscScanCbk( pdsm_wifi_scan_info_ext_s_type *p_wifi_scan, uint32 q_powerConsumed);
#endif
/** Call back function for upload begin resp **/
void fusion_csm_procUploadBeginResp(const gdt_StartResponseType * cpz_BeginResp);

/** Call back function for open resp **/
void fusion_csm_procOpenResp(const gdt_OpenResponseType *cpz_OpenResponse);

/** Call back function for upload close resp **/
void fusion_csm_procCloseResp(const gdt_CloseResponseType * cpz_CloseResponse);

/** Call back function for upload end resp **/
void fusion_csm_procUploadEndResp(const gdt_EndResponseType *cpz_EndResponse);

/** Call back function for send ack resp **/
void fusion_csm_procSendAck(const gdt_SendAckResponseType *cpz_AckMsg);

/** Call back function to report ranging wifi scans **/
#if 0
boolean fusion_csm_wifiRangingScanCbk( pdsm_RangingScanRespType *p_wifi_scan, uint32 q_powerConsumed);
#endif
/********************************************************************
*
* fusion_csm_ReportTimerExpiry
*
* Function description:
* This function is used to report fusion csm timer expiry
*
* Parameters:
* q_timerId: Timer Id which got expired
* q_param  : parameter passed while starting timer
* Return:
* void
*
*********************************************************************
*/
void fusion_csm_ReportTimerExpiry(uint32 q_timerId,uint32 q_param);

/*===========================================================================
fusion_csm_CellDbUpdate

Description:

   This function is used by TM core to update the cell db information

Parameters:
  cgps_ActiveRATEnumType e_TriggerRAT: RAT that triggered the update
  const cgps_CellInfoCachedStruct *cell_db_data_ptr: Pointer to cached cell db information

Return value:
  void
=============================================================================*/
void fusion_csm_CellDbUpdate(const cgps_ActiveRATEnumType e_TriggerRAT, const cgps_CellInfoCachedStruct *pz_cell_db_data );

/********************************************************************
*
* fusion_csm_ReportBatteryStatus
*
* Function description:
* This function is used to report battery status
*
* Parameters:
* bool: charging status, TRUE if battery is charging else FALSE
* uint8 : battery level in percentage ( 0 to 100 ) .
*
* Return:
* int16 result
*
*********************************************************************
*/
int16 fusion_csm_ReportBatteryStatus(qmiLocSetExternalPowerConfigReqMsgT_v02* pz_Req);

/********************************************************************
*
* fusion_csm_ReportPedStatus
*
* Function description:
* This function is used to report Ped status 
*
* Parameters:
*gnss_MgpPedStatusEnumType : ped status
*
* Return:
* int16 : result
*
*********************************************************************
*/
int16 fusion_csm_ReportPedStatus(gnss_MgpPedStatusEnumType e_PedStatus);

/*===========================================================================
fusion_csm_HandleEulaUpdate

Description:

   This function is used to notify fusion_csm module an eula change

Parameters:
  uint8 eula_enabled: 1 if eula is on, 0 otherwise

Return value:
  void
=============================================================================*/
void fusion_csm_HandleEulaUpdate(uint8 eula_enabled);

/*===========================================================================
fusion_csm_HandleE911Update


Description:

   This function is used to notify fusion_csm module of E911 call origination or end

Parameters:
  uint8 u_CallOrig: 1 if call origination, 0 otherwise

Return value:
  void
=============================================================================*/
void fusion_csm_HandleE911Update(uint8 u_CallOrig);

/*===========================================================================
fusion_csm_GmEventHandler

Description:

   This function is used to notify fusion_csm module about gm events

Parameters:
   q_msg_type: GM Message Type
  *p_msg_data: Pointer to the message

Return value:
  Boolean: Pass/Fail
=============================================================================*/
boolean fusion_csm_GmEventHandler(uint32 q_msg_type,  void  *p_msg_data );

/*===========================================================================
fusion_csm_UploadCsDataToAPReq

Description:

   This function is used to request fusion_csm to send Data to Ap if possible

Parameters:
   p_ReqMsg: Pointer to request Message

Return value:
  Boolean: Pass/Fail
=============================================================================*/
boolean fusion_csm_UploadCsDataToAPReq(const qmiLocCrowdSourceManagerReadDataReqMsgT_v02 *p_ReqMsg);
/*===========================================================================
fusion_csm_InjectWifiEnabledStatus

Description:

   This function is used to notify fusion_csm module an wifi enable status

Parameters:
  pdsm_wifi_enabled_status_ind_type

Return value:
  void
=============================================================================*/
#if 0
void fusion_csm_InjectWifiEnabledStatus(pdsm_wifi_enabled_status_ind_type e_wifi_enabled_status_ind_info);
#endif
/********************************************************************
*
* fusion_csm_InjectWifiAttachStatus
*
* Function description:
* Injets Wifi attachment status indication info
*
* Parameters: 
*  p_wifi_attachment_status_ind_info - Pointer to Wifi attachment status inidcation info stuct
*
* Return:
* void
*  
*********************************************************************
*/  
#if 0
void fusion_csm_InjectWifiAttachStatus(pdsm_wifi_attachment_status_ind_info *p_wifi_attachment_status_ind_info);
#endif
/*===========================================================================
fusion_csm_sensorLeStatusCbk

Description:

   This function is used to handle Sensor Le Status

Parameters:
   p_SensorLEStatus: Pointer to Sensor Le Status

Return value:
  Void
=============================================================================*/
void fusion_csm_sensorLeStatusCbk(sm_SensorLEStatusStructType *p_SensorLEStatus);
/*===========================================================================
fusion_csm_sensorLeRelDispCbk

Description:

   This function is used to handle Sensor Le Relative Displacements

Parameters:
   p_RelDisp: Pointer to Sensor Le Relative Displacements

Return value:
  Void
=============================================================================*/
void fusion_csm_sensorLeRelDispCbk(const sm_SensorLERelDispStructType * const p_RelDisp);

/*===========================================================================
fusion_csm_HandleGpsLockUpdate

Description:

   This function is used to handle GPS lock updates

Parameters:
   gps_lock: GPS Locks status

Return value:
  Void
=============================================================================*/
void fusion_csm_HandleGpsLockUpdate(pdsm_gps_lock_e_type  gps_lock);

#ifdef __cplusplus
}
#endif

#endif /*FUSION_CSM_API_H*/

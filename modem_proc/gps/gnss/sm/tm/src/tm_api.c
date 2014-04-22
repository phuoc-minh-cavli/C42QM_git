/*===========================================================================

                            TM_API

DESCRIPTION: TM's APIs.


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 - 2021 Qualcomm Technologies, Inc.
  Confidential and Proprietary - Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/src/tm_api.c#2 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  01/08/20    sai     Added API to query SM if GTP 2.0 fix is possible or not
  08/10/16    mj      Handle Leap Second Updates from ME
  07/07/16    gurdars Updated XTRA3.x indication API's to pass the constellation info
  06/06/16    mj      Add GERA support for AP-MP
  05/01/15    rh      Added get gps lock TM API
  10/27/14    rk      Added LPP RSTD >24 measurement processing support
  01/31/13    mj      Modify tm_api_reg_task_init_notifier to handle race condition
  01/12/13    gk      LPP ECID changes
  04/18/12    ssu     Added tm_api_update_best_avail_pos()
  03/01/12    rh      Added tm_api_send_engine_error_recovery_report()
  06/10/09    gk      Added Get UTC offset from NV
  1/12/09     atien   XTRA-T support
  09/15/08    gk      Rename the on-demand struct to sm_ from cgps_
  4/25/08     GK      On-Demand Changes.
  7/10/07     LT      Initial check-in.
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "aries_os_api.h"
#include "gps_common.h"
#include "tm_common.h"
#include "tm_api.h"
#include "tm_data.h"
#include "tm_xtra.h"

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                        GLOBAL DATA DECLARATIONS                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                          FUNCTION DECLARATIONS                          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                          FUNCTION DEFINITIONS                           */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

uint8 tm_map_client_threadid_to_index(os_ThreadIdEnumType clientThreadId)
{
  switch (clientThreadId)
  {
    case THREAD_ID_LOC_MIDDLEWARE:
		return (uint8)TM_LOC_MW_CLIENT_INDEX;

	default:
		return (uint8)TM_UNSUPPORTED_CLIENT_INDEX;
  }
}
/*******************************************************************************
 * tm_api_store_ftcal_tt_data #9x05 remove
 *
 * Function description:
 *
 * Parameters:
 *
 *
 * Return value:
 *
 *  void
 *
 *******************************************************************************/
void tm_api_store_ftcal_tt_data (cgps_FtcalTTStruct* p_TTData)
{
return;
}
/*===========================================================================
FUNCTION tm_api_update_gps_state_info

DESCRIPTION: for entities outside TM to provide TM with position and its
associated state info update.

RETURN VALUE

DEPENDENCIES
=============================================================================*/

void tm_api_update_gps_state_info ( const sm_FixReportStructType* const p_PosInfo )
{

  os_IpcMsgType                  *ipc_msg_ptr;
  sm_FixReportStructType         *ipc_data_ptr;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"TM_API: POS PUNC TUNC updated",0,0,0);

  ipc_msg_ptr = os_IpcCreate(sizeof(sm_FixReportStructType), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (sm_FixReportStructType*)ipc_msg_ptr->p_Data; /*lint !e826 */

    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_BEST_AVAIL_POS_REPORT;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(sm_FixReportStructType);

    *ipc_data_ptr = *p_PosInfo;

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Failed to send IPC mesage",0,0,0);
      (void)os_IpcDelete(ipc_msg_ptr);
      return;
    }
    return; /*lint !e506 */
  } /* End ipc_msg_ptr */
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Failed to create IPC mesage",0,0,0);
    return;
  } /* End ipc_msg_ptr */

}

/*******************************************************************************
 * tm_api_report_utc_offset
 *
 * Function description:
 *  Get UTC offset from TM
 *
 * Parameters:
 *
 *  p_UtcOffset - Pointer to UTC offset information
 *
 * Return value:
 *
 *  void
 *
 *******************************************************************************/
void tm_api_report_utc_offset(uint8 *p_UtcOffset)
{
  (void)tm_core_get_utc_offset(p_UtcOffset);
}
/*******************************************************************************
 * tm_api_XTM_NV_status_report
 *
 * Function description:
 *  Called by XTRAT module to report the success of NV write to TM thread.
 *
 * Parameters:
 * which nv item has been set
 *
 * Return value:
 * none
 *
 *******************************************************************************/

boolean tm_api_xtm_nv_status_report(tm_xtm_struct_nv_write_status* xtm_nv_set )
{
  os_IpcMsgType                  *ipc_msg_ptr;
  tm_xtm_struct_nv_write_status   *ipc_data_ptr;

  ipc_msg_ptr = os_IpcCreate(sizeof(tm_xtm_struct_nv_write_status), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_data_ptr = (tm_xtm_struct_nv_write_status*)ipc_msg_ptr->p_Data;
    switch (xtm_nv_set->xtm_nv_item_set)
    {
      case TM_XTM_SET_CLIENT_TOKEN:
        ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_XTM_CLIENT_TOKEN;
        break;

      case TM_XTM_SESSION_CONTROL:
        ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_XTM_SESSION_CONTROL;
        break;
    }

    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(tm_xtm_struct_nv_write_status);
    *ipc_data_ptr = *xtm_nv_set;

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_API: Failed to send IPC message (tm_api_xtm_nv_status_report)",0,0,0);
      (void)os_IpcDelete(ipc_msg_ptr);
      return FALSE;
    }
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM_API: successfully sent an IPC message (tm_api_xtm_nv_status_report)",0,0,0);
      return TRUE;
    }
  } /* End ipc_msg_ptr */
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_API: Failed to create IPC message (tm_api_xtm_nv_status_report)",0,0,0);
    return FALSE;
  } /* End ipc_msg_ptr */
}



/**
@brief Calls the tm_xtra function for indicating xtra injection
       has completed
@param q_AcceptedMask:Mask indicating the accpeted constellation's by PE.
@return None
*/
void tm_api_xtra_inj_done(uint32  q_AcceptedMask)
{
  tm_xtra_inj_done(q_AcceptedMask);
}

/*******************************************************************************
 * tm_api_report_xooffset_info
 *
 * Function description:
 *  Called by LM to update TM with the latest XO offset info
 *
 * Parameters: 
 *
 *  pz_ReportXOOffsetInfo - XO offset Info
 *
 * Return value: 
 *
 *  void
 *
 *******************************************************************************/

void tm_api_report_xooffset_info 
(
  const sm_ReportXOOffsetInfoStructType *pz_ReportXOOffsetInfo
)
{
  os_IpcMsgType *pz_IpcMsgPtr  = NULL;
  
  pz_IpcMsgPtr = os_IpcCreate(sizeof(sm_ReportXOOffsetInfoStructType), 
                             IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if( (pz_IpcMsgPtr != NULL) && (pz_ReportXOOffsetInfo != NULL) )
  {
    sm_ReportXOOffsetInfoStructType *pz_IpcDataPtr;
    pz_IpcDataPtr = (sm_ReportXOOffsetInfoStructType *) pz_IpcMsgPtr->p_Data;

    pz_IpcMsgPtr->q_MsgId          = (uint32) TM_CORE_MSG_ID_XO_OFFSET_INFO;
    pz_IpcMsgPtr->q_DestThreadId   = (uint32) THREAD_ID_SM_TM;
    pz_IpcMsgPtr->q_Size           = sizeof(*pz_IpcDataPtr);

    /* Copy all the data over */
    pz_IpcDataPtr->l_RefOffsetPpb = pz_ReportXOOffsetInfo->l_RefOffsetPpb;
    pz_IpcDataPtr->l_ClkFreqBiasPpb = pz_ReportXOOffsetInfo->l_ClkFreqBiasPpb;
    pz_IpcDataPtr->l_ClkFreqBiasUncPpb = pz_ReportXOOffsetInfo->l_ClkFreqBiasUncPpb;
    pz_IpcDataPtr->v_IsDataValid = pz_ReportXOOffsetInfo->v_IsDataValid;
    
    if (!os_IpcSend(pz_IpcMsgPtr, (uint32) THREAD_ID_SM_TM))
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
          "tm_api_report_xooffset_info: Failed to send IPC message");
      (void)os_IpcDelete(pz_IpcMsgPtr);   
    }
  } 
  else
  {
      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
           "pz_IpcMsgPtr NULL? %d,  pz_ReportXOOffsetInfo NULL? %d",
           (pz_IpcMsgPtr == NULL), (pz_ReportXOOffsetInfo == NULL));
  }
}
  
/*******************************************************************************
 * tm_api_report_wbiq_info
 *
 * Function description:
 *  Called by LM to update TM with the latest WBIQ FFT info
 *
 * Parameters:
 *
 *  p_ReportWBIQInfo - WBIQ Info
 *
 * Return value:
 *
 *  void
 *
 *******************************************************************************/

void tm_api_report_wbiq_info (const sm_ReportWBIQInfoStructType * p_ReportWBIQInfo)
{
  os_IpcMsgType                  *ipc_msg_ptr;
  int i;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"TM_API: Report WBIQ Info",0,0,0);

  ipc_msg_ptr = os_IpcCreate(sizeof(sm_ReportWBIQInfoStructType), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if( (ipc_msg_ptr != NULL) && (p_ReportWBIQInfo != NULL) )
  {
    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MGP_WBIQ_INFO;
    ipc_msg_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    ipc_msg_ptr->q_Size           = sizeof(sm_ReportWBIQInfoStructType);

    /* Copy all the data over */
    memscpy((void *)ipc_msg_ptr->p_Data, sizeof(sm_ReportWBIQInfoStructType),
            (void *)p_ReportWBIQInfo, sizeof(sm_ReportWBIQInfoStructType));

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
          "tm_api_report_wbiq_info: Failed to send IPC message");
      (void)os_IpcDelete(ipc_msg_ptr);   
    }
  } 
  else
  {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
          "tm_api_report_wbiq_info: Input parameter check failed");
  } 

}

/*******************************************************************************
 * tm_api_nav_config_change_resp
 *
 * Function description:
 *  Called by LM to update TM with the result of the NAV config change cmd.
 *
 * Parameters:
 *
 *  u_GnssConfig - Current GNSS configuration
 *
 * Return value:
 *
 *  void
 *
 *******************************************************************************/
void tm_api_nav_config_change_resp (const uint8 u_GnssConfig)
{
  os_IpcMsgType                       *p_IpcMsg;

  p_IpcMsg = os_IpcCreate(sizeof(uint8), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (p_IpcMsg != NULL)
  {
    p_IpcMsg->q_MsgId          = (uint32)TM_CORE_MGP_NAV_CONFIG_CHANGE_RESP;
    memscpy( p_IpcMsg->p_Data, sizeof(uint8), (void*)&u_GnssConfig, sizeof(uint8));

    if (!os_IpcSend(p_IpcMsg, (uint32)THREAD_ID_SM_TM))
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Failed to send IPC message");
      (void)os_IpcDelete(p_IpcMsg);
    }
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Failed to create IPC message");
  }
}
/*=============================================================================

FUNCTION
  tm_api_send_engine_error_recovery_report

DESCRIPTION
  TM API to send engine error recovery report to QMI

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

=============================================================================*/

void tm_api_send_engine_error_recovery_report( gnss_engine_err_recovery_rpt *pErrReport )
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"tm_api_send_engine_error_recovery_report",0,0,0);

  ipc_msg_ptr = os_IpcCreate(sizeof(gnss_engine_err_recovery_rpt), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    ipc_msg_ptr->q_MsgId          = (uint32)TM_CORE_MSG_ID_ENGINE_ERROR_RECOVERY_REPORT;

    memscpy((void*)(ipc_msg_ptr->p_Data), sizeof(*pErrReport), (void*)(pErrReport), sizeof(*pErrReport));

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Failed to send IPC mesage",0,0,0);
      (void)os_IpcDelete(ipc_msg_ptr);
    }
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Failed to create IPC mesage",0,0,0);
  }
}


/*=============================================================================

FUNCTION
  tm_api_send_dpo_status_report

DESCRIPTION
  TM API to send engine error recovery report to SM .

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

=============================================================================*/

void tm_api_send_dpo_status_report( sm_DpoStatusReportStructType *p_DpoStatusReport )
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"tm_api_send_dpo_status_report",0,0,0);

  msg_id = (uint32)TM_CORE_MSG_ID_ENGINE_DPO_STATUS_REPORT;
  size   = sizeof(sm_DpoStatusReportStructType);

  /* Send the MSG to SM LM thread */
  ipc_msg_ptr = sm_send_message(msg_id, size,
    (uint32)THREAD_ID_SM_TM, (void *)p_DpoStatusReport);

  if(ipc_msg_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Failed to send error report IPC mesage",0,0,0);
  }
}

/*
******************************************************************************
* tm_read_qmi_services_configuration
*
* Function description:
*
*  This function is called by QMI services to read which QMI services are
*  enabled. This function can be called only after SM has been initialized.
*
* Parameters:
*  output_cfg : QMI Configuration read from NV
*
* Return value:
*  NONE
*
* ******************************************************************************
*/
void tm_read_qmi_services_configuration( pdsm_lite_QMIServiceConfig* output_cfg)
{
  if( NULL == output_cfg )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Null pointer passed!!", 0, 0, 0);
    return;
  }

  memscpy( output_cfg, sizeof(*output_cfg), &tm_core_info.config_info.qmi_cfg, sizeof(tm_core_info.config_info.qmi_cfg));
  return;
}
/*
******************************************************************************
* tm_retrieve_IMSI
*
* Function description:
*
*  This function is called by XTM to request TM core to retrieve the IMSI
*
* Parameters:
*  NONE
*
* Return value:
*  NONE
*
* ******************************************************************************
*/
void tm_retrieve_IMSI( uint8 sub, tm_RetrieveIMSICb p_RetrieveImsiInfoCb )
{
  os_IpcMsgType  *p_ipcMsg = NULL;
  tm_RetrieveIMSIType z_retrieveImsiInfo;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"tm_retrieve_IMSI()", 0,0,0);

  z_retrieveImsiInfo.sub = sub;
  z_retrieveImsiInfo.pz_RetrieveIMSICb = p_RetrieveImsiInfoCb;

  p_ipcMsg = os_IpcCreate(sizeof(z_retrieveImsiInfo), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);
  if(p_ipcMsg)
  {
     p_ipcMsg->q_MsgId = TM_CORE_MSG_ID_RETRIEVE_IMSI;
     memscpy((void*)(p_ipcMsg->p_Data), sizeof(z_retrieveImsiInfo), (void*)(&z_retrieveImsiInfo), sizeof(z_retrieveImsiInfo));

     if(!os_IpcSend(p_ipcMsg, (uint32)THREAD_ID_SM_TM))
     {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_API: Failed to send IPC message",0,0,0);
        (void)os_IpcDelete(p_ipcMsg);
     }
  }
}



/*===========================================================================

FUNCTION tm_api_get_gps_lock_status()

DESCRIPTION
  TM module can call this function to get current GPS lock status

  This is a synchronous API, will return immediately

DEPENDENCIES

RETURN VALUE
  Current GPS lock status

SIDE EFFECTS

===========================================================================*/
pdsm_lite_gps_lock_e_type tm_api_get_gps_lock_status(void)
{
  return TM_CORE_CONFIG_DD(gps_lock);
}
#if 0
/*===========================================================================
  tm_handle_lppm_status

  Description:

     This function handles LPPM status report

 Parameters:

   pz_LppmStatus:  pointer to the LPPM status struct

  Return value:
    void

=============================================================================*/
void tm_handle_lppm_status(const sm_LppmStatusStructType *pz_LppmStatus)
{
  os_IpcMsgType  *p_ipcMsg = NULL;
  sm_LppmStatusStructType *p_hdr_ptr = NULL;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"tm_handle_lppm_status()", 0,0,0);

  if(NULL == pz_LppmStatus)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Null pointer in handling Wifi fix response",0,0,0);
    return;
  }

  p_ipcMsg = os_IpcCreate(sizeof(*p_hdr_ptr), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if(p_ipcMsg)
  {
    p_hdr_ptr = (sm_LppmStatusStructType*) p_ipcMsg->p_Data; /*lint !e826*/

    p_ipcMsg->q_MsgId          = (uint32)TM_CORE_MSG_ID_LPPM_STATUS;
    p_ipcMsg->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    p_ipcMsg->q_Size           = sizeof(sm_LppmStatusStructType);

    *(p_hdr_ptr) = (*pz_LppmStatus);

    if(!os_IpcSend(p_ipcMsg, (uint32)THREAD_ID_SM_TM))
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_API: Failed to send IPC message",0,0,0);
      (void)os_IpcDelete(p_ipcMsg);
    }
  }
}
#endif
/*===========================================================================
  tm_handle_gnss_qual_ind

  Description:

     This function is used to handle GNSS Qual Indication

 Parameters:

   e_GnssQualityInd:  GNSS Quality indication enum

  Return value:
    void

=============================================================================*/
void tm_handle_gnss_qual_ind(const gnss_QualIndEnumType e_GnssQualityInd)
{
  os_IpcMsgType  *p_ipcMsg = NULL;
  gnss_QualIndEnumType *p_hdr_ptr = NULL;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"tm_handle_gnss_qual_ind()", 0,0,0);

  p_ipcMsg = os_IpcCreate(sizeof(*p_hdr_ptr), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if(p_ipcMsg)
  {
    p_hdr_ptr = (gnss_QualIndEnumType*) p_ipcMsg->p_Data; /*lint !e826*/

    p_ipcMsg->q_MsgId          = (uint32)TM_CORE_MSG_ID_GNSS_QUAL_IND;
    p_ipcMsg->q_DestThreadId   = (uint32)THREAD_ID_SM_TM;
    p_ipcMsg->q_Size           = sizeof(gnss_QualIndEnumType);

    *(p_hdr_ptr) = e_GnssQualityInd;

    if(!os_IpcSend(p_ipcMsg, (uint32)THREAD_ID_SM_TM))
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_API: Failed to send IPC message",0,0,0);
      (void)os_IpcDelete(p_ipcMsg);
    }
  }
}
/*=============================================================================

FUNCTION
  tm_api_leap_second_report

DESCRIPTION
  Leap Second information from ME to SM

DEPENDENCIES
  None.

PARAMETERS
  mgp_LeapSecondInfoStructType - Pointer to leap second info

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/
void tm_api_leap_second_report(const mgp_LeapSecondInfoStructType *pz_LeapSecInfo)
{
  os_IpcMsgType* p_IpcMsg = NULL;

  MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM_API: Leap Second Info report from ME");

  if (NULL == pz_LeapSecInfo)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_api_leap_second_report: Null Leap Second Info");
    return;
  }

  p_IpcMsg = os_IpcCreate(sizeof(mgp_LeapSecondInfoStructType), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (NULL != p_IpcMsg)
  {
    p_IpcMsg->q_MsgId        = (uint32)TM_CORE_MSG_ID_LEAP_SEC_INFO;

    memscpy(p_IpcMsg->p_Data, sizeof(*pz_LeapSecInfo), (void*)pz_LeapSecInfo, sizeof(*pz_LeapSecInfo));

    if (!os_IpcSend(p_IpcMsg, (uint32)THREAD_ID_SM_TM))
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Failed to send IPC mesage");
      (void)os_IpcDelete(p_IpcMsg);
      return;
    }
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Failed to create IPC message");
  }
}

/*============================================================================
FUNCTION
    send_ipc_msg_to_sm_tm

DESCRIPTION
    Send an IPC Msg to TM task

PARAMETERS
   p_Data - pointer to the input data which will be put into the
                       message payload
   q_Size - size of data in bytes
   e_Msg  - Messge Id

DEPENDENCIES
    None

RETURN VALUE
    TRUE - If the data is successfully sent to TM
    FALSE - Otherwise

SIDE EFFECTS
    None

===========================================================================*/
static boolean send_ipc_msg_to_sm_tm( const void *p_Data, uint32 q_Size,
                                                      uint32 e_Msg)
{
  os_IpcMsgType *p_Msg;

  if ( ( q_Size > 0 ) && ( NULL == p_Data ) )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Cannot generate TM msg with data ptr uninitialized", 0, 0, 0);
    return FALSE;
  }

  p_Msg = os_IpcCreate( q_Size, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM );

  if (NULL == p_Msg)
  {
    return FALSE;
  }

  p_Msg->q_MsgId = (uint32)e_Msg;
  p_Msg->q_DestThreadId = (uint32)THREAD_ID_SM_TM;
  p_Msg->q_Size = q_Size;

  if ( q_Size > 0 )
  {
    memscpy(p_Msg->p_Data, q_Size, p_Data, q_Size);
  }

  if (!os_IpcSend( p_Msg, (uint32)THREAD_ID_SM_TM ))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_API: Failed to send IPC message",0,0,0);
    (void)os_IpcDelete(p_Msg);
    return FALSE;
  }

  return TRUE;
}
/*=============================================================================

FUNCTION
  tm_api_xtra_gts_time_request

DESCRIPTION
  This function is used to process GTS time request from XTRA module 

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_api_xtra_gts_time_request(void)
{
   /* send TM_CORE_MSG_ID_XTRA_GTS_TIME_REQUEST ipc message to tm */
   os_IpcMsgType  *p_ipcMsg = NULL;
   uint32 *p_reqData = NULL;
   boolean u_ret = FALSE;
   do
   {
     p_ipcMsg = os_IpcCreate(sizeof(*p_reqData), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);
     if(p_ipcMsg)
     {
       p_reqData = (uint32*)p_ipcMsg->p_Data;
       p_ipcMsg->q_MsgId = TM_CORE_MSG_ID_XTRA_GTS_TIME_REQUEST;
       *p_reqData = 0;

       if(!os_IpcSend(p_ipcMsg, (uint32)THREAD_ID_SM_TM))
       {
         MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_API: Failed to send IPC message",0,0,0);
         (void)os_IpcDelete(p_ipcMsg);
          u_ret = FALSE;
          break;
       }
         u_ret = TRUE;
     }
     else
     {
       MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_API: Failed to create IPC message",0,0,0);
       u_ret = FALSE;
       break;
     }
   }while(0);

   return u_ret;
}


/*
 ******************************************************************************
 * tm_api_request_xtra_inject_to_mgp
 *
 * Function description:
 *
 * This function is used to request XTRA injection from TM to MGP
 *
 * Parameters:
 *   None
 *
 * Return value:  
 * TRUE if request was succesful
 * FALSE otherwise
 ******************************************************************************
*/
boolean tm_api_request_xtra_inject_to_mgp(void)
{
   /* send TM_CORE_MSG_ID_XTRA_INJECT_TO_MGP_REQUEST ipc message to tm */
#if 0
   os_IpcMsgType  *p_ipcMsg = NULL;
   uint32 *p_reqData = NULL;
   boolean u_ret = FALSE;
   do
   {
     p_ipcMsg = os_IpcCreate(sizeof(*p_reqData), IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);
     if(p_ipcMsg)
     {
       p_reqData = (uint32*)p_ipcMsg->p_Data;
       p_ipcMsg->q_MsgId = TM_CORE_MSG_ID_XTRA_INJECT_TO_MGP_REQUEST;
       *p_reqData = 0;

       if(!os_IpcSend(p_ipcMsg, (uint32)THREAD_ID_SM_TM))
       {
         MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_API: Failed to send IPC message",0,0,0);
         (void)os_IpcDelete(p_ipcMsg);
          u_ret = FALSE;
          break;
       }
       u_ret = TRUE;
     }
     else
     {
       MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_API: Failed to create IPC message",0,0,0);
       u_ret = FALSE;
       break;
     }
   }while(0);
#endif 
   tm_xtra_handle_inject_to_mgp_request();
   return TRUE;
}

/*=============================================================================

FUNCTION
  tm_api_RequestPlmnCellScan

DESCRIPTION
.

DEPENDENCIES
  None.

PARAMETERS
  plmnSearch

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
boolean tm_api_RequestPlmnCellScan(tle_tm_plmn_search* plmnSearch)
{
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;

  MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_api_RequestPlmnCellScan()");

  msg_id = (uint32)TM_CORE_MSG_ID_TLE_PLMN_CELL_SCAN_REQ;
  size   = sizeof(tle_tm_plmn_search);

  /* Send the MSG to SM TM thread */
  ipc_msg_ptr = sm_send_message(msg_id, size,
    (uint32)THREAD_ID_SM_TM, (void *)plmnSearch);

  if(ipc_msg_ptr != NULL)
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"SM_API: IPC msg send to SM_TM, id = %d",ipc_msg_ptr->q_MsgId);
    return FALSE;
  }
  return TRUE;
}
/*===========================================================================

FUNCTION tm_api_get_gtp_wwan_fix_timer_val

DESCRIPTION
  This function returns the timer count base on cell RAT we are camped

  RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
void tm_api_get_gtp_wwan_fix_timer_val(uint32 *q_TimerCount)
{
  if (NULL == q_TimerCount)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "NULL Params");
    return;
  }

  tm_core_get_gtp_wwan_fix_timer_val(q_TimerCount);

}

/*===========================================================================

FUNCTION tm_api_is_gtp_2_0_fix_possible

DESCRIPTION
  This function returns TRUE if GTP 2.0 fix is possible else returns FALSE

  RETURN VALUE
  True if GTP 2.0 fix is possible else False

DEPENDENCIES
  None

SIDE EFFECTS

===========================================================================*/
bool tm_api_is_gtp_2_0_fix_possible(void)
{
  return tm_core_is_gtp_2_0_fix_possible();

}



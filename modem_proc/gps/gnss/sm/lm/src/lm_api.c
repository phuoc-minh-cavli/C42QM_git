/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc.
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */
/*======================================================================

                         Location Manager API definitions file

 GENERAL DESCRIPTION
  This file contains API definitions by which Location Manager(LM) services can 
  be accessed by other tasks (e.g Transport Manager). 

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header $$DateTime $$Author $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 10/17/17  rn    Added Xtra Integrity support.
 07/15/15  al    Added support for prescribed dwell command versions 51 and 53 
 06/18/08  jlp   Added messaging support so tm_diag can set the Sv No Exist
                  list in cd.
 6/20/06   mr   Initial creation of header file.
======================================================================*/

#include "gps_variation.h"
#include "lm_api.h"
#include "lm_iface.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "lm_data.h"
#include "gnss_common.h"
#include "tm_api.h"
#define TM_SARF_CMD_ARRAY_SIZE ARIES_MSG_ID_TM_LM_END-ARIES_MSG_ID_TM_LM_FIRST-1

const uint16 tm_sarf_cmds_payload_sizes[TM_SARF_CMD_ARRAY_SIZE] =
                                       { sizeof(lm_sarf_mode_type),
                                       MAX(sizeof(lm_GnssPrescDwellType_V51), sizeof(lm_GnssPrescDwellType_V53)),
                                       sizeof(lm_GnssIqTestType),
                                       sizeof(lm_1sv_standalone_type),
                                       sizeof(lm_mem_bw_test_type),
                                       sizeof(lm_TsgCmdType),
                                       sizeof(lm_RfCmdType),
                                       sizeof(lm_multichan_track_type),
                                       sizeof(lm_PrxRfCmdType),
                                       sizeof(lm_RfLinearityCmdType),
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       sizeof(lm_SpectrumAnalyzerCmdType),
                                       0, /*sizeof(lm_PfaTestType),Command not supported*/ 
                                       sizeof(uint8),
                                       sizeof(lm_NavIoctlType),
                                       sizeof(lm_NavConfigReqType),
                                     #ifdef FEATURE_GNSS_LTE_CATM_COEX
                                       sizeof(lm_CatmLteTxStartTestParams),
                                       sizeof(lm_CatmLteTxEndTestParams),
                                     #endif /* FEATURE_GNSS_LTE_CATM_COEX */
                                       sizeof(lm_NavBpJammerCtlType) };

/****************** Function Definitions**************************************/

/*****************************LM-TM interface functions***********************/

/*
 ******************************************************************************
 * lm_session_request 
 *
 * Function description:
 *
 * This function is called by TM for:
 * 1.  starting/stopping/updating a session with LM
 * 2.  To request a PPM report, PRM report or position report
 *     for an already established session.
 *
 * Parameters: 
 * p_req - Pointer to structure containing request parameters
 *
 * Return value: 
 *
 * boolean - IPC send successful or not 
 *
 ******************************************************************************
*/
boolean lm_session_request(const lm_session_request_info_s_type *p_req)
{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  boolean result = TRUE;  
  lm_session_request_info_s_type *p_hdr_ptr;

  if(p_req == NULL)
  {
    result = FALSE;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(sizeof(lm_session_request_info_s_type),
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_LM); 


    if(p_ipc_ptr == NULL)
    {
      result = FALSE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
      p_hdr_ptr = (lm_session_request_info_s_type*) p_ipc_ptr->p_Data;/*lint !e826 */

      p_ipc_ptr->q_MsgId          = (uint32)C_SM_LM_MSG_SESSION_REQUEST;
      p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
      p_ipc_ptr->q_Size           = sizeof(lm_session_request_info_s_type);

      *p_hdr_ptr = *p_req;

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
      {
         result = FALSE;
         (void)os_IpcDelete(p_ipc_ptr);
      }

     }

  }/*if(p_req ==  NULL) else {}*/


  return result;

}


/*
 ******************************************************************************
 * lm_request 
 *
 * Function description:
 *
 * This function is called by TM to send a LM-TM session independent request to LM.
 * The type of request could be:
 * 1.  Turn keep warm processing ON/OFF
 * 2.  Turn NMEA sentence generation ON/OFF
 * 3.  Get MGP's Aiding data wishlist
 * 4.  Delete certain parameters in MGP database
 *
 * Parameters: 
 * p_req - Pointer to structure containing request parameters
 *
 * Return value: 
 *
 * boolean - IPC send successful or not 
 *
 ******************************************************************************
*/
boolean lm_request(const lm_request_info_s_type *p_req)
{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  boolean result = TRUE;  
  lm_request_info_s_type *p_hdr_ptr;

  if(p_req == NULL)
  {
    result = FALSE;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(sizeof(lm_request_info_s_type),
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_LM); 


    if(p_ipc_ptr == NULL)
    {
      result = FALSE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
      p_hdr_ptr = (lm_request_info_s_type*) p_ipc_ptr->p_Data;/*lint !e826 */

      p_ipc_ptr->q_MsgId          = (uint32)C_SM_LM_MSG_REQUEST;
      p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
      p_ipc_ptr->q_Size           = sizeof(lm_request_info_s_type);

      *p_hdr_ptr = *p_req;

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
      {
         result = FALSE;
         (void)os_IpcDelete(p_ipc_ptr);
      }

     }

  }/*if(p_req ==  NULL) else {}*/


  return result;
}

/*
 ******************************************************************************
 * lm_translate_lm_info_to_ipc_type 
 *
 * Function description:
 *
 * This function translates from lm_info_s_type to lm_info_s_type_ipc_type
 *
 * Parameters: 
 * p_ipc_type - Pointer to structure lm_info_s_type_ipc_type
 * p_info - Pointer to structurelm_info_s_type
 *
 * Return value: 
 *
 * boolean - If conversion is succesfull or not
 *
 ******************************************************************************
*/

boolean lm_translate_lm_info_to_ipc_type(lm_info_s_type_ipc_type *p_ipc_type,
                                         const lm_info_s_type *p_info)
{
  boolean ret_value = TRUE;
  
  p_ipc_type->e_info_type = p_info->e_info_type;
  p_ipc_type->z_info.f_new_pos_unc = p_info->z_info.f_new_pos_unc; 
  p_ipc_type->z_info.f_new_time_unc = p_info->z_info.f_new_time_unc;  
  p_ipc_type->z_info.f_time_offset = p_info->z_info.f_time_offset;  
  p_ipc_type->z_info.z_sess_info = p_info->z_info.z_sess_info;     

  return ret_value;
}


/*
 ******************************************************************************
 * lm_translate_lm_info_ipc_type_to_lm_info 
 *
 * Function description:
 *
 * This function translates from lm_info_s_type_ipc_type to lm_info_s_type
 *
 * Parameters: 
 * p_ipc_type - Pointer to structure lm_info_s_type_ipc_type
 * p_info - Pointer to structurelm_info_s_type
 *
 * Return value: 
 *
 * boolean - If conversion is succesfull or not
 *
 ******************************************************************************
*/
boolean lm_translate_lm_info_ipc_type_to_lm_info(lm_info_s_type_ipc_type *p_ipc_type,
                                                 lm_info_s_type *p_info)
{
  boolean ret_value = TRUE;
  
  p_info->e_info_type = p_ipc_type->e_info_type;
  p_info->z_info.f_new_pos_unc = p_ipc_type->z_info.f_new_pos_unc; 
  p_info->z_info.f_new_time_unc = p_ipc_type->z_info.f_new_time_unc;  
  p_info->z_info.f_time_offset = p_ipc_type->z_info.f_time_offset;  
  p_info->z_info.z_sess_info = p_ipc_type->z_info.z_sess_info;    

  return ret_value;
}


/*
 ******************************************************************************
 * lm_calculate_lm_info_ipc_size 
 *
 * Function description:
 *
 * This function calculates the IPC size required for lm_info_s_type
 *
 * Parameters: 
 * p_info - Pointer to structure lm_info_s_type
 *
 * Return value: 
 *
 * boolean - If calculation is succesfull or not
 *
 ******************************************************************************
*/
uint32 lm_calculate_lm_info_ipc_size(const lm_info_s_type *p_info)
{
  uint32 ipc_size = 0;

  
  ipc_size += sizeof(lm_info_s_type_ipc_type);
    

  return ipc_size;
}


/*
 ******************************************************************************
 * lm_info 
 *
 * Function description:
 *
 * This function is called by TM to send LM-TM session independent info to LM.
 * The information type can be:
 * 1.  Aiding data to be injected in MGP.
 * 2.  Expected network delays for getting aiding data.
 *
 * Parameters: 
 * p_info - Pointer to structure containing information parameters
 *
 * Return value: 
 *
 * boolean - IPC send successful or not 
 *
 ******************************************************************************
*/
boolean lm_info(const lm_info_s_type      *p_info)
{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  boolean result = TRUE;  
  lm_info_s_type_ipc_type *p_hdr_ptr;

  if(p_info == NULL)
  {
    result = FALSE;
  }
  else
  {
    uint32 ipc_size = lm_calculate_lm_info_ipc_size(p_info);

    if(ipc_size == 0)        
    {
      LM_MSG_ERROR("Lm info IPC Size calculated as 0",0,0,0);
      result = FALSE;
    }
    else
    {
      p_ipc_ptr = os_IpcCreate(ipc_size,
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_LM); 


    if(p_ipc_ptr == NULL)
    {
        LM_MSG_ERROR("Unable to allocate Ipc memory for %d bytes",ipc_size,0,0);
      result = FALSE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
        p_hdr_ptr = (lm_info_s_type_ipc_type*) p_ipc_ptr->p_Data;/*lint !e826 */

      p_ipc_ptr->q_MsgId          = (uint32)C_SM_LM_MSG_INFO;
      p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
        p_ipc_ptr->q_Size           = ipc_size;

        if(lm_translate_lm_info_to_ipc_type(p_hdr_ptr,p_info))
        {
      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
      {
             LM_MSG_ERROR("Failure in sending IPC",0,0,0);
         result = FALSE;
         (void)os_IpcDelete(p_ipc_ptr);
      }
        }
        else
        {                  
          LM_MSG_ERROR("Unable to translate lm_info to lm_info ipc type",0,0,0);
          result = FALSE;
          (void)os_IpcDelete(p_ipc_ptr);
        }

     }

     }

  }/*if(p_req ==  NULL) else {}*/


  return result;
}


/*****************************LM-GPSDIAG interface functions***********************/

/*===========================================================================

FUNCTION lmDiag_ForwardGSCPkt

DESCRIPTION
  This procedure is called by ARIES GPS Diag to send GPS Session Control 
  DIAG packets to LM.
  
DEPENDENCIES
  None.

RETURN VALUE
boolean - IPC send successful or not 

SIDE EFFECTS
  None.

===========================================================================*/
boolean lmDiag_ForwardGSCPkt(
  PACKED const void *p_req_pkt_ptr,
  word q_pkt_len
)
{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  boolean result = TRUE;  
  void *p_hdr_ptr;

  if(p_req_pkt_ptr == NULL)
  {
    result = FALSE;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(q_pkt_len,
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_LM); 


    if(p_ipc_ptr == NULL)
    {
      result = FALSE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
      p_hdr_ptr = (void*) p_ipc_ptr->p_Data;

      p_ipc_ptr->q_MsgId          = (uint32)C_SM_LM_DIAG_GSC_MSG;
      p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
      p_ipc_ptr->q_Size           = q_pkt_len;

      memscpy((void*)p_hdr_ptr, p_ipc_ptr->q_Size, (void*)p_req_pkt_ptr, q_pkt_len);

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
      {
         result = FALSE;
         (void)os_IpcDelete(p_ipc_ptr);
      }

     }

  }/*if(p_req ==  NULL) else {}*/


  return result;
}

/*===========================================================================
FUNCTION lm_send_sarf_cmd

DESCRIPTION
  Call this function to send SARF cmds to LM:-
  1. Mode switch
  2. Single SV
  3. Presc dwell
  4. IQ test
  5. Mem BW test

PARAMETERS
  p_payload - Pointer to payload
  lm_ipc_msg_id - Enum identifying the SARF cmd. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if msg-send failed, FALSE if otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean lm_send_sarf_cmd (PACKED uint8* p_payload, tm_ipc_msg_id_type lm_ipc_msg_id)
{
  os_IpcMsgType* p_Msg;
  boolean u_return_status = TRUE;/* Initialize to failure state */
  uint16 w_payload_size;
#if 0 /* Remove parameter sanity checking done here */
  lm_multichan_track_v55_type* p_rfdev_multichan_cmd;
#endif
  int32 q_cmd_index =  ((int32)lm_ipc_msg_id - (int32)ARIES_MSG_ID_TM_LM_FIRST) - 1;
    
  /*lint -e{656} : Arithmetic operation uses (compatible) enum's */
  /*lint -e{641} : Converting enum 'tm_diag_lm_msg_id_type' to int */
  if(q_cmd_index >= 0 && q_cmd_index < TM_SARF_CMD_ARRAY_SIZE )
  {
    w_payload_size = tm_sarf_cmds_payload_sizes[q_cmd_index];
  }
  else
  {
    return TRUE;
  }


#if 0 /* Remove parameter sanity checking done here */
  /* Before sending IPC message, do sanity check on the cmd if the 
     cmd is for Callisto RF Dev multi-chan track cmd */
  if(q_cmd_index == (int32)ARIES_MSG_ID_TM_LM_RFDEV_TEST)
  {
    p_rfdev_multichan_cmd = (lm_multichan_track_v55_type*) p_payload;
    if(((p_rfdev_multichan_cmd->l_GLO_Freq_ID == -8) && (p_rfdev_multichan_cmd->u_GLO_HW_Chan_Num != 0))||
       ((p_rfdev_multichan_cmd->u_Test_Mode != (uint8)MultiChan_Track_SBAS) && (p_rfdev_multichan_cmd->u_GPS_SBAS_SV_ID > 32))||
       ((p_rfdev_multichan_cmd->u_Test_Mode == (uint8)MultiChan_Track_SBAS) && (p_rfdev_multichan_cmd->u_GPS_SBAS_SV_ID <= 32))||
       ((p_rfdev_multichan_cmd->u_Test_Mode == (uint8)MultiChan_Track_PRI_GLO_BDS) && (p_rfdev_multichan_cmd->u_BDS_SV_ID > 37))||
       ((p_rfdev_multichan_cmd->u_Test_Mode == (uint8)MultiChan_Track_PRI_BDS) && (p_rfdev_multichan_cmd->u_BDS_SV_ID > 37))||
       ((p_rfdev_multichan_cmd->u_Test_Mode == (uint8)MultiChan_Track_BDS) && (p_rfdev_multichan_cmd->u_BDS_SV_ID > 37)) ||
       ( p_rfdev_multichan_cmd->u_GAL_SV_ID > 36)
       )
    {
      /* Invalid Cmd parameter entered by user. Skip sending IPC message and return DM Error Response*/

      return TRUE;
    }
  }
#endif

  /* Dispatch LSM Subsystem requests */
  p_Msg = os_IpcCreate((uint32) w_payload_size, 
                       IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_LM);

  if (p_Msg)
  {
    /* Fill and send the IQ Collect Msg to LM */
    p_Msg->q_MsgId = (uint32)lm_ipc_msg_id;

    memscpy((void*)p_Msg->p_Data, w_payload_size, (void*) p_payload, w_payload_size);

    if (!os_IpcSend(p_Msg, (uint32)THREAD_ID_SM_LM))
    {
     (void)os_IpcDelete(p_Msg);
      ERR_FATAL("Not able to send SARF cmd to LM", 0, 0, 0);
    }
    else
    {
      u_return_status = FALSE;
    }
  }
  else
  {
    ERR_FATAL("Not able to create ipc msg", 0, 0, 0);
  }

  return u_return_status;
} /* gpsdiag_IqCollectCmd */

/*===========================================================================
FUNCTION lm_send_sv_no_exist_cmd

DESCRIPTION
  Sends the Sv No Exist List on to lm.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if msg-send was successful, FALSE if otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean lm_send_sv_no_exist_cmd(const uint32 cq_NoExistMask,
 const sv_no_exist_action_type ce_Action)
{
  os_IpcMsgType * p_Msg;
  lm_sv_no_exist_cmd_type * pz_NoExistCmd;
  boolean u_ReturnStatus = FALSE;

  /* Dispatch LSM Subsystem requests */
  p_Msg = os_IpcCreate(sizeof(lm_sv_no_exist_cmd_type), IPC_ALLOCATION_DYNAMIC,
   (uint32)THREAD_ID_SM_LM);

  if (p_Msg)
  {
    /* Fill and send the IQ Collect Msg to LM */
    p_Msg->q_MsgId = (uint32)C_SM_LM_SV_NO_EXIST;

    pz_NoExistCmd = (lm_sv_no_exist_cmd_type *)p_Msg->p_Data;
    pz_NoExistCmd->q_SvNoExistMask = cq_NoExistMask;
    pz_NoExistCmd->e_Action = ce_Action;

    if (!os_IpcSend(p_Msg, (uint32)THREAD_ID_SM_LM))
    {
      (void)os_IpcDelete(p_Msg);
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Not able to send Sv No Exist cmd to LM", 0, 0, 0);
    }
    else
    {
      u_ReturnStatus = TRUE;
    }
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Not able to create ipc msg", 0, 0, 0);
  }

  return u_ReturnStatus;
} /* lm_send_sv_no_exist_cmd */

/*===========================================================================
FUNCTION lm_send_force_dpo_for_power_meas_cmd

DESCRIPTION
  Forces DPO Operation for Power Measurements
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if msg-send was successful, FALSE if otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

boolean lm_send_force_dpo_for_power_meas_cmd(const uint8 u_ForceDpoForPowerMeas )
{
  os_IpcMsgType * p_Msg;
  lm_force_dpo_for_power_meas_cmd_type * pz_ForceDpoForPowerMeasCmd;
  boolean u_ReturnStatus = FALSE;

  /* Dispatch LSM Subsystem requests */
  p_Msg = os_IpcCreate(sizeof(lm_force_dpo_for_power_meas_cmd_type), IPC_ALLOCATION_DYNAMIC,
   (uint32)THREAD_ID_SM_LM);

  if (p_Msg)
  {
    /* Fill and send the IQ Collect Msg to LM */
    p_Msg->q_MsgId = (uint32)C_SM_LM_FORCE_DPO_FOR_POWER_MEAS;

    pz_ForceDpoForPowerMeasCmd = (lm_force_dpo_for_power_meas_cmd_type *)p_Msg->p_Data;
    pz_ForceDpoForPowerMeasCmd->u_ForceDpoForPowerMeas = u_ForceDpoForPowerMeas;

    if (!os_IpcSend(p_Msg, (uint32)THREAD_ID_SM_LM))
    {
      (void)os_IpcDelete(p_Msg);
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Not able to send Force DPO Power cmd to LM", 0, 0, 0);
    }
    else
    {
      u_ReturnStatus = TRUE;
    }
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Not able to create ipc msg", 0, 0, 0);
  }

  return u_ReturnStatus;
}

/* Helper functions */
/*
 ******************************************************************************
 * lm_update_session_state 
 *
 * Function description:
 *
 *
 * Parameters: 
 * e_newSessionState - new session state to be set
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_update_session_state(lm_session_state_e_type  e_newSessionState)
{
  z_lmControl.e_sessionState = e_newSessionState;

  LM_MSG_MED("New session state: %d.", z_lmControl.e_sessionState, 0, 0);
}

/*
 ******************************************************************************
 * lm_set_rc_on_threshold_period 
 *
 * Function description:
 *
 *
 * Parameters: 
 * the value of parameter AAGPS_APP_TRACK_RC_ON_THRESHOLD_PERIOD to be set
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_set_rc_on_threshold_period(uint32 q_gpsOnThreshold)
{

  if(q_gpsOnThreshold > LM_MAX_RC_ON_THRESHOLD)
  {
    z_lmControl.q_gpsOnThreshold = LM_MAX_RC_ON_THRESHOLD;
    LM_MSG_MED("New RC_ON_THRESHOLD value greater than maximum[%d].",
               LM_MAX_RC_ON_THRESHOLD,0,0);
  }
  else
  {
    z_lmControl.q_gpsOnThreshold = q_gpsOnThreshold;
  }

  LM_MSG_MED("Setting RC_ON_THRESHOLD to %d",z_lmControl.q_gpsOnThreshold,0,0);
}

/*
 ******************************************************************************
 * lm_set_rc_idle_threshold 
 *
 * Function description:
 *
 *
 * Parameters: 
 * the value of parameter AAGPS_APP_TRACKING_RC_IDLE_THRESHOLD to be set
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_set_rc_idle_threshold(uint32 q_gpsIdleThreshold)
{

  if(q_gpsIdleThreshold > LM_MAX_RC_IDLE_THRESHOLD)
  {
    z_lmControl.q_gpsIdleThreshold = LM_MAX_RC_IDLE_THRESHOLD;
    LM_MSG_MED("New RC_IDLE_THRESHOLD value greater than maximum[%d].",
               LM_MAX_RC_IDLE_THRESHOLD,0,0);
  }
  else
  {
    z_lmControl.q_gpsIdleThreshold = q_gpsIdleThreshold;
  }
  LM_MSG_MED("Setting RC_IDLE_THRESHOLD to %d",z_lmControl.q_gpsIdleThreshold,0,0);
}

/*
 ******************************************************************************
 * lm_set_rc_shut_off_delay_margin 
 *
 * Function description:
 *
 *
 * Parameters: 
 * the value of parameter LM_SET_PARAM_APP_TRK_DELAY_MARGIN to be set
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_set_rc_shut_off_delay_margin(uint32 q_delayMargin)
{

  if(q_delayMargin > LM_MAX_RC_SHUT_OFF_DELAY_MARGIN)
  {
    z_lmControl.q_delayMargin = LM_RC_SHUT_OFF_DELAY_MARGIN_DEFAULT;
    LM_MSG_MED("New RC_SHUT_OFF_DELAY_MARGIN value greater than maximum[%d].",
               LM_MAX_RC_SHUT_OFF_DELAY_MARGIN,0,0);
  }
  else
  {
    z_lmControl.q_delayMargin = q_delayMargin;
  }
  LM_MSG_MED("Setting RC_SHUT_OFF_DELAY_MARGIN to %d",z_lmControl.q_delayMargin,0,0);
}

/*
 ******************************************************************************
 * lm_get_rc_on_threshold_period 
 *
 * Function description:
 *
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * uint32 - returns the value of parameter AAGPS_APP_TRACK_RC_ON_THRESHOLD_PERIOD
 *
 ******************************************************************************
*/

uint32 lm_get_rc_on_threshold_period(void)
{
  
  return z_lmControl.q_gpsOnThreshold;
}

/*
 ******************************************************************************
 * lm_get_rc_idle_threshold 
 *
 * Function description:
 *
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * uint32 - returns the value of parameter AAGPS_APP_TRACKING_RC_IDLE_THRESHOLD
 *
 ******************************************************************************
*/
uint32 lm_get_rc_idle_threshold(void)
{
  
  return z_lmControl.q_gpsIdleThreshold;
}

/*
 ******************************************************************************
 * lm_get_rc_shut_off_delay_margin 
 *
 * Function description:
 *
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * uint32 - returns the value of parameter AAGPS_APP_TRACKING_RC_SHUT_OFF_GRACE_PERIOD
 *
 ******************************************************************************
*/
uint32 lm_get_rc_shut_off_delay_margin(void)
{
  
  return z_lmControl.q_delayMargin;
}

/*
 ******************************************************************************
 * lm_get_utc_leap_secs
 *
 * Function description:
 * Returns the UTC leap seconds
 *
 * Parameters: 
 * None
 *
 * Return value: 
 * utc leap seconds
 *
 ******************************************************************************
*/

uint8 lm_get_utc_leap_secs(void)
{
  return z_lmControl.z_utcData.z_utcInfo.u_DeltaTls;
}
/*
 ******************************************************************************
 * lm_send_xtra_done_msg
 *
 * Function description:
 *   Send XTRA Injection Done IPC to LM
 *
 * Parameters: 
 * q_DecodedMask: Successfully decoded mask to be sent to PE.
 *
 * Return value: 
 * None
 *
 ******************************************************************************
*/
void lm_send_xtra_done_msg( uint32 q_DecodedMask)
{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Xtra Injection Completed. Sending IND to LM.",0,0,0);
  p_ipc_ptr = os_IpcCreate(sizeof(q_DecodedMask),
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_LM); 

  if(p_ipc_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Unalbe to create IPC to send to LM",0,0,0);
  }
  else
  {

    p_ipc_ptr->q_MsgId          = (uint32)SM_MSG_ID_LM_MGP_REPORT_XTRA_INJ_DONE;
    p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
    p_ipc_ptr->q_Size           = sizeof(q_DecodedMask);

    *(uint32*) p_ipc_ptr->p_Data = q_DecodedMask ;

    if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
    {
       (void)os_IpcDelete(p_ipc_ptr);
    }
  }

}

/*
 ******************************************************************************
 * lm_send_xtraintegrity_msg
 *
 * Function description:
 *   Send XTRA Integrity IPC to LM
 *
 * Parameters: 
 * gnss_XtraIntTmStructType *p_xtraintStruct     - ptr to Xtra Integrity 
 *                                               structure holding Integrity
 *                                               records.
 * Return value: 
 * None
 *
 ******************************************************************************
*/
void lm_send_xtraintegrity_msg( const gnss_XtraIntTmStructType *p_xtraintStruct )
{
  uint16         w_Size = 0;
  os_IpcMsgType  *p_ipc_ptr = NULL;  

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Xtra Integrity processing completed. Send data to LM.",0,0,0);

  w_Size = sizeof(gnss_XtraIntTmStructType);

  p_ipc_ptr = os_IpcCreate(w_Size,IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_LM); 

  if(p_ipc_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Unable to create Xtra Integrity IPC to send to LM",0,0,0);
  }
  else
  {
    p_ipc_ptr->q_MsgId          = (uint32)SM_MSG_ID_LM_MGP_REPORT_XTRA_INTEGRITY;
    p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
    p_ipc_ptr->q_Size           = w_Size;

    //Copy gnss_XtraIntTmStructType data to IPC structure.
    memscpy((void*)&p_ipc_ptr->p_Data, w_Size,(void*)p_xtraintStruct, w_Size);

    if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
    {
       (void)os_IpcDelete(p_ipc_ptr);
    }
  }

}   /* end lm_send_xtraintegrity_msg() */


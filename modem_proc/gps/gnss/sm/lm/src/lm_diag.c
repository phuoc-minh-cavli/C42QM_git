/*======================================================================

                        Location Manager GPS DIAG interaction file

GENERAL DESCRIPTION
 This file contains functions by which Location Manager(LM) handles
 messages from GPS DIAG task.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


 Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
 Qualcomm Confidential and Proprietary. All Rights Reserved.
 Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
 Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
 Copyright (c) 2015 - 2020 Qualcomm Technologies, Inc. 
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/lm/src/lm_diag.c#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

 when      who   what, where, why
 --------  ---  ------------------------------------------------------
 05/08/20   ska     Smart compilation issues
 01/30/18  mj  Add support to block sessions during SARF
 07/15/15  al   Added support for prescribed dwell command versions 51 and 53 
 08/25/14  jv   16-bit SVID Support
 01/03/11       Removed msmhwioreg.h include (covered by msm.h)
 06/18/08  jlp  Added messaging support so tm_diag can set the Sv No Exist
                 list in cd.
 05/07/07  jw   Added Diag GPS Intelliciever support.
 04/26/07  jw   Fixed a case in diabling bit edge required flag.
 04/23/07  jw   Add another case for Single SV track to handle no bit edge
                required before we enter track.
 07/27/06  mr   Initial creation of file.
======================================================================*/

#include "gps_variation.h"
#include "lm_diag.h"
#include "lm_data.h"
//#include "sm_gpsdiag_api.h"
#include "event.h"
#include "lm_mgp.h"
#include "mgp_api.h"
#include "cgps_api.h"
#include "msm.h"
#include "tm_api.h"

//#undef FEATURE_GPS_GEN7_ME_API

typedef enum
{
  GNSS_RF_PRIM_NB_RXD_ALWAYS_OFF,
  GNSS_RF_PRIM_WB_RXD_ALWAYS_OFF,
  GNSS_RF_PRIM_WB_RXD_ON,
  GNSS_RF_BP4_GLO_TEST
} LmDiagUserLevelRfMode;

/****************** Function Definitions**************************************/

/*
 ******************************************************************************
 * mode_switch
 *
 * Function description:
 *
 * This function handles mode-switch, independent of client. OFF to ON goes through
 * a transitionary GOING-ON state. Similarly for OFF-ON. Success events are generated
 * when msg returns from MGP. When there's no msg, LM timesout and an error SARF event
 * is sent out. Client CB is called with failure.
 * Functionally, this function switches MGP ON in "SARF" operation mode.
 * This mode is same in every respect as a regular MGP ON except that search
 * strategy is disabled. SARF mode is turned OFF by turning MGP OFF
 *
 * Parameters:
 * e_sarf_state - Desired MGP mode.
 *
 *
 * Return value:
 *
 *
 ******************************************************************************
*/
void lm_diag_proceed_with_mode_switch(void)
    {
      mgp_ConfigStructType *p_OnParams = &z_lmControl.z_MgpONParams;
      lm_mgp_report_SARF_status(TRUE);
      p_OnParams->e_OperationMode = MGP_OPERATION_STANDALONE_RF_VERIF;

      p_OnParams->u_SmMeasRptPeriod   = 10;
      p_OnParams->u_EnablePpmRpt      = FALSE; /*lint -e506 */
      p_OnParams->u_EnablePosRpt      = TRUE;
      p_OnParams->u_EnableUtcRpt      = TRUE;
      p_OnParams->u_DataDemodFlag     = TRUE;
      p_OnParams->q_tbf               = 1000;
      z_lmControl.u_MgpONParamsValid = TRUE;

      /* Send FORCE IDLE if the receiver is ON/GOING_ON */
      if ((lm_is_mgp_on() == TRUE) ||
          (LM_MGP_STATE_GOING_ON == z_lmControl.e_currentMgpState)
          )
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Force RCVR IDLE, current RCVR state %u", z_lmControl.e_currentMgpState);
        /* Put MGP in forced IDLE mode if not in this mode already */
        if (!lm_mgp_update_mgp_state(LM_MGP_STATE_IDLE, NULL, LM_UPDATE_MGP_STATE_FLAG_FORCED))
        {
          event_report(EVENT_GPS_LM_ERROR_SA_RF_VERIF);
          return;
        }
      }
      else
      {
        mgp_ReceiverUpdate(MGP_RCVR_COMMAND_IDLE, NULL); /* Should not need this. Internally MGP should do an IDLE->ON. But doesnt work */
      }

      z_lmControl.z_session_request.e_mode = LM_GPS_MODE_SA_RF;
      (void)lm_mgp_update_mgp_state(LM_MGP_STATE_ON, p_OnParams, LM_UPDATE_MGP_STATE_FLAG_NONE);

      lm_update_session_state(LM_SESSION_STATE_IN_SESSION_NO_REPORT_PENDING);
      z_lm_sarf_state_db.e_sarf_state = LM_SARF_MODE_GOING_ON;
    }

/*
 ******************************************************************************
 * mode_switch
 *
 * Function description:
 *
 * This function handles mode-switch, independent of client. OFF to ON goes through
 * a transitionary GOING-ON state. Similarly for OFF-ON. Success events are generated
 * when msg returns from MGP. When there's no msg, LM timesout and an error SARF event
 * is sent out. Client CB is called with failure.
 * Functionally, this function switches MGP ON in "SARF" operation mode.
 * This mode is same in every respect as a regular MGP ON except that search
 * strategy is disabled. SARF mode is turned OFF by turning MGP OFF
 *
 * Parameters:
 * e_sarf_state - Desired MGP mode.
 *
 *
 * Return value:
 *
 *
 ******************************************************************************
*/
void mode_switch(cgps_sarf_mode_switch_enum_type e_sarf_state)
{
  /* DMM_SM code */
  lm_dmm_state_e_type e_dmm_state;
  sm_ReportXOOffsetInfoStructType z_ReportXOOffsetInfo = { 0 };

  if (z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_GOING_ON)
  {
    event_report(EVENT_GPS_LM_ERROR_SA_RF_VERIF); /* Cannot switch if in transition state */
    return;
  }

  if (e_sarf_state == CGPS_ENTER_SARF_MODE)
  {
    if(LM_DMM_STATE_LOADED != lm_get_dmm_state())
    {
      /*z_lmControl.z_session_request.e_mode has been set to  LM_GPS_MODE_SA_RF
        before loading MGP But it should be cleared 
      */
      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, " CGPS_ENTER_SARF_MODE received, current MGP state %u",lm_get_dmm_state() );
      z_lmControl.z_session_request.e_gnss_activation_reason = LM_MGP_LOAD_FOR_SARF_TEST;
      lm_dmm_state_machine(LM_DMM_EVENT_MGP_LOAD_REQ);
	  return;
    }
    
    /*Before turning receiver MARB has to be informed and so DMM state machine
      is called with event = LM_DMM_EVENT_GNSS_ACTIVATE_REQ
    */
    z_lmControl.z_session_request.e_gnss_activation_reason = LM_MGP_LOAD_FOR_SARF_TEST;
    /*if Dynamic loading of MGP is enabled send ACTIVATION Req to Marb before 
      proceeding with SARF testing
      else proceed directly
	*/
#ifdef  FEATURE_DYNAMIC_LOADING_GNSS
     lm_dmm_state_machine(LM_DMM_EVENT_GNSS_ACTIVATE_REQ);
#else
     lm_diag_proceed_with_mode_switch();
#endif
    
    
    
  }
  else if (e_sarf_state == CGPS_EXIT_SARF_MODE)
  {
/*REV 12/05: If SARF exit command comes when MGP is unloaded call ERR_FATAL */
    if(LM_DMM_STATE_LOADED != lm_get_dmm_state())
    {
		MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"mode_switch:CGPS_EXIT_SARF_MODE received when MGP is in state = %d, activation reason = %d", lm_get_dmm_state(),z_lmControl.z_session_request.e_gnss_activation_reason, 0);
        return;
	}
    z_ReportXOOffsetInfo.v_IsDataValid = FALSE;
    (void)lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL, LM_UPDATE_MGP_STATE_FLAG_NONE);
    lm_update_session_state(LM_SESSION_STATE_NO_SESSION);
    z_lmControl.z_session_request.e_mode = LM_GPS_MODE_NONE;
    z_lm_sarf_state_db.e_sarf_state = LM_SARF_MODE_GOING_OFF;
	z_lmControl.z_session_request.e_gnss_activation_reason = LM_MPG_LOAD_FOR_INVALID_REASON;
    lm_mgp_handle_xooffset_info(&z_ReportXOOffsetInfo);
    
  }
  else
  {
    event_report(EVENT_GPS_LM_ERROR_SA_RF_VERIF); /* Cannot enter SARF under any other conditions*/
  }

}

/*
 ******************************************************************************
 * lm_diag_handle_diag_gsc_msg
 *
 * Function description:
 *
 * This function handles the IPC messages coming from GPS DIAG task.
 *
 * Parameters:
 * *p_msg - IPC message from GPSDIAG
 *
 *
 * Return value:
 *
 *
 ******************************************************************************
*/
void lm_diag_handle_diag_gsc_msg(os_IpcMsgType *p_msg)
{
  aries_diag_sess_ctrl_req_type *p_gscReq;
  if (p_msg == NULL)
  {
    LM_MSG_ERROR("LM received NULL GSC Req from GPSDIAG", 0, 0, 0);
    return;
  }

  p_gscReq = (aries_diag_sess_ctrl_req_type *)p_msg->p_Data; /*lint !e826 */

  switch (p_gscReq->gps_control)
  {
    case 7: /* set GPS search mode / Turn JDA ON/OFF / Reset TIMON Card */
      {
        switch (p_gscReq->supporting_data[0])
        {
          case LM_DIAG_GPS_SRCH_MODE_HIGH: /*Set GPS SRCH mode: High */
            {
              LM_MSG_MED("LM-DIAG received \"Set GPS SRCH mode: High\" from GPSDIAG", 0, 0, 0);
              z_lmControl.u_searchMode = 0x10;
              break;
            }
          case LM_DIAG_GPS_SRCH_MODE_HIGH_DMED: /*Set GPS SRCH mode: High->DeepMedium */
            {
              LM_MSG_MED("LM-DIAG received \"Set GPS SRCH mode: High->DeepMedium\" from GPSDIAG", 0, 0, 0);
              z_lmControl.u_searchMode = 0x13;
              break;
            }
          case LM_DIAG_GPS_SRCH_MODE_HIGH_SLOW: /*Set GPS SRCH mode: High->SuperLow */
            {
              LM_MSG_MED("LM-DIAG received \"Set GPS SRCH mode: High->SuperLow\" from GPSDIAG", 0, 0, 0);
              z_lmControl.u_searchMode = 0x1b;
              break;
            }
          case LM_DIAG_TURN_OFF_JDA: /*Turn off JDA(Jammer Detection Algorithm)*/
            {
              LM_MSG_MED("LM-DIAG received \"Turn off JDA(Jammer Detection Algorithm)\" from GPSDIAG", 0, 0, 0);
              z_lmControl.u_DisableJammerDetect = TRUE;
              break;
            }
          case LM_DIAG_TURN_ON_JDA: /*Turn on JDA(Jammer Detection Algorithm) */
            {
              LM_MSG_MED("LM-DIAG received \"Turn on JDA(Jammer Detection Algorithm)\" from GPSDIAG", 0, 0, 0);
              z_lmControl.u_DisableJammerDetect = FALSE;
              break;
            }
          case LM_DIAG_RESET_TIMON_CARD: /*Reset TIMON Card */
            {
              LM_MSG_MED("LM-DIAG received \"Reset TIMON Card\" from GPSDIAG", 0, 0, 0);
              break;
            }
          case LM_DIAG_GPS_INTELLICEIVER: /* Intelliceiver Command - 0xA0 */
            {
              LM_MSG_MED("LM-DIAG received \"Intelliceiver Command: %d\" from GPSDIAG", p_gscReq->supporting_data[1], 0, 0);
              mgp_SetGpsIntelliceiver((mgp_GpsIntelliceiverLinearityType)p_gscReq->supporting_data[1]);
              break;
            }
          default:
            {
              LM_MSG_ERROR("LM-DIAG received unknown command[%d] from GPSDIAG", p_gscReq->supporting_data[0], 0, 0);
              break;
            }
        }
        break;
      } /*case 7 ends*/

    default:
      {
        LM_MSG_ERROR("Received unknown GSC command[%d] from GPSDIAG", p_gscReq->gps_control, 0, 0);
        break;
      }

  }

}

/*
 ******************************************************************************
 * lm_presc_dwell_handler
 *
 * Function description:
 *
 *  lm_presc_dwell_handler simply transfers IPC payload to Presc-dwell struct
 *  injected into MGP.
 *
 * Parameters:
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/
void lm_presc_dwell_handler(os_IpcMsgType *p_msg)
{
  uint8 *pu_VersionPtr = NULL;
  mgp_PrescDwellCmd z_Cmd;

  if (NULL == p_msg)
  {
    LM_MSG_ERROR_0("Received NULL Presc Dwell cmd from TM diag");
    return;
  }

  pu_VersionPtr = (uint8 *)p_msg->p_Data;
  LM_MSG_HIGH_1("Handle Presc Dwell cmd version %d", *pu_VersionPtr);

  if (*pu_VersionPtr == 53) /* HW Rev 56 - Full GAL Online RF Dev */
  {
    /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
    lm_GnssPrescDwellType_V53 *p_PrescDwellCmd = (lm_GnssPrescDwellType_V53 *)p_msg->p_Data;
    z_Cmd.u_CmdControlFlag = p_PrescDwellCmd->u_CmdControlFlag;
    z_Cmd.q_SeqNum = p_PrescDwellCmd->q_SeqNum;
    z_Cmd.u_GpsRxDFlag = p_PrescDwellCmd->u_GpsRxDFlag;
    z_Cmd.u_GpsControlFlag = p_PrescDwellCmd->u_GpsControlFlag;
    z_Cmd.u_GpsSv = p_PrescDwellCmd->u_GpsSv;
    z_Cmd.l_GpsDoppMps = p_PrescDwellCmd->l_GpsDoppMps;
    z_Cmd.l_GpsDoppWinHz = p_PrescDwellCmd->l_GpsDoppWinHz;
    z_Cmd.q_GpsCpCenterCx40 = p_PrescDwellCmd->q_GpsCpCenterCx40;
    z_Cmd.q_GpsCodeWinSizeChipx1 = p_PrescDwellCmd->q_GpsCodeWinSizeChipx1;
    z_Cmd.u_GpsSrchMode = p_PrescDwellCmd->u_GpsSrchMode;
    z_Cmd.w_GpsNumPostD = p_PrescDwellCmd->w_GpsNumPostD;
    z_Cmd.u_GpsPfa = p_PrescDwellCmd->u_GpsPfa;
    z_Cmd.q_GpsReserved = p_PrescDwellCmd->q_GpsReserved;
    z_Cmd.u_GloControlFlag = p_PrescDwellCmd->u_GloControlFlag;
    z_Cmd.u_GloSv = p_PrescDwellCmd->u_GloSv;
    z_Cmd.l_GloDoppMps = p_PrescDwellCmd->l_GloDoppMps;
    z_Cmd.l_GloDoppWinHz = p_PrescDwellCmd->l_GloDoppWinHz;
    z_Cmd.q_GloCpCenterCx40 = p_PrescDwellCmd->q_GloCpCenterCx40;
    z_Cmd.q_GloCodeWinSizeChipx1 = p_PrescDwellCmd->q_GloCodeWinSizeChipx1;
    z_Cmd.u_GloSrchMode = p_PrescDwellCmd->u_GloSrchMode;
    z_Cmd.w_GloNumPostD = p_PrescDwellCmd->w_GloNumPostD;
    z_Cmd.u_GloHwChan = p_PrescDwellCmd->u_GloHwChan;
    z_Cmd.u_GloPfa = p_PrescDwellCmd->u_GloPfa;
    z_Cmd.q_GloReserved = p_PrescDwellCmd->q_GloReserved;
    z_Cmd.w_DwellCnt = p_PrescDwellCmd->w_DwellCnt;
    z_Cmd.w_TotalNumDwells = p_PrescDwellCmd->w_TargetDwellCnt;
    z_Cmd.w_PassDwellCnt = p_PrescDwellCmd->w_PassDwellCnt;
    z_Cmd.q_Reserved = p_PrescDwellCmd->q_Reserved;
    z_Cmd.u_BdsControlFlag = p_PrescDwellCmd->u_BdsControlFlag;
    z_Cmd.u_BdsSv = p_PrescDwellCmd->u_BdsSv;
    z_Cmd.l_BdsDoppMps = p_PrescDwellCmd->l_BdsDoppMps;
    z_Cmd.l_BdsDoppWinHz = p_PrescDwellCmd->l_BdsDoppWinHz;
    z_Cmd.q_BdsCpCenterCx40 = p_PrescDwellCmd->q_BdsCpCenterCx40;
    z_Cmd.q_BdsCodeWinSizeChipx1 = p_PrescDwellCmd->q_BdsCodeWinSizeChipx1;
    z_Cmd.u_BdsSrchMode = p_PrescDwellCmd->u_BdsSrchMode;
    z_Cmd.w_BdsNumPostD = p_PrescDwellCmd->w_BdsNumPostD;
    z_Cmd.u_BdsPfa = p_PrescDwellCmd->u_BdsPfa;
    z_Cmd.q_BdsReserved = p_PrescDwellCmd->q_BdsReserved;
    z_Cmd.u_MaxSvDwell = p_PrescDwellCmd->u_MaxSvDwell;
    z_Cmd.u_GalControlFlag = p_PrescDwellCmd->u_GalControlFlag;
    z_Cmd.u_GalSv = p_PrescDwellCmd->u_GalSv;
    z_Cmd.l_GalDoppMps = p_PrescDwellCmd->l_GalDoppMps;
    z_Cmd.l_GalDoppWinHz = p_PrescDwellCmd->l_GalDoppWinHz;
    z_Cmd.q_GalCpCenterCx40 = p_PrescDwellCmd->q_GalCpCenterCx40;
    z_Cmd.q_GalCodeWinSizeChipx1 = p_PrescDwellCmd->q_GalCodeWinSizeChipx1;
    z_Cmd.u_GalSrchMode = p_PrescDwellCmd->u_GalSrchMode;
    z_Cmd.w_GalNumPostD = p_PrescDwellCmd->w_GalNumPostD;
    z_Cmd.u_GalPfa = p_PrescDwellCmd->u_GalPfa;
    z_Cmd.q_GalReserved = p_PrescDwellCmd->q_GalReserved;
  }
  else if (*pu_VersionPtr == 51) /* HW Rev 54 */
  {
    /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
    lm_GnssPrescDwellType_V51 *p_PrescDwellCmd = (lm_GnssPrescDwellType_V51 *)p_msg->p_Data;
    z_Cmd.u_CmdControlFlag = p_PrescDwellCmd->u_CmdControlFlag;
    z_Cmd.q_SeqNum = p_PrescDwellCmd->q_SeqNum;
    z_Cmd.u_GpsRxDFlag = p_PrescDwellCmd->u_GpsRxDFlag;
    z_Cmd.u_GpsControlFlag = p_PrescDwellCmd->u_GpsControlFlag;
    z_Cmd.u_GpsSv = p_PrescDwellCmd->u_GpsSv;
    z_Cmd.l_GpsDoppMps = p_PrescDwellCmd->l_GpsDoppMps;
    z_Cmd.l_GpsDoppWinHz = p_PrescDwellCmd->l_GpsDoppWinHz;
    z_Cmd.q_GpsCpCenterCx40 = p_PrescDwellCmd->q_GpsCpCenterCx40;
    z_Cmd.q_GpsCodeWinSizeChipx1 = p_PrescDwellCmd->w_GpsCodeWinSizeChipx1;
    z_Cmd.u_GpsSrchMode = p_PrescDwellCmd->u_GpsSrchMode;
    z_Cmd.w_GpsNumPostD = p_PrescDwellCmd->w_GpsNumPostD;
    z_Cmd.u_GpsPfa = p_PrescDwellCmd->u_GpsPfa;
    z_Cmd.q_GpsReserved = p_PrescDwellCmd->q_GpsReserved;
    z_Cmd.u_GloControlFlag = p_PrescDwellCmd->u_GloControlFlag;
    z_Cmd.u_GloSv = p_PrescDwellCmd->u_GloSv;
    z_Cmd.l_GloDoppMps = p_PrescDwellCmd->l_GloDoppMps;
    z_Cmd.l_GloDoppWinHz = p_PrescDwellCmd->l_GloDoppWinHz;
    z_Cmd.q_GloCpCenterCx40 = p_PrescDwellCmd->q_GloCpCenterCx40;
    z_Cmd.q_GloCodeWinSizeChipx1 = p_PrescDwellCmd->w_GloCodeWinSizeChipx1;
    z_Cmd.u_GloSrchMode = p_PrescDwellCmd->u_GloSrchMode;
    z_Cmd.w_GloNumPostD = p_PrescDwellCmd->w_GloNumPostD;
    z_Cmd.u_GloHwChan = p_PrescDwellCmd->u_GloHwChan;
    z_Cmd.u_GloPfa = p_PrescDwellCmd->u_GloPfa;
    z_Cmd.q_GloReserved = p_PrescDwellCmd->q_GloReserved;
    z_Cmd.w_DwellCnt = p_PrescDwellCmd->w_DwellCnt;
    z_Cmd.w_TotalNumDwells = p_PrescDwellCmd->w_TargetDwellCnt;
    z_Cmd.w_PassDwellCnt = p_PrescDwellCmd->w_PassDwellCnt;
    z_Cmd.q_Reserved = p_PrescDwellCmd->q_Reserved;
    z_Cmd.u_BdsControlFlag = p_PrescDwellCmd->u_BdsControlFlag;
    z_Cmd.u_BdsSv = p_PrescDwellCmd->u_BdsSv;
    z_Cmd.l_BdsDoppMps = p_PrescDwellCmd->l_BdsDoppMps;
    z_Cmd.l_BdsDoppWinHz = p_PrescDwellCmd->l_BdsDoppWinHz;
    z_Cmd.q_BdsCpCenterCx40 = p_PrescDwellCmd->q_BdsCpCenterCx40;
    z_Cmd.q_BdsCodeWinSizeChipx1 = p_PrescDwellCmd->w_BdsCodeWinSizeChipx1;
    z_Cmd.u_BdsSrchMode = p_PrescDwellCmd->u_BdsSrchMode;
    z_Cmd.w_BdsNumPostD = p_PrescDwellCmd->w_BdsNumPostD;
    z_Cmd.u_BdsPfa = p_PrescDwellCmd->u_BdsPfa;
    z_Cmd.q_BdsReserved = p_PrescDwellCmd->q_BdsReserved;
    z_Cmd.u_MaxSvDwell = 0;
    z_Cmd.u_GalControlFlag = 0;
    z_Cmd.u_GalSv = 0;
    z_Cmd.l_GalDoppMps = 0;
    z_Cmd.l_GalDoppWinHz = 0;
    z_Cmd.q_GalCpCenterCx40 = 0;
    z_Cmd.q_GalCodeWinSizeChipx1 = 0;
    z_Cmd.u_GalSrchMode = 0;
    z_Cmd.w_GalNumPostD = 0;
    z_Cmd.u_GalPfa = 0;
    z_Cmd.q_GalReserved = 0;
  }
  else
  {
    LM_MSG_ERROR_1("Unknown Presc Dwell cmd version %d", *pu_VersionPtr);
    return;
  }

  mgp_SendPrescDwellRequest(&z_Cmd);

}

/*
 ******************************************************************************
 * lm_spectrum_analyzer_handler
 *
 * Function description:
 *
 *  This function transfers the IPC payload to Spectrum Analyzer struct. It is then
 *  injected into MGP.
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_spectrum_analyzer_handler(os_IpcMsgType *p_msg)
{
  lm_SpectrumAnalyzerCmdType *p_Cmd;
  mgp_SpectrumAnalyzerCmd    z_SpAnCmd;

  p_Cmd = (lm_SpectrumAnalyzerCmdType *)p_msg->p_Data;

  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  z_SpAnCmd.u_Version = p_Cmd->u_Version;
  z_SpAnCmd.u_CmdControlFlag = p_Cmd->u_CmdControlFlag;
  z_SpAnCmd.u_GpsJobsEnabled = p_Cmd->u_GpsJobsEnabled;
  z_SpAnCmd.u_GloJobsEnabled = p_Cmd->u_GloJobsEnabled;
  z_SpAnCmd.u_BdsJobsEnabled = p_Cmd->u_BdsJobsEnabled;
  z_SpAnCmd.u_GalJobsEnabled = p_Cmd->u_GalJobsEnabled;
  z_SpAnCmd.u_ScanStartStop  = p_Cmd->u_ScanStartStop;
  z_SpAnCmd.u_ProbePoint     = p_Cmd->u_ProbePoint;
  z_SpAnCmd.u_TrkMode        = p_Cmd->u_TrkMode;
  z_SpAnCmd.u_NotchFilterReset = p_Cmd->u_NotchFilterReset;


  mgp_SendSpectrumAnalyzerCmdRequest(&z_SpAnCmd);

}  /* lm_spectrum_analyzer_handler */

/*
 ******************************************************************************
 * lm_Rf_VerifModeSwitch_handler
 *
 * Function description:
 *
 * lm_Rf_VerifModeSwitch_handler is called by TM diag. For this client (other
 * client being FTM), we simply return event (no CBs) OR do the required mode
 * switch.
 *
 * Parameters:
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/
void lm_Rf_VerifModeSwitch_handler(os_IpcMsgType *p_msg)
{
  lm_sarf_mode_type *p_ModeCmd;
  cgps_sarf_mode_switch_enum_type e_new_state;

  if (p_msg == NULL)
  {
    LM_MSG_ERROR("LM received NULL Presc Dwell Cmd from TM diag", 0, 0, 0);
    return;
  }
  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  p_ModeCmd = (lm_sarf_mode_type *)p_msg->p_Data;
  e_new_state = (cgps_sarf_mode_switch_enum_type)p_ModeCmd->u_sarf_mode_state;

  z_lm_sarf_state_db.p_client_cb = NULL;
  z_lm_sarf_state_db.e_client_type = LM_SARF_CLIENT_DIAG;

  if (e_new_state == CGPS_ENTER_SARF_MODE && z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_ON)
  {
    lm_mgp_report_SARF_status(TRUE);
    event_report(EVENT_GPS_LM_ENTER_SA_RF_VERIF);
  }
  else if (e_new_state == CGPS_EXIT_SARF_MODE && z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_OFF)
  {
    lm_mgp_report_SARF_status(FALSE);
    event_report(EVENT_GPS_LM_EXIT_SA_RF_VERIF);
  }
  else
    mode_switch(e_new_state);
}

/*
 ******************************************************************************
 * lm_IqTestCmd
 *
 * Function description:
 *
 *  lm_IqTestCmd transfers IPC payload to Iqtest struct and sends it to MGP.
 *
 * Parameters:
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/
void lm_IqTestCmd(os_IpcMsgType *p_msg)
{
  lm_GnssIqTestType *p_IqCmd;
  mgp_IqTestCmdStructType z_IqTestCmd;
  if (p_msg == NULL)
  {
    LM_MSG_ERROR("LM received NULL IqTest cmd from TM diag", 0, 0, 0);
    return;
  }

  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  p_IqCmd = (lm_GnssIqTestType *)p_msg->p_Data;

  z_IqTestCmd.u_Version = p_IqCmd->u_Version;
  z_IqTestCmd.u_TestControl = p_IqCmd->u_TestControl;
  z_IqTestCmd.w_Flags = p_IqCmd->w_Flags;
  z_IqTestCmd.w_RetransmitPktNum = p_IqCmd->w_RetransmitPktNum;
  z_IqTestCmd.u_RfStatusLoggingRateHz = p_IqCmd->u_RfStatusLoggingRateHz;
  z_IqTestCmd.u_CollectMode = p_IqCmd->u_CollectMode;
  z_IqTestCmd.b_GloR1Chan = p_IqCmd->b_GloR1Chan;
  z_IqTestCmd.u_GloHWChan = p_IqCmd->u_GloHWChan;
  z_IqTestCmd.w_SampleCnt1k = p_IqCmd->w_SampleCnt1k;
  z_IqTestCmd.u_Integrations = p_IqCmd->u_Integrations;
  z_IqTestCmd.e_ClientType = IQ_COLLECT_CLIENT_APP;
  z_IqTestCmd.p_Buffer = NULL;
  z_IqTestCmd.l_CenterFreqNBHz = p_IqCmd->l_CenterFreqNBHz;
  if (p_IqCmd->u_Version >= C_IQ_TEST_CMD_VERSION_V53)
  {
    z_IqTestCmd.q_ADCIqSatThresh = p_IqCmd->q_ADCIqSatThresh;
  }
  if (p_IqCmd->u_Version >= C_IQ_TEST_CMD_VERSION_V54)
  {
    z_IqTestCmd.q_AdcIQWidebandJammerStartFreqHz =
       p_IqCmd->q_AdcIQWidebandJammerStartFreqHz;
    z_IqTestCmd.q_AdcIQWidebandJammerStopFreqHz =
       p_IqCmd->q_AdcIQWidebandJammerStopFreqHz;
    z_IqTestCmd.l_AdcIQRefTonePwrDBm =
       p_IqCmd->l_AdcIQRefTonePwrDBm;

    /* Debug code, remove later */
    LM_MSG_HIGH("WB Jammer Start %d MHz Stop %d MHz, Ref tone power %d dBm",
                z_IqTestCmd.q_AdcIQWidebandJammerStartFreqHz / 1000000,
                z_IqTestCmd.q_AdcIQWidebandJammerStopFreqHz / 1000000,
                z_IqTestCmd.l_AdcIQRefTonePwrDBm);
  }
  else
  {
    z_IqTestCmd.q_AdcIQWidebandJammerStartFreqHz = 0;
    z_IqTestCmd.q_AdcIQWidebandJammerStopFreqHz = 0;
  }
  mgp_SendIqTestRequest(&z_IqTestCmd);
  (void)os_TimerStart(z_lmControl.p_IqTestingTimer, 25000, 0);

  return;
}

/*
 ******************************************************************************
 * lm_StandaloneTestCmd_handler
 *
 * Function description:
 *
 *  lm_StandaloneTestCmd_handler starts a single SV or standalone operation.
 *  If the cmd includes clear all, then database of SV (eph, alm, health etc.)
 *  are deleted.
 *
 * Parameters:
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/
void lm_StandaloneTestCmd_handler(os_IpcMsgType *p_msg)
{
  uint64 t_SvMask = 0;
  lm_dmm_state_e_type e_dmm_state;
  lm_1sv_standalone_type *p_Cmd;
  lm_request_delete_mgp_info_s_type z_delete_info;
  mgp_ConfigStructType z_OnParams, *p_OnParams = &z_OnParams;

  memset(&z_delete_info, 0, sizeof(lm_request_delete_mgp_info_s_type));

  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  p_Cmd = (lm_1sv_standalone_type *)p_msg->p_Data;

  z_OnParams.u_SmMeasRptPeriod   = 10;
  z_OnParams.u_EnablePpmRpt      = FALSE; /*lint -e506 */
  z_OnParams.u_EnablePosRpt      = TRUE;
  z_OnParams.u_EnableUtcRpt      = TRUE;
  z_OnParams.u_DataDemodFlag     = TRUE;
  z_OnParams.q_QosAccuracyThreshold = 250; /* Use a default of 250m. This is not
                                              needed for Single SV track, but
                                              is used to enable/disable DPO */
  z_OnParams.q_tbf = 1000;

  switch (p_Cmd->u_Command)
  {
    case LM_STANDALONE_TEST_START_CLEAR_ALL_NO_BIT_EDGE:
      /* This test case will not require Bit Edge detection before the SV enters
         into Track mode */
      mgp_DisableBitEdgeDetection(TRUE);

      /*lint -fallthrough */
    case LM_STANDALONE_TEST_START_CLEAR_ALL_FORCE_DPO:
      if (p_Cmd->u_Command == LM_STANDALONE_TEST_START_CLEAR_ALL_FORCE_DPO)
      {
        /* This test case will require DPO to be forced */
        mgp_ForceDpoOperation(TRUE);
      }

      /*lint -fallthrough */
    case LM_STANDALONE_TEST_START_CLEAR_ALL:
      {
        /* Send a cmd to MGP to delete ALL */
        memset(&z_delete_info, 0, sizeof(z_delete_info));
        z_delete_info.delete_all = TRUE;
        z_delete_info.force_rcvr_off = FALSE;

        /* Deletes GPS and Glonass data */
        z_delete_info.z_mgp_info.e_GnssType = MGP_GNSS_TYPE_ALL;

        lm_mgp_handle_delete_mgp_info(&z_delete_info);
      }

      /*lint -fallthrough */
    case LM_STANDALONE_TEST_START_NO_CLEAR_NO_BIT_EDGE:
      if (p_Cmd->u_Command == LM_STANDALONE_TEST_START_NO_CLEAR_NO_BIT_EDGE)
      {
        /* This test case will not require Bit Edge detection before the SV enters
           into Track mode */
        mgp_DisableBitEdgeDetection(TRUE);
      }

      /*lint -fallthrough */
    case LM_STANDALONE_TEST_START_NO_CLEAR_FORCE_DPO:
      if (p_Cmd->u_Command == LM_STANDALONE_TEST_START_NO_CLEAR_FORCE_DPO)
      {
        /* This test case will require DPO to be forced */
        mgp_ForceDpoOperation(TRUE);
      }

      /*lint -fallthrough */
    case LM_STANDALONE_TEST_START_NO_CLEAR:
      {
        /* If the Receiver is in SARF mode, just send a Single SV Track command
           Otherwise, use the legacy interface of setting all the other SVs to
           NoExist, and turning ON receiver.
        */
        if (LM_GPS_MODE_SA_RF == z_lmControl.z_session_request.e_mode)
        {
          /* Send a Single SV Track command */
          mgp_SingleSvTrack(p_Cmd->w_Sv);
        }
        else
        {
          z_OnParams.e_OperationMode = MGP_OPERATION_STANDALONE;


          /* Send a cmd to LM to turn Receiver ON */
          if (p_Cmd->w_Sv)
          {
            uint16 w_i;
            uint32 q_SvNoExistMask = 0;         /* Bit Mask specifying the SVs which do not exist ('1' => No exist) */
            gnss_AssistDataInfoStructType z_AssistData;

            for (w_i = 1; w_i <= N_SV; w_i++)
            {
              if (w_i != p_Cmd->w_Sv)
                q_SvNoExistMask |= 1 << (w_i - 1);
            }
            z_AssistData.e_AssistanceType = MGP_GNSS_ASSIST_DATA_SV_NO_EXIST;
            z_AssistData.z_AssistData.z_SvNoExistInfo.q_SvNoExistMask = q_SvNoExistMask;
            z_AssistData.u_Force = TRUE;
            z_AssistData.e_GnssType = MGP_GNSS_TYPE_GPS;

            (void)lm_mgp_update_info(&z_AssistData);
          }
          else
          {
            uint32 q_SvNoExistMask;         /* Bit Mask specifying the SVs which do not exist ('1' => No exist) */
            gnss_AssistDataInfoStructType z_AssistData;

            q_SvNoExistMask = 0;
            z_AssistData.e_AssistanceType = MGP_GNSS_ASSIST_DATA_SV_NO_EXIST;
            z_AssistData.z_AssistData.z_SvNoExistInfo.q_SvNoExistMask = q_SvNoExistMask;
            z_AssistData.u_Force = TRUE;
            z_AssistData.e_GnssType = MGP_GNSS_TYPE_GPS;
            (void)lm_mgp_update_info(&z_AssistData);
          }
          z_lmControl.u_MgpONParamsValid = TRUE;
          (void)lm_mgp_update_mgp_state(LM_MGP_STATE_ON, &z_OnParams, LM_UPDATE_MGP_STATE_FLAG_NONE);
          break;
        }
      }

    case LM_STANDALONE_TEST_START_SBAS_CLEAR_ALL:
    case LM_STANDALONE_TEST_START_SBAS_NO_CLEAR:
    case LM_STANDALONE_TEST_START_SBAS_CLEAR_ALL_NO_BITEDGE_DETECTION:
    case LM_STANDALONE_TEST_START_SBAS_NO_CLEAR_NO_BITEDGE_DETECTION:
      if ((p_Cmd->u_Command == LM_STANDALONE_TEST_START_SBAS_CLEAR_ALL_NO_BITEDGE_DETECTION) ||
          (p_Cmd->u_Command == LM_STANDALONE_TEST_START_SBAS_NO_CLEAR_NO_BITEDGE_DETECTION)
          )
      {
        /* This test case will not require Bit Edge detection before the SV enters
           into Track mode */
        mgp_DisableBitEdgeDetection(TRUE);
      }

      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Turning ON SBAS test: SV %d", p_Cmd->w_Sv, 0, 0);

      if (p_Cmd->w_Sv)
      {
        t_SvMask |= 1UL << (p_Cmd->w_Sv - 1);
      }
      else
      {
        t_SvMask = 0;
      }

      mgp_SetSbasCannedSvList(t_SvMask);

      /* Now turn on MGP */
      p_OnParams->e_OperationMode = MGP_OPERATION_SBAS_TESTMODE;
      p_OnParams->u_SmMeasRptPeriod   = 0;
      p_OnParams->u_EnablePpmRpt      = FALSE; /*lint -e506 */
      p_OnParams->u_EnablePosRpt      = FALSE;
      p_OnParams->u_EnableUtcRpt      = FALSE;
      p_OnParams->u_DataDemodFlag     = TRUE;
      p_OnParams->q_tbf = 1000;
      lm_DiagSbasStart(p_OnParams);
      break;

    case LM_STANDALONE_TEST_STOP_CLEAR_ALL:
      {
        /* Send a cmd to MGP to delete ALL */
        memset(&z_delete_info, 0, sizeof(z_delete_info));
        z_delete_info.delete_all = TRUE;
        z_delete_info.force_rcvr_off = FALSE;

        /* Deletes GPS and Glonass data */
        z_delete_info.z_mgp_info.e_GnssType = MGP_GNSS_TYPE_ALL;

        lm_mgp_handle_delete_mgp_info(&z_delete_info);
      }

      /*lint -fallthrough */
    case LM_STANDALONE_TEST_STOP_NO_CLEAR:
      {
        /* Just in case this was enabled, reset it back to requiring Bit Edge
           Detection before entering Track */
        mgp_DisableBitEdgeDetection(FALSE);

        /* Just in case DPO was forced, reset it back to default (un-forced) */
        mgp_ForceDpoOperation(FALSE);

        mgp_ReceiverUpdate(MGP_RCVR_COMMAND_IDLE, NULL);
        
        /* If MGP is in SARF mode, put it back in SARF mode, else, turn receiver OFF */
        if (z_lmControl.z_session_request.e_mode == LM_GPS_MODE_SA_RF)
        {
          z_OnParams.e_OperationMode = MGP_OPERATION_STANDALONE_RF_VERIF;
          (void)lm_mgp_update_mgp_state(LM_MGP_STATE_ON, &z_OnParams, LM_UPDATE_MGP_STATE_FLAG_NONE);
        }
        else
        {
          (void)lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL, LM_UPDATE_MGP_STATE_FLAG_NONE);
        }
        break;
      }
    default:
      break;
  }
}

/*
 ******************************************************************************
 * lm_TranslateMultiChanTrack
 *
 * Function description:
 *  Helper function to translate from older "legacy" versions of the command
 *  structure to the current one, version 56.
 *
 * Parameters:
 *   *p_Data - Pointer to the Multichannel Track command
 *
 * Return value:
 *   lm_multichan_track_type* - Pointer to the Multichannel Track command
 *   using the current definition.
 *
 ******************************************************************************
*/
static lm_multichan_track_type* lm_TranslateMultiChanTrack(uint8 *p_Data)
{
  lm_multichan_track_type *p_Cmd = NULL;
  lm_multichan_track_legacy_type z_MultiChanTrackLegacy; /* older than v55 */
  lm_multichan_track_v55_type z_MultiChanTrackv55; /* version 55 */

  if (p_Data != NULL)
  {
    p_Cmd = (lm_multichan_track_type *)p_Data;

    /* Version field is in the same offset in all versions of the command */
    if (p_Cmd->u_Version < C_GNSS_LM_MULTICHAN_TRACK_VER55)
    {
      /* Copy the legacy data */
      memscpy((void *)&z_MultiChanTrackLegacy, 
              sizeof(lm_multichan_track_legacy_type),
              (const void *)p_Data, sizeof(lm_multichan_track_legacy_type));

      /* Translate to v56 */
      memset((void*)p_Cmd, 0, sizeof(lm_multichan_track_type));
      p_Cmd->u_Version = C_GNSS_LM_MULTICHAN_TRACK_VER56;
      p_Cmd->u_Test_Mode = z_MultiChanTrackLegacy.u_Test_Mode;
      p_Cmd->u_Command = z_MultiChanTrackLegacy.u_Command;
      p_Cmd->u_GLO_HW_Chan_Num = z_MultiChanTrackLegacy.u_GLO_HW_Chan_Num;
      p_Cmd->u_GPS_L1_SBAS_SV_ID = z_MultiChanTrackLegacy.u_GPS_SBAS_SV_ID;
      p_Cmd->u_GPS_L1_SBAS_BitEdge_Setting = 
        z_MultiChanTrackLegacy.u_GPS_SBAS_BitEdge_Setting;
      p_Cmd->l_GLO_G1_Freq_ID = z_MultiChanTrackLegacy.l_GLO_Freq_ID;
      p_Cmd->u_GLO_G1_BitEdge_Setting = 
        z_MultiChanTrackLegacy.u_GLO_BitEdge_Setting;
      p_Cmd->u_BDS_B1_SV_ID = z_MultiChanTrackLegacy.u_BDS_SV_ID;
      p_Cmd->u_BDS_B1_BitEdge_Setting = 
        z_MultiChanTrackLegacy.u_BDS_BitEdge_Setting;
    }
    else if (p_Cmd->u_Version == C_GNSS_LM_MULTICHAN_TRACK_VER55)
    {
      /* Copy the legacy data */
      memscpy((void *)&z_MultiChanTrackv55, 
              sizeof(lm_multichan_track_v55_type),
              (const void *)p_Data, sizeof(lm_multichan_track_v55_type));

      /* Translate to v56 */
      memset((void*)p_Cmd, 0, sizeof(lm_multichan_track_type));
      p_Cmd->u_Version = C_GNSS_LM_MULTICHAN_TRACK_VER56;
      p_Cmd->u_Test_Mode = z_MultiChanTrackv55.u_Test_Mode;
      p_Cmd->u_Command = z_MultiChanTrackv55.u_Command;
      p_Cmd->u_GLO_HW_Chan_Num = z_MultiChanTrackv55.u_GLO_HW_Chan_Num;
      p_Cmd->u_GPS_L1_SBAS_SV_ID = z_MultiChanTrackv55.u_GPS_SBAS_SV_ID;
      p_Cmd->u_GPS_L1_SBAS_BitEdge_Setting = 
        z_MultiChanTrackv55.u_GPS_SBAS_BitEdge_Setting;
      p_Cmd->l_GLO_G1_Freq_ID = z_MultiChanTrackv55.l_GLO_Freq_ID;
      p_Cmd->u_GLO_G1_BitEdge_Setting = 
        z_MultiChanTrackv55.u_GLO_BitEdge_Setting;
      p_Cmd->u_BDS_B1_SV_ID = z_MultiChanTrackv55.u_BDS_SV_ID;
      p_Cmd->u_BDS_B1_BitEdge_Setting = 
        z_MultiChanTrackv55.u_BDS_BitEdge_Setting;
      p_Cmd->u_GAL_E1_SV_ID = z_MultiChanTrackv55.u_GAL_SV_ID;
      p_Cmd->u_GAL_E1_BitEdge_Setting = 
        z_MultiChanTrackv55.u_GAL_BitEdge_Setting;
    }
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,
        "lm_TranslateMultiChanTrack: p_Data is NULL");
  }

  return (p_Cmd);
}

/*
 ******************************************************************************
 * lm_RfDevTestCmd_handler
 *
 * Function description:
 *
 *  lm_RfDevTestCmd_handler starts a multi-channel tracking operation.
 *  If the cmd includes clear all, then database of SV (eph, alm, health etc.)
 *  are deleted.
 *
 * Parameters:
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/
void lm_RfDevTestCmd_handler(os_IpcMsgType *p_msg)
{
  uint32 q_GpsSvMask = 0;

  lm_multichan_track_type *p_Cmd = NULL;
  lm_request_delete_mgp_info_s_type z_delete_info;
  mgp_ConfigStructType *p_OnParams = &z_lmControl.z_MgpONParams;
  gnss_AssistDataInfoStructType z_AssistDataInfo;

  if (p_msg != NULL)
  {
    /* Translate the command, if necessary */
    p_Cmd = lm_TranslateMultiChanTrack(p_msg->p_Data);

    if (p_Cmd != NULL)
    {
      memset(&z_delete_info, 0, sizeof(lm_request_delete_mgp_info_s_type));
      memset(&z_delete_info, 0, sizeof(z_delete_info)); /* Initialize z_delete_info */

      p_OnParams->u_SmMeasRptPeriod   = 10;
      p_OnParams->u_EnablePpmRpt      = FALSE; /*lint -e506 */
      p_OnParams->u_EnablePosRpt      = TRUE;
      p_OnParams->u_EnableUtcRpt      = TRUE;
      p_OnParams->u_DataDemodFlag     = TRUE;
      p_OnParams->q_QosAccuracyThreshold = 250; /* Use a default of 250m. This is not
                                                  needed for Single SV track, but
                                                  is used to enable/disable DPO */
      p_OnParams->q_tbf = 1000;

      /* Update Bit Edge Detection Setting */
      mgp_GnssDisableBitEdgeDetection(MGP_GNSS_TYPE_GPS, 0, p_Cmd->u_GPS_L1_SBAS_BitEdge_Setting);
      mgp_GnssDisableBitEdgeDetection(MGP_GNSS_TYPE_GLO, 0, p_Cmd->u_GLO_G1_BitEdge_Setting);
      mgp_GnssDisableBitEdgeDetection(MGP_GNSS_TYPE_BDS, 0, p_Cmd->u_BDS_B1_BitEdge_Setting);
      mgp_GnssDisableBitEdgeDetection(MGP_GNSS_TYPE_GAL, 0, p_Cmd->u_GAL_E1_BitEdge_Setting);

      /* DPO Setting */
      if (p_Cmd->u_Test_Mode == (uint8)MultiChan_L1_DPO)
      {
        if ((p_Cmd->u_Command == (uint8)LM_RFDEV_TEST_START_CLEAR_ALL) || (p_Cmd->u_Command == (uint8)LM_RFDEV_TEST_START_NO_CLEAR))
        {
          /* Force DPO Operation */
          mgp_ForceDpoOperation(TRUE);
        }
      }

      switch (p_Cmd->u_Command)
      {
        case LM_RFDEV_TEST_START_CLEAR_ALL:
          {
            z_AssistDataInfo.e_AssistanceType = MGP_GNSS_ASSIST_DATA_RF_DEV_CMD_INFO;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.u_ClearFlag = TRUE;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.e_RfDevTrackCmd = MultiChan_Track_Cmd_Start;

            /* Clear existing RF Dev Cmd info from MC Database */
            (void)lm_mgp_update_info(&z_AssistDataInfo);

            /* Send a cmd to MGP to delete ALL */
            memset(&z_delete_info, 0, sizeof(z_delete_info));
            z_delete_info.delete_all = TRUE;
            z_delete_info.force_rcvr_off = FALSE;

            /* Deletes GNSS data */
            z_delete_info.z_mgp_info.e_GnssType = MGP_GNSS_TYPE_ALL;

            lm_mgp_handle_delete_mgp_info(&z_delete_info);
          }
          /*lint -e{616} Legacy behavior of code where case statements purposely fall over to next */
        case LM_RFDEV_TEST_START_NO_CLEAR: /* intentional cascade from previous case */
          /* SBAS Setting */
          if (p_Cmd->u_Test_Mode == (uint8)MultiChan_Track_L1_SBAS)
          {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Turning ON SBAS test: SV %d", p_Cmd->u_GPS_L1_SBAS_SV_ID, 0, 0);
            if (p_Cmd->u_GPS_L1_SBAS_SV_ID)
            {
              q_GpsSvMask |= 1UL << (p_Cmd->u_GPS_L1_SBAS_SV_ID - 1);
            }
            else
            {
              q_GpsSvMask = 0;
            }
            mgp_SetSbasCannedSvList(q_GpsSvMask);

            /* Now turn on MGP */

            p_OnParams->e_OperationMode = MGP_OPERATION_SBAS_TESTMODE;
            p_OnParams->u_SmMeasRptPeriod   = 0;
            p_OnParams->u_EnablePpmRpt      = FALSE; /*lint -e506 */
            p_OnParams->u_EnablePosRpt      = FALSE;
            p_OnParams->u_EnableUtcRpt      = FALSE;
            p_OnParams->u_DataDemodFlag     = TRUE;
            p_OnParams->q_tbf               = 1000;
            lm_DiagSbasStart(p_OnParams);
            break;
          }
          else
          {
            p_OnParams->e_OperationMode = MGP_OPERATION_STANDALONE;

            /* Update MC Database with the RF DEV Cmd info */

            z_AssistDataInfo.e_AssistanceType = MGP_GNSS_ASSIST_DATA_RF_DEV_CMD_INFO;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.u_ClearFlag = FALSE;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.u_GloHwChanNum = p_Cmd->u_GLO_HW_Chan_Num;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.e_RfDevTrackInfo = (gnss_RfDevTrackEnumType)p_Cmd->u_Test_Mode;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.u_GpsSbasSvId = p_Cmd->u_GPS_L1_SBAS_SV_ID;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.l_GloFreqId = p_Cmd->l_GLO_G1_Freq_ID;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.u_BdsSvId = p_Cmd->u_BDS_B1_SV_ID;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.u_GalSvId = p_Cmd->u_GAL_E1_SV_ID;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.e_RfDevTrackCmd = MultiChan_Track_Cmd_Start;
            /* Update GPS SV Mask */
            z_AssistDataInfo.e_GnssType = MGP_GNSS_TYPE_GPS;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.u_ClearFlag = FALSE;
            z_AssistDataInfo.u_Force = TRUE;

            (void)mgp_GnssUpdateInfo(&z_AssistDataInfo);

            /* Send a cmd to LM to turn Receiver ON */
            // mgp_ReceiverIdle(); /* Should not need this. Internally MGP should do an IDLE->ON. But doesnt work */
            // (void) lm_mgp_update_mgp_state(LM_MGP_STATE_ON,p_OnParams,FALSE);
            break;
          }

        case LM_RFDEV_TEST_STOP_CLEAR_ALL:
          {
            z_AssistDataInfo.e_AssistanceType = MGP_GNSS_ASSIST_DATA_RF_DEV_CMD_INFO;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.u_ClearFlag = TRUE;
            z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.e_RfDevTrackCmd = MultiChan_Track_Cmd_Stop;

            /* Clear existint RF Dev Cmd info from MC Database */
            (void)lm_mgp_update_info(&z_AssistDataInfo);

            /* Send a cmd to MGP to delete ALL */
            memset(&z_delete_info, 0, sizeof(z_delete_info));
            z_delete_info.delete_all = TRUE;
            z_delete_info.force_rcvr_off = FALSE;

            /* Deletes GNSS data */
            z_delete_info.z_mgp_info.e_GnssType = MGP_GNSS_TYPE_ALL;

            lm_mgp_handle_delete_mgp_info(&z_delete_info);
          }
          /*lint -e{616} Legacy behavior of code where case statements purposely fall over to next */
        case LM_RFDEV_TEST_STOP_NO_CLEAR: /* intentional cascade from previous case */
          /* Just in case this was enabled, reset it back to requiring Bit Edge
             Detection before entering Track */
          //mgp_DisableBitEdgeDetection(FALSE);
          mgp_GnssDisableBitEdgeDetection(MGP_GNSS_TYPE_GPS, 0, 0);
          mgp_GnssDisableBitEdgeDetection(MGP_GNSS_TYPE_GLO, 0, 0);
          mgp_GnssDisableBitEdgeDetection(MGP_GNSS_TYPE_BDS, 0, 0);

          /* Just in case DPO was forced, reset it back to default (un-forced) */
          mgp_ForceDpoOperation(FALSE);

          /* Send msg to MGP - MC to reset MC search strategy */

          z_AssistDataInfo.e_AssistanceType = MGP_GNSS_ASSIST_DATA_RF_DEV_CMD_INFO;
          z_AssistDataInfo.z_AssistData.z_RfDevCmdInfo.e_RfDevTrackCmd = MultiChan_Track_Cmd_Stop;
          (void)lm_mgp_update_info(&z_AssistDataInfo);

          //mgp_ReceiverForcedIdle();
          // mgp_ReceiverIdle();
          break;

        default:
          break;
      }
    }
    else
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,
          "lm_RfDevTestCmd_handler: p_Cmd is NULL");
    }
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,
        "lm_RfDevTestCmd_handler: p_msg is NULL");
  }
}

/*
 ******************************************************************************
 * lm_MemBwTestCmd_handler
 *
 * Function description:
 *
 *  lm_MemBwTestCmd_handler simply transfers IPC payload to mem-BW struct
 *  injected into MGP.
 *
 * Parameters:
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/
void lm_MemBwTestCmd_handler(os_IpcMsgType *p_msg)
{
  lm_mem_bw_test_type *p_Cmd;
  mgp_MemBwTestParams z_MemBwTestParams;

  if (p_msg == NULL)
  {
    LM_MSG_ERROR("Received NULL mem-BW-test cmd from TM diag", 0, 0, 0);
    return;
  }
  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  p_Cmd = (lm_mem_bw_test_type *)p_msg->p_Data;
  z_MemBwTestParams.u_TestStopFlag = p_Cmd->u_TestStopFlag;
  z_MemBwTestParams.u_TestType = p_Cmd->u_TestType;
  z_MemBwTestParams.u_TaskCnt = p_Cmd->u_TaskCnt;
  z_MemBwTestParams.w_IntegLength = p_Cmd->w_IntegLength;

  mgp_SendMemBwTestRequest(&z_MemBwTestParams);
}

/*
 ******************************************************************************
 * lm_nav_set_test_mode_handler
 *
 * Function description:
 * This function is used to do mandatory test mode set to NavRF driver.
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_nav_set_test_mode_handler(os_IpcMsgType *p_msg)
{
  uint8 *p_Cmd;

  if (p_msg == NULL)
  {
    LM_MSG_ERROR("Received NULL nav set test mode cmd from TM diag", 0, 0, 0);
    return;
  }

  p_Cmd = (uint8 *)p_msg->p_Data;
  (void)mgp_NavSetTestMode(*p_Cmd);
}

/*
 ******************************************************************************
 * lm_nav_set_test_mode_handler
 *
 * Function description:
 *  This function is used to run multiple GNSS RF Diag tests
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_nav_ioctl_handler(os_IpcMsgType *p_msg)
{
  lm_NavIoctlType *p_Cmd;
  mgp_NavIoctlType z_NavIoctl;

  if (p_msg == NULL)
  {
    LM_MSG_ERROR("Received NULL nav ioctl cmd from TM diag", 0, 0, 0);
    return;
  }

  p_Cmd = (lm_NavIoctlType *)p_msg->p_Data;

  z_NavIoctl.u_RequestCode = p_Cmd->u_RequestCode;
  z_NavIoctl.l_RequestParam1 = p_Cmd->l_RequestParam1;
  z_NavIoctl.l_RequestParam2 = p_Cmd->l_RequestParam2;

  mgp_NavIoctl(&z_NavIoctl);
}

/**
 * @brief lm_DiagBPJammerCtrlCmd is used to send the Jammer Meas 
 *        diag command to MGP.
 * 
 * 
 * @param p_Msg IPC data from TM.
 */
void lm_DiagBPJammerCtrlCmd(os_IpcMsgType *p_Msg)
{
  lm_NavBpJammerCtlType *p_Cmd;
  mgp_NavBPJammerCtlType z_NavBPJammerCtl;

  if (p_Msg == NULL)
  {
    LM_MSG_ERROR_0("LM received NULL Jammer meas cmd from TM diag");
    return;
  }
  p_Cmd = (lm_NavBpJammerCtlType *)p_Msg->p_Data;
  z_NavBPJammerCtl.u_RequestCode = p_Cmd->u_RequestCode;
  mgp_BPJammerCtrlCmd(&z_NavBPJammerCtl);
}
/*
 ******************************************************************************
 * lm_nav_config_handler
 *
 * Function description:
 *  This function is used to send a NAV config change command through to MGP.
 *
 * Parameters: 
 *
 * *p_Msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void lm_nav_config_handler(os_IpcMsgType *p_Msg)
{
  lm_NavConfigReqType *p_Cmd;
  mgp_NavConfigType z_NavConfig;

  if (p_Msg == NULL)
  {
    LM_MSG_ERROR_0("Received NULL nav config cmd from TM diag");
  }
  else
  {
    p_Cmd = (lm_NavConfigReqType *)p_Msg->p_Data;
    z_NavConfig.u_GnssDesiredConfig = p_Cmd->u_GnssDesiredConfig;
    mgp_NavConfig(&z_NavConfig);
  }
}

#ifdef FEATURE_GNSS_LTE_CATM_COEX
/*
 ******************************************************************************
 * lm_CatmLteTxStartCmd_handler
 *
 * Function description:
 *
 *  lm_CatmLteTxStartCmd_handler simply transfers IPC payload to z_CatMTxInd struct
 *  injected into MGP.
 *
 * Parameters:
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/

void lm_CatmLteTxStartCmd_handler(os_IpcMsgType *p_msg)
{
  lm_CatmLteTxStartTestParams *p_Cmd;
  mgp_CatmLteTxStartTestParams z_CatmLteTxStartTestParams;
  if (p_msg == NULL)
  {
    LM_MSG_ERROR("Received NULL CATM LTE Tx start cmd from TM diag", 0, 0, 0);
    return;
  }
  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  p_Cmd = (lm_CatmLteTxStartTestParams *)p_msg->p_Data;
  if (p_Cmd == NULL)
  {
    LM_MSG_ERROR("Received NULL CATM LTE Tx start msg pointer", 0, 0, 0);
    return;
  }
  LM_MSG_MED("GNSSCatM -lm_CatmLteTxStartCmd_handler Duration: %d, Bandwidth: %d, SeqCnt: %d", p_Cmd->q_Duration, p_Cmd->q_Bandwidth, p_Cmd->w_SeqCnt);
  LM_MSG_MED("GNSSCatM -lm_CatmLteTxStartCmd_handler TimeTickMs: %d, Band: %d, Power: %d", p_Cmd->q_TimeTickMs, p_Cmd->q_Band, p_Cmd->q_Power);
  z_CatmLteTxStartTestParams.q_Band = p_Cmd->q_Band;
  z_CatmLteTxStartTestParams.q_Bandwidth = p_Cmd->q_Bandwidth;
  z_CatmLteTxStartTestParams.q_Duration = p_Cmd->q_Duration;
  z_CatmLteTxStartTestParams.q_Power = p_Cmd->q_Power;
  z_CatmLteTxStartTestParams.q_TimeTickMs = (p_Cmd->q_TimeTickMs * 32.768) + mgp_TimeTickGet();
  z_CatmLteTxStartTestParams.w_SeqCnt = p_Cmd->w_SeqCnt;
  mgp_SendCatmLteTxStartCmd(&z_CatmLteTxStartTestParams);
  LM_MSG_MED("GNSSCatM -lm_CatmLteTxStartCmd_handler q_TimeTickMs: %d, mgp_TimeTickGet: %d", z_CatmLteTxStartTestParams.q_TimeTickMs, mgp_TimeTickGet(), 0);
}



/*
 ******************************************************************************
 * lm_CatmLteTxEndCmd_handler
 *
 * Function description:
 *
 *  lm_CatmLteTxEndCmd_handler simply transfers IPC payload to z_CatMTxInd struct
 *  injected into MGP.
 *
 * Parameters:
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/
void lm_CatmLteTxEndCmd_handler(os_IpcMsgType *p_msg)
{
  lm_CatmLteTxEndTestParams *p_Cmd;
  mgp_CatmLteTxEndTestParams z_CatmLteTxEndTestParams;
  if (p_msg == NULL)
  {
    LM_MSG_ERROR("GNSSCatM -Received NULL CATM LTE Tx End cmd from TM diag", 0, 0, 0);
    return;
  }
  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  p_Cmd = (lm_CatmLteTxEndTestParams *)p_msg->p_Data;
  if (p_Cmd == NULL)
  {
    LM_MSG_ERROR("Received NULL CATM LTE Tx End msg pointer", 0, 0, 0);
    return;
  }
  LM_MSG_MED("GNSSCatM -lm_CatmLteTxEndCmd_handler: TimeTicK: %d , SeqCnt: %d", p_Cmd->q_TimeTickMs, p_Cmd->w_SeqCnt, 0);
  z_CatmLteTxEndTestParams.q_TimeTickMs = p_Cmd->q_TimeTickMs;
  z_CatmLteTxEndTestParams.w_SeqCnt = p_Cmd->w_SeqCnt;
  mgp_SendCatmLteTxEndCmd(&z_CatmLteTxEndTestParams);
}
#endif /*FEATURE_GNSS_LTE_CATM_COEX*/

/*
 ******************************************************************************
 * lm_MemBwTestCmd_handler
 *
 * Function description:
 *
 *  lm_MemBwTestCmd_handler simply transfers IPC payload to mem-BW struct
 *  injected into MGP.
 *
 * Parameters:
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/
void lm_SvNoExistCmd_handler(const os_IpcMsgType *p_msg)
{
  const lm_sv_no_exist_cmd_type *cpz_SvNoExistCmd;
  gnss_AssistDataInfoStructType z_AssistData;

  if (p_msg == NULL)
  {
    LM_MSG_ERROR("Received NULL Sv No Exist cmd from TM diag", 0, 0, 0);
    return;
  }

  cpz_SvNoExistCmd = (const lm_sv_no_exist_cmd_type *)p_msg->p_Data;

  z_AssistData.e_AssistanceType = MGP_GNSS_ASSIST_DATA_SV_NO_EXIST;
  z_AssistData.z_AssistData.z_SvNoExistInfo.q_SvNoExistMask
     = cpz_SvNoExistCmd->q_SvNoExistMask;
  z_AssistData.u_Force = TRUE;
  z_AssistData.e_GnssType = MGP_GNSS_TYPE_GPS;
  (void)lm_mgp_update_info(&z_AssistData);
}

/*
 ******************************************************************************
 * lm_ForceDpoForPowerMeasCmd_handler
 *
 * Function description:
 *
 *  lm_ForceDpoForPowerMeasCmd_handler simply transfers IPC payload to inject the
 *  DPO Force command into MGP 
 *  injected into MGP.
 *
 * Parameters: 
 *
 * *p_msg - IPC message from TM diag
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void lm_ForceDpoForPowerMeasCmd_handler(const os_IpcMsgType *p_msg)
{
  const lm_force_dpo_for_power_meas_cmd_type *cpz_ForceDpoForPowerMeasCmd;

  if (!p_msg)
    return;

  cpz_ForceDpoForPowerMeasCmd = (const lm_force_dpo_for_power_meas_cmd_type *)p_msg->p_Data;

  mgp_ForceDpoForPowerMeas(cpz_ForceDpoForPowerMeasCmd->u_ForceDpoForPowerMeas);
}

/*
 ******************************************************************************
 * lm_ext_mode_switch
 *
 * Function description:
 *
 *  Mode-switch requests coming from non-GPS tasks (through cgps_ModeSwitch()) will
 *  flow into this function. This will run in the client task's context.
 *
 * Parameters:
 *
 *  e_new_state - Desired MGP state.
 *  p_cb        - Pointer to CB function.
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/
void lm_ext_mode_switch(cgps_sarf_mode_switch_enum_type e_new_state, cgps_mode_switch_callback p_cb)
{

  if (e_new_state >= CGPS_SARF_MODE_MAX)
  {
    if(p_cb != NULL)
    {
    p_cb(FALSE);
    }
    return;
  }

  z_lm_sarf_state_db.p_client_cb = p_cb;

  if (e_new_state == CGPS_ENTER_SARF_MODE && z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_ON)
  {
    if(p_cb != NULL)
    {
    p_cb(TRUE);
    }
    event_report(EVENT_GPS_LM_ENTER_SA_RF_VERIF);
    lm_mgp_report_SARF_status(TRUE);
  }
  else if (e_new_state == CGPS_EXIT_SARF_MODE && z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_OFF)
  {
    if(p_cb != NULL)
    {
    p_cb(TRUE);
    }
    event_report(EVENT_GPS_LM_EXIT_SA_RF_VERIF);
    lm_mgp_report_SARF_status(FALSE);
  }
  else
    mode_switch(e_new_state);
}

/*
 ******************************************************************************
 * Function lm_IsMgpInSarfMode
 *
 * Description:
 *
 *  Return TRUE if MGP is in SARF mode, else FALSE.
 *
 * Parameters:
 *
 * Return value:
 *
 *  TRUE  - MGP is in SARF mode
 *  FALSE - If MGP is NOT in SARF mode
 *
 ******************************************************************************
*/
boolean lm_IsMgpInSarfMode(void)
{
  if (z_lm_sarf_state_db.e_sarf_state == LM_SARF_MODE_ON)
    return TRUE;
  else
    return FALSE;
}



/*
 ******************************************************************************
 * Function lm_DiagSbasStart
 *
 * Description:
 *
 * Starts SBAS mode
 *
 * Parameters:
 *
 * Return value:
 *
 *
 ******************************************************************************
*/
void lm_DiagSbasStart(mgp_ConfigStructType *p_MgpConfig)
{
  z_lmControl.u_MgpONParamsValid = TRUE;

  (void)lm_mgp_update_mgp_state(LM_MGP_STATE_ON, p_MgpConfig, LM_UPDATE_MGP_STATE_FLAG_NONE);
}



/*
 ******************************************************************************
 * Function lm_DiagSbasStop
 *
 * Description:
 *
 * Stops SBAS mode
 *
 * Parameters:
 *
 * Return value:
 *
 *
 ******************************************************************************
*/
void lm_DiagSbasStop(void)
{
  (void)lm_mgp_update_mgp_state(LM_MGP_STATE_OFF, NULL, LM_UPDATE_MGP_STATE_FLAG_NONE);
}

/*
 ******************************************************************************
 * Function lm_DiagTsgCmd
 *
 * Description:
 *
 *  Sets TSG configuration.
 *
 * Parameters:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/
void lm_DiagTsgCmd(os_IpcMsgType *p_msg)
{
  lm_TsgCmdType *p_TsgCmd;
  mgp_TsgConfig z_TsgConfig;
  uint8 i = 0;

  if (p_msg == NULL)
  {
    LM_MSG_ERROR_0("LM received NULL Tsg Cmd from TM diag");
    return;
  }

  p_TsgCmd = (lm_TsgCmdType *)p_msg->p_Data;
  if (p_TsgCmd->u_Version != 2)
  {
    LM_MSG_ERROR_0("TSG command version deprecated, only version 2 supported");
    return;
  }

  for (i = TSG_CH1; i < TSG_NUM_CHANS; i++)
  {
    z_TsgConfig.z_TsgChanConfig[i].u_Mode = p_TsgCmd->z_TsgChanConfig[i].u_Mode;
    z_TsgConfig.z_TsgChanConfig[i].b_SvId = p_TsgCmd->z_TsgChanConfig[i].b_SvId;
    z_TsgConfig.z_TsgChanConfig[i].u_GnssType = p_TsgCmd->z_TsgChanConfig[i].u_GnssType;
    z_TsgConfig.z_TsgChanConfig[i].l_DoppHz = p_TsgCmd->z_TsgChanConfig[i].l_DoppHz;
    z_TsgConfig.z_TsgChanConfig[i].b_CodeDoppQuartCps = p_TsgCmd->z_TsgChanConfig[i].b_CodeDoppQuartCps;
    z_TsgConfig.z_TsgChanConfig[i].w_CodePhaseChips = p_TsgCmd->z_TsgChanConfig[i].w_CodePhaseChips;
    z_TsgConfig.z_TsgChanConfig[i].u_DataBitsConfig = p_TsgCmd->z_TsgChanConfig[i].u_DataBitsConfig;
    z_TsgConfig.z_TsgChanConfig[i].u_SNR = p_TsgCmd->z_TsgChanConfig[i].u_SNR;
    LM_MSG_HIGH_3("TSG Mode %u, GNSS %u, Doppler %u", z_TsgConfig.z_TsgChanConfig[i].u_Mode,
                  z_TsgConfig.z_TsgChanConfig[i].u_GnssType,
                  z_TsgConfig.z_TsgChanConfig[i].l_DoppHz);
  }
  mgp_SendTsgConfigRequest(&z_TsgConfig);
}

/*
 ******************************************************************************
 * Function lm_DiagGnssRfCmd
 *
 * Description:
 *
 *  Sets RF configuration and GNSS core registers depending on the RF config.
 *
 * Parameters:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/
void lm_DiagGnssRfCmd(os_IpcMsgType *p_msg)
{
  lm_RfCmdType *p_RfCmd;
  mgp_DiagRfCmd z_DiagRfCmd;

  if (p_msg == NULL)
  {
    LM_MSG_ERROR("LM received NULL GNSS RF Cmd from TM diag", 0, 0, 0);
    return;
  }

  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  p_RfCmd = (lm_RfCmdType *)p_msg->p_Data;

  z_DiagRfCmd.u_RfMode = p_RfCmd->u_RfMode;
  z_DiagRfCmd.u_RfLinearity = p_RfCmd->u_RfLinearity;
  z_DiagRfCmd.u_RfRxdOnTimeSec = p_RfCmd->u_RxdOnTimeSec;
  z_DiagRfCmd.u_RfRxdOffTimeSec = p_RfCmd->u_RxdOffTimeSec;
  z_DiagRfCmd.u_AdcConfig = p_RfCmd->u_AdcConfig;
  mgp_SendDiagRfCmd(&z_DiagRfCmd);

  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  p_RfCmd = (lm_RfCmdType *)p_msg->p_Data;

  LM_MSG_HIGH("RF cmd params %u %u %u", p_RfCmd->u_Version, p_RfCmd->u_RfMode, p_RfCmd->u_RfLinearity);
}

/*
 ******************************************************************************
 * Function lm_DiagGnssPrxRfCmd
 *
 * Description:
 *
 *  Sets Prx RF configuration and GNSS core registers depending on the Prx RF config.
 *
 * Parameters:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/
void lm_DiagGnssPrxRfCmd(os_IpcMsgType *p_msg)
{
  lm_PrxRfCmdType *p_PrxRfCmd;
  mgp_DiagPrxRfCmd z_DiagPrxRfCmd;

  if (p_msg == NULL)
  {
    LM_MSG_ERROR("LM received NULL GNSS Prx RF Cmd from TM diag", 0, 0, 0);
    return;
  }

  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  p_PrxRfCmd = (lm_PrxRfCmdType *)p_msg->p_Data;

  z_DiagPrxRfCmd.u_RfMode = p_PrxRfCmd->u_RfMode;
  mgp_SendDiagPrxRfCmd(&z_DiagPrxRfCmd);

  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  p_PrxRfCmd = (lm_PrxRfCmdType *)p_msg->p_Data;

  LM_MSG_HIGH("Prx RF cmd params %u %u", p_PrxRfCmd->u_Version, p_PrxRfCmd->u_RfMode, 0);
}

/*
 ******************************************************************************
 * Function lm_DiagGnssRfLinearityCmd
 *
 * Description:
 *
 *  Sets RF Linearity configuration.
 *
 * Parameters:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/
void lm_DiagGnssRfLinearityCmd(os_IpcMsgType *p_msg)
{
  lm_RfLinearityCmdType *p_RfLinearityCmd;
  mgp_DiagRfLinearityCmd z_DiagRfLinearityCmd;

  if (p_msg == NULL)
  {
    LM_MSG_ERROR("LM received NULL RF Linearity Cmd from TM diag", 0, 0, 0);
    return;
  }

  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  p_RfLinearityCmd = (lm_RfLinearityCmdType *)p_msg->p_Data;

  z_DiagRfLinearityCmd.u_RfLinearity = p_RfLinearityCmd->u_RfLinearity;
  mgp_SendDiagRfLinearityCmd(&z_DiagRfLinearityCmd);

  /*lint -e{826} : Suspicious pointer-to-pointer conversion (area too small) */
  p_RfLinearityCmd = (lm_RfLinearityCmdType *)p_msg->p_Data;

  LM_MSG_HIGH("RF Linearity cmd params %u %u", p_RfLinearityCmd->u_Version, p_RfLinearityCmd->u_RfLinearity, 0);
}




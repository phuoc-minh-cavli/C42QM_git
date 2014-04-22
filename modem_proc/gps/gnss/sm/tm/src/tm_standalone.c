/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM Standalone Sub-module

General Description
  This file contains implementations for TM Diag Interface

  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2020 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History
  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/src/tm_standalone.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/08/20   ska     Smart compilation issues
11/28/16   mj      Set UTC mask for leap second retrieval
10/19/15   rk      Continue with standalone session when the RAT isn't in full service to provide the AD. 
03/14/15   rh      Added support for MultiSIM NV 
12/06/13   ah      Support for non-deferrable timers
06/11/13   mj      Decouple calling qwip through standalone protocol
09/30/09   gk      Support ODP 2.0
07/30/09   gk      ODP 2.0 support
07/25/08   gk      session cancellation
07/09/09   gk      ODP 1.1 related fixes for warmup
04/02/09   ns      Send fix_timeout to sysd
01/25/09   ns      Added support for QWip
04/25/08   gk      On-Demand minor tweaks
06/09/08   gk      On-Demand Changes to add some logs and events
04/25/08   gk      On-Demand Change
08/01/07   rw      Add support for XTRA feature
07/30/07   ank     Added support for ext_pos_data in Position Report.

05/27/07   rw      Add support for JGPS (KDDI) module
05/09/07   ank     Changed prototype of tm_util_pd_info_data_init
10/16/06   cl      Initial Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "math.h"
#include "msg.h"
#include "aries_os_api.h"
#include "tm_common.h"
#include "tm_prtl_iface.h"
#include "sm_api.h"
#include "pdapibuf.h"
#include "tm_cm_iface.h"
#include "sm_log.h"
#include "tm_data.h"

#ifdef FEATURE_CGPS_XTRA
#include "tm_xtra.h"
#endif /* FEATURE_CGPS_XTRA */

#include "tm_standalone.h"
#include "tm_common.h"
extern pdsm_lite_pd_event_type tm_util_xlate_fixt_report_to_qmi_loc_pos_rpt_Ind(sm_FixInfoStructType *lr_data,
                                                        qmiLocEventPositionReportIndMsgT_v02* pz_PositionReportIndMsg , 
                                                        prtl_sess_info_e_type orig_sess_info_type );

tm_standalone_sess_info_s_type tm_standalone_sess_info;
pdsm_lite_pd_info_s_type            tm_standalone_info_buffer;


/* Extra time (in seconds) when start LR timer */
#define TM_STANDALONE_TIMEOUT_EXTRA  2


/*===========================================================================

FUNCTION tm_standalone_sess_termination

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_standalone_sess_termination(tm_sess_stop_reason_e_type reason)
{
  tm_sess_req_param_u_type   sessReqParam;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Standalone session termination reason %d", reason, 0, 0);
  sessReqParam.stop_param.stop_type   = TM_STOP_TYPE_SESSION;
  sessReqParam.stop_param.stop_reason = reason;

  /* Terminate session to TM-Core */
  (void)tm_sess_req(TM_PRTL_TYPE_STANDALONE,
                    tm_standalone_sess_info.sess_handle,
                    TM_SESS_REQ_STOP,
                    &sessReqParam);

  (void)os_TimerStop(tm_standalone_sess_info.sess_timer_ptr);

  tm_standalone_sess_info.sess_handle   = 0;
  tm_standalone_sess_info.session_state = TM_STANDALONE_SESS_STATE_INIT;

}

#define  tm_standalone_convert_op_mode       TM_SESS_OPERATION_MODE_STANDALONE

/*===========================================================================

FUNCTION tm_standalone_start_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  prtl_start_sess_req_result_e_type

SIDE EFFECTS
  None.

===========================================================================*/
prtl_start_sess_req_result_e_type tm_standalone_start_sess_req_handler
(
   tm_sess_handle_type               sess_handle,
   prtl_start_sess_req_param_s_type  *start_sess_req_param,
   tm_sess_req_start_param_s_type    *action_ptr
   )
{
  if ((start_sess_req_param == NULL) || (action_ptr == NULL))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null start_sess_req_param or action_ptr", 0, 0, 0);
    return PRTL_START_SESS_REQ_NOT_ALLOWED;
  }

  /* Update Standalone Session Info */
  tm_standalone_sess_info.sess_handle   = sess_handle;
  tm_standalone_sess_info.session_state = TM_STANDALONE_SESS_STATE_WAIT_WISHLIST_UPDATE;

  tm_standalone_sess_info.client_type                 = start_sess_req_param->client_type;
  tm_standalone_sess_info.client_id                   = start_sess_req_param->client_id;
  tm_standalone_sess_info.op_mode                     = tm_standalone_convert_op_mode;
  tm_standalone_sess_info.fix_rate.num_fixes          = start_sess_req_param->pd_option.fix_rate.num_fixes;
  tm_standalone_sess_info.fix_rate.time_between_fixes_ms =
     start_sess_req_param->pd_option.fix_rate.time_between_fixes_ms;
  tm_standalone_sess_info.qos.accuracy_threshold      = start_sess_req_param->pd_qos.accuracy_threshold;
  tm_standalone_sess_info.qos.gps_session_timeout     = start_sess_req_param->pd_qos.gps_session_timeout;

  /* Instruct TM-Core to kick of sessions */
  action_ptr->op_mode   = tm_standalone_sess_info.op_mode;
  action_ptr->num_fixes = tm_standalone_sess_info.fix_rate.num_fixes;
  action_ptr->tbf_ms    = tm_standalone_sess_info.fix_rate.time_between_fixes_ms;
  action_ptr->op_req    = TM_OP_REQ_LR;
  action_ptr->abort_curr_sess = FALSE;
  action_ptr->lr_qos     = tm_standalone_sess_info.qos.gps_session_timeout;
  action_ptr->accuracy_threshold = tm_standalone_sess_info.qos.accuracy_threshold;
  action_ptr->dynamic_qos_enabled = TRUE;
  return PRTL_START_SESS_REQ_ALLOWED;
} /*lint !e818 */

/*===========================================================================

FUNCTION tm_standalone_sess_req_data_handler

DESCRIPTION   TODO [an] kept this func as prtl_init uses fp*

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_standalone_sess_req_data_handler()
{
    return TRUE; /*lint !e506 */
}



/*===========================================================================

FUNCTION tm_standalone_handle_lr_data

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_standalone_handle_lr_data(sm_FixInfoStructType *lr_data, prtl_sess_info_e_type orig_sess_info_type)
{
  pdsm_lite_pd_event_type pd_event;
  utc_time z_utc_time;
  boolean time_valid = FALSE;
  uint8 u_leap_sec = 0;
  qmiLocEventPositionReportIndMsgT_v02 *pz_PositionReportIndMsg;

  memset(&tm_standalone_info_buffer.pd_info.pz_PositionReportIndMsg, 0 , sizeof(tm_standalone_info_buffer.pd_info.pz_PositionReportIndMsg));
  pz_PositionReportIndMsg = &tm_standalone_info_buffer.pd_info.pz_PositionReportIndMsg;
   

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Got LR", 0, 0, 0);

/*  lr_resp_ptr = (tm_lr_resp_type *)pdsm_getbuf(sizeof(tm_lr_resp_type)); 

  if (lr_resp_ptr != NULL)
  {
*/
    /* Generate CGPS event */
    sm_log_event_report(SM_LOG_EVENT_ID_GPS_COMPLETE, 0);

    //Convert from LM to IS801 format
  /*  if (tm_util_convert_lr(lr_data, lr_resp_ptr))
    {
    */
     tm_standalone_info_buffer.client_id = tm_standalone_sess_info.client_id;
    
     pd_event = tm_util_xlate_fixt_report_to_qmi_loc_pos_rpt_Ind(lr_data, pz_PositionReportIndMsg , 
                      orig_sess_info_type);

      /* Query SMOD iface to determine if 1x UP can post event to TM-Core */
      {
        /* Fire Postion PD API event */
        (void)tm_core_recv_event(pd_event,
                                 &tm_standalone_info_buffer,
                                 lr_data);
      }
  /* Clean up standalone module when the reported position is final fix */
  
 if(PRTL_SESS_INFO_LR == orig_sess_info_type)
 {
  tm_standalone_sess_termination(TM_STOP_REASON_COMPLETED);
 }
}









/*===========================================================================

FUNCTION tm_standalone_sess_info_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_standalone_sess_info_handler
(
   tm_sess_handle_type                session_handle,
   prtl_sess_info_e_type              sess_info_type,
   prtl_sess_info_param_u_type        *sess_info_param
   )
{
  if (sess_info_param == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Null sess_info_param pointer", 0, 0, 0);
    return FALSE;
  }

  if (session_handle == tm_standalone_sess_info.sess_handle)
  {
    switch (sess_info_type)
    {
      case PRTL_SESS_INFO_STATUS:
        break;

      case PRTL_SESS_INFO_LR:
        tm_standalone_handle_lr_data(&sess_info_param->lr_data, sess_info_type);
        break;

      default:
        break;
    }
    return TRUE; /*lint !e506 */
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Incorrect sess_handle", 0, 0, 0);
    return FALSE;
  }
}


/*===========================================================================

FUNCTION tm_standalone_stop_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_standalone_stop_sess_req_handler
(
   tm_sess_handle_type           sess_handle,
   prtl_sess_stop_type           stop_type,
   tm_sess_stop_reason_e_type    stop_reason
   )
{
  boolean ret_val = FALSE;
  prtl_sess_info_param_u_type  *sess_info_param_ptr = NULL;

  (void)stop_type;
  if (sess_handle == TM_SESS_HANDLE_NONE)
  {
    return TRUE;
  }

  if (sess_handle == tm_standalone_sess_info.sess_handle)
  {
    tm_standalone_sess_termination(stop_reason);
    ret_val = TRUE;
  }
  else
  {

    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Can not match session handle between tm-core = %d, standalone = %d",
          sess_handle,
          tm_standalone_sess_info.sess_handle,
          0);
    
    ret_val = FALSE;

  }
  return (ret_val);
}

/*===========================================================================

FUNCTION tm_standalone_timer_cb_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_standalone_timer_cb_handler
(
   void *timer_data
   )
{
  os_TimerExpiryType *timer_param = (os_TimerExpiryType *)timer_data;

  if (timer_param != NULL)
  {
    switch (timer_param->q_TimerId)
    {
      case TM_STANALONE_TIMER_ID_SESSION:
        tm_standalone_sess_termination(TM_STOP_REASON_GENERAL_ERROR);
        break;

      default:
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Uknown timer expired: %lu", timer_param->q_TimerId, 0, 0);
        break;
    }
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Can not proess null timer msg", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION tm_standalone_event_cb_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_standalone_event_cb_handler
(
   prtl_event_e_type  event_type,
   prtl_event_u_type  *event_payload_ptr
   )
{

  /* this function is here only for the sake of code symmetricity,
     no event call back is currently needed for the standalone module */

  /* check for null payload ptr */
  if (event_payload_ptr == NULL)
  {
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION tm_standalone_prtl_init

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_standalone_prtl_init(void)
{
  tm_prtl_cb_s_type prtlIfaceTbl;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_standalone_prtl_init", 0, 0, 0);

  /* Create Timers */
  tm_standalone_sess_info.sess_timer_ptr = os_TimerCreateInCgpsTimerGroup((uint32)TM_STANALONE_TIMER_ID_SESSION,
                                                                          (uint32)THREAD_ID_SM_TM);

  /* Initialize function callback table for TM-Core */
  prtlIfaceTbl.start_sess_req_fp   = tm_standalone_start_sess_req_handler;
  prtlIfaceTbl.stop_sess_req_fp    = tm_standalone_stop_sess_req_handler;
  prtlIfaceTbl.sess_req_data_fp    = tm_standalone_sess_req_data_handler;
  prtlIfaceTbl.sess_info_fp        = tm_standalone_sess_info_handler;
  prtlIfaceTbl.timer_cb_fp         = tm_standalone_timer_cb_handler;
  prtlIfaceTbl.event_cb_fp         = tm_standalone_event_cb_handler;
  prtlIfaceTbl.gm_event_handle_fp  = NULL;

  (void)tm_prtl_reg(TM_PRTL_TYPE_STANDALONE, &prtlIfaceTbl);
}

/*===========================================================================

FUNCTION tm_standalone_get_mcc_for_serving_cell

DESCRIPTION This function provides the mcc of current serving cell

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void tm_standalone_get_mcc_for_serving_cell(uint8 mcc[])
{
   /*This is place holder to get the mcc for serving cell. Right now 
     standalone doesnot have provision to store cell data. But for 
     9x05 this provision needs to be added.
   */
}


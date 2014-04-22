/*!
  @file gsndcp_static_api.c

  @brief
  External static API implementation for GLLC task.

  @detail

*/

/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gsndcp/static/gsndcp_static_api.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
24/10/17   ns      Initial Revision

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "sys.h"
#include "geran_dual_sim.h"
#include "gsndef.h"
#include "gsndcp_static_main.h"
#include "gsndcp_dyn_api.h"
#include "gprs_mem.h"
#include "assert.h"

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
#include "geran_dmm.h"
#endif

/*===========================================================================

                        DYNAMIC Loadable wrapper APIs

===========================================================================*/

gsmsn_mes_type* gsndcp_snsm_get_mes_buf(void)
{
  return(GPRS_MEM_MALLOC(sizeof(gsmsn_mes_type)));
}

void gsndcp_snsm_put_mes(gsmsn_mes_type* mes_ptr)
{
  GSN_ASSERT(sndcp_init != FALSE);

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  if (is_geran_loaded() == FALSE)
  {
    gsn_send_to_dyn_pendq(mes_ptr, SM_CMD);
  }
  else
  {
    DM_VOIDFUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, gsndcp_snsm_put_mes, mes_ptr);
  }
#else
  gsndcp_snsm_put_mes_dyn(mes_ptr);
#endif
}

void gsn_int_put_mes(gsn_internal_msg_t* mes_ptr)
{
  GSN_ASSERT(sndcp_init != FALSE);

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  if (is_geran_loaded() == FALSE)
  {
    gsn_send_to_dyn_pendq(mes_ptr, GSN_CMD);
  }
  else
  {
    DM_VOIDFUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, gsn_int_put_mes, mes_ptr);
  }
#else
  gsn_int_put_mes_dyn(mes_ptr);
#endif
}

uint16 gsndcp_snsm_seq_ind
(
  uint32                 temp_tlli,           /* Temporary Logical Link Identifier*/
  uint8                  nsapi,               /* NSAP Identifier Address          */
  uint8                  sgsn_rcv_npdu_num,   /* Receive N-PDU number             */
  sys_modem_as_id_e_type as_id /* Access Stratum ID                */
)
{
  gsmsn_mes_type *msg_ptr;
  uint16         rcv_npdu_num = 0;
  uint8          int_nsapi;

  /* Check if valid NSAPI */
  if (GSN_VALID_NSAPI(nsapi))
  {
    int_nsapi = (uint8)GSN_INTERN_NSAPI(nsapi);

    if (actv_nsapis[int_nsapi] == NULL)
    {
      MSG_GERAN_ERROR_1("GSN ""GSN: INACTIVE NSAPI %d",nsapi);
    }
    else
    {
      /* Send an GSNSM_SEQUENCE_IND message to SNDCP for normal processing. */
      msg_ptr = gsndcp_snsm_get_mes_buf();

      if(msg_ptr)
      {
        msg_ptr -> mes.snsm_seq_ind.prim          = GSNSM_SEQUENCE_IND;
        msg_ptr -> mes.snsm_seq_ind.tlli          = temp_tlli;
        msg_ptr -> mes.snsm_seq_ind.nsapi         = nsapi;
        msg_ptr -> mes.snsm_seq_ind.rcv_npdu_num  = sgsn_rcv_npdu_num;

        gsndcp_snsm_put_mes(msg_ptr);

        /* Obtain local Receive NPDU Number for return to caller. */

        GSN_LOCK();

        if (actv_nsapis[int_nsapi]->op_mode == ACK)
          rcv_npdu_num = actv_nsapis[int_nsapi]->dle.ack_rcv_npdu;
        else
          rcv_npdu_num = actv_nsapis[int_nsapi]->dle.unack_rcv_npdu;

        GSN_UNLOCK();
      }
      else
      {
        MSG_GERAN_ERROR_1("GSN ""Buffer not available %d",nsapi);
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_1("GSN ""GSN: INVALID NSAPI %d",nsapi);
  }
  return (rcv_npdu_num);
} /* end of gsndcp_snsm_seq_ind() */

gsn_internal_msg_t *gsn_int_get_mes_buf(void)
{
  return(GPRS_MEM_MALLOC(sizeof(gsn_internal_msg_t)));
}

void gsndcp_pdp_register_req
(
  uint8                   nsapi,
  wm_cb_type              pdp_ul_suspend_fnc_ptr,
  wm_cb_type              pdp_ul_resume_fnc_ptr,
  void                    (*pdp_dl_fnc_ptr)(void* ds_context, dsm_item_type **npdu),
  void                    *ds_context,
  boolean                 cipher,
  void                    (*pdp_reg_cnf_fnc_ptr)(void* ds_context, boolean success)
)
{
  gsn_internal_msg_t *msg_ptr;

  /* Obtain an internal message resource */

  msg_ptr = gsn_int_get_mes_buf();
  GSN_ASSERT(msg_ptr != NULL);

  /* Construct message and post to command queue for processing. */

  msg_ptr->cmd_id                                     = PDP_REG_REQ;
  msg_ptr->gsn_cmd.pdp_reg_req.nsapi                  = nsapi;
  msg_ptr->gsn_cmd.pdp_reg_req.pdp_ul_suspend_fnc_ptr = pdp_ul_suspend_fnc_ptr;
  msg_ptr->gsn_cmd.pdp_reg_req.pdp_ul_resume_fnc_ptr  = pdp_ul_resume_fnc_ptr;
  msg_ptr->gsn_cmd.pdp_reg_req.pdp_dl_fnc_ptr         = pdp_dl_fnc_ptr;
  msg_ptr->gsn_cmd.pdp_reg_req.ds_context             = ds_context;
  msg_ptr->gsn_cmd.pdp_reg_req.cipher                 = cipher;
  msg_ptr->gsn_cmd.pdp_reg_req.pdp_reg_cnf_fnc_ptr    = pdp_reg_cnf_fnc_ptr;

  gsn_int_put_mes(msg_ptr);
} /* end of gsndcp_pdp_register_req() */

/* Legacy PDP Registration Support */
boolean gsndcp_pdp_register
(
  uint8                   nsapi,
  wm_cb_type              pdp_ul_suspend_fnc_ptr,
  wm_cb_type              pdp_ul_resume_fnc_ptr,
  void                    (*pdp_dl_fnc_ptr)(void* ds_context, dsm_item_type **npdu),
  void                    *ds_context,
  boolean                 cipher,
  sys_modem_as_id_e_type  as_id
)
{
  gsndcp_pdp_register_req
  (
    nsapi,
    pdp_ul_suspend_fnc_ptr,
    pdp_ul_resume_fnc_ptr,
    pdp_dl_fnc_ptr,
    ds_context,
    cipher,
    NULL
 );

  return (TRUE);
} /* end of gsndcp_pdp_register() */

boolean gsndcp_to_sndcp_default_sig_hdlr
(
  comp_sig_enum_type sig,
  void *user_data_ptr
)
{
  NOTUSED(user_data_ptr);
  MSG_GERAN_ERROR_1("GSN ""PS->COMP sig %d, default hdlr called", sig);
  return TRUE;
} /* gsndcp_to_sndcp_default_sig_hdlr() */

boolean  gsndcp_register_wm_hdlr(const comp_cmd_type *cmd_ptr)
{
  uint8               nsapi;

  /* Make sure the right command was received */
  if (cmd_ptr->hdr.cmd_id != GSNDCP_REG_WM)
  {
    MSG_GERAN_ERROR_1("GSN ""cmd id is not REG_WM %d", cmd_ptr->hdr.cmd_id);
    // ASSERT(0);
    return FALSE;
  }

  /* Get the right NSAPI */
  nsapi = GSN_INTERN_NSAPI(cmd_ptr->cmd.gsndcp_reg_wm.nsapi);

  if (!GSN_VALID_NSAPI(cmd_ptr->cmd.gsndcp_reg_wm.nsapi))
  {
    MSG_GERAN_ERROR_1("GSN ""Invalid NSAPI %d", cmd_ptr->cmd.gsndcp_reg_wm.nsapi);
    return FALSE;
  }

  if (actv_nsapis[nsapi] == NULL)
  {
    MSG_GERAN_ERROR_1("GSN ""Inactive NSAPI %d", cmd_ptr->cmd.gsndcp_reg_wm.nsapi);
    return FALSE;
  }

  if((cmd_ptr->cmd.gsndcp_reg_wm.to_sndcp_wm_ptr == NULL) ||
     (cmd_ptr->cmd.gsndcp_reg_wm.from_sndcp_wm_ptr == NULL) ||
     (cmd_ptr->cmd.gsndcp_reg_wm.from_sndcp_post_rx_func_ptr == NULL))
  {
    MSG_GERAN_ERROR_0("GSN ""NULL ptr in wm reg");
    return FALSE;
  }

  /* Register both tx and rx watermarks and the callbacks with SNDCP */
  actv_nsapis[nsapi]->to_gsndcp_wm =
                             cmd_ptr->cmd.gsndcp_reg_wm.to_sndcp_wm_ptr;
  actv_nsapis[nsapi]->from_gsndcp_wm =
                            cmd_ptr->cmd.gsndcp_reg_wm.from_sndcp_wm_ptr;

  actv_nsapis[nsapi]->from_gsndcp_rx_func_parm =
                      cmd_ptr->cmd.gsndcp_reg_wm.from_sndcp_rx_func_parm;
  actv_nsapis[nsapi]->from_gsndcp_post_rx_func_ptr =
                      cmd_ptr->cmd.gsndcp_reg_wm.from_sndcp_post_rx_func_ptr;

  /* Include GAS_ID and NSAPI combination as input parameter for callback functions */
  actv_nsapis[nsapi]->to_gsndcp_wm->gone_empty_func_data   = (void*) ((uint32)nsapi);
  actv_nsapis[nsapi]->to_gsndcp_wm->gone_empty_func_ptr    =
                                         gsndcp_to_sndcp_gone_empty_cb;

  actv_nsapis[nsapi]->to_gsndcp_wm->each_enqueue_func_data = (void*) ((uint32)nsapi);
  actv_nsapis[nsapi]->to_gsndcp_wm->each_enqueue_func_ptr  =
                                         gsndcp_to_sndcp_each_enqueue_cb;


  /* Set the handler for the PS to SNDCP signal for the call. */
  (void)comp_set_sig_handler(
                actv_nsapis[nsapi]->to_gsndcp_sig,
                gsndcp_to_sndcp_sig_hdlr,
                (void *) ((uint32)nsapi));

  /* Check if data has already been enqueued on the UL WM, and if yes, process it. */

  if (!dsm_is_wm_empty(actv_nsapis[nsapi]->to_gsndcp_wm))
  {
    gsndcp_to_sndcp_each_enqueue_cb(actv_nsapis[nsapi]->to_gsndcp_wm, (void *) ((uint32)nsapi));
  }
  MSG_GERAN_HIGH_0("GSN ""SNDCP-SMD WM registered");

  return TRUE;
} /* gsndcp_register_wm_hdlr() */

void gsndcp_deregister_wm_hdlr(const comp_cmd_type *cmd_ptr)
{
  uint8    nsapi;

  /* Make sure the right command was received */
  if (cmd_ptr->hdr.cmd_id != GSNDCP_DEREG_WM)
  {
    MSG_GERAN_ERROR_1("GSN ""cmd id is not REG_WM %d", cmd_ptr->hdr.cmd_id);
    ASSERT(0);
    return;
  }

  /* Get the right NSAPI */
  nsapi = GSN_INTERN_NSAPI(cmd_ptr->cmd.gsndcp_dereg_wm.nsapi);
  MSG_MED("GSNDCP dereg wm, nsapi: %d", nsapi, 0, 0);

  /* If there was no active NSAPI, return immediately */
  if (!GSN_VALID_NSAPI(cmd_ptr->cmd.gsndcp_dereg_wm.nsapi))
  {
    MSG_GERAN_MED_1("GSN ""NSAPI %d is invalid ", cmd_ptr->cmd.gsndcp_dereg_wm.nsapi);
    return;
  }


  if (actv_nsapis[nsapi] == NULL)
  {
    MSG_GERAN_MED_1("GSN ""NSAPI %d is inactive ", cmd_ptr->cmd.gsndcp_dereg_wm.nsapi);
    return;
  }
  

  /* GSNDCP can be in down state when sndcp receives this, hence this check */
  if ((actv_nsapis[nsapi]->to_gsndcp_wm != NULL) &&
      (actv_nsapis[nsapi]->from_gsndcp_wm != NULL) &&
      (actv_nsapis[nsapi]->from_gsndcp_post_rx_func_ptr != NULL))
  {
    /* Reset both tx and rx wm to NULL.*/
    actv_nsapis[nsapi]->to_gsndcp_wm->each_enqueue_func_ptr = NULL;
    actv_nsapis[nsapi]->to_gsndcp_wm->gone_empty_func_ptr = NULL;
    actv_nsapis[nsapi]->to_gsndcp_wm = NULL;
    actv_nsapis[nsapi]->from_gsndcp_wm = NULL;
    actv_nsapis[nsapi]->from_gsndcp_post_rx_func_ptr = NULL;
  }

  /* Clear SMD->SNDCP signal handler. */
  (void)comp_set_sig_handler(
                  actv_nsapis[nsapi]->to_gsndcp_sig,
                  gsndcp_to_sndcp_default_sig_hdlr,
                  (void *) ((uint32)nsapi));

  MSG_GERAN_HIGH_0("GSN ""SNDCP-SMD WM deregistered");
} /* gsndcp_deregister_wm_hdlr() */

#ifndef FEATURE_GERAN_REDUCED_DEBUG
const gsndcp_diag_pdu_stat_rsp_t* gsndcp_diag_get_pdu_stats(uint8 nsapi_addr)
{
  const gsndcp_diag_pdu_stat_rsp_t* result = NULL;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, result, gsndcp_diag_get_pdu_stats, nsapi_addr);
#else
  result = gsndcp_diag_get_pdu_stats_dyn(nsapi_addr);
#endif
  return result;
}

void gsndcp_diag_reset_pdu_stats(uint8 nsapi_addr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, gsndcp_diag_reset_pdu_stats, nsapi_addr);
#else
  gsndcp_diag_reset_pdu_stats_dyn(nsapi_addr);
#endif
}
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

uint32 gsndcp_follow_on_req_enquiry(void)
{
  uint32 result = 0;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, result, gsndcp_follow_on_req_enquiry);
#else
  result = gsndcp_follow_on_req_enquiry_dyn();
#endif
  return result;
}

#ifdef FEATURE_DATA_MM
boolean gsndcp_process_cmd_queue(comp_sig_enum_type sig, void* pkt_instance)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  if (is_geran_loaded() == TRUE)
  {
    DM_FUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, result, gsndcp_process_cmd_queue, sig, pkt_instance);
  }
#else
  result = gsndcp_process_cmd_queue_dyn(sig, pkt_instance);
#endif
  return result;
}

boolean gsndcp_process_ll_dl_queue(comp_sig_enum_type sig, void* pkt_instance)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, result, gsndcp_process_ll_dl_queue, sig, pkt_instance);
#else
  result = gsndcp_process_ll_dl_queue_dyn(sig, pkt_instance);
#endif
  return result;
}

#else
boolean gsndcp_process_cmd_queue(void)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  if (is_geran_loaded() == TRUE)
  {
    DM_FUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, result, gsndcp_process_cmd_queue);
  }
#else
  result = gsndcp_process_cmd_queue_dyn();
#endif
  return result;
}

boolean gsndcp_process_ll_dl_queue(void)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, result, gsndcp_process_ll_dl_queue);
#else
  result = gsndcp_process_ll_dl_queue_dyn();
#endif
  return result;
}
#endif /* FEATURE_DATA_MM */

void gsndcp_to_sndcp_each_enqueue_cb(dsm_watermark_type* wm, void* callback_data)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, gsndcp_to_sndcp_each_enqueue_cb, wm, callback_data);
#else
  gsndcp_to_sndcp_each_enqueue_cb_dyn(wm, callback_data);
#endif
}

void gsndcp_to_sndcp_gone_empty_cb(dsm_watermark_type* wm, void* callback_data)
{
  uint8    nsapi;
  uint8    external_nsapi;

  /* To remove compiler warnings */
  uint32   rcvd_callback_data = (uint32)callback_data;

  /* Get the NSAPI */
  nsapi          = (uint8)(rcvd_callback_data & 0xFF);

  /* Get GAS_ID */

  /* Get external NSAPI */
  external_nsapi = GSN_EXTERN_NSAPI(nsapi);

  if (GSN_VALID_NSAPI(external_nsapi))
  {
    if (actv_nsapis[nsapi] != NULL)
    {
      /* Clear the TX DATA Signal for the nsapi */
      COMP_CLR_SIGNAL(actv_nsapis[nsapi]->to_gsndcp_sig);
    }
    else
    {
      MSG_GERAN_MED_1("GSN ""Invalid NSAPI %d", external_nsapi);
    }
  }
  else
  {
    MSG_GERAN_MED_1("GSN ""Invalid NSAPI %d", external_nsapi);
  }
} /* gsndcp_to_sndcp_gone_empty_cb() */

boolean gsndcp_to_sndcp_sig_hdlr(comp_sig_enum_type sig, void* user_data_ptr)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, result, gsndcp_to_sndcp_sig_hdlr, sig, user_data_ptr);
#else
  result = gsndcp_to_sndcp_sig_hdlr_dyn(sig, user_data_ptr);
#endif
  return result;
}

void gsn_ul_clr_est_susp(  uint8 sapi)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, gsn_ul_clr_est_susp, sapi);
#else
  gsn_ul_clr_est_susp_dyn( sapi);
#endif
}

void gsn_timer_callback(uint32 gasid_nsapi_and_timer_id)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, gsn_timer_callback, gasid_nsapi_and_timer_id);
#else
  gsn_timer_callback_dyn(gasid_nsapi_and_timer_id);
#endif
  return;
}

#ifndef FEATURE_GERAN_REDUCED_DEBUG
log_on_demand_status_enum_type sndcp_log_on_demand(log_code_type log_code)
{
  log_on_demand_status_enum_type result = LOG_ON_DEMAND_FAILED_ATTEMPT_S;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, result, sndcp_log_on_demand, log_code);
#else
  result = sndcp_log_on_demand_dyn(log_code);
#endif
  return result;
}

void sndcp_log_packet_request(uint16 log_code, uint8 zero_stats, uint8 additional_info)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GSN_INTF_ID, geran_static_gsn_ext_api_s, sndcp_log_packet_request, log_code, zero_stats, additional_info);
#else
  sndcp_log_packet_request_dyn(log_code, zero_stats, additional_info);
#endif
  return;
}
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */


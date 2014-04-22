/*****************************************************************************
***
*** TITLE
***
***  GPRS SNDCP LAYER
***
***
*** DESCRIPTION
***
***  Provides the SNDCP layer access functions to the
***  layer 2 control tasks, in the form of uplink and downlink
***  initialisation, and uplink and downlink event dispatch functions.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //components/rel/geran.mpss/5.2.0/gsndcp/src/gsndcp.c#14 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 27/11/13   ws        CR582513 GERAN API Unification for modem data services
*** 11/14/02   rc      Added multimode support.
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#define FEATURE_DSM_WM_CB

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

#include "comdef.h"
#include "gmutex.h"
#include "cfa.h"
#include "gs.h"
#include "ms.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "gllc_dyn_api.h"
#include "gprs_mem.h"
#include "gsndcp.h"
#include "gsndcpi.h"
#include "gsn_hdlr.h"
#include "gsni.h"
#include "gsndcp_v_g.h"
#include "dsm.h"
#include "string.h"
#include "time_svc.h"
#include "geran_dual_sim_g.h"
#include "assert.h"
#include "gsndcplog.h"
#include "gsndcp_static_main.h"
#include "gsndcp_dyn_api.h"

#if ((defined FEATURE_GSM_GPRS_SNDCP_PCOMP) || (defined FEATURE_GSM_GPRS_SNDCP_DCOMP))
#include "gsn_util.h"
#endif


/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

geran_pdu_priority_t gsndcp_ul_pdu_priority_hdlr(dsm_item_type *npdu);
void gsn_process_dyn_pendq(void);


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/
/* GLLC vtable to map static API fptrs with dynamic APIs */
geran_static_gsn_ext_api_s gsndcp_vtbl =
{
  gsndcp_snsm_put_mes_dyn,
  gsn_int_put_mes_dyn,
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  gsndcp_diag_get_pdu_stats_dyn,
  gsndcp_diag_reset_pdu_stats_dyn,
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
  gsndcp_follow_on_req_enquiry_dyn,
  gsndcp_process_cmd_queue_dyn,
  gsndcp_process_ll_dl_queue_dyn,
  gsndcp_to_sndcp_each_enqueue_cb_dyn,
  gsndcp_to_sndcp_sig_hdlr_dyn,
  gsn_ul_clr_est_susp_dyn,
  gsn_timer_callback_dyn,
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  sndcp_log_on_demand_dyn,
  sndcp_log_packet_request_dyn
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
};

/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_HIWATER()
===
===  DESCRIPTION
===
===    To be called when a high watermark has been reached in the SNDCP to LLC
===    watermark interface
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    PDP UL data flow on the specified NSAPI will be suspended
===
===========================================================================*/

void gsn_process_hiwater(gsn_sapi_t sapi, gsn_nsapi_mode_t  op_mode, uint8 susp_reason)
{

  gsn_nsapi_q_t     *item_ptr = NULL;
  gsn_sapi_e_t      *sapi_ptr = NULL;

  /* Lock/Protect this section. SNDCP could be in the process of deactivating
  ** PDP context and releasing actv_sapis[] and releasing queues/mutexes etc.
  ** while LLC calling this function.
  */
  GSN_LOCK();

  if (actv_sapis[sapi] != NULL)
  {
    sapi_ptr = actv_sapis[sapi];

    if (sapi_ptr->is_nsapi_ptr_q_valid)
    {

      item_ptr = (gsn_nsapi_q_t*)q_check(&(sapi_ptr->nsapi_ptr_q));

      if (item_ptr)
      {
        while(item_ptr != NULL)
        {
          if (item_ptr->nsapi_ptr->op_mode == op_mode)
          {
            /* Suspend UL data flow on the NSAPI */
            gsn_set_suspend_event(
                                item_ptr->nsapi_ptr->nsapi,
                                susp_reason);
          }
          /* Get next NSAPI from the list */
          item_ptr = (gsn_nsapi_q_t*)q_next(&(sapi_ptr->nsapi_ptr_q),
                                             &item_ptr->link);
        }
      }
      else
      {
        gsn_debug_print_invalid_sapi(sapi, GSN_GSNDCP, __LINE__);
      }
    }
    else
    {
      gsn_debug_print_invalid_sapi(sapi, GSN_GSNDCP, __LINE__);
    }
  }
  else
  {
    gsn_debug_print_invalid_sapi(sapi, GSN_GSNDCP, __LINE__);
  }

  GSN_UNLOCK();
}

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LOWATER()
===
===  DESCRIPTION
===
===    To be called when a low watermark has been reached in the SNDCP to LLC
===    watermark interface
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===   PDP UL data flow on the specified NSAPI might be resumed
===
===========================================================================*/

void gsn_process_lowater(gsn_sapi_t sapi, gsn_nsapi_mode_t op_mode, uint8 susp_reason)
{

  gsn_nsapi_q_t     *item_ptr = NULL;
  gsn_sapi_e_t      *sapi_ptr = NULL;

  /* Lock/Protect this section. SNDCP could be in the process of deactivating
  ** PDP context and releasing actv_sapis[] and releasing queues/mutexes etc.
  ** while LLC calling this function.
  */
  GSN_LOCK();

  if (actv_sapis[sapi] != NULL)
  {
    sapi_ptr = actv_sapis[sapi];

    if (sapi_ptr->is_nsapi_ptr_q_valid)
    {

      item_ptr = (gsn_nsapi_q_t*)q_check(&(sapi_ptr->nsapi_ptr_q));

      if (item_ptr)
      {
        while(item_ptr != NULL)
        {
          if (item_ptr->nsapi_ptr->op_mode == op_mode)
          {
            /* Resume UL data flow on the NSAPI */
            gsn_clr_suspend_event(item_ptr->nsapi_ptr->nsapi, susp_reason);
          }
          /* Get next NSAPI from the list */
          item_ptr = (gsn_nsapi_q_t*)q_next(&(sapi_ptr->nsapi_ptr_q),
                                             &item_ptr->link);
        }
      }
      else
      {
        gsn_debug_print_invalid_sapi(sapi, GSN_GSNDCP, __LINE__);
      }
    }
    else
    {
      gsn_debug_print_invalid_sapi(sapi, GSN_GSNDCP, __LINE__);
    }
  }
  else
  {
    gsn_debug_print_invalid_sapi(sapi, GSN_GSNDCP, __LINE__);
  }

  GSN_UNLOCK();
}

/*===========================================================================
===
===  FUNCTION      GSN_LL_DL_SIG_GET_MES_BUF()
===
===  DESCRIPTION
===
===    To be called by LLC to obtain a message buffer to convey a downlink
===    signalling message, to SNCDP.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    Pointer to allocated gprs_ll_dl_sig_mes_t buffer.
===    NULL is returned if memory is unavailable.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

gprs_ll_dl_sig_mes_t *gsn_ll_dl_sig_get_mes_buf(void)
{
  return(GPRS_MEM_MALLOC(sizeof(gprs_ll_dl_sig_mes_t)));
}

/*===========================================================================
===
===  FUNCTION      GSN_LL_DL_SIG_PUT_MES_BUF()
===
===  DESCRIPTION
===
===  This function puts a command buffer into snll_cmd_q.
===  Note that the command buffer must have been requested through a
===  call to gsn_ll_dl_sig_get_mes_buf()
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    A signal is set for the host task and this might causes a context switch.
===
===========================================================================*/
void gsn_ll_dl_sig_put_mes_buf(gprs_ll_dl_sig_mes_t *mes_ptr)
{
  gsn_ctrl_msg_t  *adpt_msg;

  #ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif

  /* Adapt LL msg so that can be stored in gsn_cmd_q
  */

  /* Allocate memory to convey the addapted message */
  adpt_msg = GPRS_MEM_MALLOC(sizeof(gsn_ctrl_msg_t));
  GSN_ASSERT(adpt_msg != NULL);

  /* Message sent by LLC */
  adpt_msg->cmd_id = LL_CMD;

  adpt_msg->ctrl_msg.ll_msg = mes_ptr;

  /* Lock here as this function could be called simultaneously
     by either LLC task in DSDA.*/

  GSN_LOCK();

  /* Initialize the queue link before putting an item in a queue. */
  (void) q_link(adpt_msg, &(adpt_msg->link));

  /* Put the item in the queue */
  q_put(&gsn_cmd_q, &(adpt_msg->link));

  /* Set the signal for this queue */
#ifdef FEATURE_DATA_MM
  COMP_SET_SIGNAL(host_cmd_queue_sig);
#else
  (void)rex_set_sigs(host_tcb_ptr, host_cmd_queue_sig);
#endif /* FEATURE_DATA_MM */

  GSN_UNLOCK();
}

/*===========================================================================
===
===  FUNCTION      GSN_LL_DL_DAT_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    To be called by LLC for posting a downlink data message to SNCDP.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsn_ll_dl_dat_put_mes_buf(gprs_ll_dl_dat_mes_t *mes_ptr)
{
  dsm_item_type *l3_pdu_ptr;

  if (mes_ptr != NULL)
  {

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */


    /* ...................................................................... */

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

    /* ...................................................................... */

    /* Tag this PDU with a timestamp to permit delayed processing detection. */

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
    (void)time_get(mes_ptr -> timestamp);
#endif

    /* ...................................................................... */

    /* Lock here as this function could be called simultaneously
       by either LLC task in DSDA.*/

    GSN_LOCK();

    /* Configure 'app_field' of lead DSM item in message l3_pdu to 'bear'
      associated message, and enqueue the PDU to the LL DL DSM watermark. */

    switch (mes_ptr -> mes.prim)
    {
    case LL_DATA_IND:

      l3_pdu_ptr = mes_ptr -> mes.data_ind_mes.l3_pdu;
      if (gsnll_data_wm.current_cnt + GPRS_PDU_LENGTH(l3_pdu_ptr) <=
           gsnll_data_wm.dont_exceed_cnt                             )
      {
        mes_ptr -> mes.data_ind_mes.l3_pdu -> app_field = (uint32)mes_ptr;
        dsm_enqueue(&gsnll_data_wm, &l3_pdu_ptr);
      }
      else
      {
        MSG_GERAN_ERROR_1("GSN " "LL %d: DL Dat Put: LL-DATA-IND: Discard",
          mes_ptr -> sapi);
        GPRS_PDU_FREE(&l3_pdu_ptr);
        GPRS_MEM_FREE(mes_ptr);
      }
      break;

    case LL_UNITDATA_IND:

      l3_pdu_ptr = mes_ptr -> mes.unitdata_ind_mes.l3_pdu;
      if (gsnll_data_wm.current_cnt + GPRS_PDU_LENGTH(l3_pdu_ptr) <=
           gsnll_data_wm.dont_exceed_cnt                             )
      {
        mes_ptr -> mes.unitdata_ind_mes.l3_pdu -> app_field = (uint32)mes_ptr;
        dsm_enqueue(&gsnll_data_wm, &l3_pdu_ptr);
      }
      else
      {
        MSG_GERAN_ERROR_1("GSN " "LL %d: DL Dat Put: LL-UDATA-IND: Discard",
          mes_ptr -> sapi);
        GPRS_PDU_FREE(&l3_pdu_ptr);
        GPRS_MEM_FREE(mes_ptr);
      }
      break;

    default:

      MSG_GERAN_LOW_2("GSN " "LL %d: DL Dat Put: Invalid Prim %d",
        mes_ptr -> sapi, mes_ptr -> mes.prim);
      GSN_MEM_FREE(mes_ptr);
    }

    /* ...................................................................... */

    /* Set the signal for this queue */

    if (!gsn_ll_dl_susp_flag)
    {
#ifdef FEATURE_DATA_MM
      COMP_SET_SIGNAL(host_ll_dl_queue_sig);
#else
      (void)rex_set_sigs(host_tcb_ptr, host_ll_dl_queue_sig);
#endif /* FEATURE_DATA_MM */
    }

    GSN_UNLOCK();
  }
  else
  {
     MSG_GERAN_LOW_0("GSN " "LL DL Data Put: Invalid mes_ptr");
  }
}

/*===========================================================================
===
===  FUNCTION    GSN_LLx_UL_ACK_HIWATER()   / GSN_LLx_UL_ACK_LOWATER()
===              GSN_LLx_UL_UNACK_HIWATER() / GSN_LLx_UL_UNACK_LOWATER()
===
===  DESCRIPTION
===
===    High and low watermark callback functions for the eight (4 ACK and 4 UNACK)
===    watermark interfaces to LLC. Since the LLC watermark interface restrict the use
===    of any function parameters to be passed via the call back functions, SNDCP
===    needs to provide 16 callback functions upon registration with LLC (4 high watermark
===    and 4 low watermark functions for the 4 ACK LLC SAPs, plus 4 high watermark
===    and 4  low watermark functions for the 4 UNACK LLC SAPs).
===    Refer to function gllc_register_ll_client_sndcp() in gllc.h.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsn_ll3_ul_ack_hiwater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

   MSG_GERAN_LOW_0("GSN " "LL 3: Info ACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater(GSN_SAPI_3, ACK, GSN_UL_SUSP_CTS_OFF);
}

/* ....................................................................... */

void gsn_ll3_ul_ack_lowater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 3: Info ACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater(GSN_SAPI_3, ACK, GSN_UL_SUSP_CTS_OFF);
}

/* ----------------------------------------------------------------------- */

void gsn_ll3_ul_unack_hiwater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 3: Info UNACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater(GSN_SAPI_3, UNACK, GSN_UL_SUSP_CTS_OFF);

}

/* ....................................................................... */

void gsn_ll3_ul_unack_lowater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 3: Info UNACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater(GSN_SAPI_3, UNACK, GSN_UL_SUSP_CTS_OFF);

}

/* ======================================================================= */

void gsn_ll5_ul_ack_hiwater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 5: Info ACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater(GSN_SAPI_5, ACK, GSN_UL_SUSP_CTS_OFF);

}

/* ....................................................................... */

void gsn_ll5_ul_ack_lowater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 5: Info ACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater(GSN_SAPI_5, ACK, GSN_UL_SUSP_CTS_OFF);

}

/* ----------------------------------------------------------------------- */

void gsn_ll5_ul_unack_hiwater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 5: Info UNACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater(GSN_SAPI_5, UNACK, GSN_UL_SUSP_CTS_OFF);

}

/* ....................................................................... */

void gsn_ll5_ul_unack_lowater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 5: Info UNACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater(GSN_SAPI_5, UNACK, GSN_UL_SUSP_CTS_OFF);

}

/* ======================================================================= */

void gsn_ll9_ul_ack_hiwater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 9: Info ACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater(GSN_SAPI_9, ACK, GSN_UL_SUSP_CTS_OFF);

}

/* ....................................................................... */

void gsn_ll9_ul_ack_lowater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 9: Info ACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater(GSN_SAPI_9, ACK , GSN_UL_SUSP_CTS_OFF);

}

/* ----------------------------------------------------------------------- */

void gsn_ll9_ul_unack_hiwater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 9: Info UNACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater(GSN_SAPI_9, UNACK, GSN_UL_SUSP_CTS_OFF);

}

/* ....................................................................... */

void gsn_ll9_ul_unack_lowater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 9: Info UNACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater(GSN_SAPI_9, UNACK, GSN_UL_SUSP_CTS_OFF);

}

/* ----------------------------------------------------------------------- */

void gsn_ll11_ul_ack_hiwater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 11: Info ACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater(GSN_SAPI_11, ACK, GSN_UL_SUSP_CTS_OFF);

}

/* ....................................................................... */

void gsn_ll11_ul_ack_lowater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 11: Info ACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater(GSN_SAPI_11, ACK, GSN_UL_SUSP_CTS_OFF);

}

/* ----------------------------------------------------------------------- */

void gsn_ll11_ul_unack_hiwater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "LL 11: Info UNACK UL Suspend");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_hiwater(GSN_SAPI_11, UNACK, GSN_UL_SUSP_CTS_OFF);

}

/* ....................................................................... */

void gsn_ll11_ul_unack_lowater(dsm_watermark_type *wm_p, void *context)
{

  NOTUSED(wm_p);

  MSG_GERAN_LOW_0("GSN " "GSN LL 11: Info UNACK UL Resume");

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

  gsn_process_lowater(GSN_SAPI_11, UNACK, GSN_UL_SUSP_CTS_OFF);

}

/* ....................................................................... */

void gsn_ll3_ul_ack_lowater_xid_reset(void)
{
  gsn_process_lowater(GSN_SAPI_3, ACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll3_ul_ack_hiwater_xid_reset(void)
{
  gsn_process_hiwater(GSN_SAPI_3, ACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll3_ul_unack_lowater_xid_reset(void)
{
  gsn_process_lowater(GSN_SAPI_3, UNACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll3_ul_unack_hiwater_xid_reset(void)
{
  gsn_process_hiwater(GSN_SAPI_3, UNACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll5_ul_ack_lowater_xid_reset(void)
{
  gsn_process_lowater(GSN_SAPI_5, ACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll5_ul_ack_hiwater_xid_reset(void)
{
  gsn_process_hiwater(GSN_SAPI_5, ACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll5_ul_unack_lowater_xid_reset(void)
{
  gsn_process_lowater(GSN_SAPI_5, UNACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll5_ul_unack_hiwater_xid_reset(void)
{
  gsn_process_hiwater(GSN_SAPI_5, UNACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll9_ul_ack_lowater_xid_reset(void)
{
  gsn_process_lowater(GSN_SAPI_9, ACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll9_ul_ack_hiwater_xid_reset(void)
{
  gsn_process_hiwater(GSN_SAPI_9, ACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll9_ul_unack_lowater_xid_reset(void)
{
  gsn_process_lowater(GSN_SAPI_9, UNACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll9_ul_unack_hiwater_xid_reset(void)
{
  gsn_process_hiwater(GSN_SAPI_9, UNACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll11_ul_ack_lowater_xid_reset(void)
{
  gsn_process_lowater(GSN_SAPI_11, ACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll11_ul_ack_hiwater_xid_reset(void)
{
  gsn_process_hiwater(GSN_SAPI_11, ACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll11_ul_unack_lowater_xid_reset(void)
{
  gsn_process_lowater(GSN_SAPI_11, UNACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll11_ul_unack_hiwater_xid_reset(void)
{
  gsn_process_hiwater(GSN_SAPI_11, UNACK, GSN_UL_SUSP_XID_RESET);
}

/* ....................................................................... */

void gsn_ll3_ul_ack_lowater_nas_susp(void)
{
  gsn_process_lowater(GSN_SAPI_3, ACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll3_ul_ack_hiwater_nas_susp(void)
{
  gsn_process_hiwater(GSN_SAPI_3, ACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll3_ul_unack_lowater_nas_susp(void)
{
  gsn_process_lowater(GSN_SAPI_3, UNACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll3_ul_unack_hiwater_nas_susp(void)
{
  gsn_process_hiwater(GSN_SAPI_3, UNACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll5_ul_ack_lowater_nas_susp(void)
{
  gsn_process_lowater(GSN_SAPI_5, ACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll5_ul_ack_hiwater_nas_susp(void)
{
  gsn_process_hiwater(GSN_SAPI_5, ACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll5_ul_unack_lowater_nas_susp(void)
{
  gsn_process_lowater(GSN_SAPI_5, UNACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll5_ul_unack_hiwater_nas_susp(void)
{
  gsn_process_hiwater(GSN_SAPI_5, UNACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll9_ul_ack_lowater_nas_susp(void)
{
  gsn_process_lowater(GSN_SAPI_9, ACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll9_ul_ack_hiwater_nas_susp(void)
{
  gsn_process_hiwater(GSN_SAPI_9, ACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll9_ul_unack_lowater_nas_susp(void)
{
  gsn_process_lowater(GSN_SAPI_9, UNACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll9_ul_unack_hiwater_nas_susp(void)
{
  gsn_process_hiwater(GSN_SAPI_9, UNACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll11_ul_ack_lowater_nas_susp(void)
{
  gsn_process_lowater(GSN_SAPI_11, ACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll11_ul_ack_hiwater_nas_susp(void)
{
  gsn_process_hiwater(GSN_SAPI_11, ACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll11_ul_unack_lowater_nas_susp(void)
{
  gsn_process_lowater(GSN_SAPI_11, UNACK, GSN_UL_SUSP_NAS_SUSP);
}

/* ....................................................................... */

void gsn_ll11_ul_unack_hiwater_nas_susp(void)
{
  gsn_process_hiwater(GSN_SAPI_11, UNACK, GSN_UL_SUSP_NAS_SUSP);
}


/* ======================================================================= */


/*****************************************************************************
***
***     Public Data
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GSNDCP_SNSM_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Post the SNSM message as given by the message pointer to the SNDCP
===    command queue.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsndcp_snsm_put_mes_dyn(gsmsn_mes_type* mes_ptr)
{
  gsn_ctrl_msg_t  *adpt_msg;

  GSN_ASSERT(sndcp_init != FALSE);

  if (mes_ptr == NULL)
  {
    MSG_GERAN_LOW_0("GSN " "NULL cmd SM => SNDCP ");
    return;
  }

  /* Log the message for call flow analysis. */

  switch (mes_ptr -> mes.prim)
  {
  case GSNSM_ACTIVATE_IND:
    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_SNDCP_SM, (uint8)GSNSM_ACTIVATE_IND,
      sizeof(gsnsm_activ_ind_type), (uint8 *)&mes_ptr->mes.snsm_activ_ind
   );
    break;

  case GSNSM_DEACTIVATE_IND:
    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_SNDCP_SM, (uint8)GSNSM_DEACTIVATE_IND,
      sizeof(gsnsm_deactiv_ind_type), (uint8 *)&mes_ptr->mes.snsm_deactiv_ind
   );
    break;

  case GSNSM_MODIFY_IND:
    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_SNDCP_SM, (uint8)GSNSM_MODIFY_IND,
      sizeof(gsnsm_modify_ind_type), (uint8 *)&mes_ptr->mes.snsm_modify_ind
   );
    break;

  case GSNSM_SEQUENCE_IND:
    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_SNDCP_SM, (uint8)GSNSM_SEQUENCE_IND,
      sizeof(gsnsm_seq_ind_type), (uint8 *)&mes_ptr->mes.snsm_seq_ind
   );
    break;

  case GSNSM_ENTER_PSM_IND:
    cfa_log_packet_ex
    (
      GS_QUEUE_SNDCP, MS_SNDCP_SM, (uint8)GSNSM_ENTER_PSM_IND,
      sizeof(gsnsm_enter_psm_ind_type), (uint8 *)&mes_ptr->mes.snsm_enter_psm_ind
   );
    break;

  default:
    break;
  }

  /* Adapt SM msg so that can be stored in gsn_cmd_q
  */

  /* Allocate memory to convey the addapted message */
  adpt_msg = GPRS_MEM_MALLOC(sizeof(gsn_ctrl_msg_t));
  GSN_ASSERT(adpt_msg != NULL);

  /* Message sent by SM */
  adpt_msg->cmd_id = SM_CMD;

  adpt_msg->ctrl_msg.sm_msg = mes_ptr;

  /* Initialize the queue link before putting an item in a queue. */
  (void) q_link(adpt_msg, &(adpt_msg->link));

  /* Put the item in the queue */
  q_put(&gsn_cmd_q, &(adpt_msg->link));

#ifdef FEATURE_DATA_MM
  /* Set the signal for this queue */
  COMP_SET_SIGNAL(host_cmd_queue_sig);
#else
  /* Set the signal for this queue */
  (void)rex_set_sigs(host_tcb_ptr, host_cmd_queue_sig);
#endif /* FEATURE_DATA_MM */

}


/*===========================================================================
===
===  FUNCTION      GSN_INT_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Post the SNDCP internal message as given by the message pointer
===    to the SNDCP command queue.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsn_int_put_mes_dyn(gsn_internal_msg_t* mes_ptr)
{
  gsn_ctrl_msg_t  *adpt_msg;

  GSN_ASSERT(sndcp_init != FALSE);

  /* Adapt SNDCP msg so that can be stored in gsn_cmd_q
  */

  /* Allocate memory to convey the adapted message */
  adpt_msg = GPRS_MEM_MALLOC(sizeof(gsn_ctrl_msg_t));
  GSN_ASSERT(adpt_msg != NULL );

  /* Message sent by SNDCP */
  adpt_msg->cmd_id = GSN_CMD;

  adpt_msg->ctrl_msg.gsn_msg = mes_ptr;

  /* Initialize the queue link before putting an item in a queue. */
  (void) q_link(adpt_msg, &(adpt_msg->link));

  /* Put the item in the queue */
  q_put(&gsn_cmd_q, &(adpt_msg->link));

#ifdef FEATURE_DATA_MM
  /* Set the signal for this queue */
  COMP_SET_SIGNAL(host_cmd_queue_sig);
#else
  /* Set the signal for this queue */
  (void) rex_set_sigs(host_tcb_ptr, host_cmd_queue_sig);
#endif /* FEATURE_DATA_MM */
}


/*===========================================================================
FUNCTION GSNDCP_TO_SNDCP_EACH_ENQUEUE_CB

DESCRIPTION
  This callback function is called when a packet is enqueued on the
  SNDCP UL Watermark.

PARAMETERS
   wm            : Pointer to the Tx watermark registered with SNDCP
   callback_data : NSAPI registered with the watermark callback

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -e818*/
void gsndcp_to_sndcp_each_enqueue_cb_dyn
(
  dsm_watermark_type *wm,
  /*lint -esym(715,wm) */
  void*              callback_data
)
{
  uint8    nsapi;
  uint8    external_nsapi;

  /* To remove compiler warnings */
  uint32   rcvd_callback_data = (uint32)callback_data;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the NSAPI */
  nsapi          = (uint8)(rcvd_callback_data & 0xFF);

  /* Get external NSAPI */
  external_nsapi = GSN_EXTERN_NSAPI(nsapi);

  if (GSN_VALID_NSAPI(external_nsapi))
  {
    if (actv_nsapis[nsapi] != NULL)
    {
      /* Set the TX DATA Signal for the nsapi */
      COMP_SET_SIGNAL(actv_nsapis[nsapi]->to_gsndcp_sig);
    }
    else
    {
      gsn_debug_print_invalid_nsapi(nsapi, GSN_GSNDCP, __LINE__);
    }
  }
  else
  {
    gsn_debug_print_invalid_nsapi(nsapi, GSN_GSNDCP, __LINE__);
  }
} /* gsndcp_to_sndcp_each_enqueue_cb() */

/*===========================================================================
FUNCTION DSGPSD_TO_SNDCP_SIG_HDLR

DESCRIPTION
  This function is called when the Tx DATA Signal is processed.

PARAMETERS
    Sig - Signal that was processed
    user_data_ptr - NSAPI for the signal that was processed.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
boolean gsndcp_to_sndcp_sig_hdlr_dyn
(
  comp_sig_enum_type sig,
  void          *user_data_ptr
)
{

  /* To remove compiler warnings */
  uint32   rcvd_callback_data = (uint32)user_data_ptr;

  /* Get the NSAPI */
  uint8    nsapi    = (uint8)(rcvd_callback_data & 0xFF);

  dsm_item_type *item_ptr;

  /* Get external NSAPI */
  uint8 external_nsapi = GSN_EXTERN_NSAPI(nsapi);

  if (!GSN_VALID_NSAPI(external_nsapi) || actv_nsapis[nsapi] == NULL)
  {
    gsn_debug_print_invalid_nsapi(nsapi, GSN_GSNDCP, __LINE__);
    return FALSE;
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((!(actv_nsapis[nsapi]->ule.susp_flag &
       GSN_UL_SUSP_ALL_REASONS_MASK)) &&
      (actv_nsapis[nsapi]->to_gsndcp_wm != NULL))
  {
    /* Dequeue data from sndcp WM.*/
    item_ptr = (dsm_item_type *)dsm_dequeue(
                   actv_nsapis[nsapi]->to_gsndcp_wm);

    /* Call function to process the Uplink packet.*/
    gsndcp_pdp_send_ul_npdu(GSN_EXTERN_NSAPI(nsapi), &item_ptr);

    /* nsapi could get deactivated if SNSM_DEACTIVATE_IND is processed during
       cmd_queue processing in gsndcp_pdp_send_ul_npdu(). If nsapi is still
       valid then proceed with UL transfer. Otherwise, free PDU and return
       FALSE.
    */

    if (actv_nsapis[nsapi]== NULL)
    {
      gsn_debug_print_invalid_nsapi(nsapi, GSN_GSNDCP, __LINE__);
      /* Free dsm item */
      GPRS_PDU_FREE(&item_ptr);
      return FALSE;
    }

    /* If there is more data in the watermark, set the signal again.*/
    if (actv_nsapis[nsapi]->to_gsndcp_wm->current_cnt != 0)
    {
      COMP_SET_SIGNAL(sig);
    }
  }
  else
  {
    MSG_GERAN_LOW_2("GSN ""ule.susp_flag %d, to_gsndcp_wm ptr %p ",
                          actv_nsapis[nsapi]->ule.susp_flag,
                          actv_nsapis[nsapi]->to_gsndcp_wm);
  }
  return TRUE;
} /* gsndcp_to_sndcp_sig_hdlr() */

/*===========================================================================
FUNCTION GSNDCP_CHECK_DATA_IN_SMD_TO_SNDCP_WM

DESCRIPTION
  This function is checks if there is any data in the SMD->SNDCP WM. If there
  is any, then process.

PARAMETERS
  nsapi.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/

void gsndcp_check_data_in_smd_to_sndcp_wm(const uint8 nsapi)
{

  uint8 external_nsapi;

  /* Get external NSAPI */
  external_nsapi = GSN_EXTERN_NSAPI(nsapi);

  if (GSN_VALID_NSAPI(external_nsapi))
  {
    if (actv_nsapis[nsapi] != NULL)
    {
      if (actv_nsapis[nsapi]->to_gsndcp_wm)
      {
        if (!dsm_is_wm_empty(actv_nsapis[nsapi]->to_gsndcp_wm))
        {
          gsndcp_to_sndcp_each_enqueue_cb_dyn(actv_nsapis[nsapi]->to_gsndcp_wm, (void*)((uint32)nsapi));
        }
      }
    }
    else
    {
      gsn_debug_print_invalid_nsapi(nsapi, GSN_GSNDCP, __LINE__);
    }
  }
  else
  {
    gsn_debug_print_invalid_nsapi(nsapi, GSN_GSNDCP, __LINE__);
  }

} /* gsndcp_check_data_in_smd_to_sndcp_wm()*/

/*===========================================================================
===
===  FUNCTION      GSNDCP_UL_PDU_PRIORITY_HDLR()
===
===  DESCRIPTION
===
===    Determines priority of UL IP packets.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Prioerity of IP packet.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

geran_pdu_priority_t gsndcp_ul_pdu_priority_hdlr(dsm_item_type *npdu)
{
    dsm_item_type        *npdu_copy;
    uint8                ip_datagram[80];
    uint16               ip_total_len;
    uint8                ip_version;
    uint8                ip_hdr_len     = 0;
    uint8                ip_prtcol_type = 0;
    uint8                tcp_hdr_ctrl   = 0;
    geran_pdu_priority_t pdu_priority   = GPDU_LOW_PRIORITY;

    /* Duplicate only the top 80 bytes of the N-PDU. These include 20 bytes (IPV4)
    ** or 40 bytes (IPV6) of IP header plus the 20 bytes TCP header (without optional
    ** fields).
    */
    ip_total_len = GPRS_PDU_LENGTH(npdu);

    if (ip_total_len > 80)
      ip_total_len = 80;

    npdu_copy = gprs_pdu_duplicate(npdu, 0, ip_total_len);

    if (npdu_copy != NULL)
    {
      if (!gprs_pdu_strip_head(&npdu_copy, ip_datagram, ip_total_len))
      {
        MSG_GERAN_ERROR_0("GSN " "GSNDCP PDU priority hdlr: gprs_pdu_strip_head() failed");
      }
      else
      {

        ip_version = ((*ip_datagram) >> 4);

        if (ip_version == 4) /* IPv4 */
        {
          ip_hdr_len          = (*ip_datagram & 0x0f) * 4;
          ip_total_len        = (((uint16) ip_datagram[2]) << 8) | ((uint16) ip_datagram[3]) ;
          ip_prtcol_type      = (uint8) ip_datagram[9];

          if (ip_prtcol_type == 1) /* ICMP protocol */
          {
            pdu_priority = GPDU_HIGH_PRIORITY;
          }
          else if (ip_prtcol_type == 6) /* TCP protocol */
          {

            tcp_hdr_ctrl = (uint8) ip_datagram[ip_hdr_len + 13];


            /* If ACK, PUSH, RESET, SYN, FIN, and PDU is less than 100 octets, then set high priority. */

            if (((tcp_hdr_ctrl & 0x10) ||
                   (tcp_hdr_ctrl & 0x01) ||
                   (tcp_hdr_ctrl & 0x02) ||
                   (tcp_hdr_ctrl & 0x04) ||
                   (tcp_hdr_ctrl & 0x08)   ) &&
                   (ip_total_len < 100 )
              )
            {
              pdu_priority = GPDU_HIGH_PRIORITY;
            }
          }
          else
          {

            /* All other protocol types are lower priority. */

            pdu_priority = GPDU_LOW_PRIORITY;

          }
        }
        else if (ip_version == 6)  /* IPv6 */
        {

          ip_hdr_len          = 40;
          ip_total_len        = (((uint16) ip_datagram[4]) << 8) | ((uint16) ip_datagram[5]) ;
          ip_prtcol_type      = (uint8) ip_datagram[6];

          if (ip_prtcol_type == 1) /* ICMP protocol */
          {
            pdu_priority = GPDU_HIGH_PRIORITY;
          }
          else if (ip_prtcol_type == 6) /* TCP protocol */
          {

            tcp_hdr_ctrl = (uint8) ip_datagram[ip_hdr_len + 13];


            /* If ACK, PUSH, RESET, SYN, FIN, and PDU is less than 100 octets, then set high priority. */

            if (((tcp_hdr_ctrl & 0x10) ||
                   (tcp_hdr_ctrl & 0x01) ||
                   (tcp_hdr_ctrl & 0x02) ||
                   (tcp_hdr_ctrl & 0x04) ||
                   (tcp_hdr_ctrl & 0x08)   ) &&
                   (ip_total_len < 100 )
              )
            {
              pdu_priority = GPDU_HIGH_PRIORITY;
            }
          }
          else
          {

            /* All other protocol types are lower priority. */

            pdu_priority = GPDU_LOW_PRIORITY;

          }

        }
        else
        {
          MSG_GERAN_ERROR_0("GSN " "GSNDCP PDU priority handler: Invalid IP version");
        }

        MSG_GERAN_LOW_3("GSN " "GSNDCP PDU priority hdlr DBG: ip_hdr_len: %d, ip_total_len: %d, ip_prtcol_type: %d",
                            ip_hdr_len,
                            ip_total_len,
                            ip_prtcol_type);

        MSG_GERAN_LOW_2("GSN " "GSNDCP PDU priority hdlr DBG: tcp_hdr_ctrl: 0x%x, pdu_priority: %d ", tcp_hdr_ctrl,
                                                                                                       pdu_priority);

      }
    }
    else
    {
      MSG_GERAN_ERROR_0("GSN " "GSNDCP PDU priority handler: run out of dup dsm items");
    }

    return pdu_priority;

} /* gsndcp_ul_pdu_priority_hdlr() */


/*===========================================================================
===
===  FUNCTION      GSN_TIMER_CALLBACK()
===
===  DESCRIPTION
===
===    Sends an internal message to SNDCP's command queue to indicate the
===    expiry of sgsn initiated rel class 2 link establishment timer.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsn_timer_callback_dyn(uint32 gasid_nsapi_and_timer_id)
{
  gsn_internal_msg_t *msg_ptr;

  gsn_nsapi_t nsapi    = (gsn_nsapi_t)((gasid_nsapi_and_timer_id >> 16) & 0xFF);
  uint16      timer_id = (uint16)(gasid_nsapi_and_timer_id & 0xFFFF);


  /* Obtain an internal message resource */

  msg_ptr = gsn_int_get_mes_buf();
  GSN_ASSERT(msg_ptr != NULL);

  /* Construct message and post to command queue for processing. */

  msg_ptr->cmd_id               = TIMER_CMD;
  msg_ptr->gsn_cmd.timer.tmr_id = (gsn_timer_id_t) timer_id;
  msg_ptr->gsn_cmd.timer.nsapi  = nsapi;

  gsn_int_put_mes_dyn(msg_ptr);

} /* gsn_timer_callback() */

boolean gsndcp_process_cmd_queue_dyn
(
#ifdef FEATURE_DATA_MM
  comp_sig_enum_type sig,
  void             *pkt_instance
#else
 void
#endif /* FEATURE_DATA_MM */
)
{
  gsn_ctrl_msg_t        *adpt_msg_ptr;
  gprs_ll_dl_sig_mes_t  *ll_msg_ptr;
  gsmsn_mes_type        *sm_msg_ptr;
  gsn_internal_msg_t    *gsn_msg_ptr;
  gprs_ll_ul_sig_mes_t  *ul_mes;
  gsn_sapi_t            sapi;

#ifdef FEATURE_DATA_MM
  NOTUSED(sig);
  NOTUSED(pkt_instance);
#endif

  GSN_ASSERT(sndcp_init != FALSE);

  /* Process SM, LLC or SNDCP control primitives
  */
  while(q_cnt(&gsn_cmd_q))
  {
    /* Read one item from the queue that holds the control primitives */
    adpt_msg_ptr = (gsn_ctrl_msg_t*) q_get(&gsn_cmd_q);

    if (adpt_msg_ptr == NULL)
    {
      MSG_GERAN_ERROR_0("GSN ""q_get() returned NULL");
    }
    /* Determine the sender of the primitive */
    else switch(adpt_msg_ptr->cmd_id)
    {
    case (LL_CMD):

      ll_msg_ptr = adpt_msg_ptr->ctrl_msg.ll_msg;


      /* Check if SAPI number is valid and if SAPI has been activated */
      if ((GSN_VALID_SAPI(ll_msg_ptr->sapi)) &&
               (actv_sapis[gsn_intern_sapi[ll_msg_ptr->sapi]] != NULL))
      {
        /* Translate LLC SAPI number to SNDCP internal SAPI number */
        sapi = (gsn_sapi_t) gsn_intern_sapi[ll_msg_ptr->sapi];

        /* Process the LL control primitive */
        gsn_ll_ctrl_prim_hdlr( &ll_msg_ptr->mes, sapi);

      }
      else
      {
        /* The SAPI is not active. The only valid message from LLC at
        ** this stage is LL_XID_IND with no XID pdu, to inform SNDCP
        ** about the N201_I and N201_U values.
        */
        if ((ll_msg_ptr->mes.prim == LL_XID_IND)              &&
            (ll_msg_ptr->mes.xid_ind_mes.l3_xid_req == NULL)  &&
            (ll_msg_ptr->mes.xid_ind_mes.n201_i)              &&
            (ll_msg_ptr->mes.xid_ind_mes.n201_u))
        {
          /* Get the N201_I and N201_U values */
          if (GSN_VALID_SAPI(ll_msg_ptr->sapi))
          {
            gsn_n201_i[gsn_intern_sapi[ll_msg_ptr->sapi]] =
                            ll_msg_ptr->mes.xid_ind_mes.n201_i;
            gsn_n201_u[gsn_intern_sapi[ll_msg_ptr->sapi]] =
                            ll_msg_ptr->mes.xid_ind_mes.n201_u;
            MSG_GERAN_LOW_1("GSN ""LL %d: Rcvd LL_XID_IND",ll_msg_ptr->sapi);
          }
          else
            gsn_debug_print_invalid_sapi(gsn_intern_sapi[ll_msg_ptr->sapi], GSN_GSNDCP, __LINE__);
        }
        else
        {
          /* It is possible to rcv an LL_RESET_IND on an Inactive SAPI.
          ** Do not print a message in this case.
          */
          if (ll_msg_ptr->mes.prim != LL_RESET_IND)
          {
            MSG_GERAN_LOW_1("GSN ""Inactive SAPI: %d",ll_msg_ptr->sapi);
          }

          gsn_ll_ctrl_prim_free_xid_pdu(ll_msg_ptr);

          /* For the  following two LL DL primitives, inform LLC about the
          ** inactive SAPI so that LLC state machine can get out of the
          ** "await response" state.
          **/
          if ((ll_msg_ptr->mes.prim == LL_XID_IND)     ||
              (ll_msg_ptr->mes.prim == LL_ESTABLISH_IND))
          {
            /* Get buffer to convey the message */
            ul_mes = gllc_ll_ul_sig_get_mes_buf();
            GSN_ASSERT(ul_mes != NULL);

            /* Load msg */
            ul_mes->sapi                        = ll_msg_ptr->sapi;
            ul_mes->mes.inact_sapi_res_mes.prim = LL_INACT_SAPI_RES;

            /* Send msg */
            gllc_ll_ul_sig_put_mes_buf( ul_mes);
          }
        }
      }

      /* Free the memory used to convey the primitive */
      GSN_MEM_FREE(ll_msg_ptr);

      break;

    case (SM_CMD):

      sm_msg_ptr = adpt_msg_ptr->ctrl_msg.sm_msg;

      /* Process the SNSM primitive */
      gsn_sm_prim_hdlr(sm_msg_ptr);

      /* Free the memory used to convey the primitive */
      GSN_MEM_FREE(sm_msg_ptr);

      break;

    case (GSN_CMD):

      gsn_msg_ptr = adpt_msg_ptr->ctrl_msg.gsn_msg;

      /* Process the SNDCP internal cmd */
      gsn_int_cmd_hdlr(gsn_msg_ptr);

      /* Free the memory used to convey the primitive */
       GSN_MEM_FREE(gsn_msg_ptr);

      break;

    default:

      MSG_GERAN_ERROR_1("GSN ""ME: Invalid control prim id: %d",
                      adpt_msg_ptr->cmd_id);
      break;

    } /* End of switch(adpt_msg_ptr->cmd_id) */

    /* Free the memory used to convey the adapted message */
    GSN_MEM_FREE(adpt_msg_ptr);
  }
  return TRUE;
}

/*===========================================================================
===
===  FUNCTION      GSNDCP_PROCESS_LL_DL_QUEUE()
===
===  DESCRIPTION
===
===    To be called by the hosting task on activation of the
===    host_ll_dl_queue_sig that was registered with SNDCP on initialization.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised. The host task is expected to clear the
===    associated REX signal before calling this handler.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

boolean gsndcp_process_ll_dl_queue_dyn
(
#ifdef FEATURE_DATA_MM
  comp_sig_enum_type sig,
  void             *pkt_instance
#else
  void
#endif
)
{
  gsn_sapi_t            sapi;
  gprs_ll_dl_dat_mes_t  *mes_ptr;
  gsn_sapi_e_t          *sapi_ptr;
  dsm_item_type         *lead_item_ptr;

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
  #define GSN_PROC_DELAY_RPT_THRESHOLD_MS   300

  time_type             process_ts, process_delay_ts;
  uint32                process_delay;
#endif


#ifdef FEATURE_DATA_MM
  NOTUSED(sig);
  NOTUSED(pkt_instance);
#endif

  GSN_ASSERT(sndcp_init != FALSE);

  /* Extract first PDU from the LL downlink data watermark. */

  if (!gsn_ll_dl_susp_flag)
    lead_item_ptr = dsm_dequeue(&gsnll_data_wm);
  else
    lead_item_ptr = NULL;

  while (lead_item_ptr != NULL)
  {

    mes_ptr = (gprs_ll_dl_dat_mes_t *)(lead_item_ptr -> app_field);


    if (mes_ptr != NULL)
    {
      /* Check if SAPI is valid */
      if ((!GSN_VALID_SAPI(mes_ptr->sapi)) ||
          (actv_sapis[gsn_intern_sapi[mes_ptr->sapi]] == NULL))
      {
        MSG_GERAN_ERROR_1("GSN ""LL %d: Invalid/Inactive SAPI",mes_ptr->sapi);

        /* Free dsm item */
        if (mes_ptr->mes.prim == LL_DATA_IND)
        {
          if (mes_ptr->mes.data_ind_mes.l3_pdu)
            GPRS_PDU_FREE(&mes_ptr->mes.data_ind_mes.l3_pdu);
        }
        else if (mes_ptr->mes.prim == LL_UNITDATA_IND)
        {
          if (mes_ptr->mes.unitdata_ind_mes.l3_pdu)
            GPRS_PDU_FREE(&mes_ptr->mes.unitdata_ind_mes.l3_pdu);
        }
        else
          MSG_GERAN_LOW_1("GSN ""LL %d: Invalid LL data prim",mes_ptr->sapi);
      }
      else
      {
        /* Translate LLC SAPI number to SNDCP internal SAPI number */
        sapi = (gsn_sapi_t) gsn_intern_sapi[mes_ptr->sapi];
        sapi_ptr = actv_sapis[sapi];

        /* Process the LL data primitive */
        if ((mes_ptr->mes.prim == LL_DATA_IND) &&
            (mes_ptr->mes.data_ind_mes.l3_pdu != NULL))
        {
          /* Check if SAPI is established */
          if ((sapi_ptr->state != ESTABLISHED) && (!sapi_ptr->re_establish))
          {
            MSG_GERAN_LOW_1("GSN ""LL %d: Not EST for LL_DATA_IND",
              gsn_extern_sapi[sapi]);

            GPRS_PDU_FREE(&mes_ptr->mes.data_ind_mes.l3_pdu);
          }
          else
          {
            /* Process DL ACK data */
            gsn_process_ll_data_ind (sapi_ptr, &mes_ptr->mes.data_ind_mes);
          }
        }
        else if ((mes_ptr->mes.prim == LL_UNITDATA_IND) &&
                 (mes_ptr->mes.unitdata_ind_mes.l3_pdu != NULL))
        {
          /* Process DL UNACK data */
          gsn_process_ll_udata_ind (sapi_ptr, &mes_ptr->mes.unitdata_ind_mes);
        }
        else
          MSG_GERAN_LOW_1("GSN ""LL %d: Invalid data indication",mes_ptr->sapi);

        /* ................................................................. */

#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
        /* Check for CPU loading as a delaying factor in getting DL PDUs
           processed. Establish the processing delay with respect to when the
           PDU was queued. */

        (void)time_get(process_ts);
        qw_sub(process_delay_ts, process_ts, mes_ptr -> timestamp);

        /* Take the low 32 bits from the total time and shift right by 16 so
           that it contains the number of 1.25ms increments that have elapsed. */

        process_delay = qw_lo(process_delay_ts) >> 16;

        /* Multiply total_time by 5/4 to convert from 1.25ms units to 1ms units */

        process_delay = (process_delay * 5) >> 2;

        /* HACK - If the processing delay is greater than 0xFFF0 then some
           spurious wrap condition has occured in the TS processing. When this
           occurs the delay was found to be less than 1ms, so force to 1ms. */

        if (process_delay > 0xFFF0UL)
          process_delay = 1;

        if (process_delay > process_delay_max)
          process_delay_max = process_delay;

        if (process_delay > GSN_PROC_DELAY_RPT_THRESHOLD_MS)
        {
          MSG_GERAN_LOW_3("GSN " "LL DL Proc Delay > %d ms - %d ms, Highest %d ms",
            GSN_PROC_DELAY_RPT_THRESHOLD_MS, process_delay, process_delay_max);
        }
#endif /* #ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST */

        /* ................................................................. */
      }

      /* Free the memory used to convey the primitive */
      GSN_MEM_FREE(mes_ptr);
    }
    else
    {
      MSG_GERAN_LOW_0("GSN " "LL DL Proc: App Field Err");
      GPRS_PDU_FREE(&lead_item_ptr);
    }

    /* Extract next PDU from the LL downlink data watermark. */

    if (!gsn_ll_dl_susp_flag)
      lead_item_ptr = dsm_dequeue(&gsnll_data_wm);
    else
      lead_item_ptr = NULL;
  }
  return TRUE;
}



/*===========================================================================
===
===  FUNCTION      GSNDCP_PDP_SEND_UL_NPDU()
===
===  DESCRIPTION
===
===    To be called by the PDP uplink packet sourcing function for onward
===    transmission via the given NSAPI address.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised, and the PDP must have been successfully
===    registered with SNDCP via a prior call to gsndcp_pdp_register().
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsndcp_pdp_send_ul_npdu
(
  uint8                nsapi,
  dsm_item_type        **npdu
)
{
  gsn_nsapi_t          gnsapi;
  gsn_nsapi_e_t        *nsapi_ptr;
  geran_pdu_priority_t pdu_priority;

  GSN_ASSERT(sndcp_init != FALSE);

  /* PS controls UL data flow and SNDCP could be locked in a continuous UL data transmit
  ** thread while there are control commands from LLC (i.e. LL_RESET_IND) in the SNDCP
  ** command queue. Process any commands, if any, before the UL transmission commences.
  */
  if (q_cnt(&gsn_cmd_q))
  {
#ifdef FEATURE_DATA_MM
    (void)gsndcp_process_cmd_queue_dyn((comp_sig_enum_type)0,NULL);
#else
    gsndcp_process_cmd_queue_dyn();
#endif
    MSG_GERAN_LOW_1("GSN ""Interrupt UL data thread for SNDCP cmd",nsapi);
  }

  if ((npdu != NULL) && (*npdu != NULL))
  {
    sndcp_log_ul_tcp_hdr(*npdu);

    /* Determine PDU priority */
    pdu_priority = gsndcp_ul_pdu_priority_hdlr(*npdu);

    /* check if valid NSAPI */
    if (GSN_VALID_NSAPI(nsapi))
    {
      /* Get NSAPI number */
      gnsapi = (gsn_nsapi_t) GSN_INTERN_NSAPI(nsapi);
      nsapi_ptr = actv_nsapis[gnsapi];

      if (nsapi_ptr != NULL)
      {
        /* Process NPDU according to the mode of operation of the NSAPI (ACK/UNACK) */
        if (nsapi_ptr->op_mode == ACK)
        {
          /* Check if NSAPI is mapped on to a valid SAPI */
          if ((nsapi_ptr->sapi_ptr != NULL) &&
              (nsapi_ptr->sapi_ptr->state != ESTABLISHED))
          {
            MSG_GERAN_ERROR_2("GSN ""NS %d: Rcvd SN_DATA for un-established SAPI %d",
              nsapi,gsn_extern_sapi[nsapi_ptr->sapi_ptr->sapi]);

            /* Free dsm item */
            GPRS_PDU_FREE(npdu);

            return;
          }
          else if (nsapi_ptr->sapi_ptr == NULL)
          {
            gsn_debug_print_invalid_nsapi(nsapi, GSN_GSNDCP, __LINE__);
            /* Free dsm item */
            GPRS_PDU_FREE(npdu);

            return;
          }

          /* Check if buffered NPDU, for re-transmission, excead the max allowed number */
          else if (q_cnt(&(nsapi_ptr->ule.npdu_buff)) >= GPRS_MAX_RETR_NPDUS)
          {
            MSG_GERAN_ERROR_1("GSN ""NS %d: N-PDU buff holds GPRS_MAX_RETR_NPDUS",nsapi);
          }

          /* Check if NSAPI is suspended */
          else if (nsapi_ptr->ule.susp_flag)
          {
            MSG_GERAN_HIGH_2("GSN ""NS %d: Rcvd SN_DATA_REQ while suspended: susp_flag %x",
              nsapi, nsapi_ptr->ule.susp_flag);
          }

          gsn_process_sn_data_req(gnsapi, npdu, FALSE, pdu_priority);
        }
        else
        {
          /* Check if NSAPI is mapped on a valid SAPI */
          if (nsapi_ptr->sapi_ptr == NULL)
          {
            MSG_GERAN_ERROR_1("GSN ""NS %d: Rcvd SN_UDATA_REQ for inalid SAPI - sapi_ptr NULL",
              nsapi);

            /* Free dsm item */
            GPRS_PDU_FREE(npdu);

            return;
          }

          if (nsapi_ptr->ule.susp_flag) /* Check if NSAPI is suspended */
          {
            MSG_GERAN_HIGH_2("GSN ""NS %d: Rcvd SN_UDATA_REQ while suspended: susp_flag %x",
                            nsapi, nsapi_ptr->ule.susp_flag);
          }

          gsn_process_sn_udata_req(gnsapi, npdu, FALSE, NULL, pdu_priority);
        }

        /* Delink PDP NPDU */
        *npdu = NULL;
      }
      else
      {
        /* Free dsm item */
        GPRS_PDU_FREE(npdu);
        gsn_debug_print_invalid_nsapi(gnsapi, GSN_GSNDCP, __LINE__);
      }
    }
    else
    {
      /* Free dsm item */
      GPRS_PDU_FREE(npdu);
      gsn_debug_print_invalid_nsapi(GSN_INTERN_NSAPI(nsapi), GSN_GSNDCP, __LINE__);
    }
  }
  else
    MSG_GERAN_ERROR_1("GSN ""ME NSAPI %d: Inval UL NPDU",nsapi);
}

/*===========================================================================
===
===  FUNCTION      GSNDCP_PDP_DL_SUSPEND()
===
===  DESCRIPTION
===
===    Suspends downlink PDU flow to the PDP.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsndcp_pdp_dl_suspend(void)
{
  MSG_GERAN_LOW_0("GSN " "LL: Info PDP DL Suspend");

  gsn_ll_dl_susp_flag = TRUE;

#ifdef FEATURE_DATA_MM
  COMP_CLR_SIGNAL(host_ll_dl_queue_sig);
#else
  (void) rex_clr_sigs(host_tcb_ptr, host_ll_dl_queue_sig);
#endif /* FEATURE_DATA_MM */
}


/*===========================================================================
===
===  FUNCTION      GSNDCP_PDP_DL_RESUME()
===
===  DESCRIPTION
===
===    Resumes downlink PDU flow to the PDP.
===
===  DEPENDENCIES
===
===    SNDCP must be initialised.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsndcp_pdp_dl_resume(void)
{
  MSG_GERAN_LOW_0("GSN " "LL: Info PDP DL Resume");

  gsn_ll_dl_susp_flag = FALSE;

  if (q_cnt(&gsnll_data_q) != 0)
#ifdef FEATURE_DATA_MM
    /* Set the signal for this queue */
    COMP_SET_SIGNAL(host_ll_dl_queue_sig);
#else
    (void) rex_set_sigs(host_tcb_ptr, host_ll_dl_queue_sig);
#endif /* FEATURE_DATA_MM */
}


/*===========================================================================
===
===  FUNCTION      GSNDCP_FOLLOW_ON_REQ_ENQUIRY()
===
===  DESCRIPTION
===
===    For use by Data Services when faced with an enquiry by NAS for
===    Follow on Request feature.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Number of octets of signalling and data awaiting transmission in the
===    GPRS LLC and RLC entities.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

uint32 gsndcp_follow_on_req_enquiry_dyn(void)
{
  return (gllc_llme_get_current_octet_count());

} /* end of gsndcp_follow_on_req_enquiry() */


#ifndef FEATURE_GERAN_REDUCED_DEBUG
/*===========================================================================
===
===  FUNCTION      GSNDCP_DIAG_REGISTER()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to register the necessary
===    callback functions for unsolicited event reporting.
===
===    Registering or re-registering with a NULL callback pointer
===    disables the associated unsolicited event reporting category.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsndcp_diag_register
(
  gsndcp_diag_actv_nsapis_rpt_fnc_t  actv_nsapis_rpt_fnc_ptr,
  gsndcp_diag_nsapi_cntx_rpt_fnc_t   nsapi_cntx_rpt_fnc_ptr,
  gsndcp_diag_nsapi_state_rpt_fnc_t  nsapi_state_rpt_fnc_ptr
)
{
  uint8 nsapi;

  actv_nsapis_rpt_fnc = actv_nsapis_rpt_fnc_ptr;
  nsapi_cntx_rpt_fnc  = nsapi_cntx_rpt_fnc_ptr;
  nsapi_state_rpt_fnc = nsapi_state_rpt_fnc_ptr;

  /* Report to diag
  */
  gsn_encode_actv_nsapis_diag_rpt();

  for (nsapi = (uint8)GSN_NSAPI_5; nsapi < GSN_NSAPI_MAX; nsapi++)
  {
    if (actv_nsapis[nsapi] != NULL)
    {
      gsn_encode_cntx_diag_rpt((uint8) GSN_EXTERN_NSAPI(nsapi));
      gsn_encode_state_diag_rpt((uint8) GSN_EXTERN_NSAPI(nsapi));
    }
  }
}

/*===========================================================================
===
===  FUNCTION      GSNDCP_DIAG_GET_PDU_STATS()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to request the current set of
===    PDU and octet statistics for the NSAPI indicated by the given NSAPI
===    address.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to structure of type gsndcp_diag_pdu_stat_rsp_t.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

const gsndcp_diag_pdu_stat_rsp_t* gsndcp_diag_get_pdu_stats_dyn
(
  uint8 nsapi_addr
)
{
  gsn_nsapi_e_t               *nsapi_ptr;
  gsndcp_diag_pdu_stat_rsp_t  *pdu_stat_rsp = NULL;
  uint8                       int_nsapi;

  /* check if valid NSAPI */
  if (GSN_VALID_NSAPI(nsapi_addr))
  {
    int_nsapi = (uint8)GSN_INTERN_NSAPI(nsapi_addr);
    nsapi_ptr = actv_nsapis[int_nsapi];

    if (nsapi_ptr != NULL)
    {
      pdu_stat_rsp             = &nsapi_ptr->diag_pdu_stat_rsp;
      pdu_stat_rsp->nsapi_addr = (uint8) nsapi_ptr->nsapi;
    }
  }

  return(pdu_stat_rsp);
}

/*===========================================================================
===
===  FUNCTION      GSNDCP_DIAG_RESET_PDU_STATS()
===
===  DESCRIPTION
===
===    For use by the DIAG task implementation to request the reset of the
===    PDU and octet statistics for the NSAPI indicated by the given NSAPI
===    address.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void gsndcp_diag_reset_pdu_stats_dyn
(
  uint8 nsapi_addr
)
{
  gsn_nsapi_e_t   *nsapi_ptr;
  uint8           int_nsapi;

  /* check if valid NSAPI */
  if (GSN_VALID_NSAPI(nsapi_addr))
  {
    int_nsapi = (uint8)GSN_INTERN_NSAPI(nsapi_addr);
    nsapi_ptr = actv_nsapis[int_nsapi];

    if (nsapi_ptr != NULL)
    {
      (void)memset
      (
        &nsapi_ptr->diag_pdu_stat_rsp, 0,
        sizeof(gsndcp_diag_pdu_stat_rsp_t)
     );
    }
    else
    {
      gsn_debug_print_invalid_nsapi(int_nsapi, GSN_GSNDCP, __LINE__);
    }
  }
  else
  {
    gsn_debug_print_invalid_nsapi(GSN_INTERN_NSAPI(nsapi_addr), GSN_GSNDCP, __LINE__);
  }
}
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

void geran_gsn_init()
{
  MSG_GERAN_HIGH_0("DMMSUSREM:: geran_gsn_init");
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  sndcp_log_init();
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
  /* In scenarios where G gets unloaded and UE moves to other RAT, NAS can send few messages to GSNDCP.
   * These messages gets stored in Dynamic pending queue. So, next time when UE moves back to G and 
   * we get loaded, we need to process those pending messages. */
  gsn_process_dyn_pendq();
}

void geran_gsn_deinit()
{
  MSG_GERAN_HIGH_0("DMMSUSREM:: geran_gsn_deinit");
}

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
void gsn_process_dyn_pendq()
{
  q_type *dyn_pendq_ptr = gsn_get_dyn_pendq_ptr();
  gsn_ext_msg_t* pend_data_ptr = (gsn_ext_msg_t*)q_get(dyn_pendq_ptr);

  while(pend_data_ptr != NULL)
  {
    switch (pend_data_ptr->msg_type)
    {
      case SM_CMD:
      {
        gsndcp_snsm_put_mes_dyn((gsmsn_mes_type*)pend_data_ptr->msg);
        break;
      }

      case GSN_CMD:
      {
        gsn_int_put_mes_dyn((gsn_internal_msg_t*)pend_data_ptr->msg);
        break;
      }

      default:
        break;
    }

    GPRS_MEM_FREE(pend_data_ptr);
    pend_data_ptr = (gsn_ext_msg_t*)q_get(dyn_pendq_ptr);
  }
}

void geran_gsn_start_post_resumption_activity(void)
{
  uint8    nsapi;

  /* Set the signal so that gsn_cmd_q can get processed */
#ifdef FEATURE_DATA_MM
  /* Set the signal for this queue */
  COMP_SET_SIGNAL(host_cmd_queue_sig);
#else
  /* Set the signal for this queue */
  (void) rex_set_sigs(host_tcb_ptr, host_cmd_queue_sig);
#endif /* FEATURE_DATA_MM */

  /* Check if there is anything pending in WM to be processed and set signal */
  for (nsapi = GSN_NSAPI_5; nsapi < GSN_NSAPI_MAX; nsapi++)
  {
    if (actv_nsapis[nsapi] && actv_nsapis[nsapi]->to_gsndcp_wm && !dsm_is_wm_empty(actv_nsapis[nsapi]->to_gsndcp_wm))
    {
      gsndcp_to_sndcp_each_enqueue_cb_dyn(actv_nsapis[nsapi]->to_gsndcp_wm, (void*)((uint32)nsapi));
    }
  }  
}

void geran_gsn_resume()
{
  MSG_GERAN_HIGH_0("DMMSUSREM:: geran_gsn_resume");
  /* Put all messages in Dynamic pending queue back to gsn_cmd_q */
  gsn_process_dyn_pendq();
  /* Start post resumption activities */
  geran_gsn_start_post_resumption_activity();
}

void geran_gsn_suspend()
{
}
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */

/*** EOF: don't remove! ***/


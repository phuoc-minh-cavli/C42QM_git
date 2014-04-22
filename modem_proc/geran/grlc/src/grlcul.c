/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK MODULE
***
***
*** DESCRIPTION
***
***  This module contains the state machine, the event handler and initialisation
***  functions for the GPRS RLC uplink module
***
***
*** EXTERNALIZED FUNCTIONS
***
***  void grlc_ul_one_time_init (void)
***  void grlc_ul_init (void)
***
***  void grlc_ul_handle_no_ps_access (  delete_pdu_cause_t flag)
***  void grlc_ul_handle_internal_sig (void)
***  void grlc_ul_handle_pl1_sig (void)
***  void grlc_ul_handle_mac_sig (void)
***  void grlc_ul_handle_llc_tmp_pdu_sig (void)
***  void grlc_ul_handle_llc_pdu_sig (void)
***  void grlc_ul_handle_llc_msg_sig (void)
***  void grlc_ul_handle_tmr_sig (void)
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  void grlc_ul_one_time_init (void)
***  void grlc_ul_init (void)
***
*** Copyright (c) 2001-2017 Qualcomm Technologies, Inc.
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
*** $Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlcul.c#10 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/28/01    hv     Created
*** 07/09/01    hv     mods only send PH_DATA_REQ on new TBF or after
***                    coding scheme change since L1 is so periodic in its
***                    nature that it can check for itself instead of being
***                    told when data is available.
*** 09/18/01    hv     Debugged and added test code.
*** 11/21/01    hv     Removed task lock around timer callback fn. Modified
***                    handling of DSM items in unack mode to allow correct
***                    freeing of DSMs. Used ERR_FATAL for signal without msg
***                    errors.
*** 08/03/04    hv     Added EGPRS code
*** 05/07/11    hv     Added promoting sig PDUs to the front of uplink queue
*** 09/19/12    hv     Added dual data space functionality
*** 26/10/12    hv     CR409255 - corruption(s) of RLC uplink FIFO
*** 10/04/13    hv     CR464591 - reset caused by IARs and Contention-res failures
*** 10/04/13    hv     CR472248 - Issue with reallocation in change of RLC mode
*** 01/29/13    hv     CR447071 - RLC per-tbf memory allocation
*** 02/05/13    hv     CR481731 - fixed gas-id prints
*** 18/07/13    hv     CR421583
*** 20/08/13    hv     CR531564
*** 04/10/13    ra     CR552875 - tlli_cs not updated in one phase mode
*** 08/11/13    hv     CR561807
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST  */

#include "comdef.h"
#include "gmutex.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlcmaccmn.h"
#include "grlci.h"
#include "grlctsk.h"
#include "geran_msgs.h"
#include "grlcutil.h"
#include "rex.h"
#include "queue.h"
#include "gllc.h"
#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
#include "gllc_v.h"
#include "gllc_v_g.h"
#endif /* ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST  */
#include "grlcultypes.h"
#include "grlcultst.h"
#include "grlctx.h"
#include "grlcpaq.h"
#include "grlcsgm.h"
#include "grlcul.h"
#include "grlculproc.h"
#include "grlculpdu.h"
#ifdef FEATURE_GSM_DTM
#include "gttp.h"
#endif /* FEATURE_GSM_DTM */

#include "rr_public_store.h"
#include "geran_eng_mode_info.h"
#include "geran_tasks.h"
#include "geran_dual_sim_g.h"
#include "grlc_dyn_api.h"
#include "grlc_static_main.h"

#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/

/* RLC UL private data types and definitions are defined in gRLCulTypes.h
*/


/*****************************************************************************
***
***     Public Data
***
*****************************************************************************/


/*-----------------------------------------------------------------
** Critical sections to protect GRLC UL message and variable access
**-----------------------------------------------------------------
*/

rex_crit_sect_type grlc_ul_msg_crit_sec;
rex_crit_sect_type grlc_ul_no_data_crit_sec;
rex_crit_sect_type grlc_ul_tx_vars_crit_sec;
rex_crit_sect_type grlc_ul_procedure_crit_sec;


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
static void grlc_ul_panic_reset (void);

/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/
/*===========================================================================
===
===  FUNCTION      GRLC_UL_CREATE_TX_ARRAY()
===
===  DESCRIPTION
===
===    Allocate memory for the transmit array
===
===  DEPENDENCIES
===
===  PARAM
===    action_on_ps_access_t:  action
===      NO_CELL_CHANGE_NO_PURGE   = 0x0,   no cell reselection has happened between no
===                                         service to service
===      CELL_CHANGE_PURGE_GMM_SIG = 0x1,   cell reselection has happened between no
===                                         service to service
===      NO_CELL_CHANGE_PURGE_GMM_SIG = 0x2 no cell reselection has happened but grlc needs to delete
===                                         gmm messages.*Special case for T3142 AND T3172 timers
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
/* Host env can not allocate more than 32500 bytes so static memory is used
*/
#ifdef DEBUG_GSM_EGPRS_DS_UNIT_TEST
#error code not present
#endif

static void grlc_ul_create_tx_array (uint16 tx_workspace)
{
  if (NULL != p_tx_arr)
  {
    MSG_GERAN_ERROR_0_G("GRLU one-time p_ta_arr not NULL");
  }
  else
  {
    uint32 size = sizeof(grlc_ul_tx_array_t)*tx_workspace;
    MSG_GERAN_LOW_1_G("TX_ARR size = %lu", size);

#ifdef DEBUG_GSM_EGPRS_DS_UNIT_TEST
    #error code not present
#else
    p_tx_arr = (grlc_ul_tx_array_t (*)[]) (EXTPL_MEM_MALLOC(size));
#endif

    if (NULL != p_tx_arr)
    {
      MSG_GERAN_HIGH_0_G("TX_ARR allocated OK");

      /* Clear all elements */
      (void)memset((*p_tx_arr), 0, size);
    }
    else
    {
      ERR_GERAN_FATAL_0_G("ERLU Failed to allocate TX_ARR");
    }
  }
} /* grlc_ul_create_tx_array */

static void grlc_timer_stop_stale_pdu_delete()
{
  if (ul_static.timer_stale_pdu_delete.state == TMR_ACTIVE)
  {
    (void)rex_clr_timer(&(ul_static.timer_stale_pdu_delete.rex_timer));
    ul_static.timer_stale_pdu_delete.state = TMR_INACTIVE;
    MSG_GERAN_MED_0_G("GRLU stale_tmr stopped");
  }
  else
  {
    MSG_GERAN_MED_1_G("GRLU stale_tmr already stopped/expired, flag:%d",
      ul_static.b_ul_pdu_delete_after_stale_tmr_expiry);
  }
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_PS_ACCESS()
===
===  DESCRIPTION
===
===    Set access sub-state to enable and load PDU and make uplink TBF establishment
===    request. If action =  CELL_CHANGE_PURGE_GMM_SIG, clear N3102 NW variables
===
===  DEPENDENCIES
===
===  PARAM
===    action_on_ps_access_t:  action
===      NO_CELL_CHANGE_NO_PURGE   = 0x0,   no cell reselection has happened between no
===                                         service to service
===      CELL_CHANGE_PURGE_GMM_SIG = 0x1,   cell reselection has happened between no
===                                         service to service
===      NO_CELL_CHANGE_PURGE_GMM_SIG = 0x2 no cell reselection has happened but grlc needs to delete
===                                         gmm messages.*Special case for T3142 AND T3172 timers
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_handle_ps_access (action_on_ps_access_t action)
{
  rlc_mac_msg_t   *rlc_msg_ptr;

  if ( ul_static.b_ul_pdu_delete_after_stale_tmr_expiry == TRUE )
  {
    gllc_grr_ul_resume_ind();
    MSG_GERAN_HIGH_0_G("GRLU - UL Resumed");
  }

  /* PS_ACCESS indicated by MAC. If running, stop Stale PDU Timer now.
  */
  grlc_timer_stop_stale_pdu_delete();

  /* PS_ACCESS indicated by MAC. Set ul_pdu_delete flag to FALSE.
  */
  ul_static.b_ul_pdu_delete_after_stale_tmr_expiry = FALSE;


  ul.b_access = TRUE;

  /* Refresh sysinfo regardless of cell change status
  */
  read_ul_sysinfo();

  MSG_GERAN_LOW_3_G("GRLU S=%u action on ps_access=%u BS_CV_MAX=%u",
            ul.state,
            action,
            sysinfo.bs_cv_max);

  if (action == CELL_CHANGE_PURGE_GMM_SIG)
  {
    SET_N3102();

    /* Delete GMM_SIGNALLING PDUs on new cell to ensure RAUs are deleted
    ** Indicate to NAS so it can perform retry without waiting for due timer to expire.
    */
    grlc_ul_del_signalling_pdus( NAS_INDICATION);
  }
  else if (action == NO_CELL_CHANGE_PURGE_GMM_SIG)
  {
    /* Delete GMM_SIGNALLING PDUs to ensure RAUs are deleted
    ** and indicate to NAS of the deletion so it can perform retry
    ** without waiting for T3330 expiry.
    */
    grlc_ul_del_signalling_pdus( NAS_INDICATION );
  }

  /* reset the cause as we have ps_access now
  */
  ul.no_ps_access_del_pdu_cause = NO_DEL;

  RESET_N3104();

  /* Indicate PS access confirm to MAC before proceeding. */
  /* pointer to message buffer to send to MAC
  */

  /*-------------------------------------------------------*/

  rlc_msg_ptr = mac_get_cmd_buf_rlc();
  if (rlc_msg_ptr != NULL)
  {
    rlc_msg_ptr->msg.ps_access_cnf.sig = RM_PS_ACCESS_CNF;
    mac_put_cmd_rlc( rlc_msg_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("ERLU Failed to acquire MAC command buffer");
  }

  (void)load_cur_pdu();
  est_tbf_from_null();

} /* grlc_ul_handle_ps_access */


/*===========================================================================
===
===  FUNCTION      INDICATE_UL_RELEASE()
===
===  DESCRIPTION
===
===    Send an uplink release indication to MAC with a release cause.
===
===  DEPENDENCIES
===
===    None
===
===  PARAMS
===
===    release cause of type rm_tbf_release_t
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
============================================================================*/
void indicate_ul_release ( rm_tbf_release_t rel_cause )
{
  /* pointer to message buffer to send to MAC
  */
  rlc_mac_msg_t   *rlc_msg_ptr;

  /*-------------------------------------------------------*/

  rlc_msg_ptr = mac_get_cmd_buf_rlc();
  if (rlc_msg_ptr != NULL)
  {
    rlc_msg_ptr->msg.ul_release_ind.sig = RM_UL_RELEASE_IND;
    rlc_msg_ptr->msg.ul_release_ind.cause = rel_cause;
    mac_put_cmd_rlc( rlc_msg_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("ERLU GRLU heap exhaustion");
  }
} /* indicate_ul_release */

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
/*===========================================================================
===
===  FUNCTION      GRLC_UL_EXT_TBF_START_IND()
===
===  DESCRIPTION
===
===    Indicate MAC for start of EXT_UL_TBF period
===
===  DEPENDENCIES
===
===    None
===
===  PARAMS
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
============================================================================*/
void grlc_ul_ext_tbf_start_ind (void)
{
  /* pointer to message buffer to send to MAC */
  rlc_mac_msg_t   *rlc_msg_ptr;

  rlc_msg_ptr = mac_get_cmd_buf_rlc();
  if (rlc_msg_ptr != NULL)
  {
    MSG_GERAN_LOW_0_G("GRLC Sent RM_UL_EXT_TBF_START_IND");
    rlc_msg_ptr->msg.ul_ext_tbf_start_ind.sig    = RM_UL_EXT_TBF_START_IND;
    mac_put_cmd_rlc( rlc_msg_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("ERLU GRLU heap exhaustion");
  }
} /* grlc_ul_ext_tbf_start_ind */
#endif /* FEATURE_GERAN_POWER_EFFICIENT_OPERATION */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_TIMER_CALLBACK()
===
===  DESCRIPTION
===
===    Handles callback actions for all rlc ul timers. If the timer is of
===    the general purpose type then it creates a timer message with the
===    signal as passed in the function parameter and send the message to
===    rlc ul task
===
===  DEPENDENCIES
===
===    ul timer queue is initialised and ready to accept queue item.
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_ul_timer_callback (uint32  timer_param)
{
  /* to hold timer message to send to rlc ul task
  */
  rm_timer_msg_t   *timer_msg_ptr;

  /*---------------------------------------------------*/

  /*---------------------------------------------------------------
  ** The timers expected are:
  **      RM_TMR_T3182_AWAIT_UL_ACKNACK,
  **      RM_TMR_AWAIT_UL_ACKNACK,
  **      RM_TMR_AWAIT_MAC_UL_RESP,
  **      RM_TMR_AWAIT_ASS_GUARD
  **
  ** send RLC UL task timer signal as specified in timer_param
  ** The signal contains only the timer id.
  **---------------------------------------------------------------
  */

  /* Get memeory for the command
  */
  timer_msg_ptr = GPRS_MEM_MALLOC(sizeof(rm_timer_msg_t));

  if (timer_msg_ptr == NULL)
  {
    /* Failed to create timer message
    */
    ERR_GERAN_FATAL_0("GRLC UL malloc failure");
  }
  else
  {

    /* the timer id becomes the signal id
    */
    timer_msg_ptr->sig = (rm_timer_t)TIMER_GET_ID(timer_param);

    /* init q item by pointing to data area
    */
    (void)q_link(timer_msg_ptr, &timer_msg_ptr->link);

    /* add command to rlc ul queue
    */
    q_put(&UTQ(), &timer_msg_ptr->link);

    /* set signal mask as the final action in posting the message
    */
    RLC_UL_SET_TMR_SIG();
  }
} /* grlc_ul_timer_callback */


/*===========================================================================
===
===  FUNCTION      grlc_ul_handle_alloc_cnf_await_ass()
===
===  DESCRIPTION
===
===    Handles alloc_cnf in US_AWAIT_ASS and in US_XFER in PSHO
===
===  DEPENDENCIES
===
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
static void grlc_ul_handle_alloc_cnf_await_ass(void)
{
  /* Show UL FIFO contents, then re-init FIFO
  */
  MSG_GERAN_LOW_0_G("GRLC UL Alloc cnf Await. Show FIFO then re-init");
  grlc_ul_show_fifo_vars();
  grlc_ul_fifo_re_init();

  ul.state = (rlc_ul_state_t)((ul.rlc_mode == GRLC_MODE_ACK) ?
                              US_ACK_XFER :
                              US_UNACK_XFER
                             );
  UPD_UL_STATE();

  /* Accept new resource. Transfer alloc_cnf into ul.cur_res and set
  ** RALS state
  */
  accept_resource();

  /* Now assign the RLC mode from the current PDU. This value
  ** determines the next state
  */
  ul.rlc_mode = cur_pdu_ptr->rlc_mode;

  /* just got cnf so allocation for next pdu does not exist,
  ** and segment and tx must be at NULL state.
  */

  ul.segment_state = SEGMENT_NULL;

  /* If next pdu is not loaded already then attempt to load from queue
  */
  if (next_pdu_ptr == NULL)
    dequeue_pdu( &next_pdu_ptr);

  /* Initialise data block according to the access phase,
  ** ie. fields are set up for one phase access if required
  ** ! check if this valid !
  */
  if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
  {
    E_NEW_DATA_BLOCK( &cur_blk);
  }
  else
  {
    NEW_DATA_BLOCK( &cur_blk);
  }

  /* Init PSN of the first PDU to free
  */
  ul.first_psn_to_free = cur_pdu_ptr->psn;
  ul.pdu_freeing_state = NONE_TO_FREE;
  ul.psn_to_free_inclusive = ul.first_psn_to_free;

  /* cur data block and cur pdu are ready for segmentation
  ** and transmit. Call appropriate ack or unack mode xfer
  ** function
  */
  if (ul.rlc_mode == GRLC_MODE_ACK)
  {
    /* we are in await assignment and not xfer state but
    ** we are about to process invoke ack_mode_xfer
    ** function. To prepare we will change our state
    ** to US_ACK_XFER before invoking the function
    */
    ul.state = US_ACK_XFER;
    UPD_UL_STATE();

    /* Set uo flag to send PH_DATA_REQ to L1 on the
    ** first data block of this TBF
    */
    ul.b_first_fifo_load = TRUE;

    if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
    {
      e_ack_mode_xfer();
    }
    else
    {
      ack_mode_xfer();
    }
  }
  else
  {
    /* we are in await assignment and not xfer state but
    ** we are about to invoke unack_mode_xfer function.
    ** To prepare we will change our state to US_ACK_XFER
    ** before invoking the function
    */
    ul.state = US_UNACK_XFER;
    UPD_UL_STATE();

    /* ! set uo flag to send PH_DATA_REQ to L1 on the
    ** first data block of this TBF
    */
    ul.b_first_fifo_load = TRUE;

    if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
    {
      e_unack_mode_xfer();
    }
    else
    {
      unack_mode_xfer();
    }
  }
#ifdef  FEATURE_GPRS_PS_HANDOVER
  ul.b_psho_llc_pdu_absent = FALSE;
#endif /* FEATURE_GPRS_PS_HANDOVER */
}/*grlc_ul_handle_alloc_cnf_await_ass */

/*===========================================================================
===
===  FUNCTION      UPDATE_TLLI_CS
===
===  DESCRIPTION
===
===  To assign tilli_cs the coding scheme in one-phase access mode.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
============================================================================*/
static void update_tlli_cs(void)
{
  /* In one-phase access mode, ul.tlli_cs should be assigned the coding scheme
  ** as follows:
  ** if alloc_cnf.tlli_block_cs = 0, tlli_cs = RM_CS_1/RM_MCS_1
  ** if alloc_cnf.tlli_block_cs = 1, tlli_cs = alloc_cnf.cs/alloc_cnf.mcs
  */
  if (ul.cur_res.ti == RM_TLLI_PRESENT)
  {
    if (ul.cur_res.tlli_block_cs == RM_TLLI_USE_CS_1)
      ul.tlli_cs = RM_CS_1; /* RM_CS_1 == RM_MCS_1*/
    else
      ul.tlli_cs = ul.cur_res.cs;
  }
}/* update_tlli_cs */

/*===========================================================================
===
===  FUNCTION      grlc_ul_handle_alloc_cnf_ack_xfer()
===
===  DESCRIPTION
===
===    Handles alloc_cnf in US_ACK_XFER
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
static void grlc_ul_handle_alloc_cnf_ack_xfer
(
  const mr_ul_alloc_cnf_t *ul_alloc_cnf
)
{
  if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    grlc_ul_show_new_alloc( &ul.cur_res,ul_alloc_cnf);
    MSG_GERAN_LOW_3_G("ERLU A ALLOC_CNF #TS=%d mcs=%d typ=%d",
                  ul_alloc_cnf->num_timeslots,
                  ul_alloc_cnf->mcs,
                  ul_alloc_cnf->tbf_type);

    /* only if it's active
    */
    TIMER_STOP_MAC_UL_RESP();

    /* Update coding scheme for engg_mode
    */
    if (ul.cur_res.cs != ul_alloc_cnf->mcs)
    {
      geran_eng_mode_data_write(ENG_MODE_EGPRS_UL_MCS, (void*)(&ul_alloc_cnf->mcs), TRUE);
    }

    /* Check and handle change of MCS/TFI/num_timeslots
    */
    if (ul.cur_res.cs != ul_alloc_cnf->mcs                      ||
        ul.cur_res.num_timeslots != ul_alloc_cnf->num_timeslots ||
        ul.cur_res.ul_tfi != ul_alloc_cnf->ul_tfi
       )
    {
      MSG_GERAN_MED_3_G("ERLU chMCS/TFI/TS ul.mcs=%u alloc_cnf.mcs=%u",
                ul.cur_res.cs,
                ul_alloc_cnf->mcs,
                0
             );
      ul.cur_res = *ul_alloc_cnf;
      ul.cur_res.cs =  ul_alloc_cnf->mcs;

      /* Assign tilli_cs the coding scheme in one-phase access mode.
      */
      update_tlli_cs();

      /* Freeze FIFO access for reprocessing
      */
      grlc_ul_fifo_disable_pl1_access();

      /* parameter contains the new coding scheme
      */
      change_coding_scheme( ul.cur_res.cs);

      /* Re-enable PL1 access to the FIFO
      */
      grlc_ul_fifo_enable_pl1_access();

      /* update the working coding scheme
      */
      ul.last_valid_mcs = ul.cur_res.cs;
    }
    else
    {
      /* For PSHO , target cell may support USF granularity
      */
      ul.cur_res.usf_granularity = ul_alloc_cnf->usf_granularity;

     /* Enable PL1 access to FIFO (for PSHO)
      */
      grlc_ul_fifo_enable_pl1_access();

      /* Even though there is nothing to do from an RLC perspective because
      ** the coding scheme hasn't actually changed, GPL1 will be expecting
      ** a PH_DATA_REQ to unblock the UL FIFO.
      */
      pl1_send_ph_data_req();
    }

    if (ul.cur_res.ws < ul_alloc_cnf->ws)
    {
      /* Since WS can't be downsized, the new WS has to be larger. So check
      ** if the current window is stalled and if so un-stall it
      */
      if (ul.si == RM_TX_STALLED)
      {
        ul.si = RM_TX_NOT_STALLED;
        GRLC_UL_SHOW_TX_VARS();

        /* No re-processing aprt from re-enabling PL1
        */
        ul.b_first_fifo_load = TRUE;
        grlc_ul_fifo_enable_pl1_access();
        pl1_send_ph_data_req();
      }

      ul.cur_res.ws = ul_alloc_cnf->ws;

      /* WS has widened. Stop T3182
      */
      TIMER_STOP_T3182();
    }
  }
  else  /* EGPRS build GPRS mode */
  {
    grlc_ul_show_new_alloc( &ul.cur_res, ul_alloc_cnf);
    MSG_GERAN_LOW_3_G("GRLU A ALLOC_CNF #TS=%d cs=%d typ=%d",
              ul_alloc_cnf->num_timeslots,
              ul_alloc_cnf->cs,
              ul_alloc_cnf->tbf_type);

    /* only if it's active
    */
    TIMER_STOP_MAC_UL_RESP();

    /* Update coding scheme for engg_mode
    */
    if (ul.cur_res.cs != ul_alloc_cnf->cs)
    {
      geran_eng_mode_data_write(ENG_MODE_GPRS_UL_CS, (void*)(&ul_alloc_cnf->cs), TRUE);
    }

    /* check and handle change of coding scheme
    */
    if (ul.cur_res.cs != ul_alloc_cnf->cs                       ||
        ul.cur_res.num_timeslots != ul_alloc_cnf->num_timeslots ||
        ul.cur_res.ul_tfi != ul_alloc_cnf->ul_tfi
       )
    {
      ul.cur_res = *ul_alloc_cnf;
      MSG_GERAN_MED_2_G("GRLU chCS/TFI/TS cs=%d new cs=%d",
              ul.cur_res.cs,
              ul_alloc_cnf->cs
              );

      /* Assign tilli_cs the coding scheme in one-phase access mode.
      */
      update_tlli_cs();

      /* Freeze FIFO access for reprocessing
      */
      grlc_ul_fifo_disable_pl1_access();

      /* parameter contains the new coding scheme
      */
      change_coding_scheme( ul_alloc_cnf->cs);

      /* Re-enable PL1 access to the FIFO
      */
      grlc_ul_fifo_enable_pl1_access();

      /* update the working coding scheme
      */
      ul.last_valid_coding_scheme = ul.cur_res.cs;
    }
    else
    {
      /* For PSHO , target cell may support USF granularity
      */
      ul.cur_res.usf_granularity = ul_alloc_cnf->usf_granularity;

     /* Enable PL1 access to FIFO (for PSHO)
      */
      grlc_ul_fifo_enable_pl1_access();

      /* Even though there is nothing to do from an RLC perspective because
      ** the coding scheme hasn't actually changed, GPL1 will be expecting
      ** a PH_DATA_REQ to unblock the UL FIFO.
      */
      pl1_send_ph_data_req();
    }
  }
} /* grlc_ul_handle_alloc_cnf_ack_xfer() */

/*===========================================================================
===
===  FUNCTION      grlc_ul_handle_alloc_cnf_unack_xfer()
===
===  DESCRIPTION
===
===    Handles alloc_cnf in US_UNACK_XFER
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/

static void grlc_ul_handle_alloc_cnf_unack_xfer
(
  const mr_ul_alloc_cnf_t *ul_alloc_cnf
)
{
  if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    grlc_ul_show_new_alloc( &ul.cur_res,ul_alloc_cnf);
    MSG_GERAN_LOW_2_G("GRLU U UnsolAss old TS=%d new TS=%d",
              ul.cur_res.num_timeslots,
              ul_alloc_cnf->num_timeslots
              );

    /* Update coding scheme for engg_mode
    */
    if (ul.cur_res.cs != ul_alloc_cnf->mcs)
    {
      geran_eng_mode_data_write(ENG_MODE_EGPRS_UL_MCS, (void*)(&ul_alloc_cnf->mcs), TRUE);
    }

    if (ul.cur_res.cs != ul_alloc_cnf->mcs        ||
        ul.cur_res.num_timeslots !=
          ul_alloc_cnf->num_timeslots             ||
        ul.cur_res.ul_tfi != ul_alloc_cnf->ul_tfi
       )
    {
      MSG_GERAN_MED_2_G("ERLU U chMCS/TFI/TS mcs=%u new mcs=%u",
              ul.cur_res.cs,
              ul_alloc_cnf->mcs
              );
      ul.cur_res = *ul_alloc_cnf;
      ul.cur_res.cs =  ul_alloc_cnf->mcs;

      /* Assign tilli_cs the coding scheme in one-phase access mode.
      */
      update_tlli_cs();

      /* Freeze FIFO access for reprocessing
      */
      grlc_ul_fifo_disable_pl1_access();

      /* parameter contains the new coding scheme
      */
      change_coding_scheme( ul.cur_res.cs);

      /* Re-enable PL1 access to the FIFO
      */
      grlc_ul_fifo_enable_pl1_access();

      /* update the working coding scheme
      */
      ul.last_valid_mcs = ul.cur_res.cs;
    }
    else
    {
      /* For PSHO , target cell may support USF granularity
      */
      ul.cur_res.usf_granularity = ul_alloc_cnf->usf_granularity;

      /* Enable PL1 access to FIFO (for PSHO)
      */
      grlc_ul_fifo_enable_pl1_access();

      /* Even though there is nothing to do from an RLC perspective because
      ** the coding scheme hasn't actually changed, GPL1 will be expecting
      ** a PH_DATA_REQ to unblock the UL FIFO.
      */
      pl1_send_ph_data_req();
    }

    if (ul.cur_res.ws < ul_alloc_cnf->ws)
    {
      /* Since WS can't be downsized, the new WS has to be larger. So check
      ** if the current window is stalled and if so un-stall it
      */
      if (ul.si == RM_TX_STALLED)
      {
        ul.si = RM_TX_NOT_STALLED;
        GRLC_UL_SHOW_TX_VARS();

        /* No re-processing apart from re-enabling PL1
        */
        ul.b_first_fifo_load = TRUE;
        grlc_ul_fifo_enable_pl1_access();
        pl1_send_ph_data_req();
      }

      ul.cur_res.ws = ul_alloc_cnf->ws;

      /* WS has widened. Stop T3182
      */
      TIMER_STOP_T3182();
    }
  } /* EGPRS mode */
  else
  {
    /* EGPRS build GPRS mode
    */
    grlc_ul_show_new_alloc( &ul.cur_res, ul_alloc_cnf);
    MSG_GERAN_LOW_3_G("GRLU U ALLOC_CNF #TS=%d cs=%d typ=%d",
            ul_alloc_cnf->num_timeslots,
            ul_alloc_cnf->cs,
            ul_alloc_cnf->tbf_type);

    /* Update coding scheme for engg_mode
    */
    if (ul.cur_res.cs != ul_alloc_cnf->cs)
    {
      geran_eng_mode_data_write(ENG_MODE_GPRS_UL_CS, (void*)(&ul_alloc_cnf->cs), TRUE);
    }

    /* Check for changes in CS/TFI/CS
    */
    if (ul.cur_res.cs != ul_alloc_cnf->cs                        ||
        ul.cur_res.num_timeslots != ul_alloc_cnf->num_timeslots  ||
        ul.cur_res.ul_tfi != ul_alloc_cnf->ul_tfi
       )
    {
      MSG_GERAN_MED_2_G("GRLU U UnsolAss old TS=%d new TS=%d",
              ul.cur_res.num_timeslots,
              ul_alloc_cnf->num_timeslots
              );
      ul.cur_res = *ul_alloc_cnf;

      /* Assign tilli_cs the coding scheme in one-phase access mode.
      */
      update_tlli_cs();

      /* Freeze FIFO access for reprocessing
      */
      grlc_ul_fifo_disable_pl1_access();

      /* parameter contains the new coding scheme
      */
      change_coding_scheme( ul.cur_res.cs);

      /* Re-enable PL1 access to the FIFO
      */
      grlc_ul_fifo_enable_pl1_access();
    }
    else
    {
      /* For PSHO , target cell may support USF granularity
      */
      ul.cur_res.usf_granularity = ul_alloc_cnf->usf_granularity;

      /* Enable PL1 access to FIFO (for PSHO)
      */
      grlc_ul_fifo_enable_pl1_access();

      /* Even though there is nothing to do from an RLC perspective because
      ** the coding scheme hasn't actually changed, GPL1 will be expecting
      ** a PH_DATA_REQ to unblock the UL FIFO.
      */
      pl1_send_ph_data_req();
    }
  } /* EGPRS build GPRS mode */
} /* grlc_ul_handle_alloc_cnf_unack_xfer */


#ifdef  FEATURE_GPRS_PS_HANDOVER

/*===========================================================================
===
===  FUNCTION      GRLC_UL_START_XFER()
===
===  DESCRIPTION
===    Load current PDU and start segmentation. Load up UL FIFO and send
===    PH_DATA_REQ to PL1. Start transfer according to ack/unack mode and
===    TBF mode
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_start_xfer
(
  mac_rlc_msg_t *mac_msg_ptr
)
{
  /* only if it's active
  */
  TIMER_STOP_MAC_UL_RESP();

  /* Force open-ended if given close-ended
  */
  if (mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf.tbf_type == RM_CLOSE_ENDED_TBF)
  {
    mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf.tbf_type = RM_OPEN_ENDED_TBF;
    MSG_GERAN_ERROR_0_G("GRLU close-ended converted to open-ended");
  }

  (void)load_cur_pdu();

  /* store info returned by MAC tbf est cnf.
  */
  grlc_ul_show_alloc_cnf_await_ass( &mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf);

  if (ul.state != US_ACK_XFER &&
      ul.state != US_UNACK_XFER
     )
  {
    /* If the RLC_RESET field indicates that any given RLC entity is
    ** not reset across PS handover then the mobile station shall
    ** ignore this field and use the same RLC mode that was used for
    ** the corresponding PFC in the old cell.
    */
    if ( cur_pdu_ptr != NULL           &&
         mac_msg_ptr->msg.ul_psho_complete_ind.ul_rlc_mode != cur_pdu_ptr->rlc_mode
       )
    {
      MSG_GERAN_ERROR_2_G("GRLU PSHO ##rlc_mode diff in cur_pdu rlc_mode =%u ass_rlc_mode=%u",
         cur_pdu_ptr->rlc_mode,
         mac_msg_ptr->msg.ul_psho_complete_ind.ul_rlc_mode);
    }

    ul.rlc_mode = mac_msg_ptr->msg.ul_psho_complete_ind.ul_rlc_mode;

    ul.state = (rlc_ul_state_t)((ul.rlc_mode == GRLC_MODE_ACK) ?
                              US_ACK_XFER :
                              US_UNACK_XFER
                             );

    if (cur_pdu_ptr != NULL)
    {
      ul.b_psho_llc_pdu_absent = FALSE;
      /* accept assignment from mac for a new uplink tbf
      */
      grlc_ul_handle_alloc_cnf_await_ass();
    }
    else
    {
      ul.b_psho_llc_pdu_absent = TRUE;
    }
  } /* US_STATE = US_NULL */
  else
  {
    /* US_STATE is XFER
    */
    if (ul.state == US_ACK_XFER )
    {
      grlc_ul_handle_alloc_cnf_ack_xfer( &mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf);
    }
    else
    {
      grlc_ul_handle_alloc_cnf_unack_xfer( &mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf);
    }
  } /* US_STATE is XFER */

  /* Re-assess flow control
  */
  grlc_llc_ul_flow_control();

} /* grlc_ul_start_xfer */

#endif /* FEATURE_GPRS_PS_HANDOVER */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_MAC_EVENT_US_NULL()
===
===  DESCRIPTION
===
===    Handler for MAC signals in US_NULL state.
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_handle_mac_event_us_null (void)
{
  mac_rlc_msg_t *mac_msg_ptr;
#ifdef FEATURE_GSM_DTM
  uint16        psn=0xffff;
#endif /*  FEATURE_GSM_DTM */

  mac_msg_ptr = (mac_rlc_msg_t *)q_get(&UMQ());

  if (mac_msg_ptr == NULL)
  {
    MSG_GERAN_LOW_0_G("GRLU US_NULL event without msg");
  }
  else
  {
    if (mac_msg_ptr->msg.sig == MR_UL_RESET_REQ)
    {
      grlc_ul_panic_reset();
    }
    else

    if (mac_msg_ptr->msg.sig == MR_NO_PS_ACCESS_IND)
    {
      grlc_ul_handle_no_ps_access( mac_msg_ptr->msg.no_ps_access_ind.delete_pdu_flag);
      MSG_GERAN_MED_1_G("GRLU NULL cur_access=%u no-access", ul.b_access);
    }
    else
    if (mac_msg_ptr->msg.sig == MR_PS_ACCESS_REQ)
    {
#ifdef FEATURE_GPRS_PS_HANDOVER
      if (ul.b_psho_active)
      {
        MSG_GERAN_MED_0_G("GRLU NULL PSHO ignore access-ind in PSHO");
      }
      else
      {
        grlc_ul_handle_ps_access( mac_msg_ptr->msg.ps_access_req.action);
        MSG_GERAN_MED_1_G("GRLU NULL cur_access=%u access", ul.b_access);
      }
#else
      /* This is to prevent race condition
      */
      grlc_ul_handle_ps_access( mac_msg_ptr->msg.ps_access_req.action);
      MSG_GERAN_LOW_1_G("GRLU NULL cur_access=%u access", ul.b_access);
#endif /* FEATURE_GPRS_PS_HANDOVER  */
    }
#ifdef  FEATURE_GSM_DTM
    else
    if (mac_msg_ptr->msg.ul_release_ind.sig == MR_UL_RELEASE_IND &&
        mac_msg_ptr->msg.ul_release_ind.cause == REL_UL_NORMAL
       )
    {
      /* Successful GTTP transfer: clear PDU, loads the next PDU.
      ** If the next PDU exists make request .. else goto US_NULL
      */
      if (cur_pdu_ptr != NULL)
      {
        psn = cur_pdu_ptr->psn;
        /* To transfer grouping info to the next PDU
        */
        ungroup_pdu( cur_pdu_ptr);

        /* Successful transmission of PDU , set the flag to TRUE
        ** to start ready timer
        */
        grlc_ul_del_free_paq_upto_psn( psn,TRUE);
      }

      MSG_GERAN_MED_1_G("GRLU GTTP psn=%d",psn);

      clr_ul_data();
      grlc_ul_unput_paq();

      ul.state = US_NULL;
      UPD_UL_STATE();

      /* Consider the TBF released. Now load the next PDU and establish new TBF
      ** otherwise go back to NULL
      */
      (void)load_cur_pdu();

      ul.b_access = FALSE;
    }
#endif /*  FEATURE_GSM_DTM */
#ifdef  FEATURE_GPRS_PS_HANDOVER
    else
    if (mac_msg_ptr->msg.sig == MR_UL_PSHO_START_IND)
    {
      if (ul.b_psho_active)
      {
        MSG_GERAN_ERROR_0_G("GRLU NULL PSHO_START whilst ALREADY active");
      }

      ul.b_psho_active = TRUE;
      ul.b_access = FALSE;
      MSG_GERAN_MED_0_G("GRLU NULL PSHO_START access is now FALSE");

      TIMER_START_AWAIT_ASS_GUARD();
    }
    else
    if (mac_msg_ptr->msg.sig == MR_UL_PSHO_COMPLETE_IND)
    {
      if (!ul.b_psho_active)
      {
        MSG_GERAN_ERROR_0_G("GRLU NULL PSHO_COMPLETE whilst NOT active");
      }

      TIMER_STOP_AWAIT_ASS_GUARD();

      MSG_GERAN_MED_3_G("GRLU A PSHO_COMPLETE succ = %u type = %u rlc_mode=%u",
        mac_msg_ptr->msg.ul_psho_complete_ind.psho_status,
        mac_msg_ptr->msg.ul_psho_complete_ind.psho_type,
        mac_msg_ptr->msg.ul_psho_complete_ind.ul_rlc_mode);

      /* Success
      */
      if (mac_msg_ptr->msg.ul_psho_complete_ind.psho_status == PSHO_SUCCESS)
      {
        MSG_GERAN_MED_0_G("GRLU NULL PSHO_COMPLETE success");

        /* Refresh sysinfo during transfer
        */
        read_ul_sysinfo();

        /* set this counter assuming that cell reselection had just taken place
        */
        SET_N3102();
#ifdef  GSM_GPRS_GRLU_SHOW_N3102
        MSG_GERAN_MED_3_G("GRLU OTInit SetN3102=max=%d inc=%d dec=%d",ul.n3102,PAN_INC(),PAN_DEC());
#endif
        /*----------------------------------------
        ** Consider PSHO ended, and resume access
        ** Process UL assignment.
        **----------------------------------------
        */
        ul.b_psho_active = FALSE;
        ul.b_access = TRUE;

        /* Promote RAU/Cell-Update PDU(s) to the front of the queue
        */
        grlc_ul_promote_sig_pdus();

        /* Move all PDUs from temp to main queue
        */
        grlc_ul_handle_llc_tmp_pdu_sig_dyn();

        ul.cur_res = mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf;

        if (mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf.tbf_mode == TBF_MODE_EGPRS)
        {
          SET_TBF_MODE_EGPRS();
          ul.cur_res.cs = mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf.mcs;

          /* Start of a EDGE UL TBF from IDLE
          ** Allocated memory for TX_ARR ,Error handling in allocation func.
          */
          grlc_ul_create_tx_array (  E_TX_WORKSPACE );

          /* Update coding scheme for engg_mode
          */
          geran_eng_mode_data_write(ENG_MODE_EGPRS_UL_MCS, &ul.cur_res.cs, TRUE);
        }
        else
        {
          SET_TBF_MODE_GPRS();
          ul.cur_res.cs = mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf.cs;

          /* Start of a GPRS UL TBF from IDLE
          ** Allocated memory for TX_ARR ,Error handling in allocation func.
          */
          grlc_ul_create_tx_array (  RM_RLC_TX_WORKSPACE );

          /* Update coding scheme for engg_mode
          */
          geran_eng_mode_data_write(ENG_MODE_GPRS_UL_CS, &ul.cur_res.cs, TRUE);
        }

        grlc_ul_start_xfer( mac_msg_ptr);

        /* Enable PL1 access to FIFO (for PSHO)
        */
        grlc_ul_fifo_enable_pl1_access();

        /* Even though there is nothing to do from an RLC perspective because
        ** the coding scheme hasn't actually changed, GPL1 will be expecting
        ** a PH_DATA_REQ to unblock the UL FIFO.
        */
        pl1_send_ph_data_req();
      } /* PSHO success in NULL */
      else if (mac_msg_ptr->msg.ul_psho_complete_ind.psho_status == PSHO_FAILURE)
      {
        ul.b_psho_active = FALSE;
        ul.b_access = FALSE;

        /* Promote RAU/Cell-Update PDU(s) to the front of the queue
        */
        grlc_ul_promote_sig_pdus();

        /* Move all PDUs from temp to main queue
        */
        grlc_ul_handle_llc_tmp_pdu_sig_dyn();

        MSG_GERAN_MED_0_G("GRLU NULL PSHO_COMPLETE failure");
      } /* PSHO fail in NULL */
    } /* PSHO_COMLPETE in NULL */
#endif /* FEATURE_GPRS_PS_HANDOVER */
    else
    {
      MSG_GERAN_LOW_1_G("GRLU NULL Unexp MAC sig %d", mac_msg_ptr->msg.sig);
    }

    GPRS_MEM_FREE(mac_msg_ptr);
  } /* end MAC msg not NULL */

} /* grlc_ul_handle_mac_event_us_null */


/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_MAC_EVENT_US_AWAIT_ASS()
===
===  DESCRIPTION
===
===    Handler for MAC signals in US_AWAIT_ASS state.
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_handle_mac_event_us_await_ass (void)
{
  mac_rlc_msg_t *mac_msg_ptr;

  mac_msg_ptr = (mac_rlc_msg_t *)q_get(&UMQ());

  if (mac_msg_ptr == NULL)
  {
    MSG_GERAN_LOW_0_G("GRLU AWAIT_ASS MAC event without msg");
  }
  else
  {
    /*----------------------------------------------------------------------------
    ** If the ul tbf is getting established for Test Mode B or SRB, then the only
    ** valid signal in AWAIT_ASS is MR_UL_ALLOC_CNF. So, emit message and
    ** drop through ..
    **----------------------------------------------------------------------------
    */
    if( ((ul.test_mode == TEST_MODE_B) || (ul.test_mode == TEST_MODE_SRB)) &&
        (mac_msg_ptr->msg.sig != MR_UL_ALLOC_CNF)   )
    {
      MSG_GERAN_LOW_1_G("GRLU Test Mode in US_AWAIT_ASS Rcvd Unexp sig %d , send stop test_mode to LLC",
        mac_msg_ptr->msg.sig);

      ul.b_access = FALSE;

      /* inform llc end of test mode */
      (void)gllc_grr_ul_tbf_rel_ind();

      /* Clear data structures relating to current TBF.
      ** Clear tx window, clear all resources granted,
      ** keep current and outstanding pdus, clear all
      ** signal masks
      */
      clr_ul_data();

      /* Back to NULL state */
      ul.state = US_NULL;

      /* End of UL tbf for Test mode */
      ul.test_mode = TEST_MODE_OFF;
    }

    if (mac_msg_ptr->msg.sig == MR_UL_RESET_REQ)
    {
      grlc_ul_panic_reset();
    }
    else
    /*---------------------------------------------------------
    ** State: US_AWAIT_ASS, event: MAC
    ** Handle ALLOC_CNF by storing resource and processing PDU
    **---------------------------------------------------------
    */
    if (mac_msg_ptr->msg.sig == MR_UL_ALLOC_CNF)
    {
      /* Force open-ended if given close-ended
      */
      if (mac_msg_ptr->msg.ul_alloc_cnf.tbf_type == RM_CLOSE_ENDED_TBF)
      {
        mac_msg_ptr->msg.ul_alloc_cnf.tbf_type = RM_OPEN_ENDED_TBF;
        MSG_GERAN_LOW_0_G("GRLU close-ended converted to open-ended");
      }

      TIMER_STOP_AWAIT_ASS_GUARD();

#ifdef  FEATURE_GSM_DTM
      if (mac_msg_ptr->msg.ul_alloc_cnf.dtm_access)
      {
        dsm_item_type *temp_dup_ptr;

        if(cur_pdu_ptr != NULL )
        {
          /* Create another dup item for the current PDU
          */
          temp_dup_ptr = gprs_pdu_duplicate(cur_pdu_ptr->dup_pdu_ptr,
                                            0,
                                            cur_pdu_ptr->len
                                           );

          if (temp_dup_ptr == NULL)
          {
            MSG_GERAN_ERROR_0_G("GRLU AWAIT_ASS GTTP ##dup returns NULL");
            grlc_ul_show_vars();

            /* Failed to create dup'ed packet. Set flag to release at the appropriate
            ** fn level
            */
            ul.b_error_recovery_required = TRUE;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
            ul_diag_stats_st.ul_gen_purpose_count_a++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

            /* To break out as we are not in GPRS/EDGE
            */
          }
          else
          {
            /* Enter GPRS transparent transfer protocol state to wait the
            ** confirmation from GRR via MAC
            */
            MSG_GERAN_MED_3_G("GRLU AWAIT_ASS send PDU via GRR: PSN %d LEN %d TLLI %08x",
                     cur_pdu_ptr->psn,
                     cur_pdu_ptr->len,
                     cur_pdu_ptr->tlli
                   );

            gttp_send_llc_pdu_to_rr(cur_pdu_ptr->tlli, temp_dup_ptr);
            ul.state = US_GTTP_XFER;
          } /* dup operation OK */
        }
        else
        {
          MSG_GERAN_LOW_0_G("GRLU AWAIT_ASS ##cur_pdu_ptr NULL on entering into GTTP_XFER");
          grlc_ul_show_vars();
          ul.b_error_recovery_required = TRUE;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
          ul_diag_stats_st.ul_gen_purpose_count_a++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
        }

        GPRS_MEM_FREE(mac_msg_ptr);

        /* Must exit at this point as we are not in GPRS/EGPRS
        */
        return;

      } /* handle dtm access */

#endif /* FEATURE_GSM_DTM */

      /* If EDGE is supported and it's an EGPRS assignment then update our
      ** unique flag to indicate we are in EGPRS mode
      */
      ul.cur_res = mac_msg_ptr->msg.ul_alloc_cnf;

      if (mac_msg_ptr->msg.ul_alloc_cnf.tbf_mode == TBF_MODE_EGPRS)
      {
        SET_TBF_MODE_EGPRS();
        ul.cur_res.cs = mac_msg_ptr->msg.ul_alloc_cnf.mcs;

        /* Start of a EDGE UL TBF from IDLE
        ** Allocated memory for TX_ARR ,Error handling in allocation func.
        */
        grlc_ul_create_tx_array (  E_TX_WORKSPACE );

        /* Update coding scheme for engg_mode
        */
        geran_eng_mode_data_write(ENG_MODE_EGPRS_UL_MCS, &ul.cur_res.cs, TRUE);
      }
      else
      {
        SET_TBF_MODE_GPRS();
        ul.cur_res.cs = mac_msg_ptr->msg.ul_alloc_cnf.cs;

        /* Start of a GPRS UL TBF from IDLE
        ** Allocated memory for TX_ARR ,Error handling in allocation func.
        */
        grlc_ul_create_tx_array (  RM_RLC_TX_WORKSPACE );

        /* Update coding scheme for engg_mode
        */
        geran_eng_mode_data_write(ENG_MODE_GPRS_UL_CS, &ul.cur_res.cs, TRUE);
      }

      /* only if it's active
      */
      TIMER_STOP_MAC_UL_RESP();

      /* store info returned by MAC tbf est cnf.
      */
      grlc_ul_show_alloc_cnf_await_ass( &mac_msg_ptr->msg.ul_alloc_cnf);

      if( ul.test_mode == TEST_MODE_OFF )
      {
        grlc_ul_handle_alloc_cnf_await_ass();
      } /* ul.test_mode == TEST_MODE_OFF */
      else if( (ul.test_mode == TEST_MODE_B) || (ul.test_mode == TEST_MODE_SRB) )
      {
        MSG_GERAN_LOW_0_G("GRLU Est UL tbf for Test Mode");

        /* Move to state US_TEST_MODE while Test Mode B or SRB is active */
        ul.state = US_TEST_MODE;

        /* ! set uo flag to send PH_DATA_REQ to L1 on the
        ** first data block of this TBF
        */
        ul.b_first_fifo_load = TRUE;

      } /* ul.test_mode == TEST_MODE_B */
      else
      {
        MSG_GERAN_LOW_1_G("GRLU Invalid Test Mode %d",ul.test_mode);
      }

      /* Re-assess flow control
      */
      grlc_llc_ul_flow_control();

    } /* ALLOC_CNF */

    else
    if (mac_msg_ptr->msg.sig == MR_NO_PS_ACCESS_IND)
    {
      MSG_GERAN_MED_1_G("GRLC AWAIT_ASS no-access flag=%u",
                mac_msg_ptr->msg.no_ps_access_ind.delete_pdu_flag);
      grlc_ul_handle_no_ps_access( mac_msg_ptr->msg.no_ps_access_ind.delete_pdu_flag);
    }
#ifdef  FEATURE_GPRS_PS_HANDOVER
    else
    if (mac_msg_ptr->msg.sig == MR_UL_PSHO_START_IND)
    {
      if (ul.b_psho_active)
      {
        MSG_GERAN_ERROR_0_G("GRLU AWAIT_ASS PSHO_START whilst ALREADY active");
      }

      /*--------------------------------------------------------------------------------
      ** If MAC sends PSHO_START when RLC sends TBF_REQ, MAC shall ignore RLC's request
      ** so RLC should come out of AWAIT_ASS and return to NULL and request TBF later
      **--------------------------------------------------------------------------------
      */
      MSG_GERAN_MED_0_G("GRLC AWAIT_ASS PSHO_START");
      ul.b_psho_active = TRUE;
      ul.b_access = FALSE;

      TIMER_START_AWAIT_ASS_GUARD();
      ul.state = US_NULL;
      clr_ul_data();
      grlc_ul_unput_paq();
    }
#endif /* FEATURE_GPRS_PS_HANDOVER  */
    else
    if (mac_msg_ptr->msg.sig == MR_PS_ACCESS_REQ)
    {
      MSG_GERAN_LOW_1_G("GRLU AWAIT_ASS action on ps_access=%u",
                mac_msg_ptr->msg.ps_access_req.action);
      grlc_ul_handle_ps_access( mac_msg_ptr->msg.ps_access_req.action);
    }
    else
    /*------------------------------------------
    ** State: US_AWAIT_ASS, Un-expected message
    **------------------------------------------
    */
    {
      /* Unexpected, so get it off the queue and free it
      */
      MSG_GERAN_LOW_1_G("GRLU AWAIT_ASS Unexp MAC sig %d",mac_msg_ptr->msg.sig);
    }

    GPRS_MEM_FREE(mac_msg_ptr);
  } /* end MAC sig not NULL */

} /* grlc_ul_handle_mac_event_us_await_ass */


#ifdef  FEATURE_GPRS_PS_HANDOVER

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_PSHO_COMPLETE_IN_XFER()
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_handle_psho_complete_in_xfer
(
  mac_rlc_msg_t *mac_msg_ptr
)
{
  MSG_GERAN_MED_3_G("GRLU A PSHO_COMPLETE type = %u reset_flag=%u rlc_mode=%u",
    mac_msg_ptr->msg.ul_psho_complete_ind.psho_type,
    mac_msg_ptr->msg.ul_psho_complete_ind.reset_flag,
    mac_msg_ptr->msg.ul_psho_complete_ind.ul_rlc_mode);

  if (mac_msg_ptr->msg.ul_psho_complete_ind.psho_status == PSHO_SUCCESS)
  {
    if (mac_msg_ptr->msg.ul_psho_complete_ind.psho_type == PSHO_GTOW)
    {
      clr_ul_data();

      ul.b_access = FALSE;

      grlc_ul_unput_paq();

      /* No STATUS_IND sent if SM PDU is deleted, hence FALSE. */

      flush_llc_pdu_queue( FALSE);

      ul.state = US_NULL;
    }
    else if (mac_msg_ptr->msg.ul_psho_complete_ind.psho_type == PSHO_GTOG)
    {
      /* Refresh sysinfo during transfer
      */
      read_ul_sysinfo();

      /* set this counter assuming that cell re selection had just taken place
      */
      SET_N3102();
#ifdef  GSM_GPRS_GRLU_SHOW_N3102
      MSG_GERAN_MED_3_G("GRLU OTInit SetN3102=max=%d inc=%d dec=%d",ul.n3102,PAN_INC(),PAN_DEC());
#endif

      if (mac_msg_ptr->msg.ul_psho_complete_ind.reset_flag)
      {
        /* Clear UL resources, unput PAQ
        */
        clr_ul_data();
        grlc_ul_unput_paq();

        /* Promote RAU/Cell-Update PDU(s) to the front of the queue
        */
        grlc_ul_promote_sig_pdus();

        /* Take UL assignment
        */
        ul.cur_res = mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf;

        if (mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf.tbf_mode == TBF_MODE_EGPRS)
        {
          SET_TBF_MODE_EGPRS();
          ul.cur_res.cs = mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf.mcs;

          /* Start of a EDGE UL TBF from IDLE
          ** Allocated memory for TX_ARR ,Error handling in allocation func.
          */
          grlc_ul_create_tx_array (  E_TX_WORKSPACE );

          /* Update coding scheme for engg_mode
          */
          geran_eng_mode_data_write(ENG_MODE_EGPRS_UL_MCS, &ul.cur_res.cs, TRUE);
        }
        else
        {
          SET_TBF_MODE_GPRS();
          ul.cur_res.cs = mac_msg_ptr->msg.ul_psho_complete_ind.ul_alloc_cnf.cs;

          /* Start of a GPRS UL TBF from IDLE
          ** Allocated memory for TX_ARR ,Error handling in allocation func.
          */
          grlc_ul_create_tx_array (  RM_RLC_TX_WORKSPACE );

          /* Update coding scheme for engg_mode
          */
          geran_eng_mode_data_write(ENG_MODE_GPRS_UL_CS, &ul.cur_res.cs, TRUE);
        }
      }
      /* Move all PDUs from temp to main queue
      */
      grlc_ul_handle_llc_tmp_pdu_sig_dyn();

      grlc_ul_start_xfer( mac_msg_ptr);
    }
    else
    {
      MSG_GERAN_HIGH_1_G("GRLU A PSHO_COMPLETE success bad type %u",
        mac_msg_ptr->msg.ul_psho_complete_ind.psho_type);
    }
  }
  else if (mac_msg_ptr->msg.ul_psho_complete_ind.psho_status == PSHO_FAILURE)
  {
    /* Move all PDUs from temp to main queue
    */
    grlc_ul_handle_llc_tmp_pdu_sig_dyn();

    /* Enable PL1 access to FIFO (for PSHO)
    */
    grlc_ul_fifo_enable_pl1_access();

    /* Even though there is nothing to do from an RLC perspective because
    ** the coding scheme hasn't actually changed, GPL1 will be expecting
    ** a PH_DATA_REQ to unblock the UL FIFO.
    */
    pl1_send_ph_data_req();
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GRLU A PSHP_COMPLETE bad status %u",
      mac_msg_ptr->msg.ul_psho_complete_ind.psho_status);
  }

} /* grlc_ul_handle_psho_complete_in_xfer */

#endif /* FEATURE_GPRS_PS_HANDOVER  */
/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_MAC_EVENT_US_ACK_XFER()
===
===  DESCRIPTION
===
===    Handler for MAC signals in US_ACK_XFER state.
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_handle_mac_event_us_ack_xfer (void)
{
  mac_rlc_msg_t *mac_msg_ptr;
  uint16        psn;

  mac_msg_ptr = (mac_rlc_msg_t *)q_get(&UMQ());

  if (mac_msg_ptr == NULL)
  {
    MSG_GERAN_LOW_0_G("GRLU A MAC event without msg");
  }
  else
  {
    if (mac_msg_ptr->msg.sig == MR_UL_RESET_REQ)
    {
      grlc_ul_panic_reset();
    }
    else
    if (mac_msg_ptr->msg.sig == MR_UL_PKT_ACKNACK_IND)
    {
      if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        e_show_rbb( &mac_msg_ptr->msg.ul_pkt_acknack_ind.egprs_acknack_desc,
                    mac_msg_ptr->msg.ul_pkt_acknack_ind.egprs_acknack_desc.fai);
      }
      else
      {
        show_rbb( &mac_msg_ptr->msg.ul_pkt_acknack_ind.acknack_desc,
                    mac_msg_ptr->msg.ul_pkt_acknack_ind.acknack_desc.fai);
      }

      /* If in one-phase and Contention Unresolved then reset N3104 counter
      ** as this paknak will resolve it
      */
      if (IS_TLLI_REQUIRED())
      {
        RESET_N3104();
#ifdef  GSM_GPRS_GRLU_SHOW_N3104
        MSG_GERAN_MED_2_G("GRLU A ResN3104=%d max=%d",ul.n3104, N3104_MAX());
#endif
      }

      /*----------------------------------------------------------------------
      ** Handle UL acknack in EGPRS mode
      **----------------------------------------------------------------------
      */
      if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        ul.egprs_acknack_desc = mac_msg_ptr->msg.ul_pkt_acknack_ind.egprs_acknack_desc;
        ul.cur_res.resegment = mac_msg_ptr->msg.ul_pkt_acknack_ind.resegment;
        ul.pre_emptive_tx = mac_msg_ptr->msg.ul_pkt_acknack_ind.pre_emptive_tx;
        MSG_GERAN_MED_3_G("ERLU A EPUAN pre_tx=%u resegment=%u tfi=%u",
                mac_msg_ptr->msg.ul_pkt_acknack_ind.pre_emptive_tx,
                mac_msg_ptr->msg.ul_pkt_acknack_ind.resegment,
                mac_msg_ptr->msg.ul_pkt_acknack_ind.tfi);
        ul.acknack_blk_cnt = mac_msg_ptr->msg.ul_pkt_acknack_ind.blk_cnt;

        if (ul.egprs_acknack_desc.fai == RM_ACKNACK_FAI_TBF_COMPLETE)
        {
          ul.b_access = FALSE;

          TIMER_STOP_T3182();

          TIMER_STOP_AWAIT_UL_ACKNACK();
          MSG_GERAN_HIGH_0_G("ERLU FAI XTimer stopped");
          ul.guard_timer_state = GUARD_OFF;

          /* To transfer grouping info to the next PDU
          */
          ungroup_pdu( cur_pdu_ptr);

          /* EGPRS mode EXT_UTBF: interpret SSN and RRB
          */
          if (IS_EXT_UTBF_ACTIVE())
          {
            /* Freeze FIFO access for reprocessing */
            grlc_ul_fifo_disable_pl1_access();

            grlc_ul_fifo_delete_unread_nack_pack_blocks( &ul.nack_vsf,
                                                        &ul.pack_vsf);

            /* Re-enable PL1 access to the FIFO */
            grlc_ul_fifo_enable_pl1_access();

            UPD_UL_NACK_VSF();
            UPD_UL_PACK_VSF();

            /* (VA-1) holds the final ack block */
            e_update_ack_state_array();

            psn = grlc_ul_paq_seek_psn_cv0( ul.va);
          }
          else
          {
            /* Ignore bitmap and free PDUs and LLC messages upto VS or CV=0 */
            psn = grlc_ul_paq_seek_psn_cv0( ul.vs);
          }

          clr_ul_data();

          /* Successful transmission of PDU , set the flag to TRUE
          ** to start ready timer
          */
          grlc_ul_del_free_paq_upto_psn( psn,TRUE);
          grlc_ul_unput_paq();

          ul.state = US_NULL;
          UPD_UL_STATE();

          /* Consider the TBF released. Now load the next PDU and establish new TBF
          ** otherwise go back to NULL
          */
          (void)load_cur_pdu();
        } /* FAI complete */
        else
        {
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
          uint16 va_last_puan = ul.va;
#endif /* FEATURE_GERAN_POWER_EFFICIENT_OPERATION */

          /* FAI=0 so update bitmap, check for ChCS and carry on */

          /* Re-process the UL FIFO as paknak will have made some blocks
          ** invalid. So delete the un-read blocks in the FIFO and rewind the
          ** transmit vars back to the BSN value of the oldest blocks
          ** (pack_vsf, nack_vsf)
          */

          /* Freeze FIFO access for reprocessing */
          grlc_ul_fifo_disable_pl1_access();

          grlc_ul_fifo_delete_unread_nack_pack_blocks( &ul.nack_vsf, &ul.pack_vsf);

          /* Re-enable PL1 access to the FIFO */
          grlc_ul_fifo_enable_pl1_access();

          UPD_UL_NACK_VSF();
          UPD_UL_PACK_VSF();

          e_update_ack_state_array();

          /* EGPRS RA_AT_TX_COMPLETE_CHANGE_MODE type 3 */
          if (IS_EXT_UTBF_ACTIVE() &&
              cur_pdu_ptr->realloc_action == RA_AT_TX_COMPLETE_CHANGE_MODE &&
              ul.realloc_state != RALS_COMPLETE &&
              IS_TX_COMPLETE() &&
              ul.va == ul.vs   &&
              next_pdu_ptr != NULL
             )
          {
            /* Send PRR with parameters from the next PDU */
            ul.realloc_pdu_ptr = next_pdu_ptr;
            ul.realloc_psn = next_pdu_ptr->psn;

            /* b_cv_zero - is set to TRUE , because we
             have transitioned to
             realloc_state = RA_AT_TX_COMPLETE_CHANGE_MODE,
             that means we have Data in our Uplink Q.
            */
            realloc_for_next_pdu( ul.realloc_pdu_ptr,FALSE);

            /* Set RALS_COMPLETE once after sending a PRR.
            ** Expects this realloc_state to change when the next current PDU is loaded
            */
            ul.realloc_state = RALS_COMPLETE;

            MSG_GERAN_LOW_1_G("ERLU prr TX_COMPLETED psn=%u", next_pdu_ptr->psn);
          }

          /* FAI = 0. Window has been un-stalled (not full) then stop TIMER
          */
          if (!IS_E_TX_WINDOW_FULL())
          {
            TIMER_STOP_T3182();
          }

          /* if there is abort from layer,pack fifo can be flushed.
          ** CR: 125334
          */
          ul.b_puan = TRUE;

          /* Check and handle change of MCS
          */
          if (ul.cur_res.cs != mac_msg_ptr->msg.ul_pkt_acknack_ind.mcs )
          {
            /* Freeze FIFO access for reprocessing
            */
            grlc_ul_fifo_disable_pl1_access();

            /* parameter contains the new coding scheme
            */
            change_coding_scheme( mac_msg_ptr->msg.ul_pkt_acknack_ind.mcs);

            /* Re-enable PL1 access to the FIFO
            */
            grlc_ul_fifo_enable_pl1_access();

            /* update the 'last' working coding scheme
            */
            ul.last_valid_mcs = ul.cur_res.cs;

            /* Update coding scheme for engg_mode
            */
            geran_eng_mode_data_write(ENG_MODE_EGPRS_UL_MCS, &ul.cur_res.cs, TRUE);

          }
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
         /*
          * FAI = 0 and check if no more blocks to transmit. If EXT_UL_TBF
          * is enabled, indicate GMAC for start of EXT_UL_TBF period 
          */
          if (IS_EXT_UTBF_ACTIVE() &&
              ul.va == ul.vs &&
              ul.vs == ul.seg_ind &&
              ul.va != va_last_puan
             )
          {
            grlc_ul_ext_tbf_start_ind();
          }
#endif /* FEATURE_GERAN_POWER_EFFICIENT_OPERATION */
        } /* end FAI not set */

        process_guard_timer();

        /* Log EPUAN
        ** Log Code: 0x5209
        */
        rlc_log_epuan( &(mac_msg_ptr->msg.ul_pkt_acknack_ind));

      } /* TBF_MODE_EGPRS */
      else
      {
        /*--------------------------------------------------------------
        ** Handle ul acknack in GPRS mode with EGPRS support, ie. using
        ** the old code with the GPRS union reference.
        **--------------------------------------------------------------
        */

        /* store the latest packet uplink acknack message (with blk_cnt)
        ** prior to looking at coding scheme change
        */
        ul.ul_acknack = mac_msg_ptr->msg.ul_pkt_acknack_ind.acknack_desc;
        ul.acknack_blk_cnt = mac_msg_ptr->msg.ul_pkt_acknack_ind.blk_cnt;

        if (ul.ul_acknack.fai == RM_ACKNACK_FAI_TBF_COMPLETE)
        {
          ul.b_access = FALSE;

          TIMER_STOP_T3182();

          MSG_GERAN_HIGH_0_G("GRLU FAI XTimer stopped");
          TIMER_STOP_AWAIT_UL_ACKNACK();
          ul.guard_timer_state = GUARD_OFF;

          /* To transfer grouping info to the next PDU
          */
          ungroup_pdu( cur_pdu_ptr);

          /* GPRS mode EXT_UTBF: interpret SSN and RRB
          */
          if (IS_EXT_UTBF_ACTIVE())
          {
            /* Freeze FIFO access for reprocessing */
            grlc_ul_fifo_disable_pl1_access();

            grlc_ul_fifo_delete_unread_nack_pack_blocks( &ul.nack_vsf,
                                                        &ul.pack_vsf);

            /* Re-enable PL1 access to the FIFO */
            grlc_ul_fifo_enable_pl1_access();

            UPD_UL_NACK_VSF();
            UPD_UL_PACK_VSF();

            /* (VA-1) holds the final ack block */
            update_ack_state_array();
            rlc_log_ul_acknack_params();

            psn = grlc_ul_paq_seek_psn_cv0( ul.va);
          }
          else
          {
            /* Ignore bitmap and free PDUs and LLC messages upto VS or CV=0 */
            psn = grlc_ul_paq_seek_psn_cv0( ul.vs);
          }
          clr_ul_data();

          /* Successful transmission of PDU , set the flag to TRUE
          ** to start ready timer
          */
          grlc_ul_del_free_paq_upto_psn( psn,TRUE);
          grlc_ul_unput_paq();

          ul.state = US_NULL;
          UPD_UL_STATE();

          /* Consider the TBF released. Now load the next PDU and establish new TBF
          ** otherwise go back to NULL
          */
          (void)load_cur_pdu();
        } /* GPRS RM_ACKNACK_FAI_TBF_COMPLETE */
        else
        {
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
          uint16 va_last_puan = ul.va;
#endif /* FEATURE_GERAN_POWER_EFFICIENT_OPERATION */

          /* FAI = 0 so update bitmap, check for ChCS and carry on */

          /* Re-process the UL FIFO as paknak will have made some blocks
          ** invalid. So delete the un-read blocks in the FIFO and rewind the
          ** transmit vars back to the BSN value of the oldest blocks
          ** (pack_vsf, nack_vsf)
          */
          /* Freeze FIFO access for reprocessing */
          grlc_ul_fifo_disable_pl1_access();

          grlc_ul_fifo_delete_unread_nack_pack_blocks( &ul.nack_vsf, &ul.pack_vsf);

          /* Re-enable PL1 access to the FIFO */
          grlc_ul_fifo_enable_pl1_access();

          UPD_UL_NACK_VSF();
          UPD_UL_PACK_VSF();

          update_ack_state_array();

          /* GPRS RA_AT_TX_COMPLETE_CHANGE_MODE type 3 */
          if (IS_EXT_UTBF_ACTIVE() &&
              cur_pdu_ptr->realloc_action == RA_AT_TX_COMPLETE_CHANGE_MODE &&
              ul.realloc_state != RALS_COMPLETE &&
              IS_TX_COMPLETE() &&
              ul.va == ul.vs   &&
              next_pdu_ptr != NULL
             )
          {
            /* Send PRR with parameters from the next PDU */
            ul.realloc_pdu_ptr = next_pdu_ptr;
            ul.realloc_psn = next_pdu_ptr->psn;

            /* b_cv_zero - is set to TRUE , because we
             have transitioned to
             realloc_state = RA_AT_TX_COMPLETE_CHANGE_MODE,
             that means we have Data in our Uplink Q.
            */
            realloc_for_next_pdu( ul.realloc_pdu_ptr,FALSE);

            /* Set RALS_COMPLETE once after sending a PRR.
            ** Expects this realloc_state to change when the next current PDU is loaded
            */
            ul.realloc_state = RALS_COMPLETE;

            MSG_GERAN_LOW_1_G("GRLU prr TX_COMPLETED psn=%u", next_pdu_ptr->psn);
          }

          rlc_log_ul_acknack_params();

          /* FA = 0. Window has been un-stalled (not full) then stop TIMER
          */
          if (!IS_TX_WINDOW_FULL())
          {
            TIMER_STOP_T3182();
          }

          /* Check and handle change of CS
          */
          if (ul.cur_res.cs != mac_msg_ptr->msg.ul_pkt_acknack_ind.cs )
          {
            /* Freeze FIFO access for reprocessing
            */
            grlc_ul_fifo_disable_pl1_access();

            /* parameter contains the new coding scheme
            */
            change_coding_scheme( mac_msg_ptr->msg.ul_pkt_acknack_ind.cs);

            /* Re-enable PL1 access to the FIFO
            */
            grlc_ul_fifo_enable_pl1_access();

            /* update the 'last' working coding scheme
            */
            ul.last_valid_coding_scheme = ul.cur_res.cs;

            /* Update coding scheme for engg_mode
            */
            geran_eng_mode_data_write(ENG_MODE_GPRS_UL_CS, &ul.cur_res.cs, TRUE);

          } /* RM_ACKNACK_FAI_TBF_COMPLETE */

          process_guard_timer();

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
          /*
          * FAI = 0 and check if no more blocks to transmit. If EXT_UL_TBF
          * is enabled, indicate GMAC for start of EXT_UL_TBF period 
          */
          if (IS_EXT_UTBF_ACTIVE() &&
              ul.va == ul.vs &&
              ul.vs == ul.seg_ind &&
              ul.va != va_last_puan
             )
          {
            grlc_ul_ext_tbf_start_ind();
          }
#endif /* FEATURE_GERAN_POWER_EFFICIENT_OPERATION */
        } /* end FAI not set */
      } /* GPRS acknack with EGPRS support */
    } /* PKT_ACKNACK_IND */

    else if (mac_msg_ptr->msg.sig == MR_UL_ALLOC_CNF)
    /*------------------------------------------------------------
    ** US_ACK_XFER, UL_ALLOC_CNF signal covers both solicited and
    ** un-solicited resource assignment
    **------------------------------------------------------------
    */
    {
      /* Refresh sysinfo during transfer
      */
      read_ul_sysinfo();

      /* Force open-ended if given close-ended
      */
      if (mac_msg_ptr->msg.ul_alloc_cnf.tbf_type == RM_CLOSE_ENDED_TBF)
      {
        mac_msg_ptr->msg.ul_alloc_cnf.tbf_type = RM_OPEN_ENDED_TBF;
        MSG_GERAN_LOW_0_G("GRLU close-ended converted to open-ended");
      }

      /*-------------------------------------------------------------------------
      ** If un-solicited assignment is of different tbf_mode then report error
      ** and perform RLC UL recovery, ie. tear down UL TBF and reset and request
      ** a new TBF
      **-------------------------------------------------------------------------
      */
      if (mac_msg_ptr->msg.ul_alloc_cnf.tbf_mode != ul.cur_res.tbf_mode)
      {
        MSG_GERAN_LOW_2_G("ERLU A ##cur_tbf_mode=%u next_tbf_mode=%u",
                  ul.cur_res.tbf_mode,
                  mac_msg_ptr->msg.ul_alloc_cnf.tbf_mode);

        grlc_ul_show_vars();
        ul.b_error_recovery_required = TRUE;
        detect_error_and_recover();
      }

      grlc_ul_handle_alloc_cnf_ack_xfer( &mac_msg_ptr->msg.ul_alloc_cnf);


      /* Assess flow control to cater for changes to MCS, windows size or number
      ** of timeslots
      */
      grlc_llc_ul_flow_control();

    } /* ALLOC_CNF */

    else
    if (mac_msg_ptr->msg.sig == MR_NO_PS_ACCESS_IND)
    {
      grlc_ul_handle_no_ps_access( mac_msg_ptr->msg.no_ps_access_ind.delete_pdu_flag);
      MSG_GERAN_LOW_0_G("GRLU XFER no-access");
    }
#ifdef  FEATURE_GPRS_PS_HANDOVER
    else
    if (mac_msg_ptr->msg.sig == MR_UL_PSHO_START_IND)
    {
      if (ul.b_psho_active)
      {
        MSG_GERAN_ERROR_0_G("GRLU A XFER PSHO_START whilst ALREADY active");
      }

      ul.b_psho_active = TRUE;
      ul.b_access = FALSE;
      MSG_GERAN_MED_0_G("GRLU A XFER PSHO_START");

      /* Disable PL1 access to FIFO
      */
      grlc_ul_fifo_disable_pl1_access();
    }
    else
    if (mac_msg_ptr->msg.sig == MR_UL_PSHO_COMPLETE_IND)
    {
      if (!ul.b_psho_active)
      {
        MSG_GERAN_ERROR_0_G("GRLU A XFER PSHO_COMPLETE whilst PSHO not active");
      }

      MSG_GERAN_MED_0_G("GRLU A XFER PSHO_COMPLETE");
      ul.b_psho_active = FALSE;
      ul.b_access = TRUE;

      grlc_ul_handle_psho_complete_in_xfer( mac_msg_ptr);

      /* Enable PL1 access to FIFO
      */
      grlc_ul_fifo_enable_pl1_access();
    }
#endif /* FEATURE_GPRS_PS_HANDOVER  */
    else
    if (mac_msg_ptr->msg.sig == MR_PS_ACCESS_REQ)
    {
#ifdef  FEATURE_GPRS_PS_HANDOVER
      MSG_GERAN_MED_2_G("GRLU XFER access (ignore access = %u) PSHO=%u",
        ul.b_access, ul.b_psho_active);
#else
      /* In ack-xfer, so reassert access state, but do nothing
      */
      ul.b_access = TRUE;
      MSG_GERAN_LOW_1_G("GRLU XFER action on ps_access=%u",mac_msg_ptr->msg.ps_access_req.action);
#endif /* FEATURE_GPRS_PS_HANDOVER  */
    }
    else
    if (mac_msg_ptr->msg.sig == MR_UL_RELEASE_IND)
    /*--------------------------------------------------------
    ** US_ACK_XFER, MR_UL_RELEASE_IND
    ** uplink abnormal and normal release indication from MAC
    **--------------------------------------------------------
    */
    {
      MSG_GERAN_MED_1_G("GRLU A Rcvd REL_IND cause %d",
        mac_msg_ptr->msg.ul_release_ind.cause);

      /* note the uplink resource release cause
      */
      ul.ul_release_cause = mac_msg_ptr->msg.ul_release_ind.cause;

      rlc_log_ul_release_ind( (uint8)ul.cur_res.ul_tfi,RLC_LOG_UL_MAC_RELEASE_IND);

      handle_ul_release_event( ul.ul_release_cause, TRUE);

      /* Assess flow control as MCS, window size and number of timeslots drop
      ** back to default values
      */
      grlc_llc_ul_flow_control();
    }
    else if (mac_msg_ptr->msg.sig == MR_UL_CONTENTION_IND)
    /*-----------------------------------------------------------
    ** US_ACK_XFER, MR_UL_CONTENTION_IND
    ** Contention resolution signal from MAC in one phase access
    **-----------------------------------------------------------
    */
    {
      ul.acknack_blk_cnt = mac_msg_ptr->msg.ul_tlli_ind.blk_cnt;

      /* ! Must check for change of MCS in contention_res..()
      */
      ul.pre_emptive_tx = mac_msg_ptr->msg.ul_tlli_ind.pre_emptive_tx;
      ul.cur_res.resegment = mac_msg_ptr->msg.ul_tlli_ind.resegment;
      MSG_GERAN_MED_2_G("ERLU A Rcvd CONTENTION_IND petx=%u reseg=%u",
        ul.pre_emptive_tx,
        ul.cur_res.resegment);

      if (mac_msg_ptr->msg.ul_tlli_ind.contention_res ==
          RM_CONTENTION_RESOLVED
         )
      {
        handle_contention_resolution_success( mac_msg_ptr);
      }

      /* Assess flow control as MCS, window size and number of timeslots may change
      ** after cont-resolution
      */
      grlc_llc_ul_flow_control();

    } /* CONTENTION_IND */

    else

    /*----------------------------------------------------------
    ** US_ACK_XFER
    ** don't expect any other signals from MAC in ACK_XFER mode
    **----------------------------------------------------------
    */
    {
      MSG_GERAN_LOW_1_G("GRLU A Unexp sig %d",mac_msg_ptr->msg.sig);

      /*------------------------------------------------------------------
      ** ! don't drop into ack_mode_xfer(). So free memory and exit here
      **------------------------------------------------------------------
      */
      GPRS_MEM_FREE(mac_msg_ptr);

      return;

    } /* bad MAC sig */

    if (cur_pdu_ptr != NULL && ul.state == US_ACK_XFER)
    {
      if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
      {
        e_ack_mode_xfer();
      }
      else
      {
        ack_mode_xfer();
      }
    }

    GPRS_MEM_FREE(mac_msg_ptr);
  } /* end MAC sig not NULL */

} /* grlc_ul_handle_mac_event_us_ack_xfer */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_TMR_EVENT_US_NULL()
===
===  DESCRIPTION
===
===    Handler for TMR signals in US_NULL state.
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_handle_tmr_event_us_null (void)
{
  rm_timer_msg_t  *timer_msg_ptr;

  timer_msg_ptr = (rm_timer_msg_t *)q_get(&UTQ());

  if (timer_msg_ptr == NULL)
  {
    MSG_GERAN_LOW_0_G("GRLU A tmr event without msg");
  }
  else
  {
    /* Stale PDU timer has expired.
    */
    if (timer_msg_ptr->sig == RM_TMR_STALE_PDU_DELETE)
    {

      ul_static.timer_stale_pdu_delete_expiry_cnt++;

      MSG_GERAN_MED_1_G("GRLU A Expy Stale PDU tmr %u",
                 ul_static.timer_stale_pdu_delete_expiry_cnt);

      /* Set timer state to inactive.
      */
      ul_static.timer_stale_pdu_delete.state = TMR_INACTIVE;

      /* Clear UL realloc variables.
      */
      grlc_clear_realloc_vars();

      /* Unput any PDUs in paq queue
      */
      grlc_ul_unput_paq();

      /* Purge all UL PDUs. No STATUS_IND sent if SM PDU is deleted, hence FALSE.
      */
      flush_llc_pdu_queue(  FALSE );

      /* Stale timer has expired, set ul_pdu_delete flag to TRUE. Any PDUs
      ** from upper layers will be deleted while this flag is TRUE.
      */
      ul_static.b_ul_pdu_delete_after_stale_tmr_expiry = TRUE;

      gllc_grr_ul_suspend_ind();

      MSG_GERAN_HIGH_0_G("GRLU - UL Suspended");

    }
    else
    {
      MSG_GERAN_LOW_1_G("GRLU A Unexp TMR sig %d",timer_msg_ptr->sig);
    }

    GPRS_MEM_FREE(timer_msg_ptr);
  }
} /* grlc_ul_handle_tmr_event_us_null */


/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_TMR_EVENT_US_ACK_XFER()
===
===  DESCRIPTION
===
===    Handler for TMR signals in US_ACK_XFER state.
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_handle_tmr_event_us_ack_xfer (void)
{
  rm_timer_msg_t  *timer_msg_ptr;

  timer_msg_ptr = (rm_timer_msg_t *)q_get(&UTQ());

  if (timer_msg_ptr == NULL)
  {
    MSG_GERAN_LOW_0_G("GRLU A tmr event without msg");
  }
  else
  {
    /*-----------------------------------------------------------------
    ** Handle T3182 expiry. Two cases: during xfer and during release.
    ** During xfer, MS shall decrement  N3102 and perform abs rel with
    ** access retry. During release, MS does the same but without
    ** decrementing N3102.
    **-----------------------------------------------------------------
    */
    if (timer_msg_ptr->sig == RM_TMR_T3182_AWAIT_UL_ACKNACK)
    {
      ul.timer_t3182_expiry_cnt++;
      MSG_GERAN_MED_1_G("GRLU A Expy T3182 %u NoAcknack",ul.timer_t3182_expiry_cnt);

      /* When T3182 expires the N3102 counter is decremented (if enabled).
      ** If N3102 expires then we perform ab rel cell resel. If not we perform
      ** abnormal release with access retry. Because abnormal release cell
      ** reselection has precedence over abnormal release with access retry
      */
      if (IS_N3102_ENABLED())
      {
        DEC_N3102();
#ifdef  GSM_GPRS_GRLU_SHOW_N3102
        MSG_GERAN_MED_3_G("GRLU A DecN3102=%d inc=%d dec=%d",ul.n3102,PAN_INC(),PAN_DEC());
#endif
        /* N3102 will cause abnormal release cell reselection which has
        ** precedence over T3182's action
        */
        if (IS_N3102_EXPIRED())
        {
          MSG_GERAN_MED_3_G("GRLU A N3102 Expy (cell-resel) = %d inc=%d dec=%d",
                  ul.n3102,
                  PAN_INC(),
                  PAN_DEC());
          /* Inform MAC of RLC initiated ul release
          */
          indicate_ul_release (  REL_UL_ABNORMAL_CELL_RESEL );
          rlc_log_ul_release_ind(  (uint8)ul.cur_res.ul_tfi,
                                  RLC_LOG_UL_EXPIRED_N3102_IN_ACK_TRANSFER);


          /* Cell reselection requested. Two actions are required:
          **
          ** (1). Set N3102 -
          **      Set it to the currently held max value again in case same
          **      cell is reselected. Assumed N3102 max won't change in same
          **      cell.
          **
          ** (2). Delete GMM PDUs -
          **      To avoid RAU PDU accumulation which occurs due to not
          **      receiving ack/nack for the sent RAU PDUs hence T3182/N3102
          **      expiry and resulting reselection request to mac but
          **      returning to the same cell repeatedly.
          */

          SET_N3102();

          grlc_ul_del_signalling_pdus( NO_NAS_INDICATION);

          /* abnormal release with cell-reselection
          */
          handle_ul_release_event( REL_UL_ABNORMAL_CELL_RESEL, FALSE);
        }
        else
        {
          /* Inform MAC of RLC initiated ul release
          */
          indicate_ul_release (  REL_UL_ABNORMAL_WITH_ACCESS_RETRY );

          /* abnormal release with access retry with n3102 status maintained
          */
          handle_ul_release_event( REL_UL_ABNORMAL_WITH_ACCESS_RETRY, FALSE);

          rlc_log_ul_release_ind( (uint8)ul.cur_res.ul_tfi,
                                  RLC_LOG_UL_EXPIRED_T3182_IN_ACK_TRANSFER);
        }
      } /* N3102 enabled */
      else
      {
        /* Inform MAC of RLC initiated UL release
        */
        indicate_ul_release (  REL_UL_ABNORMAL_WITH_ACCESS_RETRY );

        rlc_log_ul_release_ind( (uint8)ul.cur_res.ul_tfi,
                                RLC_LOG_UL_EXPIRED_T3182_IN_ACK_TRANSFER);

        /* abnormal release with access retry with n3102 status maintained
        */
        handle_ul_release_event( REL_UL_ABNORMAL_WITH_ACCESS_RETRY, FALSE);
      } /* N3102 disabled */
    } /* TMR_T3182_AWAIT_UL_ACKNACK timeout */
    else if (timer_msg_ptr->sig == RM_TMR_AWAIT_MAC_UL_RESP)
    {
      MSG_GERAN_LOW_1_G("GRLU A Expy MAC_RESP",ul.state);
      ack_mode_xfer();
    }
    /*--------------------------------------------------------------------------------------
    ** Handle TIMER_AWAIT_UL_ACKNACK. Perform abs rel with access retry if not in test mode
    **--------------------------------------------------------------------------------------
    */
    else if (timer_msg_ptr->sig == RM_TMR_AWAIT_UL_ACKNACK)
    {
      /* No release during GCF testing
      */
      rr_read_public_store(RRPS_MS_ANITE_GCF_FLAG, &ul.anite_gcf_flg);
      if (!ul.anite_gcf_flg)
      {
        ul.timer_await_ul_acknack_expiry_cnt++;
        MSG_GERAN_HIGH_1_G("ERLU XTimer expiry cnt = %u", ul.timer_await_ul_acknack_expiry_cnt);

        /* Inform MAC of RLC initiated ul release
        */
        indicate_ul_release (  REL_UL_ABNORMAL_WITH_ACCESS_RETRY );

        /* rlc_log_ul_release_ind( (uint8)ul.cur_res.ul_tfi,
                                RLC_LOG_UL_EXPIRED_T3182_IN_ACK_TRANSFER); */

        /* abnormal release with access retry with n3102 status maintained
        */
        handle_ul_release_event( REL_UL_ABNORMAL_WITH_ACCESS_RETRY, FALSE);
      }
      else
      {
        MSG_GERAN_LOW_1_G("ERLU XTimer expiry: no release cnt = %u", ul.timer_await_ul_acknack_expiry_cnt);
      }
    }
    else
    {
      MSG_GERAN_LOW_1_G("GRLU A Unexp TMR sig %d",timer_msg_ptr->sig);
    }
    GPRS_MEM_FREE(timer_msg_ptr);
  } /* TMR msg not NULL */
} /* grlc_ul_handle_tmr_event_us_ack_xfer */

#ifdef  FEATURE_GSM_DTM
/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_MAC_EVENT_US_GTTP_XFER()
===
===  DESCRIPTION
===
===    Handler for MAC signals in US_X state.
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_handle_mac_event_us_gttp_xfer (void)
{
  mac_rlc_msg_t *mac_msg_ptr;
  uint16        psn;

  mac_msg_ptr = (mac_rlc_msg_t *)q_get(&UMQ());

  if (mac_msg_ptr == NULL)
  {
    MSG_GERAN_LOW_0_G("GRLU GTTP MAC event without msg");
  }
  else
  {
    if (mac_msg_ptr->msg.sig == MR_UL_RESET_REQ)
    {
      grlc_ul_panic_reset();
    }
    else
    if (mac_msg_ptr->msg.ul_release_ind.sig == MR_UL_RELEASE_IND)
    {
      /* Successful GTTP transfer
      */
      if (mac_msg_ptr->msg.ul_release_ind.cause == REL_UL_NORMAL)
      {
        /* Successful GTTP transfer: clear PDU, loads the next PDU.
        ** If the next PDU exists make request .. else goto US_NULL
        */
        if (cur_pdu_ptr != NULL)
        {
          psn = cur_pdu_ptr->psn;

          MSG_GERAN_MED_1_G("GRLU GTTP psn %u sent OK",psn);

          /* To transfer grouping info to the next PDU
          */
          ungroup_pdu( cur_pdu_ptr);

          /* Successful transmission of PDU , set the flag to TRUE
          ** to start ready timer
          */
          grlc_ul_del_free_paq_upto_psn( psn,TRUE);
        }

        clr_ul_data();
        grlc_ul_unput_paq();

        ul.state = US_NULL;
        UPD_UL_STATE();

        /* In GTTP_XFER - Normal Release - MAC will not send MR_PS_ACCESS_REQ
         * as it has has not block RLC before.
         */
        ul.b_access = TRUE;

        /*
        * Re-order signalling pdus to be sent first to NW
        */
        grlc_ul_reorder_pdus();

        /* Consider the TBF released. Now load the next PDU and establish new TBF
        ** otherwise go back to NULL
        */
        (void)load_cur_pdu();
        est_tbf_from_null();
      }
      else
      {
        /* Only expect two causes above for whilst in US_GTTP_XFER
        */
        MSG_GERAN_LOW_1_G("GRLU GTTP RELEASE_IND Unexp cause %u",
                    mac_msg_ptr->msg.ul_release_ind.cause);
      } /* handle causes of UL release */
    } /* handle MAC signal whilst in US_GTTP_XFER */
    else if (mac_msg_ptr->msg.sig == MR_NO_PS_ACCESS_IND)
    {
      MSG_GERAN_MED_3_G("GRLU GTTP Failed. To resend psn %u delete_flag=%u",
              cur_pdu_ptr->psn,
              mac_msg_ptr->msg.no_ps_access_ind.delete_pdu_flag,
              0);
      grlc_ul_handle_no_ps_access ( mac_msg_ptr->msg.no_ps_access_ind.delete_pdu_flag);
      (void)load_cur_pdu();
    }
    else
    {
      /* Only expect one signal from MAC whilst in US_GTTP_XFER
      */
      MSG_GERAN_LOW_1_G("GRLU GTTP Unexp MAC sig %u",
                mac_msg_ptr->msg.dl_release_ind.sig);
    } /* end release_ind */

    GPRS_MEM_FREE(mac_msg_ptr);
  } /* mac sig not NULL */

} /* grlc_ul_handle_mac_event_us_gttp_xfer */
#endif /* FEATURE_GSM_DTM */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_MAC_EVENT_US_UNACK_XFER()
===
===  DESCRIPTION
===
===    Handler for MAC signals in US_UNACK_XFER state.
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_handle_mac_event_us_unack_xfer (void)
{
  mac_rlc_msg_t *mac_msg_ptr;
  uint16        psn;
  boolean       b_test_mode;
  mac_msg_ptr = (mac_rlc_msg_t *)q_get(&UMQ());

  if (mac_msg_ptr == NULL)
  {
    MSG_GERAN_LOW_0_G("GRLU U MAC event without msg");
  }
  else
  {
    if (mac_msg_ptr->msg.sig == MR_UL_RESET_REQ)
    {
      grlc_ul_panic_reset();
    }
    else
    if (mac_msg_ptr->msg.sig == MR_UL_PKT_ACKNACK_IND)
    {
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
      uint16 va_last_puan = ul.va;
#endif /* FEATURE_GERAN_POWER_EFFICIENT_OPERATION */

      TIMER_STOP_T3182();
#ifdef  GSM_GPRS_GRLU_SHOW_T3182
      MSG_GERAN_MED_0_G("GRLU U T3182 stopped paknak");
#endif

      /* even though V(A) is not used for marking the ack in unack mode,
      ** it is used to calculate how many blocks have been transmitted
      ** without receiving a paknak. So, when paknak is received, make
      ** V(A) the same as V(S) to make the tx window 0
      */
      ul.va = ul.vs;
      UPD_UL_VA();

      if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GERAN_MED_3_G("ERLU U paknak FA=%d SSN=%d",
                mac_msg_ptr->msg.ul_pkt_acknack_ind.egprs_acknack_desc.fai,
                mac_msg_ptr->msg.ul_pkt_acknack_ind.egprs_acknack_desc.ssn,
                0);

        ul.egprs_acknack_desc =  mac_msg_ptr->msg.ul_pkt_acknack_ind.egprs_acknack_desc;

        /* Ignore ul.resegment field in unack mode
        */

        /* FAI set and valid rrbp
        */
        if (ul.egprs_acknack_desc.fai == RM_ACKNACK_FAI_TBF_COMPLETE)
        {
          ul.b_access = FALSE;

          /* To transfer grouping info to the next PDU
          */
          ungroup_pdu( cur_pdu_ptr);

          /* Ignore bitmap and free PDUs and LLC messages upto VS or CV=0.
          */
          psn = grlc_ul_paq_seek_psn_cv0( ul.vs);

          /* re-init
          */
          clr_ul_data();

          /* Successful transmission of PDU , set the flag to TRUE
          ** to start ready timer
          */
          grlc_ul_del_free_paq_upto_psn( psn,TRUE);
          grlc_ul_unput_paq();

          ul.state = US_NULL;
          UPD_UL_STATE();

          (void)load_cur_pdu();
        } /* FAI */
        else
        {
          /* Check for Ext UL TBF as well.
          ** Spec. mentions UNACK_RE_TX_MAX for Non-Extended UL TBF Only.
          ** Also, If there are PACK blocks in EXT_UTBF do not queue CV=0 block.
          */
          if (ul.re_tx_cv_0_cnt < UNACK_RE_TX_MAX &&
              ul.b_cv_0 &&
              IS_TX_COMPLETE()
              && ( !( IS_EXT_UTBF_ACTIVE() && (grlc_ul_fifo_get_pack_cnt() >= 1)) )
             )
          {
            if (ul.e_bsn_of_cv_0_block != E_GRLC_INVALID_BSN)
            {
              grlc_ul_data_block_t *tx_blk_p = &TX_ARR( ul.e_bsn_of_cv_0_block).blk;
              if((tx_blk_p->cs > RM_MCS_6) && (tx_blk_p->cs <= RM_MCS_9))
              {
                /* Send Block with CV 0 as a single payload */
                grlc_ul_demote_mcs_modify_li(  ul.e_bsn_of_cv_0_block );
              }

              /* FAI clear and re-transmit count is less than 4 so send
              ** block with CV = 0 again
              */

              /* Apply new CPS for PS change. No MCS 7/8/9 since CV 0 always goes out
              ** as a single payload
              */
              if (tx_blk_p->cs <= RM_MCS_4)
              {
                GET_CPS_HDR_3( ul.e_bsn_of_cv_0_block, tx_blk_p->hdr.cps );
              }
              else if( tx_blk_p->cs <= RM_MCS_6 )
              {
                GET_CPS_HDR_2( ul.e_bsn_of_cv_0_block, tx_blk_p->hdr.cps );
              }
              else
              {
                MSG_GERAN_LOW_1_G("GRLU Invalid MCS %d",tx_blk_p->cs );
              }

              E_SEND_BLOCK_CV_0();

              /* Store new PS to old PS
              */
              tx_blk_p->old_ps = tx_blk_p->ps;


              /* Rotate PS for next Tx.
              */
              ROTATE_PS_NO_MCS_CHANGE(tx_blk_p->cs,
                                      tx_blk_p->old_ps,
                                      tx_blk_p->ps
                                      );
              ul.re_tx_cv_0_cnt++;
            }
            else
            {
              MSG_GERAN_LOW_1_G("GRLU Invalid bsn_cv_0 = %u",ul.e_bsn_of_cv_0_block);
            }
          }

          /* Check and handle change of MCS
          */
          if (ul.cur_res.cs != mac_msg_ptr->msg.ul_pkt_acknack_ind.mcs)
          {
            /* Freeze FIFO access for reprocessing
            */
            grlc_ul_fifo_disable_pl1_access();

            /* parameter contains the new coding scheme
            */
            change_coding_scheme( mac_msg_ptr->msg.ul_pkt_acknack_ind.mcs);

            /* Re-enable PL1 access to the FIFO
            */
            grlc_ul_fifo_enable_pl1_access();

            /* update the 'last' working coding scheme
            */
            ul.last_valid_mcs = ul.cur_res.cs;

            /* Update coding scheme for engg_mode
            */
            geran_eng_mode_data_write(ENG_MODE_EGPRS_UL_MCS, &ul.cur_res.cs, TRUE);
          }

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
          /*
          * FAI = 0 and check if no more blocks to transmit. If EXT_UL_TBF
          * is enabled, indicate GMAC for start of EXT_UL_TBF period 
          */
          if (IS_EXT_UTBF_ACTIVE() &&
              ul.vs == ul.seg_ind &&
              ul.va != va_last_puan
             )
          {
            grlc_ul_ext_tbf_start_ind();
          }
#endif /* FEATURE_GERAN_POWER_EFFICIENT_OPERATION */
        } /* FAI = 0 */

        /* Log EPUAN
        ** Log Code: 0x5209
        */
        rlc_log_epuan( &(mac_msg_ptr->msg.ul_pkt_acknack_ind));

      } /* EGPRS UNACK EPUAN */
      else
      {
        MSG_GERAN_MED_2_G("GRLU U paknak FA=%d SSN=%d",
                  mac_msg_ptr->msg.ul_pkt_acknack_ind.acknack_desc.fai,
                  mac_msg_ptr->msg.ul_pkt_acknack_ind.acknack_desc.ssn
                  );

        /* FAI set and valid rrbp
        */
        if (mac_msg_ptr->msg.ul_pkt_acknack_ind.acknack_desc.fai == RM_ACKNACK_FAI_TBF_COMPLETE)
        {
          ul.b_access = FALSE;

          /* To transfer grouping info to the next PDU
          */
          ungroup_pdu( cur_pdu_ptr);

          /* Ignore bitmap and free PDUs and LLC messages upto VS or CV=0.
          */
          psn = grlc_ul_paq_seek_psn_cv0( ul.vs);

          /* re-init
          */
          clr_ul_data();

          /* Successful transmission of PDU , set the flag to TRUE
          ** to start ready timer
          */
          grlc_ul_del_free_paq_upto_psn( psn,TRUE);
          grlc_ul_unput_paq();

          ul.state = US_NULL;
          UPD_UL_STATE();

          (void)load_cur_pdu();

        } /* FAI */
        else
        {
          /* Check for Ext UL TBF as well.
          ** Spec. mentions UNACK_RE_TX_MAX for Non-Extended UL TBF Only.
          ** Also, If there are PACK blocks do not queue CV=0 block.
          */
          if (ul.re_tx_cv_0_cnt < UNACK_RE_TX_MAX &&
              ul.b_cv_0 &&
              IS_TX_COMPLETE()
              && ( !(IS_EXT_UTBF_ACTIVE() && (grlc_ul_fifo_get_pack_cnt() >= 1)) )
             )
          {
            /* FAI clear and re-transmit count is less than 4 so send
            ** block with CV = 0 again
            */
            SEND_BLOCK_CV_0();
            ul.re_tx_cv_0_cnt++;
          }

          /* handle Paknak during UNACK_XFER
          */
          /* store the latest packet uplink acknack message. No need to
          ** test FAI bits etc.. as tx is not completed. Otherwise, we would
          ** be in UNACK_RELEASE state already
          */
          ul.ul_acknack = mac_msg_ptr->msg.ul_pkt_acknack_ind.acknack_desc;
          rlc_log_ul_acknack_params();

          /* Check and handle change of CS
          */
          if (ul.cur_res.cs != mac_msg_ptr->msg.ul_pkt_acknack_ind.cs)
          {
            /* Freeze FIFO access for reprocessing
            */
            grlc_ul_fifo_disable_pl1_access();

            /* parameter contains the new coding scheme
            */
            change_coding_scheme( mac_msg_ptr->msg.ul_pkt_acknack_ind.cs);

            /* Re-enable PL1 access to the FIFO
            */
            grlc_ul_fifo_enable_pl1_access();

            /* update the 'last' working coding scheme
            */
            ul.last_valid_coding_scheme = ul.cur_res.cs;

            /* Update coding scheme for engg_mode
            */
            geran_eng_mode_data_write(ENG_MODE_GPRS_UL_CS, &ul.cur_res.cs, TRUE);
          }

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
          /*
          * FAI = 0 and check if no more blocks to transmit. If EXT_UL_TBF
          * is enabled, indicate GMAC for start of EXT_UL_TBF period 
          */
          if (IS_EXT_UTBF_ACTIVE() &&
              ul.vs == ul.seg_ind &&
              ul.va != va_last_puan
             )
          {
            grlc_ul_ext_tbf_start_ind();
          }
#endif /* FEATURE_GERAN_POWER_EFFICIENT_OPERATION */
        } /* FAI = 0 */
      }
    } /* PKT_ACKNACK */

    else if (mac_msg_ptr->msg.sig == MR_UL_ALLOC_CNF)
    {
      /* Refresh sysinfo during transfer
      */
      read_ul_sysinfo();

      /* Force open-ended if given close-ended
      */
      if (mac_msg_ptr->msg.ul_alloc_cnf.tbf_type == RM_CLOSE_ENDED_TBF)
      {
        mac_msg_ptr->msg.ul_alloc_cnf.tbf_type = RM_OPEN_ENDED_TBF;
        MSG_GERAN_LOW_0_G("GRLU close-ended converted to open-ended");
      }

      /*-------------------------------------------------------------------------
      ** If un-solicited assignment is of different tbf_mode then report error
      ** and perform RLC UL recovery, ie. tear down UL TBF and reset and request
      ** a new TBF
      **-------------------------------------------------------------------------
      */
      if (mac_msg_ptr->msg.ul_alloc_cnf.tbf_mode != ul.cur_res.tbf_mode)
      {
        MSG_GERAN_LOW_2_G("ERLU U ##cur_tbf_mode=%u next_tbf_mode=%u",
                  ul.cur_res.tbf_mode,
                  mac_msg_ptr->msg.ul_alloc_cnf.tbf_mode
                  );
        grlc_ul_show_vars();
        ul.b_error_recovery_required = TRUE;
        detect_error_and_recover();
      }

      grlc_ul_handle_alloc_cnf_unack_xfer( &mac_msg_ptr->msg.ul_alloc_cnf);

      /* Assess flow control to cater for changes to MCS,
      ** windows size or number of timeslots.
      */
      grlc_llc_ul_flow_control();

    } /* ALLOC_CNF */

    else if (mac_msg_ptr->msg.sig == MR_UL_RELEASE_IND)
    {
      ul.ul_release_cause = mac_msg_ptr->msg.ul_release_ind.cause;
      MSG_GERAN_MED_1_G("GRLU U Rcvd REL_IND cause %d",ul.ul_release_cause);
      rlc_log_ul_release_ind( (uint8)ul.cur_res.ul_tfi,RLC_LOG_UL_MAC_RELEASE_IND);

      handle_ul_release_event( ul.ul_release_cause, TRUE);
    }

    else if (mac_msg_ptr->msg.sig == MR_UL_CONTENTION_IND)
    /*-----------------------------------------------------------
    ** Contention resolution signal from MAC in one phase access
    **-----------------------------------------------------------
    */
    {
      /* We are in un-ack mode. We normally do not expect to be assigned one-phase
      ** in un-ack. But 41.2.6.4 incorrectly assigns one-phase to an un-ack PDU.
      ** In order to proceed to the main part of the test in clude the following
      ** code fragment to handle the Cont-res procedure instead of printing
      ** an error message
      */
      ul.acknack_blk_cnt = mac_msg_ptr->msg.ul_tlli_ind.blk_cnt;

      /* ! Must check for change of MCS in contention_res..()
      */
      ul.pre_emptive_tx = mac_msg_ptr->msg.ul_tlli_ind.pre_emptive_tx;
      ul.cur_res.resegment = mac_msg_ptr->msg.ul_tlli_ind.resegment;

      MSG_GERAN_MED_2_G("GRLU U Rcvd CONTENTION_IND petx=%u reseg=%u",
              ul.pre_emptive_tx,
              ul.cur_res.resegment);

      if (mac_msg_ptr->msg.ul_tlli_ind.contention_res == RM_CONTENTION_RESOLVED)
      {
        handle_contention_resolution_success( mac_msg_ptr);
      }

      /* Assess flow control as MCS, window size and number of timeslots may change
      ** after cont-resolution
      */
      grlc_llc_ul_flow_control();

    } /* CONTENTION_IND */

    else
    if (mac_msg_ptr->msg.sig == MR_NO_PS_ACCESS_IND)
    {
      grlc_ul_handle_no_ps_access( mac_msg_ptr->msg.no_ps_access_ind.delete_pdu_flag);
      MSG_GERAN_LOW_3_G("GRLU UN-XFER no-access delete_flag=%u",
              mac_msg_ptr->msg.no_ps_access_ind.delete_pdu_flag,
              0,
              0);
      b_test_mode = gllc_grr_ul_tbf_rel_ind();
      if (b_test_mode)
      {
        MSG_GERAN_LOW_0_G("GRLU GPRS-TEST-MODE-A Release - Purge all PDUs");
        grlc_ul_purge_pdu_queue();
      }
    }
#ifdef  FEATURE_GPRS_PS_HANDOVER
    else
    if (mac_msg_ptr->msg.sig == MR_UL_PSHO_START_IND)
    {
      if (ul.b_psho_active)
      {
        MSG_GERAN_ERROR_0_G("GRLU U XFER PSHO_START whilst ALREADY active");
      }

      ul.b_psho_active = TRUE;
      ul.b_access = FALSE;

      MSG_GERAN_MED_0_G("GRLU U XFER PSHO_START");

      /* Disable PL1 access to FIFO
      */
      grlc_ul_fifo_disable_pl1_access();
    }
    else
    if (mac_msg_ptr->msg.sig == MR_UL_PSHO_COMPLETE_IND)
    {
      if (!ul.b_psho_active)
      {
        MSG_GERAN_ERROR_0_G("GRLU U XFER PSHO_COMPLETE whilst PSHO not active");
      }

      MSG_GERAN_MED_0_G("GRLU U XFER PSHO_COMPLETE");
      ul.b_psho_active = FALSE;
      ul.b_access = TRUE;

      grlc_ul_handle_psho_complete_in_xfer( mac_msg_ptr);

      /* Enable PL1 access to FIFO
      */
      grlc_ul_fifo_enable_pl1_access();
    }
#endif /* FEATURE_GPRS_PS_HANDOVER  */
    else
    if (mac_msg_ptr->msg.sig == MR_PS_ACCESS_REQ)
    {
#ifdef  FEATURE_GPRS_PS_HANDOVER
      MSG_GERAN_MED_2_G("GRLU U XFER access (ignore access = %u) PSHO=%u",
        ul.b_access, ul.b_psho_active);
#else
      ul.b_access = TRUE;
      MSG_GERAN_LOW_1_G("GRLU UN-XFER action on ps_access=%u",mac_msg_ptr->msg.ps_access_req.action);
#endif /* FEATURE_GPRS_PS_HANDOVER  */
    }
    else
    {
      MSG_GERAN_LOW_1_G("GRLU U Unexp MAC sig %d", mac_msg_ptr->msg.sig);
    }

    GPRS_MEM_FREE(mac_msg_ptr);
  } /* end MAC sig not NULL */

} /* grlc_ul_handle_mac_event_us_unack_xfer */


/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_MAC_TMR_US_UNACK_XFER()
===
===  DESCRIPTION
===
===    Handler for Timer signals in US_UNACK_XFER state.
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_handle_tmr_event_us_unack_xfer (void)
{
  rm_timer_msg_t  *timer_msg_ptr;
  uint16          psn;

  timer_msg_ptr = (rm_timer_msg_t *)q_get(&UTQ());

  if (timer_msg_ptr == NULL)
  {
    MSG_GERAN_LOW_0_G("GRLU U timer event without msg");
  }
  else
  {
    if (timer_msg_ptr->sig == RM_TMR_T3182_AWAIT_UL_ACKNACK)
    {
      ul.timer_t3182_expiry_cnt++;

      MSG_GERAN_MED_1_G("GRLU U Expy T3182 NoAcknack %u",ul.timer_t3182_expiry_cnt);

      /* Handle T3182 expiry during UNACK_RELEASE.
      ** Release the current TBF (PDU) as if Paknak were received.
      */

      if (IS_TX_COMPLETE())
      {
        ul.b_access = FALSE;

        psn = grlc_ul_paq_seek_psn_cv0( ul.vs);
        clr_ul_data();

        /* Successful transmission of PDU , set the flag to TRUE
        ** to start ready timer
        */
        grlc_ul_del_free_paq_upto_psn( psn,TRUE);
        grlc_ul_unput_paq();

        /* do release
        */
        ul.state = US_NULL;
        UPD_UL_STATE();

        /* Inform MAC of RLC initiated ul release as if an FAI had been seen
        */
        indicate_ul_release (  REL_UL_NORMAL );

        rlc_log_ul_release_ind( (uint8)ul.cur_res.ul_tfi,
                                RLC_LOG_UL_EXPIRED_T3182_IN_UNACK_RELEASE);

        ul.realloc_state = RALS_NULL;
        (void)load_cur_pdu();
        UPD_UL_SUBSTATE();

      } /* T3182 expiry during UNACK_XFER TX complete */
      else
      /* handle T3182 expiry during UNACK_XFER
      */
      {
        /* when this timer expires the N3104 counter should be decremented
        ** and then the MS should perform an abnormal release with access
        ** retry. However, if N3102 expires (reaches 0) then the MS should
        ** perform an abnormal release with cell reselection as a priority.
        ** Therefore, check for this before doing access with retry.
        */
        if (IS_N3102_ENABLED())
        {
          DEC_N3102();
#ifdef  GSM_GPRS_GRLU_SHOW_N3102
          MSG_GERAN_MED_3_G("GRLU U DecN3102=%d inc=%d dec=%d",ul.n3102,PAN_INC(),PAN_DEC());
#endif
          if (IS_N3102_EXPIRED())
          {
            MSG_GERAN_MED_3_G("GRLU U N3102 Expy = %d inc=%d dec=%d",
                    ul.n3102,
                    PAN_INC(),
                    PAN_DEC());

            rlc_log_ul_release_ind( (uint8)ul.cur_res.ul_tfi,
                                    RLC_LOG_UL_EXPIRED_N3102_IN_UNACK_TRANSFER);

            /* Inform MAC of RLC initiated ul release
            */
           indicate_ul_release (  REL_UL_ABNORMAL_CELL_RESEL );

            /* Cell reselection requested. Two actions are required:
            **
            ** (1). Set N3102 -
            **      Set it to the currently held max value again in case same
            **      cell is reselected.
            **
            ** (2). Delete GMM PDUs -
            **      To avoid RAU PDU accumulation which occurs due to not
            **      receiving ack/nack for the sent RAU PDUs hence T3182/N3102
            **      expiry and resulting reselection request to mac but
            **      returning to the same cell repeatedly.
            */

            SET_N3102();

            grlc_ul_del_signalling_pdus( NO_NAS_INDICATION);

            handle_ul_release_event( REL_UL_ABNORMAL_CELL_RESEL, FALSE);
          }
          else
          {
            rlc_log_ul_release_ind( (uint8)ul.cur_res.ul_tfi,
                                    RLC_LOG_UL_EXPIRED_T3182_IN_UNACK_TRANSFER);

            /* Inform MAC of RLC initiated ul release with n3102 status
            ** maintained
            */
            indicate_ul_release (  REL_UL_ABNORMAL_WITH_ACCESS_RETRY );
            handle_ul_release_event( REL_UL_ABNORMAL_WITH_ACCESS_RETRY, FALSE);
          }
        } /* N3102 expiry */
        else
        {
          rlc_log_ul_release_ind( (uint8)ul.cur_res.ul_tfi,
                                  RLC_LOG_UL_EXPIRED_T3182_IN_UNACK_TRANSFER);

          /* Inform MAC of RLC initiated ul release with n3102 status
          ** maintained
          */
          indicate_ul_release (  REL_UL_ABNORMAL_WITH_ACCESS_RETRY );
          handle_ul_release_event( REL_UL_ABNORMAL_WITH_ACCESS_RETRY, FALSE);
        }
      } /* T3182 expiry during UNACK_XFER */
    } /* end T3182 */
    else if (timer_msg_ptr->sig == RM_TMR_AWAIT_MAC_UL_RESP)
    {
      MSG_GERAN_LOW_0_G("GRLU U Expy MAC_RESP");
      unack_mode_xfer();
    }

    GPRS_MEM_FREE(timer_msg_ptr);

  } /* timer_msg_ptr not NULL */

} /* grlc_ul_handle_tmr_event_us_unack_xfer */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_MAC_EVENT_US_TEST_MODE()
===
===  DESCRIPTION
===
===    Handler for MAC signals in US_TEST_MODE state.
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_handle_mac_event_us_test_mode (void)
{
  mac_rlc_msg_t *mac_msg_ptr;

  mac_msg_ptr = (mac_rlc_msg_t *)q_get(&UMQ());

  if (mac_msg_ptr == NULL)
  {
    MSG_GERAN_LOW_0_G("GRLU Test Mode rcvd MAC event without msg");
  }
  else
  {
    if (mac_msg_ptr->msg.sig == MR_UL_RESET_REQ)
    {
      grlc_ul_panic_reset();
    }
    else
    if (mac_msg_ptr->msg.sig == MR_NO_PS_ACCESS_IND)
    {
      ul.b_access = FALSE;

      (void)gllc_grr_ul_tbf_rel_ind();

      /* Clear data structures relating to current TBF.
      ** Clear tx window, clear all resources granted,
      ** keep current and outstanding pdus, clear all
      ** signal masks
      */
      clr_ul_data();

      /* Back to NULL state */
      ul.state = US_NULL;

      /* End of UL tbf for Test mode */
      ul.test_mode = TEST_MODE_OFF;
    }
    else
    if (mac_msg_ptr->msg.sig == MR_UL_RELEASE_IND)
    {
      ul.ul_release_cause = mac_msg_ptr->msg.ul_release_ind.cause;
      ul.test_mode_ul_tbf_rel_after_fai = TRUE;

      MSG_GERAN_LOW_1_G("GRLU Test Mode Rcvd REL_IND cause %d",ul.ul_release_cause);
      rlc_log_ul_release_ind( (uint8)ul.cur_res.ul_tfi,RLC_LOG_UL_MAC_RELEASE_IND);
    }
    else if( mac_msg_ptr->msg.sig == MR_UL_PKT_ACKNACK_IND)
    {
      if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GERAN_LOW_2_G("ERLU TEST MODE paknak FA=%d SSN=%d",
                mac_msg_ptr->msg.ul_pkt_acknack_ind.egprs_acknack_desc.fai,
                mac_msg_ptr->msg.ul_pkt_acknack_ind.egprs_acknack_desc.ssn
                );

        ul.egprs_acknack_desc = mac_msg_ptr->msg.ul_pkt_acknack_ind.egprs_acknack_desc;

        /* If FAI is set or release indication has received previously with cause normal release,
        ** then release the UL TBF. Note that NW should have sent FAI after normal release, but
        ** instead of waiting for FAI just release the UL TBF. TBC.
        */
        if( (ul.egprs_acknack_desc.fai == RM_ACKNACK_FAI_TBF_COMPLETE) ||
            (ul.test_mode_ul_tbf_rel_after_fai == TRUE ))
        {
          /* Inform MAC of RLC initiated ul release
          */
          indicate_ul_release (  REL_UL_ABNORMAL_WITHOUT_RETRY );
          (void)gllc_grr_ul_tbf_rel_ind();

          /* Clear data structures relating to current TBF.
          ** Clear tx window, clear all resources granted,
          ** keep current and outstanding pdus, clear all
          ** signal masks
          */
          clr_ul_data();

          /* Back to NULL state */
          ul.state = US_NULL;

          /* End of UL tbf for Test mode */
          ul.test_mode = TEST_MODE_OFF;
          ul.b_access = FALSE;
        } /* FAI */

        /* Log EPUAN
        ** Log Code: 0x5209
        */
        rlc_log_epuan( &(mac_msg_ptr->msg.ul_pkt_acknack_ind));

      } /* EGPRS UNACK EPUAN */
      else
      {
        MSG_GERAN_LOW_2_G("GRLU TEST MODE paknak FA=%d SSN=%d",
                mac_msg_ptr->msg.ul_pkt_acknack_ind.acknack_desc.fai,
                mac_msg_ptr->msg.ul_pkt_acknack_ind.acknack_desc.ssn
                );

        /* GPRS UNACK PUAN in EGPRS build
        */
        ul.ul_acknack = mac_msg_ptr->msg.ul_pkt_acknack_ind.acknack_desc;

        /* If FAI is set or release indication has received previously with cause normal release,
        ** then release the UL TBF. Note that NW should have sent FAI after normal release, but
        ** instead of waiting for FAI just release the UL TBF. TBC.
        */
        if( (ul.ul_acknack.fai == RM_ACKNACK_FAI_TBF_COMPLETE) ||
            (ul.test_mode_ul_tbf_rel_after_fai == TRUE ))
        {
          /* Inform MAC of RLC initiated ul release
          */
          indicate_ul_release (  REL_UL_ABNORMAL_WITHOUT_RETRY );
          (void)gllc_grr_ul_tbf_rel_ind();

          /* Clear data structures relating to current TBF.
          ** Clear tx window, clear all resources granted,
          ** keep current and outstanding pdus, clear all
          ** signal masks
          */
          clr_ul_data();

          /* Back to NULL state */
          ul.state = US_NULL;

          /* End of UL tbf for Test mode */
          ul.test_mode = TEST_MODE_OFF;

          /* Consider the TBF released. Now load the next PDU and establish new TBF
          ** otherwise go back to NULL
          */
          ul.b_access = FALSE;
        } /* FAI */
      }  /* GPRS UNACK EPUAN */
    } /* PKT_ACKNACK */
    else if (mac_msg_ptr->msg.sig == MR_UL_ALLOC_CNF)
    {
      /* Refresh sysinfo during transfer
      */
      read_ul_sysinfo();

      MSG_GERAN_LOW_0_G("GRLU Test Mode rcved alloc_cnf");
      ul.b_first_fifo_load = TRUE;

      if(ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        /* Update coding scheme for engg_mode
        */
        if (ul.cur_res.cs != mac_msg_ptr->msg.ul_alloc_cnf.mcs)
        {
          geran_eng_mode_data_write(ENG_MODE_EGPRS_UL_MCS, &mac_msg_ptr->msg.ul_alloc_cnf.mcs, TRUE);
        }

        ul.cur_res    = mac_msg_ptr->msg.ul_alloc_cnf;
        ul.cur_res.cs = mac_msg_ptr->msg.ul_alloc_cnf.mcs;
      }
      else
      {
        /* Update coding scheme for engg_mode
        */
        if (ul.cur_res.cs != mac_msg_ptr->msg.ul_alloc_cnf.cs)
        {
          geran_eng_mode_data_write(ENG_MODE_GPRS_UL_CS, &mac_msg_ptr->msg.ul_alloc_cnf.cs, TRUE);
        }

        ul.cur_res    = mac_msg_ptr->msg.ul_alloc_cnf;
      }

    }
    else
    {
      MSG_GERAN_LOW_1_G("GRLU Test Mode rcvd ivalid MAC sig %d", mac_msg_ptr->msg.sig);
    }

    GPRS_MEM_FREE(mac_msg_ptr);

  } /* mac_msg_ptr != NULL */

} /* grlc_ul_handle_mac_event_us_test_mode */

/*===========================================================================
===
===  FUNCTION      RLC_UL_STATE_MACHINE()
===
===  DESCRIPTION
===
===    RLC uplink state machine.
===
===    RLC uplink event handler calls this function when an external
===    uplink event occurs.
===
===    This function acts as a front-end, it deals with the events that can
===    occur in each state. Some limited processing is also done in here.
===    Other functions are distributed in functions such as ack_mode_xfer()
===    unack_mode_xfer() and ack_release() and unack_release()
===
===  DEPENDENCIES
===
===    est_tbf_cnf -- same info as in MAC's tbf est cnf mesage
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
static void rlc_ul_state_machine (void)
{
  rm_timer_msg_t  *timer_msg_ptr;

  /*---------------------------------------------------------------*/

  /* Process and show debug messages suppressed during ISR (post_tx_actions)
  ** in task-time
  */
  GRLC_UL_SHOW_POST_TX_BLKS();

  MSG_GERAN_LOW_3_G("GRLU Info s=%d ll-pdus=%d ll-octs=%lu",ul.state,ul.llc_pdus,ul.llc_octets);

  /* No-data recovery code
  */
  if (grlc_ul_no_data_ctr >= GRLC_UL_NO_DATA_THRESHOLD)
  {
    /* Reset counter so that the dumping will continue
    */

    MSG_GERAN_LOW_3_G("GRLU ##nodata %u ext-utbf=%u pre-emptive-tx=%u",
                        GRLC_UL_NO_DATA_THRESHOLD,
                        IS_EXT_UTBF_ACTIVE(),
                        ul.pre_emptive_tx);

    GRLC_UL_NO_DATA_LOCK();
    grlc_ul_no_data_ctr = 0;
    GRLC_UL_NO_DATA_UNLOCK();

    /* Dump GRLC vars to QXDM in task time
    */
    grlc_ul_show_vars();
  }

  /* Check trigger for a flow control update
  */
  if (ul.b_adj_flow_control)
  {
    grlc_ul_adjust_flow_control_thresholds();

    /* Done adjusting flow control, then reset the flag
    */
    ul.b_adj_flow_control = FALSE;
  }

  switch(ul.state)
  {
    /*==================================================================================
    **  N U L L    S T A T E
    **==================================================================================
    */
    case US_NULL:
      /*--------------------------------
      ** State:US_NULL, event = LLC
      ** Expect GRR primitives from LLC
      **--------------------------------
      */
      if (ul.event == UE_LLC)
      {
        /* LLC calls enqueue_pdu() directly so there is no message here.
        ** As a precaution, check the access state and act on it
        */
        if (ul.b_access)
        {
          (void)load_cur_pdu();
          est_tbf_from_null();
        }
      }
      else if (ul.event == UE_PL1)
      /*------------------------------
      ** State:US_NULL, event = PL1
      **------------------------------
      */
      {
        /* PL1 only sets a Rex mask. No message body
        */
        MSG_GERAN_LOW_0_G("GRLU US_NULL Unexp PL1 sig");
      }
      else if (ul.event == UE_MAC)
      /*------------------------------
      ** State:US_NULL, event = MAC
      **------------------------------
      */
      {
        grlc_ul_handle_mac_event_us_null();
      }
      else if (ul.event == UE_TMR)
      {
        grlc_ul_handle_tmr_event_us_null();
      }
      else
      {
        MSG_GERAN_LOW_1_G("GRLU NULL Unexp evt %d",ul.event);
      }
      break;

    /*==================================================================================
    **  A W A I T    A S S I G N M E N T    S T A T E
    **==================================================================================
    */
    case US_AWAIT_ASS:

      if (ul.event == UE_MAC)
      {
        grlc_ul_handle_mac_event_us_await_ass();
      } /* end MAC event */

      else if (ul.event == UE_LLC)
      /*-----------------------------------
      **  State: US_AWAIT_ASS, event = LLC
      **-----------------------------------
      */
      {
        /* LLC already enqueued PDU and RLC waits until a TBF is active
        */
        ;
      }
      else if (ul.event == UE_TMR)
      /*-------------------------------------
      **  State: US_AWAIT_ASS, event = TIMER
      **-------------------------------------
      */
      {
        timer_msg_ptr = (rm_timer_msg_t *)q_get(&UTQ());
        if (timer_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_0_G("GRLU AWAIT_ASS tmr event without msg");
        }
        else
        {
          if (timer_msg_ptr->sig == RM_TMR_AWAIT_ASS_GUARD)
          {
            MSG_GERAN_HIGH_1_G("GRLU AWAIT_ASS Expy AWAIT_ASS_GUARD = %u",
                      ul.timer_await_ass_guard_expiry_cnt);

            ul.state = US_AWAIT_RECOVERY;

            /* If PANIC RESET RESTART is enabled perform soft recovery otherwise
            ** do an ERR_FATAL to stop where it occurs.
            */
            if( geran_get_nv_recovery_restart_enabled())
            {
              /* Invoke Panic Reset
              */
              geran_initiate_recovery_restart(GERAN_CLIENT_GRLC);
            }
            else
            {
              ERR_GERAN_FATAL_0_G("GRLU RLC AWAIT_ASS Expy AWAIT_ASS_GUARD Expiry");
            }
          }
          else
          if (timer_msg_ptr->sig == RM_TMR_AWAIT_MAC_UL_RESP)
          {
            MSG_GERAN_LOW_0_G("GRLU AWAIT_ASS Expy MAC_RESP");
          }
          else
          {
            MSG_GERAN_LOW_1_G("GRLU AWAIT_ASS Unexp TMR sig %d",timer_msg_ptr->sig);
          }

          GPRS_MEM_FREE(timer_msg_ptr);
        }
      }
      else if (ul.event == UE_PL1)
      /*-----------------------------------
      **  State: US_AWAIT_ASS, event = PL1
      **-----------------------------------
      */
      {
        MSG_GERAN_LOW_0_G("GRLU AWAIT_ASS Unexp PL1 sig");
      }
      else
      /*------------------------------------------
      **  State: US_AWAIT_ASS, event = unexpected
      **------------------------------------------
      */
      {
        MSG_GERAN_LOW_1_G("GRLU AWAIT_ASS Unexp event %d",ul.event);
      }
      break;

    /*==================================================================================
    **  A C K N O W L E D G E D    T R A N S F E R    S T A T E
    **==================================================================================
    */
    case US_ACK_XFER:

      if (ul.event == UE_PL1)
      {
#ifdef  FEATURE_GPRS_PS_HANDOVER
        if (ul.b_psho_active)
        {
          /* Ignore PH_READY_IND from PL1 in PSHO
          */
          MSG_GERAN_MED_0_G("GRLU A Ignore PH_READY_TO_SEND in PSHO");
        }
        else if (ul.b_psho_llc_pdu_absent)
        {
          /* in case of WtoG handover , cur_pdu_ptr will be null for a
          ** short period of time until we get rau_req or cell_update.
          */
          MSG_GERAN_HIGH_0_G("GRLU A PSHO cur_pdu_ptr NULL PH_READY_TO_SEND Ignored");
        }
        else
        {
          /* This event represents the PH_READY_TO_SEND_IND primitive from PL1.
          ** In response we invoke the ack mode transfer function to produce
          ** data blocks to feed the UL FIFO
          */
          MSG_GERAN_LOW_0_G("GRLU A Rcvd PH_READY_TO_SEND");

          if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
          {
            e_ack_mode_xfer();
          }
          else
          {
            ack_mode_xfer();
          }
        }
#else   /* FEATURE_GPRS_PS_HANDOVER  */
        /* This event represents the PH_READY_TO_SEND_IND primitive from PL1.
        ** In response we invoke the ack mode transfer function to produce
        ** data blocks to feed the UL FIFO
        */
        MSG_GERAN_LOW_0_G("GRLU A Rcvd PH_READY_TO_SEND");

        if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
        {
          e_ack_mode_xfer();
        }
        else
        {
          ack_mode_xfer();
        }
#endif /* FEATURE_GPRS_PS_HANDOVER  */
      }
      else if (ul.event == UE_MAC)
      /*--------------------------------
      **  State: ACK_XFER, event = MAC
      **--------------------------------
      */
      {
        grlc_ul_handle_mac_event_us_ack_xfer();
      }

      else if (ul.event == UE_LLC)
      /*--------------------------------
      **  State: ACK_XFER, event = LLC
      **--------------------------------
      */
      {
        MSG_GERAN_LOW_0_G("GRLU XFER UE_LLC");

#ifdef  FEATURE_GPRS_PS_HANDOVER
        if(ul.b_psho_llc_pdu_absent)
        {
          /* Coming out of PSHO_COMPLETE_IND cur_pdu_ptr may not be loaded.
          ** this could mostly happen in WtoG PSHO or when only DL active.
          */
          (void)load_cur_pdu();
          grlc_ul_handle_alloc_cnf_await_ass();
        }
        else
        {
          /* LLC already enqueued the PDU so carry on with the transfer
          */
          if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
          {
            e_ack_mode_xfer();
          }
          else
          {
            ack_mode_xfer();
          }
        }
#else /* FEATURE_GPRS_PS_HANDOVER */
        /* LLC already enqueued the PDU so carry on with the transfer
        */
        if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
        {
          e_ack_mode_xfer();
        }
        else
        {
          ack_mode_xfer();
        }
#endif /* FEATURE_GPRS_PS_HANDOVER */
      }
      else if (ul.event == UE_TMR)
      /*-----------------------------------
      **  State: US_ACK_XFER, event = TMR
      **-----------------------------------
      */
      {
        grlc_ul_handle_tmr_event_us_ack_xfer();
      }
      break;

#ifdef  FEATURE_GSM_DTM
    /*==================================================================================
    **  G P R S   T R A N S P A R E N T   T R A N S F E R   P R O T O C O L   S T A T E
    **==================================================================================
    */
    case US_GTTP_XFER:

      if (ul.event == UE_MAC)
      {
        grlc_ul_handle_mac_event_us_gttp_xfer();
      }
      else
      {
        MSG_GERAN_LOW_1_G("GRLU GTTP Unexp event %u",ul.event);
      }
      break;
#endif /* FEATURE_GSM_DTM */

    /*==================================================================================
    **  U N A C K N O W L E D G E D    T R A N S F E R    S T A T E
    **==================================================================================
    */
    case US_UNACK_XFER:
      /*---------------------------------------------------------------------
      ** Expect signals from:
      ** LLC:
      ** - UNITDATA/DATA_REQ more pdus to get queue'd up
      ** MAC:
      ** - packet uplink acknack
      ** - realloc confirmation
      ** - unsolicited realloc indicator
      ** L1:
      ** - PH_DATA_READY_TO_SEND_IND or blocks just taken out of ul fifo.
      ** TMR:
      **---------------------------------------------------------------------
      */
      if (ul.event == UE_PL1)
      {
#ifdef  FEATURE_GPRS_PS_HANDOVER
        if (ul.b_psho_active)
        {
          /* Ignore PH_READY_IND from PL1 in PSHO
          */
          MSG_GERAN_MED_0_G("GRLU A Ignore PH_READY_TO_SEND in PSHO");
        }
        else if (ul.b_psho_llc_pdu_absent)
        {
          /* in case of WtoG handover , cur_pdu_ptr will be null for a
          ** short period of time until we get rau_req or cell_update.
          */
          MSG_GERAN_HIGH_0_G("GRLU U PSHO cur_pdu_ptr NULL PH_READY_TO_SEND Ignored");
        }
        else
        {
          /* deal with PH_DATA_READY_TO_SEND primitive from PL1 by invoking unack
          ** mode transfer function to produce data blocks to send to the RLC UL/
          ** PL1 FIFO
          */
          MSG_GERAN_LOW_0_G("GRLU U Rcvd PH_READY_TO_SEND_IND");

          if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
          {
            e_unack_mode_xfer();
          }
          else
          {
            unack_mode_xfer();
          }
        }
#else   /* FEATURE_GPRS_PS_HANDOVER  */
        /* deal with PH_DATA_READY_TO_SEND primitive from PL1 by invoking unack
        ** mode transfer function to produce data blocks to send to the RLC UL/
        ** PL1 FIFO
        */
        MSG_GERAN_LOW_0_G("GRLU U Rcvd PH_READY_TO_SEND_IND");

        if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
        {
          e_unack_mode_xfer();
        }
        else
        {
          unack_mode_xfer();
        }
#endif /* FEATURE_GPRS_PS_HANDOVER */
      } /* UNACK_XFER UE_PL1 */

      else if (ul.event == UE_MAC)
      /*-------------------------------------
      **  State: US_UNACK_XFER, event = MAC
      **-------------------------------------
      */
      {
        grlc_ul_handle_mac_event_us_unack_xfer();
      }

      else if (ul.event == UE_LLC)
      /*-------------------------------------
      **  State: US_UNACK_XFER, event = LLC
      **-------------------------------------
      */
      {
        MSG_GERAN_LOW_0_G("ERLU U Rcvd UE_LLC");

#ifdef  FEATURE_GPRS_PS_HANDOVER
        if(ul.b_psho_llc_pdu_absent)
        {
          /* Coming out of PSHO_COMPLETE_IND cur_pdu_ptr may not be loaded.
          ** this could mostly happen in WtoG PSHO or when only DL active.
          */
          (void)load_cur_pdu();

          grlc_ul_handle_alloc_cnf_await_ass();
        }
        else
        {
          if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
          {
            e_unack_mode_xfer();
          }
          else
          {
            unack_mode_xfer();
          }
        }
#else /* FEATURE_GPRS_PS_HANDOVER */
        if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
        {
          e_unack_mode_xfer();
        }
        else
        {
          unack_mode_xfer();
        }
#endif /* FEATURE_GPRS_PS_HANDOVER */
      }
      else if (ul.event == UE_TMR)
      /*-------------------------------------
      **  State: US_UNACK_XFER, event = TMR
      **-------------------------------------
      */
      {
        grlc_ul_handle_tmr_event_us_unack_xfer();
      } /* TMR event */
      break;

    /*==================================================================================
    **  T E S T    M O D E    S T A T E
    **==================================================================================
    */
    case US_TEST_MODE:

      if (ul.event == UE_PL1)
      {
        /*-------------------------------------
        **  State: US_TEST_MODE, event = PL1
        **-------------------------------------
        */
        ul.vs = 0; /* reset index */
      }
      else if (ul.event == UE_MAC)
      {
        /*-------------------------------------
        **  State: US_TEST_MODE, event = MAC
        **-------------------------------------
        */
        grlc_ul_handle_mac_event_us_test_mode();
      }

      else if (ul.event == UE_LLC)
      /*-------------------------------------
      **  State: US_TEST_MODE, event = LLC
      **-------------------------------------
      */
      {
        MSG_GERAN_LOW_0_G("GRLU Test Mode rcvd ivalid LLC sig");
      }

      else if (ul.event == UE_TMR)
      /*-------------------------------------
      **  State: US_TEST_MODE, event = TMR
      **-------------------------------------
      */
      {
        timer_msg_ptr = (rm_timer_msg_t *)q_get(&UTQ());
        if (timer_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_0_G("GRLU Test Mode rcvd timer event without msg");
        }
        else
        {
          MSG_GERAN_LOW_1_G("GRLU Test Mode rcvd Inval timer sig %d",timer_msg_ptr->sig);
          GPRS_MEM_FREE(timer_msg_ptr);
        }
      }
      else
      {
        MSG_GERAN_LOW_1_G("GRLU Test Mode rcvd Inval UL EVENT %d",ul.event);
      }
      break; /* US_TEST_MODE */

    /*==================================================================================
    **  A W A I T    R E C O V E R Y    S T A T E
    **==================================================================================
    */
    case US_AWAIT_RECOVERY:
      if (ul.event ==UE_PL1)
      /*-----------------------------------------
      **  State: US_AWAIT_RECOVERY, event = PL1
      **-----------------------------------------
      */
      {
        /* No real messages from PL1
        */
        RLC_UL_CLR_PL1_SIG();
      }
      else
      if (ul.event == UE_MAC)
      /*-----------------------------------------
      **  State: US_AWAIT_RECOVERY, event = MAC
      **-----------------------------------------
      */
      {
        mac_rlc_msg_t *mac_msg_ptr;
        mac_msg_ptr = (mac_rlc_msg_t *)q_get(&UMQ());

        if (mac_msg_ptr != NULL)
        {
          if (mac_msg_ptr->msg.sig == MR_UL_RESET_REQ)
          {
            grlc_ul_panic_reset();
          }
          else
          {
            MSG_GERAN_MED_1_G("GRLU AWAIT_RECOVERY MAC sig %d", mac_msg_ptr->msg.sig);
          }

          GPRS_MEM_FREE(mac_msg_ptr);
        } /* end MAC msg not NULL */
        else
        {
          MSG_GERAN_LOW_0_G("GRLU US_NULL event without msg");
        }
      }
      else if (ul.event == UE_LLC)
      /*-----------------------------------------
      **  State: US_AWAIT_RECOVERY, event = LLC
      **-----------------------------------------
      */
      {
        /* We shall flush the messages later
        */
        RLC_UL_CLR_LLC_PDU_SIG();
        RLC_UL_CLR_LLC_TMP_PDU_SIG();
        RLC_UL_CLR_LLC_MSG_SIG();
      }
      else if (ul.event == UE_TMR)
      /*-----------------------------------------
      **  State: US_AWAIT_RECOVERY, event = TMR
      **-----------------------------------------
      */
      {
        timer_msg_ptr = (rm_timer_msg_t *)q_get(&UTQ());
        if (timer_msg_ptr != NULL)
        {
          MSG_GERAN_HIGH_1_G("GRLU AWAIT_RECOVERY clr timer sig %d", timer_msg_ptr->sig);
          GPRS_MEM_FREE(timer_msg_ptr);
        }
      }
      break;


    default:
      /* illegal RLC UL state
      */
      MSG_GERAN_LOW_1_G("GRLC invalid state %d",ul.state);
      break;

  } /* ul.state switch */

#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
  grlc_ul_show_paq();
#endif

} /* rlc_ul_state_machine */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_PANIC_RESET
===
===  DESCRIPTION
===
===    Perform a warm reset on the RLC UL state machine. This procedure
===    frees and clears all un-delivered PDUs and incoming messages
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_panic_reset (void)
{
  /* pointer to buffer holding message intended for MAC
  */
  rlc_mac_msg_t   *rlc_msg_ptr;

  /*---------------------------------------------------------------------*/

  ul.b_access = FALSE;

  grlc_timer_stop_stale_pdu_delete();
  ul_static.b_ul_pdu_delete_after_stale_tmr_expiry = FALSE;

  GRLC_UL_MSG_LOCK();

  /* Clear all uplink variables, timers, FIFO, state/sub-state vars
  */
  clr_ul_data();

  /* unput all the pdus in paq to the main queue
  */
  grlc_ul_unput_paq();

  /* Clear PDU grouping info
  */
  pg_clr_tbc_info( &pg.tbci);

#ifndef FEATURE_GERAN_REDUCED_DEBUG
  /* Clear Diagnostic and statistic info
  */
  grlc_diag_clr_ul_statistics();
  grlc_diag_clr_ul_abnormal_release_counts();
  grlc_diag_clr_ul_event_counts();
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

  /* Read out all PDUs in the queue and free all DSM items and clear the messages.
     No STATUS_IND sent if SM PDU is deleted, hence FALSE. */

  flush_llc_pdu_queue(  FALSE );

  /* Read out all LLC signalling messages and free them
  */
  flush_llc_sig_queue();

  /* Read out all remaining MAC messages and free them
  */
  flush_mac_sig_queue();

  /* Read out all remaining TIMER messages and free them
  */
  flush_tmr_sig_queue();

  /* Allow other tasks to run now
  */
  GRLC_UL_MSG_UNLOCK();

  ++ul.panic_reset_cnt;
  MSG_GERAN_LOW_1_G("GRLU Resetting %u",ul.panic_reset_cnt);

  /* Send Reset Confirmation to MAC
  */
  rlc_msg_ptr = mac_get_cmd_buf_rlc();
  if (rlc_msg_ptr != NULL)
  {
    rlc_msg_ptr->msg.ul_reset_cnf.sig = RM_UL_RESET_CNF;
    mac_put_cmd_rlc( rlc_msg_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("ERLU GRLD heap exhaustion");
  }

  /* Re-assess flow control given that PDUs may have been deleted
  */
  grlc_llc_ul_flow_control();

} /* grlc_ul_panic_reset */

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GRLC_UL_ONE_TIME_INIT
===
===  DESCRIPTION
===
===    Power up init of rlc downlink operation.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_ul_one_time_init (void)
{
    /* Initialise GRLC UL critical sections */
    rex_init_crit_sect(&grlc_ul_msg_crit_sec);
    rex_init_crit_sect(&grlc_ul_no_data_crit_sec);
    rex_init_crit_sect(&grlc_ul_tx_vars_crit_sec);
    rex_init_crit_sect(&grlc_ul_procedure_crit_sec);

    ul.b_error_recovery_required = FALSE;

  if (ul.b_done_ul_one_time_init == FALSE)
  {
    grlc_ul_init_paq();
    pg.psn = 0;

#ifdef DEBUG_GSM_GPRS_RLC_TEST
    #error code not present
#else
/* If FEATURE_MODEM_RCINIT is defined, then cannot access rr_read_public_store as GSM_RR task may not
   have completed initialisation - read_ul_sysinfo() will be accessed when really required anyway*/
#endif

#ifdef  FEATURE_GPRS_PS_HANDOVER
    ul.b_psho_active = FALSE;
    ul.b_psho_llc_pdu_absent = FALSE;
#endif /* FEATURE_GPRS_PS_HANDOVER */
    ul.b_access = FALSE;
    ul.b_done_ul_one_time_init = TRUE;
    ul.panic_reset_cnt = 0;

    /* Define all the general purpose (signal sent on expiry) timers
    */
    ul.timer_mac_ul_resp.state = TMR_INACTIVE;
    rex_def_timer_ex(&(ul.timer_mac_ul_resp.rex_timer),
                     (rex_timer_cb_type)grlc_ul_timer_callback,
                     ((uint32)TIMER_SET_PARAM( RM_TMR_AWAIT_MAC_UL_RESP))
                    );

    ul.timer_t3182.state = TMR_INACTIVE;
    ul.timer_t3182_expiry_cnt = 0;
    rex_def_timer_ex(&(ul.timer_t3182.rex_timer),
                     (rex_timer_cb_type)grlc_ul_timer_callback,
                     ((uint32)TIMER_SET_PARAM( RM_TMR_T3182_AWAIT_UL_ACKNACK))
                    );

    ul.timer_await_ul_acknack.state = TMR_INACTIVE;
    ul.timer_await_ul_acknack_expiry_cnt = 0;
    rex_def_timer_ex(&(ul.timer_await_ul_acknack.rex_timer),
                     (rex_timer_cb_type)grlc_ul_timer_callback,
                     ((uint32)TIMER_SET_PARAM( RM_TMR_AWAIT_UL_ACKNACK))
                    );

    ul.timer_await_ass_guard.state = TMR_INACTIVE;
    ul.timer_await_ass_guard_expiry_cnt = 0;
    rex_def_timer_ex(&(ul.timer_await_ass_guard.rex_timer),
                     (rex_timer_cb_type)grlc_ul_timer_callback,
                     ((uint32)TIMER_SET_PARAM( RM_TMR_AWAIT_ASS_GUARD))
                    );

    /*----------------------------------------------------------------------
    ** if block timers service isr is not provided by the loopback (GDSTST)
    ** envir or the DSPE envir or the actual PL1 code then register a
    ** callback to do it
    **----------------------------------------------------------------------
    */
#ifdef  DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

    /* Init RLC UL FIFO */
    grlc_ul_fifo_one_time_init();

    /* set this counter assuming that cell reselection had just taken place */
    SET_N3102();
#ifdef  GSM_GPRS_GRLU_SHOW_N3102
    MSG_GERAN_MED_3_G("GRLU OTInit SetN3102=max=%d inc=%d dec=%d",ul.n3102,PAN_INC(),PAN_DEC());
#endif

    /* .. put other one time initialisations here .. */

    ul.flow_control_suspend = 0;
    ul.psn_success_ind = 0;

    /* Use default coding scheme 1 until PUAN says otherwise */
    ul.cur_res.cs = RM_CS_1;
    ul.tlli_cs = RM_CS_1;

    /* Clear internal logging vars (additional to Diag's) every TBF */
    ul.nack_cnt = 0;
    ul.ptx_cnt = 0;
    ul.pack_cnt = 0;
    ul.num_ests = 0;

    /* Not in SUSPENSION initially */
    ul.suspend_state = GRLC_RESUME;

#ifndef FEATURE_GERAN_REDUCED_DEBUG
    /* Init Diag-related vars */
    ul_diag_stats_st.grlc_ul_state = (uint32)US_NULL;
    ul_diag_stats_st.grlc_ul_substate = (uint32)RALS_NULL;

    /* Clear Diagnostic info */
    grlc_diag_clr_ul_statistics();
    grlc_diag_clr_ul_abnormal_release_counts();
    grlc_diag_clr_ul_event_counts();
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

    /*------------------------------------------------------------------------
    ** Init LLC flow control info once at power up.
    **------------------------------------------------------------------------
    */
    ul.llc_octets = 0;
    ul.llc_pdus   = 0;
    ul.susp_state_data_pdus = 0;

    ul.pfi = RM_PFI_INVALID;

    ul.prev_realloc_psn = 0;

    /* EGPRS specific inits here */

    /* Set the tbf_mode to GPRS as default */
    ul.cur_res.tbf_mode = TBF_MODE_GPRS;

    /* .. Put other EGPRS inits here */
    /* ..E_NEW_DATA_BLOCK() */
  }

#ifdef GSM_GPRS_GRLU_CONFIRM_TX
  grlc_ul_init_post_tx_show_fifo();
#endif /* GSM_GPRS_GRLU_CONFIRM_TX */

} /* grlc_ul_one_time_init() */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_INIT_VARS
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    return value written to module var cv.
===
===========================================================================*/
void grlc_ul_init_vars(void)
{
  /*------------------------------------------------*/

  clr_ul_data();

  /* clear current and next pdu pointers
  */
  cur_pdu_ptr = NULL;
  next_pdu_ptr = NULL;

  /* load defaults
  */
  ul.event = UE_NULL;
  ul.rlc_mode = GRLC_MODE_UNACK;

  ul.test_mode_ul_tbf_rel_after_fai = FALSE;

  ul.last_valid_coding_scheme = ul.cur_res.cs;

  ul.re_tx_cv_0_cnt = 0;
  ul.octets_req = 0;
  ul.tbc = 0;
  ul.blocks_req = 0;
  ul.blocks_rem = 0;

  ul.cv = BS_CV_CONST;

  ul.psn_success_ind = 0;

  /* Ensure no reallocation when validity is in doubt
  */
  ul.ptx_cv = BS_CV_CONST;
  ul.pak_cv = BS_CV_CONST;

  ul.b_forced_release = FALSE;

  ul.b_first_fifo_load = TRUE;

  ul.contention_status = RM_CONTENTION_UNRESOLVED;
  ul.b_first_blk_sent = FALSE;

  ul.realloc_state = RALS_NULL;
  UPD_UL_SUBSTATE();

  ul.si = RM_TX_NOT_STALLED;

  ul.pfi = RM_PFI_INVALID;

  /* init current block
  */

  /* init tx vars
  */
  ul.vs = 0;
  UPD_UL_VS();

  ul.va = 0;
  UPD_UL_VA();

  ul.vsf = 0;
  UPD_UL_VSF();

  ul.nack_vsf = 0;
  UPD_UL_NACK_VSF();

  ul.pack_vsf = 0;
  UPD_UL_PACK_VSF();

  ul.nack_cnt = 0;
  ul.ptx_cnt = 0;
  ul.pack_cnt = 0;
  ul.num_ests = 0;

  /* Clear USF info relating to uplink flow control
  */
  ul.usf_info.total_usfs = 0;
  ul.usf_info.total_blocks_blanked = 0;
  ul.usf_info.num_consecutive_blanked_blocks = 0;
  ul.usf_info.blanking_percentage_average = 0;
  ul.b_adj_flow_control = FALSE;

  ul.temp_queue_octet_cnt = 0;

#ifdef GSM_GPRS_GRLU_CONFIRM_TX
  grlc_ul_init_post_tx_show_fifo();
#endif /* GSM_GPRS_GRLU_CONFIRM_TX */

} /* grlc_ul_init */


/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_NO_PS_ACCESS()
===
===  DESCRIPTION
===
===    Set sub-state to no access and delete PDU as per indicated by the flag.
===    If RLC is in user suspended state then delete all PDUs. Otherwise, delete
===    the current PDU if DEL_ONE is indicated, or delete no PDU if DEL_NONE is
===    indicated
===
===  DEPENDENCIES
===
===  PARAM
===    delete_pdu_flag: DEL_NONE or DEL_ONE
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_handle_no_ps_access (delete_pdu_cause_t flag)
{
  TIMER_STOP_AWAIT_ASS_GUARD();

  ul.b_access = FALSE;

  /* When cont-res fails, reset N3104
  */
  if (IS_TLLI_REQUIRED())
  {
    MSG_GERAN_MED_2_G("GRLU CRes failed ResN3104=%d max=%d",ul.n3104, N3104_MAX());
    RESET_N3104();
  }

  if (ul.pdu_freeing_state == FREE_CURRENT_PDU)
  {
    /* Successful transmission of PDU , set the flag to TRUE
    ** to start ready timer
    */
    grlc_ul_del_free_paq_upto_psn( ul.psn_to_free_inclusive,TRUE);
  }

  if (flag == DEL_ONE || flag == DEL_ONE_SIG)
  {
    if (cur_pdu_ptr != NULL)
    {
      /* Delete SM PDU if UE is not in any xfer state
      */
      if ( cur_pdu_ptr->cause == GRR_CAUSE_SM_SIGNALLING &&
          ul.state != US_ACK_XFER &&
          ul.state != US_UNACK_XFER
#ifdef  FEATURE_GSM_DTM
          && ul.state != US_GTTP_XFER
#endif /* FEATURE_GSM_DTM */
         )
      {

        /* Inform NAS when SIG PDU is deleted and not transmitted
        ** so it can be retried quickly, rather than waiting for large
        ** duration NAS timers
        */
        if ((cur_pdu_ptr->cause == GRR_CAUSE_SM_SIGNALLING) &&
            (! is_pdu_transmitted( cur_pdu_ptr->psn))
           )
        {
          MSG_GERAN_LOW_1_G("GRLU inform SM no ps access - DEL_ONE psn %u", cur_pdu_ptr->psn);
          indicate_to_llc_rlcmac_failure( cur_pdu_ptr);
        }

        /* Set the flag to FALSE as PDU transmission failed
        */
        grlc_ul_del_free_paq_upto_psn( cur_pdu_ptr->psn,FALSE);
      }
      else
      {
        /* Inform NAS when SIG PDU is deleted  and not fully transmitted
        ** so it can be retried quickly, rather than waiting for large
        ** duration NAS timers
        */
        if ((cur_pdu_ptr->cause == GRR_CAUSE_GMM_SIGNALLING) &&
            (! is_pdu_transmitted( cur_pdu_ptr->psn))
           )
        {
          MSG_GERAN_LOW_0_G("GRLU inform gmm pdu tx failure to upper layer");
          indicate_to_llc_rlcmac_failure( cur_pdu_ptr);
        }

        /* UN-Successful transmission of PDU , set the flag to FALSE
        ** so not to start ready timer
        */
        if ((flag == DEL_ONE_SIG && cur_pdu_ptr->cause != GRR_CAUSE_LAY3_DATA) || flag == DEL_ONE)
        {
          grlc_ul_del_free_paq_upto_psn(cur_pdu_ptr->psn, FALSE);
        }
      }
    }
  }
  else if (flag == DEL_ALL)
  {
    /* Free the current PDU and its backup. Also free the LLC msg and update
    ** the LLC PDU number and octets
    */
    if (cur_pdu_ptr != NULL)
    {
      /* UN-Successful transmission of PDU , set the flag to FALSE
      ** so not to start ready timer
      */
      grlc_ul_del_free_paq_upto_psn( cur_pdu_ptr->psn,FALSE);
      cur_pdu_ptr = NULL;
    }

    /* Purge llc_q here. STATUS_IND sent if SM PDU is deleted, hence TRUE. */

    flush_llc_pdu_queue(  TRUE );

    ul.llc_pdus   = 0;
    ul.llc_octets = 0;
  }

  /* store the no_ps_access cuase such that it can be used for pdu deletion
  ** under no_service case for the incoming signalling pdu.
  */
  ul.no_ps_access_del_pdu_cause = flag;

  /* 8.7.2: abort current TBF and establish new tbf to re-transmit
  ** current and outstanding pdus
  */
  /* Re-init
  */
  clr_ul_data();

  /* Restore PDU
  */
  grlc_ul_unput_paq();

  ul.state = US_NULL;
  UPD_UL_STATE();

  /*
  * Re-order signalling pdus to be sent first to NW
  */
  grlc_ul_reorder_pdus();

  (void)load_cur_pdu();

  grlc_llc_ul_flow_control();

  /* Start Stale PDU delete timer. On expiry, all UL PDUs will be
  ** deleted.
  */
  TIMER_START_STALE_PDU_DELETE();

  /* Check the Commercial Recovery NV setting, if it is DISABLED
  ** ,ie. LAB TESTING PRE-CS then if the PDU threshold is approaching being full
  ** then flush the PDUs keeping at least one newest PDU (more than 1 is OK)
  */
  grlc_ul_detect_and_flush_backlog_pdus();

} /* grlc_ul_handle_no_ps_access */


/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANLDE_INTERNAL_SIG()
===
===  DESCRIPTION
===
===    Handler function to send PRR Type 2. Because the conditions for sending
===    the PRR is triggered from ISR context in post_tx_actions(). Sending the
===    PRR involves allocating memory is now deferred from happening inside the ISR,
===    by setting the GRLC_UL_ITN_SIG mask, which later causes the PRR to be sent
===    in task time
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    ul.event,
===
===========================================================================*/
void grlc_ul_handle_internal_sig_dyn(void)
{
  /* when this function is called the PL1 mask is set in RLC UL space. So
  ** invoke the state machine with the event set to UE_PL1
  */
  ul.event = UE_NULL;

  /* Send PRR with parameters from the next PDU
  */
  /* b_cv_zero - is set to TRUE , because we
     have transitioned to
     realloc_state = RALS_CURRENT_PDU_TX_COMPLETED,
     that means we have Data in our Uplink Q.
  */
  if ( ul.realloc_pdu_ptr != NULL)
  {

    realloc_for_next_pdu( ul.realloc_pdu_ptr, FALSE);

    /* Set RALS_COMPLETE once after sending a PRR.
    ** Expects this realloc_state to change when the next current PDU is loaded
    */
    ul.realloc_state = RALS_COMPLETE;
  }

  GRLC_UL_MSG_LOCK();

  RLC_UL_CLR_ITN_SIG();

  GRLC_UL_MSG_UNLOCK();

} /* grlc_ul_handle_internal_sig */


/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANLDE_PL1_SIG()
===
===  DESCRIPTION
===
===    Wrapper function to invoke the RLC UL state machine to process all
===    L1 commands in the buffer.
===
===  DEPENDENCIES
===
===    grlc_ul_tmr_q
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    ul.event,
===
===========================================================================*/
void grlc_ul_handle_pl1_sig_dyn(void)
{
  /* when this function is called the PL1 mask is set in RLC UL space. So
  ** invoke the state machine with the event set to UE_PL1
  */
  ul.event = UE_PL1;

  /* service the event
  */
  rlc_ul_state_machine();

#ifndef FEATURE_GERAN_REDUCED_DEBUG
  /* Update diag stuff
  */
  ul_diag_evt_cnts_st.pl1_event_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

  /* clear the event
  */
  ul.event = UE_NULL;

  GRLC_UL_MSG_LOCK();

  RLC_UL_CLR_PL1_SIG();

  GRLC_UL_MSG_UNLOCK();

} /* grlc_ul_handle_pl1_sig */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_MAC_SIG()
===
===  DESCRIPTION
===
===    Wrapper function to invoke the RLC UL state machine to process all
===    MAC commands in the buffer.
===
===  DEPENDENCIES
===
===    grlc_ul_mac_q
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    ul.event,
===
===========================================================================*/
void grlc_ul_handle_mac_sig_dyn(void)
{
  /*--------------------------------------------------*/

  ul.event = UE_MAC;

  /* the state machine will pick up the MAC signal queue from the UE_MAC
  ** event and processes the message
  */
  rlc_ul_state_machine();

#ifndef FEATURE_GERAN_REDUCED_DEBUG
  /* Update diag stuff
  */
  ul_diag_evt_cnts_st.mac_event_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

  /* clear the input event after servicing
  */
  ul.event = UE_NULL;

  GRLC_UL_MSG_LOCK();

  if (q_cnt(&UMQ()) == 0)
  {
    RLC_UL_CLR_MAC_SIG();
  }

  GRLC_UL_MSG_UNLOCK();

} /* grlc_ul_handle_mac_sig */


/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_LLC_TMP_PDU_SIG
===
===  DESCRIPTION
===
===    Wrapper function to loop and copy LLC PDUs from the temporary queue
===    and put into RLC main queue. This is to allow PDUs to be put into RLC's
===    PDU queue in RLC context which means the exsiting function enqueue_pdu()
===    can be invoked without changes except for calls to TASKLOCK/FREE().
===
===    The problem was enqueue_pdu() was called in LLC'c context, so TASKLOCK/FREE
===    was added to lock out RLC from gaining context and operated on the same
===    PDU queue and grouping information
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    ul.event,
===
===========================================================================*/
void grlc_ul_handle_llc_tmp_pdu_sig_dyn(void)
{
  /*--------------------------------------------------*/

  gprs_grr_ul_mes_t *msg_ptr;


  GRLC_UL_MSG_LOCK();

  msg_ptr = (gprs_grr_ul_mes_t *)q_get(&ULTQ());

  while ( msg_ptr != NULL )
  {
    /* When the PDU is processed the memory will be freed then
    */

    enqueue_pdu( msg_ptr);
    msg_ptr = (gprs_grr_ul_mes_t *)q_get(&ULTQ());
  }

  RLC_UL_CLR_LLC_TMP_PDU_SIG();

  GRLC_UL_MSG_UNLOCK();

} /* grlc_ul_handle_llc_tmp_pdu_sig */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_LLC_PDU_SIG
===
===  DESCRIPTION
===
===    Wrapper function to loop and invoke the RLC UL state machine to
===    process all LLC PDU(s) in the buffer.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    ul.event,
===
===========================================================================*/
void grlc_ul_handle_llc_pdu_sig_dyn(void)
{
  /*--------------------------------------------------*/

  ul.event = UE_LLC;

  /* invoke the ul state machine
  */
  rlc_ul_state_machine();

#ifndef FEATURE_GERAN_REDUCED_DEBUG
  /* Update diag stuff
  */
  ul_diag_evt_cnts_st.llc_event_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

  /* clear the input event after servicing
  */
  ul.event = UE_NULL;

  /* When LLC needs to send an LL PDU it calls RLC function directly to
  ** place the PDU in RLC's queue. It then sets the LLC-to-RLC signal mask.
  ** The signal mask is only required to be set once for a number of PDUs.
  ** Once informed of the presence of a PDU RLC state machine will continue
  ** processing them without further need to be informed of other PDUs.
  ** That is why the mask is always cleared
  */

  GRLC_UL_MSG_LOCK();

  RLC_UL_CLR_LLC_PDU_SIG();

  GRLC_UL_MSG_UNLOCK();

} /* grlc_ul_handle_llc_pdu_sig */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_LLC_MSG_SIG
===
===  DESCRIPTION
===
===    Wrapper function to loop and process all LLC Messages in the messaging
===    buffer.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
void grlc_ul_handle_llc_msg_sig_dyn(void)
{
  gprs_grr_ul_mes_t *msg_ptr;


  /* Mutex lock /Enter critical section here so that LLC is not able to access
  ** this message queue until RLC completed processing. Not many of following
  ** messages expected any given time.
  */

  GRLC_UL_MSG_LOCK();

  msg_ptr = q_get(&ULMQ());

  while ( msg_ptr != NULL )
  {

    switch ( msg_ptr -> prim )
    {
    case GRR_SUSPEND_REQ:
      grr_suspend_req();
      break;

    case GRR_RESUME_REQ:
      grr_resume_req();
      break;

    case GRR_PURGE_SUSPENDED_DATA_REQ:
      grr_purge_suspended_data_req();
      break;

    case GRR_PURGE_DATA_REQ:
      grr_purge_data_req( msg_ptr->sapi);
      break;

    default:
      MSG_GERAN_LOW_1_G( "GRLU LLC Get: Invalid Prim %d", msg_ptr -> prim);
      break;
    }

    GPRS_MEM_FREE( msg_ptr );

    msg_ptr = q_get(&ULMQ());
  }

  RLC_UL_CLR_LLC_MSG_SIG();

  GRLC_UL_MSG_UNLOCK();

} /* grlc_ul_handle_llc_msg_sig */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_TMR_SIG
===
===  DESCRIPTION
===
===    Wrapper function to loop and invoke the RLC UL state machine to
===    process all UL timer commands in the buffer.
===
===  DEPENDENCIES
===
===    grlc_ul_tmr_q OR UTQ
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    ul.event,
===
===========================================================================*/
void grlc_ul_handle_tmr_sig_dyn(void)
{
  /*--------------------------------------------------*/

  /* mark the event to be serviced
  */
  ul.event = UE_TMR;

  /* invoke state with LLC event
  */
  rlc_ul_state_machine();

#ifndef FEATURE_GERAN_REDUCED_DEBUG
  /* Update diag stuff
  */
  ul_diag_evt_cnts_st.tmr_event_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

  /* clear the input event after servicing
  */
  ul.event = UE_NULL;

  GRLC_UL_MSG_LOCK();

  if (q_cnt(&UTQ()) == 0)
  {
    RLC_UL_CLR_TMR_SIG();
  }

  GRLC_UL_MSG_UNLOCK();

} /* grlc_ul_handle_tmr_sig */


/*** EOF: don't remove! ***/

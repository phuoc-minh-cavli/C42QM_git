/*============================================================================
  @file rr_grr51_grr52_control.c

  @brief This module implements the main GRR state machine that is active when the
  UE is camped on a cell or performing various idle-mode operations.

                Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_grr51_grr52_control.c#12 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_grr.h"
#include "rr_main.h"
#include "rr_general.h"
#include "rr_gprs_defs.h"
#include "rr_gprs_debug.h"
#include "ms.h"
#include "gs.h"
#include "ms_timer.h"
#include "cause.h"
#include "mac_grr.h"
#include "rr_mm_send.h"
#include "rr_l1_send.h"
#include "rr_conn_establish.h"
#include "rr_l1.h"
#include "rr_l2.h"
#include "rr_shared_cell_selection.h"
#include "rr_cell_selection.h"
#include "rr_gprs_conn_establish.h"
#include "rr_sys_info.h"
#include "sys.h"
#include "sys_v.h"
#include "rr_control.h"
#include "rr_sys_info_3g.h"
#include "rr_sys_info_si.h"
#include "rr_resel_calcs.h"
#include "rr_plmn_list.h"
#include "rr_pscan.h"
#include "rr_resel.h"
#include "rr_resel_utils.h"
#include "rr_cell_acq.h"
#include "mm_rr.h"
#include "rr_sys_info_ncell.h"
#include "rr_process_pmo_pcco.h"
#include "rr_main.h"
#include "rr_gprs_dsc.h"

#ifdef FEATURE_GSM_DTM
#include "rr_nv.h"
#ifdef FEATURE_GSM_EDTM
#include "rr_gprs_edtm.h"
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM  */

#include "rr_log.h"

#include "rr_resel_nacc.h"

#ifdef FEATURE_GPRS_PS_HANDOVER
#include "rr_psho.h"
#endif /* FEATURE_GPRS_PS_HANDOVER */

#include "rr_main.h"
#include "rr_mode.h"
#include "rr_sim.h"
#include "rr_l1_idle_mode.h"
#include "rr_ps_access.h"
#include "rr_ps_domain.h"
#include "rr_mac_send.h"
#include "rr_msc.h"
#include "rr_ce_paging.h"
#include "gprs_mem.h"
#include "rr_multi_sim.h"
#include "geran_grr_dyn_api.h"

#ifdef FEATURE_SGLTE
#include "rr_sglte.h"
#include "rr_msgr.h"
#include "rr_sglte_x2g.h"
#endif /* FEATURE_SGLTE */
#include "rr_sys_info_cache.h"
#ifdef FEATURE_CGPS_UMTS_CP_GSM
#include "rr_cgps.h"
#endif /* FEATURE_CGPS_UMTS_CP_GSM */
#include "rr_static_general.h"
/*----------------------------------------------------------------------------
 * External Function and Data Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef struct
{
  grr_state_t     state;
  grr_state_t     old_state;
  rr_event_T      tbf_est_abort_event;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  rr_cco_status_e x2g_cco_status;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  boolean         notify_mm_service_ind;
  boolean         notify_mm_plmn_select_cnf;
  boolean         purge_gmm_signalling;
  boolean         saved_rr_est_req_valid;
  rr_est_req_T    saved_rr_est_req;
  rr_cb_fn_t      ps_access_confim_cb_fn;
  rr_cb_fn_t      no_ps_access_confim_cb_fn;

#ifdef FEATURE_SGLTE
  sys_rat_pri_list_info_s_type sglte_pending_rat_pri_list;
  rr_cb_fn_t                   sglte_ps_removal_cb_fn;
  boolean                      is_sglte_user_rat_change_pending;
#endif /* FEATURE_SGLTE */
} grr_control_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

static grr_control_data_t grr_control_data;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

static grr_state_t transition_to_camped_state(
  boolean service_ind_sent,
  rr_event_T *grr_event,
  rr_cmd_bdy_type **new_message,
  byte *message_set,
  byte *message_id
   
);
static void transition_to_reselection_state(rr_event_T rr_event );
static rr_event_T rr_gprs_camped_timer_handler(rr_cmd_bdy_type *message );
#ifdef FEATURE_GERAN_CS_SUPPORT
static void handle_mac_grr_paging_notification(
  mac_grr_paging_notification_t *paging_notification
   
);
#endif /* FEATURE_GERAN_CS_SUPPORT */
static rr_event_T relinquish_control_to_rr(rr_event_T output_event );
#ifdef FEATURE_GERAN_CS_SUPPORT
static rr_event_T preprocess_rr_est_req(rr_est_req_T *ptr_rr_est_req  );
static rr_event_T handle_rr_est_req(rr_est_req_T *ptr_rr_est_req  );
#endif /* FEATURE_GERAN_CS_SUPPORT */
static rr_event_T handle_rr_plmn_list_req(rr_plmn_list_req_T *rr_plmn_list_req  );
static rr_event_T handle_rr_plmn_select_req(
  rr_plmn_select_req_T * rr_plmn_select_req
   
);
#ifdef FEATURE_GERAN_CS_SUPPORT
static void handle_rr_abort_req( );
#endif /* FEATURE_GERAN_CS_SUPPORT */
static rr_event_T process_saved_message(
  rr_cmd_bdy_type **new_message_ptr_ptr,
  byte *message_set,
  byte *message_id
   
);

#ifdef FEATURE_GPRS_PS_HANDOVER
static rr_event_T rr_gprs_process_psho_command(rr_cmd_bdy_type *new_message );
#if 0
static void rr_gprs_start_psho( );
#endif // #if 0
#endif

#ifdef FEATURE_SGLTE

static void grr_send_omsg_sglte_x2g_abort_cnf_cb_fn(void )
{
  rr_grr_send_omsg(RR_GRR_OMSG_SGLTE_X2G_ABORT_CNF);
}

static void grr_send_omsg_sglte_x2g_reject_ind_cb_fn(void)
{
  rr_grr_send_omsg(RR_GRR_OMSG_SGLTE_X2G_REJECT_IND);
}

static void rr_grr_handle_rr_change_mode_ind(
  rr_change_mode_ind_T * rr_change_mode_ind
   
)
{
  rr_PLMN_request_details_T * rr_plmn_request_details;
  boolean                     old_gsm_only;
  sys_srv_domain_e_type       old_service_domain;

  //RR_NULL_CHECK_RETURN_VOID(rr_change_mode_ind);

  rr_plmn_request_details = rr_get_PLMN_request_details_ptr();

  old_gsm_only = rr_gsm_only();
  old_service_domain = rr_sel_get_service_domain();

  rr_store_service_domain(rr_change_mode_ind->service_domain);
  rr_plmn_request_details->rat_pri_list_info = rr_change_mode_ind->rat_pri_list_info;

  if (old_gsm_only != rr_gsm_only())
  {
    if (rr_gsm_only())
    {
      MSG_GERAN_MED_0_G("Switching GL1 to GSM-only mode");
      rr_send_mph_ue_mode_change_req(TRUE);

      /* Cancel any IRAT reselection timers that are running. */
      rr_g2wr_reset_wcdma_surrounding_database();
    }
    else
    {
      MSG_GERAN_MED_0_G("Switching GL1 to multi-RAT mode");
      /* When switching to Multi_RAT mode, track the request, only done for Multi Mode enabling, 
      to avoid sending back to back Mode change, caused by User SGLTE split */      
      rr_set_pending_mode_change(TRUE);
      rr_send_mph_ue_mode_change_req(FALSE);
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q       
      if ( rr_default_irat_params_update_required())
      {
        /* If all Si2Q blocks received and NO LTE on all the SI2 Blocks */            
        if ( rr_check_and_populate_default_irat_prior_params() )
        {
          MSG_GERAN_HIGH_0_G("PSEUDO SI2Q Setting is a Success");
        } 
      }
#endif

      rr_l1_update_wcdma_neighbor_list();
    }
  }

  rr_ps_handle_rr_change_mode_ind_service_domain_changed(
    old_service_domain,
    rr_sel_get_service_domain()
    );
  }

#endif /* FEATURE_SGLTE */

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
static void grr_handle_pseduo_lte_list_update_ind( rr_pseudo_lte_list_update_ind_T* rr_pseudo_lte_list_upd_ptr  )
{
  boolean l1_update_required = TRUE;
  
  if ( rr_pseudo_lte_list_upd_ptr->can_use_pseudo )
  {  /* This indicates NAS instructing GRR to use Pseudo with Modified list*/
       
    /*Flush OLD List First */
    if ( rr_is_pseudo_si2q_data_used() )
    {
      rr_flush_default_irat_prior_params();
      rr_set_rr_using_pseudo_si2q_data(FALSE);

      /* Now populate and start using the new list */
      rr_check_and_populate_default_irat_prior_params() ;
    }
    else
    {     
      MSG_GERAN_HIGH_0_G("PSEUDO LTE LIST Received when RR not using Pseudo List");
      l1_update_required=FALSE;
    }
  }
  else
  {
    /* This indicates NAS instructing GRR NOT to use Pseudo list*/
    /*Flush OLD List First */
    if ( rr_is_pseudo_si2q_data_used() )
    {
      rr_flush_default_irat_prior_params();
      rr_set_rr_using_pseudo_si2q_data(FALSE);
    }
    else
    {
      MSG_GERAN_HIGH_0_G("PSEUDO LTE LIST Cleaned when RR not using Pseudo List");
      l1_update_required=FALSE;
    }
  }
  /* If Pseudo is stopped been Used or Pseudo List Modified by NAS, inform GL1, if mode is NOT GSM_ONLY */
  if ( !rr_gsm_only() && l1_update_required )
  {
    MSG_GERAN_MED_0_G("GRR Send Updated Pseudo LTE List to GL1");
    rr_l1_update_wcdma_neighbor_list();     
  }
}
#endif

/*!
 * \brief Return a pointer to the module data.
 *
 * \param 
 *
 * \return rr_ps_access_data_t*
 */
static grr_control_data_t *grr_control_get_data_ptr( void )
{
  return &grr_control_data;
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

static rr_cco_status_e rr_grr_map_rr_gprs_ce_x2g_cco_failure_event(
  rr_event_T event
)
{
  switch (event)
  {
    case EV_X2G_CCO_FAILURE_CCO_TIMER_EXPIRY:              return RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY;
    case EV_X2G_CCO_FAILURE_NO_RESPONSE:                   return RR_CCO_STATUS_FAILURE_NO_RESPONSE;
    case EV_X2G_CCO_FAILURE_IA_REJECT:                     return RR_CCO_STATUS_FAILURE_IA_REJECT;
    case EV_X2G_CCO_FAILURE_CONTENTION_RESOLUTION_FAILURE: return RR_CCO_STATUS_FAILURE_CONTENTION_RESOLUTION_FAILURE;
    case EV_X2G_CCO_FAILURE_DSF:                           return RR_CCO_STATUS_FAILURE_DSF;
    default:
      MSG_GERAN_ERROR_1("Unexpected event: %d", event);
      return RR_CCO_STATUS_UNDEFINED;
  }

  return RR_CCO_STATUS_UNDEFINED;
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

/*===========================================================================

FUNCTION rr_gprs_grr_is_active

DESCRIPTION
  Returns TRUE if GRR is active.
  This is determined according to the GRR51 and GRR52 states: If either is not
  in it's NULL state, GRR is considered to be ACTIVE. If both are in their
  NULL state, GRR is considered INACTIVE.

PARAMS
  None

RETURN VALUE
  boolean - TRUE if GRR active; FALSE otherwise

===========================================================================*/
boolean rr_gprs_grr_is_active( )
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();

  if (grr_control_data_ptr != NULL)
  {
    if (grr_control_data_ptr->state != GRR_NULL)
    {
      return TRUE;
    }
  }

  return FALSE;

} /* end rr_gprs_grr_is_active() */


boolean grr_is_idle_camped( )
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();

  if (grr_control_data_ptr != NULL)
  {
    if (grr_control_data_ptr->state == GRR_CAMPED)
    {
      return TRUE;
    }
  }

  return FALSE;

} /* end grr_is_idle_camped() */

boolean grr_in_camped_state( )
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  
  return ((grr_control_data_ptr->state == GRR_CAMPED) ||
          (grr_control_data_ptr->state == GRR_CAMPED_ENABLING_PS_ACCESS) ||
          (grr_control_data_ptr->state == GRR_CAMPED_DISABLING_PS_ACCESS) ||
          (grr_control_data_ptr->state == GRR_CAMPED_MSC_CHANGE));
} /* end grr_in_camped_state() */

boolean grr_is_sglte_x2g_resel_in_progress(  )
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  
  if (grr_control_data_ptr->state == GRR_SGLTE_X2G_RESEL)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================

FUNCTION grr_tbf_est_in_progress

DESCRIPTION
  Returns TRUE if a TBF establishment is in progress

PARAMS
  None

RETURN VALUE
  TRUE if a PS connection establishment is in progress; FALSE otherwise

===========================================================================*/

boolean grr_tbf_est_in_progress(
   
)
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();

  if (grr_control_data_ptr != NULL)
  {
    if (grr_control_data_ptr->state == GRR_CONNECTION_PENDING)
    {
      return TRUE;
    }
  }

  return FALSE;

} /* end grr_tbf_est_in_progress() */



#ifdef FEATURE_GERAN_CS_SUPPORT
/*===========================================================================

FUNCTION grr_cs_conn_est_in_progress

DESCRIPTION
  Returns TRUE if a CS connection establishment is in progress

PARAMS
  None

RETURN VALUE
  TRUE if a CS connection establishment is in progress; FALSE otherwise

===========================================================================*/

boolean grr_cs_conn_est_in_progress(
   
)
{
  return (rr_is_control_state(RR_CONNECTION_PENDING));

} /* end grr_cs_conn_est_in_progress() */
#endif /* FEATURE_GERAN_CS_SUPPORT */


/*===========================================================================

FUNCTION grr_reselection_in_progress

DESCRIPTION
  Returns TRUE if a reselection is in progress

PARAMS
  None

RETURN VALUE
  TRUE if a reselection is in progress; FALSE otherwise

===========================================================================*/

boolean grr_reselection_in_progress(
   
)
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();

  if (grr_control_data_ptr != NULL)
  {
    if ((grr_control_data_ptr->state == GRR_CELL_RESELECTION) &&
        !rr_is_camped())
    {
      return TRUE;
    }
  }

  return FALSE;

} /* end grr_reselection_in_progress() */

/*===========================================================================

FUNCTION grr_examine_saved_mm_message

DESCRIPTION
  This returns the message ID of the saved MM message

PARAMS
  * message_id - updated with the message ID of the saved MM message (optional)

RETURN VALUE
  TRUE is the saved message was a MM message; FALSE otherwise

===========================================================================*/
boolean grr_examine_saved_mm_message(
  byte *message_id
   
)
{
  rr_cmd_bdy_type * message_ptr;

  boolean msg_valid = FALSE;

  if (rr_examine_saved_message(&message_ptr) == EV_INPUT_MESSAGE)
  {
    IMH_T * message_header_ptr = (IMH_T *) message_ptr;

    if ( message_header_ptr->message_set == MS_MM_RR )
    {
      msg_valid = TRUE;
    }
  }

  if ( msg_valid && message_id )
  {
    (* message_id) = ((IMH_T *)message_ptr)->message_id;
  }

  return msg_valid;

} /* end grr_examine_saved_mm_message() */


/*===========================================================================

FUNCTION grr_examine_saved_mac_message

DESCRIPTION
  This returns the message ID of the saved MM message

PARAMS
  * message_id - updated with the message ID of the saved MM message (optional)

RETURN VALUE
  TRUE is the saved message was a MM message; FALSE otherwise

===========================================================================*/
static boolean grr_examine_saved_mac_message(
  byte *message_id
   
)
{
  rr_cmd_bdy_type * message_ptr;

  boolean msg_valid = FALSE;

  if (rr_examine_saved_message(&message_ptr) == EV_INPUT_MESSAGE)
  {
    IMH_T * message_header_ptr = (IMH_T *) message_ptr;

    if (message_header_ptr->message_set == MS_MAC_RR)
    {
      msg_valid = TRUE;
    }
  }

  if ( msg_valid && message_id )
  {
    (* message_id) = ((IMH_T *)message_ptr)->message_id;
  }

  return msg_valid;

} /* end grr_examine_saved_mm_message() */

#ifdef FEATURE_SGLTE

static boolean grr_examine_saved_msgr_message(uint32 * umid  )
{
  rr_cmd_bdy_type * message_ptr;

  boolean msg_valid = FALSE;

  if ( rr_examine_saved_message( &message_ptr) == EV_INPUT_MESSAGE )
  {
    IMH_T * message_header_ptr = (IMH_T *) message_ptr;

    if (message_header_ptr->message_set == MS_MSGR_RR)
    {
      msg_valid = TRUE;
    }
  }

  if ( msg_valid && umid )
  {
    *umid = message_ptr->rr_msgr_msg.message.hdr.id;
  }

  return msg_valid;
}

static boolean grr_examine_saved_tdsrrc_message(uint8 * message_id )
{
  rr_cmd_bdy_type * message_ptr;

  boolean msg_valid = FALSE;

  if ( rr_examine_saved_message( &message_ptr ) == EV_INPUT_MESSAGE )
  {
    IMH_T * message_header_ptr = (IMH_T *) message_ptr;

    if (message_header_ptr->message_set == MS_TDSRRC_RR)
    {
      msg_valid = TRUE;
    }
  }

  if ( msg_valid && message_id )
  {
    *message_id = ((IMH_T *)message_ptr)->message_id;
  }

  return msg_valid;
}


#endif /* FEATURE_SGLTE */

/**
  @brief Send a RR_SERVICE_IND to MM. This in addition resets the local 'notify_mm_service_ind' flag
*/
static void send_rr_service_ind(
   
)
{
  gprs_scell_info_t *scell_info_ptr;
  grr_control_data_t *grr_control_data_ptr;

  // Obtain a pointer to the module data
  grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  scell_info_ptr = rr_gprs_get_scell_info();
  //RR_NULL_CHECK_FATAL(scell_info_ptr);

  rr_send_service_ind(
    MM_AS_SERVICE_AVAILABLE,
    &(scell_info_ptr->gsm.location_area_identification),
    &(scell_info_ptr->gsm)
  );

  grr_control_data_ptr->notify_mm_service_ind = FALSE;
}


/**
  @brief Send a RR_PLMN_SELECT_CNF to MM. This in addition resets the local 'notify_mm_plmn_select_cnf' flag
*/
static void send_rr_plmn_select_cnf(
   
)
{
  grr_control_data_t *grr_control_data_ptr;
  gprs_scell_info_t *scell_info_ptr;

  // Obtain a pointer to the module data
  grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  scell_info_ptr = rr_gprs_get_scell_info();
  //RR_NULL_CHECK_FATAL(scell_info_ptr);

  rr_send_plmn_select_cnf(
    MM_AS_SUCCESS,
    MM_AS_SERVICE_AVAILABLE,
    &(scell_info_ptr->gsm.location_area_identification),
    NULL,
    scell_info_ptr->gsm.BCCH_allocation,
    &(scell_info_ptr->gsm)
  );

  grr_control_data_ptr->notify_mm_plmn_select_cnf = FALSE;
}


/**
  @brief Send a GRR_MAC_PS_ACCESS_REQ to MAC. This in addition resets the local 'purge_gmm_signalling' flag
*/
static void send_ps_imsg_ps_access_req(
  boolean purge_gmm_signalling
   
)
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  if (purge_gmm_signalling)
  {
    rr_ps_send_imsg_ps_access_req(
      TRUE     // purge_gmm_signalling
    );
  }
  else
  {
    rr_ps_send_imsg_ps_access_req(
      grr_control_data_ptr->purge_gmm_signalling   // purge_gmm_signalling
    );
  }

  grr_control_data_ptr->purge_gmm_signalling = FALSE;
}


/**
  @brief Send a GRR_MAC_NO_PS_ACCESS_IND to MAC. GPRS in GMM is suspended if indicated

  @param suspend_gmm Set to TRUE if GPRS in GMM should be suspended
*/
static void send_ps_imsg_no_ps_access_req(
  boolean suspend_gmm
   
)
{
  if (rr_ps_domain_active())
  {
    rr_ps_send_imsg_no_ps_access_req(
      suspend_gmm,    // suspend_gmm
      NO_DEL         // del_pdu
    );
  }
  else
  {
    rr_ps_send_imsg_no_ps_access_req(
      suspend_gmm,    // suspend_gmm
      DEL_ALL        // del_pdu
    );
  }
}


/*===========================================================================

FUNCTION transition_to_camped_state

DESCRIPTION
  This function starts the following GRR activities:
    Sys info handling (30s refresh, etc)
    Reselection (appropriate to NC mode)
    Resumes GPRS in GMM

PARAMS
  None

RETURN VALUE
  None

===========================================================================*/

static grr_state_t transition_to_camped_state(
  boolean service_ind_sent,
  rr_event_T *grr_event,
  rr_cmd_bdy_type **new_message,
  byte *message_set,
  byte *message_id
   
)
{
  boolean gmm_resumed = FALSE;
  boolean pending_rr_est_req = FALSE;
  boolean start_camped_procedures = TRUE;

#ifdef FEATURE_GERAN_CS_SUPPORT
  // Check that T3122 didn't expire "silently" (i.e. when it was not appropriate to inform
  // NAS via a RR_SERVICE_IND due to some other ongoing procedure) - if so, it may be necessary
  // to update NAS now
  rr_check_nas_t3122_status();
#endif /* FEATURE_GERAN_CS_SUPPORT */

  // Check if there are any saved pages and send them to NAS if so
  rr_ce_send_saved_pages_to_nas();

  // Check if there are any saved messages to process
  (* grr_event) = process_saved_message(
    new_message,
    message_set,
    message_id
  );

  if (((*grr_event) == EV_INPUT_MESSAGE) && ((*new_message) != NULL))
  {
    // If there is a any kind of saved message, then assume that this needs to be processed next
    // so don't perform any other actions
    start_camped_procedures = FALSE;

    // Check if the saved message is from MM
    if ((*message_set) == MS_MM_RR)
    {
#ifdef FEATURE_GERAN_CS_SUPPORT
      // RR_EST_REQ is checked to see if it will result in a connection establishment (which would
      // normally be the case)
      if ((*message_id) == (byte)RR_EST_REQ)
      {
        rr_event_T pending_rr_est_req_action = preprocess_rr_est_req(
          &((*new_message)->rr_est_req)
        );

        // If preprocess_rr_est_req() returns EV_RR_EST, then a connection establishment will result
        // from processing this message in GRR_CAMPED.
        // If EV_NO_EVENT is returned, then this request will not result in any action. In this case,
        // continue with normal idle procedures
        if (pending_rr_est_req_action == EV_RR_EST_REQ)
        {
          pending_rr_est_req = TRUE;
        }
        else
        if (pending_rr_est_req_action == EV_NO_EVENT)
        {
          start_camped_procedures = TRUE;
        }
      }
#endif /* FEATURE_GERAN_CS_SUPPORT */
    }
    else
    {
      // If a saved message is not from MM (e.g. MAC_GRR_UL_TBF_REQ), then this will be processed without
      // transitioning the RR state from RR_GRR. This means the saved message will not be removed, so this
      // is done here
      grr_throwaway_saved_message();
    }
  }

  // If GPRS in GMM was suspended, then resume it now, regardless of whether there is a saved message to
  // process and/or if PS access is allowed. This is so that GMM is not left suspended unintentionally.
  // (The exception to this is if about to start connection establishment).
  // This returns TRUE if a request to resume GPRS in GMM was sent
  if (!pending_rr_est_req)
  {
    gmm_resumed = rr_gmm_resume_gprs();
  }

  if (start_camped_procedures)
  {
    boolean ps_access_allowed;

    // Start 30s sys info refresh cycle, and acquire any remaining SIs
    rr_gprs_activate_camped_sys_info_handling();

    // Check if PS access for UL allowed (i.e. T3142 not running)
    ps_access_allowed = rr_mac_ul_tbf_est_is_allowed();

    if (ps_access_allowed)
    {
      if (gmm_resumed)
      {
        // If GPRS in GMM was resumed then GMM is expected to request GPRS service, if required
        MSG_GERAN_HIGH_0_G("GMM expected to request GPRS service");
      }
      else
      {
        // GMM did not need to be resumed, which means it will not send a request for GPRS service.
        // However, if a RR_SERVICE_IND was sent, then this will also prompt GMM to request GPRS service
        if (!service_ind_sent)
        {
          // There were no messages sent to GMM, so it will not request GPRS service.
          // As PS access is allowed, enable it now
          send_ps_imsg_ps_access_req(
            FALSE   // purge_gmm_signalling
          );

          // Wait for the confirmation in GRR_CAMPED_ENABLING_PS_ACCESS
          return GRR_CAMPED_ENABLING_PS_ACCESS;
        }
      }
    }
    else
    {
      // Send request to disable PS access
      send_ps_imsg_no_ps_access_req(
        FALSE   // suspend_gmm
      );

      // Wait for the confirmation in GRR_CAMPED_DISABLING_PS_ACCESS
      return GRR_CAMPED_DISABLING_PS_ACCESS;
    }
  }

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION   
  /*Inform reselection module  for any reduced mobility */
  if (rr_ue_in_peo_mode())
  {
    rr_resel_send_imsg_reduced_mobility_required_ind(RR_REDUCED_MOBILITY_MAX);
  }
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

  return GRR_CAMPED;

} /* end transition_to_camped_state() */



/*===========================================================================

FUNCTION transition_to_reselection_state

DESCRIPTION

PARAMS
  rr_event

RETURN VALUE
  None

===========================================================================*/

static void transition_to_reselection_state(
  rr_event_T rr_event
   
)
{
  switch ( rr_event )
  {
    case EV_FORCE_CELL_RESELECTION:
      rr_resel_send_imsg_required_ind(RR_RESEL_CAUSE_RESELECTION);

      break;

    case EV_DSF_RESELECTION:
      rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_DSF);

      (void) rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, NULL);
      rr_resel_send_imsg_required_ind(RR_RESEL_CAUSE_RESELECTION);
      break;
      
    case EV_DSF_FOR_DENIALS:

      rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_DSF);

      (void) rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, NULL);
      rr_resel_send_imsg_required_ind(RR_RESEL_CAUSE_RESELECTION_TRM_DENIAL);
      break;

    default:
      rr_resel_send_imsg_required_ind(RR_RESEL_CAUSE_RESELECTION);
      break;
  }


  /* notify measurement reporting of the cell reselection */
  (void) rr_gprs_nc_measurement_control(RR_EV_DEACTIVATE_MEASUREMENTS, NULL);

} /* end transition_to_reselection_state() */

const char *get_grr_control_state_name(const grr_state_t state)
{
  const char * s;

  switch ( state )
  {
    case GRR_NULL:
      s = "NULL";
      break;

    case GRR_ACQUIRE:
      s = "ACQUIRE";
      break;

    case GRR_CAMPED:
      s = "CAMPED";
      break;

    case GRR_CONNECTION_PENDING:
      s = "CONNECTION_PENDING";
      break;

    case GRR_CELL_RESELECTION:
      s = "CELL_RESELECTION";
      break;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case GRR_CELL_RESELECTION_G2W:
      s = "CELL_RESELECTION_G2W";
      break;
#endif
#ifdef FEATURE_GPRS_PS_HANDOVER
    case GRR_PSHO:
      s = "PSHO";
      break;

#endif /* FEATURE_GPRS_PS_HANDOVER */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case GRR_X2G_CCO_FAILED:
      s = "X2G_CCO_FAILED";
      break;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

    case GRR_STARTING_L1_IDLE:
      s = "STARTING_L1_IDLE";
      break;

    case GRR_CAMPED_ENABLING_PS_ACCESS:
      s = "CAMPED_ENABLING_PS_ACCESS";
      break;

    case GRR_CAMPED_DISABLING_PS_ACCESS:
      s = "CAMPED_DISABLING_PS_ACCESS";
      break;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
    case GRR_CAMPED_MSC_CHANGE:
      s = "CAMPED_MSC_CHANGE";
      break;
#endif

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case GRR_X2G_CCO_ABORTED:
      s = "X2G_CCO_ABORTED";
      break;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_SGLTE
    case GRR_SGLTE_SWITCHING_TO_GSM_ONLY:
      s = "SGLTE_SWITCHING_TO_GSM_ONLY";
      break;

    case GRR_SGLTE_X2G_RESEL:
      s = "SGLTE_X2G_RESEL";
      break;
#endif /* FEATURE_SGLTE */

    default:
      s = "?";
  }

  return s;

} /* end get_grr_control_state_name() */

boolean grr_main_state_is_acquire(
   
)
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();

  if (grr_control_data_ptr != NULL)
  {
    if (grr_control_data_ptr->state == GRR_ACQUIRE)
    {
      return TRUE;
    }
  }

  return FALSE;
}

/****************************************************************************
 *   Function name:  grr_auth_failed_calculate_timeout
 *   --------------------------
 *
 *   Description: Determines the number of SECONDS to bar a cell after
 *                receiving RR_GMM_GPRS_AUTH_FAIL_IND from NAS
 *   ------------
 *
 *
 *   Parameters:  pointer to incoming NAS message
 *   -----------
 *
 *
 *   Returns:     uint32 number of SECONDS
 *   --------
 *
 *
 ****************************************************************************/
static uint32 grr_auth_failed_calculate_timeout(
  rr_cmd_bdy_type *msg_ptr
)
{
  rr_gmm_gprs_auth_fail_ind_T *rr_gmm_gprs_auth_fail_ind;
  uint32                       barred_time_secs;

  if (msg_ptr == NULL)
  {
    return(0);
  }

  rr_gmm_gprs_auth_fail_ind = (rr_gmm_gprs_auth_fail_ind_T *)msg_ptr;

  /* NAS sends the T3302 timer value as a number of MILLISECONDS, see mm_rr.h */
  if (rr_gmm_gprs_auth_fail_ind->t3302_timer_value == 0)
  {
    /* default is 12 hours if T3302 not supplied */
    barred_time_secs = CONVERT_HOURS_TO_SECONDS(12);
  }
  else if (rr_gmm_gprs_auth_fail_ind->t3302_timer_value > CONVERT_SECONDS_TO_MILLISECONDS(60))
  {
    dword t3302_secs;

    /* convert the NAS timer value from milliseconds to seconds */
    t3302_secs = (rr_gmm_gprs_auth_fail_ind->t3302_timer_value / 1000);

    /* then subtract 60 seconds from the T3302 value */
    barred_time_secs = (uint32)(t3302_secs - 60);
  }
  else
  {
    /* T3302 is less than 60 seconds, so treat the timeout as zero */
    barred_time_secs = 0;
  }

  MSG_GERAN_HIGH_3("Cell bar timeout calculated as %d secs (T3302=%dms)",
            barred_time_secs,
            rr_gmm_gprs_auth_fail_ind->t3302_timer_value,
            0);

  return(barred_time_secs);
}

#ifdef FEATURE_SGLTE

#ifdef FEATURE_LTE
/**
  @brief Called when GERAN_GRR_LTE_PLMN_SRCH_REQ is received by GRR.
*/
static rr_event_T handle_geran_grr_lte_plmn_srch_req(
  lte_irat_plmn_srch_req_s * plmn_srch_req
   
)
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  rr_event_T          output_event = EV_NO_EVENT;
  boolean             reject = FALSE;

#ifndef FEATURE_BPLMN_SEARCH_PRIORITY
  /* A general rule - block automatic search if GPRS is active */
  if (((rr_macinfo_get_mac_status() != MAC_IDLE) ||
       grr_has_there_been_recent_gprs_activity()) &&
      plmn_srch_req->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
  {
    MSG_GERAN_HIGH_0_G("Can't suspend GPRS for automatic PLMN search");
    rr_sglte_reject_lte_plmn_srch_req();
    grr_throwaway_saved_message();
    return EV_NO_EVENT;
  }
#endif // FEATURE_BPLMN_SEARCH_PRIORITY

  /* state dependent rules */
  switch (grr_control_data_ptr->state)
  {
    case GRR_ACQUIRE:
    {
      if (plmn_srch_req->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        reject = TRUE;
      }
      else
      {
        (void)rr_gprs_bsi_acquisition_control(EV_RESET_SOFTWARE, NULL);
        output_event = EV_PROCESS_SAVED_MESSAGE;
      }
      break;
    } /* GRR_ACQUIRE */

    case GRR_CONNECTION_PENDING:
    {
      if (plmn_srch_req->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        reject = TRUE;
      }
      else
      {
        output_event = EV_PROCESS_SAVED_MESSAGE;
      }
      break;
    } /* GRR_CONNECTION_PENDING */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case GRR_CELL_RESELECTION_G2W:
    {
      MSG_GERAN_ERROR_0_G("GERAN_GRR_LTE_PLMN_SRCH_REQ unexpected during G2W reselection, rejecting.");
      reject = TRUE;
      break;
    } /* GRR_CELL_RESELECTION_G2W */
#endif

    case GRR_CELL_RESELECTION:
    {
      if (plmn_srch_req->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        reject = TRUE;
      }
      else
      {
        reject = FALSE;
      }

      if (reject == FALSE)
      {
        rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
      }
      break;
    } /* GRR_CELL_RESELECTION */

    case GRR_CAMPED:
    {
      if (rr_pcco_in_progress() || rr_failed_pcco_in_progress())
      {
        if (plmn_srch_req->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
        {
          reject = TRUE;
        }
        else
        {
          reject = FALSE;
          rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
        }
      }
      else
      {
        rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
      }
      break;
    } /* GRR_CAMPED */

    default:
    {
      break;
    }
  }

  if (reject)
  {
    MSG_GERAN_HIGH_2_G("Can't proceed GERAN_GRR_LTE_PLMN_SRCH_REQ (mode %d) in state %d",
             plmn_srch_req->network_select_mode, grr_control_data_ptr->state);
    rr_sglte_reject_lte_plmn_srch_req();
    grr_throwaway_saved_message();
  }
  else
  {
    rr_save_msg_for_recovery_after_panic_reset((rr_cmd_bdy_type *) plmn_srch_req);
  }

  return output_event;

} /* end handle_geran_grr_lte_plmn_srch_req() */
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
/**
  @brief Called when GERAN_GRR_LTE_PLMN_SRCH_REQ is received by GRR.
*/
static rr_event_T handle_tdsrr_interrat_plmn_srch_req(
  tdsrr_interrat_plmn_srch_req_type *plmn_srch_req
   
)
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  rr_event_T          output_event = EV_NO_EVENT;
  boolean             reject = FALSE;

#ifndef FEATURE_BPLMN_SEARCH_PRIORITY
  /* A general rule - block automatic search if GPRS is active */
  if (((rr_macinfo_get_mac_status() != MAC_IDLE) ||
       grr_has_there_been_recent_gprs_activity()) &&
      plmn_srch_req->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
  {
    MSG_GERAN_HIGH_0_G("Can't suspend GPRS for automatic PLMN search");
    rr_sglte_reject_tds_plmn_srch_req();
    grr_throwaway_saved_message();
    return EV_NO_EVENT;
  }
#endif // FEATURE_BPLMN_SEARCH_PRIORITY

  /* state dependent rules */
  switch (grr_control_data_ptr->state)
  {
    case GRR_ACQUIRE:
    {
      if (plmn_srch_req->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        reject = TRUE;
      }
      else
      {
        (void)rr_gprs_bsi_acquisition_control(EV_RESET_SOFTWARE, NULL);
        output_event = EV_PROCESS_SAVED_MESSAGE;
      }
      break;
    } /* GRR_ACQUIRE */

    case GRR_CONNECTION_PENDING:
    {
      if (plmn_srch_req->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        reject = TRUE;
      }
      else
      {
        output_event = EV_PROCESS_SAVED_MESSAGE;
      }
      break;
    } /* GRR_CONNECTION_PENDING */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case GRR_CELL_RESELECTION_G2W:
    {
      MSG_GERAN_ERROR_0_G("GERAN_GRR_LTE_PLMN_SRCH_REQ unexpected during G2W reselection, rejecting.");
      reject = TRUE;
      break;
    } /* GRR_CELL_RESELECTION_G2W */
#endif

    case GRR_CELL_RESELECTION:
    {
      if (plmn_srch_req->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        reject = TRUE;
      }
      else
      {
        reject = FALSE;
      }

      if (reject == FALSE)
      {
        rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
      }
      break;
    } /* GRR_CELL_RESELECTION */

    case GRR_CAMPED:
    {
      if (rr_pcco_in_progress() || rr_failed_pcco_in_progress())
      {
        if (plmn_srch_req->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
        {
          reject = TRUE;
        }
        else
        {
          reject = FALSE;
          rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
        }
      }
      else
      {
        rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
      }
      break;
    } /* GRR_CAMPED */

    default:
    {
      break;
    }
  }

  if (reject)
  {
    MSG_GERAN_HIGH_2_G("Can't proceed TDSRR_INTERRAT_PLMN_SRCH_REQ (mode %d) in state %d",
             plmn_srch_req->network_select_mode, grr_control_data_ptr->state);
    rr_sglte_reject_tds_plmn_srch_req();
    grr_throwaway_saved_message();
  }
  else
  {
    rr_save_msg_for_recovery_after_panic_reset((rr_cmd_bdy_type *) plmn_srch_req);
  }

  return output_event;
}
#endif /*FEATURE_GSM_TDS*/

#endif /* FEATURE_SGLTE */

/**
  @brief Checks whether the specified GRR state is expected to be transitory
         or not.

  GRR has a number of states which are only expected to be entered for a brief
  duration, e.g. when starting idle mode and waiting for a response from L1.
  This function can be used to check whether a GRR state is one of these
  transitory states.

  @param state The state to check.
  @return TRUE if the state is transitory, FALSE if not.
*/
static boolean grr_control_state_is_transitory(
  grr_state_t state
)
{
  switch (state)
  {
    case GRR_STARTING_L1_IDLE:
    case GRR_CAMPED_ENABLING_PS_ACCESS:
    case GRR_CAMPED_DISABLING_PS_ACCESS:
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
    case GRR_CAMPED_MSC_CHANGE:
#endif

#ifdef FEATURE_SGLTE
    case GRR_SGLTE_SWITCHING_TO_GSM_ONLY:
    case GRR_SGLTE_X2G_RESEL:
#endif /* FEATURE_SGLTE */
    {
      return TRUE;
    }

    default:
    {
      return FALSE;
    }
  }
}

/****************************************************************************
 *   Function name:  grr_control
 *   --------------------------
 *
 *   Description: This is the control of the GRR51 State Machine
 *   ------------
 *
 *
 *   Parameters: An event and a possible message
 *   -----------
 *
 *
 *   Returns:  an event to rr_control rr_event_T
 *   --------
 *
 *
 ****************************************************************************/
rr_event_T grr_control(
  rr_event_T grr_control_event,
  rr_cmd_bdy_type *new_message
   
)
{
  IMH_T      *message_header        = NULL;
  rr_event_T  output_event          = EV_NO_EVENT;
  rr_event_T  new_grr_control_event = EV_NO_EVENT;
  byte        message_id            = 0;
  byte        message_set           = 0;
  grr_control_data_t *grr_control_data_ptr;

  // Obtain a pointer to the module data
  grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  /*Get the input message header information*/
  if (grr_control_event == EV_INPUT_MESSAGE && new_message != NULL)
  {
    message_header = &new_message->message_header;
    message_set = message_header->message_set;
    message_id = message_header->message_id;

#ifdef FEATURE_GERAN_CS_SUPPORT
    /* Generic handler to bounce an abort confirm back to MM */
    if( (message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ) &&
         (grr_control_data_ptr->state != GRR_NULL) 
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
		 && (grr_control_data_ptr->state != GRR_CELL_RESELECTION_G2W) 
#endif
	  )
    {
      handle_rr_abort_req();

      /* No need to handle it further */
      grr_control_event = EV_NO_EVENT;
    }
    else
#endif /* FEATURE_GERAN_CS_SUPPORT */
    if (message_set == MS_RR_L2 && message_id == (byte)DL_UNIT_DATA_IND)
    {
      dl_unit_data_ind_T *dl_unit_data_ind_ptr;
      l2_channel_type_T   l2_channel_type;
      byte                message_length;

      dl_unit_data_ind_ptr = &new_message->dl_unit_data_ind;

      l2_channel_type = dl_unit_data_ind_ptr->l2_channel_type;
      message_length  = dl_unit_data_ind_ptr->l3_message_length;

      if (((l2_channel_type == CCCH) && (message_length > N201_MAX)))
      {
        /* cancel event to ignore message */
        grr_control_event = EV_NO_EVENT;
        MSG_GERAN_LOW_0_G("ERROR: DL_UNIT_DATA_IND contains L3 message too long");
      }
    }
    else if ( message_set == MS_RR_L1 && message_id == (byte)MPH_SURROUND_UPDATE_FAILURE_IND )
    {
      /****************************************************************/
      /* MPH_SURROUND_UPDATE_FAILURE_IND                              */
      /* This is a response to a surround_update_req. It means either */
      /* L1 cannot read the requested ncells bcch or that L1 has      */
      /* made a state transition while GRR is awaiting a response.    */
      /****************************************************************/
      mph_surround_update_failure_ind_T *su_fail_ptr = &new_message->mph_surround_update_failure_ind;
      rr_handle_surround_update_failure_ind(su_fail_ptr, TRUE, TRUE);
    }
    else if (message_set == MS_RR_L1 &&
             message_id == (byte)MPH_BLOCK_QUALITY_IND)
    {
      /* Print out some debug if the MPH_BLOCK_QUALITY_IND signal is indicating
      that the serving cell is bad. */
      if (new_message->mph_block_quality_ind.block_quality == GL1_BLOCK_QUALITY_BAD)
      {
        MSG_SPRINTF_3(
          MSG_SSID_DFLT,
          MSG_LEGACY_HIGH,
          "BQbad GRR:%d CR:%s DSC:%d",
          grr_control_data_ptr->state,
          rr_resel_current_state_name(),
          rr_gprs_return_gprs_dsc_counter_value());
      }
    }

#ifdef FEATURE_GERAN_CS_SUPPORT
    else
    /*****************************************************************/
    /* MAC->GRR: MAC_GRR_PAGING_NOTIFICATION                         */
    /* A CS or PS page has been received either on the Packet Paging */
    /* channel if the MS is in Packet Idle (52 only), or on the      */
    /* PACCH whilst in Packet Transfer                               */
    /*****************************************************************/
    if ( ( message_set == MS_MAC_RR ) &&
         ( message_id  == (byte)MAC_GRR_PAGING_NOTIFICATION ) )
    {
      handle_mac_grr_paging_notification(
        (mac_grr_paging_notification_t *) new_message    // paging_notification
      );
      grr_control_event = EV_NO_EVENT;
    }
#endif /* FEATURE_GERAN_CS_SUPPORT */

#ifdef FEATURE_CGPS_UMTS_CP_GSM
    else
    /***********************************************************/
    /* GPS->RR: RR_CGPS_EVENT_REQ                              */
    /***********************************************************/
    if ( ( message_set == MS_CGPS_RR ) &&
         ( (cgps_rr_message_id_enum_T) message_id == RR_CGPS_EVENT_REQ ) )
    {
      rr_cgps_process_cgps_event_req(new_message);
      grr_control_event = EV_NO_EVENT;
    }
#endif /* FEATURE_CGPS_UMTS_CP_GSM */
  }
  else if (grr_control_event == EV_RESET_SOFTWARE)
  {
    rr_grr_control_init();
  }

  while (grr_control_event != EV_NO_EVENT)
  {
    switch (grr_control_data_ptr->state)
    {
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* GRR_NULL - This is the initial deactivated state of GRR51    */
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case GRR_NULL:
      {
        /***************************************************************/
        /* INPUT MESSAGE handlers                                      */
        /***************************************************************/
        if (grr_control_event == EV_INPUT_MESSAGE)
        {
          /* grr_control() is could receive MM messages in GRR_NULL state,
           * due to an abortion of PLMN search procedure.
           * During PLMN search, GRR could be in NULL and
           * a MM request aborts the PLMN search will be processed here
           */
          if (message_set == MS_MM_RR)
          {
            output_event = relinquish_control_to_rr(
              EV_PROCESS_SAVED_MESSAGE   // output_event
            );
          }
          else if (message_set == MS_RR_RR)
          {
            switch (new_message->rr.header.rr_message_set)
            {
              case RR_GRR_IMSG:
              {
                switch (new_message->rr.header.imh.message_id)
                {
                  case RR_GRR_IMSG_CAMPED_ON_CELL_IND:
                  {
                    new_grr_control_event = RR_EV_CAMPED_ON_CELL;
                    break;
                  }

                  case RR_GRR_IMSG_CAMP_ON_CELL_REQ:
                  {
                    rr_grr_imsg_camp_on_cell_req_t *imsg = &new_message->rr.grr.imsg.camp_on_cell_req;

                    // Store the options supplied in the message for use later
                    grr_control_data_ptr->notify_mm_service_ind = imsg->notify_mm_service_ind;
                    grr_control_data_ptr->notify_mm_plmn_select_cnf = imsg->notify_mm_plmn_select_cnf;
                    grr_control_data_ptr->purge_gmm_signalling = imsg->purge_gmm_signalling;

                    // Put L1 in IDLE
                    rr_l1_idle_send_imsg_start_idle_req();

                    grr_control_data_ptr->state = GRR_STARTING_L1_IDLE;
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message, "GRR_NULL");
                  }
                }
                break;
              } /* RR_GRR_IMSG */

              case RR_RESEL_IMSG:
              case RR_CELL_ACQ_IMSG:
              case RR_CELL_ACQ_OMSG:
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
              case RR_RESEL_RMC_IMSG:
              case RR_RESEL_RMC_OMSG:
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
              case RR_RESEL_G2W_IMSG:
              case RR_RESEL_G2W_OMSG:
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
              case RR_PSCAN_IMSG:
              case RR_PSCAN_OMSG:
              case RR_DECODE_BCCH_IMSG:
              case RR_DECODE_BCCH_OMSG:
              case RR_PS_IMSG:
              case RR_PS_OMSG:
              {
                new_grr_control_event = rr_resel_control(
                  EV_INPUT_MESSAGE,   // event
                  new_message        // message
                );
                break;
              } /* RR_RESEL_IMSG ... */

              case RR_RESEL_OMSG:
              {
                switch (new_message->rr.resel.omsg.header.imh.message_id)
                {
                  case RR_RESEL_OMSG_STARTED_IND:
                  {
                    rr_send_rr_reselection_ind();

                    switch (new_message->rr.resel.omsg.started_ind.type)
                    {
                      case RR_RESEL_TYPE_NORMAL:
                      {
                        grr_control_data_ptr->state = GRR_CELL_RESELECTION;
                        break;
                      } /* RR_RESEL_TYPE_NORMAL */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                      case RR_RESEL_TYPE_G2W:
                      {
                        grr_control_data_ptr->state = GRR_CELL_RESELECTION_G2W;
                        break;
                      }
#endif
                      default:
                      {
                        MSG_GERAN_LOW_3_G("Unexpected reselection type started: %d",
                          new_message->rr.resel.omsg.started_ind.type, 0, 0
                        );
                      }
                    }
                    break;
                  } /* RR_RESEL_OMSG_STARTED_IND */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message, "GRR_NULL");
                  }
                }
                break;
              } /* RR_RESEL_OMSG */

              case RR_SELECT_BCCH_IMSG:
              {
                break;    // ignore - no processing required
              } /* RR_SELECT_BCCH_IMSG */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  new_message,
                  get_grr_control_state_name(grr_control_data_ptr->state)
                );
              }
            }
          }
          else
          {
            LOG_UNEXPECTED_MESSAGE(
              new_message,
              get_grr_control_state_name(grr_control_data_ptr->state)
            );
          }
        }

        else
        /***********************************************************************/
        /* RR_EV_CAMPED_ON_CELL                                                */
        /* Either Cell Selection has yielded a GPRS cell, or a CS call has     */
        /* been released, and the current cell supports GPRS.                  */
        /* GPRS CCCH Idle has already been started in L1, and MM informed.     */
        /***********************************************************************/
        if ( ( grr_control_event == RR_EV_CAMPED_ON_CELL ) ||
             ( grr_control_event == EV_GRR51_GO_TO_PACKET_IDLE ) )
        {
#ifndef FEATURE_IDLE_DRX_SCALING
          /*Initialise the DSC*/
          rr_gprs_initialise_dsc();
#endif /* !FEATURE_IDLE_DRX_SCALING */
          rr_gprs_init_bad_snr_counter();

          /* Determine if reselection handling should be activated based on NC mode */
          grr_activate_reselection_if_allowed();

          /* Move to CAMPED state, start GRR camped procedures */
          grr_control_data_ptr->state = transition_to_camped_state(
            TRUE,                             // service_ind_sent
            &new_grr_control_event,
            (rr_cmd_bdy_type **)&new_message,
            &message_set,
            &message_id
          );
        }

        else
        /***********************************************************************/
        /* EV_FORCE_CELL_RESELECTION                                           */
        /* The cell needs to be reselected, probably due to a failure in a CS  */
        /* connection establishment procedure (like T3126 expiry)              */
        /***********************************************************************/
        if ( ( grr_control_event == EV_FORCE_CELL_RESELECTION ) ||
             ( grr_control_event == EV_DSF_RESELECTION ) ||
             ( grr_control_event == EV_DSF_FOR_DENIALS ) )
        {
          transition_to_reselection_state(
            grr_control_event    // rr_event
          );
        }
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
        else
        if ( grr_control_event == RR_EV_RESELECT_WCDMA_CELL )
        {
          grr_control_data_ptr->state = GRR_CELL_RESELECTION_G2W;
        }
#endif
        else
        if ( grr_control_event == EV_CELL_RESELECTION )
        {
          grr_control_data_ptr->state = GRR_CELL_RESELECTION;
        }
        else
        if ( grr_control_event == RR_EV_TRIGGER_CELL_SELECTION )
        {
          output_event = relinquish_control_to_rr(grr_control_event);
        }
        else
        /***********************************************************************/
        /* RR_EV_PLMN_LIST_CONSTRUCTION_COMPLETE                               */
        /***********************************************************************/
        if ( grr_control_event == EV_PLMN_SEARCH_COMPLETE )
        {
          /* Determine if reselection handling should be activated based on NC mode */
          grr_activate_reselection_if_allowed();

          /* Move to CAMPED state, start GRR camped procedures */
          grr_control_data_ptr->state = transition_to_camped_state(
            FALSE,                            // service_ind_sent
            &new_grr_control_event,
            (rr_cmd_bdy_type **)&new_message,
            &message_set,
            &message_id
          );
        }
        /*
        An RPS has been action from outside this state machine. This will be due to
        a call release onto a bad scell
        */
        else if ( grr_control_event == RR_EV_START_RESELECTION_POWER_SCAN )
        {
          rr_send_rr_reselection_ind();

          rr_resel_send_imsg_required_ind(
            RR_RESEL_CAUSE_RPS   // cause
          );

          grr_control_data_ptr->state = GRR_CELL_RESELECTION;
        }

        else if ( grr_control_event == EV_GRR_RPS_STARTED)
        {
          grr_control_data_ptr->state = GRR_CELL_RESELECTION;
        }

        else if ( grr_control_event == EV_PCCO_FAILED )
        {
          grr_control_data_ptr->state = GRR_CELL_RESELECTION;
          new_grr_control_event = rr_resel_control(EV_PCCO_FAILED, NULL);
        }

        else if ( grr_control_event == EV_RESET_SOFTWARE )
        {
          /* do nothing */;
        }

        else if (grr_control_event == RR_EV_CANDIDATE_CELL_SI_RECEIVED)
        {
          /* No action necessary. */
        }
#ifdef FEATURE_SGLTE
        else if (grr_control_event == EV_SGLTE_G2X_REDIR_COMPLETE)
        {
          /* Move to CAMPED state, start GRR camped procedures */
          grr_control_data_ptr->state = transition_to_camped_state(
            FALSE,                             // service_ind_sent
            &new_grr_control_event,
            (rr_cmd_bdy_type **)&new_message,
            &message_set,
            &message_id
          );
        }
        else if (grr_control_event == EV_SGLTE_X2G_CCO_FAILED_GO_TO_IDLE)
        {
          /* Need to switch back to CS-only / GSM-only mode then send an
          appropriate failure response back to the originating RAT. */
          rr_grr_send_imsg_switch_to_cs_only_gsm_only_req(
            rr_sglte_x2g_cco_failed_cb_fn 
          );
          grr_control_data_ptr->state = GRR_CAMPED;
        }
#endif /* FEATURE_SGLTE */

        else
        {
          LOG_UNEXPECTED_EVENT(
            rr_event_name(grr_control_event),
            get_grr_control_state_name(grr_control_data_ptr->state)
          );
        }

        break;
      }


      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* GRR_ACQUIRE - In this state GRR51 is acquiring system          */
      /* information. This may be due to an Abnormal release with system*/
      /* information                                                    */
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      case GRR_ACQUIRE:
      {
        /***************************************************************/
        /* INPUT MESSAGE handlers                                      */
        /***************************************************************/
        if ( grr_control_event == EV_INPUT_MESSAGE )
        {
#ifdef FEATURE_GERAN_CS_SUPPORT
          /**********************************************************/
          /* MM->RR: RR_EST_REQ                                     */
          /* The MM layer is requesting to open a CS RR connection  */
          /**********************************************************/
          if ( ( message_set == MS_MM_RR ) &&
               ( message_id == (byte)RR_EST_REQ ) )
          {
            /* RR_EST_REQ is not handled in this state.  It will be processed
            when RR transitions to GRR_CAMPED state after acquisition has
            completed, or if acquisition fails an RR_ABORT_IND will be sent
            to MM. This message has already been saved (via rr_save_msg) */

            /* Although this message had already been saved for processing once */
            /* camped (in RR, RR_GRR state), it needs to be saved in case a */
            /* panic reset is triggered before this sys info acquisition */
            /* completes */
            rr_save_msg_for_recovery_after_panic_reset(
              new_message    // message_ptr
            );
          }

          else
#endif /* FEATURE_GERAN_CS_SUPPORT */
          /***********************************************************/
          /* MM->RR: RR_STOP_GSM_MODE_REQ                            */
          /*         RR_DEACT_REQ                                    */
          /* The current TBF (if there is one) must be released, and */
          /* GPRS operation stopped.                                 */
          /***********************************************************/
          if ( ( message_set == MS_MM_RR ) &&
               ( ( message_id == (byte)RR_STOP_GSM_MODE_REQ ) ||
                 ( message_id == (byte)RR_DEACT_REQ ) ) )
          {
            /* Save this message in case a panic reset occurs during suspension */
            rr_save_msg_for_recovery_after_panic_reset(
              new_message    // message_ptr
            );

            // Send request to block PS access (which may already be blocked)
            // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
            send_ps_imsg_no_ps_access_req(
              FALSE   // suspend_gmm
            );
          }

          else
          /***********************************************************/
          /* MM->RR: RR_PLMN_LIST_REQ                                */
          /* Pass this to the handler function to process            */
          /***********************************************************/
          if ( ( message_set == MS_MM_RR ) &&
               ( message_id == (byte)RR_PLMN_LIST_REQ ) )
          {
            rr_plmn_list_req_T *rr_plmn_list_req = (rr_plmn_list_req_T *) new_message;

            new_grr_control_event = handle_rr_plmn_list_req(
              rr_plmn_list_req   // rr_plmn_list_req
            );
          }

          else
          /***********************************************************/
          /* MM->RR: RR_PLMN_SELECT_REQ                              */
          /* Pass this to the handler function to process            */
          /***********************************************************/
          if ( ( message_set == MS_MM_RR ) &&
               ( message_id == (byte)RR_PLMN_SELECT_REQ ) )
          {
            new_grr_control_event = handle_rr_plmn_select_req(
              (rr_plmn_select_req_T *) new_message   // rr_plmn_select_req
            );
          }

          else if ( ( message_set == MS_TIMER ) &&
               ( ((rr_cmd_bdy_type *) new_message)->mid_timer_expiry.timer_id == T3174 ) )
          {
            new_grr_control_event = EV_T3174_TIMEOUT;
          }

          else if ( ( message_set == MS_TIMER ) &&
               ( ((rr_cmd_bdy_type *) new_message)->mid_timer_expiry.timer_id == T3122 ) )
          {
            // Because of possibly inconsistent system information (e.g. if full SI refresh in progress),
            // delay the indication to NAS-MM that T3122 has expired until returning to Packet Idle
            MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
          }

          else if (message_set == MS_RR_RR)
          {
            switch (new_message->rr.header.rr_message_set)
            {
              case RR_GRR_IMSG:
              {
                switch (new_message->rr.header.imh.message_id)
                {
                  case RR_GRR_IMSG_CAMPED_ON_CELL_IND:
                  {
                    new_grr_control_event = RR_EV_CAMPED_ON_CELL;
                    break;
                  } /* RR_GRR_IMSG_CAMPED_ON_CELL_IND */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
                  case RR_GRR_IMSG_MSC_CHANGE_IND:
                  {
                    /*
                    Update NAS as required
                    RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                    This will be sent when RR/L1 enter Idle
                    */
                    rr_msc_update(
                      FALSE   // can_send_service_ind
                    );
                    break;
                  }
#endif

                  case RR_GRR_IMSG_PS_ACCESS_REQ:
                  {
                    // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                    rr_grr_imsg_ps_access_req_t *imsg = &new_message->rr.grr.imsg.ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn);
                    break;
                  }

                  case RR_GRR_IMSG_NO_PS_ACCESS_REQ:
                  {
                    // No PS access request (could be TLLI unassignment, CS-only)

                    rr_grr_imsg_no_ps_access_req_t *imsg = &new_message->rr.grr.imsg.no_ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn);
                    break;
                  }

                  case RR_GRR_IMSG_PAGE_IND:
                  {
                    rr_grr_imsg_page_ind_t *imsg = &new_message->rr.grr.imsg.page_ind;

                    // A page received during cell selection is saved. It is forwarded to NAS when RR/L1
                    // are in Packet Idle
                    rr_ce_save_page(
                      &imsg->page_info   // page_info_ptr
                    );
                    break;
                  }

                  case RR_GRR_IMSG_PAGE_MODE_IND:
                  {
                    // Changes in OTA page mode are ignored during cell selection
                    // (these may be received after Early Camping)
                    MSG_GERAN_LOW_0_G("Page Mode change ignored");
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message, "GRR_ACQUIRE");
                  }
                }
                break;
              } /* RR_GRR_IMSG */

              case RR_RESEL_IMSG:
              case RR_CELL_ACQ_IMSG:
              case RR_CELL_ACQ_OMSG:
              case RR_PSCAN_IMSG:
              case RR_PSCAN_OMSG:

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
              case RR_RESEL_G2W_IMSG:
              case RR_RESEL_G2W_OMSG:
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
              {
                new_grr_control_event = rr_resel_control(
                  EV_INPUT_MESSAGE,   // event
                  new_message        // message
                );
                break;
              } /* RR_RESEL_IMSG ... */

              case RR_RESEL_OMSG:
              {
                switch (new_message->rr.header.imh.message_id)
                {
                  case RR_RESEL_OMSG_STARTED_IND:
                  {
                    rr_send_rr_reselection_ind();

                    switch (new_message->rr.resel.omsg.started_ind.type)
                    {
                      case RR_RESEL_TYPE_NORMAL:
                      {
                        grr_control_data_ptr->state = GRR_CELL_RESELECTION;
                        break;
                      } /* RR_RESEL_TYPE_NORMAL */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                      case RR_RESEL_TYPE_G2W:
                      {
                        grr_control_data_ptr->state = GRR_CELL_RESELECTION_G2W;
                        break;
                      }
#endif
                      default:
                      {
                        MSG_GERAN_LOW_3_G("Unexpected reselection type started: %d",
                          new_message->rr.resel.omsg.started_ind.type, 0, 0
                        );
                      }
                    }
                    break;
                  } /* RR_RESEL_OMSG_STARTED_IND */

                  case RR_RESEL_OMSG_ABORT_CNF:
                  {
                    /* Ignore this message in case of L1 MODE CHANGE
                    REQUIREMENT abort request should have sent in GRR_CAMPED
                    to resel and moves to GRR_ACQUIRE state and we can ignore
                    ABORT CNF in this state. */
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message,
                      get_grr_control_state_name(grr_control_data_ptr->state)
                    );
                  }
                }
                break;
              } /* RR_RESEL_OMSG */

              case RR_SELECT_BCCH_IMSG:
              {
                break;    // ignore - no processing required
              }

              case RR_PS_IMSG:
              {
                rr_ps_access_control(new_message);
                break;
              }

              case RR_PS_OMSG:
              {
                switch (new_message->rr.header.imh.message_id)
                {
                  case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                  {
                    new_grr_control_event = EV_PROCESS_SAVED_MESSAGE;
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message,
                      get_grr_control_state_name(grr_control_data_ptr->state)
                    );
                  }
                }
                break;
              }

              case RR_L1_IDLE_IMSG:
              case RR_L1_IDLE_OMSG:
              {
                rr_gprs_bsi_acquisition_control(EV_INPUT_MESSAGE, new_message);
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  new_message,
                  get_grr_control_state_name(grr_control_data_ptr->state)
                );
              }
            }
          } /* MS_RR_RR */

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
          else if (message_set == MS_RR_L1 && message_id == MPH_PROC_IND)
          {
              grr_control_data_ptr->state = GRR_NULL;
              output_event = relinquish_control_to_rr(grr_control_event);
          }
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */
          else
          /**********************************************************************/
          /* Pass all other messages into BCCH/PBCCH acquisition control        */
          /**********************************************************************/
          {
            new_grr_control_event =
              rr_gprs_bsi_acquisition_control(grr_control_event, new_message);
          }

        } /* EV_INPUT_MESSAGE */

        else
        /**********************************************************************/
        /* RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED                                 */
        /* The L1 idle mode needs to be changed. Re-camp on the cell in the   */
        /* appropriate mode                                                   */
        /**********************************************************************/
        if ( grr_control_event == RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED )
        {
          // After the L1 idle mode is changed, a RR_SERVICE_IND needs to be sent to MM
          grr_control_data_ptr->notify_mm_service_ind = TRUE;

          // Send request to RR-L1-IDLE-MODE module to change the L1 idle mode
          // This will abort any pending reselection and block PS access
          rr_l1_idle_send_imsg_change_idle_req();

          grr_control_data_ptr->state = GRR_STARTING_L1_IDLE;
        }

        else
        /***********************************************************************/
        /* RR_EV_CAMPED_ON_CELL                                                */
        /* The acquisition of sys info is complete, and GSM/GPRS idle has been */
        /* started in L1, and MM informed. MAC will have been informed if      */
        /* appropriate                                                         */
        /***********************************************************************/
        if ( ( grr_control_event == RR_EV_CAMPED_ON_CELL ) ||
             ( grr_control_event == EV_GRR51_GO_TO_PACKET_IDLE ) )
        {
#ifndef FEATURE_IDLE_DRX_SCALING
          /*Initialise the DSC*/
          rr_gprs_initialise_dsc();
#endif /* !FEATURE_IDLE_DRX_SCALING */
          rr_gprs_init_bad_snr_counter();

          /* Determine if reselection handling should be activated based on NC mode */
          grr_activate_reselection_if_allowed();

          /* Move to CAMPED state, start GRR camped procedures */
          grr_control_data_ptr->state = transition_to_camped_state(
            TRUE,                             // service_ind_sent
            &new_grr_control_event,
            (rr_cmd_bdy_type **)&new_message,
            &message_set,
            &message_id
          );
        }

        else
        /***********************************************************************/
        /* EV_RR_GPRS_ACQUISITION_FAILURE                                      */
        /* The acquisition of SIs timed-out - force a reselection              */
        /***********************************************************************/
        if ( grr_control_event == RR_EV_ACQUISITION_FAILURE )
        {
          /* Legacy reason given here. TBD whether a different reason
          is more appropriate. */
#ifdef FEATURE_GERAN_CS_SUPPORT
          (void) rr_abort_pending_rr_est_req(REASON_NOT_AVAILABLE);
#endif /* FEATURE_GERAN_CS_SUPPORT */

          transition_to_reselection_state(EV_FORCE_CELL_RESELECTION);
        }

        else if ( grr_control_event == RR_EV_W2G_GO_TO_INACTIVE )
        {
          grr_control_data_ptr->state = GRR_NULL;
        }

#ifdef FEATURE_GERAN_CS_SUPPORT
        else
        /********************************************************************/
        /* EV_RR_EST_REQ                                                    */
        /* A CS call needs to be established (either an MO call or a MT call*/
        /* in response to a page). Suspend GPRS, and return to RR           */
        /********************************************************************/
        if (grr_control_event == EV_RR_EST_REQ)
        {
          // Send request to block PS access (which may already be blocked)
          // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
          send_ps_imsg_no_ps_access_req(
            TRUE    // suspend_gmm
          );
        }
#endif /* FEATURE_GERAN_CS_SUPPORT */

        else
        /********************************************************************/
        /* EV_PROCESS_SAVED_MESSAGE                                         */
        /*   Return to RR to process a saved message.                       */
        /********************************************************************/
        if (grr_control_event == EV_PROCESS_SAVED_MESSAGE)
        {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          // If a X2G CCO is ongoing, send a 'success' indication
          if (rr_x2g_cco_in_progress())
          {
            rr_grr_send_omsg_x2g_cco_status_ind(
              RR_CCO_STATUS_SUCCESS    // status
            );
          }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

          grr_control_data_ptr->state = GRR_NULL;
          output_event = relinquish_control_to_rr(
            grr_control_event    // output_event
          );
        }
#ifdef FEATURE_WCDMA
        else if (grr_control_event == EV_CR_G2W_TIMER_EXPIRED)
        {
          MSG_GERAN_MED_0_G("Ignoring G2W timer expiry in GRR_ACQUIRE state");
        }
#endif
        /* Panic Reset triggered. Clean-up sys info acquisition */
        else if (grr_control_event == EV_PANIC_RESET)
        {
          (void) rr_gprs_bsi_acquisition_control(
            EV_RESET_SOFTWARE,    // event_in
            NULL                 // event_in_data
          );
        }

        else if (grr_control_event == EV_T3174_TIMEOUT)
        {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          if (rr_x2g_cco_in_progress())
          {
            /* Need to indicate to rr_control() that the X2G CCO procedure has
            failed due to timer expiration.  This will then cause rr_control()
            to shut down L1 and return control to the initiating RAT. */
            grr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY;

            // Send request to block PS access (which may already be blocked)
            // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
            send_ps_imsg_no_ps_access_req(
              FALSE   // suspend_gmm
            );

            grr_control_data_ptr->state = GRR_X2G_CCO_FAILED;
          }
          else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
          {
            new_grr_control_event = rr_resel_control(
              EV_T3174_TIMEOUT,   // event
              NULL               // message
            );
            grr_control_data_ptr->state = GRR_CELL_RESELECTION;
          }
        }
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
        /**
         * The following 3 events are possibly returned by the
         * transition_to_reselection_state() function.
         */
        else if (grr_control_event == RR_EV_RESELECT_WCDMA_CELL)
        {
          grr_control_data_ptr->state = GRR_CELL_RESELECTION_G2W;
        }
#endif
        else if (grr_control_event == EV_CELL_RESELECTION)
        {
          grr_control_data_ptr->state = GRR_CELL_RESELECTION;
        }

        else if (grr_control_event == RR_EV_TRIGGER_CELL_SELECTION)
        {
          grr_control_data_ptr->state = GRR_NULL;
          output_event = relinquish_control_to_rr(
            grr_control_event    // output_event
          );
        }

        else
        /**********************************************************************/
        /* Pass all other events into BCCH/PBCCH acquisition control          */
        /**********************************************************************/
        {
          new_grr_control_event = rr_gprs_bsi_acquisition_control(
            grr_control_event,    // evnet_in
            new_message          // evnet_in_data
          );
        }

        break;
      }

      case GRR_STARTING_L1_IDLE:
      {
        if (grr_control_event == EV_INPUT_MESSAGE)
        {
          switch (message_set)
          {
            case MS_RR_RR:
            {
              switch (new_message->rr.header.rr_message_set)
              {
                case RR_GRR_IMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
                    case RR_GRR_IMSG_MSC_CHANGE_IND:
                    {
                      /*
                      Update NAS as required
                      RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                      This will be sent when the current procedure completes or G is next active
                      */
                      rr_msc_update(
                        FALSE   // can_send_service_ind
                      );
                      break;
                    }
#endif

                    case RR_GRR_IMSG_PAGE_IND:
                    {
                      rr_grr_imsg_page_ind_t *imsg = &new_message->rr.grr.imsg.page_ind;

                      // A page could be received in this state due to previous Early Camping.
                      // A page is saved and forwarded to NAS when RR/L1 are in Packet Idle
                      rr_ce_save_page(
                        &imsg->page_info   // page_info_ptr
                      );
                      break;
                    }

                    case RR_GRR_IMSG_PAGE_MODE_IND:
                    {
                      // Ignore a change in OTA Page Mode as L1 may not be able to process an update.
                      // The change will again be indicated next time the paging block is read
                      MSG_GERAN_LOW_0_G("Change in Page Mode ignored");
                      break;
                    }

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                }

                case RR_L1_IDLE_IMSG:
                case RR_PS_IMSG:
                case RR_PS_OMSG:
                case RR_RESEL_IMSG:
                case RR_RESEL_OMSG:
                {
                  rr_l1_idle_control(new_message);
                  break;
                }

                case RR_L1_IDLE_OMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                    {
                      rr_l1_idle_omsg_start_idle_cnf_t *omsg = &new_message->rr.l1_idle.omsg.start_idle_cnf;
                      boolean service_ind_sent = FALSE;

                      if (omsg->gprs_allowed)
                      {
                        MSG_GERAN_MED_0_G("Cell supports GPRS");
                      }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                      if (FALSE == rr_multi_sim_mode_enabled())
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
                      {
                        /* Put L1 in MM non-DRX mode if this was requested by MM */
                        /* This is done for the case where GMM was mid-procedure on a DTM cell when the call was released */
                        rr_gprs_set_l1_in_mm_non_drx();
                      }

                      // Send a RR_PLMN_SELECT_CNF to NAS, if required
                      if (grr_control_data_ptr->notify_mm_plmn_select_cnf)
                      {
                        send_rr_plmn_select_cnf();
                      }

                      // Send a RR_SERVICE_IND to NAS, if required
                      if (grr_control_data_ptr->notify_mm_service_ind)
                      {
                        // Note: This resets grr_control_data_ptr->notify_mm_service_ind
                        send_rr_service_ind();

                        // so take a copy
                        service_ind_sent = TRUE;
                      }

#ifndef FEATURE_IDLE_DRX_SCALING
                      // Initialise the DSC
                      rr_gprs_initialise_dsc();
#endif /* !FEATURE_IDLE_DRX_SCALING */
                      rr_gprs_init_bad_snr_counter();

                      // Activate reselection (if applicable to NC mode)
                      grr_activate_reselection_if_allowed();

                      (void) rr_gprs_nc_measurement_control(
                        RR_EV_CAMPED_ON_CELL,
                        NULL
                      );

                      grr_control_data_ptr->state = transition_to_camped_state(
                        service_ind_sent,
                        &new_grr_control_event,
                        (rr_cmd_bdy_type **)&new_message,
                        &message_set,
                        &message_id
                      );                      
                      break;
                    }

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                } // RR_L1_IDLE_OMSG

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }
            }

            case MS_MAC_RR:
            {
              break;   // ignore any messages from MAC
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                new_message,
                get_grr_control_state_name(grr_control_data_ptr->state)
              );
            }
          }

          break;
        }

        else
        if (grr_control_event == EV_PANIC_RESET)
        {
          // Send a RR_PLMN_SELECT_CNF to NAS, if required
          if (grr_control_data_ptr->notify_mm_plmn_select_cnf)
          {
            send_rr_plmn_select_cnf();
          }
        }

        else
        {
          LOG_UNEXPECTED_EVENT(
            rr_event_name(grr_control_event),
            get_grr_control_state_name(grr_control_data_ptr->state)
          );
        }

        break;
      }

      case GRR_CAMPED_ENABLING_PS_ACCESS:
      {
        if (grr_control_event == EV_INPUT_MESSAGE)
        {
          switch (message_set)
          {
            case MS_RR_RR:
            {
              switch (new_message->rr.header.rr_message_set)
              {
                case RR_GRR_IMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_GRR_IMSG_PAGE_IND:
                    {
                      rr_grr_imsg_page_ind_t *imsg = &new_message->rr.grr.imsg.page_ind;

                      // Forward the page to NAS
                      rr_ce_send_page_to_nas(
                        &imsg->page_info   // page_info_ptr
                      );
                      break;
                    }

                    case RR_GRR_IMSG_PAGE_MODE_IND:
                    {
                      rr_grr_imsg_page_mode_ind_t *imsg = &new_message->rr.grr.imsg.page_mode_ind;

                      // Update L1 with new Page Mode
                      rr_ce_process_page_mode_change(
                        imsg->old_page_mode,    // old_page_mode
                        imsg->new_page_mode    // new_page_mode
                      );
                      break;
                    }

#ifdef FEATURE_LTE
                    case RR_GRR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND:
                    {
                      rr_resel_send_imsg(RR_RESEL_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND);
                      break;
                    } /* RR_GRR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND */
#endif /* FEATURE_LTE */

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                }

                case RR_PS_IMSG:
                {
                  rr_ps_access_control(new_message);
                  break;
                }

                case RR_PS_OMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_PS_OMSG_PS_ACCESS_CNF:
                    {
                      // Call the call-back function (if one was supplied)
                      CALL_RR_CB_FN(grr_control_data_ptr->ps_access_confim_cb_fn);
                      grr_control_data_ptr->ps_access_confim_cb_fn = NULL;

                      rr_send_imsg(RR_IMSG_PS_ACCESS_IND);

                      // TODO [free-float] Will return to RR but this could be UL_TBF_REQ
//                      if (rr_examine_saved_message(NULL) == EV_INPUT_MESSAGE)
//                      {
//                        new_grr_control_event = EV_PROCESS_SAVED_MESSAGE;
//                      }

#ifdef FEATURE_SGLTE
                      if (rr_sglte_perform_g2l_blind_redir())
                      {
                          send_ps_imsg_no_ps_access_req(
                            FALSE   // suspend_gmm 
                          );

                          /*Disable PS access and then initiate redirection*/

                          /*trigger redirection of PS domain to TDS/LTE, GERAN
                          remains camped for this procedure but need to move in 
                          to RR_G2X_REDIR state to process this.  GRR control 
                          will be moved to GRR_NULL for this*/ 
                          rr_set_sglte_perform_g2l_blind_redir(FALSE);

                          MSG_HIGH("SGLTE PS is activated, service pending G2L blind redir",0,0,0);

                          output_event = EV_SGLTE_G2L_BLIND_REDIR_REQUIRED;
                          grr_control_data_ptr->state = GRR_NULL;   
                      }
                      else
#endif /*FEATURE_SGLTE*/
                      {
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION                       
                        /*Inform reselection module  for any reduced mobility */
                        if (rr_ue_in_peo_mode())
                        {
                           rr_resel_send_imsg_reduced_mobility_required_ind(RR_REDUCED_MOBILITY_MAX);
                        }
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/                      
                        grr_control_data_ptr->state = GRR_CAMPED;
                      }
                      break;
                    }

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                case RR_RESEL_G2W_IMSG:
                case RR_RESEL_IMSG:
                {
                  new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                  break;
                } /* RR_RESEL_IMSG */

                case RR_RESEL_OMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_RESEL_OMSG_STARTED_IND:
                    {
                      rr_send_rr_reselection_ind();
                
                      switch (new_message->rr.resel.omsg.started_ind.type)
                      {
                        case RR_RESEL_TYPE_NORMAL:
                        {
                          grr_control_data_ptr->state = GRR_CELL_RESELECTION;
                          break;
                        } /* RR_RESEL_TYPE_NORMAL */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                        case RR_RESEL_TYPE_G2W:
                        {
                          grr_control_data_ptr->state = GRR_CELL_RESELECTION_G2W;
                          break;
                        }
#endif                
                        default:
                        {
                          MSG_GERAN_ERROR_3_G("Unexpected reselection type started: %d",
                            new_message->rr.resel.omsg.started_ind.type, 0, 0
                          );
                        }
                      }
                      break;
                    } /* RR_RESEL_OMSG_STARTED_IND */
                
                    case RR_RESEL_OMSG_ABORT_CNF:
                    {
                      break;
                    }
                
                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                } /* RR_RESEL_OMSG */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }
              break;
            }

            case MS_MAC_RR:
            {
              switch (message_id)
              {
                case MAC_GRR_MAC_STATUS:
                {
                  break;  // no action required
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                new_message,
                get_grr_control_state_name(grr_control_data_ptr->state)
              );
            }
          }
        }

        else
        if (grr_control_event == EV_PANIC_RESET)
        {
          ;  // No specific action required
        }

        else
        {
          LOG_UNEXPECTED_EVENT(
            rr_event_name(grr_control_event),
            get_grr_control_state_name(grr_control_data_ptr->state)
          );
        }

        break;
      }

      case GRR_CAMPED_DISABLING_PS_ACCESS:
      {
        if (grr_control_event == EV_INPUT_MESSAGE)
        {
          switch (message_set)
          {
            case MS_RR_RR:
            {
              switch (new_message->rr.header.rr_message_set)
              {
                case RR_GRR_IMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_GRR_IMSG_PAGE_IND:
                    {
                      rr_grr_imsg_page_ind_t *imsg = &new_message->rr.grr.imsg.page_ind;

                      // Forward the page to NAS
                      rr_ce_send_page_to_nas(
                        &imsg->page_info   // page_info_ptr
                      );
                      break;
                    }

                    case RR_GRR_IMSG_PAGE_MODE_IND:
                    {
                      rr_grr_imsg_page_mode_ind_t *imsg = &new_message->rr.grr.imsg.page_mode_ind;

                      // Update L1 with new Page Mode
                      rr_ce_process_page_mode_change(
                        imsg->old_page_mode,    // old_page_mode
                        imsg->new_page_mode    // new_page_mode
                      );
                      break;
                    }

#ifdef FEATURE_LTE
                    case RR_GRR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND:
                    {
                      rr_resel_send_imsg(RR_RESEL_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND);
                      break;
                    } /* RR_GRR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND */
#endif /* FEATURE_LTE */

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                }

                case RR_PS_IMSG:
                {
                  rr_ps_access_control(new_message);
                  break;
                }

                case RR_PS_OMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                    {
                      // Call the call-back function (if one was supplied)
                      CALL_RR_CB_FN(grr_control_data_ptr->no_ps_access_confim_cb_fn);
                      grr_control_data_ptr->no_ps_access_confim_cb_fn = NULL;   

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 
                      /*Inform reselection module  for any reduced mobility */
                      if (rr_ue_in_peo_mode())
                      {
                         rr_resel_send_imsg_reduced_mobility_required_ind(RR_REDUCED_MOBILITY_MAX);
                      }
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
                      if (TRUE == rr_static_get_suspend_in_progress())
                      {
                        MSG_GERAN_MED_0_G("DMMSUSREM:: GRR5152_Control NO_BCCH");
                        output_event = relinquish_control_to_rr(EV_NO_BCCH_FOUND);
                        grr_control_data_ptr->state = GRR_NULL;
                      }
                      else
#endif
                      {
                        grr_control_data_ptr->state = GRR_CAMPED;

                        if (rr_examine_saved_message(NULL) == EV_INPUT_MESSAGE)
                        {
                          new_grr_control_event = EV_PROCESS_SAVED_MESSAGE;
                        }
                      }
                      break;
                    }

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                }

#ifdef FEATURE_LTE
                case RR_RESEL_IMSG:
                {
                  new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                  break;
                } /* RR_RESEL_IMSG */
#endif /* FEATURE_LTE */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }
              break;
            }

            case MS_MAC_RR:
            {
              switch (message_id)
              {
                // Soak-up messages which can be received in this state but can be ignored
                case MAC_GRR_MAC_STATUS:
                case MAC_GRR_RACH_CNF:
                {
                  break;  // no action required
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                new_message,
                get_grr_control_state_name(grr_control_data_ptr->state)
              );
            }
          }
        }

        else
        if (grr_control_event == EV_PANIC_RESET)
        {
          ;  // No specific action required
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(grr_control_event),
                               get_grr_control_state_name(grr_control_data_ptr->state));
        }

        break;
      }

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* GRR_CAMPED - In this state GRR51 is camped and performing      */
      /* IDLE/TRANSFER mode proceedures                                 */
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case GRR_CAMPED:
      {
        /***************************************************************/
        /* INPUT MESSAGE handlers                                      */
        /***************************************************************/
        if (grr_control_event == EV_INPUT_MESSAGE)
        {
          switch (message_set)
          {
            case MS_RR_RR:
            {
              switch (new_message->rr.header.rr_message_set)
              {
                case RR_GRR_IMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_GRR_IMSG_CAMPED_ON_CELL_IND:
                    {
                      new_grr_control_event = RR_EV_CAMPED_ON_CELL;
                      break;
                    } /* RR_GRR_IMSG_CAMPED_ON_CELL_IND */

                    case RR_GRR_IMSG_PS_ACCESS_IND:
                    {
                      new_grr_control_event = rr_resel_control(
                        EV_PS_ACCESS_IND,
                        NULL
                      );
                      break;
                    } /* RR_GRR_IMSG_PS_ACCESS_IND */

#ifdef FEATURE_LTE

                    case RR_GRR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND:
                    {
                      rr_l1_update_wcdma_neighbor_list();
                      rr_resel_send_imsg(
                        RR_RESEL_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND
                      );
                      break;
                    } /* RR_GRR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND */

                    case RR_GRR_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND:
                    {
                      rr_l1_update_wcdma_neighbor_list();
                      rr_resel_send_imsg(
                        RR_RESEL_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND
                      );
                      break;
                    } /* RR_GRR_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND */

                    case RR_GRR_IMSG_X2G_CCO_COMPLETE_IND:
                    {
                      rr_l1_update_wcdma_neighbor_list();
                      /* TODO: Any need to inform rr_resel_control()? */
                      break;
                    } /* RR_GRR_IMSG_X2G_CCO_COMPLETE_IND */

                    case RR_GRR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND:
                    {
                      rr_resel_send_imsg(RR_RESEL_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND);
                      break;
                    } /* RR_GRR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND */
#endif /* FEATURE_LTE */

                    case RR_GRR_IMSG_PS_ACCESS_REQ:
                    {
                      rr_grr_imsg_ps_access_req_t *imsg = &new_message->rr.grr.imsg.ps_access_req;

                      if (!rr_resel_reselection_is_pending()) 
                      {
                        
                        grr_control_data_ptr->ps_access_confim_cb_fn = imsg->confirm_cb_fn;

                        send_ps_imsg_ps_access_req(
                          imsg->purge_gmm_signalling   // purge_gmm_signalling
                        );

                        grr_control_data_ptr->state = GRR_CAMPED_ENABLING_PS_ACCESS;
                      }
                      else
                      {
                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn);

                        MSG_GERAN_HIGH_0_G("PS access blocked as reselection pending");
                      }
                      break;
                    }

                    case RR_GRR_IMSG_NO_PS_ACCESS_REQ:
                    {
                      rr_grr_imsg_no_ps_access_req_t *imsg = &new_message->rr.grr.imsg.no_ps_access_req;

                      /**
                       * The expected response would be a RR_PS_IMSG_NO_PS_ACCESS_REQ is sent into 
                       * RR-PS-ACCESS and the main state is changed to CAMPED_DISABLING_PS_ACCESS. If PS 
                       * access is already blocked, then this will be a temporary transition and RR-PS-ACCESS 
                       * will confirm straight back. 
                       * However, there is a case where reselection has been triggered and the reselection 
                       * modules have requested PS access be blocked. If that case, if the above sequence is 
                       * followed, they will never receive the PS access blocked confirmation. 
                       * So to work-around this, check if PS access is already blocked first and if so, just 
                       * call the confirmation call-back. 
                       */
                      if (rr_mac_ps_access_blocked())
                      {
                        CALL_RR_CB_FN(imsg->confirm_cb_fn);
                      }
                      else
                      {
                        grr_control_data_ptr->no_ps_access_confim_cb_fn = imsg->confirm_cb_fn;

                        send_ps_imsg_no_ps_access_req(
                          FALSE   // suspend_gmm
                        );

                        grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
                      }
                      break;
                    }

                    case RR_GRR_IMSG_GPRS_IA_IND:
                    {
                      rr_grr_imsg_gprs_ia_ind_t *imsg = &new_message->rr.grr.imsg.gprs_ia_ind;

                      // Send the GPRS IA to MAC
                      grr_send_gprs_dl_ia_to_mac(
                        &imsg->packet_ch_desc,
                        &imsg->ia_rest_octets,
                        &imsg->frequency_list,
                        imsg->timing_advance
                      );
                      break;
                    }

                    case RR_GRR_IMSG_RR_EVENT_IND:
                    {
                      rr_grr_imsg_rr_event_ind_t *imsg = &new_message->rr.grr.imsg.rr_event_ind;

                      new_grr_control_event = imsg->rr_event;
                      break;
                    }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
                    case RR_GRR_IMSG_MSC_CHANGE_IND:
                    {
                      send_ps_imsg_no_ps_access_req(
                        FALSE   // suspend_gmm
                      );

                      grr_control_data_ptr->state = GRR_CAMPED_MSC_CHANGE;
                      break;
                    }
#endif
                    case RR_GRR_IMSG_PAGE_IND:
                    {
                      rr_grr_imsg_page_ind_t *imsg = &new_message->rr.grr.imsg.page_ind;

                      rr_ce_send_page_to_nas(
                        &imsg->page_info
                      );
                      break;
                    }

                    case RR_GRR_IMSG_PAGE_MODE_IND:
                    {
                      rr_grr_imsg_page_mode_ind_t *imsg = &new_message->rr.grr.imsg.page_mode_ind;

                      rr_ce_process_page_mode_change(
                        imsg->old_page_mode,
                        imsg->new_page_mode
                      );
                      break;
                    }

                    case RR_GRR_IMSG_L1_IDLE_MODE_CHANGE_REQUIRED:
                    {
                      /** 
                      * There may be a case where reselection has been triggered.
                      * if SI13 received then this message tries to put GL1 into GPRS idle mode,
                      * that leads to abort of reselection.											 
                      */
                      if (!rr_resel_reselection_is_pending())
                      {
                        // SI refresh is suspended here so that it is restarted
                        // correctly when L1 idle mode is changed.
                        rr_gprs_suspend_camped_sys_info_handling(FALSE);

                        // After the L1 idle mode is changed, a RR_SERVICE_IND needs to be sent to MM
                        grr_control_data_ptr->notify_mm_service_ind = TRUE;

                        // Send request to RR-L1-IDLE-MODE module to change the L1 idle mode
                        // This will abort any pending reselection and block PS access
                        rr_l1_idle_send_imsg_change_idle_req();

                        grr_control_data_ptr->state = GRR_STARTING_L1_IDLE;
                      }
                      else
                      {
                        MSG_GERAN_HIGH_0_G("Reselection is in progress, can't change the mode"); 
                      }
                      break;
                    } /* RR_GRR_L1_IDLE_MODE_CHANGE_REQUIRED */

#ifdef FEATURE_SGLTE
                    case RR_GRR_IMSG_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ:
                    case RR_GRR_IMSG_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ:
                    {
                      rr_grr_imsg_switch_to_cs_only_gsm_only_req_t * switch_to_cs_only_gsm_only_req;

                      switch_to_cs_only_gsm_only_req = &new_message->rr.grr.imsg.switch_to_cs_only_gsm_only_req;

                      grr_control_data_ptr->sglte_ps_removal_cb_fn = switch_to_cs_only_gsm_only_req->cb_fn;

                      rr_send_mph_ue_mode_change_req(TRUE);

                      grr_control_data_ptr->state = GRR_SGLTE_SWITCHING_TO_GSM_ONLY;
                      break;
                    } /* RR_GRR_IMSG_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ */

                    case RR_GRR_IMSG_SGLTE_X2G_REQ:
                    {
                      if (rr_is_sglte_mode_active())
                      {
                        rr_grr_imsg_sglte_x2g_req_t * req = &new_message->rr.grr.imsg.sglte_x2g_cco_req;

                        rr_sglte_x2g_init();

                        rr_sglte_x2g_send_imsg_resel_req(
                          req->activation_reason,
                            &req->rat_pri_list_info,
                            &req->lte_dedicated_pri); 

                        grr_control_data_ptr->state = GRR_SGLTE_X2G_RESEL;
                      }
                      else
                      {
                        LOG_UNEXPECTED_MESSAGE(new_message, "GRR_CAMPED");
                      }
                      break;
                    } /* RR_GRR_IMSG_SGLTE_X2G_REQ */
#endif /* FEATURE_SGLTE */

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(new_message, "GRR_CAMPED");
                    }
                  }
                  break;
                } /* RR_GRR_IMSG */

                case RR_L1_IDLE_OMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                      MSG_GERAN_HIGH_0_G("Idle mode confirmed, no further action required");
                      break;

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(new_message, "GRR_CAMPED");
                    }
                  }
                  break;
                } /* RR_L1_IDLE_OMSG */

                case RR_RESEL_IMSG:
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
                case RR_RESEL_RMC_IMSG:
                case RR_RESEL_RMC_OMSG:
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/
                case RR_CELL_ACQ_IMSG:
                case RR_CELL_ACQ_OMSG:
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                case RR_RESEL_G2W_IMSG:
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                {
                  new_grr_control_event = rr_resel_control(
                    EV_INPUT_MESSAGE,
                    new_message
                  );
                  break;
                } /* RR_CELL_ACQ_IMSG ... */

                case RR_RESEL_OMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_RESEL_OMSG_ABORT_CNF:
                    {
                      byte saved_message_id;

#ifdef FEATURE_SGLTE
                      uint32 saved_umid;
#endif /* FEATURE_SGLTE */

                      if (grr_examine_saved_mm_message(&saved_message_id))
                      {
                        MSG_GERAN_HIGH_1_G("Saved message from MM: %d", saved_message_id);

                        switch (saved_message_id)
                        {
#ifdef FEATURE_GERAN_CS_SUPPORT
                          case RR_EST_REQ:
                          {
                            rr_event_T grr_event;
                            rr_cmd_bdy_type *message_ptr;

                            (void)rr_examine_saved_message(&message_ptr);

                            /*
                             This returns
                             EV_NO_EVENT - CS conn est not allowed
                             EV_FORCE_RESELECTION - SNR is bad, reselect first
                             EV_RR_EST_REQ - CS conn est can proceed
                             */
                            grr_event = preprocess_rr_est_req(
                              (rr_est_req_T *)message_ptr
                            );

                            if (grr_event == EV_NO_EVENT)
                            {
                              /* Reject request back to MM, and discard saved message */
                              rr_send_abort_ind(OTHER_REASON, MM_RR_OTHER_FAILURE);
                              grr_throwaway_saved_message();
                            }
                            else
                            if (grr_event == EV_RR_EST_REQ)
                            {
                              /* handle_rr_est_req():                                     */
                              /*  If the CS call is allowed, EV_RR_EST_REQ is returned    */
                              /*  If the CS call is not allowed, EV_NO_EVENT is returned, */
                              /*   and RR_ABORT_IND sent to MM                            */
                              new_grr_control_event = handle_rr_est_req(
                                (rr_est_req_T *) message_ptr
                              );
                            }
                            else
                            if (grr_event == EV_FORCE_CELL_RESELECTION)
                            {
                              MSG_GERAN_HIGH_0_G("Continue with reselection (SNR bad), queue RR_EST_REQ");
                              rr_resel_send_imsg_required_ind(
                                RR_RESEL_CAUSE_RESELECTION
                              );
                            }
                            break;
                          } /* RR_EST_REQ */
#endif /* FEATURE_GERAN_CS_SUPPORT */

                          case RR_PLMN_SELECT_REQ:
                          {
                            // Send request to block PS access (which may already be blocked)
                            // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
                            send_ps_imsg_no_ps_access_req(
                              FALSE   // suspend_gmm
                            );

                            // Wait for the confirmation in GRR_CAMPED_DISABLING_PS_ACCESS
                            // Then the message will be processed
                            grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
                            break;
                          } /* RR_PLMN_SELECT_REQ */

                          case RR_STOP_GSM_MODE_REQ:
                          case RR_DEACT_REQ:
                          {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                            if (rr_x2g_cco_in_progress())
                            {
                              MSG_GERAN_HIGH_0_G("Abort X2G CCO procedure for NAS request");
                              new_grr_control_event = EV_X2G_CCO_ABORTED_FOR_HIGH_PRI_REQ;
                            }
                            else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                            {
                              MSG_GERAN_HIGH_0_G("RR_STOP_GSM_MODE_REQ/RR_DEACT_REQ received in GRR_CAMPED state");

                              /*Abort reselection*/
                              (void) rr_gprs_nc_measurement_control(
                                EV_RESET_SOFTWARE,
                                NULL
                              );

                              // Block PS access and then process the message
                              // Note: PDUs may not be purged here, but when rr_ps_no_ps_access_del_all()
                              // is called later, PDUs will be purged there if not done already
                              send_ps_imsg_no_ps_access_req(
                                FALSE   // suspend_gmm
                              );

                              grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
                            }
                            break;
                          } /* RR_STOP_GSM_MODE_REQ ... */

                          case RR_PLMN_LIST_REQ:
                          {
                            rr_plmn_list_req_T *rr_plmn_list_req;
                            rr_cmd_bdy_type *message_ptr;

                            (void)rr_examine_saved_message(&message_ptr);

                            rr_plmn_list_req = (rr_plmn_list_req_T *) message_ptr;
#ifdef FEATURE_BPLMN_SEARCH_PRIORITY
                            rr_plc_set_ps_service_priority(FALSE);

                            // Send request to block PS access (which may already be blocked)
                            // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
                            send_ps_imsg_no_ps_access_req(
                              TRUE    // suspend_gmm
                            );

                            // Wait for the confirmation in GRR_CAMPED_DISABLING_PS_ACCESS
                            // Then the message will be processed
                            grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
#else
                            if ((rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
                                rr_gsm_camped_on_ccch())
                            {
                              /* suspend GPRS during the whole plmn search cycle,
                               * GRR moves to GRR_NULL
                               */
                              rr_plc_set_ps_service_priority(FALSE);

                              // Send request to block PS access (which may already be blocked)
                              // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
                              send_ps_imsg_no_ps_access_req(
                                TRUE    // suspend_gmm
                              );

                              // Wait for the confirmation in GRR_CAMPED_DISABLING_PS_ACCESS, then the message
                              // will be processed
                              grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
                            }
                            else
                            {
                              /* GPRS is only suspended temporily in PLMN list control (when L1 is deactivated )
                               * GRR stays in GRR_CAMPED
                               */
                              rr_plc_set_ps_service_priority(TRUE);
                              new_grr_control_event = EV_GRR_PLMN_SEARCH_READY_TO_START;
                            }
#endif /* FEATURE_BPLMN_SEARCH_PRIORITY */
                            break;
                          } /* RR_PLMN_LIST_REQ */

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("Unexpected saved message (0x%02x) from MM", saved_message_id);
                          }
                        }
                      } /* grr_examine_saved_mm_message() */

                      else if (grr_examine_saved_mac_message(&saved_message_id))
                      {
                        MSG_GERAN_HIGH_1_G("Saved message from GMAC: %d", saved_message_id);

                        switch (saved_message_id)
                        {
                          case MAC_GRR_UL_TBF_REQ:
                          {
                            mac_grr_ul_tbf_req_t * mac_grr_ul_tbf_req;
                            /* get the saved message*/
                            (void) rr_get_saved_msg(&new_message);

                            mac_grr_ul_tbf_req = (mac_grr_ul_tbf_req_t *)new_message;

                            /* Do not trigger Force SNR based Reselection in case of PAGE RSP or a ongoing CCO is in progress ? */
                            if ( rr_gprs_is_snr_counter_zero() &&
                                 (mac_grr_ul_tbf_req->channel_req_info.pkt_est_cause != RR_PKT_EST_CAUSE_PAGE_RESP) &&
                                 ( (rr_x2g_cco_in_progress() == FALSE) && (rr_pcco_in_progress() == FALSE) )
                                 )
                            {
                              /* kick off an snr based reselection */
                              new_grr_control_event = EV_FORCE_CELL_RESELECTION;

                              rr_log_set_reselection_trigger(
                                RR_UL_TBF_REQ_SNR_RESELECTION_TRIGGER
                              );

                              MSG_GERAN_HIGH_0_G("++SNR Reselection for ul_tbf_req++");

                            }
                            else
                            {
                              if (rr_mac_ps_access_blocked())
                              {
                                MSG_GERAN_HIGH_0_G("Ignoring MAC_GRR_UL_TBF_REQ, PS access blocked");
                              }
                              else
                              {
                                /* Pass this to grr_conn_est_control(). This will return EV_GRR_EST_REQ */
                                /* if a GPRS conn est is proceeding */
                                new_grr_control_event = grr_conn_est_control(
                                  EV_INPUT_MESSAGE,
                                  new_message
                                );
                              }
                            }
                            break;
                          } /* MAC_GRR_UL_TBF_REQ */

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("Unexpected saved message (0x%02x) from MM", saved_message_id);
                          }
                        }
                      } /* grr_examine_saved_mac_message() */
#ifdef FEATURE_SGLTE
#ifdef FEATURE_LTE
                      else if (grr_examine_saved_msgr_message(&saved_umid))
                      {
                        switch (saved_umid)
                        {
                          case GERAN_GRR_LTE_PLMN_SRCH_REQ:
                          {
                            lte_irat_plmn_srch_req_s * plmn_srch_req;
                            rr_cmd_bdy_type *message_ptr;

                            (void) rr_examine_saved_message( &message_ptr);

                            plmn_srch_req = (lte_irat_plmn_srch_req_s *) message_ptr;
#ifdef FEATURE_BPLMN_SEARCH_PRIORITY
                            rr_plc_set_ps_service_priority(FALSE);

                            // Send request to block PS access (which may already be blocked)
                            // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
                            send_ps_imsg_no_ps_access_req(
                              TRUE // suspend_gmm
                            );

                            // Wait for the confirmation in GRR_CAMPED_DISABLING_PS_ACCESS
                            // Then the message will be processed
                            grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
#else
                            if ((plmn_srch_req->network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
                                rr_gsm_camped_on_ccch())
                            {
                              /* suspend GPRS during the whole plmn search cycle,
                               * GRR moves to GRR_NULL
                               */
                              rr_plc_set_ps_service_priority(FALSE);

                              // Send request to block PS access (which may already be blocked)
                              // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
                              send_ps_imsg_no_ps_access_req(
                                TRUE   // suspend_gmm
                              );

                              // Wait for the confirmation in GRR_CAMPED_DISABLING_PS_ACCESS, then the message
                              // will be processed
                              grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
                            }
                            else
                            {
                              /* GPRS is only suspended temporily in PLMN list control (when L1 is deactivated )
                               * GRR stays in GRR_CAMPED
                               */
                              rr_plc_set_ps_service_priority(TRUE);
                              new_grr_control_event = EV_GRR_PLMN_SEARCH_READY_TO_START;
                            }
#endif /* FEATURE_BPLMN_SEARCH_PRIORITY */
                            break;
                          } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("Unexpected saved message, UMID 0x%08x", saved_umid);
                          }
                        }
                      }
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
                      else if (grr_examine_saved_tdsrrc_message(&saved_message_id))
                      {
                        switch (saved_message_id)
                        {
                          case TDSRR_INTERRAT_PLMN_SRCH_REQ:
                          {
                            tdsrrc_interrat_plmn_srch_req_type * plmn_srch_req;
                            rr_cmd_bdy_type *message_ptr;

                            (void) rr_examine_saved_message(&message_ptr);

                            plmn_srch_req = (tdsrrc_interrat_plmn_srch_req_type *) message_ptr;
#ifdef FEATURE_BPLMN_SEARCH_PRIORITY
                            rr_plc_set_ps_service_priority(FALSE);

                            // Send request to block PS access (which may already be blocked)
                            // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
                            send_ps_imsg_no_ps_access_req(
                              TRUE   // suspend_gmm
                            );

                            // Wait for the confirmation in GRR_CAMPED_DISABLING_PS_ACCESS
                            // Then the message will be processed
                            grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
#else
                            if ((plmn_srch_req->network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) ||
                                rr_gsm_camped_on_ccch())
                            {
                              /* suspend GPRS during the whole plmn search cycle,
                               * GRR moves to GRR_NULL
                               */
                              rr_plc_set_ps_service_priority(FALSE);

                              // Send request to block PS access (which may already be blocked)
                              // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
                              send_ps_imsg_no_ps_access_req(
                                TRUE   // suspend_gmm
                              );

                              // Wait for the confirmation in GRR_CAMPED_DISABLING_PS_ACCESS, then the message
                              // will be processed
                              grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
                            }
                            else
                            {
                              /* GPRS is only suspended temporily in PLMN list control (when L1 is deactivated )
                               * GRR stays in GRR_CAMPED
                               */
                              rr_plc_set_ps_service_priority(TRUE);
                              new_grr_control_event = EV_GRR_PLMN_SEARCH_READY_TO_START;
                            }
#endif /* FEATURE_BPLMN_SEARCH_PRIORITY */
                            break;
                          } /* TDSRR_INTERRAT_PLMN_SRCH_REQ */

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("Unexpected saved message, message_id 0x%02x", saved_message_id);
                          }
                        }
                      }
#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_SGLTE */
                      else
                      {
                        /* This can only happen in case of RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED event GRR control
                           sends abort to resel control. But it does not wait for abort cnf and continue changing the layer1 mode*/
                        MSG_GERAN_HIGH_0_G("Ignoring RESEL ABORT CNF ");
                      }
                      break;
                    } /* RR_RESEL_OMSG_ABORT_CNF */

                    case RR_RESEL_OMSG_STARTED_IND:
                    {
                      /* notify measurement reporting of the cell reselection */
                      (void) rr_gprs_nc_measurement_control(
                        RR_EV_DEACTIVATE_MEASUREMENTS,
                        new_message
                      );

                      rr_send_rr_reselection_ind();

                      switch (new_message->rr.resel.omsg.started_ind.type)
                      {
                        case RR_RESEL_TYPE_NORMAL:
                        {
                          grr_control_data_ptr->state = GRR_CELL_RESELECTION;
                          break;
                        } /* RR_RESEL_TYPE_NORMAL */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                        case RR_RESEL_TYPE_G2W:
                        {
                          /* If a G2W reselection is triggered and RR has a
                          saved RR_EST_REQ (as might be the case if RR_EST_REQ
                          is received and SNR is bad, causing immediate
                          reselection to a neighbour cell with a five second
                          timer running) then the RR_EST_REQ must be cleared
                          out from rr_control() saved message store and moved
                          in to the local grr_control() saved_rr_est_req store,
                          as otherwise it's presence will cause the
                          RR_STOP_GSM_MODE_REQ that indicates successful
                          completion of G2W reselection to be buffered
                          indefinitely, causing a lockup. */
                          uint8 saved_message_id;
                          if (grr_examine_saved_mm_message(&saved_message_id))
                          {
#ifdef FEATURE_GERAN_CS_SUPPORT
                            if (saved_message_id == RR_EST_REQ)
                            {
                              rr_cmd_bdy_type *saved_message_ptr;

                              MSG_GERAN_HIGH_0_G("Moving saved RR_EST_REQ from RR to GRR during IRAT");

                              (void) rr_examine_saved_message(&saved_message_ptr);

                              // Take a local copy of the RR_EST_REQ
                              grr_control_data_ptr->saved_rr_est_req = (*(rr_est_req_T *)saved_message_ptr);
                              grr_control_data_ptr->saved_rr_est_req_valid = TRUE;

                              // Discard the copy saved in RR-CONTROL
                              grr_throwaway_saved_message();
                            }
#endif /* FEATURE_GERAN_CS_SUPPORT */
                          }
                          grr_control_data_ptr->state = GRR_CELL_RESELECTION_G2W;
                          break;
                        }
#endif
                        default:
                        {
                          MSG_GERAN_ERROR_1_G(
                            "Unexpected reselection type started: %d",
                            new_message->rr.resel.omsg.started_ind.type
                          );
                        }
                      }
                      break;
                    } /* RR_RESEL_OMSG_STARTED_IND */

                    case RR_RESEL_OMSG_MAC_IDLE_IND:
                    {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                      if (RR_PS_ABORT_ACTIVE_WILL_RESUME == rr_ms_ps_abort_process_active())
                      {
                        MSG_GERAN_HIGH_0_G("PS abort process active, ignore MAC idle status");
                      }
                      else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
                      if (rr_mac_ps_access_blocked())
                      {
                        MSG_GERAN_HIGH_0_G("MAC_IDLE received in GRR_CAMPED, resuming PS in MAC");
                        rr_ps_send_imsg_ps_access_req(
                          FALSE
                        );
                        grr_control_data_ptr->state = GRR_CAMPED_ENABLING_PS_ACCESS;
                      }
                      else
                      {
                        MSG_GERAN_HIGH_0_G("MAC_IDLE received in GRR_CAMPED, PS already resumed");
                      }
                      break;
                    } /* RR_RESEL_OMSG_MAC_IDLE_IND */

                    case RR_RESEL_OMSG_FINISHED_IND:
                    {
                      if ((rr_mac_ps_access_blocked()) && (rr_mac_ul_tbf_est_is_allowed()))
                      {
                        MSG_GERAN_HIGH_0_G("Reselection cancelled;resuming PS in MAC");
                        rr_ps_send_imsg_ps_access_req(
                           FALSE
                        );
                        grr_control_data_ptr->state = GRR_CAMPED_ENABLING_PS_ACCESS;
                      }
                      else
                      {
                        MSG_GERAN_HIGH_0_G("Reselection cancelled");
                      }
                      break;
                    } /*RR_RESEL_OMSG_FINISHED_IND*/

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                } /* RR_RESEL_OMSG */

                case RR_SELECT_BCCH_IMSG:
                {
                  break;    // ignore - no processing required
                }

                case RR_PS_IMSG:
                {
                  rr_ps_access_control(new_message);
                  break;
                }

                case RR_PS_OMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                    {
#ifdef FEATURE_GSM_EDTM
                      (void) rr_gprs_edtm_control(EV_MAC_IDLE_MODE, NULL);
#endif /* #ifdef FEATURE_GSM_EDTM */

                      new_grr_control_event = rr_resel_control(EV_PS_ACCESS_BLOCKED, new_message);
                      break;
                    }

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                }
#ifdef FEATURE_SGLTE
                case RR_SGLTE_BAND_COEX_OMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_SGLTE_BAND_COEX_OMSG_SUPPORTED_BANDS_UPDATED_IND:
                    {
                      /* Need to check if the current serving cell's band is
                      still supported by the UE. */
                      ARFCN_T scell_arfcn = rr_gprs_get_scell_arfcn();

                      if (rr_gapi_is_supported_band_dyn((scell_arfcn.band)))
                      {
                        MSG_GERAN_HIGH_0_G("SGLTE: serving cell band ok, no reselection necessary");
                        rr_gprs_update_l1_params();
                      }
                      else
                      {
                        byte  mm_message_id;

                        MSG_GERAN_HIGH_1_G("SGLTE: serving cell:%d band now barred, force reselection",scell_arfcn.num);
                        rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_SGLTE_BAND_BARRED);

#ifdef FEATURE_GERAN_CS_SUPPORT
                        if ( grr_examine_saved_mm_message(&mm_message_id) )
                        {
                          if ( mm_message_id == RR_EST_REQ)
                          {
                             MSG_GERAN_HIGH_0_G("SGLTE band coex delete saved RR_EST_REQ for forced resel to B8 cell");
                             rr_send_abort_ind(RR_ABORT_CON_EST_NOT_ATTEMPTED, MM_RR_OTHER_FAILURE);
                             grr_throwaway_saved_message();
                          }
                        }
#endif /* FEATURE_GERAN_CS_SUPPORT */
                        
                        (void) rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, NULL);

                        /* action reselection in the reselection handler */
                        rr_resel_send_imsg_required_ind(RR_RESEL_CAUSE_RESELECTION);
                      }
                      
                      break;
                    } /* RR_SGLTE_BAND_COEX_OMSG_SUPPORTED_BANDS_UPDATED_IND */

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                } /* RR_SGLTE_BAND_COEX_OMSG */
#endif /* FEATURE_SGLTE */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }
              break;
            } /* MS_RR_RR */

            case MS_RR_L1:
            {
              switch (message_id)
              {
                case MPH_NC_MEASUREMENT_IND:
                case MPH_SERVING_MEAS_IND:
                case MPH_SERVING_IDLE_MEAS_IND:
#ifdef FEATURE_WCDMA
                case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
                case MPH_SURROUND_WCDMA_XFER_MEAS_IND:
#endif /* FEATURE_WCDMA */
#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
                case MPH_SURROUND_IRAT_MEAS_IND:
#endif /* FEATURE_LTE || FEATURE_GSM_TDS */
                {
                  (void) rr_gprs_nc_measurement_control(EV_INPUT_MESSAGE, new_message);

                  new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                  break;
                } /* MPH_NC_MEASUREMENT_IND ... */

                case MPH_SURROUND_MEAS_IND:
                case MPH_SURROUND_UPDATE_FAILURE_IND:
                case MPH_SERVING_AUX_MEAS_IND:
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 
                case MPH_BSIC_CHANGE_IND:
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION */
                {
                  new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                  break;
                } /* MPH_SURROUND_MEAS_IND ... */

                case MPH_BLOCK_QUALITY_IND:
                {
                  mph_block_quality_ind_T *block_quality_ind = &new_message->mph_block_quality_ind;

                  new_grr_control_event = rr_gprs_check_dsc_in_idle(
                    block_quality_ind->block_quality,
                    block_quality_ind->l2_channel_type
                  );
                  break;
                } /* MPH_BLOCK_QUALITY_IND */

                case MPH_SELECT_SPECIFIC_BCCH_CNF:
                {
                  MSG_GERAN_MED_0_G("L1 in FIND_BCCH state");
                  break;
                }

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
                case MPH_DTM_CHANNEL_ASSIGN_CNF:
                {
                  new_grr_control_event = rr_gprs_edtm_control(EV_INPUT_MESSAGE, new_message);
                  break;
                } /* MPH_DTM_CHANNEL_ASSIGN_CNF */
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

                case MPH_RESELECTION_CNF:
                {
                  /* probable race condition - post the signal back to the GRR queue */
                  /* such that any outstanding internal signals are processed first  */
                  if (new_message->mph_reselection_cnf.state_changed == FALSE)
                  {
                    mph_reselection_cnf_T resel_cnf;

                    /* duplicate the incoming primitive */

					resel_cnf = new_message->mph_reselection_cnf;


                    /* and set the state_changed flag to indicate it should NOT be handled here again */
                    resel_cnf.state_changed = TRUE;

                    /* post the cloned message to the RR task queue */
                    (void)gs_send_message(GS_QUEUE_RR, (void *)&resel_cnf, FALSE);
                  }
                  else
                  {
                    MSG_GERAN_ERROR_0_G("Unexpected message MPH_RESELECTION_CNF(state_changed = TRUE) in state CAMPED");
                  }
                  break;
                }

#ifdef FEATURE_SGLTE
                case MPH_UE_MODE_CHANGE_CNF:
                {
                  /* No action necessary. */
                  break;
                } /* MPH_UE_MODE_CHANGE_CNF */
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
                case MPH_PROC_IND:
                {
                  MSG_GERAN_MED_0_G("DMMSUSREM:: Camped disabling ps access");
                  rr_ps_send_imsg_no_ps_access_req(FALSE, NO_DEL);
                  grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
                  MSG_GERAN_HIGH_0_G("DMMSUSREM Debug:: rr_perform_backup_for_scell is being called");
                  rr_perform_backup_for_scell();
                  break;
                }
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */
                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }
              break;
            } /* MS_RR_L1 */

            case MS_RR_L2:
            {
              switch (message_id)
              {
                case DL_UNIT_DATA_IND:
                {
                  new_grr_control_event = rr_connection_establishment_control(
                    EV_INPUT_MESSAGE,
                    new_message
                  );
                  break;
                }

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
                case DL_DATA_IND:
                {
                  new_grr_control_event = rr_gprs_edtm_control(
                    EV_INPUT_MESSAGE,
                    new_message
                  );
                  break;
                }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }
              break;
            } /* MS_RR_L2 */

            case MS_MAC_RR:
            {
              switch (message_id)
              {
                case MAC_GRR_ABNORMAL_RELEASE:
                {
                  switch (new_message->mac_grr_sig.mac_grr_abnormal_release.cause)
                  {
                    case RR_FULL_SYS_INFO_REQ:
                    {
                      MSG_GERAN_HIGH_0_G("Abnormal release with full sys info");

                      // Send request to block PS access
                      send_ps_imsg_no_ps_access_req(
                        FALSE    // suspend_gmm
                      );

                      // Wait in GRR_CAMPED_DISABLING_PS_ACCESS for the confirmation
                      grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;

                      // Post a EV_ABNORMAL_RELEASE_FULL_SI event, which is blocked from being processed
                      // in GRR_CAMPED_DISABLING_PS_ACCESS. This will be received when the state returns
                      // to GRR_CAMPED
                      rr_grr_send_imsg_rr_event_ind(
                        EV_ABNORMAL_RELEASE_FULL_SI
                      );
                      break;
                    }

                    case RR_PART_SYS_INFO_REQ:
                    {
                      MSG_GERAN_HIGH_0_G("Abnormal release with partial sys info");

                      // Send request to block PS access
                      send_ps_imsg_no_ps_access_req(
                        FALSE    // suspend_gmm
                      );

                      // Wait in GRR_CAMPED_DISABLING_PS_ACCESS for the confirmation
                      grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;

                      // Post a EV_ABNORMAL_RELEASE_FULL_SI event, which is blocked from being processed
                      // in GRR_CAMPED_DISABLING_PS_ACCESS. This will be received when the state returns
                      // to GRR_CAMPED
                      rr_grr_send_imsg_rr_event_ind(
                        EV_ABNORMAL_RELEASE_PART_SI
                      );
                      break;
                    }

                    case RR_RETURN_TO_IDLE:
                    {
                      break;
                    }

                    case RR_CELL_RESEL_REQ:
                    {
                      /* log the reselection trigger */
                      rr_log_set_reselection_trigger(
                        RR_RESELECTION_TRIGGER_N3102
                      );
                      new_grr_control_event = EV_FORCE_CELL_RESELECTION;
                      break;
                    }

                    case RR_CONTENTION_FAILED:
                    {
                      gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();

                      if (scell_info_ptr->gsm.cell_identity_valid_flag)
                      {
                        /*if MAC contention resolution fails flush cache to guard against using stale MA from SI's*/
                        MSG_GERAN_HIGH_0_G("Pkt transfer contention failed, flush cache");
#ifdef FEATURE_RR_SI_CACHE
                        rr_csi_flush_data(&(scell_info_ptr->gsm.location_area_identification), 
                                          &(scell_info_ptr->gsm.cell_identity),
                                          CSI_FLUSH_CONTENTION_FAILURE);
#endif
                      }
                      break; 
                    }

                    default:
                    {
                      MSG_GERAN_HIGH_0_G("Abnormal release cause INVALID !");
                      break;
                    }
                  }
                  break;
                } /* MAC_GRR_ABNORMAL_RELEASE */

                case MAC_GRR_MAC_STATUS:
                {
                  mac_grr_mac_status_t *mac_status = &new_message->mac_grr_sig.mac_grr_mac_status;

                  switch (mac_status->mac_status)
                  {
                    case MAC_TRANSFER:
                    {
                      (void) rr_gprs_nc_measurement_control(
                        EV_MAC_TRANSFER,
                        new_message
                      );
                      break;
                    }

                    case MAC_TRANSFER_BLOCKED:
                    {
                      /* No action necessary. */
                      break;
                    }

                    case MAC_IDLE:
                    {

#if defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION) || defined (FEATURE_GERAN_EXTENDED_DRX)
                       boolean l1_updated_flag = rr_gprs_is_l1_updated_with_lowest_edrx();
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION || FEATURE_GERAN_EXTENDED_DRX*/

                      (void) rr_gprs_nc_measurement_control(
                        EV_MAC_IDLE,
                        new_message
                      );

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
                      (void) rr_gprs_edtm_control(
                        EV_MAC_IDLE,
                        NULL
                      );
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

                      new_grr_control_event = rr_resel_control(
                        EV_MAC_IDLE_MODE,
                        new_message
                      );

#ifndef FEATURE_IDLE_DRX_SCALING
                      /* Initialise the DSC on leaving transfer - 05.08 - 6.5 */
                      rr_gprs_initialise_dsc();
#endif /* !FEATURE_IDLE_DRX_SCALING */

#if defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION) || defined (FEATURE_GERAN_EXTENDED_DRX)
                      if (l1_updated_flag)
                      {
                        rr_gprs_set_l1_updated_with_lowest_edrx();
                      }
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION || FEATURE_GERAN_EXTENDED_DRX*/
                      break;
                    }

                    case MAC_IDLE_BLOCKED:
                    {
#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
                      (void) rr_gprs_edtm_control(
                        EV_MAC_IDLE_BLOCKED,
                        NULL
                      );
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
                      break;
                    }

                    default:
                    {
                      MSG_GERAN_LOW_1_G("Unexpected mac_status: %d", mac_status->mac_status);
                    }
                  }
                  break;
                } /* MAC_GRR_MAC_STATUS */

                case MAC_GRR_DATA_IND:
                {
                  mac_grr_data_ind_t *data_ind = &new_message->mac_grr_sig.mac_grr_data_ind;

                  switch (data_ind->data_ptr.header_ptr->message_type)
                  {
                    case PACKET_CELL_CHANGE_ORDER:
                    {
                      rr_gprs_pcco_decode_params(
                        (packet_cell_change_order_t *) (data_ind->data_ptr.data_ptr)
                      );

                      (void) rr_gprs_nc_measurement_control(
                        EV_PCCO_MESSAGE,
                        NULL
                      );

                      /* Pass the mac_grr_data_ind message into cell reselection controller */
                      new_grr_control_event = rr_resel_control(
                        EV_INPUT_MESSAGE,
                        new_message
                      );
                      break;
                    }

                    case PACKET_MEASUREMENT_ORDER:
                    {
                      packet_measurement_order_t *pmo;
                      pmo = (packet_measurement_order_t*) data_ind->data_ptr.data_ptr;

                      new_grr_control_event = rr_gprs_pmo_decode_params(
                        pmo
                      );
                      break;
                    }

                    case PACKET_CELL_CHANGE_CONTINUE:
                    {
                      new_grr_control_event = rr_resel_control(
                        EV_INPUT_MESSAGE,
                        new_message
                      );
                      break;
                    }

#ifdef FEATURE_GPRS_PS_HANDOVER
                    case PACKET_PS_HANDOVER:
                    {
                      rr_event_T event = rr_gprs_process_psho_command(
                        new_message
                      );
                      if (event == EV_PSHO_STARTED)
                      {
                        grr_control_data_ptr->state = GRR_PSHO;
                      }
                      break;
                    }
#endif /* FEATURE_GPRS_PS_HANDOVER */

                    default:
                    {
                      /* No action necessary. */
                      break;
                    }
                  }
                  break;
                } /* MAC_GRR_DATA_IND */

                case MAC_GRR_CELL_RESEL_STATUS:
                {
                  (void) rr_gprs_nc_measurement_control(
                    EV_INPUT_MESSAGE,
                    new_message
                  );

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                  if (rr_x2g_cco_in_progress())
                  {
                    MSG_GERAN_HIGH_1_G("MAC_GRR_CELL_RESEL_STATUS(status => %d)",
                             new_message->mac_grr_sig.mac_grr_cell_resel_status.status);

                    if (new_message->mac_grr_sig.mac_grr_cell_resel_status.status == PCCO_SUCCESS)
                    {
                      rr_grr_send_omsg_x2g_cco_status_ind(
                        RR_CCO_STATUS_SUCCESS
                      );
                    }
                    else
                    {
                      /* Need to indicate to rr_control() that the X2G CCO procedure has
                      failed due to timer expiration.  This will then cause rr_control()
                      to shut down L1 and return control to the initiating RAT. */
                      grr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_CONTENTION_RESOLUTION_FAILURE;

                      // Send request to block PS access (which may already be blocked)
                      // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
                      send_ps_imsg_no_ps_access_req(
                        FALSE   // suspend_gmm
                      );

                      grr_control_data_ptr->state = GRR_X2G_CCO_FAILED;
                    }
                  }
                  else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                  {
                    new_grr_control_event = rr_resel_control(
                      EV_INPUT_MESSAGE,
                      new_message
                    );
                  }
                  break;
                } /* MAC_GRR_CELL_RESEL_STATUS */

                case MAC_GRR_PCCF_SENT:
                {
                  new_grr_control_event = rr_resel_control(
                    EV_INPUT_MESSAGE,
                    new_message
                  );
                  break;
                } /* MAC_GRR_PCCF_SENT */

                case MAC_GRR_MEAS_RPT_CNF:
                {
                  new_grr_control_event = rr_gprs_nc_measurement_control(
                    EV_INPUT_MESSAGE,
                    new_message
                  );
                  break;
                } /* MAC_GRR_MEAS_RPT_CNF */

                case MAC_GRR_UL_TBF_REQ:
                {
                  mac_grr_ul_tbf_req_t *ul_tbf_req = &new_message->mac_grr_sig.mac_grr_ul_tbf_req;

                  if(rr_save_msg(new_message))
                  {
                    rr_resel_abort_reason_e abort_reason;
                    if (ul_tbf_req->channel_req_info.pkt_est_cause == RR_PKT_EST_CAUSE_PAGE_RESP)
                    {
                      abort_reason = RR_RESEL_ABORT_REASON_PAGE_RESPONSE;
                    }
                    else
                    {
                      abort_reason = RR_RESEL_ABORT_REASON_MAC_UL_TBF_EST;
                    }
                    rr_resel_send_imsg_abort_req(
                      abort_reason
                    );
                  }
                  break;
                } /* MAC_GRR_UL_TBF_REQ */

                case MAC_GRR_RACH_CNF:
                case MAC_GRR_RACH_REJ:
                {
                  /********************************************************************/
                  /* MAC->GRR: MAC_GRR_RACH_CNF                                       */
                  /* MAC->GRR: MAC_GRR_RACH_REJ                                       */
                  /* MAC is confirming/rejecting the request to RACH for a UL TBF,    */
                  /* but this procedure has already been abandoned for some reason,   */
                  /* and MAC had been sent a suspension request directly following    */
                  /* the original RACH confirmation request. Race condition           */
                  /********************************************************************/
                  break;
                } /* MAC_GRR_RACH_CNF ... */

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
                case MAC_GRR_DTM_CHANNEL_ASSIGN_REJ:
                {
                  new_grr_control_event = rr_gprs_edtm_control(
                    EV_INPUT_MESSAGE,
                    new_message
                  );
                  break;
                }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }
              break;
            } /* MS_MAC_RR */

            case MS_MM_RR:
            {
              switch (message_id)
              {
                case RR_GMM_GPRS_STATE_CHANGE_REQ:
                {
                  new_grr_control_event = rr_gprs_nc_measurement_control(
                    EV_INPUT_MESSAGE,
                    new_message
                  );
                  break;
                } /* RR_GMM_GPRS_STATE_CHANGE_REQ */

#ifdef FEATURE_GERAN_CS_SUPPORT
                case RR_EST_REQ:
                {
                  rr_est_req_T * rr_est_req_message = &new_message->rr_est_req;
                  if (rr_est_req_message->establishment_cause == (byte)MM_RR_ANSWER_TO_PAGING)
                  {
                    rr_resel_send_imsg_abort_req(
                      RR_RESEL_ABORT_REASON_PAGE_RESPONSE
                    );
                  }
                  else
                  {
                    rr_resel_send_imsg_abort_req(
                      RR_RESEL_ABORT_REASON_MAC_RR_EST
                    );
                  }
                  break;
                } /* RR_EST_REQ */
#endif /* FEATURE_GERAN_CS_SUPPORT */

                case RR_STOP_GSM_MODE_REQ:
                case RR_DEACT_REQ:
                {
                  /* Save this message in case a panic reset occurs during suspension */
                  rr_save_msg_for_recovery_after_panic_reset(
                    new_message
                  );

                  rr_resel_send_imsg_abort_req(
                    RR_RESEL_ABORT_REASON_MM_REQ
                  );
                  break;
                } /* RR_STOP_GSM_MODE_REQ ... */

                case RR_PLMN_LIST_REQ:
                {
                  new_grr_control_event = handle_rr_plmn_list_req(
                    &new_message->rr_plmn_list_req
                  );
                  break;
                } /* RR_PLMN_LIST_REQ */

                case RR_PLMN_SELECT_REQ:
                {
                  new_grr_control_event = handle_rr_plmn_select_req(
                    &new_message->rr_plmn_select_req
                  );
                  break;
                } /* RR_PLMN_SELECT_REQ */

                /* GMM indicates authentication and ciphering failure to RR */
                case RR_GMM_GPRS_AUTH_FAIL_IND:
                {
                  uint32 barred_time_secs;
                  BSIC_T      local_bsic;
                  gprs_scell_info_t  *scell_info_ptr = rr_gprs_get_scell_info();
                  rr_gmm_gprs_auth_fail_ind_T *rr_gmm_gprs_auth_fail_ind;
                  rr_gmm_gprs_auth_fail_ind = (rr_gmm_gprs_auth_fail_ind_T *)new_message;

                  CONVERT_TO_BSIC_T(local_bsic, rr_gmm_gprs_auth_fail_ind->bsic);

                  /*
                  Authentication and ciphering has failed. The serving cell should be
                  barred for the period indicated by T3302 minus 60 seconds.
                  If no T3302 value was included in previous ATTACH ACCEPT / RAU ACCEPT
                  messages, then a default of 12 hours is used (see 3GPP 43.022 3.5.5)
                  */
                  barred_time_secs = grr_auth_failed_calculate_timeout(new_message);

                  if (barred_time_secs > 0)
                  {
                    rr_authentication_failed_bar_cell(
                      CONVERT_SECONDS_TO_MILLISECONDS(barred_time_secs)
                    );

                    if ((ARFCNS_EQUAL(rr_gmm_gprs_auth_fail_ind->arfcn, scell_info_ptr->gsm.BCCH_ARFCN)) &&
                        (SAME_BSIC(local_bsic, scell_info_ptr->gsm.BSIC)))
                    {
                      /* Should start cell reselection. Spec ref: 43.022 section 3.5.5*/

                      /* log the reselection trigger */
                      rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_SERVING_BARRED);

                      /* notify measurement reporting of the DSF */
                      (void)rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, new_message);

                      /* action reselection in the reselection handler */
                      rr_resel_send_imsg_required_ind(RR_RESEL_CAUSE_RESELECTION);
                    }
                    else
                    {
                      MSG_GERAN_HIGH_6_G("ARFCN/BSIC mismatch Received ARFCN num,band=(%d %d) and serving ARFCN num,band=(%d %d) and Received BSIC= %d Serving BSIC= %d", 
                        rr_gmm_gprs_auth_fail_ind->arfcn.num,
                        rr_gmm_gprs_auth_fail_ind->arfcn.band,
                        scell_info_ptr->gsm.BCCH_ARFCN.num,
                        scell_info_ptr->gsm.BCCH_ARFCN.band,
                        rr_gmm_gprs_auth_fail_ind->bsic,
                        CONVERT_BSIC_TO_BYTE(scell_info_ptr->gsm.BSIC));

                      rr_authn_fail_bar_cell_basedon_ARFCN_BSIC(
                        CONVERT_SECONDS_TO_MILLISECONDS(barred_time_secs),
                        rr_gmm_gprs_auth_fail_ind->arfcn,
                        rr_gmm_gprs_auth_fail_ind->bsic
                        );
                    }
                  }
                  else
                  {
                    MSG_GERAN_HIGH_1_G("T3302 only %d secs - not long enough to bar cell", barred_time_secs);
                  }
                  break;
                } /* RR_GMM_GPRS_AUTH_FAIL_IND */

                case RR_CHANGE_MODE_IND:
                {

#ifdef FEATURE_SGLTE
                  if (rr_is_sglte_mode_active())
                  {
                    rr_grr_handle_rr_change_mode_ind(&new_message->mm.rr_change_mode_ind);
                  }
                  else
#endif /* FEATURE_SGLTE */
                  {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                    boolean old_gsm_only = rr_gsm_only();
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#ifdef FEATURE_WCDMA
                    boolean old_wcdma_enabled = rr_is_wcdma_enabled();
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
                    boolean old_lte_enabled = rr_is_lte_enabled();
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
                    boolean old_utran_tdd_enabled = rr_is_utran_tdd_enabled();
#endif /* FEATURE_GSM_TDS */


                  rr_pending_mode_change_data_store(&new_message->mm.rr_change_mode_ind);
                  rr_pending_mode_change_data_action();
                  rr_pending_mode_change_data_clear();

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
                  if (rr_gsm_only() != old_gsm_only)
                  {
                    /* Indicate the changed mode to L1 */
                    rr_send_mph_ue_mode_change_req(rr_gsm_only());
                  }

#ifdef FEATURE_WCDMA
                  if (rr_is_wcdma_enabled() != old_wcdma_enabled)
                  {
                    rr_g2w_wcdma_measurements_delete();
                  }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
                  if (rr_is_lte_enabled() != old_lte_enabled)
                  {
                    rr_g2w_lte_measurements_delete();
                  }
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
                  if (rr_is_utran_tdd_enabled() != old_utran_tdd_enabled)
                  {
                    rr_g2w_utran_tdd_measurements_delete();
                  }
#endif /* FEATURE_GSM_TDS */

                  /* Update L1 with the current IRAT neighbour list - this will take in
                  to account any changes in supported RATs or bands. */
                  rr_l1_update_wcdma_neighbor_list();

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
                  } 
                  break;
                } /* RR_CHANGE_MODE_IND */
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q                
                case RR_PSEUDO_LTE_LIST_UPDATE_IND:
                {
                  grr_handle_pseduo_lte_list_update_ind(&new_message->mm.rr_pseudo_lte_list_update_ind );
                  break;
                }      
#endif                
                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }
              break;
            } /* MS_MM_RR */

            case MS_TIMER:
            {
              switch (message_id)
              {
                case MID_TIMER_EXPIRY:
                {
                  new_grr_control_event = rr_gprs_camped_timer_handler(new_message);
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }

              break;
            } /* MS_TIMER */

#ifdef FEATURE_SGLTE
            case MS_MSGR_RR:
            {
              switch (new_message->rr_msgr_msg.message.hdr.id)
              {
#ifdef FEATURE_LTE
                case GERAN_GRR_LTE_PLMN_SRCH_REQ:
                {
                  if (rr_is_sglte_mode_active())
                  {
                    new_grr_control_event = handle_geran_grr_lte_plmn_srch_req(
                      &new_message->rr_msgr_msg.message.geran_grr_msg_req.plmn_srch_req
                    );
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message, "GRR_CAMPED");
                  }
                  break;
                } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */
#endif /* FEATURE_LTE */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message, "GRR_CAMPED");
                }
              }
              break;
            } /* MS_MSGR_RR */

#ifdef FEATURE_GSM_TDS
            case MS_TDSRRC_RR:
            {
              switch (new_message->message_header.message_id)
              {
                case TDSRR_INTERRAT_RESELECTION_REQ:
                {
                  if (rr_is_sglte_mode_active())
                  {
                    if (rr_sglte_is_full_service_available())
                    {
                      grr_control_data_ptr->sglte_pending_rat_pri_list =
                        ((tdsrr_interrat_reselect_req_type *) new_message)->rat_pri_list_info;

                      rr_send_activation_ind(MM_AS_SGLTE_INTER_RAT_RESELECTION);
                      grr_control_data_ptr->state = GRR_SGLTE_X2G_RESEL;
                    }
                    else
                    {
                      rr_sglte_reject_tds_resel_req();
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message, "GRR_CAMPED");
                  }
                  break;
                } /* TDSRR_INTERRAT_RESELECTION_REQ */

                case TDSRR_INTERRAT_REDIRECT_REQ:
                {
                  if (rr_is_sglte_mode_active())
                  {
                    if (rr_sglte_is_full_service_available())
                    {
                      grr_control_data_ptr->sglte_pending_rat_pri_list =
                        ((tdsrr_interrat_redirect_req_type *) new_message)->rat_pri_list_info;

                      rr_send_activation_ind(MM_AS_SGLTE_INTER_RAT_REDIRECTION);
                      grr_control_data_ptr->state = GRR_SGLTE_X2G_RESEL;
                    }
                    else
                    {
                      rr_sglte_reject_tds_redir_req();
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message, "GRR_CAMPED");
                  }
                  break;
                } /* TDSRR_INTERRAT_REDIRECT_REQ */

                case TDSRR_INTERRAT_PLMN_SRCH_REQ:
                {
                  if (rr_is_sglte_mode_active())
                  {
                    new_grr_control_event = handle_tdsrr_interrat_plmn_srch_req(
                      &new_message->tdsrr_plmn_srch_req
                    );
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message, "GRR_CAMPED");
                  }
                  break;
                } /* TDSRR_INTERRAT_PLMN_SRCH_REQ */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message, "GRR_CAMPED");
                }
              }
              break;
            } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_SGLTE */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                new_message,
                get_grr_control_state_name(grr_control_data_ptr->state)
              );
            }
          }
        } /* EV_INPUT_MESSAGE */


        /***************************************************************/
        /* EVENT handlers                                              */
        /***************************************************************/
        /***************************************/
        /* Handling of cell reselection events */
        /***************************************/
        else if (grr_control_event == RR_EV_START_RESELECTION_POWER_SCAN)
        {
          rr_resel_send_imsg_required_ind(RR_RESEL_CAUSE_RPS);
        }

        else if (grr_control_event == EV_FORCE_CELL_RESELECTION)
        {
          rr_resel_send_imsg_required_ind(RR_RESEL_CAUSE_RESELECTION);
        } /* EV_FORCE_CELL_RESELECTION */

        else if (grr_control_event == EV_T3174_TIMEOUT)
        {
#ifdef FEATURE_GERAN_CS_SUPPORT
          if (grr_verify_saved_mm_message(RR_EST_REQ))
          {
            grr_throwaway_saved_message();

            rr_send_abort_ind(RR_CONNECTION_FAILURE, MM_RR_OTHER_FAILURE);
          }
#endif /* FEATURE_GERAN_CS_SUPPORT */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          if (rr_x2g_cco_in_progress())
          {

#ifdef FEATURE_SGLTE
            if (rr_is_sglte_mode_active())
            {
              /* First, switch L1 to GSM-only mode.  Once this is done
              then remove PS support from service domain. */
              rr_sglte_set_rat_priority_list_gsm_only();

              /* Need to deactive PS services, switch GL1 to GSM-only mode and
              then send a CCO failure indication to the originating RAT. */
              rr_grr_send_imsg_switch_to_cs_only_gsm_only_req(
                rr_sglte_x2g_cco_failed_cb_fn
              );
            }
            else
#endif /* FEATURE_SGLTE */
            {
              /* Need to indicate to rr_control() that the X2G CCO procedure has
              failed due to timer expiration.  This will then cause rr_control()
              to shut down L1 and return control to the initiating RAT. */
              grr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY;

              // Send request to block PS access (which may already be blocked)
              // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
              send_ps_imsg_no_ps_access_req(
                FALSE   // suspend_gmm
              );

              grr_control_data_ptr->state = GRR_X2G_CCO_FAILED;
            } 
          } 
          else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
          {
            /* If a PCCO is in progress and RR is in GRR_CAMPED state then
            there are 2 possibilities - either the AS is idle and waiting for
            some NAS request, or a CS connection establishment has just been
            started and RR is in the process of suspending MAC.  If RR is in
            the process of suspending MAC for a CS connection, the MAC
            suspension state machine will have been set up to return
            EV_PROCESS_SAVED_MESSAGE, but passing EV_T3174_TIMEOUT to
            rr_resel_control() will result in a call to
            grr_suspend_gprs() which will override that and cause it to
            return EV_MAC_IDLE_MODE when MAC is suspended. */
            new_grr_control_event = rr_resel_control(grr_control_event, NULL);
          }

          /* Handle saved messages - particularly RR_EST_REQ */
          if (new_grr_control_event == EV_NO_EVENT)
          {
            new_grr_control_event = process_saved_message(
              (rr_cmd_bdy_type **)&new_message,
              &message_set,
              &message_id
            );

            if (new_grr_control_event != EV_NO_EVENT)
            {
              MSG_GERAN_HIGH_1_G("Process saved msg returns %d", new_grr_control_event);
            }
          }
        } /* EV_T3174_TIMEOUT */

        else if (grr_control_event == EV_CELL_RESELECTION_DEACT ||
                 grr_control_event == EV_CELL_RESELECTION_ACT ||
                 grr_control_event == EV_T3176_TIMEOUT)
        {
          /* If a PCCO is in progress and RR is in GRR_CAMPED state then
          there are 2 possibilities - either the AS is idle and waiting for
          some NAS request, or a CS connection establishment has just been
          started and RR is in the process of suspending MAC.  If RR is in
          the process of suspending MAC for a CS connection, the MAC
          suspension state machine will have been set up to return
          EV_PROCESS_SAVED_MESSAGE, but passing EV_T3174_TIMEOUT to
          rr_resel_control() will result in a call to
          grr_suspend_gprs() which will override that and cause it to
          return EV_MAC_IDLE_MODE when MAC is suspended. */

          /* pass the event into the reselection handler */
          new_grr_control_event = rr_resel_control(grr_control_event, NULL);

          /* Handle saved messages - particularly RR_EST_REQ */
          if (new_grr_control_event == EV_NO_EVENT)
          {
            // Note: Only check for saved MM messages here (see CR 394234)
            if (grr_examine_saved_mm_message(NULL))
            {
              new_grr_control_event = process_saved_message(
                (rr_cmd_bdy_type **)&new_message,
                &message_set,
                &message_id
              );
            }
          }
        }

        else if (grr_control_event == RR_EV_W2G_GO_TO_INACTIVE)
        {
          grr_control_data_ptr->state = GRR_NULL;
        }

        /***************************************************/
        /* Handling of DSF event. This needs to be passed  */
        /* into both the measurement reporting handler and */
        /* the cell reselection controller                 */
        /***************************************************/
        else if ((grr_control_event == EV_DSF_RESELECTION) || (grr_control_event == EV_DSF_FOR_DENIALS))
        {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          if (rr_x2g_cco_in_progress())
          {
            /* Need to indicate to rr_control() that the X2G CCO procedure has
            failed due to timer expiration.  This will then cause rr_control()
            to shut down L1 and return control to the initiating RAT. */

            grr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_DSF;

            // Send request to block PS access (which may already be blocked)
            // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
            send_ps_imsg_no_ps_access_req(
              FALSE   // suspend_gmm
            );

            grr_control_data_ptr->state = GRR_X2G_CCO_FAILED;
          }
          else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
          {
            /* log the reselection trigger */
            rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_DSF);
            /* notify measurement reporting of the DSF */
            (void)rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, new_message
            );

            /* action reselection in the reselection handler */
            rr_resel_send_imsg_required_ind(RR_RESEL_CAUSE_RESELECTION);
          }
        }
        /****************************************************/
        /* Cell Reselection has been actioned, change state */
        /* and notify any other handlers                    */
        /****************************************************/
        else if (grr_control_event == EV_CELL_RESELECTION)
        {
          /* notify measurement reporting of the cell reselection */
          (void)rr_gprs_nc_measurement_control(RR_EV_DEACTIVATE_MEASUREMENTS, new_message);

          /* Inform (G)MM that a reselection is in progress */
          rr_send_rr_reselection_ind();

          grr_control_data_ptr->state = GRR_CELL_RESELECTION;
        }
        /***************************************************************/
        /* OUTPUT EVENT handlers                                       */
        /* These events cause a state transition in GRR51 and must also*/
        /* be output to the calling function to be passed into another */
        /* state machine                                               */
        /***************************************************************/

        /**********************************************************/
        /* events for the measurement reporting are handled here  */
        /**********************************************************/
        else if (grr_control_event == RR_EV_MEASUREMENT_PARAMS_UPDATED ||
                 grr_control_event == RR_EV_DEACTIVATE_MEASUREMENTS )
        {
          /*Pass the events into the measurement handler*/
          new_grr_control_event = rr_gprs_nc_measurement_control(grr_control_event, NULL);
        }

#ifdef FEATURE_GERAN_CS_SUPPORT
        /********************************************************************/
        /* EV_RR_EST_REQ                                                    */
        /* A CS call needs to be established (either an MO call or a MT call*/
        /* in response to a page). Suspend GPRS, and return to RR           */
        /********************************************************************/
        else if( grr_control_event == EV_RR_EST_REQ )
        {
          // Send request to block PS access (which may already be blocked)
          // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
          // GMM is not suspended until the connection establishment is
          // completes successfully.
          send_ps_imsg_no_ps_access_req(
            FALSE    // suspend_gmm
          );

          grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
        }
#endif /* FEATURE_GERAN_CS_SUPPORT */

        /******************************************************************/
        /* EV_GRR_EST_REQ                                                 */
        /* A GPRS connection establishment is taking place                */
        /******************************************************************/
        else if ( grr_control_event == EV_GRR_EST_REQ )
        {
          grr_control_data_ptr->state = GRR_CONNECTION_PENDING;

#ifdef FEATURE_GERAN_BSR_PS_STATUS
          // Update NAS with 'PS active' indication
          rr_mm_send_rr_ps_status_change_ind(
            TRUE    // ps_is_active
          );
#endif
        }

        /******************************************************************/
        /* RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED                             */
        /* The L1 idle mode needs to be changed. Re-camp on the cell in   */
        /* the appropriate mode                                           */
        /******************************************************************/
        else if ( grr_control_event == RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED )
        {
          // After the L1 idle mode is changed, a RR_SERVICE_IND needs to be sent to MM
          grr_control_data_ptr->notify_mm_service_ind = TRUE;

          rr_l1_idle_send_imsg_change_idle_req();

          grr_control_data_ptr->state = GRR_STARTING_L1_IDLE;
        }

        /******************************************************************/
        /* EV_GPRS_SUSPEND_FOR_SI_ACQUISITION                             */
        /* Suspend GPRS services in preparation for an acquisition of sys */
        /* info                                                           */
        /******************************************************************/
        else if ( grr_control_event == EV_GPRS_SUSPEND_FOR_SI_ACQUISITION )
        {
          // Send request to block PS access
          send_ps_imsg_no_ps_access_req(
            FALSE
          );

          // Send EV_GPRS_SUSPENDED_FOR_SYS_INFO_ACQUISITION, which is blocked in
          // GRR_CAMPED_DISABLING_PS_ACCESS.
          // This will be received in GRR_CAMPED once PS access is blocked
          rr_grr_send_imsg_rr_event_ind(EV_GPRS_SUSPENDED_FOR_SI_ACQUISITION);

          // Wait in GRR_CAMPED_DISABLING_PS_ACCESS for PS access to be blocked
          grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
        }

        /******************************************************************/
        /* EV_GPRS_SUSPEND_FOR_SI_ACQUISITION_NO_SI13                     */
        /* Suspend GPRS services in preparation for an acquisition of sys */
        /* info                                                           */
        /******************************************************************/
        else if ( grr_control_event == EV_GPRS_SUSPEND_FOR_SI_ACQUISITION_NO_SI13 )
        {
          rr_event_T resel_event;

          // Stop & reset NC measurement reporting (if active)
          resel_event = rr_gprs_nc_measurement_control(RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13, NULL);

          (void)rr_resel_control(resel_event, NULL);

          // Send request to block PS access
          send_ps_imsg_no_ps_access_req(
            FALSE
          );

          // Send EV_GPRS_SUSPENDED_FOR_SYS_INFO_ACQUISITION, which is blocked in
          // GRR_CAMPED_DISABLING_PS_ACCESS.
          // This will be received in GRR_CAMPED once PS access is blocked
          rr_grr_send_imsg_rr_event_ind(EV_GPRS_SUSPENDED_FOR_SI_ACQUISITION);

          // Wait in GRR_CAMPED_DISABLING_PS_ACCESS for PS access to be blocked
          grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
        }

        /***********************************************************************/
        /* EV_RR_GPRS_ACQUISITION_FAILURE                                      */
        /* The acquisition of SIs timed-out - force a reselection              */
        /***********************************************************************/
        else if ( grr_control_event == RR_EV_ACQUISITION_FAILURE )
        {
          /* Go out of service, this should result in an RR_PLMN_SELECT_CNF being
          sent if necessary. */
          new_grr_control_event = EV_FORCE_CELL_RESELECTION;
        }

        /******************************************************************/
        /* EV_GPRS_SUSPENDED_FOR_SI_ACQUISITION                           */
        /* GPRS has been suspended, inform the SI acquisition handler     */
        /******************************************************************/
        else if ( grr_control_event == EV_GPRS_SUSPENDED_FOR_SI_ACQUISITION )
        {
          new_grr_control_event =
            rr_gprs_bsi_acquisition_control(EV_GPRS_SUSPENDED_FOR_SI_ACQUISITION, NULL);
        }

        /******************************************************************/
        /* EV_ABNORMAL_RELEASE_PART_SI                                    */
        /* Perform a refresh of SIs or PSIs as appropriate                */
        /* EV_ABNORMAL_RELEASE_FULL_SI                                    */
        /* Perform a full sys info acquisition of SIs and then if PBCCH   */
        /* present, PSIs                                                  */
        /******************************************************************/
		else if ( grr_control_event == EV_ABNORMAL_RELEASE_PART_SI )
        {
          (void) bsi_acquisition_control(SIEV_SYS_INFO_PART_REFRESH, NULL);         
        }

        else if ( grr_control_event == EV_ABNORMAL_RELEASE_FULL_SI )
        {
          (void) bsi_acquisition_control(SIEV_SYS_INFO_FULL_REFRESH, NULL);
        }

        /******************************************************************/
        /* EV_NON_CAMPED_SYS_INFO_ACQUISITION                             */
        /* A full SI acquisition has been started, due to an abnormal     */
        /* release indicating full SI refresh.                            */
        /* Move to the GRR_ACQUIRE until the acquisition completes        */
        /******************************************************************/
        else if ( grr_control_event == EV_NON_CAMPED_SYS_INFO_ACQUISITION )
        {
          new_grr_control_event = EV_GPRS_SUSPENDED_FOR_SI_ACQUISITION;
          grr_control_data_ptr->state = GRR_ACQUIRE;
        }

        /******************************************************************/
        /* EV_GRR51_GO_TO_PACKET_IDLE                                     */
        /******************************************************************/
        else if ( grr_control_event == EV_GRR51_GO_TO_PACKET_IDLE )
        {
          ;  /* Nothing to do */
        }
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
        /******************************************************************/
        /* RR_EV_RESELECT_WCDMA_CELL                                      */
        /******************************************************************/
        else if (grr_control_event == RR_EV_RESELECT_WCDMA_CELL)
        {
          grr_control_data_ptr->state = GRR_CELL_RESELECTION_G2W;
        }
#endif
        /******************************************************************/
        /* EV_TRIGGER_CELL_SELECTION                                      */
        /* Return to RR and trigger a cell selection                      */
        /******************************************************************/
        else if ( grr_control_event == EV_TRIGGER_CELL_SELECTION )
        {
          // Send request to block PS access
          send_ps_imsg_no_ps_access_req(
            FALSE
          );

          // Send RR_EV_TRIGGER_CELL_SELECTION, which is blocked in GRR_CAMPED_DISABLING_PS_ACCESS
          // This will be received in GRR_CAMPED once PS access is blocked
          rr_grr_send_imsg_rr_event_ind(RR_EV_TRIGGER_CELL_SELECTION);

          // Wait in GRR_CAMPED_DISABLING_PS_ACCESS for PS access to be blocked
          grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
        }

        /******************************************************************/
        /* EV_NO_BCCH_FOUND                                               */
        /*   Return to RR to process a saved message.                     */
        /* RR_EV_TRIGGER_CELL_SELECTION                                   */
        /*   Return to RR and trigger a cell selection                    */
        /* EV_PROCESS_SAVED_MESSAGE                                       */
        /*   Return to RR to process a saved message.                     */
        /******************************************************************/
        else if ( ( grr_control_event == EV_NO_BCCH_FOUND ) ||
                  ( grr_control_event == RR_EV_TRIGGER_CELL_SELECTION ) ||
                  ( grr_control_event == EV_PROCESS_SAVED_MESSAGE ) )
        {
          grr_control_data_ptr->state = GRR_NULL;

          output_event = relinquish_control_to_rr(grr_control_event);
        }

        /******************************************************************/
        /* RR_EV_CAMPED_ON_CELL                                           */
        /* When already in GRR CAMPED state, this event would normally    */
        /* only be received after a service domain change                 */
        /******************************************************************/
        else if (grr_control_event == RR_EV_CAMPED_ON_CELL)
        {
          // Although already in GRR_CAMPED, use this function to check for saved messages
          // and/or enable/disable PS access
          grr_control_data_ptr->state = transition_to_camped_state(
            FALSE,                               // service_ind_sent
            &new_grr_control_event,
            (rr_cmd_bdy_type **) &new_message,
            &message_set,
            &message_id
          );
        }

        else if (grr_control_event == EV_PMO_MESSAGE)
        {
          new_grr_control_event = rr_gprs_nc_measurement_control(EV_PMO_MESSAGE, NULL);
        }
        else if (grr_control_event == EV_ALL_NACC_PNCD_DISPATCHED)
        {
          new_grr_control_event = rr_resel_control(grr_control_event, NULL);
        }
        else if (grr_control_event == EV_CR_G2W_TIMER_EXPIRED)
        {
          new_grr_control_event = rr_resel_control(EV_CR_G2W_TIMER_EXPIRED, NULL);
        }
#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
        else if (grr_control_event == EV_EDTM_ESTABLISHMENT_COMPLETED)
        {
          grr_control_data_ptr->state = GRR_NULL;

          (void)rr_dtm_control(EV_EDTM_ESTABLISHMENT_COMPLETED, NULL);

          output_event = relinquish_control_to_rr(grr_control_event);
        }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

        else
        /********************************************************************/
        /* EV_MAC_GUARD_TIMER_EXPIRED                                       */
        /*   Failure detected - restart lower layers                        */
        /********************************************************************/
        if ( grr_control_event == EV_MAC_GUARD_TIMER_EXPIRED )
        {
          if (geran_get_nv_recovery_restart_enabled_dyn())
          {
            MSG_GERAN_ERROR_0_G("EV MAC GUARD TIMER EXPIRED,Triggering PANIC RESET");
            geran_initiate_recovery_restart_dyn(GERAN_CLIENT_GRR);
          }
          else
          {
            ERR_GERAN_FATAL_0_G("EV MAC GUARD TIMER EXPIRED");
          }
        }

        else
        /* Panic Reset triggered */
        if ( grr_control_event == EV_PANIC_RESET )
        {
          ;  /* No specific action required */
        }
        else if (grr_control_event == EV_GRR_PLMN_SEARCH_READY_TO_START)
        {
          MSG_GERAN_HIGH_0_G("GRR is ready for PLMN search (without doing anything)");
          rr_gprs_suspend_camped_sys_info_handling(FALSE);
          output_event = RR_EV_PROCESS_SAVED_MESSAGE;
        }

        else if (grr_control_event == RR_EV_CANDIDATE_CELL_SI_RECEIVED)
        {
          /* no action required */
        }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
        else if (grr_control_event == EV_X2G_CCO_ABORTED_FOR_HIGH_PRI_REQ)
        {
          rr_timer_cancel(T3174);
          grr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
          rr_grr_send_omsg_x2g_cco_status_ind(RR_CCO_STATUS_ABORTED_HIGH_PRIORITY_REQUEST);
          grr_control_data_ptr->state = GRR_X2G_CCO_ABORTED;
        }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

        else
        {
          LOG_UNEXPECTED_MESSAGE(
            new_message,
            get_grr_control_state_name(grr_control_data_ptr->state)
          );
        }

        break;
      }

      /*********************************************************************/
      /* GRR_CONNECTION_PENDING                                            */
      /*                                                                   */
      /* Handles the initial stages of MO & MT CS call establishment, and  */
      /* negotiation of a connection as requested by MAC                   */
      /*********************************************************************/

      case GRR_CONNECTION_PENDING:
      {
        if ( grr_control_event == EV_INPUT_MESSAGE )
        {
          if ( message_set == MS_RR_RR )
          {
            switch (new_message->rr.header.rr_message_set)
            {
              case RR_GRR_IMSG:
              {
                switch (new_message->rr.header.imh.message_id)
                {
                  case RR_GRR_IMSG_GPRS_IA_IND:
                  {
                    rr_grr_imsg_gprs_ia_ind_t *imsg = &new_message->rr.grr.imsg.gprs_ia_ind;

                    // Send the GPRS IA to MAC
                    grr_send_gprs_dl_ia_to_mac(
                      &imsg->packet_ch_desc,
                      &imsg->ia_rest_octets,
                      &imsg->frequency_list,
                      imsg->timing_advance
                    );
                    break;
                  }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
                  case RR_GRR_IMSG_MSC_CHANGE_IND:
                  {
                    new_grr_control_event = grr_conn_est_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL);

                    grr_control_data_ptr->state = GRR_CAMPED_MSC_CHANGE;
                    break;
                  }
#endif

                  case RR_GRR_IMSG_PS_ACCESS_REQ:
                  {
                    // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                    rr_grr_imsg_ps_access_req_t *imsg = &new_message->rr.grr.imsg.ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn);
                    break;
                  }

#ifdef FEATURE_GERAN_CS_SUPPORT
                  case RR_GRR_IMSG_PAGE_IND:
                  {
                    rr_grr_imsg_page_ind_t *imsg = &new_message->rr.grr.imsg.page_ind;

                    // A CS or PS page has been received for the MS
                    // Note: GRR_CONNECTION_PENDING state only relates to RACHing for TBF establishment

                    /*
                    3GPP 44.018 3.3.1.1.2 Initiation of the immediate assignment procedure
                    "The RR entity of the mobile station initiates the immediate assignment procedure by
                     scheduling the sending on the RACH and leaving idle mode (in particular, the mobile
                     station shall ignore PAGING REQUEST messages)"
                    */

                    // Our behaviour is to forward CS pages to NAS if RACHing for TBF establishment
                    if (imsg->page_info.domain == RR_PAGE_DOMAIN_CS)
                    {
                      rr_ce_send_page_to_nas(&imsg->page_info);
                    }
                    break;
                  }
#endif /* FEATURE_GERAN_CS_SUPPORT */

                  case RR_GRR_IMSG_PAGE_MODE_IND:
                  {
                    // A OTA message has been received which has changed the Page Mode
                    // e.g. PAGE REQUEST, IMMEDIATE ASSIGNMENT, IMMEDIATE ASSIGNMENT REJECT, etc

                    /*
                    3GPP 3.3.2.1.1 Paging initiation using paging subchannel on CCCH
                    "The mobile station in idle mode is required to receive and analyse the paging messages
                     and immediate assignment messages sent on the paging subchannel corresponding to its
                     paging subgroup, as specified in 3GPP TS 45.002"
                    */

                    // so changes in Page Mode are ignored while RACHing
                    MSG_GERAN_LOW_0_G("Page Mode change ignored");
                    break;
                  }

                  default:
                  {
                    new_grr_control_event = grr_conn_est_control(EV_INPUT_MESSAGE, new_message);
                  }
                }
                break;
              } /* RR_GRR_IMSG */

              default:
              {
                new_grr_control_event = grr_conn_est_control(EV_INPUT_MESSAGE, new_message);
              }
            }
          } /* MS_RR_RR */

          else
          /*******************************************************************/
          /* Some messages (such as serving cell measurements) can be safely */
          /* ignored during connection establishment                         */
          /*******************************************************************/
          if ( ( ( message_set == MS_RR_L1 ) &&
                 ( ( message_id == (byte)MPH_NC_MEASUREMENT_IND ) ||
                   ( message_id == (byte)MPH_SERVING_MEAS_IND )) )
               ||
               ( ( message_set == MS_MM_RR ) &&
                 ( message_id == (byte)RR_GMM_GPRS_STATE_CHANGE_REQ ) ) )
          {
            ;
          }
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
          else if (message_set == MS_RR_L1 && message_id == MPH_PROC_IND)
          {
            rr_perform_backup_for_scell();
            grr_control_data_ptr->tbf_est_abort_event = EV_GPRS_SUSPEND;
            new_grr_control_event = grr_conn_est_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL);
          }
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */

          else
          /***************************************************************/
          /* MM->RR: RR_STOP_GSM_MODE_REQ                                */
          /*         RR_DEACT_REQ                                        */
          /* Abort any CS or PS conn est in progress, suspend GPRS, and  */
          /* return to RR                                                */
          /***************************************************************/
          if ( ( message_set == MS_MM_RR ) &&
               ( ( message_id == (byte)RR_STOP_GSM_MODE_REQ ) ||
                 ( message_id == (byte)RR_DEACT_REQ ) ) )
          {
            /* Save this message in case a panic reset occurs during suspension */
            rr_save_msg_for_recovery_after_panic_reset(new_message);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
            if (rr_x2g_cco_in_progress())
            {
              grr_control_data_ptr->tbf_est_abort_event = EV_X2G_CCO_ABORTED_FOR_HIGH_PRI_REQ;

              new_grr_control_event = grr_conn_est_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL);
            }
            else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
            {
              new_grr_control_event = EV_PROCESS_SAVED_MESSAGE;
            }
          }

          else
          /***************************************************************/
          /* MM->RR: RR_PLMN_SELECT_REQ                                  */
          /* See if any action needed - may require aborting PS conn est */
          /* and suspending GPRS                                         */
          /***************************************************************/
          if ( ( message_set == MS_MM_RR ) &&
               ( message_id == (byte)RR_PLMN_SELECT_REQ ) )
          {
            rr_plmn_select_req_T  * rr_plmn_select_req = (rr_plmn_select_req_T *) new_message;

            new_grr_control_event = handle_rr_plmn_select_req(
              rr_plmn_select_req
            );
          }

          else
          /***************************************************************/
          /* MM->RR: RR_PLMN_LIST_REQ                                    */
          /* See if any action needed - may require aborting PS conn est */
          /* and suspending GPRS                                         */
          /***************************************************************/
          if ( ( message_set == MS_MM_RR ) &&
               ( message_id == (byte)RR_PLMN_LIST_REQ ) )
          {
            rr_plmn_list_req_T  * rr_plmn_list_req = (rr_plmn_list_req_T *) new_message;

            new_grr_control_event = handle_rr_plmn_list_req(
              rr_plmn_list_req
            );
          }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q    
          else
          /***************************************************************/
          /* MM->RR: RR_PSEUDO_LTE_LIST_UPDATE_IND                       */
          /* While GPRS Connection is Established GRR can modify the     */
          /* LTE PSEUDO LIST and send new one to GL1                     */
          /***************************************************************/
          if ( ( message_set == MS_MM_RR ) &&
               ( message_id == (byte)RR_PSEUDO_LTE_LIST_UPDATE_IND) )
          {
            rr_pseudo_lte_list_update_ind_T* rr_pseudo_list_req = (rr_pseudo_lte_list_update_ind_T *) new_message;
            grr_handle_pseduo_lte_list_update_ind(rr_pseudo_list_req );
          }
#endif
#ifdef FEATURE_GERAN_CS_SUPPORT
          else
          /*****************************************************************/
          /* MM->RR: RR_EST_REQ                                            */
          /* A request for CS establishment has been received, and must    */
          /* take priority over the current PS establishment               */
          /*****************************************************************/
          if ( ( message_set == MS_MM_RR ) &&
               ( message_id == (byte)RR_EST_REQ ) )
          {
            /* This will return EV_RR_EST_REQ if the CS call is allowed */
            new_grr_control_event = handle_rr_est_req(
              (rr_est_req_T *) new_message
            );
          }
#endif /* FEATURE_GERAN_CS_SUPPORT */

          else
          /**
           * MAC->RR: MAC_GRR_MEAS_RPT_CNF
           * Indicates if MAC was able to transmit a P(E)MR or not.
           * Measurement reporting state machine should be informed of this.
           */
          if ( ( message_set == MS_MAC_RR ) &&
               ( message_id == (byte)MAC_GRR_MEAS_RPT_CNF ) )
          {
            (void) rr_gprs_nc_measurement_control(EV_INPUT_MESSAGE, new_message);
          }

          else
          if ( ( message_set == MS_MAC_RR ) &&
               ( message_id == (byte)MAC_GRR_MAC_STATUS ) )
          {
            ;   // this is not needed during TBF establishment
          }


          else if ( ( message_set == MS_TIMER ) &&
                    ( message_id  == MID_TIMER_EXPIRY) &&
                    ( ((rr_cmd_bdy_type *) new_message)->mid_timer_expiry.timer_id == T3174 ) )
          {
            new_grr_control_event = grr_conn_est_control(EV_INPUT_MESSAGE, new_message);
          }

          else if ( ( message_set == MS_TIMER ) &&
                    ( ((rr_cmd_bdy_type *) new_message)->mid_timer_expiry.timer_id == T3122 ) )
          {
            // Indicate to NAS-MM that T3122 has expired via a RR_SERVICE_IND
            grr_update_mm_with_rr_service_ind();
          }

          else
          /*******************************************************************/
          /* Pass all other messages thru to the GPRS Conn Est state machine */
          /*******************************************************************/
          {
            new_grr_control_event = grr_conn_est_control(EV_INPUT_MESSAGE, new_message);
          }
        }

#ifdef FEATURE_GERAN_CS_SUPPORT
        else
        /******************************************************************/
        /* EV_RR_EST_REQ                                                  */
        /* Abort the current PS conn est, then process the event          */
        /* When the PS conn est has been abort, grr_conn_est_control()    */
        /* returns EV_GRR51_GO_TO_PACKET_IDLE                             */
        /******************************************************************/
        if ( grr_control_event == EV_RR_EST_REQ )
        {
          grr_control_data_ptr->tbf_est_abort_event = EV_PROCESS_SAVED_MESSAGE;

          new_grr_control_event = grr_conn_est_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL);
        }
#endif /* FEATURE_GERAN_CS_SUPPORT */

        else
        /******************************************************************/
        /* EV_PROCESS_SAVED_MESSAGE                                       */
        /* EV_GPRS_SUSPEND_FOR_SI_ACQUISITION                             */
        /* Abort the current PS conn est, then process the event          */
        /* When the PS conn est has been abort, grr_conn_est_control()    */
        /* returns EV_GRR51_GO_TO_PACKET_IDLE                             */
        /******************************************************************/
        if ( ( grr_control_event == EV_PROCESS_SAVED_MESSAGE ) ||
             ( grr_control_event == EV_GPRS_SUSPEND_FOR_SI_ACQUISITION_NO_SI13 ) )
        {
          grr_control_data_ptr->tbf_est_abort_event = grr_control_event;

          new_grr_control_event = grr_conn_est_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL);
        }

        else
        /******************************************************************/
        /* EV_GRR51_GO_TO_PACKET_IDLE                                     */
        /* Return to GRR_CAMPED                                           */
        /******************************************************************/
        if ( grr_control_event == EV_GRR51_GO_TO_PACKET_IDLE )
        {
          MSG_GERAN_HIGH_0_G("EV_GRR51_GO_TO_PACKET_IDLE");

          // TODO what is this for ?
          clear_waiting_for_surround_response();

          /* Start 30s sys info refresh cycle, and acquire any remaining SIs */
          rr_gprs_activate_camped_sys_info_handling();

          if (rr_mac_ul_tbf_est_is_allowed())
          {
            send_ps_imsg_ps_access_req(
              FALSE   // purge_gmm_signalling
            );

            grr_control_data_ptr->state = GRR_CAMPED_ENABLING_PS_ACCESS;
          }
          else
          {
            send_ps_imsg_no_ps_access_req(
              FALSE    // suspend_gmm
            );

            grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
          }

#if 0
          {
            /* Start 30s sys info refresh cycle, and acquire any remaining SIs */
            rr_gprs_activate_camped_sys_info_handling();

            grr_control_data_ptr->state = GRR_CAMPED;
          }
#endif
        }

        else
        /******************************************************************/
        /* EV_TBF_CONN_EST_ABORTED_CAMPED                                  */
        /* The TBF conn est has been aborted. RACHing had not started yet */
        /* so L1 is still in Packet Idle                                  */
        /******************************************************************/
        if ( grr_control_event == EV_TBF_CONN_EST_ABORTED_CAMPED )
        {
#ifdef FEATURE_GERAN_BSR_PS_STATUS
          // Update NAS with 'PS not active' indication
          rr_mm_send_rr_ps_status_change_ind(
            FALSE   // ps_is_active
          );
#endif

          if ( grr_control_data_ptr->tbf_est_abort_event == EV_PROCESS_SAVED_MESSAGE )
          {
            boolean suspend_gmm = FALSE;

            if (grr_verify_saved_mm_message(RR_PLMN_LIST_REQ))
            {
              suspend_gmm = TRUE;
            }
#ifdef FEATURE_GERAN_CS_SUPPORT
            else
            if (grr_verify_saved_mm_message(RR_EST_REQ))
            {
              suspend_gmm = TRUE;
            }
#endif /* FEATURE_GERAN_CS_SUPPORT */

            // Send request to disable PS access (it may be already)
            send_ps_imsg_no_ps_access_req(
              suspend_gmm   // suspend_gmm
            );

            // and wait in GRR_CAMPED_DISABLING_PS_ACCESS for the confirmation
            grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
          }
          else
          if ( grr_control_data_ptr->tbf_est_abort_event == EV_GPRS_SUSPEND_FOR_SI_ACQUISITION_NO_SI13 )
          {
            /*
            SI3 has been received during RACHing with gprs_indicator=0.
            PS conn est should be aborted all SI re-acquired
            */
            rr_event_T resel_event;

            // Stop & reset NC measurement reporting (if active)
            resel_event = rr_gprs_nc_measurement_control(RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13, NULL);

            (void) rr_resel_control(resel_event, NULL);

            send_ps_imsg_no_ps_access_req(
              FALSE   // suspend_gmm
            );

            grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;

            // Send RR_IMSG_RR_EVENT_IND, containing EV_GPRS_SUSPENDED_FOR_SYS_INFO_ACQUISITION
            // Handling for this is blocked in GRR_CAMPED_DISABLING_PS_ACCESS, so it will be received
            // when theh state returns to GRR_CAMPED
            rr_grr_send_imsg_rr_event_ind(EV_GPRS_SUSPENDED_FOR_SI_ACQUISITION);
          }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          else if (grr_control_data_ptr->tbf_est_abort_event == EV_X2G_CCO_ABORTED_FOR_HIGH_PRI_REQ)
          {
            rr_timer_cancel(T3174);
            grr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
            rr_grr_send_omsg_x2g_cco_status_ind(RR_CCO_STATUS_ABORTED_HIGH_PRIORITY_REQUEST);
            grr_control_data_ptr->state = GRR_X2G_CCO_ABORTED;
          }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
          else if (grr_control_data_ptr->tbf_est_abort_event == EV_GPRS_SUSPEND)
          {
            // Send request to disable PS access (it may be already)
            send_ps_imsg_no_ps_access_req(TRUE);

            // and wait in GRR_CAMPED_DISABLING_PS_ACCESS for the confirmation
            grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
          }
          else if ( grr_control_data_ptr->tbf_est_abort_event != EV_NO_EVENT )
          {
            MSG_GERAN_LOW_1_G("Unhandled tbf_est_abort_event (%d)",
                      grr_control_data_ptr->tbf_est_abort_event);
            grr_control_data_ptr->state = GRR_CAMPED;
          }
          else
          {
            // It is not an error if there is no tbf_est_abort_event - just return to Packet Idle
            grr_control_data_ptr->state = GRR_CAMPED;
          }

          grr_control_data_ptr->tbf_est_abort_event = EV_NO_EVENT;
        }

        else
        /******************************************************************/
        /* EV_TBF_CONN_EST_ABORTED_NON_CAMPED                             */
        /* The TBF conn est has been aborted. RACHing had started, so L1  */
        /* is no longer camped (i.e. in RACH state)                       */
        /******************************************************************/
        if ( grr_control_event == EV_TBF_CONN_EST_ABORTED_NON_CAMPED )
        {
#ifdef FEATURE_GERAN_BSR_PS_STATUS
          // Update NAS with 'PS not active' indication
          rr_mm_send_rr_ps_status_change_ind(
            FALSE   // ps_is_active
          );
#endif

          switch (grr_control_data_ptr->tbf_est_abort_event)
          {
            case EV_PROCESS_SAVED_MESSAGE:
            {
              boolean suspend_gmm = FALSE;

              /* Special handling of a RR_PLMN_LIST_REQ is required. Because L1 is currently */
              /* RACHing for a GPRS Imm Ass, it is not camped, so a background search cannot */
              /* be performed. So ensure RR performs a foreground search by marking RR as */
              /* not camped before actioning the search. A cell selection will be performed */
              /* after the PLMN search is complete */
              if (grr_verify_saved_mm_message(RR_PLMN_LIST_REQ))
              {
                gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();
                scell_info_ptr->gsm.camped_on_flag = FALSE;

                suspend_gmm = TRUE;
              }
#ifdef FEATURE_GERAN_CS_SUPPORT
              else
              if (grr_verify_saved_mm_message(RR_EST_REQ))
              {
                suspend_gmm = TRUE;
              }
#endif /* FEATURE_GERAN_CS_SUPPORT */

              // Send request to disable PS access (it may be already)
              send_ps_imsg_no_ps_access_req(
                suspend_gmm   // suspend_gmm
              );

              // and wait in GRR_CAMPED_DISABLING_PS_ACCESS for the confirmation
              grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
              break;
            }

            case EV_GPRS_SUSPEND_FOR_SI_ACQUISITION_NO_SI13:
            {
              /*
              SI3 has been received during RACHing with gprs_indicator=0.
              PS conn est should be aborted all SI re-acquired
              */
              rr_event_T resel_event;

              // Stop & reset NC measurement reporting (if active)
              resel_event = rr_gprs_nc_measurement_control(RR_EV_DEACTIVATE_MEASUREMENTS_NO_SI13, NULL);

              (void) rr_resel_control(resel_event, NULL);

              // Send request to disable PS access
              send_ps_imsg_no_ps_access_req(
                FALSE   // suspend_gmm
              );

              // and wait in GRR_CAMPED_DISABLING_PS_ACCESS for the confirmation
              grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;

              /*
              Send RR_IMSG_RR_EVENT_IND, containing EV_NON_CAMPED_SYS_INFO_ACQUISITION.
              Handling for this is blocked in GRR_CAMPED_DISABLING_PS_ACCESS, so it will be received
              into GRR-CONTROL when theh state returns to GRR_CAMPED
              */
              rr_grr_send_imsg_rr_event_ind(EV_NON_CAMPED_SYS_INFO_ACQUISITION);
              break;
            }

            case EV_GPRS_SUSPEND:
            {
              // Send request to disable PS access (it may be already)
              send_ps_imsg_no_ps_access_req(TRUE);

              // Start IDLE mode
              rr_l1_idle_send_imsg_start_idle_req();
              grr_control_data_ptr->state = GRR_STARTING_L1_IDLE;
              break;
            }

            default:
            {
              // After the L1 idle mode is started, there is no need to send a RR_SERVICE_IND to MM
              grr_control_data_ptr->notify_mm_service_ind = FALSE;

              // Send request to start L1 Idle Mode
              rr_l1_idle_send_imsg_start_idle_req();

              // Wait for RR_L1_IDLE_OMSG_START_IDLE_CNF
              grr_control_data_ptr->state = GRR_STARTING_L1_IDLE;
            }
          }

          grr_control_data_ptr->tbf_est_abort_event = EV_NO_EVENT;
        }

        else
        /*****************************************************************/
        /* EV_FORCE_CELL_RESELECTION or EV_DSF_RESELECTION               */
        /* A cell reselection must be started. Return to GRR_CAMPED and  */
        /* process the request there                                     */
        /*****************************************************************/
        if ( (grr_control_event == EV_FORCE_CELL_RESELECTION) ||
             (grr_control_event == RR_EV_START_RESELECTION_POWER_SCAN)||
             (grr_control_event == EV_DSF_RESELECTION) ||
             (grr_control_event == EV_DSF_FOR_DENIALS) )
        {
#ifdef FEATURE_GERAN_BSR_PS_STATUS
          // Update NAS with 'PS not active' indication
          rr_mm_send_rr_ps_status_change_ind(
            FALSE   // ps_is_active
          );
#endif
          grr_control_data_ptr->state = GRR_CAMPED;
          new_grr_control_event = grr_control_event;
        }

        else
        /*******************************************************************/
        /* EV_T3174_TIMEOUT || EV_T3176_TIMEOUT                            */
        /* A pcco is ongoing and these timer event must be handled         */
        /*******************************************************************/
        if ( ( grr_control_event == EV_T3174_TIMEOUT ) ||
             ( grr_control_event == EV_T3176_TIMEOUT ) )
        {
#ifdef FEATURE_GERAN_BSR_PS_STATUS
          // Update NAS with 'PS not active' indication
          rr_mm_send_rr_ps_status_change_ind(
            FALSE   // ps_is_active
          );
#endif
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          if (rr_x2g_cco_in_progress())
          {
            new_grr_control_event = grr_conn_est_control(grr_control_event, NULL);
          }
          else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
          {
            grr_control_data_ptr->state = GRR_CAMPED;
            new_grr_control_event = grr_control_event;
          }
        }

        else
        if (grr_control_event == EV_PCCO_FAILED)
        {
#ifdef FEATURE_GERAN_BSR_PS_STATUS
          // Update NAS with 'PS not active' indication
          rr_mm_send_rr_ps_status_change_ind(
            FALSE   // ps_is_active
          );
#endif

          grr_control_data_ptr->state = GRR_CELL_RESELECTION;
          new_grr_control_event = rr_resel_control(EV_PCCO_FAILED, NULL);
        }

        else
        /*******************************************************************/
        /* EV_RACH_FOR_PS_FAILURE                                          */
        /* No response to request to RACH - trigger PANIC RESET            */
        /******************************************************************/
        if ( grr_control_event == EV_RACH_FOR_PS_FAILURE )
        {
#ifdef FEATURE_GERAN_BSR_PS_STATUS
          // Update NAS with 'PS not active' indication
          rr_mm_send_rr_ps_status_change_ind(
            FALSE   // ps_is_active
          );
#endif

          if (geran_get_nv_recovery_restart_enabled_dyn())
          {
            MSG_GERAN_ERROR_0_G("EV_RACH_FOR_PS_FAILURE,Triggering PANIC RESET");
            geran_initiate_recovery_restart_dyn(GERAN_CLIENT_GRR);
          }
          else
          {
            ERR_GERAN_FATAL_0_G("EV_RACH_FOR_PS_FAILURE no response to rach");
          }
        }

        else
        /*******************************************************************/
        /* EV_PANIC_RESET                                                  */
        /* Respond to panic reset - cleanup RR and GRR connection          */
        /* establishment                                                   */
        /*******************************************************************/
        if ( grr_control_event == EV_PANIC_RESET )
        {
#ifdef FEATURE_GERAN_BSR_PS_STATUS
          // Update NAS with 'PS not active' indication
          rr_mm_send_rr_ps_status_change_ind(
            FALSE   // ps_is_active
          );
#endif

          (void) rr_connection_establishment_control(EV_RESET_SOFTWARE, NULL);

          (void) grr_conn_est_control(EV_RESET_SOFTWARE, NULL);
        }
#ifdef FEATURE_WCDMA

        else if (grr_control_event == EV_CR_G2W_TIMER_EXPIRED)
        {
          MSG_GERAN_MED_1_G("Ignoring G2W timer expiry in GRR_CONNECTION_PENDING state",
            0);
        }
#endif 
        /******************************************************************/
        /* RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED                             */
        /* The L1 idle mode needs to be changed. Re-camp on the cell in   */
        /* the appropriate mode                                           */
        /******************************************************************/
        else if ( grr_control_event == RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED )
        {
#ifdef FEATURE_GERAN_BSR_PS_STATUS
          // Update NAS with 'PS not active' indication
          rr_mm_send_rr_ps_status_change_ind(
            FALSE   // ps_is_active
          );
#endif

          /* abort PS connection and reset RR's CE state machine */
          (void) rr_connection_establishment_control(EV_GPRS_RESET_RR_CE_STATE, NULL);

          /* Stop RACHes in L1 and clear NON-DRX mode */
          rr_send_mph_abort_ra_req();
          rr_gprs_clear_l1_in_mm_non_drx();

          MSG_GERAN_HIGH_0_G("Change L1 idle mode to match cell capability");

          // After the L1 idle mode is changed, a RR_SERVICE_IND needs to be sent to MM
          grr_control_data_ptr->notify_mm_service_ind = TRUE;

          // Send request to change the L1 Idle Mode. This will cancel any pending reselection, block PS
          // access, and then start idle
          rr_l1_idle_send_imsg_change_idle_req();

          // Wait for RR_L1_IDLE_OMSG_START_IDLE_CNF
          grr_control_data_ptr->state = GRR_STARTING_L1_IDLE;
        }

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
        else if (grr_control_event == EV_X2G_CCO_FAILURE_CCO_TIMER_EXPIRY ||
                 grr_control_event == EV_X2G_CCO_FAILURE_NO_RESPONSE ||
                 grr_control_event == EV_X2G_CCO_FAILURE_IA_REJECT ||
                 grr_control_event == EV_X2G_CCO_FAILURE_CONTENTION_RESOLUTION_FAILURE ||
                 grr_control_event == EV_X2G_CCO_FAILURE_DSF)
        {
#ifdef FEATURE_GERAN_BSR_PS_STATUS
          // Update NAS with 'PS not active' indication
          rr_mm_send_rr_ps_status_change_ind(
            FALSE   // ps_is_active
          );
#endif

          /* Need to indicate to rr_control() that the X2G CCO procedure has
          failed due to timer expiration.  This will then cause rr_control()
          to shut down L1 and return control to the initiating RAT. */

          grr_control_data_ptr->x2g_cco_status = rr_grr_map_rr_gprs_ce_x2g_cco_failure_event(
            grr_control_event
          );

          // Send request to block PS access (which may already be blocked)
          // This will return RR_PS_OMSG_NO_PS_ACCESS_CNF
          send_ps_imsg_no_ps_access_req(
            FALSE   // suspend_gmm
          );

          grr_control_data_ptr->state = GRR_X2G_CCO_FAILED;
        } /* EV_X2G_CCO_FAILED_IND */

        else if (grr_control_event == EV_X2G_CCO_ABORTED_FOR_HIGH_PRI_REQ)
        {
          grr_control_data_ptr->tbf_est_abort_event = grr_control_event;
          new_grr_control_event = grr_conn_est_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL);
        }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_SGLTE
        else if (grr_control_event == EV_SGLTE_X2G_CCO_FAILED_GO_TO_IDLE)
        {
#ifdef FEATURE_GERAN_BSR_PS_STATUS
          // Update NAS with 'PS not active' indication
          rr_mm_send_rr_ps_status_change_ind(
            FALSE   // ps_is_active
          );
#endif

          rr_grr_send_imsg_switch_to_cs_only_gsm_only_req(
            rr_sglte_x2g_cco_failed_cb_fn
          );
          grr_control_data_ptr->state = GRR_CAMPED;
        }
#endif /* FEATURE_SGLTE */
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION

        else if (grr_control_event == EV_BCCH_CHANGE_DETECTED)
        {
          clear_waiting_for_surround_response();

          /* Start 30s sys info refresh cycle, and acquire any remaining SIs */
          rr_gprs_activate_camped_sys_info_handling();
          if (rr_expire_refresh_timer())
          {  
            MSG_GERAN_MED_0_G("Re-acquiring SI13 due to PEO-BCCH change");
            grr_control_data_ptr->state = GRR_CAMPED;
          }
          else
          {
            MSG_GERAN_MED_0_G("Refresh cannot be done start idle and re-init RMC control");
            rr_reduced_mobility_control(EV_RESET_SOFTWARE, NULL);
            new_grr_control_event = EV_GRR51_GO_TO_PACKET_IDLE;
          }
        }
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

        else
        /*****************************************************************/
        /* Pass everything else in the GPRS Conn Est state machine       */
        /*****************************************************************/
        {
          new_grr_control_event = grr_conn_est_control(grr_control_event, NULL);
        }

        break;
      }

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* GRR_CELL_RESELECTION - In this state GRR51 is performing       */
      /* a cell reselection, and attempting to camp onto another cell   */
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

      case GRR_CELL_RESELECTION:
      {
        switch (grr_control_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_SURROUND_UPDATE_FAILURE_IND:
                  case MPH_RESELECTION_CNF:
                  case MPH_POWER_SCAN_CNF:
                  case MPH_DECODE_BCCH_LIST_CNF:
                  case MPH_SELECT_SPECIFIC_BCCH_CNF:
                  case MPH_DECODE_BCCH_LIST_FAILURE_IND:
                  {
                    new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                    break;
                  } /* MPH_SURROUND_UPDATE_FAILURE_IND ... */

                  case MPH_BLOCK_QUALITY_IND:
                  {
                    if (rr_is_camped())
                    {
                      new_grr_control_event = rr_gprs_check_dsc_in_idle(
                        new_message->mph_block_quality_ind.block_quality,
                        new_message->mph_block_quality_ind.l2_channel_type
                      );
                    }
                    break;
                  } /* MPH_BLOCK_QUALITY_IND */

                  case MPH_SERVING_MEAS_IND:
                  case MPH_SERVING_IDLE_MEAS_IND:
                  case MPH_SERVING_AUX_MEAS_IND:
                  case MPH_SURROUND_MEAS_IND:
#ifdef FEATURE_WCDMA
                  case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
                  case MPH_SURROUND_WCDMA_XFER_MEAS_IND:
#endif /* #ifdef FEATURE_WCDMA */
                  case MPH_FCCH_SCH_DECODE_IND:
                  {
                    /* No action necessary - reselection is already in
                    progress. */
                    break;
                  }
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
                  case MPH_PROC_IND:
                  {
                    if (TRUE == rr_static_get_suspend_in_progress())
                    {
                      rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
                    }
                    break;
                  }
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */
                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message,
                      get_grr_control_state_name(grr_control_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_L1 */

              case MS_RR_L2:
              {
                switch (message_id)
                {
                  case DL_RANDOM_ACCESS_CNF:
                  {
                    /* May be received here if DSF occurs while RACHing, no action necessary. */
                    break;
                  } /* DL_RANDOM_ACCESS_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message,
                      get_grr_control_state_name(grr_control_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_L2 */

              case MS_MAC_RR:
              {
                switch (message_id)
                {
                  case MAC_GRR_CELL_RESEL_STATUS:
                  case MAC_GRR_PCCF_SENT:
                  {
                    new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                    break;
                  } /* MAC_GRR_CELL_RESEL_STATUS ... */

                  case MAC_GRR_MAC_STATUS:
                  {
                    mac_grr_mac_status_t *mac_status;
                    mac_status = &new_message->mac_grr_sig.mac_grr_mac_status;

                    if (mac_status->mac_status == MAC_IDLE)
                    {
#ifdef FEATURE_GSM_EDTM
                      (void) rr_gprs_edtm_control(EV_MAC_IDLE_MODE, NULL);
#endif /* #ifdef FEATURE_GSM_EDTM */
                      new_grr_control_event = rr_resel_control(EV_MAC_IDLE_MODE, new_message);
                    }
                    break;
                  } /* MAC_GRR_MAC_STATUS */

                  case MAC_GRR_DATA_IND:
                  {
                    mac_grr_data_ind_t *data_ind;
                    data_ind = &new_message->mac_grr_sig.mac_grr_data_ind;

                    switch (data_ind->data_ptr.header_ptr->message_type)
                    {
                      case PACKET_CELL_CHANGE_CONTINUE:
                      {
                        new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                        break;
                      } /* PACKET_CELL_CHANGE_CONTINUE */

                      case PACKET_CELL_CHANGE_ORDER:
                      {
                        packet_cell_change_order_t *pcco;

                        pcco = (packet_cell_change_order_t *) data_ind->data_ptr.data_ptr;

                        rr_gprs_pcco_decode_params(pcco);

                        (void) rr_gprs_nc_measurement_control(EV_PCCO_MESSAGE, NULL);

                        /* Pass the mac_grr_data_ind message into cell reselection controller */
                        new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                        break;
                      } /* PACKET_CELL_CHANGE_ORDER */

                      case PACKET_MEASUREMENT_ORDER:
                      {
                        packet_measurement_order_t *pmo;
                        pmo = (packet_measurement_order_t *) data_ind->data_ptr.data_ptr;

                        new_grr_control_event = rr_gprs_pmo_decode_params(pmo);
                        break;
                      } /* PACKET_MEASUREMENT_ORDER */

#ifdef FEATURE_GPRS_PS_HANDOVER
                      case PACKET_PS_HANDOVER:
                      {
                        rr_event_T event;
                        event = rr_gprs_process_psho_command(new_message);
                        if (event == EV_PSHO_STARTED)
                        {
                          grr_control_data_ptr->state = GRR_PSHO;
                        }
                        break;
                      } /* PACKET_PS_HANDOVER */
#endif /* #ifdef FEATURE_GPRS_PS_HANDOVER */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          new_message,
                          get_grr_control_state_name(grr_control_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* MAC_GRR_DATA_IND */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message,
                      get_grr_control_state_name(grr_control_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_MAC_RR */

              case MS_MM_RR:
              {
                switch (message_id)
                {
#ifdef FEATURE_GERAN_CS_SUPPORT
                  case RR_EST_REQ:
                  {
                    rr_event_T grr_event;

                    /*
                    This returns
                     EV_NO_EVENT - CS conn est not allowed
                     EV_FORCE_RESELECTION - SNR is bad, reselect first
                     EV_RR_EST_REQ - CS conn est can proceed
                    */
                    grr_event = preprocess_rr_est_req(
                      (rr_est_req_T *)new_message
                    );

                    if (grr_event == EV_NO_EVENT)
                    {
                      /* Reject request back to MM, and discard saved message */
                      rr_send_abort_ind(OTHER_REASON, MM_RR_OTHER_FAILURE);
                      grr_throwaway_saved_message();
                    }
                    else
                    if (grr_event == EV_RR_EST_REQ)
                    {
                      /* Save for recovery in case of panic reset */
                      rr_save_msg_for_recovery_after_panic_reset(new_message);

                      /* The RR_EST_REQ is saved in RR and will be handled
                      when the reselection completes. */
                    }
                    else
                    if (grr_event == EV_FORCE_CELL_RESELECTION)
                    {
                      MSG_GERAN_HIGH_0_G("Continue with reselection (SNR bad), queue RR_EST_REQ");
                    }
                    break;
                  } /* RR_EST_REQ */
#endif /* FEATURE_GERAN_CS_SUPPORT */

                  case RR_GMM_GPRS_AUTH_FAIL_IND:
                  {
                    uint32 barred_time_secs;
                    BSIC_T        local_bsic;
                    gprs_scell_info_t  *scell_info_ptr = rr_gprs_get_scell_info();
                    rr_gmm_gprs_auth_fail_ind_T *rr_gmm_gprs_auth_fail_ind;
                    rr_gmm_gprs_auth_fail_ind = (rr_gmm_gprs_auth_fail_ind_T *)new_message;

                    CONVERT_TO_BSIC_T(local_bsic, rr_gmm_gprs_auth_fail_ind->bsic);

                    /*
                    Authentication and ciphering has failed. The serving cell should be
                    barred for the period indicated by T3302 minus 60 seconds.
                    If no T3302 value was included in previous ATTACH ACCEPT / RAU ACCEPT
                    messages, then a default of 12 hours is used (see 3GPP 43.022 3.5.5)
                    */
                    barred_time_secs = grr_auth_failed_calculate_timeout(new_message);

                    if (barred_time_secs > 0)
                    {
                      rr_authentication_failed_bar_cell(
                        CONVERT_SECONDS_TO_MILLISECONDS(barred_time_secs));

                      if ((ARFCNS_EQUAL(rr_gmm_gprs_auth_fail_ind->arfcn, scell_info_ptr->gsm.BCCH_ARFCN)) &&
                          (SAME_BSIC(local_bsic, scell_info_ptr->gsm.BSIC)) &&
                          rr_is_camped())
                      {
                        /* Should start cell reselection. Spec ref: 43.022 section 3.5.5*/                 
                        /* log the reselection trigger */
                        rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_SERVING_BARRED);

                        /* notify measurement reporting of the DSF */
                        (void) rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, new_message);

                        /* action reselection in the reselection handler */
                        rr_resel_send_imsg_required_ind(RR_RESEL_CAUSE_RESELECTION);
                      }
                      else
                      {
                        MSG_GERAN_HIGH_6_G("ARFCN mismatch Received ARFCN num,band=(%d %d) and Serving ARFCN num,band=(%d %d) and Received BSIC= %d Serving BSIC= %d ", 
                          rr_gmm_gprs_auth_fail_ind->arfcn.num,
                          rr_gmm_gprs_auth_fail_ind->arfcn.band,
                          scell_info_ptr->gsm.BCCH_ARFCN.num,
                          scell_info_ptr->gsm.BCCH_ARFCN.band,
                          rr_gmm_gprs_auth_fail_ind->bsic,
                          CONVERT_BSIC_TO_BYTE(scell_info_ptr->gsm.BSIC));

                        rr_authn_fail_bar_cell_basedon_ARFCN_BSIC(
                          CONVERT_SECONDS_TO_MILLISECONDS(barred_time_secs),
                          rr_gmm_gprs_auth_fail_ind->arfcn,
                          rr_gmm_gprs_auth_fail_ind->bsic);
                      }
                    }
                    else
                    {
                      MSG_GERAN_HIGH_1_G("T3302 only %d secs - not long enough to bar cell", barred_time_secs);
                    }
                    break;
                  } /* RR_GMM_GPRS_AUTH_FAIL_IND */

                  case RR_PLMN_SELECT_REQ:
                  {
                    new_grr_control_event = handle_rr_plmn_select_req(
                      &new_message->rr_plmn_select_req
                    );
                    break;
                  } /* RR_PLMN_SELECT_REQ */

                  case RR_PLMN_LIST_REQ:
                  {
                    new_grr_control_event = handle_rr_plmn_list_req(
                      &new_message->rr_plmn_list_req
                    );
                    break;
                  } /* RR_PLMN_LIST_REQ */

                  case RR_STOP_GSM_MODE_REQ:
                  case RR_DEACT_REQ:
                  {
                    /* Save this message in case a panic reset occurs while the reselection */
                    /* is aborting */
                    rr_save_msg_for_recovery_after_panic_reset(new_message);

                    /*Abort reselection*/
                    (void) rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, NULL);

                    rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
                    break;
                  } /* RR_STOP_GSM_MODE_REQ ... */

                  case RR_GMM_GPRS_STATE_CHANGE_REQ:
                  {
                    /* We don't actually want to handle the return events from
                    this state machine as RR is not camped at this time. */
                    (void) rr_gprs_nc_measurement_control(EV_INPUT_MESSAGE, new_message);
                    break;
                  }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q                  
                  case RR_PSEUDO_LTE_LIST_UPDATE_IND:
                  {
                    /* Pseudo List Update will be Ignored during reselections */
                    break;
                  }
#endif
                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message,
                      get_grr_control_state_name(grr_control_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_MM_RR */

              case MS_RR_RR:
              {
                switch (new_message->rr.header.rr_message_set)
                {
                  case RR_GRR_IMSG:
                  {
                    switch (new_message->rr.header.imh.message_id)
                    {
                      case RR_GRR_IMSG_CAMPED_ON_CELL_IND:
                      {
                        new_grr_control_event = RR_EV_CAMPED_ON_CELL;
                        break;
                      } /* RR_GRR_IMSG_CAMPED_ON_CELL_IND */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
                      case RR_GRR_IMSG_MSC_CHANGE_IND:
                      {
                        /*
                        Update NAS as required
                        RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                        This will be sent when the current procedure completes
                        */
                        rr_msc_update(
                          FALSE   // can_send_service_ind
                        );
                        break;
                      }
#endif

                      case RR_GRR_IMSG_PS_ACCESS_REQ:
                      {
                        // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                        rr_grr_imsg_ps_access_req_t *imsg = &new_message->rr.grr.imsg.ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn);
                        break;
                      }

                      case RR_GRR_IMSG_NO_PS_ACCESS_REQ:
                      {
                        // No PS access request (could be TLLI unassignment, CS-only)

                        rr_grr_imsg_no_ps_access_req_t *imsg = &new_message->rr.grr.imsg.no_ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn);
                        break;
                      }

                      case RR_GRR_IMSG_PAGE_IND:
                      {
                        rr_grr_imsg_page_ind_t *imsg = &new_message->rr.grr.imsg.page_ind;

                        // A page received during cell reselection is saved. It is forwarded to NAS when RR/L1
                        // are in Packet Idle
                        rr_ce_save_page(&imsg->page_info);
                        break;
                      }

                      case RR_GRR_IMSG_PAGE_MODE_IND:
                      {
                        // Changes in OTA page mode are ignored during Early Camping
                        MSG_GERAN_LOW_0_G("Page Mode change ignored");
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          new_message,
                          get_grr_control_state_name(grr_control_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_GRR_IMSG */

                  case RR_RESEL_OMSG:
                  {
                    switch (new_message->rr.resel.omsg.header.imh.message_id)
                    {
                      case RR_RESEL_OMSG_ABORT_CNF:
                      {
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
                        if (TRUE == rr_static_get_suspend_in_progress())
                        { 
                          /* Declare no bcch found */
                          output_event = relinquish_control_to_rr(EV_NO_BCCH_FOUND);
                          grr_control_data_ptr->state = GRR_NULL;
                        }
                        else 
#endif
                        if (rr_examine_saved_message(NULL) == EV_INPUT_MESSAGE)
                        {
                          output_event = relinquish_control_to_rr(EV_PROCESS_SAVED_MESSAGE);
                          grr_control_data_ptr->state = GRR_NULL;
                        }
                        else
                        {
                          MSG_GERAN_HIGH_3_G("There is no saved message something gone wrong. move to camped state",
                                   0,
                                   0,
                                   0);

                          /* notify measurement reporting of the cell change completion */
                          new_grr_control_event =
                            rr_gprs_nc_measurement_control(RR_EV_CAMPED_ON_CELL, NULL);

                          clear_waiting_for_surround_response();

#ifndef FEATURE_IDLE_DRX_SCALING
                          /* initialise the DSC */
                          rr_gprs_initialise_dsc();
#endif /* !FEATURE_IDLE_DRX_SCALING */
                          rr_gprs_init_bad_snr_counter();

                          if (new_grr_control_event == EV_NO_EVENT)
                          {
                            /* Move to CAMPED state, start GRR camped procedures */
                            grr_control_data_ptr->state = transition_to_camped_state(
                              FALSE,                               // service_ind_sent
                              &new_grr_control_event,
                              (rr_cmd_bdy_type **) &new_message,
                              &message_set,
                              &message_id
                            );
                          }
                        }

                        break;
                      }

                      case RR_RESEL_OMSG_FINISHED_IND:
                      {
                        if (new_message->rr.resel.omsg.finished_ind.status == RR_RESEL_STATUS_OUT_OF_SERVICE)
                        {
                          rr_PLMN_request_details_T *plmn_request_details;

                          plmn_request_details = rr_get_PLMN_request_details_ptr();

                          if (plmn_request_details->search_reason == RR_PLMN_SELECTION)
                          {
                            output_event = relinquish_control_to_rr(RR_EV_PROCESS_PLMN_SELECT_REQ);
                          }
                          else
                          {
                            output_event = relinquish_control_to_rr(RR_EV_TRIGGER_CELL_SELECTION);
                          }

                          grr_control_data_ptr->state = GRR_NULL;
                        }
                        else
                        {
#ifdef FEATURE_GERAN_CS_SUPPORT
                          gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();

                          rr_abort_pending_rr_est_req_if_lai_changed(
                            &(scell_info_ptr->gsm.location_area_identification)
                          );
#endif /* FEATURE_GERAN_CS_SUPPORT */

                          /* notify measurement reporting of the cell change completion */
                          new_grr_control_event =
                            rr_gprs_nc_measurement_control(RR_EV_CAMPED_ON_CELL, NULL);

                          /* Determine if reselection handling should be activated based on NC mode.
                          call must be performed AFTER sending RR_EV_CAMPED_ON_CELL to NC measurement
                          control state machine. */
                          grr_activate_reselection_if_allowed();

#ifndef FEATURE_IDLE_DRX_SCALING
                          /* initialise the DSC */
                          rr_gprs_initialise_dsc();
#endif /* !FEATURE_IDLE_DRX_SCALING */
                          rr_gprs_init_bad_snr_counter();

                          if (new_grr_control_event == EV_NO_EVENT)
                          {
                            send_rr_service_ind();
                            /* Move to CAMPED state, start GRR camped procedures */
                            grr_control_data_ptr->state = transition_to_camped_state(
                              TRUE,                               // service_ind_sent
                              &new_grr_control_event,
                              (rr_cmd_bdy_type **) &new_message,
                              &message_set,
                              &message_id
                            );
                          }
                        }

                        break;
                      }

                      case RR_RESEL_OMSG_STARTED_IND:
                      {
                        /* No Action */
                        break;
                      } /* RR_RESEL_OMSG_STARTED_IND */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          new_message,
                          get_grr_control_state_name(grr_control_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_RESEL_OMSG */

                  case RR_RESEL_IMSG:
                  case RR_PSCAN_IMSG:
                  case RR_PSCAN_OMSG:
                  case RR_DECODE_BCCH_IMSG:
                  case RR_DECODE_BCCH_OMSG:
                  case RR_CELL_ACQ_IMSG:
                  case RR_CELL_ACQ_OMSG:
                  case RR_L1_IDLE_IMSG:
                  case RR_L1_IDLE_OMSG:
                  case RR_PS_IMSG:
                  case RR_PS_OMSG:
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
                  case RR_RESEL_RMC_IMSG:
                  case RR_RESEL_RMC_OMSG:
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/
                  {
                    new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                    break;
                  } /* RR_PSCAN_IMSG ... */

                  case RR_SELECT_BCCH_IMSG:
                  {
                    break;    // ignore - no processing required
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message,
                      get_grr_control_state_name(grr_control_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_TIMER:
              {
                switch (message_id)
                {
                  case MID_TIMER_EXPIRY:
                  {
                    timer_expiry_T *timer_expiry = (timer_expiry_T *) new_message;
                    if (timer_expiry->timer_id == RR_GPRS_SYS_INFO_TIMER ||
                        timer_expiry->timer_id == CELL_RESELECTION_TIMER ||
                        timer_expiry->timer_id == RR_BCCH_DECODE_COMPLETE_TIMER)
                    {
                      new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                    }
                    else
                    {
                      new_grr_control_event = rr_gprs_reselection_timer_handler(
                        &new_message->mid_timer_expiry
                      );
                    }
                    break;
                  } /* MID_TIMER_EXPIRY */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message,
                      get_grr_control_state_name(grr_control_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_TIMER */

#ifdef FEATURE_SGLTE
              case MS_MSGR_RR:
              {
                switch (new_message->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case GERAN_GRR_LTE_PLMN_SRCH_REQ:
                  {
                    MSG_GERAN_HIGH_0_G("GERAN_GRR_LTE_PLMN_SRCH_REQ rejected during cell reselection");
                    rr_sglte_reject_lte_plmn_srch_req();
                    grr_throwaway_saved_message();
                    grr_control_event = EV_NO_EVENT;
                    break;
                  } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */
#endif /* FEATURE_LTE */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message, get_grr_control_state_name(grr_control_data_ptr->state));
                    break;
                  }
                }

                break;
              } /* MS_MSGR_RR */

#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch (new_message->message_header.message_id)
                {
                  case TDSRR_INTERRAT_PLMN_SRCH_REQ:
                  {
                    MSG_GERAN_HIGH_0_G("TDSRR_INTERRAT_PLMN_SRCH_REQ rejected during cell reselection");
                    rr_sglte_reject_tds_plmn_srch_req();
                    grr_throwaway_saved_message();
                    grr_control_event = EV_NO_EVENT;
                    break;
                  } /* TDSRR_INTERRAT_PLMN_SRCH_REQ */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message, get_grr_control_state_name(grr_control_data_ptr->state));
                    break;
                  }
                }

                break;
              } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_SGLTE */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  new_message,
                  get_grr_control_state_name(grr_control_data_ptr->state)
                );
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          /* Handle events that are basically just passed through to the cell
          reselection state machine first. */

          case EV_DSF_RESELECTION:
          case EV_DSF_FOR_DENIALS:
          case EV_T3174_TIMEOUT:
          case EV_T3176_TIMEOUT:
          case RR_EV_DECODING_BCCH:
          case EV_ALL_NACC_PNCD_DISPATCHED:
          case EV_MAC_IDLE_MODE:
          {
            new_grr_control_event = rr_resel_control(grr_control_event, NULL);
            break;
          } /* EV_DSF_RESELECTION ... */

          case RR_EV_EARLY_CAMP_SI_RECEIVED:
          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          case RR_EV_GSM_SI_RECEIVED:
          case RR_EV_GPRS_SI13_RECEIVED:
          {
            break;  // can be ignored
          }

          case EV_PANIC_RESET:
          {
            /* Respond to panic reset - cleanup reselection */
            (void) rr_resel_control(EV_RESET_SOFTWARE, NULL);

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
            (void) rr_reduced_mobility_control(EV_RESET_SOFTWARE, NULL);
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

            break;
          } /* EV_PANIC_RESET */

          case EV_PMO_MESSAGE:
          {
            new_grr_control_event = rr_resel_control(
              rr_gprs_nc_measurement_control(EV_PMO_MESSAGE, NULL),
              NULL
            );
            break;
          } /* EV_PMO_MESSAGE */


          /* Handle events that require some work in this state machine. */

          case EV_CELL_RESELECTION_ACT:
          case EV_CELL_RESELECTION_DEACT:
          {
            (void) rr_resel_control(grr_control_event, NULL);

            send_rr_service_ind();

            grr_control_data_ptr->state = transition_to_camped_state(
              TRUE,                               // service_ind_sent
              &new_grr_control_event,
              (rr_cmd_bdy_type **) &new_message,
              &message_set,
              &message_id
            );
            break;
          } /* EV_CELL_RESELECTION_ACT ... */

          case EV_MAC_GUARD_TIMER_EXPIRED:
          {
            /* No response from MAC to reselection request - trigger panic reset */
            if (geran_get_nv_recovery_restart_enabled_dyn())
            {
              MSG_GERAN_ERROR_0_G("No Response from MAC,Triggering PANIC RESET");
              geran_initiate_recovery_restart_dyn(GERAN_CLIENT_GRR);
            }
            else
            {
              ERR_GERAN_FATAL_0_G("No response from MAC");
            }
            break;
          } /* EV_MAC_GUARD_TIMER_EXPIRED */

#ifdef FEATURE_GERAN_CS_SUPPORT
          case EV_RR_EST_REQ:
          {
            grr_control_data_ptr->state = GRR_CAMPED;
            new_grr_control_event = EV_RR_EST_REQ;
            break;
          } /* EV_RR_EST_REQ */
#endif /* FEATURE_GERAN_CS_SUPPORT */

          case EV_ABANDON_RESELECTION:
          case RR_EV_CAMPED_ON_CELL:
          case EV_CAMP_ON_PCCF_OLD_SCELL:
          {
            /* notify measurement reporting of the cell change completion */
            new_grr_control_event = rr_gprs_nc_measurement_control(grr_control_event, new_message);

            clear_waiting_for_surround_response();

            if (grr_control_event != EV_ABANDON_RESELECTION)
            {
#ifndef FEATURE_IDLE_DRX_SCALING
              /* initialise the DSC */
              rr_gprs_initialise_dsc();
#endif /* !FEATURE_IDLE_DRX_SCALING */
              rr_gprs_init_bad_snr_counter();
            }

            if (new_grr_control_event == EV_NO_EVENT)
            {
              /* Move to CAMPED state, start GRR camped procedures */
              grr_control_data_ptr->state = transition_to_camped_state(
                FALSE,                              // service_ind_sent
                &new_grr_control_event,
                (rr_cmd_bdy_type **) &new_message,
                &message_set,
                &message_id
              );
            }
            break;
          } /* EV_ABANDON_RESELECTION ... */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          case RR_EV_RESELECT_WCDMA_CELL:
          {
            grr_control_data_ptr->state = GRR_CELL_RESELECTION_G2W;
            break;
          } /* RR_EV_RESELECT_WCDMA_CELL */
#endif 

          case RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED:
          {
            MSG_GERAN_ERROR_0_G("Unexpected RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED during reselection");
            break;
          } /* RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED */

          /* Handle output events from the cell reselection state machine. */
          case EV_NO_BCCH_FOUND:
          case RR_EV_TRIGGER_CELL_SELECTION:
          case EV_PROCESS_SAVED_MESSAGE:
          {
            grr_control_data_ptr->state = GRR_NULL;

            output_event = relinquish_control_to_rr(grr_control_event);
            break;
          } /* EV_NO_BCCH_FOUND ... */

          case RR_EV_MEASUREMENT_PARAMS_UPDATED:
          case EV_CELL_RESELECTION:
          case EV_CR_G2W_TIMER_EXPIRED:
          {
            /* No action necessary. */
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(
              rr_event_name(grr_control_event),
              get_grr_control_state_name(grr_control_data_ptr->state)
            );
          }
        }
        break;
      }

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* GRR_CELL_RESELECTION_G2W                                            */
      /* This state is entered when a G2W reselection is triggered.  GRR     */
      /* leave this state when the reselection completes successfully        */
      /* (indicated by MM) or when it is rejected (by RRC) or aborted (by MM)*/
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
      case GRR_CELL_RESELECTION_G2W:
      {
        switch (grr_control_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (new_message->rr.header.rr_message_set)
                {
                  case RR_GRR_IMSG:
                  {
                    switch (new_message->rr.header.imh.message_id)
                    {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
                      case RR_GRR_IMSG_MSC_CHANGE_IND:
                      {
                        /*
                        Update NAS as required
                        RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                        This will be sent when G is next active
                        */
                        rr_msc_update(
                          FALSE   // can_send_service_ind
                        );
                        break;
                      }
#endif

                      case RR_GRR_IMSG_PS_ACCESS_REQ:
                      {
                        // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                        rr_grr_imsg_ps_access_req_t *imsg = &new_message->rr.grr.imsg.ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn);
                        break;
                      }

                      case RR_GRR_IMSG_NO_PS_ACCESS_REQ:
                      {
                        // No PS access request (could be TLLI unassignment, CS-only)

                        rr_grr_imsg_no_ps_access_req_t *imsg = &new_message->rr.grr.imsg.no_ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn);
                        break;
                      }

                      case RR_GRR_IMSG_PAGE_IND:
                      {
                        rr_grr_imsg_page_ind_t *imsg = &new_message->rr.grr.imsg.page_ind;

                        // A page received during cell reselection is saved. It is forwarded to NAS when RR/L1
                        // are in Packet Idle
                        rr_ce_save_page(&imsg->page_info);
                        break;
                      }

                      case RR_GRR_IMSG_PAGE_MODE_IND:
                      {
                        // Changes in OTA page mode are ignored during Early Camping
                        MSG_GERAN_LOW_0_G("Page Mode change ignored");
                        break;
                      }

#ifdef FEATURE_SGLTE
                      case RR_GRR_IMSG_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ:
                      {
                        rr_resel_send_imsg(RR_RESEL_IMSG_STOP_GSM_MODE_REQ_SGLTE_RAT_CHANGE);
                        break;
                      }

                      case RR_GRR_IMSG_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ:
                      {
                        /* GRR received User Triggered SGLTE split from NAS while the Cell Reselection is Ongoing */
                        /* Remember USer Split and Abort the Ongoing Cell Reselection G2W. Once the Abort is finished */
                        /* Honour the User Triggered SGLTE Split */
                        grr_control_data_ptr->is_sglte_user_rat_change_pending = TRUE;
                        rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ); 
                        break;                       
                      }
#endif /* FEATURE_SGLTE */

                      default:
                      {
                        new_grr_control_event = rr_resel_control(grr_control_event, new_message);
                      }
                    }
                    break;
                  }

                  case RR_RESEL_OMSG:
                  {
                    switch (new_message->rr.header.imh.message_id)
                    {
                      case RR_RESEL_OMSG_ABORT_CNF:
                      {
#ifdef FEATURE_GERAN_CS_SUPPORT
                        // If there is a RR_EST_REQ stored locally, re-save it in RR-CONTROL
                        if (grr_control_data_ptr->saved_rr_est_req_valid)
                        {
                          rr_cmd_bdy_type *saved_rr_est_req_ptr = GPRS_MEM_MALLOC(sizeof(rr_cmd_bdy_type));

                          if (saved_rr_est_req_ptr != NULL)
                          {
                            saved_rr_est_req_ptr->mm.rr_est_req = grr_control_data_ptr->saved_rr_est_req;
                            rr_save_msg(saved_rr_est_req_ptr);
                            GPRS_MEM_FREE(saved_rr_est_req_ptr);
                          }

                          grr_control_data_ptr->saved_rr_est_req_valid = FALSE;
                        }
#endif /* FEATURE_GERAN_CS_SUPPORT */

#ifdef FEATURE_SGLTE
                        if ( rr_is_sglte_mode_active() && grr_control_data_ptr->is_sglte_user_rat_change_pending )
                        {
                          rr_grr_send_imsg_switch_to_cs_only_user_gsm_only_req(rr_sglte_confirm_rat_change_cb_fn);
                          /* Finish the procedure */
                          grr_control_data_ptr->is_sglte_user_rat_change_pending = FALSE;
                          grr_control_data_ptr->state = GRR_CAMPED;

                        }
                        else
                        {
#endif
                          /* Reselection has been aborted. */
                          if (rr_is_camped())
                          {
                            grr_control_data_ptr->state = transition_to_camped_state(
                              FALSE,                            // service_ind_sent
                              &new_grr_control_event,
                              (rr_cmd_bdy_type **)&new_message,
                              &message_set,
                              &message_id
                            );
                          }
                          else
                          {
                            if (grr_verify_saved_mm_message(RR_PLMN_SELECT_REQ))
                            {
                              rr_cmd_bdy_type *saved_message;

                              if (rr_examine_saved_message(&saved_message) == EV_INPUT_MESSAGE)
                              {
                                (void) rr_process_plmn_select_req(
                                  &saved_message->rr_plmn_select_req,
                                  NULL
                                );
                              }
                              else
                              {
                                MSG_GERAN_LOW_0_G("Unexpected saved message!");
                              }
                            }
                            grr_control_data_ptr->state = GRR_NULL;

                            output_event = relinquish_control_to_rr(EV_PROCESS_SAVED_MESSAGE);
                          }
#ifdef FEATURE_SGLTE
                        }
#endif
                        break;
                      } /* RR_RESEL_OMSG_ABORT_CNF */

                      case RR_RESEL_OMSG_FINISHED_IND:
                      {
                        rr_resel_omsg_finished_ind_t *finished_ind;
                        finished_ind = &new_message->rr.resel.omsg.finished_ind;

#ifdef FEATURE_GERAN_CS_SUPPORT
                        // If there is a RR_EST_REQ stored locally, re-save it in RR-CONTROL
                        if (grr_control_data_ptr->saved_rr_est_req_valid)
                        {
                          rr_cmd_bdy_type saved_rr_est_req;
                          saved_rr_est_req.rr_est_req = grr_control_data_ptr->saved_rr_est_req;

                          rr_save_msg(&saved_rr_est_req);

                          grr_control_data_ptr->saved_rr_est_req_valid = FALSE;
                        }
#endif /* FEATURE_GERAN_CS_SUPPORT */

                        switch (finished_ind->status)
                        {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                          case RR_RESEL_STATUS_G2W_RESELECTION_SUCCESS:
                          {

#ifdef FEATURE_SGLTE
                            if (rr_is_sglte_mode_active())
                            {
                              rr_sglte_set_rat_priority_list_gsm_only();

                              rr_ps_sglte_handle_ps_removal(rr_sglte_confirm_rat_change_cb_fn);

                              new_grr_control_event = EV_NO_EVENT;

                              /* Move to CAMPED state, start GRR camped procedures */
                              grr_control_data_ptr->state = transition_to_camped_state(
                                TRUE,                               // service_ind_sent
                                &new_grr_control_event,
                                (rr_cmd_bdy_type **) &new_message,
                                &message_set,
                                &message_id
                              );
                            }
                            else
#endif /* FEATURE_SGLTE */
                            {
                              grr_control_data_ptr->state = GRR_NULL;
                              output_event = relinquish_control_to_rr(EV_PROCESS_SAVED_MESSAGE);
                            }
                            break;
                          }

                          case RR_RESEL_STATUS_G2W_RESELECTION_FAILURE:
                          {
#ifdef FEATURE_SGLTE                            
                            if (rr_is_sglte_mode_active())
                            {
                              new_grr_control_event = EV_NO_EVENT;
                                
                              /* Move to CAMPED state, start GRR camped procedures, which includes GPRS Resumption */
                              grr_control_data_ptr->state = transition_to_camped_state(
                                FALSE,                               // service_ind_sent
                                &new_grr_control_event,
                                (rr_cmd_bdy_type **) &new_message,
                                &message_set,
                                &message_id
                              );
                              if (!rr_gsm_only())
                              {
                                rr_l1_update_wcdma_neighbor_list();
                              }
                            }
                            else
#endif                                
                            {
                              MSG_GERAN_ERROR_0_G("This message is NOT EXPECTED in NON _SGLTE Cases!");

                            }
                            break;
                          }
                           
                          case RR_RESEL_STATUS_G2W_PCCO_SUCCESS:
                          {
                            grr_control_data_ptr->state = GRR_NULL;

                            output_event = relinquish_control_to_rr(RR_EV_G2WR_GO_TO_INACTIVE);
                            break;
                          }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

                          /* G2W reselection was rejected by RRC.  RR has
                          recovered service on the old serving cell and idle
                          mode has been started in L1. */
                          case RR_RESEL_STATUS_IN_SERVICE:
                          {
                            /* notify measurement reporting of the cell change completion */
                            new_grr_control_event =
                              rr_gprs_nc_measurement_control(RR_EV_CAMPED_ON_CELL, NULL);

                            clear_waiting_for_surround_response();

#ifndef FEATURE_IDLE_DRX_SCALING
                            /* initialise the DSC */
                            rr_gprs_initialise_dsc();
#endif /* !FEATURE_IDLE_DRX_SCALING */
                            rr_gprs_init_bad_snr_counter();

                            if (new_grr_control_event == EV_NO_EVENT)
                            {
                              send_rr_service_ind();

                              /* Move to CAMPED state, start GRR camped procedures */
                              grr_control_data_ptr->state = transition_to_camped_state(
                                TRUE,                               // service_ind_sent
                                &new_grr_control_event,
                                (rr_cmd_bdy_type **) &new_message,
                                &message_set,
                                &message_id
                              );
                            }
                            break;
                          }

                          /* G2W reselection was rejected by RRC.  RR has
                          failed to recover service on the old serving cell. */
                          case RR_RESEL_STATUS_OUT_OF_SERVICE:
                          {
                            grr_control_data_ptr->state = GRR_NULL;
                            output_event = relinquish_control_to_rr(RR_EV_TRIGGER_CELL_SELECTION);
                            break;
                          }

                          default:
                          {
                            MSG_GERAN_ERROR_1_G("Unexpected status: %d", finished_ind->status);
                            /* TODO: Trigger panic reset? */
                          }
                        }
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          new_message,
                          "GRR_CELL_RESELECTION_G2W"
                        );
                      }
                    }
                    break;
                  } /* RR_RESEL_OMSG */

                  default:
                  {
                    new_grr_control_event = rr_resel_control(grr_control_event, new_message);
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_MM_RR:
              {
                switch (message_id)
                {
#ifdef FEATURE_GERAN_CS_SUPPORT
                  case RR_ABORT_REQ:
                  {
                    /* If MM has aborted a RR_EST_REQ which is as yet unprocessed, remove it */                    
                    /* When GRR is in state GRR_CELL_RESELECTION_G2W case, the RR_EST_REQ() is Purged from RR to GRR, 
                    so first check the GRR Queue and cancel the Saved Estabish Request, if not found then check the RR queue */
                    if ( grr_control_data_ptr->saved_rr_est_req_valid )
                    {                        
                        memset(&grr_control_data_ptr->saved_rr_est_req, 0, sizeof(rr_est_req_T));
                        grr_control_data_ptr->saved_rr_est_req_valid = FALSE;
                    }

                    if (grr_verify_saved_mm_message(RR_EST_REQ))
                    {
                        grr_throwaway_saved_message();
                    }
                    rr_send_abort_cnf(MM_AS_INTERRAT_RESELECTION);
                    break;
                  } /* RR_ABORT_REQ */

                  case RR_EST_REQ:
                  {
                    rr_event_T grr_event;

                    /*
                    This returns
                     EV_NO_EVENT - CS conn est not allowed
                     EV_FORCE_RESELECTION - SNR is bad, reselect first
                     EV_RR_EST_REQ - CS conn est can proceed
                    */
                    grr_event = preprocess_rr_est_req(
                      (rr_est_req_T *)new_message
                    );

                    /* if GRR is not currently camped, reject the establishment */
                    if ((rr_is_camped() == FALSE) &&
                        (grr_event            == EV_RR_EST_REQ))
                    {
                      grr_event = EV_NO_EVENT;
                      MSG_GERAN_LOW_0_G("G2WR in progress, not yet camped, reject call attempt");
                    }

                    if (grr_event == EV_NO_EVENT)
                    {
                      /* Reject request back to MM, and discard saved message */
                      rr_send_abort_ind(REASON_INTERRAT_RESELECTION, MM_RR_OTHER_FAILURE);
                      grr_throwaway_saved_message();
                    }
                    else if (grr_event == EV_RR_EST_REQ)
                    {
                      switch (new_message->rr_est_req.establishment_cause)
                      {
                        case MM_RR_ANSWER_TO_PAGING:
                        {
                          rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_PAGE_RESPONSE);
                          break;
                        }

                        case MM_RR_LOCATION_UPDATE:
                        {
                          /* Reselection should not be aborted for Location Update, ignore. */
                          break;
                        }

                        default:
                        {
                          rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MAC_RR_EST);
                        }
                      }

                      // Take a local copy of the RR_EST_REQ
                      grr_control_data_ptr->saved_rr_est_req = (*(rr_est_req_T *)new_message);
                      grr_control_data_ptr->saved_rr_est_req_valid = TRUE;

                      // Discard the copy saved in RR-CONTROL
                      grr_throwaway_saved_message();
                    }
                    else if (grr_event == EV_FORCE_CELL_RESELECTION)
                    {
                      /* Reject request back to MM, and discard saved message */
                      rr_send_abort_ind(REASON_INTERRAT_RESELECTION, MM_RR_OTHER_FAILURE);
                      grr_throwaway_saved_message();
                    }
                    break;
                  } /* RR_EST_REQ */
#endif /* FEATURE_GERAN_CS_SUPPORT */

                  case RR_PLMN_SELECT_REQ:
                  case RR_DEACT_REQ:
                  {
                    rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
                    break;
                  }

                  case RR_PLMN_LIST_REQ:
                  {
                    rr_plmn_list_req_T *rr_plmn_list_req = (rr_plmn_list_req_T *)new_message;
                    new_grr_control_event = handle_rr_plmn_list_req(
                      rr_plmn_list_req
                    );
                    break;
                  } /* RR_PLMN_LIST_REQ */

                  case RR_STOP_GSM_MODE_REQ:
                  {
                    if (new_message->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_UE_SGLTE_MODE_SWITCH )
                    {
                      rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
                    }
                    else
                    {
                      new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                    }

                    break;
                  }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q                  
                  case RR_PSEUDO_LTE_LIST_UPDATE_IND:
                  {
                    /* Pseudo List Update will be Ignored during CELL_RESELECTION_G2W */
                    break;
                  }
#endif
                  default:
                  {
                    new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                  }
                }
                break;
              } /* MS_MM_RR */

              case MS_RR_L2:
              {
                switch (message_id)
                {
                  case DL_UNIT_DATA_IND:
                  {
                    new_grr_control_event =
                      rr_connection_establishment_control(EV_INPUT_MESSAGE, new_message);
                    break;
                  } /* DL_UNIT_DATA_IND */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message,
                      get_grr_control_state_name(grr_control_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_L2 */

              case MS_MAC_RR:
              {
                switch (message_id)
                {
                  case MAC_GRR_MAC_STATUS:
                  {
                    mac_grr_mac_status_t *mac_status = (mac_grr_mac_status_t *) new_message;
                    if (mac_status->mac_status == MAC_IDLE)
                    {
                      new_grr_control_event = rr_resel_control(EV_MAC_IDLE_MODE, NULL);
                    }
                    break;
                  } /* MAC_GRR_MAC_STATUS */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message,
                      get_grr_control_state_name(grr_control_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_MAC_RR */

              case MS_TIMER:
              {
                switch (new_message->mid_timer_expiry.timer_id)
                {
                  case T3122:
                  {
                    // no action - don't update NAS during G2W. The next RR_SERVICE_IND/RR_PLMN_SELECT_CNF
                    // will contain the correct value
                    MSG_GERAN_LOW_0_G("T3122 expired - notify NAS at next service status update");
                    break;
                  }

                  default:
                  {
                    new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                  }
                }

                break;
              }

              default:
              {
                new_grr_control_event = rr_resel_control(EV_INPUT_MESSAGE, new_message);
                break;
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED:
          {
            /* No need to change idle mode, as L1 is not camped during reselection
            anyway.  The new idle mode will be applied correctly if necessary after
            reselection failure if it occurs. */
            MSG_GERAN_LOW_0_G("Ignoring RR_EV_L1_IDLE_MODE_CHANGE_REQUIRED during G2W reselection");
            break;
          }

          case RR_EV_MM_REQ_RECEIVED_ABORT:
          {
            new_grr_control_event = rr_resel_control(grr_control_event, NULL);
            break;
          }

          case EV_GSM_MODE_ACTIVATED:
          case EV_GSM_MODE_DEACTIVATED:
          {
            output_event = rr_resel_control(grr_control_event, NULL);
            break;
          }

          case EV_CELL_RESELECTION:
          {
            grr_control_data_ptr->state = GRR_CELL_RESELECTION;
            break;
          }

          case EV_MAC_GUARD_TIMER_EXPIRED:
          {
            new_grr_control_event = rr_resel_control(EV_MAC_GUARD_TIMER_EXPIRED, NULL);
            break;
          }

#ifdef FEATURE_GERAN_CS_SUPPORT
          case EV_RR_EST_REQ:
          {
            grr_control_data_ptr->state = GRR_CAMPED;
            new_grr_control_event = EV_RR_EST_REQ;
            break;
          }
#endif /* FEATURE_GERAN_CS_SUPPORT */

          case EV_PROCESS_SAVED_MESSAGE:
          {
            if (!rr_is_camped())
            {
              grr_control_data_ptr->state = GRR_NULL;

              output_event = relinquish_control_to_rr(grr_control_event);
            }
            else
            {
              grr_control_data_ptr->state = transition_to_camped_state(
                FALSE,                              // service_ind_sent
                &new_grr_control_event,
                (rr_cmd_bdy_type **)&new_message,
                &message_set,
                &message_id
              );
            }
            break;
          }

#ifdef FEATURE_INTERRAT_PCCO_GTOW
          case RR_EV_G2WR_GO_TO_INACTIVE:
          {
            grr_control_data_ptr->state = GRR_NULL;
            rr_gprs_deactivate_timed_sys_info_refresh();
            output_event = RR_EV_G2WR_GO_TO_INACTIVE;
            break;
          }
#endif /* FEATURE_INTERRAT_PCCO_GTOW */

          case EV_CR_G2W_TIMER_EXPIRED:
          {
            MSG_GERAN_LOW_0_G("Ignoring G2W timer expiry in GRR_CELL_RESELECTION_G2W state");
            break;
          }

          case EV_MAC_IDLE_MODE:
          {
            new_grr_control_event = rr_resel_control(grr_control_event, NULL);
            break;
          }

          case RR_EV_EARLY_CAMP_SI_RECEIVED:
          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          case RR_EV_GSM_SI_RECEIVED:
          case RR_EV_GPRS_SI13_RECEIVED:
          {
            /* Ignore legacy events */
            break;
          }

          case EV_CELL_RESELECTION_ACT:
          case EV_CELL_RESELECTION_DEACT:
          {
            (void) rr_resel_control(grr_control_event, NULL);

            send_rr_service_ind();

            grr_control_data_ptr->state = transition_to_camped_state(
              TRUE,                               // service_ind_sent
              &new_grr_control_event,
              (rr_cmd_bdy_type **) &new_message,
              &message_set,
              &message_id
            );
            break;
          } /* EV_CELL_RESELECTION_ACT ... */

          case EV_PANIC_RESET:
          {
            break;  // No specific action required
          }

          default:
          {
            new_grr_control_event = rr_resel_control(grr_control_event, NULL);
            break;
          }
        }
        break;
      } /* GRR_CELL_RESELECTION_G2W */
#endif
#ifdef FEATURE_GPRS_PS_HANDOVER
      case GRR_PSHO:
      {
        /*
        TODO - following messages are not handled:
          RR_GRR_IMSG_MSC_CHANGE_IND
          RR_GRR_IMSG_PS_ACCESS_REQ
          RR_GRR_IMSG_NO_PS_ACCESS_REQ
        */
        if (grr_control_event == EV_INPUT_MESSAGE)
        {
          if ((message_set == MS_MM_RR) &&
              (((mm_rr_message_id_enum_T) message_id == RR_DEACT_REQ) ||
               ((mm_rr_message_id_enum_T) message_id == RR_STOP_GSM_MODE_REQ) ||
               ((mm_rr_message_id_enum_T) message_id == RR_PLMN_SELECT_REQ) ||
               ((mm_rr_message_id_enum_T) message_id == RR_PLMN_LIST_REQ) ||
#ifdef FEATURE_GERAN_CS_SUPPORT
               ((mm_rr_message_id_enum_T) message_id == RR_EST_REQ)
#endif /* FEATURE_GERAN_CS_SUPPORT */
              ))
          {
            MSG_GERAN_HIGH_1_G("PSHO: NAS message received %d", message_id);
            (void)rr_save_msg(new_message);
          }
          else if (message_set == MS_TIMER)
          {
            switch (new_message->mid_timer_expiry.timer_id)
            {
              case T3122:
              {
                // Indicate to NAS-MM that T3122 has expired via a RR_SERVICE_IND
                grr_update_mm_with_rr_service_ind();
                break;
              }

              default:
              {
                new_grr_control_event = rr_psho_control(grr_control_event, new_message);
              }
            }
          }
          else
          {
            new_grr_control_event = rr_psho_control(grr_control_event, new_message);
          }
        }
        else if (grr_control_event == EV_PSHO_CONTINUE)
        {
          /* PSHO procedure continue, after got  happen */
          rr_send_rr_reselection_ind();
          rr_gprs_deactivate_timed_sys_info_refresh();

          (void) rr_resel_control(EV_CELL_RESELECTION_DEACT, NULL);

          (void) rr_gprs_nc_measurement_control(RR_EV_DEACTIVATE_MEASUREMENTS, NULL);
        }
        else if (grr_control_event == EV_PSHO_COMPLETE)
        {
          /* PSHO procedure terminated, either failure or success
           * necessary failure or success handling is done in psho_control.
           * just move GRR back to GRR_CAMPED
           */
          new_grr_control_event = process_saved_message(
            (rr_cmd_bdy_type **)&new_message,
            &message_set,
            &message_id
          );

          if (((new_grr_control_event) == EV_INPUT_MESSAGE) &&
              ((new_message) != NULL))
          {
            /* If the saved message is MM->RR */
            if (message_set != MS_MM_RR)
            {
              grr_throwaway_saved_message();
            }
          }

          grr_control_data_ptr->state = GRR_CAMPED;
        }
        /* other side events generated during PSHO */
        else if ((grr_control_event == RR_EV_MEASUREMENT_PARAMS_UPDATED) ||
                 (grr_control_event == RR_EV_GPRS_SI13_RECEIVED))
        {
          ;
        }
        else if (grr_control_event == EV_PANIC_RESET)
        {
          new_grr_control_event = rr_psho_control(EV_RESET_SOFTWARE, NULL);
        }
        else
        {
          new_grr_control_event = rr_psho_control(grr_control_event, new_message);
        }
        break;
      } /* GRR_PSHO */

#endif /* FEATURE_GPRS_PS_HANDOVER */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
      case GRR_X2G_CCO_FAILED:
      {
        ASSERT(grr_control_data_ptr->x2g_cco_status != RR_CCO_STATUS_UNDEFINED);
        switch (grr_control_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (new_message->rr.header.rr_message_set)
                {
                  case RR_GRR_IMSG:
                  {
                    switch (new_message->rr.header.imh.message_id)
                    {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
                      case RR_GRR_IMSG_MSC_CHANGE_IND:
                      {
                        /*
                        Update NAS as required
                        RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                        This will be sent when G is next active
                        */
                        rr_msc_update(
                          FALSE   // can_send_service_ind
                        );
                        break;
                      }
#endif

                      case RR_GRR_IMSG_PS_ACCESS_REQ:
                      {
                        // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                        rr_grr_imsg_ps_access_req_t *imsg = &new_message->rr.grr.imsg.ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn);
                        break;
                      }

                      case RR_GRR_IMSG_NO_PS_ACCESS_REQ:
                      {
                        // No PS access request (could be TLLI unassignment, CS-only)

                        rr_grr_imsg_no_ps_access_req_t *imsg = &new_message->rr.grr.imsg.no_ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn);
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(new_message, "GRR_X2G_CCO_FAILED");
                      }
                    }
                    break;
                  }

                  case RR_PS_IMSG:
                  {
                    rr_ps_access_control(new_message);
                    break;
                  }

                  case RR_PS_OMSG:
                  {
                    switch (new_message->rr.header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        /* TODO: Still undecided on whether RR or GRR should stop L1.
                        Leave it with GRR for the time being, although this may change. */
                        //new_grr_control_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL);
                        new_grr_control_event = EV_GSM_MODE_DEACTIVATED;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(new_message, "GRR_X2G_CCO_FAILED");
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message, "GRR_X2G_CCO_FAILED");
                  }
                }
                break;
              }

              case MS_MAC_RR:
              {
                /* Any messages from MAC may be ignored as MAC has been sent a
                GRR_MAC_NO_PS_ACCESS_IND. */
                break;
              } /* MS_MAC_RR */

              case MS_MM_RR:
              {
                switch (message_id)
                {
#ifdef FEATURE_GERAN_CS_SUPPORT
                  case RR_EST_REQ:
                  {
                    grr_throwaway_saved_message();
                    rr_send_abort_ind(RR_ABORT_CON_EST_NOT_ATTEMPTED, MM_RR_OTHER_FAILURE);
                    break;
                  }
#endif /* FEATURE_GERAN_CS_SUPPORT */
                }
                break;
              } /* MS_MM_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(new_message, "GRR_X2G_CCO_FAILED");
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_GSM_MODE_DEACTIVATED:
          {
            rr_ps_no_ps_access_del_all();
            rr_gprs_deactivate_timed_sys_info_refresh();
            rr_grr_send_omsg_x2g_cco_status_ind(grr_control_data_ptr->x2g_cco_status);
            grr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
            grr_control_data_ptr->state = GRR_NULL;
            break;
          } /* EV_GSM_MODE_DEACTIVATED */

          default:
          {
            LOG_UNEXPECTED_EVENT(
              rr_event_name(grr_control_event),
              "GRR_X2G_CCO_FAILED"
            );
          }
        }
        break;
      } /* GRR_X2G_CCO_FAILED */

      /* X2G CCO is in progress and a high priority request has been received
      from NAS.  RR has sent a signal to the originating RAT indicating that
      the CCO procedure is terminated, and is waiting for a response before
      continuing to process the NAS request. */
      case GRR_X2G_CCO_ABORTED:
      {
        switch (grr_control_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (new_message->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (new_message->rr.header.rr_message_set)
                {
                  case RR_GRR_IMSG:
                  {
                    switch (new_message->rr.header.imh.message_id)
                    {
                      case RR_GRR_IMSG_X2G_CCO_COMPLETE_IND:
                      {
                        MSG_GERAN_HIGH_0_G("X2G CCO procedure completed early, process saved message");

                        if (grr_verify_saved_mm_message(RR_PLMN_SELECT_REQ))
                        {
                          rr_cmd_bdy_type *saved_message;

                          if (rr_examine_saved_message(&saved_message) == EV_INPUT_MESSAGE)
                          {
                            new_grr_control_event = handle_rr_plmn_select_req(
                              &saved_message->rr_plmn_select_req
                            );
                            grr_control_data_ptr->state = GRR_CAMPED;
                          }
                          else
                          {
                            /* Shouldn't happen, but this should recover things if the
                            saved message is not an EV_INPUT_MESSAGE. */
                            rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
                            grr_control_data_ptr->state = GRR_CAMPED;
                          }
                        }
                        else
                        {
                          /* TODO: Find a neater way of doing this - this relies on normal
                          GRR_CAMPED processing of a saved message, where rr_resel_control()
                          is always aborted before processing the saved message.  The saved
                          message is processed in the RR_RESEL_OMSG_ABORT_CNF handler in
                          GRR_CAMPED state.  This code should be made a function that can
                          be called from multiple pseudo-camped states if need be. */
                          rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
                          grr_control_data_ptr->state = GRR_CAMPED;
                        }
                        break;
                      } /* RR_GRR_IMSG_X2G_CCO_COMPLETE_IND */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
                      case RR_GRR_IMSG_MSC_CHANGE_IND:
                      {
                        /*
                        Update NAS as required
                        RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                        This will be sent when G is next active
                        */
                        rr_msc_update(
                          FALSE   // can_send_service_ind
                        );
                        break;
                      }
#endif

                      case RR_GRR_IMSG_PS_ACCESS_REQ:
                      {
                        // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                        rr_grr_imsg_ps_access_req_t *imsg = &new_message->rr.grr.imsg.ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn);
                        break;
                      }

                      case RR_GRR_IMSG_NO_PS_ACCESS_REQ:
                      {
                        // No PS access request (could be TLLI unassignment, CS-only)

                        rr_grr_imsg_no_ps_access_req_t *imsg = &new_message->rr.grr.imsg.no_ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn);
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          new_message,
                          get_grr_control_state_name(grr_control_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_GRR_IMSG */

                  case RR_RESEL_IMSG:
                  {
                    (void) rr_resel_control(EV_INPUT_MESSAGE, new_message);
                    break;
                  } /* RR_RESEL_IMSG */

                  case RR_RESEL_OMSG:
                  {
                    switch (new_message->rr.header.imh.message_id)
                    {
                      case RR_RESEL_OMSG_ABORT_CNF:
                      {
                        break;
                      } /* RR_RESEL_OMSG_ABORT_CNF */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          new_message,
                          get_grr_control_state_name(grr_control_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_RESEL_OMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message,
                      get_grr_control_state_name(grr_control_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  new_message,
                  get_grr_control_state_name(grr_control_data_ptr->state)
                );
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          default:
          {
            LOG_UNEXPECTED_EVENT(
              rr_event_name(grr_control_event),
              get_grr_control_state_name(grr_control_data_ptr->state)
            );
          }
        }
        break;
      } /* GRR_X2G_CCO_ABORTED */

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
      case GRR_CAMPED_MSC_CHANGE:
      {
        if (grr_control_event == EV_INPUT_MESSAGE)
        {
          switch (message_set)
          {
            case MS_RR_RR:
            {
              switch (new_message->rr.header.rr_message_set)
              {
                case RR_GRR_IMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_GRR_IMSG_PAGE_IND:
                    {
                      rr_grr_imsg_page_ind_t *imsg = &new_message->rr.grr.imsg.page_ind;

                      // Forward the page to NAS
                      rr_ce_send_page_to_nas(&imsg->page_info);
                      break;
                    }

                    case RR_GRR_IMSG_PAGE_MODE_IND:
                    {
                      // Ignore a change in OTA Page Mode as L1 may not be able to process an update.
                      // The change will again be indicated next time the paging block is read
                      MSG_GERAN_LOW_0_G("Change in Page Mode ignored");
                      break;
                    }

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                }

                case RR_L1_IDLE_IMSG:
                {
                  rr_l1_idle_control(new_message);
                  break;
                }

                case RR_L1_IDLE_OMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                    {
                      rr_ps_send_imsg_no_ps_access_req(
                        FALSE,    // suspend_gmm
                        NO_DEL   // delete_pdu
                      );
                      break;
                    }

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                }

                case RR_PS_IMSG:
                {
                  rr_ps_access_control(new_message);
                  break;
                }

                case RR_PS_OMSG:
                {
                  switch (new_message->rr.header.imh.message_id)
                  {
                    case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                    {
                      // Update NAS as required
                      // RR is camped, so a RR_SERVICE_IND can be sent to MM if required
                      rr_msc_update(
                        TRUE    // can_send_service_ind
                      );

                      grr_control_data_ptr->state = GRR_CAMPED;
                      break;
                    }

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message,
                        get_grr_control_state_name(grr_control_data_ptr->state)
                      );
                    }
                  }
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }
              break;
            }

            case MS_MAC_RR:
            {
              switch (message_id)
              {
                // Soak-up messages which can be received in this state but can be ignored
                case MAC_GRR_MAC_STATUS:
                case MAC_GRR_RACH_CNF:
                {
                  break;  // no action required
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message,
                    get_grr_control_state_name(grr_control_data_ptr->state)
                  );
                }
              }
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(
                new_message,
                get_grr_control_state_name(grr_control_data_ptr->state)
              );
            }
          }
        }

        else
        /******************************************************************/
        /* EV_TBF_CONN_EST_ABORTED_CAMPED                                  */
        /* The TBF conn est has been aborted. RACHing had not started yet */
        /* so L1 is still in Packet Idle                                  */
        /******************************************************************/
        if ( grr_control_event == EV_TBF_CONN_EST_ABORTED_CAMPED )
        {
          rr_ps_send_imsg_no_ps_access_req(
            FALSE,    // suspend_gmm
            NO_DEL   // delete_pdu
          );
        }

        else
        /******************************************************************/
        /* EV_TBF_CONN_EST_ABORTED_NON_CAMPED                             */
        /* The TBF conn est has been aborted. RACHing had started, so L1  */
        /* is no longer camped (i.e. in RACH state)                       */
        /******************************************************************/
        if ( grr_control_event == EV_TBF_CONN_EST_ABORTED_NON_CAMPED )
        {
          // Send request to start L1 Idle Mode
          rr_l1_idle_send_imsg_start_idle_req();
        }

        else
        if (grr_control_event == EV_PANIC_RESET)
        {
          ;  // No specific action required
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(grr_control_event),
                               get_grr_control_state_name(grr_control_data_ptr->state));
        }

        break;
      }
#endif /* FEATURE_DUAL_SIM || FEATURE_RR_THERMAL_MITIGATION */

#ifdef FEATURE_SGLTE
      case GRR_SGLTE_SWITCHING_TO_GSM_ONLY:
      {
        switch (grr_control_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_L1:
              {
                switch (message_id)
                {
                  case MPH_UE_MODE_CHANGE_CNF:
                  {
                    rr_ps_sglte_handle_ps_removal(grr_control_data_ptr->sglte_ps_removal_cb_fn);

                    grr_control_data_ptr->sglte_ps_removal_cb_fn = NULL;

                    /* Cancel any IRAT reselection timers that are running. */
                    rr_g2wr_reset_wcdma_surrounding_database();

                    grr_control_data_ptr->is_sglte_user_rat_change_pending = FALSE;
                    /* Switch back to camped state to handle PS deactivation. */
                    grr_control_data_ptr->state = GRR_CAMPED;
                    break;
                  } /* MPH_UE_MODE_CHANGE_CNF */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message,
                      get_grr_control_state_name(grr_control_data_ptr->state)
                    );
                  }
                }
                break;
              } /* MS_RR_L1 */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  new_message,
                  get_grr_control_state_name(grr_control_data_ptr->state)
                );
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(grr_control_event),
                                 get_grr_control_state_name(grr_control_data_ptr->state));
          }
        }
        break;
      } /* GRR_SGLTE_SWITCHING_TO_GSM_ONLY */

      case GRR_SGLTE_X2G_RESEL:
      {
        switch (grr_control_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (new_message->rr.header.rr_message_set)
                {
                  case RR_SGLTE_X2G_OMSG:
                  {
                    switch (message_id)
                    {
                      case RR_SGLTE_X2G_OMSG_RESEL_CNF:
                      {
                        sys_srv_domain_e_type old_service_domain = rr_sel_get_service_domain();

                        rr_sglte_x2g_deinit();
						
                        rr_store_service_domain(SYS_SRV_DOMAIN_CS_PS);

                        if (rr_gsm_only())
                        {
                          MSG_GERAN_ERROR_0_G("GSM-only = TRUE after SGLTE X2G resel, error?");
                          /* Cancel any IRAT reselection timers that are running. */
                          rr_g2wr_reset_wcdma_surrounding_database();
                        }
                        else
                        {
                          /* Update L1 with IRAT neighbour list. */
                          rr_l1_update_wcdma_neighbor_list();
                        }

                        if (old_service_domain != SYS_SRV_DOMAIN_CS_PS)
                        {
                          rr_ps_handle_rr_change_mode_ind_service_domain_changed(
                            old_service_domain,
                            rr_sel_get_service_domain()
                          );
                        }
                        else
                        {
                          /* Finally, inform NAS of current service status. */
                          grr_update_mm_with_rr_service_ind();
                        }

                        rr_grr_send_omsg(RR_GRR_OMSG_SGLTE_X2G_CNF);

                        grr_control_data_ptr->state = GRR_CAMPED;
                        break;
                      } /* RR_SGLTE_X2G_OMSG_RESEL_CNF */

                      case RR_SGLTE_X2G_OMSG_ABORT_CNF:
                      {
                        /* Need to return to previous CS-only GSM-only configuration. */
                        grr_control_data_ptr->no_ps_access_confim_cb_fn = grr_send_omsg_sglte_x2g_abort_cnf_cb_fn;
                        send_ps_imsg_no_ps_access_req(
                          FALSE // suspend_gmm
                        );
                        grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;
                        break;
                      } /* RR_SGLTE_X2G_OMSG_ABORT_CNF */
					  
                      case RR_SGLTE_X2G_OMSG_REJECT_CNF:
                      {
                        /* Need to return to previous CS-only GSM-only configuration. */
						
						rr_sglte_x2g_deinit();
                        grr_control_data_ptr->no_ps_access_confim_cb_fn = grr_send_omsg_sglte_x2g_reject_ind_cb_fn;
                        send_ps_imsg_no_ps_access_req(
					      FALSE // suspend_gmm
						);
						grr_control_data_ptr->state = GRR_CAMPED_DISABLING_PS_ACCESS;					  	
                        break;					  	
                      } /*RR_SGLTE_X2G_OMSG_REJECT_CNF*/

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          new_message,
                          get_grr_control_state_name(grr_control_data_ptr->state)
                        );
                      }
                    }
                    break;
                  } /* RR_SGLTE_X2G_OMSG */

                  case RR_GRR_IMSG:
                  {
                    switch (message_id)
                    {
                      case RR_GRR_IMSG_SGLTE_X2G_ABORT_REQ:
                      {
                        rr_sglte_x2g_send_imsg(RR_SGLTE_X2G_IMSG_ABORT_REQ);
                        break;
                      } /* RR_GRR_IMSG_SGLTE_X2G_ABORT_REQ */

                      case RR_GRR_IMSG_PS_ACCESS_REQ:
                      {
                        rr_grr_imsg_ps_access_req_t *imsg = &new_message->rr.grr.imsg.ps_access_req;

                        grr_control_data_ptr->ps_access_confim_cb_fn = imsg->confirm_cb_fn;

                        send_ps_imsg_ps_access_req(
                          imsg->purge_gmm_signalling    // purge_gmm_signalling
                        );

                        break;
                      } /* RR_GRR_IMSG_PS_ACCESS_REQ */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(new_message, "GRR_SGLTE_X2G_RESEL");
                      }
                    }
                    break;
                  } /* RR_GRR_IMSG */

                  case RR_PS_IMSG:
                  {
                    rr_ps_access_control(new_message);
                    break;
                  } /* RR_PS_IMSG */

                  case RR_PS_OMSG:
                  {
                    switch (new_message->rr.header.imh.message_id)
                    {
                      case RR_PS_OMSG_PS_ACCESS_CNF:
                      {
                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(grr_control_data_ptr->ps_access_confim_cb_fn);
                        grr_control_data_ptr->ps_access_confim_cb_fn = NULL;

                        rr_send_imsg(RR_IMSG_PS_ACCESS_IND);
                        break;
                      } /* RR_PS_OMSG_PS_ACCESS_CNF */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          new_message,
                          get_grr_control_state_name(grr_control_data_ptr->state)
                        );
                      }
                    }
                    break;
                  }

                  default:
                  {
                    rr_sglte_x2g_control(new_message);
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_MM_RR:
              {
                switch (new_message->message_header.message_id)
                {
                  case RR_GMM_GPRS_STATE_CHANGE_REQ:
                  {
                    (void) rr_gprs_nc_measurement_control(EV_INPUT_MESSAGE, new_message);
                    break;
                  } /* RR_GMM_GPRS_STATE_CHANGE_REQ */
                  case RR_PLMN_SELECT_REQ:
                  case RR_DEACT_REQ:
                  case RR_PLMN_LIST_REQ:
				  case RR_STOP_GSM_MODE_REQ:
                  {
                    rr_sglte_x2g_send_imsg(RR_SGLTE_X2G_IMSG_REJECT_REQ);				  	
                    break;				  	
                  }              				  

                  default:
                  {
                    rr_sglte_x2g_control(new_message);
                  }
                }
                break;
              } /* MS_MM_RR */

              default:
              {
                rr_sglte_x2g_control(new_message);
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(grr_control_event),
                                 get_grr_control_state_name(grr_control_data_ptr->state));
          }
        }
        break;
      } /* GRR_SGLTE_X2G_RESEL */
#endif /* FEATURE_SGLTE */

      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* ERROR CASE: !!!!!! Unexpected state !!!!!!                     */
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      default:
      {
        if (grr_control_event == EV_INPUT_MESSAGE)
        {
          LOG_UNEXPECTED_MESSAGE(
            new_message,
            get_grr_control_state_name(grr_control_data_ptr->state)
          );
        }
        else
        {
          LOG_UNEXPECTED_EVENT(
            rr_event_name(grr_control_event),
            get_grr_control_state_name(grr_control_data_ptr->state)
          );
        }
      }
    }

#ifdef DEBUG_RR_TRACE_MSG
    if (((grr_control_event != new_grr_control_event) &&
         (new_grr_control_event != EV_NO_EVENT)) ||
        (grr_control_data_ptr->old_state != grr_control_data_ptr->state))
    {
      rr_store_trace_msg_buf(
        GRR_CONTROL_SM,
        new_grr_control_event,
        (byte)grr_control_data_ptr->state,
        new_message
      );
    }
#endif

    /* Look for panic reset first */
    /* Note: This will be in grr_control_event, not new_grr_control_event */
    if (grr_control_event == EV_PANIC_RESET)
    {
#ifdef FEATURE_GERAN_CS_SUPPORT
      if (rr_verify_panic_reset_saved_msg(MS_MM_RR, (uint8) RR_EST_REQ))
      {
        /* Inform MM that a connection failure occurred */
        rr_send_abort_ind(RR_CONNECTION_FAILURE, MM_RR_OTHER_FAILURE);

        /* Remove the saved RR_EST_REQ */
        rr_save_msg_for_recovery_after_panic_reset(NULL );
      }
      /* Move saved RR_STOP_GSM_MODE_REQ to storage for panic reset recovery handling. */
      else 
#endif /* FEATURE_GERAN_CS_SUPPORT */
      {
        rr_cmd_bdy_type * saved_message_ptr;
        
        if (rr_examine_saved_message(&saved_message_ptr) == EV_INPUT_MESSAGE )
        {
          IMH_T * message_header_ptr = (IMH_T *) saved_message_ptr;
        
          if ( ( message_header_ptr->message_set == MS_MM_RR ) &&
               ( message_header_ptr->message_id == (uint8) RR_STOP_GSM_MODE_REQ ) )
          {
            rr_save_msg_for_recovery_after_panic_reset(saved_message_ptr);
            MSG_GERAN_MED_0_G("RR_STOP_GSM_MODE_REQ is stored for panic reset recovery handling");
          }
        }
      }

      /* For all panic reset situation, throwaway any saved messages, as if a MM */
      /* message was pending at the time of the panic reset, this will have been */
      /* stored in the panic reset saved message store */
      grr_throwaway_saved_message();

      grr_control_event = EV_NO_EVENT;
      output_event = EV_NO_EVENT;
    }
    else
    if (new_grr_control_event != EV_NO_EVENT)
    {
      grr_control_event = new_grr_control_event;
      new_grr_control_event = EV_NO_EVENT;
    }
    else
    {
      grr_control_event = EV_NO_EVENT;
    }

    if (grr_control_data_ptr->state != grr_control_data_ptr->old_state)
    {
      RR_LOG_STATE_TRANSITION(
        "grr_control",
        grr_control_data_ptr->old_state,
        grr_control_data_ptr->state,
        get_grr_control_state_name
      );
#ifndef FEATURE_GERAN_REDUCED_DEBUG
      (void) rr_log_gprs_grr_state(grr_control_data_ptr->state);
#endif
      /* If the old state is a transitory state, cancel the state transition
      guard timer. */
      if (grr_control_state_is_transitory(grr_control_data_ptr->old_state))
      {
        MSG_GERAN_MED_1_G("Cancelling state transition guard timer (left state %d)",
                grr_control_data_ptr->old_state);
        rr_timer_cancel(RR_GRR_CONTROL_STATE_TRANSITION_GUARD_TIMER);
      }

      /* If the new state is a transitory state, start the state transition
      guard timer. */
      if (grr_control_state_is_transitory(grr_control_data_ptr->state))
      {
        MSG_GERAN_MED_2_G("Starting state transition guard timer (entered state %d, duration %d ms)",
                grr_control_data_ptr->state, RR_GRR_CONTROL_STATE_TRANSITION_TIMEOUT);
        rr_timer_start(
          RR_GRR_CONTROL_STATE_TRANSITION_GUARD_TIMER,
          RR_GRR_CONTROL_STATE_TRANSITION_TIMEOUT
        );
      }

      grr_control_data_ptr->old_state = grr_control_data_ptr->state;
    }
    else if (grr_control_data_ptr->state == GRR_NULL)
    {
      /* If this state machine is reset and the old_state variable is cleared
      while the state transition guard timer is running, the timer will not be
      cancelled by the above code, as no state transition will be detected.  To
      avoid this false detection of an error, we explicitly check for the guard
      timer running if the current state is GRR_NULL (which should never be the
      case) and cancel the timer if necessary. */
      if (rr_timer_is_running(RR_GRR_CONTROL_STATE_TRANSITION_GUARD_TIMER))
      {
        MSG_GERAN_MED_1_G("Cancelling state transition guard timer (state %d)",
                grr_control_data_ptr->state);
        rr_timer_cancel(RR_GRR_CONTROL_STATE_TRANSITION_GUARD_TIMER);
      }
    }
  }

  return output_event;
}

/**
  @brief Returns the current GRR control state name as a string.
*/
const char *grr_control_current_state_name(void)
{
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();

  if (grr_control_data_ptr != NULL)
  {
    return get_grr_control_state_name(grr_control_data_ptr->state);
  }

  return get_grr_control_state_name(GRR_NULL);
}


/****************************************************************************
 *   Function name:  rr_gprs_camped_timer_handler
 *   -------------
 *
 *   Description: handles the timer expiries for camped state in both GRR51 and GRR52
 *   ------------ The timer ID is extracted and the expiry forwarded to the relevant
 *                function/fsm
 *
 *   Parameters: timer_expiry_T *timer_expiry
 *   -----------
 *
 *   Returns:
 *   --------
 *
 ****************************************************************************/
static rr_event_T rr_gprs_camped_timer_handler(
  rr_cmd_bdy_type *message
   
)
{
  rr_event_T output_event = EV_NO_EVENT;

  switch(message->mid_timer_expiry.timer_id)
  {
    case PATH_LOSS_TIMER:
      /*Path loss timer has expired notify cell reselection*/
      output_event = rr_resel_control(EV_PATH_LOSS_TIMEOUT, NULL);
      break;

    case T3158:
      /*Pass the nc measurement report into the measurement report controller*/
      output_event = rr_gprs_nc_measurement_control(EV_T3158_TIMEOUT, NULL);
      break;

    case T3174:
      /*The pcco timer t3174 has expired notify cell reselection*/
      output_event = EV_T3174_TIMEOUT;
      break;

    case T3176:
      /*The pcco timer t3176 has expired notify cell reselection*/
      output_event = EV_T3176_TIMEOUT;
      break;

    case TMULTIFRM:
      /* Timer for 2nd of a 2 part IA msg */
      output_event = rr_connection_establishment_control(EV_TMULTIFRM_EXPIRY, NULL);
      break;

    case T3122:
      // Indicate to NAS-MM that T3122 has expired via a RR_SERVICE_IND
      grr_update_mm_with_rr_service_ind();
      break;

    case RR_GPRS_SYS_INFO_TIMER:
      output_event = rr_resel_control(EV_INPUT_MESSAGE, message);
      break;

    case T3208:
    {
      if (rr_gprs_nacc_type() == NACC_GSM_GSM)
      {
        /* reset NACC in GRR/MAC: will cause context switch */
        rr_gprs_nacc_reset();
      }
      output_event = rr_resel_control(EV_T3208_EXPIRY, NULL);
      break;
    }

    case T3210:
    {
      output_event = rr_resel_control(EV_T3210_EXPIRY, NULL);
      break;
    }

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 
    case RR_RMC_CCCH_READ_TIMER:
      output_event = rr_resel_control(EV_INPUT_MESSAGE, message);
      break;
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

    default:
      MSG_GERAN_HIGH_1_G("GRR TIMER (camped) not known - TimerID = %d", message->mid_timer_expiry.timer_id);
      break;
  }

  return output_event;
}

/****************************************************************************
 *   Function name:  rr_gprs_reselection_timer_handler
 *   -------------
 *
 *   Description: handles the timer expiries for reselection state in both GRR51 and GRR52
 *   ------------ The timer ID is extracted and the expiry forwarded to the relevant
 *                function/fsm
 *
 *   Parameters: timer_expiry_T *timer_expiry
 *   -----------
 *
 *   Returns:
 *   --------
 *
 ****************************************************************************/
rr_event_T rr_gprs_reselection_timer_handler(timer_expiry_T *timer_expiry )
{
rr_event_T output_event = EV_NO_EVENT;

  switch(timer_expiry->timer_id)
  {
    case T3174:
    {
      /*The pcco timer t3174 has expired notify cell reselection*/
      output_event = EV_T3174_TIMEOUT;
      break;
    }

    case T3176:
    {
      /*The pcco timer t3176 has expired notify cell reselection*/
      output_event = EV_T3176_TIMEOUT;
      break;
    }

    case T3208:
    {
      if (rr_gprs_nacc_type() == NACC_GSM_GSM)
      {
        /* reset NACC in GRR/MAC: will cause context switch */
        rr_gprs_nacc_reset();
      }
      output_event = rr_resel_control(EV_T3208_EXPIRY, NULL);
      break;
    }

    case T3210:
    {
      output_event = rr_resel_control(EV_T3210_EXPIRY, NULL);
      break;
    }

    case T3122:
      // no action required - NAS-MM will get notified when the next RR_SERVICE_IND is sent
      MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
      break;

    default:
    {
      MSG_GERAN_HIGH_1_G("GRR TIMER unknown - TimerID = %d", timer_expiry->timer_id);
      break;
    }
  }

  return output_event;
}

/****************************************************************************
 *   Function name:  get_grr_state
 *   -------------
 *
 *   Description: Returns the current state of the GRR51 FSM
 *   ------------
 *
 *
 *   Parameters: none
 *   -----------
 *
 *   Returns:  state
 *   --------
 *
 ****************************************************************************/
grr_state_t get_grr_state( )
{
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();

  if (grr_control_data_ptr != NULL)
  {
    return grr_control_data_ptr->state;
  }

  return GRR_NULL;
}


#ifdef FEATURE_GERAN_CS_SUPPORT
/*===========================================================================

FUNCTION handle_mac_grr_paging_notification

DESCRIPTION
  Called when a page is received from MAC.
  This could originate from the Packet Paging Channel if the MS is in Packet Idle (52 only),
  or from the PACCH if in Packet Transfer  (51 or 52).
  A RR_PAGING_IND is sent to MM.

PARAMS
  paging_notification - the paging notification message from MAC

RETURN VALUE
  None

===========================================================================*/

static void handle_mac_grr_paging_notification(
  mac_grr_paging_notification_t *paging_notification
   
)
{
  rr_page_info_t page_info = {0};

  page_info.domain = RR_PAGE_DOMAIN_NONE;
  page_info.identity_type = RR_IDENTITY_TYPE_NONE;

  switch (paging_notification->paging_type)
  {
    /* CS Paging:                                         */
    /*   Store the CS page details, and pass page onto MM */
    case RR_CS_PAGING:
    {
      boolean bValid = TRUE;

      page_info.domain = RR_PAGE_DOMAIN_CS;

      /* store the identity used in the page
       * MAC tells us which type (IMSI/TMSI) was used
       * but does not supply the actual identity
       */
      if (IMSI == paging_notification->identity_type)
      {
        page_info.identity_type = RR_IDENTITY_TYPE_IMSI;

        bValid = rr_siminfo_get_imsi(page_info.identity,sizeof(page_info.identity));
      }
      else if (TMSI == paging_notification->identity_type)
      {
        page_info.identity_type = RR_IDENTITY_TYPE_TMSI;

        page_info.identity[0] = TMSI_SIZE + 1; /* 4 octets, plus the format octet that follows */
        page_info.identity[1] = 0xF0 | 0x04;   /* dummy first digit and TMSI identifier "0100" */
        bValid = rr_siminfo_get_tmsi(page_info.identity+2,(sizeof(page_info.identity)-2));
      }
      else
      {
        MSG_GERAN_ERROR_0_G("Paged on PACCH but no valid CS identifier (IMSI/TMSI)");
        bValid = FALSE;
      }

      if (bValid)
      {
        page_info.channel_needed = rr_ce_map_paging_request_channel_needed(
          paging_notification->channel_needed
        );
      }
      else
      {
        page_info.domain = RR_PAGE_DOMAIN_NONE;
      }
      break;
    }

    /* PS Paging:                                         */
    /*   Pass page onto GMM                               */
    case RR_PS_PAGING:
      page_info.domain = RR_PAGE_DOMAIN_PS;
      page_info.identity_type = RR_IDENTITY_TYPE_PTMSI;
      break;

    /* PS Paging (abnormal):                              */
    /*   Pass page onto GMM                               */
    case RR_PS_PAGING_ABNORMAL:
      page_info.domain = RR_PAGE_DOMAIN_PS;
      page_info.identity_type = RR_IDENTITY_TYPE_IMSI;
      break;

    default:
      MSG_GERAN_LOW_1_G("Unknown page type (%d)",paging_notification->paging_type);
      break;
  }

  if (page_info.domain != RR_PAGE_DOMAIN_NONE)
  {
    rr_send_imsg_page_ind(&page_info);
  }
} /* end handle_mac_grr_paging_notification() */
#endif /* FEATURE_GERAN_CS_SUPPORT */


/*===========================================================================

FUNCTION relinquish_control_to_rr

DESCRIPTION
  This function is called to return control to RR

PARAMS
  output_event - this event is returned

RETURN VALUE
  rr_event_T - output_event is returned

===========================================================================*/

static rr_event_T relinquish_control_to_rr(
  rr_event_T output_event
   
)
{
  (void)rr_gprs_nc_measurement_control(RR_EV_DEACTIVATE_MEASUREMENTS, NULL);

  switch (output_event)
  {
    case EV_PROCESS_SAVED_MESSAGE:
    {
      byte  mm_message_id;

      output_event = RR_EV_PROCESS_SAVED_MESSAGE;

      if (grr_examine_saved_mm_message(&mm_message_id))
      {
        switch (mm_message_id)
        {
#ifdef FEATURE_GERAN_CS_SUPPORT
          /* RR_EST_REQ: CS establishment pending */
          case RR_EST_REQ:
            MSG_GERAN_MED_0_G("Processing Saved RR_EST_REQ message");
            break;
#endif /* FEATURE_GERAN_CS_SUPPORT */

          /* RR_PLMN_LIST_REQ: A PLMN list search is going to be performed. Usually */
          /* a background search is performed, and L1 stays camped. But if this request */
          /* interrupts a reselection, or a PS connection establishment, L1 is not */
          /* currently camped, so cell selection must be performed afterwards to regain */
          /* service. Sys info handling is suspended, not deactivated, in the case of */
          /* background search, because control will return to GRR upon completion, but */
          /* no start idle needs to be performed */
          /* N.B. Background search is not supported on PBCCH, so this must be handled as */
          /* if foreground */
          case RR_PLMN_LIST_REQ:
          {
            if (rr_is_camped())
            {
              rr_gprs_suspend_camped_sys_info_handling(FALSE);
            }

            break;
          }

          case RR_PLMN_SELECT_REQ:
          {
            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();

            /* A cell selection is going to be triggered, so deactivate sys info handling */
            rr_gprs_deactivate_timed_sys_info_refresh();

            /* The contents of the RR_PLMN_SELECT_REQ have already been stored, so the message */
            /* itself is not needed anymore */
            grr_throwaway_saved_message();

            /* Disable GRR PS services, and inform MAC of NO SERVICE */
            rr_ps_no_ps_access_del_all();

            /* Change the RR_EV_PROCESS_SAVED_MESSAGE event to RR_EV_PROCESS_PLMN_SELECT_REQ. */
            /* This indicates to RR that the RR_PLMN_SELECT_REQ message contents have already */
            /* been saved, and a cell selection is required */
            output_event = RR_EV_PROCESS_PLMN_SELECT_REQ;

            /* RR no longer camped */
            scell_info_ptr->gsm.camped_on_flag = FALSE;

            break;
          }

          case RR_DEACT_REQ:
          case RR_STOP_GSM_MODE_REQ:
          default:
          {
            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();

            rr_gprs_deactivate_timed_sys_info_refresh();

            /* RR no longer camped */
            scell_info_ptr->gsm.camped_on_flag = FALSE;
            break;
          }
        }
      }

      break;
    }

    default:
      break;
  }

  /* In case a GMM procedure was ongoing, and is interrupted by the suspension */
  /* of GPRS to perform some action, ensure L1 is taken out of MM non-DRX mode */
  rr_gprs_clear_l1_in_mm_non_drx();

  // Send the event into RR-CONTROL to be processed
  rr_send_imsg_rr_event_ind(output_event);

  return EV_NO_EVENT;

} /* end relinquish_control_to_rr() */



#ifdef FEATURE_GERAN_CS_SUPPORT
/*===========================================================================

FUNCTION preprocess_rr_est_req

DESCRIPTION
  This function is called when a RR_EST_REQ is queued.
  It determines if CS call will be allowed to proceed when it is processed

PARAMS
  ptr_rr_est_req - RR_EST_REQ message contents

RETURN VALUE
  EV_RR_EST_REQ, EV_FORCE_CELL_RESELECTION, EV_NO_EVENT

===========================================================================*/

static rr_event_T preprocess_rr_est_req(
  rr_est_req_T *ptr_rr_est_req
   
)
{
  rr_event_T rr_event = EV_NO_EVENT;

  if ( rr_gprs_is_snr_counter_zero() &&
       ( ptr_rr_est_req->establishment_cause != (byte)MM_RR_ANSWER_TO_PAGING ) )
  {
    /* SNR reselection will be triggered */
    rr_event = EV_FORCE_CELL_RESELECTION;
  }
  else
  /* Verify that the CS connection can go ahead */
  if (rr_ce_allow_cs_connection(ptr_rr_est_req->establishment_cause))
  {
    /* CS conn est can proceed */
    rr_event = EV_RR_EST_REQ;
  }

  return rr_event;

} /* end preprocess_rr_est_req() */



/*===========================================================================

FUNCTION handle_rr_est_req

DESCRIPTION
  This function is called when a RR_EST_REQ is received from MM.
  It determines if CS calls are allowed, and send a RR_ABORT_IND to MM if not.
  If the CS call is allowed, GPRS suspension is started (which means either
  EV_PROCESS_SAVED_MESSAGE is returned, or will be shortly generated once
  GPRS has been suspended).
  This function is generic for 51/52 operation.

PARAMS
  ptr_rr_est_req - RR_EST_REQ message contents

RETURN VALUE
  EV_RR_EST_REQ; EV_FORCE_RESELECTION; EV_NO_EVENT

===========================================================================*/

static rr_event_T handle_rr_est_req(
  rr_est_req_T *ptr_rr_est_req
   
)
{
  rr_event_T rr_event = EV_NO_EVENT;

  if (rr_gprs_is_snr_counter_zero() &&
      (ptr_rr_est_req->establishment_cause != (byte)MM_RR_ANSWER_TO_PAGING))
  {
    /* Kick off an SNR reselection */
    rr_event = EV_FORCE_CELL_RESELECTION;

     rr_log_set_reselection_trigger(RR_RR_EST_REQ_SNR_RESELECTION_TRIGGER);

    MSG_GERAN_HIGH_0_G("++SNR Reselection for rr_est_req++");
  }
  else
  /* Verify that the CS connection can go ahead */
  if (rr_ce_allow_cs_connection(ptr_rr_est_req->establishment_cause))
  {
    rr_gprs_cancel_all_five_second_timers();

#ifndef FEATURE_GSM_DTM
    rr_event = EV_RR_EST_REQ;
#else
    if (rr_get_nv_dtm_enabled())
    {
      rr_event = rr_dtm_control(EV_RR_EST_REQ, NULL);
    }
    else
    {
      MSG_GERAN_MED_0_G("DTM disabled in NV");
      rr_event = EV_RR_EST_REQ;
    }
#endif /* FEATURE_GSM_DTM */

    rr_save_msg_for_recovery_after_panic_reset(
      (rr_cmd_bdy_type *) ptr_rr_est_req
    );
  }
  else
  {
    MSG_GERAN_HIGH_0_G("RR_EST_REQ but CS not allowed!");

    /* Send a RR_ABORT_IND to MM so it is not left hanging */
    rr_send_abort_ind(OTHER_REASON, MM_RR_OTHER_FAILURE);

    /* The RR_EST_REQ message is saved pending in RR - remove it now, as it won't be processed */
    grr_throwaway_saved_message();
  }

  return rr_event;

} /* end handle_rr_est_req() */
#endif /* FEATURE_GERAN_CS_SUPPORT */



/*===========================================================================

FUNCTION handle_rr_plmn_list_req

DESCRIPTION
  This function is called when a RR_PLMN_LIST_REQ is received from MM.
  This function is generic for 51/52 operation.

PARAMS
  rr_plmn_list_req - message as received

RETURN VALUE
  RR event feed into grr_control()

===========================================================================*/

static rr_event_T handle_rr_plmn_list_req(
  rr_plmn_list_req_T * rr_plmn_list_req
   
)
{
  rr_event_T output_event = EV_NO_EVENT;
  boolean reject = FALSE;
  grr_control_data_t *grr_control_data_ptr;

  // Obtain a pointer to the module data
  grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

#ifndef FEATURE_BPLMN_SEARCH_PRIORITY
  /* A general rule - block automatic search if GPRS is active */
  if (((rr_macinfo_get_mac_status() != MAC_IDLE) ||
       grr_has_there_been_recent_gprs_activity()) &&
      rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
  {
    MSG_GERAN_HIGH_0_G("Can't suspend GPRS for automatic PLMN search");
    rr_send_plmn_list_cnf(
      NULL,
      MM_AS_ABORTED,
      rr_plmn_list_req
    );
    grr_throwaway_saved_message();
    return EV_NO_EVENT;
  }
#endif // FEATURE_BPLMN_SEARCH_PRIORITY

  /* state dependent rules */
  switch (grr_control_data_ptr->state)
  {
    case GRR_ACQUIRE:
    {
      if (rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        reject = TRUE;
      }
      else
      {
        (void)rr_gprs_bsi_acquisition_control(EV_RESET_SOFTWARE, NULL);
        output_event = EV_PROCESS_SAVED_MESSAGE;
      }
      break;
    } /* GRR_ACQUIRE */

    case GRR_CONNECTION_PENDING:
    {
      if (rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        reject = TRUE;
      }
      else
      {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
        if (rr_x2g_cco_in_progress())
        {
          output_event = EV_X2G_CCO_ABORTED_FOR_HIGH_PRI_REQ;
        }
        else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
        {
          output_event = EV_PROCESS_SAVED_MESSAGE;
        }
      }
      break;
    } /* GRR_CONNECTION_PENDING */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case GRR_CELL_RESELECTION_G2W:
    {
      if (rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        reject = TRUE;
      }
      else
      {
        reject = FALSE;
        rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
      }
      break;
    } /* GRR_CELL_RESELECTION_G2W */
#endif 

    case GRR_CELL_RESELECTION:
    {
      if (rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
      {
        reject = TRUE;
      }
      else
      {
        reject = FALSE;
      }

      if (reject == FALSE)
      {
        rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
      }
      break;
    } /* GRR_CELL_RESELECTION */

    case GRR_CAMPED:
    {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
      if (rr_x2g_cco_in_progress())
      {
        if (rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
        {
          reject = TRUE;
        }
        else
        {
          reject = FALSE;
          rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
          output_event = EV_X2G_CCO_ABORTED_FOR_HIGH_PRI_REQ;
        }
      }
      else if (rr_pcco_in_progress() || rr_failed_pcco_in_progress())
#else /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
      if (rr_pcco_in_progress() || rr_failed_pcco_in_progress())
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
      {
        if (rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
        {
          reject = TRUE;
        }
        else
        {
          reject = FALSE;
          rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
        }
      }
      else if (PAGING_REORGANIZATION == rr_ce_get_ota_page_mode())
      {
        reject = TRUE;
        MSG_GERAN_HIGH_0_G("UE is Page reorg mode");
      }
      else
      {
        rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
      }
      break;
    } /* GRR_CAMPED */

    default:
    {
      break;
    }
  }

  if (reject)
  {
    MSG_GERAN_HIGH_2_G("Can't proceed RR_PLMN_LIST_REQ (mode %d) in state %d",
             rr_plmn_list_req->network_selection_mode, grr_control_data_ptr->state);
    rr_send_plmn_list_cnf(
      NULL,
      MM_AS_ABORTED,
      rr_plmn_list_req
    );
    grr_throwaway_saved_message();
  }
  else
  {
    rr_save_msg_for_recovery_after_panic_reset((rr_cmd_bdy_type *) rr_plmn_list_req);
  }

  return output_event;

} /* end handle_rr_plmn_list_req() */



/*===========================================================================

FUNCTION handle_rr_plmn_select_req

DESCRIPTION
  This function is called when a RR_PLMN_SELECT_REQ is received from MM.
  This function is generic for 51/52 operation.

PARAMS
  None

RETURN VALUE
  EV_PROCESS_SAVED_MESSAGE; RR_EV_CAMPED_ON_CELL; EV_NO_EVENT

===========================================================================*/

static rr_event_T handle_rr_plmn_select_req(
  rr_plmn_select_req_T * rr_plmn_select_req
   
)
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  if (rr_x2g_cco_in_progress())
  {
    /* If an X2G CCO is in progress the safest thing to do is terminate
    it early, as the RR_PLMN_SELECT_REQ may cause a network change, or even
    deactivate the originating RAT. */
    return EV_X2G_CCO_ABORTED_FOR_HIGH_PRI_REQ;
  }
  else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  {
    rr_event_T  rr_event, grr_event;
    boolean service_domain_changed;
    sys_srv_domain_e_type old_service_domain = rr_sel_get_service_domain();

    MSG_GERAN_HIGH_0_G("Handling RR_PLMN_SELECT_REQ...");

    rr_event = rr_process_plmn_select_req(
      rr_plmn_select_req,        // rr_plmn_select_req
      &service_domain_changed   // service_domain_changed_ptr
    );

    // Note: Calling rr_process_plmn_select_req() overwrites the previous values (including service domain)

    // Check what action is required
    if (rr_event == RR_EV_NO_CELL_SELECTION_REQUIRED)
    {
      // Check for any changes in service domain
      if (service_domain_changed)
      {
        // Check for changes which affect PS operation.
        // Note: This call will send RR_PLMN_SELECT_CNF
        rr_ps_plmn_select_req_service_domain_changed(
          old_service_domain,                  // old_service_domain
          rr_plmn_select_req->service_domain  // new_service_domain
        );
      }
      else
      {
        rr_send_plmn_select_cnf_success();
      }

      // Reset the search reason to the default to stop any unwanted RR_PLMN_SELECT_CNFs being sent
      rr_sel_set_search_reason(RR_AUTONOMOUS_CELL_SELECTION);

      grr_throwaway_saved_message();

      grr_event = EV_NO_EVENT;
    }
    else
    {
      MSG_GERAN_HIGH_0_G("Cell selection required");

      /* Save the message contents in case there is a failure / panic reset */
      rr_save_msg_for_recovery_after_panic_reset((rr_cmd_bdy_type *)rr_plmn_select_req);

      switch (grr_control_data_ptr->state)
      {
        // In these cases, allow the saved message to handled
        case GRR_CONNECTION_PENDING:
        case GRR_ACQUIRE:
        {
          grr_event = EV_PROCESS_SAVED_MESSAGE;
          break;
        }

        // The default case is to abort any reselection activity and then handle the saved message
        default:
        {
          rr_resel_send_imsg_abort_req(RR_RESEL_ABORT_REASON_MM_REQ);
          grr_event = EV_NO_EVENT;
        }
      }
    }

    return grr_event;
  }
} /* end handle_rr_plmn_select_req() */



#ifdef FEATURE_GERAN_CS_SUPPORT
/*===========================================================================

FUNCTION handle_rr_abort_req

DESCRIPTION
  This function responds to a RR_ABORT_REQ

PARAMS
  abort_msg_ptr

RETURN VALUE
  None

===========================================================================*/

static void handle_rr_abort_req(
   
)
{
  MSG_GERAN_MED_0_G("GRR responding to MM ABORT_REQ");

  /* If MM has aborted a RR_EST_REQ which is as yet unprocessed, remove it */
  if (grr_verify_saved_mm_message(RR_EST_REQ))
  {
    grr_throwaway_saved_message();
  }

  /* send the abort CNF to MM */
  rr_send_abort_cnf(MM_AS_SUCCESS);

} /* end handle_rr_abort_req() */
#endif /* FEATURE_GERAN_CS_SUPPORT */



/*===========================================================================

FUNCTION process_saved_message

DESCRIPTION
  This function checks for the presence of a saved input message in RR
  Note: Any previously saved message is NOT REMOVED from RR's store

PARAMS
  msg_ptr_ptr, msg_set_ptr, msg_id_ptr

RETURN VALUE
  RR event of saved message - if available

===========================================================================*/

static rr_event_T process_saved_message(
  rr_cmd_bdy_type **new_message_ptr_ptr,
  byte *message_set_ptr,
  byte *message_id_ptr
   
)
{
  if (new_message_ptr_ptr != NULL)
  {
    // Load the saved message into rr_cmd_bdy_type pointed to by new_message_ptr_ptr
    if (rr_examine_saved_message(new_message_ptr_ptr) == EV_INPUT_MESSAGE)
    {
      // Check the message is valid
      if ((*new_message_ptr_ptr) != NULL)
      {
        rr_cmd_bdy_type *new_message_ptr = (*new_message_ptr_ptr);
        IMH_T           *message_head_ptr = (IMH_T *)new_message_ptr;

        *message_set_ptr = message_head_ptr->message_set;
        *message_id_ptr  = message_head_ptr->message_id;

        return EV_INPUT_MESSAGE;
      }
    }
  }

  return EV_NO_EVENT;

} /* end process_saved_message() */




/*===========================================================================

FUNCTION grr_verify_saved_mac_message

DESCRIPTION
  Passed a MAC->RR message id, and check if this is the current saved message

PARAMS
  message_id

RETURN VALUE
  TRUE is message_id matches the saved message; FALSE otherwise

===========================================================================*/
boolean grr_verify_saved_mac_message(byte message_id )
{
  rr_cmd_bdy_type * message_ptr;

  boolean match = FALSE;

  if (rr_examine_saved_message(&message_ptr) == EV_INPUT_MESSAGE)
  {
    IMH_T * message_header_ptr = (IMH_T *) message_ptr;

    if ( ( message_header_ptr->message_set == MS_MAC_RR ) &&
         ( message_header_ptr->message_id == message_id ) )
    {
      match = TRUE;
    }
  }

  return match;

} /* end grr_verify_saved_mac_message() */



/*===========================================================================

FUNCTION grr_verify_saved_mm_message

DESCRIPTION
  Passed a MM->RR message id, and check if this is the current saved message

PARAMS
  message_id

RETURN VALUE
  TRUE is message_id matches the saved message; FALSE otherwise

===========================================================================*/
boolean grr_verify_saved_mm_message(
  mm_rr_message_id_enum_T message_id
   
)
{
  rr_cmd_bdy_type * message_ptr;

  boolean match = FALSE;

  if (rr_examine_saved_message(&message_ptr) == EV_INPUT_MESSAGE )
  {
    IMH_T * message_header_ptr = (IMH_T *) message_ptr;

    if ( ( message_header_ptr->message_set == MS_MM_RR ) &&
         ( message_header_ptr->message_id == (uint8) message_id ) )
    {
      match = TRUE;
    }
  }

  return match;

} /* end grr_verify_saved_mm_message() */

/*===========================================================================

FUNCTION grr_verify_saved_l2_message

DESCRIPTION
  Passed a L2->RR message id, and check if this is the current saved message

PARAMS
  message_id

RETURN VALUE
  TRUE is message_id matches the saved message; FALSE otherwise

===========================================================================*/
boolean grr_verify_saved_l2_message(
  byte message_id
   
)
{
  rr_cmd_bdy_type * message_ptr;

  boolean match = FALSE;

  if (rr_examine_saved_message(&message_ptr) == EV_INPUT_MESSAGE)
  {
    IMH_T * message_header_ptr = (IMH_T *) message_ptr;

    if ( ( message_header_ptr->message_set == MS_RR_L2 ) &&
         ( message_header_ptr->message_id == message_id ) )
    {
      match = TRUE;
    }
  }

  return match;

} /* end grr_verify_saved_l2_message() */

#ifdef FEATURE_GPRS_PS_HANDOVER

/**
 * Process the PS handover command
 *
 * @param psho              pointer to PS HANDOVER COMMAND
 *
 */

static rr_event_T rr_gprs_process_psho_command(
  rr_cmd_bdy_type *new_message
   
)
{
  (void)new_message;
  return EV_PSHO_REJECTED;

  // InterRAT PSHO is not verified, therefore the feature is considered as unsupported

#if 0
  rr_event_T             output_event =  EV_PSHO_REJECTED;
  ps_handover_command_t *psho;

  psho = (ps_handover_command_t*)((mac_grr_data_ind_t *)new_message)->data_ptr.data_ptr;

  if (psho == NULL)
  {
    MSG_GERAN_ERROR_0_G("PSHO: NULL pointer");
    return EV_PSHO_REJECTED;
  }

  if (psho->target_type == CSN_PSHO_GSM_TARGET_CELL)
  {
    /* PSHO 2G -> 2G */

    ps_handover_radio_resources_t *psho_target =
      &(psho->payload.a_gb.ps_handover_radio_resources);

    MSG_GERAN_HIGH_1_G("PSHO command received, target arfcn %d", psho_target->arfcn.num);

    /* save the handover command details */
    rr_psho_save_details(psho);

    /* pass the PSHO command to cell relelection for further processing */
    if (EV_PSHO_STARTED == rr_resel_control(EV_INPUT_MESSAGE, new_message))
    {
      rr_gprs_start_psho();
      output_event = EV_PSHO_STARTED;
    }
    else
    {
      rr_psho_clean_details();
      output_event =  EV_PSHO_REJECTED;
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("PSHO command received, unknown target cell type");
    output_event =  EV_PSHO_REJECTED;
  }

  return output_event;
#endif // #if 0
}

#if 0
static void rr_gprs_start_psho(
   
)
{
  rr_params_mods_t ** params_mods = rr_params_mods();

  (void)rr_gprs_nc_measurement_control(RR_EV_DEACTIVATE_MEASUREMENTS, NULL);

  if (params_mods != NULL && *params_mods != NULL)
  {
    GPRS_MEM_FREE(*params_mods);
    *params_mods = NULL;
  }
  rr_gprs_deactivate_timed_sys_info_refresh();

  /* activate psho_control */
  (void)rr_psho_control(EV_PSHO_STARTED, NULL);
}
#endif // #if 0

#endif /* FEATURE_GPRS_PS_HANDOVER */

boolean grr_can_process_rr_resel_abort_req(
   
)
{
  boolean can_process_signal = FALSE;
  grr_control_data_t *grr_control_data_ptr;

  // Obtain a pointer to the module data
  grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  switch (grr_control_data_ptr->state)
  {
    case GRR_CELL_RESELECTION:
    case GRR_CAMPED:
    {
      can_process_signal = rr_resel_can_process_rr_resel_abort_req();
    }
    break;

    default:
    {
      can_process_signal = TRUE;
    }
    break;
  }

  return can_process_signal;
}


boolean grr_can_process_rr_abort_req(   )
{
  /* In most states, GRR processes RR_ABORT_REQ() by throwing saved MM message and responding with RR_ABORT_CNF */
  /* Few transient states, it waits for state movement */
  
  boolean can_process_msg;
  grr_control_data_t *grr_control_data_ptr;

  // Obtain a pointer to the module data
  grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  switch (grr_control_data_ptr->state)
  {    
    case GRR_ACQUIRE:
    case GRR_CAMPED:
    case GRR_CELL_RESELECTION:
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case GRR_CELL_RESELECTION_G2W:
#endif
    {
      can_process_msg = TRUE;      
      break;
    }

    /* Block RR_ABORT_REQ() in below transient states */
    case GRR_STARTING_L1_IDLE:
    case GRR_CAMPED_ENABLING_PS_ACCESS:
    case GRR_CAMPED_DISABLING_PS_ACCESS:
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
    case GRR_CAMPED_MSC_CHANGE:
#endif

#ifdef FEATURE_SGLTE
    case GRR_SGLTE_SWITCHING_TO_GSM_ONLY:
    case GRR_SGLTE_X2G_RESEL:
#endif /* FEATURE_SGLTE */
    {
      can_process_msg = FALSE;       
      break;
    }

    default:
    {
      can_process_msg = TRUE;
    }
    break;
  }

  return (can_process_msg);

}

boolean grr_can_process_rr_grr_gprs_ia_ind( )
{
  // Default action is to block processing
  boolean process = FALSE;

  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  // Only allow processing if RR state is RR_GRR.
  // This is because during BPLMN search, RR state is RR_PLMN_LIST_SEARCH but GRR state remains GRR_CAMPED.
  if (rr_is_control_state(RR_GRR))
  {
    // Change default action to process (with exceptions below)
    process = TRUE;

    // Block in states where there is an ongoing transition to Packet Idle and/or enabling PS access
    switch (grr_control_data_ptr->state)
    {
      case GRR_CAMPED_ENABLING_PS_ACCESS:
      case GRR_STARTING_L1_IDLE:
      {
        process = FALSE;
        break;
      }

      case GRR_CONNECTION_PENDING:
      {
        process = rr_ce_can_process_rr_grr_gprs_ia_ind();
      }

      default: ;
    }
  }

  return process;
}

boolean grr_can_process_rr_event_ind( )
{
  boolean process = TRUE;
  grr_control_data_t *grr_control_data_ptr;

  // Obtain a pointer to the module data
  grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  switch (grr_control_data_ptr->state)
  {
    case GRR_CAMPED_ENABLING_PS_ACCESS:
    case GRR_CAMPED_DISABLING_PS_ACCESS:
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
    case GRR_CAMPED_MSC_CHANGE:
#endif
#ifdef FEATURE_SGLTE
    case GRR_SGLTE_SWITCHING_TO_GSM_ONLY:
#endif /* FEATURE_SGLTE */
    {
      process = FALSE;
      break;
    }

    default: ;
  }

  return process;
}

boolean grr_can_process_mm_message( )
{
  boolean process = TRUE;
  grr_control_data_t *grr_control_data_ptr;

  // Obtain a pointer to the module data
  grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  switch (grr_control_data_ptr->state)
  {
    case GRR_STARTING_L1_IDLE:
    case GRR_CAMPED_ENABLING_PS_ACCESS:
    case GRR_CAMPED_DISABLING_PS_ACCESS:
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
    case GRR_CAMPED_MSC_CHANGE:
#endif
#ifdef FEATURE_SGLTE
    case GRR_SGLTE_SWITCHING_TO_GSM_ONLY:
#endif /* FEATURE_SGLTE */
    {
      process = FALSE;
      break;
    }

    case GRR_CONNECTION_PENDING:
    {
      boolean l1_idle_state;
      l1_idle_state = rr_is_l1_idle_state_start_idle();
      if ( l1_idle_state == TRUE)
      {
        process = FALSE;
      }
      break;
    }

    default: ;
  }

  return process;
}

boolean grr_can_process_timer_expiry( )
{
  boolean process = TRUE;
  grr_control_data_t *grr_control_data_ptr;

  // Obtain a pointer to the module data
  grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  switch (grr_control_data_ptr->state)
  {
    case GRR_STARTING_L1_IDLE:
    case GRR_CAMPED_ENABLING_PS_ACCESS:
    case GRR_CAMPED_DISABLING_PS_ACCESS:
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
    case GRR_CAMPED_MSC_CHANGE:
#endif
    {
      process = FALSE;
      break;
    }

    default: ;
  }

  return process;
}

boolean grr_can_process_serving_meas_ind( )
{
  boolean process = TRUE;
  grr_control_data_t *grr_control_data_ptr;

  // Obtain a pointer to the module data
  grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  switch (grr_control_data_ptr->state)
  {
    case GRR_STARTING_L1_IDLE:
    case GRR_CAMPED_ENABLING_PS_ACCESS:
    case GRR_CAMPED_DISABLING_PS_ACCESS:
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
    case GRR_CAMPED_MSC_CHANGE:
#endif
#ifdef FEATURE_SGLTE
    case GRR_SGLTE_SWITCHING_TO_GSM_ONLY:
#endif /* FEATURE_SGLTE */
    {
      process = FALSE;
      break;
    }

    default: ;
  }

  return process;
}

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
boolean grr_can_process_proc_ind_with_suspend(void)
{
  boolean process = TRUE;
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();

  switch (grr_control_data_ptr->state)
  {
    case GRR_STARTING_L1_IDLE:
    case GRR_CAMPED_ENABLING_PS_ACCESS:
    case GRR_CAMPED_DISABLING_PS_ACCESS:
    case GRR_CAMPED_MSC_CHANGE:
    {
      process = FALSE;
      break;
    }
    default: ;
  }

  return process;
}
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */
boolean grr_can_process_surround_meas_ind(void)
{
  boolean process = TRUE;
  grr_control_data_t *grr_control_data_ptr;

  // Obtain a pointer to the module data
  grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  switch (grr_control_data_ptr->state)
  {
    case GRR_STARTING_L1_IDLE:
    case GRR_CAMPED_ENABLING_PS_ACCESS:
    case GRR_CAMPED_DISABLING_PS_ACCESS:
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
    case GRR_CAMPED_MSC_CHANGE:
#endif
#ifdef FEATURE_SGLTE
    case GRR_SGLTE_SWITCHING_TO_GSM_ONLY:
#endif /* FEATURE_SGLTE */
    {
      process = FALSE;
      break;
    }

    default: ;
  }

  return process;
}

boolean grr_can_process_change_mode_ind( )
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  boolean             process;

  switch (grr_control_data_ptr->state)
  {
    case GRR_CAMPED:
      process = TRUE;
      break;

    default:
      process = FALSE;
  }

  return process;
}


#ifdef FEATURE_SGLTE

boolean grr_can_process_stop_gsm_mode_req_sglte_rat_change( )
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  boolean             process;

  switch (grr_control_data_ptr->state)
  {
    case GRR_CAMPED:
    {
      if (rr_is_pending_mode_change() )
      {
        MSG_GERAN_HIGH_1_G(
        "Processing of STOP GSM MODE Blocked due to Pending Mode Change in GRR state: %d",
         grr_control_data_ptr->state
        );
        process = FALSE;
      }
      else
      {
        process = TRUE;
      }
      
      break;
    }
    case GRR_CELL_RESELECTION_G2W:
    {
      process = TRUE;
      break;
    } /* GRR_CAMPED */

    default:
    {
      process = FALSE;
    }
  }

  return process;
}

boolean grr_can_process_sglte_x2g_req( )
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  boolean             process;

  switch (grr_control_data_ptr->state)
  {
    case GRR_CAMPED:
    {
      process = TRUE;
      break;
    } /* GRR_CAMPED */

    default:
    {
      process = FALSE;
    }
  }

  return process;
}

boolean grr_can_process_sglte_band_coex_omsg_supported_bands_updated_ind( )
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  boolean             process;

  switch (grr_control_data_ptr->state)
  {
    case GRR_CAMPED:
    {
      process = TRUE;
      break;
    } /* GRR_CAMPED */

    default:
    {
      process = FALSE;
    }
  }

  return process;
}

boolean grr_can_process_sglte_plmn_search( )
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  boolean             process;

  switch (grr_control_data_ptr->state)
  {
    case GRR_CAMPED:
    case GRR_CONNECTION_PENDING:
    case GRR_CELL_RESELECTION:
    case GRR_CAMPED_ENABLING_PS_ACCESS:
    case GRR_CAMPED_DISABLING_PS_ACCESS:
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
    case GRR_CAMPED_MSC_CHANGE:
#endif
    {
      process = TRUE;
      break;
    }

    default:
    {
      process = FALSE;
    }
  }

  return process;
}

#endif /* FEATURE_SGLTE */


/**
  @brief Initialisation function. Resets internal variables and the state machine
*/
void rr_grr_control_init( )
{
  // Obtain a pointer to the module data
  grr_control_data_t *grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  grr_control_data_ptr->state = GRR_NULL;
  grr_control_data_ptr->old_state = GRR_NULL;
  grr_control_data_ptr->tbf_est_abort_event = EV_NO_EVENT;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  grr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  grr_control_data_ptr->notify_mm_plmn_select_cnf = FALSE;
  grr_control_data_ptr->notify_mm_service_ind = FALSE;
}


#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 

boolean grr_can_process_rr_resel_reduced_mobility_msg(
   
)
{
  boolean can_process_signal = FALSE;
  grr_control_data_t *grr_control_data_ptr;

  // Obtain a pointer to the module data
  grr_control_data_ptr = grr_control_get_data_ptr();
  //RR_NULL_CHECK_FATAL(grr_control_data_ptr);

  switch (grr_control_data_ptr->state)
  {
    case GRR_CAMPED_DISABLING_PS_ACCESS:
    case GRR_CAMPED_ENABLING_PS_ACCESS:
    case GRR_STARTING_L1_IDLE:
    case GRR_CELL_RESELECTION:
    {
      can_process_signal = FALSE;
    }
    break;

    default:
    {
      can_process_signal = TRUE;
    }
    break;
  }

  return can_process_signal;
}
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/
/**
  @brief Initialisation function. Should be called once at RR task start

  Note: The primary purpose of this function is to initialise pointers to control/data structures.
*/
void rr_grr_control_task_start_init( void )
{
  // Zero the control/data structure(s)
  memset(& grr_control_data, 0, sizeof(grr_control_data));

  // Setup any specific values not setup by the general module initialisation function
}

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/* EOF */


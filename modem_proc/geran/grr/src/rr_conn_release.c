/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Connection Release Module

GENERAL DESCRIPTION
  Connection release state machine

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_conn_release.c_v   1.6   08 Jul 2002 19:28:26   jachan  $
$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_conn_release.c#8 $ $DateTime: 2019/12/02 06:22:48 $ $Author: pwbldsvc $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
May/06/09   SJW     Removed unused variable last_no_channels_used
Nov/07/08   SJW     Allow SAPI3 control to decide whether release is required
Aug/11/04   NJT     Force reconstruct of BA from Si5 before RPS after call release
Jun/07/04   NJT     Added code for optimisation to force RPS after call release.
May/26/04   DMB     Added MPH_START_IDLE_MODE_CNF handling
05/07/04    gc      Enhanced Trace Buffer Code
05/08/03    he      Used separate debug_str buffer for logging.
02/14/03    RG      Added support for G2W blind Handover
01/27/03    JAC     Restoring of cell channel description when going idle
01/22/03    he      Fixed lint errors.
12/02/02    JAC     Transfer control to GRR when serving cell is GPRS and SI valid
08/26/02    rmc     Removed FEATURE_MULTIMODE_GSM feature use
08/12/02    rmc     Changes for cell selection optimizations
07/22/02    Rao     Adding mm_as_status to abort_cnf.
07/04/02    JAC     rr_start_idle_mode interface change
06/20/02    rmc     If connection is release because of reception of RR_ABORT_REQ, RR
                    should always respond with RR_ABORT_CNF
05/19/02    JAC     always send a release message in rr_respond_to_rlf and rr_respond_to_abort
05/13/02    rmc     Dual mode changes
04/11/02    JAC     added stored plmn support
03/12/02    rmc     Dual Mode Cell Selection
02/08/02    rmc     QXDM Cleanup
01/30/02    RMC     Added logging calls

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#ifdef FEATURE_GERAN_CS_SUPPORT
#include "rr_seg_load.h"
#include "rr_conn_release.h"
#include "ms.h"
#include "rr_l2.h"
#include "rr_l1_send.h"
#include "rr_l2_send.h"
#include "rr_mm_send.h"
#include "rr_decode_ie.h"
#include "rr_general.h"
#include "cause.h"
#include "rr_log.h"
#include "rr_test.h"
#include "rr_gprs_defs.h"
#include "rr_gprs_debug.h"
#include "rr_gprs_conn_establish.h"
#include "rr_sapi3.h"
#include "rr_ded_meas_reporting.h"
#include "rr_conn_establish.h"
#ifdef FEATURE_GSM_DTM
#include "rr_dtm.h"
#ifdef FEATURE_GSM_EDTM
#include "rr_gprs_edtm.h"
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
#include "rr_shared_cell_selection.h"
#include "rr_procedure.h"
#include "rr_cell_selection.h"
#include "rr_cell_reestablish.h"
#include "rr_resel_utils.h"
#include "rr_sys_info_ncell.h"
#include "rr_sys_info_pcell.h"
#include "rr_ded_config.h"
#include "rr_ps_domain.h"
#include "rr_multi_sim.h"
#include "rr_plmn_list.h"
#include "gprs_mem.h"
#include "rr_nv.h"
#include "rr_static_main.h"

/*****************************************************************************
***
***     External Functions & Variables
***
*****************************************************************************/

/*****************************************************************************
***
***     Local Typedefs, #defines & Variables
***
*****************************************************************************/

typedef enum
{
  RR_REL_CHANNEL_CONNECTED,
  RR_REL_PENDING_MAIN_SIGNALLING_DISCONNECTION,
  RR_REL_MAIN_SIGNALLING_DISCONNECTED,
  RR_REL_NO_CONNECTION,
  RR_REL_PENDING_WAIT_FOR_PROCEDURE_TO_COMPLETE,
  RR_REL_PENDING_WAIT_FOR_DTM_RELEASE,
  RR_REL_STATE_MAX
} rr_release_state_T;

#ifdef FEATURE_GSM_DTM
typedef struct
{
  boolean   valid_bcch_carrier;
  ARFCN_T   serving_bcch_carrier;
  BSIC_T    BSIC;
} channel_release_info_t;
#endif /* FEATURE_GSM_DTM */

typedef enum
{
  RR_CE_REL_L2_RELEASE_NORMAL,
  RR_CE_REL_L2_RELEASE_LOCAL,
  RR_CE_REL_L2_RELEASE_NOT_REQUIRED,
} rr_ce_rel_l2_release_mode_e;

typedef struct
{
  rr_conn_release_cause_t      reported_cause;
  boolean                      rlf_occurred;
  boolean                      mm_abort_req;
  rr_ce_rel_l2_release_mode_e  l2_release_mode;
  #ifdef FEATURE_GSM_DTM
  rr_conn_release_mode_t       release_mode;
  boolean                      l1_in_null;
  channel_release_info_t       channel_release_info;
  #endif /* FEATURE_GSM_DTM */
  boolean                      rps_required_after_release;
  rr_release_state_T           release_state;
  rr_release_state_T           old_release_state;
  rr_event_T                   x2g_cco_failure_event;
#ifdef FEATURE_LTE
  /* Indicates whether G2L blind redirection is required.  If required, after
   * a CS call (for G originated voice, supplementary services or periodic LAU)
   * is released RR will initiate a blind redirection to LTE. */
  boolean                      g2l_blind_redir_required;

  /* Indicates whether G2L blind redirection after CSFB (i.e. FRLTE) is required.
   * If required, after a CS call resulting from a CSFB procedure is released, RR
   * will initiate a blind redirection to LTE. 
   * Distincition is required between CSFB and non-CSFB G2L redirection as we
   * need to pass CSFB status information to W during G2W CSHO. */
  boolean                      g2l_blind_redir_after_csfb_required;
#endif /* FEATURE_LTE */
} release_info_t;

static release_info_t rr_conn_release_data;

/*****************************************************************************
***
***     Function Prototypes
***
*****************************************************************************/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param 
 *
 * \return rr_ps_access_data_t*
 */
static release_info_t *rr_conn_release_get_data_ptr( void )
{
  return &rr_conn_release_data;
}

/*!
 * \brief Ensure that RPS is done after CS release is completed.
 */
static void rr_ce_rel_rps_required_after_release( void )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  rr_conn_release_data_ptr->rps_required_after_release = TRUE;
}

/**
 * Stores the cause for why the connection was released. This is used to determine
 * how to later indicate/confirm call release to MM
 *
 * @param release_cause
 *
 * @return none
 */
void rr_conn_set_release_cause(
  rr_conn_release_cause_t release_cause )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  rr_conn_release_data_ptr->reported_cause = release_cause;

  // Check for certain causes that have associated flags which are 'persistent' and remain set if another
  // release request is received during the ongoing release process which over-writes the stored cause
  switch (release_cause)
  {
    case RELEASE_CAUSE_NONE:
    {
      rr_conn_release_data_ptr->rlf_occurred = FALSE;
      rr_conn_release_data_ptr->mm_abort_req = FALSE;
      rr_conn_release_data_ptr->l2_release_mode = RR_CE_REL_L2_RELEASE_NORMAL;
#ifdef FEATURE_GSM_DTM
      rr_conn_release_data_ptr->l1_in_null = FALSE;
#endif /* FEATURE_GSM_EDTM */
      break;
    }

    case RELEASE_CAUSE_RADIO_LINK_FAILURE:
    {
      rr_conn_release_data_ptr->rlf_occurred = TRUE;
      rr_conn_release_data_ptr->l2_release_mode = RR_CE_REL_L2_RELEASE_LOCAL;
      break;
    }

    case RELEASE_CAUSE_L2_LINK_RELEASED:
    {
      rr_conn_release_data_ptr->l2_release_mode = RR_CE_REL_L2_RELEASE_LOCAL;
      break;
    }

    case RELEASE_CAUSE_L2_LINK_RESET:
    {
      rr_conn_release_data_ptr->l2_release_mode = RR_CE_REL_L2_RELEASE_NOT_REQUIRED;
      break;
    }

    case RELEASE_CAUSE_NAS_REQUEST:
    {
      rr_conn_release_data_ptr->mm_abort_req = TRUE;
      break;
    }

    case RELEASE_CAUSE_SUSPEND_IND:
    {
      rr_conn_release_data_ptr->l2_release_mode = RR_CE_REL_L2_RELEASE_LOCAL;
      break;
    }

    default: ;
  }

} /* end rr_conn_set_release_cause() */


/**
 * Returns the cause for why the connection was released
 *
 * @return release_cause
 */
rr_conn_release_cause_t rr_conn_get_release_cause( )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  return rr_conn_release_data_ptr->reported_cause;

} /* end rr_conn_get_release_cause() */


/**
 * Sends a signal to MM indicating/confirming call release, based on the call release
 * cause previously setup.
 * Optionally, a RR_SERVICE_IND is sent afterwards
 *
 * @param send_rr_service_ind - TRUE to additionally send a RR_SERVICE_IND
 *
 * @return none
 */
void rr_conn_inform_mm_of_channel_release(boolean send_rr_service_ind )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  // If NAS does not need to be updated, exit here
  if (rr_ce_should_confirm_conn_rel_to_nas() == FALSE)
  {
    return;
  }

  if (rr_conn_release_data_ptr->mm_abort_req)
  {
    rr_send_abort_cnf(MM_AS_SUCCESS);
  }
  else
  {
    switch (rr_conn_release_data_ptr->reported_cause)
    {
	  case RELEASE_CAUSE_NAS_REQUEST:
        rr_send_abort_cnf(MM_AS_SUCCESS);
        break;

      case RELEASE_CAUSE_IMPLICIT_NAS_REQUEST:
      case RELEASE_CAUSE_PCCO_FAILURE:
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
      case RELEASE_CAUSE_X2G_CCO_FAILURE:
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */	
#ifdef FEATURE_SGLTE
        if (rr_sglte_internal_stop_gsm_being_processed())
        {
          MSG_GERAN_HIGH_0_G("SGLTE: SRVCC Send RR_ABORT_IND with SRVCC failure");
          rr_sglte_srvcc_send_abort_ind();
        }
        else
#endif /* FEATURE_SGLTE */
        {
          rr_send_abort_ind(OTHER_REASON, MM_RR_OTHER_FAILURE);
        }
        break;

      case RELEASE_CAUSE_NETW_CHANNEL_RELEASE:
      {
        rr_send_rel_ind(NORMAL_RELEASE);
        break;
      }

      case RELEASE_CAUSE_CONN_EST_FAILURE:
      case RELEASE_CAUSE_RADIO_LINK_FAILURE:
      {
        rr_send_abort_ind(RR_CONNECTION_FAILURE, MM_RR_OTHER_FAILURE);
        break;
      }

      case RELEASE_CAUSE_SUSPEND_IND:
      {
        rr_send_abort_ind(OTHER_REASON, MM_RR_NO_RESOURCE_AVAILABLE);
        break;
      }

      default:
      {
        rr_send_abort_ind(OTHER_REASON, MM_RR_OTHER_FAILURE);
        break;
      }
    }
  }

  if (send_rr_service_ind)
  {
    gprs_scell_info_t * cell_info = rr_gprs_get_scell_info();

    if (cell_info->gsm.camped_on_flag)
    {
      rr_send_service_ind(
        MM_AS_SERVICE_AVAILABLE,
        &(cell_info->gsm.location_area_identification),
        &cell_info->gsm
      );
    }
    else
    {
      rr_send_no_service_ind();
    }
  }

} /* rr_conn_inform_mm_of_channel_release() */


/**
 * @return TRUE if the RR connection was released because of a radio link failure
 */
static boolean rr_rel_is_cause_radio_link_failure( void )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  return rr_conn_release_data_ptr->rlf_occurred;

} /* rr_rel_is_cause_radio_link_failure() */

/**
 * @return TRUE if the reestablishment procedure should be attempted
 */
static boolean rr_rel_is_reest_required( void )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  return (rr_conn_release_data_ptr->reported_cause == RELEASE_CAUSE_RADIO_LINK_FAILURE);

} /* rr_rel_is_reest_required() */

/**
 * Returns a string representation of the release state
 *
 * @param state  release state
 *
 * @return textual representation of the state
 */
static char *rr_release_state_name(rr_release_state_T state)
{
  char *s = "";
  switch (state)
  {
    case RR_REL_CHANNEL_CONNECTED:
    {
      s = "CHANNEL_CONNECTED";
    }
    break;

    case RR_REL_PENDING_MAIN_SIGNALLING_DISCONNECTION:
    {
      s = "PENDING_MAIN_SIGNALLING_DISCONNECTION";
    }
    break;

    case RR_REL_MAIN_SIGNALLING_DISCONNECTED:
    {
      s = "MAIN_SIGNALLING_DISCONNECTED";
    }
    break;

    case RR_REL_NO_CONNECTION:
    {
      s = "NO_CONNECTION";
    }
    break;

    case RR_REL_PENDING_WAIT_FOR_PROCEDURE_TO_COMPLETE:
    {
      s = "PENDING_WAIT_FOR_PROCEDURE_TO_COMPLETE";
    }
    break;

    case RR_REL_PENDING_WAIT_FOR_DTM_RELEASE:
    {
      s = "PENDING_WAIT_FOR_DTM_RELEASE";
    }
    break;

    default:
      s = "Unknown";
      break;
  }

  return s;
} /* rr_release_state_name */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/**
 * Determines whether the last RXQUAL is valid during RPS decisions
 *
 * @param none
 *
 * @return TRUE if RXQUAL can be used for RPS decision, FALSE otherwise
 */
static boolean rr_ms_is_rxqual_valid( )
{
  channel_spec_T *chan_spec_ptr;
  boolean         valid_rxqual = FALSE;

  chan_spec_ptr = rr_get_channel_spec(NULL);
  if (chan_spec_ptr != NULL)
  {
    /* only if a traffic channel was active should the RXQUAL be used */
    if ((chan_spec_ptr->number_of_ded_channels != 0) &&
       ((chan_spec_ptr->channel_info_1_after.channel_type == FULL_RATE_TRAFFIC) ||
        (chan_spec_ptr->channel_info_1_after.channel_type == HALF_RATE_TRAFFIC)))
    {
      byte s_now = 0;
      byte s_max = 0;

      /* retrieve the current and maximum values of the Radio Link Timeout counter */
      if (rr_retrieve_S_counter(&s_now, &s_max))
      {
        /* only treat the RXQUAL as valid if the RLT counter is degraded */
        valid_rxqual = (boolean)(s_now < s_max);
      }
      else
      {
        /* RLT counter has been invalidated, have to rely on RXQUAL only */
        valid_rxqual = TRUE;
      }
    }
  }

  MSG_GERAN_HIGH_1_G("RXQUAL valid=%d", (int)valid_rxqual);

  return(valid_rxqual);
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/**
 * Decides if the scell is good enough to camp onto after call release
 *
 * @param none
 *
 * @return boolean TRUE/FALSE
 */
static boolean rr_is_scell_ok_or_rps_required( )
{
  boolean                    rps_required = FALSE;
  boolean                    rxqual_valid;
  byte                       RXLEV_SUB_average  = 0;
  byte                       RXQUAL_SUB_average = 0;
  byte                       RXQUAL_threshold;
  surrounding_measurement_T *surrounding_measurement_ptr = rr_get_surrounding_measurement_ptr();

  /* retrieve the latest dedicated sub averages */
  rxqual_valid = rr_get_last_dedicated_sub_averages(
    &RXLEV_SUB_average,
    &RXQUAL_SUB_average
  );

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (rr_multi_sim_mode_enabled() && (rxqual_valid == TRUE))
  {
    RXQUAL_threshold = RXQUAL_SUB_DSDS_SCELL_THRESHOLD;
    rxqual_valid     = rr_ms_is_rxqual_valid();
  }
  else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  {
    RXQUAL_threshold = RXQUAL_SUB_POOR_SCELL_THRESHOLD;
  }

  if ((rxqual_valid == TRUE) &&
      (RXQUAL_SUB_average >= RXQUAL_threshold))
  {
    uint8 i;

    /* Go through the ncells to see if one exists that is better than the scell */
    for (i = 0; i < surrounding_measurement_ptr->no_of_entries; i++)
    {
      if (surrounding_measurement_ptr->measurement[i].RXLEV_average > RXLEV_SUB_average)
      {
        rps_required = TRUE;
        break;
      }
    }
  }

  return(rps_required);
}

/**
 * Performs a release of the data link (if any)
 *
 * @param release_cause The reason for releasing.  This is used to determine
 *                      the signal to send to MM when release is complete.
 * @param release_mode Release mode for the SAPI0 link
 */
static void rr_rel_release_data_link(
  rr_conn_release_cause_t release_cause,
  release_mode_T          release_mode
)
{
  link_status_T link_status = rr_get_link_status();

  MSG_GERAN_HIGH_3_G("Releasing data link: cause=%d, mode=%d, status=%d",
           (int)release_cause,
           (int)release_mode,
           (int)link_status);

  /* set the global release causes as specified */
  rr_conn_set_release_cause(release_cause);

  /* SAPI3 link may be pending or established, but any action */
  /* required must be decided by the separate RR SAPI3 module */
  rr_release_sapi3();

#ifdef FEATURE_SGLTE
  if (rr_sglte_internal_stop_gsm_being_processed())
  {
    MSG_GERAN_HIGH_0_G("SGLTE: SRVCC Internal shut down happening so set the link status to NO_LINK");
    link_status = NO_LINK;
  }
#endif /* FEATURE_SGLTE */

  /* if there is a SAPI0 link established */
  if ((link_status == SAPI0_AND_SAPI3_LINK) ||
      (link_status == SAPI0_LINK))
  {
    /* perform a release of the SAPI0 link */
    rr_send_dl_release_req(
      release_mode,
      SAPI0,
      DCCH
    );
  }
  else
  {
    /* if SAPI0 is pending, force an abnormal release of all SAPIs */
    rr_send_mdl_release_req(
      SAPI0,
      DCCH
    );
  }

  /* since the L2 data link is being released, start T3110 */
  MSG_GERAN_HIGH_1_G("Starting T3110(%d)",(int)T3110_TIMEOUT);
  (void)rr_timer_start(T3110,T3110_TIMEOUT);

  /* link status will be updated when DL_RELEASE_CNF is received */
  return;
} /* rr_rel_release_data_link */


/**
 * Called in response to a CHANNEL RELEASE message received from the network
 * to initiate the release of the RR connection
 */
static void rr_rel_channel_release_from_network(
  boolean default_resumption_ind
)
{
  ie_descriptor_T *current_IE;
  boolean          gprs_resumption_ind;
  byte            *start_IE;

  /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(CHANNEL_RELEASE);

  /* GCF 26.5.5.1.1.1 states:
  When on receipt of a message a "missing mandatory IE" error is diagnosed the
  MS shall proceed as follows: If the message is a CHANNEL RELEASE message,
  the actions taken shall be the same as specified for a normal RR-connection
  release. */
  if (current_IE != NULL)
  {
    if (current_IE->start_IE != NULL)
    {
      byte rr_cause = *(current_IE->start_IE);

      if ((rr_cause == 0x01) || (rr_cause == 0x03) ||
          (rr_cause == 0x04) || (rr_cause == 0x41) || (rr_cause == 0x05))
      {
        rr_log_gsm_call_drop(
          RR_LOG_CALL_DROPPED_DUE_TO_NW_ABNORMAL_RELEASE,
          rr_ce_access_reason()
        );
      }
    }
    else
    {
      MSG_GERAN_HIGH_0_G("Ignoring missing RR Cause IE in Channel Release");
    }
  }



  /* locate the GPRS Resumption Indication IE */
  start_IE = rr_find_start_of_IE(current_IE, GPRS_RESUMPTION_IEI);

  /* If GPRS Resumption IE is present */
  if (start_IE != NULL)
  {
    gprs_resumption_ind = *start_IE & 0x01;
    MSG_GERAN_MED_1_G("GPRS Resumption IE present (%d)",(int)gprs_resumption_ind);
  }
  else
  {
    /* not present, use whatever default was passed in as a parameter */
    gprs_resumption_ind = default_resumption_ind;
    MSG_GERAN_MED_1_G("GPRS Resumption IE not present, using %1d",
            (int)gprs_resumption_ind);
  }

  /* Set the status of the gprs_resumption_ind which will be reflected */
  /* to GMM when the RR_GMM_GPRS_RESUMPTION_IND is sent */
  grr_set_gprs_resumption_indicator(gprs_resumption_ind);

  {
    rr_chan_rel_cell_selection_indicator_t *rr_cell_select_indicator_ptr;

    rr_cell_select_indicator_ptr = rr_get_cell_select_indicator_ptr();

    /* locate the Cell Selection Indicator IE */
    start_IE = rr_find_start_of_IE(current_IE, CELL_SELECTION_INDICATOR_IEI);
    if (start_IE != NULL)
    {
      if (rr_decode_cell_selection_indicator(
            start_IE,
            *start_IE,
            rr_cell_select_indicator_ptr
          ))
      {
        MSG_GERAN_HIGH_0_G("Cell selection indicator decode succeeded");

#ifdef FEATURE_SGLTE
        if (rr_is_sglte_mode_active())
        {
          if (rr_cell_select_indicator_ptr->valid &&
              rr_cell_select_indicator_ptr->type != GSM_TARGET)
          {
            MSG_GERAN_HIGH_1_G("SGLTE: Network indicates redirection to %d, ignoring", rr_cell_select_indicator_ptr->type);
            rr_cell_select_indicator_ptr->valid = FALSE;
          }
        }
#endif /* FEATURE_SGLTE */
      }
      else
      {
        MSG_GERAN_ERROR_0_G("Cell selection indicator decode failed");
        rr_cell_select_indicator_ptr->valid = FALSE;
      }
    }
    else
    {
      rr_cell_select_indicator_ptr->valid = FALSE;
    }
  }

#ifdef FEATURE_LTE
  start_IE = rr_find_start_of_IE(current_IE, INDIVIDUAL_PRIORITIES_IEI);
  if (start_IE != NULL)
  {
    rr_individual_priorities_ie_t individual_priorities_ie;
    boolean                       individual_priorities_present;

    memset(&individual_priorities_ie, 0, sizeof(rr_individual_priorities_ie_t));

    individual_priorities_present = rr_individual_priorities_ie_decode(
      start_IE,
      *start_IE,
      &individual_priorities_ie
    );

    if (individual_priorities_present)
    {
      rr_PLMN_request_details_T * rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr();

      rr_individual_priorities_store(
        &individual_priorities_ie,
        rr_scell_arfcn(),
        rr_PLMN_request_details_ptr->requested_PLMN_id
      );
    }
    else
    {
      rr_individual_priorities_clear();
    }

    rr_individual_priorities_ie_free_children(&individual_priorities_ie);
  }
#endif /* FEATURE_LTE */

} /* rr_rel_channel_release_from_network() */


#ifdef FEATURE_GSM_DTM

/**
 * Sends a MPH_CHANNEL_RELEASE_REQ to L1 using the values for BCCH carrier
 * sent in the previous MPH_DTM_CHANNEL_RELEASE_REQ
 */
static void resend_l1_channel_release_req( )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  rr_conn_release_data_ptr->release_mode = RELEASE_MODE_GSM;
  
  MSG_GERAN_HIGH_0_G("Starting RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMER  guard timer while resending");
  (void)rr_timer_start(RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMER,
                       RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMEOUT);

  rr_send_mph_channel_release_req(
    rr_conn_release_data_ptr->channel_release_info.valid_bcch_carrier,
    rr_conn_release_data_ptr->channel_release_info.serving_bcch_carrier,
    rr_conn_release_data_ptr->channel_release_info.BSIC
  );
}

/**
 * Sends a MPH_CHANNEL_RELEASE_REQ or MPH_DTM_CHANNEL_RELEASE_REQ to L1
 * (dependant on rr_conn_release_data_ptr->release_mode)
 */
static void send_l1_channel_release_req(
  boolean valid_bcch_carrier,
  ARFCN_T serving_bcch_carrier,
  BSIC_T BSIC
)
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  rr_conn_release_data_ptr->channel_release_info.valid_bcch_carrier = valid_bcch_carrier;
  rr_conn_release_data_ptr->channel_release_info.serving_bcch_carrier = serving_bcch_carrier;
  rr_conn_release_data_ptr->channel_release_info.BSIC = BSIC;

  if ( rr_conn_release_data_ptr->release_mode == RELEASE_MODE_GSM )
  {
    rr_send_mph_channel_release_req(
      valid_bcch_carrier,
      serving_bcch_carrier,
      BSIC
    );
  }
  else
  #ifdef FEATURE_GSM_EDTM
  /* if CS is being released normally, and packet services can be maintained, */
  /* then the EDTM module will already have set the release mode accordingly: */
  /* this should therefore never happen when an abnormal release occurs (RLF) */
  if ( rr_conn_release_data_ptr->release_mode == RELEASE_MODE_EDTM )
  {
    rr_send_mph_dtm_channel_release_req(
      valid_bcch_carrier,
      serving_bcch_carrier,
      BSIC,
      FALSE /* preserve the PS connection */
    );
  }
  else
  #endif /* FEATURE_GSM_EDTM */
  {
    rr_send_mph_dtm_channel_release_req(
      valid_bcch_carrier,
      serving_bcch_carrier,
      BSIC,
      TRUE /* release the PS connection */
    );
  }

} /* end send_mph_dtm_channel_release_req() */

#endif /* FEATURE_GSM_DTM */


/**
 * Releases the physical channels (L1)
 */
static void rr_rel_deactivate_channels( )
{
  boolean                        valid_bcch_carrier;
  ARFCN_T                        serving_bcch_carrier;
  BSIC_T                         BSIC;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info();

  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  rr_set_link_status(NO_LINK);

  //RR_NULL_CHECK_FATAL(scell_info_ptr);
  BSIC = scell_info_ptr->gsm.BSIC;
  serving_bcch_carrier = scell_info_ptr->gsm.BCCH_ARFCN;

  if (rr_rel_is_cause_radio_link_failure())
  {
    /***********************************************************/
    /* Reset the camped on flag in system information database */
    /***********************************************************/
    scell_info_ptr->gsm.camped_on_flag = FALSE;

    if (rr_rel_is_reest_required())
    {
      // Prepeare RR-CELL-REEST for a re-establishment attempt
      rr_cell_reest_rlf_occurred();
    }

    rr_conn_release_data_ptr->rps_required_after_release = FALSE;

    /* L1 will not camp on the serving_bcch */
    valid_bcch_carrier = FALSE;
  }
  /*
  Check for reselection power scan after call release, unless the establishment was for
  a location update, SMS or supplimenatry service. Also make sure there are BA members.
  If the RPS is forced after a LAU in a weak signal area on a LA boundry
  the MS can jump around cells for a while.
  */
  else if (
    ( rr_ce_access_reason() != (byte)MM_RR_LOCATION_UPDATE ) &&
    ( rr_ce_access_reason() != (byte)MM_RR_SHORT_MESSAGE ) &&
    ( rr_ce_access_reason() != (byte)MM_RR_SUPPLEMENTARY_SERVICE ) &&
    ( rr_ded_ba_sacch_is_valid() ) &&
    rr_is_scell_ok_or_rps_required() )
  {
    MSG_GERAN_MED_0_G("Suspected poor scell - force RPS");

    // Passed to L1 in MPH_CHANNEL_RELEASE_REQ / MPH_DTM_CHANNEL_RELEASE_REQ
    valid_bcch_carrier = FALSE;

    // RPS is required when L1 confirms release
    rr_conn_release_data_ptr->rps_required_after_release = TRUE;
  }
  else
  {
    // Passed to L1 in MPH_CHANNEL_RELEASE_REQ / MPH_DTM_CHANNEL_RELEASE_REQ
    valid_bcch_carrier = TRUE;

    // RPS is not required when L1 confirms release
    rr_conn_release_data_ptr->rps_required_after_release = FALSE;
  }

  MSG_GERAN_HIGH_0_G("Starting RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMER  guard timer ");
  (void)rr_timer_start(RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMER,
                       RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMEOUT);

  #ifndef FEATURE_GSM_DTM
  rr_send_mph_channel_release_req(valid_bcch_carrier,serving_bcch_carrier,BSIC);
  #else
  send_l1_channel_release_req(valid_bcch_carrier,serving_bcch_carrier,BSIC);
  #endif /* FEATURE_GSM_DTM */

  /* log the new channel configuration */
  (void)rr_log_channel_configuration();
} /* rr_rel_deactivate_channels */

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
static boolean rr_pseudo_fr_return_to_tds( )
{
  rr_dedicated_data_T        *dedic_ptr = rr_get_dedicated_data_ptr();
  rr_ded_irat_measurements_t *tds_meas_ptr = rr_get_tds_ded_meas_ptr();
  uint8 i = 0;
  boolean return_to_tds = FALSE;


  if (dedic_ptr->tds_pseudo_fr.active)
  {
    /*Search for UARFCN's that have not been measured*/
    for (i=0; i<dedic_ptr->tds_pseudo_fr.num_freq; i++)
    {
      if (dedic_ptr->tds_pseudo_fr.freq_list[i].measured == FALSE)
      {
        MSG_GERAN_HIGH_1_G("RR TDS Pseudo FR UARFCN: %d not measured start G->T redir",
                 dedic_ptr->tds_pseudo_fr.freq_list[i].uarfcn);
        return_to_tds = TRUE;
        break;
      }
    }

    if (return_to_tds == FALSE)
    {
      /*Search for cells over the RSCP threshold*/
      for (i=0; i<tds_meas_ptr->no_of_entries; i++)
      {
        if (tds_meas_ptr->entries[i].data.utran_tdd.rscp > dedic_ptr->tds_pseudo_fr.rscp_threshold)
        {
          MSG_GERAN_HIGH_3_G("RR TDS Pseudo FR found suitable cell: %d, UARFCN: %d, RSCP: %ddBm start G->T redir",
                   tds_meas_ptr->entries[i].data.utran_tdd.cell_parameter_id,
                   tds_meas_ptr->entries[i].data.utran_tdd.uarfcn,
                   tds_meas_ptr->entries[i].data.utran_tdd.rscp);
          return_to_tds = TRUE;
          break;
        }
        else
        {
           MSG_GERAN_HIGH_3_G("RR TDS Pseudo FR UARFCN %d CPID %d RSCP %ddBm below threshold",tds_meas_ptr->entries[i].data.utran_tdd.uarfcn,
                                                                                    tds_meas_ptr->entries[i].data.utran_tdd.cell_parameter_id,
                                                                                    tds_meas_ptr->entries[i].data.utran_tdd.rscp);
        }
      }
    }

    if (return_to_tds == FALSE)
    {
      MSG_GERAN_MED_0_G("RR TDS Pseudo FR no suitable TDS cells found, remain on G");
    }
  }

  return return_to_tds;
}

#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

/**
 * Determine the next RR event after the channels have been deactivated
 */
static rr_event_T rr_conn_event_after_deactivation( )
{
  gprs_scell_info_t                      *scell_info_ptr = rr_gprs_get_scell_info();
  rr_event_T                              return_event   = EV_NO_EVENT;
  boolean                                 PLMN_ok = TRUE;
  sys_band_mask_e_type                    scell_band_mask;
  rr_PLMN_request_details_T              *rr_PLMN_request_details_ptr;

  rr_chan_rel_cell_selection_indicator_t *rr_cell_select_indicator_ptr;
//tlx
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr();
  rr_cell_select_indicator_ptr = rr_get_cell_select_indicator_ptr();

#ifdef FEATURE_SGLTE
#ifdef FEATURE_LTE
  /* Cancel G2L blind redirection if there is an ongoing BPLMN search*/
  if (rr_connection_release_get_g2l_blind_redir_flag() &&
      rr_is_sglte_mode_active())
  {
    if (rr_is_sglte_plmn_srch_active())
    {
      MSG_GERAN_HIGH_0_G("SGLTE cancel blind redirection ongoing BPLMN search");
      rr_connection_release_reset_g2l_blind_redir_flag();
    }
  }

  /*defer G2L blind redirection until we have camped as only the PS domain
    is redirected to LTE*/
  if (rr_connection_release_get_g2l_blind_redir_flag() &&
      rr_is_sglte_mode_active()) 
  {
    if (rr_PLMN_request_details_ptr->service_domain == SYS_SRV_DOMAIN_CS_PS)
    {
      MSG_GERAN_HIGH_0_G("SGLTE Defer G2L blind redirection until camped");
  
      rr_set_sglte_perform_g2l_blind_redir(TRUE);

      rr_connection_release_reset_g2l_blind_redir_flag();
    }
    else
    { 
      /*g2l_blind_redir_required should not be set if service domain is not CS_PS, abandon blind redir*/     
      MSG_GERAN_ERROR_1_G("SGLTE Attempting LTE blind redir in domain:%d",rr_PLMN_request_details_ptr->service_domain);
      rr_connection_release_reset_g2l_blind_redir_flag();
    }
  }
#endif /* FEATURE_LTE */
#endif /*FEATURE_SGLTE*/

  /**
   * If the CS connection was released due to Radio Link Failure, then the reestablishment module is used to 
   * try and find a suitable cell to attempt to reestablish the call.
   */
  if (rr_rel_is_cause_radio_link_failure())
  {
    if (rr_rel_is_reest_required())
    {
      return_event = EV_CHOOSE_CELL_FOR_REEST;
    }
    else
    {
      return_event = EV_CHOOSE_CELL_FOR_CAMP;
    }
  }

  else if ((rr_cell_select_indicator_ptr->valid == TRUE) &&
           (rr_cell_select_indicator_ptr->type != UNKNOWN_TARGET))
  {
#ifdef FEATURE_LTE
    /*Reset the FRLTE flag as return event would have taken care of the susbsequent action.*/
    rr_connection_release_reset_g2l_blind_redir_flag();
    rr_connection_release_reset_g2l_blind_redir_after_csfb_flag();  
#endif /* FEATURE_LTE */
    return_event = EV_CHOOSE_CELL_BY_CELL_SELECTION_INDICATOR;
  }
#ifdef FEATURE_LTE
  
  else if (rr_connection_release_get_g2l_blind_redir_flag() || 
          (rr_connection_release_get_g2l_blind_redir_after_csfb_flag()))
  {
    MSG_GERAN_HIGH_0_G(" G2L blind redirection event triggred ");
    /*Reset the FRLTE flag as return event would  take care of the susbsequent action.*/
    rr_connection_release_reset_g2l_blind_redir_flag();
    rr_connection_release_reset_g2l_blind_redir_after_csfb_flag();	
    return_event = EV_G2L_BLIND_REDIR_REQUIRED;
  }
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
  else if (rr_pseudo_fr_return_to_tds() == TRUE)
  {
    return_event = EV_G2T_PSEUDO_FR_REQUIRED;
  }
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

  else

  /**
   * If the RR_EST_REQ was for an emergency call and was received during cell
   * selection, it is possible that RR is camped on a cell that is not in the
   * requested PLMN.  If this is the case, cell selection should be triggered.
   */
  if (!rr_is_any_cell_selection_mode())
  {
    /**
     * If the serving cell's PLMN is not known (indicated by valid_data_flag
     * and valid_dedicated_data_flag being FALSE) then there is no need to
     * check that the serving PLMN matches the requested PLMN - a complete SI
     * acquisition will be scheduled before starting idle mode on the cell so
     * any mismatch in PLMNs will be detected then.
     *
     * This might be the case if a handover has been performed and SI5/6 have
     * not been received before the connection is released.
     */
    if (scell_info_ptr->gsm.valid_data_flag ||
        scell_info_ptr->gsm.valid_dedicated_data_flag)
    {
      PLMN_ok = rr_cell_selection_PLMN_match(
        rr_PLMN_request_details_ptr,
        &scell_info_ptr->gsm.location_area_identification
      );

      if (!PLMN_ok)
      {
        MSG_GERAN_HIGH_0_G("Serving cell PLMN is not requested PLMN, perform cell selection");
        return_event = EV_CHOOSE_CELL;
      }
    }

    /*MS is initially in 900/1800 band. Before Initiating the emergency call SD internally changes
     *the band_pref to all the bands.Once Emergency call is released and after 4 sec SD internally
     *changes the band_pref to the previous bands i.e 900/1800 bands. This SD change of band_pref
     *is not known to RR level. If during the emergency call MS handover to a band(850 band) which
     *is not initally supported then RR needs to do cell selection after CS call release. RR needs to see
     *PLMN SELECT BAND PREF and validate whether RR is in correct band or not if we are in diff band
     *RR needs to cell selection after CS call release.
     */
    if(rr_ce_access_reason() == (byte)MM_RR_EMERGENCY_CALL)
    {
      scell_band_mask = rr_convert_band_info(scell_info_ptr->gsm.BCCH_ARFCN.band);

      if(!(scell_band_mask & rr_band_pref()))
      {
        MSG_GERAN_HIGH_0_G("Serving cell Band is not present in plmn request band pref, perform cell selection");
        return_event = EV_CHOOSE_CELL;
      }
    }
  }
  else

  /* Check if cell is already in the barred cell list due to authentication failure */
  if (rr_is_cell_in_barred_cell_list(
        scell_info_ptr->gsm.BCCH_ARFCN,
        scell_info_ptr->gsm.BSIC
      )
     )
  {
    /* Should start cell reselection. Spec ref: 43.022 section 3.5.5*/
    rr_ce_rel_rps_required_after_release();
  }

  /* Check for a pending MM message to process */
  /* Note: This will take priority over a triggered cell selection */
  if (grr_examine_saved_mm_message(NULL))
  {
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
    if (rr_x2g_cco_in_progress())
    {
      MSG_GERAN_HIGH_0("X2G CCO in progress, returning EV_NO_EVENT so idle is resumed");
      return_event = EV_NO_EVENT;
    }
    else
    {
      /* process the message from a position of being uncamped */
      return_event = RR_EV_PROCESS_SAVED_MESSAGE;
    }
#else
    /* process the message from a position of being uncamped */
    return_event = RR_EV_PROCESS_SAVED_MESSAGE;
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
  }

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  if (TRUE == rr_static_get_suspend_in_progress())
  {
    MSG_GERAN_MED_0_G("DMMSUSREM:: CR EV_PROCESS_SAVED_msg");
    //rr_perform_backup_for_scell();
    return_event = RR_EV_PROCESS_SAVED_MESSAGE;
  }
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */

  /* for all of the events above, RR should behave as if no longer camped */
  if (return_event != EV_NO_EVENT)
  {
    scell_info_ptr->gsm.camped_on_flag = FALSE;

    rr_gprs_deactivate_timed_sys_info_refresh();
  }

#ifdef FEATURE_GSM_BAND_AVOIDANCE
  {
    /*call has ended send 0 entry freq list*/
    rr_band_avoid_ded_freq_upd_send_imsg(NULL);
  }
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

  MSG_GERAN_MED_1_G("RR event on release confirmation = %d", (int)return_event);

  return(return_event);
}

/**
  @brief Checks whether the specified RR_CONNECTION_RELEASE state is expected to be transitory or not. 
  @param state The state to check.
  @return TRUE if the state is transitory, FALSE if not.
*/
static boolean rr_connection_release_state_is_transitory(rr_release_state_T state)
{
  switch (state)
  {
    case RR_REL_PENDING_WAIT_FOR_DTM_RELEASE:
    {
      return TRUE;
    }

    default: ;
  }

  return FALSE;
}

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
boolean rr_power_scan_on_edtm_release( )
{
  return(rr_is_scell_ok_or_rps_required());
}
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

#ifdef FEATURE_LTE
void rr_connection_release_set_g2l_blind_redir_flag( )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  MSG_GERAN_HIGH_0_G("G2L blind redirection will be performed on connection release");
  rr_conn_release_data_ptr->g2l_blind_redir_required = TRUE;
}

void rr_connection_release_reset_g2l_blind_redir_flag( )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  MSG_GERAN_HIGH_0_G("G2L blind redirection flag now reset to FALSE");
  rr_conn_release_data_ptr->g2l_blind_redir_required = FALSE;
}

boolean rr_connection_release_get_g2l_blind_redir_flag( )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  MSG_GERAN_HIGH_1_G("G2L blind redirection flag=%d", 
    rr_conn_release_data_ptr->g2l_blind_redir_required);
  return rr_conn_release_data_ptr->g2l_blind_redir_required;
}

void rr_connection_release_set_g2l_blind_redir_after_csfb_flag( )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  MSG_GERAN_HIGH_0_G("G2L blind redirection after CSFB will be performed on connection release");
  rr_conn_release_data_ptr->g2l_blind_redir_after_csfb_required = TRUE;
}

void rr_connection_release_reset_g2l_blind_redir_after_csfb_flag( )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  MSG_GERAN_HIGH_0_G("G2L blind redirection after CSFB flag now reset to FALSE");
  rr_conn_release_data_ptr->g2l_blind_redir_after_csfb_required = FALSE;
}

boolean rr_connection_release_get_g2l_blind_redir_after_csfb_flag( )
{
  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  MSG_GERAN_HIGH_1_G("G2L blind redirection after CSFB flag=%d", 
    rr_conn_release_data_ptr->g2l_blind_redir_after_csfb_required);
  return rr_conn_release_data_ptr->g2l_blind_redir_after_csfb_required;
}
#endif /* FEATURE_LTE */

/**
 * RR connection release state machine
 *
 * @param release_event event for the state machine
 *
 * @param message_ptr   pointer to input message (valid when event is EV_INPUT_MESSAGE)
 *
 * @return event for calling state machine
 */
rr_event_T rr_connection_release(
  rr_event_T release_event,
  rr_cmd_bdy_type* message_ptr
)
{
  rr_event_T      new_rr_event = EV_NO_EVENT;
  rr_event_T      new_release_event = EV_NO_EVENT;
  byte            message_id = 0;
  byte            message_set= 0;

  // Obtain a pointer to the module data
  release_info_t *rr_conn_release_data_ptr = rr_conn_release_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_conn_release_data_ptr);

  while (release_event != EV_NO_EVENT)
  {
    if (release_event == EV_RESET_SOFTWARE)
    {
      MSG_GERAN_LOW_0_G("Resetting connection release module");

      /* do not process the reset event any further */
      release_event = EV_NO_EVENT;
      rr_conn_release_data_ptr->release_state = RR_REL_NO_CONNECTION;
      rr_conn_release_data_ptr->rps_required_after_release = FALSE;
#ifdef FEATURE_LTE
      rr_connection_release_reset_g2l_blind_redir_flag();
      rr_connection_release_reset_g2l_blind_redir_after_csfb_flag();
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
      rr_tds_pseudo_fr_reset_dedicated_info();
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/
    }
    else if (release_event == EV_INPUT_MESSAGE)
    {
      /************************************************************/
      /* Handle some input messages in a state independent manner */
      /************************************************************/
      if (message_ptr != NULL)
      {
        /* set the message_set & message_id for all input messages */
        message_set = message_ptr->message_header.message_set;
        message_id =  message_ptr->message_header.message_id;
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
        if (message_set == MS_RR_L1 && message_id == MPH_PROC_IND)
        {
          /* Treat this same as aborted due to other high priority NAS message for reusage of code */
          rr_conn_set_release_cause(RELEASE_CAUSE_SUSPEND_IND);
          release_event = RR_EV_MM_REQ_RECEIVED_ABORT;
        }
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */
      }
      else
      {
        MSG_GERAN_ERROR_0_G("message_ptr was NULL!");

        /* exit from the loop now */
        release_event = EV_NO_EVENT;
      }
    } /* EV_INPUT_MESSAGE */

    /* an EV_NO_EVENT should not be processed by state handlers */
    if (release_event == EV_NO_EVENT)
    {
      continue;
    }

    switch (rr_conn_release_data_ptr->release_state)
    {
      /* No RR connection exists */
      case RR_REL_NO_CONNECTION:
      {
        switch (release_event)
        {
          case EV_CONNECTION_EXISTS:
          {
            // A Layer 2 connection has been established

            rr_chan_rel_cell_selection_indicator_t * cell_selection_after_conn_release_indicator;

            cell_selection_after_conn_release_indicator = rr_get_cell_select_indicator_ptr();

            /* Initialise data in preparation for the connection release */
            rr_conn_release_data_ptr->rps_required_after_release = FALSE;

            cell_selection_after_conn_release_indicator->valid = FALSE;

#ifdef FEATURE_LTE
            rr_connection_release_reset_g2l_blind_redir_flag();
            rr_connection_release_reset_g2l_blind_redir_after_csfb_flag();
#endif /* FEATURE_LTE */

            rr_conn_release_data_ptr->release_state = RR_REL_CHANNEL_CONNECTED;
            break;
          }

          case EV_PROCEDURE_COMPLETE:
          {
            break; /* Ignore this event, no action necessary. */
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(
              rr_event_name(release_event),
              rr_release_state_name(rr_conn_release_data_ptr->release_state)
            );
          }
        }
        break;
      } /* RR_REL_NO_CONNECTION */

      /******************************************************/
      /* A channel is connected and RR is in DEDICATED mode */
      /******************************************************/
      case RR_REL_CHANNEL_CONNECTED:
      {
        switch (release_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &message_ptr->rr;

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_CE_IMSG:
                  {
                    switch (rr_rr_msg_ptr->ce.imsg.header.imh.message_id)
                    {
                      case RR_CE_IMSG_CS_ABORT_REQ:
                      {
                        uint8 release_cause;
                        rr_ce_imsg_cs_abort_req_t *msg_ptr = &rr_rr_msg_ptr->ce.imsg.cs_abort_req;

                        if (msg_ptr->l1_in_null)
                        {
                          rr_ce_rel_rps_required_after_release();
                        }

                        MSG_GERAN_HIGH_1_G("Release requested by NAS (cause %1d)",
                                 rr_conn_release_data_ptr->reported_cause);

                        /**
                         * Note: During Connection Establishment, once L2 establishment is in progress, a 
                         * RR_ABORT_REQ received will be re-directed here and this module will provide the 
                         * release functionality. In these cases, the 'reported_cause' will already have been 
                         * setup. 
                         *  
                         * RR_ABORT_REQ will also be received here due to a MM abort request after CS is 
                         * connected. In this case, the 'reported_cause' will not already have been setup. 
                         */
                        if (rr_conn_release_data_ptr->reported_cause == RELEASE_CAUSE_NONE)
                        {
                          release_cause = RELEASE_CAUSE_NAS_REQUEST;
                        }
                        else
                        {
                          release_cause = rr_conn_release_data_ptr->reported_cause;
                        }

                        if (rr_ded_proc_in_progress())
                        {
                          MSG_GERAN_LOW_0_G("Wait for procedure to complete before releasing");
                          rr_conn_set_release_cause(release_cause);

                          rr_conn_release_data_ptr->release_state =
                            RR_REL_PENDING_WAIT_FOR_PROCEDURE_TO_COMPLETE;
                        }
                        else
                        {
                          // Setup the release cause which is queried later to determine the next action
                          rr_conn_set_release_cause(
                            release_cause   // release_cause
                          );
#ifdef FEATURE_GSM_DTM
                          // Send CS release request to the DTM module
                          rr_dtm_send_imsg_cs_release_req(
                            TRUE    // edtm_allowed
                          );

                          // Wait for RR_DTM_IMSG_CS_RELEASE_CNF
                          rr_conn_release_data_ptr->release_state = RR_REL_PENDING_WAIT_FOR_DTM_RELEASE;

                          // Note: RR-CONTROL is not informed of connection release until DTM module confirms

#else /* FEATURE_GSM_DTM */

                          // Release L2 locally (DL_RELEASE_REQ)
                          rr_rel_release_data_link(
                            release_cause,
                            NORMAL_RELEASE
                          );

                          // Wait for DL_RELEASE_CNF
                          rr_conn_release_data_ptr->release_state =
                            RR_REL_PENDING_MAIN_SIGNALLING_DISCONNECTION;

                          // Inform RR-CONTROL that CS connection is being released
                          new_rr_event = RR_EV_CONNECTION_RELEASE_IN_PROGRESS;
#endif /* FEATURE_GSM_DTM */
                        }
                        break;
                      } // case RR_CE_IMSG_CS_ABORT_REQ

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message_ptr,
                          rr_release_state_name(rr_conn_release_data_ptr->release_state)
                        );
                      }
                    }
                    break;
                  } // case RR_CE_IMSG

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_release_state_name(rr_conn_release_data_ptr->release_state)
                    );
                  }
                }
                break;
              } // case MS_RR_RR

              case MS_MM_RR:
              {
                switch (message_ptr->mm.header.message_id)
                {
                  case RR_ABORT_REQ:
                  {
                    rr_ce_send_imsg_cs_abort_req(
                      FALSE,   // ded_proc_in_progress
                      NULL    // ded_proc_details_ptr
                    );
                    break;
                  }

#ifdef FEATURE_GSM_DTM
                  case RR_GMM_GPRS_AUTH_FAIL_IND:
                  {
                    MSG_GERAN_HIGH_0_G("Release due to auth & ciphering failure");

                    if (rr_ded_proc_in_progress())
                    {
                      MSG_GERAN_LOW_0_G("Wait for procedure to complete before releasing");
                      rr_conn_set_release_cause(RELEASE_CAUSE_IMPLICIT_NAS_REQUEST);
                      rr_conn_release_data_ptr->release_state = RR_REL_PENDING_WAIT_FOR_PROCEDURE_TO_COMPLETE;
                    }
                    else
                    {
                      MSG_GERAN_HIGH_0_G("Release connection to process MM message");

                      // Setup the release cause which is queried later to determine the next action
                      rr_conn_set_release_cause(
                        RELEASE_CAUSE_IMPLICIT_NAS_REQUEST   // release_cause
                      );

                      // Send CS release request to the DTM module
                      rr_dtm_send_imsg_cs_release_req(
                        TRUE    // edtm_allowed
                      );

                      // Wait for RR_DTM_IMSG_CS_RELEASE_CNF
                      rr_conn_release_data_ptr->release_state = RR_REL_PENDING_WAIT_FOR_DTM_RELEASE;

                      // Note: RR-CONTROL is not informed of connection release until DTM module confirms
                    }
                    break;
                  }
#endif /* FEATURE_GSM_DTM */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_release_state_name(rr_conn_release_data_ptr->release_state)
                    );
                  }
                }
                break;
              }

              case MS_RR_L2:
              {
                switch (message_ptr->mm.header.message_id)
                {
                  case DL_DATA_IND:
                  {
                    // A CHANNEL RELEASE has arrived from the network
                    // Note: The DL_DATA_IND is only forwarded if it contains a CHANNEL RELEASE message

                    boolean default_gprs_resumption = FALSE;

#ifdef FEATURE_GSM_EDTM
                    /* allow the EDTM module to prepare for the release */
                    default_gprs_resumption = rr_gprs_edtm_process_channel_release();
#endif /* FEATURE_GSM_EDTM */

                    MSG_GERAN_HIGH_0_G("Release requested by NW");

                    // Process CHANNEL RELEASE message
                    rr_rel_channel_release_from_network(
                      default_gprs_resumption   // default_resumption_ind
                    );

                    // Store the reason for the release
                    rr_conn_set_release_cause(RELEASE_CAUSE_NETW_CHANNEL_RELEASE);

#ifdef FEATURE_GSM_DTM
                    // Send CS release request to the DTM module
                    rr_dtm_send_imsg_cs_release_req(
                      TRUE    // edtm_allowed
                    );

                    // Wait for RR_DTM_OMSG_CS_RELEASE_CNF
                    rr_conn_release_data_ptr->release_state = RR_REL_PENDING_WAIT_FOR_DTM_RELEASE;

                    // Note: RR-CONTROL is not informed of the CS conn release until the DTM module confirms

#else /* FEATURE_GSM_DTM */

                    // Release L2 normally (DL_RELEASE_REQ)
                    rr_rel_release_data_link(
                      RELEASE_CAUSE_NETW_CHANNEL_RELEASE,
                      NORMAL_RELEASE
                    );

                    // Wait for DL_RELEASE_CNF
                    rr_conn_release_data_ptr->release_state = RR_REL_PENDING_MAIN_SIGNALLING_DISCONNECTION;

                    // Inform RR-CONTROL that CS connection is being released
                    new_rr_event = RR_EV_CONNECTION_RELEASE_IN_PROGRESS;
#endif /* FEATURE_GSM_DTM */
                    break;
                  }

                  case DL_RELEASE_IND:
                  case DL_RELEASE_CNF:
                  {
                    // The signalling link (L2) has been released autonomously (either from MS or network)

                    MSG_GERAN_HIGH_0_G("Signalling Link Disconnected");

                    // Ensure L2 link is reset later rather than released normally
                    rr_set_link_status(NO_LINK);

                    // This is treated as a normal channel release (rather than a RLF, etc)
                    rr_conn_set_release_cause(RELEASE_CAUSE_NETW_CHANNEL_RELEASE);

#ifdef FEATURE_GSM_DTM
                    // Send CS release request to the DTM module
                    rr_dtm_send_imsg_cs_release_req(
                      TRUE    // edtm_allowed
                    );

                    // Wait for RR_DTM_OMSG_CS_RELEASE_CNF
                    rr_conn_release_data_ptr->release_state = RR_REL_PENDING_WAIT_FOR_DTM_RELEASE;

                    // Note: RR-CONTROL is not informed of the CS conn release until the DTM module confirms
                    // Check if L1 can handle the release message
                    if (rr_conn_release_data_ptr->l1_in_null)
                    {
                      MSG_GERAN_HIGH_0_G("L1 in NULL - skip release");

                      // No need to release L1 (MPH_CHANNEL_RELEASE_REQ will not be sent)
                      new_release_event = EV_CHANNEL_DEACTIVATED;
                    }
                    else
#else /* FEATURE_GSM_DTM */
                    {
                      // Release L1 (MPH_CHANNEL_RELEASE_REQ will be sent)
                      rr_rel_deactivate_channels();
                    }

                    // Wait for MPH_CHANNEL_RELEASE_CNF (if required)
                    rr_conn_release_data_ptr->release_state = RR_REL_MAIN_SIGNALLING_DISCONNECTED;

                    // Inform RR-CONTROL that CS connection is being released
                    new_rr_event = RR_EV_CONNECTION_RELEASE_IN_PROGRESS;
#endif /* FEATURE_GSM_DTM */
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_release_state_name(rr_conn_release_data_ptr->release_state)
                    );
                  }
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message_ptr,
                  rr_release_state_name(rr_conn_release_data_ptr->release_state)
                );
              }
            }
            break;
          } // case EV_INPUT_MESSAGE

          case EV_RADIO_LINK_FAILURE:
          {
            // Radio Link Failure

            MSG_GERAN_HIGH_0_G("Release Link (RLF)");

            // Setup the release cause which is queried later to determine the next action
            rr_conn_set_release_cause(
              RELEASE_CAUSE_RADIO_LINK_FAILURE   // release_cause
            );

#ifdef FEATURE_GSM_DTM
            // Send CS release request to the DTM module
            rr_dtm_send_imsg_cs_release_req(
              FALSE   // edtm_allowed
            );

            // Wait for RR_DTM_IMSG_CS_RELEASE_CNF
            rr_conn_release_data_ptr->release_state = RR_REL_PENDING_WAIT_FOR_DTM_RELEASE;

            // Note: RR-CONTROL is not informed of connection release until DTM module confirms

#else /* FEATURE_GSM_DTM */

            // Release L2 locally (DL_RELEASE_REQ)
            rr_rel_release_data_link(
              RELEASE_CAUSE_RADIO_LINK_FAILURE,
              LOCAL_END_RELEASE
            );

            // Wait for DL_RELEASE_CNF
            rr_conn_release_data_ptr->release_state = RR_REL_PENDING_MAIN_SIGNALLING_DISCONNECTION;

            // Inform RR-CONTROL that CS connection is being released
            new_rr_event = RR_EV_CONNECTION_RELEASE_IN_PROGRESS;
#endif /* FEATURE_GSM_DTM */
            break;
          } /* EV_RADIO_LINK_FAILURE */

          case RR_EV_MM_REQ_RECEIVED_ABORT:
          {
            // Abort requested (or implicitly required) by upper layers

            rr_conn_release_cause_t release_cause = RELEASE_CAUSE_IMPLICIT_NAS_REQUEST;
            release_mode_T          release_mode = NORMAL_RELEASE;

            if (rr_conn_release_data_ptr->reported_cause == RELEASE_CAUSE_SUSPEND_IND)
            {
              release_cause = RELEASE_CAUSE_SUSPEND_IND;
              release_mode = LOCAL_END_RELEASE;
            }

            if (rr_ded_proc_in_progress())
            {
              MSG_GERAN_LOW_0_G("Wait for procedure to complete before releasing");
              rr_conn_set_release_cause(release_cause);
              rr_conn_release_data_ptr->release_state = RR_REL_PENDING_WAIT_FOR_PROCEDURE_TO_COMPLETE;
            }
            else
            {
              MSG_GERAN_HIGH_0_G("Release connection to process MM message");

              // Setup the release cause which is queried later to determine the next action
              rr_conn_set_release_cause(release_cause);

#ifdef FEATURE_GSM_DTM
              // Send CS release request to the DTM module
              rr_dtm_send_imsg_cs_release_req(
                TRUE    // edtm_allowed
              );

              // Wait for RR_DTM_IMSG_CS_RELEASE_CNF
              rr_conn_release_data_ptr->release_state = RR_REL_PENDING_WAIT_FOR_DTM_RELEASE;

              // Note: RR-CONTROL is not informed of connection release until DTM module confirms

#else /* FEATURE_GSM_DTM */

              // Release L2 locally (DL_RELEASE_REQ)
              rr_rel_release_data_link(
                release_cause,
                release_mode
              );

              // Wait for DL_RELEASE_CNF
              rr_conn_release_data_ptr->release_state = RR_REL_PENDING_MAIN_SIGNALLING_DISCONNECTION;

              // Inform RR-CONTROL that CS connection is being released
              new_rr_event = RR_EV_CONNECTION_RELEASE_IN_PROGRESS;
#endif /* FEATURE_GSM_DTM */
            }
            break;
          }

          case EV_PROCEDURE_COMPLETE:
          {
            break; /* Ignore this event, no action necessary. */
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(
              rr_event_name(release_event),
              rr_release_state_name(rr_conn_release_data_ptr->release_state)
            );
          }
        }
        break;
      } /* RR_REL_CHANNEL_CONNECTED */
      break;

      /************************************************************************/
      /* RR is waiting for DL_RELEASE_CNF or DL_RELEASE_IND messages from L2  */
      /* indicating that the main signalling link has been disconnected (note */
      /* that any messages related to the SAPI3 link are handled by the SAPI3 */
      /* module)                                                              */
      /************************************************************************/
      case RR_REL_PENDING_MAIN_SIGNALLING_DISCONNECTION:
      {
        if (release_event == EV_INPUT_MESSAGE)
        {
          #ifdef FEATURE_GSM_DTM
          /*
          It is possible that L1/MAC were in the process of releasing from DTM when the
          CS channel release was received. If so, L1 may exit DTM after L2 has been
          requested to release, and therefore GSM signalling should now be used
          */
          if ((message_set == MS_RR_L1) &&
              (message_id == (byte) MPH_DTM_CHANNEL_RELEASE_IND) )
          {
            mph_dtm_channel_release_ind_t *mph_dtm_channel_release_ind =
              (mph_dtm_channel_release_ind_t *)message_ptr;

            /*
            As RR hasn't yet requested L1 to release the CS via a
            MPH_DTM_CHANNEL_RELEASE_REQ, then reception of this message
            can only mean that the PS has been released
            */
            if (mph_dtm_channel_release_ind->ps_release == TRUE)
            {
              rr_conn_release_data_ptr->release_mode = RELEASE_MODE_GSM;
            }

            if ( (mph_dtm_channel_release_ind->cs_release == TRUE) ||
                 (mph_dtm_channel_release_ind->ps_release == FALSE) )
            {
              MSG_GERAN_ERROR_3_G("Unexpected release flags: CS=%1d PS=%1d EC=%1d",
                        (int)mph_dtm_channel_release_ind->cs_release,
                        (int)mph_dtm_channel_release_ind->ps_release,
                        (int)mph_dtm_channel_release_ind->error_code);
            }
          }
          else
          #endif /* FEATURE_GSM_DTM */

          /***************************************************************/
          /* The layer 2 confirms the release of the main signalling link*/
          /***************************************************************/
          if ((message_set == MS_RR_L2) &&
              ((message_id == (byte) DL_RELEASE_CNF) ||
               (message_id == (byte) DL_RELEASE_IND)))
          {
            (void)rr_timer_cancel(T3110);
            MSG_GERAN_HIGH_0_G("Stopped T3110");

            // Check if L1 has a channel connected in dedicated
            if (rr_ded_l1_chan_is_connected())
            {
              // Release L1 (MPH_CHANNEL_RELEASE_REQ will be sent)
              rr_rel_deactivate_channels();
            }
            else
            {
              // L1 has no ded channel connected - skip the release
              MSG_GERAN_HIGH_0_G("No ded chan connected in GL1 - skip release");

              // No need to release L1 (MPH_CHANNEL_RELEASE_REQ will not be sent)
              new_release_event = EV_CHANNEL_DEACTIVATED;
            }

            // Wait for MPH_CHANNEL_RELEASE_CNF (if required)
            rr_conn_release_data_ptr->release_state = RR_REL_MAIN_SIGNALLING_DISCONNECTED;
          }

          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received RR_ABORT_REQ, already releasing");

            /* Ensure a RR_ABORT_CNF is returned */
            rr_conn_set_release_cause(RELEASE_CAUSE_NAS_REQUEST);
          }
          else
          {
            /* unexpected message received while in this state */
            ERR("UnexMsg:State(%d),Id(%d,%d)",
                (int) rr_conn_release_data_ptr->release_state,(int) message_set, (int) message_id);
          }
        } /* EV_INPUT_MESSAGE */

        /*****************************************************************/
        /* The timer T3110 has expired. We consider that the main        */
        /* signalling link is released                                   */
        /*****************************************************************/
        else if (release_event == EV_T3110_TIMEOUT)
        {
          MSG_GERAN_HIGH_0_G("T3110 expired");

          /* force the link to release */
          rr_send_mdl_release_req(SAPI0,DCCH);

          /* remain in the same state to handle the response from L2 */
        }

        /***********************************************************************/
        /* RR_EV_MM_REQ_RECEIVED_ABORT                                         */
        /* A message has been received from MM (such as a PLMN LIST REQ) which */
        /* requires the connection to be released, and then the MM message     */
        /* processed.                                                          */
        /***********************************************************************/
        else if (release_event == RR_EV_MM_REQ_RECEIVED_ABORT )
        {
          MSG_GERAN_HIGH_0_G("Implicit abort required - already releasing");

          /* Ensure a RR_ABORT_IND is returned */
          if (rr_conn_release_data_ptr->reported_cause != RELEASE_CAUSE_SUSPEND_IND)
            rr_conn_set_release_cause(RELEASE_CAUSE_IMPLICIT_NAS_REQUEST);
        }
      } /* RR_REL_PENDING_MAIN_SIGNALLING_DISCONNECTION */
      break;

      case RR_REL_MAIN_SIGNALLING_DISCONNECTED:
      {
        if (release_event == EV_INPUT_MESSAGE)
        {
          /* L1 confirms release of CS channel */
          if ((message_set == MS_RR_L1) &&
              (message_id == (byte) MPH_CHANNEL_RELEASE_CNF))
          {
            new_release_event = EV_CHANNEL_DEACTIVATED;
          }

          #ifdef FEATURE_GSM_DTM
          else
          if ( (message_set == MS_RR_L1) &&
               (message_id == (byte) MPH_DTM_CHANNEL_RELEASE_IND) )
          {
            mph_dtm_channel_release_ind_t *mph_dtm_channel_release_ind =
              (mph_dtm_channel_release_ind_t *)message_ptr;

            /*
            RR hasnt requested L1 to release the CS via a MPH_DTM_CHANNEL_RELEASE_REQ,
            so a MPH_DTM_CHANNEL_RELEASE_IND is expected. However, it is possible that
            a TBF release was in progress at the time, and L1 just posted a
            MPH_DTM_CHANNEL_RELEASE_IND indicating PS release just as we sent the CS
            release request. So it is necessary to check the release flags
            */
            if ( mph_dtm_channel_release_ind->cs_release )
            {
              new_release_event = EV_CHANNEL_DEACTIVATED;
            }
            else
            /*
            L1 has indicated that the CS has not been released. This must mean that a TBF
            release had just completed at the time RR posted the CS release request.
            This means L1 has transitioned from DTM to GSM Dedicated, and will therefore
            ignore the MPH_DTM_CHANNEL_RELEASE_REQ just posted.
            Therefore, send a MPH_CHANNEL_RELEASE_REQ now to release the CS
            */
            if ( mph_dtm_channel_release_ind->ps_release )
            {
              MSG_GERAN_HIGH_0_G("L1 released PS only - resend CS release request");
              resend_l1_channel_release_req();
            }
            else
            {
              MSG_GERAN_ERROR_2_G("Unexpected release flags: CS=%d PS=%d",
                        mph_dtm_channel_release_ind->cs_release,
                        mph_dtm_channel_release_ind->ps_release);
            }
          }
          #endif /* FEATURE_GSM_DTM */

          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_RELEASE_IND))
          {
            /* message can be ignored */
          }
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            /* handled by state independent handler */
            MSG_GERAN_HIGH_0_G("RR_ABORT_REQ received, already releasing");

            /* Ensure a RR_ABORT_CNF is returned */
            rr_conn_set_release_cause(RELEASE_CAUSE_NAS_REQUEST);
          }
          else
          {
            /* unexpected message received while in this state */
            ERR("UnexMsg:State(%d),Id(%d,%d)",
                (int) rr_conn_release_data_ptr->release_state,(int) message_set, (int) message_id);
          }
        } /* EV_INPUT_MESSAGE */

        else if (release_event == EV_CHANNEL_DEACTIVATED)
        {
          // Set status to undefined
          rr_ce_rel_status_e ce_rel_status = RR_CE_REL_STATUS_MAX;

          // Default behaviour is to delete GMM signalling PDUs after CS connection release
          boolean purge_gmm_signalling = TRUE;

          rr_conn_release_cause_t release_cause = rr_conn_get_release_cause();
          rr_dedicated_data_T *dedic_ptr = rr_get_dedicated_data_ptr();

          MSG_GERAN_HIGH_0_G("Cancelling RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMER guard timer");
          (void)rr_timer_cancel(RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMER);

          if (dedic_ptr != NULL)
          {
            /* reset the channel configuration data */
            dedic_ptr->curr_channel_spec.number_of_ded_channels = 0;
            dedic_ptr->curr_channel_spec.cipher_flag            = FALSE;
            rr_set_link_status(NO_LINK);
          }

          /**
           * If a X2G HO occurred during the call, then if the PLMN of the current cell is known, set the RR 
           * EPLMN to the PLMN of the current cell. This is a workaround for the case where a RAU is performed
           * during the call (DTM) and the EPLMN list is deleted because the RAU ACCEPT has an empty EPLMN 
           * list (which is a network issue). 
           * Note that this is not done if in LIMITED.
           */
          if (rr_ce_conn_originated_on_other_rat())
          {
            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();
            
            //RR_NULL_CHECK_FATAL(scell_info_ptr);

            if (scell_info_ptr->gsm.cell_identity_valid_flag &&
                (rr_is_limited_plmn_selection_mode() == FALSE))
            {
              rr_set_equivalent_plmn(&scell_info_ptr->gsm.location_area_identification.plmn_id);
            }
          }

          if (release_cause == RELEASE_CAUSE_PCCO_FAILURE)
          {
            new_rr_event = EV_PCCO_FAILURE_CCO_TIMER_EXPIRY;
          }
          else
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
          if (release_cause == RELEASE_CAUSE_X2G_CCO_FAILURE)
          {
            /* The only possible way for a CCO to fail and require connection release procedures
            to be used is if T3174 expires during contention resolution phase. */
            new_rr_event = EV_X2G_CCO_FAILURE_CCO_TIMER_EXPIRY;
          }
          else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
          if (rr_gprs_edtm_ps_allowed_after_cs_release(
                rr_conn_release_data_ptr->release_mode
              )
             )
          {
            /* Allow EDTM module to complete the release signalling: */
            /* this would normally be handled by the DTM module, but */
            /* that would also cause an unnecessary GPRS suspension. */
            rr_gprs_edtm_complete_cs_release();

            (void) rr_connection_establishment_control(EV_CHANNEL_DEACTIVATED, NULL);

            ce_rel_status = RR_CE_REL_STATUS_CAMPED;

            // PS is not released during E-DTM release, so there is no deleting of PDUs
            purge_gmm_signalling = FALSE;
          }
          else
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
          {
            /* determine the appropriate event */
            new_rr_event = rr_conn_event_after_deactivation();
          }

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
          if (new_rr_event != EV_G2T_PSEUDO_FR_REQUIRED)
          {
            rr_tds_pseudo_fr_reset_dedicated_info();
          }
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

          /* The channel release was successful, and L1 is able to camp back on the */
          /* serving cell (or RPS is required) */
          if (new_rr_event == EV_NO_EVENT)
          {
            inter_task_BCCH_allocation_T *ba_sacch_ptr = NULL;

            if (ce_rel_status != RR_CE_REL_STATUS_CAMPED)
            {
              ce_rel_status = RR_CE_REL_STATUS_CAMP_ON_CELL;

              if (rr_conn_release_data_ptr->rps_required_after_release)
              {
                // Indicate RPS required to RR-CONTROL
                ce_rel_status = RR_CE_REL_STATUS_RPS_REQUIRED;
              }
              else
              {
                /**
                 * If the sys info is still valid (i.e. no hand-overs were performed during the call), then camp
                 * (start idle) on the cell (default action), otherwise sys info acquisition is required. 
                 */
                if (grr_verify_sys_info_is_valid_for_l1_camp())
                {
                  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();

                  // Restore the cell channel description prior to connection establishment
                  scell_info_ptr->gsm.cell_channel_description =
                    scell_info_ptr->gsm.SI_cell_channel_description;
                }
                else
                {
                  // Indicate SI acquisition required to RR-CONTROL
                  ce_rel_status = RR_CE_REL_STATUS_ACQUIRE_SI;
                }
              }

              if ((ce_rel_status == RR_CE_REL_STATUS_RPS_REQUIRED) ||
                  (ce_rel_status == RR_CE_REL_STATUS_ACQUIRE_SI))
              {
                // Allocate memory for copy of BA-SACCH
                ba_sacch_ptr = GPRS_MEM_MALLOC(sizeof(inter_task_BCCH_allocation_T));

                rr_ded_get_copy_of_ba_sacch(ba_sacch_ptr);
              }

#ifdef FEATURE_GSM_DTM
              // If the DTM feature is enabled and the cell supports DTM, then don't delete GMM signalling PDUs
              if (rr_get_nv_dtm_enabled() && rr_cell_supports_dtm())
              {
                purge_gmm_signalling = FALSE;
              }
#endif // FEATURE_GSM_DTM
            }

            // Send RR_REL_IND/RR_ABORT_IND/RR_ABORT_CNF (no RR_SERVICE_IND)
            rr_conn_inform_mm_of_channel_release(
              FALSE   // send_rr_service_ind
            );

            /* notify connection establishment module that the RR connection is released */
            (void) rr_connection_establishment_control(EV_CHANNEL_DEACTIVATED, NULL);

            rr_ce_send_imsg_cs_release_ind(
              ce_rel_status,                    // status
              rr_scell_arfcn(),           // arfcn
              rr_scell_bsic(),            // bsic
              rr_scell_rxlev_average(),   // rxlev_average
              ba_sacch_ptr,                     // ba_sacch_ptr
              purge_gmm_signalling             // purge_gmm_signalling
            );

            // Free temporary memory if allocated
            if (ba_sacch_ptr != NULL)
            {
              GPRS_MEM_FREE(ba_sacch_ptr);
            }
          } /* successful channel release */
          else
          {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
            if (rr_x2g_cco_in_progress() == FALSE)
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
            {
              /* Send RR_REL_IND / RR_ABORT_IND / RR_ABORT_CNF, no RR_SERVICE_IND */
              /* afterwards */
              rr_conn_inform_mm_of_channel_release(FALSE);
            }
          }

          /* log the new channel configuration */
          (void)rr_log_channel_configuration();

          /* Move to RR_REL_NO_CONNECTION state */
          rr_conn_release_data_ptr->release_state = RR_REL_NO_CONNECTION;

          // Open TCH loop (if relevant)
          rr_tm_open_tch_loop();
        }
      } /* RR_REL_MAIN_SIGNALLING_DISCONNECTED */
      break;

      case RR_REL_PENDING_WAIT_FOR_PROCEDURE_TO_COMPLETE:
      {
        if (release_event == EV_PROCEDURE_COMPLETE)
        {
          MSG_GERAN_LOW_0_G("RR procedure completed, continuing with connection release");

#ifdef FEATURE_GSM_DTM
          // Send CS release request to the DTM module
          rr_dtm_send_imsg_cs_release_req(
            TRUE    // edtm_allowed
          );

          // Wait for RR_DTM_IMSG_CS_RELEASE_CNF
          rr_conn_release_data_ptr->release_state = RR_REL_PENDING_WAIT_FOR_DTM_RELEASE;

          // Note: RR-CONTROL is not informed of connection release until DTM module confirms

#else // FEATURE_GSM_DTM

          rr_rel_release_data_link(
            rr_conn_get_release_cause(),
            NORMAL_RELEASE
          );

          /* need to wait for the signalling link disconnection */
          rr_conn_release_data_ptr->release_state = RR_REL_PENDING_MAIN_SIGNALLING_DISCONNECTION;
          new_rr_event = RR_EV_CONNECTION_RELEASE_IN_PROGRESS;
#endif // FEATURE_GSM_DTM
        }
        else if (release_event == EV_RADIO_LINK_FAILURE)
        {
          MSG_GERAN_HIGH_0_G("RLF during GSM ded proc and rel pending, releasing locally");

          // Update the release cause to indicate that L2 has released the link
          rr_conn_set_release_cause(
            RELEASE_CAUSE_L2_LINK_RELEASED   // release_cause
          );
#ifdef FEATURE_GSM_DTM
          // Send CS release request to the DTM module
          rr_dtm_send_imsg_cs_release_req(
            TRUE    // edtm_allowed
          );

          // Wait for RR_DTM_IMSG_CS_RELEASE_CNF
          rr_conn_release_data_ptr->release_state = RR_REL_PENDING_WAIT_FOR_DTM_RELEASE;

          // Note: RR-CONTROL is not informed of connection release until DTM module confirms

#else // FEATURE_GSM_DTM

          /* Make sure that we do a local end release as RLF has occurred */
          rr_rel_release_data_link(
            rr_conn_get_release_cause(),
            LOCAL_END_RELEASE
          );

          /* no need to note that RLF has occurred, as this would trigger a */
          /* reestablishment attempt, which is not required here as a release */
          /* was in progress anyway */

          /* need to wait for the signalling link disconnection */
          rr_conn_release_data_ptr->release_state = RR_REL_PENDING_MAIN_SIGNALLING_DISCONNECTION;
          new_rr_event = RR_EV_CONNECTION_RELEASE_IN_PROGRESS;
#endif // FEATURE_GSM_DTM
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(release_event),
                               "RR_REL_PENDING_WAIT_FOR_PROCEDURE_TO_COMPLETE");
        }
      }
      break;

#ifdef FEATURE_GSM_DTM
      case RR_REL_PENDING_WAIT_FOR_DTM_RELEASE:
      {
        switch (release_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                rr_rr_msg_u *rr_rr_msg_ptr = &message_ptr->rr;

                switch (rr_rr_msg_ptr->header.rr_message_set)
                {
                  case RR_DTM_OMSG:
                  {
                    switch (rr_rr_msg_ptr->header.imh.message_id)
                    {
                      case RR_DTM_OMSG_CS_RELEASE_CNF:
                      {
                        rr_dtm_omsg_cs_release_cnf_t *msg_ptr = &rr_rr_msg_ptr->dtm.omsg.cs_release_cnf;

                        switch (msg_ptr->ce_rel_mode)
                        {
                          case RR_DTM_CE_REL_MODE_DTM:
                          {
                            rr_conn_release_data_ptr->release_mode = RELEASE_MODE_DTM;
                            break;
                          }
#ifdef FEATURE_GSM_EDTM
                          case RR_DTM_CE_REL_MODE_EDTM:
                          {
                            rr_conn_release_data_ptr->release_mode = RELEASE_MODE_EDTM;
                            break;
                          }
#endif // FEATURE_GSM_EDTM
                          default:
                          {
                            rr_conn_release_data_ptr->release_mode = RELEASE_MODE_GSM;
                          }
                        }

                        MSG_GERAN_HIGH_1_G(
                          "Continue with CS release (release_mode=%d)",
                          (int)rr_conn_release_data_ptr->release_mode
                        );

                        if (rr_conn_release_data_ptr->l2_release_mode != RR_CE_REL_L2_RELEASE_NOT_REQUIRED)
                        {
                          release_mode_T release_mode = NORMAL_RELEASE;

                          if (rr_conn_release_data_ptr->l2_release_mode == RR_CE_REL_L2_RELEASE_LOCAL)
                          {
                            release_mode = LOCAL_END_RELEASE;
                          }

                          rr_rel_release_data_link(
                            rr_conn_get_release_cause(),
                            release_mode
                          );

                          // Wait for L2 to confirm release
                          rr_conn_release_data_ptr->release_state = RR_REL_PENDING_MAIN_SIGNALLING_DISCONNECTION;
                        }
                        else
                        {
                          // Check if L1 can handle the release message
                          if (rr_conn_release_data_ptr->l1_in_null)
                          {
                            MSG_GERAN_HIGH_0_G("L1 in NULL - skip release");

                            // No need to release L1 (MPH_CHANNEL_RELEASE_REQ will not be sent)
                            new_release_event = EV_CHANNEL_DEACTIVATED;
                          }
                          else
                          {
                            // Release L1 (MPH_CHANNEL_RELEASE_REQ will be sent)
                            rr_rel_deactivate_channels();
                          }

                          // Wait for MPH_CHANNEL_RELEASE_CNF (if required)
                          rr_conn_release_data_ptr->release_state = RR_REL_MAIN_SIGNALLING_DISCONNECTED;

                          // Inform RR-CONTROL that CS connection is being released
                          new_rr_event = RR_EV_CONNECTION_RELEASE_IN_PROGRESS;
                        }

                        // Move rr_control main state to RR_CONNECTION_RELEASE
                        new_rr_event = RR_EV_CONNECTION_RELEASE_IN_PROGRESS;
                        break;
                      } // case RR_DTM_OMSG_CS_RELEASE_CNF

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          message_ptr,
                          rr_release_state_name(rr_conn_release_data_ptr->release_state)
                        );
                      }
                    }
                    break;
                  } // case RR_DTM_OMSG

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                          message_ptr,
                          rr_release_state_name(rr_conn_release_data_ptr->release_state)
                          );
                    break;
                  }
                            
                }
                break;
              } // case MS_RR_RR

              case MS_MM_RR:
              {
                switch (message_ptr->mm.header.message_id)
                {
                  case RR_ABORT_REQ:
                  {
                    MSG_GERAN_HIGH_0_G("RR_ABORT_REQ - already releasing (DTM)");

                    // Ensure a RR_ABORT_CNF is returned
                    rr_conn_set_release_cause(RELEASE_CAUSE_NAS_REQUEST);
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      message_ptr,
                      rr_release_state_name(rr_conn_release_data_ptr->release_state)
                    );
                  }
                }
                break;
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  message_ptr,
                  rr_release_state_name(rr_conn_release_data_ptr->release_state)
                );
              }
            }
            break;
          } // case EV_INPUT_MESSAGE

          case EV_RADIO_LINK_FAILURE:
          {
            MSG_GERAN_HIGH_0_G("RLF - already releasing (DTM)");
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(
              rr_event_name(release_event),
              rr_release_state_name(rr_conn_release_data_ptr->release_state)
            );
          }
        }
        break;
      }
#endif /* FEATURE_GSM_EDTM */
      default:
      {
        break;
      }
    } /* switch (rr_conn_release_data_ptr->release_state) */

#ifdef DEBUG_RR_TRACE_MSG
    if (((release_event != new_release_event) && (new_release_event != EV_NO_EVENT)) ||
        (rr_conn_release_data_ptr->old_release_state != rr_conn_release_data_ptr->release_state))
    {
       rr_store_trace_msg_buf(
         RR_CONN_RELEASE_SM,
         new_release_event,
         (byte) rr_conn_release_data_ptr->release_state,
         message_ptr
       );
    }
#endif

    if (new_release_event != EV_NO_EVENT)
    {
      release_event = new_release_event;
      new_release_event = EV_NO_EVENT;
    }
    else
    {
      release_event = EV_NO_EVENT;
    }

    if (rr_conn_release_data_ptr->old_release_state != rr_conn_release_data_ptr->release_state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_connection_release",
        rr_conn_release_data_ptr->old_release_state,
        rr_conn_release_data_ptr->release_state,
        rr_release_state_name
      );

      /* If the old state is a transitory state, cancel the state transition guard timer */
      if (rr_connection_release_state_is_transitory(rr_conn_release_data_ptr->old_release_state))
      {
        MSG_GERAN_MED_1_G(
          "Cancelling state transition guard timer (left state %d)",
          rr_conn_release_data_ptr->old_release_state
        );

        (void)rr_timer_cancel(RR_CONNECTION_RELEASE_STATE_TRANSITION_GUARD_TIMER);
      }

      /* If the new state is a transitory state, start the state transition guard timer */
      if (rr_connection_release_state_is_transitory(rr_conn_release_data_ptr->release_state))
      {
        MSG_GERAN_MED_2_G(
          "Starting state transition guard timer (entered state %d, duration %d ms)",
          rr_conn_release_data_ptr->release_state,
          RR_CONNECTION_RELEASE_STATE_TRANSITION_TIMEOUT
        );

        (void)rr_timer_start(
          RR_CONNECTION_RELEASE_STATE_TRANSITION_GUARD_TIMER,
          RR_CONNECTION_RELEASE_STATE_TRANSITION_TIMEOUT
        );
      }

      rr_conn_release_data_ptr->old_release_state = rr_conn_release_data_ptr->release_state;
    }
  } /* while (release_event != EV_NO_EVENT) */

  return new_rr_event;
} /* rr_connection_release */

/*!
 * \brief Module initialisation function called at task start.
 */
void rr_ce_rel_task_start_init( void )
{
  // Zero the control/data structure(s)
  memset(&rr_conn_release_data, 0, sizeof(rr_conn_release_data));

  // Setup any specific values
  {
    release_info_t *data_ptr;

    data_ptr = &rr_conn_release_data;
    data_ptr->reported_cause = RELEASE_CAUSE_NONE;
    data_ptr->release_state = RR_REL_NO_CONNECTION;
    data_ptr->old_release_state = RR_REL_NO_CONNECTION;
  }

} /* end rr_ce_rel_task_start_init() */
#endif /* FEATURE_GERAN_CS_SUPPORT */

/* EOF */


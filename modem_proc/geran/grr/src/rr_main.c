/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Main Module

GENERAL DESCRIPTION
   This module contains the task loop for the RR task, auxiliary functions
   and the top-level message handler for the RR

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 by QUALCOMM Technologies, Incorporated. All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_main.c_v   1.18   15 Jul 2002 15:28:28   jachan  $
$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_main.c#17 $ $DateTime: 2020/09/01 11:08:04 $ $Author: pwbldsvc $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
15/07/14    pjr     Changed priority to l1_priority in function rr_main_lte_msgr_init
11/04/13    sjw     Do not use the pending message queue for ignored messages
14/02/13    sjw     Defer RR_PS_ABORT_REQ if first RACH response is still pending
20/03/12    pg      Add missing FEATURE_Q_NO_SELF_QPTR for q_delete and q_insert
02/10/12    sjw     Integrate DSDS controller with new message processing control
01/26/12    sjw     Add Demand Paging exclusion for rr_send_routine()
15/11/10    sjw     Added support for FEATURE_DUAL_SIM
04/07/09    sjw     Rework resource locking to use mutex/critical sections as appropriate
04/10/06    hs      Register rr_send_routine() to gsmsg to fix alignment mismatch on RVCT
06/30/03    sbs     Added support for Location Services related APDU handling
05/15/03    He      Ignore L2 fill octets
02/19/03    Rao     Fix for Releasing SAPIO link when SAPI3 Establishment fails.
10/21/02    RG      Added GSM to WCDMA Reselection support (SI2 Qtr decode ).
08/26/02    rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
07/30/02    Rao     Added InterRAT ( WCDMA To GSM )  Handover support
07/29/02    rmc     Store reselection trigger when serving cell becomes barred
07/11/02    JAC     Added early camp support when collecting full SI for reselection
06/20/02    JAC     Added debug messages for bad protocol discriminator error
06/14/02    rmc     Backing out changes for l3_message_length field in RR_DATA_REQ and
                    RR_EST_REQ, fixing lint warnings
05/30/02    rmc     Changes to support MULTIBAND_SUPPORT
05/22/02    rmc     Changes to MM-RR icd
05/01/02    rmc     Register RR task for NV
04/16/02    AT      Changed RR Cmd queue length to 20.
04/15/02    rmc     Fix for CR21733
04/11/02    JAC     added uplink/downlink bit for logging signalling msgs
04/10/02    rmc     Watchdog support for RR task
03/18/02    Rao     Added Dual Mode (Mode control) changes.
03/13/02    ATM     Added support for new GS_SEND_MESSAGE
02/20/02    ATM     Merge GS_GSM and GS functions together
02/14/02    rmc     QXDM Cleanup + Input message xdm logging
11/21/01    ATM     Replaced call to CFA with call to Generic Services (GS) --
                    which will indirectly call CFA.
11/09/01    RMC     Cleanup to match MM version and remove unit test specific code
                    Fixed problem with cfa_log_packet and removed duplicates
12/11/00    TS      Initial Revision.

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"

#ifdef FEATURE_GSM_TDS
#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"
#endif /*FEATURE_GSM_TDS*/

#include "geran_tasks.h"
#include "amssassert.h"
#include "rr_main.h"
#include "rr_control.h"
#include "rr_l2.h"
#include "ms.h"
#include "rr_l2_send.h"
#include "rr_mm_send.h"
#include "rr_test.h"
#include "rr_sys_info.h"
#include "gs.h"
#include "rr_task.h"
#include "rr_init.h"
#include "geran_msgs.h"
#include "rr_log.h"
#include "gs.h"           /* for gs_read_messages */
#include <stddef.h>

#include "rr_general.h"
#include "rr_mode.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_cell_acq_if.h"
#include "rr_cell_acq.h"
#include "rr_resel.h"
#ifdef FEATURE_WCDMA
#include "rr_resel_g2w.h"
#endif /* FEATURE_WCDMA */

#include "rr_procedure.h"
#include "rr_sapi3.h"
#include "rr_init.h"

#include "aconfig.h"
#include "task.h"
#include "rcinit.h"

#include "rr_gprs_defs.h"
#include "rr_gprs_debug.h"
#include "mac_grr.h"

#ifdef FEATURE_CGPS_UMTS_CP_GSM
#include "rr_lsm_if.h"
#endif /* FEATURE_CGPS_UMTS_CP_GSM */
#include "grr_mutex.h"

#include "msgr.h"
#include "msgr_rex.h"
#include "geran_grr_msg.h"

#ifdef FEATURE_FEMTO_GSM_NL
#include "rr_femto.h"
#include "rr_femto_if.h"
#endif /* FEATURE_FEMTO_GSM_NL */

#include "geran_multi_sim.h"
#ifdef FEATURE_DUAL_SIM
#include "rr_multi_sim.h"
#include "rr_conn_establish.h"
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#include "gprs_mem.h"
#include "rr_ded_config.h"
#include "rr_sim.h"
#include "rr_l1_idle_mode.h"
#include "geran_nv.h"
#include "rr_ps_domain.h"
#include "rr_ps_access.h"
#include "rr_nv.h"
#include "rr_decode_ie.h"
#include "rr_shared_cell_selection.h"
#include "rr_gprs_defs.h"
#include "rr_sys_info_cache.h"
#include "stringl/stringl.h"
#ifdef FEATURE_SGLTE
#include "rr_sglte.h"
#endif /*FEATURE_SGLTE*/
#include "rr_x2g.h"
#include "rr_msc.h"
#include "rr_l1_send.h"
#ifdef FEATURE_CGPS_UMTS_CP_GSM
#include "rr_cgps.h"
#endif /*FEATURE_SGLTE*/
#include "rr_select_bcch.h"
#include "rr_dtm.h"
#include "rr_acq_db.h"
#include "rr_pscan.h"
#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK  
#include "gsmdiag.h"
#endif /*FEATURE_GERAN_R13_TEST_FRAMEWORK*/

/* mmcp_sd_task_running **************************************************/
/* RCInit CM start up synchronisation - see mmcp\mmode\sd\src\sdtask.h  */
#include "rcevt_rex.h"

#include "geran_grr_dyn_api.h"
#include "rr_static_main.h"

/* Define a type for RR task pending queue + control flag */
typedef struct
{
  q_type  rr_cmd_pend_q;
  boolean nas_message_processing_suspended;
} rr_pending_queue_T;

/* RR task pending queue + control flag */
rr_pending_queue_T  rr_pending_queue;

static rex_timer_type  rr_sleep_timer;

/* Critical section to protect shared variables in GRR */
rex_crit_sect_type     grr_general_crit_sec;
rex_crit_sect_type     grr_sysinfo_crit_sec;


#ifdef FEATURE_DUAL_SIM
#endif /* FEATURE_DUAL_SIM */

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/*===========================================================================
                         FUNCTION DECLARATIONS
===========================================================================*/
static void rr_process_message(
  rr_cmd_bdy_type   * new_message,
  rr_message_type_e   message_type
);


/*===========================================================================
                         FUNCTION DEFINITIONS
===========================================================================*/


/* GRR vtable to map static API fptrs with dynamic APIs */
geran_static_grr_ext_api_s rr_vtbl =
{
  rr_message_dispatcher,
  rr_process_reset_signal,
  geran_grr_get_arfcn_band_dyn,
  geran_grr_convert_to_arfcn_type_dyn,
  mb_initialise_dyn,
  arfcns_equal_dyn,
  geran_start_capture_scan_data_dyn,
  geran_stop_capture_scan_data_dyn,
  geran_get_nv_recovery_restart_enabled_dyn,
  geran_initiate_recovery_restart_dyn,
  geran_grr_get_current_timing_advance_info_per_subs_dyn,
  geran_grr_get_current_timing_advance_info_dyn,
  geran_grr_dtx_override_on_per_subs_dyn,
  geran_grr_dtx_override_on_dyn,
  rr_dtx_override_on_dyn,
  geran_grr_dtx_override_off_per_subs_dyn,
  geran_grr_dtx_override_off_dyn,
  rr_dtx_override_off_dyn,
  geran_grr_get_current_bs_info_per_subs_dyn,
  geran_grr_get_current_bs_info_dyn,
  geran_grr_set_codec_ho_sample_rate_per_subs_dyn,
  geran_grr_set_codec_ho_sample_rate_dyn,
  geran_grr_get_l3_msg_FPOS_dyn,
  geran_grr_register_cgps_event_cb_dyn,
  rr_compute_BSIC_from_SCH_block_dyn,
  rr_read_public_store_dyn,
  rr_read_public_store_freq_list_dyn,

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  rr_nv_send_nv_refresh_ind_dyn,
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

  rr_nv_eng_mode_debug_is_enabled_dyn,
  rr_packet_assignment_mobile_alloc_dyn,
  rr_gapi_is_supported_band_dyn,
  rr_is_supported_band_dyn,
  rr_get_nmr_bcch_dyn,
  rr_api_get_nmr_bcch_dyn,
  rr_get_cell_id_dyn,
  rr_get_timing_advance_dyn,
  rr_get_band_dyn,
  rr_get_current_band_dyn,
  rr_get_current_rf_power_class_dyn,
  rr_gapi_get_band_rf_power_class_dyn,
  rr_get_band_rf_power_class_dyn,
  rr_get_supported_band_count_dyn,
  rr_get_band_indicator_for_nas_dyn,
  rr_get_maximum_power_output_for_frequency_dyn,
  rr_dtm_get_max_LAPDm_octets_per_subs_dyn,
  rr_dtm_get_max_LAPDm_octets_dyn,

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  rr_register_cell_change_ind_cb_dyn,
  rr_deregister_cell_change_ind_cb_dyn,
  
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ds_register_cell_change_ind_cb_dyn,
  rr_ds_deregister_cell_change_ind_cb_dyn,
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */

  rr_ds_get_supported_band_count_dyn,
  rr_ds_get_band_indicator_for_nas_dyn,
  rr_ds_get_nmr_bcch_dyn,
  rr_ds_api_get_nmr_bcch_dyn,
  rr_ds_get_cell_id_dyn,
  rr_ds_get_timing_advance_dyn,

#ifdef FEATURE_WCDMA
  rr_send_rrc_message_to_rr_dyn,
  rrc_get_rr_curr_camped_plmn_sub_id_dyn,
  rrc_get_rr_curr_camped_plmn_dyn,
  rr_rrc_acq_db_save_dyn,
  rr_rrc_acq_db_save_sub_id_dyn,
  rr_rrc_get_arfcn_band_dyn,
  rr_rrc_get_arfcn_band_sub_id_dyn,
  rr_rrc_set_band_pref_dyn,
  rr_rrc_set_band_pref_sub_id_dyn,
  rr_rrc_set_bst_band_pref_dyn,
  rr_rrc_set_bst_band_pref_sub_id_dyn,
#endif /* FEATURE_WCDMA */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  rr_get_inter_rat_bplmn_prtl_result_sub_id_dyn,
  rr_get_inter_rat_bplmn_prtl_result_dyn,
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  rr_init_hz_info_dyn,
  rr_hz_register_state_change_cb_dyn,
  rr_hz_deregister_state_change_cb_dyn,

#if defined (FEATURE_TDSCDMA) || defined (FEATURE_GSM_TDS)
  tdsrr_send_rrc_message_to_rr_dyn,
  tdsrr_get_arfcn_band_sub_id_dyn,
  tdsrr_get_arfcn_band_dyn,
  tdsrr_set_band_pref_sub_id_dyn,
  tdsrr_set_band_pref_dyn,
  tdsrr_set_bst_band_pref_sub_id_dyn,
  tdsrr_set_bst_band_pref_dyn,
  tdsrr_get_rr_curr_camped_plmn_sub_id_dyn,
  tdsrr_get_rr_curr_camped_plmn_dyn,
  tdsrr_acq_db_save_sub_id_dyn,
  tdsrr_acq_db_save_dyn,
#endif /* FEATURE_TDSCDMA || FEATURE_GSM_TDS */

  geran_send_cm_test_mode_on_dyn,
  geran_send_cm_test_mode_off_dyn,
#if !defined(PERLUTF) && defined(FEATURE_RR_THERMAL_MITIGATION)
  change_event_cb_fn_dyn,
  initialise_event_cb_fn_dyn,
#endif
  geran_map_gsm_band_to_sys_band_class_dyn,
  gprs_odm_info_get_rach_counter_dyn,
  gprs_odm_info_get_num_of_neighbour_cells_dyn,
  gprs_odm_info_get_paging_multiframe_dyn,
  gprs_odm_info_get_rx_lev_dBm_dyn,
  gprs_odm_info_get_rx_lev_status_dyn,
  gprs_odm_info_get_cell_type_dyn
#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
  ,gprs_odm_get_plmn_lac_list_dyn
  ,gprs_odm_set_plmn_lac_list_dyn
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */
#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
  ,geran_get_current_serving_cell_info_dyn
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */
};

/**
  @brief Peform all initialisation required at task-start
  while loaded by SOmgr. SOmgr will call this function 
  while Dynamic loading, internally from SO_CREATE

  The following are initialised here -
  Critical Sections;
  RR timers;
  Configuration of GERAN EFS-NV;
  RR module task-start initialisation
*/
void geran_grr_init()
{
  byte i=0;

  /* Initialise general GRR critical section */
  rex_init_crit_sect(&grr_general_crit_sec);

  /* Initialise GRR sysinfo critical section */
  rex_init_crit_sect(&grr_sysinfo_crit_sec);

  /* initialise the pending queue for each data space */
  (void)q_init(&(rr_pending_queue.rr_cmd_pend_q));

  /* initialise the critical section used for the public store */
  rr_public_store_lock_init();

  // Initialise RR timer module (ready for use during general RR module initialisation)
  /* create the task sleep timer */
  rex_def_timer(&rr_sleep_timer, geran_tcb_read(GERAN_TASK_ID_GRR), RR_SLEEP_SIG);

#ifdef FEATURE_RR_SI_CACHE
 // Initialise system information cache
  rr_csi_init_cache();
#endif

  // Initialise multi-sim parameters for GERAN
  geran_multi_sim_init();

  // Initialise all other RR modules
  rr_init_task_start_init();

#ifdef FEATURE_SEGMENT_LOADING
  rr_initialise_seg_load();
#endif /*FEATURE_SEGMENT_LOADING*/

#ifdef FEATURE_DUAL_SIM
#endif
}

void geran_grr_resume()
{

}
 
void geran_grr_suspend()
{


}
 
/**
  @brief Unload shared object (dynamic library) through SOmgr
         and clear rr_static_context

  @param

  @return 
*/
void geran_grr_deinit()
{

  MSG_GERAN_HIGH_0("GRR De-initializing");

 /* initialise the pending queue for each data space */
  {  
    rr_acq_db_save();
    rr_init_modem_offline();
    rr_sys_info_cache_flush_data();
  }
}

/**
  @brief Can be called by any module, and will cause RR to restart RR, L1, MAC and RLC
*/
void geran_initiate_recovery_restart_dyn(
  const geran_client_enum_t client_id
   
)
{
#if !defined(PERLUTF) && !defined(TEST_FRAMEWORK)
  /* Verify that the FTM copy of rr_l1_messages_u is at least as big as expected
     (will generate a compiler error if it isn't) */
  uint8 dummy [sizeof(mph_geran_to_ftm_padding_T) >= sizeof(rr_l1_messages_u) ? 1 : -1];
  dummy[0] = 0; /* Fix for Klocwork  warning -- 'dummy' array elements are used uninitialized in this function.*/
  dummy[0] = dummy[0]; /* sidestep compiler warning about unused variable */
#endif /* PERLUTF */

  MSG_GERAN_HIGH_1_G("GSM client (%d) triggered recovery restart", client_id);
#ifndef PERLUTF
#ifdef FEATURE_DUAL_SIM
  // Record the GAS-ID which triggered the restart - needed for when the RR task runs
#endif

  /* Set RR_RESET_SIG to be picked-up next time RR task runs */
  (void) rex_set_sigs(geran_tcb_read(GERAN_TASK_ID_GRR), RR_RESET_SIG);
#else
  #error code not present
#endif /* !PERLUTF */

  return;
} /* geran_initiate_recovery_restart_dyn() */

void rr_process_reset_signal(void)
{
  rr_debug_log_panic_reset();
  rr_control(EV_PANIC_RESET, NULL);
} /* end rr_process_reset_signal */

/**
  @brief Returns a pointer to the pending queue
*/
static rr_pending_queue_T* rr_get_pending_queue_ptr( void )
{
  return (&(rr_pending_queue));
}

#ifdef FEATURE_GERAN_CS_SUPPORT
/**
  @brief Checks for RR_EST_REQ in Pending Command Queue and removes it if found. 
  Returns TRUE if found else FALSE    
*/
static boolean rr_check_and_rem_pend_est_req( void )
{
  boolean est_req_found = FALSE;
  rr_pending_queue_T *pend_queue_ptr;
  rr_cmd_type        *cmd_ptr;

  pend_queue_ptr = rr_get_pending_queue_ptr();

  if (pend_queue_ptr != NULL)
  {
     cmd_ptr = (rr_cmd_type *)q_check(&(pend_queue_ptr->rr_cmd_pend_q));
  }
  else
  {
    cmd_ptr = NULL;
  }

  while ( (cmd_ptr != NULL) && (est_req_found == FALSE) )
  {
    switch (cmd_ptr->msg.message_header.message_set)
    {
      case MS_MM_RR:
      {
        if ( cmd_ptr->msg.message_header.message_id == RR_EST_REQ )
        {
          est_req_found = TRUE;
        }
        break;
      }
      default:
      break;
    }

    if (est_req_found)
    {
    
      MSG_GERAN_HIGH_1_G("RR Removing Pending msg_id %d", cmd_ptr->msg.message_header.message_id);
      q_delete(
               &(pend_queue_ptr->rr_cmd_pend_q),
               &cmd_ptr->link
              );
      /* Free memory allocated for this message */
      GPRS_MEM_FREE(cmd_ptr);
    }
    else
    {
      cmd_ptr = (rr_cmd_type *) q_next(&(pend_queue_ptr->rr_cmd_pend_q), &cmd_ptr->link);
    }
  }

  return(est_req_found);
}
#endif /* FEATURE_GERAN_CS_SUPPORT */

/**
  @brief Determines if a message received should be processed normally, or held in the queue
*/
static rr_process_msg_action_e should_message_be_processed(
  rr_cmd_bdy_type *msg_ptr
   
)
{
  // Assume the message is valid and can be processed
  boolean valid_message = TRUE;
  boolean process_message = TRUE;

  switch (msg_ptr->message_header.message_set)
  {
    case MS_MM_RR:
    {
      process_message = rr_can_process_mm_message();

      if (process_message)
      {
        switch (msg_ptr->message_header.message_id)
        {
#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)
          case RR_PS_ABORT_REQ:
          {
            process_message = rr_ce_can_process_ps_abort();
            break;
          }

          case RR_GMM_GPRS_SERVICE_REQ:
          {
            process_message = rr_ps_can_process_gprs_service_req();
            break;
          }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#if defined(FEATURE_LTE) && defined(FEATURE_GERAN_CS_SUPPORT)
          case RR_CSFB_BLIND_REDIR_TO_LTE_REQ:
          {
            process_message = rr_can_process_csfb_blind_redir_to_lte_req();
            break;
          } /* RR_CSFB_BLIND_REDIR_TO_LTE_REQ */
#endif /* FEATURE_LTE && FEATURE_GERAN_CS_SUPPORT */

          case RR_CHANGE_MODE_IND:
          {
            process_message = rr_can_process_change_mode_ind();
            break;
          } /* RR_CHANGE_MODE_IND */


#ifdef FEATURE_SGLTE
          case RR_STOP_GSM_MODE_REQ:
          {
            switch (msg_ptr->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason)
            {
              case RR_SGLTE_RAT_CHANGE:
              case RR_SGLTE_USER_RAT_CHANGE:
              {
                process_message = rr_can_process_stop_gsm_mode_req_sglte_rat_change();
                break;
              } /* RR_SGLTE_RAT_CHANGE */

              default:
              {
                process_message = rr_can_process_mm_message();
              }
            }
            break;
          } /* RR_STOP_GSM_MODE_REQ */
#endif /* FEATURE_SGLTE */
          default:
          {
            process_message = rr_can_process_mm_message();
          }
        }
      }
      else
      {
        /* apply any per-message exclusions for NAS primitives here */
        if (RR_GMM_GPRS_MS_RA_CAP_IE_IND == msg_ptr->message_header.message_id)
        {
          /* always process this NAS message as soon as it arrives */
          /* because it is part of a capability handshake with LTE */
          process_message = TRUE;
        }
#ifdef FEATURE_GERAN_CS_SUPPORT
        else if ( RR_ABORT_REQ == msg_ptr->message_header.message_id)
        {
          if ( rr_check_and_rem_pend_est_req() == FALSE )
          {
            process_message = rr_can_process_rr_abort_req();
          }
          else
          {
            rr_send_abort_cnf(MM_AS_SUCCESS);
            valid_message = FALSE;
          }
        }
#endif /* FEATURE_GERAN_CS_SUPPORT */
      }
      break;
    }

    case MS_RR_RR:
    case MS_RR_RR_EXT:
    {
      switch (msg_ptr->rr.header.rr_message_set)
      {
        case RR_IMSG:
        {
          switch (msg_ptr->rr.header.imh.message_id)
          {
            case RR_IMSG_SAVED_MSG_IND:
            {
              process_message = rr_can_process_saved_msg_ind();
              break;
            }

            case RR_IMSG_RR_EVENT_IND:
            {
              process_message = rr_can_process_rr_event_ind();
              break;
            }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
            case RR_IMSG_MSC_CHANGE_IND:
            {
              process_message = rr_can_process_msc_change_ind();
              break;
            }
#endif
            case RR_IMSG_NO_PS_ACCESS_REQ:
            {
              process_message = rr_can_process_no_ps_access_req();
              break;
            }

            case RR_IMSG_PS_ACCESS_REQ:
            {
              process_message = rr_can_process_ps_access_req();
              break;
            }

            case RR_IMSG_DMM_RESUME_IND:
            {
              if (!rr_is_inactive())
              {
                process_message = FALSE;
              }
              break;
            }

            default: ;
          }
          break;
        }

        case RR_GRR_IMSG:
        {
          switch (msg_ptr->rr.header.imh.message_id)
          {
            case RR_GRR_IMSG_GPRS_IA_IND:
            {
              process_message = grr_can_process_rr_grr_gprs_ia_ind();
              break;
            }

            case RR_GRR_IMSG_RR_EVENT_IND:
            {
              process_message = grr_can_process_rr_event_ind();
              break;
            }

            default: ;
          }
          break;
        }

        case RR_RESEL_IMSG:
        {
          switch (msg_ptr->rr.resel.imsg.header.imh.message_id)
          {
            case RR_RESEL_IMSG_ABORT_REQ:
            {
              process_message = rr_can_process_resel_abort_req();
              break;
            } /* RR_RESEL_IMSG_ABORT_REQ */
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 
            case RR_RESEL_IMSG_REDUCED_MOBILITY_START_IND:
            case RR_RESEL_IMSG_REDUCED_MOBILITY_STOP_IND:
            {
              process_message = rr_can_process_resel_reduced_mobility_msg();
              break;
            } /*RR_RESEL_IMSG_REDUCED_MOBILITY_START_IND || RR_RESEL_IMSG_REDUCED_MOBILITY_STOP_IND*/
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

            default: ;
          }
          break;
        } /* RR_RESEL_IMSG */

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION 

        case RR_RESEL_RMC_IMSG:
        {          
          switch (msg_ptr->rr.resel_rmc.imsg.header.imh.message_id)
          {
            case RR_RESEL_RMC_IMSG_BSI_ACQ_COMPLETE_IND:
            {
              process_message = rr_can_process_resel_reduced_mobility_msg();
              break;
            } /*RR_RESEL_IMSG_REDUCED_MOBILITY_START_IND*/

            default: ;
          }
          break;
        }
#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION*/

        case RR_L1_IDLE_IMSG:
        {
          switch (msg_ptr->rr.resel.imsg.header.imh.message_id)
          {
            case RR_L1_IDLE_IMSG_START_IDLE_REQ:
            {
              process_message = rr_l1_idle_can_process_start_idle_req();
              break;
            } /* RR_L1_IDLE_IMSG_START_IDLE_REQ */

            default: ;
          }
          break;
        } /* RR_L1_IDLE_IMSG */

#ifdef FEATURE_GSM_DTM
        case RR_DTM_IMSG:
        {
          switch (msg_ptr->rr.resel.imsg.header.imh.message_id)
          {
            case RR_DTM_IMSG_CS_RELEASE_REQ:
            {
              process_message = rr_dtm_can_process_cs_release_req();
              break;
            }

            default: ;
          }
          break;
        } // RR_DTM_IMSG
#endif // FEATURE_GSM_DTM

        default: ;
      }
      break;
    }

    case MS_RR_L1:
    {
      switch (msg_ptr->message_header.message_id)
      {
        case MPH_SERVING_MEAS_IND:
        case MPH_SERVING_IDLE_MEAS_IND:
        case MPH_SERVING_AUX_MEAS_IND:
        case MPH_NC_MEASUREMENT_IND:
        case MPH_BLOCK_QUALITY_IND:
        {
          process_message = rr_can_process_serving_meas_ind();
          break;
        }

        case MPH_SURROUND_MEAS_IND:
        case MPH_SURROUND_UPDATE_FAILURE_IND:
#ifdef FEATURE_WCDMA
        case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
        case MPH_SURROUND_WCDMA_XFER_MEAS_IND:
#endif /* FEATURE_WCDMA */
#if defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
        case MPH_SURROUND_IRAT_MEAS_IND:
#endif /* FEATURE_LTE || FEATURE_GSM_TDS */
        {
          process_message = rr_can_process_surround_meas_ind();
          break;
        }
#ifdef FEATURE_SGLTE
        case MPH_UE_MODE_CHANGE_CNF:
         if (rr_is_pending_mode_change() )
         {
             rr_set_pending_mode_change(FALSE);
         }

        break;
#endif /* FEATURE_SGLTE */
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
        case MPH_PROC_IND:
        {
          process_message = rr_can_process_proc_ind_with_suspend();
          break;
        }
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */
        default: ;
      }
      break;
    }

    case MS_TIMER:
    {
      if (msg_ptr->message_header.message_id == MID_TIMER_EXPIRY)
      {
        /* process the message expiration to mark the state of the timer as expired */
        timer_expiry_T *timer_expiry_ptr = (timer_expiry_T *) msg_ptr;

        if (rr_timer_validate_expiry_message((rr_timer_id_T)(timer_expiry_ptr->timer_id)))
        {
          // Timer is valid. Now check whether RR is ready to process its expiry
          process_message = rr_can_process_timer_expiry((rr_timer_id_T)(timer_expiry_ptr->timer_id));
        }
        else
        {
          MSG_GERAN_HIGH_0_G("Timer invalid - expiry message discarded");

          // Flag this message as invalid
          valid_message = FALSE;
        }
      }
      break;
    }

#ifdef FEATURE_SGLTE
    case MS_MSGR_RR:
    {
      switch (msg_ptr->rr_msgr_msg.message.hdr.id)
      {
#ifdef FEATURE_LTE
        case GERAN_GRR_LTE_RESEL_REQ:
        case GERAN_GRR_LTE_REDIR_REQ:
        case GERAN_GRR_LTE_CCO_REQ:
        {
          if (rr_is_sglte_mode_active())
          {
            process_message = rr_can_process_sglte_x2g_req();
          }
          else
          {
            process_message = TRUE;
          }
          break;
        } /* GERAN_GRR_LTE_RESEL_REQ ... */

        case GERAN_GRR_LTE_ABORT_RESEL_REQ:
        case GERAN_GRR_LTE_ABORT_REDIR_REQ:
        case GERAN_GRR_LTE_ABORT_CCO_REQ:
        {
          if (rr_is_sglte_mode_active())
          {
             MSG_GERAN_HIGH_0_G("Abort rcvd from LTE,check  pending X2G message");
             if (rr_sglte_reject_pending_x2g_req(TRUE))
             {
               MSG_GERAN_HIGH_0_G("Abort rcvd, pending X2G message removed");   
               valid_message = FALSE;
             }
          }
          break;
        }

        case GERAN_GRR_LTE_HO_REQ:
        {
          if (rr_is_sglte_mode_active())
          {
            process_message = rr_can_process_sglte_l2g_ho_req();

            if(!process_message)
            {
              MSG_GERAN_HIGH_0_G("GERAN is GOING INACTIVE hold of any GERAN_GRR_LTE_HO_REQ HO rqst in pending queu ");
            }
          }
          else
          {
            process_message = TRUE;
            MSG_GERAN_HIGH_0_G("SGLTE mode not active process HO as legacy ");
          }
          break;
        } /* GERAN_GRR_LTE_HO_REQ */
        case GERAN_GRR_LTE_ABORT_HO_REQ:
        {
          if (rr_sglte_internal_stop_gsm_being_processed())
          {
            process_message = FALSE;
            MSG_GERAN_HIGH_0_G("SGLTE SRVCC mode intrnl shutdown being processed queue the L2G Abort");
          }
          else if (rr_is_sglte_mode_active())
          {
            process_message = rr_can_process_sglte_l2g_ho_req();

            if(!process_message)
            {
              MSG_GERAN_HIGH_0_G("GERAN is GOING INACTIVE hold of any GERAN_GRR_LTE_ABORT_HO_REQ HO rqst in pending queue ");
            }
          }
          else
          {
            process_message = TRUE;
          }
          break;
        }
#endif /* FEATURE_LTE */

        default:
        {
          process_message = TRUE;
        }
      }
      break;
    } /* MS_MSGR_RR */

#ifdef FEATURE_GSM_TDS
    case MS_TDSRRC_RR:
    {
      switch (msg_ptr->message_header.message_id)
      {
        case TDSRR_INTERRAT_RESELECTION_REQ:
        case TDSRR_INTERRAT_REDIRECT_REQ:
        case TDSRR_INTERRAT_CC_ORDER_REQ:
        {
          if (rr_is_sglte_mode_active())
          {
            process_message = rr_can_process_sglte_x2g_req();
          }
          else
          {
            process_message = TRUE;
          }
          break;
        } /* TDSRR_INTERRAT_RESELECTION_REQ ... */

        case TDSRR_INTERRAT_RESELECTION_ABORT_REQ:
        case TDSRR_INTERRAT_REDIRECT_ABORT_REQ:
        case TDSRR_INTERRAT_CC_ORDER_ABORT_REQ:
        {
          if (rr_is_sglte_mode_active())
          {
             MSG_GERAN_HIGH_0_G("Abort rcvd from TD,check  pending X2G message");          
             if (rr_sglte_reject_pending_x2g_req(TRUE))
             {
                MSG_GERAN_HIGH_0_G("pending X2G message now removed");   
                valid_message = FALSE;
             }
          }
          break;
        }

        default:
        {
          process_message = TRUE;
        }
      }
      break;
    } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_SGLTE */

    /* All other messages should be processed immediately, regardless */
    default: ;
  }

  if (!process_message)
  {
    MSG_SPRINTF_2(
      MSG_SSID_DFLT,
      MSG_LEGACY_HIGH,
      "gs1:Processing of %s blocked in %s",
        rr_gprs_message_name(
        msg_ptr,
        msg_ptr->message_header.message_set,
        msg_ptr->message_header.message_id,
        NULL
      ),
      rr_main_current_state_name()
    );
  }

  if (valid_message)
  {
    if (process_message)
    {
      return RR_PROCESS_MSG_NOW;
    }
    else
    {
      return RR_PROCESS_MSG_LATER;
    }
  }

  return RR_PROCESS_MSG_NEVER;
} /* end should_message_be_processed() */

void rr_message_dispatcher(rr_cmd_type *cmd_ptr)
{
  rr_pending_queue_T      *pend_queue_ptr;
  rr_process_msg_action_e  process_message;

  /* single standby builds always use a default of "process now" */
  process_message = should_message_be_processed(&cmd_ptr->msg);
  
  switch (process_message)
  {
    case RR_PROCESS_MSG_NOW:
    {
      /* The message is being processed.  It may have previously been put
      on the pending queue.  This is indicated by the cmd_ptr->pended_count
      variable. */
      switch (cmd_ptr->msg.message_header.message_set)
      {
        case MS_MSGR_RR:
        {
#ifdef FEATURE_MSGR_RR_DEBUG
          rr_debug_log_imsg(
            cmd_ptr->msg.message_header.message_set,
            cmd_ptr->msg.rr_msgr_msg.message.hdr.id,
            cmd_ptr->pended_count
          );
#endif
          break;
        } /* MS_MSGR_RR */
      
        default:
        {
          rr_debug_log_imsg(
            cmd_ptr->msg.message_header.message_set,
            cmd_ptr->msg.message_header.message_id,
            cmd_ptr->pended_count
          );
        }
      }
      
      /* Process the message */
      if (cmd_ptr->pended_count > 0)
      {
        rr_process_message(
          &cmd_ptr->msg,
          RR_MESSAGE_TYPE_PENDED
        );
      }
      else
      {
        rr_process_message(
          &cmd_ptr->msg,
          RR_MESSAGE_TYPE_NEW
        );
      }
      
      /* Free memory allocated for this message */
      GPRS_MEM_FREE(cmd_ptr);
      /* Now check whether any messages from the pending queue can be transferred to the task queue */
      rr_check_for_pending_messages();
      break;
    }

    case RR_PROCESS_MSG_LATER:
    {
      /* determine the correct pending queue using the GAS ID */
      pend_queue_ptr = rr_get_pending_queue_ptr();
      //RR_NULL_CHECK_FATAL(pend_queue_ptr);

      /* Move the message to the pending queue */
      if (cmd_ptr->pended_count == 0)
      {
        /* This is the first time the message has been put on the pending
        queue, so add it to the trace buffer as it has jsut been received,
        even if RR is not able to process it yet. */
        switch (cmd_ptr->msg.message_header.message_set)
        {
          case MS_MSGR_RR:
          {
#ifdef FEATURE_MSGR_RR_DEBUG
            rr_debug_log_imsg(
              cmd_ptr->msg.message_header.message_set,
              cmd_ptr->msg.rr_msgr_msg.message.hdr.id,
              0
            );
#endif
            break;
          } /* MS_MSGR_RR */

          default:
          {
            rr_debug_log_imsg(
              cmd_ptr->msg.message_header.message_set,
              cmd_ptr->msg.message_header.message_id,
              0
            );
          }
        }
      }
      ++cmd_ptr->pended_count;
      q_put(&(pend_queue_ptr->rr_cmd_pend_q), &cmd_ptr->link);
      break;
    }

    case RR_PROCESS_MSG_NEVER:
    default:
    {
      /* Free memory allocated for this message */
      GPRS_MEM_FREE(cmd_ptr);
    }
  }
  return;
}/* rr_message_dispatcher */

#ifdef FEATURE_SGLTE
/*===========================================================================

FUNCTION     rr_sglte_reject_pending_x2g_req

DESCRIPTION  Check for L2G or T2G resel, CCO or redirection in pending queue, if abort rcvd is set to TRUE,
             it means that some X2G procedure got aborted but first check that the message is still in pending queue 
             waiting to be processd and then remove it and send abort rsp , if FALSE then reject the message as 
             it cant be processed

DEPENDENCIES None

RETURN VALUE  TRUE if message found else FALSE

SIDE EFFECTS 

===========================================================================*/  
boolean rr_sglte_reject_pending_x2g_req(boolean abort_rcvd )
{
  boolean x2g_req_found = FALSE;
  rr_pending_queue_T *pend_queue_ptr;
  rr_cmd_type        *cmd_ptr;

  pend_queue_ptr = rr_get_pending_queue_ptr();
  if (rr_is_sglte_mode_active())
  {
    if (pend_queue_ptr != NULL)
    {
       cmd_ptr = (rr_cmd_type *)q_check(&(pend_queue_ptr->rr_cmd_pend_q));
    }
    else
    {
      cmd_ptr = NULL;
    }

    while (cmd_ptr != NULL && x2g_req_found == FALSE)
    {
      switch (cmd_ptr->msg.message_header.message_set)
      {
#ifdef FEATURE_GSM_TDS
        case MS_TDSRRC_RR:
        {
          switch (cmd_ptr->msg.message_header.message_id)
          {
            case TDSRR_INTERRAT_RESELECTION_REQ:
            {
              if (abort_rcvd)
              {
                rr_x2g_send_x2g_resel_abort_rsp(RR_RAT_UTRAN_TDD);
              }
              else
              {
                rr_sglte_reject_tds_resel_req();
              }
              x2g_req_found = TRUE;
              break;
            }
            case TDSRR_INTERRAT_REDIRECT_REQ:
            {
              if (abort_rcvd)
              {
                rr_x2g_send_x2g_redir_abort_rsp(RR_RAT_UTRAN_TDD);
              }
              else
              {
                rr_sglte_reject_tds_redir_req();
              } 
              x2g_req_found = TRUE;
              break;
            }
            case TDSRR_INTERRAT_CC_ORDER_REQ:
            {
              if (abort_rcvd)
              {
                rr_x2g_send_x2g_cco_abort_rsp(RR_RAT_UTRAN_TDD);
              }
              else
              {               
                rr_x2g_send_x2g_cco_rsp_rejection(RR_RAT_UTRAN_TDD); 
              }
              x2g_req_found = TRUE;                
              break;
            }
            default:
            break;
          }
          break;
        }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
        case MS_MSGR_RR:
        {
          switch (cmd_ptr->msg.rr_msgr_msg.message.hdr.id)
          {
            case GERAN_GRR_LTE_RESEL_REQ:
            {
              if (abort_rcvd)
              {
                rr_x2g_send_x2g_resel_abort_rsp(RR_RAT_LTE);
              }
              else
              {
                rr_sglte_reject_lte_resel_req();
              }
              x2g_req_found = TRUE;
              break;
            }
            case GERAN_GRR_LTE_REDIR_REQ:
            {
              if (abort_rcvd)
              {
                rr_x2g_send_x2g_redir_abort_rsp(RR_RAT_LTE);
              }
              else
              {
                rr_sglte_reject_lte_redir_req();
              }
              x2g_req_found = TRUE;
              break;
            }
            case GERAN_GRR_LTE_CCO_REQ:
            {
              if (abort_rcvd)
              {
                rr_x2g_send_x2g_cco_abort_rsp(RR_RAT_LTE);
              }
              else
              {             
                rr_x2g_send_x2g_cco_rsp_rejection(RR_RAT_LTE);
              }
              x2g_req_found = TRUE;
              break;
            }
            default:
            break;
           }
          break;
          }
#endif /* FEATURE_LTE */

        default:
        break;
    }
       if (x2g_req_found)
       {
        if (cmd_ptr->msg.message_header.message_set == MS_TDSRRC_RR)
        {
           MSG_GERAN_HIGH_1_G("RR SGLTE rejecting T2G request msg_id %d", cmd_ptr->msg.message_header.message_id);
        }
        else if (cmd_ptr->msg.message_header.message_set == MS_MSGR_RR)
        {
       MSG_GERAN_HIGH_1_G("RR SGLTE rejecting L2G request msg_id %d", cmd_ptr->msg.rr_msgr_msg.message.hdr.id);
        }
            q_delete(
      &(pend_queue_ptr->rr_cmd_pend_q),
            &cmd_ptr->link);
        /* Free memory allocated for this message */
        GPRS_MEM_FREE(cmd_ptr);
       }
       else
       {
          cmd_ptr = (rr_cmd_type *) q_next(&(pend_queue_ptr->rr_cmd_pend_q), &cmd_ptr->link);
       }
     }
   }
  return x2g_req_found;
}
#endif /*FEATURE_SGLTE*/

/**
  @brief Check for messages on the pending message queue that may now be
  processed.

  This function iterates over each message in the pending message queue.  For
  each message, the function 'should_message_be_processed' is called.  If this
  function indicates that the message should be processed, the message is
  deleted from the pending message queue and placed on the command queue.

  @see should_message_be_processed

*/
void rr_check_for_pending_messages(
   
)
{
  rr_pending_queue_T *pend_queue_ptr;
  rr_cmd_type        *cmd_ptr;
  rr_cmd_type        *next_cmd_ptr;
  boolean             pending_message_found = FALSE;

  pend_queue_ptr = rr_get_pending_queue_ptr();

  /* get the mutex before starting queue operation */
  RR_Q_LOCK();

  if (pend_queue_ptr != NULL)
  {
    cmd_ptr = (rr_cmd_type *)q_check(&(pend_queue_ptr->rr_cmd_pend_q));
  }
  else
  {
    cmd_ptr = NULL;
  }

  if (cmd_ptr != NULL)
  {
    // Find the head of the normal command queue
    rr_cmd_type *norm_cmd_ptr = q_check(&rr_cmd_q);
    // Find the first non-internal (RR-RR) message
    while ((norm_cmd_ptr != NULL) &&
           (norm_cmd_ptr->msg.message_header.message_set == MS_RR_RR))
    {
      norm_cmd_ptr = (rr_cmd_type *) q_next(&rr_cmd_q, &norm_cmd_ptr->link);
    }
    // norm_cmd_ptr now points to the position in the command queue to insert the pending messages
    // (This will be NULL if the queue is empty at this position, or may point to a message, in which
    // case the pending messages need to be inserted at this position)

    do
    {
      /* Get the next cmd_ptr now, as we may delete the current queue element
      as a result of the following checks, so we will not be able to get the
      next element otherwise. */
      next_cmd_ptr = (rr_cmd_type *) q_next(&(pend_queue_ptr->rr_cmd_pend_q), &cmd_ptr->link);

      if (should_message_be_processed(&cmd_ptr->msg) == RR_PROCESS_MSG_NOW)
      {
        // This is the first queue item which is not MS_RR_RR
        if (norm_cmd_ptr == NULL)
        {
          pending_message_found = TRUE;
          q_delete(
            &(pend_queue_ptr->rr_cmd_pend_q),
            &cmd_ptr->link);
          q_put(&rr_cmd_q, &cmd_ptr->link);
        }
        else //There are elements in the command queue so insert first the pending messages
        {
          pending_message_found = TRUE;
          q_delete(
            &(pend_queue_ptr->rr_cmd_pend_q),
            &cmd_ptr->link);

          // Insert the new message into the queue
          q_insert(
            &rr_cmd_q,
            &cmd_ptr->link,
            &norm_cmd_ptr->link);
        }
      }

      cmd_ptr = next_cmd_ptr;
    } while (cmd_ptr != NULL);

    if (pending_message_found)
    {
      (void) rex_set_sigs( rex_self(), RR_CMD_Q_SIG );
    }
  }

  /* release the mutex */
  RR_Q_UNLOCK();
}

/**
  @brief Removes any messages from the RR pending queue
*/
void rr_flush_pending_messages(
   
)
{
  rr_pending_queue_T *pend_queue_ptr;
  rr_cmd_type        *cmd_ptr;
  rr_cmd_type        *next_cmd_ptr;

  MSG_GERAN_HIGH_0_G("Flushing pending message queue");

  pend_queue_ptr = rr_get_pending_queue_ptr();
  if (pend_queue_ptr != NULL)
  {
    cmd_ptr = (rr_cmd_type *)q_check(&(pend_queue_ptr->rr_cmd_pend_q));
  }
  else
  {
    cmd_ptr = NULL;
  }

  if (cmd_ptr != NULL)
  {
    do
    {
      /* Get the next cmd_ptr now, as we may delete the current queue element
      as a result of the following checks, so we will not be able to get the
      next element otherwise. */
      next_cmd_ptr = (rr_cmd_type *) q_next(&(pend_queue_ptr->rr_cmd_pend_q), &cmd_ptr->link);

      MSG_SPRINTF_1(
        MSG_SSID_DFLT,
        MSG_LEGACY_HIGH,
        "Discarding message: %s",
        rr_gprs_message_name(&cmd_ptr->msg, cmd_ptr->msg.message_header.message_set, cmd_ptr->msg.message_header.message_id, NULL)
      );

      q_delete(
        &(pend_queue_ptr->rr_cmd_pend_q),
        &cmd_ptr->link);

      /* Free memory allocated for this message */
      GPRS_MEM_FREE(cmd_ptr);

      cmd_ptr = next_cmd_ptr;
    } while (cmd_ptr != NULL);
  }
}

/**
  @brief Processes MPH_RESET_IND sent when GL1 triggers panic reset
*/
static void rr_handle_l1_reset_ind( )
{
  rr_panic_reset_triggered_by_l1();
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  (void)rr_control(EV_PANIC_RESET, NULL);
#else
  geran_initiate_recovery_restart_dyn(GERAN_CLIENT_GL1);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return;
}

#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK

typedef enum
{
  PEO_DSC,
  PEO_C1_DELTA_MIN,
  PEO_C1_DELTA_MAX,
  PEO_BCCH_CHANGE_MARK,
  IMPLICIT_REJECT,
  IMPLICIT_REJECT_DURING_RACH,
  PEO_RCC,
  ALLOW_PEO_RACH,
} param_to_update;

typedef enum
{
  EAB_SI21_POSITION,
  EAB_SI9_WHERE,
  EAB_SI9_CONTENT,
  EAB_SI21_CONTENT,
  EAB_CONFIG,
  EAB_SPECIAL_ACCESS_CLASS
} EAB_param_to_update;

static void rr_process_r13_param_from_diag_ind(uint8 cmd_type, uint8 cmd  )
{
  uint8    type;
  boolean  presence= FALSE;
  uint8    value ;
  
  /*Extract the type of command which is the last 3 bit from  MSB*/
  type     = (cmd >> 5);

  /*Extract the presence bit in  command which is the 4th  bit from MSB*/
  presence = ((cmd & 0x10) >> 4);

  /*Extract the value sent in  command which is the first 4 bit from LSB*/  
  value    = (cmd & 0x0f) ;

  /*For PEO and IR type*/
  if (cmd_type == GL1_SIMULATE_RR_R13_PEO_PARAMS)
  {
    switch (type)
    {
      case PEO_DSC:
      {
        if (presence)
        {
          rr_nv_update_peo_dsc_in_efs(value);
        }
        else
        {
          MSG_GERAN_HIGH_0_G("fourth bit is not set for PEO DSC, reset PEO support");
          rr_nv_update_peo_params_present_flag(FALSE);
        }
      }
      break;

      case PEO_C1_DELTA_MIN:
      {        
        if (presence)
        {
          rr_nv_update_peo_c1_delta_min_in_efs(value);
        }
        else
        {
          MSG_GERAN_HIGH_0_G("fourth bit is not set for C1_DELTA_MIN");
        }
      }
      break;

      case PEO_C1_DELTA_MAX:
      {
        if (presence)
        {
          rr_nv_update_peo_c1_delta_max_in_efs(value);
        }
        else
        {
          MSG_GERAN_HIGH_0_G("fourth bit is not set for C1_DELTA_MAX");
        }
      }
      break;

      case PEO_BCCH_CHANGE_MARK:
      {
        if (presence)
        {
          rr_nv_update_peo_bcch_change_mark_in_efs(value);
        }
        else
        {
          MSG_GERAN_HIGH_0_G("fourth bit is not set for PEO_BCCH_CHANGE_MARK");
        }
      }
      break;

      case IMPLICIT_REJECT:
      {        
        if (presence)
        {
          rr_nv_update_implict_reject_in_efs(value);
        }
        else
        {
          MSG_GERAN_HIGH_0_G("fourth bit is not set for Inplicit Reject");
        }
      }
      break;      

      case IMPLICIT_REJECT_DURING_RACH:
      {        
        if (presence)
        {
          rr_nv_update_implict_reject_during_rach_in_efs(value);
        }
        else
        {
          MSG_GERAN_HIGH_0_G("fourth bit is not set for Inplicit Reject during rach");
        }
      }
      break;      

      case PEO_RCC:
      {        
        if (presence)
        {
          rr_nv_update_peo_rcc_in_efs(value);
        }
        else
        {
          MSG_GERAN_HIGH_0_G("fourth bit is not set for RCC");
        }
      }
      break;      

      case ALLOW_PEO_RACH:
      {        
        if (presence)
        {
          rr_nv_update_allow_peo_rach_in_efs(TRUE);
        }
        else
        {
          MSG_GERAN_HIGH_0_G("fourth bit is not set for PEO RACH disable it");
          rr_nv_update_allow_peo_rach_in_efs(FALSE);
        }
      }
      break;

      default:
      {
        MSG_GERAN_HIGH_0_G("Invalid type passed");
      }
    }
  }
  else if (cmd_type == GL1_SIMULATE_RR_R13_EAB_PARAMS)
  {
    switch (type)
    {
      case EAB_SI21_POSITION:
      {
        rr_nv_update_SI21_indicator(presence);
        rr_nv_update_SI21_position(value);
        break;
      }
      case EAB_SI9_WHERE:
      {
        rr_nv_update_SI9_where_indicator(presence);
        rr_nv_update_SI9_where(value);
        break;
      }
      case EAB_SI9_CONTENT:
      {
        geran_nv_update_SI9_content_type(value);
        break;
      }
      case EAB_SI21_CONTENT:
      {
        geran_nv_update_SI21_content_type(value);
        break;
      }
      case EAB_CONFIG:
      {
        rr_nv_update_EAB_config(((value & 0x01) == TRUE));
        break;
      }
      case EAB_SPECIAL_ACCESS_CLASS:
      {
        rr_nv_update_special_access_class(((value & 0x01) == TRUE));
        break;
      }
      default:
      {
        MSG_GERAN_HIGH_0_G("Invalid type passed");
      }
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Invalid command type passed");
  }
}
#endif /*FEATURE_GERAN_R13_TEST_FRAMEWORK*/

/**
  @brief Processes a single message from RR task queue
*/
static void rr_process_message(
  rr_cmd_bdy_type   * new_message,
  rr_message_type_e   message_type
)
{
  byte protocol_discriminator;
  /*
  A MS_RR_RR_EXT message set is used when a RR-RR message in sent, but triggered by a call-back event,
  e.g. a call-back function is called, which sends a message into RR. The message set MS_RR_RR_EXT is used
  to ensure the message is queued as if received from an external task, rather than being queued ahead of
  other non-RR-RR messages. But once sent into RR, it can be processed as a standard RR-RR message
  */
  if (new_message->message_header.message_set == MS_RR_RR_EXT)
  {
    new_message->message_header.message_set = MS_RR_RR;
  }

  /* log the input message for RR */
  rr_show_input_message(
    message_type,
    new_message,
    new_message->message_header.message_set,
    new_message->message_header.message_id,
    rr_main_current_state_name(),
    get_rr_state()
  );

  if (new_message->message_header.message_set == MS_RR_RR)
  {
    // The destination for RR_*_IMSG_* messages is always the module that IMSG is defined for

    switch (new_message->rr.header.rr_message_set)
    {
#ifdef FEATURE_GSM_DTM
      case RR_DTM_IMSG:
      {
        (void)rr_dtm_control(EV_INPUT_MESSAGE, new_message);
        break;
      }
#endif /* FEATURE_GSM_DTM */

      case RR_PS_IMSG:
      {
        rr_ps_access_control(new_message);
        break;
      }

      case RR_L1_IDLE_IMSG:
      {
        rr_l1_idle_control(new_message);
        break;
      }

      case RR_SELECT_BCCH_IMSG:
      {
        rr_select_bcch_control(new_message);
        break;
      }

      case RR_RESEL_IMSG:
      {
        (void)rr_resel_control(EV_INPUT_MESSAGE, new_message);
        break;
      }

      case RR_PSCAN_IMSG:
      {
        rr_pscan_control(EV_INPUT_MESSAGE, new_message);
        break;
      }

      // TODO - all RR_*_IMSGs can be routed in this way

      default:
      {
        (void)rr_control(EV_INPUT_MESSAGE, new_message);
      }
    }
  }

  /***************************************************/
  /* The following statement deals with the messages */
  /* coming from the data link (DL) layer            */
  /***************************************************/
  else if (new_message->message_header.message_set == MS_RR_L2)
  {
    l2_channel_type_T l2_channel_type;
    byte              skip_indicator = 0x00;

    switch (new_message->message_header.message_id)
    {
#ifdef FEATURE_GERAN_CS_SUPPORT
      case DL_RELEASE_IND:
      {
        /**********************************************************/
        /* Forward the message to rr_control or SAPI3 control     */
        /**********************************************************/

        if (new_message->dl_release_ind.SAPI == SAPI3)
        {
          (void)rr_control_SAPI3_link(new_message);
        }
        else
        {
          rr_control(EV_INPUT_MESSAGE, new_message);
        }
      }
      break;
#endif /* FEATURE_GERAN_CS_SUPPORT */

#ifdef FEATURE_GERAN_CS_SUPPORT
      case DL_SUSPEND_CNF:
      case DL_RESUME_CNF:
      case DL_RECONNECT_CNF:
#endif /* FEATURE_GERAN_CS_SUPPORT */
      case DL_RANDOM_ACCESS_CNF:
      {
        /********************************************/
        /* Forward the message to the control of RR */
        /********************************************/
        rr_control(EV_INPUT_MESSAGE, new_message);
      }
      break;

#ifdef FEATURE_GERAN_CS_SUPPORT
      case MDL_ERROR_IND :
      {
        /*******************************************************/
        /* only respond to specific errors, such as T200 timer */
        /* expiry, sequence error, unsolicited DM frame with   */
        /* with F = 0, or N200 retries                         */
        /*******************************************************/

        if (
           (new_message->mdl_error_ind.error_code == T200_EXPIRY )      ||
           (new_message->mdl_error_ind.error_code == SEQUENCE_ERROR)    ||
           (new_message->mdl_error_ind.error_code == N200_EXCEEDED_PAR) ||
           (new_message->mdl_error_ind.error_code == UNSOLICITED_DM_RESPONSE_PAR)
           )
        {
          rr_event_T new_event;

          /********************************************/
          /* Forward the message to RR control or the */
          /* RR SAPI3 state machine                   */
          /********************************************/

          if (new_message->mdl_error_ind.SAPI == SAPI3)
          {
            /* SAPI3 state machine can generate a RADIO_LINK_FAILURE event here */
            new_event = rr_control_SAPI3_link(new_message);
          }
          else
          {
            new_event = EV_INPUT_MESSAGE;
          }

          if (new_event != EV_NO_EVENT)
          {
            rr_control(new_event, new_message);
          }
        }
        else
        {
          MSG_GERAN_MED_1_G("Ignoring MDL_ERROR_IND=%d from L2",(int) new_message->mdl_error_ind.error_code);
        }
      }
      break;

      case DL_RELEASE_CNF :
      {
        if (new_message->dl_release_cnf.SAPI == SAPI3)
        {
          (void)rr_control_SAPI3_link(new_message);
        }
        else
        {
          rr_control(EV_INPUT_MESSAGE, new_message);
        }
      }
      break;

      case DL_ESTABLISH_CNF :
      {
        /***************************************************/
        /* A layer 2 link has just been established. It is */
        /* either a SAPI 0 or SAPI 3 link                  */
        /***************************************************/
        if (new_message->dl_establish_cnf.SAPI == SAPI3)
        {
          /*******************************************/
          /* It is a SAPI 3 link : update the SAPI 3 */
          /* state machine                           */
          /*******************************************/
          (void)rr_control_SAPI3_link(new_message);
        }
        else
        {
          /*******************************************/
          /* It is a SAPI 0 link : pass the message  */
          /* to the main RR state machine            */
          /*******************************************/
          rr_control(EV_INPUT_MESSAGE, new_message);
        }
      }
      break;

      case DL_ESTABLISH_IND :
        if (new_message->dl_establish_ind.SAPI == SAPI0)
        {
          /* this must be a L2 re-establishment */
          rr_handle_l2_reestablishment(&(new_message->dl_establish_ind));

          /* log any changes to the channel configuration */
          (void)rr_log_channel_configuration();
        }
        else
        {
          (void)rr_control_SAPI3_link(new_message);
        }
        break;

      case DL_DATA_IND :
      {
        /**************************************************/
        /* This is a layer 3 peer to peer message.        */
        /* We have to check the protocol discriminator    */
        /* If the latter match the RR one, we have to     */
        /* forward the message to the RR main task. If    */
        /* not, we have to forward it to the layer above, */
        /* ie the MM layer.                               */
        /* The skip indicator has to be checked as well,  */
        /* but only if the message if for RR or MM        */
        /**************************************************/
        skip_indicator = new_message->dl_data_ind.layer3_message[0] & 0xF0;
        protocol_discriminator = new_message->dl_data_ind.layer3_message[0] & 0x0F;

#ifdef FEATURE_CGPS_UMTS_CP_GSM
        rr_cgps_apdu_in_progress_check(new_message);
        rr_cgps_lcs_activity_check(new_message);
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

        switch (protocol_discriminator)
        {
          case RADIO_RESSOURCES :
          {
            if (skip_indicator == 0x00)
            {
              /******************************************/
              /* The message is taken in consireration. */
              /* Pass it to the main RR state machine   */
              /******************************************/
              /* log the signalling message */
              (void) rr_log_signaling_message
                (
                  new_message->dl_data_ind.l2_channel_type,
                  /* MessageType is always in octet1 for DL_DATA_IND for RR */
                  new_message->dl_data_ind.layer3_message[1],
                  new_message->dl_data_ind.l3_message_length,
                  new_message->dl_data_ind.layer3_message,
                  RR_DOWNLINK_MSG
                );
              rr_control(EV_INPUT_MESSAGE, new_message);
            }
            else
            {
              MSG_GERAN_MED_0_G("Ignoring OTA message with skip_indicator set");
            }
          }
          break;

          case MOBILITY_MANAGEMENT :
          {
            if (skip_indicator == 0x00)
            {
              /******************************************/
              /* Forward the message to MM              */
              /******************************************/
              rr_send_data_ind(
                new_message->dl_data_ind.layer3_message,
                new_message->dl_data_ind.l3_message_length
              );
            }
          }
          break;

          case TEST_MANAGEMENT:
          {
            if (skip_indicator == 0x00)
            {
              channel_spec_T *chan_spec_ptr;

              /* log the signalling message */
              (void) rr_log_signaling_message(
                new_message->dl_data_ind.l2_channel_type,
                /* MessageType is always in octet1 for DL_DATA_IND for RR */
                new_message->dl_data_ind.layer3_message[1],
                new_message->dl_data_ind.l3_message_length,
                new_message->dl_data_ind.layer3_message,
                RR_DOWNLINK_MSG
              );

              /***************************/
              /* Check that a TCH is on. */
              /***************************/
              chan_spec_ptr = rr_get_channel_spec(NULL);
              if (chan_spec_ptr != NULL)
              {
                if ((chan_spec_ptr->number_of_ded_channels != 0) &&
                    ((chan_spec_ptr->channel_info_1_after.channel_type == FULL_RATE_TRAFFIC) ||
                     (chan_spec_ptr->channel_info_1_after.channel_type == HALF_RATE_TRAFFIC)))
                {
                  rr_handle_test_messages(&(new_message->message_header));
                }
                else
                {
                  MSG_GERAN_MED_1_G("Received TEST message=%d without TCH",
                          (int) new_message->dl_data_ind.layer3_message[1]);
                }
              }
            }
          } /* TEST_MANAGEMENT */
          break;

          #ifdef FEATURE_GSM_DTM
          case GTTP:
          {
            dl_data_ind_T * dl_data_ind = &new_message->dl_data_ind;

            rr_dtm_gttp_message_received(dl_data_ind);
            break;
          }
          #endif /* FEATURE_GSM_DTM */

          default :
          {
            /******************************************/
            /* Forward the message to MM              */
            /******************************************/
            rr_send_data_ind(
              new_message->dl_data_ind.layer3_message,
              new_message->dl_data_ind.l3_message_length
            );
          }
          break;
        } /* switch (protocol_discriminator) */
      }
      break;
#endif /* FEATURE_GERAN_CS_SUPPORT */

      case DL_UNIT_DATA_IND:
      {
        /***************************************************/
        /* This is a layer 3 peer to peer message received */
        /* in unacknowledgement manner.                    */
        /* We have to check the protocol discriminator     */
        /* If the latter match the RR one, we have to      */
        /* forward the message to the RR control  . If     */
        /* not, we have to forward it to the layer above,  */
        /* ie the MM layer.                                */
        /* The skip indicator has to be checked as well,   */
        /* but only if the message if for RR or MM         */
        /***************************************************/
        boolean short_header;
        byte    first_byte = 0;
        byte    second_byte = 0;
        dl_unit_data_ind_T *dl_unit_data_ind_ptr = &new_message->dl_unit_data_ind;

        l2_channel_type = dl_unit_data_ind_ptr->l2_channel_type;
        short_header = dl_unit_data_ind_ptr->short_header;

        if ((l2_channel_type == BCCH) || (l2_channel_type == CCCH))
        {
          skip_indicator = dl_unit_data_ind_ptr->layer3_message[1] & 0xF0;
          protocol_discriminator = dl_unit_data_ind_ptr->layer3_message[1] & 0x0F;
          first_byte = dl_unit_data_ind_ptr->layer3_message[1];
          second_byte = dl_unit_data_ind_ptr->layer3_message[2];
        }

        else if ((l2_channel_type == SACCH) && (short_header == TRUE))
        {
          /* 8th bit of octet 1 is the RR short PD */
          first_byte = dl_unit_data_ind_ptr->layer3_message[0];
          second_byte = dl_unit_data_ind_ptr->layer3_message[1];
          protocol_discriminator = dl_unit_data_ind_ptr->layer3_message[0] & 0x80;

          if (protocol_discriminator == RADIO_RESOURCES_SHORT_PD)
          {
            /* change it to RADIO_RESSOURCES */
            protocol_discriminator = RADIO_RESSOURCES;
            skip_indicator = 0x00;
          }
        }

        else
        {
          skip_indicator = dl_unit_data_ind_ptr->layer3_message[0] & 0xF0;
          protocol_discriminator = dl_unit_data_ind_ptr->layer3_message[0] & 0x0F;
          first_byte = dl_unit_data_ind_ptr->layer3_message[0];
          second_byte = dl_unit_data_ind_ptr->layer3_message[1];
        }

        switch (protocol_discriminator)
        {
          case RADIO_RESSOURCES:
          {
            byte message_type;
            byte message_length = dl_unit_data_ind_ptr->l3_message_length;

            if (skip_indicator == 0x00)
            {
              if ((l2_channel_type == BCCH) || (l2_channel_type == CCCH))
              {
                message_type = dl_unit_data_ind_ptr->layer3_message[2];
              }
#ifdef FEATURE_GERAN_CS_SUPPORT
              else if ((l2_channel_type == SACCH) && (short_header == TRUE))
              {
                message_type = dl_unit_data_ind_ptr->layer3_message[0] & 0x7C;

                /* shift right by 2 bits */
                message_type >>= 2;

                /* MEASUREMENT_INFORMATION is 0x5 which is also SYSTEM_INFORMATION_TYPE_5_BIS */
                /* So, change it to a different unused value */
                if (message_type == MEASUREMENT_INFORMATION)
                {
                  message_type = MEASUREMENT_INFORMATION_TYPE;
                }
                else
                {
                  /* Set to any other message type will be dropped at RR control */
                  message_type = UNKNOWN_SHORT_HEADER_MESSAGE_TYPE;
                }
              }
#endif /* FEATURE_GERAN_CS_SUPPORT */
              else
              {
                message_type = new_message->dl_unit_data_ind.layer3_message[1];
              }

              /* log the signalling message */
              (void) rr_log_signaling_message(
                l2_channel_type,
                message_type,
                message_length,
                dl_unit_data_ind_ptr->layer3_message,
                RR_DOWNLINK_MSG
              );

              switch (message_type)
              {
                case SYSTEM_INFORMATION_TYPE_1:
                case SYSTEM_INFORMATION_TYPE_2:
                case SYSTEM_INFORMATION_TYPE_2_BIS:
                case SYSTEM_INFORMATION_TYPE_2_TER:
                case SYSTEM_INFORMATION_TYPE_2_QUATER:
                case SYSTEM_INFORMATION_TYPE_3:
                case SYSTEM_INFORMATION_TYPE_4:
                case SYSTEM_INFORMATION_TYPE_5:
                case SYSTEM_INFORMATION_TYPE_5_TER:
                case SYSTEM_INFORMATION_TYPE_5_BIS:
                case MEASUREMENT_INFORMATION_TYPE:
                case SYSTEM_INFORMATION_TYPE_6:
                case SYSTEM_INFORMATION_TYPE_7:
                case SYSTEM_INFORMATION_TYPE_8:
                case SYSTEM_INFORMATION_TYPE_9:
                case SYSTEM_INFORMATION_TYPE_13:
                case SYSTEM_INFORMATION_TYPE_21:
                {
                  if (!grr_get_l1_in_test_mode())
                  {
                    if (message_length <= N201_MAX)
                    {
                      rr_store_system_info(dl_unit_data_ind_ptr);
                    }
                  }
                  break;
                }

                case PAGING_REQUEST_TYPE_1:
                case PAGING_REQUEST_TYPE_2:
                case PAGING_REQUEST_TYPE_3:
                {
                  boolean message_ok = TRUE;
                  RR_cause_T error_type;

                  // Check for errors in the encoding of the OTA message
                  if (dl_unit_data_ind_ptr->l3_message_length <= N201_MAX)
                  {
                    rr_check_for_protocol_errors(
                      dl_unit_data_ind_ptr->layer3_message,
                      dl_unit_data_ind_ptr->l3_message_length,
                      dl_unit_data_ind_ptr->l2_channel_type,
                      dl_unit_data_ind_ptr->short_header,
                      &message_ok,
                      &error_type
                    );
                  }
                  else
                  {
                    error_type = RR_SEMANTICALLY_INCORRECT_MESSAGE;
                    MSG_GERAN_MED_3_G("Message length exceeds message payload size:Received message length: %d,expected:%d, cause=%d",
                      dl_unit_data_ind_ptr->l3_message_length,
                      N201_MAX,
                      (int) error_type
                    );
                    message_ok = FALSE;
                  }

                  if (message_ok == TRUE)
                  {
                    // Process a paging request message. This could cause
                    //  - sending of RR_IMSG_PAGE_MODE_IND if the page mode has changed
                    //  - sending of RR_IMSG_PAGE_IND if the page addresses the MS
                    rr_ce_process_page_request(
                      message_type,
                      dl_unit_data_ind_ptr->own_paging_block
                    );
                  }
                  else /* the L3 message was not OK */
                  {
                    MSG_GERAN_ERROR_1_G("RR Bad Page Message (%d)",(int)error_type);
                  }
                  break;
                }

                default:
                {
                  rr_control(EV_INPUT_MESSAGE, new_message);
                }
              }
            }
            break;
          } /* case RADIO_RESSOURCES */

          default:
          {
            /**********************************************/
            /* The protocol discriminator in the msg      */
            /* is not the PD or RR. This is an impossible */
            /* case, as all the unacknowledge messages    */
            /* should be destinated to RR                 */
            /* However, it soes seen to happen            */
            /* occasionally...                            */
            /**********************************************/

            /* Ignore L2 padding octets */
            if(!((first_byte == 0x2B) && (second_byte == 0x2B)))
            {
              (void) rr_log_protocol_error(0, RR_PROTOCOL_ERROR_UNKNOWN_PD);
              ERR("RR Bad(%d) Protocol Discriminator",(int) protocol_discriminator,0,0);

              /**********************************************/
              /* Print dl_unit_data_ind contents for        */
              /* debugging                                  */
              /**********************************************/
              MSG_GERAN_HIGH_9_G("sapi:%d chtyp:%d ownpgblk:%d tc:%d chann:%d l3leng:%d, l3[0]:%d l3[1]:%d l3[2]:%d",
                new_message->dl_unit_data_ind.SAPI,
                new_message->dl_unit_data_ind.l2_channel_type,
                new_message->dl_unit_data_ind.own_paging_block,
                new_message->dl_unit_data_ind.tc,
                new_message->dl_unit_data_ind.channel_number.num,
                new_message->dl_unit_data_ind.l3_message_length,
                new_message->dl_unit_data_ind.layer3_message[0],
                new_message->dl_unit_data_ind.layer3_message[1],
                new_message->dl_unit_data_ind.layer3_message[2]);

              MSG_GERAN_HIGH_9_G(" l3[3]:%d l3[4]:%d l3[5]:%d l3[6]:%d l3[7]:%d l3[8]:%d l3[9]:%d l3[10]:%d l3[11]:%d",               
                new_message->dl_unit_data_ind.layer3_message[3],
                new_message->dl_unit_data_ind.layer3_message[4],
                new_message->dl_unit_data_ind.layer3_message[5],
                new_message->dl_unit_data_ind.layer3_message[6],
                new_message->dl_unit_data_ind.layer3_message[7],
                new_message->dl_unit_data_ind.layer3_message[8],
                new_message->dl_unit_data_ind.layer3_message[9],
                new_message->dl_unit_data_ind.layer3_message[10],
                new_message->dl_unit_data_ind.layer3_message[11]);
            }
            else
            {
              MSG_GERAN_MED_0_G("Ignoring L2 padding octets");
            }
            break;
          }
        } /* switch (protocol_discriminator) */

        break;
      } /* DL_DATA_IND */

      default:
      {
        /* some other L2 message not handled above */
        /* forward to control */
        rr_control(EV_INPUT_MESSAGE, new_message);
      }
      break;
    } /* switch(new_message->message_header.message_id) */
  } /* if (new_message->message_header.message_set == MS_RR_L2) */

  /***************************************************/
  /* The following statement deals with the messages */
  /* comming from the mobility management (MM) layer */
  /***************************************************/
  else if (new_message->message_header.message_set == MS_MM_RR)
  {
#ifdef FEATURE_GERAN_CS_SUPPORT
    protocol_discriminator =
      new_message->rr_data_req.layer3_message[0] & 0x0F;
#endif /* FEATURE_GERAN_CS_SUPPORT */

    switch (new_message->message_header.message_id)
    {
      case RR_SIM_INSERTED_REQ:
      {
        rr_sim_process_sim_inserted_req(
          &new_message->mm.rr_sim_inserted_req
        );
        break;
      }

      case RR_INVALIDATE_SIM_DATA_REQ:
      {
        rr_sim_process_invalidate_sim_data_req(
          &new_message->mm.rr_invalidate_sim_data_req
        );
        break;
      }

      case RR_SIM_UPDATE_REQ:
        {
        rr_sim_process_sim_update_req(
          &new_message->mm.rr_sim_update_req
        );
        break;
      }

      case RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ:
      {
        rr_ps_process_gmm_gprs_tlli_assignment_req(
          &new_message->mm.rr_gmm_gprs_tlli_assignment_req
        );
        break;
      }

      case RR_GMM_GPRS_SERVICE_REQ:
      {
        rr_ps_process_gmm_gprs_service_req(
          &new_message->mm.rr_gmm_gprs_service_req
        );
        break;
      }

      case RR_GMM_GPRS_STATE_CHANGE_REQ:
      {
        rr_ps_process_gmm_gprs_state_change_req(
          &new_message->mm.rr_gmm_gprs_state_change_req
        );

        rr_control(EV_INPUT_MESSAGE, new_message);
        break;
      }

      case RR_GMM_READY_TIMER_STATE_REQ:
      {
        rr_ps_process_gmm_ready_timer_state_req(
          &new_message->mm.rr_gmm_ready_timer_state_req
        );
        break;
      }

      case RR_GMM_GPRS_MS_RA_CAP_IE_IND:
      {
        rr_ps_process_gmm_gprs_ms_ra_cap_ie_ind(
          &new_message->mm.rr_gmm_gprs_ms_ra_cap_ie_ind
        );
        break;
      }

      case RR_GMM_START_MM_NON_DRX_IND:
      {
        rr_ps_process_gmm_start_mm_non_drx_ind(
          &new_message->mm.rr_gmm_start_mm_non_drx_ind
        );
        break;
      }

      case RR_GMM_STOP_MM_NON_DRX_IND:
      {
        rr_ps_process_gmm_stop_mm_non_drx_ind(
          &new_message->mm.rr_gmm_stop_mm_non_drx_ind
        );
        break;
      }

      case RR_GMM_GPRS_NV_PARAMS_IND:
      {
        // Note: This message is processed according to the AS-ID in the message, GAS-ID is ignored
        rr_nv_process_nv_params_ind(
          &new_message->rr_gmm_gprs_nv_params_ind
        );
        break;
      }

      case RR_LAI_REJECT_LIST_IND:
      {
        rr_store_lai_reject_list(
          &new_message->mm.rr_lai_reject_list_ind
        );
        break;
      }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      case RR_PS_ABORT_REQ:
      {
        rr_ps_process_ps_abort_req(&new_message->mm.rr_ps_abort_req);
        break;
      }

      case RR_PS_RESUME_IND:
      {
        rr_ps_process_ps_resume_ind(&new_message->mm.rr_ps_resume_ind);
        break;
      }

#ifdef FEATURE_DUAL_DATA
      case RR_DATA_PRIORITY_IND:
      {
        rr_data_priority_ind_T *msg_ptr = &new_message->mm.rr_data_prio_ind;
        geran_priority_t        priority;

        // If NAS is indidating high priority, change the L1 priority to match
        if (SYS_DATA_PRIORITY_HIGH == msg_ptr->priority)
        {
          priority = GERAN_MMS_PRIORITY_MAXIMAL;
        }
        else
        {
          priority = GERAN_MMS_PRIORITY_NOMINAL;
        }

        rr_send_mph_set_priority_ind(
          priority,   // l1_priority
          TRUE       // new_procedure
        );
        break;
      }
#endif /* FEATURE_DUAL_DATA */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef FEATURE_SGLTE
      case RR_STOP_GSM_MODE_REQ:
      {
        if (new_message->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_SGLTE_RAT_CHANGE)
        {
          rr_send_imsg(RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ);
        }
        else if (new_message->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_SGLTE_USER_RAT_CHANGE )
        {
          rr_send_imsg(RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ);
        }
        else
        {
          rr_control(EV_INPUT_MESSAGE, new_message);
       }
        break;
      } /* RR_STOP_GSM_MODE_REQ */
#endif /* FEATURE_SGLTE */

#if defined(FEATURE_GERAN_IMPLICIT_PS_REJECT) || defined(FEATURE_GERAN_EAB)
      case RR_DEVICE_PRIORITY_IND:
      {
#ifdef FEATURE_GERAN_IMPLICIT_PS_REJECT
        rr_update_device_priority(new_message->mm.rr_device_priority_ind.device_priority);
#endif /* FEATURE_GERAN_IMPLICIT_PS_REJECT */
#ifdef FEATURE_GERAN_EAB
        rr_update_eab_status(
          new_message->mm.rr_device_priority_ind.eab_enabled, 
          new_message->mm.rr_device_priority_ind.eab_cat_b, 
          new_message->mm.rr_device_priority_ind.eab_cat_c
        );
#endif /* FEATURE_GERAN_EAB */
        break;
      }
#endif /* FEATURE_GERAN_IMPLICIT_PS_REJECT || FEATURE_GERAN_EAB */
      default:
      {
        rr_control(EV_INPUT_MESSAGE, new_message);
      }
    }
  }

#ifdef FEATURE_CGPS_UMTS_CP_GSM
  /*******************************************************/
  /* The following statement deals with the messages     */
  /* coming from GPS                                     */
  /*******************************************************/
  else if (new_message->message_header.message_set == MS_CGPS_RR)
  {
    switch (new_message->message_header.message_id)
    {
      case RR_APDU_REQ:
      {
        /* Send APDU request to RR state machine for processing */
        rr_control(EV_INPUT_MESSAGE, new_message);
        break;
      }

      case RR_CGPS_EVENT_REQ:
      {
        /* Send CGPS request to RR state machine for processing */
        rr_control(EV_INPUT_MESSAGE, new_message);
        break;
      }

      default:
        break;
    }
  }
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

  else if (new_message->message_header.message_set == MS_RRC_RR)
  {
#ifdef FEATURE_FEMTO_GSM_NL
    // Pass all Femto-related message thru to the RR-FEMTO module.
    // Otherwise, pass into RR as normal
    switch (new_message->message_header.message_id)
    {
      case RRC_RR_NBR_DISC_REQ:
      case RRC_RR_START_COSC_REQ:
      case RRC_RR_STOP_COSC_REQ:
      {
        rr_femto_control(new_message);
        break;
      }

      default:
      {
        rr_control(EV_INPUT_MESSAGE, new_message);
      }
    }
#else
    rr_control(EV_INPUT_MESSAGE, new_message);
#endif // FEATURE_FEMTO_GSM_NL
  }
  /***************************************************/
  /* The following statement deals with the messages */
  /* coming from the physical layer                  */
  /***************************************************/

  else if (new_message->message_header.message_set == MS_RR_L1)
  {
    /*********************************************/
    /* Forward the message to the control of RR  */
    /*********************************************/

    switch (new_message->message_header.message_id)
    {
      case MPH_RESET_IND:
      {
        rr_handle_l1_reset_ind();
        break;
      }

      case MPH_START_GSM_MODE_CNF:
      {
        rr_event_T rr_event;

        rr_event = rr_mode_control(EV_INPUT_MESSAGE, new_message);
        rr_control(rr_event, new_message);
        break;
      }

      case MPH_START_GSM_MODE_FOR_BPLMN_CNF:
      {
        rr_event_T rr_event;

        rr_event = rr_mode_control(EV_INPUT_MESSAGE, new_message);
        rr_control(rr_event, NULL);
        break;
      }

      case MPH_STOP_GSM_MODE_CNF:
      {
        rr_event_T rr_event;

        rr_event = rr_mode_control(EV_INPUT_MESSAGE, new_message);
        rr_control(rr_event, NULL);
        break;
      }

      case MPH_START_IDLE_MODE_CNF:
      case MPH_ABORT_RA_CNF:
      {
        rr_l1_idle_control(new_message);
        break;
      }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      case MPH_L1_DS_ABORT_IND:
      {
        rr_ms_process_l1_ds_abort_ind(&new_message->mph.l1_ds_abort_ind);
        break;
      }

      case MPH_COEX_MSC_CHANGE_IND:
      {
        rr_msc_process_l1_coex_msc_change_ind(&new_message->mph.coex_msc_change_ind);
        break;
      }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef FEATURE_GERAN_R13_TEST_FRAMEWORK
      case MPH_SIMULATE_R13_PARAM_FROM_DIAG_IND:
      {
        rr_process_r13_param_from_diag_ind(new_message->mph.simulate_r13_param_from_diag.cmd_type, 
                                           new_message->mph.simulate_r13_param_from_diag.cmd);
        break;
      }

#endif /*FEATURE_GERAN_R13_TEST_FRAMEWORK*/
      default:
      {
        rr_event_T rr_event = rr_gprs_process_gprs_messages(new_message);

        if ( rr_event == EV_INPUT_MESSAGE )
        {
          rr_control(EV_INPUT_MESSAGE, new_message);
        }
        else
        if ( rr_event != EV_NO_EVENT )
        {
          rr_control(rr_event, NULL);
        }

        break;
      }
    }  /* switch */
  }
  else if ((new_message->message_header.message_set == MS_TIMER) &&
           (new_message->message_header.message_id == MID_TIMER_EXPIRY))
  {
    switch (new_message->mid_timer_expiry.timer_id)
    {
      case T3142:
      {
        rr_ps_t3142_expiry();
        break;
      }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      case RR_DUAL_SIM_HEARTBEAT_TIMER:
      {
        rr_ms_process_heartbeat();
        break;
      }

      case RR_DUAL_SIM_PS_ABORT_TIMER_1:
      case RR_DUAL_SIM_PS_ABORT_TIMER_2:
      {
        rr_ms_ps_abort_timer_expiry((rr_timer_id_e)(new_message->mid_timer_expiry.timer_id));
        break;
      }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef FEATURE_GERAN_IMPLICIT_PS_REJECT
      case T3236:
      {
        rr_ps_t3236_expiry();
        break;
      }
#endif /*FEATURE_GERAN_IMPLICIT_PS_REJECT*/

      default:
      {
        // Handle other timer expiries in RR-CONTROL according to state
        rr_control(EV_INPUT_MESSAGE, new_message);
      }
    }
  }

  else if (new_message->message_header.message_set == MS_GHDI_EXPORT)
  {
    /*********************************************/
    /* Forward the message to the control of RR  */
    /*********************************************/
    rr_control(EV_INPUT_MESSAGE, new_message);
  }

#ifdef FEATURE_GERAN_X2G_ACQ_DB_SCAN
  else if (new_message->message_header.message_set == MS_MSGR_RR)
  {
    switch (new_message->rr_msgr_msg.message.hdr.id)
    {
      case GERAN_GRR_LTE_GERAN_FREQ_INFO_IND:
      {
        rr_store_lte_geran_freq_info_ind(&(new_message->rr_msgr_msg.message.geran_grr_msg_ind.lte_geran_freq_info));
        break;
      }
      default:
      {
        // Handle other timer expiries in RR-CONTROL according to state
        rr_control(EV_INPUT_MESSAGE, new_message);
        break;
      }
    }
  }
#endif /*FEATURE_GERAN_X2G_ACQ_DB_SCAN*/

  else
  {
    rr_event_T  rr_event = rr_gprs_process_gprs_messages(new_message);

    if ( rr_event == EV_INPUT_MESSAGE )
    {
      rr_control(EV_INPUT_MESSAGE, new_message);
    }
    else
    if ( rr_event != EV_NO_EVENT )
    {
      rr_control(rr_event, NULL);
    }
  }
} /* rr_process_message */

/**
  @brief Makes the RR task wait for a specified interval

  Note: Synchronous function call - processing context is effectively "paused"

  @param millisecs - wait duration (ms)
*/
void rr_timed_wait(dword millisecs)
{
  timer_attrib_struct_type   rr_sleep_timer_attrib;
  timer_get_params(&rr_sleep_timer,&rr_sleep_timer_attrib);

  if (rr_sleep_timer_attrib.tcb_sigs_obj != NULL)
  {
    (void)rex_timed_wait((rex_sigs_type)RR_SLEEP_SIG,
                         &rr_sleep_timer,
                         (rex_timer_cnt_type)millisecs);
  }
}

/* EOF */

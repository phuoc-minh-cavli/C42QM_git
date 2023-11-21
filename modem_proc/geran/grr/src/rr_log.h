#ifndef RR_LOG_H
#define RR_LOG_H

/*! \file rr_log.h
 
  This is the header file for rr_log.c.
  This module contains RR logging functionality.
 
                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_log.h#4 $ */
/* $DateTime: 2018/11/28 04:48:52 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_log_types.h"
#include "rr_gprs_defs.h"
#include "log.h"
#include "event.h"
#include "rr_resel_calcs.h"
#include "rr_candidate_cell_db.h"
#include "gsmdiag_v.h"
#include "geran_dual_sim.h"
#include "rr_sys_info_cache.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Initialisation function for log-on-demand. The call-backs are registered.
 * 
 */
extern void rr_log_on_demand_init(
  
);

/*!
 * \brief Frees any memory used by the RR-LOG module and unregisters log-on-demand handlers.
 * 
 */
extern void rr_log_on_demand_deinit(
   
);

/*!
 * \brief Initialises a gsmdiag_pkt_info structure ready for use with logging functions.
 * 
 * \param info_ptr (out) - The gsmdiag_pkt_info structure to prepare
 * \param pkt_id (in)    - The log packet identifier
 * \param pkt_size (in)  - The size of the log packet
 */
extern void rr_log_packet_init(
  gsmdiag_pkt_info_t *info_ptr,
  log_code_type pkt_id,
  unsigned int pkt_size
   
);

#ifndef FEATURE_GERAN_REDUCED_DEBUG

/*!
 * \brief Outputs LOG_GSM_RR_STATE_C log packet.
 * 
 * \param state 
 * \param substate 
 * \param status 
 * \param mode 
 * 
 * \return boolean 
 */
extern boolean rr_log_state(
  byte state,
  byte substate,
  byte status,
  byte mode
   
);
#endif
/*!
 * \brief Outputs LOG_GSM_RR_PROTOCOL_ERROR_C log packet.
 * 
 * \param message_type 
 * \param protocol_error 
 * 
 * \return boolean 
 */
extern boolean rr_log_protocol_error(
  byte                        message_type,
  rr_protocol_error_enum_type error_type
);

/*!
 * \brief Outputs LOG_GSM_RR_SIGNALING_MESSAGE_C log packet.
 * 
 * \param channel_type 
 * \param message_type 
 * \param message_length 
 * \param message_data 
 * \param message_dir 
 * 
 * \return boolean 
 */
extern boolean rr_log_signaling_message(
  byte                               channel_type,
  byte                               message_type,
  byte                               message_length,
  byte                             * message_data,
  rr_signaling_message_direction_T   onDownlink
);

#ifndef FEATURE_GERAN_REDUCED_DEBUG

/*!
 * \brief Outputs LOG_GSM_RR_CELL_RESELECTION_PARAMS_C log packet.
 * 
 * \param csp_ptr 
 * 
 * \return boolean 
 */
extern boolean rr_log_cell_selection_parameters(
  cell_selection_parameters_T *csp_ptr
   
);

/*!
 * \brief Outputs LOG_GSM_RR_CELL_RESELECTION_PARAMS_C log packet if the contents have changed.
 * 
 * \param csp_ptr 
 * \param si_db_ptr 
 */

extern void rr_log_diff_cell_selection_parameters(
  cell_selection_parameters_T   * csp_ptr,
  system_information_database_T * si_db_ptr
   
);

/*!
 * \brief Outputs LOG_GSM_RR_RACH_CONTROL_PARAMS_C log packet.
 * 
 * \param rcp_ptr 
 * 
 * \return boolean 
 */
extern boolean rr_log_rach_control_parameters(
  RACH_control_parameters_T * rcp_ptr
   
);

/*!
 * \brief Outputs LOG_GSM_RR_RACH_CONTROL_PARAMS_C log packet if the contents have changed.
 * 
 * \param rcp_ptr 
 * \param si_db_ptr 
 */
extern void rr_log_diff_rach_control_parameters(
  RACH_control_parameters_T     * rcp_ptr,
  system_information_database_T * si_db_ptr
);

/*!
 * \brief Outputs LOG_GSM_RR_CONTROL_CHANNEL_PARAMS_C log packet.
 * 
 * \param ccd_ptr 
 * 
 * \return boolean 
 */
extern boolean rr_log_control_channel_description(
  control_channel_description_T * ccd_ptr
   
);

/*!
 * \brief Outputs LOG_GSM_RR_CONTROL_CHANNEL_PARAMS_C log packet if the contents have changed.
 * 
 * \param ccd_ptr 
 * \param si_db_ptr 
 */
extern void rr_log_diff_control_channel_description(
  control_channel_description_T * ccd_ptr,
  system_information_database_T * si_db_ptr
   
);

/*!
 * \brief Outputs LOG_GSM_RR_CELL_OPTIONS_C log packet.
 * 
 * \param cellop_ptr 
 * 
 * \return boolean 
 */
extern boolean rr_log_cell_options(
  cell_options_T * cellop_ptr
   
);

/*!
 * \brief Outputs LOG_GSM_RR_CELL_OPTIONS_C log packet if the contents have changed.
 * 
 * \param cellop_ptr 
 * \param si_db_ptr 
 */
extern void rr_log_diff_cell_options(
  cell_options_T                * cellop_ptr,
  system_information_database_T * si_db_ptr
);

/*!
 * \brief Outputs LOG_GSM_RR_CELL_INFO_C log packet.
 * 
 * \param sys_info_ptr 
 * 
 * \return boolean 
 */
extern boolean rr_log_cell_info(
  system_information_database_T * sys_info_ptr
   
);
#endif 
#ifdef FEATURE_GERAN_CS_SUPPORT
/*!
 * \brief Outputs LOG_GSM_RR_CHANNEL_CONFIGURATION_C log packet.
 * 
 * 
 * \return boolean 
 */
extern boolean rr_log_channel_configuration(
   
);
#endif /* FEATURE_GERAN_CS_SUPPORT */

/*!
 * \brief Outputs LOG_GSM_RR_CIPHERING_MODE_C log packet.
 * 
 * \param ciphering_flag 
 * \param ciphering_algorithm 
 * 
 * \return boolean 
 */
extern boolean rr_log_ciphering_mode(
  byte ciphering_flag,
  byte ciphering_algorithm
   
);

/*!
 * \brief Outputs LOG_GSM_RR_DSC_COUNTER_C log packet.
 * 
 * \param counter 
 */
extern void rr_log_dsc_counter(
  int counter
   
);

/*!
 * \brief Outputs LOG_GSM_RR_S_COUNTER_C log packet.
 * 
 * \param counter 
 */
extern void rr_log_s_counter(
  byte counter
   
);

/*!
 * \brief Outputs LOG_GSM_RR_SACCH_RPT_C log packet.
 * 
 * \param sys_info_ptr 
 * \param surr_meas_ptr 
 * \param RXLEV_FULL_average 
 * \param RXLEV_SUB_average 
 * \param RXQUAL_FULL_average 
 * \param RXQUAL_SUB_average 
 */
extern void rr_log_sacch_rpt(
  system_information_database_T * sys_info_ptr,
  surrounding_measurement_T     * surr_meas_ptr,
  byte                            RXLEV_FULL_average,
  byte                            RXLEV_SUB_average,
  byte                            RXQUAL_FULL_average,
  byte                            RXQUAL_SUB_average
);

/*!
 * \brief Outputs LOG_GSM_RR_AVAILABLE_PLMN_LIST_C log packet.
 * 
 * \param available_plmn_db_ptr 
 * 
 * \return boolean 
 */
extern boolean rr_log_available_plmn_list(
  rr_available_plmn_db_T *available_plmn_db_ptr
   
);

#ifdef FEATURE_WCDMA

/*!
 * \brief Ouptuts LOG_GSM_DED_3G_MEASUREMENTS_C log packet.
 * 
 * \param report 
 * 
 * \return boolean 
 */
extern boolean rr_log_3g_cell_list_report(
  rr_log_3g_meas_report_T * report
   
);

/*!
 * \brief Outputs LOG_GPRS_RR_3G_MEASUREMENT_PARAMS_C log packet.
 * 
 * \param in_dedicated 
 * 
 * \return boolean 
 */
extern boolean rr_log_rr_3g_measurement_params(
  boolean in_dedicated
   
);

/*!
 * \brief Outputs LOG_GPRS_RR_3G_RESELECTION_MEASUREMENTS_C log packet.
 * 
 * 
 * \return boolean 
 */
extern boolean rr_log_gprs_3g_reselection_measurements(
   
);

/*!
 * \brief Outputs LOG_GSM_IDLE_MODE_3G_RESELECT_REJ_C log packet.
 * 
 */
extern void rr_log_rrc_interrat_reselect_rej(
   
);

#endif /* FEATURE_WCDMA */

/*!
 * \brief Outputs LOG_GSM_RR_BA_LIST_C log packet.
 * 
 * \param balist 
 */
extern void rr_log_ba_list(
  inter_task_BCCH_allocation_T *balist
   
);

/*!
 * \brief Output LOG_GSM_RR_RANDOM_ACCESS_REQUEST_C log packet.
 * 
 * \param establishment_cause 
 * \param random_access_content 
 */
extern void rr_log_random_access_request(
  uint8 establishment_cause,
  uint8 random_reference
   
);
#ifndef FEATURE_GERAN_REDUCED_DEBUG

/*!
 * \brief Outputs LOG_GPRS_INDICATORS_C log packet.
 * 
 * \param ra_colour 
 * \param si13_position 
 * 
 * \return boolean 
 */
extern boolean rr_log_gprs_indicators(
  uint8 ra_colour,
  uint8 si13_position
   
);

/*!
 * \brief Outputs LOG_GPRS_INDICATORS_C log packet if the content has changed.
 * 
 * \param gprs_indicator 
 * \param ra_colour 
 * \param si13_position 
 * \param cell_info 
 */
extern void rr_log_diff_gprs_indicators(
  boolean             gprs_indicator,
  uint8               ra_colour,
  uint8               si13_position,
  gprs_scell_info_t * cell_info
);

/*!
 * \brief Outputs LOG_GPRS_GENERAL_PARAMS_C log packet.
 * 
 * \param spgc_ccch_sup 
 * \param priority_access_thr 
 * \param network_control_order 
 * \param routing_area_code 
 * \param sgsnr 
 * 
 * \return boolean 
 */
extern boolean rr_log_gprs_general_parameters(
  uint8 spgc_ccch_sup,
  uint8 priority_access_thr,
  uint8 network_control_order,
  uint8 routing_area_code,
  uint8 sgsnr
   
);

/*!
 * \brief Outputs LOG_GPRS_CELL_OPTIONS_C log packet.
 * 
 * 
 * \return boolean 
 */
extern boolean rr_log_gprs_cell_options(
  gprs_cell_options_t *
   
);

/*!
 * \brief Outputs LOG_GPRS_POWER_CONTROL_PARAMS_C log packet.
 * 
 * \param alpha 
 * \param t_avg_w 
 * \param t_avg_t 
 * \param pc_meas_chan 
 * \param n_avg_i 
 * 
 * \return boolean 
 */
extern boolean rr_log_gprs_power_control_parameters(
  uint8 alpha,
  uint8 t_avg_w,
  uint8 t_avg_t,
  boolean pc_meas_chan,
  uint8 n_avg_i
   
);

/*!
 * \brief Outputs LOG_GPRS_MOBILE_ALLOCATION_C log packet.
 * 
 * 
 * \return boolean 
 */
extern boolean rr_log_gprs_mobile_allocation(
  gprs_mobile_allocation_t *
   
);

/*!
 * \brief LOG_GPRS_MOBILE_ALLOCATION_C log packet.
 * 
 * 
 * \return boolean 
 */
extern boolean rr_log_gprs_mobile_allocation_rrps_freq_list(
  rrps_frequency_list_t *
   
);

/*!
 * \brief Outputs LOG_GPRS_GRR_STATE_C log packet.
 * 
 * \param grr_state 
 * 
 * \return boolean 
 */
extern boolean rr_log_gprs_grr_state(
  grr_state_t grr_state
   
);

/*!
 * \brief Outputs LOG_GPRS_RR_CELL_RESELECTION_PARAMS_C log packet.
 * 
 * \param scell_ptr 
 * 
 * \return boolean 
 */
extern boolean rr_log_gprs_cell_selection_parameters(
  gprs_scell_info_t * scell_ptr
   
);
#endif 
/*!
 * \brief Outputs LOG_GPRS_RR_CELL_RESELECTION_MEASMNTS_C log packet.
 * 
 * \param gprs_reselection_info_ptr 
 * \param recent_reselection 
 * 
 * \return boolean 
 */
extern boolean rr_log_gprs_cell_reselection_measurements(
  gprs_reselection_structure_T * gprs_reselection_info,
  boolean                        recent_reselection
);

/*!
 * \brief Sets the reselection trigger reason.
 *
 * The value of this trigger may be retrieved using rr_log_get_reselection_trigger.
 * \see rr_log_get_reselection_trigger
 * 
 * \param trigger (in) - the value to set the reselection trigger variable to
 */
extern void rr_log_set_reselection_trigger(
  rr_log_reselection_trigger_T trigger
   
);

/*!
 * \brief Returns the last stored reselection trigger.
 * 
 * 
 * \return rr_log_reselection_trigger_T 
 */
extern rr_log_reselection_trigger_T rr_log_get_reselection_trigger(
   
);

/*!
 * \brief Returns TRUE if there is no outstanding reselection end event, FALSE otherwise.
 * 
 * 
 * \return boolean 
 */
extern boolean rr_log_reselection_log_complete(
   
);

/*!
 * \brief Indicates if there are no outstanding cell selection log events
 * 
 * 
 * \return boolean - TRUE if selection logging counter is zero
 */
extern boolean rr_log_cell_selection_log_complete(
   
);

/*!
 * \brief Outputs EVENT_GSM_LINK_FAILURE event.
 * 
 * 
 * \return boolean 
 */
extern void rr_log_link_failure(
   
);

/*!
 * \brief Outputs EVENT_GSM_CELL_SELECTED event.
 * 
 * 
 * \return  
 */
extern void rr_log_cell_camp_status(
   
);

/*!
 * \brief Outputs EVENT_RANDOM_ACCESS_REQUEST event.
 * 
 * \param attempt 
 * \param frameNum 
 * \param reason 
 */
extern void rr_log_random_access_req(
  byte attempt,
  word frameNum,
  byte reason
   
);

/*!
 * \brief Outputs EVENT_GSM_HANDOVER_START event.
 * 
 * \param handover_type (in) - type of handover
 * \param local_arfcn (in)   - ARFCN of the target cell
 * \param bsic (in)          - BSIC of the target cell
 * \param handover_ref (in)  - handover reference (0 is not available)
 */
extern void rr_log_handover_start(
  rr_log_handover_type_T  handover_type,
  ARFCN_T                 arfcn,
  BSIC_T                  bsic,
  byte                    handover_ref
);

/*!
 * \brief Outputs EVENT_GSM_HANDOVER_END event indicating success.
 * 
 */
extern void rr_log_handover_success(
   
);

/*!
 * \brief Outputs EVENT_GSM_HANDOVER_END event indicating failure (with cause).
 * 
 * \param failure_reason (in) - reason for failure
 */
extern void rr_log_handover_failure(
  rr_log_handover_failure_T failure_reason
   
);

/*!
 * \brief Outputs EVENT_GSM_RESELECT_START event.
 * 
 * \param trigger - reason why the reselection started
 */
extern void rr_log_reselect_start(
  rr_log_reselection_trigger_T trigger
   
);

/*!
 * \brief Outputs EVENT_GSM_RESELECT_END and EVENT_GSM_RESELECT_END_V2 events indicating success.
 *          Outputs the DS versions of these events for Multisim.
 * \param arfcn (in) - ARFCN of the new cell
 * \param bsic (in)  - BSIC of the new cell
 */
extern void rr_log_reselect_success(
  ARFCN_T arfcn,
  BSIC_T bsic
   
);

/*!
 * \brief Outputs EVENT_GSM_RESELECT_END and EVENT_GSM_RESELECT_END_V2 events indicating failure (with cause).
 *          Outputs the DS versions of these events for Multisim.
 * \param failure_reason - reason for the failure
 */
extern void rr_log_reselect_failure(
  rr_log_reselection_failure_T failure_reason
   
);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)

/*!
 * \brief Outputs EVENT_GSM_TO_WCDMA_RESELECT_END event indicating failure (with cause).
 * 
 * \param uarfcn (in)         - W cell UARFCN
 * \param scr_code (in)       - W ccell PSC
 * \param failure_reason (in) - reason for the failure
 */
extern void rr_log_g2w_reselect_failure(
  word                         uarfcn,
  word                         scrambling_code,
  rr_log_reselection_failure_T failure_reason
);

/*!
 * \brief Outputs EVENT_GSM_TO_WCDMA_RESELECT_END event indicating success.
 * 
 * \param uarfcn (in)   - UARFCN of the new W cell
 * \param scr_code (in) - PSC of the new W cell
 */
extern void rr_log_g2w_reselect_success(
  word uarfcn,
  word scrambling_code
   
);

/*!
 * \brief Outputs EVENT_GSM_TO_WCDMA_REDIR_STARTED event.
 * 
 */
extern void rr_log_g2w_redir_started(
   
);

/*!
 * \brief Outputs EVENT_GSM_TO_WCDMA_REDIR_ENDED event indicating success.
 * 
 */
extern void rr_log_g2w_redir_success(
   
);

/*!
 * \brief Output EVENT_GSM_TO_WCDMA_REDIR_ENDED event with status indicating it was aborted.
 * 
 */
extern void rr_log_g2w_redir_aborted(
   
);

/*!
 * \brief Output EVENT_GSM_TO_WCDMA_REDIR_ENDED event with status indicating failure.
 * 
 */
extern void rr_log_g2w_redir_failure(
   
);

/*!
 * \brief Outputs EVENT_GSM_TO_TDSCDMA_REDIR_STARTED event.
 * 
 */
extern void rr_log_g2t_redir_started(
   
);

/*!
 * \brief Outputs EVENT_GSM_TO_TDSCDMA_REDIR_ENDED event indicating success.
 * 
 */
extern void rr_log_g2t_redir_success(
   
);

/*!
 * \brief Output EVENT_GSM_TO_TDSCDMA_REDIR_ENDED event with status indicating it was aborted.
 * 
 */
extern void rr_log_g2t_redir_aborted(
   
);

/*!
 * \brief Output EVENT_GSM_TO_TDSCDMA_REDIR_ENDED event with status indicating failure.
 * 
 */
extern void rr_log_g2t_redir_failure(
   
);


#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
/*!
 * \brief Outputs EVENT_GSM_TO_LTE_RESEL_STARTED event.
 * 
 * \param earfcn 
 * \param pcid 
 */
extern void rr_log_g2l_resel_started(
  lte_earfcn_t earfcn,
  uint16 pcid
   
);

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_RESEL_ENDED event indicating success.
 * 
 * \param earfcn 
 * \param pcid 
 */
extern void rr_log_g2l_resel_success(
  lte_earfcn_t earfcn,
  uint16 pcid
   
);

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_RESEL_ENDED event with status indicating it was aborted.
 * 
 * \param earfcn 
 * \param pcid 
 */
extern void rr_log_g2l_resel_aborted(
  lte_earfcn_t earfcn,
  uint16 pcid
   
);

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_RESEL_ENDED event with status indicating failure.
 * 
 * \param earfcn 
 * \param pcid 
 * \param status 
 */
extern void rr_log_g2l_resel_failure(
  lte_earfcn_t earfcn,
  uint16 pcid,
  rr_log_g2l_resel_status_e status
   
);

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_REDIR_STARTED event.
 * 
 */
extern void rr_log_g2l_redir_started(
   
);

/*!
 * \brief Outputs EVENT_GSM_TO_LTE_REDIR_ENDED event indicating success.
 * 
 */
extern void rr_log_g2l_redir_success(
   
);

/*!
 * \brief Output EVENT_GSM_TO_LTE_REDIR_ENDED event with status indicating it was aborted.
 * 
 */
extern void rr_log_g2l_redir_aborted(
   
);

/*!
 * \brief Output EVENT_GSM_TO_LTE_REDIR_ENDED event with status indicating failure.
 * 
 */
extern void rr_log_g2l_redir_failure(
   
);

#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS

/**
  @brief Sends EVENT_GSM_TO_TDSCDMA_RESEL_START event with target cell UARFCN
         and Cell Parameter ID in payload.
*/

extern void rr_log_g2t_reselect_started(word uarfcn, word cpid );

/**
  @brief Sends EVENT_GSM_TO_TDSCDMA_RESEL_END event with target cell UARFCN
          and Cell Parameter ID and success indicator in the payload.
*/

extern void rr_log_g2t_reselect_success(word uarfcn, word cpid  );

/**
  @brief Sends EVENT_GSM_TO_TDSCDMA_RESEL_END event with target cell UARFCN
         and Cell Parameter ID and failure reason in payload.
*/

extern void rr_log_g2t_reselect_failure(word uarfcn, word cpid, rr_log_reselection_failure_T failure_reason  );


#endif /* FEATURE_GSM_TO_LTE */

/*!
 * \brief Outputs EVENT_GSM_MESSAGE_RECEIVED event.
 * 
 * \param message_id 
 * \param pd_id 
 * \param channel_id 
 */
extern void rr_log_gsm_message_received(
  byte message_id,
  byte pd_id,
  byte channel_id
   
);

/*!
 * \brief Outputs EVENT_GSM_RR_IN_SERVICE or EVENT_GSM_RR_OUT_OF_SERVICE event.
 * 
 * \param in_service (in) - TRUE if in service; FALSE otherwise
 */
extern void rr_log_service_change(
  boolean in_service
   
);

/*!
 * \brief Outputs EVENT_GSM_PAGE_RECEIVED/EVENTS_DS_GSM_PAGE_RECEIVED event.
 * 
 */
extern void rr_log_page_received(
   
);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Outputs EVENTS_DS_GSM_PAGE_MISSED event.
 * 
 */
extern void rr_log_page_missed(
   
);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM*/


/*!
 * \brief Outputs EVENT_GSM_CAMP_ATTEMPT_START & EVENT_GSM_CAMP_ATTEMPT_START_V2 events.
 * 
 * \param local_arfcn 
 * \param bsic 
 */
extern void rr_log_camping_attempt_start(
  ARFCN_T arfcn,
  BSIC_T bsic
   
);

/*!
 * \brief Outputs EVENT_GSM_CAMP_ATTEMPT_END event (with failure reason)
 * 
 * \param failure_reason (in) - why the camp attempt failed
 */
extern void rr_log_camping_attempt_failure(
  rr_log_camping_attempt_failure_T failure_reason
   
);

/*!
 * \brief Outputs EVENT_GSM_CAMP_ATTEMPT_END event.
 * 
 */
extern void rr_log_camping_attempt_success(
   
);

/*!
 * \brief Outputs EVENT_GSM_ACCESS_FAILURE event.
 * 
 * \param access_failure_reason 
 * \param access_reason 
 * \param no_of_rach_attemps 
 */
extern void rr_log_gsm_access_failure(
  rr_log_access_failure_reason_T access_failure_reason,
  uint8                          access_reason,
  uint8                          no_of_rach_attemps
);

/*!
 * \brief Outputs EVENT_GSM_CALL_DROP event.
 * 
 * \param call_drop_reason (in)
 * \param access_reason (in)
 */
extern void rr_log_gsm_call_drop(
  rr_log_call_dropped_reason_T call_drop_reason,
  uint8                        access_reason
);

/*!
 * \brief Outputs EVENT_GSM_CELL_SELECTION_START event.
 * 
 * \param start_reason 
 * \param any_cell_selection_mode 
 * \param automatic_PLMN_selection_criteria 
 */
extern void rr_log_cell_selection_start(
  rr_log_cell_selection_start_reason_T start_reason,
  boolean                              any_cell_selection_mode,
  boolean                              automatic_PLMN_selection_criteria
);

/*!
 * \brief Outputs EVENT_GSM_CELL_SELECTION_END event.
 * 
 * \param end_reason 
 */
extern void rr_log_cell_selection_end(
  rr_log_cell_selection_end_reason_T end_reason
   
);

#ifndef FEATURE_GERAN_REDUCED_DEBUG

/*!
 * \brief Outputs EVENT_GSM_POWER_SCAN_STATUS event.
 * 
 */
extern void rr_log_power_scan_started(
   
);

/*!
 * \brief Outputs EVENT_GSM_POWER_SCAN_STATUS event.
 * 
 */
extern void rr_log_power_scan_completed(
   
);

/*!
 * \brief Outputs EVENT_GSM_POWER_SCAN_STATUS event indicating that a reselection power-scan has started.
 * 
 */
extern void rr_log_reselection_power_scan_started(
   
);

/*!
 * \brief Outputs EVENT_GSM_POWER_SCAN_STATUS event indicating that a reselection power-scan has completed.
 * 
 */
extern void rr_log_reselection_power_scan_completed(
   
);
#endif
/*!
 * \brief Outputs EVENT_GSM_PLMN_LIST_START event.
 * 
 * \param search_mode 
 */
extern void rr_log_plmn_list_started(
  rr_log_plmn_list_search_mode_T search_mode
   
);

/*!
 * \brief Outputs EVENT_GSM_PLMN_LIST_END event.
 * 
 */
extern void rr_log_plmn_list_completed(
   
);

#ifdef FEATURE_GERAN_CS_SUPPORT
#ifndef FEATURE_GERAN_REDUCED_DEBUG

/*!
 * \brief Outputs Sends EVENT_GSM_AMR_MULTIRATE_IE event.
 * 
 * \param config_ptr 
 * \param no_of_codec_modes 
 */
extern void rr_log_multirate_parameters(
  multirate_config_T * config_ptr,
  byte no_of_codec_modes
   
);
#endif /* FEATURE_GERAN_CS_SUPPORT */
#endif

/*!
 * \brief Outputs EVENT_GPRS_PAGE_RECEIVED/EVENTS_DS_GPRS_PAGE_RECEIVED event.
 * 
 */
extern void rr_log_gprs_page_received(
   
);
#ifndef FEATURE_GERAN_REDUCED_DEBUG

/*!
 * \brief Outputs EVENT_GPRS_SURROUND_SEARCH_START event.
 * 
 * \param bcch_arfcn 
 * \param pbcch_arfcn 
 */
extern void rr_log_gprs_surround_search_start(
  ARFCN_T bcch_arfcn,
  ARFCN_T pbcch_arfcn
   
);

/*!
 * \brief Outputs EVENT_GPRS_SURROUND_SEARCH_END with a failure reason.
 * 
 * \param failure_reason 
 */
extern void rr_log_gprs_surround_search_failure(
  rr_log_gprs_surround_search_failure_T failure_reason
   
);

/*!
 * \brief Outputs EVENT_GPRS_SURROUND_SEARCH_END event indicating success.
 * 
 */
extern void rr_log_gprs_surround_search_success(
   
);


/*!
 * \brief Outputs EVENT_GPRS_EARLY_CAMPING event.
 * 
 * \param bcch_arfcn 
 * \param bsic 
 */
extern void rr_log_gprs_early_camping(
  ARFCN_T bcch_arfcn,
  BSIC_T bsic
   
);
#endif
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)

/*!
 * \brief Outputs EVENT_GERAN_GRR_REDIR_STARTED event.
 * 
 * \param rat 
 */
extern void rr_log_redir_started(
  rr_log_rat_e rat
   
);

/*!
 * \brief Outputs EVENT_GERAN_GRR_REDIR_COMPLETED event.
 * 
 */
extern void rr_log_redir_completed(
   
);

/*!
 * \brief Outputs EVENT_GERAN_GRR_REDIR_ABORTED event.
 * 
 */
extern void rr_log_redir_aborted(
   
);

#endif /* FEATURE_WCDMA || FEATURE_LTE */

#if defined (FEATURE_WCDMA) || defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
/*!
 * \brief Logs IRAT measurements.
 * 
 * \param irat_measurements 
 */
extern void rr_log_irat_measurements(
  const mph_surround_irat_meas_ind_t *
   
);
#endif /*FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

#ifdef FEATURE_RR_SI_CACHE
/*!
 * \brief Logs SI Cache flushes.
 *
 * \param 
 */
extern void rr_log_csi_flush_data(csi_entry_t *entry_ptr,
  const rr_si_cache_flush_cause_T csi_flush_cause
   );

/*!
 * \brief Logs SI Cache entry updates.
 *
 * \param 
 */
extern void rr_log_csi_update_entry(csi_entry_t *entry_ptr,
  const rr_si_cache_update_status_T status
   );

/*!
 * \brief Logs SI Cache retrieve entry.
 *
 * \param 
 */
extern void rr_log_csi_retrieve_entry(csi_entry_t *entry_ptr
   
);
#endif /* FEATURE_RR_SI_CACHE */

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 *
 *  Note: This intialises all data-spaces.
 */
extern void rr_log_task_start_init(
  void
);

#endif /* RR_LOG_H */

/* EOF */


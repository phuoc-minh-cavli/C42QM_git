#ifndef RR_PSCAN_H
#define RR_PSCAN_H

/*! \file rr_pscan.h 
 
  This module implements RR functionality related to requesting and processing the
  results of power scans by L1.
 
  A set of functions are provided to encapsulate sending a variety of different
  power scan requests to L1 and perform generic processing when L1 indicates thqt
  it has completed the power scan.
 
                Copyright (c) 2007-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_pscan.h#5 $ */
/* $DateTime: 2019/03/18 00:27:08 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#include "rr_l1.h"
#include "rr_l1_g.h"
#include "sys.h"
#include "sys_v.h"
#include "rr_defs.h"
#include "rr_task.h"
#include "rr_candidate_cell_db.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum
{
  RR_PSCAN_REQ_NONE,
  RR_PSCAN_REQ_CELL_SELECTION
}rr_pscan_request_type_e;

/**
 * PSCAN related data
 */
/**
 * PSCAN states, possible state transitions:
 * INACTIVE -> IN_PROGRESS -> INACTIVE;
 * INACTIVE -> IN_PROGRESS -> ABORT -> INACTIVE;
 */
typedef enum
{
  RR_PSCAN_INACTIVE,
  RR_PSCAN_IN_PROGRESS,
  RR_PSCAN_ABORT
} rr_pscan_state_e;

#ifdef FEATURE_DUAL_SIM
typedef struct
{
  boolean              pending_request;
  word                 num_frequencies;
  word                 storage_limit;
  rr_l1_freq_power_T  *meas;
} rr_common_results_T;
#endif /* FEATURE_DUAL_SIM */
typedef struct
{
  rr_pscan_state_e   state;
  rr_pscan_state_e   old_state;
  rr_search_mode_T   search_mode;
  sys_band_mask_type band_pref;
} rr_pscan_control_data_t;

typedef struct
{
  rr_pscan_control_data_t rr_pscan_control_data;
  rr_pscan_results_db_T   rr_pscan_results_db;
  boolean                 rr_pscan_last_acq_db_freqs_valid;
  rr_l1_pscan_results_T   rr_pscan_last_acq_db_freqs;
#ifdef FEATURE_DUAL_SIM
  rr_common_results_T     common_results;
#endif
} rr_pscan_data_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Sends a power scan request to L1 for all frequencies in each band specified in 'band_pref'.
 *
 * \param search_mode 
 * \param band_pref 
 * \param filter_acq_db_freqs 
 * 
 * \return rr_event_T - RR_EV_PERFORM_POWER_SCAN if a foreground power scan request is sent to L1
 *                    - RR_EV_BACKGROUND_PLC_STARTED if a background power scan request is sent to L1
 */
extern rr_event_T rr_perform_power_scan(
  rr_search_mode_T search_mode,
  sys_band_mask_type band_pref,
  boolean filter_acq_db_freqs
   
);

/*!
 * \brief Perform a power scan based on information in the acquisition database.
 *
 *  Sends a power scan request to L1 including all ARFCNs on all bands included
 *  in the specified band-pref, but only if they match entries in the acquisition
 *  database.
 * 
 * \param pscan_db - Pointer to the power scan results database to use.
 * \param band_pref - Bitmask of the bands that should be included in the search.
 * \param plmn_id - The PLMN to search for.
 * \param requested_plmn_only - TRUE if RR should include on ARFCNs that are known to be associated with the
 *                              PLMN to search for
 *                            - FALSE if RR should include ARFCNs from other known PLMNs.
 * \param req_type - reason for acq db power scan
 * 
 * \return rr_event_T - EV_NO_EVENT if a power scan request is sent to L1 and RR should wait for the
 *                      power scan confirm to be received.
 *                    - EV_PSCAN_FAILURE if no frequencies were available to send to L1.
 */
extern rr_event_T rr_pscan_perform_pscan_acq_db(
  rr_pscan_results_db_T *pscan_db,
  sys_band_mask_type band_pref,
  sys_plmn_id_s_type plmn_id,
  boolean requested_plmn_only,
  rr_search_mode_T search_mode,
  rr_pscan_request_type_e req_type
   
);

/*!
 * \brief Generic processing of MPH_POWER_SCAN_CNF.
 * 
 * \param msg_ptr 
 */
extern void rr_process_mph_power_scan_cnf(
  mph_power_scan_cnf_T *msg_ptr
);

/*!
 * \brief This function must be called when receiving MPH_POWER_SCAN_CNF. 
 *  
 *  It manipulates power scan results from L1 (based on FEATUREs) and then populates the Candidate Cell DB
 *  with the results.
 *  
 * \param search_order 
 * 
 * \return word 
 */
extern word rr_pscan_process_power_scan_results(
  rr_search_order_e search_order
   
);

/*!
 * \brief Returns TRUE if the contents of the power scan results database is; FALSE otherwise.
 * 
 * The validity of the power scan results database is determined by examining the state of the 
 * RR_INVALIDATE_PSCAN_RESULTS_TIMER - if it is running, the power scan results database is assumed to be
 * valid, if it is expired or stopped the power scan results database is assumed to be invalid.
 *  
 * 
 * \return boolean 
 */
extern boolean rr_pscan_is_valid(
   
);

/*!
 * \brief Invalidates the power scan results without clearing them.
 * 
 */
extern void rr_invalidate_power_scan_results(
   
);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Mark an entry in the acquisition DB results as "blocked". 
 *  
 *  An entry marked as "blocked" will NOT be filtered from any subsequent scans.
 * 
 * \param blocked_arfcn 
 */
extern void rr_ms_pscan_mark_arfcn_blocked(
  ARFCN_T blocked_arfcn
   
);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*!
 * \brief Clears the power-scan database.
 * 
 * \param pscan_db_ptr 
 */
extern void rr_pscan_clear_results(
  rr_pscan_results_db_T *pscan_db_ptr
);

/*!
 * \brief Adds a list of frequences to the power-scan database.
 * 
 * \param arfcn_list 
 * \param no_of_entries 
 */
extern void rr_pscan_add_frequencies(
  ARFCN_T *arfcn_list, 
  uint16 no_of_entries
   
);

/*!
 * \brief Returns the number of entries in the power scan results database.
 * 
 * \param pscan_db_ptr - pointer to power-scan database
 * 
 * \return word - Number of frequencies in the database
 */
extern word rr_pscan_get_num_frequencies(
  rr_pscan_results_db_T *pscan_db_ptr
);

/*!
 * \brief Stores the strongest N frequencies from the power scan resuls in the candidate cell DB. 
 *  
 *  Stores the strongest N frequencies from the power scan results starting from the given offset into the
 *  given candidate cell database (which is completely cleared before storing) in order of decreasing
 *  signal signal strength.
 * 
 * \param pscan_db_ptr - pointer to power scan database
 * \param candidate_cell_db_ptr - pointer to candidate cell database
 * \param power_scan_offset - power scan offset (number of strongest frequencies to skip)
 * \param num_frequencies - number of frequencies to extract from power scan
 * \param search_order 
 * 
 * \return word - actual number of frequencies that were extracted from power scan results
 */
extern word rr_pscan_store_strongest_frequencies_in_candidate_db(
  rr_pscan_results_db_T *pscan_db_ptr,
  rr_candidate_cell_db_T *candidate_cell_db_ptr,
  word power_scan_offset,
  word num_frequencies,
  rr_search_order_e search_order
   
);

/*!
 * \brief Returns the strongest N frequencies in the power scan database that are also in the given list. 
 *  
 *  Returns the strongest N frequencies in the power scan database that are also in the given list. The
 *  resulting frequencies are returned in the given output buffer.
 *  Note: The output buffer must have enough space to store the results.
 * 
 * \param pscan_db_ptr - pointer to power scan database
 * \param num_frequencies - number of frequencies to extract from the power scan results
 * \param list_num_frequencies - number of frequencies in the provided list
 * \param frequency_list - array of frequencies to search for in power scan
 * \param result_buffer_ptr - Result where the frequencies/powers found are stored.
 * 
 * \return int 
 */
extern int rr_pscan_get_strongest_frequencies_in_list(
  rr_pscan_results_db_T *pscan_db_ptr,
  int num_frequencies,
  int list_num_frequencies,
  ARFCN_T *frequency_list,
  rr_l1_freq_power_T *result_buffer_ptr
);

/*!
 * \brief Must be called before the RR-PSCAN module is used to perform a power-scan
 * 
 */
extern void rr_pscan_setup(
   
);

/*!
 * \brief The power-scan control state-machine
 * 
 * \param event 
 * \param message_ptr 
 */
extern void rr_pscan_control(
  rr_event_T event,
  rr_cmd_bdy_type *message_ptr
   
);

/*!
 * \brief Returns a pointer to rr_pscan_results_db.
 * 
 * 
 * \return rr_pscan_results_db_T* 
 */
rr_pscan_results_db_T *rr_get_pscan_results_db_ptr(
   
);

/*!
 * \brief Sets the band preference to be used for a power-scan.
 * 
 * \param band_pref 
 */
extern void rr_pscan_set_band_pref(
  sys_band_mask_type band_pref
   
);

/*!
 * \brief Returns TRUE if power-scan results are available for the given band_pref.
 * 
 * 
 * \return boolean - TRUE if power-scan results are available, FALSE otherwise
 */
extern boolean rr_pscan_results_are_reusable(
  sys_band_mask_type band_pref
   
);

/*!
 * \brief Returns TRUE if power-scan data capture mode is enabled; FALSE otherwise.
 * 
 * 
 * \return boolean 
 */
extern boolean rr_pscan_data_capture_enabled(
   
);

#ifdef FEATURE_DUAL_SIM
/*!
 * \brief Searches other data spaces for reusable power scan results.
 * 
 * \param req_ptr 
 * 
 */
extern void rr_pscan_prepare_common_results(mph_power_scan_req_T *req_ptr);

/*!
 * \brief Discards any reusable power scan results stored previously
 * 
 * 
 */
extern void rr_pscan_discard_common_results( );
#endif
/*!
 * \brief Determines if an ARFCN is in the supplied
 * pscan request
 * 
 * \param pscan_request_ptr
 * \param arfcn
 * 
 */
extern boolean rr_pscan_arfcn_present(rr_l1_pscan_results_T *pscan_request_ptr, uint16 num_freq, ARFCN_T arfcn  );
extern void rr_pscan_task_start_init(void);

#ifdef FEATURE_PS_PREFERRED_CELL
extern void rr_pscan_remove_unprocessed_freq_from_last_pscan(const rr_candidate_cell_db_T *candidate_db_ptr);
#endif

#endif /* ifndef RR_PSCAN_H */

/* EOF */


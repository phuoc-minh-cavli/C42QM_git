#ifndef RR_CANDIDATE_CELL_DB_H
#define RR_CANDIDATE_CELL_DB_H

/*! \file rr_candidate_cell_db.h 
 
  This is the header file for rr_candidate_cell_db.c
 
                Copyright (c) 2007-2013 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_candidate_cell_db.h#10 $ */
/* $DateTime: 2020/08/04 06:38:13 $$Author: pwbldsvc $ */


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_defs.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define RR_NON_CANDIDATE_CELL (RR_MAX_CANDIDATE_FREQ_LIST+1)


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  RR_CANDIDATE_VALID_BCCH,
  RR_CANDIDATE_NOT_BCCH,
  RR_CANDIDATE_BCCH_WITH_DECODE_FAILURES,
  RR_CANDIDATE_UNKNOWN,
  RR_CANDIDATE_FILTERED
} rr_candidate_freq_type_enum_T;

typedef struct
{
  byte    RXLEV_ACCESS_MIN;
  byte    MS_TXPWR_MAX_CCH;
  byte    POWER_OFFSET;
  boolean CELL_BAR_ACCESS;
  byte    CELL_BAR_QUALIFY;
  boolean REESTABLISHMENT_ALLOWED;
  LAI_T   lai;
  boolean control_channel_description_valid;
  control_channel_description_T control_channel_description;
  boolean GPRS_indicator;
  two_quater_position_T two_quater_position;
  rr_si2ter_expected_T  two_ter_indicator;
  byte    access_control[2];
  boolean cell_id_valid;
  uint16  cell_id;
  boolean si13_on_bcch_ext;
} rr_candidate_SI_info_T;

typedef enum
{
  RR_CELL_UNSUITABLE,
  RR_CELL_SUITABLE,
  RR_CELL_SUITABILITY_UNKNOWN
} rr_cell_suitable_type_enum_T;

typedef struct
{
  rr_candidate_SI_info_T           si_info;
  rr_l1_freq_power_T               meas;
  BSIC_T                           bsic;

  rr_cell_suitable_type_enum_T     is_acceptable;

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
  uint16                           fcch_snr;
  rr_cell_suitable_type_enum_T     is_suitable;
  boolean                          fcch_snr_processed;
  boolean                          fcch_snr_rcvd;
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */
  boolean                          bsic_valid;
  rr_cell_type_T                   priority;

  rr_candidate_freq_type_enum_T    type;
  boolean                          processed;
#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
  uint32                           cell_timing;
  uint32                           power_meas_timestamp;
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */
} rr_candidate_cell_T;

typedef struct
{
  rr_candidate_cell_T candidate_cells[RR_MAX_CANDIDATE_FREQ_LIST];
  word                num_frequencies;

  word                frequency_index[RR_MAX_ARFCN];
  word                power_scan_offset;
  rr_l1_band_bitmap_T bands_searched;
  boolean             valid;
  boolean             populated_from_acq_db;
} rr_candidate_cell_db_T;

typedef enum
{
  /* strategies on processing the candidate cell database, which
   * is used in different scenarios, e.g. manual PLMN selection uses a
   * different strategy with automatic PLMN selection.
   */
  RR_CANDIDATE_STRONGEST_FIRST,     /* find the strongest unprocessed cell */
  RR_CANDIDATE_HIGH_PRIORITY_CELLS, /* find only high priority cells */
  RR_CANDIDATE_LOW_PRIORITY_CELLS,   /* find only low priority cells */
  RR_CANDIDATE_HIGH_PRIORITY_CSPS_CELLS,
  RR_CANDIDATE_HIGH_PRIORITY_CS_CELLS,
  RR_CANDIDATE_LOW_PRIORITY_CSPS_CELLS,
  RR_CANDIDATE_LOW_PRIORITY_CS_CELLS
} rr_candidate_process_strategy_T;

typedef struct
{
  PLMN_id_T              PLMN_id;
  rr_l1_band_indicator_T band_indicator;
  rr_plmn_cell_info_T    cell_info[BA_MAX_LEN];
  word                   num_frequencies;
} rr_plmn_info_T;

typedef struct
{
  rr_plmn_info_T plmn_info[SYS_PLMN_LIST_MAX_LENGTH];
  byte           num_plmn;
  boolean        valid;
  rr_plmn_info_T *last_updated_plmn_ptr;
  /* band_pref records the bands which the last PLMN search was performed on */
  sys_band_mask_type band_pref;
} rr_available_plmn_db_T;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern rr_candidate_cell_db_T *rr_get_candidate_cell_db_ptr( );

extern rr_available_plmn_db_T *rr_get_available_plmn_db_ptr( );

extern void rr_candidate_cell_db_init( );

extern void rr_candidate_db_clear(
  rr_candidate_cell_db_T *candidate_db
);

extern void rr_candidate_db_clear_offset(
  rr_candidate_cell_db_T *candidate_db
);

extern boolean rr_candidate_db_is_valid(
  const rr_candidate_cell_db_T * candidate_db
);

extern void rr_candidate_db_set_valid(
  rr_candidate_cell_db_T *candidate_db,
  boolean valid
);

extern uint16 rr_candidate_db_get_power_scan_offset(
  const rr_candidate_cell_db_T * candidate_db
);

extern uint16 rr_candidate_db_get_num_frequencies(
  const rr_candidate_cell_db_T *candidate_db
);

extern uint16 rr_candidate_db_get_num_unprocessed_cell(
  const rr_candidate_cell_db_T *candidate_db
);

extern void rr_candidate_db_add_frequency(
  rr_candidate_cell_db_T *candidate_db,
  const rr_l1_freq_power_T     * candidate
);

extern void rr_candidate_db_mark_all_as_processed(
  rr_candidate_cell_db_T *candidate_db
);

extern boolean rr_candidate_db_is_candidate_cell(
  ARFCN_T BCCH_ARFCN
);

extern rr_candidate_cell_T *rr_candidate_db_store_si_info(
  ARFCN_T                        BCCH_ARFCN,
  const rr_candidate_SI_info_T * si_info
);

extern void rr_candidate_db_set_type(
  rr_candidate_cell_db_T        *candidate_db,
  ARFCN_T                       BCCH_ARFCN,
  rr_candidate_freq_type_enum_T type
);

extern void rr_candidate_db_set_type_from_failure_ind(
  rr_candidate_cell_db_T      *candidate_db,
  ARFCN_T                     BCCH_ARFCN,
  rr_l1_decode_bcch_failure_T failure_reason
);

extern void rr_candidate_db_set_processed(
  rr_candidate_cell_db_T *candidate_db,
  ARFCN_T                BCCH_ARFCN,
  boolean                processed
);

extern void rr_candidate_db_mark_known_entries_as_processed(
  rr_candidate_cell_db_T *candidate_db
);

extern void rr_candidate_db_mark_invalid_entries_as_processed(
  rr_candidate_cell_db_T *candidate_db
);

extern rr_candidate_cell_T* rr_candidate_db_get_entry(
  rr_candidate_cell_db_T *candidate_db,
  ARFCN_T                BCCH_ARFCN
);

extern void rr_candidate_db_set_unknown_entry_types(
  rr_candidate_cell_db_T *candidate_db,
  rr_candidate_freq_type_enum_T new_type
);

#ifdef FEATURE_PS_PREFERRED_CELL
extern boolean rr_candidate_db_force_find_next_ps(
  rr_candidate_cell_db_T  *candidate_db_ptr,
  rr_candidate_cell_T     **entry_ptr_ptr,
  rr_candidate_process_strategy_T strategy
);
#endif /* FEATURE_PS_PREFERRED_CELL */

extern boolean rr_candidate_db_find_next(
  rr_candidate_cell_db_T  *candidate_db,
  rr_candidate_cell_T     **entry,
  rr_candidate_process_strategy_T strategy
);

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
extern boolean rr_candidate_db_fcch_snr_find_next(
  rr_candidate_cell_db_T           *candidate_db_ptr,
  rr_candidate_cell_T             **entry_ptr_ptr,
  rr_candidate_process_strategy_T   strategy
);
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */

extern boolean rr_candidate_db_search_for_BCCH(
  const rr_candidate_cell_db_T *candidate_db
);

extern void rr_candidate_db_mark_valid_entries_as_unprocessed(
  rr_candidate_cell_db_T *candidate_db
);

extern void rr_set_decode_cnf_received(
  boolean cnf_receiveds
);

extern boolean rr_candidate_db_decode_bcch_complete( );

extern boolean rr_candidate_db_get_populated_from_acq_db(
  const rr_candidate_cell_db_T * candidate_db
);

extern void rr_candidate_db_set_populated_from_acq_db(
  rr_candidate_cell_db_T *candidate_db,
  boolean populated_from_acq_db
);

extern boolean rr_check_candidate_suitable(
  rr_candidate_cell_T * candidate_ptr,
  boolean               only_acceptable,
  boolean               automatic_selection
);

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
extern void rr_candidate_db_find_best_of_fcch_snr(
  rr_candidate_cell_db_T *candidate_db_ptr,
  boolean                *best_cell_found,
  uint16                 *best_cell_index
);
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */

rr_event_T rr_perform_bcch_list_decode_from_candidate_db(
  const rr_candidate_cell_db_T * candidate_db_ptr
);

rr_event_T rr_redo_bcch_list_decode_from_candidate_db(
  rr_candidate_cell_db_T  *candidate_db_ptr,
  rr_search_order_e        search_order
);

extern boolean rr_plmn_db_is_valid(
  const rr_available_plmn_db_T * plmn_db_ptr,
  sys_band_mask_type             band_pref
);

extern void rr_plmn_db_set_valid(
  rr_available_plmn_db_T * plmn_db_ptr,
  sys_band_mask_type band_pref
);

extern void rr_plmn_db_clear( 
  rr_available_plmn_db_T * plmn_db_ptr
);

extern void rr_invalidate_available_plmn_database( );

extern void rr_plmn_db_get_plmn_frequency_info(
  rr_available_plmn_db_T * plmn_db_ptr,
  const PLMN_id_T *        PLMN_id_ptr,
  rr_l1_idle_freq_list_T * high_priority_array,
  rr_l1_idle_freq_list_T * low_priority_array
);

extern void rr_plmn_db_update(
  rr_candidate_cell_T *candidate_cell_ptr,
  ARFCN_T channel_number 
);
boolean rr_candidate_db_send_decode_bcch_start_ind(
  rr_search_mode_T bcch_decode_type,
  boolean si3_only,
  sys_band_mask_type filter_band_mask
);

uint16 rr_candidate_db_get_num_unknown_cell(
  const rr_candidate_cell_db_T *candidate_db_ptr
);

extern void rr_candidate_update_FCCH_SCH(
  mph_fcch_sch_decode_ind_T *fcch_sch_ptr
);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
void rr_ms_candidate_db_search_timeout( );
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

extern void rr_candidate_get_bsi_scheduling_info(
  rr_candidate_cell_T *candidate_cell_ptr,
  uint32 *si_on_bcch_ext_ptr,
  uint32 *si_not_broadcast_ptr
);

extern void rr_candidate_cell_db_populate_cell_info_list(
  sys_geran_cell_list_s_type    *detected_frequency_list
);

#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
extern void rr_candidate_cell_db_populate_gps_plmn_search_list(
  sys_loc_wwan_scan_result_s_type    **detected_frequency_list
);
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */

extern boolean rr_candidate_db_found_best_pci_cells(uint8 num_cells_to_search);

#endif /* RR_CANDIDATE_CELL_DB_H */


/* EOF */


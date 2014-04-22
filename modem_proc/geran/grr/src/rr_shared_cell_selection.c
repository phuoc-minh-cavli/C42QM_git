/*! \file rr_shared_cell_selection.c

  This module contains utility functions which are used by cell selection & reselection, such as
  for checking cell suitability for camping.

                Copyright (c) 2001-2014 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_shared_cell_selection.c#5 $ */
/* $DateTime: 2019/03/12 05:52:37 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include <string.h>
#include "rr_seg_load.h"
#include "rr_shared_cell_selection.h"
#include "rr_test.h"
#include "rr_general.h"
#include "rr_l1_send.h"
#include "rr_mm_send.h"
#include "gs.h"
#include "amssassert.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_cell_selection.h"
#include "rr_rrc_if.h"
#include "rr_log.h"
#include "rr_gprs_defs.h"
#include "mm.h"
#include "mm_rr.h"
#include "rr_w2g_service_redirection.h"
#include "rr_plmn_list.h"
#include "rr_multi_sim.h"
#include "geran_eng_mode_info.h"
#include "geran_grr_dyn_api.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Maximum number of penalty timers */
#define RR_MAX_PENALTY_TIMERS 6

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  byte no_of_entries;
  LAI_T lai[MAX_NO_AVAILABLE_LAIS];
} rr_lai_reject_list_t;

typedef struct
{
  /**************************************************************/
  /* saves 6 best surround cells in meas from L1, used for usat */
  /**************************************************************/
  best_six_info_T rr_last_best_six;
  rr_lai_reject_list_t lai_reject_list;
} rr_shared_sel_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_shared_sel_data_t  rr_shared_sel_data;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 *
 * \return rr_shared_sel_data_t*
 */
static rr_shared_sel_data_t *rr_shared_sel_get_data_ptr( void )
{
  return &rr_shared_sel_data;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Updates the five second serving timer when it expires.
 *
 */
void rr_update_serving_five_second_timer(
   
)
{
  rr_timer_id_T      timer_id;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();

  timer_id = scell_info_ptr->gsm.reselection_info.five_second_timer_id;

  /* check for valid timer ID */
  if(timer_id == 0)
  {
    MSG_GERAN_ERROR_0_G("Serving Cell 5 sec timer running but system info has been cleared");
  }
  else
  {
    ASSERT(timer_id == RR_SERV_FIVE_SEC_C1_TIMER);
    /* timer should not be running */
    ASSERT(rr_timer_is_running(timer_id) == FALSE);
    (void) timer_id; /* make lint happy */
  }
} /* rr_update_serving_five_second_timer */

/*!
 * \brief Stops the five second timer for the serving cell.
 *
 */
void rr_stop_serving_five_second_timer(
   
)
{
  /* only generate stopping message if timer is not already stopped */
  if (rr_timer_cancel(RR_SERV_FIVE_SEC_C1_TIMER))
  {
    MSG_GERAN_HIGH_0_G("Stopped serving 5 sec timer");
  }
  /* do not change the serving cell five second timer id */
} /* rr_stop_serving_five_second_timer */

/*!
 * \brief Cancels all the five second timers for the serving and surrounding cells.
 *
 */
void rr_cancel_all_five_second_timers(
   
)
{
  /*******************************************************************************************/
  /* do NOT reset the five second timerId values, they are issued when cells are added to DB */
  /* and should last as long as the cell remains in the database                             */
  /*******************************************************************************************/

  /* cancel all the surrounding timers */
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_1);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_2);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_3);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_4);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_5);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_6);

  /* cancel the serving timer */
  rr_stop_serving_five_second_timer();

  MSG_GERAN_HIGH_0_G("All 5 sec timers stopped");
} /* rr_cancel_all_five_second_timers */

/*!
 * \brief Starts the penalty timer of the given cell to the given PENALTY_TIME value.
 *
 * \param resel_candidate - pointer to the candidate cell
 * \param PENALTY_TIME    - PENALTY TIME value (note if 31 it means timer should not be started)
 */
void rr_start_penalty_timer(
  rr_resel_candidate_t *resel_candidate,
  byte PENALTY_TIME
   
)
{
  rr_timer_id_T penalty_timer_id;

  ASSERT(resel_candidate!= NULL);
  penalty_timer_id = resel_candidate->penalty_timer_id;

  /* cell should have a valid penalty timer Id associated with it */
  if (penalty_timer_id != 0)
  {
    /* if the penalty timer for the cell is stopped and a penalty timer should be started */
    if ((PENALTY_TIME != 31) && (rr_timer_is_active(penalty_timer_id) == FALSE))
    {
      /* start the penalty timer using the provided penalty time value */
      MSG_GERAN_HIGH_3_G("Cell=%d,PTimer Start(%d,%d)",
               (int) resel_candidate->ncell_data->ncell_data.bcch.gsm.BCCH_ARFCN.num,
               (int) penalty_timer_id,
               (int) 20000 * (PENALTY_TIME + 1));
      (void)rr_timer_start(penalty_timer_id,(dword)20000 * (PENALTY_TIME + 1));
    }
  }
  else
  {
    MSG_GERAN_HIGH_3_G("cell %d not in best 6 dont start ptimer",
      resel_candidate->ncell_data->ncell_data.bcch.gsm.BCCH_ARFCN.num,
      0,
      0
    );
  }
  /* otherwise the timer is either running, expired, or should not be started (PENALTY_TIME is 31) */
} /* rr_start_penalty_timer */

/*!
 * \brief Returns the lat best six neighbour cells.
 *
 *
 * \return best_six_info_T*
 */
best_six_info_T* rr_get_last_best_six(
   
)
{
  rr_shared_sel_data_t *rr_shared_sel_data_ptr;

  // Obtain a pointer to the module data
  rr_shared_sel_data_ptr = rr_shared_sel_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_shared_sel_data_ptr);

  return &rr_shared_sel_data_ptr->rr_last_best_six;
}

/*!
 * \brief LAI_T to inter_task_lai_T type conversion function.
 *
 * \param src
 *
 * \return inter_task_lai_T
 */
inter_task_lai_T rr_convert_rr_lai_to_inter_task_lai(
  LAI_T *src
)
{
  inter_task_lai_T dst;

  dst.PLMN_id.identity[0] = src->plmn_id.identity[0];
  dst.PLMN_id.identity[1] = src->plmn_id.identity[1];
  dst.PLMN_id.identity[2] = src->plmn_id.identity[2];
  dst.location_area_code[0] = (src->location_area_code >> 8) & 0xff;
  dst.location_area_code[1] = src->location_area_code & 0xff;

  return dst;
}

/*!
 * \brief Checks if the given LAI is in the LAI Reject List.
 *
 * \param lai_ptr
 *
 * \return boolean
 */
boolean rr_check_lai_in_reject_list(
  LAI_T *lai_ptr
   
)
{
  int i;
  uint8 no_of_entries;
  rr_shared_sel_data_t *rr_shared_sel_data_ptr;

  //RR_NULL_CHECK_RETURN_PARAM(lai_ptr, FALSE);

  // Obtain a pointer to the module data
  rr_shared_sel_data_ptr = rr_shared_sel_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_shared_sel_data_ptr);

  no_of_entries = rr_shared_sel_data_ptr->lai_reject_list.no_of_entries;

  for (i = 0;
       (i < no_of_entries) && (i < MAX_NO_AVAILABLE_LAIS);
       i++)
  {
    LAI_T *entry_ptr = &rr_shared_sel_data_ptr->lai_reject_list.lai[i];

    if ((entry_ptr->plmn_id.identity[0] == lai_ptr->plmn_id.identity[0]) &&
        (entry_ptr->plmn_id.identity[1] == lai_ptr->plmn_id.identity[1]) &&
        (entry_ptr->plmn_id.identity[2] == lai_ptr->plmn_id.identity[2]) &&
        (entry_ptr->location_area_code == lai_ptr->location_area_code))
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*!
 * \brief Stores the LAI Reject List.
 *
 * \param lai_reject_list_ind_ptr - Message containing the new LAI Reject List
 */
void rr_store_lai_reject_list(
  rr_lai_reject_list_ind_T *lai_reject_list_ind_ptr
   
)
{
  int i;
  uint8 no_of_entries;
  rr_shared_sel_data_t *rr_shared_sel_data_ptr;

  //RR_NULL_CHECK_RETURN_VOID(lai_reject_list_ind_ptr);

  // Obtain a pointer to the module data
  rr_shared_sel_data_ptr = rr_shared_sel_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_shared_sel_data_ptr);

  no_of_entries = (uint8) lai_reject_list_ind_ptr->nas_lai_reject_list.no_of_entries;

  for (i = 0;
       (i < no_of_entries) && (i < MAX_NO_AVAILABLE_LAIS);
       i++)
  {
    inter_task_lai_T *src_lai_ptr =
      &lai_reject_list_ind_ptr->nas_lai_reject_list.location_area_identification[i];

    LAI_T *dst_lai_ptr =
      &rr_shared_sel_data_ptr->lai_reject_list.lai[i];

    dst_lai_ptr->plmn_id.identity[0] = src_lai_ptr->PLMN_id.identity[0];
    dst_lai_ptr->plmn_id.identity[1] = src_lai_ptr->PLMN_id.identity[1];
    dst_lai_ptr->plmn_id.identity[2] = src_lai_ptr->PLMN_id.identity[2];

    dst_lai_ptr->location_area_code =
      (word) ((src_lai_ptr->location_area_code[0] * 256) + (src_lai_ptr->location_area_code[1]));
  }

  // reset the rest of the entries in the list to 0 (for unit test consistency)
  for (; i < MAX_NO_AVAILABLE_LAIS; i++)
  {
    LAI_T *dst_lai_ptr =
      &rr_shared_sel_data_ptr->lai_reject_list.lai[i];

    dst_lai_ptr->plmn_id.identity[0] = 0;
    dst_lai_ptr->plmn_id.identity[1] = 0;
    dst_lai_ptr->plmn_id.identity[2] = 0;
    dst_lai_ptr->location_area_code = 0;
  }

  rr_shared_sel_data_ptr->lai_reject_list.no_of_entries = no_of_entries;

} /* rr_store_lai_reject_list */

#ifdef FEATURE_WCDMA
/*!
 * \brief Checks the system information of a cell to determine if it is suitable for W2G redirection.
 *
 * \param cell_selection_parameters         - cell selection parameter information
 * \param RACH_control_parameters           - RACH control parameter information
 * \param location_area_identification      - location area identification for cell
 * \param rxlev                             - RXLEV value for cell
 * \param BCCH_ARFCN                        - ARCFCN for cell
 * \param bsic_ptr                          - BSIC for cell
 * \param check_acceptable                  - if TRUE checks if cell is acceptable instead of suitable,
 *                                            otherwise checks for suitability
 * \param automatic_plmn_selection_criteria - if TRUE, automatic PLMN selection criteria should be applied to
 *                                            determine if cell is suitable, otherwise manual PLMN selection
 *                                            criteria should be used
 * \param gprs_indicator                    - GPRS indicator for the cell
 *
 * \return boolean - TRUE if cell is suitable, FALSE otherwise
 */
boolean rr_check_cell_suitable_w2g_redirection(
  cell_selection_parameters_T *cell_selection_parameters,
  RACH_control_parameters_T   *RACH_control_parameters,
  LAI_T                       *location_area_identification,
  byte                        rxlev,
  ARFCN_T                     BCCH_ARFCN,
  BSIC_T                      *bsic_ptr,
  boolean                     check_acceptable,
  boolean                     automatic_plmn_selection_criteria,
  boolean                     gprs_indicator
)
{
  boolean         plmn_and_lai_ok = TRUE;
  boolean         band_ok = TRUE;
  boolean         auth_failed_cell_barred = FALSE;
  boolean         registration_not_possible = FALSE;
  access_result_T access_result;

  rr_cell_type_T cell_type = rr_get_cell_priority(
    cell_selection_parameters->CELL_BAR_QUALIFY,    // CELL_BAR_QUALIFY
    RACH_control_parameters->CELL_BAR_ACCESS,       // CELL_BAR_ACCESS
    location_area_identification,                   // location_area_identification
    RACH_control_parameters->access_control,        // access_control
    NULL                                           // cell_test_operation_ptr
  );

  int C1 = rr_get_C1(
    BCCH_ARFCN,                                     // arfcn
    rxlev,                                          // RXLEV_average
    cell_selection_parameters->RXLEV_ACCESS_MIN,    // RXLEV_ACCESS_MIN
    cell_selection_parameters->MS_TXPWR_MAX_CCH,    // pwoer_control_level
    cell_selection_parameters->POWER_OFFSET        // POWER_OFFSET
  );

  /*****************************************/
  /* Check PLMN and forbidden LAI CR22284  */
  /* only if not requesting for acceptable */
  /*****************************************/
  if (!check_acceptable)
  {
    plmn_and_lai_ok = rr_w2g_redirection_check_plmn_lai(
      BCCH_ARFCN,                     // arfcn
      location_area_identification   // lai_ptr
    );
  } /* check for suitable not acceptable */

  if (!rr_gapi_is_supported_band_dyn(
         BCCH_ARFCN.band    // band
       ))
  {
    band_ok = FALSE;
  }

  if (rr_is_cell_in_barred_cell_list(
        BCCH_ARFCN,   // arfcn
        *bsic_ptr    // bsic
      ))
  {
    auth_failed_cell_barred = TRUE;
  }

  /* Also need to check whether the access control allows RACHing */
  if (rr_w2g_oos_search_in_progress())
  {
    access_result = rr_check_access_class_for_w2g_oos_search(
      location_area_identification->plmn_id,         // plmn_id
      RACH_control_parameters->access_control,       // access_control
      RACH_control_parameters->CELL_BAR_ACCESS,      // cell_bar_access
      cell_selection_parameters->CELL_BAR_QUALIFY   // cell_bar_qualify
    );

    if (access_result != NORMAL_CALLS_ALLOWED &&
        access_result != ALL_CALLS_ALLOWED)
    {
      registration_not_possible = TRUE;
    }
  }

  if (cell_type == RR_CELL_BARRED ||
      (C1 <= 0 && !rr_allow_negative_c1(location_area_identification->plmn_id)) ||
      (! plmn_and_lai_ok) ||
      (! band_ok) ||
      auth_failed_cell_barred
      || registration_not_possible
     )
  {
    MSG_GERAN_HIGH_2_G("Cell_type: %d, C1: %d",cell_type,C1);
    MSG_GERAN_HIGH_3_G("PLMN_LAI_ok: %d, band_ok: %d, auth_failed_cell_barred: %d",
             plmn_and_lai_ok,
             band_ok,
             auth_failed_cell_barred);
    MSG_GERAN_HIGH_3_G("reg_not_possible: %d",
             registration_not_possible,
             0,
             0);
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* rr_check_cell_suitable_w2g_redirection */
#endif /* FEATURE_WCDMA */

/*!
 * \brief Checks the system information of a cell to determine if it is suitable.
 *
 * \param cell_selection_parameters         - cell selection parameter information
 * \param RACH_control_parameters           - RACH control parameter information
 * \param location_area_identification      - location area identification for cell
 * \param rxlev                             - RXLEV value for cell
 * \param BCCH_ARFCN                        - ARCFCN for cell
 * \param bsic_ptr                          - BSIC for cell
 * \param check_acceptable                  - if TRUE checks if cell is acceptable instead of suitable,
 *                                            otherwise checks for suitability
 * \param automatic_plmn_selection_criteria - if TRUE, automatic PLMN selection criteria should be applied to
 *                                            determine if cell is suitable, otherwise manual PLMN selection
 *                                            criteria should be used
 * \param check_plmn_match                  - set to TRUE to check the PLMN of the cell against that
 *                                            requested
 *                                            Note: This check is not done if check_acceptable is set TRUE
 *
 * \return boolean - TRUE if cell is suitable, FALSE otherwise
 */
boolean rr_check_cell_suitable(
  cell_selection_parameters_T *cell_selection_parameters,
  RACH_control_parameters_T   *RACH_control_parameters,
  LAI_T                       *location_area_identification,
  byte                        rxlev,
  ARFCN_T                     BCCH_ARFCN,
  BSIC_T                      *bsic_ptr,
  boolean                     check_acceptable,
  boolean                     automatic_plmn_selection_criteria,
  boolean                     check_plmn_match,
  boolean                     gprs_indicator
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr();
  boolean                    auth_failed_cell_barred     = FALSE;
  boolean                    forbidden_lai               = FALSE;
  boolean                    PLMN_ok                     = TRUE;
  boolean                    emergency_call_allowed      = TRUE;
  boolean                    band_ok;
  eng_mode_plmn_id_t         eng_plmn;

  rr_cell_type_T cell_type = rr_get_cell_priority(
    cell_selection_parameters->CELL_BAR_QUALIFY,    // CELL_BAR_QUALIFY
    RACH_control_parameters->CELL_BAR_ACCESS,       // CELL_BAR_ACCESS
    location_area_identification,                   // location_area_identification
    RACH_control_parameters->access_control,        // access_control
    NULL                                           // cell_test_operation_ptr
  );

  int C1 = rr_get_C1(
    BCCH_ARFCN,                                     // arfcn
    rxlev,                                          // RXLEV_average
    cell_selection_parameters->RXLEV_ACCESS_MIN,    // RXLEV_ACCESS_MIN
    cell_selection_parameters->MS_TXPWR_MAX_CCH,    // pwoer_control_level
    cell_selection_parameters->POWER_OFFSET        // POWER_OFFSET
  );

  //RR_NULL_CHECK_RETURN_PARAM(location_area_identification,FALSE);

  /* publish each PLMN ID to the engineering mode interface */
  eng_plmn.plmn_id[0] = location_area_identification->plmn_id.identity[0];
  eng_plmn.plmn_id[1] = location_area_identification->plmn_id.identity[1];
  eng_plmn.plmn_id[2] = location_area_identification->plmn_id.identity[2];
  geran_eng_mode_data_write(ENG_MODE_CANDIDATE_PLMN_ID, &eng_plmn, TRUE);

  /*****************************************/
  /* Check PLMN and forbidden LAI CR22284  */
  /* only if not requesting for acceptable */
  /*****************************************/
  if (!check_acceptable)
  {
    if (check_plmn_match)
    {
      PLMN_ok = rr_cell_selection_PLMN_match(
        rr_PLMN_request_details_ptr,    // request_details_ptr
        location_area_identification   // received_LAI_ptr
      );
    }

    /* only check forbidden LAI if automatic_plmn_selection_criteria is being used */
    if (automatic_plmn_selection_criteria)
    {
      forbidden_lai = rr_is_forbidden_LAI(
        location_area_identification   // lai_ptr
      );
    }
    else if (rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
    {
      forbidden_lai = rr_check_lai_in_reject_list(
        location_area_identification   // lai_ptr
      );
    }
  } /* check for suitable not acceptable */

  /* check whether the BCCH band is supported */
  band_ok = rr_gapi_is_supported_band_dyn(
    BCCH_ARFCN.band    // band
  );

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  if (band_ok == FALSE)
  {
    /* if a W2G BCCH decode is in effect, the band may not actually be listed */
    /* as supported, but RRC may have updated GRR's "preferred" bands instead */
    if (rr_plc_wtog_decoding_bcch())
    {
      band_ok = rr_is_preferred_band(
        (rr_internal_band_T)(BCCH_ARFCN.band)    // band
      );
      MSG_GERAN_HIGH_1_G("W2G BPLMN search in progress, band_ok = %d", (int)band_ok);
    }
  }
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

  if (rr_is_cell_in_barred_cell_list(
        BCCH_ARFCN,   // arfcn
        *bsic_ptr    // bsic
      ))
  {
    auth_failed_cell_barred = TRUE;
  }

  /* 43.022 3.2.2 & 3.7 
     3.7 No suitable cell (limited service state)
     If the MS is unable to obtain normal service from a PLMN (see 3GPP TS 23.122), the MS attempts to camp on an
     acceptable cell, irrespective of its PLMN identity, so that emergency calls can be made or warning notifications can be
     received if necessary */

  /* Check for access_class when network selection mode is LIMITED & No MPLMN search going on*/
  if (rr_is_limited_plmn_selection_mode() && (!rr_background_plmn_search_in_progress()))
  {
    access_result_T  access_result;

    access_result = rr_check_access_class(
                      MM_RR_EMERGENCY_CALL,
                      location_area_identification->plmn_id,          // location_area_identification
                      RACH_control_parameters->access_control,        // access_control
                      RACH_control_parameters->CELL_BAR_ACCESS,       // CELL_BAR_ACCESS
                      cell_selection_parameters->CELL_BAR_QUALIFY    // CELL_BAR_QUALIFY
                    );

    if (access_result == EMERGENCY_CALLS_ALLOWED || access_result == ALL_CALLS_ALLOWED)
    {
      emergency_call_allowed = TRUE;
    }
    else
    {
      emergency_call_allowed = FALSE;
    }
  }

  if ((cell_type != RR_CELL_BARRED) && (C1 > 0 || rr_allow_negative_c1(location_area_identification->plmn_id)))
  {
    /*set the ue_oos flag FALSE if some service (FULL or LIMITED) is found */
    rr_cell_selection_set_ue_oos_status(
      FALSE    // status
    );
  }

  if (cell_type == RR_CELL_BARRED ||
      (C1 <= 0 && !rr_allow_negative_c1(location_area_identification->plmn_id)) ||
      forbidden_lai ||
      (! PLMN_ok) ||
      (! band_ok) ||
      auth_failed_cell_barred ||
#ifndef FEATURE_GERAN_CS_SUPPORT
      gprs_indicator == FALSE ||
#endif /* !FEATURE_GERAN_CS_SUPPORT */
      (! emergency_call_allowed) )
  {
    MSG_GERAN_HIGH_8_G("Cell_type: %d, C1: %d, forbidden_lai: %d, emergency_call_allowed: %d, PLMN_ok: %d, band_ok: %d, auth_failed_cell_barred: %d, gprs_indicator: %d",
      cell_type, C1, forbidden_lai, emergency_call_allowed,
      PLMN_ok, band_ok, auth_failed_cell_barred, gprs_indicator);

    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* rr_check_cell_suitable */


/**
 * Resets the surrounding database
 */
/*!
 * \brief Resets the surrounding database.
 *
 */
void rr_reset_surrounding_database(
   
)
{
  rr_shared_sel_data_t *rr_shared_sel_data_ptr;

  // Obtain a pointer to the module data
  rr_shared_sel_data_ptr = rr_shared_sel_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_shared_sel_data_ptr);

  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_1);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_2);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_3);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_4);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_5);
  (void)rr_timer_cancel(RR_FIVE_SEC_C2_TIMER_6);

  rr_shared_sel_data_ptr->rr_last_best_six.no_of_entries = 0;
} // rr_reset_surrounding_database

/*!
 * \brief Returns TRUE if the requested PLMN matches the given PLMN for cell selection purposes.
 *
 * \param req_details_ptr  - pointer to request details structure
 * \param received_LAI_ptr - pointer to received LAI value
 *
 * \return boolean - TRUE if the PLMNs should be considered a match, FALSE otherwise
 */
boolean rr_cell_selection_PLMN_match(
  rr_PLMN_request_details_T *req_details_ptr,
  LAI_T *received_LAI_ptr
   
)
{
  boolean                    plmn_ok = FALSE;
  gprs_scell_info_t         *scell_info_ptr = rr_gprs_get_scell_info();
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr();

  ASSERT(req_details_ptr != NULL);
  ASSERT(received_LAI_ptr != NULL);

  // use the sys_plmn_match function to perform comparison
  // CR21042 order of parameters to function is important (requested,received)

  // In case of Limited routine Acquisition, compare with Serving Cell PLMN,  since requested might
  // be Any.
  if ( rr_PLMN_request_details_ptr->network_select_mode ==
         SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION )
  {
    if ( sys_plmn_match(
           scell_info_ptr->gsm.location_area_identification.plmn_id,
           rr_lai_get_PLMN(*received_LAI_ptr) ) )
    {
      MSG_GERAN_MED_0_G("PLMN matched against SI3 PLMN id (LIMITED_ROUTINE_ACQUISITION)");
      plmn_ok = TRUE;
    }
  }
  else
  {
    if ( sys_plmn_match(
           req_details_ptr->requested_PLMN_id,
           rr_lai_get_PLMN(*received_LAI_ptr) ) )
    {
      MSG_GERAN_MED_0_G("PLMN matched against requested_PLMN_id");
      plmn_ok = TRUE;
    }
  }

  /**
   * There need to be 2 calls to check the equivalent PLMN in RR here
   * (one for the requested PLMN and another for the serving PLMN) and
   * both need to return TRUE in order for the PLMNs to be considered
   * equivalent.
   */
  if (!plmn_ok)
  {
    if (rr_eplmn_list_equivalent_plmn(rr_lai_get_PLMN(*received_LAI_ptr)) &&
        rr_eplmn_list_equivalent_plmn(req_details_ptr->requested_PLMN_id))
    {
      MSG_GERAN_MED_0_G("Requested PLMN id and SI3 PLMN id present in REG EPLMN list");
      plmn_ok = TRUE;

      rr_set_equivalent_plmn(&received_LAI_ptr->plmn_id);
    }
  }

  /*
  For the case where the EPLMN list in REG has been deleted, RR keeps a copy of the
  PLMN id of the serving cell (equivalent_PLMN_id) if it was originally found to be
  equivalent to the originally requested PLMN id (requested_PLMN_id).
  Check if RR's equivalent PLMN id matches that currently being broadcast in SI3
  */
  if (! plmn_ok)
  {
    if ( sys_plmn_match( req_details_ptr->equivalent_PLMN_id,
                         rr_lai_get_PLMN(*received_LAI_ptr) ) )
    {
      MSG_GERAN_MED_0_G("RR EPLMN id matched against SI3 PLMN id");
      plmn_ok = TRUE;
    }
  }

  /**
   * FR20971: 
   * If requested PLMN id is not same as SI3 id and 
   * network selection mode is automatic and is_ehplmn_to_be_camped is SET
   * then check that the received SI3 PLMN id in the EHPLMN list,	 
   * if it returns TRUE in order for the PLMN to be considered EHPLMN.
   */
  if ( !plmn_ok && 
       (rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC) && 
       (rr_PLMN_request_details_ptr->is_ehplmn_to_be_camped == TRUE) )
  {
    if (rr_is_ehplmn_or_hplmn(rr_lai_get_PLMN(*received_LAI_ptr)))
    {
      MSG_GERAN_HIGH_0_G("SI3 PLMN id present in EHPLMN list or same as HPLMN");
      plmn_ok = TRUE;
    }
  }

  /**
   * FR20971 extended for W2G redir: 
   * If requested PLMN id is not same as SI3 id and 
   * PLMN search reason is RR_W2G_SERVICE_REDIRECTION and is_ehplmn_to_be_camped is SET
   * then check that the received SI3 PLMN id in the EHPLMN list,	 
   * if it returns TRUE in order for the PLMN to be considered for EHPLMN checking.
   */
#ifdef FEATURE_WCDMA
  if ( !plmn_ok &&
       (rr_w2g_redirection_in_progress()) &&  
       (rr_PLMN_request_details_ptr->search_reason == RR_W2G_SERVICE_REDIRECTION) && 
       rr_w2g_redirection_is_ehplmn_search_allowed() )
  {
    if (rr_is_ehplmn_or_hplmn(rr_lai_get_PLMN(*received_LAI_ptr)))
    {
      MSG_GERAN_HIGH_0_G("(W2G redir req) SI3 PLMN id present in EHPLMN list or same as HPLMN");
      plmn_ok = TRUE;
    }
  }
#endif /* FEATURE_WCDMA */

  return plmn_ok;
} /* rr_cell_selection_PLMN_match */

/*!
 * \brief Clears the reselection_info_T structure.
 *
 * \param reselection_info_ptr
 */
void rr_clear_reselection_info(
  reselection_info_T *reselection_info_ptr
)
{
  ASSERT(reselection_info_ptr != NULL);

  reselection_info_ptr->RXLEV_average = 0;
  reselection_info_ptr->C1 = 0;
  reselection_info_ptr->C2 = 0;
  reselection_info_ptr->measurements_valid = FALSE;
  reselection_info_ptr->five_second_timer_id = RR_TIMER_UNDEFINED;
} /* rr_clear_reselection_info */

/*!
 * \brief This function checks if the cell is barred for Access or C1 <=0.
 *
 * \param cell_selection_parameters    - cell selection parameter information
 * \param RACH_control_parameters      - RACH control parameter information
 * \param location_area_identification - location area identification for cell
 * \param rxlev                        - RXLEV value for cell
 * \param BCCH_ARFCN                   - ARCFCN for cell
 *
 * \return boolean
 */
boolean rr_cell_barred_or_c1_unsuitable(
  cell_selection_parameters_T *cell_selection_parameters,
  RACH_control_parameters_T   *RACH_control_parameters,
  LAI_T                       *location_area_identification,
  byte                        rxlev,
  ARFCN_T                     BCCH_ARFCN,
  BSIC_T                      *bsic_ptr
)
{
  rr_cell_type_T cell_type = rr_get_cell_priority(
    cell_selection_parameters->CELL_BAR_QUALIFY,    // CELL_BAR_QUALIFY
    RACH_control_parameters->CELL_BAR_ACCESS,       // CELL_BAR_ACCESS
    location_area_identification,                   // location_area_identification
    RACH_control_parameters->access_control,        // access_control
    NULL                                           // cell_test_operation_ptr
  );

  int C1 = rr_get_C1(
    BCCH_ARFCN,                                     // arfcn
    rxlev,                                          // RXLEV_average
    cell_selection_parameters->RXLEV_ACCESS_MIN,    // RXLEV_ACCESS_MIN
    cell_selection_parameters->MS_TXPWR_MAX_CCH,    // pwoer_control_level
    cell_selection_parameters->POWER_OFFSET        // POWER_OFFSET
  );

  if (cell_type == RR_CELL_BARRED || (C1 <= 0 && !rr_allow_negative_c1(location_area_identification->plmn_id)))
  {
    return TRUE;
  }
  else if (rr_is_cell_in_barred_cell_list(BCCH_ARFCN,*bsic_ptr ) ==  TRUE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} // rr_check_cell_for_selection

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 */
void rr_shared_sel_task_start_init( void )
{
  // Zero the control/data structure(s)
  memset(&rr_shared_sel_data, 0, sizeof(rr_shared_sel_data));

  // Setup any specific values not setup by the general module initialisation function

} /* end rr_shared_sel_task_start_init() */

/* EOF */


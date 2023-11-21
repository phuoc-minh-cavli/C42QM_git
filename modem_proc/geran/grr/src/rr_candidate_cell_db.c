/*! \file rr_candidate_cell_db.c

  This module provides functions for manipulating the RR candidate cell database.

                Copyright (c) 2002-2014 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_candidate_cell_db.c#21 $
*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include <stdlib.h>
#include "rr_seg_load.h"
#include "rr_candidate_cell_db.h"
#include "rr_gprs_defs.h"
#include "rr_general.h"
#include "rr_l1_send.h"
#include "rr_pscan.h"
#include "rr_shared_cell_selection.h"
#include "rr_w2g_service_redirection.h"
#include "gprs_mem.h"
#include "rr_decode_bcch.h"
#include "rr_multi_sim.h"
#include "rr_sys_info_ncell.h"
#include "rr_nv.h"
#include "rr_acq_db.h"
#include "rr_static_general.h"
#include "rr_plmn_list.h"
#include "sys.h"
#include "modem_mem.h"
#include "ref_cnt_obj.h"
#include "geran_dmm.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
/* SNR(dB) = 10*log(SNR(linear)/256)
 * 10 dB = 2560 */
#define FCCH_SNR_THRESHOLD_1 2560 /* 10 dB */

/* Number of cells that need to be compared if strongest cell in not above threshold_1. */
#define FCCH_SNR_MAX_CELLS_TO_COMPARE 3
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef enum
{
  RR_DECODE_BCCH_INACTIVE  = 0,
  RR_DECODE_BCCH_IN_PROGRESS,
  RR_DECODE_BCCH_ABORT
} rr_decode_bcch_state_e;

typedef struct
{
  rr_decode_bcch_state_e    state;
  rr_decode_bcch_state_e    old_state;
  rr_search_mode_T          bcch_decode_type;
  sys_band_mask_type        band_pref;
  boolean                   decode_cnf_received;
  boolean                   si3_only;
} rr_decode_bcch_data_T;

typedef struct
{
  int filtered_bs_count; /* number of channels filtered by BST */
  int filtered_sc_count; /* number of channels filtered due to match with serving cell (should only see maximum of 1 here) */
  int filtered_ba_count; /* number of channels filtered due to match in BA list */
  int filtered_ca_count; /* number of channels filtered due to match in Cell Allocation */
} rr_decode_bcch_stats_T;

typedef struct
{
  sys_network_selection_mode_e_type  scan_mode;
  inter_task_BCCH_allocation_T      *ba_ptr;
  cell_channel_description_T        *ca_ptr;
  rr_search_mode_T                   decode_type;
  sys_band_mask_type                 band_mask;
  ARFCN_T                            scell_arfcn;
  boolean                            camped;
  rr_decode_bcch_stats_T             arfcn_stats;
} rr_decode_bcch_filter_T;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static rr_candidate_cell_db_T   rr_candidate_cell_db;
static rr_available_plmn_db_T   rr_available_plmn_db;
static rr_decode_bcch_data_T  rr_decode_bcch_data;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param 
 *
 * \return rr_decode_bcch_data_T*
 */
static rr_decode_bcch_data_T *rr_decode_bcch_get_data_ptr(void )
{
  return &rr_decode_bcch_data;
}

/*!
 * \brief Returns a pointer to the Available PLMN DB.
 *
 * \param 
 *
 * \return rr_available_plmn_db_T*
 */
static rr_available_plmn_db_T *rr_available_plmn_db_get_ptr(void )
{
  return &rr_available_plmn_db;
}

/*!
 * \brief Returns a pointer to the Candidate Cell DB.
 *
 * \param 
 *
 * \return rr_candidate_cell_db_T*
 */
static rr_candidate_cell_db_T *rr_candidate_cell_db_get_ptr(void )
{
  return &rr_candidate_cell_db;
}

/**
 * Returns a unique value to be used as an index in to the lookup table
 * 'frequency_index' to optimise searching of the candidate cell database
 * for a specific ARFCN.
 *
 * This is basically a mapping of the 971 unique ARFCNs to unique indexes
 * within the range 0 - 1023.  To achieve this, E/PGSM ARFCNs remain
 * unchanged, GSM850 and DCS1800 ARFCNs are shifted down by a fixed amount
 * and PCS1900 are shifted up by a fixed amount.
 *
 * This results in the following mappings:
 *
 *                        Channel Number     Mapped Value   Offset
 *   GSM900 lower range   0 - 124            0 - 124        0
 *   GSM850               128 - 251          125 - 248      -3
 *   DCS1800              512 - 885          249 - 622      -263
 *   PCS1900              512 - 810          623 - 921      +111
 *   Unused                                  922 - 974
 *   GSM900 upper range   975 - 1023         975 - 1023     0
 *
 * The valid return values for this function are 0 - 1023.  If the ARFCN
 * is invalid and cannot be hashed, a value of RR_MAX_ARFCN will be returned.
 */
static uint16 rr_candidate_db_hash_arfcn(ARFCN_T arfcn)
{
  #define RR_CANDIDATE_DB_ARFCN_HASH_GSM_900_OFFSET  (0)
  #define RR_CANDIDATE_DB_ARFCN_HASH_DCS_1800_OFFSET (-263)
  #define RR_CANDIDATE_DB_ARFCN_HASH_GSM_850_OFFSET  (-3)
  #define RR_CANDIDATE_DB_ARFCN_HASH_PCS_1900_OFFSET (+111)

  uint16 hashed_value = RR_MAX_ARFCN;

  switch (arfcn.band)
  {
    case SYS_BAND_PGSM_900:
    case SYS_BAND_EGSM_900:
    {
      hashed_value = (uint16) (arfcn.num + RR_CANDIDATE_DB_ARFCN_HASH_GSM_900_OFFSET);
    }
    break;

    case SYS_BAND_DCS_1800:
    {
      hashed_value = (uint16) (arfcn.num + RR_CANDIDATE_DB_ARFCN_HASH_DCS_1800_OFFSET);
    }
    break;

    case SYS_BAND_PCS_1900:
    {
      hashed_value = (uint16) (arfcn.num + RR_CANDIDATE_DB_ARFCN_HASH_PCS_1900_OFFSET);
    }
    break;

    case SYS_BAND_CELL_850:
    {
      hashed_value = (uint16) (arfcn.num + RR_CANDIDATE_DB_ARFCN_HASH_GSM_850_OFFSET);
    }
    break;

    default:
      MSG_GERAN_ERROR_2("ARFCN(%d,%d) band is unexpected value", arfcn.num, arfcn.band);
  }

  if (hashed_value >= RR_MAX_ARFCN)
  {
    MSG_GERAN_ERROR_1("hashed value %d is out of range", hashed_value);
    hashed_value = RR_MAX_ARFCN;
  }

  return hashed_value;
}
#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
/**
 *
 * @param void pointer
 *
 * @return 
 */
 static void rr_sys_loc_wwan_scan_result_destructor(void *me)
{
  sys_loc_wwan_scan_result_s_type *sys_loc_wwan_scan_result = (sys_loc_wwan_scan_result_s_type *)me;
  
  modem_mem_free(sys_loc_wwan_scan_result, MODEM_MEM_CLIENT_GERAN);
  sys_loc_wwan_scan_result = NULL;
}
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */

/**
 *
 * @param candidate_db_ptr pointer to candidate database
 *
 * @return number of entries with type as unknown
 */
uint16 rr_candidate_db_get_num_unknown_cell(
  const rr_candidate_cell_db_T *candidate_db_ptr
)
{
  uint16 counter = 0;
  uint16 index;

  for (index = 0; index < candidate_db_ptr->num_frequencies; ++index)
  {
    if (candidate_db_ptr->candidate_cells[index].type == RR_CANDIDATE_UNKNOWN)
    {
      counter++;
    }
  }

  return counter;
} /* rr_candidate_db_get_num_unknown_cell */

boolean rr_check_cell_same_priority(rr_cell_type_T priority, rr_cell_type_T candidate_priority)
{
  boolean check = FALSE;

  if (priority == candidate_priority)
  {
    return TRUE;
  }

  switch (priority)
  {
    case RR_HIGH_PRIORITY_CELL:
    {
      if (candidate_priority == RR_HIGH_PRIORITY_CELL ||
          candidate_priority == RR_HIGH_PRIORITY_CS_CELL ||
          candidate_priority == RR_HIGH_PRIORITY_CSPS_CELL)
      {
        check = TRUE;
      }
      break;
    }

    case RR_LOW_PRIORITY_CELL:
    {
      if (candidate_priority == RR_LOW_PRIORITY_CELL ||
          candidate_priority == RR_LOW_PRIORITY_CS_CELL ||
          candidate_priority == RR_LOW_PRIORITY_CSPS_CELL)
      {
        check = TRUE;
      }
      break;
    }

    default:
    break;
  }

  return check;
}

/**
 * Finds the strongest unprocessed entry of the given priority in the database
 *
 * @param candidate_db_ptr pointer to candidate cell database
 *
 * @param priority         priority cell must be in order to be considered
 *
 * @param entry_ptr        [out] set to point to the entry that is found
 *
 * @return TRUE if a cell was found, FALSE otherwise
 */
static boolean rr_candidate_db_find_strongest_of_priority(
  rr_candidate_cell_db_T  *candidate_db_ptr,
  rr_cell_type_T          priority,
  rr_candidate_cell_T     **entry_ptr_ptr
)
{
  int counter = 0;

  if ((candidate_db_ptr == NULL) || (entry_ptr_ptr == NULL))
  {
    return FALSE;
  }

  for (counter = 0; counter < candidate_db_ptr->num_frequencies; counter++)
  {
    if (!candidate_db_ptr->candidate_cells[counter].processed)
    {
      /******************************************************************************/
      /* NOTE: priority can only be known for cells that have received a valid BCCH */
      /* so if it is not this type just return the cell                             */
      /******************************************************************************/
      if (candidate_db_ptr->candidate_cells[counter].type != RR_CANDIDATE_VALID_BCCH ||
          rr_check_cell_same_priority(priority, candidate_db_ptr->candidate_cells[counter].priority))
      {
        *entry_ptr_ptr = &candidate_db_ptr->candidate_cells[counter];
        return TRUE;
      }
    }
  }

  return FALSE;
} /* rr_candidate_db_find_strongest_of_priority */

/**
 * Finds the strongest unprocessed entry in the database
 *
 * @param candidate_db_ptr pointer to candidate cell database
 *
 * @param entry_ptr        [out] set to point to the entry that is found
 *
 * @return TRUE if a cell was found, FALSE otherwise
 */
static boolean rr_candidate_db_find_strongest(
  rr_candidate_cell_db_T * candidate_db_ptr,
  rr_candidate_cell_T **   entry_ptr_ptr
)
{
  int counter = 0;

  if ((candidate_db_ptr == NULL) || (entry_ptr_ptr == NULL))
  {
    return FALSE;
  }

  for (counter =0 ; (counter < candidate_db_ptr->num_frequencies) && (counter < RR_MAX_CANDIDATE_FREQ_LIST); counter++)
  {
    if (!candidate_db_ptr->candidate_cells[counter].processed)
    {
      /* Return the first non processed cell */
      if ( (candidate_db_ptr->candidate_cells[counter].type != RR_CANDIDATE_VALID_BCCH) ||
           (candidate_db_ptr->candidate_cells[counter].priority != RR_CELL_BARRED)
         )
      {
        *entry_ptr_ptr = &candidate_db_ptr->candidate_cells[counter];
        return TRUE;
      }
    }
  }

  return FALSE;
} /* rr_candidate_db_find_strongest */

/**
 * Preloads a filter structure for use by the candidate cell decode operarion
  *
 * @param filter_ptr 
 *
 * @return VOID
 */
static void rr_decode_bcch_load_filter_info(
  rr_decode_bcch_filter_T *filter_ptr
)
{
  if (filter_ptr != NULL)
  {
    rr_PLMN_list_req_info_T *plmn_list_info_ptr = rr_get_plmn_list_req_info_ptr();
    gprs_scell_info_t       *scell_ptr          = rr_gprs_get_scell_info();

    if (plmn_list_info_ptr != NULL)
    {
      filter_ptr->scan_mode = plmn_list_info_ptr->network_selection_mode;
    }
    else
    {
      /* set default to indicate mode info is not available */
      filter_ptr->scan_mode = SYS_NETWORK_SELECTION_MODE_NONE;
    }

    if (scell_ptr != NULL)
    {
      filter_ptr->scell_arfcn = scell_ptr->gsm.BCCH_ARFCN;
      filter_ptr->ba_ptr      = rr_gprs_get_ba_gprs();
      filter_ptr->ca_ptr      = &(scell_ptr->gsm.SI_cell_channel_description);
      filter_ptr->camped      = scell_ptr->gsm.camped_on_flag;
    }
    else
    {
      /* set defaults to indicate no serving cell is available */
      filter_ptr->scell_arfcn.num  = INVALID_ARFCN;
      filter_ptr->scell_arfcn.band = SYS_BAND_NONE;
      filter_ptr->ba_ptr           = NULL;
      filter_ptr->ca_ptr           = NULL;
      filter_ptr->camped           = FALSE;
    }

    /* reset the filtering stats for debug purposes */
    memset(&(filter_ptr->arfcn_stats), 0, sizeof(rr_decode_bcch_stats_T));
  }
  else
  {
    MSG_GERAN_LOW_0_G("Candidate database filter structure is NULL");
  }
 
  return;
}

/**
 * Checks whether the specified ARFCN should be filtered
 *
 * @param arfcn 
 * @param filter_ptr 
 *
 * @return TRUE if the filter should apply to the specified ARFCN
 */
static boolean rr_decode_bcch_filter_channel(
  ARFCN_T arfcn,
  rr_decode_bcch_filter_T *filter_ptr
)
{
  if (filter_ptr != NULL)
  {
    uint8 i;

    /* always check for band filtering first */
    if (filter_ptr->band_mask != SYS_BAND_MASK_EMPTY)
    {
      if ((rr_convert_band_info(arfcn.band) & filter_ptr->band_mask) == 0)
      {
        filter_ptr->arfcn_stats.filtered_bs_count++;
        return(TRUE);
      }
    }

    /* before checking the rest of the filter, verify the current mode */
    /* because the pruning only applies for background manual searches */
    if ((rr_nv_get_plmn_search_freq_pruning_enabled() == FALSE) ||
        (filter_ptr->scan_mode   != SYS_NETWORK_SELECTION_MODE_MANUAL)            ||
        (filter_ptr->decode_type != RR_SEARCH_BACKGROUND)                         ||
        (filter_ptr->camped      == FALSE))
    {
      return(FALSE);
    }

    /* check whether the ARFCN is the current serving cell */
    if (ARFCNS_EQUAL(filter_ptr->scell_arfcn, arfcn))
    {
      filter_ptr->arfcn_stats.filtered_sc_count++;
      return(TRUE);
    }

    if (filter_ptr->ca_ptr != NULL)
    {
      /* assume anything in the Cell Allocation is in the same PLMN */
      for (i = 0; i < filter_ptr->ca_ptr->no_of_entries; i++)
      {
        if (ARFCNS_EQUAL(filter_ptr->ca_ptr->channel_number[i], arfcn))
        {
          filter_ptr->arfcn_stats.filtered_ca_count++;
          return(TRUE);
        }
      }
    }

    /* assume anything in the BA list is in the same PLMN */
    if (filter_ptr->ba_ptr != NULL)
    {
      for (i = 0; i < filter_ptr->ba_ptr->no_of_entries; i++)
      {
        if (ARFCNS_EQUAL(filter_ptr->ba_ptr->BCCH_ARFCN[i], arfcn))
        {
          filter_ptr->arfcn_stats.filtered_ba_count++;
          return(TRUE);
        }
      }
    }
  }

  return(FALSE);
}

/**
 * Starts a parallel decode operation using the information in the
 * candidate cell database
 *
 * @param candidate_db_ptr pointer to candidate cell database from which to perform the
 *                         parallel decode operation
 *
 * @return TRUE if there were candidates to process, FALSE otherwise
 */
boolean rr_candidate_db_send_decode_bcch_start_ind(
   rr_search_mode_T bcch_decode_type,
   boolean si3_only,
   sys_band_mask_type filter_band_mask
)
{
  rr_candidate_cell_db_T     *candidate_db_ptr = rr_candidate_cell_db_get_ptr();
  rr_decode_bcch_start_ind_t  decode_bcch_start_ind;
  rr_decode_bcch_filter_T     decode_bcch_filter;
  ARFCN_T                     candidate_arfcn;
  int                         num_candidates;
  int                         num_frequencies;
  int                         index;

  //RR_NULL_CHECK_FATAL(candidate_db_ptr);
  RR_ZERO_STRUCTURE(decode_bcch_start_ind);

  num_candidates  = candidate_db_ptr->num_frequencies;
  num_frequencies = 0;

  /* retrieve all the relevant parameters for this GAS ID */
  /* which avoids repeating the same code within the loop */
  rr_decode_bcch_load_filter_info(&decode_bcch_filter);
  decode_bcch_filter.decode_type = bcch_decode_type;
  decode_bcch_filter.band_mask   = filter_band_mask;

  /* build the frequency list from the candidate database */
  for (index = 0; index < num_candidates; index++)
  {
    candidate_arfcn.num = candidate_db_ptr->candidate_cells[index].meas.arfcn_num;
    candidate_arfcn.band = candidate_db_ptr->candidate_cells[index].meas.arfcn_band;
  
    /* apply filter per ARFCN (filtering is disabled for Cell Search) */
    if (rr_cell_search_in_progress() == FALSE && rr_decode_bcch_filter_channel(candidate_arfcn, &decode_bcch_filter))
    {
      candidate_db_ptr->candidate_cells[index].type = RR_CANDIDATE_FILTERED;
      candidate_db_ptr->candidate_cells[index].processed = TRUE;
    }
    else
    {
      decode_bcch_start_ind.bcch_freq_list.arfcn[num_frequencies] = candidate_arfcn;
      num_frequencies++;
    }
  }

  decode_bcch_start_ind.bcch_freq_list.num_frequencies = num_frequencies;

  MSG_GERAN_HIGH_2_G("Candidate cell database pruned from %d to %d channels", num_candidates, num_frequencies);
  if (num_candidates != num_frequencies)
  {
    /* additional debug to show which categories were used by the filter */
    MSG_GERAN_HIGH_2_G("BST count %d Serving Cell count %d",
                       decode_bcch_filter.arfcn_stats.filtered_bs_count,
                       decode_bcch_filter.arfcn_stats.filtered_sc_count);
    MSG_GERAN_HIGH_2_G("BA List count %d Cell Alloc count %d",
                       decode_bcch_filter.arfcn_stats.filtered_ba_count,
                       decode_bcch_filter.arfcn_stats.filtered_ca_count);
  }

  if (num_frequencies == 0)
  {
    /* nothing to process, send the appropriate signal to finish */
    MSG_GERAN_HIGH_0_G("No arfcns in database, send RR_DECODE_BCCH_OMSG_FINISHED_IND");
    rr_decode_bcch_send_finish_ind(NULL);
    return FALSE;
  }
  else
  {
    /* some frequencies were found, start the decode operation */
    decode_bcch_start_ind.bcch_decode_type = bcch_decode_type;
    decode_bcch_start_ind.si3_only = si3_only;
    rr_decode_bcch_send_start_ind(decode_bcch_start_ind);
    return TRUE;
  }
} /* rr_candidate_db_send_decode_bcch_start_ind */

/**
 * Searches DB for plmn
 *
 * @param plmn_db_ptr
 *       PLMN database
 * @param plmn_id
 *       PLMN id to search for
 * @return
 *       Pointer to PLMN in PLMN database or NULL if not found
 */
static rr_plmn_info_T* rr_plmn_db_search_plmn(
  rr_available_plmn_db_T *plmn_db_ptr,
  PLMN_id_T               plmn_id,
  boolean                 check_eplmn
)
{
  int counter=0;

  if (plmn_db_ptr->num_plmn>SYS_PLMN_LIST_MAX_LENGTH)
  {
    MSG_GERAN_ERROR_0_G("Invalid plmn db");
    return NULL;
  }

  /*search for plmn*/
  for (counter=0; counter<plmn_db_ptr->num_plmn; counter++)
  {
    if (sys_plmn_match(plmn_db_ptr->plmn_info[counter].PLMN_id, plmn_id))
    {
      /*got a match*/
      return &plmn_db_ptr->plmn_info[counter];
    }
  }

  if (check_eplmn)
  {
    /**
     * If the specific PLMN could not be found, we need to check for any
     * equivalent PLMNs.  2 PLMNs may be considered equivalent if they are both
     * in the EPLMN list.
     */
    if (rr_eplmn_list_equivalent_plmn(plmn_id))
    {
      for (counter=0; counter<plmn_db_ptr->num_plmn; counter++)
      {
        if (rr_eplmn_list_equivalent_plmn(plmn_db_ptr->plmn_info[counter].PLMN_id))
        {
          return &plmn_db_ptr->plmn_info[counter];
        }
      }
    }
  }

  return(NULL);
}

/*=======================================================
 * Function: Adds a freq into plmn db, if plmn is full
 *           does not add the freq. Assumes order that
 *           freqs are added into it are by signal strength.
 *
 * Input:    @plmn_db_ptr: PLMN db to add freq to
 *           @meas:        Measurement info of freq
 *           @lai:         Location area of freq
 *           @priority:    Priority of freq
 *           @GPRS_indicator: If the cell supports GPRS
 *
 * Output:   @plmn_db_ptr: the modified PLMN db
 =======================================================*/
static void rr_plmn_db_add_plmn_frequency_info(
  rr_available_plmn_db_T *plmn_db_ptr,
  rr_l1_freq_power_T     meas,
  LAI_T                  lai,
  rr_cell_type_T         priority,
  boolean                GPRS_indicator,
  ARFCN_T                channel_number
)
{
  int freq_index;
  int current_index;
  rr_plmn_info_T *temp_plmn_ptr;
  boolean add_new_entry = FALSE;

  /****************************************/
  /* Set pointer to plmn we are modifying */
  /****************************************/
  temp_plmn_ptr = rr_plmn_db_search_plmn(
    plmn_db_ptr,
    lai.plmn_id,
    FALSE/* Do not consider equivalent PLMNs to be identical when adding
          entries to the PLMN database. */
  );

  if (temp_plmn_ptr == NULL)
  {
    if (plmn_db_ptr->num_plmn == SYS_PLMN_LIST_MAX_LENGTH)
    {
      MSG_GERAN_ERROR_0_G("PLMN db full, can't add new plmn");
      return;
    }

    /* New PLMN, add new plmn */
    temp_plmn_ptr = &plmn_db_ptr->plmn_info[plmn_db_ptr->num_plmn];

    /*temp_plmn_ptr->band_indicator = band_indicator;*/
    temp_plmn_ptr->PLMN_id = lai.plmn_id;
    temp_plmn_ptr->num_frequencies = 0;

    /* for now set the band indicator to DCS 1800  */
    temp_plmn_ptr->band_indicator = RR_L1_DCS_1800;

    /**********************************/
    /* Increment number of plmn in db */
    /**********************************/
    plmn_db_ptr->num_plmn++;
  }

  /* find the appropriate position to insert the new entry (should be decreasing signal strength) */
  for (freq_index = temp_plmn_ptr->num_frequencies; freq_index > 0; --freq_index)
  {
    /* if the previous entry is >= than the new entry */
    if (temp_plmn_ptr->cell_info[freq_index - 1].meas.RXLEV_average >= meas.RXLEV_average)
    {
      /* place the new entry at the current index */
      break;
    }
  }

  /* find the appropriate position to insert the new entry (should be decreasing signal strength) */
  for (freq_index = temp_plmn_ptr->num_frequencies; freq_index > 0; --freq_index)
  {
    /* if the previous entry is >= than the new entry */
    if (temp_plmn_ptr->cell_info[freq_index - 1].meas.RXLEV_average >= meas.RXLEV_average)
    {
      /* place the new entry at the current index */
      break;
    }
  }

  if (temp_plmn_ptr->num_frequencies > BA_MAX_LEN)
  {
    MSG_GERAN_ERROR_2_G("temp_plmn_ptr->num_frequencies (%d) out of range, truncating to %d",
              temp_plmn_ptr->num_frequencies,
              BA_MAX_LEN);

    temp_plmn_ptr->num_frequencies = BA_MAX_LEN;
  }

  /* if the cell information array is not full */
  if (temp_plmn_ptr->num_frequencies < BA_MAX_LEN)
  {
    /* make room for new entry by shifting to the right */
    for (current_index = temp_plmn_ptr->num_frequencies; current_index > freq_index; --current_index)
    {
      temp_plmn_ptr->cell_info[current_index] = temp_plmn_ptr->cell_info[current_index - 1];
    }

    /* add the new entry */
    add_new_entry = TRUE;

    /* since the list wasn't full it now has one more entry */
    temp_plmn_ptr->num_frequencies++;
  }
  else /* maximum number of frequencies */
  {
    word channel_number = meas.arfcn_num;
    /* drop the cell with the smallest RXLEV (could be new cell) */
    if (freq_index == temp_plmn_ptr->num_frequencies)
    {
      /* it was the new cell */
      MSG_GERAN_HIGH_1_G("Ignore Cell=%d, AvailablePLMNDB full",(int) channel_number);

      /* don't add the new entry */
      add_new_entry = FALSE;
    }
    else
    {
      channel_number = temp_plmn_ptr->cell_info[temp_plmn_ptr->num_frequencies-1].meas.arfcn_num;

      MSG_GERAN_HIGH_1_G("Dropping Cell=%d, AvailablePLMNDB full",(int) channel_number);

      /* shift all the entries to the end dropping the last one */
      for (current_index = temp_plmn_ptr->num_frequencies - 1; current_index > freq_index; --current_index)
      {
        temp_plmn_ptr->cell_info[current_index] = temp_plmn_ptr->cell_info[current_index - 1];
      }

      /* add the new entry */
      add_new_entry = TRUE;

      /* list was full, so don't increment number of entries */
    }
  }

  if (add_new_entry)
  {
    temp_plmn_ptr->cell_info[freq_index].meas = meas;
    temp_plmn_ptr->cell_info[freq_index].priority = priority;
    temp_plmn_ptr->cell_info[freq_index].GPRS_indicator = GPRS_indicator;
    temp_plmn_ptr->cell_info[freq_index].lai = lai;
  }

  plmn_db_ptr->last_updated_plmn_ptr = temp_plmn_ptr;
} /* rr_plmn_db_add_plmn_frequency_info */


/**
 * Returns the name of the decode bcch state in string form
 *
 * @param: N/A
 *
 * @return Name of the state in string form
 */
static char * rr_decode_bcch_state_name(rr_decode_bcch_state_e state)
{
  char * s = "";

  switch( state )
  {
    case RR_DECODE_BCCH_INACTIVE:
      s = "InActive";
      break;

    case RR_DECODE_BCCH_IN_PROGRESS:
      s = "InProgress";
      break;

    case RR_DECODE_BCCH_ABORT:
      s = "Abort";
      break;

    default:
      s = "Unknown";
      break;
  }

  return s;
} /* rr_decode_bcch_state_name */

static void rr_candidate_cell_db_populate_detected_frequency_list(
  rr_arfcn_list_t        * detected_frequency_list,
  rr_candidate_cell_db_T * candidate_cell_db
)
{
  uint32 i;

  //RR_NULL_CHECK_RETURN_VOID(detected_frequency_list);
  //RR_NULL_CHECK_RETURN_VOID(candidate_cell_db);

  memset(detected_frequency_list, 0, sizeof(rr_arfcn_list_t));

  for (i = 0;
       i < candidate_cell_db->num_frequencies &&
       i < RR_MAX_CANDIDATE_FREQ_LIST;
       ++i)
  {
    if (candidate_cell_db->candidate_cells[i].type == RR_CANDIDATE_VALID_BCCH ||
        candidate_cell_db->candidate_cells[i].type == RR_CANDIDATE_BCCH_WITH_DECODE_FAILURES)
    {
      detected_frequency_list->entries[detected_frequency_list->count].num = candidate_cell_db->candidate_cells[i].meas.arfcn_num;
      detected_frequency_list->entries[detected_frequency_list->count++].band = candidate_cell_db->candidate_cells[i].meas.arfcn_band;

    }
  }
}

/*----------------------------------------------------------------------------
* Public Function Definitions
* -------------------------------------------------------------------------*/

void rr_candidate_cell_db_init( )
{
  rr_candidate_db_clear(rr_candidate_cell_db_get_ptr());
}

/**
 * Clears the given candidate cell database.
 *
 * @param candidate_db Pointer to the candidate cell database to clear.
 */
void rr_candidate_db_clear(rr_candidate_cell_db_T *candidate_cell_db)
{
  if (candidate_cell_db)
  {
    uint16 i;

    candidate_cell_db->valid = FALSE;
    candidate_cell_db->num_frequencies = 0;

    for (i = 0; i < RR_MAX_ARFCN; i++)
    {
      candidate_cell_db->frequency_index[i] = RR_NON_CANDIDATE_CELL;
    }

    candidate_cell_db->power_scan_offset = 0;

    MSG_GERAN_LOW_0("rr_candidate_db_clear set_populated_from_acq_db to 0");

    candidate_cell_db->populated_from_acq_db = FALSE;
  }
}

/**
 * Searches for a bcch in candidate db
 *
 * @param candidate_db_ptr
 *                   Database to search
 * @param BCCH_ARFCN Freq to search for
 *
 * @return Pointer to candidate cell if found, otherwise NULL
 */
rr_candidate_cell_T * rr_candidate_db_get_entry(
  rr_candidate_cell_db_T * candidate_cell_db,
  ARFCN_T                  BCCH_ARFCN
)
{
  if (candidate_cell_db)
  {
    uint16 hashed_value = rr_candidate_db_hash_arfcn(BCCH_ARFCN);

    if (hashed_value == RR_MAX_ARFCN)
    {
      return NULL;
    }
    else
    {
      uint16 i = candidate_cell_db->frequency_index[hashed_value];

      if ((i == RR_NON_CANDIDATE_CELL) || (i >= RR_MAX_CANDIDATE_FREQ_LIST))
      {
        return NULL;
      }
      else
      {
        return &candidate_cell_db->candidate_cells[i];
      }
    }
  }
  else
  {
    MSG_GERAN_LOW_0("candidate_cell_db == NULL");
    return NULL;
  }
} /* rr_candidate_db_get_entry */

/**
 * Refreshes the stored RXLEV for the specified candidate ARFCN
 * and Also the BSIC received from L1 
 * @param fcch_sch_ptr pointer to tone detect primitive from GL1
 */
void rr_candidate_update_FCCH_SCH(mph_fcch_sch_decode_ind_T *fcch_sch_ptr )
{
  if (fcch_sch_ptr != NULL)
  {
    rr_candidate_cell_db_T *candidate_db_ptr = rr_candidate_cell_db_get_ptr();
    
    if (candidate_db_ptr != NULL)
    {
      rr_candidate_cell_T *candidate_cell_ptr;

      /* look up the candidate cell entry for this ARFCN */
      candidate_cell_ptr = rr_candidate_db_get_entry(candidate_db_ptr, fcch_sch_ptr->BCCH_ARFCN);

      if (candidate_cell_ptr != NULL)
      {
        byte last_meas = candidate_cell_ptr->meas.RXLEV_average;

        /* update the value stored from the power scan with that of the SCH */
        candidate_cell_ptr->meas.RXLEV_average = fcch_sch_ptr->RXLEV_average;
        candidate_cell_ptr->bsic = fcch_sch_ptr->BSIC;
        candidate_cell_ptr->bsic_valid = TRUE;


        MSG_GERAN_HIGH_3_G("SCH Decoded: updated ARFCN %d from RXLEV %d to RXLEV %d",
                           (int)(fcch_sch_ptr->BCCH_ARFCN.num),
                           (int)last_meas,
                           (int)(fcch_sch_ptr->RXLEV_average));

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
        candidate_cell_ptr->fcch_snr = fcch_sch_ptr->fcch_snr;
        candidate_cell_ptr->fcch_snr_rcvd = TRUE;
        MSG_GERAN_HIGH_2_G("ARFCN=%d, SNR=%d", 
                            fcch_sch_ptr->BCCH_ARFCN.num, fcch_sch_ptr->fcch_snr);
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */

#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
        candidate_cell_ptr->meas.cell_timing = fcch_sch_ptr->frame_num;
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */
      }
    }
  }

  return;
}

/**
 * Clears the offset associated with the candiate
 * database
 *
 * @param candidate_db_ptr pointer to candidate database
 */
void rr_candidate_db_clear_offset(rr_candidate_cell_db_T *candidate_cell_db)
{
  if (candidate_cell_db)
  {
    candidate_cell_db->power_scan_offset = 0;
  }
} /* rr_candidate_db_clear_offset */

/**
 * Marks all the freq in the candidate db as unprocessed
 *
 * @param candidate_db_ptr
 *               Candidate database
 */
void rr_candidate_db_mark_all_as_processed(
  rr_candidate_cell_db_T *candidate_cell_db
)
{
  if (candidate_cell_db)
  {
    int i = 0;

    /*******************************************************/
    /* Change the status of all frequencies to processed   */
    /*******************************************************/
    for (i = 0; i < candidate_cell_db->num_frequencies; i++)
    {
      candidate_cell_db->candidate_cells[i].processed = TRUE;
    }
  }
} /* rr_candidate_db_mark_all_as_processed */


/**
 * Adds the given frequency information to the candidate cell database (if there is room)
 *
 * @param candidate_db_ptr candidate cell database to add results to
 *
 * @param candidate        pointer to candidate cell information to add
 */
void rr_candidate_db_add_frequency(
  rr_candidate_cell_db_T *   candidate_cell_db,
  const rr_l1_freq_power_T * candidate
)
{
  if (candidate_cell_db)
  {
    uint16 hashed_value;
    ARFCN_T arfcn;

    if (candidate_cell_db->num_frequencies >= RR_MAX_CANDIDATE_FREQ_LIST)
    {
      MSG_GERAN_ERROR_0("Candidate db full");
      return;
    }

    arfcn.num = candidate->arfcn_num;
    arfcn.band = candidate->arfcn_band;

    hashed_value = rr_candidate_db_hash_arfcn(arfcn);

    if (hashed_value == RR_MAX_ARFCN)
    {
      MSG_GERAN_LOW_2("Hashed ARFCN value indicates invalid ARFCN=(%d,%d), not added",
        candidate->arfcn_num,
        candidate->arfcn_band);
    }
    else
    {
      candidate_cell_db->candidate_cells[candidate_cell_db->num_frequencies].meas = *candidate;
      candidate_cell_db->candidate_cells[candidate_cell_db->num_frequencies].processed = FALSE;
      candidate_cell_db->candidate_cells[candidate_cell_db->num_frequencies].type = RR_CANDIDATE_UNKNOWN;
      candidate_cell_db->candidate_cells[candidate_cell_db->num_frequencies].is_acceptable = RR_CELL_SUITABILITY_UNKNOWN;
#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
      candidate_cell_db->candidate_cells[candidate_cell_db->num_frequencies].is_suitable = RR_CELL_SUITABILITY_UNKNOWN;
      candidate_cell_db->candidate_cells[candidate_cell_db->num_frequencies].fcch_snr_processed = FALSE;
      candidate_cell_db->candidate_cells[candidate_cell_db->num_frequencies].fcch_snr_rcvd = FALSE;
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */
      candidate_cell_db->frequency_index[hashed_value] = candidate_cell_db->num_frequencies;
#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
      candidate_cell_db->candidate_cells[candidate_cell_db->num_frequencies].power_meas_timestamp = geran_get_time_ms();
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */

      candidate_cell_db->num_frequencies += 1;
    }
  }
} /* rr_candidate_db_add_frequency */

/**
 * Checks if the freq is in the candidate db
 *
 * @param BCCH_ARFCN Freq to search for
 * @return TRUE if freq exists
 */
boolean rr_candidate_db_is_candidate_cell(
  ARFCN_T BCCH_ARFCN
)
{
  rr_candidate_cell_db_T *candidate_db_ptr = rr_candidate_cell_db_get_ptr();
  rr_candidate_cell_T    *candidate_cell_ptr;

  if (candidate_db_ptr != NULL)
  {
    candidate_cell_ptr = rr_candidate_db_get_entry(candidate_db_ptr, BCCH_ARFCN);
  }
  else
  {
    candidate_cell_ptr = NULL;
  }

  if (candidate_cell_ptr != NULL)
  {
    return(TRUE);
  }

  return(FALSE);
} /* rr_candidate_db_is_candidate_cell */

/**
 * Gets power scan offset
 *
 * @param candidate_db_ptr pointer to candidate cell database
 *
 * @return power scan offset of the candidate cell database
 */
word rr_candidate_db_get_power_scan_offset(
  const rr_candidate_cell_db_T * candidate_cell_db
)
{
  if (candidate_cell_db)
  {
    return candidate_cell_db->power_scan_offset;
  }
  else
  {
    MSG_GERAN_LOW_0("candidate_cell_db == NULL");
    return 0;
  }
} /* rr_candidate_db_get_power_scan_offset */

/**
 * Stores SI information for a candidate cell
 *
 * @param candidate_db_ptr
 *                   Database to search
 * @param BCCH_ARFCN Freq of candidate cell
 * @param si_info SI information to store
 */
rr_candidate_cell_T *rr_candidate_db_store_si_info(
  ARFCN_T                        BCCH_ARFCN,
  const rr_candidate_SI_info_T * si_info
)
{
  rr_candidate_cell_db_T *candidate_db_ptr = rr_candidate_cell_db_get_ptr();
  rr_candidate_cell_T    *candidate_cell_ptr;

  if (candidate_db_ptr != NULL)
  {
    candidate_cell_ptr = rr_candidate_db_get_entry(candidate_db_ptr, BCCH_ARFCN);
  }
  else
  {
    candidate_cell_ptr = NULL;
  }

  if ((candidate_cell_ptr == NULL) || (candidate_cell_ptr->type == RR_CANDIDATE_FILTERED))
  {
    return NULL;
  }

  if (candidate_cell_ptr->type == RR_CANDIDATE_UNKNOWN)
  {
    /* this is the first SI for this candidate cell, so reset the fields not supplied by SI4 */
    MSG_GERAN_LOW_0_G("SI4: reset CCD,SI2ter and SI2q info for candidate");
    candidate_cell_ptr->si_info.control_channel_description_valid = FALSE;
    memset(&(candidate_cell_ptr->si_info.control_channel_description),0,sizeof(control_channel_description_T));
    candidate_cell_ptr->si_info.two_ter_indicator = RR_SI2TER_UNKNOWN;
    candidate_cell_ptr->si_info.two_quater_position = SI2Q_POSITION_UNKNOWN;
  }

  candidate_cell_ptr->si_info = *si_info;
  candidate_cell_ptr->type = RR_CANDIDATE_VALID_BCCH;

#ifdef FEATURE_PS_PREFERRED_CELL
  if (candidate_cell_ptr->si_info.GPRS_indicator)
  {
    rr_inc_num_ps_cells_si_received();
  }
#endif /* FEATURE_PS_PREFERRED_CELL */

  if (candidate_cell_ptr->si_info.CELL_BAR_QUALIFY == 1)
  {
#ifdef FEATURE_PS_PREFERRED_CELL
    if (rr_ps_preferred_allowed())
    {
      if (candidate_cell_ptr->si_info.GPRS_indicator)
      {
        candidate_cell_ptr->priority = RR_LOW_PRIORITY_CSPS_CELL;
      }
      else
      {
        candidate_cell_ptr->priority = RR_LOW_PRIORITY_CS_CELL;
      }
    }
    else
#endif /* FEATURE_PS_PREFERRED_CELL */
    {
      candidate_cell_ptr->priority = RR_LOW_PRIORITY_CELL;
    }
  }
  else
  {
#ifdef FEATURE_PS_PREFERRED_CELL
    if (rr_ps_preferred_allowed())
    {
      if (candidate_cell_ptr->si_info.GPRS_indicator)
      {
        candidate_cell_ptr->priority = RR_HIGH_PRIORITY_CSPS_CELL;
      }
      else
      {
        candidate_cell_ptr->priority = RR_HIGH_PRIORITY_CS_CELL;
      }
    }
    else
#endif /* FEATURE_PS_PREFERRED_CELL */
    {
      candidate_cell_ptr->priority = RR_HIGH_PRIORITY_CELL;
    }
  }

  return candidate_cell_ptr;
} /* rr_candidate_db_store_si_info */


/**
 * Sets type field of a candidate cell
 *
 * @param candidate_db_ptr
 *                   Database to search
 * @param BCCH_ARFCN Freq to search for
 * @param type   The value the type field should be set to
 */
void rr_candidate_db_set_type(
  rr_candidate_cell_db_T        *candidate_db_ptr,
  ARFCN_T                       BCCH_ARFCN,
  rr_candidate_freq_type_enum_T type
)
{
  rr_candidate_cell_T *candidate_cell_ptr = rr_candidate_db_get_entry(candidate_db_ptr,BCCH_ARFCN);

  if (candidate_cell_ptr==NULL)
  {
    return;
  }

  candidate_cell_ptr->type = type;
} /* rr_candidate_db_set_type */

/**
 * Sets the type for a candidate cell entry based on the given failure reason
 *
 * @param candidate_db_ptr pointer to candidate cell database
 *
 * @param BCCH_ARFCN       ARFCN to modify
 *
 * @param failure_reason   failure reason to use to set the type
 */
void rr_candidate_db_set_type_from_failure_ind(
  rr_candidate_cell_db_T      *candidate_db_ptr,
  ARFCN_T                     BCCH_ARFCN,
  rr_l1_decode_bcch_failure_T failure_reason
)
{
  /* get the entry for the given frequency */
  rr_candidate_cell_T *candidate_cell_ptr = rr_candidate_db_get_entry(candidate_db_ptr,BCCH_ARFCN);
  boolean failed = TRUE;

  if (candidate_cell_ptr != NULL)
  {
    if (failure_reason == RR_L1_BCCH_FAILED)
    {
      candidate_cell_ptr->type = RR_CANDIDATE_BCCH_WITH_DECODE_FAILURES;
    }
    else if (failure_reason == RR_L1_SCH_FAILED)
    {
      candidate_cell_ptr->type = RR_CANDIDATE_NOT_BCCH;
    }
    else if (failure_reason == RR_L1_BCCH_BLOCKED)
    {
      /* treat a blocked BCCH as a failure in the same way as above */
      candidate_cell_ptr->type = RR_CANDIDATE_BCCH_WITH_DECODE_FAILURES;
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      /* but in multi-SIM mode, ensure this information is retained */
      if (rr_multi_sim_mode_enabled())
      {
        rr_ms_pscan_mark_arfcn_blocked(BCCH_ARFCN);
      }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
    }
    else
    {
      failed = FALSE;
    }

    /* If cell was previously considered as a PS cell then decrement the total count */
    if (failed && rr_acq_db_is_arfcn_ps_supported(BCCH_ARFCN))
    {
      rr_dec_num_ps_supported_cells_in_pscan();
    }
  }
  else
  {
    word channel_number = BCCH_ARFCN.num;
    MSG_GERAN_ERROR_1_G("Cell=%d not found in candidateDB",(int) channel_number);
  }
} /* rr_candidate_db_set_type_from_failure_ind */

/**
 * Sets processed status of a candidate cell
 *
 * @param candidate_db_ptr
 *                   Database to search
 * @param BCCH_ARFCN Freq of candidate cell
 * @param processed Value to set processed field of candidate cell
 */
void rr_candidate_db_set_processed(
  rr_candidate_cell_db_T *candidate_db_ptr,
  ARFCN_T                BCCH_ARFCN,
  boolean                processed
)
{
  rr_candidate_cell_T *candidate_cell_ptr = rr_candidate_db_get_entry(candidate_db_ptr,BCCH_ARFCN);

  if (candidate_cell_ptr==NULL)
  {
    return;
  }

  candidate_cell_ptr->processed = processed;
} /* rr_candidate_db_set_processed */


/**
 * Marks all of the valid BCCH entries in the candidate cell database
 * as unprocessed.
 */
void rr_candidate_db_mark_valid_entries_as_unprocessed(rr_candidate_cell_db_T *candidate_db_ptr)
{
  uint32 index;

  for (index = 0; index < candidate_db_ptr->num_frequencies; index++)
  {
    if (candidate_db_ptr->candidate_cells[index].type == RR_CANDIDATE_VALID_BCCH)
    {
      candidate_db_ptr->candidate_cells[index].processed = FALSE;
    }
  }
}

/**
 * Marks all entries with non-valid BCCH entries in the candidate cell database
 * as processed.
 */
void rr_candidate_db_mark_invalid_entries_as_processed(rr_candidate_cell_db_T *candidate_db_ptr)
{
  uint16 index;

  for (index = 0; index < candidate_db_ptr->num_frequencies; index++)
  {
    if ( (candidate_db_ptr->candidate_cells[index].type ==
           RR_CANDIDATE_NOT_BCCH) ||
         (candidate_db_ptr->candidate_cells[index].type ==
           RR_CANDIDATE_BCCH_WITH_DECODE_FAILURES)
       )
    {
      candidate_db_ptr->candidate_cells[index].processed = TRUE;
    }
  }
}

/**
  * Marks all the known (types other than RR_CANDIDATE_UNKNOWN) entries in the database
  * as processed
  */
void rr_candidate_db_mark_known_entries_as_processed(rr_candidate_cell_db_T *candidate_db_ptr)
{
  /* walk through all the cells in the database */
  int index;

  for (index = 0; index < candidate_db_ptr->num_frequencies; ++index)
  {
    /* if the type of this entry is anything other than unknown */
    if (candidate_db_ptr->candidate_cells[index].type != RR_CANDIDATE_UNKNOWN)
    {
      /* mark it as processed */
      candidate_db_ptr->candidate_cells[index].processed = TRUE;
    }
  }
} /* rr_candidate_db_mark_known_entries_as_processed */


/**
 * Returns whether the database is valid
 *
 * @param candidate_db_ptr
 *               Candidate Database
 * @return TRUE - Database is valid
 */
boolean rr_candidate_db_is_valid(
  const rr_candidate_cell_db_T * candidate_db_ptr
)
{
  return candidate_db_ptr->valid;
} /* rr_candidate_db_is_valid */

/**
 * Set valid flag in candidate db
 *
 * @param candidate_db_ptr
 *               Candidate db
 * @param valid  Value that candidate db valid flag should be set to
 */
void rr_candidate_db_set_valid(
  rr_candidate_cell_db_T *candidate_db_ptr,
  boolean                valid_info
)
{
  candidate_db_ptr->valid = valid_info;
} /* rr_candidate_db_set_valid */

/**
 * Returns the total number of frequencies in the candidate cell database.
 */
word rr_candidate_db_get_num_frequencies(
  const rr_candidate_cell_db_T * candidate_db
)
{
  return candidate_db->num_frequencies;
}

/**
 * Counts number of unprocessed candidate cells
 *
 * @param candidate_db_ptr
 *               Database to search
 * @return Number of unprocessed candidate cells
 */
uint16 rr_candidate_db_get_num_unprocessed_cell(
  const rr_candidate_cell_db_T * candidate_db_ptr
)
{
  uint16 counter=0;
  uint16 index=0;

  for (index=0; index<candidate_db_ptr->num_frequencies; index++)
  {
    if ((candidate_db_ptr->candidate_cells[index].processed == FALSE) &&
        (candidate_db_ptr->candidate_cells[index].type != RR_CANDIDATE_FILTERED))
    {
      counter++;
    }
  }

  return counter;
} /* rr_candidate_db_get_num_unprocessed_cell */

/**
 * Sets the type of all unknown entries to the given value
 *
 * @param candidate_db_ptr pointer to candidate db
 *
 * @param new_type         new value to set all unknown entries to
 */
void rr_candidate_db_set_unknown_entry_types(
  rr_candidate_cell_db_T *candidate_db_ptr,
  rr_candidate_freq_type_enum_T new_type
)
{
  int index;
  for (index = 0; (index < candidate_db_ptr->num_frequencies) && (index < RR_MAX_CANDIDATE_FREQ_LIST); ++index)
  {
    if (candidate_db_ptr->candidate_cells[index].type == RR_CANDIDATE_UNKNOWN)
    {
      /* set the new type */
      candidate_db_ptr->candidate_cells[index].type = new_type;
    }
  }
} /* rr_candidate_db_set_unknown_entry_types */

#ifdef FEATURE_PS_PREFERRED_CELL
/**
 * This function attempts to find any unprocessed but valid BCCH cell in the database,
 * using the specified search strategy.
 *
 * @return TRUE if a cell was found, otherwise FALSE
 *
 */
boolean rr_candidate_db_force_find_next_ps(
  rr_candidate_cell_db_T  *candidate_db_ptr,
  rr_candidate_cell_T     **entry_ptr_ptr,
  rr_candidate_process_strategy_T strategy
)
{
  int            counter = 0;
  rr_cell_type_T priority = (strategy == RR_CANDIDATE_HIGH_PRIORITY_CSPS_CELLS) ? RR_HIGH_PRIORITY_CSPS_CELL : RR_LOW_PRIORITY_CSPS_CELL;

  for (counter = 0; counter < candidate_db_ptr->num_frequencies; counter++)
  {
    if (!candidate_db_ptr->candidate_cells[counter].processed &&
        candidate_db_ptr->candidate_cells[counter].type == RR_CANDIDATE_VALID_BCCH &&
        rr_check_cell_same_priority(priority, candidate_db_ptr->candidate_cells[counter].priority))
    {
      *entry_ptr_ptr = &candidate_db_ptr->candidate_cells[counter];
      return TRUE;
    }
  }

  return FALSE;
}
#endif /* FEATURE_PS_PREFERRED_CELL */

/**
 * This function attempts to find a cell in the candidate cell database, using
 * the specified search strategy.
 *
 * @return TRUE if a cell was found.  The cell may have already had it's BCCH
 *         decoded, but it may not.  If it has not, this indicates that RR
 *         should continue to wait for this to happen.
 *
 *         FALSE if no cell was found, and there are no cells left waiting to
 *         be decoded.
 */
boolean rr_candidate_db_find_next(
  rr_candidate_cell_db_T  *candidate_db_ptr,
  rr_candidate_cell_T     **entry_ptr_ptr,
  rr_candidate_process_strategy_T strategy
)
{
  boolean cell_found = FALSE;

  switch (strategy)
  {
    case RR_CANDIDATE_STRONGEST_FIRST:
      cell_found = rr_candidate_db_find_strongest(candidate_db_ptr, entry_ptr_ptr);
      break;

    case RR_CANDIDATE_HIGH_PRIORITY_CELLS:
      cell_found = rr_candidate_db_find_strongest_of_priority(
        candidate_db_ptr, RR_HIGH_PRIORITY_CELL, entry_ptr_ptr);
      break;

    case RR_CANDIDATE_LOW_PRIORITY_CELLS:
      cell_found = rr_candidate_db_find_strongest_of_priority(
        candidate_db_ptr, RR_LOW_PRIORITY_CELL, entry_ptr_ptr);
      break;

#ifdef FEATURE_PS_PREFERRED_CELL
    case RR_CANDIDATE_HIGH_PRIORITY_CSPS_CELLS:
        cell_found = rr_candidate_db_find_strongest_of_priority(
          candidate_db_ptr, RR_HIGH_PRIORITY_CSPS_CELL, entry_ptr_ptr);
        break;

    case RR_CANDIDATE_HIGH_PRIORITY_CS_CELLS:
        cell_found = rr_candidate_db_find_strongest_of_priority(
          candidate_db_ptr, RR_HIGH_PRIORITY_CS_CELL, entry_ptr_ptr);
        break;

    case RR_CANDIDATE_LOW_PRIORITY_CSPS_CELLS:
        cell_found = rr_candidate_db_find_strongest_of_priority(
          candidate_db_ptr, RR_LOW_PRIORITY_CSPS_CELL, entry_ptr_ptr);
       break;

    case RR_CANDIDATE_LOW_PRIORITY_CS_CELLS:
        cell_found = rr_candidate_db_find_strongest_of_priority(
          candidate_db_ptr, RR_LOW_PRIORITY_CS_CELL, entry_ptr_ptr);
        break;
#endif /* FEATURE_PS_PREFERRED_CELL */

    default:
      break;
  }

  return cell_found;
} /* rr_candidate_db_find_next */

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
/**
 * This function attempts to find a cell in the candidate cell database, using
 * the specified search strategy.
 *
 * @return TRUE if a cell was found. return TRUE in following cases
 *         If cell's BCCH received but not processed.
 *         If cell's BCCH not received but SNR received and SNR not processed  
 *
 *         FALSE if no cell was found, and there are no cells left waiting to
 *         be decoded.
 */
boolean rr_candidate_db_fcch_snr_find_next(
  rr_candidate_cell_db_T           *candidate_db_ptr,
  rr_candidate_cell_T             **entry_ptr_ptr,
  rr_candidate_process_strategy_T   strategy
)
{
  int            counter = 0;
  boolean        cell_found = FALSE;
  boolean        check_priority = FALSE;
  rr_cell_type_T priority = RR_LOW_PRIORITY_CELL;

  if (candidate_db_ptr == NULL)
  {
    MSG_GERAN_ERROR_0("rr_candidate_db_fcch_snr_find_next: candidate_db_ptr == NULL");
    return FALSE;
  }

  if (entry_ptr_ptr == NULL)
  {
    MSG_GERAN_ERROR_0("rr_candidate_db_fcch_snr_find_next: entry_ptr_ptr == NULL");
    return FALSE;
  }

  switch (strategy)
  {
    case RR_CANDIDATE_STRONGEST_FIRST:
      break;

    case RR_CANDIDATE_HIGH_PRIORITY_CELLS:
      check_priority = TRUE;
      priority = RR_HIGH_PRIORITY_CELL;
      break;

    case RR_CANDIDATE_LOW_PRIORITY_CELLS:
      check_priority = TRUE;
      priority = RR_LOW_PRIORITY_CELL;
      break;

#ifdef FEATURE_PS_PREFERRED_CELL
    case RR_CANDIDATE_HIGH_PRIORITY_CSPS_CELLS:
      check_priority = TRUE;
      priority = RR_HIGH_PRIORITY_CSPS_CELL;
      break;

    case RR_CANDIDATE_HIGH_PRIORITY_CS_CELLS:
      check_priority = TRUE;
      priority = RR_HIGH_PRIORITY_CS_CELL;
      break;

    case RR_CANDIDATE_LOW_PRIORITY_CS_CELLS:
      check_priority = TRUE;
      priority = RR_LOW_PRIORITY_CS_CELL;
      break;

    case RR_CANDIDATE_LOW_PRIORITY_CSPS_CELLS:
      check_priority = TRUE;
      priority = RR_LOW_PRIORITY_CSPS_CELL;
      break;
#endif /* FEATURE_PS_PREFERRED_CELL */

    default:
      MSG_GERAN_ERROR_1("Unknown Candidate cell process strategy, %d",strategy);
      break;
  }

  for (counter = 0; counter < candidate_db_ptr->num_frequencies; counter++)
  {
    if (!candidate_db_ptr->candidate_cells[counter].processed)
    {
      if (candidate_db_ptr->candidate_cells[counter].type == RR_CANDIDATE_UNKNOWN)
      {
        if (!candidate_db_ptr->candidate_cells[counter].fcch_snr_processed &&
            candidate_db_ptr->candidate_cells[counter].fcch_snr_rcvd)
        {
          cell_found = TRUE;
        }
      }
      else if (candidate_db_ptr->candidate_cells[counter].type == RR_CANDIDATE_VALID_BCCH)
      {
        if ((!check_priority && candidate_db_ptr->candidate_cells[counter].priority != RR_CELL_BARRED) ||
            (check_priority && rr_check_cell_same_priority(priority, candidate_db_ptr->candidate_cells[counter].priority)))
        {
          cell_found = TRUE;
        }
      }
    }

    if (cell_found)
    {
      *entry_ptr_ptr = &candidate_db_ptr->candidate_cells[counter];
      break;
    }
  }

  return cell_found;
} /* rr_candidate_db_fcch_snr_find_next */
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */

/**
 * Searches if a bcch (can be valid/invalid due to decode failures)
 * has been found in the candidate database
 *
 * @param candidate_db_ptr
 *               Database to search
 * @return TRUE - if BCCH found
 */
boolean rr_candidate_db_search_for_BCCH(
  const rr_candidate_cell_db_T * candidate_db_ptr
)
{
  int index=0;

  for (index=0;index<candidate_db_ptr->num_frequencies;index++)
  {
    if (candidate_db_ptr->candidate_cells[index].type==RR_CANDIDATE_VALID_BCCH ||
        candidate_db_ptr->candidate_cells[index].type==RR_CANDIDATE_BCCH_WITH_DECODE_FAILURES )
    {
      return TRUE;
    }
  }

  return FALSE;
} /* rr_candidate_db_search_for_BCCH */

/**
 * Checks if a candidate cell is suitable
 *
 * @param candidate_ptr
 *               Candidate cell
 * @param only_acceptable
 *               If TRUE does not check if if plmn matches requested PLMN
 * @param automatic_selection
 *               if TRUE checks that it is not in a forbidden LAI
 * @return TRUE if candidate cell is suitable
 */
boolean rr_check_candidate_suitable(
  rr_candidate_cell_T* candidate_ptr,
  boolean only_acceptable,
  boolean automatic_selection
)
{
  RACH_control_parameters_T temp_rach_control_parameters;
  cell_selection_parameters_T temp_cell_selection_parameters;
  BSIC_T invalid_bsic = {0xFF, 0xFF};
  ARFCN_T arfcn;

  temp_rach_control_parameters.CELL_BAR_ACCESS = candidate_ptr->si_info.CELL_BAR_ACCESS;
  temp_rach_control_parameters.access_control[0] = candidate_ptr->si_info.access_control[0];
  temp_rach_control_parameters.access_control[1] = candidate_ptr->si_info.access_control[1];
  temp_cell_selection_parameters.CELL_BAR_QUALIFY = candidate_ptr->si_info.CELL_BAR_QUALIFY;
  temp_cell_selection_parameters.MS_TXPWR_MAX_CCH = candidate_ptr->si_info.MS_TXPWR_MAX_CCH;
  temp_cell_selection_parameters.POWER_OFFSET = candidate_ptr->si_info.POWER_OFFSET;
  temp_cell_selection_parameters.power_offset_valid = TRUE;
  temp_cell_selection_parameters.RXLEV_ACCESS_MIN = candidate_ptr->si_info.RXLEV_ACCESS_MIN;

  /* Check whether the bsic in cand db has valid bsic.
   * Then update the invalid_bsic with cand db value
   */

  if(candidate_ptr->bsic_valid == TRUE)
  {
    invalid_bsic.PLMN_colour_code = candidate_ptr->bsic.PLMN_colour_code;
    invalid_bsic.BS_colour_code = candidate_ptr->bsic.BS_colour_code;

    MSG_GERAN_LOW_2_G("BSIC updated locally from CAND DB, PLMN CC: %d   BS CC %d", invalid_bsic.PLMN_colour_code, invalid_bsic.BS_colour_code);

  }

#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
  if (!rr_cell_is_in_plmn_lac_range(candidate_ptr->si_info.lai))
  {
    /* if a cell is not in the LAC list, don't camp on it, skip it */
    MSG_GERAN_HIGH_1_G("cell's LAC %d is not in the range of the selected PLMN",
      candidate_ptr->si_info.lai.location_area_code);
    return FALSE;
  }
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */

  arfcn.num = candidate_ptr->meas.arfcn_num;
  arfcn.band = candidate_ptr->meas.arfcn_band;

#ifdef FEATURE_WCDMA
  if (rr_w2g_redirection_in_progress())
  {
    return rr_check_cell_suitable_w2g_redirection(
      &temp_cell_selection_parameters,
      &temp_rach_control_parameters,
      &candidate_ptr->si_info.lai,
      candidate_ptr->meas.RXLEV_average,
      arfcn,
      /* BSIC not available yet, use invalid BSIC */
      &invalid_bsic,
      only_acceptable,
      automatic_selection,
      candidate_ptr->si_info.GPRS_indicator
    );
  }
  else
#endif /* FEATURE_WCDMA */
  {
    return rr_check_cell_suitable(
      &temp_cell_selection_parameters,
      &temp_rach_control_parameters,
      &candidate_ptr->si_info.lai,
      candidate_ptr->meas.RXLEV_average,
      arfcn,
      /* BSIC not available yet, use invalid BSIC */
      &invalid_bsic,
      only_acceptable,
      automatic_selection,
      TRUE,
      candidate_ptr->si_info.GPRS_indicator
    );
  }
} /* rr_check_candidate_suitable */

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB
/**
 * Compares strongest cell with next top cells to find best candidate
 *
 * @param candidate_db_ptr Candidate database
 *
 * @param best_cell_found Will be set to TRUE is best cell found
 *
 * @param best_cell_index Will be set to the index of best cell
 *               
 * @return void
 */
void rr_candidate_db_compare_for_best_fcch_snr(
  rr_candidate_cell_db_T *candidate_db_ptr,
  uint16                  strongest_cell_index,
  boolean                *best_cell_found,
  uint16                 *best_cell_index
)
{
  uint16               i = 0;
  uint16               s_index;
  uint16               strongest_cell_snr;
  uint16               b_index = 0;
  uint16               best_cell_snr;
  rr_candidate_cell_T *top_candidate_ptr = NULL;
  boolean              wait_for_info = FALSE;

  RR_NULL_CHECK_RETURN_VOID(candidate_db_ptr);

  s_index = strongest_cell_index;
  strongest_cell_snr = candidate_db_ptr->candidate_cells[s_index].fcch_snr;
  b_index = s_index;
  best_cell_snr = strongest_cell_snr;

  /* check if complete info is received for next top cells to compare SNR and get the best candidate. 
   * Maximum number of top cells to compare is FCCH_SNR_MAX_CELLS_TO_COMPARE. */
  for(i = 1; 
      (i <= FCCH_SNR_MAX_CELLS_TO_COMPARE) && ((i + s_index) < candidate_db_ptr->num_frequencies); 
      i++)
  {
    top_candidate_ptr = &candidate_db_ptr->candidate_cells[i + s_index];
    
    if(top_candidate_ptr->fcch_snr_rcvd)
    {
      /* 
       * Consider top cell if its SNR (SNR2) is more than strongest cell (SNR1) by 3dB
       * i.e. SNR2(dB) - SNR1(dB) >= 3 dB
       * => 10 * log(SNR2(linear)/256) - 10 * log(SNR1(linear)/256) >= 3
       * => 10 * log(SNR2(linear) / SNR1(linear)) >= 3
       * => log(SNR2(linear) / SNR1(linear)) >= 0.3
       * => SNR2(linear) / SNR1(linear) >= 2
       * => SNR2(linear) /2  >= SNR1(linear)
       */
      if((top_candidate_ptr->fcch_snr >> 1) >= strongest_cell_snr)
      {
        if(top_candidate_ptr->fcch_snr > best_cell_snr)
        {
          /* SI 3/4 received.*/
          if(top_candidate_ptr->type == RR_CANDIDATE_VALID_BCCH)
          {
            /* If cell is suitable mark it as best cell. */
            if(top_candidate_ptr->is_suitable == RR_CELL_SUITABLE)
            {
              best_cell_snr = top_candidate_ptr->fcch_snr;
              b_index = i + s_index;
              /* Top Cell is BETTER cell...STOP comparision. */
              break;
            }
            else if(top_candidate_ptr->is_suitable == RR_CELL_SUITABILITY_UNKNOWN)
            {
              wait_for_info = TRUE;
              break;
            }
          }
          /* SI 3/4 not received, so wait for it. */
          else
          {
            wait_for_info = TRUE;
            break;
          }
        }
      }
    }
    /* FCCH SNR of some top cells are not received, so wait for it. */
    else
    {
      wait_for_info = TRUE;
      break;
    }
  }

  if(wait_for_info)
  {
    *best_cell_found = FALSE;
  }
  else
  {
    *best_cell_found = TRUE;
    *best_cell_index = b_index;
  }
} /* rr_candidate_db_compare_for_best_fcch_snr */

/**
 * Finds a best candidate based on FCCH SNR criteria
 *
 * @param candidate_db_ptr Candidate database
 *
 * @param best_cell_found Will be set to TRUE is best cell found
 *
 * @param best_cell_index Will be set to the index of best cell
 *               
 * @return void
 */
void rr_candidate_db_find_best_of_fcch_snr(
  rr_candidate_cell_db_T *candidate_db_ptr,
  boolean                *best_cell_found,
  uint16                 *best_cell_index
)
{
  uint16               i = 0;
  uint16               strongest_cell_index;
  rr_candidate_cell_T *strongest_candidate_ptr = NULL;
  rr_candidate_cell_T *cell_ptr = NULL;

  RR_NULL_CHECK_RETURN_VOID(candidate_db_ptr);
  *best_cell_found = FALSE;

  /* If we have received any suitable cell whose SNR is more than 10dBm camp on it. */
  for(i = 0; i < candidate_db_ptr->num_frequencies; i++)
  {
    cell_ptr = &candidate_db_ptr->candidate_cells[i];

    if(cell_ptr->is_suitable == RR_CELL_SUITABLE && 
       cell_ptr->fcch_snr >= FCCH_SNR_THRESHOLD_1)
    {
      *best_cell_found = TRUE;
      *best_cell_index = i;
      return;
    }
  }
  
  /* get the strongest suitable cell for which FCCH SNR based evaluation can be done. */
  for(i = 0; i < candidate_db_ptr->num_frequencies; i++)
  {
    cell_ptr = &candidate_db_ptr->candidate_cells[i];

    if(cell_ptr->is_suitable == RR_CELL_SUITABILITY_UNKNOWN && 
       cell_ptr->type == RR_CANDIDATE_UNKNOWN)
    {
      break;
    }
    else if(cell_ptr->is_suitable == RR_CELL_SUITABLE)
    {
      strongest_candidate_ptr = cell_ptr;
      strongest_cell_index = i;
      break;
    }
  }

  if(strongest_candidate_ptr != NULL)
  {
    if(strongest_candidate_ptr->fcch_snr_rcvd)
    {
      /* SNR is more than FCCH_SNR_THRESHOLD_1, it is suitable to camp on. */
      if(strongest_candidate_ptr->fcch_snr >= FCCH_SNR_THRESHOLD_1)
      {
        *best_cell_found = TRUE;
        *best_cell_index = strongest_cell_index;
      }
      /* SNR is not suitable. Compare strongest cell's SNR with other top cells to get best cell */
      else
      {
        rr_candidate_db_compare_for_best_fcch_snr(candidate_db_ptr, strongest_cell_index, best_cell_found, best_cell_index);
      }
    }
    else
    {
      MSG_GERAN_ERROR_0_G("ERROR:: FCCH SNR not received / invalid");
    }
  }
} /* rr_candidate_db_find_best_of_fcch_snr */
#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */

/**
 * Starts a parallel decode operation using the information in the
 * candidate cell database
 *
 * @param candidate_db_ptr pointer to candidate cell database from which to perform the
 *                         parallel decode operation
 *
 * @return RR_EV_DECODING_BCCH
 */
rr_event_T rr_perform_bcch_list_decode_from_candidate_db(
  const rr_candidate_cell_db_T * candidate_db_ptr
)
{
  uint16 index;
  uint16 num_frequencies;
  rr_event_T event_out = EV_NO_BCCH_FOUND;

  rr_l1_idle_freq_list_T *freq_list_ptr = GPRS_MEM_CALLOC(sizeof(rr_l1_idle_freq_list_T));
  RR_NULL_CHECK_FATAL(freq_list_ptr);

  num_frequencies = MIN(candidate_db_ptr->num_frequencies, ARR_SIZE(freq_list_ptr->arfcn));

  /* build the frequency list from the candiate database */
  for (index = 0; index < num_frequencies; ++index)
  {
    freq_list_ptr->arfcn[index].num = candidate_db_ptr->candidate_cells[index].meas.arfcn_num;
    freq_list_ptr->arfcn[index].band = candidate_db_ptr->candidate_cells[index].meas.arfcn_band;
  }

  freq_list_ptr->num_frequencies = num_frequencies;

  MSG_GERAN_HIGH_1_G("no of arfcns = %d", num_frequencies);

  /* if no frequencies were found in the candidate database */
  if (freq_list_ptr->num_frequencies > 0)
  {
    /* clear the pending channel data store since it could interfere with the candidate DB processing */
    rr_clear_pending_channel_data();

    /* now send the message to L1 */
    rr_send_mph_decode_bcch_list_req(freq_list_ptr, TRUE, FALSE, FALSE);

    event_out = RR_EV_DECODING_BCCH;
  }

  GPRS_MEM_FREE(freq_list_ptr);

  return  event_out;
} /* rr_perform_bcch_list_decode_from_list */


/**
 * Retarts a parallel decode operation. Frequency list sent to layer1 includes
 * those arfcns for which decodes have not yet been received.
 *
 * @param candidate_db_ptr pointer to candidate cell database from which to
 * perform the parallel decode operation
 *
 * @return EV_NO_BCCH_FOUND if there no unknown cells otherwise
 *         returns RR_EV_DECODING_BCCH.
 */
rr_event_T rr_redo_bcch_list_decode_from_candidate_db(
  rr_candidate_cell_db_T  *candidate_db_ptr,
  rr_search_order_e        search_order
)
{
  uint16 index;
  uint16 num_frequencies = 0;
  rr_event_T event_out = EV_NO_BCCH_FOUND;

  rr_l1_idle_freq_list_T *freq_list_ptr = GPRS_MEM_CALLOC(sizeof(rr_l1_idle_freq_list_T));
  RR_NULL_CHECK_FATAL(freq_list_ptr);

  /* build the frequency list from the candiate database */
  for (index = 0; index < candidate_db_ptr->num_frequencies; ++index)
  {
    /* only those UNKNOWN (no SI received) cells need to be decoded */
    if(candidate_db_ptr->candidate_cells[index].type == RR_CANDIDATE_UNKNOWN)
    {
      freq_list_ptr->arfcn[num_frequencies].num = candidate_db_ptr->candidate_cells[index].meas.arfcn_num;
      freq_list_ptr->arfcn[num_frequencies++].band = candidate_db_ptr->candidate_cells[index].meas.arfcn_band;
    }
  }
  freq_list_ptr->num_frequencies = num_frequencies;

  MSG_GERAN_HIGH_1_G("no of arfcns = %d",(int) num_frequencies);

  /* if no frequencies were found in the candidate database */
  if (num_frequencies == 0)
  {
    MSG_GERAN_LOW_0_G("No unprocessed arfcns in candidate db");
  }
  else /* some unprocessed frequencies were found */
  {
    MSG_GERAN_HIGH_0_G("Unprocessed arfcns in candidate db");

    /* clear the pending channel data store since it could interfere with the
       candidate DB processing */
    rr_clear_pending_channel_data();

    /* now send the message to L1 */
    rr_send_mph_decode_bcch_list_req(freq_list_ptr, TRUE, FALSE, FALSE);

    if (rr_candidate_db_get_populated_from_acq_db(candidate_db_ptr))
    {
      MSG_GERAN_LOW_0_G("Candidate cell database was populated from ACQ-DB scan");
      event_out = RR_EV_DECODING_ACQ_DB_BCCH;
    }
    else
    {
      MSG_GERAN_LOW_0_G("Candidate cell database was populated from full scan");
      event_out = RR_EV_DECODING_BCCH;
    }
  }

  GPRS_MEM_FREE(freq_list_ptr);

  return event_out;
} /* rr_redo_bcch_list_decode_from_list */


/**
 * Checks whether the candidate cell database was populated as a result of
 * an ACQ-DB search or a full search.
 *
 * @param candidate_db The candidate cell database to check.
 * @return TRUE if the candidate cell database was populated as a result of an
 *         ACQ-DB search, FALSE otherwise.
 */
boolean rr_candidate_db_get_populated_from_acq_db(
  const rr_candidate_cell_db_T * candidate_db_ptr
)
{
  MSG_GERAN_MED_1("rr_candidate_db_get_populated_from_acq_db: %d",
    candidate_db_ptr->populated_from_acq_db);
  return candidate_db_ptr->populated_from_acq_db;
}


/**
 * Sets the flag that indicates whether the candidate cell database was populated
 * as a result of an ACQ-DB search or a full search.
 *
 * @param candidate_db The candidate cell database.
 * @param populated_from_acq_db TRUE if the candidated cell database was
 *        populated as a result of an ACQ-DB search, FALSE otherwise.
 */
void rr_candidate_db_set_populated_from_acq_db(
  rr_candidate_cell_db_T *candidate_db_ptr,
  boolean populated_from_acq_db
)
{
  MSG_GERAN_MED_1("rr_candidate_db_set_populated_from_acq_db: %d",
    populated_from_acq_db);
  candidate_db_ptr->populated_from_acq_db = populated_from_acq_db;
}


/**
 * Checks if candidate db decode is complete, i.e.
 * DECODE_BCCH_LIST_CNF has been received and number of
 * UNKNOWN candidate_cells is not 1 (if it is one, we shall
 * wait for the single outstanding SI).
 *
 * @Params: N/A
 *
 * @Outputs: TRUE if decode is complete, FALSE otherwise
 */
boolean rr_candidate_db_decode_bcch_complete(void)
{
  rr_candidate_cell_db_T *candidate_db_ptr;
  uint16                  num_unknown;
  rr_decode_bcch_data_T *rr_decode_bcch_data_ptr = rr_decode_bcch_get_data_ptr();

  if (rr_decode_bcch_data_ptr->decode_cnf_received == FALSE)
  {
    return FALSE;
  }

  candidate_db_ptr = rr_candidate_cell_db_get_ptr();
  if (candidate_db_ptr != NULL)
  {
    /**
     * The type of all entries in the database (except for possibly the last one)
     * should be known by now. Check how many there are left with unknown status
     */
    num_unknown = rr_candidate_db_get_num_unknown_cell(candidate_db_ptr);

    if (num_unknown == 1)
    {
      /*******************************************************/
      /* There are still one unprocessed cells, this is okay */
      /* race condition between RR and L2                    */
      /*******************************************************/
      MSG_GERAN_HIGH_0_G("Starting BCCHDecodeComplete Timer");
      (void)rr_timer_start(RR_BCCH_DECODE_COMPLETE_TIMER,RR_BCCH_DECODE_COMPLETE_TIMEOUT);

      /* wait for the last event to arrive */
      return FALSE;
    }

    if (num_unknown > 1)
    {
      /****************************************************************/
      /* There is more then 1 unknown type cell, something went wrong */
      /* Don't bother waiting for those BCCHs (mark them as not BCCH) */
      /****************************************************************/
      MSG_GERAN_ERROR_1_G("Cand. db error %d unknown",(int) num_unknown);
      rr_candidate_db_set_unknown_entry_types(candidate_db_ptr, RR_CANDIDATE_NOT_BCCH);
    }
  }

  /* No more candidate cells left */
  if (rr_timer_cancel(RR_BCCH_DECODE_COMPLETE_TIMER))
  {
    MSG_GERAN_HIGH_0_G("Stopped BCCHDecodeComplete Timer");
  }

  return TRUE;

}

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param 
 * \return rr_candidate_cell_db_T*
 */
rr_candidate_cell_db_T *rr_get_candidate_cell_db_ptr( void )
{
  // Obtain a pointer to the Available PLMN DB
  return rr_candidate_cell_db_get_ptr();
}

/***********************************************************
 *   rr_available_plmn_db related functions
 ***********************************************************/

/**
 * Function: Checks if the RR available PLMN database is valid.
 *           The database is considered valid if it's 'valid' flag is set,
 *           it has more than 0 entries and the band_pref that was
 *           effective when it was set valid is equal to the band_pref
 *           passed in to this function.
 *           Additionally, even if the PLMN database is valid, if RR is camped
 *           in Limited Service this function will always indicate
 *           that it is not valid.  This is because during cell selection
 *           for Limited Service, RR will terminate the cell selection as soon as
 *           any cell is found, so the PLMN database will not contain a complete
 *           set of cells for the current environment.
 *
 * Input:    plmn_db_ptr - PLMN database
 *           band_pref   - The band_pref to use when verifying the validity
 *
 * Output:   return TRUE if PLMN database is valid
 */
boolean rr_plmn_db_is_valid(
   const rr_available_plmn_db_T *plmn_db_ptr,
   sys_band_mask_type band_pref
)
{
  boolean plmn_db_valid = FALSE;

  RR_NULL_CHECK_RETURN_PARAM(plmn_db_ptr, FALSE);
  RR_ASSERT_RETURN_VALUE(plmn_db_ptr->num_plmn <= SYS_PLMN_LIST_MAX_LENGTH, FALSE);

  plmn_db_valid = (boolean) (plmn_db_ptr->valid &&
                     plmn_db_ptr->num_plmn != 0 &&
                     plmn_db_ptr->band_pref == band_pref);

  if ( plmn_db_valid )
  {
    /* If camped on limited service, to avoid delay of finding normal service,
     * always starts new search, i.e. declares current plmn db invalid
     */
    if ( rr_is_camped() && rr_is_any_cell_selection_mode() )
    {
      plmn_db_valid = FALSE;
    }
  }

  return plmn_db_valid;

} /* rr_plmn_db_is_valid */

/**
 * Function: Sets PLMN valid status
 *
 * Input:    @plmn_db_ptr - PLMN database
 *           @valid - valid status
 *
 * Output:   updated PLMN database valid status
 */
void rr_plmn_db_set_valid(
  rr_available_plmn_db_T *plmn_db_ptr,
  sys_band_mask_type band_pref
)
{
  plmn_db_ptr->valid = TRUE;
  plmn_db_ptr->band_pref = band_pref;

  if (rr_timer_cancel(RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER))
  {
    MSG_GERAN_MED_0_G("Cancelled RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER");
  }

  MSG_GERAN_HIGH_0_G("Starting RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER");
  (void)rr_timer_start(RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER,
                       RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMEOUT);

} /* rr_plmn_db_set_valid */

/**
 * Invalidates the available PLMN database without clearing it (the contents
 * are not cleared, only its valid status is changed)
 */
void rr_invalidate_available_plmn_database(void)
{
  rr_available_plmn_db_T * available_plmn_db_ptr;
  rr_candidate_cell_db_T * candidate_db_ptr;

  available_plmn_db_ptr = rr_get_available_plmn_db_ptr();
  if (available_plmn_db_ptr != NULL)
  {
    rr_plmn_db_clear(available_plmn_db_ptr);
  }

  candidate_db_ptr = rr_candidate_cell_db_get_ptr();
  if (candidate_db_ptr != NULL)
  {
    /* invalidate the candidate cell database as well */
    rr_candidate_db_set_valid(candidate_db_ptr, FALSE);
  }

  return;
} /* rr_invalidate_available_plmn_database */

/**
 * Function: Clears the available PLMN database
 *
 * Input:    @plmn_db_ptr - PLMN db to be cleared
 *
 * Output:   @plmn_db_ptr - the cleared PLMN db
 */
void rr_plmn_db_clear(
  rr_available_plmn_db_T *plmn_db_ptr
)
{
  if (plmn_db_ptr)
  {
    plmn_db_ptr->band_pref = SYS_BAND_MASK_EMPTY;
    plmn_db_ptr->num_plmn = 0;
    plmn_db_ptr->valid = FALSE;
    plmn_db_ptr->last_updated_plmn_ptr = NULL;

    if (rr_timer_cancel(RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER))
    {
      MSG_GERAN_MED_0_G("Cancelled RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER");
    }
  }
} /* rr_plmn_db_clear */

/**
 * Searches for plmn in plmn_db. Returns frequencies (sorted
 * by strength) into the high priority and low priority arrays.
 *
 * @param plmn_db_ptr
 *               Database to search through
 * @param PLMN_id_ptr
 *               PLMN to search for, if null searches all
 * @param high_priority_array
 *               buffer for high priority cells
 * @param low_priority_array
 *               buffer for low priority cells
 */
void rr_plmn_db_get_plmn_frequency_info(
  rr_available_plmn_db_T * plmn_db_ptr,
  const PLMN_id_T *        PLMN_id_ptr,
  rr_l1_idle_freq_list_T * high_priority_array,
  rr_l1_idle_freq_list_T * low_priority_array
)
{
  int counter=0;
  word high_priority_index=0;
  word low_priority_index=0;
  int plmn_counter=0;
  rr_plmn_info_T *temp_plmn_ptr=NULL;
  rr_l1_freq_power_T high_priority_results[RR_MAX_CANDIDATE_FREQ_LIST];
  rr_l1_freq_power_T low_priority_results[RR_MAX_CANDIDATE_FREQ_LIST];


  if (PLMN_id_ptr!=NULL)
  {
    /****************************************/
    /* We are searching for a specific plmn */
    /* Set pointer to plmn we are modifying */
    /****************************************/
    temp_plmn_ptr = rr_plmn_db_search_plmn(
      plmn_db_ptr,
      *PLMN_id_ptr,
      TRUE/* Allow this function to return equivalent PLMNs. */
    );
  }
  else
  {
    /****************************************/
    /* No plmn specified check them all     */
    /* starting from the first one          */
    /****************************************/
    if (plmn_db_ptr->num_plmn!=0)
    {
      temp_plmn_ptr = &plmn_db_ptr->plmn_info[0];
    }
  }

  while (temp_plmn_ptr!=NULL)
  {
    /***********************************************************************/
    /* Loop through the cells in the plmn and put them in the proper lists */
    /***********************************************************************/
    for (counter=0;counter<temp_plmn_ptr->num_frequencies;counter++)
    {
      if (rr_check_cell_same_priority(RR_HIGH_PRIORITY_CELL, temp_plmn_ptr->cell_info[counter].priority) &&
          (high_priority_index < RR_MAX_CANDIDATE_FREQ_LIST))
      {
        high_priority_results[high_priority_index] =  temp_plmn_ptr->cell_info[counter].meas;
        high_priority_index++;
      }
      else if (rr_check_cell_same_priority(RR_LOW_PRIORITY_CELL, temp_plmn_ptr->cell_info[counter].priority) &&
               (low_priority_index < RR_MAX_CANDIDATE_FREQ_LIST))
      {
        low_priority_results[low_priority_index] = temp_plmn_ptr->cell_info[counter].meas;
        low_priority_index++;
      }
      else
      {
        /* TODO may need some special logic when full, should never be full */
        MSG_GERAN_HIGH_2_G("Barred cell or full (hi=%d,lo=%d)",
                 high_priority_index, low_priority_index);
      }
    }

    plmn_counter++;

    /*****************************************************/
    /* Finished looking at that plmn check if we need to */
    /* check all of them, if we do make sure we aren't   */
    /* didn't just finish the last one                   */
    /*****************************************************/
    if ( PLMN_id_ptr!=NULL || plmn_counter>=plmn_db_ptr->num_plmn )
    {
      break;
    }
    else
    {
      if (plmn_counter < SYS_PLMN_LIST_MAX_LENGTH)
      {
        temp_plmn_ptr = &plmn_db_ptr->plmn_info[plmn_counter];
      }
      else
      {
        MSG_GERAN_ERROR_1_G("plmn_counter (%d) out of range", plmn_counter);
      }
    }
  }

  qsort((void *)high_priority_results,
        high_priority_index,
        sizeof(rr_l1_freq_power_T),
        rr_compare_freq_power);

  qsort((void *)low_priority_results,
        low_priority_index,
        sizeof(rr_l1_freq_power_T),
        rr_compare_freq_power);

  /*Copy over*/
  for (counter=0; counter<high_priority_index; counter++)
  {
    high_priority_array->arfcn[counter].num = high_priority_results[counter].arfcn_num;
    high_priority_array->arfcn[counter].band = high_priority_results[counter].arfcn_band;
  }

  for (counter=0; counter<low_priority_index; counter++)
  {
    low_priority_array->arfcn[counter].num = low_priority_results[counter].arfcn_num;
    low_priority_array->arfcn[counter].band = low_priority_results[counter].arfcn_band;

  }

  high_priority_array->num_frequencies = high_priority_index;
  low_priority_array->num_frequencies = low_priority_index;

#ifdef FEATURE_GSM_LAC_BASED_PLMN_SEARCH
  rr_plmn_lac_range_valid = FALSE;
#endif /* FEATURE_GSM_LAC_BASED_PLMN_SEARCH */

  return;
}

/**
 * Function: Add a candidate cell into rr_available_plmn_db
 *
 * Input:    @candidate_cell_ptr: pointer to a cell to be added
 * Output:   updated rr_available_plmn_db
 */
void rr_plmn_db_update(
  rr_candidate_cell_T *candidate_cell_ptr,
  ARFCN_T              channel_number
)
{
  rr_available_plmn_db_T *available_plmn_db_ptr;

  available_plmn_db_ptr = rr_get_available_plmn_db_ptr();

  if (candidate_cell_ptr == NULL)
  {
    return;
  }

  if (candidate_cell_ptr->type != RR_CANDIDATE_VALID_BCCH)
  {
    MSG_GERAN_ERROR_0_G("candidate cell is not valid BCCH");
    return;
  }

  /* if the cell is acceptable (manual vs. automatic mode doesn't matter) */
  if (rr_check_candidate_suitable(candidate_cell_ptr,TRUE,FALSE) &&
      (available_plmn_db_ptr != NULL))
  {
    /* Add the cell the the available PLMN database */
    MSG_GERAN_HIGH_2_G("Cell=(%d, %d) acceptable, add to PLMN DB",
             (int)candidate_cell_ptr->meas.arfcn_num,
             (int)candidate_cell_ptr->meas.arfcn_band);

    /* Can't select acceptable cell (must be other PLMN or forbidden LAI) */
    /* add it to the available PLMN db */
    rr_plmn_db_add_plmn_frequency_info(
      available_plmn_db_ptr,
      candidate_cell_ptr->meas,
      candidate_cell_ptr->si_info.lai,
      candidate_cell_ptr->priority,
      candidate_cell_ptr->si_info.GPRS_indicator,
      channel_number
    );

    candidate_cell_ptr->is_acceptable = RR_CELL_SUITABLE;
  }
  else
  {
    MSG_GERAN_HIGH_2_G("Cell=(%d,%d) not acceptable",
             (int)candidate_cell_ptr->meas.arfcn_num,
             (int)candidate_cell_ptr->meas.arfcn_band);
    candidate_cell_ptr->is_acceptable = RR_CELL_UNSUITABLE;
  }
}


rr_available_plmn_db_T * rr_get_available_plmn_db_ptr(void)
{
  // Obtain a pointer to the Available PLMN DB
  return rr_available_plmn_db_get_ptr();
}

/***********************************************************
 * Decode_bcch list functions
 *
 * Decode bcch list activity takes rr_candidate_cell_db
 * (derived from pscan results), sends parrallel decoding requst
 * to L1, updates rr_candidate_cell_db with the received SIs or failure_indications
 * also populates rr_available_plmn_db
 ***********************************************************/

/**
 * Function: Setup decode_bcch_list state machine before use it
 *
 * Input:    @foreground: decode in foreground or background
 *           @band_pref:  decode band_pref
 *
 * Output:   updated rr_decode_bcch_data_ptr
 */
void rr_decode_bcch_list_setup( )
{
  rr_decode_bcch_data_T *rr_decode_bcch_data_ptr = rr_decode_bcch_get_data_ptr();

  memset(rr_decode_bcch_data_ptr, 0, sizeof(rr_decode_bcch_data_T));
  rr_decode_bcch_data_ptr->decode_cnf_received = FALSE;
  rr_decode_bcch_data_ptr->si3_only = FALSE;
  rr_decode_bcch_data_ptr->state = RR_DECODE_BCCH_INACTIVE;
  rr_decode_bcch_data_ptr->old_state = RR_DECODE_BCCH_INACTIVE;
}

/**
 * Function: Set decode_cnf_received flag
 *
 * Input:    @cnf_received: CNF received status
 * Output:   updated rr_decode_bcch_data_ptr->decode_cnf_received
 */
void rr_set_decode_cnf_received(boolean cnf_received )
{
  rr_decode_bcch_data_T *rr_decode_bcch_data_ptr = rr_decode_bcch_get_data_ptr();

  //RR_NULL_CHECK_FATAL(rr_decode_bcch_data_ptr);

  rr_decode_bcch_data_ptr->decode_cnf_received = cnf_received;
}

/**
 * Function: decode_bcch_list state machine
 *
 * Input:    @event
 *           @message_ptr
 *
 * Output:   output event from decode_bcch_list state machin
 *             EV_NO_EVENT  - nothing to output
 *             EV_DECODE_BCCH_LIST_COMPLETE - decode complete
 */
void rr_decode_bcch_list_control(rr_event_T event, rr_cmd_bdy_type *message_ptr )
{
  rr_decode_bcch_data_T *rr_decode_bcch_data_ptr = rr_decode_bcch_get_data_ptr();
  rr_candidate_cell_db_T *candidate_db_ptr = rr_candidate_cell_db_get_ptr();
  rr_event_T              new_event        = EV_NO_EVENT;
  byte                    message_id       = 0;
  byte                    message_set      = 0;

  event = rr_extract_message_header_details(event, message_ptr,
                                            &message_set,
                                            &message_id);

  /* EV_RESET_SOFTWARE and EV_PANIC_RESET are handled in stateless manner. */
  if ((event == EV_RESET_SOFTWARE) || (event == EV_PANIC_RESET))
  {
    memset(rr_decode_bcch_data_ptr, 0, sizeof(rr_decode_bcch_data_T));
    rr_decode_bcch_data_ptr->state = RR_DECODE_BCCH_INACTIVE;
    return;
  }

  while (event != EV_NO_EVENT)
  {
    if ((event == EV_INPUT_MESSAGE) && (message_ptr == NULL))
    {
      event = EV_NO_EVENT;
      MSG_GERAN_ERROR_0_G("message == NULL");
      continue;
    }

    switch (rr_decode_bcch_data_ptr->state)
    {
      case RR_DECODE_BCCH_INACTIVE:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_DECODE_BCCH_IMSG:
                  {
                    switch (message_ptr->rr.decode_bcch.imsg.header.imh.message_id)
                    {
                      case RR_DECODE_BCCH_IMSG_START_IND:
                      {
#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
                        sys_loc_wwan_scan_rat_gsm_param_s_type wwan_scan_assistance_data_gsm = (rr_get_plmn_list_req_info_ptr())->gps_wwan_search_info.assist_param.wwan_rat_scan_param.gsm_params;
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */
                        rr_l1_idle_freq_list_T frequency_list;
                        uint8 index;
                        boolean decode_fcch_only = FALSE;

                        RR_ZERO_STRUCTURE(frequency_list);

                        rr_decode_bcch_data_ptr->bcch_decode_type =
                          message_ptr->rr.decode_bcch.imsg.decode_bcch_start_ind.bcch_decode_type;
                        rr_decode_bcch_data_ptr->si3_only =
                          message_ptr->rr.decode_bcch.imsg.decode_bcch_start_ind.si3_only;
                        rr_decode_bcch_data_ptr->decode_cnf_received = FALSE;

                        /* clear the pending channel data as it could interfere with the candidate DB processing */
                        rr_clear_pending_channel_data();

                        //Take a copy of frequecy list                                                         
                        frequency_list = message_ptr->rr.decode_bcch.imsg.decode_bcch_start_ind.bcch_freq_list; 


#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
                       for (index = 0; index < wwan_scan_assistance_data_gsm.num_gsm_ad_element && index < SYS_MAX_RESULT_PER_RAT; index++)
                       {
                         if (wwan_scan_assistance_data_gsm.gsm_ad_elems[index].decode_gcid == TRUE)
                         {
                           break;
                         }
                       }
                       if (wwan_scan_assistance_data_gsm.num_gsm_ad_element != 0 && (index == wwan_scan_assistance_data_gsm.num_gsm_ad_element || index == SYS_MAX_RESULT_PER_RAT))
                       {
                         MSG_GERAN_HIGH_0_G("GPS FR : setting decode fcch only to TRUE");
                         decode_fcch_only = TRUE;
                       }
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */

                        if((rr_decode_bcch_data_ptr->bcch_decode_type == RR_SEARCH_NORMAL) ||
                          (rr_decode_bcch_data_ptr->bcch_decode_type == RR_SEARCH_RESELECTION))
                        {
                          /* foreground or reselection BCCH list decode */
                          rr_send_mph_decode_bcch_list_req(
                            &frequency_list,
                            TRUE,
                            message_ptr->rr.decode_bcch.imsg.decode_bcch_start_ind.si3_only,
                            decode_fcch_only
                          );
                        }
                        else
                        {
                          /* background BCCH list decode */
                          rr_send_mph_decode_bcch_list_req(
                            &frequency_list,
                            FALSE,
                            message_ptr->rr.decode_bcch.imsg.decode_bcch_start_ind.si3_only,
                            decode_fcch_only
                          );
                        }

                        /* MPH_DECODE_BCCH_LIST_REQ has been sent to L1,
                         * move to RR_DECODE_BCCH_IN_PROGRESS wait for SIs etc
                         */
                        rr_decode_bcch_data_ptr->state = RR_DECODE_BCCH_IN_PROGRESS;

                        break;
                      }

                      case RR_DECODE_BCCH_IMSG_CANDIDATE_CELL_SI_IND:
                      {
                        break;  // Ignore
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message_ptr,rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
                      }
                    }
                    break;
                  } /* RR_PSCAN_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr,rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
                  }
                }
                break;
              } /*MS_RR_RR*/

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr,rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
              }
            }
            break;
          }

          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          {
            break;  // ignore
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(event), rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
          }
        }
        break;
      }

      case RR_DECODE_BCCH_IN_PROGRESS:
      {
        switch (event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_DECODE_BCCH_IMSG:
                  {
                    switch (message_ptr->rr.decode_bcch.imsg.header.imh.message_id)
                    {
                      case RR_DECODE_BCCH_IMSG_ABORT_REQ:
                      {
                        rr_send_mph_decode_bcch_list_abort_req();

                        rr_decode_bcch_data_ptr->state = RR_DECODE_BCCH_ABORT;
                        break;
                      }

                      case RR_DECODE_BCCH_IMSG_CANDIDATE_CELL_SI_IND:
                      {
                        new_event = EV_DECODE_BCCH_LIST_UPDATE;
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message_ptr,rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
                      }
                    }
                    break;
                  } /* RR_PSCAN_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr,rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
                  }
                }
                break;
              } /*MS_RR_RR*/

              case MS_RR_L1:
              {
                switch ((rr_l1_message_id_enum_T) message_id)
                {
                  case MPH_DECODE_BCCH_LIST_CNF:
                  {
                    rr_decode_bcch_data_ptr->decode_cnf_received = TRUE;
                    new_event = EV_DECODE_BCCH_LIST_UPDATE;
                    break;
                  }

                  case MPH_DECODE_BCCH_LIST_FAILURE_IND:
                  {
                    rr_candidate_db_set_type_from_failure_ind(
                      candidate_db_ptr,
                      message_ptr->mph_decode_bcch_list_failure_ind.frequency,
                      message_ptr->mph_decode_bcch_list_failure_ind.failure_cause
                    );
                    new_event = EV_DECODE_BCCH_LIST_UPDATE;
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr,rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
                  }
                }
                break;
              } /*MS_RR_L1*/

              case MS_TIMER:
              {
                switch (message_id)
                {
                  case MID_TIMER_EXPIRY:
                  {
                    timer_expiry_T *timer_expiry = (timer_expiry_T *) message_ptr;

                    if (timer_expiry->timer_id == RR_BCCH_DECODE_COMPLETE_TIMER)
                    {
                      rr_candidate_db_set_unknown_entry_types(candidate_db_ptr,
                                                              RR_CANDIDATE_NOT_BCCH);
                      new_event = EV_DECODE_BCCH_LIST_UPDATE;
                    }
                    else
                    {
                      MSG_GERAN_ERROR_1_G("Unexpected timer expiry, timer_id=%d",timer_expiry->timer_id);
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr,rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
                  }
                }
                break;
              } /*MS_TIMER*/

              default:
              {
                LOG_UNEXPECTED_MESSAGE(message_ptr,rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
              }
            }
            break;
          }

          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
            new_event = EV_DECODE_BCCH_LIST_UPDATE;
            break;

          case RR_EV_DECODE_BCCH_COMPLETE_TIMEOUT:
            rr_candidate_db_set_unknown_entry_types(candidate_db_ptr,
                                                    RR_CANDIDATE_NOT_BCCH);
            new_event = EV_DECODE_BCCH_LIST_UPDATE;
            break;

          case EV_DECODE_BCCH_LIST_UPDATE:
            if (rr_candidate_db_decode_bcch_complete())
            {
              if (!rr_candidate_db_search_for_BCCH(candidate_db_ptr))
              {
                /* No BCCHs were found in the last parallel decode
                 * operation, try the next set of cells
                 */
                MSG_GERAN_HIGH_0_G("No BCCHs found in last decode, search next set");

                if((rr_decode_bcch_data_ptr->bcch_decode_type == RR_SEARCH_NORMAL) ||
                  (rr_decode_bcch_data_ptr->bcch_decode_type == RR_SEARCH_BACKGROUND))
                {
                  /* Check the power scan to see if we have any cells left */
                  if (rr_pscan_process_power_scan_results(RR_NORMAL_SEARCH_ORDER) != 0)
                  {
                    (void) rr_candidate_db_send_decode_bcch_start_ind(
                             rr_decode_bcch_data_ptr->bcch_decode_type,
                             rr_decode_bcch_data_ptr->si3_only,
                             SYS_BAND_MASK_EMPTY   // TODO - filter_band_mask for MPLMN ?
                           );
                  }
                  else
                  {
                    MSG_GERAN_LOW_0_G("BCCH LIST decode complete!");
                    rr_decode_bcch_send_finish_ind(NULL);
                  }
                }
                else
                {
                  MSG_GERAN_LOW_0_G("BCCH LIST decode complete!");
                  rr_decode_bcch_send_finish_ind(NULL);
                }
              }
              else
              {
                rr_arfcn_list_t detected_frequency_list;

                MSG_GERAN_LOW_0_G("BCCH LIST decode complete!");


                rr_candidate_cell_db_populate_detected_frequency_list(
                  &detected_frequency_list,
                  candidate_db_ptr
                );

                rr_decode_bcch_send_finish_ind(
                  &detected_frequency_list
                );
              }
              rr_decode_bcch_data_ptr->state = RR_DECODE_BCCH_INACTIVE;
            }
            break;

          default:
            LOG_UNEXPECTED_EVENT(rr_event_name(event),
              rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
            break;
        }
        break;
      }

      case RR_DECODE_BCCH_ABORT:
        switch (event)
        {
          case EV_INPUT_MESSAGE:
            switch (message_set)
            {
              case MS_RR_L1:
                switch (message_id)
                {
                  case MPH_DECODE_BCCH_LIST_CNF:
                    MSG_GERAN_LOW_1_G("BCCH LIST abort complete (msg 0x%02X)", (int)message_id);
                    rr_decode_bcch_send_abort_cnf();
                    rr_decode_bcch_data_ptr->state = RR_DECODE_BCCH_INACTIVE;
                    break;

                  case MPH_DECODE_BCCH_LIST_FAILURE_IND:
                    break;

                  default:
                    LOG_UNEXPECTED_MESSAGE(message_ptr,
                      rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
                    break;
                }
                break;


              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_DECODE_BCCH_IMSG:
                  {
                    switch (message_ptr->rr.decode_bcch.imsg.header.imh.message_id)
                    {
                      case RR_DECODE_BCCH_IMSG_ABORT_REQ:
                      {
                        MSG_GERAN_LOW_0_G("Received RR_DECODE_BCCH_IMSG_ABORT_REQ when already aborting");
                        break;
                      }

                      case RR_DECODE_BCCH_IMSG_CANDIDATE_CELL_SI_IND:
                      {
                        break;  // Ignore
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(message_ptr,rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
                      }
                    }
                    break;
                  } /* RR_PSCAN_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr,rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
                  }
                }
                break;
              } /*MS_RR_RR*/

              case MS_TIMER:
              {
                switch (message_id)
                {
                  case MID_TIMER_EXPIRY:
                  {
                    timer_expiry_T *timer_expiry = (timer_expiry_T *) message_ptr;

                    if ((timer_expiry->timer_id == RR_BCCH_DECODE_COMPLETE_TIMER) && (rr_decode_bcch_data_ptr->decode_cnf_received))
                    {
                      MSG_GERAN_HIGH_0_G("BCCH decode abort completed)");
                      rr_candidate_db_set_unknown_entry_types(candidate_db_ptr,RR_CANDIDATE_NOT_BCCH);
                      rr_decode_bcch_send_abort_cnf();
                      rr_decode_bcch_data_ptr->state = RR_DECODE_BCCH_INACTIVE;
                    }
                    else
                    {
                      MSG_GERAN_ERROR_1_G("Unexpected timer expiry, timer_id=%d",timer_expiry->timer_id);
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(message_ptr,rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
                  }
                }
                break;
              } /*MS_TIMER*/

              default:
                LOG_UNEXPECTED_MESSAGE(message_ptr,
                  rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
                break;
            }
            break;

          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
            break;

          default:
            LOG_UNEXPECTED_EVENT(rr_event_name(event),
              rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
            break;
        }
        break;

      default:
        MSG_GERAN_ERROR_2_G("UnexEV. Ev(%d), State(%s)",(int)event,
          rr_decode_bcch_state_name(rr_decode_bcch_data_ptr->state));
        break;
    }

#ifdef DEBUG_RR_TRACE_MSG
    /* If the state has changed or a new event has been generated then update
    the trace buffer. */
    if (rr_decode_bcch_data_ptr->state != rr_decode_bcch_data_ptr->old_state ||
        new_event != EV_NO_EVENT)
    {
      rr_store_trace_msg_buf(
        RR_DECODE_BCCH_LIST_SM,
        new_event,
        (byte) rr_decode_bcch_data_ptr->state,
        message_ptr
      );
    }
#endif

    if (rr_decode_bcch_data_ptr->state != rr_decode_bcch_data_ptr->old_state)
    {
      RR_LOG_STATE_TRANSITION(
        "rr_decode_bcch_list_control",
        rr_decode_bcch_data_ptr->old_state,
        rr_decode_bcch_data_ptr->state,
        rr_decode_bcch_state_name
      );

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
      /* The state has changed, manage the BPLMN guard timer */
      if (rr_decode_bcch_data_ptr->bcch_decode_type == RR_SEARCH_BACKGROUND)
      {
        if (rr_decode_bcch_data_ptr->state == RR_DECODE_BCCH_IN_PROGRESS)
        {
          /* BPLMN has just started */
          if (rr_timer_start(RR_DUAL_SIM_BPLMN_LIST_TIMER, RR_DUAL_SIM_BPLMN_LIST_TIMEOUT))
          {
            MSG_GERAN_HIGH_0_G("Started BPLMN guard timer");
          }
        }
        else if (rr_decode_bcch_data_ptr->old_state == RR_DECODE_BCCH_IN_PROGRESS)
        {
          /* BPLMN has just finished, been aborted, or state machine was reset */
          if (rr_timer_cancel(RR_DUAL_SIM_BPLMN_LIST_TIMER))
          {
            MSG_GERAN_HIGH_0_G("Cancelled BPLMN guard timer");
          }
        }
      }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

      rr_decode_bcch_data_ptr->old_state = rr_decode_bcch_data_ptr->state;
    }

    if (new_event != EV_NO_EVENT)
    {
      event = new_event;
      new_event = EV_NO_EVENT;
    }
    else
    {
      event = EV_NO_EVENT;
    }
  }
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*===========================================================================

FUNCTION     rr_ms_candidate_db_search_timeout

DESCRIPTION  Performs actions specific to a DSDS search timeout

PARAMS       

DEPENDENCIES

RETURN VALUE None

===========================================================================*/
void rr_ms_candidate_db_search_timeout(void )
{
  rr_candidate_cell_db_T *db_ptr = rr_candidate_cell_db_get_ptr();

  if (db_ptr != NULL)
  {
    uint16 index;
    uint16 counter = 0;

    /* converts all unknown candidates to BCCH failures so that no further searching will occur */
    for (index = 0; index < db_ptr->num_frequencies; index++)
    {
      if (db_ptr->candidate_cells[index].type == RR_CANDIDATE_UNKNOWN)
      {
        db_ptr->candidate_cells[index].type = RR_CANDIDATE_BCCH_WITH_DECODE_FAILURES;
        counter++;
      }
    }

    MSG_GERAN_HIGH_2_G("BCCH list had %d candidates, %d will be treated as failures", (int)index, (int)counter);
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Candidate cell database pointer is NULL");
  }

  return;
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

void rr_candidate_get_bsi_scheduling_info(
  rr_candidate_cell_T *candidate_cell_ptr,
  uint32 *si_on_bcch_ext_ptr,
  uint32 *si_not_broadcast_ptr
)
{
  uint32 on_bcch_ext   = MASK_SI_NONE;
  uint32 not_broadcast = MASK_SI_NONE;

  if (candidate_cell_ptr->si_info.two_ter_indicator == RR_SI2TER_NOT_EXPECTED)
  {
    not_broadcast |= MASK_SI_2_TER;
    not_broadcast |= MASK_SI_2_TER_REST;
  }

  if (candidate_cell_ptr->si_info.two_quater_position == SI2Q_POSITION_NONE)
  {
    not_broadcast |= MASK_SI_2_QUATER;
  }
  else
  if (candidate_cell_ptr->si_info.two_quater_position == SI2Q_POSITION_BCCH_Ext)
  {
    on_bcch_ext |= MASK_SI_2_QUATER;
  }

  if (candidate_cell_ptr->si_info.GPRS_indicator)
  {
    if (candidate_cell_ptr->si_info.si13_on_bcch_ext)
    {
      on_bcch_ext |= MASK_SI_13;
    }
  }
  else
  {
    not_broadcast |= MASK_SI_13;
  }

  if (si_on_bcch_ext_ptr != NULL)
  {
    *si_on_bcch_ext_ptr = on_bcch_ext;
  }

  if (si_not_broadcast_ptr != NULL)
  {
    *si_not_broadcast_ptr = not_broadcast;
  }
}

static mm_rr_ps_support_e_type rr_mm_is_arfcn_ps_supported(ARFCN_T arfcn)
{
  mm_rr_ps_support_e_type mm_ps_support = MM_PS_SUPPORT_UNKNOWN;
  rr_ps_support_status_t ps_support = rr_acq_db_is_arfcn_ps_supported(arfcn);

  switch (ps_support)
  {
    case PS_NOT_SUPPORTED:
      mm_ps_support = MM_PS_NOT_SUPPORTED;
      break;
      
    case PS_SUPPORTED:
        mm_ps_support = MM_PS_SUPPORTED;
        break;

    case PS_SUPPORT_UNKNOWN:
    default: break;
  }

  return mm_ps_support;
}

boolean rr_candidate_db_found_best_pci_cells(uint8 num_cells_to_search)
{
  uint32 i;
  uint8 num_freq_found = 0;
  
  rr_candidate_cell_db_T *candidate_db_ptr = rr_get_candidate_cell_db_ptr();

  for (i = 0;
       i < candidate_db_ptr->num_frequencies &&
       i < RR_MAX_CANDIDATE_FREQ_LIST &&
       num_freq_found < num_cells_to_search;
       ++i)
  {
    if (candidate_db_ptr->candidate_cells[i].is_acceptable == RR_CELL_SUITABLE)
    {
      num_freq_found++;
    }
    else if (candidate_db_ptr->candidate_cells[i].type == RR_CANDIDATE_UNKNOWN)
    {
      MSG_GERAN_LOW_1_G("waiting for ARFCN %d bcch_decode", candidate_db_ptr->candidate_cells[i].meas.arfcn_num);
      return FALSE;
    }
  }

  MSG_GERAN_MED_2_G("Found %d freq so far (num_cells_to_search=%d)", num_freq_found, num_cells_to_search);

  if (num_freq_found >= num_cells_to_search)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
 * Populates the processed candidate db information into
 * plmn_list_cnf to pass available cell information to NAS
 *
 * @param available_plmn_db_ptr to be populated
 *
 */
void rr_candidate_cell_db_populate_cell_info_list(
  sys_geran_cell_list_s_type     *detected_frequency_list
)
{
  uint32 i;
  rr_candidate_cell_db_T *candidate_db_ptr = rr_get_candidate_cell_db_ptr();

  memset(detected_frequency_list, 0, sizeof(sys_cell_search_list_s_type));

  for (i = 0;
       i < candidate_db_ptr->num_frequencies &&
       i < RR_MAX_CANDIDATE_FREQ_LIST &&
       detected_frequency_list->num_cell < SYS_PCI_SCAN_MAX_NUM_PCI;
       ++i)
  {
    MSG_GERAN_LOW_2_G("PLMN_LIST_CNF: ARFCN %d added = %d",
      candidate_db_ptr->candidate_cells[i].meas.arfcn_num,
      candidate_db_ptr->candidate_cells[i].is_acceptable);

    if (candidate_db_ptr->candidate_cells[i].is_acceptable == RR_CELL_SUITABLE)
    {
      sys_geran_cell_info_s_type *temp_geran_cell_info = 
      &detected_frequency_list->geran_cell_info[detected_frequency_list->num_cell];

      temp_geran_cell_info->ARFCN.num = candidate_db_ptr->candidate_cells[i].meas.arfcn_num;
      temp_geran_cell_info->ARFCN.band = candidate_db_ptr->candidate_cells[i].meas.arfcn_band;
      temp_geran_cell_info->BSIC = CONVERT_BSIC_TO_BYTE(candidate_db_ptr->candidate_cells[i].bsic);
      temp_geran_cell_info->cell_identity = candidate_db_ptr->candidate_cells[i].si_info.cell_id;

      if (candidate_db_ptr->candidate_cells[i].priority == RR_CELL_BARRED)
      {
        temp_geran_cell_info->cell_barred = TRUE;
      }

      temp_geran_cell_info->gprs_supported = rr_mm_is_arfcn_ps_supported(temp_geran_cell_info->ARFCN);
      temp_geran_cell_info->PLMN = candidate_db_ptr->candidate_cells[i].si_info.lai.plmn_id;
      temp_geran_cell_info->LAC = candidate_db_ptr->candidate_cells[i].si_info.lai.location_area_code;
      temp_geran_cell_info->RxLev = candidate_db_ptr->candidate_cells[i].meas.RXLEV_average;
      detected_frequency_list->num_cell++;
    }
  }
}

#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
void rr_candidate_cell_db_populate_gps_plmn_search_list(
  sys_loc_wwan_scan_result_s_type    **dfl
)
{
  uint8 i;
  uint8 result_index = 0;
  rr_candidate_cell_T *candidate_db_cell;
  rr_candidate_cell_db_T *candidate_db_ptr = rr_get_candidate_cell_db_ptr();
  sys_loc_wwan_scan_rat_gsm_param_s_type wwan_scan_assistance_data_gsm = (rr_get_plmn_list_req_info_ptr())->gps_wwan_search_info.assist_param.wwan_rat_scan_param.gsm_params;
  gprs_scell_info_t *scell_info = rr_gprs_get_scell_info();
  sys_loc_wwan_scan_result_s_type    *result;

  MSG_GERAN_HIGH_0_G("[GPS search] : Filling result for PLMN SCAN result");

  result = (sys_loc_wwan_scan_result_s_type *)modem_mem_alloc(sizeof(sys_loc_wwan_scan_result_s_type), MODEM_MEM_CLIENT_GERAN);

  if (result != NULL)
  {
    result->wwan_scan_result_per_rat.scan_ref_time = rr_get_gps_plmn_search_scan_ref_time();
    result->wwan_scan_result_per_rat.rat = SYS_RAT_GSM_RADIO_ACCESS;

    if (scell_info->gsm.camped_on_flag)
    {
      MSG_GERAN_HIGH_0_G("[GPS search] : adding scell in PLMN SCAN result");
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.num = scell_info->gsm.BCCH_ARFCN.num;
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.band = scell_info->gsm.BCCH_ARFCN.band;
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_bsic_valid = TRUE;
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.bsic = CONVERT_BSIC_TO_BYTE(scell_info->gsm.BSIC);
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_cell_timing_present = TRUE;
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_timing = scell_info->gsm.cell_timing;
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_global_cell_id_present = scell_info->gsm.cell_identity_valid_flag;
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.cell_identity = scell_info->gsm.cell_identity;
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_rx_power_meas_present = TRUE;
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.rx_power_meas.rx_power_meas_sel = SYS_POWER_MEAS_TYPE_RXLEVEL;
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.rx_power_meas.power_measurement.rxlevel = (int16) scell_info->gsm.reselection_info.RXLEV_average - 111;
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.channel_scan_result_status = SYS_SCAN_COMPLETE_SUCCESS;
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.plmn = scell_info->gsm.location_area_identification.plmn_id;
      result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.lac  = scell_info->gsm.location_area_identification.location_area_code;
      MSG_GERAN_HIGH_6_G("[GPS Search]SCell: index(%d) bsic_valid(%d) num(%u) band(%u) RxLev(%d) bsic(%d)",
                          result_index,
                          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_bsic_valid,
                          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.num,
                          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.band,
                          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.rx_power_meas.power_measurement.rxlevel,
                          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.bsic);

      result_index++;
    }

    if (wwan_scan_assistance_data_gsm.num_gsm_ad_element != 0)
    {
      for (i = 0; i < wwan_scan_assistance_data_gsm.num_gsm_ad_element && i < SYS_MAX_RESULT_PER_RAT; i++)
      {
        candidate_db_cell = rr_candidate_db_get_entry(candidate_db_ptr, wwan_scan_assistance_data_gsm.gsm_ad_elems[i].arfcn);

        if ((candidate_db_cell != NULL) && (rr_is_this_the_scell(wwan_scan_assistance_data_gsm.gsm_ad_elems[i].arfcn)))
        {
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[0].wwan_scan_result_gsm.delta_time_offset = candidate_db_cell->power_meas_timestamp - (uint32)result->wwan_scan_result_per_rat.scan_ref_time;
          continue;
        }

        if (result_index >= SYS_MAX_RESULT_PER_RAT)
        {
          MSG_GERAN_ERROR_0("[GPS search] : result_index is overflowed");
          break;
        }

        if (candidate_db_cell != NULL)
        {
          MSG_GERAN_HIGH_1_G("[GPS search] : adding ARFCN %d in PLMN SCAN result ", candidate_db_cell->meas.arfcn_num);
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.num = candidate_db_cell->meas.arfcn_num;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.band = candidate_db_cell->meas.arfcn_band;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_bsic_valid = candidate_db_cell->bsic_valid;

          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.bsic = CONVERT_BSIC_TO_BYTE(candidate_db_cell->bsic);
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_cell_timing_present = TRUE;
          if (candidate_db_cell->bsic_valid)
          {
            result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_cell_timing_present = TRUE;
            result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_timing = candidate_db_cell->meas.cell_timing;
            result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.channel_scan_result_status = SYS_SCAN_COMPLETE_SUCCESS;
          }
          else
          {
            result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_cell_timing_present = FALSE;
            result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_timing = 0;
            result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.channel_scan_result_status = SYS_SCAN_COMPLETE_NO_CELL_FOUND;
          }
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_global_cell_id_present = candidate_db_cell->si_info.cell_id_valid;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.cell_identity = candidate_db_cell->si_info.cell_id;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.delta_time_offset = candidate_db_cell->power_meas_timestamp - (uint32)result->wwan_scan_result_per_rat.scan_ref_time;

          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_rx_power_meas_present = TRUE;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.rx_power_meas.rx_power_meas_sel = SYS_POWER_MEAS_TYPE_RXLEVEL;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.rx_power_meas.power_measurement.rxlevel = (int16) candidate_db_cell->meas.RXLEV_average - 111;

          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.plmn = candidate_db_cell->si_info.lai.plmn_id;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.lac = candidate_db_cell->si_info.lai.location_area_code;
          MSG_GERAN_HIGH_6_G("[GPS Search] [AD search] : index(%d) bsic_valid(%d) num(%u) band(%u) RxLev(%d) bsic(%d)",
                              result_index,
                              result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_bsic_valid,
                              result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.num,
                              result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.band,
                              result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.rx_power_meas.power_measurement.rxlevel,
                              result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.bsic);

          result_index++;
        }
        else
        {
          MSG_GERAN_HIGH_1_G("[GPS search] : did not find ARFCN %d for PLMN SCAN result ", wwan_scan_assistance_data_gsm.gsm_ad_elems[i].arfcn.num);
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_bsic_valid = FALSE;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_cell_timing_present = FALSE;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_global_cell_id_present = FALSE;

          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_rx_power_meas_present = FALSE;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.num = wwan_scan_assistance_data_gsm.gsm_ad_elems[i].arfcn.num;
		  result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.band = wwan_scan_assistance_data_gsm.gsm_ad_elems[i].arfcn.band;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.channel_scan_result_status = SYS_SCAN_COMPLETE_NO_CELL_FOUND;
          result_index++;
        }
      }
    }
    else
    {
      MSG_GERAN_HIGH_1_G("[GPS search] [Blind scan] : total ARFCN %d in candidate db", candidate_db_ptr->num_frequencies);
      for(i = 0; i < candidate_db_ptr->num_frequencies && i < SYS_MAX_RESULT_PER_RAT; i++)
      {
        ARFCN_T meas_arfcn;

        meas_arfcn.num = candidate_db_ptr->candidate_cells[i].meas.arfcn_num;
        meas_arfcn.band = candidate_db_ptr->candidate_cells[i].meas.arfcn_band;

        if (rr_is_this_the_scell(meas_arfcn))
        {
          continue;
        }

        if (result_index >= SYS_MAX_RESULT_PER_RAT)
        {
          MSG_GERAN_ERROR_0("[GPS search] : result_index is overflowed");
          break;
        }

        MSG_GERAN_HIGH_1_G("[GPS search] [Blind scan] : adding ARFCN %d in PLMN SCAN result ", candidate_db_ptr->candidate_cells[i].meas.arfcn_num);
        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.num = candidate_db_ptr->candidate_cells[i].meas.arfcn_num;
        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.band = candidate_db_ptr->candidate_cells[i].meas.arfcn_band;
        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_bsic_valid = candidate_db_ptr->candidate_cells[i].bsic_valid;
        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.bsic = CONVERT_BSIC_TO_BYTE(candidate_db_ptr->candidate_cells[i].bsic);
        if (candidate_db_ptr->candidate_cells[i].bsic_valid)
        {
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_cell_timing_present = TRUE;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_timing = candidate_db_ptr->candidate_cells[i].meas.cell_timing;
        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.channel_scan_result_status = SYS_SCAN_COMPLETE_SUCCESS;
        }
        else
        {
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_cell_timing_present = FALSE;
          result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_timing = 0;
        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.channel_scan_result_status = SYS_SCAN_COMPLETE_NO_CELL_FOUND;
        }
        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_global_cell_id_present = candidate_db_ptr->candidate_cells[i].si_info.cell_id_valid;
        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.cell_identity = candidate_db_ptr->candidate_cells[i].si_info.cell_id;
        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.delta_time_offset = candidate_db_ptr->candidate_cells[i].power_meas_timestamp - (uint32)result->wwan_scan_result_per_rat.scan_ref_time;

        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_rx_power_meas_present = TRUE;
        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.rx_power_meas.rx_power_meas_sel = SYS_POWER_MEAS_TYPE_RXLEVEL;
        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.rx_power_meas.power_measurement.rxlevel = (int16)candidate_db_ptr->candidate_cells[i].meas.RXLEV_average - 111;

        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.plmn = candidate_db_ptr->candidate_cells[i].si_info.lai.plmn_id;
        result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.cell_info.rat_cell.gsm_cell.lac = candidate_db_ptr->candidate_cells[i].si_info.lai.location_area_code;
        MSG_GERAN_HIGH_6_G("[GPS Search]blind search: index(%d) bsic_valid(%d) num(%u) band(%u) RxLev(%d) bsic(%d)",
                             result_index,
                             result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.is_bsic_valid,
                             result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.num,
                             result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.arfcn.band,
                             result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.rx_power_meas.power_measurement.rxlevel,
                             result->wwan_scan_result_per_rat.wwan_scan_result_per_rat[result_index].wwan_scan_result_gsm.bsic);

        result_index++;
      }
    }

    result->wwan_scan_result_per_rat.num_scan_result_per_rat = result_index;

    if (result->wwan_scan_result_per_rat.num_scan_result_per_rat > 0)
    {
      result->wwan_scan_result_per_rat.scan_status_per_rat = SYS_PLMN_LIST_SUCCESS;
    }
	else
	{
	  result->wwan_scan_result_per_rat.scan_status_per_rat = SYS_PLMN_LIST_REJ_IN_RLF;
	}


    *dfl = result;
    MSG_GERAN_HIGH_1_G("[GPS search] : dfl length of result - %d for PLMN SCAN result ", (*dfl)->wwan_scan_result_per_rat.num_scan_result_per_rat);

    ref_cnt_obj_init(*dfl, rr_sys_loc_wwan_scan_result_destructor);
  }
  else
  {
     MSG_GERAN_ERROR_0_G("[GPS search] Memory allocation to Result failed for PLMN list confirmation");
  }

}
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */

/* EOF */


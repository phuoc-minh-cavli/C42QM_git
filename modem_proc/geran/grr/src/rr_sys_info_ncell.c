/*! \file rr_sys_info_si.c

  Contains system information functionality related to Neighbour Cells.

                Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_sys_info_ncell.c#6 $ */
/* $DateTime: 2018/11/28 04:48:52 $$Author: pwbldsvc $ */

/*****************************************************************************

Notes on BA, BA(GPRS) and GSM Neighbour Cell List
-------------------------------------------------

The BA list is derived in RR when SI2, SI2bis, etc, are received.

The BA(GPRS) list is derived in GRR from the GSM Neighbour Cell List.

The GSM Neighbour Cell List is derived in GRR, and is based on the BA list given in SI2
(and variants).
The GSM Neighbour Cell List is also deviated by the reception of a PMO or PCCO
message which contains added or removed ncells.

*****************************************************************************/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_sys_info_ncell.h"
#include "rr_sys_info_pcell.h"
#include "rr_general.h"
#include "rr_gprs_defs.h"
#include "csn.h"
#include "rr_public_store.h"
#include "rr_shared_cell_selection.h"
#include "rr_l1_send.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_sys_info_i.h"
#include "rr_resel_utils.h"
#include "rr_process_pmo_pcco.h"
#include "rr_control.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "rr_log.h"
#include "rr_ded_meas_reporting.h"
#include "rr_ps_domain.h"
#include "rr_multi_sim.h"
#include "rr_sys_info_cache.h"
#include "rr_arfcn.h"
#include "geran_grr_dyn_api.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*Grr is waiting for a surround read response*/
typedef struct
{
 boolean waiting_flag;
 ARFCN_T arfcn;
 uint16  surround_success;
 uint16  surround_failure;
 boolean from_cache;
} set_waiting_for_surround_response_t;

typedef struct
{
  set_waiting_for_surround_response_t waiting_for_surround_response;
} rr_sys_info_ncell_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
#define _RR_SYS_INFO_NCELL_DATA_INIT \
   {FALSE, {INVALID_ARFCN, (sys_band_T)BAND_NONE}, 0, 0, FALSE}

static rr_sys_info_ncell_data_t rr_sys_info_ncell_data =
                       INITIAL_VALUE_S(_RR_SYS_INFO_NCELL_DATA_INIT) ;
#undef _RR_SYS_INFO_NCELL_DATA_INIT

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 *
 * \return rr_sys_info_ncell_data_t*
 */
static rr_sys_info_ncell_data_t *rr_sys_info_ncell_get_data_ptr( void )
{
  return &rr_sys_info_ncell_data;
}

/*!
 * \brief Scans the GSM Neighbour Cell List to find the next ncell entry of the indicated type.
 *
 *  Scans the GSM Neighbour Cell List to find the next ncell entry of the indicated type, starting at the
 *  index supplied. If a ncell is found, the index is updated and the function returns TRUE.
 *
 * \param index (in/out)   - starting index
 * \param bcch_or_pmo (in) - source type
 *
 * \return boolean - TRUE if a ncell was found; FALSE otherwise
 */
static boolean rr_gprs_find_next_ncell_index(
  uint8 *index,
  bcch_or_pmo_format_t bcch_or_pmo
   
)
{
  gprs_ncells_info_t * ncells_info = rr_gprs_get_ncells_info();
  gsm_neighbour_cell_list_t * gsm_ncell_list = &ncells_info->gsm_neighbour_cell_list;

  if ( index && ( (* index)++ < gsm_ncell_list->num_of_ncells ) )
  {
    gsm_ncell_list_member_t * ncell;

    uint8 i = (* index);

    while ( i < gsm_ncell_list->num_of_ncells )
    {
      ncell = &gsm_ncell_list->ncells[i];

      if ( ( ( ncell->status == NCELL_ORIGINAL ) ||
             ( ncell->status == NCELL_ADDED ) ) &&
           ( ncell->bcch_or_pmo == bcch_or_pmo ) )
      {
        (* index) = i;
        return TRUE;
      }

      i++;
    }
  }

  return FALSE;

} /* end rr_gprs_find_next_ncell_index() */

/*!
 * \brief Scans the GSM Neighbour Cell List to find the first ncell entry of the indicated type.
 *
 *  Scans the GSM Neighbour Cell List to find the first ncell entry of the indicated type, starting at the
 *  index supplied. If a ncell is found, the index is updated and the returns TRUE.
 *
 * \param index (in/out) - index
 * \param bcch_or_pmo (in) - source type of cell
 *
 * \return boolean - TRUE if a ncell was found; FALSE otherwise
 */
static boolean rr_gprs_find_first_ncell_index(
  uint8 *index,
  bcch_or_pmo_format_t bcch_or_pmo
   
)
{
  gprs_ncells_info_t * ncells_info = rr_gprs_get_ncells_info();
  gsm_neighbour_cell_list_t * gsm_ncell_list = &ncells_info->gsm_neighbour_cell_list;

  if ( index && ( (* index) < gsm_ncell_list->num_of_ncells ) )
  {
    gsm_ncell_list_member_t * ncell;

    uint8 i = (* index);

    while ( i < gsm_ncell_list->num_of_ncells && i < MAX_SIZE_OF_GSM_NEIGHBOUR_CELL_LIST )
    {
      ncell = &gsm_ncell_list->ncells[i];

      if ( ( ( ncell->status == NCELL_ORIGINAL ) ||
             ( ncell->status == NCELL_ADDED ) ) &&
           ( ncell->bcch_or_pmo == bcch_or_pmo ) )
      {
        (* index) = i;
        return TRUE;
      }

      i++;
    }
  }

  return FALSE;

} /* end rr_gprs_find_next_ncell_index() */

/*!
 * \brief This function is called to notify scell sys info handling that a ncell SI acquisition is required.
 *
 *  This function is called to notify scell sys info handling that a ncell SI acquisition is required.
 *  If this is for a high priority ncell, then any scell SI acquisition is suspended to allow the ncell SI
 *  acquisition to complete.
 *  If this is for a lower priority ncell, if a scell SI acquisition is in progress, the ncell SI acquisition
 *  is not allowed.
 *
 * \param high_priority (in) - set to TRUE if the ncell is high priority.
 *
 * \return boolean - TRUE if the ncell SI acquisition is allowed; FALSE otherwise
 */
static boolean nsi_notify_ncell_si_acquisition_required(
  boolean high_priority
   
)
{
  boolean ncell_si_acquisition_allowed;

  if ( ( high_priority ) ||
       ( ! rr_gprs_scell_sys_info_acquisition_in_progress() ) )
  {
    ncell_si_acquisition_allowed = TRUE;

    /* Suspend sys info handling, with auto retry */
    rr_gprs_suspend_camped_sys_info_handling(TRUE);
  }
  else
  {
    ncell_si_acquisition_allowed = FALSE;
  }

  return ncell_si_acquisition_allowed;

} /* end nsi_notify_ncell_si_acquisition_required() */

/*!
 * \brief This function returns the index of the arfcn passed in into the BA(GPRS) list.
 *
 *  This function returns the index of the arfcn passed in into the BA(GPRS) list, and a pointer to its
 *  member details structure.
 *  Either the ARFCN only, or an ARFCN+BSIC pair can be used to find a ncell in the list.
 *  If only the ARFCN is supplied, a ncell with a matching ARFCN, regardless of BSIC, will be returned.
 *  If ARFCN+BSIC are supplied, then if the ncell details include a BSIC, both ARFCN+BSIC must match. If the
 *  ncell details don't include a BSIC, only the ARFCN is relevant.
 *  If a ncell is duplicated in the database, the last added will be returned.
 *
 * \param arfcn (in)
 * \param bsic_ptr (in)        - BSIC (optional). Set to NULL if not supplied.
 * \param matching_ncell (out) - address of pointer, which is updated to point to the matching ncell details.
 *
 * \return uint8 - index
 */
static uint8 find_gsm_ncell_list_ncell(
  ARFCN_T arfcn,
  BSIC_T *bsic_ptr,
  gsm_ncell_list_member_t **matching_ncell
   
)
{
  uint8  index;
  gsm_ncell_list_member_t  * ncell;
  BSIC_T bsic = {0};
  gprs_ncells_info_t  * ncells = NULL;
  gsm_neighbour_cell_list_t  * gsm_ncell_list = NULL;
  uint8 nc_mode = rr_gprs_get_nc_mode();

  ncells = rr_gprs_get_ncells_info();
  gsm_ncell_list = &ncells->gsm_neighbour_cell_list;

  (* matching_ncell) = NULL;

  if( bsic_ptr )
  {
    bsic = (* bsic_ptr);
  }

  for ( index = 0; index < gsm_ncell_list->num_of_ncells; index++ )
  {
    ncell = &gsm_ncell_list->ncells[index];

    if (ARFCNS_EQUAL(ncell->arfcn, arfcn) &&
         ( ( bsic_ptr == NULL )
           ||
           ( ncell->bsic_valid && SAME_BSIC( ncell->bsic, bsic ) )
           ||
           ( (!ncell->bsic_valid) && (ncell->bcch_or_pmo == BCCH_FORMAT) )
           )
         &&
         ( ( ncell->status == NCELL_ORIGINAL ) ||
           ( ncell->status == NCELL_ADDED )    ||
           ( ( ncell->status == NCELL_ADDED_NC2_ONLY ) &&
             ( nc_mode == NC_MEAS_NC2 ) ) ) )
    {
      (* matching_ncell) = ncell;
      break;
    }
    else
    if ( ARFCNS_EQUAL(ncell->arfcn, arfcn) &&
         ( bsic_ptr != NULL ) &&
         ( ncell->bcch_or_pmo == PMO_FORMAT ) &&
         !SAME_BSIC(ncell->bsic, bsic) )
    {
      MSG_GERAN_MED_2_G("PBCCH BSIC heteroclitical exp:%d rec:%d",
              CONVERT_FROM_BSIC_T(ncell->bsic),CONVERT_FROM_BSIC_T(bsic));
    }
  }

  return index;

} /* end find_gsm_ncell_list_ncell_index() */

/*!
 * \brief Scans the GSM Neighbour Cell List to find the next PSI ncell entry, starting at the index supplied.
 *
 *  Scans the GSM Neighbour Cell List to find the next PSI ncell entry, starting at the index supplied. If a
 *  PSI ncell is found, the index is updated and the function returns TRUE.
 *
 * \param index (in/out) - start index / index of the ncell.
 *
 * \return boolean - TRUE if a ncell is found; FALSE otherwise.
 */
static boolean rr_gprs_find_next_pmo_pcco_ncell_index(
  uint8 *index
   
)
{
  return rr_gprs_find_next_ncell_index(index, PMO_FORMAT);

} /* end rr_gprs_find_next_pmo_pcco_ncell_index() */

/*!
 * \brief Scans the GSM Neighbour Cell List to find the next BCCH ncell entry, starting at the index supplied.
 *
 *  Scans the GSM Neighbour Cell List to find the next BCCH ncell entry, starting at the index supplied. If a
 *  BCCH ncell is found, the index is updated and the function returns TRUE.
 *
 * \param index (in/out) - start index / index of the ncell.
 *
 * \return boolean - TRUE if a ncell is found; FALSE otherwise.
 */
static boolean rr_gprs_find_next_bcch_ncell_index(
  uint8 *index
   
)
{
  return rr_gprs_find_next_ncell_index(index, BCCH_FORMAT);

} /* end rr_gprs_find_next_bcch_ncell_index() */

/*!
 * \brief  Marks a GSM Neighbour Cell List entry as unallocated.
 *
 * \param ncell (in/out)
 */
static void unallocate_gsm_ncell_list_member(
  gsm_ncell_list_member_t *ncell
   
)
{
  //RR_NULL_CHECK_RETURN_VOID(ncell);

  ncell->status = NCELL_UNALLOCATED;
  ncell->arfcn.num = INVALID_ARFCN;
  ncell->arfcn.band = (sys_band_T)BAND_NONE;

  ncell->bsic_valid = FALSE;

} /* end unallocate_gsm_ncell_list_member() */

/*!
 * \brief Adds a ncell to the GSM Neighbour Cell List.
 *
 *  Adds a ncell to the GSM Neighbour Cell List. This ncell could be from sys info from SI2, SI2+SI2quater,
 *  a PMO or a PCCO.
 *  The origin of the data is indicated by 'addition_reason', and the combination of parameters supplied.
 *
 * \param addition_reason (in)            - origin of the data {NCELL_ORIGINAL, NCELL_ADDED}
 * \param bcch_arfcn (in)                 - ARFCN
 * \param bsic_ptr (in)                   - BSIC
 * \param bcch_or_pmo (in)                - origin of the data {BCCH_FORMAT, PMO_FORMAT}
 * \param gprs_cell_selection_params (in) - GPRS Cell Selection Parameters (optional)
 * \param bsic_description (in)           - BSIC Description
 */
static void add_ncell_to_gsm_neighbour_cell_list(
  ncell_list_member_status_t addition_reason,
  ARFCN_T bcch_arfcn,
  BSIC_T * bsic_ptr,
  bcch_or_pmo_format_t bcch_or_pmo,
  gprs_cell_selection_params_t *gprs_cell_selection_params,
  gsm_bsic_description_entry_T *bsic_description
   
)
{
  gprs_ncells_info_t         * ncells = NULL;
  gsm_neighbour_cell_list_t  * gsm_ncell_list = NULL;

  ncells = rr_gprs_get_ncells_info();
  gsm_ncell_list = &ncells->gsm_neighbour_cell_list;

  /* Check the ncell list is not full */
  if ( gsm_ncell_list->num_of_ncells < MAX_SIZE_OF_GSM_NEIGHBOUR_CELL_LIST )
  {
    gsm_ncell_list_member_t  * ncell;

    (void) find_gsm_ncell_list_ncell(bcch_arfcn, bsic_ptr, &ncell);

    if ( ncell )
    {
      if ( ncell->bcch_or_pmo == BCCH_FORMAT )
      {
        ncell->ncell_data.bcch.system_info_timer_count = UNINITIALISED_SYS_INFO_COUNT;
      }

      ncell->status = NCELL_DUPLICATE;
    }

    /* Point to next available index */
    ncell = &gsm_ncell_list->ncells[gsm_ncell_list->num_of_ncells];

    /* Clear out any existing data */
    memset(ncell,0, sizeof(gsm_ncell_list_member_t));

    /* Store the ARFCN - this is always supplied */
    ncell->arfcn = bcch_arfcn;

    /* Store the BSIC - this may be omitted if the data origin is SI2-only */
    if ( bsic_ptr != NULL)
    {
      ncell->bsic_valid = TRUE;
      ncell->bsic = (* bsic_ptr);
    }
    else
    {
      ncell->bsic_valid = FALSE;
    }

    ncell->bcch_or_pmo = bcch_or_pmo;

     /* Check if the data origin is PMO/PCCO */
    if (bcch_or_pmo == PMO_FORMAT)
    {
      /* These parameters may not be present in a PMO or PCCO. This indicates this ncell */
      /* should not be used unless the NC meas mode is 2 */
      if (gprs_cell_selection_params)
      {
        if (bsic_ptr != NULL)
        {
          ncell->ncell_data.pmo_pcco.bsic = (* bsic_ptr);
        }
        else
        {
          MSG_GERAN_ERROR_0_G("Warning: BSIC ptr not valid for PBCCH");
        }
        ncell->ncell_data.pmo_pcco.gprs_cell_selection_params = (* gprs_cell_selection_params);
      }
      else
      {
        /* Change the ncell status to 'NCELL_ADDED_NC2_ONLY', which is used later to determine */
        /* if this ncell is included in the GSM Neighbour Cell List or not */
        addition_reason = NCELL_ADDED_NC2_ONLY;
      }
    }
    else
    {
      ncell->ncell_data.bcch.system_info_timer_count = UNINITIALISED_SYS_INFO_COUNT;

      if (bsic_description != NULL)
      {
        ncell->ncell_data.bcch.bsic_description = (* bsic_description);
      }
    }

    /* Store the ncell member status. This will be as supplied, apart for NCELL_ADDED_NC2_ONLY */
    ncell->status = addition_reason;

    /* Increment the total ncell count */
    gsm_ncell_list->num_of_ncells++;
  }
  else
  {
    MSG_GERAN_LOW_0_G("GSM Neighbour Cell List overflow!");
  }

} /* end add_ncell_to_gsm_neighbour_cell_list() */

/*!
 * \brief Removes the indicated neighbour cell from the GSM Neighbour Cell List.
 *
 *  Removes the indicated neighbour cell from the GSM Neighbour Cell List. The ncell and associated data is
 *  either removed (i.e. hidden), or removed permanently, depending on the setting of 'delete_permanently'.
 *
 * \param index (in) - index of the ncell.
 * \param delete_permanently (in) - set TRUE if the data is to be deleted permanently; FALSE otherwise.
 */
static void remove_ncell_from_gsm_neighbour_cell_list_by_index(
  uint8 index,
  boolean delete_permanently
   
)
{
  gprs_ncells_info_t        *ncells = NULL;
  gsm_neighbour_cell_list_t *gsm_ncell_list = NULL;

  ncells = rr_gprs_get_ncells_info();
  gsm_ncell_list = &ncells->gsm_neighbour_cell_list;

  if (index >= MAX_SIZE_OF_GSM_NEIGHBOUR_CELL_LIST)
  {
    MSG_GERAN_LOW_1_G("GSM ncell index out of range %d", (int)index);
    return;
  }

  if ( delete_permanently )
  {
    unallocate_gsm_ncell_list_member(&gsm_ncell_list->ncells[index]);
  }
  else
  {
    gsm_ncell_list->ncells[index].status = NCELL_REMOVED;
  }

} /* remove_ncell_from_gsm_neighbour_cell_list_by_index() */

#if 0  // Not used at present
/*!
 * \brief Removes the indicated neighbour cell from the GSM Neighbour Cell List.
 *
 *  Removes the indicated neighbour cell from the GSM Neighbour Cell List.
 *  Note: The ncell and associated data is removed permanently, and cannot be retrieved.
 *
 * \param bcch_arfcn
 * \param bsic
 */
void remove_ncell_from_gsm_neighbour_cell_list_by_arfcn(
  ARFCN_T bcch_arfcn,
  BSIC_T *bsic
   
)
{
  uint8 index;
  gsm_ncell_list_member_t *gsm_ncell_list_member;

  index = find_gsm_ncell_list_ncell(bcch_arfcn, bsic, &gsm_ncell_list_member);

  remove_ncell_from_gsm_neighbour_cell_list_by_index(index, TRUE);

} /* remove_ncell_from_gsm_neighbour_cell_list_by_arfcn() */
#endif // #if 0

/*!
 * \brief Invalidates the sys info data for the indicated ncell.
 *
 * \param index (in) - index into the GSM Neighbour Cell List.
 */
static void invalidate_surround_ncell_sys_info_by_index(
  uint8 index
   
)
{
  if (index < MAX_SIZE_OF_GSM_NEIGHBOUR_CELL_LIST)
  {
    gprs_ncells_info_t        *ncells_info = rr_gprs_get_ncells_info();
    gsm_neighbour_cell_list_t *gsm_ncell_list = &ncells_info->gsm_neighbour_cell_list;
    gsm_ncell_list_member_t   *ncell = &gsm_ncell_list->ncells[index];

    if (ncell->bcch_or_pmo == BCCH_FORMAT)
    {
      ncell->ncell_data.bcch.gsm.valid_data_flag = FALSE;
      ncell->ncell_data.bcch.gprs.si13_data_valid_flag = FALSE;
    }
  }

  return;
} /* end invalidate_surround_ncell_sys_info() */

/*!
 * \brief Iterates over all entries in the list, reassigning the band according to the band indicator.
 *
 * \param list (in)     - list of ARFCNs.
 * \param band_ind (in) - band indicator
 */
static void rr_bcch_frequency_list_reassign_bands(
  BCCH_frequency_list_T *list,
  si_psi_band_ind_T      band_ind
)
{
  uint32 i;

  for (i = 0; i < list->no_of_entries && i < BA_MAX_LEN; ++i)
  {
    list->BCCH_ARFCN[i].band = (sys_band_T) rr_get_band_using_band_indicator(
      list->BCCH_ARFCN[i].num,
      band_ind
    );
  }
}

/*-----------------------------------------------------------------------------
 * Externalized Function Definitions
 * --------------------------------------------------------------------------*/

/*!
 * \brief Increments either the success or failure surround response metrics.
 *
 * \param success (in)
 */
void set_waiting_for_surround_response_metrics(
  boolean success
   
)
{
  // Obtain a pointer to the module data
  rr_sys_info_ncell_data_t *rr_sys_info_ncell_data_ptr = rr_sys_info_ncell_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sys_info_ncell_data_ptr);

  if (success)
  {
    rr_sys_info_ncell_data_ptr->waiting_for_surround_response.surround_success++;
  }
  else
  {
    rr_sys_info_ncell_data_ptr->waiting_for_surround_response.surround_failure++;
  }

  if( (rr_sys_info_ncell_data_ptr->waiting_for_surround_response.surround_success == 0xffff) ||
      (rr_sys_info_ncell_data_ptr->waiting_for_surround_response.surround_failure == 0xffff) )
  {
    rr_sys_info_ncell_data_ptr->waiting_for_surround_response.surround_success = 0;
    rr_sys_info_ncell_data_ptr->waiting_for_surround_response.surround_failure = 0;
  }
}

/*!
 * \brief Returns the waiting_for_surround_response flag.
 *
 *
 * \return boolean
 */
boolean get_waiting_for_surround_response(
   
)
{
  // Obtain a pointer to the module data
  rr_sys_info_ncell_data_t *rr_sys_info_ncell_data_ptr = rr_sys_info_ncell_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sys_info_ncell_data_ptr);

  return rr_sys_info_ncell_data_ptr->waiting_for_surround_response.waiting_flag;
}

/*!
 * \brief Sets the waiting_for_surround_response flag.
 *
 * \param arfcn (in)
 * \param si_from_cache (in)
 */
void set_waiting_for_surround_response(
  ARFCN_T arfcn,
  boolean si_from_cache
   
)
{
  // Obtain a pointer to the module data
  rr_sys_info_ncell_data_t *rr_sys_info_ncell_data_ptr = rr_sys_info_ncell_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sys_info_ncell_data_ptr);

  rr_sys_info_ncell_data_ptr->waiting_for_surround_response.waiting_flag = TRUE;
  rr_sys_info_ncell_data_ptr->waiting_for_surround_response.arfcn        = arfcn;
  rr_sys_info_ncell_data_ptr->waiting_for_surround_response.from_cache   = si_from_cache;

  return;
}

/*!
 * \brief Updates the SI cache flag for the specified ARFCN
 *
 * \param arfcn (in)
 * \param si_from_cache (in)

 */
void mark_surround_response_as_cached(ARFCN_T arfcn, boolean si_from_cache  )
{
  {
    // Obtain a pointer to the module data
    rr_sys_info_ncell_data_t *ncell_data_ptr = rr_sys_info_ncell_get_data_ptr();
    //RR_NULL_CHECK_FATAL(ncell_data_ptr);

    /* if a surround response is pending for this ARFCN, update its cache flag */
    if (ARFCNS_EQUAL(ncell_data_ptr->waiting_for_surround_response.arfcn, arfcn) &&
        ncell_data_ptr->waiting_for_surround_response.waiting_flag)
    {
      ncell_data_ptr->waiting_for_surround_response.from_cache = si_from_cache;
    }
  }

  return;
}

/*!
 * \brief Clears the waiting_for_surround_response flag.
 *
 */
void clear_waiting_for_surround_response(
   
)
{
  // Obtain a pointer to the module data
  rr_sys_info_ncell_data_t *rr_sys_info_ncell_data_ptr = rr_sys_info_ncell_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sys_info_ncell_data_ptr);

  rr_sys_info_ncell_data_ptr->waiting_for_surround_response.waiting_flag = FALSE;
  rr_sys_info_ncell_data_ptr->waiting_for_surround_response.arfcn.num = INVALID_ARFCN;
  rr_sys_info_ncell_data_ptr->waiting_for_surround_response.arfcn.band = (sys_band_T)BAND_NONE;
  rr_sys_info_ncell_data_ptr->waiting_for_surround_response.from_cache = FALSE;

  return;
}

/*!
 * \brief Prints the parameters in the waiting_for_surround_response structure.
 *
 */
void print_waiting_for_surround_response(
   
)
{
  uint32 total;

  // Obtain a pointer to the module data
  rr_sys_info_ncell_data_t *rr_sys_info_ncell_data_ptr = rr_sys_info_ncell_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sys_info_ncell_data_ptr);


  MSG_GERAN_LOW_3_G("Surr Up stats:%d %d %d",rr_sys_info_ncell_data_ptr->waiting_for_surround_response.waiting_flag,
                                    CONVERT_FROM_ARFCN_T(rr_sys_info_ncell_data_ptr->waiting_for_surround_response.arfcn),
                                    0);


  total = rr_sys_info_ncell_data_ptr->waiting_for_surround_response.surround_failure;
  total = total + rr_sys_info_ncell_data_ptr->waiting_for_surround_response.surround_success;

  MSG_GERAN_LOW_2_G("SU total:%d fail:%d",total,rr_sys_info_ncell_data_ptr->waiting_for_surround_response.surround_failure);
}

/*!
 * \brief Returns TRUE if the ARFCN passed in matches the one stored in waiting_for_surround_response.
 *
 * \param arfcn (in)
 *
 * \return boolean - TRUE if the ARFCN matches; FALSE otherwise
 */
boolean match_waiting_for_surround_response(
  ARFCN_T arfcn
   
)
{
  // Obtain a pointer to the module data
  rr_sys_info_ncell_data_t *rr_sys_info_ncell_data_ptr = rr_sys_info_ncell_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sys_info_ncell_data_ptr);

  if (rr_sys_info_ncell_data_ptr->waiting_for_surround_response.waiting_flag)
  {
    return ARFCNS_EQUAL(rr_sys_info_ncell_data_ptr->waiting_for_surround_response.arfcn, arfcn);
  }
  else
  {
    return FALSE;
  }
}

/*!
 * \brief Returns TRUE if the ARFCN passed in matches the one stored in waiting_for_surround_response.
 *
 * \param arfcn (in)
 * \param served_by_cache_ptr (out)
 *
 * \return boolean - TRUE if the ARFCN matches; FALSE otherwise
 */
boolean match_surround_response_with_cache_check(ARFCN_T arfcn, boolean *served_by_cache_ptr )
{
  if (match_waiting_for_surround_response(arfcn))
  {
    // Obtain a pointer to the module data
    rr_sys_info_ncell_data_t *rr_sys_info_ncell_data_ptr = rr_sys_info_ncell_get_data_ptr();

    if ((rr_sys_info_ncell_data_ptr != NULL) && (served_by_cache_ptr != NULL))
    {
      *served_by_cache_ptr = rr_sys_info_ncell_data_ptr->waiting_for_surround_response.from_cache;
    }
    return(TRUE);
  }

  return(FALSE);
}

/*!
 * \brief Resets the timers for cells that have already requested system info but got no response.
 *
 *  Resets the timers for cells that have already requested system info but got no response. The strongest
 *  in the list will be re-requested.
 *
 */
void rr_gprs_reset_ncell_sysinfo_manager(
   
)
{
  gsm_ncell_list_member_t * ncell = NULL;
  gprs_ncells_info_t  * ncells = rr_gprs_get_ncells_info();
  gsm_neighbour_cell_list_t  * gsm_ncell_list = &ncells->gsm_neighbour_cell_list;
  uint8 ncell_index = 0;

  // Get the first ncell and then scan from there
  if (rr_gprs_find_first_ncell_index(&ncell_index, BCCH_FORMAT))
  {
    ncell = &gsm_ncell_list->ncells[ncell_index];

    if ( (ncell->ncell_data.bcch.system_info_timer_count != UNINITIALISED_SYS_INFO_COUNT) &&
         (ncell->ncell_data.bcch.gsm.valid_data_flag == FALSE) )
    {
      ncell->ncell_data.bcch.system_info_timer_count = UNINITIALISED_SYS_INFO_COUNT;
    }

    // now for the remaining cells
    while (rr_gprs_find_next_bcch_ncell_index(&ncell_index))
    {
      ncell = &gsm_ncell_list->ncells[ncell_index];

      if ( (ncell->ncell_data.bcch.system_info_timer_count != UNINITIALISED_SYS_INFO_COUNT) &&
           (ncell->ncell_data.bcch.gsm.valid_data_flag == FALSE) )
      {
        ncell->ncell_data.bcch.system_info_timer_count = UNINITIALISED_SYS_INFO_COUNT;
      }
    }
  }
} /* end rr_gprs_reset_ncell_sysinfo_manager() */

/*!
 * \brief Manages requests for ncell SI reads.
 *
 * \param bcch_ncell_info (in)
 * \param cancel_non_drx (in)
 */
void rr_gprs_ncell_bcch_read_manager(
  gsm_ncell_list_member_t *bcch_ncell_info,
  boolean cancel_non_drx
   
)
{
  boolean si_from_cache;

  //RR_NULL_CHECK_RETURN_VOID(bcch_ncell_info);

  /*******************************************************/
  /* GRR manages the ncell bcch reads from this function */
  /* Don't do any if we are in Transfer and in GMM Idle  */
  /* as this is an attach message                        */
  /*******************************************************/
  if ( !((rr_macinfo_get_mac_status() == MAC_TRANSFER) &&
         (rr_gmminfo_get_gmm_status() == GMM_IDLE_STATE)) )
  {
    /*
    Only ask for surround read if
    1. The sys info count is reset
    2. GRR is not currently waiting for a response for a previous request from L1
    3. GRR is not trying to get the periodic scell sys infos. Telling the scell sys info
       handler that this is a high priority cell will suspend this scell reading. This is indicated
       by cancel_non_drx.
    */
    if ( (bcch_ncell_info->ncell_data.bcch.system_info_timer_count == UNINITIALISED_SYS_INFO_COUNT) &&
          !get_waiting_for_surround_response() &&
          nsi_notify_ncell_si_acquisition_required(cancel_non_drx) &&
          !grr_get_l1_in_test_mode() )
    {
      gprs_surround_data_t gprs_surround_data;

      /* Allows 15 secs to get some sort of L1 response otherwise will try again */
      bcch_ncell_info->ncell_data.bcch.system_info_timer_count = SYSTEM_INFO_20_SEC_RETRY;

      CONVERT_TO_ARFCN_T(gprs_surround_data.bcch_params.bcch_arfcn, bcch_ncell_info->arfcn);
      gprs_surround_data.bcch_params.si13_normal_or_ext = bcch_ncell_info->ncell_data.bcch.gprs.si13_position;
      gprs_surround_data.bcch_params.si_requested = GRR_L1_DECODE_SI3_ONLY;

      si_from_cache = rr_send_mph_gprs_surround_update_req(&gprs_surround_data, cancel_non_drx, &(bcch_ncell_info->ncell_data.bcch));

      /* Flag that we are awaiting a response from L1 for a surround read */
      set_waiting_for_surround_response(bcch_ncell_info->arfcn, si_from_cache);
    }
  }

  return;
}

/*!
 * \brief Initialises the neighbour cell database. The best six ncells stored in reselection are reset.
 *
 */
void rr_init_ncell_data(
   
)
{
  rr_gprs_init_gsm_neighbour_cell_list();
  rr_gprs_init_best_six();

  return;
} /* end rr_init_ncell_data() */

/*!
 * \brief Clears the GSM Neighbour Cell List.
 *
 */
void rr_gprs_init_gsm_neighbour_cell_list(
   
)
{
  uint8  i;
  gsm_ncell_list_member_t     *ncell;
  gprs_ncells_info_t          *ncells = rr_gprs_get_ncells_info();
  gsm_neighbour_cell_list_t   *gsm_ncell_list = NULL;

  gsm_ncell_list = &ncells->gsm_neighbour_cell_list;

  for ( i = 0; i < MAX_SIZE_OF_GSM_NEIGHBOUR_CELL_LIST; i++ )
  {
    ncell = &gsm_ncell_list->ncells[i];

    unallocate_gsm_ncell_list_member(ncell);
  }

  gsm_ncell_list->num_of_ncells = 0;

} /* rr_gprs_init_gsm_neighbour_cell_list() */

/*!
 * \brief Checks if a ncell exists in the best-six ncell list.
 *
 *  Checks if a ncell exists in the best-six ncell list.
 *  Note: If the ncell exists in the Neighbour Cell List, but is not part of the best-six this will not
 *  return a match. Use rr_is_this_a_ncell() for this.
 *
 * \param bcch_arfcn (in)
 *
 * \return boolean - TRUE if a match; FALSE otherwise.
 */
boolean rr_gprs_is_this_a_surround_ncell(
  ARFCN_T bcch_arfcn
   
)
{
  return (rr_gprs_get_best_six_by_arfcn(bcch_arfcn, NULL) != NULL);

} /* end rr_gprs_is_this_a_surround_ncell() */

/*!
 * \brief Checks if a ncell exists in either the BCCH or PMO ncell lists.
 *
 *  Checks if a ncell exists in either the BCCH or PBCCH ncell lists, returning TRUE if so.
 *  If the optional parameter data_format is supplied, this is set to indicate whether the ncell data
 *  from a BCCH scan, or from a PMO / PCCO.
 *
 * \param bcch_arfcn (in)
 * \param data_format (out) - updated to indicate the origin of the cell (optional).
 *
 * \return boolean - TRUE if a match; FALSE otherwise.
 */
boolean rr_is_this_a_ncell(
  ARFCN_T bcch_arfcn,
  bcch_or_pmo_format_t *data_format
   
)
{
  gsm_ncell_list_member_t  * matching_ncell;

  (void) find_gsm_ncell_list_ncell(bcch_arfcn, NULL, &matching_ncell);

  if (matching_ncell)
  {
    if (data_format != NULL)
    {
      (* data_format) = matching_ncell->bcch_or_pmo;
    }
  }

  return (matching_ncell != NULL);

} /* end rr_is_this_a_ncell() */

/*!
 * \brief Indicates if the total number of ncells stored in the GSM Neighbour Cell List is zero.
 *
 *
 * \return boolean - TRUE if the number of cells is zero; FALSE if non-zero
 */
boolean rr_gprs_gsm_neighbour_cell_list_is_empty(
   
)
{
  gprs_ncells_info_t *ncell_info_ptr = rr_gprs_get_ncells_info();

  return (ncell_info_ptr->gsm_neighbour_cell_list.num_of_ncells == 0);

} /* end rr_gprs_gsm_neighbour_cell_list_is_empty() */

/*!
 * \brief Indicates if there are any PMO/PCCO ncells in the GSM Neighbour Cell List.
 *
 *
 * \return boolean - TRUE if there are PMO/PCCO ncells present; FALSE otherwise
 */
boolean rr_gprs_pmo_pcco_ncells_are_present(
   
)
{
  uint8 index = 0;

  return rr_gprs_find_next_pmo_pcco_ncell_index(&index);

} /* rr_gprs_psi_ncells_are_present() */

/*!
 * \brief Removes a ncell (denoted by index) from the GSM Neighbour Cell List.
 *
 * \param ncell_index (in)
 */
void rr_gprs_remove_pmo_ncell_from_ba_gprs(
  uint8 ncell_index
   
)
{
  remove_ncell_from_gsm_neighbour_cell_list_by_index(ncell_index, FALSE);

} /* end rr_gprs_remove_pmo_ncell_from_ba_gprs() */

/*!
 * \brief Adds a ncell (NOT originated from PMO/PCCO) to the GSM Neighbour Cell List.
 *
 *  Adds a ncell to the GSM Neighbour Cell List. This ncell data is assumed to originate from sys info,
 *  i.e. SI2 (+ optionally SI2quater), but NOT a PMO/PCCO.
 *
 * \param bcch_arfcn (in)
 * \param bsic (in) - BSIC from SI2quater (optional). Set to NULL if not available (i.e. SI2-only).
 * \param bsic_description (in) - BSIC Description (optional). Set to NULL if not available.
 */
void rr_gprs_add_ncell_to_ba_gprs(
  ARFCN_T bcch_arfcn,
  BSIC_T *bsic,
  gsm_bsic_description_entry_T *bsic_description
   
)
{
  add_ncell_to_gsm_neighbour_cell_list(
    NCELL_ORIGINAL,
    bcch_arfcn,
    bsic,
    BCCH_FORMAT,
    NULL,
    bsic_description
  );

} /* rr_gprs_add_ncell_to_ba_gprs() */

/*!
 * \brief Reassigns values for band in the system information database based on the band indicator.
 *
 *  Scans following parameters in the system information database and reassigns the band for each ARFCN using
 *  rr_get_band_dyn():
 *
 *  - GSM Neighbour Cell List
 *  - Mobile Allocations
 *  - CBCH Channel Information
 *  - Cell Channel Description
 *  - Idle BA-list
 *  - Dedicated BA-list
 *
 * \param freq_list_mask (in) - Bitmap indicating which frequency lists to scan through.
 * \param is_camped (in)      - Set to TRUE if current camped-on.
 */
void rr_gprs_reassign_band(
  uint16 freq_list_mask,
  boolean is_camped
   
)
{
  system_information_database_T *sys_info_db;
  gprs_scell_info_t             *cell_info;
  si_psi_band_ind_T              band_ind;
  uint8                          i;
  uint8                          j;

  if (is_camped == FALSE)
  {
    cell_info = rr_gprs_get_pcell_info();
  }
  else
  {
    cell_info = rr_gprs_get_scell_info();
  }

  sys_info_db = &cell_info->gsm;

  /* do a one-time retrieval of the BAND_INDICATOR */
  band_ind = rr_get_current_band_indicator(is_camped);

  MSG_GERAN_HIGH_3_G("Reassign band: mask=0x%04X camped=%1d band_ind=%1d",
           (int)freq_list_mask,
           (int)is_camped,
           (int)band_ind);

  /* reassign band for each arfcn in neighbour cell list */
  if (freq_list_mask & NCELL_FREQ_LIST)
  {
    gprs_ncells_info_t        *ncells = rr_gprs_get_ncells_info();
    gsm_neighbour_cell_list_t *gsm_ncell_list = &ncells->gsm_neighbour_cell_list;
    gsm_ncell_list_member_t   *ncell;

    for (i = 0; i < gsm_ncell_list->num_of_ncells; i++)
    {
      ncell = &gsm_ncell_list->ncells[i];
      ncell->arfcn.band = (sys_band_T) rr_get_band_using_band_indicator(ncell->arfcn.num, band_ind);
    }
  }

  /* reassign band for the arfcn in cbch channel description IE */
  if (freq_list_mask & CBCH_FREQ)
  {
    uint16 freq = sys_info_db->cbch_channel_information.frequency_list.channel[0].num;

    sys_info_db->cbch_channel_information.frequency_list.channel[0].band =
      (sys_band_T) rr_get_band_using_band_indicator(
        freq,
        band_ind
      );
  }

  /* Reassign band in cell allocation captured from PSI2 or SI1 */
  if (freq_list_mask & CELL_ALLOC_FREQ_LIST)
  {
    if (sys_info_db->valid_CA_received == RR_CA_RECEIVED)
    {
      cell_channel_description_T *cell_allocation;
      cell_allocation = &sys_info_db->cell_channel_description;

      for (i = 0; i < cell_allocation->no_of_entries; i++)
      {
        cell_allocation->channel_number[i].band =
          (sys_band_T) rr_get_band_using_band_indicator(
            cell_allocation->channel_number[i].num,
            band_ind
          );
      }
    }
    else
    {
      MSG_GERAN_HIGH_0_G("Valid cell allocation is not present");
    }
  }

  /* Reassign band in mobile allocation captured from SI13 and PSI2 */
  if (freq_list_mask & MOBILE_ALLOC_FREQ_LIST)
  {
    gprs_mobile_allocations_t      *mobile_allocation;
    mobile_allocation = &cell_info->mobile_allocations;

    MSG_GERAN_HIGH_1_G("ma valid bitmap 0x:%x",mobile_allocation->ma_valid_bitmap);
    for (i = 0; i < MAX_NUM_OF_GPRS_MA; i++)
    {
      if (mobile_allocation->ma_valid_bitmap & (1 << i))
      {
        uint8 ma_offset = MAP_MA_NUMBER_TO_OFFSET(i);
        if (ma_offset < MAX_NUM_OF_GPRS_MA)
        {
           for(j = 0; j < mobile_allocation->gprs_mobile_allocation[ma_offset].list_length; j++)
           {
             mobile_allocation->gprs_mobile_allocation[ma_offset].ma_number[j].band =
               (sys_band_T) rr_get_band_using_band_indicator(
                 mobile_allocation->gprs_mobile_allocation[ma_offset].ma_number[j].num,
                 band_ind
               );
           }
        }
      }
    }
  }

  /* Reassign the band info in the complete idle BA list */
  if (freq_list_mask & IDLE_BA_FREQ_LIST)
  {
    rr_bcch_frequency_list_reassign_bands(&sys_info_db->complete_BA_list, band_ind);
    rr_bcch_frequency_list_reassign_bands(&sys_info_db->SI2_BA_list, band_ind);
    rr_bcch_frequency_list_reassign_bands(&sys_info_db->SI2bis_BA_list, band_ind);
    rr_bcch_frequency_list_reassign_bands(&sys_info_db->SI2ter_BA_list, band_ind);
  }

#ifdef FEATURE_GERAN_CS_SUPPORT
  /* Reassign the band info in the complete dedicated BA list */
  if (freq_list_mask & DED_BA_FREQ_LIST)
  {
    rr_ba_lists_t *rr_ba_lists_ptr = rr_get_ba_lists_ptr();

    //RR_NULL_CHECK_FATAL(rr_ba_lists_ptr);

    rr_bcch_frequency_list_reassign_bands(&rr_ba_lists_ptr->complete_BA_sacch, band_ind);
    rr_bcch_frequency_list_reassign_bands(&rr_ba_lists_ptr->SI5_BA_list, band_ind);
    rr_bcch_frequency_list_reassign_bands(&rr_ba_lists_ptr->SI5bis_BA_list, band_ind);
    rr_bcch_frequency_list_reassign_bands(&rr_ba_lists_ptr->SI5ter_BA_list, band_ind);
    rr_bcch_frequency_list_reassign_bands(&rr_ba_lists_ptr->SI5_SI5bis_measurement_report_BA_list, band_ind);
    rr_bcch_frequency_list_reassign_bands(&rr_ba_lists_ptr->SI5ter_measurement_report_BA_list, band_ind);
  }
#endif /* FEATURE_GERAN_CS_SUPPORT */

} /* rr_gprs_reassign_band() */

/*!
 * \brief Adds PMO/PCCO ncells to the GSM Neighbour Cell List.
 *
 *  Adds a PMO/PCCO ncell to the GSM Neighbour Cell List. This ncell data is assumed to originate from either
 *  a PMO or PCCO (i.e. NOT SI2, etc, sys info)
 *
 * \param bcch_arfcn (in)
 * \param bsic (in)
 * \param gprs_cell_selection_params (in) - GPRS Cell Selection Parameters, if available (optional).
 *
 * \return boolean - TRUE if the added cell is the serving cell; FALSE otherwise
 */
boolean rr_gprs_add_pmo_ncell_to_ba_gprs(
  ARFCN_T bcch_arfcn,
  BSIC_T *bsic,
  gprs_cell_selection_params_t *gprs_cell_selection_params
   
)
{

  gprs_scell_info_t * scell_info = rr_gprs_get_scell_info();
  boolean             bServing = FALSE;

  /* If this is the scell then load up the scell cell selection params */
  if ( ARFCNS_EQUAL( scell_info->gsm.BCCH_ARFCN, bcch_arfcn ) &&
       ( scell_info->gsm.BSIC.BS_colour_code == bsic->BS_colour_code ) &&
       ( scell_info->gsm.BSIC.PLMN_colour_code == bsic->PLMN_colour_code ) )
  {
    scell_info->bcch_sys_info.pmo_scell_params_valid = TRUE;

    scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.cell_bar_access_2 = gprs_cell_selection_params->cell_bar_access_2;
    scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.exc_access = gprs_cell_selection_params->exc_acc;
    scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.gprs_ms_txpwr_max_cch = gprs_cell_selection_params->gprs_ms_txpwr_max_cch;
    scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.gprs_rxlev_access_min = gprs_cell_selection_params->gprs_rxlev_access_min;
    scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params_present = gprs_cell_selection_params->hcs_params_present;
    scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params.priority_class = gprs_cell_selection_params->hcs_params.priority_class;
    scell_info->bcch_sys_info.pmo_gprs_serving_cell_params.hcs_params.hcs_thr = gprs_cell_selection_params->hcs_params.hcs_thr;

    bServing = TRUE;

    rr_eng_mode_update_scell_config();
  }
  else
  {
    add_ncell_to_gsm_neighbour_cell_list(
      NCELL_ADDED,
      bcch_arfcn,
      bsic,
      PMO_FORMAT,
      gprs_cell_selection_params,
      NULL
    );
  }

  return(bServing);
} /* rr_gprs_add_pmo_ncell_to_ba_gprs() */

/*!
 * \brief checks if a given cell is present in a given BA(GPRS) list.
 *
 * \param arfcn (in) - ARFCN
 * \param *ba_gprs (in) - BA(GPRS) list
 *
 * \return boolean - TRUE if a match; FALSE otherwise.
 */
static boolean rr_gprs_ncell_in_ba_gprs(
  ARFCN_T                       arfcn,
  inter_task_BCCH_allocation_T *ba_gprs
)
{
  uint8   i = 0;
  boolean found = FALSE;

  if (ba_gprs == NULL)
  {
    return FALSE;
  }

  for (i = 0; (i < ba_gprs->no_of_entries) && (i < BA_MAX_LEN); i++)
  {
    if(ARFCNS_EQUAL(ba_gprs->BCCH_ARFCN[i], arfcn))
    {
      found = TRUE;
      break;
    }
  }

  return found;
}

/*!
 * \brief Returns a pointer to the BA(GPRS) list.
 *
 *  Returns a pointer to the BA(GPRS) list. If this list is out of date with respect to the GSM Neighbour
 *  Cell List, it is rebuilt before the function returns.
 *  Note: The serving cell is NOT excluded from the returned BA(GPRS) list if present in the GSM Neighbour
 *  Cell List.
 *
 *
 * \return inter_task_BCCH_allocation_T* - Pointer to the BA(GPRS) list
 */
inter_task_BCCH_allocation_T *rr_gprs_get_ba_gprs(
   
)
{
  gprs_ncells_info_t  * ncells_info = NULL;

  boolean  arfcn_allowed;
  uint8  gsm_ncell_index, ba_gprs_index;
  gsm_ncell_list_member_t  *ncell;

  gsm_neighbour_cell_list_t     *gsm_ncell_list = NULL;
  inter_task_BCCH_allocation_T  *ba_gprs_list = NULL;
  uint8                          nc_mode = rr_gprs_get_nc_mode();

  ba_gprs_index = 0;

  ncells_info = rr_gprs_get_ncells_info();
  gsm_ncell_list = &ncells_info->gsm_neighbour_cell_list;
  ba_gprs_list = &ncells_info->ba_gprs_list;
  ba_gprs_list->no_of_entries = 0;
  
  for ( gsm_ncell_index = 0; gsm_ncell_index < gsm_ncell_list->num_of_ncells; gsm_ncell_index++ )
  {
    ncell = &gsm_ncell_list->ncells[gsm_ncell_index];

    if ( ( ncell->status == NCELL_ORIGINAL ) ||
         ( ncell->status == NCELL_ADDED ) ||
         ( ( ncell->status == NCELL_ADDED_NC2_ONLY ) &&
           ( nc_mode == NC_MEAS_NC2 ) ) )
    {
      arfcn_allowed = TRUE;

      if (!rr_gapi_is_supported_band_dyn((sys_band_T)GET_ARFCN_BAND(ncell->arfcn)))
      {
        MSG_GERAN_MED_1("BA entry=%d not included (unsupported band)",(int)CONVERT_FROM_ARFCN_T(ncell->arfcn));
        arfcn_allowed = FALSE;
      }

      if (arfcn_allowed)
      {
        boolean arfcn_present = rr_gprs_ncell_in_ba_gprs(ncell->arfcn, ba_gprs_list);
        
        if(!arfcn_present)
        {
          ba_gprs_list->BCCH_ARFCN[ba_gprs_list->no_of_entries] = ncell->arfcn;
          ba_gprs_list->no_of_entries++;
  
          if ( ba_gprs_list->no_of_entries >= MAX_SIZE_OF_BA_GPRS_LIST_FOR_L1 )
          {
            MSG_GERAN_HIGH_0_G("BA(GPRS) list size capped at 32");
            break;
          }
        }
      }
    }
  }

  for (ba_gprs_index = ba_gprs_list->no_of_entries; ba_gprs_index < MAX_SIZE_OF_BA_GPRS_LIST_FOR_L1; ba_gprs_index++ )
  {
#ifndef PERLUTF
    ba_gprs_list->BCCH_ARFCN[ba_gprs_index].num = INVALID_ARFCN;
#else
    #error code not present
#endif

#ifndef PERLUTF
    ba_gprs_list->BCCH_ARFCN[ba_gprs_index].band = (sys_band_T)BAND_NONE;
#else
    #error code not present
#endif
  }

  return ba_gprs_list;

} /* end rr_gprs_get_ba_gprs() */

#ifdef FEATURE_SGLTE

/*===========================================================================

FUNCTION rr_gprs_get_ba_gprs

DESCRIPTION
  Get the band based on supplied ARFCN, does not check if band is supported.

PARAMS
  None

RETURN VALUE
  Pointer to the BA(GPRS) list

===========================================================================*/
static rr_internal_band_T rr_sglte_get_band(uint16 arfcn)
{
  rr_internal_band_T band;
  si_psi_band_ind_T band_ind;

  if ((arfcn >= RR_ARFCN_PCS_1900_MIN) &&
      (arfcn <= RR_ARFCN_PCS_1900_MAX))
  {
    boolean is_camped = rr_is_camped();

    /* determine the current band indicator */
    band_ind = rr_get_current_band_indicator(is_camped);
  }
  else
  {
    /* band indicator is not relevant, but must be set to something */
    band_ind = SI_PSI_BAND_IND_DCS1800;
  }

  /* pass the ARFCN and the band indicator to the worker function */
  band = rr_determine_actual_band(arfcn, band_ind);

  return band;
}

void rr_gprs_get_ba_gprs_unfiltered( inter_task_BCCH_allocation_T * ba_gprs_list   )
{
  gprs_ncells_info_t * ncells_info = NULL;

  uint8  gsm_ncell_index, ba_gprs_index;
  gsm_ncell_list_member_t  * ncell;

  gsm_neighbour_cell_list_t      * gsm_ncell_list = NULL;
  uint8                            nc_mode = rr_gprs_get_nc_mode();
  uint8 i = 0;

  ba_gprs_index = 0;

  ncells_info = rr_gprs_get_ncells_info();
  gsm_ncell_list = &ncells_info->gsm_neighbour_cell_list;

  //RR_NULL_CHECK_RETURN_VOID(ba_gprs_list);

  *ba_gprs_list = ncells_info->ba_gprs_list;

  {
    /*additional debug*/
    uint8 gsm_900_cells = 0;
    uint8 dcs_cells = 0;
    uint8 c = 0;

    for (c=0; c<ncells_info->ba_gprs_list.no_of_entries && c<BA_MAX_LEN; c++)
    {
      if (rr_sglte_get_band(ncells_info->ba_gprs_list.BCCH_ARFCN[c].num) == BAND_PGSM ||
          rr_sglte_get_band(ncells_info->ba_gprs_list.BCCH_ARFCN[c].num) == BAND_EGSM)
      {
        gsm_900_cells++;
      }
      else if (rr_sglte_get_band(ncells_info->ba_gprs_list.BCCH_ARFCN[c].num) == BAND_DCS)
      {
        dcs_cells++;
      }

      if (ncells_info->ba_gprs_list.BCCH_ARFCN[c].band == INVALID_BAND ||
          ncells_info->ba_gprs_list.BCCH_ARFCN[c].band == SYS_BAND_NONE)
      {
        MSG_GERAN_ERROR_3_G("ba_gprs_list index:%d ARFCN:%d bad band:%d",
                            c,
                            ncells_info->ba_gprs_list.BCCH_ARFCN[c].num,
                            ncells_info->ba_gprs_list.BCCH_ARFCN[c].band);
      }
    }

    MSG_GERAN_HIGH_3_G("ba_gprs_list no entries:%d GSM_900:%d, DCS:%d",
                          ncells_info->ba_gprs_list.no_of_entries,
                          gsm_900_cells,
                          dcs_cells);

    gsm_900_cells = 0;
    dcs_cells = 0;

    for (c=0; c<gsm_ncell_list->num_of_ncells && c<MAX_SIZE_OF_GSM_NEIGHBOUR_CELL_LIST; c++)
    {
      if (rr_sglte_get_band(gsm_ncell_list->ncells[c].arfcn.num) == BAND_PGSM ||
          rr_sglte_get_band(gsm_ncell_list->ncells[c].arfcn.num) == BAND_EGSM)
      {
        gsm_900_cells++;
      }
      else if (rr_sglte_get_band(gsm_ncell_list->ncells[c].arfcn.num) == BAND_DCS)
      {
        dcs_cells++;
      }

      if (gsm_ncell_list->ncells[c].arfcn.band == INVALID_BAND ||
          gsm_ncell_list->ncells[c].arfcn.band == SYS_BAND_NONE)
      {
        MSG_GERAN_ERROR_3_G("gsm_ncell_list index ARFCN:%d bad band:%d",
                             c,
                             gsm_ncell_list->ncells[c].arfcn.num,
                             gsm_ncell_list->ncells[c].arfcn.band);
      }
    }

    MSG_GERAN_HIGH_3_G("gsm_ncell_list no entries:%d GSM_900:%d, DCS:%d",
                          gsm_ncell_list->num_of_ncells,
                          gsm_900_cells,
                          dcs_cells);
  }

  ba_gprs_list->no_of_entries = 0;
  for ( gsm_ncell_index = 0; gsm_ncell_index < gsm_ncell_list->num_of_ncells; gsm_ncell_index++ )
  {
    ncell = &gsm_ncell_list->ncells[gsm_ncell_index];

    if ( ( ncell->status == NCELL_ORIGINAL ) ||
         ( ncell->status == NCELL_ADDED ) ||
         ( ( ncell->status == NCELL_ADDED_NC2_ONLY ) &&
           ( nc_mode == NC_MEAS_NC2 ) ) )
    {
      boolean arfcn_present = rr_gprs_ncell_in_ba_gprs(ncell->arfcn, ba_gprs_list);
      
      if(!arfcn_present)
      {
        ba_gprs_list->BCCH_ARFCN[ba_gprs_list->no_of_entries] = ncell->arfcn;
        ba_gprs_list->no_of_entries++;
  
        if ( ba_gprs_list->no_of_entries >= MAX_SIZE_OF_BA_GPRS_LIST_FOR_L1 )
        {
          MSG_GERAN_HIGH_0_G("BA(GPRS) list size capped at 32");
          break;
        }
      }
    }
  }

  for (ba_gprs_index = ba_gprs_list->no_of_entries; ba_gprs_index < MAX_SIZE_OF_BA_GPRS_LIST_FOR_L1; ba_gprs_index++ )
  {
#ifndef PERLUTF
    ba_gprs_list->BCCH_ARFCN[ba_gprs_index].num = INVALID_ARFCN;
#else
    #error code not present
#endif

#ifndef PERLUTF
    ba_gprs_list->BCCH_ARFCN[ba_gprs_index].band = (sys_band_T)BAND_NONE;
#else
    #error code not present
#endif
  }

  /*Santize band info*/
  for (i=0; i<ba_gprs_list->no_of_entries && i<BA_MAX_LEN; i++)
  {
    if (ba_gprs_list->BCCH_ARFCN[i].band == INVALID_BAND ||
        ba_gprs_list->BCCH_ARFCN[i].band == SYS_BAND_NONE)
    {
      MSG_GERAN_HIGH_3_G("SGLTE BA List for band coex index:%d ARFCN:%d set to invalid band:%d",i,
                                                                                      ba_gprs_list->BCCH_ARFCN[i].num,
                                                                                      ba_gprs_list->BCCH_ARFCN[i].band);

      ba_gprs_list->BCCH_ARFCN[i].band = (sys_band_T) rr_sglte_get_band(ba_gprs_list->BCCH_ARFCN[i].num);
    }
  }
} /* end rr_gprs_get_ba_gprs_unfiltered() */

#endif /* FEATURE_SGLTE */

/*!
 * \brief Invalidates the sys info for the specified BCCH ncell in the GSM Neighbour Cell List.
 *
 * \param arfcn
 * \param bsic - BSIC (optional). Set to NULL if not known.
 */
void rr_gprs_invalidate_surround_ncell_sys_info(
  ARFCN_T arfcn,
  BSIC_T *bsic
   
)
{
  gsm_ncell_list_member_t *matching_ncell;

  uint8 index = find_gsm_ncell_list_ncell(arfcn, bsic, &matching_ncell);

  if (matching_ncell != NULL)
  {
    invalidate_surround_ncell_sys_info_by_index(index);
  }

} /* end rr_gprs_invalidate_surround_ncell_sys_info() */

/*!
 * \brief Reverts the BA(GPRS) list to broadcast system information.
 *
 */
void rr_gprs_revert_ba_gprs(
   
)
{
  gprs_ncells_info_t        *ncell_db_ptr = NULL;
  gsm_neighbour_cell_list_t *gsm_ncell_list = NULL;
  gsm_ncell_list_member_t   *ncell_ptr;
  uint8                      index;

  ncell_db_ptr = rr_gprs_get_ncells_info();
  gsm_ncell_list = &ncell_db_ptr->gsm_neighbour_cell_list;
  ncell_ptr = gsm_ncell_list->ncells;

  for ( index = 0; index < gsm_ncell_list->num_of_ncells; index++ )
  {
    if ( ( ncell_ptr->status == NCELL_ADDED ) ||
         ( ncell_ptr->status == NCELL_ADDED_NC2_ONLY ) )
    {
      unallocate_gsm_ncell_list_member(ncell_ptr);
    }
    else
    if ( ( ncell_ptr->status == NCELL_DUPLICATE ) ||
         ( ncell_ptr->status == NCELL_REMOVED ) )
    {
      ncell_ptr->status = NCELL_ORIGINAL;
    }

    ncell_ptr++;
  }

  rr_gprs_reset_pcco_nc_frequency_list();
} /* end rr_gprs_revert_ga_gprs() */

/*!
 * \brief Called when a sys info timer for a ncell expires.
 *
 *  Called when a sys info timer for a ncell expires. The ncell database is examined to find all BCCH entries.
 *  Each entries system info count is decremented. If the count becomes zero the ncell data is marked as
 *  invalid.
 *
 */
void rr_gprs_ncell_sys_info_timer_expiry( )
{
  gsm_ncell_list_member_t * ncell = NULL;
  gprs_ncells_info_t  * ncells = rr_gprs_get_ncells_info();
  gsm_neighbour_cell_list_t  * gsm_ncell_list = &ncells->gsm_neighbour_cell_list;
  boolean si_from_cache;
  uint8 ncell_index = 0;

  // Get the first ncell and then scan from here
  if (rr_gprs_find_first_ncell_index(&ncell_index, BCCH_FORMAT))
  {
    ncell = &gsm_ncell_list->ncells[ncell_index];

    if ( ncell->ncell_data.bcch.system_info_timer_count != UNINITIALISED_SYS_INFO_COUNT )
    {
      if ( --ncell->ncell_data.bcch.system_info_timer_count == 0 )
      {
        MSG_GERAN_MED_1_G("Ncell SI timer expired for ARFCN %d, flush cache",CONVERT_FROM_ARFCN_T(ncell->arfcn));

        /* Invalidate the data */
        ncell->ncell_data.bcch.gsm.valid_data_flag = FALSE;
        ncell->ncell_data.bcch.gprs.si13_data_valid_flag = FALSE;
        ncell->ncell_data.bcch.system_info_timer_count = UNINITIALISED_SYS_INFO_COUNT;

        /* If GRR is waiting for a response for this BCCH then clear the flag */
        si_from_cache = FALSE;
        if (match_surround_response_with_cache_check(ncell->arfcn, &si_from_cache))
        {
          set_waiting_for_surround_response_metrics(FALSE);
          if (!si_from_cache)
          {
            /* Tell L1 to stop trying for this one */
            rr_send_mph_stop_surround_update_req();
          }
          clear_waiting_for_surround_response();
          /* Give it another 20 secs before allowing a retry on this arfcn*/
          ncell->ncell_data.bcch.system_info_timer_count = SYSTEM_INFO_20_SEC_RETRY;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
          rr_log_gprs_surround_search_failure(RR_LOG_GPRS_BCCH_DECODING_TIMEOUT);
#endif
        }

#ifdef FEATURE_RR_SI_CACHE
        /* flush the SI cache for this CGI now */
        if (ncell->ncell_data.bcch.gsm.cell_id_valid)
        {
          rr_csi_flush_data(&(ncell->ncell_data.bcch.gsm.location_area_identification),
                   (uint16*)&(ncell->ncell_data.bcch.gsm.cell_id),
                            CSI_FLUSH_SI_CHANGE);
        }
#endif
      }
    }

    // now for the remaining cells
    while (rr_gprs_find_next_bcch_ncell_index(&ncell_index))
    {
      ncell = &gsm_ncell_list->ncells[ncell_index];

      if ( ncell->ncell_data.bcch.system_info_timer_count == UNINITIALISED_SYS_INFO_COUNT )
        continue;

      if ( --ncell->ncell_data.bcch.system_info_timer_count == 0 )
      {
        MSG_GERAN_MED_1_G("Ncell SI timer expired for ARFCN %d, flush cache",CONVERT_FROM_ARFCN_T(ncell->arfcn));

        /* Invalidate the data */
        ncell->ncell_data.bcch.gsm.valid_data_flag = FALSE;
        ncell->ncell_data.bcch.gprs.si13_data_valid_flag = FALSE;
        ncell->ncell_data.bcch.system_info_timer_count = UNINITIALISED_SYS_INFO_COUNT;

        /* If GRR is waiting for a response for this BCCH then clear the flag */
        si_from_cache = FALSE;
        if (match_surround_response_with_cache_check(ncell->arfcn, &si_from_cache))
        {
          set_waiting_for_surround_response_metrics(FALSE);
          if (!si_from_cache)
          {
            /* Tell L1 to stop trying for this one */
            rr_send_mph_stop_surround_update_req();
          }
          clear_waiting_for_surround_response();
          /* Give it another 20 secs before allowing a retry on this arfcn*/
          ncell->ncell_data.bcch.system_info_timer_count = SYSTEM_INFO_20_SEC_RETRY;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
          rr_log_gprs_surround_search_failure(RR_LOG_GPRS_BCCH_DECODING_TIMEOUT);
#endif
        }

#ifdef FEATURE_RR_SI_CACHE
        /* flush the SI cache for this CGI now */
        if (ncell->ncell_data.bcch.gsm.cell_id_valid)
        {
          rr_csi_flush_data(&(ncell->ncell_data.bcch.gsm.location_area_identification),
                   (uint16*)&(ncell->ncell_data.bcch.gsm.cell_id),
                            CSI_FLUSH_SI_CHANGE);
        }
#endif
      }
    }
  }

  /* always re-start the si timer */
  (void)rr_timer_start(GRR_SYSTEM_INFO_TIMER, SYSTEM_INFO_CLOCK_TIMEOUT);

} /* end rr_gprs_ncell_sys_info_timer_expiry() */

/*!
 * \brief Returns the index of the arfcn passed in in the BA(GPRS) list.
 *
 *  Returns the index of the arfcn passed in in the BA(GPRS) list. Either ARFCN-only or ARFCN+BSIC can be
 *  used for the search. If ARFCN+BSIC are supplied, then ARFCN+BSIC must match. If only ARFCN is
 *  supplied, then a match will only be made against a ncell with the same ARFCN but no associated BSIC.
 *
 * \param arfcn (in)
 * \param bsic (in) - BSIC (optional). Set to NULL if not required.
 *
 * \return uint8 - index
 */
uint8 rr_gprs_find_ba_gprs_index(
  ARFCN_T arfcn,
  BSIC_T *bsic
   
)
{
  uint8 index;
  gsm_ncell_list_member_t *ncell;

  index = find_gsm_ncell_list_ncell(arfcn, bsic, &ncell);

  if (ncell == NULL)
  {
    MSG_GERAN_ERROR_1_G("Ncell (%d) not found in GSM ncell list",arfcn.num);
  }

  return index;

} /* end rr_gprs_find_ba_gprs_index() */

/*!
 * \brief Finds the ncell data which matches the supplied ARFCN + BSIC.
 *
 *  Finds the ncell data which matches the supplied ARFCN + BSIC. Either ARFCN-only or ARFCN+BSIC can be used
 *  for the search. If ARFCN+BSIC is used, then ARFCN+BSIC must match.
 *
 * \param arfcn (in)
 * \param bsic (in) - BSIC (optional). Set to NULL if not required.
 *
 * \return gsm_ncell_list_member_t*
 */
gsm_ncell_list_member_t *rr_gprs_get_ncell_info(
  ARFCN_T arfcn,
  BSIC_T *bsic
   
)
{
  gsm_ncell_list_member_t *ncell;

  (void) find_gsm_ncell_list_ncell(arfcn, bsic, &ncell);

  return ncell;

} /* end rr_gprs_get_psi_ncell_sys_info() */

/*!
 * \brief Finds the ncell data for a BCCH ncell.
 *
 * \param arfcn (in)
 *
 * \return bcch_ncell_info_t*
 */
bcch_ncell_info_t * rr_gprs_get_surround_ncell_sys_info(
  ARFCN_T arfcn
   
)
{
  gsm_ncell_list_member_t *ncell = rr_gprs_get_ncell_info(arfcn, NULL);
  bcch_ncell_info_t       *ncell_info = NULL;

  if (ncell != NULL)
  {
    if (ncell->bcch_or_pmo == BCCH_FORMAT)
    {
      ncell_info = &ncell->ncell_data.bcch;
    }
    else
    {
      MSG_GERAN_HIGH_1("PSI ncell %d",CONVERT_FROM_ARFCN_T(arfcn));
    }
  }

  return ncell_info;

} /* end rr_gprs_get_surround_ncell_sys_info() */

/*!
 * \brief Finds the ncell data for a PMO/PCCO ncell.
 *
 * \param arfcn (in)
 * \param bsic (in)
 *
 * \return pmo_ncell_info_t*
 */

pmo_ncell_info_t *rr_gprs_get_pmo_pcco_ncell_sys_info(
  ARFCN_T arfcn,
  BSIC_T *bsic
   
)
{
  gsm_ncell_list_member_t *ncell = rr_gprs_get_ncell_info(arfcn, bsic);
  pmo_ncell_info_t        *ncell_info = NULL;

  if (ncell != NULL)
  {
    if (ncell->bcch_or_pmo == PMO_FORMAT)
    {
      ncell_info = &ncell->ncell_data.pmo_pcco;
    }
    else
    {
      MSG_GERAN_ERROR_2_G("Ncell (%d,%1d) is not PBCCH format",ncell->arfcn.num,ncell->arfcn.band);
    }
  }

  return ncell_info;

} /* end rr_gprs_get_psi_ncell_sys_info() */

/*!
 * \brief Finds the ncell data for a surround ncell.
 *
 * \param arfcn (in)
 *
 * \return information_T* - Pointer to ncell system information
 */
information_T *rr_gprs_get_surround_ncell_gsm_sys_info(
  ARFCN_T arfcn
   
)
{
  gsm_ncell_list_member_t *ncell_info = rr_gprs_get_best_six_by_arfcn(arfcn, NULL);

  if ((ncell_info != NULL) && (ncell_info->bcch_or_pmo == BCCH_FORMAT))
  {
    return &ncell_info->ncell_data.bcch.gsm;
  }
  else
  {
    return NULL;
  }

} /* end rr_gprs_get_surround_ncell_gsm_sys_info() */

/*!
 * \brief Returns a pointer to the neighbour cells data store.
 *
 *
 * \return gprs_ncells_info_t*
 */
gprs_ncells_info_t *rr_gprs_get_ncells_info(
   
)
{
  gprs_system_info_database_t *gprs_system_info_database_ptr = rr_get_gprs_system_info_database_ptr();

  return &gprs_system_info_database_ptr->ncells;

} /* end rr_gprs_get_ncells_info() */

/*!
 * \brief This function aborts sys info acquisition for a ncell.
 *
 */
void rr_gprs_nsi_notify_scell_si_acquisition_required(
   
)
{
  // Obtain a pointer to the module data
  rr_sys_info_ncell_data_t *rr_sys_info_ncell_data_ptr = rr_sys_info_ncell_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_sys_info_ncell_data_ptr);

  /* If GRR is waiting for a response for this BCCH then clear the flag */
  if (get_waiting_for_surround_response())
  {
    gsm_ncell_list_member_t *ncell_info_ptr = rr_gprs_get_best_six_by_arfcn(
      rr_sys_info_ncell_data_ptr->waiting_for_surround_response.arfcn,
      NULL
    );

    /* NOTE: request to L1 and clearance of our "waiting" flag occurs here */
    /* _EVEN_ if the neighbour cell ARFCN cannot be found in the best six  */

    /* tell L1 to stop trying for this one, and clear our waiting flag */
    rr_send_mph_stop_surround_update_req();
    clear_waiting_for_surround_response();

    if (ncell_info_ptr != NULL)
    {
      /* Give it another 10 secs before allowing a retry on this arfcn */
      ncell_info_ptr->ncell_data.bcch.system_info_timer_count = SYSTEM_INFO_10_SEC_RETRY;
      MSG_GERAN_HIGH_1_G("Ncell %d read suspended for scell read",ncell_info_ptr->arfcn.num);
    }
    else
    {
      MSG_GERAN_MED_2_G("WARNING: no ncell info for ARFCN %d (band %1d)",
                rr_sys_info_ncell_data_ptr->waiting_for_surround_response.arfcn.num,
                rr_sys_info_ncell_data_ptr->waiting_for_surround_response.arfcn.band);
    }
#ifndef FEATURE_GERAN_REDUCED_DEBUG
    rr_log_gprs_surround_search_failure(RR_LOG_GPRS_BCCH_DECODING_TIMEOUT);
#endif
  }

} /* end rr_gprs_nsi_notify_scell_si_acquisition_required() */

/*!
 * \brief Called when SI7 or SI8 is received for a neighbour cell.
 *
 *  Called when SI7 or SI8 is received for a neighbour cell.
 *  Note: It is assumed that the SI contents have been decoded & stored in the ncell data store before this
 *  function is called.
 *
 * \param bcch_arfcn (in)
 * \param ncell_info_ptr (in)
 */
void rr_ncell_si7_si8_received(
  ARFCN_T bcch_arfcn,
  bcch_ncell_info_t *ncell_info_ptr
   
)
{
  //RR_NULL_CHECK_FATAL(ncell_info_ptr);

  if (rr_timer_is_running(GRR_SYSTEM_INFO_TIMER) == FALSE)
  {
    (void)rr_timer_start(GRR_SYSTEM_INFO_TIMER, SYSTEM_INFO_CLOCK_TIMEOUT);
  }

  ncell_info_ptr->system_info_timer_count = MAX_SYS_INFO_COUNT_VALUE;

  if (ncell_info_ptr->gsm.cell_selection_parameters.CELL_RESELECT_PARAM_IND)
  {
    rr_start_penalty_timer(
      rr_resel_best_six_find(bcch_arfcn, NULL),
      ncell_info_ptr->gsm.cell_selection_parameters.PENALTY_TIME
    );
  }
}

/*!
 * \brief Called when SI3 or SI4 is received for a neighbour cell.
 *
 *  Called when SI3 or SI4 is received for a neighbour cell.
 *  Note: It is assumed that the SI contents have been decoded & stored in the ncell data store before this
 *  function is called.
 *
 * \param bcch_arfcn (in)
 * \param ncell_info_ptr (in)
 */
void rr_ncell_si3_si4_received(
  ARFCN_T bcch_arfcn,
  bcch_ncell_info_t *ncell_info_ptr
   
)
{
  boolean si_from_cache = FALSE;

  //RR_NULL_CHECK_FATAL(ncell_info_ptr);

  rr_ncell_si7_si8_received(bcch_arfcn, ncell_info_ptr);

  /* If GRR is awaiting a surround response check this is it, and clear it */
  if (match_surround_response_with_cache_check(bcch_arfcn, &si_from_cache))
  {
    set_waiting_for_surround_response_metrics(TRUE);

    if (!si_from_cache)
    {
      /* Stop L1, this is probably not needed but keeps the cycle consistant */
      rr_send_mph_stop_surround_update_req();
    }

    clear_waiting_for_surround_response();

    /* Resume serving cell sys info acquistion (if suspended) */
    rr_gprs_resume_camped_sys_info_handling();
#ifndef FEATURE_GERAN_REDUCED_DEBUG
    rr_log_gprs_surround_search_success();
#endif
  }
}

/* EOF */


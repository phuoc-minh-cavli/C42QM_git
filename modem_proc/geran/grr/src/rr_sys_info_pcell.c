/*! \file rr_sys_info_pcell.c

  Functionality related to pending cell (pcell) data storage.
  This is a duplicate of serving cell (scell) data, which is aquired during a full
  acquisition of system information prior to camping on a new scell.

                Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_sys_info_pcell.c#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_sys_info_pcell.h"
#include "rr_sys_info_ncell.h"
#include "rr_general.h"
#include "rr_resel_utils.h"
#include "geran_dual_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Clears the GPRS elements of the pending cell store.
 *
 */
static void clear_gprs_pcell_data(
   
)
{
  gprs_scell_info_t *pcell_info = rr_gprs_get_pcell_info();

  /* Zero out all the pcell GPRS elements (Except gsm!)- this will destroy any pointer links */
  rr_clear_gprs_cell_info(pcell_info);

  /* Now setup any 'non-zero' elements, and restore any links */
  pcell_info->mobile_allocations.si13_change_mark = RRPS_CHANGE_MARK_NOT_VALID;

  pcell_info->gsm.gprs_indicator = TRUE;
  pcell_info->bcch_sys_info.sgsnr = SGSNR_NOT_PRESENT;

  pcell_info->gprs_meas_params.report_type = REPORT_TYPE_N0RMAL;

} /* end clear_gprs_pcell_data() */

/*-----------------------------------------------------------------------------
 * Externalized Function Definitions
 * --------------------------------------------------------------------------*/

/*!
 * \brief Initialises the pcell data store.
 *
 */
void rr_init_pcell_data(
   
)
{
  gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info();

  /* Set any non-zero items */
  pcell_info_ptr->mobile_allocations.si13_change_mark = RRPS_CHANGE_MARK_NOT_VALID;
  pcell_info_ptr->bcch_sys_info.sgsnr = SGSNR_NOT_PRESENT;
  pcell_info_ptr->gprs_meas_params.report_type = REPORT_TYPE_N0RMAL;

} /* end rr_init_pcell_data() */

/*!
 * \brief Returns a pointer to the pending cell system information.
 *
 *
 * \return gprs_scell_info_t*
 */
gprs_scell_info_t * rr_gprs_get_pcell_info(
   
)
{
  gprs_system_info_database_t *gprs_system_info_database_ptr = rr_get_gprs_system_info_database_ptr();

  return &gprs_system_info_database_ptr->pcell;

} /* end rr_gprs_get_pcell_info() */

/*!
 * \brief The pcell is populated with the system information from a ncell.
 *
 * \param arfcn
 */
void rr_ncell_ccch_to_pcell(
  ARFCN_T bcch_arfcn
   
)
{
  gprs_scell_info_t       *pcell_info_ptr = rr_gprs_get_pcell_info();
  gsm_ncell_list_member_t *ncell = rr_gprs_get_ncell_info(bcch_arfcn, NULL);

  if ( ncell && ( ncell->bcch_or_pmo == BCCH_FORMAT ) )
  {
    bcch_ncell_info_t * ncell_info = &ncell->ncell_data.bcch;

    /* Initialise the pcell data store to receive BCCH sys info acquisition data */
    clear_gprs_pcell_data();

    if ( ncell_info->gsm.control_channel_description_valid )
    {
      pcell_info_ptr->gsm.control_channel_description_valid = TRUE;
      pcell_info_ptr->gsm.control_channel_description.ATT = ncell_info->gsm.control_channel_description.ATT;
      pcell_info_ptr->gsm.control_channel_description.BS_AG_BLKS_RES = ncell_info->gsm.control_channel_description.BS_AG_BLKS_RES;
      pcell_info_ptr->gsm.control_channel_description.BS_CC_CHANS = ncell_info->gsm.control_channel_description.BS_CC_CHANS;
      pcell_info_ptr->gsm.control_channel_description.BS_CCCH_SDCCH_COMB = ncell_info->gsm.control_channel_description.BS_CCCH_SDCCH_COMB;
      pcell_info_ptr->gsm.control_channel_description.BS_PA_MFRMS = ncell_info->gsm.control_channel_description.BS_PA_MFRMS;
      pcell_info_ptr->gsm.control_channel_description.T3212_timeout = ncell_info->gsm.control_channel_description.T3212_timeout;
      pcell_info_ptr->gsm.control_channel_description.MSCR = ncell_info->gsm.control_channel_description.MSCR;
      pcell_info_ptr->gsm.location_area_identification = ncell_info->gsm.location_area_identification;
    }
    else
    {
      pcell_info_ptr->gsm.control_channel_description_valid = FALSE;
    }
    pcell_info_ptr->gsm.two_quater_position = ncell_info->gprs.two_quater_position;

    /* as the service domain must allow PS services */
    pcell_info_ptr->gsm.gprs_indicator = ncell_info->gprs_supported;

    if (ncell_info->gsm.cell_id_valid)
    {
      pcell_info_ptr->gsm.cell_identity_valid_flag = ncell_info->gsm.cell_id_valid;
      pcell_info_ptr->gsm.cell_identity = ncell_info->gsm.cell_id;
    }

    /* Remove this ncell from the ncell list */
    rr_gprs_remove_from_best_six(bcch_arfcn, ncell_info->gsm.BSIC);  // DMB  is this right ???
  }
  else
  {
#ifndef PERLUTF
    MSG_GERAN_ERROR_0_G("No ncell found for copy to pcell");
#else
    #error code not present
#endif
  }

} /* end rr_ncell_ccch_to_pcell() */

/*!
 * \brief The pending cell (pcell) becomes the serving cell (scell), and the camped_on_flag is set TRUE.
 *
 *
 * \return gprs_scell_info_t*
 */
gprs_scell_info_t * rr_pcell_becomes_scell(
   
)
{
  gprs_scell_info_t  * scell_info = rr_gprs_get_scell_info();
  gprs_scell_info_t  * pcell_info = rr_gprs_get_pcell_info();

  if ((scell_info != NULL) && (pcell_info != NULL))
  {
#ifdef FEATURE_GERAN_CS_SUPPORT
    /* free the dedicated memory if it differs from the pending store */
    if (scell_info->gsm.dedicated_mode_params != NULL)
    {
      if (scell_info->gsm.dedicated_mode_params != pcell_info->gsm.dedicated_mode_params)
      {
        rr_clear_system_information(&scell_info->gsm);
      }
    }
#endif /* FEATURE_GERAN_CS_SUPPORT */

    /* Copy pending cell data store over to serving cell data store */
    (* scell_info) = (* pcell_info);

    /* Assume that SI data is now valid for this serving cell */
    scell_info->gsm.valid_data_flag = TRUE;

    /* Clear GSM pending cell data */
    rr_clear_pending_channel_data();

    /* Clear pcell GPRS data */
    clear_gprs_pcell_data();
  }

  return(scell_info);
} /* end rr_pcell_becomes_scell() */

/*!
 * \brief Checks the BCCH ARFCN supplied against the BCCH ARFCN of the pcell.
 *
 * \param bcch_arfcn
 *
 * \return boolean
 */
boolean rr_is_this_the_pcell(
  ARFCN_T bcch_arfcn
   
)
{
  gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info();

  return ARFCNS_SAME_CHANNEL_AND_BAND(pcell_info_ptr->gsm.BCCH_ARFCN, bcch_arfcn);

} /* end rr_is_this_the_pcell() */

/* EOF */


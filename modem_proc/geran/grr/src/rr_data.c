/*! \file rr_data.c

  This module contains RR-global data declarations and memory management functionality

                Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_data.c#5 $ */
/* $DateTime: 2019/04/15 01:13:13 $$Author: pwbldsvc $ */


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include <stdio.h>
#include "sys_type.h"
#include "sys_stru.h"
#include "sys_cnst.h"
#include "rex.h"
#include "timers.h"
#include "gs.h"
#include "rr_seg_load.h"
#include "rr_l2_l1.h"
#include "rr_l2.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "err.h"
#include "rr_defs.h"
#include "gprs_mem.h"
#include "rr_general.h"
#include "rr_decode_ie.h"
#ifdef FEATURE_LTE
#include "lte_irat_types.h"
#endif /* FEATURE_LTE */
#include "rr_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/**
 * description of each OTA message received by the RR. Contains information about
 * IEs contained in the message (Mandatory Vs. Optional/Conditional, type, length,
 * etc.)
 */
static message_list_T  message_list[NUMBER_OF_MT_RR_MESSAGES];

/**
 * Maximum index in the message_list data structure
 */
static byte  max_message_index;

/**
 * Information about the PLMN currently requested by the MM and the cell selection
 * mode of the RR(normal vs. any cell selection). Also contains flags that are used
 * during cell selection.
 */
static rr_PLMN_request_details_T      rr_PLMN_request_details;

/**
 * Information about the PLMN list request from MM
 */
static rr_PLMN_list_req_info_T   rr_plmn_list_req_info;


static rr_l1_gsm_deact_reason_T   rr_l1_gsm_deact_reason;

static si2ter_instances_T  *si2ter_instances_ptrs = 0;
static instances_T         *si2q_instances_ptrs = 0;
static instances_T         *mi_instances_ptrs = 0;
#ifdef FEATURE_GERAN_EAB
static si21_instances_T    *si21_instances_ptrs = 0;
#endif /* FEATURE_GERAN_EAB */

#ifdef FEATURE_WCDMA
/* used for measurement reporting */
static wcdma_ncell_list_data_t  wcdma_ncell_list_data;
static surr_wcdma_ded_measurements_T  wcdma_ded_meas;
#endif 
#ifdef FEATURE_GSM_TDS
static rr_ded_irat_measurements_t  tds_ded_meas;
#endif /*FEATURE_GSM_TDS*/

/**
 * stores surrounding measurement information in dedicated mode (move to cell_reestablish)
 */
static surrounding_measurement_T      surrounding_measurement;

static rr_measured_signal_data_t  rr_measured_signal_data;

static rr_nas_message_buffer_T  *rr_nas_message_buffer[MAX_NAS_MESSAGE_BUFFER_SIZE];

static rr_chan_rel_cell_selection_indicator_t  rr_cell_select_indicator;

#ifdef FEATURE_LTE
/* This is a pointer to a structure containing the current individual priorities. */
static rr_individual_priorities_t *individual_priorities_ptr = 0;
#endif /* FEATURE_LTE */

/**
  This is a pointer to a structure containing the current set of modifications
  to broadcast system information, as received in Packet Cell Change Order,
  Packet Measurement Order of X2G Cell Change Order (NCO only, in X2G CCO).

  @see rr_params_mods

  The rr_params_mods function returns the address of this pointer (a pointer
  to this pointer) to the caller.
*/
static rr_params_mods_t  * params_mods;
//static rr_params_mods_t ** params_mods_ptr = params_mods;

/**
  This is a pointer to a structure containing the set of modifications
  to broadcast system information, as received in Packet Cell Change Order,
  that have not yet been applied.

  @see rr_pcco_params_mods

  The rr_pcco_params_mods function returns the address of this pointer (a
  pointer to this pointer) to the caller.
*/
static rr_params_mods_t  * pcco_params_mods;
//static rr_params_mods_t ** pcco_params_mods_ptr = pcco_params_mods;


/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

#ifdef FEATURE_LTE
/*!
 * \brief Returns a pointer to the individual priorities data.
 *
 * \return rr_individual_priorities_t**
 */
rr_individual_priorities_t ** rr_individual_priorities( void )
{
  return (&individual_priorities_ptr);
}
#endif /* FEATURE_LTE */

/**
  @brief Returns the address of the pointer to the current set of modifications
  to broadcast system information.
*/
rr_params_mods_t ** rr_params_mods( void )
{
  return &(params_mods);
}

/**
  @brief Returns the address of the pointer to the set of modifications to
  broadcast system information provided by a Packet Cell Change Order.
*/
rr_params_mods_t ** rr_pcco_params_mods( void )
{
  return &(pcco_params_mods);
}


/*!
 * \brief Initialised the cell seleciton inidicator data.
 */
void rr_init_cell_selection_indicator( )
{
  rr_chan_rel_cell_selection_indicator_t *cell_sel_indicator_ptr = rr_get_cell_select_indicator_ptr();

  memset(cell_sel_indicator_ptr, 0, sizeof(rr_chan_rel_cell_selection_indicator_t));
  cell_sel_indicator_ptr->valid = FALSE;
  cell_sel_indicator_ptr->type = UNKNOWN_TARGET;
}



/*!
 * \brief Returns a pointer to the SI2Q/MI instances data.
 *
 * \param si2q_or_mi
 * \param 
 *
 * \return instances_T*
 */
instances_T *rr_get_instances_ptr(
  si2q_mi_instance_e_type si2q_or_mi
)
{
  instances_T *result_ptr = NULL;

  if (si2q_or_mi == SI2Q_INSTANCE)
  {

    if (si2q_instances_ptrs == NULL)
    {
      si2q_instances_ptrs = (instances_T *)EXTPL_MEM_MALLOC(sizeof(instances_T));
      RR_NULL_CHECK_FATAL(si2q_instances_ptrs);
      memset(si2q_instances_ptrs, 0, sizeof(instances_T));
    }

    result_ptr = si2q_instances_ptrs;
  }
  else if (si2q_or_mi == MI_INSTANCE)
  {
    if (mi_instances_ptrs == NULL)
    {
      mi_instances_ptrs = (instances_T *)EXTPL_MEM_MALLOC(sizeof(instances_T));
      RR_NULL_CHECK_FATAL(mi_instances_ptrs);
      memset(mi_instances_ptrs, 0, sizeof(instances_T));
    }

    result_ptr = mi_instances_ptrs;
  }
  else
  {
    MSG_GERAN_LOW_1_G("si2q_or_mi value invalid (%d)", (int)si2q_or_mi);
  }

  if (result_ptr != NULL)
  {
    result_ptr->si2q_or_mi = si2q_or_mi;
  }
  else
  {
    MSG_GERAN_FATAL_0_G("rr_get_instances_ptr() alloc failed");
  }

  return (result_ptr);
}

/*!
 * \brief Returns a pointer to the SI2ter instances data.
 * 
 * \param  
 * 
 * \return si2ter_instances_T* 
 */
si2ter_instances_T *rr_get_SI2ter_instances_ptr( void )
{

  if (si2ter_instances_ptrs == NULL)
  {
    si2ter_instances_ptrs = (si2ter_instances_T *)GPRS_MEM_CALLOC(sizeof(si2ter_instances_T));
  }

  if (si2ter_instances_ptrs == NULL)
  {
    MSG_GERAN_FATAL_0_G("rr_get_instances_ptr() alloc failed");
  }

  return (si2ter_instances_ptrs);
}

#ifdef FEATURE_GERAN_EAB
/*!
 * \brief Returns a pointer to the SI21 instances data.
 * 
 * \param  
 * 
 * \return si21_instances_T* 
 */
si21_instances_T *rr_get_SI21_instances_ptr( void )
{

  if (si21_instances_ptrs == NULL)
  {
    si21_instances_ptrs = (si21_instances_T *)GPRS_MEM_CALLOC(sizeof(si21_instances_T));
  }

  if (si21_instances_ptrs == NULL)
  {
    MSG_GERAN_FATAL_0_G("rr_get_SI21_instances_ptr() alloc failed");
  }

  return (si21_instances_ptrs);
}
#endif /* FEATURE_GERAN_EAB */

/*!
 * \brief Returns a pointer to the message list data.
 *
 * \return message_list_T*
 */
message_list_T *rr_get_message_list_ptr( void )
{
  return (&message_list[0]);
}


/*!
 * \brief Returns the number of elements in the message list.
 *
 * \return uint16
 */
uint16 rr_elements_in_message_list( void )
{
  // Obtain data-space index

  return (NELEMS(message_list));
}


/*!
 * \brief Returns a pointer to the PLMN select request data.
 *
 * \return rr_PLMN_request_details_T*
 */
rr_PLMN_request_details_T *rr_get_PLMN_request_details_ptr( void )
{
  return (&rr_PLMN_request_details);
}


/*!
 * \brief Initialise the PLMN select request data (Note: Includes RAT Priority List)
 * 
 * \param  
 */
void rr_init_plmn_select_req_info( void )
{
  rr_PLMN_request_details_T *data_ptr = rr_get_PLMN_request_details_ptr();

  // Zero the PLMN selection info structure
  memset(data_ptr, 0, sizeof(rr_PLMN_request_details_T));

  // Setup any default values which may be non-zero
  data_ptr->search_reason  = RR_AUTONOMOUS_CELL_SELECTION;
  data_ptr->service_domain = SYS_SRV_DOMAIN_CS_ONLY;

  return;
}


/*!
 * \brief Returns a pointer to the PLMN list request data.
 *
 * \return rr_PLMN_list_req_info_T*
 */
rr_PLMN_list_req_info_T *rr_get_plmn_list_req_info_ptr( void )
{

  return (&rr_plmn_list_req_info);
}


/*!
 * \brief Initialise the PLMN list request data
 * 
 * \param  
 */
void rr_init_plmn_list_req_info( void )
{
  rr_PLMN_list_req_info_T *data_ptr = rr_get_plmn_list_req_info_ptr();

  // Zero the PLMN list info structure
  memset(data_ptr, 0, sizeof(rr_PLMN_list_req_info_T));

  // Setup any default values which may be non-zero
  data_ptr->network_selection_mode = SYS_NETWORK_SELECTION_MODE_NONE;
  data_ptr->trans_id = 0xff;
#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  data_ptr->service_search = SYS_SERVICE_SEARCH_NONE;
#endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

  return;
}

/*!
 * \brief Gets the L1 deact reason
 *
 * \param 
 *
 * \return rr_l1_gsm_deact_reason_T
 */
rr_l1_gsm_deact_reason_T rr_l1_gsm_deact_reason_get( void )
{

  return (rr_l1_gsm_deact_reason);
}


/*!
 * \brief Sets the L1 deact reason
 *
 * \param gsm_deact_reason
 * \param 
 */
void rr_l1_gsm_deact_reason_set(
  rr_l1_gsm_deact_reason_T gsm_deact_reason )
{
  rr_l1_gsm_deact_reason = gsm_deact_reason;
}


#ifdef FEATURE_GSM_TDS
/*!
 * \brief Returns a pointer to tds_ded_meas
 *
 * \return rr_ded_irat_measurements_t*
 */
rr_ded_irat_measurements_t *rr_get_tds_ded_meas_ptr( void )
{
  return (&tds_ded_meas);
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
/*!
 * \brief Returns a pointer to wdma_ncell_list_data
 *
 * \return wcdma_ncell_list_data_t*
 */
wcdma_ncell_list_data_t *rr_get_wcdma_ncell_list_data_ptr( void )
{
  return (&wcdma_ncell_list_data);
}


/*!
 * \brief Returns a pointer to wcdma_ded_meas
 *
 * \return surr_wcdma_ded_measurements_T*
 */
surr_wcdma_ded_measurements_T *rr_get_wcdma_ded_meas_ptr( void )
{
  return (&wcdma_ded_meas);
}
#endif 
static void rr_data_free_SI2ter_instances_data( void )
{
  if (si2ter_instances_ptrs != NULL)
  {
    MSG_GERAN_LOW_0_G("Freeing memory allocated for SI2ter instances!");
    GPRS_MEM_FREE(si2ter_instances_ptrs);
    si2ter_instances_ptrs = NULL;
  }
}

#ifdef FEATURE_GERAN_EAB
static void rr_data_free_SI21_instances_data( void )
{
  if (si21_instances_ptrs != NULL)
  {
    int i = 0;
   
    MSG_GERAN_LOW_0_G("Freeing memory allocated for SI21 instances!");

    for (i = 0; i < MAX_SI21_INSTANCES; i++)
    {
      if (si21_instances_ptrs->instance[i] != NULL)
      {
        GPRS_MEM_FREE(si21_instances_ptrs->instance[i]);
        si21_instances_ptrs->instance[i] = NULL;
      }
    }

    GPRS_MEM_FREE(si21_instances_ptrs);
    si21_instances_ptrs = NULL;
  }
}
#endif /* FEATURE_GERAN_EAB */

static void rr_data_free_SI2quater_instances_data( )
{
  
  if (si2q_instances_ptrs != NULL)
  {
    int i = 0;
    /* Free the memory allocated dynamically for structures assoiciated with si2q_params CR#718258*/
    for (i = 0; i < MAX_INSTANCES; ++i)
    {
      rr_si2q_params_free_children(&si2q_instances_ptrs->instance[i].si2q_mi.si2q_params);
    }
    MSG_GERAN_LOW_0_G("Freeing memory allocated for SI2quater instances!");
    EXTPL_MEM_FREE(si2q_instances_ptrs);
    si2q_instances_ptrs = NULL;
  }
}

static void rr_data_free_mi_instances_data( )
{

  if (mi_instances_ptrs != NULL)
  {
    MSG_GERAN_LOW_0_G("Freeing memory allocated for Measurement Information instances!!");
    EXTPL_MEM_FREE(mi_instances_ptrs);
    mi_instances_ptrs = NULL;
  }
}


void rr_data_discard_data( )
{
  MSG_GERAN_HIGH_0_G("Discarding SI2ter, SI2q and MI instances");
  rr_data_free_SI2ter_instances_data();
  rr_data_free_SI2quater_instances_data();
  rr_data_free_mi_instances_data();
#ifdef FEATURE_GERAN_EAB
  rr_data_free_SI21_instances_data();
#endif /* FEATURE_GERAN_EAB */
}

/*!
 * \brief Returns a poitner to rr_measured_signal_data
 *
 * \return rr_measured_signal_data_t*
 */
rr_measured_signal_data_t *rr_get_measured_signal_data_ptr( void )
{
  return (&rr_measured_signal_data);
}


/*!
 * \brief Returns a pointer to max_message_index
 *
 * \return byte*
 */
byte *rr_get_max_message_index_ptr( void )
{
  return (&max_message_index);
}


/*!
 * \brief Returns a pointer to rr_cell_select_indicator
 *
 * \return rr_chan_rel_cell_selection_indicator_t*
 */
rr_chan_rel_cell_selection_indicator_t *rr_get_cell_select_indicator_ptr( void )
{
  return (&rr_cell_select_indicator);
}


/*!
 * \brief Returns a pointer to surrounding_measurement
 *
 * \return surrounding_measurement_T*
 */
surrounding_measurement_T *rr_get_surrounding_measurement_ptr( void )
{
  return (&surrounding_measurement);
}


/*!
 * \brief Returns a pointer to rr_nas_message_buffer
 *
 * \return rr_nas_message_buffer_T**
 */
rr_nas_message_buffer_T **rr_get_nas_message_buffer_ptr_ptr( void )
{
  return (&rr_nas_message_buffer[0]);
}

/* EOF */


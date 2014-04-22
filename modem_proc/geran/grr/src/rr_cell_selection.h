#ifndef RR_CELL_SELECTION_H
#define RR_CELL_SELECTION_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_cell_selection.h

GENERAL DESCRIPTION
   This module contains functions for dealing with cell selection.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_cell_selection.h#3 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_task.h"
#include "rr_events.h"
#include "sys.h"
#include "geran_multi_sim.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

extern rr_event_T rr_selection_control(
  rr_event_T cell_select_event,
  rr_cmd_bdy_type *new_message
   
);

extern boolean rr_compare_plmn_select_req_info(
  rr_plmn_select_req_T *rr_plmn_select_req
);

extern rr_event_T rr_process_plmn_select_req(
  rr_plmn_select_req_T *rr_plmn_select_req,
  boolean *service_domain_changed_ptr
);

extern boolean rr_cell_selection_waiting_for_pscan_results( void );
 
extern rr_event_T rr_selection_process_select_specific_bcch_cnf(
  mph_select_specific_bcch_cnf_T * mph_select_specific_bcch_cnf
);

extern void rr_store_plmn_id(
  PLMN_id_T *plmn_id
);

extern void rr_set_equivalent_plmn(
  PLMN_id_T *plmn_id
);

extern boolean rr_equivalent_plmn_is_undefined( void );

extern boolean rr_cell_selection_get_ue_oos_status( void );

extern void rr_cell_selection_set_ue_oos_status(
  boolean status
);


#ifdef FEATURE_WCDMA
extern void rr_update_band_preference_for_rrc(
  sys_band_mask_type band_pref_mask
);
#endif

extern void rr_update_band_preference_for_tdsrrc(
  sys_band_mask_type band_pref_mask
);

/**
  @brief Checks whether the cell that RR is currently camped on is suitable
         or not.

  @return TRUE if the currently camped on cell is suitable
  @return FALSE if the currently camped on cell is not suitable
*/
extern boolean rr_cell_selection_is_camped_cell_suitable( void );

extern byte rr_cell_selection_get_decode_bcch_retry_status( void );

extern void rr_cell_selection_clear_decode_bcch_retry_status( void );

/**
  @brief Returns the current service domain
*/
extern sys_srv_domain_e_type rr_sel_get_service_domain(void );

/**
  @brief Clears cell selection details data structure
*/
extern void rr_clear_cell_selection_details( void );

/**
  @brief Sets the search reason
*/
extern void rr_sel_set_search_reason(
  rr_search_reason_T search_reason
);

/**
  @brief Gets the search reason
 */
extern rr_search_reason_T rr_sel_get_search_reason( void );

#endif /* #ifndef RR_CELL_SELECTION_H */

/* EOF */


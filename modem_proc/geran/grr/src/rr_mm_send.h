#ifndef RR_MM_SEND_H
#define RR_MM_SEND_H

/*! \file rr_mm_send.h 
 
  This is the header file for rr_mm_send.c.
  This module contains functions which send messages from RR to MM.
 
  Copyright (c) 2002-2014 by QUALCOMM Technologies, Incorporated. All Rights Reserved.
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_mm_send.h#3 $ */
/* $DateTime: 2019/12/02 06:22:48 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "comdef.h"
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "cb_rr.h"
#include "timers.h"
#include "rr_defs.h"
#ifdef FEATURE_DUAL_SIM
#include "geran_dual_sim.h"
#include "geran_dual_sim_g.h"
#endif /* FEATURE_DUAL_SIM */
#include "rr_ce_paging.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern void rr_mm_send_reset_last_cell_info(
   
);

extern void rr_send_service_ind(
  mm_as_service_state_T         service_state,
  LAI_T                         *lai_ptr,
  system_information_database_T *si_db_ptr
);

#ifdef FEATURE_GSM_DTM
extern void rr_send_service_ind_from_ota_msg(
  mm_as_service_state_T         service_state,
  LAI_T                         *lai_ptr,
  system_information_database_T *si_db_ptr
   
);

#ifdef FEATURE_GSM_EDTM
extern void rr_send_est_ind(
   
);
#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

extern void rr_send_no_service_ind(
   
);

extern void rr_send_no_service_for_plmn_list(  
   
);


#ifdef FEATURE_SGLTE
extern void rr_sglte_srvcc_send_no_service_ind(mm_as_service_state_T service_state  );
extern void rr_sglte_srvcc_send_abort_ind( );
#endif /*FEATURE_SGLTE*/

#ifdef FEATURE_GERAN_CS_SUPPORT
void rr_send_sync_ind(
  sync_reason_T sync_reason,
  channel_type_T channel_type,
  channel_mode_T channel_mode,
  byte subchannel
   
);

extern void rr_send_abort_ind(
  byte RR_abort_cause,
  mm_rr_failure_cause_e_type MM_fail_cause
   
);

extern void rr_send_rel_ind(
  byte cause
   
);

extern void rr_send_abort_cnf(
  mm_as_status_T status
   
);

extern void rr_send_est_cnf(
   
);
#endif /* FEATURE_GERAN_CS_SUPPORT */

extern void rr_send_paging_ind(
  rr_page_domain_e domain,
  rr_identity_type_e identity_type
   
);

extern void rr_send_plmn_select_cnf(
  mm_as_status_T                error_code,
  mm_as_service_state_T         service_state,
  LAI_T                         *lai_ptr,
  sys_detailed_plmn_list_s_type *available_plmn_list_ptr,
  byte                          *bcch_allocation_ptr,
  system_information_database_T *sys_info_db_ptr
);

extern void rr_send_plmn_select_cnf_no_service(
  mm_as_status_T error_code
   
);

extern void rr_send_plmn_select_cnf_success(
   
);

extern void rr_send_plmn_list_cnf(
  sys_detailed_plmn_list_s_type *available_plmn_list_ptr,
  mm_as_status_T status,
  rr_PLMN_list_req_info_T *plmn_list_req
   
);

extern void rr_send_cell_change_ind(
  LAI_T,
  word,
  boolean
   
);

extern void rr_send_deact_cnf(
   
);

extern void rr_send_cm_status_ind(
   
);

extern void rr_send_cb_info(
  boolean cbch_present
   
);

extern void rr_send_data_ind(
  byte *layer3_message,
  byte message_length
   
);

extern void rr_send_stop_gsm_mode_cnf(
  mm_as_status_T status
   
);

extern void rr_send_activation_ind(
  mm_as_status_T status
   
);

#ifdef FEATURE_GERAN_CS_SUPPORT
extern boolean rr_abort_pending_rr_est_req(
  byte RR_abort_cause
   
);

extern void rr_abort_pending_rr_est_req_if_lai_changed(
  LAI_T *
   
);
#endif /* FEATURE_GERAN_CS_SUPPORT */

extern void rr_send_grr_gmm_gprs_suspension_ind(
   
);
#ifdef FEATURE_GSM_DTM
extern void rr_send_grr_gmm_gprs_resumption_ind(
  boolean gprs_resumption_ind,
  boolean cell_update_required
   
);
#else
extern void rr_send_grr_gmm_gprs_resumption_ind(
  boolean gprs_resumption_ind
   
);
#endif /* FEATURE_GSM_DTM */

extern void rr_LAI_with_PLMN_only(
  LAI_T *lai_ptr,
  PLMN_id_T *plmn_id_ptr
);

extern void rr_set_previous_LAI(
  PLMN_id_T plmn_id,
  word lac
   
);

extern void rr_send_rr_reselection_ind(
   
);

extern void rr_send_rr_gmm_gprs_service_cnf(
   
);

#ifdef FEATURE_INTERRAT_PCCO_GTOW
extern void rr_send_gtow_cco_complete_ind(
   
);
#endif

#ifdef FEATURE_GPRS_PS_HANDOVER
extern void rr_send_ps_handover_ind(
   
);
#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)

extern void rr_mm_send_ps_abort_cnf(
   
);

extern void rr_ms_send_rr_plmn_select_cnf(
  const mm_as_status_T error_code,
  const mm_rr_failure_cause_e_type dsds_cause,
  const boolean as_oos
);

extern void rr_ms_send_rr_plmn_list_cnf(
  const rr_plmn_list_req_T *req_ptr,
  const mm_as_status_T status
);

extern void rr_ms_send_deact_cnf(void);

extern void rr_ms_send_stop_mode_cnf(void);

extern void rr_ms_send_rr_ps_abort_cnf(void);

#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*!
 * \brief Sends RR_GMM_GPRS_MULTISLOT_CLASS_IND to GMM.
 * 
 * \param egprs_mulislot_class (in)
 * \param dtm_egprs_multislot_subclass (in)
 * \param gprs_multislot_class (in)
 */
extern void rr_send_gmm_gprs_multislot_class_ind(
  uint8 egprs_mulislot_class,
#ifdef FEATURE_GSM_DTM
  uint8 dtm_egprs_multislot_subclass,
#endif
  uint8 gprs_multislot_class
   
);

/*!
 * \brief Sends RR_GMM_GPRS_MS_RA_CAP_REQ to GMM.
 * 
 */
extern void rr_send_gmm_gprs_ms_ra_cap_req(
   
);

/*!
 * \brief Returns the service state last updated to MM.
 * 
 * 
 * \return mm_as_service_state_T 
 */
extern mm_as_service_state_T rr_mm_get_service_state(
   
);

/*!
 * \brief Returns the service capability last updated to MM.
 * 
 * 
 * \return sys_srv_capability_e_type 
 */
extern sys_srv_capability_e_type rr_mm_get_service_capability(
   
);

/*!
 * \brief Returns the Multislot Class value(s) last updated to GMM.
 * 
 * \param egprs_mulislot_class_ptr (out)
 * \param dtm_egprs_multislot_subclass_ptr (out)
 * \param gprs_multislot_class_ptr (out)
 * 
 * \return boolean - TRUE if GMM has been updated with these values (i.e. they are valid); FALSE otherwise
 */
extern boolean rr_mm_get_multislot_class(
  uint8 *egprs_mulislot_class_ptr,
#ifdef FEATURE_GSM_DTM
  uint8 *dtm_egprs_multislot_subclass_ptr,
#endif
  uint8 *gprs_multislot_class_ptr
   
);

/*!
 * \brief Sends RR_MULTIMODE_SUBS_CHGD_CNF to MM. The message has no content.
 */
extern void rr_mm_send_multimode_subs_chgd_cnf(void);

/*!
 * \brief Sends RR_SRVCC_HANDOVER_FAILURE_IND to MM.
 * 
 */
extern void rr_send_srvcc_handover_failure_ind(
   
);

#ifdef FEATURE_GERAN_BSR_PS_STATUS
/*!
 * \brief Sends RR_PS_STATUS_CHANGE_IND to NAS-MM.
 * 
 * \param ps_is_active (in)
 */
extern void rr_mm_send_rr_ps_status_change_ind(
  boolean ps_is_active
   
);
#endif // FEATURE_GERAN_BSR_PS_STATUS

/*!
 * \brief Compares the supplied GPRS sys info with that last updated to MM
 * 
 * \param gprs_sys_info_ptr (in)
 * 
 * \return boolean - TRUE if they are different, FALSE otherwise
 */
extern boolean rr_mm_diff_gprs_sys_info(
  mm_gprs_sys_info_T *gprs_sys_info_ptr
   
);

extern void rr_send_rr_gmm_gprs_update_priority_cnf(
   
);

#if defined(FEATURE_GERAN_IMPLICIT_PS_REJECT) || defined(FEATURE_GERAN_EAB)
void rr_send_ps_implicit_reject_ind(
  uint32  wait_time
   
);
#endif /* FEATURE_GERAN_IMPLICIT_PS_REJECT || FEATURE_GERAN_EAB */
extern ARFCN_T rr_get_previous_camped_BCCH_ARFCN ( );
/*!
 * \brief Initialisation function. Should be called once at task-start.
 */
extern void rr_mm_send_task_start_init(void);
extern void rr_mm_send_update_old_scell(
  ARFCN_T rr_previous_BCCH_ARFCN,
  BSIC_T  rr_previous_BSIC,
  LAI_T   rr_previous_LAI);

#endif /* RR_MM_SEND_H */

/* EOF */


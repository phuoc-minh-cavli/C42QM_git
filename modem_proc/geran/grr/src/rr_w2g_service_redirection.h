#ifndef RR_W2G_SERVICE_REDIRECTION_H
#define RR_W2G_SERVICE_REDIRECTION_H

/*! \file rr_w2g_service_redirection.h 
 
                Copyright (c) 2005-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_w2g_service_redirection.h#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG

#include "comdef.h"
#include "rr_defs.h"
#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
#include "rr_candidate_cell_db.h"
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/*!
 * \brief Indicates if a W2G redirection is in progress.
 * 
 * 
 * \return boolean - TRUE if a W2G redirection is in progress, FALSE otherwise
 */
extern boolean rr_w2g_redirection_in_progress(
   
);


extern boolean rr_l2g_redirection_in_progress( );
extern boolean rr_t2g_redirection_in_progress( );

#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION

/*!
 * \brief Indicates if a W2G Service Search is in progress.
 * 
 * 
 * \return boolean - TRUE if a service search is in progress, FALSE otherwise.
 */
extern boolean rr_w2g_service_search_in_progress(
   
);

#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */

/*!
 * \brief Indicates if a W2G Redirection Search is in progress.
 * 
 * 
 * \return boolean - TRUE if a redirection search is in progress, FALSE otherwise.
 */
extern boolean rr_w2g_redirection_search_in_progress(
   
);

/*!
 * \brief Indicates if an EHPLMN search is allowed for W2G redir req.
 * 
 * 
 * \return boolean - TRUE if an EHPLMN search is allowed, FALSE otherwise.
 */
extern boolean rr_w2g_redirection_is_ehplmn_search_allowed(
   
);

/*!
 * \brief Indicates if a W2G Redirection Search for Emergency Call is in progress.
 * 
 * 
 * \return boolean - TRUE if a redirection search is in progress for Emergency Call, FALSE otherwise.
 */
extern boolean rr_x2g_redir_search_for_emergency_call_in_progress(
   
);

/*!
 * \brief Indicates if a W2G Out Of Service Search is in progress.
 * 
 * 
 * \return boolean - TRUE if an out of service service search is in progress, FALSE otherwise.
 */
boolean rr_w2g_oos_search_in_progress(
   
);

/*!
 * \brief This is the main state machine for W2G Service Redirection.
 * 
 * \param event 
 * \param new_message 
 * 
 * \return rr_event_T 
 */
extern rr_event_T rr_w2g_service_redirection_control(
  rr_event_T event,
  void *new_message
   
);

/*!
 * \brief Checks if a cell is suitable for W2G redirection.
 * 
 * \param arfcn 
 * \param lai_ptr 
 * 
 * \return boolean TRUE if the cell is suitable; FALSE otherwise
 */
extern boolean rr_w2g_redirection_check_plmn_lai(
  ARFCN_T arfcn,
  LAI_T *lai_ptr
   
);

/*!
 * \brief Returns a pointer to rr_w2g_redirection_cell_list.
 * 
 * 
 * \return rr_reselection_pscan_cell_list_T* 
 */
rr_reselection_pscan_cell_list_T *rr_get_w2g_redirection_cell_list_ptr(
   
);

#ifdef FEATURE_LTE
/*!
 * \brief Sends redirection failure to LTE
 *
 *
 * \return None
 */
void rr_x2g_send_lte_redir_failure(
   
);
#endif /* FEATURE_LTE */

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT

/*!
 * \brief Determine if an X2G redirection with tunnelled SI procedure is in progress.
 * 
 * 
 * \return boolean - TRUE if an X2G redirection with tunnelled SI procedure is in progress; FALSE otherwise
 */
extern boolean rr_x2g_redir_with_si_in_progress(
   
);

/*!
 * \brief For each entry in the candidate cell database, sends tunnelled SI3 or SI4 if it is available.
 * 
 * \param candidate_cell_db 
 */
extern void rr_x2g_redir_with_si_send_si3_si4_if_available(
  rr_candidate_cell_db_T * candidate_cell_db
   
);

/*!
 * \brief Send all available system information for the specified cell.
 * 
 * \param arfcn - the ARFCN of the cell.
 * \param bsic the BSIC of the cell.
 */
extern void rr_x2g_redir_send_all_si(
  ARFCN_T arfcn,
  BSIC_T  bsic
   
);

/*!
 * \brief Prioritise the pscan results to favour ARFCNs for which tunnelled system information is available.
 * 
 * \param pscan_results - The power scan results to modify.
 */
extern void rr_x2g_redir_prioritise_pscan_results(
  rr_l1_pscan_results_T * pscan_results
   
);

#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */

#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

#endif /* #ifndef RR_W2G_SERVICE_REDIRECTION_H */

/* EOF */


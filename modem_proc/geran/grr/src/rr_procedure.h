#ifndef RR_PROCEDURE_H
#define RR_PROCEDURE_H

/*! \file rr_procedure.h 
 
  This module contains functionality related to co-ordination of dedicated procedures.
 
                Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_procedure.h#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_GERAN_CS_SUPPORT
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_l2.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"
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

/*!
 * \brief Checks whether there is an RR procedure currently ongoing or not.
 * 
 * 
 * \return boolean - TRUE if a procedure is ongoing; FALSE otherwise
 */
extern boolean rr_ded_proc_in_progress(
   
);

/*!
 * \brief Top-level controller for dedicated procedures.
 * 
 * \param proc_dispatch_event 
 * \param new_message 
 * 
 * \return rr_event_T 
 */
extern rr_event_T rr_handle_procedures(
  rr_event_T proc_dispatch_event,
  void *new_message
  
);

/*!
 * \brief Resets all SAPIs in response to a DL_ESTABLISH_IND for SAPI0.
 * 
 * \param msg_ptr - DL_ESTABLISH_IND message
 */
extern void rr_handle_l2_reestablishment(
  dl_establish_ind_T *msg_ptr
  
);

/*!
 * \brief Resets L2 and RR SAPI controls in response to a lower layer failure.
 * 
 */
extern void rr_reset_sapis_due_to_failure(
  
);

/*!
 * \brief Processes the SAPI0 DL_DATA_ACK_IND for a specific L3 message number.
 * 
 * \param l3_msg_number 
 */
extern void rr_handle_signalling_acknowledgement(
  uint8 l3_msg_number
  
);
#endif /* FEATURE_GERAN_CS_SUPPORT */


#endif /* #ifndef RR_PROCEDURE_H */

/* EOF */


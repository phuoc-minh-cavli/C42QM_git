#ifndef RR_PS_ACCESS_H
#define RR_PS_ACCESS_H

/*! \file rr_ps_access.h 
 
  This is the header file for rr_ps_access.c.
  This module contains the functionality behind the API as described in rr_ps_if.h,
  for co-ordinating the enabling/disabling of PS access.
  There is also other functionality, such as T3142 management.
 
                Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_ps_access.h#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_task.h"
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
 * \brief Start T3142 with the time-out value indicated. The ARFCN+BSIC of the current cell is recorded.
 * 
 * \param t3142_timeout_secs (in)
 */
extern void grr_start_t3142(
  uint8 t3142_timeout_secs
   
);

/*!
 * \brief Returns TRUE if an UL TBF establishment is allowed.
 * 
 *  An UL TBF establishment is allowed unless:
 *   - GMM has not requested GPRS service (ever)
 *   - the access class of the cell does not allow access
 * 
 *  
 * \return boolean 
 */
extern boolean rr_mac_ul_tbf_est_is_allowed( );

#ifdef FEATURE_GSM_DTM
/*!
 * \brief Returns TRUE if an UL transfer via GTTP is allowed. 
 *  
 *  An UL transfer via GTTP is allowed unless:
 *   - RR not in dedicated
 *   - the cell does not support DTM
 *   - GMM has not requested GPRS service
 * 
 *  
 * \return boolean 
 */
extern boolean rr_mac_gttp_transfer_is_allowed( );
#endif // FEATURE_GSM_DTM

/*!
 * \brief Returns TRUE if a DL TBF establishment is allowed. 
 *  
 *  A DL TBF establishment is allowed unless:
 *   - GMM has not requested GPRS service (ever)
 * 
 *  
 * \return boolean 
 */
extern boolean rr_mac_dl_tbf_est_is_allowed( );

/*!
 * \brief Called when T3142 expires.
 *  
 *  GRR_MAC_NO_PS_ACCESS_IND is sent to GMAC indicating deletion of all PDUs, if that was not the previous
 *  action
 *  
 */
extern void rr_ps_t3142_expiry( );

#ifdef FEATURE_GERAN_IMPLICIT_PS_REJECT

/*!
 * \brief Called when T3236expires.
 *  
 *  
 */
extern void rr_ps_t3236_expiry( );
#endif /*FEATURE_GERAN_IMPLICIT_PS_REJECT*/

/*!
 * \brief Ensures that PS access is blocked with a DEL ALL action. 
 *  
 *  RR_IMSG_PS_ACCESS_REQ is sent to RR if UL TBF establishment is now allowed
 *  
 */
extern void rr_ps_no_ps_access_del_all( );

#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)
/*!
 * \brief Indicates if PS access has been blocked with a DEL ALL action.
 *  
 *  
 * \return boolean - TRUE if this is confirmed, FALSE otherwise
 */
extern boolean rr_ps_access_blocked_del_all( );

/*!
 * \brief Indicates if all other subs have PS access blocked.
 * 
 * 
 * \return boolean - TRUE if all other subs have PS access blocked, FALSE otherwise
 */
extern boolean rr_ps_access_blocked_other_subs( );

/*!
 * \brief Indicates if all other subs have PS access blocked in DEL_ALL state.
 * 
 * 
 * \return boolean - TRUE if all other subs have PS access blocked in DEL_ALL state, FALSE otherwise
 */
extern boolean rr_ps_access_blocked_del_all_other_subs( );
#endif // #if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)

/*!
 * \brief The RR-PS-ACCESS state machine.
 * 
 * \param msg_ptr (in)
 */
extern void rr_ps_access_control(
  const rr_cmd_bdy_type *msg_ptr
   
);

/*!
 * \brief Initialisation function.
 *  
 */
extern void rr_ps_access_init( );

/*!
 * \brief Initialisation function. Called once at task start.
 */
extern void rr_ps_access_task_start_init(void);

#endif /* #ifndef RR_PS_ACCESS_H */

/* EOF */


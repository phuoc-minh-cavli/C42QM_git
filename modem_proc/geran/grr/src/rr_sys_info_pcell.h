#ifndef RR_SYS_INFO_PCELL_H
#define RR_SYS_INFO_PCELL_H

/*! \file rr_sys_info_pcell.h 
 
  This is the header file for the rr_sys_info_pcell.c module.
 
                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_sys_info_pcell.h#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "sys_type.h"
#include "rr_gprs_defs.h"
#include "geran_dual_sim.h"

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
 * \brief Returns a pointer to the pending cell system information.
 * 
 * 
 * \return gprs_scell_info_t* 
 */
extern gprs_scell_info_t *rr_gprs_get_pcell_info(
   
);

/*!
 * \brief The pcell is populated with the system information from a ncell.
 * 
 * \param arfcn 
 */
extern void rr_ncell_ccch_to_pcell(
  ARFCN_T arfcn
   
);

/*!
 * \brief The pending cell (pcell) becomes the serving cell (scell), and the camped_on_flag is set TRUE.
 * 
 * 
 * \return gprs_scell_info_t* 
 */
extern gprs_scell_info_t *rr_pcell_becomes_scell(
   
);

/*!
 * \brief Checks the BCCH ARFCN supplied against the BCCH ARFCN of the pcell.
 * 
 * \param bcch_arfcn 
 * 
 * \return boolean 
 */
extern boolean rr_is_this_the_pcell(
  ARFCN_T bcch_arfcn
   
);

#endif /* #ifndef RR_SYS_INFO_PCELL_H */

/* EOF */


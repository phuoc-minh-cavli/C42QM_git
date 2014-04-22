#ifndef RR_X2G_CGI_H
#define RR_X2G_CGI_H

/*! \file rr_x2g_cgi.h 
 
  This module implements a state machine that handles CGI requests from other RATs.
 
                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_x2g_cgi.h#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_LTE_TO_GSM_CGI

#include "comdef.h"
#include "rr_events.h"
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
 * \brief Initialise the module, allocating any memory as necessary.
 *  
 *  This function MUST be called before any of this module's other APIs are used.
 *  
 */
extern void rr_x2g_cgi_init(void);

/*!
 * \brief Deinitialises the module, freeing any associated memory.
 * 
 */
extern void rr_x2g_cgi_deinit(void);

/*!
 * \brief This is the main state machine for this module.
 * 
 * \param message - An input message for this module.
 */
extern void rr_x2g_cgi_control(
  rr_cmd_bdy_type * message
   
);

#endif /* FEATURE_LTE_TO_GSM_CGI */

#endif /* #ifndef RR_X2G_CGI_H */

/* EOF */


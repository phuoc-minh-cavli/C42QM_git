#ifndef RR_TEST_H
#define RR_TEST_H

/*! \file rr_test.h 
 
  This is the header file for rr_test.c.
  This module contains SIM & loop-back test support.
 
                Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_test.h#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
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
 * \brief Handler for test-related messages.
 * 
 * \param new_message (in)
 */
extern void rr_handle_test_messages(
  void *new_message
   
);

#ifdef FEATURE_GERAN_CS_SUPPORT
/*!
 * \brief Opens the TCH loop.
 * 
 */
extern void rr_tm_open_tch_loop(
   
);
#endif /* FEATURE_GERAN_CS_SUPPORT */

/*!
 * \brief Initialisation function. Should be called once at RR task start.
 */
extern void rr_test_task_start_init(
  void
);

#endif /* #ifndef RR_TEST_H */

/* EOF */


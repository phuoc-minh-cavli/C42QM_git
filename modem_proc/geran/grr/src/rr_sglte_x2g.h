#ifndef RR_SGLTE_X2G_H
#define RR_SGLTE_X2G_H

/*============================================================================
  @file rr_sglte_x2g.h


                Copyright (c) 2009-2013 QUALCOMM Technologies, Inc
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_sglte_x2g.h#2 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_SGLTE

#include "comdef.h"
#include "rr_events.h"
#include "rr_task.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @brief Initialise the module and allocates necessary memory.
*/
extern void rr_sglte_x2g_init(void);

/**
  @brief Deinitialises the module and returns any allocated memory to the system.
*/
extern void rr_sglte_x2g_deinit(void);

/**
  @brief This is the main state machine for this module.

  @param message An input message for this module.
*/
extern void rr_sglte_x2g_control(
  rr_cmd_bdy_type * message
);

#endif /* FEATURE_SGLTE */

#endif /* RR_SGLTE_X2G_H */

/* EOF */


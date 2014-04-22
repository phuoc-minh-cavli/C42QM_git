#ifndef RR_RESEL_G2W_H
#define RR_RESEL_G2W_H

/** @file */

/*============================================================================
  @file rr_resel_g2w.h

  This is the header file for the main RR G2W Cell Reselection state machine.

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_resel_g2w.h#2 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

#include "comdef.h"
#include "rr_task.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/**
  @brief Initialises the module.
*/
extern void rr_resel_g2w_init(
   
);

/**
  @brief This is the main RR G2W Cell Reselection state machine.
*/
extern void rr_resel_g2w_control(
  rr_cmd_bdy_type *msg
   
);

extern boolean rr_resel_g2w_in_progress(
   
);

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#endif /* #ifndef RR_RESEL_G2W_H */

/* EOF */


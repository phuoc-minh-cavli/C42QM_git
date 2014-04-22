#ifndef RR_GPRS_DSC_H
#define RR_GPRS_DSC_H

/*============================================================================
  @file rr_gprs_dsc.h

  Header file for the RR GPRS Downlink Signalling Counter module.

                Copyright (c) 2002-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_gprs_dsc.h#2 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_events.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/
#define RR_DSC_DRX_DURATION_PER_MULTIFRAME 235 /* millisecs  */
#define GPRS_DSC_DIVIDEND                   90

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/**
  @brief Get the current downlink signalling counter value.

  @return The current downlink signalling counter value.
*/
extern int rr_gprs_return_gprs_dsc_counter_value( void );

#ifdef FEATURE_IDLE_DRX_SCALING
/**
  @brief Set the current downlink signalling counter value.

  @param dsc: new DSC value that need to be used

  @return None.
*/
extern void rr_gprs_set_gprs_dsc_counter_value(const int dsc );
#endif /* FEATURE_IDLE_DRX_SCALING */

/**
  @brief Apply the specified block quality indication to the downlink
         signalling counter and determine if reselection is required.

  Applies the specified block_quality indication to the current downlink
  signalling counter value.

  @param block_quality An indication of the block quality, provided by GL1.
  @param l2_channel_type An indication of the channel where decode was attempted.

  @return EV_DSF_RESELECTION if the downlink signalling counter has reached zero
          RR_EV_START_RESELECTION_POWER_SCAN if RPS is required to resync the scell
          EV_NO_EVENT otherwise.
*/
extern rr_event_T rr_gprs_check_dsc_in_idle(
  const gl1_block_quality_t block_quality,
  const l2_channel_type_T   l2_channel_type);

/**
  @brief Initialise the downlink signalling counter.

  This function will initialise the downlink signalling counter.
*/
extern void rr_gprs_initialise_dsc(void );
extern int rr_gprs_get_max_dsc_value(const boolean use_scaling );

/**
  @brief Return an indication of whether the downlink signalling counter is
         below it's maximum permitted value.

  @return TRUE if the downlink signalling counter is below it's maximum value,
  FALSE otherwise.
*/
extern boolean rr_gprs_is_dsc_below_max_value(void );

#if defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION) || defined (FEATURE_GERAN_EXTENDED_DRX)

extern boolean rr_gprs_is_l1_updated_with_lowest_edrx(void );
extern void rr_gprs_set_l1_updated_with_lowest_edrx( void );

#endif /*FEATURE_GERAN_POWER_EFFICIENT_OPERATION || FEATURE_GERAN_EXTENDED_DRX*/

#endif /* RR_GPRS_DSC_H */

/* EOF */


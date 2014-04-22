#ifndef __VS_VOCEVRCNW_H__
#define __VS_VOCEVRCNW_H__

/**
  @file vs_vocevrcnw.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for EVRC-NW vocoder

*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocevrcnw.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"


/**
 * EVRC-NW vocoder format.
 */
#define VS_VOCEVRCNW_MEDIA_ID ( 0x00010FC5 )


/**
 * Estimated average encoding rates for active speech 
 * (channel encoding rates).
 */
typedef enum vs_vocevrcnw_avg_enc_rate_t {

  VS_VOCEVRCNW_AVG_RATE_COP_0 = 0,
    /**< COP_0 -> RAT_8.5_kbps. */
  VS_VOCEVRCNW_AVG_RATE_COP_1 = 1,
    /**< COP_0 -> RAT_9.3_kbps. */
  VS_VOCEVRCNW_AVG_RATE_COP_2 = 2,
    /**< COP_0 -> RAT_7.5_kbps. */
  VS_VOCEVRCNW_AVG_RATE_COP_3 = 3,
    /**< COP_0 -> RAT_7.0_kbps. */
  VS_VOCEVRCNW_AVG_RATE_COP_4 = 4,
    /**< COP_0 -> RAT_6.6_kbps. */
  VS_VOCEVRCNW_AVG_RATE_COP_5 = 5,
    /**< COP_0 -> RAT_6.2_kbps. */
  VS_VOCEVRCNW_AVG_RATE_COP_6 = 6,
    /**< COP_0 -> RAT_5.8_kbps. */
  VS_VOCEVRCNW_AVG_RATE_COP_7 = 7,
    /**< COP_0 -> RAT_4.8_kbps. */

} vs_vocevrcnw_avg_enc_rate_t;

/**
 * EVRC-NW frame type (rate) information. 
 */
typedef enum vs_vocevrcnw_frame_rate_t
{
  VS_VOCEVRCNW_FRAME_BLANK_RATE = 0,
      /**< Blank. */
  VS_VOCEVRCNW_FRAME_EIGHTH_RATE = 1,
      /**< Rate 1/8. */
  VS_VOCEVRCNW_FRAME_QUARTER_RATE = 2,
      /**< Rate 1/4. */
  VS_VOCEVRCNW_FRAME_HALF_RATE = 3,
      /**< Rate 1/2. */
  VS_VOCEVRCNW_FRAME_FULL_RATE = 4,
      /**< Rate 1. */
  VS_VOCEVRCNW_FRAME_ERASURE_RATE = 14,
      /**< Erasure rate; applicable only for downlink. */
  VS_VOCEVRCNW_FRAME_EIGHTH_RATE_NONCRITICAL = 255,
      /**< Noncritical 1/8 rate. */

} vs_vocevrcnw_frame_rate_t;


/**
 * EVRC-NW frame information structure.
 */
typedef struct vs_vocevrcnw_frame_header_t {

  uint8_t rate;
    /**< EVRC-NW frame rate. */

} vs_vocevrcnw_frame_header_t;

#endif /* __VS_VOCEVRCNW_H__ */


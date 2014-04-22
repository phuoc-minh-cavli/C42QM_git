#ifndef __VS_VOCEVRCNW2K_H__
#define __VS_VOCEVRCNW2K_H__

/**
  @file vs_vocevrcnw2k.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for EVRC-NW2K vocoder

*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocevrcnw2k.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/


/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"


/** EVRC-NW2K vocoder format. */
#define VS_VOCEVRCNW2K_MEDIA_ID ( 0x00010FD7 )


/**
 * Estimated average encoding rates for active speech (channel 
 * coding rates).
 */
typedef enum vs_vocevrcnw2k_avg_enc_rate_t {

  VS_VOCEVRCNW2K_AVG_RATE_COP_0 = 0,
    /**< COP_0 -> RATE_8.5_kbps  */
  VS_VOCEVRCNW2K_AVG_RATE_COP_1 = 1,
    /**< COP_0 -> RATE_9.3_kbps  */
  VS_VOCEVRCNW2K_AVG_RATE_COP_2 = 2,
    /**< COP_0 -> RATE_7.5_kbps  */
  VS_VOCEVRCNW2K_AVG_RATE_COP_3 = 3,
    /**< COP_0 -> RATE_2.4_kbps  */
  VS_VOCEVRCNW2K_AVG_RATE_COP_4 = 4,
    /**< COP_0 -> RATE_6.6_kbps  */
  VS_VOCEVRCNW2K_AVG_RATE_COP_5 = 5,
    /**< COP_0 -> RATE_6.2_kbps  */
  VS_VOCEVRCNW2K_AVG_RATE_COP_6 = 6,
    /**< COP_0 -> RATE_5.8_kbps  */
  VS_VOCEVRCNW2K_AVG_RATE_COP_7 = 7,
    /**< COP_0 -> RATE_4.8_kbps  */

} vs_vocevrcnw2k_avg_enc_rate_t;


/**
 * EVRC-NW2K frame type (rate) information. 
 */
typedef enum vs_vocevrcnw2k_frame_rate_t {
 
  VS_VOCEVRCNW2K_FRAME_BLANK_RATE = 0,
      /**< Blank. */
  VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE = 1,
      /**< Rate 1/8. */
  VS_VOCEVRCNW2K_FRAME_QUARTER_RATE = 2,
      /**< Rate 1/4. */
  VS_VOCEVRCNW2K_FRAME_HALF_RATE = 3,
      /**< Rate 1/2. */
  VS_VOCEVRCNW2K_FRAME_FULL_RATE = 4,
      /**< Rate 1. */
  VS_VOCEVRCNW2K_FRAME_ERASURE_RATE = 14,
      /**< Erasure rate; applicable only for downlink. */
  VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE_NONCRITICAL = 255,
      /**< Noncritical 1/8 rate. */

} vs_vocevrcnw2k_frame_rate_t;


/**
 * EVRC-NW2K frame header structure.
 */
typedef struct vs_vocevrcnw2k_frame_header_t {

  uint8_t rate;
    /**< EVRC-NW2K frame rate. */

} vs_vocevrcnw2k_frame_header_t;

#endif /* __VS_VOCEVRCNW2K_H__ */



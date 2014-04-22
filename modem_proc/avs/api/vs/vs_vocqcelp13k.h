#ifndef __VS_VOCQCELP13K_H__
#define __VS_VOCQCELP13K_H__
/**
  @file vs_vocqcelp13k.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for QCELP-13k vocoder

*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocqcelp13k.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"


/**
 * QCELP-13K vocoder format.
 */
#define VS_VOCQCELP13K_MEDIA_ID ( 0x00010FC1 )


/**
 * Average encoding rates for active speech.
 */
typedef enum vs_vocqcelp13k_avg_enc_rate_t {

  VS_VOCQCELP13K_AVG_RATE_COP_0 = 0,
    /**< COP_0 -> RATE_14.4_kbps  */
  VS_VOCQCELP13K_AVG_RATE_COP_1 = 1,
    /**< COP_0 -> RATE_12.2_kbps  */
  VS_VOCQCELP13K_AVG_RATE_COP_2 = 2,
    /**< COP_0 -> RATE_11.2_kbps  */
  VS_VOCQCELP13K_AVG_RATE_COP_3 = 3,
    /**< COP_0 -> RATE_9.0_kbps  */
  VS_VOCQCELP13K_AVG_RATE_COP_4 = 4,
    /**< COP_0 -> RATE_7.2_kbps  */

} vs_vocqcelp13k_avg_enc_rate_t;


/**
 * QCELP-13K frame type (rate) information. 
 */
typedef enum vs_vocqcelp13k_frame_rate_t {

  VS_VOCQCELP13K_FRAME_BLANK_RATE = 0,
    /**< Blank. */
  VS_VOCQCELP13K_FRAME_EIGHTH_RATE = 1,
    /**< Rate 1/8. */
  VS_VOCQCELP13K_FRAME_QUARTER_RATE = 2,
    /**< Rate 1/4. */
  VS_VOCQCELP13K_FRAME_HALF_RATE = 3,
    /**< Rate 1/2. */
  VS_VOCQCELP13K_FRAME_FULL_RATE = 4,
    /**< Rate 1. */
  VS_VOCQCELP13K_FRAME_ERASURE_RATE = 14,
    /**< Erasure rate; applicable only for downlink. */

} vs_vocqcelp13k_frame_rate_t;


/**
 * QCELP-13K frame information structure.
 */
typedef struct vs_vocqcelp13k_frame_header_t {

  uint8_t rate;
    /**< QCELP-13K frame rate. */

} vs_vocqcelp13k_frame_header_t;

#endif /* __VS_VOCQCELP13K_H__ */


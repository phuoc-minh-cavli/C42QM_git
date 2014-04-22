#ifndef __VS_VOCEVRCB_H__
#define __VS_VOCEVRCB_H__

/**
  @file vs_vocevrcb.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for EVRC-B vocoder

*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
  ============================================================================ 
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocevrcb.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"

/****************************************************************************
 * EVRC-B TYPE DEFINITIONS                                                  *
 ****************************************************************************/

/**
 * EVRC-B vocoder format.
 */
#define VS_VOCEVRCB_MEDIA_ID ( 0x00010FC3 )


/** 
 * EVRC-B average encoding rate for active speech.
 *
 * It indicates the capacity operating point of encoder.
 */
typedef enum vs_vocevrcb_avg_enc_rate_t {

  VS_VOCEVRCB_AVG_RATE_COP_0 = 0,
    /**< COP_0 -> RATE_9.3_kbps  */
  VS_VOCEVRCB_AVG_RATE_COP_1 = 1,
    /**< COP_1 -> RATE_8.5_kbps  */
  VS_VOCEVRCB_AVG_RATE_COP_2 = 2,
    /**< COP_2 -> RATE_7.5_kbps  */
  VS_VOCEVRCB_AVG_RATE_COP_3 = 3,
    /**< COP_3 -> RATE_7.0_kbps  */
  VS_VOCEVRCB_AVG_RATE_COP_4 = 4,
    /**< COP_4 -> RATE_6.6_kbps  */
  VS_VOCEVRCB_AVG_RATE_COP_5 = 5,
    /**< COP_5 -> RATE_6.2_kbps  */
  VS_VOCEVRCB_AVG_RATE_COP_6 = 6,
    /**< COP_6 -> RATE_5.8_kbps  */
  VS_VOCEVRCB_AVG_RATE_COP_7 = 7,
    /**< COP_7 -> RATE_4.8_kbps  */

} vs_vocevrcb_avg_enc_rate_t;


/**
 * EVRC-B frame rate information. 
 */
typedef enum vs_vocevrcb_frame_rate_e
{
  VS_VOCEVRCB_FRAME_BLANK_RATE = 0,
    /**< Blank. */
  VS_VOCEVRCB_FRAME_EIGHTH_RATE = 1,
    /**< Rate 1/8. */
  VS_VOCEVRCB_FRAME_QUARTER_RATE = 2,
    /**< Rate 1/4. */
  VS_VOCEVRCB_FRAME_HALF_RATE = 3,
    /**< Rate 1/2. */
  VS_VOCEVRCB_FRAME_FULL_RATE = 4,
    /**< Rate 1. */
  VS_VOCEVRCB_FRAME_ERASURE_RATE = 14,
    /**< Erasure rate; applicable only for downlink. */

} vs_vocevrcb_frame_rate_t;


/**
 * EVRC-B frame Header structure.
 */
typedef struct vs_vocevrcb_frame_header_t {

  uint8_t rate;
    /**< EVRC-B frame rate. */

} vs_vocevrcb_frame_header_t;

#endif /* __VS_VOCEVRCB_H__ */
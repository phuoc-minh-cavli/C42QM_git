#ifndef __VS_VOCEVRCWB_H__
#define __VS_VOCEVRCWB_H__

/**
  @file vs_vocevrcwb.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for EVRC-WB vocoder

*/

/*
  ============================================================================
  Copyright (C) 2014, 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocevrcwb.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"


/**
 * EVRC-WB vocoder format.
 */
#define VS_VOCEVRCWB_MEDIA_ID ( 0x00010FC4 )


/**
 * Estimated average encoding rates for active speech 
 * channel encoding rates.
 */
typedef enum vs_vocevrcwb_avg_enc_rate_t {

  VS_VOCEVRCWB_AVG_RATE_COP_0 = 0,
    /**< COP_0 -> RAT_8.5_kbps. */
  VS_VOCEVRCWB_AVG_RATE_COP_4 = 4, 
    /**< COP_4 -> RATE_9.3_kbps. */
  VS_VOCEVRCWB_AVG_RATE_COP_7 = 7,
    /**< COP_7 -> RATE_4.8_kbps. */

} vs_vocevrcwb_avg_enc_rate_t;


/**
 * EVRC-WB frame type (rate) information. 
 */
typedef enum vs_vocevrcwb_frame_rate_t {

  VS_VOCEVRCWB_FRAME_BLANK_RATE = 0,
    /**< Blank. */
  VS_VOCEVRCWB_FRAME_EIGHTH_RATE = 1,
    /**< Rate 1/8. */
  VS_VOCEVRCWB_FRAME_QUARTER_RATE = 2,
    /**< Rate 1/4. */
  VS_VOCEVRCWB_FRAME_HALF_RATE = 3,
    /**< Rate 1/2. */
  VS_VOCEVRCWB_FRAME_FULL_RATE = 4,
    /**< Rate 1. */
  VS_VOCEVRCWB_FRAME_ERASURE_RATE = 14,
    /**< Erasure rate; applicable only for downlink. */

} vs_vocevrcwb_frame_rate_t;


/**
 * EVRC-WB frame Header structure..
 */
typedef struct vs_vocevrcwb_frame_header_t {

  vs_vocevrcwb_frame_rate_t rate;
    /**< EVRC-WB frame rate. */

} vs_vocevrcwb_frame_header_t;

#endif /* __VS_VOCEVRCWB_H__ */




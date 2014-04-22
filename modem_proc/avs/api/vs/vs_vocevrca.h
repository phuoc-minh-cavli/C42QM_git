#ifndef __VS_VOCEVRCA_H__
#define __VS_VOCEVRCA_H__

/**
  @file vs_vocevrca.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for EVRC-A vocoder
*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocevrca.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"

/****************************************************************************
 * EVRC-A TYPE DEFINITIONS                                                  *
 ****************************************************************************/
 
/** 
 * EVRC-A vocoder format.
 */
#define VS_VOCEVRCA_MEDIA_ID ( 0x00010FC2 )


/** 
 * EVRC-A Average encoder rate. It indicates the capacity 
 * operating point.
 */
typedef enum vs_vocevrca_avg_enc_rate_t {

  VS_VOCEVRCA_AVG_RATE_COP_0 = 0,
    /**< COP_0 -> RATE_14.4_kbps  */
  VS_VOCEVRCA_AVG_RATE_COP_1 = 1,
    /**< COP_1 -> RATE_12.2_kbps  */
  VS_VOCEVRCA_AVG_RATE_COP_2 = 2,
    /**< COP_2 -> RATE_11.2_kbps  */
  VS_VOCEVRCA_AVG_RATE_COP_3 = 3,
    /**< COP_3 -> RATE_9.0_kbps  */
  VS_VOCEVRCA_AVG_RATE_COP_4 = 4,
    /**< COP_4 -> RATE_7.4_kbps  */

} vs_vocevrca_avg_enc_rate_t;

/**
 * EVRC-A frame type (rate) information. 
 */
typedef enum vs_vocevrca_frame_rate_t {

  VS_VOCEVRCA_FRAME_BLANK_RATE = 0,
    /**< Blank. */
  VS_VOCEVRCA_FRAME_EIGHTH_RATE = 1,
    /**< Rate 1/8. */
  VS_VOCEVRCA_FRAME_HALF_RATE = 3,
    /**< Rate 1/2. */
  VS_VOCEVRCA_FRAME_FULL_RATE = 4,
    /**< Rate 1. */
  VS_VOCEVRCA_FRAME_ERASURE_RATE = 14,
    /**< Erasure rate, applicable only for downlink. */

} vs_vocevrca_frame_rate_t;


/**
 * EVRC-A frame Header structure.
 */
typedef struct vs_vocevrca_frame_header_t {

  uint8_t rate;
    /**< EVRC-A frame rate. */

} vs_vocevrca_frame_header_t;

#endif /* __VS_VOCEVRCA_H__ */
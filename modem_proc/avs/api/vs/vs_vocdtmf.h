#ifndef __VS_VOCDTMF_H__
#define __VS_VOCDTMF_H__

/**
  @file vs_vocdtmf.h
  @brief This file contains the AMR-NB interface definitions.
*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocdtmf.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"

/****************************************************************************
 * DTMF TYPE DEFINITIONS                                                  *
 ****************************************************************************/

/**
 * Dual-tone multi frequency format.
 *
 * Unlike other media format DTMF is not a standard media format and 
 * cannot be not used to condfigure vocoder services.
 *
 * During steady state vocoder packet exchange client can replace
 * configured vocoder frame with DTMF frame.
 */
#define VS_VOCDTMF_MEDIA_ID ( 0x00013295 )


/**
 * DTMF frame Header definition.
 */
typedef struct vs_vocdtmf_frame_header_t {

  uint8_t reserved : 8;
    /**< Reserved. */	
} vs_vocdtmf_frame_header_t;


/**
 * DTMF frame Data definition.
 */
typedef struct vs_vocdtmf_frame_data_t vs_vocdtmf_frame_data_t;

#include "apr_pack_begin.h"
struct vs_vocdtmf_frame_data_t {

  uint16_t low_frequency;
    /**< Low frequency for DTMF in Hz:
      *  - 697
      *  - 770
      *  - 852
      *  - 941
      */
  uint16_t high_frequency;
    /**< High frequency for DTMF in Hz:
      *  - 1209
      *  - 1336
      *  - 1447
      *  - 1633
      */
  uint32_t duration;
    /**< Supported values (in ms):
      *  -1: infinite duration
      *  0: stop tone
      *  >0: finite duration, it includes ramp up duration of 1ms and 
      *      ramp down duration of 2ms.
      */
  uint16_t gain;
    /**< Gain applied to DTMF tone. This is dBFs value in Q13 format. */
  uint16_t mix_enable;
    /**< Specifies if DTMF need to be mised with speech.
      *
      * @Supported Values:
      *   - 0: Disable speech and DTMF tone mixing (default).
      *   - 1: Enable speech and DTMF tone mixing.
      */
}
#include "apr_pack_end.h"
;
#endif /* __VS_VOCDTMF_H__ */


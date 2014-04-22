#ifndef __VS_VOCG711ALAW_V2_H__
#define __VS_VOCG711ALAW_V2_H__

/**
  @file vs_vocg711alaw_v2.h
  @brief This file contains the definitions of the constants,
         data structures and interfaces for G711 A-Law V2 vocoder

  G711 A-Law : G.711 also known as Pulse Code Modulation is a narrawband
  audio codec that provides audio at 64 kbits/s

  A-Law : A-Law encoding takes a 13-bit signed linear audio sample as input
  and converts it into 8bit value.
*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocg711alaw_v2.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"

/****************************************************************************
 *  G711ALAW-V2 TYPE DEFINITIONS                                                  *
 ****************************************************************************/

/** 
 * G.711 A-Law V2 vocoder format.
 */
#define VS_VOCG711ALAW_V2_MEDIA_ID ( 0x000131A1 )


/**
 * G.711 A-Law V2 frame type information. 
 */
typedef enum vs_vocg711alaw_v2_frame_type_t {

  VS_VOCG711ALAW_V2_FRAME_SPEECH_TYPE = 0,
    /**< Speech or active frame (full rate). */
  VS_VOCG711ALAW_V2_FRAME_SID_TYPE = 1,
    /**< SID frame. */
  VS_VOCG711ALAW_V2_FRAME_NTX_TYPE = 2,
    /**< No transmission frame. */
  VS_VOCG711ALAW_V2_FRAME_ERASURE_TYPE = 3,
    /**< Erasure flag; applicable for downlink only. */

} vs_vocg711alaw_v2_frame_type_t;


/**
 * G.711 A-Law V2 frame Header structure.
 */
typedef struct vs_vocg711alaw_v2_frame_header_t {

  uint8_t frame_type : 2;
    /**< BIT(0 - 1): Frame type.
      *    - 3 -- Erasure flag for DL only 
      *    - 2 -- NTX frame 
      *    - 1 -- SID frame 
      *    - 0 -- Speech frame 
      */
  uint8_t companding_type : 2;
    /**< BIT(2 - 3): Companding type;
      *  Applies when frame type corresponds to speech frame 
      *    –  On UL encoder sets these bits to value 1 
      *    -  on DL client is expected to set these bits to value 1 
      *    -  Invalid otherwise 
      */
  uint8_t reserved_bit_4 : 1;
    /**< Reserved. */
  uint8_t reserved_bit_5 : 1;
    /**< Reserved. */
  uint8_t reserved_bit_6 : 1;
    /**< Reserved. */
  uint8_t reserved_bit_7 : 1;
    /**< Reserved. */
}vs_vocg711alaw_v2_frame_header_t;

#endif /* __VS_VOCG711ALAW_V2_H__ */

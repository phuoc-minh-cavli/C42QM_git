#ifndef __VS_VOCAMR_H__
#define __VS_VOCAMR_H__

/**
  @file vs_vocamr.h
  @brief This file contains the AMR-NB interface definitions.
*/

/*
  ============================================================================
  Copyright (C) 2017-2018 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocamr.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"

/****************************************************************************
 * AMR-NB TYPE DEFINITIONS                                                  *
 ****************************************************************************/

/**
 * Adaptive Multirate Narrowband (AMR-NB) vocoder format.
 */
#define VS_VOCAMR_MEDIA_ID ( 0x00010FC6 )

/**
 * Enhanced Adaptive Multirate Narrowband (EAMR) vocoder.
 * Vocoder rate and frame format are similar to that of AMR-NB but 
 * vocoder frames are capable of carying wideband content.
 *
 * Network sees this as AMR_NB.
 */
#define VS_VOCEAMR_MEDIA_ID ( 0x00010FD4 )

/** Length of AMR packet speech data in bytes. */
#define VS_VOCAMR_MAX_PACKET_DATA_LENGTH  ( 34 )

/**
 * AMR codec mode. This denotes all the AMR codec bitrates.
 */
typedef enum {

  VS_VOCAMR_CODEC_MODE_0475 = 0,
    /**< 4.75 kbps. */
  VS_VOCAMR_CODEC_MODE_0515 = 1,
    /**< 5.15 kbps. */
  VS_VOCAMR_CODEC_MODE_0590 = 2,
    /**< 5.90 kbps. */
  VS_VOCAMR_CODEC_MODE_0670 = 3,
    /**< 6.70 kbps. */
  VS_VOCAMR_CODEC_MODE_0740 = 4,
    /**< 7.40 kbps. */
  VS_VOCAMR_CODEC_MODE_0795 = 5, 
    /**< 7.95 kbps. */
  VS_VOCAMR_CODEC_MODE_1020 = 6, 
    /**< 10.2 kbps. */
  VS_VOCAMR_CODEC_MODE_1220 = 7,
    /**< 12.2 kbps. */
}
  vs_vocamr_codec_mode_t;


/**
 * AMR vocoder frame types. This is used to specify
 * the contents of the AMR vocoder frame.
 *
 * These frame types are applicable to both uplink and downlink unless
 * explicitly stated otherwise.
 */
typedef enum {

  VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD = 0,
    /**< Good speech frame. */
  VS_VOCAMR_FRAME_TYPE_SPEECH_DEGRADED = 1,
    /**< Degraded speech frame. */
  VS_VOCAMR_FRAME_TYPE_ONSET = 2,
    /**< Onset -- Announces the beginning of a speech burst. This
      * does not contain information bits.  */
  VS_VOCAMR_FRAME_TYPE_SPEECH_BAD = 3,
    /**< Corrupt speech frame (bad CRC). */
  VS_VOCAMR_FRAME_TYPE_SID_FIRST = 4,
    /**< Marks the beginning of a comfort noise period (silence).
      * This does not contain information bits. */
  VS_VOCAMR_FRAME_TYPE_SID_UPDATE = 5,
    /**< Comfort noise frame. Contains information bits. */
  VS_VOCAMR_FRAME_TYPE_SID_BAD = 6,
    /**< Comfort noise frame (bad CRC).    */
  VS_VOCAMR_FRAME_TYPE_NO_DATA = 7,
    /**< No useful information (nothing to transmit). */
}
  vs_vocamr_frame_type_t;


/**
 * AMR frame Header definition.
 */
typedef struct vs_vocamr_frame_header_t {

  uint8_t codec_mode : 4;
    /**< AMR codec mode type. */
  uint8_t frame_type : 4;
    /**< AMR frame type. */

} vs_vocamr_frame_header_t;

#endif /* __VS_VOCAMR_H__ */


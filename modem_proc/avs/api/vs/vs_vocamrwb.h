#ifndef __VS_VOCAMRWB_H__
#define __VS_VOCAMRWB_H__

/**
  @file vs_vocamrwb.h
  @brief This file contains the AMR-WB interface definitions.
*/

/*
 ============================================================================
 Copyright (C) 2017 Qualcomm Technologies, Inc. 
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc. 
 ============================================================================
 $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocamrwb.h#1 $
 $Author: pwbldsvc $
 ============================================================================
*/


/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"

/****************************************************************************
 * AMR-WB TYPE DEFINITIONS                                                  *
 ****************************************************************************/

/**
 * Adaptive Multirate Wideband (AMR-WB) vocoder format.
 */
#define VS_VOCAMRWB_MEDIA_ID ( 0x00010FC7 )

/** Length of AMR WB packet speech data in bytes. */
#define VS_VOCAMRWB_MAX_PACKET_DATA_LENGTH  ( 60 )

/**
 * AMR-WB codec mode. This denotes all the AMR-WB codec bitrates.
 */
typedef enum vs_vocamrwb_codec_mode_t {

  VS_VOCAMRWB_CODEC_MODE_0660 = 0,
    /**< 6.60 kbps. */
  VS_VOCAMRWB_CODEC_MODE_0885 = 1,
    /**< 8.85 kbps. */
  VS_VOCAMRWB_CODEC_MODE_1265 = 2,
    /**< 12.65 kbps. */
  VS_VOCAMRWB_CODEC_MODE_1425 = 3,
    /**< 14.25 kbps. */
  VS_VOCAMRWB_CODEC_MODE_1585 = 4,
    /**< 15.85 kbps. */
  VS_VOCAMRWB_CODEC_MODE_1825 = 5,
    /**< 18.25 kbps. */
  VS_VOCAMRWB_CODEC_MODE_1985 = 6,
    /**< 19.85 kbps. */
  VS_VOCAMRWB_CODEC_MODE_2305 = 7, 
    /**< 23.05 kbps. */
  VS_VOCAMRWB_CODEC_MODE_2385 = 8  
    /**< 23.85 kbps. */

} vs_vocamrwb_codec_mode_t;


/**
 * AMR wideband vocoder frame types, used to specify the contents of
 * the AMR-WB vocoder frame. 
 *
 * These frame types are applicable to both uplink and downlink unless 
 * explicitly stated otherwise.
 */
typedef enum vs_vocamrwb_frame_type_t {

  VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD = 0,
    /**< Good speech frame. */
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_PROBABLY_DEGRADED = 1,
    /**< Speech frame with a good CRC, but there may
      * be errors in the class2 bits. */
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_LOST = 2,
    /**< No frame recieved. */
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_BAD = 3,
    /**< Corrupt speech frame (bad CRC). */
  VS_VOCAMRWB_FRAME_TYPE_SID_FIRST = 4,
    /**< Marks the beginning of a comfort noise period (silence).
      * This does not contain information bits. */
  VS_VOCAMRWB_FRAME_TYPE_SID_UPDATE = 5,
    /**< Comfort noise frame.
      *  This contains inforamtion bits. */
  VS_VOCAMRWB_FRAME_TYPE_SID_BAD = 6,
    /**< Comfort noise frame (bad CRC). */
  VS_VOCAMRWB_FRAME_TYPE_NO_DATA = 7
    /**< No useful information (nothing to transmit). */

} vs_vocamrwb_frame_type_t;
 

/**
 * AMR-WB frame Header structure.
 */
typedef struct vs_vocamrwb_frame_header_t {

  uint8_t codec_mode : 4;
    /**< AMR-WB codec mode type. */
  uint8_t frame_type : 4;
    /**< AMR-WB frame type. */

} vs_vocamrwb_frame_header_t;

#endif /* __VS_VOCAMRWB_H__ */


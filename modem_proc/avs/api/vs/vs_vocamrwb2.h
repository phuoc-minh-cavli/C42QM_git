#ifndef __VS_VOCAMRWB2_H__
#define __VS_VOCAMRWB2_H__

/**
  @file vs_vocamrwb2.h
  @brief This file contains the hybrid vocoder (AMR-WB in uplink and EVS AMR-WB IO in downlink) interface definitions.
*/

/*
 ============================================================================
 Copyright (C) 2018 Qualcomm Technologies, Inc. 
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc. 
 ============================================================================
 $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocamrwb2.h#1 $
 $Author: pwbldsvc $
 ============================================================================
*/


/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"

/****************************************************************************
 * HYBRID VOCODER TYPE DEFINITIONS                                                  *
 ****************************************************************************/

/**
 * Hybrid vocoder format. AMR WB in uplink and EVS AMR-WB IO in downlink.
 */
#define VS_VOCAMRWB_2_MEDIA_ID ( 0x0001333E )

/** Length of hybrid vocoder packet speech data in bytes. */
#define VS_VOCAMRWB_2_MAX_PACKET_DATA_LENGTH  ( 60 )

/**
 * Codec modes corresponding to hybrid vocoder.
 */
typedef enum vs_vocamrwb2_codec_mode_t {

  VS_VOCAMRWB2_CODEC_MODE_0660 = 0,
    /**< 6.60 kbps. */
  VS_VOCAMRWB2_CODEC_MODE_0885 = 1,
    /**< 8.85 kbps. */
  VS_VOCAMRWB2_CODEC_MODE_1265 = 2,
    /**< 12.65 kbps. */
  VS_VOCAMRWB2_CODEC_MODE_1425 = 3,
    /**< 14.25 kbps. */
  VS_VOCAMRWB2_CODEC_MODE_1585 = 4,
    /**< 15.85 kbps. */
  VS_VOCAMRWB2_CODEC_MODE_1825 = 5,
    /**< 18.25 kbps. */
  VS_VOCAMRWB2_CODEC_MODE_1985 = 6,
    /**< 19.85 kbps. */
  VS_VOCAMRWB2_CODEC_MODE_2305 = 7, 
    /**< 23.05 kbps. */
  VS_VOCAMRWB2_CODEC_MODE_2385 = 8  
    /**< 23.85 kbps. */

} vs_vocamrwb2_codec_mode_t;


/**
 * Hybrid vocoder frame types, used to specify the contents of
 * the hybrid vocoder frame. 
 */
typedef enum vs_vocamrwb2_frame_type_t {

  VS_VOCAMRWB2_FRAME_TYPE_SPEECH_GOOD = 0,
    /**< Good speech frame. */
  VS_VOCAMRWB2_FRAME_TYPE_SPEECH_PROBABLY_DEGRADED = 1,
    /**< Speech frame with a good CRC, but there may
      * be errors in the class2 bits. */
  VS_VOCAMRWB2_FRAME_TYPE_SPEECH_LOST = 2,
    /**< No frame recieved. */
  VS_VOCAMRWB2_FRAME_TYPE_SPEECH_BAD = 3,
    /**< Corrupt speech frame (bad CRC). */
  VS_VOCAMRWB2_FRAME_TYPE_SID_FIRST = 4,
    /**< Marks the beginning of a comfort noise period (silence).
      * This does not contain information bits. */
  VS_VOCAMRWB2_FRAME_TYPE_SID_UPDATE = 5,
    /**< Comfort noise frame.
      *  This contains inforamtion bits. */
  VS_VOCAMRWB2_FRAME_TYPE_SID_BAD = 6,
    /**< Comfort noise frame (bad CRC). */
  VS_VOCAMRWB2_FRAME_TYPE_NO_DATA = 7
    /**< No useful information (nothing to transmit). */

} vs_vocamrwb2_frame_type_t;
 

/**
 * Frame Header structure of hybrid vocoder.
 */
typedef struct vs_vocamrwb2_frame_header_t {

  uint8_t codec_mode : 4;
    /**< codec mode type. */
  uint8_t frame_type : 4;
    /**< frame type. */

} vs_vocamrwb2_frame_header_t;

#endif /* __VS_VOCAMRWB2_H__ */


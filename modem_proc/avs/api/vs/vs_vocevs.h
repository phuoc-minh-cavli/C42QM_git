#ifndef __VS_VOCEVS_H__
#define __VS_VOCEVS_H__

/**
  @file vs_vocevs.h
  @brief This file contains the definitions of the constants,
         data structures and interfaces for EVS vocoder
*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocevs.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"


/****************************************************************************
 * EVS TYPE DEFINITIONS                                                  *
 ****************************************************************************/

/** 
 * EVS vocoder format.
 */
#define VS_VOCEVS_MEDIA_ID ( 0x00010FD8 )


/**
 * EVS codec mode. This denotes all the EVS codec bitrates.
 */
typedef enum vs_vocevs_codec_mode_t {

  VS_VOCEVS_CODEC_MODE_0660 = 0,  /**< 06.60 kbps, AMR-WB IO (WB). */
  VS_VOCEVS_CODEC_MODE_0885 = 1,  /**< 08.85 kbps, AMR-WB IO (WB). */
  VS_VOCEVS_CODEC_MODE_1265 = 2,  /**< 12.65 kbps, AMR-WB IO (WB). */
  VS_VOCEVS_CODEC_MODE_1425 = 3,  /**< 14.25 kbps, AMR-WB IO (WB). */
  VS_VOCEVS_CODEC_MODE_1585 = 4,  /**< 15.85 kbps, AMR-WB IO (WB). */
  VS_VOCEVS_CODEC_MODE_1825 = 5,  /**< 18.25 kbps, AMR-WB IO (WB). */
  VS_VOCEVS_CODEC_MODE_1985 = 6,  /**< 19.85 kbps, AMR-WB IO (WB). */
  VS_VOCEVS_CODEC_MODE_2305 = 7,  /**< 23.05 kbps, AMR-WB IO (WB). */
  VS_VOCEVS_CODEC_MODE_2385 = 8,  /**< 23.85 kbps, AMR-WB IO (WB). */
  VS_VOCEVS_CODEC_MODE_0590 = 9,  /**< 05.90 kbps, EVS (NB, WB). */
  VS_VOCEVS_CODEC_MODE_0720 = 10, /**< 07.20 kbps, EVS (NB, WB). */
  VS_VOCEVS_CODEC_MODE_0800 = 11, /**< 08.00 kbps, EVS (NB, WB). */
  VS_VOCEVS_CODEC_MODE_0960 = 12, /**< 09.60 kbps, EVS (NB, WB, SWB). */
  VS_VOCEVS_CODEC_MODE_1320 = 13, /**< 13.20 kbps, EVS (NB, WB, SWB). */
  VS_VOCEVS_CODEC_MODE_1640 = 14, /**< 16.40 kbps, EVS (NB, WB, SWB, FB). */
  VS_VOCEVS_CODEC_MODE_2440 = 15, /**< 24.40 kbps, EVS (NB, WB, SWB, FB) */
  VS_VOCEVS_CODEC_MODE_3200 = 16, /**< 32.00 kbps, EVS (WB, SWB, FB). */
  VS_VOCEVS_CODEC_MODE_4800 = 17, /**< 48.00 kbps, EVS (WB, SWB, FB). */
  VS_VOCEVS_CODEC_MODE_6400 = 18, /**< 64.00 kbps, EVS (WB, SWB, FB). */
  VS_VOCEVS_CODEC_MODE_9600 = 19, /**< 96.00 kbps, EVS (WB, SWB, FB). */  
  VS_VOCEVS_CODEC_MODE_12800 = 20, /**< 128.00 kbps, EVS (WB, SWB, FB). */

} vs_vocevs_codec_mode_t;


/**
 * EVS frame type information. 
 */
typedef enum vs_vocevs_frame_type_t {

  VS_VOCEVS_AMR_WB_IO_6_6        = 0, /**< AMR-WB IO 6.6 kbps. */
  VS_VOCEVS_AMR_WB_IO_8_85       = 1, /**< AMR-WB IO 8.85 kbps. */
  VS_VOCEVS_AMR_WB_IO_12_65      = 2, /**< AMR-WB IO 12.65 kbps. */
  VS_VOCEVS_AMR_WB_IO_14_25      = 3, /**< AMR-WB IO 14.25 kbps. */
  VS_VOCEVS_AMR_WB_IO_15_85      = 4, /**< AMR-WB IO 15.85 kbps. */
  VS_VOCEVS_AMR_WB_IO_18_25      = 5, /**< AMR-WB IO 18.25 kbps. */
  VS_VOCEVS_AMR_WB_IO_19_85      = 6, /**< AMR-WB IO 19.85 kbps. */
  VS_VOCEVS_AMR_WB_IO_23_05      = 7, /**< AMR-WB IO 23.05 kbps. */
  VS_VOCEVS_AMR_WB_IO_23_85      = 8, /**< AMR-WB IO 23.85 kbps. */
  VS_VOCEVS_AMR_WB_IO_2_0_SID    = 9, /**< AMR-WB IO 2.0 kbps SID. */
  VS_VOCEVS_RESERVED_1          = 10, /**< RESERVED. */
  VS_VOCEVS_RESERVED_2          = 11, /**< RESERVED. */
  VS_VOCEVS_RESERVED_3          = 12, /**< RESERVED. */
  VS_VOCEVS_RESERVED_4          = 13, /**< RESERVED. */
  VS_VOCEVS_SPEECH_LOST         = 14, /**< SPEECH LOST. */
  VS_VOCEVS_NO_DATA             = 15, /**< NO DATA. */
  VS_VOCEVS_EVS_PRIMARY_2_8     = 16, /**< EVS Primary 2.8 kbps. */
  VS_VOCEVS_EVS_PRIMARY_7_2     = 17, /**< EVS Primary 7.2 kbps. */
  VS_VOCEVS_EVS_PRIMARY_8_0     = 18, /**< EVS Primary 8.0 kbps. */
  VS_VOCEVS_EVS_PRIMARY_9_6     = 19, /**< EVS Primary 9.6 kbps. */
  VS_VOCEVS_EVS_PRIMARY_13_2    = 20, /**< EVS Primary 13.2 kbps. */
  VS_VOCEVS_EVS_PRIMARY_16_4    = 21, /**< EVS Primary 16.4 kbps. */
  VS_VOCEVS_EVS_PRIMARY_24_4    = 22, /**< EVS Primary 24.4 kbps. */
  VS_VOCEVS_EVS_PRIMARY_32_0     =23, /**< EVS Primary 32.0 kbps. */
  VS_VOCEVS_EVS_PRIMARY_48_0    = 24, /**< EVS Primary 48.0 kbps. */
  VS_VOCEVS_EVS_PRIMARY_64_0    = 25, /**< EVS Primary 64.0 kbps. */
  VS_VOCEVS_EVS_PRIMARY_96_0    = 26, /**< EVS Primary 96.0 kbps. */
  VS_VOCEVS_EVS_PRIMARY_128_0   = 27, /**< EVS Primary 128.0 kbps. */
  VS_VOCEVS_EVS_PRIMARY_2_4_SID = 28, /**< EVS Primary 2.4 kbps SID. */
  VS_VOCEVS_RESERVED_5          = 29, /**< RESERVED. */
  VS_VOCEVS_RESERVED_6          = 30, /**< RESERVED. */
  VS_VOCEVS_RESERVED_7          = 31  /**< RESERVED. */

} vs_vocevs_frame_type_t;


/**
 * EVS frame header structure.
 */
typedef struct vs_vocevs_frame_header_t {

  uint8_t frame_type : 5;
    /**< EVS frame type. */
  uint8_t pfi : 1;
    /**< BIT(5): Paratial Frame Indicator.
     *    1 - Partial Redundant Frame.
     *    0 - Normal Frame.
     */
  uint8_t reserved_bits : 2;
    /**< Reserved bits. */
} vs_vocevs_frame_header_t;

#endif /* __VS_VOCEVS_H__ */

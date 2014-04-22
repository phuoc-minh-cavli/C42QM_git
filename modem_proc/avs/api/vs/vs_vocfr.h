#ifndef __VS_VOCFR_H__
#define __VS_VOCFR_H__

/**
  @file vs_vocfr.h
  @brief This file contains the definitions of the constants,
    	 data structures, and interfaces for GSM Full Rate (FR)
    	 vocoder.
*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocfr.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"

/****************************************************************************
 * EFR TYPE DEFINITIONS                                                  *
 ****************************************************************************/

/** 
 * GSM Full Rate (FR) vocoder format.
 */
#define VS_VOCFR_MEDIA_ID ( 0x00010FC9 )

/** Length of FR packet speech data in bytes. */
#define VS_VOCFR_MAX_PACKET_DATA_LENGTH ( 33 )

/**
 * GSM FR vocoder frame Header structure.
 */
typedef struct vs_vocfr_frame_header_t
{
  uint8_t bfi : 1;
    /**< BIT(0): Bad Frame Indication (BFI); used for error concealment.
      * The BFI applies to downlink only. Unused for uplink.
      */
  uint8_t sid : 2;
    /**< BIT(1-2): Silence Insertion Descriptor (SID);
      * Applies to both uplink and downlink: \n
      *    - 3 -- Reserved \n
      *    - 2 -- Valid SID frame \n
      *    - 1 -- Invalid SID frame \n
      *    - 0 -- Speech frame @tablebulletend
      */
  uint8_t taf : 1;
    /**< BIT(3): Time alignment flag.
      * The TAF applies to downlink only. Unused for uplink: \n
      * - 1 -- Position of the SID frame within the Slow Associated Control
      *         Channel (SACCH) multiframe \n
      * - 0 -- Frame is not aligned @tablebulletend
      */
  uint8_t reserved_bit_4 : 1; 
    /**< Reserved. */
  uint8_t reserved_bit_5 : 1;
    /**< Reserved. */
  uint8_t reserved_bit_6 : 1;
    /**< Reserved. */
  uint8_t reserved_bit_7 : 1;
    /**< Reserved. */

} vs_vocfr_frame_header_t;

#endif /* __VS_VOCFR_H__ */



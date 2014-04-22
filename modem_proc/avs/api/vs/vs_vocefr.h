#ifndef __VS_VOCEFR_H__
#define __VS_VOCEFR_H__

/**
  @file vs_vocefr.h
  @brief This file contains the EFR interface definitions.
*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_vocefr.h#1 $
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
 * GSM Enhanced Full Rate (EFR) vocoder format.
 */
#define VS_VOCEFR_MEDIA_ID ( 0x00010FC8 )

/** Length of EFR packet speech data in bytes. */
#define VS_VOCEFR_MAX_PACKET_DATA_LENGTH ( 31 )

/**
 * EFR frame Header structure.
 */
typedef struct vs_vocefr_frame_header_t {

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
      *    - 0 -- Speech frame @tablebulletend.
	  */
  uint8_t taf : 1;
    /**< BIT(3): Time Alignment Flag (TAF). The TAF applies to downlink only.
      *  Unused for uplink: \n
      *  - 1 -- Position of the SID frame within the Slow Associated Control
      *         Channel (SACCH) multiframe \n
      *  - 0 -- Frame is not aligned @tablebulletend
      */
  uint8_t reserved_bit_4 : 1;
    /**< Reserved. */
  uint8_t reserved_bit_5 : 1;
    /**< Reserved. */
  uint8_t reserved_bit_6 : 1;
    /**< Reserved. */
  uint8_t reserved_bit_7 : 1;
    /**< Reserved. */

} vs_vocefr_frame_header_t;

#endif /* __VS_VOCEFR_H__ */



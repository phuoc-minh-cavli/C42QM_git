/*===========================================================================
   FILE          mpm_utils.c
 
   DESCRIPTION   Contains implementation of public MPM utility functions.

                 Copyright © 2011-2012 Qualcomm Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

   $Header: //components/rel/core.tx/6.0/power/mpm/src/mpm_utils.c#1 $
=============================================================================*/

/*============================================================================
                           INCLUDE FILES FOR MODULE
============================================================================*/

#include "mpm_utils.h"
#include "HALmpmintInternal.h"

/*===========================================================================
 *                        GLOBAL FUNCTION DEFINITIONS
 * =========================================================================*/

/*
 * mpm_set_wakeup_timetick
 */

void mpm_set_wakeup_timetick(uint64_t wakeup_tick)
{
  uint32 nReg;
  nReg = registerLayout.WakeupReg;

  /* Capturing lower 32 bits */
  outpdw(nReg, (uint32)(wakeup_tick & UINT32_MAX));

  nReg += sizeof(uint32);

  /* Caputuring upper 32 bits */
  outpdw(nReg, (uint32)((wakeup_tick >> 32) & UINT32_MAX));

  return;
}

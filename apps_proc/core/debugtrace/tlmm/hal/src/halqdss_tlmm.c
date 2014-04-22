/*=============================================================================

FILE:         halqdss_tlmm.c

GENERAL DESCRIPTION
   This file implements HAL for the top-level mode multiplexor modules
   used by the QDSS system.

   TLMM_CSR
      TLMM_ETM_MODE*
      TLMM_SDC2_HDRV*

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/tlmm/hal/src/halqdss_tlmm.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "HALhwio.h"
#include "halqdss_tlmm.h"     // Must precede include of qdss_tlmm_regs.h
#include "qdss_tlmm_regs.h"   // Needs TLMM_BASE from halqdss_tlmm.h

/*---------------------------------------------------------------------------
 * Global Variable Definitions
 * ------------------------------------------------------------------------*/

volatile uint32 TLMM_BASE;

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

void HAL_qdss_tlmm_HalConfigInit(uint32 nBaseAddr)
{
   TLMM_BASE = nBaseAddr;
}

//=========================================================================//
void HAL_qdss_tlmm_SetEtmMode(uint32 val)
{
   HWIO_TLMM_ETM_MODE_OUT(val);
}

void HAL_qdss_tlmm_EtmMode_POR(void)
{
   HWIO_TLMM_ETM_MODE_OUT(HWIO_TLMM_ETM_MODE_POR);
}

//=========================================================================//
void HAL_qdss_tlmm_SetSdc2HdrvPullCtl(uint32 val)
{
   HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUT(val);
}

void HAL_qdss_tlmm_Sdc2HdrvPullCtl_POR(void)
{
   HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUT(HWIO_TLMM_SDC2_HDRV_PULL_CTL_POR);
}



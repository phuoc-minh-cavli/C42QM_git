#ifndef HALQDSS_TLMM_H
#define HALQDSS_TLMM_H

/*=============================================================================

FILE:         halqdss_tlmm.h

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

extern volatile uint32 TLMM_BASE;

void HAL_qdss_tlmm_HalConfigInit(uint32 nBaseAddr);

void HAL_qdss_tlmm_SetEtmMode(uint32 val);    //Set ETM Mode
void HAL_qdss_tlmm_EtmMode_POR(void);         //Set to Power-on-reset value

void HAL_qdss_tlmm_SetSdc2HdrvPullCtl(uint32 val);  // Set
void HAL_qdss_tlmm_Sdc2HdrvPullCtl_POR(void); //Set to Power-on-reset value

#endif //HALQDSS_TLMM_H


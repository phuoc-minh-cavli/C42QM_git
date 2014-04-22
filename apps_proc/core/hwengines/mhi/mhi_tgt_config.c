/**
 * @file mhi_tgt_config.c
 * @brief
 * Has the target configurable parameters.
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================

Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi_i.h"
#include "mhi_tgt.h"

const mhi_core_tgt_config_t mhi_core_tgt_config =
{
   MHI_ISR_INVALID_VEC,  /* MHI interrupt vector */
   0x01000000,           /* MHI version */
   0x87000,              /* Physical address of MMIO base */ 
   39,                   /* Maximum number of Link Request Objects (less than maximum number of DMA descriptors supported) */
   1,                    /* Timeout for pending DMA completions in ms */
   0x00080188,           /* Local physical address for writing MSI */
#ifdef MHI_SUPPORT_IPABAM
   0x7B72000,            /* CHDB target address for MHI HW ACC */
   0x7B72080,            /* EDB target address for MHI HW ACC */
#else /* MHI supports GSI */
   0x7B22000,            /* CHDB target address for MHI HW ACC */
   0x7B22150,            /* EDB target address for MHI HW ACC */
#endif
   200,                  /* MHI CORE Worker Thread Priority */
   209,                  /* Highest MHI CORE Worker Thread Priority */
   100                   /* TEST thread priority */    
};



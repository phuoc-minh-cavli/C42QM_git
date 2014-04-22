#ifndef MHI_TGT_H
#define MHI_TGT_H
/**
 * @file mhi_tgt.h
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

#include "comdef.h"

/** MHI target specific configuration */ 
typedef struct _mhi_core_tgt_config
{
   uint32 mhi_isr_vec;                    /* MHI interrupt vector */
   uint32 mhiversion;                     /* MHI version */
   uint32 mmio_basephys;                  /* Physical address of MMIO base */ 
   uint32 max_linkreq;                    /* Maximum number of Link Request Objects (less than maximum number of DMA descriptors supported)  */
   uint32 link_completion_timeout;        /* Timeout for pending DMA completions in ms */
   uint32 msiaddr;                        /* Local physical address for writing MSI */
   uint32 mhi_hw_acc_mailbox_cdb_target;  /* CHDB target address for MHI HW ACC */
   uint32 mhi_hw_acc_mailbox_edb_target;  /* EDB target address for MHI HW ACC */
   uint32 mhi_worker_priority;            /* MHI CORE Worker Thread Priority */
   uint32 mhi_worker_priority_high;       /* Highest MHI CORE Worker Thread Priority */
   uint32 mhi_test_priority;              /* TEST thread priority */ 
} mhi_core_tgt_config_t;

/** 
 * @brief      mhi_get_core_tgt_info
 *
 *             Get target specific information for core 
 *               
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Pointer to target specific information for core
 * 
 * @sideeffect None 
 */
const mhi_core_tgt_config_t *mhi_get_core_tgt_info(void);

#endif /* #ifndef MHI_TGT_H */

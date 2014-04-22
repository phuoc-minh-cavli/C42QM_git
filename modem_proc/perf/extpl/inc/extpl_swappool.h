/*====================================================================
 *
 * FILE: extpl_swappool.h
 *
 * SERVICES: external pager swappool header file
 *
 * DESCRIPTION: This file defines the 
 *              external pager swappool data structure
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 *==================================================================*/


/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/perf.mpss/2.1.2.0/extpl/inc/extpl_swappool.h#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/18/17   che     Initial Version

===========================================================================*/

#ifndef __EXTPL_SWAPPOOL_H__
#define __EXTPL_SWAPPOOL_H__

#define EXTPL_PAGE_SHIFT 12
#define EXTPL_PAGE_SIZE (1<<EXTPL_PAGE_SHIFT)

#ifdef ALIGN 
#undef ALIGN
#define ALIGN(addr, size)  ((addr) & ~(size-1))
#endif

#include "flash_ftl.h"

typedef struct _extpl_module_swap_pool {
    unsigned int swap_size;
    qurt_mem_region_t region_rx;   /**< QuRT memory region handled (if sourced from a pool) */
    qurt_mem_region_t region_rw;   /**< QuRT memory region handled (if sourced from a pool) */
    qurt_paddr_t      paddr_base;  /**< Physical address of this swap memory */
    qurt_addr_t       vaddr_rx;    /**< Internal Virtual address for RX usage */
    qurt_addr_t       vaddr_rw;    /**< Internal Virtual address for RW usage */
} extpl_module_swap_pool_t;


extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_page_size__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_text_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_text_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_rodata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_rodata_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_rwdata_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_rwdata_end__;

extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_bss_start__;
extern unsigned int __attribute__ ((weak)) __swapped_segments_extpl_bss_end__;

extern FLASH_FTL_STATUS extpl_load_pages
(
    unsigned int start_vpn, 
    unsigned int end_vpn, 
    int partition, 
    unsigned int dst_va
);

extern FLASH_FTL_STATUS extpl_save_pages
(
    unsigned int start_vpn, 
    unsigned int end_vpn, 
    unsigned int src_va
);


extern void extpl_swappool_init(unsigned int rx_size, unsigned int rw_size);
extern extpl_status_t extpl_swappool_open_module(extpl_module_id_t module_id);
extern extpl_status_t extpl_swappool_close_module(extpl_module_id_t module_id);
extern extpl_status_t extpl_swappool_activate_module(extpl_module_id_t module_id);
extern extpl_status_t extpl_swappool_deactivate_module(extpl_module_id_t module_id);


#define PAGING_PARTITION 0
#define PAGING_PARTITION_M1 2
#define SWAP_PARTITION 1

#endif













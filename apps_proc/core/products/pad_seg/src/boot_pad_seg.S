;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;             ARM V4 architecture assembly language MMU support
;
; GENERAL DESCRIPTION
;   These 4K areas are defined to align the segments in the scatterload file 
;   to 4K for L4.  This is the only way to accomplish that - derived from 6800
;
; EXTERNALIZED FUNCTIONS
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   
; Copyright (c) 2004-2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;  This section contains comments describing changes made to the
;  Notice that changes are listed in reverse chronological order.
;
;$Header: //components/rel/core.tx/6.0/products/pad_seg/src/boot_pad_seg.S#1 $ $DateTime: 2020/01/30 22:49:35 $
;
;when       who     what, where, why
;--------   ---     ----------------------------------------------------------
;06/29/07   sam     Added padding for uninit segment.
;06/14/07   dp      Initial revision

;============================================================================
;
;                             MODULE INCLUDES
;
;============================================================================

#include "customer.h"

;============================================================================

#if !defined(BUILD_BOOT_CHAIN) && !defined(BUILD_BOOT_CHAIN_OEMSBL)
;============================================================================
   AREA  APPS_CODE_SEGMENT_START, DATA, READONLY, ALIGN = 12
mmu_4k_pad_apps_code
   % 4096
   
   AREA  APPS_DATA_SEGMENT_START, DATA, READONLY, ALIGN = 12
mmu_4k_pad_apps_data
   % 4096

   AREA  APPS_UNINIT_SEGMENT_START, NOINIT, ALIGN = 12
mmu_4k_pad_apps_uninit
   % 4096
   
   AREA  IRAM_CODE_SEGMENT_START, NOINIT, ALIGN = 12
mmu_4k_pad_iram_code_uninit
   % 4096  

#if defined (FEATURE_L4_DEMAND_LOAD_BOOT)
   AREA  DLPOOL_SEGMENT_START, DATA, READONLY, ALIGN = 12
mmu_4k_pad_dlload_code
   % 4096 
   
   AREA DLPOOL_SEGMENT_PAD1, DATA, READONLY, ALIGN = 12
mmu_4k_pad_dlpool_code1
   % 2097152

   AREA DLPOOL_SEGMENT_PAD2, DATA, READONLY, ALIGN = 12
mmu_4k_pad_dlpool_code2
   % 2097152

   AREA  SWAPPED_SEGMENT_START, DATA, READONLY, ALIGN = 12
mmu_4k_pad_apps_code_swapped_start
   % 4096   

   AREA  SWAPPED_SEGMENT_END, DATA, READONLY, ALIGN = 12
mmu_4k_pad_apps_code_swapped_end
   % 4096   

   AREA  MSM_LAST_ENTRY_SEGMENT_END, DATA, READONLY, ALIGN = 12
mmu_4k_pad_last_entry_data
   % 4096   

#endif 

#endif /* !BUILD_BOOT_CHAIN && !BUILD_BOOT_CHAIN_OEMSBL */  
   END

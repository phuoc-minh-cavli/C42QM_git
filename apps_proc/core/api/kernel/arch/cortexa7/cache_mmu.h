#ifndef CACHE_MMU_H
#define CACHE_MMU_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     A R M 1 1 3 6 E J - S   C A C H E   A N D   M M U   C O N T R O L

DESCRIPTION

  This header file contains the public interface for the ARM1136EJ-S cache
  and mmu control functions.
          
EXTERNALIZED FUNCTIONS
  
boot_cache_mmu_init
  Initialize the ARM1136EJ-S MMU and enable the MMU and caches.
  
cache_mmu_disable
  Disable the ARM1136EJ-S MMU and caches.
  
cache_mmu_re_enable
  Re-enable the ARM1136EJ-S MMU and caches.
  
mmu_invalidate_data_cache
  Invalidate all ARM1136EJ-S data cache lines.
  
mmu_invalidate_data_cache_lines
  Invalidate ARM1136EJ-S cache lines for a specified address range.
  
mmu_invalidate_instruction_cache
  Invalidate all ARM1136EJ-S data cache lines.

mmu_enable_branch_prediction
  Enable the ARM1136EJ-S branch prediction.
  
mmu_enable_instruction_cache
  Enable the ARM1136EJ-S instruction cache.

mmu_enable_peripheral_remap
  Setup Peripheral Port Memory Remap Register for peripheral bus access.

mmu_disable_peripheral_remap
  Clears Peripheral Port Memory Remap Register.

mmu_set_control_reg
  Sets a default value into the mmu control register.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cache_set_memory_barrier_location() must be called with a pointer
  to non-buffered memory before any call to memory_barrier(), 
  for the instruction and data streams to be syncronized by
  the memory_barrier() operation.
  
Copyright (c) 2003-2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/kernel/arch/cortexa7/cache_mmu.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/07   mjs     Changes to support obtaining memory barrier location
                   using new SMEM interface smem_alloc().
01/27/06   sd      Exported function to set default control register cfg. 
01/22/06   sd      Added support for aARM boot loader.

============================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#ifdef __cplusplus
		extern "C"
		{
#endif
   

/*===========================================================================
                DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
#define ARCH_DCACHE_LINE_SIZE 64
#define ARCH_ICACHE_LINE_SIZE 32


/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/

/*===========================================================================
  FUNCTION  BOOT_CACHE_MMU_INIT

  DESCRIPTION
    Initialize and enable the MMU, and enable the I-cache and D-cache.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    0 = successful completion.

  SIDE EFFECTS
    None.
===========================================================================*/
int boot_cache_mmu_init( void );


/*===========================================================================
  FUNCTION  CACHE_MMU_DISABLE

  DESCRIPTION
    Disable the ARM1136EJ-S MMU and caches.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    Instruction and data caches will be disabled.
===========================================================================*/
void cache_mmu_disable( void );


/*===========================================================================
  FUNCTION  CACHE_MMU_RE_ENABLE

  DESCRIPTION
    Re-enable the ARM1136EJ-S MMU and caches.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    Instruction and data caches will be enabled.
===========================================================================*/
void cache_mmu_re_enable( void );
  

/*===========================================================================
  FUNCTION  MMU_INVALIDATE_DATA_CACHE

  DESCRIPTION
    Invalidate all ARM1136EJ-S data cache lines.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    All data cache lines will be invalidated.

===========================================================================*/
void mmu_invalidate_data_cache( void );
  

/*===========================================================================
  FUNCTION  MMU_INVALIDATE_DATA_CACHE_LINES

  DESCRIPTION
    Invalidate ARM1136EJ-S cache lines for a specified address range.
      
  PARAMETERS
    start_ptr - base address of data region to invalidate.
    byte_cnt  - number of bytes in data region to invalidate.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    Data cache lines that contain bytes from the specified region will be
    invalidated.

===========================================================================*/
void mmu_invalidate_data_cache_lines
(
  uint32*  start_ptr,
  uint32   byte_cnt
);


/*===========================================================================
  FUNCTION  MMU_INVALIDATE_INSTRUCTION_CACHE

  DESCRIPTION
    Invalidate all ARM1136EJ-S instruction cache lines.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    All instruction cache lines will be invalidated.
===========================================================================*/
void mmu_invalidate_instruction_cache( void );
  
/*===========================================================================
  FUNCTION  MMU_ENABLE_INSTRUCTION_CACHE

  DESCRIPTION
    Enable the ARM1136EJ-S instruction cache.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The ARM1136EJ-S instruction cache will be enabled.

===========================================================================*/
void mmu_enable_instruction_cache( void );

/*============================================================================
; dcache_clean_only_region
;
; PROTOTYPE
;   void dcache_clean_only_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Cleans a memory region in the cache.    
;============================================================================*/
void dcache_clean_only_region(void *addr, unsigned int length);

/*============================================================================
; dcache_inval_region
;
; PROTOTYPE
;   void dcache_inval_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Invalidate a memory region in the cache.    
;============================================================================*/
void dcache_inval_region(void *addr, unsigned int length);

/*============================================================================
; dcache_clean_region
;
; PROTOTYPE
;   void dcache_clean_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Cleans a memory region in the cache.    
;============================================================================*/
void dcache_clean_region(void *addr, unsigned int length);


/*============================================================================
; memory_barrier
;
; PROTOTYPE
;   void memory_barrier(void);
;
; ARGS
;
; DESCRIPTION
;   Creates a memory barrier
;
; DEPENDENCIES
;   cache_set_memory_barrier_location() must be called with a pointer
;   to non-buffered memory before any call to memory_barrier(),
;   for the instruction and data streams to be syncronized by
;   the memory_barrier() operation.
;============================================================================*/
void memory_barrier(void);

#ifdef __cplusplus
}
#endif
#endif /* CACHE_MMU_H */

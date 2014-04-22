#ifndef CACHE_MMU_H
#define CACHE_MMU_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     A R M 9 2 6 E J - S   C A C H E   A N D   M M U   C O N T R O L

DESCRIPTION

  This header file contains the public interface for the ARM926EJ-S cache
  and mmu control functions.
          
EXTERNALIZED FUNCTIONS
  
boot_cache_mmu_init
  Initialize the ARM926EJ-S MMU and enable the MMU and caches.
  
cache_mmu_disable
  Disable the ARM926EJ-S MMU and caches.
  
cache_mmu_re_enable
  Re-enable the ARM926EJ-S MMU and caches.
  
mmu_invalidate_data_cache
  Invalidate all ARM926EJ-S data cache lines.
  
mmu_invalidate_data_cache_lines
  Invalidate ARM926EJ-S cache lines for a specified address range.
  
mmu_invalidate_instruction_cache
  Invalidate all ARM926EJ-S data cache lines.
  
mmu_enable_instruction_cache
  Enable the ARM926EJ-S instruction cache.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  None.

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/kernel/arch/arm9/cache_mmu.h#1 $    

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/07   mjs     Changes to support obtaining memory barrier location
                   using new SMEM interface smem_alloc().
10/16/06   mjs     Added icache_inval_region for boot chain.
08/16/05   sd      Merged from 6800.
10/01/03   st      Created.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "msm.h" 
#ifdef __cplusplus
		extern "C"
		{
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/



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
    None.
    
  SIDE EFFECTS
    None.

===========================================================================*/
void boot_cache_mmu_init( void );


/*===========================================================================

  FUNCTION  CACHE_MMU_DISABLE

  DESCRIPTION
    Disable the ARM926EJ-S MMU and caches.
      
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
    Re-enable the ARM926EJ-S MMU and caches.
      
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
    Invalidate all ARM926EJ-S data cache lines.
      
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

  FUNCTION  MMU_CLEAN AND_INVALIDATE_DATA_CACHE

  DESCRIPTION
    Clean and invalidate entire ARM926EJ-S data cache lines.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    All data cache lines will be invalidated.

===========================================================================*/
void mmu_clean_and_invalidate_data_cache(void);


/*===========================================================================

  FUNCTION  MMU_INVALIDATE_DATA_CACHE_LINES

  DESCRIPTION
    Invalidate ARM926EJ-S cache lines for a specified address range.
      
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

  FUNCTION  MMU_INVALIDATE_DATA_CACHE_LINES

  DESCRIPTION
    Clean and invalidate ARM926EJ-S cache lines for a specified address range.
      
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
void mmu_clean_and_invalidate_data_cache_lines
(
  uint32*  start_ptr,
  uint32   byte_cnt
);
/*===========================================================================

  FUNCTION  MMU_INVALIDATE_INSTRUCTION_CACHE

  DESCRIPTION
    Invalidate all ARM926EJ-S instruction cache lines.
      
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
    Enable the ARM926EJ-S instruction cache.
      
  PARAMETERS
    None.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The ARM926EJ-S instruction cache will be enabled.

===========================================================================*/
void mmu_enable_instruction_cache( void );

/*===========================================================================

  FUNCTION  MMU_INVALIDATE_TLB_SINGLE_ENTRY

  DESCRIPTION
    Invalidates the ARM926EJ-S TLB entry for the specified  address.

  PARAMETERS
    The virtual address for which the TLB entry needs to be invalidated.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The ARM926EJ-S instruction cache will be enabled.

===========================================================================*/

void  mmu_invalidate_tlb_single_entry
(
	uint32 virtual_address
);

/*===========================================================================
; MMU_RE_ENABLE_DATA_CACHE
;
;  DESCRIPTION -
;        Re-Enables  the ARM MMU data cache
===========================================================================*/
void mmu_re_enable_data_cache( void );

/*===========================================================================
; MMU_DISBLE_DATA_CACHE
;
;  DESCRIPTION -
;        Enables  the ARM MMU data cache
===========================================================================*/
void mmu_disable_data_cache( void );

/*============================================================================
; dcache_flush_line
;
; PROTOTYPE
;   void dcache_flush_line(void *addr);
;
; ARGS
;   void *addr            - Address in cache line
;
; DESCRIPTION
;   Cleans and invalidates a line in the cache
;============================================================================*/
void dcache_flush_line(void *addr);

/*============================================================================
; dcache_flush_region
;
; PROTOTYPE
;   void dcache_flush_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Cleans and invalidates a memory region in the cache.    
;============================================================================*/
void dcache_flush_region(void *addr, unsigned int length);

/*============================================================================
; dcache_inval_line
;
; PROTOTYPE
;   void dcache_inval_line(void *addr);
;
; ARGS
;   void *addr            - Address in cache line
;
; DESCRIPTION
;   Invalidate a line in the cache
;============================================================================*/
void dcache_inval_line(void *addr);

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
; dcache_clean_line
;
; PROTOTYPE
;   void dcache_clean_line(void *addr);
;
; ARGS
;   void *addr            - Address in cache line
;
; DESCRIPTION
;   Cleans a line in the cache
;============================================================================*/
void dcache_clean_line(void *addr);

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
;   cache_set_memory_barrier_location() must be called with a pointer to
;   non-buffered or non-cached memory before any call to memory_barrier(),
;   for the instruction and data streams to be syncronized by
;   the memory_barrier() operation.
;============================================================================*/
void memory_barrier(void);
#ifdef __cplusplus
}
#endif
#endif /* CACHE_MMU_H */

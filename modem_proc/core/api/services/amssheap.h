#ifndef AMSSHEAP_H
#define AMSSHEAP_H
/**
  @file amssheap.h
  @brief A simple sub-allocator to manage memory allocations and deallocations
  using a Next Fit strategy. 

*/

/*===========================================================================*/
/*
Copyright (c) 2013-16 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
*/
/*===========================================================================

                                 Edit History

$Header: //components/rel/core.mpss/3.10/api/services/amssheap.h#1 $ $DateTime: 2018/06/26 03:18:17 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     --------------------------------------------------------
12/12/13   rks     Created.
===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */

#include <stddef.h>
#include "memheap.h"

/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
   extern "C"
   {
#endif
/*===========================================================================

FUNCTION amssheap_get_current_heap_usages

DESCRIPTION
  returns the current amss_mem_heap usage(i.e. in use memory)

DEPENDENCIES
  None

SIDE EFFECTS
  None
ARGUMENTS
  None

===========================================================================*/
unsigned long amssheap_get_current_heap_usages(void);

/*===========================================================================

FUNCTION amssheap_get_current_in_use_blocks_count

DESCRIPTION
  returns the total current in use blocks in amss_mem_heap

DEPENDENCIES
  None

SIDE EFFECTS
  None
ARGUMENTS
  None
===========================================================================*/
unsigned long amssheap_get_current_in_use_blocks_count(void);

/*===========================================================================

FUNCTION amssheap_mem_get_block_size

DESCRIPTION
  Returns the block size from a assigned ptr in the amss_mem_heap.

  @param[in] void Pointer to the memory block to be queried.

  @return
  Returns the size (in bytes) of the block, excluding the block header 
  overhead and the unused bytes at the end of the block.

===========================================================================*/
unsigned long amssheap_mem_get_block_size (void *ptr);

/*===========================================================================

FUNCTION amssheap_add_section

DESCRIPTION
  Add a memory section to amss_mem_heap.

===========================================================================*/
void amssheap_add_section(
   /* Pointer to contiguous block of memory used for this section  */
   void                          *section_start,
   /* Size of the section  */
   unsigned long                  section_size
);

/**
  Function : amssheap_get_task_stats
  Description :
     Returns the amss_mem_heap statistics for a thread with given memheap TLS index 

  @param[in]  thread_id    Thread index
  @param[out] stats double pointer to the heap statistics structure having the 
              statistics for the given thread id. 

  @return
   0 SUCCESS
  -1 Invalid arguments
  -2 Heap Statistics not enabled for this heap

  @dependencies
  The heap must have been initialized previously. \n
*/
#ifdef FEATURE_MEM_DEBUG
int amssheap_get_task_stats(uint32 thread_id, memheap_task_stats_type **stats);
#endif



#ifdef __cplusplus
   }
#endif

#endif /* AMSSHEAP_H */

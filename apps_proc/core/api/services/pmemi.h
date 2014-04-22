#ifndef PMEMI_H
#define PMEMI_H
/*==========================================================================

               Physical Memory Library Internal Header File

DESCRIPTION
  This file contains the definitions for internal use by the PMEM library.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/core.tx/6.0/api/services/pmemi.h#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 12/13/07  jc     Merged WinMobile branch.
 01/11/07  jc     Added feature to allocate MM_HEAP1 from L4 iguana.
                  Added support for SMI self-refresh.
 11/09/06  jc     Added support for IMEM and aDSP heaps, "small heap"
                  allocator, improved TCM heap management, and SMI disable.
 05/16/06  jc     Added dynamic heap add, delete, and reconfiguration.
 05/05/06  jc     Added statistics and heap consistency checks.
 01/15/06  jc     Initial release.

============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "memheap.h"

/*============================================================================
*                         CONSTANT DEFINITIONS
============================================================================*/

/* Number of free blocks to list in the heap statistics */
#define PMEM_INFO_FREEBLOCKS    64

/* Code to enter and exit critical sections */
/* PMEM_CRITICAL_BEGIN is not defined previously - CMI Cleanup */
#define PMEM_CRITICAL_BEGIN     pmem_critical_begin
#define PMEM_CRITICAL_END       pmem_critical_end

/* Default address boundary for PMEM allocations */
#define PMEM_DEF_BOUNDARY       1024
#define PMEM_DEF_BOUNDARY_MASK  (PMEM_DEF_BOUNDARY - 1)

/* Invalid PMEM identifier */
#define PMEM_INVALID_ID         PMEM_NUM_IDS

/*============================================================================
                          DATA DECLARATIONS
============================================================================*/

/* Heap information data */
typedef struct pmemi_info_s
{
  /* Overall heap allocation statistics */
  mem_heap_totals_type heap_totals;
  unsigned int freeblocks[PMEM_INFO_FREEBLOCKS];

} pmemi_info_s_type;

/* Information data returned upon freeing of a block */
typedef struct pmemi_free_info_s
{
  unsigned int id;
  size_t size;
} pmemi_free_info_s_type;

/* Heap-specific memory allocator function table */
typedef struct pmemi_func_s
{
  void *(*malloc)(size_t size, unsigned int id);
  void *(*realloc)(void *ptr, size_t size, pmemi_free_info_s_type *info);

  /* The following functions return zero on success and -1 on failure */
  int (*free)(void *ptr, pmemi_free_info_s_type *info);
  int (*heap_init)(void *start, size_t size, unsigned int attr);
  int (*heap_info)(pmemi_info_s_type *ptr);
  int (*heap_resize)(void *start, size_t size);
  int (*heap_delete)(void *start);

  /* The following function returns TRUE if the heap is free, FALSE otherwise */
  int (*heap_is_free)(void *start);

  /* The following functions are called whenever the access state of
   * the heap changes.  Unlike the functions above, these are supported
   * only for memheap-managed heaps, and not for target-specific heaps.
   */
  void (*heap_access_enable)(void *start);
  void (*heap_access_disable)(void *start);
} pmemi_func_s_type;

/* This structure keeps track of our heaps */
typedef struct heap_control
{
  unsigned int attributes;
  size_t size;
  void *start;
  void *end;
  pmemi_func_s_type func;
  mem_heap_type mem_heap;
  void *first_malloc;
} heap_control_s_type;

/*============================================================================
*                         MACRO DEFINITIONS
============================================================================*/

/*============================================================================
*                         EXPORTED FUNCTIONS
============================================================================*/

/*===========================================================================

FUNCTION pmem_msm_init

DESCRIPTION
  This function performs MSM-specific initialization of the pmem library.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pmem_msm_init(void);

/*===========================================================================

FUNCTION pmem_msm_get_funcs

DESCRIPTION
  This function fills in the heap-specific functions for the specified
  memory region.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pmem_msm_get_funcs
(
  void *start,            /* Memory region start address */
  unsigned int attr,      /* Memory region PMEM attributes */
  pmemi_func_s_type *func  /* Function table filled in by this function */
);

/*===========================================================================

FUNCTION find_attributes

DESCRIPTION
  Find the attributes corresponding to the id given

DEPENDENCIES
  None

RETURN VALUE
  int - 0 on success and -1 on failure.

SIDE EFFECTS
  None

===========================================================================*/
int pmem_find_attributes
(
  unsigned int id,  /* A unique ID that identifies the request */
  uint32 *attr      /* The returned attributes */
);

/*===========================================================================

FUNCTION pmem_critical_begin

DESCRIPTION
  This function must be called when entering a PMEM critial section.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pmem_critical_begin(void);

/*===========================================================================

FUNCTION pmem_critical_end

DESCRIPTION
  This function must be called when exiting a PMEM critial section.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pmem_critical_end(void);

/*===========================================================================

FUNCTION pmem_heap_find

DESCRIPTION
  Find the heap corresponding to the specified address.

DEPENDENCIES
  None

RETURN VALUE
  heap_control_s_type - pointer to heap, or NULL if not found

SIDE EFFECTS
  None

===========================================================================*/
heap_control_s_type *pmem_heap_find(void *address);

#endif  /* PMEMI_H */

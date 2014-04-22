/* =========================================================================

                              A M S S   H E A P

DESCRIPTION
  Wrapper for malloc, realloc, calloc and free calls for AMSS_HEAP.

Copyright (c) 2006-2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/entry/entry/malloc.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/16/14   mg     File created
============================================================================ */


/*============================================================================
                             INCLUDE FILES
============================================================================*/
#include <stdlib.h>
#include <string.h>
#include "com_dtypes.h"
#include "malloc.h"
//#include "memheap_lite.h"

/*============================================================================
                             CONSTANT DEFINITIONS
============================================================================*/

/*============================================================================
                             DATA DECLARATIONS
=============================================================================*/

/*============================================================================
                             GLOBAL VARIABLES
============================================================================*/

/*============================================================================
                             LOCAL VARIABLES
============================================================================*/


/*============================================================================
                             MACRO DEFINITIONS
============================================================================*/


/*============================================================================
                             FORWARD DECLARATIONS
============================================================================*/
#if 0
static mem_heap_type amss_mem_heap;
static boolean is_amss_heap_initialized = FALSE;
#endif

/*===========================================================================

FUNCTION MALLOC_INIT

DESCRIPTION
  Initializes function pointers for malloc and free functionality

DEPENDENCIES
  None

RETURN VALUE
 boolean - TRUE if malloc and free function pointers were
           correctly initialized. 
           FALSE if malloc and free function pointers are NULL

SIDE EFFECTS
  Will be called on the first malloc call

ARGUMENTS
  None

===========================================================================*/
boolean malloc_init
(
  heap_config_type* heap_cfg
)
{
  return TRUE;
}

#if 0
/*===========================================================================

FUNCTION MALLOC_INIT

DESCRIPTION
  Initializes function pointers for malloc and free functionality

DEPENDENCIES
  None

RETURN VALUE
 boolean - TRUE if malloc and free function pointers were
           correctly initialized. 
           FALSE if malloc and free function pointers are NULL

SIDE EFFECTS
  Will be called on the first malloc call

ARGUMENTS
  None

===========================================================================*/
boolean malloc_deinit
(
  void
)
{
   mem_deinit_heap(&amss_mem_heap);
   is_amss_heap_initialized = FALSE;
  return TRUE;
}

/*===========================================================================

FUNCTION malloc

DESCRIPTION
  Allocates from the either RTOS heap in FOM mode and internal heap in 
  SOM mode.

DEPENDENCIES
  None

RETURN VALUE
  A a pointer to the newly allocated block, or NULL if the block
  could not be allocated.

SIDE EFFECTS
  Replaces C Std Library implementation.

ARGUMENTS
  size  - Size of allocation request.

===========================================================================*/
void *malloc
(
  size_t size
)
{

   if(!is_amss_heap_initialized)
  {
    return NULL;       
  }
   return mem_malloc(&amss_mem_heap,size);
}

/*===========================================================================

FUNCTION free

DESCRIPTION
  Deallocates a block of memory and returns it to the heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Replaces C Std Library implementation.

ARGUMENTS
  ptr - A pointer to the memory block that needs to be deallocated.

===========================================================================*/
void free
(
  void *ptr
)
{
   mem_free(&amss_mem_heap,ptr);
   return;
}

/*===========================================================================
FUNCTION MEM_CALLOC

DESCRIPTION
  Allocates enough space for elt_count elements each of elt_size bytes
  from the heap and initializes the space to nul bytes.  If
  heap_ptr is NULL or elt_count or elt_size is 0, the NULL pointer will
  be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
/*lint -sem(mem_calloc,1p,2n>=0&&3n>=0&&(@p==0||@P==2n*3n)) */
void* calloc(size_t count, size_t size)
{
  size_t total_length;
  void *answer;
  total_length = count * size;

  answer = mem_malloc(&amss_mem_heap, total_length);
  if( answer )
  {
    /* calloc always returns cleared memory blocks  */
    memset(answer, 0, total_length);
  }
  return answer;
} /* END mem_calloc */
#endif


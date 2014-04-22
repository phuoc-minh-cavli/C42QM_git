/*===========================================================================

                       D S _ S Y S T E M _ H E A P . C

DESCRIPTION
  The Data Services Protocol Stack system heap wrapper.

EXTERNALIZED FUNCTIONS

  ps_system_heap_mem_alloc()
    Allocates requested amount of memory from the system heap.

  DS_SYSTEM_HEAP_MEM_FREE()
    Frees the memory allocated through ps_system_heap_mem_alloc().

INTIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2018 -2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/utils/src/ds_system_heap.c#1 $
  $DateTime: 2020/03/17 08:03:16 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/23/10    mkumarsa     Created module
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ds_system_heap.h"
#include "data_msg.h"
#include "amssassert.h"


#ifdef TEST_FRAMEWORK
#error code not present
#endif

#ifdef FEATURE_DATA_PS_SYSTEM_HEAP

/* Include the modem memory manager API */
#include "modem_mem.h"

#else

/*! 
 * Defines the PS system heap used in the absense of modem heap.
 *
 * The value of this constant is currently configured for AMSS environments.
 * For other environments, we have a limited set of VUs and functionality
 * exported and this needs to be tuned accordingly.
 */

#include "memheap.h"
 
#define DS_SYSTEM_HEAP_SIZE (500 * 1024)

static mem_heap_type ds_system_heap;
static uint8 ds_system_heap_buffer[DS_SYSTEM_HEAP_SIZE];

#endif 

#ifdef TEST_FRAMEWORK
 #error code not present
#elif defined(FEATURE_QDSP6)
 #define  CALLER_ADDRESS(level) ((void *)__builtin_return_address(level))
#else
 #define  CALLER_ADDRESS(level) ((void *) __return_address())
#endif

/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
   
/*===========================================================================
FUNCTION  ds_system_heap_mem_alloc

DESCRIPTION
  This function is a wrapper on top of the system's malloc function.

PARAMETERS
  num_bytes - Size (in bytes) of the memory to be allocated.

RETURN VALUE
  Pointer to memory block if successful.
  NULL if memory could not be allocated.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void* ds_system_heap_mem_alloc
(
  unsigned long num_bytes,
  const char      *file_name,
  unsigned int     line_num
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

void *ptr = NULL;
#ifdef TEST_FRAMEWORK
  #error code not present
#endif

#ifdef FEATURE_DATA_PS_SYSTEM_HEAP

  ptr = modem_mem_alloc(num_bytes, MODEM_MEM_CLIENT_DATA);
#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */
  return ptr;
#else

  ptr = mem_malloc( &ps_system_heap, (size_t) num_bytes );
#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */
  return ptr;

#endif
} /* ds_system_heap_mem_alloc() */


/*==========================================================================
FUNCTION  DS_SYSTEM_HEAP_VALIDATE_MEM_ALLOC

DESCRIPTION
  This function is to validate the memory allocated by the malloc function
  and prints caller address for tracking

PARAMETERS
  buf_ptr  - pointer to store the address of the memory allocated
  size - Size (in bytes) of the memory to be allocated.
  is_graceful - boolean specifies to handle gracefully or not

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_system_heap_validate_mem_alloc
(
  void *         buf_ptr,
  unsigned long  size,
  boolean        is_graceful
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(buf_ptr == NULL)
  {
    DS_MSG_SPRINTF_2(MSG_LEGACY_ERROR,"failed to allocate memory in caller_func_ptr: 0x%p for size (%d)",
                     CALLER_ADDRESS(0), size);
    DS_ASSERT(is_graceful);
  }
  else
  {
    memset(buf_ptr, 0, size);
  }
}

/*===========================================================================
FUNCTION  ds_system_heapi_mem_free

DESCRIPTION
  Frees the memory allocated by ps_system_heap_mem_alloc(). Internal method, 
  Use only exported free method: DS_SYSTEM_HEAP_MEM_FREE

PARAMTERS
  mem_ptr   - Memory to free.

RETURN VALUE
  None.

DEPENDENCIES
  The memory chunk passed to be freed must have been allocated by
  ps_system_heap_mem_alloc().

SIDE EFFECTS
  Sets the passed in pointer to memory to NULL.
===========================================================================*/
void ds_system_heapi_mem_free
(
  void *mem_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == mem_ptr)
  {
    return;
  }

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

#ifdef FEATURE_DATA_PS_SYSTEM_HEAP

  modem_mem_free (mem_ptr, MODEM_MEM_CLIENT_DATA);

#else

  mem_free (&ps_system_heap, mem_ptr);

#endif
  
  return;

} /* ds_system_heapi_mem_free() */

/*===========================================================================
FUNCTION  ds_system_heap_init

DESCRIPTION
  Performs system heap initialization

PARAMTERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_system_heap_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_DATA_PS_SYSTEM_HEAP
  /*------------------------------------------------------------------------
    Initialize the PS system heap module.
  -------------------------------------------------------------------------*/
  mem_init_heap (&ds_system_heap,
                 ds_system_heap_buffer,
                 DS_SYSTEM_HEAP_SIZE,
                 NULL);
#endif

} /* ds_system_heap_mem_init */

/*===========================================================================
FUNCTION  ds_system_heap_deinit

DESCRIPTION
  Performs system heap cleanup

PARAMTERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_system_heap_deinit
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* ds_system_heap_deinit */

/*===========================================================================
FUNCTION  ds_system_heap_simulate_out_of_mem

DESCRIPTION
  Simulates out of memory scenario for test purposes. Only to be used in 
  test scenarios, not in production builds.

PARAMTERS
  out_of_mem  - If TRUE  - then simulate out of memory.
                If FALSE - then reset simulate out of memory flag.

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_system_heap_simulate_out_of_mem
(
  boolean out_of_mem
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef TEST_FRAMEWORK
#error code not present
#else

  DS_ERROR_FATAL("ds_system_heap_deinit(): Not supported!");

#endif /* TEST_FRAMEWORK */

} /* ps_system_heap_simulate_out_of_mem */


/*===========================================================================
FUNCTION  ds_system_heap_get_alloc_buf_count

DESCRIPTION
  Returns the number of allocated buffers by the PS_SYSTEM_HEAP. Used on
  test environments to ensure no memory leak.

PARAMTERS
  None.

RETURN VALUE
  Count of currently allocated buffers from ps_system_heap.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 ds_system_heap_get_alloc_buf_count
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef TEST_FRAMEWORK
#error code not present
#else

  DS_ERROR_FATAL("ds_system_heap_get_alloc_buf_count(): Not supported!");
  return 0;

#endif /* TEST_FRAMEWORK */

} /* ds_system_heap_get_alloc_buf_count */



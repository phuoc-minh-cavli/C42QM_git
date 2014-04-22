#ifndef DS_SYSTEM_HEAP_H
#define DS_SYSTEM_HEAP_H
/*===========================================================================

                       D S _ S Y S T E M _ H E A P . H

DESCRIPTION
  The Data Services Protocol Stack system heap wrapper.

EXTERNALIZED FUNCTIONS

  ds_system_heap_mem_alloc()
    Allocates requested amount of memory from the system heap.

  DS_SYSTEM_HEAP_MEM_FREE()
    Frees the memory allocated through ps_system_heap_mem_alloc().

INTIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2018 - 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/utils/inc/ds_system_heap.h#1 $
  $DateTime: 2007/05/18 18:56:57

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/24/10    ua     Fixed compilation errors. 
02/26/09    sp     IPsec CMI Decoupling - Fixed linking errors.
02/09/09    hm     Created initial module.
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "modem_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
MACRO DS_SYSTEM_HEAP_MEM_ALLOC / DS_SYSTEM_HEAP_MEM_ALLOC_ASSERT

DESCRIPTION
  This macro will internally call modem_mem_alloc/ds_system_heap_alloc based
  on featurization. It additionally validates the buffer allocation, prints debug 
  info and conditionally ASSERTs.
  
PARAMETERS
  buf_ptr  - pointer to store the address of the memory allocated
  size - Size (in bytes) of the memory to be allocated.
  typecast_type - data type to which the pointer needs to be typecasted

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.  
===========================================================================*/

#ifdef TEST_FRAMEWORK
#error code not present
#else
#define DS_SYSTEM_HEAP_MEM_ALLOC(buf_ptr,size,typecast_type) \
  buf_ptr = (typecast_type)modem_mem_alloc(size, MODEM_MEM_CLIENT_DATA); \
  ds_system_heap_validate_mem_alloc(buf_ptr, size, TRUE);



#define DS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(buf_ptr,size,typecast_type) \
  buf_ptr = (typecast_type)modem_mem_alloc(size, MODEM_MEM_CLIENT_DATA); \
  ds_system_heap_validate_mem_alloc(buf_ptr, size, FALSE)

#endif


/*===========================================================================
MACRO DS_SYSTEM_HEAP_MEM_ALLOC_CRIT / DS_SYSTEM_HEAP_MEM_ALLOC_ASSERT_CRIT

DESCRIPTION
  This macro will internally call modem_mem_alloc/ds_system_heap_alloc based
  on featurization. It additionally validates the buffer allocation, prints debug 
  info and conditionally ASSERTs.
  
PARAMETERS
  buf_ptr  - pointer to store the address of the memory allocated
  size - Size (in bytes) of the memory to be allocated.
  typecast_type - data type to which the pointer needs to be typecasted

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.  
===========================================================================*/

#define DS_SYSTEM_HEAP_MEM_ALLOC_CRIT(buf_ptr,size,typecast_type) \
  buf_ptr = (typecast_type)modem_mem_alloc(size, MODEM_MEM_CLIENT_DATA_CRIT);

#define DS_SYSTEM_HEAP_MEM_ALLOC_ASSERT_CRIT(buf_ptr,size,typecast_type) \
  buf_ptr = (typecast_type)modem_mem_alloc(size, MODEM_MEM_CLIENT_DATA_CRIT); \
  ds_system_heap_validate_mem_alloc(buf_ptr, size, FALSE)

/*===========================================================================
FUNCTION  DS_SYSTEM_HEAP_VALIDATE_MEM_ALLOC

DESCRIPTION
   This function is a wrapper on top of the system's malloc function and 
   prints caller address for tracking

PARAMETERS
  buf_ptr  - pointer to the memory allocated
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
   void * buf_ptr,
   unsigned long size,
  boolean     is_graceful
);

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
);

/*===========================================================================
MACRO DS_SYSTEM_HEAP_MEM_FREE

DESCRIPTION
  This macro frees the memory allocated by ps_system_heap_mem_alloc(). It 
  also sets the pointer variable to NULL. 

PARAMTERS
  mem_ptr - Memory to free.

RETURN VALUE
  None.

DEPENDENCIES
  The memory chunk passed to be freed must have been allocated by
  ds_system_heap_mem_alloc().

SIDE EFFECTS
  Sets the passed in pointer to memory to NULL.
===========================================================================*/
#define DS_SYSTEM_HEAP_MEM_FREE(mem_ptr)                                    \
  do                                                                        \
  {                                                                         \
    ds_system_heapi_mem_free((void *)mem_ptr);                              \
    mem_ptr = NULL;                                                         \
  } while (0)


/*===========================================================================
FUNCTION  ds_system_heapi_mem_free

DESCRIPTION
  Internal helper routine for DS_SYSTEM_HEAP_MEM_FREE 

PARAMTERS
  mem_ptr - Memory to free.

RETURN VALUE
  None.

DEPENDENCIES
  The memory chunk passed to be freed must have been allocated by
  ds_system_heap_mem_alloc().

SIDE EFFECTS
  None.
===========================================================================*/
void ds_system_heapi_mem_free
(
  void *mem_ptr
);

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
);

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
);

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
);


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
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#ifdef __cplusplus
}
#endif

#endif /* DS_SYSTEM_HEAP_H */


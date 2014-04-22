/*===========================================================================

                    S H A R E D   M E M O R Y   Q U R T

DESCRIPTION
    Defines the OS and API specific helper functions for SMEM API.  Qurt is 
    fully supported by all ModemSW and ADSP, but this file requires the build 
    to have an xml config that sets the properties of "smem_pool".

Copyright (c) 2012-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/18/13   bt      Rename IMEM defines to more-generic target info.
05/14/13   bt      Use DAL to map SMEM region.
12/03/12   bt      Initial revision for Qurt-specific smem_map_memory_va, 
                   and retrieval of SMEM physical address and size.
===========================================================================*/
/*===========================================================================
                  INCLUDE FILES
===========================================================================*/
#include <stdint.h>
#include <stringl.h>
#include "qurt.h"
#include "qurt_memory.h"
#include "smem_os_common.h"
#include "smem_v.h"
#include "smem_target.h"
#include "smem_targ_info.h"

/*===========================================================================
                  LOCAL DATA DECLARATIONS
===========================================================================*/

/*===========================================================================
                  PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smem_os_ext_get_smem_base
===========================================================================*/
/**
  Returns the shared memory base physical address.

  @return
  Shared memory base physical address
*/
/*=========================================================================*/
void *smem_os_ext_get_smem_base( void )
{
  smem_targ_info_type  *smem_targ_info_entry;

  /* Try to get SMEM base/size from smem target info struct */
  smem_targ_info_entry = smem_map_smem_targ_info();
  if (smem_targ_info_entry != NULL && smem_targ_info_entry != SMEM_INVALID_ADDR)
  {
    if(smem_targ_info_entry->identifier == SMEM_TARG_INFO_IDENTIFIER)
    {
      /* Cast uint64 to a pointer. */
      return (void *)(uintptr_t)(smem_targ_info_entry->smem_base_phys_addr);
    }
  }

  ERR_FATAL("smem_os_ext_get_smem_base: Cannot determine SMEM physical "
            "address!", 0, 0, 0);
  return SMEM_INVALID_ADDR;
} /* smem_ext_get_smem_base */

/*===========================================================================
  FUNCTION  smem_os_ext_get_smem_size
===========================================================================*/
/**
  Returns the shared memory size.

  @return
  Shared memory size
*/
/*=========================================================================*/
uint32 smem_os_ext_get_smem_size( void )
{
  smem_targ_info_type *smem_targ_info_entry; 

  /* Try to get SMEM size from smem target info struct. */
  smem_targ_info_entry = smem_map_smem_targ_info();
  if (smem_targ_info_entry != NULL && smem_targ_info_entry != SMEM_INVALID_ADDR)
  {
    if(smem_targ_info_entry->identifier == SMEM_TARG_INFO_IDENTIFIER)
    {
      return smem_targ_info_entry->smem_size;
    }
  }

  ERR_FATAL("smem_os_ext_get_smem_size: Cannot determine SMEM size!", 0, 0, 0);
  return 0;
} /* smem_ext_get_smem_size */

/*===========================================================================
  FUNCTION  smem_mem_copy
===========================================================================*/
/**
  Copies bytes from the source buffer to the destination buffer.

  This function ensures that there will not be a copy beyond the size of 
  the destination buffer.

  The result of calling this on overlapping source and destination
  buffers is undefined.
  
  @param[out] dst       Destination buffer.
  @param[in]  dst_size  Size of the destination buffer in bytes.
  @param[in]  src       Source buffer.
  @param[in]  copy_size Number of bytes to copy from source buffer.

  @return
  The number of bytes copied to the destination buffer.  It is the
  caller's responsibility to check for trunction if it cares about it -
  truncation has occurred if the return value is less than src_size.
*/
/*==========================================================================*/
uint32 smem_mem_copy
(
  void       *dst, 
  uint32     dst_size, 
  const void *src, 
  uint32     copy_size
)
{
  uint32 ret_size;

  ret_size = memscpy(dst, dst_size, src, copy_size);
  return ret_size;
}

/*===========================================================================
  FUNCTION  smem_mem_move
===========================================================================*/
/**
  Moves bytes from the source buffer to the destination buffer.

  This function ensures that there will not be a copy beyond 
  the size of the destination buffer. 

  This function should be used in preference to memscpy() if there
  is the possiblity of source and destination buffers overlapping.
  The result of the operation is defined to be as if the copy were from
  the source to a temporary buffer that overlaps neither source nor
  destination, followed by a copy from that temporary buffer to the
  destination.
   
  @param[out] dst       Destination buffer.
  @param[in]  dst_size  Size of the destination buffer in bytes.
  @param[in]  src       Source buffer.
  @param[in]  copy_size Number of bytes to copy from source buffer.
   
  @return 
  The number of bytes copied to the destination buffer.  It is the
  caller's responsibility to check for trunction if it cares about it -
  truncation has occurred if the return value is less than src_size.
*/
/*==========================================================================*/
uint32 smem_mem_move
(
  void       *dst, 
  uint32     dst_size, 
  const void *src, 
  uint32     copy_size
)
{
  uint32 ret_size;

  ret_size = memsmove(dst, dst_size, src, copy_size);
  return ret_size;
}

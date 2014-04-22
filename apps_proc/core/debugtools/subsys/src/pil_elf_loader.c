/*=============================================================================

        P E R I P H E R A L   I M A G E   L O A D E R    M O D U L E
              E L F   D A T A   L O A D I N G   M O D U L E 

GENERAL DESCRIPTION
  Implements various Elf loading/parsing apis for PIL Framework

REGIONAL FUNCTIONS 
  None
  
EXTERNALIZED FUNCTIONS

  pil_elf_loader_init(partition name, elf handle)
    Opens requested partition in Flash, prepares elf handle structure,
    reads elf header, program tables

  pil_elf_loader_deinit(elf handle)
    Closes requested partition and cleans up, frees elf handle structure
    
  pil_elf_read_hash_seg(elf handle)
    Reads Hash segment data from Flash to given buffer
    
  pil_elf_load_nth_segment(elf handle)
    Loads nth segment to the physical address indicated in the segment
    
  pil_elf_load_loadable_segments(elf handle)
    Loads all segments to their corresponding Physical addresses
    
  pil_set_reloadable_offset(elf handle, offset)
    Sets the Relocatable offset to given offset. Once this is set, the 
    segments will get loaded at offset from physical address
    
  pil_get_read_elf_size(elf handle)
    Sets the Relocatable offset to given offset. Once this is set, the 
    segments will get loaded at offset from physical address
    
  pil_elf_get_edhr_entry(elf handle)
    Returns entry address from elf header. If Reloc offset is set, will 
    add that much offset to entry address and returns
    
  pil_elf_get_segments_count(elf handle)
    Returns number of segments in given elf partition
    
  pil_elf_get_segment_size(elf handle, index)
    Returns size of given segment
    
  pil_reconstruct_mba_elf_image(elf handle, buffer, size)
    Reconstructs Elf data by reading header, segments etc
    
  pil_elf_populate_meta_info(elf handle, buffer, size)
    Reads Metadata and populates to given buffer
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    pil_elf_loader_init() should be called before doing any other Elf operations

Copyright (c) 2016-17,2019-2020 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil_elf_loader.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

07/28/16   ab      File created.
05/04/17   ab      Added Support for PIL Hibernation

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <stringl/stringl.h>
#include "msg.h"
#include "pil_utils.h"
#ifdef PIL_DALSYS_AVAILABLE
#include "DALSys.h"
#else
#include "qurt.h"
#endif
#include "pil.h"
#include "pil_elf_loader.h"
#include "timer.h"
#include "pil_scm.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
/* Max Data Chunk size to be 1MB */
#define MAX_CHUNK_SIZE 0x100000

#define MAX_ADDRESS_32 0xFFFFFFFF
#define MAX_ADDRESS 0xFFFFFFFFFFFFFFFF

uint64 modem_start_va = 0; /* Global variable which stores the virtual start address of modem */

extern uint64 modem_start_pa;

#ifdef PIL_TZ_FILTER
  extern uint64 modem_image_size;
  #ifdef PIL_DALSYS_AVAILABLE
    DALSYSMemHandle MemObj_global = 0;
  #else
    qurt_mem_region_t MemObj_global = 0;
  #endif /* PIL_DALSYS_AVAILABLE */
#endif /* PIL_TZ_FILTER */

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================
FUNCTION 
  pil_elf_is_auth_enabled

DESCRIPTION
  This function returns whether Authentication is enabled or not.
  Currently it always returns False.

DEPENDENCIES
  None.

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
=============================================================================*/
pil_error_type pil_elf_is_auth_enabled(boolean *is_auth_enabled)
{
  if(is_auth_enabled == NULL)
    return PIL_INVALID_PARAMETERS;
  
  *is_auth_enabled = FALSE;
  return PIL_SUCCESS;
} /* pil_elf_is_auth_enabled */

/*=============================================================================
FUNCTION 
  pil_elf_phdr_is_loadable

DESCRIPTION
  This function checks if a specific segment is a loadable segment or not

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle
  Program header pointer

RETURN VALUE
  TRUE if loadable
  FALSE if not loadable
=============================================================================*/
static boolean pil_elf_phdr_is_loadable(pil_elf_struct_type *handle, void * phdr)
{
  uint32 phdr_type;
  uint32 phdr_flags;
  
  PIL_VERIFY( phdr != NULL, "NULL Phdr passed", 0, 0, 0 );
  
  phdr_type = (handle->elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_type : ((Elf32_Phdr *)phdr)->p_type;
  phdr_flags = (handle->elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_flags : ((Elf32_Phdr *)phdr)->p_flags;

  return (  phdr_type == MI_PBT_ELF_PT_LOAD  &&
            MI_PBT_PAGE_MODE_VALUE(phdr_flags) == MI_PBT_NON_PAGED_SEGMENT  &&
            MI_PBT_ACCESS_TYPE_VALUE(phdr_flags) != MI_PBT_NOTUSED_SEGMENT  &&
           /* hash segment to be loaded explicitly to fixed buffer */
            MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) != MI_PBT_HASH_SEGMENT   &&
            MI_PBT_ACCESS_TYPE_VALUE(phdr_flags) != MI_PBT_SHARED_SEGMENT  );
} /* pil_elf_phdr_is_loadable */


/*===========================================================================
 **  Function :  pil_elf_phdr_is_loadable_in_hibernation
 ** ==========================================================================
 */
/*!
 * 
 * @brief
 *   This function checks if a specific program header should be loaded
 *   into memory or not in hibernation mode
 *
 * @param[in] phdr - Pointer to the Program Header structure 
 *
 * @par Dependencies
 *   None 
 * 
 * @retval
 *   TRUE - If Program Header should be loaded
 *   FALSE - Otherwise
 * 
 */
static boolean pil_elf_phdr_is_loadable_in_hibernation(pil_elf_struct_type *handle, void * phdr)
{
  uint32 phdr_flags;

  PIL_VERIFY( phdr != NULL, "NULL Phdr passed", handle, 0, 0 );

  phdr_flags = (handle->elf_format == ELF_CLASS_64) ? ((Elf64_Phdr *)phdr)->p_flags : ((Elf32_Phdr *)phdr)->p_flags;

  if (handle->hiber_boot == TRUE)
    return((MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) == MI_PBT_HASH_SEGMENT) ||
            ((MI_PBT_FLAG_PERM_VALUE(phdr_flags) != MI_PBT_PERM_RX)         &&
         (MI_PBT_FLAG_PERM_VALUE(phdr_flags) != MI_PBT_PERM_R)          &&
         (MI_PBT_FLAG_PERM_VALUE(phdr_flags) != MI_PBT_PERM_X)          &&
         (MI_PBT_FLAG_PERM_VALUE(phdr_flags) != MI_PBT_PERM_NONE)));
  else
    return TRUE;
} /* pil_elf_phdr_is_loadable_in_hibernation */


/*=============================================================================
FUNCTION 
  pil_find_first_loadable_segment

DESCRIPTION
  This function returns segment pointer to first loadable segment

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle

RETURN VALUE
  First Program segment pointer
=============================================================================*/
void * pil_find_first_loadable_segment(pil_elf_struct_type *handle)
{
  int i;
  
  if(handle == NULL)
  {
    PIL_LOG_ERR(1,"handle is NULL", 0);
    return NULL;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return NULL;
  }
  
  if(handle->elf_format == ELF_CLASS_64)
  {
    for(i=0; i<handle->phdr_num; i++)
    {
      if(pil_elf_phdr_is_loadable(handle, (void *)&handle->elf.e64.phdr_array[i]) == TRUE)
        return (void *)&handle->elf.e64.phdr_array[i];
    }
  }
  else
  {
    for(i=0; i<handle->phdr_num; i++)
    {
      if(pil_elf_phdr_is_loadable(handle, (void *)&handle->elf.e32.phdr_array[i]) == TRUE)
        return (void *)&handle->elf.e32.phdr_array[i];
    }    
  }
  
  return NULL;
} /* pil_find_first_loadable_segment */


/*=============================================================================
FUNCTION 
  pil_set_reloadable_offset

DESCRIPTION
  This function sets given offset as relocatable offset for that elf segments

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle - Handle to Initialized Elf Structure
  Offset - Offset value that should be used to offset a segment address

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
=============================================================================*/
pil_error_type pil_set_reloadable_offset(pil_elf_struct_type *handle, int offset)
{
  if(handle == NULL)
  {
    PIL_LOG_ERR(1,"handle is NULL", 0);
    return PIL_INVALID_PARAMETERS;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected magic: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return PIL_ELF_HANDLE_NOT_INITED;
  }
  
  handle->reloc_offset = offset;
  
  return PIL_SUCCESS;
} /* pil_set_reloadable_offset */


/*=============================================================================
FUNCTION 
  pil_get_read_elf_size

DESCRIPTION
  This function returns size of elf data read till that point.
  Note that this will not give full elf size but only data read till that point
  This can be used to know how much size of data is read between two events

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle - Handle to Initialized Elf Structure

RETURN VALUE
  Size of elf data read till a given time
=============================================================================*/
uint32 pil_get_read_elf_size(pil_elf_struct_type *handle)
{
  if(handle == NULL)
  {
    PIL_LOG_ERR(1,"handle is NULL", 0);
    return 0;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected magic: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return 0;
  }
  
  return handle->total_elf_segments_size;
} /* pil_get_read_elf_size */


/*=============================================================================
FUNCTION 
  pil_elf_get_edhr_entry

DESCRIPTION
  This function returns size of elf data read till that point.
  Note that this will not give full elf size but only data read till that point
  This can be used to know how much size of data is read between two events

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle - Handle to Initialized Elf Structure

RETURN VALUE
  Size of elf data read till a given time
=============================================================================*/
unsigned int pil_elf_get_edhr_entry(pil_elf_struct_type *handle)
{
  unsigned int ehdr_entry;
  
  if(handle == NULL)
  {
    PIL_LOG_ERR(1,"pil_elf_get_edhr_entry: handle is NULL", 0);
    return 0;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected magic: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return 0;
  }
  
  ehdr_entry = (handle->elf_format == ELF_CLASS_64) ? handle->elf.e64.ehdr.e_entry : handle->elf.e32.ehdr.e_entry;
  
  ehdr_entry += handle->reloc_offset;
  
  return ehdr_entry;
} /* pil_elf_get_edhr_entry */


/*=============================================================================
FUNCTION 
  pil_elf_get_segments_count

DESCRIPTION
  Returns Segments Count in given Elf partition

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle[in] - Handle to Initialized Elf Structure
  Count[out] - Fills count variable with number of segments

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
  PIL_ELF_HANDLE_NOT_INITED
=============================================================================*/
pil_error_type pil_elf_get_segments_count(pil_elf_struct_type *handle, uint32 *count)
{
  if(count == NULL || handle == NULL)
  {
    PIL_LOG_ERR(1,"pil_elf_get_segments_count: elf handle or count ptr is NULL", 0);
    return PIL_INVALID_PARAMETERS;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected magic: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return PIL_ELF_HANDLE_NOT_INITED;
  }
    
  *count = handle->phdr_num;
  return PIL_SUCCESS;
} /* pil_elf_get_segments_count */


/*=============================================================================
FUNCTION 
  pil_elf_get_edhr_entry

DESCRIPTION
  This function returns size of given segment

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle[in] - Handle to Initialized Elf Structure
  segment index[in] - Index of Segment whose size is required
  size[out] - Pointer to size variable in which size variable will be filled

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
  PIL_ELF_HANDLE_NOT_INITED
=============================================================================*/
pil_error_type pil_elf_get_segment_size(pil_elf_struct_type *handle, uint32 seg_idx, uint64 *size)
{
  if(size == NULL || handle == NULL)
  {
    PIL_LOG_ERR(1,"elf handle or size ptr is NULL", 0);
    return PIL_INVALID_PARAMETERS;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected magic: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return PIL_ELF_HANDLE_NOT_INITED;
  }
  
  if(seg_idx >= handle->phdr_num)
  {
    PIL_LOG_ERR(3,"segment index(%d) is beyond max segments(%d)", handle, seg_idx, handle->phdr_num);
    return PIL_ELF_INVALID_SEGMENT_INDEX;
  }
  
  if(handle->elf_format == ELF_CLASS_64)
    *size = handle->elf.e64.phdr_array[seg_idx].p_memsz;
  else
    *size = handle->elf.e32.phdr_array[seg_idx].p_memsz;
  
  return PIL_SUCCESS;
} /* pil_elf_get_segment_size */


/*=============================================================================
FUNCTION 
  pil_elf_set_hiber_boot

DESCRIPTION
  This function enables/disables boot from hibernation. This function can be 
  used to indicate PIL ELF loader to not load RO and X segments if previous
  shutdown is done with hibernation option. 

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle[in] - Handle to Initialized Elf Structure
  enable[out] - If set TRUE, then RO segments will not be loaded further

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
  PIL_ELF_HANDLE_NOT_INITED
=============================================================================*/
pil_error_type pil_elf_set_hiber_boot(pil_elf_struct_type *handle, boolean enable)
{
  if(handle == NULL)
  {
    PIL_LOG_ERR(1,"elf handle is NULL", 0);
    return PIL_INVALID_PARAMETERS;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected magic: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return PIL_ELF_HANDLE_NOT_INITED;
  }
  
  handle->hiber_boot = enable;
  
  return PIL_SUCCESS;
} /* pil_elf_set_hiber_boot */

/*=============================================================================
FUNCTION 
  pil_elf_load_seg_mba

DESCRIPTION
  This function loads the data segment (loadable and non-loadable if force is TRUE) 
  into memory using the information from a specific program header. This function is
  specifically for the case where authentication is done by mba.
  
Note1: If dest_addr is non-Null, segment data will be copied at address in dest_addr.
          dest_addr should be Virtual address
Note2: If force is TRUE, this tries to copy data of a non-loadable segment as well
Note3: If reloc_offset is set and dest_addr is NULL, will add reloc_offset to 
          phys address in program segment and loads segment data at that location

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle - Handle to Initialized Elf Structure
  phdr - Pointer to the Program Header structure 
  dest_addr - If the dest_addr is non-NULL, will load segment data at that address
              rather than using address in Phdr. If Null, will loads Segment data
              at Physical address indicated in given Program header structure
  force - If TRUE, will load non-loadable segment as well.

RETURN VALUE
  PIL_SUCCESS
  PIL_NON_LOADABLE_SEGMENT
  PIL_FLASH_READ_FAILED
  Other errors
=============================================================================*/
#ifdef PIL_MBA_FILTER
static pil_error_type pil_elf_load_seg_mba(pil_elf_struct_type *handle, void *phdr, void *dest_addr, boolean force)
{
  uint32 chunk_size = MAX_CHUNK_SIZE;
  uint64 remaining_data = 0, chunk_dest_addr =0, chunk_offset =0;
  uint64 phdr_entry_offset, phdr_entry_paddr, phdr_entry_filesz, phdr_entry_memsz; 
  pil_error_type status = PIL_FAILURE;
  uint64 phdr_entry_paddr_temp;

  #ifdef PIL_DALSYS_AVAILABLE
    DALSYSMemHandle MemObj = 0;
  #else
    qurt_mem_region_t MemObj = 0;
  #endif

  PIL_VERIFY( phdr != NULL, "phdr is NULL", 0, 0, 0);
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected magic: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return PIL_ELF_HANDLE_NOT_INITED;
  }

  /* If booting from Hibernation, check if this segment needs to be loaded or not */
  if(pil_elf_phdr_is_loadable_in_hibernation(handle, phdr))
  {
    if (handle->elf_format == ELF_CLASS_64)
    {
      phdr_entry_offset = ((Elf64_Phdr *)phdr)->p_offset;
      if(dest_addr == NULL)
      {
        phdr_entry_paddr = ((Elf64_Phdr *)phdr)->p_paddr + handle->reloc_offset;
      }
      else
      {
        phdr_entry_paddr = (unsigned int)dest_addr;
      }
      phdr_entry_filesz = ((Elf64_Phdr *)phdr)->p_filesz;
      phdr_entry_memsz = ((Elf64_Phdr *)phdr)->p_memsz;
    }
    else
    {
      phdr_entry_offset = ((Elf32_Phdr *)phdr)->p_offset;
      if(dest_addr == NULL)
      {
        phdr_entry_paddr = ((Elf32_Phdr *)phdr)->p_paddr + handle->reloc_offset;
      }
      else
      {
        phdr_entry_paddr = (unsigned int)dest_addr;
      }
      phdr_entry_filesz = ((Elf32_Phdr *)phdr)->p_filesz;
      phdr_entry_memsz = ((Elf32_Phdr *)phdr)->p_memsz;
    }
  
    if (pil_elf_phdr_is_loadable(handle, phdr) || force == TRUE)
    {
      /* Only process segments with valid parameters */
      /* 0x0 is a valid address. Hence no need to validate dest_addr for null pointer 
      */
	  
	  
	  // This is for reading from partition in flash
      if (phdr_entry_filesz)
      {
        remaining_data = phdr_entry_filesz;
        phdr_entry_paddr_temp = phdr_entry_paddr;

        chunk_offset = phdr_entry_offset;
        while(remaining_data !=0)
        {
          if(remaining_data < chunk_size )
          {
            chunk_size = remaining_data;
          }
          
          /* If dest_addr is NULL, need to get virt addr for phys addr is phdr */
          if(dest_addr == NULL)
          {
            status = pil_map_and_get_virt_addr(phdr_entry_paddr_temp, chunk_size, &chunk_dest_addr, &MemObj);
            if(status != PIL_SUCCESS)
            {
              PIL_LOG_ERR(3,"pil_map_and_get_virt_addr failed with status %d for phys addr 0x%x, size %d", status, (uint32)phdr_entry_paddr_temp, (uint32)chunk_size);
              return status;
            }
          }
          else
            chunk_dest_addr = phdr_entry_paddr_temp;
          /* Copy the data segment flash into memory */
          status = pil_flash_read_partition( handle,
                                           (void *)(unsigned long)chunk_dest_addr,
                                           chunk_offset,
                                           chunk_size );
          if(status != PIL_SUCCESS)
          {
            if(dest_addr == NULL)
              pil_unmap_virt_addr(&MemObj);
            
            PIL_LOG_ERR(3,"Couldnt read flash for segment loading", status, (uint32)chunk_offset, chunk_size);
            return PIL_FLASH_READ_FAILED;
          }

          if(dest_addr == NULL)
          {
            status = pil_unmap_virt_addr(&MemObj);
            if( status != PIL_SUCCESS)
            {
              PIL_LOG_ERR(1,"pil_unmap_virt_addr failed with err %d", status);
              return status;
            }
          }
            
          phdr_entry_paddr_temp += chunk_size;

          chunk_offset += chunk_size;
          remaining_data = remaining_data - chunk_size;
          
          /* Update size indicating how much elf is loaded */
          PIL_VERIFY((handle->total_elf_segments_size + chunk_size > handle->total_elf_segments_size), "total_elf_segments_size may get overflowed",
                    handle->total_elf_segments_size, chunk_size, 0); 
          handle->total_elf_segments_size = handle->total_elf_segments_size + chunk_size;
        }
      }
    
      /* If uninitialized data exists, make sure to zero-init */
      if (phdr_entry_memsz > phdr_entry_filesz)
      {
        remaining_data = phdr_entry_memsz - phdr_entry_filesz;
        phdr_entry_paddr_temp = phdr_entry_paddr+phdr_entry_filesz;

        chunk_size = MAX_CHUNK_SIZE;
        
        while(remaining_data !=0)
        {
          if(remaining_data < chunk_size )
          {
            chunk_size = remaining_data;
          }
          
          /* If dest_addr is NULL, need to get virt addr for phys addr is phdr */
          if(dest_addr == NULL)
          {
            //This is needed only in case when we are using Physical address
            status = pil_map_and_get_virt_addr(phdr_entry_paddr_temp, chunk_size, &chunk_dest_addr, &MemObj);
            if(status != PIL_SUCCESS)
            {
              PIL_LOG_ERR(3,"pil_map_and_get_virt_addr failed with status %d for phys addr 0x%x, size %d", status, (uint32)phdr_entry_paddr_temp, (uint32)chunk_size);
              return status;
            }
          }
          else
            chunk_dest_addr = phdr_entry_paddr_temp;

          memset((void *)(unsigned long)chunk_dest_addr,
                  0,
                  chunk_size);

        if(dest_addr == NULL)
        {
          status = pil_unmap_virt_addr(&MemObj);
          if( status != PIL_SUCCESS)
          {
            PIL_LOG_ERR(1,"pil_unmap_virt_addr failed with err %d", status);
            return status;
          }
        }
        phdr_entry_paddr_temp += chunk_size;

        remaining_data = remaining_data - chunk_size;
        }
      
        /* Update size indicating how much elf is loaded */
        PIL_VERIFY((handle->total_elf_segments_size + (phdr_entry_memsz - phdr_entry_filesz) > handle->total_elf_segments_size),
                         "total_elf_segments_size gets overflowed", handle->total_elf_segments_size, phdr_entry_memsz, phdr_entry_filesz); 
        handle->total_elf_segments_size = handle->total_elf_segments_size + (phdr_entry_memsz - phdr_entry_filesz);
      }
      PIL_LOG_INFO(3,"filesz: %d, memsz: %d, total_elf_seg_size: %d", (uint32)phdr_entry_filesz, (uint32)phdr_entry_memsz, (uint32)handle->total_elf_segments_size);
    }
    else
    {
      PIL_LOG_INFO(1,"non-loadable segment and Force is FALSE", 0);
      return PIL_NON_LOADABLE_SEGMENT;
    }
  }
  else
  {
    PIL_LOG_INFO(1,"skipping segment loading for 0x%x phdr", phdr);
    status = PIL_SUCCESS;
  }
  
  return PIL_SUCCESS;
} /* pil_elf_load_seg_mba */
#endif /* PIL_MBA_FILTER */

/*=============================================================================
FUNCTION 
  pil_elf_load_seg_tz

DESCRIPTION
  This function loads the data segment (loadable and non-loadable if force is TRUE) 
  into memory using the information from a specific program header. This function is
  specifically for the case where authentication is done by TZ.
  
Note1: If dest_addr is non-Null, segment data will be copied at address in dest_addr.
          dest_addr should be Virtual address
Note2: If force is TRUE, this tries to copy data of a non-loadable segment as well
Note3: If reloc_offset is set and dest_addr is NULL, will add reloc_offset to 
          phys address in program segment and loads segment data at that location

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle - Handle to Initialized Elf Structure
  phdr - Pointer to the Program Header structure 
  dest_addr - If the dest_addr is non-NULL, will load segment data at that address
              rather than using address in Phdr. If Null, will loads Segment data
              at Physical address indicated in given Program header structure
  force - If TRUE, will load non-loadable segment as well.

RETURN VALUE
  PIL_SUCCESS
  PIL_NON_LOADABLE_SEGMENT
  PIL_FLASH_READ_FAILED
  Other errors
=============================================================================*/
#ifdef PIL_TZ_FILTER
static pil_error_type pil_elf_load_seg_tz(pil_elf_struct_type *handle, void *phdr, void *dest_addr, boolean force)
{
  uint32 chunk_size = MAX_CHUNK_SIZE;
  uint64 remaining_data = 0, chunk_dest_addr =0, chunk_offset =0;
  uint64 phdr_entry_offset, phdr_entry_paddr, phdr_entry_filesz, phdr_entry_memsz; 
  pil_error_type status = PIL_FAILURE;
  uint64 pa_va_offset;

  
  #ifdef PIL_DALSYS_AVAILABLE
    DALSYSMemHandle MemObj = 0;
  #else
    qurt_mem_region_t MemObj = 0;
  #endif

  PIL_VERIFY( phdr != NULL, "phdr is NULL", 0, 0, 0);
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected magic: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return PIL_ELF_HANDLE_NOT_INITED;
  }

  //Currently not doing any Authentication, MBA will be doing that
  //status = pil_elf_is_auth_enabled(&is_auth_enabled);

  /* If booting from Hibernation, check if this segment needs to be loaded or not */
  if(pil_elf_phdr_is_loadable_in_hibernation(handle, phdr))
  {
    if (handle->elf_format == ELF_CLASS_64)
    {
      phdr_entry_offset = ((Elf64_Phdr *)phdr)->p_offset;
      if(dest_addr == NULL)
      {
        phdr_entry_paddr = ((Elf64_Phdr *)phdr)->p_paddr + handle->reloc_offset;
        pa_va_offset = phdr_entry_paddr - modem_start_pa; /* This will give the offset from the beginning. */
      }
      else
      {
        phdr_entry_paddr = (unsigned int)dest_addr;
      }
      phdr_entry_filesz = ((Elf64_Phdr *)phdr)->p_filesz;
      phdr_entry_memsz = ((Elf64_Phdr *)phdr)->p_memsz;
    }
    else
    {
      phdr_entry_offset = ((Elf32_Phdr *)phdr)->p_offset;
      if(dest_addr == NULL)
      {
        phdr_entry_paddr = ((Elf32_Phdr *)phdr)->p_paddr + handle->reloc_offset;
        pa_va_offset = phdr_entry_paddr - modem_start_pa; /* This will give the offset from the beginning. */
      }
      else
      {
        phdr_entry_paddr = (unsigned int)dest_addr;
      }
      phdr_entry_filesz = ((Elf32_Phdr *)phdr)->p_filesz;
      phdr_entry_memsz = ((Elf32_Phdr *)phdr)->p_memsz;
    }
  
    if (pil_elf_phdr_is_loadable(handle, phdr) || force == TRUE)
    {
      /* Only process segments with valid parameters */
      /* 0x0 is a valid address. Hence no need to validate dest_addr for null pointer 
      */
	  
      if(dest_addr == NULL)
        chunk_dest_addr = modem_start_va + pa_va_offset; /* This is the virtual address from which the segment
                                                              will be loaded. */
      else
        chunk_dest_addr = phdr_entry_paddr;
	  
	  // This is for reading from partition in flash
      if (phdr_entry_filesz)
      {
        remaining_data = phdr_entry_filesz;
        chunk_offset = phdr_entry_offset;
        while(remaining_data !=0)
        {
          if(remaining_data < chunk_size )
          {
            chunk_size = remaining_data;
          }
          
          /* Copy the data segment flash into memory */
          status = pil_flash_read_partition( handle,
                                           (void *)(unsigned long)chunk_dest_addr,
                                           chunk_offset,
                                           chunk_size );
          if(status != PIL_SUCCESS)
          {
            if(dest_addr == NULL)
              pil_unmap_virt_addr(&MemObj);
            
            PIL_LOG_ERR(3,"Couldnt read flash for segment loading", status, (uint32)chunk_offset, chunk_size);
            return PIL_FLASH_READ_FAILED;
          }

          chunk_dest_addr += chunk_size;
          chunk_offset += chunk_size;
          remaining_data = remaining_data - chunk_size;
          
          /* Update size indicating how much elf is loaded */
          PIL_VERIFY((handle->total_elf_segments_size + chunk_size > handle->total_elf_segments_size), "total_elf_segments_size may get overflowed",
                    handle->total_elf_segments_size, chunk_size, 0); 
          handle->total_elf_segments_size = handle->total_elf_segments_size + chunk_size;
        }
      }
    
      /* If uninitialized data exists, make sure to zero-init */
      if (phdr_entry_memsz > phdr_entry_filesz)
      {
        remaining_data = phdr_entry_memsz - phdr_entry_filesz;
        chunk_size = MAX_CHUNK_SIZE;
        
        while(remaining_data !=0)
        {
          if(remaining_data < chunk_size )
          {
            chunk_size = remaining_data;
          }
          
          /* If dest_addr is NULL, need to get virt addr for phys addr is phdr */
          memset((void *)(unsigned long)chunk_dest_addr,
                  0,
                  chunk_size);

          chunk_dest_addr += chunk_size;
          remaining_data = remaining_data - chunk_size;
        }
      
        /* Update size indicating how much elf is loaded */
        PIL_VERIFY((handle->total_elf_segments_size + (phdr_entry_memsz - phdr_entry_filesz) > handle->total_elf_segments_size),
                         "total_elf_segments_size gets overflowed", handle->total_elf_segments_size, phdr_entry_memsz, phdr_entry_filesz); 
        handle->total_elf_segments_size = handle->total_elf_segments_size + (phdr_entry_memsz - phdr_entry_filesz);
      }
      PIL_LOG_INFO(3,"filesz: %d, memsz: %d, total_elf_seg_size: %d", (uint32)phdr_entry_filesz, (uint32)phdr_entry_memsz, (uint32)handle->total_elf_segments_size);
    }
    else
    {
      PIL_LOG_INFO(1,"non-loadable segment and Force is FALSE", 0);
      return PIL_NON_LOADABLE_SEGMENT;
    }
  }
  else
  {
    PIL_LOG_INFO(1,"skipping segment loading for 0x%x phdr", phdr);
    status = PIL_SUCCESS;
  }
  
  return PIL_SUCCESS;
} /* pil_elf_load_seg_tz */
#endif /* PIL_TZ_FILTER */



/*=============================================================================
FUNCTION 
  pil_verify_elf_header

DESCRIPTION
  This function verifies the ELF Header by sanity checking various fields
  within the ELF Header structure 

DEPENDENCIES
  Should be called after reading Elf header.
  
INPUTS
  Elf Handle[in] - Handle to Initialized Elf Structure

RETURN VALUE
  TRUE - If ELF Header has passed verification
  FALSE - Otherwise
=============================================================================*/
static boolean pil_verify_elf_header(pil_elf_struct_type *handle)
{
  /* Sanity check the ELF header,
    * and ensure that there is at least 1 prog-header entry
    * and total number of prog-hdr segments are not more than the allocated prog-hdr buffer
    */
  if (handle->elf_format == ELF_CLASS_32)
    return (handle->elf.e32.ehdr.e_ident[ELFINFO_MAG0_INDEX] == ELFINFO_MAG0 && 
            handle->elf.e32.ehdr.e_ident[ELFINFO_MAG1_INDEX] == ELFINFO_MAG1 &&
            handle->elf.e32.ehdr.e_ident[ELFINFO_MAG2_INDEX] == ELFINFO_MAG2 &&
            handle->elf.e32.ehdr.e_ident[ELFINFO_MAG3_INDEX] == ELFINFO_MAG3 &&
            handle->elf.e32.ehdr.e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_32 &&
            handle->elf.e32.ehdr.e_ident[ELFINFO_VERSION_INDEX] == ELF_VERSION_CURRENT &&
            handle->elf.e32.ehdr.e_ehsize == sizeof(Elf32_Ehdr) &&
            handle->elf.e32.ehdr.e_phentsize == sizeof(Elf32_Phdr) &&
            handle->elf.e32.ehdr.e_phnum != 0 &&
            handle->elf.e32.ehdr.e_phnum <= MI_PBT_MAX_SEGMENTS);
            
  else if (handle->elf_format == ELF_CLASS_64)
  return (  handle->elf.e64.ehdr.e_ident[ELFINFO_MAG0_INDEX] == ELFINFO_MAG0 && 
            handle->elf.e64.ehdr.e_ident[ELFINFO_MAG1_INDEX] == ELFINFO_MAG1 &&
            handle->elf.e64.ehdr.e_ident[ELFINFO_MAG2_INDEX] == ELFINFO_MAG2 &&
            handle->elf.e64.ehdr.e_ident[ELFINFO_MAG3_INDEX] == ELFINFO_MAG3 &&
            handle->elf.e64.ehdr.e_ident[ELFINFO_CLASS_INDEX] == ELF_CLASS_64 &&
            handle->elf.e64.ehdr.e_ident[ELFINFO_VERSION_INDEX] == ELF_VERSION_CURRENT &&
            handle->elf.e64.ehdr.e_ehsize == sizeof(Elf64_Ehdr) &&
            handle->elf.e64.ehdr.e_phentsize == sizeof(Elf64_Phdr) &&
            handle->elf.e64.ehdr.e_phnum != 0 &&
            handle->elf.e64.ehdr.e_phnum <= MI_PBT_MAX_SEGMENTS);

  else
    return FALSE; // invalid
} /* pil_verify_elf_header */


/*=============================================================================
FUNCTION 
  pil_verify_elf_prog_hdr_table

DESCRIPTION
  This function sanity checks the ELF program header table

DEPENDENCIES
  Should be called after reading Elf Program header table.
  
INPUTS
  Elf Handle[in] - Handle to Initialized Elf Structure

RETURN VALUE
  TRUE - If ELF Header has passed verification
  FALSE - Otherwise
=============================================================================*/
static pil_error_type pil_verify_elf_prog_hdr_table(pil_elf_struct_type *handle)
{
  uint32 index = 0;  
  void *phdr_entry_ptr;
  //uint32 phdr_flags;
  uint64 phdr_memsz, phdr_filesz;
  //pil_error_type status = PIL_SUCCESS;
  
  for(index=0; index < handle->phdr_num; index++)
  {
    if (handle->elf_format == ELF_CLASS_64)
    {
      phdr_entry_ptr = &handle->elf.e64.phdr_array[index];
      //phdr_flags = handle->elf.e64.phdr_array[index].p_flags;
      phdr_memsz = handle->elf.e64.phdr_array[index].p_memsz;
      phdr_filesz = handle->elf.e64.phdr_array[index].p_filesz;
    }
    else
    {
      phdr_entry_ptr = &handle->elf.e32.phdr_array[index];
      //phdr_flags = handle->elf.e32.phdr_array[index].p_flags;
      phdr_memsz = handle->elf.e32.phdr_array[index].p_memsz;
      phdr_filesz = handle->elf.e32.phdr_array[index].p_filesz;
    }

    /* Check to make sure loadable segment has valid size */
    if (pil_elf_phdr_is_loadable(handle, phdr_entry_ptr))
    {
      //PIL_VERIFY( (phdr_memsz >= phdr_filesz), "Invalid Program Header sizes", index, phdr_memsz, phdr_filesz);
      if(phdr_memsz < phdr_filesz)
      {
        PIL_LOG_ERR(3,"for segment %d: phdr_memsz(%d) < phdr_filesz(%d) failed", index, phdr_memsz, phdr_filesz);
        return PIL_ELF_PROG_TABLE_INVALID;
      }
    }
  }
  
  //Todo: Check if more checks can be added over prog hdr table
  //Todo: Try to add checks to check boundaries of MBA, Modem memory ranges from segments details.. 
  //They should be inline with what memory regions we give from config file
  //Otherwise assert here itself.
  
  return PIL_SUCCESS;
} /* pil_verify_elf_prog_hdr_table */


/*=============================================================================
FUNCTION 
  pil_elf_loader_init

DESCRIPTION
  This function initializes Elf structure handle by opening given partition
  in Flash storage, reading elf header, program segments, basic sanity checking

DEPENDENCIES
  None.
  
INPUTS
  Partition Name[in] - Partition name whose elf data needs to be read
  Elf Handle[out] - Initialized Elf Structure Handle

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
  PIL_MALLOC_FAILURE
  PIL_FLASH_READ_FAILED
  so on
=============================================================================*/
pil_error_type pil_elf_loader_init(char* partition_name, pil_elf_struct_type **elf_handle)
{
  pil_error_type status;
  boolean bool_status;
  pil_elf_struct_type *handle;
  uint64 ehdr_phoff;
  uint16 phdr_array_size;
  
  if(elf_handle == NULL)
  {
    PIL_LOG_ERR(1,"elf_handle passed is NULL", 0);
    return PIL_INVALID_PARAMETERS;
  }

  handle = calloc(1, sizeof(pil_elf_struct_type));
  if(handle == NULL)
  {
    PIL_LOG_ERR(1,"handle malloc failed", 0);
    return PIL_MALLOC_FAILURE;
  }
  
  //Copy partition name to elf structure
  memscpy(handle->partition_name, MAX_PIL_NAME_SIZE, partition_name, MAX_PIL_NAME_SIZE);

  //Open Flash partition
  status = pil_flash_open_partition(handle, partition_name);
  if(status != PIL_SUCCESS)
  {
    free(handle);
    PIL_LOG_ERR(1,"partition opening failed with err %d", status);
    return PIL_PARTITION_OPEN_FAILURE;
  }
  
  //Read Elf header.. First try to read 64bit elf header size
  status = pil_flash_read_partition(handle, &handle->elf.e64.ehdr, 0, sizeof(Elf64_Ehdr));
  if(status != PIL_SUCCESS)
  {
    pil_flash_close_partition(handle);
    free(handle);
    PIL_LOG_ERR(1,"Failed to read elf header with status %d", status);
    return PIL_FLASH_READ_FAILED;
  }

   /* Check the EI_CLASS to see if it's 32bit or 64bit ELF 
    * Note the e_ident format is the same in both 32bit and 64bit
    */
  handle->elf_format = handle->elf.e64.ehdr.e_ident[ELFINFO_CLASS_INDEX];
  
  //do basic sanity of elf header
  bool_status = pil_verify_elf_header(handle);
  if(bool_status == FALSE)
  {
    pil_flash_close_partition(handle);
    free(handle);
    PIL_LOG_ERR(1,"Elf header verification failed", 0);
    return PIL_ELF_HEADER_INVALID;
  }

  //copy elf header details based of elf class type
  if (handle->elf_format == ELF_CLASS_64)
  {
    handle->ehdr_ptr =&handle->elf.e64.ehdr;
    handle->ehdr_size = sizeof(Elf64_Ehdr);
    handle->phdr_size = handle->elf.e64.ehdr.e_phentsize;  
    handle->phdr_num = handle->elf.e64.ehdr.e_phnum;
    handle->phdr_array_ptr = handle->elf.e64.phdr_array = calloc(handle->phdr_num, sizeof(Elf64_Phdr));
  }
  else
  {
    handle->ehdr_ptr =&handle->elf.e32.ehdr;
    handle->ehdr_size = sizeof(Elf32_Ehdr);
    handle->phdr_size = handle->elf.e32.ehdr.e_phentsize;
    handle->phdr_num = handle->elf.e32.ehdr.e_phnum;
    handle->phdr_array_ptr = handle->elf.e32.phdr_array = calloc(handle->phdr_num, sizeof(Elf32_Phdr));
  }
  
  if(handle->phdr_array_ptr == NULL)
  {
    pil_flash_close_partition(handle);
    free(handle);
    PIL_LOG_ERR(1,"phdr_array malloc failed", 0);
    return PIL_MALLOC_FAILURE;
  }
  
  //Read Program header Table
  ehdr_phoff = (handle->elf_format == ELF_CLASS_64) ? handle->elf.e64.ehdr.e_phoff : handle->elf.e32.ehdr.e_phoff;
  phdr_array_size = handle->phdr_num * handle->phdr_size;

  /* Load the program headers */
  status = pil_flash_read_partition( handle,
                                   handle->phdr_array_ptr,
                                   (uint32)ehdr_phoff,
                                   phdr_array_size);
  
  if(status != PIL_SUCCESS)
  {
    pil_flash_close_partition(handle);
    free(handle->phdr_array_ptr);
    free(handle);
    PIL_LOG_ERR(1,"Failed to read elf program table with status %d", status);
    return PIL_FLASH_READ_FAILED;
  }
  
  //do basic sanity of elf header
  status = pil_verify_elf_prog_hdr_table(handle);
  if(status != PIL_SUCCESS)
  {
    pil_flash_close_partition(handle);
    free(handle->phdr_array_ptr);
    free(handle);
    PIL_LOG_ERR(1,"Elf Program header verification failed with status %d", status);
    return status;
  }
  
  PIL_LOG_INFO(1,"successfully read elf header, program tables for %s partition", partition_name);
  
  //Update magic flag for security check
  handle->magic = MAGIC_FLAG;
  *elf_handle = handle;
  
  return PIL_SUCCESS;
} /* pil_elf_loader_init */


/*=============================================================================
FUNCTION 
  pil_elf_loader_deinit

DESCRIPTION
  This function de-initializes Elf structure handle by closing given partition
  in Flash storage, freeing up elf structure memory

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle[in] - Initialized Elf Structure Handle

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
=============================================================================*/
pil_error_type pil_elf_loader_deinit(pil_elf_struct_type **elf_handle)
{
  pil_elf_struct_type *handle;
  pil_error_type status;
  
  if(elf_handle == NULL || (*elf_handle) == NULL || (*elf_handle)->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"elf_handle passed is NULL or not valid", 0, 0, 0);
    return PIL_INVALID_PARAMETERS;
  }
  
  handle = *elf_handle;
  *elf_handle = NULL;
  
  status = pil_flash_close_partition(handle);
  if(status != PIL_SUCCESS)
    PIL_LOG_ERR(3,"flash partition close failed with status %d", status, 0, 0);
  
  free(handle->phdr_array_ptr);
  free(handle);
  
  return PIL_SUCCESS;
} /* pil_elf_loader_deinit */


/*=======================================================================================
FUNCTION 
  pil_get_hash_seg_size

DESCRIPTION
  This function returns the size of the hash segment.

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle[in] - Initialized Elf Structure Handle

RETURN VALUE
  Size of hash segment
  0 if elf handle is NULL, magic number is not initialized or if no hash segemnt is found
===========================================================================================*/
uint64 pil_get_hash_seg_size(pil_elf_struct_type *handle)
{
  uint32 index = 0;  
  uint32 phdr_flags = 0;
  uint64 phdr_memsz = 0;

  if(handle == NULL)
  {
  	PIL_LOG_ERR(1,"handle is NULL",0);
	return 0;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return 0;
  }
  
  for(index=0; index<handle->phdr_num; index++)
  {
    if (handle->elf_format == ELF_CLASS_64)
    {
      phdr_flags = handle->elf.e64.phdr_array[index].p_flags;
      phdr_memsz = handle->elf.e64.phdr_array[index].p_memsz;
    }
    else
    {
      phdr_flags = handle->elf.e32.phdr_array[index].p_flags;
      phdr_memsz = handle->elf.e32.phdr_array[index].p_memsz;
    }

    /* Check for the hash segment program header to parse QC header */
    if( MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) == MI_PBT_HASH_SEGMENT)
    {
      return phdr_memsz;
    }
  }
  
  return 0; /* Did not find any hash segment so returning 0 */
} /* pil_elf_read_hash_seg */

/*=======================================================================================
FUNCTION 
  pil_get_meta_data_size

DESCRIPTION
  This function returns the size of the meta data which is as follows
  Elf header size + (num of segments/prg hdrs * size of each prg hdr) + size of hash segment

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle[in] - Initialized Elf Structure Handle

RETURN VALUE
  Size of meta data
  0 if elf handle is NULL, magic number is not initialized
===========================================================================================*/


uint64 pil_get_meta_data_size(pil_elf_struct_type *handle)
{
  uint64 hash_seg_size = 0;
  uint64 meta_data_size = 0;

  if(handle == NULL)
  {
	PIL_LOG_ERR(1,"handle is NULL",0);
	return 0;
  }

  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return 0;
  }

  /* Retrieving size of hash segment */
  hash_seg_size = pil_get_hash_seg_size(handle);

  /* Elf header size + (num of segments/prg hdrs * size of each prg hdr) + size of hash segment */
  meta_data_size = handle->ehdr_size + (handle->phdr_num * handle->phdr_size) + hash_seg_size;

  return meta_data_size;
  
}

/*=============================================================================
FUNCTION 
  pil_elf_read_hash_seg

DESCRIPTION
  This function copies Hash segment data to given virtual buffer address

DEPENDENCIES
  None.
  
INPUTS
  Elf Handle[in] - Initialized Elf Structure Handle
  hash_buf[in] - virtual address of buffer where hash data to be copied
  hash_size[in] - size of input buffer

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
  PIL_ELF_HANDLE_NOT_INITED
=============================================================================*/
pil_error_type pil_elf_read_hash_seg(pil_elf_struct_type *handle, void *hash_buf, uint32 hash_size)
{
  uint32 index = 0;  
  void *phdr_entry_ptr;
  uint32 phdr_flags;
  //uint64 phdr_memsz, phdr_filesz;
  pil_error_type status = PIL_SUCCESS;
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return PIL_ELF_HANDLE_NOT_INITED;
  }
  
  for(index=0; index<handle->phdr_num; index++)
  {
    if (handle->elf_format == ELF_CLASS_64)
    {
      phdr_entry_ptr = &handle->elf.e64.phdr_array[index];
      phdr_flags = handle->elf.e64.phdr_array[index].p_flags;
      //phdr_memsz = handle->elf.e64.phdr_array[index].p_memsz;
      //phdr_filesz = handle->elf.e64.phdr_array[index].p_filesz;
    }
    else
    {
      phdr_entry_ptr = &handle->elf.e32.phdr_array[index];
      phdr_flags = handle->elf.e32.phdr_array[index].p_flags;
      //phdr_memsz = handle->elf.e32.phdr_array[index].p_memsz;
      //phdr_filesz = handle->elf.e32.phdr_array[index].p_filesz;
    }

    /* Check for the hash segment program header to parse QC header */
    if( MI_PBT_SEGMENT_TYPE_VALUE(phdr_flags) == MI_PBT_HASH_SEGMENT)
    {
      #ifdef PIL_MBA_FILTER
        status = pil_elf_load_seg_mba(handle, phdr_entry_ptr, hash_buf, TRUE);  //True since hash segment is generally non-loadable segment
      #else
	    status = pil_elf_load_seg_tz(handle, phdr_entry_ptr, hash_buf, TRUE);  //True since hash segment is generally non-loadable segment
      #endif  /* PIL_MBA_FILTER */
	  
	  PIL_LOG_INFO(1,"hash segment loading status: %d", status, 0, 0);
      break;
    }
  }
  
  return status;
} /* pil_elf_read_hash_seg */


/*=============================================================================
FUNCTION 
  pil_reconstruct_mba_elf_image

DESCRIPTION
  This function reconstructs the ELF image in one blob as laid out in the flash
  This tries to load elf hdr + prog hdr table before hash segment, hash segment
  before other segments and loads rest all segments according to their offsets
  
  NOTE: image_load_base and image_max_size are assumed to be set 
        by caller that uses correct address/size
        
DEPENDENCIES
  None.
  
INPUTS
  elf handle - Initialized elf handle
  image_load_base  - physical base address where the image will be reconstructed
  image_max_size   - maximum size of the region used for reconstruction

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
  PIL_ELF_HANDLE_NOT_INITED
=============================================================================*/
#ifdef PIL_MBA_FILTER
pil_error_type pil_reconstruct_mba_elf_image(pil_elf_struct_type *handle, uint8 *image_load_base, uint32 image_max_size)
{
  uint32 index = 0;  
  uint64 phdr_offset;
  void *phdr_entry_ptr;
  //uint32 seg_size=0;
  pil_error_type status = PIL_SUCCESS;
  #ifdef PIL_DALSYS_AVAILABLE
  DALSYSMemHandle MemObj = 0;
  #else
  qurt_mem_region_t MemObj = 0;
  #endif
  
  PIL_LOG_INFO(2,"load_base: 0x%x, load_size: 0x%x", (uint32)image_load_base, image_max_size);

  if(handle == NULL)
  {
    PIL_LOG_ERR(3,"handle is NULL", 0, 0, 0);
    return PIL_INVALID_PARAMETERS;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return PIL_ELF_HANDLE_NOT_INITED;
  }
  
  // After this image_load_base will contain virt addr
  status = pil_map_and_get_virt_addr((uint64)image_load_base, image_max_size, (uint64 *)&image_load_base, &MemObj);
  if(status != PIL_SUCCESS)
  {
    PIL_LOG_ERR(3,"pil_map_and_get_virt_addr returned %d for phyd addr 0x%x size %d", status, (uint32)image_load_base, image_max_size);
    return status;
  }
    
  /* Sanity check the address and size */
  PIL_VERIFY(((image_load_base + image_max_size) > image_load_base), "pil_reconstruct_mba_elf_image: address overflow", PIL_FAILURE, 0, 0);
  
  /* Zero out the memory region till first segment. Rest will be 
     zeroed out as needed as part of load_seg function */
  memset((uint8*)image_load_base,
        0,
        ((handle->elf_format == ELF_CLASS_64) ? handle->elf.e64.phdr_array[1].p_offset : handle->elf.e32.phdr_array[1].p_offset));
  
  /* Copy the ELF header to the image destination */
  memscpy((void *)(unsigned int)image_load_base, image_max_size, handle->ehdr_ptr, handle->ehdr_size);
  
  /* Append the program header table */
  memscpy((void *)(unsigned int)(image_load_base + handle->ehdr_size), image_max_size, handle->phdr_array_ptr, (handle->phdr_size * handle->phdr_num));
  
  // Starting from 1st segment since 0th segment is elf hdr + phdr tbl 
  // which is already copied above
  for (index = 1; index < handle->phdr_num; index++)
  {
    if (handle->elf_format == ELF_CLASS_64)
    {
      phdr_entry_ptr = &handle->elf.e64.phdr_array[index];
      phdr_offset = handle->elf.e64.phdr_array[index].p_offset;
    }
    else
    {
      phdr_entry_ptr = &handle->elf.e32.phdr_array[index];
      phdr_offset = handle->elf.e32.phdr_array[index].p_offset;
    }
  
    PIL_LOG_INFO(2,"pil_elf_load_seg_mba loading segment %d at offset 0x%x", index, phdr_offset);
    
    //Todo: need to enable the check
    /* Make sure the image being copied doesn't exceed the region's limit,
      * also check against integer overflow 
      */
    //BL_VERIFY(((uint32)(phdr_offset + phdr_filesz) <= image_max_size) && 
      //        ((phdr_offset + phdr_filesz)  >= phdr_offset) &&
        //      ((image_load_base + (uint32)phdr_offset) >= image_load_base), 
          //    BL_ERR_IMG_SIZE_TOO_BIG);
  
    status = pil_elf_load_seg_mba(handle, phdr_entry_ptr, (void *)(unsigned int)(image_load_base + phdr_offset), TRUE);  //TRUE since we want to load hash segment as well
    if(status != PIL_SUCCESS)
    {
      pil_unmap_virt_addr(&MemObj);
      PIL_LOG_ERR(2,"pil_elf_load_seg_mba returned %d for segment %d", status, index);
      return status;
    }
  }
  
  status = pil_unmap_virt_addr(&MemObj);
  if(status != PIL_SUCCESS)
  {
    PIL_LOG_ERR(1,"pil_unmap_virt_addr failed with status %d", status);
    return status;
  }
  
  return status;
} /* pil_reconstruct_mba_elf_image */
#endif /* PIL_MBA_FILTER */

/*=============================================================================
FUNCTION 
  pil_elf_populate_meta_info_mba

DESCRIPTION
  This function will populate a buffer with the meta data for the current
  elf image being processed. Meta data format: elf header followed by 
  program header table followed by hash segment. This function is specifically
  for the case where authentication is done by mba.
  
  NOTE: meta_buff_ptr and meta_buff_size are assumed to be set 
        by caller that uses correct address/size
        
DEPENDENCIES
  None.
  
INPUTS
  elf handle - initialized elf structure handle
  meta_buff_ptr - physical base address where the metadata needs to be copied
  meta_buff_size - maximum size of the buffer

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
  PIL_ELF_HANDLE_NOT_INITED
=============================================================================*/
#ifdef PIL_MBA_FILTER
pil_error_type pil_elf_populate_meta_info_mba(pil_elf_struct_type *handle, uint8 *meta_buff_ptr, uint64 meta_buff_size)
{
  pil_error_type status;
  uint32 result = 0;
  #ifdef PIL_DALSYS_AVAILABLE
  DALSYSMemHandle MemObj = 0;
  #else
  qurt_mem_region_t MemObj = 0;
  #endif

  
  if(handle == NULL)
  {
    PIL_LOG_ERR(1,"handle is NULL", 0);
    return PIL_INVALID_PARAMETERS;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return PIL_ELF_HANDLE_NOT_INITED;
  }
  
  if(meta_buff_ptr == NULL)
  {
    PIL_LOG_ERR(1,"meta_buff_ptr is NULL", 0);
    return PIL_INVALID_PARAMETERS;
  }
  
  // After this image_load_base will contain virt addr
  status = pil_map_and_get_virt_addr((uint64)meta_buff_ptr, meta_buff_size, (uint64 *)&meta_buff_ptr, &MemObj);
  if(status != PIL_SUCCESS)
  {
    PIL_LOG_ERR(3,"pil_map_and_get_virt_addr returned %d for phyd addr 0x%x size %d", status, (uint32)meta_buff_ptr, meta_buff_size);
    return status;
  }

  /* zi init max possible meta_buff_ptr with ehdr, phdr,
     hash segment will be memset by load_seg function */
  memset((uint8*)meta_buff_ptr,
          0,
          (handle->ehdr_size + 
           (handle->phdr_size * handle->phdr_num)));

  /* Copy elf header to buffer */
  memscpy(meta_buff_ptr, meta_buff_size, handle->ehdr_ptr, handle->ehdr_size);

  /* Update buffer to point to next free location */
  meta_buff_ptr = meta_buff_ptr + handle->ehdr_size;
  meta_buff_size = meta_buff_size - handle->ehdr_size;

  /* Copy program header table to buffer */
  memscpy(meta_buff_ptr, meta_buff_size, handle->phdr_array_ptr,
                 (handle->phdr_size * handle->phdr_num));

  /* Update buffer to point to next free location */
  meta_buff_ptr = meta_buff_ptr + 
                  (handle->phdr_size * handle->phdr_num);
  meta_buff_size = meta_buff_size - (handle->phdr_size * handle->phdr_num);

  /* Copy hash segment to buffer */
  result = pil_elf_read_hash_seg(handle, meta_buff_ptr, meta_buff_size);
  if(result!=PIL_SUCCESS)
  {
    PIL_LOG_ERR(1,"pil_elf_read_hash_seg failed and returned error code %d", result);
    return PIL_FAILURE;
  }


  /* Unmap whatever we have loaded till now i.e elf header, program headers, hash segment
     as all these are non-loadable segments. We forcefully loaded these to authenticate
     the metadata. */
  status = pil_unmap_virt_addr(&MemObj);
  if(status != PIL_SUCCESS)
  {
    PIL_LOG_ERR(1,"pil_unmap_virt_addr failed with status %d", status);
    return status;
  }
    
  return status;
} /* pil_elf_populate_meta_info_mba */
#endif /* PIL_MBA_FILTER */

/*=============================================================================
FUNCTION 
  pil_elf_populate_meta_info_tz

DESCRIPTION
  This function will populate a buffer with the meta data for the current
  elf image being processed. Meta data format: elf header followed by 
  program header table followed by hash segment. Then it will make a TZ
  call to validate the meta data.
  
  NOTE: meta_buff_ptr and meta_buff_size are assumed to be set 
        by caller that uses correct address/size
        
DEPENDENCIES
  None.
  
INPUTS
  elf handle - initialized elf structure handle
  meta_buff_ptr - physical base address where the metadata needs to be copied
  meta_buff_size - maximum size of the buffer

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
  PIL_ELF_HANDLE_NOT_INITED
=============================================================================*/
#ifdef PIL_TZ_FILTER
pil_error_type pil_elf_populate_meta_info_tz(pil_elf_struct_type *handle, uint8 *meta_buff_ptr, uint64 meta_buff_size)
{
  pil_error_type status;
  uint32 result = 0;
  #ifdef PIL_DALSYS_AVAILABLE
  DALSYSMemHandle MemObj = 0;
  #else
  qurt_mem_region_t MemObj = 0;
  #endif

  /* These will be used to keep a copy of meta_buff_size
     and meta_buff_ptr, so that the original values are
     passed to TZ for authentication. */
  uint64 meta_buff_size_copy = pil_get_meta_data_size(handle);
  uint8* meta_buff_ptr_copy = NULL;

  
  if(handle == NULL)
  {
    PIL_LOG_ERR(1,"handle is NULL", 0);
    return PIL_INVALID_PARAMETERS;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return PIL_ELF_HANDLE_NOT_INITED;
  }
  
  if(meta_buff_ptr == NULL)
  {
    PIL_LOG_ERR(1,"meta_buff_ptr is NULL", 0);
    return PIL_INVALID_PARAMETERS;
  }
  
  // After this image_load_base will contain virt addr
  status = pil_map_and_get_virt_addr((uint64)meta_buff_ptr, meta_buff_size, (uint64 *)&meta_buff_ptr, &MemObj);
  if(status != PIL_SUCCESS)
  {
    PIL_LOG_ERR(3,"pil_map_and_get_virt_addr returned %d for phyd addr 0x%x size %d", status, (uint32)meta_buff_ptr, meta_buff_size);
    return status;
  }

  meta_buff_ptr_copy = (uint8*)meta_buff_ptr;
	
  /* zi init max possible meta_buff_ptr with ehdr, phdr,
     hash segment will be memset by load_seg function */
  memset((uint8*)meta_buff_ptr,
          0,
          (handle->ehdr_size + 
           (handle->phdr_size * handle->phdr_num)));

  /* Copy elf header to buffer */
  memscpy(meta_buff_ptr, meta_buff_size, handle->ehdr_ptr, handle->ehdr_size);

  /* Update buffer to point to next free location */
  meta_buff_ptr = meta_buff_ptr + handle->ehdr_size;
  meta_buff_size = meta_buff_size - handle->ehdr_size;

  /* Copy program header table to buffer */
  memscpy(meta_buff_ptr, meta_buff_size, handle->phdr_array_ptr,
                 (handle->phdr_size * handle->phdr_num));

  /* Update buffer to point to next free location */
  meta_buff_ptr = meta_buff_ptr + 
                  (handle->phdr_size * handle->phdr_num);
  meta_buff_size = meta_buff_size - (handle->phdr_size * handle->phdr_num);

  /* Copy hash segment to buffer */
  result = pil_elf_read_hash_seg(handle, meta_buff_ptr, meta_buff_size);
  if(result!=PIL_SUCCESS)
  {
    PIL_LOG_ERR(1,"pil_elf_read_hash_seg failed and returned error code %d", result);
    return PIL_FAILURE;
  }

 		
  /* Validate the elf header, program header and hash segment using scm call */
  result = pil_validate_elf(meta_buff_ptr_copy,meta_buff_size_copy);
  	
  if(result!=PIL_SUCCESS)
  {
    PIL_LOG_ERR(1,"pil_validate_elf failed and scm call returned error %d", result);
    return PIL_FAILURE;
  }

  /* Unmap whatever we have loaded till now i.e elf header, program headers, hash segment
	 as all these are non-loadable segments. We forcefully loaded these to authenticate
	 the metadata. */
  status = pil_unmap_virt_addr(&MemObj);
  if(status != PIL_SUCCESS)
  {
    PIL_LOG_ERR(1,"pil_unmap_virt_addr failed with status %d", status);
    return status;
  }
    
  return status;
} /* pil_elf_populate_meta_info_tz */
#endif /* PIL_TZ_FILTER */

/*=============================================================================
FUNCTION 
  pil_elf_load_nth_segment

DESCRIPTION
  This function loads Nth segment into memory at physical address indicated in
  corresponding program header
  
DEPENDENCIES
  None.
  
INPUTS
  elf handle - initialized elf structure handle
  index - segment index whose data needs to be loaded into memory

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
  PIL_ELF_HANDLE_NOT_INITED
=============================================================================*/
pil_error_type pil_elf_load_nth_segment(pil_elf_struct_type *handle, uint32 index)
{
  uint64 phdr_offset;
  void *phdr_entry_ptr;
  pil_error_type status = PIL_SUCCESS;
    
  if(handle == NULL)
  {
    PIL_LOG_ERR(1,"handle is NULL", 0);
    return PIL_INVALID_PARAMETERS;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return PIL_ELF_HANDLE_NOT_INITED;
  }
  
  if(index > handle->phdr_num)
  {
    PIL_LOG_ERR(2,"index(%d) beyond total segments(%d)", index, handle->phdr_num);
    return PIL_INVALID_PARAMETERS;
  }
  
  {
    if (handle->elf_format == ELF_CLASS_64)
    {
      phdr_entry_ptr = &handle->elf.e64.phdr_array[index];
      phdr_offset = handle->elf.e64.phdr_array[index].p_offset;
    }
    else
    {
      phdr_entry_ptr = &handle->elf.e32.phdr_array[index];
      phdr_offset = handle->elf.e32.phdr_array[index].p_offset;
    }
  
    PIL_LOG_INFO(2,"pil_elf_load_seg_mba or pil_elf_load_seg_tz loading segment %d at offset 0x%x", index, phdr_offset);
    
    /* Make sure the image being copied doesn't exceed the region's limit,
      * also check against integer overflow 
      */
    //BL_VERIFY(((uint32)(phdr_offset + phdr_filesz) <= image_max_size) && 
      //        ((phdr_offset + phdr_filesz)  >= phdr_offset) &&
        //      ((image_load_base + (uint32)phdr_offset) >= image_load_base), 
          //    BL_ERR_IMG_SIZE_TOO_BIG);
	#ifdef PIL_MBA_FILTER
      status = pil_elf_load_seg_mba(handle, phdr_entry_ptr, NULL, FALSE);  //FALSE since we want to load only loadable segments
    #else
      status = pil_elf_load_seg_tz(handle, phdr_entry_ptr, NULL, FALSE);  //FALSE since we want to load only loadable segments
    #endif /* PIL_MBA_FILTER */
	
	if(status != PIL_SUCCESS && status != PIL_NON_LOADABLE_SEGMENT)
    {
      PIL_LOG_ERR(2,"pil_elf_load_seg_mba or pil_elf_load_seg_tz returned %d for segment %d", status, index);
      return status;
    }
    
    //Adding minor delay between one segment and another loading.. Should we remove?
    //timer_sleep(50, T_MSEC, TRUE);
  }

  return status;
} /* pil_elf_load_nth_segment */


/*=============================================================================
FUNCTION 
  pil_elf_load_loadable_segments

DESCRIPTION
  This function loads all loadable segments into memory at physical addresses 
  indicated in corresponding program segment headers
  
DEPENDENCIES
  None.
  
INPUTS
  elf handle - initialized elf structure handle

RETURN VALUE
  PIL_SUCCESS
  PIL_INVALID_PARAMETERS
  PIL_ELF_HANDLE_NOT_INITED
=============================================================================*/
pil_error_type pil_elf_load_loadable_segments(pil_elf_struct_type *handle)
{
  uint32 index = 0;  
  //uint64 phdr_offset;
  //void *phdr_entry_ptr;
  pil_error_type status = PIL_SUCCESS;
  
  if(handle == NULL)
  {
    PIL_LOG_ERR(1,"handle is NULL", 0);
    return PIL_INVALID_PARAMETERS;
  }
  
  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return PIL_ELF_HANDLE_NOT_INITED;
  }

  /* Create a mapping for the entire modem image size. Following this only if
     authentication is done by TZ. */
  #ifdef PIL_TZ_FILTER
    status = pil_map_and_get_virt_addr(modem_start_pa, modem_image_size, &modem_start_va, &MemObj_global);
    if(status != PIL_SUCCESS)
    {
      PIL_LOG_ERR(3,"pil_map_and_get_virt_addr failed with status %d for phys addr 0x%x, size %d", status, (uint32)modem_start_pa, (uint32)modem_image_size);
      return status;
    }
  #endif

  
  for (index = 0; index < handle->phdr_num; index++)
  {
    status = pil_elf_load_nth_segment(handle, index);
    if(status != PIL_SUCCESS && status != PIL_NON_LOADABLE_SEGMENT)
    {
      PIL_LOG_ERR(2,"load_nth_segment returned %d for segment %d", status, index);
      return status;
    } 
    //If status == PIL_NON_LOADABLE_SEGMENT, change to PIL_SUCCESS
    status = PIL_SUCCESS;
  }
  
  return status;
} /* pil_elf_load_loadable_segments */


/*=============================================================================
FUNCTION 
  pil_get_modem_start_addr

DESCRIPTION
  This function returns the physical start address of the first loadable segment 
  of modem.
  
DEPENDENCIES
  None.
  
INPUTS
  elf handle - initialized elf structure handle

RETURN VALUE
  Physical start address of the first loadable segment of modem.
  0 if elf handle is NULL, magic number is not updated or all segments are non loadable.
=============================================================================*/

uint64 pil_get_modem_start_addr(pil_elf_struct_type *handle)
{
  uint64 modem_start_addr = 0;

  if(handle == NULL)
  {
    PIL_LOG_ERR(1,"handle is NULL",0);
	return 0;
  }

  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return 0;
  }

  if(handle->elf_format == ELF_CLASS_64)
  {
    Elf64_Phdr *p_array = malloc(sizeof(Elf64_Phdr));
	p_array = (Elf64_Phdr *)pil_find_first_loadable_segment(handle);
	if(p_array)
	{
	  /* Start physical address of the first loadable segment */
	  modem_start_addr = p_array->p_paddr;
	}  
	else
	{
	  PIL_LOG_ERR(1,"pil_find_first_loadable_segment returned NULL",0);
	  modem_start_addr = 0;
	}  
  }
  else
  {
    Elf32_Phdr *p_array = malloc(sizeof(Elf32_Phdr));
	p_array = (Elf32_Phdr *)pil_find_first_loadable_segment(handle);
	if(p_array)
	{
	  /* Start physical address of the first loadable segment */
	  modem_start_addr = p_array->p_paddr;
	}  
	else
	{
	  PIL_LOG_ERR(1,"pil_find_first_loadable_segment returned NULL",0);
	  modem_start_addr = 0;
	}      
  }
  
  return modem_start_addr;
}


/*====================================================================================================================
FUNCTION 
  pil_get_modem_image_size

DESCRIPTION
  This function returns the size of the modem image by calculating the following
  (Starting PA of last loadable segment + memsize of last loadable segment) - Starting PA of first loadable segment
  
DEPENDENCIES
  None.
  
INPUTS
  elf handle - initialized elf structure handle

RETURN VALUE
  Size of modem image if handle is non NULL
  0 if elf handle is NULL magic number is not updated or none of the segments are loadable
=======================================================================================================================*/


#ifdef PIL_TZ_FILTER
uint64 pil_get_modem_image_size(pil_elf_struct_type *handle)
{
  uint64 modem_image_size = 0;
  int num_seg = 0;

  if(handle == NULL)
  {
    PIL_LOG_ERR(1,"handle is NULL",0);
	return 0;
  }

  if(handle->magic != MAGIC_FLAG)
  {
    PIL_LOG_ERR(3,"handle 0x%x is not initialized. magic: 0x%x, expected: 0x%x", handle, handle->magic, MAGIC_FLAG);
    return 0;
  }

  if(handle->elf_format == ELF_CLASS_64)
  {
    for(num_seg=handle->phdr_num-1;num_seg>=0;num_seg--)
    {
      if(pil_elf_phdr_is_loadable(handle, (void *)&handle->elf.e64.phdr_array[num_seg]) == TRUE)
      {
        /* (Starting PA of last loadable segment + memsize of last loadable segment) - Starting PA of first loadable segment */
	    modem_image_size = (handle->elf.e64.phdr_array[num_seg].p_paddr + handle->elf.e64.phdr_array[num_seg].p_memsz) - modem_start_pa;
		return modem_image_size;
      }
    }
  }
  else
  {
    for(num_seg=handle->phdr_num-1;num_seg>=0;num_seg--)
    {
      if(pil_elf_phdr_is_loadable(handle, (void *)&handle->elf.e32.phdr_array[num_seg]) == TRUE)
      {
        /* (Starting PA of last loadable segment + memsize of last loadable segment) - Starting PA of first loadable segment */
	    modem_image_size = (handle->elf.e32.phdr_array[num_seg].p_paddr + handle->elf.e32.phdr_array[num_seg].p_memsz) - modem_start_pa;
		return modem_image_size;
      }
    }  
  }
 
  return 0; /* Did not find any loadable segment so returning 0 */
}
#endif /* #ifdef PIL_TZ_FILTER */


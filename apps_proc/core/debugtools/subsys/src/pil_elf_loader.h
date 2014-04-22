#ifndef __PIL_ELF_LOADER_H__
#define __PIL_ELF_LOADER_H__

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

Copyright (c) 2016-17, 2020 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil_elf_loader.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

07/28/16   ab      File created.
05/04/17   ab      Added Support for PIL Hibernation

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "pil.h"
#include "boot_elf_header.h"
#include "miprogressive.h"
#include "flash_ftl.h"

/*=============================================================================

                           DATA DECLARATIONS

=============================================================================*/

#define MAX_PIL_NAME_SIZE 8
#define MAGIC_FLAG 0xE1F00E1F

/* 32 bit elf header and phdr array structure */
typedef struct
{
  Elf32_Ehdr ehdr;
  Elf32_Phdr *phdr_array;
}pil_elf_32_type;

/* 64 bit elf header and phdr array structure */
typedef struct
{
  Elf64_Ehdr ehdr;
  Elf64_Phdr *phdr_array;
}pil_elf_64_type;

/* PIL Elf Structure */
typedef struct 
{
  uint32 magic;                                //Magic check
  char partition_name[MAX_PIL_NAME_SIZE];      //Partition name
  //uint8 partition_id;                          //
  uint8 elf_format;                            //32 or 64 bit format
  uint16 phdr_num;                             //No of Prog headers
  uint32 ehdr_size, phdr_size;                 //elf header, prog header sizes
  flash_ftl_client_t flash_if;                 //Flash FTL Handle
  void *ehdr_ptr, *phdr_array_ptr;             //pointer to ehdr, phdr below
  uint32  total_elf_segments_size;             //total size read from partition till now
  int reloc_offset;                            //relocatable offset
  boolean hiber_boot;                          //Indicates Boot from Hibernation
  
  union
  {
    pil_elf_32_type e32;                       //32 bit elf header and prog tbl array
    pil_elf_64_type e64;                       //64 bit elf header and prog tbl array
  }elf;
}pil_elf_struct_type;


/*=============================================================================

                            FUNCTION DECLARATIONS
                            
=============================================================================*/
/* For short description check this file header and in pil_elf_loader.c above
   each function for detailed description */

pil_error_type pil_elf_loader_init(char* parition_name, pil_elf_struct_type **handle);

pil_error_type pil_elf_loader_deinit(pil_elf_struct_type **handle);

pil_error_type pil_elf_read_hash_seg(pil_elf_struct_type *handle, void *hash_buf, uint32 hash_size);

pil_error_type pil_elf_load_nth_segment(pil_elf_struct_type *handle, uint32 index);

pil_error_type pil_elf_load_loadable_segments(pil_elf_struct_type *handle);

pil_error_type pil_set_reloadable_offset(pil_elf_struct_type *handle, int offset);

uint32 pil_get_read_elf_size(pil_elf_struct_type *handle);

unsigned int pil_elf_get_edhr_entry(pil_elf_struct_type *handle);

pil_error_type pil_elf_get_segments_count(pil_elf_struct_type *handle, uint32 *count);

pil_error_type pil_elf_get_segment_size(pil_elf_struct_type *handle, uint32 seg_idx, uint64 *size);

pil_error_type pil_reconstruct_mba_elf_image(pil_elf_struct_type *handle, uint8 *image_load_base, uint32 image_max_size);

#ifdef PIL_MBA_FILTER
  pil_error_type pil_elf_populate_meta_info_mba(pil_elf_struct_type *handle, uint8 *meta_buff_ptr, uint64 meta_buff_size);
#else
  pil_error_type pil_elf_populate_meta_info_tz(pil_elf_struct_type *handle, uint8 *meta_buff_ptr, uint64 meta_buff_size);
#endif /* PIL_MBA_FILTER */

pil_error_type pil_elf_set_hiber_boot(pil_elf_struct_type *handle, boolean enable);

uint64 pil_get_hash_seg_size(pil_elf_struct_type *handle);

uint64 pil_get_meta_data_size(pil_elf_struct_type *handle);

uint64 pil_get_modem_image_size(pil_elf_struct_type *handle);

uint64 pil_get_modem_start_addr(pil_elf_struct_type *handle);


#endif /* __PIL_ELF_LOADER_H__ */

#ifndef MEMHEAP_V_H
#define MEMHEAP_V_H
/**
  @file memheap_v.h
  @brief 
    Contains the heap manager internal interface.

*/
/*===========================================================================

  Copyright (c) 2014-2017, 2019 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/services/utils/src/memheap_v.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/16   nk       Added check to avoid 16Byte Block creation in MBA image
07/11/16   nk       Added check for footer forward offset within heap boundary
07/11/16   nk       Removal of 16byte blocks
04/07/14   rks      Initial Version
===========================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "memheap.h"
#include "modem_mem.h"        /* External interface to modem_mem.c */
#include <comdef.h> 

/*=============================================================================

                        MACRO DEFINITIONS

=============================================================================*/



#define OVERFLOW_CHECK(elt_count, elt_size) (!(elt_count >= (1U<<10) || elt_size >= (1U<<22)) || ((((uint64)elt_count * (uint64)elt_size) >> 32) == 0))

#define BOUNDARY_CHECK_SECTIONS(theBlock, heap_ptr) ((theBlock >= ((mem_heap_type*)heap_ptr)->sections[((mem_block_header_type *)theBlock)->section_index].start_addr) && (theBlock < (mem_block_header_type *)((char*)(((mem_heap_type*)heap_ptr)->sections[((mem_block_header_type *)theBlock)->section_index].start_addr) + ((mem_heap_type*)heap_ptr)->sections[((mem_block_header_type *)theBlock)->section_index].size)))

#define FRD_OFFSET_CHECK(block, heap_ptr) ((((mem_block_header_type *)block)->forw_offset + (char *)block) <= ((((char*)((mem_heap_type*)heap_ptr)->first_block) + ((mem_heap_type*)heap_ptr)->total_bytes)))

#define FRD_OFFSET_CHECK_SECTIONS(block, heap_ptr) ((((mem_block_header_type *)block)->forw_offset + (char *)block) <= ((((char*)((mem_heap_type*)heap_ptr)->sections[((mem_block_header_type *)block)->section_index].start_addr) + ((mem_heap_type*)heap_ptr)->sections[((mem_block_header_type *)block)->section_index].size)))

#define FOOTER_FRD_OFFSET_CHECK_SECTIONS(block, heap_ptr, footer_offset) ((footer_offset) <= ((char *)block - ((char*)(((mem_heap_type*)heap_ptr)->sections[((mem_block_header_type *)block)->section_index].start_addr))))

/* XOR based Guard byte calculations and restore */
#define INTEGRITY_CHECK_ON_USED_HEADER(magic_num_used, block) \
            (block[0]^block[1]^block[2]^block[3]^magic_num_used^((unsigned short)block)^((unsigned short)(((unsigned int)block)>> 16)))


#define INTEGRITY_CHECK_ON_FREE_HEADER(magic_num_free, block) \
            (block[0]^block[1]^block[2]^block[3]^block[4]^block[5]^magic_num_free^((unsigned short)block)^((unsigned short)(((unsigned int)block)>> 16)))


#define ADD_GUARD_BYTES_TO_USED_HEADER(magic_num_used, block) \
            (block[0] = block[1]^block[2]^block[3]^magic_num_used^((unsigned short)block)^((unsigned short)(((unsigned int)block)>> 16)))


#define ADD_GUARD_BYTES_TO_FREE_HEADER(magic_num_free, block) \
            (block[0] = block[1]^block[2]^block[3]^block[4]^block[5]^magic_num_free^((unsigned short)block)^((unsigned short)(((unsigned int)block)>> 16)))


#define MIN_HEAP_SIZE_FOR_BINS 0x20000 

#define MEMHEAP2_BINSIZE_OVERHEAD (sizeof(mem_block_header_type))

#define NEXT_BLOCK_SIZE(temp) ((mem_block_header_type*)(temp->nextPtr))->forw_offset

#ifdef FEATURE_MEM_DEBUG
#define MEMHEAP_MAX_EXTRA  31 // since it can be  (kMinBlockSize-1s)
#else
#define MEMHEAP_MAX_EXTRA  39 // since it can be  (kMinBlockSize - 1 +(kMinChunkSize - sizeof(mem_block_header_type)))
#endif

#define MEMHEAP2_BINSIZE0         (0x00000020)
#define MEMHEAP2_BINSIZE1         (0x00000030)
#define MEMHEAP2_BINSIZE2         (0x00000040)
#define MEMHEAP2_BINSIZE3         (0x00000050)
#define MEMHEAP2_BINSIZE4         (0x00000060)
#define MEMHEAP2_BINSIZE5         (0x00000080)
#define MEMHEAP2_BINSIZE6         (0x00000090)
#define MEMHEAP2_BINSIZE7         (0x000000C0)
#define MEMHEAP2_BINSIZE8         (0x000000E0)
#define MEMHEAP2_BINSIZE9         (0x00000100)
#define MEMHEAP2_BINSIZE10        (0x00000120)
#define MEMHEAP2_BINSIZE11        (0x00000150)
#define MEMHEAP2_BINSIZE12        (0x00000180)
#define MEMHEAP2_BINSIZE13        (0x00000200)
#define MEMHEAP2_BINSIZE14        (0x00000250)
#define MEMHEAP2_BINSIZE15        (0x00000300)
#define MEMHEAP2_BINSIZE16        (0x00000400)
#define MEMHEAP2_BINSIZE17        (0x00000600)
#define MEMHEAP2_BINSIZE18        (0x00000800)
#define MEMHEAP2_BINSIZE19        (0x00000C00)
#define MEMHEAP2_BINSIZE20        (0x00001000)
#define MEMHEAP2_BINSIZE21        (0x00001800)
#define MEMHEAP2_BINSIZE22        (0x00002000)
#define MEMHEAP2_BINSIZE23        (0x00003000)
#define MEMHEAP2_BINSIZE24        (0x00004000)
#define MEMHEAP2_BINSIZE25        (0x00006000)
#define MEMHEAP2_BINSIZE26        (0x00008000)
#define MEMHEAP2_BINSIZE27        (0x0000C000)
#define MEMHEAP2_BINSIZE28        (0x00010000)
#define MEMHEAP2_BINSIZE29        (0x00018000)
#define MEMHEAP2_BINSIZE30        (0x00020000)
#define MEMHEAP2_BINSIZE31        (0xFFFFFFFF)

typedef struct frd_Offset_info_type{
  unsigned long pad;
  unsigned long freeBlock_frdOff;
}frd_Offset_info;


#ifndef MEMHEAP_V_INFO_FREEBLOCKS
#define MEMHEAP_V_INFO_FREEBLOCKS  8
#endif

/* Structure used to log heap allocation failures */
typedef struct memheap_v_info_s
{
  struct mem_heap_struct *heap_ptr;
  size_t size;
  mem_heap_totals_type totals;
  unsigned int freeblocks[MEMHEAP_V_INFO_FREEBLOCKS];
} memheap_v_info_s_type;


/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

/* All the functions below are for the modem internal heap */

#ifdef FEATURE_SPLIT_MODEM_HEAP
void* modem_internal_mem_calloc(
  size_t               elt_count,  /*!< Number of elements to allocate */
  size_t               elt_size,   /*!< Size of each element*/
  modem_mem_client_e    client
  
);

void* modem_internal_mem_alloc(
  size_t               size,  /*!< Number of elements to allocate */
  modem_mem_client_e    client
  
);

void *modem_internal_mem_realloc(
  void                *ptr,        /*!< A block previously allocated 
                                        from heap */
  size_t               size,       /*!< New size (in bytes) of the ptr block
                                        of memory */
modem_mem_client_e    client                                        
);

void* modem_internal_mem_memalign_group(
  size_t           block_size,  /* alignment size for each element in the group */
  size_t           size,        /* requested memory size for each element       */
  int *            count        /* count of elements of 'size'                  */
);

/*API for freeing  heap to modem_internal heap */
void modem_internal_mem_free(
  void                *ptr,        /*!< Memory to free */
  modem_mem_client_e    client
);
#endif /*FEATURE_SPLIT_MODEM_HEAP */
#endif /* MEMHEAP_V_H */

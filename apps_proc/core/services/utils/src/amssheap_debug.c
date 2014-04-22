/*==========================================================================

                         AMSS Heap Debug Module

DESCRIPTION
  This file contains the AMSS heap debug functions and associated data.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The memdebug_init() function must be called before any other function
  in this module.

Copyright (c) 2006-12,2016  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/core.tx/6.0/services/utils/src/amssheap_debug.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

 when       who    what, where, why
 --------   -----  ----------------------------------------------------------
 11/18/16   nk     Fixed KW Warnings
 09/21/12   mg     fix for CR398914
 07/14/12   rks    fix for CR368628
 08/29/06   jc     Initial release.

============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "customer.h"
#include "memheap.h"
#include "qurt.h"
#include "amssassert.h"
#ifdef FEATURE_MEM_DEBUG

/*============================================================================
*                         CONSTANT DEFINITIONS
============================================================================*/

/* Code to enter and exit critical sections */
#ifdef FEATURE_MEMHEAP_MT
#define BEGIN_CRITICAL_SECTION(heap) (heap)->lock_fnc_ptr(heap)
#define END_CRITICAL_SECTION(heap)   (heap)->free_fnc_ptr(heap)
#else
#define BEGIN_CRITICAL_SECTION(heap)
#define END_CRITICAL_SECTION(heap)
#endif



/* Get pointer to next mem_heap header */
#define NEXT_HEADER(header) \
  ((void *)((char *)(header) + (header)->forw_offset))

#ifndef MEMDEBUG_DB_SIZE_SHFT
#define MEMDEBUG_DB_SIZE_SHFT   14
#endif
#define MEMDEBUG_DB_SIZE        (1 << MEMDEBUG_DB_SIZE_SHFT)
#define MEMDEBUG_DB_SIZE_MASK   (MEMDEBUG_DB_SIZE - 1)

uint16 debug_free_index[MEMDEBUG_DB_SIZE];
uint16  debug_free_index_cnt=0;
/*============================================================================
                        DATA DECLARATIONS
============================================================================*/

typedef struct
{
  int verified;
/*   int freed; */
  void *caller_ptr;
  qurt_thread_t tid;
  size_t size;
  void *ptr;
} memdebug_db_s_type;


volatile static  uint16 enable_tcb_count=0;

/*============================================================================
                          GLOBAL VARIABLES
============================================================================*/

/* All local module data */
typedef struct
{
  uint32 heap_unverified;
  uint32 db_unverified;
  uint32 free_block_max_size;
  int db_index;
  memdebug_db_s_type db[MEMDEBUG_DB_SIZE];
} memdebug_s_type; 

static memdebug_s_type memdebug;

/* The AMSS heap control struct */
extern mem_heap_type amss_mem_heap;

/*============================================================================
                          LOCAL VARIABLES
============================================================================*/

/*============================================================================
                          MACRO DEFINITIONS
============================================================================*/

/*============================================================================
                          FORWARD DECLARATIONS
============================================================================*/
/*===========================================================================

FUNCTION memdebug_init

DESCRIPTION
  This function initializes local data for the memory debug module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void memdebug_init(void)
{
  int i;
  /* memset(&memdebug, 0, sizeof(memdebug)); */
  for(i=0;i< MEMDEBUG_DB_SIZE;i++)
  {
      debug_free_index[i]=i;
  }
	return;
}

/*===========================================================================

FUNCTION memdebug_malloc

DESCRIPTION
  This function logs a memory block allocation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void memdebug_malloc
(
  size_t size,      /* Size of allocation request (bytes) */
  void *caller_ptr, /* Pointer to caller */
  void *ptr         /* Pointer to allocated memory block */
)
{
  mem_block_header_type *theBlock;
  uint32           db_index_local;

  if (ptr == NULL)
  {
    return;
  }

#ifdef FEATURE_QDSP6
  if(size <= 4)
  {
    theBlock = (mem_block_header_type *)
                            ((char *) ptr - sizeof(mem_block_header_type)+ 4);
  }else
  {
#endif
    theBlock = (mem_block_header_type *)
                            ((char *) ptr - sizeof(mem_block_header_type));
#ifdef FEATURE_QDSP6
  }
#endif

  
  //assign the new index
  db_index_local  =debug_free_index[debug_free_index_cnt];
  debug_free_index_cnt = (debug_free_index_cnt+1) % MEMDEBUG_DB_SIZE;
  memdebug.db_index=db_index_local;
  theBlock->header_debug_info.debug_index=db_index_local;
  if(enable_tcb_count < 15)
  {
    enable_tcb_count++;
  }else
  {
    memdebug.db[db_index_local].tid = qurt_thread_get_id();
  }

  /* Add the entry */
  memdebug.db[db_index_local].ptr = ptr;
  memdebug.db[db_index_local].caller_ptr = caller_ptr;
  
  memdebug.db[db_index_local].size = size;
}

/*===========================================================================

FUNCTION memdebug_malloc_updateparams

DESCRIPTION
  This function updates the information about a memory block allocation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void memdebug_malloc_updateparams
(
  size_t size,      /* Size of allocation request (bytes) */
  void *caller_ptr, /* Pointer to caller */
  void *ptr         /* Pointer to allocated memory block */
)
{
  mem_block_header_type *theBlock;
  uint32           db_index_local;

  if (ptr == NULL)
  {
    return;
  }

#ifdef FEATURE_QDSP6
  if(size <= 4)
  {
    theBlock = (mem_block_header_type *)
                            ((char *) ptr - sizeof(mem_block_header_type)+ 4);
  }else
  {
#endif
    theBlock = (mem_block_header_type *)
                          ((char *) ptr - sizeof(mem_block_header_type));
#ifdef FEATURE_QDSP6
  }
#endif

  
  //assign the new index
  db_index_local  = theBlock->header_debug_info.debug_index;
  if(enable_tcb_count < 15)
  {
    enable_tcb_count++;
  }else
  {
    memdebug.db[db_index_local].tid = qurt_thread_get_id();
  }
  


  /* Add the entry */
  memdebug.db[db_index_local].ptr = ptr;
  memdebug.db[db_index_local].caller_ptr = caller_ptr;
  
  memdebug.db[db_index_local].size = size;
 
}



void memdebug_updatecallerptr(void *ptr,void *caller_ptr,size_t size)
{
  int db_index=-1;
  mem_block_header_type *theBlock;

  if (ptr == NULL)
  {
    return;
  }

#ifdef FEATURE_QDSP6
  if(size <= 4)
  {
    theBlock = (mem_block_header_type *)
                            ((char *) ptr - sizeof(mem_block_header_type)+ 4);
  }else
  {
#endif
    theBlock = (mem_block_header_type *)
                          ((char *) ptr - sizeof(mem_block_header_type));
#ifdef FEATURE_QDSP6
  }
#endif
  db_index=theBlock->header_debug_info.debug_index;

  if(( db_index >= 0) && (db_index < MEMDEBUG_DB_SIZE_MASK) && (memdebug.db[db_index].ptr == ptr))
  {
      memdebug.db[db_index].caller_ptr = caller_ptr;
  }
}
/*===========================================================================

FUNCTION memdebug_free

DESCRIPTION
  This function logs a memory block free.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void memdebug_free
(
  void *ptr,         /* Pointer to allocated memory block */
  uint32 db_index
)
{
  if((db_index < MEMDEBUG_DB_SIZE_MASK) && (memdebug.db[db_index].ptr == ptr))
  {
    /* Clear the entry */
    memset(&memdebug.db[db_index], 0, sizeof(memdebug.db[db_index]));
    if((debug_free_index_cnt-1) >= 0)
      {
        debug_free_index[debug_free_index_cnt-1]=db_index;
        debug_free_index_cnt--;
      }
  }
}

/*===========================================================================

FUNCTION memdebug_failure_handler

DESCRIPTION
  This function checks the heap for consistency.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void memdebug_failure_handler(void)
{
  int index_read;
  int index_write;
  mem_heap_type *heap_ptr;
  mem_block_header_type *header;
  mem_block_header_type *end_ptr;
  int block_count;
  unsigned long free_size;
  void *user_ptr;

  /* Compact the database */
  index_write = 0;
  for (index_read = 0; index_read < MEMDEBUG_DB_SIZE; index_read++)
  {
    if (memdebug.db[index_read].ptr == NULL)
    {
      continue;   /* Skip unused entry */
    }

    if (index_write == index_read)
    {
      /* No need to move this entry */
      index_write++;
      continue;
    }

    /* Move the entry */
    memdebug.db[index_write] = memdebug.db[index_read];
    memdebug.db[index_write].verified = FALSE;
    memset(&memdebug.db[index_read], 0, sizeof(memdebug.db[index_read]));
    index_write++;
  }
  memdebug.db_index = index_write - 1;

  /* Verify that the database entries agree with the actual heap allocations */
  heap_ptr = &amss_mem_heap;
  BEGIN_CRITICAL_SECTION(heap_ptr);
  header = heap_ptr->first_block;
  end_ptr = (void *)((char *)header + heap_ptr->total_bytes);
  memdebug.free_block_max_size = 0;
  free_size = 0;
  for (block_count = heap_ptr->total_blocks;
       block_count > 0;
       block_count--, header = NEXT_HEADER(header))
  {
    /* Check that block is within the limits of the heap */
    ASSERT(header >= heap_ptr->first_block);
    ASSERT(header < end_ptr);

    /* Verify the block */
    //ASSERT(header->pad1 == MEMHEAP_PAD_CHAR);
    ASSERT(header->free_flag == (char)kBlockFree ||
      header->free_flag == (char)kBlockUsed);
    ASSERT(header->last_flag == 0 || header->last_flag == (char)kLastBlock);
    ASSERT(header->forw_offset >= sizeof(mem_block_header_type));
    ASSERT(header->extra < kMinChunkSize);

    /* Check that only last block is marked so */
    if (block_count > 1)
    {
      ASSERT(!header->last_flag);
    }
    else
    {
      ASSERT(header->last_flag);
    }

    /* Is this block allocated or free? */
    if (header->free_flag == kBlockUsed)
    {
      /* Match block to database entry */
      #ifdef FEATURE_QDSP6
      if(header->forw_offset == 16)
      {
        user_ptr = ((char *) header + sizeof(mem_block_header_type)- 4);
      }else
      {
      #endif
        user_ptr = header + 1;
      #ifdef FEATURE_QDSP6
      }
      #endif
      for (index_read = 0; index_read < MEMDEBUG_DB_SIZE; index_read++)
      {
        if (memdebug.db[index_read].ptr == user_ptr)
        {
          /* Found matching entry */
          memdebug.db[index_read].verified = TRUE;
          break;
        }
      }
      if (index_read >= MEMDEBUG_DB_SIZE )
      {
        /* No matching database entry was found */
        memdebug.heap_unverified++;
      }

      /* This allocated block may have closed a set of free blocks.
       * Check the accumulated free block total size.
       */
      if (free_size > memdebug.free_block_max_size)
      {
        memdebug.free_block_max_size = free_size;
        free_size = 0;
      }
    }
    else
    {
      /* This block is free. Add its size to the accumulated free block total */
      free_size += mem_get_block_logical_size(header,heap_ptr);
    }
  }
  //end block can be the last blcok and free block
  if (free_size > memdebug.free_block_max_size)
  {
    memdebug.free_block_max_size = free_size;
  }
  END_CRITICAL_SECTION(heap_ptr);

  /* Count the number of unverified database entries */
  memdebug.db_unverified = 0;
  for (index_read = 0; index_read < MEMDEBUG_DB_SIZE; index_read++)
  {
    if (memdebug.db[index_read].ptr == NULL)
    {
      continue;   /* Skip unused entry */
    }

    if (!memdebug.db[index_read].verified)
    {
      memdebug.db_unverified++;
    }
  }
  ASSERT(memdebug.db_unverified == 0);
  ASSERT(memdebug.heap_unverified == 0);
}

#endif  /* FEATURE_AMSSHEAP_DEBUG */

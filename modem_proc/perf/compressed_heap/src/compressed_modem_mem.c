/*!
  @file
  compressed_modem_mem.c

  @brief
  Implementation of a heap manager to manage memory allocations
  and deallocations using a Best-Fit strategy.

  For any clarification, please send an email to 
  modem.memory.hotline@qti.qualcomm.com

  This is the implementation for FR - 29357

*/

/*===========================================================================

  Copyright (c) 2009-2015 Qualcomm Technologies Incorporated. All Rights Reserved

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
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/perf.mpss/2.1.2.0/compressed_heap/src/compressed_modem_mem.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/18/15   skp      Initial Version (adapted from modem_mem.c)

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>                      /* Definition for basic types and macros        */
#include <compressed_modem_mem.h>        /* External interface to compressed_modem_mem.c */
#include <memheap.h>
#include "err.h"

/* Ideally, following ifdef is not required. This FEATURE_COMPRESSED_HEAP is defined
   in target.builds file. Irrespective of that i.e. whether defined or not, following
   implementation in this file should work seamlessly. The FEATURE_COMPRESSED_HEAP
   is mainly for other modules using the services of these compressed heap APIs.
   
   But, to keep the expectations consistent and avoid any surprises due to mismatch in 
   feature flag defines in target.builds file, including whole implementation under
   FEATURE_COMPRESSED_HEAP feature flag.
   
*/
#ifdef FEATURE_COMPRESSED_HEAP

#define FEATURE_COMPRESSED_MEM_DEBUG_COMMON


/* Following defines are excluded intentionally for now in the first draft.
   Just --- not to forget which were the feature flags in future, listing
   these here. Keep following commented until enabled later after due
   profiling.
*/
/*
#define COMPRESSED_FEATURE_ENABLE_HEAP_PROFILING
#define FEATURE_COMPRESSED_MEM_DEBUG_ARRAY
#define COMPRESSED_MEMDEBUG_DB_SIZE 1024
*/

mem_heap_type compressed_mem_heap;

/* COMPRESSED_MODEM_MEM_HEAP_SIZE used in following variable definition 
   is defined in target.builds file. This gives flexibility to alter the
   size without actually modifying this source code.
*/

static unsigned int            is_compressed_heap_initialized   = 0;
static volatile unsigned int   compressed_modem_mem_heap_size   = COMPRESSED_MODEM_MEM_HEAP_SIZE;

#ifdef FEATURE_COMPRESSED_MEM_DEBUG_ARRAY

static memdebug_s_type memdebug_compressed_modem_heap;
uint16 free_index_debug_compressed_modem_heap[COMPRESSED_MEMDEBUG_DB_SIZE];
#endif

#ifdef FEATURE_COMPRESSED_MEM_DEBUG_COMMON
#ifndef COMPRESSED_MODEM_HEAP_CALLER_LEVEL
#define COMPRESSED_MODEM_HEAP_CALLER_LEVEL     (0)
#endif

#if defined(FEATURE_QDSP6)
 #define  COMPRESSED_MEM_HEAP_CALLER_ADDRESS() ((void *)__builtin_return_address(COMPRESSED_MODEM_HEAP_CALLER_LEVEL));
#else
 #define COMPRESSED_MEM_HEAP_CALLER_ADDRESS() ((void *) __return_address())
#endif

#endif

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief
** Contains the information for a memory section input configuration
*/
typedef struct _compressed_modem_mem_section_cfg
{
   byte                 *start;       /*!< start address of section */
   uint32                size;        /*!< size of the section */
} compressed_modem_mem_section_cfg;

#ifdef FEATURE_COMPRESSED_MEM_DEBUG_COMMON
/* memory debug functions */
extern void memdebug_init(mem_heap_type *heap_ptr);
extern void memdebug_updatecallerptr(mem_heap_type *heap_ptr,void *ptr,void *caller_ptr, size_t size);
extern uint32 memheap_global_error;
memheap_task_stats_type compressed_modem_heap_task_stats[MEMHEAP_MAX_THREADS];

#endif  /* FEATURE_COMPRESSED_MEM_DEBUG_COMMON */

#ifdef COMPRESSED_FEATURE_ENABLE_HEAP_PROFILING
memheap_profile_table_type compressed_memheap_profile_table[50];

static uint32 max_malloc_time_ch;
static uint32 max_free_time_ch;
static uint32 max_realloc_time_ch;
static uint32 max_calloc_time_ch;

#endif

extern void mem_heap_add_section(
     mem_heap_type *heap_ptr, 
     void *section_start, 
     unsigned long section_size);

extern uint32 timetick_get_safe();

/*===========================================================================

  FUNCTION:  compressed_modem_mem_validate_sections

===========================================================================*/
/*!
    @brief
    Validates and arranges the memory sections available for the Modem Heap

    @return
    None
*/
/*=========================================================================*/
void compressed_modem_mem_validate_sections(
  compressed_modem_mem_section_cfg mem_sections[]   /*!< array of available memory
                                              sections */
)
{
  /*! @brief temporary buffer for modem heap section entry
  */
  compressed_modem_mem_section_cfg t_section;
  /*! @brief for loop index
  */
  int i, j;

  /*-----------------------------------------------------------------------*/

  for (i = 0; mem_sections[i].start != 0x0; ++i) 
  {

    for (j = i + 1; mem_sections[j].start != 0x0; ++j) 
    {
      if (mem_sections[i].start > mem_sections[j].start) 
      {
        t_section = mem_sections[i];
        mem_sections[i] = mem_sections[j];
        mem_sections[j] = t_section;
      }
    }
  }
  return;

} /* compressed_modem_mem_validate_sections() */

/*===========================================================================

  FUNCTION:  compressed_modem_mem_init_heap

===========================================================================*/
/*!
    @brief
    Initializes a heap using given memory sections.

    @detail
    Initializes a heap using the memory sections given.

    @return
    None

    @note
    The modem heap clients should NOT call this function; this funcion is 
    called only once during the system initialization.
	
	IMPORTANT: Do not change this implementation (array based). Otherwise, entire feature
	will collapse. Please refer the documentation on Compressed Heap or contact
	Modem Memory Team (modem.memory.hotline@qti.qualcomm.com) for further details.
*/
/*=========================================================================*/
static void compressed_modem_mem_init_heap(void)
{
  /*! @brief 
  **  Modem Central heap buffer area
  */
  static byte compressed_heap_buffer[COMPRESSED_MODEM_MEM_HEAP_SIZE];
  /*! @brief array of free memory sections
  */
  compressed_modem_mem_section_cfg compressed_sections[] = 
  {
  /* It's important to keep T_WINNT checking here; on QTF builds the target
   * specific modem heap configuration should NOT be used 
   */
  
  #ifndef T_WINNT
    #ifdef MODEM_MEM_SECTION_BASE0
    { (byte *) MODEM_MEM_SECTION_BASE0, MODEM_MEM_SECTION_SIZE0 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE1
    { (byte *) MODEM_MEM_SECTION_BASE1, MODEM_MEM_SECTION_SIZE1 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE2
    { (byte *) MODEM_MEM_SECTION_BASE2, MODEM_MEM_SECTION_SIZE2 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE3
    { (byte *) MODEM_MEM_SECTION_BASE3, MODEM_MEM_SECTION_SIZE3 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE4
    { (byte *) MODEM_MEM_SECTION_BASE4, MODEM_MEM_SECTION_SIZE4 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE5
    { (byte *) MODEM_MEM_SECTION_BASE5, MODEM_MEM_SECTION_SIZE5 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE6
    { (byte *) MODEM_MEM_SECTION_BASE6, MODEM_MEM_SECTION_SIZE6 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE7
    { (byte *) MODEM_MEM_SECTION_BASE7, MODEM_MEM_SECTION_SIZE7 },
    #endif
  #endif */ /* T_WINNT */
    { compressed_heap_buffer, sizeof(compressed_heap_buffer)    }, 
    { NULL,                    0                                }
  };
  
  compressed_modem_mem_heap_size =  COMPRESSED_MODEM_MEM_HEAP_SIZE;
    
  memset(&compressed_mem_heap, 0, sizeof(mem_heap_type));
  
  compressed_modem_mem_validate_sections(compressed_sections);
  
  //Keep following commented. Will be useful for debugging later. It can be enabled and built again.
  //ERR("Address of section start = %x, starting of arr = %x", compressed_sections[0].start, compressed_heap_buffer, 0);
  
  mem_init_heap(&compressed_mem_heap,compressed_heap_buffer,sizeof(compressed_heap_buffer) ,NULL); 
  
  for (int j = 1; compressed_sections[j].start != NULL; ++j) 
  {
    if(j<MEM_HEAP_SECTIONS_MAX)
    {
      mem_heap_add_section(&compressed_mem_heap, compressed_sections[j].start, compressed_sections[j].size); 
    }
  }
  
  //Keep following commented. Later if required - this will be helpful to just enable and introduce crash here.
  //ERR_FATAL("Address of section start = %x, starting of arr = %x", compressed_sections[0].start, compressed_heap_buffer, 0);
  
  #ifdef FEATURE_COMPRESSED_MEM_DEBUG_ARRAY
  /* Init AMSS memory debug module */
  compressed_mem_heap.debug_array              = &memdebug_compressed_modem_heap;
  compressed_mem_heap.free_index_array         = (uint16 *)&free_index_debug_compressed_modem_heap;
  compressed_mem_heap.free_index_array_index   = 0;
  memdebug_init(&compressed_mem_heap);
#endif

#ifdef FEATURE_COMPRESSED_MEM_DEBUG_COMMON
   if(MEM_TLS_CREATE_ERR != memheap_global_error)
   {
      compressed_mem_heap.heapStatistics = &compressed_modem_heap_task_stats[0];
   }
   else
   {
      //Must crash here.
      ERR_FATAL( "Compressed Heap - Stat Assertion : memheap_global_error = %d", memheap_global_error, 0, 0 ); 
   }
#endif

  return;
} /* compressed_modem_mem_init_heap() */


/*===========================================================================

  FUNCTION:  compressed_modem_mem_check_heap_initialized

===========================================================================*/
/*!
    @brief
    Checks to see if the heap is initialized. If not, this function 
    initializes the heap.

    @return
    None
*/
/*=========================================================================*/
static void compressed_modem_mem_check_heap_initialized(void)
{
  /* Immediately return if the heap is initialied; except for the very 
  ** first function call, it will return from here
  */
  if (is_compressed_heap_initialized) 
  {
    return;
  }

  
  if (!(is_compressed_heap_initialized))
  {
	is_compressed_heap_initialized=1;
    compressed_modem_mem_init_heap();    
  }
  return;
} /* compressed_modem_mem_check_heap_initialized() */


/*===========================================================================

  FUNCTION:  modem_mem_alloc_ch

===========================================================================*/
/*!
    @brief
    Allocates a block of size bytes from the heap.

    @return
    Returns a pointer to the newly allocated block, or 
    NULL if the block could not be allocated.
*/
/*=========================================================================*/
void* modem_mem_alloc_ch(
  size_t                           size,       /*!< Number of bytes to allocate */
  compressed_modem_mem_client_e    client      /*!< Client ID for this mem block */
)
{
  void *ptr = NULL;

  /*-----------------------------------------------------------------------*/

  if (!size) 
  {
    return NULL;  /* Zero size request - return NULL immediately */
  }

  /*-----------------------------------------------------------------------*/

  compressed_modem_mem_check_heap_initialized();

  #ifdef COMPRESSED_FEATURE_ENABLE_HEAP_PROFILING
    uint32 malloc_inTime  = 0, malloc_outTime = 0, diff = 0;
    malloc_inTime             = timetick_get_safe();
  #endif


  ptr = mem_malloc(&compressed_mem_heap,size);
  
  #ifdef COMPRESSED_FEATURE_ENABLE_HEAP_PROFILING
	
    malloc_outTime = timetick_get_safe();
    diff           = malloc_outTime - malloc_inTime;
	
    compressed_memheap_profile_table[compressed_mem_heap.debug_heap_index].malloc_avg += diff;
    compressed_memheap_profile_table[compressed_mem_heap.debug_heap_index].heap_ptr    = &compressed_mem_heap;
    compressed_memheap_profile_table[compressed_mem_heap.debug_heap_index].malloc_cnt++;
	
    if(max_malloc_time_ch < diff) 
    {
       max_malloc_time_ch=diff;
    }
	
  #endif /*COMPRESSED_FEATURE_ENABLE_HEAP_PROFILING*/

  #ifdef FEATURE_COMPRESSED_MEM_DEBUG_COMMON
  {
    void *caller_ptr=COMPRESSED_MEM_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(&compressed_mem_heap,ptr, caller_ptr,size);
  }
  #endif

  return ptr;
 
} /* modem_mem_alloc_ch() */

void* modem_mem_alloc_ch_setcaller(
  size_t                         size,           /*!< Number of bytes to allocate               */
  compressed_modem_mem_client_e  client,         /*!< Client ID for this mem block              */
  void*                          caller_ptr      /*!< Caller Ptr to be set in debug information */
)
{
  void *ptr = modem_mem_alloc_ch(size,client);
#ifdef FEATURE_COMPRESSED_MEM_DEBUG_COMMON
  memdebug_updatecallerptr(&compressed_mem_heap,ptr, caller_ptr,size);
#endif
  return ptr;
}

/*===========================================================================

  FUNCTION:  modem_mem_calloc_ch

===========================================================================*/
/*!
    @brief
    Allocates enough space for elt_count elements each of elt_size bytes
    from the heap and initializes the space to nul bytes.

    @return
    Returns a pointer to the newly allocated block, or NULL if the block
    could not be allocated. If elt_count or elt_size is 0, the NULL pointer 
    will be silently returned.
*/
/*=========================================================================*/

void* modem_mem_calloc_ch(
  size_t                          elt_count,  /*!< Number of elements to allocate */
  size_t                          elt_size,   /*!< Size of each element           */
  compressed_modem_mem_client_e   client      /*!< Client ID for this mem block   */
)
{
  void *ptr=NULL;
  compressed_modem_mem_check_heap_initialized();
  ptr=mem_calloc(&compressed_mem_heap,elt_count, elt_size);

  #ifdef FEATURE_COMPRESSED_MEM_DEBUG_COMMON
  {
    void *caller_ptr=COMPRESSED_MEM_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(&compressed_mem_heap,ptr, caller_ptr,elt_count * elt_size);
  }
  #endif /* FEATURE_COMPRESSED_MEM_DEBUG_COMMON */

  return ptr;
} /* modem_mem_calloc_ch() */


void* modem_mem_calloc_ch_setcaller(
  size_t                                                   elt_count,  /*!< Number of elements to allocate */
  size_t                                                   elt_size,     /*!< Size of each element*/
  compressed_modem_mem_client_e  client,        /*!< Client ID for this mem block */
  void*                                                    caller_ptr   /*!< Caller Ptr to be set in debug information */
)
{
  void *ptr = NULL;
  ptr       = modem_mem_calloc_ch(elt_count,elt_size,client);

#ifdef FEATURE_COMPRESSED_MEM_DEBUG_COMMON
  memdebug_updatecallerptr(&compressed_mem_heap,ptr, caller_ptr,elt_count * elt_size);
#endif

  return ptr;
}
/*===========================================================================

  FUNCTION:  modem_mem_realloc_ch

===========================================================================*/
/*!
    @brief
    Resizes the ptr block of memory to be size bytes while preserving the
    block's contents.

    @detail
    Resizes the ptr block of memory to be size bytes while preserving the
    block's contents.  If the block is shortened, bytes are discarded off the
    end.  If the block is lengthened, the new bytes added are not initialized
    and will have garbage values.  
    If ptr is NULL, the function behaves exactly like modem_mem_alloc_ch 
    (see above). If ptr is not NULL and size is 0, the function behaves
    exactly like modem_mem_free_ch (see below).  
    If the block cannot be resized, ptr is not NULL and size is not 0, then 
    NULL is returned and the original block is left untouched.  If the ptr 
    block IS successfully resized and the returned value is different from 
    the ptr value passed in, the old block passed in must be considered 
    deallocated and no longer useable (i.e. do NOT try to mem_free it!) 
    This function will call modem_mem_alloc_ch if it can't grow the block in 
    place.

    @return
    Returns a pointer to the beginning of the resized block of memory (which
    may be different than ptr) or NULL if the block cannot be resized.
*/
/*=========================================================================*/

void *modem_mem_realloc_ch(
  void*                           ptr,        /*!< A block previously allocated from heap         */
  size_t                          size,       /*!< New size (in bytes) of the ptr block of memory */
  compressed_modem_mem_client_e   client      /*!< Client ID for this mem block                   */
)
{
  void *ptr_ret=NULL;
  compressed_modem_mem_check_heap_initialized();
  
  #ifdef COMPRESSED_FEATURE_ENABLE_HEAP_PROFILING
    uint32 realloc_inTime=0 , realloc_outTime=0, diff=0;
    realloc_inTime = timetick_get_safe();
  #endif

  ptr_ret=mem_realloc(&compressed_mem_heap, ptr,size);
   
  #ifdef COMPRESSED_FEATURE_ENABLE_HEAP_PROFILING
    realloc_outTime = timetick_get_safe( );
    diff            = realloc_outTime - realloc_inTime;

    compressed_memheap_profile_table[compressed_mem_heap.debug_heap_index].realloc_avg  = diff;
	compressed_memheap_profile_table[compressed_mem_heap.debug_heap_index].heap_ptr     = &compressed_mem_heap;
    compressed_memheap_profile_table[compressed_mem_heap.debug_heap_index].realloc_cnt++;
    if(max_realloc_time_ch < diff) 
	{
		max_realloc_time_ch = diff;
	}
  #endif

#ifdef FEATURE_COMPRESSED_MEM_DEBUG_COMMON
    {
      void *caller_ptr=COMPRESSED_MEM_HEAP_CALLER_ADDRESS();
      memdebug_updatecallerptr(&compressed_mem_heap,ptr_ret, caller_ptr,size);
    }
  #endif
  return ptr_ret;
} /* modem_mem_realloc_ch() */

void* modem_mem_realloc_setcaller_ch(
  void*                           ptr,            /*!< A block previously allocated from heap         */
  size_t                          size,           /*!< New size (in bytes) of the ptr block of memory */
  compressed_modem_mem_client_e   client,         /*!< Client ID for this mem block                   */
  void*                           caller_ptr      /*!< Caller Ptr to be set in debug information      */
)
{
  void *ptr_ret=NULL;
  ptr_ret=modem_mem_realloc_ch(ptr,size,client);

#ifdef FEATURE_COMPRESSED_MEM_DEBUG_COMMON
  memdebug_updatecallerptr(&compressed_mem_heap,ptr_ret, caller_ptr,size);
#endif

  return ptr_ret;
}

/*===========================================================================

  FUNCTION:  modem_mem_free_ch

===========================================================================*/
/*!
    @brief
    Deallocates the ptr block of memory. 

    @detail
    Deallocates the ptr block of memory.  If ptr is NULL, or ptr is outside 
    the range of memory managed by heap, then this function call does nothing
    (and is guaranteed to be harmless).

    @return
    None.
*/
/*=========================================================================*/
void modem_mem_free_ch(
  void*                           ptr,        /*!< Memory to free             */
  compressed_modem_mem_client_e   client      /*!< User ID for this mem block */
)
{
#ifdef COMPRESSED_FEATURE_ENABLE_HEAP_PROFILING
    uint32 free_inTime=0, free_outTime=0, diff=0;
    free_inTime = timetick_get_safe();
  #endif
  
/** If the FEATURE_MEM_DEBUG_ARRAY is NOT defined then only following is required.
 *  Otherwise, memset is taken care by memdebug_free (defined in amssheap_debug.c file)
 */
#ifndef FEATURE_MEM_DEBUG_ARRAY
	if(NULL != ptr) {

		uint32 extra           = 0;
		uint32 size            = 0;
		uint32 sizeBlockHeader = sizeof(mem_block_header_type);
		
		mem_block_header_type *theBlock;

		//We need to just memset the actual memory that was allocated for usage. The header block info is not be memset.
		//Because this can be used by heap management algorithm.
		uint32 ptrCopy = (uint32)ptr;

		theBlock = (mem_block_header_type *)((char *) ptr - sizeBlockHeader);
		extra    = (uint32)theBlock->extra;
		
		//The actual size that was requested at the time of malloc related APIs is to be taken with following formula.
		size = theBlock->forw_offset - sizeBlockHeader - extra;
		
#endif //FEATURE_MEM_DEBUG_ARRAY


		mem_free(&compressed_mem_heap,ptr);


#ifndef FEATURE_MEM_DEBUG_ARRAY
	    //Now memset to zero. This will be useful for high RW compression ratio.
		memset((void*)ptrCopy, 0, size);
	 }
	//Else do nothing. This is for quick processing in compressed scenario and avoid crashes.
	
#endif //FEATURE_MEM_DEBUG_ARRAY

#ifdef COMPRESSED_FEATURE_ENABLE_HEAP_PROFILING
    free_outTime = timetick_get_safe();
    diff         = (free_outTime - free_inTime);
	
    compressed_memheap_profile_table[compressed_mem_heap.debug_heap_index].free_avg += diff;
	compressed_memheap_profile_table[compressed_mem_heap.debug_heap_index].heap_ptr = &compressed_mem_heap;
    compressed_memheap_profile_table[compressed_mem_heap.debug_heap_index].free_cnt++;
	
    if(max_free_time_ch < diff){
		max_free_time_ch = diff;
    }
	
  #endif 
} /* modem_mem_free_ch() */


/*===========================================================================

  FUNCTION:  compressed_modem_mem_add_region

===========================================================================*/
/*!
    @brief
    Add a memory region to the modem heap

    @detail
    This API can be used to add extra left over memory that can be used 
    in the modem heap. 

    @return
    None.
*/
/*=========================================================================*/

void compressed_modem_mem_add_region(void *section_start, unsigned long section_size)
{
   mem_heap_add_section(&compressed_mem_heap,section_start,section_size);
}

/*===========================================================================

  FUNCTION:  compressed_modem_mem_get_section_info

===========================================================================*/
/*!
    @brief
    Get the section information about modem heap

    @return
    None.
*/
/*=========================================================================*/

void compressed_modem_mem_get_section_info(void **section_info)
{
	*section_info=(void *)&compressed_mem_heap.sections[0];
}

#endif /* FEATURE_COMPRESSED_HEAP */


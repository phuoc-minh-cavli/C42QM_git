/*!
  @file
  modem_mem.c

  @brief
  Implementation of a heap manager to manage memory allocations
  and deallocations using a Best-Fit strategy.

*/

/*===========================================================================

  Copyright (c) 2009-2015,2017, 2019-2020, 2022 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/core.mpss/3.10/services/utils/src/modem_mem.c#8 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/08/19   kdey    Making amss and modem heap static to prevent physpool fragmentation
02/28/17   nk      Removed Random allocationfrom at bootup modemheap on low memory targets
09/20/15   ps      FR29424-CR911469- Memheap quota support
11/12/14   ps      Remove Debug Array
01/07/14   rks     CR491964:Add dynamic sections support in memheap
12/12/13   rks     CR586541:Add a symbol that stores the Modem mem heap size into the ELF,
                   for memory calculation purposes
02/05/13   ps      Add debug information to the block header
12/06/11   rs      Klocwork Errors removed 
12/01/11   rs      Minor change: Added ifdef FEATURE_QDSP6 :CR318841
10/11/11   ada     Fix size check, Add function modem_mem_FatalDump to dump headers in a dynamic buffer in case of crash
10/04/11   ch      Check if size is corrupted 
09/01/11   rs      Minor fix for compilation errors in MOBs.
08/10/11   rs      Added Safe Unlinking during unlinking of free block (CR298163) 
08/10/11   rs      Added fix for CR 276505 - Fix for detecting corrupt ClientID 
                   while accessing Client Memory Budget 
08/10/11   rs      Added fix for CR 276504 - Fix for potential INT overflow 
                   of extra field or old size of block during Realloc 
08/10/11   rs      Added fix for CR 276478 - Fix for potential INT overflow 
                   of block size 
08/10/11   rs      Added fix for CR 274236 - Check ClientID before updating 
                   Client Memory Usage	 
03/09/10   ic      Multi-section supports
06/07/09   ic      Added extra debugging supports - heap dumps and logging
04/24/09   ic      Added new fields - client & flags - in modem_mem_block_s
                   Removed two fields - last & free - in modem_mem_blocks_s,
                   and moved them to the new field
03/16/09   ic      Initial Version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>           /* Definition for basic types and macros */
#include "modem_mem.h"        /* External interface to modem_mem.c */
#include "memheap.h"
#include "memheap_v.h"
#include <string.h>
#include <stringl/stringl.h>
#include <atomic_ops.h>
#include <qurt.h>


#ifndef USES_LTE_CATM
#include "PrngML.h"
#endif


#ifdef FEATURE_MEM_DEBUG
#include "rcinit.h"
#endif
mem_heap_type modem_mem_heap;
static unsigned int is_heap_initialized=0;

extern void  QURTK_handle_pool_stack_return(void);
uint8* glboal_autostack_ptr = (uint8*)&QURTK_handle_pool_stack_return;

#ifndef USES_LTE_CATM
//static void *random_alloc_ptr=NULL;
#define MODEM_MEM_RANDOM_ALLOC_MASK 0x1FFFF
#endif

#ifdef FEATURE_MEM_DEBUG
#ifndef MODEM_HEAP_CALLER_LEVEL
#define MODEM_HEAP_CALLER_LEVEL     (0)
#endif

#ifdef FEATURE_SPLIT_MODEM_HEAP
  extern mem_heap_type modem_internal_mem_heap;

/*threshold for allocating memory from internal heap */
  #ifndef MODEM_INTERNAL_HEAP_ALLOC_THRESHOLD
    #define MODEM_INTERNAL_HEAP_ALLOC_THRESHOLD (75 * 1024)
  #endif

#endif
#if defined(FEATURE_QDSP6)
 #define  MEM_HEAP_CALLER_ADDRESS() ((void *)__builtin_return_address(MODEM_HEAP_CALLER_LEVEL));
 #define  MEM_HEAP_CALLER_ADDRESS_EXT() ((void *)__builtin_return_address(MODEM_HEAP_CALLER_LEVEL+1));
 #define  MEM_HEAP_CALLER_ADDRESS_EXT2() ((void *)__builtin_return_address(MODEM_HEAP_CALLER_LEVEL+2));
#else
 #define MEM_HEAP_CALLER_ADDRESS() ((void *) __return_address())
#endif

#endif

/* Heap allocation failure constants */
#ifndef MODEMHEAP_FAIL_ARRAY_SIZE
#define MODEMHEAP_FAIL_ARRAY_SIZE  2
#endif

int modemheap_fail_index = 0;

/* Array used to log heap allocation failures */
memheap_v_info_s_type modemheap_fail_info[MODEMHEAP_FAIL_ARRAY_SIZE];
#ifdef FEATURE_MEM_DEBUG
  static uint32 modem_mem_get_task_quota(void);
#endif

#ifndef MODEM_MEM_HEAP_SIZE
    #define MODEM_MEM_HEAP_SIZE 0x140000
#endif /* MODEM_MEM_HEAP_SIZE */

#ifdef FEATURE_STATIC_MODEM_HEAP
  #define MODEMHEAP_STATIC_BUFFER_ATTR __attribute__((section(".bss.modemheap"),aligned (64))) 
  static uint8 modem_heap_static_buffer[MODEM_MEM_HEAP_SIZE] MODEMHEAP_STATIC_BUFFER_ATTR;
#endif /* FEATURE_STATIC_MODEM_HEAP */

static volatile unsigned int modem_mem_heap_size = MODEM_MEM_HEAP_SIZE;

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief
** Contains the information for a memory section input configuration
*/
typedef struct _modem_mem_section_cfg
{
   byte                 *start;       /*!< start address of section */
   uint32                size;        /*!< size of the section */
} modem_mem_section_cfg;


#ifdef FEATURE_MEM_DEBUG
/* AMSS memory debug functions */
extern void memdebug_updatecallerptr(mem_heap_type *heap_ptr,void *ptr,void *caller_ptr, size_t size);
memheap_task_stats_type modem_heap_task_stats[MEMHEAP_MAX_THREADS];
extern uint32 memheap_global_error;
#endif  /* FEATURE_MEM_DEBUG */

#ifdef FEATURE_ENABLE_HEAP_PROFILING
extern memheap_profile_table_type memheap_profile_table[50];
#endif
extern void mem_heap_add_section(
     mem_heap_type *heap_ptr, 
     void *section_start, 
     unsigned long section_size);

extern void mem_init_heap_alloc(
   mem_heap_type                 *heap_ptr,
   unsigned long                  heap_mem_size,
   mem_allocator_failed_proc_type fail_fnc_ptr
);


void modemheap_malloc_failed
(
  struct mem_heap_struct *heap_ptr, /* Heap on which allocation is attempted */
  size_t size                       /* Size of failed request */
);

#ifdef FEATURE_SPLIT_MODEM_HEAP

/*===========================================================================

  FUNCTION:  alloc_from_modem_internal

===========================================================================*/
/*!
    @brief
    Checks the condition for heap allocation from internal heap 

    @return
    True -  if heap allocation needs to be from internal heap
    False - if heap allocation needs to be done from modem heap
*/
/*=========================================================================*/
static boolean alloc_from_modem_internal(size_t size)
{

  if(size <=  MODEM_INTERNAL_HEAP_ALLOC_THRESHOLD)
    return TRUE;
  else
    return FALSE;
}


/*===========================================================================

  FUNCTION:  modem_internal_heap_addr_range

===========================================================================*/
/*!
    @brief
    This function checks if the passed heap pointer is within internal heap address range 

    @return
    True -  If heap pointer is in internal heap range 
    False - If heap pointer not in internal heap range 
*/
/*=========================================================================*/

static boolean modem_internal_heap_addr_range(void *heap_ptr)
{
  uint16 section_number;
  mem_block_header_type *theBlock;	
  
  if(heap_ptr == NULL)
    return FALSE;

  theBlock = (mem_block_header_type *)((char *) heap_ptr - sizeof(mem_block_header_type));
  
  section_number = theBlock->section_index;                              
  
  if(section_number >= modem_internal_mem_heap.number_of_sections)
    return FALSE;
   
  if((BOUNDARY_CHECK_SECTIONS(theBlock, &modem_internal_mem_heap)))
    return TRUE;
  else
    return FALSE;
}
#endif /*FEATURE_SPLIT_MODEM_HEAP */
/*===========================================================================

  FUNCTION:  modem_mem_validate_sections

===========================================================================*/
/*!
    @brief
    Validates and arranges the memory sections available for the Modem Heap

    @return
    None
*/
/*=========================================================================*/
void modem_mem_validate_sections(
  modem_mem_section_cfg mem_sections[]   /*!< array of available memory
                                              sections */
)
{
  /*! @brief temporary buffer for modem heap section entry
  */
  modem_mem_section_cfg t_section;
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

} /* modem_mem_validate_sections() */

/*===========================================================================

  FUNCTION:  modem_mem_init_heap

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
*/
/*=========================================================================*/
static void modem_mem_init_heap(void)
{
  /*! @brief 
  **  Modem Central heap buffer area
  */
/*#ifndef USES_LTE_CATM
  uint32 random_allocation_size=0;
#endif*/
  int j;

  /*! @brief array of free memory sections
  */
  modem_mem_section_cfg sections[] = 
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
  #endif /* T_WINNT */
    { NULL,                    0                       }
  };
  modem_mem_heap_size = MODEM_MEM_HEAP_SIZE;
  memset(&modem_mem_heap, 0, sizeof(mem_heap_type));
#ifdef FEATURE_STATIC_MODEM_HEAP  
  mem_init_heap(&modem_mem_heap,modem_heap_static_buffer,modem_mem_heap_size,&modemheap_malloc_failed);
#else
  mem_init_heap_alloc(&modem_mem_heap,MODEM_MEM_HEAP_SIZE,&modemheap_malloc_failed);
#endif
  modem_mem_validate_sections(sections);
  mem_init_heap(&modem_mem_heap,sections[0].start,sections[0].size,&modemheap_malloc_failed);
  for (j = 0; (j < (sizeof(sections)/sizeof(modem_mem_section_cfg))) && (sections[j].start != 0x0); ++j)   
  {
    mem_heap_add_section(&modem_mem_heap,sections[j].start, sections[j].size);
  }

/*#ifndef USES_LTE_CATM
   // Do a random allocation and add it back when heap gets exhaused
   //we can get uint32 random number which can randomize the LSB 32 bits
   //MAsk the random number so that we can get max 128k allocation
  if(PRNGML_ERROR_NONE != PrngML_getdata(((uint8*)&random_allocation_size), 4))
  {
     ASSERT(0);
  }
  random_allocation_size=random_allocation_size&MODEM_MEM_RANDOM_ALLOC_MASK; 

  if (random_allocation_size != 0)
  {
     random_alloc_ptr=modem_mem_alloc(random_allocation_size,0);
     if (random_alloc_ptr == NULL)
     {
        ASSERT(0);
     }
  }
#endif*/

#ifdef FEATURE_MEM_DEBUG
   if(MEM_TLS_CREATE_ERR != memheap_global_error)
   {
      modem_mem_heap.heapStatistics = &modem_heap_task_stats[0];
   }
   modem_mem_heap.mem_get_quota_fnc_ptr = modem_mem_get_task_quota;
#endif

  return;
} /* modem_mem_init_heap() */

/*===========================================================================

  FUNCTION:  modem_heap_init

===========================================================================*/
/*!
    @brief
    Initializes the modem by calling into modem_heap_init.

    @detail
    Initializes the modem heap at an early stage so that no client fails to allocate memory.

    @return
    None

    @note
    This function is called in rcinit group0.
*/
/*=========================================================================*/

  void modem_heap_init()
  {
    modem_mem_init_heap();
    is_heap_initialized=1;
  }


/*===========================================================================

  FUNCTION:  modem_mem_alloc

===========================================================================*/
/*!
    @brief
    Allocates a block of size bytes from the heap.

    @return
    Returns a pointer to the newly allocated block, or 
    NULL if the block could not be allocated.
*/
/*=========================================================================*/
void* modem_mem_alloc(
  size_t                size,       /*!< Number of bytes to allocate */
  modem_mem_client_e    client      /*!< Client ID for this mem block */
)
{
  void *ptr = NULL;

  /*-----------------------------------------------------------------------*/

  if (!size) 
  {
    return NULL;  /* Zero size request - return NULL immediately */
  }

  /*-----------------------------------------------------------------------*/

  if(!is_heap_initialized)
    return NULL;

  #ifdef FEATURE_ENABLE_HEAP_PROFILING
    uint32 malloc_inTime=0, malloc_outTime=0, diff =0;
	malloc_inTime = timetick_get_safe();
    #endif

  #ifdef FEATURE_SPLIT_MODEM_HEAP

    /* Variable to know if allocation needs to be done from modem mem heap */
    boolean normal_heap_alloc = FALSE;

    /* alloc from internal heap based on requested size,fall back
       to normal heap in case of failure  */
    if((alloc_from_modem_internal(size)))
    {
      ptr = modem_internal_mem_alloc(size,client);
      if(!ptr)
      {
        ptr = mem_malloc(&modem_mem_heap,size);
        normal_heap_alloc = TRUE;
      }
    }
    else
    {
      ptr = mem_malloc(&modem_mem_heap,size);
      normal_heap_alloc = TRUE;
    }
  #else
    ptr = mem_malloc(&modem_mem_heap,size);
  #endif /* FEATURE_SPLIT_MODEM_HEAP */

	#ifdef FEATURE_ENABLE_HEAP_PROFILING
	malloc_outTime = timetick_get_safe();
	diff = malloc_outTime - malloc_inTime;
    memheap_profile_table[modem_mem_heap.debug_heap_index].malloc_avg+= diff;
    memheap_profile_table[modem_mem_heap.debug_heap_index].malloc_cnt++;
	#endif

  #ifdef FEATURE_MEM_DEBUG
    {
      void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();

      if((uint8 *)caller_ptr <= (glboal_autostack_ptr+400) && (uint8 *)caller_ptr >= (glboal_autostack_ptr))
      caller_ptr=MEM_HEAP_CALLER_ADDRESS_EXT();
          
      #ifdef FEATURE_SPLIT_MODEM_HEAP
        if(normal_heap_alloc)    
          memdebug_updatecallerptr(&modem_mem_heap,ptr, caller_ptr,size);
        else
          memdebug_updatecallerptr(&modem_internal_mem_heap,ptr, caller_ptr,size);
      #else
        memdebug_updatecallerptr(&modem_mem_heap,ptr, caller_ptr,size);    
      #endif /*FEATURE_SPLIT_MODEM_HEAP*/
    }
  #endif /* FEATURE_MEM_DEBUG */

  return ptr;
 
} /* modem_mem_alloc() */

void* modem_mem_alloc_setcaller(
  size_t               size,       /*!< Number of bytes to allocate */
  modem_mem_client_e   client,      /*!< Client ID for this mem block */
  void*                caller_ptr   /*!< Caller Ptr to be set in debug information */
)
{
  void *ptr = NULL;
    
  #ifdef FEATURE_SPLIT_MODEM_HEAP

    /* Variable to know if allocation needs to be done from modem mem heap */
    boolean normal_heap_alloc = FALSE;

    /* alloc from internal heap based on requested size,fall back
       to normal heap in case of failure */
    if((alloc_from_modem_internal(size)))
    {
      ptr = modem_internal_mem_alloc(size,client);
      if(!ptr)
      {    
        ptr = mem_malloc(&modem_mem_heap,size);
        normal_heap_alloc = TRUE;
      }
    }
    else
    {
      ptr = mem_malloc(&modem_mem_heap,size);
      normal_heap_alloc = TRUE;
    }
  #else
    ptr = mem_malloc(&modem_mem_heap,size);
  #endif

  #ifdef FEATURE_MEM_DEBUG

      if((uint8 *)caller_ptr <= (glboal_autostack_ptr+400) && (uint8 *)caller_ptr >= (glboal_autostack_ptr))
      {
            caller_ptr=MEM_HEAP_CALLER_ADDRESS_EXT();
            if((uint8 *)caller_ptr <= (glboal_autostack_ptr+400) && (uint8 *)caller_ptr >= (glboal_autostack_ptr))
            caller_ptr=MEM_HEAP_CALLER_ADDRESS_EXT2();
      }
    #ifdef FEATURE_SPLIT_MODEM_HEAP
      if(normal_heap_alloc)
        memdebug_updatecallerptr(&modem_mem_heap,ptr, caller_ptr,size);
      else
        memdebug_updatecallerptr(&modem_internal_mem_heap,ptr, caller_ptr,size);      
    #else
      memdebug_updatecallerptr(&modem_mem_heap,ptr, caller_ptr,size);    
    #endif/*FEATURE_SPLIT_MODEM_HEAP*/
  #endif /*FEATURE_MEM_DEBUG */

  return ptr;
}

/*===========================================================================

  FUNCTION:  modem_mem_calloc

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

void* modem_mem_calloc(
  size_t               elt_count,  /*!< Number of elements to allocate */
  size_t               elt_size,   /*!< Size of each element*/
  modem_mem_client_e   client      /*!< Client ID for this mem block */
)
{
  void *ptr=NULL;

  if(!is_heap_initialized)
  return NULL;
  #ifdef FEATURE_SPLIT_MODEM_HEAP

    /* Variable to know if allocation needs to be done from modem mem heap */
    boolean normal_heap_alloc = FALSE;

    /* alloc from internal heap based on requested size,fall back
       to normal heap in case of failure */
    if((alloc_from_modem_internal((elt_count * elt_size))))
    {
      ptr = modem_internal_mem_calloc(elt_count,elt_size,client);
      if(!ptr)
      {
        ptr=mem_calloc(&modem_mem_heap,elt_count, elt_size);
        normal_heap_alloc = TRUE;
      }
    }
    else
    {
      ptr=mem_calloc(&modem_mem_heap,elt_count, elt_size);
      normal_heap_alloc = TRUE;
    }
  #else
    ptr=mem_calloc(&modem_mem_heap,elt_count, elt_size);
  #endif /* FEATURE_SPLIT_MODEM_HEAP */

  #ifdef FEATURE_MEM_DEBUG
  {
      void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();

      if((uint8 *)caller_ptr <= (glboal_autostack_ptr+400) && (uint8 *)caller_ptr >= (glboal_autostack_ptr))
            caller_ptr=MEM_HEAP_CALLER_ADDRESS_EXT();

    #ifdef FEATURE_SPLIT_MODEM_HEAP    
      if(normal_heap_alloc)
        memdebug_updatecallerptr(&modem_mem_heap,ptr, caller_ptr,elt_count * elt_size);
      else
        memdebug_updatecallerptr(&modem_internal_mem_heap,ptr,caller_ptr,elt_count * elt_size);
    #else
      memdebug_updatecallerptr(&modem_mem_heap,ptr, caller_ptr,elt_count * elt_size);
    #endif /*FEATURE_MEM_DEBUG*/
  }
  #endif /* FEATURE_MEM_DEBUG */

  return ptr;
} /* modem_mem_calloc() */

void* modem_mem_calloc_setcaller(
  size_t              elt_count,  /*!< Number of elements to allocate */
  size_t              elt_size,   /*!< Size of each element*/
  modem_mem_client_e  client,      /*!< Client ID for this mem block */
  void*                caller_ptr   /*!< Caller Ptr to be set in debug information */
)
{
  void *ptr = NULL;

  #ifdef FEATURE_SPLIT_MODEM_HEAP  
    /* Variable to know if allocation needs to be done from modem mem heap */
    boolean normal_heap_alloc = FALSE;

    /* alloc from internal heap based on requested size,fall back
       to normal heap in case of failure */
    if((alloc_from_modem_internal((elt_count * elt_size))))
    {
      ptr = modem_internal_mem_calloc(elt_count,elt_size,client);
      if(!ptr)
      {
        ptr=mem_calloc(&modem_mem_heap,elt_count, elt_size);
        normal_heap_alloc = TRUE;
      }
    }
    else
    {
      ptr=mem_calloc(&modem_mem_heap,elt_count, elt_size);
      normal_heap_alloc = TRUE;
    }
  #else
    ptr=mem_calloc(&modem_mem_heap,elt_count, elt_size);
  #endif

{
  #ifdef FEATURE_MEM_DEBUG

      if((uint8 *)caller_ptr <= (glboal_autostack_ptr+400) && (uint8 *)caller_ptr >= (glboal_autostack_ptr))
      {
            caller_ptr=MEM_HEAP_CALLER_ADDRESS_EXT();
            if((uint8 *)caller_ptr <= (glboal_autostack_ptr+400) && (uint8 *)caller_ptr >= (glboal_autostack_ptr))
            caller_ptr=MEM_HEAP_CALLER_ADDRESS_EXT2();
      }

    #ifdef FEATURE_SPLIT_MODEM_HEAP
      if(normal_heap_alloc)
        memdebug_updatecallerptr(&modem_mem_heap,ptr, caller_ptr,elt_count * elt_size);
      else
        memdebug_updatecallerptr(&modem_internal_mem_heap,ptr, caller_ptr,elt_count * elt_size);      
    #else
      memdebug_updatecallerptr(&modem_mem_heap,ptr, caller_ptr,elt_count * elt_size);    
    #endif/*FEATURE_SPLIT_MODEM_HEAP*/
  #endif /*FEATURE_MEM_DEBUG */

}

  return ptr;
}
/*===========================================================================

  FUNCTION:  modem_mem_realloc

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
    If ptr is NULL, the function behaves exactly like modem_mem_alloc 
    (see above). If ptr is not NULL and size is 0, the function behaves
    exactly like modem_mem_free (see below).  
    If the block cannot be resized, ptr is not NULL and size is not 0, then 
    NULL is returned and the original block is left untouched.  If the ptr 
    block IS successfully resized and the returned value is different from 
    the ptr value passed in, the old block passed in must be considered 
    deallocated and no longer useable (i.e. do NOT try to mem_free it!) 
    This function will call modem_mem_alloc if it can't grow the block in 
    place.

    @return
    Returns a pointer to the beginning of the resized block of memory (which
    may be different than ptr) or NULL if the block cannot be resized.
*/
/*=========================================================================*/

void *modem_mem_realloc(
  void                *ptr,        /*!< A block previously allocated 
                                        from heap */
  size_t               size,       /*!< New size (in bytes) of the ptr block
                                        of memory */
  modem_mem_client_e   client      /*!< Client ID for this mem block */
)
{
  void *ptr_ret=NULL;
  if(!is_heap_initialized)
    return NULL;

  #ifdef FEATURE_ENABLE_HEAP_PROFILING
    uint32 realloc_inTime=0 , realloc_outTime=0;
    realloc_inTime = timetick_get_safe();
  #endif
  #ifdef FEATURE_SPLIT_MODEM_HEAP 
    mem_block_header_type *theBlock = NULL;
    size_t oldSize = 0;
    /*free item to internal heap or normal heap based on address range  */
    if(modem_internal_heap_addr_range(ptr))
    {      
      ptr_ret=modem_internal_mem_realloc(ptr,size,client);
      if(ptr_ret == NULL)
      {
        ptr_ret=mem_malloc(&modem_mem_heap,size);
        if(ptr_ret != NULL)
        {
          /* Copy data to the newly allocated memory */
          if(ptr != NULL)
          {
             theBlock = (mem_block_header_type *)((char *) ptr - sizeof(mem_block_header_type));
             oldSize = (size_t) mem_get_block_logical_size(theBlock, ptr);
             memscpy(ptr_ret,size,ptr,oldSize);
          }
          mem_free(&modem_internal_mem_heap,ptr);
        }
      }
    }
    else
    {
      ptr_ret=mem_realloc(&modem_mem_heap, ptr,size);
    }
  #else
    ptr_ret=mem_realloc(&modem_mem_heap, ptr,size);
  #endif /* FEATURE_SPLIT_MODEM_HEAP */
   
  #ifdef FEATURE_ENABLE_HEAP_PROFILING
    realloc_outTime = timetick_get_safe( );
    memheap_profile_table[modem_mem_heap.debug_heap_index].realloc_avg = realloc_outTime - realloc_inTime;
    memheap_profile_table[modem_mem_heap.debug_heap_index].realloc_cnt++;
  #endif

  #ifdef FEATURE_MEM_DEBUG
    {
      void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
	  
      if((uint8 *)caller_ptr <= (glboal_autostack_ptr+400) && (uint8 *)caller_ptr >= (glboal_autostack_ptr))
            caller_ptr=MEM_HEAP_CALLER_ADDRESS_EXT();

      #ifdef FEATURE_SPLIT_MODEM_HEAP
        if(!modem_internal_heap_addr_range(ptr))
          memdebug_updatecallerptr(&modem_mem_heap,ptr_ret, caller_ptr,size);
        else
          memdebug_updatecallerptr(&modem_internal_mem_heap,ptr_ret, caller_ptr,size);
      #else
        memdebug_updatecallerptr(&modem_mem_heap,ptr_ret, caller_ptr,size);    
      #endif /*FEATURE_SPLIT_MODEM_HEAP*/      
    }
  #endif /* FEATURE_MEM_DEBUG */

  return ptr_ret;
} /* modem_mem_realloc() */

void* modem_mem_realloc_setcaller(
  void                *ptr,        /*!< A block previously allocated 
                                        from heap */
  size_t               size,       /*!< New size (in bytes) of the ptr block
                                        of memory */
  modem_mem_client_e   client,      /*!< Client ID for this mem block */

  void*                caller_ptr   /*!< Caller Ptr to be set in debug information */
)
{
  void *ptr_ret=NULL;
  ptr_ret=modem_mem_realloc(ptr,size,client);

  #ifdef FEATURE_MEM_DEBUG
      if((uint8 *)caller_ptr <= (glboal_autostack_ptr+400) && (uint8 *)caller_ptr >= (glboal_autostack_ptr))
      {
            caller_ptr=MEM_HEAP_CALLER_ADDRESS_EXT();
            if((uint8 *)caller_ptr <= (glboal_autostack_ptr+400) && (uint8 *)caller_ptr >= (glboal_autostack_ptr))
            caller_ptr=MEM_HEAP_CALLER_ADDRESS_EXT2();
      }
    #ifdef FEATURE_SPLIT_MODEM_HEAP
      if(modem_internal_heap_addr_range(ptr))
        memdebug_updatecallerptr(&modem_internal_mem_heap,ptr_ret, caller_ptr,size);
      else
        memdebug_updatecallerptr(&modem_mem_heap,ptr_ret, caller_ptr,size);    
    #else
      memdebug_updatecallerptr(&modem_mem_heap,ptr_ret, caller_ptr,size);    
    #endif /*FEATURE_SPLIT_MODEM_HEAP*/
#endif /* FEATURE_MEM_DEBUG */

  return ptr_ret;
}

/*===========================================================================

  FUNCTION:  modem_mem_free

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
void modem_mem_free(
  void                *ptr,        /*!< Memory to free */
  modem_mem_client_e   client      /*!< User ID for this mem block */
)
{
  #ifdef FEATURE_ENABLE_HEAP_PROFILING
    uint32 free_inTime=0, free_outTime=0;
    free_inTime = timetick_get_safe();
  #endif

  #ifdef FEATURE_SPLIT_MODEM_HEAP  
    /*free item to internal heap or normal heap based on address range  */
    if(modem_internal_heap_addr_range(ptr))
      mem_free(&modem_internal_mem_heap,ptr);
    else
      mem_free(&modem_mem_heap,ptr);
  #else
    mem_free(&modem_mem_heap,ptr);
  #endif /* FEATURE_SPLIT_MODEM_HEAP */

#ifdef FEATURE_ENABLE_HEAP_PROFILING
    free_outTime = timetick_get_safe();
    memheap_profile_table[modem_mem_heap.debug_heap_index].free_avg += (free_outTime - free_inTime);
    memheap_profile_table[modem_mem_heap.debug_heap_index].free_cnt++;
  #endif 
} /* modem_mem_free() */


/*===========================================================================

  FUNCTION:  modem_mem_add_region

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

void modem_mem_add_region(void *section_start, unsigned long section_size)
{
   mem_heap_add_section(&modem_mem_heap,section_start,section_size);
}

/*===========================================================================

  FUNCTION:  modem_mem_get_section_info

===========================================================================*/
/*!
    @brief
    Get the section information about modem heap

    @detail
   

    @return
    None.
*/
/*=========================================================================*/

void modem_mem_get_section_info(void **section_info)
{
	*section_info=(void *)&modem_mem_heap.sections[0];
}

/*===========================================================================

FUNCTION modemheap_malloc_failed

DESCRIPTION
  This function logs heap statistics, and is called when memheap detects
  an allocation failure from the Modem heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void modemheap_malloc_failed
(
  struct mem_heap_struct *heap_ptr, /* Heap on which allocation is attempted */
  size_t size                       /* Size of failed request */
)
{
  memheap_v_info_s_type *info;

  /* Record the current heap state */
  if (modemheap_fail_index < ARR_SIZE(modemheap_fail_info))
  {
    int i, freeBlk_list_idx = 0;
    unsigned long  totalFreeBlocks = 0;
    unsigned long  totalSizeOfFreeBlocks=0;
    freeBlockList *temp;
    uint16* pblk;
    info = &modemheap_fail_info[modemheap_fail_index];
    (info->totals).largest_free_block=0;
    for (i= 0; i< MEMHEAP2_BIN_COUNT; i++)
    {
       totalFreeBlocks += heap_ptr->BIN[i].totalNodes;
     }

     for (i = MEMHEAP2_BIN_COUNT-1; i>=0; i--)
     {
         if((heap_ptr->BIN[i].firstNode)!=0){
           temp = heap_ptr->BIN[i].firstNode;

           pblk =  (uint16*)temp;
           ASSERT(!(INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk)));

           if((info->totals).largest_free_block == 0){
              if((heap_ptr->BIN[i].lastNode)!=0){
                   uint16* pblk2  = (uint16*)(heap_ptr->BIN[i].lastNode);
                   ASSERT(!(INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk2)));
                  (info->totals).largest_free_block = ((mem_block_header_type*)(heap_ptr->BIN[i].lastNode))->forw_offset;
              }
           }

           totalSizeOfFreeBlocks+=((mem_block_header_type*)temp)->forw_offset;
           if(MEMHEAP_V_INFO_FREEBLOCKS > freeBlk_list_idx)
           {
               info->freeblocks[freeBlk_list_idx] = ((mem_block_header_type*)temp)->forw_offset;
               freeBlk_list_idx++;
           }
           
           while(temp->nextPtr)
           {
               pblk =  (uint16*)(temp->nextPtr);
               ASSERT(!(INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk)));
               if(MEMHEAP_V_INFO_FREEBLOCKS > freeBlk_list_idx)
               {
                   info->freeblocks[freeBlk_list_idx] = NEXT_BLOCK_SIZE(temp);
                   freeBlk_list_idx++;
               }
               totalSizeOfFreeBlocks+=NEXT_BLOCK_SIZE(temp);
               temp = temp->nextPtr;
           }
         }
     }

    
    info->heap_ptr = heap_ptr;
    info->size = size;

    (info->totals).total_physical_bytes = heap_ptr->total_bytes; 
    (info->totals).current_block_count = heap_ptr->total_blocks;
    (info->totals).used_blocks = heap_ptr->total_blocks - totalFreeBlocks;
  
    (info->totals).used_bytes = heap_ptr->used_bytes;
    (info->totals).free_bytes = totalSizeOfFreeBlocks;
    (info->totals).max_logical_used = heap_ptr->max_used;
    (info->totals).max_logical_request = heap_ptr->max_request;
  }
  modemheap_fail_index++;

  MSG_ERROR("mdoem heap failed to allocate %d bytes (failure %d)",
    size, modemheap_fail_index, 0);
}


#ifdef FEATURE_MEM_DEBUG
static uint32 modem_mem_get_task_quota(void)
{
   RCINIT_INFO rc_handle;
   RCINIT_HEAPQSZ rc_heap_quota=0;

   rc_handle = rcinit_lookup_self();
   if(NULL != rc_handle)
   {
       rc_heap_quota = rcinit_lookup_heapqsz_info(rc_handle,&modem_mem_heap);
   }

   return rc_heap_quota;
}
#endif

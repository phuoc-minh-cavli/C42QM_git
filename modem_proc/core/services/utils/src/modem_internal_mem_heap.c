/*!
  @file
  modem_internal_mem.c

  @brief
  Implementation of a heap manager to manage memory allocations
  and deallocations using a Best-Fit strategy.

*/

/*===========================================================================

  Copyright (c) 2009-2015,2017, 2019 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>           /* Definition for basic types and macros */
#include "modem_mem.h"        /* External interface to modem_mem.c */
#include "memheap.h"
#include "memheap_v.h"

#ifndef USES_LTE_CATM
#include "PrngML.h"
#endif


#ifdef FEATURE_MEM_DEBUG
#include "rcinit.h"
#endif

mem_heap_type modem_internal_mem_heap;
static unsigned int is_modem_internal_heap_initialized=0;

#ifndef USES_LTE_CATM
//static void *random_alloc_ptr=NULL;
#define MODEM_INTERNAL_RANDOM_ALLOC_MASK 0x1FFFF
#endif

#ifdef FEATURE_MEM_DEBUG
#ifndef MODEM_INTERNAL_HEAP_CALLER_LEVEL
#define MODEM_INTERNAL_HEAP_CALLER_LEVEL     (1)
#endif

#if defined(FEATURE_QDSP6)
 #define  MEM_HEAP_CALLER_ADDRESS() ((void *)__builtin_return_address(MODEM_INTERNAL_HEAP_CALLER_LEVEL));
#else
 #define MEM_HEAP_CALLER_ADDRESS() ((void *) __return_address())
#endif

#endif



#ifndef MODEM_INTERNAL_HEAP_SIZE
    #define MODEM_INTERNAL_HEAP_SIZE 0x40000
#endif /* MODEM_INTERNAL_HEAP_SIZE */


  #define MODEM2HEAP_STATIC_BUFFER_ATTR __attribute__((section(".bss.modemheap_internal"),aligned (64))) 
  static uint8 modem_internal_heap_static_buffer[MODEM_INTERNAL_HEAP_SIZE] MODEM2HEAP_STATIC_BUFFER_ATTR;


static volatile unsigned int modem_internal_mem_heap_size = MODEM_INTERNAL_HEAP_SIZE;

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief
** Contains the information for a memory section input configuration
*/
typedef struct _modem_internal_mem_section_cfg
{
   byte                 *start;       /*!< start address of section */
   uint32                size;        /*!< size of the section */
} modem_internal_mem_section_cfg;


#ifdef FEATURE_MEM_DEBUG
/* AMSS memory debug functions */
extern void memdebug_updatecallerptr(mem_heap_type *heap_ptr,void *ptr,void *caller_ptr, size_t size);
memheap_task_stats_type modem_internal_heap_task_stats[MEMHEAP_MAX_THREADS];
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
/*===========================================================================

  FUNCTION:  modem_internal_mem_validate_sections

===========================================================================*/
/*!
    @brief
    Validates and arranges the memory sections available for the Modem Heap

    @return
    None
*/
/*=========================================================================*/
void modem_internal_mem_validate_sections(
  modem_internal_mem_section_cfg mem_sections[]   /*!< array of available memory
                                              sections */
)
{
  /*! @brief temporary buffer for modem heap section entry
  */
  modem_internal_mem_section_cfg t_section;
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

} /* modem_internal_mem_validate_sections() */

/*===========================================================================

  FUNCTION:  modem_internal_mem_init_heap

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
static void modem_internal_mem_init_heap(void)
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
  modem_internal_mem_section_cfg sections[] = 
  {
  /* It's important to keep T_WINNT checking here; on QTF builds the target
   * specific modem heap configuration should NOT be used 
   */
  #ifndef T_WINNT
    #ifdef MODEM_INTERNAL_SECTION_BASE0
    { (byte *) MODEM_INTERNAL_SECTION_BASE0, MODEM_INTERNAL_SECTION_SIZE0 },
    #endif
    #ifdef MODEM_INTERNAL_SECTION_BASE1
    { (byte *) MODEM_INTERNAL_SECTION_BASE1, MODEM_INTERNAL_SECTION_SIZE1 },
    #endif
    #ifdef MODEM_INTERNAL_SECTION_BASE2
    { (byte *) MODEM_INTERNAL_SECTION_BASE2, MODEM_INTERNAL_SECTION_SIZE2 },
    #endif
    #ifdef MODEM_INTERNAL_SECTION_BASE3
    { (byte *) MODEM_INTERNAL_SECTION_BASE3, MODEM_INTERNAL_SECTION_SIZE3 },
    #endif
    #ifdef MODEM_INTERNAL_SECTION_BASE4
    { (byte *) MODEM_INTERNAL_SECTION_BASE4, MODEM_INTERNAL_SECTION_SIZE4 },
    #endif
    #ifdef MODEM_INTERNAL_SECTION_BASE5
    { (byte *) MODEM_INTERNAL_SECTION_BASE5, MODEM_INTERNAL_SECTION_SIZE5 },
    #endif
    #ifdef MODEM_INTERNAL_SECTION_BASE6
    { (byte *) MODEM_INTERNAL_SECTION_BASE6, MODEM_INTERNAL_SECTION_SIZE6 },
    #endif
    #ifdef MODEM_INTERNAL_SECTION_BASE7
    { (byte *) MODEM_INTERNAL_SECTION_BASE7, MODEM_INTERNAL_SECTION_SIZE7 },
    #endif
  #endif /* T_WINNT */
    { NULL,                    0                       }
  };
  modem_internal_mem_heap_size = MODEM_INTERNAL_HEAP_SIZE;
  memset(&modem_internal_mem_heap, 0, sizeof(mem_heap_type));
#ifdef FEATURE_STATIC_MODEM_HEAP  
  mem_init_heap(&modem_internal_mem_heap,modem_internal_heap_static_buffer,modem_internal_mem_heap_size,NULL);
#else
  mem_init_heap_alloc(&modem_internal_mem_heap,MODEM_INTERNAL_HEAP_SIZE,NULL);
#endif
  modem_internal_mem_validate_sections(sections);
  mem_init_heap(&modem_internal_mem_heap,sections[0].start,sections[0].size,NULL);
  for (j = 0; (j < (sizeof(sections)/sizeof(modem_internal_mem_section_cfg))) && (sections[j].start != 0x0); ++j)   
  {
    mem_heap_add_section(&modem_internal_mem_heap,sections[j].start, sections[j].size);
  }
#ifdef FEATURE_MEM_DEBUG
   if(MEM_TLS_CREATE_ERR != memheap_global_error)
   {
      modem_internal_mem_heap.heapStatistics = &modem_internal_heap_task_stats[0];
   }
   modem_internal_mem_heap.mem_get_quota_fnc_ptr = NULL;
#endif

  return;
} /* modem_internal_mem_init_heap() */


/*===========================================================================

  FUNCTION:  modem_internal_mem_check_heap_initialized

===========================================================================*/
/*!
    @brief
    Checks to see if the modem_internal heap is initialized. If not, this function 
    initializes the heap.

    @return
    None
*/
/*=========================================================================*/
static void modem_internal_mem_check_heap_initialized(void)
{
  if (is_modem_internal_heap_initialized) 
  {
    return;
  }

  if (!(is_modem_internal_heap_initialized))
  {
    modem_internal_mem_init_heap();    
    is_modem_internal_heap_initialized = 1;
  }
  
} /* modem_internal_mem_check_heap_initialized() */

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

  void modem_internal_heap_init()
  {
    modem_internal_mem_init_heap();
    is_modem_internal_heap_initialized=1;
  }


/*===========================================================================

  FUNCTION:  modem_internal_mem_alloc

===========================================================================*/
/*!
    @brief
    Allocates a block of size bytes from the heap.

    @return
    Returns a pointer to the newly allocated block, or 
    NULL if the block could not be allocated.
*/
/*=========================================================================*/
void* modem_internal_mem_alloc(
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

  modem_internal_mem_check_heap_initialized();

  if(!is_modem_internal_heap_initialized)
    return NULL;

  

  ptr=mem_malloc(&modem_internal_mem_heap,size);

#ifdef FEATURE_MEM_DEBUG
  {
    void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(&modem_internal_mem_heap,ptr, caller_ptr,size);
  }
#endif
  return ptr;
 
} /* modem_internal_mem_alloc() */

void* modem_internal_mem_alloc_setcaller(
  size_t               size,       /*!< Number of bytes to allocate */
  modem_mem_client_e   client,      /*!< Client ID for this mem block */
  void*                caller_ptr   /*!< Caller Ptr to be set in debug information */
)
{
  void *ptr = NULL;
  ptr=modem_internal_mem_alloc(size,client);
#ifdef FEATURE_MEM_DEBUG
  memdebug_updatecallerptr(&modem_internal_mem_heap,ptr, caller_ptr,size);
#endif
  return ptr;
}

/*===========================================================================

  FUNCTION:  modem_internal_mem_calloc

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

void* modem_internal_mem_calloc(
  size_t               elt_count,  /*!< Number of elements to allocate */
  size_t               elt_size,   /*!< Size of each element*/
  modem_mem_client_e   client      /*!< Client ID for this mem block */
)
{
  void *ptr=NULL;
  modem_internal_mem_check_heap_initialized();


  ptr=mem_calloc(&modem_internal_mem_heap,elt_count, elt_size);
#ifdef FEATURE_MEM_DEBUG
  {
    void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(&modem_internal_mem_heap,ptr, caller_ptr,elt_count * elt_size);
  }
  #endif
  return ptr;
} /* modem_internal_mem_calloc() */

void* modem_internal_mem_calloc_setcaller(
  size_t              elt_count,  /*!< Number of elements to allocate */
  size_t              elt_size,   /*!< Size of each element*/
  modem_mem_client_e  client,      /*!< Client ID for this mem block */
  void*                caller_ptr   /*!< Caller Ptr to be set in debug information */
)
{
  void *ptr = NULL;
  ptr=modem_internal_mem_calloc(elt_count,elt_size,client);
#ifdef FEATURE_MEM_DEBUG
  memdebug_updatecallerptr(&modem_internal_mem_heap,ptr, caller_ptr,elt_count * elt_size);
#endif
  return ptr;
}
/*===========================================================================

  FUNCTION:  modem_internal_mem_realloc

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
    If ptr is NULL, the function behaves exactly like modem_internal_mem_alloc 
    (see above). If ptr is not NULL and size is 0, the function behaves
    exactly like modem_internal_mem_free (see below).  
    If the block cannot be resized, ptr is not NULL and size is not 0, then 
    NULL is returned and the original block is left untouched.  If the ptr 
    block IS successfully resized and the returned value is different from 
    the ptr value passed in, the old block passed in must be considered 
    deallocated and no longer useable (i.e. do NOT try to mem_free it!) 
    This function will call modem_internal_mem_alloc if it can't grow the block in 
    place.

    @return
    Returns a pointer to the beginning of the resized block of memory (which
    may be different than ptr) or NULL if the block cannot be resized.
*/
/*=========================================================================*/

void *modem_internal_mem_realloc(
  void                *ptr,        /*!< A block previously allocated 
                                        from heap */
  size_t               size,       /*!< New size (in bytes) of the ptr block
                                        of memory */
  modem_mem_client_e   client      /*!< Client ID for this mem block */
)
{
  void *ptr_ret=NULL;
  modem_internal_mem_check_heap_initialized();


  ptr_ret=mem_realloc(&modem_internal_mem_heap, ptr,size);
   

#ifdef FEATURE_MEM_DEBUG
    {
      void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
      memdebug_updatecallerptr(&modem_internal_mem_heap,ptr_ret, caller_ptr,size);
    }
  #endif
  return ptr_ret;
} /* modem_internal_mem_realloc() */

void* modem_internal_mem_realloc_setcaller(
  void                *ptr,        /*!< A block previously allocated 
                                        from heap */
  size_t               size,       /*!< New size (in bytes) of the ptr block
                                        of memory */
  modem_mem_client_e   client,      /*!< Client ID for this mem block */

  void*                caller_ptr   /*!< Caller Ptr to be set in debug information */
)
{
  void *ptr_ret=NULL;
  ptr_ret=modem_internal_mem_realloc(ptr,size,client);
#ifdef FEATURE_MEM_DEBUG
  memdebug_updatecallerptr(&modem_internal_mem_heap,ptr_ret, caller_ptr,size);
#endif
  return ptr_ret;
}

/*===========================================================================

  FUNCTION:  modem_internal_mem_free

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
void modem_internal_mem_free(
  void                *ptr,        /*!< Memory to free */
  modem_mem_client_e   client      /*!< User ID for this mem block */
)
{

  mem_free(&modem_internal_mem_heap,ptr);
} /* modem_internal_mem_free() */


/*===========================================================================

  FUNCTION:  modem_internal_mem_add_region

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

void modem_internal_mem_add_region(void *section_start, unsigned long section_size)
{
   mem_heap_add_section(&modem_internal_mem_heap,section_start,section_size);
}

/*===========================================================================

  FUNCTION:  modem_internal_mem_get_section_info

===========================================================================*/
/*!
    @brief
    Get the section information about modem heap

    @detail
   

    @return
    None.
*/
/*=========================================================================*/

void modem_internal_mem_get_section_info(void **section_info)
{
	*section_info=(void *)&modem_internal_mem_heap.sections[0];
}



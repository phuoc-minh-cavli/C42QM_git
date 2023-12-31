/* =========================================================================

                              A M S S   H E A P

DESCRIPTION
  Wrapper for malloc, realloc, calloc and free calls for AMSS_HEAP.

Copyright (c) 2006-2014, 2019 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/services/utils/src/amssheap.c#2 $ $DateTime: 2020/05/18 06:06:25 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
08/30/12   rks     robust XOR logic and enable PRNG uses
05/10/12   rks     memheap2 
12/05/07   jc      Added periodic defragmentation feature.
08/15/07   jc      Added dynamic heap feature.
08/29/06   jc      Added heap debug support.
07/03/06   jc      Added heap init checks and malloc failure logging.
03/27/06   jhs     File created
============================================================================ */


/*============================================================================
                             INCLUDE FILES
============================================================================*/
#include <stringl/stringl.h>
#include "customer.h"
#include "memheap.h"
#include "msg.h"
#include <stdlib.h>
#include "qurt.h"
#include "osal.h"
#include "timetick.h"
#include "assert.h"

#ifdef FEATURE_MEMHEAP2_USE_PRNG
#include "PrngML.h"
#endif

/*============================================================================
                             CONSTANT DEFINITIONS
============================================================================*/

/* Heap allocation failure constants */
#ifndef AMSSHEAP_FAIL_ARRAY_SIZE
#define AMSSHEAP_FAIL_ARRAY_SIZE  2
#endif
#ifndef AMSSHEAP_INFO_FREEBLOCKS
#define AMSSHEAP_INFO_FREEBLOCKS  64
#endif

#ifndef MEMHEAP_CRIT_SEC_SIZE 
#define MEMHEAP_CRIT_SEC_SIZE 64
#endif

/* Calculate XOR based Guard byte and restore */
#define ADD_GUARD_BYTES_TO_FREE_HEADER(magic_num_free, block) \
            (block[0] = block[1]^block[2]^block[3]^block[4]^block[5]^magic_num_free)

#ifdef FEATURE_MEM_DEBUG
#ifndef AMSS_HEAP_CALLER_LEVEL
#define AMSS_HEAP_CALLER_LEVEL     (0)
#endif

#if defined(FEATURE_QDSP6)
 #define  MEM_HEAP_CALLER_ADDRESS() ((void *)__builtin_return_address(AMSS_HEAP_CALLER_LEVEL));
#else
 #define MEM_HEAP_CALLER_ADDRESS() ((void *) __return_address())
#endif
#endif

/*============================================================================
                             DATA DECLARATIONS
=============================================================================*/
#ifdef FEATURE_ENABLE_HEAP_PROFILING
int start_profiling = 0;
#define PROFILE_START_POINT 100
typedef struct _memheap_profile_table_type {
  void * heap_ptr;
  uint32 malloc_avg;
  uint32 malloc_cnt;
  uint32 realloc_avg;
  uint32 realloc_cnt;
  uint32 free_avg;
  uint32 free_cnt;
  uint32 pad;
}memheap_profile_table_type;

memheap_profile_table_type memheap_profile_table[50];
uint32 memheap_profile_table_index = 0;

static uint32 malloc_in_out_biggest_diff = 0;
static uint32 malloc_in_out_smallest_diff = 100;

#endif /*FEATURE_ENABLE_HEAP_PROFILING */

/* Structure used to log heap allocation failures */
typedef struct amssheap_info_s
{
  struct mem_heap_struct *heap_ptr;
  size_t size;
  mem_heap_totals_type totals;
  unsigned int freeblocks[AMSSHEAP_INFO_FREEBLOCKS];
} amssheap_info_s_type;

/* Structure used to compare address ranges */
typedef struct amssheap_addr_cmp_s
{
  void *start;
  void *end;
} amssheap_addr_cmp_s_type;

#define MALLOC_NOT_READY 0
#define MALLOC_INITIALIZING 1
#define MALLOC_READY 2
static uint32 amssheap_init_flag = MALLOC_NOT_READY;

/* This is for random number. Alignment is needed for the purpose
   of flushing and invalidating cache, which needs to be done before and after making scm calls */
static uint8 rand_prng[32] __attribute__ ((aligned(32)));


extern dword magic_num[];
extern uint16 magic_num_index;
extern uint16 magic_num_index_array[];
extern uint16 magic_num_index;


extern void mem_heap_get_random_num(void*  random_ptr, int random_len);

/*============================================================================
                             GLOBAL VARIABLES
============================================================================*/

/*============================================================================
                             LOCAL VARIABLES
============================================================================*/

static uint8 mem_for_crit_sec[MEMHEAP_CRIT_SEC_SIZE ];

#if !defined(FEATURE_BLAST) && !defined(FEATURE_QURT) && !defined(FEATURE_L4) && !defined(FEATURE_UTILS_MEMHEAP_NOSTATIC_INIT)

/* really should define in SConscript so it can be updated in configuration */
#define MEMHEAP_HEAP_SIZE 0x300000

static uint8 amss_malloc_buffer[MEMHEAP_HEAP_SIZE];

#endif


mem_heap_type amss_mem_heap;

/* Current index into the array used to log heap allocation failures.
 * This index will continue to be incremented even when the array is full,
 * to serve as a total failure counter.
 */
int amssheap_fail_index = 0;

/* Array used to log heap allocation failures */
amssheap_info_s_type amssheap_fail_info[AMSSHEAP_FAIL_ARRAY_SIZE];

/*============================================================================
                             MACRO DEFINITIONS
============================================================================*/


/*============================================================================
                             FORWARD DECLARATIONS
============================================================================*/

void amssheap_malloc_failed
(
  struct mem_heap_struct *heap_ptr, /* Heap on which allocation is attempted */
  size_t size                       /* Size of failed request */
);

#ifdef FEATURE_MEM_DEBUG
/* AMSS memory debug functions */
extern void memdebug_init(void);
extern void memdebug_failure_handler(void);
extern void memdebug_updatecallerptr(void *ptr,void *caller_ptr, size_t size);
#endif  /* FEATURE_MEM_DEBUG */

extern void mem_heap_init_lock_free_fns(mem_heap_type *heap_ptr);


extern void memheap_addNodeToBin(mem_heap_type *heap_ptr, void *freeBlock);

extern void init_binToUSeForMalloc(
  mem_heap_type             *heap_ptr, 
  mem_block_header_type     *mem_block
);

#ifdef FEATURE_UTILS_ENABLE_AMSS_MALLOC_INIT
void amss_malloc_init
#else
static void amss_malloc_init
#endif
(
  void *start_heap,
  void *end_heap
);

#ifdef FEATURE_ENABLE_HEAP_PROFILING
/*===========================================================================
FUNCTION print_memheap_profile_table

DESCRIPTION
   This function will print the profiling numbers for all of the memheap users
   
   QXDM CMD : send_data 0x4B 0x49 0x00 0x00 0x15 0x00

===========================================================================*/
void print_memheap_profile_table(void){

   MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"memheap2-Profiling numbers  malloc_in_out_smallest_diff=%d malloc_in_out_biggest_diff:=%d",
               malloc_in_out_smallest_diff, malloc_in_out_biggest_diff);

  {
   int i;
   for(i = 0; (i < memheap_profile_table_index) && (memheap_profile_table[i].heap_ptr) ; i++){
    
      mem_heap_type * temp = (mem_heap_type *)(memheap_profile_table[i].heap_ptr);

      MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"----------------------Next Heap-----------------------");
            
      {
         MSG_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"heapNo.=%d -heap_ptr:=0x%x  ..heap_size:=0x%x",
            i, memheap_profile_table[i].heap_ptr, 
            ((mem_heap_type *)(memheap_profile_table[i].heap_ptr))->total_bytes);

         MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"malloc_avg = %d     malloc_cnt = %d ",
            memheap_profile_table[i].malloc_avg, memheap_profile_table[i].malloc_cnt);

         MSG_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"free_avg: = %d   free_cnt = %d   realloc_avg = %d",
            memheap_profile_table[i].free_avg, memheap_profile_table[i].free_cnt, 
            memheap_profile_table[i].realloc_avg);

         MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"used_bytes = 0x%x  total_blocks = %d",temp->used_bytes,
              temp->total_blocks);

         if(temp->legacy_check & kUseBINsImpl)
         {
           unsigned long binNo = MEMHEAP2_BIN_COUNT - 1;

           while(!(temp->BIN[binNo]).lastNode && binNo >= 0)
              binNo--;

           MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Biggest Availabe FreeBlock size = 0x%x ",
              (((temp->BIN[binNo]).lastNode)->freeBlock).forw_offset);

         }
      }
   }
  }

}


void init_heap_debug(mem_heap_type *heap_ptr){
  heap_ptr->debug_heap_index = memheap_profile_table_index;
  memheap_profile_table[heap_ptr->debug_heap_index].heap_ptr = (void*)heap_ptr;
  memheap_profile_table_index++;
}

#endif /*FEATURE_ENABLE_HEAP_PROFILING*/

void amss_heap_init(void *start_heap, uint32 size)
{
  uint16 * pblk = NULL;
  char *memory_end_ptr;
  unsigned long chunks;
  mem_block_header_type *blk_ptr;
  char *memory_start_ptr = (char *)start_heap;

  memory_end_ptr = memory_start_ptr + size;
  while( (((unsigned long)memory_start_ptr) & 0x000FUL) )
  { 
     ++memory_start_ptr;
  }

  chunks = (unsigned long) ((memory_end_ptr - memory_start_ptr) / kMinChunkSize);

  amss_mem_heap.first_block       = (mem_block_header_type *) memory_start_ptr;
  amss_mem_heap.next_block        = amss_mem_heap.first_block;
  amss_mem_heap.total_blocks      = 1;
  amss_mem_heap.max_used          = 0;
  amss_mem_heap.max_request       = 0;
  amss_mem_heap.used_bytes        = 0;
  amss_mem_heap.total_bytes       = chunks * kMinChunkSize;
  amss_mem_heap.fail_fnc_ptr      = amssheap_malloc_failed;
  amss_mem_heap.memheap_crit_sect = &mem_for_crit_sec;
  amss_mem_heap.legacy_check = kUseBINsImpl;
  
  blk_ptr            = (mem_block_header_type *)memory_start_ptr;
  blk_ptr->free_flag = (char)0x1;
  blk_ptr->last_flag = (char)0x1;
  blk_ptr->forw_offset = chunks * kMinChunkSize;
  blk_ptr->extra = 0;
  ((freeBlockList*)blk_ptr)->nextPtr = NULL;

  /* Making a single call to get random number of 8 bytes */
  mem_heap_get_random_num(rand_prng, 8);

  /* Copying the random numbers one by one, 4 + 2 +2 bytes */
  memscpy(((uint8*)(&(magic_num[magic_num_index_array[magic_num_index]]))), 4, rand_prng, 4);
  memscpy(((uint8*)(&(amss_mem_heap.magic_num_free))), 2, (uint8*)((uint32)rand_prng + 4), 2);
  memscpy(((uint8*)(&(amss_mem_heap.magic_num_used))), 2, (uint8*)((uint32)rand_prng + 6), 2);

  amss_mem_heap.magic_num = magic_num[magic_num_index_array[magic_num_index]];
  amss_mem_heap.magic_num_index = magic_num_index_array[magic_num_index];
  magic_num_index++;

  init_binToUSeForMalloc(&amss_mem_heap, amss_mem_heap.first_block);
  memheap_addNodeToBin(&amss_mem_heap, amss_mem_heap.first_block); 
  
  pblk = (uint16*)blk_ptr;
  ADD_GUARD_BYTES_TO_FREE_HEADER(amss_mem_heap.magic_num_free, pblk);



#ifdef FEATURE_ENABLE_HEAP_PROFILING
  init_heap_debug(&amss_mem_heap);
#endif
}


/*===========================================================================

FUNCTION malloc

DESCRIPTION
  Allocates from the AMSS heap

DEPENDENCIES
  None

RETURN VALUE
  A a pointer to the newly allocated block, or NULL if the block
  could not be allocated.

SIDE EFFECTS
  Replaces C Std Library implementation.

ARGUMENTS
  size  - Size of allocation request.

===========================================================================*/

#ifdef FEATURE_L4_NOT_PRESENT
extern void L4_get_user_heap_data(unsigned long *base, unsigned long *size);
#endif

#ifdef AMSSHEAP_UNIT_TEST
void *malloc_ut
(
  size_t size
)
#else
void *malloc
(
  size_t size
)
#endif
{
  void *ptr = NULL;
#ifndef FEATURE_UTILS_ENABLE_AMSS_MALLOC_INIT
  if(amssheap_init_flag == MALLOC_NOT_READY) 
  {
    
#if defined(FEATURE_BLAST) || defined(FEATURE_QURT) || defined(FEATURE_L4)
    void *heap_start;
    void *heap_end;
#ifdef FEATURE_QDSP6
#ifdef FEATURE_QURT
    int ret;
    qurt_sysenv_app_heap_t heap_obj;
    ret = qurt_sysenv_get_app_heap(&heap_obj);
    ASSERT(ret == QURT_EOK);

    heap_start = (void *)heap_obj.heap_base;
    heap_end = (void *)heap_obj.heap_limit;
    memset(heap_start, 0, (uint32)heap_end-(uint32)heap_start);

#else
#ifdef FEATURE_BLAST
    int ret;
    blast_app_heap_obj_t heap_obj;
    ret = blast_get_env_object(BLAST_ENV_OBJ_APP_HEAP, (unsigned int *)&heap_obj);
    ASSERT(ret == BLAST_EOK);

    heap_start = (void *)heap_obj.heap_base;
    heap_end = (void *)heap_obj.heap_limit;
#endif
#endif
#endif

#ifdef FEATURE_L4_NOT_PRESENT
    unsigned long heap_base;
    unsigned long heap_size;

    L4_get_user_heap_data(&heap_base, &heap_size);
    heap_start = (void*)heap_base;
    heap_end = (void*)(heap_base + heap_size - 1);
#endif

	/* Get the heap memory from Qurt */
    int ret;
    qurt_sysenv_app_heap_t heap_obj;
    ret = qurt_sysenv_get_app_heap(&heap_obj);
    ASSERT(ret == QURT_EOK);

    heap_start = (void *)heap_obj.heap_base;
    heap_end = (void *)heap_obj.heap_limit;
    memset(heap_start, 0, (uint32)heap_end-(uint32)heap_start);

    amss_malloc_init(heap_start, heap_end);

#else 
  // lets make this static implementation optional as it is not needed for RIVA, riva directly calls amss_malloc_init
#ifndef FEATURE_UTILS_MEMHEAP_NOSTATIC_INIT  
    amss_malloc_init((&amss_malloc_buffer[0]), (void *)(&amss_malloc_buffer[0] +sizeof(amss_malloc_buffer)-1));
#endif
#endif /* defined(FEATURE_BLAST) || defined(FEATURE_QURT) || defined(FEATURE_L4) */


  }
#endif /*FEATURE_UTILS_ENABLE_AMSS_MALLOC_INIT*/

  {
    #ifdef FEATURE_ENABLE_HEAP_PROFILING
    uint32 malloc_inTime=0, malloc_outTime=0, diff =0;
    start_profiling++;
    if(start_profiling > PROFILE_START_POINT){
       malloc_inTime = timetick_get_safe();
    }
    #endif

    ptr = mem_malloc( &amss_mem_heap, size );

	#ifdef FEATURE_ENABLE_HEAP_PROFILING
  if(start_profiling > PROFILE_START_POINT){
    malloc_outTime = timetick_get_safe();
    diff = malloc_outTime - malloc_inTime;
    memheap_profile_table[amss_mem_heap.debug_heap_index].malloc_avg+= diff;
    memheap_profile_table[amss_mem_heap.debug_heap_index].malloc_cnt++;

    if (diff > malloc_in_out_biggest_diff)
      malloc_in_out_biggest_diff = diff;

      if (diff < malloc_in_out_smallest_diff)
        malloc_in_out_smallest_diff = diff;
  }
  #endif
  }
#ifdef FEATURE_MEM_DEBUG
  {
    void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(ptr, caller_ptr,size);
  }
#endif
  return ptr;
}

void *amssheap_malloc
(
  size_t size,
  void *caller_ptr
)
{
   void *ptr=malloc(size);
#ifdef FEATURE_MEM_DEBUG
   memdebug_updatecallerptr(ptr,caller_ptr,size);
#endif
   return ptr;

}

/*===========================================================================

FUNCTION realloc

DESCRIPTION
  Reallocates from the AMSS heap

DEPENDENCIES
  None

RETURN VALUE
  A a pointer to the newly allocated block, or NULL if the block
  could not be allocated.

SIDE EFFECTS
  None

ARGUMENTS
  size  - Size of allocation request.

===========================================================================*/
#ifdef AMSSHEAP_UNIT_TEST
void *realloc_ut
(
  void   *ptr,
  size_t size
)
#else
void *realloc
(
  void   *ptr,
  size_t size
)
#endif
{

  #ifdef FEATURE_ENABLE_HEAP_PROFILING
  uint32 realloc_inTime=0 , realloc_outTime=0;
  if(start_profiling > PROFILE_START_POINT){
    realloc_inTime = timetick_get_safe();
  }
  #endif
  
  ptr = mem_realloc( &amss_mem_heap, ptr, size );
  
  #ifdef FEATURE_ENABLE_HEAP_PROFILING
  if(start_profiling > PROFILE_START_POINT){
    realloc_outTime = timetick_get_safe( );
    memheap_profile_table[amss_mem_heap.debug_heap_index].realloc_avg = realloc_outTime - realloc_inTime;
    memheap_profile_table[amss_mem_heap.debug_heap_index].realloc_cnt++;
  }
  #endif

#ifdef FEATURE_MEM_DEBUG
    {
      void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
      memdebug_updatecallerptr(ptr, caller_ptr,size);
    }
#endif
  return ptr;
}


/*===========================================================================

FUNCTION calloc

DESCRIPTION
  Callocates from the AMSS heap

DEPENDENCIES
  None

RETURN VALUE
  A a pointer to the newly allocated block, or NULL if the block
  could not be allocated.

SIDE EFFECTS
  Replaces C Std Library implementation.

ARGUMENTS
  size  - Size of allocation request.

===========================================================================*/
#ifdef AMSSHEAP_UNIT_TEST
void *calloc_ut
(
  size_t count,
  size_t size
)
#else
void *calloc
(
  size_t count,
  size_t size
)
#endif
{
  void *ptr;
  ptr = mem_calloc( &amss_mem_heap, count, size );
#ifdef FEATURE_MEM_DEBUG
  {
    void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(ptr, caller_ptr,count * size);
  }
#endif
  return ptr;
}

/*===========================================================================

FUNCTION free

DESCRIPTION
  Deallocates a block of memory and returns it to the heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Replaces C Std Library implementation.

ARGUMENTS
  ptr - A pointer to the memory block that needs to be deallocated.

===========================================================================*/
#ifdef AMSSHEAP_UNIT_TEST
void free_ut
(
  void *ptr
)
#else
void free
(
  void *ptr
)
#endif
{
  #ifdef FEATURE_ENABLE_HEAP_PROFILING
  uint32 free_inTime=0, free_outTime=0;
  if(start_profiling > PROFILE_START_POINT){
    free_inTime = timetick_get_safe();
  }
  #endif
  
  mem_free( &amss_mem_heap, ptr );
  
  #ifdef FEATURE_ENABLE_HEAP_PROFILING
  if(start_profiling > PROFILE_START_POINT){
      free_outTime = timetick_get_safe();
      memheap_profile_table[amss_mem_heap.debug_heap_index].free_avg += (free_outTime - free_inTime);
      memheap_profile_table[amss_mem_heap.debug_heap_index].free_cnt++;
    }
  #endif /*FEATURE_ENABLE_HEAP_PROFILING*/
}


/*===========================================================================

FUNCTION amssheap_malloc_failed

DESCRIPTION
  This function logs heap statistics, and is called when memheap detects
  an allocation failure from the AMSS heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void amssheap_malloc_failed
(
  struct mem_heap_struct *heap_ptr, /* Heap on which allocation is attempted */
  size_t size                       /* Size of failed request */
)
{
  amssheap_info_s_type *info;

#ifdef FEATURE_MEM_DEBUG
  //memdebug_failure_handler();
#endif

  /* Record the current heap state */
  if (amssheap_fail_index < ARR_SIZE(amssheap_fail_info))
  {
    info = &amssheap_fail_info[amssheap_fail_index];
    info->heap_ptr = heap_ptr;
    info->size = size;
    mem_heap_get_totals(heap_ptr, &info->totals);
    mem_heap_get_freeblock_info(heap_ptr, AMSSHEAP_INFO_FREEBLOCKS,
      info->freeblocks);
  }
  amssheap_fail_index++;

  MSG_2(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"AMSS heap failed to allocate %d bytes (failure %d)",
    size, amssheap_fail_index);
}



/*===========================================================================

FUNCTION amss_malloc_init

DESCRIPTION
  Initializes the AMSS heap

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

ARGUMENTS
  start_heap - Pointer to the beginning of the heap.
  end_heap   - Pointer to the end of the heap.

===========================================================================*/
#ifdef FEATURE_UTILS_ENABLE_AMSS_MALLOC_INIT
void amss_malloc_init
#else
static void amss_malloc_init
#endif
(
  void *start_heap,
  void *end_heap
)
{
  uint32 size;
  int ret_value;
  /* Clear the heap control struct in case of failure */
  memset(&amss_mem_heap, 0, sizeof(amss_mem_heap));

  size = (uint32)end_heap - (uint32)start_heap;

  if (start_heap == NULL || size == 0)
  {
    for ( ; ; )   /* Cannot fatal at this point in boot */
    {
    }
  } 
  amssheap_init_flag = MALLOC_INITIALIZING;
  amss_heap_init(start_heap, size);
  ret_value = osal_init_crit_sect((osal_crit_sect_t*)((amss_mem_heap.memheap_crit_sect)));
  if(ret_value != OSAL_SUCCESS)
  {
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH," Failed:osal_init_crit_sect ret_value = %d ",ret_value);
  }else
  {
    amssheap_init_flag = MALLOC_READY;
  }

  mem_heap_init_lock_free_fns(&amss_mem_heap);

  /* This function will force memheap to use int locks for protection
   * But some times the heap APIs can take longer than usual time
   * causing the interrupts to be disabled which 
   * would indefinitely increase interrupt latency, so commented.
   * By default Memheap uses Critical Section for protection/synchronization
   */  
  /* mem_heap_set_int_lock( &amss_mem_heap ); */

#ifdef FEATURE_MEM_DEBUG
  /* Init AMSS memory debug module */
  memdebug_init();
#endif
}


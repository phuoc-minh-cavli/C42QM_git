/* =========================================================================

DESCRIPTION
  Implementation of a simple sub-allocator to manage memory allocations
  and deallocations using a Next Fit strategy.  

Copyright (c) 1997-2014,2016-2017, 2019 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/memheap.c_v   1.2   22 Mar 2002 16:54:42   rajeevg  $
$Header: //components/rel/core.tx/6.0/services/utils/src/memheap.c#2 $ $DateTime: 2020/05/18 06:06:25 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
26/10/16   nk      Use qurt apis instead of osal apis
04/03/14   psu     Update diag macros using optimized versions.
12/05/12   mg      CR413140 klockwork warning fix
11/26/12   ps      CR424310 Add guard bytes in mem_realloc
08/30/12   rks     robust XOR logic and enable PRNG uses
17/05/12   rks     fix for CR362308
013/04/12  rks     memheap2 
09/03/12   rks     Use OSAL for OS abstraction and remove OS specific code/featurization
08/04/010  sri     Added checks for size variable overflow in mem_malloc()
08/04/009  sri     Added NULL checks where ever applicable
06/01/009  sri     Avoid compiler warnings based on MEMHEAP_ASSERT_NONE
04/14/009  sri     Replaced sprintf, strcat with snprintf, std_strlcat 
                   respectively, memheap_write_data_to_file() is changed to 
                   return void.
03/31/009  sri     qmutex attribute updated for qmutex_create()
09/01/004   gr     Deleted the context argument to the debug versions of
                   the memheap functions; the context is no longer used.
04/06/04    gr     Fixed a bug in the definition of the BEGIN_CRITICAL_SECTION
                   and END_CRITICAL_SECTION macros.
11/25/03    gr     Fixes for compiler warnings when FEATURE_MEM_DEBUG is
                   turned on.
11/18/03    rj     Added check for mem_malloc success before saving header 
                   info.
11/07/03    gr     More debug support.
09/23/03    gr     Added functions to retrieve information about a heap.
09/19/03    gr     Added functionality to store information in flash when
                   an ASSERT fails.
09/17/03    gr     Mainlined FEATURE_HEAP_DEBUG. Added support for storing a
                   filename hash and a line number in the block header.
07/08/03    gr     Simplified mem_heap_task_lock and mem_heap_task_free.
06/26/03    gr     Renamed ASSERT to MEMHEAP_ASSERT. By default, MEMHEAP_ASSERT
                   throws a fatal error and prints out the task id.
06/25/03    gr     Added more ASSERTS. Changed the behavior of
                   FEATURE_MEM_DEBUG to keep just the last n stores and
                   frees. The id of the allocating task is now stored in
                   the info array. Set the block header pad to a non-zero
                   value to try and catch heap corruption.
06/21/03    gr     Added ASSERTS in mem_realloc and mem_free to catch bad
                   pointers passed in.
06/03/03   kjm     Fixed problem in realloc when block is actually grown.
                   If nextBlock is pointing to the free block merged onto
                   the end of the grown block, nextBlock was failing to
                   get updated to point to the following block.
03/11/03    gr     Added support for a heap-specific locking mechanism. This
                   allows some heaps to be restricted to task-context use,
                   and others to be usable from ISRs.
01/20/03   lad     Changed critical section macros to use INTLOCK() instead
                   of TASKLOCK().  Featurized ASSERT calls to optimize.
03/22/02    gr     Corrected a bug in mem_malloc.
10/10/01    gr     BEGIN_CRITICAL_SECTION and END_CRITICAL_SECTION now map
                   to TASKLOCK and TASKFREE, instead of to rex_enter_crit_sect
                   and rex_leave_crit_sect.
10/01/01    gr     Added support for some basic heap statistics.
11/09/00   jct     Revised for coding standard, integrate with REX critical
                   sections
09/09/99   kjm     Added even more ASSERTs to detect corrupt heaps.
04/22/99   kjm     mem_realloc now correctly updates used_bytes, max_used and
                     max_request.
02/17/99   kjm     Added realloc function.  Added support for two
                     variations of heap block headers.  Support backing up
                     and auto-reset when last block is freed to reduce
                     fragmentation.
11/24/98   kjm     Ported to C-only code for use in phone software from
                     original SubAllocator.cp file.
08/22/97   kjm     Original SubAllocator.cp file created.
============================================================================ */


/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "memheap.h"
#include <stdio.h>
#include <string.h>
#include "err.h"
#include <stringl/stringl.h>


#include "qurt_memory.h"
#include "qurt_error.h"
#include "scm.h"


#ifdef FEATURE_MEM_DEBUG
  #include "crc.h"
#endif

#include "msg.h"
#include "osal.h"
#ifdef FEATURE_WINCE
  #define MEMHEAP_ASSERT_WARN
#endif /* FEATURE_WINCE */



/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */
/* These are for random number generation */
#define TZ_PRNG_GETDATA_ID_PARAM_ID          0x00000022
#define TZ_PRNG_GETDATA_ID                   0x02000A01
#define TZ_SVC_CRYPTO                        0x0000000A
#define TZ_PRNG_GETDATA_ID_QSEEv2_5          0x00002801 /*TZ_SYSCALL_CREATE_CMD_ID(TZ_SVC_CRYPTO, 0x01)=TZ_SVC_CRYPTO<<10+0x01*/

#define OVERFLOW_CHECK(elt_count, elt_size) (!(elt_count >= (1U<<10) || elt_size >= (1U<<22)) || ((((uint64)elt_count * (uint64)elt_size) >> 32) == 0))

#define BOUNDARY_CHECK(theBlock, heap_ptr) ((theBlock >= ((mem_heap_type*)heap_ptr)->first_block) && (theBlock < (((mem_heap_type*)heap_ptr)->first_block + heap_ptr->total_bytes)))

#define FRD_OFFSET_CHECK(block, heap_ptr) ((((mem_block_header_type *)block)->forw_offset + (char *)block) <= ((((char*)((mem_heap_type*)heap_ptr)->first_block) + ((mem_heap_type*)heap_ptr)->total_bytes)))

/* XOR based Guard byte calculations and restore */
#define INTEGRITY_CHECK_ON_USED_HEADER(magic_num_used, block) \
            (block[0]^block[1]^block[2]^block[3]^magic_num_used)


#define INTEGRITY_CHECK_ON_FREE_HEADER(magic_num_free, block) \
            (block[0]^block[1]^block[2]^block[3]^block[4]^block[5]^magic_num_free)


#define ADD_GUARD_BYTES_TO_USED_HEADER(magic_num_used, block) \
            (block[0] = block[1]^block[2]^block[3]^magic_num_used)


#define ADD_GUARD_BYTES_TO_FREE_HEADER(magic_num_free, block) \
            (block[0] = block[1]^block[2]^block[3]^block[4]^block[5]^magic_num_free)

#define MEMHEAP_MIN_BLOCK_SIZE 16
#define MIN_HEAP_SIZE_FOR_BINS 0x20000

#define MEMHEAP2_BINSIZE_OVERHEAD (sizeof(mem_block_header_type))

#define MEMHEAP2_BINSIZE0         (0x00000010)
#define MEMHEAP2_BINSIZE1         (0x00000020)
#define MEMHEAP2_BINSIZE2         (0x00000030)
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

/*
 scm_cmd_buf_t struct
 
 */
typedef struct {
  uint32 addr;
  uint32 size;
  uint32 resp_addr;
  uint32 resp_size;
} scm_cmd_buf_t;


#define MAX_HEAP_INIT 31
uint32 magic_num[MAX_HEAP_INIT] = {(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1};
uint16 magic_num_index_array[MAX_HEAP_INIT] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
uint16 magic_num_index = 0;


extern mem_heap_type amss_mem_heap;


#define NEXT_BLOCK_SIZE(temp) ((mem_block_header_type*)(temp->nextPtr))->forw_offset

/* NOTUSED */
#define NOTUSED(i) if(i){}

#ifdef FEATURE_QUBE
#undef os_is_in_irq_mode
#define os_is_in_irq_mode()  FALSE
/*lint -emacro(506, MEMHEAP_ASSERT) */
/*lint -emacro(506, MEMHEAP_ASSERT_EX) */
#endif

#define TCB_TYPE     qurt_thread_t
#define TCB_SELF()   qurt_thread_get_id()

/* By default, MEMHEAP_ASSERT will generate a fatal error.
*/
#if defined( MEMHEAP_ASSERT_WARN )

   #ifndef FEATURE_WINCE

   #define MEMHEAP_ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR( "In task 0x%x, Assertion " #xx_exp " failed", \
           (unsigned long) TCB_SELF(), 0, 0 ); \
     }

   #else /* FEATURE_WINCE */

     #define MEMHEAP_ASSERT( xx_exp ) \
       if( !(xx_exp) ) \
       { \
          ERR( "In task 0x%x, Assertion  %d failed", \
             (unsigned long) TCB_SELF(), xx_exp , 0 ); \
       }

   #endif /* FEATURE_WINCE */

   #define MEMHEAP_ASSERT_EX( xx_exp, xx_file, xx_line ) \
      MEMHEAP_ASSERT( xx_exp )
#else

   #define MEMHEAP_ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        TCB_TYPE  self = TCB_SELF();                             \
        ERR_FATAL( "In task 0x%x, Assertion " #xx_exp " failed", \
                   (unsigned long) self, 0, 0 );                 \
     }

   #define MEMHEAP_ASSERT_EX( xx_exp, xx_file, xx_line ) \
     if( !(xx_exp) ) \
     { \
        TCB_TYPE  self = TCB_SELF();                             \
        ERR_FATAL( "In task 0x%x, Assertion " #xx_exp " failed", \
                   (unsigned long) self, 0, 0 );                 \
     }

#endif

/* Code to enter and exit critical sections.
*/
   #define BEGIN_CRITICAL_SECTION(heap) \
     do { \
       if ((heap)->lock_fnc_ptr) \
         (heap)->lock_fnc_ptr(heap); \
     } while (0)
   #define END_CRITICAL_SECTION(heap) \
     do { \
       if ((heap)->free_fnc_ptr) \
         (heap)->free_fnc_ptr(heap); \
     } while (0)

/* Temporarily define the size of qurt task name string */
#define MEMHEAP_QURT_TASK_NAME_LEN  QURT_THREAD_ATTR_NAME_MAXLEN

mem_allocator_failed_proc_type mem_allocator_failed_hook = NULL;

static void mem_init_block_header(mem_block_header_type *, unsigned long, mem_heap_type *hep_ptr);
static mem_block_header_type *mem_find_free_block(mem_heap_type*,unsigned long);

#ifdef FEATURE_ENABLE_HEAP_PROFILING
extern void init_heap_debug(mem_heap_type *heap_ptr);
#endif

#ifdef FEATURE_MEM_DEBUG
extern void memdebug_malloc
(
  size_t size,      /* Size of allocation request (bytes) */
  void *caller_ptr, /* Pointer to caller */
  void *ptr         /* Pointer to allocated memory block */
);
extern void memdebug_free
(
  void *ptr,         /* Pointer to allocated memory block */
  uint32 db_index   /* index of the db*/
);
extern void memdebug_malloc_updateparams
(
  size_t size,      /* Size of allocation request (bytes) */
  void *caller_ptr, /* Pointer to caller */
  void *ptr         /* Pointer to allocated memory block */
);
#endif

#ifdef FEATURE_MEM_DEBUG_EX
   #define MEM_INFO_ARRAY_SIZE    1000
   typedef struct {
      void          *mem_ptr;
      unsigned long  size;
      unsigned long  task_id;
      unsigned char  freed;
   } mem_info_type;
   static mem_info_type mem_info_array[ MEM_INFO_ARRAY_SIZE ] = { 0 };
#endif

#ifdef FEATURE_MEM_DEBUG

#ifndef MEM_HEAP_CALLER_ADDRESS_LEVEL
#define MEM_HEAP_CALLER_ADDRESS_LEVEL 0
#endif

#if defined(FEATURE_QDSP6)
 #define  MEM_HEAP_CALLER_ADDRESS(level) ((void *)__builtin_return_address(level));
#else
 #define MEM_HEAP_CALLER_ADDRESS(level) ((void *) __return_address())
#endif

#define MEM_INFO_FREE_ARRAY_SIZE    1000
typedef struct {
  void          *heap_ptr;
  void          *mem_ptr;
  unsigned long  task_id;
  void          *caller;
} mem_info_free_type;
mem_info_free_type mem_info_free_array[ MEM_INFO_FREE_ARRAY_SIZE ];
static int memheap_free_array_cnt=0;
#define MEMHEAP_DEBUG_MODE_PAD_VALUE 0xABCDABCD
#endif

/* Lock function for Memheap.
*/
static void
mem_heap_enter_crit_sect( void * ptr)
{
  int ret_value;
  ret_value = osal_enter_crit_sect((osal_crit_sect_t*) (((mem_heap_type *)ptr)->memheap_crit_sect));
  MEMHEAP_ASSERT( ret_value == OSAL_SUCCESS );
} /* END mem_heap_enter_crit_sect */

/* Matching free function for mem_heap_lock_mutex().
*/
static void
mem_heap_leave_crit_sect( void * ptr)
{
  int ret_value;
  ret_value = osal_exit_crit_sect((osal_crit_sect_t*) (((mem_heap_type *)ptr)->memheap_crit_sect));
  MEMHEAP_ASSERT( ret_value == OSAL_SUCCESS );
} /* END mem_heap_leave_crit_sect */

#ifndef FEATURE_QUBE
/* Lock function that works by locking interrupts. A heap that uses this
** lock function can be used by both ISRs and tasks.
*/
static void
mem_heap_int_lock( void * ptr)
{
    //NOTUSED(ptr);
    //INTLOCK();
  int ret_value;
  ret_value = osal_enter_crit_sect((osal_crit_sect_t*) (((mem_heap_type *)ptr)->memheap_crit_sect));
  MEMHEAP_ASSERT( ret_value == OSAL_SUCCESS );    
} /* END mem_heap_int_lock */

/* Matching free function for mem_heap_int_lock().
*/
static void
mem_heap_int_free( void * ptr )
{
    //NOTUSED(ptr);
    //INTFREE();
	int ret_value;
	ret_value = osal_exit_crit_sect((osal_crit_sect_t*) (((mem_heap_type *)ptr)->memheap_crit_sect));
	MEMHEAP_ASSERT( ret_value == OSAL_SUCCESS );	
} /* END mem_heap_int_free */


/* Lock function that works by disabling task switching. A heap that uses
** this lock function can only be used in task context.
**Need to have this function for legacy reasons
*/
static void
mem_heap_task_lock( void * ptr )
{
   NOTUSED(ptr);
} /* END mem_heap_task_lock */

/* Matching free function for mem_heap_task_lock().
**Need to have this function for legacy reasons
*/
static void
mem_heap_task_free( void * ptr )
{
   NOTUSED(ptr);
} /* END mem_heap_task_free */
#endif /* FEATURE_QUBE */

#ifdef FEATURE_QUBE
/*lint -emacro(506,mem_heap_callable_by_isr) */
#define mem_heap_callable_by_isr(h)  FALSE
#else /* FEATURE_QUBE */
static int
mem_heap_callable_by_isr( mem_heap_type *heap_ptr )
{
   if (heap_ptr->lock_fnc_ptr == NULL ||
       heap_ptr->lock_fnc_ptr == mem_heap_int_lock)
      return 1;
   else
      return 0;
} /* END mem_heap_callable_by_isr */
#endif /* FEATURE_QUBE */


/*===========================================================================
FUNCTION mem_heap_get_random_num
DESCRIPTION

===========================================================================*/
void mem_heap_get_random_num(void*  random_ptr, int random_len)
{


   scm_desc_t desc;
   scm_cmd_buf_t prng_data;
   int ret;
   
   if(random_ptr==NULL)
   {
     ERR_FATAL("Buffer is NULL", 0, 0, 0);
   }

   if(!scm_is_armv8())
   {
     /* to support backward compatibility for QSEEv2.5 version */
     memset(&prng_data, 0x0, sizeof(scm_cmd_buf_t));
     prng_data.addr = qurt_lookup_physaddr((qurt_addr_t)random_ptr);
     prng_data.size = random_len;
   }
   else
   {
     /* to support QSEEv4 version */
     memset(&desc, 0x0, sizeof(scm_desc_t));
     desc.arginfo = TZ_PRNG_GETDATA_ID_PARAM_ID;
     desc.args[0] = qurt_lookup_physaddr((qurt_addr_t)random_ptr); //physical address
     desc.args[1] = random_len;
   }
   // Invalidate cache on buffer so we read correct random data from DDR
   // Need to do this first because ARM upgrades invalidates to flushes if line is dirty
   ret = qurt_mem_cache_clean((qurt_addr_t)random_ptr,random_len,QURT_MEM_CACHE_FLUSH,QURT_MEM_DCACHE);
   if(ret!=QURT_EOK)
   {
     ERR_FATAL("Cache could not be cleaned", 0, 0, 0);
   }
	  
   /* Get random number */
   if(!scm_is_armv8())
   {
     ret = scm_call(TZ_SVC_CRYPTO,TZ_PRNG_GETDATA_ID_QSEEv2_5,(void*)&prng_data,sizeof(prng_data),NULL,0);
   }
   else
   {
     /* scm_call2 has a dependency on qurt_qtmr_timer_init, instead calling into scm_call2_STM */
     ret = scm_call2_STM(TZ_PRNG_GETDATA_ID, &desc);
   }

   /* If we get the random number, ret will be zero */
   if (ret)
   {
     ERR_FATAL("Cannot get random number, ret=%d", ret, 0, 0);
   }

   /* Invalidating the cache after scm call returns */
   ret = qurt_mem_cache_clean((qurt_addr_t)random_ptr,random_len,QURT_MEM_CACHE_INVALIDATE,QURT_MEM_DCACHE);
   if(ret!=QURT_EOK)
   {
     ERR_FATAL("Cache could not be cleaned", 0, 0, 0);
   }
}



/*===========================================================================
FUNCTION memheap_copy_frd_offset_at_end

DESCRIPTION
      This function will copy the forward offset of the memory block at the end
      of the block(which will be used later while doing defragmentation)
===========================================================================*/

static void memheap_copy_frd_offset_at_end(mem_block_header_type *mem_block, mem_heap_type *heap_ptr)
{
  frd_Offset_info *temp = NULL;
  // store the frd_offset at the last bytes in the free block
  temp = (frd_Offset_info *)((char*)mem_block + (mem_block->forw_offset - sizeof(frd_Offset_info)));
  if(mem_block->forw_offset != kMinChunkSize){
    temp->pad = magic_num[heap_ptr->magic_num_index];
  }
  temp->freeBlock_frdOff = mem_block->forw_offset;
}
/*===========================================================================
FUNCTION mem_heap_init_lock_free_fns

DESCRIPTION
     init the lock and free functions of the heap , its called from amssheap.c
===========================================================================*/
void mem_heap_init_lock_free_fns(mem_heap_type  *heap_ptr)
{

   MEMHEAP_ASSERT (heap_ptr != NULL);

   if(heap_ptr != NULL){
      heap_ptr->lock_fnc_ptr = mem_heap_enter_crit_sect;
      heap_ptr->free_fnc_ptr = mem_heap_leave_crit_sect;
  }

}

/*===========================================================================*/
/*!
    @brief
    Determines a bin number for the given value.

    @detail
    This function is effectivly an opened up binary search algorithm hard
    coded for 32 values, with the exception that it looks for the
    closest match without going over instead of the exact match.  This
    function is large but extremely fast, speed is much more important here
    than size.
    With the given value, it finds a bin number that this value
    should fall into. It does that by:
    (1) compare the value with the BIN in the center, therefore BIN15
        out of BIN0 to BIN31,
    (2) if the value is less than (or equal to) BIN15, it compares the value
        with the center bin with lower half of the bins, therefore, BIN7
        out of BIN0 to BIN15.
    (3) if the value is greater than BIN15 in the step (1), it compares
        the value with the center bin with higher half of the bins,
        therefore, BIN23 outof BIN16 to BIN31.
    (4) this process of binary search continues until it determines the
        bin number.

    @return
    bin number (0 - 31)
*/
/*=========================================================================*/
static unsigned long memheap_find_bin
(
  unsigned long value   /*!< value to be determined for bin assignment */
)
{
  /*! @brief bin number to return
  */
  unsigned long bin;

  /*-----------------------------------------------------------------------*/

  if (value <= MEMHEAP2_BINSIZE15)
  {
    if (value <= MEMHEAP2_BINSIZE7)
    {
      if (value <= MEMHEAP2_BINSIZE3)
      {
        if (value <= MEMHEAP2_BINSIZE1)
        {
          if (value <= MEMHEAP2_BINSIZE0)
          {
            bin = 0;
          }
          else /* value > MEMHEAP2_BINSIZE0 */
          {
            bin = 1;
          }
        }
        else   /* value > MEMHEAP2_BINSIZE1 */
        {
          if (value <= MEMHEAP2_BINSIZE2)
          {
            bin = 2;
          }
          else /* value > MEMHEAP2_BINSIZE2 */
          {
            bin = 3;
          }
        }
      }
      else     /* value > MEMHEAP2_BINSIZE3 */
      {
        if (value <= MEMHEAP2_BINSIZE5)
        {
          if (value <= MEMHEAP2_BINSIZE4)
          {
            bin = 4;
          }
          else /* value > MEMHEAP2_BINSIZE4 */
          {
            bin = 5;
          }
        }
        else   /* value > MEMHEAP2_BINSIZE5 */
        {
          if (value <= MEMHEAP2_BINSIZE6)
          {
            bin = 6;
          }
          else /* value > MEMHEAP2_BINSIZE6 */
          {
            bin = 7;
          }
        }
      }
    }
    else       /* value > MEMHEAP2_BINSIZE7 */
    {
      if (value <= MEMHEAP2_BINSIZE11)
      {
        if (value <= MEMHEAP2_BINSIZE9)
        {
          if (value <= MEMHEAP2_BINSIZE8)
          {
            bin = 8;
          }
          else /* value > MEMHEAP2_BINSIZE8 */
          {
            bin = 9;
          }
        }
        else   /* value > MEMHEAP2_BINSIZE9 */
        {
          if (value <= MEMHEAP2_BINSIZE10)
          {
            bin = 10;
          }
          else /* value > MEMHEAP2_BINSIZE10 */
          {
            bin = 11;
          }
        }
      }
      else     /* value > MEMHEAP2_BINSIZE11 */
      {
        if (value <= MEMHEAP2_BINSIZE13)
        {
          if (value <= MEMHEAP2_BINSIZE12)
          {
            bin = 12;
          }
          else /* value > MEMHEAP2_BINSIZE12 */
          {
            bin = 13;
          }
        }
        else   /* value > MEMHEAP2_BINSIZE13 */
        {
          if (value <= MEMHEAP2_BINSIZE14)
          {
            bin = 14;
          }
          else /* value > MEMHEAP2_BINSIZE14 */
          {
            bin = 15;
          }
        }
      }
    }
  }
  else         /* value > MEMHEAP2_BINSIZE15 */
  {
    if (value <= MEMHEAP2_BINSIZE23)
    {
      if (value <= MEMHEAP2_BINSIZE19)
      {
        if (value <= MEMHEAP2_BINSIZE17)
        {
          if (value <= MEMHEAP2_BINSIZE16)
          {
            bin = 16;
          }
          else /* value > MEMHEAP2_BINSIZE16 */
          {
            bin = 17;
          }
        }
        else   /* value > MEMHEAP2_BINSIZE17 */
        {
          if (value <= MEMHEAP2_BINSIZE18)
          {
            bin = 18;
          }
          else /* value > MEMHEAP2_BINSIZE18 */
          {
            bin = 19;
          }
        }
      }
      else     /* value > MEMHEAP2_BINSIZE19 */
      {
        if (value <= MEMHEAP2_BINSIZE21)
        {
          if (value <= MEMHEAP2_BINSIZE20)
          {
            bin = 20;
          }
          else /* value > MEMHEAP2_BINSIZE20 */
          {
            bin = 21;
          }
        }
        else   /* value > MEMHEAP2_BINSIZE21 */
        {
          if (value <= MEMHEAP2_BINSIZE22)
          {
            bin = 22;
          }
          else /* value > MEMHEAP2_BINSIZE22 */
          {
            bin = 23;
          }
        }
      }
    }
    else       /* value > MEMHEAP2_BINSIZE23 */
    {
      if (value <= MEMHEAP2_BINSIZE27)
      {
        if (value <= MEMHEAP2_BINSIZE25)
        {
          if (value <= MEMHEAP2_BINSIZE24)
          {
            bin = 24;
          }
          else /* value > MEMHEAP2_BINSIZE24 */
          {
            bin = 25;
          }
        }
        else   /* value > MEMHEAP2_BINSIZE25 */
        {
          if (value <= MEMHEAP2_BINSIZE26)
          {
            bin = 26;
          }
          else /* value > MEMHEAP2_BINSIZE26 */
          {
            bin = 27;
          }
        }
      }
      else     /* value > MEMHEAP2_BINSIZE27 */
      {
        if (value <= MEMHEAP2_BINSIZE29)
        {
          if (value <= MEMHEAP2_BINSIZE28)
          {
            bin = 28;
          }
          else /* value > MEMHEAP2_BINSIZE28 */
          {
            bin = 29;
          }
        }
        else   /* value > MEMHEAP2_BINSIZE29 */
        {
          if (value <= MEMHEAP2_BINSIZE30)
          {
            bin = 30;
          }
          else /* value > MEMHEAP2_BINSIZE30 */
          {
            bin = 31;
          }
        }
      }
    }
  }

  return bin;
} /* memheap_find_bin() */

/*===========================================================================
FUNCTION init_binToUSeForMalloc

DESCRIPTION
   this function will initialize all of the BINs in a heap to the default bin no.
   with the highest bin available in the heap at the time of mem_init_heap.
   So initially for malloc this is the bin its going to use.

===========================================================================*/
void init_binToUSeForMalloc(
  mem_heap_type             *heap_ptr,
  mem_block_header_type     *mem_block
)
{
  unsigned long bin = memheap_find_bin(mem_block->forw_offset);
  int i;
  for( i = bin; i >= 0 ; i-- )
  {
     heap_ptr->BIN[i].binToUSeForMalloc = bin;
  }
  /*intiate the remaining bins with 0s */
  /*if (bin < MEMHEAP2_BIN_COUNT -1 )
  {
     for( i = bin + 1; i >= MEMHEAP2_BIN_COUNT -1 ; i++ )
     {
       heap_ptr->BIN[bin + i].binToUSeForMalloc = 0;
       i++;
     }
  }*/
}
/*===========================================================================
FUNCTION bin_active

DESCRIPTION
   this function will update the binToUSeForMalloc of each applicable BIN of the heap,
   whenever new bin is active

===========================================================================*/
static void bin_active(
  mem_heap_type             *heap_ptr,
  unsigned long             binNo
)
{
  int i = binNo;
  /*do the first iteration for the newly active BIN as it has freeBlock (firstNode is not NULL)*/
  heap_ptr->BIN[i].binToUSeForMalloc = binNo;
  i--;
  while( i >= 0)
  {
    if( !(heap_ptr->BIN[i].firstNode) )
    {
      heap_ptr->BIN[i].binToUSeForMalloc = binNo;
    }
    else{
      break;
    }
    i--;
  }
}

/*===========================================================================
FUNCTION bin_deactive

DESCRIPTION
   this function will update the binToUSeForMalloc of each applicable BIN of the heap,
   whenever new bin is deactivate , means there is no free block that the bin holds
   and hence we cannot use the bin to allocate memory.

===========================================================================*/
static void bin_deactive(
  mem_heap_type             *heap_ptr,
  unsigned long             binNo
)
{
  int i = binNo;
  if((binNo+1) < MEMHEAP2_BIN_COUNT)  
  {
	while((i >= 0)&&(heap_ptr->BIN[i].binToUSeForMalloc == binNo)){    
      heap_ptr->BIN[i].binToUSeForMalloc = heap_ptr->BIN[binNo+1].binToUSeForMalloc;
      i--;
    }
  }
}

/*===========================================================================
FUNCTION memheap_addNodeToBin

DESCRIPTION:
     This function adds the free memory block into the apropriate bin

===========================================================================*/
void memheap_addNodeToBin(mem_heap_type *heap_ptr, void *freeBlock)
{

  unsigned long  binNo = memheap_find_bin(((mem_block_header_type*)freeBlock)->forw_offset);
  uint16 * pblk = NULL;

  binStruct *binToUpdate = &heap_ptr->BIN[binNo];

  if(binToUpdate->firstNode){
    freeBlockList tempFreeBlock = {{0},0};
    freeBlockList *temp;

    tempFreeBlock.nextPtr = binToUpdate->firstNode;
    temp = &tempFreeBlock;

    // search the linked list for the right location
    while((temp->nextPtr != NULL)
       && (NEXT_BLOCK_SIZE(temp) < ((mem_block_header_type*)freeBlock)->forw_offset)){
      /*sanity check for the free block in the list*/
      MEMHEAP_ASSERT(BOUNDARY_CHECK((mem_block_header_type*)(temp->nextPtr), heap_ptr));
      temp = temp->nextPtr;
    }

    if(&tempFreeBlock != temp)
    {
      pblk =  (uint16*)temp;
      MEMHEAP_ASSERT(!(INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk)));
    }

    if(temp->nextPtr == binToUpdate->firstNode){
     //its the firstNode in the BIN to be updated
      //((freeBlockList *)freeBlock)->nextPtr = binToUpdate->firstNode; //redundent
      binToUpdate->firstNode = (freeBlockList *)freeBlock;
    }
    if(!temp->nextPtr){
       // update the lastNode
       binToUpdate->lastNode = (freeBlockList *)freeBlock;
    }

    ((freeBlockList *)freeBlock)->nextPtr = temp->nextPtr;
    temp->nextPtr = (freeBlockList *)freeBlock;
    /*update the header_guard of the temp  since the next has  changed*/
    if(&tempFreeBlock != temp){
      pblk = (uint16*) temp;    
      ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
    }
    binToUpdate->totalNodes = binToUpdate->totalNodes + 1;
    return;
  }else{
    binToUpdate->firstNode = (freeBlockList *)freeBlock;
    binToUpdate->lastNode = (freeBlockList *)freeBlock;
    ((freeBlockList *)freeBlock)->nextPtr = NULL;
    binToUpdate->totalNodes = 1;
    bin_active(heap_ptr, binNo);
    return;
  }

}

/*===========================================================================
FUNCTION findNRemoveFreeBlockInBINs

DESCRIPTION:
     This function find a free memory block of requested size in case if
     size_needed is not NULL else if mem_ptr is not NULL then will search for the
     memory block in the bins list ,remove it from the list and return it

===========================================================================*/
static mem_block_header_type * findNRemoveFreeBlockInBINs(
  mem_heap_type *heap_ptr,
  void *mem_ptr,
  size_t size_needed
)
{
  mem_block_header_type *answer = NULL;
  uint16 * pblk= NULL;


  if(size_needed){
    unsigned long bin = memheap_find_bin(size_needed);
    binStruct *binToSearch;
    freeBlockList tempFreeBlock = {{0},0};
    freeBlockList *temp;

    bin = heap_ptr->BIN[bin].binToUSeForMalloc;
    binToSearch = &heap_ptr->BIN[bin];

    if(binToSearch->firstNode == NULL)
    {
       return NULL;
    }

    if(((binToSearch->lastNode)->freeBlock).forw_offset <  size_needed){
     if(bin < MEMHEAP2_BIN_COUNT - 1)
     {
	bin = heap_ptr->BIN[bin + 1].binToUSeForMalloc;
	binToSearch = &heap_ptr->BIN[bin];
     }
    }
    tempFreeBlock.nextPtr = binToSearch->firstNode;
    temp = &tempFreeBlock;

    while((temp->nextPtr != NULL) && (NEXT_BLOCK_SIZE(temp) < size_needed)){
     /*sanity check for the free block in the list*/
      MEMHEAP_ASSERT(BOUNDARY_CHECK((mem_block_header_type*)(temp->nextPtr), heap_ptr));
      temp = temp->nextPtr;
    }

    if(temp->nextPtr != NULL){

      answer = (mem_block_header_type*)(temp->nextPtr);
      /* Output sanitization */
      pblk = (uint16 *)answer;
      MEMHEAP_ASSERT(!(INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk)));
      if(answer == (mem_block_header_type*)(binToSearch->firstNode))
        binToSearch->firstNode = temp->nextPtr->nextPtr;

      if(answer == (mem_block_header_type*)(binToSearch->lastNode)){
        if((temp->freeBlock).forw_offset)
          binToSearch->lastNode = temp;
        else
          binToSearch->lastNode = NULL;
      }


      temp->nextPtr = temp->nextPtr->nextPtr;
      /*update the header_guard of the temp  since the next has  changed*/
      if(&tempFreeBlock != temp){
        pblk = (uint16*) temp;
        ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
       }
      binToSearch->totalNodes = binToSearch->totalNodes - 1;

      if(!(binToSearch->totalNodes)){
        bin_deactive(heap_ptr, bin);
      }
      /*for safe-unlinking and Output sanitization  */
     ((freeBlockList*)answer)->nextPtr = NULL;
      return answer;
    }
    else{
      return NULL;
    }


  }
  else if(mem_ptr){

    unsigned long bin = memheap_find_bin(((mem_block_header_type*)mem_ptr)->forw_offset);
    binStruct *binToSearch = &heap_ptr->BIN[bin];
    freeBlockList tempFreeBlock = {{0},0};
    freeBlockList *temp;
    tempFreeBlock.nextPtr = binToSearch->firstNode;
    temp = &tempFreeBlock;

    while((temp->nextPtr != NULL) && ((void *)temp->nextPtr != mem_ptr)){
     /*sanity check for the free block in the list*/
      MEMHEAP_ASSERT(BOUNDARY_CHECK((mem_block_header_type*)(temp->nextPtr), heap_ptr));
       temp = temp->nextPtr;
    }

    if(temp->nextPtr != NULL){
      /* Output sanitization */
      pblk = (uint16*)(temp->nextPtr);
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk));
      if(mem_ptr == (mem_block_header_type*)(binToSearch->firstNode))
        binToSearch->firstNode = temp->nextPtr->nextPtr;

      if(mem_ptr == (mem_block_header_type*)(binToSearch->lastNode)){
        if((temp->freeBlock).forw_offset)
          binToSearch->lastNode = temp;
        else
          binToSearch->lastNode = NULL;
      }

      temp->nextPtr = temp->nextPtr->nextPtr;
      /*update the header_guard of the temp  since the next has  changed*/
      if(&tempFreeBlock != temp){
        pblk = (uint16*) temp;
        ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
      }
      binToSearch->totalNodes = binToSearch->totalNodes - 1;

      if(!(binToSearch->totalNodes)){
        bin_deactive(heap_ptr, bin);
      }
      /*for safe-unlinking */
      ((freeBlockList*)mem_ptr)->nextPtr = NULL;
      return mem_ptr;
    }
    else{
      // If the address of the freeblock is not found then it must assert
       MEMHEAP_ASSERT(0);
    }
  }
  return NULL;
}

#ifdef FEATURE_MEM_DEBUG_EX
/*===========================================================================
FUNCTION MEM_INFO_ENQUEUE

DESCRIPTION
   Stores information about the current allocation.
===========================================================================*/
static void store_alloc_info(
   void          *mem_ptr,
   unsigned long  size,
)
{
   static int i = 0;

   MEMHEAP_ASSERT(mem_ptr);

   /* If out of space, overwrite the first entry.
   */

   if (i == MEM_INFO_ARRAY_SIZE)
      i = 0;

   mem_info_array[ i ].mem_ptr = mem_ptr;
   mem_info_array[i].size      = size;
   mem_info_array[i].task_id   = (unsigned long) TCB_SELF( );
   mem_info_array[ i ].freed   = 0;

   i++;

   return;
} /* END store_alloc_info */


/*===========================================================================
FUNCTION STORE_FREE_INFO

DESCRIPTION
   Marks an entry in the allocation info array as freed.
===========================================================================*/
static void store_free_info(
   void *mem_ptr
)
{
   int i;

   MEMHEAP_ASSERT(mem_ptr);

   for ( i = 0; i < MEM_INFO_ARRAY_SIZE; i++ )
   {
      if ( mem_info_array[i].mem_ptr == mem_ptr )
      {
         mem_info_array[i].freed = 1;
         break;
      }
   }

   return;
} /* END store_free_info */

#endif /* FEATURE_MEM_DEBUG_EX */


/*===========================================================================
FUNCTION MEM_GET_NEXT_BLOCK

DESCRIPTION
  Return the next block header in the heap for the block following the
  given one.  If the given block is the last block in the heap, return
  the first block in the heap.  Never returns NULL.

  Returns a pointer the the memory block header of the block following the given
  block (or the first block of the heap if the given block was the last
  block of the heap).

===========================================================================*/
static mem_block_header_type *mem_get_next_block
(
   const mem_heap_type         *heap_ptr,
     /*  The heap the given block belongs to -- may NOT be NULL!
     */
   const mem_block_header_type *block_ptr
     /*  The block in the heap for which to return the following heap
         block
     */
)
{
   uint16 *pblk = NULL;
   mem_block_header_type *nextBlkPtr = NULL;
   MEMHEAP_ASSERT(block_ptr != NULL);
   MEMHEAP_ASSERT(heap_ptr != NULL); //this may be redundent since , it a static function re-visit this for possible optimisation
   MEMHEAP_ASSERT(block_ptr->forw_offset != 0);
   MEMHEAP_ASSERT(FRD_OFFSET_CHECK(block_ptr, heap_ptr));
   MEMHEAP_ASSERT(!((block_ptr->forw_offset)%kMinChunkSize));


   nextBlkPtr =  block_ptr->last_flag  ? heap_ptr->first_block
           : (mem_block_header_type *) ((char *) block_ptr + block_ptr->forw_offset);
   pblk = (uint16*)nextBlkPtr;
   if(nextBlkPtr->free_flag == kBlockFree){
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk));
   }
   else{
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->magic_num_used, pblk));
   }
   return nextBlkPtr;
} /* END mem_get_next_block */


/*===========================================================================

FUNCTION mem_malloc_failed

DESCRIPTION
  This function is called when memheap detects an allocation failure
  from any heap managed by memheap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mem_malloc_failed
(
  struct mem_heap_struct *heap_ptr, /* Heap on which allocation is attempted */
  size_t size                       /* Size of failed request */
)
{
  MSG_2(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"mem_malloc heap 0x%x failed to allocate %d bytes",
    (uint32)heap_ptr, size);
}

/*===========================================================================
FUNCTION MEM_INIT_HEAP

DESCRIPTION
  Initializes the heap_ptr object and sets up inMemoryChunk for use with the
  heap_ptr object.  inMemoryChunk may be aligned on any boundary.  Beginning
  bytes will be skipped until a paragraph boundary is reached.  Do NOT pass
  in NULL pointers.  infail_fnc_ptr may be NULL in which case no function will
  be called if mem_malloc or mem_calloc is about to fail.  If infail_fnc_ptr
  is provided, it will be called once and then the allocation will be
  attempted again.  See description of my_allocator_failed_proc for details.
  There is no protection for initializing a heap more than once.  If a heap
  is re-initialized, all pointers previously allocated from the heap are
  immediately invalidated and their contents possibly destroyed.  If that's
  the desired behavior, a heap may be initialized more than once.
===========================================================================*/
/*lint -sem(mem_init_heap,1p,2p,2P>=3n) */
void mem_init_heap(
   mem_heap_type                 *heap_ptr,
      /* Statically allocated heap structure
      */
   void                          *heap_mem_ptr,
      /* Pointer to contiguous block of memory used for this heap
      */
   unsigned long                  heap_mem_size,
      /* The size in bytes of the memory pointed to by heap_mem_ptr
      */
   mem_allocator_failed_proc_type fail_fnc_ptr
      /* Function to call when allocation fails, can be NULL
      */
)
{
  char *memory_end_ptr;
    /* 1 beyond computed end of memory passed in to use as heap.
    */
  char *memory_start_ptr;
    /* The computed beginning of the memory passed in to use as heap.  This
       computed value guarantees the heap actually starts on a paragraph
       boundary.
    */
  unsigned long chunks;
    /* How many whole blocks of size kMinChunkSize fit in the area of
       memory starting at memory_start_ptr and ending at (memory_end_ptr-1)
    */

  int ret_value;
  uint16 * pblk = NULL;

  MEMHEAP_ASSERT(heap_ptr);

  MEMHEAP_ASSERT(magic_num_index < MAX_HEAP_INIT); /* support at the most 30 heaps*/

  if( (heap_ptr->magic_num) &&
      (heap_ptr->magic_num == magic_num[heap_ptr->magic_num_index])){
    /* heap is already initialized so just return */
    return;
  }

  memset(heap_ptr, 0, sizeof(mem_heap_type));

  if( heap_mem_size < MIN_HEAP_SIZE_FOR_BINS){
     heap_ptr->legacy_check = kUseLegacyImpl;
  }else{
     heap_ptr->legacy_check = kUseBINsImpl;
  }

  MEMHEAP_ASSERT(heap_mem_ptr);
  MEMHEAP_ASSERT(heap_mem_size);
  MEMHEAP_ASSERT(heap_mem_size >= (2*kMinChunkSize-1));

  memory_start_ptr = (char *)heap_mem_ptr;
  memory_end_ptr   = memory_start_ptr + heap_mem_size;
  /* Initialize the memheap critical section */

  /*use the memory required for memheap_crit_sect from the heap getting initialized */
  while( (((unsigned long)memory_start_ptr) & 0x000FUL) )
  {
     ++memory_start_ptr;
     ++(heap_ptr->initial_skipped_bytes);
  }

  heap_ptr->memheap_crit_sect = memory_start_ptr;

  ret_value = osal_init_crit_sect((osal_crit_sect_t*)heap_ptr->memheap_crit_sect);
  MEMHEAP_ASSERT(ret_value == OSAL_SUCCESS );

  /* by default it is critical section */
  heap_ptr->lock_fnc_ptr = mem_heap_enter_crit_sect;
  heap_ptr->free_fnc_ptr = mem_heap_leave_crit_sect;

  /*move the memory start pointer by sizeof(osal_crit_sect_t) as we have used that much out of the heap;*/
  memory_start_ptr = (char *)memory_start_ptr + sizeof(osal_crit_sect_t);
  heap_ptr->initial_skipped_bytes=heap_ptr->initial_skipped_bytes+sizeof(osal_crit_sect_t);
  /* Advance to the nearest paragraph boundary. This while loop should work
  ** regardless of how many bits are required for address pointers near or
  ** far, etc.
  **
  ** Turn off lint "size incompatibility" warning because cast from pointer
  ** to unsigned long will lose bits, but we don't care because we're only
  ** interested in the least significant bits and we never cast back into a
  ** pointer, so the warning can be safely ignored
  */
  /*lint --e(507)*/
  while( (((unsigned long)memory_start_ptr) & 0x000FUL) )
  {
    ++memory_start_ptr;
    ++(heap_ptr->initial_skipped_bytes);
  }

  chunks = (unsigned long) ((memory_end_ptr - memory_start_ptr) / kMinChunkSize);

  heap_ptr->first_block            = (mem_block_header_type *) memory_start_ptr;
  heap_ptr->next_block             = heap_ptr->first_block;


  mem_heap_get_random_num((&(magic_num[magic_num_index_array[magic_num_index]])), 4);
  heap_ptr->magic_num = magic_num[magic_num_index_array[magic_num_index]];
  heap_ptr->magic_num_index = magic_num_index_array[magic_num_index];
  mem_heap_get_random_num(&(heap_ptr->magic_num_free), 2);
  mem_heap_get_random_num(&(heap_ptr->magic_num_used), 2);
  
  
  magic_num_index++;  
  mem_init_block_header(heap_ptr->first_block, chunks * kMinChunkSize, heap_ptr);
  heap_ptr->first_block->last_flag = (char) kLastBlock;
  heap_ptr->total_blocks           = 1;
  heap_ptr->max_used               = 0;
  heap_ptr->max_request            = 0;
  heap_ptr->used_bytes             = 0;
  heap_ptr->heap_used_bytes        = 0;
  heap_ptr->total_bytes            = chunks * kMinChunkSize;
  heap_ptr->fail_fnc_ptr           = fail_fnc_ptr;

  if(heap_ptr->legacy_check & kUseBINsImpl){
    init_binToUSeForMalloc(heap_ptr, heap_ptr->first_block);
    memheap_addNodeToBin(heap_ptr, heap_ptr->first_block);
  }
  pblk = (uint16*)(heap_ptr->first_block);
  ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
  // On Q6 this needs to be a FUTEX that supports spin lock
#ifdef FEATURE_ENABLE_HEAP_PROFILING
  init_heap_debug(heap_ptr);
#endif
  return;
} /* END mem_init_heap */


/*===========================================================================
FUNCTION MEM_DEINIT_HEAP

DESCRIPTION
  De-Initializes the heap_ptr object only if the heap is in reset state.
  User is responsible for freeing all the allocated pointers before  calling
  into this function.
===========================================================================*/
void mem_deinit_heap(
   mem_heap_type                 *heap_ptr
      /* Statically allocated heap structure
      */
)
{
  int ret_value;
  // return the magic number
  magic_num_index--;
  if(magic_num_index < MAX_HEAP_INIT)
  {
    magic_num_index_array[magic_num_index]=heap_ptr->magic_num_index;
  }
 /* De-initialize heap only if all the allocated blocks are freed */
 if(heap_ptr->used_bytes == 0)
 {
   ret_value = osal_delete_crit_sect((osal_crit_sect_t*)heap_ptr->memheap_crit_sect);
   MEMHEAP_ASSERT(ret_value == OSAL_SUCCESS );
   memset(heap_ptr, 0, sizeof(heap_ptr));
 }
 else
 {
#if !defined( MEMHEAP_ASSERT_WARN )
   ERR_FATAL("Heap 0x%x Deinit Unsuccessful, used bytes is %d",heap_ptr,
                heap_ptr->used_bytes,0);
#else /* MEMHEAP_ASSERT_WARN */
   MEMHEAP_ASSERT(heap_ptr->used_bytes == 0);
#endif /* MEMHEAP_ASSERT_WARN */
 }
}
/*===========================================================================
FUNCTION MEM_INIT_BLOCK_HEADER

DESCRIPTION
  Initializes a memory block header to control a block of memory in the
  heap.  The header may still need to some of its fields adjusted after
  this call if it will be a used block or the last block in the heap.
===========================================================================*/
void mem_init_block_header(
   mem_block_header_type *block_ptr,
     /* Memory header block to be initialized
     */
   unsigned long          size,
     /* The size of the block of memory controlled by this
        memory header block INCLUDING the size of the
        header block itself
     */
     mem_heap_type *heap_ptr
)
{
  char *p_temp;
  MEMHEAP_ASSERT(block_ptr);
  MEMHEAP_ASSERT(size >= sizeof(freeBlockList));

  p_temp = ((char*)block_ptr);
  memset(p_temp, 0 , sizeof(freeBlockList));
  block_ptr->free_flag   = (char) kBlockFree;
  block_ptr->forw_offset = size;

  //block_ptr->free_flag   = (char) kBlockFree;
  //block_ptr->last_flag   = 0;
  //block_ptr->pad1        = MEMHEAP_PAD_CHAR;
  //block_ptr->extra       = 0;
  //block_ptr->forw_offset = size;

  return;
} /* END mem_init_block_header */


/*===========================================================================
FUNCTION MEM_CALLOC

DESCRIPTION
  Allocates enough space for elt_count elements each of elt_size bytes
  from the heap and initializes the space to nul bytes.  If
  heap_ptr is NULL or elt_count or elt_size is 0, the NULL pointer will
  be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
/*lint -sem(mem_calloc,1p,2n>=0&&3n>=0&&(@p==0||@P==2n*3n)) */
void* mem_calloc(
  mem_heap_type *heap_ptr,
     /* Heap from which to allocate
     */
  size_t         elt_count,
     /* Number of elements to allocate
     */
  size_t         elt_size
     /* Size of each element
     */
)
{

  register size_t total_length;
    /* the computed total length of the allocation request in bytes
    */
  void *answer;
    /* the block of memory to return to satisfy the request
    */

  if( !(OVERFLOW_CHECK(elt_count, elt_size))){
    MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH," mem_calloc_debug() Integer overflow elt_count = %d elt_size =%d",elt_count,elt_size);
    return NULL;
  }

  total_length = elt_count * elt_size;
  MEMHEAP_ASSERT(!os_is_in_irq_mode() || mem_heap_callable_by_isr(heap_ptr));
  if (os_is_in_irq_mode() && !mem_heap_callable_by_isr(heap_ptr))
     return NULL;
  answer = mem_malloc(heap_ptr, total_length);
  if( answer )
  {
    /* calloc always returns cleared memory blocks
    */
    memset(answer, 0, total_length);
  }
  return answer;
} /* END mem_calloc */


/*===========================================================================
FUNCTION MEM_MALLOC

DESCRIPTION
  Allocates a block of size bytes from the heap.  If heap_ptr is NULL
  or size is 0, the NULL pointer will be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
/*lint -sem(mem_malloc,1p,2n>=0&&(@p==0||@P==2n)) */
void* mem_malloc(
  mem_heap_type *heap_ptr,
     /* Heap from which to allocate
     */
  size_t         size
     /* Number of bytes to allocate
     */
)
{
  register int attempts = 2;
    /* how many times maximum to try to find a block of the requested
       size.  The second attempt is only made if the first attempt
       fails AND an allocator failed proc is called */

  unsigned long chunks;
    /* the computed minimum size of the memory block in chunks needed
       to satisfy the request */

  unsigned long actualSize;
    /* the computed minimum size of the memory block in bytes needed
       to satisfy the request */

  unsigned char bonusBytes;
    /* the computed number of unused bytes at the end of the allocated
       memory block.  Will always be < kMinChunkSize */

  mem_block_header_type *freeBlock = NULL;
    /* the free block found of size >= actualSize */

  void *answer = NULL;
    /* the address of memory to be returned to the caller */

  uint16 * pblk = NULL;
  uint32 blockHeaderSize=sizeof(mem_block_header_type);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MEMHEAP_ASSERT(heap_ptr != NULL);
  MEMHEAP_ASSERT(!os_is_in_irq_mode() || mem_heap_callable_by_isr(heap_ptr));

  if (!size) return NULL;

  if (os_is_in_irq_mode() && !mem_heap_callable_by_isr(heap_ptr))
     return NULL;

  /* quick check if requested size of memory is available */
  if( (unsigned long) size > heap_ptr->total_bytes ) return NULL;

  /* chunks overflow check : check max memory that can be malloc'd at a time */
  if( (0xFFFFFFFF - ((unsigned long) size + sizeof(mem_block_header_type)) )
        < (kMinChunkSize ) ) return NULL;

#if defined FEATURE_MEM_DEBUG && defined FEATURE_QDSP6
  if(size <= 4)
  {
    blockHeaderSize=blockHeaderSize-4;
  }
#endif


  chunks = ((unsigned long) size + blockHeaderSize
            + kMinChunkSize - 1) / kMinChunkSize;
  actualSize = chunks * kMinChunkSize;
  bonusBytes = (unsigned char)
                (actualSize - size - blockHeaderSize);

  do {
    /* This odd contortion of a do while loop exists to
       allow definitions for the BEGIN & END CRITICAL macros to
       introduce a new block nesting level.  Furthermore, we don't
       want a critical section active during the allocator failed
       callback */

    if (attempts == 1) {
      /* If there's a fail proc, call it now OUTSIDE of any
         critical section and then try the allocation again.
         Otherwise, we fail now */

      mem_allocator_failed_proc_type saveProc;

      BEGIN_CRITICAL_SECTION(heap_ptr);
      saveProc = heap_ptr->fail_fnc_ptr;
      heap_ptr->fail_fnc_ptr = NULL;
      END_CRITICAL_SECTION(heap_ptr);
      if (saveProc) {
        /* Must call failure procedure -- notice
           that no critical section is active at
           the moment */
        (*saveProc)(heap_ptr, size);

        /* Now must restore failure proc */
        BEGIN_CRITICAL_SECTION(heap_ptr);
        heap_ptr->fail_fnc_ptr = saveProc;
        END_CRITICAL_SECTION(heap_ptr);
      }
      else {
        break;  /* don't bother trying to find
                   a free block again because
                   there was no failure proc
                   to call to free up memory */
      }
    }

    BEGIN_CRITICAL_SECTION(heap_ptr);


    MEMHEAP_ASSERT(heap_ptr->magic_num == magic_num[heap_ptr->magic_num_index]);

   if(heap_ptr->legacy_check & kUseBINsImpl){
      freeBlock = findNRemoveFreeBlockInBINs(heap_ptr,NULL, actualSize);
    }
    else{
      freeBlock = mem_find_free_block(heap_ptr, actualSize);
    }

    if (freeBlock)
    {
      /* split the block (if necessary) and return the new block */

      MEMHEAP_ASSERT(freeBlock->forw_offset > 0);
      // frd offset and actual size are chunk aligned
      if (freeBlock->forw_offset > actualSize)
      {
        /* must split into two free blocks */


        mem_block_header_type *newBlock = (mem_block_header_type *)
                                          ((char *) freeBlock + actualSize);
        mem_init_block_header(newBlock, freeBlock->forw_offset - actualSize, heap_ptr);
        newBlock->last_flag = freeBlock->last_flag;
        freeBlock->forw_offset = actualSize;
        freeBlock->last_flag = 0;


        ++heap_ptr->total_blocks;

      //Add this new remaining free block after spliting into the free Block list
        if(heap_ptr->legacy_check & kUseBINsImpl){
          memheap_addNodeToBin(heap_ptr, newBlock);
        }
        memheap_copy_frd_offset_at_end(newBlock, heap_ptr);
        pblk = (uint16*)newBlock;
        ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
      }


      /* mark the block as used and return it */

      freeBlock->free_flag = kBlockUsed;
      freeBlock->extra = bonusBytes;
      /*  set up next block to search for
          next allocation request */
      if(!(heap_ptr->legacy_check & kUseBINsImpl)){
        heap_ptr->next_block = mem_get_next_block(heap_ptr, freeBlock);
       } else{
        /* this is unused member incaseof BINs updating for debug purpose*/
         heap_ptr->next_block =  freeBlock;
       }

      heap_ptr->used_bytes += size;
	  heap_ptr->heap_used_bytes += freeBlock->forw_offset;
      MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->used_bytes);

      if (heap_ptr->used_bytes > heap_ptr->max_used) {
        heap_ptr->max_used = heap_ptr->used_bytes;
      }
      if (size > heap_ptr->max_request) {
        heap_ptr->max_request = size;
      }

     /*set the frd_Offset_info in the free block to NULL for security */
      {
        frd_Offset_info *temp;
        temp = (frd_Offset_info*)((char*)freeBlock + freeBlock->forw_offset - sizeof(frd_Offset_info));
        temp->freeBlock_frdOff = NULL;
        temp->pad = NULL;
      }
      pblk = (uint16*)freeBlock;
      ADD_GUARD_BYTES_TO_USED_HEADER(heap_ptr->magic_num_used, pblk);
      answer = (char *) freeBlock + blockHeaderSize;
    }

#ifdef FEATURE_MEM_DEBUG
    if(answer != NULL)
    {
      MEMHEAP_ASSERT( freeBlock != NULL );
      if(heap_ptr != &amss_mem_heap){
        freeBlock->header_debug_info.caller_ptr=MEM_HEAP_CALLER_ADDRESS(MEM_HEAP_CALLER_ADDRESS_LEVEL);
      }
      else
      {
        void *caller_ptr=MEM_HEAP_CALLER_ADDRESS(1);
        memdebug_malloc(size,caller_ptr,answer);
      }
      #ifdef FEATURE_QDSP6
      if(size > 4)
      {
        freeBlock->pad2= MEMHEAP_DEBUG_MODE_PAD_VALUE;
      }
      #endif
    }
#endif

    END_CRITICAL_SECTION(heap_ptr);

  } while (--attempts && !answer);



   if (!answer && mem_allocator_failed_hook) {
      (*mem_allocator_failed_hook)(heap_ptr, size);
   }

#ifdef FEATURE_MEM_DEBUG_EX
  BEGIN_CRITICAL_SECTION(heap_ptr);
   if(!(heap_ptr->legacy_check & kUseBINsImpl)){
     store_alloc_info (heap_ptr, answer, size);
   }
  END_CRITICAL_SECTION(heap_ptr);
#endif




  return answer;
} /* END mem_malloc */

typedef struct
{
  qurt_thread_t            tid;
} memheap_free_error_log_type;

#define MAX_MEMHEAP_FREE_ERROR_LOGS 20
static memheap_free_error_log_type memheap_free_error_log[MAX_MEMHEAP_FREE_ERROR_LOGS];
static int memheap_free_error_log_cnt = 0;

void print_memheap_free_error_log(void){
   int i, err;
   char qurt_task_name[MEMHEAP_QURT_TASK_NAME_LEN];

   MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"memheap2-Free Error Log");
   for(i = 0; i < MAX_MEMHEAP_FREE_ERROR_LOGS ; i++){
     MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Memheap_free:tcb = 0x%x",memheap_free_error_log[i].tid);
     #ifndef FEATURE_QURT
	 err = qurt_thread_get_name(&qurt_task_name[0],MEMHEAP_QURT_TASK_NAME_LEN);
     if (QURT_EINVALID != err)
     {
        ERR_FATAL("Failed to get QURT task name.", 0, 0, 0);
     }
     MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          " Memheap_free: TaskName %s", qurt_task_name );
     #endif
   }
}
/*===========================================================================
FUNCTION MEM_FREE

DESCRIPTION
  Deallocates the ptr block of memory.  If ptr is NULL, heap_ptr is NULL or
  ptr is outside the range of memory managed by heap_ptr, then this function
  call does nothing (and is guaranteed to be harmless).  This function will
  ASSERT if it can detect an attempt to free an already freed block.  (This
  is not always reliable though, so it might not catch it.)
===========================================================================*/
/*lint -sem(mem_free,1p) */
void mem_free(
  mem_heap_type *heap_ptr,
     /* Heap in which to free memory
     */
  void          *ptr
     /* Memory to free
     */
)
{
  mem_block_header_type *theBlock;
    /* The computed address of the memory header block in the heap that
       controls the memory referenced by ptr */

  frd_Offset_info *temp;

  uint32          sizeBlockHeader=sizeof(mem_block_header_type);
  uint16 * pblk = NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MEMHEAP_ASSERT(heap_ptr != NULL);
  /*commented out the below assert since NULL free occurances are found and once
   corrsponding fixes are in its need to be uncomment*/
  MEMHEAP_ASSERT(heap_ptr->magic_num == magic_num[heap_ptr->magic_num_index]);
  MEMHEAP_ASSERT(!os_is_in_irq_mode() || mem_heap_callable_by_isr(heap_ptr));


  /*commented out the below assert since NULL free occurances are found and once
   corrsponding fixes are in its need to be uncomment*/
  if(ptr == NULL)
  {
     BEGIN_CRITICAL_SECTION(heap_ptr);
     memheap_free_error_log[memheap_free_error_log_cnt].tid = qurt_thread_get_id();
     memheap_free_error_log_cnt = (memheap_free_error_log_cnt +1) % MAX_MEMHEAP_FREE_ERROR_LOGS;
     END_CRITICAL_SECTION(heap_ptr);
     MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR," NULL ptr occurenaces in mem_free()");
    //MEMHEAP_ASSERT(0);

    return;
  }

  if (os_is_in_irq_mode() && !mem_heap_callable_by_isr(heap_ptr))
     return;

  /* free the passed in block */

  MEMHEAP_ASSERT(heap_ptr->first_block);
  BEGIN_CRITICAL_SECTION(heap_ptr);


  theBlock = (mem_block_header_type *)
                          ((char *) ptr - sizeBlockHeader);
  // now we have to make a guess that size of (mem_block_header_type) is 12 or 16
#if defined FEATURE_MEM_DEBUG && defined FEATURE_QDSP6
  if ( theBlock->pad2 != MEMHEAP_DEBUG_MODE_PAD_VALUE )
  {
    theBlock=(mem_block_header_type *)((char *)theBlock+4);
    sizeBlockHeader=sizeBlockHeader-4;
  }
#endif
  /* boundry check for the ptr passed to free */
  MEMHEAP_ASSERT(BOUNDARY_CHECK(theBlock, heap_ptr));


  /* Try to detect corruption. */

  MEMHEAP_ASSERT(!theBlock->free_flag);  /* Attempt to detect multiple
                                            frees of same block */
  /* Make sure forw_offset is reasonable */
  MEMHEAP_ASSERT(theBlock->forw_offset >= sizeof(freeBlockList));
  /* Make sure extra is reasonable */
  MEMHEAP_ASSERT(theBlock->extra < kMinChunkSize);

  /* Make sure forw_offset is not spiling over the heap boundry */
  MEMHEAP_ASSERT(FRD_OFFSET_CHECK(theBlock, heap_ptr));



  if (!theBlock->free_flag) /* Be intelligent about not munging the heap if
                              a multiple free of the same block is detected */
  {


    MEMHEAP_ASSERT((theBlock->forw_offset - sizeBlockHeader
                                  - theBlock->extra) <= heap_ptr->used_bytes);
    heap_ptr->used_bytes -= theBlock->forw_offset - sizeBlockHeader
                                  - theBlock->extra;
	heap_ptr->heap_used_bytes -= theBlock->forw_offset;

    MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->used_bytes);

#ifdef FEATURE_MEM_DEBUG
    memdebug_free(ptr,theBlock->header_debug_info.debug_index);
#endif

    theBlock->free_flag = (char) kBlockFree;

    /* try to do defragmentation if possible*/
    /*check if it can concatenate the previous block of the theBlock*/
    if(theBlock != heap_ptr->first_block)
    {
      mem_block_header_type *preFreeBlock;
      int validBlock = FALSE;
      temp = (frd_Offset_info*)((char*)theBlock - sizeof(frd_Offset_info));

      if(!((temp->freeBlock_frdOff)%kMinChunkSize)){

        if(MEMHEAP_MIN_BLOCK_SIZE == temp->freeBlock_frdOff){
           validBlock = TRUE;
        }
        else if(temp->pad  == magic_num[heap_ptr->magic_num_index]){
          validBlock = TRUE;
        }

        if(validBlock){
           preFreeBlock =  (mem_block_header_type *)((char*)theBlock - temp->freeBlock_frdOff);
           pblk = (uint16*)preFreeBlock;
           if(preFreeBlock >= heap_ptr->first_block){
              if((preFreeBlock->forw_offset == temp->freeBlock_frdOff) 
                 &&(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk)) /*we don’t want to ASSERT if integrity check fails , just don’t concatenate the block*/
                 &&(preFreeBlock->free_flag == kBlockFree)){

                 /* preFreeBlock is free and now we can join it with the new
                   free block theBlock so remove it from the list if apply*/
                 if(heap_ptr->legacy_check & kUseBINsImpl){
                   (void)findNRemoveFreeBlockInBINs(heap_ptr, preFreeBlock, 0);
                 }
                  /* Set preFreeBlock's end bytes to 0 as the frd_offset has changed */
                 temp->freeBlock_frdOff = 0;
                 temp->pad = NULL;
                 --heap_ptr->total_blocks;
                 preFreeBlock->forw_offset += theBlock->forw_offset;
                 preFreeBlock->last_flag = theBlock->last_flag;
                 /* now set the theBlock's header info. to NULL */
                 {
                   uint32 *temp = (uint32*)theBlock; /*did it like this for optimization purpose*/
                   temp[0] = NULL;
                   temp[1] = NULL;
                 }
                 theBlock = preFreeBlock;
              }
           }
        }/*if(validBlock)*/

      }/*if(!((temp->freeBlock_frdOff)%kMinChunkSize))*/

    }/*if(theBlock != heap_ptr->first_block)*/

    /*check if it can concatenate the next block of the theBlock*/
    if(theBlock->last_flag != kLastBlock){

      if((char*)theBlock + theBlock->forw_offset < ((char*)(heap_ptr->first_block) + heap_ptr->total_bytes))
      {
         mem_block_header_type *nextFreeBlock =
           (mem_block_header_type *)((char *)theBlock + theBlock->forw_offset);

         if(nextFreeBlock->free_flag == kBlockFree)
         {
           pblk = (uint16*)nextFreeBlock;
           MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk));
           if(heap_ptr->legacy_check & kUseBINsImpl){
             (void)findNRemoveFreeBlockInBINs(heap_ptr, nextFreeBlock, 0);
           }
           theBlock->forw_offset += nextFreeBlock->forw_offset;
           theBlock->last_flag = nextFreeBlock->last_flag;
           /* now set the nextFreeBlock's header info. to NULL */
           {
             uint32 *temp = (uint32*)nextFreeBlock; /*did it like this for optimization purpose*/
             temp[0] = NULL;
             temp[1] = NULL;
           }
           --heap_ptr->total_blocks;
         }
      }
    }

    /* now backup the next pointer if applicable */
    //next_block = mem_get_next_block(heap_ptr, theBlock);

    if (theBlock < heap_ptr->next_block) {
      /* Backup now to lessen possible fragmentation */
      heap_ptr->next_block = theBlock;
    }
    if(heap_ptr->legacy_check & kUseBINsImpl){
      memheap_addNodeToBin(heap_ptr, (void*)theBlock);
    }
    pblk = (uint16*)theBlock;
    ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
    memheap_copy_frd_offset_at_end(theBlock, heap_ptr);
    /* reset heap to initial state if everything is now freed */
    if (!heap_ptr->used_bytes) {
#ifdef FEATURE_MEM_DEBUG_EX
    char reset[] ="reset - heap";
#endif
      /* reset heap now, but retain statistics */
      heap_ptr->next_block = heap_ptr->first_block;
      mem_init_block_header(heap_ptr->first_block, heap_ptr->total_bytes, heap_ptr);
      heap_ptr->first_block->last_flag = (char) kLastBlock;
      heap_ptr->total_blocks = 1;
      if(heap_ptr->legacy_check & kUseBINsImpl){
        memset(&(heap_ptr->BIN), 0, sizeof(binStruct)*MEMHEAP2_BIN_COUNT);
        init_binToUSeForMalloc(heap_ptr, heap_ptr->first_block);
        memheap_addNodeToBin(heap_ptr, heap_ptr->first_block);
      }
      memheap_copy_frd_offset_at_end(heap_ptr->first_block, heap_ptr);
      pblk = (uint16*)(heap_ptr->first_block);
      ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
#ifdef FEATURE_MEM_DEBUG_EX
      store_alloc_info(heap_ptr,heap_ptr,0,reset,0);
#endif
    }
  }

#ifdef FEATURE_MEM_DEBUG_EX
  store_free_info( heap_ptr, ptr );
#endif
#ifdef FEATURE_MEM_DEBUG
   mem_info_free_array[memheap_free_array_cnt].heap_ptr  = heap_ptr;
   mem_info_free_array[memheap_free_array_cnt].mem_ptr = ptr;
   mem_info_free_array[memheap_free_array_cnt].task_id   = (unsigned long) TCB_SELF( );
   if(heap_ptr == &amss_mem_heap)
   {
      mem_info_free_array[ memheap_free_array_cnt ].caller   = MEM_HEAP_CALLER_ADDRESS(1);
   }else
   {
      mem_info_free_array[ memheap_free_array_cnt ].caller   = MEM_HEAP_CALLER_ADDRESS(MEM_HEAP_CALLER_ADDRESS_LEVEL);
   }
   memheap_free_array_cnt = (memheap_free_array_cnt + 1) % MEM_INFO_FREE_ARRAY_SIZE;
#endif
     END_CRITICAL_SECTION(heap_ptr);
} /* END mem_free */


/*===========================================================================
FUNCTION MEM_REALLOC

DESCRIPTION
  Resizes the ptr block of memory to be size bytes while preserving the
  block's contents.  If the block is shortened, bytes are discarded off the
  end.  If the block is lengthened, the new bytes added are not initialized
  and will have garbage values.  If heap_ptr is NULL, the contents of ptr are
  unchanged and the function silently returns NULL.  If ptr is NULL, the
  functions behaves exactly like mem_malloc (see above).  If ptr is not NULL
  and size is 0, the function behaves exactly like mem_free (see below).  If
  the block cannot be resized, ptr is not NULL and size is not 0, then NULL
  is returned and the original block is left untouched.  If the ptr block IS
  successfully resized and the returned value is different from the ptr value
  passed in, the old block passed in must be considered deallocated and no
  longer useable (i.e. do NOT try to mem_free it!)  This function will ASSERT
  if it can detect a bad pointer or a pointer to a free block within the
  range of memory managed by heap_ptr.  However, it's not always possible to
  do this.  Passing in a ptr block that is outside the memory managed by
  heap_ptr will result in a silent return of NULL with the contents of ptr
  unchanged.  This function will call mem_malloc if it can't grow the block
  in place, so the allocater failed function WILL be called if the heap is
  out of room to grow the block.

  Returns a pointer to the beginning of the resized block of memory (which
  may be different than ptr) or NULL if the block cannot be resized.
===========================================================================*/
/*lint -sem(mem_realloc,1p,3n>=0&&(@p==0||@P==3n)) */
void *mem_realloc(
  mem_heap_type *heap_ptr,
     /* Heap in which to re-allocate
     */
  void          *ptr,
     /* A block previously allocated from heap_ptr
     */
  size_t         size
     /* New size (in bytes) of the ptr block of memory
     */
)
{
  size_t oldSize;
    /* the logical size of the old block passed in */

  unsigned long chunks;
    /* the minimum number of memory chunks needed to satisfy the request */

  unsigned long actualSize;
    /* the size in bytes of the minimum number of chunks needed to satisfy
       the request */

  unsigned char bonusBytes;
    /* the computed number of unused bytes in a block satisfying the
       new block size request */

  size_t sizeChange;
    /* a signed value representing the increase (positive value) in block
       size or the decrease (negative value) in block size following a
       successful mem_realloc request -- number is in logical bytes */

  mem_block_header_type *theBlock;
    /* The address of the memory block header controlling the memory
       addressed by the ptr argument */

  mem_block_header_type *followingBlock;
    /* A loop variable used when attempting to collapse adjacent following
       free blocks to satisfy a grow request */
  uint16 * pblk = NULL;
  uint32          sizeBlockHeader=sizeof(mem_block_header_type);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MEMHEAP_ASSERT(heap_ptr != NULL);
  MEMHEAP_ASSERT(!os_is_in_irq_mode() || mem_heap_callable_by_isr(heap_ptr));
  MEMHEAP_ASSERT(heap_ptr->magic_num == magic_num[heap_ptr->magic_num_index]);

  if (os_is_in_irq_mode() && !mem_heap_callable_by_isr(heap_ptr))
     return NULL;

  /* check max memory that can be malloc'd at a time */

  if( (unsigned long) size > (heap_ptr->total_bytes - heap_ptr->used_bytes -
                             sizeof(mem_block_header_type)) ) return NULL;

  /*
   * This is one nasty evil function to implement -- it has
   * to know everything about the heap and tweak it in wierd
   * ways to grow a block.  Ugh.
   */

  if (!ptr)
  {
    return mem_malloc(heap_ptr, size);
  }
  if (!size)
  {
    mem_free(heap_ptr, ptr);
    return NULL;
  }

  MEMHEAP_ASSERT(heap_ptr->first_block);
  theBlock = (mem_block_header_type *)
                    ((char *) ptr - sizeBlockHeader);

 #if defined FEATURE_MEM_DEBUG && defined FEATURE_QDSP6
  if ( theBlock->pad2 != MEMHEAP_DEBUG_MODE_PAD_VALUE )
  {
    theBlock=(mem_block_header_type *)((char *)theBlock+4);
    sizeBlockHeader=sizeBlockHeader-4;
  }
#endif
  pblk = (uint16*)theBlock;
  MEMHEAP_ASSERT(!(INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->magic_num_used, pblk)));
  MEMHEAP_ASSERT(BOUNDARY_CHECK(theBlock, heap_ptr));


  /* Attempt to detect a free block passed in */
  MEMHEAP_ASSERT(!theBlock->free_flag);


  if (theBlock->free_flag) return NULL; /* Behave sensibly then */

 #if defined FEATURE_MEM_DEBUG && defined FEATURE_QDSP6
 //lets handle the case when block forw_offset is 16
  if(theBlock->forw_offset == kMinChunkSize)
  {
    void *answer = NULL;
    answer = mem_malloc(heap_ptr, size);
    if (!answer) return NULL; /* No room return NULL, orig. ptr unchanged */
    memcpy(answer, ptr, theBlock->forw_offset-sizeBlockHeader-theBlock->extra);
    mem_free(heap_ptr, ptr);
    return answer; /* Return new block */
  }
  if(size <=4)
  {
    sizeBlockHeader=sizeBlockHeader-4;
  }
 #endif

  oldSize = (size_t) mem_get_block_logical_size(theBlock, heap_ptr);
  if (oldSize == size) return ptr; /* No size change, return same pointer */

  sizeChange = size - oldSize;
  chunks = ((unsigned long) size + sizeBlockHeader
                                    + kMinChunkSize - 1) / kMinChunkSize;
  actualSize = chunks * kMinChunkSize;
  bonusBytes = (unsigned char)
                      (actualSize - size - sizeBlockHeader);

  if (theBlock->forw_offset < actualSize) {
    /* We must grow the block.  This can take one
       of two forms:
         1) Grow the block in place if there are
            enough adjacent free block(s).
         2) Malloc a new block, copy the preserved
            bytes into it and free the old block.
       Option (1) is kinda tricky and that code
       is done here.  Option (2) is handled by just
       calling malloc, memcpy, & free */

    void *answer = NULL;
    mem_block_header_type *newBlock, *nextFollowingBlock;
    unsigned long sizeNeeded = actualSize - theBlock->forw_offset;

    /* Begin by attempting (1) */
    BEGIN_CRITICAL_SECTION(heap_ptr);

    /* This code looks a lot like mem_find_free_block, but it's not
       exactly the same */

    followingBlock = mem_get_next_block(heap_ptr, theBlock);
    if (followingBlock > theBlock && followingBlock->free_flag) {
      nextFollowingBlock = mem_get_next_block(heap_ptr, followingBlock);

      // remove this followingBlock from the free list as it will be used here
      if(heap_ptr->legacy_check & kUseBINsImpl){
        (void)findNRemoveFreeBlockInBINs(heap_ptr, followingBlock, 0);
      }
      while (followingBlock->forw_offset < sizeNeeded
             && nextFollowingBlock->free_flag
             && nextFollowingBlock > followingBlock) {
        /* collapse adjacent free blocks into one if it
         * will allow us to satisfy the request
         */

        unsigned char fixupNext = heap_ptr->next_block == nextFollowingBlock;

        // remove this nextFollowingBlock from the free list as it will be used here
        if(heap_ptr->legacy_check & kUseBINsImpl){
          (void)findNRemoveFreeBlockInBINs(heap_ptr, nextFollowingBlock, 0);
        }

        followingBlock->forw_offset += nextFollowingBlock->forw_offset;
        followingBlock->last_flag = nextFollowingBlock->last_flag;
        pblk = (uint16*)followingBlock;
        ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
        
        /* now set the nextFollowingBlock's header info. to NULL */
         {
           uint32 *temp = (uint32*)nextFollowingBlock; /*did it like this for optimization purpose*/
           temp[0] = NULL;
           temp[1] = NULL;
         }
        --heap_ptr->total_blocks;
        nextFollowingBlock = mem_get_next_block(heap_ptr, followingBlock);

        if (fixupNext) heap_ptr->next_block = nextFollowingBlock;
      }
      if (followingBlock->forw_offset >= sizeNeeded) {
        unsigned char fixupNext;

        if (followingBlock->forw_offset > sizeNeeded) {
          /* must split free block into two free blocks */

          nextFollowingBlock = mem_get_next_block(heap_ptr, followingBlock);
          newBlock = (mem_block_header_type *)
                                  ((char *) followingBlock + sizeNeeded);
          mem_init_block_header(newBlock,
                                followingBlock->forw_offset - sizeNeeded, heap_ptr);
          newBlock->last_flag = followingBlock->last_flag;
          followingBlock->forw_offset = sizeNeeded;
          followingBlock->last_flag = 0;
          pblk = (uint16*)followingBlock;
          ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
          ++heap_ptr->total_blocks;

          if (heap_ptr->next_block == nextFollowingBlock) {
            /* Backup our next fit searching pointer */
            heap_ptr->next_block = newBlock;
          }
          // add this newBlock into the free list
          if(heap_ptr->legacy_check & kUseBINsImpl){
            memheap_addNodeToBin(heap_ptr, (void*)newBlock);
          }
          memheap_copy_frd_offset_at_end(newBlock, heap_ptr);

          pblk = (uint16*)newBlock;
          ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
        }
        /* Now join the correct size free block onto the end of theBlock */

        theBlock->last_flag = followingBlock->last_flag;
        theBlock->forw_offset += followingBlock->forw_offset;
        /* now set the followingBlock's header info. to NULL */
        {
          uint32 *temp = (uint32*)followingBlock; /*did it like this for optimization purpose*/
          temp[0] = NULL;
          temp[1] = NULL;
        }
        --heap_ptr->total_blocks;
        heap_ptr->used_bytes = heap_ptr->used_bytes + sizeChange;
		heap_ptr->heap_used_bytes += sizeChange;

        MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->used_bytes);

        theBlock->extra = bonusBytes;
        fixupNext = (heap_ptr->next_block == followingBlock);
        pblk = (uint16*)theBlock;
        ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);

        if (heap_ptr->used_bytes > heap_ptr->max_used) {
          heap_ptr->max_used = heap_ptr->used_bytes;
        }
        if (size > heap_ptr->max_request) {
          heap_ptr->max_request = size;
        }
        followingBlock = mem_get_next_block(heap_ptr, theBlock);

        if (fixupNext) heap_ptr->next_block = followingBlock;
        MEMHEAP_ASSERT(theBlock->forw_offset == actualSize);
        answer = ptr; /* Trigger return when out of critical section */
      }
      else{
       // we need to add the followingBlock back into the free list as it is not used
        if(heap_ptr->legacy_check & kUseBINsImpl){
          memheap_addNodeToBin(heap_ptr, (void*)followingBlock);
        }
        memheap_copy_frd_offset_at_end(followingBlock, heap_ptr);
        pblk = (uint16*)followingBlock;
        ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free,pblk);
      }
    }

    END_CRITICAL_SECTION(heap_ptr);

    if (answer)
    {
#ifdef FEATURE_MEM_DEBUG
      if(heap_ptr != &amss_mem_heap){
        theBlock->header_debug_info.caller_ptr=MEM_HEAP_CALLER_ADDRESS(MEM_HEAP_CALLER_ADDRESS_LEVEL);
      }else
      {
         void *caller_ptr=MEM_HEAP_CALLER_ADDRESS(1);
         memdebug_malloc_updateparams(size,caller_ptr,ptr);
      }
#endif
      pblk = (uint16*)theBlock;
      ADD_GUARD_BYTES_TO_USED_HEADER(heap_ptr->magic_num_used, pblk);

      return ptr; /* (1) succeeded, return now */
    }

    /* Must use method (2) */
    answer = mem_malloc(heap_ptr, size);

    if (!answer) return NULL; /* No room return NULL, orig. ptr unchanged */
    memcpy(answer, ptr, oldSize);
    mem_free(heap_ptr, ptr);
    return answer; /* Return new block */
  }

  BEGIN_CRITICAL_SECTION(heap_ptr);

  if (theBlock->forw_offset > actualSize) {
    /* We must shrink the block down in size which
       means splitting it into two and marking the
       new block as free */
    mem_block_header_type *newBlock;

    followingBlock = mem_get_next_block(heap_ptr, theBlock);
    newBlock = (mem_block_header_type *) ((char *) theBlock + actualSize);
    mem_init_block_header(newBlock, theBlock->forw_offset - actualSize, heap_ptr);
    newBlock->last_flag = theBlock->last_flag;
    theBlock->forw_offset = actualSize;
    theBlock->last_flag = 0;
    pblk = (uint16*)theBlock;
    ADD_GUARD_BYTES_TO_USED_HEADER(heap_ptr->magic_num_used, pblk);

    ++heap_ptr->total_blocks;

    if (heap_ptr->next_block == followingBlock) {
      /* Backup our next fit searching pointer */
      heap_ptr->next_block = newBlock;
    }

    // add this newBlock into the free list
    if(heap_ptr->legacy_check & kUseBINsImpl){
      memheap_addNodeToBin(heap_ptr, (void*)newBlock);
    }
    memheap_copy_frd_offset_at_end(newBlock, heap_ptr);
    pblk = (uint16*)newBlock;
    ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
  }

  heap_ptr->used_bytes = heap_ptr->used_bytes + sizeChange;
  heap_ptr->heap_used_bytes = heap_ptr->heap_used_bytes + sizeChange;

  MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->used_bytes);
  theBlock->extra = bonusBytes;
  pblk = (uint16*)theBlock;
  ADD_GUARD_BYTES_TO_USED_HEADER(heap_ptr->magic_num_used, pblk);
  if (heap_ptr->used_bytes > heap_ptr->max_used) {
    heap_ptr->max_used = heap_ptr->used_bytes;
  }
  if (size > heap_ptr->max_request) {
    heap_ptr->max_request = size;
  }
 #if defined FEATURE_MEM_DEBUG && defined FEATURE_QDSP6
  if(size <=4)
  {
    ptr=(char*)ptr-4;
  }
 #endif

  END_CRITICAL_SECTION(heap_ptr);

#ifdef FEATURE_MEM_DEBUG
  if(heap_ptr != &amss_mem_heap){
    theBlock->header_debug_info.caller_ptr=MEM_HEAP_CALLER_ADDRESS(MEM_HEAP_CALLER_ADDRESS_LEVEL);
  }else
  {
     void *caller_ptr=MEM_HEAP_CALLER_ADDRESS(1);
     memdebug_malloc_updateparams(size,caller_ptr,ptr);
  }
#endif

  return ptr;
} /* END mem_realloc */


/*===========================================================================
FUNCTION MEM_HEAP_USED

DESCRIPTION
  Returns the number of allocated bytes, excluding overhead.
===========================================================================*/
size_t mem_heap_used (mem_heap_type *heap_ptr)
{
  size_t used = 0;

  if (heap_ptr != NULL)
  {
    used = heap_ptr->used_bytes;
  }

  return used;
}

/*===========================================================================
FUNCTION MEM_HEAP_OVERHEAD

DESCRIPTION
  Returns the number of bytes of overhead, per allocation, for the heap.
===========================================================================*/
size_t mem_heap_overhead (mem_heap_type *heap_ptr)
{
#ifdef FEATURE_QUBE
  NOTUSED(heap_ptr);
#endif
  return sizeof (mem_block_header_type);
}

/*===========================================================================
FUNCTION MEM_HEAP_CONSUMED

DESCRIPTION
  Returns the number of bytes consumed by allocation and overhead.
===========================================================================*/
size_t mem_heap_consumed (mem_heap_type *heap_ptr)
{
  size_t consumed_cnt = 0;

  if (heap_ptr)
  {
    consumed_cnt = heap_ptr->used_bytes;

    consumed_cnt += heap_ptr->total_blocks * sizeof (mem_block_header_type);
  }

  return consumed_cnt;
}

/*===========================================================================
FUNCTION MEM_FIND_FREE_BLOCK

DESCRIPTION
  Find a free block of at least inSizeNeeded total bytes.  Collapse
  adjacent free blocks along the way.

  Returns a pointer to a memory block header describing a free block
  of at least inSizeNeeded total bytes.  Returns NULL if no such free
  block exists or can be created by collapsing adjacent free blocks.
===========================================================================*/
mem_block_header_type *mem_find_free_block(
   mem_heap_type *heap_ptr,
     /*  The heap to search for a free block
     */
   unsigned long  size_needed
     /*  The minimum size in bytes of the block needed (this size
         INCLUDES the size of the memory block header itself)
     */
)
{
  long searchBlocks;
    /* The maximum number of blocks to search.  After searching this
       many, we've been through the heap once and the allocation fails
       if we couldn't find/create a satisfactory free block */

  mem_block_header_type *followingBlock;
    /* a loop variable used to walk through the blocks of the heap */
  uint16 *pblk = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MEMHEAP_ASSERT(heap_ptr);
  MEMHEAP_ASSERT(heap_ptr->first_block);
  /* sanity check for the  heap_ptr->next_block  */
  if((heap_ptr->next_block)->free_flag == kBlockFree){
      pblk = (uint16*)(heap_ptr->next_block);
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk));
   }
   else{
      pblk = (uint16*)(heap_ptr->next_block);
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->magic_num_used, pblk));
   }
        
  searchBlocks = (long) heap_ptr->total_blocks;

  followingBlock = mem_get_next_block(heap_ptr, heap_ptr->next_block);

  for (; searchBlocks > 0; --searchBlocks, heap_ptr->next_block=followingBlock,
                  followingBlock=mem_get_next_block(heap_ptr, heap_ptr->next_block))
  {
    MEMHEAP_ASSERT(heap_ptr->next_block->forw_offset > 0);
    MEMHEAP_ASSERT(followingBlock->forw_offset > 0);

    if (heap_ptr->next_block->free_flag)
    {
      while (heap_ptr->next_block->forw_offset < size_needed
             && followingBlock->free_flag && followingBlock > heap_ptr->next_block)
      {
        /* collapse adjacent free blocks into one if it
         * will allow us to satisfy a request
         */

        heap_ptr->next_block->forw_offset += followingBlock->forw_offset;
        heap_ptr->next_block->last_flag = followingBlock->last_flag;
        pblk = (uint16*) (heap_ptr->next_block);
        ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);

        --heap_ptr->total_blocks;
        --searchBlocks; /* don't search more than we need */
        followingBlock = mem_get_next_block(heap_ptr, heap_ptr->next_block);

      }
      if (heap_ptr->next_block->forw_offset >= size_needed) {
        return heap_ptr->next_block;
      }
    }
  }
  return 0;       /* didn't find anything */
} /* END mem_find_free_block */


/*===========================================================================
FUNCTION MEM_GET_BLOCK_LOGICAL_SIZE

DESCRIPTION
  Returns the logical block size of a memory block in the heap

  Returns the size (in bytes) of the block excluding block header overhead and
  unused bytes at the end of the block.
===========================================================================*/
/*lint -sem(mem_get_block_logical_size,1p) */
unsigned long mem_get_block_logical_size(
   const mem_block_header_type *block_hdr_ptr,
     /*  A memory block to get the logical size of
     */
    mem_heap_type *heap_ptr
)
{
  uint32 sizeBlockHeader=sizeof(mem_block_header_type);
  MEMHEAP_ASSERT(block_hdr_ptr);
  MEMHEAP_ASSERT(block_hdr_ptr->forw_offset != 0);
  MEMHEAP_ASSERT(!((block_hdr_ptr->forw_offset)%kMinChunkSize));
  #if defined FEATURE_QDSP6 && defined FEATURE_MEM_DEBUG
  if (block_hdr_ptr->forw_offset == kMinChunkSize)
  {
    sizeBlockHeader=sizeBlockHeader-4;
  }
  #endif
  return block_hdr_ptr->forw_offset - sizeBlockHeader
            - (block_hdr_ptr->free_flag ? 0 : block_hdr_ptr->extra);
} /* mem_get_block_logical_size */

/* ------------------------------------------------------------------------
**
** Routines that return information about the heap
**
** ------------------------------------------------------------------------ */

/*===========================================================================
FUNCTION MEM_HEAP_BLOCK_ITERATOR_INIT

DESCRIPTION
  Initializes an iterator structure to be used for walking the blocks
  in the specified heap.
===========================================================================*/
void mem_heap_block_iterator_init(
   mem_heap_block_iterator_type *block_iter_ptr,
     /*  The Iterator structure to be initialized
     */
   const mem_heap_type          *heap_ptr
     /*  The heap this iterator should iterate over
     */
)
{
  MEMHEAP_ASSERT(block_iter_ptr);
  MEMHEAP_ASSERT(heap_ptr);

  block_iter_ptr->mAllocator = heap_ptr;
  mem_heap_block_iterator_reset(block_iter_ptr);
} /* mem_heap_block_iterator_init */


/*===========================================================================
FUNCTION MEM_HEAP_BLOCK_ITERATOR_RESET

DESCRIPTION
  Reset a heap block iterator to begin the iteration of heap blocks from
  the beginning.
===========================================================================*/
void mem_heap_block_iterator_reset(
   mem_heap_block_iterator_type *block_iter_ptr
     /* The iterator structure to be reset
     */
)
{
  MEMHEAP_ASSERT(block_iter_ptr);
  block_iter_ptr->mBlock = NULL;
} /* mem_heap_block_iterator_reset */


/*===========================================================================
FUNCTION MEM_HEAP_BLOCK_ITERATOR_NEXT

DESCRIPTION
  Return a memory block header for the next block (or next virtual block)
  in the heap being iterated.  Description is only valid if the function
  returns a non-zero value.

  The heap being iterated should not be changed during the iteration as the
  iterator may get lost or return bogus values.  There is no protection
  against this however.

  Returns:
      zero      There are no more heap blocks to return
      non-zero  The next heap block was returned in outBlock successfully
===========================================================================*/
int mem_heap_block_iterator_next(
   mem_heap_block_iterator_type *block_iter_ptr,
     /* Iterator to return next iterated block from
     */
   mem_block_header_type        *out_block_ptr,
     /* Pointer to a location where a copy of a memory block header
        is placed describing the returned memory block.  Only
        valid if the function returns a non-zero value.
     */
   int                           fake_free_coalesced
     /* Input flag.  zero => return the next block header from the heap
        exactly as it is.  non-zero => return the next virtual block
        header from the heap.  Virtual block headers are created by
        collapsing all adjacent free blocks into one.
     */
)
{
  int ans;
    /* Holds function result */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MEMHEAP_ASSERT(block_iter_ptr);
  MEMHEAP_ASSERT(out_block_ptr);

  ans = 0;
  if (!block_iter_ptr->mBlock)
  {
    block_iter_ptr->mBlock = block_iter_ptr->mAllocator->first_block;
    if (block_iter_ptr->mBlock) {
      ans = 1;
    }
  }
  else if (!block_iter_ptr->mBlock->last_flag)
  {
    block_iter_ptr->mBlock = mem_get_next_block(block_iter_ptr->mAllocator, block_iter_ptr->mBlock);
    ans = 1;
  }
  memset(out_block_ptr, 0, sizeof(mem_block_header_type));
  if (ans)
  {
    *out_block_ptr = *block_iter_ptr->mBlock;
    if (fake_free_coalesced && out_block_ptr->free_flag)
    {
      /* Pretend as though adjacent free blocks were coalesced */
      for (;;)
      {
        mem_block_header_type *next_block;
        if (block_iter_ptr->mBlock->last_flag) break;
        next_block = mem_get_next_block(block_iter_ptr->mAllocator, block_iter_ptr->mBlock);
        if (!next_block->free_flag) break;
        block_iter_ptr->mBlock = next_block;
        out_block_ptr->forw_offset += block_iter_ptr->mBlock->forw_offset;
      }
    }
  }
  return ans;
} /* mem_heap_block_iterator_next */


/*===========================================================================
FUNCTION MEM_HEAP_GET_TOTALS

DESCRIPTION
  Returns heap totals for the heap.  This function IS thread safe.
  Heap must already have been initialized.
  Return totals for the heap.
===========================================================================*/
void mem_heap_get_totals(
   mem_heap_type  *heap_ptr,
      /* The heap to return totals for
      */
   mem_heap_totals_type *totals
      /* A pointer to a totals structure to receive the various
         heap information
      */
)
{
  mem_heap_block_iterator_type heapIter;
    /* heap iterator used to collect heap statistics */

  mem_block_header_type theBlock;
    /* holds info returned by heap iterator */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MEMHEAP_ASSERT(heap_ptr);
  MEMHEAP_ASSERT(totals);

  BEGIN_CRITICAL_SECTION(heap_ptr);
  memset(totals, 0, sizeof(*totals));
  mem_heap_block_iterator_init(&heapIter, heap_ptr);
  totals->max_logical_used = heap_ptr->max_used;
  totals->max_logical_request = heap_ptr->max_request;
  while (mem_heap_block_iterator_next(&heapIter, &theBlock, 1))
  {
    unsigned long blkSize = mem_get_block_logical_size(&theBlock, heap_ptr);
    ++totals->current_block_count;
    totals->header_bytes += sizeof(mem_block_header_type);
    #if defined FEATURE_QDSP6 && defined FEATURE_MEM_DEBUG
    if (theBlock.forw_offset == kMinChunkSize)
    {
     totals->header_bytes-=4;
    }
    #endif

    if (theBlock.free_flag)
    {
      totals->free_bytes += blkSize;
      if (blkSize > totals->largest_free_block)
        totals->largest_free_block = blkSize;
    }
    else
    {
      ++totals->used_blocks;
      totals->used_bytes += blkSize;
      totals->wasted_bytes += theBlock.extra;
    }
  }
  totals->total_physical_bytes = totals->free_bytes
                                    + totals->used_bytes
                                    + totals->wasted_bytes
                                    + totals->header_bytes;
  MEMHEAP_ASSERT(totals->used_bytes == heap_ptr->used_bytes);
  MEMHEAP_ASSERT(totals->current_block_count >= 1);
  MEMHEAP_ASSERT(totals->current_block_count <= heap_ptr->total_blocks);
  MEMHEAP_ASSERT(totals->total_physical_bytes == heap_ptr->total_bytes);
  END_CRITICAL_SECTION(heap_ptr);
} /* mem_heap_get_totals */


/*===========================================================================
FUNCTION MEM_HEAP_SET_INT_LOCK

DESCRIPTION
  This function sets up the specified heap to use interrupt locking and
  freeing as its locking mechanism.
  Returns 1 on success and 0 on failure.

SIDE EFFECTS
  There are cases where the memheap APIs take longer time than
  usual (eg: Defrag enabled), causing interrupts to be lost/delayed
===========================================================================*/
int
mem_heap_set_int_lock( mem_heap_type *heap_ptr )
{
#ifdef FEATURE_QUBE
  NOTUSED(heap_ptr);
  return 0;
#else /* FEATURE_QUBE */

   MEMHEAP_ASSERT (heap_ptr != NULL);

   if ( heap_ptr != NULL )
   {
      heap_ptr->lock_fnc_ptr = mem_heap_int_lock;
      heap_ptr->free_fnc_ptr = mem_heap_int_free;
      return 1;
   }
   else
      return 0;
#endif /* FEATURE_QUBE */
} /* END mem_heap_set_int_lock */

/*===========================================================================
FUNCTION MEM_HEAP_SET_TASK_LOCK

DESCRIPTION
  This function sets up the specified heap to use task locking and
  freeing as its locking mechanism.
  Returns 1 on success and 0 on failure.
===========================================================================*/
int
mem_heap_set_task_lock( mem_heap_type *heap_ptr )
{
#ifdef FEATURE_QUBE
  NOTUSED(heap_ptr);
  return 0;
#else /* FEATURE_QUBE */
   MEMHEAP_ASSERT (heap_ptr != NULL);

   if ( heap_ptr != NULL )
   {
      heap_ptr->lock_fnc_ptr = mem_heap_task_lock;
      heap_ptr->free_fnc_ptr = mem_heap_task_free;
      return 1;
   }
   else
      return 0;
#endif /* FEATURE_QUBE */
} /* END mem_heap_set_task_lock */

/*===========================================================================
FUNCTION MEM_HEAP_SET_CRIT_SECT

DESCRIPTION
  This function sets up the specified heap to use QUBE Mutex/REX crit_sect/
  WM CriticalSection for its locking mechanism.
  Returns 1 on success and 0 on failure.
===========================================================================*/
int
mem_heap_set_crit_sect( mem_heap_type *heap_ptr )
{
#ifdef FEATURE_QUBE
  NOTUSED(heap_ptr);
  return 0;
#else /* FEATURE_QUBE */
   MEMHEAP_ASSERT (heap_ptr != NULL);

   if ( heap_ptr != NULL )
   {
      heap_ptr->lock_fnc_ptr = mem_heap_enter_crit_sect;
      heap_ptr->free_fnc_ptr = mem_heap_leave_crit_sect;
      return 1;
   }
   else
      return 0;
#endif /* FEATURE_QUBE */
} /* END mem_heap_set_crit_sect */

/*===========================================================================
FUNCTION MEM_HEAP_SET_NO_LOCK

DESCRIPTION
  This function sets up the specified heap to use no locking. This implies
  that this heap should only be used from the context of one task.
  Returns 1 on success and 0 on failure.
===========================================================================*/
int
mem_heap_set_no_lock( mem_heap_type *heap_ptr )
{

  MEMHEAP_ASSERT (heap_ptr != NULL);

   if ( heap_ptr != NULL )
   {
      heap_ptr->lock_fnc_ptr = NULL;
      heap_ptr->free_fnc_ptr = NULL;
      return 1;
   }
   else
      return 0;
} /* END mem_heap_set_no_lock */


/*===========================================================================
FUNCTION MEM_HEAP_GET_FREEBLOCK_INFO

DESCRIPTION
  Returns information about the sizes of the largest n free blocks in the
  heap.
  The heap must already have been initialized. The buffer passed in must
  have enough space to hold the requested information.
===========================================================================*/
void
mem_heap_get_freeblock_info(
  mem_heap_type *heap_ptr,
  unsigned int   num_blocks,
  unsigned int  *buf
)
{
  mem_heap_block_iterator_type heap_iter;
  mem_block_header_type the_block;
  unsigned int blk_size;
  unsigned int min_blk_size;
  unsigned int min_blk_index;
  unsigned int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MEMHEAP_ASSERT( heap_ptr );
  MEMHEAP_ASSERT( buf );
  MEMHEAP_ASSERT( num_blocks >= 1 );

  BEGIN_CRITICAL_SECTION( heap_ptr );

  memset( buf, 0, num_blocks * sizeof( int ) );

  mem_heap_block_iterator_init( &heap_iter, heap_ptr );

  while ( mem_heap_block_iterator_next( &heap_iter, &the_block, 1 ) )
  {
    if ( the_block.free_flag )
    {
      blk_size = mem_get_block_logical_size( &the_block ,heap_ptr);
      min_blk_size  = buf[0];
      min_blk_index = 0;
      for ( i = 1; i < num_blocks; i++ )
      {
        if ( min_blk_size > buf[i] )
        {
          min_blk_size  = buf[i];
          min_blk_index = i;
        }
      }
      if ( blk_size > min_blk_size )
        buf[min_blk_index] = blk_size;
    }
  }
  END_CRITICAL_SECTION(heap_ptr);
} /* mem_heap_get_freeblock_info */


#ifdef FEATURE_MEM_DEBUG

/*===========================================================================
FUNCTION MEM_HEAP_GET_ALLOC_INFO

DESCRIPTION
  Returns information about the sizes of the first n allocated blocks in the
  heap.
  The heap must already have been initialized. The buffer passed in must
  have enough space to hold the requested information.
===========================================================================*/
void
mem_heap_get_alloc_info(
  mem_heap_type            *heap_ptr,
  unsigned int              num_blocks,
  mem_heap_alloc_info_type *buf
)
{
  mem_heap_block_iterator_type heap_iter;
  mem_block_header_type the_block;
  char *alloc_ptr;
  uint32 i = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MEMHEAP_ASSERT( heap_ptr );
  MEMHEAP_ASSERT( buf );
  MEMHEAP_ASSERT( num_blocks >= 1 );

  BEGIN_CRITICAL_SECTION( heap_ptr );

  memset( buf, 0, num_blocks * sizeof( mem_heap_alloc_info_type ) );

  mem_heap_block_iterator_init( &heap_iter, heap_ptr );

  alloc_ptr = (char *) heap_ptr->first_block;

  while ( mem_heap_block_iterator_next( &heap_iter, &the_block, 1 ) )
  {
    if ((i >= num_blocks) || (the_block.forw_offset == 0))
      break;

    if ( !the_block.free_flag )
    {
      buf[i].ptr                = alloc_ptr;
      buf[i].size               = mem_get_block_logical_size( &the_block ,heap_ptr);
      buf[i].caller_ptr         = the_block.header_debug_info.caller_ptr;
      i++;
    }

   alloc_ptr +=  the_block.forw_offset;
  }

  END_CRITICAL_SECTION(heap_ptr);
} /* mem_heap_get_freeblock_info */

#endif /* FEATURE_MEM_DEBUG */

/*===========================================================================
FUNCTION MEM_HEAP_GET_CHUNK_SIZE

DESCRIPTION
  Returns the minimum chunk size used for allocations from a heap. All
  allocations from a specific heap are multiples of this size.
===========================================================================*/
unsigned int
mem_heap_get_chunk_size( mem_heap_type *heap_ptr )
{
#ifdef FEATURE_QUBE
  NOTUSED(heap_ptr);
#endif
  return (unsigned int) kMinChunkSize;
} /* END mem_heap_get_chunk_size */



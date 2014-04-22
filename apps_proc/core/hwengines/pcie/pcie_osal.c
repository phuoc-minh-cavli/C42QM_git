/**
 * @file pcieosal.c
 * @brief
 * This file contains implementation of interfaces that provide a thin
 * abstraction to various OS-specific APIs used by the pcie driver.
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
                   Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "pcie_osal.h"
#include "pcie_i.h"
#include "DDIInterruptController.h"

#ifndef  PCIE_BOOT
#include "ULogFront.h"
#include "busywait.h"
#include "dog.h"
#include "time_types.h"
#include "time_svc.h"
#include <kernel.h>
#else /* PCIE_BOOT */
#include "boot_error_if.h"
#include "DALDeviceId.h"
#include "DALFramework.h"
#endif /* PCIE_BOOT */

#define PCIE_ULOG_BUFFER_MAX     8192       /** PCIe Ulog buffer size */

/** OSAL initialization done flag */
uint32 pcie_osal_init_done = 0;

#ifndef PCIE_BOOT
/** Flag for ulog enable */
boolean pcie_osal_ulog_enable;
/** ULog global handle */
ULogHandle pcieulog;
/** Handle for time ticks */
static DalDeviceHandle *pcie_osal_timetick_handle = NULL;
#endif /* PCIE_BOOT */

/** Prototypes */
#ifndef PCIE_BOOT
extern void bam_drv_init(void);
/* Initialize PCIe Ulogs */
static void pcie_osal_uloginit(void);
/* De-initialize PCIe Ulogs */
static void pcie_osal_ulogdeinit(void);
/* Initialize timeticks from system time */
static void pcie_osal_timetick_init(void);
#endif

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_osal_ulogInit
** ============================================================================
*/
/**
 * Ulog message logging interface.Allocates buffer for logging.On successful intialization it will return the
 * Ulog pcie handle.
 *
 * @param[in]    void
 *
 * @return       void
 */
static void pcie_osal_uloginit(void)
{
   /* Ulog Handle and buffer intialization. */
   ULogFront_RealTimeInit(&pcieulog, "pcie", PCIE_ULOG_BUFFER_MAX, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
   pcie_osal_ulog_enable = TRUE;
}

/* ============================================================================
**  Function : pcie_osal_ulogdeinit
** ============================================================================
*/
/**
 * Ulog messsgae logging interface Deintialiization.
 *
 * @param[in]    void
 *
 * @return       void
 */
void pcie_osal_ulogdeinit(void)
{
   pcie_osal_ulog_enable = FALSE;
   return;
}

/* ============================================================================
**  Function : pcie_osal_timetick_init
** ============================================================================
*/
/**
 * Initializes timeticks from system time.
 *
 * @param[in]    void
 *
 * @return       void
 */
static void pcie_osal_timetick_init(void)
{
   DALResult result;
   pcie_osal_timetick_handle = NULL;

   result = DalTimetick_Attach("SystemTimer", &pcie_osal_timetick_handle);
   if((result != DAL_SUCCESS) || (pcie_osal_timetick_handle == NULL))
   {
      pcie_osal_debug(MSG_FATAL, "DalTimetick_Attach failed");
      pcie_osal_timetick_handle = NULL;
      return;
   }

   result = DalTimetick_InitTimetick64(pcie_osal_timetick_handle);
   if((result != DAL_SUCCESS))
   {
      pcie_osal_debug(MSG_FATAL, "DalTimetick_InitTimetick64 failed");
      pcie_osal_timetick_handle = NULL;
   }
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_osal_init
** ============================================================================
*/
/**
 * Initializes OSAL Interfaces. Performs any intiialization pertaining to the
 * OS specific APIs.
 *
 * @param[in]    void
 *
 * @return       void
 */
void pcie_osal_init(void)
{
   if(pcie_osal_init_done)
   {
      return;
   }

   DALSYS_InitMod(NULL);

#ifndef PCIE_BOOT
   pcie_osal_uloginit();
   pcie_osal_timetick_init();
   bam_drv_init();
#endif

   pcie_osal_init_done = 1;
}

/* ============================================================================
**  Function : pcie_osal_deinit
** ============================================================================
*/
/**
 * Deinitializes OSAL Interfaces. Performs any deinitialization pertaining to the
 * OS specific APIs.
 *
 * @param[in]    void
 *
 * @return       void
 */
void pcie_osal_deinit(void)
{
#ifndef PCIE_BOOT
   pcie_osal_ulogdeinit();
#endif

   if(!pcie_osal_init_done)
   {
      return;
   }
   pcie_osal_init_done = 0;

   DALSYS_DeInitMod();
}

/* ============================================================================
**  Function : pcie_osal_malloc
** ============================================================================
*/
/**
 * Mallocs a memory (virtual) using OS Malloc API. The memory allocated
 * is for SW usage only.
 *
 * @param[in]    size    Size of the memory to be allocated
 *
 * @return       void*
 *    Pointer to the memory allocated. NULL if failed.
 *
 * @sa
 * pcie_osal_mallocmemregion, pcie_osal_free
 */
void* pcie_osal_malloc(uint32 size)
{
   void *ptr;

   DALResult res = DALSYS_Malloc(size, &ptr);

   if(res != DAL_SUCCESS)
   {
      return(NULL);
   }

   return(ptr);
}

/* ============================================================================
**  Function : pcie_osal_free
** ============================================================================
*/
/**
 * Frees the memory allocated by pcie_osal_malloc.
 *
 * @param[in,out]    mem    Pointer to the memory to be freed
 *
 * @return           void
 *
 * @sa
 * pcie_osal_malloc.
 */
void pcie_osal_free(void *pmem)
{
   DALSYS_Free(pmem);
}

/* ============================================================================
**  Function : pcie_osal_memset
** ============================================================================
*/
/**
 * Sets memory of specified size to specified value
 *
 * @param[in,out]   mem    Pointer to the block of memory to fill
 * @param[in]       val    Value of byte to fill the memory with
 * @param[in]       size    Number of bytes to fill
 *
 * @return          void
 */
void pcie_osal_memset(void *mem, uint8 val, uint32 size)
{
   DALSYS_memset(mem, (int)val, size);
}

/* ============================================================================
**  Function : pcie_osal_malloc_aligned
** ============================================================================
*/
/**
 * Alloc an aligned chunk of memory
 *
 * @param[in]    size     Size to malloc in bytes
 * @param[in]    align    Amount to align by in bytes no bigger than 255. Must be a power of 2
 *
 * @return       void*
 *    Pointer to the memory region
 */
void* pcie_osal_malloc_aligned(uint32 size, byte align)
{
   void *mem, *mem_aligned;
   byte offset;

   mem = pcie_osal_malloc(size + align);

   if(NULL == mem)
      return(mem);

   mem_aligned = (void*)(((uint32)mem + align) & ~(align - 1));
   offset = (uint32)mem_aligned - (uint32)mem;

   /* Pad the unused region with alignment offset */
   pcie_osal_memset(mem, offset, offset);

   return(mem_aligned);
}

/* ============================================================================
**  Function : pcie_osal_free_aligned
** ============================================================================
*/
/**
 * Free an aligned chunk of memory
 *
 * @param[in,out]    void*    memory to free
 *
 * @return           void
 */
void pcie_osal_free_aligned(void *mem)
{
   uint8 pad, i;

   if(NULL == mem)
      return;

   pad = *(((uint8 * )mem) - 1);

   for(i = 0;i < pad && *(((uint8 * )mem) - i - 1) == pad;i++);

   if(i != pad)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_osal_free_aligned: pad corrupted, fatal error");
   }

   pcie_osal_free((uint8 * )mem - i);
}

/* ============================================================================
**  Function : pcie_osal_memcpy_reg
** ============================================================================
*/
/**
 * Copies uint32 data from source to destination.
 *
 * @param[in,out]   d      Pointer to the destination memory
 * @param[in,out]   s      Pointer to the source memory
 * @param[in]       size   Number of bytes to be copied (usually a multiple of 4 since we have 32 bit registers)
 *
 * @return          void
 */
void pcie_osal_memcpy_reg(uint32 *d, uint32 *s, uint32 size)
{
   uint32 *dst = d;
   const uint32 *src = s;
   uint32 m = (size % 4) ? ((uint32)(size / 4) + 1) : (size / 4);
#ifndef PCIE_BOOT
   /* Pet the watch dog */
   dog_force_kick();
#endif /* PCIE_BOOT */
   while(m--)
   {
      *dst++ = *src++;
   }
}

/* ============================================================================
**  Function : pcie_osal_memcpy
** ============================================================================
*/
/**
 * Performs a CPU memory copy
 *
 * @param[in,out]   dest         Pointer to the destination buffer
 * @param[in]       dest_size    Destination buffer size
 * @param[in,out]   src          Pointer to the source buffer
 * @param[in]       src_size     Source buffer size
 *
 * @return          uint32
 *                  Number of bytes copied
 */
uint32 pcie_osal_memcpy(void* dest, uint32 dest_size, void* src, uint32 src_size)
{
   return(DALSYS_memscpy(dest, dest_size, src, src_size));
}

/* ============================================================================
**  Function : pcie_osal_sleep
** ============================================================================
*/
/**
 * Suspends the client task for the specified time.
 * Task will surely resume (non-deferrable) after time specified
 *
 * @param[in]    time    time in msec.
 *
 * @return       void
 */
void pcie_osal_sleep(uint32 time_ms)
{
#ifndef PCIE_BOOT
   timer_sleep(time_ms, T_MSEC, TRUE);
#endif /* PCIE_BOOT */
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_osal_intlock
** ============================================================================
*/
/**
 * Lock all interrupts
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_osal_intlock(void)
{
   rex_int_lock();
}

/* ============================================================================
**  Function : pcie_osal_intfree
** ============================================================================
*/
/**
 * Free all interrupt locking
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_osal_intfree(void)
{
   rex_int_free();
}

/* ============================================================================
**  Function : pcie_osal_get_timetick
** ============================================================================
*/
/**
 * Get time ticks
 *
 * @param[in]      void
 *
 * @return         uint64
 *    returns the current tick
 */
uint64 pcie_osal_get_timetick(void)
{
   DalTimetickTime64Type timetick = 0;
   DALResult result;

   if(pcie_osal_timetick_handle != NULL)
   {
      result = DalTimetick_GetTimetick64(pcie_osal_timetick_handle, &timetick);
      if(result != DAL_SUCCESS)
      {
         pcie_osal_debug(MSG_FATAL, "DalTimetick_GetTimetick64 failed");
         timetick = 0;
      }
   }
   else
   {
      pcie_osal_debug(MSG_FATAL, "pcie_osal_timetick_handle is NULL");
      timetick = 0;
   }
   return((uint64)timetick);
}

/* ============================================================================
**  Function : pcie_osal_timetick_diff_us
** ============================================================================
*/
/**
 * Get time difference in microseconds
 *
 * @param[in]      end_time      End time in ticks
 * @param[in]      start_time    Start time in ticks
 *
 * @return         uint64
 *    Time difference in micro seconds
 */
uint64 pcie_osal_timetick_diff_us(uint64 end_time, uint64 start_time)
{
   DalTimetickTime64Type timetick_difference, time_us = 0;
   DALResult result;
   DalTimetickTime64Type max_time;

   max_time = ~(0);

   if(end_time >= start_time)
   {
      timetick_difference = (uint64)(end_time - start_time);
   }
   else
   {
      timetick_difference = (max_time - start_time) + (end_time);
   }

   if(pcie_osal_timetick_handle != NULL)
   {
      result = DalTimetick_CvtFromTimetick64(pcie_osal_timetick_handle,
                                             timetick_difference, T_USEC, &time_us);
      if(result != DAL_SUCCESS)
      {
         pcie_osal_debug(MSG_FATAL, "DalTimetick_CvtFromTimetick64 failed");
         time_us = 0;
      }
   }
   else
   {
      pcie_osal_debug(MSG_FATAL, "pcie_osal_timetick_handle is NULL");
      time_us = 0;
   }

   return((uint64)time_us);
}

/* ============================================================================
**  Function : pcie_osal_busywait_us
** ============================================================================
*/
/**
 * Busy wait execution of a thread for a specified time
 *
 * @param[in]      pause_time_us    time in us
 *
 * @return         void
 */
void pcie_osal_busywait_us(uint32 pause_time_us)
{
   busywait(pause_time_us);
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_osal_get_systime_ms
** ============================================================================
*/
/**
 * Get system time in ms
 *
 * @param[in]      void
 *
 * @return         uint64
 *    Time in ms
 */
uint64 pcie_osal_get_systime_ms(void)
{
#ifndef PCIE_BOOT
   time_type time_ms;

   time_get_ms(time_ms);
   return(((uint64)time_ms[1] << 32) | (uint64)time_ms[0]);
#else
   return(uint64)(~0);
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_osal_mallocmemregion
** ============================================================================
*/
/**
 * Allocates a physically contiguous memory location using OS specific API. The
 * memory allocated is expected to be share-able with the hw.
 *
 * @param[in,out]  meminfo    Info structure for the memory allocated.
 * @param[in]      size       Size of the memory to be allocated.
 * @param[in]      map_op     Maping type of Device or memory region.
 *
 * @return         void
 *
 * @sa
 * pcie_osal_freememregion.
 */
void pcie_osal_mallocmemregion(pcie_osal_meminfo *meminfo, uint32 size, pcie_mapping_op_type map_op)
{
   DALSYSMemInfo dalmeminfo;

   if(meminfo == NULL)
   {
      return;
   }

#ifdef PCIE_BOOT
      if(meminfo->pa)
      {
         meminfo->va = meminfo->pa;
         meminfo->handle = (void *)0xDEAD;
         return;
      }
#endif

   if(PCIE_DEVICE_MAPPING == map_op)
   {
      if(DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO | DALSYS_MEM_PROPS_PHYS_CONT,
                                              DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                              meminfo->pa ? meminfo->pa : DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                              size,
                                              &meminfo->handle,
                                              NULL))
      {
         meminfo->handle = NULL;
         return;
      }
   }
   else if(PCIE_MEMORY_MAPPING == map_op)
   {

      if(DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED | DALSYS_MEM_PROPS_PHYS_CONT,
                                              DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                              meminfo->pa ? meminfo->pa : DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                              size,
                                              &meminfo->handle,
                                              NULL))
      {
         meminfo->handle = NULL;
         return;
      }
   }
   else
   {
      return;
   }

   DALSYS_MemInfo(meminfo->handle, &dalmeminfo);

   if(meminfo->pa != 0)
   {
      /* Check that requested region lies within the allocated space */
      if(meminfo->pa < dalmeminfo.PhysicalAddr ||
         meminfo->pa + size > dalmeminfo.PhysicalAddr + dalmeminfo.dwLen)
      {
         DALSYS_DestroyObject(meminfo->handle);
         meminfo->handle = NULL;
         return;
      }

      /* Allocated region may not start right at the requested physical address
      so the virtual address has to be adjusted accordingly */
      meminfo->va = dalmeminfo.VirtualAddr + meminfo->pa - dalmeminfo.PhysicalAddr;
   }
   else
   {
      meminfo->va = dalmeminfo.VirtualAddr;
      meminfo->pa = dalmeminfo.PhysicalAddr;
   }

   meminfo->size = dalmeminfo.dwLen;

   return;
}

/* ============================================================================
**  Function : pcie_osal_freememregion
** ============================================================================
*/
/**
 * Frees the memory allocated using pcie_osal_mallocmemregion.
 *
 * @param[in,out]  meminfo    Info structure for the memory region to be freed.
 *
 * @return         void
 *
 * @sa
 * pcie_osal_mallocmemregion.
 */
void pcie_osal_freememregion(pcie_osal_meminfo *meminfo)
{
   if(meminfo != NULL && meminfo->handle != NULL)
   {
      DALSYS_DestroyObject(meminfo->handle);
   }
}

/* ============================================================================
**  Function : pcie_osal_cachecmd
** ============================================================================
*/
/**
 * Performs a cache maintanence operation
 *
 * @param[in]      cache_op    Cache Operation to perform (clean/flush/invalidate)
 * @param[in,out]  buf         Memory for which cache maintanence to be performed
 * @param[in]      size        Size of the memory
 *
 * @return     void
 */
void pcie_osal_cachecmd(pcie_cache_op_type cache_op,void* buf, uint32 size)
{
#ifndef PCIE_BOOT
    switch (cache_op)
    {
       case PCIE_CACHE_INVALIDATE:
          KxCache_DCacheInvalidateBytes((addr_t)buf, size);
          break;
       case PCIE_CACHE_FLUSH:
          KxCache_DCacheFlushBytes((addr_t)buf, size);
          break;
       case PCIE_CACHE_CLEAN:
          break;
       default:
          return;
    }
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_osal_va_to_pa
** ============================================================================
*/
/**
 * Converts Virtual address to Physical address
 *
 * @param[in,out]  buf         Virtual Address
 *
 * @return         void*       Physical Address
 */
void* pcie_osal_va_to_pa(void* buf)
{
#ifndef PCIE_BOOT
   return ((void*)okl4_va_to_pa((okl4_word_t)buf));
#else
   return ((void*)buf);
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_osal_memorybarrier
** ============================================================================
*/
/**
 * Performs a processor specific memory barrier operation.
 *
 * @param     void
 *
 * @return    void
 */
void pcie_osal_memorybarrier(void)
{
   DALFW_MemoryBarrier();
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_osal_atomic_inc
** ============================================================================
*/
/**
 * Atomically increment
 *
 * @param[in,out]    addr     Address of uint32 value to increment
 *
 * @return           uint32   The value after increament
 */
uint32 pcie_osal_atomic_inc(uint32 *addr)
{
   return ((uint32)okl4_atomic_inc_return((okl4_atomic_word_t * )addr));
}

/* ============================================================================
**  Function : pcie_osal_atomic_dec
** ============================================================================
*/
/**
 * Atomically decrement
 *
 * @param[in]    addr    Address of uint32 value to increment
 *
 * @return       uint32  The value after decreament
 */
uint32 pcie_osal_atomic_dec(uint32 *addr)
{
   return ((uint32)okl4_atomic_dec_return((okl4_atomic_word_t * )addr));
}

/* ============================================================================
**  Function : pcie_osal_atomic_set
** ============================================================================
*/
/**
 * Atomically sets the value
 *
 * @param[in]    addr    Address of uint32 value to set
 * @param[in]    value   Value to be set
 *
 * @return       unsigned long
 *               The value of the variable after set
 */
unsigned long pcie_osal_atomic_set(uint32 *addr,uint32 value)
{
   return(okl4_atomic_set((okl4_atomic_word_t * )addr,(okl4_atomic_plain_word_t)value));
}

/* ============================================================================
**  Function : pcie_osal_atomic_read
** ============================================================================
*/
/**
 * Atomically read
 *
 * @param[in]    addr    Address of uint32 value to read
 *
 * @return       unsigned long
 *               The value of the variable
 */
unsigned long pcie_osal_atomic_read(uint32 *addr)
{
   return(okl4_atomic_read((okl4_atomic_word_t * )addr));
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_osal_syncinit
** ============================================================================
*/
/**
 * Initializes a sync object
 *
 * @param[in,out]   sync     Pointer to the object to be used for sync
 *
 * @return          pcie_return_status
 *    Status of the operation.
 */
pcie_return_status pcie_osal_syncinit(pcie_osal_sync_type *sync)
{
   if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
                                       (DALSYSSyncHandle * )sync,
                                       NULL))
   {
      *(DALSYSSyncHandle * )sync = NULL;
      return(PCIE_FAILED);
   }
   return(PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_osal_syncdestroy
** ============================================================================
*/
/**
 * Destroys a synchronization object
 *
 * @param[in,out]   sync    Pointer to the Sync object to be destroyed
 *
 * @return          pcie_return_status
 *    Status of the operation.
 */
pcie_return_status pcie_osal_syncdestroy(pcie_osal_sync_type *sync)
{
   DALSYS_DestroyObject((DALSYSObjHandle * )sync);
   *(DALSYSSyncHandle * )sync = NULL;
   return(PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_osal_syncenter
** ============================================================================
*/
/**
 * Enters into a synchronized context
 *
 * @param[in,out]    sync    Sync object to be used
 *
 * @return           void
 */
void pcie_osal_syncenter(pcie_osal_sync_type *sync)
{
   DALSYS_SyncEnter(*((DALSYSSyncHandle * )sync));
}

/* ============================================================================
**  Function : pcie_osal_syncleave
** ============================================================================
*/
/**
 * Leaves a synchronized context
 *
 * @param[in,out]    sync    Sync object to be used
 *
 * @return           void
 */
void pcie_osal_syncleave(pcie_osal_sync_type *sync)
{
   DALSYS_SyncLeave(*((DALSYSSyncHandle * )sync));
}

/* ============================================================================
**  Function : pcie_osal_debug_i
** ============================================================================
*/
/**
 * Debug message logging interface. Depending upon the OSAL implementation, it
 * could be printed to a term window or sent as a diag msg or saved to a file
 *
 * @param[in,out]  file      File name
 * @param[in]      line      Line number
 * @param[in]      msgLevel  Message priority
 * @param[in]      nArgs     Number of arguments
 * @param[in]      fmt       Format of the variable
 *
 * @return         void
 */
void pcie_osal_debug_i(const char *file, uint16 line, pcie_osal_msg_type msgLevel, uint32 nArgs, const char *fmt, ...)
{
#ifndef PCIE_BOOT
   va_list arglist;

   if(pcie_osal_ulog_enable != TRUE)
   {
      return;
   }
   if(PCIE_OSAL_MSG_LEVEL > msgLevel)
   {
      return;
   }

   va_start(arglist, fmt);

   /* Ulog messages print */
   ULogFront_RealTimeVprintf(pcieulog, nArgs, fmt, arglist);

   /* For fatal call err fatal */
   if(msgLevel == MSG_FATAL)
   {
      err_const_type err_msg = {0,NULL,NULL};
	  err_msg.line = line;
	  err_msg.fname = file;
	  err_msg.fmt = "(%s): ";
	  err_Fatal_aux_msg ((const err_const_type*)&err_msg, fmt, nArgs);
   }

   va_end(arglist);
#else
   /* If error is fatal call error fatal handler */
   if (msgLevel == MSG_FATAL)
   {
      BL_ERR_FATAL(0);
   }
#endif /* PCIE_BOOT */
}


/**
 * @file pcieosal.h
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
#ifndef _PCIE_OSAL_H_
#define _PCIE_OSAL_H_

#include "com_dtypes.h"

#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(0, ## __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define VA_NUM_ARGS_IMPL(_0,_1,_2,_3,_4,_5,_6, _7, _8, _9, _10, N,...) N

#define pcie_osal_debug(LEVEL, FMT, ...) pcie_osal_debug_i(__FILE__, __LINE__, LEVEL, (VA_NUM_ARGS(__VA_ARGS__)+1), "(%s): " FMT,__func__,##__VA_ARGS__)

/** Information on a piece of memory allocated */
typedef struct _pcie_meminfo {
   uint32 pa;              /** < -- Physical address of the memory */
   uint32 va;              /** < -- Virtual address of the memory */
   uint32 size;            /** < -- Size of the memory */
   void *handle;           /** < -- Target specific handle to the memory (reserved) */
}pcie_osal_meminfo;

/** Flags for device nad memory mapping */
typedef enum
{
   PCIE_DEVICE_MAPPING = 0x0,     /** < --- Mapps PCIE device space */
   PCIE_MEMORY_MAPPING = 0x1      /** < --- Maps DDR Memory space */
}pcie_mapping_op_type;

/** Flags for various cache operations */
typedef enum
{
   PCIE_CACHE_INVALIDATE  = 0x0,      /** < -- Invalidates a cache */
   PCIE_CACHE_FLUSH       = 0x1,      /** < -- Flush a cache */
   PCIE_CACHE_CLEAN       = 0x2       /** < -- Clean a cache */
}pcie_cache_op_type;

/** Sync object */
typedef struct {
   uint32 none;
}pcie_osal_sync_type;

/** Debug Message Type */
typedef enum {
   MSG_LOW = 1,         /** < -- Low priority */
   MSG_MEDIUM,         /** < -- Medium priority */
   MSG_HIGH,           /** < -- High priority */
   MSG_ERR,            /** < -- Error */
   MSG_FATAL           /** < -- Fatal Error */
}pcie_osal_msg_type;

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
void pcie_osal_init(void);

/* ============================================================================
**  Function : pcie_osal_deinit
** ============================================================================
*/
/**
 * DeInitializes OSAL Interfaces. Performs any deinitialization pertaining to the
 * OS specific APIs.
 *
 * @param[in]    void
 *
 * @return       void
 */
void pcie_osal_deinit(void);

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
void* pcie_osal_malloc(uint32 size);

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
void pcie_osal_free(void *mem);

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
void pcie_osal_memset(void *mem, uint8 val, uint32 size);

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
void* pcie_osal_malloc_aligned(uint32 size, byte align);

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
void pcie_osal_free_aligned(void *mem);

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
void pcie_osal_memcpy_reg(uint32 *d, uint32 *s, uint32 size);

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
uint32 pcie_osal_memcpy(void* dest, uint32 dest_size, void* src, uint32 src_size);

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
void pcie_osal_sleep(uint32 time_ms);

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
void pcie_osal_intlock(void);

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
void pcie_osal_intfree(void);

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
uint64 pcie_osal_get_timetick(void);

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
uint64 pcie_osal_timetick_diff_us(uint64 end_time, uint64 start_time);

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
void pcie_osal_busywait_us(uint32 pause_time_us);

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
uint64 pcie_osal_get_systime_ms(void);

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
void pcie_osal_mallocmemregion(pcie_osal_meminfo *meminfo, uint32 size, pcie_mapping_op_type map_op);

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
void pcie_osal_freememregion(pcie_osal_meminfo *meminfo);

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
void pcie_osal_cachecmd(pcie_cache_op_type cache_op,void* buf, uint32 size);

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
void* pcie_osal_va_to_pa(void* buf);

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
void pcie_osal_memorybarrier(void);

/* ============================================================================
**  Function : pcie_osal_atomic_inc
** ============================================================================
*/
/**
 * Atomically increment
 *
 * @param[in,out]    addr     Address of uint32 value to increment
 *
 * @return           void
 */
uint32 pcie_osal_atomic_inc(uint32 *addr);

/* ============================================================================
**  Function : pcie_osal_atomic_dec
** ============================================================================
*/
/**
 * Atomically decrement
 *
 * @param[in]    addr    Address of uint32 value to increment
 *
 * @return       void
 */
uint32 pcie_osal_atomic_dec(uint32 *addr);

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
unsigned long pcie_osal_atomic_set(uint32 *addr,uint32 value);

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
unsigned long pcie_osal_atomic_read(uint32 *addr);

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
enum _pcie_return_status pcie_osal_syncinit(pcie_osal_sync_type *sync);

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
enum _pcie_return_status pcie_osal_syncdestroy(pcie_osal_sync_type *sync);

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
void pcie_osal_syncenter(pcie_osal_sync_type *sync);

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
void pcie_osal_syncleave(pcie_osal_sync_type *sync);

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
void pcie_osal_debug_i(const char *file, uint16 line, pcie_osal_msg_type msgLevel, uint32 nArgs, const char *fmt, ...);

#endif /* _pcie_OSAL_H_ */

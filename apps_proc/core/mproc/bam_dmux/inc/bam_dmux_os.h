/*=============================================================================

                            BAM DMUX OS

 BAM Data Multiplexer OS interface

 Copyright  2016-2017 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/bam_dmux/inc/bam_dmux_os.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   rv      Initial version.
===========================================================================*/

#ifndef BAM_DMUX_OS_H
#define BAM_DMUX_OS_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "stdlib.h"
#include "string.h"
#include "stringl.h"
#include "qurt.h"
#include "qurt_memory.h"
#include "timer.h"
#include "bam_dmux_log.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/
#ifndef BAM_DMUX_LOG_TAG
#define BAM_DMUX_LOG_TAG "OS"
#endif

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/
typedef qurt_mutex_t bam_dmux_os_mutex_type;
typedef timer_type   bam_dmux_os_timer_type;
typedef void (*bam_dmux_os_timer_cb_type)(void *);

/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/
extern bam_dmux_os_mutex_type bam_dmux_os_atomic;

/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_os_malloc
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void *bam_dmux_os_malloc(uint32 size)
{
  return malloc(size);
}

/*=============================================================================
  FUNCTION  bam_dmux_os_calloc
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void *bam_dmux_os_calloc(uint32 count, uint32 size)
{
  return calloc(count, size);
}

/*=============================================================================
FUNCTION  bam_dmux_os_calloc_assert_dbg
=============================================================================*/
/**
* @param[in]
*
* @return
*
* @sideeffects
*/
/*===========================================================================*/
static inline void *bam_dmux_os_calloc_assert_dbg
(
  uint32      count,
  uint32      size,
  uint32      line,
  const char *func
)
{
  void *mem;

  mem = bam_dmux_os_calloc(count, size);
  if (mem == NULL)
  {
    BAM_DMUX_LOG_FATAL(4, "Calloc failed count=%d, size=%d, func=%s, line=%d.",
        count, line, func, line);
  }

  return mem;
}


#define bam_dmux_os_calloc_assert(count, size) \
  bam_dmux_os_calloc_assert_dbg(count, size, __LINE__, __FUNCTION__)

/*=============================================================================
  FUNCTION  bam_dmux_os_free
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_free(void *ptr)
{
  free(ptr);
}

/*=============================================================================
  FUNCTION  bam_dmux_os_mutex_init
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_mutex_init(bam_dmux_os_mutex_type *mutex)
{
  qurt_mutex_init(mutex);
}

/*=============================================================================
  FUNCTION  bam_dmux_os_mutex_deinit
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_mutex_deinit(bam_dmux_os_mutex_type *mutex)
{
  qurt_mutex_destroy(mutex);
}

/*=============================================================================
  FUNCTION  bam_dmux_os_mutex_lock
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_mutex_lock(bam_dmux_os_mutex_type *mutex)
{
  qurt_mutex_lock(mutex);
}

/*=============================================================================
  FUNCTION  bam_dmux_os_mutex_unlock
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_mutex_unlock(bam_dmux_os_mutex_type *mutex)
{
  qurt_mutex_unlock(mutex);
}

/*=============================================================================
  FUNCTION  bam_dmux_os_va_to_pa
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline uint32 bam_dmux_os_va_to_pa(void *vaddr)
{
  return qurt_lookup_physaddr((qurt_addr_t)vaddr);
}

/*=============================================================================
  FUNCTION  bam_dmux_os_uncached_malloc
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
void *bam_dmux_os_uncached_malloc(uint32 size, uint32 *paddr);

/*=============================================================================
  FUNCTION  bam_dmux_os_mem_barrier
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
void qurt_mem_barrier(void);
static inline void bam_dmux_os_mem_barrier(void)
{
  qurt_mem_barrier();
}

/*=============================================================================
  FUNCTION  bam_dmux_os_cache_flush
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_cache_flush(void *vaddr, uint32 size)
{
  int ret;

  ret = qurt_mem_cache_clean((qurt_addr_t)vaddr,
                             size,
                             QURT_MEM_CACHE_FLUSH_INVALIDATE,
                             QURT_MEM_DCACHE);
  if (ret != QURT_EOK)
  {
    BAM_DMUX_LOG_FATAL(3, "qurt_mem_cache_clean cache flush failed!, "
                       "vaddr=0x%08X, size=%d, ret=%d.", (uint32)vaddr, size, ret);
  }
}

/*=============================================================================
  FUNCTION  bam_dmux_os_cache_invalidate
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_cache_invalidate(void *vaddr, uint32 size)
{
  int ret;

  ret = qurt_mem_cache_clean((qurt_addr_t)vaddr,
                             size,
                             QURT_MEM_CACHE_INVALIDATE,
                             QURT_MEM_DCACHE);
  if (ret != QURT_EOK)
  {
    BAM_DMUX_LOG_FATAL(3, "qurt_mem_cache_clean cache invalidate failed!, "
                       "vaddr=0x%08X, size=%d, ret=%d.", (uint32)vaddr, size, ret);
  }
}

/*=============================================================================
  FUNCTION  bam_dmux_os_timer_init
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_timer_init(bam_dmux_os_timer_type    *timer,
                                          bam_dmux_os_timer_cb_type cb,
                                          void                      *cb_ctx)
{
  timer_error_type ret;

  ret = timer_def_osal(timer, &timer_non_defer_group, TIMER_FUNC1_CB_TYPE, cb,
                       (time_osal_notify_data)cb_ctx);

  if (ret != TE_SUCCESS)
  {
    BAM_DMUX_LOG_FATAL(3, "timer_def_osal failed!, cb=0x%X, cb_ctx=0x%X, "
                       "ret=%d.", cb, cb_ctx, ret);
  }
}

/*=============================================================================
  FUNCTION  bam_dmux_os_timer_deinit
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_timer_deinit(bam_dmux_os_timer_type *timer)
{
  timer_error_type ret;

  ret = timer_undef(timer);
  if (ret != TE_SUCCESS)
  {
    BAM_DMUX_LOG_FATAL(3, "timer_undef failed!, timer=0x%X, ret=%d.",
                       timer, ret, 0);
  }
}

/*=============================================================================
  FUNCTION  bam_dmux_os_timer_set
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_timer_set(bam_dmux_os_timer_type *timer,
                                         uint32                 timeout_ms)
{
  timer_set(timer, timeout_ms, 0, T_MSEC);
}

/*=============================================================================
  FUNCTION  bam_dmux_os_timer_clr
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline uint32 bam_dmux_os_timer_clr(bam_dmux_os_timer_type *timer)
{
  timetick_type remaining_ms;

  remaining_ms = timer_clr(timer, T_MSEC);
  return (uint32)remaining_ms;
}

/*=============================================================================
  FUNCTION  bam_dmux_os_sleep
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_sleep(uint32 usec)
{
  timer_sleep(usec, T_USEC, FALSE);
}


/*=============================================================================
  FUNCTION  bam_dmux_os_atomic_inc
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline uint32 bam_dmux_os_atomic_inc(volatile uint32 *loc)
{
  uint32 val;

  bam_dmux_os_mutex_lock(&bam_dmux_os_atomic);
  val = ++(*loc);
  bam_dmux_os_mutex_unlock(&bam_dmux_os_atomic);

  return val;
}

/*=============================================================================
  FUNCTION  bam_dmux_os_atomic_dec
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline uint32 bam_dmux_os_atomic_dec(volatile uint32 *loc)
{
  uint32 val;

  bam_dmux_os_mutex_lock(&bam_dmux_os_atomic);
  val = --(*loc);
  bam_dmux_os_mutex_unlock(&bam_dmux_os_atomic);

  return val;
}

/*=============================================================================
  FUNCTION  bam_dmux_os_atomic_set_mask
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_atomic_set_mask(volatile uint32 *loc, uint32 mask)
{
  volatile uint32 curr;
  
  bam_dmux_os_mutex_lock(&bam_dmux_os_atomic);
  
  curr = *loc;
  curr |= mask;
  *loc = curr;
  
  bam_dmux_os_mutex_unlock(&bam_dmux_os_atomic);
}

/*=============================================================================
  FUNCTION  bam_dmux_os_atomic_get_mask
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline uint32 bam_dmux_os_atomic_get_mask(volatile uint32 *loc)
{
  return *loc;
}

/*=============================================================================
  FUNCTION  bam_dmux_os_atomic_clr_mask
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline uint32 bam_dmux_os_atomic_clr_mask(volatile uint32 *loc, uint32 mask)
{
  volatile uint32 curr;

  bam_dmux_os_mutex_lock(&bam_dmux_os_atomic);
  curr = *loc;
  curr &= ~mask;
  *loc = curr;
  bam_dmux_os_mutex_unlock(&bam_dmux_os_atomic);

  return curr;
}

/*=============================================================================
  FUNCTION  bam_dmux_os_get_thread_id
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
uint32 bam_dmux_os_get_thread_id(void);

/*=============================================================================
  FUNCTION  bam_dmux_os_init
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static inline void bam_dmux_os_init(void)
{
  bam_dmux_os_mutex_init(&bam_dmux_os_atomic);
}

#ifdef __cplusplus
}
#endif

#endif /* BAM_DMUX_OS_H */

#ifndef MHIOSAL_H
#define MHIOSAL_H

/**
  @file mhi_osal.h
  @brief
  This file implements the interfaces that provide O/S abstraction   
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/06/15   nk      Add sys timetick api
11/17/14   am      Enable/disable and read irq state
11/03/14   am      Get system time in ms
01/31/13   nk      Added atomic increment and decrement API's
10/01/13   am      Added memcpy
06/25/13   nk      Added mem map
02/20/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi.h" 
#include "mhi_i.h"

/* DAL */
#include "DALSys.h"
#include "DALStdErr.h"
#include "DALFramework.h"
#include "DALDeviceId.h"

#ifndef MHI_BOOT
#include "timer.h"
#endif

/* Message Level */ 
enum {
        MSG_LOW=1,
        MSG_MEDIUM,
        MSG_HIGH, 
        MSG_ERR,
        MSG_FATAL
     }; 

/* 32 bit Debug Message bit mask. Lower 4 bits reserved for message level */
#define GENERAL_LOG   0x10
#define PROFILING_LOG 0x20

typedef struct {
	uint32 mutex; /* dummy */ 
}mhi_osal_sync_type;

/* OSAL Thread Object */ 
typedef struct
{
   DALSYSWorkLoopHandle handle;      /* Handle to DAL Work Loop */ 
   DALSYSEventHandle    startEvent;  /* Start event used to start thread */ 
   void                 *arg;        /* Arguments to pass to thread */ 
   void (*entry)(void *arg);         /* Thread entry point function */ 
}mhi_osal_thread_t;

/* Memory info structure */
typedef struct _mhi_meminfo{
    uint32 pa;     /**< -- Physical address of the memory */
    uint32 va;     /**< -- Virtual address of the memory */
    uint32 size;   /**< -- Size of the memory */
    void *handle;  /**< -- Target specific handle to the memory (reserved) */
}mhi_osal_meminfo;

/** @name IrqHandle Object */
typedef struct {   
    uint32           dwIrqId;        /**< -- IRQ number to be registered to */
}mhi_osal_irq_type;

/* Event Handle */
typedef void* mhi_osal_event_handle; 

/* Event Object */
typedef DALSYSEventObj mhi_osal_event;

#ifndef MHI_BOOT
/* Timer object */
typedef timer_type mhi_timer_t;

/* Timer callback */
typedef timer_t2_cb_type mhi_timer_cb; 

/* User data to be supplied to timer callback */
typedef timer_cb_data_type mhi_timer_user_data;

/* Time in ms after which callback should be invoked */
typedef timetick_type mhi_timer_time_ms;

/* Time after first timer expiry the timer should expire again and invoke the timer callback */
typedef timetick_type mhi_timer_reload_ms;
#else
typedef uint32 mhi_timer_t;
#endif /* MHI_BOOT */

/* Cache operations */
typedef enum _mhi_osal_cache_op_t
{
   MHI_CACHE_OP_CLEAN,
   MHI_CACHE_OP_FLUSH, 
   MHI_CACHE_OP_INVALIDATE
}mhi_osal_cache_op_t;

/** Flags for device and memory mapping */
typedef enum
{
   MHI_DEVICE_MAPPING = 0x0,     /** < --- address region has device attribute  */
   MHI_MEMORY_MAPPING = 0x1      /** < --- address region has memory attribute  */
}mhi_mapping_op_type;


/* ============================================================================
 **  Function : mhi_osal_init
 ** ============================================================================
 */
/**
  Initializes OSAL Interfaces.

  Initializes OSAL Interfaces. Performs any initialization pertaining to the
  OS specific APIs.

  @param  None

  @return
  None.

*/
void mhi_osal_init(void);

/** 
 * @brief      mhi_osal_spawn_thread
 *
 *             Spawn a worker thread 
 *              
 * 
 * @param[in]  *obj   - Thread object
 * @param[in]  *name  - Name of thread
 * @param[in]  *entry - Entry point function 
 * @param[in]  *ctx   - Context to pass to entry
 * @param[in]  priority - Priority of thread
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_osal_spawn_thread(mhi_osal_thread_t *obj, char* name, void *entry, void *ctx, uint32 priority);

/** 
 * @brief      mhi_osal_eventcreate
 *
 *             Create an event 
 *              
 * 
 * @param[out]  *handle - Handle to event
 * @param[in]   *obj    - Event object
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MHI_SUCCESS if successful otherwise MHI_ERROR
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_osal_eventcreate(mhi_osal_event_handle *handle, mhi_osal_event *obj);

/** 
 * @brief      mhi_osal_eventdestroy
 *
 *             Destroy the event
 *              
 * 
 * @param[in]  *handle - handle to event
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MHI_SUCCESS
 *
 * @sideeffect None 
 */
mhi_status_t mhi_osal_eventdestroy(mhi_osal_event_handle *handle );

/** 
 * @brief      mhi_osal_eventsignal
 *
 *             Signal event
 *              
 * 
 * @param[in]  handle
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_eventsignal(mhi_osal_event_handle handle);

/** 
 * @brief      mhi_osal_eventwait
 *
 *             Wait for an event to be signalled
 *              
 * 
 * @param[in]  handle - handle to event
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_eventwait(mhi_osal_event_handle handle);

/** 
 * @brief      mhi_osal_isrmask
 *
 *             Mask Interrupt
 *              
 * 
 * @param[in]  *irqhandle - Handle to IRQ
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_isrmask(mhi_osal_irq_type *irqhandle); 

/** 
 * @brief      mhi_osal_isrunmask
 *
 *             Unmask interrupt
 *              
 * 
 * @param[in]  *irqhandle - Handle to IRQ
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_isrunmask(mhi_osal_irq_type *irqhandle); 

/* ============================================================================
 **  Function : mhi_osal_israck
 ** ============================================================================
 */
/**
  Acknowledges the IRQ.

  Acknowledges the IRQ. Some OS implementations require the ISR to ack the IRQ
  to re-activate the IRQ.

  @param[in,out]  irqhandle    Interrupt handle

  @return
  None.

*/
void mhi_osal_israck(mhi_osal_irq_type *irqhandle);

/* ============================================================================
 **  Function : mhi_osal_isrinstall
 ** ============================================================================
 */
/**
  Registers an isr to an IRQ id.

  Registers with the interrupt controller, an ISR to service an IRQ.

  @param[in,out]  irqhandle    Handle to be used with the interrupt controller
  @param[in]      irq          IRQ ID to be registered for
  @param[in,out]  isr          ISR to be invoked to the service the irq
  @param[in,out]  data         data to be supplied to the isr upon invocation

  @return
  Success of the operation.

  @sa
  Other_Relevant_APIS_or_Delete.
  */
mhi_status_t mhi_osal_isrinstall(mhi_osal_irq_type *irqhandle,uint32 irq,void *isr, void *data);

/* ============================================================================
 **  Function : mhi_osal_isruninstall
 ** ============================================================================
 */
/**
  Deregisters the ISR from the interrupt controller.

  Deregisters the ISR from the interrupt controller.

  @param[in,out]  irqhandle    Interrupt controller handle to be used

  @return
  None.

*/
void mhi_osal_isruninstall(mhi_osal_irq_type *irqhandle);

/** 
 * @brief      mhi_osal_disable_irq
 *
 *             Disable IRQ 
 *              
 * @param[in]  irqhandle - Handle to be used with the interrupt controller
 *
 * @dependencies
 *             mhi_osal_init
 *              
 * 
 * @return       
 *             MHI_SUCCESS/MHI_ERROR
 * 
 * @sideeffect None 
 */  
mhi_status_t mhi_osal_disable_irq(mhi_osal_irq_type irqhandle);

/** 
 * @brief      mhi_osal_enable_irq
 *
 *             Enable IRQ 
 *              
 * @param[in]  irqhandle - Handle to be used with the interrupt controller
 *
 * @dependencies
 *             mhi_osal_init
 *              
 * 
 * @return       
 *             MHI_SUCCESS/MHI_ERROR
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_osal_enable_irq(mhi_osal_irq_type irqhandle);

/** 
 * @brief      mhi_osal_is_irq_set
 *
 *             Check with the interrupt controller if the supplied IRQ is set 
 *              
 * @param[in]  irqhandle - Handle to be used with the interrupt controller
 *
 * @dependencies
 *             mhi_osal_init
 *              
 * 
 * @return       
 *             TRUE - Interrupt is set
 *             FALSE - Interrpt is not set 
 * 
 * @sideeffect None 
 */
boolean mhi_osal_is_irq_set(mhi_osal_irq_type irqhandle);

/* ============================================================================
**  Function : mhi_osal_syncinit
** ============================================================================
*/
/**
  Initializes a sync object.
    
  Initializes a sync object.
    
  @param[in,out] sync     Pointer to the object to be used for sync

  @return
  Success of the operation.
 
*/
mhi_status_t mhi_osal_syncinit(mhi_osal_sync_type *sync);

/* ============================================================================
**  Function : mhi_osal_syncdestroy
** ============================================================================
*/
/**
  Destroys a synchronization object.
    
  Destroys a synchronization object.
    
  @param[in,out]  sync    Sync object to be destroyed

  @return
  None.
  
*/
mhi_status_t mhi_osal_syncdestroy(mhi_osal_sync_type *sync);

/* ============================================================================
**  Function : mhi_osal_syncenter
** ============================================================================
*/
/**
  Enters into a synchronized context.
    
  Enters into a synchronized context.
    
  @param[in,out]  sync    Sync object to be used

  @return
  None.
  
*/
void mhi_osal_syncenter(mhi_osal_sync_type *sync);

/* ============================================================================
**  Function : mhi_osal_syncleave
** ============================================================================
*/
/**
  Leaves a synchronized context.
    
  Leaves a synchronized context.
    
  @param[in,out]  sync    Sync object to be used

  @return
  None.
  
*/
void mhi_osal_syncleave(mhi_osal_sync_type *sync);

/* ============================================================================
**  Function : mhi_osal_mallocmemregion
** ============================================================================
*/
/**
  Allocates a physically contiguous memory location using OS specific API.

  Allocates a physically contiguous memory location using OS specific API. The
  memory allocated is expected to be share-able with the hw.

  @param[out]  meminfo    Info structure for the memory allocated.
  @param[in]   size       Size of the memory to be allocated.
  @param[in]   map_op     Memory mapping type.

  @return
  None.

  @sa
  pcie_osal_freememregion.
*/
void mhi_osal_mallocmemregion(mhi_osal_meminfo *meminfo, uint32 size, mhi_mapping_op_type map_op);
/* ============================================================================
**  Function : mhi_osal_malloc
** ============================================================================
*/
/**
  Mallocs a memory (virtual) using OS Malloc API.
    
  Mallocs a memory (virtual) using OS Malloc API. The memory allocated
  is for SW usage only.
    
  @param[in]  size    Size of the memory to be allocated

  @return
  Pointer to the memory allocated. NULL if failed.
  
  @sa
  mhi_osal_mallocmemregion, mhi_osal_free
*/
void* mhi_osal_malloc(uint32 size);


/* ============================================================================
**  Function : mhi_osal_memset
** ============================================================================
*/
/**
  Sets memory of specified size to specified value 
    
  Fills memory mem ,of size size, to value val. 
    
  @param[in] mem Pointer to the block of memory to fill
  @param[in] val Value of byte to fill the memory with  
  @param[in] size Number of bytes to fill  


  @return
  None.
  
*/
void mhi_osal_memset(void *mem, uint8 val, uint32 size);

/* ============================================================================
**  Function : mhi_osal_memcpy
** ============================================================================
*/
/**
  Copies memory of a specofied size to the destination. 
    
  @param[in] dest Pointer to the destination memory address
  @param[in] source Pointer to the source memory address  
  @param[in] size Number of bytes to copy  


  @return
  None.
  
*/
void mhi_osal_memcpy(void *dest, void *source, uint32 size);

/** 
 * @brief      mhi_osal_malloc_aligned
 *
 *             Alloc an aligned chunk of memory
 *              
 * 
 * @param[in]  size - size to malloc in bytes
 * @param[in]  align - amount to align by in bytes no bigger than 255. Must be a power of 2
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Pointer to memory allocated, NULL if none
 * 
 * @sideeffect None 
 */
void* mhi_osal_malloc_aligned(uint32 size, byte align);

/** 
 * @brief      mhi_osal_free_aligned
 *
 *             Free an aligned chunk of memory
 *              
 * 
 * @param[in]  void* - memory to free 
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_free_aligned(void* mem);

/* ============================================================================
**  Function : mhi_osal_free
** ============================================================================
*/
/**
  Frees the memory allocated by mhi_osal_malloc.
    
  Frees the memory (virtual) using OS Free API.   
    
  @param[in,out]  mem    Pointer to the memory to be freed

  @return
  None.
  
  @sa
  mhi_osal_malloc.
*/
void mhi_osal_free(void *mem);

 /** 
 * @brief      mhi_osal_atomic_set_bit
 *
 *             Atomically sets a bit of a uint32 dword 
 *              
 * 
 * @param[in]  *addr  
 * @param[in]  bit
 *
 * @dependencies
 *              
 * 
 * @return       
 *              
 * 
 * @sideeffect  None 
 */
void mhi_osal_atomic_set_bit(uint32 *addr, uint32 bit );
/** 
 * @brief      mhi_osal_atomic_clear
 *
 *             Atomically clears a bit of a uint32 dword 
 *              
 * 
 * @param[in]  *addr  
 * @param[in]  bit
 *
 * @dependencies
 *              
 * 
 * @return       
 *              
 * 
 * @sideeffect  None 
 */
void mhi_osal_atomic_clear_bit(uint32 *addr, uint32 bit);

 /** 
 * @brief      mhi_osal_atomic_dec
 *
 *             Atomically decrement 
 *              
 * 
 * @param[in]  *addr - Address of uint32 value to decrement
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_atomic_dec(uint32 *addr);

/** 
 * @brief      mhi_osal_atomic_inc
 *
 *             Atomically increment 
 *              
 * 
 * @param[in]  *addr - Address of uint32 value to increment
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_atomic_inc(uint32 *addr);

/** 
 * @brief      mhi_osal_wait
 *
 *             Waits for specified amount of milliseconds
 *              
 * 
 * @param[in]  time_in_milli - time in milliseconds to wait 
 *
 * @dependencies
 *              
 * 
 * @return       
 *              
 * 
 * @sideeffect  None 
 */
void mhi_osal_wait(uint32 time_in_milli);


/** 
 * @brief      mhi_osal_timer_init
 *
 *             Defines a simple clock callback style timer. More complexity can
 *             be added to the mhi timer apis in future, if required. 
 *              
 * 
 * @param[in]  *timer - Pointer to MHI timer to define 
 *
 * @dependencies
 *             timer_init() 
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
 */
void mhi_osal_timer_init(mhi_timer_t *timer);

#ifndef MHI_BOOT
/** 
 * @brief      mhi_osal_timer_start
 *
 *             Starts a timer with the supplied parameters 
 *              
 * 
 * @param[in]  *timer - Pointer to MHI timer to define 
 * @param[in]  mhi_timer_cb - callback at timer expiry
 * @param[in]  mhi_timer_user_data - User supplied data to be supplied to callback
 * @param[in]  mhi_timer_time_ms - Time (in ms) until first timer expiry
 * @param[in]  mhi_timer_reload_ms - periodicity of timer in ms
 *
 * @dependencies
 *             timer_init() 
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
 */
void mhi_osal_timer_start(mhi_timer_t *timer, 
                          mhi_timer_cb cb, 
                          mhi_timer_user_data user_data,
                          mhi_timer_time_ms,
                          mhi_timer_reload_ms);

/** 
 * @brief      mhi_osal_timer_stop
 *
 *             Stops the timer associated with the supplied timer handle 
 *              
 * 
 * @param[in]  *timer - Pointer to MHI timer to define  
 *
 * @dependencies
 *             timer_init() 
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None 
 */
void mhi_osal_timer_stop(mhi_timer_t *timer);
#endif /* #ifndef MHI_BOOT */

/** 
 * @brief      mhi_osal_cache_ctrl
 *
 *             Perform a cache operatio on given buffer
 *              
 * 
 * @param[in]  op - Cache operation
 * @param[in]  *addr - Address to perform on
 * @param[in]  size - size
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_cache_ctrl(mhi_osal_cache_op_t op, mhi_addr_t *addr, uint32 size);

/** 
 * @brief      mhi_osal_get_systime_ticks
 *
 *             Return system time ticks 
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             System time ticks
 * 
 * @sideeffect None 
 */
uint64 mhi_osal_get_systime_ticks(void);

/** 
 * @brief      mhi_osal_get_systime_ms
 *
 *             Get system time in ms
 *              
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Time in ms
 * 
 * @sideeffect None 
 */
uint64 mhi_osal_get_systime_ms(void);

/** 
 * @brief      mhi_osal_set_priority
 *
 *             Set priority of calling thread
 *              
 * 
 * @param[in]  pri - Value to set to
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_osal_set_priority(uint32 pri);

/** 
 * @brief      mhi_osal_va_to_pa
 *
 *             Get physical address of a given virtual address
 *              
 *
 * @param[in]  buf - Virtual address
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Physical address
 * 
 * @sideeffect None 
 */
void* mhi_osal_va_to_pa(void* buf);


#ifdef MHI_DEBUG
#ifndef MHI_BOOT

#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(0, ## __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define VA_NUM_ARGS_IMPL(_0,_1,_2,_3,_4,_5,_6, _7, _8, _9, _10, N,...) N

#define mhi_osal_debug(MSGMASK, FMT, ...) mhi_osal_debug_i(__FILE__, __LINE__, MSGMASK, VA_NUM_ARGS(__VA_ARGS__), FMT,##__VA_ARGS__)
void mhi_osal_debug_i(const char* file, uint16 line, uint32 msgMask, uint32 nArgs, const char *fmt, ...);
#else
#define mhi_osal_debug(msgMask, id, args, fmt, ...) mhi_osal_debug_c(msgMask,id,args,##__VA_ARGS__)
void mhi_osal_debug_c(uint32 msgMask, uint32 id, char* args, ...);
#endif
#else
#define mhi_osal_debug(...)
#endif


#endif 


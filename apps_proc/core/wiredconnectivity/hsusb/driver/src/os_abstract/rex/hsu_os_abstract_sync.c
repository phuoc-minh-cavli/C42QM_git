/*==============================================================================

		           H S U _ O S _ A B S T R A C T _ S Y N C . C

  
GENERAL DESCRIPTION
  High-Speed USB Implementation of the REX OS abstraction layer 
  synchronization API for the REX OS.

==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/os_abstract/rex/hsu_os_abstract_sync.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================

	
EXTERNALIZED FUNCTIONS
  Externalized to outside of the OS abstraction layer:
    hsu_os_delay_us
    hsu_os_delay_ms
    hsu_os_get_time
    hsu_os_get_hz
    hsu_os_spinlock_init
    hsu_os_spinlock_uninit
    hsu_os_spinlock_lock
    hsu_os_spinlock_unlock
    hsu_os_thread_create
    hsu_os_msleep
    hsu_os_wakeup
    hsu_os_safe_enter_impl
    hsu_os_safe_leave_impl
    hsu_os_get_context

  Externalized internally in the OS abstraction layer:
    hsu_os_sync_init
    hsu_os_sync_uninit
    hsu_os_safe_enter_d
    hsu_os_safe_leave_d

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hsu_os_init should be called before using the API defined in this file.
		
Copyright (c) 2007, 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/10   gs      Prepended hsu to some os function names.
05/09/06   ds      Created.

===========================================================================*/


/*==============================================================================

LOCAL LINT FLAGS

==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Rex include files */
//#include <rex.h>
#include "hsu_qurt_util.h"
//#include <task.h>

#include <time_svc.h>

/* High Speed USB subsystem include files. */
#include <hsu_common_int.h>

/* USB stack include files */
#include <jerrno.h>

/* Global OS abstraction layer include files */
#include <port_generic_int.h>
#include <port_sync_int.h>

/* For TIMETEST profiling support */
#ifdef TIMETEST
#include "timetest.h"
#endif /* TIMETEST */

/* Local include files. */
#include "hsu_os_abstract_sync.h"

/* C-Runtime library include files */
#include <memory.h>
#include <stdio.h>

#ifdef FEATURE_HS_USB_POLLING
#include "hsu_polling.h"
#endif /* FEATURE_HS_USB_POLLING */

#include "busywait.h"
#include "hsu_timer.h"
#include "hsu_log.h"
#include "qurt_interrupt.h"
#include "rcinit.h"

#ifndef USB_THREADX
#ifdef T_MDM9X15
TASK_EXTERN(hsu_controller)
TASK_EXTERN(hsu_core)
TASK_EXTERN(hsu_driver)
TASK_EXTERN(hsu_other)
#endif /* T_MDM9X15 */
#endif
/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
void hsu_os_thread_wrapper_function( void * pool_index );

/*==============================================================================
  Constants and Macros
==============================================================================*/
//+++ HSU ADDITION for QBI
//#define HSU_STACK_SIZ_FACTOR     2 

//#ifndef CONFIG_TASK_SINGLE
//#define OS_THREAD_STACK_SIZE_IN_BYTES   (1024 * HSU_STACK_SIZ_FACTOR)
//#else
//#define OS_THREAD_STACK_SIZE_IN_BYTES   (1024 * HSU_STACK_SIZ_FACTOR * 2)
//#endif
//--- HSU ADDITION for QBI
#define OS_THREAD_STACK_SIZE_IN_BYTES   8192

#ifdef FEATURE_HS_USB_BAM
#define OS_SPS_SLEEP_HANDLES 1
#else
#define OS_SPS_SLEEP_HANDLES 0
#endif /* FEATURE_HS_USB_BAM */

#define   OS_MAX_THREADS 1
enum
{
  /* Thread pool size. */
  #ifndef USB_THREADX
  #ifdef CONFIG_TASK_SINGLE
  OS_MAX_THREADS          =  1,  
  #elif !defined(FEATURE_HSU_TEST)
  OS_MAX_THREADS          =  6, /* 4 + JMS_ASYNC_THREADS */
  #else
  OS_MAX_THREADS          =  5,
  #endif /* FEATURE_HSU_TEST */
  #endif

  /* msleep handle pool size. */
  #ifdef CONFIG_TASK_SINGLE
  OS_MAX_MSLEEP_HANDLES   = 1 + OS_SPS_SLEEP_HANDLES,
  #else
  #ifdef FEATURE_HS_USB_OTG
  /* when in host mode we'd like to support 3 disk-on-keys,
  which require 20 handles each (worst-case)*/
  OS_MAX_MSLEEP_HANDLES   = 69 + OS_SPS_SLEEP_HANDLES,
  #else /* FEATURE_HS_USB_OTG */
  /* TODO: Remove the next line, as it seems like it is never compiled. 
     This is because CONFIG_TASK_SINGLE is defined in non-OTG build. */
  OS_MAX_MSLEEP_HANDLES   = 60 + OS_SPS_SLEEP_HANDLES,
  #endif /* FEATURE_HS_USB_OTG */
  #endif /* CONFIG_TASK_SINGLE */

  OS_MAX_SPINLOCK_HANDLES = 10,

  OS_MAX_MUTEXES          = 10
};

enum
{
  /* This value must be a power of 2, since the 
  ** implementation assumes it.
  */
  OS_MAX_THREADS_PER_MSLEEP_HANDLE = 4
};

enum
{
  OS_MUTEX_SLEEP_TIME_MS = 3
};

#define HSU_OS_ABSTRACT_SYNC_TIME_WRAPAROUND  0x100000000ll

/*==============================================================================
  Typedefs
==============================================================================*/

typedef uint64 os_sync_msecs_time_type;

/* Execution information of an OS abstraction layer thread. */
typedef struct
{
  #ifdef USB_THREADX
  //qurt_thread_t os_thread_tcb;
  qurt_thread_t os_thread_id;
  qurt_signal_t os_thread_sig;
  #else
  rex_tcb_type         os_thread_tcb;
  unsigned char        os_thread_stack[OS_THREAD_STACK_SIZE_IN_BYTES];
  #endif


  /* The function which is currently executed by the thread.
  ** NULL means that the thread is currently idle.
  */
  jthread_func         thread_function;

  /* The argument passed to the thread function. */
  void*                thread_function_argument;

  /* The thread name */
  char os_thread_name[REX_TASK_NAME_LEN + 1];

} os_thread_pool_entry_type;
typedef os_thread_pool_entry_type os_thread_tcb_t;


/* task msleep status */
typedef enum {
  HSU_OS_SYNC_MLSEEP_INIT_VAL,
  /*status has this value at all times except from the time it wakes up,
    until it's handled in hsu_os_msleep */
  HSU_OS_SYNC_MLSEEP_TIMER_EXPIRED, 
  HSU_OS_SYNC_MSLEEP_WAKEN_UP 
} os_msleep_status_enum; 


/* Information of a thread sleeping on an msleep handle. */
typedef struct 
{
#ifdef USB_THREADX
  //qurt_thread_t os_thread_tcb;
  //qurt_signal_t* os_thread_sig;
  os_thread_tcb_t* thread_tcb_ptr;
#else
  /* The TCB of the sleeping thread. */
  rex_tcb_type*   thread_tcb_ptr;
#endif
  /* The thread's msleep timeout timer. */
  hsu_timer_handle_type
    thread_sleep_timer;

  /* Flag to define the msleep timeout timer only once */
  boolean
    is_thread_sleep_timer_defined;

  /* task msleep status */
  os_msleep_status_enum status; 

} os_sleeping_thread_info;


/* msleep handle pool entry. */
typedef struct 
{
  /* Marks whether the pool entry is available or 
  ** occupied.
  */
  boolean  is_available;

  /* Positive value - The number of pending wake-ups.
  ** Negative value - The number of waiting threads.
  */
  int32    count;

  /* The index of the next thread that will be waked up
  ** by an hsu_os_wakeup call. Each hsu_os_wakeup call advances this value,
  ** and thus starvation of the waiting threads is prevented.
  */
  uint32   next_waked_up;

  /* A list of threads sleeping on this handle */
  os_sleeping_thread_info  sleep_list[OS_MAX_THREADS_PER_MSLEEP_HANDLE];

} os_msleep_handle_pool_entry_type;

/* mutex pool entry. */
typedef struct 
{
  /* Marks whether the pool entry is available or 
  ** occupied.
  */
  boolean  is_available;
  boolean  is_locked;

} os_mutex_pool_entry_type;

typedef struct 
{
  dword   intlock_state;
  uint32  lock_nesting;
} os_spinlock_state_type;

typedef struct
{
  char        *locker_file;
  uint32       locker_line;
  #ifdef USB_THREADX
  qurt_thread_t locker_tcb;
  #else
  rex_tcb_type *locker_tcb;
  #endif
} giant_mutex_debug_type;

/* Files global variables. */
typedef struct
{
  boolean                    os_thread_pool_is_initialized;

  os_thread_pool_entry_type  os_thread_pool[OS_MAX_THREADS];

  os_mutex_pool_entry_type   os_mutex_pool[OS_MAX_MUTEXES];

  os_msleep_handle_pool_entry_type 
    os_msleep_handle_pool[OS_MAX_MSLEEP_HANDLES]; 

  os_spinlock_state_type  spinlock_state;


  #ifdef USB_THREADX
  qurt_mutex_t              os_giant_mutex;
  #else
  /* The 'Giant Mutex'. used by the High-Speed USB stack core's
  ** code for its synchronization.
  */
  rex_crit_sect_type os_giant_mutex;
  #endif
  giant_mutex_debug_type giant_mutex_debug_info;

  /* The Giant Mutex's reference counter
  */
  uint32 os_giant_mutex_ref_count;

} os_sync_file_static_vars_type;

/*==============================================================================
  Externalized Variables
==============================================================================*/


/*==============================================================================
  File Static Variables
==============================================================================*/

static os_sync_file_static_vars_type hsu_os_abstract_sync;

static os_sync_file_static_vars_type* const
  file_static_vars_ptr = &hsu_os_abstract_sync;

static unsigned long hsu_sync_intlock_flag;

/*==============================================================================

                    FUNCTION DEFINITIONS

==============================================================================*/

/*===========================================================================
FUNCTION HSU_OS_SAFE_ENTER_IMPL

DESCRIPTION
  Acquires the High-Speed USB stack Giant mutex.
  This function may not be invoked from an interrupt context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Prevents execution of other High-Speed USB tasks until the mutex is 
  released.

===========================================================================*/
void hsu_os_safe_enter_impl(void)
{
  HSU_ASSERT(!rex_is_in_irq_mode());

  rex_enter_crit_sect(&(file_static_vars_ptr->os_giant_mutex));

  /* Increment giant mutex ref count - at this point the mutex is held */
  file_static_vars_ptr->os_giant_mutex_ref_count++;

} /* hsu_os_safe_enter_impl */


/*===========================================================================
FUNCTION HSU_OS_SAFE_LEAVE_IMPL

DESCRIPTION
  Releases the High-Speed USB stack Giant mutex.
  This function may not be invoked from an interrupt context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Enables execution of other High-Speed USB tasks.

===========================================================================*/
void hsu_os_safe_leave_impl(void)
{
  HSU_ASSERT(!rex_is_in_irq_mode());

  /* We want to alert if the mutex isn't locked */
  HSU_ASSERT(file_static_vars_ptr->os_giant_mutex_ref_count > 0);

  /* Decrement giant mutex ref count -  at this point the mutex is held */
  file_static_vars_ptr->os_giant_mutex_ref_count--;

  rex_leave_crit_sect(&(file_static_vars_ptr->os_giant_mutex));

} /* hsu_os_safe_leave_impl */

/*===========================================================================
FUNCTION HSU_OS_SAFE_ENTER_D

DESCRIPTION
Wrapper function to acquire the High-Speed USB stack Giant mutex, and save
the locker details.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Prevents execution of other High-Speed USB tasks until the mutex is 
released.

===========================================================================*/
void hsu_os_safe_enter_d(char *file, uint32 line)
{
#ifndef SINGLE_THREADED
  hsu_os_safe_enter_impl();
  hsu_os_abstract_sync.giant_mutex_debug_info.locker_file = file;
  hsu_os_abstract_sync.giant_mutex_debug_info.locker_line = line;
  hsu_os_abstract_sync.giant_mutex_debug_info.locker_tcb = rex_self();
#else
  return;
#endif
} /* hsu_os_safe_enter_d */


/*
 * Temporary wrapper function for hsu_os_safe_enter_d. DO NOT USE.
 */
void os_safe_enter_d(char *file, uint32 line)
{
  hsu_os_safe_enter_d(file, line);
}


/*===========================================================================
FUNCTION HSU_OS_SAFE_LEAVE_D

DESCRIPTION
Wrapper function to release the High-Speed USB stack Giant mutex, and reset
locker details.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Enables execution of other High-Speed USB tasks.

===========================================================================*/
void hsu_os_safe_leave_d(void)
{
  if (hsu_os_abstract_sync.os_giant_mutex_ref_count == 1)
  {
    hsu_os_abstract_sync.giant_mutex_debug_info.locker_file = NULL;
    hsu_os_abstract_sync.giant_mutex_debug_info.locker_line = 0;
    hsu_os_abstract_sync.giant_mutex_debug_info.locker_tcb = NULL;
  }
  hsu_os_safe_leave_impl();
} /* hsu_os_safe_leave_d */


/*
 * Temporary wrapper function for hsu_os_safe_leave_d. DO NOT USE.
 */
void os_safe_leave_d(void)
{
  hsu_os_safe_leave_d();
}


/*===========================================================================
FUNCTION HSU_OS_IS_GIANT_LOCKED_BY_CURR_TASK

DESCRIPTION
Retunrs TRUE if the Giant mutex is currently locked by the running task

DEPENDENCIES
None.

RETURN VALUE
TRUE - if the Giant mutex is currently locked by the running task

SIDE EFFECTS
None.

===========================================================================*/
boolean hsu_os_is_giant_locked_by_curr_task(void)
{
   if (hsu_os_abstract_sync.giant_mutex_debug_info.locker_tcb == rex_self())
   {
      return TRUE;
   }
   return FALSE;
} /* hsu_os_is_giant_locked_by_curr_task */

/*===========================================================================
FUNCTION HSU_OS_GET_CONTEXT

DESCRIPTION
  Return a unique identifier of the current running thread (from the local OS 
  perspective).

DEPENDENCIES
  None.

RETURN VALUE
  Return a unique identifier of the current running thread (from the local OS 
  perspective).  

SIDE EFFECTS
  None.

===========================================================================*/
juint32_t hsu_os_get_context(void)
{
#ifdef FEATURE_L4
  return ((juint32_t)(rex_self()));
#else /* FEATURE_L4 */
  /* non-L4 target returns rex_curr_task in interrupt context. It caused an issue in
  * jos_sync.c  jungo drop, Change 747926. jtask_schedule missed task_signal, when
  * it is called in interrupt context and interrupted task was the same task as 
  * task_thread_arr[task->type].context. 
  */
  return (juint32_t)(rex_is_in_irq_mode() ? NULL : rex_self());
#endif /* !FEATURE_L4 */
} /* hsu_os_get_context */

os_thread_tcb_t* hsu_os_get_thread_tcb(qurt_thread_t thread_id)
{
  uint32 i;
  for (i = 0; i < OS_MAX_THREADS; i++)
  {
    if (file_static_vars_ptr->os_thread_pool[i].os_thread_id == thread_id)
    {
      return &(file_static_vars_ptr->os_thread_pool[i]);
    }
  }
  return 0;
}



/*===========================================================================
FUNCTION HSU_OS_THREAD_CREATE

DESCRIPTION
  Creates a new USB stack thread which will execute the user's function.
  This function seeks for an available entry in the OS abstraction layer 
  thread pool. When such an entry is found, the user's function address 
  and the argument are stored in this entry. Then, a new REX task is created.
  This new task gets the user's function address and from the pool entry and
  executes it.

  This function may not be invoked from an interrupt context 
  or a DSR context.

DEPENDENCIES

RETURN VALUE
  0 on success. Otherwise, returns one of error codes defined in jtypes.h.

SIDE EFFECTS
  Make a REX task runnable.

===========================================================================*/
jresult_t hsu_os_thread_create
(
  /* (IN) Thread's main function.          */
  jthread_func func,

  /* (IN) Thread's main function argument. */
  void *arg,

  /* (IN) Thread's priority */
  os_thread_priority priority,

  /* (OUT) Returned thread handle.         */
  void **handle
)
{
  uint32     i;
  jresult_t  return_value = 0;
  boolean    found_avail_pool_entry;

#ifdef USB_THREADX
  uint16 rex_priority;
  uint32 stack_size;
#else
  rex_priority_type rex_priority;
#endif

  os_thread_pool_entry_type* const 
    threadl_ptr = file_static_vars_ptr->os_thread_pool;

#ifdef FEATURE_HS_USB_POLLING
  if(hsu_polling_is_active()){return 0;}
#endif /* FEATURE_HS_USB_POLLING */  

  HSU_ASSERT(!rex_is_in_irq_mode());

  HSU_MSG_HIGH_1("hsu_os_thread_create: Creating a new HS-USB stack thread with priority %d.",
                priority);

#ifndef USB_THREADX
  switch(priority)
  {
#ifdef T_MDM9X15
  case THREAD_PRIORITY_CONTROLLER:
    /* rex_priority = TASK_PRIO(hsusb); */
    rex_priority = 151; /* Hard coded for now */
    break;

  case THREAD_PRIORITY_CORE:
    rex_priority = TASK_PRIO(hsu_core);
    break;

  case THREAD_PRIORITY_DRIVER:
    rex_priority = TASK_PRIO(hsu_driver);
    break;

  case THREAD_PRIORITY_OTHER:
    rex_priority = TASK_PRIO(hsu_other);
    break;
#else /* T_MDM9X15 */
  case THREAD_PRIORITY_CONTROLLER:
    rex_priority = TASK_PRIORITY(HSU_CONTROLLER_PRI_ORDER);
    break;

  case THREAD_PRIORITY_CORE:
    rex_priority = TASK_PRIORITY(HSU_CORE_PRI_ORDER);
    break;

  case THREAD_PRIORITY_DRIVER:
    rex_priority = TASK_PRIORITY(HSU_DRIVER_PRI_ORDER);
    break;

  case THREAD_PRIORITY_OTHER:
    rex_priority = TASK_PRIORITY(HSU_OTHER_PRI_ORDER);
    break;
#endif /* T_MDM9X15 */

  default:
    HSU_ERR_FATAL("hsu_os_thread_create: Error - Illegal thread priority value: %d",
      priority, 0, 0);
    
    return JEINVAL;
  }
#endif

  for (i = 0, found_avail_pool_entry = FALSE;
      i < OS_MAX_THREADS;
      i++)
  {
    if (threadl_ptr[i].thread_function == NULL)
    {
      threadl_ptr[i].thread_function          = func;
      threadl_ptr[i].thread_function_argument = arg;

      found_avail_pool_entry = TRUE;
      break;
    }
  }

  if (!found_avail_pool_entry)
  {
    HSU_ERR_FATAL("hsu_os_thread_create: Error - Ran out of available threads",
      0, 0, 0);

    return_value = JENOMEM;
  }
  else
  {
#ifdef USB_THREADX
    qurt_thread_attr_t attr;

    rex_priority = rcinit_lookup_prio(threadl_ptr[i].os_thread_name);
    stack_size = rcinit_lookup_stksz(threadl_ptr[i].os_thread_name);

    if (handle != NULL)
    {
      qurt_thread_attr_init(&attr);
      qurt_signal_init(&(threadl_ptr[i].os_thread_sig));
      qurt_thread_attr_set_name((qurt_thread_attr_t*)&attr, threadl_ptr[i].os_thread_name);
      qurt_thread_attr_set_priority((qurt_thread_attr_t*)&attr, rex_priority); 

      qurt_thread_attr_set_stack_size((qurt_thread_attr_t*)&attr, stack_size);//OS_THREAD_STACK_SIZE_IN_BYTES

      (jresult_t)qurt_thread_create(&(threadl_ptr[i].os_thread_id), (qurt_thread_attr_t*)&attr,
                                        hsu_os_thread_wrapper_function, (void *) i);

      return_value = 0;
      *handle = (void*)((int32)(&(threadl_ptr[i])));
    }

#else //USB_THREADX
    if (handle != NULL)
    {
      /* Double casting is done in order to eliminate 
      ** Lint message #826.
      */
      *handle = (void*)((int32)(&(threadl_ptr[i])));
    }

    /* Create a thread on demand */
    /*lint -e{662} 
    ** Suppress Lint error 662: Possible creation of out-of-bounds pointer,
    ** Since no out of bounds access is possible due to the check in line 475.
    */
    rex_def_task_ext(&(threadl_ptr[i].os_thread_tcb),
      threadl_ptr[i].os_thread_stack,
      OS_THREAD_STACK_SIZE_IN_BYTES,
      rex_priority,
      hsu_os_thread_wrapper_function,
      (dword)i,
      threadl_ptr[i].os_thread_name,
      FALSE);

    HSU_MSG_HIGH_2("hsu_os_thread_create: Created a new HS-USB thread. Pool entry %d. Priority: %d.",
                 i, priority);

    return_value = 0;
#endif //USB_THREADX
  }

  return return_value;
} /* hsu_os_thread_create */


/*===========================================================================
FUNCTION HSU_OS_MUTEX_INIT

DESCRIPTION
  Creates a new mutex object and returns a handle to this mutex.
  Seeks for an available mutex in the OS abstraction layer mutex pool. If
  such a mutex is found, it is marked in the pool as allocated, and the 
  mutex index in the pool is returned as a handle to the user.

  This function may not be invoked from an interrupt context 
  or a DSR context.

DEPENDENCIES

RETURN VALUE
  0 on success. Otherwise, returns one of the error codes 
  defined in jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_mutex_init(os_mutex_h *mtx)
{
  uint32 i;

  os_mutex_pool_entry_type* const
    os_mutex_pool_ptr = file_static_vars_ptr->os_mutex_pool;


  HSU_ASSERT(mtx != NULL);

  for (i = 0; i < OS_MAX_MUTEXES; i++)
  {
    if (os_mutex_pool_ptr[i].is_available)
    {
      os_mutex_pool_ptr[i].is_available = FALSE;
      break;
    }
  }

  HSU_ASSERT(i < OS_MAX_MUTEXES);

  if (i < OS_MAX_MUTEXES)
  {
    *mtx = (os_mutex_h)i;
    return HSU_JSUCCESS;
  }
  else
  {
    *mtx = NULL;
    return JENOMEM;
  }

} /* hsu_os_mutex_init */


/*===========================================================================
FUNCTION HSU_OS_MUTEX_UNINIT

DESCRIPTION
  Frees a mutex object.
  Marks the mutex as available in the OS abstraction layer mutex pool.
  Consequently, the mutex may be used by other threads.

  This function may not be invoked from an interrupt context 
  or a DSR context.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_mutex_uninit(os_mutex_h mtx)
{
  os_mutex_pool_entry_type* const
    os_mutex_pool_ptr = file_static_vars_ptr->os_mutex_pool;

  const uint32 mutex_index_in_pool = (uint32)mtx;


  HSU_ASSERT(mutex_index_in_pool < OS_MAX_MUTEXES);
  HSU_ASSERT(os_mutex_pool_ptr[mutex_index_in_pool].is_available == FALSE);
  HSU_ASSERT(os_mutex_pool_ptr[mutex_index_in_pool].is_locked == FALSE);

  os_mutex_pool_ptr[mutex_index_in_pool].is_available = TRUE;

} /* hsu_os_mutex_uninit */


/*===========================================================================
FUNCTION HSU_OS_MUTEX_LOCK

DESCRIPTION
  Locks a mutex object.

  If the mutex non-initialized or was un-initiaized prior to this call,
  this function generates an assertion.

  This function may not be invoked from an interrupt context 
  or a DSR context.

DEPENDENCIES
  The mutex needs to be initialized (using the hsu_os_mutex_init function)
  before calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  Other threads which try to lock the same mutex will be blocked until 
  the mutex is unlocked (using the hsu_os_mutex_unlock function).
===========================================================================*/
void hsu_os_mutex_lock(os_mutex_h mtx)
{
  os_mutex_pool_entry_type* const
    os_mutex_pool_ptr = file_static_vars_ptr->os_mutex_pool;

  const uint32 mutex_index_in_pool = (uint32)mtx;
  
#ifdef FEATURE_HS_USB_POLLING
  if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 


  HSU_ASSERT(mutex_index_in_pool < OS_MAX_MUTEXES);
  HSU_ASSERT(os_mutex_pool_ptr[mutex_index_in_pool].is_available == FALSE);

  while(os_mutex_pool_ptr[mutex_index_in_pool].is_locked)
  {
    hsu_os_delay_ms(OS_MUTEX_SLEEP_TIME_MS);
  }

  os_mutex_pool_ptr[mutex_index_in_pool].is_locked = TRUE;

}  /* hsu_os_mutex_lock */


/*===========================================================================
FUNCTION HSU_OS_MUTEX_UNLOCK

DESCRIPTION
  Unlocks a mutex object.

DEPENDENCIES
  The mutex needs to be initialized (using the hsu_os_mutex_init function)
  prior to calling this function.

  This function may not be invoked from an interrupt context 
  or a DSR context.

RETURN VALUE
  None.

SIDE EFFECTS
  If there are other threads waiting to lock the mutex, one of them
  will lock the mutex and become runnable.

  If the mutex was not initialized prior to calling this function, 
  an assertion is generated.

===========================================================================*/
void hsu_os_mutex_unlock(os_mutex_h mtx)
{
  os_mutex_pool_entry_type* const
    os_mutex_pool_ptr = file_static_vars_ptr->os_mutex_pool;

  const uint32 mutex_index_in_pool = (uint32)mtx;

#ifdef FEATURE_HS_USB_POLLING
  if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 

  HSU_ASSERT(mutex_index_in_pool < OS_MAX_MUTEXES);
  HSU_ASSERT(os_mutex_pool_ptr[mutex_index_in_pool].is_available == FALSE);

  os_mutex_pool_ptr[mutex_index_in_pool].is_locked = FALSE;

}  /* hsu_os_mutex_unlock */


/*===========================================================================
FUNCTION HSU_OS_SPINLOCK_INIT

DESCRIPTION
  Initialize a spinlock object, and returns a handle to a spinlock object.

  This function may not be invoked from an interrupt context 
  or from a DSR context.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success. Otherwise returns an error code.

SIDE EFFECTS
  None.

===========================================================================*/
jresult_t hsu_os_spinlock_init(os_spinlock_h* lock)
{
#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return 0;}
#endif /* FEATURE_HS_USB_POLLING */  

  HSU_ASSERT(lock != NULL);
  HSU_ASSERT(!rex_is_in_irq_mode());

  *lock = &(file_static_vars_ptr->spinlock_state);
  return HSU_JSUCCESS;

} /* hsu_os_spinlock_init */


/*===========================================================================
FUNCTION HSU_OS_SPINLOCK_UNINIT

DESCRIPTION
  Un-Initialize a spinlock object.

  This function may not be invoked from an interrupt context 
  or from a DSR context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hsu_os_spinlock_uninit(os_spinlock_h lock)
{
  HSU_USE_PARAM(lock);
  HSU_ASSERT(!rex_is_in_irq_mode());

} /*  hsu_os_spinlock_uninit*/


/*===========================================================================
FUNCTION HSU_OS_SPINLOCK_LOCK

DESCRIPTION
  Locks a spinlock object.

  Spinlock nesting is supported. That means that if a thread calls
  hsu_os_spinlock_lock n times consecutively, then it needs to call 
  hsu_os_spinlock_unlock n times in order to release the lock.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  While a spinlock is locked, all the interrupts in the system are blocked,
  and preemptions are disabled. Therefore, extra care should be taken
  while using this API. A spinlock object must be locked only for very 
  short periods.

  NOTE - A context switch still can occur if a Rex blocking call is called.

===========================================================================*/
void hsu_os_spinlock_lock(os_spinlock_h lock)
{
  os_spinlock_state_type* spinlock_state_ptr;

#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */  

  spinlock_state_ptr = (os_spinlock_state_type*)lock;

  HSU_ASSERT(spinlock_state_ptr == 
             &(file_static_vars_ptr->spinlock_state));

  /* If the spinlock is currently unlocked, lock it.
  ** Otherwise, only increment the nesting level.
  */
  if (spinlock_state_ptr->lock_nesting == 0)
  {
    INTLOCK_SAVE(spinlock_state_ptr->intlock_state);
  }

  spinlock_state_ptr->lock_nesting++;

} /* hsu_os_spinlock_lock */


/*===========================================================================
FUNCTION HSU_OS_SPINLOCK_UNLOCK

DESCRIPTION
  Unlock a spinlock object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the spinlock becomes unlocked (i.e., no nesting is left), interrupts and
  context switches become enabled again.

===========================================================================*/
void hsu_os_spinlock_unlock(os_spinlock_h lock)
{
  os_spinlock_state_type* spinlock_state_ptr;
  
#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 

  spinlock_state_ptr = (os_spinlock_state_type*)lock;

  HSU_ASSERT(spinlock_state_ptr ==
             &(file_static_vars_ptr->spinlock_state));

  spinlock_state_ptr->lock_nesting--;

  if (spinlock_state_ptr->lock_nesting == 0)
  {
    INTFREE_RESTORE(spinlock_state_ptr->intlock_state);
  }

} /* hsu_os_spinlock_unlock */


/*===========================================================================
FUNCTION HSU_OS_DELAY_US

DESCRIPTION
  Delays the execution of the calling thread for the specified 
  amount of microseconds.

  The function performs a busy sleep (does not yield the context).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_delay_us
(
  /* (IN) Busy-wait time in microseconds. */
  juint32_t usec
)
{
  
#ifdef FEATURE_HS_USB_POLLING
      if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 

  /*lint -e{834, 713} */
  busywait(usec);
} /* hsu_os_delay_us */


/*===========================================================================
FUNCTION HSU_OS_DELAY_MS

DESCRIPTION
  Delays the calling thread's execution for the specified 
  amount of milliseconds.

  The function performs a non-busy sleep (may lose the context).

  This function may not be invoked from an interrupt context 
  or a DSR context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_delay_ms
(
  /* (IN) Sleep time in milliseconds. */
  juint32_t msec
)
{
  int lock_count;
  int i;

#ifdef FEATURE_HS_USB_POLLING
  if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 
 
  HSU_ASSERT(!rex_is_in_irq_mode());
  
  /* Enter the critical section to access the global lock count */
  jsafe_enter(); 

  lock_count = hsu_os_abstract_sync.os_giant_mutex_ref_count;
  for(i = 0; i < lock_count; i++)
  {
    /* Release the Giant mutex. */
    jsafe_leave();
  }

  hsu_qurt_sleep_ms((unsigned long) msec);

  /* Re-acquire the Giant mutex.
     It should have been locked "lock_count" times,
     and then released once - because of the additional lock in this function
     (the one for accessing the global lock count).
     So in order to save two accesses to the Giant mutex,
     that actually cancel one another,
     we only lock it "lock_count-1" times
  */
  for(i = 0; i < lock_count-1; i++)
  {
    /* Re-aquire the Giant mutex. */
    jsafe_enter();
  }


} /* hsu_os_delay_ms */


/*===========================================================================
FUNCTION HSU_OS_LOCKED_DELAY_MS

DESCRIPTION
  Sleep for msec mili-second without releasing giant mutex. It should not be
  called in interrupt context. Note that the function halts caller 
  task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_locked_delay_ms
(
  /* (IN) Sleep time in milliseconds. */
  juint32_t msec
)
{

#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 

  hsu_qurt_sleep_ms((unsigned long)msec);


} /* hsu_os_locked_delay_ms */


/* ==========================================================================
FUNCTION HSU_OS_MSLEEP_TIMER_CB

DESCRIPTION
This callback is called when the sleep timer expires.
It signals the sleeping task (the one that called the hsu_os_msleep)

RETURN VALUE
None.

SIDE EFFECTS
Resume the waiting task
============================================================================= */
void hsu_os_msleep_timer_cb(unsigned long task_info_ptr)
{ 
  os_sleeping_thread_info* sleep_thread = 
    (os_sleeping_thread_info*)task_info_ptr; 

  /* lock this func in case the timer will expire simultaneously 
  ** with an hsu_os_wakeup call
  */
 INTLOCK_SAVE(hsu_sync_intlock_flag) 

  /* Make sure that the sleeping thread's entry wasn't 
  ** removed by an 'hsu_os_wakeup' call just now. */
  if (sleep_thread->thread_tcb_ptr != NULL)
  {
    sleep_thread->status = HSU_OS_SYNC_MLSEEP_TIMER_EXPIRED; 
    #ifndef USB_THREADX
    rex_set_sigs(sleep_thread->thread_tcb_ptr, 
                 HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL);
    #else
    qurt_signal_set(&((sleep_thread->thread_tcb_ptr)->os_thread_sig), HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL);
    #endif
  }
    
  INTFREE_RESTORE(hsu_sync_intlock_flag) 
} 


/*===========================================================================
FUNCTION HSU_OS_MSLEEP_INIT

DESCRIPTION
  Initializes a new msleep handle and returns a pointer to it.
  This handle may then be passed to other msleep/wakeup functions.

  Seeks for an available msleep handle in the OS abstraction layer 
  msleep handles pool. If such a handle is found, it is marked in the pool 
  as allocated, and the handle index in the pool is returned to the user.

  This function may not be invoked from an interrupt context 
  or a DSR context.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success. Otherwise, returns one of the error codes 
  defined in jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_msleep_init
(
  /* (OUT) The new allocated msleep handle. */
  os_msleep_h* msleep_h
)
{
  uint32     i;
  jresult_t  return_value = 0;

  boolean    found_avail_pool_entry;

  os_msleep_handle_pool_entry_type* const
    os_msleep_handle_pool_ptr = file_static_vars_ptr->os_msleep_handle_pool;
  
#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return 0;}
#endif /* FEATURE_HS_USB_POLLING */  

  HSU_ASSERT(msleep_h != NULL);

  for (i = 0, found_avail_pool_entry = FALSE;
       i < OS_MAX_MSLEEP_HANDLES;
       i++)
  {
    if (os_msleep_handle_pool_ptr[i].is_available == TRUE)
    {
      os_msleep_handle_pool_ptr[i].is_available = FALSE;
      found_avail_pool_entry                    = TRUE;

      break;
    }
  }

  if (!found_avail_pool_entry)
  {
    HSU_MSG_ERROR_1("Ran out of available msleep handles. (%d)",
                  OS_MAX_MSLEEP_HANDLES);
    return_value = JENOMEM;
  }
  else
  {
    *msleep_h    = (os_msleep_h)i;
    return_value = 0;
  }

  return return_value;

} /* hsu_os_msleep_init */


/*===========================================================================
FUNCTION HSU_OS_MSLEEP

DESCRIPTION
  Suspends the execution of the calling thread until the given 
  handle is awaken or the specified timeout period (in milliseconds)
  expires.

  The function performs a non-busy sleep.

  If the 'handle' argument does not refer to a valid msleep handle pool
  entry, an assertion is generated.

  If the handle is not initialized or was un-initiaized prior to this call,
  an assertion is generated.

  This function may not be called from an interrupt context or a DSR context.

DEPENDENCIES
  The handle must be previously initialized using hsu_os_msleep_init.

RETURN VALUE
  0 on success. Otherwise, returns one of the error codes 
  defined in jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_msleep
(
  /* (IN) The msleep handle to sleep on. */
  os_msleep_h handle,

  /* (IN) Sleep timeout in milliseconds. */
  juint32_t msec
)
{
  const uint32
    msleep_handle_pool_index = (uint32)handle;

  os_msleep_handle_pool_entry_type* const
    os_msleep_handle_pool_entry_ptr =
      &(file_static_vars_ptr->os_msleep_handle_pool[msleep_handle_pool_index]);

  os_sleeping_thread_info* const
    sleep_list_ptr = os_msleep_handle_pool_entry_ptr->sleep_list;
#ifdef USB_THREADX
  uint32  sigs = 0;
  //qurt_thread_t const thread_id = (qurt_thread_t)qurt_thread_get_id();
  os_thread_tcb_t* thread_tcb = hsu_os_get_thread_tcb((qurt_thread_t)qurt_thread_get_id()); 
#else
  rex_tcb_type* const my_tcb = rex_self();
  rex_sigs_type    sigs;
#endif
  dword            intlock_state;
  boolean          wakeups_pending;

  jresult_t        return_value           = EUNKNOWN;
  uint32           sleep_list_index       = 0;

  int lock_count;
  int i;
  boolean is_timer_set_required = FALSE;

#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return 0;}
#endif /* FEATURE_HS_USB_POLLING */  


  /* Make sure that the handle refers to a valid pool entry */
  HSU_ASSERT(msleep_handle_pool_index < OS_MAX_MSLEEP_HANDLES);

  /* Make sure that the handle is allocated. */
  HSU_ASSERT(!os_msleep_handle_pool_entry_ptr->is_available);

  HSU_ASSERT(!rex_is_in_irq_mode());

  //HSU_ULOG_1(NO_MSG, DBG_E_LOG,"tcb 0x%X", (uint32)thread_tcb);

  /* Make sure that the msleep signals do not conflict with any
  ** other signals which were set somewhere outside the HS-USB module.
  */
  #ifndef USB_THREADX
  sigs   = rex_get_sigs(my_tcb);
  #else
  if(thread_tcb != NULL)
  sigs = qurt_signal_get(&(thread_tcb->os_thread_sig));
  #endif

  HSU_ASSERT((sigs & (HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL)) == 0);

  INTLOCK_SAVE(intlock_state);

#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
  if (msleep_handle_pool_index == 0) /* only log for the first pool for count specific issue */
  {
    hsu_task_schedule_log(JTASK_SCH_HSU_OS_MSLEEP_LOG, os_msleep_handle_pool_entry_ptr->count);
  }
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
  wakeups_pending = (boolean)(os_msleep_handle_pool_entry_ptr->count > 0);

  /* If there are no pending wake-ups, add the current thread
  ** to the handle's sleeping threads queue and wait for a wakeup signal
  ** or a timeout.
  */
  if (!wakeups_pending)
  {
    /* Verify that there is an available entry in the msleep handle's
    ** waiting threads queue;
    */
    if (-os_msleep_handle_pool_entry_ptr->count == 
        OS_MAX_THREADS_PER_MSLEEP_HANDLE)
    {

      HSU_ASSERT((uint32)"msleep handle's waiting threads queue is full!." == NULL);

      INTFREE_RESTORE(intlock_state);

      return JENOMEM;
    }

    /* Find an available slot in the handle's sleep list. */
    for (sleep_list_index = 0; 
         sleep_list_index < OS_MAX_THREADS_PER_MSLEEP_HANDLE;
         sleep_list_index++)
    {
      #ifdef USB_THREADX
      if (sleep_list_ptr[sleep_list_index].thread_tcb_ptr == NULL)
      #else
      if (sleep_list_ptr[sleep_list_index].thread_tcb_ptr == NULL)
      #endif
      {
        break;
      }
    }

    /* Verify that an available sleep slot was found
    ** (should always happen).
    */
    HSU_ASSERT(sleep_list_index < OS_MAX_THREADS_PER_MSLEEP_HANDLE);

    /*lint -e{661}
    ** No out of bounds access occurs here, since it is done only 
    ** if an available (in bounds) entry was found 
    ** (otherwise, an assertion occurs).
    */
    #ifdef USB_THREADX
    sleep_list_ptr[sleep_list_index].thread_tcb_ptr = thread_tcb;
    #else
    sleep_list_ptr[sleep_list_index].thread_tcb_ptr = my_tcb;
    #endif   

    /* If msec > 0, there is an msleep timeout period, and therefore a 
    ** timeout timer is armed. Otherwise, msleep blocks unlimitedly until 
    ** a wakeup signal is received.
    */
    if (msec > 0)
    { 
      is_timer_set_required = TRUE;
    }

  }  /* if !(wakeups_pending) */

#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
  if (msleep_handle_pool_index == 0) /* only log for the first pool for count specific issue */
  {
    hsu_task_schedule_log(JTASK_SCH_COUNT_DEC_LOG, os_msleep_handle_pool_entry_ptr->count);
  }
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
  os_msleep_handle_pool_entry_ptr->count--;

  INTFREE_RESTORE(intlock_state);

  if (is_timer_set_required)
  {
    if (!sleep_list_ptr[sleep_list_index].is_thread_sleep_timer_defined)
    {
      sleep_list_ptr[sleep_list_index].thread_sleep_timer = hsu_timer_init(
        HSU_TIMER_CID__THREAD_SLEEP,
        HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
        hsu_os_msleep_timer_cb,
        (timer_cb_data_type)&(sleep_list_ptr[sleep_list_index]));

      sleep_list_ptr[sleep_list_index].is_thread_sleep_timer_defined = TRUE;
    }

    hsu_timer_set(&sleep_list_ptr[sleep_list_index].thread_sleep_timer,
      msec,
      T_MSEC);
  }

  if (wakeups_pending)
  {
    return_value = HSU_JSUCCESS;
  }
  else
  {
    /* Enter the critical section to access the global lock count */
    jsafe_enter(); 

    lock_count = hsu_os_abstract_sync.os_giant_mutex_ref_count;

    for(i = 0; i < lock_count; i++)
    {
      /* Release the Giant mutex. */
      jsafe_leave();
    }
#ifndef USB_THREADX
    sigs = rex_wait(HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL);
#else
    if((sleep_list_ptr[sleep_list_index].thread_tcb_ptr) != NULL)
    sigs = qurt_signal_wait(&((sleep_list_ptr[sleep_list_index].thread_tcb_ptr)->os_thread_sig),
                            HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL,
                            QURT_SIGNAL_ATTR_WAIT_ANY|QURT_SIGNAL_ATTR_CLEAR_MASK);
#endif

    /* Re-acquire the Giant mutex.
    It should have been locked "lock_count" times,
    and then released once - because of the additional lock in this function
    (the one for accessing the global lock count).
    So in order to save two accesses to the Giant mutex,
    that actually cancel one another,
    we only lock it "lock_count-1" times
    */
    for(i = 0; i < lock_count - 1; i++)
    {
      jsafe_enter();
    }

   INTLOCK_SAVE(hsu_sync_intlock_flag)
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
  if (msleep_handle_pool_index == 0) /* only log for the first pool for count specific issue */
  {
    hsu_task_schedule_log(JTASK_SCH_HSU_OS_MSLEEP_2_LOG, os_msleep_handle_pool_entry_ptr->count);
  }
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */

    if (sleep_list_ptr[sleep_list_index].status == HSU_OS_SYNC_MSLEEP_WAKEN_UP) 
    { 
      return_value = HSU_JSUCCESS; 
    }
    else if (sleep_list_ptr[sleep_list_index].status == 
             HSU_OS_SYNC_MLSEEP_TIMER_EXPIRED)
    { 
      /* If msec == 0, no timeout period exists, and thus
      ** receiving a timeout signal is an error.
      */
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
      hsu_task_schedule_log(HSU_OS_SYNC_MLSEEP_TIMER_EXPIRED_LOG, msec);
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */

      if (msec == 0)
      {
        static uint32 hsu_os_msleep_msec_0_cnt;

        hsu_os_msleep_msec_0_cnt++;
        HSU_ULOG(NO_MSG, DATA_LOG, "hsu_os_sync_mlseed_timer_expired, msec 0");
      }

      return_value = JEWOULDBLOCK; 
    }
    else if(thread_tcb != NULL)
    { 
      HSU_ERR_FATAL("hsu_os_msleep: A wakeup or timeout is expected", 0, 0, 0);
    } 

    /* Make sure that the sleeping thread's entry wasn't 
    ** removed by an 'hsu_os_wakeup' call. 
    */ 
    if (sleep_list_ptr[sleep_list_index].thread_tcb_ptr != NULL) 
    { 
      /* Remove the thread from the sleep list. */ 
      sleep_list_ptr[sleep_list_index].thread_tcb_ptr = NULL; 

#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
    if (msleep_handle_pool_index == 0) /* only log for the first pool for count specific issue */
    {
      hsu_task_schedule_log(JTASK_SCH_COUNT_INC_LOG, os_msleep_handle_pool_entry_ptr->count);
    }
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
      os_msleep_handle_pool_entry_ptr->count++; 
    } 

    sleep_list_ptr[sleep_list_index].status = HSU_OS_SYNC_MLSEEP_INIT_VAL;

    INTFREE_RESTORE(hsu_sync_intlock_flag) 

    /* Timer might have been set and should be cleared  as wake up isn't doing it*/
    if (sleep_list_ptr[sleep_list_index].is_thread_sleep_timer_defined == TRUE)
    {
       hsu_timer_cancel(&sleep_list_ptr[sleep_list_index].thread_sleep_timer);  
    }
    
    qurt_signal_clear(&(thread_tcb->os_thread_sig), HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL);
  }  /* if (wakeups_pending) */
  return return_value;

} /* hsu_os_msleep */


/*===========================================================================
FUNCTION HSU_OS_MSLEEP_UNINIT

DESCRIPTION
  Frees a OS abstraction layer msleep handle that was previously initialized
  using the hsu_os_msleep_init function.

  Marks the handle as available in the OS abstraction layer msleep
  handles pool. The msleep handle may then be used by other threads.

  If the handle argument does not refer to a valid msleep handle pool
  entry, an assertion is generated.

  If the handle is not initialized or was un-initiaized prior to this call,
  an assertion is generated.

DEPENDENCIES
  The handle must be previously initialized using hsu_os_msleep_init.

  All the threads sleeping on this handle must be waked-up before 
  calling this function. Otherwise, an assertion occurs.

RETURN VALUE
  None.

SIDE EFFECTS
  Stops all the timers associated with this handle and removes them 
  from the Rex active timers list .
===========================================================================*/
void hsu_os_msleep_uninit
(
  /* (IN) The msleep handle to uninitialize. */
  os_msleep_h msleep_h
)
{
  const uint32
    msleep_handle_pool_index = (uint32)msleep_h;

  os_msleep_handle_pool_entry_type* const
    os_msleep_handle_pool_entry_ptr =
      &(file_static_vars_ptr->os_msleep_handle_pool[msleep_handle_pool_index]);

  os_sleeping_thread_info* const
    sleep_list_ptr = os_msleep_handle_pool_entry_ptr->sleep_list;

  uint32 sleep_list_index;

  
#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */  

  /* Make sure that the handle refers to a valid pool entry */
  HSU_ASSERT(msleep_handle_pool_index < OS_MAX_MSLEEP_HANDLES);

  /* Make sure that the handle is allocated. */
  HSU_ASSERT(!os_msleep_handle_pool_entry_ptr->is_available);

  /* There should be no threads sleeping on this handle. */
  HSU_ASSERT(os_msleep_handle_pool_entry_ptr->count >= 0);

#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
  if (msleep_handle_pool_index == 0) /* only log for the first pool for count specific issue */
  {
    hsu_task_schedule_log(JTASK_SCH_HSU_OS_MSLEEP_UNINIT_LOG, os_msleep_handle_pool_entry_ptr->count);
  }
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */

  /* Go over the handle's sleep list and clear it. */
  for (sleep_list_index = 0;
       sleep_list_index < OS_MAX_THREADS_PER_MSLEEP_HANDLE;
       sleep_list_index++)
  {
    if (sleep_list_ptr[sleep_list_index].thread_tcb_ptr != NULL)
    {
      hsu_timer_cancel(&(sleep_list_ptr[sleep_list_index].thread_sleep_timer));
      
      sleep_list_ptr[sleep_list_index].thread_tcb_ptr = NULL;
    }
  }

  os_msleep_handle_pool_entry_ptr->next_waked_up = 0;
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
  if (msleep_handle_pool_index == 0) /* only log for the first pool for count specific issue */
  {
    hsu_task_schedule_log(JTASK_SCH_COUNT_ZERO_LOG, os_msleep_handle_pool_entry_ptr->count);
  }
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
  os_msleep_handle_pool_entry_ptr->count         = 0;
  os_msleep_handle_pool_entry_ptr->is_available  = TRUE;

} /* hsu_os_msleep_uninit */


/*===========================================================================
FUNCTION HSU_OS_WAKEUP

DESCRIPTION
  Wakes up a thread waiting on the given handle.

  If the 'handle' argument does not refer to a valid msleep handle pool
  entry, an assertion is generated.

  If the handle is not initialized or was un-initiaized prior to this call,
  an assertion is generated.

DEPENDENCIES
  The handle must be previously initialized using hsu_os_msleep_init.

RETURN VALUE
  Always returns 0.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_wakeup
(
  /* The msleep handle to wake-up. */
  os_msleep_h handle
)
{
  const uint32
    msleep_handle_pool_index = (uint32)handle;

  os_msleep_handle_pool_entry_type* const
    os_msleep_handle_pool_entry_ptr =
    &(file_static_vars_ptr->os_msleep_handle_pool[msleep_handle_pool_index]);

  os_sleeping_thread_info* const
    sleep_list_ptr = os_msleep_handle_pool_entry_ptr->sleep_list;

  int32* const 
    os_msleep_handle_count_ptr = &(os_msleep_handle_pool_entry_ptr->count);

  uint32* const
    next_waked_up_ptr = &(os_msleep_handle_pool_entry_ptr->next_waked_up);
#ifndef USB_THREADX
  rex_tcb_type* awaken_thread_tcb_ptr;
#else
  os_thread_tcb_t* awaken_thread_tcb_ptr;
  qurt_signal_t sig_ptr;
#endif

  boolean  sleeping_thread_found;
  uint32   i;

  dword    intlock_state;

#ifdef FEATURE_HS_USB_POLLING
   if(hsu_polling_is_active()){return 0;}
#endif /* FEATURE_HS_USB_POLLING */ 

  /* Make sure that the handle refers to a valid pool entry */
  HSU_ASSERT(msleep_handle_pool_index < OS_MAX_MSLEEP_HANDLES);

  /* Make sure that the handle is allocated. */
  HSU_ASSERT(!os_msleep_handle_pool_entry_ptr->is_available);

  /* INTLOCK_SAV is used here, since 'hsu_os_wakeup' may be 
  ** called from an interrupt context. Therefore, a REX critical section 
  ** can't be used.
  */
  INTLOCK_SAVE(intlock_state);
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
  if (msleep_handle_pool_index == 0) /* only log for the first pool for count specific issue */
  {
    hsu_task_schedule_log(JTASK_SCH_HSU_OS_WAKEUP_LOG, *os_msleep_handle_count_ptr);
  }
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */

  HSU_ASSERT(*os_msleep_handle_count_ptr <= 1);

  /* If no threads are currently waiting on this handle,
  ** return immediately. Otherwise, wake-up a waiting thread.
  */
  if (*os_msleep_handle_count_ptr == 1)
  {
    INTFREE_RESTORE(intlock_state);
    return HSU_JSUCCESS;
  }
  else if (*os_msleep_handle_count_ptr == 0)
  {
    /* Let the next 'hsu_os_msleep' call return immediately.
    */
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
    if (msleep_handle_pool_index == 0) /* only log for the first pool for count specific issue */
    {
      hsu_task_schedule_log(JTASK_SCH_COUNT_ONE_LOG, *os_msleep_handle_count_ptr);
    }
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
    (*os_msleep_handle_count_ptr) = 1;

    INTFREE_RESTORE(intlock_state);
    return HSU_JSUCCESS;
  }

  /* *os_msleep_handle_count_ptr < 0 
  ** ==> there are threads sleeping on this handle.
  */

  /* Go over the handle's sleep list from the location 
  ** of the last awaken thread entry and on, and wake-up
  ** the first sleeping thread found.
  */
  sleeping_thread_found = FALSE;

  for (i = 0;
       !sleeping_thread_found && (i < OS_MAX_THREADS_PER_MSLEEP_HANDLE);
       i++)
  {
    if (sleep_list_ptr[*next_waked_up_ptr].thread_tcb_ptr != NULL)
    {
      awaken_thread_tcb_ptr = 
        sleep_list_ptr[*next_waked_up_ptr].thread_tcb_ptr;

      sleeping_thread_found = TRUE;

      /* Make sure that sleeping thread's entry wasn't
      ** already removed due to an hsu_os_msleep timeout.
      */
      if (sleep_list_ptr[*next_waked_up_ptr].thread_tcb_ptr != NULL)
      {
        if (sleep_list_ptr[*next_waked_up_ptr].is_thread_sleep_timer_defined)
        {
         /*  clearing the timer here is running into race in timer thread as wakeup is called in interrupt context 
                 ** we cancel it in msleep after the signal is received
                 */
         //hsu_timer_cancel(&(sleep_list_ptr[*next_waked_up_ptr].thread_sleep_timer));
        }
        sleep_list_ptr[*next_waked_up_ptr].thread_tcb_ptr = NULL;

        /* Update the msleep handle's sleep count. */
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
        if (msleep_handle_pool_index == 0) /* only log for the first pool for count specific issue */
        {
          hsu_task_schedule_log(JTASK_SCH_COUNT_INC_LOG, *os_msleep_handle_count_ptr);
        }
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
        os_msleep_handle_pool_entry_ptr->count++;
      }

      sleep_list_ptr[*next_waked_up_ptr].status = HSU_OS_SYNC_MSLEEP_WAKEN_UP; 

      /* Advance the pointer to the sleeping thread, so that he would be
      ** be awaken up on the next wakeup call.
      */
      (*next_waked_up_ptr)++;

      /*  Wrap around */
      if (*next_waked_up_ptr == OS_MAX_THREADS_PER_MSLEEP_HANDLE)
      {
        *next_waked_up_ptr = 0;
      }

      //HSU_ULOG_1(NO_MSG, DBG_E_LOG,"tcb 0x%X", (uint32)awaken_thread_tcb_ptr);
      
      #ifdef USB_THREADX
      qurt_signal_set(&(awaken_thread_tcb_ptr->os_thread_sig), HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL);
      
      #else
      rex_set_sigs(awaken_thread_tcb_ptr, HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL);
      #endif
    }
    else
    {
      (*next_waked_up_ptr)++;

      /*  Wrap around */
      if (*next_waked_up_ptr == OS_MAX_THREADS_PER_MSLEEP_HANDLE)
      {
        *next_waked_up_ptr = 0;
      }
    }
  }

  /* There should have been a sleeping thread that
  ** was awaken up. 
  */
  HSU_ASSERT(sleeping_thread_found);

  INTFREE_RESTORE(intlock_state);

  return HSU_JSUCCESS;

} /* hsu_os_wakeup */

/*===========================================================================
FUNCTION HSU_OS_GET_TIME_MS

DESCRIPTION
Get the current system time without calling timetick_get_ms which has mutex locked.
Time accuracy is in milliseconds.

This function uses "timetick_get_sclk64" which returns time in 64b format
and conversts into ms by calling timetick_cvt_from_sclk64

DEPENDENCIES
None.

RETURN VALUE
timetick_type.

SIDE EFFECTS
None.
===========================================================================*/
timetick_type hsu_os_get_time_ms
(
)
{
  uint64 tc = timetick_get_sclk64();
  return (timetick_type)timetick_cvt_from_sclk64(tc, T_MSEC);

}/* hsu_os_get_time_ms */


/*===========================================================================
FUNCTION HSU_OS_GET_TIME

DESCRIPTION
Get the current system time.
Time accuracy is in milliseconds.

This function uses "timetick_get_ms" which returns time in 32b format
(granularity is  30.5us per tick).
It stores the higher part of a 64b variable in "saved_time_high".
It is updated every time the 23b value wraps-around.
(which is expected to happen every 49 days -according to the 
"timetick_get_ms" documentation).


DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_os_get_time
(
 /* (OUT) Returned current time. */
 os_time_t* time
)
{
  os_sync_msecs_time_type  time_in_msecs;
  uint32 short_time_ms;

  /* holds the higher 32b of the time (in 64b format). updated upon wrap-around*/
  static os_sync_msecs_time_type saved_time_high = 0;
  /* holds the lower 32b of last time taken. updated every call*/
  static uint32 saved_time_low = 0;

#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 

  short_time_ms = hsu_os_get_time_ms();
  if (short_time_ms < saved_time_low)
  { /* wrap-around*/
    saved_time_high += HSU_OS_ABSTRACT_SYNC_TIME_WRAPAROUND;
  }

  /* calc returned value */
  time_in_msecs = saved_time_high + short_time_ms;

  /* update stored val every time */
  saved_time_low = short_time_ms;
  
  time->tv_sec  = (jint32_t)(time_in_msecs / 1000);
  time->tv_usec = (jint32_t)((time_in_msecs % 1000) * 1000);

}/* hsu_os_get_time */


#if 0
/* This function is not used so far, so it is compiled out.
**
** inside the comment there is an apparently correct implementation, but
** it was not tested, due to its lack of usage. Whenever this API is used,
** It should be verified that this implementation is meets the requirements
** of this API.
*/
/*===========================================================================
FUNCTION HSU_OS_GET_HZ

DESCRIPTION
  Get number of clock ticks-in-second.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the number of clock ticks per second.

SIDE EFFECTS
  None.
===========================================================================*/
juint32_t hsu_os_get_hz(void)
{
  #ifndef T_REXNT
  return clk_regime_msm_get_clk_freq_khz(CLKRGM_MARM_CLK);
  #else
  /* Return a dummy value under the RexNT platform. */
  return 1;
  #endif
} /* hsu_os_get_hz */
#endif  /* 0 */


/* ==========================================================================
FUNCTION HSU_OS_THREAD_WRAPPER_FUNCTION

DESCRIPTION
   The entry function of an OS abstraction layer thread.
   This function does the following:
   
     1. Reads the address to the function to execute and its argument 
        (stored by hsu_os_thread_create in its entry in the thread pool array).

     2. Invokes the function and passes it the argument.

     3. Clears the function and argument addresses from the thread pool entry,
        making it available again.

DEPENDENCIES
   None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================= */
#ifndef USB_THREADX
static 
#endif
void hsu_os_thread_wrapper_function
(
  /* The index of this thread in the thread pool. */
  void * pool_index
)
{
  jthread_func my_function;
  void*        my_argument;

  os_thread_pool_entry_type* const
    thread_pool_entry_ptr = &(file_static_vars_ptr->os_thread_pool[(dword)pool_index]);


  HSU_ASSERT((dword)pool_index < OS_MAX_THREADS);

  /* Acquire the Giant mutex. */
    jsafe_enter();

    /* Fetch the thread execution information. */
    my_function = thread_pool_entry_ptr->thread_function;
    my_argument = thread_pool_entry_ptr->thread_function_argument;

    HSU_ASSERT(my_function != NULL);

    /* Execute the requested function. */
    my_function(my_argument);

    /* After executing the thread function, clear the thread pool entry data.
    ** This makes the OS thread pool entry available again.
    */
    thread_pool_entry_ptr->thread_function          = NULL;
    thread_pool_entry_ptr->thread_function_argument = NULL;

    /* Release the Giant mutex. */
    jsafe_leave();

}  /* hsu_os_thread_wrapper_function */


/* ==========================================================================
FUNCTION HSU_OS_SYNC_INIT

DESCRIPTION
   Initializes the REX OS abstraction layer thread synchronization module.

DEPENDENCIES
   None.

RETURN VALUE
   If the initialization is successful, returns zero. Otherwise, returns 
   a non-zero error code.

SIDE EFFECTS
   Creates new REX threads.
============================================================================= */
jresult_t hsu_os_sync_init(void)
{
  uint32    i, k;
 
  /*lint -esym(550, result)
  ** This symbol is accessed, but for some reason Lint thinks it is not.
  */
  
  jint32_t  result;
  
  os_thread_pool_entry_type* const
    os_thread_ptr = file_static_vars_ptr->os_thread_pool;

  os_msleep_handle_pool_entry_type* const
    os_msleep_handle_pool_ptr = file_static_vars_ptr->os_msleep_handle_pool;

#ifdef FEATURE_HS_USB_POLLING
   if(hsu_polling_is_active()){return 0;}
#endif /* FEATURE_HS_USB_POLLING */ 

  HSU_MSG_HIGH("hsu_os_sync_init: Initializing HS-USB OS AL synchronization module.");
  
  /*-----------------------------*/
  /* Thread pool initialization. */
  /*-----------------------------*/

  if (!file_static_vars_ptr->os_thread_pool_is_initialized)
  {
    /* Clear the thread pool data. */
    (void)memset((void*)(file_static_vars_ptr->os_thread_pool),
                 0,
                 sizeof(file_static_vars_ptr->os_thread_pool));

    for (i = 0; i < OS_MAX_THREADS; i++)
    {
      result = jos_snprintf(os_thread_ptr[i].os_thread_name,
                           sizeof(os_thread_ptr[i].os_thread_name),
                           "HS_USB_%u",
                           i);

      HSU_ASSERT(result > 0);
#ifdef TIMETEST
      if (i == 0)
      {
        os_thread_ptr[i].os_thread_tcb.leds = TIMETEST_HSU_PERIPHERAL_TASK_ID;
      }
      else
      {   
        os_thread_ptr[i].os_thread_tcb.leds = TIMETEST_HSU_HOST_TASK_ID;
      }
#endif /* TIMETEST */
    }
    file_static_vars_ptr->os_thread_pool_is_initialized = TRUE;
   
  }

  /*-----------------------------*/
  /* Initialize the Giant mutex. */
  /*-----------------------------*/
  rex_init_crit_sect(&file_static_vars_ptr->os_giant_mutex);

  /* Initialize giant mutex ref counter to 0 */
  file_static_vars_ptr->os_giant_mutex_ref_count = 0;

  /* 
   * lock the giant for the rest of the execution. It is released by the calling 
   * function at the config selector's level (outside jstart_stack()). 
   */
  jsafe_enter(); 

  /*------------------------------------*/
  /* msleep handle pool initialization. */
  /*------------------------------------*/
  (void)memset(file_static_vars_ptr->os_msleep_handle_pool,
               0,
               sizeof(file_static_vars_ptr->os_msleep_handle_pool));

  for (i = 0; i < OS_MAX_MSLEEP_HANDLES; i++)
  {
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
    if (i == 0) /* only log for the first pool for count specific issue */
    {
      hsu_task_schedule_log(JTASK_SCH_HSU_OS_SYNC_INIT_LOG, os_msleep_handle_pool_ptr[i].count);
    }
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
    os_msleep_handle_pool_ptr[i].count         = 0;
    os_msleep_handle_pool_ptr[i].next_waked_up = 0;
    os_msleep_handle_pool_ptr[i].is_available  = TRUE;
    for (k=0; k< OS_MAX_THREADS_PER_MSLEEP_HANDLE; k++)
    {
      (os_msleep_handle_pool_ptr[i].sleep_list[k]).status = HSU_OS_SYNC_MLSEEP_INIT_VAL;
      //qurt_signal_init(&(os_msleep_handle_pool_ptr[i].sleep_list[k].signal));
    }
  }

  /*------------------------------------*/
  /* Mutex pool initialization.         */
  /*------------------------------------*/
  for (i = 0; i < OS_MAX_MUTEXES; i++)
  {
    file_static_vars_ptr->os_mutex_pool[i].is_available = TRUE;
    file_static_vars_ptr->os_mutex_pool[i].is_locked    = FALSE;
  }

  /*---------------------------*/
  /* Spinlocks initialization. */
  /*---------------------------*/
  file_static_vars_ptr->spinlock_state.lock_nesting = 0;


  HSU_MSG_HIGH("hsu_os_sync_init: HS-USB OS AL synchronization module initialization is completed.");

  return HSU_JSUCCESS;

} /* hsu_os_sync_init */


/* ==========================================================================
FUNCTION HSU_OS_SYNC_UNINIT

DESCRIPTION
   Un-initializes the REX OS abstraction layer thread synchronization module.

DEPENDENCIES
   The caller must make sure that all the synchronization resources 
   (e.g., threads) were released (using the proper un-init 
   functions) prior to calling this function.

RETURN VALUE
   None.

SIDE EFFECTS
   Rex tasks in the OS abstraction layer 'thread pool' are not killed during
   the OS abstraction layer un-initialization, since the behavior of 
   'rex_kill_task' is unknown.
============================================================================= */
void hsu_os_sync_uninit(void)
{
  uint32 i;

  /*lint -esym(529, os_msleep_pool_ptr)
  ** This symbol is referenced by the HSU_ASSERT macro, but Lint
  ** does not catch it.
  */
  os_msleep_handle_pool_entry_type* const
    os_msleep_pool_ptr = file_static_vars_ptr->os_msleep_handle_pool;

#ifdef FEATURE_HS_USB_POLLING
   if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 

  HSU_MSG_HIGH("hsu_os_sync_uninit: Un-initializing HS-USB OS AL synchronization module.");

  for (i = 0; i < OS_MAX_MSLEEP_HANDLES; i++)
  {
    HSU_ASSERT(os_msleep_pool_ptr[i].is_available == TRUE);
  }

  HSU_MSG_HIGH("hsu_os_sync_uninit: HS-USB OS AL synch. module un-initialization is completed.");

} /* hsu_os_sync_uninit */

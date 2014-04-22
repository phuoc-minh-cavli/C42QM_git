#ifndef QURT_THREAD_H
#define QURT_THREAD_H
/**
  @file qurt_thread.h 
  @brief  Prototypes of Thread API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013-2015, 2019, 2021-2022  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/
#include <string.h>
#include "tx_api.h"
#include "qurt_memory.h"
#include "comdef.h"

/*=============================================================================
                                 CONSTANTS AND MACROS
=============================================================================*/ 
/** @addtogroup thread_macros
@{ */
/**   @xreflabel{sec:qurt_thread_cfg} */  

#define QURT_THREAD_ATTR_OBJ_SIZE_BYTES      128  
#define QURT_THREAD_ATTR_PRIORITY_MAX        0
#define QURT_THREAD_ATTR_PRIORITY_DEFAULT    128
#define QURT_THREAD_ATTR_PRIORITY_MIN        255
#define QURT_THREAD_ATTR_NAME_MAXLEN         20
#define QURT_THREAD_DEFAULT_STACK_SIZE       256   
#define QURT_THREAD_MAX_PRIORITIES           256

/** 
 Maximum no. of QURT TLSs 
*/
#define QURT_MAX_TLS              MAX_TLS
#define QURT_MAX_TLS_INDEX (((QURT_MAX_TLS - 1)/32) + 1)
#define QURT_MAX_TLS_DESTRUCTOR_ITERATIONS 1

#if OEM_POOL_SIZE>0x80000
/* Define maximum user threads in QAPI context */
#define MAX_USER_THREADS_IN_QAPI_CONTEXT        10
/* Total number of DAM threads */
#define MAX_USER_THREADS                        80
#define TXM_OBJECT_SIZE                         40*1024

#elif ((OEM_POOL_SIZE<=SIZE_512K)&&(OEM_POOL_SIZE>SIZE_256K))
/* Define maximum user threads in QAPI context */
#define MAX_USER_THREADS_IN_QAPI_CONTEXT        3
/* Total number of DAM threads */
#define MAX_USER_THREADS                        8
#define TXM_OBJECT_SIZE                         20*1024

#elif ((OEM_POOL_SIZE<=SIZE_256K)&(OEM_POOL_SIZE>SIZE_128K))
/* Define maximum user threads in QAPI context */
#define MAX_USER_THREADS_IN_QAPI_CONTEXT        2
/* Total number of DAM threads */
#define MAX_USER_THREADS                        4
#define TXM_OBJECT_SIZE                         8*1024

#elif ((OEM_POOL_SIZE<=SIZE_128K)&&(OEM_POOL_SIZE>=SIZE_64K))
/* Define maximum user threads in QAPI context */
#define MAX_USER_THREADS_IN_QAPI_CONTEXT        1
/* Total number of DAM threads */
#define MAX_USER_THREADS                        2
#define TXM_OBJECT_SIZE                         4*1024

#else
/* The case when the OEM pool size is less than 64K. 64K is the least size expected when there is a DAM
   that needs to be loaded in OEM pool. Below 64K, we are not allocating any memory for the OEM pool.
   Todo: MAX_USER_THREADS_IN_QAPI_CONTEXT is kept as 1 so that we dont get any issue while creating
         the tracking structure for QAPI stack, module_kernel_stack_list. We need to featurize the 
		 module_kernel_stack_list definition and usage based on the OEM pool size.*/
#define MAX_USER_THREADS_IN_QAPI_CONTEXT        1
/* Total number of DAM threads */
#define MAX_USER_THREADS                        0
#define TXM_OBJECT_SIZE                         0
#endif

/*
This module_object_pool below is used to allocate memory block for below purposes:
1) to allocate Module control objects like thread handle, mutex/queue handles etc.

2) to allocate Kernel stack when User thread enters into Kernel mode/space.

3) to allocate QAPI stack, when user thread does a QAPI and enters kernel space.

Note: This pool is carved out from OEM pool and hence, effective usable OEM pool size = (TOTAL OEM POOL size - OBJECT pool size)
      The TXM_MODULE_OBJECT_POOL_SIZE is aligned to 4K.
*/
#ifndef TXM_MODULE_OBJECT_POOL_SIZE
#define TXM_MODULE_OBJECT_POOL_SIZE             (((TXM_OBJECT_SIZE + (MAX_USER_THREADS*TXM_MODULE_KERNEL_STACK_SIZE) + (TXM_MODULE_QAPI_KERNEL_STACK_SIZE*MAX_USER_THREADS_IN_QAPI_CONTEXT) ) + 0xFFF) & ~0xFFF) 
#endif

/** @} */ /* end_addtogroup thread_macros */
/*=============================================================================
                                    TYPEDEFS
=============================================================================*/
/** @addtogroup thread_types
@{ */
/** Thread ID type */
typedef unsigned long qurt_thread_t;
/** Thread attributes structure */
typedef struct qurt_thread_attr  /* 8 byte aligned */
{
   unsigned long long _bSpace[QURT_THREAD_ATTR_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
}qurt_thread_attr_t;

/** @} */ /* end_addtogroup thread_types */

/**
 QuRT TLS reservation 
 */
struct QURT_tls_reserve {
   unsigned int tls_bitmask[QURT_MAX_TLS_INDEX];
   void (*destructor [QURT_MAX_TLS]) (void *);
};


/*=============================================================================
                                    FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_thread_attr_init
  Initializes the structure used to set the thread attributes when a thread is created. 
  
  After an attribute structure is initialized, the individual attributes in the structure
  can be explicitly set using the thread attribute operations.
  
  @datatypes
  #qurt_thread_attr_t
  
  @param[in,out] attr Thread attribute structure.

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_thread_attr_init (qurt_thread_attr_t *attr);

/**@ingroup func_qurt_thread_attr_set_name
  Sets the thread name attribute. \n
  
  This function specifies the name to be used by a thread. Thread names are used to 
  uniquely identify a thread during debugging or profiling. 
  
  @note1hang Thread names differ from the kernel-generated thread identifier used to 
  specify threads in the API thread operations.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Thread attribute structure.
  @param[in] name Character string containing the thread name.

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_thread_attr_set_name (qurt_thread_attr_t *attr, const char *name);

/**@ingroup func_qurt_thread_attr_set_priority 
  Sets the thread priority to be assigned to a thread. Thread priorities are specified
  as numeric values in the range 0 - (QURT_THREAD_MAX_PRIORITIES -1), with 0 representing
  the highest priority.
  
  @note1hang The QuRT library can be configured at build time to have different priority ranges. 
  
  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Thread attribute structure.
  @param[in] priority Thread priority.

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_thread_attr_set_priority (qurt_thread_attr_t *attr, uint16 priority);

/**@ingroup func_qurt_thread_attr_set_stack_size
  Sets the thread stack size attribute.

  The thread stack size specifies the size of the memory area to be used for a thread's
  call stack. The QuRT library allocates the memory area when the thread is created. 

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Thread attribute structure.
  @param[in] stack_size Size (in bytes) of the thread stack.

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_thread_attr_set_stack_size (qurt_thread_attr_t *attr, uint32 stack_size);
     
/**@ingroup func_qurt_thread_get_name 
  Gets the thread name of current thread.

  Returns the thread name of the current thread. 

  Thread names are assigned to threads as thread attributes. They are used to uniquely
  identify a thread during debugging or profiling.
   
  @datatypes
  #qurt_thread_attr_t
  
  @param[out] name     Character string which specifies the address where the returned
                       thread name is stored.
  @param[in] max_len   Integer which specifies the maximum length of the
                       character string that can be returned.

  @return
  String length of the returned thread name

  @dependencies
  None.
  
*/
int qurt_thread_get_name  (char *name, uint8 max_len);

/**@ingroup func_qurt_thread_create
  Creates a new thread with the specified attributes, and makes it executable. 
 
  @datatypes
  #qurt_thread_t \n
  #qurt_thread_attr_t
  
  @param[out]  thread_id  Returns a pointer to the thread identifier if the thread was
                          successfully created. 
  @param[in]   attr       Pointer to the initialized thread attribute structure which 
                          specifies the attributes of the created thread.
  @param[in]   entrypoint C function pointer which speficies the thread's main function.
  @param[in]   arg        Pointer to a thread-specific argument structure.
  
   
  @return 
  QURT_EOK -- Thread created. \n
  QURT_EINVALID -- Invalid argument. \n
  QURT_EMEM -- Failed to allocate memory for thread stack. \n
  QURT_EFAILED � All other failures. 
  
  @dependencies
  None.
  
 */
int qurt_thread_create (qurt_thread_t *thread_id, qurt_thread_attr_t *attr, void (*entrypoint) (void *),                        void *arg);

/**@ingroup func_qurt_thread_suspend
  Suspends the execution of a specified thread.

  A thread can  use this function to suspend itself. After being suspended, the thread
  must be resumed by qurt_thread_resume() to execute again.
  
  @datatypes
  #qurt_thread_t
  
  @param[in]  thread_id Thread identifier.

  @return 
  QURT_EOK -- Thread successfully suspended. \n
  QURT_EINVALID -- Invalid thread identifier.

  @dependencies
  None.
   
 */
int qurt_thread_suspend (qurt_thread_t thread_id);

/**@ingroup func_qurt_thread_resume
  Resumes the execution of a suspended thread.

  @datatypes
  #qurt_thread_t

  @param[in]  thread_id Thread identifier.

  @return 
  QURT_EOK -- Thread successfully resumed. \n
  QURT_EINVALID -- Invalid thread identifier.

  @dependencies
  None.
   
 */
int qurt_thread_resume (qurt_thread_t thread_id);

/**@ingroup func_qurt_thread_yield
  Suspends the execution of the current thread, and schedules another ready thread(with
  the same priority) for execution. 
   
  @param[in]
  None

  @return 
  None

  @dependencies
  None.
   
 */
void qurt_thread_yield (void);

/**@ingroup func_qurt_thread_get_id
  Gets the identifier of the current thread.\n

  @datatypes
  #qurt_thread_t
  
  @param[in]
  None

  @return 
  Thread identifier

  @dependencies
  None.
   
 */
qurt_thread_t qurt_thread_get_id (void);

/**@ingroup func_qurt_thread_get_priority 
  Gets the priority of the specified thread. 

  Returns the thread priority of the specified thread.

  Thread priorities are specified as numeric values in a range as large as 0- (QURT_THREAD_MAX_PRIORITIES -1), with lower values representing higher priorities. 
  - 0 represents the highest possible thread priority. 

  @note1hang The QuRT can be configured at build time to have different priority ranges.

  @datatypes
  #qurt_thread_t

  @param[in]  thread_id   Thread identifier

  @return
  QURT_EINVALID -- Invalid thread identifier. \n
  0 -- ( QURT_THREAD_MAX_PRIORITIES -1) -- Thread priority value.

  @dependencies
  None.
   
 */
int qurt_thread_get_priority(qurt_thread_t thread_id);

/**@ingroup func_qurt_thread_set_priority
  Sets the priority of the specified thread.

  Thread priorities are specified as numeric values in a range as large as 0- (QURT_THREAD_MAX_PRIORITIES -1), with lower values representing higher priorities. 
  0 represents the highest possible thread priority.

  @note1hang The QuRT can be configured at build time to have different priority ranges.

  @datatypes
  #qurt_thread_t

  @param[in] thread_id     Thread Identifier.
  @param[in] newprio       New thread priority value.

  @return
  QURT_EOK -- Thread priority set success . \n
  QURT_EINVALID -- Invalid argument. \n
  QURT_EFAILED -- Thread priority set failed. 
    
  @dependencies
  None.
   
 */
int qurt_thread_set_priority (qurt_thread_t thread_id, uint16 newprio);

/**@ingroup func_qurt_thread_stop
  Stop the current thread.
  
  Stops the execution of current thread, frees the memory used for the thread stack, frees 
  the associated kernel TCB, and schedules the next-highest ready thread for execution.
   
  @param[in]
  None

  @return
  void 

  @dependencies
  None.
   
 */
void qurt_thread_stop (void);

/**@ingroup func_qurt_thread_sleep

  Sleeps calling thread for 'duration' ticks.
  duration : ticks in 19.2 MHz clock
  i.e. 19200 ticks = 1 ms

  This function supports sleep for duration of multiples of timer interrupt 
  expiry value (10 ms).

  @param[in]    duration Sleeps current thread for 'duration' ticks

  @return
  None

  @dependencies
  None.
  
 */
void qurt_thread_sleep(qurt_time_t duration);

/**@ingroup func_qurt_thread_sleep_ext

  Deferrable version of qurt_thread_sleep
  Sleeps calling thread for 'duration' ticks.
  duration : ticks in 19.2 MHz clock
  i.e. 19200 ticks = 1 ms

  This function supports sleep for duration of multiples of timer interrupt 
  expiry value (10 ms). 

  In case system goes into low power mode, timer would be halted and will
  resume from halted time/tick when system wakes up at later point of time.

  @param[in]    duration Sleeps current thread for 'duration' ticks

  @return
  None

  @dependencies
  None.
  
 */
void qurt_thread_sleep_ext(qurt_time_t duration);

/**@ingroup func_qurt_thread_get_idletime
   
  Gets the minimum idle time duration remaining for threads created in the system.\n
 
  The remaining duration indicates (in system ticks) how much time remains before 
  any thread will be made runnable by the scheduler.
  
  @note1hang This API must be used only when the QuRT application system is running 
  in STM mode with all interrupts disabled. Otherwise, it will result in undefined 
  behaviour, and may have side effects.
  
  @note1handle Primary user of this API is sleep. 
  
  @param
  None.
  
  @return
  Number of system ticks until next thread is scheduled.
  QURT_TIME_WAIT_FOREVER � no pending timer events.
   
  @dependencies
  None.
  
 */
qurt_time_t qurt_thread_get_idletime ( void );

/*
   Gets the QuRT API version.
 
  @return
  Qurt API version.

  @dependencies
  None.
  
 */
uint32 qurt_api_version(void);


/**@ingroup func_qurt_tls_create_key
  Creates a key for accessing a thread local storage data item.\n
  The key value is used in subsequent get and set operations.

  @note1hang The destructor function performs any clean-up operations needed by a thread
             local storage item when its containing thread is deleted.

  @param[out] key         Pointer to the newly-created thread local storage key value.
  @param[in]  destructor  Pointer to the key-specific destructor function. Passing NULL 
                          specifies that no destructor function is defined for the key.

  @return	
  QURT_EOK -- Key successfully created. \n
  QURT_ETLSAVAIL -- No free TLS key available. 

  @dependencies
  None.
 */
int qurt_tls_create_key (int *key, void (*destructor)(void *));

/**@ingroup func_qurt_tls_set_specific
  Stores a data item to thread local storage along with the specified key.

  @param[in]    key  Thread local storage key value.
  @param[in]    value  Pointer to user data value to store.

  @return  
  QURT_EOK -- Data item successfully stored. \n
  QURT_EINVALID -- Invalid key. \n
  QURT_EFAILED -- Invoked from a non-thread context.
 */
int qurt_tls_set_specific (int key, const void *value);

/**@ingroup func_qurt_tls_get_specific
  Loads the data item from thread local storage. \n
  Returns the data item that is stored in thread local storage with the specified key.
  The data item is always a pointer to user data.

  @param[in]    key Thread local storage key value.

  @return
  Pointer -- Data item indexed by key in thread local storage. \n
  0 (NULL) -- Key out of range.

  @dependencies
  None.
 */
void *qurt_tls_get_specific (int key);


/**@ingroup func_qurt_tls_delete_key
  Deletes the specified key from thread local storage.

  @note1hang Explicitly deleting a key does not execute any destructor function that is
             associated with the key (Section @xref{sec:tls_create_key}).

  @param[in]   key  Thread local storage key value to delete.

  @return  
  QURT_EOK -- Key successfully deleted. \n
  QURT_ETLSENTRY -- Key already free.

  @dependencies
  None.
 */
int qurt_tls_delete_key (int key);


/**@ingroup func_qurt_thread_join
   Waits for a specified thread to finish.
   The caller thread is suspended until the specified thread exits. When this happens the
   caller thread is awakened. \n
   @note1hang If the specified thread has already exited, this function returns immediately
              with the result value QURT_ENOTHREAD. \n
   @note1cont Two threads cannot call qurt_thread_join to wait for the same thread to finish.
              If this happens QuRT generates an exception.
  
   @param[in]   tid     Thread identifier.
   @param[out]  status  Destination variable for thread exit status. Returns an application-defined 
                        value indicating the termination status of the specified thread. 
  
   @return  
   QURT_ENOTHREAD -- Thread has already exited. \n
   QURT_EOK -- Thread successfully joined with valid status value. 

   @dependencies
   None.
 */
int qurt_thread_join(qurt_thread_t tid, int *status);

/**@ingroup qurt_start_profiling_thread
  Starts profiling window for thread id passed as parameter. 
   
  @param[in] thread_id   thread_id of thread for which profiling window is started.
  
  @return
  void

  @dependencies
  None.
   
 */ 

void qurt_start_profiling_thread(qurt_thread_t);

/**@ingroup qurt_start_profiling_Idle_thread
  Starts profiling window for IDLE thread.
   
  @return
  void

  @dependencies
  None.
   
 */ 
void qurt_start_profiling_Idle_thread(void);

/**@ingroup qurt_get_profiling_time
  Get total timetick thread "tid" has run since profiling is started.
  If profiling is not started explicitly , it returns total tick tid thread has run
  since system started.
   
  @param[in] tid   thread_id of thread.
  
  @return
  total ticks the thread "tid" has run since profiling window started
    If profiling is not started explicitly , it returns total tick tid thread has run
  since system started.

  @dependencies
  None.
   
 */ 
qurt_time_t qurt_get_profiling_time(qurt_thread_t tid);


/**@ingroup qurt_get_profiling_time_Idle
  Gets total timetick IDLE thread  has run since profiling is started.
  If profiling is not started explicitly , it returns total tick IDLE thread has run
  since system started.
  
  @return
  total ticks the IDLE thread  has run since profiling window started
    If profiling is not started explicitly , it returns total tick IDLE thread has run
  since system started.

  @dependencies
  None.
   
 */ 
qurt_time_t qurt_get_profiling_time_Idle(void);


/**@ingroup qurt_profile_getTotalTime
   Gets total ticks since profiling window is started for thread "tid"   
  @param[in] tid   thread_id of thread.
  
  @return	
      total ticks since profiling window is started for thread "tid" 

  @dependencies
  None.
   
 */ 
qurt_time_t qurt_profile_getTotalTime(qurt_thread_t tid);

/**@ingroup qurt_profile_getTotalTime_IdleThread
   Gets total ticks since profiling window is started for IDLE thread   
  @param[in] tid   thread_id of thread.
  
  @return	
      total ticks since profiling window is started for IDlE thread 

  @dependencies
  None.
   
 */ 
qurt_time_t qurt_profile_getTotalTime_IdleThread(void);

/**
 * @ingroup qurt_txm_last_ran_thread_info
 *  Get thread status information for the previously ran DAM thread
 *
 * 
 * @param [in] module_instance -> pointer to the DAM module instance
 *             0(NULL)  -> Kernel
 *             Non NULL -> DAM instance
 * 
 * @param [out] last_ran_thread_info_ptr -> pointer to output struct
 *              contains info about SP, LR, PC
 *
 * @return 0 - Success
 *         -ve value - Failure / Information not available
 *
 * @note In struct TXM_MODULE_LAST_RUN_THREAD, if LR and PC are equal, then the thread
 *       is in suspended state
 **/
int qurt_txm_last_ran_thread_info(const TXM_MODULE_INSTANCE* module_instance, TXM_MODULE_LAST_RUN_THREAD *last_ran_thread_info_ptr);


#endif /* QURT_THREAD_H */


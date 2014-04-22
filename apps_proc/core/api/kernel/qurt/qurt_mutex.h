#ifndef QURT_MUTEX_H
#define QURT_MUTEX_H
/**
  @file qurt_mutex.h 
  @brief   Prototypes of mutex API.  

  The QuRT mutex service supports two type of mutex behaviour:
  - Recursive mutex 
  - Recursive mutex with priority inheritance.

  The two mutex types have different performance; in particular, mutex lock operations are significantly slower with priority inheritance mutexes. 

  note1hang Both mutex behaviors may not be supported on some target platforms.
  The mutex behavior is specified at system build time – it cannot be changed at runtime.

    
EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/
#include "qurt_types.h"

/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/

/** @addtogroup mutex_const_macros
@{ */

#define QURT_MUTEX_OBJ_SIZE_BYTES    64

/** @} */ /* end_addtogroup mutex_const_macros */

/** @addtogroup mutex_types
@{ */
/*=============================================================================
                        TYPEDEFS
=============================================================================*/
/** QuRT mutex type.                                       
  
   Both non-recursive mutex lock/unlock and recursive
   mutex lock/unlock can be applied to this type.
 */
typedef struct qurt_mutex  /* 8 byte aligned */
{
   unsigned long long _bSpace[QURT_MUTEX_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
}qurt_mutex_t;

/** @} */ /* end_addtogroup mutex_types */
/*=============================================================================
                        FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_mutex_init
  Initializes a mutex object. The mutex is initially unlocked.

  The mutex behavior (recursive, or recursive with prioirity inheritance) is specified 
  at system build time – it cannot be changed at runtime.

  Each mutex-based object has one or more kernel resources associated with it; to prevent
  resource leaks, be sure to call qurt_mutex_destroy() when this object is not used anymore.
  
  @datatypes
  #qurt_mutex_t
  
  @param[out]  lock  Pointer to the mutex object. Returns the initialized object.

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_mutex_init(qurt_mutex_t *lock);

/**@ingroup func_qurt_mutex_destroy
  Destroys the specified mutex.

  Mutexes must be destroyed when they are no longer in use. Failure to do this causes
  resource leaks in the QuRT kernel.

  Mutexes must not be destroyed while they are still in use. If this happens the behavior
  of QuRT is undefined.
  
  @datatypes
  #qurt_mutex_t

  @param[in]  lock  Pointer to the mutex object to destroy.

  @return 
  None.

  @dependencies
  None.
  
 */
void qurt_mutex_destroy(qurt_mutex_t *lock); 

/**@ingroup func_qurt_mutex_lock
  Locks the specified mutex. 

  If a thread performs a lock operation on a mutex that is not being used, the thread 
  gains access to the shared resource that is protected by the mutex, and continues executing.

  If a thread performs a lock operation on a mutex (priority inheritance or recursive 
  mutex)that is already being used by another thread, the thread is suspended. When the
  mutex available becomes again (because the other thread has unlocked it), the thread is
  awakened and given access to the shared resource.

  If a thread is suspended on a mutex that support priority inheritance mutex, and the 
  priority of the suspended thread is higher than the priority of the thread that has 
  locked the mutex, then the thread with the mutex acquires the higher priority of the
  suspended thread. The locker thread blocks until the lock is available.

  A thread is not suspended if it locks a recursive mutex that it has already locked by
  itself, however mutex does not become available to other thread(s) until the thread 
  performs a balanced number of unlocks on the mutex.

  The lock operation is significantly slower for priority inheritance mutexes than it is 
  for recursive mutexes.

  @datatypes
  #qurt_mutex_t

  @param[in]  lock  Pointer to the mutex object.

  @return 
  None.

  @dependencies
  None.  
 */
void qurt_mutex_lock(qurt_mutex_t *lock);   

/**@ingroup func_qurt_mutex_lock_timed
  Locks the specified mutex until either the mutex is unlocked or the specified timeout
  interval is exceeded. 

  If a thread performs a lock operation on a mutex that is not being used, the thread 
  gains access to the shared resource that is protected by the mutex, and continues executing.

  If a thread performs a lock operation on a mutex (priority inheritance or recursive 
  mutex) that is already being used by another thread, the thread is suspended. When the
  mutex available becomes again (because the other thread has unlocked it), the thread is
  awakened and given access to the shared resource. 

  If a mutex is not available and the time exceeds the timeout interval, the API returns
  with the return value QURT_EFAILED_TIMEOUT. 

  If a thread is suspended on a mutex that support priority inheritance mutex, and the 
  priority of the suspended thread is higher than the priority of the thread that has
  locked the mutex, then the thread with the mutex acquires the higher priority of the
  suspended thread. The locker thread blocks until the lock is available or time exceed
  the timeout interval.

  A thread is not suspended if it locks a recursive mutex that it has already locked by
  itself, however mutex does not become available to other thread(s) until the thread 
  performs a balanced number of unlocks on the mutex.

  The lock operation is significantly slower for priority inheritance mutexes than it
  is for recursive mutexes

  @datatypes
  #qurt_mutex_t

  @param[in]  lock     Pointer to the mutex object to lock.
  @param[in]  timeout  How long a thread will wait on the mutex before the wait is cancelled. 
                         - QURT_TIME_NO_WAIT - Return immediately without any waiting.
                         - QURT_TIME_WAIT_FOREVER – Equivalent to regular qurt_mutex_wait()
                         - Timeout value (in system ticks) – Return after waiting for 
                           specified time value.
                         
  @return
  QURT_EOK – Mutex locked. \n
  QURT_EINVALID - Invalid argument. \n
  QURT_EFAILED_TIMEOUT – Wait ended because timeout interval was exceeded.

  @dependencies
  None.
  
 */
int qurt_mutex_lock_timed(qurt_mutex_t *lock, qurt_time_t timeout);

/**@ingroup func_qurt_mutex_lock_timed_ext
  same as qurt_mutex_lock_timed , except that it uses deferrable timers.
  Actual timeout with deferrable timers can exceed specified value if system enters into sleep mode due to deferrable
  behaviour of the timer.

  @datatypes
  #qurt_mutex_t

  @param[in]  lock     Pointer to the mutex object to lock.
  @param[in]  timeout  How long a thread will wait on the mutex before the wait is cancelled. 
                         - QURT_TIME_NO_WAIT - Return immediately without any waiting.
                         - QURT_TIME_WAIT_FOREVER – Equivalent to regular qurt_mutex_wait()
                         - Timeout value (in system ticks) – Return after waiting for 
                           specified time value.
                         
  @return
  QURT_EOK – Mutex locked. \n
  QURT_EINVALID - Invalid argument. \n
  QURT_EFAILED_TIMEOUT – Wait ended because timeout interval was exceeded.

  @dependencies
  None.
  
 */
int qurt_mutex_lock_timed_ext(qurt_mutex_t *lock, qurt_time_t timeout);


/**@ingroup func_qurt_mutex_unlock
  Unlocks the specified mutex. 

  More than one thread can be suspended on a mutex. When the mutex is unlocked, thread
  waiting on the mutex is awakened. If the awakened thread has higher priority than the
  current thread, a context switch occurs.

  When a thread unlocks a priority inheritance mutex, its thread priority is restored to
  its original value from any higher-priority value that it acquired from another thread
  suspended on the mutex.

  When a thread unlocks a recursive mutex, the mutex is not available until the balanced
  number of locks and unlocks has been performed on the mutex.

  @datatypes
  #qurt_mutex_t

  @param[in]  lock  Pointer to the mutex object.

  @return
  None.

  @dependencies
  None.  
 */
void qurt_mutex_unlock(qurt_mutex_t *lock);	/* unlock */

/**@ingroup func_qurt_mutex_try_lock
  Attempts to lock the specified mutex.

  If a thread performs a try_lock operation on a mutex that is not being used, the thread
  gains access to the shared resource that is protected by the mutex, and continues
  executing.

  If a thread performs a try_lock operation on a mutex that it has already locked or 
  is in use by another thread, qurt_mutex_try_lock immediately returns with a nonzero
  result value. 

  For more information on this operation, see the description of qurt_mutex_lock().
  
  @datatypes
  #qurt_mutex_t

  @param[in]  lock  Pointer to the mutex object. 

  @return 
  0 -- Success. \n 
  Nonzero -- Fail.

  @dependencies
  None.
 */
int qurt_mutex_try_lock(qurt_mutex_t *lock);	

#endif /* QURT_MUTEX_H */


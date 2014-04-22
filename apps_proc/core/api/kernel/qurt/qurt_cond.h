#ifndef QURT_COND_H
#define QURT_COND_H

/*======================================================================
                        qurt_cond.h

GENERAL DESCRIPTION
  QuRT condition functions

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2016-2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/

#include "tx_api.h"
#include "qurt_mutex.h"

#define QURT_COND_OBJ_SIZE_BYTES 64

typedef struct qurt_cond {
  unsigned long long _bSpace[QURT_COND_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
} qurt_cond_t;

/*=====================================================================
 Functions
======================================================================*/

/*======================================================================*/
/**@ingroup func_qurt_cond_init
  Initializes a conditional variable object.

  @datatypes
  #qurt_cond_t

  @param[out] cond Pointer to the initialized condition variable object.

  @return
  1. QURT_EOK       - success
  2. QURT_EINVALID  - invalid parameters
  3. QURT_EFAILED   - failed to create a semaphore associated with the 
                      condition variable

  @dependencies
  None.
 */
/* ======================================================================*/
INT qurt_cond_init(qurt_cond_t *cond);

/*======================================================================*/
/**@ingroup func_qurt_cond_destroy
  Destroys the specified condition variable.

  @note1hang Conditions must be destroyed when they are no longer in use. Failure to do
             this causes resource leaks in the QuRT kernel.\n
  @note1cont Conditions must not be destroyed while they are still in use. If this happens
             the behavior of QuRT is undefined.

  @datatypes
  #qurt_cond_t

  @param[in] cond Pointer to the condition variable object to destroy.

  @return
  1. QURT_EOK       - success
  2. QURT_EINVALID  - invalid parameters
  3. QURT_EFAILED   - failed to delete the semaphore associated with the 
                      condition variable

 */
/* ======================================================================*/
INT qurt_cond_destroy(qurt_cond_t *cond);

/*======================================================================*/
/**@ingroup func_qurt_cond_signal
  Signals a waiting thread that the specified condition is true. \n

  When a thread wishes to signal that a condition is true on a shared data item, it must
  perform the following procedure: \n
     1. Lock the mutex that controls access to the data item. \n
     2. Perform the signal condition operation. \n
     3. Unlock the mutex.

  @note1hang Failure to properly lock and unlock a condition variable's mutex may cause
             the threads to never be suspended (or suspended but never awakened).
  @note1hang Condition variables can be used only with regular mutexes - attempting to use
  recursive mutexes or priority inheritance mutexes will result in undefined behavior.
  Recursive and priority inheritance mutexes cannot be used with conditional variables.

  @datatypes
  #qurt_cond_t

  @param[in] cond Pointer to the condition variable object to signal.

  @return
  1. QURT_EOK       - success
  2. QURT_EINVALID  - invalid parameters
  3. QURT_EFAILED   - failed to resume the thread suspended on the 
                      condition variable's semaphore

  @dependencies
  None.
 */
/* ======================================================================*/
INT qurt_cond_signal(qurt_cond_t *cond);

/*======================================================================*/
/**@ingroup func_qurt_cond_wait
  Suspends the current thread until the specified condition is true.
  When a thread wishes to wait for a specific condition on a shared data item, it must
  perform the following procedure: \n
  1. Lock the mutex that controls access to the data item. \n
  2. If the condition is not satisfied, perform the wait condition operation on the
  condition variable (which both suspends the thread and unlocks the mutex).

  @note1hang Failure to properly lock and unlock a condition variable's mutex may cause
             the threads to never be suspended (or suspended but never awakened).
  @note1hang Condition variables can be used only with regular mutexes - attempting to use
  recursive mutexes or priority inheritance mutexes will result in undefined behavior.
  Recursive and priority inheritance mutexes cannot be used with conditional variables.
  @note1hang qurt mutex object will be unlocked before operating on the condition variable 
  and will be locked before returning. Any failure during the operation will result in returing 
  this API without locking the mutex. So, clients should check for the return status and unlock 
  the mutex only incase of success.

  @datatypes
  #qurt_cond_t \n
  #qurt_mutex_t

  @param[in] cond     Pointer to the condition variable object to wait on.
  @param[in] mutex    Pointer to the mutex associated with condition variable to wait on.

  @return
  1. QURT_EOK       - success
  2. QURT_EINVALID  - invalid parameters
  3. QURT_EFAILED   - failed to wait/suspend the thread

  @dependencies
  None.
 */
/* ======================================================================*/
INT qurt_cond_wait(qurt_cond_t *cond, qurt_mutex_t *mutex);

/*=============================================================================
 * Function Name  : qurt_cond_wait_timedwait
 *
 * Description    : Suspends the current thread until the specified condition
 *                  is true or timeout specified.
 *
 * Parameters     : [in] cond - Pointer to the condition variable object to 
 *                              wait on.
 *                  [in] mutex- Pointer to the mutex associated with condition 
 *                              variable to wait on.
 *                  [in] timeout - Timeout value (in system ticks). Returns after 
 *                                 waiting for specified time value.
 *
 * Return         :  1. QURT_EOK       - success
 *                   2. QURT_EINVALID  - invalid parameters
 *                   3. QURT_EFAILED   - failed to wait/suspend the thread
 *                    
=============================================================================*/
INT qurt_cond_wait_timedwait(qurt_cond_t *cond, qurt_mutex_t *mutex, UINT timeout);

/*=============================================================================
 * Function Name  : qurt_cond_wait_timedwait_ext
 *
 * Description    : Deferrable version of qurt_cond_wait_timedwait
 *                  Suspends the current thread until the specified condition
 *                  is true or timeout specified.
 *                  In case system goes into low-power mode, timer would be halted  
 *                  and will resume from halted time/tick when system wakes up at 
 *                  later point of time.
 *
 * Parameters     : [in] cond - Pointer to the condition variable object to 
 *                              wait on.
 *                  [in] mutex- Pointer to the mutex associated with condition 
 *                              variable to wait on.
 *                  [in] timeout - Timeout value (in system ticks). Returns after 
 *                                 waiting for specified time value.
 *
 * Return         :  1. QURT_EOK       - success
 *                   2. QURT_EINVALID  - invalid parameters
 *                   3. QURT_EFAILED   - failed to wait/suspend the thread
 *                    
=============================================================================*/
INT qurt_cond_wait_timedwait_ext(qurt_cond_t *cond, qurt_mutex_t *mutex, UINT timeout);


/*======================================================================*/
/**@ingroup func_qurt_cond_broadcast
  Signals multiple waiting threads that the specified condition is true.\n
  When a thread wishes to broadcast that a condition is true on a shared data item, it must
  perform the following procedure: \n
     1. Lock the mutex that controls access to the data item. \n
     2. Perform the broadcast condition operation. \n
     3. Unlock the mutex.\n

  @note1hang Failure to properly lock and unlock a condition variable's mutex may cause
             the threads to never be suspended (or suspended but never awakened).
  @note1hang Condition variables can be used only with regular mutexes - attempting to use
  recursive mutexes or priority inheritance mutexes will result in undefined behavior.
  Recursive and priority inheritance mutexes cannot be used with conditional variables.

  @datatypes
  #qurt_cond_t

  @param[in] cond Pointer to the condition variable object to signal.

  @return
  1. QURT_EOK       - success
  2. QURT_EINVALID  - invalid parameters
  3. QURT_EFAILED   - failed to resume the thread(s) suspended on the 
                      condition variable's semaphore

  @dependencies
  None.
 */
/* ======================================================================*/
INT qurt_cond_broadcast(qurt_cond_t *cond);

#endif // QURT_COND_H


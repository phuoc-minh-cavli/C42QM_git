#ifndef QURT_PTHREAD_H
#define QURT_PTHREAD_H

/*==========================================================================
 * FILE:         pthread_loc.h
 *
 * SERVICES:     POSIX pthread API interface
 *
 * DESCRIPTION:  POSIX pthread API interface based upon POSIX 1003.1-2004
 *
 *               Copyright (c) 2016  by Qualcomm Technologies, Inc.  All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *==========================================================================
 *
 *                          EDIT HISTORY FOR MODULE
 *
 *  This section contains comments describing changes made to the module.
 *  Notice that changes are listed in reverse chronological order.
 *
 *
 *
 *  when       who     what, where, why
 *  --------   ---     -------------------------------------------------------
 *  02/11/16   mc      Initial version.
 *==========================================================================*/

#include <qurt.h>
#include <qurt_cond.h>

#if 0
//#define QURT_MUTEX_INIT 1
#define MUTEX_MAGIC 0xfe
#define QURTK_FUTEX_FREE_MAGIC 0xab // TBD!!!!!!!!
#define QURT_MUTEX_INIT {{MUTEX_MAGIC, 0, QURTK_FUTEX_FREE_MAGIC,0}}

typedef int qurt_anysignal_t;
typedef int qurt_thread_t;
typedef int qurt_mutex_t;
typedef int qurt_thread_attr_t;

#else
#define PTHREAD_EBUSY       16
#define PTHREAD_ETIMEDOUT   60
/*
typedef union {
    unsigned long long raw;
    struct {
        unsigned int count;
        unsigned int n_waiting;
        unsigned int queue;
        unsigned int reserved;
    }X;
} qurt_cond_t;
*/

#endif



#define PTHREAD_MUTEX_INITIALIZER QURT_MUTEX_INIT
#define PTHREAD_NAME_LEN             16

typedef qurt_thread_t pthread_t;
typedef unsigned int pthread_mutexattr_t;
typedef unsigned int pthread_condattr_t;

typedef qurt_mutex_t pthread_mutex_t;
typedef qurt_cond_t  pthread_cond_t;

typedef unsigned int cpu_set_t;

typedef struct pthread_attr_t
{
    void         *stackaddr;
    int          internal_stack; /* this flag==1 means the stack needs to be freed by posix */
    int          stacksize;
    int          priority;
    unsigned int timetest_id;
    cpu_set_t    cpumask;
    char         name[PTHREAD_NAME_LEN];
    /* This flag indicates whether pthread lib should create thread contexts for other OSALs */
    /* This is used internally by POSIX and not available for general usage */
    int          ext_context;
} pthread_attr_t;

/* Mutexes */
int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
//int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);

/* Condition variables */
int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(
        pthread_cond_t *cond, pthread_mutex_t *mutex, unsigned int timeout_seconds);

// Fetch location thread priority
uint16 pthread_get_loc_thread_prio(void);

/** Create Thread.
 * Please refer to POSIX standard for details.
 */
int pthread_create(pthread_t * tid, const pthread_attr_t * attr, void *(*start)(void *), void *arg);

int pthread_detach(pthread_t id);

void pthread_kill(pthread_t id);

/** Wait for thread termination.
 * Please refer to POSIX standard for details.
 * @param thread    [in]  the thread to be joined
 * @param value_ptr [out] the pointer of the exit status
 */
int pthread_join(pthread_t thread, void **value_ptr);

#endif /* QURT_PTHREAD_H */


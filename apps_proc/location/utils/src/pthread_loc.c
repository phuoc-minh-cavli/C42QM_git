/*==========================================================================
 * FILE:         pthread_loc.c
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

#include <pthread_loc.h>
#include <stdio.h>
#include <qurt.h>
#include <qurt_timetick.h>
#include "loc_util_log.h"
#include <rcinit.h>

#define COND_SIGNAL 0x80

int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr)
{
    qurt_mutex_init(mutex);
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    qurt_mutex_lock(mutex);
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    qurt_mutex_unlock(mutex);
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    qurt_mutex_destroy(mutex);
    return 0;
}

int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr)
{
    qurt_cond_init(cond);
//    qurt_signal_init(cond);
    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    qurt_cond_signal(cond);
//    qurt_signal_set(cond, COND_SIGNAL);
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    qurt_cond_broadcast(cond);
//    qurt_signal_set(cond, COND_SIGNAL);
    return 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    qurt_cond_wait(cond, mutex);
//    qurt_signal_wait(cond, COND_SIGNAL,
//                    QURT_SIGNAL_ATTR_WAIT_ANY | QURT_SIGNAL_ATTR_CLEAR_MASK);
    return 0;
}

int pthread_cond_timedwait(
        pthread_cond_t *cond, pthread_mutex_t *mutex, unsigned int timeout_seconds)
{
    qurt_cond_wait_timedwait(
            cond, mutex, qurt_timer_convert_time_to_ticks(timeout_seconds, QURT_TIME_SEC));
    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    qurt_cond_destroy(cond);
//    qurt_signal_destroy (cond);
    return 0;
}

// Function to fetch Location thread priority
// We try to fetch from RCINIT, if it fails use default one
// RCINIT defines LOCATION_PRI_ORDER in rcinit_task_prio.csv
// So our thread name must be location for lookup
#define LOC_THREAD_NAME      "location"
#define LOC_THREAD_DEF_PRIO  121
uint16 pthread_get_loc_thread_prio(){

    uint16 priority = (uint16)rcinit_lookup_prio(LOC_THREAD_NAME);
    if( priority > 0){
        return priority;
    }
    return LOC_THREAD_DEF_PRIO;
}

#define FAKE_POSIX_STACK_SIZE  (20*1024)

int pthread_create(pthread_t * tid, const pthread_attr_t * attr, void *(*start)(void *), void *arg)
{
    int pt;
    static qurt_thread_attr_t fake_posix_thread_attr;

    if (NULL == attr)
    {
       qurt_thread_attr_init(&fake_posix_thread_attr);
       qurt_thread_attr_set_name(&fake_posix_thread_attr, (char*)LOC_THREAD_NAME);
       qurt_thread_attr_set_priority(&fake_posix_thread_attr, pthread_get_loc_thread_prio());
       qurt_thread_attr_set_stack_size(&fake_posix_thread_attr, FAKE_POSIX_STACK_SIZE);
    }
    pt = qurt_thread_create(tid,
                          &fake_posix_thread_attr,
                          (void(*)(void*))start,
                          arg);
    return pt;
}

int pthread_detach(pthread_t id)
{
    return 0;
}

int pthread_join(pthread_t thread, void **value_ptr)
{
    qurt_thread_join(thread, NULL);
    return 0;
}

void _qurt_thread_terminate(TX_THREAD*);
void pthread_kill(pthread_t id)
{
    _qurt_thread_terminate((TX_THREAD*)id);
}

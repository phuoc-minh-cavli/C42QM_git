/*==========================================================================
 * FILE:         shutdown.c
 *
 * DESCRIPTION:  Maximize chances of a clean shutdown right before STM mode
 *
 * Copyright (c) 2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
 ===========================================================================*/

/*==========================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/perf.mpss/2.1.2.0/shutdown/src/shutdown.c#1 $

  when       who     what, where, why
  --------   ---     --------------------------------------------------------
  10/26/15   rr      Register perf_shutdown_preSTM_cb with perf error callback
  06/30/15   ao      Initial revision
  ===========================================================================*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "qurt.h"
#include "customer.h"
#include "shutdown.h"
#include "perf_err_cb.h"

#ifndef FEATURE_SAFE_SHUTDOWN
#include "shutdown.h"

void perf_shutdown_init(void* pArgs)
{
    (void)pArgs;
}

perf_shutdown_safe_t perf_shudown_is_safe(void)
{
    return PERF_SHUTDOWN_UNKNOWN;
}

#else

extern qurt_thread_t dlpager_tid;
extern int QURT_MAX_HTHREADS;

//#define DEBUG

#ifdef DEBUG
#define PS_DEBUG printf
#else
#define PS_DEBUG(...)
#endif

#define PERF_N_BUSY_WAIT_THREADS (QURT_MAX_HTHREADS - 1)

perf_shutdown_safe_t ps_is_safe = PERF_SHUTDOWN_UNKNOWN;

static qurt_anysignal_t* perf_wait_signal;
static int*              perf_wait_done;

static void perf_shutdown_preSTM_cb(uint32_t);
static void perf_shutdown_cb(void);


void q6zip_sw_worker_set_prio(unsigned int prio);
extern void qurt_disable_interrupts_all(void) __attribute__((weak));

void qurt_disable_interrupts_all(void)
{
    static int perf_local_disable_interrupts = 0;
    perf_local_disable_interrupts++;
    PS_DEBUG("Local qurt_disable_interrupts\n");
}

perf_shutdown_safe_t perf_shudown_is_safe(void)
{
    return ps_is_safe;
}

static void perf_shutdown_preSTM_cb(uint32_t tid)
{
    if (tid == dlpager_tid)
    {
        ps_is_safe = PERF_SHUTDOWN_NO;
    }
    else
    {
        perf_shutdown_cb();
    }
}

static void perf_shutdown_cb(void)
{
    int i, count;
    unsigned long long start_ticks;

    typedef void (*func_no_args_no_ret_t)(void);
    volatile func_no_args_no_ret_t forced_full_addr_range_call =
        &qurt_disable_interrupts_all;
    forced_full_addr_range_call();

    qurt_thread_set_priority(qurt_thread_get_id(), 0);
    qurt_thread_set_priority(dlpager_tid, 0);
    q6zip_sw_worker_set_prio(0);//set the q6zip worker threads to low priority as well

    start_ticks = qurt_sysclock_get_hw_ticks();

    for (i = 0; i < PERF_N_BUSY_WAIT_THREADS; ++i)
    {
        PS_DEBUG("signal[%d]\n", i);
        qurt_anysignal_set(&perf_wait_signal[i], 1);
    }

    /* while not reaching timeout of 15 usecs in ticks */
    while (qurt_sysclock_get_hw_ticks() - start_ticks < 15 * 19200)
    {
        /* if all busy wait threads are done return */
        for (i = 0; i < PERF_N_BUSY_WAIT_THREADS; ++i)
            if (!perf_wait_done[i]) break;
        if (i == PERF_N_BUSY_WAIT_THREADS)
        {
            PS_DEBUG("Done after all busy wait threads are done\n");
            ps_is_safe = PERF_SHUTDOWN_YES;
            return;
        }
    }
    /* returns after timeout expires */
    PS_DEBUG("Done after timeout waiting for busy wait threads\n");
    ps_is_safe = PERF_SHUTDOWN_LIKELY;
}

void perf_shutdown_wait(void* param)
{
    int id = (int)param;
    volatile int dummy = 1;
    while (1)
    {
        qurt_anysignal_wait(&perf_wait_signal[id], -1);
        perf_wait_done[id] = 1;
        while(1) { dummy++; }
    }
}

void perf_shutdown_init(void* pArgs)
{
#define WAIT_STACK_SIZE 512
#define WAIT_PRIO 2
#define WAIT_TASK_NAME_BASE "shutdown_busy_"

    int i, status;
    void* pStack;
    qurt_thread_t tid;
    qurt_thread_attr_t thread_attr;
    char task_name[strlen(WAIT_TASK_NAME_BASE) + 1 + 1]; /* name_base + n + \0 */

    perf_wait_signal = malloc(PERF_N_BUSY_WAIT_THREADS * sizeof(qurt_anysignal_t));
    perf_wait_done   = malloc(PERF_N_BUSY_WAIT_THREADS * sizeof(int));

    for (i = 0; i < PERF_N_BUSY_WAIT_THREADS; ++i)
    {
        qurt_anysignal_init(&perf_wait_signal[i]);

        snprintf(task_name, strlen(WAIT_TASK_NAME_BASE) + 1 + 1,
                 WAIT_TASK_NAME_BASE "%d", i);

        qurt_thread_attr_init (&thread_attr);
        pStack = malloc (WAIT_STACK_SIZE);
        ASSERT (pStack != NULL);
        qurt_thread_attr_set_name (&thread_attr, task_name);
        qurt_thread_attr_set_stack_addr (&thread_attr, pStack);
        qurt_thread_attr_set_stack_size (&thread_attr, WAIT_STACK_SIZE);
        qurt_thread_attr_set_priority (&thread_attr, WAIT_PRIO);
        status = qurt_thread_create (&tid, &thread_attr, perf_shutdown_wait, (void *)i);
        ASSERT(status == 0);
        PS_DEBUG("Starting shutdown busy wait thread %d tid=%d \n", i, tid);
    }

    status = perf_err_crash_cb_reg_pre_STM(perf_shutdown_preSTM_cb);
}

#endif

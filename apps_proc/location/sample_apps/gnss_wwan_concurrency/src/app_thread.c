/***************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
****************************************************************************/
#include <stdio.h>
#include <qapi_timer.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "qflog_utils.h"
#include "app_msg_q.h"
#include "app_thread.h"
#include "app_utils_misc.h"

#define LOCATION_STACK_SIZE (16384)

static AppThreadContext threadPool[APP_THREAD_ID_MAX];
static boolean isModuleInit = 0;

static void app_thread_msg_dealloc(void *msg)
{
    AppMsg *payload = (AppMsg *)msg;

    if(NULL == payload)
    {
        return;
    }

    if (payload->msg)
    {
        app_utils_byte_release(payload->msg);
    }

    app_utils_byte_release(payload);
}

static int app_thread_context_reset(app_thread_id threadId)
{
    int ret = -1;

    if(threadId >= APP_THREAD_ID_MAX)
    {
        APP_LOGE("Invalid thread id %d", threadId);
        return -1;
    }

    threadPool[threadId].handle = NULL;
    threadPool[threadId].msgQ   = NULL;
    threadPool[threadId].bValid = 0;

    return 0;
}

static int app_thread_context_deinit(app_thread_id threadId)
{
    int ret = -1;
    AppThreadContext *context = NULL;

    if(threadId >= APP_THREAD_ID_MAX)
    {
        APP_LOGE("Invalid thread id %d", threadId);
        return -1;
    }

    context = &threadPool[threadId];

    if (context->handle)
    {
        txm_module_object_deallocate(context->handle);
    }

    if(context->msgQ)
    {
        app_msg_q_flush((void *)context->msgQ);
        app_msg_q_destroy((void **)&(context->msgQ));
    }

    context->bValid = 0;
    app_thread_context_reset(threadId);
    return 0;
}

static int app_thread_context_init(app_thread_id threadId)
{

    int ret = -1;
    AppThreadContext *context = NULL;

    if(threadId >= APP_THREAD_ID_MAX)
    {
        APP_LOGE("Invalid thread id %d", threadId);
        return -1;
    }

    context = &threadPool[threadId];

    do
    {
        if (TX_SUCCESS != (ret = txm_module_object_allocate(
                (void**)&(context->handle), sizeof(TX_THREAD))))
        {
            APP_LOGE("Handle allocation failed err %d",ret);
            break;
        }

        if(NULL == (context->msgQ = app_msg_q_init2()))
        {
            APP_LOGE("msgQ allocation failed");
            ret = -1;
            break;
        }

        context->bValid = 1;
    } while(0);

    if (ret != 0)
    {
        app_thread_context_deinit(threadId);
    }

    return ret;
}

/**************************************************************************
 * PUBLIC FUNCTIONS
 **************************************************************************/
void app_thread_module_init()
{
    int i = 0;

    if(isModuleInit)
    {
        APP_LOGE("Module already initialized");
        return;
    }

    for(i = 0; i<APP_THREAD_ID_MAX; i++)
    {
        app_thread_context_reset(i);
    }

    isModuleInit = 1;
}

int app_thread_create(
        const char *threadName, threadEntry entryFunction,
        const app_thread_id threadId)
{
    int ret = -1;
    char *stackPtr = NULL;
    AppThreadContext *context = NULL;

    /*Param Check*/
    if((threadId >= APP_THREAD_ID_MAX) ||
            (NULL == entryFunction) || (NULL == threadName))
    {
        APP_LOGE("Invalid param");
        return -1;
    }

    context = &threadPool[threadId];

    if(0 != context->bValid)
    {
        APP_LOGE("Thread id allready in use");
        return -1;
    }

    do
    {
        /* Initialise the context for the thread */
        if (0 != (ret = app_thread_context_init(threadId)))
        {
            APP_LOGE("context init failed err %d",ret);
            break;
        }

        /* Allocate the stack*/
        if (TX_SUCCESS != (ret =
                app_utils_byte_allocate((VOID **)&stackPtr, LOCATION_STACK_SIZE)))
        {
            APP_LOGE("Stack allocation failed err %d",ret);
            break;
        }

        /* Create the application thread */
        if (TX_SUCCESS != (ret = tx_thread_create(
                                       (context->handle),
                                       (char*)threadName,
                                       entryFunction,
                                       (ULONG)threadId,
                                       stackPtr,
                                       LOCATION_STACK_SIZE,
                                       150,
                                       150,
                                       TX_NO_TIME_SLICE,
                                       TX_AUTO_START)))
        {
            APP_LOGE("Thread creation failed err %d",ret);
            break;
        }
    } while(0);

    if (ret != TX_SUCCESS)
    {
        if(stackPtr)
        {
            app_utils_byte_release(stackPtr);
        }
        app_thread_context_deinit(threadId);
    }
    return ret;
}

int app_thread_destroy(app_thread_id threadId)
{
    int ret = -1;
    AppThreadContext *context = NULL;

    if(threadId >= APP_THREAD_ID_MAX)
    {
        APP_LOGE("Invalid thread id %d", threadId);
        return -1;
    }

    context = &threadPool[threadId];

    if(0 == context->bValid)
    {
        APP_LOGE("Thread already destroyed %d", (context->bValid));
        return -1;
    }

    if(TX_SUCCESS != (ret = tx_thread_terminate(context->handle)))
    {
        APP_LOGE("Thread termination failed err %d", ret);
        return ret;
    }

    if(TX_SUCCESS != (ret = tx_thread_delete(context->handle)))
    {
        APP_LOGE("Thread delete failed err %d", ret);
        return ret;
    }

    app_thread_context_deinit(threadId);
    context->bValid = 0;

    return 0;
}

int app_thread_send_msg(app_thread_id dstThreadId, int msgId, void *msg, size_t msgSize)
{
    int ret = -1;
    AppThreadContext *context = NULL;
    AppMsg *payload = NULL;

    if(dstThreadId >= APP_THREAD_ID_MAX)
    {
        APP_LOGE("Invalid thread id %d", dstThreadId);
        return -1;
    }

    context = &threadPool[dstThreadId];

    if(0 == context->bValid)
    {
        APP_LOGE("Invalid context %d", (context->bValid));
        return -1;
    }

    do
    {
        /* Allocate payload */
        if (TX_SUCCESS != (ret =
                app_utils_byte_allocate((void **)&payload, sizeof(*payload))))
        {
            APP_LOGE("Payload allocation failed err %d",ret);
            break;
        }
        else
        {
            payload->msg = NULL;
            payload->msgSize = 0;
            payload->msgId= 0;
        }

        /* Allocate msg */
        if (msg != NULL && msgSize > 0)
        {
            if (TX_SUCCESS != (ret = app_utils_byte_allocate((VOID **)&(payload->msg), msgSize))) {
                APP_LOGE("Msg allocation failed err %d",ret);
                break;
            }
        }
    } while(0);

    if(0 != ret)
    {
        app_thread_msg_dealloc(payload);
        return ret;
    }

    payload->msgId = msgId;

    if (msg != NULL && msgSize > 0)
    {
        memcpy(payload->msg, msg, msgSize);
        payload->msgSize = msgSize;
    }

    APP_LOGM("Sending message to TID %d", dstThreadId);

    if(0 != (ret = app_msg_q_snd(context->msgQ, payload, app_thread_msg_dealloc)))
    {
        APP_LOGE("Couldnot send the message %d",ret);
        app_thread_msg_dealloc(payload);
        return -1;
    }

    return ret;
}

int app_thread_rcv_msg(app_thread_id threadId, void **payload)
{
    int ret = -1;
    AppThreadContext *context = NULL;

    if(threadId >= APP_THREAD_ID_MAX)
    {
        APP_LOGE("Invalid thread id %d", threadId);
        return -1;
    }

    context = &threadPool[threadId];

    if(0 == context->bValid)
    {
        APP_LOGE("Invalid context %d", (context->bValid));
        return -1;
    }

    if(0 != (ret = app_msg_q_rcv((void *)context->msgQ, payload)))
    {
        APP_LOGE("Could not receive the message %d",ret);
        return -1;
    }

    APP_LOGM("Message received for TID%d", threadId);
    return ret;
}

AppThreadContext app_utils_get_context(app_thread_id threadId)
{
    return threadPool[threadId];
}

void app_utils_sleep_ms(uint32 millis)
{
    // Not possible to sleep for less than 15 millis
    if (millis < 15)
    {
        millis = 15;
    }
    qapi_Timer_Sleep(millis, QAPI_TIMER_UNIT_MSEC, 0);
}

void app_utils_sleep_ms_with_log(uint32 millis, const char* msg)
{
    // Not possible to sleep for less than 10 millis
    if (millis < 15)
    {
        millis = 15;
    }

    uint32 seconds = millis / 1000;
    uint32 milliseconds = millis % 1000;
    for (uint32 i = 0; i < seconds; i++)
    {
        qapi_Timer_Sleep(1000, QAPI_TIMER_UNIT_MSEC, 0);
        APP_LOGSH("[%s] Slept for %d/%d seconds..", msg, i+1, seconds);
    }
    if (milliseconds > 15)
    {
        qapi_Timer_Sleep(milliseconds, QAPI_TIMER_UNIT_MSEC, 0);
    }
}

/**************************************************************************
    Signal Utilities
***************************************************************************/
int app_utils_init_signal(TX_EVENT_FLAGS_GROUP** signal, const char* signalName)
{
    txm_module_object_allocate((void**)signal, sizeof(TX_EVENT_FLAGS_GROUP));
    tx_event_flags_create(*signal, (char*)signalName);
    return 0;
}

int app_utils_deinit_signal(TX_EVENT_FLAGS_GROUP* signal)
{
    tx_event_flags_delete(signal);
    txm_module_object_deallocate(signal);
    return 0;
}

uint32_t app_utils_wait_on_signal(TX_EVENT_FLAGS_GROUP* signal, TX_MUTEX* mutex)
{
    return app_utils_timedwait_on_signal(signal, mutex, TX_WAIT_FOREVER);
}

uint32_t app_utils_timedwait_on_signal(
        TX_EVENT_FLAGS_GROUP* signal, TX_MUTEX* mutex, uint32_t waitForTicks)
{
    ULONG        setSignal = 0;
    UINT         old_threshold, dummy;
    TX_THREAD*   thread;

    /* Making the operation atomic. Find the current thread and
     * Raise its preemption threshold so it does not get de-scheduled. */
    thread = tx_thread_identify();
    tx_thread_preemption_change(thread, 0, &old_threshold);

    app_utils_mutex_put(mutex);
    uint32_t ret = tx_event_flags_get(
                       signal,
                       1,
                       TX_OR_CLEAR,
                       &setSignal,
                       waitForTicks);

    /* Restore original preemption threshold and lock the Mutex*/
    tx_thread_preemption_change(thread, old_threshold, &dummy);
    app_utils_mutex_get(mutex);

    return ret;
}

void app_utils_set_signal(TX_EVENT_FLAGS_GROUP* signal, TX_MUTEX* mutex)
{
    if (mutex != NULL)
    {
        app_utils_mutex_get(mutex);
    }

    tx_event_flags_set(signal, 1, TX_OR);

    if (mutex != NULL)
    {
        app_utils_mutex_put(mutex);
    }
}

/**************************************************************************
    Mutex Utilities
***************************************************************************/
int app_utils_mutex_init(TX_MUTEX** mutex, CHAR *name_ptr)
{
    int ret = -1;
    if(NULL == mutex)
    {
        APP_LOGE("NULL mutex in Init");
        return -1;
    }

    if(0 != txm_module_object_allocate((void**)mutex, sizeof(TX_MUTEX)))
    {
        APP_LOGE("Object aloocation for mutex failed");
        return -1;
    }

    if(0 != tx_mutex_create(*mutex, name_ptr, TX_NO_INHERIT))
    {
        APP_LOGSE("mutex %s creation failed", name_ptr);
        txm_module_object_deallocate(mutex);
        return -1;
    }

    return 0;
}

int app_utils_mutex_get(TX_MUTEX* mutex)
{
    if(NULL == mutex)
    {
        APP_LOGE("NULL mutex in get");
        return -1;
    }
    return tx_mutex_get(mutex, TX_WAIT_FOREVER);
}

int app_utils_mutex_put(TX_MUTEX* mutex)
{
    if(NULL == mutex)
    {
        APP_LOGE("NULL mutex in put");
        return -1;
    }
    return tx_mutex_put(mutex);
}

int app_utils_mutex_delete(TX_MUTEX* mutex)
{
    if(mutex)
    {
        APP_LOGE("NULL mutex in delete");
        return -1;
    }
    return tx_mutex_delete(mutex);
}

/*=============================================================================
  @file sns_test_osa.c

  Several Operating System Abstractions to be used by the sns_qsocket client
  library.

  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdbool.h>
#include "qurt.h"
#include "qurt_rmutex.h"
#include "qurt_sem.h"
#include "umemheap_lite.h"
#include "uSleep_mode_trans.h"
#include "sns_test_osa.h"

/*=============================================================================
  Macros
  ===========================================================================*/

#define SNS_CLIENT_THREAD_PRIO 0x8D

#define SNS_PRINT_MESSAGE_MAX_LEN  256

#define SNS_TEST_OSA_HEAP_LEN 5120

#if defined(SSC_TARGET_HEXAGON_CORE_QDSP6_3_0)
#define USLEEP_REG(callback) uSleep_registerNotificationCallbackEx(USLEEP_ISLAND_SENSORS, 1000, 1000, callback)
#elif defined(SSC_TARGET_HEXAGON_CORE_QDSP6_2_0)
#define USLEEP_REG(callback) uSleep_registerNotificationCallback(1000, 0, callback)
#else
#define USLEEP_REG(callback) NULL
#endif

/*=============================================================================
  type definitions
  ===========================================================================*/

typedef struct sns_test_osa_thread
{
  qurt_thread_t thread;
  sns_test_osa_thread_func func;
  void *arg;
} sns_test_osa_thread;

typedef struct sns_test_osa_mutex
{
  qurt_mutex_t mutex;
} sns_test_osa_mutex;

typedef struct sns_test_osa_sem
{
  qurt_sem_t sem;
} sns_test_osa_sem;

/*=============================================================================
  Static Data
  ===========================================================================*/

static uint8_t heap_buf[SNS_TEST_OSA_HEAP_LEN];
static mem_heap_type test_heap;
static bool island_status = false;  // True: in island
static sns_test_osa_island_cb island_cb_func = NULL;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Wrapper function, as QURT's thread entry function prototype is different
 * from pthread, which this OSA is modeled after.
 */
static void
thread_func(void *arg)
{
  sns_test_osa_thread *thread = arg;
  thread->func(thread->arg);
}

/**
 * Island callback notifies entry/exit from island mode.
 *
 * @note No callback data is provided, so must use a global reference to the
 * test state.
 */
static void
island_cb(uSleep_state_notification state)
{
  island_status = (USLEEP_STATE_ENTER == state);

  if(NULL != island_cb_func)
    island_cb_func(island_status);
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

int
sns_test_osa_thread_create(sns_test_osa_thread **thread_out, char *name,
    sns_test_osa_thread_func func, void *arg, void *stack, uint32_t stack_len)
{
  int err;
  qurt_thread_attr_t attr;
  sns_test_osa_thread *thread;

  thread = sns_test_osa_malloc(sizeof(*thread));
  SNS_ASSERT(NULL != thread);
  thread->func = func;
  thread->arg = arg;

  qurt_thread_attr_init(&attr);
  qurt_thread_attr_set_name(&attr, name);
  qurt_thread_attr_set_priority(&attr, SNS_CLIENT_THREAD_PRIO);
  qurt_thread_attr_set_stack_size(&attr, stack_len);
  qurt_thread_attr_set_stack_addr(&attr, stack);

#if defined(SNS_ISLAND_INCLUDE_TEST_CLIENT)
qurt_thread_attr_set_tcb_partition(&attr, 1); // Use Island thread
#endif

  err = qurt_thread_create(&thread->thread, &attr, &thread_func, thread);
  if(QURT_EOK != err)
  {
    SNS_LOG(ERROR, "Failed to create QSOCKET_CLIENT thread");
    return -1;
  }

  *thread_out = thread;
  return 0;
}

int
sns_test_osa_mutex_create(sns_test_osa_mutex **mutex_out)
{
  sns_test_osa_mutex *mutex;

  mutex = sns_test_osa_malloc(sizeof(*mutex));
  SNS_ASSERT(NULL != mutex);

  qurt_rmutex_init(&mutex->mutex);

  *mutex_out = mutex;
  return 0;
}

int
sns_test_osa_mutex_lock(sns_test_osa_mutex *mutex)
{
  qurt_rmutex_lock(&mutex->mutex);

  return 0;
}

int
sns_test_osa_mutex_unlock(sns_test_osa_mutex *mutex)
{
  qurt_rmutex_unlock(&mutex->mutex);

  return 0;
}

int
sns_test_osa_mutex_destroy(sns_test_osa_mutex *mutex)
{
  qurt_rmutex_destroy(&mutex->mutex);
  sns_test_osa_free(mutex);

  return 0;
}

int
sns_test_osa_sem_create(struct sns_test_osa_sem **sem_out)
{
  sns_test_osa_sem *sem;

  sem = sns_test_osa_malloc(sizeof(*sem));
  SNS_ASSERT(NULL != sem);

  qurt_sem_init_val(&sem->sem, 0);

  *sem_out = sem;
  return 0;
}

int
sns_test_osa_sem_post(struct sns_test_osa_sem *sem)
{
  qurt_sem_up(&sem->sem);
  return 0;
}

int
sns_test_osa_sem_wait(struct sns_test_osa_sem *sem)
{
  qurt_sem_down(&sem->sem);
  return 0;
}

int
sns_test_osa_sem_destroy(struct sns_test_osa_sem *sem)
{
  qurt_sem_destroy(&sem->sem);
  sns_test_osa_free(sem);
  return 0;
}

void
sns_test_osa_printf(sns_test_osa_priority prio, const char *file,
    uint32_t line, const char *format, ...)
{
  uint32_t ssid = MSG_SSID_SNS;
  char buf[SNS_PRINT_MESSAGE_MAX_LEN];
  va_list args;

  if(!island_status)
  {
    va_start(args, format);
    vsnprintf(buf, SNS_PRINT_MESSAGE_MAX_LEN, format, args);
    va_end(args);
    msg_const_type msg_const = (msg_const_type) {
      { line, ssid, 1 << prio, }, "%s", file
    };
    msg_sprintf(&msg_const, buf);
  }
}

// The init process is clearly not thread safe
void*
sns_test_osa_malloc(size_t len)
{
  static bool initialized = false;

  if(!initialized)
  {
    umem_init_heap(&test_heap, heap_buf, sizeof(heap_buf));
    initialized = true;
  }

  return umem_malloc(&test_heap, len);
}

void
sns_test_osa_free(void *ptr)
{
  return umem_free(&test_heap, ptr);
}

void
sns_test_osa_island_reg(sns_test_osa_island_cb cb_func)
{
  SNS_ASSERT(NULL == island_cb_func);
  island_cb_func = cb_func;
}

void
sns_test_osa_island_unreg(sns_test_osa_island_cb cb_func)
{
  SNS_ASSERT(island_cb_func == cb_func);
  island_cb_func = NULL;
}

void
sns_test_osa_init(void)
{
  static uSleep_notification_cb_handle usleep_handle = NULL;

  if(NULL == usleep_handle)
  {
    usleep_handle = USLEEP_REG(&island_cb);
    SNS_ASSERT(usleep_handle > 0);
  }
}

bool
sns_test_osa_island_status(void)
{
  return island_status;
}

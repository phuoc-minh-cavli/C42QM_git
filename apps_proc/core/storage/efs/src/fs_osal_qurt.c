/***********************************************************************
 * fs_osal_qurt.c
 *
 * FS OS Abstraction Layer for QURT
 * Copyright (C) 2016,2019 QUALCOMM Technologies, Inc.
 *
 * FS OS Abstraction Layer for QURT
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/efs/src/fs_osal_qurt.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2019-01-12   as    Add timer support for Qurt osal wrapper.
2016-11-08   as    Initialize key for efs_errno.
2016-09-21   as    Added errno support for Qurt OS.
2016-08-08   rp    Create

===========================================================================*/

#include "fs_config_i.h"
#include "fs_osal.h"
#include "assert.h"
#include "fs_err.h"

#ifdef FEATURE_EFS_OSAL_IS_QURT

#include <string.h>
#include "malloc.h"

#ifndef FS_OSAL_QURT_MAX_DURATION
#define FS_OSAL_QURT_MAX_DURATION   (10000)
#endif


void
fs_os_init_crit_sect (fs_os_crit_sect_type *cs_arg)
{
  qurt_mutex_t *qurt_cs;

  qurt_cs = cs_arg;

  ASSERT(qurt_cs != NULL);

  qurt_mutex_init(qurt_cs);
}

void
fs_os_enter_crit_sect (fs_os_crit_sect_type *cs_arg)
{
  qurt_mutex_t *qurt_cs;

  qurt_cs = cs_arg;

  ASSERT(qurt_cs != NULL);

  qurt_mutex_lock(qurt_cs);
}

void
fs_os_leave_crit_sect (fs_os_crit_sect_type *cs_arg)
{
  qurt_mutex_t *qurt_cs;

  qurt_cs = cs_arg;

  ASSERT(qurt_cs != NULL);

  qurt_mutex_unlock(qurt_cs);

}

void
fs_os_signal_init (fs_os_signal_handle_type *sigh_arg)
{
  qurt_signal_t *sigh;

  sigh = sigh_arg;

  ASSERT(sigh != NULL);

  qurt_signal_init(sigh);
}

void
fs_os_signal_destroy (fs_os_signal_handle_type *sigh_arg)
{
  qurt_signal_t *sigh;

  sigh = sigh_arg;

  ASSERT(sigh != NULL);

  qurt_signal_destroy(sigh);
}

void
fs_os_clr_sigs (fs_os_tcb_type *tcb_arg,
                fs_os_signal_handle_type *sigh_arg, fs_os_sigs_type sigs_arg)
{
  qurt_signal_t *sigh;
  unsigned int sigs;

  (void) tcb_arg;

  sigh = sigh_arg;
  sigs = sigs_arg;

  ASSERT(sigh != NULL);
  ASSERT(sigs > 0);

  qurt_signal_clear (sigh, sigs);
}

void
fs_os_set_sigs(fs_os_tcb_type *tcb_arg, fs_os_signal_handle_type *sigh_arg,
               fs_os_sigs_type sigs_arg)
{
  qurt_signal_t *sigh;
  unsigned int sigs;

  (void) tcb_arg;

  sigh = sigh_arg;
  sigs = sigs_arg;

  ASSERT(sigh != NULL);
  ASSERT(sigs > 0);

  qurt_signal_set (sigh, sigs);
}

fs_os_sigs_type
fs_os_wait(fs_os_signal_handle_type *sigh_arg, fs_os_sigs_type sigs_arg)
{
  qurt_signal_t *sigh;
  unsigned int  sigs, ret_sigs;

  sigh = sigh_arg;
  sigs = sigs_arg;

  ASSERT(sigh != NULL);
  ASSERT(sigs > 0);

  ret_sigs = qurt_signal_wait(sigh, sigs, QURT_SIGNAL_ATTR_WAIT_ANY);
  return ret_sigs;
}

void fs_os_def_timer(fs_os_timer_type *timer_arg, fs_os_tcb_type *tcb_arg,
  fs_os_signal_handle_type *sigh_arg, fs_os_sigs_type sigs_arg)
{
  timer_error_type result;
  ASSERT(timer_arg != NULL);
  
  result = timer_def_osal(  timer_arg,
                            NULL,
                            TIMER_NATIVE_OS_SIGNAL_TYPE,
                            sigh_arg,
                            sigs_arg);
  (void) tcb_arg;
  (void) result;
}

void
fs_os_set_timer(fs_os_timer_type *timer_arg, uint32 msecs_arg)
{
  ASSERT(timer_arg != NULL);
  
  timer_set(timer_arg, (timetick_type)msecs_arg, 0, (timer_unit_type)T_MSEC);
}

void
fs_os_clr_timer(fs_os_timer_type *timer_arg)
{
  ASSERT(timer_arg != NULL);
  
  timer_clr(timer_arg, T_MSEC);
  timer_undef(timer_arg);
}

void
fs_os_timed_wait(fs_os_signal_handle_type *sigh_arg, fs_os_sigs_type sigs_arg,
    fs_os_timer_type *timer_arg, uint32 cnt_arg)
{
  fs_os_set_timer(timer_arg, cnt_arg);
  fs_os_wait(sigh_arg, sigs_arg);
}

fs_os_tcb_type* fs_os_self(void)
{
  qurt_thread_t tid;
  fs_os_tcb_type *tcb;

  tid = qurt_thread_get_id();

  tcb = (fs_os_tcb_type *)tid;
  return tcb;
}

int fs_os_autodog_enable(int dog_report_val_arg)
{
  (void) dog_report_val_arg;
  return 0;
}

int fs_os_autodog_enable_self(void)
{
  return 0;
}

int fs_os_autodog_disable(void)
{
  return 0;
}

void fs_os_set_dac(fs_os_tcb_type *tcb_arg, unsigned int dac_arg)
{
  (void) tcb_arg; (void) dac_arg;
}

void
fs_os_get_cur_task_name(char *dest_arg, unsigned int size_arg)
{
  unsigned char size;

  ASSERT(dest_arg != NULL);
  ASSERT(size_arg > 0);

  size = (unsigned char )size_arg;

  qurt_thread_get_name(dest_arg, size);
}

void fs_os_def_task_ext(fs_os_tcb_type *tcb_arg,  uint8 *stack_addr_arg,
      unsigned int stack_size_arg, unsigned int pri_arg,
      fs_os_task_func_type task_func_arg,
      fs_os_task_func_arg_type task_func_param_arg,
      char *task_name_arg, boolean suspended,
      int dog_report_val_arg)
{
  qurt_thread_attr_t qurt_attr;
  int result;

  (void) suspended; (void) dog_report_val_arg;

  ASSERT(tcb_arg != NULL);
  ASSERT(stack_addr_arg != NULL);
  ASSERT(stack_size_arg > 0);
  ASSERT(pri_arg > 0);
  ASSERT(task_func_arg != NULL);
  ASSERT(task_name_arg != NULL);

  memset(&qurt_attr, 0, sizeof(qurt_attr));
  qurt_thread_attr_init(&qurt_attr);
  qurt_thread_attr_set_name(&qurt_attr, task_name_arg);
  qurt_thread_attr_set_stack_size(&qurt_attr, stack_size_arg);
  //qurt_thread_attr_set_stack_addr(&qurt_attr, stack_addr_arg);
  qurt_thread_attr_set_priority(&qurt_attr, pri_arg);

  result = qurt_thread_create(tcb_arg, &qurt_attr, task_func_arg,
                              (void*)task_func_param_arg);
  if (result != QURT_EOK)
  {
    FS_ERR_FATAL("%d, qurt_thread_create failed", result, 0, 0);
  }
}


static int fs_os_qurt_efs_errno_key = -1;

void fs_os_efs_errno_init (void)
{
  int result;

  fs_os_qurt_efs_errno_key = -1;

  result = qurt_tls_create_key (&fs_os_qurt_efs_errno_key, NULL);
  if (result != QURT_EOK)
  {
    FS_ERR_FATAL("%d, qurt_tls_create_key failed", result, 0, 0);
  }
}

int* fs_os_efs_errno_get_address (void)
{
  int *ptr;
  int result;

  ptr = (int *)qurt_tls_get_specific (fs_os_qurt_efs_errno_key);
  if (ptr == (int *)QURT_EINVALID || ptr == (int *)QURT_EFAILED )
  {
    FS_ERR_FATAL("%d , qurt_tls_get_specific failed", ptr, 0, 0);
  }

  if (ptr == NULL)
  {
    ptr = (int *)malloc(sizeof(int));
    if (ptr == NULL)
    {
      FS_ERR_FATAL("malloc failed", 0, 0, 0);
    }

    result = qurt_tls_set_specific(fs_os_qurt_efs_errno_key, (void *)ptr);
    if (result != QURT_EOK)
    {
      free(ptr); 
      FS_ERR_FATAL("%d, qurt_tls_set_specific failed", result, 0, 0);
    }
  }
  
  return ptr;
}

#endif /* FEATURE_EFS_OSAL_IS_QURT */


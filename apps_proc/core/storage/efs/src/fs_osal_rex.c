/***********************************************************************
 * fs_osal_rex.c
 *
 * FS OS Abstraction Layer for REX
 * Copyright (C) 2016 QUALCOMM Technologies, Inc.
 *
 * FS OS Abstraction Layer for REX
 *
 ***********************************************************************/

/*===========================================================================

			EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/efs/src/fs_osal_rex.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-09-21   as    Modified errno get_address code for supporting other than Rex OS.
2016-08-08   rp    Create

===========================================================================*/

#include "fs_config_i.h"
#include "fs_osal.h"
#include "assert.h"
#include "fs_err.h"

#if defined (FEATURE_EFS_COMPILE_WITHOUT_REX)

#elif defined (FEATURE_EFS_OSAL_IS_QURT)

#else /* REX is defined, redirect all calls to the REX API */

void
fs_os_init_crit_sect (fs_os_crit_sect_type *cs_arg)
{
  rex_crit_sect_type *rex_cs;

  rex_cs = cs_arg;

  ASSERT(rex_cs != NULL);

  rex_init_crit_sect(rex_cs);
}

void
fs_os_enter_crit_sect(fs_os_crit_sect_type *cs_arg)
{
  rex_crit_sect_type *rex_cs;

  rex_cs = cs_arg;

  ASSERT(rex_cs != NULL);

  rex_enter_crit_sect(rex_cs);
}

void
fs_os_leave_crit_sect(fs_os_crit_sect_type *cs_arg)
{
  rex_crit_sect_type *rex_cs;

  rex_cs = cs_arg;

  ASSERT(rex_cs != NULL);

  rex_leave_crit_sect(rex_cs);
}

void
fs_os_signal_init (fs_os_signal_handle_type *sigh_arg)
{
  (void) sigh_arg;
}

void
fs_os_signal_destroy (fs_os_signal_handle_type *sigh_arg)
{
  (void) sigh_arg;
}

void
fs_os_clr_sigs (fs_os_tcb_type *tcb_arg,
		fs_os_signal_handle_type *sigh_arg, fs_os_sigs_type sigs_arg)
{
  rex_tcb_type *tcb;
  rex_sigs_type sigs;

  (void) sigh_arg;

  tcb = tcb_arg;
  sigs = sigs_arg;

  ASSERT(tcb != NULL);
  ASSERT(sigs > 0);

  (void) rex_clr_sigs(tcb, sigs);
}

void
fs_os_set_sigs(fs_os_tcb_type *tcb_arg, fs_os_signal_handle_type *sigh_arg,
	       fs_os_sigs_type sigs_arg)
{
  rex_tcb_type *tcb;
  rex_sigs_type sigs;

  (void) sigh_arg;

  tcb = tcb_arg;
  sigs = sigs_arg;

  ASSERT(tcb != NULL);
  ASSERT(sigs > 0);

  (void) rex_set_sigs(tcb, sigs);
}

fs_os_sigs_type
fs_os_wait(fs_os_signal_handle_type *sigh_arg, fs_os_sigs_type sigs_arg)
{
  rex_sigs_type sigs, ret_sigs;

  (void)sigh_arg;
  sigs = sigs_arg;

  ASSERT(sigs > 0);

  ret_sigs = rex_wait(sigs);
  return ret_sigs;
}

void fs_os_def_timer(fs_os_timer_type *timer_arg, fs_os_tcb_type *tcb_arg,
  fs_os_signal_handle_type *sigh_arg, fs_os_sigs_type  sigs_arg)
{
  rex_timer_type *timer;
  rex_tcb_type *tcb;
  rex_sigs_type sigs;

  (void) sigh_arg;

  timer = timer_arg;
  tcb = tcb_arg;
  sigs = sigs_arg;

  ASSERT(timer != NULL);
  ASSERT(tcb != NULL);
  ASSERT(sigs > 0);

  rex_def_timer(timer, tcb, sigs);
}

void
fs_os_set_timer(fs_os_timer_type *timer_arg, uint32 msecs_arg)
{
  rex_timer_type *timer;
  rex_timer_cnt_type msecs;

  timer = timer_arg;
  msecs = (rex_timer_cnt_type)msecs_arg;

  ASSERT(timer != NULL);
  ASSERT(msecs > 0);

  (void) rex_set_timer(timer, msecs);
}


void
fs_os_clr_timer(fs_os_timer_type *timer_arg)
{
  rex_timer_type *timer;

  timer = timer_arg;

  ASSERT(timer != NULL);

  (void) rex_clr_timer(timer);
}

void
fs_os_timed_wait(fs_os_signal_handle_type *sigh_arg, fs_os_sigs_type sigs_arg,
		 fs_os_timer_type *timer_arg, uint32 cnt_arg)
{
  rex_sigs_type sigs;
  rex_timer_type *timer;
  rex_timer_cnt_type cnt;

  (void) sigh_arg;

  sigs = sigs_arg;
  timer = timer_arg;
  cnt = (rex_timer_cnt_type)cnt_arg;

  ASSERT(sigs > 0);
  ASSERT(timer != NULL);

  (void) rex_timed_wait(sigs, timer, cnt);
}

fs_os_tcb_type* fs_os_self(void)
{
  rex_tcb_type *tcb;
  tcb = rex_self();
  return tcb;
}

int fs_os_autodog_enable(int dog_report_val_arg)
{
  return rex_autodog_enable(dog_report_val_arg);
}

int fs_os_autodog_enable_self(void)
{
  return rex_autodog_enable_self();
}

int fs_os_autodog_disable(void)
{
  return rex_autodog_disable();
}

void fs_os_set_dac(fs_os_tcb_type *tcb_arg, unsigned int dac_arg)
{
  rex_tcb_type* tcb;
  unsigned int dac;

  tcb = tcb_arg;
  dac = dac_arg;

  ASSERT(tcb != NULL);
#ifdef FEATURE_DAC_MEMORY_PROTECTION
  rex_set_dac(tcb, dac);
#endif
}

void
fs_os_get_cur_task_name(char *dest_arg, unsigned int size_arg)
{
  rex_tcb_type* tcb;
  size_t size, ret_size;

  tcb = rex_self();
  size = size_arg;

  (void) rex_get_task_name(tcb, dest_arg, size, &ret_size);
}

void fs_os_def_task_ext(fs_os_tcb_type *tcb_arg,  uint8 *stack_addr_arg,
      unsigned int stack_size_arg, unsigned int pri_arg,
      fs_os_task_func_type task_func_arg,
      fs_os_task_func_arg_type task_func_param_arg,
      char *task_name_arg, boolean suspended,
      int dog_report_val_arg)
{
  rex_tcb_type *tcb;
  unsigned char *stack_addr;
  rex_stack_size_type stack_size;
  rex_priority_type pri;
  rex_task_func_type task_func;
  dword task_func_param;

  tcb = tcb_arg;
  stack_addr = (unsigned char * )stack_addr_arg;
  stack_size = (rex_stack_size_type )stack_size_arg;
  pri = (rex_priority_type )pri_arg;
  task_func = (rex_task_func_type )task_func_arg;
  task_func_param = (dword)task_func_param_arg;

  ASSERT(tcb != NULL);
  ASSERT(stack_addr != NULL);
  ASSERT(stack_size > 0);
  ASSERT(pri > 0);
  ASSERT(task_func != NULL);

#ifdef FEATURE_QDSP6
  rex_def_task_ext5 (REX_ANY_CPU_AFFINITY_MASK, tcb, stack_addr,
		     stack_size, pri, task_func, task_func_param,
		     task_name_arg, suspended, dog_report_val_arg);
#else
  rex_def_task_ext (tcb, stack_addr,
		    stack_size, pri, task_func, task_func_param,
		    task_name_arg, suspended, dog_report_val_arg);
#endif
}

void fs_os_efs_errno_init (void)
{
}

#ifdef FEATURE_REX_OPAQUE_TCB_APIS
  extern int *rex_get_errno_addr (void);
#endif

int* fs_os_efs_errno_get_address (void)
{
#ifdef FEATURE_REX_OPAQUE_TCB_APIS
  return rex_get_errno_addr ();
#else /* todo : check and remove this #else part */
  fs_os_tcb_type *tcb;

  tcb = fs_os_self ();
  if (tcb == NULL)
  {
    FS_ERR_FATAL ("null tcb !!!",0,0,0);
  }

  return &(tcb->err_num);
#endif
}


#endif

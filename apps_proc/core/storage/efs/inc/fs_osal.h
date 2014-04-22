/***********************************************************************
 * fs_osal.h
 *
 * This is a thin OS abstraction layer.
 * Copyright (C) 2009-2013, 2016,2019 Qualcomm Technologies, Inc.
 *
 * Using this file allows us to compile EFS/HFAT code without any sort
 * of underlying operating system (REX in this case). This situation
 * arises when we are compiling code into qcsbl or oemsbl.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/efs/inc/fs_osal.h#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2019-01-12   as    Add timer support for Qurt osal wrapper.
2016-09-21   as    Modified errno get_address code for supporting other than Rex OS.
2016-08-08   rp    Enhance fs-osal to support both rex and qurt.
2013-02-08   wek   Fix type definitions when compiling with out rex.
2013-01-22   nr    Add fucntion to get current task name.
2012-03-20   wek   Cleanup customer header file inclusion.
2012-02-29   rp    Make Tasks free-float (CPU affinity free)
2011-11-03   wek   Remove dereference to task TCB.
2010-02-08   rp    CMI compliance, remove #ifdef from API files.
2009-06-12   ebb   Create

===========================================================================*/

#ifndef __FS_OSAL_H__
#define __FS_OSAL_H__

#include "fs_config_i.h"
#include "comdef.h"

/* Invalid Dog-report id; dog.h does not provide a #define for this. */
#define FS_DOG_INVALID_RPT        (-1)

/* REX not defined, stub everything out */
#if defined (FEATURE_EFS_COMPILE_WITHOUT_REX)

  /* These two fields must be present because they are getting dereferenced
   * in our code */
  typedef struct
  {
    int     err_num;
    uint32  dog_report_val;
  } fs_os_tcb_type;

  typedef uint32               fs_os_timer_type;
  typedef uint32               fs_os_timer_cnt_type;
  typedef uint32               fs_os_sigs_type;
  typedef uint32               fs_os_crit_sect_type;
  typedef uint32               fs_os_stack_size_type;
  typedef uint32               fs_os_priority_type;
  typedef uint32               fs_os_signal_handle_type;
  typedef unsigned long        fs_os_task_func_arg_type;
  typedef void (*fs_os_task_func_type) (fs_os_task_func_arg_type);

#elif defined (FEATURE_EFS_OSAL_IS_QURT)

  #include "qurt_thread.h"
  #include "qurt_mutex.h"
  #include "qurt_signal.h"
  #include "qurt_timer.h"
  #include "qurt_error.h"
  #include "timer.h"

  /* Data types */
  typedef qurt_thread_t           fs_os_tcb_type;
  typedef timer_type              fs_os_timer_type;
  typedef uint32                  fs_os_timer_cnt_type;
  typedef unsigned int            fs_os_sigs_type;
  typedef qurt_mutex_t            fs_os_crit_sect_type;
  typedef qurt_signal_t           fs_os_signal_handle_type;
  typedef void*                   fs_os_task_func_arg_type;
  typedef void (*fs_os_task_func_type) (fs_os_task_func_arg_type);

#else /* REX is defined, redirect all calls to the REX API */

  /* Include rex header */
  #include "rex.h"

  /* Data types */
  typedef rex_tcb_type         fs_os_tcb_type;
  typedef rex_timer_type       fs_os_timer_type;
  typedef rex_timer_cnt_type   fs_os_timer_cnt_type;
  typedef rex_sigs_type        fs_os_sigs_type;
  typedef rex_crit_sect_type   fs_os_crit_sect_type;
  typedef uint32               fs_os_signal_handle_type;
  typedef unsigned long        fs_os_task_func_arg_type;
  typedef void (*fs_os_task_func_type) (fs_os_task_func_arg_type);

#endif /* FEATURE_EFS_COMPILE_WITHOUT_REX */

#ifdef FEATURE_EFS_COMPILE_WITHOUT_REX
  /* Needed to maintain the notion of time for unit tests */
  #ifdef FS_UNIT_TEST
    extern uint64 unit_test_time;
  #endif

  fs_os_tcb_type* fs_os_self (void);

  static inline void fs_os_init_crit_sect (fs_os_crit_sect_type *cs_arg)
  {
    (void) cs_arg;
  }

  static inline void fs_os_enter_crit_sect (fs_os_crit_sect_type *cs_arg)
  {
    (void) cs_arg;
  }

  static inline void fs_os_leave_crit_sect (fs_os_crit_sect_type *cs_arg)
  {
    (void) cs_arg;
  }

  static inline void fs_os_signal_init(fs_os_signal_handle_type *sigh_arg)
  {
    (void) sigh_arg;
  }

  static inline void fs_os_signal_destroy(fs_os_signal_handle_type *sigh_arg)
  {
    (void) sigh_arg;
  }

  static inline void fs_os_clr_sigs (fs_os_tcb_type *tcb_arg,
                      fs_os_signal_handle_type *sigh_arg,
                      fs_os_sigs_type sigs_arg)
  {
    (void)tcb_arg; (void) sigh_arg; (void)sigs_arg;
  }

  static inline void fs_os_set_sigs (fs_os_tcb_type *tcb_arg,
    fs_os_signal_handle_type *sigh_arg, fs_os_sigs_type sigs_arg)
  {
    (void)tcb_arg; (void)sigh_arg; (void)sigs_arg;
  }

  static inline fs_os_sigs_type fs_os_wait (fs_os_signal_handle_type *sigh_arg,
                                               fs_os_sigs_type sigs_arg)
  {
    (void)sigh_arg, (void) sigs_arg;
    return 0;
  }

  static inline void fs_os_def_timer (fs_os_timer_type *timer_arg,
                                      fs_os_tcb_type *tcb_arg,
                                      fs_os_signal_handle_type *sigh_arg,
                                      fs_os_sigs_type sigs_arg)
  {
    (void)timer_arg; (void)tcb_arg; (void)sigh_arg; (void)sigs_arg;
  }

  static inline void fs_os_set_timer (fs_os_timer_type *timer_arg,
                                      uint32 ms_arg)
  {
    (void)timer_arg; (void)ms_arg;
  }

  static inline void fs_os_clr_timer (fs_os_timer_type *timer_arg)
  {
    (void)timer_arg;
  }

  static inline void fs_os_timed_wait(fs_os_signal_handle_type *sigh_arg,
                                      fs_os_sigs_type sigs_arg,
                                      fs_os_timer_type *timer_arg,
                                      uint32 cnt_arg)
  {
    (void) sigh_arg; (void)sigs_arg; (void)timer_arg; (void)cnt_arg;

    #ifdef FS_UNIT_TEST
      unit_test_time += cnt_arg;
    #endif
  }

  static inline int fs_os_autodog_enable (int val)
  {
    (void)val;
    return 0;
  }

  static inline int fs_os_autodog_enable_self (void)
  {
    return 0;
  }

  static inline int fs_os_autodog_disable (void)
  {
    return 0;
  }

  static inline void fs_os_set_dac (fs_os_tcb_type *tcb, unsigned int dac)
  {
    (void)tcb; (void)dac;
  }

  static inline void fs_os_def_task_ext(fs_os_tcb_type *tcb_arg,
          uint8 *stack_addr_arg, unsigned int stack_size_arg,
          unsigned int pri_arg, fs_os_task_func_type task_func_arg,
          fs_os_task_func_arg_type task_func_param_arg,
          char *task_name_arg, boolean suspended, int dog_report_val_arg)
  {
    (void)tcb_arg; (void)stack_addr_arg; (void)stack_size_arg; (void)pri_arg;
    (void)task_func_arg; (void)task_func_param_arg; (void)task_name_arg;
    (void)suspended; (void) dog_report_val_arg;
  }

  static inline void
  fs_os_get_cur_task_name(char *dest, unsigned int size)
  {
    (void) dest;
    (void) size;
  }

  static inline void
  fs_os_efs_errno_init(void)
  {
  }

  int* fs_os_efs_errno_get_address(void);

#else

  void fs_os_init_crit_sect(fs_os_crit_sect_type *cs_arg);
  void fs_os_enter_crit_sect(fs_os_crit_sect_type *cs_arg);
  void fs_os_leave_crit_sect(fs_os_crit_sect_type *cs_arg);

  void fs_os_signal_init(fs_os_signal_handle_type *sigh_arg);
  void fs_os_signal_destroy(fs_os_signal_handle_type *sigh_arg);

  void fs_os_clr_sigs(fs_os_tcb_type *tcb_arg,
    fs_os_signal_handle_type *sigh_arg, fs_os_sigs_type sigs_arg);

  void fs_os_set_sigs(fs_os_tcb_type *tcb_arg,
    fs_os_signal_handle_type *sigh_arg, fs_os_sigs_type sigs_arg);

  fs_os_sigs_type fs_os_wait(fs_os_signal_handle_type *sigh_arg,
    fs_os_sigs_type sigs_arg);

  void fs_os_def_timer(fs_os_timer_type *timer_arg, fs_os_tcb_type *tcb_arg,
    fs_os_signal_handle_type *sigh_arg, fs_os_sigs_type  sigs_arg);

  void fs_os_set_timer(fs_os_timer_type *timer_arg,
    uint32 msecs_arg);

  void fs_os_clr_timer(fs_os_timer_type *timer_arg);

  void fs_os_timed_wait(fs_os_signal_handle_type *sigh_arg,
                        fs_os_sigs_type sigs_arg,
                        fs_os_timer_type *timer_arg,
                        uint32 cnt_arg);

  fs_os_tcb_type* fs_os_self(void);

  int fs_os_autodog_enable(int dog_report_val_arg);
  int fs_os_autodog_enable_self(void);
  int fs_os_autodog_disable(void);
  void fs_os_set_dac(fs_os_tcb_type *tcb_arg, unsigned int dac_arg);

  void fs_os_get_cur_task_name(char *dest_arg, unsigned int size_arg);

  void fs_os_def_task_ext(fs_os_tcb_type *tcb_arg,  uint8 *stack_addr_arg,
        unsigned int stack_size_arg, unsigned int pri_arg,
        fs_os_task_func_type task_func_arg,
        fs_os_task_func_arg_type task_func_param_arg,
        char *task_name_arg, boolean suspended, int dog_report_val_arg);

  void fs_os_efs_errno_init(void);
  int* fs_os_efs_errno_get_address(void);

#endif /* !FEATURE_EFS_COMPILE_WITHOUT_REX */

#endif /* not __FS_OSAL_H__ */

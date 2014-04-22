/*===========================================================================
                        M O D E M C F G    O S A L  (modem_cfg_osal.h)

DESCRIPTION

   OS Abstraction layer for modem_cfg module 
   (to allow it to compile with any OS layer or even without it) 
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_proc/inc/mcfg_osal.h#1 $ $DateTime: 2020/02/19 23:07:19 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
1/18/12      sbt   Create

===========================================================================*/

#ifndef MODEM_CFG_OSAL_H
#define MODEM_CFG_OSAL_H

//#include "customer.h"
#include "comdef.h"
#include "modem_mem.h"
#include <stringl/stringl.h>

/* REX not defined, stub everything out */

//#ifdef MODEM_CFG_REX_OSAL

  /* Include rex header */
  #include "rex.h"
  #include "rcevt_rex.h"

  /* Data types */
  typedef rex_tcb_type         modem_cfg_os_tcb_type;
  typedef rex_timer_type       modem_cfg_os_timer_type;
  typedef rex_timer_cnt_type   modem_cfg_os_timer_cnt_type;
  typedef rex_sigs_type        modem_cfg_os_sigs_type;
  typedef rex_crit_sect_type   modem_cfg_os_crit_sect_type;

  /* Functions */
  #define modem_cfg_os_clr_sigs(tcb,sigs)       rex_clr_sigs(tcb,sigs)
  #define modem_cfg_os_set_sigs(tcb,sigs)       rex_set_sigs(tcb,sigs)
  #define modem_cfg_os_def_timer(tmr,tcb,sigs)  rex_def_timer(tmr,tcb,sigs)
  #define modem_cfg_os_set_timer(tmr,msecs)     rex_set_timer(tmr,msecs)
  #define modem_cfg_os_clr_timer(tmr)           rex_clr_timer(tmr)
  #define modem_cfg_os_wait(sigs)               rex_wait(sigs)
  #define modem_cfg_os_timed_wait(sigs,tmr,cnt) rex_timed_wait(sigs,tmr,cnt)
  #define modem_cfg_os_self()                   rex_self()
  #define modem_cfg_os_autodog_enable(val)      rex_autodog_enable(val)
  #define modem_cfg_os_autodog_enable_self()    rex_autodog_enable_self()
  #define modem_cfg_os_autodog_disable()        rex_autodog_disable()
  #define modem_cfg_os_init_crit_sect(cs)       rex_init_crit_sect(cs)
  #define modem_cfg_os_enter_crit_sect(cs)      rex_enter_crit_sect(cs)
  #define modem_cfg_os_leave_crit_sect(cs)      rex_leave_crit_sect(cs)
  #define modem_cfg_os_def_task_ext(tcb,stk,stksz,pri,tsk,p,nme,s) \
          rex_def_task_ext(tcb,stk,stksz,pri,tsk,p,nme,s)
  #define MODEM_CFG_OS_RCEVT_SIGEX_TYPE         RCEVT_SIGEX_TYPE_SIGREX
  #define MODEM_CFG_OS_RCEVT_SIGEX              RCEVT_SIGEX_SIGREX

// #endif /* MODEM_CFG_REX_OSAL */

#ifndef MODEM_MEM_CLIENT_MCFG_CRIT
#define MODEM_MEM_CLIENT_MCFG_CRIT  (43)  // Use the client ID for NI PL
#endif

  static int mcfg_heap_blocks = 0;

  #define mcfg_malloc(size)                     modem_mem_alloc(size, MODEM_MEM_CLIENT_MCFG_CRIT);\
                                                mcfg_heap_blocks++; \
                                                MCFG_MSG_LOW_1("mcfg_malloc, mcfg_heap_blocks %d", mcfg_heap_blocks)

  #define mcfg_free(ptr)                        modem_mem_free(ptr, MODEM_MEM_CLIENT_MCFG_CRIT); \
                                                ptr = NULL;\
                                                mcfg_heap_blocks--;\
                                                MCFG_MSG_LOW_1("mcfg_free, mcfg_heap_blocks %d", mcfg_heap_blocks)


#endif /* MODEM_CFG_OSAL_H */

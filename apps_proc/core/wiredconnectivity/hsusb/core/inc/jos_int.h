/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.



==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/inc/jos_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================








************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _JOS_INT_H
#define _JOS_INT_H
#include "jos.h"
#include <jconfig.h>

#include "jdebug.h"
#include "jtypes_int.h"
#include "compiler_int.h"
#include "os_port_int.h"
#include "jutil_int.h"
#ifdef CONFIG_TEST_DICT
# include <test_dict.h>
#endif

#ifdef CONFIG_BUS_PCI
# include "pci_defs.h"
#endif

/* Task manipulation methods */
typedef enum {
    /* Soft interrupts */
    TASK_CONTROLLER =   THREAD_PRIORITY_CONTROLLER,

#ifndef CONFIG_TASK_SINGLE
    /* Core specific, non-blocking tasks */
    TASK_CORE =         THREAD_PRIORITY_CORE,

    /* Generic task for use by class & function drivers */
    TASK_CLASS_DRIVER = THREAD_PRIORITY_DRIVER,
    
#ifdef JDEBUG
    /* Used by the monitor task, to catch deadlocks. 
     * Must be one before last */
    TASK_DEBUG =        THREAD_PRIORITY_OTHER, 
#endif
#endif
    /* Must be last ! */
    TASK_TYPE_COUNT 
} jtask_type;

#ifndef SINGLE_THREADED
jresult_t jtask_init(jtask_h *task, jtask_type type);
# ifndef JDEBUG
jresult_t jtask_schedule(jtask_h task, juint32_t timeout, 
    jthread_func cb, void *arg);
# else
#  define jtask_schedule(a,b,c,d)   \
    jtask_schedule_d(a,b,c,d, __FILE__, __LINE__)
jresult_t jtask_schedule_d(jtask_h task, juint32_t timeout, 
    jthread_func cb, void *arg, char *file, jint_t line);
# endif
#endif

#ifndef SINGLE_THREADED
# define CORE_SAFE_ENTER        jcore_safe_enter()
# define CORE_SAFE_LEAVE        jcore_safe_leave()
void jcore_safe_enter(void);
void jcore_safe_leave(void);
#else
# define CORE_SAFE_ENTER        do { } while (0)
# define CORE_SAFE_LEAVE        do { } while (0)
#endif

#ifndef PAGE_SHIFT
#define PAGE_SHIFT      12              /* LOG2(PAGE_SIZE) */
#endif
#ifndef LINUX24
#ifndef PAGE_SIZE
#define PAGE_SIZE       (1<<PAGE_SHIFT) /* bytes/page */
#endif
#endif

#ifdef FEATURE_HS_USB_MEM_TEST
void jos_mem_test(void* param);
#endif // FEATURE_HS_USB_MEM_TEST
#endif


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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/classes/inc/os_port_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================










************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _OS_PORT_INT_H
#define _OS_PORT_INT_H
#include "os_port.h"
#include <os_common_int.h>
#ifdef SINGLE_THREADED
# define jthread_create(a,b,c,d)  0
# define jmutex_init(a)         0
# define jmutex_uninit(a)       do { } while (0)
# define jmutex_lock(a)         do { } while (0)
# define jmutex_unlock(a)       do { } while (0)

# define jdelay_us              hsu_os_delay_us
# define jdelay_ms(a)           hsu_os_delay_us((a) * 1000)

# define jmsleep_init           0
# define jmsleep_uninit         0
# define jmsleep(a,b)           0
# define jwakeup(a)             do { } while (0)

/*
These functions are unimplemented

# define jtask_h                os_timer_h
# define jtask_init(a,b)        os_timer_init(a)
# define jtask_uninit           os_timer_uninit
# define jtask_schedule         os_timer_reset
# define jtask_stop             os_timer_stop
# define jtask_pending          os_timer_pending
*/


#else /* Multi threaded */

# define jthread_create         hsu_os_thread_create

#ifdef DEBUG_SYNC
# define jmutex_init(a)         jmutex_init_d(a, __FILE__, __LINE__)
# define jmutex_uninit(a)       jmutex_uninit_d(a, __FILE__, __LINE__)
# define jmutex_lock(a)         jmutex_lock_d(a, __FILE__, __LINE__)
# define jmutex_unlock(a)       jmutex_unlock_d(a, __FILE__, __LINE__)
typedef void *jmutex_h;
jresult_t jmutex_init_d(jmutex_h *mtx_h, char *file, int line);
void jmutex_uninit_d(jmutex_h mtx_h, char *file, int line);
void jmutex_lock_d(jmutex_h mtx_h, char *file, int line);
void jmutex_unlock_d(jmutex_h mtx_h, char *file, int line);
#else
# define jmutex_h               os_mutex_h
# define jmutex_init            hsu_os_mutex_init
# define jmutex_uninit          hsu_os_mutex_uninit
# define jmutex_lock            hsu_os_mutex_lock
# define jmutex_unlock          hsu_os_mutex_unlock
#endif

# define jdelay_us              hsu_os_delay_us
# define jdelay_ms              hsu_os_delay_ms

# define jmsleep_init           hsu_os_msleep_init
# define jmsleep_uninit         hsu_os_msleep_uninit
# define jmsleep                hsu_os_msleep
# define jwakeup                hsu_os_wakeup
#endif

/* HSU modification - Make jprintf use AMSS logging mechanism. (Start) */
  #ifdef JDEBUG
    #define jprintf                 MSG_HIGH_VAR_ARGS
  #else
    #define jprintf
  #endif /* JDEBUG */
/* HSU modification - Make jprintf use AMSS logging mechanism. (End) */

#include <port_int.h>

/* HSU Addition */
#define j_panic                 HSU_ASSERT
/* HSU Addition End */

#endif


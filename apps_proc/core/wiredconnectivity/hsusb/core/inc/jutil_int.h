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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/inc/jutil_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================









************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _JUTIL_INT_H
#define _JUTIL_INT_H
#include "jutil.h"
#include "jdebug.h"
#include <hsu_common_int.h>
#ifdef JDEBUG
/* Used to catch invalid pointer dereferences */
# define INVALID_HANDLE ((void *)MAGIC_NUMBER)
#else
# define INVALID_HANDLE         ((void*)-1)
#endif

#ifdef DEBUG_PERF_LOG
extern jbool_t jstart_perf;
void jdump_time(void);
void jmark_time_v(char *file, jint32_t line);
# define DEBUG_PERF_LOG_ON jstart_perf = 1;
# define DEBUG_PERF_LOG_OFF do { jdump_time(); jstart_perf = 0; } while(0);
# define DEBUG_PERF_LOG_MARK do { jmark_time_v(__FILE__, __LINE__); } while(0);
#else
# define DEBUG_PERF_LOG_ON 
# define DEBUG_PERF_LOG_OFF 
# define DEBUG_PERF_LOG_MARK 
#endif

/* HSU change - Use ERR FATAL in place of j_panic in the KASSERT macro. */
#define KASSERT(expr,msg) \
  if (!(expr)) \
  { \
    ERR_FATAL( "hsu " #expr, 0, 0, 0 ); \
  }

#endif


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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/classes/inc/os_common_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================









************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _OS_COMMON_INT_H
#define _OS_COMMON_INT_H
#include "os_common.h"
#include <compiler_int.h>
#include <jdebug.h>
#include <jtypes_int.h>

#ifdef JDEBUG
# ifdef CONFIG_PORT_NO_GET_CONTEXT
/* HSU Fix:
** For now, we deliberately disable synchronization debugging,
** and thus we do not need the warnings.
*/
#if 0
#  ifndef T_REXNT
#   warning "Cannot debug sychronization"
#   warning "No 'hsu_os_get_context()' configure flag is set in the config file."
#  else
#   pragma message("Cannot debug sychronization")
#   pragma message("No 'hsu_os_get_context()' configure flag is set in the config file.")
#  endif
#endif /* 0 */
/* End HSU Fix */
#  undef  DEBUG_SYNC
# else
#  define DEBUG_SYNC
# endif
#endif

#endif


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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/classes/inc/port_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================










************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _PORT_INT_H
#define _PORT_INT_H
#include "port.h"
#include <jconfig.h>
#include <compiler_int.h>
#include <os_common_int.h>
#include <port_sync_int.h>
#include <port_generic_int.h>
#include <port_bus_int.h>
#include "uw_args.h"
#ifdef CONFIG_BUS_PCI
# include <pci_defs.h>
# include <port_pci.h>
#endif

#ifdef CONFIG_STATISTICS   
# include <port_statistics.h>
#endif

#if defined(CONFIG_TEST) && !defined(CONFIG_TEST_AUTORUN)
# include <port_test.h>
#endif

/* Usb stack API's to be used by the porting layer */

/* Initialize and load the stack */
jresult_t jstart_stack(uw_args_t *args);

#endif


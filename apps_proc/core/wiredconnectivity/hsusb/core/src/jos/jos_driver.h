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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/src/jos/jos_driver.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================










************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _JOS_DRIVERS_H_
#define _JOS_DRIVERS_H_

#include <jos_int.h>
#include "jos_internal.h"

jresult_t jdrivers_init(void);
void jdrivers_uninit(void);

/* Drivers manipulation */
jresult_t jdriver_register(jint_t type, driver_methods_t *methods,
    char *name, jint_t id, juint16_t size);

jresult_t jdriver_attach(jint_t type, jdevice_t device, jint_t id, void *arg);
void jdriver_detach(jdevice_t device);

#endif


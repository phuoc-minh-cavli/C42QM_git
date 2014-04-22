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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/classes/inc/port_bus_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================










************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _PORT_BUS_INT_H
#define _PORT_BUS_INT_H
#include "port_bus.h"
typedef enum {
#ifdef CONFIG_JSLAVE
    NOTIFY_DEVICE_CONNECT       = 0,
    NOTIFY_DEVICE_DISCONNECT,
    NOTIFY_DEVICE_SUSPEND,
    NOTIFY_DEVICE_RESUME,
    NOTIFY_DEVICE_RESUME_COMPLETED,
    NOTIFY_DEVICE_REMOTE_WAKEUP,
    NOTIFY_DEVICE_CONFIGURED,
    NOTIFY_DEVICE_UNCONFIGURED,
    /* HSU Addition */
    NOTIFY_DEVICE_RESET,
    NOTIFY_DEVICE_SPEED_CHANGE,
#ifdef FEATURE_HS_USB_TEST_SPOOF_DISCONNECT
    NOTIFY_DEVICE_SPOOF_CONNECT,
    NOTIFY_DEVICE_SPOOF_DISCONNECT,
#endif // FEATURE_HS_USB_TEST_SPOOF_DISCONNECT
    /* HSU Addition end */
#endif
    
#ifdef CONFIG_JHOST 
    NOTIFY_HOST_CONNECT         = 50,
    NOTIFY_HOST_SUSPEND,
    NOTIFY_HOST_RESUME,
#endif

#ifdef CONFIG_JOTG
    NOTIFY_OTG_IDLE             = 100,
    NOTIFY_OTG_BUSY, 
#endif

    NOTIFY_LAST
} jevent_t;

jresult_t hsu_os_notify(jevent_t event, void *param);


#endif


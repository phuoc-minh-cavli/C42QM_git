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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/classes/inc/port_sync_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================










************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _PORT_SYNC_INT_H
#define _PORT_SYNC_INT_H
#include "port_sync.h"
#include <port_int.h>
#include <jtypes_int.h>
#ifndef SINGLE_THREADED
/* context tracker (ISR, DSR or normal) */
#define NORMAL_CONTEXT 0x1
#define DSR_CONTEXT 0x2
#define ISR_CONTEXT 0x4
extern juint8_t jcurr_ctx;

#ifdef JDEBUG
/* HSU ADDITION */                                  
#define ASSERT_CONTEXT(valid_ctx)                               \
    do {                                                        \
        if (!((jcurr_ctx) & (valid_ctx)))                       \
        {                                                       \
            HSU_ERR_FATAL("Context invalid: jcurr_ctx (%u), valid_ctx (%u)", jcurr_ctx, valid_ctx, 0);     \
        }                                                       \
    } while (0)
/* HSU ADDITION */                                  
#else

#define ASSERT_CONTEXT(valid_ctx)                               

#endif
#endif

#endif


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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/inc/jusb_common_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================









************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _JUSB_COMMON_INT_H
#define _JUSB_COMMON_INT_H
#include "jusb_common.h"
#include <jos_int.h>
#include <jutil_int.h>
#include "jslave_init.h"
#if defined(CONFIG_JSLAVE_TEST_MODE) && defined(CONFIG_FULL_SPEED_ONLY)
#error "CONFIG_JSLAVE_TEST_MODE can not be set with CONFIG_FULL_SPEED_ONLY"
#endif

jresult_t core_init(uw_hw_core_param_t *hw_core_params, juint8_t num_of_cores);
jresult_t core_reconfig(juint8_t core_index, device_params_t *new_config,
    reconfig_complete_cb_t cb, void *arg);

#endif


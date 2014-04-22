/*==============================================================================

     High Speed USB Configuration Selector Stack Utilities
        H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This module defines functions that setup, stop, re-configure, etc. the USB 
    stack.

  EXTERNALIZED FUNCTIONS
    hsu_conf_sel_stack_utils_init_memory
    hsu_conf_sel_stack_utils_setup_device_stack
    hsu_conf_sel_stack_utils_switch_device_stack
    hsu_conf_sel_stack_utils_handle_chg_connected
    hsu_conf_sel_stack_utils_handle_chg_disconnected
    hsu_conf_sel_stack_utils_enable_perf_boost
    hsu_conf_sel_stack_utils_device_restart_core

    hsu_conf_sel_stack_utils_is_forced_disable
    hsu_conf_sel_stack_utils_force_disable_start
    hsu_conf_sel_stack_utils_force_disable_stop
    hsu_conf_sel_stack_utils_disable
    hsu_conf_sel_stack_utils_enable

  INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_conf_sel_stack_utils_init_memory should be called before any other 
    function, and before the CDC ALs are initialized. 
    
  Copyright (c) 2007-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_conf_sel_stack_utils_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  08/25/09 rg  Initial version
    
==============================================================================*/
#ifndef _HSU_CONF_SEL_STACK_UTILS_INT_H
#define _HSU_CONF_SEL_STACK_UTILS_INT_H
#include "hsu_conf_sel_stack_utils.h"

#include "customer.h" 
#include "jos_int.h"
#include "jslave_init.h"
#include "jtypes_int.h"
#include "hsu_conf_sel_types_int.h"


/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
#ifdef T_UNDP
boolean hsu_is_undp_net_first_comp_id
(
  hsu_conf_sel_composition_id_enum composition_id
);

boolean hsu_is_undp_nmea_comp_id
(
  hsu_conf_sel_composition_id_enum composition_id
);
boolean hsu_is_undp_ser3_port_enabled (void);
boolean hsu_is_undp_curr_comp_id_for_nmea (void);
#endif /* T_UNDP */
/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_SETUP_STACK

DESCRIPTION
  This function is called both by the config_selector (when FEATURE_HSU_TEST is 
  NOT defined) and by the ARMPROG/DLOAD Adaptation Layers.
  It sets-up the USB device stack (by calling jstart_stack and jslave_enable) 
  based on the cores configuration. 

DEPENDENCIES
  Assumes all AL related initialization were already done. 
  Should be called only once after power up.

RETURN VALUE
  jresult_t - HSU_JSUCCESS for success, other values for failure (currently all 
  cause an ASSERT).

SIDE EFFECTS
  None.

==============================================================================*/
jresult_t hsu_conf_sel_stack_utils_setup_stack(
#ifdef T_UNDP
  boolean 	use_amss_product_id
#else
  void
#endif
);
/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_RECONFIG_DONE

DESCRIPTION
This function is called when the stack re-configuration process is completed. 
This function updates the ARM9 product_id and serial_number to be used in SW 
download.

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
void hsu_conf_sel_stack_utils_reconfig_done
(
 hsu_conf_sel_comp_info_type* comp_info
);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_SET_SERIAL_NUMBER

DESCRIPTION
This function sets the serial number according to the composition and to the
configuration (AMSS / DLOAD / ARMPROG)

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_conf_sel_stack_utils_set_serial_number(
  dev_info_t *  device_info,
  uint16        product_id,
  uint8         core_idx
);

#ifdef T_UNDP
/*==============================================================================

FUNCTION          HSU_IS_ALTERNATE_NET_ENABLED

DESCRIPTION
This function checks if the current config enabled alternate NET or not. 
By default, this feature should be set as DISABLED.
Also, it should return FALSE if this function is called in DLOAD/QDL mode

DEPENDENCIES
None.

RETURN VALUE
TRUE/FALSE

SIDE EFFECTS
None.
==============================================================================*/
boolean hsu_is_alternate_net_enabled (void);
#endif /* T_UNDP */

/*===========================================================================
FUNCTION HSU_CONF_SEL_STACK_UTILS_RESTART_CORE_CONFIRM_COMPLETE

DESCRIPTION
Wrapper function for confirming the asynchronous completion of restarting core

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_conf_sel_stack_utils_restart_core_confirm_complete(uint32 logical_core_index);


/*=============================================================================
FUNCTION    hsu_conf_sel_stack_utils_enable_perf_boost_idx
=============================================================================*/
/**
Configures various hardware registers for throughput optimizations.

@dependencies
The enabling of the various hardware registers is dependent on the hardware
version.

@sideeffects
None.

@return
None.
*/
void hsu_conf_sel_stack_utils_enable_perf_boost_idx(uint8 core_index);

/*=============================================================================
FUNCTION    hsu_conf_sel_alloc_uncached_memory
=============================================================================*/
/**
This function allocates physically contiguous uncached memory specified by
the parameter size.

@dependencies
None.

@sideeffects
None.

@return
Memory pointer to uncached memory.
*/
void* hsu_conf_sel_alloc_uncached_memory(uint32 size);
#endif


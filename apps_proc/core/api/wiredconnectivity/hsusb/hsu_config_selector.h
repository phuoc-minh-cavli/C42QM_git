#ifndef _HSU_CONFIG_SELECTOR_H_
#define _HSU_CONFIG_SELECTOR_H_

/*=============================================================================

High Speed USB configuration Selector Module        
          H E A D E R   F I L E

=============================================================================*/
/**
@file hsu_config_selector.h

Initializes the High-Speed USB stack.

Externalized functions:
- hsu_config_selector_init()
- hsu_get_num_of_net_instances()
- hsu_conf_sel_get_rm_wakeup_delay()
- hsu_conf_sel_diag_is_on_legacy_port()
- hsu_conf_sel_uim_first_inst_class()
- hsu_conf_sel_use_pmic_otg_comparators()
- hsu_conf_sel_reconnect_on_reset_during_suspend()
- hsu_conf_sel_number_of_sdcc_luns()
- hsu_conf_sel_disable_sleep_voting()
- hsu_config_selector_is_standard_ecm_supported()
- hsu_config_selector_is_standard_acm_supported()
- hsu_conf_sel_is_hsusb_initialized()
- hsu_conf_sel_update_bmp_composition()
- hsu_conf_sel_get_bmp_device_info()
- hsu_conf_sel_hs_main_core_enabled()
- hsu_conf_sel_hsic_core_enabled()
- hsu_conf_sel_is_diag_over_hsic() 
- hsu_conf_sel_hide_ftl()
- hsu_config_selector_is_ahb2ahb_bypass_enabled()

hsu_config_selector_init() should be called after DSM pools are initialized,
and before the creation of tasks that use USB, such as DIAG, Modem, RmNET, 
etc.
*/

/*=============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/wiredconnectivity/hsusb/hsu_config_selector.h#1 $
$DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/25/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
09/12/06  ke  Initial version

=============================================================================*/
/*=============================================================================
Copyright (c) 2007-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

#include "nv_items.h"
#include "rdevmap.h"
#include "uw_args.h"
#include "hsu_conf_sel_types.h"

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
Forward Declarations
=============================================================================*/

/*=============================================================================
Constants
=============================================================================*/

/*=============================================================================
Macros
=============================================================================*/

/*=============================================================================
Typedefs
=============================================================================*/

/** BrewMP device data structure.
*/
typedef struct hsu_conf_sel_brewmp_device_data_struct 
{
  boolean               is_initialized;
    /**< TRUE if the BrewMP device data structure is initialized. */
  dev_info_t            device_info;
    /**< Device information. */
  config_param_t        configuration_info;
    /** Configuration information. */
  uint8                 num_of_rdm_dev_in_comp;
    /**< Number of RDM devices used. */
  rdm_device_enum_type  rdm_devices[HSU_NUM_OF_RDM_USB_PORTS];
    /**< Array of RDM devices. */
} hsu_conf_sel_brewmp_device_data_type;


/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================
FUNCTION    hsu_config_selector_init
==============================================================================*/
/**
Initializes the High-Speed USB stack according to the default composition;
called from the application's processor TMC.

@dependencies
Should be called after DSM pools are initialized, and before the creation of 
tasks that use USB, such as DIAG, Modem, RmNET, etc.

@sideeffects
Various tasks related to the HS-USB stack are created (HSU_AL_MS_TASK, 
HSU_AL_TASK, and several tasks that are created by Jungo).

@return
None.
*/
void hsu_config_selector_init(void);

/*===========================================================================
FUNCTION    hsu_get_num_of_net_instances
==============================================================================*/
/**
Returns the number of network (ECM) FD instances included in
the current composition for the primary core.

@dependencies
FEATURE_HS_USB must be defined. 
Must be called after hsu_config_selector_init().

@sideeffects
None.

@return
The number of network (ECM) FD instances included in the current
composition.
*/
uint8 hsu_get_num_of_net_instances(void);

/*===========================================================================
FUNCTION    hsu_get_num_of_acm_instances
==============================================================================*/
/**
Returns the number of ACM FD instances included in
the current composition for the primary core.

@dependencies
Must be called after hsu_config_selector_init().

@sideeffects
None.

@return
The number of ACM FD instances included in the current
composition.
*/
uint8 hsu_get_num_of_acm_instances(void);

/*===========================================================================
FUNCTION    hsu_conf_sel_change_pid_comp
==============================================================================*/
/**
Updates the PID for enumeration on boot up.

@dependencies
Must be called after hsu_config_selector_init().

@sideeffects
The hsusb composition will change from default value with PID passed.

@return
TRUE on Success or False on Failure

*/
boolean hsu_conf_sel_change_pid_comp(uint16);

/*===========================================================================
FUNCTION    hsu_conf_sel_get_cur_pid
==============================================================================*/
/**
Returns the current PID for the device.

@dependencies
Must be called after hsu_config_selector_init().

@sideeffects
none

@return
PID of the HSUSB composition

*/
uint16 hsu_conf_sel_get_cur_pid(void);



#endif /* _HSU_CONFIG_SELECTOR_H_ */

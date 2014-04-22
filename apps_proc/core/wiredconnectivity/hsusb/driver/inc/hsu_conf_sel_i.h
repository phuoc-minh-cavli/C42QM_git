#ifndef _HSU_CONF_SEL_I_H_
#define _HSU_CONF_SEL_I_H_

/*==============================================================================

     High Speed USB configuration Selector Module        
        I N T E R N A L  H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This file defined the HSU Configuration Selector module's internal functions,
    meaning, functions that should only be used from HSU source files. 

  EXTERNALIZED FUNCTIONS
    hsu_conf_sel_get_cur_comp_details
    hsu_conf_sel_get_cur_comp_id
    hsu_conf_sel_switch_dev_stack

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.
    
  Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_conf_sel_i.h#2 $
  $DateTime: 2021/02/08 00:10:03 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  09/12/06  ke      Initial version
    
==============================================================================*/
#include "jos_int.h"
#include "jslave_init.h"
#include "hsu_config_selector.h"
#include "hsu_conf_sel_types_int.h"
#include "jtypes_int.h"
#include "hsu_conf_sel_ui_int.h"
#include "sio.h"

/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================

FUNCTION          HSU_CONF_SEL_GET_CUR_COMP_DETAILS

DESCRIPTION
  This function returns a pointer to the information relevant for the current 
  composition, for the primary core.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to hsu_conf_sel_comp_info_type for success, 
  Currently all failures cause ASSERT.

SIDE EFFECTS
  None.

==============================================================================*/
const hsu_conf_sel_comp_info_type * 
  hsu_conf_sel_get_cur_comp_details(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_GET_CUR_COMP_ID

DESCRIPTION
  This function returns the hsu_conf_sel_composition_id_enum of the current 
  composition, for the primary core.

DEPENDENCIES
  None.

RETURN VALUE
  hsu_conf_sel_composition_id_enum.

SIDE EFFECTS
  None.

==============================================================================*/
hsu_conf_sel_composition_id_enum hsu_conf_sel_get_cur_comp_id(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_SWITCH_DEV_STACK

DESCRIPTION
  This function first stops the current device stack, and then initializes and 
  starts the device stack according to the new composition given as a parameter.
  It registers reconfig_cb_func_ptr as the callback function to be invoked when 
  the stack re-configuration process is completed. 

DEPENDENCIES
  Assumes the device stack is up. 

RETURN VALUE
  TRUE for success, currently all failures cause an ASSERT.

SIDE EFFECTS
  cur_comp_details is updated with the new composition.

==============================================================================*/
boolean hsu_conf_sel_switch_dev_stack
(
  hsu_conf_sel_composition_id_enum  new_hsu_comp_id,
  reconfig_complete_cb_t            reconfig_cb_func_ptr,
  void *      arg_for_reconfig_cb
);

/*===========================================================================
FUNCTION    hsu_conf_sel_get_remote_wakeup_timeout_val
==============================================================================*/
/**
Returns the value of timer for gpio_remote_wakeup_timeout

@dependencies
Must be called after hsu_config_selector_init().

@sideeffects
None.

@return
The value of timer for gpio_remote_wakeup_timeout.
*/
uint32 hsu_conf_sel_get_remote_wakeup_timeout_val(void);

/*===========================================================================
FUNCTION    hsu_conf_sel_get_a2_pc_debounce_time_for_suspend_val
==============================================================================*/
/**
Returns the timer value for A2 Power collapse debounce timer

@dependencies
Must be called after hsu_config_selector_init().

@sideeffects
None.

@return
Returns the timer value for A2 Power collapse debounce timer

*/
uint32 hsu_conf_sel_get_a2_pc_debounce_time_for_suspend_val(void);

/*===========================================================================
FUNCTION    hsu_conf_sel_get_aggressive_ep0_error_capturing_val
==============================================================================*/
/**
Returns the value of aggressive_ep0_error_capturing EFS item.

@dependencies
Must be called after hsu_config_selector_init().

@sideeffects
None.

@return
Returns the value of aggressive_ep0_error_capturing EFS item.

*/
uint8 hsu_conf_sel_get_aggressive_ep0_error_capturing_val(void);

/*===========================================================================
FUNCTION    hsu_conf_sel_disable_a2_pc_for_suspend_val
==============================================================================*/
/**
Returns the value of disable_a2_pc_for_suspend EFS item.

@dependencies
Must be called after hsu_config_selector_init().

@sideeffects
None.

@return
Returns the value of disable_a2_pc_for_suspend EFS item.

*/
boolean hsu_conf_sel_disable_a2_pc_for_suspend_val(void);

/*===========================================================================
FUNCTION    hsu_config_selector_is_ecm_int_ep_sharing_supported
==============================================================================*/
/**
This function returns TRUE if the current composition is one that supports 
ECM interrupt Endpoint mode.

@dependencies
Must be called after hsu_config_selector_init().

@sideeffects
Only the first ECM instance will have a Interrupt pipe rest of the ECM instance 
will only have a BULk In adn BULK OUT and will share the Interrupt pipe from 
first intstance

@return
TRUE if the current composition is one that supports ECM interrupt Endpoint mode.

*/
boolean hsu_config_selector_is_ecm_int_ep_sharing_supported(void);

/*==============================================================================

FUNCTION          hsu_conf_sel_is_curr_instance_intr_in_only_ecm

DESCRIPTION
This function returns TRUE if the current composition is one that supports 
INTR IN only ECM mode i.e. RMNET instance - BUNL IN & BULK OUT

DEPENDENCIES
This does not support miltiple instanceand the first instance will always be MBIM/RMNET
so this function returns TRUE only on second call. All other calls it will returns FALSE. 

This will always be the second ECM instance

RETURN VALUE
TRUE if current composition supports INTR In ONLY ECM interface.

SIDE EFFECTS
This does not support miltiple instanceand the first instance will always be MBIM/RMNET
so this function returns TRUE only on second call. All other calls it will returns FALSE. 

This will always be the second ECM instance

==============================================================================*/
boolean hsu_conf_sel_is_curr_instance_intr_in_only_ecm(uint8);


/*==============================================================================

FUNCTION          hsu_conf_sel_is_curr_instance_dpl

DESCRIPTION
This function returns TRUE if the current composition is one that supports 
DPL which only has a BULK IN interface

DEPENDENCIES


RETURN VALUE
TRUE if current composition supports BULK IN only.

SIDE EFFECTS

==============================================================================*/
boolean hsu_conf_sel_is_curr_instance_dpl(uint8 instance_ecm);


/*===========================================================================
FUNCTION          HSU_CONF_SEL_IS_EP0_AGGRESSIVE_ERR_CHK_ENABLED
==============================================================================*/

/**
This function returns TRUE when HSU_EFS_AGGRESSIVE_EP0_ERROR_CAPTURING_ITEM_FILE
bit field 0 is set.  This may increase the frequency of assertion errors and
should only be used to debug specific EP0 related issues.

@dependencies
None.

@sideeffects
Increased frequency of assertion errors may be seen.  This feature
should only be enabled to debug specific EP0 related issues

@return
TRUE if the aggressive error checking is enabled.  FALSE when aggressive error
checking is not enabled.

*/
boolean hsu_conf_sel_is_ep0_aggressive_err_chk_enabled(void);


/*===========================================================================
FUNCTION          HSU_CONF_SEL_IS_EP0_TIMER_ON_RX_TX_ZLP_ERR_CHK_ENABLED
==============================================================================*/

/**
This function returns TRUE when HSU_EFS_AGGRESSIVE_EP0_ERROR_CAPTURING_ITEM_FILE
bit field 1 is set.  This may increase the frequency of assertion errors and
should only be used to debug specific EP0 related issues.

@dependencies
None.

@sideeffects
Increased frequency of assertion errors may be seen.  This feature
should only be enabled to debug specific EP0 related issues

@return
TRUE if the timer based aggressive error checking is enabled.  FALSE when aggressive error
checking is not enabled.

*/
boolean hsu_conf_sel_is_ep0_timer_on_rx_tx_zlp_err_chk_enabled(void);

/*===========================================================================
FUNCTION    hsu_conf_sel_ignore_dtr_drop
==============================================================================*/
/**
Returns the value of ignore_dtr_drop EFS item.

@dependencies
Must be called after hsu_config_selector_init().

@sideeffects
None.

@return
Returns the value of ignore_dtr_drop EFS item.

*/
boolean hsu_conf_sel_ignore_dtr_drop(void);

/*===========================================================================
FUNCTION    hsu_conf_sel_mbim_wMTU
==============================================================================*/
/**
Returns the value of mbim_wMTU from EFS item.
The mbim_wMTU would be set to default if EFS item does not hold valid range.

@dependencies
Must be called after hsu_config_selector_init().

@sideeffects
None.

@return
Returns the value of mbim_wmtu EFS item.

*/
uint16 hsu_conf_sel_mbim_wMTU(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_is_diag_over_hsic
=============================================================================*/
/**

This function returns whether the DIAG is over High-Speed Inter-Chip.

@dependencies
None.

@sideeffects
None.

@return
TRUE -- If the value of the hs_usb diag_over_hs_core EFS item is DIAG over 
        High-Speed Inter-Chip or no HS-USB OTG core is defined or EFS item
        could not be read.\n
FALSE -- If the value of the hs_usb diag_over_hs_core EFS item is DIAG over HS-USB.   
*/
boolean hsu_conf_sel_is_diag_over_hsic (void);

/*=============================================================================
FUNCTION    hsu_conf_sel_is_rmnet_over_hsusb
=============================================================================*/
/**

This function returns whether the RMNET is over High-Speed USB.

@dependencies
None.

@sideeffects
None.

@return
FALSE -- If the value of the hs_usb rmnet_over_hs_core EFS item is RMNET over 
        High-Speed Inter-Chip or no HS-USB OTG core is defined or EFS item
        could not be read.\n
TRUE -- If the value of the hs_usb rmnet_over_hs_core EFS item is RMNET over HSUSB.   
*/
boolean hsu_conf_sel_is_rmnet_over_hsusb (void);

/*=============================================================================
FUNCTION    hsu_conf_sel_is_instance_mbim
=============================================================================*/
/**
Returns TRUE if the current composition is one that supports MBIM.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The current composition supports MBIM.\n
FALSE -- The current composition does not support MBIM.
*/
boolean hsu_conf_sel_is_instance_mbim(uint8);

/*=============================================================================
FUNCTION    HSU_CONFIG_SELECTOR_IS_QDSS_BAM_SUPPORTED
=============================================================================*/
/**
Returns TRUE if the current composition is one that supports QDSS in BAM mode.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The current composition supports QDSS in BAM mode.\n
FALSE -- The current composition does not support QDSS in BAM mode.
*/
boolean hsu_config_selector_is_qdss_bam_supported(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_is_diag_over_mbim
=============================================================================*/
/**
Returns TRUE if the current composition is one that supports MBIM only.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The current composition supports MBIM only.\n
FALSE -- The current composition does not support MBIM only.
*/
boolean hsu_conf_sel_is_diag_over_mbim(void);

/*==============================================================================
FUNCTION    hsu_conf_sel_get_rm_wakeup_delay
==============================================================================*/
/**
Returns the delay (in milliseconds) used for performing an
explicit remote wakeup.

@dependencies
None.

@sideeffects
None.

@return
The remote wakeup delay (in milliseconds).
*/
uint32 hsu_conf_sel_get_rm_wakeup_delay(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_diag_is_on_legacy_port
=============================================================================*/
/**
Reads the NV item that determines whether DIAG should run over full-speed or
high-speed USB and saves it in the variable hsu_diag_on_legacy_usb_port.

@note This function is relevant only if both FS-legacy and HS-OTG cores are
      supported in the image. Otherwise, DIAG is always routed from the primary
	  core.

@dependencies
hsu_config_selector_init must be called before this function.

@sideeffects
None.

@return
TRUE  -- DIAG should run over full-speed USB port.\n
FALSE -- DIAG should run over high-speed USB port.
*/
boolean hsu_conf_sel_diag_is_on_legacy_port(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_uim_first_inst_class
=============================================================================*/
/**
Returns the operating mode of the Universal Integrated Circuit Card.

@dependencies
hsu_config_selector_init must be called before this function.

@sideeffects
None.

@return
UICC operating mode.
*/
nv_uim_first_inst_class_type hsu_conf_sel_uim_first_inst_class(void);
  
/*=============================================================================
FUNCTION    hsu_conf_sel_use_pmic_otg_comparators
=============================================================================*/
/**
Returns whether or not to use Power Management IC OTG comparators, based on the
value of the NV item hs_usb_use_pmic_otg_comparators.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- PMIC comparators are used during LPM.\n
FALSE -- PMIC comparators are not used during LPM.
*/
boolean hsu_conf_sel_use_pmic_otg_comparators(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_reconnect_on_reset_during_suspend
=============================================================================*/
/**
Returns whether or not to use the workaround for the issue with the USB bus
reset being on during TCXO shutdown, based on the value of the NV item
hs_usb_reconnect_on_reset_during_suspend.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The workaround is used.\n
FALSE -- The workaround is not used.
*/
boolean hsu_conf_sel_reconnect_on_reset_during_suspend(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_number_of_sdcc_luns
=============================================================================*/
/**
Returns the number of SDCC/MMC LUNs that are enabled.

@dependencies
None.

@sideeffects
None.

@return
The number of SDCC/MMC LUNs that are enabled.
*/
uint8 hsu_conf_sel_number_of_sdcc_luns (void);

/*=============================================================================
FUNCTION    hsu_config_selector_is_standard_ecm_supported
=============================================================================*/
/**
Returns TRUE if the current composition is one that supports standard ECM mode.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The current composition supports standard ECM mode.\n
FALSE -- The current composition does not support standard ECM mode.
*/
boolean hsu_config_selector_is_standard_ecm_supported(void);

/*=============================================================================
FUNCTION    hsu_config_selector_is_standard_acm_supported
=============================================================================*/
/**
Returns TRUE if the current composition is one that supports standard ACM mode.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The current composition supports standard ACM mode.\n
FALSE -- The current composition does not support standard ACM mode.
*/
boolean hsu_config_selector_is_standard_acm_supported(void);


/*=============================================================================
FUNCTION    hsu_conf_sel_disable_sleep_voting
=============================================================================*/
/**
Returns whether or not to disable voting for sleep when entering LPM, based on
the value of the NV item hs_usb_disable_sleep_voting.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The value of the hs_usb_disable_sleep_voting NV item is TRUE.\n
FALSE -- The value of the hs_usb_disable_sleep_voting NV item is FALSE, 
         or the NV item could not be read.
*/
boolean hsu_conf_sel_disable_sleep_voting(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_hsic_core_enabled
=============================================================================*/
/**

This function returns whether the High-Speed Inter-Chip is enabled at runtime.

@dependencies
None.

@sideeffects
None.

@return
TRUE -- If the value of the hs_usb hsic_enabled NV item is TRUE, or no
        HS-USB OTG core is defined.\n
FALSE -- NV item could not be read or the HSIC core not enabled.
*/
boolean hsu_conf_sel_hsic_core_enabled (void);

/*=============================================================================
FUNCTION    hsu_conf_sel_hide_ftl
=============================================================================*/
/**

This function returns whether FTL should be hidden at runtime.

@dependencies
None.

@sideeffects
None.

@return
TRUE -- When value of the hs_usb hide_ftl NV/EFS item is TRUE
FALSE -- NV item could not be read or when hide_ftl is FALSE
*/
boolean hsu_conf_sel_hide_ftl (void);

/*=============================================================================
FUNCTION    hsu_config_selector_is_ahb2ahb_bypass_enabled
=============================================================================*/
/**

This function returns whether ahb2ahb bypass mode is enabled at runtime.

@dependencies
None.

@sideeffects
None.

@return
TRUE -- When value of the ahb2ahb bypass NV/EFS item is TRUE
FALSE -- NV item could not be read or when ahb2ahb bypass is FALSE
*/
boolean hsu_config_selector_is_ahb2ahb_bypass_enabled(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_hs_main_core_enabled
=============================================================================*/
/**

This function returns whether the HS-USB OTG core is enabled at runtime.

@dependencies
None.

@sideeffects
None.

@return
TRUE -- If the value of the hs_usb hsic_enabled NV item is FALSE, or no HSIC
        core is defined.\n
FALSE -- If the value of the hsic_enabled NV item is FALSE.

*/
boolean hsu_conf_sel_hs_main_core_enabled (void);


/*=============================================================================
FUNCTION    hsu_conf_sel_is_hsusb_initialized
=============================================================================*/
/**
Returns whether hsu_config_selector_init() completed.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- hsu_config_selector_init() completed.\n
FALSE -- hsu_config_selector_init() did not complete.
*/
boolean hsu_conf_sel_is_hsusb_initialized(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_update_bmp_composition
=============================================================================*/
/**
Defines the details of the BrewMP device composition and starts an
enumeration as a BrewMP device.

@param *device_info            [IN] -- Device information.
@param *configuration_info     [IN] -- Configuration information.
@param num_of_rdm_dev_in_comp  [IN] -- Number of RDM devices used.
@param rdm_devices             [IN] -- Array of RDM devices used.

@dependencies
The BrewMP composition must be active.

@sideeffects
None.

@return
TRUE  -- The composition was updated successfully.\n
FALSE -- The composition was not updated successfully.
*/
boolean hsu_conf_sel_update_bmp_composition(
  dev_info_t*           device_info,
  config_param_t*       configuration_info,
  uint8                 num_of_rdm_dev_in_comp,
  rdm_device_enum_type  rdm_devices[HSU_NUM_OF_RDM_USB_PORTS]
);

/*=============================================================================
FUNCTION    hsu_conf_sel_get_bmp_device_info
=============================================================================*/
/**
Retrieves the BrewMP device information as provided by the BrewMP layer upon
the call to hsu_config_selector_update_bmp_composition().

@dependencies
None.

@sideeffects
None.

@return
A pointer to brewmp_device_data_struct.
*/
hsu_conf_sel_brewmp_device_data_type* hsu_conf_sel_get_bmp_device_info(void);

/** @} */ /* end_group hsusb_qc_api */

/*=============================================================================
FUNCTION    hsu_conf_sel_get_iface_index_comp
=============================================================================*/

uint8 hsu_conf_sel_get_iface_index_comp(hsu_iface_index_enum_type);

hsu_iface_index_enum_type hsu_conf_sel_get_iface_type_from_instance_number(
  uint32 iface_instance_number, hsu_iface_index_enum_type start_type);

/** @} */ /* end_group hsu_conf_sel_get_iface_index_comp */

/*===========================================================================
FUNCTION          HSU_CONF_SEL_IS_DTD_MEM_LOGGING_ENABLED
==============================================================================*/

/**
This function returns TRUE when HSU_EFS_AGGRESSIVE_EP0_ERROR_CAPTURING_ITEM_FILE
bit field 2 is set.  Enable this to debug TD active bit related issues.

@dependencies
None.

@sideeffects
Transfer speed may be reduced as logging the TD structure requires CPU time

@return
TRUE means DTD and MEM logging will be enabled.  FALSE otherwise.

*/
boolean hsu_conf_sel_is_dtd_mem_logging_enabled(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_IS_CURRENT_COMP_UNDP

==============================================================================*/
boolean hsu_conf_sel_is_current_comp_undp(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_IS_RMNET_OVER_SECONDARY_CORE

==============================================================================*/
boolean hsu_conf_sel_is_rmnet_over_secondary_core(void);

/*===========================================================================
FUNCTION          HSU_CONF_SEL_GET_IFACE_TYPE_FROM_PORT_ID
==============================================================================*/
/**
This function returns interface type from given sio_port id.

@dependencies
None.

@sideeffects
None.

@return
USB interface type

*/
hsu_iface_index_enum_type hsu_conf_sel_get_iface_type_from_port_id
(
  sio_port_id_type port_id
);

#endif /* _HSU_CONF_SEL_I_H_ */

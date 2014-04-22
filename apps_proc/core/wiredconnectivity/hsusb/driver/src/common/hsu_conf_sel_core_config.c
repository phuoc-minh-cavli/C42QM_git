/*==============================================================================

          Config selector data for HS-USB Core configuraion

GENERAL DESCRIPTION
    Contains the config selector data for HS-USB core configuration.

EXTERNALIZED FUNCTIONS

INITALIZATION AND SEQUENCING REQUIREMENTS
    None

Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_conf_sel_core_config.c#1 $
  $DateTime: 2020/01/30 22:49:35 $


when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/17/08 me  Initial version

==============================================================================*/
/*==============================================================================
INCLUDE FILES FOR MODULE
==============================================================================*/
#include "customer.h"
#include "hsu_conf_sel_core_config.h"
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_al_ser.h"
#include "hsu_core_config.h"
#include "hsu_conf_sel_i.h"

#ifdef FEATURE_HS_USB_ECM
#include "hsu_al_ecm_int.h"
#endif /* FEATURE_HS_USB_ECM */

/*==============================================================================
LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
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
Variables
==============================================================================*/

/* USB3.0 USB core. */
#ifdef FEATURE_HS_USB_SNPS_CORE
static hsu_config_params_type hsu_usb3_config_params[] = 
{ 
  /* Configuration 0 data */
  {
    {0,0,0,0,0,0,0,0,0},              /* num_of_fd_instances - to be set later according to the chosen composition */
    "USB3.0 configuration 0",         /* config_name */
    NULL                              /* cur_dev_comp_details - to be set later according to the chosen composition */
  }
};

static hsu_device_params_type hsu_usb3_device_params =
{
    HSU_CONF_SEL_HIGH_SPEED_CORE,
    0,                                  /* product_id - to be set later according to the chosen composition */
    HSU_MAX_NUM_OF_CONFIGS,             /* num_of_configs */
    &hsu_usb3_config_params[0]
};
#endif /* FEATURE_HS_USB_SNPS_CORE */

/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
EXTERNAL FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/
/*===========================================================================
FUNCTION ()
===========================================================================*/
/*==============================================================================

FUNCTION          HSU_CONF_SEL_CORE_CONFIG_SET_CORE_INFO

DESCRIPTION
Set the pointers to the config selector info for all cores.

DEPENDENCIES
Should be called before hsu_conf_sel_stack_utils_setup_stack() was called. 

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_conf_sel_core_config_set_core_info(void)
{
#ifdef FEATURE_HS_USB_SNPS_CORE
  if(hsu_conf_sel_is_usb_core_snps())
  {
    hsu_core_config_set_usb3_core_conf_sel_info((void*)&hsu_usb3_device_params);
  }
#endif
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_CORE_CONFIG_SET_CORE_COMP

DESCRIPTION
Set the pointers to the composition info for a specific core.
In case the composition was read from NV, update with that composition. Else,
update with the composition according to the default PID.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
static void hsu_conf_sel_core_config_set_core_comp(const hsu_conf_sel_comp_info_type* nv_comp_details,
                                                   hsu_device_params_type dev_params)
{
  if (dev_params.product_id == 0)
  {
    dev_params.configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details = 
      nv_comp_details;
  }
  else
  {
    dev_params.configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details = 
      hsu_conf_sel_comp_utils_get_comp_details(nv_comp_details->efs_comp_id);
  }
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_CORE_CONFIG_SET_CORES_COMP

DESCRIPTION
Set the pointers to the composition info for all cores.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_conf_sel_core_config_set_cores_comp(const hsu_conf_sel_comp_info_type* nv_comp_details)
{
#ifdef FEATURE_HS_USB_SNPS_CORE
  if(hsu_conf_sel_is_usb_core_snps())
  {
    hsu_conf_sel_core_config_set_core_comp(nv_comp_details, hsu_usb3_device_params);
  }
#else
  #error "Compile time configuration invalid"
#endif
}

#ifndef FEATURE_HS_USB_SNPS_CORE
/*==============================================================================

FUNCTION          HSU_CONF_SEL_CORE_CONFIG_GET_HS_MAIN_CORE_CONF_SEL_INFO

DESCRIPTION
This function returns the config selector info of the HS-USB main core

DEPENDENCIES
None.

RETURN VALUE
The address of the config selector info of HS-USB main core.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_device_params_type* hsu_conf_sel_core_config_get_hs_main_core_conf_sel_info(void)
{
  return &hsu_usb3_device_params;
}
#endif

#ifdef FEATURE_HS_USB_SNPS_CORE
/*==============================================================================

FUNCTION          HSU_CONF_SEL_CORE_CONFIG_GET_USB3_CORE_CONF_SEL_INFO

DESCRIPTION
This function returns the config selector info of the USB3.0 core.

DEPENDENCIES
None.

RETURN VALUE
The address of the config selector info of the USB3.0 core.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_device_params_type* hsu_conf_sel_core_config_get_usb3_core_conf_sel_info(void)
{
  return &hsu_usb3_device_params;
}
#endif /* FEATURE_HS_USB_SNPS_CORE */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_CORE_CONFIG_GET_CORE_CONF_SEL_INFO

DESCRIPTION
This function returns the config selector info of a given core, according to
a given core index.

DEPENDENCIES
None.

RETURN VALUE
The address of the config selector info of the given core index.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_device_params_type* hsu_conf_sel_core_config_get_core_conf_sel_info(uint32 core_index)
{
  hsu_core_config_type* core =  hsu_core_config_get_core((uint8)core_index); 
  HSU_ASSERT(core);

  return core->hsu_conf_sel_core_info;
}


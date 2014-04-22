/*==============================================================================

                            HS-USB Core configuraion

GENERAL DESCRIPTION
    Contains the core configuration for HS-USB that is common for all targets.

EXTERNALIZED FUNCTIONS

INITALIZATION AND SEQUENCING REQUIREMENTS
    None

Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/core_config/hsu_core_config.c#1 $
  $DateTime: 2020/01/30 22:49:35 $


when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/17/08 me  Initial version

==============================================================================*/

/*==============================================================================
INCLUDE FILES FOR MODULE
==============================================================================*/
#include "hsu_core_config_int.h"
#include "hsu_conf_sel_i.h"

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
extern hsu_core_config_type* hsu_core_config_table[];


hsu_core_config_type cores_details[HSU_NUM_OF_CORES_TYPES] = 
{
  { /* High-Speed USB core. */
#ifdef FEATURE_HS_USB_OTG
    HSU_CORE_CONFIG_INIT_MODE_OTG,
#else
    HSU_CORE_CONFIG_INIT_MODE_DEVICE,
#endif /* FEATURE_HS_USB_OTG */
    NULL,
    NULL
  }
};


/*==============================================================================
EXTERNAL FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/
#ifndef T_REXNT
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_CORE

DESCRIPTION
This function returns the core according to the core index

DEPENDENCIES
None.

RETURN VALUE
A pointer to the core in the cores table.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_core_config_type* hsu_core_config_get_core(uint8 core_idx)
{
  if (core_idx >= hsu_core_config_get_cores_table_size())
  {
    return NULL;
  }

  return hsu_core_config_table[core_idx];
}

/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_CLEAR_CORE

DESCRIPTION
This function clears the core according to the core index

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_core_config_clear_core(uint8 core_idx)
{
  hsu_core_config_table[core_idx] = NULL;
}

/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_NUM_OF_CORES

DESCRIPTION
This .
The table will be function returns the number of active cores in the cores table 
(NOT including the NULL cores).

DEPENDENCIES
None.

RETURN VALUE
The number of active cores in the cores table.

SIDE EFFECTS
None. 

==============================================================================*/
uint8 hsu_core_config_get_num_of_cores(void)
{
  uint8 num_of_cores = 0;
  uint8 core_idx;
  uint8 cores_table_size = hsu_core_config_get_cores_table_size();

  for (core_idx = 0; core_idx < cores_table_size ; ++core_idx)
  {
    if (hsu_core_config_table[core_idx] != NULL)
    ++num_of_cores;
  }
  return num_of_cores;
}

#endif /* T_REXNT */

#if !defined (FEATURE_HS_USB_NO_HS_OTG_CORE)
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_HS_MAIN_CORE

DESCRIPTION
This function returns the HS-USB main core

DEPENDENCIES
None.

RETURN VALUE
The address of HS-USB main core.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_core_config_type* hsu_core_config_get_hs_main_core(void)
{
  return &cores_details[HSU_USB3_SNPS_CORE];
}
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_STACK_INIT_MODE

DESCRIPTION
Returns the mode in which the stack was configured at power up (device/OTG) for each core.

DEPENDENCIES
Should be called after hsu_conf_sel_stack_utils_setup_stack() was called. 

RETURN VALUE
HSU_CORE_CONFIG_INIT_MODE_OTG / HSU_CORE_CONFIG_INIT_MODE_DEVICE / HSU_CORE_CONFIG_INIT_MODE_HOST
HSU_CORE_CONFIG_INIT_MODE_LAST for error.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_core_config_init_mode_enum hsu_core_config_get_stack_init_mode(uint8 core_idx)
{
  hsu_core_config_type* core_config = hsu_core_config_get_core(core_idx);

  if (core_config == NULL)
  {
    HSU_MSG_ERROR_1("*_get_stack_init_mode((): hsu_core_config_table entry for core index %u is NULL",
      core_idx);
    return HSU_CORE_CONFIG_INIT_MODE_LAST;
  }

  return core_config->init_mode;
}

#ifdef FEATURE_HS_USB_SNPS_CORE
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_SET_HS_MAIN_CORE_CONF_SEL_INFO

DESCRIPTION
Set the pointer to the config selector info for HS-USB main core.

DEPENDENCIES
Should be called before hsu_conf_sel_stack_utils_setup_stack() was called. 

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_core_config_set_usb3_core_conf_sel_info(void* conf_sel_info)
{
  cores_details[HSU_USB3_SNPS_CORE].hsu_conf_sel_core_info = conf_sel_info;
}
#endif /* FEATURE_HS_USB_SNPS_CORE */

/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_PRIMARY_CORE_IDX

DESCRIPTION
This function returns the HAL core index of the core that is currently active.

DEPENDENCIES
None.

RETURN VALUE
The primary core index.

SIDE EFFECTS
None. 

==============================================================================*/
uint8 hsu_core_config_get_primary_core_idx(void)
{
    return HSU_CORE_CONFIG_USB_IDX;
}

/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_SECONDARY_CORE_IDX

DESCRIPTION
This function returns the HAL core index of the secondary core.

DEPENDENCIES
None.

RETURN VALUE
The secondary core index.

SIDE EFFECTS
None. 

==============================================================================*/
uint8 hsu_core_config_get_secondary_core_idx(void)
{
  return HSU_CORE_CONFIG_USB_IDX;
}


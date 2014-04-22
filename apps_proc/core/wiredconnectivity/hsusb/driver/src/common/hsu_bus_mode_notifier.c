/*==============================================================================

    High Speed USB suspend/resume notifier
  
  GENERAL DESCRIPTION
    This module defines suspend resume notifier. It allows registering for
    suspend and resume events from the USB stack

==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_bus_mode_notifier.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================    
      
  EXTERNALIZED FUNCTIONS
    hsu_register_bus_mode_notif_cb
    hsu_unregister_bus_mode_notif_cb
    hsu_bus_mode_changed

  INITALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_bus_mode_notifier.h"
#include "comdef.h"
#include "hsu_common_int.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_core_config_int.h"

/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
/*
 * The maximal number of suspend/resume callbacks that can be 
 * registered 
 */
#define HSU_BUS_MODE_MAX_REGISTERED_CB 10 * HSU_MAX_NUM_OF_CORES

/*==============================================================================
  Variables
==============================================================================*/
/*
 * The current suspend/resume state of the USB stack 
 */
static hsu_bus_mode_enum hsu_current_bus_mode[HSU_MAX_NUM_OF_CORES] = 
{
  HSU_BUS_MODE_SUSPENDED,     // HSU_CORE_CONFIG_HS_CI_IDX
};

/*
 * registered callbacks
 */
static hsu_bus_mode_cb_info_struct 
  hsu_bus_mode_registered_cb_array[HSU_BUS_MODE_MAX_REGISTERED_CB] = {{NULL}};
static hsu_bus_mode_multi_core_cb_info_struct 
  hsu_bus_mode_multi_core_registered_cb_array[HSU_BUS_MODE_MAX_REGISTERED_CB] = {{NULL}};

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================

FUNCTION          HSU_BUS_MODE_CHANGED

DESCRIPTION
  This function should be called when a change in bus mode happens (either bus
  was suspended or resumed).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_bus_mode_changed
(
  uint32 core_index,
  hsu_bus_mode_enum new_mode
)
{
  uint8 i;

  if(hsu_current_bus_mode[core_index] == new_mode)
  {
    HSU_MSG_MED_2("hsu_bus_mode_changed(): state %d not changed, core_idx %d (0=S,1=R)", 
      new_mode, core_index);
    return;
  }

  HSU_MSG_HIGH_2("hsu_bus_mode_changed(): new mode = %d, core_idx %d(0=S,1=R)", 
    new_mode, core_index);
  hsu_current_bus_mode[core_index] = new_mode;

  /* Need to notify registered callbacks of the change */

  /* Legacy single core implementation */
  /*  unable to derive core_index information for legacy callback, resort to always notify callback */
  for(i = 0 ; i < HSU_BUS_MODE_MAX_REGISTERED_CB ; ++i)
  {
    if(hsu_bus_mode_registered_cb_array[i].bus_mode_cb != NULL)
    {
      hsu_bus_mode_registered_cb_array[i].bus_mode_cb(new_mode, hsu_bus_mode_registered_cb_array[i].app_data);
    }
  }

  /* Multi-core implementation */
  for(i = 0 ; i < HSU_BUS_MODE_MAX_REGISTERED_CB ; ++i)
  {
    if((hsu_bus_mode_multi_core_registered_cb_array[i].core_index == core_index) &&
       (hsu_bus_mode_multi_core_registered_cb_array[i].bus_mode_cb != NULL))
    {
      hsu_bus_mode_multi_core_registered_cb_array[i].bus_mode_cb(new_mode, hsu_bus_mode_multi_core_registered_cb_array[i].app_data);
    }
  }
} /* hsu_bus_mode_changed */

/*===========================================================================

FUNCTION      HSU_REGISTER_BUS_MODE_NOTIF_CB

DESCRIPTION
  Register a callback function to be invoked when the suspend/resume state of 
  the HSU stack is changed.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE for success.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hsu_register_bus_mode_notif_cb
(
  hsu_bus_mode_cb_info_struct cb_info
)
{
  uint8 i;

  for(i = 0 ; i < HSU_BUS_MODE_MAX_REGISTERED_CB ; ++i)
  {
    if ((hsu_bus_mode_registered_cb_array[i].bus_mode_cb == NULL) &&
      (hsu_bus_mode_registered_cb_array[i].app_data == NULL))
    {
      break; /* we found an available entry */
    }
  }

  if (i == HSU_BUS_MODE_MAX_REGISTERED_CB)
  {
    return FALSE;  
  }

  hsu_bus_mode_registered_cb_array[i].bus_mode_cb = cb_info.bus_mode_cb;
  hsu_bus_mode_registered_cb_array[i].app_data = cb_info.app_data;

  return TRUE;
} /* hsu_register_bus_mode_notif_cb */


/*===========================================================================

FUNCTION      HSU_REGISTER_BUS_MODE_MULTI_CORE_NOTIF_CB

DESCRIPTION
  Register a callback function to be invoked when the suspend/resume state of 
  the HSU stack is changed. 
  Applicable for function drivers that supports multi-core.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE for success.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hsu_register_bus_mode_multi_core_notif_cb
(
  hsu_bus_mode_multi_core_cb_info_struct cb_info
)
{
  uint8 i;

  for(i = 0 ; i < HSU_BUS_MODE_MAX_REGISTERED_CB ; ++i)
  {
    if ((hsu_bus_mode_multi_core_registered_cb_array[i].bus_mode_cb == NULL) &&
      (hsu_bus_mode_multi_core_registered_cb_array[i].app_data == NULL))
    {
      break; /* we found an available entry */
    }
  }

  if (i == HSU_BUS_MODE_MAX_REGISTERED_CB)
  {
    return FALSE;  
  }

  hsu_bus_mode_multi_core_registered_cb_array[i].core_index = cb_info.core_index;
  hsu_bus_mode_multi_core_registered_cb_array[i].bus_mode_cb = cb_info.bus_mode_cb;
  hsu_bus_mode_multi_core_registered_cb_array[i].app_data = cb_info.app_data;

  return TRUE;
} /* hsu_register_bus_mode_multi_core_notif_cb */

/*===========================================================================

FUNCTION      HSU_UNREGISTER_BUS_MODE_NOTIF_CB

DESCRIPTION
  Unregister a suspend/resume callback function.

DEPENDENCIES
  hsu_register_bus_mode_notif_cb must have been called with the same 
  callback function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hsu_unregister_bus_mode_notif_cb
(
  hsu_bus_mode_cb_info_struct cb_info
)
{
  uint8 i;

  for(i = 0 ; i < HSU_BUS_MODE_MAX_REGISTERED_CB ; ++i)
  {
    if ((hsu_bus_mode_registered_cb_array[i].bus_mode_cb == cb_info.bus_mode_cb) &&
        (hsu_bus_mode_registered_cb_array[i].app_data == cb_info.app_data))
    {
      hsu_bus_mode_registered_cb_array[i].bus_mode_cb = NULL;
      hsu_bus_mode_registered_cb_array[i].app_data = NULL;
      break; 
    }
  }
} /* hsu_unregister_bus_mode_notif_cb */


/*===========================================================================

FUNCTION      HSU_UNREGISTER_BUS_MODE_MULTI_CORE_NOTIF_CB

DESCRIPTION
  Unregister a suspend/resume callback function.

DEPENDENCIES
  hsu_register_bus_mode_multi_core_notif_cb must have been called with the same 
  callback function. Applicable for function drivers that support multi-core.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hsu_unregister_bus_mode_multi_core_notif_cb
(
  hsu_bus_mode_multi_core_cb_info_struct cb_info
)
{
  uint8 i;

  for(i = 0 ; i < HSU_BUS_MODE_MAX_REGISTERED_CB ; ++i)
  {
    if ((hsu_bus_mode_multi_core_registered_cb_array[i].core_index == cb_info.core_index) &&
        (hsu_bus_mode_multi_core_registered_cb_array[i].bus_mode_cb == cb_info.bus_mode_cb) &&
        (hsu_bus_mode_multi_core_registered_cb_array[i].app_data == cb_info.app_data))
    {
      hsu_bus_mode_multi_core_registered_cb_array[i].core_index = 0;
      hsu_bus_mode_multi_core_registered_cb_array[i].bus_mode_cb = NULL;
      hsu_bus_mode_multi_core_registered_cb_array[i].app_data = NULL;
      break; 
    }
  }
} /* hsu_unregister_bus_mode_multi_core_notif_cb */


/*===========================================================================

FUNCTION      HSU_GET_CURRENT_BUS_MODE

DESCRIPTION
  Returns current bus mode.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  Returns current bus mode.

SIDE EFFECTS
  None.

===========================================================================*/
hsu_bus_mode_enum hsu_get_current_bus_mode
(
  uint32 hal_core_index
)
{
  HSU_ASSERT(hal_core_index < HSU_MAX_NUM_OF_CORES);

  return hsu_current_bus_mode[hal_core_index];
}


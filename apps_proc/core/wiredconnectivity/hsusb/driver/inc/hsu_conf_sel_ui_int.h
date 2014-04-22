#ifndef _HSU_CONF_SEL_UI_INT_H_
#define _HSU_CONF_SEL_UI_INT_H_

/*==============================================================================

     High Speed USB configuration Selector UI related
        H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This file includes functions that are related to High Speed USB composition 
    changes through the UI.

  EXTERNALIZED FUNCTIONS

  INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_conf_sel_ui_comp_menu_init should be called during power up, before the 
    user can perform any UI actions.
    
  Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_conf_sel_ui_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  12/04/06 ke  Initial version
    
==============================================================================*/
#include "hsu_conf_sel_ui.h"
#include "hsu_conf_sel_types_int.h"

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
typedef struct 
{
  char *                            item_string; 
    /* Must be exactly 12 chars long */
  
  /* The HSU composition id representing this composition */
  hsu_conf_sel_composition_id_enum  hsu_comp_id; 
} hsu_conf_sel_ui_composition_item_type;


/*---------------------------------------------------------------------------
 * Structure definitions for High Speed USB Composition UI MENU 
 *-------------------------------------------------------------------------*/
typedef struct
{
  char *  heading;       
    /* Must be exactly 12 characters long, 
     * For example: "DIAG,MS     "         */
  uint8   current_index; 
    /* index of current value 0..0xFF
     * "0xFF" indicates current_index is unknown. 
     * REAL item indexes range from zero to (HSU_MAX_UI_ITEMS - 1).  */
  uint8   num_items;     
    /* length of the items array below (MAX of HSU_MAX_UI_ITEMS */
  hsu_conf_sel_ui_composition_item_type * items[HSU_MAX_UI_ITEMS]; 
} hsu_conf_sel_ui_composition_menu_type;

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

#ifndef FEATURE_HSU_TEST
/*===========================================================================

FUNCTION              HSU_CONF_SEL_UI_SWITCH_COMP

DESCRIPTION
  Called when composition change should be performed.
  Start the process of dynamically changing the current USB configuration. 
  Does so by setting internal variables, and starting the stage of closing 
  application that are using a USB port. 

DEPENDENCIES
  Must be called after stack initialization. 

RETURN VALUE
  TRUE for success (only indicates first step was successful), FALSE for failure.

SIDE EFFECTS
  None

===========================================================================*/
boolean hsu_conf_sel_ui_switch_comp
(
  hsu_conf_sel_composition_id_enum  new_usb_comp_id,
  rdm_assign_port_cb_type           rdm_comp_cb_func_ptr
);
#endif /* FEATURE_HSU_TEST */


#endif /* _HSU_CONF_SEL_UI_H_ */

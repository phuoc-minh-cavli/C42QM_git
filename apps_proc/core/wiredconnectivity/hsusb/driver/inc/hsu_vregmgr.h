/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_vregmgr.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------

    
==============================================================================*/


#ifndef HSU_VREGMGR_H
#define HSU_VREGMGR_H
/*===========================================================================

                     HS USB VREG MANAGER
                     
DESCRIPTION
  This file contains functions and data structure declarations to control 
  VREG for HS USB. 

REFERENCES

  Copyright (c) 2009 Qualcomm Technologies Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*==============================================================================
                   TYPE DEFINITIONS
==============================================================================*/
typedef enum {
  HSU_VREG_ID_HSUSB = 0,
  HSU_VREG_ID_SSUSB = 1,
} hsu_vreg_id_type;

/*==============================================================================
                    FUNCTION DECLARATIONS
==============================================================================*/
/*===========================================================================
FUNCTION hsu_vregmgr_turn_on

DESCRIPTION
  Turn on vreg.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_vregmgr_turn_on(hsu_vreg_id_type vreg_id);

/*===========================================================================
FUNCTION hsu_vregmgr_turn_off

DESCRIPTION
  Turn off vreg.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_vregmgr_turn_off(hsu_vreg_id_type vreg_id);

/*===========================================================================
FUNCTION hsu_vregmgr_init

DESCRIPTION
  initialize VREGMGR.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_vregmgr_init(void);

/*===========================================================================
FUNCTION hsu_vregmgr_hsic_init

DESCRIPTION
  initialize VREGMGR for HSIC.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_vregmgr_hsic_init(void);

/*===========================================================================
FUNCTION hsu_vregmgr_is_usb_cable_attached

DESCRIPTION
  Returns status of USB cable
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - USB cable attached
  FALSE - USB cable dettached

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_vregmgr_is_usb_cable_attached(void);

/*===========================================================================
FUNCTION hsu_vregmgr_pmic_vbus_register

DESCRIPTION
  Register for PMIC GPIO for USB VBUS status

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_vregmgr_pmic_vbus_register(void);

/*===========================================================================
FUNCTION hsusb_vregmgr_vbus_callback

DESCRIPTION
  Callback function for connect/disconnect

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsusb_vregmgr_vbus_callback(boolean is_high);

#endif /* HSU_VREGMGR_H */

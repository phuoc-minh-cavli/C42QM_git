#ifndef HSU_CLKMGR_H
#define HSU_CLKMGR_H
/*==============================================================================

  USB and HSIC Clock Management
  
  GENERAL DESCRIPTION
    This module defines APIs for USB and HSIC clock management.

  INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_clkmgr_init and hsu_clkmgr_hsic should be call prior to calling
    any other APIs.

  Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_clkmgr.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  11/02/13  jc    Initial version

==============================================================================    

  INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_clkmgr_init and hsu_clkmgr_hsic should be call prior to calling
    any other APIs.

  Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

//==============================================================================
// Include Files for Module
//==============================================================================
#include "comdef.h"
#include "hsu_core_config.h"

//==============================================================================
// Typedefs
//==============================================================================

// Clock speed level definition
typedef enum {
  HSU_CLKMGR_SPEED_LOW = 0,
  HSU_CLKMGR_SPEED_MID,
  HSU_CLKMGR_SPEED_HIGH
} hsu_clkmgr_speed_level_type;

// Clock manager states
typedef enum {
  HSU_CLKMGR__CLK_OFF = 0,
  HSU_CLKMGR__CLK_ON,
} hsu_clkmgr_state;

// GCC Reset Type
typedef enum {
  HSU_CLKMGR__RESET_GCC_USB2_M,                /* Combined USB clock regimes, this resets the Link Only */
  HSU_CLKMGR__RESET_GCC_USB2_PHY_RESET_M,      /* USB PHY RESET regime, this resets the PHY, Wrapper and ULPI bridge */
  HSU_CLKMGR__RESET_GCC_USB2_PHY_POR_RESET_M,   /* This resets the PHY only, typical usage is to do this after changing PHY tune params */ 

  // GCC_USB3_PHY_BCR
  HSU_CLKMGR__RESET_GCC_USB3_PHY_BCR,

  // GCC_USB3PHY_PHY_BCR - can be used with 'gcc_usb3_aux_clk' and 'gcc_usb3_pipe_clk'
  HSU_CLKMGR__RESET_GCC_USB3PHY_PHY_BCR,

} hsu_clkmgr_reset_clock_type;

//==============================================================================
// Global Function Definitions
//==============================================================================

/*=============================================================================
FUNCTION    hsu_clkmgr_init
=============================================================================*/
/** 
The function initializes internal structures for the USB clocks

@param None

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_clkmgr_init(void);

/*=============================================================================
FUNCTION    hsu_clkmgr_config_clocks
=============================================================================*/
/** 
Turns ON USB clocks

@param core_type    [IN]  -- CI USB or SNPS USB.

@dependencies
hsu_clkmgr_init().

@sideeffects
None.

@return
None.
*/
void hsu_clkmgr_config_clocks(hsu_core_type_enum core_type);

/*=============================================================================
FUNCTION    hsu_clkmgr_config_npa_clocks
=============================================================================*/
/** 
Turns ON USB bus clocks that requires NPA call which could take several
milliseconds to complete

@param core_type    [IN]  -- CI USB or SNPS USB.

@dependencies
hsu_clkmgr_init().

@sideeffects
None.

@return
None.
*/
void hsu_clkmgr_config_npa_clocks(hsu_core_type_enum core_type);

/*=============================================================================
FUNCTION    hsu_clkmgr_release_clocks
=============================================================================*/
/** 
Turns OFF USB clocks

@param core_type    [IN]  -- CI USB or SNPS USB.

@dependencies
hsu_clkmgr_init() 

@sideeffects
None.

@return
None.
*/
void hsu_clkmgr_release_clocks(hsu_core_type_enum core_type);

/*=============================================================================
FUNCTION    hsu_clkmgr_release_npa_clocks
=============================================================================*/
/** 
Turns OFF USB bus clocks that requires NPA call to turn OFF

@param core_type    [IN]  -- CI USB or SNPS USB.

@dependencies
hsu_clkmgr_init() 

@sideeffects
None.

@return
None.
*/
void hsu_clkmgr_release_npa_clocks(hsu_core_type_enum core_type);

/*=============================================================================
FUNCTION    hsu_clkmgr_hsic_init
=============================================================================*/
/** 
The function initializes internal structures for the HSIC clocks

@param None

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_clkmgr_hsic_init(void);

/*=============================================================================
FUNCTION    hsu_clkmgr_hsic_config_clocks
=============================================================================*/
/** 
Turns ON HSIC clocks

@param None

@dependencies
hsu_clkmgr_hsic_init().

@sideeffects
None.

@return
None.
*/
void hsu_clkmgr_hsic_config_clocks(void);

/*=============================================================================
FUNCTION    hsu_clkmgr_hsic_release_clocks
=============================================================================*/
/** 
Turns OFF HSIC clocks

@param None

@dependencies
hsu_clkmgr_hsic_init().

@sideeffects
None.

@return
None.
*/
void hsu_clkmgr_hsic_release_clocks(void);

/*=============================================================================
FUNCTION    hsu_clkmgr_enable_power_domain
=============================================================================*/
/** 
The function turns ON or OFF VDD Power Domain for the core

@param enable       [IN]  -- TRUE = turn VDD power domain ON -- FALSE = OFF
@param core_type    [IN]  -- USB or HSIC.

@dependencies
hsu_clkmgr_init() and hsu_clkmgr_hsic_init().

@sideeffects
None.

@return
None.
*/
void hsu_clkmgr_enable_power_domain(boolean enable, hsu_core_type_enum core_type);

/*=============================================================================
FUNCTION    hsu_clkmgr_assert_reset
=============================================================================*/
/** 
Assert and deassert GCC clock resets

@param set_assert       [IN]  -- TRUE = ASSERT -- FALSE = DEASSERT
@param clock_type       [IN]  -- GCC clock reset type id

@dependencies
hsu_clkmgr_init() and hsu_clkmgr_hsic_init().

@sideeffects
None.

@return
None.
*/
void hsu_clkmgr_assert_reset(boolean set_assert, hsu_clkmgr_reset_clock_type clock_type);

/*=============================================================================
FUNCTION    hsu_clkmgr_is_clock_on
=============================================================================*/
/** 
The function return whether the clocks are ON for the USB or HSIC core

@param core_type  [IN]  -- USB or HSIC.

@dependencies
hsu_clkmgr_init() and hsu_clkmgr_hsic_init().

@sideeffects
None.

@return
TRUE  - clock ON
FALSE - clock OFF
*/
boolean hsu_clkmgr_is_clock_on(hsu_core_type_enum core_type);

/*=============================================================================
FUNCTION    hsu_clkmgr_is_peripheral_clock_on
=============================================================================*/
/** 
Returns boolean if the USB peripheral clock to access the USB LINK HW 
register is accessible.

@param core_type  [IN]  -- USB or HSIC.

@dependencies
hsu_clkmgr_init() and hsu_clkmgr_hsic_init().

@sideeffects
None.

@return
TRUE  - if clock to access link hw register is enabled.
FALSE - otherwise.
*/
boolean hsu_clkmgr_is_peripheral_clock_on(hsu_core_type_enum core_type);

/*=============================================================================
FUNCTION    hsu_clkmgr_enable_ahb2ahb_bridge_bypass_mode
=============================================================================*/
/** 
Enables the Bypass Mode for the AHB2AHB bridge which synchronizes USB/HSIC 
AHB clock and SYSTEM clock.
When this setting is set it has following effect:

* SYSTEM clock will be no longer used -
    Calling this function will turn system clock OFF when if not already OFF

* AHB clock will be used to run protocol engine, replacing System clock's
    functionality.

* DMA performance may increase by 5% marging on maximum throughput scenario 
    as the bridge is bypassed.

* AHB clock cannot fall below 50Mhz.

@param core_type  [IN]  -- USB or HSIC.

@dependencies
hsu_clkmgr_init() and hsu_clkmgr_hsic_init().

@sideeffects
AHB2AHB Bridge Bypass Mode is disabled.
USB/HSIC System clock will be turned ON if not already ON

@return
None.
*/
void hsu_clkmgr_enable_ahb2ahb_bridge_bypass_mode(hsu_core_type_enum core_type);

/*=============================================================================
FUNCTION    hsu_clkmgr_disable_ahb2ahb_bridge_bypass_mode
=============================================================================*/
/** 
Disable the Bypass Mode for the AHB2AHB bridge which synchronizes USB/HSIC 
AHB clock and SYSTEM clock.
When this setting is set it has following effect:

* Calling this function will turn system clock ON when if not already ON

@param core_type  [IN]  -- USB or HSIC.

@dependencies
hsu_clkmgr_init() and hsu_clkmgr_hsic_init().

@sideeffects
AHB2AHB Bridge Bypass Mode is disabled.
USB/HSIC System clock will be turned ON if not already ON

@return
None.
*/
void hsu_clkmgr_disable_ahb2ahb_bridge_bypass_mode(hsu_core_type_enum core_type);


//==============================================================================
// FUNCTION hsu_clkmgr_select_utmi_clk
//==============================================================================
/** 
This is used for configuring the core to UTMI clock instead of pipe clock.
This needs to be called when there is no SS USB PHY.

@param None
  
@dependencies
None

@sideeffects
None. 
  
@return
None.
*/
void hsu_clkmgr_select_utmi_clk(void);


//==============================================================================
// FUNCTION hsu_clkmgr_is_hs_only_mode
//==============================================================================
/** 
Function to query whether High Speed only mode or UTMI path mode is selected

@param None
  
@dependencies
None

@sideeffects
None. 
  
@return
TRUE - System is operating at high speed only mode
FALSE - Otherwise
*/
boolean hsu_clkmgr_is_hs_only_mode(void);

//==============================================================================
// FUNCTION hsu_clkmgr_is_usb3pipe_clk_on
//==============================================================================
/** 
Function to query whehter pipe clock is functional

@param None
  
@dependencies
None

@sideeffects
None. 
  
@return
TRUE - Pipe clock is generated by the PHY
FALSE - Otherwise
*/
boolean hsu_clkmgr_is_usb3pipe_clk_on(void);
#endif


/*
===========================================================================

FILE:         HAL_hsusb_impl_p3q2r13.c

DESCRIPTION:  
This is the HAL implementation for the HS-USB core usb_otg_hs_xp3q2r13.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/hal/src/HAL_hsusb_impl_p3q2r13.c#1 $

===========================================================================

===========================================================================
Copyright © 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HAL_hsusb_impl_p3q2r13.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_hsusb_GetAHBBufferableDataAccessImplp3q2r13
**
**  Description:
**    This function reports whether the AHB Bufferable data access is  enabled
**    for the HS-USB core usb_otg_hs_p3q2r13 implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 6 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The AHB Bufferable data access is  enabled
**    FALSE   - The AHB Bufferable data access is disabled
**
** ======================================================================== */
static boolean HAL_hsusb_GetAHBBufferableDataAccessImplp3q2r13(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_DMA_HPROT_CTRL_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetAHBBufferableDataAccessImplp3q2r13
**
**  Description:
**    This function enables/disables the AHB Bufferable data access
**    for the HS-USB core usb_otg_hs_p3q2r13 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 6 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the TX streaming automatic determination mode.
**             Values are:
**             TRUE  - Enable the AHB Bufferable data access
**             FALSE - Disable the AHB Bufferable data access
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetAHBBufferableDataAccessImplp3q2r13(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_DMA_HPROT_CTRL_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_DMA_HPROT_CTRL_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_GetLegacyModeImplp3q2r13
**
**  Description:
**    This function reports whether the Legacy mode is enabled
**    for the HS-USB core usb_otg_hs_p3q2r13 implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 6 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The Legacy mode is  enabled
**    FALSE   - The Legacy mode is disabled
**
** ======================================================================== */
static boolean HAL_hsusb_GetLegacyModeImplp3q2r13(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_USB_BAM_DISABLE_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetLegacyModeImplp3q2r13
**
**  Description:
**    This function sets the Legacy mode
**    for the HS-USB core usb_otg_hs_p3q2r13 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 6 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the TX streaming automatic determination mode.
**             Values are:
**             TRUE  - Enable the Legacy mode
**             FALSE - Disable the Legacy mode
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetLegacyModeImplp3q2r13(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_USB_BAM_DISABLE_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_USB_BAM_DISABLE_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_GetTestBusSelectorImplp3q2r13
**
**  Description:
**    This function reports which group of the internal signals is 
**    routed to the test bus for the HS-USB core usb_otg_hs_p3q2r13 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 6 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    The group of the internal signals that is routed to the test bus.
**
** ======================================================================== */
static HAL_HSUSB_TestBusSelectorType HAL_hsusb_GetTestBusSelectorImplp3q2r13(uint8 instance)
{
  uint32 data = HAL_hsusb_Read32(instance, HAL_HSUSB_GEN_CONFIG_ADDR);
  return (HAL_HSUSB_TestBusSelectorType)(
    ((data&HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_BMSK)>>HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_SHFT) |
    ((data&HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_4_0_BMSK)>>HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_4_0_SHFT)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_SetTestBusSelectorImplp3q2r13
**
**  Description:
**    This function sets which group of the internal signals is 
**    routed to the test bus for the HS-USB core usb_otg_hs_p3q2r13 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 6 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    eMode    - Determines which group of the internal signals is routed 
**             to the test bus.
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetTestBusSelectorImplp3q2r13(uint8 instance, HAL_HSUSB_TestBusSelectorType eMode)
{
  HAL_hsusb_ClearAndSetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, 
    HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_BMSK | HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_4_0_BMSK, 
    ((eMode<<HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_SHFT)&HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_BMSK) |
    ((eMode<<HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_4_0_SHFT)&HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_4_0_BMSK));
}

/* ===========================================================================
**
**  HAL_hsusb_GetEndpointPipeIDImplp3q2r13
**
**  Description:
**    This function reports the endpoint pipe number
**    for the HS-USB core usb_otg_hs_p3q2r13 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 6 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    ep       - Determines the endpoint number.
**             Values are: 1-15.
**    direction - Determines the endpoint direction.
**             Values are:
**             TRUE  - IN endpoint
**             FALSE - OUT endpoint
**
**  Return:
**     The endpoint pipe number.
**     Values are: 0-0x1F.
**
** ======================================================================== */
uint8 HAL_hsusb_GetEndpointPipeIDImplp3q2r13(uint8 instance, uint8 ep, boolean direction)
{
  if ((ep < 1) || (ep>15))
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
    return 0x1F;
  }
  return (uint8)(direction ?
    HAL_hsusb_GetBits32(instance, HAL_HSUSB_ENDPT_PIPE_ID_ADDR(ep), HAL_HSUSB_ENDPT_PIPE_ID_TX_BMSK) >> HAL_HSUSB_ENDPT_PIPE_ID_TX_SHFT :
    HAL_hsusb_GetBits32(instance, HAL_HSUSB_ENDPT_PIPE_ID_ADDR(ep), HAL_HSUSB_ENDPT_PIPE_ID_RX_BMSK) >> HAL_HSUSB_ENDPT_PIPE_ID_RX_SHFT);
}

/* ===========================================================================
**
**  HAL_hsusb_SetEndpointPipeIDImplp3q2r13
**
**  Description:
**    This function sets the endpoint pipe number
**    for the HS-USB core usb_otg_hs_p3q2r13 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 6 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    ep       - Determines the endpoint number.
**             Values are: 1-15.
**    direction - Determines the endpoint direction.
**             Values are:
**             TRUE  - IN endpoint
**             FALSE - OUT endpoint
**    id       - Determines the endpoint pipe number.
**             Values are: 0-0x1F.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetEndpointPipeIDImplp3q2r13(uint8 instance, uint8 ep, boolean direction, uint8 id)
{
  if ((ep < 1) || (ep>15))
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
    return;
  }
  (void)(direction ?
    HAL_hsusb_ClearAndSetBits32(instance, HAL_HSUSB_ENDPT_PIPE_ID_ADDR(ep), HAL_HSUSB_ENDPT_PIPE_ID_TX_BMSK, id << HAL_HSUSB_ENDPT_PIPE_ID_TX_SHFT) :
    HAL_hsusb_ClearAndSetBits32(instance, HAL_HSUSB_ENDPT_PIPE_ID_ADDR(ep), HAL_HSUSB_ENDPT_PIPE_ID_RX_BMSK, id << HAL_HSUSB_ENDPT_PIPE_ID_RX_SHFT));
}

/* ===========================================================================
**
**  HAL_hsusb_ConstructImplp3q2r13
**
**  Description:
**    This function initializes the HS-USB implementation object for 
**    the  HS-USB core usb_otg_hs_xp3q2r13 implementation.
**    
**  Dependencies:
**    None.
**
**  Parameters:
**    this     - Points to the HS-USB core interface object.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_ConstructImplp3q2r13(struct HAL_HSUSB_CoreIfType* this)
{
  HAL_hsusb_ConstructImpl3r0(this);
  this->Version = 6;
  this->GetAHBBufferableDataAccess = HAL_hsusb_GetAHBBufferableDataAccessImplp3q2r13;
  this->SetAHBBufferableDataAccess = HAL_hsusb_SetAHBBufferableDataAccessImplp3q2r13;
  this->GetLegacyMode = HAL_hsusb_GetLegacyModeImplp3q2r13;
  this->SetLegacyMode = HAL_hsusb_SetLegacyModeImplp3q2r13;
  this->GetTestBusSelector = HAL_hsusb_GetTestBusSelectorImplp3q2r13;
  this->SetTestBusSelector = HAL_hsusb_SetTestBusSelectorImplp3q2r13;
  this->GetEndpointPipeID = HAL_hsusb_GetEndpointPipeIDImplp3q2r13;
  this->SetEndpointPipeID = HAL_hsusb_SetEndpointPipeIDImplp3q2r13;
}

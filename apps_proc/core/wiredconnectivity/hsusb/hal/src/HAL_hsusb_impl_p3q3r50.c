/*
===========================================================================

FILE:         HAL_hsusb_impl_p3q3r50.c

DESCRIPTION:  
This is the HAL implementation for the HS-USB core usb_otg_hs_xp3q3r50.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/hal/src/HAL_hsusb_impl_p3q3r50.c#1 $

===========================================================================

===========================================================================
Copyright © 2009-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HAL_hsusb_impl_p3q3r50.h"

/* -----------------------------------------------------------------------
**                           CONSTANTS
** ----------------------------------------------------------------------- */
  static const uint32  max_endpoints             = 16;
  static const uint8   min_remote_wakeup_time_us = 50;
  static const uint8   max_remote_wakeup_time_us = 65;

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/*=============================================================================
FUNCTION:      HAL_hsusb_GetTxEpPrimeL1ExitImplp3q3r50
=============================================================================*/
/**
Reports whether remote wakeup in L1 state when SW starts priming specific
endpoint is enabled.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param ep       [IN] -- The endpoint number

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Remote wakeup when SW starts priming specific endpoint is enabled
FALSE -- Remote wakeup when SW starts priming specific endpoint is disabled
*/
static boolean HAL_hsusb_GetTxEpPrimeL1ExitImplp3q3r50(uint8 instance, uint8 ep)
{
  uint32 bitmask;

  if (ep >= max_endpoints)
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
    return FALSE;
  }

  bitmask = HAL_hsusb_GetBits32(instance, 
                                HAL_HSUSB_L1_EP_CTRL_ADDR,
                                HAL_HSUSB_L1_EP_CTRL_TX_EP_PRIME_L1_EXIT_BMSK);

  bitmask &= (0x1 << (ep + HAL_HSUSB_L1_EP_CTRL_TX_EP_PRIME_L1_EXIT_SHFT));

  return (bitmask != 0);
}

/*=============================================================================
FUNCTION:      HAL_hsusb_SetTxEpPrimeL1ExitImplp3q3r50
=============================================================================*/
/**
Sets whether remote wakeup in L1 state when SW starts priming specific
endpoint is enabled.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param ep       [IN] -- The endpoint number
@param state    [IN] -- TRUE enables remote wakeup as SW starts priming specific endpoint. 
                        FALSE disables remote wakeup as SW starts priming specific endpoint.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_SetTxEpPrimeL1ExitImplp3q3r50(uint8 instance, uint8 ep, boolean state)
{
  uint32 bitmask;

  if (ep >= max_endpoints)
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
    return;
  }

  bitmask = 0x1 << (HAL_HSUSB_L1_EP_CTRL_TX_EP_PRIME_L1_EXIT_SHFT + ep);

  if (state == TRUE)
  {
    HAL_hsusb_SetBits32(instance, 
                        HAL_HSUSB_L1_EP_CTRL_ADDR,
                        bitmask);
  }
  else
  {
    HAL_hsusb_ClearBits32(instance, 
                          HAL_HSUSB_L1_EP_CTRL_ADDR,
                          bitmask);   
  }
}

/*=============================================================================
FUNCTION:      HAL_hsusb_GetTxEpPrimeL1EnableImplp3q3r50
=============================================================================*/
/**
Reports whether the transition to L1 state can occur when a specific endpoint
is primed

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param ep       [IN] -- The endpoint number

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Refrain from transitioning to L1 state whenever the particular TX
         endpoint is primed
FALSE -- Transition to L1 state is allowed even if the particular TX endpoint
         is primed
*/
static boolean HAL_hsusb_GetTxEpPrimeL1EnableImplp3q3r50(uint8 instance, uint8 ep)
{
  uint32 bitmask;

  if (ep >= max_endpoints)
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
    return FALSE;
  }

  bitmask = HAL_hsusb_GetBits32(instance, 
                                HAL_HSUSB_L1_EP_CTRL_ADDR,
                                HAL_HSUSB_L1_EP_CTRL_TX_EP_PRIME_L1_EN_BMSK);

  bitmask &= (0x1 << (ep + HAL_HSUSB_L1_EP_CTRL_TX_EP_PRIME_L1_EN_SHFT));

  return (bitmask != 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_SetTxEpPrimeL1EnableImplp3q3r50
=============================================================================*/
/**
Sets whether the transition to L1 state can occur when a specific endpoint
is primed

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param ep       [IN] -- The endpoint number
@param state    [IN] -- TRUE to refrain from transitioning to L1 state whenever
                        the particular TX endpoing is primed.
                        FALSE to allow transition to L1 state even when the 
                        particular endpoint is primed 

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_SetTxEpPrimeL1EnableImplp3q3r50(uint8 instance, uint8 ep, boolean state)
{
  uint32 bitmask;

  if (ep >= max_endpoints)
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
    return;
  }

  bitmask = 0x1 << (HAL_HSUSB_L1_EP_CTRL_TX_EP_PRIME_L1_EN_SHFT + ep);

  if (state == TRUE)
  {
    HAL_hsusb_SetBits32(instance, 
                        HAL_HSUSB_L1_EP_CTRL_ADDR,
                        bitmask);
  }
  else
  {
    HAL_hsusb_ClearBits32(instance, 
                          HAL_HSUSB_L1_EP_CTRL_ADDR,
                          bitmask);   
  }
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1PllPwrDwnEnableImplp3q3r50
=============================================================================*/
/**
Reports whether power down of 480 MHz PLL in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Power down of 480 MHz PLL in L1 state is enabled
FALSE -- Power down of 480 MHz PLL in L1 state is disabled
*/
static boolean HAL_hsusb_GetL1PllPwrDwnEnableImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance,
          HAL_HSUSB_L1_CONFIG_ADDR, HAL_HSUSB_L1_CONFIG_PLL_PWR_DWN_EN_BMSK) != 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1PllPwrDwnEnableImplp3q3r50
=============================================================================*/
/**
Sets whether power down of 480 MHz PLL in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE to enable
                        FALSE to disable

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Power down of 480 MHz PLL in L1 state is enabled
FALSE -- Power down of 480 MHz PLL in L1 state is disabled
*/
static void HAL_hsusb_SetL1PllPwrDwnEnableImplp3q3r50(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_L1_CONFIG_ADDR, HAL_HSUSB_L1_CONFIG_PLL_PWR_DWN_EN_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_L1_CONFIG_ADDR, HAL_HSUSB_L1_CONFIG_PLL_PWR_DWN_EN_BMSK)
    );
}

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1PhyLPMEnableImplp3q3r50
=============================================================================*/
/**
Reports whether ULPI Low Power Mode is enabled in L1 state

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- ULPI Low Power Mode in L1 state is enabled
FALSE -- ULPI Low Power Mode in L1 state is disabled 
*/
static boolean HAL_hsusb_GetL1PhyLPMEnableImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
                              HAL_HSUSB_L1_CONFIG_ADDR, 
                              HAL_HSUSB_L1_CONFIG_PHY_LPM_EN_BMSK) != 0);
}

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1PhyLPMEnableImplp3q3r50
=============================================================================*/
/**
Sets whether ULPI Low Power Mode is enabled in L1 state

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE to enable
                        FALSE to disable

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_SetL1PhyLPMEnableImplp3q3r50(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_L1_CONFIG_ADDR, HAL_HSUSB_L1_CONFIG_PHY_LPM_EN_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_L1_CONFIG_ADDR, HAL_HSUSB_L1_CONFIG_PHY_LPM_EN_BMSK)
    );
}

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1GateAhbClkEnableImplp3q3r50
=============================================================================*/
/**
Reports whether clock request signaling is enabled for usb_ahb_clk in L1 state


@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Clock request signaling is enabled for usb_ahb_clk in L1 state is enabled
FALSE -- Clock request signaling is enabled for usb_ahb_clk in L1 state disabled 
*/
static boolean HAL_hsusb_GetL1GateAhbClkEnableImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
                              HAL_HSUSB_L1_CONFIG_ADDR, 
                              HAL_HSUSB_L1_CONFIG_GATE_AHB_CLK_EN_BMSK) != 0);
}

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1GateAhbClkEnableImplp3q3r50
=============================================================================*/
/**
Sets whether clock request signaling is enabled for usb_ahb_clk in L1 state

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE to enable
                        FALSE to disable

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_SetL1GateAhbClkEnableImplp3q3r50(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, 
                        HAL_HSUSB_L1_CONFIG_ADDR, 
                        HAL_HSUSB_L1_CONFIG_GATE_AHB_CLK_EN_BMSK) : 
    HAL_hsusb_ClearBits32(instance,
                          HAL_HSUSB_L1_CONFIG_ADDR,
                          HAL_HSUSB_L1_CONFIG_GATE_AHB_CLK_EN_BMSK)
    );
}

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1GateFsXcvrClkEnableImplp3q3r50
=============================================================================*/
/**
Reports whether clock gating of usb_fs_xcvr_clk in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return

TRUE  -- Clock gating of usb_fs_xcvr_clk in L1 state is enabled
FALSE -- Clock gating of usb_fs_xcvr_clk in L1 state is disabled
*/
static boolean HAL_hsusb_GetL1GateFsXcvrClkEnableImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
          HAL_HSUSB_L1_CONFIG_ADDR,
          HAL_HSUSB_L1_CONFIG_GATE_FS_XCVR_CLK_EN_BMSK) != 0);
}

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1GateFsXcvrClkEnableImplp3q3r50
=============================================================================*/
/**
Sets whether clock gating of usb_fs_xcvr_clk in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE to enable
                        FALSE to disable

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/

static void HAL_hsusb_SetL1GateFsXcvrClkEnableImplp3q3r50(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, 
                        HAL_HSUSB_L1_CONFIG_ADDR,
                        HAL_HSUSB_L1_CONFIG_GATE_FS_XCVR_CLK_EN_BMSK) : 
    HAL_hsusb_ClearBits32(instance, 
                          HAL_HSUSB_L1_CONFIG_ADDR,
                          HAL_HSUSB_L1_CONFIG_GATE_FS_XCVR_CLK_EN_BMSK)
    );
}

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1GateSysClkEnableImplp3q3r50
=============================================================================*/
/**
Reports whether clock gating of usb_system_clk in L1 state is enabled


@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return

TRUE  -- Clock gating of usb_system_clk in L1 state is enabled
FALSE -- Clock gating of usb_system_clk in L1 state is disabled
*/
static boolean HAL_hsusb_GetL1GateSysClkEnableImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
          HAL_HSUSB_L1_CONFIG_ADDR,
          HAL_HSUSB_L1_CONFIG_GATE_SYS_CLK_EN_BMSK) != 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1GateSysClkEnableImplp3q3r50
=============================================================================*/
/**
Sets whether clock gating of usb_system_clk in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE to enable
                        FALSE to disable

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_SetL1GateSysClkEnableImplp3q3r50(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, 
                        HAL_HSUSB_L1_CONFIG_ADDR,
                        HAL_HSUSB_L1_CONFIG_GATE_SYS_CLK_EN_BMSK) : 
    HAL_hsusb_ClearBits32(instance, 
                          HAL_HSUSB_L1_CONFIG_ADDR,
                          HAL_HSUSB_L1_CONFIG_GATE_SYS_CLK_EN_BMSK)
    );
}

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1GateXcvrClkEnableImplp3q3r50
=============================================================================*/
/**
Reports whether clock gating of usb_xcvr_clk in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return

TRUE  -- Clock gating of usb_xcvr_clk in L1 state is enabled
FALSE -- Clock gating of usb_xcvr_clk in L1 state is disabled
*/
static boolean HAL_hsusb_GetL1GateXcvrClkEnableImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
          HAL_HSUSB_L1_CONFIG_ADDR,
          HAL_HSUSB_L1_CONFIG_GATE_XCVR_CLK_EN_BMSK) != 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1GateXcvrClkEnableImplp3q3r50
=============================================================================*/
/**
Sets whether clock gating of usb_xcvr_clk in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE to enable
                        FALSE to disable

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_SetL1GateXcvrClkEnableImplp3q3r50(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, 
                        HAL_HSUSB_L1_CONFIG_ADDR,
                        HAL_HSUSB_L1_CONFIG_GATE_XCVR_CLK_EN_BMSK) : 
    HAL_hsusb_ClearBits32(instance, 
                          HAL_HSUSB_L1_CONFIG_ADDR,
                          HAL_HSUSB_L1_CONFIG_GATE_XCVR_CLK_EN_BMSK)
    );
}

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1RemoteWakeupEnableImplp3q3r50
=============================================================================*/
/**
Reports whether Remote Wakeup in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return

TRUE  -- Link controller can initiate Remote Wakeup.
FALSE -- Link controller never initiates Remote Wakeup in L1 state.
*/
static boolean HAL_hsusb_GetL1RemoteWakeupEnableImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
          HAL_HSUSB_L1_CONFIG_ADDR,
          HAL_HSUSB_L1_CONFIG_L1_REMOTE_WAKEUP_EN_BMSK) != 0);
}

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1RemoteWakeupEnableImplp3q3r50
=============================================================================*/
/**
Sets whether Remote Wakeup in L1 state is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE for link controller can initiate Remote Wakeup.
                        FALSE for link controller never initiates Remote Wakeup in L1 state.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_SetL1RemoteWakeupEnableImplp3q3r50(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, 
                        HAL_HSUSB_L1_CONFIG_ADDR,
                        HAL_HSUSB_L1_CONFIG_L1_REMOTE_WAKEUP_EN_BMSK) : 
    HAL_hsusb_ClearBits32(instance, 
                          HAL_HSUSB_L1_CONFIG_ADDR,
                          HAL_HSUSB_L1_CONFIG_L1_REMOTE_WAKEUP_EN_BMSK)
    );
}

/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1LPMEnableImplp3q3r50
=============================================================================*/
/**
Reports whether Link Power Management Support is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return

TRUE  -- Link Power Management support is enabled
FALSE -- Link Power Management support is disabled. No response to LPM extended
         transactions.
*/
static boolean HAL_hsusb_GetL1LPMEnableImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
          HAL_HSUSB_L1_CONFIG_ADDR,
          HAL_HSUSB_L1_CONFIG_LPM_EN_BMSK) != 0);
}

/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1LPMEnableImplp3q3r50
=============================================================================*/
/**
Sets whether Link Power Management Support is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state    [IN] -- TRUE for enabling Link Power Management
                        FALSE for disabling Link Power Management and disable
                        hardware response to LPM extended transactions.

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_SetL1LPMEnableImplp3q3r50(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, 
                        HAL_HSUSB_L1_CONFIG_ADDR,
                        HAL_HSUSB_L1_CONFIG_LPM_EN_BMSK) : 
    HAL_hsusb_ClearBits32(instance, 
                          HAL_HSUSB_L1_CONFIG_ADDR,
                          HAL_HSUSB_L1_CONFIG_LPM_EN_BMSK)
    );
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1PllTurnoffMinHirdImplp3q3r50
=============================================================================*/
/**
Reports the minimum expected HIRD value from host that enables HW mechanism for
turning off the 480 MHz PLL

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
Default value is 50us.
*/static uint8 HAL_hsusb_GetL1PllTurnoffMinHirdImplp3q3r50(uint8 instance)
{
  uint32 bitmask;
  
  bitmask = HAL_hsusb_GetBits32(instance, 
                              HAL_HSUSB_L1_CONFIG_ADDR,
                              HAL_HSUSB_L1_CONFIG_PLL_TURNOFF_MIN_HIRD_BMSK);
  
  return bitmask >> HAL_HSUSB_L1_CONFIG_PLL_TURNOFF_MIN_HIRD_SHFT;
}


/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1PllTurnoffMinHirdImplp3q3r50
=============================================================================*/
/**
Sets the minimum expected HIRD value from host that enables HW mechanism for
turning off the 480 MHz PLL

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param minHIRD [IN]  -- Minimum expected HIRD value to use

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_SetL1PllTurnoffMinHirdImplp3q3r50(uint8 instance, uint8 minHIRD)
{
    HAL_hsusb_SetBits32(instance, 
                        HAL_HSUSB_L1_CONFIG_ADDR,
                        (minHIRD << HAL_HSUSB_L1_CONFIG_PLL_TURNOFF_MIN_HIRD_SHFT) & HAL_HSUSB_L1_CONFIG_PLL_TURNOFF_MIN_HIRD_BMSK);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1LongEntryCountImplp3q3r50
=============================================================================*/
/**
Reports the count of number of exits from L1 where duration in L1 is > 200us

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
Count of number of exits from L1 where duration in L1 is > 200us
*/
static uint16 HAL_hsusb_GetL1LongEntryCountImplp3q3r50(uint8 instance)
{
  uint32 bitmask;

  bitmask = HAL_hsusb_GetBits32(instance, 
                                HAL_HSUSB_LPM_DEBUG_1_ADDR,
                                HAL_HSUSB_LPM_DEBUG_1_DEBUG_L1_LONG_ENT_CNT_BMSK);

  return (bitmask >> HAL_HSUSB_LPM_DEBUG_1_DEBUG_L1_LONG_ENT_CNT_SHFT);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1ShortEntryCountImplp3q3r50
=============================================================================*/
/**
Reports the count of number of exits from L1 where duration in L1 is <= 200us

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
Count of number of exits from L1 where duration in L1 is <= 200us
*/
static uint16 HAL_hsusb_GetL1ShortEntryCountImplp3q3r50(uint8 instance)
{
  uint32 bitmask;

  bitmask = HAL_hsusb_GetBits32(instance, 
                                HAL_HSUSB_LPM_DEBUG_1_ADDR,
                                HAL_HSUSB_LPM_DEBUG_1_DEBUG_L1_SHORT_ENT_CNT_BMSK);

  return (bitmask >> HAL_HSUSB_LPM_DEBUG_1_DEBUG_L1_SHORT_ENT_CNT_SHFT);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_ClearL1EntryCountsImplp3q3r50
=============================================================================*/
/**
Clears the L1 entry counters

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_ClearL1EntryCountsImplp3q3r50(uint8 instance)
{
  HAL_hsusb_Write32(instance, HAL_HSUSB_LPM_DEBUG_1_ADDR, 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1RemoteWakeupTimeImplp3q3r50
=============================================================================*/
/**
Reports the duration that the device drives remote wakeup

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
The duration that the device drives remote wakeup, defaults to 50us.
*/
static uint8 HAL_hsusb_GetL1RemoteWakeupTimeImplp3q3r50(uint8 instance)
{
  uint32 bitmask;


  bitmask = HAL_hsusb_GetBits32(instance, 
                                HAL_HSUSB_LPM_DEBUG_2_ADDR,
                                HAL_HSUSB_LPM_DEBUG_2_L1_RMT_WKUP_TIME_BMSK);

  return ((bitmask >> HAL_HSUSB_LPM_DEBUG_2_L1_RMT_WKUP_TIME_SHFT) + 
          min_remote_wakeup_time_us);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1RemoteWakeupTimeImplp3q3r50
=============================================================================*/
/**
Sets the duration that the device drives remote wakeup

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param timeInUS [IN] -- Duration that the device should drive remote wakeup                       

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_SetL1RemoteWakeupTimeImplp3q3r50(uint8 instance,
                                                      uint8 timeInUS)
{
  if ((timeInUS < min_remote_wakeup_time_us) ||
      (timeInUS > max_remote_wakeup_time_us) )
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
    return;
  }

  HAL_hsusb_SetBits32(instance, 
                      HAL_HSUSB_LPM_DEBUG_2_ADDR,
                      ((timeInUS - min_remote_wakeup_time_us) << HAL_HSUSB_LPM_DEBUG_2_L1_RMT_WKUP_TIME_SHFT) 
                         & HAL_HSUSB_LPM_DEBUG_2_L1_RMT_WKUP_TIME_BMSK);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1FPRImplp3q3r50
=============================================================================*/
/**
Reports whether L1 force port resume is currently active

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Software is driving force port resume
FALSE -- Software is not driving force port resume 
*/
static boolean HAL_hsusb_GetL1FPRImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
                              HAL_HSUSB_LPM_DEBUG_2_ADDR,
                              HAL_HSUSB_LPM_DEBUG_2_L1_FPR_BMSK) != 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_SetL1FPRImplp3q3r50
=============================================================================*/
/**
Sets whether L1 force port resume is currently active

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state [IN]    -- TRUE for software driving force port resume signaling

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_SetL1FPRImplp3q3r50(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, 
                        HAL_HSUSB_LPM_DEBUG_2_ADDR,
                        HAL_HSUSB_LPM_DEBUG_2_L1_FPR_BMSK) : 
    HAL_hsusb_ClearBits32(instance, 
                          HAL_HSUSB_L1_CONFIG_ADDR,
                          HAL_HSUSB_LPM_DEBUG_2_L1_FPR_BMSK)
    );
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetHSICClkPllBypassNLImplp3q3r50
=============================================================================*/
/**
Disables PLL analog logic. Active low. Connects to bypassnl
input of NT_PLL.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- PLL analog logic is not disabled
FALSE -- PLL analog logic is disabled
*/
static boolean HAL_hsusb_GetHSICClkPllBypassNLImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
                              HAL_HSUSB_LPM_DEBUG_2_ADDR,
                              HAL_HSUSB_LPM_DEBUG_2_HSIC_CLK_PLL_BYPASSNL_BMSK) != 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetHSICClkPllResetImplp3q3r50
=============================================================================*/
/**
Resets all FF in PLL. Active low. Connects to reset_n input of
NT_PLL

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- FF in PLL is not resetting
FALSE -- FF in PLL is resetting
*/
static boolean HAL_hsusb_GetHSICClkPllResetImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
                              HAL_HSUSB_LPM_DEBUG_2_ADDR,
                              HAL_HSUSB_LPM_DEBUG_2_HSIC_CLK_PLL_RESET_BMSK) != 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetHSICClkGateImplp3q3r50
=============================================================================*/
/**
Clock gating of hsic_clk and ulpi_clk, without turning off HSIC
PLL

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Clock gating of hsic_clk and ulpi_clk are active
FALSE -- Clock gating of hsic_clk and ulpi_clk are not active
*/
static boolean HAL_hsusb_GetHSICClkGateImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
                              HAL_HSUSB_LPM_DEBUG_2_ADDR,
                              HAL_HSUSB_LPM_DEBUG_2_HSIC_CLK_GATE_BMSK) != 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetSysClkGateImplp3q3r50
=============================================================================*/
/**
Clock gating of cc_usb_system_clk

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Clock gating of cc_usb_system_clk is active
FALSE -- Clock gating of cc_usb_system_clk is not active
*/
static boolean HAL_hsusb_GetSysClkGateImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
                              HAL_HSUSB_LPM_DEBUG_2_ADDR,
                              HAL_HSUSB_LPM_DEBUG_2_SYS_CLK_GATE_BMSK) != 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetAHBClkGateImplp3q3r50
=============================================================================*/
/**
Clock gating of usb_ahb_clk

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Clock gating of usb_ahb_clk is active
FALSE -- Clock gating of usb_ahb_clk is not active
*/
static boolean HAL_hsusb_GetAHBClkGateImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
                              HAL_HSUSB_LPM_DEBUG_2_ADDR,
                              HAL_HSUSB_LPM_DEBUG_2_AHB_CLK_GATE_BMSK) != 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetL1StateImplp3q3r50
=============================================================================*/
/**
Reports whether device is in L1 state. 

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Link Controller and HSIC PHY are in L1 state
FALSE -- Link Controller and HSIC PHY are not in L1 state
*/
static boolean HAL_hsusb_GetL1StateImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
                              HAL_HSUSB_LPM_DEBUG_2_ADDR,
                              HAL_HSUSB_LPM_DEBUG_2_L1_STATE_BMSK) != 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetDebugL1EnableImplp3q3r50
=============================================================================*/
/**
Reports whether DEBUG counters operation is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Debug counters are enabled
FALSE -- Debug counters are disabled
*/
static boolean HAL_hsusb_GetDebugL1EnableImplp3q3r50(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, 
                              HAL_HSUSB_LPM_DEBUG_2_ADDR,
                              HAL_HSUSB_LPM_DEBUG_2_DEBUG_L1_EN_BMSK) != 0);
}


/*=============================================================================
FUNCTION:      HAL_hsusb_SetDebugL1EnableImplp3q3r50
=============================================================================*/
/**
Sets whether DEBUG counters operation is enabled

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().
@param state [IN]    -- TRUE to enable debug counters
                        FALSE to disable debug counters

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
None.
*/
static void HAL_hsusb_SetDebugL1EnableImplp3q3r50(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, 
                        HAL_HSUSB_LPM_DEBUG_2_ADDR,
                        HAL_HSUSB_LPM_DEBUG_2_DEBUG_L1_EN_BMSK) : 
    HAL_hsusb_ClearBits32(instance, 
                          HAL_HSUSB_LPM_DEBUG_2_ADDR,
                          HAL_HSUSB_LPM_DEBUG_2_DEBUG_L1_EN_BMSK)
    );
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetBRemoteWakeImplp3q3r50
=============================================================================*/
/**
Reports whether host allows device to wakeup host upon any meaningful
application specific event.

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
TRUE  -- Device can initiate remote wakeup
FALSE -- Disables the device from initiating remote wakeup
*/
static uint8 HAL_hsusb_GetBRemoteWakeImplp3q3r50(uint8 instance)
{
  uint32 bitmask;
  bitmask = (HAL_hsusb_GetBits32(instance, 
                              HAL_HSUSB_LPM_ATTRIBUTES_ADDR,
                              HAL_HSUSB_LPM_ATTRIBUTES_BREMOTEWAKE_BMSK) != 0);
  return bitmask >> HAL_HSUSB_LPM_ATTRIBUTES_BREMOTEWAKE_SHFT;
}

static void HAL_hsusb_SetBRemoteWakeImplp3q3r50(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, 
                        HAL_HSUSB_LPM_ATTRIBUTES_ADDR,
                        HAL_HSUSB_LPM_ATTRIBUTES_BREMOTEWAKE_BMSK) : 
    HAL_hsusb_ClearBits32(instance, 
                          HAL_HSUSB_LPM_ATTRIBUTES_ADDR,
                          HAL_HSUSB_LPM_ATTRIBUTES_BREMOTEWAKE_BMSK)
    );
}


/*=============================================================================
FUNCTION:      HAL_hsusb_GetHIRDImplp3q3r50
=============================================================================*/
/**
Reports the host initiated resume duration

@param instance [IN] -- Determines which HS-USB core to use. This value should
                        be one less than reported by 
                        HAL_hsusb_SupportedCoresQty().

@dependencies
HAL_hsusb_Init() must have been called prior to this function.

@side effects
None.

@return
Value of the host initiated resume duration.
*/
static uint8 HAL_hsusb_GetHIRDImplp3q3r50(uint8 instance)
{
  uint32 bitmask;

  bitmask = HAL_hsusb_GetBits32(instance, 
                                HAL_HSUSB_LPM_ATTRIBUTES_ADDR,
                                HAL_HSUSB_LPM_ATTRIBUTES_HIRD_BMSK);

  return (bitmask >> HAL_HSUSB_LPM_ATTRIBUTES_HIRD_SHFT);
}

static void HAL_hsusb_SetHIRDImplp3q3r50(uint8 instance, uint8 HIRD)
{
  HAL_hsusb_SetBits32(instance, 
                      HAL_HSUSB_LPM_ATTRIBUTES_ADDR,
                      (HIRD << HAL_HSUSB_LPM_ATTRIBUTES_HIRD_SHFT) & HAL_HSUSB_LPM_ATTRIBUTES_HIRD_BMSK);
}

/* ===========================================================================
**
**  HAL_hsusb_ConstructImplp3q3r50
**
**  Description:
**    This function initializes the HS-USB implementation object for 
**    the  HS-USB core usb_otg_hs_xp3q3r50 implementation.
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
void HAL_hsusb_ConstructImplp3q3r50(struct HAL_HSUSB_CoreIfType* this)
{
  HAL_hsusb_ConstructImplp3q2r13(this);
  this->Version = 8;
  this->GetTxEpPrimeL1Exit       = HAL_hsusb_GetTxEpPrimeL1ExitImplp3q3r50;
  this->SetTxEpPrimeL1Exit       = HAL_hsusb_SetTxEpPrimeL1ExitImplp3q3r50;
  this->GetTxEpPrimeL1Enable     = HAL_hsusb_GetTxEpPrimeL1EnableImplp3q3r50;
  this->SetTxEpPrimeL1Enable     = HAL_hsusb_SetTxEpPrimeL1EnableImplp3q3r50;
  this->GetL1PllPwrDwnEnable     = HAL_hsusb_GetL1PllPwrDwnEnableImplp3q3r50;
  this->SetL1PllPwrDwnEnable     = HAL_hsusb_SetL1PllPwrDwnEnableImplp3q3r50;
  this->GetL1PhyLPMEnable        = HAL_hsusb_GetL1PhyLPMEnableImplp3q3r50;
  this->SetL1PhyLPMEnable        = HAL_hsusb_SetL1PhyLPMEnableImplp3q3r50;
  this->GetL1GateAhbClkEnable    = HAL_hsusb_GetL1GateAhbClkEnableImplp3q3r50;
  this->SetL1GateAhbClkEnable    = HAL_hsusb_SetL1GateAhbClkEnableImplp3q3r50;
  this->GetL1GateFsXcvrClkEnable = HAL_hsusb_GetL1GateFsXcvrClkEnableImplp3q3r50;
  this->SetL1GateFsXcvrClkEnable = HAL_hsusb_SetL1GateFsXcvrClkEnableImplp3q3r50;
  this->GetL1GateSysClkEnable    = HAL_hsusb_GetL1GateSysClkEnableImplp3q3r50;
  this->SetL1GateSysClkEnable    = HAL_hsusb_SetL1GateSysClkEnableImplp3q3r50;
  this->GetL1GateXcvrClkEnable   = HAL_hsusb_GetL1GateXcvrClkEnableImplp3q3r50;
  this->SetL1GateXcvrClkEnable   = HAL_hsusb_SetL1GateXcvrClkEnableImplp3q3r50;
  this->GetL1RemoteWakeupEnable  = HAL_hsusb_GetL1RemoteWakeupEnableImplp3q3r50;
  this->SetL1RemoteWakeupEnable  = HAL_hsusb_SetL1RemoteWakeupEnableImplp3q3r50;
  this->GetL1LPMEnable           = HAL_hsusb_GetL1LPMEnableImplp3q3r50;
  this->SetL1LPMEnable           = HAL_hsusb_SetL1LPMEnableImplp3q3r50;
  this->GetL1PllTurnoffMinHird   = HAL_hsusb_GetL1PllTurnoffMinHirdImplp3q3r50;
  this->SetL1PllTurnoffMinHird   = HAL_hsusb_SetL1PllTurnoffMinHirdImplp3q3r50;
  this->GetL1LongEntryCount      = HAL_hsusb_GetL1LongEntryCountImplp3q3r50;
  this->GetL1ShortEntryCount     = HAL_hsusb_GetL1ShortEntryCountImplp3q3r50;
  this->ClearL1EntryCounts       = HAL_hsusb_ClearL1EntryCountsImplp3q3r50;
  this->GetL1RemoteWakeupTime    = HAL_hsusb_GetL1RemoteWakeupTimeImplp3q3r50;
  this->SetL1RemoteWakeupTime    = HAL_hsusb_SetL1RemoteWakeupTimeImplp3q3r50;
  this->GetL1FPR                 = HAL_hsusb_GetL1FPRImplp3q3r50;
  this->SetL1FPR                 = HAL_hsusb_SetL1FPRImplp3q3r50;
  this->GetHSICClkPllBypassNL    = HAL_hsusb_GetHSICClkPllBypassNLImplp3q3r50;
  this->GetHSICClkPllReset       = HAL_hsusb_GetHSICClkPllResetImplp3q3r50;
  this->GetHSICClkGate           = HAL_hsusb_GetHSICClkGateImplp3q3r50;
  this->GetSysClkGate            = HAL_hsusb_GetSysClkGateImplp3q3r50;
  this->GetAHBClkGate            = HAL_hsusb_GetAHBClkGateImplp3q3r50;
  this->GetL1State               = HAL_hsusb_GetL1StateImplp3q3r50;
  this->GetDebugL1Enable         = HAL_hsusb_GetDebugL1EnableImplp3q3r50;
  this->SetDebugL1Enable         = HAL_hsusb_SetDebugL1EnableImplp3q3r50;
  this->GetBRemoteWake           = HAL_hsusb_GetBRemoteWakeImplp3q3r50;
  this->SetBRemoteWake           = HAL_hsusb_SetBRemoteWakeImplp3q3r50;
  this->GetHIRD                  = HAL_hsusb_GetHIRDImplp3q3r50;
  this->SetHIRD                  = HAL_hsusb_SetHIRDImplp3q3r50;
}


/*
===========================================================================

FILE:         HAL_hsusb_impl_Default.c

DESCRIPTION:  
This is the HAL implementation for the HS-USB default core.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/hal/src/HAL_hsusb_impl_Default.c#1 $

===========================================================================

===========================================================================
Copyright © 2008-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HAL_hsusb_impl_Default.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_hsusb_GetCoreBooleanDefault
**
**  Description:
**    This function implements the not supported HS-USB core "Get" interfaces
**    which have uint8 input parameter and return boolean.
**      
**  Dependencies:
**    None.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    FALSE   - The default value
**
** ======================================================================== */
static boolean HAL_hsusb_GetCoreBooleanDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetCoreBooleanDefault
**
**  Description:
**    This function implements the not supported HS-USB core "Set" interfaces
**    which have uint8 and boolean input parameters and return void.
**      
**  Dependencies:
**    None.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - not in use
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetCoreBooleanDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_ResetDefault
**
**  Description:
**    This function is intended to return the HS-USB core
**    to its  power-on status.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_ResetDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_ReadUlpiPhyDefault
**
**  Description:
**    This function reads from a ULPI PHY register of the HS-USB default core port.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    port     - Determines which HS-USB core port to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedPortsQty().
**    reg      - Determines the register address. This value must be 
**             in the address ranges defined by the HW specification. 
**    data     - Returns the read data.
**
**  Return:
**    HAL_HSUSB_NotSupported - The function is not supported by the HS-USB core.
**
** ======================================================================== */
HAL_HSUSB_ErrorType HAL_hsusb_ReadUlpiPhyDefault(uint8 instance, uint8 port, uint8 reg, uint8* data)
{
  (void)port;
  (void)reg;
  (void)data;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_WriteUlpiPhyDefault
**
**  Description:
**    This function writes to a ULPI PHY register of the HS-USB default core port.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    port     - Determines which HS-USB core port to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedPortsQty().
**    reg      - Determines the register address. This value must be 
**             in the address ranges defined by the HW specification. 
**    data     - Determines the write data.
**
**  Return:
**    HAL_HSUSB_NotSupported - The function is not supported by the HS-USB core.
**
** ======================================================================== */
HAL_HSUSB_ErrorType HAL_hsusb_WriteUlpiPhyDefault(uint8 instance, uint8 port, uint8 reg, uint8 data)
{
  (void)port;
  (void)reg;
  (void)data;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetAhbBurstModeImplDefault
**
**  Description:
**    This function reports the mode of the AMBA AHB master interface 
**    m_hburst signal for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**     The mode of the AMBA AHB master interface m_hburst signal.
**
** ======================================================================== */
static HAL_HSUSB_AhbBurstMode HAL_hsusb_GetAhbBurstModeImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return HAL_HSUSB_AHB_BURST_INCR;
}

/* ===========================================================================
**
**  HAL_hsusb_SetAhbBurstModeImplDefault
**
**  Description:
**    This function sets the mode of the AMBA AHB master interface 
**    m_hburst signal for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    eMode    - Determines mode of the AMBA AHB master interface 
**             m_hburst signal.
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetAhbBurstModeImplDefault(uint8 instance, HAL_HSUSB_AhbBurstMode eMode)
{
  (void)eMode;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetHprotModeImplDefault
**
**  Description:
**    This function reports the HPROT signal mode for the default HS-USB 
**    cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    The HPROT signal mode.
**
** ======================================================================== */
static HAL_HSUSB_HprotMode HAL_hsusb_GetHprotModeImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return HAL_HSUSB_HPROT_NONPOSTED;
}

/* ===========================================================================
**
**  HAL_hsusb_SetHprotModeImplDefault
**
**  Description:
**    This function sets the HPROT signal mode for the default HS-USB 
**    cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    eMode    - Determines the HPROT signal mode.
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetHprotModeImplDefault(uint8 instance, HAL_HSUSB_HprotMode eMode)
{
  (void)eMode;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetTestBusSelectorImplDefault
**
**  Description:
**    This function reports which group of the internal signals is 
**    routed to the test bus for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static HAL_HSUSB_TestBusSelectorType HAL_hsusb_GetTestBusSelectorImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return HAL_HSUSB_TEST_BUS_KSM;
}

/* ===========================================================================
**
**  HAL_hsusb_SetTestBusSelectorImplDefault
**
**  Description:
**    This function sets which group of the internal signals is 
**    routed to the test bus for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static void HAL_hsusb_SetTestBusSelectorImplDefault(uint8 instance, HAL_HSUSB_TestBusSelectorType eMode)
{
  (void)eMode;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetEndpointPipeIDImplDefault
**
**  Description:
**    This function reports the endpoint pipe number
**    for the default HS-USB cores' implementation.
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
uint8 HAL_hsusb_GetEndpointPipeIDImplDefault(uint8 instance, uint8 ep, boolean direction)
{
  (void)ep;
  (void)direction;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return 0x1F;
}

/* ===========================================================================
**
**  HAL_hsusb_SetEndpointPipeIDImplDefault
**
**  Description:
**    This function sets the endpoint pipe number
**    for the default HS-USB cores' implementation.
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
void HAL_hsusb_SetEndpointPipeIDImplDefault(uint8 instance, uint8 ep, boolean direction, uint8 id)
{
  (void)ep;
  (void)direction;
  (void)id;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetTxEpPrimeL1Default
**
**  Description:
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    ep       - Determines the endpoint number.
**             Values are: 1-15.
**
**  Return: FALSE
**
** ======================================================================== */
boolean HAL_hsusb_GetTxEpPrimeL1Default(uint8 instance, uint8 ep)
{
  (void)instance;
  (void) ep;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}


/* ===========================================================================
**
**  HAL_hsusb_SetTxEpPrimeL1Default
**
**  Description:
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 8 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    ep       - Determines the endpoint number.
**             Values are: 1-15.
**    state    - Desired bit state.
**
**  Return: FALSE
**
** ======================================================================== */
void HAL_hsusb_SetTxEpPrimeL1Default(uint8 instance, uint8 ep, boolean state)
{
  (void)instance;
  (void)ep;
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}


void HAL_hsusb_ClearL1EntryCountsDefault(uint8 instance)
{
  (void)instance;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}


uint8 HAL_hsusb_GetUint8Default(uint8 instance)
{
  (void)instance;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return 0;
}

void HAL_hsusb_SetUint8Default(uint8 instance, uint8 val)
{
  (void)instance;
  (void)val;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}


uint16 HAL_hsusb_GetUint16Default(uint8 instance)
{
  (void)instance;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return 0;
}

/* ===========================================================================
**
**  HAL_hsusb_ConstructImplDefault
**
**  Description:
**    This function initializes the HS-USB implementation object
**    for the  HS-USB core default implementation.
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
void HAL_hsusb_ConstructImplDefault(struct HAL_HSUSB_CoreIfType* this)
{
  this->Reset = HAL_hsusb_ResetDefault;
  this->ReadUlpiPhy = HAL_hsusb_ReadUlpiPhyDefault;
  this->WriteUlpiPhy = HAL_hsusb_WriteUlpiPhyDefault;
  this->GetAhbBurstMode = HAL_hsusb_GetAhbBurstModeImplDefault;
  this->SetAhbBurstMode = HAL_hsusb_SetAhbBurstModeImplDefault;
  this->GetSe0GlitchFixCtrl = HAL_hsusb_GetCoreBooleanDefault;
  this->SetSe0GlitchFixCtrl = HAL_hsusb_SetCoreBooleanDefault;
  this->GetFs3Wire2WireSelect = HAL_hsusb_GetCoreBooleanDefault;
  this->SetFs3Wire2WireSelect = HAL_hsusb_SetCoreBooleanDefault;
  this->GetXtorBypass = HAL_hsusb_GetCoreBooleanDefault;
  this->SetXtorBypass = HAL_hsusb_SetCoreBooleanDefault;
  this->GetPostedWrites = HAL_hsusb_GetCoreBooleanDefault;
  this->SetPostedWrites = HAL_hsusb_SetCoreBooleanDefault;
  this->GetGrantStolen = HAL_hsusb_GetCoreBooleanDefault;
  this->SetGrantStolen = HAL_hsusb_SetCoreBooleanDefault;
  this->GetHprotMode = HAL_hsusb_GetHprotModeImplDefault;
  this->SetHprotMode = HAL_hsusb_SetHprotModeImplDefault;
  this->GetIncrOverride = HAL_hsusb_GetCoreBooleanDefault;
  this->SetIncrOverride = HAL_hsusb_SetCoreBooleanDefault;
  this->GetAsyncBridgesBypass = HAL_hsusb_GetCoreBooleanDefault;
  this->SetAsyncBridgesBypass = HAL_hsusb_SetCoreBooleanDefault;
  this->GetPeDpTxFifoIdleForce = HAL_hsusb_GetCoreBooleanDefault;
  this->SetPeDpTxFifoIdleForce = HAL_hsusb_SetCoreBooleanDefault;
  this->GetTestBusSelector = HAL_hsusb_GetTestBusSelectorImplDefault;
  this->SetTestBusSelector = HAL_hsusb_SetTestBusSelectorImplDefault;
  this->GetTxBufCollisionMode = HAL_hsusb_GetCoreBooleanDefault;
  this->SetTxBufCollisionMode = HAL_hsusb_SetCoreBooleanDefault;
  this->GetStreamRxBypassMode = HAL_hsusb_GetCoreBooleanDefault;
  this->SetStreamRxBypassMode = HAL_hsusb_SetCoreBooleanDefault;
  this->GetStreamTxAutoMode = HAL_hsusb_GetCoreBooleanDefault;
  this->SetStreamTxAutoMode = HAL_hsusb_SetCoreBooleanDefault;
  this->GetHostSimTimersStdMode = HAL_hsusb_GetCoreBooleanDefault;
  this->SetHostSimTimersStdMode = HAL_hsusb_SetCoreBooleanDefault;
  this->GetHostSimTimersSuspMode = HAL_hsusb_GetCoreBooleanDefault;
  this->SetHostSimTimersSuspMode = HAL_hsusb_SetCoreBooleanDefault;
  this->GetDevicePEStateReset = HAL_hsusb_GetCoreBooleanDefault;
  this->SetDevicePEStateReset = HAL_hsusb_SetCoreBooleanDefault;
  this->GetIsochronousBugFix = HAL_hsusb_GetCoreBooleanDefault;
  this->SetIsochronousBugFix = HAL_hsusb_SetCoreBooleanDefault;
  this->GetAHBBufferableDataAccess = HAL_hsusb_GetCoreBooleanDefault;
  this->SetAHBBufferableDataAccess = HAL_hsusb_SetCoreBooleanDefault;
  this->GetLegacyMode = HAL_hsusb_GetCoreBooleanDefault;
  this->SetLegacyMode = HAL_hsusb_SetCoreBooleanDefault;
  this->GetEndpointPipeID = HAL_hsusb_GetEndpointPipeIDImplDefault;
  this->SetEndpointPipeID = HAL_hsusb_SetEndpointPipeIDImplDefault;
  this->GetSessVldOperation = HAL_hsusb_GetCoreBooleanDefault;
  this->SetSessVldOperation = HAL_hsusb_SetCoreBooleanDefault;
  this->GetSessVldStatus = HAL_hsusb_GetCoreBooleanDefault;
  this->SetSessVldStatus = HAL_hsusb_SetCoreBooleanDefault;
  this->GetSessVldChangeStatus = HAL_hsusb_GetCoreBooleanDefault;
  this->SetSessVldChangeStatus = HAL_hsusb_SetCoreBooleanDefault;
  this->GetSessVldChangeInterrupt = HAL_hsusb_GetCoreBooleanDefault;
  this->SetSessVldChangeInterrupt = HAL_hsusb_SetCoreBooleanDefault;
  this->GetSysClkSw = HAL_hsusb_GetCoreBooleanDefault;
  this->SetSysClkSw = HAL_hsusb_SetCoreBooleanDefault;
  this->GetResumeEndInterrupt = HAL_hsusb_GetCoreBooleanDefault;
  this->SetResumeEndInterrupt = HAL_hsusb_SetCoreBooleanDefault;
  this->GetSuspendWrongOpmode = HAL_hsusb_GetCoreBooleanDefault;
  this->SetSuspendWrongOpmode = HAL_hsusb_SetCoreBooleanDefault;
  this->GetNoSofRxFifoFull = HAL_hsusb_GetCoreBooleanDefault;
  this->SetNoSofRxFifoFull = HAL_hsusb_SetCoreBooleanDefault;
  this->GetIsoInZlpPrime = HAL_hsusb_GetCoreBooleanDefault;
  this->SetIsoInZlpPrime = HAL_hsusb_SetCoreBooleanDefault;
  this->GetDatalinePulsingReset = HAL_hsusb_GetCoreBooleanDefault;
  this->SetDatalinePulsingReset = HAL_hsusb_SetCoreBooleanDefault;
  this->GetDevicePortChirpKTime = HAL_hsusb_GetCoreBooleanDefault;
  this->SetDevicePortChirpKTime = HAL_hsusb_SetCoreBooleanDefault;
  this->GetDevicePortPreChirpKTime = HAL_hsusb_GetCoreBooleanDefault;
  this->SetDevicePortPreChirpKTime = HAL_hsusb_SetCoreBooleanDefault;
  this->GetSessVldControl = HAL_hsusb_GetCoreBooleanDefault;
  this->SetSessVldControl = HAL_hsusb_SetCoreBooleanDefault;

  this->GetTxEpPrimeL1Exit       = HAL_hsusb_GetTxEpPrimeL1Default;
  this->SetTxEpPrimeL1Exit       = HAL_hsusb_SetTxEpPrimeL1Default;
  this->GetTxEpPrimeL1Enable     = HAL_hsusb_GetTxEpPrimeL1Default;
  this->SetTxEpPrimeL1Enable     = HAL_hsusb_SetTxEpPrimeL1Default;
  this->GetL1PllPwrDwnEnable     = HAL_hsusb_GetCoreBooleanDefault;
  this->SetL1PllPwrDwnEnable     = HAL_hsusb_SetCoreBooleanDefault;
  this->GetL1PhyLPMEnable        = HAL_hsusb_GetCoreBooleanDefault;
  this->SetL1PhyLPMEnable        = HAL_hsusb_SetCoreBooleanDefault;
  this->GetL1GateAhbClkEnable    = HAL_hsusb_GetCoreBooleanDefault;
  this->SetL1GateAhbClkEnable    = HAL_hsusb_SetCoreBooleanDefault;
  this->GetL1GateFsXcvrClkEnable = HAL_hsusb_GetCoreBooleanDefault;
  this->SetL1GateFsXcvrClkEnable = HAL_hsusb_SetCoreBooleanDefault;
  this->GetL1GateSysClkEnable    = HAL_hsusb_GetCoreBooleanDefault;
  this->SetL1GateSysClkEnable    = HAL_hsusb_SetCoreBooleanDefault;
  this->GetL1GateXcvrClkEnable    = HAL_hsusb_GetCoreBooleanDefault;
  this->SetL1GateXcvrClkEnable    = HAL_hsusb_SetCoreBooleanDefault;
  this->GetL1RemoteWakeupEnable  = HAL_hsusb_GetCoreBooleanDefault;
  this->SetL1RemoteWakeupEnable  = HAL_hsusb_SetCoreBooleanDefault;
  this->GetL1LPMEnable           = HAL_hsusb_GetCoreBooleanDefault;
  this->SetL1LPMEnable           = HAL_hsusb_SetCoreBooleanDefault;
  this->GetL1PllTurnoffMinHird   = HAL_hsusb_GetCoreBooleanDefault;
  this->SetL1PllTurnoffMinHird   = HAL_hsusb_SetCoreBooleanDefault;
  this->GetL1LongEntryCount      = HAL_hsusb_GetUint16Default;
  this->GetL1ShortEntryCount     = HAL_hsusb_GetUint16Default;
  this->ClearL1EntryCounts       = HAL_hsusb_ClearL1EntryCountsDefault;
  this->GetL1RemoteWakeupTime    = HAL_hsusb_GetCoreBooleanDefault;
  this->SetL1RemoteWakeupTime  = HAL_hsusb_SetCoreBooleanDefault;
  this->GetL1FPR                 = HAL_hsusb_GetCoreBooleanDefault;
  this->SetL1FPR                 = HAL_hsusb_SetCoreBooleanDefault;
  this->GetHSICClkPllBypassNL    = HAL_hsusb_GetCoreBooleanDefault;
  this->GetHSICClkPllReset       = HAL_hsusb_GetCoreBooleanDefault;
  this->GetHSICClkGate           = HAL_hsusb_GetCoreBooleanDefault;
  this->GetSysClkGate            = HAL_hsusb_GetCoreBooleanDefault;
  this->GetAHBClkGate            = HAL_hsusb_GetCoreBooleanDefault;
  this->GetL1State               = HAL_hsusb_GetCoreBooleanDefault;
  this->GetDebugL1Enable         = HAL_hsusb_GetCoreBooleanDefault;
  this->SetDebugL1Enable         = HAL_hsusb_SetCoreBooleanDefault;
  this->GetBRemoteWake           = HAL_hsusb_GetCoreBooleanDefault;
  this->SetBRemoteWake           = HAL_hsusb_SetCoreBooleanDefault;
  this->GetHIRD                  = HAL_hsusb_GetUint8Default;
  this->SetHIRD                  = HAL_hsusb_SetUint8Default;
}

/*
===========================================================================

FILE:         HAL_hsusb_impl.c

DESCRIPTION:  
This is the HAL implementation for the HS-USB core.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/hal/src/HAL_hsusb_impl.c#1 $

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

#include "HAL_hsusb_impl.h"
#include "qurt_interrupt.h"

/* -----------------------------------------------------------------------
**                           EXTERNS
** ----------------------------------------------------------------------- */



/*
* HS-USB Cores' base addresses.
*/
extern uint32 usb_core_base[];

/*
* HS-USB HAL target-specific Constructor.
*/
extern void HAL_hsusb_ConstructImplInstance
(
  HAL_HSUSB_CoresType* 
);

/* -----------------------------------------------------------------------
Constants and Macros
** ----------------------------------------------------------------------- */
/*HS-USB HAL CoreInfo table size */
#define HAL_HSUSB_MAX_CORES_QTY 3

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */
static unsigned long hsu_hal_intlock_flag;

/* ChipIdea and Legacy core interface */
static HAL_HSUSB_CoreIfType HAL_HSUSB_CoreIf[2];

/* USB core 0 ports */
static HAL_HSUSB_PortInfoType HAL_HSUSB_PortInfoCore0[1];
/* USB core 1 ports */
static HAL_HSUSB_PortInfoType HAL_HSUSB_PortInfoCore1[1];
/* USB core 2 ports */
static HAL_HSUSB_PortInfoType HAL_HSUSB_PortInfoCore2[1];

/*HS-USB HAL CoreInfo table */
static HAL_HSUSB_CoreInfoType HAL_HSUSB_CoreInfo[HAL_HSUSB_MAX_CORES_QTY];

/* HS-USB HAL Cores object */
HAL_HSUSB_CoresType HAL_HSUSB_CoresObj =
{
  &HAL_HSUSB_CoreIf[0],
  &HAL_HSUSB_CoreInfo[0],
  0,
  FALSE
};

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_hsusb_Init
**
**  Description:
**    This function initializes the HS-USB core Hardware prior 
**    to the first usage after power-up or a call to HAL_hsusb_Reset().
**    
**  Dependencies:
**    None.
**
**  Parameters:
**    None.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_Init(void)
{
  if (!HAL_HSUSB_CoresObj.Initialized)
  {
    uint8 instance;

    HAL_HSUSB_CoresObj.CoreInfo[0].PortInfo = &HAL_HSUSB_PortInfoCore0[0];
    HAL_HSUSB_CoresObj.CoreInfo[1].PortInfo = &HAL_HSUSB_PortInfoCore1[0];
    HAL_HSUSB_CoresObj.CoreInfo[2].PortInfo = &HAL_HSUSB_PortInfoCore2[0];
    HAL_hsusb_ConstructImplInstance(&HAL_HSUSB_CoresObj);
    for (instance=0; instance<HAL_HSUSB_MAX_CORES_QTY; instance++)
    {
      HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress = usb_core_base[instance];
      HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = (usb_core_base[instance] == 0)
        ? HAL_HSUSB_NotSupported
        : HAL_HSUSB_Success;
    }
    HAL_HSUSB_CoresObj.Initialized = TRUE;
  }
}

/* ===========================================================================
**
**  HAL_hsusb_Reset
**
**  Description:
**    This function is intended to return the HS-USB hardware to its 
**    power-on status.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    None.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_Reset(void)
{
  uint8 instance;

  for (instance=0; instance<HAL_hsusb_SupportedCoresQty() ; instance++)
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].If->Reset(instance);
  }
  HAL_HSUSB_CoresObj.Initialized = FALSE;
}


/* ===========================================================================
**
**  HAL_hsusb_SupportedCoresQty
**
**  Description:
**    This function reports the HS-USB cores’ amount.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    None.
**
**  Return:
**     The number of the HS-USB cores attached to the target.
**
** ======================================================================== */
uint8 HAL_hsusb_SupportedCoresQty(void)
{
  return HAL_HSUSB_CoresObj.MaxCoresQty;
}

/* ===========================================================================
**
**  HAL_hsusb_HWversion
**
**  Description:
**    This function reports the HW version for a given HS-USB core.
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
**    The HW version for a given HS-USB core.
**
** ======================================================================== */
uint16 HAL_hsusb_HWversion(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->Version;
}

/* ===========================================================================
**
**  HAL_hsusb_SupportedPortsQty
**
**  Description:
**    This function reports the HS-USB core ports' amount.
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
**     The number of the ports attached to a HS-USB core.
**
** ======================================================================== */
uint8 HAL_hsusb_SupportedPortsQty(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty;
}

/* ===========================================================================
**
**  HAL_hsusb_SupportedPhyType
**
**  Description:
**    This function reports which PHY is supported by a HS-USB core port.
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
**
**  Return:
**    The PHY supported by a HS-USB core port.
**
** ======================================================================== */
HAL_HSUSB_PhyType* HAL_hsusb_SupportedPhyType(uint8 instance, uint8 port)
{
  if (port >= HAL_hsusb_SupportedPortsQty(instance))
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_WrongPort;
    return NULL;
  }

  return &HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[port].PhyType;
}

/* ===========================================================================
**
**  HAL_hsusb_GetLastError
**
**  Description:
**    This function gets the last error code of the HS-USB core recorded
**    from the HS-USB HAL failure. It is intended for debug purposes.
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
**     The last error code of the HS-USB core recorded from the HS-USB HAL failure.
**
** ======================================================================== */
HAL_HSUSB_ErrorType HAL_hsusb_GetLastError(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].LastError;
}

/* ===========================================================================
**
**  HAL_hsusb_Read8
**
**  Description:
**    This function reads 8 bits from the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**
**  Return:
**    The read data.
**
** ======================================================================== */
uint8 HAL_hsusb_Read8(uint8 instance, uint32 offset)
{
  return inp(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset);
}

/* ===========================================================================
**
**  HAL_hsusb_Read16
**
**  Description:
**    This function reads 16 bits from the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**
**  Return:
**    The read data.
**
** ======================================================================== */
uint16 HAL_hsusb_Read16(uint8 instance, uint32 offset)
{
  return inpw(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset);
}

/* ===========================================================================
**
**  HAL_hsusb_Read32
**
**  Description:
**    This function reads 32 bits from the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**
**  Return:
**    The read data.
**
** ======================================================================== */
uint32 HAL_hsusb_Read32(uint8 instance, uint32 offset)
{
  return inpdw(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset);
}

/* ===========================================================================
**
**  HAL_hsusb_Write8
**
**  Description:
**    This function writes 8 bits to the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    data     - Determines the write data.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_Write8(uint8 instance, uint32 offset, uint8 data)
{
  outp(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset, data);
}

/* ===========================================================================
**
**  HAL_hsusb_Write16
**
**  Description:
**    This function writes 16 bits to the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    data     - Determines the write data.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_Write16(uint8 instance, uint32 offset, uint16 data)
{
  outpw(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset, data);
}

/* ===========================================================================
**
**  HAL_hsusb_Write32
**
**  Description:
**    This function writes 32 bits to the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    data     - Determines the write data.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_Write32(uint8 instance, uint32 offset, uint32 data)
{
  outpdw(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset, data);
}

/* ===========================================================================
**
**  HAL_hsusb_GetBits32
**
**  Description:
**    This function gets bits from the HS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be get.
**
**  Return:
**    The read data masked with <bits>.
**
** ======================================================================== */
uint32 HAL_hsusb_GetBits32(uint8 instance, uint32 offset, uint32 bits)
{
  return (inpdw(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset) & bits);
}

/* ===========================================================================
**
**  HAL_hsusb_SetBits32
**
**  Description:
**    This function sets bits in the HS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be set.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetBits32(uint8 instance, uint32 offset, uint32 bits)
{
  uint32 address;
  uint32 data;

  address = HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset;
  INTLOCK_SAVE(hsu_hal_intlock_flag)
  data = inpdw(address);
  outpdw(address, data | bits);
  INTFREE_RESTORE(hsu_hal_intlock_flag)
}

/* ===========================================================================
**
**  HAL_hsusb_ClearBits32
**
**  Description:
**    This function clears bits in the HS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be cleared.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_ClearBits32(uint8 instance, uint32 offset, uint32 bits)
{
  uint32 address;
  uint32 data;

  address = HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset;
  INTLOCK_SAVE(hsu_hal_intlock_flag)
  data = inpdw(address);
  outpdw(address, data & ~bits);
  INTFREE_RESTORE(hsu_hal_intlock_flag)
}

/* ===========================================================================
**
**  HAL_hsusb_ClearAndSetBits32
**
**  Description:
**    This function clears and then sets bits in the HS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance  - Determines which HS-USB core to use. 
**              This value should be one less than reported by 
**              the HAL_hsusb_SupportedCoresQty().
**    offset    - Determines the register offset. 
**              This value must be aligned at the 32 bit boundary and 
**              in the address ranges defined by the HW specification.
**    clearBits - Determines the bits to be cleared.
**    setBits   - Determines the bits to be set.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_ClearAndSetBits32(uint8 instance, uint32 offset, uint32 clearBits, uint32 setBits)
{
  uint32 address;
  uint32 data;

  address = HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset;
  INTLOCK_SAVE(hsu_hal_intlock_flag)
  data = inpdw(address);
  outpdw(address, (data & ~clearBits) | setBits);
  INTFREE_RESTORE(hsu_hal_intlock_flag)
}

/* ===========================================================================
**
**  HAL_hsusb_ReadUlpiPhy
**
**  Description:
**    This function reads from a ULPI PHY register of the HS-USB core port.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
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
**    HAL_HSUSB_Success           - the returned <data> value is valid.
**    HAL_HSUSB_WrongPort         – the <port> is out of scope reported 
**                                  by the HAL_hsusb_SupportedPortsQty().
**    HAL_HSUSB_WrongPhyInterface – the Phy Interface is not ULPI.
**    HAL_HSUSB_TimeoutExpired    – the maximal timeout has been expired.
**
** ======================================================================== */
HAL_HSUSB_ErrorType HAL_hsusb_ReadUlpiPhy(uint8 instance, uint8 port, uint8 reg, uint8* data)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->ReadUlpiPhy(instance, port, reg, data);
}

/* ===========================================================================
**
**  HAL_hsusb_WriteUlpiPhy
**
**  Description:
**    This function writes to a ULPI PHY register of the HS-USB core port.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
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
**    HAL_HSUSB_Success           - the requested <data> is set.
**    HAL_HSUSB_WrongPort         – the <port> is out of scope reported 
**                                  by the HAL_hsusb_SupportedPortsQty().
**    HAL_HSUSB_WrongPhyInterface – the Phy Interface is not ULPI.
**    HAL_HSUSB_TimeoutExpired    – the maximal timeout has been expired.
**
** ======================================================================== */
HAL_HSUSB_ErrorType HAL_hsusb_WriteUlpiPhy(uint8 instance, uint8 port, uint8 reg, uint8 data)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->WriteUlpiPhy(instance, port, reg, data);
}

/* ===========================================================================
**
**  HAL_hsusb_GetAhbBurstMode
**
**  Description:
**    This function reports the mode of the AMBA AHB master interface 
**    m_hburst signal.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
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
HAL_HSUSB_AhbBurstMode HAL_hsusb_GetAhbBurstMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetAhbBurstMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetAhbBurstMode
**
**  Description:
**    This function sets the mode of the AMBA AHB master interface 
**    m_hburst signal.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
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
void HAL_hsusb_SetAhbBurstMode(uint8 instance, HAL_HSUSB_AhbBurstMode eMode)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetAhbBurstMode(instance, eMode);
}

/* ===========================================================================
**
**  HAL_hsusb_GetSe0GlitchFixCtrl
**
**  Description:
**    This function reports whether the SE0 glitch fix mechanism is activated.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The SE0 glitch fix mechanism is activated.
**    FALSE   - The SE0 glitch fix mechanism is not activated.
**
** ======================================================================== */
boolean HAL_hsusb_GetSe0GlitchFixCtrl(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetSe0GlitchFixCtrl(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetSe0GlitchFixCtrl
**
**  Description:
**    This function activates/disactivate the SE0 glitch fix mechanism.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the requested action.
**             Values are:
**             TRUE  - Activate the SE0 glitch fix mechanism
**             FALSE - Deactivate the SE0 glitch fix mechanism
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetSe0GlitchFixCtrl(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetSe0GlitchFixCtrl(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetFs3Wire2WireSelect
**
**  Description:
**    This function reports the two wire interface on the fs_dat and 
**    fs_se0 pins mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 3 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The two wire interface on the fs_dat and 
**              fs_se0 pins is enabled.
**    FALSE   - The two wire interface on the fs_dat and 
**              fs_se0 pins is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetFs3Wire2WireSelect(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetFs3Wire2WireSelect(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetFs3Wire2WireSelect
**
**  Description:
**    This function enables/disables the two wire interface on the fs_dat and 
**    fs_se0 pins.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 3 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the requested action.
**             Values are:
**             TRUE  - Enable the two wire interface on the fs_dat and 
**                     fs_se0 pins
**             FALSE - Disable the two wire interface on the fs_dat and 
**                     fs_se0 pins
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetFs3Wire2WireSelect(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetFs3Wire2WireSelect(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetXtorBypass
**
**  Description:
**    This function reports whether the AHB Transactor is bypassed.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The AHB Transactor is bypassed.
**    FALSE   - The AHB Transactor is not bypassed.
**
** ======================================================================== */
boolean HAL_hsusb_GetXtorBypass(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetXtorBypass(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetXtorBypass
**
**  Description:
**    This function sets the AHB Transactor bypass mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the AHB Transactor bypass mode.
**             Values are:
**             TRUE  - Bypass the AHB Transactor
**             FALSE - Do not bypass the AHB Transactor
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetXtorBypass(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetXtorBypass(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetPostedWrites
**
**  Description:
**    This function reports the USB Core AHB posted data writes mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 only.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE  - The AHB master interface enables posted data writes
**    FALSE - The AHB master interface disables posted data writes
**
** ======================================================================== */
boolean HAL_hsusb_GetPostedWrites(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetPostedWrites(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetPostedWrites
**
**  Description:
**    This function sets the USB Core AHB posted data writes mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 only.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB Core AHB posted data writes mode.
**             Values are:
**             TRUE  - The AHB master interface enables posted data writes
**             FALSE - The AHB master interface disables posted data writes
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetPostedWrites(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetPostedWrites(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetGrantStolen
**
**  Description:
**    This function reports whether the arbiter removed the hgrant signal 
**    prior to completing a transaction.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The arbiter removed the hgrant signal 
**              prior to completing a transaction.
**    FALSE   - The arbiter doesn’t removed the hgrant signal 
**              prior to completing a transaction.
**
** ======================================================================== */
boolean HAL_hsusb_GetGrantStolen(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetGrantStolen(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetGrantStolen
**
**  Description:
**    This function sets the arbiter removing the hgrant signal 
**    prior to completing a transaction.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the arbiter removing the hgrant signal 
**             prior to completing a transaction mode.
**             Values are:
**             TRUE  - Enable the arbiter removed the hgrant signal 
**                   prior to completing a transaction
**             FALSE - Disable the arbiter doesn’t removed the hgrant signal 
**                   prior to completing a transaction
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetGrantStolen(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetGrantStolen(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetHprotMode
**
**  Description:
**    This function reports the HPROT signal mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
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
HAL_HSUSB_HprotMode HAL_hsusb_GetHprotMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetHprotMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetHprotMode
**
**  Description:
**    This function sets the HPROT signal mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
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
void HAL_hsusb_SetHprotMode(uint8 instance, HAL_HSUSB_HprotMode eMode)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetHprotMode(instance, eMode);
}

/* ===========================================================================
**
**  HAL_hsusb_GetIncrOverride
**
**  Description:
**    This function reports the USB Core INCR bursts transformation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE  - The all INCR bursts from the USB Core will be 
**            internally transformed into SINGLE transfers
**    FALSE - If the USB Core issues an INCR burst, it will 
**            propagate to the external master AHB port
**
** ======================================================================== */
boolean HAL_hsusb_GetIncrOverride(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetIncrOverride(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetIncrOverride
**
**  Description:
**    This function sets the USB Core INCR bursts transformation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB Core INCR bursts transformation.
**             Values are:
**             TRUE  - The all INCR bursts from the USB Core will be 
**                   internally transformed into SINGLE transfers
**             FALSE - If the USB Core issues an INCR burst, it will 
**                   propagate to the external master AHB port
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetIncrOverride(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetIncrOverride(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetAsyncBridgesBypass
**
**  Description:
**    This function reports the asynchronous bridge bypass mode on 
**    the master AHB interface.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The asynchronous bridge on the master AHB interface is bypassed.
**    FALSE   - The asynchronous bridge on the master AHB interface issued.
**
** ======================================================================== */
boolean HAL_hsusb_GetAsyncBridgesBypass(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetAsyncBridgesBypass(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetAsyncBridgesBypass
**
**  Description:
**    This function sets the asynchronous bridge bypass mode on 
**    the master AHB interface.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the asynchronous bridge bypass mode on 
**             the master AHB interface.
**             Values are:
**             TRUE  - The asynchronous bridge on the master AHB interface 
**                   is bypassed
**             FALSE - The asynchronous bridge on the master AHB interface 
**                   issued
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetAsyncBridgesBypass(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetAsyncBridgesBypass(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetPeDpTxFifoIdleForce
**
**  Description:
**    This function reports the PE_DP_TXFIO_IDLE_FORCE bit status in
**    GEN_CONFIG register.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - PE_DP_TXFIO_IDLE_FORCE bit is set (default)
**    FALSE   - PE_DP_TXFIO_IDLE_FORCE bit is cleared
**
** ======================================================================== */
boolean HAL_hsusb_GetPeDpTxFifoIdleForce(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetPeDpTxFifoIdleForce(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetPeDpTxFifoIdleForce
**
**  Description:
**    This function sets the PE_DP_TXFIO_IDLE_FORCE bit status in
**    GEN_CONFIG register.  Clear this bit to 0 to prevent TX data corruption.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the PE_DP_TXFIO_IDLE_FORCE status
**             Values are:
**             TRUE  - PE_DP_TXFIO_IDLE_FORCE bit to set to 1 (default)
**             FALSE - PE_DP_TXFIO_IDLE_FORCE bit cleared to 0
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetPeDpTxFifoIdleForce(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetPeDpTxFifoIdleForce(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetTestBusSelector
**
**  Description:
**    This function reports which group of the internal signals is 
**    routed to the test bus.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
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
HAL_HSUSB_TestBusSelectorType HAL_hsusb_GetTestBusSelector(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetTestBusSelector(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetTestBusSelector
**
**  Description:
**    This function sets which group of the internal signals is 
**    routed to the test bus.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
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
void HAL_hsusb_SetTestBusSelector(uint8 instance, HAL_HSUSB_TestBusSelectorType eMode)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetTestBusSelector(instance, eMode);
}

/* ===========================================================================
**
**  HAL_hsusb_GetTxBufCollisionMode
**
**  Description:
**    This function reports the TX buffer controller mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 4 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The TX buffer controller direct mode.
**    FALSE   - The TX buffer controller revert mode.
**
** ======================================================================== */
boolean HAL_hsusb_GetTxBufCollisionMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetTxBufCollisionMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetTxBufCollisionMode
**
**  Description:
**    This function sets the TX buffer controller mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 4 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the TX buffer controller mode.
**             Values are:
**             TRUE  - The TX buffer controller direct mode
**             FALSE - The TX buffer controller revert mode
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetTxBufCollisionMode(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetTxBufCollisionMode(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetStreamRxBypassMode
**
**  Description:
**    This function reports the RX streaming bypass mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The RX streaming bypass is enabled.
**    FALSE   - The RX streaming bypass is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetStreamRxBypassMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetStreamRxBypassMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetStreamRxBypassMode
**
**  Description:
**    This function sets the RX streaming bypass mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the RX streaming bypass mode.
**             Values are:
**             TRUE  - Enable the RX streaming bypassing
**             FALSE - Disable the RX streaming bypassing
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetStreamRxBypassMode(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetStreamRxBypassMode(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetStreamTxAutoMode
**
**  Description:
**    This function reports the TX streaming automatic determination mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The TX streaming automatic determination is enabled.
**    FALSE   - The TX streaming automatic determination s is disabled
**
** ======================================================================== */
boolean HAL_hsusb_GetStreamTxAutoMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetStreamTxAutoMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetStreamTxAutoMode
**
**  Description:
**    This function sets the TX streaming automatic determination mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the TX streaming automatic determination mode.
**             Values are:
**             TRUE  - Enable TX streaming automatic determination
**             FALSE - Disable TX streaming automatic determination
**
**  Return:
**    TRUE    - the requested <state> is set.
**    FALSE   - the requested <state> is not set.
**
** ======================================================================== */
void HAL_hsusb_SetStreamTxAutoMode(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetStreamTxAutoMode(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetHostSimTimersStdMode
**
**  Description:
**    This function reports the timers used for the USB reset on the ULPI mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The timers used for the USB reset on the ULPI are enabled.
**    FALSE   - The timers used for the USB reset on the ULPI are disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetHostSimTimersStdMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetHostSimTimersStdMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetHostSimTimersStdMode
**
**  Description:
**    This function sets the timers used for the USB reset on the ULPI mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the timers used for the USB reset on the ULPI mode.
**             Values are:
**             TRUE  - Enable the timers used for the USB reset on the ULPI
**             FALSE - Disable the timers used for the USB reset on the ULPI
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetHostSimTimersStdMode(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetHostSimTimersStdMode(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetHostSimTimersSuspMode
**
**  Description:
**    This function reports the timers used for the USB suspend process mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The timers used for the USB suspend process short 
**              for faster simulation and ATE time.
**    FALSE   - The timers used for the USB suspend process are 
**              according to the USB specification.
**
** ======================================================================== */
boolean HAL_hsusb_GetHostSimTimersSuspMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetHostSimTimersSuspMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetHostSimTimersSuspMode
**
**  Description:
**    This function sets the timers used for the USB suspend process mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the timers used for the USB suspend process mode.
**             Values are:
**             TRUE  - The timers used for the USB suspend process short 
**                     for faster simulation and ATE time
**             FALSE - The timers used for the USB suspend process are 
**                     according to the USB specification
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetHostSimTimersSuspMode(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetHostSimTimersSuspMode(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetDevicePEStateReset
**
**  Description:
**    This function reports whether an automatic reset of Device PE State is enabled.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - An automatic reset of Device PE State is enabled.
**    FALSE   - An automatic reset of Device PE State is enabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetDevicePEStateReset(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetDevicePEStateReset(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetDevicePEStateReset
**
**  Description:
**    This function enables/disables an automatic reset of Device PE State.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines an automatic reset of Device PE State.
**             Values are:
**             TRUE  - Enables an automatic reset of Device PE State
**             FALSE - Disables an automatic reset of Device PE State
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetDevicePEStateReset(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetDevicePEStateReset(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetIsochronousBugFix
**
**  Description:
**    This function reports whether the Isochronous bug fix is enabled.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - Tthe Isochronous bug fix is enabled.
**    FALSE   - The Isochronous bug fix is enabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetIsochronousBugFix(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetIsochronousBugFix(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetIsochronousBugFix
**
**  Description:
**    This function enables/disables the Isochronous bug fix.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the Isochronous bug fix.
**             Values are:
**             TRUE  - Enables the Isochronous bug fix
**             FALSE - Disables the Isochronous bug fix
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetIsochronousBugFix(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetIsochronousBugFix(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetAHBBufferableDataAccess
**
**  Description:
**    This function reports whether the AHB Bufferable data access is enabled.
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
**    TRUE    - The AHB Bufferable data access is enabled
**    FALSE   - The AHB Bufferable data access is disabled
**
** ======================================================================== */
boolean HAL_hsusb_GetAHBBufferableDataAccess(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetAHBBufferableDataAccess(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetAHBBufferableDataAccess
**
**  Description:
**    This function enables/disables the AHB Bufferable data access.
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
void HAL_hsusb_SetAHBBufferableDataAccess(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetAHBBufferableDataAccess(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetLegacyMode
**
**  Description:
**    This function reports whether the Legacy mode is enabled.
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
**    TRUE    - The Legacy mode is enabled
**    FALSE   - The Legacy mode is disabled
**
** ======================================================================== */
boolean HAL_hsusb_GetLegacyMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetLegacyMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetLegacyMode
**
**  Description:
**    This function sets the Legacy mode.
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
void HAL_hsusb_SetLegacyMode(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetLegacyMode(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetEndpointPipeID
**
**  Description:
**    This function reports the endpoint pipe number.
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
uint8 HAL_hsusb_GetEndpointPipeID(uint8 instance, uint8 ep, boolean direction)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetEndpointPipeID(instance, ep, direction);
}

/* ===========================================================================
**
**  HAL_hsusb_SetEndpointPipeID
**
**  Description:
**    This function sets the endpoint pipe number.
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
void HAL_hsusb_SetEndpointPipeID(uint8 instance, uint8 ep, boolean direction, uint8 id)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetEndpointPipeID(instance, ep, direction, id);
}

/* ===========================================================================
**
**  HAL_hsusb_GetSessVldOperation
**
**  Description:
**    This function reports whether the Link Controller operation
**    is enabled after switching interface from Serial to ULPI.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The Link Controller operation is enabled
**    FALSE   - The Link Controller operation is disabled
**
** ======================================================================== */
boolean HAL_hsusb_GetSessVldOperation(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetSessVldOperation(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetSessVldOperation
**
**  Description:
**    This function enables/disables the Link Controller operation
**    after switching interface from Serial to ULPI.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the Link Controller operation.
**             Values are:
**             TRUE  - Enable the Link Controller operation
**             FALSE - Disable the Link Controller operation
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetSessVldOperation(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetSessVldOperation(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetSessVldStatus
**
**  Description:
**    This function reports whether the SESS_VLD status of PHY.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The SESS_VLD status of PHY is set
**    FALSE   - The SESS_VLD status of PHY is not set
**
** ======================================================================== */
boolean HAL_hsusb_GetSessVldStatus(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetSessVldStatus(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetSessVldStatus
**
**  Description:
**    This function sets/clears the SESS_VLD status of PHY.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the SESS_VLD status of PHY.
**             Values are:
**             TRUE  - Set the SESS_VLD status of PHY
**             FALSE - Clear the SESS_VLD status of PHY
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetSessVldStatus(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetSessVldStatus(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetSessVldChangeStatus
**
**  Description:
**    This function reports whether the SESS_VLD status change of PHY.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The SESS_VLD status of PHY is changed
**    FALSE   - The SESS_VLD status of PHY is not changed
**
** ======================================================================== */
boolean HAL_hsusb_GetSessVldChangeStatus(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetSessVldChangeStatus(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetSessVldChangeStatus
**
**  Description:
**    This function sets/clears the SESS_VLD status change of PHY.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the SESS_VLD status change of PHY.
**             Values are:
**             TRUE  - Set the SESS_VLD status change of PHY
**             FALSE - Clear the SESS_VLD status change of PHY
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetSessVldChangeStatus(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetSessVldChangeStatus(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetSessVldChangeInterrupt
**
**  Description:
**    This function reports whether the SESS_VLD status change of PHY
**    interrupt is enabled.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The SESS_VLD status change of PHY interrupt is enabled
**    FALSE   - The SESS_VLD status change of PHY interrupt is disabled
**
** ======================================================================== */
boolean HAL_hsusb_GetSessVldChangeInterrupt(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetSessVldChangeInterrupt(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetSessVldChangeInterrupt
**
**  Description:
**    This function enables/disables the SESS_VLD status change of PHY
**    interrupt.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the SESS_VLD status change of PHY interrupt state.
**             Values are:
**             TRUE  - Enable the SESS_VLD status change of PHY interrupt
**             FALSE - Disable the SESS_VLD status change of PHY interrupt
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetSessVldChangeInterrupt(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetSessVldChangeInterrupt(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetSysClkSw
**
**  Description:
**    This function reports whether the USB core is voting for USB_SYSTEM_CLK.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The USB core is voting for USB_SYSTEM_CLK
**    FALSE   - The USB core doesn't request USB_SYSTEM_CLK
**
** ======================================================================== */
boolean HAL_hsusb_GetSysClkSw(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetSysClkSw(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetSysClkSw
**
**  Description:
**    This function enables/disables the USB core voting for USB_SYSTEM_CLK.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB core voting for USB_SYSTEM_CLK.
**             Values are:
**             TRUE  - Enable the USB core voting for USB_SYSTEM_CLK
**             FALSE - Disable the USB core voting for USB_SYSTEM_CLK
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetSysClkSw(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetSysClkSw(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetResumeEndInterrupt
**
**  Description:
**    This function reports whether the port change interrupt
**    at resume end in Host mode is enabled.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The port change interrupt at resume end in Host mode is enabled
**    FALSE   - The port change interrupt at resume end in Host mode is disabled
**
** ======================================================================== */
boolean HAL_hsusb_GetResumeEndInterrupt(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetResumeEndInterrupt(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetResumeEndInterrupt
**
**  Description:
**    This function enables/disables the port change interrupt at resume end in Host mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB core voting for USB_SYSTEM_CLK.
**             Values are:
**             TRUE  - Enable the port change interrupt at resume end in Host mode
**             FALSE - Disable the port change interrupt at resume end in Host
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetResumeEndInterrupt(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetResumeEndInterrupt(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetSuspendWrongOpmode
**
**  Description:
**    This function reports whether the USB core asserts opmode with
**    the wrong value when enters a Suspend state.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The USB core doesn't asserts opmode with the wrong value when enters a Suspend state
**    FALSE   - The USB core asserts opmode with the wrong value when enters a Suspend state
**
** ======================================================================== */
boolean HAL_hsusb_GetSuspendWrongOpmode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetSuspendWrongOpmode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetSuspendWrongOpmode
**
**  Description:
**    This function enables/disables the USB core to assert opmode with
**    the wrong value when enters a Suspend state.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB core assertion of opmode with
**             the wrong value when enters a Suspend state.
**             Values are:
**             TRUE  - Disable the USB core to assert opmode with the wrong value when enters a Suspend state
**             FALSE - Enable the USB core to assert opmode with the wrong value when enters a Suspend state
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetSuspendWrongOpmode(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetSuspendWrongOpmode(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetNoSofRxFifoFull
**
**  Description:
**    This function reports whether the USB core waits until
**    the RX Fifo becomes not full to generate SOF when it is full.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The USB core waits until the RX Fifo becomes not full
**              to generate SOF when it is full
**    FALSE   - The USB core generates SOF when the RX Fifo becomes full
**
** ======================================================================== */
boolean HAL_hsusb_GetNoSofRxFifoFull(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetNoSofRxFifoFull(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetNoSofRxFifoFull
**
**  Description:
**    This function enables/disables the USB core to wait until
**    the RX Fifo becomes not full to generate SOF when it is full.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB core waiting until the RX Fifo becomes not full
**             to generate SOF when it is full.
**             Values are:
**             TRUE  - Disable the USB core to wait until the RX Fifo becomes not full
**                     to generate SOF when it is full
**             FALSE - Enable the USB core to wait until the RX Fifo becomes not full
**                     to generate SOF when it is full
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetNoSofRxFifoFull(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetNoSofRxFifoFull(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetIsoInZlpPrime
**
**  Description:
**    This function reports whether the USB core replies with a zero length packet
**    before the last token IN is received when using ISO IN endpoints
**    with MULT=3 and low bandwidth system bus access.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The USB core replies with a zero length packet
**              before the last token IN is received when using ISO IN endpoints
**              with MULT=3 and low bandwidth system bus access
**    FALSE   - The USB USB core doesn't repliy with a zero length packet
**              before the last token IN is received when using ISO IN endpoints
**              with MULT=3 and low bandwidth system bus access
**
** ======================================================================== */
boolean HAL_hsusb_GetIsoInZlpPrime(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetIsoInZlpPrime(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetIsoInZlpPrime
**
**  Description:
**    This function enables/disables the USB core to reply with a zero length packet
**    before the last token IN is received when using ISO IN endpoints
**    with MULT=3 and low bandwidth system bus access.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB core reply with a zero length packet
**             before the last token IN is received when using ISO IN endpoints
**             with MULT=3 and low bandwidth system bus access.
**             Values are:
**             TRUE  - Enable the USB core to reply with a zero length packet
**                     before the last token IN is received when using ISO IN endpoints
**                     with MULT=3 and low bandwidth system bus access
**             FALSE - Disable the USB core to reply with a zero length packet
**                     before the last token IN is received when using ISO IN endpoints
**                     with MULT=3 and low bandwidth system bus access
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetIsoInZlpPrime(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetIsoInZlpPrime(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetDatalinePulsingReset
**
**  Description:
**    This function reports whether the USB core recognizes the attached Device
**    attempts to perform a Session Request Protocol by using Data-line Pulsing
**    in Host mode when VBUS is turned off.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The USB core recognizes the attached Device attempts
**              to perform a Session Request Protocol by using Data-line Pulsing
**              in Host mode when VBUS is turned off
**    FALSE   - The USB core doesn't recognize the attached Device attempts
**              to perform a Session Request Protocol by using Data-line Pulsing
**              in Host mode when VBUS is turned off
**
** ======================================================================== */
boolean HAL_hsusb_GetDatalinePulsingReset(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetDatalinePulsingReset(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetDatalinePulsingReset
**
**  Description:
**    This function enables/disables the USB core to recognize the attached Device
**    attempts to perform a Session Request Protocol by using Data-line Pulsing
**    in Host mode when VBUS is turned off.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB core recognition of the attached Device
**             attempts to perform a Session Request Protocol by using Data-line Pulsing
**             in Host mode when VBUS is turned off.
**             Values are:
**             TRUE  - Enable the USB core to recognize the attached Device
**                     attempts to perform a Session Request Protocol by using Data-line Pulsing
**                     in Host mode when VBUS is turned off
**             FALSE - Disable the USB core to recognize the attached Device
**                     attempts to perform a Session Request Protocol by using Data-line Pulsing
**                     in Host mode when VBUS is turned off
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetDatalinePulsingReset(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetDatalinePulsingReset(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetDevicePortChirpKTime
**
**  Description:
**    This function reports the Device Port Control State Machine Chirp K time.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The Device Port Control State Machine drives Chirp K for 2 ms
**    FALSE   - The Device Port Control State Machine drives Chirp K for 1 ms
**
** ======================================================================== */
boolean HAL_hsusb_GetDevicePortChirpKTime(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetDevicePortChirpKTime(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetDevicePortChirpKTime
**
**  Description:
**    This function sets the Device Port Control State Machine Chirp K time.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the Device Port Control State Machine Chirp K time.
**             Values are:
**             TRUE  - Set the Device Port Control State Machine Chirp K time to 2 ms
**             FALSE - Set the Device Port Control State Machine Chirp K time to 1 ms
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetDevicePortChirpKTime(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetDevicePortChirpKTime(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetDevicePortPreChirpKTime
**
**  Description:
**    This function reports the Device Port Control State Machine waiting time
**    from USB Reset detection until starting driving Chirp K.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The Device Port Control State Machine waits 1.5 mcs
**              from USB Reset detection until starting driving Chirp K
**    FALSE   - The Device Port Control State Machine waits 2.5 mcs
**              from USB Reset detection until starting driving Chirp K
**
** ======================================================================== */
boolean HAL_hsusb_GetDevicePortPreChirpKTime(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetDevicePortPreChirpKTime(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetDevicePortPreChirpKTime
**
**  Description:
**    This function sets the Device Port Control State Machine waiting time
**    from USB Reset detection until starting driving Chirp K.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the Device Port Control State Machine waiting time
**             from USB Reset detection until starting driving Chirp K.
**             Values are:
**             TRUE  - Set the Device Port Control State Machine to wait 1.5 mcs
**                     from USB Reset detection until starting driving Chirp K
**             FALSE - Set the Device Port Control State Machine to wait 2.5 mcs
**                     from USB Reset detection until starting driving Chirp K
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetDevicePortPreChirpKTime(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetDevicePortPreChirpKTime(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetSessVldControl
**
**  Description:
**    This function reports whether the Link Controller sess_vld signal
**    is controlled by bit 25 of USBCMD register or is received from PHY.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The Link Controller sess_vld signal
**              is controlled by bit 25 of USBCMD register
**    FALSE   - The Link Controller sess_vld signal is received from PHY
**
** ======================================================================== */
boolean HAL_hsusb_GetSessVldControl(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetSessVldControl(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetSessVldControl
**
**  Description:
**    This function sets the Link Controller sess_vld signal control.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 7 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the Link Controller sess_vld signal control.
**             Values are:
**             TRUE  - Set the Link Controller sess_vld signal
**                     to be controlled by bit 25 of USBCMD register
**             FALSE - Set the Link Controller sess_vld signal to be received from PHY
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetSessVldControl(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetSessVldControl(instance, state);
}



boolean HAL_hsusb_GetTxEpPrimeL1Exit(uint8 instance, uint8 ep)
{
  return
    HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetTxEpPrimeL1Exit(instance, ep);
}

void HAL_hsusb_SetTxEpPrimeL1Exit(uint8 instance, uint8 ep, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetTxEpPrimeL1Exit(instance, ep, state);
}


boolean HAL_hsusb_GetTxEpPrimeL1Enable(uint8 instance, uint8 ep)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetTxEpPrimeL1Enable(instance, ep);
}


void HAL_hsusb_SetTxEpPrimeL1Enable(uint8 instance, uint8 ep, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetTxEpPrimeL1Enable(instance, ep, state);
}


boolean HAL_hsusb_GetL1PllPwrDwnEnable(uint8 instance)
{
  return 
    HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1PllPwrDwnEnable(instance);
}


void HAL_hsusb_SetL1PllPwrDwnEnable(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetL1PllPwrDwnEnable(instance, state);
}


boolean HAL_hsusb_GetL1PhyLPMEnable(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1PhyLPMEnable(instance);
}


void HAL_hsusb_SetL1PhyLPMEnable(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetL1PhyLPMEnable(instance, state);
}


boolean HAL_hsusb_GetL1GateAhbClkEnable(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1GateAhbClkEnable(instance);
}


void HAL_hsusb_SetL1GateAhbClkEnable(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetL1GateAhbClkEnable(instance, state);
}


boolean HAL_hsusb_GetL1GateFsXcvrClkEnable(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1GateFsXcvrClkEnable(instance);
}


void HAL_hsusb_SetL1GateFsXcvrClkEnable(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetL1GateFsXcvrClkEnable(instance, state);
}


boolean HAL_hsusb_GetL1GateSysClkEnable(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1GateSysClkEnable(instance);
}


void HAL_hsusb_SetL1GateSysClkEnable(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetL1GateSysClkEnable(instance, state);
}


boolean HAL_hsusb_GetL1GateXcvrClkEnable(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1GateXcvrClkEnable(instance);
}


void HAL_hsusb_SetL1GateXcvrClkEnable(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetL1GateXcvrClkEnable(instance, state);
}


boolean HAL_hsusb_GetL1RemoteWakeupEnable(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1RemoteWakeupEnable(instance);
}


void HAL_hsusb_SetL1RemoteWakeupEnable(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetL1RemoteWakeupEnable(instance, state);
}


boolean HAL_hsusb_GetL1LPMEnable(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1LPMEnable(instance);
}


void HAL_hsusb_SetL1LPMEnable(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetL1LPMEnable(instance, state);
}


uint8 HAL_hsusb_GetL1PllTurnoffMinHird(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1PllTurnoffMinHird(instance);
}

void HAL_hsusb_SetL1PllTurnoffMinHird(uint8 instance, uint8 minHIRD)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetL1PllTurnoffMinHird(instance, minHIRD);
}


uint16 HAL_hsusb_GetL1LongEntryCount(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1LongEntryCount(instance);
}


uint16 HAL_hsusb_GetL1ShortEntryCount(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1ShortEntryCount(instance);
}


void HAL_hsusb_ClearL1EntryCounts(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->ClearL1EntryCounts(instance);
}


uint8 HAL_hsusb_GetL1RemoteWakeupTime(uint8 instance)
{
  return 
    HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1RemoteWakeupTime(instance);
}


void HAL_hsusb_SetL1RemoteWakeupTime(uint8 instance, uint8 minHIRD)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetL1RemoteWakeupTime(instance, minHIRD);
}


boolean HAL_hsusb_GetL1FPR(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1FPR(instance);
}

void HAL_hsusb_SetL1FPR(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetL1FPR(instance, state);
}

boolean HAL_hsusb_GetHSICClkPllBypassNL(uint8 instance)
{
  return 
    HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetHSICClkPllBypassNL(instance);
}

boolean HAL_hsusb_GetHSICClkPllReset(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetHSICClkPllReset(instance);
}


boolean HAL_hsusb_GetHSICClkGate(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetHSICClkGate(instance);
}


boolean HAL_hsusb_GetSysClkGate(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetSysClkGate(instance);
}


boolean HAL_hsusb_GetAHBClkGate(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetAHBClkGate(instance);
}


boolean HAL_hsusb_GetL1State(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetL1State(instance);
}


boolean HAL_hsusb_GetDebugL1Enable(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetDebugL1Enable(instance);
}

void HAL_hsusb_SetDebugL1Enable(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetDebugL1Enable(instance, state);
}


boolean HAL_hsusb_GetBRemoteWake(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetBRemoteWake(instance);
}

void HAL_hsusb_SetBRemoteWake(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetBRemoteWake(instance, state);
}

uint8 HAL_hsusb_GetHIRD(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetHIRD(instance);
}

void HAL_hsusb_SetHIRD(uint8 instance, uint8 val)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetHIRD(instance, val);
}

/*
===========================================================================

FILE:         HAL_ssusb_impl.c

DESCRIPTION:
This is the HAL implementation for the SS-USB cores.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/hal/src/HAL_ssusb_impl.c#1 $

===========================================================================

===========================================================================
Copyright © 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HAL_ssusb_impl.h"
#include "HALhwio.h"
#include "hsu_log.h"

/* -----------------------------------------------------------------------
**                           EXTERNS
** ----------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/*
* SS-USB HAL target-specific Constructor.
*/
void HAL_ssusb_ConstructImplInstance
(
  HAL_SSUSB_CoresType*
);

#ifdef __cplusplus
}
#endif

/* -----------------------------------------------------------------------
Constants and Macros
** ----------------------------------------------------------------------- */
/*SS-USB HAL CoreInfo table size */
#define HAL_SSUSB_MAX_CORES_QTY 3

#define HAL_SSUSB_ADDR(instance, offset) \
  (HAL_SSUSB_CoresObj.CoreInfo[instance].BusParams.BaseAddress+offset)

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */
/*SS-USB HAL CoreInfo table */
static HAL_SSUSB_CoreInfoType HAL_SSUSB_CoreInfo[HAL_SSUSB_MAX_CORES_QTY];

/* SS-USB HAL Cores object */
static HAL_SSUSB_CoresType HAL_SSUSB_CoresObj =
{
  &HAL_SSUSB_CoreInfo[0],
  0,
  FALSE
};

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_ssusb_Init
**
**  Description:
**    This function initializes the SS-USB core Hardware prior
**    to the first usage after power-up or a call to HAL_ssusb_Reset().
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
void HAL_ssusb_Init(void)
{
  if (!HAL_SSUSB_CoresObj.Initialized)
  {
    uint8 instance;

    HAL_SSUSB_CoresObj.MaxCoresQty = 0;
    for (instance=0; instance<HAL_SSUSB_MAX_CORES_QTY; instance++)
    {
      HAL_SSUSB_CoreInfoType* pCoreInfo = &HAL_SSUSB_CoresObj.CoreInfo[instance];

      if (0 == ssusb_cores_bus_params[instance].BaseAddress)
      {
        pCoreInfo->LastError = HAL_SSUSB_NotSupported;
      }
      else
      {
        pCoreInfo->BusParams.BaseAddress =
          ssusb_cores_bus_params[instance].BaseAddress;
        pCoreInfo->BusParams.IntNumber =
          ssusb_cores_bus_params[instance].IntNumber;
        pCoreInfo->LastError = HAL_SSUSB_Success;
        HAL_SSUSB_CoresObj.MaxCoresQty++;
      }
    }
    HAL_ssusb_ConstructImplInstance(&HAL_SSUSB_CoresObj);
    HAL_SSUSB_CoresObj.Initialized = TRUE;
  }
}

/* ===========================================================================
**
**  HAL_ssusb_Reset
**
**  Description:
**    This function is intended to return the SS-USB hardware to its
**    power-on status.
**
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    None.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_ssusb_Reset(void)
{
  if (HAL_SSUSB_CoresObj.Initialized)
  {
    HAL_SSUSB_CoresObj.Initialized = FALSE;
  }
}

/* ===========================================================================
**
**  HAL_ssusb_SupportedCoresQty
**
**  Description:
**    This function reports the SS-USB cores’ amount.
**
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    None.
**
**  Return:
**     The number of the SS-USB cores attached to the target.
**
** ======================================================================== */
uint8 HAL_ssusb_SupportedCoresQty(void)
{
  return HAL_SSUSB_CoresObj.MaxCoresQty;
}

/* ===========================================================================
**
**  HAL_ssusb_GetLastError
**
**  Description:
**    This function gets the last error code of the SS-USB core recorded
**    from the SS-USB HAL failure. It is intended for debug purposes.
**
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use.
**             This value should be one less than reported by
**             the HAL_ssusb_SupportedCoresQty().
**
**  Return:
**     The last error code of the SS-USB core recorded from the SS-USB HAL failure.
**
** ======================================================================== */
HAL_SSUSB_ErrorType HAL_ssusb_GetLastError(uint8 instance)
{
  return HAL_SSUSB_CoresObj.CoreInfo[instance].LastError;
}

/* ===========================================================================
**
**  HAL_ssusb_GetInterruptNumber
**
**  Description:
**    This function gets the interrupt number of the SS-USB core.
**
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use.
**             This value should be one less than reported by
**             the HAL_ssusb_SupportedCoresQty().
**
**  Return:
**     The interrupt number of the SS-USB core.
**
** ======================================================================== */
uint8 HAL_ssusb_GetInterruptNumber(uint8 instance)
{
  return HAL_SSUSB_CoresObj.CoreInfo[instance].BusParams.IntNumber;
}

/* ===========================================================================
**
**  HAL_ssusb_Read32
**
**  Description:
**    This function reads 32 bits from the SS-USB core register.
**
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use.
**             This value should be one less than reported by
**             the HAL_ssusb_SupportedCoresQty().
**    offset   - Determines the register offset.
**             This value must be aligned at the 32 bit boundary and
**             in the address ranges defined by the HW specification.
**
**  Return:
**    The read data.
**
** ======================================================================== */
uint32 HAL_ssusb_Read32(uint8 instance, uint32 offset)
{
  return __inpdw(HAL_SSUSB_ADDR(instance, offset));
}

/* ===========================================================================
**
**  HAL_ssusb_Write32
**
**  Description:
**    This function writes 32 bits to the SS-USB core register.
**
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use.
**             This value should be one less than reported by
**             the HAL_ssusb_SupportedCoresQty().
**    offset   - Determines the register offset.
**             This value must be aligned at the 32 bit boundary and
**             in the address ranges defined by the HW specification.
**    data     - Determines the write data.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_ssusb_Write32(uint8 instance, uint32 offset, uint32 data)
{
  __outpdw(HAL_SSUSB_ADDR(instance, offset), data);
  //hsu_reg_log(HAL_SSUSB_ADDR(instance, offset), data);
  hsu_ulog_2(REG_LOG, "0x%08X, 0x%08X", 0, 0);//HAL_SSUSB_ADDR(instance, offset), data);
}

/* ===========================================================================
**
**  HAL_ssusb_GetBits32
**
**  Description:
**    This function gets bits from the SS-USB core 32-bit register.
**
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use.
**             This value should be one less than reported by
**             the HAL_ssusb_SupportedCoresQty().
**    offset   - Determines the register offset.
**             This value must be aligned at the 32 bit boundary and
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be get.
**
**  Return:
**    The read data masked with <bits>.
**
** ======================================================================== */
uint32 HAL_ssusb_GetBits32(uint8 instance, uint32 offset, uint32 bits)
{
  return (__inpdw(HAL_SSUSB_ADDR(instance, offset)) & bits);
}

/* ===========================================================================
**
**  HAL_ssusb_SetBits32
**
**  Description:
**    This function sets bits in the SS-USB core 32-bit register.
**
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use.
**             This value should be one less than reported by
**             the HAL_ssusb_SupportedCoresQty().
**    offset   - Determines the register offset.
**             This value must be aligned at the 32 bit boundary and
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be set.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_ssusb_SetBits32(uint8 instance, uint32 offset, uint32 bits)
{
  uint32 address;
  uint32 data;

  address = HAL_SSUSB_ADDR(instance, offset);
  data = __inpdw(address);
  __outpdw(address, data | bits);
}

/* ===========================================================================
**
**  HAL_ssusb_ClearBits32
**
**  Description:
**    This function clears bits in the SS-USB core 32-bit register.
**
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which SS-USB core to use.
**             This value should be one less than reported by
**             the HAL_ssusb_SupportedCoresQty().
**    offset   - Determines the register offset.
**             This value must be aligned at the 32 bit boundary and
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be cleared.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_ssusb_ClearBits32(uint8 instance, uint32 offset, uint32 bits)
{
  uint32 address;
  uint32 data;

  address = HAL_SSUSB_ADDR(instance, offset);
  data = __inpdw(address);
  __outpdw(address, data & ~bits);
}

/* ===========================================================================
**
**  HAL_ssusb_ClearAndSetBits32
**
**  Description:
**    This function clears and then sets bits in the SS-USB core 32-bit register.
**
**  Dependencies:
**    HAL_ssusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance  - Determines which SS-USB core to use.
**              This value should be one less than reported by
**              the HAL_ssusb_SupportedCoresQty().
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
void HAL_ssusb_ClearAndSetBits32(uint8 instance, uint32 offset, uint32 clearBits, uint32 setBits)
{
  uint32 address;
  uint32 data;

  address = HAL_SSUSB_ADDR(instance, offset);
  data = __inpdw(address);
  __outpdw(address, (data & ~clearBits) | setBits);
}

/*
===============================================================================

         P U L S E   W I D T H   M O D U L A T I O N  ( P W M )
          H A R D W A R E   A B S T R A C T I O N   L A Y E R

FILE:         HALpwm.c

DESCRIPTION:
  This modules contains common functionality to read and write to the PWM
  registers.

===============================================================================

                             Edit History

  $Header: //components/rel/core.tx/6.0/systemdrivers/pwm/hal/src/HALpwm.c#1 $
  $Author: pwbldsvc $
  $DateTme: $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/28/19   shm     Initial version

===============================================================================
  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===============================================================================
*/

/*=============================================================================
                         INCLUDE FILES FOR MODULE
=============================================================================*/

#include <HALcomdef.h>
#include <HALhwio.h>
#include "HALpwm.h"

#include "HALpwmHWIO.h"
#include "HALpwmInternal.h"

#include "busywait.h"

/*
 * HAL_pwm_Type
 *
 * Consolidate of all PWM related data structures
 *
 */
typedef struct
{
  boolean              bInit;
  HAL_pwm_PlatformType mPlatform;

} HAL_pwm_Type;

/*=============================================================================
                    GLOBAL STRUCTURES FOR THIS MODULE
=============================================================================*/

HAL_pwm_Type HAL_pwm;

uint32 nPWMBaseAddress;
uint32 nPWMBaseAddressPhys;


/*=============================================================================
  =============================================================================
                   FUNCTIONS CONTAINED WITHIN THIS MODULE
=============================================================================*/


/* ===========================================================================
**  HAL_pwm_Init
**
** ======================================================================== */

void HAL_pwm_Init
(
  HAL_pwm_ContextType *pTarget
)
{
  uint32                  nIdx;

  if (HAL_pwm.bInit == TRUE)
  {
    return;
  }

  /*
   * Platform early init, which fills in the mPlatform structure.
   */
  HAL_pwm_PlatformInitEarly(&HAL_pwm.mPlatform);

  /*
   * Platform late init if it exists
   */
  if (HAL_pwm.mPlatform.InitLate != NULL)
  {
    HAL_pwm.mPlatform.InitLate();
  }

  HAL_pwm.bInit = TRUE;

} /* END HAL_pwm_Init */


/*=============================================================================

FUNCTION
  void HAL_pwm_Enable(uint32 nBaseAddr, uint32 nPWM)

DESCRIPTION
   Enables the requested PWM instance (by enabling the corresponding bit in
   TOPCTL0 in the PWM frame module.

PARAMETERS
   nBaseAddr - Base address of the PWM frame
   nPWM      - PWM instance (0 - 9) to enable

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void HAL_pwm_Enable
(
  uint32  nBaseAddr,
  uint32  nPWM
)
{
  uint32 nVal;

  nVal = HWIO_INXF(nBaseAddr, WEB_PWM_TOPCTL0, PWM_CLK_EN);
  nVal |= (1 << nPWM);

  HWIO_OUTXF(nBaseAddr, WEB_PWM_TOPCTL0, PWM_CLK_EN, nVal);

  return;
}

/*=============================================================================

FUNCTION
  void HAL_pwm_Disable(uint32 nBaseAddr, uint32 nPWM)

DESCRIPTION
   Disables the requested PWM instance (by enabling the corresponding bit in
   TOPCTL0 in the PWM frame module.

PARAMETERS
   nBaseAddr - Base address of the PWM frame
   nPWM      - PWM instance (0 - 9) to disable

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void HAL_pwm_Disable
(
  uint32  nBaseAddr,
  uint32  nPWM
)
{
  uint32 nVal;

  nVal = HWIO_INXF(nBaseAddr, WEB_PWM_TOPCTL0, PWM_CLK_EN);
  nVal &= ~(1 << nPWM);

  HWIO_OUTXF(nBaseAddr, WEB_PWM_TOPCTL0, PWM_CLK_EN, nVal);

  return;
}

/*=============================================================================

FUNCTION
  HAL_pwm_WriteDutyCtlRegs

DESCRIPTION
   Programs the PWMn_CTL1 & PWMn_CTL2 registers based on the input parameters.

PARAMETERS
   nBaseAddr - Base address of the PWM frame
   nPWM      - PWM instance (0 - 9) to configure
   nCtl1     - CTL1 value to be programmed
   nCtl2     - CTL2 value to be programmed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void HAL_pwm_WriteDutyCtlRegs
(
  uint32 nBaseAddr,
  uint32 nPWM,
  uint32 nCtl1,
  uint32 nCtl2
)
{
  uint32 nReg, nCfgRegOffset;

  nCfgRegOffset =
    HWIO_WEB_PWM1_CTL0_ADDR(nBaseAddr) - HWIO_WEB_PWM0_CTL0_ADDR(nBaseAddr);

  nReg = HWIO_WEB_PWM0_CTL1_ADDR(nBaseAddr) + nPWM * nCfgRegOffset;

  outpdw(nReg, nCtl1);

  nReg = HWIO_WEB_PWM0_CTL2_ADDR(nBaseAddr) + nPWM * nCfgRegOffset;

  outpdw(nReg, nCtl2);
}


/*=============================================================================

FUNCTION
  HAL_pwm_WriteCycCfg

DESCRIPTION
   Programs the PWMn_CYC_CFG register based on the input parameter.

PARAMETERS
   nBaseAddr - Base address of the PWM frame
   nPWM      - PWM instance (0 - 9) to configure
   nCycCfg   - CYC_CFG value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void HAL_pwm_WriteCycCfg
(
  uint32 nBaseAddr,
  uint32 nPWM,
  uint32 nCycCfg
)
{
  uint32 nReg, nCfgRegOffset;

  nCfgRegOffset =
    HWIO_WEB_PWM1_CTL0_ADDR(nBaseAddr) - HWIO_WEB_PWM0_CTL0_ADDR(nBaseAddr);

  nReg = HWIO_WEB_PWM0_CYC_CFG_ADDR(nBaseAddr) + nPWM * nCfgRegOffset;

  outpdw(nReg, nCycCfg);
}


/*=================================================================================

FUNCTION
  HAL_pwm_Update

DESCRIPTION
  Will trigger UPDATE to latch in the configurations & poll on it to go LOW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/

void HAL_pwm_Update
(
  uint32  nBaseAddr,
  uint32  nPWM,
  boolean bWaitForClear
)
{
  uint32 nReg, nCfgRegOffset;

  nCfgRegOffset =
    HWIO_WEB_PWM1_CTL0_ADDR(nBaseAddr) - HWIO_WEB_PWM0_CTL0_ADDR(nBaseAddr);

  nReg = HWIO_WEB_PWM0_UPDATE_ADDR(nBaseAddr) + nPWM * nCfgRegOffset;

  outpdw(nReg, 1);

  if (bWaitForClear)
  {
    while (inpdw(nReg) == 1)
    {
      busywait(5);
    }
  }

}


/*=================================================================================

FUNCTION
  void HAL_pwm_Reset(uint32 nPWM)

DESCRIPTION
  Will reset the input PWM instance

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/

void HAL_pwm_Reset(uint32 nPWM)
{
  /* Reserved for future use. */
}


/*=================================================================================

FUNCTION
  uint32 HAL_pwm_GetCounterWidth(void)

DESCRIPTION
  Returns the bit width of the PWM counter

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/

uint32 HAL_pwm_GetCounterWidth(void)
{

  return HAL_pwm.mPlatform.mCtl.nCounterBitWidth;

}

/*===============================================================================*/

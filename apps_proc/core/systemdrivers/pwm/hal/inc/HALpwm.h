#ifndef __HALPWM_H__
#define __HALPWM_H__

/*=============================================================================

         P U L S E   W I D T H   M O D U L A T I O N  (P W M)
          H A R D W A R E   A B S T R A C T I O N   L A Y E R

FILE:         HALpwm.h

DESCRIPTION:
  This contains externalized PWM HAL code. Functions that
  are prototyped here are available to the PWM driver.

===============================================================================

                             Edit History

  $Header: //components/rel/core.tx/6.0/systemdrivers/pwm/hal/inc/HALpwm.h#1 $
  $Author: pwbldsvc $
  $DateTime: 2020/01/30 22:49:35 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  06/13/19   shm     Updated duty cycle calculation macro name
  02/28/19   shm     Initial version

===============================================================================
  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===============================================================================
*/

#include "HALcomdef.h"

#define HAL_PWM_CVT_TO_CYC_CFG(nFreqHz, nCoreClkFreqHz)   \
  ((nCoreClkFreqHz / nFreqHz) - 1)

#define HAL_PWM_CVT_TO_CTL1(nDuty, nFreqHz, nCoreClkFreqHz) \
  ((HAL_PWM_CVT_TO_CYC_CFG(nFreqHz, nCoreClkFreqHz) + 1) * nDuty / 100)

typedef struct
{
  uint32  nNumPWM;
  uint32  nBaseAddress;

} HAL_pwm_InitType;

typedef struct
{
  // uint16 nCtl0; // Currently not modified by PWM driver
  uint16 nCtl1;
  uint16 nCtl2;
  uint16 nCycCfg;
} HAL_pwm_ConfigType;

typedef struct
{
  void *pTargetData;
} HAL_pwm_ContextType;

typedef struct
{
  uint32 *pnPWMBase;
  uint32 *pnPWMBasePhys;
} HAL_pwm_TargetDataType;

/*=============================================================================
    Function prototypes that are available to the PWM driver.
  ===========================================================================*/
void HAL_pwm_Init                 (HAL_pwm_ContextType *pTarget);

void HAL_pwm_Reset                (uint32 nPWM);

void HAL_pwm_WriteDutyCtlRegs     (uint32 nBaseAddr, uint32 nPWM, uint32 nCtl1, uint32 nCtl2);
void HAL_pwm_WriteCycCfg          (uint32 nBaseAddr, uint32 nPWM, uint32 nCycCfg);

void HAL_pwm_Enable               (uint32 nBaseAddr, uint32 nPWM);
void HAL_pwm_Disable              (uint32 nBaseAddr, uint32 nPWM);
void HAL_pwm_Update               (uint32 nBaseAddr, uint32 nPWM, boolean bWaitForClear);

uint32 HAL_pwm_GetCounterWidth    (void);

/*
===============================================================================
  INTERFACES DOCUMENTATION - See source files for parameter descriptions.
===============================================================================

FUNCTION
  void HAL_pwm_Init (HAL_pwm_InitType* pHALPWMInit)

DESCRIPTION
  Does the initializing of the hardware buffers and address structures

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===============================================================================

FUNCTION
  void HAL_pwm_Reset(void)

DESCRIPTION
  Programs hardware to the reset state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

#endif /* __HALPWM_H__ */

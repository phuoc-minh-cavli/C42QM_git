/*
===========================================================================

FILE:         HALpwmPlatform.c

DESCRIPTION:
  This is the platform hardware abstraction layer implementation for the
  PWM block.

===========================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/systemdrivers/pwm/hal/hw/mdm9205/HALpwmPlatform.c#1 $
$Author: pwbldsvc $
$DateTime: 2020/01/30 22:49:35 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/28/19   shm     Initial version

===========================================================================
  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <HALpwm.h>

#include "HALpwmInternal.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */


/* ===========================================================================
**  HAL_pwm_PlatformInitEarly
**
** ======================================================================== */

void HAL_pwm_PlatformInitEarly
(
  HAL_pwm_PlatformType *pmPlatform
)
{
  pmPlatform->mCtl.nCounterBitWidth = 16;

  return;

} /* END HAL_pwm_PlatformInitEarly */

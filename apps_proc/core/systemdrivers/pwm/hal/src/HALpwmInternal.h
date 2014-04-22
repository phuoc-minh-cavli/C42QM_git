#ifndef __HALPWMINTERNAL_H__
#define __HALPWMINTERNAL_H__

/*
===============================================================================

FILE:         HALpwmInternal.h

DESCRIPTION:
  This is the hardware abstraction layer internal interface for the PWM
  block.

===============================================================================

                             Edit History

$Header: //components/rel/core.tx/6.0/systemdrivers/pwm/hal/src/HALpwmInternal.h#1 $
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



/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
 * HAL_pwm_CtlType
 * The description of PWM register interface.
 *
 */
typedef struct
{
  uint32 nCounterBitWidth;

} HAL_pwm_CtlType;

/*
 * HAL_pwm_PlatformType
 *
 * Platform (asic/processor) specific information filled in by the platform
 * code in HAL_pwm_PlatformInitEarly.
 *
 *  nNumCtl:  Number of PWM instances.
 *
 *  InitLate:      Optional function executed at the end of HAL_pwm_Init.
 *  Save:          Optional function executed at the end of HAL_pwm_Save.
 *  Restore:       Optional function executed at the start of
 *                 HAL_pwm_Restore.
 */
typedef struct
{
  uint32            nTotalNumber;
  HAL_pwm_CtlType   mCtl;
  void            (*InitLate)(void);
  void            (*Save)    (void);
  void            (*Restore) (void);

} HAL_pwm_PlatformType;


/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */
void HAL_pwm_PlatformInitEarly
(
  HAL_pwm_PlatformType *pmPlatform
);

/*
===============================================================================
  INTERFACES DOCUMENTATION
===============================================================================

HAL_pwm_PlatformInitEarly( )

Description:
   This function is invoked at the start of HAL_pwm_Init to fill in the
   platform specific details in the given core interface structure.  This
   function must be implemented by the platform core.

Parameters:
   pmPlatform - pointer to the platform interface structure to fill in.

===============================================================================

*/

#endif /* __HALPWMINTERNAL_H__ */

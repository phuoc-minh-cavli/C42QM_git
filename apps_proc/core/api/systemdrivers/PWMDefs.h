#ifndef PWMDEFS_H
#define PWMDEFS_H

/**
  @file PWMDefs.h

  Public definitions for the PWM driver.
*/
/*===========================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR

  ===========================================================================

  $Header: //components/rel/core.tx/6.0/api/systemdrivers/PWMDefs.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 
  02/28/19   shm     Initial version

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "com_dtypes.h" /* for "uint32" */


/*=========================================================================
      Type Definitions
==========================================================================*/


/** @addtogroup pwm_functions
@{ */

/**
  PWM Identifiers
 */
typedef enum
{
  PWM_INSTANCE_0,
  PWM_INSTANCE_1,
  PWM_INSTANCE_2,
  PWM_INSTANCE_3,
  PWM_INSTANCE_4,
  PWM_INSTANCE_5,
  PWM_INSTANCE_6,
  PWM_INSTANCE_7,
  PWM_INSTANCE_8,
  PWM_INSTANCE_9,

  PWM_NUM_INSTANCES
} PWMInstanceType;

/**
  PWM instance handle.
 */
typedef uint32 DalPWMIdType;

/** @} */ /* end_addtogroup pwm_functions */

#endif /* PWMDEFS_H */

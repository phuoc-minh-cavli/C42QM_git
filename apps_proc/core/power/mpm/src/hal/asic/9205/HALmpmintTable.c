/*===========================================================================
   FILE:         HALmpmintPlatform.c
 
   DESCRIPTION:   This is the platform hardware abstraction layer implementation for the
  MPM interrupt controller block.
                  This platform is for the RPM on 9x25.

                 Copyright © 2012-2018 QUALCOMM Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

  =============================================================================
  =============================================================================
$Header: //components/rel/core.tx/6.0/power/mpm/src/hal/asic/9205/HALmpmintTable.c#1 $
$DateTime: 2020/01/30 22:49:35 $
$Author: pwbldsvc $
========================================================================*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <HALmpmint.h>
#include "HALmpmintInternal.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/*
 * DEFINE_IRQ
 *
 * Macro to define an IRQ mpmint_irq_data_type entry in the table. 
 *
 * The bit position field is no longer used.
 */
#define DEFINE_IRQ( trigger, gpio ) \
  {                                                 \
    HAL_MPMINT_TRIGGER_##trigger,                   \
    gpio,                                           \
    HAL_MPMINT_INVALID_BIT_POS,                     \
  }

/*
 * Target-specific interrupt configurations
 */
HAL_mpmint_PlatformIntType aInterruptTable[HAL_MPMINT_NUM] = 
{
  /*          Trigger            GPIO           */
  /*          -------  -----------------------  */
  DEFINE_IRQ( HIGH,    HAL_MPMINT_INVALID_GPIO  ),         /* 0 */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( HIGH,    HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  5                        ),

  DEFINE_IRQ( RISING,  0                        ),         /* 5 */
  DEFINE_IRQ( RISING,  2                        ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  9                        ),

  DEFINE_IRQ( RISING,  13                       ),         /* 10 */
  DEFINE_IRQ( RISING,  1                        ),
  DEFINE_IRQ( RISING,  20                       ),
  DEFINE_IRQ( RISING,  21                       ),
  DEFINE_IRQ( RISING,  22                       ),

  DEFINE_IRQ( RISING,  50                       ),         /* 15 */
  DEFINE_IRQ( RISING,  52                       ),
  DEFINE_IRQ( RISING,  28                       ),
  DEFINE_IRQ( RISING,  51                      ),
  DEFINE_IRQ( RISING,  26                       ),

  DEFINE_IRQ( RISING,  4                        ),         /* 20 */
  DEFINE_IRQ( RISING,  47                       ),
  DEFINE_IRQ( RISING,  6                        ),
  DEFINE_IRQ( RISING,  8                        ),
  DEFINE_IRQ( RISING,  14                       ),

  DEFINE_IRQ( RISING,  19                       ),         /* 25 */
  DEFINE_IRQ( RISING,  27                       ),
  DEFINE_IRQ( RISING,  30                       ),
  DEFINE_IRQ( RISING,  36                       ),
  DEFINE_IRQ( RISING,  46                       ),

  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),         /* 30 */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
                                                
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),         /* 35 */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
                                                
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),         /* 40 */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( HIGH,    HAL_MPMINT_INVALID_GPIO  ),

  DEFINE_IRQ( HIGH,    HAL_MPMINT_INVALID_GPIO  ),         /* 45 */
  DEFINE_IRQ( HIGH,    HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( HIGH,    HAL_MPMINT_INVALID_GPIO  ),

  DEFINE_IRQ( RISING,  49                       ),         /* 50 */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( HIGH,    HAL_MPMINT_INVALID_GPIO  ),

  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),         /* 55 */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( HIGH,    HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( HIGH,    HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),

  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),         /* 60 */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),

  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO  ),         /* 63 */
};


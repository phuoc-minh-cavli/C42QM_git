#ifndef HALMPMINT9x07_H
#define HALMPMINT9x07_H
/*
===========================================================================

FILE:         HALmpmint8909.h

DESCRIPTION:
  Target-specific enumerations for HALmpmint.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.3/core/power/mpm/hal/source/9x05/HALmpmint9x05.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
             QUALCOMM Proprietary and Confidential
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif


/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */

#include "comdef.h"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/*
 * HAL_mpmint_IsrType
 *
 * List of possible interrupt sources.  All of these are not necessarily
 * supported on the HW.
 *
 * NOTE: This list must be in the same order as the lists in the
 *       MPM driver.
 */
typedef enum
{
  /* Hard Wired */
  HAL_MPMINT_QTIMER_ISR                =  0,
  HAL_MPMINT_PEN_ISR                   =  1,
  HAL_MPMINT_TSENS_UPPER_LOWER_ISR     =  2,

  /* High Voltage */
  //UNUSED                             =  3,
  HAL_MPMINT_GPIO5_ISR                 =  4,
  HAL_MPMINT_GPIO0_ISR                 =  5,
  HAL_MPMINT_GPIO2_ISR                 =  6,
  //UNUSED                			   =  7,
  //UNUSED                			   =  8,
  HAL_MPMINT_GPIO9_ISR                 =  9,
  HAL_MPMINT_GPIO13_ISR                = 10,
  HAL_MPMINT_GPIO1_ISR                 = 11,
  HAL_MPMINT_GPIO20_ISR                = 12,
  HAL_MPMINT_GPIO21_ISR                = 13,
  HAL_MPMINT_GPIO22_ISR                = 14,
  HAL_MPMINT_GPIO50_ISR                = 15,
  HAL_MPMINT_GPIO52_ISR                = 16,
  HAL_MPMINT_GPIO28_ISR                = 17,
  HAL_MPMINT_GPIO51_ISR                = 18,
  HAL_MPMINT_GPIO26_ISR                = 19,
  HAL_MPMINT_GPIO4_ISR                 = 20,
  HAL_MPMINT_GPIO47_ISR                = 21,
  HAL_MPMINT_GPIO6_ISR                 = 22,
  HAL_MPMINT_GPIO8_ISR                 = 23,
  HAL_MPMINT_GPIO14_ISR                = 24,
  HAL_MPMINT_GPIO19_ISR                = 25,
  HAL_MPMINT_GPIO27_ISR                = 26,
  HAL_MPMINT_GPIO30_ISR                = 27,
  HAL_MPMINT_GPIO36_ISR                = 28,
  HAL_MPMINT_GPIO46_ISR                = 29,
  //UNUSED			                   = 30,
  //UNUSED			                   = 31,
  //UNUSED			                   = 32,
  //UNUSED			                   = 33,
  //UNUSED			                   = 34,
  //UNUSED			                   = 35,
  //UNUSED			                   = 36,
  //UNUSED			                   = 37,
  //UNUSED			                   = 38,
  //UNUSED			                   = 39,
  //UNUSED			                   = 40,
  HAL_MPMINT_TSENS_UPPER_INT           = 41,
  HAL_MPMINT_CORE                      = 42,
  //UNUSED                             = 43,  
  //UNUSED				               = 44,
  //UNUSED				               = 45,
  HAL_MPMINT_SRST_N_ISR                = 46,
  HAL_MPMINT_SVLD_HV_ISR               = 47,
  HAL_MPMINT_ID_HV_ISR                 = 48,
  HAL_MPMINT_DPSE_HV_ISR               = 49,
  HAL_MPMINT_GPIO49_ISR                = 50,
  HAL_MPMINT_USB_ASYNC_WAKE_ISR        = 51,
  //UNUSED                             = 52,
  //UNUSED                             = 53,
  //UNUSED                             = 54,
  //UNUSED                             = 55,
  //UNUSED                             = 56,
  //UNUSED                             = 57,
  HAL_MPMINT_DMSE_HV_ISR               = 58,
  //UNUSED                             = 59,
  HAL_MPMINT_UIM_CARD_ISR              = 60,
  HAL_MPMINT_UIM_BATT_ISR              = 61,
  HAL_MPMINT_SPMI_WAKE_ISR             = 62,
  //UNUSED                             = 63,
  HAL_MPMINT_NUM                       = 64,

  /* Interrupts below this point are legacy definitions, not supported by this
   * version of the hardware. */
  HAL_MPMINT_NOT_DEFINED_ISR       = HAL_MPMINT_NUM,


} HAL_mpmint_IsrType;

#ifdef __cplusplus
}
#endif

#endif /* HALMPMINT8909_H */




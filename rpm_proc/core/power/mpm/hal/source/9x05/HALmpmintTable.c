/*
===========================================================================

FILE:         HALmpmintPlatform.c

DESCRIPTION:
  This is the platform hardware abstraction layer implementation for the
  MPM interrupt controller block.
  This platform is for the RPM on 8909.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.3/core/power/mpm/hal/source/9x05/HALmpmintTable.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

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
 * Macro to define an IRQ mpmint_irq_data_type entry in the table
 */
#define DEFINE_IRQ( trigger, gpio, padPos ) \
  {                                                 \
    HAL_MPMINT_TRIGGER_##trigger,                   \
    gpio,                                           \
    padPos,                                         \
  }

/*
 * Target-specific interrupt configurations
 */
HAL_mpmint_PlatformIntType aInterruptTable[HAL_MPMINT_NUM] =
{
  /*          Trigger            GPIO                      Pad Bit          */
  /*          -------  -----------------------   -------------------------- */
  DEFINE_IRQ( HIGH,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_QTIMER_ISR                             00*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_PEN_ISR                                  */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_TSENS_UPPER_LOWER_ISR                    */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* UNUSED                             */
  DEFINE_IRQ( RISING,  5,                        4                          ), /* HAL_MPMINT_GPIO5_ISR                                */
  DEFINE_IRQ( RISING,  0,                        0                          ), /* HAL_MPMINT_GPIO0_ISR                              05*/
  DEFINE_IRQ( RISING,  2,                        2                          ), /* HAL_MPMINT_GPIO2_ISR                                */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED                		                    */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED                		                    */   
  DEFINE_IRQ( RISING,  9,                        7                          ), /* HAL_MPMINT_GPIO9_ISR                                */
  DEFINE_IRQ( RISING,  13,                       8                          ), /* HAL_MPMINT_GPIO13_ISR                             10*/
  DEFINE_IRQ( RISING,  1,                        1                          ), /* HAL_MPMINT_GPIO1_ISR                                */
  DEFINE_IRQ( RISING,  20,                       11                         ), /* HAL_MPMINT_GPIO20_ISR                               */
  DEFINE_IRQ( RISING,  21,                       12                         ), /* HAL_MPMINT_GPIO21_ISR                               */
  DEFINE_IRQ( RISING,  22,                       13                         ), /* HAL_MPMINT_GPIO22_ISR                               */
  DEFINE_IRQ( RISING,  50,                       22                         ), /* HAL_MPMINT_GPIO50_ISR                             15*/
  DEFINE_IRQ( RISING,  52,                       24                         ), /* HAL_MPMINT_GPIO52_ISR                               */
  DEFINE_IRQ( RISING,  28,                       16                         ), /* HAL_MPMINT_GPIO28_ISR                               */
  DEFINE_IRQ( RISING,  51,                       23                         ), /* HAL_MPMINT_GPIO51_ISR                               */
  DEFINE_IRQ( RISING,  26,                       14                         ), /* HAL_MPMINT_GPIO26_ISR                               */
  DEFINE_IRQ( RISING,  4,                        3                          ), /* HAL_MPMINT_GPIO4_ISR                              20*/
  DEFINE_IRQ( RISING,  47,                       20                         ), /* HAL_MPMINT_GPIO47_ISR                               */
  DEFINE_IRQ( RISING,  6,                        5                          ), /* HAL_MPMINT_GPIO6_ISR                                */
  DEFINE_IRQ( RISING,  8,                        6                          ), /* HAL_MPMINT_GPIO8_ISR                                */
  DEFINE_IRQ( RISING,  14,                       9                          ), /* HAL_MPMINT_GPIO14_ISR                               */
  DEFINE_IRQ( RISING,  19,                       10                         ), /* HAL_MPMINT_GPIO19_ISR                             25*/
  DEFINE_IRQ( RISING,  27,                       15                         ), /* HAL_MPMINT_GPIO27_ISR                               */
  DEFINE_IRQ( RISING,  30,                       17                         ), /* HAL_MPMINT_GPIO30_ISR                               */
  DEFINE_IRQ( RISING,  36,                       18                         ), /* HAL_MPMINT_GPIO36_ISR                               */
  DEFINE_IRQ( RISING,  46,                       19                         ), /* HAL_MPMINT_GPIO46_ISR                               */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED			                              30*/    
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED			                                */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED			                                */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED			                                */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED			                                */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED			                              35*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED			                                */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED			                                   */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED			                                */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED			                                */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED			                              40*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_TSENS_UPPER_INT                          */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_CORE                                     */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED                                            */  
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED				                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED				                          45*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  25                         ), /* HAL_MPMINT_SRST_N_ISR                               */
  DEFINE_IRQ( DUAL,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_SVLD_HV_ISR                              */
  DEFINE_IRQ( DUAL,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_ID_HV_ISR                                */
  DEFINE_IRQ( DUAL,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_DPSE_HV_ISR                              */
  DEFINE_IRQ( RISING,  49,                       21                         ), /* HAL_MPMINT_GPIO49_ISR                             50*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_USB_ASYNC_WAKE_ISR                       */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED                                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED                                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED                                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED                                          55*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED                                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED                                            */
  DEFINE_IRQ( DUAL,    HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_DMSE_HV_ISR                              */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED                                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_UIM_CARD_ISR                           60*/
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_UIM_BATT_ISR                             */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* HAL_MPMINT_SPMI_WAKE_ISR                            */
  DEFINE_IRQ( RISING,  HAL_MPMINT_INVALID_GPIO,  HAL_MPMINT_INVALID_BIT_POS ), /* //UNUSED                                            */
};


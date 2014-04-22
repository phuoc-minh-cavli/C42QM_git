#ifndef HALMPMINT9x35_H
#define HALMPMINT9x35_H
/*===========================================================================
   FILE:         HALmpmint9x45.h
 
   DESCRIPTION:  Target-specific enumerations for HALmpmint.

                 Copyright © 2013-2018 QUALCOMM Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

  =============================================================================
  =============================================================================
$Header: //components/rel/core.tx/6.0/power/mpm/src/hal/asic/9205/HALmpmint9205.h#1 $
$DateTime: 2020/01/30 22:49:35 $
$Author: pwbldsvc $
========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <HALcomdef.h>

/* -----------------------------------------------------------------------
**                           TYPES
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
  HAL_MPMINT_MPM_BIT_00_ISR          = 0,  /* 0 */
  HAL_MPMINT_MPM_BIT_01_ISR,
  HAL_MPMINT_MPM_BIT_02_ISR,
  HAL_MPMINT_MPM_BIT_03_ISR,
  HAL_MPMINT_GPIO5_ISR,
  
  HAL_MPMINT_GPIO0_ISR,        /* 5 */
  HAL_MPMINT_GPIO2_ISR,
  HAL_MPMINT_MPM_BIT_07_ISR,
  HAL_MPMINT_MPM_BIT_08_ISR,
  HAL_MPMINT_GPIO9_ISR,
  
  HAL_MPMINT_GPIO13_ISR,                         /* 10 */
  HAL_MPMINT_GPIO1_ISR,
  HAL_MPMINT_GPIO20_ISR,
  HAL_MPMINT_GPIO21_ISR,
  HAL_MPMINT_GPIO22_ISR,
  
  HAL_MPMINT_GPIO50_ISR,       /* 15 */
  HAL_MPMINT_GPIO52_ISR,
  HAL_MPMINT_GPIO28_ISR,   
  HAL_MPMINT_GPIO51_ISR,   
  HAL_MPMINT_GPIO26_ISR,
  
  HAL_MPMINT_GPIO4_ISR,       /* 20 */
  HAL_MPMINT_GPIO47_ISR,  
  HAL_MPMINT_GPIO6_ISR,    
  HAL_MPMINT_GPIO8_ISR,
  HAL_MPMINT_GPIO14_ISR,
  
  HAL_MPMINT_GPIO19_ISR,       /* 25 */
  HAL_MPMINT_GPIO27_ISR,     
  HAL_MPMINT_GPIO30_ISR,   
  HAL_MPMINT_GPIO36_ISR,
  HAL_MPMINT_GPIO46_ISR,         
  
  HAL_MPMINT_MPM_BIT_30_ISR,       /* 30 */
  HAL_MPMINT_MPM_BIT_31_ISR,
  HAL_MPMINT_MPM_BIT_32_ISR,
  HAL_MPMINT_MPM_BIT_33_ISR,
  HAL_MPMINT_MPM_BIT_34_ISR,
  
  HAL_MPMINT_MPM_BIT_35_ISR,       /* 35 */
  HAL_MPMINT_MPM_BIT_36_ISR,
  HAL_MPMINT_MPM_BIT_37_ISR,
  HAL_MPMINT_MPM_BIT_38_ISR,
  HAL_MPMINT_MPM_BIT_39_ISR,
  
  HAL_MPMINT_MPM_BIT_40_ISR,       /* 40 */
  HAL_MPMINT_TSENS_UPPER_LOWER_ISR,   
  HAL_MPMINT_MPM_INT_ISR,
  HAL_MPMINT_MPM_BIT_43_ISR,
  HAL_MPMINT_MPM_BIT_44_ISR,
  
  HAL_MPMINT_MPM_BIT_45_ISR,   /* 45 */
  HAL_MPMINT_SRST_N_ISR,
  HAL_MPMINT_USB2_PHY_SVLD_ISR,
  HAL_MPMINT_USB2_PHY_ID_ISR,
  HAL_MPMINT_USB2_PHY_DPSE_ISR,
  
  HAL_MPMINT_GPIO49_ISR,       /* 50 */
  HAL_MPMINT_USB20_POWER_EVENT_ISR,
  HAL_MPMINT_MPM_BIT_52_ISR,
  HAL_MPMINT_MPM_BIT_53_ISR,
  HAL_MPMINT_MPM_BIT_54_ISR, 
  
  HAL_MPMINT_MPM_BIT_55_ISR,                     /* 55 */
  HAL_MPMINT_MPM_BIT_56_ISR,
  HAL_MPMINT_MPM_BIT_57_ISR, 
  HAL_MPMINT_USB2_PHY_DMSE_ISR,
  HAL_MPMINT_MPM_BIT_59_ISR,
  
  HAL_MPMINT_UIM_CONTROLLER_CARD_EVENT_ISR,      /* 60 */
  HAL_MPMINT_UIM_CONTROLLER_BATT_ISR,
  HAL_MPMINT_MPM_SPMI_WAKE_ISR,
  HAL_MPMINT_MPM_BIT_63_ISR,
  
  
  HAL_MPMINT_NUM,
  HAL_MPMINT_NOT_DEFINED_ISR                = HAL_MPMINT_NUM,

} HAL_mpmint_IsrType;
                                                         
#ifdef __cplusplus
}
#endif

#endif /* HAL_MPMINT9x25_H */



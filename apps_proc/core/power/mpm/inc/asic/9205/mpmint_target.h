#ifndef MPMINT_TARGET_H
#define MPMINT_TARGET_H

/*===========================================================================

                M P M   I N T E R R U P T   C O N T R O L L E R

GENERAL DESCRIPTION
  This file contains the 9x35 specific definitions for the MPM
  Interrupt Controller.

Copyright (c) 2013-2018 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

$Header: //components/rel/core.tx/6.0/power/mpm/inc/asic/9205/mpmint_target.h#1 $ 
   
===========================================================================*/


/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/** 
 * A list of all of the supported MPM interrupts.  
 * NOTE:  The hardware does not necessarily support all of these interrupts.
 * IMPORTANT: This list must be in the same order as
 *            HAL_mpmint_IsrType for this target.
 */
typedef enum
{
  MPM_MPM_BIT_00_ISR,   /* 0 */
  MPM_MPM_BIT_01_ISR,
  MPM_MPM_BIT_02_ISR,
  MPM_MPM_BIT_03_ISR,
  MPM_GPIO5_ISR,

  MPM_GPIO0_ISR,        /* 5 */
  MPM_GPIO2_ISR,
  MPM_MPM_BIT_07_ISR,
  MPM_MPM_BIT_08_ISR,
  MPM_GPIO9_ISR,
  
  MPM_GPIO13_ISR,        /* 10 */
  MPM_GPIO1_ISR,
  MPM_GPIO20_ISR,
  MPM_GPIO21_ISR,
  MPM_GPIO22_ISR,
  
  MPM_GPIO50_ISR,       /* 15 */
  MPM_GPIO52_ISR,
  MPM_GPIO28_ISR,   
  MPM_GPIO51_ISR,   
  MPM_GPIO26_ISR,
  
  MPM_GPIO4_ISR,       /* 20 */
  MPM_GPIO47_ISR,  
  MPM_GPIO6_ISR,    
  MPM_GPIO8_ISR,
  MPM_GPIO14_ISR,
 
  MPM_GPIO19_ISR,       /* 25 */
  MPM_GPIO27_ISR,     
  MPM_GPIO30_ISR,   
  MPM_GPIO36_ISR,
  MPM_GPIO46_ISR,         
 
  MPM_MPM_BIT_30_ISR,       /* 30 */
  MPM_MPM_BIT_31_ISR,
  MPM_MPM_BIT_32_ISR,
  MPM_MPM_BIT_33_ISR,
  MPM_MPM_BIT_34_ISR,
 
  MPM_MPM_BIT_35_ISR,       /* 35 */
  MPM_MPM_BIT_36_ISR,
  MPM_MPM_BIT_37_ISR,
  MPM_MPM_BIT_38_ISR,
  MPM_MPM_BIT_39_ISR,
 
  MPM_MPM_BIT_40_ISR,       /* 40 */
  MPM_TSENS_UPPER_LOWER_ISR,   
  MPM_MPM_INT_ISR,
  MPM_MPM_BIT_43_ISR,
  MPM_MPM_BIT_44_ISR,
 
  MPM_MPM_BIT_45_ISR,   /* 45 */
  MPM_SRST_N_ISR,
  MPM_USB2_PHY_SVLD_ISR,
  MPM_USB2_PHY_ID_ISR,
  MPM_USB2_PHY_DPSE_ISR,

  MPM_GPIO49_ISR,       /* 50 */
  MPM_USB20_POWER_EVENT_ISR,
  MPM_MPM_BIT_52_ISR,
  MPM_MPM_BIT_53_ISR,
  MPM_MPM_BIT_54_ISR, 

  MPM_MPM_BIT_55_ISR,       /* 55 */
  MPM_MPM_BIT_56_ISR,
  MPM_MPM_BIT_57_ISR, 
  MPM_USB2_PHY_DMSE_ISR,
  MPM_MPM_BIT_59_ISR,

  MPM_UIM_CONTROLLER_CARD_EVENT_ISR,       /* 60 */
  MPM_UIM_CONTROLLER_BATT_ISR,
  MPM_MPM_SPMI_WAKE_ISR,
  MPM_MPM_BIT_63_ISR,
  
  MPMINT_NUM_INTS,                         
  MPMINT_NOT_DEFINED = MPMINT_NUM_INTS,
  MPM_RESERVED_ISR   = MPMINT_NOT_DEFINED,
} mpmint_isr_type;                         
                                           
#endif  /* MPMINT_TARGET_H */
 

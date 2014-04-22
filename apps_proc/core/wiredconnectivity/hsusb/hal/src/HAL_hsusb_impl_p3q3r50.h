#ifndef HAL_HSUSB_impl_p3q3r50
#define HAL_HSUSB_impl_p3q3r50
/*
===========================================================================

FILE:         HAL_hsusb_impl_p3q3r26.h

DESCRIPTION:  
This is the external interface for the HS-USB core usb_otg_hs_lpm_p3q3r50d0 implementation.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/hal/src/HAL_hsusb_impl_p3q3r50.h#1 $

===========================================================================

===========================================================================
Copyright © 2009-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INTERFACES
** ----------------------------------------------------------------------- */

#ifndef T_REXNT
	#include "HAL_hsusb_impl_p3q2r13.h"
#endif /* T_REXNT */

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* Qualcomm registers */
#define HAL_HSUSB_L1_EP_CTRL_ADDR                       0x00000250
#define HAL_HSUSB_L1_EP_CTRL_TX_EP_PRIME_L1_EN_BMSK     0x0000FFFF
#define HAL_HSUSB_L1_EP_CTRL_TX_EP_PRIME_L1_EN_SHFT              0
#define HAL_HSUSB_L1_EP_CTRL_TX_EP_PRIME_L1_EXIT_BMSK   0xFFFF0000
#define HAL_HSUSB_L1_EP_CTRL_TX_EP_PRIME_L1_EXIT_SHFT         0x10

#define HAL_HSUSB_L1_CONFIG_ADDR                        0x00000254
#define HAL_HSUSB_L1_CONFIG_PLL_TURNOFF_MIN_HIRD_BMSK          0xF
#define HAL_HSUSB_L1_CONFIG_PLL_TURNOFF_MIN_HIRD_SHFT          0x0
#define HAL_HSUSB_L1_CONFIG_LPM_EN_BMSK                       0x10
#define HAL_HSUSB_L1_CONFIG_LPM_EN_SHFT                        0x4
#define HAL_HSUSB_L1_CONFIG_L1_REMOTE_WAKEUP_EN_BMSK          0x20
#define HAL_HSUSB_L1_CONFIG_L1_REMOTE_WAKEUP_EN_SHFT           0x5
#define HAL_HSUSB_L1_CONFIG_GATE_XCVR_CLK_EN_BMSK             0x40
#define HAL_HSUSB_L1_CONFIG_GATE_XCVR_CLK_EN_SHFT              0x6
#define HAL_HSUSB_L1_CONFIG_GATE_SYS_CLK_EN_BMSK              0x80
#define HAL_HSUSB_L1_CONFIG_GATE_SYS_CLK_EN_SHFT               0x7
#define HAL_HSUSB_L1_CONFIG_GATE_FS_XCVR_CLK_EN_BMSK         0x100
#define HAL_HSUSB_L1_CONFIG_GATE_FS_XCVR_CLK_EN_SHFT           0x8
#define HAL_HSUSB_L1_CONFIG_GATE_AHB_CLK_EN_BMSK             0x200
#define HAL_HSUSB_L1_CONFIG_GATE_AHB_CLK_EN_SHFT               0x9
#define HAL_HSUSB_L1_CONFIG_PHY_LPM_EN_BMSK                  0x400
#define HAL_HSUSB_L1_CONFIG_PHY_LPM_EN_SHFT                    0xA
#define HAL_HSUSB_L1_CONFIG_PLL_PWR_DWN_EN_BMSK              0x800
#define HAL_HSUSB_L1_CONFIG_PLL_PWR_DWN_EN_SHFT                0xB

#define HAL_HSUSB_LPM_DEBUG_1_ADDR                            0x00000258
#define HAL_HSUSB_LPM_DEBUG_1_DEBUG_L1_SHORT_ENT_CNT_BMSK     0x0000FFFF
#define HAL_HSUSB_LPM_DEBUG_1_DEBUG_L1_SHORT_ENT_CNT_SHFT              0
#define HAL_HSUSB_LPM_DEBUG_1_DEBUG_L1_LONG_ENT_CNT_BMSK      0xFFFF0000
#define HAL_HSUSB_LPM_DEBUG_1_DEBUG_L1_LONG_ENT_CNT_SHFT            0x10

#define HAL_HSUSB_LPM_DEBUG_2_ADDR                            0x0000025C
#define HAL_HSUSB_LPM_DEBUG_2_DEBUG_L1_EN_BMSK                       0x1
#define HAL_HSUSB_LPM_DEBUG_2_DEBUG_L1_EN_SHFT                       0x0
#define HAL_HSUSB_LPM_DEBUG_2_L1_STATE_BMSK                          0x2
#define HAL_HSUSB_LPM_DEBUG_2_L1_STATE_SHFT                          0x1
#define HAL_HSUSB_LPM_DEBUG_2_AHB_CLK_GATE_BMSK                      0x4
#define HAL_HSUSB_LPM_DEBUG_2_AHB_CLK_GATE_SHFT                      0x2
#define HAL_HSUSB_LPM_DEBUG_2_SYS_CLK_GATE_BMSK                      0x8
#define HAL_HSUSB_LPM_DEBUG_2_SYS_CLK_GATE_SHFT                      0x3
#define HAL_HSUSB_LPM_DEBUG_2_FS_XCVR_CLK_GATE_BMSK                 0x10
#define HAL_HSUSB_LPM_DEBUG_2_FS_XCVR_CLK_GATE_SHFT                  0x4
#define HAL_HSUSB_LPM_DEBUG_2_HSIC_CLK_GATE_BMSK                    0x20
#define HAL_HSUSB_LPM_DEBUG_2_HSIC_CLK_GATE_SHFT                     0x5
#define HAL_HSUSB_LPM_DEBUG_2_HSIC_CLK_PLL_RESET_BMSK               0x40
#define HAL_HSUSB_LPM_DEBUG_2_HSIC_CLK_PLL_RESET_SHFT                0x6
#define HAL_HSUSB_LPM_DEBUG_2_HSIC_CLK_PLL_BYPASSNL_BMSK            0x80
#define HAL_HSUSB_LPM_DEBUG_2_HSIC_CLK_PLL_BYPASSNL_SHFT             0x7
#define HAL_HSUSB_LPM_DEBUG_2_L1_FPR_BMSK                          0x100
#define HAL_HSUSB_LPM_DEBUG_2_L1_FPR_SHFT                            0x8
#define HAL_HSUSB_LPM_DEBUG_2_L1_RMT_WKUP_TIME_BMSK               0x1E00
#define HAL_HSUSB_LPM_DEBUG_2_L1_RMT_WKUP_TIME_SHFT                  0x9

#define HAL_HSUSB_LPM_ATTRIBUTES_ADDR                         0x00000260
#define HAL_HSUSB_LPM_ATTRIBUTES_HIRD_BMSK                           0xF
#define HAL_HSUSB_LPM_ATTRIBUTES_HIRD_SHFT                           0x0
#define HAL_HSUSB_LPM_ATTRIBUTES_BREMOTEWAKE_BMSK                   0x10
#define HAL_HSUSB_LPM_ATTRIBUTES_BREMOTEWAKE_SHFT                    0x4

/* -----------------------------------------------------------------------
**                           INTERFACES DESCRIPTION
** ----------------------------------------------------------------------- */

/* 
* C++ wrapper
*/
#ifdef __cplusplus
extern "C" {
#endif

void HAL_hsusb_ConstructImplp3q3r50(struct HAL_HSUSB_CoreIfType* this);

#ifdef __cplusplus
}
#endif

#endif /* HAL_HSUSB_impl_p3q3r50 */


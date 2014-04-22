#ifndef __HALCLKHFPLL_H__
#define __HALCLKHFPLL_H__
/*
==============================================================================

FILE:         HALclkHFPLL.h

DESCRIPTION:
  Internal, chipset specific PLL definitions for the clock HAL module.

==============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
07/11/11   vs      Created.

==============================================================================
  Copyright (c) 2018 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALclkInternal.h"



/* ============================================================================
**    Definitions
** ==========================================================================*/


/*
 * PLL register offsets.
 */
#define HWIO_PLL_MODE_OFFS                                  0x00000000
#define HWIO_PLL_L_VAL_OFFS                                 0x00000004
#define HWIO_PLL_M_VAL_OFFS                                 0x00000008
#define HWIO_PLL_N_VAL_OFFS                                 0x0000000c
#define HWIO_PLL_USER_CTL_OFFS                              0x00000010
#define HWIO_PLL_CONFIG_CTL_OFFS                            0x00000014
#define HWIO_PLL_TEST_CTL_OFFS                              0x00000018
#define HWIO_PLL_STATUS_OFFS                                0x0000001c
#define HWIO_PLL_DROOP_CTL_OFFS                             0x00000020
#define HWIO_PLL_SECURE_OFFS                                0x00000030



/*
 * PLL_MODE register definitions.
 */
#define HWIO_PLL_MODE_PLL_PLLTEST_BMSK                             0x8
#define HWIO_PLL_MODE_PLL_PLLTEST_SHFT                             0x3
#define HWIO_PLL_MODE_PLL_RESET_N_BMSK                             0x4
#define HWIO_PLL_MODE_PLL_RESET_N_SHFT                             0x2
#define HWIO_PLL_MODE_PLL_BYPASSNL_BMSK                            0x2
#define HWIO_PLL_MODE_PLL_BYPASSNL_SHFT                            0x1
#define HWIO_PLL_MODE_PLL_OUTCTRL_BMSK                             0x1
#define HWIO_PLL_MODE_PLL_OUTCTRL_SHFT                             0x0

/*
 * PLL_USER_CTL register definitions.
 */
#define HWIO_PLL_USER_CTL_MN_ACC_BMSK                        0x1000000
#define HWIO_PLL_USER_CTL_MN_ACC_SHFT                             0x18
#define HWIO_PLL_USER_CTL_CLK_REF_PRE_DIV_BMSK                  0x1000
#define HWIO_PLL_USER_CTL_CLK_REF_PRE_DIV_SHFT                     0xc
#define HWIO_PLL_USER_CTL_CLK_POST_DIV_BMSK                      0x300
#define HWIO_PLL_USER_CTL_CLK_POST_DIV_SHFT                        0x8
#define HWIO_PLL_USER_CTL_OUT_INV_EN_BMSK                         0x80
#define HWIO_PLL_USER_CTL_OUT_INV_EN_SHFT                          0x7
#define HWIO_PLL_USER_CTL_LVMST_EN_BMSK                           0x20
#define HWIO_PLL_USER_CTL_LVMST_EN_SHFT                            0x5
#define HWIO_PLL_USER_CTL_PLLOUT_LV_TEST_EN_BMSK                  0x10
#define HWIO_PLL_USER_CTL_PLLOUT_LV_TEST_EN_SHFT                   0x4
#define HWIO_PLL_USER_CTL_LVEARLY_EN_BMSK                          0x8
#define HWIO_PLL_USER_CTL_LVEARLY_EN_SHFT                          0x3
#define HWIO_PLL_USER_CTL_PLLOUT_LV_BIST_EN_BMSK                   0x4
#define HWIO_PLL_USER_CTL_PLLOUT_LV_BIST_EN_SHFT                   0x2
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX_EN_BMSK                    0x2
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX_EN_SHFT                    0x1
#define HWIO_PLL_USER_CTL_LVMAIN_EN_BMSK                           0x1
#define HWIO_PLL_USER_CTL_LVMAIN_EN_SHFT                           0x0


/*
 * PLL_STATUS register definitions.
 */
#define HWIO_PLL_STATUS_LOCK_DET_BMSK                      0x10000
#define HWIO_PLL_STATUS_LOCK_DET_SHFT                         0x10
#define HWIO_PLL_STATUS_OV_VLT_CNT_STS_BMSK                   0x3C
#define HWIO_PLL_STATUS_OV_VLT_CNT_STS_SHFT                    0x2
#define HWIO_PLL_STATUS_OV_VLT_DET_BMSK                        0x2
#define HWIO_PLL_STATUS_OV_VLT_DET_SHFT                        0x1
#define HWIO_PLL_STATUS_CLK_N_BMSK                             0x1
#define HWIO_PLL_STATUS_CLK_N_SHFT                             0x0

/*
 * PLL_SECURE register definitions.
 */
#define HWIO_PLL_SECURE_PLL_CTL_BMSK                             0x1
#define HWIO_PLL_SECURE_PLL_CTL_SHFT                             0x0

/* ============================================================================
**    External Definitions
** ==========================================================================*/


/* ============================================================================
**    External Definitions
** ==========================================================================*/


/* ============================================================================
**    Functions
** ==========================================================================*/

/*
 * Source control functions.  These can be used if some of the control
 * needs to be overridden by a specific PLL.
 */
static boolean HAL_clk_HFPLLEnable           ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData );
static void    HAL_clk_HFPLLDisable          ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData );
static boolean HAL_clk_HFPLLIsEnabled        ( HAL_clk_SourceDescType *pSource );
static void    HAL_clk_HFPLLConfig           ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceConfigType eConfig );
static boolean HAL_clk_HFPLLConfigPLL        ( HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pmConfig, HAL_clk_SourceConfigMode eMode );
static void    HAL_clk_HFPLLDetectPLLConfig  ( HAL_clk_SourceDescType *pSource, HAL_clk_PLLConfigType *pmConfig );
static boolean HAL_clk_HFPLLEnableVote       ( HAL_clk_SourceDescType *pSource );
static void    HAL_clk_HFPLLDisableVote      ( HAL_clk_SourceDescType *pSource );
static boolean HAL_clk_HFPLLIsVoteEnabled    ( HAL_clk_SourceDescType *pSource );


#endif  /* __HALCLKHFPLL_H__ */


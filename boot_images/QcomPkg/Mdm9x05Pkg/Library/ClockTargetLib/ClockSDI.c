/**
==============================================================================

  @file ClockSDI.c
  
  Clock functions for support System Debug Image driver.  Since SDI image is
  required to be every small memory footprint, clock drivers only brings in
  necessary code to support it.

==============================================================================

                             Edit History


when      who           what, where, why
--------  ------------  ------------------------------------------------------
03/02/18  avk           Initial QCS405 revision, branched from SDM660.

==============================================================================
            Copyright (c) 2018 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR

==============================================================================
*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "ClockHWIO.h"
#include "busywait.h"
#include "ddr_drivers.h"

/*=========================================================================
                       MACRO DEFINITIONS
==========================================================================*/

/*stromer */

/*
 * PLL register offsets.
 */
#define HWIO_GCC_STROMER_PLL_MODE_OFFS                                  0x00000000
#define HWIO_GCC_STROMER_PLL_L_VAL_OFFS                                 0x00000004
#define HWIO_GCC_STROMER_PLL_ALPHA_VAL_OFFS                             0x00000008
#define HWIO_GCC_STROMER_PLL_ALPHA_VAL_U_OFFS                           0x0000000c
#define HWIO_GCC_STROMER_PLL_USER_CTL_OFFS                              0x00000010
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_OFFS                            0x00000014
#define HWIO_GCC_STROMER_PLL_CONFIG_CTL_OFFS                            0x00000018
#define HWIO_GCC_STROMER_PLL_TEST_CTL_OFFS                              0x0000001c
#define HWIO_GCC_STROMER_PLL_TEST_CTL_U_OFFS                            0x00000020
#define HWIO_GCC_STROMER_PLL_STATUS_OFFS                                0x00000024

/*
 * PLL_MODE register definitions.
 */
#define HWIO_GCC_STROMER_PLL_MODE_PLL_LOCK_DET_BMSK                     0x80000000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_LOCK_DET_SHFT                           0x1f
#define HWIO_GCC_STROMER_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                  0x40000000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                        0x1e
#define HWIO_GCC_STROMER_PLL_MODE_PLL_ACK_LATCH_BMSK                    0x20000000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_ACK_LATCH_SHFT                          0x1d
#define HWIO_GCC_STROMER_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK         0x800000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT             0x17
#define HWIO_GCC_STROMER_PLL_MODE_PLL_UPDATE_BMSK                         0x400000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_UPDATE_SHFT                             0x16
#define HWIO_GCC_STROMER_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                 0x200000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                     0x15
#define HWIO_GCC_STROMER_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                   0x100000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                       0x14
#define HWIO_GCC_STROMER_PLL_MODE_PLL_BIAS_COUNT_BMSK                      0xfc000
#define HWIO_GCC_STROMER_PLL_MODE_PLL_BIAS_COUNT_SHFT                          0xe
#define HWIO_GCC_STROMER_PLL_MODE_PLL_LOCK_COUNT_BMSK                       0x3f00
#define HWIO_GCC_STROMER_PLL_MODE_PLL_LOCK_COUNT_SHFT                          0x8
#define HWIO_GCC_STROMER_PLL_MODE_PLL_PLLTEST_BMSK                             0x8
#define HWIO_GCC_STROMER_PLL_MODE_PLL_PLLTEST_SHFT                             0x3
#define HWIO_GCC_STROMER_PLL_MODE_PLL_RESET_N_BMSK                             0x4
#define HWIO_GCC_STROMER_PLL_MODE_PLL_RESET_N_SHFT                             0x2
#define HWIO_GCC_STROMER_PLL_MODE_PLL_BYPASSNL_BMSK                            0x2
#define HWIO_GCC_STROMER_PLL_MODE_PLL_BYPASSNL_SHFT                            0x1
#define HWIO_GCC_STROMER_PLL_MODE_PLL_OUTCTRL_BMSK                             0x1
#define HWIO_GCC_STROMER_PLL_MODE_PLL_OUTCTRL_SHFT                             0x0

/*
 * PLL_USER_CTL register definitions.
 */
#define HWIO_GCC_STROMER_PLL_USER_CTL_SSC_MODE_CONTROL_BMSK              0x8000000
#define HWIO_GCC_STROMER_PLL_USER_CTL_SSC_MODE_CONTROL_SHFT                   0x1b
#define HWIO_GCC_STROMER_PLL_USER_CTL_ALPHA_EN_BMSK                      0x1000000
#define HWIO_GCC_STROMER_PLL_USER_CTL_ALPHA_EN_SHFT                           0x18
#define HWIO_GCC_STROMER_PLL_USER_CTL_VCO_SEL_BMSK                        0x300000
#define HWIO_GCC_STROMER_PLL_USER_CTL_VCO_SEL_SHFT                            0x14
#define HWIO_GCC_STROMER_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                    0x7000
#define HWIO_GCC_STROMER_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                       0xc
#define HWIO_GCC_STROMER_PLL_USER_CTL_POST_DIV_RATIO_BMSK                    0x300
#define HWIO_GCC_STROMER_PLL_USER_CTL_POST_DIV_RATIO_SHFT                      0x8
#define HWIO_GCC_STROMER_PLL_USER_CTL_OUTPUT_INV_BMSK                         0x80
#define HWIO_GCC_STROMER_PLL_USER_CTL_OUTPUT_INV_SHFT                          0x7
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                     0x10
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                      0x4
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                     0x8
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                     0x3
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                      0x4
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                      0x2
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                       0x2
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                       0x1
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                      0x1
#define HWIO_GCC_STROMER_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                      0x0


/*
 * PLL_USER_CTL_U register definitions.
 */
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK          0x800
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT            0xb
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                 0x700
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                   0x8
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_DSM_BMSK                              0x80
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_DSM_SHFT                               0x7
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_WRITE_STATE_BMSK                      0x40
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_WRITE_STATE_SHFT                       0x6
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_TARGET_CTL_BMSK                       0x38
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_TARGET_CTL_SHFT                        0x3
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_LOCK_DET_BMSK                          0x4
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_LOCK_DET_SHFT                          0x2
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_FREEZE_PLL_BMSK                        0x2
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_FREEZE_PLL_SHFT                        0x1
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_TOGGLE_DET_BMSK                        0x1
#define HWIO_GCC_STROMER_PLL_USER_CTL_U_TOGGLE_DET_SHFT                        0x0




/*SR PLL */


/*
 * PLL register offsets.
 */
#define HWIO_GCC_SR_PLL_MODE_OFFS                                  0x00000000
#define HWIO_GCC_SR_PLL_L_VAL_OFFS                                 0x00000004
#define HWIO_GCC_SR_PLL_M_VAL_OFFS                                 0x00000008
#define HWIO_GCC_SR_PLL_N_VAL_OFFS                                 0x0000000c
#define HWIO_GCC_SR_PLL_USER_CTL_OFFS                              0x00000010
#define HWIO_GCC_SR_PLL_CONFIG_CTL_OFFS                            0x00000014
#define HWIO_GCC_SR_PLL_TEST_CTL_OFFS                              0x00000018
#define HWIO_GCC_SR_PLL_STATUS_OFFS                                0x0000001c

/*
 * PLL_MODE register definitions.
 */
#define HWIO_GCC_SR_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                 0x200000
#define HWIO_GCC_SR_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                     0x15
#define HWIO_GCC_SR_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                   0x100000
#define HWIO_GCC_SR_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                       0x14
#define HWIO_GCC_SR_PLL_MODE_PLL_BIAS_COUNT_BMSK                      0xfc000
#define HWIO_GCC_SR_PLL_MODE_PLL_BIAS_COUNT_SHFT                          0xe
#define HWIO_GCC_SR_PLL_MODE_PLL_LOCK_COUNT_BMSK                       0x3f00
#define HWIO_GCC_SR_PLL_MODE_PLL_LOCK_COUNT_SHFT                          0x8
#define HWIO_GCC_SR_PLL_MODE_PLL_PLLTEST_BMSK                             0x8
#define HWIO_GCC_SR_PLL_MODE_PLL_PLLTEST_SHFT                             0x3
#define HWIO_GCC_SR_PLL_MODE_PLL_RESET_N_BMSK                             0x4
#define HWIO_GCC_SR_PLL_MODE_PLL_RESET_N_SHFT                             0x2
#define HWIO_GCC_SR_PLL_MODE_PLL_BYPASSNL_BMSK                            0x2
#define HWIO_GCC_SR_PLL_MODE_PLL_BYPASSNL_SHFT                            0x1
#define HWIO_GCC_SR_PLL_MODE_PLL_OUTCTRL_BMSK                             0x1
#define HWIO_GCC_SR_PLL_MODE_PLL_OUTCTRL_SHFT                             0x0

/*
 * PLL_USER_CTL register definitions.
 */
#define HWIO_GCC_SR_PLL_USER_CTL_ADDITIVE_FACTOR_BMSK              0xc0000000
#define HWIO_GCC_SR_PLL_USER_CTL_ADDITIVE_FACTOR_SHFT                    0x1e
#define HWIO_GCC_SR_PLL_USER_CTL_VCO_DOUBLER_QUADRUPLER_MODE_BMSK  0x30000000
#define HWIO_GCC_SR_PLL_USER_CTL_VCO_DOUBLER_QUADRUPLER_MODE_SHFT        0x1c
#define HWIO_GCC_SR_PLL_USER_CTL_MN_EN_BMSK                         0x1000000
#define HWIO_GCC_SR_PLL_USER_CTL_MN_EN_SHFT                              0x18
#define HWIO_GCC_SR_PLL_USER_CTL_VCO_SEL_BMSK                        0x300000
#define HWIO_GCC_SR_PLL_USER_CTL_VCO_SEL_SHFT                            0x14
#define HWIO_GCC_SR_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                    0x7000
#define HWIO_GCC_SR_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                       0xc
#define HWIO_GCC_SR_PLL_USER_CTL_POST_DIV_RATIO_BMSK                    0x300
#define HWIO_GCC_SR_PLL_USER_CTL_POST_DIV_RATIO_SHFT                      0x8
#define HWIO_GCC_SR_PLL_USER_CTL_OUTPUT_INV_BMSK                         0x80
#define HWIO_GCC_SR_PLL_USER_CTL_OUTPUT_INV_SHFT                          0x7
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_DIFF_90_BMSK                     0x40
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_DIFF_90_SHFT                      0x6
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_DIFF_0_BMSK                      0x20
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_DIFF_0_SHFT                       0x5
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                     0x10
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                      0x4
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                     0x8
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                     0x3
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                      0x4
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                      0x2
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                       0x2
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                       0x1
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                      0x1
#define HWIO_GCC_SR_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                      0x0


/*
 * PLL_STATUS register definitions.
 */
#define HWIO_GCC_SR_PLL_STATUS_PLL_ACTIVE_FLAG_BMSK                   0x20000
#define HWIO_GCC_SR_PLL_STATUS_PLL_ACTIVE_FLAG_SHFT                      0x11
#define HWIO_GCC_SR_PLL_STATUS_PLL_LOCK_DET_BMSK                      0x10000
#define HWIO_GCC_SR_PLL_STATUS_PLL_LOCK_DET_SHFT                         0x10
#define HWIO_GCC_SR_PLL_STATUS_PLL_D_BMSK                              0xffff
#define HWIO_GCC_SR_PLL_STATUS_PLL_D_SHFT                                 0x0





#define HWIO_PLL_MODE_OFFS_STROMER                 HWIO_OFFS(GCC_STROMER_PLL_MODE)
#define HWIO_PLL_L_VAL_OFFS_STROMER                HWIO_OFFS(GCC_STROMER_PLL_L_VAL)
#define HWIO_PLL_ALPHA_VAL_OFFS_STROMER            HWIO_OFFS(GCC_STROMER_PLL_ALPHA_VAL)
#define HWIO_PLL_ALPHA_VAL_U_OFFS_STROMER          HWIO_OFFS(GCC_STROMER_PLL_ALPHA_VAL_U)
#define HWIO_PLL_USER_CTL_OFFS_STROMER             HWIO_OFFS(GCC_STROMER_PLL_USER_CTL)
#define HWIO_PLL_CONFIG_CTL_OFFS_STROMER           HWIO_OFFS(GCC_STROMER_PLL_CONFIG_CTL)
#define HWIO_PLL_TEST_CTL_OFFS_STROMER             HWIO_OFFS(GCC_STROMER_PLL_TEST_CTL)


#define HWIO_PLL_MODE_OFFS_SR                 HWIO_OFFS(GCC_SR_PLL_MODE)
#define HWIO_PLL_L_VAL_OFFS_SR                HWIO_OFFS(GCC_SR_PLL_L_VAL)
#define HWIO_PLL_M_VAL_OFFS_SR                HWIO_OFFS(GCC_SR_PLL_M_VAL)
#define HWIO_PLL_N_VAL_OFFS_SR                HWIO_OFFS(GCC_SR_PLL_N_VAL)
#define HWIO_PLL_USER_CTL_OFFS_SR             HWIO_OFFS(GCC_SR_PLL_USER_CTL)
#define HWIO_PLL_CONFIG_CTL_OFFS_SR           HWIO_OFFS(GCC_SR_PLL_CONFIG_CTL)
#define HWIO_PLL_TEST_CTL_OFFS_SR             HWIO_OFFS(GCC_SR_PLL_TEST_CTL)
#define HWIO_PLL_STATUS_OFFS_SR               HWIO_OFFS(GCC_SR_PLL_STATUS)




#define CLOCK_PLL_MODE_ACTIVE  \
  (HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_OUTCTRL)  | \
   HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_BYPASSNL) | \
   HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_RESET_N))

/*
 * Definitions for configuring the PLL in FSM Mode
 */
#define CLOCK_SRPLL_BIAS_COUNT_VAL  (0x0 << HWIO_SHFT(GCC_SR_PLL_MODE, PLL_BIAS_COUNT))
#define CLOCK_PLL_BIAS_COUNT_VAL  (0x1 << HWIO_SHFT(GCC_STROMER_PLL_MODE, PLL_BIAS_COUNT))
#define CLOCK_PLL_LOCK_COUNT_VAL  (0x0 << HWIO_SHFT(GCC_STROMER_PLL_MODE, PLL_LOCK_COUNT))

/*
 * CLOCK_PLL_INIT_TIME_US
 *
 * Init time in microseconds after de-asserting PLL reset (normal mode).
 */
#define CLOCK_PLL_INIT_TIME_US 5

/*
 * CLOCK_PLL_WARMUP_TIME_US
 *
 * Warm-up time in microseconds after turning on the PLL.
 */
#define CLOCK_PLL_WARMUP_TIME_US 50

/*
 * NOT_2D / NOT_N_MINUS_M
 *
 * Macros to return the inverted value of the 2D field or (N - M)
 * in a type 1 mux structure.  Used to prepare the value for writing
 * to the hardware register field.
 */
#define NOT_2D(mux)         (~(mux)->n2D)
#define NOT_N_MINUS_M(mux)  (~((mux)->nN - (mux)->nM))

/*
 * HALF_DIVIDER
 *
 * Macro to return the normalized half divider for a given mux structure.
 * NOTE: Expecting (2 * divider) value as input.
 */
#define HALF_DIVIDER(mux)  ((mux)->nDiv2x ? (((mux)->nDiv2x) - 1) : 0)

/*
 * PLL lock timeout (2x max locking time).
 */
#define CLOCK_PLL_LOCK_TIMEOUT_US 200



/* Definitions for generalizing clock configuration */
#define CLOCK_CMD_CFG_UPDATE_FMSK   HWIO_FMSK(GCC_PCNOC_BFDCD_CMD_RCGR, UPDATE)

#define CLOCK_CFG_REG_OFFSET        (HWIO_ADDR(GCC_PCNOC_BFDCD_CFG_RCGR)-HWIO_ADDR(GCC_PCNOC_BFDCD_CMD_RCGR))
#define CLOCK_CFG_CGR_SRC_SEL_FMSK  HWIO_FMSK(GCC_PCNOC_BFDCD_CFG_RCGR, SRC_SEL)
#define CLOCK_CFG_CGR_SRC_SEL_SHFT  HWIO_SHFT(GCC_PCNOC_BFDCD_CFG_RCGR, SRC_SEL)
#define CLOCK_CFG_CGR_SRC_DIV_FMSK  HWIO_FMSK(GCC_PCNOC_BFDCD_CFG_RCGR, SRC_DIV)
#define CLOCK_CFG_CGR_SRC_DIV_SHFT  HWIO_SHFT(GCC_PCNOC_BFDCD_CFG_RCGR, SRC_DIV)

#define CLOCK_CFG_CGR_MODE_FMSK     HWIO_FMSK(GCC_BLSP1_UART4_APPS_CFG_RCGR, MODE)
#define CLOCK_CFG_CGR_MODE_SHFT     HWIO_SHFT(GCC_BLSP1_UART4_APPS_CFG_RCGR, MODE)
#define CLOCK_CFG_CGR_MODE_DUAL_EDGE_VAL  0x2
#define CLOCK_CFG_CGR_MODE_BYPASS_VAL     0x0

#define CLOCK_M_REG_OFFSET         (HWIO_ADDR(GCC_BLSP1_UART4_APPS_M)-HWIO_ADDR(GCC_BLSP1_UART4_APPS_CMD_RCGR))
#define CLOCK_N_REG_OFFSET         (HWIO_ADDR(GCC_BLSP1_UART4_APPS_N)-HWIO_ADDR(GCC_BLSP1_UART4_APPS_CMD_RCGR))
#define CLOCK_D_REG_OFFSET         (HWIO_ADDR(GCC_BLSP1_UART4_APPS_D)-HWIO_ADDR(GCC_BLSP1_UART4_APPS_CMD_RCGR))

#define CLOCK_UPDATE_TIMEOUT_US 1000

#define CLK_ENABLE_MSK 0x00000001
#define CLK_OFF_MSK    0x80000000


typedef enum{
  CLK_SOURCE_PLL_SR,
  CLK_SOURCE_PLL_SR2,
  CLK_SOURCE_PLL_STROMER,
  CLK_SOURCE_PLL_SR1,
  CLK_SOURCE_PLL_SPARK,
  CLK_SOURCE_PLL_BRAMMO,
  NUM_CLK_SOURCE_PLL_TYPES
} ClockSDI_SourcePllType;
/*
 * ClockSDIConfigPLLType
 *
 * Parameters used for configuring a source that is a PLL.
 *
 *  nPLLModeAddr  - The address of the PLL MODE register.
 *  nVoteAddr     - The address of the PLL voting register. Set to NULL for 
 *                  non-FSM mode.
 *  nVoteMask     - The mask of the voting bit.
 *  eVCO          - The internal VCO to use.
 *  nPreDiv       - The pre-divider value (generally 1 or 2).
 *  nPostDiv      - The post-divider value 
 *  nL            - The L value for the PLL.  The PLL output frequency is derived
 *                  as out_freq = ((in_freq / nPreDiv) * (L + M/N)) / nPostDiv.
 *  nM            - The M value (see above).
 *  nN            - The N value (see above).
 *  nAlpha        - The Alpha(prog) value. For the 20nm PLLs, the output frequency is
 *                  derived as :
 *                  out_freq = ((in_freq / nPreDiv) * (L + ALPHAfrac)) / nPostDiv.
 *                  ALPHAprog = 2^b x ALPHAfrac where 'b' is 40 for Prius and 40 for Tesla.
 */
typedef struct ClockSDIConfigPLL
{
  size_t nPLLModeAddr;
  size_t nVoteAddr;
  size_t nVoteMask;
  size_t nVCO;
  uint8  nPreDiv;
  uint8  nPostDiv;
  uint16 nL;
  uint16  nM;
  uint16  nN;
  uint64 nConfigCtl;
  uint64 nTestCtl;
  uint64 nAlpha;
  ClockSDI_SourcePllType ePLL;
} ClockSDIConfigPLLType;

/*
 * ClockSDIConfigMuxType
 *
 * Parameters used for configuring a standard clock multiplexer.
 *
 *  nCMDRCGRAddr - The address of the CMD RCGR register.
 *  eSource     - The source to use [RCG i/p selection value].
 *  nDiv2x      - The integer (2 * divider) value to use.
 *  nM          - The M value for any M/N counter, or 0 to bypass.
 *  nN          - The N value for any M/N counter.
 *  n2D         - Twice the D value for any M/N counter.
 */
typedef struct ClockSDIConfigMux
{
  uintnt nCMDRCGRAddr;
  uint32 nSource; // Originally 'ClockSourceType'.
  uint32 nDiv2x;
  uint32 nM;
  uint32 nN;
  uint32 n2D;

} ClockSDIConfigMuxType;

/*
 * ClockSDI_CBCRtoggleType
 *
 * A type to choose the the operation on clocks(enable/disable).
 */
typedef enum{
  CLK_TOGGLE_DISABLE    = 0,
  CLK_TOGGLE_ENABLE     = 1,
  NUM_CLK_TOGGLE_TYPES
} ClockSDI_CBCRtoggleType;

/*=========================================================================
      Data
==========================================================================*/
#if 0 //Enable the code if need to run the BIMC and its interface clocks at NOMINAL speed

/* GPLL0 @ 800 MHz (general purpose PLL). */
static ClockSDIConfigPLLType PLL0_Cfg =
  {
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL0_MODE), 
    .nVoteAddr     =  HWIO_ADDR(GCC_APCS_GPLL_ENA_VOTE), 
    .nVoteMask     =  HWIO_FMSK(GCC_APCS_GPLL_ENA_VOTE, GPLL0),
    .nVCO          =  1, 
    .nPreDiv       =  1, 
    .nPostDiv      =  1, 
    .nL            =  41, 
    .nM            =  0,
    .nN            =  1,
    .nAlpha        =  0xAAAA000000,
    .nConfigCtl    =  0x4001055B,
    .ePLL          =  CLK_SOURCE_PLL_STROMER
  };

#endif
/* GPLL5 @ 844.8 MHz (Used by BIMC_DDR and BIMC_GPU) */

/*
static ClockSDIConfigPLLType PLL2_Cfg = 
  {
      .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL2_MODE), 
      .nVoteAddr     =  0,
      .nVoteMask     =  0, 
      .nVCO          =  1, 
      .nPreDiv       =  1, 
      .nPostDiv      =  1, 
      .nL            =  44, 
      .nM            =  0,
      .nN            =  1,
      .nConfigCtl    =  0x4C015765,
      .ePLL          = CLK_SOURCE_PLL_SR2	  
  };
*/
/*=========================================================================
      Function Prototypes
==========================================================================*/
static boolean ClockSDI_ConfigurePLL  ( const ClockSDIConfigPLLType *pConfig );
static boolean ClockSDI_EnablePLL     ( const ClockSDIConfigPLLType *pConfig );
static boolean ClockSDI_ConfigMux     ( const ClockSDIConfigMuxType *pConfig );
static boolean ClockSDI_TriggerUpdate ( uintnt cmdReg, uintnt nMask );
static boolean ClockSDI_ToggleClock   ( uintnt CBCR_addr, ClockSDI_CBCRtoggleType toggle_clk );

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/* =========================================================================
**  Function : ClockSDI_TriggerUpdate
** =========================================================================*/
/*!
    Clock switch and wait for UPDATE bit to complete

    @param cmdReg - Address of the CBCR register

    TRUE -- CBCR programming successful.
    FALSE -- CBCR programming failed.

    @dependencies
    None.

    @sa None
*/
static boolean ClockSDI_TriggerUpdate(uintnt cmdReg, uintnt nMask)
{
  uintnt nVal;

  nVal = inp32(cmdReg) | nMask;

  /* Trigger CMD_REG:UPDATE */
  outp32(cmdReg, nVal);

  /* Wait for CMD_REG:UPDATE to clear, showing the clock switch is complete */
  while (inp32(cmdReg) & nMask);

  return TRUE;
}


/* =========================================================================
**  Function : ClockSDI_ToggleClock
** =========================================================================*/
/*!
    Enable/Disable a Clock and poll for CLK_OFF BIT. 

    @param CBCR_addr - Address of the CBCR register
           enable :-  enable/disable the CBCR 
    TRUE -- CBCR programming successful.
    FALSE -- CBCR programming failed.

    @dependencies
    None.

    @sa None
*/
static boolean ClockSDI_ToggleClock(uintnt CBCR_addr, ClockSDI_CBCRtoggleType toggle_clk)
{
  uintnt CBCR_value;
  
  if(toggle_clk >= NUM_CLK_TOGGLE_TYPES) return FALSE;

  CBCR_value = inp32(CBCR_addr);
  
  if(toggle_clk == CLK_TOGGLE_ENABLE)  
  {
    CBCR_value = CBCR_value | CLK_ENABLE_MSK;
    outp32(CBCR_addr, CBCR_value);  
    do
    {
      CBCR_value = inp32(CBCR_addr);
    }while((CBCR_value & CLK_OFF_MSK) != 0);
    
  }
  else
  {
    CBCR_value = CBCR_value & ~CLK_ENABLE_MSK;
    outp32(CBCR_addr, CBCR_value);  
  }
  return TRUE;
}

/* =========================================================================
**  Function : ClockSDI_ConfigurePLL
** =========================================================================*/
/**
  Configures a PLL.

  @param *pConfig [in] -- PLL configuration

*/
static boolean ClockSDI_ConfigurePLL(const ClockSDIConfigPLLType *pConfig)
{
  uintnt nModeAddr;
  uint32 nUserVal, nModeVal;

  nModeAddr = pConfig->nPLLModeAddr;
  nModeVal = inp32(nModeAddr);

  if((nModeVal & CLOCK_PLL_MODE_ACTIVE) == CLOCK_PLL_MODE_ACTIVE)
  {
    return FALSE;
  }
  else if(nModeVal & HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_VOTE_FSM_ENA)) 
  {
    if((pConfig->nVoteAddr)&&(inp32(pConfig->nVoteAddr) & pConfig->nVoteMask))
    {
      return FALSE;
    }
  }

  if(pConfig->ePLL == CLK_SOURCE_PLL_STROMER)
  { /*
     * Program L and Alpha.
     */
    outp32(nModeAddr + HWIO_PLL_L_VAL_OFFS_STROMER, pConfig->nL);
    outp32(nModeAddr + HWIO_PLL_ALPHA_VAL_OFFS_STROMER, (uint32)pConfig->nAlpha);
    outp32(nModeAddr + HWIO_PLL_ALPHA_VAL_U_OFFS_STROMER, (uint32)(pConfig->nAlpha >> 32));

    /*
     * Get the CONFIG value and clear out fields we will configure.
     */
     nUserVal = inp32(nModeAddr + HWIO_PLL_USER_CTL_OFFS_STROMER);
     nUserVal &= ~(HWIO_FMSK(GCC_STROMER_PLL_USER_CTL, ALPHA_EN)      |
                   HWIO_FMSK(GCC_STROMER_PLL_USER_CTL, VCO_SEL)       |
                   HWIO_FMSK(GCC_STROMER_PLL_USER_CTL, PRE_DIV_RATIO) |
                   HWIO_FMSK(GCC_STROMER_PLL_USER_CTL, POST_DIV_RATIO));

     /*
      * Program the VCO.
      */
     nUserVal |= (pConfig->nVCO << HWIO_SHFT(GCC_STROMER_PLL_USER_CTL, VCO_SEL));

     /*
      * Program the pre-div value (div-1 to div-8).
      */
     if (pConfig->nPreDiv <= 8 && pConfig->nPreDiv > 0)
     {
       nUserVal |= ((pConfig->nPreDiv - 1) << HWIO_SHFT(GCC_STROMER_PLL_USER_CTL, PRE_DIV_RATIO));
     }

     /*
      * Program the post-div value (div-3 not supported)
      */
     if (pConfig->nPostDiv == 2)
     {
       nUserVal |= (1 << HWIO_SHFT(GCC_STROMER_PLL_USER_CTL, POST_DIV_RATIO));
     }
     else if (pConfig->nPostDiv == 4)
     {
      nUserVal |= (3 << HWIO_SHFT(GCC_STROMER_PLL_USER_CTL, POST_DIV_RATIO));
     }

     /*
      *  Check fractional output or integer output.
      */
     if (pConfig->nAlpha != 0)
     {
       nUserVal |= HWIO_FMSK(GCC_STROMER_PLL_USER_CTL, ALPHA_EN);
     }

     /*
      * Enable MAIN_OUT_ENA bit. 
      */
     nUserVal |= HWIO_FMSK(GCC_STROMER_PLL_USER_CTL, PLLOUT_LV_MAIN);
 
     /*
      * Finally program the CONFIG register.
      */
     outp32(nModeAddr + HWIO_PLL_USER_CTL_OFFS_STROMER, nUserVal);

  } 
  else
  {
     /*
      * Program L, M, and N.
      */
     outp32(nModeAddr + HWIO_PLL_L_VAL_OFFS_SR, pConfig->nL);
     outp32(nModeAddr + HWIO_PLL_M_VAL_OFFS_SR, pConfig->nM);
     outp32(nModeAddr + HWIO_PLL_N_VAL_OFFS_SR, pConfig->nN);
   
     /*
      * Get the CONFIG value and clear out fields we will configure.
      */
     nUserVal = inp32(nModeAddr + HWIO_PLL_USER_CTL_OFFS_SR);
     nUserVal &= ~(HWIO_FMSK(GCC_SR_PLL_USER_CTL, MN_EN)         |
                  HWIO_FMSK(GCC_SR_PLL_USER_CTL, VCO_SEL)       |
                  HWIO_FMSK(GCC_SR_PLL_USER_CTL, PRE_DIV_RATIO) |
                  HWIO_FMSK(GCC_SR_PLL_USER_CTL, POST_DIV_RATIO));

     /*
      * Program the VCO.
      */
     nUserVal |= (pConfig->nVCO << HWIO_SHFT(GCC_SR_PLL_USER_CTL, VCO_SEL));

     /*
      * Program the pre-div value (div-1 to div-8).
      */
     if (pConfig->nPreDiv <= 8 && pConfig->nPreDiv > 0)
     {
       nUserVal |= ((pConfig->nPreDiv - 1) << HWIO_SHFT(GCC_SR_PLL_USER_CTL, PRE_DIV_RATIO));
     }

     /*
      * Program the post-div value (div-3 not supported)
      */
     if (pConfig->nPostDiv == 2)
     {
       nUserVal |= (1 << HWIO_SHFT(GCC_SR_PLL_USER_CTL, POST_DIV_RATIO));
     }
     else if (pConfig->nPostDiv == 4)
     {
       nUserVal |= (3 << HWIO_SHFT(GCC_SR_PLL_USER_CTL, POST_DIV_RATIO));
     }
   
     /*
      *  Check fractional output or integer output.
      */
     if (pConfig->nM != 0 && pConfig->nM < pConfig->nN)
     {
       nUserVal |= HWIO_FMSK(GCC_SR_PLL_USER_CTL, MN_EN);
     }

     /*
      * Enable MAIN_OUT_ENA bit. 
      */
     nUserVal |= HWIO_FMSK(GCC_SR_PLL_USER_CTL, PLLOUT_LV_MAIN);

     /*
      * Finally program the CONFIG register.
      */
     outp32(nModeAddr + HWIO_PLL_USER_CTL_OFFS_SR, nUserVal);
 } 
  /*
   * Program the FSM portion of the mode register.
   */
  nModeVal &= ~HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_BIAS_COUNT);
  nModeVal &= ~HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_LOCK_COUNT);

  if(pConfig->ePLL == CLK_SOURCE_PLL_STROMER)
  {
  nModeVal |= CLOCK_PLL_BIAS_COUNT_VAL;
  }
  else
  {
    nModeVal |=  CLOCK_SRPLL_BIAS_COUNT_VAL;
  } 
  nModeVal |= CLOCK_PLL_LOCK_COUNT_VAL;

  if (pConfig->nVoteAddr != 0)
  {
    nModeVal |= HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_VOTE_FSM_ENA);
  }

  outp32(nModeAddr, nModeVal);
  
  if(pConfig->ePLL == CLK_SOURCE_PLL_STROMER)
  {
     /* The CONFIG_CTL resets with an incorrect default value.  Fix it. */
     outp32(nModeAddr + HWIO_PLL_CONFIG_CTL_OFFS_STROMER, pConfig->nConfigCtl);
  }
  else
  {
     /* The CONFIG_CTL resets with an incorrect default value.  Fix it. */
     outp32(nModeAddr + HWIO_PLL_CONFIG_CTL_OFFS_SR, pConfig->nConfigCtl);
  }
  return TRUE;
}

/* ============================================================================
**  Function : ClockSDI_EnablePLL
** ============================================================================
*/
/*!
    Enable a PLL.  If it is voteable, it will be set for FSM mode
    and voted for using the vote register. Doesn't configure the PLL.

    @param pConfig -  [IN] PLL configuration structure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
static boolean ClockSDI_EnablePLL(const ClockSDIConfigPLLType *pConfig)
{
  uintnt nModeAddr, nStatusAddr;
  uint32 nStatusMask,nLockDetMask,nModeVal, nTimeout = 0;

  nModeAddr = pConfig->nPLLModeAddr;
  if(pConfig->ePLL != CLK_SOURCE_PLL_STROMER)
  {
     nStatusAddr = nModeAddr + HWIO_PLL_STATUS_OFFS_SR;
     nStatusMask = HWIO_FMSK(GCC_SR_PLL_STATUS, PLL_ACTIVE_FLAG);
     nLockDetMask = HWIO_FMSK(GCC_SR_PLL_STATUS, PLL_LOCK_DET);
  }
  else
  {
     nStatusAddr =  nModeAddr;
     nStatusMask =  HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_ACTIVE_FLAG);
     nLockDetMask = HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_LOCK_DET);
  } 
  nModeVal = inp32(nModeAddr);

  if ( ((nModeVal & CLOCK_PLL_MODE_ACTIVE) == CLOCK_PLL_MODE_ACTIVE) ||
       (nModeVal & HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_VOTE_FSM_ENA)) )
  {
    /*
     * The PLL is already running, so vote for the resource but don't configure it.
     */
    if (pConfig->nVoteAddr != 0)
    {
      outp32(pConfig->nVoteAddr, inp32(pConfig->nVoteAddr) | pConfig->nVoteMask);

      /*
       * Wait for the PLL to go active.
       */
      nTimeout = 0;
      while ((inp32(nStatusAddr) & nStatusMask) == 0)
      {
        if (nTimeout++ >= CLOCK_PLL_LOCK_TIMEOUT_US)
        {
          return FALSE;
        }
        busywait(1);
      }
    }

     return TRUE;
  }

  /*
   * Configure the PLL.
   */
  if(! ClockSDI_ConfigurePLL(pConfig)) return FALSE;

  /*
   * Enable the PLL.
   */
  if (pConfig->nVoteAddr != 0)
  {
    outp32(pConfig->nVoteAddr, inp32(pConfig->nVoteAddr) | pConfig->nVoteMask);

    /*
     * Wait for the PLL to go active.
     */
    while ((inp32(nStatusAddr) & nStatusMask) == 0)
       {
         if (nTimeout++ >= CLOCK_PLL_LOCK_TIMEOUT_US)
         {
           return FALSE;
         }
         
         busywait(1);
       }
    }
    else
    {
    nModeVal |= HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_BYPASSNL);
    outp32(nModeAddr, nModeVal);
    busywait(CLOCK_PLL_INIT_TIME_US);

    nModeVal |= HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_RESET_N);
    outp32(nModeAddr, nModeVal);
    busywait(CLOCK_PLL_WARMUP_TIME_US);

    while ((inp32(nStatusAddr) & nLockDetMask) == 0)
    {
      if (nTimeout++ >= CLOCK_PLL_LOCK_TIMEOUT_US)
      {
        return FALSE;
      }
      
      busywait(1);
    }

    nModeVal |= HWIO_FMSK(GCC_STROMER_PLL_MODE, PLL_OUTCTRL);
    outp32(nModeAddr, nModeVal);
  }

  return TRUE;
}

/* =========================================================================
**  Function : ClockSDI_ConfigMux
** =========================================================================*/
/*!
    Configure a clock mux. 

    @param pConfig -  [IN] Clock mux config structure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
static boolean ClockSDI_ConfigMux (const ClockSDIConfigMuxType *pConfig)
{
  uintnt nCmdCGRAddr, nCfgCGRAddr;
  uint32 nCmdCGRVal, nCfgCGRVal;
  uintnt nMAddr, nNAddr, nDAddr;
  uint32 nSource = pConfig->nSource;
  uint32 nTimeout = 0;
  
  nCmdCGRAddr = pConfig->nCMDRCGRAddr;
  nCmdCGRVal  = inp32(nCmdCGRAddr);
  nCfgCGRAddr = pConfig->nCMDRCGRAddr + CLOCK_CFG_REG_OFFSET; 
  nCfgCGRVal  = inp32(nCfgCGRAddr);

  /*
   * Clear the fields
   */
  nCfgCGRVal &= ~(CLOCK_CFG_CGR_SRC_SEL_FMSK |
                  CLOCK_CFG_CGR_SRC_DIV_FMSK |
                  CLOCK_CFG_CGR_MODE_FMSK);

  /*
   * Program the source and divider values.
   */
  nCfgCGRVal |= (nSource << CLOCK_CFG_CGR_SRC_SEL_SHFT)
                  & CLOCK_CFG_CGR_SRC_SEL_FMSK;
  nCfgCGRVal |= ((HALF_DIVIDER(pConfig) << CLOCK_CFG_CGR_SRC_DIV_SHFT)
                  & CLOCK_CFG_CGR_SRC_DIV_FMSK);

  /*
   * Set MND counter mode depending on if it is in use
   */
  if (pConfig->nM != 0 && (pConfig->nM < pConfig->nN))
  {
    nMAddr = pConfig->nCMDRCGRAddr + CLOCK_M_REG_OFFSET;
    nNAddr = pConfig->nCMDRCGRAddr + CLOCK_N_REG_OFFSET;
    nDAddr = pConfig->nCMDRCGRAddr + CLOCK_D_REG_OFFSET;

    outp32(nMAddr, pConfig->nM);
    outp32(nNAddr, NOT_N_MINUS_M(pConfig));
    outp32(nDAddr, NOT_2D(pConfig));

    nCfgCGRVal |= ((CLOCK_CFG_CGR_MODE_DUAL_EDGE_VAL << CLOCK_CFG_CGR_MODE_SHFT)
                    & CLOCK_CFG_CGR_MODE_FMSK);
  }

  /*
   * Write the final CFG register value
   */
  outp32(nCfgCGRAddr, nCfgCGRVal);

  /*
   * Trigger the update
   */
  nCmdCGRVal |= CLOCK_CMD_CFG_UPDATE_FMSK;
  outp32(nCmdCGRAddr, nCmdCGRVal);

  /*
   * Wait until update finishes
   */
  while(inp32(nCmdCGRAddr) & CLOCK_CMD_CFG_UPDATE_FMSK)
  {
    if (nTimeout++ >= CLOCK_UPDATE_TIMEOUT_US)
    {
      return FALSE;
    }
    
    busywait(1);
  }

  return TRUE;

}

/* ========================================================================
**  Function : Clock_ConfigureDDR
** ======================================================================*/
/*
    Description: Configure all clocks needed for DDR configuration.  This
    extension API is used for bootup and emergency download mode.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_ConfigureDDR( void )
{
  uint32 mask;
  //uint8 ddr_type;
  //const ClockSDIConfigMuxType bimc_ddr_clkCfg   = {HWIO_ADDR(GCC_BIMC_DDR_CMD_RCGR),      3,  2, 0, 0, 0};  //gpll5_out_early ( Config to PCDDR3 Min i.e, 422.4 )
 // const ClockSDIConfigMuxType bimc_gpu_clkCfg   = {HWIO_ADDR(GCC_BIMC_GPU_CMD_RCGR),      3,  8, 0, 0, 0};  //gpll5_out_early

#if 0 //Enable the code if need to run the BIMC and its interface clocks at NOMINAL speed

  const ClockSDIConfigMuxType bimc_ddr_clkCfg   = {HWIO_ADDR(GCC_BIMC_DDR_CMD_RCGR),      3,  2, 0, 0, 0};  //gpll5_out_early NOM = 595.2 (config PLL to 1190.4)
  const ClockSDIConfigMuxType bimc_gpu_clkCfg   = {HWIO_ADDR(GCC_BIMC_GPU_CMD_RCGR),      3,  8, 0, 0, 0};  //gpll5_out_early NOM = 297.6
  const ClockSDIConfigMuxType bimc_cdsp_clkCfg  = {HWIO_ADDR(GCC_CDSP_BIMC_CMD_RCGR),     1,  6, 0, 0, 0};  //gpll0_out_main NOM = 266.67
  const ClockSDIConfigMuxType bimc_lpass_clkCfg = {HWIO_ADDR(GCC_LPASS_BIMC_CMD_RCGR),    2,  7, 0, 0, 0};  //gpll6_out_main NOM = 308.57
  const ClockSDIConfigMuxType bimc_mdss_clkCfg  = {HWIO_ADDR(GCC_MDSS_BIMC_CMD_RCGR),     1,  6, 0, 0, 0};  //gpll0_out_main NOM = 266.7
  const ClockSDIConfigMuxType apss_axi_clkCfg   = {HWIO_ADDR(GCC_APSS_AXI_CMD_RCGR),      1,  4, 0, 0, 0};  //gpll0_out_main NOM = 266.7
  const ClockSDIConfigMuxType apss_tcu_clkCfg   = {HWIO_ADDR(GCC_APSS_TCU_CMD_RCGR),      1,  5, 0, 0, 0};  //gpll0_out_main NOM = 320

  if ( !ClockSDI_EnablePLL(&PLL0_Cfg) ) return FALSE;
  if ( !ClockSDI_EnablePLL(&PLL5_Cfg) ) return FALSE;
  /* Enable the Early output it is used by bimc_ddr and bimc_gpu RCGs */
  HWIO_OUTF(GCC_GPLL5_USER_CTL, PLLOUT_LV_EARLY, 1);
  if ( !ClockSDI_EnablePLL(&PLL6_Cfg) ) return FALSE;

  /* Enable Imem and Msg RAM clock */
  ClockSDI_ToggleClock(HWIO_GCC_IMEM_CFG_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, IMEM_AXI_CLK_ENA, 1);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA, 1);
  /*Enable all the necessary clocks for BIMC, APSS, DDR */
  mask = 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, SYS_NOC_APSS_AHB_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BIMC_APSS_AXI_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AHB_CLK_ENA) |
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AXI_CLK_ENA);
// GCC_BIMC_APSS_TCU_CBCR doesn't have the enable bit n not votable default on
  HWIO_OUTM(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, mask, mask);
  HWIO_OUTF(GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE, APSS_TCU_ASYNC_CLK_ENA, 1);
  ClockSDI_ToggleClock(HWIO_GCC_SYS_NOC_TPRB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_PCNOC_DDR_CFG_CBCR_ADDR, CLK_TOGGLE_ENABLE);  
  ClockSDI_ToggleClock(HWIO_GCC_DDR_DIM_CFG_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_DDR_DIM_SLEEP_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_BIMC_XO_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  ClockSDI_ToggleClock(HWIO_GCC_BIMC_CFG_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  ClockSDI_ToggleClock(HWIO_GCC_BIMC_SLEEP_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  ClockSDI_ToggleClock(HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  ClockSDI_ToggleClock(HWIO_GCC_BIMC_CBCR_ADDR, CLK_TOGGLE_ENABLE); 
  
  HWIO_OUTF(GCC_BIMC_MISC, BIMC_FRQSW_FSM_DIS, 1);
  HWIO_OUTF(GCC_BIMC_MISC, BIMC_DDR_LEGACY_2X_MODE_EN, 1);

  if( !ClockSDI_ConfigMux(&bimc_ddr_clkCfg)) return FALSE;
  if( !ClockSDI_ConfigMux(&bimc_gpu_clkCfg)) return FALSE;
  if( !ClockSDI_ConfigMux(&bimc_cdsp_clkCfg)) return FALSE;
  if( !ClockSDI_ConfigMux(&bimc_lpass_clkCfg)) return FALSE;
  if( !ClockSDI_ConfigMux(&bimc_mdss_clkCfg)) return FALSE;
  if( !ClockSDI_ConfigMux(&apss_axi_clkCfg)) return FALSE;
  if( !ClockSDI_ConfigMux(&apss_tcu_clkCfg)) return FALSE;
  
  /*
   * Trigger the FSM update manually and wait for the frequency to switch.
   */
  if(! ClockSDI_TriggerUpdate(HWIO_ADDR(GCC_BIMC_MISC),
                           HWIO_FMSK(GCC_BIMC_MISC, BIMC_FSM_DIS_DDR_UPDATE))) return FALSE;
  
 /* Needed by Debug UI */
  HWIO_OUTF(GCC_QDSS_DAP_AHB_CBCR, CLK_ENABLE, 0x1);
  HWIO_OUTF(GCC_QDSS_CFG_AHB_CBCR,CLK_ENABLE, 0x1);
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, QDSS_DAP_CLK_ENA, 0x1);

  /*Enable the code once DDR type APIs from boot team is available */
#endif



  /* Enable Imem and Msg RAM clock */
  ClockSDI_ToggleClock(HWIO_GCC_IMEM_CFG_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, IMEM_AXI_CLK_ENA, 1);
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA, 1);
  /*Enable all the necessary clocks for BIMC, APSS, DDR */
  mask = 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, PCNOC_APSS_AHB_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, BIMC_APSS_AXI_CLK_ENA) | 
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AHB_CLK_ENA) |
    HWIO_FMSK(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, APSS_AXI_CLK_ENA);
// GCC_BIMC_APSS_TCU_CBCR doesn't have the enable bit n not votable default on
  HWIO_OUTM(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, mask, mask);
 //HWIO_OUTF(GCC_APCS_SMMU_CLOCK_BRANCH_ENA_VOTE, APSS_TCU_ASYNC_CLK_ENA, 1);
  //ClockSDI_ToggleClock(HWIO_GCC_SYS_NOC_TPRB_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  /* Needed by Debug UI */
  HWIO_OUTF(GCC_QDSS_DAP_AHB_CBCR, CLK_ENABLE, 0x1);
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE,QDSS_CFG_AHB_CLK_ENA, 0x1);
  HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, QDSS_DAP_CLK_ENA, 0x1);
  /* Needed by DDR*/
  HWIO_GCC_PCNOC_DDR_CFG_CBCR_OUT(HWIO_GCC_PCNOC_DDR_CFG_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK );
/* Enable all possible BIMC and DDR clocks, (K) means KPSS_Boot_Clk_CTL On.
     bimc_ddr_xo_clk_src, root auto enables. */
  ClockSDI_ToggleClock(HWIO_GCC_PCNOC_DDR_CFG_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  /* gcc_ddr_dim_cfg_clk */
  ClockSDI_ToggleClock(HWIO_GCC_DDR_DIM_CFG_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  /* gcc_ddr_dim_sleep_clk */
  ClockSDI_ToggleClock(HWIO_GCC_DDR_DIM_SLEEP_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  /* gcc_bimc_xo_clk */
  ClockSDI_ToggleClock(HWIO_GCC_BIMC_XO_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  /* gcc_bimc_cfg_ahb_clk */
  ClockSDI_ToggleClock(HWIO_GCC_BIMC_CFG_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  /* gcc_bimc_sleep_clk */
  ClockSDI_ToggleClock(HWIO_GCC_BIMC_SLEEP_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  /* (K)gcc_bimc_sysnoc_axi_clk */
  //ClockSDI_ToggleClock(HWIO_GCC_BIMC_SYSNOC_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  /* (K)gcc_bimc_clk */
  ClockSDI_ToggleClock(HWIO_GCC_BIMC_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  
 
    HWIO_OUTF(GCC_BIMC_MISC, BIMC_FRQSW_FSM_DIS, 1);
    HWIO_OUTF(GCC_BIMC_MISC, BIMC_DDR_LEGACY_2X_MODE_EN, 1);

  return TRUE;
}
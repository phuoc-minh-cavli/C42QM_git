/*
 * HALclkInitASM.S
 * This file contains the implementation for setting up the initial Q6 configuration.
 *
 * $Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/HALclkInitASM.S#4 $
 *
 */

#include "HALclkHWIOASM.h"
#include "HALclkPLLSettings.h"


/////////////////////////////////////////////////////////////////////////////////
//
//       Additional definitions and bit masks not defined in HWIO inc files.
//
/////////////////////////////////////////////////////////////////////////////////


#define MSS_Q6SS_CORE_CLK_PLL_SEL             0x1
#define MSS_Q6SS_CORE_CLK_DIV                 0x1

#define HALCLK_WAIT_5_US                      0x60
#define HALCLK_WAIT_10_US                     0xC0
#define HALCLK_WAIT_50_US                     0x3C0

#define TCSR_SOC_VERSION_MAJOR_V1_VAL         0x100

#define MSS_QDSP6SS_PLL_VCO_MODE_VAL          0x2

#define MSS_MPLL1_POSTDIV_VAL                 0x1

#define MSS_BUS_CLK_PLL_SEL                   0x1
#define MSS_BUS_CLK_DIV                       0x17


/////////////////////////////////////////////////////////////////////////////////
//
//                        Data declarations start here.
//
/////////////////////////////////////////////////////////////////////////////////

  // PLL L and CALIBRATION_L values for Q6 PLL
  .equ Q6_PLL_CALIB_L_VAL,                    0x1C  // 28 for 537.6 MHz
  .equ Q6SS_PLL_L_VAL,                        0x19  // 25 for 480.0 MHz

  // PLL L value for MPLL1
  .equ MPLL1_PLL_L_VAL,                       0x30  // 48 for 921.6 MHz


/////////////////////////////////////////////////////////////////////////////////
//
//                 Macros Start Here.
//
/////////////////////////////////////////////////////////////////////////////////


  //
  // MACRO:  WRITE_REGISTER
  //
  // DESCRIPTION:
  //   This macro writes an unmasked value to a register.  Any contents will
  //   be overwritten in the proccess.  r6 and r7 may be clobbered.
  //

  .macro write_register  val
    {
      r6 = \val
      r7 = r0
    }
    memw(r0) = r6
    barrier
  .endm


  //
  // MACRO:  CLEAR_BITS
  //
  // DESCRIPTION:
  //   This macro clears one ore more bits in a register.  r4, r5, r6, and r7
  //   may be clobbered when running this macro.
  //

  .macro clear_bits val_mask
    {
      r7 = r0
      r5.h = #hi(\val_mask)
    }
    {
      r5.l = #lo(\val_mask)
    }
    {
      r4 = memw(r0)
      r5 = not(r5)
    }
    r4 = and(r4, r5)    // clear the mask bits
    {
      memw(r0) = r4     //write the reg value
    }
    barrier
  .endm


  //
  // MACRO:  SET_BITS
  //
  // DESCRIPTION:
  //   This macro sets one or more bits in a register.  r4, r5 and r7 will be
  //   clobbered when using this macro.
  //

  .macro set_bits val_mask
    {
      r7 = r0
      r5.h = #hi(\val_mask)
    }
    {
    r5.l = #lo(\val_mask)
    dcinva(r7)
    }
    {
      r4 = memw(r0)
      r6 = not(r5)
    }
    r4 = and(r4, r6)    // clear the fields
    r4 = or(r4,r5)      // set the value

    {
      memw(r0) = r4     //write the reg value
    }
    barrier
  .endm

  .macro __busywait_init
    r0 = ##HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(0)
    memw(r0) = ##HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RMSK
    r0 = ##HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(1)
    memw(r0) = ##HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RMSK
    r0 = ##HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(2)
    memw(r0) = ##HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RMSK
    barrier
  .endm

  .macro __busywait_deinit
    r0 = ##HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(0)
    memw(r0) = ##0x0
    r0 = ##HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(1)
    memw(r0) = ##0x0
    r0 = ##HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(2)
    memw(r0) = ##0x0
    barrier
  .endm


/////////////////////////////////////////////////////////////////////////////////
//
//         Executable code starts here.
//
/////////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------------
// FUNCTION: __HEXAGON_BSP_INIT
//
// DESCRIPTION:
//   This is the main API for this file. This function will check the status of the
//   main MSS Q6 PLL and configure it if it is not running. Otherwise, it will just
//   add a HW vote on behalf of the MSS Q6 processor.
//
//   The QDSP6 clock will then be initialized if not already set up. The
//   purpose of this is to ensure the QDSP6 clock is running at maximum
//   frequency under NOM voltage.
//
// ASSUMPTIONS:
//   There is no stack available when this is called, so only device registers can be
//   accessed.
//----------------------------------------------------------------------------------
#if defined(SIM)
  // No initialization.
#else
  .section .start,"ax",@progbits
  .falign
  .global __hexagon_bsp_init
  .type  __hexagon_bsp_init, @function

__hexagon_bsp_init:
  __busywait_init

  jump start_prog

__busywait:
  {
    r8 = r1
    r9 = r2
    r10 = r3
  }
  r2 = r0             // delay_count
  r0 = ##HWIO_MSS_QDSP6SS_QTMR_V1_CNTPCT_LO_1_ADDR
  r1 = memw(r0)       // start count

__buswait_loop:
  r3 = memw(r0)
  r3 = sub(r3, r1)
  {
    p0 = cmp.gtu(r3,r2)
    if (!p0.new) jump:t __buswait_loop
  }
  {
    r1 = r8
    r2 = r9
    r3 = r10
  }
  jumpr r13

start_prog:

  //*******************************************************************
  // Program the PLL_CONFIG_CTL values based on recommendations.
  //*******************************************************************

  r0 = ##HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_ADDR
  memw(r0) = ##HAL_CLK_MSS_CC_QDSP6SS_PLL_CONFIG_CTL

  r0 = ##HWIO_MSS_MPLL1_CONFIG_CTL_ADDR
  memw(r0) = ##HAL_CLK_MSS_CC_MPLL1_CONFIG_CTL

  //*******************************************************************
  // Program the USER_CTL_U values based on recommendations.
  //*******************************************************************

  r0 = ##HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR
  memw(r0) = ##HAL_CLK_MSS_CC_QDSP6SS_PLL_USER_CTL_U

  //*******************************************************************
  // Program the TEST_CTL and TEST_CTL_U values based on recommendations.
  //*******************************************************************

  r0 = ##HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ADDR
  memw(r0) = ##HAL_CLK_MSS_CC_QDSP6SS_PLL_TEST_CTL

  r0 = ##HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_ADDR
  memw(r0) = ##HAL_CLK_MSS_CC_QDSP6SS_PLL_TEST_CTL_U

  r0 = ##HWIO_MSS_MPLL1_TEST_CTL_ADDR
  memw(r0) = ##HAL_CLK_MSS_CC_MPLL1_TEST_CTL

  barrier

  //*******************************************************************
  //
  // Begin main PLL programming sequence here.
  //
  //*******************************************************************

start_pll:

  //*******************************************************************
  // Check the HW version for the PLL_CONFIG_CTL programming.
  //*******************************************************************

  r0 = ##HWIO_TCSR_SOC_HW_VERSION_ADDR
  r1 = memw(r0)
  r2 = and(r1, ##TCSR_SOC_VERSION_MAJOR_V1_VAL)
  {
    p0 = cmp.eq(r2,##TCSR_SOC_VERSION_MAJOR_V1_VAL)
    if (p0.new) jump:t v1_pll_configs
  }

  // Use the last supported version if a valid
  // version wasn't detected
  jump v1_pll_configs

v1_pll_configs:

  r14 = ##Q6SS_PLL_L_VAL
  r15 = ##Q6_PLL_CALIB_L_VAL
  r16 = ##MSS_Q6SS_CORE_CLK_DIV
  jump program_q6ss_pll

program_q6ss_pll:

  //*******************************************************************
  // Set up the PLL L, CALIBRATION_L & ALPHA value
  //*******************************************************************

  {
    r0 = ##HWIO_MSS_QDSP6SS_PLL_L_VAL_ADDR
    r1 = r14
  }
  memw(r0) = r1
  barrier

  r0 = ##HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR
  r1 = memw(r0)
  clear_bits HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_CALIBRATION_L_BMSK
  r1 = memw(r0)
  r2 = asl(r15,#HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_CALIBRATION_L_SHFT)
  r1 = or(r1,r2)
  memw(r0) = r1
  barrier

  r0 = ##HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_ADDR
  clear_bits HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_RMSK
  barrier

  r0 = ##HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_ADDR
  clear_bits HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_RMSK
  barrier

  //*******************************************************************
  // Program VCO mode
  //*******************************************************************

  r0 = ##HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR
  r1 = memw(r0)
  clear_bits HWIO_MSS_QDSP6SS_PLL_USER_CTL_VCO_SEL_BMSK
  r2 = ##(MSS_QDSP6SS_PLL_VCO_MODE_VAL << HWIO_MSS_QDSP6SS_PLL_USER_CTL_VCO_SEL_SHFT)
  r1 = or(r1,r2)
  memw(r0)= r1
  barrier

  //*******************************************************************
  // Misc. programming
  //*******************************************************************

  r0 = ##HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR
  clear_bits HWIO_MSS_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_BMSK
  clear_bits HWIO_MSS_QDSP6SS_PLL_USER_CTL_PRE_DIV_RATIO_BMSK
  clear_bits HWIO_MSS_QDSP6SS_PLL_USER_CTL_ALPHA_EN_BMSK

  r0 = ##HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR
  set_bits HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_LOCK_DET_BMSK

  //*******************************************************************
  // Enable dynamic programming
  //*******************************************************************

  r0 = ##HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR
  clear_bits HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK

  r0 = ##HWIO_MSS_QDSP6SS_PLL_MODE_ADDR
  set_bits HWIO_MSS_QDSP6SS_PLL_MODE_PLL_BYPASSNL_BMSK
  r0 = ##HALCLK_WAIT_5_US
  r13 = add(PC, #0x8)
  jump __busywait

  r0 = ##HWIO_MSS_QDSP6SS_PLL_MODE_ADDR
  set_bits HWIO_MSS_QDSP6SS_PLL_MODE_PLL_RESET_N_BMSK
  r0 = ##HALCLK_WAIT_50_US
  r13 = add(PC, #0x8)
  jump __busywait

  //*******************************************************************
  // Poll on the lock detect value until it transitions to '1'
  //*******************************************************************

  r4 = ##HWIO_MSS_QDSP6SS_PLL_MODE_ADDR
  r2 = ##HWIO_MSS_QDSP6SS_PLL_MODE_PLL_LOCK_DET_BMSK

q6_lock_det_loop:
  r0 = ##HALCLK_WAIT_5_US
  r13 = add(PC, #0x8)
  jump __busywait

  r1 = memw(r4)
  r3 = and(r1, r2)
  {
    p0 = cmp.eq(r2,r3)
    if (!p0.new) jump:t q6_lock_det_loop
  }

  //*******************************************************************
  // Enable PLL Outputs
  //*******************************************************************

  r0 = ##HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR
  set_bits HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK

  r0 = ##HWIO_MSS_QDSP6SS_PLL_MODE_ADDR
  set_bits HWIO_MSS_QDSP6SS_PLL_MODE_PLL_OUTCTRL_BMSK

  // ...End QDSP6SS PLL Setup...

  //*******************************************************************
  // Switch the Q6 to run on the QDSP6SS_PLL.
  //*******************************************************************

check_qdsp6:
  r0 = ##HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_ADDR
  r1 = memw(r0)

  // Clear out the source selection and divider values.
  {
    r2 = ##HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_BMSK
    r3 = ##HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_BMSK
  }
  r2 = or(r2, r3)
  r2 = not(r2)
  r1 = and(r1, r2)

  // Add the new values.
  {
    r2 = ##MSS_Q6SS_CORE_CLK_PLL_SEL<<HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_SHFT
    r3 = asl(r16,#HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_SHFT)
  }
  r2 = or(r2, r3)
  r1 = or(r1, r2)
  memw(r0) = r1
  barrier

  // Ensure Q6 is on.
  r0 = ##HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ADDR
  r1 = memw(r0)

  // Trigger the update
  r2 = ##HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_UPDATE_BMSK
  r1 = or(r1, r2)
  memw(r0) = r1
  barrier

q6_cmd_update:
  r1 = memw(r0)
  r3 = and(r1, r2)
  {
    p0 = cmp.eq(r3, r2)
    if (p0.new) jump:t q6_cmd_update
  }

  //*******************************************************************
  // Configure MPLL1 and program config bus to 76.8 MHz
  //*******************************************************************

program_mpll1:
  r14 = ##MPLL1_PLL_L_VAL

  //*******************************************************************
  // Set up the PLL L and clear out M/N
  //*******************************************************************

  {
    r0 = ##HWIO_MSS_MPLL1_L_VAL_ADDR
    r1 = r14
  }
  memw(r0) = r1
  barrier

  r0 = ##HWIO_MSS_MPLL1_M_VAL_ADDR
  clear_bits HWIO_MSS_MPLL1_M_VAL_RMSK
  barrier

  r0 = ##HWIO_MSS_MPLL1_N_VAL_ADDR
  clear_bits HWIO_MSS_MPLL1_N_VAL_RMSK
  barrier

  //*******************************************************************
  // Program POST_DIV value
  //*******************************************************************

  r0 = ##HWIO_MSS_MPLL1_USER_CTL_ADDR
  r1 = memw(r0)
  clear_bits HWIO_MSS_MPLL1_USER_CTL_POSTDIV_CTL_BMSK
  r2 = ##(MSS_MPLL1_POSTDIV_VAL << HWIO_MSS_MPLL1_USER_CTL_POSTDIV_CTL_SHFT)
  r1 = or(r1,r2)
  memw(r0)= r1
  barrier

  //*******************************************************************
  // Misc. programming
  //*******************************************************************

  r0 = ##HWIO_MSS_MPLL1_USER_CTL_ADDR
  clear_bits HWIO_MSS_MPLL1_USER_CTL_PREDIV2_EN_BMSK
  clear_bits HWIO_MSS_MPLL1_USER_CTL_MN_EN_BMSK

  //*******************************************************************
  // Enable the PLL
  //*******************************************************************

  r0 = ##HWIO_MSS_MPLL1_MODE_ADDR
  set_bits HWIO_MSS_MPLL1_MODE_PLL_BYPASSNL_BMSK
  r0 = ##HALCLK_WAIT_5_US
  r13 = add(PC, #0x8)
  jump __busywait

  r0 = ##HWIO_MSS_MPLL1_MODE_ADDR
  set_bits HWIO_MSS_MPLL1_MODE_PLL_RESET_N_BMSK
  r0 = ##HALCLK_WAIT_50_US
  r13 = add(PC, #0x8)
  jump __busywait

  //*******************************************************************
  // Poll on the lock detect value until it transitions to '1'
  //*******************************************************************

  r4 = ##HWIO_MSS_MPLL1_STATUS_ADDR
  r2 = ##HWIO_MSS_MPLL1_STATUS_PLL_LOCK_DET_BMSK

mpll1_lock_det_loop:
  r0 = ##HALCLK_WAIT_5_US
  r13 = add(PC, #0x8)
  jump __busywait

  r1 = memw(r4)
  r3 = and(r1, r2)
  {
    p0 = cmp.eq(r2,r3)
    if (!p0.new) jump:t mpll1_lock_det_loop
  }

  //*******************************************************************
  // Enable PLL outputs
  //*******************************************************************

  r0 = ##HWIO_MSS_MPLL1_USER_CTL_ADDR
  set_bits HWIO_MSS_MPLL1_USER_CTL_LVMAIN_EN_BMSK
  set_bits HWIO_MSS_MPLL1_USER_CTL_LVEARLY_EN_BMSK
  set_bits HWIO_MSS_MPLL1_USER_CTL_LVAUX_EN_BMSK

  r0 = ##HWIO_MSS_MPLL1_MODE_ADDR
  set_bits HWIO_MSS_MPLL1_MODE_PLL_OUTCTRL_BMSK

  // ...End MPLL1 Setup...

  //*******************************************************************
  // Switch the config bus to run at 76.8 MHz
  //*******************************************************************

check_bus:
  r0 = ##HWIO_MSS_BUS_CFG_RCGR_ADDR
  r1 = memw(r0)

  // Clear out the source selection and divider values.
  {
    r2 = ##HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_BMSK
    r3 = ##HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_BMSK
  }
  r2 = or(r2, r3)
  r2 = not(r2)
  r1 = and(r1, r2)

  // Add the new values.
  {
    r2 = ##MSS_BUS_CLK_PLL_SEL<<HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_SHFT
    r3 = ##MSS_BUS_CLK_DIV<<HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_SHFT
  }
  r2 = or(r2, r3)
  r1 = or(r1, r2)
  memw(r0) = r1
  barrier

  r0 = ##HWIO_MSS_BUS_CMD_RCGR_ADDR
  r1 = memw(r0)

  // Trigger the update
  r2 = ##HWIO_MSS_BUS_CMD_RCGR_UPDATE_BMSK
  r1 = or(r1, r2)
  memw(r0) = r1
  barrier

bus_cmd_update:
  r1 = memw(r0)
  r3 = and(r1, r2)
  {
    p0 = cmp.eq(r3, r2)
    if (p0.new) jump:t bus_cmd_update
  }

  __busywait_deinit

end_init:
  jumpr r31

.size __hexagon_bsp_init, .-__hexagon_bsp_init
#endif // SIM

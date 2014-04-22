/*! \file pm_config_target.c
 *
 *  \brief This file contains customizable target specific driver settings & PMIC registers.
 *         This file info is taken from Target Component Resource Specific Settings from PDM
 *         that is maintained for each of the targets separately.
 *
 *  &copy; Copyright 2013-2019 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.3/core/systemdrivers/pmic/config/mdm9x05/pm_config_target.c#14 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/18   ks      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pmapp_npa.h"
#include "pm_npa.h"
#include "pm_target_information.h"
#include "pm_rpm_ldo_trans_apply.h"
#include "pm_rpm_smps_trans_apply.h"
#include "pm_rpm_clk_buffer_trans_apply.h"
#include "pm_config.h"
#include "pm_sleep_config.h"

uint32   num_of_ldo[]        = {7};
uint32   num_of_smps[]       = {2};

/* LpHpCurrentThreshold, SafetyHeadRoom, IsSawControlled, AccessAllowed, AlwaysOn, MinPwrMode, BypassDisallowed, DroopDetect, MinVoltage, MaxVoltage, MinSettlingTime, SettlingErrorEnabled, SettlingEnabled, Reserved */
pm_rpm_ldo_rail_info_type ldo_rail_a[] =
{
    {10, 52,   0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 496, 1328, 0,   PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, //LDO1 LDO510_N1200_100 VDDCX_1 
    {10, 52,   0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_ALLOWED,    PM_DROOP_DETECT_DIS, 1200,1232, 0,   PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO2 LDO510_N1200_100 
    {10, 52,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1232,1296, 0,   PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO3 LDO510_N300_100
    {10, 52,   0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 496, 1328, 0,   PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO4 LDO510_N600_100 VDDMX_1
    {10, 320,  0, PM_ACCESS_ALLOWED, PM_ALWAYS_ON, PM_NPA_SW_MODE_LDO__NPM,   PM_NPA_BYPASS_ALLOWED,    PM_DROOP_DETECT_DIS, 1800,1800, 0,   PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO5 LDO510_MV_P600_500  IO rail
    {10, 320,  0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800,1800, 0,   PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO510_MV_P600_500  
    {10, 24,   0, PM_ACCESS_ALLOWED, PM_NONE,      PM_NPA_SW_MODE_LDO__IPEAK, PM_NPA_BYPASS_DISALLOWED, PM_DROOP_DETECT_DIS, 1800,1800, 0,   PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, // LDO510_LV_P600_100
};

/* LpHpCurrentThreshold, IsSawControlled, AccessAllowed, AlwaysOn, MinPwrMode, MinFreq, MaxFreq, MinVoltage, MaxVoltage, MinSettlingTime, SettlingErrorEnabled, SettlingEnabled, Reserved */
pm_rpm_smps_rail_info_type smps_rail_a[] =
{
    {100, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_SMPS__AUTO, PM_CLK_3p2_MHz,  PM_CLK_3p2_MHz, PM_DROOP_DETECT_DIS, 1200, 1400, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, //	HF BUCK
    {100, 0, PM_ACCESS_ALLOWED,    PM_ALWAYS_ON, PM_NPA_SW_MODE_SMPS__AUTO, PM_CLK_1p92_MHz, PM_CLK_1p92_MHz, PM_DROOP_DETECT_DIS,1800, 2400, 0, PM_SETTLING_ERR_EN, PM_SETTLING_EN, 0}, //	HF BUCK
};

/* AccessAllowed, AlwaysOn */
pm_rpm_clk_info_type clk_info_a[]=
{
    {PM_ACCESS_ALLOWED,      PM_NONE},      //enabled XO for the sleep selection in Twizy 
    {PM_ACCESS_DISALLOWED,   PM_NONE}, //BB_CLK1 *PBS puts in PIN control
    {PM_ACCESS_DISALLOWED,   PM_NONE},      //BB_CLK2, not supported
    {PM_ACCESS_DISALLOWED,   PM_NONE},      //EMPTY
    {PM_ACCESS_ALLOWED,      PM_NONE},      //RF_CLK1
    {PM_ACCESS_DISALLOWED,   PM_NONE},      //RF_CLK2
    {PM_ACCESS_DISALLOWED,   PM_NONE},      //RF_CLK3, not supported
    {PM_ACCESS_DISALLOWED,   PM_NONE},      //DIFF_CLK1
    {PM_ACCESS_DISALLOWED,   PM_NONE},      //EMPTY
    {PM_ACCESS_DISALLOWED,   PM_NONE},      //CLK_DIST *not supported
    {PM_ACCESS_DISALLOWED,   PM_ALWAYS_ON}, //SLEEP_CLK
    {PM_ACCESS_DISALLOWED,   PM_ALWAYS_ON}, //DIV_CLK1
    {PM_ACCESS_DISALLOWED,   PM_NONE}, //DIV_CLK2
    {PM_ACCESS_DISALLOWED,   PM_NONE}  //DIV_CLK3
};

pm_rpm_ldo_rail_info_type* ldo_rail[]=
{
    ldo_rail_a
};

pm_rpm_smps_rail_info_type* smps_rail[]=
{
    smps_rail_a
};

pm_rpm_clk_info_type* clk_info[]=
{
    clk_info_a
};

/* Sleep configuration for enter XO_SHUTDOWN or VDD_MIN. */
static const pm_sleep_reg_type enter_xo_shutdown[] =
{
    {0, 0x5146, 0x01}, // BB_CLK1_EN_CTL: Set BBCLK_NOT_FORCE, and follow Pin
    {0, 0xFFFF, 0x00}, // invalid setting, used to check the end of the array.
};

static const pm_sleep_reg_type enter_vdd_min[] =
{
   {1,0x1440,0xB0},    // Source: Write(PME9205_3p0.S1_CTRL.VSET_LB, 0xB0)
   {1,0x1441,0x04},    // Source: Write(PME9205_3p0.S1_CTRL.VSET_UB, 0x04)
   {1,0x4145,0x02},    // Source: Write(PME9205_3p0.LDO02.MODE_CTL1, 0x02) Forced Bypass
   {1,0x1445,0x05},    // Source: Write(PME9205_3p0.S1_CTRL.MODE_CTL1, 0x05) RM_NPM
   
   {0, 0xFFFF, 0x00}, // invalid setting, used to check the end of the array.
};

const pm_sleep_cfg_type sleep_enter_info[] = {
    {enter_xo_shutdown},
    {enter_vdd_min}
};

static const pm_sleep_reg_type exit_xo_shutdown[] =
{
    {0, 0x5146, 0x81}, // BB_CLK1_EN_CTL: Force BBCLK enable and follow Pin
    {0, 0xFFFF, 0x00}, // invalid setting, used to check the end of the array.
};

static const pm_sleep_reg_type exit_vdd_min[] =
{
    // Will remove these commented S1 entries during mainline, only for review
    //{1, 0x1440,0xE8},       // Source: Write(PME9205_3p0.S1_CTRL.VSET_LB, 0xE8)
    //{1, 0x1441,0x04},       // Source: Write(PME9205_3p0.S1_CTRL.VSET_UB, 0x04)
    {1, 0x4145,0x07},       // Source: Write(PME9205_3p0.LDO02.MODE_CTL1, 0x07)
    {1, 0x1445,0x06},        // Source: Write(PME9205_3p0.S1_CTRL.MODE_CTL1, 0x06)
    
    {0x0, 0x540,  0x01},    // Sending resend all command.
    {0x0, 0x614,  0x01},    // HW bug fix, clear SPMI INT LATCHED bit.
    {0x0, 0xFFFF, 0x00},    // check the end of the array.
};

const pm_sleep_cfg_type sleep_exit_info[] = {
    {exit_xo_shutdown},
    {exit_vdd_min}
};


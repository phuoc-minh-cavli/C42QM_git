/*! \file
 *  
 *  \brief  pm_common_config.c ----This file contains peripheral related settings that are common across PMICs.
 *  \details This file contains peripheral related settings that are common across PMICs.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Resource Setting Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Software Register Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Processor Allocation Information Version: VU.Please Provide Valid Label - Not Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2013-2020 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/config/common/pm_config_common.c#2 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_gpio_driver.h"
#include "pm_clk_driver.h"
#include "pm_ldo_driver.h"
#include "pm_smps_driver.h"
#include "pm_pbs_client_driver.h"
#include "pm_vbus_driver.h"
#include "pm_mpp_driver.h"


pm_gpio_register_info_type gpio_reg[1] = 
{
    {0xC000, 0x100, 0x008, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01A, 0x01B, 0x040, 0x042, 0x043, 0x045, 0x041, 0x046, 0x44}
};
pm_mpp_register_info_type mpp_reg[1] =
{
    {0xA000, 0x100, 0x008, 0x040, 0x042, 0x043, 0x041, 0x046, 0x04C, 0x04A, 0x048}
};
pm_vbus_register_info_type vbus_reg[1] = 
{
    {0x0D00, 0x100, 0x008, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01A, 0x01B}
};
pm_clk_register_info_type clk_reg[1] = 
{
    {0x5000, 0x100, 0x044, 0x046, 0x048, 0x43, 0x08}
};
pm_clk_sleep_register_info_type clk_sleep_reg[1] = 
{
    //SLPSRC_CTL1
    {0x049}
};
pm_pbs_client_register_info_type pbs_client_reg[1] = 
{
    {0x7100, 0x100, 0x042, 0x046, 0x10, 0x11 , 0x12, 0x13, 0x14, 0x15, 0x16, 0x18, 0x19, 0x40}
};

pm_pwr_register_info_type smps_reg[1] = 
{
    {0x1400, 0x300, 0x008, 0x00B, 0x000, 0x045, 0x048, 0x046, 0x040, 0x041, 0x044, 0x000, 0x050, 0x063, 0x04C, 
     0x051, 0x005, 0x000, 0x0DB, 0x061, 0x06C, 0x047, 0x042, 0x043, 0x068, 0x069,0x6B,0x39,0x0A0}
};

pm_pwr_register_info_type ldo_reg[1] = 
{
    {0x4000, 0x100, 0x008, 0x00B, 0x000, 0x045, 0x048, 0x046, 0x040, 0x041, 0x044, 0x000, 0x000, 0x000, 0x000, 
     0x000, 0x005, 0x000, 0x0DB, 0x000, 0x06C, 0x047, 0x042, 0x043, 0x068, 0x069,0x6B,0x39,0x0A0}
};

pm_pwr_volt_info_type fts_volt[2] =
{
    { 0,        1275000,          5000},  //FTS     0 LV  For MV_RANGE = 0: VSET => 0.005V * m, where m = <7:0>
    { 0,        2550000,         10000},  //FTS     1 MV  For MV_RANGE = 1: VSET => 0.010V * m, where m = <7:0>
};

/* Supported LV Range: 0.08V - 1.355V Recommended LV Range: 0.35V - 1.355V
   Supported MV Range: 0.16V - 2.2V   Recommended MV Range: 0.7V - 2.2V */
pm_pwr_volt_info_type fts2p5_volt[2] =
{
    { 80000,        1355000,          5000},  //FTS 2.5  0 LV  For MV_RANGE = 0: VSET => (0.005V * m) + 0.08, where m = <7:0>
    {160000,        2200000,         10000},  //FTS 2.5  1 MV  For MV_RANGE = 1: VSET => (0.010V * m) + 0.16, where m = <7:0>
};

pm_pwr_volt_info_type hfs_volt[2] =
{
    { 375000,   1562500,         12500},  //HFS     0   Band 1
    {1550000,   3125000,         25000},  //HFS     1   Band 2
};


pm_pwr_volt_info_type ult_buck_volt_1[2] = 
{
    { 375000,   1562500,         12500},  //ULT Buck1,2 3 - Range 0
    { 750000,   1525000,         25000},  //ULT Buck1,2 3    Range 1
};

pm_pwr_volt_info_type ult_buck_volt_2[1] = 
{
    { 1550000,   3125000,         25000},  //ULT Buck 4
};


pm_clk_type clk_common[PM_MAX_NUM_CLKS] = 
{
   PM_CLK_XO,
   PM_CLK_BB_1,
   PM_CLK_BB_2,
   PM_CLK_INVALID,
   PM_CLK_RF_1, 
   PM_CLK_RF_2, 
   PM_CLK_RF_3, 
   PM_CLK_DIFF_1,
   PM_CLK_LN_BB,
   PM_CLK_DIST,
   PM_CLK_SLEEP, 
   PM_CLK_DIV_1,
   PM_CLK_DIV_2,
   PM_CLK_DIV_3,
};

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
 *  &copy; Copyright 2013-2018 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.


when        who     what, where, why
--------    ----    ---------------------------------------------------------
05/02/17    sv      Added 0x160D schg_misc_sysok_reason_status register
03/20/17    czq     Added 0xC00A,0xC00B,0xC00C in haptics_reg struct variable
11/15/16    mr      Added support for OLEDB and LCDB
11/14/16   sm      Added 0x0C0 in schg_misc_rer struct variable
11/03/16   aab     Added  OCP CTL reg for Vregs
07/18/16    llioe   Handled register-address differences v1.1(and earlier) vs v2.0
11/03/15    al      Updated with latest
06/30/15    al      Adding Haptics and coincell support
07/10/15    al      Adding support for HP type
08/11/14    al      Adding LPG test registers
11/19/14    al      Adding RTC
05/08/14    dm      Changes for 8994 haptics
03/31/14    aab     Updated to the latest PMIC driver architecture
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_gpio_driver.h"
#include "pm_rtc_driver.h"
#include "pm_rgb_driver.h"
#include "pm_wled_driver.h"
#include "pm_lpg_driver.h"
#include "pm_vib_driver.h"
#include "pm_clk_driver.h"
#include "pm_pwr_alg.h"
#include "pm_smps_driver.h"
#include "pm_pbs_client_driver.h"
#include "pm_schg_driver.h"
#include "pm_fg_driver.h"
#include "pm_ibb_driver.h"
#include "pm_lab_driver.h"
#include "pm_oledb_driver.h"
#include "pm_lcdb_driver.h"
#include "pm_config_sbl_test.h" /* For settings validation test */
#include "pm_haptics_driver.h"
#include "pm_coincell_driver.h"
#include "pm_mpp_driver.h"


/*===========================================================================

                        COMMON MODULE DATA

===========================================================================*/
/* For settings validation test */
pm_settings_test_flag_reg_info_type settings_test_flag_reg[1] = {{0x88F}}; //current targetes (8994,9x45,8909) has this same spare register from primary pmic available. If register changes in future then it needs to be moved in pmic specific config.

pm_pwr_range_info_type fts_range[1] =
{
    {0, 1}
};
pm_pwr_volt_info_type fts_volt[2] =
{
    { 0,        1275000,          5000},  //FTS     0 LV  For MV_RANGE = 0: VSET => 0.005V * m, where m = <7:0>
    { 0,        2550000,         10000},  //FTS     1 MV  For MV_RANGE = 1: VSET => 0.010V * m, where m = <7:0>
};

pm_pwr_range_info_type fts2p5_range[1] =
{
    {0, 1}
};
/* Supported LV Range: 0.08V - 1.355V Recommended LV Range: 0.35V - 1.355V
   Supported MV Range: 0.16V - 2.2V   Recommended MV Range: 0.7V - 2.2V */
pm_pwr_volt_info_type fts2p5_volt[2] =
{
    { 80000,        1355000,          5000},  //FTS 2.5  0 LV  For MV_RANGE = 0: VSET => (0.005V * m) + 0.08, where m = <7:0>
    {160000,        2200000,         10000},  //FTS 2.5  1 MV  For MV_RANGE = 1: VSET => (0.010V * m) + 0.16, where m = <7:0>
};

pm_pwr_range_info_type hfs_range[1] =
{
    {0, 1}
};
pm_pwr_volt_info_type hfs_volt[2] =
{
    { 375000,   1562500,         12500},  //HFS     0   Band 1
    {1550000,   3125000,         25000},  //HFS     1   Band 2
};

pm_pwr_range_info_type n1200_range[1] =
{
    {0, 2}
};
pm_pwr_volt_info_type n1200_volt[3] =
{
    { 375000,   1537500,         12500},  //LDO     0 (auto low range)
    { 375000,   1537500,         12500},  //LDO     1 (ultra low range)
    { 750000,   1537500,         12500},  //LDO     2 (low range)
};

pm_pwr_range_info_type n600_range[1] =
{
    {1, 2}
};
pm_pwr_volt_info_type n600_volt[3] =
{
    {      0,         0,             0},  //LDO     //invalid range   -- place holder
    { 375000,   1287500,         12500},  //LDO     1 (ultra low range)
    { 750000,   1537500,         12500},  //LDO     2 (low range)
};

pm_pwr_range_info_type pmos_range[1] =
{
    {2, 4}
};
pm_pwr_volt_info_type pmos_volt[5] =
{
    { 0,              0,             0},  //invalid range   -- place holder
    { 0,              0,             0},  //invalid range   -- place holder
    { 750000,   1537500,         12500},  //LDO     2 (low range)
    {1500000,   3075000,         25000},  //LDO     3 (mid range)
    {1750000,   4900000,         50000},  //LDO     4(high range)
};

pm_pwr_range_info_type ln_range[1] =
{
    {0, 1}
};
pm_pwr_volt_info_type ln_volt[2] =
{
    {1380000,   2220000,        120000},  //LN_LDO  0
    { 690000,   1110000,         60000},  //LN_LDO  1
};

pm_pwr_range_info_type nmos_range[1] =
{
    {2, 2}
};
pm_pwr_volt_info_type nmos_volt[3] =
{
    { 0,              0,            0},   //invalid range   -- place holder
    { 0,              0,            0},   //invalid range   -- place holder
    { 750000,   1537500,         12500},  //LDO     2 (low range)
};

pm_pwr_range_info_type ult_buck_range_1[1] =
{
    {0, 1}
};
pm_pwr_volt_info_type ult_buck_volt_1[2] =
{
    { 375000,   1562500,         12500},  //ULT Buck1,2 3 - Range 0
    { 750000,   1525000,         25000},  //ULT Buck1,2 3    Range 1
};

pm_pwr_range_info_type ult_buck_range_2[1] =
{
    {0, 0}
};
pm_pwr_volt_info_type ult_buck_volt_2[1] =
{
    { 1550000,   3125000,         25000},  //ULT Buck 4
};

pm_pwr_range_info_type ult_pmos_range[1] =
{
    {0, 0}
};
pm_pwr_volt_info_type ult_pmos_volt[1] =
{
    { 1750000,  3337500,         12500},  //same for P50, P150, P300 and P600 PLDO

};

pm_pwr_range_info_type ult_nmos_range[1] =
{
    {0, 0}
};
pm_pwr_volt_info_type ult_nmos_volt[1] =
{
    { 375000,   1537500,         12500},  //same for N300, N600 and N1200 NLDO
};

/* VMIN , VMAX, Vstep */
pm_pwr_volt_info_type hp_volt[1] =
{
    { 80000,        1355000,          5000}  //Voltage = Vmin + VSET*(Vstep) 0 : 0.08 to 1.355 V at steps of 5 mV (Vmin = 0.08 V, Vstep = 5 mV)
};
pm_pwr_range_info_type hp_range[1] =
{
    {0, 0}
};

pm_clk_register_info_type clk_reg[1] =
{
    {0x5000, 0x100, 0x044, 0x046, 0x048}
};

pm_pwr_register_info_type smps_reg[1] =
{
    {0x1400, 0x300, 0x008, 0x045, 0x048, 0x046, 0x040, 0x041,
     0x054, 0x050, 0x063, 0x060, 0x061, 0x051, 0x0D0, 0x000,
     0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x066, 0x047,
     0x042, 0x043, 0x068, 0x069, 0x06B, 0x039, 0x0A0}
};

pm_pwr_register_info_type ldo_reg[1] =
{
    {0x4000, 0x100, 0x008, 0x045, 0x048, 0x046, 0x040, 0x041,
     0x000, 0x000, 0x000, 0x060, 0x061, 0x000, 0x0D0, 0x000,
     0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x066, 0x047,
     0x042, 0x043, 0x068, 0x069, 0x000, 0x039, 0x0A0}
};

pm_pwr_register_info_type vs_reg[1] =
{
    {0x8000, 0x100, 0x008, 0x045, 0x048, 0x046, 0x000, 0x000,
     0x000, 0x000, 0x000, 0x060, 0x061, 0x000, 0x0D0, 0x000,
     0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x000, 0x047,
     0x042, 0x043, 0x068, 0x069 }
};

pm_pwr_register_info_type boost_reg[1] =
{
    {0xA000, 0x100, 0x008, 0x045, 0x000, 0x046, 0x040, 0x041,
     0x000, 0x000, 0x000, 0x060, 0x061, 0x000, 0x0D0, 0x000,
     0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x000, 0x047,
     0x042, 0x043, 0x068, 0x069}
};

pm_clk_sleep_register_info_type clk_sleep_reg[1] =
{
//  1.smp_ctl1 - 2.cal_rc4
    {0x048, 0x05B}
};

pm_clk_xo_register_info_type clk_xo_reg[1] =
{
//  1.model_ctl1 - 2.xo_adj_fine
    {0x044, 0x05C}
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

pm_gpio_register_info_type gpio_reg[1] =
{
    {0xC000, 0x100, 0x008, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01A, 0x01B, 0x040, 0x042, 0x043, 0x045, 0x041, 0x046, 0x044}
};

pm_rtc_register_info_type rtc_reg[1] =
{
    {0x6000, 0x100, 0x044, 0x046, 0x046, 0x046, 0x014, 0x040, 0x041, 0x042, 0x043, 0x048, 0x049, 0x04A, 0x04B, 0x040, 0x041, 0x042, 0x043, 0x0E0,0x08, 0x10, 0x11, 0x12, 0x13, 0x15, 0x16, 0x18, 0x19, 0x1A, 0x1B }
};

pm_rgb_register_info_type rgb_reg[1] =
{
    {0xD000, 0x100, 0x045, 0x046, 0x047}
};

pm_lpg_register_info_type lpg_reg[1] =
{
    {0xB000, 0x100, 0x005, 0x040, 0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x047, 0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x057, 0x040, 0x041, 0x0C8, 0x0d0, 0x0e2, 0x0e3, 0x0e4, 0x0e5}
};

pm_vib_register_info_type vib_reg[1] =
{
    {0xC000, 0x100, 0x046, 0x051, 0x070}
};

pm_haptics_register_info_type haptics_reg[1] =
{
  {0xC000, 0x0A, 0x0B, 0x0C, 0x046, 0x048, 0x04A, 0x04C, 0x04D, 0x04F, 0x050, 0x051, 0x054, 0x055, 0x056, 0x057, 0x058, 0x05C, 0x05E, 0x070}
};

pm_wled_ctrl_register_info_type wled_ctrl_reg[1]=
{
  {0xD800, 0x01, 0x02, 0x03, 0x04, 0x05, 0x08, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x18, 0x19, 0x1A, 0x1B, 0x44, 0x46, 0x48, 0x49, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5D, 0x5E, 0x5F, 0xD0, 0xE2, 0xE4, 0xE5}
};

pm_wled_sink_register_info_type wled_sink_reg[1]=
{
  {0xD900, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x50, 0x51, 0x52, 0x53, 0x56, 0x57, 0x58, 0x60, 0x61, 0x62, 0x63, 0x66, 0x67, 0x68, 0x70, 0x71, 0x72, 0x73, 0x76, 0x77, 0x78, 0x80, 0x81, 0x82, 0x83, 0x86, 0x87, 0x88, 0xD0}
};

pm_wled_register_info_type wled_reg[1] =
{
    {
      wled_ctrl_reg,
      wled_sink_reg
    }
};

pm_ibb_register_info_type ibb_reg[1] =
{
   {0xDC00, 0x00, 0x01, 0x05, 0x08, 0x09, 0x0A, 0x0B, 0x10, 0x40, 0x41,
            0x44, 0x45, 0x46, 0x47, 0x49, 0x4A, 0x4B, 0x50, 0x55, 0x58,
            0x59, 0x5A, 0x5D, 0x5F, 0x65, 0xD0}
};

pm_lab_register_info_type lab_reg[1] =
{
   {0xDE00, 0x00, 0x01, 0x05, 0x08, 0x09, 0x0A, 0x0B, 0x10, 0x40, 0x41,
            0x44, 0x44, 0x45, 0x46, 0x47, 0x49, 0x4A, 0x4B, 0x4C, 0x50,
            0x5E, 0x70, 0x72, 0x78, 0x7E, 0xD0}
};

pm_oledb_register_info_type oledb_reg[1] =
{
   {0xE000, 0x00, 0x01, 0x08, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x18, 0x19, 0x1A, 0x1B, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x60, 0x64, 0x66, 0xD0, 0xD8, 0xD9, 0xDA, 0xDB, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEF, 0xF0, 0xF1, 0xF2}
};

pm_lcdb_register_info_type lcdb_reg[1] =
{
   {0xEC00, 0x00, 0x01, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x18, 0x19, 0x1A, 0x1B, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x68, 0x69, 0x70, 0x71, 0x74, 0x75, 0x77, 0x7B, 0x7C, 0x7D, 0x7F, 0x80, 0x81, 0x84, 0x85, 0x87, 0x88, 0x8B, 0x8C, 0x8D, 0x8F, 0x90, 0xD0, 0xD8, 0xD9, 0xDA, 0xDB, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4}
};

pm_pbs_client_register_info_type pbs_client_reg[1] =
{
    {0x7100, 0x100,0x0040,0x0041, 0x0042, 0x0046,0x0048, 0x0050, 0x0051}
};


schg_chgr_register_ds schg_chgr_reg[1] =
{
  {
   0x1000, 0x004, 0x005, 0x0D0, 0x0DF, 0x006, 0x007, 0x009, 0x00A, 0x00B, 0x00C, 0x00D, 0x00E, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019,
   0x008, 0x040, 0x041, 0x042, 0x043, 0x044, 0x050, 0x051, 0x052, 0x053, 0x054, 0x060, 0x061, 0x062, 0x063, 0x064, 0x070, 0x071, 0x072, 0x073,
   0x074, 0x075, 0x080, 0x081, 0x082, 0x083, 0x084, 0x085, 0x090, 0x091, 0x092, 0x0A0, 0x0A1, 0x0A2, 0x0B0, 0x0B1, 0x0B2, 0x0B3, 0x0B4, 0x0B5, 0x0B6, 0x0B7, 0x0B8,
   0x0B9, 0x0BA, 0x0BB, 0x0BC, 0x0BD, 0x0BE, 0x0BF, 0x076, 0x077, 0x078, 0x079, 0x07A, 0x07B, 0x07C, 0x07D, 0x07E, 0x07F
  }
};

schg_otg_register_ds schg_otg_reg[1] =
{
   {0x1100, 0x004, 0x005, 0x0D0, 0x0DF, 0x006, 0x007, 0x008, 0x009, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01A, 0x01B, 0x040, 0x051, 0x052,
     0x053}
};

schg_batif_register_ds schg_batif_reg[1] =
{
   {0x1200 , 0x004, 0x005, 0x0D0, 0x0DF, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x040, 0x050, 0x051, 0x052, 0x060,
    0x061, 0x062, 0x070, 0x071, 0x072}
};

schg_usb_register_ds schg_usb_reg[1] =
{
   {0x1300, 0x004, 0x005, 0x0D0, 0x0DF, 0x006, 0x007, 0x008, 0x009, 0x00A, 0x00B, 0x00C, 0x00D, 0x00E, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016,
    0x018, 0x019, 0x040, 0x041, 0x043, 0x057, 0x058, 0x059, 0x060, 0x062, 0x063, 0x064, 0x065, 0x066, 0x067, 0x068, 0x069, 0x070, 0x080, 0x081, 0x082,
    0x083, 0x084}
};

schg_dc_register_ds schg_dc_reg[1] =
{
   {0x1400, 0x004, 0x005, 0x0D0, 0x006, 0x007, 0x008, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x040, 0x0DF, 0x060,
    0x065, 0x070, 0x080, 0x081, 0x082, 0x090, 0x091, 0x092, 0x093, 0x094, 0x095, 0x096, 0x097, 0x098}
};

schg_misc_register_ds schg_misc_reg[1] =
{
   {0x1600, 0x004, 0x005, 0x0D0, 0x0DF, 0x006, 0x007, 0x008, 0x009, 0x00A, 0x00B, 0x00C, 0x00D, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019,
    0x040, 0x041, 0x042, 0x043, 0x044, 0x045, 0x04F, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x060, 0x061, 0x062, 0x070, 0x071, 0x072, 0x073, 0x074,
    0x075, 0x076, 0x080, 0x081, 0x082, 0x083, 0x084, 0x090, 0x091, 0x092, 0x093, 0x094, 0x0C0}
};

schg_bsi_register_ds schg_bsi_reg[1] =
{
   {0x1b00, 0x005, 0x008, 0x009, 0x00a, 0x00d, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01a, 0x01b, 0x046, 0x04f, 0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x059, 0x05a, 0x05b, 0x05c, 0x05d, 0x060, 0x061, 0x062, 0x063, 0x064, 0x065, 0x066, 0x067, 0x068, 0x069, 0x06a, 0x06d, 0x070, 0x0a7, 0x0a8, 0x0a9}
};

schg_register_ds  schg_reg[1]=
{
   {
    schg_bsi_reg,
    schg_chgr_reg,
    schg_otg_reg,
    schg_batif_reg,
    schg_usb_reg,
    schg_dc_reg,
    schg_misc_reg
   }
};

fg_batt_soc_register_ds batt_soc_reg[1] =
{
  {
    0x4000, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x00A, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019,
    0x01A, 0x01B, 0x046, 0x048, 0x04A, 0x052, 0x056, 0x0B6, 0x0B8, 0x0BA, 0x0D0
  }
};

fg_batt_info_register_ds batt_info_reg[2] =
{
  //[0] is latest revision, i.e used for PMI89982.0 and later
  {
    0x4100, 0x004, 0x005, 0x006, 0x007, 0x009, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01A, 0x01B, 0x048, 0x04F, 0x050, 0x051, 0x056, 0x059,
    0x05C, 0x05D, 0x05E, 0x05F, 0x061, 0x062, 0x063, 0x064, 0x065, 0x069, 0x06B, 0x070, 0x071, 0x073, 0x074, 0x075, 0x0A1, 0x0A0, 0x0A3, 0x0A2, 0x0A5, 0x0A4, 0x0A7,
    0x0A6, 0x0A9, 0x0A8, 0x0AB, 0x0Aa, 0x0AD, 0x0AC, 0x0AF, 0x0AE, 0x0D0
  },
  //older revision, i.e used for PMI89981.x and older
  {
    0x4100, 0x004, 0x005, 0x006, 0x007, 0x009, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01A, 0x01B, 0x048, 0x04F, 0x050, 0x051, 0x056, 0x059,
    0x05C, 0x05D, 0x05E, 0x05F, 0x061, 0x062, 0x063, 0x064, 0x065, 0x069, 0x06B, 0x070, 0x071, 0x073, 0x074, 0x075, 0x0A0, 0x0A1, 0x0A2, 0x0A3, 0x0A4, 0x0A5, 0x0A6,
    0x0A7, 0x0A8, 0x0A9, 0x0AA, 0x0AB, 0x0AC, 0x0AD, 0x0AE, 0x0AF, 0x0D0
  }
};

fg_mem_if_register_ds mem_if_reg[1] =
{
  {
    0x4400, 0x001, 0x004, 0x005, 0x007, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01A, 0x01B, 0x050, 0x051, 0x052, 0x054, 0x055, 0x056, 0x057, 0x05F,
    0x060, 0x061, 0x062, 0x063, 0x064, 0x065, 0x066, 0x067, 0x068, 0x069, 0x06A, 0x070, 0x071, 0x072, 0x0D0,
  }
};

fg_bcl_register_ds bcl_reg[1] =
{
  {
   0x4200, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01A, 0x01B, 0x046, 0x048, 0x04E, 0x050, 0x051,
   0x052, 0x053, 0x054, 0x055, 0x056, 0x057, 0x058, 0x059, 0x05A, 0x05B, 0x05C, 0x05D, 0x05E, 0x05F, 0x060, 0x061, 0x062, 0x063, 0x064, 0x065, 0x066, 0x067, 0x06A,
   0x070, 0x072, 0x075, 0x076, 0x078, 0x079, 0x080, 0x081, 0x084, 0x086, 0x087, 0x088, 0x089, 0x090, 0x091, 0x092, 0x093, 0x0A0, 0x0A1, 0x0A2, 0x0A3, 0x0A4, 0x0A5,
   0x0A6, 0x0A7, 0x0A8, 0x0B0, 0x0B2, 0x0B3, 0x0B4, 0x0B6, 0x0B7, 0x0B8, 0x0B9, 0x0BA, 0x0D0
  }
};

fg_adc_rr_register_ds adc_rr_reg[1] =
{
  {
    0x4500, 0x004, 0x005, 0x008, 0x009, 0x00A, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01A, 0x01B, 0x046, 0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x058, 0x059,
    0x05A, 0x05B, 0x060, 0x061, 0x062, 0x063, 0x066, 0x067, 0x068, 0x069, 0x06A, 0x06B, 0x070, 0x071, 0x072, 0x073, 0x074, 0x075, 0x076, 0x080, 0x081, 0x082, 0x083, 0x084, 0x085, 0x086,
    0x087, 0x088, 0x089, 0x08A, 0x08B, 0x090, 0x091, 0x092, 0x094, 0x095, 0x098, 0x099, 0x09A, 0x09C, 0x09D, 0x0A0, 0x0A1, 0x0A2, 0x0A4, 0x0A5, 0x0A8, 0x0A9, 0x0AA, 0x0AC, 0x0AD, 0x0B0,
    0x0B1, 0x0B2, 0x0B3, 0x0B4, 0x0B5, 0x0B8, 0x0B9, 0x0BA, 0x0BB, 0x0BC, 0x0BD, 0x0BE, 0x0BF, 0x0C0, 0x0C1, 0x0C2, 0x0C4, 0x0C5, 0xD0
  }
};

fg_register_ds  fg_reg[1]=
{
   {
     batt_soc_reg,
     batt_info_reg,
     bcl_reg,
     mem_if_reg,
     adc_rr_reg,
   }
};

pm_coincell_register_info_type coincell_reg[1] =
{
    {0x2800, 0x100, 0x046, 0x044, 0x045 }
};

pm_mpp_register_info_type mpp_reg[1] =
{
    {0xA000, 0x100, 0x008, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x018, 0x019, 0x01A, 0x01B, 0x040, 0x042, 0x043, 0x041, 0x046, 0x04C, 0x04A, 0x048}
};

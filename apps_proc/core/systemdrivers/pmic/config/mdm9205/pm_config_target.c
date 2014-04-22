/*! \file
 *  
 *  \brief  rpm_settings.c ----This file contains customizable target specific driver settings & PMIC registers.
 *  \details This file contains customizable target specific 
 * driver settings & PMIC registers. This file is generated from database functional
 * configuration information that is maintained for each of the targets.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Resource Setting Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Software Register Information Version: VU.Please Provide Valid Label - Not Approved
 *    PMIC code generation Processor Allocation Information Version: VU.Please Provide Valid Label - Not Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2010 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/config/mdm9205/pm_config_target.c#1 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_target_information.h"
#include "pm_npa.h"
#include "pm_mpp_driver.h"
#include "pm_gpio_driver.h"
#include "pm_clk_driver.h"
#include "pm_pbs_client_driver.h"
#include "pm_rtc_driver.h"
#include "pmapp_pwr.h"


uint32   num_of_ldo[] = {7} ;
uint32   num_of_smps[] = {2};
uint32   num_of_gpio[] = {4} ;
uint32   num_of_mpp[]  = {0};
uint32   num_of_megaxo[] = {1};
uint32   num_of_pbs_client[] ={2};
uint32   num_of_pwron[] ={0};
uint32   num_of_vbus[] ={0};
uint32   num_of_rtc[] = {1} ;
//=======================================================================================

uint8 num_of_cx_corners = 7;
uint32 pm_cx_corners[7] = 
{ 16, 48, 64, 128, 192, 256, 384};
char PM_APPS_CX_VREG[50] = "/pmic/device/ldo/A/ldo1/vec";

uint8 num_of_mx_corners = 7;
uint32 pm_mx_corners[7] = 
{ 16, 48, 64, 128, 192, 256, 384};
char PM_APPS_MX_VREG[50] = "/pmic/device/ldo/A/ldo4/vec";

pm_pwr_resource_info_type mx_rail[1] = 
{
    {RPM_LDO_A_REQ, 4, &num_of_mx_corners, pm_mx_corners, PM_APPS_MX_VREG }// data 1 is the size of the LUT, data 2 is the MX LUT, data3 is the RPM resource string 
};
pm_pwr_resource_info_type cx_rail[1] = 
{
    {RPM_LDO_A_REQ, 1, &num_of_cx_corners, pm_cx_corners, PM_APPS_CX_VREG }// data 1 is the size of the LUT, data 2 is the CX LUT, data3 is the RPM resource string.
};

uint32 cblpwrn_debounce_timer[] ={ 1000 };

pm_mpp_specific_info_type mpp_specific[1] =
{
    {0x9E0, 4}
};

//use as initial values only, values may be changed runtime and are not static. 
pm_rail_cfg_info_type pm_apps_config_info[4] =
{
    {TRUE, 0},  //Used for Voltage_CTL2 in old scheme, used for VSET_LB in Unified Reg 
    {TRUE, 0},  //Used for PBS_TRIGGER in old scheme, used for VSET_UB in Unified Reg
    {FALSE, 0},
    {FALSE, 0},
};

uint32  apps_stepper_rate = 1500;


pm_pwr_resource_info_type apps_rail[1] =
{
    {RPM_LDO_A_REQ, 1, NULL, NULL, &apps_stepper_rate }
    // data1 : PBS sequence associated with APPS (if necessary)
    // data2 : APPS ocp workaround : disable ocp, save mode, mode=pwm, Enable mss, enable ocp, restore mode
    // data3 : APPS stepper rate
};



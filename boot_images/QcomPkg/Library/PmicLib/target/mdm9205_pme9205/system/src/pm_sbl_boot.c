/*! \file pm_sbl_boot.c
*  \n
*  \brief This file contains PMIC device initialization function where initial PMIC
*  \n SBL settings are configured through the PDM auto-generated code.
*  \n
*  \n &copy; Copyright 2013-2018 Qualcomm Technologies Inc, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/18   sv      Updated to support SMB231 charging.
05/08/18   sv      Updated the configuration of WCSS Retention voltage. (FR45484)
11/19/17   aab     Updated check to support RUMI with out PMIC.    
05/23/17   sv      Added support for PBS AFP.
02/17/17   pxm     Called function to check if auto power on. 
01/12/17   as      L5 retention and active register has been swapped (Cr1110540)
12/27/16   sv      Updating pm_ldo_volt_level call to set LDO5 voltage with micro volt's.
01/09/16   ps      Added support for PM660 and PM660L PMICs (CR-1074489)
10/21/16   akm     Added pm_get_pon_reason_history, pm_pbs_header_info_init in pm_sbl_pre_config
10/07/16   aab     Call pm_sbl_config_skin_temp_threshold() for targets that require charging
09/13/16   aab     Updated logging text 
08/25/16   pxm     Config skin temperature threshold for all target
07/31/16   aab     Added num spmi transaction conditional logging
07/14/16   aab     Updated to support target with out PMI8998
06/02/16   aab     Updated pm_device_programmer_init()
03/23/16   aab     Updated pm_config_ssc_active_and_retention_level()
02/18/16   aab     Updated pm_sbl_chg_init()
02/18/16   aab     Added pm_system_init()
02/18/16   aab     Renamed pm_set_ssc_active_and_retention_levels_mv()  to pm_config_ssc_active_and_retention_level() 
02/05/16   aab     Added pm_set_ssc_active_and_retention_levels_mv()
01/25/16   aab     Updated pm_device_init() and pm_sbl_chg_init()to support RUMI targets with out PMIC 
12/22/15   aab     Added Support for PMI8998 SBL Charging
12/10/15   aab     Added Cx/Mx pvs_retention_data[]  
12/04/15   aab     Updated to support msm8998
10/14/15   aab     Branch from 8996 target
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_boot.h"
#include "pm_ldo.h"
#include "pm_sbl_boot.h"
#include "pm_device.h"
#include "pm_config_sbl.h"
#include "pm_config_sbl_test.h"
#include "pm_app_chgr.h"
#include "pm_app_chgr_common.h"
#include "pm_pbs_info.h"
#include "pm_target_information.h"
#include "pm_sbl_boot_target.h"
#include "pm_log_utils.h"
#include "pm_pbs.h"
#include "pmio_pon.h"
#include "DALDeviceId.h"
#include "DDIPlatformInfo.h"
#include "DDIChipInfo.h"

#include <stdio.h>
#include "boot_logger.h"
#include "CoreVerify.h"
#include "railway.h"
#include "pm_smb.h"
#include "busywait.h"

#define DEBUGBOARD_BAT_ID_LL                    6375        //debug board bat id margin is 15% -/+ of 7.5K  
#define DEBUGBOARD_BAT_ID_UL                    8625 

//#include "pm_utils.h"
#define THREHOLD_encode_mV(x)   ((x -2250)/25)

/*===========================================================================

                     PROTOTYPES 

===========================================================================*/
static pm_err_flag_type pm_sbl_pre_config(void);
static pm_err_flag_type pm_sbl_target_detect(void);
static pm_err_flag_type pm_config_wlan_active_and_retention_level(void);
static pm_err_flag_type pm_config_cx_mx_nom_level(void); 


/*===========================================================================

                        GLOBALS and TYPE DEFINITIONS 

===========================================================================*/
static boolean pm_ram_image_loaded_flag = FALSE;
static boolean verbose_log = FALSE;
static pm_sbl_specific_data_type      *sbl_param_ptr     = NULL;
static pm_sbl_smb_specific_data_type *sbl_chg_param_ptr = NULL;

/*===========================================================================

                        FUNCTION DEFINITIONS 

===========================================================================*/
pm_err_flag_type 
pm_device_init ( void )
{
    static pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint32 initial_num_spmi_trans = pm_log_num_spmi_transaction(0, FALSE);

    err_flag |= pm_device_setup();
    pm_target_information_init();
    
/*     if( (pm_is_target_pre_silicon() == TRUE) && (pm_is_pmic_present(PMIC_A) == FALSE) )
    {
       pm_log_message("Bootup: No PMIC on RUMI Target");
       return err_flag = PM_ERR_FLAG__SUCCESS;
    }
 */
    pm_comm_info_init();

    err_flag |= pm_device_pre_init(); 

    err_flag |= pm_pon_init();      

    err_flag |= pm_pbs_info_rom_init();  /* Read PBS INFO for the pmic rom devices */

    err_flag |= pm_sbl_pre_config(); /* SBL Pre Configuration */       

    err_flag |= pm_sbl_config(); /* SBL Configuration */
    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
       pm_ram_image_loaded_flag = TRUE;
    }    

    err_flag |= pm_sbl_config_test(); /* SBL Configuration validation, only executes complete code if spare reg 0x88F bit 0 is set*/        

    err_flag |= pm_pbs_info_ram_init();  /* Read PBS INFO for the pmic ram devices */

    err_flag |= pm_pbs_ram_version_validation_test(); /* PBS RAM Version validation, only executes complete code if spare reg 0x88F bit 0 is set*/        
    
    err_flag |= pm_device_post_init(); /* Initialize PMIC with the ones PDM can not perform */

    pm_log_num_spmi_transaction(initial_num_spmi_trans, verbose_log);
    
    //Write to Spare bit for pm_device_init_status
   if(err_flag == PM_ERR_FLAG__SUCCESS)
   {
      err_flag = pm_comm_write_byte_mask(PMIC_A_SLAVEID_PRIM, PMIO_PON_PERPH_RB_SPARE_ADDR, PON_PERPH_RB_SPARE_DEVICE_INIT_MASK,PON_PERPH_RB_SPARE_DEVICE_INIT_MASK, 0); 
   }

    return err_flag; /* NON ZERO return means an ERROR */
}

enum
{
    pm_NoCharger,
    pm_SMB2351Charger,
    pm_SMB231Charger,
    pm_InvalidCharger,
};

pm_err_flag_type
pm_sbl_chg_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  boolean smbI2CChargerExist = FALSE;
  uint32 batt_id_ohms = 0;
  // uint8 device_index=0;
  uint8 temp;

  //Check if PMIC is supported in RUMI
  if( (pm_is_target_pre_silicon() == TRUE) && (pm_is_pmic_present(PMIC_A) == FALSE) )
  {
     pm_log_message("Bootup: No PMIC on RUMI Target");
     return PM_ERR_FLAG__SUCCESS;
  }
  err_flag = pm_smb_get_charger_exist_status(&smbI2CChargerExist);
  if((PM_ERR_FLAG__SUCCESS == err_flag) && (smbI2CChargerExist == TRUE))
  {

    err_flag = pm_sbl_chg_pre_init();
    err_flag |= pm_sbl_get_batt_id(&batt_id_ohms);
    if (err_flag != PM_ERR_FLAG__SUCCESS)
      {
       pm_log_message("ERROR, pm_sbl_get_batt_id()");
      }
    if((batt_id_ohms >= DEBUGBOARD_BAT_ID_LL) && (batt_id_ohms <= DEBUGBOARD_BAT_ID_UL))
      {
      // disable the charging as debug board detected
      err_flag |= pm_smb_enable_charging(TRUE);// disable charging --its  active HIGH
      pm_log_message("debug board found");
    }
          //Handle SMB231 charging stuff here.
          err_flag |= pm_sbl_config_chg_parameters();
          // err_flag |= pm_sbl_chg_check_weak_battery_status(device_index);
          
          if(err_flag != PM_ERR_FLAG__SUCCESS)
          {
             pm_log_message("ERROR, pm_sbl_chg_check_weak_battery_status()");
          }
      }
  else
  {
        err_flag  |= pm_comm_write_byte(0, 0x3F49, THREHOLD_encode_mV(3200), 0);
        err_flag  |= pm_comm_write_byte(0, 0x3F40, 0x07, 0);
        err_flag  |= pm_comm_write_byte(0, 0x3F46, 0x80, 0);
        pm_busy_wait(100);
        err_flag  |= pm_comm_read_byte(0, 0x3F10, &temp, 0);

        if(temp & 0x02)
        {
            pm_log_message("ERROR, VBAT too LOW!! ");
            pm_device_shutdown();
        }
   }
   
   //configure the GPIO-1 thus it can sent INT to SPMI PIC -FR:handling INT's on SOI
   err_flag |= pm_comm_write_byte(0x0,0xC011,0x01,0x0);     // Edge trigger interrupts
   err_flag |= pm_comm_write_byte(0x0,0xC012,0x01,0x0);     // Raising edge enable
   
   if (smbI2CChargerExist)
      {
      err_flag |= pm_comm_write_byte(0x0,0xC042,0x05,0x0);     // no pull up enable
   
      }
   else
  {
      err_flag |= pm_comm_write_byte(0x0,0xC013,0x01,0x0);     // falling trigger enable
  }
   err_flag |= pm_comm_write_byte(0x0,0xC014,0x01,0x0);     // Clears the internal sticky
   err_flag |= pm_comm_write_byte(0x0,0xC015,0x01,0x0);     // Interrupt Enable
   err_flag |= pm_comm_write_byte(0x0,0xC040,0x00,0x0);     // DIGITAL_INPUT
   err_flag |= pm_comm_write_byte(0x0,0xC046,0x80,0x0);     // GPIO1_EN_CTL 
   err_flag |= pm_sbl_chg_post_init();  

  return err_flag;
}



pm_err_flag_type
pm_device_programmer_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  
  if( (pm_is_target_pre_silicon() == TRUE) && (pm_is_pmic_present(PMIC_A) == FALSE) )
  {
     pm_log_message("Bootup: No PMIC on RUMI Target");
     return err_flag = PM_ERR_FLAG__SUCCESS;
  }
  
  err_flag |= pm_device_setup();
  pm_target_information_init();
  pm_comm_info_init();

  err_flag |= pm_pon_init();
    
  return err_flag;
}




static pm_err_flag_type
pm_sbl_pre_config(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  if (sbl_param_ptr == NULL)
  {
     sbl_param_ptr = (pm_sbl_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_SPECIFIC_DATA);
  }
  CORE_VERIFY_PTR(sbl_param_ptr);

  if (sbl_chg_param_ptr == NULL)
  {
     sbl_chg_param_ptr = (pm_sbl_smb_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_SPECIFIC_DATA);
  }
  CORE_VERIFY_PTR(sbl_chg_param_ptr);
  verbose_log = sbl_param_ptr->verbose_uart_logging;

  err_flag |= pm_log_pon_reasons(FALSE);

  err_flag |= pm_sbl_target_detect();

  err_flag = pm_pbs_header_info_init(); 
  
  pm_get_pon_reason_history();
  
  return err_flag;
}




pm_err_flag_type
pm_sbl_target_detect(void) /* SBL Target detect */
{
    static pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS; 

    return err_flag;
}

pm_err_flag_type pm_system_init(void)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

   //Twizy has no wlan
   //err_flag = pm_config_wlan_active_and_retention_level(); 

   err_flag = pm_config_cx_mx_nom_level(); 

   return err_flag;
}

static pm_err_flag_type 
pm_config_wlan_active_and_retention_level(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint32 retention_voltage_mv = 0x00;
    uint32 active_voltage_mv = 0x00;
    uint8  retention_level[2] = {0};
    uint8  rail = rail_id("vdda_wlan");
    uint8 primary_pmic_index = 0;

    //Store Retention Voltage to S5 and S2 scratch registers
    retention_voltage_mv = railway_get_corner_voltage(rail,  RAILWAY_RETENTION) / 1000; //get Retention voltage level
    retention_level[0]  =  (uint8)( retention_voltage_mv & 0xFF);
    retention_level[1]  =  (uint8)((retention_voltage_mv >> 8) & 0xFF);
    err_flag |= pm_comm_write_byte(PM_SCRATCH_SLAVE_ADDR, S2_SCRATCH_RET_ADDR_LB, retention_level[0], 0);
    err_flag |= pm_comm_write_byte(PM_SCRATCH_SLAVE_ADDR, S5_SCRATCH_RET_ADDR_UB, retention_level[1], 0);
    
    //Get Active Voltage from railway driver
    active_voltage_mv = railway_get_corner_voltage(rail,  RAILWAY_NOMINAL ) / 1000; //get Active voltage level
    //Configure L2A with Active Level
    err_flag |= pm_ldo_volt_level(primary_pmic_index, PM_LDO_2, (active_voltage_mv * 1000));

    //Temp WA to store WCSS Active voltage to PBS RAM    
    busywait (10);  // Delay 10 us
    //Trigger HW CTRL (GPIO7 High to low will be configured)
    err_flag  |= pm_comm_write_byte(0, 0x7242, 0x01, 0);  // configure Trigger HW CTRL
    
    //Temp WA (CR-2274999): Turn ON LDO2 in XBL as workaround for PCNOC error issue when SDI enable in TZ
    err_flag  |= pm_comm_write_byte(1, 0x4146, 0x80, 0);  // Enable LDO2A
    return err_flag;
}


static pm_err_flag_type 
pm_config_cx_mx_nom_level(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint32 cx_nom_voltage_mv = 0x00;
    uint32 mx_nom_voltage_mv = 0x00;
    uint8  cx_nom_level[2] = {0};
    uint8  mx_nom_level[2] = {0};
    uint8  cx_rail = rail_id("vddcx");
    uint8  mx_rail = rail_id("vddmx");

    //Store cx/mx Nom Voltages to pmic scratch registers, 
	//PBS will apply these nom voltages to cx/mx on next pon onwords
    //LDO01_MISC1 | 0x00014086   as CX_NOM_LB
    //LDO04_MISC1 | 0x00014386   as CX_NOM_UB
    //MISC_MISC_SPARE_PERPH_RB 0x95A   as MX_NOM_LB
    //MISC_MISC_SPARE_XVDD_RB  0x95C   as MX_NOM_UB

    cx_nom_voltage_mv = railway_get_corner_voltage(cx_rail,  RAILWAY_NOMINAL) / 1000; //get cx Nom voltage level
    mx_nom_voltage_mv = railway_get_corner_voltage(mx_rail,  RAILWAY_NOMINAL) / 1000; //get mx Nom voltage level
    cx_nom_level[0]  =  (uint8)( cx_nom_voltage_mv & 0xFF);
    cx_nom_level[1]  =  (uint8)((cx_nom_voltage_mv >> 8) & 0xFF);
    mx_nom_level[0]  =  (uint8)( mx_nom_voltage_mv & 0xFF);
    mx_nom_level[1]  =  (uint8)((mx_nom_voltage_mv >> 8) & 0xFF);
    err_flag  = pm_comm_write_byte(PM_LDO_MISC_SLAVE_ID, LDO01_MISC1_ADDR, cx_nom_level[0], 0);
    err_flag |= pm_comm_write_byte(PM_LDO_MISC_SLAVE_ID, LDO04_MISC1_ADDR, cx_nom_level[1], 0);

    err_flag |= pm_comm_write_byte(PM_MISC_SLAVE_ID, MISC_MISC_SPARE_PERPH_RB_ADDR, mx_nom_level[0], 0);
    err_flag |= pm_comm_write_byte(PM_MISC_SLAVE_ID, MISC_MISC_SPARE_XVDD_RB_ADDR, mx_nom_level[1], 0);

    return err_flag;
}
pm_err_flag_type 
pm_check_pbs_ram_warm_reset_seq_presence(uint32 device_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    return err_flag;
}

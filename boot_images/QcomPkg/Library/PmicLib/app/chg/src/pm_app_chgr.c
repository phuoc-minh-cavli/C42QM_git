/*! \file
*  
*  \brief  pm_app_schgr.c
*  \details Implementation file for pmic sbl charging algorithm
*    
*  \n &copy; Copyright (c)2016-2018  by QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/18   pbitra  Updated pm_smb_get_charging_status API
05/04/18   sv      Added Dead battery charging support for SMB1351.
05/31/17   czq     Rerun APSD unconditionally.
05/03/17   czq     Detect the bad conditions of FG SRAM (trim registers) and issue VDD hard reset on PM/PMI to recover.
04/25/17   sv      Updated pm_sbl_auto_power_on_check with pon reason.
04/14/17   czq     Improve legacy cable mis-detction WA.
03/27/17   pbitra  Remove bat id conversion config from pm_sbl_enable_batt_detection()
03/14/17   pxm     Need Manually setting DCP ICL to 1500mA. 
02/17/17   pxm     Added function to check if auto power on.
02/10/17   mr      Enabled RGB LED during Charging
02/02/17   cs      SBL needs to set the batt id missing threshold to 0 to get max threshold.
01/19/17   cs      let UEFI enable battery missing detection to add more delay before enable batt missing.
12/19/16   sv      Disabled RGB LED for SDM660.
12/16/16   pxm     Added fake batt_id configuration function
10/20/16   aab     Added Batt_therm Out of bound temperature check
10/13/16   pxm     Pet (reset) BARK and BITE watchdog timers in charging loop
09/30/16   aab     Updated logging:  Used  pm_log_messag() macro
09/01/16   ps      Added support for PM660 and PM660L PMICs (CR-1074489)
08/25/16   pxm     Add skin temperature threshold configuration function 
08/12/16   aab     Added debugboard detection support 
11/08/16   pxm     Updated pm_sbl_schg_get_charger_progress_status() to latest WiPower Algorithm
11/08/16   pxm     Removed toggling of SHDN_N_CLEAR_CMD Reg when DC_IN path detected  
08/02/16   yps     Fixed compiling issue for Clang38
07/26/16   aab     Updated pm_sbl_schg_config_red_led()
07/07/16   aab     Updated to support saving Entry Vbatt/SOC state to smem
05/27/16   pxm     Implement wipower power customization
03/15/16   aab     Driver update (Support for PMI8998)
02/16/16   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "com_dtypes.h"
#include "boothw_target.h"
#include "boot_api.h"
#include "CoreVerify.h"

#include "pm_chg.h"
#include "pm_app_chgr.h"
#include "pm_smb.h"
#include "smem.h"
#include "pm_comm.h"
#include "pm_utils.h"
#include "pm_log_utils.h"
#include "pm_target_information.h"
#include "DDIAdc.h"
#include "pm_app_chg.h" 

/*=========================================================================== 
 
                     GLOBAL TYPE DEFINITIONS
 
===========================================================================*/
#define  PM_WEAK_BATTERY_CHARGING_DELAY    500 * 1000   //500ms


//static pm_sbl_schg_vlowbatt_threshold_data_type pm_dbc_bootup_volt_threshold;
static boolean verbose_chg_log = TRUE;        //TBD: Needs update sbl charging logging option.  Probably need to use boot-profiler which keeps log messages default off 
//static uint32 batt_id_ohms = 0;

/*===========================================================================

                     PROTOTYPES 

===========================================================================*/

pm_err_flag_type pm_app_chgr_check_batt_temp_limits(uint32 device_index, pm_sbl_schg_batt_therm_type *batt_therm_ptr);
pm_err_flag_type pm_sbl_schg_set_icl(uint32 device_index);



/*===========================================================================

                     FUNCTION IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_sbl_chg_get_vbus_detect_status(boolean *vBusDetected)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg = 0xD08;
    pm_register_data_type data = 0;
    
    if(NULL == vBusDetected)
    {
        return PM_ERR_FLAG__INVALID_PARAMETER;
    }
    
    err_flag = pm_comm_read_byte(0, reg, &data, 0);
    if( 0x80 == (0x80 & data))
    {
        *vBusDetected = TRUE;
    }
    else
    {
        *vBusDetected = FALSE;
    }
    return err_flag;
}

pm_err_flag_type pm_sbl_chg_check_weak_battery_status(uint32 device_index)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

   static pm_sbl_smb_specific_data_type *chg_param_ptr = NULL;
   boolean LowBatt = TRUE;
   boolean usbin_present = FALSE;
   boolean vbatt_weak_status        = TRUE;
   boolean bat_missing              = FALSE;
   boolean chg_prog_message_flag    = FALSE;
   boolean chg_progress_status      = FALSE;
   
   chg_param_ptr = (pm_sbl_smb_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_SPECIFIC_DATA);
   CORE_VERIFY_PTR(chg_param_ptr);
   verbose_chg_log = chg_param_ptr->verbose_uart_logging;
   
   
   //Check Battery presence
   err_flag |= pm_smb_get_batt_missing_status(&bat_missing);
   if( bat_missing == TRUE )
   {
      pm_log_message("NO Battery: Suspend SMB and bootup the device"); 
      err_flag = pm_smb_cfg_suspend(TRUE);
      return err_flag;
   }

   err_flag |= pm_smb_get_low_bat_threshold_status(&LowBatt);
   if(LowBatt == TRUE)
   {
      pm_log_message("PMIC SBL Bootup: Battery is Low");
   }
   else
   {
      pm_log_message("PMIC SBL Bootup: Battery is Good");
      return err_flag;
   }
   
   err_flag |= pmapp_chg_usb_get_vbus_presence(&usbin_present);
   if(usbin_present == FALSE) 
   {
       boot_hw_powerdown();
   }

   while( vbatt_weak_status == TRUE )  //While battery is in weak state
   {   
      err_flag |= pm_smb_get_low_bat_threshold_status(&LowBatt);
      if ( err_flag != PM_ERR_FLAG__SUCCESS )  
      { 
          return err_flag;
      }
      
      if ( !LowBatt)  //If battery is above threshold then Bootup
      {
         vbatt_weak_status = FALSE;
         break; //Battery good : BootUp
      }

      err_flag |= pm_smb_get_charging_status(&chg_progress_status);
      if (chg_progress_status == FALSE)
      {
         pm_log_message("ERROR, Charging is NOT in progress: Shutting Down");
         //Read the error from SMB and Handle JEITA.
         boot_hw_powerdown();
      }
      else
      {
         pm_log_message("SBL Charging in progress....");
         chg_prog_message_flag = TRUE;
         err_flag |= pm_busy_wait(500000);
      }

   }//while

   if (err_flag != PM_ERR_FLAG__SUCCESS)
   {
      pm_log_message("ERROR, In SBL Charging ...");
   }
   else if (chg_prog_message_flag == TRUE)
   {
      if(verbose_chg_log)
      {
        pm_log_message("SBL Charging completed.");
      }
   }

   return err_flag; 
}

pm_err_flag_type
pm_sbl_get_debug_board_presence(uint32 device_index, pm_sbl_schg_specific_data_type *chg_param_ptr, boolean *debug_board_present)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
/*  // Need to update.
  boolean bat_missing       = FALSE;

   CORE_VERIFY_PTR(chg_param_ptr);
   CORE_VERIFY_PTR(debug_board_present);

   //Check Battery presence before doing batt_id ADC reading
   err_flag |= pm_schg_batif_irq_status(device_index, PM_SCHG_BATIF_BAT_THERM_OR_ID_MISSING, PM_IRQ_STATUS_RT, &bat_missing);
   if (err_flag != PM_ERR_FLAG__SUCCESS)
   {
      pm_log_message("ERROR, pm_schg_batif_irq_status()");
      return err_flag;
   }

   if( bat_missing == TRUE )
   {
      pm_log_message("BOOTUP, NO Battery"); 
      *debug_board_present = TRUE;
      return err_flag;
   }

   if ( (batt_id_ohms >=chg_param_ptr->debug_board_batt_id_pd_res_range.min_vlue) &&
        (batt_id_ohms <=chg_param_ptr->debug_board_batt_id_pd_res_range.max_value) )
   {
      *debug_board_present = TRUE;
      pm_log_message("BOOTUP, Debug Board being used"); 
   }
   else
   {
      *debug_board_present = FALSE;
   }
*/
   return err_flag; 
}



pm_err_flag_type pm_sbl_chg_config_vbat_low_threshold(pm_smb_batif_low_battery_threshold lowBattThreshold)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   
   err_flag = pm_smb_set_low_bat_threshold(lowBattThreshold); // Can move this to Target Config file.
   
   return err_flag; 
}



pm_err_flag_type pm_sbl_config_chg_parameters(void)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

   static pm_sbl_smb_specific_data_type *chg_param_ptr;
   
   if(chg_param_ptr == NULL)
   {
      chg_param_ptr = (pm_sbl_smb_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_CHG_SPECIFIC_DATA);
      CORE_VERIFY_PTR(chg_param_ptr);
   }
   
   //Vlowbatt Threshold  
   if (chg_param_ptr->dbc_bootup_volt_threshold.enable_config == PM_ENABLE_CONFIG)
   {
      pm_smb_batif_low_battery_threshold lowBattThreshold = chg_param_ptr->dbc_bootup_volt_threshold.vlowbatt_threshold;
      err_flag = pm_smb_set_low_bat_threshold(lowBattThreshold);
      err_flag = pm_smb_enable_charging(FALSE);
      err_flag |= pm_busy_wait(50000);
   }
   
   //Battery Missing Detection Source 
   if (chg_param_ptr->bat_miss_detect_src.enable_config == PM_ENABLE_CONFIG)
   {
      //Default it enabled through Battery therm pin. TODO: update if configuration needed.
      //boolean en_bat_missing_via_batid_therm_pin = chg_param_ptr->bat_miss_detect_src.en_bat_missing_via_batid_therm_pin;
      //err_flag |= pm_schg_batif_cfg_batt_missing_src(device_index, PM_SCHG_BAT_MISS_SRC_CFG__PIN_SRC_EN, en_bat_missing_via_batid_therm_pin);
      
   }

   //Float Voltage : 3600mV to 4500 mv                   
   if (chg_param_ptr->float_volt_theshold_mv.enable_config == PM_ENABLE_CONFIG)
   {
      //TODO: if configuration needed add support here
   }
   
   //Enable/Disable JEITA Hard Temp Limit Check in SBL
   if (chg_param_ptr->enable_jeita_hard_limit_check == TRUE)
   {
      //TODO: if configuration needed add support here
   }

   return err_flag; 
}

pm_err_flag_type pm_sbl_schg_info_to_smem_update()
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  

  return err_flag; 
}


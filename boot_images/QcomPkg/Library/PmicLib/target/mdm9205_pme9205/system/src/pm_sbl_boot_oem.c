/*! \file pm_sbl_boot_oem.c
*  \n
*  \brief This file contains PMIC device initialization function where initial PMIC
*  \n SBL settings are configured through the PDM auto-generated code.
*  \n
*  \n Copyright (c) 2015-2017, 2023 Qualcomm Technologies, Inc. All rights reserved.
*/

/*===========================================================================

                        Edit History

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when        who     what, where, why
--------    ---     ---------------------------------------------------------
07/18/17    ks      RF_CLK2.CLKBUFF_HOLD_CTL  to 0x01 (CR 2076624 )
06/30/17    ks      Added the condional code to enable/disable the RFCLK1(CR2011208)
04/10/17    ks      Configured LPI CX/MX pin controlled setting to stable solution.
03/31/17   pbitra   pm_sbl_chg_pre_init(); added a call pm_sbl_set_batt_id_forced_mode()
03/16/17    sv      Remove Charger FSM configuration for uUSB and add factory-mode configuratiion (CR#2020297)
02/09/17    sv      Enable LDO2B during bootup
12/12/16    mr      Config TypeC/uUSB Operation Mode
01/09/16    ps      Added support for PM660 and PM660L PMICs (CR-1074489)
04/29/15    aab     Updated pm_device_post_init()
12/04/15    aab     Updated to support MSM8998 target
12/04/15    aab     Creation
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include <stdio.h>
#include "CoreVerify.h"
#include "boot_logger.h"
#include "DDIChipInfo.h"
#include "DALDeviceId.h"
#include "DDIPlatformInfo.h"
#include "pm_sbl_boot_target.h"
#include "pm_app_pon.h"
#include "pm_sbl_boot.h"
#include "pm_device.h"
#include "pm_comm.h"
#include "pm_utils.h"
#include "pm_ldo.h"
#include "pm_smps.h"
#include "pm_app_vreg.h"
#include "pm_uefi.h"
#include "pm_target_information.h"
#include "pm_clk.h"
#include "ddr_drivers.h"
#include "SpmiCfg.h"
#include "SpmiBus.h"
#include "pm_dal_prop_ids.h"
#include "pm_smb.h"
#include "pm_utils.h"
#include "pm_log_utils.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
#define PM660L_LCD_VARIENT 0x2
#define PME9205_BOOT_OPTION_FLAG     0x11
#define PME9205_TINY_MODEM_WITH_APPS 0x11
#define PME9205_TINY_MODEM_NO_APPS   0x01

#ifdef KEY_PRESS_DBNC
#define PM_PON_KPDPWR_N_RESET_S2_CTL2_ADDR   0x843
#define PM_PON_KPDPWR_N_RESET_S2_CTL2_S2_RESET_EN_BMSK    0x80
#endif

pm_err_flag_type pm_sbl_ddr_config_init(void);
pm_err_flag_type pm_sbl_spmi_reconfiguration(void);
/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

pm_err_flag_type
pm_device_pre_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_sbl_specific_data_type *config_param_ptr = NULL;

  config_param_ptr = (pm_sbl_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SBL_SPECIFIC_DATA);
  CORE_VERIFY_PTR(config_param_ptr);

  if(config_param_ptr->rfclk1_config_flag)
  {
  	err_flag |= pm_comm_write_byte(0, 0x5447, 0x0, 0);	// Do NOT follow pin control
	err_flag |= pm_comm_write_byte(0, 0x5448, 0x80, 0);	// Enable pull down for RF_CLK1 peripheral
  }
  

  return err_flag;
}

pm_err_flag_type
pm_device_post_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
#ifdef KEY_PRESS_DBNC
  uint8                     rtc_alarm_read[4];
#endif
  //These configurations is only used for development phones and should be commented out for production phones
  err_flag |= pm_app_pon_pshold_cfg(PM_APP_PON_CFG_WARM_RESET);
#ifdef KEY_PRESS_DBNC
  err_flag = pm_comm_write_byte_mask(0, PM_PON_KPDPWR_N_RESET_S2_CTL2_ADDR, PM_PON_KPDPWR_N_RESET_S2_CTL2_S2_RESET_EN_BMSK, 0x0, 0);
#else
  err_flag |= pm_app_pon_reset_cfg( PM_APP_PON_RESET_SOURCE_KPDPWR, PM_APP_PON_CFG_WARM_RESET, 10256, 2000); //PON KPDPWR PON Reset configuration
#endif
  err_flag |= pm_app_pon_reset_cfg( PM_APP_PON_RESET_SOURCE_RESIN_AND_KPDPWR, PM_APP_PON_CFG_DVDD_HARD_RESET, 10256, 2000); //PON RESIN_AND_KPDPWR PON Reset configuration

  err_flag |= pm_app_pon_reset_init();

  /*check for valid PON keypress */
  err_flag |= pm_app_pwrkey_long_press_check(PM_PON_PWRKEY_DBNC_CHK_AT_LOADER);
#ifdef KEY_PRESS_DBNC
  err_flag |= pm_comm_read_byte(0,0xa40,&rtc_alarm_read[0],0);
  err_flag |= pm_comm_read_byte(0,0xa41,&rtc_alarm_read[1],0);
  err_flag |= pm_comm_read_byte(0,0xa46,&rtc_alarm_read[2],0);
  err_flag |= pm_comm_read_byte(0,0xa47,&rtc_alarm_read[3],0);
  if((rtc_alarm_read[0]==0)&&(rtc_alarm_read[1]==0)&&(rtc_alarm_read[2]==0)&&(rtc_alarm_read[3]==0)) //in xvdd case
  {
    err_flag |= pm_comm_write_byte(0,0xa40,0xFE,0); //writing reset state
    err_flag |= pm_comm_write_byte(0,0xa41,0xFE,0); //writing reset state
    err_flag |= pm_comm_write_byte(0,0xa46,0xFE,0); //writing reset state
    err_flag |= pm_comm_write_byte(0,0xa47,0xFE,0); //writing reset state
  }
  else
  {

    err_flag |= pm_comm_write_byte(0,0xa40,0xFF,0); //writing reset state
    err_flag |= pm_comm_write_byte(0,0xa41,0xFF,0); //writing reset state
    err_flag |= pm_comm_write_byte(0,0xa46,0xFF,0); //writing reset state
    err_flag |= pm_comm_write_byte(0,0xa47,0xFF,0); //writing reset state
  }
  pm_log_message("Error flag %d :cleared AON reg \n\r",err_flag);
#endif
  return err_flag;
}

pm_err_flag_type
pm_driver_pre_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  return err_flag;
}


pm_err_flag_type
pm_driver_post_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 data =0;
  uint32 *ocimem_address = (uint32 *)SHARED_IMEM_PMIC_BASE;
  err_flag = pm_sbl_ddr_config_init();

  //hema ...check if needed
  err_flag  |= pm_comm_write_byte(0, 0x6044, 0x40, 0);  // Write(PM660.RTC_RW.TIME_ADJ, 0x40)
  err_flag  |= pm_comm_write_byte(0, 0x6046, 0x80, 0);  // Write(PM660.RTC_RW.EN_CTL1, 0x80)
  
  // Read YODA.3.0 MISC_MISC_MDM_BOOT_OPTION (Address: 0x00000095D) data update to OCIMEM address 0x08628FFC  
  err_flag  |= pm_comm_read_byte(0, 0x095D, &data, 0);
  *ocimem_address = (uint32 )data;
  
  return err_flag;
}

pm_err_flag_type
pm_sbl_chg_pre_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
#ifdef AWARE_SMB_CONFIG
  uint8 data = 0;
#endif
    //open access 
    err_flag |= i2c_smb_write_mask(0x12, 0x30, 0x80,0x80);
    err_flag |= i2c_smb_write_mask(0x12, 0x09, 0xFF,0x01);  //enable INT's only for INOK
#ifdef AWARE_SMB_CONFIG
  err_flag = i2c_smb_read(0x12 , 0x39, &data);
  if (data & 0x10)
  {
    boot_log_message("NO Battery Found");
    return PM_ERR_FLAG__SUCCESS;
  }
  else
  {
    boot_log_message("Battery Found");
  }
//fii add for SMB231 settings.
  err_flag |= i2c_smb_write_mask(0x12, 0x31, 0x03,0x03);//force to 500mA mode/ACmode
  err_flag |= i2c_smb_write_mask(0x12, 0x00, 0xFF,0x19);//100-500mA-1500mA-30mA
  err_flag |= i2c_smb_write_mask(0x12, 0x01, 0xF0,0xF0);//pre-75mA-3.0V-3.25V
  err_flag |= i2c_smb_write_mask(0x12, 0x02, 0xFF,0x4F);//en-80-en-always-shut-1000mA
  err_flag |= i2c_smb_write_mask(0x12, 0x03, 0xFF,0x92);//500mA-4.20V
  err_flag |= i2c_smb_write_mask(0x12, 0x04, 0xDF,0x12);//auto-360min-45min-vbatt
  err_flag |= i2c_smb_write_mask(0x12, 0x05, 0xFF,0x34);//en-sus-com-120-dis-dis
  err_flag |= i2c_smb_write_mask(0x12, 0x06, 0xF0,0x20);//vbatt-lowirq
  err_flag |= i2c_smb_write_mask(0x12, 0x07, 0x07,0x00);//Vin supply-inok
  err_flag |= i2c_smb_write_mask(0x12, 0x08, 0xFF,0x51);//5-15-45-55
  err_flag |= i2c_smb_write_mask(0x12, 0x09, 0xFF,0xFD);//safety-ov-mis-hard-soft-irq
#endif
  return PM_ERR_FLAG__SUCCESS;
}

pm_err_flag_type
pm_sbl_chg_post_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  return err_flag;
}

pm_err_flag_type pm_sbl_ddr_config_init (void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 ddr_type;
  
  //TODO_TWIZY: Update these DDR configs
  //Get DDR_TYPE
  ddr_type = ddr_get_device_type();
  #if 0
  if((ddr_type == DDR_TYPE_LPDDR2) || (ddr_type == DDR_TYPE_LPDDR3))
  {
      err_flag |= pm_smps_volt_level(PMIC_A, PM_SMPS_5, 1350 * 1000);  //Set S5 Voltage
      err_flag |= pm_ldo_volt_level(PMIC_A, PM_LDO_8, 1200 * 1000);    //Set L8 Voltage
  }
  else if(ddr_type == DDR_TYPE_PCDDR3L)
  {
      err_flag |= pm_smps_volt_level(PMIC_A, PM_SMPS_5, 1400 * 1000);  //Set S5 Voltage
      err_flag |= pm_ldo_volt_level(PMIC_A, PM_LDO_8, 1350 * 1000);    //Set L8 Voltage
  }
  else if(ddr_type == DDR_TYPE_PCDDR3)
  {
      
      err_flag |= pm_ldo_volt_level(PMIC_A, PM_LDO_8, 1304 * 1000);  //Set L8 Voltage
      //wait for settle
      
      err_flag |= pm_smps_volt_level(PMIC_A, PM_SMPS_5, 1304 * 1000); //Set S5 to LDO8 Voltage
      
      //Keep L8 in By-pass mode
      err_flag |= pm_comm_write_byte(1, 0x4754, 0x01, 0); //Enable LDO8 bypass
      err_flag |= pm_comm_write_byte(1, 0x4753, 0x80, 0); //DIG BYPASS when parent S5 voltage = LDO8 voltage.
      
      err_flag |= pm_smps_volt_level(PMIC_A, PM_SMPS_5, 1500 * 1000);  //Set SMPS5 to 1500mV
  }
 #endif
  return err_flag;
}

pm_err_flag_type pm_qsee_pre_target_init( void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  
// FR58302: AP Only Boot ,gave RTC alarm/PON access to APPS  
#ifdef APPS_ONLY_BOOT
  err_flag = pm_sbl_spmi_reconfiguration();
#else    
  uint8 *ocimem_address = (uint8 *)SHARED_IMEM_PMIC_BASE;
  if ( ((*ocimem_address) & PME9205_BOOT_OPTION_FLAG) == PME9205_TINY_MODEM_NO_APPS ) // reassigning spmi configs depending upon tiny boot image without APPS only
  {
    err_flag = pm_sbl_spmi_reconfiguration();
  }   
#endif

  return err_flag;
}

pm_err_flag_type pm_sbl_spmi_reconfiguration()
{
	pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
	uint32 index = 0;
	SpmiCfg_ChannelCfg*  spmi_cfg_ptr       = NULL;
	uint32* spmi_cfg_size                   = NULL;
    uint8 periphID = 0x08,periphID1 = 0xC0;
    pm_periph_owner intOwner = PM_MSS_OWNER,intOwner1 = PM_MSS_OWNER ;
    uint8 break_flag = 2, break_count =0;
    
	spmi_cfg_ptr = (SpmiCfg_ChannelCfg*)pm_target_information_get_specific_info(PM_PROP_SPMI_CHANNEL_CFG);
	CORE_VERIFY_PTR(spmi_cfg_ptr);

	spmi_cfg_size = (uint32*)pm_target_information_get_specific_info(PM_PROP_SPMI_CHANNEL_CFG_SIZE);
	CORE_VERIFY_PTR(spmi_cfg_size);
  
	if(spmi_cfg_ptr == NULL || spmi_cfg_size == NULL)
	{
		return PM_ERR_FLAG__INVALID_PARAMETER;
	}
    
    // FR58302: AP Only Boot , change the RTC alarm/Misc access to APPS
    #ifdef APPS_ONLY_BOOT
    periphID = 0x61;
    intOwner = PM_APPS_HLOS_OWNER;
    periphID1 = 0x09;
    intOwner1 = PM_APPS_HLOS_OWNER;
    break_flag = 2;
    #endif
     
	//Re-configure SPMI IntOwner  Permission for PAGE ONLY image
	for(index = 0; index < spmi_cfg_size[0]; index++)
	{
		
			if( (spmi_cfg_ptr[index].periphId == periphID ) && ( spmi_cfg_ptr[index].slaveId == 0) ) 
			{
				//Update the Ppid array with GPIO4 register address
				//pvc_port_cfg_ptr[index]->ppids[1].address = 0x0105;  //dummy
				//pvc_port_cfg_ptr[index]->ppids[2].address = 0x0105;  //dummy
				//pvc_port_cfg_ptr[index]->ppids[0].address = 0x0105;  //dummy
				spmi_cfg_ptr[index].intOwner = intOwner;
                break_count++;
			}	
            if( (spmi_cfg_ptr[index].periphId == periphID1 ) && ( spmi_cfg_ptr[index].slaveId == 0) ) 
			{
				//Update the Ppid array with GPIO4 register address
				//pvc_port_cfg_ptr[index]->ppids[1].address = 0x0105;  //dummy
				//pvc_port_cfg_ptr[index]->ppids[2].address = 0x0105;  //dummy
				//pvc_port_cfg_ptr[index]->ppids[0].address = 0x0105;  //dummy
				spmi_cfg_ptr[index].intOwner = intOwner1;
                break_count++;
			}
            if(break_count == break_flag )
                break;
	}
	return err_flag;
}


/*! \file
*
*  \brief  pm_smb.c
*  \details Implementation file for SMB resourece type.
*
*  Copyright (c) 2018, 2019, 2023 Qualcomm Technologies, Inc. All rights reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

when       who     what, where, why
---------- ---     ----------------------------------------------------------
05/22/2018 pbitra Updated with APIs, covering more SMB functionality
05/05/2018 sv     Creation
===========================================================================*/

#include "DALDeviceId.h"
#include "I2cDevice.h"
#include "DalDevice.h"
#include "DALStdErr.h"
#include "DDITlmm.h"
#include "I2cApi.h"
#include "pm_smb.h"
#include "boot_logger.h"

#include "pm_target_information.h"

static boolean  pm_sbl_i2c_smb_init_flag = FALSE;
#define SMB_I2C_SLAVE_ADDR 0x12
#define I2C_DEFAULT_BUS_FREQ   400
#define I2C_TRANSFER_TIMEOUT_US 2500

#define BIT(n) (1 << (n))

/* SMB1351 charger registers. */


#define SMB_FCC_ICL_REG 0x00
#define AC_ICL_MASK 0x0F

#define SMB_PIN_ENABLE_CTRL_REG 0x07
#define CMD_DIS_CHG_POLARITY_MASK 0x20

#define SMB_FLEX_CHARGE_REG 0x01
#define LOW_BATT_THRSHLD_MSK 0x0F

#define SMB_CMD_IL_REG 0x30
#define SUSPEND_MASK 0x4
#define CHG_ENABLE_MASK 0x02

//#define SMB_CMD_CHG_REG 0x32


//#define SMB_STATUS_REG5_REG 0x3B  //APSD result status
//#define SORCE_DETECTED_MASK 0xFF

#define SMB_CHARGING_STATUS_REGISTER_A          0X3C
#define SMB_STATUS_REG4_REG 0x3D
#define CHARGING_STATUS_MASK 0x06

//#define SMB_STATUS_REG8_REG 0x3E
#define USBIN_STATUS_MASK 0x04
#define SMB_CHARGING_SYSOK_OPTIONS      0X07

#define SMB_IRQ_B_REG        0x39
#define BATT_MISS_STATUS       BIT(5)
#define LOW_BATT_VOLT_STATUS   BIT(2)

//#define SMB_IRQ_G_REG 0x46  //APSD done status
//#define SORCE_DETECT_STATUS_MASK BIT(6)

#define DATA_SIZE 1

static VOID *i2c_handle  = NULL;

pm_err_flag_type pm_smb_cfg_suspend(boolean Suspend)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8  smbcfg   = 0;
  
  if (Suspend == TRUE)
  {
      smbcfg = 0x40;
  }
  
  err_flag = i2c_smb_write_mask(SMB_I2C_SLAVE_ADDR,SMB_CMD_IL_REG, SUSPEND_MASK, smbcfg );
  if (PM_ERR_FLAG__SUCCESS != err_flag)
  {
    //boot_log_message("pm_smb_cfg_suspend: I2C write Failed");
  }
  
  return err_flag;
}


pm_err_flag_type pm_smb_set_low_bat_threshold (pm_smb_batif_low_battery_threshold vBatLowThreshold)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  
  err_flag = i2c_smb_write_mask(SMB_I2C_SLAVE_ADDR,SMB_FLEX_CHARGE_REG, LOW_BATT_THRSHLD_MSK, vBatLowThreshold );
  if (PM_ERR_FLAG__SUCCESS != err_flag)
  {
    //boot_log_message("pm_smb_set_low_bat_threshold: I2C write Failed");
  }
  
  return err_flag;
}



pm_err_flag_type pm_smb_get_batt_missing_status(boolean *battMissing)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8  smbStatus   = 0;
  
  if(NULL == battMissing)
  {
    return PM_ERR_FLAG__INVALID_PARAMETER;
  }
  
  err_flag = i2c_smb_read(SMB_I2C_SLAVE_ADDR,SMB_IRQ_B_REG, &smbStatus );
  
  *battMissing = FALSE;
  if (PM_ERR_FLAG__SUCCESS != err_flag)
  {
    //boot_log_message("pm_smb_get_batt_missing_status: I2C Read Failed");
    return err_flag;
  }
  if(BATT_MISS_STATUS & smbStatus)
  {
      *battMissing = TRUE;
  }
  return err_flag;
}

pm_err_flag_type pm_smb_get_low_bat_threshold_status(boolean *LowBatt)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8  smbStatus   = 0;
  
  if(NULL == LowBatt)
  {
    return PM_ERR_FLAG__INVALID_PARAMETER;
  }
  
  err_flag = i2c_smb_read(SMB_I2C_SLAVE_ADDR,SMB_IRQ_B_REG, &smbStatus );
  
  *LowBatt = FALSE;
  if (PM_ERR_FLAG__SUCCESS != err_flag)
  {
    //boot_log_message("pm_smb_get_low_bat_threshold_status: I2C Read Failed");
    return err_flag;
  }
  
  if(LOW_BATT_VOLT_STATUS & smbStatus)
  {
      *LowBatt = TRUE;
  }
  return err_flag;
}


pm_err_flag_type pm_smb_get_charging_status(boolean *chargingInprogress)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8  ChgStatus   = SMB_INVALID_CHARGING;
  
  if(NULL == chargingInprogress)
  {
    return PM_ERR_FLAG__INVALID_PARAMETER;
  }
  
  err_flag = i2c_smb_read(SMB_I2C_SLAVE_ADDR,SMB_STATUS_REG4_REG, &ChgStatus );
  
  *chargingInprogress = FALSE;
  if (PM_ERR_FLAG__SUCCESS != err_flag)
  {
    //boot_log_message("pm_smb_get_charging_status: I2C Read Failed");
    return err_flag;
  }
  
  if((((CHARGING_STATUS_MASK & ChgStatus)>>1) > SMB_NO_CHARGING) &&  ( ((CHARGING_STATUS_MASK & ChgStatus)>>1) < SMB_INVALID_CHARGING ))
  {
      *chargingInprogress = TRUE;
  }
  else
  {
      *chargingInprogress = FALSE;
  }
  return err_flag;
}

pm_err_flag_type pm_smb_get_charger_exist_status(boolean *ChargeExist)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  //Initialize I2C 
  err_flag = pm_smb_init();
  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
      *ChargeExist = FALSE;  // Failed to initialize I2C treat it as SMB1351 not exists.
      err_flag = PM_ERR_FLAG__SUCCESS; //Failing to initialize SMB Due to SMB not exist so return success.
  }
  else
  {
      *ChargeExist = TRUE;   //SMB1351 detected.
  }
  return err_flag;
}

pm_err_flag_type pm_smb_enable_charging(boolean ChargeEnable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  if(ChargeEnable)
  {
	  err_flag = i2c_smb_write_mask(SMB_I2C_SLAVE_ADDR,SMB_CMD_IL_REG, CHG_ENABLE_MASK, CHG_ENABLE_MASK );
	  if (PM_ERR_FLAG__SUCCESS != err_flag)
	  {
		//boot_log_message("pm_smb_enable_charging: I2C write Failed");
	  }
  }
  else
  {
  	  err_flag = i2c_smb_write_mask(SMB_I2C_SLAVE_ADDR,SMB_CMD_IL_REG, CHG_ENABLE_MASK, 0 );
	  if (PM_ERR_FLAG__SUCCESS != err_flag)
	  {
		//boot_log_message("pm_smb_enable_charging: I2C write Failed");
	  }

  }
  return err_flag;
}

#if 0
pm_err_flag_type pm_smb_get_apsd_status(boolean *result)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data = 0;

   err_flag = i2c_smb_read(SMB_I2C_SLAVE_ADDR,SMB_IRQ_G_REG , &data );
   if (PM_ERR_FLAG__SUCCESS != err_flag)
   {
	 //boot_log_message("pm_smb_get_apsd_status: I2C Read Failed");
	 return err_flag;
   }
   *result = (data & SORCE_DETECT_STATUS_MASK) ? 1: 0;

   return err_flag;
}
#endif

pm_err_flag_type pm_smb_get_apsd_result_status(pm_smb_apsd_result_status_type *apsd_result)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   #if 0
   pm_register_data_type data = 0;
   int8 usb_type = 0x07;

   err_flag = i2c_smb_read(SMB_I2C_SLAVE_ADDR,SMB_STATUS_REG5_REG , &data );
   if (PM_ERR_FLAG__SUCCESS != err_flag)
   {
	 //boot_log_message("pm_smb_get_apsd_result_status: I2C Read Failed");
	 return err_flag;
   }
  
   *apsd_result = PM_SMB_APSD_RESULT_STATUS_INVALID;
      /*Check for charger port type starting from bit 7 to 0*/
      while (usb_type >= 0)
      {
        if (data & (1<<usb_type))
        {
           *apsd_result = (pm_smb_apsd_result_status_type)usb_type;
           break;
        }
        usb_type--;
      }
  #endif
   return err_flag;
}


pm_err_flag_type pm_smb_set_usbin_current_limit_cfg(pm_smb_ac_icl_max icl_max)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;

   if (icl_max>SMB_AC_ICL_MAX_MAX)
   {
      icl_max = SMB_AC_ICL_MAX_MAX;
   }
   err_flag = i2c_smb_write_mask(SMB_I2C_SLAVE_ADDR,SMB_FCC_ICL_REG, AC_ICL_MASK, icl_max );
   if (PM_ERR_FLAG__SUCCESS != err_flag)
   {
   	  //boot_log_message("pm_smb_set_usbin_current_limit_cfg: I2C write Failed");
  }

  return err_flag;
}

pm_err_flag_type pm_smb_get_usbin_status(boolean *Valid)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data = 0;
   *Valid = FALSE;

   err_flag = i2c_smb_read(SMB_I2C_SLAVE_ADDR,SMB_CHARGING_STATUS_REGISTER_A , &data );
   if (PM_ERR_FLAG__SUCCESS != err_flag)
   {
	 //boot_log_message("pm_smb_get_usbin_status: I2C Read Failed");
	 return err_flag;
   }
   if ((data & USBIN_STATUS_MASK) >> 2)
   {
	 *Valid = TRUE;
   }
  
   return err_flag;
}

pm_err_flag_type pm_smb_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8  smbStatus   = 0;

  /* Initialize the I2C Core for SMB1351 charger */
  err_flag = pm_sbl_i2c_smb_init();
  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
      //boot_log_message("pm_smb_init: I2C SMB init Failed");
      return err_flag;
  }
  //Read SMB status register
  err_flag = i2c_smb_read(SMB_I2C_SLAVE_ADDR,SMB_IRQ_B_REG, &smbStatus );
  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
    //boot_log_message("pm_smb_init: I2C Read Failed ");
    return err_flag;
  }
 /* 
  else
  {
      //Print SMB read status. if needed
		
	  // Charger related initialization 	  
	  // CMD_DIS_CHG_POLARITY=0 => 0 in command register disables charger
	  err_flag = i2c_smb_write_mask(SMB_I2C_SLAVE_ADDR,SMB_PIN_ENABLE_CTRL_REG, CMD_DIS_CHG_POLARITY_MASK, 0 );
	  if (PM_ERR_FLAG__SUCCESS != err_flag)
	  {
		//boot_log_message("pm_smb_init: I2C write Failed");
	  }
  }
  */
  return PM_ERR_FLAG__SUCCESS;
     
}

pm_err_flag_type pm_smb_deinit(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  /* DeInitialize the I2C Core for SMB1351 charger */
  err_flag = pm_sbl_i2c_smb_exit();
  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
      //boot_log_message("pm_smb_deinit: I2C SMB deinit Failed");
      return err_flag;
  }
  return PM_ERR_FLAG__SUCCESS;
     
}

pm_err_flag_type pm_sbl_i2c_smb_init(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  i2c_status istatus = I2C_SUCCESS;
  /* Initialize the I2C Core for external bucks DBU1 and DBU2 control*/
  if(pm_sbl_i2c_smb_init_flag == FALSE)
  {
  istatus = i2c_open((i2c_instance) I2C_INSTANCE_002, &i2c_handle);  // I2C_INSTANCE_002 GPIO 6&7
  if (I2C_SUCCESS != istatus)
  {
    //boot_log_message("pm_sbl_i2c_smb_init: Failed to initialize I2C");
    err_flag = PM_ERR_FLAG__I2C_OPT_ERR;
  }
  pm_sbl_i2c_smb_init_flag = TRUE;
  }
  return err_flag;
}

pm_err_flag_type pm_sbl_i2c_smb_exit(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    if (I2C_SUCCESS != i2c_close(i2c_handle))
    {
      //boot_log_message("pm_sbl_i2c_smb_exit: Failed to DeInitialize I2C ");
      err_flag = PM_ERR_FLAG__I2C_OPT_ERR;
    }
    return err_flag;
}


pm_err_flag_type i2c_smb_write_mask(uint8 slaveAddress, uint8 offset, uint8 mask,  uint8 data)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  i2c_status istatus = I2C_SUCCESS;
  UINT32    bytes_written    = 0;
  UINT32    bytes_read =0;
  UINT8     writebuf =0;
  UINT8     readbuf =0;
  i2c_config cfg;
  
  cfg.bus_frequency_khz = I2C_DEFAULT_BUS_FREQ;
  cfg.slave_address = slaveAddress;
  cfg.slave_address_type = I2C_07_BIT_SLAVE_ADDRESS;
  //cfg.mode = I2C;
  //cfg.slave_max_clock_stretch_us = 500;
  //cfg.core_configuration1 = 0;
  //cfg.core_configuration2 = 0;
  
  
  istatus = i2c_read (i2c_handle, &cfg, offset, 1, &readbuf, 1, &bytes_read, I2C_TRANSFER_TIMEOUT_US);
  if (I2C_SUCCESS != istatus)
  {
    //boot_log_message("I2C Read Failed");
    err_flag = PM_ERR_FLAG__I2C_OPT_ERR;
  }
  
  readbuf &= (~mask) ; 
  writebuf = (readbuf|(data & mask));
   
  istatus = i2c_write (i2c_handle, &cfg, offset, 1, &writebuf, 1, &bytes_written, I2C_TRANSFER_TIMEOUT_US);
  if (I2C_SUCCESS != istatus)
  {
    //boot_log_message("I2C Write Failed");
    err_flag = PM_ERR_FLAG__I2C_OPT_ERR;
  }
  
  return err_flag;
}

pm_err_flag_type i2c_smb_read(uint8 slaveAddress, uint8 offset, uint8 *data)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  i2c_status istatus = I2C_SUCCESS;
  UINT32    bytes_read =0;
  UINT8     readbuf =0;
  i2c_config cfg;
  
  cfg.bus_frequency_khz = I2C_DEFAULT_BUS_FREQ;
  cfg.slave_address = slaveAddress;
  cfg.slave_address_type = I2C_07_BIT_SLAVE_ADDRESS;
  //cfg.mode = I2C;
  //cfg.slave_max_clock_stretch_us = 500;
  //cfg.core_configuration1 = 0;
  //cfg.core_configuration2 = 0;
  
  if(NULL == data)
  {
    return PM_ERR_FLAG__INVALID_PARAMETER;
  }
  istatus = i2c_read (i2c_handle, &cfg, offset, 1, &readbuf, 1, &bytes_read, I2C_TRANSFER_TIMEOUT_US);
  if (I2C_SUCCESS != istatus)
  {
    //boot_log_message("I2C Read Failed");
    err_flag = PM_ERR_FLAG__I2C_OPT_ERR;
  }
  *data = readbuf;
  return err_flag;
}

pm_err_flag_type pm_smb_sysok_option(uint8 option)
{
  pm_err_flag_type  err_flag	= PM_ERR_FLAG__SUCCESS;

  if(option > 3)
    return PM_ERR_FLAG__INVALID;

  err_flag = i2c_smb_write_mask(SMB_I2C_SLAVE_ADDR,SMB_CHARGING_SYSOK_OPTIONS,0x03,option);
  if(PM_ERR_FLAG__SUCCESS != err_flag)
    return PM_ERR_FLAG__I2C_OPT_ERR;

  return err_flag;
}
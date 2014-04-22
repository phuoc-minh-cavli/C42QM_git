/*! \file
*  
*  \brief  pm_app_chg.c
*  \wraper to get the charger details 
*    
*  \n &copy; Copyright (c)2016-2018, 2019  by QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/18   ks      Creation of wrapper api's of charger
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_app_chg.h"
#include "pm_version.h"
#include "hw_module_type.h"
#include "pm_schg_usb.h"
//#include "pm_smbchg_misc.h"
//#include "pm_smbchg_usb_chgpth.h"
#include "device_info.h"
#include "pm_config_target.h"
#include "DDIChipInfo.h"
#include "pm_smb.h"
#include "pm_comm.h"

/*=========================================================================== 
 
                     GLOBAL TYPE DEFINITIONS
 
===========================================================================*/

#define CHG_PERIPHERAL_BASE_ADDRESS 0x1000

#define GPIO01_INT_RT_STS_ADDRESS 0xC010 
static peripheral_info_type  chg_peripheral_info;
static uint8 pm_app_chg_pmic_index = 0xFF; //PMIC_IS_INVALID
static boolean pm_app_chg_initialized = FALSE;
/*===========================================================================

                     PROTOTYPES 

===========================================================================*/

/*===========================================================================

                     FUNCTION IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_app_chg_init()
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 sid_index = 0;
  uint8 pmic_device_index = 0;
  chg_peripheral_info.base_address = CHG_PERIPHERAL_BASE_ADDRESS;
  boolean smbI2CChargerExist = FALSE;
  
  if(!pm_app_chg_initialized)
  {
    for (pmic_device_index = 0; pmic_device_index < PM_MAX_NUM_PMICS; pmic_device_index++)  
    {
      sid_index = pmic_device_index * 2;
      err_flag = pm_get_peripheral_info(sid_index, &chg_peripheral_info);
      if ( PM_HW_MODULE_CHARGER == chg_peripheral_info.peripheral_type)
      {
        pm_app_chg_pmic_index = pmic_device_index; //    first PMIC with a charger should be the Primary Charger PMIC
        break;
      }
    }
    if(PMIC_IS_INVALID == pm_app_chg_pmic_index) //if no internal charger found
    {
      err_flag = pm_smb_get_charger_exist_status(&smbI2CChargerExist); //have to overwrite the err flag as it return error "invalid_access_attempted" if no charger found
    }
    pm_app_chg_initialized = TRUE;
  }
  return err_flag;
}

pm_err_flag_type pmapp_chg_usb_get_charger_type(pmapp_chg_usb_apsd_result_status_type *src_detected)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
//  pm_smbchg_misc_src_detect_type smbchg_src_detected = PM_SMBCHG_MISC_SRC_DETECT_INVALID;
  pm_schg_usb_apsd_result_status_type schg_src_detected = PM_SCHG_USB_APSD_RESULT_STATUS_INVALID;
  boolean smbI2CChargerExist = FALSE;
  pm_smb_apsd_result_status_type apsd_result = PM_SMB_APSD_RESULT_STATUS_INVALID;
  uint32 chipId = DalChipInfo_ChipId();
  
  if(NULL == src_detected)
  {
    return PM_ERR_FLAG__INVALID_PARAMETER;
  }
  
  if(DALCHIPINFO_ID_MDM9205 == chipId) 
  {
    //Read SMB231 I2C data
    err_flag = pm_smb_get_charger_exist_status(&smbI2CChargerExist);
    if((PM_ERR_FLAG__SUCCESS == err_flag) && (smbI2CChargerExist == TRUE))
    {
      err_flag |= pm_smb_get_apsd_result_status(&apsd_result);
      switch(apsd_result)
      {
        case PM_SMB_APSD_RESULT_STATUS_SDP_CHARGER:
             *src_detected = PM_APP_CHG_SDP_CHARGER;
             break;
        case PM_SMB_APSD_RESULT_STATUS_OCP_CHARGER:
             *src_detected = PM_APP_CHG_OCP_CHARGER;
             break;
        case PM_SMB_APSD_RESULT_STATUS_CDP_CHARGER:
             *src_detected = PM_APP_CHG_CDP_CHARGER;
             break;
        case PM_SMB_APSD_RESULT_STATUS_DCP_CHARGER:
             *src_detected = PM_APP_CHG_DCP_CHARGER;
             break;
        case PM_SMB_APSD_RESULT_STATUS_ACA_DOCK:
             *src_detected = PM_APP_CHG_ACA_DOCK;
             break;
        case PM_SMB_APSD_RESULT_STATUS_ACA_C_CHARGER:
             *src_detected = PM_APP_CHG_ACA_C_CHARGER;
             break;
        case PM_SMB_APSD_RESULT_STATUS_ACA_B_CHARGER:
             *src_detected = PM_APP_CHG_ACA_B_CHARGER;
             break;
        case PM_SMB_APSD_RESULT_STATUS_ACA_A_CHARGER:
             *src_detected = PM_APP_CHG_ACA_A_CHARGER;
             break;
        case PM_SMB_APSD_RESULT_STATUS_INVALID:
             *src_detected = PM_APP_CHG_INVALID;
             break;
        default:
             break;
      }
      return err_flag;
    }
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  
  if(!pm_app_chg_initialized)
  {
    err_flag = pm_app_chg_init();
  }
  if(PMIC_IS_INVALID != pm_app_chg_pmic_index)
  {
/*     if (PM_HW_MODULE_CHARGER_SMBCHG_CHG == chg_peripheral_info.peripheral_subtype )
    {
      err_flag |= pm_smbchg_misc_chgr_port_detected(pm_app_chg_pmic_index, &smbchg_src_detected);
      switch(smbchg_src_detected)
      {
        case PM_SMBCHG_MISC_SRC_DETECT_SDP:
             *src_detected = PM_APP_chg_SDP_CHARGER;
             break;
        case PM_SMBCHG_MISC_SRC_DETECT_CDP:
             *src_detected = PM_APP_CHG_CDP_CHARGER;
             break;
        case PM_SMBCHG_MISC_SRC_DETECT_DCP:
             *src_detected = PM_APP_CHG_DCP_CHARGER;
             break;
        case PM_SMBCHG_MISC_SRC_DETECT_OTHER_CHARGING_PORT:
             *src_detected = PM_APP_CHG_FLOAT_CHARGER;
             break;
        case PM_SMBCHG_MISC_SRC_DETECT_INVALID:
             *src_detected = PM_APP_CHG_INVALID;
             break;
        default:
             break;
      }
    }
    else */ if(PM_HW_MODULE_CHARGER_SCHG_CHG == chg_peripheral_info.peripheral_subtype)
    {
      err_flag |= pm_schg_usb_get_apsd_result_status(pm_app_chg_pmic_index, &schg_src_detected);
      switch(schg_src_detected)
      {
        case PM_SCHG_USB_APSD_RESULT_STATUS_SDP_CHARGER:
             *src_detected = PM_APP_CHG_SDP_CHARGER;
             break;
        case PM_SCHG_USB_APSD_RESULT_STATUS_OCP_CHARGER:
             *src_detected = PM_APP_CHG_OCP_CHARGER;
             break;
        case PM_SCHG_USB_APSD_RESULT_STATUS_CDP_CHARGER:
             *src_detected = PM_APP_CHG_CDP_CHARGER;
             break;
        case PM_SCHG_USB_APSD_RESULT_STATUS_DCP_CHARGER:
             *src_detected = PM_APP_CHG_DCP_CHARGER;
             break;
        case PM_SCHG_USB_APSD_RESULT_STATUS_FLOAT_CHARGER:
             *src_detected = PM_APP_CHG_FLOAT_CHARGER;
             break;
        case PM_SCHG_USB_APSD_RESULT_STATUS_QC_2P0:
             *src_detected = PM_APP_CHG_QC_2P0;
             break;
        case PM_SCHG_USB_APSD_RESULT_STATUS_QC_3P0:
             *src_detected = PM_APP_CHG_QC_3P0;
             break;
        case PM_SCHG_USB_APSD_RESULT_STATUS_ICL_OVERRIDE_LATCH:
             *src_detected = PM_APP_CHG_ICL_OVERRIDE_LATCH;
             break;
        case PM_SCHG_USB_APSD_RESULT_STATUS_INVALID:
             *src_detected = PM_APP_CHG_INVALID;
             break;
        default:
             break;
      }
    }
    else
    {
       return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
  }
  else
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
   return err_flag;
}

pm_err_flag_type pmapp_chg_usb_get_vbus_presence(boolean *status)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
//  pm_smbchg_usb_chgpth_input_sts_type input_type;
  boolean smbI2CChargerExist = FALSE;
  uint32 chipId = DalChipInfo_ChipId();
  uint8 data = 0;
  
  if(NULL == status)
  {
    return PM_ERR_FLAG__INVALID_PARAMETER;
  }

  if(DALCHIPINFO_ID_MDM9205 == chipId) 
  {
    //Read SMB1351 I2C data
    err_flag = pm_smb_get_charger_exist_status(&smbI2CChargerExist);
    if((PM_ERR_FLAG__SUCCESS == err_flag) && (smbI2CChargerExist == TRUE))
    {
      err_flag = pm_smb_get_usbin_status(status);
      return err_flag;
    }
    else
    {
		err_flag = pm_comm_read_byte(0x0, GPIO01_INT_RT_STS_ADDRESS, &data, 0x0); // MPP RT status
        *status = (data & 1);
        return err_flag;
    }
  }
  
  if(!pm_app_chg_initialized)
  {
    err_flag = pm_app_chg_init();
  }
  if(PMIC_IS_INVALID != pm_app_chg_pmic_index)
  {
/*     if ( PM_HW_MODULE_CHARGER_SMBCHG_CHG == chg_peripheral_info.peripheral_subtype)
    {
     err_flag |= pm_smbchg_usb_chgpth_input_sts(pm_app_chg_pmic_index, PM_SMBCHG_CHAR_TYPE_USB, &input_type);
     if(PM_SMBCHG_5V_9V_CHGR_DETECTED == input_type)
     {
       *status = TRUE;
     }    
     else
     {
       *status = FALSE;  
     }
    }
    else */ if(PM_HW_MODULE_CHARGER_SCHG_CHG == chg_peripheral_info.peripheral_subtype)
    {
      err_flag |= pm_schg_usb_irq_status(pm_app_chg_pmic_index, PM_SCHG_USB_IRQ_USBIN_PLUGIN, PM_IRQ_STATUS_RT, status);
    }
    else
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
  }
  else
  {
    if(smbI2CChargerExist)
    {
      err_flag |= pm_smb_get_usbin_status(status);
    }
  }

  return err_flag;
}

pm_err_flag_type pmapp_chg_usb_get_cc_status(pmapp_chg_usb_cc_status_type *cc_status)
{
  return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;  //no support on this target
}

pm_err_flag_type pmapp_chg_usb_get_usb_connector_type(pmapp_chg_usb_connector_type *usb_type)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
//  boolean smbI2CChargerExist = FALSE;
  uint32 chipId = DalChipInfo_ChipId();
  
  if(NULL == usb_type)
  {
    return PM_ERR_FLAG__INVALID_PARAMETER;
  }
  if(DALCHIPINFO_ID_MDM9205 == chipId) 
  {
    //Read SMB1351 I2C data
    //err_flag = pm_smb_get_charger_exist_status(&smbI2CChargerExist);
   // if((PM_ERR_FLAG__SUCCESS == err_flag) || (smbI2CChargerExist == TRUE))
  //  {
  //    err_flag = pm_smb_get_usbin_status(status);
  //    return err_flag;
  //  }
    *usb_type = PM_APP_CHG_USB_CONNECTOR_MICRO_USB; //WR for QCS405 BU,rest of two SKU's are based on WALL CHARGER
    return err_flag;
  }
  else
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  
  return err_flag;
}


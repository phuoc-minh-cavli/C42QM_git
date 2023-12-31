/*! @file PmicUsbProtocol.c

*  PMIC- USB MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support
*  the PMIC USB module.
*
*  Copyright (c) 2015-17 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Inc Proprietary and Confidential.
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/01/18   pbitra  Change SetVconn API input to call by value
06/01/18   pbitra  Added support for PmicUsbGetPortRoleSetTime
06/08/17   pbitra  Added API to set USB connector type
05/18/17   pbitra  Check Charger Present though Vbus instead of CC connection
04/04/17   pbitra  Added API EFI_PmicUsbGetUSBConnectorType
02/23/17   mr      Added Protocol API to get Charger (TypeC/uUSB) presence
11/28/16   mr      Removed PmicIndex from PmicUsbProtocol APIs
09/19/16   sv      Updated to support SDM660.
06/09/16   sm      Redefined GetOtgStatus API
06/02/16   sm      Added changes to remove support for PMI8998 v1.0
06/01/16   sm      Removed EFI_QCOM_CHARGER_EX_PROTOCOL related changes
05/10/16   sm      Added API to return charger presence
05/09/16   sm      Enabled code inside EFI_PmicUsbEnableOtg()
04/06/16   sm      Completed the API definitions.
04/06/16   sm      Made changes to always assume we have bad PMIv1.0 part
                   and always run workaround
03/07/16   sm      Added TypeC related APIs (Skeleton)
04/14/15   al      added support for PMI8950
02/11/15   sv      added support for 8909 and 8916
01/30/15   al      New file.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/QcomBaseLib.h>

#include <Protocol/EFIPlatformInfo.h>
#include <Protocol/EFIPmicSchg.h>
#include <Protocol/EFIPmicUsb.h>

#include "pm_uefi.h"
#include "pm_version.h"
#include "pm_schg_misc.h"
#include "pm_schg_usb.h"
#include "PmicFileLog.h"
#include "pm_app_chg.h"
#include "DDIChipInfo.h"
/*===========================================================================

                        GLOBAL DATA DEFINITIONS

===========================================================================*/
STATIC EFI_QCOM_PMIC_SCHG_PROTOCOL  *PmicSchgProtocol = NULL;
STATIC UINT32                       PmicDeviceIndex = 0xFF;
STATIC UINT64                             gPortRoleSetTimeInmS   = 0;


/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/
STATIC
EFI_STATUS
EFIPmicUsbInit
(
  VOID
)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32 chipId = DalChipInfo_ChipId();
  
  if(DALCHIPINFO_ID_QCS405 == chipId)
  {
    return EFI_SUCCESS;
  }
   if(NULL == PmicSchgProtocol)
  {
    Status = gBS->LocateProtocol(&gQcomPmicSchgProtocolGuid, NULL, (VOID **)&PmicSchgProtocol);
    if(EFI_SUCCESS == Status && NULL != PmicSchgProtocol)
    {
      EFI_PM_SCHG_PMIC_INFO SchgPmicInfo = {0};
      Status = PmicSchgProtocol->SchgGetPmicInfo(&SchgPmicInfo);
      PmicDeviceIndex = SchgPmicInfo.PmicIndex;
    }
  } 

  return Status;
}


/*===========================================================================

                        EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
EFI_STATUS
EFIAPI
EFI_PmicUsbUsbinValid
(
  OUT BOOLEAN *pValid
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

    if(!pValid)
    {
      return EFI_INVALID_PARAMETER;
    }

    *pValid = FALSE;

    if(EFI_SUCCESS != EFIPmicUsbInit())
    {
      return EFI_UNSUPPORTED;
    }
    errFlag = pmapp_chg_usb_get_vbus_presence(pValid);
//  Status = PmicSchgProtocol->UsbinValid(PmicDeviceIndex, pValid);
    Status = (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
    
    return Status;
}

/**
EFI_PmicChargerPortType ()

@brief
Gets charger port type
*/
EFI_STATUS
EFIAPI
EFI_PmicUsbChargerPortType
(
  OUT EFI_PM_USB_CHGR_PORT_TYPE *PortType
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pmapp_chg_usb_apsd_result_status_type src_detected = PM_APP_CHG_INVALID;
    if(!PortType)
    {
      return EFI_INVALID_PARAMETER;
    }

    if(EFI_SUCCESS != EFIPmicUsbInit())
    {
      return EFI_UNSUPPORTED;
    }

      errFlag = pmapp_chg_usb_get_charger_type(&src_detected);
      switch(src_detected)
      {
        case PM_APP_CHG_SDP_CHARGER:
             *PortType = EFI_PM_USB_CHG_PORT_SDP;
             break;
        case PM_APP_CHG_OCP_CHARGER:
             *PortType = EFI_PM_USB_CHG_PORT_OTHER;
             break;
        case PM_APP_CHG_CDP_CHARGER:
             *PortType = EFI_PM_USB_CHG_PORT_CDP;
             break;
        case PM_APP_CHG_DCP_CHARGER:
             *PortType = EFI_PM_USB_CHG_PORT_DCP;
             break;
        case PM_APP_CHG_FLOAT_CHARGER:
             *PortType = EFI_PM_USB_CHG_PORT_FLOAT_CHARGE;
             break;
        case PM_APP_CHG_QC_2P0:
             *PortType = EFI_PM_USB_CHG_PORT_QC_2P0;
             break;
        case PM_APP_CHG_QC_3P0:
             *PortType = EFI_PM_USB_CHG_PORT_QC_3P0;
             break;
        case PM_APP_CHG_ICL_OVERRIDE_LATCH:
             *PortType = EFI_PM_USB_CHG_ICL_OVERRIDE_LATCH;
             break;
        case PM_APP_CHG_INVALID:
             *PortType = EFI_PM_USB_CHG_PORT_INVALID;
             break;
        default:
             break;
      }
//    Status = PmicSchgProtocol->GetChargerPortType(PmicDeviceIndex, (EFI_PM_SCHG_CHGR_PORT_TYPE*)PortType);
    Status = (PM_ERR_FLAG__SUCCESS == errFlag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;
    
    return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbSetOtgILimit
(
  IN  UINT32   ImAmp
)
{
    EFI_STATUS  Status = EFI_SUCCESS;

    if(EFI_SUCCESS != EFIPmicUsbInit())
    {
      return EFI_UNSUPPORTED;
    }

    Status = PmicSchgProtocol->SetOtgILimit(PmicDeviceIndex, ImAmp);

    return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbEnableOtg
(
  IN  BOOLEAN  Enable
)
{
    EFI_STATUS  Status = EFI_SUCCESS;

    if(EFI_SUCCESS != EFIPmicUsbInit())
    {
      return EFI_UNSUPPORTED;
    }

    Status = PmicSchgProtocol->EnableOtg(PmicDeviceIndex, Enable);

    return Status;
}


EFI_STATUS
EFIAPI
EFI_PmicUsbGetOtgStatus
(
   OUT EFI_PM_USB_OTG_STATUS_TYPE  *pOtgStatus
)
{
    EFI_STATUS  Status = EFI_SUCCESS;

    if(!pOtgStatus)
    {
      return EFI_INVALID_PARAMETER;
    }

    *pOtgStatus = EFI_PM_USB_OTG_STATUS_INVALID;

    if(EFI_SUCCESS != EFIPmicUsbInit())
    {
      return EFI_UNSUPPORTED;
    }

    Status = PmicSchgProtocol->GetOtgStatus(PmicDeviceIndex, (EFI_PM_SCHG_OTG_STATUS_TYPE *)pOtgStatus);

    return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbCGetPortState
(
  OUT  EFI_PM_USB_TYPEC_PORT_STATUS  *TypeCStatus
)
{
  EFI_STATUS       Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  if (!TypeCStatus)
  {
    return EFI_INVALID_PARAMETER;
  }

  SetMem(TypeCStatus, sizeof(EFI_PM_USB_TYPEC_PORT_STATUS), 0x00);

  err_flag = pm_schg_usb_get_typec_status(PmicDeviceIndex, (pm_schg_usb_typec_status_type *)TypeCStatus);
  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
    return EFI_DEVICE_ERROR;
  }

  return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbCGetConnectState
(
  OUT  EFI_PM_USB_TYPEC_CONNECT_MODE_STATUS  *pTypeCConnStatus
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  if (NULL == pTypeCConnStatus)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pTypeCConnStatus = EFI_PM_USB_TYPEC_CONNECT_MODE_INVALID;
  err_flag = pm_schg_usb_typec_get_port_role(PmicDeviceIndex, (pm_schg_usb_typec_port_role_type*)pTypeCConnStatus);
  Status = (PM_ERR_FLAG__SUCCESS == err_flag)? EFI_SUCCESS : EFI_DEVICE_ERROR;

  return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbCSetVConn
(
  IN  EFI_PM_USB_TYPEC_VCONN_CONFIG  VconnConfig
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  err_flag  = pm_schg_usb_typec_configure_vconn(PmicDeviceIndex ,
                                               (pm_schg_usb_typec_vconn_ilimit) VconnConfig.vconn_curr_lim, 
                                               (pm_schg_usb_typec_vconn_ss_pd_ima) VconnConfig.ss_pd_curr);
  err_flag |= pm_schg_usb_typec_vconn_enable(PmicDeviceIndex, (pm_schg_usb_typec_vconn_enable_type)VconnConfig.vconn_en_ctrl);
  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbCSetPortRole
(
  IN  EFI_PM_USB_TYPEC_PORT_ROLE  PortRole
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  if (PortRole >= EFI_PM_USB_TYPEC_ROLE_INVALID)
  {
    return EFI_INVALID_PARAMETER;
  }
  /*Disable try sink mode*/
  err_flag = pm_schg_usb_typec_enable_try_sink_mode(PmicDeviceIndex, FALSE);
  err_flag |= pm_schg_usb_typec_set_port_role(PmicDeviceIndex, (pm_schg_usb_typec_port_role_type)PortRole);
  
  gPortRoleSetTimeInmS = GetTimerCountms();

  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbCEnable
(
  IN  BOOLEAN  Enable
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  err_flag = pm_schg_usb_typec_disable(PmicDeviceIndex, (!Enable));
  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbGetChargerPresence
(
  OUT BOOLEAN  *pChargerPresent
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_schg_usb_connector_type  UsbConnectorType;
  EFI_PM_USB_TYPEC_PORT_STATUS TypeCStatus;

  if(NULL == pChargerPresent)
  {
    return EFI_INVALID_PARAMETER;
  }

  *pChargerPresent = FALSE;

  err_flag = pm_schg_usb_get_usb_connector_type(PmicDeviceIndex, &UsbConnectorType);
  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
    return EFI_DEVICE_ERROR;
  }

  Status = EFI_PmicUsbUsbinValid(pChargerPresent);
  
  if(EFI_SUCCESS != Status)
  {
    return Status;
  }
  
  if(UsbConnectorType == PM_SCHG_USB_CONNECTOR_TYPEC)
  {
    Status = EFI_PmicUsbCGetPortState( &TypeCStatus);
    
    if((TRUE == *pChargerPresent) && (FALSE == TypeCStatus.ccout_out_hiz_sts) && (EFI_SUCCESS == Status))
    {
      //Print Warning Message if Type-C CCOUT is Hi-Z (FALSE) but Vbus is valid
      PMIC_DEBUG((EFI_D_WARN, "PmicDxe:: %a WARN: Invalid CC Status. ccout_out_hiz_sts = %d \n\r", __FUNCTION__, TypeCStatus.ccout_out_hiz_sts));
      DEBUG((EFI_D_WARN, "PmicDxe:: %a WARN: Invalid CC Status. ccout_out_hiz_sts = %d \n\r", __FUNCTION__, TypeCStatus.ccout_out_hiz_sts));
    }
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
EFI_PmicUsbGetUSBConnectorType
(
  OUT EFI_PM_USB_CONNECTOR_TYPE  *pUSBConnectorType
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pmapp_chg_usb_connector_type usb_type= PM_APP_CHG_USB_CONNECTOR_INVALID;
  if(NULL == pUSBConnectorType)
  {
    return EFI_INVALID_PARAMETER;
  }
   err_flag = pmapp_chg_usb_get_usb_connector_type(&usb_type);
   switch(usb_type)
   {
     case PM_APP_CHG_USB_CONNECTOR_TYPEC:
          *pUSBConnectorType = EFI_PM_USB_CONNECTOR_TYPEC;
          break;
     case PM_APP_CHG_USB_CONNECTOR_MICRO_USB:
          *pUSBConnectorType = EFI_PM_USB_CONNECTOR_MICRO_USB;
          break;
     case PM_APP_CHG_USB_CONNECTOR_INVALID:
          *pUSBConnectorType = EFI_PM_USB_CONNECTOR_INVALID;
          break;
     default:
          break;
   }
  //err_flag = pm_schg_usb_get_usb_connector_type(PmicDeviceIndex,(pm_schg_usb_connector_type*)pUSBConnectorType);
    Status = (PM_ERR_FLAG__SUCCESS == err_flag) ? EFI_SUCCESS : EFI_DEVICE_ERROR;

    return Status;
}



EFI_STATUS
EFIAPI
EFI_PmicUsbSetUSBConnectorType
(
  IN EFI_PM_USB_CONNECTOR_TYPE  pUSBConnectorType
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  
  err_flag = pm_schg_usb_set_usb_connector_type(PmicDeviceIndex,(pm_schg_usb_connector_type)pUSBConnectorType);
  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
    return EFI_DEVICE_ERROR;
  }

  return Status;
}

EFI_STATUS
EFIAPI
EFI_PmicUsbGetPortRoleSetTime
(
  OUT UINT64  *pPortRoleSetTime
)
{
  if(!pPortRoleSetTime)
    return EFI_INVALID_PARAMETER;

  if(gPortRoleSetTimeInmS)
  {
    *pPortRoleSetTime = gPortRoleSetTimeInmS;
  }
  else 
  {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
PMIC USB UEFI Protocol implementation
*/
EFI_QCOM_PMIC_USB_PROTOCOL PmicUsbProtocolImplementation =
{
    PMIC_USB_REVISION,
    EFI_PmicUsbUsbinValid,
    EFI_PmicUsbChargerPortType,
    EFI_PmicUsbSetOtgILimit,
    EFI_PmicUsbEnableOtg,
    EFI_PmicUsbGetOtgStatus,
    EFI_PmicUsbCGetPortState,
    EFI_PmicUsbCGetConnectState,
    EFI_PmicUsbCSetVConn,
    EFI_PmicUsbCSetPortRole,
    EFI_PmicUsbCEnable,
    EFI_PmicUsbGetChargerPresence,
    EFI_PmicUsbGetUSBConnectorType,
    EFI_PmicUsbSetUSBConnectorType,
    EFI_PmicUsbGetPortRoleSetTime,	
};

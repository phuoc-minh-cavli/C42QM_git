/**
@file  UsbPwrCtrlLibPmSmb.c
@brief Implementation of UsbPWrCtrlLibrary API for PMIC SMB module
*/
/*=============================================================================
Copyright (c) 2018 Qualcomm Technologies, Incorporated.
All rights reserved.
Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                    EDIT HISTORY

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/21/18   pbitra      Initial Release
=============================================================================*/

/*===========================================================================*/
/*                  INCLUDE FILES                                            */
/*===========================================================================*/

#include <Uefi.h>

/**
  Library Dependencies
*/
#include <Library/QcomBaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
/**
  Protocol Dependencies
*/
/**
  Internal Dependencies
*/
#include "UsbPwrCtrlLibPmSmbchg.h"

#include "api/systemdrivers/pmic/pm_smb.h"

/*===========================================================================*/
/*                  TYPEDEF AND MACRO DEFINITIONS                            */
/*===========================================================================*/
#define USBPWRCTRL_SMBCHG_MAX_APSD_DONE_CHECK_ITERATION 5
#define USBPWRCTRL_SMBCHG_200_MS_WAIT					 200000 //200 milli Second wait

#define EFI_FAILURE                (80)

/*===========================================================================*/
/*                  FUNCTIONS PROTOTYPES                                     */
/*===========================================================================*/

/*===========================================================================*/
/*                  LOCAL VARIABLE DECLARATIONS                              */
/*===========================================================================*/


/**
  Initialize the interface to PMIC SMBCHG Module

  @param none

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSmbchg_Init(void)
{
	EFI_STATUS Status = EFI_SUCCESS;

	if ( PM_ERR_FLAG__SUCCESS != pm_smb_init())
	{
	  Status =	EFI_FAILURE;
	}

    USBPWRCTRL_LIB_DEBUG((EFI_D_WARN, "%a  SMB Charger Init Status: %d \r\n", __FUNCTION__, Status));

    return Status;
}



/**
  Get Vbus Detect result from PMIC SMBCHG Module

  @param[IN]    HwInfo          Hardware Information for this request
  @param[OUT]   bVbusDetect     Return the result of Vbus Detect

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSmbchg_GetVbusDetect(
  BOOLEAN *bVbusDetect)
{
  EFI_STATUS Status = EFI_SUCCESS;
  if (bVbusDetect == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if ( PM_ERR_FLAG__SUCCESS != pm_smb_get_usbin_status(bVbusDetect))
  {
  	Status =  EFI_FAILURE;
  }

  return Status;
}


/**
  Get HSUSB Charger Port Type

  @param[IN]    HwInfo          Hardware Information for this request
  @param[OUT]   ChgPortType     Return Charger Port Tpye

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSmbchg_GetHSUsbPortType(
  EFI_USB_PWR_CTRL_HSUSB_CHGPORT_TYPE *ChgPortType)
{

  pm_err_flag_type	errFlag 	  = PM_ERR_FLAG__SUCCESS;  
  BOOLEAN			 ApsdDoneStatus = FALSE;  
  UINT32			ApsdDoneCheckIteration = 0;  

  if(NULL == ChgPortType)  
  {	
    return EFI_INVALID_PARAMETER;  
  }
  *ChgPortType = EFI_USB_PWR_CTRL_HSUSB_CHGPORT_INVALID;  
  do  
  {	
    errFlag = pm_smb_get_apsd_status(&ApsdDoneStatus);	  
	if(PM_ERR_FLAG__SUCCESS != errFlag)	
	{
		break;	 
	}	  

	if(TRUE == ApsdDoneStatus)	
	{	   
		errFlag =  pm_smb_get_apsd_result_status((pm_smb_apsd_result_status_type*) ChgPortType);	  
		if(PM_ERR_FLAG__SUCCESS != errFlag)	   
		{
			break;		
		}	 
	}
	else
	{ 	 
		/*if APSD result is not available yet, read again after 200 milli second.*/	  
		gBS->Stall(USBPWRCTRL_SMBCHG_200_MS_WAIT);	 
	}	  
	ApsdDoneCheckIteration ++;  
  }while ((FALSE == ApsdDoneStatus) && (ApsdDoneCheckIteration < USBPWRCTRL_SMBCHG_MAX_APSD_DONE_CHECK_ITERATION));  

  if(PM_ERR_FLAG__SUCCESS != errFlag)	
  {	 
  	*ChgPortType = EFI_USB_PWR_CTRL_HSUSB_CHGPORT_INVALID;	
	return EFI_DEVICE_ERROR;  
  }

  return EFI_SUCCESS;
  
}


/**
  Set Sink Max Input Current

  @param[IN]    HwInfo          Hardware Information for this request
  @param[IN]    MaxCurrentMA    Value to set Max Input Current

  @return
  EFI_SUCCESS:                  Function returned successfully.
  EFI_INVALID_PARAMETER:        A Parameter was incorrect.
  EFI_DEVICE_ERROR:             The physical device reported an error.
  EFI_UNSUPPORTED:              The physical device does not support this function
*/
EFI_STATUS
UsbPwrCtrlLibPmSmbchg_SetSnkMaxInputCurrent(
  UINT32 MaxCurrentMA)
{
  EFI_STATUS Status = EFI_SUCCESS;
  if (PM_ERR_FLAG__SUCCESS != pm_smb_set_usbin_current_limit_cfg(MaxCurrentMA))
  {
  	Status =  EFI_FAILURE;
  }

  return Status;
}


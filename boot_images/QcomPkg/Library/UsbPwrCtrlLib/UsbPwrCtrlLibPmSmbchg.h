/**
@file  UsbPwrCtrlLibPmSmbchg.h
@brief Provide UsbPwrCtrlLibPmSmbchg Related definition
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
#ifndef __USBPWRCTRLLIBPMSMBCHG_H__
#define __USBPWRCTRLLIBPMSMBCHG_H__

/*===========================================================================*/
/*                  INCLUDE FILES                                            */
/*===========================================================================*/
/**
  Library Dependencies
*/
#include <Library/UsbPwrCtrlLib.h>

/**
  Protocol Dependencies
*/
#include <Protocol/EFIPmicUsb.h>
#include <Protocol/EFIPmicSmbchg.h>

/**
  Internal Dependencies
*/
#include "UsbPwrCtrlLibConfig.h"


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
UsbPwrCtrlLibPmSmbchg_Init();

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
  BOOLEAN *bVbusDetect);

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
  EFI_USB_PWR_CTRL_HSUSB_CHGPORT_TYPE *ChgPortType);

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
  UINT32 MaxCurrentMA);

#endif

/*! \file pm_usb_app.c
*  
*  \brief Implementation file for PON APP level APIs.
*    
*  &copy; Copyright 2019 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===================================================================
EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Date: 2019/12/27 $ 
$Change: 21991403 $
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/14/19   ks     Added file for Tiny Modem image in Twizy
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pon.h"
#include "pm_app_pon.h"
#include "device_info.h"
#include "pm_comm.h"
#include "pm_device.h"
#include "pmapp_usb.h"
#include "products_utility.h" 
/*===============================================================================
                            DEFINITIONS
/*=============================================================================== */
extern boolean bPMICStub;
static pmapp_usb_isr_data        isr_data;
/*===========================================================================
                       Function  Prototype
===========================================================================*/
static void* pmapp_usb_spmi_isr(void* me, uint32 intrMask); 		/* SPMI ISR */


/*===========================================================================
                   API Implementation
===========================================================================*/


pm_err_flag_type pmapp_usb_vbus_status_callback(pmapp_usb_vbus_det_fn_cb pmapp_usb_cb)
{
  static uint8                     run_once;
  static DalDeviceHandle          *dalIntHandle;
  boolean status = FALSE;
  DALResult                        result   = DAL_SUCCESS;
  pm_err_flag_type                 err_flag = PM_ERR_FLAG__SUCCESS;
  
  if (bPMICStub == TRUE)
  {
      return err_flag;
  }
  if( NULL == pmapp_usb_cb )
  {
    return  PM_ERR_FLAG__INVALID_POINTER;
  }
  if( MODEM_BOOT_TINY_NO_APSS != get_modem_boot_mode())
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED; 
  }
  
  if(run_once == FALSE)
  {
    
    //Get DAL handle
    result = DAL_StringDeviceAttach("DALDEVICEID_SPMI_DEVICE", &dalIntHandle);

    if( (result == DAL_SUCCESS) && (NULL != dalIntHandle) )
    {
        result = DalDevice_Open(dalIntHandle, DAL_OPEN_SHARED);
        if(result == DAL_SUCCESS)
        {
            isr_data.pmapp_usb_cb = pmapp_usb_cb;
            //Configure ISR for VBUS_USB on GPIO-1
            result = DalSpmi_RegisterIsr(dalIntHandle, 0xC0, 0xFF, pmapp_usb_spmi_isr, &isr_data);
        }
    }
    if(result != DAL_SUCCESS)
    {
        err_flag = PM_ERR_FLAG__DAL_SERVICE_REGISTRATION_FAILED;
    }
    err_flag |= pm_comm_read_byte_mask(0, 0xC018, 0x01, &status,0); 
    if((PM_ERR_FLAG__SUCCESS == err_flag) && (TRUE == status))
    {
        //disabling the GPIO-1 INT instead of clearing as we couldn't clear the SMB INT's due to unavailability of i2c.
        pm_comm_write_byte(0, 0xC016, 0x01, 0);
        pmapp_usb_cb(); //fire the CB even for GPIO-1 latched INT 
 
    }
    
    run_once = TRUE;
  }
	
 return err_flag;
}

static void* pmapp_usb_spmi_isr(void* me, uint32 intrMask)
{
  pmapp_usb_isr_data* isr_data   = (pmapp_usb_isr_data*)me;
  
  if( NULL == isr_data )
  {
    return NULL; // don't call CB
  }
  //disabling the GPIO-1 INT instead of clearing as we couldn't clear the SMB INT's due to unavailability of i2c
  //in general flow should be like "clear smb int's and then PMIC GPIO-1 INT's 
  pm_comm_write_byte(0, 0xC016, 0x01, 0); 
  isr_data->pmapp_usb_cb();

  return NULL;
}

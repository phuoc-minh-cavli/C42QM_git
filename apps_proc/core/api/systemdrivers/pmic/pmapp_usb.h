#ifndef __PMAPP_USB_H__
#define __PMAPP_USB_H__
/*===========================================================================


               P M A P P_ V B US  H E A D E R    F I L E

\details
    This file contains functions prototypes and variable/type/constant 
  declarations for the PMIC USB VBUS application
  
Copyright (c) 2013            by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pmapp_usb.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/14/13    rh      Created file
===========================================================================*/
#include "comdef.h"
#include "pm_err_flags.h"
#include "DDISpmi.h"
#include "pm_version.h"

/*===========================================================================

                PM APP USB TYPE DEFINITIONS

===========================================================================*/

typedef void (*pmapp_usb_vbus_det_fn_cb) (boolean is_high);

typedef struct pmapp_usb_isr_data
{
    pmapp_usb_vbus_det_fn_cb pmapp_usb_cb;
    pm_device_info_type pmic_device_info;
}pmapp_usb_isr_data;

/*===========================================================================

                PM APP USB FUNCTION PROTOTYPES

===========================================================================*/

/*=========================================================================== */
/*                     pmapp_usb_vbus_status_callback                         */
/*=========================================================================== */

/**
  This function registers a USB VBUS callback which will get called on rising
  and falling edges of the USB VBUS line

  @return      pm_err_flag_type - PMIC error flag status 

*/
pm_err_flag_type pmapp_usb_vbus_status_callback(pmapp_usb_vbus_det_fn_cb pmapp_usb_cb);

#endif /* __PMAPP_UICC_H__ */

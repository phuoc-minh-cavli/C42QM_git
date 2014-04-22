#ifndef _QAPI_USB_INTERNAL_H_
#define _QAPI_USB_INTERNAL_H_

/*==============================================================================

    Internal file for QAPI SER3 interface support
                  H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    Contains function declarations needed for internal use only to support
    QAPI SER3 interface
  
  EXTERNALIZED FUNCTIONS
    hsu_al_qapi_app_rx_cb_func
    hsu_al_qapi_register

  INITALIZATION AND SEQUENCING REQUIREMENTS
    
  Copyright (c) 2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/qapi_usb_internal.h#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  02/22/17  ak  Initial version
    
==============================================================================*/

#include "dsm.h"

/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
/* Takes the dsm item from HW and then calls the client rx cb func
   notifying that data is available                               */
void hsu_al_qapi_app_rx_cb_func (dsm_item_type ** rx_ptr);


#endif /* _HSU_CDC_FD_API_H_ */


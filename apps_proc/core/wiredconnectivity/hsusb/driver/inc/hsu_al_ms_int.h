#ifndef _HSU_AL_MS_INT_H_
#define _HSU_AL_MS_INT_H_

/*==============================================================================

                High Speed USB Mass Storage Adaptation Layer
  
  GENERAL DESCRIPTION
    Defines externalized data and functions for the mass storage adaptation layer.
  
  EXTERNALIZED FUNCTIONS
    hsu_al_ms_get_num_devices
    hsu_al_ms_task
    
  Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
    $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_al_ms_int.h#1 $
    
    when     who  what, where, why
    -------- ---  ---------------------------------------------------------
    
==============================================================================*/

#include "hsu_al_ms.h"
#include "customer.h"

#ifdef HS_USB_SCSI_BUFFER_SIZE
#define HSU_SCSI_BUFFER_SIZE HS_USB_SCSI_BUFFER_SIZE
#else
#define HSU_SCSI_BUFFER_SIZE (32*1024)
#endif /* HS_USB_SCSI_BUFFER_SIZE */

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

#endif /* _HSU_AL_MS_INT_H_ */

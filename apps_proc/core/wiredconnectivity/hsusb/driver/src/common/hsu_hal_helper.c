/*
===========================================================================

FILE:         hsu_hal_helper.c

DESCRIPTION:  
This is the USB cores' base addresses of all MSM targets for HAL.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_hal_helper.c#1 $

===========================================================================

===========================================================================
Copyright © 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "hsu_hwio_dependency.h"
#include "customer.h"
#include "hsu_common_int.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */
/*
* HS-USB Cores' base addresses.
*/

uint32 usb_core_base[] = 
{
  0x01A00000,
};


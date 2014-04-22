/*
===========================================================================

FILE:         HAL_hsusb_target.c

DESCRIPTION:  
This is the HAL implementation for the MDM 9x06.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/hal/src/9205/HAL_hsusb_target.c#1 $

==========================================================================

===========================================================================
Copyright © 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HAL_hsusb_MDM9x06.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/*
* HS-USB HAL Constructor.
*/
void HAL_hsusb_ConstructImplInstance
(
  HAL_HSUSB_CoresType* this
)
{
  HAL_hsusb_ConstructImplInstanceMDM9x06(this);
}

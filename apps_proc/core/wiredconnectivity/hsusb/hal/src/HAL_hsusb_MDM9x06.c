/*
===========================================================================

FILE:         HAL_hsusb_MDM9x06.c

DESCRIPTION:  
This is the HAL implementation of the run-time initialization for MDM9x06.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/hal/src/HAL_hsusb_MDM9x06.c#1 $

===========================================================================

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
#include "HAL_hsusb_impl_p3q3r50.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/*
* HS-USB HAL Constructor.
*/
void HAL_hsusb_ConstructImplInstanceMDM9x06
(
  HAL_HSUSB_CoresType* this
)
{
  uint8 instance;

  HAL_hsusb_ConstructImplp3q3r50(&this->CoreIf[0]);
  this->MaxCoresQty = 1;
  /* USB Core 0 */
  instance = 0;
  this->CoreInfo[instance].If = &this->CoreIf[0];
  this->CoreInfo[instance].PortsQty = 1;
  this->CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_ULPI;
  this->CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
}

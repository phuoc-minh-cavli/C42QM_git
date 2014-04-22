#ifndef HAL_HSUSB_MDM9x06
#define HAL_HSUSB_MDM9x06
/*
===========================================================================

FILE:         HAL_hsusb_MDM9x06.h

DESCRIPTION:  
This is the external interface of the run-time initialization for MDM9x07.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/hal/src/HAL_hsusb_MDM9x06.h#1 $

===========================================================================

===========================================================================
Copyright © 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INTERFACES
** ----------------------------------------------------------------------- */

#include "HAL_hsusb_impl.h"

/* -----------------------------------------------------------------------
**                           INTERFACES DESCRIPTION
** ----------------------------------------------------------------------- */

/* 
* C++ wrapper
*/
#ifdef __cplusplus
extern "C" {
#endif

void HAL_hsusb_ConstructImplInstanceMDM9x06(
  HAL_HSUSB_CoresType* 
);

#ifdef __cplusplus
}
#endif

#endif /* HAL_HSUSB_MDM9x06 */

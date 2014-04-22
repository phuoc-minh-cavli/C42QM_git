#ifndef HAL_SSUSB_IMPL
#define HAL_SSUSB_IMPL
/*
===========================================================================

FILE:         HAL_ssusb_impl.h

DESCRIPTION:  
This is the HAL internal interface for the SS-USB core.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/hal/src/HAL_ssusb_impl.h#1 $

===========================================================================

===========================================================================
Copyright © 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HAL_ssusb.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
* HAL_SSUSB_CoreInfoType
*/
typedef struct
{
  HAL_SSUSB_BusParamsType BusParams;
  HAL_SSUSB_ErrorType     LastError;
} HAL_SSUSB_CoreInfoType;

/*
* HAL_SSUSB_CoresType
*/
typedef struct
{
  HAL_SSUSB_CoreInfoType* CoreInfo;
  uint8                   MaxCoresQty;
  boolean                 Initialized;
} HAL_SSUSB_CoresType;

#endif /* HAL_SSUSB_IMPL */

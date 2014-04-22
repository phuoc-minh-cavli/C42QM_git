/*==============================================================================

  D A L   G P I O   I N T E R R U P T   C O N T R O L L E R

DESCRIPTION
  This file has the direct connect GPIO interrupt support.

REFERENCES

        Copyright (c) 2018 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/GPIOInt/config/mdm9205/GPIOInt_ConfigData.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/18   shm     First version for 9205
===========================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"
#include "DDIGPIOInt.h"
#include "HALgpioint.h"
#include "GPIOInt.h"

/*
 * Modem interrupt config map
 */

GPIOIntConfigMapType interrupt_config_map[] =
{
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_0, 71, 0}, /* MSS direct connect 1 */
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_1, 72, 0}, /* MSS direct connect 2 */
};

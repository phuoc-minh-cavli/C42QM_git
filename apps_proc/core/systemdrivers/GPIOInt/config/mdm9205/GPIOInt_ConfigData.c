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

$Header: //components/rel/core.tx/6.0/systemdrivers/GPIOInt/config/mdm9205/GPIOInt_ConfigData.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/18   shm     First version for 9205 codebase
===========================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"
#include "DDIGPIOInt.h"
#include "HALgpioint.h"
#include "GPIOInt.h"

/*
 * Interrupt config map
 */

GPIOIntConfigMapType interrupt_config_map[] =
{
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_0, 248},   /* Direct connect 0 */
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_1, 247},   /* Direct connect 1 */
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_2, 246},   /* Direct connect 2 */
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_3, 245},   /* Direct connect 3 */
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_4, 244},   /* Direct connect 4 */
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_5, 243},   /* Direct connect 5 */
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_6, 242},   /* Direct connect 6 */
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_7, 241},   /* Direct connect 7 */
};

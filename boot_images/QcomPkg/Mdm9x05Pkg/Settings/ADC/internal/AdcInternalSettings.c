/*============================================================================
  FILE:         AdcInternalSettings.c

  OVERVIEW:     Internal settings for ADC.

  DEPENDENCIES: None

                Copyright (c) 2014-2016,2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-04-24  KS   Added changes for Vipertooth.
  2016-05-05  jjo  Add FG ADC.
  2015-05-05  jjo  Use designated initializers.
  2014-04-02  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AdcBsp.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof((a)[0]))

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*
 * MDM9205_PME9205
 */
 static const AdcPhysicalDeviceType adcPhysicalDevices[] = 
 {
	 /* pme9205 */
	{
      .pszDevName = "/vadc/pme9205",
    },
 };


const AdcBspType AdcBsp[] =
{
   {
      .paAdcPhysicalDevices = adcPhysicalDevices,
      .uNumDevices          = ARRAY_LENGTH(adcPhysicalDevices)
   }
};






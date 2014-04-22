/*============================================================================
  FILE:         AdcBsp.c

  OVERVIEW:     Board support package for the ADC DAL.

  DEPENDENCIES: None

                Copyright (c) 2012, 2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.tx/6.0/hwengines/adc/config/9205/AdcBsp.c#1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-01-08  jjo  Add queue config.
  2012-06-13  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalAdc.h"

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
enum
{
   ADC_QUEUE_VADC,
   ADC_NUM_QUEUES
};

static const AdcQueueConfigType aAdcQueueConfigs[ADC_NUM_QUEUES] =
{
   /* ADC_QUEUE_VADC */
   {
      .pszWorkLoopName = "ADC_WORK_LOOP",
      .uQueueSize      = 10
   }
};

static const AdcPhysicalDeviceType adcPhysicalDevices[] =
{
   /* VADC */
   {
      .pszDevName = "DALDEVICEID_VADC",
      .uQueueIdx  = ADC_QUEUE_VADC
   },
};

const AdcBspType AdcBsp[] =
{
   {
      .paAdcPhysicalDevices = adcPhysicalDevices,
      .uNumPhysicalDevices  = ARRAY_LENGTH(adcPhysicalDevices),
      .paAdcQueueConfigs    = aAdcQueueConfigs,
      .uNumQueues           = ARRAY_LENGTH(aAdcQueueConfigs)
   }
};


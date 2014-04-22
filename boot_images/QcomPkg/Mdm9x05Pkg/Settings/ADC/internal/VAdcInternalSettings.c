/*============================================================================
  FILE:         VAdcInternalSettings.c

  OVERVIEW:     Internal settings for VADC.

  DEPENDENCIES: None

                Copyright (c) 2016, 2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Settings/ADC/internal/VAdcInternalSettings.c#2 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-05-10  KS   Updated for Vipertooth.
  2016-10-07  SA   Updated for SDM660.
  2016-08-12  jjo  Split BSP.
  2016-03-04  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalVAdc.h"
#include "AdcInputs.h"

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
extern const VAdcChannelConfigType gVAdcChannels[];
extern const uint32 uNumVAdcChannels;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
/*
 * This table is a mapping of ADC channels to GPIO inputs.
 */
static const uint8 aucGpio1Channels[]  = {0x12, 0x32, 0x52, 0x72, 0x92};
static const uint8 aucGpio2Channels[]  = {0x13, 0x33, 0x53, 0x73, 0x93};
static const uint8 aucGpio3Channels[] = {0x14, 0x34, 0x54, 0x74, 0x94};
static const uint8 aucGpio4Channels[] = {0x15, 0x35, 0x55, 0x75, 0x95};

static const VAdcGpioChannelMappingType vAdcGpioChannelMappings[] =
{
   {
      .paucChannels = aucGpio1Channels,
      .uNumChannels = ARRAY_LENGTH(aucGpio1Channels),
      .eGpio        = PM_GPIO_1,
   },

   {
      .paucChannels = aucGpio2Channels,
      .uNumChannels = ARRAY_LENGTH(aucGpio2Channels),
      .eGpio        = PM_GPIO_2,
   },

   {
      .paucChannels = aucGpio3Channels,
      .uNumChannels = ARRAY_LENGTH(aucGpio3Channels),
      .eGpio        = PM_GPIO_3,
   },

   {
      .paucChannels = aucGpio4Channels,
      .uNumChannels = ARRAY_LENGTH(aucGpio4Channels),
      .eGpio        = PM_GPIO_4,
   },
   
};

const VAdcBspType VAdcBsp[] =
{
   {
      .paChannels              = gVAdcChannels,
      .puNumChannels           = &uNumVAdcChannels,
      .paAveragedChannels      = NULL,
      .puNumAveragedChannels   = NULL,
      .paGpioChannelMappings   = vAdcGpioChannelMappings,
      .uNumGpioChannelMappings = ARRAY_LENGTH(vAdcGpioChannelMappings),
      .bUsesInterrupts         = FALSE,
      .uFullScale_code         = 0x70E4,
      .uFullScale_uV           = 1875000,
      .uReadTimeout_us         = 500000,
      .uLDOSettlingTime_us     = 17,
      .uSlaveId                = 0,
      .uPeripheralId           = 0x31,
      .uMasterID               = 0,
      .eAccessPriority         = SPMI_BUS_ACCESS_PRIORITY_LOW,
      .ucPmicDevice            = 0,
   }
};


/*============================================================================
  FILE:         TsensBsp.c

  OVERVIEW:     Tsens bsp file

  DEPENDENCIES: None
 
                Copyright (c) 2014-2015, 2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.tx/6.0/hwengines/tsens/config/9205/TsensBsp.c#1 $$DateTime: 2020/01/30 22:49:35 $$Author: pwbldsvc $
 
  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-12-14  AY   Updated default slope and ADC code at 30C for 9205
  2015-07-17  SA   Created for 9x07 (from 8909).

  Previous history:
  -----------------
  2014-10-17  SA   Updated default slope to 3.0 based on PTE recommendation.
  2014-07-07  SA   Created (from 8916) and updated for 8909.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
//#include "DALFramework.h"
#include "TsensBsp.h"
#include "TsensiConversion.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define TSENS_NUM_SENSORS                    5
#define TSENS_SENSOR_ENABLE_MASK          0x1F
#define TSENS_TURBO_MODE                  TRUE
#define TSENS_SENSOR_CONV_TIME_US          150
#define TSENS_INTERRUPT_ID                 216

#define TSENS_Y1                            30
#define TSENS_Y2                           120

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
 /* Default TSENS ADC codes at 30 deg C */
const static int32 anX1_default[TSENS_NUM_SENSORS] = 
{
   480,
   480,
   480,
   480,
   480
};

/* Default slope: factor * median slope [C/code] */
const static int32 anM_default[TSENS_NUM_SENSORS] = 
{
   (int32)(TSENS_FACTOR * (1 / 2.6)),
   (int32)(TSENS_FACTOR * (1 / 2.6)),
   (int32)(TSENS_FACTOR * (1 / 2.6)),
   (int32)(TSENS_FACTOR * (1 / 2.6)),
   (int32)(TSENS_FACTOR * (1 / 2.6))
};

const TsensBspType TsensBsp[] = {
   {
      /* .uNumSensors         */ TSENS_NUM_SENSORS,
      /* .uSensorEnableMask   */ TSENS_SENSOR_ENABLE_MASK,
      /* .uInterruptId        */ TSENS_INTERRUPT_ID,
      /* .uSensorConvTime_us  */ TSENS_SENSOR_CONV_TIME_US,
      /* .panX1_default       */ anX1_default,
      /* .panM_default        */ anM_default,
      /* .nY1                 */ TSENS_Y1,
      /* .nY2                 */ TSENS_Y2
   }
};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/ 

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/


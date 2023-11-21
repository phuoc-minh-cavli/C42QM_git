/*============================================================================
  FILE:         TsensBsp.c

  OVERVIEW:     Tsens bsp file

  DEPENDENCIES: None
 
                Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.10/hwengines/tsens/config/9607/TsensBsp.c#1 $$DateTime: 2018/06/26 03:18:17 $$Author: pwbldsvc $
 
  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-07-17  SA   Created for 9x07 (from 8909).

  Previous history:
  -----------------
  2014-10-17  SA   Updated default slope to 3.0 based on PTE recommendation.
  2014-07-07  SA   Created (from 8916) and updated for 8909.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALFramework.h"
#include "TsensBsp.h"
#include "TsensiConversion.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define TSENS_NUM_SENSORS                    5
#define TSENS_SENSOR_ENABLE_MASK          0x1F
#define TSENS_TURBO_MODE                  TRUE
#define TSENS_SENSOR_CONV_TIME_US          150

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
   500,
   500,
   500,
   500,
   500
};

/* Default slope: factor * median slope [C/code] */
const static int32 anM_default[TSENS_NUM_SENSORS] = 
{
   (int32)(TSENS_FACTOR * (1 / 3.0)),
   (int32)(TSENS_FACTOR * (1 / 3.0)),
   (int32)(TSENS_FACTOR * (1 / 3.0)),
   (int32)(TSENS_FACTOR * (1 / 3.0)),
   (int32)(TSENS_FACTOR * (1 / 3.0))
};

const TsensBspType TsensBsp[] = {
   {
      /* .uNumSensors         */ TSENS_NUM_SENSORS,
      /* .uSensorEnableMask   */ TSENS_SENSOR_ENABLE_MASK,
      /* .bTurboMode          */ TSENS_TURBO_MODE,
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


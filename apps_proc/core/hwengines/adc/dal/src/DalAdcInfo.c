/*============================================================================
  FILE:         DALAdcInfo.c

  OVERVIEW:     Implementation of the ADC DAL

  DEPENDENCIES: None

                Copyright (c) 2009-2010,2012, 2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.tx/6.0/hwengines/adc/dal/src/DalAdcInfo.c#1 $$DateTime: 2020/01/30 22:49:35 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-01-14  jjo  Cleanup.
  2012-04-19  jdt  Updated for 8974.
  2009-03-16  jdt  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALReg.h"
#include "DALDeviceId.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult
Adc_DalAdc_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
static DALDEVICEID DalAdc_DeviceId[1] = {DALDEVICEID_ADC};

const DALREG_DriverInfo DALAdc_DriverInfo =
{
   Adc_DalAdc_Attach,
   1,
   DalAdc_DeviceId
};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/


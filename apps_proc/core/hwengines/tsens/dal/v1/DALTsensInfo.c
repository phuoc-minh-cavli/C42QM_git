/*============================================================================
  FILE:         DALTsensInfo.c

  OVERVIEW:     Implementation of the Tsens DAL Info

  DEPENDENCIES: None

                Copyright (c) 2010, 2014-2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.tx/6.0/hwengines/tsens/dal/v1/DALTsensInfo.c#1 $$DateTime: 2020/01/30 22:49:35 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-01-29  jjo  Added comments.
  2014-07-02  jjo  File cleanup.

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
Tsens_DalTsens_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
static DALDEVICEID DalTsens_DeviceId[1] = {DALDEVICEID_TSENS};

const DALREG_DriverInfo DALTsens_DriverInfo =
{
   Tsens_DalTsens_Attach,
   1,
   DalTsens_DeviceId
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


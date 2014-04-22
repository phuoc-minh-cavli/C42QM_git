#ifndef __TSENSI_CONVERSION_H__
#define __TSENSI_CONVERSION_H__
/*============================================================================
  @file TsensiConversion.h

  TSENS internal API for temperature conversions.

                Copyright (c) 2012, 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.tx/6.0/hwengines/tsens/dal/v1/TsensiConversion.h#1 $ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define TSENS_FACTOR (0x8000)    /* integer factor used in decimal calculations */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALStdDef.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
void Tsensi_CalibrationInit(uint32 uNumTsensSensors, const int32 *panX1_default, const int32 *panM_default, int32 nY1, int32 nY2);
DALResult Tsensi_ConvertToDegC(uint32 uSensor, uint32 uAdcCode, int32 *pnTempDegC);
DALResult Tsensi_ConvertFromDegC(uint32 uSensor, int32 nTempDegC, uint32 *puAdcCode);
DALBOOL Tsensi_IsCalibrated(uint32 uSensor);

#endif

#ifndef TSENSI_CONVERSION_H
#define TSENSI_CONVERSION_H
/*============================================================================
  @file TsensiConversion.h

  TSENS internal API for temperature conversions.

                Copyright (c) 2012, 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/* $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/TsensLib/TsensiConversion.h#1 $ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define TSENS_FACTOR (0x8000)    /* integer factor used in decimal calculations */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALStdDef.h"
#include "TsensLib.h"
#include "TsensBsp.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
void Tsensi_CalibrationInit(const TsensSensorCfgType *paSensors, uint32 uNumSensors, int32 nY1, int32 nY2);
TsensResultType Tsensi_ConvertToDegC(uint32 uSensor, uint32 uAdcCode, int32 *pnTempDegC);
TsensResultType Tsensi_ConvertFromDegC(uint32 uSensor, int32 nTempDegC, uint32 *puAdcCode);

#endif

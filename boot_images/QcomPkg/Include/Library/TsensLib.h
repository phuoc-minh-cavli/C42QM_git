#ifndef __TSENS_LIB_H__
#define __TSENS_LIB_H__
/*============================================================================
  @file TsensLib.h

  MSM on-die temperature sensor library API.

  This file contains data structures and functions used to configure, control,
  and query temperature sensors.

               Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALStdDef.h"

#define TSENS_MAX_NUM_SENSORS 16
#define TSENS_MAX_NUM_CPU_SENSORS 8

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   TSENS_SUCCESS,
   TSENS_ERROR_INVALID_CALIBRATION,
   TSENS_ERROR_TIMEOUT,
   TSENS_ERROR_NOT_INITIALIZED,
   TSENS_ERROR,
   TSENS_ERROR_SENSOR_NOT_AVAILABLE,
} TsensResultType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
   Initializes TSENS in the boot:
    - Enables critical thresholds
    - Enables all the sensors

   @return TSENS_SUCCESS, else an error.
 */
TsensResultType Tsens_Init(void);

/**
   Returns the temperature for the given sensor. The number of sensors
   can be obtained by calling Tsens_GetNumSensors.

   @param uSensor: [in] sensor index (zero-based).
   @param pnDegC: [out] sensor temperature in degrees C.

   @see Tsens_Init

   @return TSENS_SUCCESS, else an error.
 */
TsensResultType Tsens_GetTemp(uint32 uSensor, int32 *pnDegC);

/**
   Returns the number of sensors.

   @param puNumSensors: [out] number of sensors .

   @see Tsens_Init

   @return TSENS_SUCCESS, else an error.
 */
TsensResultType Tsens_GetNumSensors(uint32 *puNumSensors);

TsensResultType Tsens_GetMaxTemp(int32 *pnMaxDeciDegC);

#endif /* #ifndef __TSENS_LIB_H__ */


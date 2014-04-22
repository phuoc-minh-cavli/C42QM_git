#ifndef __DDITSENS_H__
#define __DDITSENS_H__

/**
  @file DDITsens.h

  Temperature Sensor (TSens) Device Access Library (DAL) device driver interface.

  This file contains data structures and functions used to configure, control,
  and query temperature sensors.
*/

/*============================================================================
               Copyright (c) 2010-2015 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/core.tx/6.0/api/hwengines/DDITsens.h#2 $ */

#include "DalDevice.h"
#include "DALSys.h"
#include "DALDeviceId.h"

#define DALTSENS_INTERFACE_VERSION DALINTERFACE_VERSION(3,0)

enum
{
   TSENS_ERROR_INVALID_CALIBRATION = 1,
   TSENS_ERROR_TIMEOUT,
   TSENS_ERROR_INVALID_PARAMETER,
   TSENS_ERROR_UNSUPPORTED,
   TSENS_ERROR_DEAD_SENSOR
};

/*!
 * Threshold type - used when setting thresholds
 */
typedef enum
{
   TSENS_THRESHOLD_LOWER = 0,
   TSENS_THRESHOLD_UPPER,
   _TSENS_NUM_THRESHOLDS
} TsensThresholdType;

#define TSENS_INVALID_THRESHOLD (0x7fffffff)

/** @addtogroup c_structs
@{ */

typedef struct
{
   int32 nMinDegC;  /**< Minimum threshold temperature in degrees Celsius */
   int32 nMaxDegC;  /**< Maximum threshold temperature in degrees Celsius */
} TsensTempRangeType;

typedef struct
{
   int32 nDegC;      /**< Temperature in degrees Celsius */
} TsensTempType;

typedef struct
{
   TsensThresholdType eThreshold;  /**< Type of threshold that triggered */
   uint32 uSensor;                 /**< Sensor that triggered */
   int32 nTriggeredDegC;           /**< Temperature value that triggered */
} TsensCallbackPayloadType;
/** @} */ /* end_addtogroup c_structs */

typedef void (*TsensThresholdCbType)(void *pCtxt, const TsensCallbackPayloadType *pPayload);

/**@cond
*/
typedef struct DalTsens DalTsens;
struct DalTsens
{
   DalDevice DalDevice;
   DALResult (*GetTemp)(DalDeviceHandle *, uint32 uSensor, TsensTempType *);
   DALResult (*GetTempRange)(DalDeviceHandle *, uint32 uSensor, TsensTempRangeType *);
   DALResult (*SetThreshold)(DalDeviceHandle *, uint32 uSensor, TsensThresholdType, int32, TsensThresholdCbType pfnCallback, void *pCtxt);
   DALResult (*SetEnableThresholds)(DalDeviceHandle *, DALBOOL);
   DALResult (*GetNumSensors)(DalDeviceHandle *, uint32 *);
   DALResult (*CheckCalibration)(DalDeviceHandle *, uint32 uSensor);
};

typedef struct DalTsensHandle DalTsensHandle;
struct DalTsensHandle
{
   uint32 dwDalHandleId;
   const DalTsens *pVtbl;
   void *pClientCtxt;
};
/** @endcond
*/

/** @addtogroup api_funcs
@{ */

/**
  Attaches the client to the TSens driver.

  This define is used to obtain the device handle that is required to use the TSens
  driver APIs.

  @param[in]  DevId        Device ID (e.g., DALDEVICEID_TSENS).
  @param[out] hDalDevice   A double pointer to the DAL device handle.

  @return
  DAL_SUCCESS -- Attach was successful.\n
  DAL_ERROR -- An error occurred when attaching to the device.
*/
#define DAL_TsensDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALTSENS_INTERFACE_VERSION,hDalDevice)

/**
  Detaches the client to the TSens driver.

  This define is used by clients to detach to the TSENS driver
  

  @param[in] hDalDevice       Handle to the TSens driver instance instantiated
                              by DAL_TsensDeviceAttach.

  @return
  DAL_SUCCESS -- Detach was successful.\n
  DAL_ERROR --  An error occurred when attaching to the device.
*/

#define DAL_TsensDeviceDetach(hDalDevice)   DAL_DeviceDetach(hDalDevice)
        	

/**
  Gets the last temperature measurement that occurred within a measurement
  period.

  This function waits until a measurement is complete. This means the calling
  thread can be blocked by up to several hundreds of microseconds. The exact
  delay depends on the number of sensors present in the hardware and the hardware
  conversion time per sensor. There is a fixed timeout value built into this function.
  If the measurement does not complete before the timeout, this function
  will return TSENS_ERROR_TIMEOUT.

  @param[in] _h               Handle to the TSens driver instance instantiated
                              by DAL_TsensDeviceAttach.
  @param[in] uSensor          Sensor to read from.
  @param[out] pTemp           Pointer to the temperature

  @return
  DAL_SUCCESS -- Successfully retrieved the temperature measurement.\n
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.\n
  TSENS_ERROR_UNSUPPORTED -- The TSENS hardware sensor is not supported.\n
  TSENS_ERROR_INVALID_CALIBRATION -- The TSENS hardware sensor is not calibrated.\n
  TSENS_ERROR_TIMEOUT -- The TSENS reading timed out.\n
  TSENS_ERROR_DEAD_SENSOR -- The TSENS sensor is not available.
*/
static __inline DALResult
DalTsens_GetTemp(DalDeviceHandle *_h, uint32 uSensor, TsensTempType *pTemp)
{
   if (pTemp == NULL)
   {
      return DAL_ERROR;
   }

   return ((DalTsensHandle *)_h)->pVtbl->GetTemp(_h, uSensor, pTemp);
}

/**
  Gets the temperature range supported for a given sensor.

  @param[in]  _h                Handle to the TSens driver instance instantiated
                                by DAL_TsensDeviceAttach.
  @param[in] uSensor            Sensor to get range from.
  @param[out] pTsensTempRange   Pointer to the sensor range.

  @return
  DAL_SUCCESS -- Successfully retrieved the supported temperature range.\n
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.\n
  TSENS_ERROR_UNSUPPORTED -- The TSENS hardware sensor is not supported.\n
  TSENS_ERROR_INVALID_CALIBRATION -- The TSENS hardware sensor is not calibrated.\n
  TSENS_ERROR_DEAD_SENSOR -- The TSENS sensor is not available.
*/
static __inline DALResult
DalTsens_GetTempRange(DalDeviceHandle *_h, uint32 uSensor, TsensTempRangeType * pTsensTempRange)
{
   if (pTsensTempRange == NULL)
   {
      return DAL_ERROR;
   }

   return ((DalTsensHandle *)_h)->pVtbl->GetTempRange(_h, uSensor, pTsensTempRange);
}

/**
  Configures a temperature threshold for the specified sensor

  The threshold event will be triggered once when the threshold is crossed.
  After the event is triggered, the threshold will not trigger the event again
  and will be in a triggered state until the client calls DalTsens_SetThreshold
  to set a new threshold.

  Note that thresholds can be disabled/re-enabled on a per client basis by calling
  DalTsens_SetEnableThresholds. Thresholds are enabled by default, but calling
  DalTsens_SetThreshold does not automatically re-enable them if they were previously
  disabled by a call to DalTsens_SetEnableThresholds.

  The event type can be:
    - DALSYS_EVENT_ATTR_NORMAL: the event will be triggered.
    - DALSYS_EVENT_ATTR_CALLBACK_EVENT: the callback will have a payload pointing
      to a TsensCallbackPayloadType struct.

  @param[in]  _h                Handle to the TSens driver instance instantiated
                                by DAL_TsensDeviceAttach.
  @param[in] uSensor            Sensor to use for the threshold.
  @param[in] eThreshold         Type of threshold to set.
  @param[in] nDegC              Threshold temperature in degrees C.
  @param[in] hEvent             Event which is notified when the threshold is reached.

  @return
  DAL_SUCCESS -- Successfully set the threshold.\n
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.\n
  TSENS_ERROR_UNSUPPORTED -- The TSENS hardware sensor is not supported.\n
  TSENS_ERROR_DEAD_SENSOR -- The TSENS sensor is not available.
*/
static __inline DALResult
DalTsens_SetThreshold(DalDeviceHandle *_h, uint32 uSensor, TsensThresholdType eThreshold, int32 nDegC, TsensThresholdCbType pfnCallback, void *pCtxt)
{
   return ((DalTsensHandle *)_h)->pVtbl->SetThreshold(_h, uSensor, eThreshold, nDegC, pfnCallback, pCtxt);
}

/**
  Enables or disables the upper and lower thresholds that were registered by this
  client by calls to DalTsens_SetThreshold.

  By default, thresholds are enabled.

  Thresholds are not monitored while the thresholds are disabled,
  and any threshold crossings which occurred while the thresholds were disabled
  are ignored.

  Threshold values and event handles set by DalTsens_SetThreshold
  are still retained while thresholds are disabled.

  This does not affect the critical thresholds. Critical thresholds are always
  enabled.

  @param[in]  _h                Handle to the TSens driver instance instantiated
                                by DAL_TsensDeviceAttach.
  @param[in] bEnableThresholds  TRUE to enable thresholds. FALSE to disable thresholds.

  @return
  DAL_SUCCESS -- Operation occurred successfully.\n
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.
*/
static __inline DALResult
DalTsens_SetEnableThresholds(DalDeviceHandle *_h, DALBOOL bEnableThresholds)
{
   return ((DalTsensHandle *)_h)->pVtbl->SetEnableThresholds(_h, bEnableThresholds);
}

/**
  Returns the number of on-die temperature (TSENS) sensors present in the SoC.

  @param[in]  _h                Handle to the TSens driver instance instantiated
                                by DAL_TsensDeviceAttach.
  @param[out] pnNumSensors      Number of sensors

  @return
  DAL_SUCCESS -- Successfully retrieved the number of sensors.\n
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.
*/
static __inline DALResult
DalTsens_GetNumSensors(DalDeviceHandle *_h, uint32 *pnNumSensors)
{
   return ((DalTsensHandle *)_h)->pVtbl->GetNumSensors(_h, pnNumSensors);
}

/**
  Checks the status of the Tsens calibration.

  @param[in]  _h                Handle to the TSens driver instance instantiated
                                by DAL_TsensDeviceAttach.
  @param[in] uSensor            Sensor to check the calibration.

  @return
  DAL_SUCCESS -- The TSENS calibration data is present in the hardware.\n
  TSENS_ERROR_INVALID_PARAMETER -- An invalid parameter was passed.\n
  TSENS_ERROR_UNSUPPORTED -- The TSENS hardware sensor is not supported.\n
  TSENS_ERROR_INVALID_CALIBRATION -- The TSENS hardware sensor is not calibrated.\n
  TSENS_ERROR_DEAD_SENSOR -- The TSENS sensor is not available.
*/
static __inline DALResult
DalTsens_CheckCalibration(DalDeviceHandle *_h, uint32 uSensor)
{
   return ((DalTsensHandle *)_h)->pVtbl->CheckCalibration(_h, uSensor);
}

/** @} */ /* addtogroup api_funcs */

#endif


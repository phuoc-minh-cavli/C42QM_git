/*=======================================================================
 Copyright (c) 2017-2019, 2020  Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/

 /*===========================================================================

						 EDIT HISTORY FOR MODULE

   This section contains comments describing changes made to the module.
   Notice that changes are listed in reverse chronological order.

 when         who   what, where, why
 ----------   ---   ---------------------------------------------------------
 2019-05-08   AY    CR:2440855 Set threshold api changes
 2018-01-15   leo   (Tech Comm) Edited/added Doxygen comments and markup.
 2017-04-26   leo   (Tech Comm) Edited/added Doxygen comments and markup.
 2017-04-10   KS    QAPI changes.
 2017-03-13   PC    Create.

 ===========================================================================*/

#ifndef QAPI_TSENS_H
#define QAPI_TSENS_H
/**
 * @file qapi_tsens.h
 * 
 * Contains QAPI driver definitions
 *
 * Temperature Sensor (TSENS)
 *
 * @addtogroup qapi_tsens_apis
 * @{
 *
 * The temperature sensor is used to monitor the temperature of the SoC
 * using on-die analog sensors.
 *
 * This programming interface allows client software to read the
 * temperature returned by each sensor. The code snippet below shows
 * an example usage.
 *
 * Consult hardware documentation for
 * the placement of the sensors on the die.
 *
 * @code
 *
 *
 *   * The code snippet below demonstates usage of this interface. The example
 *   * below opens TSENS to obtain a handle, gets the number of sensors, sets
 *   * temparature thresholds for each sensor, reads each sensor's
 *   * temperature, and then closes the handle.
 *
 *   qapi_Status_t status;
 *   qapi_TSENS_Handle_t handle;
 *   uint32_t num_sensors;
 *   uint32_t sensor;
 *   qapi_TSENS_Result_t result;
 *   qapi_TSENS_ThresholdType_t Threshold_Type;
 *   int32_t Threshold_Degree;
 *   QAPI_Tsens_Threshold_Cb_Type Threshold_CB;

 *   status = qapi_TSENS_Open(&handle);
 *   if (status != QAPI_OK) { ... }
 *
 *   status = qapi_TSENS_Get_Num_Sensors(handle, &num_sensors);
 *   if (status != QAPI_OK) { ... }
 *
 *   for (sensor = 0; sensor < num_sensors; sensor++)
 *   {
 *     status = qapi_TSENS_Get_Calibration_Status(handle, sensor, &result);
 *     if (status != QAPI_OK) { ... }
 *
 *     else
 *        {
 *          status=qapi_TSENS_Get_Temp(handle, sensor, &result);
 *          if (status != QAPI_OK) { ... }
 *
 *          else
 *             {
 *               status= qapi_TSENS_Set_Thresholds(handle, sensor, 
 *                               Threshold_Type, Threshold_Degree,
 *                                       Threshold_CB, context_ptr);
 *             	 if (status != QAPI_OK) { ... }
 *
 *      	     else
 * 	                {
 *                    status=qapi_TSENS_Set_Enable_Thresholds(handle,enable);
 *	                  if (status != QAPI_OK) { ... }
 * 	                 }
 *              }
 *          }
 *      // result->Deg_C is the temperature in degrees Celsius
 *   }
 *   status = qapi_TSENS_Close(handle);
 *   if (status != QAPI_OK) { ... }
 *   handle = NULL;
 *
 * @endcode
 *
 * @}
 */
/*
 * Copyright (C) 2017, 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.			
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2019-05-08   AY    CR:2440855 Set threshold api changes 
2017-04-10   KS    QAPI changes.
2017-03-13   PC    Create.

===========================================================================*/

#include "qapi_tsens_types.h"
                                                                                                                           
/*============================================================================
                USER_Mode_DEFS
============================================================================*/

                 /** Driver ID defines */

#define  TXM_QAPI_TSENS_OPEN                   TXM_QAPI_TSENS_BASE
#define  TXM_QAPI_TSENS_GET_NUM_SENSORS        TXM_QAPI_TSENS_BASE + 1
#define  TXM_QAPI_TSENS_GET_TEMP               TXM_QAPI_TSENS_BASE + 2
#define  TXM_QAPI_TSENS_GET_CALIBRATION_STATUS TXM_QAPI_TSENS_BASE + 3
#define  TXM_QAPI_TSENS_CLOSE                  TXM_QAPI_TSENS_BASE + 4

#define  TXM_QAPI_TSENS_SET_TRESHOLD_CB        CUSTOM_CB_QAPI_TSENS_BASE

int32_t qapi_TSENS_Handler(uint32_t id, uint32_t a, uint32_t b, uint32_t c,
                            uint32_t d, uint32_t e, uint32_t f, uint32_t g,
                            uint32_t h, uint32_t i, uint32_t j, uint32_t k,
                            uint32_t l);

#ifdef  QAPI_TXM_MODULE

/* CALLBACK DISPACTCHER */
int qapi_custom_cb_TSENS_dispatcher(UINT cb_id, void *app_cb, 
									 UINT cb_param1, UINT cb_param2, 
									 UINT cb_param3, UINT cb_param4,
									 UINT cb_param5, UINT cb_param6,
									 UINT cb_param7, UINT cb_param8);
              
/* API Definitions */

#define qapi_TSENS_Open(Handle)\
    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_TSENS_OPEN,\
    (ULONG) Handle, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0,\
    (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
    (ULONG) 0))

#define qapi_TSENS_Get_Num_Sensors(Handle, Num_Sensors_Ptr)\
    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_TSENS_GET_NUM_SENSORS,\
    (ULONG) Handle, (ULONG) Num_Sensors_Ptr, (ULONG) 0, (ULONG) 0,\
    (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0,\
    (ULONG) 0))

#define qapi_TSENS_Get_Temp(Handle, Sensor_Num, Temp_Result_Ptr)\
    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_TSENS_GET_TEMP,\
    (ULONG) Handle, (ULONG) Sensor_Num, (ULONG) Temp_Result_Ptr,\
    (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0,\
    (ULONG) 0, (ULONG) 0, (ULONG) 0))

#define qapi_TSENS_Get_Calibration_Status(Handle, Sensor_Num)\
    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_TSENS_GET_CALIBRATION_STATUS,\
    (ULONG) Handle, (ULONG) Sensor_Num, (ULONG) 0,\
    (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0,\
    (ULONG) 0, (ULONG) 0, (ULONG) 0))

#define qapi_TSENS_Close(Handle)\
    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_TSENS_CLOSE,\
    (ULONG) Handle, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0,\
    (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
    (ULONG) 0))
	
	

                                                                                                                                                                                                                                                                                                                 
#elif defined QAPI_TXM_SOURCE_CODE
    
/*============================================================================
                KERNEL_Mode_DEFS
============================================================================*/

/** @addtogroup qapi_tsens_apis
@{ */

/*============================================================================
                qapi_TSENS_Open
============================================================================*/
/**
*  Opens TSENS.
*
*  @param[out] Handle   Pointer to a TSENS handle.
*
*  @return
*  - QAPI_OK                -- Call succeeded.
*  - QAPI_ERROR             -- Call failed.
*  - QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t qapi_TSENS_Open
(
   qapi_TSENS_Handle_t *Handle
);

/*============================================================================
                qapi_TSENS_Get_Num_Sensors
============================================================================*/
/**
*  Gets the number of TSENS sensors.
*
*  This function gets the number of TSENS sensors supported by the SoC.
*  The sensor index is zero-based and ranges from 0 to the number of sensors
*  minus one.
*
*  @param[in]  Handle           Handle provided by qapi_TSENS_Open().
*  @param[out] Num_Sensors_Ptr  Number of sensors
*
*  @return
*  - QAPI_OK                -- Call succeeded.
*  - QAPI_ERROR             -- Call failed.
*  - QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t qapi_TSENS_Get_Num_Sensors
(
   qapi_TSENS_Handle_t Handle,
   int32_t *Num_Sensors_Ptr
);


/*============================================================================
                qapi_TSENS_Get_Temp
============================================================================*/
/**
*  Gets the temperature of a specified sensor.
*
*  This function waits until a measurement is complete. This means the calling
*  thread can be blocked by up to several hundreths of microseconds.
*  The exact delay depends on the number of sensors present in the hardware
*  and the hardware conversion time per sensor. There is a fixed timeout value
*  built into this function. If the measurement does not complete before the
*  timeout, this function returns TSENS_ERROR_TIMEOUT.
*
*  @param[in]  Handle           Handle provided by qapi_TSENS_Open().
*  @param[in]  Sensor_Num       Selected sensor
*  @param[out] Temp_Result_Ptr  Temperature reported by the sensor.
*
*  @return
*  - QAPI_OK                -- Call succeeded.
*  - QAPI_ERROR             -- Call failed.
*  - QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t qapi_TSENS_Get_Temp
(
   qapi_TSENS_Handle_t Handle,
   uint32_t  Sensor_Num,
   qapi_TSENS_Result_t * Temp_Result_Ptr
);


/*============================================================================
                qapi_TSENS_Get_Calibration_Status
============================================================================*/
/**
*  Gets the calibration status for a temperature sensor.
*
*  @param[in]  Handle       Handle provided by qapi_TSENS_Open().
*  @param[in]  Sensor_Num   Selected sensor number.
*
*  @return
*  - QAPI_OK                -- Call succeeded.
*  - QAPI_ERROR             -- Call failed.
*  - QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*  - QAPI_ERR_TIMEOUT       -- The sensor did not return a reading before the timeout.
*/
qapi_Status_t qapi_TSENS_Get_Calibration_Status
(
   qapi_TSENS_Handle_t Handle,
   uint32_t Sensor_Num
);


/*============================================================================
                qapi_TSENS_Close
============================================================================*/
/**
*  Closes TSENS.
*
*  @param[in] Handle   Handle provided by qapi_TSENS_Open().
*
*  @return
*  - QAPI_OK                -- Call succeeded.
*  - QAPI_ERROR             -- Call failed.
*  - QAPI_ERR_INVALID_PARAM -- Invalid parameters were specified.
*/
qapi_Status_t qapi_TSENS_Close
(
   qapi_TSENS_Handle_t Handle
);


#else   

  #error "No QAPI flags defined"

#endif

/** @} */ /* end_addtogroup qapi_tsens_apis */

#endif /** QAPI_TSENS_H */


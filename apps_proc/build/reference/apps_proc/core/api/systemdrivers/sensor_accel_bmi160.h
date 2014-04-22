#ifndef SENSOR_ACCEL_BMI160_H
#define SENSOR_ACCEL_BMI160_H
/**
  @file sensor_accel_bmi160.h
  @brief  APIs to interface with BMI160 accelerometer on 9x06 dev board (thething)

*/
/** @addtogroup accel_apis
@{ */

/*=============================================================================
  Copyright (c) 2016  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

#include <comdef.h>
#include "DALStdErr.h"

/*=============================================================================
                                 CONSTANTS AND MACROS
=============================================================================*/
#define MOTION_X_POSITIVE 0 /**< Motion in positive x direction */
#define MOTION_X_NEGATIVE 1 /**< Motion in negative x direction */
#define MOTION_Y_POSITIVE 2 /**< Motion in positive y direction */
#define MOTION_Y_NEGATIVE 3 /**< Motion in negative y direction */
#define MOTION_Z_POSITIVE 4 /**< Motion in positive z direction */
#define MOTION_Z_NEGATIVE 5 /**< Motion in negative z direction */

/*=============================================================================
                                    TYPEDEFS
=============================================================================*/

/** Callback type to handle motion events
   
*@param[in]  motion_trigger   Indicates the type of motion that triggered the event.
                              This field will take one of the following values:\n
                              - #MOTION_X_POSITIVE\n
                              - #MOTION_X_NEGATIVE\n
                              - #MOTION_Y_POSITIVE\n 
                              - #MOTION_Y_NEGATIVE\n 
                              - #MOTION_Z_POSITIVE\n 
                              - #MOTION_Z_NEGATIVE\n 
*/
typedef void (*motion_event_handler_type)(int motion_direction);

/*=============================================================================
                                    FUNCTIONS
=============================================================================*/
/**
    @brief
    Start up accelerometer
 
    @return
    DAL_SUCCESS if successful
*/
int bmi160_init(void);

/**
    @brief
    Shutdown accelerometer
 
    @return 
    DAL_SUCCESS if successful
*/
int bmi160_shutdown(void);

/**
    @brief
    Set threshold for motion events

    @details
	Absolute value of threshold will depend on the range (2g, 4g, 8g, 16g), which is internally configured by driver.\n
	Any motion duration (number of points above threshold required to trigger an interrupt) is also internally configured
	by the driver.

    @param[in] threshold		value in range 0 to 255

    @return 
    DAL_SUCCESS if successful
*/
int bmi160_set_motion_event_threshold(uint8 threshold);

/**
    @brief
    Enable generation of motion events (when motion threshold is crossed)

    @return 
    DAL_SUCCESS if successful
*/
int bmi160_enable_motion_events(void);

/**
    @brief
    Disable motion events

    @return 
    DAL_SUCCESS if successful
*/
int bmi160_disable_motion_events(void);

/**
    @brief
    Attach callback to be invoked when a motion event is triggered

    @param[in] handler		handler to be invoked to process motion event

    @return None
*/
void bmi160_attach_motion_event_handler(motion_event_handler_type handler);

/**
    @brief
    Get acceleration on x,y,z axis (scaled to units of 'g')

    @param[out] x		acceleration (scaled to units of 'g') on x-axis
    @param[out] y		acceleration (scaled to units of 'g') on y-axis
    @param[out] z		acceleration (scaled to units of 'g') on z-axis

    @return 
    DAL_SUCCESS if successful
*/
int bmi160_get_accel(float* x, float* y, float* z);

/** @} */ /* end_addtogroup accel_apis */

#endif // SENSOR_ACCEL_BMI160_H

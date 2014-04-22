#ifndef SENSOR_ALS_TMD2772_H
#define SENSOR_ALS_TMD2772_H

/**
 @file sensor_als_tmd2772.h
 @brief APIs to interface with TMD2772 ambient light sensor on 
        9x06 dev board (thething)
*/

/** @addtogroup als_apis
@{ */

#include <comdef.h>
#include "DALStdErr.h"

/** Callback type to handle light events */
typedef void (*light_event_handler_type)(void);

/**
   @brief
   Start up ambient light sensor 
 
   @return
   DAL_SUCCESS if successful
*/
int tmd2772_init(void);

/**
   @brief
   Shutdown ambient light sensor
 
   @return
   DAL_SUCCESS if successful
*/
int tmd2772_shutdown(void);

/** 
   @brief 
   Configure threshold for light events.\n
   Two luminance values (in Lux) are configured here (threshold_low, threshold_high).
   When the ambient light level (measured in Lux) falls below 
   threshold_low or goes above threshold_high, a light event will be triggered. \n
   The driver controls (via APERS) how many consecutive data points need 
   to be outside of this range before an event will be triggered.
   
   @return
   DAL_SUCCESS if successful
*/
int tmd2772_set_light_event_threshold(float threshold_low, float threshold_high);

/**
    @brief
    Enable generation of light events.
    Events are generated when ambient light level goes below threshold_low or above
    threshold_high as configured in tmd2772_set_light_event_threshold().

    @return 
    DAL_SUCCESS if successful
*/
int tmd2772_enable_light_events(void);

/**
    @brief
    Disable light events.

    @return 
    DAL_SUCCESS if successful
*/
int tmd2772_disable_light_events(void);


/**
    @brief
    Attach callback to be invoked when a light event is triggered

    @param[in] handler		handler to be invoked to process light event

    @return None
*/
void tmd2772_attach_light_event_handler(light_event_handler_type handler);

/**
    @brief
    Get Luminance (in units of Lux)

    This function reads the raw sensor data (C0Data, C1Data) and uses parameter values 
    configured by the driver [integration time (ATIME) and ALS gain (AGAIN)] with a formula 
    specified by the TMD2772 data sheet to compute the luminance value in units of Lux.

    @return 
    Luminance value in units of lux (floating point value)
*/
float tmd2772_get_lux(void);

/** @} */ /* end_addtogroup als_apis */

#endif // SENSOR_ALS_TMD2772_H



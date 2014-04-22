/*=============================================================================
  Copyright (c) 2016  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

#ifndef SENSOR_HUMIDITY_HTS221_H
#define SENSOR_HUMIDITY_HTS221_H

/**
 @file sensor_humidity_hts221.h
 @brief APIs to interface with HTS221 humidity + temperature sensor on 
        9x06 dev board (thething)
*/

/** @addtogroup humidtemp_apis
@{ */

#include <comdef.h>
#include "DALStdErr.h"

/**
   @brief
   Start up HTS221 humidity + temp sensor
 
   @return
   DAL_SUCCESS if successful
*/
int hts221_init(void);

/**
   @brief
   Shutdown HTS221 humidity + temp sensor
 
   @return
   DAL_SUCCESS if successful
*/
int hts221_shtudown(void);

/**
   @brief
   Read current temperature value (in degrees Celcius)
 
   @return
   Temperature in degrees Celcius (floating point)
*/
float hts221_get_temp(void);

/**
   @brief
   Read relative humidity (\%rH value between 0.0 and 100.0)

   \note
   This function is optional and may not be supported in the initial driver version.
   Please check driver release notes prior to using.
 
   @return
   Relative humidity (\%rH value between 0.0 and 100.0)
*/
float hts221_get_humidity(void); // optional

/** @} */ /* end_addtogroup humidtemp_apis */

#endif // SENSOR_HUMIDITY_HTS221_H

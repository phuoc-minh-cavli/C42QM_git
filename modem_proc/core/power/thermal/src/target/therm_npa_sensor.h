#ifndef THERM_NPA_SENSOR_H
#define THERM_NPA_SENSOR_H

/*============================================================================
  FILE:         therm_npa_sensor.h
  
  OVERVIEW:     Thermal NPA sensor header file.
 
                Copyright (c) 2011-2014, 2017 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.mpss/3.10/power/thermal/src/target/therm_npa_sensor.h#1 $
$DateTime: 2019/01/21 21:54:53 $
$Author: pwbldsvc $
========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*
===============================================================================

                             TYPE DEFINITION(S)

===============================================================================
*/

/*
===============================================================================

                           GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
  @brief therm_npa_sensor_init
 
  Thermal NPA sensor init..
 
*/
void therm_npa_sensor_init(void);

/**
  @brief therm_sampling_timer_cb
  
  Used to trigger sensor read update.

  @param  : timerIndex: Unused.

  @return : None.
*/
void therm_sampling_timer_cb(void);
#ifdef __cplusplus
}
#endif

#endif /* THERM_NPA_SENSOR_H */

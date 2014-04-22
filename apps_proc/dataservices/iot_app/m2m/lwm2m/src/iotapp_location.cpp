 /*
 * \file        iotapp_location.cpp
 * \brief       File contains the Location API Integration for ThingSpace client and LWM2M
 */
/*===========================================================================
  Copyright (c) 2017-2021  by Qualcomm Technologies, Inc.  All Rights Reserved.
 
  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 ======================================================================*/  
#include <cstdio>

extern "C" {
#include <stdio.h>
#include <stdint.h>
#include "qurt.h"
#include <stdlib.h>
#include <ctype.h>
#include <stringl/stringl.h>
#include "object_location.h"
#include "qapi_location.h"
#include "lwm2m_signals.h"
#include "liblwm2m.h"
#include "ds_log.h"

}

/*******************************************************************
 *                Preprocesser Directives
 * *****************************************************************/

#define LOC_SLEEP_TIMEOUT   5
#define LOC_SIGNAL_MASK	    1
/**********************************************************************
 *                Function prototypes
 **********************************************************************/
static bool location_init();
static void location_deinit();
inline void update_common_gps_data(qapi_Location_t location);



/**********************************************************************
 *                Variable declations
 **********************************************************************/
uint32 trackingSessionId = 0; /*Location tracking session ID*/
float latitude_data;
float longitude_data;
static qurt_signal_t locationResponse; /*Location repsonse signal handle*/

bool fetch_best_avail_loc = false;
bool location_init_status;    /* Variable used to maintain the status of location service init */
/* global variables to update gps data */
static uint64_t timestamp; 
static double latitude;    
static double longitude;
static float bearing;
static float horUncertainity;
static double altitude_wrt_msl;    
static float speed;
static float radius;
static qapi_loc_client_id locClientId;
extern qurt_signal_t lwm2m_signal_Handle;
extern bool active_single_shot;
extern uint32_t location_min_interval;

#define IOTAPP_USER_BUFFER_SIZE 512
static uint8_t *iotAppUserBuffer = NULL;

extern "C"{

  extern void lwm2m_indicate_location_resource_value_change(const char *uriPath);	

  /**
   * @brief location single shot
   * @return Session ID
   */

  uint32_t lh_location_single_shot()
  {
    uint32_t session_id;
    uint32_t loc_signals = 0;
    
    ds_iot_ulog_high("LOCATION SINGLE SHOT");
    qapi_Loc_Get_Single_Shot(locClientId,QAPI_LOCATION_POWER_HIGH,&session_id);
    ds_iot_ulog_high1("LOCATION SINGLE SHOT RETURN session id=%u", session_id);
  
    ds_iot_ulog_high("Waiting for Response Callback...");
      
    /* Waiting for Start Response Callback */
    qurt_signal_wait_timed_ext(&locationResponse, LOC_SIGNAL_MASK,
          QURT_SIGNAL_ATTR_WAIT_ANY | QURT_SIGNAL_ATTR_CLEAR_MASK, &loc_signals, 
  		  qurt_timer_convert_time_to_ticks(LOC_SLEEP_TIMEOUT, QURT_TIME_SEC));
  	
    ds_iot_ulog_high("LOCATION SINGLE SHOT DONE");
    return session_id;
  }

  /**
   * @brief location best available
   * @return Session ID
   */

  uint32_t lh_location_best_available()
  {
    uint32_t session_id = 0;
    uint32_t loc_signals = 0;
    
    ds_iot_ulog_high("LOCATION BEST AVAILABLE");
    qapi_Loc_Get_Best_Available_Position(locClientId, &session_id);
    ds_iot_ulog_high1("LOCATION BEST AVAILABLE RETURN session id=%u", session_id);
  
    ds_iot_ulog_high("Waiting for Response Callback...");
      
    /* Waiting for best available Response Callback */
    qurt_signal_wait_timed_ext(&locationResponse, LOC_SIGNAL_MASK,
          QURT_SIGNAL_ATTR_WAIT_ANY | QURT_SIGNAL_ATTR_CLEAR_MASK, &loc_signals, 
  		  qurt_timer_convert_time_to_ticks(LOC_SLEEP_TIMEOUT, QURT_TIME_SEC));
  	
    ds_iot_ulog_high("LOCATION BEST AVAILABLE DONE");
    return session_id;
  }
   
}

/**
 * @brief Capabilities callback
 * @param capabilities Callback data
 */
static void location_capabilities_callback(qapi_Location_Capabilities_Mask_t capabilities)
{
  ds_iot_ulog_high1("LOCATION CAPABILITIES CALLBACK 0x%X", capabilities);
}

/**
 * @brief Location response callback
 * @param err Error number
 * @param id Session ID on which callback is called
 */
static void location_response_callback(qapi_Location_Error_t err, uint32_t id)
{
  ds_iot_ulog_high2("LOCATION RESPONSE CALLBACK err=%u id=%u", err, id);

  qurt_signal_set(&locationResponse, LOC_SIGNAL_MASK);
}

static void location_geofence_response_callback(size_t count, qapi_Location_Error_t* err, uint32_t* ids)
{
  if((*err <= QAPI_LOCATION_ERROR_SUCCESS) && (*err >= QAPI_LOCATION_ERROR_GEOFENCES_AT_MAX))
  {
    LOG_DEBUG(" IN Func %s Invalid parameter : err",__func__); 
    return;
  }
  ds_iot_ulog_high("LOCATION GEOFENCE RESPONSE CALLBACK");
  for (uint32_t i=0; i < count; ++i)
  {
    ds_iot_ulog_high2("err=%u id=%u", err[i], ids[i]);
  }
}
static void location_single_shot_callback(qapi_Location_t location,qapi_Location_Error_t err)
{
  ds_iot_ulog_high("LOCATION SINGLE SHOT CALLBACK");
  active_single_shot = false;
  if(err == QAPI_LOCATION_ERROR_SUCCESS)
  {

    if(location.flags & QAPI_LOCATION_IS_BEST_AVAIL_POS_BIT)
    {
      ds_iot_ulog_high("QAPI_LOCATION_BEST_AVAIL_POS_BIT set callback");
      fetch_best_avail_loc = false;
    }
    ds_iot_log_strf_high("LAT: %d.%d LON: %d.%d ALT: %d.%d ALT_MSL: %d.%d ACC: %d.%d",
      (int)location.latitude, ((int)(location.latitude*100000))%100000,
      (int)location.longitude, ((int)(location.longitude*100000))%100000,
      (int)location.altitude, ((int)(location.altitude*100))%100,
      (int)location.altitudeMeanSeaLevel, ((int)(location.altitudeMeanSeaLevel*100))%100,
      (int)location.accuracy, ((int)(location.accuracy*100))%100);
    latitude_data  = location.latitude;
    longitude_data = location.longitude;
    qurt_signal_set(&lwm2m_signal_Handle, LOCATION_SINGLE_SHOT_CALLBACK);
    update_common_gps_data(location);     /* API used to update GPS Info for M2M */

    ds_iot_log_strf_high("SPEED: %d.%d BEAR: %d.%d TIME: 0x%x%x FLAGS: %u",
      (int)location.speed, ((int)(location.speed*100))%100,
      (int)location.bearing, ((int)(location.bearing*100))%100,
      location.timestamp>>32, location.timestamp, location.flags);
  }
  else if(err == QAPI_LOCATION_ERROR_BEST_AVAIL_POS_INVALID)
  {
    ds_iot_ulog_high("QAPI_LOCATION_ERROR_BEST_AVAIL_POS_INVALID");
    fetch_best_avail_loc = false;
    qurt_signal_set(&lwm2m_signal_Handle, LOCATION_SINGLE_SHOT_CALLBACK);
  }
  else
  {
    ds_iot_ulog_high("LOCATION SINGLE SHOT API TIMEDOUT");
    fetch_best_avail_loc = true;
    qurt_signal_set(&lwm2m_signal_Handle, LOCATION_FETCH);
  }
  
}

/** Location callbacks*/

static qapi_Location_Callbacks_t lhLocationCallbacks = {
  sizeof(qapi_Location_Callbacks_t),
  location_capabilities_callback,
  location_response_callback,
  location_geofence_response_callback,
  NULL,
  NULL,
  NULL,
  location_single_shot_callback
};


extern "C" {
/**
 * @brief Initialize Location object wrapper in C
 * @param minInterval Minimum Interval in ms
 * @param minDistance Minimum Distance in meters
 * @return Success or Failure
 */
bool lh_location_init()
{
  return location_init();
}

/**
 * @brief Deinitialize Location object wrapper in C
 * @return Success or Failure
 */
void lh_location_deinit()
{
   location_deinit();
}

} /*extern "C"*/

/**
 * @brief Initialize Location object 
 * @param minInterval Minimum Interval in ms
 * @param minDistance Minimum Distance in meters
 * @return Success or Failure
 */

static bool location_init()
{       
  int ret;

  if(!location_init_status)
  {
    ds_iot_ulog_err("Location Init");
    
    qurt_signal_init(&locationResponse);

    ret = qapi_Loc_Init(&locClientId, &lhLocationCallbacks);
    if(ret != QAPI_LOCATION_ERROR_SUCCESS) {
      ds_iot_ulog_err("Location initialization failed");
      return FALSE;
    }
	iotAppUserBuffer = (uint8_t *)malloc(IOTAPP_USER_BUFFER_SIZE);
	if(iotAppUserBuffer == NULL)
	{
      return FALSE; 
	}
	memset(iotAppUserBuffer, 0x00, IOTAPP_USER_BUFFER_SIZE);
    ret = qapi_Loc_Set_User_Buffer(locClientId, iotAppUserBuffer, IOTAPP_USER_BUFFER_SIZE);
    if(ret != QAPI_LOCATION_ERROR_SUCCESS) {
      ds_iot_ulog_err("Location initialization failed");
      return FALSE;
    }

    ds_iot_ulog_high1("Location initialization succeded!!ClientId = %d",locClientId);
    location_init_status = TRUE;
    }
  else
  {
    ds_iot_ulog_high("Location services already initialized ");
  }
  return TRUE;
}


/**
 * @brief Deinitialize Location object
 * @return Success or Failure
 */
static void location_deinit()
{
  ds_iot_ulog_high("Location Deinit");

  if (location_init_status) {
    qapi_Loc_Deinit(locClientId);
    location_init_status = FALSE;
	if(iotAppUserBuffer)
	{
	  free(iotAppUserBuffer);
	  iotAppUserBuffer = NULL;
	}
  }

  qurt_signal_destroy(&locationResponse);
}

inline void update_common_gps_data(qapi_Location_t location)
{
  if(ABS_DIFF(latitude, location.latitude) >= EPSILON)
  {
    ds_iot_ulog_high("update_common_gps_data:latitude changed");
  latitude = location.latitude;
    lwm2m_indicate_location_resource_value_change("/6/0/0");
  }
  if(ABS_DIFF(longitude, location.longitude) >= EPSILON)
  {
    ds_iot_ulog_high("update_common_gps_data:longitude changed");
    longitude = location.longitude;
    lwm2m_indicate_location_resource_value_change("/6/0/1");
  }
  if(ABS_DIFF(altitude_wrt_msl, location.altitudeMeanSeaLevel) >= EPSILON)
  {
    ds_iot_ulog_high("update_common_gps_data:altitude changed");
    altitude_wrt_msl = location.altitudeMeanSeaLevel;
    lwm2m_indicate_location_resource_value_change("/6/0/2");
  }
  if((double)ABS_DIFF(radius, location.accuracy) >= EPSILON)
  {
    ds_iot_ulog_high("update_common_gps_data:radius changed");
    radius = location.accuracy;
    lwm2m_indicate_location_resource_value_change("/6/0/3");
  }
  if(timestamp != location.timestamp)
  {
    ds_iot_ulog_high("update_common_gps_data:timestamp changed");
    timestamp = location.timestamp;
    lwm2m_indicate_location_resource_value_change("/6/0/5");
  }
  if((double)ABS_DIFF(speed, location.speed) >= EPSILON)
  {
    ds_iot_ulog_high("update_common_gps_data:speed changed");
    lwm2m_indicate_location_resource_value_change("/6/0/6");
  }
  if(((double)ABS_DIFF(speed, location.speed) >= EPSILON) || ((double)ABS_DIFF(bearing, location.bearing) >= EPSILON))
  {
    ds_iot_ulog_high("update_common_gps_data:velocity changed");
    lwm2m_indicate_location_resource_value_change("/6/0/4");
  }
  speed = location.speed;
  bearing = location.bearing;
  horUncertainity = location.speedAccuracy;

  return;
}


void get_m2m_gps_data(location_data_t* loc_data, float *loc_bearing, float *loc_horUncertainity)
{
  if(NULL == loc_data)
  {
    LOG_DEBUG(" In func :%s Invalid parameters Loc_data & loc_bearing",__func__);
    return;
  }

  loc_data->latitude = latitude;
  loc_data->longitude = longitude;
  loc_data->altitude = altitude_wrt_msl;
  loc_data->speed = speed;
  loc_data->radius = radius;
  if(loc_data != NULL)
    loc_data->timestamp = timestamp;
  if(NULL != loc_bearing)
  {
    memscpy(loc_bearing, sizeof(loc_bearing), &bearing, sizeof(loc_bearing));
  }
  if(loc_horUncertainity != NULL)
  {
    *loc_horUncertainity = horUncertainity;
  }
}



/*============================================================================
 @file loc_sensor_client.c

 loc MW sensor client implementation

 GENERAL DESCRIPTION

 This file contains the loc middleware sensor client implementation.

 Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
 All Rights Reserved
 Qualcomm Confidential and Proprietary
 
 Copyright (c) 2015 Qualcomm Atheros, Inc.
 All Rights Reserved. 
 Qualcomm Atheros Confidential and Proprietary.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.

 =============================================================================*/

/*============================================================================

 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.

 $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_sensor_iod.c#1 $
 $DateTime: 2020/07/21 21:23:44 $
 $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

=============================================================================*/

#include "customer.h"

#if (defined(FEATURE_GEOFENCE_SENSOR_SUPPORT) && (!defined(FEATURE_SMART_OFFLINE_COMPILATION)))
#ifdef FEATURE_GEOFENCE_IOD_SENSOR_SUPPORT
#include "comdef.h"
#include "msg.h"
#include "loc_sensor_iod.h"
#include "gm_api.h"

#define SHIM_MSG_LOW( str, a, b, c )                                          \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str,  a, b, c )
#define SHIM_MSG_MED( str, a, b, c )                                          \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str, a, b, c )
#define SHIM_MSG_HIGH( str, a, b, c )                                         \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str, a, b, c )
#define SHIM_MSG_ERROR( str, a, b, c )                                        \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR, str, a, b, c )
#define SHIM_ERR_FATAL( str, a, b, c )                                        \
                  MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_FATAL, str, a, b, c )

/** loc_geofence_sensor_start_iod_autocal
 *    Handles IOD AutoCal Start request from Geofence module. We start the 
 *    AutoCal only once in a boot cycle. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_iod_autocal
(
  uint32  q_utc_time_secs,
  uint32  q_sunrisetime_sec,
  uint32  q_sunsettime_sec
)
{
  (void)q_utc_time_secs;
  (void)q_sunrisetime_sec;
  (void)q_sunsettime_sec;

  SHIM_MSG_ERROR("loc_geofence_sensor_start_iod_autocal: IOD Sensor Service not available in SLIM\n", 0, 0, 0);
  return FALSE;
}

/** loc_geofence_sensor_start_iod
 *    Handles IOD Start request from Geofence module. 
 *  @param [in] report_period
 * */
boolean loc_geofence_sensor_start_iod
(
  uint32 utc_ts,
  uint32 sunrise_ts,
  uint32 sunset_ts
)
{
  (void)utc_ts;
  (void)sunrise_ts;
  (void)sunset_ts;

  SHIM_MSG_ERROR("loc_geofence_sensor_start_iod: IOD Sensor Service not available in SLIM\n",0, 0,0);
  return FALSE;
}


/** loc_geofence_sensor_stop_iod
 *    Handles IOD Stop request from Geofence module. 
 *  @param [in] void
 * */
boolean loc_geofence_sensor_stop_iod
(
  uint8 u_instance_id_valid,
  uint8 u_instance_id
)
{
  (void)u_instance_id_valid;
  (void)u_instance_id;

  SHIM_MSG_ERROR("loc_geofence_sensor_stop_iod: IOD Sensor Service not available in SLIM\n", 0, 0, 0);
  return FALSE;
}

/** loc_geofence_configure_iod
 *    Handles IOD Configure request from Geofence module. 
 *  @param [in] p_config_iod
 * */
boolean loc_geofence_configure_iod(sns_sam_iod_update_req_msg_v01 *p_config_iod)
{
  (void)p_config_iod;

  SHIM_MSG_ERROR("loc_geofence_sensor_configure_iod: IOD Sensor Service not available in SLIM\n", 0, 0, 0);
  return FALSE;
}
#endif /*FEATURE_GEOFENCE_IOD_SENSOR_SUPPORT*/
#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/



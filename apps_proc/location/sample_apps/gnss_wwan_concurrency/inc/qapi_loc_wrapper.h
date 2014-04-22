/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#ifndef __QAPI_LOC_WRAPPER_H__
#define __QAPI_LOC_WRAPPER_H__

#include "app_types.h"
#include <qapi_location.h>

void qapi_loc_wrapper_setup();

/*-----------------------------------------------------------------------------------
  Wrappers around Location QAPI functions
  Return 0 for successful QAPI invocation, -1 for error.
 -----------------------------------------------------------------------------------*/
int qapi_loc_wrapper_init();
int qapi_loc_wrapper_deinit();

int qapi_loc_wrapper_get_single_shot(
        uint32 powerLevel, uint32 accuracy, uint32 timeout);
int qapi_loc_wrapper_cancel_single_shot();

int qapi_loc_wrapper_tracking_start(
        uint32 tbf, uint32 dbf, uint32 accuracy);
int qapi_loc_wrapper_tracking_stop();

int qapi_loc_wrapper_get_best_available_position();

int qapi_loc_wrapper_start_motion_tracking(
        qapi_Location_Motion_Type_Mask_t mask);
int qapi_loc_wrapper_stop_motion_tracking();

int qapi_loc_wrapper_add_geofence(
        double latitude, double longitude, double radius,
        uint16 breachTypeMask, uint32 responsivenessMillis, uint32 dwellTimeMillis);
int qapi_loc_wrapper_remove_geofence();

/*---------------------------------------------------------
 * CALLBACKS registered with Location QAPI
 *--------------------------------------------------------*/
void qapi_loc_wrapper_capabilities_cb(
        qapi_Location_Capabilities_Mask_t capabilitiesMask);
void qapi_loc_wrapper_response_cb(
        qapi_Location_Error_t err, uint32_t id);
void qapi_loc_wrapper_collective_response_cb(
        size_t count, qapi_Location_Error_t* err, uint32_t* ids);

void qapi_loc_wrapper_tracking_cb(qapi_Location_t location);
void qapi_loc_wrapper_batching_cb(size_t count, qapi_Location_t* location);
void qapi_loc_wrapper_single_shot_cb(
    qapi_Location_t location, qapi_Location_Error_t err);
void qapi_loc_wrapper_geofence_breach_cb(
        qapi_Geofence_Breach_Notification_t notification);
void qapi_loc_wrapper_gnss_data_cb(qapi_Gnss_Data_t gnssData);
void qapi_loc_wrapper_gnss_meta_data_cb(qapi_Location_Meta_Data_t metaData);
void qapi_loc_wrapper_gnss_nmea_cb(qapi_Gnss_Nmea_t nmea);
void qapi_loc_wrapper_motion_tracking_cb(qapi_Location_Motion_Info_t info);

#endif /* __QAPI_LOC_WRAPPER_H__ */

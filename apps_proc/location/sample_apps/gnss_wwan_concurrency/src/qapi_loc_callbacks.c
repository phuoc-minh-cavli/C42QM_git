/*****************************************************************************
 Copyright (c) 2020 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 *****************************************************************************/
#include <stdint.h>

#include "app_types.h"
#include "app_utils_misc.h"
#include "qapi_loc_wrapper.h"
#include "qapi_location.h"
#include "gnss_worker_thread.h"
#include "app_cfg.h"

extern TX_MUTEX* _MutexResponse;
extern TX_EVENT_FLAGS_GROUP* _SignalResponse;
extern TX_EVENT_FLAGS_GROUP* _SignalCollectiveResponse;

// This one cb for gnss worker to be invoked in loc context
void gnss_worker_sm_singleshot_cb_in_loc_context(
        qapi_Location_t location, qapi_Location_Error_t err);

/*---------------------------------------------------------
 * RESPONSE CALLBACKs
 *--------------------------------------------------------*/
void qapi_loc_wrapper_response_cb(qapi_Location_Error_t err, uint32_t id)
{
    APP_LOG_QAPI_CALLBACK("RESPONSE CB (err %d id %d)", err, id);

    app_utils_set_signal(_SignalResponse, _MutexResponse);
}

/*---------------------------------------------------------
 * COLLECTIVE RESPONSE CALLBACKs
 *--------------------------------------------------------*/
void qapi_loc_wrapper_collective_response_cb(
        size_t count, qapi_Location_Error_t* err, uint32_t* ids)
{
    APP_LOG_QAPI_CALLBACK("COLLECTIVE RESPONSE CB (count %d)", count);

    for (uint32 i = 0; i < count; ++i) {
        APP_LOGH("ID[%d] = %d, err[%d] = %d", i, ids[i], i, err[i]);
    }

    app_utils_set_signal(_SignalCollectiveResponse, _MutexResponse);
}

/*---------------------------------------------------------
 * CAPABILITIES CALLBACKs
 *--------------------------------------------------------*/
void qapi_loc_wrapper_capabilities_cb(
        qapi_Location_Capabilities_Mask_t capabilitiesMask)
{
    APP_LOG_QAPI_CALLBACK("CAPABILITIES CB (mask 0x%x)", capabilitiesMask);
}

/*---------------------------------------------------------
 * TRACKING CALLBACKs
 *--------------------------------------------------------*/
void qapi_loc_wrapper_tracking_cb(qapi_Location_t location)
{
    APP_LOG_QAPI_CALLBACK("TRACKING CB");

    uint64_t ts_sec = location.timestamp/1000;

    APP_LOGH("Time(HH:MM:SS): %02d:%02d:%02d",
            (int)((ts_sec/3600)%24), (int)((ts_sec/60)%60), (int)(ts_sec%60));

    APP_LOGH("LAT: %d.%05d LON: %d.%05d ACC: %d.%02d",
        (int)location.latitude, (abs((int)(location.latitude * 100000))) % 100000,
        (int)location.longitude, (abs((int)(location.longitude * 100000))) % 100000,
        (int)location.accuracy, (abs((int)(location.accuracy * 100))) % 100);

    APP_LOGH("ALT: %d.%02d ALT_MSL: %d.%02d",
        (int)location.altitude, (abs((int)(location.altitude * 100))) % 100,
        (int)location.altitudeMeanSeaLevel, (abs((int)(location.altitudeMeanSeaLevel * 100))) % 100);

    APP_LOGH("SPEED: %d.%02d BEAR: %d.%02d TIME: 0x%x%x FLAGS: %u",
        (int)location.speed, (abs((int)(location.speed * 100))) % 100,
        (int)location.bearing, (abs((int)(location.bearing * 100))) % 100,
        (int)(location.timestamp >> 32), (int)location.timestamp, location.flags);

    gnss_worker_send_msg_cb_tracking(location);
}

/*---------------------------------------------------------
 * BATCHING CALLBACKs
 *--------------------------------------------------------*/
void qapi_loc_wrapper_batching_cb(size_t count, qapi_Location_t* location)
{
    APP_LOG_QAPI_CALLBACK("BATCHING CB (count %d)", count);

    uint64_t ts_sec = 0;
    for (uint32 i = 0; i < count; ++i) {
        ts_sec = location[i].timestamp/1000;
        APP_LOGH("Time(HH:MM:SS): %02d:%02d:%02d",
                (int)((ts_sec/3600)%24), (int)((ts_sec/60)%60), (int)(ts_sec%60));

        APP_LOGH("location[%d]:", i);

        APP_LOGH("LAT: %d.%05d LON: %d.%05d ACC: %d.%02d",
            (int)location[i].latitude, (abs((int)(location[i].latitude * 100000))) % 100000,
            (int)location[i].longitude, (abs((int)(location[i].longitude * 100000))) % 100000,
            (int)location[i].accuracy, (abs((int)(location[i].accuracy * 100))) % 100);

        APP_LOGH("ALT: %d.%02d ALT_MSL: %d.%02d",
            (int)location[i].altitude, (abs((int)(location[i].altitude * 100))) % 100,
            (int)location[i].altitudeMeanSeaLevel, (abs((int)(location[i].altitudeMeanSeaLevel * 100))) % 100);

        APP_LOGH("SPEED: %d.%02d BEAR: %d.%02d TIME: 0x%x%x FLAGS: %u",
            (int)location[i].speed, (abs((int)(location[i].speed * 100))) % 100,
            (int)location[i].bearing, (abs((int)(location[i].bearing * 100))) % 100,
            (int)(location[i].timestamp >> 32), (int)location[i].timestamp, location[i].flags);
    }
}

/*---------------------------------------------------------
 * GEOFENCE BREACH CALLBACKs
 *--------------------------------------------------------*/
void qapi_loc_wrapper_geofence_breach_cb(
        qapi_Geofence_Breach_Notification_t notification)
{
    APP_LOG_QAPI_CALLBACK("GEOFENCE BREACH CB (count %d)", notification.count);

    // Breach type
    if (notification.type == QAPI_GEOFENCE_BREACH_ENTER) {
        APP_LOGH("Breach Type: ENTER");
    } else if (notification.type == QAPI_GEOFENCE_BREACH_EXIT) {
        APP_LOGH("Breach Type: EXIT");
    } else if (notification.type == QAPI_GEOFENCE_BREACH_DWELL_IN) {
        APP_LOGH("Breach Type: DWELL IN");
    } else if (notification.type == QAPI_GEOFENCE_BREACH_DWELL_OUT) {
        APP_LOGH("Breach Type: DWELL OUT");
    } else {
        APP_LOGH("Breach type invalid: %d", notification.type);
    }

    // Timestamp and List of IDs received
    uint64_t ts_sec = notification.location.timestamp/1000;
    APP_LOGH("Time(HH:MM:SS): %02d:%02d:%02d : "
            "List of IDs recvd (count %d):",
            (int)((ts_sec/3600)%24), (int)((ts_sec/60)%60), (int)(ts_sec%60),
            notification.count);
    for(uint32 i = 0; i < notification.count; i++) {
        APP_LOGH("ID: %d", notification.ids[i]);
    }

    APP_LOGH("LAT: %d.%05d LON: %d.%05d ACC: %d.%02d",
        (int)notification.location.latitude,
        (abs((int)(notification.location.latitude * 100000))) % 100000,
        (int)notification.location.longitude,
        (abs((int)(notification.location.longitude * 100000))) % 100000,
        (int)notification.location.accuracy,
        (abs((int)(notification.location.accuracy * 100))) % 100);

    APP_LOGH("ALT: %d.%02d ALT_MSL: %d.%02d",
        (int)notification.location.altitude,
        (abs((int)(notification.location.altitude * 100))) % 100,
        (int)notification.location.altitudeMeanSeaLevel,
        (abs((int)(notification.location.altitudeMeanSeaLevel * 100))) % 100);

    APP_LOGH("SPEED: %d.%02d BEAR: %d.%02d TIME: 0x%x%x FLAGS: %u",
        (int)notification.location.speed,
        (abs((int)(notification.location.speed * 100))) % 100,
        (int)notification.location.bearing,
        (abs((int)(notification.location.bearing * 100))) % 100,
        (int)(notification.location.timestamp >> 32),
        (int)notification.location.timestamp, notification.location.flags);
}

/*---------------------------------------------------------
 * SINGLESHOT CALLBACKs
 *--------------------------------------------------------*/
void qapi_loc_wrapper_single_shot_cb(
        qapi_Location_t location, qapi_Location_Error_t err)
{
    APP_LOG_QAPI_CALLBACK("SINGLESHOT CB");

    if (QAPI_LOCATION_ERROR_TIMEOUT == err)
    {
        APP_LOGH("Timeout occurred, location is invalid");
    }
    else
    {
        uint64_t ts_sec = location.timestamp / 1000;

        APP_LOGH("Time(HH:MM:SS): %02d:%02d:%02d",
            (int)((ts_sec / 3600) % 24), (int)((ts_sec / 60) % 60), (int)(ts_sec % 60));

        APP_LOGH("LAT: %d.%05d LON: %d.%05d ACC: %d.%02d",
            (int)location.latitude, (abs((int)(location.latitude * 100000))) % 100000,
            (int)location.longitude, (abs((int)(location.longitude * 100000))) % 100000,
            (int)location.accuracy, (abs((int)(location.accuracy * 100))) % 100);

        APP_LOGH("ALT: %d.%02d ALT_MSL: %d.%02d",
            (int)location.altitude, (abs((int)(location.altitude * 100))) % 100,
            (int)location.altitudeMeanSeaLevel, (abs((int)(location.altitudeMeanSeaLevel * 100))) % 100);

        APP_LOGH("SPEED: %d.%02d BEAR: %d.%02d TIME: 0x%x%x FLAGS: %u",
            (int)location.speed, (abs((int)(location.speed * 100))) % 100,
            (int)location.bearing, (abs((int)(location.bearing * 100))) % 100,
            (int)(location.timestamp >> 32), (int)location.timestamp, location.flags);
    }

    if (APP_CONFIG.LOCATION_MODE == 0)
    {
        gnss_worker_sm_singleshot_cb_in_loc_context(location, err);
    }
    gnss_worker_send_msg_cb_singleshot(location, err);
}

/*---------------------------------------------------------
 * GNSS DATA CALLBACKs
 *--------------------------------------------------------*/
void qapi_loc_wrapper_gnss_data_cb(qapi_Gnss_Data_t gnssData)
{
    APP_LOG_QAPI_CALLBACK("GNSS DATA CB");

    APP_LOGH("JAMMER_GPS: %u JAMMER_GLONASS: %u",
        gnssData.jammerInd[QAPI_GNSS_SV_TYPE_GPS],
        gnssData.jammerInd[QAPI_GNSS_SV_TYPE_GLONASS]);

    APP_LOGH("JAMMER_BEIDOU: %u JAMMER_GALILEO: %u",
        gnssData.jammerInd[QAPI_GNSS_SV_TYPE_BEIDOU],
        gnssData.jammerInd[QAPI_GNSS_SV_TYPE_GALILEO]);
}

/*---------------------------------------------------------
 * META DATA CALLBACKs
 *--------------------------------------------------------*/
void qapi_loc_wrapper_gnss_meta_data_cb(qapi_Location_Meta_Data_t metaData)
{
    APP_LOG_QAPI_CALLBACK("GNSS META DATA CB");

    char engineStatusArr[27][60] = {
            "QAPI_LOCATION_ENGINE_STATUS_UNKNOWN",
            "QAPI_LOCATION_ENGINE_STATUS_DATA_CALL_SUCCESS",
            "QAPI_LOCATION_ENGINE_STATUS_DATA_CALL_FAILED",
            "QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_START",
            "QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_SUCCESS",
            "QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_FAILED",
            "QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_THROTTLED",
            "QAPI_LOCATION_ENGINE_STATUS_NTP_DL_START",
            "QAPI_LOCATION_ENGINE_STATUS_NTP_DL_SUCCESS",
            "QAPI_LOCATION_ENGINE_STATUS_NTP_DL_FAILED",
            "QAPI_LOCATION_ENGINE_STATUS_NTP_DL_THROTTLED",
            "QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_START",
            "QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_SUCCESS",
            "QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_FAILED",
            "QAPI_LOCATION_ENGINE_STATUS_WIFI_POS_START",
            "QAPI_LOCATION_ENGINE_STATUS_WIFI_POS_SUCCESS",
            "QAPI_LOCATION_ENGINE_STATUS_WIFI_POS_FAILED",
            "QAPI_LOCATION_ENGINE_STATUS_LOAD_SUCCESS",
            "QAPI_LOCATION_ENGINE_STATUS_LOAD_FAILED",
            "QAPI_LOCATION_ENGINE_STATUS_UNLOADED",
            "QAPI_LOCATION_ENGINE_STATUS_FIX_FAILED",
            "QAPI_LOCATION_ENGINE_STATUS_POS_ACC_CHECK_FAILED",
            "QAPI_LOCATION_ENGINE_STATUS_FIX_SUCCESS",
            "QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_MAINT_START",
            "QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_MAINT_SUCCESS",
            "QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_MAINT_FAILED",
            "QAPI_LOCATION_ENGINE_STATUS_GNSS_WEAK_SIGNALS",
    };

    APP_LOGM("FLAGS: %p (backoff time 0x1 / nearest geofence 0x2 / engine status 0x10)", metaData.flags);
    if (QAPI_LOCATION_META_DATA_HAS_BACKOFF_TIME & metaData.flags)
    {
        APP_LOGH("BACKOFF TIME: %d millis", metaData.backoffTimeMillis);
    }
    if (QAPI_LOCATION_META_DATA_HAS_DISTANCE_TO_NEAREST_GEOFENCE & metaData.flags)
    {
        APP_LOGH("NEAREST GEOFENCE: %d meters", metaData.nearestGeofenceMeters);
    }
    if (QAPI_LOCATION_META_DATA_HAS_ENGINE_STATUS & metaData.flags)
    {
        APP_LOGSH("ENGINE STATUS: %d [%s]", metaData.engineStatus, engineStatusArr[metaData.engineStatus]);
    }

    gnss_worker_send_msg_cb_meta_data(metaData);
}

/*---------------------------------------------------------
 * NMEA CALLBACKs
 *--------------------------------------------------------*/
void qapi_loc_wrapper_gnss_nmea_cb(qapi_Gnss_Nmea_t nmea)
{
    APP_LOG_QAPI_CALLBACK("GNSS NMEA CB");

    APP_LOGH("Timestamp: 0x%x%x", (int)(nmea.timestamp >> 32), (int)nmea.timestamp);
    APP_LOGSH("NMEA: %s", nmea.nmea);
    APP_LOGH("Source: %d", nmea.source);

    gnss_worker_send_msg_cb_nmea(nmea);
}

/*---------------------------------------------------------
 * MOTION TRACKING CALLBACKs
 *--------------------------------------------------------*/
void qapi_loc_wrapper_motion_tracking_cb(qapi_Location_Motion_Info_t info)
{
    APP_LOG_QAPI_CALLBACK("MOTION TRACKING CB");

    const char* motionType[8][50];
    uint64_t ts_sec = info.timestamp/1000;

    motionType[QAPI_LOCATION_MOTION_TYPE_REST][0]             = "QAPI_LOCATION_MOTION_TYPE_REST";
    motionType[QAPI_LOCATION_MOTION_TYPE_MOVE_ABSOLUTE][0]    = "QAPI_LOCATION_MOTION_TYPE_MOVE_ABSOLUTE";
    motionType[QAPI_LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT][0] = "QAPI_LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT";

    APP_LOGH("Time(HH:MM:SS): %02d:%02d:%02d",
            (int)((ts_sec/3600)%24), (int)((ts_sec/60)%60), (int)(ts_sec%60));

    APP_LOGSH("Motion Type %s", motionType[info.motionType][0]);

    gnss_worker_send_msg_cb_motion(info);
}

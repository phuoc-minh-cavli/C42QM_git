/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include <qapi_fs.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "app_utils_misc.h"
#include "qapi_diag.h"

/***************************************************************************
    Logging Utilities
****************************************************************************/
void app_utils_log_location(qapi_Location_t location)
{
    uint64_t ts_sec = location.timestamp / 1000;

    APP_LOGH("Time(HH:MM:SS): %02d:%02d:%02d",
        (int)((ts_sec / 3600) % 24), (int)((ts_sec / 60) % 60), (int)(ts_sec % 60));

    if (location.flags & QAPI_LOCATION_HAS_LAT_LONG_BIT)
    {
        APP_LOGH("LATITUDE: %d.%d",
            (int)location.latitude, (abs((int)(location.latitude * 100000))) % 100000);
        APP_LOGH("LONGITUDE: %d.%d",
            (int)location.longitude, (abs((int)(location.longitude * 100000))) % 100000);
    }
    else
    {
        APP_LOGH("Latitude and longitude are not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_ALTITUDE_BIT)
    {
        APP_LOGH("ALTITUDE: %d.%d",
            (int)location.altitude, (abs((int)(location.altitude * 100))) % 100);
        APP_LOGH("ALTITUDE MSL: %d.%d",
            (int)location.altitudeMeanSeaLevel, (abs((int)(location.altitudeMeanSeaLevel * 100))) % 100);
    }
    else
    {
        APP_LOGH("Altitude is not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_SPEED_BIT)
    {
        APP_LOGH("SPEED: %d.%d",
            (int)location.speed, (abs((int)(location.speed * 100))) % 100);
    }
    else
    {
        APP_LOGH("Speed is not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_BEARING_BIT)
    {
        APP_LOGH("BEARING: %d.%d",
            (int)location.bearing, (abs((int)(location.bearing * 100))) % 100);
    }
    else
    {
        APP_LOGH("Bearing is not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_ACCURACY_BIT)
    {
        APP_LOGH("ACCURACY: %d.%d",
            (int)location.accuracy, (abs((int)(location.accuracy * 100))) % 100);
    }
    else
    {
        APP_LOGH("Accuracy is not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_VERTICAL_ACCURACY_BIT)
    {
        APP_LOGH("VERTICAL ACCURACY: %d.%d",
            (int)location.verticalAccuracy, (abs((int)(location.verticalAccuracy * 100))) % 100);
    }
    else
    {
        APP_LOGH("Vertical accuracy is not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_SPEED_ACCURACY_BIT)
    {
        APP_LOGH("SPEED ACCURACY: %d.%d",
            (int)location.speedAccuracy, (abs((int)(location.speedAccuracy * 100))) % 100);
    }
    else
    {
        APP_LOGH("Speed accuracy is not provided");
    }

    if (location.flags & QAPI_LOCATION_HAS_BEARING_ACCURACY_BIT)
    {
        APP_LOGH("BEARING ACCURACY: %d.%d",
            (int)location.bearingAccuracy, (abs((int)(location.bearingAccuracy * 100))) % 100);
    }
    else
    {
        APP_LOGH("Bearing accuracy is not provided");
    }
}

void app_utils_log_location_report(qapi_Location_t location)
{
    APP_LOG_GNSS_WORKER_REPORT("TS 0x%x%x Lat %d.%d Long %d.%d Acc %d.%d",
            (int)(location.timestamp >> 32), (int)location.timestamp,
            (int)location.latitude, (abs((int)(location.latitude * 100000))) % 100000,
            (int)location.longitude, (abs((int)(location.longitude * 100000))) % 100000,
            (int)location.accuracy, (abs((int)(location.accuracy * 100))) % 100);
}

void app_utils_log_nmea(qapi_Gnss_Nmea_t nmea)
{
    APP_LOGSH("NMEA: [%s] Length: [%d]", nmea.nmea, nmea.length);
}

const char* POWER_HIGH_STR = "POWER_HIGH";
const char* POWER_MED_STR = "POWER_MED";
const char* POWER_LOW_STR = "POWER_LOW";
const char* POWER_INVALID_STR = "POWER_INVALID";
const char* app_utils_power_level_str(qapi_Location_Power_Level_t power)
{
    if (QAPI_LOCATION_POWER_HIGH == power)
    {
        return POWER_HIGH_STR;
    }
    if (QAPI_LOCATION_POWER_MED == power)
    {
        return POWER_MED_STR;
    }
    if (QAPI_LOCATION_POWER_LOW == power)
    {
        return POWER_LOW_STR;
    }
    return POWER_INVALID_STR;
}

const char* ACCURACY_HIGH_STR = "ACCURACY_HIGH";
const char* ACCURACY_MED_STR = "ACCURACY_MED";
const char* ACCURACY_LOW_STR = "ACCURACY_LOW";
const char* ACCURACY_UNKNOWN_STR = "ACCURACY_UNKNOWN";
const char* ACCURACY_INVALID_STR = "ACCURACY_INVALID";
const char* app_utils_accuracy_level_str(qapi_Location_Accuracy_Level_t accuracy)
{
    if (QAPI_LOCATION_ACCURACY_HIGH == accuracy)
    {
        return ACCURACY_HIGH_STR;
    }
    if (QAPI_LOCATION_ACCURACY_MED == accuracy)
    {
        return ACCURACY_MED_STR;
    }
    if (QAPI_LOCATION_ACCURACY_LOW == accuracy)
    {
        return ACCURACY_LOW_STR;
    }
    if (QAPI_LOCATION_ACCURACY_UNKNOWN == accuracy)
    {
        return ACCURACY_UNKNOWN_STR;
    }
    return ACCURACY_INVALID_STR;
}

static char _GnssWorkerStateArr[12][40] =
{
    "UNKNOWN",
    "IDLE",
    "WAITING_FOR_GNSS_LOAD",
    "WAITING_FOR_AD_DL_COMPLETE",
    "AD_DL_COMPLETE",
    "GNSS_LOADED",
    "BACKOFF_BAD_WWAN_CONNECTION",
    "BACKOFF_WEAK_GNSS_SIGNALS",
    "BACKOFF_WWAN_HIGH_PRIORITY",
    "GNSS_FIX_GENERATED",
    "GNSS_RETRY",
    "WAITING_FOR_NON_GNSS_POSITION",
};
const char* app_utils_gnss_worker_state_str(gnss_worker_state state)
{
    return _GnssWorkerStateArr[state];
}

void app_utils_log_engine_status_recvd(
        gnss_worker_state state,
        qapi_Location_Meta_Data_Engine_Status_t status)
{
    switch(status)
    {
    case QAPI_LOCATION_ENGINE_STATUS_UNKNOWN:
        APP_LOGEVENT("Recvd Engine Status [UNKNOWN] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_DATA_CALL_SUCCESS:
        APP_LOGEVENT("Recvd Engine Status [DATA_CALL_SUCCESS] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_DATA_CALL_FAILED:
        APP_LOGEVENT("Recvd Engine Status [DATA_CALL_FAILED] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_START:
        APP_LOGEVENT("Recvd Engine Status [XTRA_DL_START] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_SUCCESS:
        APP_LOGEVENT("Recvd Engine Status [XTRA_DL_SUCCESS] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_FAILED:
        APP_LOGEVENT("Recvd Engine Status [XTRA_DL_FAILED] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_XTRA_DL_THROTTLED:
        APP_LOGEVENT("Recvd Engine Status [XTRA_DL_THROTTLED] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_START:
        APP_LOGEVENT("Recvd Engine Status [NTP_DL_START] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_SUCCESS:
        APP_LOGEVENT("Recvd Engine Status [NTP_DL_SUCCESS] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_FAILED:
        APP_LOGEVENT("Recvd Engine Status [NTP_DL_FAILED] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_NTP_DL_THROTTLED:
        APP_LOGEVENT("Recvd Engine Status [NTP_DL_THROTTLED] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_START:
        APP_LOGEVENT("Recvd Engine Status [GTPWWAN_POS_START] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_SUCCESS:
        APP_LOGEVENT("Recvd Engine Status [GTPWWAN_POS_SUCCESS] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_POS_FAILED:
        APP_LOGEVENT("Recvd Engine Status [GTPWWAN_POS_FAILED] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_WIFI_POS_START:
        APP_LOGEVENT("Recvd Engine Status [WIFI_POS_START] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_WIFI_POS_SUCCESS:
        APP_LOGEVENT("Recvd Engine Status [WIFI_POS_SUCCESS] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_WIFI_POS_FAILED:
        APP_LOGEVENT("Recvd Engine Status [WIFI_POS_FAILED] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_LOAD_SUCCESS:
        APP_LOGEVENT("Recvd Engine Status [LOAD_SUCCESS] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_LOAD_FAILED:
        APP_LOGEVENT("Recvd Engine Status [LOAD_FAILED] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_UNLOADED:
        APP_LOGEVENT("Recvd Engine Status [UNLOADED] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_FIX_FAILED:
        APP_LOGEVENT("Recvd Engine Status [FIX_FAILED] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_POS_ACC_CHECK_FAILED:
        APP_LOGEVENT("Recvd Engine Status [POS_ACC_CHECK_FAILED] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_FIX_SUCCESS:
        APP_LOGEVENT("Recvd Engine Status [FIX_SUCCESS] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_MAINT_START:
        APP_LOGEVENT("Recvd Engine Status [GTPWWAN_MAINT_START] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_MAINT_SUCCESS:
        APP_LOGEVENT("Recvd Engine Status [GTPWWAN_MAINT_SUCCESS] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_GTPWWAN_MAINT_FAILED:
        APP_LOGEVENT("Recvd Engine Status [GTPWWAN_MAINT_FAILED] (%d) in state (%d)", status, state);
        break;
    case QAPI_LOCATION_ENGINE_STATUS_WEAK_GNSS_SIGNALS:
        APP_LOGEVENT("Recvd Engine Status [WEAK_GNSS_SIGNALS] (%d) in state (%d)", status, state);
        break;
    default:
        APP_LOGEVENT("Recvd Engine Status [INVALID] (%d) in state (%d)", status, state);
        APP_LOGE("Invalid engine status received");
        break;
    }
}

/***************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
****************************************************************************/
#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "app_types.h"

// CONFIG FILE PATH
#define APP_CONF_FILE_PATH "/concurrency_demo/app.conf"

// DEFAULT VALUES FOR CONFIG ITEMS
#define APP_CFG_DEF_BOOTUP_DELAY_SEC                       20
#define APP_CFG_DEF_LOCATION_REPORTING_INTERVAL_SEC        60
#define APP_CFG_DEF_LOCATION_REPORT_COUNT                  10
#define APP_CFG_DEF_MOTION_DETECTION_ENABLED               1
#define APP_CFG_DEF_LOCATION_SESSION_MAX_POWER_LEVEL       1
#define APP_CFG_DEF_LOCATION_SESSION_MIN_ACCURACY_LEVEL    1
#define APP_CFG_DEF_ALLOW_SYSTEM_PRIORITY_INVERSION        1
#define APP_CFG_DEF_LOCATION_MODE                          0
#define APP_CFG_DEF_TRACKING_TBF_MILLIS                    15000
#define APP_CFG_DEF_TRACKING_ACCURACY                      3
#define APP_CFG_DEF_TRACKING_DURATION_SEC                  0
#define APP_CFG_DEF_GEOFENCE_LATITUDE                      17.4398
#define APP_CFG_DEF_GEOFENCE_LONGITUDE                     78.3803
#define APP_CFG_DEF_GEOFENCE_RADIUS_METERS                 100
#define APP_CFG_DEF_GEOFENCE_MASK                          15
#define APP_CFG_DEF_GEOFENCE_RESPONSIVENESS_MILLIS         1000
#define APP_CFG_DEF_GEOFENCE_DWELL_MILLIS                  30000
#define APP_CFG_DEF_WWAN_SLEEP_EVENT_ENABLED               0

typedef struct {

    // Is the config initialized
    uint8 initialized;

    // Bootup wait time before starting app logic
    // Mainly used to provide time for logging init
    uint32 BOOTUP_DELAY_SEC;

    // Recurring interval in seconds at which the application
    // needs to report a location fix.
    uint32 LOCATION_REPORTING_INTERVAL_SEC;

    // Number of location reports to be generated
    uint32 LOCATION_REPORT_COUNT;

    // Maximum power level each location session can use.
    // 0 = High
    // 1 = Low
    // 2 = Medium
    uint32 LOCATION_SESSION_MAX_POWER_LEVEL;

    // Minimum accuracy level each location session can use.
    // 1 = Low
    // 2 = Medium
    // 3 = High
    uint32 LOCATION_SESSION_MIN_ACCURACY_LEVEL;

    // Is the application allowed to perform system priority
    // inversion ?
    // 0 = No
    // 1 = Yes
    uint32 ALLOW_SYSTEM_PRIORITY_INVERSION;

    // What mode of location reporting do we wish to try
    // 0 = Singleshot
    // 1 = Tracking
    // 2 = Geofence
    uint32 LOCATION_MODE;

    // For time based tracking, time interval between fixes
    // Specified in Milliseconds
    uint32 TRACKING_TBF_MILLIS;

    // Accuracy for the tracking session
    // 1 = Low
    // 2 = Medium
    // 3 = High
    uint32 TRACKING_ACCURACY;

    // How long to run the tracking session
    // Value 0 indicates to run the session indefinitely
    // Specified in seconds
    uint32 TRACKING_DURATION_SEC;

    // Geofence parameters
    double GEOFENCE_LATITUDE;
    double GEOFENCE_LONGITUDE;
    double GEOFENCE_RADIUS_METERS;

    // Mask value for geofence with below bits
    // BREACH_ENTER = 1
    // BREACH_EXIT = 2
    // BREACH_DWELL_IN = 4
    // BREACH_DWELL_OUT = 8
    uint32 GEOFENCE_MASK;

    uint32 GEOFENCE_RESPONSIVENESS_MILLIS;

    uint32 GEOFENCE_DWELL_MILLIS;

    // Is WWAN Sleep Event Handling Enabled ?
    // 0 = No
    // 1 = Yes
    uint32 WWAN_SLEEP_EVENT_ENABLED;

} AppConfig;

// Parse the config file, and populate the global config struct
int app_read_config();

void app_print_config();

// The config global which any file can refer to
extern AppConfig APP_CONFIG;

#endif /* __APP_CONFIG_H__ */

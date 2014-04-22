/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include "app_cfg.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <qapi_fs.h>

#include "app_utils_misc.h"
#include "qapi_diag.h"

// Global config struct
AppConfig APP_CONFIG = {};

/* Function to get a line from the config file */
#define LINE_SIZE 128
static int _fgetline(char* s, int fd)
{
    char file_contents[LINE_SIZE] = "";
    int ch;
    int  read_length = 0;
    int i;

    qapi_FS_Read(fd, file_contents, LINE_SIZE, &read_length);

    if (read_length != 0)
    {
        for (i = 0; i < read_length; i++)
        {
            ch = *(file_contents + i);
            *(s + i) = ch;
            if ('\n' == ch || EOF == ch)
            {
                *(s + i) = '\0';
                return i;
            }
        }
        *(s + i) = '\0';
        return i;
    }
    return -1;
}

// Parsing config file
#define IS_WHITESPACE(c) (c == ' ' || c == '\t' || c == '\r' || c == '\n')
#define TRIM_WHITESPACES(ptr, maxPtr) \
    while(IS_WHITESPACE(*ptr) && ptr < maxPtr) \
    { \
        ptr++; \
    }
#define TRIM_WHITESPACES_TRAILING(ptr, minPtr) \
    while(IS_WHITESPACE(*ptr) && ptr > minPtr) \
    { \
        ptr--; \
    }
#define KEY_VALUE_SEPARATOR '='

static void _parse_config_line(char* line, uint32_t lineLen)
{
    char* keyStartPtr = line;
    char* keyEndPtr = NULL;
    char* valueStartPtr = NULL;
    char* valueEndPtr = NULL;
    char* currentPtr = line;
    const char* endPtr = line + lineLen - 1;
    uint32_t keyLen = 0, valueLen = 0;

    char key[128];
    char value[128];

    // Skip whitespaces
    TRIM_WHITESPACES(keyStartPtr, endPtr);

    // Ignore comment line (starting with #)
    if (*keyStartPtr == '#')
    {
        return;
    }

    // look for key value separator
    currentPtr = strchr(keyStartPtr, KEY_VALUE_SEPARATOR);
    if (currentPtr == NULL)
    {
        APP_LOGH("Invalid config file format !");
        return;
    }

    keyEndPtr = currentPtr - 1;

    // Trim any trailing whitespaces
    TRIM_WHITESPACES_TRAILING(keyEndPtr, keyStartPtr);

    keyLen = keyEndPtr - keyStartPtr + 1;

    memset(key, 0, sizeof(key));
    memcpy(key, keyStartPtr, keyLen);

    // Key found, now look for value
    valueStartPtr = currentPtr + 1; // Right after the key value separator char

    // trim whitespaces
    TRIM_WHITESPACES(valueStartPtr, endPtr);

    // Look for value end
    valueEndPtr = (char*)endPtr;

    // Trim any trailing whitespaces
    TRIM_WHITESPACES_TRAILING(valueEndPtr, valueStartPtr);

    // We have value start and value end, update the value string
    valueLen = valueEndPtr - valueStartPtr + 1;

    memset(value, 0, sizeof(value));
    memcpy(value, valueStartPtr, valueLen);

    // process key/value pair
    if (0 == strcmp(key, "BOOTUP_DELAY_SEC"))
    {
        APP_CONFIG.BOOTUP_DELAY_SEC = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "LOCATION_REPORTING_INTERVAL_SEC"))
    {
        APP_CONFIG.LOCATION_REPORTING_INTERVAL_SEC = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "LOCATION_REPORT_COUNT"))
    {
        APP_CONFIG.LOCATION_REPORT_COUNT = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "LOCATION_SESSION_MAX_POWER_LEVEL"))
    {
        APP_CONFIG.LOCATION_SESSION_MAX_POWER_LEVEL = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "LOCATION_SESSION_MIN_ACCURACY_LEVEL"))
    {
        APP_CONFIG.LOCATION_SESSION_MIN_ACCURACY_LEVEL = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "ALLOW_SYSTEM_PRIORITY_INVERSION"))
    {
        APP_CONFIG.ALLOW_SYSTEM_PRIORITY_INVERSION = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "LOCATION_MODE"))
    {
        APP_CONFIG.LOCATION_MODE = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "TRACKING_TBF_MILLIS"))
    {
        APP_CONFIG.TRACKING_TBF_MILLIS = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "TRACKING_ACCURACY"))
    {
        APP_CONFIG.TRACKING_ACCURACY = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "TRACKING_DURATION_SEC"))
    {
        APP_CONFIG.TRACKING_DURATION_SEC = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "GEOFENCE_LATITUDE"))
    {
        APP_CONFIG.GEOFENCE_LATITUDE = qapi_loc_util_string_to_double(value, 0);
    }
    else if (0 == strcmp(key, "GEOFENCE_LONGITUDE"))
    {
        APP_CONFIG.GEOFENCE_LONGITUDE = qapi_loc_util_string_to_double(value, 0);
    }
    else if (0 == strcmp(key, "GEOFENCE_RADIUS_METERS"))
    {
        APP_CONFIG.GEOFENCE_RADIUS_METERS = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "GEOFENCE_MASK"))
    {
        APP_CONFIG.GEOFENCE_MASK = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "GEOFENCE_RESPONSIVENESS_MILLIS"))
    {
        APP_CONFIG.GEOFENCE_RESPONSIVENESS_MILLIS = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "GEOFENCE_DWELL_MILLIS"))
    {
        APP_CONFIG.GEOFENCE_DWELL_MILLIS = app_utils_string_to_uint64(value, 0);
    }
    else if (0 == strcmp(key, "WWAN_SLEEP_EVENT_ENABLED"))
    {
        APP_CONFIG.WWAN_SLEEP_EVENT_ENABLED = app_utils_string_to_uint64(value, 0);
    }
}

static void _app_cfg_set_defaults()
{
    APP_CONFIG.BOOTUP_DELAY_SEC = APP_CFG_DEF_BOOTUP_DELAY_SEC;
    APP_CONFIG.LOCATION_REPORTING_INTERVAL_SEC = APP_CFG_DEF_LOCATION_REPORTING_INTERVAL_SEC;
    APP_CONFIG.LOCATION_REPORT_COUNT = APP_CFG_DEF_LOCATION_REPORT_COUNT;
    APP_CONFIG.LOCATION_SESSION_MAX_POWER_LEVEL = APP_CFG_DEF_LOCATION_SESSION_MAX_POWER_LEVEL;
    APP_CONFIG.LOCATION_SESSION_MIN_ACCURACY_LEVEL = APP_CFG_DEF_LOCATION_SESSION_MIN_ACCURACY_LEVEL;
    APP_CONFIG.ALLOW_SYSTEM_PRIORITY_INVERSION = APP_CFG_DEF_ALLOW_SYSTEM_PRIORITY_INVERSION;
    APP_CONFIG.LOCATION_MODE = APP_CFG_DEF_LOCATION_MODE;
    APP_CONFIG.TRACKING_TBF_MILLIS = APP_CFG_DEF_TRACKING_TBF_MILLIS;
    APP_CONFIG.TRACKING_ACCURACY = APP_CFG_DEF_TRACKING_ACCURACY;
    APP_CONFIG.TRACKING_DURATION_SEC = APP_CFG_DEF_TRACKING_DURATION_SEC;
    APP_CONFIG.GEOFENCE_LATITUDE = APP_CFG_DEF_GEOFENCE_LATITUDE;
    APP_CONFIG.GEOFENCE_LONGITUDE = APP_CFG_DEF_GEOFENCE_LONGITUDE;
    APP_CONFIG.GEOFENCE_RADIUS_METERS = APP_CFG_DEF_GEOFENCE_RADIUS_METERS;
    APP_CONFIG.GEOFENCE_MASK = APP_CFG_DEF_GEOFENCE_MASK;
    APP_CONFIG.GEOFENCE_RESPONSIVENESS_MILLIS = APP_CFG_DEF_GEOFENCE_RESPONSIVENESS_MILLIS;
    APP_CONFIG.GEOFENCE_DWELL_MILLIS = APP_CFG_DEF_GEOFENCE_DWELL_MILLIS;
    APP_CONFIG.WWAN_SLEEP_EVENT_ENABLED = APP_CFG_DEF_WWAN_SLEEP_EVENT_ENABLED;
}

void app_print_config()
{
    APP_LOGH("BOOTUP_DELAY_SEC = %d", APP_CONFIG.BOOTUP_DELAY_SEC);
    APP_LOGH("LOCATION_REPORTING_INTERVAL_SEC = %d", APP_CONFIG.LOCATION_REPORTING_INTERVAL_SEC);
    APP_LOGH("LOCATION_REPORT_COUNT = %d", APP_CONFIG.LOCATION_REPORT_COUNT);
    APP_LOGH("LOCATION_SESSION_MAX_POWER_LEVEL = %d", APP_CONFIG.LOCATION_SESSION_MAX_POWER_LEVEL);
    APP_LOGH("LOCATION_SESSION_MIN_ACCURACY_LEVEL = %d", APP_CONFIG.LOCATION_SESSION_MIN_ACCURACY_LEVEL);
    APP_LOGH("ALLOW_SYSTEM_PRIORITY_INVERSION = %d", APP_CONFIG.ALLOW_SYSTEM_PRIORITY_INVERSION);
    APP_LOGH("LOCATION_MODE = %d", APP_CONFIG.LOCATION_MODE);
    APP_LOGH("TRACKING_TBF_MILLIS = %d", APP_CONFIG.TRACKING_TBF_MILLIS);
    APP_LOGH("TRACKING_ACCURACY = %d", APP_CONFIG.TRACKING_ACCURACY);
    APP_LOGH("TRACKING_DURATION_SEC = %d", APP_CONFIG.TRACKING_DURATION_SEC);
    APP_LOGH("GEOFENCE_LATITUDE = %d.%d",
            (int)APP_CONFIG.GEOFENCE_LATITUDE, (abs((int)(APP_CONFIG.GEOFENCE_LATITUDE * 100000))) % 100000);
    APP_LOGH("GEOFENCE_LONGITUDE = %d.%d",
            (int)APP_CONFIG.GEOFENCE_LONGITUDE, (abs((int)(APP_CONFIG.GEOFENCE_LONGITUDE * 100000))) % 100000);
    APP_LOGH("GEOFENCE_RADIUS_METERS = %d", APP_CONFIG.GEOFENCE_RADIUS_METERS);
    APP_LOGH("GEOFENCE_MASK = %d", APP_CONFIG.GEOFENCE_MASK);
    APP_LOGH("GEOFENCE_RESPONSIVENESS_MILLIS = %d", APP_CONFIG.GEOFENCE_RESPONSIVENESS_MILLIS);
    APP_LOGH("GEOFENCE_DWELL_MILLIS = %d", APP_CONFIG.GEOFENCE_DWELL_MILLIS);
    APP_LOGH("WWAN_SLEEP_EVENT_ENABLED = %d", APP_CONFIG.WWAN_SLEEP_EVENT_ENABLED);
}

int app_read_config()
{
    int fd = -1;
    char line[LINE_SIZE];
    uint32 offset;
    int retvalue;
    qapi_FS_Offset_t seekStatus = 0;
    const char* confFilePath = APP_CONF_FILE_PATH;

    // Set Defaults
    _app_cfg_set_defaults();

    // Read from config file
    qapi_FS_Status_t ret = qapi_FS_Open(confFilePath, QAPI_FS_O_RDONLY_E, &fd);
    if (fd >= 0)
    {
        offset = 0;

        do
        {
            qapi_FS_Seek(fd, offset, QAPI_FS_SEEK_SET_E, &seekStatus);
            memset(line, 0, sizeof(line));
            retvalue = _fgetline(line, fd);
            if (retvalue > 0)
            {
                APP_LOGSH("conf line = [%s] len = %d", line, retvalue);
                offset += (retvalue + 1);
                _parse_config_line(line, retvalue);
            }
        } while (retvalue > 0);

        qapi_FS_Close(fd);

        APP_CONFIG.initialized = 1;

        return 0;
    }
    else
    {
        APP_LOGSE("Failed to read conf file [%s] ret %d", confFilePath, ret);
        return -1;
    }
}

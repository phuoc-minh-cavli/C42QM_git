/******************************************************************************
 * LOC OS IFACE . H
 * Interface to the OS specific calls for use in Location code.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#ifndef __LOC_OS_IFACE_H__
#define __LOC_OS_IFACE_H__

#include <comdef.h>
#include <stdlib.h>
#include <stdint.h>
#include <platform_lib_time.h>
#include "LocationAPI.h"
#include "time_types.h"

typedef int (*xtraGetGnssTimeCb)(void *data, GnssTime& time);

typedef enum
{
    LOC_OS_IFACE_TIME_SOURCE_INVALID = 0,
    LOC_OS_IFACE_TIME_SOURCE_GPS,
    LOC_OS_IFACE_TIME_SOURCE_NTP
} LocOSIface_time_source;

/* LocOSIface_Julian_Time_t
 * Represents current time in Julian format.
 * */
typedef struct
{
  uint16 year;   /**< Year [1980 through 2100]. */
  uint16 month;  /**< Month of year [1 through 12]. */
  uint16 day;    /**< Day of month [1 through 31]. */
  uint16 hour;   /**< Hour of day [0 through 23]. */
  uint16 minute; /**< Minute of hour [0 through 59]. */
  uint16 second; /**< Second of minute [0 through 59]. */
  uint16 day_of_week; /**< Day of the week [0 through 6] or [Monday through
                           Sunday]. */
} LocOSIface_Julian_Time_t;

/* Function: LocOSIface_gettimeofday()
 *
 * Fetch the current time, populate the timeval struct pointer.
 * The time corresponds to millis elapsed since Epoch.
 *
 * timeZoneInfoPtr is currently unused, just pass in NULL
 *
 * Returns 0 for success, -1 for failure.
 * */
int LocOSIface_gettimeofday(timeval* timeOfDayPtr, void* timeZoneInfoPtr, bool bFetchGnssTime = false);

/* Function: LocOSIface_getMillisSincePowerup()
 *
 * Returns the time as millis elapsed since device boot up.
 *
 * Returns : millis since device bootup
 * */
uint64 LocOSIface_getMillisSincePowerup();

/* Macro: MIN_MILLIS_SINCE_EPOCH
 * Used to estimate if the millis since epoch value is valid or not
 * */
#define MIN_MILLIS_SINCE_EPOCH (uint64)(315360000)  // ~ 10 years

/* Function: LocOSIface_getMillisUTC()
 *
 * Returns the time as millis elapsed since UTC epoch.
 *
 * Returns : millis since UTC epoch - Jan 01, 1970
 * */
uint64 LocOSIface_getMillisUTC();

/* Function: LocOSIface_getMillisSinceEpoch()
 *
 * Returns the time as millis elapsed since GPS epoch.
 *
 * Returns : millis since GPS epoch - Jan 06, 1980
 * */
uint64 LocOSIface_getMillisSinceEpoch(bool bFetchGnssTime = false);

/* Function: LocOSIface_getSecondsSinceEpoch()
 *
 * Returns the time as seconds elapsed since epoch.
 *
 * Returns : seconds since epoch
 * */
uint64 LocOSIface_getSecondsSinceEpoch(bool bFetchGnssTime = false);

/* Function: LocOSIface_getCurrentJulianTime()
 *
 * Returns the current time in Julian format.
 *
 * Returns : Julian time via passed in pointer.
 * */
void LocOSIface_getCurrentJulianTime(LocOSIface_Julian_Time_t* timePtr);

/* Function: convert_UTC_seconds_to_julian()
 *
 * Returns the current time in Julian format.
 *
 * Returns : Julian time via passed in pointer.
 * */
void convert_UTC_seconds_to_julian(LocOSIface_Julian_Time_t* timePtr, uint64 secs);

/* Function: LocOSIface_isValidTimeAvailable()
 *
 * Tries to guess if valid time is available.
 *
 * Returns : true if valid time available, else false.
 * */
bool LocOSIface_isValidTimeAvailable(bool bFetchGnssTime = false);

/* Function: LocOSIface_getGid()
 *
 * Fetch the group ID for the specified group name
 *
 * Returns the integer value of the group id, or -1 in case of failure.
 * */
int LocOSIface_getGid(
        const char* grpNamePtr, size_t grpNameSize);

/* Function: LocOSIface_getUid()
 *
 * Fetch the user id of the calling process
 *
 * Returns the integer value of the user id, or -1 in case of failure.
 * */
int LocOSIface_getUid();

/* Function: LocOSIface_generateRand64()
 *
 * Uses time as seed and generates a random 64 bit number
 *
 * Returns a 64 bit unsigned random value.
 * */
uint64 LocOSIface_generateRand64();

/* Function: LocOSIface_getProcessName()
 *
 * Returns the name of the current process.
 *
 * Populates the passed in buffer with process name.
 * */
void LocOSIface_getProcessName(char* buffer, uint8 bufferLen);

/* Function: LocOSIface_regGetTimeCb()
 *
 * Set the getTime callback function passed by XTRA module.
 *
 * */
void LocOSIface_regGetTimeCb(void *contextData, xtraGetGnssTimeCb timeCb);

/* Function: LocOSIface_injectTime()
 *
 * Imput Param:
 *      timeMsecs: Time to be injected
 *      timeSource: Source of the time eg. GPS, UTC. Needed to identify time base
 * Returns void
 * */
void LocOSIface_injectTime(uint64_t timeMsecs, LocOSIface_time_source timeSource) ;

#endif // __LOC_OS_IFACE_H__

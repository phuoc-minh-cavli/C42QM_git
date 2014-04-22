/* Copyright (c) 2017-2018 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LOCATIONAPI_H
#define LOCATIONAPI_H

#include <stdint.h>
#include <string.h>

typedef enum {
    LOCATION_TECHNOLOGY_TYPE_GNSS = 0,
    LOCATION_TECHNOLOGY_TYPE_GTP_WWAN = 1
} LocationTechnologyType;

typedef enum {
    LOCATION_ERROR_SUCCESS = 0,
    LOCATION_ERROR_GENERAL_FAILURE,
    LOCATION_ERROR_CALLBACK_MISSING,
    LOCATION_ERROR_INVALID_PARAMETER,
    LOCATION_ERROR_ID_EXISTS,
    LOCATION_ERROR_ID_UNKNOWN,
    LOCATION_ERROR_ALREADY_STARTED,
    LOCATION_ERROR_GEOFENCES_AT_MAX,
    LOCATION_ERROR_NOT_SUPPORTED,
    LOCATION_ERROR_TIMEOUT,
    LOCATION_ERROR_DDR_UNAVAILABLE,
    LOCATION_ERROR_LOCATION_DISABLED,
    LOCATION_ERROR_BEST_AVAIL_POS_INVALID,
    LOCATION_ERROR_TIMEOUT_ACC_CHECK_FAILED,
    LOCATION_ERROR_THRESHOLD_REACHED,
    LOCATION_ERROR_XTRA_DISABLED
} LocationError;

// Flags to indicate which values are valid in a Location
typedef uint16_t LocationFlagsMask;
typedef enum {
    LOCATION_HAS_LAT_LONG_BIT          = (1<<0), // location has valid latitude and longitude
    LOCATION_HAS_ALTITUDE_BIT          = (1<<1), // location has valid altitude
    LOCATION_HAS_SPEED_BIT             = (1<<2), // location has valid speed
    LOCATION_HAS_BEARING_BIT           = (1<<3), // location has valid bearing
    LOCATION_HAS_ACCURACY_BIT          = (1<<4), // location has valid accuracy
    LOCATION_HAS_VERTICAL_ACCURACY_BIT = (1<<5), // location has valid vertical accuracy
    LOCATION_HAS_SPEED_ACCURACY_BIT    = (1<<6), // location has valid speed accuracy
    LOCATION_HAS_BEARING_ACCURACY_BIT  = (1<<7), // location has valid bearing accuracy
    LOCATION_HAS_ALTITUDE_MSL_BIT      = (1<<8), // location has valid altitude wrt mean sea level
    LOCATION_IS_BEST_AVAIL_POS_BIT     = (1<<9), // location is the currently best available position
} LocationFlagsBits;

typedef enum {
    GNSS_SV_TYPE_UNKNOWN = 0,
    GNSS_SV_TYPE_GPS,
    GNSS_SV_TYPE_SBAS,
    GNSS_SV_TYPE_GLONASS,
    GNSS_SV_TYPE_QZSS,
    GNSS_SV_TYPE_BEIDOU,
    GNSS_SV_TYPE_GALILEO,
    MAX_NUMBER_OF_CONSTELLATIONS
} GnssSvType;

typedef enum {
    GEOFENCE_BREACH_ENTER = 0,
    GEOFENCE_BREACH_EXIT,
    GEOFENCE_BREACH_DWELL_IN,
    GEOFENCE_BREACH_DWELL_OUT,
    GEOFENCE_BREACH_UNKNOWN,
} GeofenceBreachType;

typedef uint16_t GeofenceBreachTypeMask;
typedef enum {
    GEOFENCE_BREACH_ENTER_BIT     = (1<<0),
    GEOFENCE_BREACH_EXIT_BIT      = (1<<1),
    GEOFENCE_BREACH_DWELL_IN_BIT  = (1<<2),
    GEOFENCE_BREACH_DWELL_OUT_BIT = (1<<3),
} GeofenceBreachTypeBits;

typedef uint32_t LocationCapabilitiesMask;
typedef enum {
    // supports startTracking API with minInterval param
    LOCATION_CAPABILITIES_TIME_BASED_TRACKING_BIT           = (1<<0),
    // supports startBatching API with minInterval param
    LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT           = (1<<1),
    // supports startTracking API with minDistance param
    LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT       = (1<<2),
    // supports startBatching API with minDistance param
    LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT       = (1<<3),
    // supports addGeofences API
    LOCATION_CAPABILITIES_GEOFENCE_BIT                      = (1<<4),
    // supports startGetGnssData API
    LOCATION_CAPABILITIES_GNSS_DATA_BIT                     = (1<<5),
} LocationCapabilitiesBits;

typedef enum {
    LOCATION_POWER_HIGH = 0, // Use all technologies available to calculate location
    LOCATION_POWER_LOW,      // Use all low power technologies to calculate location
    LOCATION_POWER_MED,      // Use only low and medium power technologies to calculate location
} LocationPowerLevel;

typedef enum {
    LOCATION_ACCURACY_UNKNOWN = 0,     /* Accuracy is not specified, use default. */
    LOCATION_ACCURACY_LOW,             /* Low Accuracy for location is acceptable. */
    LOCATION_ACCURACY_MED,             /* Medium Accuracy for location is acceptable. */
    LOCATION_ACCURACY_HIGH,            /* Only High Accuracy for location is acceptable. */
} LocationAccuracyLevel;

// Flags to indicate which values are valid in a LocationMetaData object
typedef uint32_t LocationMetaDataFlagsMask;
typedef enum {
    LOCATION_META_DATA_HAS_BACKOFF_TIME                  = (1<<0),
    LOCATION_META_DATA_HAS_DISTANCE_TO_NEAREST_GEOFENCE  = (1<<1),
    LOCATION_META_DATA_ENERGY_CONSUMED_SINCE_LAST_BOOT   = (1<<2),
    LOCATION_META_DATA_ENERGY_CONSUMED_SINCE_FIRST_BOOT  = (1<<3),
    LOCATION_META_DATA_ENGINE_STATUS                     = (1<<4),
} LocationMetaDataFlagsBits;

typedef enum {
    LOCATION_ENGINE_STATUS_UNKNOWN = 0,
    LOCATION_ENGINE_STATUS_DATA_CALL_SUCCESS,
    LOCATION_ENGINE_STATUS_DATA_CALL_FAILED,
    LOCATION_ENGINE_STATUS_XTRA_DL_START,
    LOCATION_ENGINE_STATUS_XTRA_DL_SUCCESS,
    LOCATION_ENGINE_STATUS_XTRA_DL_FAILED,
    LOCATION_ENGINE_STATUS_XTRA_DL_THROTTLED,
    LOCATION_ENGINE_STATUS_NTP_DL_START,
    LOCATION_ENGINE_STATUS_NTP_DL_SUCCESS,
    LOCATION_ENGINE_STATUS_NTP_DL_FAILED,
    LOCATION_ENGINE_STATUS_NTP_DL_THROTTLED,
    LOCATION_ENGINE_STATUS_GTPWWAN_POS_START,
    LOCATION_ENGINE_STATUS_GTPWWAN_POS_SUCCESS,
    LOCATION_ENGINE_STATUS_GTPWWAN_POS_FAILED,
    LOCATION_ENGINE_STATUS_WIFI_POS_START,
    LOCATION_ENGINE_STATUS_WIFI_POS_SUCCESS,
    LOCATION_ENGINE_STATUS_WIFI_POS_FAILED,
    LOCATION_ENGINE_STATUS_LOAD_SUCCESS,
    LOCATION_ENGINE_STATUS_LOAD_FAILED,
    LOCATION_ENGINE_STATUS_UNLOADED,
    LOCATION_ENGINE_STATUS_FIX_FAILED,
    LOCATION_ENGINE_STATUS_POS_ACC_CHECK_FAILED,
    LOCATION_ENGINE_STATUS_FIX_SUCCESS,
    LOCATION_ENGINE_STATUS_GTPWWAN_MAINT_START,
    LOCATION_ENGINE_STATUS_GTPWWAN_MAINT_SUCCESS,
    LOCATION_ENGINE_STATUS_GTPWWAN_MAINT_FAILED,
    LOCATION_ENGINE_STATUS_WEAK_GNSS_SIGNALS
} LocationMetaDataEngineStatus;

typedef struct {
    size_t size;             // set to sizeof(Location)
    LocationFlagsMask flags; // bitwise OR of LocationFlagsBits to mark which params are valid
    uint64_t timestamp;      // UTC timestamp for location fix, milliseconds since January 1, 1970
    double latitude;         // in degrees
    double longitude;        // in degrees
    double altitude;         // in meters above the WGS 84 reference ellipsoid
    double altitudeMeanSeaLevel;  // in meters above mean sea level
    float speed;             // in meters per second
    float bearing;           // in degrees; range [0, 360)
    float accuracy;          // in meters
    float verticalAccuracy;  // in meters
    float speedAccuracy;     // in meters/second
    float bearingAccuracy;   // in degrees (0 to 359.999)
    uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
    uint16_t gpsWeek;
    /**<   Current GPS week as calculated from midnight, Jan. 6, 1980. \n
     * - Units: Weeks */
    uint32_t gpsTimeOfWeekMs;
    /**<   Amount of time into the current GPS week. \n$
           - Units: Milliseconds */
} Location;

typedef struct {
    size_t size;                                        // Size. Set to the sizeof(GnssData)
    uint32_t jammerInd[MAX_NUMBER_OF_CONSTELLATIONS];   // Jammer Indication
} GnssData;

typedef struct {
    size_t size;                     // Size. Set to the sizeof(GnssData)
    LocationMetaDataFlagsMask flags; // bitwise OR of LocationMetaDataFlagsBits
    uint32_t backoffTimeMillis;      // Backoff value in milliseconds
    uint32_t nearestGeofenceMeters;  // UE distance to nearest geofence in meters
    uint64_t energyConsumedSinceLastBoot;   // Energy consumed by the GNSS engine since
                                            // bootup in units of 0.1 milli watt seconds
    uint64_t energyConsumedSinceFirstBoot;  // Energy consumed by the GNSS engine since
                                            // the first bootup in units of 0.1 milli watt seconds.
    LocationMetaDataEngineStatus engineStatus; // Location Engine status
} LocationMetaData;

typedef struct  {
    size_t size;          // set to sizeof(LocationOptions)
    uint32_t minInterval; // in milliseconds
    uint32_t minDistance; // in meters. if minDistance > 0, gnssSvCallback/gnssNmeaCallback/
                          // gnssMeasurementsCallback may not be called
    uint8_t suplMode;     // Standalone:0, MSB:1
    LocationAccuracyLevel accuracyLevel;  // accuracy level for tracking session
} LocationOptions;

typedef struct {
    size_t size;               // set to sizeof(GtpOptions)
    uint32_t  timeout;         
    uint8_t  requestTag[5];    // Tag to identify a GTP MCS request
} GtpOptions;

typedef struct  {
    size_t    size;            // set to sizeof(SingleShotOptions)
    uint32_t  timeout;         // Timeout (millis) for singleshot fix
    LocationPowerLevel powerLevel;  // Power level to indicate tech to be used
    LocationAccuracyLevel accuracyLevel;  // Acceptable accuracy level for reports
    GtpOptions gtpOptions;     // GTP MCS Positioning Options
} SingleShotOptions;

typedef uint32_t LocationMotionTypeMask;
typedef enum {
    LOCATION_MOTION_TYPE_REST =              (1 << 0),   // Device at rest
    LOCATION_MOTION_TYPE_MOVE_ABSOLUTE =     (1 << 1),   // Device in motion
    LOCATION_MOTION_TYPE_MOVE_SIGNIFICANT =  (1 << 2),   // Device in significant motion
} LocationMotionType;

typedef struct  {
    size_t size;  // set to sizeof(LocationMotionOptions)
    LocationMotionTypeMask motionTypeMask;  // Types of motion of interest
} LocationMotionOptions;

typedef struct {
    size_t size;                    // set to sizeof(LocationMotionInfo)
    LocationMotionType motionType;  // Type of motion detected.
    uint64_t timestamp;             // Timestamp of the motion event;
                                    // milliseconds since Jan. 1, 1970.
} LocationMotionInfo;

typedef struct {
    size_t size;                            // set to sizeof(GeofenceOption)
    GeofenceBreachTypeMask breachTypeMask;  // bitwise OR of GeofenceBreachTypeBits
    uint32_t responsiveness;                // in milliseconds
    uint32_t dwellTime;                     // in seconds
} GeofenceOption;

typedef struct  {
    size_t size;      // set to sizeof(GeofenceInfo)
    double latitude;  // in degrees
    double longitude; // in degrees
    double radius;    // in meters
} GeofenceInfo;

typedef struct {
    size_t size;             // set to sizeof(GeofenceBreachNotification)
    size_t count;            // number of ids in array
    uint32_t* ids;           // array of ids that have breached
    Location location;       // location associated with breach
    GeofenceBreachType type; // type of breach
    uint64_t timestamp;      // timestamp of breach
} GeofenceBreachNotification;

/* Source of the NMEA string, Whether Final position, Intermediate position or SV Info*/
typedef enum {
    LOCATION_NMEA_SOURCE_UNKNOWN = 0,
    LOCATION_NMEA_SOURCE_POSITION_FINAL,
    LOCATION_NMEA_SOURCE_POSITION_INTERMEDIATE,
    LOCATION_NMEA_SOURCE_SV_INFO,
    LOCATION_NMEA_SOURCE_GNSS_ENGINE,
}NmeaSource;

typedef struct {
    size_t size;         // set to sizeof(GnssNmeaNotification)
    uint64_t timestamp;  // timestamp
    const char* nmea;    // nmea text
    size_t length;       // length of the nmea text
    NmeaSource source;   /* NMEA Source*/
} GnssNmeaNotification;

// Configures how GPS is locked when GPS is disabled (through GnssDisable)
typedef enum {
    GNSS_CONFIG_GPS_LOCK_NONE = 0, // gps is not locked when GPS is disabled (GnssDisable)
    GNSS_CONFIG_GPS_LOCK_MO,       // gps mobile originated (MO) is locked when GPS is disabled
    GNSS_CONFIG_GPS_LOCK_NI,       // gps network initiated (NI) is locked when GPS is disabled
    GNSS_CONFIG_GPS_LOCK_MO_AND_NI,// gps MO and NI is locked when GPS is disabled
} GnssConfigGpsLock;

typedef struct {
    uint32_t  locationEnabled;
    uint32_t  batchSize;
    uint32_t  batchingTimeout;
    uint32_t  accuracy;
    uint32_t  mode;
    uint32_t  intermediatePos;
    uint32_t  singleShotTimeout;
    uint32_t  wwanFixAccuracyThreshold;
    uint32_t  nmeaProvider;
    uint32_t  batchAllPos;
    uint32_t  xtraIntDloadWaitTime;
    uint32_t  xtraIntDloadRetryLimit;
    uint32_t  noZppBeforeWwan;
    uint32_t  disableNmeaTimeCheck;
    uint32_t  nmeaForIntermediatePos;
    uint32_t  lowAccuracyThreshold;
    uint32_t  medAccuracyThreshold;
    uint32_t  highAccuracyThreshold;
    uint32_t  maxEfsLogCount;
    uint32_t  gtpMcsPositioningEnabled;
    uint32_t  wifiPositioningEnabled;
} LocationConfig;

typedef struct {
    uint32_t gpsWeek;
    uint32_t gpsTimeOfWeekMs;
    uint32_t leapSecondsCurrent;
    uint32_t leapSecondsCurrent_valid;
    uint8_t  gpsTimeUncMs_valid;
    uint32_t gpsTimeUncMs;
} GnssTime;

class LocationAPI;

/* Provides the capabilities of the system
   capabilities callback is called once soon after createInstance is called */
typedef void(*capabilitiesCallback)(
    LocationAPI* pLocationAPI,
    LocationCapabilitiesMask capabilitiesMask // bitwise OR of LocationCapabilitiesMask
);

/* Used by tracking, batching, and miscellanous APIs
   responseCallback is called for every Tracking, Batching API, and Miscellanous API */
typedef void(*responseCallback)(
    LocationAPI* pLocationAPI,
    LocationError, // if not SUCCESS, then id is not valid
    uint32_t id    // id to be associated to the request
);

/* Used by APIs that gets more than one LocationError in it's response
   collectiveResponseCallback is called for every geofence API call.
   ids array and LocationError array are only valid until collectiveResponseCallback returns. */
typedef void(*collectiveResponseCallback)(
    LocationAPI* pLocationAPI,
    size_t count,   // number of locations in arrays
    LocationError*, // array of LocationError associated to the request
    uint32_t* ids   // array of ids to be associated to the request
);

/* Used for startTracking API, optional can be NULL
   trackingCallback is called when delivering a location in a tracking session
   broadcasted to all clients, no matter if a session has started by client */
typedef void(*trackingCallback)(
    LocationAPI* pLocationAPI,
    Location location
);

/* Used for startBatching API, optional can be NULL
   batchingCallback is called when delivering locations in a batching session.
   broadcasted to all clients, no matter if a session has started by client */
typedef void(*batchingCallback)(
    LocationAPI* pLocationAPI,
    size_t count,      // number of locations in array
    Location* location  // array of locations
);

/* Used for addGeofences API, optional can be NULL
   geofenceBreachCallback is called when any number of geofences have a state change */
typedef void(*geofenceBreachCallback)(
    LocationAPI* pLocationAPI,
    GeofenceBreachNotification geofenceBreachNotification
);

/* Used for getSingleShot API, optional can be NULL
   singleShotCallback is called when delivering a location in a single shot session
   broadcasted to all clients, no matter if a session has started by client */
typedef void(*singleShotCallback)(
    LocationAPI* pLocationAPI,
    Location location, // valid only if err == LOCATION_ERROR_SUCCESS
    LocationError err  // can be LOCATION_ERROR_SUCCESS or LOCATION_ERROR_TIMEOUT
);

/* Used for startGetGnssData API, optional can be NULL
   gnssDataCallback is called when delivering  a GNSS Data structure. The GNSS Data
   structure contains useful information (e.g. jammer indication, etc.). 
   This callback will be called every second. */
typedef void(*gnssDataCallback)(
    LocationAPI* pLocationAPI,
    GnssData gnssData
);

/* Used for below scenarios, optional can be NULL
   - getBackoffTime() to fetch backoff time from GNSS engine. */
typedef void(*metaDataCallback)(
    LocationAPI* pLocationAPI,
    LocationMetaData metaData
);

/* Gives GNSS NMEA data, optional can be NULL
    gnssNmeaCallback is called only during a tracking session
    broadcasted to all clients, no matter if a session has started by client */
typedef void(*gnssNmeaCallback)(
    LocationAPI* pLocationAPI,
    GnssNmeaNotification gnssNmeaNotification
);

typedef void(*motionTrackingCallback)(
    LocationAPI* pLocationAPI,
    LocationMotionInfo motionInfo
);

typedef struct {
    size_t size; // set to sizeof(LocationCallbacks)
    capabilitiesCallback capabilitiesCb;             // mandatory
    responseCallback responseCb;                     // mandatory
    collectiveResponseCallback collectiveResponseCb; // mandatory
    trackingCallback trackingCb;                     // optional
    batchingCallback batchingCb;                     // optional
    geofenceBreachCallback geofenceBreachCb;         // optional
    singleShotCallback singleShotCb;                 // optional
    gnssDataCallback gnssDataCb;                     // optional
    metaDataCallback metaDataCb;                     // optional
    gnssNmeaCallback gnssNmeaCb;                     // optional
    motionTrackingCallback motionTrackingCb;         // optional
} LocationCallbacks;

typedef uint32_t GnssAidingDataSvMask;
typedef enum {
	GNSS_AIDING_DATA_SV_EPHEMERIS_BIT    = (1<<0), // ephemeris
	GNSS_AIDING_DATA_SV_ALMANAC_BIT      = (1<<1), // almanac
	GNSS_AIDING_DATA_SV_HEALTH_BIT       = (1<<2), // health
	GNSS_AIDING_DATA_SV_DIRECTION_BIT    = (1<<3), // direction
	GNSS_AIDING_DATA_SV_STEER_BIT        = (1<<4), // steer
	GNSS_AIDING_DATA_SV_ALMANAC_CORR_BIT = (1<<5), // almanac correction
	GNSS_AIDING_DATA_SV_BLACKLIST_BIT    = (1<<6), // blacklist SVs
	GNSS_AIDING_DATA_SV_SA_DATA_BIT      = (1<<7), // sensitivity assistance data
	GNSS_AIDING_DATA_SV_NO_EXIST_BIT     = (1<<8), // SV does not exist
	GNSS_AIDING_DATA_SV_IONOSPHERE_BIT   = (1<<9), // ionosphere correction
	GNSS_AIDING_DATA_SV_TIME_BIT         = (1<<10),// reset satellite time
	GNSS_AIDING_DATA_SV_MB_DATA          = (1<<11),// delete multiband data
	GNSS_AIDING_DATA_SV_POLY_BIT         = (1<<12),// poly
} GnssAidingDataSvBits;

typedef uint32_t GnssAidingDataSvTypeMask;
typedef enum {
		GNSS_AIDING_DATA_SV_TYPE_GPS_BIT      = (1<<0),
		GNSS_AIDING_DATA_SV_TYPE_GLONASS_BIT  = (1<<1),
		GNSS_AIDING_DATA_SV_TYPE_QZSS_BIT     = (1<<2),
		GNSS_AIDING_DATA_SV_TYPE_BEIDOU_BIT   = (1<<3),
		GNSS_AIDING_DATA_SV_TYPE_GALILEO_BIT  = (1<<4),
		GNSS_AIDING_DATA_SV_TYPE_NAVIC_BIT    = (1<<5),
} GnssAidingDataSvTypeBits;

typedef uint32_t PositioningEngineMask;
typedef enum {
	STANDARD_POSITIONING_ENGINE = (1 << 0),
	DEAD_RECKONING_ENGINE       = (1 << 1),
	PRECISE_POSITIONING_ENGINE  = (1 << 2)
} PositioningEngineBits;
#define POSITION_ENGINE_MASK_ALL \
	(STANDARD_POSITIONING_ENGINE|DEAD_RECKONING_ENGINE|PRECISE_POSITIONING_ENGINE)

typedef struct {
	GnssAidingDataSvMask svMask;         // bitwise OR of GnssAidingDataSvBits
	GnssAidingDataSvTypeMask svTypeMask; // bitwise OR of GnssAidingDataSvTypeBits
} GnssAidingDataSv;

typedef uint32_t GnssAidingDataCommonMask;

typedef enum {
	GNSS_AIDING_DATA_COMMON_POSITION_BIT      = (1<<0), // position estimate
	GNSS_AIDING_DATA_COMMON_TIME_BIT          = (1<<1), // reset all clock values
	GNSS_AIDING_DATA_COMMON_UTC_BIT           = (1<<2), // UTC estimate
	GNSS_AIDING_DATA_COMMON_RTI_BIT           = (1<<3), // RTI
	GNSS_AIDING_DATA_COMMON_FREQ_BIAS_EST_BIT = (1<<4), // frequency bias estimate
	GNSS_AIDING_DATA_COMMON_CELLDB_BIT        = (1<<5), // all celldb info
} GnssAidingDataCommonBits;

typedef struct {
	GnssAidingDataCommonMask mask; // bitwise OR of GnssAidingDataCommonBits
} GnssAidingDataCommon;

typedef struct {
    size_t size;                 // set to sizeof(GnssAidingData)
    bool deleteAll;              // if true, delete all aiding data and ignore other params
    GnssAidingDataSv sv;         // SV specific aiding data
    GnssAidingDataCommon common; // common aiding data
    PositioningEngineMask posEngineMask; // engines to perform the delete operation on.
} GnssAidingData;

class LocationAPI
{
private:
    LocationAPI();
    ~LocationAPI();

public:
    /* Creates an instance to LocationAPI object.
       Will return NULL if mandatory parameters are invalid or if the maximum number
       of instances have been reached */
    static LocationAPI* createInstance(LocationCallbacks&);

    /* Destroys/cleans up the instance, which should be called when LocationAPI object is
       no longer needed. LocationAPI* returned from createInstance will no longer valid
       after deleteInstance is called */
    void deleteInstance();

    /* Updates/changes the callbacks that will be called.
       Mandatory callbacks must be present for callbacks to be successfully updated
       No return value */
    void updateCallbacks(LocationCallbacks&);

    /* ================================== TRACKING ================================== */

    /* startTracking starts a tracking session, which returns a session id that will be
       used by the other tracking APIs and also in the responseCallback to match command
       with response. locations are reported on the trackingCallback passed in createInstance
       periodically according to LocationOptions.
        responseCallback returns:
                LOCATION_ERROR_SUCCESS if session was successfully started
                LOCATION_ERROR_ALREADY_STARTED if a startTracking session is already in progress
                LOCATION_ERROR_CALLBACK_MISSING if no trackingCallback was passed in createInstance
                LOCATION_ERROR_INVALID_PARAMETER if LocationOptions parameter is invalid */
    uint32_t startTracking(LocationOptions&); // returns session id

    /* stopTracking stops a tracking session associated with id parameter.
        responseCallback returns:
                LOCATION_ERROR_SUCCESS if successful
                LOCATION_ERROR_ID_UNKNOWN if id is not associated with a tracking session */
    void stopTracking(uint32_t id);

    /* updateTrackingOptions changes the LocationOptions of a tracking session associated with id
        responseCallback returns:
                LOCATION_ERROR_SUCCESS if successful
                LOCATION_ERROR_INVALID_PARAMETER if LocationOptions parameters are invalid
                LOCATION_ERROR_ID_UNKNOWN if id is not associated with a tracking session */
    void updateTrackingOptions(uint32_t id, LocationOptions&);

    /* ================================== BATCHING ================================== */

    /* startBatching starts a batching session, which returns a session id that will be
       used by the other batching APIs and also in the responseCallback to match command
       with response. locations are reported on the batchingCallback passed in createInstance
       periodically according to LocationOptions. A batching session starts tracking on
       the low power processor and delivers them in batches by the batchingCallback when
       the batch is full or when getBatchedLocations is called. This allows for the processor
       that calls this API to sleep when the low power processor can batch locations in the
       backgroup and wake up the processor calling the API only when the batch is full, thus
       saving power
        responseCallback returns:
                LOCATION_ERROR_SUCCESS if session was successful
                LOCATION_ERROR_ALREADY_STARTED if a startBatching session is already in progress
                LOCATION_ERROR_CALLBACK_MISSING if no batchingCallback was passed in createInstance
                LOCATION_ERROR_INVALID_PARAMETER if a parameter is invalid
                LOCATION_ERROR_NOT_SUPPORTED if batching is not supported */
    uint32_t startBatching(LocationOptions&); // returns session id

    /* stopBatching stops a batching session associated with id parameter.
        responseCallback returns:
                LOCATION_ERROR_SUCCESS if successful
                LOCATION_ERROR_ID_UNKNOWN if id is not associated with batching session */
    void stopBatching(uint32_t id);

    /* updateBatchingOptions changes the LocationOptions of a batching session associated with id
        responseCallback returns:
                LOCATION_ERROR_SUCCESS if successful
                LOCATION_ERROR_INVALID_PARAMETER if LocationOptions parameters are invalid
                LOCATION_ERROR_ID_UNKNOWN if id is not associated with a batching session */
    void updateBatchingOptions(uint32_t id, LocationOptions&);

    /* getBatchedLocations gets a number of locations that are currently stored/batched
       on the low power processor, delivered by the batchingCallback passed in createInstance.
       Location are then deleted from the batch stored on the low power processor.
        responseCallback returns:
                LOCATION_ERROR_SUCCESS if successful, will be followed by batchingCallback call
                LOCATION_ERROR_CALLBACK_MISSING if no batchingCallback was passed in createInstance
                LOCATION_ERROR_ID_UNKNOWN if id is not associated with a batching session */
    void getBatchedLocations(uint32_t id, size_t count);

    /* ================================== GEOFENCE ================================== */

    /* addGeofences adds any number of geofences and returns an array of geofence ids that
       will be used by the other geofence APIs and also in the collectiveResponseCallback to
       match command with response. The geofenceBreachCallback will deliver the status of each
       geofence according to the GeofenceOption for each. The geofence id array returned will
       be valid until the collectiveResponseCallback is called and has returned.
        collectiveResponseCallback returns:
                LOCATION_ERROR_SUCCESS if session was successful
                LOCATION_ERROR_CALLBACK_MISSING if no geofenceBreachCallback
                LOCATION_ERROR_INVALID_PARAMETER if any parameters are invalid
                LOCATION_ERROR_NOT_SUPPORTED if geofence is not supported */
    uint32_t* addGeofences(size_t count, GeofenceOption*, GeofenceInfo*); // returns id array

    /* removeGeofences removes any number of geofences. Caller should delete ids array after
       removeGeofences returneds.
        collectiveResponseCallback returns:
                LOCATION_ERROR_SUCCESS if successful
                LOCATION_ERROR_ID_UNKNOWN if id is not associated with a geofence session */
    void removeGeofences(size_t count, uint32_t* ids);

    /* modifyGeofences modifies any number of geofences. Caller should delete ids array after
       modifyGeofences returns.
        collectiveResponseCallback returns:
                LOCATION_ERROR_SUCCESS if successful
                LOCATION_ERROR_ID_UNKNOWN if id is not associated with a geofence session
                LOCATION_ERROR_INVALID_PARAMETER if any parameters are invalid */
    void modifyGeofences(size_t count, uint32_t* ids, GeofenceOption* options);

    /* pauseGeofences pauses any number of geofences, which is similar to removeGeofences,
       only that they can be resumed at any time. Caller should delete ids array after
       pauseGeofences returns.
        collectiveResponseCallback returns:
                LOCATION_ERROR_SUCCESS if successful
                LOCATION_ERROR_ID_UNKNOWN if id is not associated with a geofence session */
    void pauseGeofences(size_t count, uint32_t* ids);

    /* resumeGeofences resumes any number of geofences that are currently paused. Caller should
       delete ids array after resumeGeofences returns.
        collectiveResponseCallback returns:
                LOCATION_ERROR_SUCCESS if successful
                LOCATION_ERROR_ID_UNKNOWN if id is not associated with a geofence session */
    void resumeGeofences(size_t count, uint32_t* ids);

    /* =============================== SINGLE SHOT =================================== */

        /* getSingleShot attempts a single location fix. It returns
       a session id that can be used by the cancelSingleShot API and also in the responseCallback
       to match command with response.
           responseCallback returns:
                LOCATION_ERROR_SUCCESS if session was successfully started
                LOCATION_ERROR_CALLBACK_MISSING if no singleShotCallback passed in createInstance
                LOCATION_ERROR_INVALID_PARAMETER if LocationOptions parameter is invalid
       If responseCallback reports LOCATION_ERROR_SUCESS, then the following is
       can happen to end the single shot session:
       1) A location will be reported on the singleShotCallback
       2) LOCATION_ERROR_TIMEOUT will be reported on the singleShotCallback.
       3) The single shot session is canceled using the cancelSingleShot API
       In either of these cases, the session is considered complete and the session id will
       no longer be valid */
    uint32_t getSingleShot(LocationPowerLevel); // returns session id

        /* getSingleShot attempts a single location fix. It returns
       a session id that can be used by the cancelSingleShot API and also in the responseCallback
       to match command with response.
           responseCallback returns:
                LOCATION_ERROR_SUCCESS if session was successfully started
                LOCATION_ERROR_CALLBACK_MISSING if no singleShotCallback passed in createInstance
                LOCATION_ERROR_INVALID_PARAMETER if LocationOptions parameter is invalid
       If responseCallback reports LOCATION_ERROR_SUCCESS, then the following
       can happen to end the single shot session:
       1) A location will be reported on the singleShotCallback
       2) LOCATION_ERROR_TIMEOUT will be reported on the singleShotCallback.
       3) The single shot session is canceled using the cancelSingleShot API
       In either of these cases, the session is considered complete and the session id will
       no longer be valid */
    uint32_t getSingleShotV2(SingleShotOptions); // returns session id

    /* cancelSingleShot cancels a single shot session associated with id parameter.
        responseCallback returns:
                LOCATION_ERROR_SUCCESS if successful
                LOCATION_ERROR_ID_UNKNOWN if id is not associated with a single shot session */
    void cancelSingleShot(uint32_t id);

    /* =============================== GNSS DATA =================================== */

    /*  startGetGnssDataStarts a "Get GNSS Data" session, which returns a 
         session ID that will be used by stopGetGnssData API and in 
         the response callback to match the command with a response. GNSS Data is 
         reported on the GNSS Data callback passed in createInstance() periodically
         (every second until stopGetGnssData is called).
        responseCallback returns:
                LOCATION_ERROR_SUCCESS if session was successfully started
                LOCATION_ERROR_ALREADY_STARTED if a startGetGnssData session is already in progress
                LOCATION_ERROR_CALLBACK_MISSING if no gnssDataCallback was passed in createInstance */
    uint32_t startGetGnssData(); // returns session id

    /* stopGetGnssData stops a "Get GNSS Data" session associated with id parameter.
        responseCallback returns:
                LOCATION_ERROR_SUCCESS if successful
                LOCATION_ERROR_ID_UNKNOWN if id is not associated with a "Get GNSS Data" session */
    void stopGetGnssData(uint32_t id);

    /* =============================== META DATA =================================== */

    /*  getBackoffTime sends request to GNSS engine for providing a backoff time.
        The backoff time indicates the minimum time for which the client will not
        receive a location callback.
        responseCallback returns:
                LOCATION_ERROR_SUCCESS if session was successfully started
                LOCATION_ERROR_CALLBACK_MISSING if no metaDataCallback was passed in createInstance */
    uint32_t getBackoffTime();

    /*  getGnssEnergyConsumed sends request to GNSS engine for providing the power
        consumption numbers.
        responseCallback returns:
                LOCATION_ERROR_SUCCESS if session was successfully started
                LOCATION_ERROR_CALLBACK_MISSING if no metaDataCallback was passed in createInstance */
    uint32_t getGnssEnergyConsumed();

    /* ======================== BEST AVAILABLE POSITION ==================== */

    /* getBestAvailablePosition requests the best available position from the
       GNSS Engine.
       Returns a session id that can be used in the responseCallback to match
       command with response.
       responseCallback returns:
                LOCATION_ERROR_SUCCESS if session was successfully started
                LOCATION_ERROR_CALLBACK_MISSING if no singleShotCallback passed in createInstance
                LOCATION_ERROR_INVALID_PARAMETER if LocationOptions parameter is invalid
       If responseCb reports LOCATION_ERROR_SUCESS, then the following is what
       can happen:
       1) A location will be reported on the singleShotCb.
          The err parameter of the callback would be set to
          LOCATION_ERROR_BEST_AVAILABLE_POSITION.
          This location can have a large accuracy value, which must be
          checked by the client if it suffices it's purpose. */
    uint32_t getBestAvailablePosition();

    /* ========================= MOTION TRACKING =========================== */

    /* startMotionTracking starts a motion tracking session, which returns a
       session id that will be used by the other motion tracking APIs and also
       in the responseCallback to match command with response.
       Motion update is reported on the motionTrackingCb passed in createInstance
       according to LocationMotionOptions.
       responseCallback returns:
                LOCATION_ERROR_SUCCESS if session was successfully started
                LOCATION_ERROR_ALREADY_STARTED if a startTracking session is already in progress
                LOCATION_ERROR_CALLBACK_MISSING if no trackingCallback was passed in createInstance
                LOCATION_ERROR_INVALID_PARAMETER if LocationOptions parameter is invalid */
    uint32_t startMotionTracking(LocationMotionOptions&); // returns session id

    /* stopMotionTracking stops a motion tracking session associated with id
       parameter.
       responseCallback returns:
                LOCATION_ERROR_SUCCESS if successful
                LOCATION_ERROR_ID_UNKNOWN if id is not associated with a tracking session */
    void stopMotionTracking(uint32_t id);

    /* startNtpDownload starts a NTP download session, which returns a
       session id that will be used by the other NTP download APIs and also
       in the responseCallback to match command with response.
       responseCallback returns:
                LOCATION_ERROR_SUCCESS if session was successfully started
                LOCATION_ERROR_ALREADY_STARTED if a NTP download session is already in progress
                LOCATION_ERROR_XTRA_DISABLED if XTRA module is disabled*/

    uint32_t startNtpDownload();
};

/* Used by Location Control API Clients */
typedef void(*responseControlCallback)(
    LocationError // if not SUCCESS, then id is not valid
);

typedef struct {
    size_t size; // set to sizeof(LocationControlCallbacks)
    responseControlCallback responseControlCb;            // mandatory
} LocationControlCallbacks;

class LocationControlAPI
{
private:
    LocationControlAPI();
    ~LocationControlAPI();

    /* Delete required aiding data for validation purposes */
    void gnssDeleteAidingData(GnssAidingData& data);

    bool mIsGnssEnabled;
    bool mIsGtpWwanEnabled;

public:
    /* creates an instance to LocationControlAPI object.
    Will return NULL if mandatory parameters are invalid or if the maximum number
    of instances have been reached. Only once instance allowed */
    static LocationControlAPI* createInstance(LocationControlCallbacks&);

    /* Specifies whether a particular location technology is enabled or not.
       responseCallback returns:
                LOCATION_ERROR_SUCCESS if successful
                LOCATION_ERROR_INVALID_PARAMETER if any parameters are invalid
                LOCATION_ERROR_GENERAL_FAILURE if failure for any other reason */
    static bool isEnabled(LocationTechnologyType techType);

    /* destroy/cleans up the instance, which should be called when LocationControlAPI object is
    no longer needed. LocationControlAPI* returned from createInstance will no longer valid
    after destroy is called */
    void destroy();

    /* enableGtpWwan will enable GTP WWAN
    This effect is global for all clients of LocationAPI
    responseControlCallback returns:
    LOCATION_ERROR_SUCCESS if successful
    LOCATION_ERROR_GENERAL_FAILURE if failure */
    void enableGtpWwan();
    void disableGtpWwan();

    /* Convenience APIs for cold and warm start */
    void gnssDeleteAidingDataAll();           // cold start
    void gnssDeleteAidingDataEphemeris();     // warm start
    void gnssDeleteAidingDataCachedLocation(); //Delete Cached location

    /* Enable or disable XTRA download. Will be enabled by default. */
    void enableXtraDload();
    void disableXtraDload();
};

#endif /* LOCATIONAPI_H */

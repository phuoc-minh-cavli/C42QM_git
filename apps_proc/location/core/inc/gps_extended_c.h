/* Copyright (c) 2013-2016, 2018 The Linux Foundation. All rights reserved.
 *
 * Not a Contribution.
 *
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GPS_EXTENDED_C_H
#define GPS_EXTENDED_C_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "pthread_loc.h"
#include <LocationAPI.h>

    /** Location has valid source information. */
#define LOCATION_HAS_SOURCE_INFO   0x0020
    /** GpsLocation has valid "is indoor?" flag */
#define GPS_LOCATION_HAS_IS_INDOOR   0x0040
    /** GpsLocation has valid floor number */
#define GPS_LOCATION_HAS_FLOOR_NUMBER   0x0080
    /** GpsLocation has valid map URL*/
#define GPS_LOCATION_HAS_MAP_URL   0x0100
    /** GpsLocation has valid map index */
#define GPS_LOCATION_HAS_MAP_INDEX   0x0200

    /** Position source is ULP */
#define ULP_LOCATION_IS_FROM_HYBRID   0x0001
    /** Position source is GNSS only */
#define ULP_LOCATION_IS_FROM_GNSS     0x0002
    /** Position source is ZPP only */
#define ULP_LOCATION_IS_FROM_ZPP      0x0004
    /** Position is from a Geofence Breach Event */
#define ULP_LOCATION_IS_FROM_GEOFENCE 0X0008
    /** Position is from Hardware FLP */
#define ULP_LOCATION_IS_FROM_HW_FLP   0x0010
    /** Position is from NLP */
#define ULP_LOCATION_IS_FROM_NLP      0x0020
    /** Position is from PIP */
#define ULP_LOCATION_IS_FROM_PIP      0x0040
    /** Position is from external DR solution*/
#define ULP_LOCATION_IS_FROM_EXT_DR   0X0080

#define ULP_MIN_INTERVAL_INVALID 0xffffffff

#define GPS_GEOFENCE_ENTERED     (1<<0L)
#define GPS_GEOFENCE_EXITED      (1<<1L)
#define GPS_GEOFENCE_UNCERTAIN   (1<<2L)

#define GPS_GEOFENCE_UNAVAILABLE (1<<0L)
#define GPS_GEOFENCE_AVAILABLE   (1<<1L)

#define GPS_GEOFENCE_OPERATION_SUCCESS           0
#define GPS_GEOFENCE_ERROR_TOO_MANY_GEOFENCES -100
#define GPS_GEOFENCE_ERROR_ID_EXISTS          -101
#define GPS_GEOFENCE_ERROR_ID_UNKNOWN         -102
#define GPS_GEOFENCE_ERROR_INVALID_TRANSITION -103
#define GPS_GEOFENCE_ERROR_GENERIC            -149

#define GNSS_NMEA_STRING_MAX_LENGTH  200

/** GPS PRN Range */
#define GPS_SV_PRN_MIN      1
#define GPS_SV_PRN_MAX      32
#define GLO_SV_PRN_MIN      65
#define GLO_SV_PRN_MAX      96
#define QZSS_SV_PRN_MIN     193
#define QZSS_SV_PRN_MAX     197
#define BDS_SV_PRN_MIN      201
#define BDS_SV_PRN_MAX      235
#define GAL_SV_PRN_MIN      301
#define GAL_SV_PRN_MAX      336

typedef uint32_t LocPosTechMask;
#define LOC_POS_TECH_MASK_DEFAULT ((LocPosTechMask)0x00000000)
#define LOC_POS_TECH_MASK_SATELLITE ((LocPosTechMask)0x00000001)
#define LOC_POS_TECH_MASK_CELLID ((LocPosTechMask)0x00000002)
#define LOC_POS_TECH_MASK_WIFI ((LocPosTechMask)0x00000004)
#define LOC_POS_TECH_MASK_SENSORS ((LocPosTechMask)0x00000008)
#define LOC_POS_TECH_MASK_REFERENCE_LOCATION ((LocPosTechMask)0x00000010)
#define LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION ((LocPosTechMask)0x00000020)
#define LOC_POS_TECH_MASK_AFLT ((LocPosTechMask)0x00000040)
#define LOC_POS_TECH_MASK_HYBRID ((LocPosTechMask)0x00000080)


/** Flags to indicate which values are valid in a GpsLocationExtended. */
typedef uint64_t GpsLocationExtendedFlags;
/** GpsLocationExtended has valid pdop, hdop, vdop. */
#define GPS_LOCATION_EXTENDED_HAS_DOP                              0x00000001
/** GpsLocationExtended has valid altitude mean sea level. */
#define GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL          0x00000002
/** UlpLocation has valid magnetic deviation. */
#define GPS_LOCATION_EXTENDED_HAS_MAG_DEV                          0x00000004
/** UlpLocation has valid mode indicator. */
#define GPS_LOCATION_EXTENDED_HAS_MODE_IND                         0x00000008
/** GpsLocationExtended has valid vertical uncertainty */
#define GPS_LOCATION_EXTENDED_HAS_VERT_UNC                         0x00000010
/** GpsLocationExtended has valid speed uncertainty */
#define GPS_LOCATION_EXTENDED_HAS_SPEED_UNC                        0x00000020
/** GpsLocationExtended has valid heading uncertainty */
#define GPS_LOCATION_EXTENDED_HAS_BEARING_UNC                      0x00000040
/** GpsLocationExtended has valid horizontal reliability */
#define GPS_LOCATION_EXTENDED_HAS_HOR_RELIABILITY                  0x00000080
/** GpsLocationExtended has valid vertical reliability */
#define GPS_LOCATION_EXTENDED_HAS_VERT_RELIABILITY                 0x00000100
/** GpsLocationExtended has valid Horizontal Elliptical Uncertainty (Semi-Major Axis) */
#define GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MAJOR               0x00000200
/** GpsLocationExtended has valid Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
#define GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MINOR               0x00000400
/** GpsLocationExtended has valid Elliptical Horizontal Uncertainty Azimuth */
#define GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_AZIMUTH             0x00000800
/** GpsLocationExtended has valid gnss sv used in position data */
#define GPS_LOCATION_EXTENDED_HAS_GNSS_SV_USED_DATA                0x00001000
/** GpsLocationExtended has valid navSolutionMask */
#define GPS_LOCATION_EXTENDED_HAS_NAV_SOLUTION_MASK                0x00002000
/** GpsLocationExtended has valid LocPosTechMask */
#define GPS_LOCATION_EXTENDED_HAS_POS_TECH_MASK                    0x00004000
/** GpsLocationExtended has valid LocSvInfoSource */
#define GPS_LOCATION_EXTENDED_HAS_SV_SOURCE_INFO                   0x00008000
/** GpsLocationExtended has valid position dynamics data */
#define GPS_LOCATION_EXTENDED_HAS_POS_DYNAMICS_DATA                0x00010000
/** GpsLocationExtended has GPS Time */
#define GPS_LOCATION_EXTENDED_HAS_GPS_TIME                         0x00020000
/** GpsLocationExtended has Extended Dilution of Precision */
#define GPS_LOCATION_EXTENDED_HAS_EXT_DOP                          0x00040000
/** GpsLocationExtended has North standard deviation */
#define GPS_LOCATION_EXTENDED_HAS_NORTH_STD_DEV                    0x00080000
/** GpsLocationExtended has East standard deviation*/
#define GPS_LOCATION_EXTENDED_HAS_EAST_STD_DEV                     0x00100000
/** GpsLocationExtended has North Velocity */
#define GPS_LOCATION_EXTENDED_HAS_NORTH_VEL                        0x00200000
/** GpsLocationExtended has East Velocity */
#define GPS_LOCATION_EXTENDED_HAS_EAST_VEL                         0x00400000
/** GpsLocationExtended has up Velocity */
#define GPS_LOCATION_EXTENDED_HAS_UP_VEL                           0x00800000
/** GpsLocationExtended has North Velocity Uncertainty */
#define GPS_LOCATION_EXTENDED_HAS_NORTH_VEL_UNC                    0x01000000
/** GpsLocationExtended has East Velocity Uncertainty */
#define GPS_LOCATION_EXTENDED_HAS_EAST_VEL_UNC                     0x02000000
/** GpsLocationExtended has up Velocity Uncertainty */
#define GPS_LOCATION_EXTENDED_HAS_UP_VEL_UNC                       0x04000000
/** GpsLocationExtended has Clock Bias */
#define GPS_LOCATION_EXTENDED_HAS_CLOCK_BIAS                       0x08000000
/** GpsLocationExtended has Clock Bias std deviation*/
#define GPS_LOCATION_EXTENDED_HAS_CLOCK_BIAS_STD_DEV               0x10000000
/** GpsLocationExtended has Clock drift*/
#define GPS_LOCATION_EXTENDED_HAS_CLOCK_DRIFT                      0x20000000
/** GpsLocationExtended has Clock drift std deviation**/
#define GPS_LOCATION_EXTENDED_HAS_CLOCK_DRIFT_STD_DEV              0x40000000
/** GpsLocationExtended has leap seconds **/
#define GPS_LOCATION_EXTENDED_HAS_LEAP_SECONDS                     0x80000000
/** GpsLocationExtended has time uncertainty **/
#define GPS_LOCATION_EXTENDED_HAS_TIME_UNC                        0x100000000
/** GpsLocationExtended has heading rate  **/
#define GPS_LOCATION_EXTENDED_HAS_HEADING_RATE                    0x200000000
/** GpsLocationExtended has DGNSS data age  **/
#define GPS_LOCATION_EXTENDED_HAS_DGNSS_DATA_AGE                  0x400000000
/** GpsLocationExtended has reference station ID  **/
#define GPS_LOCATION_EXTENDED_HAS_DGNSS_REF_STATION_ID            0x800000000


typedef uint32_t LocNavSolutionMask;
/* Bitmask to specify whether SBAS ionospheric correction is used  */
#define LOC_NAV_MASK_SBAS_CORRECTION_IONO        ((LocNavSolutionMask)0x0001)
/* Bitmask to specify whether SBAS fast correction is used  */
#define LOC_NAV_MASK_SBAS_CORRECTION_FAST        ((LocNavSolutionMask)0x0002)
/**<  Bitmask to specify whether SBAS long-tem correction is used  */
#define LOC_NAV_MASK_SBAS_CORRECTION_LONG        ((LocNavSolutionMask)0x0004)
/**<  Bitmask to specify whether SBAS integrity information is used  */
#define LOC_NAV_MASK_SBAS_INTEGRITY              ((LocNavSolutionMask)0x0008)
/**<  Bitmask to specify whether Position Report is DGNSS corrected  */
#define LOC_NAV_MASK_DGNSS_CORRECTION            ((LocNavSolutionMask)0x0010)
/**<  Bitmask to specify whether Position Report is RTK corrected   */
#define LOC_NAV_MASK_RTK_CORRECTION              ((LocNavSolutionMask)0x0020)
/**<  Bitmask to specify whether Position Report is PPP corrected   */
#define LOC_NAV_MASK_PPP_CORRECTION              ((LocNavSolutionMask)0x0040)
/**<  Bitmask to specify whether Position Report is RTK fixed corrected   */
#define LOC_NAV_MASK_RTK_FIXED_CORRECTION        ((LocNavSolutionMask)0x0080)

#define GNSS_SV_MAX            (80)

/** Receive GPS fixes on a recurring basis at a specified period. */
#define GPS_POSITION_RECURRENCE_PERIODIC   0
/** Request a single shot GPS fix. */
#define GPS_POSITION_RECURRENCE_SINGLE      1

// Nmea sentence types mask
typedef uint32_t NmeaSentenceTypesMask;
#define LOC_NMEA_MASK_GGA_V02   ((NmeaSentenceTypesMask)0x00000001) /**<  Enable GGA type  */
#define LOC_NMEA_MASK_RMC_V02   ((NmeaSentenceTypesMask)0x00000002) /**<  Enable RMC type  */
#define LOC_NMEA_MASK_GSV_V02   ((NmeaSentenceTypesMask)0x00000004) /**<  Enable GSV type  */
#define LOC_NMEA_MASK_GSA_V02   ((NmeaSentenceTypesMask)0x00000008) /**<  Enable GSA type  */
#define LOC_NMEA_MASK_VTG_V02   ((NmeaSentenceTypesMask)0x00000010) /**<  Enable VTG type  */
#define LOC_NMEA_MASK_PQXFI_V02 ((NmeaSentenceTypesMask)0x00000020) /**<  Enable PQXFI type  */
#define LOC_NMEA_MASK_PSTIS_V02 ((NmeaSentenceTypesMask)0x00000040) /**<  Enable PSTIS type  */
#define LOC_NMEA_MASK_GLGSV_V02 ((NmeaSentenceTypesMask)0x00000080) /**<  Enable GLGSV type  */
#define LOC_NMEA_MASK_GNGSA_V02 ((NmeaSentenceTypesMask)0x00000100) /**<  Enable GNGSA type  */
#define LOC_NMEA_MASK_GNGNS_V02 ((NmeaSentenceTypesMask)0x00000200) /**<  Enable GNGNS type  */
#define LOC_NMEA_MASK_GARMC_V02 ((NmeaSentenceTypesMask)0x00000400) /**<  Enable GARMC type  */
#define LOC_NMEA_MASK_GAGSV_V02 ((NmeaSentenceTypesMask)0x00000800) /**<  Enable GAGSV type  */
#define LOC_NMEA_MASK_GAGSA_V02 ((NmeaSentenceTypesMask)0x00001000) /**<  Enable GAGSA type  */
#define LOC_NMEA_MASK_GAVTG_V02 ((NmeaSentenceTypesMask)0x00002000) /**<  Enable GAVTG type  */
#define LOC_NMEA_MASK_GAGGA_V02 ((NmeaSentenceTypesMask)0x00004000) /**<  Enable GAGGA type  */
#define LOC_NMEA_MASK_PQGSA_V02 ((NmeaSentenceTypesMask)0x00008000) /**<  Enable PQGSA type  */
#define LOC_NMEA_MASK_PQGSV_V02 ((NmeaSentenceTypesMask)0x00010000) /**<  Enable PQGSV type  */
#define LOC_NMEA_MASK_DEBUG_V02 ((NmeaSentenceTypesMask)0x00020000) /**<  Enable DEBUG type  */

#define LOC_NMEA_ALL_SUPPORTED_MASK  (LOC_NMEA_MASK_GGA_V02 | LOC_NMEA_MASK_RMC_V02 | \
              LOC_NMEA_MASK_GSV_V02 | LOC_NMEA_MASK_GSA_V02 | LOC_NMEA_MASK_VTG_V02 | \
        LOC_NMEA_MASK_PQXFI_V02 | LOC_NMEA_MASK_PSTIS_V02 | LOC_NMEA_MASK_GLGSV_V02 | \
        LOC_NMEA_MASK_GNGSA_V02 | LOC_NMEA_MASK_GNGNS_V02 | LOC_NMEA_MASK_GARMC_V02 | \
        LOC_NMEA_MASK_GAGSV_V02 | LOC_NMEA_MASK_GAGSA_V02 | LOC_NMEA_MASK_GAVTG_V02 | \
        LOC_NMEA_MASK_GAGGA_V02 | LOC_NMEA_MASK_PQGSA_V02 | LOC_NMEA_MASK_PQGSV_V02 | \
        LOC_NMEA_MASK_DEBUG_V02 )

#define smaller_of(a, b) (((a) > (b)) ? (b) : (a))
#define MAX_APN_LEN 100

// This will be overridden by the individual adapters
// if necessary.
#define DEFAULT_IMPL(rtv)                                     \
{                                                             \
    MSG(MSG_SSID_GPS, MSG_LEGACY_LOW, "default implementation invoked"); \
    return rtv;                                               \
}

#define LOC_API_ADAPTER_BIT_PARSED_POSITION_REPORT           (1<<LOC_API_ADAPTER_REPORT_POSITION)
#define LOC_API_ADAPTER_BIT_SATELLITE_REPORT                 (1<<LOC_API_ADAPTER_REPORT_SATELLITE)
#define LOC_API_ADAPTER_BIT_NMEA_1HZ_REPORT                  (1<<LOC_API_ADAPTER_REPORT_NMEA_1HZ)
#define LOC_API_ADAPTER_BIT_NMEA_POSITION_REPORT             (1<<LOC_API_ADAPTER_REPORT_NMEA_POSITION)
#define LOC_API_ADAPTER_BIT_ASSISTANCE_DATA_REQUEST          (1<<LOC_API_ADAPTER_REQUEST_ASSISTANCE_DATA)
#define LOC_API_ADAPTER_BIT_IOCTL_REPORT                     (1<<LOC_API_ADAPTER_REPORT_IOCTL)
#define LOC_API_ADAPTER_BIT_STATUS_REPORT                    (1<<LOC_API_ADAPTER_REPORT_STATUS)
#define LOC_API_ADAPTER_BIT_REQUEST_WIFI                     (1<<LOC_API_ADAPTER_REQUEST_WIFI)
#define LOC_API_ADAPTER_BIT_SENSOR_STATUS                    (1<<LOC_API_ADAPTER_SENSOR_STATUS)
#define LOC_API_ADAPTER_BIT_REQUEST_TIME_SYNC                (1<<LOC_API_ADAPTER_REQUEST_TIME_SYNC)
#define LOC_API_ADAPTER_BIT_REPORT_SPI                       (1<<LOC_API_ADAPTER_REPORT_SPI)
#define LOC_API_ADAPTER_BIT_REPORT_NI_GEOFENCE               (1<<LOC_API_ADAPTER_REPORT_NI_GEOFENCE)
#define LOC_API_ADAPTER_BIT_GEOFENCE_GEN_ALERT               (1<<LOC_API_ADAPTER_GEOFENCE_GEN_ALERT)
#define LOC_API_ADAPTER_BIT_REPORT_GENFENCE_BREACH           (1<<LOC_API_ADAPTER_REPORT_GENFENCE_BREACH)
#define LOC_API_ADAPTER_BIT_BATCHED_GENFENCE_BREACH_REPORT   (1<<LOC_API_ADAPTER_BATCHED_GENFENCE_BREACH_REPORT)
#define LOC_API_ADAPTER_BIT_PEDOMETER_CTRL                   (1<<LOC_API_ADAPTER_PEDOMETER_CTRL)
#define LOC_API_ADAPTER_BIT_MOTION_CTRL                      (1<<LOC_API_ADAPTER_MOTION_CTRL)
#define LOC_API_ADAPTER_BIT_REQUEST_WIFI_AP_DATA             (1<<LOC_API_ADAPTER_REQUEST_WIFI_AP_DATA)
#define LOC_API_ADAPTER_BIT_BATCH_FULL                       (1<<LOC_API_ADAPTER_BATCH_FULL)
#define LOC_API_ADAPTER_BIT_BATCHED_POSITION_REPORT          (1<<LOC_API_ADAPTER_BATCHED_POSITION_REPORT)
#define LOC_API_ADAPTER_BIT_GDT_UPLOAD_BEGIN_REQ             (1<<LOC_API_ADAPTER_GDT_UPLOAD_BEGIN_REQ)
#define LOC_API_ADAPTER_BIT_GDT_UPLOAD_END_REQ               (1<<LOC_API_ADAPTER_GDT_UPLOAD_END_REQ)
#define LOC_API_ADAPTER_BIT_GNSS_MEASUREMENT                 (1<<LOC_API_ADAPTER_GNSS_MEASUREMENT)
#define LOC_API_ADAPTER_BIT_REQUEST_TIMEZONE                 (1<<LOC_API_ADAPTER_REQUEST_TIMEZONE)
#define LOC_API_ADAPTER_BIT_REPORT_GENFENCE_DWELL            (1<<LOC_API_ADAPTER_REPORT_GENFENCE_DWELL_REPORT)
#define LOC_API_ADAPTER_BIT_REQUEST_GTPWWAN_DATA             (1<<LOC_API_ADAPTER_REQUEST_GTPWWAN_DATA)

typedef uint8_t                         LocGnssConstellationType;
#define LOC_GNSS_CONSTELLATION_UNKNOWN      0
#define LOC_GNSS_CONSTELLATION_GPS          1
#define LOC_GNSS_CONSTELLATION_SBAS         2
#define LOC_GNSS_CONSTELLATION_GLONASS      3
#define LOC_GNSS_CONSTELLATION_QZSS         4
#define LOC_GNSS_CONSTELLATION_BEIDOU       5
#define LOC_GNSS_CONSTELLATION_GALILEO      6

// This max backoff value indicates no gnss session on Modem
#define LOC_CLIENT_MAX_BACKOFF_IN_SECS    (0xFFFF)
#define LOC_CLIENT_MAX_BACKOFF_IN_MILLIS  (LOC_CLIENT_MAX_BACKOFF_IN_SECS * 1000)

#ifdef FEATURE_LOCATION_MCS
#define SCAN_OBS_DATA_MAX 5
#define WWAN_ASSIST_DATA_MAX_AIT 2
#define ASSIST_CHANNEL_LIST_MAX 32
#define ASSIST_PLID_LIST_MAX 8
#define WWAN_SCAN_AITINFO_MAX 2
#define CHANNEL_INFO_MAX 32
#define CELL_INFO_MAX 8
#endif

typedef pthread_t(*gps_create_thread)(const char* name, void(*start)(void *), void* arg);

/** Milliseconds since January 1, 1970 */
typedef int64_t GpsUtcTime;

/** Represents a location. */
typedef struct {
    /** set to sizeof(GpsLocation) */
    size_t          size;
    /** Contains GpsLocationFlags bits. */
    uint16_t        flags;
    /** Represents latitude in degrees. */
    double          latitude;
    /** Represents longitude in degrees. */
    double          longitude;
    /**
    * Represents altitude in meters above the WGS 84 reference ellipsoid.
    */
    double          altitude;
    /** Represents speed in meters per second. */
    float           speed;
    /** Represents heading in degrees. */
    float           bearing;
    /** Represents expected accuracy in meters. */
    float           accuracy;
    /** Timestamp for the location fix. */
    GpsUtcTime      timestamp;
} GpsLocation;

typedef struct {

    uint8_t homeMncValid;
    /**<   Boolean indicating if homeMnc is valid */

    uint32_t homeMcc;
    /**<   MCC of home carrier. */

    uint32_t homeMnc;
    /**<   MNC of home carrier. */
} HomeCarrierIdStructT;  /* Type */
typedef struct __attribute__ ((packed)) {

    uint8_t unspecifiedInternal;
    /**<	 Unspecified Internal.
      - Range: 0 to 100% */

    uint8_t unspecifiedExternal;
    /**<	 Unspecified External.
      - Range: 0 to 100% */

    uint8_t gnss;
    /**<	 GNSS.
      - Range: 0 to 100% */

    uint8_t sensors;
    /**<	 Sensors.
      - Range: 0 to 100% */

    uint8_t wifi;
    /**<	 Wifi.
      - Range: 0 to 100% */

    uint8_t wwan;
    /**<	 WWAN.
      - Range: 0 to 100% */
} LocPositionSource;
#ifdef FEATURE_LOCATION_MCS
typedef struct __attribute__ ((packed)) {
	float horizontalSpeed;
	float horizontalSpeedUnc;
	float heading;
	float headingUnc;
}VelocityInfo;
#endif

typedef struct __attribute__ ((packed)) {

    double latitude;

    double longitude;

    float horizontalUnc;

    float altitude;

    float altitudeUnc;

    LocPositionSource latestPositionSources;
#ifdef FEATURE_LOCATION_MCS
	VelocityInfo velocityInfo;
#endif
} LocBsBestPosition;

typedef uint64_t BsObservedDataValidMask;
#define BS_OBSERVED_DATA_MASK_VALID_GPSWEEK_V02 ((BsObservedDataValidMask)0x00000001ull) /**<  GPS week field is valid in BS observed data */
#define BS_OBSERVED_DATA_MASK_VALID_GPSMSEC_V02 ((BsObservedDataValidMask)0x00000002ull) /**<  GPS Milliseconds field is valid in BS observed data */
#define BS_OBSERVED_DATA_MASK_VALID_LATITUDE_V02 ((BsObservedDataValidMask)0x00000004ull) /**<  Latitude field is valid in BS observed data */
#define BS_OBSERVED_DATA_MASK_VALID_LONGITUDE_V02 ((BsObservedDataValidMask)0x00000008ull) /**<  Longitude field is valid in BS observed data */
#define BS_OBSERVED_DATA_MASK_VALID_HOR_UNC_V02 ((BsObservedDataValidMask)0x00000010ull) /**<  Horizontal Uncertainty field is valid in BS observed data */
#define BS_OBSERVED_DATA_MASK_VALID_ALTITUDE_V02 ((BsObservedDataValidMask)0x00000020ull) /**<  Altitude field is valid in BS observed data */
#define BS_OBSERVED_DATA_MASK_VALID_ALT_UNC_V02 ((BsObservedDataValidMask)0x00000040ull) /**<  Altitude Uncertainty field is valid in BS observed data */
#define BS_OBSERVED_DATA_MASK_VALID_SPEED_V02 ((BsObservedDataValidMask)0x00000080ull) /**<  Speed field is valid in BS observed data */
#define BS_OBSERVED_DATA_MASK_VALID_SPEED_UNC_V02 ((BsObservedDataValidMask)0x00000100ull) /**<  Speed uncertainty field is valid in BS observed data */
#define BS_OBSERVED_DATA_MASK_VALID_HEADING_V02 ((BsObservedDataValidMask)0x00000200ull) /**<  Heading field is valid in BS observed data */
#define BS_OBSERVED_DATA_MASK_VALID_HEADING_UNC_V02 ((BsObservedDataValidMask)0x00000400ull) /**<  Heading Uncertainty field is valid in BS observed data */
typedef enum {
    AIRINTERFACETYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
    AIR_INTERFACE_TYPE_CDMA_V02 = 0, /**<  FDCL CDMA cell  */
    AIR_INTERFACE_TYPE_GSM_V02 = 1, /**<  FDCL GSM cell  */
    AIR_INTERFACE_TYPE_WCDMA_V02 = 2, /**<  FDCL WCDMA cell  */
    AIR_INTERFACE_TYPE_LTE_V02 = 3, /**<  FDCL LTE cell  */
    AIR_INTERFACE_TYPE_LTE_M1_V02 = 4, /**<  FDCL LTE-M1 cell  */
    AIR_INTERFACE_TYPE_LTE_NB1_V02 = 5, /**<  FDCL LTE-NB1 cell  */
    AIR_INTERFACE_TYPE_LTE_M1_MODE_A_V02 = 6, /**<  FDCL LTE-M1ModeA cell  */
    AIR_INTERFACE_TYPE_LTE_M1_MODE_B_V02 = 7, /**<  FDCL LTE-M1ModeB cell  */
    AIR_INTERFACE_TYPE_LTE_UNKNOWN_V02 = 8, /**<  FDCL LTE-Unknown cell  */
    AIRINTERFACETYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}AirInterfaceTypeEnum;

  typedef enum {
    CELLRECORDTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
    CELL_RECORD_TYPE_INVALID_V02 = 0, /**<  Invalid value  */
    CELL_RECORD_TYPE_L1_REGION_WITH_POSITION_V02 = 1, /**<  L1 region with position  */
    CELL_RECORD_TYPE_L2_REGION_WITH_POSITION_V02 = 2, /**<  L2 region with position  */
    CELL_RECORD_TYPE_L3_REGION_WITH_POSITION_V02 = 3, /**<  L3 region with position  */
    CELL_RECORD_TYPE_L4_CELL_RECORD_WITH_POSITION_V02 = 4, /**<  L4 cell record with position  */
    CELL_RECORD_TYPE_L4_CELL_RECORD_NOT_RESOLVED_V02 = 5, /**<  L4 cell record not resolved  */
    CELL_RECORD_TYPE_L4_CELL_RECORD_NO_INFO_V02 = 6, /**<  L4 cell record no information	*/
    CELLRECORDTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}CellRecordTypeEnum;

typedef enum {
    RELIABILITYENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.	Do not change or use*/
    LOC_RELIABILITY_NOT_SET_V02 = 0, /**<  Location reliability is not set  */
    LOC_RELIABILITY_VERY_LOW_V02 = 1, /**<  Location reliability is very low; use it at your own risk  */
    LOC_RELIABILITY_LOW_V02 = 2, /**<  Location reliability is low; little or no cross-checking is possible  */
    LOC_RELIABILITY_MEDIUM_V02 = 3, /**<	Location reliability is medium; limited cross-check passed	 */
    LOC_RELIABILITY_HIGH_V02 = 4, /**<  Location reliability is high; strong cross-check passed  */
    RELIABILITYENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
} LocReliabilityEnum;

typedef enum {
    GTPSTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
    GTPWWAN_DOWNLOAD_SUCCESS_V02 = 0, /**<  Download success  */
    GTPWWAN_QMI_INTIALIZATION_FAIL_V02 = 1, /**<  Failure in intialization  */
    GTPWWAN_QMI_FDCL_LIST_FAIL_V02 = 2, /**<  Failure in getting fdcl list from QMI  */
    GTPWWAN_ENCODE_FAIL_V02 = 3, /**< Failure during binary encoding */
    GTPWWAN_DATA_SERVICE_FAIL_V02 = 4, /**<  Data Service not available  */
    GTPWWAN_SSL_FAIL_V02 = 5, /**<  Failure in SSL  */
    GTPWWAN_CONNECTION_FAIL_V02 = 6, /**<  Failure in http connection  */
    GTPWWAN_HTTP_REQUEST_FAIL_V02 = 7, /**<  Failure in sending request to server */
    GTPWWAN_HTTP_DOWNLOAD_ERROR_V02 = 8, /**<  Failure in HTTP download */
    GTPWWAN_SERVER_REJECT_V02 = 9, /**<  Request rejected by server */
    GTPWWAN_DECODE_FAIL_V02 = 10, /**<  Failure during decode */
    GTPWWAN_UNSPECIFIED_FAILURE_V02 = 11, /**< Unspecified failure */
    GTPSTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}GtpStatusEnum;

  typedef uint32_t FdclCellPosValidMask;
#define FDCL_CELL_POS_MASK_VALID_ALTITUDE_V02 ((FdclCellPosValidMask)0x00000001) /**<  Altitude field is valid in cell position  */
#define FDCL_CELL_POS_MASK_VALID_ALT_UNC_V02 ((FdclCellPosValidMask)0x00000002) /**<  Altitude uncertainty field is valid in cell position  */
#define FDCL_CELL_POS_MASK_VALID_ALT_CONFIDENCE_V02 ((FdclCellPosValidMask)0x00000004) /**<  Altitude confidence is valid in cell position  */
#define FDCL_CELL_POS_MASK_VALID_ALT_RELIABILITY_V02 ((FdclCellPosValidMask)0x00000008) /**<  Altitude reliability field is valid in cell position  */
#define FDCL_CELL_POS_MASK_VALID_VELOCITY_V02 ((FdclCellPosValidMask)0x00000010) /**<  velocity field is valid in cell position  */

enum optionalGenericTimeStampFieldMask
{

    GPS_TIME_PRESENT  = (1 << 0),
    TIME_TICK_SEC_PRESENT  = (1 << 1),
    UTC_TIME_PRESENT  = (1 << 2),
    GPS_TIME_MSEC_PRESENT = (1 << 3),
    TIME_UNCERTAINTY_PRESENT = (1 << 4)
};


typedef uint8_t OptionalBandMask;
#define RF_BANDS_LTE (1 << 0)
#define	RF_BANDS_GSM (1 << 1)

typedef uint8_t CapabilitiesMask;
#define GTP20_SUPPORTED  (1 << 0)
#define WIFI_SUPPORTED   (1 << 1)
#define GNSS_SUPPORTED   (1 << 2)
#define GTP_20_CACHE_FIX (1 << 3)





enum OemModelSourceMask
{

    CONFIGURATIONFILE_OR_NV  = (1 << 0),
    QWES_CONFIGURATION  = (1 << 1)
};


enum RFBandLTEBandMaskIndex
{
	LTE_BANDS_INDEX_1_16 = 0,
	LTE_BANDS_INDEX_17_32,
	LTE_BANDS_INDEX_33_48,
	LTE_BANDS_INDEX_49_64,
	LTE_BANDS_INDEX_65_80,
	LTE_BANDS_INDEX_81_96,
	LTE_BANDS_INDEX_97_112,
	LTE_BANDS_INDEX_113_128,
	LTE_BANDS_INDEX_129_144,
	LTE_BANDS_INDEX_145_160,
	LTE_BANDS_INDEX_161_176,
	LTE_BANDS_INDEX_177_192,
	LTE_BANDS_INDEX_193_208,
	LTE_BANDS_INDEX_209_224,
	LTE_BANDS_INDEX_225_240,
	LTE_BANDS_INDEX_241_256
};


enum RFBandLTESubBandMask
{
	LTE_BANDS_1_16    = (1 << LTE_BANDS_INDEX_1_16),
	LTE_BANDS_17_32   = (1 << LTE_BANDS_INDEX_17_32),
	LTE_BANDS_33_48   = (1 << LTE_BANDS_INDEX_33_48),
	LTE_BANDS_49_64   = (1 << LTE_BANDS_INDEX_49_64),
	LTE_BANDS_65_80   = (1 << LTE_BANDS_INDEX_65_80),
	LTE_BANDS_81_96   = (1 << LTE_BANDS_INDEX_81_96),
	LTE_BANDS_97_112  = (1 << LTE_BANDS_INDEX_97_112),
	LTE_BANDS_113_128 = (1 << LTE_BANDS_INDEX_113_128),
	LTE_BANDS_129_144 = (1 << LTE_BANDS_INDEX_129_144),
	LTE_BANDS_145_160 = (1 << LTE_BANDS_INDEX_145_160),
	LTE_BANDS_161_176 = (1 << LTE_BANDS_INDEX_161_176),
	LTE_BANDS_177_192 = (1 << LTE_BANDS_INDEX_177_192),
	LTE_BANDS_193_208 = (1 << LTE_BANDS_INDEX_193_208),
	LTE_BANDS_209_224 = (1 << LTE_BANDS_INDEX_209_224),
	LTE_BANDS_225_240 = (1 << LTE_BANDS_INDEX_225_240),
	LTE_BANDS_241_256 = (1 << LTE_BANDS_INDEX_241_256)
};




enum CellOptFieldMask 
{
	CHANNEL_NUM_PRESENT = (1 << 0),
	PLID_PRESENT = (1 << 1),
	NB_CARRIER_FREQ_OFFSET_PRESENT = (1 << 2),
	NB_CHANNEL_RASTER_OFFSET_PRESENT = (1 << 3)
};

enum RequestedDataTypesMask
{
	L4_CELLS_REQUESTED = (1 << 0),
	L3_CELLS_REQUESTED = (1 << 1),
	L2_CELLS_REQUESTED = (1 << 2),
	RESTRICTED_REGION_REQUESTED = (1 << 3),
	WWAN_SCAN_ASSIST_DATA_REQUESTED = (1 << 4),
	TIME_ASSIST_DATA_REQUESTED = (1 << 5),
	WWAN_SCAN_POSITION_REQUESTED = (1 << 6)
};

typedef struct __attribute__ ((packed)) {
    AirInterfaceTypeEnum airInterfaceType;
    uint32_t id1;
    uint32_t id2;
    uint32_t id3;
    uint32_t id4;
	uint8_t cellOptFields;
	uint32_t channelNum;
	uint16_t plId;
	uint8_t NBCarrierFreqOffset;
	uint8_t NBChannelRasterOffset;
}LocCellId;

#ifdef FEATURE_LOCATION_MCS
typedef struct __attribute__ ((packed)) {
	
	uint8_t timeStampMask; /* optionalGenericTimeStampFieldMask */
	uint16_t gpsWeek;
	uint32_t gpsSeconds;
	uint32_t timeTicSeconds;
	uint32_t UtcTimestamp;
	uint32_t gpsMilliseconds; //millsec of GPS week
	uint8_t timeUncertainty;
}GenericTimeStamp;
#endif

#define RF_LTE_BANDS_MASK 16
#define MODEL_SW_VERSION_MAX 256
#define OEM_ID_MAX 256
#define MODEL_ID_MAX 256
#define CDMA_REALM_MAX 256


typedef struct {

    /* Optional */
    /*  GTP client software version */
    uint8_t gtpClientSwVer_valid;
    uint8_t gtpClientSwMajorVersion;
    uint8_t gtpClientSwMinorVersion;

    uint8_t modemSwVersion_valid;
    char modemSwVersion[MODEL_SW_VERSION_MAX];

    uint8_t homeCarrierId_valid;
    uint32_t homeMccId;
    uint8_t homeMncValid;
    uint32_t homeMncId;

    uint8_t gpsWeek_valid;  /**< Must be set to true if gpsWeek is being passed */
    uint16_t gpsWeek;
	uint32_t gpsSeconds;

    uint8_t gpsMilliseconds_valid;  /**< Must be set to true if gpsMilliseconds is being passed */
    uint32_t gpsMilliseconds;

    uint8_t cdmaRealm_valid;
    char cdmaRealm[CDMA_REALM_MAX];
    uint8_t gtpMaxNumRecordsAccepted_valid;  /**< Must be set to true if gtpMaxNumRecordsAccepted is being passed */
    uint16_t gtpMaxNumRecordsAccepted;
    uint8_t currentCellId_valid;  /**< Must be set to true if currentCellId is being passed */
    LocCellId currentCellId;

    uint8_t requestedDataTypes_valid;
    uint8_t requestedDataTypes;

    uint8_t restrictedRegionVersion_valid;
    uint8_t restrictedRegionVersion;

    uint8_t bsBestPosition_valid;
    LocBsBestPosition bestPosition;

	char OemId[OEM_ID_MAX];
	char modelId[MODEL_ID_MAX];
	OemModelSourceMask oemModelSourceMask;
	CapabilitiesMask capabilityMask;
	OptionalBandMask bandMask;
	uint8_t timeUncertainty;
	//RFBandLTESubBandMask rfBandLTESubBandMask;
	uint16_t rfBandLTESubBandMask;
	uint16_t rfBandsLTEMask[RF_LTE_BANDS_MASK];
	uint16_t rfBandsGSM;
	uint8_t laasId_valid;
	uint64_t laasId;
	uint8_t configId_valid;
	uint8_t configId;
	uint8_t scanAssistId_valid; /** scan assist id will the the current cell id */
	uint64_t scanAssistId;


} clientinformation;  /* Message */

typedef struct {

    BsObservedDataValidMask validMask;
    /**< Bitmask indicating which of the fields in this TLV are valid.
      Valid bitmasks: \n
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_GPSWEEK (0x00000001) --  GPS week field is valid in BS observed data
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_GPSMSEC (0x00000002) --  GPS Milliseconds field is valid in BS observed data
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_LATITUDE (0x00000004) --  Latitude field is valid in BS observed data
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_LONGITUDE (0x00000008) --  Longitude field is valid in BS observed data
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_HOR_UNC (0x00000010) --  Horizontal Uncertainty field is valid in BS observed data
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_ALTITUDE (0x00000020) --  Altitude field is valid in BS observed data
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_ALT_UNC (0x00000040) --  Altitude Uncertainty field is valid in BS observed data
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_SPEED (0x00000080) --  Speed field is valid in BS observed data
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_SPEED_UNC (0x00000100) --  Speed uncertainty field is valid in BS observed data
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_HEADING (0x00000200) --  Heading field is valid in BS observed data
      - QMI_LOC_BS_OBSERVED_DATA_MASK_VALID_HEADING_UNC (0x00000400) --  Heading Uncertainty field is valid in BS observed data
      */

    LocCellId cellid;

    uint16_t gpsWeek;
    /**<	 GPS week \n
      - Units: Weeks */

    uint32_t gpsMilliseconds;
    /**<	 GPS milliseconds \n
      - Units: Milliseconds */
    uint32_t bsLocalTimestamp;
    /**<   Last observed local timestamp for the cell */

    int32_t posDeltaTime;
    /**<   Delta time between position and cell ID detection \n
      - Units: Milliseconds */

    float speed;
    /**<	 Horizontal speed. \n
      - Units: Meters/second */

    float speedUnc;
    /**<	 Horizontal speed uncertainty. \n
      - Units: Meters/second */

    double heading;
    /**<	 Heading. \n
      - Units: Radian */

    double headingUnc;
    /**<	 Heading uncertainty. \n
      - Units: Radian */

    LocBsBestPosition bestPosition;
    /**<	 Base Station Best Position */

} LocBsObservedData;

#define LOC_FDCL_BS_LIST_MAX_SIZE_V02        10
#define LOC_OBS_BS_LIST_MAX_SIZE_V02         10
#define LOC_INJECT_LIST_MAX_SIZE_V02         10
#define LOC_RESTRICTED_REGIONS_LIST_MAX_SIZE 20

#ifdef FEATURE_LOCATION_MCS
enum ScanOptionalFieldMask{
    SCAN_CS_CONFIGURATION  = (1 << 0),
    SCAN_CS_ACTIVE_CONFIG   = (1 << 1),
    SCAN_POS_CONFIGURATION  = (1 << 2)
};

enum WwanScanCellOptFieldMask 
{
	ID1_PRESENT = (1 << 0),
	ID2_PRESENT = (1 << 1),
	ID3_PRESENT = (1 << 2),
	ID4_PRESENT = (1 << 3),
	CELLFLAGS_PRESENT = (1 << 4),
	POWER_MEAS_PRESENT = (1 << 5),
	NBCARRIER_FREQ_OFFSET_PRESENT = (1 << 6),
	NBCHANN_RASTER_OFFSET_PRESENT = (1 << 7)
};

typedef struct __attribute__ ((packed)) {
	uint16_t plid;
	uint16_t cellOptFields; /* WwanScanCellOptFieldMask */
	uint16_t id1;
	uint16_t id2;
	uint16_t id3;
	uint32_t id4;
	uint8_t cellFlags;
	uint8_t powerMeasurments;
	uint8_t NBCarrierFreqOffset;
	uint8_t NBChannelRasterOffset;
}CellInfo;

typedef struct __attribute__ ((packed)) {
	uint32_t channelNum : 24;
	uint8_t channelflags;
	uint16_t channelScanTime;
	uint8_t numOfCells;
	CellInfo cellInfo[CELL_INFO_MAX];
}ChannelInfo;

typedef struct __attribute__ ((packed)) {
	uint8_t AirInterfaceType;
	GenericTimeStamp AITScanStartTime;
	uint8_t numOfChannels;
	ChannelInfo channelInfo[CHANNEL_INFO_MAX];
}AirInterfaceInfo;


typedef struct __attribute__ ((packed)) {
    uint8_t AirInterfaceType;
    GenericTimeStamp AITScanStartTime;
    uint8_t numOfChannels;
    ChannelInfo channelInfo;
}ServingAirInterfaceInfo;

typedef struct __attribute__ ((packed))
{
	uint16_t length;
	uint8_t numOfAITs;
	AirInterfaceInfo AITInfo[WWAN_SCAN_AITINFO_MAX];
	ServingAirInterfaceInfo ServingAITInfo;
	uint32_t crc32;
}WwanScanList;

#endif

typedef struct __attribute__ ((packed))  {

    uint32_t BsList_len;	/**< Must be set to # of elements in BsList */
    LocCellId BsList[LOC_FDCL_BS_LIST_MAX_SIZE_V02];

    /*  Base Station Best Position */
    uint8_t bsBestPosition_valid;  /**< Must be set to true if bsBestPosition is being passed */
    LocBsBestPosition bsBestPosition;
#ifdef FEATURE_LOCATION_MCS
	uint8_t wwanScanList_valid;
	WwanScanList wwanScanList;
#endif
#if 0
LocFdclBsList()
{
}
LocFdclBsList(const LocFdclBsList& a)
{
	BsList_len = a.BsList_len;
    for (int i = 0; i<a.BsList_len; i++) 
	{
    	BsList[i].airInterfaceType = a.BsList[i].airInterfaceType;
    	BsList[i].id1 = a.BsList[i].id1;
    	BsList[i].id2 = a.BsList[i].id2;
    	BsList[i].id3 = a.BsList[i].id3;
    	BsList[i].id4 = a.BsList[i].id4;
    }
	memset(&bsBestPosition, 0, sizeof(LocBsBestPosition));
	wwanScanList_valid = 0;
	memset(&wwanScanList, 0, sizeof(WwanScanList));
}
#endif
}LocFdclBsList;  /* Message */

typedef struct {
    uint32_t bsData_len;	/**< Must be set to # of elements in bsData */
    LocBsObservedData bsData[LOC_OBS_BS_LIST_MAX_SIZE_V02];
    /**<	 \vspace{4pt} \n A list of base station IDs for FDCL request. */
} LocBsObsData;

typedef struct {

    FdclCellPosValidMask validMask;
    /**<   Bitmask indicating which of the fields in this TLV are valid.

      Valid bitmasks: \n
      - QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALTITUDE (0x00000001) --  Altitude field is valid in cell position
      - QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_UNC (0x00000002) --  Altitude uncertainty field is valid in cell position
      - QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_CONFIDENCE (0x00000004) --  Altitude confidence is valid in cell position
      - QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_RELIABILITY (0x00000008) --  Altitude reliability field is valid in cell position
      */

    LocCellId cellId;
    /**<   \vspace{4pt} \n The cell ID for this record. */

    double latitude;
    /**<   Latitude (specified in WGS84 datum).
      \begin{itemize1}
      \item    Type: Floating point
      \item    Units: Degrees
      \item    Range: -90.0 to 90.0       \begin{itemize1}
      \item    Positive values indicate northern latitude
      \item    Negative values indicate southern latitude
      \vspace{-0.18in} \end{itemize1} \end{itemize1}
      */

    double longitude;
    /**<   Longitude (specified in WGS84 datum).
      \begin{itemize1}
      \item    Type: Floating point
      \item    Units: Degrees
      \item    Range: -180.0 to 180.0     \begin{itemize1}
      \item    Positive values indicate eastern longitude
      \item    Negative values indicate western longitude
      \vspace{-0.18in} \end{itemize1} \end{itemize1}
      */

    float horCoverageRadius;
    /**<   Horizontal coverage radius (circular).\n
      - Units: Meters */

    uint8_t horConfidence;
    /**<   Horizontal confidence, as defined by ETSI TS 101 109 (3GPP \hyperref[TS 03.32]{TS 03.32).
      \begin{itemize1}
      \item    Units: Percent (1 to 99)
      \item    0, 101 to 255 -- invalid value
      \item    If 100 is received, reinterpret to 99
      \end{itemize1}
      */

    LocReliabilityEnum horReliability;
    /**<   Specifies the reliability of the horizontal position.

      Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Location reliability is not set
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Location reliability is very low; use it at your own risk
      - eQMI_LOC_RELIABILITY_LOW (2) --  Location reliability is low; little or no cross-checking is possible
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Location reliability is medium; limited cross-check passed
      - eQMI_LOC_RELIABILITY_HIGH (4) --  Location reliability is high; strong cross-check passed
      */

    float altitude;
    /**<   Altitude with respect to mean sea level.\n
      - Units: Meters */

    float altUnc;
    /**<   Vertical uncertainty. This is mandatory if either altitudeWrtEllipsoid
      or altitudeWrtMeanSeaLevel is specified.\n
      - Units: Meters */

    uint8_t altConfidence;
    /**<   Vertical confidence, as defined by  ETSI TS 101 109 (3GPP \hyperref[TS 03.32]{TS 03.32}).
      \begin{itemize1}
      \item    Units: Percent (0-99)
      \item    0 -- invalid value
      \item    100 to 256 -- not used
      \item    If 100 is received, reinterpret to 99
      \end{itemize1}
      */

    LocReliabilityEnum altReliability;
    /**<   Specifies the reliability of the vertical position.

      Valid values: \n
      - eQMI_LOC_RELIABILITY_NOT_SET (0) --  Location reliability is not set
      - eQMI_LOC_RELIABILITY_VERY_LOW (1) --  Location reliability is very low; use it at your own risk
      - eQMI_LOC_RELIABILITY_LOW (2) --  Location reliability is low; little or no cross-checking is possible
      - eQMI_LOC_RELIABILITY_MEDIUM (3) --  Location reliability is medium; limited cross-check passed
      - eQMI_LOC_RELIABILITY_HIGH (4) --  Location reliability is high; strong cross-check passed
      */
} FdclCellPosStructT;  /* Type */
enum LocationMask
{
	ALTITUDE_PRESENT = (1 << 0),
	ALTITUDE_UNC_PRESENT = ( 1<< 1),
	LOCATION_TIMESTAMP_PRESENT = ( 1<< 2),
	LOCATION_SOURCE_PRESENT = (1 << 3),
	VELOCITY_INFORMATION_PRESENT = (1 << 4)
};

#ifdef FEATURE_LOCATION_MCS
enum TTOptionalMask
{
    TARGET_HOUR_PRESENT  = (1 << 1),
    TARGET_DAY_PRESENT  = (1 << 2),
    TARGET_WEEK_PRESENT = (1 << 3)
};

enum ChannelOptionalFieldMask
{
    CHANNEL_PLIDS_PRESENT  = (1 << 0)
};

enum PlidOptionalFieldMask
{
    NB_CARRIER_FREQ_OFFSET  = (1 << 0),
	NB_CHANNEL_RASTER_OFFSET = (1 << 1)
};


typedef struct __attribute__ ((packed)) {
	uint16_t plid;
	uint8_t plidOptFields; /* enum PlidOptionalFieldMask */
	uint8_t NBCarrierFreqOffset;
	uint8_t NBChannelRasterOffset;
}AssistPlidList;


typedef struct __attribute__ ((packed)) {
	uint32_t channelNum : 24;
	uint8_t channelFlags : 4;
	uint8_t channelOptFields; /* enum ChannelOptionalFieldMask */
	uint8_t channelPLIDs_valid;
	uint8_t numPlids;
	AssistPlidList assistPlidList[ASSIST_PLID_LIST_MAX];
}AssistChannelList;


typedef struct __attribute__ ((packed)) {
	uint8_t AITInterfaceType;
	uint8_t NumOfChannels;
	AssistChannelList assistChannelList[ASSIST_CHANNEL_LIST_MAX];
}AssistAITList;

typedef struct __attribute__ ((packed)) {
	uint64_t ScanAssistId;
	uint8_t NumAITs;
	AssistAITList assistAITList[WWAN_ASSIST_DATA_MAX_AIT];
}WWanScanAssistanceData;

typedef struct __attribute__ ((packed)) {
	uint8_t maxScanCsDataSets : 4;
	uint8_t maxSIB1DecodeCS : 4;
	uint8_t csPunc99Limit : 4;
	uint8_t csTechSwitchPunc99 : 4;
	uint8_t scanCsDeltaTimeThrhd;
	uint8_t csScanDurationLTE: 4;
	uint8_t csScanDurationNB1 : 4;
	uint8_t csScanDurationGSM : 4;
	uint8_t csPeriodicity : 4;
	uint8_t primaryAITCount: 4;
	uint8_t secondaryAITCount: 4;
	
}ScanCsConfigInfo;

typedef struct __attribute__ ((packed)) {
	/* CS_TARGET_TIME */
	uint8_t ttOptFieldMask: 4; /* enum TTOptionalMask */
	uint8_t targetMin: 4;
	uint8_t targetHour_valid;
	uint8_t targetHour;
	uint8_t targetDay_valid;
	uint8_t targetDay;
	uint8_t targetWeek_valid;
	uint8_t targetWeek;

	/* CS_TARGET_TIME_INC */
	uint8_t ttIncOptFieldMask: 4;
	uint8_t targetIncMin: 4;
	uint8_t targetIncHour_valid;
	uint8_t targetIncHour;
	uint8_t targetIncDay_valid;
	uint8_t targetIncDay;
	uint8_t targetIncWeek_valid;
	uint8_t targetIncWeek;
}ScanCsActivConfig;

typedef struct __attribute__ ((packed)) {
	uint8_t scanPosDurationLTE : 4;
	uint8_t scanPosDurationNB1 : 4;
	uint8_t scanPosDurationGSM : 4;
	uint8_t maxSIB1DecodePos : 4;
	uint8_t maxCellsPerChannel : 4;
	uint8_t maxAITPerScan : 4;
	uint8_t scanUpdateMinChnCount: 4;
	uint8_t scanUpdateMinPLIDCount: 4;
}ScanPosConfig;

typedef struct __attribute__ ((packed)) {
	uint8_t scanOptFields;
	/* SCAN_CS_CONFIGURATION */
	uint8_t scanCSconfig_valid;
	ScanCsConfigInfo scanCsConfigInfo;
	/* SCAN_CS_ACTIVE_CONFIG */
	uint8_t scanCsActiveConfig_valid;
	ScanCsActivConfig scanCsActiveConfig;
	/* SCAN_POS_CONFIGURATION */
	uint8_t scanPosConfig_valid;
	ScanPosConfig scanPosConfig;
}ClientConfigScanInfo;

typedef struct __attribute__ ((packed)) {
	//uint16_t length;
	uint8_t optFieldMask_2;
	uint8_t clientControl_2;
	uint8_t clientConfigScan_valid;
	ClientConfigScanInfo clientConfigScanInfo;

	uint8_t scanAssistanceData_valid;
	WWanScanAssistanceData wwanScanAssistanceData;

	uint8_t timeAssistanceData_valid;
	GenericTimeStamp genericTimeStamp;

	uint8_t scanCacheFixConfig_valid;
	uint8_t minDetChnCountOverlap;
	uint8_t minDetChnPercentOverlap;

  uint8_t configId_valid;
  uint8_t configId;

	//uint32_t crc32;
}GtpMCSResponse;


typedef struct __attribute__ ((packed))
{
	AirInterfaceInfo AITInfo;
	ServingAirInterfaceInfo ServingAITInfo;	
	GenericTimeStamp timestamp;
	uint8_t locationMask;
	LocBsBestPosition locBsBestPosition;
	uint16_t posDeltaTimeScan;
}ScanOBSUploadData;
#endif //FEATURE_LOCATION_MCS

typedef struct {

    /* Mandatory */
    /*  FDCL Cell Position list */
    uint32_t cellPosList_len;  /**< Must be set to # of elements in cellPosList */
    FdclCellPosStructT cellPosList[LOC_INJECT_LIST_MAX_SIZE_V02];
    /**<   Cell-position list. */

    GtpStatusEnum status;

    /* Mandatory */
    /*  Number of days this FDCL data is valid for */
    uint32_t daysValid;
    /**<   Days valid. */

    /* Optional */
    /*  UTC Timestamp */
    uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
    uint64_t timestampUtc;
    /**<   UTC timestamp.
      - Units: Milliseconds (since Jan. 1, 1970) */

    /* Optional */
    /*  Cell Record Type List */
    uint8_t cellRecordTypeList_valid;  /**< Must be set to true if cellRecordTypeList is being passed */
    uint32_t cellRecordTypeList_len;  /**< Must be set to # of elements in cellRecordTypeList */
    CellRecordTypeEnum cellRecordTypeList[LOC_INJECT_LIST_MAX_SIZE_V02];
    /**<   A list of cell record types for FDCL request.
      The cell record type list shall be aligned with the records in cellPosList.
      Valid values: \n
      - eQMI_LOC_CELL_RECORD_TYPE_INVALID (0) --  Invalid value
      - eQMI_LOC_CELL_RECORD_TYPE_L1_REGION_WITH_POSITION (1) --  L1 region with position
      - eQMI_LOC_CELL_RECORD_TYPE_L2_REGION_WITH_POSITION (2) --  L2 region with position
      - eQMI_LOC_CELL_RECORD_TYPE_L3_REGION_WITH_POSITION (3) --  L3 region with position
      - eQMI_LOC_CELL_RECORD_TYPE_L4_CELL_RECORD_WITH_POSITION (4) --  L4 cell record with position
      - eQMI_LOC_CELL_RECORD_TYPE_L4_CELL_RECORD_NOT_RESOLVED (5) --  L4 cell record not resolved
      - eQMI_LOC_CELL_RECORD_TYPE_L4_CELL_RECORD_NO_INFO (6) --  L4 cell record no information
      */

    /* Optional */
    /*  More FDCL Data Available */
    uint8_t moreBsAvailable_valid;  /**< Must be set to true if moreBsAvailable is being passed */
    uint8_t moreBsAvailable;
    /**<   Indicates whether more FDCL data is available.
      - 0x00 (FALSE) -- No more FDCL data is available
      - 0x01 (TRUE)  -- More FDCL data is available
      If not specified, moreBsAvailable defaults to FALSE.
      */

    /* Optional */
    /*  Client Controls */
    uint8_t clientControls_valid;  /**< Must be set to true if clientControls is being passed */
    uint16_t clientControls;
    /**<   Client Controls */

    /* Optional */
    /*  Maximum Number of Cells to Upload */
    uint8_t maxNumCellsToUpload_valid;  /**< Must be set to true if maxNumCellsToUpload is being passed */
    uint16_t maxNumCellsToUpload;
    /**<   Maximum number of cells to upload */

    /* Optional */
    /*  Minimum Seconds Between Requests */
    uint8_t minSecsBtwRequests_valid;  /**< Must be set to true if minSecsBtwRequests is being passed */
    uint16_t minSecsBtwRequests;
    /**<   Minimum seconds between requests */

    /* Optional */
    /*  Restricted Region Version */
    uint8_t restrictedRegionVersion_valid;  /**< Must be set to true if restrictedRegionVersion is being passed */
    uint8_t restrictedRegionVersion;
    /**<   Restricted Region Version */

    /* Optional */
    /*  Maximum Number of Cells to Download */
    uint8_t maxNumCellsToDownload_valid;  /**< Must be set to true if maxNumCellsToDownload is being passed */
    uint16_t maxNumCellsToDownload;
    /**<   Maximum number of cells to Download. */

    /* Optional */
    /*  Maximum number of GTP cell requests with server access per day */
    uint8_t maxGtpCellRequestsPerDay_valid;  /**< Must be set to true if maxGtpCellRequestsPerDay is being passed */
    uint16_t maxGtpCellRequestsPerDay;
    /**<   Maximum number of GTP cell requests with server access per day. */

    /* Optional */
    /*  Maximum value of delta time in Secs to crowd source data.   */
    uint8_t posDeltaTimeThreshold_valid;  /**< Must be set to true if posDeltaTimeThreshold is being passed */
    uint32_t posDeltaTimeThreshold;
    /**<   Maximum value of delta time in Secs to crowd source data. \n
      - Units: Seconds */

    /* Optional */
    /*  List of Restricted Regions */
    uint8_t restrictedRegions_valid;  /**< Must be set to true if restrictedRegions is being passed */
    uint32_t restrictedRegions_len;  /**< Must be set to # of elements in restrictedRegions */
    HomeCarrierIdStructT restrictedRegions[LOC_RESTRICTED_REGIONS_LIST_MAX_SIZE];
    /**<   List of restricted regions */
	uint8_t positionResponse_valid;
	FdclCellPosStructT positionResponse;
#ifdef FEATURE_LOCATION_MCS
	uint8_t gtpMCSresponse_valid;
	GtpMCSResponse gtpMCSResponse;
#endif
} Xt_Data;

/**
* The callback associated with the geofence.
* Parameters:
*      geofence_id - The id associated with the add_geofence_area.
*      location    - The current GPS location.
*      transition  - Can be one of GPS_GEOFENCE_ENTERED, GPS_GEOFENCE_EXITED,
*                    GPS_GEOFENCE_UNCERTAIN.
*      timestamp   - Timestamp when the transition was detected.
*
* The callback should only be called when the caller is interested in that
* particular transition. For instance, if the caller is interested only in
* ENTERED transition, then the callback should NOT be called with the EXITED
* transition.
*
* IMPORTANT: If a transition is triggered resulting in this callback, the GPS
* subsystem will wake up the application processor, if its in suspend state.
*/
typedef void(*gps_geofence_transition_callback) (int32_t geofence_id, GpsLocation* location,
    int32_t transition, GpsUtcTime timestamp);

/**
* The callback associated with the availability of the GPS system for geofencing
* monitoring. If the GPS system determines that it cannot monitor geofences
* because of lack of reliability or unavailability of the GPS signals, it will
* call this callback with GPS_GEOFENCE_UNAVAILABLE parameter.
*
* Parameters:
*  status - GPS_GEOFENCE_UNAVAILABLE or GPS_GEOFENCE_AVAILABLE.
*  last_location - Last known location.
*/
typedef void(*gps_geofence_status_callback) (int32_t status, GpsLocation* last_location);

/**
* The callback associated with the add_geofence call.
*
* Parameter:
* geofence_id - Id of the geofence.
* status - GPS_GEOFENCE_OPERATION_SUCCESS
*          GPS_GEOFENCE_ERROR_TOO_MANY_GEOFENCES  - geofence limit has been reached.
*          GPS_GEOFENCE_ERROR_ID_EXISTS  - geofence with id already exists
*          GPS_GEOFENCE_ERROR_INVALID_TRANSITION - the monitorTransition contains an
*              invalid transition
*          GPS_GEOFENCE_ERROR_GENERIC - for other errors.
*/
typedef void(*gps_geofence_add_callback) (int32_t geofence_id, int32_t status);

/**
* The callback associated with the remove_geofence call.
*
* Parameter:
* geofence_id - Id of the geofence.
* status - GPS_GEOFENCE_OPERATION_SUCCESS
*          GPS_GEOFENCE_ERROR_ID_UNKNOWN - for invalid id
*          GPS_GEOFENCE_ERROR_GENERIC for others.
*/
typedef void(*gps_geofence_remove_callback) (int32_t geofence_id, int32_t status);

/**
* The callback associated with the pause_geofence call.
*
* Parameter:
* geofence_id - Id of the geofence.
* status - GPS_GEOFENCE_OPERATION_SUCCESS
*          GPS_GEOFENCE_ERROR_ID_UNKNOWN - for invalid id
*          GPS_GEOFENCE_ERROR_INVALID_TRANSITION -
*                    when monitor_transitions is invalid
*          GPS_GEOFENCE_ERROR_GENERIC for others.
*/
typedef void(*gps_geofence_pause_callback) (int32_t geofence_id, int32_t status);

/**
* The callback associated with the resume_geofence call.
*
* Parameter:
* geofence_id - Id of the geofence.
* status - GPS_GEOFENCE_OPERATION_SUCCESS
*          GPS_GEOFENCE_ERROR_ID_UNKNOWN - for invalid id
*          GPS_GEOFENCE_ERROR_GENERIC for others.
*/
typedef void(*gps_geofence_resume_callback) (int32_t geofence_id, int32_t status);

typedef struct {
    gps_geofence_transition_callback geofence_transition_callback;
    gps_geofence_status_callback geofence_status_callback;
    gps_geofence_add_callback geofence_add_callback;
    gps_geofence_remove_callback geofence_remove_callback;
    gps_geofence_pause_callback geofence_pause_callback;
    gps_geofence_resume_callback geofence_resume_callback;
    gps_create_thread create_thread_cb;
} GpsGeofenceCallbacks;

/** Extended interface for GPS_Geofencing support */
typedef struct {
    /** set to sizeof(GpsGeofencingInterface) */
    size_t          size;

    /**
    * Opens the geofence interface and provides the callback routines
    * to the implementation of this interface.
    */
    void(*init)(GpsGeofenceCallbacks* callbacks);

    /**
    * Add a geofence area. This api currently supports circular geofences.
    * Parameters:
    *    geofence_id - The id for the geofence. If a geofence with this id
    *       already exists, an error value (GPS_GEOFENCE_ERROR_ID_EXISTS)
    *       should be returned.
    *    latitude, longtitude, radius_meters - The lat, long and radius
    *       (in meters) for the geofence
    *    last_transition - The current state of the geofence. For example, if
    *       the system already knows that the user is inside the geofence,
    *       this will be set to GPS_GEOFENCE_ENTERED. In most cases, it
    *       will be GPS_GEOFENCE_UNCERTAIN.
    *    monitor_transition - Which transitions to monitor. Bitwise OR of
    *       GPS_GEOFENCE_ENTERED, GPS_GEOFENCE_EXITED and
    *       GPS_GEOFENCE_UNCERTAIN.
    *    notification_responsiveness_ms - Defines the best-effort description
    *       of how soon should the callback be called when the transition
    *       associated with the Geofence is triggered. For instance, if set
    *       to 1000 millseconds with GPS_GEOFENCE_ENTERED, the callback
    *       should be called 1000 milliseconds within entering the geofence.
    *       This parameter is defined in milliseconds.
    *       NOTE: This is not to be confused with the rate that the GPS is
    *       polled at. It is acceptable to dynamically vary the rate of
    *       sampling the GPS for power-saving reasons; thus the rate of
    *       sampling may be faster or slower than this.
    *    unknown_timer_ms - The time limit after which the UNCERTAIN transition
    *       should be triggered. This parameter is defined in milliseconds.
    *       See above for a detailed explanation.
    */
    void(*add_geofence_area) (int32_t geofence_id, double latitude, double longitude,
        double radius_meters, int last_transition, int monitor_transitions,
        int notification_responsiveness_ms, int unknown_timer_ms);

    /**
    * Pause monitoring a particular geofence.
    * Parameters:
    *   geofence_id - The id for the geofence.
    */
    void(*pause_geofence) (int32_t geofence_id);

    /**
    * Resume monitoring a particular geofence.
    * Parameters:
    *   geofence_id - The id for the geofence.
    *   monitor_transitions - Which transitions to monitor. Bitwise OR of
    *       GPS_GEOFENCE_ENTERED, GPS_GEOFENCE_EXITED and
    *       GPS_GEOFENCE_UNCERTAIN.
    *       This supersedes the value associated provided in the
    *       add_geofence_area call.
    */
    void(*resume_geofence) (int32_t geofence_id, int monitor_transitions);

    /**
    * Remove a geofence area. After the function returns, no notifications
    * should be sent.
    * Parameter:
    *   geofence_id - The id for the geofence.
    */
    void(*remove_geofence_area) (int32_t geofence_id);

} GpsGeofencingInterface;
    
typedef struct {
    /** set to sizeof(UlpLocation) */
    size_t          size;
    GpsLocation     gpsLocation;
    /* Provider indicator for HYBRID or GPS */
    uint16_t        position_source;
    /*allows HAL to pass additional information related to the location */
    int             rawDataSize;         /* in # of bytes */
    void            * rawData;
    bool            is_indoor;
    float           floor_number;
} UlpLocation;

typedef struct {
    uint64_t gps_sv_used_ids_mask;
    uint64_t glo_sv_used_ids_mask;
    uint64_t gal_sv_used_ids_mask;
    uint64_t bds_sv_used_ids_mask;
    uint64_t qzss_sv_used_ids_mask;
} GnssSvUsedInPosition;

/** Represents gps location extended. */
typedef struct {

    /** set to sizeof(GpsLocationExtended) */
    size_t                       size;
    /** Contains GpsLocationExtendedFlags bits. */
    GpsLocationExtendedFlags     flags;
    /** Contains the Altitude wrt mean sea level */
    float                        altitudeMeanSeaLevel;
    /** Contains Position Dilusion of Precision. */
    float                        pdop;
    /** Contains Horizontal Dilusion of Precision. */
    float                        hdop;
    /** Contains Vertical Dilusion of Precision. */
    float                        vdop;
    /** Contains Magnetic Deviation. */
    float                        magneticDeviation;
    /** vertical uncertainty in meters */
    float                        vert_unc;
    /** speed uncertainty in m/s */
    float                        speed_unc;
    /** heading uncertainty in degrees (0 to 359.999) */
    float                        bearing_unc;
    /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
    float                        horUncEllipseSemiMajor;
    /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
    float                        horUncEllipseSemiMinor;
    /*    Elliptical Horizontal Uncertainty Azimuth */
    float                        horUncEllipseOrientAzimuth;
    /** horizontal reliability. */
    LocReliabilityEnum           horizontal_reliability;
    /** vertical reliability. */
    LocReliabilityEnum           vertical_reliability;
    /** Gnss sv used in position data */
    GnssSvUsedInPosition         gnss_sv_used_ids;
    /** Nav solution mask to indicate sbas corrections */
    LocNavSolutionMask           navSolutionMask;
    /** Position technology used in computing this fix */
    LocPosTechMask               tech_mask;
    /** If DGNSS is used, DGNSS Reference station ID used for position report */
    uint16_t dgnssRefStationId;
    /**  If DGNSS is used, DGNSS data age in milli-seconds  */
    uint32_t dgnssDataAgeMsec;
} GpsLocationExtended;

enum GnssSvOptionsMask {
    GNSS_SV_OPTIONS_HAS_EPHEMER_BIT = (1<<0),
    GNSS_SV_OPTIONS_HAS_ALMANAC_BIT = (1<<1),
    GNSS_SV_OPTIONS_USED_IN_FIX_BIT = (1<<2)
};

/** GNSS Signal Type and RF Band */
typedef uint32_t GnssSignalTypeMask;
typedef enum {
    /** GPS L1CA Signal */
    GNSS_SIGNAL_GPS_L1CA            = (1<<0),
    /** GPS L1C Signal */
    GNSS_SIGNAL_GPS_L1C             = (1<<1),
    /** GPS L2 RF Band */
    GNSS_SIGNAL_GPS_L2              = (1<<2),
    /** GPS L5 RF Band */
    GNSS_SIGNAL_GPS_L5              = (1<<3),
    /** GLONASS G1 (L1OF) RF Band */
    GNSS_SIGNAL_GLONASS_G1          = (1<<4),
    /** GLONASS G2 (L2OF) RF Band */
    GNSS_SIGNAL_GLONASS_G2          = (1<<5),
    /** GALILEO E1 RF Band */
    GNSS_SIGNAL_GALILEO_E1          = (1<<6),
    /** GALILEO E5A RF Band */
    GNSS_SIGNAL_GALILEO_E5A         = (1<<7),
    /** GALILEO E5B RF Band */
    GNSS_SIGNAL_GALILEO_E5B         = (1<<8),
    /** BEIDOU B1_I RF Band */
    GNSS_SIGNAL_BEIDOU_B1I          = (1<<9),
    /** BEIDOU B1C RF Band */
    GNSS_SIGNAL_BEIDOU_B1C          = (1<<10),
    /** BEIDOU B2_I RF Band */
    GNSS_SIGNAL_BEIDOU_B2I          = (1<<11),
    /** BEIDOU B2A_I RF Band */
    GNSS_SIGNAL_BEIDOU_B2AI         = (1<<12),
    /** QZSS L1CA RF Band */
    GNSS_SIGNAL_QZSS_L1CA           = (1<<13),
    /** QZSS L1S RF Band */
    GNSS_SIGNAL_QZSS_L1S            = (1<<14),
    /** QZSS L2 RF Band */
    GNSS_SIGNAL_QZSS_L2             = (1<<15),
    /** QZSS L5 RF Band */
    GNSS_SIGNAL_QZSS_L5             = (1<<16),
    /** SBAS L1 RF Band */
    GNSS_SIGNAL_SBAS_L1             = (1<<17)
} GnssSignalTypeBits;

typedef uint32 svInfoValidMask;
typedef enum {
    GNSS_SV_INFO_MASK_VALID_SYSTEM             	= (1<<0),
    GNSS_SV_INFO_MASK_VALID_GNSS_SVID          	= (1<<1),
    GNSS_SV_INFO_MASK_VALID_HEALTH_STATUS       = (1<<2),
    GNSS_SV_INFO_MASK_VALID_PROCESS_STATUS      = (1<<3),
    GNSS_SV_INFO_MASK_VALID_SVINFO_MASK         = (1<<4),
    GNSS_SV_INFO_MASK_VALID_ELEVATION           = (1<<5),
    GNSS_SV_INFO_MASK_VALID_AZIMUTH             = (1<<6),
    GNSS_SV_INFO_MASK_VALID_SNR                 = (1<<7)
} svInfoValidBits;


typedef struct {
    size_t size;       // set to sizeof(GnssSv)
    svInfoValidMask svInfoMask; //Validity of the following members
    uint16_t svId;     // Unique Identifier
    GnssSvType type;   // type of SV (GPS, SBAS, GLONASS, QZSS, BEIDOU, GALILEO)
    float cN0Dbhz;     // signal strength
    float elevation;   // elevation of SV (in degrees)
    float azimuth;     // azimuth of SV (in degrees)
    GnssSvOptionsMask gnssSvOptionsMask; // Bitwise OR of GnssSvOptionsBits
    float carrierFrequencyHz; // carrier frequency of the signal tracked
    GnssSignalTypeMask gnssSignalTypeMask; // Specifies GNSS signal type
} GnssSv;

typedef struct {
    size_t size;                 // set to sizeof(GnssSvNotification)
    size_t count;                // number of SVs in the GnssSv array
    bool gnssSignalTypeMaskValid;
    GnssSv gnssSvs[GNSS_SV_MAX]; // information on a number of SVs
} GnssSvNotification;

enum loc_registration_mask_status {

    LOC_REGISTRATION_MASK_ENABLED,
    LOC_REGISTRATION_MASK_DISABLED
};

/** Requested recurrence mode for GPS operation. */
typedef uint32_t GpsPositionRecurrence;

typedef enum loc_position_mode_type {
    LOC_POSITION_MODE_INVALID = -1,
    LOC_POSITION_MODE_STANDALONE = 0,
    LOC_POSITION_MODE_MS_BASED,
    LOC_POSITION_MODE_MS_ASSISTED,
    LOC_POSITION_MODE_RESERVED_1,
    LOC_POSITION_MODE_RESERVED_2,
    LOC_POSITION_MODE_RESERVED_3,
    LOC_POSITION_MODE_RESERVED_4,
    LOC_POSITION_MODE_RESERVED_5

} LocPositionMode;

#define MIN_POSSIBLE_FIX_INTERVAL 1000 /* msec */

enum loc_sess_status {
    LOC_SESS_SUCCESS,
    LOC_SESS_INTERMEDIATE,
    LOC_SESS_FAILURE,
    LOC_SESS_TIMEOUT,
    LOC_SESS_DDR_UNAVAILABLE
};

typedef enum {
  LOC_ENG_IF_REQUEST_SENDER_ID_QUIPC = 0,
  LOC_ENG_IF_REQUEST_SENDER_ID_MSAPM,
  LOC_ENG_IF_REQUEST_SENDER_ID_MSAPU,
  LOC_ENG_IF_REQUEST_SENDER_ID_GPSONE_DAEMON,
  LOC_ENG_IF_REQUEST_SENDER_ID_MODEM,
  LOC_ENG_IF_REQUEST_SENDER_ID_UNKNOWN
} loc_if_req_sender_id_e_type;

typedef enum {
    LOC_API_ADAPTER_ERR_SUCCESS             = 0,
    LOC_API_ADAPTER_ERR_GENERAL_FAILURE     = 1,
    LOC_API_ADAPTER_ERR_UNSUPPORTED         = 2,
    LOC_API_ADAPTER_ERR_INVALID_HANDLE      = 4,
    LOC_API_ADAPTER_ERR_INVALID_PARAMETER   = 5,
    LOC_API_ADAPTER_ERR_ENGINE_BUSY         = 6,
    LOC_API_ADAPTER_ERR_PHONE_OFFLINE       = 7,
    LOC_API_ADAPTER_ERR_TIMEOUT             = 8,
    LOC_API_ADAPTER_ERR_SERVICE_NOT_PRESENT = 9,
    LOC_API_ADAPTER_ERR_INTERNAL            = 10,

    /* equating engine down to phone offline, as they are the same errror */
    LOC_API_ADAPTER_ERR_ENGINE_DOWN         = LOC_API_ADAPTER_ERR_PHONE_OFFLINE,
    LOC_API_ADAPTER_ERR_FAILURE             = 101,
    LOC_API_ADAPTER_ERR_UNKNOWN
} loc_api_adapter_err;

enum loc_api_adapter_event_index {
    LOC_API_ADAPTER_REPORT_POSITION = 0,               // Position report comes in loc_parsed_position_s_type
    LOC_API_ADAPTER_REPORT_SATELLITE,                  // Satellite in view report
    LOC_API_ADAPTER_REPORT_NMEA_1HZ,                   // NMEA report at 1HZ rate
    LOC_API_ADAPTER_REPORT_NMEA_POSITION,              // NMEA report at position report rate
    LOC_API_ADAPTER_REQUEST_NI_NOTIFY_VERIFY,          // NI notification/verification request
    LOC_API_ADAPTER_REQUEST_ASSISTANCE_DATA,           // Assistance data, eg: time, predicted orbits request
    LOC_API_ADAPTER_REQUEST_LOCATION_SERVER,           // Request for location server
    LOC_API_ADAPTER_REPORT_IOCTL,                      // Callback report for loc_ioctl
    LOC_API_ADAPTER_REPORT_STATUS,                     // Misc status report: eg, engine state
    LOC_API_ADAPTER_GEOFENCE_GEN_ALERT,                //
    LOC_API_ADAPTER_REPORT_GENFENCE_BREACH,            //
    LOC_API_ADAPTER_BATCH_FULL,                        // Batching on full
    LOC_API_ADAPTER_BATCHED_POSITION_REPORT,           // Batching on fix
    LOC_API_ADAPTER_BATCHED_GENFENCE_BREACH_REPORT,    //
    LOC_API_ADAPTER_GDT_UPLOAD_BEGIN_REQ,              // GDT upload start request
    LOC_API_ADAPTER_GDT_UPLOAD_END_REQ,                // GDT upload end request
    LOC_API_ADAPTER_GNSS_MEASUREMENT,                  // GNSS Measurement report
    LOC_API_ADAPTER_REQUEST_TIMEZONE,                  // Timezone injection request
    LOC_API_ADAPTER_REPORT_GENFENCE_DWELL_REPORT,      // Geofence dwell report
    LOC_API_ADAPTER_REQUEST_GTPWWAN_DATA,             //gtpwwan data  
    LOC_API_ADAPTER_EVENT_MAX
};

typedef unsigned int LOC_API_ADAPTER_EVENT_MASK_T;

typedef enum loc_api_adapter_msg_to_check_supported {
    LOC_API_ADAPTER_MESSAGE_LOCATION_BATCHING,               // Batching 1.0
    LOC_API_ADAPTER_MESSAGE_BATCHED_GENFENCE_BREACH,         // Geofence Batched Breach
    LOC_API_ADAPTER_MESSAGE_DISTANCE_BASE_TRACKING,          // DBT 2.0
    LOC_API_ADAPTER_MESSAGE_ADAPTIVE_LOCATION_BATCHING,      // Batching 1.5
    LOC_API_ADAPTER_MESSAGE_DISTANCE_BASE_LOCATION_BATCHING, // Batching 2.0

    LOC_API_ADAPTER_MESSAGE_MAX
} LocCheckingMessagesID;

typedef int IzatDevId_t;

enum GnssCapability {
    GPS_CAPABILITY_SCHEDULING = (1 << 0),

    /** GPS supports MS-Based AGPS mode */
    GPS_CAPABILITY_MSB = (1 << 1),

    /** GPS supports MS-Assisted AGPS mode */
    GPS_CAPABILITY_MSA = (1 << 2),

    /** GPS supports single-shot fixes */
    GPS_CAPABILITY_SINGLE_SHOT = (1 << 3),

    /** GPS supports on demand time injection */
    GPS_CAPABILITY_ON_DEMAND_TIME = (1 << 4),

    /** GPS supports Geofencing  */
    GPS_CAPABILITY_GEOFENCING = (1 << 5),

    /** GPS supports Measurements. */
    GPS_CAPABILITY_MEASUREMENTS = (1 << 6),

    /** GPS supports Navigation Messages */
    GPS_CAPABILITY_NAV_MESSAGES = (1 << 7),
};

typedef enum {
    LOC_SUPPORTED_FEATURE_ODCPI_2_V02 = 0, /**<  Support ODCPI version 2 feature  */
    LOC_SUPPORTED_FEATURE_WIFI_AP_DATA_INJECT_2_V02, /**<  Support Wifi AP data inject version 2 feature  */
    LOC_SUPPORTED_FEATURE_DEBUG_NMEA_V02, /**< Support debug NMEA feature */
    LOC_SUPPORTED_FEATURE_GNSS_ONLY_POSITION_REPORT, /**< Support GNSS Only position reports */
    LOC_SUPPORTED_FEATURE_FDCL, /**< Support FDCL */
    LOC_SUPPORTED_FEATURE_CONSTELLATION_ENABLEMENT_V02, /**< Support constellation enablement */
    LOC_SUPPORTED_FEATURE_AGPM_V02, /**< Support AGPM feature */
    LOC_SUPPORTED_FEATURE_XTRA_INTEGRITY /**< Support XTRA integrity */
} loc_supported_feature_enum;

typedef enum {
    SINGLESHOT_REQUEST_TYPE_INVALID = 0,
    SINGLESHOT_REQUEST_TYPE_GNSS,
    SINGLESHOT_REQUEST_TYPE_GTPWWAN_1_0,  // Legacy GTP WWAN
    SINGLESHOT_REQUEST_TYPE_GTPWWAN_2_0,  // GTP MCS
    SINGLESHOT_REQUEST_TYPE_WIFI,
} SingleshotRequestType;

typedef enum {
    GEOFENCE_ALERT_INVALID = 0,
    GEOFENCE_ALERT_GNSS_UNAVAILABLE = 1, /**<  GNSS is unavailable and GNSS position fixes
         cannot be used to monitor geofences */
    GEOFENCE_ALERT_GNSS_AVAILABLE = 2, /**<  GNSS is now available and GNSS postion fixes can
         be used to monitor geofences */
    GEOFENCE_ALERT_OOS = 3, /**<  The engine is out of service and no cell ID coverage
         information is available */
    GEOFENCE_ALERT_TIME_INVALID = 4, /**<  The engine has an invalid time  */
    GEOFENCE_ALERT_MGP_UNAVAILABLE = 5, /**<  MGP is unloaded/unavailable  */
} GeofenceAlertType;

#define GPS_L1CA_CARRIER_FREQUENCY      1575420000.0
#define GPS_L1C_CARRIER_FREQUENCY       1575420000.0
#define GPS_L2C_L_CARRIER_FREQUENCY     1227600000.0
#define GPS_L5_Q_CARRIER_FREQUENCY      1176450000.0
#define GLONASS_G1_CARRIER_FREQUENCY    1602000000.0
#define GLONASS_G2_CARRIER_FREQUENCY    1246000000.0
#define GALILEO_E1_C_CARRIER_FREQUENCY  1575420000.0
#define GALILEO_E5A_Q_CARRIER_FREQUENCY 1176450000.0
#define GALILEO_E5B_Q_CARRIER_FREQUENCY 1207140000.0
#define BEIDOU_B1_I_CARRIER_FREQUENCY   1561098000.0
#define BEIDOU_B1C_CARRIER_FREQUENCY    1575420000.0
#define BEIDOU_B2_I_CARRIER_FREQUENCY   1207140000.0
#define BEIDOU_B2A_I_CARRIER_FREQUENCY  1176450000.0
#define BEIDOU_B2A_Q_CARRIER_FREQUENCY  1176450000.0
#define QZSS_L1CA_CARRIER_FREQUENCY     1575420000.0
#define QZSS_L1S_CARRIER_FREQUENCY      1575420000.0
#define QZSS_L2C_L_CARRIER_FREQUENCY    1227600000.0
#define QZSS_L5_Q_CARRIER_FREQUENCY     1176450000.0
#define SBAS_L1_CA_CARRIER_FREQUENCY    1575420000.0 

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GPS_EXTENDED_C_H */

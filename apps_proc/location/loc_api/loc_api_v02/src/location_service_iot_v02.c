/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        L O C A T I O N _ S E R V I C E _ I O T _ V 0 2  . C

GENERAL DESCRIPTION
  This is the file which defines the loc_iot service Data structures.

  Copyright (c) 2010-2020 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/location.tx/3.0/loc_api/loc_api_v02/src/location_service_iot_v02.c#4 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Wed Sep 23 2020 (Spin 0)
   From IDL File: location_service_iot_v02.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "location_service_iot_v02.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t qmiLocPowerModeStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocPowerModeStructT_v02, powerMode),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocPowerModeStructT_v02, timeBetweenMeasurement),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocGPSTimeStructT_data_v02[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocGPSTimeStructT_v02, gpsWeek),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGPSTimeStructT_v02, gpsTimeOfWeekMs),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocDOPStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDOPStructT_v02, PDOP),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDOPStructT_v02, HDOP),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDOPStructT_v02, VDOP),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocExtDOPStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocExtDOPStructT_v02, PDOP),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocExtDOPStructT_v02, HDOP),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocExtDOPStructT_v02, VDOP),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocExtDOPStructT_v02, GDOP),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocExtDOPStructT_v02, TDOP),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocSensorUsageIndicatorStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSensorUsageIndicatorStructT_v02, usageMask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSensorUsageIndicatorStructT_v02, aidingIndicatorMask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocJammerIndicatorStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocJammerIndicatorStructT_v02, agcMetricDb),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocJammerIndicatorStructT_v02, bpMetricDb),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocAssistanceServerUrlStructT_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocAssistanceServerUrlStructT_v02, serverUrl),
  QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocTimeServerListStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocTimeServerListStructT_v02, delayThreshold),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocTimeServerListStructT_v02, timeServerList),
  QMI_LOC_MAX_NTP_SERVERS_V02,
  QMI_IDL_OFFSET8(qmiLocTimeServerListStructT_v02, timeServerList) - QMI_IDL_OFFSET8(qmiLocTimeServerListStructT_v02, timeServerList_len),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocPredictedOrbitsAllowedSizesStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocPredictedOrbitsAllowedSizesStructT_v02, maxFileSizeInBytes),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocPredictedOrbitsAllowedSizesStructT_v02, maxPartSize),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocPredictedOrbitsServerListStructT_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocPredictedOrbitsServerListStructT_v02, serverList),
  QMI_LOC_MAX_PREDICTED_ORBITS_SERVERS_V02,
  QMI_IDL_OFFSET8(qmiLocPredictedOrbitsServerListStructT_v02, serverList) - QMI_IDL_OFFSET8(qmiLocPredictedOrbitsServerListStructT_v02, serverList_len),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocGeofencePositionStructT_data_v02[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, timestampUtc),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, latitude),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, longitude),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, horUncEllipseSemiMinor),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, horUncEllipseSemiMajor),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, horUncEllipseOrientAzimuth),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, speedHorizontal_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, speedHorizontal),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, altitudeWrtEllipsoid_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, altitudeWrtEllipsoid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, vertUnc_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, vertUnc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, speedVertical_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, speedVertical),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, heading_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofencePositionStructT_v02, heading),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocPredictedOrbitsDataValidityStructT_data_v02[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocPredictedOrbitsDataValidityStructT_v02, startTimeInUTC),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocPredictedOrbitsDataValidityStructT_v02, durationHours),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocAltitudeSrcInfoStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAltitudeSrcInfoStructT_v02, source),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAltitudeSrcInfoStructT_v02, linkage),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAltitudeSrcInfoStructT_v02, coverage),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocCircularGeofenceArgsStructT_data_v02[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocCircularGeofenceArgsStructT_v02, latitude),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocCircularGeofenceArgsStructT_v02, longitude),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocCircularGeofenceArgsStructT_v02, radius),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocGeofenceMotionStateConfigStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofenceMotionStateConfigStructT_v02, motionState),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGeofenceMotionStateConfigStructT_v02, motionStateSpeed),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocGSMCellIdStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGSMCellIdStructT_v02, MCC),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGSMCellIdStructT_v02, MNC),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGSMCellIdStructT_v02, LAC),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGSMCellIdStructT_v02, CID),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocBatchedReportStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, fixId),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, validFields),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, latitude),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, longitude),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, horUncCircular),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, speedHorizontal),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, speedUnc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, altitudeWrtEllipsoid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, speedVertical),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, heading),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, headingUnc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, technologyMask),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, timestampUtc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, timeUnc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, magneticDeviation),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, vertUnc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, horConfidence),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocBatchedReportStructT_v02, gpsTime),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocDbtPositionStructT_data_v02[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, timestampUtc),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, latitude),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, longitude),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, horUncEllipseSemiMinor),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, horUncEllipseSemiMajor),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, horUncEllipseOrientAzimuth),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, speedHorizontal_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, speedHorizontal),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, altitudeWrtEllipsoid_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, altitudeWrtEllipsoid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, vertUnc_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, vertUnc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, speedVertical_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, speedVertical),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, heading_valid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDbtPositionStructT_v02, heading),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocDeleteSatelliteDataStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteSatelliteDataStructT_v02, system),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteSatelliteDataStructT_v02, deleteSatelliteDataMask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocXtraInfoStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocXtraInfoStructT_v02, reportMask),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocXtraInfoStructT_v02, absAgeHrs),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocXtraInfoStructT_v02, relAgeInUTC),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocXtraInfoStructT_v02, xtraServerInfo),
  QMI_IDL_TYPE88(0, 9),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocXtraInfoStructT_v02, ntpServerInfo),
  QMI_IDL_TYPE88(0, 7),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocXtraInfoStructT_v02, timeRequest),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocXtraInfoStructT_v02, preferedValidAgeHrs),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocGtpClientSwVerStructT_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGtpClientSwVerStructT_v02, gtpClientSwMajorVersion),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGtpClientSwVerStructT_v02, gtpClientSwMinorVersion),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocHomeCarrierIdStructT_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocHomeCarrierIdStructT_v02, homeMncValid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocHomeCarrierIdStructT_v02, homeMcc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocHomeCarrierIdStructT_v02, homeMnc),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocPositionSourceStructT_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocPositionSourceStructT_v02, unspecifiedInternal),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocPositionSourceStructT_v02, unspecifiedExternal),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocPositionSourceStructT_v02, gnss),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocPositionSourceStructT_v02, sensors),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocPositionSourceStructT_v02, wifi),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocPositionSourceStructT_v02, wwan),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocBsBestPositionStructT_data_v02[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocBsBestPositionStructT_v02, latitude),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocBsBestPositionStructT_v02, longitude),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBsBestPositionStructT_v02, horizontalUnc),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBsBestPositionStructT_v02, altitude),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBsBestPositionStructT_v02, altitudeUnc),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocBsBestPositionStructT_v02, latestPositionSources),
  QMI_IDL_TYPE88(0, 22),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocFdclCellIdStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdStructT_v02, airInterfaceType),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdStructT_v02, id1),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdStructT_v02, id2),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdStructT_v02, id3),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdStructT_v02, id4),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocFdclCellPosStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellPosStructT_v02, validMask),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocFdclCellPosStructT_v02, cellId),
  QMI_IDL_TYPE88(0, 24),
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellPosStructT_v02, latitude),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellPosStructT_v02, longitude),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellPosStructT_v02, horCoverageRadius),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellPosStructT_v02, horConfidence),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellPosStructT_v02, horReliability),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellPosStructT_v02, altitude),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellPosStructT_v02, altUnc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellPosStructT_v02, altConfidence),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellPosStructT_v02, altReliability),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocNextLeapSecondInfoStructT_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNextLeapSecondInfoStructT_v02, gpsTimeCurrent_valid),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocNextLeapSecondInfoStructT_v02, gpsTimeCurrent),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNextLeapSecondInfoStructT_v02, leapSecondsCurrent_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNextLeapSecondInfoStructT_v02, leapSecondsCurrent),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNextLeapSecondInfoStructT_v02, gpsTimeNextLsEvent_valid),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocNextLeapSecondInfoStructT_v02, gpsTimeNextLsEvent),
  QMI_IDL_TYPE88(0, 1),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNextLeapSecondInfoStructT_v02, leapSecondsNext_valid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNextLeapSecondInfoStructT_v02, leapSecondsNext),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocBsObservedDataStructT_data_v02[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocBsObservedDataStructT_v02, validMask),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocBsObservedDataStructT_v02, cellId),
  QMI_IDL_TYPE88(0, 24),
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocBsObservedDataStructT_v02, gpsWeek),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBsObservedDataStructT_v02, gpsMilliseconds),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBsObservedDataStructT_v02, bsLocalTimestamp),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocBsObservedDataStructT_v02, posDeltaTime),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocBsObservedDataStructT_v02, bsBestPosition),
  QMI_IDL_TYPE88(0, 23),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocSvInfoStructT_data_v02[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, validMask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, system),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, gnssSvId),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, healthStatus),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, svStatus),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, svInfoMask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, elevation),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, azimuth),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, snr),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, gnssSignalType),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocFdclCellIdExtStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdExtStructT_v02, airInterfaceType),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdExtStructT_v02, id1),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdExtStructT_v02, id2),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdExtStructT_v02, id3),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdExtStructT_v02, id4),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdExtStructT_v02, validMask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdExtStructT_v02, channelNumber),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdExtStructT_v02, plid),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdExtStructT_v02, nbCarrierFreqOff),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclCellIdExtStructT_v02, nbChannelRasterOff),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLTEBandStructT_data_v02[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLTEBandStructT_v02, bandMask),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLTEBandStructT_v02, rfBandsLTE),
  QMI_LOC_RF_BAND_LENGTH_V02,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocFdclPosStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclPosStructT_v02, validMask),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclPosStructT_v02, latitude),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclPosStructT_v02, longitude),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclPosStructT_v02, horCoverageRadius),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclPosStructT_v02, horConfidence),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclPosStructT_v02, horReliability),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclPosStructT_v02, altitude),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclPosStructT_v02, altUnc),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclPosStructT_v02, altConfidence),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocFdclPosStructT_v02, altReliability),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * qmiLocGenReqMsgT is empty
 * static const uint8_t qmiLocGenReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGenRespMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGenRespMsgT_v02, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qmiLocInformClientRevisionReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInformClientRevisionReqMsgT_v02, revision)
};

static const uint8_t qmiLocRegEventsReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocRegEventsReqMsgT_v02, eventRegMask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocRegEventsReqMsgT_v02, clientStrId) - QMI_IDL_OFFSET8(qmiLocRegEventsReqMsgT_v02, clientStrId_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocRegEventsReqMsgT_v02, clientStrId),
  QMI_LOC_MAX_CLIENT_ID_STRING_LENGTH_V02
};

static const uint8_t qmiLocStartReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, sessionId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, fixRecurrence) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, fixRecurrence_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, fixRecurrence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, horizontalAccuracyLevel) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, horizontalAccuracyLevel_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, horizontalAccuracyLevel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, intermediateReportState) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, intermediateReportState_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, intermediateReportState),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, minInterval) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, minInterval_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, minInterval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, configAltitudeAssumed) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, configAltitudeAssumed_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, configAltitudeAssumed),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, minIntermediatePositionReportInterval) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, minIntermediatePositionReportInterval_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, minIntermediatePositionReportInterval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, positionReportTimeout) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, positionReportTimeout_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, positionReportTimeout),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, sharePosition) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, sharePosition_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, sharePosition),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, reportGnssOnlyPosition) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, reportGnssOnlyPosition_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, reportGnssOnlyPosition),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, powerMode) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, powerMode_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, powerMode),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, horizontalAccuracy) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, horizontalAccuracy_valid)),
  0x1A,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, horizontalAccuracy)
};

static const uint8_t qmiLocStopReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStopReqMsgT_v02, sessionId)
};

static const uint8_t qmiLocEventPositionReportIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, sessionStatus),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, sessionId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, latitude) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, latitude_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, latitude),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, longitude) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, longitude_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, longitude),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncCircular) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncCircular_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncCircular),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseSemiMinor) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseSemiMinor_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseSemiMinor),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseSemiMajor) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseSemiMajor_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseSemiMajor),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseOrientAzimuth) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseOrientAzimuth_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseOrientAzimuth),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horConfidence) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horConfidence_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horReliability) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horReliability_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horReliability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedHorizontal) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedHorizontal_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedHorizontal),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedUnc) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedUnc_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, altitudeWrtEllipsoid) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, altitudeWrtEllipsoid_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, altitudeWrtEllipsoid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, altitudeWrtMeanSeaLevel) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, altitudeWrtMeanSeaLevel_valid)),
  0x1B,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, altitudeWrtMeanSeaLevel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertUnc) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertUnc_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertConfidence) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertConfidence_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertReliability) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertReliability_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertReliability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedVertical) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedVertical_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedVertical),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, heading) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, heading_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, heading),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, headingUnc) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, headingUnc_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, headingUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, magneticDeviation) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, magneticDeviation_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, magneticDeviation),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, technologyMask) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, technologyMask_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, technologyMask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, DOP) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, DOP_valid)),
  0x24,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, DOP),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, timestampUtc) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, timestampUtc_valid)),
  0x25,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, timestampUtc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, leapSeconds) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, leapSeconds_valid)),
  0x26,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, leapSeconds),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, gpsTime) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, gpsTime_valid)),
  0x27,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, gpsTime),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, timeUnc) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, timeUnc_valid)),
  0x28,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, timeUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, timeSrc) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, timeSrc_valid)),
  0x29,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, timeSrc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, sensorDataUsage) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, sensorDataUsage_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, sensorDataUsage),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, fixId) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, fixId_valid)),
  0x2B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, fixId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, gnssSvUsedList) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, gnssSvUsedList_valid)),
  0x2C,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, gnssSvUsedList),
  QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02,
  QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, gnssSvUsedList) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, gnssSvUsedList_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, altitudeAssumed) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, altitudeAssumed_valid)),
  0x2D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, altitudeAssumed),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, velEnu) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, velEnu_valid)),
  0x2E,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, velEnu),
  QMI_LOC_ENU_ARRAY_LENGTH_V02,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, velUncEnu) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, velUncEnu_valid)),
  0x2F,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, velUncEnu),
  QMI_LOC_ENU_ARRAY_LENGTH_V02,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, navSolutionMask) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, navSolutionMask_valid)),
  0x30,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, navSolutionMask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, sensorSubTechnologyMask) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, sensorSubTechnologyMask_valid)),
  0x31,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, sensorSubTechnologyMask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, gnssOnlyPosition) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, gnssOnlyPosition_valid)),
  0x32,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, gnssOnlyPosition),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, extDOP) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, extDOP_valid)),
  0x33,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, extDOP),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, dgnssStationId) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, dgnssStationId_valid)),
  0x34,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, dgnssStationId),
  QMI_LOC_DGNSS_STATION_ID_ARRAY_LENGTH_V02,
  QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, dgnssStationId) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, dgnssStationId_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, spoofReportMask) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, spoofReportMask_valid)),
  0x35,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, spoofReportMask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, expandedGnssSvUsedList) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, expandedGnssSvUsedList_valid)),
  0x36,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, expandedGnssSvUsedList),
  QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02,
  QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, expandedGnssSvUsedList) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, expandedGnssSvUsedList_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, gnssSvUsedSignalTypeList) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, gnssSvUsedSignalTypeList_valid)),
  0x37,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, gnssSvUsedSignalTypeList),
  QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02,
  QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, gnssSvUsedSignalTypeList) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, gnssSvUsedSignalTypeList_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, jammerIndicatorList) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, jammerIndicatorList_valid)),
  0x38,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, jammerIndicatorList),
  QMI_LOC_MAX_GNSS_SIGNAL_TYPE_V02,
  QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, jammerIndicatorList) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, jammerIndicatorList_len),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t qmiLocEventInjectTimeReqIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventInjectTimeReqIndMsgT_v02, timeServerInfo) - QMI_IDL_OFFSET8(qmiLocEventInjectTimeReqIndMsgT_v02, timeServerInfo_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventInjectTimeReqIndMsgT_v02, timeServerInfo),
  QMI_IDL_TYPE88(0, 7)
};

static const uint8_t qmiLocEventInjectPredictedOrbitsReqIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02, allowedSizes),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02, serverList) - QMI_IDL_OFFSET8(qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02, serverList_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02, serverList),
  QMI_IDL_TYPE88(0, 9)
};

static const uint8_t qmiLocEventEngineStateIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventEngineStateIndMsgT_v02, engineState)
};

static const uint8_t qmiLocEventFixSessionStateIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventFixSessionStateIndMsgT_v02, sessionState),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventFixSessionStateIndMsgT_v02, sessionId) - QMI_IDL_OFFSET8(qmiLocEventFixSessionStateIndMsgT_v02, sessionId_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventFixSessionStateIndMsgT_v02, sessionId)
};

static const uint8_t qmiLocEventGeofenceGenAlertIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventGeofenceGenAlertIndMsgT_v02, geofenceAlert)
};

static const uint8_t qmiLocEventGeofenceBreachIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventGeofenceBreachIndMsgT_v02, geofenceId),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventGeofenceBreachIndMsgT_v02, breachType),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventGeofenceBreachIndMsgT_v02, geofencePosition) - QMI_IDL_OFFSET8(qmiLocEventGeofenceBreachIndMsgT_v02, geofencePosition_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventGeofenceBreachIndMsgT_v02, geofencePosition),
  QMI_IDL_TYPE88(0, 10),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventGeofenceBreachIndMsgT_v02, breachConfidence) - QMI_IDL_OFFSET8(qmiLocEventGeofenceBreachIndMsgT_v02, breachConfidence_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventGeofenceBreachIndMsgT_v02, breachConfidence)
};

/*
 * qmiLocGetServiceRevisionReqMsgT is empty
 * static const uint8_t qmiLocGetServiceRevisionReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetServiceRevisionIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetServiceRevisionIndMsgT_v02, status),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetServiceRevisionIndMsgT_v02, revision),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetServiceRevisionIndMsgT_v02, gnssMeFWVerString) - QMI_IDL_OFFSET8(qmiLocGetServiceRevisionIndMsgT_v02, gnssMeFWVerString_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocGetServiceRevisionIndMsgT_v02, gnssMeFWVerString),
  QMI_LOC_GNSS_ME_VERSION_STRING_MAX_LENGTH_V02,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetServiceRevisionIndMsgT_v02, gnssHostSWVerString) - QMI_IDL_OFFSET8(qmiLocGetServiceRevisionIndMsgT_v02, gnssHostSWVerString_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocGetServiceRevisionIndMsgT_v02, gnssHostSWVerString),
  QMI_LOC_GNSS_HOSTED_SW_VERSION_STRING_MAX_LENGTH_V02,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetServiceRevisionIndMsgT_v02, gnssSWVerString) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetServiceRevisionIndMsgT_v02, gnssSWVerString_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetServiceRevisionIndMsgT_v02, gnssSWVerString),
  QMI_LOC_GNSS_SW_VERSION_STRING_MAX_LENGTH_V02
};

/*
 * qmiLocGetFixCriteriaReqMsgT is empty
 * static const uint8_t qmiLocGetFixCriteriaReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetFixCriteriaIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, horizontalAccuracyLevel) - QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, horizontalAccuracyLevel_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, horizontalAccuracyLevel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, intermediateReportState) - QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, intermediateReportState_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, intermediateReportState),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, minInterval) - QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, minInterval_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, minInterval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, configAltitudeAssumed) - QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, configAltitudeAssumed_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, configAltitudeAssumed),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, minIntermediatePositionReportInterval) - QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, minIntermediatePositionReportInterval_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, minIntermediatePositionReportInterval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, positionReportTimeout) - QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, positionReportTimeout_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, positionReportTimeout),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, sharePosition) - QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, sharePosition_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, sharePosition),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, powerMode) - QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, powerMode_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, powerMode),
  QMI_IDL_TYPE88(0, 0)
};

/*
 * qmiLocGetPredictedOrbitsDataSourceReqMsgT is empty
 * static const uint8_t qmiLocGetPredictedOrbitsDataSourceReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetPredictedOrbitsDataSourceIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, allowedSizes) - QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, allowedSizes_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, allowedSizes),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, serverList) - QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, serverList_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, serverList),
  QMI_IDL_TYPE88(0, 9)
};

/*
 * qmiLocGetPredictedOrbitsDataValidityReqMsgT is empty
 * static const uint8_t qmiLocGetPredictedOrbitsDataValidityReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetPredictedOrbitsDataValidityIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02, validityInfo) - QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02, validityInfo_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02, validityInfo),
  QMI_IDL_TYPE88(0, 11)
};

static const uint8_t qmiLocInjectUtcTimeReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectUtcTimeReqMsgT_v02, timeUtc),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectUtcTimeReqMsgT_v02, timeUnc)
};

static const uint8_t qmiLocInjectUtcTimeIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectUtcTimeIndMsgT_v02, status)
};

static const uint8_t qmiLocInjectPositionReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, latitude) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, latitude_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, latitude),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, longitude) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, longitude_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, longitude),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horUncCircular) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horUncCircular_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horUncCircular),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horConfidence) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horConfidence_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horReliability) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horReliability_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horReliability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altitudeWrtEllipsoid) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altitudeWrtEllipsoid_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altitudeWrtEllipsoid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altitudeWrtMeanSeaLevel) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altitudeWrtMeanSeaLevel_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altitudeWrtMeanSeaLevel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertUnc) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertUnc_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertConfidence) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertConfidence_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertReliability) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertReliability_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertReliability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altSourceInfo) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altSourceInfo_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altSourceInfo),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timestampUtc) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timestampUtc_valid)),
  0x1B,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timestampUtc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timestampAge) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timestampAge_valid)),
  0x1C,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timestampAge),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, positionSrc) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, positionSrc_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, positionSrc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, rawHorUncCircular) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, rawHorUncCircular_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, rawHorUncCircular),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, rawHorConfidence) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, rawHorConfidence_valid)),
  0x1F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, rawHorConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, onDemandCpi) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, onDemandCpi_valid)),
  0x20,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, onDemandCpi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, positionSrcProvider) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, positionSrcProvider_valid)),
  0x21,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, positionSrcProvider),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, gpsTime) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, gpsTime_valid)),
  0x22,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, gpsTime),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timeUnc) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timeUnc_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timeUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, velEnu) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, velEnu_valid)),
  0x24,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, velEnu),
  QMI_LOC_ENU_ARRAY_LENGTH_V02,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, velUncEnu) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, velUncEnu_valid)),
  0x25,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, velUncEnu),
  QMI_LOC_ENU_ARRAY_LENGTH_V02,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, expandedGnssSvUsedList) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, expandedGnssSvUsedList_valid)),
  0x26,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, expandedGnssSvUsedList),
  QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, expandedGnssSvUsedList) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, expandedGnssSvUsedList_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectPositionReqMsgT_v02, numSvInFix) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectPositionReqMsgT_v02, numSvInFix_valid)),
  0x27,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectPositionReqMsgT_v02, numSvInFix)
};

static const uint8_t qmiLocInjectPositionIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionIndMsgT_v02, status)
};

static const uint8_t qmiLocSetXtraTSessionControlReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetXtraTSessionControlReqMsgT_v02, xtraTSessionControl)
};

static const uint8_t qmiLocSetXtraTSessionControlIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetXtraTSessionControlIndMsgT_v02, status)
};

/*
 * qmiLocGetXtraTSessionControlReqMsgT is empty
 * static const uint8_t qmiLocGetXtraTSessionControlReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetXtraTSessionControlIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetXtraTSessionControlIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetXtraTSessionControlIndMsgT_v02, xtraTSessionControl) - QMI_IDL_OFFSET8(qmiLocGetXtraTSessionControlIndMsgT_v02, xtraTSessionControl_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetXtraTSessionControlIndMsgT_v02, xtraTSessionControl)
};

/*
 * qmiLocGetRegisteredEventsReqMsgT is empty
 * static const uint8_t qmiLocGetRegisteredEventsReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetRegisteredEventsIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetRegisteredEventsIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetRegisteredEventsIndMsgT_v02, eventRegMask) - QMI_IDL_OFFSET8(qmiLocGetRegisteredEventsIndMsgT_v02, eventRegMask_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetRegisteredEventsIndMsgT_v02, eventRegMask)
};

static const uint8_t qmiLocSetPositionEngineConfigParametersReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, injectedPositionControl) - QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, injectedPositionControl_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, injectedPositionControl),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, filterSvUsage) - QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, filterSvUsage_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, filterSvUsage),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, storeAssistData) - QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, storeAssistData_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, storeAssistData),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, enableFasterTTFF) - QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, enableFasterTTFF_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, enableFasterTTFF)
};

static const uint8_t qmiLocSetPositionEngineConfigParametersIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersIndMsgT_v02, failedPositionEngineConfigParamMask) - QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersIndMsgT_v02, failedPositionEngineConfigParamMask_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersIndMsgT_v02, failedPositionEngineConfigParamMask)
};

static const uint8_t qmiLocGetPositionEngineConfigParametersReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersReqMsgT_v02, getPositionEngineConfigParamMask)
};

static const uint8_t qmiLocGetPositionEngineConfigParametersIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, injectedPositionControl) - QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, injectedPositionControl_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, injectedPositionControl),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, filterSvUsage) - QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, filterSvUsage_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, filterSvUsage),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, storeAssistData) - QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, storeAssistData_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, storeAssistData),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, enableFasterTTFF) - QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, enableFasterTTFF_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, enableFasterTTFF)
};

static const uint8_t qmiLocAddCircularGeofenceReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, transactionId),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, circularGeofenceArgs),
  QMI_IDL_TYPE88(0, 13),

  0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, breachMask),

  0x04,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, includePosition),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, responsiveness) - QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, responsiveness_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, responsiveness),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, confidence) - QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, confidence_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, confidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, customResponsivenessValue) - QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, customResponsivenessValue_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, customResponsivenessValue),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, dwellTime) - QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, dwellTime_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, dwellTime),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, dwellTypeMask) - QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, dwellTypeMask_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceReqMsgT_v02, dwellTypeMask)
};

static const uint8_t qmiLocAddCircularGeofenceIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceIndMsgT_v02, transactionId) - QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceIndMsgT_v02, transactionId_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceIndMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceIndMsgT_v02, geofenceId) - QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceIndMsgT_v02, geofenceId_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAddCircularGeofenceIndMsgT_v02, geofenceId)
};

static const uint8_t qmiLocDeleteGeofenceReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteGeofenceReqMsgT_v02, geofenceId),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteGeofenceReqMsgT_v02, transactionId)
};

static const uint8_t qmiLocDeleteGeofenceIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteGeofenceIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocDeleteGeofenceIndMsgT_v02, geofenceId) - QMI_IDL_OFFSET8(qmiLocDeleteGeofenceIndMsgT_v02, geofenceId_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteGeofenceIndMsgT_v02, geofenceId),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocDeleteGeofenceIndMsgT_v02, transactionId) - QMI_IDL_OFFSET8(qmiLocDeleteGeofenceIndMsgT_v02, transactionId_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteGeofenceIndMsgT_v02, transactionId)
};

static const uint8_t qmiLocSetGeofenceEngineConfigReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, gnssUnavailableIndicationTimeout) - QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, gnssUnavailableIndicationTimeout_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, gnssUnavailableIndicationTimeout),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, maxGeofences) - QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, maxGeofences_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, maxGeofences),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, enableMotionDetectionSources) - QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, enableMotionDetectionSources_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, enableMotionDetectionSources),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, enableCpiUsage) - QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, enableCpiUsage_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, enableCpiUsage),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, gnssPositionSessionTimeout) - QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, gnssPositionSessionTimeout_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, gnssPositionSessionTimeout),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, gnssPositionMaxPuncAcceptable) - QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, gnssPositionMaxPuncAcceptable_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, gnssPositionMaxPuncAcceptable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, mediumResponsivenessValue) - QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, mediumResponsivenessValue_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, mediumResponsivenessValue),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, chalGnssEnvMinCpiWaitInterval) - QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, chalGnssEnvMinCpiWaitInterval_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, chalGnssEnvMinCpiWaitInterval),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, motionStateInfo) - QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, motionStateInfo_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, motionStateInfo),
  QMI_LOC_GEOFENCE_MAX_MOTION_STATES_V02,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, motionStateInfo) - QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigReqMsgT_v02, motionStateInfo_len),
  QMI_IDL_TYPE88(0, 14)
};

static const uint8_t qmiLocSetGeofenceEngineConfigIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigIndMsgT_v02, transactionId) - QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigIndMsgT_v02, transactionId_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGeofenceEngineConfigIndMsgT_v02, transactionId)
};

static const uint8_t qmiLocGetGeofenceEngineConfigReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigReqMsgT_v02, transactionId)
};

static const uint8_t qmiLocGetGeofenceEngineConfigIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, transactionId) - QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, transactionId_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, gnssUnavailableIndicationTimeout) - QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, gnssUnavailableIndicationTimeout_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, gnssUnavailableIndicationTimeout),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, maxGeofences) - QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, maxGeofences_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, maxGeofences),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, enabledMotionDetectionSources) - QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, enabledMotionDetectionSources_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, enabledMotionDetectionSources),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, enabledCpiUsage) - QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, enabledCpiUsage_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetGeofenceEngineConfigIndMsgT_v02, enabledCpiUsage)
};

static const uint8_t qmiLocGetBestAvailablePositionReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionReqMsgT_v02, transactionId)
};

static const uint8_t qmiLocGetBestAvailablePositionIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, transactionId) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, transactionId_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, latitude) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, latitude_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, latitude),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, longitude) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, longitude_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, longitude),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horUncCircular) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horUncCircular_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horUncCircular),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, altitudeWrtEllipsoid) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, altitudeWrtEllipsoid_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, altitudeWrtEllipsoid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertUnc) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertUnc_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, timestampUtc) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, timestampUtc_valid)),
  0x16,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, timestampUtc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, timeUnc) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, timeUnc_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, timeUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horUncEllipseSemiMinor) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horUncEllipseSemiMinor_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horUncEllipseSemiMinor),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horUncEllipseSemiMajor) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horUncEllipseSemiMajor_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horUncEllipseSemiMajor),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horUncEllipseOrientAzimuth) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horUncEllipseOrientAzimuth_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horUncEllipseOrientAzimuth),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horCircularConfidence) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horCircularConfidence_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horCircularConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horEllipticalConfidence) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horEllipticalConfidence_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horEllipticalConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horReliability) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horReliability_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horReliability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horSpeed) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horSpeed_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horSpeed),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horSpeedUnc) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horSpeedUnc_valid)),
  0x1F,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, horSpeedUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, altitudeWrtMeanSeaLevel) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, altitudeWrtMeanSeaLevel_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, altitudeWrtMeanSeaLevel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertConfidence) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertConfidence_valid)),
  0x21,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertReliability) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertReliability_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertReliability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertSpeed) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertSpeed_valid)),
  0x23,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertSpeed),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertSpeedUnc) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertSpeedUnc_valid)),
  0x24,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, vertSpeedUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, heading) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, heading_valid)),
  0x25,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, heading),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, headingUnc) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, headingUnc_valid)),
  0x26,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, headingUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, magneticDeviation) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, magneticDeviation_valid)),
  0x27,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, magneticDeviation),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, technologyMask) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, technologyMask_valid)),
  0x28,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, technologyMask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, DOP) - QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, DOP_valid)),
  0x29,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetBestAvailablePositionIndMsgT_v02, DOP),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, gpsTime) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, gpsTime_valid)),
  0x2A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetBestAvailablePositionIndMsgT_v02, gpsTime),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, timeSrc) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, timeSrc_valid)),
  0x2B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetBestAvailablePositionIndMsgT_v02, timeSrc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, sensorDataUsage) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, sensorDataUsage_valid)),
  0x2C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetBestAvailablePositionIndMsgT_v02, sensorDataUsage),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, gnssSvUsedList) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, gnssSvUsedList_valid)),
  0x2D,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetBestAvailablePositionIndMsgT_v02, gnssSvUsedList),
  QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02,
  QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, gnssSvUsedList) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, gnssSvUsedList_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, extDOP) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, extDOP_valid)),
  0x2E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetBestAvailablePositionIndMsgT_v02, extDOP),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, spoofReportMask) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, spoofReportMask_valid)),
  0x2F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetBestAvailablePositionIndMsgT_v02, spoofReportMask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, expandedGnssSvUsedList) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, expandedGnssSvUsedList_valid)),
  0x30,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetBestAvailablePositionIndMsgT_v02, expandedGnssSvUsedList),
  QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02,
  QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, expandedGnssSvUsedList) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, expandedGnssSvUsedList_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, gnssSvUsedSignalTypeList) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, gnssSvUsedSignalTypeList_valid)),
  0x31,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetBestAvailablePositionIndMsgT_v02, gnssSvUsedSignalTypeList),
  QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02,
  QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, gnssSvUsedSignalTypeList) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetBestAvailablePositionIndMsgT_v02, gnssSvUsedSignalTypeList_len)
};

static const uint8_t qmiLocInjectGSMCellInfoReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocInjectGSMCellInfoReqMsgT_v02, gsmCellId),
  QMI_IDL_TYPE88(0, 15),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectGSMCellInfoReqMsgT_v02, roamingStatus),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectGSMCellInfoReqMsgT_v02, timingAdvance) - QMI_IDL_OFFSET8(qmiLocInjectGSMCellInfoReqMsgT_v02, timingAdvance_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectGSMCellInfoReqMsgT_v02, timingAdvance)
};

static const uint8_t qmiLocInjectGSMCellInfoIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectGSMCellInfoIndMsgT_v02, status)
};

static const uint8_t qmiLocGetBatchSizeReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBatchSizeReqMsgT_v02, transactionId),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBatchSizeReqMsgT_v02, batchSize),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBatchSizeReqMsgT_v02, batchType) - QMI_IDL_OFFSET8(qmiLocGetBatchSizeReqMsgT_v02, batchType_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBatchSizeReqMsgT_v02, batchType)
};

static const uint8_t qmiLocGetBatchSizeIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBatchSizeIndMsgT_v02, status),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBatchSizeIndMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBatchSizeIndMsgT_v02, batchSize)
};

static const uint8_t qmiLocStartBatchingReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, minInterval) - QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, minInterval_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, minInterval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, horizontalAccuracyLevel) - QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, horizontalAccuracyLevel_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, horizontalAccuracyLevel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, fixSessionTimeout) - QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, fixSessionTimeout_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, fixSessionTimeout),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, minDistance) - QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, minDistance_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, minDistance),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, batchAllPos) - QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, batchAllPos_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, batchAllPos),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, requestId) - QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, requestId_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartBatchingReqMsgT_v02, requestId)
};

static const uint8_t qmiLocStartBatchingIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartBatchingIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartBatchingIndMsgT_v02, requestId) - QMI_IDL_OFFSET8(qmiLocStartBatchingIndMsgT_v02, requestId_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartBatchingIndMsgT_v02, requestId)
};

static const uint8_t qmiLocEventBatchFullIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventBatchFullIndMsgT_v02, batchCount),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventBatchFullIndMsgT_v02, batchType) - QMI_IDL_OFFSET8(qmiLocEventBatchFullIndMsgT_v02, batchType_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventBatchFullIndMsgT_v02, batchType),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventBatchFullIndMsgT_v02, accumulatedDistance) - QMI_IDL_OFFSET8(qmiLocEventBatchFullIndMsgT_v02, accumulatedDistance_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventBatchFullIndMsgT_v02, accumulatedDistance),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventBatchFullIndMsgT_v02, batchedPosition) - QMI_IDL_OFFSET8(qmiLocEventBatchFullIndMsgT_v02, batchedPosition_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventBatchFullIndMsgT_v02, batchedPosition)
};

static const uint8_t qmiLocReadFromBatchReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocReadFromBatchReqMsgT_v02, numberOfEntries),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocReadFromBatchReqMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocReadFromBatchReqMsgT_v02, batchType) - QMI_IDL_OFFSET8(qmiLocReadFromBatchReqMsgT_v02, batchType_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocReadFromBatchReqMsgT_v02, batchType)
};

static const uint8_t qmiLocReadFromBatchIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocReadFromBatchIndMsgT_v02, status),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocReadFromBatchIndMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocReadFromBatchIndMsgT_v02, numberOfEntries) - QMI_IDL_OFFSET8(qmiLocReadFromBatchIndMsgT_v02, numberOfEntries_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocReadFromBatchIndMsgT_v02, numberOfEntries),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocReadFromBatchIndMsgT_v02, batchedReportList) - QMI_IDL_OFFSET8(qmiLocReadFromBatchIndMsgT_v02, batchedReportList_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocReadFromBatchIndMsgT_v02, batchedReportList),
  QMI_LOC_READ_FROM_BATCH_MAX_SIZE_V02,
  QMI_IDL_OFFSET8(qmiLocReadFromBatchIndMsgT_v02, batchedReportList) - QMI_IDL_OFFSET8(qmiLocReadFromBatchIndMsgT_v02, batchedReportList_len),
  QMI_IDL_TYPE88(0, 16)
};

static const uint8_t qmiLocStopBatchingReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStopBatchingReqMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStopBatchingReqMsgT_v02, requestId) - QMI_IDL_OFFSET8(qmiLocStopBatchingReqMsgT_v02, requestId_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStopBatchingReqMsgT_v02, requestId),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStopBatchingReqMsgT_v02, batchType) - QMI_IDL_OFFSET8(qmiLocStopBatchingReqMsgT_v02, batchType_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStopBatchingReqMsgT_v02, batchType)
};

static const uint8_t qmiLocStopBatchingIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStopBatchingIndMsgT_v02, status),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStopBatchingIndMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStopBatchingIndMsgT_v02, requestId) - QMI_IDL_OFFSET8(qmiLocStopBatchingIndMsgT_v02, requestId_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStopBatchingIndMsgT_v02, requestId)
};

static const uint8_t qmiLocReleaseBatchReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocReleaseBatchReqMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocReleaseBatchReqMsgT_v02, batchType) - QMI_IDL_OFFSET8(qmiLocReleaseBatchReqMsgT_v02, batchType_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocReleaseBatchReqMsgT_v02, batchType)
};

static const uint8_t qmiLocReleaseBatchIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocReleaseBatchIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocReleaseBatchIndMsgT_v02, transactionId)
};

static const uint8_t qmiLocGetAvailWwanPositionReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionReqMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionReqMsgT_v02, gtpLocMask) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionReqMsgT_v02, gtpLocMask_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionReqMsgT_v02, gtpLocMask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionReqMsgT_v02, timeOut) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionReqMsgT_v02, timeOut_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionReqMsgT_v02, timeOut)
};

static const uint8_t qmiLocGetAvailWwanPositionIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, transactionId) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, transactionId_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, latitude) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, latitude_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, latitude),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, longitude) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, longitude_valid)),
  0x12,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, longitude),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horUncCircular) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horUncCircular_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horUncCircular),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, altitudeWrtEllipsoid) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, altitudeWrtEllipsoid_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, altitudeWrtEllipsoid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, vertUnc) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, vertUnc_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, vertUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, timestampUtc) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, timestampUtc_valid)),
  0x16,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, timestampUtc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, timeUnc) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, timeUnc_valid)),
  0x17,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, timeUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horUncEllipseSemiMinor) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horUncEllipseSemiMinor_valid)),
  0x18,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horUncEllipseSemiMinor),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horUncEllipseSemiMajor) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horUncEllipseSemiMajor_valid)),
  0x19,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horUncEllipseSemiMajor),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horUncEllipseOrientAzimuth) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horUncEllipseOrientAzimuth_valid)),
  0x1A,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horUncEllipseOrientAzimuth),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horCircularConfidence) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horCircularConfidence_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horCircularConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horEllipticalConfidence) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horEllipticalConfidence_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horEllipticalConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horReliability) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horReliability_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, horReliability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, altitudeWrtMeanSeaLevel) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, altitudeWrtMeanSeaLevel_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, altitudeWrtMeanSeaLevel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, vertConfidence) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, vertConfidence_valid)),
  0x1F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, vertConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, vertReliability) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, vertReliability_valid)),
  0x20,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, vertReliability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, gpsTime) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, gpsTime_valid)),
  0x21,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, gpsTime),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, timeSrc) - QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, timeSrc_valid)),
  0x22,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetAvailWwanPositionIndMsgT_v02, timeSrc)
};

static const uint8_t qmiLocSetPremiumServicesCfgReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPremiumServicesCfgReqMsgT_v02, premiumServiceType),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPremiumServicesCfgReqMsgT_v02, premiumServiceCfg)
};

static const uint8_t qmiLocSetPremiumServicesCfgIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPremiumServicesCfgIndMsgT_v02, status)
};

static const uint8_t qmiLocSetXtraVersionCheckReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetXtraVersionCheckReqMsgT_v02, xtraVersionCheckMode)
};

static const uint8_t qmiLocSetXtraVersionCheckIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetXtraVersionCheckIndMsgT_v02, status)
};

static const uint8_t qmiLocSetGNSSConstRepConfigReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigReqMsgT_v02, measReportConfig) - QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigReqMsgT_v02, measReportConfig_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigReqMsgT_v02, measReportConfig),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigReqMsgT_v02, svPolyReportConfig) - QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigReqMsgT_v02, svPolyReportConfig_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigReqMsgT_v02, svPolyReportConfig),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigReqMsgT_v02, reportFullEphemerisDb) - QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigReqMsgT_v02, reportFullEphemerisDb_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigReqMsgT_v02, reportFullEphemerisDb),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigReqMsgT_v02, reportFullSvPolyDb) - QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigReqMsgT_v02, reportFullSvPolyDb_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigReqMsgT_v02, reportFullSvPolyDb)
};

static const uint8_t qmiLocSetGNSSConstRepConfigIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetGNSSConstRepConfigIndMsgT_v02, status)
};

static const uint8_t qmiLocInjectGtpClientDownloadedDataReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectGtpClientDownloadedDataReqMsgT_v02, ClientDownloadedData),
  ((QMI_LOC_MAX_GTP_WWAN_CLIENT_DOWNLOADED_DATA_LEN_V02) & 0xFF), ((QMI_LOC_MAX_GTP_WWAN_CLIENT_DOWNLOADED_DATA_LEN_V02) >> 8),
  QMI_IDL_OFFSET8(qmiLocInjectGtpClientDownloadedDataReqMsgT_v02, ClientDownloadedData) - QMI_IDL_OFFSET8(qmiLocInjectGtpClientDownloadedDataReqMsgT_v02, ClientDownloadedData_len)
};

static const uint8_t qmiLocInjectGtpClientDownloadedDataIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectGtpClientDownloadedDataIndMsgT_v02, status)
};

static const uint8_t qmiLocStartDbtReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartDbtReqMsgT_v02, reqId),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartDbtReqMsgT_v02, minDistance),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartDbtReqMsgT_v02, distanceType),

  0x04,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartDbtReqMsgT_v02, needOriginLocation),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartDbtReqMsgT_v02, maxLatency) - QMI_IDL_OFFSET8(qmiLocStartDbtReqMsgT_v02, maxLatency_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartDbtReqMsgT_v02, maxLatency),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartDbtReqMsgT_v02, usageType) - QMI_IDL_OFFSET8(qmiLocStartDbtReqMsgT_v02, usageType_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartDbtReqMsgT_v02, usageType)
};

static const uint8_t qmiLocStartDbtIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartDbtIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartDbtIndMsgT_v02, reqId) - QMI_IDL_OFFSET8(qmiLocStartDbtIndMsgT_v02, reqId_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartDbtIndMsgT_v02, reqId)
};

static const uint8_t qmiLocStopDbtReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStopDbtReqMsgT_v02, reqId)
};

static const uint8_t qmiLocStopDbtIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStopDbtIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStopDbtIndMsgT_v02, reqId) - QMI_IDL_OFFSET8(qmiLocStopDbtIndMsgT_v02, reqId_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStopDbtIndMsgT_v02, reqId)
};

static const uint8_t qmiLocEventDbtPositionReportIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, reqId),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, dbtPosition),
  QMI_IDL_TYPE88(0, 17),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, positionType),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, headingUnc) - QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, headingUnc_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, headingUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, speedUnc) - QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, speedUnc_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, speedUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, horConfidence) - QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, horConfidence_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, horConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, vertConfidence) - QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, vertConfidence_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, vertConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, DOP) - QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, DOP_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, DOP),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, gnssSvUsedList) - QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, gnssSvUsedList_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, gnssSvUsedList),
  QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02,
  QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, gnssSvUsedList) - QMI_IDL_OFFSET8(qmiLocEventDbtPositionReportIndMsgT_v02, gnssSvUsedList_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventDbtPositionReportIndMsgT_v02, positionSrc) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventDbtPositionReportIndMsgT_v02, positionSrc_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventDbtPositionReportIndMsgT_v02, positionSrc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventDbtPositionReportIndMsgT_v02, extDOP) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventDbtPositionReportIndMsgT_v02, extDOP_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventDbtPositionReportIndMsgT_v02, extDOP),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventDbtPositionReportIndMsgT_v02, expandedGnssSvUsedList) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventDbtPositionReportIndMsgT_v02, expandedGnssSvUsedList_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventDbtPositionReportIndMsgT_v02, expandedGnssSvUsedList),
  QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02,
  QMI_IDL_OFFSET16RELATIVE(qmiLocEventDbtPositionReportIndMsgT_v02, expandedGnssSvUsedList) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventDbtPositionReportIndMsgT_v02, expandedGnssSvUsedList_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventDbtPositionReportIndMsgT_v02, gnssSvUsedSignalTypeList) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventDbtPositionReportIndMsgT_v02, gnssSvUsedSignalTypeList_valid)),
  0x19,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventDbtPositionReportIndMsgT_v02, gnssSvUsedSignalTypeList),
  QMI_LOC_EXPANDED_SV_INFO_LIST_MAX_SIZE_V02,
  QMI_IDL_OFFSET16RELATIVE(qmiLocEventDbtPositionReportIndMsgT_v02, gnssSvUsedSignalTypeList) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventDbtPositionReportIndMsgT_v02, gnssSvUsedSignalTypeList_len)
};

static const uint8_t qmiLocEventDbtSessionStatusIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventDbtSessionStatusIndMsgT_v02, dbtSessionStatus),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventDbtSessionStatusIndMsgT_v02, reqId) - QMI_IDL_OFFSET8(qmiLocEventDbtSessionStatusIndMsgT_v02, reqId_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventDbtSessionStatusIndMsgT_v02, reqId)
};

static const uint8_t qmiLocEventBatchingStatusIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventBatchingStatusIndMsgT_v02, batchingStatus)
};

static const uint8_t qmiLocQueryAonConfigReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocQueryAonConfigReqMsgT_v02, transactionId)
};

static const uint8_t qmiLocQueryAonConfigIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocQueryAonConfigIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocQueryAonConfigIndMsgT_v02, transactionId) - QMI_IDL_OFFSET8(qmiLocQueryAonConfigIndMsgT_v02, transactionId_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocQueryAonConfigIndMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocQueryAonConfigIndMsgT_v02, aonCapability) - QMI_IDL_OFFSET8(qmiLocQueryAonConfigIndMsgT_v02, aonCapability_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocQueryAonConfigIndMsgT_v02, aonCapability)
};

static const uint8_t qmiLocDeleteGNSSServiceDataReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteAllFlag),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteClockInfoMask) - QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteClockInfoMask_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteClockInfoMask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteCellDbDataMask) - QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteCellDbDataMask_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteCellDbDataMask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteCommonDataMask) - QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteCommonDataMask_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteCommonDataMask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteSatelliteData) - QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteSatelliteData_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataReqMsgT_v02, deleteSatelliteData),
  QMI_IDL_TYPE88(0, 18)
};

static const uint8_t qmiLocDeleteGNSSServiceDataIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteGNSSServiceDataIndMsgT_v02, status)
};

static const uint8_t qmiLocInjectXtraDataReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectXtraDataReqMsgT_v02, totalSize),

  0x02,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectXtraDataReqMsgT_v02, totalParts),

  0x03,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectXtraDataReqMsgT_v02, partNum),

  0x04,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectXtraDataReqMsgT_v02, partData),
  ((QMI_LOC_MAX_XTRA_PART_LEN_V02) & 0xFF), ((QMI_LOC_MAX_XTRA_PART_LEN_V02) >> 8),
  QMI_IDL_OFFSET8(qmiLocInjectXtraDataReqMsgT_v02, partData) - QMI_IDL_OFFSET8(qmiLocInjectXtraDataReqMsgT_v02, partData_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectXtraDataReqMsgT_v02, formatType) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectXtraDataReqMsgT_v02, formatType_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectXtraDataReqMsgT_v02, formatType)
};

static const uint8_t qmiLocInjectXtraDataIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectXtraDataIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectXtraDataIndMsgT_v02, partNum) - QMI_IDL_OFFSET8(qmiLocInjectXtraDataIndMsgT_v02, partNum_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectXtraDataIndMsgT_v02, partNum),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectXtraDataIndMsgT_v02, ConstellationMask) - QMI_IDL_OFFSET8(qmiLocInjectXtraDataIndMsgT_v02, ConstellationMask_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectXtraDataIndMsgT_v02, ConstellationMask)
};

static const uint8_t qmiLocInjectXtraPcidReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectXtraPcidReqMsgT_v02, xtraPcid)
};

static const uint8_t qmiLocInjectXtraPcidIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectXtraPcidIndMsgT_v02, status)
};

/*
 * qmiLocGetSupportedFeatureReqMsgT is empty
 * static const uint8_t qmiLocGetSupportedFeatureReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetSupportedFeatureIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetSupportedFeatureIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetSupportedFeatureIndMsgT_v02, feature),
  QMI_LOC_SUPPORTED_FEATURE_LENGTH_V02,
  QMI_IDL_OFFSET8(qmiLocGetSupportedFeatureIndMsgT_v02, feature) - QMI_IDL_OFFSET8(qmiLocGetSupportedFeatureIndMsgT_v02, feature_len)
};

static const uint8_t qmiLocQueryXtraInfoReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocQueryXtraInfoReqMsgT_v02, xtraConfig)
};

static const uint8_t qmiLocQueryXtraInfoIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocQueryXtraInfoIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocQueryXtraInfoIndMsgT_v02, xtraInfo),
  QMI_IDL_TYPE88(0, 19)
};

static const uint8_t qmiLocStartOutdoorTripBatchingReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartOutdoorTripBatchingReqMsgT_v02, batchDistance),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartOutdoorTripBatchingReqMsgT_v02, minTimeInterval),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartOutdoorTripBatchingReqMsgT_v02, fixSessionTimeout) - QMI_IDL_OFFSET8(qmiLocStartOutdoorTripBatchingReqMsgT_v02, fixSessionTimeout_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartOutdoorTripBatchingReqMsgT_v02, fixSessionTimeout),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartOutdoorTripBatchingReqMsgT_v02, batchAllPos) - QMI_IDL_OFFSET8(qmiLocStartOutdoorTripBatchingReqMsgT_v02, batchAllPos_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartOutdoorTripBatchingReqMsgT_v02, batchAllPos)
};

static const uint8_t qmiLocStartOutdoorTripBatchingIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartOutdoorTripBatchingIndMsgT_v02, status)
};

/*
 * qmiLocQueryOTBAccumulatedDistanceReqMsgT is empty
 * static const uint8_t qmiLocQueryOTBAccumulatedDistanceReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocQueryOTBAccumulatedDistanceIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocQueryOTBAccumulatedDistanceIndMsgT_v02, status),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocQueryOTBAccumulatedDistanceIndMsgT_v02, accumulatedDistance),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocQueryOTBAccumulatedDistanceIndMsgT_v02, batchedPosition)
};

static const uint8_t qmiLocEventFdclServiceReqIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventFdclServiceReqIndMsgT_v02, gtpClientSwVer) - QMI_IDL_OFFSET8(qmiLocEventFdclServiceReqIndMsgT_v02, gtpClientSwVer_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventFdclServiceReqIndMsgT_v02, gtpClientSwVer),
  QMI_IDL_TYPE88(0, 20),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventFdclServiceReqIndMsgT_v02, modemSwVersion) - QMI_IDL_OFFSET8(qmiLocEventFdclServiceReqIndMsgT_v02, modemSwVersion_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocEventFdclServiceReqIndMsgT_v02, modemSwVersion),
  QMI_LOC_GNSS_SW_VERSION_STRING_MAX_LENGTH_V02,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, homeCarrierId) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, homeCarrierId_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, homeCarrierId),
  QMI_IDL_TYPE88(0, 21),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, cdmaRealm) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, cdmaRealm_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, cdmaRealm),
  QMI_LOC_GNSS_SW_VERSION_STRING_MAX_LENGTH_V02,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, gtpMaxNumRecordsAccepted) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, gtpMaxNumRecordsAccepted_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, gtpMaxNumRecordsAccepted),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, gpsWeek) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, gpsWeek_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, gpsWeek),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, gpsMilliseconds) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, gpsMilliseconds_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, gpsMilliseconds),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, bsBestPosition) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, bsBestPosition_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, bsBestPosition),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, currentCellId) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, currentCellId_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, currentCellId),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, requestedDataTypes) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, requestedDataTypes_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, requestedDataTypes),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, restrictedRegionVersion) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, restrictedRegionVersion_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, restrictedRegionVersion),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, rfBandsLte) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, rfBandsLte_valid)),
  0x1B,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, rfBandsLte),
  QMI_IDL_TYPE88(0, 30),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, rfBandsGSM) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, rfBandsGSM_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, rfBandsGSM),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, capabilitiesEnabled) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, capabilitiesEnabled_valid)),
  0x1D,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, capabilitiesEnabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, scanAssistId) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, scanAssistId_valid)),
  0x1E,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, scanAssistId),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, configId) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventFdclServiceReqIndMsgT_v02, configId_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventFdclServiceReqIndMsgT_v02, configId)
};

static const uint8_t qmiLocGetFdclBsListReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetFdclBsListReqMsgT_v02, maxNumBs) - QMI_IDL_OFFSET8(qmiLocGetFdclBsListReqMsgT_v02, maxNumBs_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFdclBsListReqMsgT_v02, maxNumBs)
};

static const uint8_t qmiLocGetFdclBsListIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFdclBsListIndMsgT_v02, status),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetFdclBsListIndMsgT_v02, BsList),
  QMI_LOC_FDCL_BS_LIST_MAX_SIZE_V02,
  QMI_IDL_OFFSET8(qmiLocGetFdclBsListIndMsgT_v02, BsList) - QMI_IDL_OFFSET8(qmiLocGetFdclBsListIndMsgT_v02, BsList_len),
  QMI_IDL_TYPE88(0, 24),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetFdclBsListIndMsgT_v02, moreBsAvailable) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetFdclBsListIndMsgT_v02, moreBsAvailable_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetFdclBsListIndMsgT_v02, moreBsAvailable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetFdclBsListIndMsgT_v02, bsBestPosition) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetFdclBsListIndMsgT_v02, bsBestPosition_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetFdclBsListIndMsgT_v02, bsBestPosition),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetFdclBsListIndMsgT_v02, dataBuffer) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetFdclBsListIndMsgT_v02, dataBuffer_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetFdclBsListIndMsgT_v02, dataBuffer),
  ((QMI_LOC_SCAN_DATA_MAX_SIZE_V02) & 0xFF), ((QMI_LOC_SCAN_DATA_MAX_SIZE_V02) >> 8),
  QMI_IDL_OFFSET16RELATIVE(qmiLocGetFdclBsListIndMsgT_v02, dataBuffer) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetFdclBsListIndMsgT_v02, dataBuffer_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetFdclBsListIndMsgT_v02, servCellId) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetFdclBsListIndMsgT_v02, servCellId_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetFdclBsListIndMsgT_v02, servCellId),
  QMI_IDL_TYPE88(0, 29)
};

static const uint8_t qmiLocInjectFdclDataReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocInjectFdclDataReqMsgT_v02, cellPosList),
  QMI_LOC_FDCL_CELL_POS_LIST_LENGTH_V02,
  QMI_IDL_OFFSET8(qmiLocInjectFdclDataReqMsgT_v02, cellPosList) - QMI_IDL_OFFSET8(qmiLocInjectFdclDataReqMsgT_v02, cellPosList_len),
  QMI_IDL_TYPE88(0, 25),

  0x02,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, daysValid),

  0x03,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, timestampUtc) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, timestampUtc_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, timestampUtc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, cellRecordTypeList) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, cellRecordTypeList_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, cellRecordTypeList),
  QMI_LOC_FDCL_CELL_POS_LIST_LENGTH_V02,
  QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, cellRecordTypeList) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, cellRecordTypeList_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, moreBsAvailable) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, moreBsAvailable_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, moreBsAvailable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, clientControls) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, clientControls_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, clientControls),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, maxNumCellsToUpload) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, maxNumCellsToUpload_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, maxNumCellsToUpload),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, minSecsBtwRequests) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, minSecsBtwRequests_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, minSecsBtwRequests),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, restrictedRegionVersion) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, restrictedRegionVersion_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, restrictedRegionVersion),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, maxNumCellsToDownload) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, maxNumCellsToDownload_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, maxNumCellsToDownload),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, maxGtpCellRequestsPerDay) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, maxGtpCellRequestsPerDay_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, maxGtpCellRequestsPerDay),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, posDeltaTimeThreshold) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, posDeltaTimeThreshold_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, posDeltaTimeThreshold),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, restrictedRegions) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, restrictedRegions_valid)),
  0x1A,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, restrictedRegions),
  QMI_LOC_RESTRICTED_REGIONS_LIST_MAX_SIZE_V02,
  QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, restrictedRegions) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, restrictedRegions_len),
  QMI_IDL_TYPE88(0, 21),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, responseBuffer) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, responseBuffer_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, responseBuffer),
  ((QMI_LOC_INJ_DATA_MAX_SIZE_V02) & 0xFF), ((QMI_LOC_INJ_DATA_MAX_SIZE_V02) >> 8),
  QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, responseBuffer) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, responseBuffer_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, responsePos) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectFdclDataReqMsgT_v02, responsePos_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectFdclDataReqMsgT_v02, responsePos),
  QMI_IDL_TYPE88(0, 31)
};

static const uint8_t qmiLocInjectFdclDataIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectFdclDataIndMsgT_v02, status),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectFdclDataIndMsgT_v02, inject_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectFdclDataIndMsgT_v02, errorMsg) - QMI_IDL_OFFSET8(qmiLocInjectFdclDataIndMsgT_v02, errorMsg_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocInjectFdclDataIndMsgT_v02, errorMsg),
  QMI_LOC_INJECT_FDCL_DATA_ERROR_MSG_LEN_V02
};

/*
 * qmiLocStartListenPositionReqMsgT is empty
 * static const uint8_t qmiLocStartListenPositionReqMsgT_data_v02[] = {
 * };
 */

/*
 * qmiLocStopListenPositionReqMsgT is empty
 * static const uint8_t qmiLocStopListenPositionReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocSystemInfoReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSystemInfoReqMsgT_v02, systemInfo)
};

static const uint8_t qmiLocSystemInfoIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSystemInfoIndMsgT_v02, systemInfo),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSystemInfoIndMsgT_v02, nextLeapSecondInfo) - QMI_IDL_OFFSET8(qmiLocSystemInfoIndMsgT_v02, nextLeapSecondInfo_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocSystemInfoIndMsgT_v02, nextLeapSecondInfo),
  QMI_IDL_TYPE88(0, 26),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSystemInfoIndMsgT_v02, gpsTimeUncMs) - QMI_IDL_OFFSET8(qmiLocSystemInfoIndMsgT_v02, gpsTimeUncMs_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSystemInfoIndMsgT_v02, gpsTimeUncMs)
};

static const uint8_t qmiLocGetConstellationConfigIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, gps_status) - QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, gps_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, gps_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, glonass_status) - QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, glonass_status_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, glonass_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, bds_status) - QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, bds_status_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, bds_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, qzss_status) - QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, qzss_status_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, qzss_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, galileo_status) - QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, galileo_status_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetConstellationConfigIndMsgT_v02, galileo_status)
};

static const uint8_t qmiLocEnablePositionInjectionTestModeReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEnablePositionInjectionTestModeReqMsgT_v02, enableTestMode)
};

static const uint8_t qmiLocEnablePositionInjectionTestModeIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEnablePositionInjectionTestModeIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEnablePositionInjectionTestModeIndMsgT_v02, testModeState) - QMI_IDL_OFFSET8(qmiLocEnablePositionInjectionTestModeIndMsgT_v02, testModeState_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEnablePositionInjectionTestModeIndMsgT_v02, testModeState)
};

static const uint8_t qmiLocInjectTestPositionReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectTestPositionReqMsgT_v02, latitude),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectTestPositionReqMsgT_v02, longitude),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectTestPositionReqMsgT_v02, horUncCircular),

  0x04,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectTestPositionReqMsgT_v02, timestampUtc),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectTestPositionReqMsgT_v02, positionSource) - QMI_IDL_OFFSET8(qmiLocInjectTestPositionReqMsgT_v02, positionSource_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectTestPositionReqMsgT_v02, positionSource)
};

static const uint8_t qmiLocInjectTestPositionIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectTestPositionIndMsgT_v02, status)
};

static const uint8_t qmiLocEventBsObsDataServiceReqIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventBsObsDataServiceReqIndMsgT_v02, numCellsAvailable)
};

static const uint8_t qmiLocGetBsObsDataReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetBsObsDataReqMsgT_v02, numCellsUpload) - QMI_IDL_OFFSET8(qmiLocGetBsObsDataReqMsgT_v02, numCellsUpload_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBsObsDataReqMsgT_v02, numCellsUpload)
};

static const uint8_t qmiLocGetBsObsDataIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetBsObsDataIndMsgT_v02, status),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetBsObsDataIndMsgT_v02, bsData),
  QMI_LOC_FDCL_BS_LIST_MAX_SIZE_V02,
  QMI_IDL_OFFSET8(qmiLocGetBsObsDataIndMsgT_v02, bsData) - QMI_IDL_OFFSET8(qmiLocGetBsObsDataIndMsgT_v02, bsData_len),
  QMI_IDL_TYPE88(0, 27),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocGetBsObsDataIndMsgT_v02, moreBsAvailable) - QMI_IDL_OFFSET16RELATIVE(qmiLocGetBsObsDataIndMsgT_v02, moreBsAvailable_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocGetBsObsDataIndMsgT_v02, moreBsAvailable)
};

static const uint8_t qmiLocEventReportIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventReportIndMsgT_v02, eventReport)
};

static const uint8_t qmiLocEventNmeaIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocEventNmeaIndMsgT_v02, nmea),
  QMI_LOC_NMEA_STRING_MAX_LENGTH_V02,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventNmeaIndMsgT_v02, expandedNmea) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventNmeaIndMsgT_v02, expandedNmea_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventNmeaIndMsgT_v02, expandedNmea),
  ((QMI_LOC_EXPANDED_NMEA_STRING_MAX_LENGTH_V02) & 0xFF), ((QMI_LOC_EXPANDED_NMEA_STRING_MAX_LENGTH_V02) >> 8)
};

static const uint8_t qmiLocGetLocationBackoffPeriodReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetLocationBackoffPeriodReqMsgT_v02, transactionId)
};

static const uint8_t qmiLocGetLocationBackoffPeriodIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetLocationBackoffPeriodIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetLocationBackoffPeriodIndMsgT_v02, transactionId) - QMI_IDL_OFFSET8(qmiLocGetLocationBackoffPeriodIndMsgT_v02, transactionId_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetLocationBackoffPeriodIndMsgT_v02, transactionId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetLocationBackoffPeriodIndMsgT_v02, locationBackoffPeriodMs) - QMI_IDL_OFFSET8(qmiLocGetLocationBackoffPeriodIndMsgT_v02, locationBackoffPeriodMs_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetLocationBackoffPeriodIndMsgT_v02, locationBackoffPeriodMs),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetLocationBackoffPeriodIndMsgT_v02, distanceToNearestGeofence) - QMI_IDL_OFFSET8(qmiLocGetLocationBackoffPeriodIndMsgT_v02, distanceToNearestGeofence_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetLocationBackoffPeriodIndMsgT_v02, distanceToNearestGeofence)
};

/*
 * qmiLocQueryGNSSEnergyConsumedReqMsgT is empty
 * static const uint8_t qmiLocQueryGNSSEnergyConsumedReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocQueryGNSSEnergyConsumedIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocQueryGNSSEnergyConsumedIndMsgT_v02, energyConsumedSinceLastBoot),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocQueryGNSSEnergyConsumedIndMsgT_v02, energyConsumedSinceFirstBoot)
};

static const uint8_t qmiLocEventGnssSvInfoIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventGnssSvInfoIndMsgT_v02, altitudeAssumed),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventGnssSvInfoIndMsgT_v02, svList) - QMI_IDL_OFFSET8(qmiLocEventGnssSvInfoIndMsgT_v02, svList_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventGnssSvInfoIndMsgT_v02, svList),
  QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02,
  QMI_IDL_OFFSET8(qmiLocEventGnssSvInfoIndMsgT_v02, svList) - QMI_IDL_OFFSET8(qmiLocEventGnssSvInfoIndMsgT_v02, svList_len),
  QMI_IDL_TYPE88(0, 28)
};

static const uint8_t qmiLocGetWwanDataReqMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetWwanDataReqMsgT_v02, numOfCells),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetWwanDataReqMsgT_v02, isAssisted),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetWwanDataReqMsgT_v02, airInterface)
};

static const uint8_t qmiLocGetWwanDataIndMsgT_data_v02[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetWwanDataIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetWwanDataIndMsgT_v02, moreBsAvailable) - QMI_IDL_OFFSET8(qmiLocGetWwanDataIndMsgT_v02, moreBsAvailable_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetWwanDataIndMsgT_v02, moreBsAvailable),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetWwanDataIndMsgT_v02, dataBuffer) - QMI_IDL_OFFSET8(qmiLocGetWwanDataIndMsgT_v02, dataBuffer_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetWwanDataIndMsgT_v02, dataBuffer),
  ((QMI_LOC_DATA_BUFF_MAX_SIZE_V02) & 0xFF), ((QMI_LOC_DATA_BUFF_MAX_SIZE_V02) >> 8),
  QMI_IDL_OFFSET8(qmiLocGetWwanDataIndMsgT_v02, dataBuffer) - QMI_IDL_OFFSET8(qmiLocGetWwanDataIndMsgT_v02, dataBuffer_len)
};

/* Type Table */
static const qmi_idl_type_table_entry  loc_iot_type_table_v02[] = {
  {sizeof(qmiLocPowerModeStructT_v02), qmiLocPowerModeStructT_data_v02},
  {sizeof(qmiLocGPSTimeStructT_v02), qmiLocGPSTimeStructT_data_v02},
  {sizeof(qmiLocDOPStructT_v02), qmiLocDOPStructT_data_v02},
  {sizeof(qmiLocExtDOPStructT_v02), qmiLocExtDOPStructT_data_v02},
  {sizeof(qmiLocSensorUsageIndicatorStructT_v02), qmiLocSensorUsageIndicatorStructT_data_v02},
  {sizeof(qmiLocJammerIndicatorStructT_v02), qmiLocJammerIndicatorStructT_data_v02},
  {sizeof(qmiLocAssistanceServerUrlStructT_v02), qmiLocAssistanceServerUrlStructT_data_v02},
  {sizeof(qmiLocTimeServerListStructT_v02), qmiLocTimeServerListStructT_data_v02},
  {sizeof(qmiLocPredictedOrbitsAllowedSizesStructT_v02), qmiLocPredictedOrbitsAllowedSizesStructT_data_v02},
  {sizeof(qmiLocPredictedOrbitsServerListStructT_v02), qmiLocPredictedOrbitsServerListStructT_data_v02},
  {sizeof(qmiLocGeofencePositionStructT_v02), qmiLocGeofencePositionStructT_data_v02},
  {sizeof(qmiLocPredictedOrbitsDataValidityStructT_v02), qmiLocPredictedOrbitsDataValidityStructT_data_v02},
  {sizeof(qmiLocAltitudeSrcInfoStructT_v02), qmiLocAltitudeSrcInfoStructT_data_v02},
  {sizeof(qmiLocCircularGeofenceArgsStructT_v02), qmiLocCircularGeofenceArgsStructT_data_v02},
  {sizeof(qmiLocGeofenceMotionStateConfigStructT_v02), qmiLocGeofenceMotionStateConfigStructT_data_v02},
  {sizeof(qmiLocGSMCellIdStructT_v02), qmiLocGSMCellIdStructT_data_v02},
  {sizeof(qmiLocBatchedReportStructT_v02), qmiLocBatchedReportStructT_data_v02},
  {sizeof(qmiLocDbtPositionStructT_v02), qmiLocDbtPositionStructT_data_v02},
  {sizeof(qmiLocDeleteSatelliteDataStructT_v02), qmiLocDeleteSatelliteDataStructT_data_v02},
  {sizeof(qmiLocXtraInfoStructT_v02), qmiLocXtraInfoStructT_data_v02},
  {sizeof(qmiLocGtpClientSwVerStructT_v02), qmiLocGtpClientSwVerStructT_data_v02},
  {sizeof(qmiLocHomeCarrierIdStructT_v02), qmiLocHomeCarrierIdStructT_data_v02},
  {sizeof(qmiLocPositionSourceStructT_v02), qmiLocPositionSourceStructT_data_v02},
  {sizeof(qmiLocBsBestPositionStructT_v02), qmiLocBsBestPositionStructT_data_v02},
  {sizeof(qmiLocFdclCellIdStructT_v02), qmiLocFdclCellIdStructT_data_v02},
  {sizeof(qmiLocFdclCellPosStructT_v02), qmiLocFdclCellPosStructT_data_v02},
  {sizeof(qmiLocNextLeapSecondInfoStructT_v02), qmiLocNextLeapSecondInfoStructT_data_v02},
  {sizeof(qmiLocBsObservedDataStructT_v02), qmiLocBsObservedDataStructT_data_v02},
  {sizeof(qmiLocSvInfoStructT_v02), qmiLocSvInfoStructT_data_v02},
  {sizeof(qmiLocFdclCellIdExtStructT_v02), qmiLocFdclCellIdExtStructT_data_v02},
  {sizeof(qmiLTEBandStructT_v02), qmiLTEBandStructT_data_v02},
  {sizeof(qmiLocFdclPosStructT_v02), qmiLocFdclPosStructT_data_v02}
};

/* Message Table */
static const qmi_idl_message_table_entry loc_iot_message_table_v02[] = {
  {sizeof(qmiLocGenReqMsgT_v02), 0},
  {sizeof(qmiLocGenRespMsgT_v02), qmiLocGenRespMsgT_data_v02},
  {sizeof(qmiLocInformClientRevisionReqMsgT_v02), qmiLocInformClientRevisionReqMsgT_data_v02},
  {sizeof(qmiLocRegEventsReqMsgT_v02), qmiLocRegEventsReqMsgT_data_v02},
  {sizeof(qmiLocStartReqMsgT_v02), qmiLocStartReqMsgT_data_v02},
  {sizeof(qmiLocStopReqMsgT_v02), qmiLocStopReqMsgT_data_v02},
  {sizeof(qmiLocEventPositionReportIndMsgT_v02), qmiLocEventPositionReportIndMsgT_data_v02},
  {sizeof(qmiLocEventInjectTimeReqIndMsgT_v02), qmiLocEventInjectTimeReqIndMsgT_data_v02},
  {sizeof(qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02), qmiLocEventInjectPredictedOrbitsReqIndMsgT_data_v02},
  {sizeof(qmiLocEventEngineStateIndMsgT_v02), qmiLocEventEngineStateIndMsgT_data_v02},
  {sizeof(qmiLocEventFixSessionStateIndMsgT_v02), qmiLocEventFixSessionStateIndMsgT_data_v02},
  {sizeof(qmiLocEventGeofenceGenAlertIndMsgT_v02), qmiLocEventGeofenceGenAlertIndMsgT_data_v02},
  {sizeof(qmiLocEventGeofenceBreachIndMsgT_v02), qmiLocEventGeofenceBreachIndMsgT_data_v02},
  {sizeof(qmiLocGetServiceRevisionReqMsgT_v02), 0},
  {sizeof(qmiLocGetServiceRevisionIndMsgT_v02), qmiLocGetServiceRevisionIndMsgT_data_v02},
  {sizeof(qmiLocGetFixCriteriaReqMsgT_v02), 0},
  {sizeof(qmiLocGetFixCriteriaIndMsgT_v02), qmiLocGetFixCriteriaIndMsgT_data_v02},
  {sizeof(qmiLocGetPredictedOrbitsDataSourceReqMsgT_v02), 0},
  {sizeof(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02), qmiLocGetPredictedOrbitsDataSourceIndMsgT_data_v02},
  {sizeof(qmiLocGetPredictedOrbitsDataValidityReqMsgT_v02), 0},
  {sizeof(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02), qmiLocGetPredictedOrbitsDataValidityIndMsgT_data_v02},
  {sizeof(qmiLocInjectUtcTimeReqMsgT_v02), qmiLocInjectUtcTimeReqMsgT_data_v02},
  {sizeof(qmiLocInjectUtcTimeIndMsgT_v02), qmiLocInjectUtcTimeIndMsgT_data_v02},
  {sizeof(qmiLocInjectPositionReqMsgT_v02), qmiLocInjectPositionReqMsgT_data_v02},
  {sizeof(qmiLocInjectPositionIndMsgT_v02), qmiLocInjectPositionIndMsgT_data_v02},
  {sizeof(qmiLocSetXtraTSessionControlReqMsgT_v02), qmiLocSetXtraTSessionControlReqMsgT_data_v02},
  {sizeof(qmiLocSetXtraTSessionControlIndMsgT_v02), qmiLocSetXtraTSessionControlIndMsgT_data_v02},
  {sizeof(qmiLocGetXtraTSessionControlReqMsgT_v02), 0},
  {sizeof(qmiLocGetXtraTSessionControlIndMsgT_v02), qmiLocGetXtraTSessionControlIndMsgT_data_v02},
  {sizeof(qmiLocGetRegisteredEventsReqMsgT_v02), 0},
  {sizeof(qmiLocGetRegisteredEventsIndMsgT_v02), qmiLocGetRegisteredEventsIndMsgT_data_v02},
  {sizeof(qmiLocSetPositionEngineConfigParametersReqMsgT_v02), qmiLocSetPositionEngineConfigParametersReqMsgT_data_v02},
  {sizeof(qmiLocSetPositionEngineConfigParametersIndMsgT_v02), qmiLocSetPositionEngineConfigParametersIndMsgT_data_v02},
  {sizeof(qmiLocGetPositionEngineConfigParametersReqMsgT_v02), qmiLocGetPositionEngineConfigParametersReqMsgT_data_v02},
  {sizeof(qmiLocGetPositionEngineConfigParametersIndMsgT_v02), qmiLocGetPositionEngineConfigParametersIndMsgT_data_v02},
  {sizeof(qmiLocAddCircularGeofenceReqMsgT_v02), qmiLocAddCircularGeofenceReqMsgT_data_v02},
  {sizeof(qmiLocAddCircularGeofenceIndMsgT_v02), qmiLocAddCircularGeofenceIndMsgT_data_v02},
  {sizeof(qmiLocDeleteGeofenceReqMsgT_v02), qmiLocDeleteGeofenceReqMsgT_data_v02},
  {sizeof(qmiLocDeleteGeofenceIndMsgT_v02), qmiLocDeleteGeofenceIndMsgT_data_v02},
  {sizeof(qmiLocSetGeofenceEngineConfigReqMsgT_v02), qmiLocSetGeofenceEngineConfigReqMsgT_data_v02},
  {sizeof(qmiLocSetGeofenceEngineConfigIndMsgT_v02), qmiLocSetGeofenceEngineConfigIndMsgT_data_v02},
  {sizeof(qmiLocGetGeofenceEngineConfigReqMsgT_v02), qmiLocGetGeofenceEngineConfigReqMsgT_data_v02},
  {sizeof(qmiLocGetGeofenceEngineConfigIndMsgT_v02), qmiLocGetGeofenceEngineConfigIndMsgT_data_v02},
  {sizeof(qmiLocGetBestAvailablePositionReqMsgT_v02), qmiLocGetBestAvailablePositionReqMsgT_data_v02},
  {sizeof(qmiLocGetBestAvailablePositionIndMsgT_v02), qmiLocGetBestAvailablePositionIndMsgT_data_v02},
  {sizeof(qmiLocInjectGSMCellInfoReqMsgT_v02), qmiLocInjectGSMCellInfoReqMsgT_data_v02},
  {sizeof(qmiLocInjectGSMCellInfoIndMsgT_v02), qmiLocInjectGSMCellInfoIndMsgT_data_v02},
  {sizeof(qmiLocGetBatchSizeReqMsgT_v02), qmiLocGetBatchSizeReqMsgT_data_v02},
  {sizeof(qmiLocGetBatchSizeIndMsgT_v02), qmiLocGetBatchSizeIndMsgT_data_v02},
  {sizeof(qmiLocStartBatchingReqMsgT_v02), qmiLocStartBatchingReqMsgT_data_v02},
  {sizeof(qmiLocStartBatchingIndMsgT_v02), qmiLocStartBatchingIndMsgT_data_v02},
  {sizeof(qmiLocEventBatchFullIndMsgT_v02), qmiLocEventBatchFullIndMsgT_data_v02},
  {sizeof(qmiLocReadFromBatchReqMsgT_v02), qmiLocReadFromBatchReqMsgT_data_v02},
  {sizeof(qmiLocReadFromBatchIndMsgT_v02), qmiLocReadFromBatchIndMsgT_data_v02},
  {sizeof(qmiLocStopBatchingReqMsgT_v02), qmiLocStopBatchingReqMsgT_data_v02},
  {sizeof(qmiLocStopBatchingIndMsgT_v02), qmiLocStopBatchingIndMsgT_data_v02},
  {sizeof(qmiLocReleaseBatchReqMsgT_v02), qmiLocReleaseBatchReqMsgT_data_v02},
  {sizeof(qmiLocReleaseBatchIndMsgT_v02), qmiLocReleaseBatchIndMsgT_data_v02},
  {sizeof(qmiLocGetAvailWwanPositionReqMsgT_v02), qmiLocGetAvailWwanPositionReqMsgT_data_v02},
  {sizeof(qmiLocGetAvailWwanPositionIndMsgT_v02), qmiLocGetAvailWwanPositionIndMsgT_data_v02},
  {sizeof(qmiLocSetPremiumServicesCfgReqMsgT_v02), qmiLocSetPremiumServicesCfgReqMsgT_data_v02},
  {sizeof(qmiLocSetPremiumServicesCfgIndMsgT_v02), qmiLocSetPremiumServicesCfgIndMsgT_data_v02},
  {sizeof(qmiLocSetXtraVersionCheckReqMsgT_v02), qmiLocSetXtraVersionCheckReqMsgT_data_v02},
  {sizeof(qmiLocSetXtraVersionCheckIndMsgT_v02), qmiLocSetXtraVersionCheckIndMsgT_data_v02},
  {sizeof(qmiLocSetGNSSConstRepConfigReqMsgT_v02), qmiLocSetGNSSConstRepConfigReqMsgT_data_v02},
  {sizeof(qmiLocSetGNSSConstRepConfigIndMsgT_v02), qmiLocSetGNSSConstRepConfigIndMsgT_data_v02},
  {sizeof(qmiLocInjectGtpClientDownloadedDataReqMsgT_v02), qmiLocInjectGtpClientDownloadedDataReqMsgT_data_v02},
  {sizeof(qmiLocInjectGtpClientDownloadedDataIndMsgT_v02), qmiLocInjectGtpClientDownloadedDataIndMsgT_data_v02},
  {sizeof(qmiLocStartDbtReqMsgT_v02), qmiLocStartDbtReqMsgT_data_v02},
  {sizeof(qmiLocStartDbtIndMsgT_v02), qmiLocStartDbtIndMsgT_data_v02},
  {sizeof(qmiLocStopDbtReqMsgT_v02), qmiLocStopDbtReqMsgT_data_v02},
  {sizeof(qmiLocStopDbtIndMsgT_v02), qmiLocStopDbtIndMsgT_data_v02},
  {sizeof(qmiLocEventDbtPositionReportIndMsgT_v02), qmiLocEventDbtPositionReportIndMsgT_data_v02},
  {sizeof(qmiLocEventDbtSessionStatusIndMsgT_v02), qmiLocEventDbtSessionStatusIndMsgT_data_v02},
  {sizeof(qmiLocEventBatchingStatusIndMsgT_v02), qmiLocEventBatchingStatusIndMsgT_data_v02},
  {sizeof(qmiLocQueryAonConfigReqMsgT_v02), qmiLocQueryAonConfigReqMsgT_data_v02},
  {sizeof(qmiLocQueryAonConfigIndMsgT_v02), qmiLocQueryAonConfigIndMsgT_data_v02},
  {sizeof(qmiLocDeleteGNSSServiceDataReqMsgT_v02), qmiLocDeleteGNSSServiceDataReqMsgT_data_v02},
  {sizeof(qmiLocDeleteGNSSServiceDataIndMsgT_v02), qmiLocDeleteGNSSServiceDataIndMsgT_data_v02},
  {sizeof(qmiLocInjectXtraDataReqMsgT_v02), qmiLocInjectXtraDataReqMsgT_data_v02},
  {sizeof(qmiLocInjectXtraDataIndMsgT_v02), qmiLocInjectXtraDataIndMsgT_data_v02},
  {sizeof(qmiLocInjectXtraPcidReqMsgT_v02), qmiLocInjectXtraPcidReqMsgT_data_v02},
  {sizeof(qmiLocInjectXtraPcidIndMsgT_v02), qmiLocInjectXtraPcidIndMsgT_data_v02},
  {sizeof(qmiLocGetSupportedFeatureReqMsgT_v02), 0},
  {sizeof(qmiLocGetSupportedFeatureIndMsgT_v02), qmiLocGetSupportedFeatureIndMsgT_data_v02},
  {sizeof(qmiLocQueryXtraInfoReqMsgT_v02), qmiLocQueryXtraInfoReqMsgT_data_v02},
  {sizeof(qmiLocQueryXtraInfoIndMsgT_v02), qmiLocQueryXtraInfoIndMsgT_data_v02},
  {sizeof(qmiLocStartOutdoorTripBatchingReqMsgT_v02), qmiLocStartOutdoorTripBatchingReqMsgT_data_v02},
  {sizeof(qmiLocStartOutdoorTripBatchingIndMsgT_v02), qmiLocStartOutdoorTripBatchingIndMsgT_data_v02},
  {sizeof(qmiLocQueryOTBAccumulatedDistanceReqMsgT_v02), 0},
  {sizeof(qmiLocQueryOTBAccumulatedDistanceIndMsgT_v02), qmiLocQueryOTBAccumulatedDistanceIndMsgT_data_v02},
  {sizeof(qmiLocEventFdclServiceReqIndMsgT_v02), qmiLocEventFdclServiceReqIndMsgT_data_v02},
  {sizeof(qmiLocGetFdclBsListReqMsgT_v02), qmiLocGetFdclBsListReqMsgT_data_v02},
  {sizeof(qmiLocGetFdclBsListIndMsgT_v02), qmiLocGetFdclBsListIndMsgT_data_v02},
  {sizeof(qmiLocInjectFdclDataReqMsgT_v02), qmiLocInjectFdclDataReqMsgT_data_v02},
  {sizeof(qmiLocInjectFdclDataIndMsgT_v02), qmiLocInjectFdclDataIndMsgT_data_v02},
  {sizeof(qmiLocStartListenPositionReqMsgT_v02), 0},
  {sizeof(qmiLocStopListenPositionReqMsgT_v02), 0},
  {sizeof(qmiLocSystemInfoReqMsgT_v02), qmiLocSystemInfoReqMsgT_data_v02},
  {sizeof(qmiLocSystemInfoIndMsgT_v02), qmiLocSystemInfoIndMsgT_data_v02},
  {sizeof(qmiLocGetConstellationConfigIndMsgT_v02), qmiLocGetConstellationConfigIndMsgT_data_v02},
  {sizeof(qmiLocEnablePositionInjectionTestModeReqMsgT_v02), qmiLocEnablePositionInjectionTestModeReqMsgT_data_v02},
  {sizeof(qmiLocEnablePositionInjectionTestModeIndMsgT_v02), qmiLocEnablePositionInjectionTestModeIndMsgT_data_v02},
  {sizeof(qmiLocInjectTestPositionReqMsgT_v02), qmiLocInjectTestPositionReqMsgT_data_v02},
  {sizeof(qmiLocInjectTestPositionIndMsgT_v02), qmiLocInjectTestPositionIndMsgT_data_v02},
  {sizeof(qmiLocEventBsObsDataServiceReqIndMsgT_v02), qmiLocEventBsObsDataServiceReqIndMsgT_data_v02},
  {sizeof(qmiLocGetBsObsDataReqMsgT_v02), qmiLocGetBsObsDataReqMsgT_data_v02},
  {sizeof(qmiLocGetBsObsDataIndMsgT_v02), qmiLocGetBsObsDataIndMsgT_data_v02},
  {sizeof(qmiLocEventReportIndMsgT_v02), qmiLocEventReportIndMsgT_data_v02},
  {sizeof(qmiLocEventNmeaIndMsgT_v02), qmiLocEventNmeaIndMsgT_data_v02},
  {sizeof(qmiLocGetLocationBackoffPeriodReqMsgT_v02), qmiLocGetLocationBackoffPeriodReqMsgT_data_v02},
  {sizeof(qmiLocGetLocationBackoffPeriodIndMsgT_v02), qmiLocGetLocationBackoffPeriodIndMsgT_data_v02},
  {sizeof(qmiLocQueryGNSSEnergyConsumedReqMsgT_v02), 0},
  {sizeof(qmiLocQueryGNSSEnergyConsumedIndMsgT_v02), qmiLocQueryGNSSEnergyConsumedIndMsgT_data_v02},
  {sizeof(qmiLocEventGnssSvInfoIndMsgT_v02), qmiLocEventGnssSvInfoIndMsgT_data_v02},
  {sizeof(qmiLocGetWwanDataReqMsgT_v02), qmiLocGetWwanDataReqMsgT_data_v02},
  {sizeof(qmiLocGetWwanDataIndMsgT_v02), qmiLocGetWwanDataIndMsgT_data_v02}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object loc_iot_qmi_idl_type_table_object_v02;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *loc_iot_qmi_idl_type_table_object_referenced_tables_v02[] =
{&loc_iot_qmi_idl_type_table_object_v02, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object loc_iot_qmi_idl_type_table_object_v02 = {
  sizeof(loc_iot_type_table_v02)/sizeof(qmi_idl_type_table_entry ),
  sizeof(loc_iot_message_table_v02)/sizeof(qmi_idl_message_table_entry),
  1,
  loc_iot_type_table_v02,
  loc_iot_message_table_v02,
  loc_iot_qmi_idl_type_table_object_referenced_tables_v02,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry loc_iot_service_command_messages_v02[] = {
  {QMI_LOC_INFORM_CLIENT_REVISION_REQ_V02, QMI_IDL_TYPE16(0, 2), 7},
  {QMI_LOC_REG_EVENTS_REQ_V02, QMI_IDL_TYPE16(0, 3), 18},
  {QMI_LOC_START_REQ_V02, QMI_IDL_TYPE16(0, 4), 77},
  {QMI_LOC_STOP_REQ_V02, QMI_IDL_TYPE16(0, 5), 4},
  {QMI_LOC_GET_SERVICE_REVISION_REQ_V02, QMI_IDL_TYPE16(0, 13), 0},
  {QMI_LOC_GET_FIX_CRITERIA_REQ_V02, QMI_IDL_TYPE16(0, 15), 0},
  {QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02, QMI_IDL_TYPE16(0, 17), 0},
  {QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02, QMI_IDL_TYPE16(0, 19), 0},
  {QMI_LOC_INJECT_UTC_TIME_REQ_V02, QMI_IDL_TYPE16(0, 21), 18},
  {QMI_LOC_INJECT_POSITION_REQ_V02, QMI_IDL_TYPE16(0, 23), 540},
  {QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02, QMI_IDL_TYPE16(0, 25), 4},
  {QMI_LOC_GET_XTRA_T_SESSION_CONTROL_REQ_V02, QMI_IDL_TYPE16(0, 27), 0},
  {QMI_LOC_GET_REGISTERED_EVENTS_REQ_V02, QMI_IDL_TYPE16(0, 29), 0},
  {QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02, QMI_IDL_TYPE16(0, 31), 16},
  {QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02, QMI_IDL_TYPE16(0, 33), 7},
  {QMI_LOC_ADD_CIRCULAR_GEOFENCE_REQ_V02, QMI_IDL_TYPE16(0, 35), 70},
  {QMI_LOC_DELETE_GEOFENCE_REQ_V02, QMI_IDL_TYPE16(0, 37), 14},
  {QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ_V02, QMI_IDL_TYPE16(0, 43), 7},
  {QMI_LOC_INJECT_GSM_CELL_INFO_REQ_V02, QMI_IDL_TYPE16(0, 45), 30},
  {QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_REQ_V02, QMI_IDL_TYPE16(0, 39), 224},
  {QMI_LOC_GET_GEOFENCE_ENGINE_CONFIG_REQ_V02, QMI_IDL_TYPE16(0, 41), 7},
  {QMI_LOC_GET_BATCH_SIZE_REQ_V02, QMI_IDL_TYPE16(0, 47), 21},
  {QMI_LOC_START_BATCHING_REQ_V02, QMI_IDL_TYPE16(0, 49), 39},
  {QMI_LOC_READ_FROM_BATCH_REQ_V02, QMI_IDL_TYPE16(0, 52), 21},
  {QMI_LOC_STOP_BATCHING_REQ_V02, QMI_IDL_TYPE16(0, 54), 21},
  {QMI_LOC_RELEASE_BATCH_REQ_V02, QMI_IDL_TYPE16(0, 56), 14},
  {QMI_LOC_GET_AVAILABLE_WWAN_POSITION_REQ_V02, QMI_IDL_TYPE16(0, 58), 22},
  {QMI_LOC_SET_PREMIUM_SERVICES_CONFIG_REQ_V02, QMI_IDL_TYPE16(0, 60), 14},
  {QMI_LOC_SET_XTRA_VERSION_CHECK_REQ_V02, QMI_IDL_TYPE16(0, 62), 7},
  {QMI_LOC_SET_GNSS_CONSTELL_REPORT_CONFIG_V02, QMI_IDL_TYPE16(0, 64), 30},
  {QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_REQ_V02, QMI_IDL_TYPE16(0, 66), 517},
  {QMI_LOC_START_DBT_REQ_V02, QMI_IDL_TYPE16(0, 68), 36},
  {QMI_LOC_STOP_DBT_REQ_V02, QMI_IDL_TYPE16(0, 70), 4},
  {QMI_LOC_QUERY_AON_CONFIG_REQ_V02, QMI_IDL_TYPE16(0, 75), 7},
  {QMI_LOC_DELETE_GNSS_SERVICE_DATA_REQ_V02, QMI_IDL_TYPE16(0, 77), 36},
  {QMI_LOC_INJECT_XTRA_DATA_REQ_V02, QMI_IDL_TYPE16(0, 79), 1053},
  {QMI_LOC_INJECT_XTRA_PCID_REQ_V02, QMI_IDL_TYPE16(0, 81), 11},
  {QMI_LOC_GET_SUPPORTED_FEATURE_REQ_V02, QMI_IDL_TYPE16(0, 83), 0},
  {QMI_LOC_QUERY_XTRA_INFO_REQ_V02, QMI_IDL_TYPE16(0, 85), 7},
  {QMI_LOC_START_OUTDOOR_TRIP_BATCHING_REQ_V02, QMI_IDL_TYPE16(0, 87), 25},
  {QMI_LOC_QUERY_OTB_ACCUMULATED_DISTANCE_REQ_V02, QMI_IDL_TYPE16(0, 89), 0},
  {QMI_LOC_GET_FDCL_BS_LIST_REQ_V02, QMI_IDL_TYPE16(0, 92), 5},
  {QMI_LOC_INJECT_FDCL_DATA_REQ_V02, QMI_IDL_TYPE16(0, 94), 4467},
  {QMI_LOC_START_LISTEN_POSITION_REQ_V02, QMI_IDL_TYPE16(0, 96), 0},
  {QMI_LOC_STOP_LISTEN_POSITION_REQ_V02, QMI_IDL_TYPE16(0, 97), 0},
  {QMI_LOC_SYSTEM_INFO_REQ_V02, QMI_IDL_TYPE16(0, 98), 7},
  {QMI_LOC_GET_CONSTELLATION_CONTROL_REQ_V02, QMI_IDL_TYPE16(0, 0), 0},
  {QMI_LOC_ENABLE_POSITION_INJECTION_TEST_MODE_REQ_V02, QMI_IDL_TYPE16(0, 101), 4},
  {QMI_LOC_INJECT_TEST_POSITION_REQ_V02, QMI_IDL_TYPE16(0, 103), 47},
  {QMI_LOC_GET_BS_OBS_DATA_REQ_V02, QMI_IDL_TYPE16(0, 106), 5},
  {QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_REQ_V02, QMI_IDL_TYPE16(0, 110), 7},
  {QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_REQ_V02, QMI_IDL_TYPE16(0, 112), 0},
  {QMI_LOC_GET_WWAN_DATA_REQ_V02, QMI_IDL_TYPE16(0, 115), 16}
};

static const qmi_idl_service_message_table_entry loc_iot_service_response_messages_v02[] = {
  {QMI_LOC_INFORM_CLIENT_REVISION_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_REG_EVENTS_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_START_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_STOP_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_SERVICE_REVISION_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_FIX_CRITERIA_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_INJECT_UTC_TIME_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_INJECT_POSITION_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_SET_XTRA_T_SESSION_CONTROL_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_XTRA_T_SESSION_CONTROL_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_REGISTERED_EVENTS_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_ADD_CIRCULAR_GEOFENCE_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_DELETE_GEOFENCE_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_BEST_AVAILABLE_POSITION_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_INJECT_GSM_CELL_INFO_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_GEOFENCE_ENGINE_CONFIG_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_BATCH_SIZE_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_START_BATCHING_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_READ_FROM_BATCH_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_STOP_BATCHING_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_RELEASE_BATCH_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_AVAILABLE_WWAN_POSITION_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_SET_PREMIUM_SERVICES_CONFIG_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_SET_XTRA_VERSION_CHECK_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_SET_GNSS_CONSTELL_REPORT_CONFIG_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_START_DBT_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_STOP_DBT_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_QUERY_AON_CONFIG_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_DELETE_GNSS_SERVICE_DATA_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_INJECT_XTRA_DATA_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_INJECT_XTRA_PCID_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_SUPPORTED_FEATURE_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_QUERY_XTRA_INFO_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_START_OUTDOOR_TRIP_BATCHING_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_QUERY_OTB_ACCUMULATED_DISTANCE_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_FDCL_BS_LIST_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_INJECT_FDCL_DATA_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_START_LISTEN_POSITION_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_STOP_LISTEN_POSITION_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_SYSTEM_INFO_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_CONSTELLATION_CONTROL_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_ENABLE_POSITION_INJECTION_TEST_MODE_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_INJECT_TEST_POSITION_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_BS_OBS_DATA_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_RESP_V02, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_LOC_GET_WWAN_DATA_RESP_V02, QMI_IDL_TYPE16(0, 1), 7}
};

static const qmi_idl_service_message_table_entry loc_iot_service_indication_messages_v02[] = {
  {QMI_LOC_EVENT_POSITION_REPORT_IND_V02, QMI_IDL_TYPE16(0, 6), 2412},
  {QMI_LOC_EVENT_INJECT_TIME_REQ_IND_V02, QMI_IDL_TYPE16(0, 7), 776},
  {QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_IND_V02, QMI_IDL_TYPE16(0, 8), 783},
  {QMI_LOC_EVENT_ENGINE_STATE_IND_V02, QMI_IDL_TYPE16(0, 9), 7},
  {QMI_LOC_EVENT_FIX_SESSION_STATE_IND_V02, QMI_IDL_TYPE16(0, 10), 11},
  {QMI_LOC_GET_SERVICE_REVISION_IND_V02, QMI_IDL_TYPE16(0, 14), 532},
  {QMI_LOC_GET_FIX_CRITERIA_IND_V02, QMI_IDL_TYPE16(0, 16), 64},
  {QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_IND_V02, QMI_IDL_TYPE16(0, 18), 790},
  {QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_IND_V02, QMI_IDL_TYPE16(0, 20), 20},
  {QMI_LOC_INJECT_UTC_TIME_IND_V02, QMI_IDL_TYPE16(0, 22), 7},
  {QMI_LOC_INJECT_POSITION_IND_V02, QMI_IDL_TYPE16(0, 24), 7},
  {QMI_LOC_SET_XTRA_T_SESSION_CONTROL_IND_V02, QMI_IDL_TYPE16(0, 26), 7},
  {QMI_LOC_GET_XTRA_T_SESSION_CONTROL_IND_V02, QMI_IDL_TYPE16(0, 28), 11},
  {QMI_LOC_GET_REGISTERED_EVENTS_IND_V02, QMI_IDL_TYPE16(0, 30), 18},
  {QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02, QMI_IDL_TYPE16(0, 32), 14},
  {QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02, QMI_IDL_TYPE16(0, 34), 23},
  {QMI_LOC_EVENT_GEOFENCE_GEN_ALERT_IND_V02, QMI_IDL_TYPE16(0, 11), 7},
  {QMI_LOC_EVENT_GEOFENCE_BREACH_NOTIFICATION_IND_V02, QMI_IDL_TYPE16(0, 12), 85},
  {QMI_LOC_ADD_CIRCULAR_GEOFENCE_IND_V02, QMI_IDL_TYPE16(0, 36), 21},
  {QMI_LOC_DELETE_GEOFENCE_IND_V02, QMI_IDL_TYPE16(0, 38), 21},
  {QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND_V02, QMI_IDL_TYPE16(0, 44), 2193},
  {QMI_LOC_INJECT_GSM_CELL_INFO_IND_V02, QMI_IDL_TYPE16(0, 46), 7},
  {QMI_LOC_SET_GEOFENCE_ENGINE_CONFIG_IND_V02, QMI_IDL_TYPE16(0, 40), 14},
  {QMI_LOC_GET_GEOFENCE_ENGINE_CONFIG_IND_V02, QMI_IDL_TYPE16(0, 42), 39},
  {QMI_LOC_GET_BATCH_SIZE_IND_V02, QMI_IDL_TYPE16(0, 48), 21},
  {QMI_LOC_START_BATCHING_IND_V02, QMI_IDL_TYPE16(0, 50), 14},
  {QMI_LOC_EVENT_BATCH_FULL_NOTIFICATION_IND_V02, QMI_IDL_TYPE16(0, 51), 28},
  {QMI_LOC_READ_FROM_BATCH_IND_V02, QMI_IDL_TYPE16(0, 53), 460},
  {QMI_LOC_STOP_BATCHING_IND_V02, QMI_IDL_TYPE16(0, 55), 21},
  {QMI_LOC_RELEASE_BATCH_IND_V02, QMI_IDL_TYPE16(0, 57), 14},
  {QMI_LOC_GET_AVAILABLE_WWAN_POSITION_IND_V02, QMI_IDL_TYPE16(0, 59), 145},
  {QMI_LOC_SET_PREMIUM_SERVICES_CONFIG_IND_V02, QMI_IDL_TYPE16(0, 61), 7},
  {QMI_LOC_SET_XTRA_VERSION_CHECK_IND_V02, QMI_IDL_TYPE16(0, 63), 7},
  {QMI_LOC_SET_GNSS_CONSTELL_REPORT_CONFIG_IND_V02, QMI_IDL_TYPE16(0, 65), 7},
  {QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_IND_V02, QMI_IDL_TYPE16(0, 67), 7},
  {QMI_LOC_START_DBT_IND_V02, QMI_IDL_TYPE16(0, 69), 11},
  {QMI_LOC_EVENT_DBT_POSITION_REPORT_IND_V02, QMI_IDL_TYPE16(0, 72), 2074},
  {QMI_LOC_EVENT_DBT_SESSION_STATUS_IND_V02, QMI_IDL_TYPE16(0, 73), 11},
  {QMI_LOC_STOP_DBT_IND_V02, QMI_IDL_TYPE16(0, 71), 11},
  {QMI_LOC_EVENT_BATCHING_STATUS_IND_V02, QMI_IDL_TYPE16(0, 74), 7},
  {QMI_LOC_QUERY_AON_CONFIG_IND_V02, QMI_IDL_TYPE16(0, 76), 21},
  {QMI_LOC_DELETE_GNSS_SERVICE_DATA_IND_V02, QMI_IDL_TYPE16(0, 78), 7},
  {QMI_LOC_INJECT_XTRA_DATA_IND_V02, QMI_IDL_TYPE16(0, 80), 23},
  {QMI_LOC_INJECT_XTRA_PCID_IND_V02, QMI_IDL_TYPE16(0, 82), 7},
  {QMI_LOC_GET_SUPPORTED_FEATURE_IND_V02, QMI_IDL_TYPE16(0, 84), 111},
  {QMI_LOC_QUERY_XTRA_INFO_IND_V02, QMI_IDL_TYPE16(0, 86), 1569},
  {QMI_LOC_START_OUTDOOR_TRIP_BATCHING_IND_V02, QMI_IDL_TYPE16(0, 88), 7},
  {QMI_LOC_QUERY_OTB_ACCUMULATED_DISTANCE_IND_V02, QMI_IDL_TYPE16(0, 90), 21},
  {QMI_LOC_EVENT_FDCL_SERVICE_REQ_IND_V02, QMI_IDL_TYPE16(0, 91), 692},
  {QMI_LOC_GET_FDCL_BS_LIST_IND_V02, QMI_IDL_TYPE16(0, 93), 4296},
  {QMI_LOC_INJECT_FDCL_DATA_IND_V02, QMI_IDL_TYPE16(0, 95), 272},
  {QMI_LOC_SYSTEM_INFO_IND_V02, QMI_IDL_TYPE16(0, 99), 35},
  {QMI_LOC_GET_CONSTELLATION_CONTROL_IND_V02, QMI_IDL_TYPE16(0, 100), 42},
  {QMI_LOC_ENABLE_POSITION_INJECTION_TEST_MODE_IND_V02, QMI_IDL_TYPE16(0, 102), 14},
  {QMI_LOC_INJECT_TEST_POSITION_IND_V02, QMI_IDL_TYPE16(0, 104), 7},
  {QMI_LOC_EVENT_BS_OBS_DATA_SERVICE_REQ_IND_V02, QMI_IDL_TYPE16(0, 105), 5},
  {QMI_LOC_GET_BS_OBS_DATA_IND_V02, QMI_IDL_TYPE16(0, 107), 775},
  {QMI_LOC_EVENT_REPORT_IND_V02, QMI_IDL_TYPE16(0, 108), 11},
  {QMI_LOC_EVENT_NMEA_IND_V02, QMI_IDL_TYPE16(0, 109), 4301},
  {QMI_LOC_GET_LOCATION_BACKOFF_PERIOD_IND_V02, QMI_IDL_TYPE16(0, 111), 28},
  {QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_IND_V02, QMI_IDL_TYPE16(0, 113), 22},
  {QMI_LOC_EVENT_GNSS_SV_INFO_IND_V02, QMI_IDL_TYPE16(0, 114), 3768},
  {QMI_LOC_GET_WWAN_DATA_IND_V02, QMI_IDL_TYPE16(0, 116), 4016}
};

/*Service Object*/
struct qmi_idl_service_object loc_iot_qmi_idl_service_object_v02 = {
  0x06,
  0x02,
  0x50,
  4467,
  { sizeof(loc_iot_service_command_messages_v02)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(loc_iot_service_response_messages_v02)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(loc_iot_service_indication_messages_v02)/sizeof(qmi_idl_service_message_table_entry) },
  { loc_iot_service_command_messages_v02, loc_iot_service_response_messages_v02, loc_iot_service_indication_messages_v02},
  &loc_iot_qmi_idl_type_table_object_v02,
  0x08,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type loc_iot_get_service_object_internal_v02
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( LOC_IOT_V02_IDL_MAJOR_VERS != idl_maj_version || LOC_IOT_V02_IDL_MINOR_VERS != idl_min_version
       || LOC_IOT_V02_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&loc_iot_qmi_idl_service_object_v02;
}


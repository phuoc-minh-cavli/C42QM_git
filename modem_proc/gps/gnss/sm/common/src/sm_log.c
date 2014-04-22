/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   LOCATION SERVICES MANAGER LOGGING MODULE

GENERAL DESCRIPTION
  This file contains LSM log record definitions

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.
  Copyright (c) 2013 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/common/src/sm_log.c#3 $ $DateTime: 2022/01/17 05:46:45 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/24/20   sai     Change to calculate extended lat long in 1482 log packet properly
10/30/17   nath    Updated data types of alongAxisUnc and perpAxisUnc to float
03/30/17   sum     Replaced velHor with velHorU 
11/04/16   muk     Enable QZSS meas and Poly data
11/30/15   gk      Lppe changes 
11/30/15   mj       Assign struct instead of copying individual fields in function sm_log_eph_download()
09/13/15   muk     OEMDRE Log V2 extension
04/17/15   jv      Added QZSS Support
08/27/14   jv      16-bit SVID & IODE Support
06/07/13   rh      Removed NO_FIX report handling (now BestAvailPos)
08/28/12   rk      CR 392443 - Avoid mem leak in the event of unsupported PDSM PA event logging.
                   Also fixed bitmask to enum conversion for successful PDSM PA event logging.
06/11/12   rk      Enhance sm_log_pa_event_callback with new PA event enum based assignment.
04/23/12   ssu     Adding sm_log_get_best_avail_pos_info, sm_log_report_ext_status_best_avail_pos_info
03/12/12   rh      Added version 2 for CPI log
01/29/10   pn      Updated DmLog structs with PACKED_POST.
09/21/09   rb      added log for Eph download
7/27/09    gk      add a reason in update failure. fixed ipv6 address
07/25/09   atien   sm_log_diag_position() - initialize position source before use
07/25/09   atien   PDSM_LITE_PD_EVENT_UPDATE_FAILURE - report event payload with failure reason
5/14/09    gk      fixed log version for the gps state
04/02/09   ns      support for Qwip logs.
04/01/09   atien   Fix critical lint error
02/26/09   atien   Make sure to clear XTRA-T log fields before use
02/04/09   atien   MOLR vs AD call flow support
01/16/09   atien   XTRA-T support
07/29/08   gk      On-demand log
07/28/08   gk      Fix Klocwork errors
06/05/08   lt      Add PD_EVENTs(14-19) that's been in design doc but not here.
07/25/08   gk      fix a log which incorrectly defined
06/12/08   lt     Support for external coarse position injection.
06/09/08   gk      On-Demand Changes to add some logs and events
05/20/08   gk      On-demand changes
09/14/07   gk      NMEA CONFIG set parameters support
7/13/07    gk      umts noticafication not logged correctly. 
5/8/07     gk      position source not initialzed properly. 
4/25/07    gk      modified a pa log to match the new  structure
4/16/07    gk      renumbered delete params 
2/14/07    gk      changed UMTS/Supl notification flags to uint16 
2/06/07    gk      Fixed notification parameters logs, lint errors, warnings 
1/11/07    gk      Fixed set parameters logs, ip address was incorrect in get pos
12/06/06   gk      Check null pointers
12/04/06   gk      Updated position report log to reflect the spec, minor mods
11/8/06    gk      Initial checkin for converged GPS

===========================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"
#include "sm_log.h"
#include "pdapi_lite.h"
#include "pdapiclient_lite.h"
#include "sm_api.h"
#include "gnss_common.h"
#include "aries_os_api.h"
#include "sm_util.h"
#include "tm_common.h"
#include "tm_data.h"
#include "math.h"

#define SM_MEMCPY(dst,src) memscpy((void*)&(dst), sizeof(dst), (void*)&(src), sizeof(src))
#define SM_MEMCLR(a)   memset((void*)&(a), 0, sizeof(a))
#define TM_CORE_MIN_VELOCTY            0.15 /* 0.15 m/s */


#define CGPS_SM_LOG_PKT_VERSION_1 1
#define CGPS_SM_LOG_PKT_VERSION_2 2
#define CGPS_SM_LOG_PKT_VERSION_3 3
#define CGPS_SM_LOG_PKT_VERSION_4 4
#define CGPS_SM_LOG_PKT_VERSION_5 5
#define CGPS_SM_LOG_PKT_VERSION_6 6
#define CGPS_SM_LOG_PKT_VERSION_7 7
#define CGPS_SM_LOG_PKT_VERSION_8 8

#ifdef FEATURE_CIRC_BUF_F3
#define SM_LOG_CIRC_BUF_MAX_META_FILE_NAME_LEN         20
#define SM_LOG_CIRC_BUF_TEMP_FILE_NAME_LEN_MAX         64
#define SM_LOG_CIRC_BUF_F3_LEN_MAX                     100
#define SM_LOG_CIRC_BUF_ASCII_VALUE_ZERO               48
#define SM_LOG_CIRC_BUF_FILE_NAME_LEN_NO_OF_BITS       2
#define SM_LOG_CIRC_BUF_VALUE_TEN                      10
#endif/*#ifdef FEATURE_CIRC_BUF_F3*/

#if defined(FEATURE_CIRC_BUF_IPC) || defined(FEATURE_CIRC_BUF_F3)
#define SM_LOG_CIRC_BUF_TIME_STAMP_STR_LEN             13
#define SM_LOG_CIRC_BUF_CONVERT_TIME_SEC_TO_MSEC       1000
#define SM_LOG_CIRC_BUF_CONVERT_TIME_SEC_TO_MIN        60
#define SM_LOG_CIRC_BUF_CONVERT_TIME_HOURS_TO_DAY      24
#endif/*#if defined(FEATURE_CIRC_BUF_IPC) || defined(FEATURE_CIRC_BUF_F3)*/

#ifdef FEATURE_CIRC_BUF_F3
sm_log_circ_buf_f3    sm_log_F3_buf;
#endif/*#ifdef FEATURE_CIRC_BUF_F3*/

#ifdef FEATURE_CIRC_BUF_IPC
sm_log_circ_buf_ipc    sm_log_IPC_buf;
#endif/*#ifdef FEATURE_CIRC_BUF_IPC*/

LOG_RECORD_DEFINE(LOG_CGPS_REP_EVT_LOG_PACKET_C)
uint8         u_EventId;   /* ID for the LSM specific event */
uint8         u_EventData; /* Data associated with the event ID */
uint8         u_PacketVersion; /* Reserved for future use */
uint8         u_Reserved2; /* Reserved for future use */
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_GET_POSITION_C)
uint32 q_ClientId;
uint16 w_ServiceType;
uint8 u_SessionType;
uint8 u_OperationMode;
uint8 u_ServerOption;
uint8 u_DatadownloadMode;
uint16 w_DatadownloadDuration;
uint32 q_ServerAddress;
uint32 q_PortId;
uint32 q_NumFixes;
uint32 q_TimebetFixes;
uint32 q_AccuracyThreshold;
uint8 u_PrivacyLevel;
uint8 u_GpsSessionTimeout;
uint8 u_Reserved[8];
uint8 u_PacketVersion;
uint8 u_ServerAddrType;
uint8 q_ServerAddress_v4v6[16];
uint32 q_ServerPort_v4v6;
uint32 q_ClassId;
uint8 u_UrlLength;
char  u_ServerURL[255];
uint8  u_Reserved2[16];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_ON_DEMAND_SESSION_START_C)
uint8  u_PktVersion;
uint8  u_GpsSessionTimeout;
uint32 q_Accuracy;
uint32 f_Punc;
uint32 f_Tunc;
uint8  u_OnDemandMode;
uint32 q_Reserved[4];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_ON_DEMAND_SESSION_STOP_C)
uint8  u_PktVersion;
uint8  u_OnDemandMode;
uint8  u_Reason;
uint32 q_NumFailedFixes;
uint32 q_Reserved[4];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_ON_DEMAND_SESSION_NOT_STARTED_C)
uint8  u_PktVersion;
uint8  u_OnDemandMode;
uint8  u_Reason;
uint32 q_NumFailedFixes;
uint32 f_Punc;
uint32 f_Tunc;
uint32 q_Reserved[4];
LOG_RECORD_END

#if 0
LOG_RECORD_DEFINE( LOG_CGPS_PDSM_EXTERN_COARSE_POS_INJ_START_C )
uint8  u_PktVersion;
uint32 q_ClientId;
uint32 q_opt_field_mask;
uint32 q_time_info_type;
uint32 q_timestamp_sec;
uint32 q_alt_def;
uint32 q_source;
int32  l_latitude;  /* Lat.: rad. LSB = pi/(2^32), 2's comp. */
int32  l_longitude; /* Long.: rad, LSB = pi /(2^31), 2's comp. */
uint32 l_loc_unc_hor;   /* horizontal location unc.: meters */
int16  x_altitude; /* alt: meters, positive height, negative depth  */
uint16 w_loc_unc_vert;   /* loc unc vertical: meters */
uint8  u_confidence_hor;  /* horizontal confidence */
uint8  u_confidence_vert; /* vertical confidence */
uint32 q_Reserved[4];
LOG_RECORD_END
#endif

LOG_RECORD_DEFINE(LOG_GNSS_PDSM_SET_PARAMETERS_C)
uint8  u_PacketVersion;
uint32 q_ClientId;
uint8 u_ParameterType;
uint8 u_Length;
//sm_log_pdsm_lite_set_parameters_value  z_ParameterValue;
uint8 u_ParameterValue[255];
uint8  u_Reserved[3];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_END_SESSION_C)
uint32 q_ClientId;
uint8 u_SessionType;
uint8 u_Reserved[3];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GNSS_PDSM_POSITION_REPORT_CALLBACK_C)
uint8  u_PktVersion;
uint32 q_ClientId;
int32 l_Latitude;
int32 l_Longitude;
uint32 q_TimeStamp;
uint8 u_LocUncAng;
uint8 u_LocUncA;
uint8 u_LocUncP;
uint8 u_OptFieldMask;
int16 x_Altitude;
uint16 w_Heading;
uint16 w_VelocityHor;
uint8 u_FixType;
int8 s_VelocityVer;
uint8 u_LocUncertaintyV;
uint8 u_PositionMethod;
uint8 u_PositionSource;
uint8 u_EndSessionCause;
uint8 u_UncEllConfidence;
int8 s_utcOffset;
int32 q_ExtLoclat;
int32 q_ExtLoclon;
uint16 w_ExtLocUncA;
uint16 w_ExtLocUncP;
int32 q_VelHoriz;
int32 q_VelVert;
uint32 q_TimeStampTowMs;
uint16 w_TimeStampGpsWeek;
uint8 u_PosReportedToNetwork;
uint32 w_VelUncHoriz;
uint32 w_VelUncVert;

uint32        q_Hdop;       /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
uint32        q_Pdop;       /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
uint32        q_Vdop;       /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
uint32        q_Tdop;       /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
uint32        q_Gdop;       /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
uint8      u_UtcHour;       /**< Range [0..23] hours */
uint8      u_UtcMin;        /**< Range [0..59] minutes */
uint16     w_UtcSec;        /**< Count in units of 10 msec. Range [0..5999] */
uint32     q_UtcDate;       /* UTC Date ddmmyyyy 
                               dd  in range of [1..31]
                               mm in range of [1..12]
                               yyyy in range of [0..9999]   
                               */
uint32  q_LocUncHorizontal;    /*  The HEPE value in meters */
uint8  u_TimeMask;             /**< BIT[0] = GPS - if set to 1, indicates GPS SVs were used 
                                                   in obtaining the time stamp.
                                    BIT[1] = GLO - if set to 1, indicates GLO SVs were used 
                                                   in obtaining the time stamp.
                                    BIT[2] = GLO Offset Valid - if set to 1, indicates that 
                                                   the Glonass-GPS time offset field is valid.
                                    BIT[3] = BDS - if set to 1, indicates BDS SVs were used 
                                                   in obtaining the time stamp.
                                    BIT[4] = GAL - if set to 1, indicates GAL SVs were used 
                                                   in obtaining the time stamp.
                               */
uint32 q_GloOffset;            /**< milliseconds offset between gps and glonass time. */
uint32 q_Reserved[2];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GNSS_PDSM_PA_EVENT_CALLBACK_C) 
uint8 u_PacketVersion;
uint32 q_ClientId;
uint8 u_EventType;
uint8 u_Length;
uint8 u_Reserved;
uint8 u_ParameterValue[255];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_PD_CMD_ERR_CALLBACK_C)
uint32 q_ClientId;
uint8 u_CmdType;
pdsm_lite_pd_cmd_err_e_type z_CmdErrType;
uint8 u_PacketVersion;
uint8 u_Reserved[2];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_PA_CMD_ERR_CALLBACK_C)
uint32 q_ClientId;
uint8 u_CmdType;
pdsm_lite_pa_cmd_err_e_type z_CmdErrType;
uint8 u_PacketVersion;
uint8 u_Reserved[2];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_EXT_STATUS_POS_REPORT_C)
  uint8 u_PacketVersion;                     /*!< Packet version */
  uint32 q_ClientId;                         /*!< Client Id */
  int32 l_Latitude;                          /*!< Location latitude */
  int32 l_Longitude;                         /*!< Location longitude */
  uint32 q_TimeStamp;                        /*!< Time stamp for the position fix (seconds). */
  uint8 u_LocUncAng;                         /*!< Location uncertainty angle. */
  uint8 u_LocUncA;                           /*!< Location uncertainty along. */
  uint8 u_LocUncP;                           /*!< Location uncertainty perp. */
  uint8 u_OptFieldMask;                      /*!< Optional fields are validity. */
  int16 x_Altitude;                          /*!< Altitiude valid if u_OptFieldMask has PDSM_LITE_PD_ALTITUDE_VALID bit set */
  uint16 w_Heading;                          /*!< Heading valid if u_OptFieldMask has PDSM_LITE_PD_HEADING_VALID bit set */
  uint16 w_VelocityHor;                      /*!< Horizontal velocity valid if u_OptFieldMask has PDSM_LITE_PD_VELOCITY_VALID bit set */
  uint8 u_FixType;                           /*!< Fix type, TRUE if 3D */
  int8 s_VelocityVer;                        /*!< Vertical velocity valid if u_OptFieldMask has PDSM_LITE_PD_VELOCITY_VALID bit set */
  uint8 u_LocUncertaintyV;                   /*!< loc uncertainity vertical valid if u_OptFieldMask has PDSM_LITE_PD_ALTITUDE_VALID bit set */
  uint8 u_PositionMethod;                    /*!< Position method */
  uint8 u_PositionSource;                    /*!< Position source (new - 1, old - 0) */
  uint8 u_EndSessionCause;                   /*!< End session cause */
  uint8 u_UncEllConfidence;                  /*!< confidence of the loc unc. valid if u_OptFieldMask has PDSM_LITE_PD_UNCERTAINTY_VALID bit set */
  int8 s_utcOffset;                          /*!< Time utc offset valid if u_OptFieldMask has PDSM_LITE_PD_UTCOFFSET_VALID bit set */
  int32 q_ExtLoclat;                         /*!< Latitude: LSB = Pi/2^32, Range -Pi/2 to Pi/2. Valid PDSM_LITE_PD_LAT_LONG_VALID bit set */
  int32 q_ExtLoclon;                         /*!<  Longitude: LSB = Pi/2^31, Range -Pi to Pi. Valid PDSM_LITE_PD_LAT_LONG_VALID bit set */
  uint16 w_ExtLocUncA;                       /*!< Along axis Unc in meters. Max value is 65535. If this value is
                                                  greater than 65535, then the u_OptFieldMask for
                                                  PDSM_LITE_PD_UNCERTATINITY_VALID is not set. f_loc_unc_horizontal value
                                                  is used to get the circular UNC */
  uint16 w_ExtLocUncP;                       /*!< Perp axis Unc in meters. Max value is 65535. If this value is
                                                  greater than 65535, then the u_OptFieldMask for
                                                  PDSM_LITE_PD_UNCERTATINITY_VALID is not set. f_loc_unc_horizontal value
                                                  is used to get the circular UNC   */
  int32 q_VelHoriz;                          /*!< Used for publishing hori velocity more than 0x7FFF. Valid if
                                                  u_OptFieldMask PDSM_LITE_PD_EXT_VELOCITY_VALID is set */
  int32 q_VelVert;                           /*!< Vertical Velocity: cm/sec., positive for upward. Valid if u_OptFieldMask
                                                  PDSM_LITE_PD_VELOCITY_VALID is set */
  uint32 q_TimeStampTowMs;                   /*!< gnss time stamp, number of ms this week (TOW MilliSeconds). Valid if u_OptFieldMask
                                                  PDSM_LITE_PD_GPS_TIME_VALID is set */
  uint16 w_TimeStampGpsWeek;                 /*!< Gnss time stamp, number of weeks since GPS Week. Valid if u_OptFieldMask
                                                  PDSM_LITE_PD_GPS_TIME_VALID is set */
  uint8 u_PosReportedToNetwork;              /*!< flag indicating if this position was reported to the network */
  uint32 w_VelUncHoriz;                      /*!< Velocity Uncertainity horizontal: cm/sec. Valid if u_OptFieldMask
                                                  PDSM_LITE_PD_VELOCITY_VALID is set */
  uint32 w_VelUncVert;                       /*!< Velocity Uncertainity vertical: cm/sec. Valid if u_OptFieldMask
                                                  PDSM_LITE_PD_VELOCITY_VALID is set */
  uint32 q_FixTimeRemainingMSec;             /* Time remaining for the Fix to Timeout */
  uint8  u_PosFromOem;                       /*!< 1 if the position is received from the OEM,
                                                  0 if the position is from position engine. */
  uint32        q_Hdop;                      /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  uint32        q_Pdop;                      /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  uint32        q_Vdop;                      /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */  
  uint32        q_Tdop;                      /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  uint32        q_Gdop;                      /* [x10]   Range: 1.0 (highest accuracy) 50.0 (lowest accuracy) */
  uint8      u_UtcHour;                      /*!< UTC hours in range of  00..23 */
  uint8      u_UtcMin;                       /*!< UTC minutes in range of 00..59  */
  uint16     w_UtcSec;                       /*!< UTC seconds ssss [x100 sec]  in range of  0..5999 */
  uint32     q_UtcDate;                      /* UTC Date ddmmyyyy 
                                                dd  in range of [1..31]
                                                mm in range of [1..12]
                                                yyyy in range of [0..9999]   
                                             */
  uint32  q_LocUncHorizontal;               /*  The HEPE value in meters */
  uint16  w_XtraStartGpsWeek;               /*  Current XTRA info is good starting this GPS week */
  uint16  w_XtraStartGpsMinutes;            /*  Current XTRA information is good starting this many minutes into the week */
  uint16  w_XtravalidDurationHours;         /*  XTRA info is valid for this many hours */

  uint32 q_SvIdGps;                         /* sv id of Gps satellites used in the fix */
  uint32 q_SvIdGlo;                         /* sv id of Glo satellites used in the fix */
  uint64 t_SvIdBds;                         /* sv id of Bds satellites used in the fix */
  uint64 t_SvIdGal;                         /* sv id of Gal satellites used in the fix */
  uint8  u_SvIdQzss;                        /* sv id of QZSS satellites used in the fix */
  uint32 q_PositionSourceEx;                /*!< Position source ex */
  uint8  u_Reserved[8];                     /*!< Reserved */
LOG_RECORD_END
//  TODO:  [an][del] found only in log_codes.h

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  uint32  u_Reserved[4];
} LOG_CGPS_PDSM_GPS_STATE_INFO_C_type;

#define LOG_PDSM_EXT_NMEA_RPT_DATA_LEN (200)

LOG_RECORD_DEFINE(LOG_CGPS_PDSM_EXT_STATUS_NMEA_REPORT_C)
uint8 u_PacketVersion;
uint32 q_ClientId;
uint32 q_NmeaSentenceType;
uint8 u_NmeaSentenceLength;
uint8 u_NmeaData[LOG_PDSM_EXT_NMEA_RPT_DATA_LEN];
LOG_RECORD_END

typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  int32 q_Lat;  /* Latitude: LSB = Pi/2^32, Range -Pi/2 to Pi/2 */
  int32 q_Lon;  /* Longitude: LSB = Pi/2^31, Range -Pi to Pi */
  int16 q_Altitude; /* Altitude in meters */
  uint16 w_LocUncVertical; /* vertical unc in meters  */
  uint32 q_Punc; /* Position Unc in meters */
  uint32 q_TimestampTowMs; /* Timestamp in GPS TOW milliseconds */
  uint16 w_TimestampGpsWeek;  /*GPS week number */
  uint8  b_EngineState; /*  GPS engine state on/off */
  uint32 q_TuncMs; /* Time uncertainty in milli seconds */
  uint32 q_EphSvMask; /* SV Mask for the ephemeris */
  uint32 q_AlmSvMask; /* SV Mask for the Almanac */
  uint32 q_IonoValid;  /*  IONO validity */
  uint32 q_HealthMask; /* SV Mask for the Health */
  uint32 q_VisibleSvMask; /* SV Mask of visible SVs */
  uint16 w_XtraStartGpsWeek; /*  Current XTRA info is good starting this GPS week */
  uint16 w_XtraStartGpsMinutes; /*  Current XTRA information is good starting this many minutes into the week */
  uint16  w_XtraValidDurationHours; /*  XTRA info is valid for this many hours */
  uint32  q_Reserved[4];
} LOG_CGPS_PDSM_EXT_STATUS_GPS_STATE_INFO_C_type;


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  uint32  q_Reserved[4];
} LOG_GNSS_PDSM_BEST_AVAIL_POS_INFO_C_type;


typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8 u_PacketVersion;
  uint32 q_ClientId;
  uint32 sessionStatus;
  /**<   Session status.
  
        Valid values: \n
          - 0x00000000 -- SESS_STATUS_SUCCESS \n
          - 0x00000001 -- SESS_STATUS_IN_PROGRESS \n
          - 0x00000002 -- SESS_STATUS_GENERAL_FAILURE \n
          - 0x00000003 -- SESS_STATUS_TIMEOUT \n
          - 0x00000004 -- SESS_STATUS_USER_END \n
          - 0x00000005 -- SESS_STATUS_BAD_PARAMETER \n
          - 0x00000006 -- SESS_STATUS_PHONE_OFFLINE \n
          - 0x00000007 -- SESS_STATUS_ENGINE_LOCKED
       */
  uint8 sessionId;
  /**<    ID of the session that was specified in the Start request 
        LOC_ENG_START_REQ. \n
        - Type: Unsigned integer \n
        - Range: 0 to 255  */
  /*****  The position report information  which is reported to the QMI client ****/
  /*  Latitude */
  uint8 latitude_valid;  /**< Must be set to true if latitude is being passed */
  double latitude;
  /**<   Latitude (specified in WGS84 datum). \n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: -90.0 to 90.0 \n
       - Positive values indicate northern latitude \n
       - Negative values indicate southern latitude  */

  /*   Longitude */
  uint8 longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).\n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: -180.0 to 180.0 \n
       - Positive values indicate eastern longitude \n
       - Negative values indicate western longitude  */

  /*   Circular Horizontal Position Uncertainty */
  uint8 horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Type: Floating point \n
       - Units: Meters  */

  /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
  uint8 horUncEllipseSemiMinor_valid;  /**< Must be set to true if horUncEllipseSemiMinor is being passed */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty.\n
       - Type: Floating point \n
       - Units: Meters  */

  /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
  uint8 horUncEllipseSemiMajor_valid;  /**< Must be set to true if horUncEllipseSemiMajor is being passed */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty.\n
       - Type: Floating point \n
       - Units: Meters  */

  /*  Elliptical Horizontal Uncertainty Azimuth */
  uint8 horUncEllipseOrientAzimuth_valid;  /**< Must be set to true if horUncEllipseOrientAzimuth is being passed */
  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation.\n
       - Type: Floating point \n
       - Units: Decimal degrees \n
       - Range: 0 to 180  */

  /*  Horizontal Circular Confidence */
  uint8 horCircularConfidence_valid;  /**< Must be set to true if horCircularConfidence is being passed */
  uint8 horCircularConfidence;
  /**<   Horizontal circular uncertainty confidence.\n
       - Type: Unsigned integer \n 
       - Units: Percent \n
       - Range: 0 to 99  */

  /*  Horizontal Reliability */
  uint8 horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  uint32 horReliability;
  /**<   Specifies the reliability of the horizontal position.

       Valid values: \n
         - 0x00000000 -- RELIABILITY_NOT_SET \n
         - 0x00000001 -- RELIABILITY_VERY_LOW \n
         - 0x00000002 -- RELIABILITY_LOW \n
         - 0x00000003 -- RELIABILITY_MEDIUM \n
         - 0x00000004 -- RELIABILITY_HIGH
    */

  /*  Horizontal Speed */
  uint8 horSpeed_valid;  /**< Must be set to true if horSpeed is being passed */
  float horSpeed;
  /**<   Horizontal speed.\n
       - Type: Floating point \n 
       - Units: Meters/second  */

  /*  Horizontal Speed Uncertainty */
  uint8 horSpeedUnc_valid;  /**< Must be set to true if horSpeedUnc is being passed */
  float horSpeedUnc;
  /**<   2D (horizontal) Speed uncertainty.\n
       - Type: Floating point \n 
       - Units: Meters/second  */

  /*  Altitude With Respect to Ellipsoid */
  uint8 altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Type: Floating point \n 
       - Units: Meters \n
       - Range: -500 to 15883  */

  /*  Altitude With Respect to Sea Level */
  uint8 altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level.\n
       - Type: Floating point \n 
       - Units: Meters  */

  /*  Vertical Uncertainty */
  uint8 vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Type: Floating point \n 
       - Units: Meters  */

  /*  Vertical Confidence */
  uint8 vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8 vertConfidence;
  /**<   Vertical uncertainty confidence.\n
       - Type: Unsigned integer \n 
       - Units: Percent \n
       - Range: 0 to 99  */

  /*  Vertical Reliability */
  uint8 vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  uint32 vertReliability;
  /**<   Specifies the reliability of the vertical position.

        Valid values: \n
          - 0x00000000 -- RELIABILITY_NOT_SET \n
          - 0x00000001 -- RELIABILITY_VERY_LOW \n
          - 0x00000002 -- RELIABILITY_LOW \n
          - 0x00000003 -- RELIABILITY_MEDIUM \n
          - 0x00000004 -- RELIABILITY_HIGH  */

  /*  Vertical Speed */
  uint8 vertSpeed_valid;  /**< Must be set to true if vertSpeed is being passed */
  float vertSpeed;
  /**<   Vertical speed.\n
         - Type: Floating point \n
         - Units: Meters/second  */

  /*  Vertical Speed Uncertainty */
  uint8 vertSpeedUnc_valid;  /**< Must be set to true if vertSpeedUnc is being passed */
  float vertSpeedUnc;
  /**<   1-D (vertical) speed uncertainty.\n
       - Type: Floating point \n 
       - Units: Meters/second  */

  /*  Heading */
  uint8 heading_valid;  /**< Must be set to true if heading is being passed */
  float heading;
  /**<   Heading.\n
         - Type: Floating point \n
         - Units: Degrees \n
         - Range: 0 to 359.999   */

  /*  Heading Uncertainty */
  uint8 headingUnc_valid;  /**< Must be set to true if headingUnc is being passed */
  float headingUnc;
  /**<   Heading uncertainty.\n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: 0 to 359.999  */

  /*  Magnetic Deviation */
  uint8 magneticDeviation_valid;  /**< Must be set to true if magneticDeviation is being passed */
  float magneticDeviation;
  /**<   Difference between the bearing to true north and the bearing shown 
      on a magnetic compass. The deviation is positive when the magnetic
      north is east of true north. \n
      - Type: Floating point  */

  /*  Technology Used */
  uint8 technologyMask_valid;  /**< Must be set to true if technologyMask is being passed */
  uint32 technologyMask;
  /**<   Technology used in computing this fix.

       Valid bitmasks: \n
         - 0x00000001 -- SATELLITE \n
         - 0x00000002 -- CELLID \n
         - 0x00000004 -- WIFI \n
         - 0x00000008 -- SENSORS \n 
         - 0x00000010 -- REFERENCE_LOCATION  */

  /*  Dilution of Precision */
  uint8 PDOP_valid;  /**< Must be set to true if PDOP is being passed */
  float PDOP;
  /**<   Position dilution of precision.\n
       - Type: Floating point \n
       - Range: 1 (highest accuracy) to 50 (lowest accuracy)\n
       - PDOP = square root of (HDOP^2 + VDOP^2)        */

  uint8 HDOP_valid;  /**< Must be set to true if HDOP is being passed */
  float HDOP;
  /**<   Horizontal dilution of precision.\n
       - Type: Floating point \n  
       - Range: 1 (highest accuracy) to 50 (lowest accuracy)  */

  uint8 VDOP_valid;  /**< Must be set to true if VDOP is being passed */
  float VDOP;
  /**<   Vertical dilution of precision.\n
       - Type: Floating point. \n
       - Range: 1 (highest accuracy) to 50 (lowest accuracy)  */

  /*  UTC Timestamp */
  uint8 timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64 timestampUtc;
  /**<   UTC timestamp, including the leap seconds \n
       - Type: Unsigned integer \n 
       - Units: Milliseconds since Jan. 1, 1970  */

  uint8 leapSeconds_valid;
  uint8 leapSeconds;

  /*  Time Uncertainty */
  uint8 timeUnc_valid;  /**< Must be set to true if timeUnc is being passed */
  float timeUnc;
  /**<   Time uncertainty. \n
       - Type: Floating point \n
       - Units: Milliseconds   */

  /*  Time Source */
  uint8 timeSrc_valid;  /**< Must be set to true if timeSrc is being passed */
  uint32 timeSrc;
  /**<   Time source.
  
        Valid values: \n
          - 0x00000000 -- TIME_SRC_INVALID \n 
          - 0x00000001 -- TIME_SRC_NETWORK_TIME_TRANSFER \n  
          - 0x00000002 -- TIME_SRC_NETWORK_TIME_TAGGING \n   
          - 0x00000003 -- TIME_SRC_EXTERNAL_ INPUT \n         
          - 0x00000004 -- TIME_SRC_TOW_DECODE \n             
          - 0x00000005 -- TIME_SRC_TOW_CONFIRMED \n          
          - 0x00000006 -- TIME_SRC_TOW_AND_WEEK_CONFIRMED \n 
          - 0x00000007 -- TIME_SRC_NAV_SOLUTION \n  
          - 0x00000008 -- TIME_SRC_SOLVE_FOR_TIME  */

  /*  Sensor Data Usage */
  uint8 sensorDataUsage_valid;  /**< Must be set to true if sensorDataUsage is being passed */
  uint32 sensorUsageMask;
  /**<   Specifies which sensors are used.
  
       Valid bitmasks are specified by the following constants: \n       
         - 0x00000001 -- SENSOR_USED_ACCEL \n
         - 0x00000002 -- SENSOR_USED_GYRO  */

  uint32 sesorAidingIndicatorMask;
  /**<   Specifies which results are aided by sensors.

       Valid bitmasks are specified by the following constants: \n
         - 0x00000001 -- AIDED_HEADING \n
         - 0x00000002 -- AIDED_SPEED \n
         - 0x00000004 -- AIDED_POSITION \n
         - 0x00000008 -- AIDED_VELOCITY  */

  /*  SV's Used to Calculate the Fix */
  uint8 gnssSvUsedList_valid;  /**< Must be set to true if gnssSvUsedList is being passed */
  uint32 gnssSvUsedList_len;  /**< Must be set to # of elements in gnssSvUsedList */
  uint16 gnssSvUsedList[LOC_ENG_MAX_SV_USED_LIST_LENGTH_V01];

  /*  Horizontal Elliptical Confidence */
  uint8 horEllipticalConfidence_valid;  /**< Must be set to true if horEllipticalConfidence is being passed */
  uint8 horEllipticalConfidence;
  /**<   Horizontal elliptical uncertainty confidence.\n
       - Type: Unsigned integer \n 
       - Units: Percent \n
       - Range: 0 to 99  */

  /*  GPS Time */
  uint8 gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  uint16 gpsWeek;
  /**<   Current GPS week as calculated from midnight, Jan. 6, 1980. \n
       - Type: Unsigned integer \n
       - Units: Weeks  */

  uint32 gpsTimeOfWeekMs;
  /**<   Amount of time into the current GPS week. \n
       - Type: Unsigned integer \n
       - Units: Milliseconds  */

  /**<   \n The number of weeks since Jan. 5, 1980, and
       milliseconds into the current week.  */


  /* End of position report information */
  uint32  q_Reserved[4];
} LOG_GNSS_PDSM_EXT_STATUS_BEST_AVAIL_POS_INFO_C_type;

LOG_RECORD_DEFINE(LOG_GNSS_SM_ERROR_C)
uint8    u_PacketVersion;
uint8    u_Severity;
uint32   q_Reserved[2];
uint8    u_Length;
uint8    u_MessageData[1];
LOG_RECORD_END

/* Measurement Report specific logs*/
//  TODO: [AN][DEL] all meas related logs not used are removed
/* DM Log 0x14E1 */ //  TODO : [an][del] not needed delete later
typedef PACKED struct PACKED_POST
{
  log_hdr_type xx_hdr;  /* Standard DM header */
  uint8   u_PacketVersion;
  uint16  w_Sv;           /* Sv ID */
  /* GPS: 1-32, GLO: 65-96, 0: Invalid All others are reserved */
  uint8   b_FreqNum;      /* Freq index, only valid if u_SysInd is GLO */
  uint8   u_SvPolyFlags;  /* Indicates the validity of data components, see those
                                  C_SV_POLY_... defintions above */
  uint16  w_Iode;         /* Ephemeris reference time
                         GPS:Issue of Data Ephemeris used [unitless].
                         GLO: Tb 7-bit, refer to ICD02 
                         GAL: 10-bit iode, NOT SUPPORTED! */
  uint64  d_T0;           /* Reference time for polynominal calculations
                         GPS: Secs in week.
                         GLO: Full secs since Jan/01/96 */
  uint64  pd_xyz0[3];     /* C0X, C0Y, C0Z */
  uint64  pd_xyzN[9];     /* C1X, C2X ... C2Z, C3Z */
  uint32  pf_other[4];    /* C0T, C1T, C2T, C3T */
  uint32  f_PosUnc;       /* SV position uncertainty [m]. */
  uint32  f_IonoDelay;    /* Ionospheric delay at d_T0 [m]. */
  uint32  f_IonoDot;      /* Iono delay rate [m/s].  */
  uint32  f_sbasIonoDelay; /* SBAS Ionospheric delay at d_T0 [m]. */
  uint32  f_sbasIonoDot;  /* SBAS Iono delay rate [m/s].  */
  uint32  f_TropoDelay;   /* Tropospheric delay [m]. */
  uint32  f_Elevation;    /* Elevation [rad] at d_T0 */
  uint32  f_ElevDot;      /* Elevation rate [rad/s] */
  uint32  f_ElevUnc;      /* SV elevation [rad] uncertainty */
  uint64  d_VelCoef[12];  /* Coefficients of velocity poly */
} LOG_GNSS_OEMDRE_SVPOLY_REPORT_C_type; //gnss_SvPoly;

extern int16 tm_util_select_pos_unc(float Std);
extern void tm_util_get_timestamp_from_gps_time(sm_FixInfoStructType    *input_lr_buf_ptr, me_Clock         * z_Clock,unsigned long * timeStamp );
extern tm_core_param_s_type     tm_core_info;
extern boolean tm_xtra_set_xtra_time_and_validity
(
pdsm_lite_xtra_download_info_s_type *p_download_info 
);

/*
 ******************************************************************************
 * sm_log_sm_event
 *
 * Function description:
 *
 * This function sends a DM log message with LSM specific event information.  
 * This is sent out asynchronously when the event occurs.
 *
 * If there is no memory space available or the logging for this
 * DM message has not been enabled, then the log_alloc will return a NULL ptr.
 *
 *
 * Parameters: 
 *
 *  p_EvtInfo - pointer to structure containing the event information
 *
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_sm_event(sm_report_evt_type *p_EvtInfo)
{
  LOG_CGPS_REP_EVT_LOG_PACKET_C_type *p_LogPkt;

  if (p_EvtInfo == NULL)
  {
    return;
  }

  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_REP_EVT_LOG_PACKET_C_type *)log_alloc(
     LOG_CGPS_REP_EVT_LOG_PACKET_C,
     sizeof(LOG_CGPS_REP_EVT_LOG_PACKET_C_type));

  if (p_LogPkt != 0) /* sufficient space and log is enabled */
  {
    p_LogPkt->u_EventId   = p_EvtInfo->u_EventId;
    p_LogPkt->u_EventData = p_EvtInfo->u_EventData;
    p_LogPkt->u_PacketVersion = p_EvtInfo->u_PacketVersion;
    p_LogPkt->u_Reserved2 = p_EvtInfo->u_Reserved2;

    log_commit(p_LogPkt);

  }

}


/*
 ******************************************************************************
 * sm_log_event_report
 *
 * Function description:
 *
 * This function acts as the interface between the event report and the log
 * function which actually sends out the log for the event.  This function
 * assumes only the EventId and EventData is provided, and therefore sets the
 * Reserved bytes to 0.
 *
 *
 * Parameters: 
 *
 * u_EventId - enumerated value for the event which occurred
 *  u_EventData - data associated with the event which occurred
 *
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_event_report(uint8 u_EventId, uint8 u_EventData)
{

  sm_report_evt_type z_event;

  z_event.u_EventId   = u_EventId;
  z_event.u_EventData = u_EventData;
  z_event.u_PacketVersion = 0;
  z_event.u_Reserved2 = 0;

  sm_log_sm_event(&z_event);

}


/*
 ******************************************************************************
 * sm_log_get_position_request
 *
 * Function description:
 *  This function logs the parameters received from PDAPI when a pdsm_lite_get_position 
 *  was called.
 *
 *
 * Parameters: 
 *
 *  pdsm_lite_cmd_s_type - data associated with the event which occurred
 *
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_get_position_request(pdsm_lite_cmd_s_type *p_CmdType)
{
  LOG_CGPS_PDSM_GET_POSITION_C_type *p_LogPkt;
  uint8 i = 0;
  uint8 j = 0;

  if (p_CmdType == NULL)
  {
    return;
  }

  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_GET_POSITION_C_type *)log_alloc(
     LOG_CGPS_PDSM_GET_POSITION_C, sizeof(
                                          LOG_CGPS_PDSM_GET_POSITION_C_type));

  if (p_LogPkt != 0) /* sufficient space and log is enabled */
  {
    p_LogPkt->w_ServiceType = 0;
    p_LogPkt->u_SessionType =
       (uint8)p_CmdType->cmd.pd.info.option_type.session;
    p_LogPkt->u_OperationMode =
       (uint8)p_CmdType->cmd.pd.info.option_type.operation_mode;
    p_LogPkt->u_DatadownloadMode = 0;
    p_LogPkt->w_DatadownloadDuration = 0;
    p_LogPkt->q_NumFixes =
       p_CmdType->cmd.pd.info.option_type.fix_rate.num_fixes;
    p_LogPkt->q_TimebetFixes =
       p_CmdType->cmd.pd.info.option_type.fix_rate.time_between_fixes_ms;
    p_LogPkt->u_PrivacyLevel = 0;

    p_LogPkt->q_ServerAddress = 0;
    p_LogPkt->q_PortId = 0;
    p_LogPkt->u_GpsSessionTimeout =
       p_CmdType->cmd.pd.info.qos.gps_session_timeout;
    p_LogPkt->q_AccuracyThreshold =
       p_CmdType->cmd.pd.info.qos.accuracy_threshold;
    p_LogPkt->q_ClientId = (uint32)p_CmdType->cmd.pd.client_id;
    for (i = 0; i < 8; i++)
    {
      p_LogPkt->u_Reserved[i] = 0;
    }
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_1;
    for (i = 0; i < 16; i++)
    {
      p_LogPkt->q_ServerAddress_v4v6[i] = 0;
    }
    p_LogPkt->q_ServerPort_v4v6 = 0;
    p_LogPkt->u_UrlLength = 0;
    p_LogPkt->u_ServerURL[0] = '\0';
   
    p_LogPkt->q_ClassId = (uint32)p_CmdType->cmd.pd.info.option_type.class_id;
    for (i = 0; i < 16; i++)
    {
      p_LogPkt->u_Reserved2[i] = 0;
    }
    log_commit(p_LogPkt);

  }
}

/*
 ******************************************************************************
 * sm_log_set_parameters_request
 *
 * Function description:
 *  This function logs the parameters received from PDAPI when a pdsm_lite_set_parameters 
 *  was called.
 *
 *
 * Parameters: 
 *
 *  pdsm_lite_cmd_s_type - data associated with the event which occurred
 *
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_set_parameters_request(pdsm_lite_cmd_s_type *p_CmdType)
{
  LOG_GNSS_PDSM_SET_PARAMETERS_C_type *p_LogPkt;
  uint8 i, j;

  if (p_CmdType == NULL)
  {
    return;
  }
  p_LogPkt = (LOG_GNSS_PDSM_SET_PARAMETERS_C_type *)log_alloc(
     LOG_GNSS_PDSM_SET_PARAMETERS_C, sizeof(
                                            LOG_GNSS_PDSM_SET_PARAMETERS_C_type));


  if (p_LogPkt == NULL)
  {
    return;
  }
  p_LogPkt->u_PacketVersion =  CGPS_SM_LOG_PKT_VERSION_3;
  p_LogPkt->q_ClientId = (uint32)p_CmdType->cmd.pa.client_id;
  p_LogPkt->u_ParameterType = (uint8)p_CmdType->cmd.pa.info.set_type;

  p_LogPkt->u_Length = 4;
  switch (p_CmdType->cmd.pa.info.set_type)
  {
    case PDSM_LITE_PA_GPS_LOCK:           /* GPS LOCK */
      p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.gps_lock;
      break;

    case PDSM_LITE_PA_DELETE_PARAMS_EX1: /* intentional cascade */
       { /* create new stack */
        uint32 q_temp = 0; /* temporarily uint32 hold data to be pumped out */
        uint8 *pOutputByteArray = p_LogPkt->u_ParameterValue;
        p_LogPkt->u_Length = SM_LOG_LENGTH_PA_EVENT_DELETE_PARAMS;

        //lower 32-bits
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.pdsm_lite_delete_parms_flags & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //upper 32-bits
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.pdsm_lite_delete_parms_flags >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.time_unc;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.pos_unc;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.time_offset;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.pos_offset;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = (uint32)p_CmdType->cmd.pa.info.pa_set.delete_params.eph_sv_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = (uint32)p_CmdType->cmd.pa.info.pa_set.delete_params.alm_sv_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.glo_eph_sv_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.glo_alm_sv_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);


        //BDS EPH low
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.bds_eph_sv_mask & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //BDS EPH hi
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.bds_eph_sv_mask >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        //BDS ALM low
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.bds_alm_sv_mask & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //BDS ALM hi
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.bds_alm_sv_mask >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        //GAL EPH low
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.gal_eph_sv_mask & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //GAL EPH hi
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.gal_eph_sv_mask >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        //GAL ALM low
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.gal_alm_sv_mask & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //GAL ALM hi
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.gal_alm_sv_mask >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.qzss_eph_sv_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.qzss_alm_sv_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.gps_gnss_sv_blacklist_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.glo_gnss_sv_blacklist_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.bds_gnss_sv_blacklist_mask & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.bds_gnss_sv_blacklist_mask >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //GAL BLACKLIST low
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.gal_gnss_sv_blacklist_mask & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //GAL BLACKLIST hi
        q_temp = (uint32)(p_CmdType->cmd.pa.info.pa_set.delete_params.gal_gnss_sv_blacklist_mask >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.qzss_gnss_sv_blacklist_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
#if 0 //TODO: [TBD][APP][Whether Cell DB is required or not]
        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.delete_celldb_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
#endif 
        q_temp = p_CmdType->cmd.pa.info.pa_set.delete_params.delete_clock_info_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

      }
      break;

    case PDSM_LITE_PA_DPO_CONTROL: /* DPO Control */
      p_LogPkt->u_ParameterValue[0] = (uint8)p_CmdType->cmd.pa.info.pa_set.dpo_control;
      break;

    default:
      break;
  }

  p_LogPkt->u_Reserved[0] = 0;
  p_LogPkt->u_Reserved[1] = 0;
  p_LogPkt->u_Reserved[2] = 0;
  log_commit(p_LogPkt);

}
/*
 ******************************************************************************
 * sm_log_end_session
 *
 * Function description:
 *  This function logs the parameters received from PDAPI when a pdsm_lite_end_session 
 *  was called.
 *
 *
 * Parameters: 
 *
 *  session_type - end session type
 *
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_end_session(pdsm_lite_client_id_type          l_ClientId )
{
  LOG_CGPS_PDSM_END_SESSION_C_type *p_LogPkt;
  uint8 i = 0;

  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_END_SESSION_C_type *)log_alloc(
     LOG_CGPS_PDSM_END_SESSION_C,
     sizeof(LOG_CGPS_PDSM_END_SESSION_C_type));

  if (p_LogPkt != 0) /* sufficient space and log is enabled */
  {
    p_LogPkt->q_ClientId = (uint32)l_ClientId;
    p_LogPkt->u_SessionType = 0;

    for (i = 0; i < 3; i++)
    {
      p_LogPkt->u_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);
  }

}

/*
 ******************************************************************************
 * sm_log_notify_verify_response
 *
 * Function description:
 *  This function logs the parameters received from PDAPI when a 
 * pdsm_lite_notify_verify_response was called.
 *
 * Parameters: 
 *   pdapi_ver_resp_lcs_ptr: pointer to the lcs structure of the pdapi 
 *     command notif_verification_response
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
#if 0
void sm_log_notify_verify_response(pdsm_lite_lcs_cmd_s_type *p_LcsType)
{
  LOG_CGPS_PDSM_NOTIFY_VERIFY_RESP_C_type *p_LogPkt;
  uint16 w_DataPos;
  uint8 i = 0;

  if (p_LcsType == NULL)
  {
    return;
  }

  p_LogPkt = (LOG_CGPS_PDSM_NOTIFY_VERIFY_RESP_C_type *)log_alloc(
     LOG_CGPS_PDSM_NOTIFY_VERIFY_RESP_C,
     sizeof(LOG_CGPS_PDSM_NOTIFY_VERIFY_RESP_C_type));

  if (p_LogPkt == NULL)
  {
    return;
  }
  p_LogPkt->u_PktVersion = CGPS_SM_LOG_PKT_VERSION_3;
  p_LogPkt->q_ClientId = (uint32)p_LcsType->client_id;


  switch (p_LcsType->info.lcs_cmd_info.resp_info.lcs_info_cmd_type)
  {
    case PDSM_LITE_LCS_CMD_TYPE_VX_REQ:
      /* Try to allocate memory for the log packet */
      w_DataPos = sizeof(sm_log_1x_notify_verify_data);

      p_LogPkt->u_NotifyStatus = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.resp;
      p_LogPkt->u_NotifyType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info_cmd_type;
      p_LogPkt->q_NotifyLength = w_DataPos;
      p_LogPkt->z_NotifyData.oneX_data.u_NotificationPrivType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.notification_priv_type;
      p_LogPkt->z_NotifyData.oneX_data.u_PosTechInd = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.pos_tech;
      p_LogPkt->z_NotifyData.oneX_data.u_PosQoSPres = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.pos_qos_incl;
      p_LogPkt->z_NotifyData.oneX_data.u_PosQoS = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.pos_qos;
      p_LogPkt->z_NotifyData.oneX_data.q_NumFixes = (uint32)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.num_fixes;
      p_LogPkt->z_NotifyData.oneX_data.q_Tbf = (uint32)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.tbf;
      p_LogPkt->z_NotifyData.oneX_data.u_PosMode = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.pos_mode;
      p_LogPkt->z_NotifyData.oneX_data.u_DatacodingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.encoding_scheme;
      p_LogPkt->z_NotifyData.oneX_data.u_RequestorIdLen = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.requester_id.requester_id_length;
      if (p_LogPkt->z_NotifyData.oneX_data.u_RequestorIdLen != 0)
      {
        for (i = 0; i < p_LogPkt->z_NotifyData.oneX_data.u_RequestorIdLen; i++)
        {
          p_LogPkt->z_NotifyData.oneX_data.u_RequestorId[i] = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.requester_id.requester_id[i];
        }
      }
      p_LogPkt->z_NotifyData.oneX_data.w_UserRespTimer = (uint16)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.vx_req.user_resp_timer_val;
      break;
    case PDSM_LITE_LCS_CMD_TYPE_UMTS_CP_REQ:
      w_DataPos = sizeof(sm_log_umts_notify_verify_data);
      p_LogPkt->u_NotifyStatus = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.resp;
      p_LogPkt->u_NotifyType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info_cmd_type;
      p_LogPkt->q_NotifyLength = w_DataPos;

      p_LogPkt->z_NotifyData.umts_data.u_NotificationPrivType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.notification_priv_type;
      p_LogPkt->z_NotifyData.umts_data.u_SessionInvokeId = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.invoke_id;
      p_LogPkt->z_NotifyData.umts_data.w_Flags = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.flags;
      p_LogPkt->z_NotifyData.umts_data.u_NotificationTextLength = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.notification_length;
      memscpy((void *)(p_LogPkt->z_NotifyData.umts_data.u_NotificationText), sizeof(p_LogPkt->z_NotifyData.umts_data.u_NotificationText),
              p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.notification_text,
              p_LogPkt->z_NotifyData.umts_data.u_NotificationTextLength);

      if (p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.flags & PDSM_LITE_LCS_CLIENT_EXTADDR_PRESENT)
      {
        p_LogPkt->z_NotifyData.umts_data.u_DatacodingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.datacoding_scheme;
        p_LogPkt->z_NotifyData.umts_data.u_ExtClientAddressLen = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.ext_client_address_data.ext_client_address_len;
        memscpy((void *)(p_LogPkt->z_NotifyData.umts_data.u_ExtClientAddress), sizeof(p_LogPkt->z_NotifyData.umts_data.u_ExtClientAddress),
                p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.ext_client_address_data.ext_client_address,
                p_LogPkt->z_NotifyData.umts_data.u_ExtClientAddressLen);
      }
      p_LogPkt->z_NotifyData.umts_data.u_LocationType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.location_type;

      if (p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.flags & PDSM_LITE_LCS_DEF_LOCATION_TYPE_PRESENT)
      {
        p_LogPkt->z_NotifyData.umts_data.u_DeferredLocationUnusedBits = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.deferred_location.unused_bits;
        p_LogPkt->z_NotifyData.umts_data.u_DeferredLocationMsAvail = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.deferred_location.ms_available;
      }

      if (p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.flags & PDSM_LITE_LCS_REQUESTOR_ID_PRESENT)
      {
        p_LogPkt->z_NotifyData.umts_data.u_RequestorIDDataCodeingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.requestor_id.data_coding_scheme;
        p_LogPkt->z_NotifyData.umts_data.u_RequestorIdLen = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.requestor_id.string_len;
        memscpy((void *)(p_LogPkt->z_NotifyData.umts_data.u_RequesterID), sizeof(p_LogPkt->z_NotifyData.umts_data.u_RequesterID),
                p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.requestor_id.requestor_id_string,
                p_LogPkt->z_NotifyData.umts_data.u_RequestorIdLen);
      }

      if (p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.flags & PDSM_LITE_LCS_CODEWORD_PRESENT)
      {
        p_LogPkt->z_NotifyData.umts_data.u_CodewordDataCodingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.codeword_string.data_coding_scheme;
        p_LogPkt->z_NotifyData.umts_data.u_CodewordLen = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.codeword_string.string_len;
        memscpy((void *)(p_LogPkt->z_NotifyData.umts_data.u_CodewordStr), sizeof(p_LogPkt->z_NotifyData.umts_data.u_CodewordStr),
                p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.codeword_string.lcs_codeword_string,
                p_LogPkt->z_NotifyData.umts_data.u_CodewordLen);
      }

      if (p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.flags & PDSM_LITE_LCS_SERVICE_TYPE_ID_PRESENT)
        p_LogPkt->z_NotifyData.umts_data.u_ServiceTypeId = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.umts_cp_req.service_type_id.lcs_service_type_id;


      break;
    case PDSM_LITE_LCS_CMD_TYPE_SUPL_REQ:
      /* Try to allocate memory for the log packet */
      w_DataPos = sizeof(sm_log_supl_notify_verify_data);
      p_LogPkt->u_NotifyStatus = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.resp;
      p_LogPkt->u_NotifyType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info_cmd_type;
      p_LogPkt->q_NotifyLength = w_DataPos;
      p_LogPkt->z_NotifyData.supl_data.u_NotificationPrivType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.notification_priv_type;
      p_LogPkt->z_NotifyData.supl_data.w_Flags = (uint16)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.flags;
      p_LogPkt->z_NotifyData.supl_data.u_DataCodingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.datacoding_scheme;
      p_LogPkt->z_NotifyData.supl_data.u_posMethod = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.pos_method;
      p_LogPkt->z_NotifyData.supl_data.u_Unused[0] = 0;
      p_LogPkt->z_NotifyData.supl_data.u_Unused[1] = 0;
      p_LogPkt->z_NotifyData.supl_data.u_Unused[2] = 0;
      for (i = 0; i < 8; i++)
      {
        p_LogPkt->z_NotifyData.supl_data.u_SlpHash[i] = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_hash[i];
      }

      p_LogPkt->z_NotifyData.supl_data.u_RequestorIdLen = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.requestor_id.string_len;
      if (p_LogPkt->z_NotifyData.supl_data.u_RequestorIdLen != 0)
      {
        for (i = 0; i < p_LogPkt->z_NotifyData.supl_data.u_RequestorIdLen; i++)
        {
          p_LogPkt->z_NotifyData.supl_data.u_RequestorId[i] = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.requestor_id.requestor_id_string[i];
        }
        p_LogPkt->z_NotifyData.supl_data.u_RequstorIdDataCodingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.requestor_id.data_coding_scheme;

      }
      p_LogPkt->z_NotifyData.supl_data.u_ClientNameLen = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.client_name.string_len;
      if (p_LogPkt->z_NotifyData.supl_data.u_ClientNameLen != 0)
      {
        for (i = 0; i < p_LogPkt->z_NotifyData.supl_data.u_ClientNameLen; i++)
        {
          p_LogPkt->z_NotifyData.supl_data.u_ClientName[i] = p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.client_name.client_name_string[i];
        }
        p_LogPkt->z_NotifyData.supl_data.u_ClientNameDataCodingScheme = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.client_name.data_coding_scheme;

      }

      p_LogPkt->z_NotifyData.supl_data.w_UserRespTimer = (uint16)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.user_response_timer;
      p_LogPkt->z_NotifyData.supl_data.u_QopFlags = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_qop.bit_mask;
      if (p_LogPkt->z_NotifyData.supl_data.u_QopFlags & PDSM_LITE_LCS_SUPL_QOP_VALID)
      {
        p_LogPkt->z_NotifyData.supl_data.u_QopHorAcc = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_qop.horacc;
        if (p_LogPkt->z_NotifyData.supl_data.u_QopFlags & PDSM_LITE_LCS_SUPL_QOP_VERACC_VALID)
          p_LogPkt->z_NotifyData.supl_data.u_QopVerAcc = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_qop.veracc;
        if (p_LogPkt->z_NotifyData.supl_data.u_QopFlags & PDSM_LITE_LCS_SUPL_QOP_MAXAGE_VALID)
          p_LogPkt->z_NotifyData.supl_data.w_MaxLocAge = (uint16)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_qop.maxLocAge;
        if (p_LogPkt->z_NotifyData.supl_data.u_QopFlags & PDSM_LITE_LCS_SUPL_QOP_DELAY_VALID)
          p_LogPkt->z_NotifyData.supl_data.u_Delay = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_qop.delay;
      }
      p_LogPkt->z_NotifyData.supl_data.u_SlpSessIdPres = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.presence;
      for (i = 0; i < 4; i++)
      {
        p_LogPkt->z_NotifyData.supl_data.u_SlpSessId[i] = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.session_id[i];
      }
      p_LogPkt->z_NotifyData.supl_data.u_SlpAddrType = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.choice;
      if (p_LogPkt->z_NotifyData.supl_data.u_SlpAddrType  == PDSM_LITE_SERVER_TYPE_IPV4_ADDRESS) // 0: ipv4
      {
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[0] = (uint8)(p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs & 0xFF);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[1] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs & 0xFF00) >> 8);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[2] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs & 0xFF0000) >> 16);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[3] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.server_adrs & 0xFF000000) >> 24);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[4] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id & 0xFF));
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[5] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id & 0xFF00) >> 8);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[6] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id & 0xFF0000) >> 16);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[7] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v4.port_id & 0xFF000000) >> 24);
      }

      else if (p_LogPkt->z_NotifyData.supl_data.u_SlpAddrType == PDSM_LITE_SERVER_TYPE_URL_ADDRESS) // 1: FQDN
      {
        (void)GNSS_STRLCPY((char *)&p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[0],
                           (char *)(p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url),
                           p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_url.url_length + 1);
      }

      else if (p_LogPkt->z_NotifyData.supl_data.u_SlpAddrType == PDSM_LITE_SERVER_TYPE_IPV6_ADDRESS) // 2: ipv6
      {
        for (i = 0; i < 8; i = i + 2)
        {
          p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[i] = (uint8)p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs[i];
          p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[i + 1] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.server_adrs[i] & 0xF0) >> 4);
        }
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[16] = (uint8)(p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id & 0xFF);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[17] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id & 0xFF00) >> 8);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[18] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id & 0xFF0000) >> 16);
        p_LogPkt->z_NotifyData.supl_data.u_SlpAddr[19] = (uint8)((p_LcsType->info.lcs_cmd_info.resp_info.lcs_info.supl_req.supl_slp_session_id.slp_address.supl_slp_address.server_adrs_v6.port_id & 0xFF000000) >> 24);
      }

      break;

    default:
      w_DataPos = 0;
      break;
  }
  p_LogPkt->u_PktVersion = CGPS_SM_LOG_PKT_VERSION_3;
  p_LogPkt->q_ClientId = (uint32)p_LcsType->client_id;
  p_LogPkt->q_Reserved[0] = 0;
  p_LogPkt->q_Reserved[1] = 0;
  log_commit(p_LogPkt);
}
#endif

/*
 ******************************************************************************
 * sm_log_report_position
 *
 * Function description:
 *
 * This function generates a DM log message when position report is recvd by the
 * PDAPI
 *
 * Parameters: 
 * pointer to sm_pd_info_s_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_report_position(pdsm_client_id_type         client_id, sm_FixInfoStructType * input_lr_buf_ptr  , boolean sft, boolean kf)
{
  LOG_GNSS_PDSM_POSITION_REPORT_CALLBACK_C_type *p_LogPkt;
  uint8 i = 0;
  me_Clock z_Clock;
  qword timeStamp;
  DBL heading_angle;
  DBL vel_east;
  DBL vel_north;
  DBL f_tmp;
  FLT ext_vel_horiz_unc;
  FLT ext_vel_vert_unc;
  FLT ext_loc_uncrtnty_a;
  FLT ext_loc_uncrtnty_p;
  utc_time z_utc_time;
  float velHorU;

  if (input_lr_buf_ptr == NULL)
  {
    return;
  }
  

  
  tm_util_get_timestamp_from_gps_time(input_lr_buf_ptr, &z_Clock, timeStamp);
  
  vel_east = input_lr_buf_ptr->z_NavPos.f_VelEnu[0];
  vel_north = input_lr_buf_ptr->z_NavPos.f_VelEnu[1];

  f_tmp = sqrt(vel_east * vel_east + vel_north * vel_north);
  ext_vel_horiz_unc = tm_util_calc_vel_horiz_uncertainty(&input_lr_buf_ptr->z_NavPos);
  ext_vel_vert_unc  = tm_util_calc_vel_vert_uncertainty(&input_lr_buf_ptr->z_NavPos);

  ext_loc_uncrtnty_a  = input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[1];
  ext_loc_uncrtnty_p  = input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[2];
  
  
  if (ext_loc_uncrtnty_a > CGPS_WHOLE_EARTH_UNC)
  {
    ext_loc_uncrtnty_a = CGPS_WHOLE_EARTH_UNC;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Error Ellipse along greater than earth radius", 0, 0, 0);
  }
  if (ext_loc_uncrtnty_p > CGPS_WHOLE_EARTH_UNC)
  {
    ext_loc_uncrtnty_p = CGPS_WHOLE_EARTH_UNC;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Error Ellipse along greater than earth radius", 0, 0, 0);
  }
  
  

  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_GNSS_PDSM_POSITION_REPORT_CALLBACK_C_type *)log_alloc(LOG_GNSS_PDSM_POSITION_REPORT_CALLBACK_C,
                                                                        sizeof(LOG_GNSS_PDSM_POSITION_REPORT_CALLBACK_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->u_PktVersion = GNSS_GEN8_LOG_VERSION; /* new Callisto log */
    /* p_LogPkt->u_PktVersion = CGPS_SM_LOG_PKT_VERSION_3; */

    p_LogPkt->q_ClientId = (uint32)client_id;
    p_LogPkt->l_Longitude = tm_util_nearest_long(input_lr_buf_ptr->z_NavPos.d_PosLla[1] * RAD2DEG *
                                                      LAT_LONG_SCALE);
// p_PdInfoType->pd_info.pd_data.lon;
    p_LogPkt->l_Latitude = tm_util_nearest_long(input_lr_buf_ptr->z_NavPos.d_PosLla[0] * RAD2DEG *
                                                     LAT_LONG_SCALE);
//p_PdInfoType->pd_info.pd_data.lat;
    p_LogPkt->x_Altitude = (sint15)tm_util_nearest_long(input_lr_buf_ptr->z_NavPos.d_PosLla[2])- 500;
//p_PdInfoType->pd_info.pd_data.altitude - 500; // subtract 500 before logging
    
    p_LogPkt->q_TimeStamp = timeStamp[0];
    //p_PdInfoType->pd_info.pd_data.time_stamp;
    p_LogPkt->u_LocUncAng = (byte)(input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[0] / 5.625F);
    //p_PdInfoType->pd_info.pd_data.loc_uncertainty_ang;
    p_LogPkt->u_LocUncA  = (uint8)tm_util_select_pos_unc(input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[1]);
    //p_PdInfoType->pd_info.pd_data.loc_uncertainty_a;
    p_LogPkt->u_LocUncP = (uint8)tm_util_select_pos_unc(input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[2]);
    //  p_PdInfoType->pd_info.pd_data.loc_uncertainty_p;
    p_LogPkt->u_OptFieldMask = PDSM_PD_UNCERTAINTY_VALID | PDSM_PD_MSEC_VALID | PDSM_PD_EXT_STATUS_VALID | PDSM_PD_ALTITUDE_VALID;
    //p_PdInfoType->pd_info.pd_data.opt_field_mask;
    if(input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsVelValid)
    {
       p_LogPkt->u_OptFieldMask |= PDSM_PD_VELOCITY_VALID;
    }
    if (0 == tm_core_info.config_info.z_LeapSecondInfo.u_DeltaLeapSecUnc)
    {
       p_LogPkt->u_OptFieldMask |= PDSM_PD_UTCOFFSET_VALID;
    }
    if( input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed == FALSE ||input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed == TRUE)
    {
       p_LogPkt->u_OptFieldMask &= ~PDSM_PD_MSEC_VALID;
    }
    tm_util_calculate_heading(input_lr_buf_ptr, &heading_angle);
    
    p_LogPkt->w_Heading = (uint16)tm_util_nearest_long(heading_angle * HEADING_SCALE);
    //p_PdInfoType->pd_info.pd_data.heading;
    if (f_tmp < TM_CORE_MIN_VELOCTY)
    {
       p_LogPkt->w_VelocityHor = 0;
    }
    else
    {
       p_LogPkt->w_VelocityHor = (uint16)tm_util_nearest_long(4.0 * f_tmp);
    }
    
    if(input_lr_buf_ptr->z_NavPos.z_PosFlags.b_Is3D)
    {
       p_LogPkt->u_FixType = TRUE;
       p_LogPkt->s_VelocityVer = (int1)tm_util_nearest_long(2.0 * input_lr_buf_ptr->z_NavPos.f_VelEnu[2]);	  
    }	
    //p_PdInfoType->pd_info.pd_data.fix_type;
    
    p_LogPkt->u_LocUncertaintyV = (uint8)tm_util_select_pos_unc(input_lr_buf_ptr->z_NavPos.f_PuncVertMeters);
    //p_PdInfoType->pd_info.pd_data.loc_uncertainty_v;
    p_LogPkt->s_utcOffset = tm_core_info.config_info.z_LeapSecondInfo.u_DeltaLeapSec;
    //p_PdInfoType->pd_info.pd_data.gpsUtcOffset;
    p_LogPkt->u_UncEllConfidence = LOC_UNC_CONFIDENCE;
    //p_PdInfoType->pd_info.pd_data.loc_uncertainty_conf;
    p_LogPkt->u_PositionMethod = 2;

    p_LogPkt->u_PositionSource = 0;
    p_LogPkt->u_PositionSource |= POSITION_SOURCE_OLDNEW;

    if(input_lr_buf_ptr->z_NavPos.z_SmPosFlags.b_IsRefLoc)
    { 
      p_LogPkt->u_PositionSource |=  POSITION_SOURCE_DEFAULT;
    }  
    else 
    {
    
      /* above are mutually exclusive. however, in the following cases, either
      GNSS system might have been used */
      if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed)
      {
        p_LogPkt->u_PositionSource |= POSITION_SOURCE_GPS;
        if (sft == TRUE)
        {
          p_LogPkt->u_PositionSource |= POSITION_SOURCE_SFT;
        }
      }
    
      if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed)
      {
        p_LogPkt->u_PositionSource |= POSITION_SOURCE_GLO;
        if (sft == TRUE)
        {
          p_LogPkt->u_PositionSource |= POSITION_SOURCE_SFT;
        }
      }
    
      /* //RH TODO  u_PositionSource is only uint8
      if (PDSM_LITE_PD_POSITION_SOURCE_BDS & p_PdInfoType->pd_info.pd_data.positioning_source)
      {
        p_LogPkt->u_PositionSource |= POSITION_SOURCE_BDS;
        if (sft == TRUE)
        {
          p_LogPkt->u_PositionSource |= POSITION_SOURCE_SFT;
        }
      }
      */
    
      /* //JV TODO  u_PositionSource is only uint8
      if (PDSM_LITE_PD_POSITION_SOURCE_GAL & p_PdInfoType->pd_info.pd_data.positioning_source)
      {
        p_LogPkt->u_PositionSource |= POSITION_SOURCE_GAL;
        if (sft == TRUE)
        {
          p_LogPkt->u_PositionSource |= POSITION_SOURCE_SFT;
        }
      }
      */
      }
   
    if (kf == TRUE)
    {
      p_LogPkt->u_PositionSource |= POSITION_SOURCE_KF_WLS;
    }

   
    p_LogPkt->q_ExtLoclat = (int32)(input_lr_buf_ptr->z_NavPos.d_PosLla[0]* C_2_32_OVER_PI);
    
    //p_ExtPosData.lat;
    p_LogPkt->q_ExtLoclon = (int32)(input_lr_buf_ptr->z_NavPos.d_PosLla[1]* C_2_31_OVER_PI);
    //p_ExtPosData.lon;
    p_LogPkt->w_ExtLocUncA = (uint16)ext_loc_uncrtnty_a;
    p_LogPkt->w_ExtLocUncP = (uint16)ext_loc_uncrtnty_p;
    velHorU = sqrt(SQR(input_lr_buf_ptr->z_NavPos.f_VelEnu[0]) + SQR(input_lr_buf_ptr->z_NavPos.f_VelEnu[1]));
    
    p_LogPkt->q_VelHoriz = (velHorU < (float)(TM_CORE_MIN_VELOCTY))? 0 : (uint16)(velHorU* 100) ;
    
    p_LogPkt->q_VelVert  = (int32)(input_lr_buf_ptr->z_NavPos.f_VelEnu[2] * 100);
    
    p_LogPkt->q_TimeStampTowMs = (uint32)z_Clock.z_Time.q_GpsMsec;
    p_LogPkt->w_TimeStampGpsWeek = (uint16)z_Clock.z_Time.w_GpsWeek;
    p_LogPkt->u_PosReportedToNetwork = (uint8)FALSE;
    
    if (ext_vel_horiz_unc * 100 > (float)CGPS_MAX_UINT16)
    {
     p_LogPkt->w_VelUncHoriz = (uint32)CGPS_MAX_UINT16;
    }
    else
    {
      p_LogPkt->w_VelUncHoriz = (uint32)(ext_vel_horiz_unc * 100);
    }
	
    //p_LogPkt->w_VelUncHoriz = (uint32)p_ExtPosData.velUncHoriz;
	if (ext_vel_vert_unc * 100 > (float)CGPS_MAX_UINT16)
    {
      p_LogPkt->w_VelUncVert = (uint32) CGPS_MAX_UINT16;
    }
    else
    {
     p_LogPkt->w_VelUncVert = (uint32)(ext_vel_vert_unc * 100);
    }
    //p_LogPkt->w_VelUncVert = (uint32)p_ExtPosData.velUncVert;
    p_LogPkt->q_Hdop = (uint32)(input_lr_buf_ptr->z_NavPos.f_HDOP * 10.0 + 0.5);
    p_LogPkt->q_Pdop = (uint32)(input_lr_buf_ptr->z_NavPos.f_PDOP * 10.0 + 0.5);
    p_LogPkt->q_Vdop = (uint32)(input_lr_buf_ptr->z_NavPos.f_VDOP * 10.0 + 0.5);
    p_LogPkt->q_Tdop = (uint32)(input_lr_buf_ptr->z_NavPos.f_TDOP * 10.0 + 0.5);
    p_LogPkt->q_Gdop = (uint32)(input_lr_buf_ptr->z_NavPos.f_GDOP * 10.0 + 0.5);

    if((FALSE == input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed) &&
       (TRUE == input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed))
    {
        if (tm_util_glo_to_utc(input_lr_buf_ptr->z_NavPos.u_GloNumFourYear,
                               input_lr_buf_ptr->z_NavPos.w_GloNumDaysInFourYear,
                               input_lr_buf_ptr->z_NavPos.q_GloTimeMs,
                               &z_utc_time))
        {
          

          p_LogPkt->u_UtcHour = z_utc_time.q_hours;
          p_LogPkt->u_UtcMin = z_utc_time.q_mins;
          p_LogPkt->w_UtcSec = (uint16)((z_utc_time.q_secs * 100) + (z_utc_time.q_tenths * 10));

          p_LogPkt->q_UtcDate =
             (uint32)((z_utc_time.q_utc_days * 1000000) +
                      (z_utc_time.q_month * 10000) + z_utc_time.q_year);
        }
       
      }
      else
      {

        z_utc_time = tm_util_construct_utc_time(input_lr_buf_ptr->z_NavPos.w_GpsWeek,
                                                input_lr_buf_ptr->z_NavPos.q_GpsTimeMs,
                                                input_lr_buf_ptr->z_NavPos.q_UtcTimeMs);
        p_LogPkt->u_UtcHour = z_utc_time.q_hours;
        p_LogPkt->u_UtcMin = z_utc_time.q_mins;
        p_LogPkt->w_UtcSec = (uint16)((z_utc_time.q_secs * 100) + (z_utc_time.q_tenths * 10));


        p_LogPkt->q_UtcDate =
           (uint32)((z_utc_time.q_utc_days * 1000000) +
                    (z_utc_time.q_month * 10000) + z_utc_time.q_year);
      }
	
	
   // p_LogPkt->u_UtcHour = p_ExtPosData.utc_time.hour;
   // p_LogPkt->u_UtcMin = p_ExtPosData.utc_time.minute;
   // p_LogPkt->w_UtcSec = p_ExtPosData.utc_time.sec;
   // p_LogPkt->q_UtcDate = p_ExtPosData.utc_time.date;
   
   p_LogPkt->q_LocUncHorizontal = (uint32)(ext_loc_uncrtnty_a * ext_loc_uncrtnty_a + ext_loc_uncrtnty_p * ext_loc_uncrtnty_p);
	//p_ExtPosData.f_loc_unc_horizontal;

    p_LogPkt->u_TimeMask = 0;

    if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed)
    {
      p_LogPkt->u_TimeMask |= SM_LOG_TIME_MASK_SOURCE_GPS;
    }

    if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed)
    {
      p_LogPkt->u_TimeMask |= SM_LOG_TIME_SOURCE_MASK_GLO;
    }

    if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed && input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed)
    {
      p_LogPkt->u_TimeMask |= SM_LOG_TIME_MASK_GLO_GPS_TIMEOFFSET_VALID;
	  p_LogPkt->q_GloOffset = (FLT)(input_lr_buf_ptr->z_NavPos.z_InterGnssTb[GNSS_TB_GPSGLO].f_FiltMeters * ONE_OVER_LIGHT_MSEC);
	  //p_PdInfoType->pd_info.pd_data.ext_pos_data.glo_offset;
    }
    

    if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsBdsUsed)
    {
      p_LogPkt->u_TimeMask |= SM_LOG_TIME_SOURCE_MASK_BDS;
    }

    if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGalUsed)
    {
      p_LogPkt->u_TimeMask |= SM_LOG_TIME_SOURCE_MASK_GAL;
    }


    for (i = 0; i < GNSS_ARRAY_SIZEOF(p_LogPkt->q_Reserved); i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);
  }
}

/*
 ******************************************************************************
 * sm_log_pd_event_callback
 *
 * Function description:
 *
 * This function generates a DM log message when a PD_Event_Callback other than
 * a position report is recvd PDAPI
 *
 * Parameters: 
 * pdsm_lite_pd_event_type
 * pointer to pdsm_lite_pd_info_s_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_pd_event_callback(pdsm_lite_pd_event_type e_PdEvent,
                              const pdsm_lite_pd_info_s_type *p_PdInfoType)
{

  LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C_type *p_LogPkt;
  uint8 i;

  if (p_PdInfoType == NULL)
  {
    return;
  }
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C_type *)log_alloc(
     LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C,
     sizeof(LOG_GNSS_PDSM_PD_EVENT_CALLBACK_C_type));

  if (p_LogPkt == NULL)
    return;
  p_LogPkt->q_ClientId = (uint32)p_PdInfoType->client_id;
  p_LogPkt->u_EventData = 0;
  switch (e_PdEvent)
  {
    case PDSM_LITE_PD_EVENT_END:
      p_LogPkt->u_EventType = PD_EVENT_END;
      p_LogPkt->u_EventData = (uint8)p_PdInfoType->pd_info.end_status;
      break;
    case PDSM_LITE_PD_EVENT_BEGIN:
      p_LogPkt->u_EventType = PD_EVENT_BEGIN;
      break;
    case PDSM_LITE_PD_EVENT_DONE:
      p_LogPkt->u_EventType = PD_EVENT_DONE;
      break;
#if 0
    case PDSM_LITE_PD_EVENT_COMM_BEGIN:
      p_LogPkt->u_EventType = PD_EVENT_COMM_BEGIN;
      p_LogPkt->u_EventData = (uint8)p_PdInfoType->pd_info.comm_data.protocol_type;
      break;

    case PDSM_LITE_PD_EVENT_COMM_CONNECTED:
      p_LogPkt->u_EventType = PD_EVENT_COMM;
      break;
    case PDSM_LITE_PD_EVENT_COMM_FAILURE:
      p_LogPkt->u_EventType =  PD_EVENT_COMM_FAILURE;
      p_LogPkt->u_EventData = (uint8)p_PdInfoType->pd_info.end_status;
      break;
    case PDSM_LITE_PD_EVENT_COMM_DONE:
      p_LogPkt->u_EventType =  PD_EVENT_COMM_DONE;
      break;
    case PDSM_LITE_PD_EVENT_WARM_PROC_BEGIN:
      p_LogPkt->u_EventType = PD_EVENT_WARM_PROC_BEGIN;
      break;
    case PDSM_LITE_PD_EVENT_WARM:
      p_LogPkt->u_EventType = PD_EVENT_WARM;
      break;
    case PDSM_LITE_PD_EVENT_NOT_WARM:
      p_LogPkt->u_EventType = PD_EVENT_NOT_WARM;
      break;
    case PDSM_LITE_PD_EVENT_WARM_PROC_DONE:
      p_LogPkt->u_EventType = PD_EVENT_WARM_PROC_DONE;
      break;
#endif  

    case PDSM_LITE_PD_EVENT_GPS_BEGIN:
      p_LogPkt->u_EventType =  PD_EVENT_GPS_BEGIN;
      break;
    case PDSM_LITE_PD_EVENT_GPS_DONE:
      p_LogPkt->u_EventType =  PD_EVENT_GPS_END;
      break;

    case PDSM_LITE_PD_EVENT_UPDATE_FAILURE:
      p_LogPkt->u_EventType =  PD_EVENT_UPDATE_FAILURE;
      p_LogPkt->u_EventData = (uint8)p_PdInfoType->pd_info.end_status;
      break;

      /* note PD_DLOAD_INVALID = 19 has no counterpart in pdsm.h,
         therefore it is not currently used */

    case PDSM_LITE_PD_EVENT_COARSE_POS_INJ_DONE:
      p_LogPkt->u_EventType = PD_EVENT_COARSE_POS_INJ_DONE;
      p_LogPkt->u_EventData = (uint8)p_PdInfoType->pd_info.end_status;
      break;

    case PDSM_LITE_PD_EVENT_COARSE_POS_INJ_FAILED:
      p_LogPkt->u_EventType =  PD_EVENT_COARSE_POS_INJ_FAILED;
      break;

    case PDSM_LITE_PD_EVENT_COARSE_POS_INJ_ERROR:
      p_LogPkt->u_EventType =  PD_EVENT_COARSE_POS_INJ_ERROR;
      break;


    default:
      p_LogPkt->u_EventType = PD_EVENT_UNDEF_FOR_LOG;
      p_LogPkt->u_EventData = 0;
      break;
  }
  p_LogPkt->u_PacketVersion = GNSS_GEN8_LOG_VERSION; /* new log for Gen8 */
  for (i = 0; i < 7; i++)
  {
    p_LogPkt->u_Reserved[i] = 0;
  }
  log_commit(p_LogPkt);


}

/*
 ******************************************************************************
 * sm_log_pa_event_callback
 *
 * Function description:
 *
 * This function generates a DM log message when a PA_Event_Callback 
 * is recvd PDAPI
 *
 * Parameters: 
 * pdsm_lite_pa_event_type
 * pointer to pdsm_lite_pa_info_s_type
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/


void sm_log_pa_event_callback(pdsm_lite_pa_event_type e_PaEvent,
                              const pdsm_lite_pa_info_s_type *p_PaInfoType)
{

  LOG_GNSS_PDSM_PA_EVENT_CALLBACK_C_type *p_LogPkt;
  uint8 i;
  uint8 j;

  if (p_PaInfoType == NULL)
  {
    return;
  }
  /* Try to allocate memory for the log packet */

  p_LogPkt = (LOG_GNSS_PDSM_PA_EVENT_CALLBACK_C_type *)log_alloc(LOG_GNSS_PDSM_PA_EVENT_CALLBACK_C,
                                                                 sizeof(LOG_GNSS_PDSM_PA_EVENT_CALLBACK_C_type));


  if (p_LogPkt == NULL)
  {
    return;
  }

  p_LogPkt->q_ClientId = (uint32)p_PaInfoType->client_id;
  p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_3;
  p_LogPkt->u_EventType = (uint8)PDSM_LITE_PA_NONE;       /* Init */
  p_LogPkt->u_Length = 4;

  switch (e_PaEvent)
  {

    case PDSM_LITE_PA_EVENT_GPS_LOCK:
      p_LogPkt->u_Length = 4;
      p_LogPkt->u_EventType = (uint8)PDSM_LITE_PA_GPS_LOCK;
      p_LogPkt->u_ParameterValue[0] = (uint8)p_PaInfoType->pa_info.gps_lock;
      break;

    case PDSM_LITE_PA_EVENT_DELETE_PARAMS:     /* PDSM_LITE_PA_DELETE_PARAMS_EX also
                                          uses PDSM_LITE_PA_EVENT_DELETE_PARAMS since payloads
                                          are congruent, and there are no free bits
                                          to define a new PA_EVENT */
      { /* create new stack */
        //uint8 u_byte_iter = 0;
        uint32 q_temp = 0;    /* temporarily uint32 hold data to be pumped out */
        uint64 t_pdsm_lite_delete_parms_flags = 0;
        uint8 *pOutputByteArray = p_LogPkt->u_ParameterValue;

        p_LogPkt->u_Length = SM_LOG_LENGTH_PA_EVENT_DELETE_PARAMS;
        p_LogPkt->u_EventType = (uint8)PDSM_LITE_PA_DELETE_PARAMS_EX1; /* PDSM_LITE_PA_DELETE_PARAMS_EX also
                                                              uses this same event since payloads
                                                              are congruent */

        //lower 32-bits
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.pdsm_lite_delete_parms_flags & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //upper 32-bits
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.pdsm_lite_delete_parms_flags >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.time_unc;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.pos_unc;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.time_offset;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.pos_offset;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.eph_sv_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.alm_sv_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.glo_eph_sv_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.glo_alm_sv_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);



        //BDS EPH low
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.bds_eph_sv_mask & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //BDS EPH hi
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.bds_eph_sv_mask >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        //BDS ALM low
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.bds_alm_sv_mask & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //BDS ALM hi
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.bds_alm_sv_mask >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        //GAL EPH low
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.gal_eph_sv_mask & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //GAL EPH hi
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.gal_eph_sv_mask >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        //GAL ALM low
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.gal_alm_sv_mask & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //GAL ALM hi
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.gal_alm_sv_mask >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.qzss_eph_sv_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.qzss_alm_sv_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.gps_gnss_sv_blacklist_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.glo_gnss_sv_blacklist_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.bds_gnss_sv_blacklist_mask & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.bds_gnss_sv_blacklist_mask >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //GAL BLACK LIST low
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.gal_gnss_sv_blacklist_mask & CGPS_MAX_UINT32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
        //GAL BLACK LIST hi
        q_temp = (uint32)(p_PaInfoType->pa_info.delete_params.gal_gnss_sv_blacklist_mask >> 32);
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

        q_temp = p_PaInfoType->pa_info.delete_params.qzss_gnss_sv_blacklist_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
#if 0 //TOOD: [TBD][APP][Whether cell db is required or not]
        q_temp = p_PaInfoType->pa_info.delete_params.delete_celldb_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);
#endif
        q_temp = p_PaInfoType->pa_info.delete_params.delete_clock_info_mask;
        (void)sm_log_write_uint32_little_endian(pOutputByteArray, q_temp);
        pOutputByteArray += sizeof(q_temp);

      }
      break;

    
    case PDSM_LITE_PA_EVENT_DPO_CONTROL:
      p_LogPkt->u_Length = 1;
      p_LogPkt->u_EventType = (uint8)PDSM_LITE_PA_DPO_CONTROL;
      p_LogPkt->u_ParameterValue[0] = (uint8)p_PaInfoType->pa_info.dpo_control;
      break;


    default:
      break;
  }

  /*! (New method)
   *  PA event enum based assignment - Assign PA event enum if no conversion to
   *  PA event bitmap takes place in the above section.
  */
  
  

  p_LogPkt->u_Reserved = 0;
  log_commit(p_LogPkt);
}


/*
 ******************************************************************************
 * sm_log_pd_cmd_err
 *
 * Function description:
 *
 * This function generates a DM log message when a pd_cmd_err
 * is recvd by PDAPI
 *
 * Parameters: 
 * client_id
 * command_type
 * err_value
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_pd_cmd_err(uint32 l_ClientId,
                       pdsm_lite_pd_cmd_e_type e_CmdType,
                       pdsm_lite_pd_cmd_err_e_type e_CmdErr)
{
  LOG_CGPS_PDSM_PD_CMD_ERR_CALLBACK_C_type *p_LogPkt;
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_PD_CMD_ERR_CALLBACK_C_type *)log_alloc(LOG_CGPS_PDSM_PD_CMD_ERR_CALLBACK_C,
                                                                   sizeof(LOG_CGPS_PDSM_PD_CMD_ERR_CALLBACK_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->q_ClientId = l_ClientId;
    p_LogPkt->u_CmdType = (uint8)e_CmdType;
    p_LogPkt->z_CmdErrType = e_CmdErr;
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_1;
    p_LogPkt->u_Reserved[0] = 0;
    p_LogPkt->u_Reserved[1] = 0;
    log_commit(p_LogPkt);
  }

}
/*
 ******************************************************************************
 * sm_log_pa_cmd_err
 *
 * Function description:
 *
 * This function generates a DM log message when a pa_cmd_err
 * is recvd by PDAPI
 *
 * Parameters: 
 * l_ClientId
 * command_type
 * err_value
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_pa_cmd_err(uint32 l_ClientId,
                       pdsm_lite_pa_cmd_e_type e_CmdType,
                       pdsm_lite_pa_cmd_err_e_type e_CmdErr)
{
  LOG_CGPS_PDSM_PA_CMD_ERR_CALLBACK_C_type *p_LogPkt;
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_PA_CMD_ERR_CALLBACK_C_type *)log_alloc(LOG_CGPS_PDSM_PA_CMD_ERR_CALLBACK_C,
                                                                   sizeof(LOG_CGPS_PDSM_PA_CMD_ERR_CALLBACK_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->q_ClientId = l_ClientId;
    p_LogPkt->u_CmdType = (uint8)e_CmdType;
    p_LogPkt->z_CmdErrType = e_CmdErr;
    p_LogPkt->u_PacketVersion = 0;
    p_LogPkt->u_Reserved[0] = 0;
    p_LogPkt->u_Reserved[1] = 0;
    log_commit(p_LogPkt);
  }
}


/*
 ******************************************************************************
 * sm_log_diag_position
 *
 * Function description:
 *
 * This function generates a DM log message when diagnostic position report is recvd by the
 * PDAPI
 *
 * Parameters: 
 * l_ClientId - l_ClientId 
 * p_PdInfoType - pointer to position
 * pos_from_oem - 1 if the position is received from the OEM, 0 if the position is from position engine.
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_diag_position(uint32 l_ClientId,
                          sm_FixInfoStructType *input_lr_buf_ptr,
                          uint8 u_PosFromOem,
                          boolean sft, boolean kf)
{
  me_Clock z_Clock;
  LOG_CGPS_PDSM_EXT_STATUS_POS_REPORT_C_type *p_LogPkt;
  qword timeStamp; 
  DBL heading_angle;
  byte i = 0;
  DBL vel_east;
  DBL vel_north;
  DBL f_tmp;
  pdsm_xtra_download_info_s_type    p_download_info ;
  boolean xtra_valid = TRUE;
  FLT ext_vel_horiz_unc;
  FLT ext_vel_vert_unc;
  FLT ext_loc_uncrtnty_a;
  FLT ext_loc_uncrtnty_p;
  utc_time z_utc_time;
  float velHorU;

  if (input_lr_buf_ptr == NULL)
  {
    return;
  }
  
  tm_util_get_timestamp_from_gps_time(input_lr_buf_ptr, &z_Clock, timeStamp);
  
  vel_east = input_lr_buf_ptr->z_NavPos.f_VelEnu[0];
  vel_north = input_lr_buf_ptr->z_NavPos.f_VelEnu[1];

  f_tmp = sqrt(vel_east * vel_east + vel_north * vel_north);
  

  ext_vel_horiz_unc = tm_util_calc_vel_horiz_uncertainty(&input_lr_buf_ptr->z_NavPos);
  ext_vel_vert_unc  = tm_util_calc_vel_vert_uncertainty(&input_lr_buf_ptr->z_NavPos);

  ext_loc_uncrtnty_a  = input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[1];
  ext_loc_uncrtnty_p  = input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[2];
  
  
  if (ext_loc_uncrtnty_a > CGPS_WHOLE_EARTH_UNC)
  {
    ext_loc_uncrtnty_a = CGPS_WHOLE_EARTH_UNC;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Error Ellipse along greater than earth radius", 0, 0, 0);
  }
  if (ext_loc_uncrtnty_p > CGPS_WHOLE_EARTH_UNC)
  {
    ext_loc_uncrtnty_p = CGPS_WHOLE_EARTH_UNC;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Error Ellipse along greater than earth radius", 0, 0, 0);
  }


  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_EXT_STATUS_POS_REPORT_C_type *)log_alloc(LOG_CGPS_PDSM_EXT_STATUS_POS_REPORT_C,
                                                                     sizeof(LOG_CGPS_PDSM_EXT_STATUS_POS_REPORT_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_8;
    p_LogPkt->q_ClientId = l_ClientId;
    p_LogPkt->l_Longitude = tm_util_nearest_long(input_lr_buf_ptr->z_NavPos.d_PosLla[1] * RAD2DEG *
                                                      LAT_LONG_SCALE);
    p_LogPkt->l_Latitude = tm_util_nearest_long(input_lr_buf_ptr->z_NavPos.d_PosLla[0] * RAD2DEG *
                                                     LAT_LONG_SCALE);
    p_LogPkt->x_Altitude = (sint15)tm_util_nearest_long(input_lr_buf_ptr->z_NavPos.d_PosLla[2])- 500;
    p_LogPkt->q_TimeStamp = timeStamp[0];
    p_LogPkt->u_LocUncAng = (byte)(input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[0] / 5.625F);
    p_LogPkt->u_LocUncA  = (uint8)tm_util_select_pos_unc(input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[1]);
    p_LogPkt->u_LocUncP = (uint8)tm_util_select_pos_unc(input_lr_buf_ptr->z_NavPos.f_ErrorEllipse[2]);
    p_LogPkt->u_OptFieldMask = PDSM_PD_UNCERTAINTY_VALID | PDSM_PD_MSEC_VALID | PDSM_PD_EXT_STATUS_VALID | PDSM_PD_ALTITUDE_VALID;
    //p_PdInfoType->pd_info.pd_data.opt_field_mask;
    if(input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsVelValid)
    {
       p_LogPkt->u_OptFieldMask |= PDSM_PD_VELOCITY_VALID;
    }
    if (0 == tm_core_info.config_info.z_LeapSecondInfo.u_DeltaLeapSecUnc)
    {
       p_LogPkt->u_OptFieldMask |= PDSM_PD_UTCOFFSET_VALID;
    }
    if( input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed == FALSE ||input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed == TRUE)
    {
       p_LogPkt->u_OptFieldMask &= ~PDSM_PD_MSEC_VALID;
    }
    tm_util_calculate_heading(input_lr_buf_ptr, &heading_angle);
    
    p_LogPkt->w_Heading = (uint16)tm_util_nearest_long(heading_angle * HEADING_SCALE);
    //p_PdInfoType->pd_info.pd_data.heading;
    if (f_tmp < TM_CORE_MIN_VELOCTY)
    {
       p_LogPkt->w_VelocityHor = 0;
    }
    else
    {
       p_LogPkt->w_VelocityHor = (uint16)tm_util_nearest_long(4.0 * f_tmp);
    }
    
    if(input_lr_buf_ptr->z_NavPos.z_PosFlags.b_Is3D)
    {
       p_LogPkt->u_FixType = TRUE;
       p_LogPkt->s_VelocityVer = (int1)tm_util_nearest_long(2.0 * input_lr_buf_ptr->z_NavPos.f_VelEnu[2]);	  
    }

    p_LogPkt->u_LocUncertaintyV = (uint8)tm_util_select_pos_unc(input_lr_buf_ptr->z_NavPos.f_PuncVertMeters);
    p_LogPkt->s_utcOffset = tm_core_info.config_info.z_LeapSecondInfo.u_DeltaLeapSec;
   
	/*TODO: [DEL][APP] This switch case is being commented for time being for compilation purposes
	  Latter this whole function has to be removed
	*/
    p_LogPkt->u_UncEllConfidence = LOC_UNC_CONFIDENCE;
    //p_PdInfoType->pd_info.pd_data.loc_uncertainty_conf;
    p_LogPkt->u_PositionMethod = 2;

    p_LogPkt->u_PositionSource = 0;
    p_LogPkt->u_PositionSource |= POSITION_SOURCE_OLDNEW;

    if(input_lr_buf_ptr->z_NavPos.z_SmPosFlags.b_IsRefLoc)
    { 
      p_LogPkt->u_PositionSource |=  POSITION_SOURCE_DEFAULT;
    }  
    else 
    {
    
      /* above are mutually exclusive. however, in the following cases, either
      GNSS system might have been used */
      if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed)
      {
        p_LogPkt->u_PositionSource |= POSITION_SOURCE_GPS;
        if (sft == TRUE)
        {
          p_LogPkt->u_PositionSource |= POSITION_SOURCE_SFT;
        }
      }
    
      if (input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed)
      {
        p_LogPkt->u_PositionSource |= POSITION_SOURCE_GLO;
        if (sft == TRUE)
        {
          p_LogPkt->u_PositionSource |= POSITION_SOURCE_SFT;
        }
      }
    }
    if (kf == TRUE)
    {
      p_LogPkt->u_PositionSource |= POSITION_SOURCE_KF_WLS;
    }

    
    p_LogPkt->q_ExtLoclat = (int32)input_lr_buf_ptr->z_NavPos.d_PosLla[0]* C_2_32_OVER_PI;
    p_LogPkt->q_ExtLoclon = (int32)input_lr_buf_ptr->z_NavPos.d_PosLla[1]* C_2_32_OVER_PI;
    p_LogPkt->w_ExtLocUncA = (uint16)ext_loc_uncrtnty_a;
    p_LogPkt->w_ExtLocUncP = (uint16)ext_loc_uncrtnty_p;
    velHorU = sqrt(SQR(input_lr_buf_ptr->z_NavPos.f_VelEnu[0]) + SQR(input_lr_buf_ptr->z_NavPos.f_VelEnu[1]));
    
    p_LogPkt->q_VelHoriz = (velHorU < (float)(TM_CORE_MIN_VELOCTY))? 0 : (uint16)velHorU* 100 ;

    p_LogPkt->q_VelVert  = (int32) input_lr_buf_ptr->z_NavPos.f_VelEnu[2] * 100;
    p_LogPkt->q_TimeStampTowMs = (uint32)z_Clock.z_Time.q_GpsMsec;
    p_LogPkt->w_TimeStampGpsWeek = (uint16)z_Clock.z_Time.w_GpsWeek;
    p_LogPkt->u_PosReportedToNetwork = (uint8)FALSE;
    
    if (ext_vel_horiz_unc * 100 > (float)CGPS_MAX_UINT16)
    {
     p_LogPkt->w_VelUncHoriz = (uint32)CGPS_MAX_UINT16;
    }
    else
    {
      p_LogPkt->w_VelUncHoriz = (uint32)(ext_vel_horiz_unc * 100);
    }
	
    //p_LogPkt->w_VelUncHoriz = (uint32)p_ExtPosData.velUncHoriz;
	if (ext_vel_vert_unc * 100 > (float)CGPS_MAX_UINT16)
    {
      p_LogPkt->w_VelUncVert = (uint32) CGPS_MAX_UINT16;
    }
    else
    {
     p_LogPkt->w_VelUncVert = (uint32)(ext_vel_vert_unc * 100);
    }

    p_LogPkt->q_FixTimeRemainingMSec = tm_core_info.session_info.q_FixTimeRemainingMSec/1000;    /* Time remaining for the Fix to Timeout */

    p_LogPkt->u_PosFromOem = u_PosFromOem;
    p_LogPkt->q_Hdop = (uint32)(input_lr_buf_ptr->z_NavPos.f_HDOP * 10.0 + 0.5);
    p_LogPkt->q_Pdop = (uint32)(input_lr_buf_ptr->z_NavPos.f_PDOP * 10.0 + 0.5);
    p_LogPkt->q_Vdop = (uint32)(input_lr_buf_ptr->z_NavPos.f_VDOP * 10.0 + 0.5);
    p_LogPkt->q_Tdop = (uint32)(input_lr_buf_ptr->z_NavPos.f_TDOP * 10.0 + 0.5);
    p_LogPkt->q_Gdop = (uint32)(input_lr_buf_ptr->z_NavPos.f_GDOP * 10.0 + 0.5);
    if((FALSE == input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGpsUsed) &&
       (TRUE == input_lr_buf_ptr->z_NavPos.z_PosFlags.b_IsGloUsed))
    {
        if (tm_util_glo_to_utc(input_lr_buf_ptr->z_NavPos.u_GloNumFourYear,
                               input_lr_buf_ptr->z_NavPos.w_GloNumDaysInFourYear,
                               input_lr_buf_ptr->z_NavPos.q_GloTimeMs,
                               &z_utc_time))
        {
          

          p_LogPkt->u_UtcHour = z_utc_time.q_hours;
          p_LogPkt->u_UtcMin = z_utc_time.q_mins;
          p_LogPkt->w_UtcSec = (uint16)((z_utc_time.q_secs * 100) + (z_utc_time.q_tenths * 10));

          p_LogPkt->q_UtcDate =
             (uint32)((z_utc_time.q_utc_days * 1000000) +
                      (z_utc_time.q_month * 10000) + z_utc_time.q_year);
        }
       
      }
      else
      {

        z_utc_time = tm_util_construct_utc_time(input_lr_buf_ptr->z_NavPos.w_GpsWeek,
                                                input_lr_buf_ptr->z_NavPos.q_GpsTimeMs,
                                                input_lr_buf_ptr->z_NavPos.q_UtcTimeMs);
        p_LogPkt->u_UtcHour = z_utc_time.q_hours;
        p_LogPkt->u_UtcMin = z_utc_time.q_mins;
        p_LogPkt->w_UtcSec = (uint16)((z_utc_time.q_secs * 100) + (z_utc_time.q_tenths * 10));


        p_LogPkt->q_UtcDate =
           (uint32)((z_utc_time.q_utc_days * 1000000) +
                    (z_utc_time.q_month * 10000) + z_utc_time.q_year);
      }

     p_LogPkt->q_LocUncHorizontal = (uint32)(ext_loc_uncrtnty_a * ext_loc_uncrtnty_a + ext_loc_uncrtnty_p * ext_loc_uncrtnty_p);
    xtra_valid = tm_xtra_set_xtra_time_and_validity((pdsm_lite_xtra_download_info_s_type *)&p_download_info);
    if (xtra_valid)
    {

    p_LogPkt->w_XtraStartGpsMinutes =  p_download_info.startGpsMinutes;
    p_LogPkt->w_XtraStartGpsWeek = p_download_info.startGpsWeek;
    p_LogPkt->w_XtravalidDurationHours = p_download_info.validDurationHours;
    }
    p_LogPkt->q_SvIdGps = input_lr_buf_ptr->z_NavSvInfo.q_GpsSvMaskUsed;
    p_LogPkt->q_SvIdGlo = input_lr_buf_ptr->z_NavSvInfo.q_GloSvMaskUsed;
    p_LogPkt->t_SvIdBds = input_lr_buf_ptr->z_NavSvInfo.t_BdsSvMaskUsed;
    p_LogPkt->t_SvIdGal = input_lr_buf_ptr->z_NavSvInfo.t_GalSvMaskUsed;
    p_LogPkt->u_SvIdQzss = input_lr_buf_ptr->z_NavSvInfo.u_QzssSvMaskUsed;

    for (i = 0; i < 8; i++)
    {
      p_LogPkt->u_Reserved[i] = 0;
    }

    log_commit(p_LogPkt);
  }

}
/*
 ******************************************************************************
 * sm_log_nmea_position_report
 *
 * Function description:
 *
 * This function generates a DM log message when nmea report is sent to the
 * PDAPI
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void sm_log_nmea_position_report(uint32 l_ClientId,
                                     const pdsm_lite_pd_nmea_s_type *p_ExtNMEAType)
{
  LOG_CGPS_PDSM_EXT_STATUS_NMEA_REPORT_C_type *p_LogPkt;
  uint8 i = 0;

  if (p_ExtNMEAType == NULL)
  {
    return;
  }
  /* Try to allocate memory for the log packet */
  p_LogPkt = (LOG_CGPS_PDSM_EXT_STATUS_NMEA_REPORT_C_type *)log_alloc(LOG_CGPS_PDSM_EXT_STATUS_NMEA_REPORT_C,
                                                                      sizeof(LOG_CGPS_PDSM_EXT_STATUS_NMEA_REPORT_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_2;
    p_LogPkt->q_ClientId = l_ClientId;
    p_LogPkt->q_NmeaSentenceType = p_ExtNMEAType->nmea_type;
    p_LogPkt->u_NmeaSentenceLength = (uint8)p_ExtNMEAType->nmea_length;
    for (i = 0; (i < p_ExtNMEAType->nmea_length) && (i < LOG_PDSM_EXT_NMEA_RPT_DATA_LEN); i++)
    {
      p_LogPkt->u_NmeaData[i] = p_ExtNMEAType->nmea_data[i];
    }
    log_commit(p_LogPkt);
  }
  return;
}

  
/*
 ******************************************************************************
 * sm_log_extern_coarse_pos_inj_start
 *
 * Function description:
 *
 * This function generates a DM log message when external coarse position 
 * injection is started
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_extern_coarse_pos_inj_start(pdsm_lite_cmd_s_type *p_cmd_type)
{

  LOG_CGPS_PDSM_EXTERN_COARSE_POS_INJ_START_C_type *p_LogPkt;
  uint8 i = 0;
  pdsm_lite_pd_external_position_info_type *ext_pos_ptr;

  ext_pos_ptr = &p_cmd_type->cmd.pd.info.ext_pos_info;

  p_LogPkt = (LOG_CGPS_PDSM_EXTERN_COARSE_POS_INJ_START_C_type *)log_alloc(LOG_CGPS_PDSM_EXTERN_COARSE_POS_INJ_START_C,
                                                                           sizeof(LOG_CGPS_PDSM_EXTERN_COARSE_POS_INJ_START_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->u_PktVersion = CGPS_SM_LOG_PKT_VERSION_2;
    p_LogPkt->q_ClientId = p_cmd_type->cmd.pd.client_id;
    p_LogPkt->q_opt_field_mask = ext_pos_ptr->opt_field_mask;
    p_LogPkt->q_time_info_type = ext_pos_ptr->time_info_type;
    p_LogPkt->q_timestamp_sec =  ext_pos_ptr->timestamp_sec;

    p_LogPkt->q_alt_def = ext_pos_ptr->alt_def;
    p_LogPkt->q_source = ext_pos_ptr->source;
    p_LogPkt->d_latitude =  ext_pos_ptr->latitude;
    p_LogPkt->d_longitude = ext_pos_ptr->longitude;
    p_LogPkt->f_loc_unc_hor = ext_pos_ptr->loc_unc_hor;
    p_LogPkt->f_altitude =  ext_pos_ptr->altitude;
    p_LogPkt->f_loc_unc_vert = ext_pos_ptr->loc_unc_vert;
    p_LogPkt->u_confidence_hor =  ext_pos_ptr->confidence_hor;
    p_LogPkt->u_confidence_vert =  ext_pos_ptr->confidence_vert;

    for (i = 0; i < 4; i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);

  }
}

/*
 ******************************************************************************
 * sm_log_get_gps_state_info
 *
 * Function description:
 *
 * This function generates a DM log message when the receiver state info is
 * reported to PDAPI
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_get_gps_state_info(uint32 q_ClientId)
{

  LOG_CGPS_PDSM_GPS_STATE_INFO_C_type *p_LogPkt;


  p_LogPkt = (LOG_CGPS_PDSM_GPS_STATE_INFO_C_type *)log_alloc(LOG_CGPS_PDSM_GPS_STATE_INFO_C,
                                                              sizeof(LOG_CGPS_PDSM_GPS_STATE_INFO_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_1;
    p_LogPkt->q_ClientId = q_ClientId;
    log_commit(p_LogPkt);

  }
}

/*
 ******************************************************************************
 * sm_log_get_best_avail_pos_info
 *
 * Function description:
 *
 * This function generates a DM log message when the best available position info
 * is reported to PDAPI from the measurement engine.
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_get_best_avail_pos_info(uint32 q_ClientId)
{

  LOG_GNSS_PDSM_BEST_AVAIL_POS_INFO_C_type *p_LogPkt;
  uint32 i = 0;
  p_LogPkt = (LOG_GNSS_PDSM_BEST_AVAIL_POS_INFO_C_type *)log_alloc(LOG_GNSS_PDSM_BEST_AVAIL_POS_INFO_C,
                                                                   sizeof(LOG_GNSS_PDSM_BEST_AVAIL_POS_INFO_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_1;
    p_LogPkt->q_ClientId = q_ClientId;
    for (i = 0; i < 4; i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);
  }
}


/*
 ******************************************************************************
 * sm_log_report_ext_status_best_avail_pos_info
 *
 * Function description:
 *
 * This function generates a DM log message when the best available position is
 * reported to PDAPI from measurement engine.
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_report_ext_status_best_avail_pos_info(uint32 q_ClientId,
                                                  const qmiLocGetBestAvailablePositionIndMsgT_v02 *p_ExtBestAvailPosType)
{
  LOG_GNSS_PDSM_EXT_STATUS_BEST_AVAIL_POS_INFO_C_type *p_LogPkt;
  uint32 i = 0;

  p_LogPkt = (LOG_GNSS_PDSM_EXT_STATUS_BEST_AVAIL_POS_INFO_C_type *)log_alloc(LOG_GNSS_PDSM_EXT_STATUS_BEST_AVAIL_POS_INFO_C,
                                                                              sizeof(LOG_GNSS_PDSM_EXT_STATUS_BEST_AVAIL_POS_INFO_C_type));

  if (p_LogPkt)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "log_alloc success ", 0, 0, 0);
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_8;
    p_LogPkt->q_ClientId = q_ClientId;
    /* Copy the position report information from the extended status position report to the DM log packet position report */
    p_LogPkt->sessionStatus = 0x00000001;//SESS_STATUS_IN_PROGRESS

    /*****  The position report information  which is reported to the QMI client ****/
    /*  Latitude */
    p_LogPkt->latitude_valid = p_ExtBestAvailPosType->latitude_valid;
    p_LogPkt->latitude = p_ExtBestAvailPosType->latitude;
    /*   Longitude */
    p_LogPkt->longitude_valid = p_ExtBestAvailPosType->longitude_valid;
    p_LogPkt->longitude = p_ExtBestAvailPosType->longitude;

    /*   Circular Horizontal Position Uncertainty */
    p_LogPkt->horUncCircular_valid = p_ExtBestAvailPosType->horUncCircular_valid;
    p_LogPkt->horUncCircular = p_ExtBestAvailPosType->horUncCircular;

    /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
    p_LogPkt->horUncEllipseSemiMinor_valid = p_ExtBestAvailPosType->horUncEllipseSemiMinor_valid;
    p_LogPkt->horUncEllipseSemiMinor = p_ExtBestAvailPosType->horUncEllipseSemiMinor;

    /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
    p_LogPkt->horUncEllipseSemiMajor_valid = p_ExtBestAvailPosType->horUncEllipseSemiMajor_valid;
    p_LogPkt->horUncEllipseSemiMajor = p_ExtBestAvailPosType->horUncEllipseSemiMajor;

    /*  Elliptical Horizontal Uncertainty Azimuth */
    p_LogPkt->horUncEllipseOrientAzimuth_valid = p_ExtBestAvailPosType->horUncEllipseOrientAzimuth_valid;
    p_LogPkt->horUncEllipseOrientAzimuth = p_ExtBestAvailPosType->horUncEllipseOrientAzimuth;

    /*  Horizontal Circular Confidence */
    p_LogPkt->horCircularConfidence_valid = p_ExtBestAvailPosType->horCircularConfidence_valid;
    p_LogPkt->horCircularConfidence = p_ExtBestAvailPosType->horCircularConfidence;

    /*  Horizontal Reliability */
    p_LogPkt->horReliability_valid = p_ExtBestAvailPosType->horReliability_valid;
    p_LogPkt->horReliability = p_ExtBestAvailPosType->horReliability;

    /*  Horizontal Speed */
    p_LogPkt->horSpeed_valid = p_ExtBestAvailPosType->horSpeed_valid;
    p_LogPkt->horSpeed = p_ExtBestAvailPosType->horSpeed;

    /*  Horizontal Speed Uncertainty */
    p_LogPkt->horSpeedUnc_valid = p_ExtBestAvailPosType->horSpeedUnc_valid;
    p_LogPkt->horSpeedUnc = p_ExtBestAvailPosType->horSpeedUnc;

    /*  Altitude With Respect to Ellipsoid */
    p_LogPkt->altitudeWrtEllipsoid_valid = p_ExtBestAvailPosType->altitudeWrtEllipsoid_valid;
    p_LogPkt->altitudeWrtEllipsoid = p_ExtBestAvailPosType->altitudeWrtEllipsoid;

    /*  Altitude With Respect to Sea Level */
    p_LogPkt->altitudeWrtMeanSeaLevel_valid = p_ExtBestAvailPosType->altitudeWrtMeanSeaLevel_valid;
    p_LogPkt->altitudeWrtMeanSeaLevel = p_ExtBestAvailPosType->altitudeWrtMeanSeaLevel;

    /*  Vertical Uncertainty */
    p_LogPkt->vertUnc_valid = p_ExtBestAvailPosType->vertSpeedUnc_valid;
    p_LogPkt->vertUnc = p_ExtBestAvailPosType->vertUnc;

    /*  Vertical Confidence */
    p_LogPkt->vertConfidence_valid = p_ExtBestAvailPosType->vertConfidence_valid;
    p_LogPkt->vertConfidence = p_ExtBestAvailPosType->vertConfidence;

    /*  Vertical Reliability */
    p_LogPkt->vertReliability_valid = p_ExtBestAvailPosType->vertReliability_valid;
    p_LogPkt->vertReliability = p_ExtBestAvailPosType->vertReliability;

    /*  Vertical Speed */
    p_LogPkt->vertSpeed_valid = p_ExtBestAvailPosType->vertSpeed_valid;
    p_LogPkt->vertSpeed = p_ExtBestAvailPosType->vertSpeed;

    /*  Vertical Speed Uncertainty */
    p_LogPkt->vertSpeedUnc_valid = p_ExtBestAvailPosType->vertSpeedUnc_valid;
    p_LogPkt->vertSpeedUnc = p_ExtBestAvailPosType->vertSpeedUnc;

    /*  Heading */
    p_LogPkt->heading_valid = p_ExtBestAvailPosType->heading_valid;
    p_LogPkt->heading = p_ExtBestAvailPosType->heading;

    /*  Heading Uncertainty */
    p_LogPkt->headingUnc_valid = p_ExtBestAvailPosType->headingUnc_valid;
    p_LogPkt->headingUnc = p_ExtBestAvailPosType->headingUnc;

    /*  Magnetic Deviation */
    p_LogPkt->magneticDeviation_valid = p_ExtBestAvailPosType->magneticDeviation_valid;
    p_LogPkt->magneticDeviation = p_ExtBestAvailPosType->magneticDeviation;

    /*  Technology Used */
    p_LogPkt->technologyMask_valid = p_ExtBestAvailPosType->technologyMask_valid;
    p_LogPkt->technologyMask = p_ExtBestAvailPosType->technologyMask;

    /*  Dilution of Precision */
    p_LogPkt->PDOP_valid = p_ExtBestAvailPosType->DOP_valid;
    p_LogPkt->PDOP = p_ExtBestAvailPosType->DOP.PDOP;

    p_LogPkt->HDOP_valid = p_ExtBestAvailPosType->DOP_valid;
    p_LogPkt->HDOP = p_ExtBestAvailPosType->DOP.HDOP;

    p_LogPkt->VDOP_valid = p_ExtBestAvailPosType->DOP_valid;
    p_LogPkt->VDOP = p_ExtBestAvailPosType->DOP.VDOP;

    /*  UTC Timestamp */
    p_LogPkt->timestampUtc_valid = p_ExtBestAvailPosType->timestampUtc_valid;
    p_LogPkt->timestampUtc = p_ExtBestAvailPosType->timestampUtc;

    if ((p_ExtBestAvailPosType->timestampUtc_valid == TRUE) &&
        (p_ExtBestAvailPosType->gpsTime_valid == TRUE)
        )
    {
      p_LogPkt->leapSeconds = (uint8)((uint32)((p_ExtBestAvailPosType->gpsTime.gpsTimeOfWeekMs -
                                                (uint32)p_ExtBestAvailPosType->timestampUtc)) / 1000);
      p_LogPkt->leapSeconds_valid = TRUE;

    }
    else
    {
      p_LogPkt->leapSeconds_valid = FALSE;
    }

    /*  Time Uncertainty */
    p_LogPkt->timeUnc_valid = p_ExtBestAvailPosType->timeUnc_valid;
    p_LogPkt->timeUnc = p_ExtBestAvailPosType->timeUnc;

    /*  Time Source */
    p_LogPkt->timeSrc_valid = p_ExtBestAvailPosType->timeSrc_valid;
    p_LogPkt->timeSrc = p_ExtBestAvailPosType->timeSrc;

    /*  Sensor Data Usage */
    p_LogPkt->sensorDataUsage_valid = p_ExtBestAvailPosType->sensorDataUsage_valid;
    p_LogPkt->sensorUsageMask = p_ExtBestAvailPosType->sensorDataUsage.usageMask;
    p_LogPkt->sesorAidingIndicatorMask = p_ExtBestAvailPosType->sensorDataUsage.aidingIndicatorMask;

    /*  SV's Used to Calculate the Fix */
    p_LogPkt->gnssSvUsedList_valid = p_ExtBestAvailPosType->gnssSvUsedList_valid;
    p_LogPkt->gnssSvUsedList_len = p_ExtBestAvailPosType->gnssSvUsedList_len;
    memscpy((void *)p_LogPkt->gnssSvUsedList, sizeof(p_LogPkt->gnssSvUsedList),
            p_ExtBestAvailPosType->gnssSvUsedList, sizeof(p_ExtBestAvailPosType->gnssSvUsedList));

    /*  Horizontal Elliptical Confidence */
    p_LogPkt->horEllipticalConfidence_valid = p_ExtBestAvailPosType->horEllipticalConfidence_valid;
    p_LogPkt->horEllipticalConfidence = p_ExtBestAvailPosType->horEllipticalConfidence;

    /*  GPS Time */
    p_LogPkt->gpsTime_valid = p_ExtBestAvailPosType->gpsTime_valid;
    p_LogPkt->gpsWeek = p_ExtBestAvailPosType->gpsTime.gpsWeek;
    p_LogPkt->gpsTimeOfWeekMs = p_ExtBestAvailPosType->gpsTime.gpsTimeOfWeekMs;

    /* End of position report information */
    for (i = 0; i < 4; i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "log_alloc failure", 0, 0, 0);
  }
}

/*
 ******************************************************************************
 * sm_log_report_gps_state_info
 *
 * Function description:
 *
 * This function generates a DM log message when the receiver state info is
 * reported to PDAPI
 *
 * Parameters: 
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/

void sm_log_report_ext_status_gps_state_info(uint32 q_ClientId,
                                             const pdsm_lite_pd_gps_state_s_type *p_ExtGpsStateType)
{

  LOG_CGPS_PDSM_EXT_STATUS_GPS_STATE_INFO_C_type *p_LogPkt;
  uint8 i = 0;

  p_LogPkt = (LOG_CGPS_PDSM_EXT_STATUS_GPS_STATE_INFO_C_type *)log_alloc(LOG_CGPS_PDSM_EXT_STATUS_GPS_STATE_INFO_C,
                                                                         sizeof(LOG_CGPS_PDSM_EXT_STATUS_GPS_STATE_INFO_C_type));

  if (p_LogPkt)
  {
    p_LogPkt->u_PacketVersion = CGPS_SM_LOG_PKT_VERSION_2;
    p_LogPkt->q_ClientId = q_ClientId;
    p_LogPkt->q_Lat = p_ExtGpsStateType->lat;  /* Latitude: LSB = Pi/2^32, Range -Pi/2 to Pi/2 */
    p_LogPkt->q_Lon = p_ExtGpsStateType->lon;  /* Longitude: LSB = Pi/2^31, Range -Pi to Pi */
    p_LogPkt->q_Altitude = p_ExtGpsStateType->altitude; /* Altitude in meters */
    p_LogPkt->q_Punc = (uint32)p_ExtGpsStateType->f_loc_unc_horizontal; /* Position Unc in meters */
    p_LogPkt->w_LocUncVertical = p_ExtGpsStateType->loc_unc_vertical; /* vertical unc in meters  */
    p_LogPkt->q_TimestampTowMs = p_ExtGpsStateType->timestamp_tow_ms; /* Timestamp in GPS TOW milliseconds */
    p_LogPkt->w_TimestampGpsWeek = p_ExtGpsStateType->timestamp_gps_week;  /*GPS week number */
 
    p_LogPkt->b_EngineState = p_ExtGpsStateType->engine_state; /*  GPS engine state on/off */
    
 

    p_LogPkt->w_XtraStartGpsWeek = p_ExtGpsStateType->xtra_start_gps_week; /*  Current XTRA info is good starting this GPS week */
    p_LogPkt->w_XtraStartGpsMinutes = p_ExtGpsStateType->xtra_start_gps_minutes; /*  Current XTRA information is good starting this many minutes into the week */
    for (i = 0; i < 4; i++)
    {
      p_LogPkt->q_Reserved[i] = 0;
    }
    log_commit(p_LogPkt);

  }
}
/**
* writes out a given uint32 as little endian to given byte array
* @param pOutputByteArray
   array where data will be written out to
* @param data 
   uint32 data to be written out
* @returns boolean
   false if pOutputByteArray == NULL. True otherwise.
  @notes Caller must ensure that pOutputByteArray has enough space
*/
boolean sm_log_write_uint32_little_endian(uint8 *pOutputByteArray, uint32 data)
{
  boolean b_retval = (NULL != pOutputByteArray);
  uint8 u_byte_iter = 0;
  if (b_retval)
  {
    for (u_byte_iter = 0; u_byte_iter < sizeof(data); u_byte_iter++)
    {
      pOutputByteArray[u_byte_iter] = SM_LOG_EXTRACT_BYTE(u_byte_iter, data);
    }
  }
  return b_retval;
}


/** logs an error message generated within SM.
 * 
 * @param p_error_msg
 * pointer to error message string to be outputted.
 * 
 * @param u_severity
 *  user defined numeric value denoting severity of error (maybe
 *  meaningful only to SM)
 * 
 * @param p_filename
 * pointer to filename where error message occured
 * 
 * @param q_linenum
 * line number of error
 */
void sm_log_error_message(const char *p_error_msg,
                          uint8 u_severity,
                          const char *p_filename,
                          uint32 q_linenum)
{
  if (!p_filename)
  {
    p_filename = "null";
  }

  if (p_error_msg)
  {
    LOG_GNSS_SM_ERROR_C_type *p_LogPkt = NULL;
    uint32 q_filename_length = 0;
    char c_temp = 0;
    uint32 q_message_length = 0;
    int32 n_retval = 0;

    /* estimate size of "filename:line " text */
    n_retval = (int32)(strlen(p_filename) + snprintf(&c_temp, 0, ":%lu ", q_linenum));
    q_filename_length = n_retval >= 0 ? n_retval : 0;
    q_message_length = (uint32)(strlen(p_error_msg) + q_filename_length);
    q_message_length = MIN(q_message_length,
                           SM_LOG_LENGTH_MAX_SM_ERROR_MESSAGE - 1);
    /* -1 to keep u_length below from overflowing */

    p_LogPkt = (LOG_GNSS_SM_ERROR_C_type *)
       log_alloc(LOG_GNSS_SM_ERROR_C,
                 sizeof(LOG_GNSS_SM_ERROR_C_type) + q_message_length);
    if (p_LogPkt)
    {
      p_LogPkt->u_PacketVersion = GNSS_GEN8_LOG_VERSION;
      p_LogPkt->q_Reserved[0] = 0;
      p_LogPkt->q_Reserved[1] = 0;
      p_LogPkt->u_Length = (uint8)(q_message_length + 1);
      /* +1 because of LOG_GNSS_SM_ERROR_C_type::u_MessageData[1] */

      p_LogPkt->u_Severity = u_severity;

      if (n_retval >= 0)
      {
        /* if message + filename / line number is larger, hopefully, filename + line number will
        give us enough clues about the error */
        (void)snprintf((char *)p_LogPkt->u_MessageData, p_LogPkt->u_Length, "%s:%lu %s",
                       p_filename,
                       q_linenum,
                       p_error_msg);
      }
      else
      {
        memscpy(p_LogPkt->u_MessageData, p_LogPkt->u_Length - 1, p_error_msg, p_LogPkt->u_Length - 1);
      }
      p_LogPkt->u_MessageData[p_LogPkt->u_Length - 1] = '\0'; /* nul terminate string */

      log_commit(p_LogPkt);
    } /* end if(p_LogPkt) */
  } /* end if(p_error_msg) */
  return;
}

#if defined(FEATURE_CIRC_BUF_IPC) || defined(FEATURE_CIRC_BUF_F3)

/*===========================================================================

FUNCTION sm_log_get_timestamp

DESCRIPTION
  This function is to fetch time from 6 Jan 1980 00:00:00 in format days,hr,sec,msec

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_log_circ_buf_get_timestamp(sm_log_circ_buf_entry_timestamp* pz_timestamp)
{
  /*store sys time and fetch time stamp info */
  uint64 t_sys_time ;
  uint64 t_hr,t_min,t_sec,t_msec,t_days;

  if(pz_timestamp == NULL)
  {
    return;
  }

  /* fetch time since 6 Jan 1980 00:00:00 in ms */
  t_sys_time = time_get_ms_native();
  
  t_msec = t_sys_time % SM_LOG_CIRC_BUF_CONVERT_TIME_SEC_TO_MSEC;
  t_sec = t_sys_time / SM_LOG_CIRC_BUF_CONVERT_TIME_SEC_TO_MSEC; // set the seconds remaining
  t_min = t_sec / SM_LOG_CIRC_BUF_CONVERT_TIME_SEC_TO_MIN; //convert seconds to minutes
  t_hr = t_min / SM_LOG_CIRC_BUF_CONVERT_TIME_SEC_TO_MIN; //convert minutes to hours
  t_days = t_hr / SM_LOG_CIRC_BUF_CONVERT_TIME_HOURS_TO_DAY; //convert hours to days

  t_sec = t_sec- (t_min * SM_LOG_CIRC_BUF_CONVERT_TIME_SEC_TO_MIN); //subtract the coverted seconds to minutes in order to display 59 secs max
  t_min = t_min - (t_hr * SM_LOG_CIRC_BUF_CONVERT_TIME_SEC_TO_MIN); //subtract the coverted minutes to hours in order to display 59 minutes max
  t_hr = t_hr - (t_days * SM_LOG_CIRC_BUF_CONVERT_TIME_HOURS_TO_DAY); //subtract the coverted hours to days in order to display 23 hours max

  /* fill the input struct */
  pz_timestamp->t_time_hr = t_hr;
  pz_timestamp->t_time_min = t_min;
  pz_timestamp->t_time_sec = t_sec;
  pz_timestamp->t_time_msec = t_msec;
  pz_timestamp->t_time_days = t_days;

}
#endif /*#if defined(FEATURE_BUFFER_IPC) || defined(FEATURE_BUFFER_F3)*/

#ifdef FEATURE_CIRC_BUF_F3
/*===========================================================================

FUNCTION sm_log_circ_buf_get_file_name

DESCRIPTION
  This function will extract filename from the full pathname and encodes the filename

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_log_circ_buf_get_file_name(char* p_path_name, char* p_file_name, uint32 q_size)
{
  /* pointer to iterate path_name array*/
  char*  p_path = NULL;

  /*used to maintain index of temp_file_name array*/
  uint32 q_tmpfile_index  =  0;

  /* used to maintain index of file_name array*/
  uint32 q_filename_index = 0;

  /* used for iteration in loops*/
  uint32 q_iter = 0;
  
  /*used to store file name temporarly before encoding is done*/
  char u_temp_file_name[SM_LOG_CIRC_BUF_TEMP_FILE_NAME_LEN_MAX];

  /* length of file name*/
  uint32 q_file_name_len = 0;

  /* point to end of the full path name*/
  p_path = p_path_name + strlen(p_path_name);
  p_path--;  // excluding '\0' while iterating 

  /* Start iterating backward from end of full path name till we encounter '\\' or '/' 
     after below iteration will have file name extracted in reverse in c_temp_file_name 
     eg:- c.eroc_mt,if the full path name is gnss/sm/src/tm_core.c*/
  while(((*p_path != '\\') && (*p_path != '\/')) &&
        (q_tmpfile_index < sizeof(u_temp_file_name)))
  {
    u_temp_file_name[q_tmpfile_index++] = *p_path;  // will store file name in reverse order in the temp array
    p_path--;
  }

  /*calculating exact length by removing ".c" length i.e 2*/
  if(q_tmpfile_index > 0)
  {
    q_file_name_len = q_tmpfile_index -SM_LOG_CIRC_BUF_FILE_NAME_LEN_NO_OF_BITS ;
  }
  else
  {
    q_file_name_len = 0;
  }
  /* File name is encoded as first 2bytes for length and followed by alternative character if filename is
  greater than a specific length that is passed from "qsize-2" 
  eg:- if qsize is tm_core -> 07tm_core,tm_cm_iface-->12t_miae */
  
  /* encoding filename  length "*/
  p_file_name[1] = SM_LOG_CIRC_BUF_ASCII_VALUE_ZERO + (q_file_name_len % SM_LOG_CIRC_BUF_VALUE_TEN);      // fill the first two bytes with the length of the file
  p_file_name[0] = SM_LOG_CIRC_BUF_ASCII_VALUE_ZERO + ((q_file_name_len / SM_LOG_CIRC_BUF_VALUE_TEN) % SM_LOG_CIRC_BUF_VALUE_TEN);


  /*encoding file name based on file name length, encode full file nane 
  in case if file name is less that qsize -2 
  else encode alternative characters*/
  if(q_file_name_len < (q_size - SM_LOG_CIRC_BUF_FILE_NAME_LEN_NO_OF_BITS)) //print entire file name in case length is less than arraysize-2 as first 2 bytes indicates the lenght value
  {
    q_filename_index = SM_LOG_CIRC_BUF_FILE_NAME_LEN_NO_OF_BITS;/*ignoring first bytes as it indicates the length value*/
    for(q_iter = q_file_name_len + 1;q_iter >= SM_LOG_CIRC_BUF_FILE_NAME_LEN_NO_OF_BITS;) // 
      {
        p_file_name[q_filename_index] = u_temp_file_name[q_iter];
        q_filename_index++;
        q_iter--;
      }
  }
  else
  {
    q_filename_index = SM_LOG_CIRC_BUF_FILE_NAME_LEN_NO_OF_BITS;/*ignoring first bytes as it indicates the length value*/
    for(q_iter = q_file_name_len + 1 ; q_iter >= SM_LOG_CIRC_BUF_FILE_NAME_LEN_NO_OF_BITS;)
    {
      p_file_name[q_filename_index] = u_temp_file_name[q_iter];
      q_filename_index++;
      q_iter -= 2;
      if(q_filename_index >= q_size)
         break;
    }
  }
  
     if(q_filename_index < q_size)
          p_file_name[q_filename_index] = '\0';
        else 
          p_file_name[q_size-1] = '\0';

}


/*===========================================================================

FUNCTION sm_log_circ_buf_push

DESCRIPTION
 This function will push log into the buffer to f3 buffer or ipc buffer

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_log_circ_buf_push(char* p_buffer)
{
  /*string lenght of the f3 that is pushed on to buffer*/
  uint32 q_str_len;
  q_str_len = strlen(p_buffer)+1;
  
  // check if current char pointer is pointing to any string, free the memory in case it is pointer
   if(sm_log_F3_buf.p_msg_link[sm_log_F3_buf.q_curr_buf_index]  != NULL)
   {
     sm_log_F3_buf.w_heap_tracker = sm_log_F3_buf.w_heap_tracker - strlen((const char*)sm_log_F3_buf.p_msg_link[sm_log_F3_buf.q_curr_buf_index]) -1;
     free(sm_log_F3_buf.p_msg_link[sm_log_F3_buf.q_curr_buf_index] );
     sm_log_F3_buf.p_msg_link[sm_log_F3_buf.q_curr_buf_index]  == NULL;
   }

   //allocate memory for F3 and push in to the circular buffer 
   sm_log_F3_buf.p_msg_link[sm_log_F3_buf.q_curr_buf_index]   = malloc(sizeof(char) * q_str_len);
   if(sm_log_F3_buf.p_msg_link[sm_log_F3_buf.q_curr_buf_index] != NULL)
   {
     memscpy(sm_log_F3_buf.p_msg_link[sm_log_F3_buf.q_curr_buf_index],q_str_len, p_buffer, q_str_len);
     sm_log_F3_buf.w_heap_tracker = sm_log_F3_buf.w_heap_tracker + q_str_len;
     sm_log_F3_buf.q_curr_buf_index =  (sm_log_F3_buf.q_curr_buf_index+1) % SM_LOG_CIRC_BUF_F3_MAX;
   }

}

/*===========================================================================

FUNCTION sm_log_write_F3_to_circ_buf

DESCRIPTION
  This function will generate log of a particular format and push into buffer

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_log_write_F3_to_circ_buf(char* p_path_name, uint32 q_line, sm_log_args num_arg, int32 l_arg1, int32 l_arg2, int32 l_arg3, int32 l_arg4)
{
  sm_log_circ_buf_entry_timestamp z_timestamp = {0};

  /*Store timestamp used for final log generation*/
  char u_timestamp[SM_LOG_CIRC_BUF_TIME_STAMP_STR_LEN];

  /*store encodedfile name used for final log generation*/
  char u_enc_file_name[SM_LOG_CIRC_BUF_MAX_META_FILE_NAME_LEN];

  /* final log will be pushed into buffer*/
  char u_f3[SM_LOG_CIRC_BUF_F3_LEN_MAX];

  /*index of ff3 buffer*/
  uint8_t u_index;

  /* Fetch time stamp since 6 Jan 1980 00:00:00 */
  sm_log_circ_buf_get_timestamp(&z_timestamp);

  /*Generation of part of log "timestamp" */  
  snprintf(u_timestamp, sizeof(u_timestamp), "%02d:%02d:%02d:%03d",
          (int)z_timestamp.t_time_hr,
          (int)z_timestamp.t_time_min,
          (int)z_timestamp.t_time_sec,
          (int)z_timestamp.t_time_msec);

  /*fetch encoded file name from the full file path */ 
  sm_log_circ_buf_get_file_name(p_path_name, u_enc_file_name, SM_LOG_CIRC_BUF_MAX_META_FILE_NAME_LEN);

  /*Generation of part of log  --> "filename linename timestamp" */
  u_index= snprintf(u_f3, sizeof(u_f3), "%s %d %s", u_enc_file_name, q_line, u_timestamp);

  /*Generation of part of log  --> "filename linename timestamp arg1 arg2 arg3", end of log*/
  switch(num_arg)
  {
    case SM_LOG_CIRC_BUF_ARGS_NUM_1:
      u_index += snprintf((char *)(u_f3+u_index), 
                           sizeof(u_f3) - u_index, 
                           " %d", 
                           l_arg1);
      break;
    case SM_LOG_CIRC_BUF_ARGS_NUM_2:
      u_index += snprintf((char *)(u_f3+u_index), 
                           sizeof(u_f3) - u_index, 
                           " %d %d", 
                           l_arg1, l_arg2);
      break;
      
    case SM_LOG_CIRC_BUF_ARGS_NUM_3:
      u_index += snprintf((char *)(u_f3+u_index), 
                           sizeof(u_f3) - u_index, 
                           " %d %d %d", 
                           l_arg1, l_arg2, l_arg3);
      break;
      
    case SM_LOG_CIRC_BUF_ARGS_NUM_4:
      u_index += snprintf((char *)(u_f3+u_index), 
                           sizeof(u_f3) - u_index, 
                           " %d %d %d %d", 
                           l_arg1, l_arg2, l_arg3, l_arg4);
      break;
  }

  // Push log into the buffer 
  sm_log_circ_buf_push(u_f3);

}
#endif /*#ifdef FEATURE_BUFFER_F3*/

#ifdef FEATURE_CIRC_BUF_IPC
/*===========================================================================

FUNCTION sm_log_write_IPC_to_circ_buf

DESCRIPTION
    This function will generate log of a particular format and push into IPC buffer

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_log_write_IPC_to_circ_buf(uint32 q_arg)
{
  sm_log_circ_buf_entry_timestamp z_timestamp;
  
  /*Store timestamp which is used for final log generation*/
  char u_timestamp[SM_LOG_CIRC_BUF_TIME_STAMP_STR_LEN];

  /* Fetch time stamp since 6 Jan 1980 00:00:00*/
  sm_log_circ_buf_get_timestamp(&z_timestamp);

  /*Converting time stamp to format hr:min:sec:msec */
  snprintf(u_timestamp, sizeof(u_timestamp), "%02d:%02d:%02d:%03d",
          (int)z_timestamp.t_time_hr,
          (int)z_timestamp.t_time_min,
          (int)z_timestamp.t_time_sec,
          (int)z_timestamp.t_time_msec);

   memscpy(sm_log_IPC_buf.z_ipc_info[sm_log_IPC_buf.q_curr_buf_index].u_timestamp, SM_LOG_CIRC_BUF_TIME_STAMP_LEN, u_timestamp, SM_LOG_CIRC_BUF_TIME_STAMP_LEN);
   sm_log_IPC_buf.z_ipc_info[sm_log_IPC_buf.q_curr_buf_index].q_ipc = q_arg;
   sm_log_IPC_buf.q_curr_buf_index = (sm_log_IPC_buf.q_curr_buf_index + 1) % SM_LOG_CIRC_BUF_IPC_MAX;
}
#endif /*#ifdef FEATURE_CIRC_BUF_IPC*/


#ifndef SM_API_H
#define SM_API_H
/*------------------------------------------------------------------------------
  Copyright (c) 2009 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2020 Qualcomm Technologies, Inc.
  Confidential and Proprietary - Qualcomm Technologies, Inc.   All Rights Reserved.
                  
      Export of this technology or software is regulated by the U.S. Government.
      Diversion contrary to U.S. law prohibited.
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
File comment
------------------------------------------------------------------------------*/

/**
  @file sm_api.h 
    
  SM interface exposed to other modules

*/

/*------------------------------------------------------------------------------
Mainpage
------------------------------------------------------------------------------*/
/**
  @mainpage SM Public API
  
*/

/**
     @defgroup SM Session Manager
     
     TODO: Add info about what session manager does here
*/

/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/inc/sm_api.h#4 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/28/20   katta    Added new flag to indicate if wwan ranging is used to calculate fix
06/26/20   sai     Added QZSS default enablement based on config NV as part of FR63097
29/04/20   ska     Added changes for asking less proc duration when EPH is valid
10/17/17   rn      Added Xtra Integrity support. 
05/29/17   sum     Added two DOP fields TDOP and GDOP
08/10/16   mj      Handle Leap Second Info updates from ME
02/17/16   mj      Added a new bool u_CpiInconsistent into sm_GnssNavSolution to flag CPI inconsistencies 
02/02/16   mk      Add support for LPPM/PDR3.0 feature
09/23/15   hs      Unified fix reporting API for all fixes to be reported to SM
                   via this API
08/28/15   mj     Add support for INS only fixes for SAP changes
07/09/15   rh      Keep sm_UpdateCellIDInfo() API for SMART even with CM CID change  
07/03/15   jv      QZSS API Changes
06/04/15   jyw     Added CPE Timing Control
03/18/15   ss       Added function to report Wifi Pos fix response
09/15/14   jv      Galileo GNSS Support
08/08/14   jv      16-bit SVID support
07/24/14   jyw     Reserving IPC for CPE measurements 
07/18/14   jyw     Added API for CPE measurements
03/10/14   ah      Added support for Get Avail WWAN Pos
12/02/13   ah      Support GPS State Info requests for multiple clients simultaneously
10/28/13   hm      Added SM error event report for signal processing memory allocation failure
06/13/13   rh      Added sm_GnssNavSvInfo for SV info & sm_BdsAssistDataStruct
06/07/13   rh      Removed NO_FIX report and changed FIX report structure 
03/20/13  zhangwen Updated the IPCs in correspondence to payload changes in nofix/fix reports
01/28/13   mj      Updated sm_RegisterApiNotifier to include new param clientId
02/06/13   ah      Added support for BDS (BeiDou)
04/18/12   ssu     Adding sm_PositionReportStructType for GPS state information.
03/14/12   ss      Support of reset location serive.
02/27/12   rh      Added sm_SendEngineErrorRecoveryReport() API 
03/28/11   rh      Added heading uncertainty
04/28/10   kgc     Added bad health list to gnss assistance data. redfined ipc
02/12/10   vp      Added Sv polynomial function for OEM DRE enablement
06/10/09   gk      ODP 2.0 support
06/10/09   gk      Added Get UTC offset from NV
06/10/09   gk      ODP 1.1 support
01/12/09   atien   XTRA-T support
09/15/08   gk      Rename the on-demand struct to sm_ from cgps_04/27/08   gk      On_Demand Changes
04/17/08   jb      Allow changing IPC log with modified gps_NavSolutionExtra structure
08/01/07   rw      Add support for XTRA feature
03/27/07   mr      Removed positionflag #define s from this file
02/08/07   br      Added field to PPM report structure.
12/19/06   br      Clean-up.
12/01/06   jw      Modified sm_GpsMeasRptStructType to include gps_MeasBlkStructType
                   instead of redefining each field.
10/20/06   br      Exchanged include pdsmsrch.h with gps_1xl1_api.h.
07/03/06   mr      Changes MSG-IDs from macros to enums
04/04/06   sk      Added more definitions
03/20/06   ld      Initial version.

=============================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "gps_variation.h"
#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */
#include "gps_common.h"
#include "gps_rrc_common.h"
#include "gnss_common.h"
#include "gnss_consts.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "cgps_api.h"
#include "pdapi_lite.h"
#include "gnss_cqsvc.h"  /* For Engine Error Recovery Report */
#include "gnss_lpp_ecid.h" /* LPP ECID definitions */
#include "wl_common.h"
#include "slim_client_types.h"
#include "mgp_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/*=============================================================================

       Constants

=============================================================================*/
#define NUM_SOFT_DECISIONS 20

#ifdef FEATURE_SM_DEBUG
#error code not present
#else
#define SM_ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR( "SM: Assertion " #xx_exp " failed",0,0,0 ); \
     }
#endif /* FEATURE_SMLM_DEBUG */

#define SM_IS_RCVR_CONFIG_UPDATE_NEEDED(x)  (tm_core_is_rcvr_config_update_needed(x))

/*=============================================================================
   GNSS Measurement Report structure
=============================================================================*/
//  TODO: [an][del] clearing api first , in the next step clean this
typedef struct
{
  uint32                 e_MeasRptReason; /* gnss_MeasRptReasonEnumType */
  gnss_MeasBlkStructType z_MeasBlk;

} sm_GpsMeasRptStructType;


/**
* Structure containing Bias information for an individual sensor
**/
typedef struct
{
  uint16  w_SensorValidMask; /* Indicated which of the axes have valid data
                                Use SENSOR_BIAS_VALID_xxx macro to find which
                                of the axes have valid data*/
  FLT f_XaxisValue; /*  X-axis Bias
                      - Units gyroscope:(radians)/(seconds)
                      - Units accelerometer:(meters)/(seconds^2)*/
  FLT f_YaxisValue; /*  Y-axis Bias
                      - Units gyroscope:(radians)/(seconds)
                      - Units accelerometer:(meters)/(seconds^2)*/
  FLT f_ZaxisValue; /*  Z-axis Bias
                      - Units gyroscope:(radians)/(seconds)
                      - Units accelerometer:(meters)/(seconds^2)*/
} sm_SensorBiasValueStructType;

/**
 * On-device Sensor Bias data from PE
 */
typedef struct
{
  uint16                       w_SeqenceNumber; /* Incremental packet serial number starting from 0, to identify missing packets  */
  sm_SensorBiasValueStructType z_GyroBias;      /* Contains axis validity mask + x,y,z values for Gyro bias */
  sm_SensorBiasValueStructType z_AccelBias;     /* Contains axis validity mask + x,y,z values for accel bias */
} sm_SensorParamRptStructType;

//  TODO : [an][del] delete till here, from sm_nav some members go off
/*=============================================================================*/

/* SM-specific navigation position flags. Each field is binary. The TRUE/FALSE state is described below. */
typedef struct
{
  /* GPS position report validity flags */
  uint64 b_IsItarViolated      : 1;
  uint64 b_IsGpsTimeValid      : 1;
  uint64 b_IsPuncValid         : 1;
  uint64 b_IsTuncValid         : 1;

  /* Technology used in computing the position fix flags */
  uint64 b_IsSatellite         : 1;   /* Satellite is used for computing position fix */
  uint64 b_IsCellId            : 1;   /* Cell Id is used for computing position fix */
  uint64 b_IsWifi              : 1;   /* Wifi is used for computing postion fix */
  uint64 b_IsSensors           : 1;   /* Sensors is used for computing position fix */
  uint64 b_IsRefLoc            : 1;   /* Reference location is used for computing position fix */
  uint64 b_IsCoarsePos         : 1;   /* Coarse position is used for computing position fix */

  uint64 b_IsPosBestAvail      : 1;   /* Pos is best available position */
  uint64 b_IsSensorPropogatedGNSSFix : 1;   /* Solution used Only INS ONLY fixes */
  uint64 b_IsTdp               : 1;   /* TDP meas are used for computing position fix */
  uint64 b_IsPosInConsistent:1; /* Indication to SM is any inconsistency detected amongst
                               position in ALE database during a emergency session */
  uint64 b_IsPosFallBackGnss:1; /* Position is a fall back GNSS position and not necessarily best */
  uint64 b_IsWwanRanging    :1; /*WWAN RANGING meas/GTP2_0 is used for computing position fix*/
} sm_NavPosFlags;

/**   struct sm_NavPositionStructType describes the current position,
 *    velocity in the position engine. DO NOT change this
 *    structure without versioning the IPC log which use this
 *    struct.  Due to Unified Fix Reporting, only one IPC needs updating.
 *     - ARIES_MSG_ID_MGP_LM_REPORT_FIX
 * 
 *    This struct is derived from gps_NavSolution and differs
 *    only in that new fields have been added. this was done to
 *    help limit the changes cascading through the rest of SM.
 * 
 */
typedef struct
{
  /* Bit flags copied verbatim from gnss_NavSolutionStructType */
  gnss_NavSlnFlagsStructType z_PosFlags;

  /* Additional bit flags specific to SM */
  sm_NavPosFlags             z_SmPosFlags;

  /* GNSS Fix Time Information */
  uint16 w_GpsWeek;             /* GPS week number of fix */
  uint32 q_GpsTimeMs;           /* GPS time in week (milliseconds) of fix */
  uint8  u_GloNumFourYear;      /* Glonass four-year cycle number */
  uint16 w_GloNumDaysInFourYear; /* Glonass calendar day within the current four-year cycle */
  uint32 q_GloTimeMs;           /* Glonass time in day (milliseconds) */
  uint32 q_UtcTimeMs;           /* UTC time (milliseconds) adjusted for leap seconds */
  uint32 q_RefFCount;           /* Receiver's ms counter value */
  uint8  u_InitGpsTimeSrc;      /* Time source information (pdsm_liteextended_time_src_e_type) */

  /* GNSS Fix Position Information */
  uint32 q_PosCounter;          /* Position structure counter */
  DBL    d_PosEcef[3];          /* Earth Centered, Earth Fixed position fix (m) */
  DBL    d_PosLla[3];           /* Lat, Long, Alt */
  DBL    d_VelEcef[3];          /* ECEF velocity fix (m/s) */
  FLT    f_VelEnu[3];           /* East, North, Up velocity (m/s) */
  FLT    f_HeadingRad;          /* Heading angle, [0,2*pi] (rad). Referenced to North (0 rad) */
  FLT    f_HeadingUncRad;       /* Heading uncertainty (rad) */
  FLT    f_GnssHeadingRad;      /* Gnss Only heading */
  FLT    f_GnssHeadingUncRad;   /* Gnss Only heading uncertainty (rad) */

  FLT    f_Punc3dMeters;        /* Position standard deviation estimate (m) */
  FLT    f_PuncLatLonMeters[2]; /* Position (Lat, Long) uncertainty (m) */
  FLT    f_PuncVertMeters;      /* Vertical position uncertainty (m) */
  FLT    f_ErrorEllipse[3];     /* loc_uncrtnty_ang (degree), loc_uncrtnty_a (m), loc_uncrtnty_p (m) */
  FLT    f_Vunc3dMps;           /* Velocity uncertainty (m/s) */
  FLT    f_VuncEastNorthMps[2]; /* East, North velocity uncertainty (m/s) */
  FLT    f_VuncVertMps;         /* Vertical velocity uncertainty (m/s) */
  FLT    f_ClockBias;           /* Receiver's clock bias (m) */
  FLT    f_ClockBiasUncMs;      /* Receiver's clock uncertainty (m) */
  FLT    f_ClockDriftRate;      /* Receiver's clock drift (m/s) */
  FLT    f_ClockDriftRateUncMps; /* Receiver's clock frequency bias uncertainty (m/s) */
  gnss_PeTimeBiasStructType z_InterGnssTb[GNSS_TB_TOTNUM]; /* Inter-constellation Time Biases */
  FLT    f_FilteredAlt;         /* Filtered altitude (m) */
  FLT    f_FilteredAltUnc;      /* Filtered altitude uncertainty, one-sided (m) */
  FLT    f_UnfilteredAlt;       /* Unfiltered altitude (m) */
  FLT    f_UnfilteredAltUnc;    /* Unfiltered altitude uncertainty, one-sided (m) */
  FLT    f_MagDeviation;        /* Magnetic deviation from true north (degrees) */

  FLT    f_PDOP;                /* Position Dilution of Precision (unitless) */
  FLT    f_HDOP;                /* Horizontal DOP (unitless) */
  FLT    f_VDOP;                /* Vertial DOP (unitless) */
  FLT    f_TDOP;                /* Time DOP (unitless) */
  FLT    f_GDOP;                /* Geometrical DOP (unitless) */

  uint8  u_FirstFix;            /* Set TRUE when we have are first fix */
  uint8  u_DriftEstimated;      /* Set TRUE when the clock drift rate is estimated from
                                 * a difference of two clock biases */

  uint8  u_MagDevGood;          /* TRUE if magnetic devation is good */
  uint8  u_IonoCorrected;       /* FALSE indicates that not all SV measurements were iono corrected */
  uint8  u_TropoCorrected;      /* FALSE indicates that not all SV measurements were tropo corrected */

  pdsm_lite_pd_reliability_e_type e_HoriRelIndicator; /* horizontal reliability */
  pdsm_lite_pd_reliability_e_type e_VertRelIndicator; /* vertical reliability */

  uint16 w_SensorDataUsageMask;    /* A bit set to 1 indicates that data from the corresponding 
                                      sensor was used in order to compute the content (position,
                                      velocity, heading) in this packet.
                                      0x0001 - Accelerometer
                                      0x0002 - Gyro
                                      0xFFFC - Reserved
                                      Future versions of the GPS service may use reserved bits 
                                      to represent new sensor types. A control point should 
                                      ignore any bits that it considers to be reserved. */
  uint16 w_SensorAidingIndicator;  /* A bit set to to 1 indicates that certain fields (position, 
                                      velocity, heading, etc) in parsed position data TLV were 
                                      aided with sensor data.
                                      0x0001 - Heading aided with sensor data
                                      0x0002 - Speed aided with sensor data
                                      0x0004 - Position aided with sensor data
                                      0x0008 - Velocity aided with sensor data
                                      0FFF0  - Reserved
                                      Future versions of the GPS service may use reserved bits 
                                      to represent new methods of sensor aiding (e.g. tightly 
                                      coupled measurement combinations). A control point should 
                                      ignore any bits that it considers to be reserved. */
  boolean b_CpiInconsistent;       /* TRUE if a faulty CPI fix was previously sent to SM. 
                                      A faulty CPI is detected by PE using Reference Location sent by network */

  FLT f_TechContributions[TECHNOLOGIES_MAX]; /* Technology contributions.  */

  /* This structure is part of an IPC that needs to be updated before adding anything to it */
} sm_NavPositionStructType;


/**
     struct containing fix info from PE

     This structure is used by PE to notify SM of periodic fixes
   
     @see sm_ReportFix
*/

/* for backward/forward compatibility, in case some day we may need to make 
   SM_API & PDAPI's NavInfo structs different */
#define SM_GNSS_SV_INFO_MEAS_VALID       PDSM_LITE_GNSS_SV_INFO_MEAS_VALID
#define SM_GNSS_SV_INFO_USABLE           PDSM_LITE_GNSS_SV_INFO_USABLE
#define SM_GNSS_SV_INFO_USED_IN_POS_FIX  PDSM_LITE_GNSS_SV_INFO_USED_IN_POS_FIX
#define SM_GNSS_SV_INFO_USED_IN_VEL_FIX  PDSM_LITE_GNSS_SV_INFO_USED_IN_VEL_FIX

/* use PDAPI SVINFO structure diretly since PDAPI is now internal API */
typedef pdsm_lite_GnssNavSvInfo      sm_GnssNavSvInfo;
typedef pdsm_lite_GnssSvInfoType     sm_GnssSvInfoType;


/*=============================================================================
   GPS Assistance data status structure
=============================================================================*/
/* Bit-mask definitions for the Wish list */
#define C_GNSS_ASSIST_REFTIME  (1U<<0)      /// Reference Time (GPS or GLONASS)
#define C_GNSS_ASSIST_REFLOC   (1U<<1)      /// Reference Location
#define C_GNSS_ASSIST_SA        (1U<<3)     /// Sensitvity Assistance
#define C_GNSS_ASSIST_NAVMODL  (1U<<4)      /// Ephemeris a.k.a. NavigationModel
#define C_GNSS_ASSIST_ALM      (1U<<5)      /// Almanac
#define C_GNSS_ASSIST_IONO     (1U<<6)      /// IonosphericModel (Propogated signal delay)
#define C_GNSS_ASSIST_RTI      (1U<<7)      /// RealTimeIntegrity a.k.a satellite health info
#define C_GNSS_ASSIST_UTC      (1U<<8)      /// UTC Time info (used to relate with GPS or GLONASS system)
#define C_GNSS_ASSIST_TIME_MODELS (1U<<9)   /// Time model (GNSS-GNSS system time offset)
#define C_GNSS_ASSIST_AUXI_INFO   (1U<<10)  /// Auxiliary Info (Used in conjunction with NavModel)

/* the following are retained for backwards compatibility
   but their use is deprecated. Use C_GNSS_ASSIST_xxx flags
   instead 
*/
#define C_GPS_ASSIS_REFTIME      C_GNSS_ASSIST_REFTIME
#define C_GPS_ASSIS_REFLOC       C_GNSS_ASSIST_REFLOC
#define C_GPS_ASSIS_SA           C_GNSS_ASSIST_SA
#define C_GPS_ASSIS_NAVMODL      C_GNSS_ASSIST_NAVMODL
#define C_GPS_ASSIS_ALM          C_GNSS_ASSIST_ALM
#define C_GPS_ASSIS_IONO         C_GNSS_ASSIST_IONO
#define C_GPS_ASSIS_RTI          C_GNSS_ASSIST_RTI
#define C_GPS_ASSIS_UTC          C_GNSS_ASSIST_UTC

/* The data size required for single SV information in PQWP7 is
- DataType  : 1 digit
- DataSource: 1 digit
- Data age  : 11 digits (+/-int32_max sec range)
- 3 commas  : 3 bytes
This should multiplied by total number of SVs and adding 100 to accomodate for
header, time and sentence termination*/
#define C_DBUG_NMEA_PQWP7_BUFER_SIZE (16*(N_ALL_GNSS_SV-N_BDS_SV+N_BDS_SV_TOTAL)+100)


typedef struct
{
  uint16 w_Sv;                     /* SV PRN */
  uint16 w_Iode;                   /* Issue of data Ephemeris tag */

} sm_SvEphIodeStructType;

typedef struct
{
  FLT f_PosUnc;
  boolean u_Valid;
} sm_PosUncStructType;

typedef struct
{
  FLT f_TimeUnc;
  boolean u_Valid;
} sm_TimeUncStructType;

typedef struct
{
  uint16                 w_Week;     /* GPS Week of Ephemeris validity */
  uint16                 w_Toe;      /* Reference time of the latest Ephemeris */
  uint16                 w_ToeLimit; /* Ephemeris Age tolerance (hours) */

} sm_GpsEphTime;

typedef struct
{
  uint16                 w_N4;        /* GLONASS 4-Year count (N4)*/
  uint16                 w_Na;        /* GLONASS Day in 4-year cycle (Na) */
  uint16                 w_Tb;        /* Reference time of the latest Ephemeris: uinit: 15 minutes, range 0 to 95 */
  uint16                 w_ToeLimit;  /* Ephemeris Age tolerance, double sided. 
                                         1 = 30 min, 2 = 45 min, 3 = 60 min; 
                                         0 = reserved; other = invalid */
} sm_GloEphTime;

typedef struct
{
  uint16                 w_Week;     /* BDS Week of Ephemeris validity */
  uint16                 w_ToeLimit; /* Ephemeris Age tolerance (hours) */
  uint32                 q_Toe;      /* Reference time of the latest Ephemeris */
} sm_BdsEphTime;

typedef struct
{
  uint16                 w_Week;     /* GAL Week of Ephemeris validity */
  uint16                 w_ToeLimit; /* Ephemeris Age tolerance (hours) */
  uint32                 q_Toe;      /* Reference time of the latest Ephemeris */
} sm_GalEphTime;

typedef union
{
  sm_GpsEphTime z_GpsEphTime;
  sm_GloEphTime z_GloEphTime;
  sm_BdsEphTime z_BdsEphTime;
  sm_GalEphTime z_GalEphTime;
  /* add other constellation times here */
} sm_GnssEphTime;

typedef struct
{
  boolean                u_EphInfoIncl;     /**< TRUE if EPH information is included */
  uint8                  u_NumSvs;          /**< Number of SVs for which IODE info is provided */
  uint32                 q_WishListMask;    /**< Bit-mask indicating the type of data needed */
  uint32                 q_EphHaveList;     /**< Bit-mask of SVs for which Ephemeris is available */
  uint32                 q_EphNeedList;     /**< Bit-mask of SVs for which Ephemeris is needed */
  uint32                 q_AlmHaveList;     /**< Bit-mask of SVs for which Almanac is available */
  uint32                 q_AlmNeedList;     /**< Bit-mask of SVs for which Almanac is needed */
  uint32                 q_VisibleSVList;   /**< Bit-mask of all visible SVs*/
  uint32                 q_SvNoExistMask;   /**< Bit-mask of SVs which are non-existent */
  sm_GnssEphTime         z_AssistDataTime;  /**< time associated with navigation data */
  sm_TimeUncStructType   z_TimeUnc;         /**< Time uncertainty information */
  sm_PosUncStructType    z_PosUnc;          /**< Horizontal position uncertainty */
  uint32                 q_HealthHaveList;  /**< Good list */
  uint32                 q_HealthNeedList;  /**< Unknown */
  uint32                 q_HealthBadList;   /**< Bad SVs */
  sm_SvEphIodeStructType z_SvEphIode[N_SV];
} sm_GnssAssistDataStruct;

typedef struct
{
  boolean                u_EphInfoIncl;     /**< TRUE if EPH information is included */
  uint8                  u_NumSvs;          /**< Number of SVs for which IODE info is provided */
  uint32                 q_WishListMask;    /**< Bit-mask indicating the type of data needed */
  uint64                 t_EphHaveList;     /**< Bit-mask of SVs for which Ephemeris is available */
  uint64                 t_EphNeedList;     /**< Bit-mask of SVs for which Ephemeris is needed */
  uint64                 t_AlmHaveList;     /**< Bit-mask of SVs for which Almanac is available */
  uint64                 t_AlmNeedList;     /**< Bit-mask of SVs for which Almanac is needed */
  uint64                 t_VisibleSVList;   /**< Bit-mask of all visible SVs*/
  uint64                 t_SvNoExistMask;   /**< Bit-mask of SVs which are non-existent */
  sm_GnssEphTime         z_AssistDataTime;  /**< time associated with navigation data */
  sm_TimeUncStructType   z_TimeUnc;         /**< Time uncertainty information */
  sm_PosUncStructType    z_PosUnc;          /**< Horizontal position uncertainty */
  uint64                 t_HealthHaveList;  /**< Good list */
  uint64                 t_HealthNeedList;  /**< Unknown */
  uint64                 t_HealthBadList;   /**< Bad SVs */
  sm_SvEphIodeStructType z_SvEphIode[N_BDS_SV];
} sm_BdsAssistDataStruct;

typedef struct
{
  boolean                u_EphInfoIncl;     /**< TRUE if EPH information is included */
  uint8                  u_NumSvs;          /**< Number of SVs for which IODE info is provided */
  uint32                 q_WishListMask;    /**< Bit-mask indicating the type of data needed */
  uint64                 t_EphHaveList;     /**< Bit-mask of SVs for which Ephemeris is available */
  uint64                 t_EphNeedList;     /**< Bit-mask of SVs for which Ephemeris is needed */
  uint64                 t_AlmHaveList;     /**< Bit-mask of SVs for which Almanac is available */
  uint64                 t_AlmNeedList;     /**< Bit-mask of SVs for which Almanac is needed */
  uint64                 t_VisibleSVList;   /**< Bit-mask of all visible SVs*/
  uint64                 t_SvNoExistMask;   /**< Bit-mask of SVs which are non-existent */
  sm_GnssEphTime         z_AssistDataTime;  /**< time associated with navigation data */
  sm_TimeUncStructType   z_TimeUnc;         /**< Time uncertainty information */
  sm_PosUncStructType    z_PosUnc;          /**< Horizontal position uncertainty */
  uint64                 t_HealthHaveList;  /**< Good list */
  uint64                 t_HealthNeedList;  /**< Unknown */
  uint64                 t_HealthBadList;   /**< Bad SVs */
  sm_SvEphIodeStructType z_SvEphIode[N_GAL_SV];
} sm_GalAssistDataStruct;

typedef struct
{
  boolean                 u_EphInfoIncl;     /**< TRUE if EPH information is included */
  uint8                   u_NumSvs;          /**< Number of SVs for which IODE info is provided */
  uint32                  q_WishListMask;    /**< Bit-mask indicating the type of data needed */
  uint32                  q_EphHaveList;     /**< Bit-mask of SVs for which Ephemeris is available */
  uint32                  q_EphNeedList;     /**< Bit-mask of SVs for which Ephemeris is needed */
  uint32                  q_AlmHaveList;     /**< Bit-mask of SVs for which Almanac is available */
  uint32                  q_AlmNeedList;     /**< Bit-mask of SVs for which Almanac is needed */
  uint32                  q_VisibleSVList;   /**< Bit-mask of all visible SVs*/
  uint32                  q_SvNoExistMask;   /**< Bit-mask of SVs which are non-existent */
  sm_GnssEphTime          z_AssistDataTime;  /**< time associated with navigation data */
  sm_TimeUncStructType    z_TimeUnc;         /**< Time uncertainty information */
  sm_PosUncStructType     z_PosUnc;          /**< Horizontal position uncertainty */
  uint32                  q_HealthHaveList;  /**< Good list */
  uint32                  q_HealthNeedList;  /**< Unknown */
  uint32                  q_HealthBadList;   /**< Bad SVs */
  sm_SvEphIodeStructType  z_SvEphIode[N_QZSS_SV];
} sm_QzssAssistDataStruct;





typedef struct
{
  uint32                   q_WishListMask;   /**< Bit-mask indicating the type of data needed */
  uint32                   q_EphHaveList;    /**< Bit-mask of SVs for which Ephemeris is available */
  uint32                   q_EphNeedList;    /**< Bit-mask of SVs for which Ephemeris is needed */
  uint32                   q_AlmHaveList;    /**< Bit-mask of SVs for which Almanac is available */
  uint32                   q_AlmNeedList;    /**< Bit-mask of SVs for which Almanac is needed */
  uint32                   q_VisibleSVList;  /**< Bit-mask of all visible SVs*/
  uint32                   q_SvNoExistMask;  /**< Bit-mask of SVs which are non-existent */
  boolean                  u_EphInfoIncl;    /**< TRUE if EPH information is included */
  uint16                   w_GpsWeek;        /**< GPS Week of Ephemeris validity */
  uint16                   w_Toe;            /**< Reference time of the latest Ephemeris */
  uint16                   w_ToeLimit;    /**< Ephemeris Age tolerance (hours) */
  uint8                    u_NumSvs;      /**< Number of SVs for which IODE info is provided */
  sm_TimeUncStructType     z_TimeUnc;       /**< Time uncertainty information */
  sm_PosUncStructType      z_PosUnc;        /**< Horizontal position uncertainty */
  sm_SvEphIodeStructType   z_SvEphIode[N_SV];
  uint32                   q_HealthHaveList;
  uint32                   q_HealthNeedList;
  uint32                   q_HealthNoExList;
  uint32                   q_HealthBadList;

  sm_GnssAssistDataStruct  z_GloAssistData; /**< glonass assist data status structure */
  sm_BdsAssistDataStruct   z_BdsAssistData; /**< BDS assist data status structure */
  sm_GalAssistDataStruct   z_GalAssistData; /**< GAL assist data status structure */
  sm_QzssAssistDataStruct  z_QzssAssistData; /**< QZSS assist data status structure */
  uint8                    u_Valid; /**< which assistance data is valid. 
                                                  Bit 0 for gps,
                                                  bit 1 for Glonass
                                                  bit 2 for BDS
                                                  bit 3 for Galileo
                                                  bit 4 for QZSS
                                                  bit set=valid, not-set=invalid.
                                                  Use SM_ASSIST_DATA_VALID_xxx */
} sm_InternalAssistDataStatusStructType;
#define SM_ASSIST_DATA_VALID_MASK_GPS          ((uint16)(0x01<<0))
#define SM_ASSIST_DATA_VALID_MASK_GLONASS      ((uint16)(0x01<<1))
#define SM_ASSIST_DATA_VALID_MASK_BDS          ((uint16)(0x01<<2))
#define SM_ASSIST_DATA_VALID_MASK_GAL          ((uint16)(0x01<<3))
#define SM_ASSIST_DATA_VALID_MASK_QZSS         ((uint16)(0x01<<4))

typedef struct
{
  uint16                   w_Valid; /**< which assistance data is valid. 
                                                  Bit 0 for gps,
                                                  bit 1 for Glonass
                                                  bit 2 for BDS
                                                  bit 3 for GAL
                                                  bit 4 for QZSS
                                           bit set=valid, not-set=invalid.
                                           Use SM_ASSIST_DATA_VALID_xxx */

  sm_GnssAssistDataStruct    z_GpsAssistData;   /**< gps assistance data */
  sm_GnssAssistDataStruct    z_GloAssistData;   /**< glonass assistance data */
  sm_BdsAssistDataStruct     z_BdsAssistData;   /**< BDS assistance data */
  sm_GalAssistDataStruct     z_GalAssistData;   /**< GAL assistance data */
  sm_QzssAssistDataStruct    z_QzssAssistData;  /**< QZSS assistance data */
} sm_GnssAssistDataStatusStructType;


/*=============================================================================
   Pilot Phase Measurement Report structure
=============================================================================*/
typedef struct
{
  int32 l_PilotPnPhase;
  uint8 u_PilotRmse;
  uint8 u_PilotEcio;

} sm_PilotInfoStructType;

/** UTC information
 * 
 * UTC data decoded by PE off the air to SM. Since both GLONASS
 * and GPS times are consistent, leap second conversion is
 * unnecessary and the same API may be used. Refer MGP TO SM API
 * documentation for details
 * 
 */
typedef struct
{
  uint32 q_A0;      /* UTC: convert from GPS time to UTC time. */
  uint32 q_A1;
  uint8  u_DeltaTls;
  uint8  u_Tot;
  uint8  u_WNt;
  uint8  u_WNlsf;
  uint8  u_DN;
  uint8  u_DeltaTlsf;

} sm_UtcInfoRptStructType;

/*=============================================================================
   Receiver State Change event
=============================================================================*/
/**
     Receiver State Change event

     ME reports receiver state change and associated gpsRtc
*/
typedef struct
{
  uint32                   q_GpsRtcMs;
  gps_RcvrStateEnumType e_CurrentRcvrState;
  gnss_RcvrSubStateStructType z_currentRcvrSubState;
} sm_RcvrStateChangeStructType;

/**
     SM Event Report type

     ME and PE report back to SM when one of these events
     happens
*/
typedef enum
{
  /**< Event type */                 /**< When generated */
  SM_EVENT_MIN = 0,
  SM_EVENT_ME_TIME_TICK,          /**< Periodically in Receiver IDLE/ON modes */
  SM_EVENT_ME_NEW_MEAS,           /**< Periodically, if new measurements are obtained */
  SM_EVENT_ME_ASSIGN_CHANGE,      /**< Processing mode changes in ME */
  SM_EVENT_ME_CELL_CHANGE,        /**< When serving cell changes */
  SM_EVENT_ME_TIME_TRANSFER,      /**< New time-transfer took place. Cell changed or periodic update on current cell */
  SM_EVENT_ME_TIME_UPDATE,        /**< Clock update in ME, better (or forced) estimate received */
  SM_EVENT_ME_TIME_DISCONTINUITY, /**< ME detected a time discontinuity */
  SM_EVENT_ME_TIME_ERR_DETECT,    /**< ME detected an error in its clock estimate */
  SM_EVENT_ME_MEAS_SUSPENDED,     /**< Measurements are suspended (resources pre-empted) */
  SM_EVENT_ME_MEAS_ABORTED,       /**< Measurements are aborted (resources lost completely for the session) */
  SM_EVENT_ME_SA_DATA_NEEDED,     /**< ME needs SA data to improve the sensitivity */
  SM_EVENT_PE_NEW_NAVDATA,        /**< PE received new Ephemeris */
  SM_EVENT_PE_NEW_ALMANAC,        /**< PE received new Almanac */
  SM_EVENT_PE_NEW_HEALTH,         /**< PE received new SV Health info */
  SM_EVENT_PE_NEW_IONO_UTC,       /**< PE received new Iono/UTC info */
  SM_EVENT_PE_DEL_NAVDATA,        /**< Ephemeris data is deleted */
  SM_EVENT_PE_DEL_ALMANAC,        /**< Almanac data is deleted */
  SM_EVENT_PE_STITCH_FIRST_BP,    /**< PE has started Stitch back propogation */
  SM_EVENT_PE_STITCH_INCOMPLETE_BP, /**< PE has suspended Stitch back propogation */
  SM_EVENT_PE_STITCH_LAST_BP,     /**< PE has completed Stitch back propogation */
  SM_EVENT_PE_KF_RESET_OR_INIT,   /**< KF has been reset or initialized */
  SM_EVENT_PE_LOG_KF,             /**< KF data has been logged */
  SM_EVENT_ME_IQ_DONE,            /**< IQ Collect/FFT done */
  SM_EVENT_PE_NO_NEW_FIX,         /**< PE no new fix generated */
  SM_EVENT_PE_LOG_ALM_CORR,       /**< XTRA alm corr have been logged */
  SM_EVENT_ME_SIG_PROC_MEM_ALLOC_ERROR, /** Error event to indicate Session mode allocation failure for Signal Processing memory*/
  SM_EVENT_MAX

} sm_EventEnumType;

/*=============================================================================
   Structure for reporting events to SM
=============================================================================*/
typedef struct
{
  sm_EventEnumType e_EventType;
  uint32           q_EventData;
  uint32           q_EventTime;
} sm_ReportEventStructType;

typedef union
{
  cgps_1xRfInfoType   CDMARfInfo;
  cgps_EVDORfInfoType EVDORfInfo;
} sm_ReportRfInfoUnionType;

typedef struct
{
  gps_DataSourceType       RfInfoSource;
  sm_ReportRfInfoUnionType RfInfoData;
} sm_ReportRfInfoStructType;

/**
 * Time report from MGP/PE
 * 
 * Delivers estimate of current time to SM. PE calls this API in
 * response to a request from SM via mgp_ReqGnssTime
 */
typedef struct
{
  gps_TimeChkSourceType source;
  gnss_ClockStructType  gnssClock;
} sm_ReportGnssTimeStructType;


/**
 * SM_GEN7_COMPATIBILITY(sm_ReportGpsTimeStructType)
 * sm_ReportGpsTimeStructType is being retained for backwards
 * compatibility. Gen7 MGP APIs call this to report time.
 * this however, may be removed at a later date since the
 * data actually comes from PE.
 * See sm_ReportGnssTime in MGP to SM api documentation
 */
typedef struct
{
  gps_TimeChkSourceType    source;
  gps_ClockStructType      GpsClock;
} sm_ReportGpsTimeStructType;

/**
     Latest position and associated uncertainties

     This structure is used by ME to indicate the position and
     its associated uncertainties.
   
     @see sm_UpdatePosPuncTunc
*/
typedef struct cgps_OnDemandPosPuncTunc
{
  //  sm_GpsFixRptStructType pos_info; /* null if no valid position is available */
  boolean b_Valid; /**< specifies if the pos punc/tunc are valid */
  FLT     f_Tunc; /**< in us */
  FLT     f_Punc; /**< in meters */
  uint32  eph_svmask; /**< SV Mask for the ephemeris */
  uint32  alm_svmask; /**< SV Mask for the Almanac */
  uint32  health_svmask; /**< SV Mask for the health */
  uint32  health_bad_svmask; /**< SV Mask for the health */
  uint32  health_noex_svmask; /**< SV Mask for the health */
  uint32  health_unk_svmask; /**< SV Mask for the health */
  uint32  visible_svmask; /**< SV mask for visible SVs*/
} sm_OnDemandPosPuncTuncStruct;

typedef struct
{
  /* GPS L1 C/N0 results */
  FLT f_GpsL1CNoDBHz;  /* Signal strength calculated in 
                          dBHz as part of WBIQ test*/
  FLT f_GpsL1FreqHz; /* Freq in Hz calculated as part of WBIQ test */

  /* GLO G1 C/N0 results */
  FLT f_GloG1CNoDBHz;  /* Signal strength calculated in 
                           dBHz as part of WBIQ test*/
  FLT f_GloG1FreqHz; /* Freq in Hz calculated as part of WBIQ test */

  /* BDS B1 C/N0 results */
  FLT f_BdsB1CNoDBHz;  /* Signal strength calculated in 
                          dBHz as part of WBIQ test*/
  FLT f_BdsB1FreqHz; /* Freq in Hz calculated as part of WBIQ test */

  /* GAL E1 C/N0 results */
  FLT f_GalE1CNoDBHz;  /* Signal strength calculated in 
                          dBHz as part of WBIQ test*/
  FLT f_GalE1FreqHz; /* Freq in Hz calculated as part of WBIQ test */

  uint8 u_GnssConfig;    /* Flags containing configured 
                            constellations */

  /* ADC mean/amp in units of 0.1 mV */
  int32 l_AdcMeanIp1mV; /* Mean Estimate (DC) of I component in 
                       ADC processor in 0.1 mV */
  int32 l_AdcMeanQp1mV; /* Mean Estimate (DC) of Q component in 
                       ADC processor in 0.1 mV */
  uint32 q_AdcAmpIp1mV; /* Amplitude estimate of I component in 
                       ADC processor in 0.1 mV */
  uint32 q_AdcAmpQp1mV; /* Amplitude estimate of Q component in 
                       ADC processor in 0.1 mV */

  uint8  u_AdcIqSatPerStatus;  /* Indicates validity/status of data.
                                  0 - ADC IQ saturation percentage is not valid
                                  3 – Number of samples in ADC IQ test was zero
                                  4 – ADC IQ saturation percentage is valid */
  uint32 q_ISatPer; /* Percentage of I samples saturated, in units of 0.001 */
  uint32 q_QSatPer; /* Percentage of Q samples saturated, in units of 0.001 */
  int32  l_AdcIqWbJammerPwrEstDb; /* Estimated wideband jammer power in units of 0.01 dB.
                                     Only valid if wideband jammer power estimation was enabled in the 
                                     ADC IQ test request.*/
  int32  l_GpsL1NoiseFloorEstDb; /* GPS Noise floor estimate over 2 MHz in units of 0.01 dB */
  int32  l_GloG1NoiseFloorEstDb; /* GLO Noise floor estimate over 10 MHz in units of 0.01 dB */
  int32  l_BdsB1NoiseFloorEstDb; /* BDS Noise floor estimate over 4 MHz in units of 0.01 dB */
  int32  l_GalE1NoiseFloorEstDb; /* GAL Noise floor estimate over 4 MHz in units of 0.01 dB */
  int32  l_GloG1FreqNoiseFloorEstDb[N_GLO_FREQ]; /* GLO Noise floor estimate 
                                                  for each 1 MHz in units of 0.01 dB */
  
  uint32 q_MultiBandConfig; /* Flags containing configured constellations.
                               C/N0 results are valid for the configured constellations.  
                               Bit 9 – GPS L5
                               Bit 10 – GAL E5a */

  /* GPS L5 / GAL E5a C/N0 results */
  FLT f_GpsL5GalE5aCNoDBHz;  /* Signal strength calculated in 
                          dBHz as part of WBIQ test*/
  FLT f_GpsL5GalE5aFreqHz; /* Freq in Hz calculated as part of WBIQ test */
} sm_ReportWBIQInfoStructType;

typedef struct {
  int32   l_RefOffsetPpb;        /* XO offset in PPB */
  int32   l_ClkFreqBiasPpb;      /* Clock frequency bias in PPB */
  int32   l_ClkFreqBiasUncPpb;   /* Clock frequency uncertainty in PPB */
  boolean v_IsDataValid;         /* Data validity */
}sm_ReportXOOffsetInfoStructType;

/* This structure is used to pass on NBIQ data pointer
   and size of NBIQ data to SM from MGP */
typedef struct
{
  void *p_NBIQDataPtr;
  uint32 q_NBIQDataSize;
} sm_ReportNBIQInfoStructType;


/* This structure is used to pass on BP Amp and 
   PGA gain information from MGP to SM. This can be extended
   later on if OEM wants other GNSS RF related data */
typedef struct
{
  int32   l_PGAGain;

  /* BP1 params */
  uint32  q_Bp1LbwAmplI;
  uint32  q_Bp1LbwAmplQ;

  /* BP3 GLO processor status */
  uint32  q_Bp3GloAmplI;
  uint32  q_Bp3GloAmplQ;
} sm_ReportGNSSRfStatusInfoStructType;

#define SM_GPS_STATE_POS_VALID 0x0001
#define SM_GPS_STATE_TIME_VALID 0x0002
#define SM_GPS_STATE_PUNC_VALID 0x0004
#define SM_GPS_STATE_TUNC_VALID 0x0008
#define SM_GPS_STATE_FUNC_VALID 0x0010
#define SM_GPS_STATE_FCLKDRT_VALID 0x0020
#define SM_GPS_STATE_CLKBIAS_VALID 0x0040

typedef enum
{
  /*Get GPS State Info Req */
  SM_GPS_STATE_INFO_REQ_TYPE_NONE                      = 0,
  SM_GPS_STATE_INFO_REQ_TYPE_MIN                       = 1,
  SM_GPS_STATE_INFO_REQ_TYPE_ON_DEMAND                 = SM_GPS_STATE_INFO_REQ_TYPE_MIN, /* deprecated */
  SM_GPS_STATE_INFO_REQ_TYPE_QUERY_STATE               = 2,
  SM_GPS_STATE_INFO_REQ_TYPE_DIAG_POS                  = 3, /* deprecated */
  SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_TECH_SEL = 4,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI    = 5,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_XTRA_T = 6,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI     = 7,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_GNSS_AVAIL_POS_QMI     = 8,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_REFLOC_AVAIL_POS_QMI     = 9,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_COARSE_AVAIL_POS_QMI     = 10,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_WIFI_AVAIL_POS_QMI     = 11,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_GILE_AVAIL_POS_QMI     = 12,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_DBH                   = 13,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_GFC     = 14,
  SM_GPS_STATE_INFO_REQ_TYPE_GET_GTP_WWAN_FIX          = 15,
  /****ADD ALL GPS STATE INFO REQUESTORS ABOVE THIS.
           AND ANY OTHER KIND OF REQUESTS AT THE END OF THE NUM and update MAX
   ****/
  SM_GPS_STATE_INFO_REQ_TYPE_MAX                        = SM_GPS_STATE_INFO_REQ_TYPE_GET_GTP_WWAN_FIX,

  /*Wifi fix request*/
  SM_REQ_TYPE_WIFI_POS_FOR_MIN                          = SM_GPS_STATE_INFO_REQ_TYPE_MAX + 1,
  SM_REQ_TYPE_WIFI_POS_FOR_BLE                          = SM_REQ_TYPE_WIFI_POS_FOR_MIN,
  SM_REQ_TYPE_WIFI_POS_FOR_MAX                          = SM_REQ_TYPE_WIFI_POS_FOR_BLE,

  /* Position Estimation request */
  SM_REQ_TYPE_GET_POS_EST                               = SM_REQ_TYPE_WIFI_POS_FOR_MAX + 1,
  SM_REQ_TYPE_MAX                                       = SM_REQ_TYPE_GET_POS_EST
} sm_ReqEnumType;

typedef enum
{
  SM_LOC_EST_SRC_GPS  = 1,
  SM_LOC_EST_SRC_WIFI  = 2
}sm_LocEstimateSrcEnumType;

typedef struct sm_GpsStateInfo
{
  sm_ReqEnumType  b_ReqType;
  uint32                      q_Flags;      /* flags to indicate which fields are valid */
  uint8                       e_PosInfoSrc; /* Source of the Position Estimate */
  DBL                         d_PosLLa[3];
  FLT                         f_AltUnc;     /* Altitude Uncertainty in meters */
  FLT                         f_Tunc;       /* in us */
  FLT                         f_Punc;       /* in meters */

  uint16                      w_GpsWeek;    /* GPS Week time reference for the Position */
  uint32                      q_GpsMsec;    /* GPS Msec time reference for the Position */
  FLT                         f_Func;       /* 1 sided max freq bias uncertainty [m/s] */
  FLT                         f_ClkDrift;   /* Clock Drift in meters/sec */
  FLT                         f_ClkBias;    /* Clock frequency bias in meters/sec */
} sm_GpsStateInfoStruct;

/* Fix Auxiliary into type enum that indicates what element of the union 
sm_FixAuxInfoStructType is reported to SM */
typedef enum
{
  SM_AUXINFO_TYPE_NONE = 0,
  SM_AUXINFO_TYPE_WIFI,
  SM_AUXINFO_TYPE_TDP,
} sm_FixAuxInfoTypeEnumType;

/* LP GTP-Wifi Auxiliary info structure */
typedef struct
{
  wl_ResultFlagsType  z_WifiResult;
} sm_WifiAuxInfoStructType;

/* Unified fix reporting Auxliary information */
typedef union
{
  sm_WifiAuxInfoStructType z_WifiAuxInfo;
} sm_FixAuxInfoStructType;

typedef struct
{
  sm_NavPositionStructType    z_NavPos;     /* position-related info */

  sm_GnssNavSvInfo            z_NavSvInfo;  /* SV-related info */

  sm_SensorParamRptStructType z_OnDeviceSensorParam; /* sensor-related info */
} sm_FixInfoStructType;

//DELME
typedef sm_FixInfoStructType   sm_GpsFixRptStructType;


/* Unified fix reporting structure. ALE SHALL always report all fixes to any SM component
via a common API with this payload */
typedef struct
{
  sm_ReqEnumType b_ReqType;      /* Source of the request for best available position */
  sm_FixInfoStructType       z_FixInfo;     /* position-related info */
  sm_FixAuxInfoTypeEnumType  z_FixAuxInfoType; /* Type that indicates what element of the below union is
                                            going to be reported */
  sm_FixAuxInfoStructType    z_FixAuxInfo; /* Auxiliary info for the reported fix is available */
} sm_FixReportStructType;

#ifdef FEATURE_CGPS_XTRA_T
/* Structure for transferring cell data from MC to SM */
typedef struct
{
  uint8  u_PartNumber; /* current part number of this structure */
  uint8  u_TotalParts; /* total number of parts in overall transfer */
  uint32 q_DataSize;   /* number of bytes of data pointed to by p_DataPtr */
  void *p_DataPtr;    /* pointer to byte buffer containing cell data */
} sm_ReportCellDataStructType;
#endif /* FEATURE_CGPS_XTRA_T */

typedef pdsm_pd_ext_var_len_debug_nmea_s_type sm_DebugNMEAVarLen;

typedef struct
{
  uint8    u_Buffer[MAX_NMEA_SENTENCE_LEN]; /* debug NMEA payload */
  uint32   dwLength;    /* length of payload including the null termination */
  uint32   dwReserved;  /* reserved for future use */
} sm_DebugNMEA;

typedef struct
{
  uint16            w_Prn;
  gps_EphStructType z_GpsEphemeris;
} sm_GpsEphemeris;

typedef struct
{
  uint16            w_SlotId;
  glo_EphStructType z_GloEphemeris;
} sm_GloEphemeris;

typedef struct
{
  uint16            w_Prn;
  bds_EphStructType z_BdsEphemeris;
} sm_BdsEphemeris;

typedef struct
{
  uint16            w_Prn;
  gal_EphStructType z_GalEphemeris;
} sm_GalEphemeris;

/*---------------------------------------------------------------------------
                      GPS Presc Dwell LOG TYPE
---------------------------------------------------------------------------*/
/* -------------------------------------------------------------------
** NAME : LOG_GPS_PRESCRIBED_DWELL_RESULT
**
** DESCRIPTION : It is logged everytime there's presc dwell result avaialable.
**
** PERIODICITY : Async. Depends on dwell duration.
** ------------------------------------------------------------------- */

typedef PACKED struct PACKED_POST
{
  uint8  u_Version;
  uint8  u_RespType; /* 0 Dwell Results
                      * 1 Incoh Sums
                      * 2 Coh Sums
                      * 3-255 Reserved
                      */

  uint32  q_TotalPackets;
  uint32  q_CurrPacketNum;
} sm_PrescDwellCommon;

typedef PACKED struct PACKED_POST
{
  uint16  w_Sv;
  uint32  q_GpsRtc;
  uint8   u_Flags;
  uint8   u_SrchModeIndex;
  uint16  w_PostDetN;
  uint16  w_CnoDb;
  uint32  f_SubMs;
  uint32  f_DopplerFreq;
  uint32  f_MultipathEst;
  uint32  q_Reserved0;
} sm_PrescDwellResult;

/* Final struct defining the DM log packet */
LOG_RECORD_DEFINE(LOG_GPS_PRESCRIBED_DWELL_RESULT_C)

sm_PrescDwellCommon z_PrescDwellCommon;
sm_PrescDwellResult z_PrescDwellResult;
LOG_RECORD_END

   typedef LOG_GPS_PRESCRIBED_DWELL_RESULT_C_type sm_PrescDwellLogType;



/*---------------------------------------------------------------------------
                      GPS DEMOD SOFT DECISIONS LOG TYPE
---------------------------------------------------------------------------*/

/* -------------------------------------------------------------------
** NAME : log_gps_demod_soft_decisions_type
**
** DESCRIPTION : It is logged everytime GPS searcher accumulates
** 20 soft DPSK decisions from firmware.
**
** PERIODICITY : every 20 bits which is approximately 400ms for each
**   active channel.
** ------------------------------------------------------------------- */

LOG_RECORD_DEFINE(LOG_GPS_DEMOD_SOFT_DECISIONS_C)

uint8 u_VersionId;        /* The version of this log type */

uint16 w_Sv;               /* SV PRN number */

uint16 w_DemodId;         /* This ID will be used to correlate across demod-
                          ** related logs that come from the same channel. It
                          ** is incremented everytime any channel is newly started.
                          ** It has a different purpose than SV PRN number which
                          ** one might think can be used, but if let's say a
                          ** channel is aborted due to tuneback, we want to
                          ** distinguish between data coming from channel before
                          ** tuneback or after tuneback (since they have the same
                          ** SV PRN number) */

uint32 q_BitId;           /* This ID will be used to enumerate this log, it
                          ** serves two purposes:
                          ** - to detect dropped packet of this log
                          ** - in addition to task id, to further correlate
                          **   this log with other demod related logs
                          ** It represents how many GPS bits have passed since
                          ** the start of the current demod session */

/* Soft DPSK decisions straight from firmware, untruncated */
int16 x_SoftDecisions[NUM_SOFT_DECISIONS];

uint8 u_NumBits;          /* Number of bits in the array */

LOG_RECORD_END

   typedef LOG_GPS_DEMOD_SOFT_DECISIONS_C_type log_gps_demod_soft_decisions_type;

/* for engine error recovery report, QMI header file gnss_cqsvc.h uses it
   original struct defined in position_determination_service_v01.h  */
typedef  cqsvc_engine_err_recovery_rpt_ind_msg gnss_engine_err_recovery_rpt;
typedef void (*sm_ApiInitNotifierCb)
   (
   void
   );

typedef pdsm_lite_pd_ext_dpo_status_rpt sm_DpoStatusReportStructType;


/*---------------------------------------------------------------------------
                      LPPM Status Report
---------------------------------------------------------------------------*/
#define SM_LPPM_GNSS_TYPE_MAX (MGP_GNSS_TYPE_GAL + 1)

typedef struct {
  mgp_GnssType e_ConstellationType;
  /**<   Constellation type.
      Valid Values :
      - MGP_GNSS_TYPE_GPS (0) --  Constellation type is GPS.
      - MGP_GNSS_TYPE_GLO (1) --  Constellation type is GLONASS.
      - MGP_GNSS_TYPE_BDS (2) --  Constellation type is Beidou.
      - MGP_GNSS_TYPE_GAL (3) --  Constellation type is Galileo.    */

  uint32 q_RequiredSvOrbitalModelBitMask;
  /**<   List of required SVs' orbital model (bit mask), one bit per-SV.
       Valid values for each of 32 bits:
         0 -- SV's orbital model not required.
         1 -- SV's orbital model required.
  */

  uint32 q_AvailableSvOrbitalModelBitMask;
  /**<   List of available SVs' orbital model (bit mask), one bit per-SV.
       Valid values for each of 32 bits:
         0 -- SV's orbital model not available.
         1 -- SV's orbital model available.
  */
} sm_LppmConstellationDataStructType;

/** LPPM status structure */
typedef struct {

  /* Flag to indicate if LPPM can engage.
     When already engaged, this flag will always return TRUE. */
  boolean v_EngagePossible;

  /* Current LPPM State */
  gnss_RcvrLppmStateEnumType e_State;
  /**<   LPPM state.
  Valid Values :
      - GNSS_RCVR_STATE_LPPM_NONE (0) -- disabled.
      - GNSS_RCVR_STATE_LPPM_WAITING_CLOCK (1) -- enabled but waiting for clock.
      - GNSS_RCVR_STATE_LPPM_DISENGAGED (2) -- enabled and disengaged.
      - GNSS_RCVR_STATE_LPPM_INIT (3) -- enabled and engaged in initialization mode.
      - GNSS_RCVR_STATE_LPPM_HIGH_UNC (4) -- enabled and engaged with high uncertainty.
      - GNSS_RCVR_STATE_LPPM_LOW_UNC (5) -- enabled and engaged with low uncertainty. */

  /*  Time Uncertainty */
  float f_TimeUncMs;
  /**<   Time uncertainty.
       - Type: Floating point
       - Units: Milliseconds */

  /*  Constellation Data For LPPM */
  uint32 q_ConstellationDataLen;  /**< Must be set to # of elements in constellationData */
  sm_LppmConstellationDataStructType z_ConstellationData[SM_LPPM_GNSS_TYPE_MAX];
} sm_LppmStatusStructType;

/*************************************************************************
  GNSS LE Reporting Data-Structures
*************************************************************************/
/** GNSS LE Reporting Enum */
typedef enum
{
  /*GNSS LE reporting Off*/
  SM_GNSS_LE_RPT_OFF,
  /*GNSS LE reporting On*/
  SM_GNSS_LE_RPT_ON,
  SM_GNSS_LE_RPT_MAX
} sm_GnssLEReportingEnumType;

/** GNSS LE Reporting Error Code Enums */
typedef enum
{
  /* No error */
  SM_GNSS_LE_RPT_STATUS_ERROR_NONE,
  /* MGP session is inactive */
  SM_GNSS_LE_RPT_STATUS_ERROR_SESSION_INACTIVE,
  /* GNSS LE reporting already On */
  SM_GNSS_LE_RPT_STATUS_ERROR_ALREADY_ON,
  SM_GNSS_LE_RPT_STATUS_ERROR_MAX
}sm_GnssLERptStatusErrorEnum;

/**GNSS LE Reporting Status Structure*/
typedef struct {
  sm_GnssLEReportingEnumType e_State;
  sm_GnssLERptStatusErrorEnum e_Error;
} sm_GnssLERptStatusStructType;

/*************************************************************************
  Sensor LE Data-Structures
*************************************************************************/

/** Sensor Relative Displacement Status */
typedef enum
{
  /* Relative Displacement is Valid*/
  SM_SENSOR_REL_DISP_STATUS_SUCCESS,
  /*Relative Displacement is not valid due to an Error*/
  SM_SENSOR_REL_DISP_STAUS_ERROR,
  SM_SENSOR_REL_DISP_STATUS_MAX = CGPS_MAX_UINT32
}sm_SensorLERelDispStatusEnum;

/** Sensor LE Relative Displacement Struct */
typedef struct
{
  /*Modem timestamp of the begining of the diplacement interval*/
  uint64 t_ModemTsFromMsec;
  /*Modem timestamp of the end of the diplacement interval*/
  uint64 t_ModemTsToMsec;
  /*Displacement Status Enum*/
  sm_SensorLERelDispStatusEnum e_Status;
  /*North, East and Vertical Displacement in meters
    [0]: North Displacement
    [1]: East Displacement
    [2]: Vertical Displacement*/
  FLT f_EnuDisp[3];
  /*Displacement Error
    [0]: Displacement error ellipse semi-major axis. Units: meters
    [1]: Displacement error ellipse semi-minor axis. Units: meters
    [2]: Displacement error ellipse angle. Units: Radians*/
  FLT f_DispError[3];
  /*Displacement Vertical Uncertainity*/
  FLT f_DispVertUnc;
}sm_SensorLERelDispStructType;

/** Sensor LE State Enum */
typedef enum
{
  /*Sensor LE State Off*/
  SM_SENSOR_LE_STATE_OFF,
  /*Sensor LE State On*/
  SM_SENSOR_LE_STATE_ON,
  SM_SENSOR_LE_STATE_MAX
} sm_SensorLEStateEnumType;

/** Sensor LE Status Error Code Enums */
typedef enum
{
  /* No error */
  SM_SENSOR_LE_STATUS_ERROR_NONE,
  /* MGP session is inactive */
  SM_SENSOR_LE_STATUS_ERROR_SESSION_INACTIVE,
  /* Sensor LE already On */
  SM_SENSOR_LE_STATUS_ERROR_ALREADY_ON,
  /* Sensor LE insufficient memory */
  SM_SENSOR_LE_STATUS_ERROR_NO_MEMORY,
  SM_SENSOR_LE_STATUS_ERROR_MAX
}sm_SensorLEStatusErrorEnum;

/**Sensor LE State Structure*/
typedef struct {
  sm_SensorLEStateEnumType e_State;
  sm_SensorLEStatusErrorEnum e_Error;
} sm_SensorLEStatusStructType;

typedef struct

{
  uint32 q_EfsStatus;      /*File operation status*/
  uint8  u_FileOp;
  uint8  u_Filename[PDSM_LITE_PD_FS_MAX_PATH_LEN_BYTES];   //Changed from GPS_FS_MAX_PATH_LEN due to compilation error.
} sm_GpsFsTaskNotificationDataType;

typedef enum
{
  /**< Constellation type */
  SM_XTRA_CONSTEL_TYPE_GPS = 0,    /**< Constellation type GPS */
  SM_XTRA_CONSTEL_TYPE_GLO,        /**< Constellation type GLO */
  SM_XTRA_CONSTEL_TYPE_BDS,        /**< Constellation type BDS */
  SM_XTRA_CONSTEL_TYPE_GAL,        /**< Constellation type GAL */
  SM_XTRA_CONSTEL_TYPE_QZSS,       /**< Constellation type QZSS */
  SM_XTRA_CONSTEL_TYPE_MAX
} sm_XtraConstelEnumType;

typedef enum
{
  SM_LFM_FEATURE_MIN      =0,
  SM_LFM_FEATURE_GNSS_SERVICE  =1,      /*GNSS service enable/disabled*/
  SM_LFM_FEATURE_GTP_WWAN   =2,         /*GTP WWAN service enable/disable*/
  SM_LFM_FEATURE_MAX,
}sm_LFMFeatureEnumType;

typedef struct
{
  sm_LFMFeatureEnumType e_featureType;
  boolean v_status;
}sm_LFMConfigUpdateStructType;

/*-------------------------------------------------------------------------------*/
//  TODO: [an][del] this section needs to e deleted

/**
  @brief  This function sends the GNSS SV poly report calculated by
          PE to SM. GNSS SV poly reports are sent periodically
          once MGP is turned ON

  @return
  @sideeffects None
  @see

*/
void sm_ReportSVPolynomialGNSS(const gnss_SvPoly *p_GnssSvPolyRpt);


/*=============================================================================

FUNCTION
  sm_ReportDrSyncPulseDescriptor

DESCRIPTION
  

DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportDrSyncPulseDescriptor(gnss_DRSyncPulseDescType *pDrSyncPulseDesc);


/*=============================================================================

FUNCTION
  sm_ReportDrSyncConfig

DESCRIPTION


DEPENDENCIES
  None.

PARAMETERS


RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportDrSyncConfig(gnss_DRSyncConfigType *pDrSyncConfig);

/*===========================================================================
  sm_ReportPedSensorAssistAvailInd

  Description:

     This function is used by MGP to report Pedestrian Sensor Assistance
     availibility to SM.

 Parameters:

   v_PedSensorAssistAvailable: whether Pedestrian Sensor Assistance is available or not

  Return value:
    void

=============================================================================*/
void sm_ReportPedSensorAssistAvailInd(boolean v_PedSensorAssistAvailable);

/*===========================================================================
  sm_ReportGnssQualInd

  Description:

     This function is used by MGP to report GNSS Quality indications to SM.

 Parameters:

   e_GnssQualityInd:  GNSS Quality indication enum

  Return value: 
    void

=============================================================================*/
void sm_ReportGnssQualInd(const gnss_QualIndEnumType e_GnssQualityInd);

/*=============================================================================

FUNCTION
  sm_UpdateCPETimingControl

DESCRIPTION
  This updates SM about the CPE timing control that is read by NV

DEPENDENCIES
  None.

PARAMETERS
  q_CPEControl - CPE Control Value

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_UpdateCPETimingControl(uint32 q_CPEControl);

/*=============================================================================

       Functions

=============================================================================*/

/**
 * This function sends the GNSS Measurements obtained by ME to
 * SM. PS Measurements are sent out periodically for NMEA
 * sentence generation and for diagnostic reasons (periodicity
 * is set by SM). They are also sent when measurement processing
 * is complete in 1x MS-Assisted mode of operation. The
 * measurement status report indicates the reason for sending
 * the measurements.
 * @param e_MeasRptReason
 * @param meas_ptr
 */
void sm_ReportGnssMeasInfo(gnss_MeasRptReasonEnumType    e_MeasRptReason,
                           const gnss_MeasBlkStructType *meas_ptr);

/*=============================================================================

FUNCTION
  sm_ReportGpsEphemeris

DESCRIPTION
  This function sends a GPS ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_Prn    - PRN of ephemeride being returned
  pz_GpsEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGpsEphemeris(const uint16 w_Prn, const gps_EphStructType *pz_GpsEph);


/*=============================================================================

FUNCTION
  sm_ReportGloEphemeris

DESCRIPTION
  This function sends a GLO ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_SlotId  - Slot ID of ephemeride being returned
  pz_GloEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGloEphemeris(const uint16 w_SlotId, const glo_EphStructType *pz_GloEph);


/*=============================================================================

FUNCTION
  sm_ReportBdsEphemeris

DESCRIPTION
  This function sends a BDS ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_Sv      - SV ID of ephemeride being returned
  pz_BdsEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportBdsEphemeris(const uint16 w_Sv, const bds_EphStructType *pz_BdsEph);


/*=============================================================================

FUNCTION
  sm_ReportGalEphemeris

DESCRIPTION
  This function sends a GAL ephemeride from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  w_Sv      - SV ID of ephemeride being returned
  pz_GalEph - Pointer to the ephemeris information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGalEphemeris(const uint16 w_Sv, const gal_EphStructType *pz_GalEph);
/*===========================================================================
  sm_ReportPedAlignAvailInd

  Description:

     This function is used by MGP to report Pedestrian Alignement availibility to SM.

 Parameters:

   v_PedAlignSensorAvailable: whether Pedestrian alignement is available or not

  Return value: 
    void

=============================================================================*/
void sm_ReportPedAlignAvailInd(boolean v_PedAlignSensorAvailable);

/*===========================================================================
  sm_ReportMgpPedStatus

  Description:

     This function is used by MGP to report Pedestrian status to SM.

 Parameters:

   e_PedStatus:  Pedestrian status enum

  Return value: 
    void

=============================================================================*/
void sm_ReportMgpPedStatus(gnss_MgpPedStatusEnumType e_PedStatus);

/*
******************************************************************************
* sm_GloStringPut
*
* Function description:
*
*  This function is called by PE to provide a decoded GLONASS string to PE.
*
* Parameters:
*
*  pz_Str -  input, Pointer to a GLONASS string
*
* Return value:
*
*  void
*
* ******************************************************************************
*/
void sm_GloStringPut(const gnss_GloStringStruct *pz_Str);

/*
******************************************************************************
* sm_SvGalPagePut
*
* Function description:
*
*    This function is called by PE to provide a decoded Galileo Page to PE.
*
* Parameters:
*
*  p_SvSframe -  Pointer to SV GAL Page structure
*
* Return value:
*
*  void
*
* ******************************************************************************
*/
void sm_SvGalPagePut(const gnss_GalPageStruct *p_SvGalPage);

/*
******************************************************************************
* sm_SvSframePut
*
* Function description:
*
*    This function is called by PE to provide a decoded subframe to PE.
*
* Parameters:
*
*  p_SvSframe -  Pointer to SV subframe structure
*
* Return value:
*
*  void
*
* ******************************************************************************
*/
void sm_SvSubframePut(const gnss_SvSubframeStruct *p_SvSubframe);


/*===========================================================================
  sm_ReportPedDevContext

  Description:

     This function is used by MGP to report Pedestrian Device Context to SM.

 Parameters:

   e_PedDevCtx:  Pedestrian Device Context enum

  Return value: 
    void

=============================================================================*/
void sm_ReportPedDevContext(const slimPedDevCxtTypeEnumT e_PedDevCtx);

/*=============================================================================

FUNCTION
  sm_ReportGnssLEStatus

DESCRIPTION
  This function is to send GNSS LE fix reporting status to SM

DEPENDENCIES
  None.

PARAMETERS
  p_GnssLERptStatus -Pointer to GNSS LE Reporting Status struct

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGnssLEStatus(sm_GnssLERptStatusStructType *p_GnssLERptStatus);

/*=============================================================================

FUNCTION
  sm_ReportSensorLEStatus

DESCRIPTION
  This function is to report Sensor LE State Change

DEPENDENCIES
  None.

PARAMETERS
  p_SensorLEStatus -Pointer to Sensor LE Status struct

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportSensorLEStatus(const sm_SensorLEStatusStructType *const p_SensorLEStatus);

/*=============================================================================

FUNCTION
  sm_ReportSensorLERelDisplacement

DESCRIPTION
  This function is to report Sensor Relative Displacement from Sensor LE

DEPENDENCIES
  None.

PARAMETERS
  p_RelDisp -Pointer to Relative Displacement Struct.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportSensorLERelDisplacement(const sm_SensorLERelDispStructType *const p_RelDisp);


/*===========================================================================
  sm_loc_lte_ecid_recv_measurements

  Description:

     This function is used by LOC_LTE_ECID module to send the ECID measurements
 Parameters:
    Reason: stop reason

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_ecid_recv_measurements(
   gnss_lpp_ecid_meas_info_type *p_ecid_measurement_info);


/**
* @brief  ME calls this function to request SA (assistance) data
*         from the network
  
* @return      void
  @sideeffects None
  @see


*/
void sm_RequestSAData(void);

/**
* @brief  PE indicates to SM the status of current assistance
*         data and wish-list using this API
  
  @return    
  @sideeffects None
  @see 

*/
void sm_ReportGnssAssistDataStatus(const sm_GnssAssistDataStatusStructType *p_AssistDataInfo);

//TODO  [an][del]  delete till here
/*-------------------------------------------------------------------------------*/

/**
  @brief   This function sends the GPS Measurements obtained by ME to SM.

  This function sends the Receiver State change event report to
  SM
  
  @return      
  @sideeffects None
  @see         
*/
void sm_ReportRcvrStateChange(
   uint32 q_GpsRtcMs,
   /**< GNSS RTC msec at which this report was generated.  */
   gps_RcvrStateEnumType e_CurRcvrState,
   /**< new receiver state */
   const gnss_RcvrSubStateStructType *pz_CurRcvrSubState
   /**< new receiver sub-state */
   );

/**
  @brief  This function sends the GPS Fix report calculated by
          PE to SM. GPS FIX/NO-FIX reports are sent periodically
          once MGP is turned ON. 
          This function is also used to send BestAvail Pos, Wifi fix,
          Pos estimatin, etc to SM.
  
  @return      
  @sideeffects None
  @see 

*/
void sm_ReportFix(const sm_FixReportStructType *p_FixRpt
                  /**< Pointer to the structure containing the GPS Fix data */);

/*=============================================================================

FUNCTION
  sm_ReportUtcInfo

DESCRIPTION
  This function sends the UTC data decoded by PE off the air to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_UtcInfo - Pointer to the structure containing the UTC info

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportUtcInfo
(
   const sm_UtcInfoRptStructType *p_UtcInfo
   );

/**
  @brief  ME and PE typically inform SM about events of interest by calling this function.
          SM may call other APIs in response to an event of interest
  
  @return      
  @sideeffects None
  @see         
*/
void sm_ReportEvent(const sm_ReportEventStructType *p_ReportEvent
                    /**< payload denoting event of interest */);

/**
  @brief  This function sends RF Info data from PGI to SM
  
  @return      
  @sideeffects None
  @see
  @todo add more meaningful parameter info
*/
void sm_ReportRfInfo(const sm_ReportRfInfoStructType *p_ReportRfInfo
                     /**< Pointer to the structure containing RF Info data. */);

/*=============================================================================

FUNCTION
  sm_ReportWBIQInfo

DESCRIPTION
  This function sends Wide Band IQ Info data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportWBIQInfo - Pointer to the structure containing WBIQ params

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportWBIQInfo(const sm_ReportWBIQInfoStructType *p_ReportWBIQInfo);

/*=============================================================================

FUNCTION
  sm_ReportNBIQInfo

DESCRIPTION
  This function sends Narrow Band IQ Info data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportNBIQInfo - Pointer to the structure containing WBIQ params

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportNBIQInfo(const sm_ReportNBIQInfoStructType *p_ReportNBIQInfo);


/*=============================================================================

FUNCTION
  sm_ReportGNSSRfStatusInfo

DESCRIPTION
  This function sends GNSS RF status Info data from MC to SM.

DEPENDENCIES
  None.

PARAMETERS
  p_ReportGNSSRfInfo - Pointer to the structure containing GNSS RF status
  information 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGNSSRfStatusInfo(const sm_ReportGNSSRfStatusInfoStructType *p_ReportGNSSRfInfo);

#if 1 /* APIs called from PE */
/*=============================================================================

FUNCTION
  sm_ReportInjDone

DESCRIPTION
  Indication from CD that xtra or xtra integrity injection was completed 
  successfully. 

DEPENDENCIES
  None.

PARAMETERS
gnss_XtraInjMaskType: The mask to indicate the updated constellations.
uint8               : 0 for XTRA or 1 for XTRA Integrity.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportInjDone( gnss_XtraInjMaskType q_AcceptedMask, 
                       uint8 u_XtraInjectType );
#endif /* #if 1 */
/**
  @brief  Delivers estimate of current time to SM. PE calls this
          API in response to a request from SM via
          mgp_ReqGnssTime (to be defined by MGP).
  
  @return  
  @sideeffects None
  @see 

*/
void sm_ReportGnssTime(const sm_ReportGnssTimeStructType *p_ReportGnssTime/**< gnss time struct */);


/*===========================================================================
FUNCTION sm_send_message

DESCRIPTION
  Function to send an IPC message to SM threads

RETURN VALUE
 ipc_msg_ptr = If IPC message was send successfully

DEPENDENCIES
  None

=============================================================================*/

os_IpcMsgType* sm_send_message(uint32 msg_id, uint32 size, uint32 thread, const void *data);

/*=============================================================================
FUNCTION
  sm_ReportPrescDwellResult

DESCRIPTION
  This function gets called when a presc dwell log is available.

DEPENDENCIES
  None.

PARAMETERS
  p_PrescDwellResult - Pointer to the structure containing presc dwell result

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportPrescDwellResult(const sm_PrescDwellLogType *p_PrescDwellResult);

/*=============================================================================

FUNCTION
  sm_GetGpsUtcOffset

DESCRIPTION
  This function gets UTC offset configured in the NV.

DEPENDENCIES
  None.

PARAMETERS
  p_UtcOffset - Pointer to the UTC offset value

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void  sm_GetGpsUtcOffset(uint8 *p_UtcOffset);


/*=============================================================================

FUNCTION
  sm_ReportDebugNMEA

DESCRIPTION
  Accepts formatted proprietary NMEA string for debug purpose.
  NMEA string is sent as IPC to the LM task by this API

DEPENDENCIES
  None.

PARAMETERS
  sm_DebugNMEA

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportDebugNMEA(sm_DebugNMEA *pDebugNMEA);

/*=============================================================================

FUNCTION
  sm_RegisterApiNotifierCb

DESCRIPTION
  Called by potential clients to register their callbacks. The callbacks are 
  used to notify the clients that the API/Engine is now ready to accept clients

  If the API/Engine is ready to accept clients, the callback will be called 
  inline.  The caller should be ready to handle this and queue an IPC to itself. 

DEPENDENCIES
  None.

PARAMETERS
  pNotifierCb - The callback of the client to be registered

RETURN VALUE
  TRUE:  registration has succeeded, or if Engine is ready the callback is 
         called immediately;
  FALSE:  registration failed

SIDE EFFECTS
  None.

=============================================================================*/
boolean sm_RegisterApiNotifier(sm_ApiInitNotifierCb pNotifierCb, os_ThreadIdEnumType clientThreadId);


/*=============================================================================

FUNCTION
  sm_ReportXtraInjDone

DESCRIPTION
  Indication from CD that xtra injection completed successfully. 

DEPENDENCIES
  None.

PARAMETERS
q_AcceptedMask:The mask to indicate the updated constellations.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportXtraInjDone(gnss_XtraInjMaskType q_AcceptedMask);


/*=============================================================================
  
FUNCTION

  sm_SendEngineErrorRecoveryReport

DESCRIPTION
  ME will call this API to send engine error recovery report to SM so it'll be
  forwarded to QMI by calling gnss_CQmiSvcSendEngineErrorRecoveryReport()
  No sanity check will be performed for the error recovery report from ME

DEPENDENCIES
  None.

PARAMETERS
 None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_SendEngineErrorRecoveryReport(gnss_engine_err_recovery_rpt *pErrReport);

/*=============================================================================

FUNCTION
  sm_DpoStatusReport

DESCRIPTION
  Function to report periodic DPO status report.
  When DPO status reporting is enabled by the application, this function
  will be called at a nominal rate of 1 Hz. 

DEPENDENCIES
  None.

PARAMETERS
  Information about DPO disengagement from MGP to SM . 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_DpoStatusReport(sm_DpoStatusReportStructType *p_DpoStatusReport);


/*===========================================================================
  sm_InitGnssNavPosition

  Description:

     Initalizes a sm_NavPositionStructType safely

 Parameters:

   pz_Pos:  pointer to the struct to initialize

  Return value: 
    boolean: true if successfully initialized

=============================================================================*/
boolean sm_InitGnssNavPosition(sm_NavPositionStructType *pz_Pos);

/*===========================================================================
  sm_ReportMgpLppmStatus

  Description:

     This function is used by MGP to report LPPM status to SM.

 Parameters:

   pz_LppmStatus:  pointer to the LPPM status struct

  Return value: 
    void

=============================================================================*/
#if 0
void sm_ReportMgpLppmStatus(const sm_LppmStatusStructType *pz_LppmStatus);
#endif
/*=============================================================================

FUNCTION
  sm_ReportLeapSecondInfo

DESCRIPTION
  This function sends a Leap Second Info from ME to SM.

DEPENDENCIES
  None.

PARAMETERS
  mgp_LeapSecondInfoStructType - Pointer to leap second info

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportLeapSecondInfo(const mgp_LeapSecondInfoStructType *pz_LeapSecInfo);

/*=============================================================================

FUNCTION
  sm_NavConfigChangeResp

DESCRIPTION
  This function sends MGP's response to the Nav config change diag command.

DEPENDENCIES
  None.

PARAMETERS
  u_CurrGnssConfig - Current GNSS configuration.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_NavConfigChangeResp(const uint8 u_CurrGnssConfig);

/*=============================================================================

FUNCTION
  sm_ReportNMEAForDebug

DESCRIPTION
  Accepts formatted proprietary NMEA string for debug purpose.
  NMEA string is sent as IPC to the TM task by this API, for forwarding to HLOS
  for debugging purpose.

DEPENDENCIES
  None.

PARAMETERS
  sm_DebugNMEA

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportNMEAForDebug(sm_DebugNMEA *pDebugNMEA);

#ifdef FEATURE_LOCTECH_NHZ
/*===========================================================================
  sm_NhzConfigPut

  Description:

     Get the NHz config from SM

 Parameters:

   pq_NhzConfig:  pointer to the config

  Return value: 
    None

=============================================================================*/
void sm_NhzConfigPut(uint32 *pq_NhzConfig);

/*===========================================================================
  sm_NhzDefaultConfigPut

  Description:  Get the NHz default config from SM
  
  Parameters :  None

  Return value: Default config

=============================================================================*/
uint32 sm_NhzDefaultConfigPut();
#endif /* FEATURE_LOCTECH_NHZ */

/*=============================================================================

FUNCTION
  sm_ReportXOOffsetInfo

DESCRIPTION
  This function sends XO offset data to SM.

DEPENDENCIES
  None.

PARAMETERS

  l_RefOffsetPpb - Ref osc offset in PPB
  l_ClkFreqBiasPpb - Clock freq bias in PPB
  l_ClkFreqBiasUncPpb - Clock freq bias uncertainty in PPB

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportXOOffsetInfo(int32 l_RefOffsetPpb,
                           int32 l_ClkFreqBiasPpb,
                           int32 l_ClkFreqBiasUncPpb);

/*=============================================================================

FUNCTION
  sm_ReportVarLenNMEAForDebug

DESCRIPTION
  Accepts formatted proprietary NMEA string for debug purpose.
  NMEA string is sent as IPC to the TM task by this API, for forwarding to HLOS
  for debugging purpose.

DEPENDENCIES
  None.

PARAMETERS
  sm_DebugNMEA

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportVarLenNMEAForDebug(sm_DebugNMEAVarLen const *const cpz_DebugNMEA);

/*=============================================================================

FUNCTION
  sm_ReportGpsASBit

DESCRIPTION
  This function is used to send AS Bit value from GPS SV decode.

DEPENDENCIES
  None.

PARAMETERS
  cu_ASBitSv - AS bit indicates simulated GPS. Value 1 means AS bit was decoded to be 0 (which would
  indicate from GPS Simulator) in the last subframe.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGpsASBit(uint8 const cu_ASBitSv);

/*=============================================================================
 FUNCTION
  sm_GetXtraConstelType

DESCRIPTION
  This function used to get the constellation type for which the XTRA download  
  is needed. XTRA shall be requested for GPS and the other constellation 
  returned by this API. 

DEPENDENCIES
  None.

PARAMETERS
  pe_ConstelType: The constellation type for which XTRA is needed. 
  This value shall be other than GPS . 

RETURN VALUE
  TRUE if the constellation type is determined. 
  FALSE if the constellation type cannot be determined.

SIDE EFFECTS
  None.
=============================================================================*/
boolean sm_GetXtraConstelType(sm_XtraConstelEnumType *pe_ConstelType);


/*=============================================================================
 FUNCTION
  sm_GpsFsTaskNotification

DESCRIPTION
  This function is a callback function used to report back EFS operation status.

DEPENDENCIES
  None.

PARAMETERS
  q_FileOperationResult - EFS operation status code 
  u_PathFilename[] - File name 
  u_FileOp - File operation 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void sm_GpsFsTaskNotification(uint32 q_FileOperationResult, 
                              const uint8 u_PathFilename[], 
                              uint8 u_FileOp);




/*=============================================================================
 FUNCTION
  sm_GetXtraConstelType

DESCRIPTION
  This function used to get the constellation type for which the XTRA download  
  is needed. XTRA shall be requested for GPS and the other constellation 
  reyurned by this API. 

DEPENDENCIES
  None.

PARAMETERS
  pe_ConstelType: The constellation type for which XTRA is needed. 
  This value shall be other than GPS . 

RETURN VALUE
  TRUE if the constellation type is determined. 
  FALSE if the constellation type cannot be determined.

SIDE EFFECTS
  None.
=============================================================================*/
boolean sm_GetXtraConstelType(sm_XtraConstelEnumType *pe_ConstelType);

/*===========================================================================

FUNCTION tm_core_get_curr_sec_gnss_constellation()

DESCRIPTION
  This function provides the gnss constellation config to be used 
  for navigation.

DEPENDENCIES

RETURN VALUE  gnss constellation ID

SIDE EFFECTS

===========================================================================*/
uint8 tm_core_get_curr_sec_gnss_constellation();

/*===========================================================================

FUNCTION tm_core_get_curr_gnss_constellation_config()

DESCRIPTION
  This function provides the gnss constellation currently being used, along 
  with GPS, for navigation.

DEPENDENCIES

RETURN VALUE  gnss constellation ID

SIDE EFFECTS

===========================================================================*/
uint32 tm_core_get_curr_gnss_constellation_config(void);

/* !!!!!!!!!!!!!!!!!!!!!!APIs called from PE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
void sm_MgpDeInitAckStatus(boolean v_AckStatus);
void sm_MgpInitAckStatus(boolean v_AckStatus);
void sm_DeleteXtraData(void);
void sm_DeleteXtraIntegrity(void);
/* !!!!!!!!!!!!!!!!!!!!!!APIs called from PE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

/*===========================================================================

FUNCTION sm_RcvrInUsStatus()

DESCRIPTION
  This function will be called to say if receiver is inside/outside US.
  Called every second and only when PE generated fixes are available.

DEPENDENCIES

RETURN VALUE  

SIDE EFFECTS

===========================================================================*/
void sm_RcvrInUsStatus(boolean v_RcvrInUS);

/*=============================================================================

FUNCTION
  sm_ReportCPEMeas

DESCRIPTION
  Report CPE meas from ME 

DEPENDENCIES
  None.

PARAMETERS
 p_CPEMeas - Pointer to CPE Measurement Block

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportCPEMeas(gnss_CPEMeasBlockStructType *p_CPEMeas);

/*=============================================================================

FUNCTION
  sm_ReportGnssEphExpTime

DESCRIPTION
  This function sends the GNSS ephemeris time to be expired from PE to SM.

DEPENDENCIES
  None.

PARAMETERSa
  pz_GnssEphAvailInfo - Pointer to the ephemeris expire time information

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void sm_ReportGnssEphExpTime(const gnss_EphExpTimeStructType *pz_GnssEphAvailInfo);

/*===========================================================================

FUNCTION tm_core_is_qzss_enabled()

DESCRIPTION
  This function returns True if QZSS is enabled in config mask

DEPENDENCIES

RETURN VALUE  
  True if QZSS enabled else False

SIDE EFFECTS

===========================================================================*/

boolean tm_core_is_qzss_enabled(void);

#ifdef __cplusplus
}
#endif
#endif /* SM_API_H */



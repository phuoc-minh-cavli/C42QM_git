#ifndef TM_COMMON_H
#define TM_COMMON_H

#include "gps_variation.h"

/*===========================================================================

                  TM_COMMON Header File

DESCRIPTION
  This header file contains definitions of common data structure necessary for
  TM module.

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2019 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
 
  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/prot/tm_common.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/19   vg      Implement system unc specific session duration
10/09/17   ak      Add TM_PHONE_EVENT_E911_CALL_CONNECT.
09/15/17   sum     Added function tm_util_calculate_offset_print
02/21/17   kpn     Support of 1x CP on Sub2
01/24/17   kpn     Support E911 CP session on non-DD sub
11/04/16   muk     Enable QZSS meas and Poly data
04/04/16   mj      Add GERA support
10/01/15   rk      Define all position protocols.
08/27/15   pa      Removing unsued Leap Second constants 
06/18/15   jv      Added tm_util_clean_qzss_sbas_meas_rpt()
05/21/15   nl      Refactor code
01/07/15   jv      QZSS Support
12/04/12   rk      CR 408753 - Update tm_lr_resp_type struct to include SV extn params.
07/31/12   mjohn   Removed NMEA_GPSLEAP_SECOND and GPS_UTC_LEAP_SECS
02/16/12   gk      Altitude also reported in float now
11/23/11   ssu     Added GFC_QMI feature.  
06/16/11   rh      Added TM_PHONE_EVENT_OTASP_STATUS_COMMITTED event
01/03/11   gk      Eph throttling rework
11/03/10   rh      Added horizontal/vertical velocity uncertainty calc routines
03/03/10   ns      DSDS Support
12/2/09    gk      intermediate position fixes
09/01/09   gk      Added a check to validate the directory for EFS operation
12/24/08    ns     Added timer for QWip System determination  
11/06/08   atien   NMEA_GPSLEAP_SECOND, GPS_UTC_LEAP_SECS  update to 15 seconds
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
04/25/08    gk     On-demand changes
02/06/08    cl     Added accommodating "forced TM termination".
08/01/07    rw     Add support for XTRA feature
08/02/07    LT     Added a utility function for handling SVs non-existence case.
07/30/07    ank    Added support for ext_pos_data in Position Report.
07/17/07    lt     L1 interface.
06/27/07    lt     Added the passing of WCDMA in-service to UMTS submodules.
06/15/07    ank    Added TM_MAX_SESSION_TIMEOUT.
05/25/07    lt     Added SMG_IDs for MM and RRC; Added two utility functions.
05/09/07    ank    Changed prototype of tm_util_pd_info_data_init
02/01/07    lt     Corrected a typo in the constant of TM_UMTS_UP_TRANSACTION_ID_RANGE
12/22/06    ank    Added support for MSBased Throttling feature.
07/12/06    cl     Initial Release
===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */

#include "aries_os_globals.h"
#include "aries_os_api.h"
#include "mgp_api.h"
#include "sm_api.h"
#include "pdapi_lite.h"
#include "gps_consts.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Definitions used to obtain gps weeks and milliseconds within the week. */
#define MINUTES_PER_WEEK    ( 7 * 24 * 60 )
#define MS_PER_MINUTE       ( 60 * 1000 )
#define MS_PER_WEEK         ( MINUTES_PER_WEEK * MS_PER_MINUTE )
#define SEC_PER_WEEK        (60*60*24*7)

#define TENTH_MSECS         ((uint32)100)
#define SEC_MSECS           ((uint32)1000)
#define MIN_MSECS           ((uint32)60*SEC_MSECS)
#define HOUR_MSECS          ((uint32)60*MIN_MSECS)

/* Define hex negative one in 32bits*/
#define HEX_NEGATIVE_ONE 0xFFFFFFFF

/* Gpsone time stamp related definitions.
 *
 * To obtain the number below, the range specified in time_get (time_svc.c),
 * 0 - 768 * 64, spanning 1.25 ms, is first rescaled to the range
 * 0 - 960 * 64, and one fifth of that value is used.
*/
#define QUARTER_MS_VAL      ( 192 * 64 )

/* Definitions used in formatting navigation fix. */
#define MAX_POS_UNC_IND 32
#define HEADING_SCALE (128.0 / 45.0)

#define LOC_UNC_CONFIDENCE          39       /* PE uses a standard error ellipse and the confidence level 
                                              for a standard error ellipse is defined as 39% */
#define ME_LOC_UNC_CONFIDENCE       63       /* ME uses a standard 2D Circular and the confidence level 
                                              for a standard error ellipse is defined as 39% */
#define C_2_32_OVER_PI              (4294967296.0 / C_PI )
#define C_2_31_OVER_PI              (2147483648.0 / C_PI )

#define GLO_FREQ_NUMBER_OFFSET	7

/*  The SV ID of a GPS SV is within the range defined for GPS SVs in MGP:     1 - 32. */
#define PRN_OK(x)           ( ((x)>0) && ((x)<(N_GPS_SV+1)) )
/*  The SV ID of a Glonass SV is within the range defined for Glonass SVs in     MGP: 65 - 88. */
#define SLOTID_OK(x)        ( ((x)>GLO_SLOTID_OFFSET) && ((x)<(N_GLO_SV+GLO_SLOTID_OFFSET+1)) )
/*  The frequency number of a Glonass SV is not within the range defined for     Glonass SVs in MGP: 0 - 13. */
#define GLO_SIGNED_FREQ_INVALID(x) ((x)<GLO_LOWEST_FREQ_NUMBER || (x)>GLO_HIGHEST_FREQ_NUMBER )
/*  The SV ID of a BDS SV is  within the range in MGP: 201 - 237. */
#define BDS_ID_OK(x)        ( ((x)>BDS_SV_ID_OFFSET) && ((x)<=(N_BDS_SV+BDS_SV_ID_OFFSET)) )
/*  The SV ID of a GAL SV is  within the range in MGP: 301 - 336. */
#define GAL_ID_OK(x)        ( ((x)>GAL_SV_ID_OFFSET) && ((x)<=(N_GAL_SV+GAL_SV_ID_OFFSET)) )

#define QZSS_PRN_OK(x)      ( ((x)>= C_FIRST_QZSS_SV_PRN) && ((x)<= C_LAST_QZSS_SV_PRN) )

#define GNSS_ID_OK(x)       ( PRN_OK(x) || SLOTID_OK(x) || BDS_ID_OK(x) || GAL_ID_OK(x) || QZSS_PRN_OK(x) )

/**
 * @brief Calendar time with tenth of second accuracy.
 *
 * This is a legacy structure that can represent time with tenth of second
 * accuracy in a form of Gregorian calendar fields.
 *
 * @deprecated Use tm_TimeT instead where applicable.
 * @sa tm_TimeT
 */
typedef struct
{
  uint32 q_year;
  uint32 q_month;
  uint32 q_utc_days;
  uint32 q_hours;
  uint32 q_mins;
  uint32 q_secs;
  uint32 q_tenths;
}utc_time;

/**
 * @brief Calendar time with milliseconds
 *
 * Gregorian calendar time structure. This structure can be converted into and
 * from GPS, GLO and UTC timestamps.
 *
 * @sa utc_time
 */
typedef struct
{
  uint32 q_Year;       /**< @brief Year 1970+ */
  uint32 q_Month;      /**< @brief Month 1-12 */
  uint32 q_Day;        /**< @brief Day in month 1-31 */
  uint32 q_Hour;       /**< @brief Hour   0-23 */
  uint32 q_Minute;     /**< @brief Minute 0-59 */
  uint32 q_Second;     /**< @brief Second 0-59 */
  uint32 q_Millisecond;/**< @brief Millisecond 0-999 */
} tm_CalendarTimeT;

/* Store GPS, Glonass time provided by network */
typedef struct
{
  struct
  {
    unsigned refTimeWeekNumValid : 1;   /* Set 1 for w_ref_time_week_num to be valid */
    unsigned refTimeTowMsValid : 1;     /* Set 1 for q_ref_time_tow_ms to be valid */
    unsigned posTimeStampValid : 1;     /* Set 1 for t_pos_time_stamp to be valid */
    unsigned deltaTPresent : 1;         /* Set 1 for b_deltaT to be valid */
    unsigned glonassTimeValid : 1;      /* Set 1 for z_GlonassTime to be valid */
  } m;

  uint16 w_ref_time_week_num;         /* GPS week number. Valid when refTimeWeekNumValid == 1  */
  uint32 q_ref_time_tow_ms;           /* GPS of week in milli second (0..604799999). Valid when refTimeTowMsValid == 1 */
  uint64 t_pos_time_stamp_ms;         /* Milli seconds since birth of GPS time. Valid when pos_time_stamp_valid = 1 */
  int8 b_deltaT;                      /* Leap second offset time from network (-128..127). Used to get GPS or GLONASS
                                         from another GNSS system. Valid when deltaTPresent == 1 */
  glo_TimeStructType z_GlonassTime;   /* GLONASS time structure. Valid when glonassTimeValid == 1 */
} gnss_time_info;

extern const FLT f_PosUncArr[];

typedef struct
{
  uint16 w_GpsWeek;            /* GPS week number at reference tick [weeks] */
  uint32 q_GpsMsec;            /* GPS msec at reference tick [msecs] */
  float f_ClkTimeBias;        /* Clock bias [msecs] */
  float f_ClkTimeUncMs;       /* 1 sided max time bias uncertainty [msecs] */
  uint16 w_MsAmbig;            /* msec ambiguity in time [msec] */
} me_Time;

typedef struct
{
  float f_ClkFreqBias;  /* Clock frequency bias. [m/s] */
  float f_ClkFreqUnc;   /* 1 sided max freq bias uncertainty [m/s] */
} me_Freq;

typedef struct
{
  uint32 q_RefFCount;        /* FCount value at reference tick */
  me_Time z_Time;             /* Time at w_RefFCount */
  me_Freq z_Freq;             /* Freq at w_RefFCount */
} me_Clock;

/*--------------------------------------------------

   Common Defines for TM Modules

---------------------------------------------------*/
#define TM_MAX_BUFFSIZE                     200 /* Max PDDM Size */
#define TM_MAX_SESSION_TIMEOUT              255 /* Max Session Timeout */


/*--------------------------------------------------

      Supported Protocol Types by TM-Core            

---------------------------------------------------*/

/* The protocol number must be set to TM_CORE_PRTL_CB_ARR_INDEX_MAX + 1 */
#define TM_PRTL_NUM                 5
typedef uint32 tm_prtl_type;
#define TM_PRTL_TYPE_NONE           0x0000
#define TM_PRTL_TYPE_STANDALONE     0x0001
#define TM_PRTL_TYPE_DIAG           0x0020
#define TM_PRTL_TYPE_GFC_QMI        0x0080
#define TM_PRTL_TYPE_XTRA_T         0x0100

#define TM_PRTL_TYPE_ALL            (TM_PRTL_TYPE_STANDALONE|\
                                     TM_PRTL_TYPE_DIAG|\
                                     TM_PRTL_TYPE_GFC_QMI|\
                                     TM_PRTL_TYPE_XTRA_T | \
                                     )
/*--------------------------------------------------

   TM core session handle type for NI/MT sessions

---------------------------------------------------*/
typedef uint32 tm_sess_handle_type;
/* Pre-defined session handle for NI/MT sessions */
/* 0x0000 ~ 0x100 is reserved for MO/MI session */
#define TM_SESS_HANDLE_NONE          0x0000
#define TM_SESS_HANDLE_FTEST         0x1000 /* Used by diag module for scripted IS801 */
#define TM_SESS_HANDLE_FORCE_STOP    0x397f /* A magic handle to force session termination. This handle is called from protocols.
                                               Hence we won't ask protocol to stop session when this handle used.*/
/*------------------------------------------------------------------

   PD Comm transaction ID range allocation for protocol sub-moudles
    
--------------------------------------------------------------------*/
//#9x05 remove
#define TM_UMTS_UP_GM_TRANSACTION_ID_RANGE              0x00010000

/*--------------------------------------------------------------------

 Geofence Manager transaction ID range allocation for GFC_QMI
    
--------------------------------------------------------------------*/
#define TM_GFC_QMI_GM_TRANSACTION_ID_BASE               0x02000000
#define TM_GFC_QMI_GM_TRANSACTION_ID_RANGE              0x00010000

/*--------------------------------------------------------------------

 Geofence Manager transaction ID range allocation for TM_XTRA_T

--------------------------------------------------------------------*/
#define TM_XTRA_T_GM_TRANSACTION_ID_BASE               0x04000000
#define TM_XTRA_T_GM_TRANSACTION_ID_RANGE              0x00010000


/*--------------------------------------------------------------------

 Geofence Manager transaction ID range allocation for CSM

--------------------------------------------------------------------*/
#define TM_CSM_GM_TRANSACTION_ID_BASE               0x08000000
#define TM_CSM_GM_TRANSACTION_ID_RANGE              0x00010000

/* UMTS UP protocol sub-module is allowed to use transaction ID from 0x02000000 - 0x02FFFFFF*/

/* Location Uncertainty */
#define TM_LR_THRESHOLD_NO_POS            6000000                /* 6000km no Pos available with MS */
#define TM_LR_THRESHOLD_NO_SEED_POS       5000                   /* 5km no pos for seed fix */
#define TM_TIME_THRESHOLD_NO_TIME         24000                  /* 24000 no ref time available with MS */
#define TM_PUNC_THRESHOLD_IN_SERVICE      12000                  /*  Use Punc threshold of 12km if 1X is in service */

#define TM_MGP_PR_TIMEOUT_MAX             30*1000

/* Target's max doppler uncertainty is 900 meters/sec */
#define TM_MGP_MAX_DOPPLER_UNC_M_SEC 900.0f

#if 0
/* MSBased aiding throttling feature. This feature throttles MS-PDE 
** connections to download Aiding data, AFLT seed, Eph & Almanac.
** MS starts with a min back-off and goes up to max back-off, increasing
** the interval by back off factor. If the requests shows up on time 
** after the safe duration, then the back off time is reset back to min.
*/
  #define GPSONE_MSB_THROTTLE_ENABLE TRUE
  #define GPSONE_MSB_THROTTLE_DISABLE FALSE
  #define BACK_OFF_FACTOR 4             /* Exp back-off by a factor of 4 */
  #define BACK_OFF_MIN_DURATION 60      /* Min duration is 1 min */
  #define BACK_OFF_MAX_DURATION 900     /* Max is 15 min's */
  #define BACK_OFF_RESET_DURATION 900   /* Safe value is 15 min's */
#endif

#define C_PI            3.1415926535898       /* From ICD */
#define C_GAD_LAT_TO_RAD (double)((double)C_PI / (double)16777216)      /* 90/2^23 * (pi/180) */
#define C_GAD_LON_TO_RAD (double)(2.0 * C_GAD_LAT_TO_RAD)               /* 360/2^24 * (pi/180) */

#define C_LAT_RAD_TO_GAD (double)( (double)16777216 / (double)C_PI )  /* (180/pi) * ( 2^23 / 90 ) */
#define C_LON_RAD_TO_GAD (double)( (double)8388608 / (double)C_PI )   /* (180/pi) * ( 2^24 / 360 ) */

/*--------------------------------------

       Phone Event Enumeration 
    
---------------------------------------*/
typedef enum
{
  TM_PHONE_EVENT_NONE = 0,
  TM_PHONE_EVENT_OOS,
  TM_PHONE_EVENT_TC_CONNECTED,
  TM_PHONE_EVENT_TC_DISCONNECTED,
  TM_PHONE_EVENT_TC_EXIT,
  TM_PHONE_EVENT_TC_UP,
  TM_PHONE_EVENT_NEW_SS_INFO,
  TM_PHONE_EVENT_REVA_READ,
  TM_PHONE_EVENT_SS_SRV_DOMAIN,
  TM_PHONE_EVENT_SS_SRV_STATUS,
  TM_PHONE_EVENT_SYS_MODE_LPM,
  TM_PHONE_EVENT_OTASP_STATUS_COMMITTED,
  TM_PHONE_EVENT_IN_TDS_COVERAGE,  
  TM_PHONE_EVENT_HO_COMPLETE,
  TM_PHONE_EVENT_MAX
} tm_phone_event_e_type;

/*--------------------------------------

    Supported Positioning Protocols
    
---------------------------------------*/

typedef struct
{
  sys_srv_status_e_type srv_status;
  sys_sys_mode_e_type sys_mode;
  sys_srv_domain_e_type srv_domain;
  sys_plmn_id_s_type plmn;
  sys_sys_mode_e_type prev_sys_mode;
} tm_cm_ss_info_s_type;

typedef union
{
  tm_cm_ss_info_s_type ss_info;
} tm_phone_event_payload_u_type;

/*TODO:[DEL][APP] Removing strunture tm_lr_resp_type as the position report is being directly to QMI structure
            so intermediate structures are not required*/
/*TODO:[ADD][APP] Reinstate the structure as it is used by geofence need to figure out if we can move this in Gm */


typedef struct 
{
  qword timestamp;             /* CDMA time that the data was received */
  int16 timestamp_msec;               /* Time in msec resolution 0-999 */
  byte msec_incl;                 /* Time in msec resolution included */
  uint16 time_ref_cdma;   /* CDMA system time at time solution is valid */
  sint31 latitude;                    /* Latitude, referenced to WGS-84 */
  sint31 longitude;                  /* Longitude, referenced to WGS-84 */
  byte loc_uncrtnty_ang;              /* Angle of axis wrt true north 
                                               for position uncertainty */
  byte loc_uncrtnty_a;      /* Standard deviation of axis along angle 
                                     specified for position uncertainty */
  byte loc_uncrtnty_p; /* Standard deviation of axis perpendicular to 
                               angle specified for position uncertainty */
  byte fix_type;                                          /* Fix type */
  byte velocity_incl;                /* Velocity information included */
  uint16 velocity_hor;                 /* Horizontal velocity magnitude */
  byte heading_valid;
  uint16 heading;                                            /* Heading */
  int1 velocity_ver;                   /* Vertical velocity magnitude */
  byte clock_incl;                      /* Clock information included */
  sint31 clock_bias;                                      /* Clock bias */
  sint15 clock_drift;                                    /* Clock drift */
  byte height_incl;                    /* Height information included */
  sint15 height;                        /* Height, referenced to WGS-84 */
  byte loc_uncrtnty_v;              /* Standard deviation of vertical 
                                       error for position uncertainty */

  pdsm_lite_gnss_time_source_mask time_src;   /* src of timestamp is GPS/Glo? */
  FLT f_GGTBMs;                         /* GPS to Glonass bias (ms). */
  FLT f_GGTBUncMs;          /* GPS to Glonass bias uncertainty (ms). */
  pdsm_lite_pd_reliability_e_type e_HoriRelIndicator; /*horizontal reliability*/
  pdsm_lite_pd_reliability_e_type e_VertRelIndicator; /*vertical reliability*/

  FLT f_clock_bias_unc;          /* Clock bias unc */
  uint8 u_TimeSource;            /* Extended Time source information */

  FLT   f_HeadingRad;     /* heading in rad. */
  FLT   f_HeadingUncRad;  /* heading uncertainty in rad. */
  FLT   f_GnssHeadingRad;     /* GNSS-only heading in rad. */
  FLT   f_GnssHeadingUncRad;  /* GNSS-only heading uncertainly in rad. */
  FLT   f_altitude; /* Floating point value of the altitude in meters */

  sm_GnssNavSvInfo   z_NavSvInfo;

}tm_lr_resp_type;



/*-----------------------------------------------------------------------

                                MACROS    
-----------------------------------------------------------------------*/
#define SQR(f)  ((f)*(f))


/*-----------------------------------------------------------------------

                           Extern Functions
    
-----------------------------------------------------------------------*/

/*===========================================================================
FUNCTION tm_util_nearest_long

DESCRIPTION
    tm_util_nearest_long() rounds the double floating point var and returns long

RETURN VALUE
    Rounded long signed integer

DEPENDENCIES

===========================================================================*/
extern int32 tm_util_nearest_long( double d_X );

/*===========================================================================

FUNCTION tm_util_covert_pos_unc_to_meters

DESCRIPTION
    Converts position uncertainity in IS801 spec to meters
    
    No floating point or divides are used to optimize performance, and care
    is taken to eliminate integer overflows.
  
DEPENDENCIES
    None.

RETURN VALUE
  Void
SIDE EFFECTS
    None.

===========================================================================*/
extern void tm_util_covert_pos_unc_to_meters
(
    int16 un_a, 
    int16 un_p, 
    long *m_un_a,
    long *m_un_b
);


/*============================================================================
FUNCTION tm_util_convert_lr #9x05 remove

DESCRIPTION
    Function to convert navsolution into IS801 Location response structure

RETURN VALUE
    None

DEPENDENCIES

LIMITATIONS

=============================================================================*/
extern boolean tm_util_convert_lr
(
  sm_FixInfoStructType *input_lr_buf_ptr,
  tm_lr_resp_type         *output_lr_buf_ptr
);
/*===========================================================================
FUNCTION tm_util_calculate_heading

DESCRIPTION

  This function is used to calculate the heading value from the PE position report

DEPENDENCIES 
  None.

PARAMETERS:
  sm_GpsFixRptStructType  *fix_ptr,
  Heading 
  

RETURN VALUE
    
SIDE EFFECTS
  None

===========================================================================*/

extern void tm_util_calculate_heading
( const sm_FixInfoStructType *fix_ptr,
  DBL *heading_angle
  );

/*
 ******************************************************************************
 *
 * tm_util_get_cep_scale_ratio_from_pct
 *
 * Function description:
 *  This function gives the ratio to scale circular uncertainties (CEP's) from
 *  different percentiles.  E.g. 39% conf. "standard" to 99% conf.
 *
 * Parameters:
 *
 *  f_from_percentile: CEP percentile to convert from (range 0.00 to 1.00)
 *  f_to_percentile: CEP percentile to convert to (range 0.00 to 1.00)
 *
 * Return value:
 *
 *  The scale factor to multiply by to scale the CEP<from>% to a CEP<to>% 
 *   value (in meters.)
 *
 ******************************************************************************
*/
extern float tm_util_get_cep_scale_ratio_from_pct( float f_from_percentile,
                                                   float f_to_percentile );


/**
 * @brief Converts calendar time with milliseconds into UTC time structure.
 *
 * During the conversion the milliseconds are truncated to fit the storage type.
 *
 * @param[in]  pz_TimeIn  Calendar time with milliseconds.
 * @param[out] pz_TimeOut UTC time structure.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 */
boolean tm_util_time_to_utc_time
(
  const tm_CalendarTimeT *pz_TimeIn,
  utc_time               *pz_TimeOut
);

/**
 * @brief Converts calendar time with milliseconds into UTC time structure.
 *
 * During the conversion the milliseconds are truncated to fit the storage type.
 *
 * @param[in]  pz_TimeIn  Calendar time with milliseconds.
 * @param[out] pz_TimeOut UTC time structure.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 */
boolean tm_util_time_to_pdsm_time
(
  const tm_CalendarTimeT *pz_TimeIn,
  pdsm_lite_utc_time_s_type   *pz_TimeOut
);

/**
 * @brief Constructs calendar structure from GPS input.
 *
 * @param[in]  w_GpsWeek        GPS week number.
 * @param[in]  q_FixTimeGpsMsec GPS fix time in milliseconds.
 * @param[in]  q_FixTimeUtcMsec UTC fix time in milliseconds.
 * @param[out] pz_TimeOut       Conversion result.
 * @param[out] q_TotalDays computes total time in days
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 *
 * @sa tm_util_construct_glo_time
 */
boolean tm_util_construct_gps_time
(
  uint16            w_GpsWeek,
  uint32            q_FixTimeGpsMsec,
  uint32            q_FixTimeUtcMsec,
  tm_CalendarTimeT *pz_TimeOut,
  uint32           *q_TotalDays
);

/**
 * @brief Constructs calendar structure from GLO input.
 *
 * @param[in]  u_GloNumFourYear       GLO 4 year number from 1996.
 * @param[in]  w_GloNumDaysInFourYear GLO day number in 4 years.
 * @param[in]  q_GloTimeMs            GLO millisecond in the day.
 * @param[out] pz_TimeOut             Conversion result.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 *
 * @sa tm_util_construct_gps_time
 */
boolean tm_util_construct_glo_time
(
  uint8             u_GloNumFourYear,
  uint16            w_GloNumDaysInFourYear,
  uint32            q_GloTimeMs,
  tm_CalendarTimeT *pz_TimeOut,
  uint32           *q_TotalDays
);

/**
 * @brief Construct UTC time from GPS time fields.
 *
 * This function constructs UTC time YearMonthHourMinuteSecond using GPS
 * WeekNumber, GPS msec_of_week and UTC leap time adjusted msec_of_week.
 *
 * @param[in] w_FixWn          fix time GPS week;
 * @param[in] q_FixTimeGpsMsec fix time GPS msec. of the week;
 * @param[in] q_FixTimeUtcMsec fix time UTC msec (adj. for leap seconds ).
 *
 * @return UTC time YearMonthHourMinuteSecond. (if fails to construct UTC time,
 *         UTC will be set to all zero)
 */
utc_time tm_util_construct_utc_time
(
  uint16 w_FixWn,
  uint32 q_FixTimeGpsMsec,
  uint32 q_FixTimeUtcMsec
);


/*
 ******************************************************************************
 *
 * tm_util_num_bits_set_in_long
 *
 * Function description:
 *  This function counts the number of bits set in a uint32 bit mask and returns
 *  the result.
 *
 * Parameters:
 *
 *  q_long_word: bit mask;
 *
 * Return value:
 *
 * number of bits set.
 ******************************************************************************
*/
uint8 tm_util_num_bits_set_in_long( uint32 q_long_word );

/*
 ******************************************************************************
 *
 * tm_util_num_bits_set_in_long64
 *
 * Function description:
 *  This function counts the number of bits set in a uint64 bit mask and returns
 *  the result.
 *
 * Parameters:
 *
 *  t_long [IN]: bit mask;
 *
 * Return value:
 *
 *  Number of bits set.
 ******************************************************************************
*/
uint8 tm_util_num_bits_set_in_long64( uint64 t_long );

/*
 ******************************************************************************
 *
 * tm_util_calc_vel_horiz_uncertainty
 *
 * Function description:
 *  This function calculate the horizontal velocity uncertainty and
 *  return the result
 *
 * Return value:
 *
 *  horizontal velocity uncertainty (m/s)
 ******************************************************************************
*/
extern FLT tm_util_calc_vel_horiz_uncertainty(const sm_NavPositionStructType* nav_ptr);

/*
 ******************************************************************************
 *
 * tm_util_calc_vel_vert_uncertainty
 *
 * Function description:
 *  This function calculate the vertical velocity uncertainty and
 *  return the result
 *
 * Return value:
 *
 *  vertical velocity uncertainty (m/s)
 ******************************************************************************
*/
extern FLT tm_util_calc_vel_vert_uncertainty(const sm_NavPositionStructType* nav_ptr);

/**
 * @brief Computes UTC time from GPS or GLO time source.
 *
 * This function calculates UTC time YearMonthHourMinuteSecond using GPS
 * time or GLONASS time specified in fix report. If both GPS and GLONASS
 * time are valid, GPS time will be used to calculate UTC.
 *
 * This routine can return UTC in two different UTC formats (pdsm_utc_time
 * and utc_time), you can set one to NULL if not used. If you pass in both
 * pointers, two UTC formats will be returned.
 *
 * @param[in]  pz_Fix         Fix report pointer
 * @param[out] pz_PdsmTimeOut PDSM UTC structure pointer (optional, can be NULL)
 * @param[out] pz_UtcTimeOut  TM UTC structure pointer (optional, can be NULL)
 *
 * @retval TRUE  Successfully calculated UTC time
 * @retval FALSE Failed to calculate UTC time (UTC will be set to all zero)
 *
 * @deprecated Use tm_util_calculate_time, tm_util_time_to_utc_time,
 *             tm_util_time_to_pdsm_time
 *
 * @sa tm_util_calculate_time
 * @sa tm_util_time_to_utc_time
 * @sa tm_util_time_to_pdsm_time
 */
boolean tm_util_calculate_utc
(
  const sm_FixInfoStructType *pz_Fix,
  pdsm_lite_utc_time_s_type          *pz_PdsmTimeOut,
  utc_time                      *pz_UtcTimeOut
);




/*
******************************************************************************
*
 * FUNCTION:
 * tm_util_calculate_position_print
 *
 * DESCRIPTION:
 *  This function calculates position using fix report
 *  Position is calcuated in Latitude and Longitude and printed to the i/p string
 *
 * PARAMETERS:
 *  p_fix:          [Input] ptr to fix report
 *  p_Str:          [Output]ptr to the string where data will be written
 *  q_buf_len:      length of string
 *
 * RETURN VALUE:
 *  q_len           length of string written
 *
 ******************************************************************************
*/
extern int tm_util_calculate_position_print
( const sm_FixInfoStructType *p_fix, 
  char *p_Str, 
  int q_buf_len
);

/*===========================================================================
FUNCTION tm_util_calculate_offset_print

DESCRIPTION

 tm_util_calculate_offset_print() is used to Calcullate offset for PQDTM. It is difference of Lat/Long/alt between WGS84 and PZ90 cooordinate system 

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS
 
===========================================================================*/
int tm_util_calculate_offset_print(const double *d_PosLlaWGS84, const double *d_PosLlaPZ90, char *p_Str, int q_buf_len );

/**
 * @brief Converts Glonass time to UTC time.
 *
 * @param[in]  u_GloNumFourYear       Count of 4-year intervals since 1996.
 * @param[in]  w_GloNumDaysInFourYear Number of days within 4-year interval.
 * @param[in]  q_GloTimeMs            Number of milliseconds since midnight.
 * @param[out] pz_UtcTimeOut          Parsed UTC structure with decisecond
 *                                    accuracy.
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 *
 * @deprecated Use tm_util_construct_glo_time
 * @sa tm_util_construct_glo_time
 */
boolean tm_util_glo_to_utc
(
  uint8 u_GloNumFourYear,
  uint16 w_GloNumDaysInFourYear,
  uint32 q_GloTimeMs,
  utc_time* pz_UtcTimeOut
);


/**
 * @brief Adjusts time information according to GPS offset time (deltaT)
 *
 * This function processes GPS offset time (deltaT) to compute the GLONASS time
 * & set the appropriate GLONASS time fields in gnss_time_info struct.
 * The deltaT could be from GPS UTCModel AD and/or GLONASS TimeModel AD.
 *
 * @param[in]     b_DeltaT     Positive GPS time offset value.
 * @param[in,out] pz_AgnssTime Pointer to position time structure.
 * 
 * @retval TRUE  On success
 * @retval FALSE Failure indication.
 */
boolean tm_util_convert_to_glonass_time
(
  int8            b_DeltaT,
  gnss_time_info *pz_AgnssTime
);

/*
 ******************************************************************************
 *
 * tm_util_get_rand
 *
 * Function description:
 *   Gets a random number from security module.
 *
 * Parameters:
 *   None
 *
 * Return value:
 *  uint32 random number.
 *
 ******************************************************************************
*/
uint32 tm_util_get_rand(void);

/**
 * @brief Helper for populating date component of Gregorian calendar.
 *
 * This function has a limitation to work for years between 1900 and 2100.
 *
 * @param[in]  q_StartingYear Starting year: 1980 or 1996. Shall be a leap year.
 * @param[in]  q_Days         Day number within four-year cycle.
 * @param[out] pz_TimeOut     Structure for filling in date components.
 */
void fill_calendar_date
(
  uint32            q_StartingYear,
  uint32            q_Days,
  tm_CalendarTimeT *pz_TimeOut
);

/**
 * @brief Calculates total UTC seconds from UTC epoch
 *
 * This function calculates  total UTC seconds from UTC epoch from the GPS fix,
 *
 * @param[in]  pz_GnssFix     Fix report.
 * @param[out] *pr_total_utc_secs Total UTC seconds from UTC epoch
 *
 * @retval TRUE  Conversion is successful.
 * @retval FALSE Conversion failed or invalid arguments.
 */
boolean tm_util_get_utc_total_secs(const sm_FixInfoStructType *pz_GnssFix, int64 *pr_total_utc_secs);

#ifdef __cplusplus
}
#endif


#endif /* TM_COMMON_H */

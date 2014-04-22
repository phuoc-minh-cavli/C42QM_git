#ifndef _GTS_LOG_H_
#define _GTS_LOG_H_

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Global Time Services Log Module

GENERAL DESCRIPTION
This file is the main file of Global Time Service Log module

Copyright (c) 2014 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2015 - 2018 Qualcomm Technologies, Inc. 
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header:
$DateTime:
$Author:

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "cgps_api.h"
#include "gnss_common.h"
#include "gts_api.h"
#include "gts_drsync.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/

#define LOG_GNSS_GTS_INPUT_REPORT_VERSION   8
#define LOG_GNSS_GTS_TIME_UPDATE_VERSION    3
#define LOG_GNSS_GTS_DB_DUMP_VERSION        5
#define LOG_GNSS_GTS_EVENTS_VERSION         7
#define LOG_GNSS_GTS_QTIME_CALIB_VERSION    3

#define LOG_GNSS_GTS_INPUT_REPORT_SUBVERSION 2
#define LOG_GNSS_GTS_DB_DUMP_SUBVERSION     2

/* LOG_GNSS_GTS_EVENTS_SUB_ID */
#define LOG_GNSS_GTS_EVENTS_LATCH_DB_LOG    1
#define LOG_GNSS_GTS_EVENTS_CDMA_VSTMR_LOG  2
#define LOG_GNSS_GTS_EVENTS_GPS_FCOUNT_LOG  3
#define LOG_GNSS_GTS_EVENTS_QTIME_LOG       4
#define LOG_GNSS_GTS_EVENTS_LTE_VSTMR_LOG   5
#define LOG_GNSS_GTS_EVENTS_COMP_TIME_LOG   6
#define LOG_GNSS_GTS_EVENTS_TIME_TICK_LOG   7
#define LOG_GNSS_GTS_EVENTS_SCLK_FCOUNT_LOG 8
#define LOG_GNSS_GTS_EVENTS_GET_TIME        9
#define LOG_GNSS_GTS_EVENTS_DRSYNC_LOG      10
#define LOG_GNSS_GTS_EVENTS_OFFSET_LOG      11
#define LOG_GNSS_GTS_EVENTS_TICK_PROC_LOG   12
#define LOG_GNSS_GTS_EVENTS_CONV_NS_LOG     13
#define LOG_GNSS_GTS_EVENTS_DEL_TIME_LOG    14
#define LOG_GNSS_GTS_EVENTS_SS_UPDATE_LOG   15

/* Time Transfer DM Log Structure */
#define GTS_TIME_TRANSFER_INFO_LOG_VERSION  58

#define GTS_TTR_PARAMS_LOG_GNSS_FREQ_OFFSET_NORMAL  (0U<<1)

#define GTS_MAX_TIME_TRANSFER_SAMPLES 1

/* To control event logging */
#define GTS_DSBL_LOG_LATCH_DB       (1 << LOG_GNSS_GTS_EVENTS_LATCH_DB_LOG) 
#define GTS_DSBL_LOG_CDMA_VSTMR     (1 << LOG_GNSS_GTS_EVENTS_CDMA_VSTMR_LOG)
#define GTS_DSBL_LOG_FC             (1 << LOG_GNSS_GTS_EVENTS_GPS_FCOUNT_LOG)
#define GTS_DSBL_LOG_QTIME          (1 << LOG_GNSS_GTS_EVENTS_QTIME_LOG)
#define GTS_DSBL_LOG_LTE_VSTMR      (1 << LOG_GNSS_GTS_EVENTS_LTE_VSTMR_LOG)
#define GTS_DSBL_LOG_COMP_TIME      (1 << LOG_GNSS_GTS_EVENTS_COMP_TIME_LOG)
#define GTS_DSBL_LOG_TIME_TICK_REQ  (1 << LOG_GNSS_GTS_EVENTS_TIME_TICK_LOG)
#define GTS_DSBL_LOG_SCLK           (1 << LOG_GNSS_GTS_EVENTS_SCLK_FCOUNT_LOG)
#define GTS_DSBL_LOG_GET_GTS_TIME   (1 << LOG_GNSS_GTS_EVENTS_GET_TIME)
#define GTS_DSBL_LOG_PPS            (1 << LOG_GNSS_GTS_EVENTS_DRSYNC_LOG)
#define GTS_DSBL_LOG_OFFSET         (1 << LOG_GNSS_GTS_EVENTS_OFFSET_LOG)
#define GTS_DSBL_LOG_TICK_PROC      (1 << LOG_GNSS_GTS_EVENTS_TICK_PROC_LOG)
#define GTS_DSBL_LOG_CONV_NS        (1 << LOG_GNSS_GTS_EVENTS_CONV_NS_LOG)
#define GTS_DSBL_LOG_DEL_TIME       (1 << LOG_GNSS_GTS_EVENTS_DEL_TIME_LOG)
#define GTS_DSBL_LOG_SS_UPDATE      (1 << LOG_GNSS_GTS_EVENTS_SS_UPDATE_LOG)

/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/* Declare TTr register latch DB for logging purposes */
extern gts_TtrRegDbType gts_TtrRegDb;

/* Max Time Source elements - Currently GPS, LTE, ME and FREQEST */
/* These are also used to reference the DB array */
typedef enum
{
  LOG_GNSS_PE, /* 0 */
  LOG_GNSS_ME, /* 1 */
  LOG_LTE_ML1_SUB1, /* 2 */
  LOG_FREQEST, /* 3 */
  LOG_GSM_L1_SUB1, /* 4 */
  LOG_GSM_L1_SUB2, /* 5 */
  LOG_GSM_L1_SUB3, /* 6 */
  LOG_WCDMA_L1_SUB1, /* 7 */
  LOG_WCDMA_L1_SUB2, /* 8 */
  LOG_TDSCDMA_L1, /* 9 */
  LOG_CDMA_M1X, /* 10 */
  LOG_LTE_ML1_TTR_SUB1, /* 11 */
  LOG_SLW_CLK_TTR, /* 12 */
  LOG_LTE_ML1_SUB2, /* 13 */
  LOG_LTE_ML1_TTR_SUB2, /* 14 */
  LOG_CORE_SVC,         /* 15 */
  LOG_PMIC_RTC,       /* 16 */
  LOG_QTIME_CALIB,      /* 17 */
  LOG_EXT_INJ,      /* 18 */
  LOG_PACE,      /* 19 */
  LOG_HSFN_SYNC_PMIC_RTC = 26,    /*26 */
  LOG_MAX_SRC
} gts_TimeSrcModuleTypeForLog;

typedef enum
{
  GTS_TTR_SRC_LTE_SIB,
  GTS_TTR_SRC_LTE_FTA
}
gts_TtrSource;

typedef enum
{
  LOG_TT_SOURCE_1X,
  LOG_TT_SRC_LTE_SIB,
  LOG_TT_SRC_LTE_FTA,
  LOG_TT_MAX_SRC
}gts_TTrSrcForLog; /* similar to enum type mc_pgi_TTSource */

typedef enum
{
  GPS_RTC_CLK_RUNNING_BETWEEN_LOGS,
  GPS_RTC_CLK_STOPPED_BETWEEN_LOGS
}
gts_TtrClkStatusBtwLogsType;

typedef enum
{
  /* All time elements are logged as it is */
  GTS_DBDUMP_RAW_TIME,
  /* All time elements are propagated to PE GPS Fix time */
  GTS_DBDUMP_PROP_FIX_TIME,
  /* All time elements are propagated to current Qtime */
  GTS_DBDUMP_CURRENT_TIME
} gts_DbDumpType;

/* enum structure for different clock rates */
typedef enum
{
  GTS_GCx20,
  GTS_GCx32,
  GTS_GCx40,
  GTS_GCx80,
  GTS_INVALID_RATE
} 
gts_BpClkRateType;

typedef enum
{
  GTS_GET_TIME,
  GTS_GET_TIME_USTMR,
  GTS_GET_TIME_FCOUNT,
  GTS_GET_TIME_OFFSET,
  GTS_GET_TIME_EXT
} gts_GetTimeType;

typedef enum
{
  GTS_ERR_NONE,
  GTS_ERR_NULL_PTR,
  GTS_ERR_NO_FCOUNT,
  GTS_ERR_NO_LATCH,
  GTS_ERR_NO_TIME_AVL,
  GTS_ERR_MAX_SRC,
  GTS_ERR_FCQT_FAIL,
  GTS_ERR_NO_QTIME
} gts_ErrorType;

typedef enum
{
  GTS_QTIME_CALIB_LOG_SRC_SV_FIX,
  GTS_QTIME_CALIB_LOG_SRC_MC_REQ
}
gts_QtimeCalibLogSrcType;

/* Log Packet Definition */
#ifdef T_WINNT
#error code not present
#endif


/* LTE Time Transfer sets of O_STMR+GNSS RTCs latched at the same time structure */
#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* O_STMR count value (in CDMA Chipx8) */
  uint64  t_Ostmr;

  /* O_STMR phase value (in CDMA Chipx2^35) */
  uint32  q_OstmrPhase;

  /* GNSS RTC value (in GPS Chipx80) */
  uint32  q_GnssRtc;

  /* GNSS Phase value (in sub-GPS Chipx80) */
  uint32  q_GnssPhase;

  /* GNSS Sample Count status */
  uint32  q_SampleCount;  

  /* USTMR Latch (in 19.2Mhz units ) */
  uint32  q_Ustmr;  
}
gts_LteTimeLatchElemType;

typedef struct
{
  uint8 u_NumElems;
  uint64 t_Qtime;  
  DBL d_LatchUnc;
  gts_LteTimeLatchElemType z_LatchedData[GTS_MAX_TIME_TRANSFER_SAMPLES];
}
gts_LteTimeLatchType;

/* LTE timing advance structure */
#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Validity of timing advance data */
  boolean b_TaValid;

  /* Timing advance (Rx-Tx diff) in Ts units */
  S16 x_TaData;

  /* Serving cell physical ID. Range ~ 0 to 503 */
  U16 w_ServPhyCellId;
}
gts_TtrLogTaDataType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* LTE Time Transfer Report content */
  /* Request ID of the LTE Time Transfer request that triggered this report */
  U8   u_ReqID;

  /* Subscription Information – For MultiSim Support.
   * Type - 'sys_modem_as_id_e_type' as defined in 'mmcp\api\public\sys.h' */
  U8 u_SubInfo;

  /* Report status. Of Type 'gts_LteTimeStatusType' */
  U8 e_RptStatus;

  /* Physical cell ID of the LTE serving cell [Range ~ 0 to 503] */
  U16  w_CellId;

  /* This field specifies the E-UTRA carrier frequency of the LTE serving cell 
   * based on lte_earfcn_t */
  U32  w_Earfcn;

  /* SIB8 "cdma-EUTRA-Synchronisation" info:
     TRUE if LTE NW is synchronized. Otherwise FALSE. */
  boolean b_Sib8Valid;

  /* The CDMA2000 system time corresponding to the SFN
     boundary at or after the ending boundary of the
     SI-Window in which this Time Transfer was taken. The size
     is 52 bits and the unit is [CDMA2000 chips]
  */
  U64  t_Sib8CdmaSystemTime;

  /* SIB16 System Time info*/
  boolean b_Sib16Valid;

  /* The Coordinated universal system time corresponding to the SFN
                              boundary at or after the ending boundary of the
     SI-Window in which this Time Transfer was taken. The size
     is 34 bits and the unit is [1 ms]
  */
  U64  t_Sib16GPSTime;

  /* Number of leap seconds between UTC time and GPS time */
  U16  w_LeapSeconds;

  /* LTE SFN system time of the subframe in which TTr was acquired
     in the unit of subframe */
  U16  w_Sfn;

  /* O-STMR value at the boundary of the subframe in which TTr was acquired */
  U64  t_OstmrSfBoundary;

  /* Best estimate of the current LTE system time uncertainty */
  U32  q_SysTimeUncEst;

  /* Current value of the slow clock counter */
  U64  t_Qtime;

  /* LPM RTC value latched at the same time as the first latched O_STMR/GNSS RTC
     set (in 19.2MHz untis) */
  U32  q_LpmRtcLatched;

  /* Universal STMR value latched at the same time as the first latched
     O_STMR/GNSS RTC set (in 19.2MHz untis) */
  U32  q_UnivStmrLatched;

  /* O_STMR control register read at the same time as the first latched
     O_STMR/GNSS RTC set (in Ts/2^30 units) */
  U32  q_OstmrFcw;

  /* Mark if the next two fields are valid or not */
  boolean b_OstmrVsrcValid;

  /* OSTMR From FW */
  U32 q_OstmrFromFW[2];

  /* VSRC From FW */
  U32 q_VsrcFromFW[2];

  /* Difference between O_STMR and VSRC STMR (in TBD units) */
  U32  q_OstmrVsrcStmrDiff;

  /* Number of sets of latched RTCs/STMRs per LTE Time Transfer Data report.
     u_NumTTSamples should be <= GTS_MAX_TIME_TRANSFER_SAMPLES
     u_NumTTSamples == 0 presents error condition
   */
  U8   u_NumTTSamples;

  /* Timing advance data to predict TTr uncertainity */
  gts_TtrLogTaDataType z_TaData;

  /* Sets of O_STMR + GNSS RTCs latched at the same time */
  gts_LteTimeLatchElemType z_LatchedData[ GTS_MAX_TIME_TRANSFER_SAMPLES];
} gts_LteTtrDmLog;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /*lint -e754 : local structure member not referenced */
  log_hdr_type z_Hdr; /* Standard DM header */
  U8  u_Version;   /* Version number for the log */

  /* Type of Time Transfer information in this log */
  U8 e_TTSource;  /* gts_TtrSource */

  union
  {
    gts_LteTtrDmLog z_Lte;
  }z_Ttr;

} gts_TtrDmLog;

/* Common Time Transfer Data Structure */
#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Indicate if the latch was successful */
  U8 b_Latch;
  /* GPS RTC value latched at the same time as CDMA RTC (in GPS Chipx32) */
  U32  q_GnssRtc;
  /* GNSS RTC sample and chip counts @ Time transfer latch */
  U32  q_SampleCount;
  /* GNSS RTC Phase count @ Time transfer latch */
  U32  q_GnssPhase;
  /* Universal STMR value latched at the same time as the first latched
     CDMA/GNSS RTC set (in 19.2MHz untis) */
  U32  q_Ustmr;
} gts_LatchLogType;

/* Reference Time structure for Timetags */
#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Ref Time */
  U8  b_QtimeValid;
  /* Ref Time Validity */
  U64 t_Qtime;
  U32 f_QtimeBias;
} gts_QtimeLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  U16 w_GpsWeek;     /* GPS week number at reference tick [weeks] */
  U32 q_GpsMsec;     /* GPS msec at reference tick [msecs] */
  U32 f_ClkTimeBias; /* Clock bias [msecs] */
  U32 f_ClkTimeUncMs;/* 1 sided max time bias uncertainty [msecs] */
} gts_GpsTimeLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  U32 f_ClkFreqBias;  /* Clock frequency bias. [m/s] */
  U32 f_ClkFreqUnc; /* 1 sided max freq bias uncertainty [m/s] */
} gts_GpsFreqLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  U32  q_RefFCount;  /* FCount value at reference tick */
  U8   u_SystemRtcValid;
  U64  t_SystemRtcMs;    /* System RTC value at clock validity */
  U32  q_NumClockResets; /* Cumulative # of Clock resets. */
  gts_GpsTimeLogType z_Time;   /* Time at w_RefFCount */
  gts_GpsFreqLogType z_Freq;   /* Freq at w_RefFCount */
} gts_GpsClockLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  U8 b_Valid;
  U8 b_Used;
  gts_LatchLogType z_Latch;
  gts_QtimeLogType z_Qtime;
  U32 q_FCount;
} gts_FCLatchLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  U8 v_FixValid;     /* TRUE if fix is valid, FALSE if invalid */
  U16 w_GpsWeek;       /* GPS week number */
  U32 q_GpsTimeMs;     /* GPS time in week (milliseconds) */
  U32 q_RefFcount;     /* Receiver's ms counter value. */
  U64 t_TimeTickMsec;  /* Receiver's time tick ms counter value */
  U64 d_PosLla[3];        /* LLA position (rad, m) */
  U32 f_Punc3dMeters;     /* Uncertainty of position (m) */
  U64 f_PuncLatLonMeters[2];  /** Latitude, longitude uncertianty (m), 
                                  68% conf. within +/- this 1-sigma value */
  U32 f_PuncVertMeters;         /* Vertical uncertainty (m) */
  U32 f_ErrorEllipse[3];  /** Unc angle (deg), unc a (m), unc b (m)
                              39% conf. within this one-sigma ellipse */
  U8 e_HoriRelIndicator; /*horizontal reliability*/
  U8 e_VertRelIndicator; /*vertical reliability*/
  U64 d_VelEcef[3];       /* Velocity in ECEF (m/s) */
  U64 d_VelEnu[3];        /* Velocity in ENU (m/s) */
  U64 f_HeadingRad;       /* Heading angle, [0,2*pi) (rad). Referenced to North
                            (0 rad) and positive clock-wise. This is the "best" estimate
                            of heading and uses sensor data (if available and enabled).
                            See also: f_GnssHeadingRad below */
  U64 f_HeadingUncRad;    /* Heading uncertainty in Radians */
  U64 f_Vunc3dMps;              /* Velocity uncertainty (m/s) */
  U64 f_VuncEastNorthMps[2];    /* East, North uncertianty (m/s) */
  U64 f_VuncVertMps;      /* Vertical uncertainty (m/s) */
} gts_PosVelLogType; /* Pos Vel data to GTS */


#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  gts_PosVelLogType z_PosVelLog;

  /* Latched Information */
  gts_LatchLogType z_LatchInfoLog;
  gts_QtimeLogType z_QtimeInfoLog;
} gts_PosEstMsgLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  U32  q_VsrcFromFW[2];
  U32  q_VsrcPhFromFW[2];
  U32  q_OstmrFromFW[2];
  U32  q_OstmrPhFromFW[2];
  U64  t_RefDlTime;
} gts_VsrcInfoLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  U32  q_Freq;    /* DL frequency - Same as lte_earfcn_t*/
  U16  w_CellId;  /* Physical Cell ID */

  /* Subscription Information – For MultiSim Support.
   * Type - 'sys_modem_as_id_e_type' as defined in 'mmcp\api\public\sys.h' */
  U8 u_SubInfo;

  /* SIB8 TRUE: LTE NW is synchronized and supports 1x. FALSE: LTE NW is NOT synchronized */
  U8 b_Sib8Valid;
  /* The CDMA2000 system time corresponding to the SFN boundary at or after the ending boundary of the SI-Window in which this Time Transfer was taken. The size is 52 bits and the unit is [CDMA2000 chips] */
  U64  t_Sib8CdmaSystemTime;

  U8 b_Sib16Valid; /* SIB16 System Time info*/
  U64  t_Sib16GpsMsecs; /* The Abs GPS time at the SFN boundary in of 10 msecs */
  U16  w_LeapSeconds; /* Number of leap seconds between UTC time and GPS time */

  U16  w_Sfn;             /* System Frame Number at which data was collected */
  U64  t_OStmrSfBoundary;  /* OSTMR at the boundary of the subframe */

  /* Latch Information */
  U32  q_UstmrLatched;   /* USTMR Latched in 19.2 Mhz Units */
  U64  t_OstmrLatched;   /* OSTMR Latched in 30.2 Mhz Units (Ts) */
  U32  q_OstmrPhLatched; /* OSTMR Latched in 30.2 Mhz Units (sub Ts)*/

  /* VSRC Information for PCC */
  /* Mark if the next field is valid or not */
  U8 b_OstmrVsrcValid;
  gts_VsrcInfoLogType z_RefVsrcInfo;
  gts_VsrcInfoLogType z_CurrVsrcInfo;

  /* Reference Time (QTIME) */
  gts_QtimeLogType  z_QtimeInfo;

  /* LTE State */
  U8 u_LteState;
} gts_LteTimeInfoLogType;

/* ARFCN type, includes band if quadband is supported */
typedef enum
{
  GTS_SYS_BAND_EGSM_900,
  GTS_SYS_BAND_PGSM_900,
  GTS_SYS_BAND_PCS_1900,
  GTS_SYS_BAND_DCS_1800,
  GTS_SYS_BAND_CELL_850,
  GTS_INVALID_BAND,
  GTS_SYS_BAND_NONE = 0xFF  
} gts_GsmBandLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  U16 w_BandNum;
  U8 e_BandType; // type 'gts_GsmBandLogType'
} gts_GsmArfcnLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Subscription Information – For MultiSim Support.
   * Type - 'sys_modem_as_id_e_type' as defined in 'mmcp\api\public\sys.h' */
  U8 u_SubInfo;

  /* Cell Information. Base Station Identification code and freq */
  U8 u_Bsic;
  gts_GsmArfcnLogType z_Arfcn;

  /* Time Information */
  /* Frame Number 0..(26L*51L*2048L-1)  ~4.615ms frames */
  U32 q_FrameNum;
  /* Time Slot Number 0..7 */
  U8 u_TimeSlot;
  /* QSymbol Count (sub fn) */
  U32 q_QSymCnt;

  /* GSM State */
  U8 u_GL1State;

  /* Additional Drift in the time in QSym Units (if any) */
  S8 b_DriftQSym;

  /* Reference Time (QTIME) */
  gts_QtimeLogType  z_QtimeInfo;
} gts_GsmTimeInfoLogType;

/* GTS Freq Est Structure */
#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  U64 t_PrevQtime;  /* Time at the last temp Update */
  U64 t_CurrQtime; /* Time at the current temp Update */
  S32  l_FreqEst;   /* Freq Est based on Temperature at Prev and Curr Time */
  U32  f_FreqEstUncMs; /* Time Uncertainity estimate based on temp table */
  U8  u_FtCalState;  /* FT Calibration state */
} gts_FreqEstInfoLogType;

/* Time Estimate Message from ME */
#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  U8 e_MESrc;
  gts_GpsClockLogType z_GpsTimeEst;

  /* Latched Information */
  gts_QtimeLogType z_QtimeInfoLog;
  U32 q_FCount;
} gts_METimeEstMsgLogType;

/* Data structure to latch Slow Clock with System Time */
#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* 1x System time in qword format */
  qword   t_SystemTimeT1;
  
  /* CDMA RTC value exactly when System Time (T1) was obtained */
  U32  q_CdmaRtcAtT1;

  /* Value of the Slow clock counter latched at the same time(T2) as CDMA RTC */
  U64  q_SclkRtcAtT2;

  /* CDMA RTC value latched at the same time(T2) as Slow Clock counter */
  U32  q_CdmaRtcAtT2;

  /* Reacquisition slew */
  U32  q_ReacqSlew;

  /* Frequency Error Estimate of the slow clock */
  S32  q_SclkFee;

  /* Best estimate of the current 1x system time uncertainty */
  U32  q_SystemTimeUncertaintyEstimate;

  /* Maximum Antenna Range */
  U32  q_Mar;
}gts_SclkTtrInfoLogType;

/* GTS Time Input Log Type */
#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef union PACKED_POST
#else
typedef PACKED union PACKED_POST
#endif
{
  gts_PosEstMsgLogType z_PosEstMsgLog;
  gts_METimeEstMsgLogType z_METimeEstMsgLog;
  gts_LteTimeInfoLogType z_LteTimeInfoLog;
  gts_GsmTimeInfoLogType z_GsmTimeInfoLog;
  gts_FreqEstInfoLogType z_FreqEstInfoLog;
} gts_ModuleInfoLogType;

/* GTS Input Information Log Packet */
#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard DM header */
  log_hdr_type z_Hdr;

  /* Version number for the log */
  U8 u_Version;

  /* Sub-Version number for the log */
  U8 u_SubVersion;

  U8 e_TimeModuleSource; // of enum type 'gts_TimeSrcModuleTypeForLog'
  gts_ModuleInfoLogType z_TimeInputLog;
} gts_TimeInputLogType;

/* GTS Time Update Log Packet */
#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* If the time below is valid or not */
  U8 b_Valid;

  /* Time Source */
  U8 e_TimeSrc;

  /* Absolute GPS Time related information */
  U16 w_GpsWeek;     /* GPS week number at reference tick [weeks] */
  U32 q_GpsMsec;     /* GPS msec at reference tick [msecs] */
  U32 f_ClkTimeBias; /* Clock bias [msecs]. Constraint to >-0.5 and <0.5 */
  U64 d_ClkTimeUncMs; /* 1 sided max time bias uncertainty [msecs] */
  U32 f_ClkTimeUncMs;

  /* Reference Time (QTIME) */
  gts_QtimeLogType  z_QtimeLogInfo;

  /* GPS Clock Specific Fields */
  U8  b_FCountValid; /* Mark if FCount below is valid */
  U32 q_GpsFCount;    /* GPS FCount at the Time Estimate */
}gts_TimeLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard DM header */
  log_hdr_type z_Hdr;

  /* Version number for the log */
  U8 u_Version;

  U8 e_ModSrc; // of enum type 'gts_TimeSrcModuleTypeForLog'
  gts_TimeLogType z_PrevTime;
  gts_TimeLogType z_CurrTime;
}gts_TimeUpdateLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Source Module */
  U8  e_SrcMod; // of enum type 'gts_TimeSrcModuleTypeForLog'
  /* Source Module Time */
  gts_TimeLogType z_GpsTime;

  /* Time Difference with GPS Ref Time */
  U8  b_TimeDiffValid;
  U64 d_TimeDiffMs;
}gts_DbTimeLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard DM header */
  log_hdr_type z_Hdr;

  /* Version number for the log */
  U8 u_Version;

  /* Sub-Version number for the log */
  U8 u_SubVersion;

  /* 0x0 - Raw Time Dump
     0x1 - Time Dump propagated to recent GPS Fix */
  U8 u_Flags;
  /* Reference Time from GPS Fix */
  gts_TimeLogType z_GpsRefTime;
  gts_DbTimeLogType z_ModSrcTime[MAX_SRC];
}gts_DbLogType;

/* Time transfer register latches sent by CC/NavRx to GTS */
#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  U32 q_GnssRtc;
  U32 q_GnssRtcPhase;
  U32 q_Ustmr;
  U32 q_RawUstmr;
  U32 q_SampleSkip;
}
gts_TtrLatchRegSetLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard DM header */
  log_hdr_type z_Hdr;

  /* Version number for the log */
  U8 u_Version;

  /* SubLog Id Type */
  U8 u_SubLogIdType;    
} gts_EventsLogHdrType;

/* GTS Events Register DB Log Packet */
#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard Header for GTS Events */
  gts_EventsLogHdrType z_Hdr;

  /* Log GTS Qtime */
  gts_QtimeLogType z_Qtime;

  /* Other Log Packet Fields for Reg Db */
  U8 u_LatchType;
  U16 w_Cnt;
  gts_TtrLatchRegSetLogType z_Elem[GTS_TTR_LATCH_REG_API_MAX_SIZE];

  /* Information from the stored DB */
  U16 w_ValidCntDb;

  /* Latch Stats */
  U32 q_NumTtrLatchSets;
  U32 q_NumTtrLatchErrorsPerSet;
  U32 q_NumTtrLatchErrorsTotal;  
} gts_TtrRegDbLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard Header for GTS Events */
  gts_EventsLogHdrType z_Hdr;
  
  /* USTMR  */
  U32 q_Ustmr;  

  /* OSTMR */
  U32 q_Ostmr;

  /* OSTMR Phase */
  U32 q_OstmrPhase;
} gts_LteVstmrLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard Header for GTS Events */
  gts_EventsLogHdrType z_Hdr;
  
  /* GPS FCount is valid ? */
  U8 b_Valid;  

  /* GPS FCount */
  U32 q_GpsFCount;
} gts_GpsFCountLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard Header for GTS Events */
  gts_EventsLogHdrType z_Hdr;
  
  /* Qtime valid */
  U8 b_Valid;  

  /* Qtime */
  U64 t_Qtime;

  /* Qtime Bias*/
  FLT f_QtimeBias;
} gts_QtimeEventLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard Header for GTS Events */
  gts_EventsLogHdrType z_Hdr;

  /* RAT that was used for Comparison */
  U8 b_Rat;
  /* Prev GTS Time Info */
  gts_TimeLogType pz_PrevTime;
  /* Curr GTS Time info */
  gts_TimeLogType pz_CurrTime;

  /* Raw Clock for Previous Clock */
  U32 q_PrevStmr;
  U32 q_PrevStmrPh;

  /* Raw Clock for Current Clock */
  U32 q_CurrStmr;
  U32 q_CurrStmrPh;
} gts_CompTimeLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard Header for GTS Events */
  gts_EventsLogHdrType z_Hdr;
  
  /* Information is valid or not */
  U8 b_Valid;

  /* GPS FCount */
  U32 q_GpsFCount;

  /* Time Tick Sub Ms (in Ms) */
  U32 f_TimeTickSubMs;

  /* Unc of Sub Ms */
  U32 f_TimeTickUncMs;

  /* Time Tick (in Ms) */
  U64 t_TimeTickMs; 

  /* QTime Information */
  gts_QtimeLogType z_Qtime;
}gts_TimeTickInfoLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard Header for GTS Events */
  gts_EventsLogHdrType z_Hdr;

  /* Information is valid or not */
  U8 b_Valid;

  /* SlowClkTickCount */
  uint32 q_SlowClkTickCount; 
  
  /* GPS FCount */  
  uint32 q_FCount32; 

  /* GPS Cx80 */
  uint32 q_GnssChipx80;
}gts_SclkFCountLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard Header for GTS Events */
  gts_EventsLogHdrType z_Hdr;

  /* Type of Get Time */
  U8 u_GetTimeType;

  /* Error Code */
  U8 u_ErrorCode;

  /* Information is valid or not */
  gts_TimeLogType z_GtsTime;

  /* USTMR information */
  U32 q_Ustmr;

  /* Offset Information */
  U64 d_Offset;
}gts_GetGtsTimeLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard Header for GTS Events */
  gts_EventsLogHdrType z_Hdr;

  /* GTS Time Used */
  gts_TimeLogType z_GtsTime;

  /* USTMR used for calculating Assert */
  U32 q_Ustmr;

  /* USTMR at which the DRSync pulse was asserted */
  U32 q_ATUstmr;

  /* USTMR at which the DRSync pulse was deasserted */
  U32 q_DATUstmr;

  /* Time diff for the next GPS sec boundary. DBL value stored in uint64 */
  U64 d_AssertTimeMsec;
}gts_DRSyncLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard Header for GTS Events */
  gts_EventsLogHdrType z_Hdr;

}gts_TickProcLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard Header for GTS Events */
  gts_EventsLogHdrType z_Hdr;

  /*Error recovery flag to blacklist the source */
  boolean b_ErrorRecovery;

}gts_DelTimeLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /* Standard Header for GTS Events */
  gts_EventsLogHdrType z_Hdr;

  int8  b_SubId;
  
  U8 u_NumStacks;

  boolean b_SerCellChanged;

  int8 b_SysMode;  /* Same as ENUM sys_sys_mode_e_type */
  
  int8 b_SrvStatus; /* same as ENUM sys_srv_status_e_type */

}gts_SSUpdateInfoLogType;

#if defined (T_APQ8064) || defined (FEATURE_GNSS_SA)
typedef struct PACKED_POST
#else
typedef PACKED struct PACKED_POST
#endif
{
  /*lint -e754 : local structure member not referenced */

  log_hdr_type z_hdr;             /* Standard DM header */

  /* Version of log packet.Set to 1 for GTS QTime Calib */
  U8 u_Version;
  
  /* This flag indicates whether it is calibrated or uncalibrated */
  U8 e_CalibState;

  /* Event which set or invalidate calibration */
  U8 e_CalibStateReason;

  /* This flag indivates whether calibration is based on MC request or Position fix */
  U8 e_QtimeCalibLogSrcType;
  
  /* To store current offset which can be used later */
  U64 d_PrevFCOffset;
  
  /* Store the Frequency biases for next update */
  U64 d_QtimeFreqBiasM1;
  U64 d_QtimeFreqBiasM2;
  
  /* Store previous freq bias to compute Fcount offset when
     receiver wake up from DPO sleep */
  U64 d_ApproxClkFreqBias;
  
  /* Store latest clock frequency bias estimate */
  U64 d_ClkFreqBias;
  
  /* Store latest clock frequency bias Unc */
  U64 d_ClkFreqBiasUnc;
  
  /* GTS time associated with QTime latch after position fix */
  gts_TimeLogType z_GtsTime1;
  gts_TimeLogType z_GtsTime2;
  
  /* Store latched QTime and Fcount at perticular Tick and wake-up instant */
  gts_FCLatchLogType z_TicLatch;
  gts_FCLatchLogType z_WakLatch;
}gts_QtimeCalibLogType;

#ifdef T_WINNT
#error code not present
#endif

/*--------------------------------------------------------------------------
 * Function Prototype Declarations
 *-----------------------------------------------------------------------*/

/*
 ******************************************************************************
 * Function gts_LogVstmrValues
 *
 * Description:
 *
 *  Generate the Time update log packet which provides the new time and the old
 *  time for the particular Time Src
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogLteVstmrValues( U32 q_Ostmr,U32 q_OstmrPhase,U32 q_Ustmr );

/*
 ******************************************************************************
 *
 * Function:
 *
 *   gts_LteTtrLog
 *
 * Description:
 *
 *  This function logs the LTE Time transfer data
 *
 * Parameters:
 *
 *  pz_CdmaTimeInfo - CDMA Time Info Structure
 *  pz_CdmaLatchDb - CDMA Latch Database
 *
 * Return value:
 *
 *  None
 *
  ******************************************************************************
*/
void gts_LteTtrLog( const gts_LteTimeInfoType *pz_LteTimeInfo,
                    const gts_LteTimeLatchType *pz_LteLatchDb );


/*
 ******************************************************************************
 * Function gts_LogQtimeCalib
 *
 * Description:
 *
 *  This function logs the USTMR calibration parameters when SV/PE makes
 *  GPS fix or when MC request for USTMR calibration source
 *
 * Parameters:
 *
 *  e_CalibSrcLog - Enum for calibration Source (MC request or Position fix)
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogQtimeCalib( const gts_QtimeCalibLogSrcType e_CalibSrcLog );

/*
 ******************************************************************************
 * Function gts_LogGpsFCount
 *
 * Description:
 *
 *  Generate the GPS FCount Event Log packet. This will help in Playback 
 *  processing of logs
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogGpsFCount( boolean b_Valid,U32 q_GpsFCount );

/*
 ******************************************************************************
 * Function gts_LogTimeUpdate
 *
 * Description:
 *
 *  Generate the Time update log packet which provides the new time and the old
 *  time for the particular Time Src
 *
 * Parameters:
 *
 *  pz_PrevTime - Previous Time estimate from DB
 *  pz_CurrTime - Current computed Time estimate
 *  e_ModSrc    - Module Source that is injecting the Time
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogTimeUpdate( gts_TimeType* pz_PrevTime,
                        gts_TimeType* pz_CurrTime,
                        gts_TimeSrcModuleType e_ModSrc );

/*
 ******************************************************************************
 * Function gts_LogMETimeEstMsg
 *
 * Description:
 *
 *  This function logs ME Time information coming into GTS
 *
 * Parameters:
 *
 *  pz_MeTime - ME Time Estimate Report Message
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogMETimeEstMsg( const gts_METimeEstMsgType* pz_MeTime );

/*
 ******************************************************************************
 * Function gts_LogPosTimeEstMsg
 *
 * Description:
 *
 *  This function logs Position Estimate Message from PE
 *
 * Parameters:
 *
 *  pz_PosTime - PE Position Estimate Report Message
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogPosTimeEstMsg( gts_PosEstMsgType* pz_PosTime );

/*
 ******************************************************************************
 * Function gts_LogFreqEstMsg
 *
 * Description:
 *
 *  This function logs Freq Estimate Message from FEST
 *
 * Parameters:
 *
 *  pz_FreqEstInfo - Freq Estimate Message
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogFreqEstMsg( gts_FreqEstInfoType* pz_FreqEstInfo );

/*
 ******************************************************************************
 * Function gts_LogLteTimeMsg
 *
 * Description:
 *
 *  This function logs Time Info msg from LTE ML1 into GTS Input Log packet
 *
 * Parameters:
 *
 *  pz_LteTime - LTE Time info msg from LTE ML1
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogLteTimeMsg( const gts_LteTimeInfoType* pz_LteTime );

/*
 ******************************************************************************
 * Function gts_CopyGtsTimeInfo
 *
 * Description:
 *
 *  Function copies the Time info to the packed log Time info structure
 *
 * Parameters:
 *
 *  pz_Log - GTS Time info Log Pointer
 *  pz_Time - GTS Time info pointer
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_CopyGtsTimeInfo( gts_TimeLogType* pz_Log, const gts_TimeType* pz_Time );

/*
 ******************************************************************************
 * Function gts_LogTimeDb
 *
 * Description:
 *
 *  This function logs the database either in Raw format or Time comparison
 *  format where it compares with Reference time or in Current Time format,
 *  where all the time is estiamted current time for all sources and logged
 *
 * Parameters:
 *
 *  e_DumpType - Type of Log DB Dump (Raw,Prop,Current)
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogTimeDb( gts_DbDumpType e_DumpType );

/*
 ******************************************************************************
 * Function gts_LogGpsFCount
 *
 * Description:
 *
 *  Generate the GPS FCount Event Log packet. This will help in Playback 
 *  processing of logs
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogGpsFCount( boolean b_Valid,U32 q_GpsFCount );

/*
 ******************************************************************************
 * Function gts_LogCompTime
 *
 * Description:
 *
 *  Generate the GPS Compare Time Log packet. This will help in Playback 
 *  processing of logs
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogCompTime( gts_TimeType* pz_PrevTime, 
                      gts_TimeType* pz_CurrTime,
                      gts_METimeCompType* pz_PrevCompTime, 
                      gts_METimeCompType* pz_CurrCompTime);

/*
 ******************************************************************************
 * Function gts_LogTimeTickReq
 *
 * Description:
 *
 *  Generate the TimeTickReq. This will help in Playback 
 *  processing of logs
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogTimeTickReq(gts_TimeTickInfoType* pz_TimeTickInfo);

/*
 ******************************************************************************
 * Function gts_LogSclkFCount
 *
 * Description:
 *
 *  Slow Clock and FCount Log Packet Generation
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogSclkFCount( U32 q_SlowClkTickCount, 
                        U32 q_FCount32,
                        U32 q_GnssChipx80, 
                        boolean b_Valid);

/*
 ******************************************************************************
 * Function gts_LogGsmTimeMsg
 *
 * Description:
 *
 *  This function logs Time Info msg from GSM into GTS Input Log packet
 *
 * Parameters:
 *
 *  pz_GsmTime - GSM Time info msg from GSM module
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogGsmTimeMsg( const gts_GsmTimeInfoType* pz_GsmTime );

/*
 ******************************************************************************
 * Function gts_LogLatchDataGen8
 *
 * Description:
 *
 *  Log the GPS RTC/USTMR latches for Gen8 PLs
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogLatchDataGen8( void );

/*
 ******************************************************************************
 * Function gts_LogLatchDb
 *
 * Description:
 *
 *  Log the GPS RTC/USTMR latches sent by CC
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogLatchDb( const gts_TtrRegLatchInfoType* pz_TtrLatch );

/*
 ******************************************************************************
 * Function gts_LogQtime
 *
 * Description:
 *
 *  Generate the Qtime Event Log packet. This will help in Playback 
 *  processing of logs
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogQtime( gts_QtimeType* pz_Qtime );

/*
 ******************************************************************************
 * Function gts_LogGetGtsTime
 *
 * Description:
 *
 *  Generate the Get GTS Time Event Log packet. This will help in Playback 
 *  processing of logs
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogGetGtsTime( gts_GetTimeType e_GetTimeType,
                        gts_TimeType* pz_GtsTime, 
                        U32 q_Ustmr,
                        DBL d_Offset,
                        gts_ErrorType e_Err );

/*
 ******************************************************************************
 * Function gts_LogDRSyncPulseTime
 *
 * Description:
 *
 *  Generate the Log Packet when DRSync pulse was programmed
 *
 * Parameters:
 *
 *  p_DrSyncParams - DR sync params
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogDRSyncPulseTime( const gts_DRSyncParamType* p_DrSyncParams );

/*
 ******************************************************************************
 * Function gts_LogTickProc
 *
 * Description:
 *
 *  Generate the Log Packet when Offset request was recieved
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LogTickProc();

/*
 ******************************************************************************
 * Function gts_LogDelTime
 *
 * Description:
 *
 *  Generate the Log Packet when time delete is received from MCF
 *
 * Parameters:
 *
 *  pz_Deltime - pointer to delete time structure 
 *
 * Dependencies:
 *F
 *  NoneF
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/

void gts_LogDelTime(const gts_DelTimeType *pz_Deltime);

/*
******************************************************************************
* Function gts_LogSSChangeInfo
*
* Description:
*
*  Generate the Log Packet when Serving cell info is received from SM
*
* Parameters:
*
*  pz_SSInfo - pointer to serving cell info
*
* Dependencies:
*
*  None
*
* Return value:
*
*  None
*
******************************************************************************
*/
void gts_LogSSChangeInfo(const gts_SSInfoType *pz_SSInfo);

#endif //#ifndef _GTS_LOG_H_

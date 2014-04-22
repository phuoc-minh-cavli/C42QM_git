#ifndef _GTS_H_
#define _GTS_H_

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Global Time Services Module

GENERAL DESCRIPTION
This file is the main file of Global Time Service module

Copyright (c) 2014 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2015 - 2018 Qualcomm Technologies, Inc. 
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/wwanme/gts/inc/gts.h#6 $
$DateTime: 2020/12/21 01:51:23 $
$Author: pwbldsvc $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "msg.h"
#include "gts_loc_api.h"
#include "sys.h"
#include "gps_types.h"
#include "gps_common.h"
#include "gts_task.h"
#include "wwanme_common_defs.h"
#include "pm.h"
#include "pm_err_flags.h"
#include "pm_rtc.h"
#include "fs_sys_types.h"
#include "fs_fcntl.h"
#include "pmapp_rtc.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/

#ifndef ABS_GTS
#define ABS_GTS(a)          (((a) < 0) ? (-(a)) : (a))
#endif

#define C_GPS_WEEK_UNKNOWN  0xFFFF

/* Macros for conversion of USTMR to slow clock units and for slow clock to GNSS cx80 */
/* Slow clock freq is 19.2MHz/586.009034306 (refer to macro 'SCLK_DIV_FACTOR' in file
 * 'modem_proc\core\systemdrivers\timetick\src\timetickLegacy.c’) */
#define GTS_USTMR_TO_SLOW_CLK_TICK  (DBL)586.009034306
/* Slow clock ticks per msec. 19.2MHz / (586.009034306*1000) = 32.764 Hz */
#define GTS_SLOW_CLK_TICKS_PER_MS   (DBL)32.764

/* Time related paramaters */
#define GTS_GNSSRTC_CNT_1MS         131072
#define GTS_GNSSRTC_COMPLETE_ROLLOVER     4294967296
#define GTS_GNSSRTC_MS_MASK        0x7fff
#define GTS_GNSSRTC_MS_ROLLOVER    0x8000
#define GTS_GNSSRTC_MS_SHFT         (17)

/* Define the bits and masks regarding GPSRTC */
#define GTS_GPSRTC_CNT_1MS_MASK     0xffff8000
#define GTS_GPSRTC_CNT_1MS_SHFT            0xf
#define GTS_GPSRTC_CNT_CX32_MASK        0x7fff
#define GTS_GPSRTC_CNT_CX32_SHFT           0x0

/* GPS CHIPS related definitions */
#define GTS_GCX1_PER_MSEC           1023
#define GTS_GCX32_PER_MSEC          (32*GTS_GCX1_PER_MSEC)
#define GTS_GCX80_PER_MSEC          (80*GTS_GCX1_PER_MSEC)

/*Define the bits and masks for GNSSRTC (Gen8)*/
#define GTS_GNSSRTC_CNT_CX80_MASK        (0x1FFFF)

/* 16:7  Chip count */
#define GTS_GNSSRTC_CHIP_CNT_MASK        (0x1FF80)
#define GTS_GNSSRTC_CHIP_CNT_SHIFT       (7)

/* 6:0  subChip x 80 sample count */
#define GTS_GNSSRTC_SAMPLE_CNT_MASK      (0x7F)

#define GTS_GNSSPHASE_PER_CX80           4294967296

/* 64 bit Dal Time - Lower 24 bits are Ustmr, so higher 40 are MSB */
#define GTS_USTMR_MSB_SHIFT               (24)

/* USTMR Clock speed = 19.2 MHz */
#define GTS_USTMR_CLOCK_FREQ_HZ          19200000

/* USTMR Ticks Per Msec (GTS_USTMR_CLOCK_FREQ_HZ/1000) */
#define GTS_USTMR_TICKS_PER_MSEC         19200

/* USTMR Rolls over every 0x8000000 counts */
#define GTS_USTMR_ROLLOVER_COUNT         16777216

/* USTMR Rollover midpoint */
#define GTS_USTMR_HALF_ROLLOVER_COUNT    8388608

/* USTMR Rolls over every 873 ms (16777216/19200)  */
#define GTS_USTMR_ROLLOVER_MS            873.813

/* Number of CDMA chips in 1 week.
 * 1.2288 Mcps * 3600 * 24 * 7 */
#define GTS_NUM_OF_SECS_PER_WEEK_CX1   743178240000

/* No of ms in a week 7*24*60*60*1000 */
#define GTS_NUM_OF_MSEC_IN_ONE_WEEK  604800000
#define GTS_MS_PER_WEEK_DBL          ((DBL)604800000)

/* OSTMR Clock Speed */
#define GTS_OSTMR_PER_MS 30720.0

/* OSTMR Phase Conversion factor */
#define GTS_OSTMR_PHASE_FACTOR  4294967296.0

#define GTS_1MS_IN_LTE_TS   GTS_OSTMR_PER_MS
#define GTS_11MS_IN_LTE_TS   337920 /* 11 * 30720 */
#define GTS_OSTMR_ROLLOVER  0x40000000
#define GTS_TIMING_ADVANCE_IN_TS 0.0000325521

#define GTS_MAX_NO_OF_LATCHES 8
#define GTS_USTMR_PER_OSTMR   0.625   /* 19.2 Mhz/30.72 Mhz */

#define GTS_MAX_USTMR_PROPAGATION_MS    86400000 /* 24 hours in ms */

/* Define the Network clock drift to be 0.25ppm/250 ppb */
#define GTS_NETWORK_CLOCK_DRIFT 0.25e-6

/* Max Time between two LTE time - Just shy of OSTMR Rollover */
#define GTS_MAX_LTE_DIFF_MS    34000

/* Minimum Uncertainity for HAT mode to be enabled - 100 Us*/
#define GTS_MIN_UNC_FOR_HAT_EN_MS    0.1

/* Time uncertainity due to sleep clock latching for HAT */
#define GTS_TIME_UNC_PER_LATCH_MS    0.002

/* Uncertainity for HAT for Low to High XO Mode switches (per second) */
#define GTS_TIME_UNC_MODE_SWITCH_MS    0.001

/* 1 ppb */
#define GTS_UNC_1024_PPB                ((DBL)1e6*1024*1024)

/* To convert PPB to PPM, divide by 1024 instead of 1000.
 * Use the same conv. factor to convert XO FreqEst to PPB */
#define GTS_XO_FEST_SCALING_FACTOR     ((DBL)1024.0)

/* Max error in Freq Est of XO Clock in PPB (17 PPM) */
#define GTS_FREQ_EST_ERR_BOUND_1024_PPB  17*1024*1024

/* Default Uncertainity if there is not enough motion data available */
#define GTS_DEFAULT_WWAN_TUNC_MS     0.1

/* LTE Time Unc based on MAR of 10 Km (33us) + Base Station Clock error (5us) */
#define GTS_LTE_MAR_TUNC_MS          0.038

/* GSM Time Unc based on MAR of 10 Km (33us) + 3 symbols wide unc. */
#define GTS_GSM_MAR_TUNC_MS          0.044077

/* Air speed model for uncertainty propagation in meters/second */
/* #define LOC_PROP_AIR_SPEED_MPS      ((FLT)(250.0)) ~ approx 1us/s*/
#define GTS_PROP_AIR_SPEED_TUNC_MS  0.01e-6

/* Max buffer size of Diag commands */
#define GTS_MAX_DIAG_CMD_SIZE   36

/* Max Time diff in MS between FCount Propagations QTime and ME Est */
#define GTS_MAX_FCOUNT_PROP_DIFF_MS     100

/* FT State and corresponding UNC Definitions (ppm) */
/* Based on the current FT state, XO Unc (USTMR/QTime) Unc will vary */
/* Following are the Max UNC constants for few of the FT States, For all 
   other FT States, The UNC value will be the Coarse Cal UNC */
#define GTS_FT_STATE_COARSE_CAL   5
#define GTS_COARSE_CAL_UNC        17e-6

#define GTS_FT_STATE_IFC          6
#define GTS_IFC_UNC               11e-6

#define GTS_FT_STATE_FINE_CAL     7
#define GTS_FINE_CAL_UNC          10e-6

#define GTS_FT_STATE_FT1          8
#define GTS_FT1_UNC               9e-6

#define GTS_FT_STATE_FT2          11
#define GTS_FT2_UNC               3e-6

/* Define the Max Ustmr clock drift to be 17 ppm which is coarse Cal UNC */
#define GTS_USTMR_CLOCK_DRIFT     GTS_COARSE_CAL_UNC

/* Used to indicate some calculation failed and hence default value was used */
#define GTS_ERROR_DIFF_MS        4294967296.0

/* GSM frame timing */
#define GTS_GSM_FRAMES_PER_HYPERFRAME 2715648L // 26 * 51 * 2048
#define GTS_GSM_MSECS_PER_HYPERFRAME  12533760L // GSM_HYPERFRAME * 120 / 26
#define GTS_GSM_MSECS_PER_FRAME       4.61538462f // 120 / 26
#define GTS_GSM_NUM_SYMBOLS_PER_FRAME     5000
#define GTS_GSM_NUM_SYMBOLS_PER_SLOT      625
#define GTS_GSM_SYMBOL_LEN_IN_USEC        0.923076923f // 120 / (5 * 26)
#define GTS_GSM_MAX_DIFF_MS    GTS_GSM_MSECS_PER_HYPERFRAME //Max time between 2 GSM time instances
#define GTS_GSM_MAX_DRIFT_QSYMS    16   //Max Additional Drift For GSM Time


/* Time Estimate for MC constants */
#define GTS_MAX_FCOUNT_EST_DIFF_MS 5000

/* Rollover detection point is half of Rollover value 0x8000 */
#define GTS_DETECT_FCOUNT_ROLLOVER_MS 16384
#define GTS_MIN_GPS_TIME_UNC_MS       (DBL)0.1

/* Max latch attempts for GPS RTC and USTMR on Gen8 */
#define GTS_MAX_LATCH_ATTEMPT_GEN8    5

/* ----------------------------------------------------- *
 * IMPORTANT: The routines below are useful given the    *
 *            following preconditions:                   *
 *  - A must be in the range [0..M-1]                    *
 *  - B must be in the range [0..M-1]                    *
 *  - RESULT will be in the range [0..M-1]               *
 *     but requires a datatype in the range [0..2M-2]    *
 *     to prevent overflow.                              *
 *                                                       *
 *  If these conditions are not true, you can NOT use    *
 *  these functions!!                                    *
 * ----------------------------------------------------- */

/* Routine for (A + B) mod M and, (A - B) mod M */
#define GTS_ADD_MOD( a, b, m ) ( ( (a) + (b) < (m) ) ? ( (a) + (b) ) : ( (a) + (b) - (m) ) )
#define GTS_SUB_MOD( a, b, m ) ( ( (a) >= (b) ) ? ( (a) - (b) ) : ( (m) + (a) - (b) ) )

/* Routine for adding or subtracting 2 GSM frame numbers */
#define GTS_ADD_FN_GSM( a, b ) GTS_ADD_MOD( (a), (b), GTS_GSM_FRAMES_PER_HYPERFRAME )
#define GTS_SUB_FN_GSM( a, b ) GTS_SUB_MOD( (a), (b), GTS_GSM_FRAMES_PER_HYPERFRAME )
#define GTS_SUB_FN_WCDMA( a, b ) GTS_SUB_MOD( (a), (b), GTS_WCDMA_MAX_SFN_COUNT )
#define GTS_SUB_FN_TDS( a, b ) GTS_SUB_MOD( (a), (b), GTS_TDS_ROLLOVER_Cx8 )

/* USTMR tick in 5secs (19.2e6 * 5) */
#define GTS_USTMR_TICK_5SECS  96000000L

/* USTMR tick in 30secs (19.2e6 * 5) */
#define GTS_USTMR_TICK_30SECS  576000000L

/* USTMR Ticks in 100 Us */
#define GTS_USTMR_PER_100_US  1920

/* Max Propagation for Latch Ms */
#define GTS_MAX_PROP_LATCH_MS 40

/* QTime Calibration related Params */
#define GTS_QTIME_CLK_LATCH_UNC_MS  0.0001 /* QTIME clock latch unc in msec */

/* Max TBF supported (for DPO) is 10 sec + some buffer */
#define GTS_MAX_TIME_LIMIT_FOR_QTIME_CALIB_MS  ((FLT) C_MAX_TIME_LIMIT_FOR_DPO_OPN * 1000)
/* Max integration time for DPO is 400ms. Calibration interval
   should be atleast 400ms */
#define GTS_MIN_TIME_LIMIT_FOR_QTIME_CALIB_MS ((FLT)400)

/* Max freq drift rate per sec */
#define GTS_MAX_FREQ_DRIFT_PER_SEC 0.2

/* Min Calibration UNC */
#define GTS_MIN_QTIME_CAL_UNC_MS 0.001

/* Macros to check for consistency in GPS RTC/USTMR latches sent by CC */
#define GTS_GPS_RTC_LATCH_JITTER  1
#define GTS_USTMR_LATCH_JITTER    1
/* 1 USTMR tick is 52.08ns, 1 GPS RTC tick is 12.22ns.
 * Thus, GPS RTC diff can be 4 +/- 1 ticks when USTMR diff is 1 tick */
#define GTS_GPS_RTC_CNT_ONE_USTMR_TICK    4
#define GTS_MIN_DIFF_GPS_RTC_USTMR_TICK   (GTS_GPS_RTC_CNT_ONE_USTMR_TICK - GTS_GPS_RTC_LATCH_JITTER)
#define GTS_MAX_DIFF_GPS_RTC_USTMR_TICK   (GTS_GPS_RTC_CNT_ONE_USTMR_TICK + GTS_GPS_RTC_LATCH_JITTER)

#define GTS_ADV_VALID_MAX_SAMPLE_SIZE    5

/* No of ms in a week 7*24*60*60*1000 */
#define GTS_NUM_OF_NSEC_IN_MSEC  1000000

#define GTS_DEFAULT_LEAP_SEC             18
#define GTS_DEFAULT_LEAP_SEC_UNC         5

/* Max TUNC (8.5secs) and FUNC (1ppm) values for pre-check */
#define GTS_PRECHECK_MAX_TUNC_MS      ((DBL)8500.0)
#define GTS_PRECHECK_MAX_FUNC_PPM  ((FLT)1.0)
#define GTS_MAX_PMICRTC_TUNC   	  ((DBL)(6500e-6))
#define GTS_PMICRTC_MS_ADJ_OFFSET     ((DBL)(6500e-6))

#define GTS_MAX_PMICRTC_TUNC_USB_CONNECTED         ((DBL)(10000e-6))
#define GTS_PMICRTC_MS_ADJ_OFFSET_USB_CONNECTED     ((DBL)(10000e-6))

#define GTS_PMIC_MAX_ERROR_RATE ((DBL)(15000e-6))

#define GTS_PMIC_USB_CONN_MAX_ERROR_RATE ((DBL)(20000e-6))

/* Max allowed diff between the WWAN provided PSM duration and EFS stored PSM dureation */
#define GTS_PMIC_PSM_DURATION_SYNC_LIMIT ((DBL)(60000))

/* HSFN rollover duration 1024 *1024*10 ms  */
#define GTS_PMIC_HSFN_ROLLOVER_DURATION  ((DBL)(10485760))

#define GTS_PMICRTC_RESOLUTION_ERROR_MS ((FLT)1.0) //1 milli secs error

/* additional error added to the corrected pmic value provided by WWAN */
#define GTS_PMIC_WWAN_CORRECTION_ERROR_MS 100

/* Delay timer to log PMIC F3's after bootup/reboot - 30 secs */
#define GTS_EFS_PMIC_LOG_TIMER_VALUE_MS   30000
#define GTS_EFS_DELAY_LOG_MAX_CTR    4

/* Minimum TUNC for HSFN SYNC PMIC src to be injected into ME/SM - 1 sec */
#define GTS_MIN_HSFN_SYNC_PMIC_TUNC_MS ((FLT)1000)

/* Max time diff (in ms) for using XO FEST (based on last PE fix) during propagation.
 * This value is chosen such that is slightly more than the worst case diff between 2
 * consecutive FEST updates. 
 * For non-CV2X PLs, worst case diff between FEST updates will be every 1 sec when GPS engine is OFF. 
 * And we give a grace of 500ms, leading to 1.5 secs or 1.5 * 19.2MHz = 23040000 */
#define GTS_MAX_DIFF_FOR_XO_FEST_PROP   28800000

/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/* Max Time Source elements */
/* These are also used to reference the DB array */
typedef enum
{
  GNSS_PE, /* 0 */
  GNSS_ME, /* 1 */
  LTE_ML1_SUB1, /* 2 */
  FREQEST, /* 3 */
  GSM_L1_SUB1, /* 4 */
  LTE_ML1_TTR_SUB1, /* 5 */
  CORE_SVC,         /* 6 */
  PMIC_RTC,       /* 7 */
  QTIME_CALIB,      /* 8 */
  EXT_INJ,      /* 9 */
  HSFN_SYNC_PMIC_RTC,      /* 10 */
  MAX_SRC
} gts_TimeSrcModuleType;

typedef enum
{
  /* Device state is not known */
  GTS_MOTION_STATE_UNKNOWN      = 0,
  /* Device state is Stationary */
  GTS_MOTION_STATE_STATIONARY   = 1,
  /* Device state is In Motion */
  GTS_MOTION_STATE_IN_MOTION    = 2,
  /* Out of Range */
  GTS_MOTION_STATE_MAX = GTS_MOTION_STATE_IN_MOTION
} gts_MotionType;


/* Common Time Transfer Data Structure */
typedef struct
{
  /* Indicate if the latch was successful */
  boolean b_Latch;
  /* GPS RTC value latched at the same time as CDMA RTC (in GPS Chipx32) */
  U32  q_GnssRtc;
  /* GNSS RTC sample and chip counts @ Time transfer latch */
  U32  q_SampleCount;
  /* GNSS RTC Phase count @ Time transfer latch */
  U32  q_GnssPhase;
  /* Universal STMR value latched at the same time as the first latched
     CDMA/GNSS RTC set (in 19.2MHz untis) */
  U32  q_Ustmr;
} gts_LatchType;

/* IPC Message Definitions */

/* Registration Message Info Type */
typedef struct
{
  gts_ClientType e_ClientInfo;
  gts_TimeEstCbType p_CbFunction;
} gts_RegisterMsgType;

/* Boolean to delete the time db */
typedef struct
{
  boolean b_ErrorRecovery;
} gts_DelTimeType;

/*parametes for efs read wrapper function */
typedef struct 
{
  fs_ssize_t l_BytesRead;
  boolean b_BadRead;
}gts_EFSReadParams;

/* High Accuracy Time Maintenance Mode Enable/Disable Message */
typedef struct
{
  boolean b_HighAccuMode;
} gts_HATModeMsgType;

typedef struct
{
  gts_MotionType e_MotionState;
  gts_QtimeType z_QtimeInfo;
} gts_MotionRptMsgType;

/* Position Estimate Message */
typedef struct
{
  gnss_PosVelSolutionStructType z_PosVelType;

  /* Latched Information */
  gts_LatchType z_LatchInfo;
  gts_QtimeType z_QtimeInfo;
} gts_PosEstMsgType;

/* Time Estimate Message from ME */
typedef struct
{
  U8 e_MESrc;  /* mgp_TimeEstPutSourceIDEnum */
  gps_ClockStructType z_GpsTimeEst;

  /* Qtime Information */
  gts_QtimeType z_QtimeInfo;

  /* FCount at Qtime */
  U32 q_FCountQtime;
} gts_METimeEstMsgType;

/* Diag Command Msg */
typedef struct
{
  U8 u_DiagData[GTS_MAX_DIAG_CMD_SIZE];
  U8 u_Size;
} gts_DiagCmdMsgType;

/* Time Storage Structures */
typedef struct
{
  /* DL frequency */
  U32 q_Freq;
  /* Cell ID */
  U16 w_CellId;

  U64 t_DlRefTime;
  U16 w_Sfn;

  /* Clocked @ 30.72MHz */
  U64 t_Ostmr;
  FLT  f_OstmrBias;

  gts_QtimeType z_Qtime;

  /* Determine if Default UNC is already included */
  boolean b_DefaultUncAdded;
} gts_LteInfoDBType;

typedef struct
{
  /* Cell Information */
  /* Base Station Identification code */
  U8 u_Bsic;
  /* Control Channel Frequency */
  ARFCN_T z_Arfcn;

  /* Time Information */
  /* Frame Number 0..(26L*51L*2048L-1)  ~4.615ms frames */
  U32 q_FrameNum;
  /* Time Slot Number 0..7 */
  U8 u_TimeSlot;
  /* QSymbol Count (sub fn) */
  U32 q_QSymCnt;

  /* GSM State */
  U8 u_GL1State;

  /* Addl UNC in us for this time input */
  U8 u_UncUs;

  /* Reference Time ( QTIME ) */
  gts_QtimeType z_QtimeInfo;

  /* Determine if Default UNC is already included */
  boolean b_DefaultUncAdded;
} gts_GsmInfoDBType;

/* Time Storage Structures */
/* ME Time Storage Structure */
typedef struct
{
  /* sys_sys_mode_e_type indicating which RAT */
  U8 b_Rat;

  /* Raw Clock Information */
  U32 q_Stmr;
  U32 q_StmrPh;
} gts_METimeCompType;

typedef struct
{
  /* GPS RTC @ the Time estimate */
  U32 q_GpsRtc;

  /* Source from which Time estimate was rcvd */
  mgp_TimeEstPutSourceIDEnum e_TimeEstSrc;

  /* Freq Bias information */
  gps_FreqStructType z_Freq;

  /* Clock information to do Comparison */
  gts_METimeCompType z_CompTime;
} gts_METimeType;

/* Position Storage structure */
typedef struct
{
  /* Position and Velocity information */
  gnss_PosVelSolutionStructType z_PosVel;
} gts_PosEstDbType;

typedef struct
{
  /* Indicates if the mobile moved since the last update */
  boolean b_Moved;
  /* Qtime at the earliest instance when motion was detected */
  gts_QtimeType z_FirstMoveTime;

  /* Type of movement detected at the last sensor input */
  gts_MotionType e_LastMoveType;
  /* Qtime at the last instance when motion was detected */
  gts_QtimeType z_LastMoveTime;
} gts_MotionInfoType;

typedef struct
{
  U64 t_PrevQtime;    /* Time at the last temp Update */
  U64 t_CurrQtime;    /* Time at the current temp Update */
  DBL d_CurrFreqEst;      /* Freq Est based on Temperature at Prev and Curr Time (in KHz) */
  FLT f_FreqEstUncMs; /* Time Uncertainity estimate based on temp table */
  DBL d_FreqEstPpb; /* Freq Est based on Temperature in PPB */
}gts_FreqEstDBType;


/* All Possible Time Source Modules */
typedef union
{
  gts_LteInfoDBType z_LteInfo;
  gts_GsmInfoDBType z_GsmInfo;
  gts_FreqEstDBType z_FreqEstInfo;
  gts_METimeType z_MeTimeInfo;
  gts_PosEstDbType z_PePosInfo;
} gts_InputInfoType;

/* Time Storage Database Element Definition */
typedef struct
{
  /* Element has valid information */
  boolean b_Valid;

  /* Indicate whether the item is blacklisted */
  boolean b_BlackListed;

  /* Source Module from which Time was recieved */
  gts_TimeSrcModuleType e_SrcModule;

  /* Input information for propagation */
  gts_InputInfoType z_InputInfo;

  /* Absolute GPS Time */
  gts_TimeType z_GPSTime;
} gts_TimeDbElementType;

/* Time Storage Database Definition */
typedef struct
{
  /* QTIME at the last Time info update */
  U64 t_LastUpdateTime;
  /* Module which provided the last Time update */
  gts_TimeSrcModuleType e_LastUpdateModule;
  /* Elements with Time information */
  gts_TimeDbElementType z_TimeDbElement[MAX_SRC];
} gts_TimeDbType;

typedef struct
{
  /* High Accuracy Time Update Enabled/Disabled */
  U16 b_HATModeEn : 1;
  /* High Accuracy Time Update Status - Enabled/Disabled */
  U16 b_HATStatus : 1;
  /* Use Punc based Tunc model */
  U16 b_UsePuncBasedTunc : 1;
  /* Enabling/Disabling DB Dump at Position Report */
  U16 b_EnDbDump : 1;
  /* When doing Dump Db log, should it be Raw Db or Prop Db*/
  U16 b_RawDbDump : 1;
  /* Determines if ME should use GTS Time for Regular sessions */
  U16 b_ForceUseGtsTime : 1;
  /* Determines if ME should use FT State based USTMR Unc */
  U16 b_UseFTStateUstmrUnc : 1;
  /* Determines if LTE Time transfer can be requested */
  U16 b_LteTtrAllowedSub1 : 1;
  /* Determines if default TUNC should be used */
  U16 b_UseMinTunc : 1;
  /* Determine if LTE time refresh disabled */
  U16 b_DisLteTimeRefresh : 1;
  /* Did deletion of time happened */
  U16 b_DbTimeDeleted : 1;
  /* Use Calibration from GTS */
  U16 b_CalibEn : 1;
  /* Use Slow Clk Source from GTS */
  U16 b_ExtInjEn : 1;
  //U16 : 0;
} gts_FlagsStructType;

/* Request Time Info structure */
typedef struct
{
  U8 b_InUse;
  gts_ClientType e_ClientInfo;
  gts_TimeEstCbType p_CbFunction;
  boolean b_RefreshRequested;
  U8 b_ContUpdates;
  gts_QtimeType z_LastUpdateQtime;
} gts_ClientsDbType;

/* Structure for collecting statistics abt TTr latches from CC */
typedef struct
{
  U32 q_NumTtrLatchSets;
  U32 q_NumTtrLatches;
  U32 q_NumTtrLatchErrorsPerSet;
  U32 q_NumTtrLatchErrorsTotal;
  U32 q_NumFirstValidFail;
  U32 q_NumTotalValidFail;
} gts_TtrLatchStatType;

/* GTS Paramaters */
typedef struct
{
  gts_FlagsStructType z_Flags;
  /* Diag based Input Source Blocking */
  U32 q_BlockSrc;

  /* GTS Clients with callback function */
  gts_ClientsDbType z_GtsClients[GTS_CLIENT_MAX];
  gts_MotionInfoType z_MotionInfo;
  gts_SrvSysInfoType z_SrvSys;

  /* USTMR Unc Ms based on FT State */
  DBL d_UstmrUncMs;

  /* Time Err that should be injected (only for test modes) */
  U8 u_InjTimeErrSec;

  /* Structure for collecting stats on TTr latches */
  gts_TtrLatchStatType z_TtrStat;

  /* Leap Sec Info */
  U8 u_LeapSec;
  U8 u_LeapSecUnc;

  /* GPS Group Delay Information (Upto 4 Seconds Delay Supported */
  U8 b_DelayStatus;
  U32 q_GroupDelayNs;

  /* Global variable to store the EFS read value for controlled event logging */
  U32 q_GtsEfsCntrlEvntLog;

  /* Global variable to store the NV value from MGP EFS to enable and disable TTR/1x transfer*/
  U32 q_NvEfsConfig;

  /* Flag to indicate that GTS module has been initialized */
  boolean b_GtsInitialized;

  /* Timer to delay EFS read */
  os_TimerMsgType *p_EfsReadDelayTimer;
  uint8 u_EfsLogCtr;
} gts_ParamsType;

typedef struct
{
  /* WWAN STMR registers at the same time as the last RTC Latch */
  U8 b_Rat;
  U32 q_RefUstmr;
  U32 q_WwanStmr;
  U32 q_WwanStmrPhase;
} gts_WwanTtrLatchType;

/* Database to store latched time transfer registers */
typedef struct
{
  boolean b_ClockStopped;
  U16 w_Cnt;
  gts_TtrRegDbElemType z_Elem[GTS_TTR_LATCH_REG_API_MAX_SIZE];
  gts_WwanTtrLatchType z_WwanTtr;
} gts_TtrRegDbType;

typedef struct
{
  boolean b_Valid;
  boolean b_Used;
  gts_LatchType z_Latch;
  gts_QtimeType z_Qtime;
  U32 q_FCount;
} gts_FCLatchType;

/* Structure for USTMR/Qtime calibration (using GPS time) */
typedef struct
{
  gts_QtimeClkCalStateType e_CalibState;
  gts_QtimeClkCalStateReasonType e_CalibStateReason;
  DBL d_PrevFCOffset;
  DBL d_QtimeFreqBiasM1;
  DBL d_QtimeFreqBiasM2;
  DBL d_ApproxClkFreqBias;
  DBL d_ClkFreqBias;
  DBL d_ClkFreqBiasUnc;
  gts_TimeType z_GtsTime1;
  gts_TimeType z_GtsTime2;
  gts_FCLatchType z_TicLatch;
  gts_FCLatchType z_WakLatch;
} gts_QtimeCalibStructType;

typedef struct
{
  uint32  q_SlowClkCount; /* Slow Clock count at latch */
  uint32  q_LatchFCount;  /* FCount value at slow clock latch */
  uint32  q_GnssCX80;  /* Cx80 at slow clock latch */
  boolean b_LatchAlreadyUsedForOffsetComp; /* Boolean which indicates if this latch has been already used for offset computation*/
} gts_QtimeLatchType;

/* Leap Second Message from ME */
typedef struct
{
  U32 q_GpsMsec;
  U8 u_LeapSec;
  U8 u_LeapSecUnc;
} gts_LeapSecMsgType;

/*
 *  This structure holds the GPS time and PMIC RTC time.  These structures
    are read from EFS during startup and written to EFS at
 *  modem shutdown.
 */

typedef struct
{
  boolean   b_RTCValid;            /* RTC-to-GPS relationship validity flag */

  /* Time Source */
  gts_TimeSrcType e_TimeSrc;

  /* Absolute GPS Time related information */
  uint16    w_GpsWeek;      /* GPS week at time of EFS write (in Ms) */	
  uint32    q_GpsMsec;      /* GPS msec at reference tick [msecs] at time of EFS write (in Ms) */
  float     f_ClkTimeBias;  /* Clock bias [msecs]. Constraint to >-0.5 and <0.5  */
  float     f_ClkTimeUncMs;  /* RTC at time of EFS write (in Ms) */
  pm_hal_rtc_time_type q_PmicRtcTime;/*Pmic Rtc time ins seconds */
}gts_GpsPmicrtcDataStruct;

/*
 *  This structure holds the GPS time,PMIC RTC time,error conditions/flags if any condition fails and will be stored
 *  in db after timer expiry. This helps to know which condition failed and helps to debug
*/
typedef enum
{
  GTS_EFS_READ_NO_ERROR,
  GTS_EFS_READ_BAD_FD_READ,
  GTS_EFS_READ_BAD_CHECKSUM,
  GTS_EFS_READ_BAD_BYTES_READ,
  GTS_EFS_READ_INVALID_EFS,
  GTS_EFS_PMIC_READ_ERROR,
  GTS_EFS_WEEK_UNKNOWN,
  GTS_EFS_MKDIR_ERR,
  GTS_EFS_PMIC_NEG_DIFF
} gts_PmicEfsReadErrorCause;

typedef struct
{
  gts_TimeType z_PrevPmicGtsTime; /* EFS contents */
  gts_TimeType z_CurrGtsTime; /* GTS time after propogation */
  gts_TimeType z_CorrectedGtsTime; /* Corrected GTS time after HSFN exit after propogation */
  int FdRead; /* File open descriptor checking  flag */
  int32 l_BytesRead; /* Number of bytes read */
  uint16 w_CsumOfEfsRead; /* Checksum of EFS data */
  pm_err_flag_type e_PmErr; /* Pmic Flag success checking */
  pm_rtc_time_type z_CurrPmicRtc; /* Current PmicRtc time */
  pm_rtc_time_type z_PrevPmicRtc; /* Previous PmicRtc time */
  DBL d_DiffPmicRtcMs;
  gts_PmicEfsReadErrorCause e_ErrReading; /* Enum to indicate any error in reading PMIC */
  int32 l_EfsMkDirErrNum; /* Error cause if Make Dir fails */
  int32 l_EfsOpenErrNumInit; /* Error cause if EFS open at init fails */
  int32 l_EfsReadErrNumInit; /* Error cause if EFS read at init fails */
  U32 q_WwanPsmDiff;/* Provided PSm duration based on HSFN diff */
  U32 q_DiffOfDiffPsmEfsread;/* diff b/w WWAN provided psm duration and EFS stored psm duration */
  U32 q_CorrPsmDiffMsec;/* final corrected psm fiss based on HSFn diff during psm cycle*/
  gts_PmicTimeCorrectionType z_PmicTimeCorr; /* structure to store WWAN PMIC correction info */
  uint32 q_UsbNvEnabled;/* flag to check usb nv enabled or not */
  uint8 u_HsfnWhitelistCounter; /* counter to whitelist the HSFN source */
}gts_PmicErrDatabaseType;

/* variables to store PMICRTC information for debug purpose after the timer expiry */
extern gts_PmicErrDatabaseType gts_PmicErrDb;

/*
 *  This is the actual structure of the EFS data as it resides in EFS.
 *  It consists of some header information (used for error checking)
 *  and the data itself.
 */
typedef struct {
  uint16          w_Xsum16;   /* Sum of the data section only */
  uint16          w_Len;      /* Length of data section only (in bytes) */
} gts_GpsEfsHdrStruct;

typedef struct
{
  pm_rtc_time_type z_PmicRtc; /*Pmic Rtc time in secs and msecs */
  gts_TimeType z_GtsTime;
  uint8 u_HsfnWhitelistCounter; /* counter to whitelist the HSFN source */
} gts_PmicAndGtsDataType;

typedef struct
{
  gts_GpsEfsHdrStruct	 z_Hdr;
  gts_PmicAndGtsDataType z_Data;
}gts_EfsPmicRtcStruct;

/*--------------------------------------------------------------------------
 * Variable Prototypes
 *-----------------------------------------------------------------------*/
/* To be used in logging module */
extern gts_QtimeCalibStructType gts_QtimeClkCalib;

/* gts_Params.q_GtsEfsCntrlEvntLog To be used in logging module */
extern gts_ParamsType gts_Params;


/*--------------------------------------------------------------------------
 * Function Prototype Declarations
 *-----------------------------------------------------------------------*/

/*
 ******************************************************************************
 * Function: gts_LatchGpsClock
 *
 * Description:
 *  This function calls the Time Latching Function which in turn captures
 *  USTMR and GPS RTC
 *
 * Parameters:
 *  z_LatchInfo - GPS and USTMR is latched and updated in this structure
 *  z_QtimeInfo - Qtime captured close to the latch
 *
 * Dependencies:
 *  The capture is done blocking interrupts
 *
 * Return value:
 *  None
 *
 ******************************************************************************
 */
void gts_LatchGpsClock(gts_LatchType *pz_LatchInfo, gts_QtimeType *pz_QtimeInfo);

/*
 ******************************************************************************
 * Function gts_InputSrcDisabled
 *
 * Description:
 *
 *  Returns current status of Diag based blocking input source from input modules
 *
 * Parameters:
 *
 *  e_ModSrc - Source which should be checked
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  Returns the TRUE if the source should be blocked, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_InputSrcDisabled(gts_TimeSrcModuleType e_ModSrc);

/*
 ******************************************************************************
 * Function gts_FindBestGpsTime
 *
 * Description:
 *
 *  Runs through the time database to pick up the best time estimate
 *
 * Parameters:
 *
 *  pz_TimeInfo - Pointer in which the best time is updated
 *  pz_Qtime - Qtime at which the best time is estimated
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  Source ID from which the time was derieved, if the time couldn't be 
 *  estimated, then MAX_SRC will be returned. 
 *
 ******************************************************************************
*/
gts_TimeSrcModuleType gts_FindBestGpsTime( gts_TimeType *pz_TimeInfo,
                                                  const gts_QtimeType* pz_Qtime );


/*
 ******************************************************************************
 * Function gts_GetGtsTimeNow
 *
 * Description:
 *
 *  This function reads the current time from GTS module. The possible sources
 *  are ME (GPS Fix, Timetag, Timetransfer etc) or FreqEst based time or
 *  LTE based Time
 *
 * Parameters:
 *
 *  pz_TimeEstMsg - Pointer to the GTS Time structure - This will be updated with
 *  Time information
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  Module SRC ID of the GTS time. If time is not valid, SRC ID will be MAX_SRC
 *
 ******************************************************************************
*/
gts_TimeSrcModuleType gts_GetGtsTimeNow(gts_TimeType *pz_TimeEst, gts_TimeSrcModuleType e_Src);

/*
 ******************************************************************************
 * Function gts_ProcFreqEstMsg
 *
 * Description:
 *
 *  This function will use the Freq Estimate Error to propagate last known
 *  GPS Fix time which was based on Freq Est to current Qtime/Slow Clock Time
 *
 * Parameters:
 *
 *  pz_FreqEst - Freq Estimate Information
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
void gts_ProcFreqEstMsg(gts_FreqEstInfoType *pz_FreqEst);

/*
 ******************************************************************************
 * Function gts_ProcPosEstMsg
 *
 * Description:
 *
 *  This function handles the Position Est Message from PE which has GPS Time
 *  from Position Fix
 *
 * Parameters:
 *
 *  pz_PosEst - Position Estimate Message ( includes Time as well )
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
void gts_ProcPosEstMsg(gts_PosEstMsgType *pz_PosEst);

/*
 ******************************************************************************
 * Function gts_ProcTimeEstRptMsg
 *
 * Description:
 *
 *  This function handles the Time estimate from Measurement Engine
 *
 * Parameters:
 *
 *  pz_TimeEstMsg - Time Estimate Report Message
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
void gts_ProcTimeEstRptMsg(const gts_METimeEstMsgType *pz_TimeEstMsg);

/*
 ******************************************************************************
 * Function gts_ProcTimeEstReqMsg
 *
 * Description:
 *
 *  This function handles the Time estimate request message from any client. 
 *  Time estimate is inturn sent to the client using the call back function 
 *  which they have registered during Init
 *
 * Parameters:
 *
 *  pz_TimeEst - Time Estimate Report Message
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
void gts_ProcTimeEstReqMsg();

/*
 ******************************************************************************
 * Function gts_ProcMotionRptMsg
 *
 * Description:
 *
 *  Handle the motion data from Sensors ( sent by SLIM module )
 *  Motion data is mainly used to restrict the uncertainity of WWAN Time
 *
 * Parameters:
 *
 *  pz_MotionRptMsg - Motion data Msg
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
void gts_ProcMotionRptMsg(const gts_MotionRptMsgType *pz_MotionRptMsg);

/*
 ******************************************************************************
 * Function gts_DelTime
 *
 * Description:
 *
 *  Delete GTS Time Database
 *  
 * Parameters: 
 *
 *  uint8 - Reason for the deletion
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
void gts_ProcDelTime( gts_DelTimeType *p_Data );

/*
 ******************************************************************************
 * Function gts_ProcHATModeMsg
 *
 * Description:
 *
 *  This function handles the High Accuracy Mode Msg which indicates that
 *  Background positioning is enabled and need  high accuracy time with a
 *  increased power consumption. This will enable Time maintenance using
 *  Slowclock and Temperature compensation
 *
 * Parameters:
 *
 *  pz_HAModeMsg - High Accuracy Mode Msg
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
void gts_ProcHATModeMsg(const gts_HATModeMsgType *pz_HAModeMsg);

/*
 ******************************************************************************
 * Function gts_ProcHATStatusMsg
 *
 * Description:
 *
 *  This function handles the High Accuracy Status Msg which indicates that
 *  high accuracy time updates are enabled in FREQEST module
 *
 * Parameters:
 *
 *  pz_HATStatus - High Accuracy Status Msg
 *
 * Dependencies:
 *
 *  Client should have already registered with GTS 
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_ProcHATStatusMsg(const gts_HATStatusMsgType *pz_HATStatus);

/*
 ******************************************************************************
 * Function gts_ProcDiagCmdMsg
 *
 * Description:
 *
 *  Handle the diag commands IPC
 *
 * Parameters:
 *   pz_DiagMsg - Pointer to the Diag command Message
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
void gts_ProcDiagCmdMsg(gts_DiagCmdMsgType *pz_DiagMsg);

/*
 ******************************************************************************
 * Function gts_ProcLteTimeMsg
 *
 * Description:
 *
 *  Processes LTE Time Info Msg. This Function will propagate the last LTE
 *  based GPS Time to current LTE Time.
 *
 * Parameters:
 *
 *  pz_LteTime - LTE Time Information
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
void gts_ProcLteTimeMsg(const gts_LteTimeInfoType *pz_LteTime);

/*
 ******************************************************************************
 *
 * Function:
 *
 *   gts_ProcPeriodicLteTtr
 *
 * Description:
 *
 *  This function is store SIB8/SIB16 time from LTE time transfer info.
 *
 * Parameters:
 *
 *  LTE Time Transfer Report structure.
 *
 * Return value:
 *
 *  None
 *
  ******************************************************************************
*/
void gts_ProcPeriodicLteTtr(const gts_LteTimeInfoType *p_TTInfo);


/*
 ******************************************************************************
 * Function gts_ProcGsmTimeMsg
 *
 * Description:
 *
 *  Processes GSM Time Info Msg. This Function will propagate the last GSM
 *  based GPS Time to current GSM Time.
 *
 * Parameters:
 *
 *  pz_GsmTime - GSM Time Information
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
void gts_ProcGsmTimeMsg(const gts_GsmTimeInfoType *pz_GsmTime);

/*
 ******************************************************************************
 * Function gts_ProcTtrRegLatchMsg
 *
 * Description:
 *  This Function will process the time transfer registers sent by CC.
 *
 * Parameters:
 *  p_TtrLatch - Pointer to maximum of 20 register latches
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
 */
void gts_ProcTtrRegLatchMsg(const gts_TtrRegLatchInfoType *pz_TtrLatchInfo);

/*
 ******************************************************************************
 * Function gts_ProcSSChangeMsg
 *
 * Description:
 *  Handle the diag commands IPC
 *
 * Parameters:
 *   pz_SSInfo - Pointer to the Serving System Change Message
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/
void gts_ProcSSChangeMsg(gts_SSInfoType *pz_SSInfo);

/*
 ******************************************************************************
 * Function gts_GpsMsecWeekLimit
 *
 * Description:
 *  gts_GpsMsecWeekLimit is a helper function used to perform the 
 *  fairly common check to see if a msec is indeed with the allowable
 *  range of 0 thru WEEK_MSECS-1. Only values within a single week
 *  are entertained. (ie- Multiple week adjustments are not performed
 *
 *  Assigning a NULL pointer to p_GpsWeek will disable the week 
 *  adjustment logic ... ie - The p_GpsMsec will only be affected.
 *
 * Parameters: 
 *  p_GpsMsecs - Pointer to the msec value under test
 *  p_GpsWeek - Pointer to week number which may be adjusted
 *
 * Dependencies:
 *  None
 *
 * Return value: 
 *  void
 *
 ******************************************************************************
*/
void gts_GpsMsecWeekLimit(S32 *p_GpsMsecs, U16 *p_GpsWeek);

/*
 ******************************************************************************
 * Function gts_GetTtrDbCopy
 *
 * Description:
 *  This Function will copy the DB into a local DB structure
 *
 * Parameters:
 *  p_TtrLatchRegSet - Pointer to provide the latched register values from DB
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
 */
void gts_GetTtrDbCopy(gts_TtrRegDbType *p_TtrDb);

/*
 ******************************************************************************
 *
 * Function
 *   gts_QtimeDiffMs
 *
 * Description:
 *  Calculate difference between two QTIME values ( Qtime2-Qtime1 )
 *
 * Parameters:
 *  pz_Qt1 - Qtime at Instance 2
 *  pz_Qt2 - Qtime at Instance 1
 *  pd_Diff - Difference value in Ms is updated to this pointer
 *
 * Return value:
 *  TRUE if the calculation was successful, FALSE otherwise
 *
  ******************************************************************************
*/

boolean gts_QtimeDiffMs(const gts_QtimeType *pz_Qt2,
                        const gts_QtimeType *pz_Qt1,
                        DBL *pd_Diff);

/*
 ******************************************************************************
 * Function: 
 *  gts_GetDbElem
 *
 * Description:
 *  This function returns pointer to a db Element based on Source Module Type
 *
 * Parameters:
 *  e_Mod - Source Module for the DB
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  Pointer to the DB element of type gts_TimeDbElementType*
 *
 ******************************************************************************
 */
gts_TimeDbElementType* gts_GetDbElem(gts_TimeSrcModuleType e_Mod);

/*
 ******************************************************************************
 *
 * Function
 *   gts_ConstructUstmr64
 *
 * Description:
 *  Create a 64 bit USTMR based on 24 bit USTMR and 64 bit Dal Time.
 *  DAL Time should always be captured close to USTMR with max error of
 *  400 ms ( half of USTMR rollover )
 *
 * Parameters:
 *  w_Ustmr - 24 bit USTMR value
 *  t_DalTime - 64 bit Dal Time from Time tick
 *
 * Return value:
 *  64 bit USTMR value
 *
  ******************************************************************************
*/
U64 gts_ConstructUstmr64(const U32 w_Ustmr, const U64 t_DalTime);

/*
 ******************************************************************************
 * Function gts_GetLeapSecAndUnc
 *
 * Description:
 *
 *  Get the Leap Sec Msg
 *
 * Parameters:
 *
 *  p_LeapSec - Leap Second value
 *  p_LeapSecUnc- Leap secoond unc value
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

void gts_GetLeapSecAndUnc(U8 *p_LeapSec, U8 *p_LeapSecUnc);

/*
 ******************************************************************************
 * Function 
 *  gts_IsWwanActive
 *
 * Description:
 *  Checks if the particular WWAN is active in any Sub/Stack
 *
 * Parameters:
 *  e_SysMode - WWAN that has to be checked if it is active or not
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  True - WWAN is one of the active RATs
 *  False - WWAN is not active in any Subs or Stacks
 *
 ******************************************************************************
*/
boolean gts_IsWwanActive(sys_sys_mode_e_type e_SysMode);

/*
 ******************************************************************************
 * Function gts_TimeRefreshUpdate
 *
 * Description:
 *
 *  This Function will be called by GTS TTR to indicate there was a time update 
 *  due to CDMA Time or LTE Time owing to time refresh or a timer expired when
 *  a time refresh was in process
 *
 * Parameters:
 *
 *  b_Successful - Was the refresh successful or a timeout occured
 *  e_SrcModule - Source from which the time was refreshed
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
void gts_TimeRefreshUpdate(boolean b_Successful,
                           gts_TimeSrcModuleType e_SrcModule);

/*
 ******************************************************************************
 *
 * Function
 *  gts_RestrictClkBias
 *
 * Description:
 *  Restrict the time-bias to be within [-0.5, 0.5]ms and updates GPS RTC Ms
 *  accordingly
 *
 * Parameters:
 *  p_GpsTime - pointer to the GPS time.
 *
 * Return value:
 *  None
 *
  ******************************************************************************
*/
void gts_RestrictClkBias(gts_TimeType *pz_GpsTime);

/*
 ******************************************************************************
 *
 * Function:
 *
 *   gts_ConstructGpsRtc
 *
 * Description:
 *
 *  This function is used to obtain a new GPS RTC based on a base GPS RTC and positive msec diff.
 *  Note: The RTC is constructed for GPS system (BP1, BP2 or BP4) at full rate.
 *
 * Parameters:
 *
 *  Base GPS RTC and time (in msec) to advance the RTC
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
 */
U32 gts_ConstructGpsRtc(const DBL d_TimeDiffMs, const U32 q_BaseGpsRtc);

/*
 ******************************************************************************
 * Function gts_UstmrUncMs
 *
 * Description:
 *
 *  Return USTMR Unc in Us/Second
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
 *  Ustmr Unc in PPM
 *
 ******************************************************************************
*/
DBL gts_UstmrUncMs(void);

/*
 ******************************************************************************
 * Function gts_GetTtrDb
 *
 * Description:
 *
 *  This Function will return the pointer to the DB
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
gts_TtrRegDbType* gts_GetTtrDb(void);

/*
 ******************************************************************************
 *
 * Function
 *   gts_TimeDiffMs
 *
 * Description:
 *  Calculate difference between two GTS Time values ( T2-T1 )
 *
 * Parameters:
 *  pz_T2 - GTS Time at Instance 2
 *  pz_T1 - GTS Time at Instance 1
 *  pd_Diff - Difference value in Ms is updated into this pointer
 *
 * Return value:
 *  TRUE if the calculation was successful, FALSE otherwise
 *
  ******************************************************************************
*/

boolean gts_TimeDiffMs(const gts_TimeType *pz_T2,
                       const gts_TimeType *pz_T1,
                       DBL *pd_Diff);

/*
 ******************************************************************************
 * Function gts_EstGpsTimeAtQtime
 *
 * Description:
 *
 *  Estimates GPS Fix time at a Qtime instant
 *
 * Parameters:
 *
 *  pz_OrigGpsTime - Original GPS Fix Time
 *  pz_TargQtime   - Target Qtime where the GPS Time has to be estimated
 *  pz_TargGpsTime - Estimated GPS Fix Time
 *  e_SrcModule - Source of GTS time
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
void gts_EstGpsTimeAtQtime(const gts_TimeType *pz_OrigGpsTime,
                           const gts_QtimeType *pz_TargQtime,
                           gts_TimeType *pz_TargGpsTime,
                           const gts_TimeSrcModuleType e_SrcModule);

/*
 ******************************************************************************
 * Function: gts_CopyGpsTimeToGtsTime
 *
 * Description:
 *  This function Copies GPS Time into GTS Time structure
 *
 * Parameters:
 *  pz_GpsTime - GPS Clock Structure
 *  pz_GtsTime - GTS Clock Structure
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
 */
void gts_CopyGpsTimeToGtsTime(const gps_ClockStructType *pz_GpsTime,
                              gts_TimeType *pz_GtsTime);


/*
 ******************************************************************************
 * Function: gts_UpdComDbInfo
 *
 * Description:
 *  Updates the database common info with the module info
 *
 * Parameters:
 *  e_Mod - Source Module which is updating common Db info
 *  pz_Qtime - Qtime at which the last time update was made
 *
 * Dependencies:
 *  None
 *
 * Return value:
 *  None
 *
 ******************************************************************************
 */
void gts_UpdComDbInfo(gts_TimeSrcModuleType e_Mod, gts_QtimeType *pz_Qtime);

/*
 ******************************************************************************
 * Function gts_GetFCount
 *
 * Description:
 *
 *  Used by MC to determine if it should use GTS time instead of requesting
 *  Time tag or Time Transfer
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
 *  TRUE - If GTS Time should be used by MC for regular GPS Sessions
 *
 ******************************************************************************
*/
boolean gts_GetFCount(U32 *p_FCountNow);

/*
 ******************************************************************************
 * Function gts_CalcFCount32
 *
 * Description:
 * This function constructs the 32-bit FCount of a GPSRTC using a
 * Reference FCount. The GPSRTC FCount does not have the full 32-bits
 * of milliseconds valid. So this function populates the higher order
 * bits from the Reference FCount supplied
 *
 * Parameters:
 *  q_RefFCount  - 32-bit Reference FCount
 *  q_RtcFCount  - 16-bit FCount from the GPSRTC 
 *
 * Dependencies:
 *  The Reference FCount and FCount should have been captured
 *  within a few seconds (less than 32 seconds).
 *
 * Return value:
 *  32-bit FCount value
 *
 ******************************************************************************
 */

uint32 gts_CalcFCount32(uint32 q_RefFCount, uint32 q_RtcFCount);

/*
 ******************************************************************************
 * Function gts_GetWwanTtrLatch
 *
 * Description:
 *
 *  This Function will return the latest latched WWAN TTr register set
 *
 * Parameters:
 *
 *  p_WwanTtr - Pointer to provide the latched register values from DB.
 *  e_Src - Module that is requesting for GPS RTC at slow clock boundary
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
void gts_GetWwanTtrLatch(gts_WwanTtrLatchType *p_WwanTtr, gts_GetRtcReqSrcType e_Src);

/*
 ******************************************************************************
 * Function gts_SetCalibState
 *
 * Description:
 *
 *  This function is used to set or invalidate calibration.
 *  This function is called during task startup or when the last fix made was
 *  more than 12 seconds ago. (12 seconds is chosen as the limit - assuming
 *  max TBF of 10 seconds)
 *
 * Parameters:
 *
 *  e_CalibState - Whether Qtime calibrated or uncalibrated
 *  e_CalibStateReason - Event which set or invalidate calibration
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
void gts_SetCalibState( const gts_QtimeClkCalStateType e_CalibState, 
                               const gts_QtimeClkCalStateReasonType e_CalibStateReason );

/*
 ******************************************************************************
 * Function gts_GetLatchStats
 *
 * Description:
 *
 *  This Function will return the TTR latch Stats
 *
 * Parameters:
 *
 *  pz_GtsLatchStats - Structure with Stats related to latches
 *  w_CurrLatchCount - Number of current latches Pointer
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
void gts_GetLatchStats(gts_TtrLatchStatType *pz_GtsLatchStats,
                       U16 *w_CurrLatchCount);

/*
 ******************************************************************************
 * Function gts_GetBestGpsFixTime
 *
 * Description:
 *
 *  Takes the latest GPS Fix time and propagates it using combination of GPS
 *  RTC/USTMR latch DB to the Qtime provided in the input parameter.
 *
 * Parameters:
 *
 *  pz_TimeInfo - Pointer in which the best time is updated
 *  pz_Qtime - Qtime at which the best time is estimated
 *  b_PropToLatchDb - Boolean flag to propogate upto the USTMR/GPS RTC latch DB only
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if the time was valid and operation was successful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_GetBestGpsFixTime(gts_TimeType *pz_TimeInfo, const gts_QtimeType *pz_Qtime,
                              boolean b_PropToLatchDbOnly);

/*
 ******************************************************************************
 * Function gts_GetGtsFreqNow
 *
 * Description:
 *
 *  This function reads the current Freq Error 
 *
 * Parameters:
 *
 *  pz_FreqEst - Pointer to the GTS Freq structure 
 *  e_Src - Freq Err source
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if Time was filled in correctly, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_GetGtsFreqNow(gps_FreqStructType *pz_FreqEst,
                          const gts_TimeType *pz_GtsTime);

/*
 ******************************************************************************
 * Function gts_PosVelUpdNow
 *
 * Description:
 *
 *  This function reads the current Position Information like Velocity, PUNC 
 *
 * Parameters:
 *
 *  f_PosUnc  - Position Unc in meter/sec is updated 
 *  f_VelocityMs - Velocity of the device in meter/sec
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if Time was filled in correctly, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_PosVelUpdNow(FLT *f_PosUnc,
                         FLT *f_VelocityMs);

/*
  ******************************************************************************
  * Function gts_SavePmicToEfs.
  *
  * Description:
  *
  *  This function writes GPS PMIC data structure to the EFS
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

void gts_SavePmicToEfs(void);

/*
 ******************************************************************************
 * Function: 
 *   gts_InvalidatePmicRtc
 *
 * Description:
 *   This function invalidates the PmicRtc Gps data structure.
 *
 * Parameters:
 *   None
 *
 * Return value:
 *   None
 *
 ******************************************************************************
*/
void  gts_InvalidatePmicRtc(void);

/*
 ******************************************************************************
 * Function: 
 *   gts_PmicWrite
 *
 * Description:
 *   This function writes Pmic Gps data strucure to the EFS.
 *
 * Parameters:
 *   None
 *
 * Return value:
 *   None
 *
 ******************************************************************************
*/	
void gts_PmicWrite(void);

/*
 ******************************************************************************
 * Function: 
 *   gts_ReadPmicFromEFS
 *
 * Description:
 *   This function reads PmicRtc GPS structure from the EFS.
 *
 * Parameters:
 *   None
 *
 * Return value:
 *   None
 *
 ******************************************************************************
*/
void gts_ReadPmicFromEFS(void);


/*
 ******************************************************************************
 * Function gts_checksum16
 *
 * Description:
 *
 *  This function calculates a 16 bit wide checksum of bytes.  NOTE - The
 *  checksum is equal to the one's complement of the sum of all the bytes in
 *  the array.
 *
 * Parameters:
 *
 *  p_Data - Pointer to an array of bytes
 *  n_bytes - Number of bytes to be checksummed
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  w_xsum16 - The 16-bit summation of all of the bytes
 *
 ******************************************************************************
*/
static U16 gts_Checksum16(const uint8 *p_Data, uint16 n_Bytes);

/*
 ******************************************************************************
 * Function gts_GetCoreSvcTime
 *
 * Description:
 *
 *  This function reads time from Core Services and updates the DB
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
//void gts_GetCoreSvcTime();

/*
 ******************************************************************************
 * Function gts_GetGpsPathDelay
 *
 * Description:
 *
 *  Used to get GPS Path Delay Information
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
 *  TRUE - If GTS Time should be used by MC for regular GPS Sessions
 *
 ******************************************************************************
*/
U32 gts_GetGpsPathDelay();

/*
 ******************************************************************************
 *
 * Function
 *   gts_ProcInjectAbsTime
 *
 * Description:
 *  Function to process IPC sent by external modules to inject absolute time (in GPS time format)
 *  into GTS. The modules could be external time injection from XTRA/NTP, PMIC based propogation etc.
 *
 * Parameters:
 *  p_AbsTime - GTS Time from other module
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/
void gts_ProcInjectAbsTimeMsg(const gts_TimeType *p_AbsTime);

/*
 ******************************************************************************
 *
 * Function
 *  gts_ProcTimePmicCorrection
 *
 * Description:
 *  Function used to process IPC which will use provided Corrected Pmic Error in ms  after HSFN syncronisation
 *
 * Parameters:
 *  pz_PmicTimeType - Corrected Pmic Time Type
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/

void gts_ProcTimePmicCorrection(const gts_PmicTimeCorrectionType *pz_PmicTimeType);

/*
 *******************************************************************************
 *
 * Function:
 *  gts_EFSControlEventLog
 *
 * Description:
 *  This function reads the efs to Control GTS Event Logging.
 *
 * Return value:
 *  Value to enable/diable GTS event logging
 *
 *******************************************************************************
*/
void gts_EFSControlEventLog();

/*
 ******************************************************************************
 * Function:
 *  gts_EFSRead
 *
 * Description:
 *  This is the common function for efs read inside GTS
 *
 * Return value:
 *  gts_EFSReadParams
 *
 ******************************************************************************
*/
void gts_EFSRead(const char *p_Path, void *p_Data, uint32 q_Size, gts_EFSReadParams *pz_ReadStatus);

/*
 *********************************************************************************
 * Function:
 *    gts_EFSWrite
 *
 * Description:
 *  This is the common function for efs write inside GTS
 *
 * Return value:
 *  None
 *********************************************************************************
*/
int gts_EFSWrite(const char *p_Path, void *p_Data, uint32 q_Size);

#endif //#ifndef _GTS_H_


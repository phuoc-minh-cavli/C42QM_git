/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          MGP API Header File

GENERAL DESCRIPTION
  This file contains contains the structure definitions and function prototypes for 
  the MGPSS Core software interface.

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2021 Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.


Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/mgp/inc/mgp_api.h#3 $ $DateTime: 2021/02/01 02:05:45 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/16   mk      deprecate & replace mgp_ReceiverOn/Off/Idle/ForcedOff/ForcedIdle by _mgp_ReceiverUpdate
09/09/15   am      mgp_SetSbasCannedSvList accepts uint64 bit mask
05/07/15   mv      Support for constellation configuration API
02/28/13   vr      Support for injecting vehicle gyro and odometry data
12/31/10   spn     Support for Positioning Method
01/29/10   pn      Updated DmLog structs with PACKED_POST.
09/16/09   jd      Added support for Phase II RF HW Status logging.
08/05/09   mwb     Added support for MobiSens high-power mode
07/30/09   gk      Added support for On-demand positioning 2.0.
06/05/09   jlp     Added support for injecting Sparse network Region(s)
01/21/09   kinw    Added support for XTRA-T upload.
11/06/08   jlp     Added support for getting/setting the gps restriction level.
07/17/09   jd      Added _mgp_SendDiagRfCmd() for HW Config cmd over QXDM
07/17/09   jd      Added _mgp_RfBPStartInit() and _mgp_RfBPStopInit() for 
                   RF Boot up calibration.
10/27/08   jlp     Added interface to set Sv's that we should not search for
                   or track.
07/25/08   gk      Force receiver off changes
07/02/08   ank     Added LBS support for CDG OMH R-UIM Spec
06/12/08    lt     Support for external coarse position injection.
04/25/08   gk      On-Demand feature changes
02/14/08    jw     Support for Optimistic Punc
12/18/07   lt      Added support for disabling time-alignment in periodic MSA.
07/31/07   br      Added support for 1x Allow GPS interface.
05/31/07   jw      Added Rcvr memconfig type (Hi or Lo Mem)
05/07/07   jw      Added Diag GPS Intelliciever support.
05/07/07   br      Added support for running PPM in MC task.
05/05/07   br      Added support for RF Info interface.
04/24/07    ld     Support for CELLDB (Interim Check-in)
04/23/07   jw      Flow the disable bit edge required before track flag to MC.
04/19/07    ld     Intermediate check-in for UMTS Time-tagging
04/17/07    ld     Modified MGP Tick Time to 1 second
02/20/07   br      Added support for TCXO Manager Rotator Push.
01/24/07    jw     Added new MGP Time Tick functionality
11/17/06   br      Added 1x SRCH interface.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


#include "gps_variation.h"
#ifdef WIN32
#pragma once
#endif


#ifndef MGP_API_H
#define MGP_API_H

/*
 * Include files
*/
#include "customer.h"
#include "comdef.h"
#include "gps_types.h"
#include "gps_rrc_common.h"
#include "gnss_consts.h"
#include "gnss_common.h"
#include "aries_os_globals.h"
#include "cgps_api.h"
#include "gnss_api.h"
#include "pdapi.h"
#include "pdapi_lite.h"
#include "cm.h"
#include "mgp_pe_sensors_api.h"
#include "lsmp_api.h" 
#include "slim_client_msg_types.h"
#ifdef FEATURE_GNSS_LTE_CATM_COEX
  #include "lte_cphy_msg.h"
#endif /* FEATURE_GNSS_LTE_CATM_COEX */

/*
 * Constant definitions
*/
//#define MAX_PPM_RM_RPT_SAMPLES 25
#define RAD2DEG (180.0 / C_PI)
#define DEG2RAD (C_PI / 180.0)
#define TWO_TO_25 33554432.0
#define LAT_LONG_SCALE (TWO_TO_25 / 180.0)
#define C_GLONASS_BAND_RECENTER_HZ (double) -281250.0
#define C_GEN9VTV2_GLONASS_G1_BAND_RECENTER_HZ (double) -281000.0
#define C_GEN9VTV3_GLONASS_G1_BAND_RECENTER_HZ (double) -281000.0
#define C_GEN9VTV2_GLONASS_G2_BAND_RECENTER_HZ (double) -219000.0
#define C_GLO_FREQ_OFFSET_HZ (double) 562500.0
#define C_GLO_G2_FREQ_OFFSET_HZ ((double)437500.0)

/* Define the Tick period in milliseconds */
#define MGP_TICK_TIME_MS 1000

/* Definition for half the tick time interval in milliseconds */
#define C_MGP_HALF_TICK_TIME_MS 500

/*Maximum duration for which MGP could enter DPO type sleep*/
#define C_MGP_DPO_MAX_SLEEP_TIME_MS 10000

/*
 * Macro definitions
*/
#define SBAS_PRN(x)  (((x) >= C_FIRST_SBAS_SV_PRN) && ((x) <= C_LAST_SBAS_SV_PRN))


/* Max buffer size of Diag commands */
#define MGP_MAX_DIAG_CMD_SIZE   36

/*
 * Structure definitions
*/

#if defined(WIN32) && defined(ADS_120_COMPILER_4_BYTE_ALIGNMENT)
#pragma pack(push, MGP_PE_COMMON_HH, 4)
#endif
/* MGP Constellation configuration */
typedef uint32 mgp_GnssConstelConfigType;

/*************************************************************************/
/* MGP Operation mode type                                               */
/*************************************************************************/
typedef enum
{
  MGP_OPERATION_MIN = 0,
  MGP_OPERATION_MSBASED,
  MGP_OPERATION_MSASSISTED,
  MGP_OPERATION_1X_MSASSISTED,
  MGP_OPERATION_FTM,
  MGP_OPERATION_STANDALONE_RF_VERIF,
  MGP_OPERATION_STANDALONE,
  MGP_OPERATION_SBAS_TESTMODE, /* Only SBAS running in this mode */
  MGP_OPERATION_ODP,           /* On demand position mode */
  MGP_OPERATION_LPPM,          /* LPPM operation mode */
  MGP_OPERATION_MAX

} mgp_ConfigOperationEnumTypeVal;
typedef uint8 mgp_ConfigOperationEnumType;

/* Receiver comamnd types */
typedef enum
{
  MGP_RCVR_COMMAND_OFF,           /* Turn Receiver State to OFF */
  MGP_RCVR_COMMAND_IDLE,          /* Turn Receiver State to IDLE */
  MGP_RCVR_COMMAND_ON,            /* Turn Receiver State to ON */
  MGP_RCVR_COMMAND_FORCED_OFF,    /* Turn Receiver State to FORCED OFF */
  MGP_RCVR_COMMAND_FORCED_IDLE,   /* Turn Receiver State to FORCED IDLE */
  MGP_RCVR_COMMAND_IDLE_TRANSITIONING,   /* Prepare MGP State to TRANSITION */
  /*DO NOT CHANGE THE ORDER OF THE ENUM.
       ADD NEW ELEMENTS BELOW */

  MGP_RCVR_COMMAND_MAX = MGP_RCVR_COMMAND_IDLE_TRANSITIONING
} mgp_RcvrCommandEnumTypeVal;
typedef uint8 mgp_RcvrCommandEnumType;

typedef enum
{
  MGP_NET_PLANE_UP = 0,
  MGP_NET_PLANE_CP
} mgp_ConfigNetPlaneEnumTypeVal;
typedef uint8 mgp_ConfigNetPlaneEnumType;

typedef enum
{
  MGP_LO_MEM = 0,
  MGP_HI_MEM
} mgp_MemoryConfigEnumTypeVal;
typedef uint8 mgp_MemoryConfigEnumType;

typedef enum
{
  MGP_ASSIST_DATA_REQ_MSB_QUERY = 0,
  MGP_ASSIST_DATA_REQ_NON_MSB_QUERY
} mgp_AssistDataRequestEnumTypeVal;
typedef uint8 mgp_AssistDataRequestEnumType;


typedef enum
{
  C_MGP_VER_STR_FULL,
  C_MGP_VER_STR_HOST,
  C_MGP_VER_STR_ME,
  C_MGP_VER_STR_COUNT
} mgp_VerStringsIDEnumVal;
typedef uint8 mgp_VerStringsIDEnum;

/* MGP Constellation configuration */
typedef enum
{
  MGP_CONSTEL_TYPE_GLO = 0,
  MGP_CONSTEL_TYPE_BDS,
  MGP_CONSTEL_TYPE_GAL,
  MGP_CONSTEL_TYPE_QZSS,
  MGP_CONSTEL_TYPE_MAX

} mgp_GnssConstelConfigEnumType;


typedef enum
{
  C_MGP_ADC_IQ_SAT_CMD_NOT_SUPPORTED, /* Saturation percentage diag command not supported */
  C_MGP_ADC_IQ_NOT_RUN, /* ADC IQ test has not run yet */
  C_MGP_ADC_IQ_IN_PROG, /* ADC IQ test is in progress */
  C_MGP_ADC_IQ_NUM_SAMPLE_ZERO, /* Num of samples in ADC IQ test was zero*/
  C_MGP_ADC_IQ_SAT_PER_VALID /* ADC IQ saturation percentage is valid */
} mgp_AdcIqSatPerStatusEnum;


/* Enumeration for TimeEstPut Source */
typedef enum
{
  C_MGP_TIME_EST_PUT_INVALID,
  C_MGP_TIME_EST_PUT_TIME_TRANSFER,
  C_MGP_TIME_EST_PUT_WCDMA_TIMETAG,
  C_MGP_TIME_EST_PUT_GSM_TIMETAG,
  C_MGP_TIME_EST_PUT_WCDMA_SLEEP_TIMETAG,
  C_MGP_TIME_EST_PUT_GSM_SLEEP_TIMETAG,
  C_MGP_TIME_EST_PUT_EXTERNAL,
  C_MGP_TIME_EST_PUT_SLOW_CLOCK,
  C_MGP_TIME_EST_PUT_TIME_ALIGN,
  C_MGP_TIME_EST_PUT_PATTERN_MATCH,
  C_MGP_TIME_EST_PUT_LTE_TIMETAG,
  C_MGP_TIME_EST_PUT_LTE_SLEEP_TIMETAG,    
  C_MGP_TIME_EST_PUT_SCLK_TIME_TRANSFER,
  C_MGP_TIME_EST_PUT_LTE_TIME_TRANSFER_SIB8,
  C_MGP_TIME_EST_PUT_LTE_TIME_TRANSFER_LPP_FTA,
  C_MGP_TIME_EST_PUT_TDSCDMA_TIMETAG,
  C_MGP_TIME_EST_PUT_TDSCDMA_SLEEP_TIMETAG,    
  C_MGP_TIME_EST_PUT_GTS_LTE,
  C_MGP_TIME_EST_PUT_GTS_FEST,
  C_MGP_TIME_EST_PUT_GTS_OTHER,
  C_MGP_TIME_EST_PUT_GPS,
  C_MGP_TIME_EST_PUT_CLK_INIT_FROM_SLOW_CLK,   
  C_MGP_TIME_EST_PUT_GTS_GSM,
  C_MGP_TIME_EST_PUT_GTS_WCDMA,
  C_MGP_TIME_EST_PUT_GTS_TDSCDMA,  
  C_MGP_TIME_EST_PUT_GTS_QTIME_CALIB,
  C_MGP_TIME_EST_PUT_HSFN_SYNC_PMIC,
  C_MGP_TIME_EST_PUT_MAX
} mgp_TimeEstPutSourceIDEnum;

/*************************************************************************/
/* MGP Measurement Report Client type                                    */
/*************************************************************************/
#define C_MEAS_RPT_CLIENT_SM (1U << 0)
#define C_MEAS_RPT_CLIENT_PE (1U << 1)

/*************************************************************************/
/* Data Availability Bit-Mask                                            */
/*************************************************************************/
#define C_DATA_AVAIL_MASK_SA_DATA   (1U << 0)
#define C_DATA_AVAIL_MASK_EPH_DATA  (1U << 1)
#define C_DATA_AVAIL_MASK_ALM_DATA  (1U << 2)
#define C_DATA_AVAIL_MASK_IONO_DATA (1U << 3)
#define C_DATA_AVAIL_MASK_RTI_DATA  (1U << 4)

/*************************************************************************/
/* Almanac-health bit-mask                                               */
/*************************************************************************/

/* Define the fields taken by the health entry used by functions 
   me_SvAlmHealthPut and me_SvEphHealthPut 
   
   The C_HEALTH_NOEXIST field really only applies to the Almanac. 
   The Almanac is assumed to be complete when every SV has an almanac 
   health entry that is not C_HEALTH_UNKNOWN */

#define C_HEALTH_UNKNOWN  (0<<0)  /* No health information */
#define C_HEALTH_BAD      (1<<0)  /* Satellite is sick */
#define C_HEALTH_GOOD     (2<<0)  /* Satellite is known good */
#define C_HEALTH_NOEXIST  (3<<0)  /* Satellite does not exist */

/* Setting bit 2 causes the health to be ignored */

#define C_HEALTH_IGNORE   (1<<2)  /* TRUE causes health to be assumed good */

/* Version 53 - Added 4 bytes to ADC IQ test command for saturation threshold */
#define C_IQ_TEST_CMD_VERSION_V53   53
/* Version 54 - Added 4 bytes to ADC IQ test command for 2H calculation */
#define C_IQ_TEST_CMD_VERSION_V54   54

/*************************************************************************/
/* MGP Configuration structure                                           */
/*************************************************************************/
typedef struct
{
  mgp_ConfigOperationEnumType e_OperationMode;
  uint8                       u_Qos;              /* Performance QOS */
  uint32                      q_SessionTimeoutMs;
  boolean                     q_SADataAvailability; /* Bit mask for SA, EPH, ALM etc */
  uint32                      q_RoundTripDelayMs; /* Round trip delay for data requests */
  uint8                       u_SmMeasRptPeriod; /* Measurement report period in units of GPS_MEAS_TICK */
  boolean                     u_EnablePpmRpt;
  boolean                     u_EnablePosRpt;
  boolean                     u_EnableUtcRpt;
  boolean                     u_DataDemodFlag; /* Perform data demodulation: 1 = TRUE, 0 = FALSE */
  mgp_ConfigNetPlaneEnumType  e_NetPlane;
  boolean                     u_multi_report_msa; /* indicating multi-reporting in MSA */
  uint32                      q_QosAccuracyThreshold; /* Accuracy threshold in meters */
  uint32                      q_tbf; /*Time between fixes*/
  boolean                     u_OdpSession; /* true if the session is an ODP session */
  uint32                      q_Tunc; /* current tunc for ODP session */
  uint32                      q_Punc; /* current punc for ODP session */

} mgp_ConfigStructType;

/* MGP LPPM subconfig types */
typedef enum
{
  MGP_LPPM_SUBCONFIG_MIN = 0,
  MGP_LPPM_SUBCONFIG_FIXED_TIME = MGP_LPPM_SUBCONFIG_MIN,  /* fixed-time LPPM subconfig */
  MGP_LPPM_SUBCONFIG_FIXED_POWER,                          /* fixed-power LPPM subconfig */

  MGP_LPPM_SUBCONFIG_MAX = MGP_LPPM_SUBCONFIG_FIXED_POWER,
  MGP_LPPM_SUBCONFIG_FORCE_32BIT = 2147483647 /* Force 32bit */
} mgp_LppmSubConfigEnumType;

/* LPPM fixed-power subconfig parameter types */
typedef enum
{
  MGP_LPPM_FIXED_POWER_LEVEL_MIN = 0,
  MGP_LPPM_FIXED_POWER_LEVEL_LOW = MGP_LPPM_FIXED_POWER_LEVEL_MIN,
  MGP_LPPM_FIXED_POWER_LEVEL_MED,
  MGP_LPPM_FIXED_POWER_LEVEL_HIGH,

  MGP_LPPM_FIXED_POWER_LEVEL_MAX = MGP_LPPM_FIXED_POWER_LEVEL_HIGH,
  MGP_LPPM_FIXED_POWER_LEVEL_FORCE_32BIT = 2147483647 /* Force 32bit */
} mgp_PowerLevelEnumType;

/* MGP LPPM configuration structure */
typedef struct
{
  uint32    e_LppmSubMode;
  uint32    e_PowerLevel;
  uint8     u_TbmSec;
} mgp_LppmConfigStructType;

typedef struct {
  uint32                      q_Timeout;
  /* Timeout before which measurements should be ready */
  uint8                       q_Qos;
  /* measurement QoS for searching.Usage TBD */
  boolean                     b_FTcalFlag;
  /* Flag if this is the Fast TCal session */
  uint32                      q_RequestingTask;
  /* bit mask defining the module requesting CDMA On */
} mgp_CdmaSearcherOnCmdType;

typedef struct
{
  uint16  w_Index;
  uint16  w_Snr;
} IqPeak;

/* Diag Command Msg */
typedef struct
{
  U8 u_DiagData[MGP_MAX_DIAG_CMD_SIZE];
  U8 u_Size;
} mgp_DiagCmdMsgType;

enum
{
  DUAL_POINT_SM_COLLECT,
  SINGLE_POINT_SM_CHAN0,
  SINGLE_POINT_SM_CHAN1,
  ANY_OTHER_POINT_SM
};

typedef enum
{
  IQ_COLLECT_CLIENT_APP,
  IQ_COLLECT_CLIENT_FTM,
  IQ_COLLECT_CLIENT_TCXOMGR,
  IQ_COLLECT_CLIENT_MAX
}mgp_IqCollectClientsVal;
typedef uint8 mgp_IqCollectClients;



#define C_IQ_NB_AUTO_CENTER 0x80000000

#define IQTEST_FLAG_ENABLE_IQ_LOGGING            0x0001
#define IQTEST_FLAG_ENABLE_FFT_LOGGING           0x0002
#define IQTEST_FLAG_ENABLE_HANNING               0x0004
#define IQTEST_FLAG_DISABLE_DROPPED_LOG_RECOVERY 0x0008
#define IQTEST_FLAG_ENABLE_ON_BOARD_FFT          0x0010
#define IQTEST_FLAG_RUN_WBIQ_FOR_ALL_GNSS        0x0020
#define IQTEST_FLAG_EXIT_SARF_MODE               0x0040
#define IQTEST_FLAG_ENABLE_WIDEBAND_JAMMER_EST   0x0080

/* Any change in below enum will require changes in iq_driver tool in 
 * gnss_tools/main/latest/standalone_rfv/iq_test/iq_driver/iq_driver_main.cpp
 * and iq packet parser tool in 
 * gnss_tools/main/latest/standalone_rfv/iq_test/iq_pkt_parser/IqPacketParser.c
 * and iq cmd perl script in 
 * gnss_tools/main/latest/standalone_rfv/iq_test/gnss_iq_collect.pl
 */

typedef enum
{
  COLLECT_MODE_RESERVED_0,
  COLLECT_MODE_WBIQ_GPS_L1,
  COLLECT_MODE_WBIQ_GPS_L2,
  COLLECT_MODE_WBIQ_GLO_G1,
  COLLECT_MODE_WBIQ_GPS_L5_GAL_E5A,
  COLLECT_MODE_NBIQ_GPS_L1_AUTO,
  COLLECT_MODE_WBIQ_GPS_L5, /* For internal use only */
  COLLECT_MODE_RESERVED_7,
  COLLECT_MODE_NBIQ_GPS_L1_MANUAL,
  COLLECT_MODE_RESERVED_9,
  COLLECT_MODE_RESERVED_10,
  COLLECT_MODE_RESERVED_11,
  COLLECT_MODE_RESERVED_12,
  COLLECT_MODE_RESERVED_13,
  COLLECT_MODE_RESERVED_14,
  COLLECT_MODE_FBIQ_GLO_G1,
  COLLECT_MODE_L1_ADC_IQ,
  COLLECT_MODE_WBIQ_BDS_B1,
  COLLECT_MODE_WBIQ_GAL_E1,
  COLLECT_MODE_WBIQ_GLO_G2,
  COLLECT_MODE_WBIQ_BDS_B2,
  COLLECT_MODE_FBIQ_GPS_L1_GAL_E1,
  COLLECT_MODE_FBIQ_BDS_B1,
  COLLECT_MODE_FBIQ_GLO_G2,
  COLLECT_MODE_FBIQ_GPS_L2,
  COLLECT_MODE_FBIQ_BDS_B2,
  COLLECT_MODE_L5_ADC_IQ,
  COLLECT_MODE_NBIQ_GPS_L5_AUTO,
  COLLECT_MODE_NBIQ_GPS_L5_MANUAL,
  COLLECT_MODE_MAX
} e_CollectModeTypeVal;
typedef uint8 e_CollectModeType;

typedef struct
{
  mgp_IqCollectClients  e_ClientType; /* Client that has requested IQ collect */
  CplxS32 *p_Buffer;                  /* FTM client will provide memory for IQ samples */
  uint8  u_Version;
  uint8  u_TestControl;
  uint16 w_Flags;
  uint16 w_RetransmitPktNum;
  uint8  u_RfStatusLoggingRateHz;
  uint8  u_CollectMode;
  int8   b_GloR1Chan;
  uint8  u_GloHWChan;
  uint16 w_SampleCnt1k;     /* Number of IQ pairs (units of 1024 samples) */
  uint8  u_Integrations;    /* Number of integrations */
  int32  l_CenterFreqNBHz;  /* Narrowband FFT Center Frequency */
  cgps_SARF_Cb *p_FtmCb;
  cgps_XOCAL_Cb *p_XoCalCb;
  cgps_XOCAL_results_Cb *p_XoCalResultsCb;
  int32  l_Power;
  int32  l_FreqOffset;
  uint32  q_ADCIqSatThresh;
  uint32 q_AdcIQWidebandJammerStartFreqHz;
  uint32 q_AdcIQWidebandJammerStopFreqHz;
  int32  l_AdcIQRefTonePwrDBm;
} mgp_IqTestCmdStructType;

/* For above options 0, 1, 2, below collect points are possible */
enum
{
  COLLECT_PT_NONE,
  COLLECT_PT_MISC1_OUTPUT_BITS,
  COLLECT_PT_BC_OUTPUT,
  COLLECT_PT_TS_I_NONQUANTIZED,
  COLLECT_PT_BC_I_FILTERED,
  COLLECT_PT_BC_I_NONQUANTIZED,
  COLLECT_PT_SIGNAL_LOG,
  COLLECT_PT_MISC2_OUTPUT_BITS
};

/* When SM_POINT equals ANY_OTHER_POINT_SM, below collect points are possible */
enum
{
  COLLECT_POINT_SAMPLE_SERVER_2MHZ,
  COLLECT_POINT_SEARCHER_1KHZ,
  COLLECT_POINT_DC_OFFSET_ESTIMATOR,
  COLLECT_POINT_AFLT,
  COLLECT_POINT_AFLT_AND_BC
};

typedef struct
{
  uint16 w_TsInfo;          /* 1/0 = TS On/Off */
  uint8  u_TsSignalPower;   /* Not used */
  uint32 q_TsDoppler;       /* Units: 16.368e6 / 2**24 Hz/Bit */
  uint8  u_SmPoint;         /* Detemines where the samples are collected from */
  uint8  u_CollectPoint;    /* Further control on collection point. 3 is normal */
  uint8  u_Scale;           /* Not used */
} Iq_HwConfig;


enum
{
  PRESC_DWELL_NO_RXD,
  PRESC_DWELL_PRI_RXD_LINKED,
  PRESC_DWELL_RXD_ONLY,
  PRESC_DWELL_RXD_MAX
};

typedef struct
{
  uint8   u_CmdControlFlag;
  uint32  q_SeqNum;                  /* Seq number from cmd */

  uint8   u_GpsRxDFlag;              /* 0: No RxD
                                     ** 1: Primary + RxD linked
                                     ** 2: RxD ONLY
                                     ** 4-255: Reserved */

  uint8   u_GpsControlFlag;          /* BIT0:-
                                     ** 0 = As Prescribed. Applicable to 
                                     ** TRK dwells ONLY.No trunctation of code 
                                     ** phase center
                                     ** 1 = Automatic. Automatic code phase
                                     ** and doppler centering. Code phase and 
                                     ** doppler parameters from the DM command 
                                     ** are ignored. The GNSS software propagates
                                     ** the values from prior measurements stored
                                     ** in software.
                                     ** 
                                     ** BIT1: Applicable to TRK dwells ONLY.
                                     ** 0 = No Truncation of code phase center 
                                     ** 1 = Truncation of code phase center
                                     ** so that the peak is randomly sampled 
                                     ** within the chip.
                                     ** 
                                     ** BIT2: Applicable to TRK dwells ONLY.
                                     ** 0 = No randomization of Doppler
                                     ** 1 = Randomization of Doppler.
                                     ** 
                                     ** BIT3-BIT7:
                                     ** Reserved */

  uint8   u_GpsSv;                   /* GPS SV id or SBAS Satellite id */
  int32   l_GpsDoppMps;              /* Doppler in m/s */
  int32   l_GpsDoppWinHz;            /* Doppler window in Hertz */
  uint32  q_GpsCpCenterCx40;         /* Code phase center in chipx40 */
  uint32  q_GpsCodeWinSizeChipx1;    /* Code window size in chipx1 */
  uint8   u_GpsSrchMode;             /* Search mode */
  uint16  w_GpsNumPostD;             /* Number of PostD */
  uint8   u_GpsPfa;                  /* PFA number */
  uint32  q_GpsReserved;

  uint8   u_GloControlFlag;          /* BIT0:-
                                     ** 0 = As Prescribed. Applicable to 
                                     ** TRK dwells ONLY.No trunctation of code 
                                     ** phase center
                                     ** 1 = Automatic. Automatic code phase
                                     ** and doppler centering. Code phase and 
                                     ** doppler parameters from the DM command 
                                     ** are ignored. The GNSS software propagates
                                     ** the values from prior measurements stored
                                     ** in software.
                                     ** 
                                     ** BIT1: Applicable to TRK dwells ONLY.
                                     ** 0 = No Truncation of code phase center 
                                     ** 1 = Truncation of code phase center
                                     ** so that the peak is randomly sampled 
                                     ** within the chip.
                                     ** 
                                     ** BIT2: Applicable to TRK dwells ONLY.
                                     ** 0 = No randomization of Doppler
                                     ** 1 = Randomization of Doppler.
                                     ** 
                                     ** BIT3-BIT7:
                                     ** Reserved */

  uint8   u_GloSv;                   /* Glo SV id [1..14] */
  int32   l_GloDoppMps;              /* Doppler in m/s */
  int32   l_GloDoppWinHz;            /* Doppler window in Hertz */
  uint32  q_GloCpCenterCx40;         /* Code phase center in chipx40 */
  uint32  q_GloCodeWinSizeChipx1;    /* Code window size in chipx1 */
  uint8   u_GloSrchMode;             /* Search mode */
  uint16  w_GloNumPostD;             /* Number of PostD */
  uint8   u_GloHwChan;               /* Glo HW chan */
  uint8   u_GloPfa;                  /* PFA number */
  uint32  q_GloReserved;

  uint16  w_DwellCnt;
  uint16  w_TotalNumDwells;
  uint16  w_PassDwellCnt;
  uint32  q_Reserved;

  uint8   u_BdsControlFlag;          /* BIT0:-
                                     ** 0 = As Prescribed. Applicable to 
                                     ** TRK dwells ONLY.No trunctation of code 
                                     ** phase center
                                     ** 1 = Automatic. Automatic code phase
                                     ** and doppler centering. Code phase and 
                                     ** doppler parameters from the DM command 
                                     ** are ignored. The GNSS software propagates
                                     ** the values from prior measurements stored
                                     ** in software.
                                     ** 
                                     ** BIT1: Applicable to TRK dwells ONLY.
                                     ** 0 = No Truncation of code phase center 
                                     ** 1 = Truncation of code phase center
                                     ** so that the peak is randomly sampled 
                                     ** within the chip.
                                     ** 
                                     ** BIT2: Applicable to TRK dwells ONLY.
                                     ** 0 = No randomization of Doppler
                                     ** 1 = Randomization of Doppler.
                                     ** 
                                     ** BIT3-BIT7:
                                     ** Reserved */

  uint8   u_BdsSv;                   /* GPS SV id or Glo Satellite id */
  int32   l_BdsDoppMps;               /* Doppler in m/s */
  int32   l_BdsDoppWinHz;            /* Doppler window in Hertz */
  uint32  q_BdsCpCenterCx40;         /* Code phase center in chipx40 */
  uint32  q_BdsCodeWinSizeChipx1;    /* Code window size in chipx1 */
  uint8   u_BdsSrchMode;             /* Search mode */
  uint16  w_BdsNumPostD;             /* Number of PostD */
  uint8   u_BdsPfa;                  /* PFA number */
  uint32  q_BdsReserved;             /* Not really needed, but maintain it similar to GPS/GLO */

  uint8   u_MaxSvDwell; /* Enable max Sv dwell
                                             ** 0 for disable
                                             ** 1 for enable
                                             ** Bit0: Enable for GPS
                                             ** Bit1: Enable for GLO
                                             ** Bit2: Enable for BDS
                                             ** Bit3: Enable for GAL */

  uint8   u_GalControlFlag;          /* BIT0:- 
                                     ** 0 = As Prescribed. Applicable to  
                                     ** TRK dwells ONLY.No trunctation of code  
                                     ** phase center 
                                     ** 1 = Automatic. Automatic code phase 
                                     ** and doppler centering. Code phase and  
                                     ** doppler parameters from the DM command  
                                     ** are ignored. The GNSS software propagates 
                                     ** the values from prior measurements stored 
                                     ** in software. 
                                     **  
                                     ** BIT1: Applicable to TRK dwells ONLY. 
                                     ** 0 = No Truncation of code phase center  
                                     ** 1 = Truncation of code phase center 
                                     ** so that the peak is randomly sampled  
                                     ** within the chip. 
                                     **  
                                     ** BIT2: Applicable to TRK dwells ONLY. 
                                     ** 0 = No randomization of Doppler 
                                     ** 1 = Randomization of Doppler. 
                                     **  
                                     ** BIT3-BIT7: 
                                     ** Reserved */

  uint8   u_GalSv;                   /* GAL SV id */
  int32   l_GalDoppMps;              /* Doppler in m/s */
  int32   l_GalDoppWinHz;            /* Doppler window in Hertz */
  uint32  q_GalCpCenterCx40;         /* Code phase center in chipx40 */
  uint32  q_GalCodeWinSizeChipx1;    /* Code window size in chipx1 */
  uint8   u_GalSrchMode;             /* Search mode */
  uint16  w_GalNumPostD;             /* Number of PostD */
  uint8   u_GalPfa;                  /* PFA number */
  uint32  q_GalReserved;             /* Not really needed, but maintain it similar to GPS/GLO */

} mgp_PrescDwellCmd;

typedef struct
{
  uint8   u_Version;         /* Version Number */
  uint8   u_CmdControlFlag;  /* SpAn control mode
                                  0 – Single Shot mode, 1 – Continuous mode. */
  uint8   u_ScanStartStop;   /* Start or Stop Spectrum Analyzer scan.
                               1 - Start Scan; 0 - Stop scan; 
                               2 – Stop and restart scan. */

  uint8   u_GpsJobsEnabled;  /* Bit mask for the GPS Scan types enabled. 
                                Set Bit 0 to 1 to enable Fast scan;
                                Set Bit 1 to 1 to enable Deep scan */
  uint8   u_GloJobsEnabled;  /* Bit mask for the GLO Scan types enabled. 
                                Set Bit 0 to 1 to enable Fast scan;
                                Set Bit 1 to 1 to enable Deep scan */

  uint8   u_ProbePoint;      /* Bit mask for the Spectrum Analyzer probe point.
                               Set Bit 0:3 to 1 for Probe point P1-P4 */
  uint8   u_TrkMode;         /* Enable/disable tracking mode for jammers. 
                               1 - Enable; 0 – Disable; */
  uint8   u_NotchFilterReset; /* 1 - Reset notch filters
                                 0 - Do not reset notch filters */
  uint8   u_BdsJobsEnabled;  /* Bit mask for the BDS Scan types enabled. 
                                Set Bit 0 to 1 to enable Fast scan;
                                Set Bit 7 to 1 to enable Shallow scan */
  uint8   u_GalJobsEnabled;  /* Bit mask for the GAL scan types enabled. 
                                Set Bit 0 to 1 to enable Fast scan;
                                Set Bit 7 to 1 to enable Shallow scan */
} mgp_SpectrumAnalyzerCmd;

typedef enum
{
  TSG_CH1,
  TSG_CH2,
  TSG_NUM_CHANS
}nav_TsgChanNumberType;

enum
{
  TSG_CH_MODE_OFF,
  TSG_CH_MODE_CW,
  TSG_CH_MODE_SV
};

/**
*   Bit field values in
*   TSG_CMD0 regsiter
*/


typedef enum
{
  NAV_TSG_TESTSEL_OFF = 0,
  NAV_TSG_TESTSEL_P1,
  NAV_TSG_TESTSEL_P2,
  NAV_TSG_TESTSEL_P3,
  NAV_TSG_TESTSEL_P4,
  NAV_TSG_TESTSEL_P5,
  NAV_TSG_TESTSEL_P6,
  NAV_TSG_TESTSEL_P7
}nav_TsgTestSelEnumTypeVal;
typedef uint32 nav_TsgTestSelEnumType;

typedef enum
{
  NAV_TSG_INIT_NO = 0,
  NAV_TSG_INIT_YES
}nav_TsgInitEnumTypeVal;
typedef uint32 nav_TsgInitEnumType;

/**
*   Bit field values in TSG_CH1_CONFIG
*   regsiter
*/

typedef enum
{
  NAV_TSG_CH_SNR_0 = 0,
  NAV_TSG_CH_SNR_1,
  NAV_TSG_CH_SNR_2,
  NAV_TSG_CH_SNR_3,
  NAV_TSG_CH_SNR_4,
  NAV_TSG_CH_SNR_5,
  NAV_TSG_CH_SNR_6,
  NAV_TSG_CH_SNR_7
}nav_TsgChSnrEnumTypeVal;
typedef uint32 nav_TsgChSnrEnumType;

typedef enum
{
  NAV_TSG_CH_MODLEV_0 = 0,
  NAV_TSG_CH_MODLEV_1,
  NAV_TSG_CH_MODLEV_2,
  NAV_TSG_CH_MODLEV_3,
  NAV_TSG_CH_MODLEV_4,
  NAV_TSG_CH_MODLEV_5,
  NAV_TSG_CH_MODLEV_6,
  NAV_TSG_CH_MODLEV_7
}nav_TsgChModLevEnumTypeVal;
typedef uint32 nav_TsgChModLevEnumType;

typedef enum
{
  NAV_TSG_CH_DATATYPE_PROG1 = 0,
  NAV_TSG_CH_DATATYPE_PROG20,
  NAV_TSG_CH_DATATYPE_TOGGLE,
  NAV_TSG_CH_DATATYPE_RANDOM
}nav_TsgChDataEnumTypeVal;
typedef uint32 nav_TsgChDataEnumType;

typedef enum
{
  NAV_TSG_CH_CONSTCODE_NO = 0,
  NAV_TSG_CH_CONSTCODE_ONE,
  NAV_TSG_CH_CONSTCODE_ZERO,
  NAV_TSG_CH_CONSTCODE_NO2
}nav_TsgChConstCodeEnumTypeVal;
typedef uint32 nav_TsgChConstCodeEnumType;

typedef enum
{
  NAV_TSG_CH_RATE_OFF = 0,
  NAV_TSG_CH_RATE_FULL,
  NAV_TSG_CH_RATE_HALF,
  NAV_TSG_CH_RATE_QTR
}nav_TsgChRateEnumTypeVal;
typedef uint32 nav_TsgChRateEnumType;


typedef enum
{
  NAV_TSG_CH_MODE_OFF = 0,
  NAV_TSG_CH_MODE_L1,
  NAV_TSG_CH_MODE_R1,
  NAV_TSG_CH_MODE_L1C,
  NAV_TSG_CH_MODE_L2C,
  NAV_TSG_CH_MODE_L5,
  NAV_TSG_CH_MODE_E1,
  NAV_TSG_CH_MODE_B1
}nav_TsgChModeEnumTypeVal;
typedef uint32 nav_TsgChModeEnumType;

typedef enum
{
  NAV_TSG_CH_NO_OVERLAY_NO = 0,
  NAV_TSG_CH_NO_OVERLAY_YES
}nav_TsgChNoOverlayEnumTypeVal;
typedef uint32 nav_TsgChNoOverlayEnumType;

typedef enum
{
  NAV_TSG_CH_RS_EN_NO = 0,
  NAV_TSG_CH_RS_EN_YES
}nav_TsgChRsEnEnumTypeVal;
typedef uint32 nav_TsgChRsEnEnumType;
/**
*   Bit field values in TSG_RX1_CONFIG
*   regsiter
*/
typedef enum
{
  NAV_TSG_RX_SDMQUANT_OFF = 0,
  NAV_TSG_RX_SDMQUANT_Q2,
  NAV_TSG_RX_SDMQUANT_Q3,
  NAV_TSG_RX_SDMQUANT_Q4
}nav_TsgRxSdmQuantEnumTypeVal;
typedef uint32 nav_TsgRxSdmQuantEnumType;

typedef enum
{
  NAV_TSG_RX_DACREF_REF_100_MV = 0,
  NAV_TSG_RX_DACREF_REF_200_MV,
  NAV_TSG_RX_DACREF_REF_400_MV,
  NAV_TSG_RX_DACREF_REF_800_MV
}nav_TsgRxDacRefType;
typedef uint32 nav_TsgRxDacRefEnumType;

typedef enum
{
  NAV_TSG_RX_QUANTSEL_LINEAR = 0,
  NAV_TSG_RX_QUANTSEL_SDM
}nav_TsgRxQuantSelEnumTypeVal;
typedef uint32 nav_TsgRxQuantSelEnumType;

typedef enum
{
  NAV_TSG_RX_SDMAMP_0 = 0,
  NAV_TSG_RX_SDMAMP_1,
  NAV_TSG_RX_SDMAMP_2,
  NAV_TSG_RX_SDMAMP_3,
  NAV_TSG_RX_SDMAMP_4,
  NAV_TSG_RX_SDMAMP_5,
  NAV_TSG_RX_SDMAMP_6,
  NAV_TSG_RX_SDMAMP_7
}nav_TsgRxSdmAmpEnumTypeVal;
typedef uint32 nav_TsgRxSdmAmpEnumType; 

typedef enum
{
  NAV_TSG_RX_LPFSW_OFF = 0,
  NAV_TSG_RX_LPFSW_ON
}nav_TsgRxLpfSwEnumTypeVal;
typedef uint32 nav_TsgRxLpfSwEnumType;

typedef enum
{
  NAV_TSG_RX_FINEAMP_ATTEN = 0,
  NAV_TSG_RX_FINEAMP_PASS
}nav_TsgRxFineAmpEnumTypeVal;
typedef uint32 nav_TsgRxFineAmpEnumType;

typedef enum
{
  NAV_TSG_RX_SIGNALSW2_NO = 0,
  NAV_TSG_RX_SIGNALSW2_YES
}nav_TsgRxSignalSw2EnumTypeVal;
typedef uint32 nav_TsgRxSignalSw2EnumType;

typedef enum
{
  NAV_TSG_RX_SIGNALSW1_NO = 0,
  NAV_TSG_RX_SIGNALSW1_YES
}nav_TsgRxSignalSw1EnumTypeVal;
typedef uint32 nav_TsgRxSignalSw1EnumType;

typedef enum
{
  NAV_TSG_RX_NOISESW_NO = 0,
  NAV_TSG_RX_NOISESW_YES
}nav_TsgRxNoiseSwEnumTypeVal;
typedef uint32 nav_TsgRxNoiseSwEnumType;

typedef enum
{
  NAV_TSG_RX_RATE_CH1 = 0,
  NAV_TSG_RX_RATE_CH2
}nav_TsgRxRateEnumTypeVal;
typedef uint32 nav_TsgRxRateEnumType;


enum
{
 TSG_DATABITS_OFF,
 TSG_DATABITS_RANDOM,
 TSG_DATABITS_TOGGLE
};

typedef struct
{
  uint8   u_Mode;
  uint8   u_GnssType;
  int8    b_SvId;
  int32  l_DoppHz;
  int8    b_CodeDoppQuartCps;
  uint16  w_CodePhaseChips;
  uint8   u_DataBitsConfig;
  uint8  u_SNR; 
} mgp_TsgChanConfig;

typedef struct
{
  mgp_TsgChanConfig z_TsgChanConfig[TSG_NUM_CHANS];
} mgp_TsgConfig;

typedef struct
{
  uint8 u_RfMode;
  uint8 u_RfLinearity;
  uint8 u_RfRxdOnTimeSec;
  uint8 u_RfRxdOffTimeSec;
  uint8 u_AdcConfig;
} mgp_DiagRfCmd;

typedef struct
{
  uint8 u_RfMode;
} mgp_DiagPrxRfCmd;

typedef struct
{
  uint8 u_RfLinearity;
} mgp_DiagRfLinearityCmd;

typedef struct
{
  uint8 u_TestStopFlag;
  uint8 u_TestType;
  uint8 u_TaskCnt;
  uint16 w_IntegLength;
} mgp_MemBwTestParams;

/* Cell Position struct */
typedef cgps_PosInfoSrcType mgp_PosInfoSrcType;
typedef cgps_CellDBPosInfoStruct mgp_CellDBPosInfoStruct;


/* Structure for the Cell DB reference Position */
/* If this is updated, cgps_CellDBPackedPosInfoStruct also needs to be updated */
typedef PACKED struct PACKED_POST
{
  cgps_PosInfoSrcType e_PosInfoSrc; /* Source of the Position Estimate */
  boolean u_PosInfoValid; /* Validity indicator for the Position Information */
  boolean u_GpsTimeValid; /* GPS Time validity indicator for the Position */
  uint16  w_GpsWeek;      /* GPS Week time reference for the Position */
  uint32  q_GpsMsec;      /* GPS Msec time reference for the Position */
  /* Lalitude, Longitude in Radians and Altitude in meters */
  int64   d_PosLLa[3];
  int32   f_PosUnc;       /* Horizontal Uncertainty in meters (circular) */
  int32   f_AltUnc;       /* Altitude Uncertainty in meters */

} mgp_CellDBPosInfoStruct_ForDmLog;

/* Position Estimate Inject Structure 
   This structure contains estimates for both Regular and Optimistic Positions 
*/

typedef struct
{
  boolean u_OptPosValid;            /* Validity field for Optimistic Position */
  mgp_CellDBPosInfoStruct z_CellDBPos;
  mgp_CellDBPosInfoStruct z_OptimisticPos;
  U64     t_TimeTickMsec;      /* TimeTickMsec of current state (0 means unknown) */
} mgp_PosEstInfoStruct;

/* Used by PP to request CD to enter a large grid log at a lower priority
   and for CD to reply */
typedef struct
{
  uint8  u_SegSerialNum;
  uint16 w_LogSerialNum;
  uint16 w_LogTypeCode;
  uint16 w_SizePart1;
  uint16 w_SizePart2;
  uint8 *u_DataPart1;
  uint8 *u_DataPart2;
} pe_PpLogProxyType;


#if defined(WIN32) && defined(ADS_120_COMPILER_4_BYTE_ALIGNMENT)
#pragma pack(pop, MGP_PE_COMMON_HH)
#endif

/* RF and HW status for Primary Rx */
#define GNSS_NAV_HW_VER_MAX_STR_LEN 15 /**< Hardware version string max length */
#define GNSS_CONFIG_MAX_STR_LEN 20 /**< Receiver configuration string max length */
#define GNSS_RF_MODE_MAX_STR_LEN 10 /**< RF device operation mode string max lenth */
#define GNSS_RF_BP_MODE_MAX_STR_LEN 15 /**< BP operation mode string max lenth */

#define GNSS_RF_STATUS_LOG_VER 5 /**< 0x147E RF Status log version */

/**
 * BP3 Subchannel structure
 */
typedef PACKED struct PACKED_POST
{
  char    u_Bp3SubChXMode[GNSS_RF_BP_MODE_MAX_STR_LEN]; /**< BP3 SubchannelX Mode */
  uint32  q_Bp3SubChXAmpI; /**< BP3 SubchannelX amplitude I */
  uint32  q_Bp3SubChXAmpQ; /**< BP3 SubchannelX amplitude Q */
  int32   l_Bp3SubChXPwrDb; /**< BP3 SubchannelX power in units of dB */
} mgp_Bp3RfStatusType;

/** 
 * 0x147E RF Status log packet version 5
*/
typedef PACKED struct PACKED_POST
{
  log_hdr_type z_hdr; /**< Standard DM header */

  uint8   u_Version; /**< 0x147E version number */
  char    u_GnssNavHwVer[GNSS_NAV_HW_VER_MAX_STR_LEN]; /**< Nav Core version string */
  char    u_GnssRfBPConfig[GNSS_CONFIG_MAX_STR_LEN];  /**< RF/BP configuration string */
  uint32  q_RefFCount;  /**< FCount value at log generation time */

  /* RF Status */
  char    u_RfIcId[GNSS_RF_STATUS_MAX_STR_LEN]; /**< RF device ID string */
  char    u_Rx1RfMode[GNSS_RF_MODE_MAX_STR_LEN]; /**< RX1 mode string*/
  int32   l_Rx1PgaGain; /**< RX1 PGA setting in dB */
  char    u_Rx2RfMode[GNSS_RF_MODE_MAX_STR_LEN]; /**< RX2 mode string*/
  int32   l_Rx2PgaGain; /**< RX2 PGA setting in dB */

  /* AD processor status */
  uint32  q_AdRx1SampleRateHz; /**< RX1 ADC sample rate in Hz */
  int32   l_AdRx1MeanIp1mV; /**< RX1 ADC mean I in units of 0.1 mV */
  int32   l_AdRx1MeanQp1mV; /**< RX1 ADC mean Q in units of 0.1 mV */
  uint32  l_AdRx1AmpIp1mV; /**< RX1 ADC amplitude I in units of 0.1 mV */
  uint32  l_AdRx1AmpQp1mV; /**< RX1 ADC amplitude I in units of 0.1 mV */
  uint32  q_AdRx1BlankPercentage; /**< RX1 ADC blank percentage */
  uint32  q_AdRx2SampleRateHz; /**< RX2 ADC sample rate in Hz */
  int32   l_AdRx2MeanIp1mV; /**< RX2 ADC mean I in units of 0.1 mV */
  int32   l_AdRx2MeanQp1mV; /**< RX2 ADC mean Q in units of 0.1 mV */
  uint32  l_AdRx2AmpIp1mV; /**< RX2 ADC amplitude I in units of 0.1 mV */
  uint32  l_AdRx2AmpQp1mV; /**< RX2 ADC amplitude I in units of 0.1 mV */
  uint32  q_AdRx2BlankPercentage; /**< RX2 ADC blank percentage */
  uint32  q_NumMeasurements; /** < Number of measurements used for this log packet */

  /* Baseband Processor 1 (BP1) status */
  uint8   u_Bp1GainStep; /**< BP1 gain step in hardware units */
  uint32  q_Bp1PreAmpI; /**< BP1 pre-amplitude I */
  uint32  q_Bp1PreAmpQ; /**< BP1 pre-amplitude Q */
  char    u_Bp1SubCh2Mode[GNSS_RF_BP_MODE_MAX_STR_LEN]; /**< BP1 Subchannel2 Mode */
  uint32  q_Bp1SubCh2AmpI; /**< BP1 Subchannel2 amplitude I */
  uint32  q_Bp1SubCh2AmpQ; /**< BP1 Subchannel2 amplitude Q */
  int32   l_Bp1SubCh2PwrDb; /**< BP1 Subchannel2 power in units of dB */
  char    u_Bp1SubCh3Mode[GNSS_RF_BP_MODE_MAX_STR_LEN]; /**< BP1 Subchannel3 Mode */
  uint32  q_Bp1SubCh3AmpI; /**< BP1 Subchannel3 amplitude I */
  uint32  q_Bp1SubCh3AmpQ; /**< BP1 Subchannel3 amplitude Q */
  int32   l_Bp1SubCh3PwrDb; /**< BP1 Subchannel3 power in units of dB */
  uint32  q_Bp1Cx20AmpI; /**< BP1 ChipX20 amplitude I */
  uint32  q_Bp1Cx20AmpQ; /**< BP1 ChipX20 amplitude Q */

  /* Baseband Processor 2 (BP2) status */
  uint8   u_Bp2GainStep; /**< BP2 gain step in hardware units */
  uint32  q_Bp2PreAmpI; /**< BP2 pre-amplitude I */
  uint32  q_Bp2PreAmpQ; /**< BP2 pre-amplitude Q */
  char    u_Bp2SubCh2Mode[GNSS_RF_BP_MODE_MAX_STR_LEN]; /**< BP2 Subchannel2 Mode */
  uint32  q_Bp2SubCh2AmpI; /**< BP2 Subchannel2 amplitude I */
  uint32  q_Bp2SubCh2AmpQ; /**< BP2 Subchannel2 amplitude Q */
  int32   l_Bp2SubCh2PwrDb; /**< BP2 Subchannel2 power in units of dB */
  char    u_Bp2SubCh3Mode[GNSS_RF_BP_MODE_MAX_STR_LEN]; /**< BP2 Subchannel3 Mode */
  uint32  q_Bp2SubCh3AmpI; /**< BP2 Subchannel3 amplitude I */
  uint32  q_Bp2SubCh3AmpQ; /**< BP2 Subchannel3 amplitude Q */
  int32   l_Bp2SubCh3PwrDb; /**< BP2 Subchannel3 power in units of dB */
  uint32  q_Bp2Cx20AmpI; /**< BP2 ChipX20 amplitude I */
  uint32  q_Bp2Cx20AmpQ; /**< BP2 ChipX20 amplitude Q */

  /* Baseband Processor 2P (BP2P) status */
  uint8   u_Bp2pGainStep; /**< BP2 gain step in hardware units */
  uint32  q_Bp2pPreAmpI; /**< BP2 pre-amplitude I */
  uint32  q_Bp2pPreAmpQ; /**< BP2 pre-amplitude Q */
  char    u_Bp2pSubCh2Mode[GNSS_RF_BP_MODE_MAX_STR_LEN]; /**< BP2P Subchannel2 Mode */
  uint32  q_Bp2pSubCh2AmpI; /**< BP2P Subchannel2 amplitude I */
  uint32  q_Bp2pSubCh2AmpQ; /**< BP2P Subchannel2 amplitude Q */
  int32   l_Bp2pSubCh2PwrDb; /**< BP2 Subchannel2 power in units of dB */
  char    u_Bp2pSubCh3Mode[GNSS_RF_BP_MODE_MAX_STR_LEN]; /**< BP2 Subchannel3 Mode */
  uint32  q_Bp2pSubCh3AmpI; /**< BP2 Subchannel3 amplitude I */
  uint32  q_Bp2pSubCh3AmpQ; /**< BP2 Subchannel3 amplitude Q */
  int32   l_Bp2pSubCh3PwrDb; /**< BP2 Subchannel3 power in units of dB */

  /* Baseband Processor 3 (BP3) status */
  uint8   u_Bp3GainStep; /**< BP3 gain step in hardware units */
  uint32  q_Bp3PreAmpI; /**< BP3 pre-amplitude I */
  uint32  q_Bp3PreAmpQ; /**< BP3 pre-amplitude Q */
  uint32  q_Bp3Cx20AmpI; /**< BP3 ChipX20 amplitude I */
  uint32  q_Bp3Cx20AmpQ; /**< BP3 ChipX20 amplitude Q */
  uint8   u_Bp3L2GainStep; /**< BP3 L2 gain step in hardware units */
  uint32  q_Bp3L2PreAmpI; /**< BP3 L2 pre-amplitude I */
  uint32  q_Bp3L2PreAmpQ; /**< BP3 L2 pre-amplitude Q */
  uint32  q_Bp3Cx20L2AmpI; /**< BP3 L2 ChipX20 amplitude I */
  uint32  q_Bp3Cx20L2AmpQ; /**< BP3 L2 ChipX20 amplitude Q */
  /* BP3 Subchannel status */
  mgp_Bp3RfStatusType z_Bp3Info[C_MGP_TOTAL_GLO_SUB_CHAN_NUM]; /**< BP3 subchannel info*/

  /* Baseband Processor 4 (BP4) status */
  char    u_Bp4Mode[GNSS_RF_BP_MODE_MAX_STR_LEN]; /**< BP4 Mode */
  uint32  q_Bp4AmpI; /**< BP2P Subchannel2 amplitude I */
  uint32  q_Bp4AmpQ; /**< BP2P Subchannel2 amplitude Q */
  int32   l_Bp4PwrDb; /**< BP2 Subchannel2 power in units of dB */
} mgp_RxRfStatusDmLog;

/* RF Status version 4 log packet kept for backwards compatibility with Gen8 */
typedef PACKED struct PACKED_POST{
  int8    b_Bp3FreqIndex; /* -7 to +6 with -8 representing unused */
  uint32  q_Bp3AmplI; /* BP3 Amp I */
  uint32  q_Bp3AmplQ; /* BP3 Amp Q */
  uint32  f_Bp3RotFineFreq; /* Fine Rotator Value in kHz */
} mgp_Bp3RfStatusTypeV4;

typedef PACKED struct PACKED_POST
{
  /*lint -e754 : local structure member not referenced */
  log_hdr_type z_hdr;        /* Standard DM header */

  uint8   u_Version;
  char    u_GnssNavHwVer[GNSS_NAV_HW_VER_MAX_STR_LEN];
  char    u_GnssActualConfig[GNSS_CONFIG_MAX_STR_LEN];
  uint32  q_RefFCount;  /* FCount value at reference tick */

  /* RF Status */
  char    u_RfIcId[GNSS_RF_STATUS_MAX_STR_LEN];
  char    u_RfOpMode[GNSS_RF_MODE_MAX_STR_LEN];
  uint8   u_RfDcOffsetI[2];
  uint8   u_RfDcOffsetQ[2];
  int32   l_PGAGain;

  /* ADC processor status */
  uint32  q_AdcSampleRateHz;
  int32   l_AdcMeanIp1mV;
  int32   l_AdcMeanQp1mV;
  uint32  q_AdcAmpIp1mV;
  uint32  q_AdcAmpQp1mV;

  /* BP1 LBW processor status */
  uint8   u_Bp1LbwSampleRate;
  uint8   u_Bp1LbwGainStep;
  uint32  q_Bp1LbwPreAmplI;
  uint32  q_Bp1LbwPreAmplQ;
  uint32  q_Bp1LbwAmplI;
  uint32  q_Bp1LbwAmplQ;
  uint32  q_Bp1GalAmplI;
  uint32  q_Bp1GalAmplQ;
  uint32  q_Bp1LbwRotFineFreq;
  uint32  q_Bp1LbwRsRateHWU;
  uint8   u_Bp1LbwNumActNotch;
  uint16  w_Bp1LbwGrpDelay;

  /* BP2 BDS processor status */
  uint8   u_Bp2BdsSampleRate;
  uint8   u_Bp2BdsGainStep;
  uint32  q_Bp2BdsPreAmplI;
  uint32  q_Bp2BdsPreAmplQ;
  uint32  q_Bp2BdsAmplI;
  uint32  q_Bp2BdsAmplQ;
  uint32  q_Bp2BdsRotFineFreq;
  uint32  q_Bp2BdsRsRateHWU;
  uint8   u_Bp2BdsNumActNotch;
  uint16  w_Bp2BdsGrpDelay;

  /* BP3 GLO processor status */
  uint8   u_Bp3GloSampleRate;
  uint8   u_Bp3GloGainStep;
  uint32  q_Bp3GloPreAmplI;
  uint32  q_Bp3GloPreAmplQ;
  uint32  q_Bp3GloRsRateHWU;
  uint8   u_Bp3GloNumActNotch;
  uint16  w_Bp3GloGrpDelay;

  /* M/N:D to derive chipx80 from GPLL0 */
  uint32  q_MNDCntrM;
  uint32  q_MNDCntrN;
  uint32  q_MNDCntrD;

  char    u_Reserved[8];
  mgp_Bp3RfStatusTypeV4 z_Bp3Info[C_MGP_TOTAL_GLO_SUB_CHAN_NUM];

} mgp_RxRfStatusDmLogV4;

/* Enumeration for setting the RF Intelliceiver Linearity */
typedef enum
{
  MGP_INTELLICEIVER_LINEARITY_AUTO = 0,
  MGP_INTELLICEIVER_LINEARITY_HIGH,
  MGP_INTELLICEIVER_LINEARITY_RF_RESERVED, /* Used only via DM cmd and never in mission mode */
  MGP_INTELLICEIVER_LINEARITY_UNINITIALIZED
} mgp_GpsIntelliceiverLinearityTypeVal;
typedef uint8 mgp_GpsIntelliceiverLinearityType;

/* Interface between PPM and PGI modules */
typedef enum
{
  PPM_STATE_DISABLED,
  PPM_STATE_ENABLED,
  PPM_STATE_OPERATION_INTERRUPTED
}
mgp_PpmStateEnumVal;
typedef uint8 mgp_PpmStateEnum;

typedef struct
{
  mgp_PpmStateEnum e_State;
}
mgp_PpmStateDataType;

typedef struct
{
  boolean b_Enabled; /* Blanking config from RF */
  uint32 q_Rx1Counter; /* Blanking counter  for Rx1 */
  uint32 q_Rx2Counter; /* Blanking counter  for Rx2 */
} mgp_QueryBlankingType;

typedef struct
{
  uint8 u_GnssDesiredConfig;    /* flags containing constellations 
                                   desired, uses the 
                                   C_RCVR_GNSS_CONFIG_XXX defines */
  uint8 u_GnssSupportedConfig; /* flags containing constellations 
                                  supported by HW, uses the 
                                  C_RCVR_GNSS_CONFIG_XXX defines */
  uint8 u_GnssActualConfig;    /* flags containing constellations 
                                  actually configured, uses the 
                                  C_RCVR_GNSS_CONFIG_XXX defines */
} mgp_NavConfigType;


typedef struct
{
  uint8  u_AdcIqSatPerStatus;   /* Indicates validity/status of data */
  uint32 q_ISatPer; /* Saturation percentage of I samples */
  uint32 q_QSatPer; /* Saturation percentage of Q samples */
} mgp_QueryAdcIqSatPerType;

typedef struct
{
  uint8 u_RequestCode; /*0-Not used, 1-Enable/disable blanking, 2-Enable/disable DPGA, 3-PGA backoff,
                       4-Enable/disable PGA cal, 5-Query PGA state, 6-Query PGA setting, 7-Set PGA, 8- Enable/disable DCOC*/
  int32 l_RequestParam1;
  int32 l_RequestParam2;
  int32 l_ResponseParam1;
  int32 l_ResponseParam2;
} mgp_NavIoctlType;

typedef struct
{
  /** 0 - NoiseFloorCalRequest
   *  1 - MeasureJammingPower
   *  2 - PopulateNoiseFloorTable
   */
  uint8 u_RequestCode; 
} mgp_NavBPJammerCtlType;

typedef struct
{
  mgp_PeSensorsTimeStamp z_GpsTime;
  locEngVehicleSensorSampleListStructT_v01 z_VehicleGyroSamples;
}
mgp_VehicleGyroDataType;

typedef struct
{
  mgp_PeSensorsTimeStamp z_GpsTime;
  locEngVehicleOdometrySampleListStructT_v01 z_VehicleOdometrySamples;
}
mgp_VehicleOdometryDataType;

/*************************************************************************
  Health information source                        
*************************************************************************/
typedef enum
{
  MGP_GNSS_SV_HEALTH_SRC_EPHE = 0,
  MGP_GNSS_SV_HEALTH_SRC_ALM,
  MGP_GNSS_SV_HEALTH_SRC_SBAS,
  MGP_GNSS_SV_HEALTH_SRC_RTI
} mgp_GnssHealthSrcTypeVal;
typedef uint8 mgp_GnssHealthSrc;

typedef struct
{
  U8 u_FileOp;
  U16 w_RecId;
  U64 t_Mask;
  U32 q_EfsStatus;	      /*File operation status*/
  U64 t_FailedDeleteMask; /*Only used in case of GPS_FS_DELETE_FILE_LIST */
}
mgp_GpsFsTaskNotificationDataType;


typedef enum
{
  MGP_MIN_ENERGYMODES = 0,
  MGP_ENERGYMODE_ACROSS_REBOOTS = MGP_MIN_ENERGYMODES,
  MGP_ENERGYMODE_SINCE_REBOOT,
  MGP_ENERGYMODE_SINCE_RCVR_ON,

  /* Make sure that MGP_MAX_ENERGYMODES is always
    at the end - to be used as total modes count */
  MGP_MAX_ENERGYMODES
}mgp_GnssEnergyMode;


/* Structure returned to the caller of mgp_GetEnergyConsumption API */
typedef struct
{
  boolean b_IsValid;               /* This flag is set to false when
                                  data is unavailable */
  uint64 u_Energy;    /* energy consumed by the receiver */

} mgp_GetEnergyConsumptionType;

/*
 * Function definitions
*/

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
  Sensor LE
*************************************************************************/
typedef struct
{
  uint32 q_SensorDataBufferLenMsecs; /*Length of Sensor MBM in milli seconds*/
}mgp_SensorLEConfigStructType;

/*
 ******************************************************************************
 * _mgp_ReceiverUpdate
 *
 * Function description:
 *
 *    This function starts/stops GPS receiver operation, according to the specified 
 *    configuration. It calls me_ConfigPut() and pe_ConfigPut() to set the configuration 
 *    parameters from ME and PE individually. This function returns after MGP
 *    has been started. 
 *    This API function consolidates current receiver operations including following,
 *       mgp_ReceiverOn()
 *       mgp_ReceiverOff()
 *       mgp_ReceiverForcedOff()
 *       mgp_ReceiverIdle()
 *       mgp_ReceiverForcedIdle()
 *    
 * Parameters: 
 *
 *  e_RcvrCommand - type of receiver operation command
 *  pz_MgpConfig - Pointer to the new MGP configuration
 *
 * Return value: 
 *
 *  None
 * 
 * ******************************************************************************
*/
void _mgp_ReceiverUpdate
( 
  mgp_RcvrCommandEnumType     e_RcvrCommand,
  const mgp_ConfigStructType* pz_MgpConfig
);
void mgp_ReceiverUpdate
( 
  mgp_RcvrCommandEnumType     e_RcvrCommand,
  const mgp_ConfigStructType* pz_MgpConfig
);

/*
 ******************************************************************************
 * _mgp_ReceiverLppmUpdate
 *
 * Function description:
 *
 *    This function starts or updates receiver LPPM operation, according to the specified 
 *    configuration. 
 *    It is required to call this API when turning receiver on in LPPM operation mode, or 
 *    when modifying the parameters for the ongoing LPPM operation. 
 *    TODO:: LPPM on-the-fly parameter update restrictions.
 *    
 * Parameters: 
 *
 *  pz_MgpConfig - Pointer to the new MGP configuration
 *  pz_MgpLppmConfig - Pointer to the LPPM configuration
 *
 * Return value: 
 *
 *  None
 * 
 * ******************************************************************************
*/
void _mgp_ReceiverLppmUpdate
( 
  const mgp_ConfigStructType* pz_MgpConfig,
  const mgp_LppmConfigStructType* pz_MgpLppmConfig 
);
void mgp_ReceiverLppmUpdate
( 
  const mgp_ConfigStructType* pz_MgpConfig,
  const mgp_LppmConfigStructType* pz_MgpLppmConfig 
);

/*
 ******************************************************************************
 * _mgp_ReceiverLPPMQuery
 *
 * Function description:
 *
 *    This function shall be called by SM to query if the receiver can be 
 *    engaged in LPPM operation mode for the specified subconfig. The response 
 *    is sent back to SM by the LPPM status message (see sm_ReportMgpLppmStatus()).
 *    
 * Parameters: 
 *
 *  e_LppmSubconfig - LPPM sub-configuration type
 *
 * Return value: 
 *
 *  None
 * 
 * ******************************************************************************
*/
void _mgp_ReceiverLPPMQuery( mgp_LppmSubConfigEnumType e_LppmSubconfig );
void mgp_ReceiverLPPMQuery( mgp_LppmSubConfigEnumType e_LppmSubconfig );

/*
 ******************************************************************************
 * _mgp_GnssUpdateInfo
 *
 * Function description:
 *
 *    SM uses this command to send information to MGP, which includes SV Acquisition
 *    Assistance information, SV differences, sensitivity assistance, change in QOS, 
 *    Reference Time, Reference Position (includes altitude also), Ephemeris, 
 *    Almanac, Health, Ionospheric corrections, UTC data, DGPS Corrections, 
 *    Real Time integrity and SV directions. All the data will be referenced to GPS time. 
 *    If GPS time is not specified, it will be applied immediately.
 *    This structure may contain only one information element at a time. 
 *
 *    Ideally data can come in any order, and the Receiver should do its best to use 
 *    the available data for processing and should be flexible to change strategy when 
 *    new information is received. Does this apply to MS-Assisted mode also? Should ME 
 *    wait till Acquisition assistance is received from the PDE. Does it make sense to 
 *    start ME as soon as start_MGP is received, and fine tune the search when 
 *    acq-assistance is received?
 *
 *    If Reference Time provided by the PDE (or other means) does not agree with the 
 *    current estimate of time in MGP, what should happen? Should MGP realize that TIME 
 *    has been reset/re-started from a GPS simulator etc, and restart all its 
 *    measurements (this is a requirement from Test team)? Or can we expect the Test
 *    interface to delete assistance data before re-starting external simulators etc.
 *
 * Parameters: 
 *
 *  p_Info - Pointer to the structure containing info
 *
 * Return value:  None
 *
 *
 * ******************************************************************************
*/
void _mgp_GnssUpdateInfo(const gnss_AssistDataInfoStructType *p_Info);
void mgp_GnssUpdateInfo(const gnss_AssistDataInfoStructType *p_Info);


/*
 ******************************************************************************
 * _mgp_RequestAssistDataStatus
 *
 * Function description:
 *
 *   This API is called by SM to request assistance data wishlist and current 
 *   assistance data status from PE.
 *
 * Parameters: 
 *
 *  e_ReqestType - Type of assistance data request  
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_RequestAssistDataStatus(const mgp_AssistDataRequestEnumType e_RequestType);
void mgp_RequestAssistDataStatus(const mgp_AssistDataRequestEnumType e_RequestType);
void mgp_RequestAssistDataStatus_temp(const mgp_AssistDataRequestEnumType e_RequestType);
/*
 ******************************************************************************
 * _mgp_ComputeGpsFix
 *
 * Function description:
 *
 *   This API is called by SM to strobe PE for any available GPS fix. PE will
 *   asynchronously respond with a Fix report if one is available.
 *
 * Parameters: 
 *
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_ComputeGpsFix(void);
void mgp_ComputeGpsFix(void);


/*************************************************************************/
/* MGP Diagnostic Interfaces                                             */
/*************************************************************************/
/*
 ******************************************************************************
 * _mgp_DeleteInfo
 *
 * Function description:
 *
 *    This is a diagnostic API to test the phone in various scenarios (hot/warm/cold).
 *    SM will issue commands to delete particular type of assistance data including
 *    Reference Time (ME time, time-transfer/time-tag time, RTC time, GPS Week number etc),
 *    Reference Position (from PE, Cell Database, KF etc), Ephemeris, Almanac, IONO etc.
 *    If Delete All is specified all assistance data is deleted, creating a cold-start.
 *
 * Parameters: 
 *
 *  p_DeleteInfo - Pointer to the structure specifying the info to be deleted
 *  u_DeleteAll  - If TRUE, all information is deleted from the MGP.
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_DeleteInfo(gps_DeleteInfoStructType *p_DeleteInfo, boolean u_DeleteAll);
void mgp_DeleteInfo(gps_DeleteInfoStructType *p_DeleteInfo, boolean u_DeleteAll);

/*
 ******************************************************************************
 * _mgp_GnssDeleteInfo
 *
 * Function description:
 *
 *    This is a diagnostic API to test the phone in various scenarios (hot/warm/cold).
 *    SM will issue commands to delete particular type of assistance data including
 *    Reference Time (ME time, time-transfer/time-tag time, RTC time, GPS Week number etc),
 *    Reference Position (from PE, Cell Database, KF etc), Ephemeris, Almanac, IONO etc.
 *    If Delete All is specified all assistance data is deleted, creating a cold-start.
 *
 * Parameters: 
 *
 *  p_DeleteInfo - Pointer to the structure specifying the info to be deleted
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/
void _mgp_GnssDeleteInfo(gnss_DeleteInfoStructType *p_DeleteInfo);
void mgp_GnssDeleteInfo(gnss_DeleteInfoStructType *p_DeleteInfo);


/*
 ******************************************************************************
 * _mgp_ModifyTimeUnc
 *
 * Function description:
 *
 *    This function is called by SM to modify GPS time uncertainty. 
 *
 * Parameters: 
 *  
 *  f_NewTimeUnc - New time uncertainty (msec)
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

//void _mgp_ModifyTimeUnc(FLT f_NewTimeUnc);
//void mgp_ModifyTimeUnc(FLT f_NewTimeUnc);


/*
 ******************************************************************************
 * _mgp_GnssModifyTimeUnc
 *
 * Function description:
 *
 *    This function is called by SM to modify GNSS(GPS or GLO) time uncertainty. 
 *
 * Parameters: 
 *  
 *  mgp_GnssType  - e_GnssType
 *  f_NewTimeUnc - New time uncertainty (msec)
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_GnssModifyTimeUnc(mgp_GnssType e_GnssType, FLT f_NewTimeUnc);
void mgp_GnssModifyTimeUnc(mgp_GnssType e_GnssType, FLT f_NewTimeUnc);

/*
 ******************************************************************************
 * _mgp_ModifyFreqUnc
 *
 * Function description:
 *
 *    This function is called by SM to modify GPS freq uncertainty. 
 *
 * Parameters: 
 *
 *  f_NewFreqUnc - New freq uncertainty (meters/sec)
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_ModifyFreqUnc(FLT f_NewFreqUnc);
void mgp_ModifyFreqUnc(FLT f_NewFreqUnc);

/*
 ******************************************************************************
 * mgp_ModifyPositionUnc
 *
 * Function description:
 *
 *    This function is called by SM to modify Reference Position uncertainty. 
 *    It reads the current position estimate, modifies it and forces into PE.
 *
 * Parameters: 
 *
 *  f_NewPosUnc - New position uncertainty (meters)
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_ModifyPosUnc(FLT f_NewPosUnc);
void mgp_ModifyPosUnc(FLT f_NewPosUnc);

/*
 ******************************************************************************
 * _mgp_ModifyAltitudeUnc
 *
 * Function description:
 *
 *    This function is called by SM to modify Altitude uncertainty. 
 *    It reads the current position estimate, modifies it and forces into PE.
 *
 * Parameters: 
 *
 *  f_NewAltUnc - New position uncertainty (meters)
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_ModifyAltitudeUnc(FLT f_NewAltUnc);
void mgp_ModifyAltitudeUnc(FLT f_NewAltUnc);

/*
 ******************************************************************************
 * _mgp_AddTimeOffset
 *
 * Function description:
 *
 *    This function is called by SM to add an offset to the GPS time. 
 *
 * Parameters: 
 *
 *  f_TimeOffset - Time offset to be added (msec)
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_AddTimeOffset(FLT f_TimeOffset);
void mgp_AddTimeOffset(FLT f_TimeOffset);


/*
 ******************************************************************************
 * _mgp_GnssAddTimeOffset
 *
 * Function description:
 *
 *    This function is called by SM to add an offset to the GNSS(GPS or GLO) time. 
 *
 * Parameters: 
 *
 *  mgp_GnssType - e_GnssType
 *  f_TimeOffset - Time offset to be added (msec)
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_GnssAddTimeOffset(mgp_GnssType e_GnssType, FLT f_TimeOffset);
void mgp_GnssAddTimeOffset(mgp_GnssType e_GnssType, FLT f_TimeOffset);

/*
 ******************************************************************************
 * _mgp_AddFreqOffset
 *
 * Function description:
 *
 *    This function is called by SM to add an offset to the GPS freq. It reads the 
 *    current freq estimate, modifies it and forces into ME.
 *
 * Parameters: 
 *
 *  f_FreqOffset - Freq offset to be added (meters/sec)
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_AddFreqOffset(FLT f_FreqOffset);
void mgp_AddFreqOffset(FLT f_FreqOffset);

/*
 ******************************************************************************
 * _mgp_GetGpsRtcMs
 *
 * Function description:
 *
 *    mgp_GetGpsRtcMs obtains the current value of the GPS RTC in milliseconds
 *    (extended to 32-bits). Lower 19-bits match the current Hardware 
 *    millisecond part for the RTC if GPS clocks are running.
 *    If this function is called when GPS clocks are off, it returns the last
 *    accessed value of GpsRtcMs with a return status of FALSE.
 *
 * Parameters: 
 *
 *  p_GpsRtcMs - Pointer for the variable where GpsRtcMs will be stored
 *               
 * Return value: 
 *
 *  TRUE if the GpsRtc is valid (i.e. GPS Clock is running).
 *
 *
 * ******************************************************************************
*/

boolean _mgp_GetGpsRtcMs(uint32 *p_GpsRtcMs);
boolean mgp_GetGpsRtcMs(uint32 *p_GpsRtcMs);

/*
 ******************************************************************************
 * _mgp_GetExternalGpsRtcMs
 *
 * Function description:
 *
 *    mgp_GetExternalGpsRtcMs obtains the current value of the Millisecond counter
 *    (extended to 32-bits). This will have an offset from the real hardware GPSRTC
 *    for DPO modes. In DPO, GPSRTC will have discontinuities (turned off to save power).
 *    But the ExternalGpsRtcMs will be adjusted to include the OFF_TIME so that
 *    external clients are not affected by DPO.
 *
 * Parameters:
 *
 *  p_GpsRtcMs - Pointer for the variable where GpsRtcMs will be stored
 *
 * Return value:
 *
 *  TRUE if the GpsRtc is valid (i.e. GPS Clock is running).
 *
 * ******************************************************************************
*/

boolean _mgp_GetExternalGpsRtcMs(uint32 *p_GpsRtcMs);
boolean mgp_GetExternalGpsRtcMs(uint32 *p_GpsRtcMs);

/*
 ******************************************************************************
 * _mgp_GetGnssRtcMs
 *
 * Function description:
 *
 *    mgp_GetGnssRtcMs obtains the current value of the GNSS RTC in milliseconds
 *    (extended to 32-bits). Lower 19-bits match the current Hardware 
 *    millisecond part for the RTC if GNSS clocks are running.
 *    If this function is called when GNSS clocks are off, it returns the last
 *    accessed value of GnssRtcMs with a return status of FALSE.
 *
 * Parameters: 
 *
 *  p_GnssRtcMs - Pointer for the variable where GnssRtcMs will be stored
 *               
 * Return value: 
 *
 *  TRUE if the GnssRtc is valid (i.e. GNSS Clock is running).
 *
 *
 * ******************************************************************************
*/

boolean _mgp_GetGnssRtcMs(uint32 *p_GnssRtcMs);
boolean mgp_GetGnssRtcMs(uint32 *p_GnssRtcMs);

/*
 ******************************************************************************
 * _mgp_CDMASearcherOn
 *
 * Function description:
 *
 *    This function starts CDMA searcher operation to collect AFLT measurements.
 *    QoS value specifies whether Quick or Normal PPM is used.
 *    
 * Parameters: 
 *
 *  u_QoS - Quality of Service value defining whether Quick or Normal PPM is used.
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_CDMASearcherOn(const mgp_CdmaSearcherOnCmdType *p_CdmaSearcherOnCmd);
void mgp_CDMASearcherOn(const mgp_CdmaSearcherOnCmdType *p_CdmaSearcherOnCmd);


/*
 ******************************************************************************
 * _mgp_CDMASearcherOff
 *
 * Function description:
 *
 *    This function stops CDMA searcher and AFLT measurements.
 *    
 * Parameters: 
 *
 *  None
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_CDMASearcherOff(uint32 q_RequestingTask);
void mgp_CDMASearcherOff(uint32 q_RequestingTask);


/*
 ******************************************************************************
 * _mgp_CDMAGetPpmResults
 *
 * Function description:
 *
 *    This function request for the PPM results from CDMA searcher
 *    
 * Parameters: 
 *
 *  u_EndPpm - If TRUE, PPM processing stops
 *  q_RequestingTask - Which task is calling the API (LM or P.PPM)
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_CDMAGetPpmResults
(
   boolean   u_EndPpm,
   uint32    q_RequestingTask
   );
void mgp_CDMAGetPpmResults
(
   boolean   u_EndPpm,
   uint32    q_RequestingTask
   );

/*
 ******************************************************************************
 * _mgp_CDMAStartFTCal
 *
 * Function description:
 *
 *    This function restarts PPM Fast TCal session if it is currently
 *    active.
 *    
 * Parameters: 
 *
 *  None
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_CDMAStartFTCal(void);
void mgp_CDMAStartFTCal(void);

/*
 ******************************************************************************
 * _mgp_FTMSetSearchMode
 *
 * Function description:
 *
 *  This function sets the search mode that the FTM strategy will use.
 *    
 * Parameters: 
 *
 *  u_SearchMode - Search Mode to be used
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_FTMSetSearchMode(uint8 u_SearchMode);
void mgp_FTMSetSearchMode(uint8 u_SearchMode);


/*
 ******************************************************************************
 * _mgp_FTMSetJammerDetect
 *
 * Function description:
 *
 *  This function sets whether Jammer Detect is turned on or off.
 *    
 * Parameters: 
 *
 *  u_DisableJammerDetect - True to turn OFF jammer detect
 *
 * Return value: 
 *
 *  None
 *
 * ******************************************************************************
*/

void _mgp_FTMSetJammerDetect(boolean u_DisableJammerDetect);
void mgp_FTMSetJammerDetect(boolean u_DisableJammerDetect);

/*
 ******************************************************************************
 * _mgp_SvAlmHealthPut
 *
 * Function description:
 *
 *  mgp_SvAlmHealthPut allows the application to load the almanac health
 *  attributes of a satellite into the Measurement Engine
 *  UNKNOWN, GOOD, BAD and NOEXIST are available. The satellite selection 
 *  algorithm treats UNKNOWN as GOOD.  An additional bit is included that
 *  allows the application to force the measurement engine to ignore the
 *  health setting. This would cause the selection process to treat the
 *  health as good. 
 *
 *
 * Parameters: 
 *
 *  u_Sv - Specifies the satellite ID (1 - 32) 
 *  u_SvHealth - Almanac health
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void _mgp_SvAlmHealthPut(uint8 u_Sv, uint8 u_HealthNet);
void mgp_SvAlmHealthPut(uint8 u_Sv, uint8 u_HealthNet);

/*
 ******************************************************************************
 * _mgp_SendPrescDwellRequest
 *
 * Function description:
 *
 *  mgp_SendPrescDwellRequest allows the application to load the parameters needed by MGP 
 * to start a prescribed dwell test.
 *
 * Parameters: 
 *
 *  p_Payload - Pointer to data needed to be injected into MGP.
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void _mgp_SendPrescDwellRequest(const mgp_PrescDwellCmd *p_PdCmd);
void mgp_SendPrescDwellRequest(const mgp_PrescDwellCmd *p_PdCmd);


/*
 ******************************************************************************
 * _mgp_SendSpectrumAnalyzerCmdRequest
 *
 * Function description:
 *
 *  mgp_SendSpectrumAnalyzerCmdRequest allows the application to load the parameters needed by MGP
 * to start a Spectrum Analyzer Scan.
 *
 * Parameters:
 *
 *  p_Cmd - Pointer to SpectrumAnalyzer Command to be injected into MGP.
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/

void _mgp_SendSpectrumAnalyzerCmdRequest(const mgp_SpectrumAnalyzerCmd *p_Cmd);
void mgp_SendSpectrumAnalyzerCmdRequest(const mgp_SpectrumAnalyzerCmd *p_Cmd);


/*
 ******************************************************************************
 * _mgp_SendIqTestRequest
 *
 * Function description:
 *
 *  mgp_SendIqTestRequest allows the application to load the parameters needed by MGP 
 * to start a spectral test.
 *
 * Parameters: 
 *
 *  p_Payload - Pointer to data needed to be injected into MGP.
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
void _mgp_SendIqTestRequest(const mgp_IqTestCmdStructType *p_IqTestCmd);
void mgp_SendIqTestRequest(const mgp_IqTestCmdStructType *p_IqTestCmd);

/*
 ******************************************************************************
 * _mgp_SendTsgConfigRequest
 *
 * Function description:
 *
 *  mgp_SendTsgConfigRequest allows the application to load parameters needed by MGP 
 * to setup the Test Source Generator.
 *
 * Parameters: 
 *
 *  p_TsgConfigCmd - Pointer to data needed to be injected into MGP.
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void _mgp_SendTsgConfigRequest(const mgp_TsgConfig *p_TsgConfigCmd);
void mgp_SendTsgConfigRequest(const mgp_TsgConfig *p_TsgConfigCmd);

/**
 * @brief mgp_BPJammerCtrlCmd sends the diag command 
 *        request to test the BP based Jammer detection HW
 *        module.
 * 
 * 
 * @param p_BPJammerCtlCmd Diag control parameters
 */
void _mgp_BPJammerCtrlCmd(mgp_NavBPJammerCtlType *p_BPJammerCtlCmd);
void mgp_BPJammerCtrlCmd(mgp_NavBPJammerCtlType *p_BPJammerCtlCmd);

/******************************************************************************
 * _mgp_SendDiagRfCmd
 *
 * Function description:
 *
 *  mgp_SendDiagRfCmd allows the diag application to send RF cmds to GNSS RF interface.
 *
 * Parameters: 
 *
 *  p_DiagRfCmd - Pointer to data needed to be injected into MGP.
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void _mgp_SendDiagRfCmd(const mgp_DiagRfCmd *p_DiagRfCmd);
void mgp_SendDiagRfCmd(const mgp_DiagRfCmd *p_DiagRfCmd);

/*
 ******************************************************************************
 * _mgp_SendDiagPrxRfCmd
 *
 * Function description:
 *
    mgp_SendDiagPrxRfCmd allows the diag application to send Prx RF cmds to
    GNSS RF interface.
 *
 * Parameters: 
 *
 *  p_DiagPrxRfCmd - Pointer to data needed to be injected into MGP.
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void _mgp_SendDiagPrxRfCmd(const mgp_DiagPrxRfCmd *p_DiagPrxRfCmd);
void mgp_SendDiagPrxRfCmd(const mgp_DiagPrxRfCmd *p_DiagPrxRfCmd);

/*
 ******************************************************************************
 * _mgp_SendDiagRfLinearityCmd
 *
 * Function description:
 *
    mgp_SendDiagRfLinearityCmd allows the diag application to send RF Linearity
    cmds to GNSS RF interface.
 *
 * Parameters: 
 *
 *  p_DiagRfLinearityCmd - Pointer to data needed to be injected into MGP.
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void _mgp_SendDiagRfLinearityCmd(const mgp_DiagRfLinearityCmd *p_DiagRfLinearityCmd);
void mgp_SendDiagRfLinearityCmd(const mgp_DiagRfLinearityCmd *p_DiagRfLinearityCmd);

/*
 ******************************************************************************
 * _mgp_SendMemBwTestRequest
 *
 * Function description:
 *
 *  mgp_SendMemBwTestRequest allows the application to load parameters needed by MGP 
 * to start a memory bandwidth test.
 *
 * Parameters: 
 *
 *  p_Payload - Pointer to data needed to be injected into MGP.
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void _mgp_SendMemBwTestRequest(const mgp_MemBwTestParams *p_MemBwTestParams);
void mgp_SendMemBwTestRequest(const mgp_MemBwTestParams *p_MemBwTestParams);

/*
 ******************************************************************************
 * Function _mgp_RfSynthModifyPut
 *
 * Description:
 *
 *  This function allows the RF driver to report the RF synthesizer
 *  modification events to the GNSS engine. 
 *
 *  The problems encountered with the RTR6285 and Ramsis RF solutions 
 *  will likely require that the RF Driver reprogram the GPS VCO to 
 *  address. A side effect of programming the GPS synthesizer is that a 
 *  180 degree carrier phase ambiguity may be imposed.
 * 
 *  The GNSS SW can takes steps to accommodate the carrier phase ambiguity, 
 *  but needs to know the GNSS RTC at the moment of modification.
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
extern void _mgp_RfSynthModifyPut(void);
extern void mgp_RfSynthModifyPut(void);


/*
 ******************************************************************************
 * gnss_RfMeasCmdPut
 *
 * Description: 
 *  The baseband analog signal may have a large
 *  DC offset due to the mixer circuits within the RF component.
 *  It is mandatory to compensate most of the DC offset before the SDM
 *  interface. 
 *
 *  This function provides an interface for RF drivers to enable and 
 *  configure the measurement period for the preliminary mean and 
 *  amplitude estimators. This triggers to the start and stop of 
 *  mean/amplitude estimation period. RF drivers will then read the 
 *  mean estimator to correct large DC offsets.  This setting applies 
 *  to all four sync and async ADC processors.  In the fixed integration 
 *  mode, the integration period is always 1ms. 
 *
 * Parameters:
 *  gnss_MeanMonConfigType that captures new linearity setting
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfMeasConfigPut(const gnss_MeanMonConfigEnum e_Config);
extern void mgp_RfMeasConfigPut(const gnss_MeanMonConfigEnum e_Config);


/*
 ******************************************************************************
 * _mgp_RfMeasCmdPut
 *
 * Description: 
 *  The baseband analog signal may have a large
 *  DC offset due to the mixer circuits within the RF component.
 *  It is mandatory to compensate most of the DC offset befreo the SDM
 *  interface. 
 *
 *  This function provides an interface for RF drivers to enable and 
 *  configure the measurement period for the preliminary mean and 
 *  amplitude estimators.  This triggers the start and stop of 
 *  mean/amplitude estimation period. RF drivers will then read the 
 *  mean estimator to correct large DC offsets.  This setting applies 
 *  to all four sync and async ADC processors.  
 *
 *  The RF driver calling sequence for mean estimation is: 
 *  1. Start the mean estimation with a call to 
 *     gnss_RfcMeasEnablePut() with gnss_RfMeasCmdEnum = MEAS_START; 
 *  2. Wait (RF drivers timer, in MC task context)
 *  3. Stop the mean estimation with a call to 
 *     gnss_RfMeasEnablePut() with gnss_RfMeasCmdEnum = MEAS_STOP; 
 *  4. Read the mean estimates with a call to gnss_RfAdcProcMeanMonGet().
 *
 *  In the fixed integration mode, the integration period is always 1ms, 
 *  and calls to this API are then unnecessary.
 *
 * Parameters:
 *  gnss_RfMeasCmdType that captures new linearity setting
 *
 * Return value: 
 *  None
 *
 ******************************************************************************
*/
extern void _mgp_RfMeasCmdPut(const gnss_RfMeasCmdEnum e_MeasCmd);
extern void mgp_RfMeasCmdPut(const gnss_RfMeasCmdEnum e_MeasCmd);

/*
 ******************************************************************************
 * _mgp_RfAdcProcMeanMonGet
 *
 * Description: 
 *  The baseband analog signal may have a large
 *  DC offset due to the mixer circuits within the RF component.
 *  It is mandatory to compensate most of the DC offset befreo the SDM
 *  interface. 
 *
 *  This function provides an interface for RF drivers to query the 
 *  I/Q means of the digitized  IF signal, before any downconversion. 
 *  MC is responsible for mean values from the bit fields AD_PREMEANMONI/Q.
 *  To do this calculation, Callisto must check the bit fields AD_MEAS_PERIOD 
 *  and AD_MEAS_BLANK.
 *
 *  In the asynchronous mean estimation mode (AD_MEAS_PERIOD = 0), the bit 
 *  field AD_MEAS_LENGTH contains the number of samples collected to form the 
 *  mean estimate.  Assuming AD_MEAS_BLANK = 0, the accumulated sums in 
 *  AD_PREMEANMONI/Q must be divided by AD_MEAS_LENGTH to give mean estimates.
 *
 *  If AD_MEAS_BLANK = 1, then the mean estimate is invalid.  Since there is 
 *  no way of knowing how many samples were blanked, this means that variable 
 *  integration for mean/amplitude estimation cannot be used in GSM mode.
 *
 *
 *  The RF driver calling sequence for mean estimation is: 
 *  1. Start the mean estimation with a call to 
 *     gnss_RfcMeasEnablePut() with gnss_RfMeasCmdType = MEAS_START; 
 *  2. Wait (RF drivers timer, in MC task context)
 *  3. Stop the mean estimation with a call to 
 *     gnss_RfMeasEnablePut() with gnss_RfMeasCmdType = MEAS_STOP; 
 *  4. Read the mean estimates with a call to gnss_RfAdcProcMeanMonGet().
 *
 *  In the fixed integration mode, the integration period is always 1ms, 
 *  and calls to this API are then unnecessary.
 *
 * Parameters:
 *  gnss_AdcProcEnum - The ADC processing block 
 *                     (async ADC proc 1 or 2, sync ADC proc 1 or 2).
 *
 *  gnss_AdcProcMeanMonType - Gets the status registers for either the 
 *                            async or sync ADC processing block that 
 *                            monitors the I/Q mean.Function must be 
 *                            called as part of GPS RF turn on processing
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfAdcProcMeanMonGet
(
   const gnss_AdcProcEnum   e_AdcProc,
   gnss_MeanMonType         *p_AdcProcMeanMon
   );
extern void mgp_RfAdcProcMeanMonGet
(
   const gnss_AdcProcEnum   e_AdcProc,
   gnss_MeanMonType         *p_AdcProcMeanMon
   );


/*
 ******************************************************************************
 * _mgp_RfAdcProcConfigPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to configure bit 
 *  fields in each BP.  RF drivers will call this function once on 
 *  narrowband mode entry or once on diversity mode entry. The RF driver 
 *  is controlling the mapping of ADC processor to BP.  So the driver may 
 *  have a redirection layer that is RF card dependent. Function must be 
 *  called as part of GPS RF turn on processing.
 *
 * Parameters:
 *  gnss_AdcProcEnum - The ADC processing block 
 *                     (async ADC proc primary or diversity
 *                      or sync ADC proc 1 or 2).
 *
 *  gnss_AdcProcConfigPutType - Sets the static controls for either the 
 *                              async or sync ADC processing block. 
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfAdcProcConfigPut
(
   const gnss_AdcProcEnum     e_AdcProc,
   gnss_AdcProcConfigPutType  *p_AdcProcConfigPut
   );
extern void mgp_RfAdcProcConfigPut
(
   const gnss_AdcProcEnum     e_AdcProc,
   gnss_AdcProcConfigPutType  *p_AdcProcConfigPut
   );


/*
 ******************************************************************************
 * _mgp_RfAdcProcIqCntrlPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to configure bit 
 *  fields in each BP.  RF drivers will call this function once on 
 *  narrowband mode entry, wideband entry, or once on diversity mode entry.
 *  These bit fields could change dynamically.  RF drivers might need 
 *  different settings in high and low linearity. 
 *
 * Parameters:
 *  gnss_AdcProcEnum - The ADC processing block 
 *                     (async ADC proc primary or diversity
 *                      or sync ADC proc 1 or 2).
 *
 *  gnss_AdcProcIqCntrlType - Sets the static controls for either the 
 *                            async or sync ADC processing block for 
 *                            swapping I/Q or sign inverting I/Q data. 
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfAdcProcIqCntrlPut
(
   const gnss_AdcProcEnum     e_AdcProc,
   gnss_AdcProcIqCntrlType    *p_IqCntrl
   );
extern void mgp_RfAdcProcIqCntrlPut
(
   const gnss_AdcProcEnum     e_AdcProc,
   gnss_AdcProcIqCntrlType    *p_IqCntrl
   );


/*
 ******************************************************************************
 * _mgp_RfAdcProcBlankConfigPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to configure the 
 *  blanking configuration, such as the enable/disable.  RF API must call 
 *  this API for primary and diversity separately. There are two blanking 
 *  controls which map to either the primary or diversity path selection.
 *  MC will map the e_AdcProc to the correct blanking control path 
 *  (either primary or diversity path ADC processor).
 *
 * Parameters:
 *  gnss_AdcProcEnum - The ADC processing block 
 *                     (async ADC proc primary or diversity
 *                      or sync ADC proc 1 or 2).
 *
 *  gnss_AdcProcBlankConfigType - Sets the static control for blanking.
 *                                (Does not blank the signal)
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfAdcProcBlankConfigPut
(
   const gnss_AdcProcEnum         e_AdcProc,
   gnss_AdcProcBlankConfigType    *p_BlankConfig
   );
extern void mgp_RfAdcProcBlankConfigPut
(
   const gnss_AdcProcEnum         e_AdcProc,
   gnss_AdcProcBlankConfigType    *p_BlankConfig
   );


/*
 ******************************************************************************
 * _mgp_RfAdcProcBlankPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to start software 
 *  blanking, which takes effect immediately. The intelliceiver calls this 
 *  API during transitions.  RF API must call this API for primary and 
 *  diversity separately. There are two blanking controls which map to 
 *  either the primary or diversity path selection.  MC will map the 
 *  e_AdcProc to the correct blanking control path 
 *  (either primary or diversity path ADC processor).
 *
 * Parameters:
 *  gnss_AdcProcEnum - The ADC processing block 
 *                     (async ADC proc primary or diversity
 *                      or sync ADC proc 1 or 2).
 *
 *  boolean u_BlankSig - Blank signal command. This command is 
 *                       ORed with the blank signal input to the core.
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfAdcProcBlankPut
(
   const gnss_AdcProcEnum   e_AdcProc,
   boolean                  u_BlankSig
   );
extern void mgp_RfAdcProcBlankPut
(
   const gnss_AdcProcEnum   e_AdcProc,
   boolean                  u_BlankSig
   );





/*
 ******************************************************************************
 * _mgp_RfBpConfigPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to configure bit 
 *  fields in each BP.  RF drivers will call this function once on 
 *  narrowband mode entry, once on diversity mode entry, and three times 
 *  on wideband mode entry (NB, WB, Glonass BPs).
 *  
 *  Callisto software should be in control of certain bit fields in each BP.
 *  For NB and diversity BP, the bit field BP[1-2]_SCH[1-3]_EN controls this 
 *  option. The wideband BP does not have subchannels but uses the bit field 
 *  BP4_MODE to select from several matched filter options. The Glonass BP 
 *  has 12 subchannels, each with controls for the fine rotator frequency 
 *  (BP3_SCH[1-12]_FREQ) and matched filter selection. (BP3_SCH[1-12]_MODE).
 *
 *  MC must ensure that all BP clocks have been enabled before writing to 
 *  the appropriate BP registers.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpConfigPutType - Sets the static controls for the BP specified 
 *                         by the first parameter 
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfBpConfigPut
(
   const gnss_BpEnum       e_Bp,
   gnss_BpConfigPutType    *p_Config
   );
extern void mgp_RfBpConfigPut
(
   const gnss_BpEnum       e_Bp,
   gnss_BpConfigPutType    *p_Config
   );


/*
 ******************************************************************************
 * _mgp_RfBpIfDownConvPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to set the coarse 
 *  rotator frequency in the BPs. The RF driver shall convert the desired 
 *  frequency shift in Hz to the frequency command format.
 *
 *  Note: The baseband processors provides the same capability, but is 
 *  bypassed when using the auxiliary inputs.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpIfDownConvType - Sets the IF down converter frequency 
 *                          and type (LIF or ZIF)
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfBpIfDownConvPut
(
   const gnss_BpEnum        e_Bp,
   gnss_BpIfDownConvType    *p_IfDownConv
   );
extern void mgp_RfBpIfDownConvPut
(
   const gnss_BpEnum        e_Bp,
   gnss_BpIfDownConvType    *p_IfDownConv
   );


/*
 ******************************************************************************
 * _mgp_RfBpEqCoefPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to write the complex 
 *  equalizer coefficients in the BPs.  These coefficients can change 
 *  dynamically, for example in high and low linearity.
 *
 *  MC must ensure that all BP clocks have been enabled before writing 
 *  to the appropriate BP registers.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpEqCoefType - Structure containing the settings for the FIR 
 *                      complex equalizer with programmable coefficients. 
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfBpEqCoefPut
(
   const gnss_BpEnum        e_Bp,
   gnss_BpEqCoefType        *p_EqCoef
   );
extern void mgp_RfBpEqCoefPut
(
   const gnss_BpEnum        e_Bp,
   gnss_BpEqCoefType        *p_EqCoef
   );

/*
 ******************************************************************************
 * _mgp_RfBpGroupDelayPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to set the group 
 *  delay jump in nanoseconds. The group delay input argument is a jump 
 *  relative to low-linearity mode, not an absolute jump. If the input 
 *  argument is +10ns, then BP_INTDELAY and BP_FRACDELAY bit fields should 
 *  be programmed to ?10ns.
 *
 *  For example:
 *  1. Enter low linearity mode: gnss_RfGroupDelayPut(0ns).
 *  2. Enter high linearity mode: gnss_RfGroupDelayPut(50ns) if RF delay 
 *     is 50ns greater in high linearity than in low linearity. The input 
 *     argument is the jump relative to low linearity mode, not the absolute 
 *     jump.
 *
 *  Group delay differences being corrected through this API:
 *  1. Any jumps due to change of sampling rate when Intelliceiver state 
 *     changes.
 *  2. Difference in RF/analog delay between wideband and narrowband modes.
 *  3. Difference in RF/analog delay between primary NB and diversity paths.  
 *
 *  MC must ensure that all BP clocks have been enabled before writing to the 
 *  appropriate BP registers.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_RfGrpDelay - Group Delay in Nanoseconds, either HW or SW compensation
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfBpGroupDelayPut
(
   const gnss_BpEnum        e_Bp,
   gnss_RfGrpDelayType      *p_GrpDelay
   );
extern void mgp_RfBpGroupDelayPut
(
   const gnss_BpEnum        e_Bp,
   gnss_RfGrpDelayType      *p_GrpDelay
   );


/*
 ******************************************************************************
 * _mgp_RfBpNotchFltrConfigPut
 *
 * Description: 
 *  This function provides an interface for RF drivers to the notch filters 
 *  (up to six filters) in each BP. MC will convert the frequency in Hz into 
 *  the correct command register format.
 *
 *  MC must ensure that all BP clocks have been enabled before writing to 
 *  the appropriate BP registers.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpNotchFltrType - Sets the notch filter bandwidth, center 
 *                         frequency and mode.
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfBpNotchFltrConfigPut
(
   const gnss_BpEnum        e_Bp,
   gnss_BpNotchFltrType     *p_NotchFltr
   );
extern void mgp_RfBpNotchFltrConfigPut
(
   const gnss_BpEnum        e_Bp,
   gnss_BpNotchFltrType     *p_NotchFltr
   );

/*
 ******************************************************************************
 * _mgp_RfBpMeanEstPut
 *
 * Description: 
 *  Provides write access to configure the mean estimator block such as BP 
 *  bit fields BP_DCALIGN, BP_OFFSETI, and BP_OFFSETQ to allow software 
 *  control of the DC filter.
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpMeanEstType - Structure that stores the mean estimate 
 *                       configuration
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfBpMeanEstPut
(
   const gnss_BpEnum        e_Bp,
   gnss_BpMeanEstType       *p_MeanEst
   );
extern void mgp_RfBpMeanEstPut
(
   const gnss_BpEnum        e_Bp,
   gnss_BpMeanEstType       *p_MeanEst
   );


/*
 ******************************************************************************
 * _mgp_RfBpSubChanThreshPut
 *
 * Description: 
 *  Provides write access to the BP bit fields BP_GAINALIGN, BP_THRESHI, 
 *  and BP_THRESHQ to allow software control of AGC. Overriding the AGC may 
 *  enable end-to-end noise figure measurements using a calibrated noise 
 *  source.  
 *
 *  NOTE: This API is not expected to be exercised normal operation mode, 
 *        but may be useful to debug problems.  
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpSubChanThreshType - Structure that stores sub-channel 
 *                             configuration
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfBpSubChanThreshPut
(
   const gnss_BpEnum             e_Bp,
   gnss_BpSubChanThreshType      *p_SubChanThresh
   );
extern void mgp_RfBpSubChanThreshPut
(
   const gnss_BpEnum             e_Bp,
   gnss_BpSubChanThreshType      *p_SubChanThresh
   );


/*
 ******************************************************************************
 * _mgp_RfBpPreAmplGet
 *
 * Description: 
 *  This function provides an interface for RF drivers to get the value 
 *  of preliminary amplitude monitor BP registers.  The amplitude estimates 
 *  will be used for coarse gain trimming in the RFIC. The integration period 
 *  is controlled by bit field AD_MEAS_PERIOD and AD_MEAS_ENABLE in the ADC 
 *  processing block. MC should scale the accumulator value.  
 *
 * Parameters:
 *  gnss_BpEnum - Select one of four BPs
 *                ( BP1 LBW, BP2 DRx, BP3 GLO, BP4 HBW )
 *
 *  gnss_BpPreAmplMonType - Query the current register values for 
 *                             preliminary amplitude estimates.
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfBpPreAmplGet
(
   const gnss_BpEnum             e_Bp,
   gnss_BpPreAmplMonType         *p_PreAmplMon
   );
extern void mgp_RfBpPreAmplGet
(
   const gnss_BpEnum             e_Bp,
   gnss_BpPreAmplMonType         *p_PreAmplMon
   );

#ifndef FEATURE_NAVRF
/*
 ******************************************************************************
 *  _mgp_RfStatusReportPut
 *
 * Function description:
 *
 *  Stores the data reported by the RF driver to be logged later.
 *
 * Parameters: 
 *  gnss_RfStatusType - data reported
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
extern void _mgp_RfStatusReportPut
(
   gnss_RfStatusType          *p_RfReport
   );
extern void mgp_RfStatusReportPut
(
   gnss_RfStatusType          *p_RfReport
   );

/*
 ******************************************************************************
 *  _mgp_RfDynamicStatusReportPut
 *
 * Function description:
 *
 *  Stores the data reported by the RF driver to be logged later.
 *
 * Parameters: 
 *  gnss_RfDynamicStatusType - data reported
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
void _mgp_RfDynamicStatusReportPut
(
   gnss_RfDynamicStatusType  *p_RfDynamicReport
   );
void mgp_RfDynamicStatusReportPut
(
   gnss_RfDynamicStatusType  *p_RfDynamicReport
   );
#endif

/*
 ******************************************************************************
 *  _mgp_RfSamplClkRatePut
 *
 * Function description:
 *
 *  This function configures the sample clock source MUX when using the 
 *  synchronous ADC processor.
 *
 * Parameters: 
 *
 *   gnss_AdcProcSampleRateEnum d_SamplRate - Desired sample clock 0 rate
 *   (chipx80/chipx40/chipx20/off)
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
void _mgp_RfSamplClkRatePut
(
   gnss_AdcProcSampleRateEnum d_SamplRate
   );
void mgp_RfSamplClkRatePut
(
   gnss_AdcProcSampleRateEnum d_SamplRate
   );

/*
 ******************************************************************************
 * Function _mgp_EstimatedReferenceOscillatorOffsetPut
 *
 * Description:
 *
 *  This function is called to provide MGP with the pertinent reference
 *  oscillator offset parameters such as drift and drift rate
 *
 * Parameters: 
 *
 * p_Offset - Pointer to Reference Oscillator Offset Structure
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
void _mgp_EstimatedReferenceOscillatorOffsetPut(cgps_RefOscOffsetType *p_Offset);
void mgp_EstimatedReferenceOscillatorOffsetPut(cgps_RefOscOffsetType *p_Offset);

#ifdef FEATURE_SRCH_CGPS_IF
/*
 ******************************************************************************
 * Function _mgp_1xSendSystemTimeUncertaintyEstimate
 *
 * Description:
 *
 *  This function is indirectly called by 1x SRCH task to pass System Time
 *  Uncertainty Estimate to PGI module.
 *
 * Parameters: 
 *
 *  System Time Uncertainty Estimate in microseconds units.
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

extern void _mgp_1xSendSystemTimeUncertaintyEstimate(uint32);
extern void mgp_1xSendSystemTimeUncertaintyEstimate(uint32);


/*
 ******************************************************************************
 * Function _mgp_1xDisableTTDataRecordingAck
 *
 * Description:
 *
 *  This function is indirectly called by 1x SRCH task to inform PGI module
 *  that Time Transfer Data Recording is disabled.
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

extern void _mgp_1xDisableTTDataRecordingAck(void);
extern void mgp_1xDisableTTDataRecordingAck(void);


/*
 ******************************************************************************
 * Function _mgp_1xSendTTData
 *
 * Description:
 *
 *  This function is indirectly called by 1x SRCH task to pass Time Transfer
 *  Data packet to PGI module.
 *
 * Parameters: 
 *
 *  Time Transfer Data structure.
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

extern void _mgp_1xSendTTData(const cgps_1xTTDataType *);
extern void mgp_1xSendTTData(const cgps_1xTTDataType *);


/*
 ******************************************************************************
 * Function _mgp_1xSendSclkTTData
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to provide a Slow Clock Time
 *  Transfer data packet to the PGI module.
 *  
 *
 * Parameters: 
 *
 *  Data structure which contains information with which Time can be transferred
 *  to the Slow Clock.
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
extern void _mgp_1xSendSclkTTData(const cgps_1xSclkTTDataType *p_SclkTTData);
extern void mgp_1xSendSclkTTData(const cgps_1xSclkTTDataType *p_SclkTTData);

/*
 ******************************************************************************
 * Function _mgp_1xTTOperationStops
 *
 * Description:
 *
 *  This function is indirectly called by 1x SRCH task to pass information
 *  that the Time Transfer operation will be temproarily stopped
 *  (i.e. 1x clocks and RTC could stop) after this message.
 *  This message is sent to PGI module.
 *
 * Parameters:
 *
 *  Information if 1x clocks/RTC would be interrupted after this command
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
extern void _mgp_1xTTOperationStops(const cgps_1xTTOpStopsDataType *p_1xTTOpStopsData);
extern void mgp_1xTTOperationStops(const cgps_1xTTOpStopsDataType *p_1xTTOpStopsData);

/*
 ******************************************************************************
 * Function _mgp_PpmState
 *
 * Description:
 *
 *  This function is called by PPM module to send message to PGI task about 
 *  its current state.
 *
 * Parameters:
 *
 *  Current PPM state
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
extern void _mgp_PpmState(const mgp_PpmStateDataType *p_PpmStateData);
extern void mgp_PpmState(const mgp_PpmStateDataType *p_PpmStateData);

/*
 ******************************************************************************
 * Function _mgp_1xSendRfInfo
 *
 * Description:
 *
 *  This function is indirectly called by 1x SRCH task to pass RF Info
 *  packet to PGI module.
 *
 * Parameters: 
 *
 *  RF Info structure.
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

extern void _mgp_1xSendRfInfo(const cgps_1xRfInfoType *);
extern void mgp_1xSendRfInfo(const cgps_1xRfInfoType *);

/*
 ******************************************************************************
 * Function _mgp_1xGPSAllowedAck
 *
 * Description:
 *
 *  This function is indirectly called by 1x SRCH task to inform PGI module
 *  that GPS searching can start now.
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
extern void _mgp_1xGPSAllowedAck(void);
extern void mgp_1xGPSAllowedAck(void);
#endif /* FEATURE_SRCH_CGPS_IF */


#ifdef FEATURE_CGPS_EVDO_IF
/*
 ******************************************************************************
 * Function _mgp_EVDOSendRfInfo
 *
 * Description:
 *
 *  This function is indirectly called by EVDO module to pass RF Info
 *  packet to PGI module.
 *
 * Parameters: 
 *
 *  RF Info structure.
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

extern void _mgp_EVDOSendRfInfo(const cgps_EVDORfInfoType *);
extern void mgp_EVDOSendRfInfo(const cgps_EVDORfInfoType *);
#endif /* FEATURE_CGPS_EVDO_IF */


/*
 ******************************************************************************
 * Function _mgp_RfInfoReq
 *
 * Description:
 *
 *  This function is called by TM module to request 1x or EVDO RF Info.
 *
 * Parameters: 
 *
 *  RF Info structure.
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
extern void _mgp_RfInfoReq(const gps_DataSourceType, const boolean);
extern void mgp_RfInfoReq(const gps_DataSourceType, const boolean);


/*
 ******************************************************************************
 * Function _mgp_TCXOManagerCB
 *
 * Description:
 *
 *  This function is indirectly called by TCXO Manager to inform PGI module
 *  that the current TCXO Manager restriction has changed.
 *
 * Parameters: 
 *
 *  New TCXO Manager restriction.
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

extern void _mgp_TCXOManagerCB(const cgps_TCXOMgrRestrictionDataType *);
extern void mgp_TCXOManagerCB(const cgps_TCXOMgrRestrictionDataType *);

#ifndef FEATURE_MC_SIMULATION
/*
 ******************************************************************************
 * Function _mgp_TCXOManagerRPushCB
 *
 * Description:
 *
 *  This function is indirectly called by TCXO Manager to send Frequency
 *  Estimate Update data to PGI module.
 *
 * Parameters: 
 *
 *  Frequency Estimate Update structure.
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

extern void _mgp_TCXOManagerRPushCB(const cgps_FreqEstimateDataType *);
extern void mgp_TCXOManagerRPushCB(const cgps_FreqEstimateDataType *);


/*
 ******************************************************************************
 * Function _mgp_TCXOManagerTempCB
 *
 * Description:
 *
 *
 * Parameters: 
 *
 *  Frequency Estimate Update structure.
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

void _mgp_TCXOManagerTempCB(const cgps_FreqEstimateTempDataType *p_FreqEstimateTempData);
void mgp_TCXOManagerTempCB(const cgps_FreqEstimateTempDataType *p_FreqEstimateTempData);

#endif /* FEATURE_MC_SIMULATION */

/*
 ******************************************************************************
 * Function _mgp_TRMLockGrantedCB
 *
 * Description:
 *
 *  This function is indirectly called by TRM module when TRM lock is granted
 *  to GPS.
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

extern void _mgp_TRMLockGrantedCB(uint32);
extern void mgp_TRMLockGrantedCB(uint32);


/*
 ******************************************************************************
 * Function _mgp_TRMUnlockCB
 *
 * Description:
 *
 *  This function is indirectly called by TRM when higher priority system
 *  needs TRM lock.
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

extern void _mgp_TRMUnlockCB(const cgps_TRMUnlockDataType *);
extern void mgp_TRMUnlockCB(const cgps_TRMUnlockDataType *);


/*
 ******************************************************************************
 * Function _mgp_TimeTickGet
 *
 * Description:
 *
 *  This function will return a 64 bit value by extended the 32
 *  bit counter incremented by the 32kHz Sleep Clock.
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
 *  A 64 bit timetick value
 *
 ******************************************************************************
*/

uint64 _mgp_TimeTickGet(void);
uint64 mgp_TimeTickGet(void);


/*
 ******************************************************************************
 * Function _mgp_TimeTickDiffSecs
 *
 * Description:
 *
 *  This function will take the difference of two 64 bit
 *  TimeTick values and return the value in Seconds.
 *
 *  diff_sec = y_EndTime - y_StartTime
 *
 * Parameters: 
 *
 *  y_StartTime - TimeTick Value
 *  y_EndTime - TimeTick Value
 *
 * Dependencies:
 *
 *  None
 *
 * Return value: 
 *
 *  A 64 bit timetick value
 *
 ******************************************************************************
*/

int64 _mgp_TimeTickDiffSecs(uint64 y_StartTime, uint64 y_EndTime);
int64 mgp_TimeTickDiffSecs(uint64 y_StartTime, uint64 y_EndTime);


/*
 ******************************************************************************
 * Function _mgp_TimeTickDiffMs
 *
 * Description:
 *
 *  This function will take the difference of two 64 bit
 *  TimeTick values and return the value in milli-seconds.
 *
 *  diff_msec = t_EndTime - t_StartTime
 *
 * Parameters: 
 *
 *  t_StartTime - TimeTick Value
 *  t_EndTime - TimeTick Value
 *
 * Dependencies:
 *
 *  None
 *
 * Return value: 
 *
 *  The time diff in seconds
 *
 ******************************************************************************
*/

int64 _mgp_TimeTickDiffMs(uint64 t_StartTime, uint64 t_EndTime);
int64 mgp_TimeTickDiffMs(uint64 t_StartTime, uint64 t_EndTime);


/*
 ******************************************************************************
 * Function _mgp_SlowClkExtTimePut
 *
 * Description:
 *
 *  This function accepts external time injections from sources such
 *  as the network (eg. SNTP) or the host (eg. a PC).
 *
 * Parameters: 
 *
 *  *p_InjectedTime - External time to be injected into MGP
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
void _mgp_SlowClkExtTimePut(const gps_SlowClkTimeInjectionType *p_InjectedTime);
void mgp_SlowClkExtTimePut(const gps_SlowClkTimeInjectionType *p_InjectedTime);


/*
 ******************************************************************************
 * Function _mgp_GnssSlowClkExtTimePut
 *
 * Description:
 *
 *  This function accepts external time injections from sources such
 *  as the network (eg. SNTP) or the host (eg. a PC). 
 *
 *  NOTE: This function can inject time in different GNSS time scale.
 *  
 *
 * Parameters: 
 *
 *  *p_InjectedTime - External time to be injected into MGP
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
void _mgp_GnssSlowClkExtTimePut(const gnss_SlowClkTimeInjectionType *p_InjectedGnssTime);
void mgp_GnssSlowClkExtTimePut(const gnss_SlowClkTimeInjectionType *p_InjectedGnssTime);


/*
 ******************************************************************************
 * Function _mgp_GetSlowClkTime
 *
 * Description:
 *
 *  This function accepts requests for Slow Clock.  The result is an
 *  asynchronous call to the call back pointed to by p_Cb.
 *
 * Parameters:
 *
 *  p_Cb - Pointer the to Call Back function to return the slow clock time to
 *         the requesting thread.
 *
 *  b_ReqReason - Slow Clock request reason.
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
void _mgp_GetSlowClkTime(gnss_SlowClkCbType p_Cb, gnss_SlowClkReqReasonEnumType e_ReqReason);
void mgp_GetSlowClkTime(gnss_SlowClkCbType p_Cb, gnss_SlowClkReqReasonEnumType e_ReqReason);


/*
 ******************************************************************************
 * Function _mgp_ExternalCoarsePosPut
 *
 * Description:
 *
 *  This function accepts external coarse injections from external sources.
 *
 * Parameters: 
 *
 *  *p_InjectedExternCoarsePos - External coarse position to be injected 
 *   into MGP
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
void _mgp_ExternalCoarsePosPut(const gnss_ExternCoarsePosInjectionType  *p_InjectedExternCoarsePos);
void mgp_ExternalCoarsePosPut(const gnss_ExternCoarsePosInjectionType  *p_InjectedExternCoarsePos);

/*
 ******************************************************************************
 * Function _mgp_DSPrefUpdate
 *
 * Description:
 *
 *  This function is called when Dual Standby preference is updated
 *  or the preferred data subscription is changed.
 *
 * Parameters: 
 *
 *  p_DSPref - Pointer to the DS Preference data.
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

void _mgp_DSPrefUpdate(const cgps_DSPrefType *const p_DSPref);
void mgp_DSPrefUpdate(const cgps_DSPrefType *const p_DSPref);

#ifdef FEATURE_CGPS_CELLDB
/*==============================================================================
  FUNCTION      _mgp_SSChange

  DESCRIPTION   
    Alert mgp of a change in serving cell type or in 1x cell information (SID,
    NID, bsid, lat, lon, search window size).
 
  DEPENDENCIES
    None

  PARAMETERS
    p_SSInfo - The type must be:
               CGPS_SSINFO_SRV_SYS - Contains the serving system type.
               CGPS_SSINFO_CELLINFO - Contains the complete 1x serving cell
                info.  If position is not valid, the lat and lon shall be set
                to 0.

  RETURN VALUE  
    None

  SIDE EFFECTS  
    None

==============================================================================*/
void _mgp_SSChange(const cgps_SSInfoType *const p_SSInfo);
void mgp_SSChange(const cgps_SSInfoType *const p_SSInfo);

/*
 ******************************************************************************
 * Function _mgp_InjectSparseNetworkRegion
 *
 * Description:
 *
 *  This function is used to inject region(s) into the sparse network database.
 *
 * Parameters: 
 *
 *  cpz_PackedRecord - List of packed Regions to add.
 *
 *  cq_NumRecords - The number of regions to add
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

void _mgp_InjectSparseNetworkRegion(
   const cgps_SndbInjectedRecordType *const cpz_PackedRecord,
   const uint32 cq_NumRecords);
void mgp_InjectSparseNetworkRegion(
   const cgps_SndbInjectedRecordType *const cpz_PackedRecord,
   const uint32 cq_NumRecords);
#endif /* FEATURE_CGPS_CELLDB */

/*
 ******************************************************************************
 * Function _mgp_DisableBitEdgeDetection
 *
 * Description:
 *
 *  This function will tell MGP to bypass Bit Edge Detection and jump into
 *  Track.  This is handy for tests using a GSS where Navigation data is turned
 *  off.
 *
 * Parameters: 
 *
 *  u_Disable - Non Zero will disable Bit Edge Detection
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
void _mgp_DisableBitEdgeDetection(uint8 u_Disable);
void mgp_DisableBitEdgeDetection(uint8 u_Disable);

/*
 ******************************************************************************
 * Function _mgp_SingleSvTrack
 *
 * Description:
 *
 *  This function initiates tracking on a Single SV. This is used for Standalone
 *  RF Verification tests to evaluate the Receiver performance.
 *  The can be called only after entering Standalone RF Verification (SARF) Mode.
 *
 * Parameters: 
 *
 *  u_SingleSVTrack - SV to be tracked. Non-zero=>Specfied SV will be tracked.
 *                                      '0' => All SVs will be tracked (test mode).
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
void _mgp_SingleSvTrack(uint8 u_SingleSVTrack);
void mgp_SingleSvTrack(uint8 u_SingleSVTrack);


/*
 ******************************************************************************
 * Function _mgp_GnssDisableBitEdgeDetection
 *
 * Description:
 *
 *  This function will tell MGP to bypass Bit Edge Detection and jump into
 *  Track.  This is handy for tests using a GSS where Navigation data is turned
 *  off.
 *
 * Parameters: 
 *  e_GnssType - GNSS system type
 *  u_CohPDI   - PDI length 10 or 20ms
 *  u_Disable - Non Zero will disable Bit Edge Detection
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

void _mgp_GnssDisableBitEdgeDetection(mgp_GnssType  e_GnssType, uint8 u_CohPDI, uint8 u_Disable);
void mgp_GnssDisableBitEdgeDetection(mgp_GnssType  e_GnssType, uint8 u_CohPDI, uint8 u_Disable);

/*
 ******************************************************************************
 * Function _mgp_BypassSvChecks
 *
 * Description:
 *
 *  This function will tell MGP to bypass Sv Checks and jump into
 *  Track.  This is handy for tests using a GSS or Power Measurement Tests,
 *  when you want to bypass the settings in the preloaded Alamanac. 
 *
 * Parameters: 
 *
 *  BypassMask - uint8 bit mask specifying which SV checks we want to bypass.
 *                    Bit 0 set = Bypass SV Steering
 *                    Bit 1 set = Bypass SV Direction
 *                    Bit 2 set = Bypass SV Alm Health
 *                    Bit 3 set = Bypass SV Eph Health
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
void _mgp_BypassSvChecks(uint8 u_BypassMask);
void mgp_BypassSvChecks(uint8 u_BypassMask);

/*
 ******************************************************************************
 * Function _mgp_GnssTrackMode
 *
 * Description:
 *
 *
 * Parameters: 
 *
 *  u_GnssTrackState - Track state for the GNSS system
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

void _mgp_GnssTrackMode(boolean u_GnssTrackState);
void mgp_GnssTrackMode(boolean u_GnssTrackState);



/*
 ******************************************************************************
 * Function _mgp_SetGpsIntelliceiver
 *
 * Description:
 *
 *  This function will set the GPS Intelliceiver mode.
 *
 * Parameters: 
 *
 *  u_Mode - Auto or High
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
void _mgp_SetGpsIntelliceiver(mgp_GpsIntelliceiverLinearityType u_Mode);
void mgp_SetGpsIntelliceiver(mgp_GpsIntelliceiverLinearityType u_Mode);

/*
 ******************************************************************************
 * Function _mgp_GpsFsTaskNotification
 *
 * Description:
 *
 *  This function notifies MGP to GPS FS 
 *
 * Parameters: 
 *
 *  pz_Msg - data for the notification
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
void _mgp_GpsFsTaskNotification(const mgp_GpsFsTaskNotificationDataType *pz_Msg);
void mgp_GpsFsTaskNotification(const mgp_GpsFsTaskNotificationDataType *pz_Msg);

/*****************************************************************************
 * Name     : me_PpLogProxySend
 * Description  :   PP requests CD to log grid at lower priority
 * Inputs     : *msg - define data to be logged
 * Outputs    : None
 * Returns    : Success/failure of message send
 *****************************************************************************
*/
boolean me_PpLogProxySend(pe_PpLogProxyType *p_Msg);

/*****************************************************************************
 * Name     : pe_PpLogProxySend
 * Description  :   PP requests CD to log grid at lower priority
 * Inputs     : *msg - define data to be logged
 * Outputs    : None
 * Returns    : Success/failure of message send
 *****************************************************************************
*/
boolean pe_PpLogProxySend(pe_PpLogProxyType *p_Msg);

/*****************************************************************************
 * Name     : pe_PpLogProxyReply
 * Description  :   CD reports completion of log
 * Inputs     : *msg - identifies data that was logged
 * Outputs    : None
 * Returns    : Success/failure of message send
 *****************************************************************************
*/
boolean pe_PpLogProxyReply(pe_PpLogProxyType *p_Msg);

/*****************************************************************************
 * Name     : me_PpLogProxyReply
 * Description  :   CD reports completion of log
 * Inputs     : p_Reply - structure contains pointers to data that was logged
 * Outputs    : None
 * Returns    : Success/failure of message send
 *****************************************************************************
*/
boolean me_PpLogProxyReply(pe_PpLogProxyType *p_Reply);

/*
 ******************************************************************************
 * Function _mgp_ReqGpsTime
 *
 * Description:
 *
 *  This function is called by LM to request the current estimate of GPS time 
 *  from MGP
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
void _mgp_ReqGpsTime(void);
void mgp_ReqGpsTime(void);

/*
 ******************************************************************************
 * Function _mgp_ReqGpsTime
 *
 * Description:
 *
 *  This function is called by LM to request the current estimate of GPS time 
 *  from MGP
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
void _mgp_GnssReqGpsTime(void);
void mgp_GnssReqGpsTime(void);

/*
 ******************************************************************************
 * Function _mgp_InjectPosEstimate
 *
 * Description:
 *
 *  This function injects a position into the Position Manager
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
void _mgp_InjectPosEstimate(const mgp_CellDBPosInfoStruct *const p_PosEst, boolean u_Force);
void mgp_InjectPosEstimate(const mgp_CellDBPosInfoStruct *const p_PosEst, boolean u_Force);


/*
 ******************************************************************************
 * Function _mgp_InjectSbasUserPreference
 *
 * Description:
 *
 *  This function injects the change in SBAS user preference to the CD task
 *
 * Parameters: 
 *
 *  u_SbasUserPreference
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
void _mgp_InjectSbasUserPreference(const boolean u_SbasUserPreference);
void mgp_InjectSbasUserPreference(const boolean u_SbasUserPreference);

/*
 ******************************************************************************
 * Function _mgp_ForceDpoOperation
 *
 * Description:
 *
 *  This function will tell MGP to Force DPO operation.
 *
 * Parameters: 
 *
 *  u_Force - when TRUE, DPO is forced
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
void _mgp_ForceDpoOperation(uint8 u_Force);
void mgp_ForceDpoOperation(uint8 u_Force);

/*
 ******************************************************************************
 * Function _mgp_ForceDpoForPowerMeas
 *
 * Description:
 *
 *  This function will tell MGP to Force DPO operation specific to Power Measurements
 *  DPO Operation is forced after entering DPO for the first time.
 *
 * Parameters: 
 *
 *  u_ForceDpoForPowerMeas - when TRUE, DPO is forced after first entry into DPO
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
void _mgp_ForceDpoForPowerMeas(uint8 u_ForceDpoForPowerMeas);
void mgp_ForceDpoForPowerMeas(uint8 u_ForceDpoForPowerMeas);

/*
 ******************************************************************************
 * Function _mgp_UpdateDpoControl
 *
 * Description:
 *
 *  This function sends the DPO Control to MGP.
 *
 * Parameters: 
 *
 *  u_DpoControl - DPO is enabled when this is non-zero 
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
void _mgp_UpdateDpoControl(uint8 u_DpoControl);
void mgp_UpdateDpoControl(uint8 u_DpoControl);

/*
 ******************************************************************************
 * Function _mgp_UpdateMgpPpmRfControl
 *
 * Description:
 *
 *  This function specifies the state of the RF during periodic PPM operation.
 *
 * Parameters: 
 *
 *  u_MgpPpmRfControl - TRUE if RF needs to be enabled. 
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
void _mgp_UpdateMgpPpmRfControl(boolean u_MgpPpmRfControl);
void mgp_UpdateMgpPpmRfControl(boolean u_MgpPpmRfControl);

/*
 ******************************************************************************
 * Function _mgp_GetDpoControlStatus
 *
 * Description:
 *
 *  This function obtains the current status of DPO control.
 *
 * Parameters: 
 *
 *  p_DpoControl - Pointer to store the DpoControlStatus 
 *  q_ThreadId   - Thread calling the API (only GPS threads are allowed).
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
void _mgp_GetDpoControlStatus(uint8 *p_DpoControl, os_ThreadIdEnumType q_ThreadId);
void mgp_GetDpoControlStatus(uint8 *p_DpoControl, os_ThreadIdEnumType q_ThreadId);



/*
 ******************************************************************************
 * Function _mgp_SetSbasCannedSvList
 *
 * Description:
 *
 *  This function will set the MGP's canned SBAS list.
 *
 * Parameters:
 *
 *  t_SvMask - SV mask
 *             Bit 0 = SV 120
 *             Bit 1 = SV 121
 *             ...
 *             Bit 18 = SV 138
 *             Bits 19..63 - reserved
 *
 *             Bit value "1" - SV is part of the canned list
 *             Bit value "0" - SV is not part of the canned list
 *
 *             E.g. t_SvMask = 0x04_00_01 means that
 *             the canned list has SVs {120, 138}
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
void _mgp_SetSbasCannedSvList( uint64 t_SvMask );
void mgp_SetSbasCannedSvList( uint64 t_SvMask );

/*
******************************************************************************
* Function _mgp_GetPosPuncTunc
*
* Description:
*
*  This function gets the current position and its associated PUNC and TUNC
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
void _mgp_GetPosPuncTunc(void);
void mgp_GetPosPuncTunc(void);

/*
 ******************************************************************************
 * Function _mgp_MinGpsWeekNumberGet
 *
 * Description:
 *
 *  Retrieves the Minimum GPS Week Number that is stored in the NV database
 *
 * Parameters: 
 *
 *   Pointer to output variable
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
void _mgp_MinGpsWeekNumberGet(uint16 *p_MinGpsWeekNumber);
void mgp_MinGpsWeekNumberGet(uint16 *p_MinGpsWeekNumber);

/*
 ******************************************************************************
 * Function _mgp_DpoNvControlGet
 *
 * Description:
 *
 *  Retrieves the DPO Control value from NV memory
 *
 * Parameters: 
 *
 *   e_ThreadId - calling Thread ID
 *
 * Dependencies:
 *
 *  None
 *
 * Return value: 
 *
 *  Value of DPO NV control
 *
 ******************************************************************************
*/
uint8 _mgp_DpoNvControlGet( os_ThreadIdEnumType e_ThreadId );
uint8 mgp_DpoNvControlGet( os_ThreadIdEnumType e_ThreadId );

/*
 ******************************************************************************
 * Function mgp_GetGPSStatusInfo
 *
 * Description:
 *
 *  This function gets the GPS state info
 *
 * Parameters: 
 *   b_Type - Type of request, can be ODP request, Diag Pos req or GPS state info req
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
void _mgp_GetGPSStateInfo(uint8 b_Type);
void mgp_GetGPSStateInfo(uint8 b_Type);


#ifdef FEATURE_UIM_SUPPORT_LBS
/*
 ******************************************************************************
 * Function _mgp_SeedPosOptionInfoUpdate
 *
 * Description:
 *
 *
 * Parameters: 
 *
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

void _mgp_SeedPosOptionInfoUpdate(const uint16 w_CellDBPosOption);
void mgp_SeedPosOptionInfoUpdate(const uint16 w_CellDBPosOption);


/*
 ******************************************************************************
 * Function _mgp_RuimXtraInit
 *
 * Description:
 *
 *
 * Parameters: 
 *
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

void _mgp_RuimXtraInit(void);
void mgp_RuimXtraInit(void);
#endif /* FEATURE_UIM_SUPPORT_LBS */

/*============================================================================
FUNCTION - _mgp_RfBPStartInit

DESCRIPTION
    Function for rf sw to call which enables clocks needed for all BP's 
    and call BP init for each BP. 

    Note: This is called at boot up in TMC ask context when 
          other tasks are not yet started.

    This function is used by Rf SW to callibrate items related to 
    preMean and preAmpl at boot up.

RETURN VALUE

DEPENDENCIES

LIMITATIONS

=============================================================================*/
void _mgp_RfBPStartInit(gnss_RfModeEnum gnss_rf_mode_selected);
void mgp_RfBPStartInit(gnss_RfModeEnum gnss_rf_mode_selected);

/*============================================================================
FUNCTION - _mgp_RfBPStopInit

DESCRIPTION
    Function for rf sw to call which turns of clocks needed for all BPs. 

    Note: This is called at boot up in TMC ask context when 
          other tasks are not yet started.

    This function stops all the clocks started in gnss_RfBPStartInit 
    effectively disabling all BPs

RETURN VALUE

DEPENDENCIES

LIMITATIONS

=============================================================================*/
void _mgp_RfBPStopInit(void);
void mgp_RfBPStopInit(void);

/*
 ******************************************************************************
 * Function _mgp_GetCellData
 *
 * Description:
 *
 *  This function queues a get cell data message to the ME.
 *
 * Parameters: 
 *
 *  mgp_GetCellDataParm - Used to select the cells the ME reports back.
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value: 
 *
 *  TRUE if the message could be sent successfully to ME.
 *
 ******************************************************************************
*/
typedef enum {
  C_MGP_XT_GET_ALL = 0,               /* get all cells in CellDB */
  C_MGP_XT_GET_ALL_CHANGED,           /* get all changed cells in CellDB */
  C_MGP_XT_GET_CHANGED_IN_N_MINUTES,  /* get all changed in last N minutes */
  C_MGP_XT_GET_LAST_N_CHANGED_CELLS,  /* get last N changed cells */
  C_MGP_XT_GET_MAX,                   /* max valid selection */
  C_MGP_XT_GET_E_SIZE = 0x10000000    /* force to 32 bits size */
} mgp_GetCellDataEnumType;

typedef struct {
  mgp_GetCellDataEnumType   e_GetDataSelect;  /* select cell data to get */
  uint32                    q_Value;          /* use for passing N (if needed) */
} mgp_GetCellDataParm;

boolean _mgp_GetCellData(const mgp_GetCellDataParm *const p_GetCellDataParm);

/*
 ******************************************************************************
 * Function _mgp_AckCellData
 *
 * Description:
 *
 *  This function queues an acknowledge cell data message to the ME.
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
 *  TRUE if the message could be sent successfully to ME.
 *
 ******************************************************************************
*/
boolean _mgp_AckCellData(void);
boolean mgp_AckCellData(void);

/*
 ******************************************************************************
 * Function _mgp_AbortCellUpload
 *
 * Description:
 *
 *  This function queues an abort cell upload message to the ME.
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
 *  TRUE if the message could be sent successfully to ME.
 *
 ******************************************************************************
*/
boolean _mgp_AbortCellUpload(void);
boolean mgp_AbortCellUpload(void);

#ifdef FEATURE_GPS_RESTRICTION
#error code not present
#endif /* ifdef FEATURE_GPS_RESTRICTION */


/*
 ******************************************************************************
 * Function _mgp_ReportSensorData
 *
 * Description:
 *
 *  This function queues a report sensor data message to the PE.
 *
 * Parameters: 
 *
 *  mgp_ReportSensorDataParm - Used to hold the sensor data to report.
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

typedef enum {
  C_MGP_SNS_INVALID                 = 0x0000000,  /* invalid data */

  /* Sensor Streaming Data */
  C_MGP_SNS_DATA_MOTION_LOW_POWER   = 0x0000001,  /* low power stationary pos indicator */
  C_MGP_SNS_DATA_VEL_1HZ_1SEC,                    /* velocity process noise 1Hz operating interval 1s */
  C_MGP_SNS_DATA_JERK_1HZ_1SEC,                   /* jerk variance 1Hz operating interval 1s */
  C_MGP_SNS_DATA_MOTION_FULL_POWER,               /* full power stationary pos indicator */
  C_MGP_SNS_DATA_VEL_1HZ_100MS,                   /* velocity process noise 1Hz operating interval 100ms */
  C_MGP_SNS_DATA_JERK_1HZ_100MS,                  /* jerk variance 1Hz operating interval 100ms */
  C_MGP_SNS_DATA_MAX_RANGE,                       /* max range for Sensor Streaming Data messages */

  /* Sensor Control Data */
  C_MGP_SNS_CTRL_STARTUP            = 0x00000100, /* sensor handler is starting up */
  C_MGP_SNS_CTRL_SHUTDOWN,                        /* sensor handler is shutting down */
  C_MGP_SNS_CTRL_AVAILABLE,                       /* sensor data streams now available */
  C_MGP_SNS_CTRL_UNAVAILABLE,                     /* sensor data streams now unavailable */
  C_MGP_SNS_CTRL_MAX_RANGE,                       /* max range for Sensor Control Data messages */

  C_MGP_EXTERNAL_DATA_MOTION            = 0x00000200,
  C_MGP_EXTERNAL_DATA_MOTION_MAX_RANGE,

  C_MGP_SNS_MAX_RANGE               = 0x10000000  /* not used (force enum to 4 bytes) */

} mgp_ReportSensorDataType;

/* 
   This enumeration is defined in a MobiSens header file, however that header
   file is currently private.  It's really buggy to redefine it here, but doing
   it properly would require a new MobiSens release, which we don't have the
   time for right now.  I've made Ashish and Ricky aware of this, and it is
   on their to-do list for the next MobiSens release.
   M. Bevilacqua - 08/04/2009
 */
typedef enum {
  C_MGP_SNS_HP_SPI_000          = 0, /* don't know */
  C_MGP_SNS_HP_SPI_001          = 1, /* don't know */
  C_MGP_SNS_HP_SPI_010          = 2, /* don't know */
  C_MGP_SNS_HP_SPI_011          = 3, /* don't know */
  C_MGP_SNS_HP_SPI_MAYBE_STATIC = 4, /* high-power SPI indicates maybe-stationary */
  C_MGP_SNS_HP_SPI_101          = 5, /* reserved */
  C_MGP_SNS_HP_SPI_110          = 6, /* reserved */
  C_MGP_SNS_HP_SPI_KNOWN_STATIC = 7  /* high-power SPI indicates stationary */
} mgp_SensorFullPowerSPIState;

typedef enum {
  C_MGP_EXTERNAL_DATA_MOTION_UNKNOWN = 0,
  C_MGP_EXTERNAL_DATA_MOTION_NON_STATIONARY = 4,
  C_MGP_EXTERNAL_DATA_MOTION_STATIONARY = 7,
  C_MGP_EXTERNAL_DATA_MOTION_MAX = 0x7FFFFFFF /* reserved. force enum to at least 4 bytes */
} mgp_ExternalDataMotionType;

#define MGP_EXTERNAL_DATA_MOTION_CONFIDENCE_UNKNOWN  (0xff)


typedef struct {
  uint64                    t_TimeTickMsec;       /* 64-bit extended slow clock at report generation */
  mgp_ReportSensorDataType  e_SensorDataType;     /* select sensor data type     */
  uint32                    q_Value;              /* use for passing sensor data */
  uint8                     u_Confidence;
} mgp_ReportSensorDataParm;

void _mgp_ReportSensorData(const mgp_ReportSensorDataParm *p_ReportSensorDataParm);
void mgp_ReportSensorData(const mgp_ReportSensorDataParm *p_ReportSensorDataParm);

/* ============================================================================
enum type to set timer for periodical fix and to configure time stamp in a gps
position report
                                              
                                                     Periodical fix timer
                                                      On            Off
alignment of stamp to gps whole sec:  Not required    N/S (0)       S(1)
                                      Except first     S (2)       N/S (3)
                                      Always           S (4)       N/S (5)

                                      S = Supported
                                      N/S = Not Supported
=============================================================================*/
typedef enum
{
  C_MGP_FIX_TIMER_OFF_ALIGN_NOT_REQ = 1,  /* Turn off periodical fix timer in PE.
                                          Pos time stamped with meas blk time.
                                          N/A to TBF-n */
  C_MGP_FIX_TIMER_ON_ALIGN_EXP_FIRST = 2, /* Turn on periodcal fix timer in PE,
                                          expedite first fix and then align
                                          following fix to gps whole secs 
                                          For TBF, this appliest to TBF=1~4*/
  C_MGP_FIX_TIMER_ON_ALIGN_ALWAYS = 4,    /* Turn on periodical fix timer in PE,
                                          align all fixes to gps whole secs.
                                          For TBF, this applies to TBF>=5 */
  C_MGP_FIX_TIMER_MAX = 0x7FFFFFFF /* reserved. force enum to at least 4 bytes */
} mgp_PeriodicFixAndAlignEnumType;

/**
 * @brief Configures periodic fix time and alignment
 * 
 * This function configures the PE periodical fix time and alignment of
 * position time stamp
 *
 * @param[in] e_Config Used to set the timer and alignment.
 * @param[in] q_Tbf_ms Time between fixes for timer alignment.
 *
 * @retval TRUE  If the message could be sent successfully to PE.
 * @retval FALSE On error
 */
boolean _mgp_ConfigPeriodicFixAndAlignment
(
  mgp_PeriodicFixAndAlignEnumType e_Config,
  uint32                          q_Tbf_ms
);
boolean mgp_ConfigPeriodicFixAndAlignment
(
  mgp_PeriodicFixAndAlignEnumType e_Config,
  uint32                          q_Tbf_ms
);


/*
 ******************************************************************************
 * Function _mgp_ReportSmEvent
 *
 * Description:
 *
 *  This function sends various events from Session Manager to MGP. MGP makes use
 *  of these events to maintain the proper state of GPS engine.
 *
 * Parameters: 
 *
 *  mgp_SmEventData * - SM event to be forwarded to MGP
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value: 
 *
 *  TRUE if the message could be sent successfully to MC.
 *
 ******************************************************************************
*/

typedef enum
{
  C_MGP_FIX_START_IND,
  C_MGP_FIX_END_IND,
  C_MGP_SM_EVENT_MAX_RANGE      = 0x10000000  /* not used (force enum to 4 bytes) */
} mgp_SmEventType;

typedef struct
{
  uint32 q_Tbf;    /* Time Between Fixes for the new session*/
  uint32 q_RcvrOnThresholdTime; /* NV item AAGPS_APP_TRACK_RC_ON_THRESHOLD_PERIOD */
  uint32 q_RcvrIdleThresholdTime; /* NV item AAGPS_APP_TRACKING_RC_IDLE_THRESHOLD */
} mgp_SmEventFixStartData;

typedef struct
{
  mgp_SmEventType e_SmEvent;
  union /*Make this a union for future API extension*/
  {
    mgp_SmEventFixStartData z_FixStartData; /*Payload assocated with C_MGP_FIX_START_IND event*/
  } z_SmEventpayload;
} mgp_SmEventData;

void _mgp_ReportSmEvent(mgp_SmEventData *p_SmEvent);
void mgp_ReportSmEvent(mgp_SmEventData *p_SmEvent);

#ifdef FEATURE_GNSS_LTE_CATM_COEX
typedef struct 
{
  uint32                q_TimeTickMs;  /* Time of next scheduled LTE Tx start, in MPSS time ticks in ms*/
  uint32                q_Duration;    /* Duration of the next scheduled LTE Tx in 1ms */
  sys_sband_lte_e_type  q_Band;        /* LTE Tx Band information */
  lte_bandwidth_e       q_Bandwidth;   /* Bandwidth of Tx in kHz */
  uint32                q_Power;       /* Power of the concerned LTE Tx in dBm */
  uint16                w_SeqCnt;      /* Sequence number of the message sent. */ 	
} mgp_CatmLteTxStartTestParams;

typedef struct 
{
  uint32  q_TimeTickMs;  /* Time of next scheduled LTE Tx start, in MPSS time ticks in ms */
  uint16  w_SeqCnt;      /* Sequence number of the message sent. */
} mgp_CatmLteTxEndTestParams;

#endif /* FEATURE_GNSS_LTE_CATM_COEX */

/******************************************************************************
 * _mgp_OemDreSvPolyDataRequest
 *
 * Function description:
 *
 *  Enable/Disable (MGP/PE/CD) sv poly report to SM to support OEM DR enabling.
 *
 * Parameters:   b_IsOn
 *               TRUE: turn on report
 *               FALSE: turn off report
 *
 * Return value: None
 *
 ******************************************************************************
*/
void _mgp_OemDreSvPolyDataRequest(boolean b_IsOn);
void mgp_OemDreSvPolyDataRequest(boolean b_IsOn);

/*
 ******************************************************************************
 * Function _mgp_GetDRSyncConfig
 *    
 * Description:
 *  Retrieve the DR Sync Configuration.
 *
 * Parameters: 
 *
 * Dependencies:
 *  None
 *
 * Return value: 
 *  boolean (IPC was sent with success or not)
 ******************************************************************************
*/
boolean _mgp_GetDRSyncConfig(void);
boolean mgp_GetDRSyncConfig(void);

/*
 ******************************************************************************
 * Function _mgp_SetDRSyncConfig
 *
 * Description:
 *  Set the DR Sync Configuration.
 *
 * Parameters: 
 *
 * Dependencies:
 *  None
 *
 * Return value: 
 *  boolean (IPC was sent with success or not)
 ******************************************************************************
 */
boolean _mgp_SetDRSyncConfig(const gnss_DRSyncConfigType *p_Config);
boolean mgp_SetDRSyncConfig(const gnss_DRSyncConfigType *p_Config);

/*
 ******************************************************************************
 * Function _mgp_ArmDRSync
 *
 * Description:
 *  Arm the DR Sync.
 *
 * Parameters: 
 *
 * Dependencies:
 *  None
 *
 * Return value: 
 *  boolean (IPC was sent with success or not)
 ******************************************************************************
 */

boolean _mgp_ArmDRSync(void);
boolean mgp_ArmDRSync(void);


/*
 ******************************************************************************
 * Function _mgp_SetLowErrEst
 *
 * Description:
 *
 *  This function sets the aggressively low error estimate in the low 
 *  measurement count scenario via the LocMW interface.
 *
 * Parameters: 
 *
 *  q_LowErrEstCfg - Used to set the low error estimate 
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value: 
 *
 *  boolean - TRUE if the setting is successfully queued. FALSE otherwise.
 *
 ******************************************************************************
*/
boolean _mgp_SetLowErrEst(const uint32 q_LowErrEstCfg);
boolean mgp_SetLowErrEst(const uint32 q_LowErrEstCfg);


#ifdef FEATURE_CGPS_CELLDB
/*
 ******************************************************************************
 * _mgp_InjectCellDbPosEstimate
 *
 * Function description:
 *
 *  This function is used by ME to inject a position estimate from the 
 *  Cell Database. 
 *    
 * Parameters: 
 *
 *  void
 *
 * Return value: 
 *
 *  void
 *
 * ******************************************************************************
*/
void _mgp_InjectCellDbPosEstimate(const mgp_PosEstInfoStruct *p_PosEstInfo);
void mgp_InjectCellDbPosEstimate(const mgp_PosEstInfoStruct *p_PosEstInfo);

/*
 ******************************************************************************
 * _mgp_RequestPosEstimate
 *
 * Function description:
 *
 *  This function is used by TLE to request a position report asynchronously 
 *    
 * Parameters: 
 *
 *  void
 *
 * Return value: 
 *
 *  void
 *
 * ******************************************************************************
*/
void _mgp_RequestPosEstimate(void);
void mgp_RequestPosEstimate(void);
#endif /* FEATURE_CGPS_CELLDB */

/* -------------------------------------------------------------------------
* _mgp_RfWwanStatus
*
* Description: 
*  This function provides an interface for RF drivers to notify the
*  GPS engine the status of the current WWAN technology.  This should be
*  called prior to the RF tuning to the band/channel.
 *
 * Parameters:
*  e_CurWwan - Current active WWAN Technology
*  e_Band - Use to set band when e_CurWwan is not GNSS_WWAN_STATE_WCDMA. 
*           In this case, e_WCDMABand will be invalid.
*  e_WCDMABand - Use to set band when e_CurWwan is GNSS_WWAN_STATE_WCDMA.
*                In this case, e_Band will be invalid.
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
void _mgp_RfWwanStatus
(
   gnss_WwanStateEnum       e_CurWwan,
   gnss_RfCardBandEnum      e_Band,
   gnss_RfcomWcdmaBandEnum  e_WCDMABand,
   uint32                   q_Channel
   );
void mgp_RfWwanStatus
(
   gnss_WwanStateEnum       e_CurWwan,
   gnss_RfCardBandEnum      e_Band,
   gnss_RfcomWcdmaBandEnum  e_WCDMABand,
   uint32                   q_Channel
   );


/* -------------------------------------------------------------------------
* _mgp_ExternalWLanStatus
*
* Description: 
*  This function provides an interface for GNSS to be notified of the current
*  WLan (WiFi) status.   This should be called prior to the WLan tuning to the
*  new band/channel.
*
* Parameters:
*  e_BlankStatus - See gnss_WlanBlankEnum
*  q_Channel - WLan Channel - BitMask of the channels Valid bits 0 to 13
*                             {0x00000000 to 0x00003FFF}
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
void _mgp_ExternalWLanStatus
(
   gnss_WlanBlankEnum       e_BlankStatus,
   uint32                   q_Channel
   );
void mgp_ExternalWLanStatus
(
   gnss_WlanBlankEnum       e_BlankStatus,
   uint32                   q_Channel
   );


/* -------------------------------------------------------------------------
* _mgp_WLanAttachStatusPut
*
* Description: 
*  This function provides an interface for MGP to be notified of the current
*  WLan (WiFi) attachment status. This should be called prior to the WLAN tuning
*  to the new band/channel.
*
* Parameters:
*  b_isWlanAttached - WLAN Attach status
*                       TRUE  - WLAN attached
*                       FALSE - WLAN detached
*  e_WlanBand       - The WLAN band to which the device is attached.
*                          Valid Values: 0 - Unknown
*                                        1 - 2.4GHz
*                                        2 - 5GHz
*                                        3 - 3.6GHz
*                                        4 - 4.9GHz(Public Safety band)
*  u_WlanChannelNum - The WLAN channel number to which the device is attached.
*                       Valid Range: 1 - 14 for 2.4 Ghz band
*                                    1 - 196 for 5 GHz band
*                       Default: 0xFF, when unknown or WLAN is detached
*  u_WlanChannelBw  - The bandwidth of the WLAN Channel in Mhz.
*                     Valid Range:  5-40 MHz
*
* Return value: 
*  0x1 - Success; Parameters were accepted for processing.
*  0x2 - Parameters are invalid error
*  0x3 - Regular attach command in Simulation mode error
* -----------------------------------------------------------------------*/
gnss_WlanImdRetCodeEnum _mgp_WLanAttachStatusPut
(
   boolean  b_isWlanAttached,
   gnss_WlanBandEnumType e_WlanBand,
   uint8                 u_WlanChannelNum,
   uint8                 u_WlanChannelBw
   );
gnss_WlanImdRetCodeEnum mgp_WLanAttachStatusPut
(
   boolean  b_isWlanAttached,
   gnss_WlanBandEnumType e_WlanBand,
   uint8                 u_WlanChannelNum,
   uint8                 u_WlanChannelBw
   );


/* -------------------------------------------------------------------------
* mgp_WLanAttachSimTxPut
*
* Description: 
*  This function provides an interface for the GNSS MGP to be notified of the 
*  current WLan (WiFi) attachment status and the status of WLan Tx simulation.
*  This should be called prior to the WLAN tuning to the new band/channel.
*
* Parameters:
*  b_isWlanAttached - WLAN Attach status
*                       TRUE  - WLAN attached
*                       FALSE - WLAN detached
*  e_WlanBand       - The WLAN band to which the device is attached.
*                          Valid Values: 0 - Unknown
*                                        1 - 2.4GHz
*                                        2 - 5GHz
*                                        3 - 3.6GHz
*                                        4 - 4.9GHz(Public Safety band)
*  u_WlanChannelNum - The WLAN channel number to which the device is attached.
*                       Valid Range: 1 - 14 for 2.4 Ghz band
*                                    1 - 196 for 5 GHz band
*                       Default: 0xFF, when unknown or WLAN is detached
*  u_WlanChannelBw  - The bandwidth of the WLAN Channel in Mhz.
*                     Valid Range:  5-40 MHz
*  b_WlanTxOn       - Simulate Wlan Tx status
*                       TRUE  - WLAN is transmitting
*                       FALSE - WLAN is not transmitting
*
* Return value: 
*  0x1 - Success; Parameters were accepted for processing.
*  0x2 - Parameters are invalid error
*  0x3 - Regular attach command in Simulation mode error
* -----------------------------------------------------------------------*/
gnss_WlanImdRetCodeEnum _mgp_WlanAttachSimTxPut
(
   boolean  b_isWlanAttached,
   gnss_WlanBandEnumType    e_WlanBand,
   uint8                    u_WlanChannelNum,
   uint8                    u_WlanChannelBw,
   boolean  b_WlanTxOn
   );
gnss_WlanImdRetCodeEnum mgp_WlanAttachSimTxPut
(
   boolean  b_isWlanAttached,
   gnss_WlanBandEnumType    e_WlanBand,
   uint8                    u_WlanChannelNum,
   uint8                    u_WlanChannelBw,
   boolean  b_WlanTxOn
   );


/* -------------------------------------------------------------------------
*
*  _mgp_SimulateImd
*
* Description: 
*   This functions provides an MGP interface to simulate an IMD Jamming scenario.
*
* Parameters:
*  p_SimulateImdParams - Pointer to a structure that has configuration 
*                         parameters needed for IMD simulation.
*
* Return value: 
*  TRUE if parameters were accepted for processing, FALSE otherwise
* -----------------------------------------------------------------------*/
boolean _mgp_SimulateImd
(
   gnss_SimulateImdType *p_SimulateImdParams
   );
boolean mgp_SimulateImd
(
   gnss_SimulateImdType *p_SimulateImdParams
   );



/* -------------------------------------------------------------------------
* mgp_BluetoothImdJammingStatus
*
* Description: 
*  This function provides an interface for GNSS to be notified of the current
*  Bluetooth IMD Jamming status.   This should be called prior to Bluetooth
*  being turned ON/OFF.
*
* Parameters:
*  e_ImdJammingStatus -  see gnss_BluetoothImdEnum
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
void _mgp_BtImdJammingStatus(gnss_BluetoothImdEnum e_BtImdJammingStatus);
void mgp_BtImdJammingStatus(gnss_BluetoothImdEnum e_BtImdJammingStatus);


/* -------------------------------------------------------------------------
* _mgp_MgpCcReset
*
* Description: 
*  This function provides an interface for pausing and resuming GPS. The 
*  lowest layer in the GPS stack (Chan Controller) and the layer above it 
*  (meas controller) are paused and resumed. Upon returning from this function
*  (after several ms) CC and MC are guaranteed to be in pause state. A few ms
*  later, the resume will automatically trigger. CC flushes the data mover
*  during the pause.
*
* Parameters:
*  None
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
void _mgp_MgpCcReset(void);
void mgp_MgpCcReset(void);

/*
 ******************************************************************************
 * Function _mgp_SingleSvTrack
 *
 * Description:
 *
 *  This function initiates tracking on a Single SV. This is used for Standalone
 *  RF Verification tests to evaluate the Receiver performance.
 *  The can be called only after entering Standalone RF Verification (SARF) Mode.
 *
 * Parameters: 
 *
 *  u_SingleSVTrack - SV to be tracked. Non-zero=>Specfied SV will be tracked.
 *                                      '0' => All SVs will be tracked (test mode).
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
void _mgp_SingleSvTrack(uint8 u_SingleSVTrack);
void mgp_SingleSvTrack(uint8 u_SingleSVTrack);

/*
 ******************************************************************************
 * Function mgp_InitGetTickTimeMutex
*
* Description: 
 *
 *  This function initiates Mutex which is used to get TimeTick.
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
 *  TRUE if it initilized Mutex Sucessfully, Otherwise FALSE
 *
 ******************************************************************************
*/
boolean mgp_InitTimeTickGetMutex(void);


/* -------------------------------------------------------------------------
* _mgp_RfBpGloGroupDelayPut
*
* Description: 
*  This function provides an interface for GNSS to be provided
*  the group delay compensation info for all GLO channels
*
* Parameters:
*  p_GrpDelay - Grp Delay for all Glonass channels
*                   0th index is for Freq -7
*                   13th index is for Freq 6
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
void _mgp_RfBpGloGroupDelayPut
(
   gnss_GloGrpDelayType *p_GrpDelay
   );
void mgp_RfBpGloGroupDelayPut
(
   gnss_GloGrpDelayType *p_GrpDelay
   );

/*
 ******************************************************************************
 * Function _mgp_ForceXoCoreOn
 *
 * Description:
 *
 *  This function queues a XO Core ON request to MC PGI.
 *
 * Parameters: 
 *
 * Return value: 
 *
 *  TRUE  - IPC send successful
 *  FALSE - IPC send error
 *
 ******************************************************************************
*/
boolean _mgp_ForceXoCoreOn(void);
boolean mgp_ForceXoCoreOn(void);

/*=============================================================================

  FUNCTION      _mgp_SdpInjectLsmpPremiumServiceMsg()

  DESCRIPTION   Interface for SDP to send premium service msg, that it 
                received from SM, to NF

  DEPENDENCIES  None

  PARAMETERS    pz_LsmpPremiumServiceMsg: Pointer to LSMP defined premium
                service message

  RETURN VALUE  TRUE: premium service message injection was successful
                FALSE: premium service message injection was unsuccessful

  SIDE EFFECTS  None

=============================================================================*/
boolean _mgp_SdpInjectLsmpPremiumServiceMsg(const lsmp_premium_services_param_type *pz_LsmpPremiumServiceMsg);
boolean mgp_SdpInjectLsmpPremiumServiceMsg(const lsmp_premium_services_param_type *pz_LsmpPremiumServiceMsg);

/* -------------------------------------------------------------------------
* _mgp_SdpInjectPedometerData
*
* Description: 
*  This function provides an async interface for SDP to send pedometer data
*  to PE for processing
*
* Parameters:
*  pz_Ind - pointer to pedometer structure 
*             
*
*
* Return value: TRUE if the message was sent to PE, FALSE otherwise
* 
* -----------------------------------------------------------------------*/
boolean _mgp_SdpInjectPedometerData(const slimInjectPedometerDataIndMsgT *pz_Ind);
boolean mgp_SdpInjectPedometerData(const slimInjectPedometerDataIndMsgT *pz_Ind);

/* -------------------------------------------------------------------------
* _mgp_SdpValidatePedometerData
*
* Description: 
*  Evaluate validity of pedometer data for processing in NF
*
* Parameters:
*  pz_PedometerData - pointer to pedometer structure 
*             
* Return value: TRUE if valid, FALSE otherwise
* 
* -----------------------------------------------------------------------*/
boolean _mgp_SdpValidatePedometerData(
   const slimPedometerDataStructT *pz_PedometerData);
boolean mgp_SdpValidatePedometerData(
   const slimPedometerDataStructT *pz_PedometerData);

/* -------------------------------------------------------------------------
* _mgp_SdpInjectPedestrianAlignmentData
*
* Description: 
*  This function provides an async interface for SDP to send pedestrian
*  alignment data to PE for processing
*
* Parameters:
*  pz_Ind - pointer to pedestrian alignment structure 
*
* Return value: TRUE if the message was sent to PE, FALSE otherwise
* 
* -----------------------------------------------------------------------*/
boolean _mgp_SdpInjectPedestrianAlignmentData(
   const slimInjectPedestrianAlignmentDataIndMsgT *pz_Ind);
boolean mgp_SdpInjectPedestrianAlignmentData(
   const slimInjectPedestrianAlignmentDataIndMsgT *pz_Ind);

/* -------------------------------------------------------------------------
* _mgp_SdpValidatePedestrianAlignmentData
*
* Description: 
*  Evaluate validity of pedestrian alignment data for processing in NF
*
* Parameters:
*  pz_PedestrianAlignmentData - pointer to pedestrian alignment structure 
*             
* Return value: TRUE if valid, FALSE otherwise
* 
* -----------------------------------------------------------------------*/
boolean _mgp_SdpValidatePedestrianAlignmentData(
   const slimPedestrianAlignmentDataStructT *pz_PedestrianAlignmentData);
boolean mgp_SdpValidatePedestrianAlignmentData(
   const slimPedestrianAlignmentDataStructT *pz_PedestrianAlignmentData);

/* -------------------------------------------------------------------------
* _mgp_SdpInjectMotionData
*
* Description: 
*  This function provides an async interface for SDP to send motion data
*  to PE for processing
*
* Parameters:
*  pz_MotionData - pointer to locEng defined motion data structure 
*             
*
*
* Return value: TRUE if the message was sent to PE, FALSE otherwise
* 
* -----------------------------------------------------------------------*/
boolean _mgp_SdpInjectMotionData(const slimInjectMotionDataIndMsgT *pz_MotionData);
boolean mgp_SdpInjectMotionData(const slimInjectMotionDataIndMsgT *pz_MotionData);

/* -------------------------------------------------------------------------
* _mgp_SdpValidateMotionData
*
* Description: 
*  This function provides an API for SDP to evaluate the validity of 
*  motion data for processing in MGP
*
* Parameters:
*  pz_MotionData - pointer to locEng defined motion data structure 
*             
*
*
* Return value: TRUE if the message was sent to PE, FALSE otherwise
* 
* -----------------------------------------------------------------------*/
boolean _mgp_SdpValidateMotionData(const slimMotionDataStructT *pz_MotionData);
boolean mgp_SdpValidateMotionData(const slimMotionDataStructT *pz_MotionData);

/* -------------------------------------------------------------------------
* _mgp_SdpInjectServiceStatusEvent
*
* Description: 
*  This function provides an async interface for SDP to send service
*  status event data to PE for processing
*
* Parameters:
*  pz_Ind - pointer to service status event data
*
* Return value: TRUE if the message was sent to PE, FALSE otherwise
* 
* -----------------------------------------------------------------------*/
boolean _mgp_SdpInjectServiceStatusEvent(
   const slimInjectServiceStatusEventIndMsgT *pz_Ind);
boolean mgp_SdpInjectServiceStatusEvent(
   const slimInjectServiceStatusEventIndMsgT *pz_Ind);


/*
 ******************************************************************************
 * _mgp_XtraInjInd
 *
 * Function description:
 *
 * This function indicates to MGP/PE that SM just finishes an xtra injection.
 *    
 * Parameters: 
 *   q_XtraInjMask - mask to indicate the XTRA data injection by GNSS constellations
 *
 * Return value: 
 *
 *   void
 *
 * ******************************************************************************
*/
void _mgp_XtraInjInd(gnss_XtraInjMaskType q_XtraInjMask);
void mgp_XtraInjInd(gnss_XtraInjMaskType q_XtraInjMask);

/*
 ******************************************************************************
 * Function _mgp_SetPositioningMethodConfig
 *
 * Description:
 *
 *  Set general use, positioning method configurations in the Position Engine 
 *  based on the PD-API defined input argument
 *
 * Parameters: 
 *  pz_PeConfigParams: Pointer to a structure containing the positioning method 
 *                     configurations
 *
 * Return value: 
 *  TRUE: Sending the IPC message was successful
 *  TRUE: Sending the IPC message was NOT successful
 *
 ******************************************************************************
*/
boolean _mgp_SetPositioningMethodConfig(const pdsm_lite_pa_pe_config_s_type *pz_PosMethodConfig);
boolean mgp_SetPositioningMethodConfig(const pdsm_lite_pa_pe_config_s_type *pz_PosMethodConfig);


/*
 ******************************************************************************
 * Function _mgp_GetGNSSSWVersion
 *
 * Description:
 *
 *  This function reads the specified GNSS SW version string. There are three 
 *  sw version strings that can be read
 *    1. GNSS ME SW version string (AKA the SOC Version, or the Baseband 
 *       version) This captures the version of the GNSS SW running on the GNSS 
 *       chip.
 *    2. Host SW version string. This captures the version of the GNSS SW 
 *       running on the host
 *    3. Full Version. This is a combination of the ME and the host sw versions
 *       and captures all the GNSS SW version related information.
 *
 * Parameters: 
 *    strind_id   : Which version string to return
 *    string      : An output param that is a char array, in which the specified 
 *                  string shall be returned.
 *    string_size : size of the char array string[], in bytes
 *
 * Return value: 
 *    TRUE:  version string populated
 *    FALSE: version string unpopulated
 ******************************************************************************
*/

boolean _mgp_GetGNSSSWVersion(mgp_VerStringsIDEnum string_id, char string[], uint16 string_size);
boolean mgp_GetGNSSSWVersion(mgp_VerStringsIDEnum string_id, char string[], uint16 string_size);
#ifdef FEATURE_GNSS_ENH_TIME_SET
/*=============================================================================

FUNCTION 
  mc_TsInitTowAssistance

DESCRIPTION
  This functions clears the TOW assistance data.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/

void mc_TsInitTowAssistance(void);

/*=============================================================================

FUNCTION 
  mc_TsFillTowAssistance

DESCRIPTION
  This functions populates the TOW Assistance data obtained from SM.

PARAMETERS
  p_RefTime - reference to the GPS ref time structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/

void mc_TsFillTowAssistance(const gps_RefTimeStructType *p_RefTime);
#endif /*FEATURE_GNSS_ENH_TIME_SET*/
/*
 ******************************************************************************
 * Function _mgp_FTCalEnable
 *
 * Description:
 *
 *  This function enables Fast TCal session.
 *
 * Parameters: 
 *  None
 *
 * Return value: 
 *  None
 *
 * *****************************************************************************
*/
void _mgp_FTCalEnable(void);
void mgp_FTCalEnable(void);


#ifdef FEATURE_GNSS_CELLDB_CONTROL
#error code not present
#endif /* #ifdef FEATURE_GNSS_CELLDB_CONTROL */


/*=============================================================================

  FUNCTION      _mgp_InjectMotionData()

  DESCRIPTION   Send motion data to MGP for processing in the navigation 
                solution

  DEPENDENCIES  None

  PARAMETERS    pz_MotionData: Pointer to PD-API defined motion data to be 
                               processed by MGP

  RETURN VALUE  TRUE: Motion data injection was successful
                FALSE: Motion data injection was unsuccessful

  SIDE EFFECTS  None

=============================================================================*/
boolean _mgp_InjectMotionData(const pdsm_pd_motion_data_s_type *pz_MotionData);
boolean mgp_InjectMotionData(const pdsm_pd_motion_data_s_type *pz_MotionData);


/*=============================================================================

  FUNCTION      _mgp_ValidateMotionData()

  DESCRIPTION   Evaluate validity of motion data for processing in MGP

  DEPENDENCIES  None

  PARAMETERS    pz_MotionData: Pointer to PD-API defined motion data to be 
                               validated

  RETURN VALUE  TRUE: Motion data is valid
                FALSE: Motion data is not valid

  SIDE EFFECTS  None

=============================================================================*/
boolean _mgp_ValidateMotionData(const pdsm_pd_motion_data_s_type *pz_MotionData);
boolean mgp_ValidateMotionData(const pdsm_pd_motion_data_s_type *pz_MotionData);


/* -------------------------------------------------------------------------
* _mgp_RfWwanInfo
*
* Description: 
*  This function provides an interface for RF drivers to notify the
*  GPS engine the status of the current WWAN technology.  This should be
*  called prior to the RF tuning to the band/channel.
*  This function will make gnss_RfWwanStatus() obsolete.
*
* Parameters:
*  e_CurWwan - Current active WWAN Technology
*  e_Band - Union of Band information.  
*           When e_CurWwan is GNSS_WWAN_STATE_WCDMA, use e_WCDMABand. 
*           When e_CurWwan is GNSS_WWAN_STATE_LTE, use e_LTEBand. 
*           All else, use e_RfBand.
*  q_Channel - Channel number
*  q_BwKhz - bandwidth ocupied by the channel in KHz
*  q_Direction - DL/UL direction of the information
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
void _mgp_RfWwanInfo
(
   gnss_WwanStateEnum       e_CurWwan,
   gnss_RfBandType          e_Band,
   uint32                   q_Channel,
   uint32                   q_BwKhz,
   cxm_tech_link_direction  q_Direction
   );
void mgp_RfWwanInfo
(
   gnss_WwanStateEnum       e_CurWwan,
   gnss_RfBandType          e_Band,
   uint32                   q_Channel,
   uint32                   q_BwKhz,
   cxm_tech_link_direction  q_Direction
   );

/* -------------------------------------------------------------------------
* _mgp_RfWwanTxInfo
*
* Description: 
*  This function provides an interface for RF drivers to notify the
*  GPS engine if the current WWAN is in TX or not.
*
*  u_TxOn shall not be deasserted when the Tx is turned off briefly while 
*  inn the connected state. Examples include:
*    IS-95 puncturing
*    WCDMA compressed mode
*    GSM Rx/idle slots
*    HSPA Tx gating (DTx) during continuous packet connections
*    Hard hand-offs
*
* Parameters:
*  u_TxOn - TRUE: WWAN is in TX
*           FALSE: WWAN is NOT in TX
*
* Return value: 
*  None
* -----------------------------------------------------------------------*/
void _mgp_RfWwanTxInfo
(
   boolean u_TxOn
   );
void mgp_RfWwanTxInfo
(
   boolean u_TxOn
   );



/*
******************************************************************************
* _mgp_UpdateWwanTxAntennaStatus
*
* Function description:
*
*  This function is used to update the status of which
*  TX Antenna is being used for WWAN transmissions. This antenna selection
*  control is needed to estimate the impact of the WLAN and WWAN Tx on GPS.
*  Different antennas provide different levels of isolation from the GPS band.
*  Based on the antenna used, GNSS receiver selects appropriate thresholds
*  for cross-correlation, or IMD technique to mitigate the impact.
*
*  This function should be called at start of a GPS session, and whenever the 
*  antenna selection is changed. 
*  The function updates a GNSS global variable to set the Tx Antenna status
*  and returns immediately without performing any further processing.
*  Note: If the device has only a single antenna for WWAN Tx, this API should 
*        be called with "WWAN_TX_ANTENNA_MAIN" as the antenna type.
*
* Parameters:
*
*  e_TxAntUsed - Indicates which Tx Antenna is selected.
*
* Return value:
*
*  void
*
******************************************************************************
*/
void _mgp_UpdateWwanTxAntennaStatus(gnss_TxAntennaEnumType e_TxAntUsed);
void mgp_UpdateWwanTxAntennaStatus(gnss_TxAntennaEnumType e_TxAntUsed);

/*
 ******************************************************************************
 * Function _mgp_EnableDpoStatusReport
 *
 * Description:
 *
 *  This function enables periodic DPO status reporting.
 *  When enabled, the DPO status is reported at a nominal 1 Hz rate when receiver
 *  is active. This control is persistent across GNSS sessions, i.e. when the
 *  configuration is set, it stays till the phone is powered off (or changed
 *  by a new configuration).
 *
 * Parameters:
 *  u_EnableDpoStatusRpt -- DPO status reporting will be enabled when set to TRUE.
 *
 * Return value:
 *  TRUE - if command is accepted
 *
 ******************************************************************************
*/
boolean _mgp_EnableDpoStatusReport(boolean u_EnableDpoStatusRpt);
boolean mgp_EnableDpoStatusReport(boolean u_EnableDpoStatusRpt);

/*
******************************************************************************
* _mgp_HardReset
*
* Function description:
*
*  This function is used to set MGP to the default state.
*
* Parameters:
*
*  none.
*
* Return value:
*
*  void
*
******************************************************************************
*/
void _mgp_HardReset(void);
void mgp_HardReset(void);

/*
******************************************************************************
* _mgp_UpdateEmergencyEvent
*
* Function description:
*
*  Session manager uses this function to update MGP/ME on current emergency event.
*  Based on emergency event, ME might disable WWAN-WLAN IMD, DPO.
*  This API should be called by SM, whenever call manager updates Session mananger
*  with emergency event.
*
* Parameters:
*
*  u_EmergencyEvent -  uint8 containing current emergency event.
*
* Return value:
*
*  void
*
******************************************************************************
*/
void _mgp_UpdateEmergencyEvent(MgpEmergencyEventType u_EmergencyEvent);
void mgp_UpdateEmergencyEvent(MgpEmergencyEventType u_EmergencyEvent);

/* 
* ------------------------------------------------------------------------------
* _mgp_ConfigRfSplAntGain
*
* Description: 
* This function provides an inteface to configure the following RF params
* 1) GNSS peak antenna gain in tenth of dB-Hz
* 2) GPS SPL ( system processing loss ) in tenth of dB-Hz
* 3) GLO SPL ( system processing loss ) for GLO frequencies -7,0,+6
* 4) BDS SPL ( system processing loss ) in tenth of dB-Hz
* 5) GAL SPL ( system processing loss ) in tenth of dB-Hz
*
* Parameters:
*  p_GnssRfConfig - Pointer to RF configuration
*
* Return value: 
*  True - RF configuration successfully sent to Measuerment Engine.
*  False - Unable to send RF configuration to Measurement Engine.
* 
* -----------------------------------------------------------------------*/
boolean _mgp_ConfigRfSplAntGain
(
   const gnss_RfConfigSplAntennaGainParamsType *p_RfConfig
   );
boolean mgp_ConfigRfSplAntGain
(
   const gnss_RfConfigSplAntennaGainParamsType *p_RfConfig
   );

/*
 ******************************************************************************
 * Function _mgp_LteTTRpt
 *
 * Description:
 *
 *  This function is indirectly called by LTE module to pass Time Transfer
 *  Report packet to PGI module.
 *
 * Parameters:
 *
 *  Time Transfer Report structure.
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

extern void _mgp_LteTTRpt(const cgps_LteTTRptType *);
extern void mgp_LteTTRpt(const cgps_LteTTRptType *);

/*
 ******************************************************************************
 * _mgp_ReqGnssEphInfo
 *
 * Function description:
 *
 *    This function requests the ephemeris for the specified GNSS SV.
 *    
 * Parameters: 
 *    w_Sv - GNSS SV for which ephemeris is requested
 *           GPS: 1-32, GLO: 65-96, BDS: 201-237 (PRN+200), GAL: 301-336 (PRN+300)
 *
 * Return value: 
 *
 *  void
 *
 * ******************************************************************************
*/
void _mgp_ReqGnssEphInfo(uint16 w_Sv);
void mgp_ReqGnssEphInfo(uint16 w_Sv);

/* -------------------------------------------------------------------------
 * _mgp_RfLTEB13TxPower
 *
 * Description: 
 *
 *  This function provides an interface for LTE L1 to notify GNSS SW
 *  of LTE TX power. LTE L1 layer provides this update at 100 msec rate.
 *
 * Parameters:
 *  t_LteClockTick   : LTE clock when Max tx power is updated
 *  x_LteAvgTxPower: LTE Max tx power relative to absolute LTE max tx power in tenth of a dB.
 *    Invalid value : 0x8000 ( No transmit in the last 100 msecs )
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
void _mgp_RfLTEB13TxPower(uint64 t_LteClockTick, int16 x_LteAvgTxPower);
void mgp_RfLTEB13TxPower(uint64 t_LteClockTick, int16 x_LteAvgTxPower);

/*
 ******************************************************************************
 * _mgp_HardReset
 *
 * Function description:
 *
 *  This function is used to set MGP to the default state.
 *
 * Parameters:
 *
 *  none.
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/
void _mgp_HardReset(void);
void mgp_HardReset(void);

/*
 ******************************************************************************
 * _mgp_ProcLteRfDevCmd
 *
 * Function description:
 *
 *  This function is used by DIAG to enable/disable RF dev flag in PGI.
 *
 * Parameters:
 *
 *  Sub command.
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/
void _mgp_ProcLteRfDevCmd(uint8 b_SubCmd);
void mgp_ProcLteRfDevCmd(uint8 b_SubCmd);


/* -------------------------------------------------------------------------
* _mgp_InjectCourseOverGroundData
*
* Description: 
* This function provides an interface to inject the course over ground 
* information to the Position Engine

* Parameters:
*  pz_CogData - Pointer to Course Over Ground information 
*
* Return value: 
*  True - COG data is valid and successfully sent to Position Engine.
*  False - Unable to send COG data to Position Engine, or it is invalid
* 
* -----------------------------------------------------------------------*/
boolean _mgp_InjectCourseOverGroundData
(
   const gnss_CourseOverGroundDataType *pz_CogData
   );
boolean mgp_InjectCourseOverGroundData
(
   const gnss_CourseOverGroundDataType *pz_CogData
   );

/* -------------------------------------------------------------------------
* _mgp_InjectExternalSensorAssistData
*
* Description: 
* This function provides an interface to inject the External sensor assitance
* data information to the Position Engine

* Parameters:
*  pz_Data - Pointer to External sensor assistance information 
*
* Return value: 
*  eLOC_ENG_SUCCESS_V01 - Ext Sensor data is valid and successfully sent to Position Engine.
*  eLOC_ENG_GENERAL_FAILURE_V01 - Unable to send Ext Sensor data to Position Engine, because
*                                 the data is invalid or IPC sending has failed
*  eLOC_ENG_UNSUPPORTED_V01 - Unable to send Ext Sensor data to Position Engine, because
*                             the time source is not supported
* 
* -----------------------------------------------------------------------*/
boolean _mgp_InjectExternalSensorAssistData
(
   const gnss_ExternalSensorAssistDataType *pz_Data
   );
boolean mgp_InjectExternalSensorAssistData
(
   const gnss_ExternalSensorAssistDataType *pz_Data
   );

/*
 ******************************************************************************
 * Function _mgp_GetGpsTime
 *
 * Description:
 * Provide a time API to Tcxo-Mgr for XO aging feature.
 *  
 * Parameters:
 * None
 *
 * Dependencies:
 * None
 *
 * Return value: 
 *
 *  uint64: GPS time in seconds, 0 indicates time is not known.
 *
 ******************************************************************************
*/
uint64 _mgp_GetGpsTime(void);
uint64 mgp_GetGpsTime(void);

/*
 ******************************************************************************
 * Function _mgp_MapToGnssType
 *
 * Description:
 * This is a mapping function which maps a number to the corresponding value
 * of type mgp_GnssType.
 *  
 * Parameters:
 * q_GnssType - Number representing the GNSS system
 *
 * Dependencies:
 * None
 *
 * Return value: 
 *
 *  The mapped mgp_GnssType. An invalid system is mapped to MGP_GNSS_TYPE_ALL
 *
 ******************************************************************************
*/
mgp_GnssType _mgp_MapToGnssType(uint32 q_GnssType);
mgp_GnssType mgp_MapToGnssType(uint32 q_GnssType);

/* -------------------------------------------------------------------------
 * _mgp_QueryBlanking
 *
 * Description: 
 *
 *  This function provides DIAG a way to query the blanking config from RF
 *  and get the current blanking counter value.
 *
 * Parameters:
 *  p_QueryBlanking : Pointer to a struct to hold the query results.
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
void _mgp_QueryBlanking(mgp_QueryBlankingType *p_QueryBlanking);
void mgp_QueryBlanking(mgp_QueryBlankingType *p_QueryBlanking);

/* -------------------------------------------------------------------------
 * _mgp_NavConfig
 *
 * Description: 
 *
 *  This function is used to configure the Nav Core, which will then
 *  configure Nav RF as needed and notify upper layers like MC.
 *
 * Parameters:
 *  p_NavConfig : Pointer to a struct to hold the config results.
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
void _mgp_NavConfig(mgp_NavConfigType *p_NavConfig);
void mgp_NavConfig(mgp_NavConfigType *p_NavConfig);

/* -------------------------------------------------------------------------
 * _mgp_NavSetTestMode
 *
 * Description: 
 * This function is used to do mandatory test mode set to NavRF driver.
 *
 * Parameters:
 *    uint8 u_Cmd
 *
 * Return value: 
 *  TRUE/FALSE
 * -----------------------------------------------------------------------*/
boolean _mgp_NavSetTestMode(uint8 u_Cmd);
boolean mgp_NavSetTestMode(uint8 u_Cmd);


/* -------------------------------------------------------------------------
 * _mgp_QueryAdcIqSatPer
 *
 * Description: 
 *
 *  This function provides DIAG a way to query the ADC IQ saturation
 *  percentage 
 *
 * Parameters:
 *  p_QueryAdcIqSatPer : Pointer to a struct to hold the query results.
 *
 * Return value: 
 *  None
 * -----------------------------------------------------------------------*/
void _mgp_QueryAdcIqSatPer(mgp_QueryAdcIqSatPerType *p_QueryAdcIqSatPer);
void mgp_QueryAdcIqSatPer(mgp_QueryAdcIqSatPerType *p_QueryAdcIqSatPer);


/* -------------------------------------------------------------------------
 * _mgp_NavIoctl
 *
 * Description:
 * 
 *  This function is used to run multiple GNSS RF Diag tests
 *
 * Parameters:
 *  p_NavIoctl : Pointer to a struct to hold and request/response parameters.
 *
 * Return value: 
 *  TRUE  - Diag command query is successful
 *  FALSE - query failed
 * -----------------------------------------------------------------------*/
boolean _mgp_NavIoctl(mgp_NavIoctlType *p_NavIoctl);
boolean mgp_NavIoctl(mgp_NavIoctlType *p_NavIoctl);

/* -------------------------------------------------------------------------
* _mgp_SdpInjectVehicleSensorData
*
* Description: 
*  This function provides an async interface for SDP to send vehicle sensor
*  data to PE for processing
*
* Parameters:
*  pz_VehicleSensorData - pointer to vehicle sensor data structure 
*             
*
* Return value: TRUE if the message was sent to PE, FALSE otherwise
* 
* -----------------------------------------------------------------------*/
boolean _mgp_SdpInjectVehicleSensorData(
   const slimInjectVehicleSensorDataIndMsgT *pz_VehicleSensorData);
boolean mgp_SdpInjectVehicleSensorData(
   const slimInjectVehicleSensorDataIndMsgT *pz_VehicleSensorData);

/* -------------------------------------------------------------------------
* _mgp_SdpValidateVehicleSensorData
*
* Description: 
*  This function provides an API for SDP to evaluate the validity of 
*  vehicle sensor data for processing in MGP
*
* Parameters:
*  pz_VehicleSensorData - pointer to vehicle sensor data structure 
*             
*
* Return value: TRUE if valid, FALSE otherwise
* 
* -----------------------------------------------------------------------*/
boolean _mgp_SdpValidateVehicleSensorData(
   const slimVehicleSensorDataStructT *pz_VehicleGyroData);
boolean mgp_SdpValidateVehicleSensorData(
   const slimVehicleSensorDataStructT *pz_VehicleGyroData);

/* -------------------------------------------------------------------------
* _mgp_SdpInjectOdometryData
*
* Description: 
*  This function provides an async interface for SDP to send odometry data
*  to PE for processing
*
* Parameters:
*  pz_OdometryData - pointer to odometry data structure 
*             
*
* Return value: TRUE if the message was sent to PE, FALSE otherwise
* 
* -----------------------------------------------------------------------*/
boolean _mgp_SdpInjectOdometryData(
   const slimInjectVehicleOdometryDataIndMsgT *pz_OdometryData);
boolean mgp_SdpInjectOdometryData(
   const slimInjectVehicleOdometryDataIndMsgT *pz_OdometryData);

/* -------------------------------------------------------------------------
* _mgp_SdpValidateOdometryData
*
* Description: 
*  This function provides an API for SDP to evaluate the validity of 
*  odometry data for processing in MGP
*
* Parameters:
*  pz_OdometryData - pointer to odometry data structure 
*             
*
* Return value: TRUE if valid, FALSE if not
* 
* -----------------------------------------------------------------------*/
boolean _mgp_SdpValidateOdometryData(
   const slimVehicleOdometryDataStructT *pz_OdometryData);
boolean mgp_SdpValidateOdometryData(
   const slimVehicleOdometryDataStructT *pz_OdometryData);


/* -------------------------------------------------------------------------
* _mgp_SdpInjectNavigationConfig
*
* Description: 
*  This function provides an async interface for SDP to send navigation 
*  config to PE for processing
*
* Parameters:
*  pz_NavigationConfig - pointer to locEng navigation config structure 
*             
*
* Return value: TRUE if the message was sent to PE, FALSE otherwise
* 
* -----------------------------------------------------------------------*/
boolean _mgp_SdpInjectNavigationConfig(
   const locEngSetNavigationConfigReqMsgT_v01 *pz_NavigationConfig);
boolean mgp_SdpInjectNavigationConfig(
   const locEngSetNavigationConfigReqMsgT_v01 *pz_NavigationConfig);

/* -------------------------------------------------------------------------
* _mgp_SdpValidateNavigationConfig
*
* Description: 
*  This function provides an API for SDP to evaluate the validity of 
*  navigation config for processing in MGP
*
* Parameters:
*  pz_NavigationConfig - pointer to locEng navigation config structure 
*             
*
* Return value: locEng status of the validation
* 
* -----------------------------------------------------------------------*/
locEngStatusEnumT_v01 _mgp_SdpValidateNavigationConfig(
   const locEngSetNavigationConfigReqMsgT_v01 *pz_NavigationConfig);
locEngStatusEnumT_v01 mgp_SdpValidateNavigationConfig(
   const locEngSetNavigationConfigReqMsgT_v01 *pz_NavigationConfig);

/* -------------------------------------------------------------------------
* _mgp_SdpGetNavigationConfig
*
* Description: 
*  This function provides an async interface for SDP to query the configuration
*  settings within PE/NF. The response is an IPC. 
*
* Parameters:
*  pz_Query - locEng get navigation config request structure
*
* Return value: TRUE if the message was sent to PE, FALSE otherwise
* See also:   locEngGetNavigationConfigReqMsgT_v01
* -----------------------------------------------------------------------*/
boolean _mgp_SdpGetNavigationConfig(const locEngGetNavigationConfigReqMsgT_v01 *pz_Query);
boolean mgp_SdpGetNavigationConfig(const locEngGetNavigationConfigReqMsgT_v01 *pz_Query);

/* -------------------------------------------------------------------------
 * _mgp_GetGnssSlowClockTime
 *
 * Description: 
 *  This function provides the current GNSS slow clock time.
 *
 * Parameters:
 *  p_GnssSlowClockTime -  Pointer to GNSS slow clock data structure
 *
 * Return value: 
 *  TRUE - Valid GPS time available
 *  FALSE - No GPS time available
 * -----------------------------------------------------------------------*/
boolean _mgp_GetGnssSlowClockTime(gnss_SlowClockStructType *p_GnssSlowClockTime);
boolean mgp_GetGnssSlowClockTime(gnss_SlowClockStructType *p_GnssSlowClockTime);


/*=============================================================================

FUNCTION
  _mgp_GetGGRfgdMs()

DESCRIPTION
  This function returns the current value of the RF Group Delay of GPS/GLO.
  The value is used to propagate GLO measurements to the GPS time-stamp
  
PARAMETERS

RETURN VALUE
  RFGD value in milli-seconds (If RFGD is not valid, 0 is returned) 
=============================================================================*/
FLT _mgp_GetGGRfgdMs(void);
FLT mgp_GetGGRfgdMs(void);

/*
******************************************************************************
* _mgp_GetGnssRcvrState
*
* Function description:
*
*  This API returns the current GNSS receiver state
*
* Parameters:
*  None.
* 
* Return value:
*  boolean: GNSS Receiver State
*  TRUE:   GNSS Receiver Session is active
*  FALSE:  GNSS Receiver session is inactive
*
******************************************************************************
*/
boolean _mgp_GetGnssRcvrState(void);
boolean mgp_GetGnssRcvrState(void);

/*
******************************************************************************
* _mgp_GetActiveGnssRcvrConfiguration
*
* Function description:
*
*  This API returns the current GNSS receiver configuration.
*
*  Active Receiver GNSS configuration bit mask.
*  #define C_RCVR_GNSS_CONFIG_GPS_ENABLED                (1<<0)
*  #define C_RCVR_GNSS_CONFIG_GLO_ENABLED                (1<<1)
*  #define C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE      (1<<4)
*  #define C_RCVR_GNSS_CONFIG_BDS_ENABLED (C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE)
*
* Parameters:
*  Pointer to configuration mask.
* 
* Return value:
*  None.
*
******************************************************************************
*/
void _mgp_GetActiveGnssRcvrConfiguration(uint8 *pu_CfgMask);
void mgp_GetActiveGnssRcvrConfiguration(uint8 *pu_CfgMask);

/*
******************************************************************************
* _mgp_GetHwSupportedRcvrConfiguration
*
* Function description:
*
*  This API returns the current GNSS HW supported receiver configuration.
*
*  Active Receiver GNSS configuration bit mask.
*  #define C_RCVR_GNSS_CONFIG_GPS_ENABLED                (1<<0)
*  #define C_RCVR_GNSS_CONFIG_GLO_ENABLED                (1<<1)
*  #define C_RCVR_GNSS_CONFIG_BDS_ENABLED_OUTSIDE_OF_US  (1<<2)
*  #define C_RCVR_GNSS_CONFIG_GAL_ENABLED_OUTSIDE_OF_US  (1<<3)
*  #define C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE      (1<<4)
*  #define C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE      (1<<5)
*
* Parameters:
*  Pointer to configuration mask.
* 
* Return value:
*  None.
*
******************************************************************************
*/
void _mgp_GetHwSupportedRcvrConfiguration(uint8 *pu_CfgMask);
void mgp_GetHwSupportedRcvrConfiguration(uint8 *pu_CfgMask);

/*
******************************************************************************
* _mgp_DisableBdsGnssRcvr
*
* Function description:
*
*  This API enables/disables BeiDou signal tracking / positioning at run time.
*  The setting does not persist across modem restarts, i.e. the value is not
*  stored in non-volatile memory.
*  Notes:
*  1) Changes are effective on next session start 
*  2) Request is ignored if the receiver is in RF dev mode. 
*  3) Will not write setting to NV.
*  4) BeiDou enablement is only honored if LTE B13 is not active and
*     memory is available for BeiDou to function properly.
*
* Parameters:
*  u_DisableBdsGnssRcvr - BeiDou will be disabled when this is set to TRUE
*                       - If this parameter is set to FALSE, BeiDou will
*                         be enabled only if BeiDou memory is available
*                         and the receiver has not detected LTE B13 concurrency,
*                         and receiver is not in RF dev mode.
* 
* 
* Return value:
*  None.
*
******************************************************************************
*/
void _mgp_DisableBdsGnssRcvr(boolean u_DisableBdsGnssRcvr);
void mgp_DisableBdsGnssRcvr(boolean u_DisableBdsGnssRcvr);

/*
******************************************************************************
* _mgp_DisableGalGnssRcvr
*
* Function description:
*
*  This API enables/disables Galileo signal tracking / positioning regardless of
*  Galileo enablement thru non-volatile memory or not.
*  The setting does not persist across modem restarts, i.e. the value is not
*  stored in non-volatile memory.
*  Notes:
*  1) Changes are effective on next session start
*  2) Request is ignored if the receiver is in RF dev mode.
*  3) Will not write setting to NV.
*  4) Galileo enablement is only honored if LTE B13 is not active and
*     memory is available for Galileo to function properly.
*
* Parameters:
*  u_GalCtrl   -   TRUE -  Galileo will be enabled regardless of NV setting
*                   FALSE-  Galileo will be disabled regardless of NV setting
*
* Return value:
*  None.
*
******************************************************************************
*/
void _mgp_DisableGalGnssRcvr( uint8 u_GalCtrl);
void mgp_DisableGalGnssRcvr( uint8 u_GalCtrl);

#ifdef FEATURE_GNSS_DYNAMIC_SP_MEM_ALLOCATION

/*
******************************************************************************
* _mgp_IqTestCompleteMemRelHandler()
*
* Function description:
*
*  This API calls handles the cleanup of RF dev terst memories and allocation of mission mode memories
*   to be called at the end of a RF dev test
* Parameters:
*  Pointer to configuration mask.
* 
* Return value:
*  None.
*
******************************************************************************
*/
void _mgp_IqTestCompleteMemRelHandler(void);
void mgp_IqTestCompleteMemRelHandler(void);
#endif
/*
******************************************************************************
* mgp_UpdateAgpsAgnssSessionTimeOutMsecs
*
* Function description:
*
*  Session manager uses this function to update MGP/ME on MS-B session time out
*
* Parameters:
*
*  q_SessionTimeOutMsecs -  SM session timeout in msecs
*
* Return value:
*
*  None
*
******************************************************************************
*/
void _mgp_UpDateAgpsAgnssSessionTimeOutMsecs(uint32 q_SessionTimeOutMsecs);
void mgp_UpDateAgpsAgnssSessionTimeOutMsecs(uint32 q_SessionTimeOutMsecs);

/*
 ******************************************************************************
 * Function _mgp_GetTimeInfo
 *
 * Description:
 * Api for latching Reference Time information for WWANs during Time related 
 * operations like time tag and time transfer
 *  
 * Parameters:
 * pz_ComTime - Common Time structure
 * e_TimeType - Type of operation
 *
 * Dependencies:
 * None
 *
 * Return value: 
 * None
 *
 ******************************************************************************
*/
void _mgp_GetTimeInfo
(
   cgps_ComTimeType *pz_ComTime,
   cgps_TimeReqType e_TimeReqType
   );
void mgp_GetTimeInfo
(
   cgps_ComTimeType *pz_ComTime,
   cgps_TimeReqType e_TimeReqType
   );

/*
******************************************************************************
* _mgp_AgpsNetworkConnectActive
*
* Function description:
*
*  Session manager uses this function to update MGP/ME on Network connection being 
*  active for AGPS/AGNSS session. 
*
* Parameters: None
*
* Return value:   None
********************************************************************************
*/
void _mgp_AgpsNetworkConnectActive(void);
void mgp_AgpsNetworkConnectActive(void);

/*
******************************************************************************
* _mgp_GetRfDevFeatureNum
*
* Function description:
*
*  Retrieves the HW-specific RF Dev feature number
*
* Parameters: None
*
* Return value:
*   RF Dev Feature number
********************************************************************************
*/
uint8 mgp_GetRfDevFeatureNum(void);

/*
******************************************************************************
* _mgp_GnssGetConstellationConfig
*
* Function description:
*
*  Reads NV item which describes the enabled constellations and SBAS control information.
*
* Parameters:
*  p_GnssCfg- pointer to GNSS Configuration constellations information
*                   Bit 0  Controls GPS, and has to be set to 1 always
*			Bit 1  Controls Glonass.
*			Bit 2  Controls Beidou outside of USA.
*			Bit 3  Controls Galileo outside of USA.
*			Bit 4  Controls Beidou worldwide.
*			Bit 5  Controls Galileo worldwide.
*			Bit 6  Reserved for future use, and has to be set to 0 always.
*			Bit 7  Reserved for HBW
*			Bits 8..31  Reserved, and has to be set to 0 always.
*
*  p_SbasCfg- pointer to SBAS control information
*			Value 0  SBAS is disabled
*			Value 1  SBAS is enabled
*			All other values  Reserved
*
* Return value:
*  TRUE in case both values are read otherwise FALSE incase the input pointers are corrupted
*
********************************************************************************
*/
boolean _mgp_GnssGetConstellationConfig(uint32 *p_GnssCfg, uint32 *p_SbasCfg);
boolean mgp_GnssGetConstellationConfig(uint32 *p_GnssCfg, uint32 *p_SbasCfg);

/**
  @brief Heap allocation failure test type.
 */
typedef enum
{
  C_HEAP_ALLOC_FAIL_TEST_NONE = 0, /*!< No heap allocation failure */
  C_HEAP_ALLOC_FAIL_TEST_GNSS = 1, /*!< GNSS heap allocation failure */
  C_HEAP_ALLOC_FAIL_TEST_SBAS = 2, /*!< SBAS heap allocation failure */
  C_HEAP_ALLOC_FAIL_TEST_ALL  = 3  /*!< GNSS & SBAS heap allocation failure */
} mgp_HeapAllocFailTestType;

/*
******************************************************************************
* _mgp_SetHeapAllocFailTestVolatile
*
* Function description:
*
* Used to set the heap memory allocation test failure status.
*
* Parameters:
*  e_FailHeapAlloc Heap allocation failure type.
*
* Return value:
*  None.
*
******************************************************************************
*/
void _mgp_SetHeapAllocFailTestVolatile(mgp_HeapAllocFailTestType e_FailHeapAlloc);
void mgp_SetHeapAllocFailTestVolatile(mgp_HeapAllocFailTestType e_FailHeapAlloc);

/* 
 ******************************************************************************
* _mgp_InitTimeTickDalHandle
*
* Function description:
*
*  Initialize the handle to be able to call Qtimer timetick APIs.
*
* Parameters: 
*   None
*
* Return value:
*   None
********************************************************************************
*/
void _mgp_InitTimeTickDalHandle( void );
void mgp_InitTimeTickDalHandle( void );

/*
 ******************************************************************************
 * Function _mgp_GetXOTimeTickDiffMs
 *
 * Description:
 *
 * This function returns the 64 bit timetick counter value clocked at 19.2MHz.
 * This function definition technically must reside in mgp_api.c but since 
 * its a uimage function it's placed in mgp_gpm_uimage.c
 *
 * Parameters: 
 * 
 *  None
 *
 * Dependencies:
 *
 *  mgpp_DalDevHandle should be initialized
 *
 * Return value: 
 *
 *  uint64 counter at 19.2 MHz
 *
 ******************************************************************************
*/
uint64 _mgp_GetXOTimeTick( void );
uint64 mgp_GetXOTimeTick( void );

/*
 ******************************************************************************
 * Function _mgp_GetXOTimeTickDiffMs
 *
 * Description:
 *
 *  This function takes two 64 bit TimeTick values in Qtimer units 
 * (ticks @ 19.2MHz) and returns the difference in ms.
 * This function definition technically must reside in mgp_api.c but since 
 * its a uimage function it's placed in mgp_gpm_uimage.c
 *
 *  diff_msec = t_EndTime - t_StartTime
 *
 * Parameters: 
 *
 *  t_StartTime - TimeTick Value in Qtimer units (19.2MHz)
 *  t_EndTime - TimeTick Value in Qtimer units (19.2MHz)
 *
 * Dependencies:
 *
 *  None
 *
 * Return value: 
 *
 *  The time diff in milli-seconds
 *
 ******************************************************************************
*/
int64 _mgp_GetXOTimeTickDiffMs( uint64 t_StartTime, uint64 t_EndTime );
int64 mgp_GetXOTimeTickDiffMs( uint64 t_StartTime, uint64 t_EndTime );

/* 
 ******************************************************************************
* _mgp_InitTimeTickDalHandle
*
* Function description:
*
*  Initialize the handle to be able to call Qtimer timetick APIs.
*
* Parameters: 
*   None
*
* Return value:
*   None
********************************************************************************
*/
void _mgp_InitTimeTickDalHandle( void );
void mgp_InitTimeTickDalHandle( void );

/*
 ******************************************************************************
 *  _mgp_ClockTimeEstPutWithSrc
 *
 * Function description:
 *
 *  mgp_ClockTimeEstPutWithSrc provides a mechanism for the application to transfer
 *  a time measurement to the Measurement Engine. The time estimate may be
 *  derived from a variety of sources including network time, the RTC or
 *  from an external Event.
 *
 *  If u_Force is FALSE, the ME will only accept the new time estimate if
 *  that estimates uncertainty is less that ME's current clock uncertainty.
 *
 *  If u_Force is TRUE, then ME's time estimate is unconditionally overwritten.
 *
 *  Time src became a "must have" parameter when PGI became a separate task.
 *  This function was written as opposed to modifying me_ClockTimeEstPut()
 *  mainly to minimize code mods.
 *
 * Parameters:
 *
 *  q_FCount - FCount reference for the time estimate.

 *  p_Time - Pointer to the structure that contains the time measurement.
 *  u_Force - TRUE forces new time estimate to be accepted
 *  e_Src - Source of time.
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/

void _mgp_ClockTimeEstPutWithSrc(
uint32 q_FCount,
const gps_TimeStructType *p_Time,
boolean u_Force,
mgp_TimeEstPutSourceIDEnum e_Src
);
void mgp_ClockTimeEstPutWithSrc(
uint32 q_FCount,
const gps_TimeStructType *p_Time,
boolean u_Force,
mgp_TimeEstPutSourceIDEnum e_Src
);

/* 
 ******************************************************************************
* _mgp_ForceCCError
*
* Function description:
*   Diag command to force CC error
*
* Parameters: 
*   None
*
* Return value:
*   None
********************************************************************************
*/
void _mgp_ForceCCError(void);
void mgp_ForceCCError(void);

/*
 ******************************************************************************
 * Function mgp_ProcMeDiagCmd
 *
 * Description:
 *
 *  Send an IPC message to GTS module with the Diag command information 
 *
 * Parameters:
 *   p_Data - Pointer to buffer containing diag data
 *   q_Size - Size of the diag command in bytes
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
void _mgp_ProcMeDiagCommands( U8* p_Data, U8 u_Length );
void mgp_ProcMeDiagCommands( U8* p_Data, U8 u_Length );

/*=============================================================================

  FUNCTION      _mgp_UpdateQmiPosRptTime()

  DESCRIPTION   This function provides an indication of the QMI Position
                report time, logged when QMI is sending out the report
                over QMI pipe. The message is sent to Measurement Engine
                and is used to measure processing latency from ME->PE->SM->QMI
                This is used both for diagnostics and possibly for MIPS management
 

  DEPENDENCIES  None

  PARAMETERS    t_QmiPosRptTimeMsec --- Time Tick msec corresponding to QMI
                                        Pos report time
                w_GpsWeek --- GPS Week corresponding to the Pos Report
                q_GpsMsec --- GPS Msec of the week corresponding to the Pos Report
 
  RETURN VALUE  None

  SIDE EFFECTS  This is used both for diagnostics and possibly for MIPS management

=============================================================================*/
void _mgp_UpdateQmiPosRptTime
(
   uint16 w_GpsWeek,
   uint32 q_GpsMsec,
   uint64 t_QmiPosRptTimeMsec
);
void mgp_UpdateQmiPosRptTime
(
   uint16 w_GpsWeek,
   uint32 q_GpsMsec,
   uint64 t_QmiPosRptTimeMsec
);

/*
 ******************************************************************************
 * Function:
 *
 *   _mgp_pgi_SendSibTimeRefreshMsg
 *
 * Description:
 *
 *  Sends a SIB Time refresh message via MSGR to LTE RRC
 *
 * Parameters:
 *
 *  LTE SUB ID that needs to be reset
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void _mgp_pgi_SendSibTimeRefreshMsg( int8 b_SubInfo );
void mgp_pgi_SendSibTimeRefreshMsg( int8 b_SubInfo );

/*
******************************************************************************
* _mgp_SendOfflineCxmTestRequest
*
* Function description:
*  Send a request to start the Offline CXM test.
*
* Parameters:
*   None
*
* Return value:
*   None
********************************************************************************
*/
void _mgp_SendOfflineCxmTestRequest(void);
void mgp_SendOfflineCxmTestRequest(void);
/* 
 ******************************************************************************
 * 
 *  _mgp_ConfigExtTimeTransferMode
 *
 * Description:
 *  This function is stubbed out
 *
 *
 * Parameters:
 *       u_EnableFlag - Input is ignored
 *
 * Return value: 
 *     FALSE: The feature is not supported
 ******************************************************************************
*/
uint8 _mgp_ConfigExtTimeTransferMode(uint8 u_EnableFlag);
uint8 mgp_ConfigExtTimeTransferMode(uint8 u_EnableFlag);

/*
******************************************************************************
* _mgp_UpdateUax3Config
*
* Function description:
*  Update the UAX3 configuration information to MGP
*
* Parameters:
*   q_Uax3CfgInfo - UAX3 configuration information
*
* Return value: 
********************************************************************************
*/
void _mgp_UpdateUax3Config(uint32 q_Uax3CfgInfo);
void mgp_UpdateUax3Config(uint32 q_Uax3CfgInfo);

typedef struct
{
  uint16 w_GpsWeek;     /* GPS week number for Leap Second Information [weeks] */
  uint32 q_GpsMsec;     /* GPS msec for Leap Second Information [msecs] */

  /*About Current Leap Second*/
  uint8  u_DeltaLeapSec;    /* GPS time leap second delta to UTC time */
  uint8  u_DeltaLeapSecUnc; /* Uncertainty for GPS leap second */
  uint8  u_DeltaLeapSecSrc; /* Source of Leap Sec */

  /*About pontential Leap Second Rollover*/
  DBL    d_UtcLsChangeTimeSec;  /* LS change time in UTC time 
                                   in secs since January 6th, 1980.
                                   If UTC data is not valid, this field reports
                                   the nearest quarter boundary in UTC time.*/  
  uint8  u_UtcValid;            /*0: UTC is unknown   1: UTC is known*/
  uint8  u_PreGuardDuration;    /*In sec*/
  uint8  u_PostGuardDuration;   /*In sec*/
  int8   b_Correction;          /* Leap Second Correction valid when u_DeltaLeapSecUnc 
                                   is 0.
                                   -1:  on negative leap second correction. 
                                        UTC output shall report "00" instead 
                                        of "59" in UTC seconds
                                    0:  when no leap second correction is done
                                   +1:  on positive leap second correction. 
                                        UTC output shall report "60" instead 
                                        of "00" in UTC seconds.
                                */

} mgp_LeapSecondInfoStructType;



/*
 ******************************************************************************
 * _mgp_LeapSecInfoPut
 *
 * Function description:
 *
 *    This function sends LeapSec and Rollover information to the PE.
 *
 * Parameters:
 *
 *  p_LeapSecInfo - Pointer to the LeapSec and Rollover information
 *
 * Return value:
 *
 *  void
 *
 * ******************************************************************************
*/
void _mgp_LeapSecInfoPut( const mgp_LeapSecondInfoStructType *p_LeapSecInfo );
void mgp_LeapSecInfoPut( const mgp_LeapSecondInfoStructType *p_LeapSecInfo );

/*
******************************************************************************
* _mgp_GetPgaGain
*
* Function description:
*  This function gets the current PGA setting in units of dB
*
* Parameters:
*  None
* 
* Return value:
*  PGA setting in units of dB
*
******************************************************************************
*/
int32 _mgp_GetPgaGain(void);
int32 mgp_GetPgaGain(void);

#ifdef FEATURE_GNSS_LTE_CATM_COEX

/*
 ******************************************************************************
 * _mgp_SendCatmLteTxStartCmd
 *
 * Function description:
 *
 *  p_CatmLteTxStartTestParams allows the application to load parameters needed by MGP 
 * to start CATM LTE Tx test.
 *
 * Parameters: 
 *
 *  p_CatmLteTxStartTestParams - Pointer to data needed to be injected into MGP.
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void _mgp_SendCatmLteTxStartCmd( const mgp_CatmLteTxStartTestParams *p_CatmLteTxStartTestParams);
void mgp_SendCatmLteTxStartCmd( const mgp_CatmLteTxStartTestParams *p_CatmLteTxStartTestParams);

/*
 ******************************************************************************
 * _mgp_SendCatmLteTxEndCmd
 *
 * Function description:
 *
 *  p_CatmLteTxEndTestParams allows the application to load parameters needed by MGP 
 * to trigger CATM LTE Tx End response.
 *
 * Parameters: 
 *
 *  p_CatmLteTxEndTestParams - Pointer to data needed to be injected into MGP.
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void _mgp_SendCatmLteTxEndCmd( const mgp_CatmLteTxEndTestParams *p_CatmLteTxEndTestParams);
void mgp_SendCatmLteTxEndCmd( const mgp_CatmLteTxEndTestParams *p_CatmLteTxEndTestParams);
#endif /* FEATURE_GNSS_LTE_CATM_COEX */

/*=============================================================================

FUNCTION
  _mgp_CheckNavContinueDuringE911

DESCRIPTION
  This function checks all the conditions to see if GNSS can continue durina 
  E911 call
DEPENDENCIES
  Search Strategy must have been selected already and PE/ME status 
  should be available

PARAMETERS

  None


RETURN VALUE
  TRUE if Nav operation can continue

SIDE EFFECTS
  None.

=============================================================================*/

uint8 _mgp_CheckNavContinueDuringE911(void);
uint8 mgp_CheckNavContinueDuringE911(void);

/*
 ******************************************************************************
 * _mgp_E911SessionIndPut
 *
 * Function description:
 *
 *    This function sends E911 Start / Stop indication to PE.
 *
 * Parameters:
 *
 *  u_E911Indicator - 1 if E911 session is started, 0 if stopped
 *
 * Return value:
 *
 *  void
 *
 * ******************************************************************************
*/
void _mgp_E911SessionIndPut( const uint8 u_E911Indicator );
void mgp_E911SessionIndPut( const uint8 u_E911Indicator );

/* -------------------------------------------------------------------------
* _mgp_InjectMagCal
*
* Description:
*  This function provides an async interface for SAMLITE to send usr MagCal 
*  status to MGP/NF
*
* Parameters:
*  v_MagCalUserCalibrated - usr MagCal sucessful
*
* Return None
*
* -----------------------------------------------------------------------*/
void _mgp_InjectMagCal(const boolean v_MagCalUserCalibrated);
void mgp_InjectMagCal(const boolean v_MagCalUserCalibrated);

/*
******************************************************************************
* _mgp_AbdsSessionIndPut
*
* Function description:
*  Update the A-BDS session information to MGP 
*
* Parameters:
*   v_AbdsSessionState - A-BDS session information
*
* Return value: 
******************************************************************************
*/
void _mgp_AbdsSessionIndPut(const boolean v_AbdsSessionState);
void mgp_AbdsSessionIndPut(const boolean v_AbdsSessionState);

/*
 ******************************************************************************
 * _mgp_GnssLEReportingOn
 *
 * Function description:
 *
 *  Request to turn on Gnss LE only fix reporting. Once enabled, PE shall report fixes from GNSS LE
 * as well during the positioning session. This is in addition to the hybrid fix reporting.
 * Note: Engine should not be in off state when calling this api.
 * Note: Ale will automatically stop reporting Gnss LE fixes when the engine is turned off. Any new session
 * would need to start Gnss LE again 
 *
 * Parameters: 
 *  void
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
boolean _mgp_GnssLEReportingOn(void);
boolean mgp_GnssLEReportingOn(void);

/*
 ******************************************************************************
 * _mgp_GnssLEReportingOff
 *
 * Function description:
 *
 *  Request to turn off Gnss LE reporting.. Once Gnss LE reporting is turned off, PE shall stop reporting fixes from GNSS
 * LE in the positioning session. It may still continue reporting positiongs from hybrid LE. 
 * Note: Ale will automatically stop reporting Gnss LE fixes when the engine is turned off. Any new session
 * would need to start Gnss LE reporting again
 *
 * Parameters: 
 *
 * void
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/
boolean _mgp_GnssLEReportingOff(void);
boolean mgp_GnssLEReportingOff(void);

/*
 ******************************************************************************
 * _mgp_SensorLEOn
 *
 * Function description:
 *
 *  Request to turn on Sensor LE
 *
 * Parameters: 
 *
 *  pz_SensorLEConfig - Pointer to Sensor LE configuration
 *
 * Return value: 
 *
 *  TRUE if the request is sent successfully, FALSE otherwise
 *
 ******************************************************************************
*/
boolean _mgp_SensorLEOn(const mgp_SensorLEConfigStructType  * const pz_SensorLEConfig);
boolean mgp_SensorLEOn(const mgp_SensorLEConfigStructType  * const pz_SensorLEConfig);

/*
 ******************************************************************************
 * _mgp_SensorLEOff
 *
 * Function description:
 *
 *  Request to turn off Sensor LE
 *
 * Parameters: 
 *
 * void
 *
 * Return value: 
 *
 *  TRUE if the request is sent successfully, FALSE otherwise
 *
 ******************************************************************************
*/
boolean _mgp_SensorLEOff(void);
boolean mgp_SensorLEOff(void);

/*
 ******************************************************************************
 * _mgp_SensorLEGetRelDisplacement
 *
 * Function description:
 *
 *  Request Relative Displacements from Sensor LE between two timestamps
 *
 * Parameters: 
 *
 * t_ModemTsFromMsec - Start timestamp of the relative displacement
 * t_ModemTsToMsec   - Stop timestamp of the relative displacement
 *
 * Return value: 
 *
 *  TRUE if the request is sent successfully, FALSE otherwise
 *
 ******************************************************************************
*/
boolean _mgp_SensorLEGetRelDisplacement(uint64 t_ModemTsFromMsec, 
                                       uint64 t_ModemTsToMsec);
boolean mgp_SensorLEGetRelDisplacement(uint64 t_ModemTsFromMsec, 
                                       uint64 t_ModemTsToMsec);

/*
 ******************************************************************************
 * _mgp_GpsMcFsTaskNotification
 *
 * Function description:
 *
 *  mgp_GpsMcFsTaskNotification allows the FS task to indicate FS task is complete
 *
 * Parameters:
 *
 *  pz_Msg - Pointer to FS acknowledgement data.
 *
 * Return value:
 *
 *  void
 *
 ******************************************************************************
*/

void _mgp_GpsMcFsTaskNotification( const mgp_GpsFsTaskNotificationDataType* pz_Msg );
void mgp_GpsMcFsTaskNotification( const mgp_GpsFsTaskNotificationDataType* pz_Msg );
  
  /*
  ******************************************************************************
  * mgp_GetEnergyConsumption
  *
  * Function description:
  *  This API returns the GNSS receiver energy consumption.
  *
  * Parameters:
  *  p_GetMECompleteActiveCfg - [OUT] Pointer to structure that contains
  *                                energy consumption parameters.
  *
  *  e_energyMode  - [IN] mode for which the energy consumption is required
  *
  * Return value:
  *  None.
  *
  ******************************************************************************
  */
  void mgp_GetEnergyConsumption (mgp_GetEnergyConsumptionType *p_GetEnergyConsumption,
                                    mgp_GnssEnergyMode e_energyMode);
  void _mgp_GetEnergyConsumption (mgp_GetEnergyConsumptionType *p_GetEnergyConsumption,
                                    mgp_GnssEnergyMode e_energyMode);

/*
 ******************************************************************************
 * mgp_MeInitAckStatus
 *
 * Function description:
 *
 *  MGP API call to acknowledge ME task init
 *
 * Parameters:
 * TRUE/FALSE: Ack status
 *
 * Return value: 
 *  None
 ******************************************************************************
*/
void mgp_MeInitAckStatus(boolean v_AckStatus);

/*
 ******************************************************************************
 * mgp_MeDeInitAckStatus
 *
* Function description:
*
 *  MGP API call to acknowledge ME task De-init
*
* Parameters:
 * TRUE/FALSE: Ack status
*
* Return value: 
*  None
******************************************************************************
*/
void mgp_MeDeInitAckStatus(boolean v_AckStatus);

/*
 ******************************************************************************
 * mgp_XtraIntegrityInject
 *
 * Function description:
 *
 * This function injects XTRA integrity data to MGP.
 *
 * Parameters:
 *   pz_XtraIntegrityData - Pointer to XTRA integrity data injected to MGP
 *
 * Return value:
 *
 *   void
 *
 * ******************************************************************************
*/
void mgp_XtraIntegrityInject(const gnss_XtraIntegrityIpcStructType* pz_XtraIntegrityData);
/*
 ******************************************************************************
 * _mgp_InitGnssDbOnMgpLoad
 *
 * Function description:
 *
 *  Request to initialize database on MGP load
 *
 * Parameters: 
 *
 * q_GnssConfig - Constellation config
 *
 * Return value: 
 *
 *  TRUE/FALSE
 *
 ******************************************************************************
*/
boolean _mgp_InitGnssDbOnMgpLoad(uint32 q_GnssConfig);
boolean mgp_InitGnssDbOnMgpLoad(uint32 q_GnssConfig);
/*
 ******************************************************************************
 * _mgp_DeInitMgpForUnload
 *
 * Function description:
 *
 *  Request to de-initialize database on MGP unload
 *
 * Parameters: 
 *
 * None
 *
 * Return value: 
 *
 * None
 *
 ******************************************************************************
*/
void _mgp_DeInitMgpForUnload(void);
void mgp_DeInitMgpForUnload(void);

/**
  @brief This function provides a mapping between the Source of
         the time from the TimeEstPut to the time source that is
         visible to the application.
  mgp_MapMgpToCommonTimeSource - Resident Api
  _mgp_MapMgpToCommonTimeSource - External Api

  @param[in] e_SourceID Source of the time estimate put.

  @retval u_TimeSource The mapped value of the time source.
*/
uint8 _mgp_MapMgpToCommonTimeSource(mgp_TimeEstPutSourceIDEnum e_SourceID);
uint8 mgp_MapMgpToCommonTimeSource(mgp_TimeEstPutSourceIDEnum e_SourceID);

#ifdef __cplusplus
}
#endif 
#endif /* MGP_API_H */
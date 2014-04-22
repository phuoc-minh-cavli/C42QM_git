#ifndef GTS_API_H
#define GTS_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Global Time Services Header File

GENERAL DESCRIPTION
This file contains API definitions between Global Time Service module and
other modules outside GNSS.

  Copyright (c) 2014 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2018 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //components/rel/gnss8.mpss/9.5.1.1/api/gts_api.h#5 $
$DateTime: 2020/12/21 00:31:38 $
$Author: pwbldsvc $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "comdef.h"
#include "sys_type.h"
#include "lte_as.h"
#include "qw.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

typedef enum  
{
  GTS_CLIENT_INVALID,
  GTS_CLIENT_GNSS_ME,
  GTS_CLIENT_CM,
  GTS_CLIENT_MAX = GTS_CLIENT_CM
} gts_ClientType;

typedef enum
{
  GTS_PULSE_CLIENT_INVALID,
  GTS_PULSE_CLIENT_GNSS_ME,
  GTS_PULSE_CLIENT_5G,
  GTS_PULSE_CLIENT_TEST_DEBUG,
  GTS_PULSE_CLIENT_MAX = GTS_PULSE_CLIENT_TEST_DEBUG
} gts_PulseClientType;

typedef enum
{
 GTS_GL1_STATE_OTHER          = 0,
 GTS_GL1_STATE_IDLE           = 30,
 GTS_GL1_STATE_GPRS_IDLE      = 31,
 GTS_GL1_STATE_DEDICATED      = 50,
 GTS_GL1_STATE_TRANSFER       = 51,
 GTS_GL1_STATE_DTM            = 52,
 GTS_GL1_STATE_MULTI_SIM_IDLE = 61
} gts_gl1_state_type;

/* Enums for selecting GPIO number for pulses */
typedef enum
{
  GTS_MDM9205_PULSE_GPIO_NUM_57 = 57, /*Default GPIO for 9205*/
  GTS_MDM9205_PULSE_GPIO_NUM_60 = 60,
} gts_PulseMdm9205GpioNumType;

typedef enum
{
  /* Default - No valid Time Source */
  GTS_TIME_SRC_NONE,
  
  /* WWAN Based Time */
  GTS_TIME_SRC_LTE,
  GTS_TIME_SRC_WCDMA,
  GTS_TIME_SRC_CDMA,
  GTS_TIME_SRC_GSM,
  GTS_TIME_SRC_TDSCDMA,
  GTS_TIME_SRC_LTE_SIB,
  GTS_TIME_SRC_LTE_FTA,  
  GTS_TIME_SRC_SCLK_TTR,  

  /* Time from GPS Satellites */
  GTS_TIME_SRC_GPS,
  
  /* Other Clock Source */
  GTS_TIME_SRC_FEST,
  GTS_TIME_SRC_PMIC,
  GTS_TIME_SRC_EXTINJ,
  GTS_TIME_SRC_CORESVC,
  GTS_TIME_SRC_CALIB,
  GTS_TIME_SRC_HSFN_SYNC_PMIC,
  GTS_TIME_SRC_OTHER,
  GTS_TIME_SRC_MAX = GTS_TIME_SRC_OTHER
} gts_TimeSrcType;

/* Reference Time structure for Timetags */
typedef struct 
{
  /* Qtime is Valid */
  boolean b_QtimeValid;
  /* Qtime or 64 bit Dal Time @ Time Estimate in units of 19.2 Mhz */
  uint64  t_Qtime;
  /* Bias if applicable in Qtime units */
  float   f_QtimeBias;
} gts_QtimeType;

typedef enum 
{
  GTS_LTE_TIME_RPT_PERIODIC,
  GTS_LTE_TIME_REQUESTED_VALID,
  GTS_LTE_TIME_REQUESTED_NOSIB,
  GTS_LTE_TIME_REQUESTED_ABORT,
  GTS_LTE_TIME_REQUESTED_ERROR
}
gts_LteTimeStatusType;

typedef enum 
{
  GTS_TRUE_TIME_SUCCESS,
  GTS_TRUE_TIME_DELAY_NOT_AVAILABLE,
  GTS_TRUE_TIME_INPUT_INVALID,
  GTS_TRUE_TIME_OTHER
}
gts_TrueTimeStatusType;

/* GTS will invalidate WWAN DB if WWAN state is OOS or INVALID.
   It will maintain TT only if WWAN state is IDLE/TRAFFIC. */
typedef enum
{
  GTS_WWAN_STATE_INVALID,  /* Default state */
  GTS_WWAN_STATE_OOS,      /* WWAN is going to unload */
  GTS_WWAN_STATE_IDLE,     /* WWAN is in idle state */
  GTS_WWAN_STATE_TRAFFIC,  /* WWAN is in connected state */
  GTS_WWAN_STATE_OTHER     /* WWAN is in RACH state */
}
gts_WwanStateInfoType;


/* LTE timing advance structure */
typedef struct
{
  /* Validity of timing advance data */
  boolean b_TaValid;

  /* Timing advance (Rx-Tx diff) in Ts units */
  int16 x_TaData;
  
  /* Serving cell physical ID. Range ~ 0 to 503 */
  uint16 w_ServPhyCellId;
}
gts_TaDataType;

typedef struct
{
  uint32  q_Ustmr[2];
  uint32  q_VsrcFromFW[2]; 
  uint32  q_VsrcPhFromFW[2]; 
  uint32  q_OstmrFromFW[2];
  uint32  q_OstmrPhFromFW[2];
  uint64  t_RefDlTime;
} gts_VsrcInfoType;

typedef struct 
{
  /* Request ID of the LTE Time Transfer request that triggered this report */
  uint8   u_ReqID;

  /* Report status - gts_LteTimeStatusType */
  uint8 e_RptStatus;

  uint32  q_Freq;    /* DL frequency - Same as lte_earfcn_t*/ 
  uint16  w_CellId;  /* Physical Cell ID */

  /* SIB8 TRUE: LTE NW is synchronized and supports 1x. FALSE: LTE NW is NOT synchronized */
  boolean b_Sib8Valid;   
  /* The CDMA2000 system time corresponding to the SFN boundary at or after the ending boundary of the SI-Window in which this Time Transfer was taken. The size is 52 bits and the unit is [CDMA2000 chips] */
  uint64  t_Sib8CdmaSystemTime; 
  
  boolean b_Sib16Valid; /* SIB16 System Time info*/
  uint64  t_Sib16GpsMsecs; /* The Abs GPS time at the SFN boundary in of 10 msecs */
  uint16  w_LeapSeconds; /* Number of leap seconds between UTC time and GPS time */
  
  uint16  w_Sfn;             /* System Frame Number at which data was collected */
  uint64  q_OStmrSfBoundary;  /* OSTMR at the boundary of the subframe */

  /* Latch Information */
  uint32  q_UstmrLatched;   /* USTMR Latched in 19.2 Mhz Units */
  uint64  q_OstmrLatched;   /* OSTMR Latched in 30.2 Mhz Units ( Ts ) */
  uint32  q_OstmrPhLatched; /* OSTMR Latched in 30.2 Mhz Units ( sub Ts )*/

  /* Best estimate of the current LTE system time uncertainty */
  uint32  q_SysTimeUncEst;

  /* VSRC Information for PCC */
  /* Mark if the next field is valid or not */
  boolean b_OstmrVsrcValid; 
  gts_VsrcInfoType z_RefVsrcInfo;  
  gts_VsrcInfoType z_CurrVsrcInfo;

  /* Timing advance data to predict SIB8 TTr uncertainity */
  gts_TaDataType z_TaData;

  /* Reference Time ( QTIME ) */
  gts_QtimeType  z_QtimeInfo;

  /* Subscription Information – For MultiSim Support.
   * Type - 'sys_modem_as_id_e_type' as defined in 'mmcp\api\public\sys.h' */
  uint8 u_SubInfo;

  /* LTE state. Of type 'gts_WwanStateInfoType' */
  uint8  u_LteState;
}gts_LteTimeInfoType; 

typedef struct
{
  /* Subscription Information – For MultiSim Support.
   * Type - 'sys_modem_as_id_e_type' as defined in 'mmcp\api\public\sys.h' */
  uint8 u_SubInfo;
  /* Cell Information */
  /* Base Station Identification code */
  uint8 u_BSIC;
  /* Control Channel Frequency */
  ARFCN_T z_Arfcn;

  /* Time Information */
  /* Frame Number 0..(26L*51L*2048L-1)  ~4.615ms frames */
  uint32 q_FrameNum;
  /* Time Slot Number 0..7 */
  uint8 u_TimeSlot;
  /* QSymbol Count (sub fn) */
  uint32 q_QSymCnt;

  /* GSM State. Of type 'gts_WwanStateInfoType' */
  uint8 u_GL1State;

  /* Additional Drift in the time in QSym Units (if any) */
  int8 b_DriftQSym;

  /* Reference Time ( QTIME ) */
  gts_QtimeType  z_QtimeInfo;
} gts_GsmTimeInfoType;


typedef struct
{
  /* Subscription Information – For MultiSim Support.
   * Type - 'sys_modem_as_id_e_type' as defined in 'mmcp\api\public\sys.h' */
  uint8 u_SubInfo;

  /* Cell Information */
  /* DL frequency and Primary scrambling code */
  uint16  w_Freq;
  uint16  w_PSC;

  /* Time Information */
  /* 10.3.6.75  System Frame Number  0..4095 10ms frames */
  uint16  w_SysFn;

  /* 0..307200 Chipx8 per frame */
  uint32  q_CX8Num;

  /* 24-bit XO value (USTMR) at SFN/Cx8 boundary */
  uint32 q_UstmrAtCx8;

  /* WCDMA State */
  uint8 u_WL1State;

  /* Reference Time ( QTIME ) */
  gts_QtimeType  z_QtimeInfo;
}gts_WcdmaTimeInfoType;


typedef struct
{
  /* Subscription Information – For MultiSim Support */
  uint8 u_SubInfo;
  /* Cell Information */
  /* DL frequency. */
  uint16  w_Freq;

  /* Cell Param ID */
  uint16  w_CellId;

  /* Time Information */
  /* 64-bits value in TDS chipx8 units at status dump, clocked @ 10.24 MHz */
  uint64  q_TdsTime;

  /* 30-bits value in TDS sub chipx8 units at status dump */
  uint32  q_TdsPhase;

  /* USTMR Value at status dump, clocked @ 19.2 Mhz */
  uint32 q_Ustmr;

  /* TDS L1 State 0 – Paging, 1- Traffic*/
  uint8 u_TDSL1State;

  /* Reference Time ( QTIME ) */
  gts_QtimeType  z_QtimeInfo;
}  gts_TdscdmaTimeInfoType;


/* Time Transfer Data Structure */
typedef struct
{
  /* 1x system time in qword format */
  qword   t_SystemTime;

  /* Current value of the slow clock counter */
  uint64  t_Qtime;

  /* CDMA RTC value latched at the last Sync80 tick (in CDMA Chipx8) */
  uint32  q_CdmaRtcSync80;

  /* Current 1x Ref PN offset */
  uint16  w_RefPN;

  /* Current 1x serving frequency band associated to Ref PN */
  uint8   u_Band;

  /* Current 1x serving frequency channel associated to Ref PN */
  uint16  w_Channel;

  /* Current master finger position, where master fingers is
   * assigned to Ref PN Offset */
  int32   l_MasterFingerPosition;

  /* Maximum Antenna Range */
  uint32  q_Mar;

  /* Best estimate of the current 1x system time uncertainty */
  uint32  q_SystemTimeUncertaintyEstimate;

  /* CDMA RTC value latched at the same time as GPS RTC (in CDMA Chipx32) */
  uint32  q_CdmaRtcLatched;

  /* CDMA RTC phase value latched at the last Sync80 tick (in CDMA Chipx2^35) */
  uint32  q_CdmaRtcSync80Phase;

  /* Universal STMR value latched at the same time as the first latched
     CDMA/GNSS RTC set (in 19.2MHz untis) */
  uint32  q_UnivStmrLatched;

  /* CDMA RTC STMR control register read at the same time as the first latched
     CDMA/GNSS RTC set (in CDMA Chipx2^35 units) */
  uint32  q_CdmaRtcFcw;

  /* Difference between CDMA RTC and VSRC STMR (in CDMA Chipx8192 units) */
  uint32  q_CdmaRtcVsrcStmrDiff;

  /* Reference Time ( QTIME ) */
  gts_QtimeType  z_QtimeInfo;

  /* CDMA state. Of type 'gts_WwanStateInfoType' */
  uint8  u_CdmaState;
}
gts_CdmaTimeInfoType;

typedef struct
{
  /* If the time below is valid or not */
  boolean b_Valid;

  #ifdef T_WINNT
  #error code not present
#else
  /* Time Source */
  gts_TimeSrcType e_TimeSrc; 
  #endif

  /* Absolute GPS Time related information */
  uint16  w_GpsWeek;     /* GPS week number at reference tick [weeks] */
  uint32  q_GpsMsec;     /* GPS msec at reference tick [msecs] */
  float   f_ClkTimeBias; /* Clock bias [msecs]. Constraint to >-0.5 and <0.5 */
  double  d_ClkTimeUncMs; /* 1 sided max time bias uncertainty [msecs] */
  float   f_ClkTimeUncMs;

  /* Reference Time ( QTIME ) */
  gts_QtimeType  z_QtimeInfo;
  
  /* Freq Error Information */
  boolean b_FreqEstValid;  /* Frequency Error Estimate is valid */
  int32 x_FreqEstErrPpb;   /* Freq Error Estimate in PPB (2^-10 PPM) */
  int32 x_FreqEstErrUncPpb;   /* Freq Error Estimate Unc in PPB (2^-10 PPM) */

  /* Velocity Information */
  boolean b_VelValid;
  float f_VelocityMs;      /* Velocity of the device in meter/sec */

  /* Position Information */
  boolean b_PosValid;
  float f_PosUnc;          /* Position Unc in meter/sec */
  
  /* Leap Second Info */
  boolean b_LeapSecValid;
  uint8 u_LeapSecond;      /* Leap Second value in Seconds */
  
  /* GPS Clock Specific Fields */
  boolean b_FCountValid; /* Mark if FCount below is valid */
  uint32  q_GpsFCount;    /* GPS FCount at the Time Estimate */
  boolean b_HighPriority; /* Mark if the time was after a time refresh */
} gts_TimeType;

/* Corrected PMIC error rate in PPM */
typedef struct
{
/* HSFN values and PMIC tick are populated from WWAN just used for debug purposes,only x_CorrectedPmicMsec is used to correct stored pmic time*/
  uint32 q_HsfnPsmEntry; /* HSFN value at PSM entry */
  uint16 w_SfnPsmEntry; /* SFN value at PSM entry*/
  uint64 t_PmicTimePsmEntryInMsec; /*Pmic tick at PSm entry*/
  uint32 q_HsfnPsmExit; /*HSFN value at PSM exit */
  uint64 t_PmicTimePsmExitInMsec; /* pmic tick at PSM exit */
  uint16 w_SfnPsmExit;/* SFN value at PSM exit */
  int32 x_CorrectedPmicMsec; /*absolute error correction in msec based on HSFNdiff during psm cycle */
} gts_PmicTimeCorrectionType;

/* Callback Function for recieving Time updates */
typedef void ( * gts_TimeEstCbType )( const gts_TimeType * );

#ifndef T_WINNT
/* Request Time Info structure */
typedef struct
{
  gts_ClientType e_ClientInfo;
  boolean b_ContUpdates;
  boolean b_HighPriority;
}gts_ReqTimeInfoType;
#else
#error code not present
#endif

/*--------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/
/*
 ******************************************************************************
 * Function gts_ReqGpsTime
 *
 * Description:
 *
 *  This function requests for the latest time from GTS module 
 *
 * Parameters:
 *
 *  pz_ReqTimeInfo - Pointer related to requesting time info like client info
 *  and continous updates
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if the request was sent successfully, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_ReqGpsTime( gts_ReqTimeInfoType* pz_ReqTimeInfo );

/*
 ******************************************************************************
 * Function gts_LteTimeUpdate
 *
 * Description:
 *
 *  This function sends current LTE Time to GTS module using IPC
 *
 * Parameters:
 *
 *  pz_LteTime - Pointer to the LTE Time structure 
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

void gts_LteTimeUpdate( gts_LteTimeInfoType* pz_LteTime );

/*
 ******************************************************************************
 * Function gts_GsmTimeUpdate
 *
 * Description:
 *
 *  This function sends current GSM Time to GTS module using IPC
 *
 * Parameters:
 *
 *  pz_GsmTime - Pointer to the GSM Time structure 
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

void gts_GsmTimeUpdate( const gts_GsmTimeInfoType* pz_GsmTime );

/*
 ******************************************************************************
 * Function gts_WcdmaTimeUpdate
 *
 * Description:
 *
 *  This function sends current WCDMA Time to GTS module using IPC
 *
 * Parameters:
 *
 *  pz_WcdmaTime - Pointer to the WCDMA Time structure 
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

void gts_WcdmaTimeUpdate( const gts_WcdmaTimeInfoType* pz_WcdmaTime );

/*
 ******************************************************************************
 * Function gts_TdscdmaTimeUpdate
 *
 * Description:
 *
 *  This function sends current TDS Time to GTS module using IPC
 *
 * Parameters:
 *
 *  pz_TdscdmaTime - Pointer to the TDS Time structure 
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

void gts_TdscdmaTimeUpdate( const gts_TdscdmaTimeInfoType* pz_TdscdmaTime );

/*
 ******************************************************************************
 * Function gts_RegForGtsTime
 *
 * Description:
 *
 *  This function allows clients to register a callback function to GTS for
 *  sending Time updates
 *
 * Parameters:
 *
 *  e_ClientInfo - Information of which client is registering
 *  p_CbFunction - Callback function which will be invoked by GTS to send
 *  time information to the client
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if registration was successful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_RegForGtsTime( gts_ClientType e_ClientInfo, gts_TimeEstCbType p_CbFunction ); 

/*
 ******************************************************************************
 * gts_GetQtime
 *
 * Function description:
 *
 * This function returns the Qtime @ the calling instant
 *
 * Parameters:
 *  pz_Qtime - Qtime structure ( Validity,Time and Bias ) is updated
 *
 * Return value:
 *  None, but callers should check pz_Qtime->b_QtimeValid which will be TRUE 
 *  if the get time was successful, FALSE otherwise
 *
 ******************************************************************************
*/
void gts_GetQtime ( gts_QtimeType* pz_Qtime );

/*
 ******************************************************************************
 * Function gts_LteTtrUpdate
 *
 * Description:
 *
 *  This function sends current LTE Time to GTS module using IPC
 *
 * Parameters:
 *
 *  p_TTRpt - Pointer to the LTE Time structure 
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

void gts_LteTtrUpdate( gts_LteTimeInfoType * p_TTRpt );

/*
 ******************************************************************************
 * Function gts_CdmaTtrUpdate
 *
 * Description:
 *
 *  This function sends current CDMA Time to GTS module using IPC
 *
 * Parameters:
 *
 *  p_TTRpt - Pointer to the CDMA Time structure 
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
void gts_CdmaTtrUpdate( gts_CdmaTimeInfoType * p_TTRpt );

/*
 ******************************************************************************
 * Function gts_ConvertGtsTimeToNs
 *
 * Description:
 *
 *  This function takes GTS Time as input and provides the output in nanoseconds
 *
 * Parameters:
 *
 *  p_GtsTime - Pointer to the GTS Time structure 
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  GPS Time in Ns units (0 if there is any error)
 *
 ******************************************************************************
*/
uint64 gts_ConvertGtsTimeToNs( gts_TimeType * p_GtsTime );

/*
 ******************************************************************************
 * Function gts_ConvGtsTimeToTrueNs
 *
 * Description:
 *
 *  This function calculates the True Time in Ns using the GTS Time that was 
 *  provided. True time is the GPS Time at the Antenna port. So the function
 *  corrects for the patch Delay between the Antenna port and Sample Server
 *
 * Parameters:
 *
 *  pz_GtsTime - Pointer to the GTS Time structure that has to be converted
 *  p_GpsTimeNs - GPS Time in Ns (since Jan 6, 1980)
 *  p_GpsTimeUncNs - Unc/Error Associated with the above Time in Ns
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  GTS_TRUE_TIME_SUCCESS - True time was successfully computed
 *  GTS_TRUE_TIME_DELAY_NOT_AVAILABLE - Time was converted, but delay was not
 *  available
 *  GTS_TRUE_TIME_INPUT_INVALID - GTS Time provided as input had some error
 *  GTS_TRUE_TIME_OTHER - Unknown error
 *
 ******************************************************************************
*/
gts_TrueTimeStatusType gts_ConvGtsTimeToTrueNs( const gts_TimeType * pz_GtsTime, 
                                                uint64 *p_TimeNs, 
                                                uint32 *p_TimeUncNs );

/*
 ******************************************************************************
 * Function gts_GetTime
 *
 * Description:
 *
 *  Get the current GTS/GPS Time in Wk, Ms, Bias and UNC format. GTS Time is the 
 *  Numpber of Millisec from Jan 6,1980. The Time is mapped to the current 
 *  QTimer which is the XO Counter Value. In cases where GTS Time is not available
 *  just the QTimer filed in the structre will be updated and the function will
 *  return FALSE indicating GTS Time is valid or not
 *
 * Parameters:
 *
 *  pz_GtsTime - Pointer to the GTS Time Estimate
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
boolean gts_GetTime( gts_TimeType* pz_GtsTime );

/*
 ******************************************************************************
 *
 * Function
 *   gts_TimeDiffMsec
 *
 * Description:
 *  Calculate difference between two GTS Time values ( T2-T1 ) either using GTS
 *  time or if it is not valid then using QTime. Although the resolution is ms,  
 *  accuracy depends on various factors. So if the time diff is more than 5 min,
 *  then it can have an error of upto 1 ms
 *
 * Parameters:
 *  pz_T2 - GTS Time at Instance 2
 *  pz_T1 - GTS Time at Instance 1
 *  px_Diff - Difference value in Ms is updated into this pointer
 *
 * Return value:
 *  TRUE if the calculation was successful, FALSE otherwise
 *
  ******************************************************************************
*/
boolean gts_TimeDiffMsec( const gts_TimeType* pz_T2,
                          const gts_TimeType* pz_T1,
                          int32* px_Diff);

/*
 ******************************************************************************
 *
 * Function
 *   gts_InjectAbsTime
 *
 * Description:
 *  API used by external modules to inject absolute time (in GPS time format) into GTS.
 *  The modules could be external time injection from XTRA/NTP, PMIC based propogation etc.
 *
 * Parameters:
 *  p_AbsTime - GTS Time from other module
 *
 * Return value:
 *  TRUE if IPC send was sucessful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_InjectAbsTime( const gts_TimeType *p_AbsTime );

/*
 ******************************************************************************
 *
 * Function
 *  gts_ProvideTimeCorrectionOnPsmExit
 *
 * Description:
 *  CallBack function used to provide Corrected Pmic Error rate after HSFN syncronisation
 *
 * Parameters:
 *  pz_PmicTimeType - Corrected Pmic Time Type
 *
 * Return value:
 *  None
 *
 ******************************************************************************
*/
boolean gts_ProvideTimeCorrectionOnPsmExit( gts_PmicTimeCorrectionType *pz_PmicTimeType);

#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* GTS_API_H */


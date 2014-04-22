#ifndef GTS_LOC_API_H
#define GTS_LOC_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Global Time Services Location Tech Header File

GENERAL DESCRIPTION
This file contains API definitions between Global Time Service module between
other Location Tech Modules.

  Copyright (c) 2014 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2018 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2018 - 2022 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
  
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/wwanme/gts/api/gts_loc_api.h#3 $
$DateTime: 2022/09/27 06:31:11 $
$Author: pwbldsvc $

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*--------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "gts_api.h"
#include "gnss_common.h"
#include "mgp_api.h"
#include "lm_api.h"

/*--------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -----------------------------------------------------------------------*/

#define GTS_TTR_LATCH_REG_API_MAX_SIZE    20

/* USTMR Constants. Mask to extract the least 24-bits */
#define GTS_USTMR_MASK                   (0xFFFFFF)
/* Macro to mask LSB of 8 byte */
#define GTS_UINT64_LSB_BIT_MASK           (uint64)(0xFFFFFFFFFFFFFFFE)

/* Ms to USTMR conversion */
#define GTS_USTMR_PER_MS           19200

/* Num of periodic 20ms triggers from CC to generate 100ms trigger for GTS */
#define GTS_100MS_PERIODIC_TRIGGER   5


/*   MIN_UTC_TIME_UNC_MS is based on the expected worst case uncertainty of the delta betwen UTC and GPS time over the next 10 years.2.5 seconds is based upon historical data  */
#define MIN_UTC_TIME_UNC_MS_DBL   2500.0

/*--------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
/* GTS Freq Est Structure */
typedef struct
{
  /* Time at the last temp Update */
  U64 t_PrevQtime;    
  /* Time at the current temp Update */
  U64 t_CurrQtime;    
  /* Freq Est based on Temp in 2^10 PPB */
  S32  l_FreqEst;      
  /* Time Uncertainity estimate based on temp table in Ms/Second */
  FLT  f_FreqEstUncMs; 
  /* FT Calibration state */
  U8  u_FtCalState;
} gts_FreqEstInfoType;

/* GTS Freq Est Structure */
typedef struct
{
  boolean b_Enable;
} gts_HATStatusMsgType;

/* Time transfer register latches sent by CC/NavRx to GTS */
typedef struct
{
  /* WTR_STMR_STATUS1_RD in the register set. Register index #72 in 128 register set sent
   * by WTR to CC via Qlink.
   * When the time latching mechanism is triggered, this register holds the captured value of
   * the GNSS_RTC count. The GNSS_RTC mapping for Frame/Chip/Sample Counts is illustrated in 
   * RC_GNSS_RTC_STATUS register */
  U32 q_GnssRtc;

  /* WTR_STMR_STATUS2_RD in the register set. Register index #73 in 128 register set sent
   * by WTR to CC via Qlink.
   * When the time latching mechanism is triggered, the internal GNSS sample phase register 
   * is captured. Note: Sample phase = 0 when resampler is not enabled */
  U32 q_GnssRtcPhase;

  /* WTR_STMR_STATUS0_RD in the register set. Register index #71 in 128 register set sent
   * by WTR to CC via Qlink.
   * When the time latching mechanism is triggered, this register holds the captured value 
   * of the WTR STMR_RTC count */
  U32 q_Ustmr;

  /* WTR_STMR_STATUS4_RD in the register set. Register index #75 in 128 register set sent
   * by WTR to CC via Qlink.
   * Raw free running WTR STMR counter */
  U32 q_RawUstmr;

  /* WTR_STMR_STATUS3_RD in the register set. Register index #74 in 128 register set sent
   * by WTR to CC via Qlink.
   * When the time latching mechanism is triggered, the internal GNSS skip sample register 
   * is captured. Note: Skip Sample = 0 when resampler is not enabled. */
  U32 q_SampleSkip;
}
gts_TtrLatchRegSetType;

typedef struct
{
  /* Number of valid register sets sent by CC/NavRx */
  U8 e_LatchType;  /* 0 - EMEM,1 - IMEM */
  U16 w_Cnt;
  gts_TtrLatchRegSetType z_Latch[GTS_TTR_LATCH_REG_API_MAX_SIZE];
}
gts_TtrRegLatchInfoType;

/* Structure of latched registers for time transfer stored in GTS DB */
typedef struct
{
  gts_TtrLatchRegSetType z_Latch;
  U64 t_Ustmr64bit;
  boolean b_Valid;
} gts_TtrRegDbElemType;

/* Time Transfer Request */
typedef struct
{
  boolean b_HighPriority;
} gts_ReqTtrType;


/* LTE Time Transfer Request Params */
typedef struct
{
  boolean b_HighPriority;
  boolean b_Sib8Req;
  sys_modem_as_id_e_type e_SubId;
} gts_LteTtrReqType;

/* Structure to provide GPS RTC, USTMR and slow clock at a common time instance */
typedef struct
{
  boolean b_Valid;
  U64 t_Ustmr;
  U64 t_SlwClk;
  U32 q_GpsMsec;
  U32 q_GpsSubMsCx80;
  U32 q_GpsRtc;
} gts_CommonRtcAndClockTimeType;

/* Serving System Information */
typedef struct
{
  int8 b_SysMode;  /* Same as ENUM sys_sys_mode_e_type */
  int8 b_SrvStatus; /* same as ENUM sys_srv_status_e_type */
} gts_SrvSysInfoType;

typedef struct
{
  boolean b_Changed;
  gts_SrvSysInfoType z_SrvSysInfo;
}gts_SysStatusType;

typedef struct
{
  int8  b_SubId;    
  U8 u_NumStacks;
  gts_SysStatusType z_SysStatus;
}gts_SSInfoType; 

typedef struct
{
  boolean b_Valid;
  U64 t_Qtime;
  U64 t_SlowClk;
} gts_GetSlowClkType;

typedef struct
{
  int16 x_CcTickCnt;
} gts_PeriodicTriggerType;

/* Enum type to distinguish various modules that request GPS RTC and USTMR from GTS DB */
typedef enum
{
  GTS_SRC_GTS_GET_SLW_CLK_AND_FC,
  GTS_SRC_MC_PUT_SLW_CLK_EST,
  GTS_SRC_MC_GET_TICK_RTC,
  GTS_SRC_MC_TIMETAG_UPDATE,
  GTS_SRC_GTS_LATCH_GPS_CLK,
  GTS_SRC_GTS_LATCH_AT_SLW_CLK_BDRY,
  GTS_SRC_CGPS_GET_TIME_INFO,
  GTS_SRC_MC_LATCH_USTMR_TICK,
  GTS_SRC_LTE_TTR,
  GTS_SRC_MC_CLK_PUT_GPS,
  GTS_SRC_GTS_TIME_EST_UPDATE,
  GTS_SRC_TIMETICK_AT_FCOUNT,
  GTS_SRC_GPSFIXTIME_AT_FCOUNT,
  GTS_SRC_CLK_STOP_PROC,
  GTS_SRC_CALIB_PROC
}gts_GetRtcReqSrcType;

typedef enum
{
  GTS_QTIME_CLK_UNCALIBRATED,
  GTS_QTIME_CLK_CALIBRATED
}gts_QtimeClkCalStateType;

/* Enum type to destiguish various event that set or invalidate Qtime Calibration */
typedef enum
{
  GTS_QTIME_CAL_GTS_INIT,
  GTS_QTIME_CAL_GNSS_CLK_INIT,
  GTS_QTIME_CAL_CC_DSBLD_ERR_REC,
  GTS_QTIME_CAL_CC_DSBLD_CR_ERR_REC,
  GTS_QTIME_CAL_EXCEED_MAX_LIMIT_GET_OFFSET,
  GTS_QTIME_CAL_EXCEED_MAX_LIMIT_GET_CALIB_TIME,
  GTS_QTIME_CAL_EXCEED_MAX_LIMIT_LATCH1_VALID,
  GTS_QTIME_CAL_EXCEED_MAX_LIMIT_LATCH2_VALID,
  GTS_QTIME_CAL_AFTER_TWO_SV_FIX
}gts_QtimeClkCalStateReasonType;

typedef struct
{
  /* Information is valid or not */
  boolean b_Valid;

  /* GPS FCount */
  U32 q_GpsFCount;

  /* Time Tick (in Ms) */
  U64 t_TimeTickMs; 

  /* Time Tick Sub Ms (in Ms) */
  FLT f_TimeTickSubMs;

  /* Unc of Sub Ms */
  FLT f_TimeTickUncMs;

  /* QTime Information */
  gts_QtimeType z_Qtime;
}gts_TimeTickInfoType;

/* Data structure to provide pre-check information to GPS LM */
typedef struct
{
  /* TUNC in millisecs */
  boolean b_TuncValid;
  FLT f_ClkTimeUncMs;

  /* FUNC in PPM */
  boolean b_FuncValid;
  FLT f_FreqEstUncPpm;
} gts_PreCheckInfoType;

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/
/*
 ******************************************************************************
 * Function gts_GetGpsTimeEst
 *
 * Description:
 *
 *  This function reads the current time from GTS module. This time could be 
 *  from any source like XO or LTE or Timetag or Time transfer. 
 *
 * Parameters:
 *
 *  pz_GpsTimeEst - Pointer to the GPS Time Estimate from GTS
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
boolean gts_GetGpsTimeEst( gps_ClockStructType* pz_GpsTimeEst );

/*
 ******************************************************************************
 * Function gts_GetGtsTime
 *
 * Description:
 *
 *  This function reads the current time from GTS module.
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
boolean gts_GetGtsTime( gts_TimeType* pz_GtsTime );


/*
 ******************************************************************************
 * Function gts_PutGpsTimeEst
 *
 * Description:
 *
 *  This function creates a ME GPS Time estimate message and sends it GTS 
 *  module
 *
 * Parameters:
 *
 *  pz_GpsTimeEst - GPS Time Estimate information
 *  e_MESrc - ME Time source that provided this time
 *  pz_Qtime - Qtime at the Time Estimate instance
 *  q_FCount - FCount at the Time Estimate instance
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
void gts_PutGpsTimeEst( const gps_ClockStructType* pz_GpsTimeEst,
                        mgp_TimeEstPutSourceIDEnum e_MESrc,
                        const gts_QtimeType* pz_Qtime,
                        U32 q_FCount);

/*
 ******************************************************************************
 * Function gts_SetHighAccuracyMode
 *
 * Description:
 *
 *  This function Indicates that Background positioning is enabled and need  
 *  high accuracy time with a increased power consumption. This will enable
 *  Time maintenance using Slowclock and Temperature compensation
 *
 * Parameters:
 *
 *  b_HighAccuracy - TRUE/FALSE to enable/Disable high accuracy time tracking
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
void gts_SetHighAccuracyMode( boolean b_HighAccuracy );
/*
 ******************************************************************************
* Function gts_ExternalSourceInjection
*
* Description:
*
*  This function inject external injection time and tag current Qtime and sends it GTS
*  module
*
* Parameters:
*
*  gts_ExternalInjectionType - Exterenal source time information
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
void gts_ExternalSourceInjection(lm_request_slow_clk_time_inject_s_type *p_slow_clk_inject);

/*
 ******************************************************************************
 * Function gts_FreqEstUpdate
 *
 * Description:
 *
 *  Freq Est module will use this function to inject Freq Est and corresponding
 *  slow clock time to GTS
 *
 * Parameters:
 *
 *  pz_FreqEstInfo - Freq Estimate Information
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
void gts_FreqEstUpdate ( gts_FreqEstInfoType* pz_FreqEstInfo );

/*
 ******************************************************************************
 * Function gts_HATStatusUpdate
 *
 * Description:
 *
 *  Freq Est module will use this function to indicate GTS if HAT Enable/Disable
 *  was successful and what is the current status.
 *
 * Parameters:
 *
 *  b_Enable - Enable/Disable Status
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
void gts_HATStatusUpdate ( boolean b_Enable );

/*
 ******************************************************************************
 * Function gts_PutGpsPosVelUpdate
 *
 * Description:
 *
 *	Used by PE to update Position and Velocity information  
 *
 * Parameters:
 *
 *	pz_PosVelType - Structure with all Position/Velocity Information 
 *
 * Dependencies:
 *
 *	None
 *
 * Return value:
 *
 *	TRUE - If GTS Sources should be used by MC for regular GPS Sessions
 *
 ******************************************************************************
*/
void gts_PutGpsPosVelUpdate (gnss_PosVelSolutionStructType* pz_PosVelType);

/*
 ******************************************************************************
 * Function gts_MotionInfoUpdate
 *
 * Description:
 *
 *  SLIM will use this function to inject motion data into GTS
 *
 * Parameters:
 *
 *  pz_MotionData - Motion Data Information
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
void gts_MotionInfoUpdate ( const slimInjectMotionDataIndMsgT* pz_MotionData );

/*
 ******************************************************************************
 * Function gts_ForceUseGtsTime
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
boolean gts_ForceUseGtsTime ();

/*
 ******************************************************************************
 * Function gts_MapMESrcToGtsSrc
 *
 * Description:
 *
 *  ME Source into GTS Time source
 *
 * Parameters:
 *
 *  e_MESrc - ME Time Source Type
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  GTS Source Type
 *
 ******************************************************************************
*/
gts_TimeSrcType gts_MapMESrcToGtsSrc ( mgp_TimeEstPutSourceIDEnum e_MESrc );

/*
 ******************************************************************************
 * Function gts_MapGtsSrcToMESrc
 *
 * Description:
 *
 *  Converts GTS Source into ME Time source
 *
 * Parameters:
 *
 *  e_GtsSrc - GTS Time Source Type
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  ME Time Source Type
 *
 ******************************************************************************
*/
mgp_TimeEstPutSourceIDEnum gts_MapGtsSrcToMESrc( gts_TimeSrcType e_GtsSrc );

/*
 ******************************************************************************
 * Function gts_ProcDiagCmd
 *
 * Description:
 *
 *  Create and Send an IPC message to GTS module with the Diag command info 
 *
 * Parameters:
 *
 *  p_Data - Diag Command Packet Data
 *  u_Length - Length of the Diag Packet Data
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
void gts_ProcDiagCmd( U8* p_Data, U8 u_Length );

/*
 ******************************************************************************
 * Function gts_CalcUstmrAtFCount
 *
 * Description:
 *
 *  Generic Handler Function which will update Qtime at any provided FCount
 *
 * Parameters:
 *
 *  pz_Qtime - Qtime structure that will be updated
 *  q_FCount - Frame Count at which Qtime is estimated
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
void gts_CalcUstmrAtFCount( gts_QtimeType* pz_Qtime, U32 q_FCount );

/*
 ******************************************************************************
 * Function gts_SendTtrRegLatchInfo
 *
 * Description:
 *
 *  This function sends the latched time transfer registers. The registers are latched by WTR and
 *  sent to CC/NavRx via Qlink. CC/NavRx in-turn passes on the info to GTS. 20 Latch registers are sent
 *  at every PP tick. Latched registers are also sent during Rcvr ON and DPO wake-up
 *
 * Parameters:
 *
 *  pz_TtrLatch - Pointer to the latched registers
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
void gts_SendTtrRegLatchInfo( const gts_TtrRegLatchInfoType* pz_TtrLatch );

/*
 ******************************************************************************
 * Function gts_GetTtrLatch
 *
 * Description:
 *
 *  This Function will return the latest latched TTr register set
 *
 * Parameters:
 *
 *  p_TtrLatchRegSet - Pointer to provide the latched register values from DB.
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
void gts_GetTtrLatch( gts_TtrRegDbElemType *p_TtrRegSet, gts_GetRtcReqSrcType e_Src );

/*
 ******************************************************************************
 * Function gts_GetSlowClkAndFcount
 *
 * Description:
 *
 *  This Function will be called by MC module and it will return the slow clock boundary and the GNSS FCount/cx80
 *  at the slow clock boundary. The latest latched values of USTMR and GNSS RTC will be used to determine the slow
 *  clock value
 *
 * Parameters:
 *
 *  Pointers to provide the slow clock count, Fcount and Cx80
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
void gts_GetSlowClkAndFcount(U32 *p_SlowClkTickCount, U32 *p_FCount32, U32 *p_GnssChipx80);

/*
 ******************************************************************************
 * Function gts_GetClockAndRtcAtCommonBdry
 *
 * Description:
 *
 *  This Function will return the GPS RTC, USTMR and slow clock at a common time instance.
 *  The slow clock boundary will be used as a common time epoch for providing the data.
 *  The input for this function will be obtained from GTS TTr latch DB
 *
 * Parameters:
 *
 *  Pointers to provide the slow clock count, GPS RTC and USTMR.
 *  Module that is requesting for GPS RTC at slow clock boundary.
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
void gts_GetClockAndRtcAtCommonBdry( gts_CommonRtcAndClockTimeType *p_RtcAndClock, gts_GetRtcReqSrcType e_Src );

/*
 ******************************************************************************
 * Function gts_LatchAndStoreTickLatch
 *
 * Description:
 *
 *  This Function will Latch the GPS RTC, USTMR and FCount at a common time 
 *  and store this information inside the Calibration Database for GTS
 *  This function is called at Tick Proc from MC. 
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
void gts_LatchAndStoreTickLatch();

/*
 ******************************************************************************
 * Function gts_GetGtsTimeAndOffset
 *
 * Description:
 *
 *  This function reads the current time from GTS module and computes offset
 *  from last sleep cycle
 *
 * Parameters:
 *
 *  pz_GtsTime - Pointer to the GTS Time Estimate
 *  p_Offset - Offset field will be updated 
 *  p_GpsRtcDiffMs - GPS RTC diff Ms from the wakeup point to now
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
boolean gts_GetGtsTimeAndOffset( gts_TimeType* pz_GtsTime, DBL* p_Offset, 
                                 DBL* p_GpsRtcDiffMs);


/*
 ******************************************************************************
 * Function gts_GetCalibState
 *
 * Description:
 *
 *  This function is used to Get Calibration State
 *
 * Parameters:
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  boolean  - Whether the calibration is done or not
 *
 ******************************************************************************
*/
boolean gts_GetCalibState();

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
 * Function gts_TickProc
 *
 * Description:
 *
 *  This Function will Latch the GPS RTC, USTMR and FCount at a common time 
 *  and store this information inside the Calibration Database for GTS
 *  This function is called at Tick Proc from MC. 
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
void gts_TickProc();

/*
 ******************************************************************************
 *
 * Function:
 *
 *   gts_LteTtrProcRfDevCmd
 *
 * Description:
 *
 *  This function will set/reset the flag for enabling/disabling LTE RF dev test.
 *  Also increase the number of TT samples required to 8.
 *
 * Parameters:
 *
 *  Sub command
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_LteTtrProcRfDevCmd( U8 b_SubCmd );

/*
 ******************************************************************************
 * Function gts_ReqTtr
 *
 * Description:
 *
 *  This function requests Time Transfer with Priority. GTS will inturn request
 *  1X or LTE TTR based on the systems available
 *  
 * Parameters: 
 *
 *  Requested Time Transfer parameters (Priority)
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

void gts_ReqTtr( gts_ReqTtrType * p_ReqParams );

/*
 ******************************************************************************
 * Function gts_ForceTtr
 *
 * Description:
 * This functions forces CDMA time transfer in GTS. The function is called by 
 * MC when it wakes up from DPO sleep and finds that the slow clock goes out
 * of calibration. GTS enables Time transfer if it is not in a transition phase 
 * and also stops the Time transfer enable timer. 
 *
 * Parameters: 
 * 
 * None 
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

void gts_ForceTtr( void );

/*
 ******************************************************************************
 * Function gts_StartFTCal
 *
 * Description:
 * This functions restarts collecting Fast TCal Time Transfer information.
 *
 * Parameters: 
 * 
 * None 
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
void gts_StartFTCal( void );

/*
 ******************************************************************************
 * Function gts_StopFTCal
 *
 * Description:
 * This functions stops collecting Fast TCal Time Transfer information.
 *
 * Parameters: 
 * 
 * None 
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

void gts_StopFTCal( void );

/*
 ******************************************************************************
 * Function gts_CdmaSclkTTrUpdate
 *
 * Description:
 *
 *  This function is called by 1x SRCH task to provide a Slow Clock Time
 *  Transfer data packet to the GTS module.
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
void gts_CdmaSclkTTrUpdate( const cgps_1xSclkTTDataType * p_SclkTTData );

/*
 ******************************************************************************
 * Function gts_SSChange
 *
 * Description:
 *
 *  Alert GTS of a change in serving cell type
 *  
 * Parameters: 
 *
 *  p_SSInfo of CGPS_SSINFO_SRV_SYS - Contains the serving system type.
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

void gts_SSChange(const cgps_SSInfoType * const p_SSInfo);

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
 *  U8 - Reason for the deletion
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
void gts_DelTime(boolean b_ErrorRecovery);

/*
 ******************************************************************************
 * Function gts_TimeTickMsAtFCount
 *
 * Description:
 *
 *  This function returns the Time Tick Ms and Sub Ms Corresponding to an input
 *  FCount
 *
 * Parameters:
 *
 *  FCount Value
 *  Pointer to update : TimeTickMs and TimeTick SubMs
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
void gts_TimeTickMsAtFCount( U32 q_FCount, gts_TimeTickInfoType* pz_TimeTickInfo );

/*
 ******************************************************************************
 * Function gts_GetGtsTimeAtUstmr
 *
 * Description:
 *
 *  This function reads the current time from GTS module corresponding to a 
 *  particular USTMR value. This function is synchronously called by any LocSW
 *  task (to get GPS time rightaway in task's context). The calling module has 
 *  to make sure the USTMR that is being provided is close to the calling point
 *  by at the maximum 500 ms
 *
 * Parameters:
 *
 *  pz_GtsTime - Pointer to the GTS Time Estimate
 *  q_Ustmr - USTMR Count at which the GTS Time Estimate is requested at
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
boolean gts_GetGtsTimeAtUstmr( gts_TimeType* pz_GtsTime, uint32 q_Ustmr );

/*
 ******************************************************************************
 * Function:
 *
 *   gts_SendSibTimeRefreshMsg
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
void gts_SendSibTimeRefreshMsg( const int8 b_SubInfo );

/*
 ******************************************************************************
 * Function gts_DRSyncSetConfig
 *
 * Description:
 *
 *  This function sends an IPC Message to configure DRSync parameters
 *
 * Parameters:
 *
 *  p_ConfigMsg - DRSync Configuration Information
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if IPC send was successful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_DRSyncSetConfig( const gnss_DRSyncConfigType *p_ConfigMsg );

/*
 ******************************************************************************
 * Function gts_DRSyncGetConfig
 *
 * Description:
 *
 *  This function sends an IPC Message to retrieve DRSync parameters
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
 *  TRUE if IPC send was successful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_DRSyncGetConfig();

/*
 ******************************************************************************
 * Function:
 *
 *   gts_DRSyncTrigger
 *
 * Description:
 *
 *  Processes Trigger for DRSync. This will be invoked from CC every 
 *  20 Ms, So the processing done here should be minimal.
 *
 * Parameters:
 *
 *  x_TickCnt - CC Tick Count
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void gts_DRSyncTrigger(S16 x_TickCnt);

/*
 ******************************************************************************
 * Function gts_DRSyncSendTrigger
 *
 * Description:
 *
 *  This function sends an IPC Message to trigger DRSync Processing
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
 *  TRUE if IPC send was successful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_DRSyncSendTrigger();

/*
 ******************************************************************************
 * Function gts_DRSyncSendArm
 *
 * Description:
 *
 *  This function sends an IPC Message to trigger DRSync Arming
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
 *  TRUE if IPC send was successful, FALSE otherwise
 *
 ******************************************************************************
*/
boolean gts_DRSyncSendArm();

/*
 ******************************************************************************
 * Function gts_LeapSecUpdate
 *
 * Description:
 *
 *  This function sends an IPC Message to update Leap Second
 *
 * Parameters:
 *
*  pz_LeapSecInfo - Leap Sec Info
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
F *
 ******************************************************************************
*/
void gts_LeapSecUpdate(const mgp_LeapSecondInfoStructType* pz_LeapSecInfo);

/*
 ******************************************************************************
 * Function gts_ForceUseGtsQtimeCalib
 *
 * Description:
 *
 *  Used by MC to determine if it should use GTS QTIME Calib  instead of 
 *  Slow Clock Calib
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
 *  TRUE - If GTS QtimeCalib should be used by MC for regular GPS Sessions
 *
 ******************************************************************************
*/
boolean gts_ForceUseGtsQtimeCalib ();

/*
 ******************************************************************************
 * Function gts_ForceUseGtsExtInjSrc
 *
 * Description:
 *
 *  Used by MC to determine if it should use GTS Time Sources instead of 
 *  Slow Clock Time Sources
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
 *  TRUE - If GTS Sources should be used by MC for regular GPS Sessions
 *
 ******************************************************************************
*/
boolean gts_ForceUseGtsExtInjSrc ();

/*
 ******************************************************************************
 * Function gts_PutGpsPosVelUpdate
 *
 * Description:
 *
 *	Used by PE to update Position and Velocity information  
 *
 * Parameters:
 *
 *	pz_PosVelType - Structure with all Position/Velocity Information 
 *
 * Dependencies:
 *
 *	None
 *
 * Return value:
 *
 *	TRUE - If GTS Sources should be used by MC for regular GPS Sessions
 *
 ******************************************************************************
*/
void gts_PutGpsPosVelUpdate (gnss_PosVelSolutionStructType* pz_PosVelType);

/*
 ******************************************************************************
 * Function gts_SlowClockUpdate
 *
 * Description:
 *
 *  This function creates a Slow Clk Time estimate message and sends it GTS 
 *  module
 *
 * Parameters:
 *
 *  e_Src - Slow Clk Time source that provided this time
 *  pz_GpsTimeEst - GPS Time Estimate information
 *  pz_Qtime - Qtime at the Time Estimate instance
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
void gts_SlowClkTimeUpdate( U8 u_SlwClkSrc,
                            gps_TimeStructType *pz_GpsTimeEst,
                            gts_QtimeType* pz_Qtime);

/*
 ******************************************************************************
 * Function gts_GetCurrentSlwClkTick
 *
 * Description:
 *
 *  This function returns the current slow clock tick value
 *
 * Parameters:
 *
 *  Pointer to return curent slow clock and current USTMR value
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
void gts_GetCurrentSlwClkTick( gts_GetSlowClkType *pz_Clk );


/* *********************************************************************************
 * Function: gts_GetPreCheckSystemUnc
 *
 * Description:
 *
 *  This function will be called by GPS Location Manager (LM) in IOT PLs to get the best GPS system
 *  uncertainties (TUNC and FUNC). This will be used by LM as a pre-check before loading GPS MGP into memory
 *  
 * Parameters: 
 *
 *  Pointer to return pre-check data
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
void gts_GetPreCheckSystemUnc(gts_PreCheckInfoType *p_PreCheckData);

#ifdef __cplusplus
}
#endif
#endif /* GTS_LOC_API_H */

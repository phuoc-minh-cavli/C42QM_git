#ifndef XOFESTRM_UTILS_H
#define XOFESTRM_UTILS_H

/*============================================================================
  FILE:         xofestrm_utils.h

  OVERVIEW:     This file contains the definitions for XO Resident Module Processing.

  DEPENDENCIES: None
  Copyright (c) 2018 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/wwanme/xofestrm/inc/xofestrm_utils.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-08-02  rb  Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "tcxomgr.h"
#include "tcxomgr_cal.h" 
#include "log.h"
#include "aries_os_api.h"
#include "gts_loc_api.h"
#include "wwanme_common_defs.h"
#include "xofestrm_proc.h"



/**
 * This function provides the index of the oldest entry in the database.
 * 
 * @param u_CurrentIndex uint8 - Current Index
 * @param uint8 - Oldest index
 */

uint8 xofestrm_GetOldestIndex(uint8 u_CurrentIndex);

#ifndef MAX
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#endif

#ifndef ABS_XOFESTRM
#define ABS_XOFESTRM(a)          (((a) < 0) ? (-(a)) : (a))
#endif

#define XOFESTRM_IS_AS_ID_VALID(as_id) ((as_id > -1) && (as_id < 3))

#ifndef XOFESTRM_MIN
#define XOFESTRM_MIN(a, b)       ((a) < (b) ? (a) : (b))
#endif

int32 xofestrm_findRpushClientPriority(tcxomgr_client_id_type e_ClientId, tcxomgr_client_state_type e_ClientState);

typedef float FLT;

/* Max number of entries in Freq Est log packet */
#define TMFE_LOG_FREQ_EST_MAX_ENTRIES  10
#define TMFE_FREQ_EST_LOG_VERSION      1

/* Rpush information - input to Freq Estimator */
typedef PACK(struct)
{
  uint16 w_RotTime;         /* Time of rpush */
  int16 x_RotValue;        /* Rpush value */
  tcxomgr_client_id_type e_RotClientId;       /* Rpush client */
  tcxomgr_client_state_type e_RotClientState; /* Rpush client state */
  tcxomgr_temp_type l_XoTemp;          /* XO temp at rpush */
}
xofestrm_log_freq_est_rpush_struct_type;


/* Freq Est data */
typedef PACK(struct)
{
  uint32 q_DeltaTimeMs;    /* Time since log packet start */
  uint32 q_TimeElapsedMs;       /* Time in ms since start of estimator */
  tcxomgr_temp_type l_XoTemp;       /* XO Temperature */
  tcxomgr_temp_type l_PmicTemp;     /* PMIC temperature */
  int32 l_ThermAccum;   /* Thermal Accum */
  int32 l_TotalAccum;   /* Total accum after RFE correction */
  int32 l_Rfe;           /* Residual Freq Error */
  int32 l_RfeSlope;     /* RFE slope */
  int32 l_TotalSlope;   /* Total sloep = RFE Slope + thermal slope */
}
xofestrm_log_freq_est_data_struct_type;

/* Log header */
typedef PACK(struct)
{
  time_type ts;                  /* Timestamp of log */
  uint8 u_NumFreqEstData;   /* Num of freq_est data entries */
  uint8 u_NumRpushData;      /* num of rpush entries */
  boolean v_IsRotValid;           /* num of rot samples */
}
xofestrm_log_freq_est_hdr_struct_type;

/* Complete information which is logged
   The rot samples if present are synchronous to freq_est data and are useful
   for debugging the performance of the estimator loop */
typedef PACK(struct)
{
  xofestrm_log_freq_est_hdr_struct_type z_FreqEstHdr;
  xofestrm_log_freq_est_data_struct_type z_FeData[TMFE_LOG_FREQ_EST_MAX_ENTRIES];
  xofestrm_log_freq_est_rpush_struct_type z_RpushData[TMFE_LOG_FREQ_EST_MAX_ENTRIES];
  int16 x_Rot[TMFE_LOG_FREQ_EST_MAX_ENTRIES];
  timetick_type z_TimeTickAtLogStart;
}
xofestrm_log_freq_est_struct_type;

/*--------------------------------------------------------------------------
                    FREQ EST LOG PACKET INFO
--------------------------------------------------------------------------*/
/* seems below macro deprecated recently. keeping this for legacy reasons*/
LOG_RECORD_DEFINE(LOG_XO_FREQ_EST_C)  
uint8 version; /* The version number of the packet. */
xofestrm_log_freq_est_struct_type z_FestLogDatarmtemp;
LOG_RECORD_END  

#define GPS_IS_AS_ID_VALID(as_id) ((as_id > -1) && (as_id < 3))

/**
 * @brief addRpushToLogPacket API adds the provided Rpush 
 *        data to the 13D1 log packet structure.
 * 
 * @param z_RpushData 
 */
void xofestrm_addRpushToLogPacket(const         cgps_FreqEstimateDataType* z_RpushData);

/**
 * @brief addDataToLogPacket API adds the provided XO/PMIC Temp data to the
 *		  0x13D1 log packet structure. Also, inits/flushes the
 *		  log packet if needed. 
 *
 * @param t_CurrTick Current time tick
 * @param l_RotValue RPUSH value to be logged 
 * @param v_IsRotValid is the rotator value valid 
 *	
 * @return None
 */
void xofestrm_addDataToLogPacket
(
  const uint64 t_CurrTick,
  const tcxomgr_rotator_type l_RotValue,
  const boolean v_IsRotValid
);

/**
 * @brief flushLogPacket API clears 0x13D1. 
 *
 * @param None
 * @return None
 */
void xofestrm_flushLogPacket(void);

/*** @brief This method initializes the 13D1 log packet data.
 *
 * @param None
 * @return None
 */
void xofestrm_initLogPacket(void);

/* Tick related #defineds were introduced for use by slow clk and its APIs.
 * Ticks per second is defined at compile time in /services/time/timetick.h
 */
inline boolean xofestrm_getSlowClockTick(   uint64 *pt_TimeTick);


/*
 ******************************************************************************
 * Function xofestrm_TimeTickDiffMs
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
 *  The time diff in milli-seconds
 *
 ******************************************************************************
*/

int64 xofestrm_TimeTickDiffMs( uint64 t_StartTime, uint64 t_EndTime );


#endif


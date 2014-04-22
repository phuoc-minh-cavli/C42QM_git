#ifndef XOFESTRM_PROC_H
#define XOFESTRM_PROC_H

/*============================================================================
  FILE:         xofestrm_proc.h

  OVERVIEW:     This file contains the definitions for XO Fest Processing.

  DEPENDENCIES: None
  Copyright (c) 2018-2022 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/wwanme/xofestrm/inc/xofestrm_proc.h#2 $
  $DateTime: 2022/01/12 05:54:02 $
  $Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-08-02  rb  Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "aries_os_api.h"
#include "xofestrm_api.h"
#include "xofestrm_data.h"
#include "gts_loc_api.h"
#include "wwanme_common_defs.h"
#include "xofestrm_utils.h"
#include "tcxomgr_cal.h" 
#include "gps_common.h"
#include "log.h"



typedef struct
{
  /* temperature in degrees C * 1000 */
  tcxomgr_temp_set_type tset;

  /* Time in ms since start of estimator loop */
  uint32 time;

  /* Time of rotator reading for this db entry */
  uint32 rot_time;

  /* Residual frequency error */
  int32 res_err;

  /* FT Curve val */
  int32 ft_val;
  
  /* Slow clock timetick value for this db entry */
  timetick_type timetick;
}
xofestrm_db_entry_type;


typedef struct
{
  /* The client to do a rotator push */
  tcxomgr_client_id_type rot_client;

  /* The client's state to do a rotator push */
  tcxomgr_client_state_type rot_client_state;

  /* To store the subscription ID */
  tcxomgr_as_id_enum_type rot_as_id;

  /* The rotator value from rotator push in 2^-20 ppm */
  int32 rot_value;

  /* The timetick at which the rotator push was performed */
  timetick_type rot_tick;

  /* Priority of the Rpush Client */
  int32 priority;

  /* The temperature at which the rotator push was performed */
  tcxomgr_temp_type rot_temp;

} xofestrm_rpush_info_type;


/* To maintain array of client_states to process state_transitions */
typedef struct
{
  /* client_state being maintained for state_change */
  tcxomgr_client_state_type client_state;

  /* The timetick at which latest state was updated */
  timetick_type tick;

  /* Subscription ID of the most recent client to do a rotator push */
  tcxomgr_as_id_enum_type rot_as_id;

}xofestrm_client_states;


/* RPush priority order */
typedef enum
{
  XOFESTRM_DEFAULT_PRIORITY = -1,
  XOFESTRM_GSM_IDLE,
  XOFESTRM_HDR_IDLE,
  XOFESTRM_WCDMA_IDLE,
  XOFESTRM_TDSCDMA_IDLE,
  XOFESTRM_1X_IDLE,
  XOFESTRM_LTE_IDLE,
  XOFESTRM_NUM_OF_RPUSH_CLIENTS,
  XOFESTRM_GSM_TRAFFIC = XOFESTRM_NUM_OF_RPUSH_CLIENTS,
  XOFESTRM_HDR_TRAFFIC,
  XOFESTRM_WCDMA_TRAFFIC,
  XOFESTRM_TDSCDMA_TRAFFIC,
  XOFESTRM_1X_TRAFFIC,
  XOFESTRM_LTE_TRAFFIC
}
xofestrm_rpush_priority;


/* State transitions */
typedef enum
{
  XOFESTRM_UNKNOWN_STATE,
  XOFESTRM_IDLE_TO_TRAFFIC,
  XOFESTRM_TRAFFIC_TO_IDLE
}
xofestrm_state_change_type;

/* To identify which client actually changed state */
typedef struct
{
  /* Most recent client who's state changed */
  tcxomgr_client_id_type state_changed_client;

  /* Type of state change */
  xofestrm_state_change_type state_change;

}xofestrm_state_change;


/* Structure to read the NV 4955 data */
typedef struct
{
  int32 c1toc2_0;
  int32 c1toc2_1;
  int32 c1toc3_0;
  int32 c1toc3_1;
} xofestrm_nv_struct_type;

/* State transitions */
typedef enum
{
  XOFESTRM_MGP_UNKNOWN,
  XOFESTRM_MGP_LOADED,
  XOFESTRM_MGP_UNLOADED,
  XOFESTRM_MGP_MAX
}
xofestrm_mgp_state_type;


typedef struct
{
  /* keep track if table has been updated */
  xofestrm_mgp_state_type mgp_state;


  /* time/temp database */
  xofestrm_db_entry_type db[XOFESTRM_DB_SIZE];

  xofestrm_db_entry_type current_xofestrm_db_entry;

  uint32 no_of_temp_reads;

  mc_posFix_fee_type z_FreqBiasFuncData;
  mc_posFix_fee_type z_PeFixFreqData;

  boolean pe_func_available;

  /* pointer to the newest entry in the time/temp database */
  uint8 index;

  /* timetick at start of estimation cycle */
  uint32 start_tick;

  /* ms counter at start_tick */
  uint32 start_ms;

  /* AFC accumulator in 2^-20 ppm */
  int32 therm_accum[XOFESTRM_DB_SIZE];

  /* Outer accumulator in 2^-20 ppm */
  int32 outer_accum[XOFESTRM_DB_SIZE];

  /* Total slope 2^-30 ppm/msec*/
  int32 slope;

  /* Residual error slope 2^-30 ppm/msec */
  int32 res_err_slope;

  /* The most recent client to do a rotator push */
  tcxomgr_client_id_type rot_client;

  /* Subscription ID of the most recent client to do a rotator push */
  tcxomgr_as_id_enum_type rot_as_id;

  /* The most recent client's state to do a rotator push */
  tcxomgr_client_state_type rot_client_state;

  /* The last rotator value from rotator push in 2^-20 ppm */
  int32 rot_value;

  /* The timetick at which the last rotator push was performed */
  timetick_type rot_tick;

  /* The temperature at which the last rotator push was performed */
  tcxomgr_temp_type rot_temp;

  /* IF there is new rpush data or not */
  uint8 rot_flag;

  /* Difference between FT curve and therm_accum due to init from RGS
     or rotators */
  int32 therm_bias;

  /* FT coefficients */
  tcxomgr_cal_fac_data_type coeff;

  /* FT coefficients Update*/
  tcxomgr_cal_fac_data_type updatedcoeff;

  /* temperature in degrees C * 1024 */
  tcxomgr_temp_set_type current_tset;

  /* IS thermal compensation enabled or not */
  boolean tc_on;

  /* Unused rpush */
    xofestrm_rpush_info_type unused_rpush; 

  /* Array to store all the incoming RPush for priority processing */
  xofestrm_rpush_info_type rpush_array[TCXOMGR_NUM_CLIENTS];
  xofestrm_rpush_info_type current_rpush;
  cgps_FreqEstimateDataType z_rpushdata;
  
  /* To track the number of elements in the rpush_array */
  uint8 no_of_valid_rpush;

  /* To track state_changes */
  xofestrm_client_states rpush_client_states[TCXOMGR_NUM_CLIENTS];

  /* State Change Variable */
  xofestrm_state_change client_state_change;

  /* store the NV data */
  xofestrm_nv_struct_type nv;

  uint8 new_coeff_updated;

  boolean is_rpush_requested;

  boolean dbg_f3_enabled;

  boolean v_IsTemperatureReadAperiodic;

  /** Mutex to protect ref osc offset queries */
  os_MutexBlockType z_RefOscOffsetQueryMutex;
  
  /** Last temperature for which CC received an update */
  tcxomgr_temp_set_type last_cc_tset;
  
  uint8 u_TempChangeHystCount;
  
  boolean v_XOMicroImageQualifyState;

  uint8 u_FieldCalState;
  
  /** GTS specific datatypes */
  xofestrm_temp_type z_XoTempData;
  
  /** Mutex protected variables for CC to query */  
  uint32             q_RefOscUpdateTimestampForCC;
  int32              l_RefOscOffsetForCC;
  int32              l_RefOscOffsetRateForCC;
  
}
xofestrm_db_struct_type;


void xofestrm_ModuleInit(void);

void xofestrm_MessageProc ( const os_IpcMsgType* p_Msg );

/*
 ******************************************************************************
 * Function xofestrm_ProcXOMgrTempData
 *
 * Description:
 * Process temperature data from TCXO Manager.
 *
 * Parameters:
 * p_Msg - Pointer to the temperature data
 *
 * Dependencies:
 * None
 *
 * Return value:
 * None
 *
 ******************************************************************************
*/
 void  xofestrm_ProcTempData( cgps_FreqEstimateTempDataType* temp_data);

/*
 ******************************************************************************
 * Function xofestrm_ProcCalCoeff
 *
 * Description:
 * Process updated calibration coefficient data from TCXO Manager.
 *
 * Parameters:
 * p_Msg - Pointer to the coefficient data
 *
 * Dependencies:
 * None
 *
 * Return value:
 * None
 *
 ******************************************************************************
*/

void xofestrm_ProcCalCoeff( tcxomgr_external_coeff_type* cal_coeff);

/****************** GTS specific functions *************/
/**
 * @brief xofestrm_sendFestToGts 
 *        sends the frequency estimate information  to GTS when 
 *        High Accuracy Time Maintenance Mode is enabled and if
 *        the field cal state is IFC or more.
 * @param None 
 * @return None 
 *
 ******************************************************************************
 */

void xofestrm_SendFestToGts();

/**
 * Description:: This API checks if the RPush information is usable.
 * 
 * @param q_CurrTimeTick - Current time
 * @return boolean
 */
boolean xofestrm_IsRpushValid(timetick_type q_CurrTimeTick);

/**
 * Description: This API is used to register or deregister the RPush callback
 * function with TCXOManager depending on the input boolean value.
 * 
 * @param id tcxomgr_client_id_type - Client doing the registration
 * @param register_callback boolean - Register/Deregister
 * @return None
 */
void xofestrm_RegisterForRpush
(
tcxomgr_client_id_type id,
boolean register_callback
);

/*
 ******************************************************************************
 * Function xofestrm_UpdateDb
 *
 * Description:
 * This function updates xofestrm_db structure 
 *
 * Parameters:   current tick
 *
 * Dependencies: None
 *
 * Return value: None
 *
 ******************************************************************************
*/
void xofestrm_UpdateDb( uint32 q_CurrTimeTick );

/*
 ******************************************************************************
 * Function xofestrm_ProcRpushData
 *
 * Description:
 * @brief processRpush processes the new RPUSH data. It 
 *        evaluates the priority of the RPUSH and stores it in
 *        the database for the next estimation cycle. 
 *  
 * @param pz_rpush_data Reference to the RPUSH structure
 ******************************************************************************
*/
void xofestrm_ProcRpushData(cgps_FreqEstimateDataType* pz_rpush_data);


void xofestrm_StoreFreqData(mc_posFix_fee_type* p_FreqBiasFuncData);
void xofestrm_ProcMgpLoad(boolean v_IsLoadEvent);
FLT xofestrm_EstimateFunc();


/**
 * @brief sendFreqEstLogToDiag sends the 13D1 log packet out if 
 *  the log packet is enabled.
 * @param None 
 * @return None 
 */
void xofestrm_sendFreqEstLogToDiag(void);


#endif


#ifndef XOFESTRM_API_H
#define XOFESTRM_API_H

/*============================================================================
  FILE:         xofestrm_api.h

  OVERVIEW:     This file contains the API definitions for XO Resident Module.

  DEPENDENCIES: None
  Copyright (c) 2018-2019 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/wwanme/xofestrm/api/xofestrm_api.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-08-02  rb  Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "gps_common.h"
#include "tcxomgr_cal.h"
#include "timetick.h"

/* GTS Specific data type */
typedef struct
{
  /* Last two XO temperature values */
  tcxomgr_temp_type xo[2];
  /* Time of last two XO temperature values */
  uint64 time[2];

}xofestrm_temp_type;

/*
 ******************************************************************************
 * Function xofestrm_Init
 *
 * Description: Initialization of XOFESTRM module .
 *
 * Parameters:    None
 *
 * Dependencies:  None
 *
 * Return value:  boolean
 *
 ******************************************************************************
*/
boolean xofestrm_Init(void);

/*
 ******************************************************************************
 * Function xofestrm_TemperatureUpdate
 *
 * Description:
 * xofestrm_TemperatureUpdate processes the Temp data recieved from 
 * TCXO Manager and updates the local copy.
 * Parameters: 
 *
 * Frequency Estimate Update structure.
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

void xofestrm_TemperatureUpdate(const cgps_FreqEstimateTempDataType * p_FreqEstimateTempData);


/**
 * @brief xofestrm_CalCoeffUpdate processes the field calibration values from 
 * TCXO Manager and updates the local copy.
 *  
 * @param cal_coeff New XO field cal coefficients 
 * @return None 
 */

void xofestrm_CalCoeffUpdate(tcxomgr_external_coeff_type *cal_coeff);

/*
 ******************************************************************************
 * Function xofestrm_RpushUpdate
 *
 * Description:
 *
 *  This function is indirectly called by TCXO Manager to send Frequency
 *  Estimate Update data (using Rotator Push interface).
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

void xofestrm_RpushUpdate ( const cgps_FreqEstimateDataType * p_FreqEstimateData );

/*
 ******************************************************************************
 * Function xofestrm_StoreFreqBiasAndFunc
 *
 * Description:
 *  This function processes the FreqBias & Func update
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

void xofestrm_StoreFreqBiasAndFunc( mc_posFix_fee_type* p_FreqBiasFuncData);

/*
 ******************************************************************************
 * Function: xofestrm_MgpLoadEvent
 *
 * Description:
 *  This function process MGP Load event (Load/Unload MGP)
 *
 * Parameters:
 *
 *  boolean: 
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

void xofestrm_MgpLoadEvent(boolean v_IsLoadEvent);

/*
 ******************************************************************************
 * Function: xofestrm_GetBestFunc
 *
 * Description:
 *  This function provides bestFUNC
 *
 * Parameters:
 *
 *  boolean: 
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  Func
 *
 ******************************************************************************
*/

FLT xofestrm_GetBestFunc();
/*
 ******************************************************************************
 * Function: xofestrm_GetStoredRpush
 *
 * Description:
 *  This function provides Rpush data
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

void xofestrm_GetStoredRpush(cgps_FreqEstimateDataType* p_RpushData );

/*
 ******************************************************************************
 * Function: xofestrm_GetStoredFuncdata
 *
 * Description:
 *  This function provides Func data
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
 *  boolean
 *
 ******************************************************************************
*/

boolean xofestrm_GetStoredFuncdata(mc_posFix_fee_type* p_PropAgedFuncFreqBiasData);

/*
 ******************************************************************************
 * Function: xofestrm_StoreGTSdata
 *
 * Description: API saves last GTS update from mc_feqest module
 *
 * Parameters: pointer to GTS specific data type
 *
 * Return value:  none
 *
 ******************************************************************************
*/
void xofestrm_StoreGTSdata(xofestrm_temp_type* p_XofestrmGtsData);

/*
 ******************************************************************************
 * Function: xofestrm_GetStoredGTSdata
 *
 * Description: API proivdes XOFESTRM update (last XO temp & time data) sent to GTS
 *
 * Parameters: pointer to GTS specific data type
 *
 * Return value:  none
 *
 ******************************************************************************
*/
void xofestrm_GetStoredGTSdata(xofestrm_temp_type* p_XofestrmGtsData);

#endif


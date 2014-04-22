/*! 
  @file
  ftm_common_data.c
 
  @brief
  This file contains Centralized Global Data used by Common FTM.
 
  @addtogroup RF_COMMON_FTM_DATA
  @{
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfa.mpss/3.10/rf/common/ftm/src/ftm_common_data.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/03/18   gk      fno-common enable-compiltion fix
05/21/14   cri     Merge Pin Cal
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
07/16/13   dj      Removed legacy Tx internal Cal support
03/29/13   cri     Moving the data module to rfcommon_data
05/11/12   aa      Changes for Code Maintainability and data access 
04/10/12   id      Add support for Tx Internal Device Cal params
08/12/11   aro     Added Cal timer data structure
06/20/11   aro     Added Calibration V3 Data structure and accessor function
06/20/11   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "ftm_common_data.h"
#include "ftm_msg.h"

ftm_common_data_type *ftm_common_data;
/*============================================================================*/
/*!
  @name Query Data

  @brief
  Functions to query the data from FTM data strucuture
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the address of the data container for Calibration V3

  @details
  This function is used by the FTM handlers to get/set calibration v3
  data structure.

  @return
  Address of calibration v3 Data Container.
*/
ftm_calibration_data_type*
ftm_common_data_get_cal_v3_ptr
(
  void
)
{
  return &(ftm_common_data->cal_data.cal_v3);
}/* ftm_common_data_get_cal_v3_ptr */




/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the address of the data container for enh_internal_cal Ptr

  @details
  This function is used by the FTM handlers to get/set enhanced internal cal
  data structure.

  @return
  Address of enh_internal_calibration Data Container.
*/
ftm_enh_internal_device_cal_data_type*
ftm_common_data_get_enh_internal_device_cal_ptr
(
  void
)
{
  return &(ftm_common_data->cal_data.enh_internal_device_cal_data);
}/* ftm_common_data_get_cal_v3_ptr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the address of the data container for Timer

  @details
  This function is used by the FTM handlers to get/set timer
  data structure.

  @return
  Address of calibration timer Data Container.
*/
ftm_common_timer_type*
ftm_common_data_get_timer_ptr
(
  void
)
{
  return &(ftm_common_data->cal_timer);
}/* ftm_common_data_get_timer_ptr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the address of the data container for FTM Timer

  @details
  This function is used by the FTM handlers to get/set the FTM Timer structure. 
  These data are used during Tx Sweep Calibration.

  @return
  Address of FTM Timer Data Container.
*/
ftm_common_proc_clock_type*
ftm_common_get_proc_clock_ptr
(
  void
)
{
  return &( ftm_common_data->proc_clock );
} /* ftm_common_get_proc_clock_ptr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the address of the data container for the calibration data module

  @details
  This function is used by the data module handlers to get the data module
  structure.

  @return
  Address of the data module structure.
*/
ftm_cal_data_module_type*
ftm_common_data_get_data_module_ptr
(
  void
)
{
  return &(ftm_common_data->cal_data.data_module);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the curr channel for cal

  @details
  This function is used by the FTM handlers to get the current chan used for calibration

  @return
  Current channel
*/
uint32
ftm_common_data_get_curr_channel(void)
{
  return (ftm_common_data->cal_data.cal_v3.status.curr_chan);
}/* ftm_common_data_get_curr_chan */


/*! @} */

/*! @} */

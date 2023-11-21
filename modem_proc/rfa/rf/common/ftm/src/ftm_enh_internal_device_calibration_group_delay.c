/*!
  @file
  ftm_enh_internal_device_calibration_group_delay.c

  @brief

*/
/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //components/rel/rfa.mpss/3.10/rf/common/ftm/src/ftm_enh_internal_device_calibration_group_delay.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 


12/15/13  xs     Initial Revision

==============================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN

#include "rfa_variation.h"
#include "comdef.h"
#include "ftm_common_timer.h"
#include "ftm_common_control.h"
#include "ftm_common_data.h"
#include "rfcommon_math.h"
#include "modem_mem.h"
#include "string.h"
#include "stringl.h" /* for memscpy */
#include "ftm_msg.h"
#include "ftm_common_enh_internal_device_cal.h"
//#include "rfm_internal.h"
#include "DALSys.h" 
#include "rfcommon_time_profile.h" 
#include "zlib.h"

#ifdef FEATURE_FACTORY_TESTMODE

/*!
  @name: ftm_enh_internal_cal_group_delay

  @brief: feedback group delay
  
 
  @details
  
  @param
  input: req_instr, payload instruction
  output: res_len:  cal result length
             group_delay_result: output of group delay cal NV
  
  @return
     success or fail
*/

boolean ftm_enh_internal_device_cal_group_delay
(
  void const *req_instr,
  uint16 *res_len,
  void *group_delay_result 
)
{
   boolean ret_val = TRUE;
   FTM_MSG_1(FTM_ERROR, "FB GROUP DELAY_CAL Not Supported Yet, Cmd: %s", req_instr);
   return ret_val;
}



#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

/*! @} */



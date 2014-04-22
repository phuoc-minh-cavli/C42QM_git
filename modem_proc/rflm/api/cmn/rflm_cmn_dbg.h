/*!
  @file
  rflm_cmn_dbg.h

  @brief
  RFLM common debug command handler
  
  @detail
 
*/

/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rflm.mpss/1.10/api/cmn/rflm_cmn_dbg.h#1 $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
2018-05-25   mns     CCS MMU Changes.
2015-10-19   sbo     Added new Dbg command to read debug file from EFS
2015-01-15   rca     THOR CCS M3 Benchmarking support for THOR
2014-09-23   rca     SCRUTINY task trig time run time enable
2014-08-21   rca     SCRUTINY mode run time enable
2014-02-03   jc      created

===========================================================================*/

#ifndef RFLM_CMN_DBG_H
#define RFLM_CMN_DBG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


/*===========================================================================

 Type Declarations

===========================================================================*/

/*! @brief RFLM diag command enums */

typedef enum
{
  RFLM_CMN_DBG_CMD__SET_WARNING_ACTION,
  RFLM_CMN_DBG_CMD__DUMP_WARNING_CNT,
  RFLM_CMN_DBG_CMD__RESET_WARNING_CNT,
  RFLM_CMN_DBG_CMD__ENABLE_SCRUTINY_TASK_MEMORY,
  RFLM_CMN_DBG_CMD__ENABLE_SCRUTINY_PROGRAM_MEMORY,
  RFLM_CMN_DBG_CMD__ENABLE_SCRUTINY_TASK_TRIG_TIME,
  RFLM_CMN_DBG_CMD__START_CCS_M3_BENCHMARKING,
  RFLM_CMN_DBG_CMD__POST_MORTEM_READ_EFS_FILE,
  RFLM_CMN_DBG_CMD__CCS_MMU_MEM_USAGE_ENABLE,
  RFLM_CMN_DBG_CMD__CCS_RFFE_GRFC_LOG_START,
  RFLM_CMN_DBG_CMD__CCS_RFFE_GRFC_LOG_UPDATE,
  RFLM_CMN_DBG_CMD__CCS_RFFE_GRFC_LOG_STOP
  
} rflm_cmn_dbg_cmd_e;


/*===========================================================================

 Function prototypes

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

extern 
void rflm_cmn_dbg_cmd_handler( uint16 param_arr[] );

#ifdef __cplusplus
}  // extern "C"
#endif 


#endif /* RFLM_CMN_DBG_H */

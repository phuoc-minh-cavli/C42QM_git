/*!
  @file
  rflm_ccs_rf_rffe_log.h

  @brief
  RF RFEE logging 

  @detail
  Provides API to program RF scripts during based on events
  */
/*============================================================================== 

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

  ==============================================================================*/ 
/*==============================================================================

  EDIT HISTORY FOR MODULE 

  This section contains comments describing changes made to the module. 
  Notice that changes are listed in reverse chronological order. 

  $Header: //components/rel/rfmodem_jolokia.mpss/1.10/api/ccs/rflm_ccs_rf_rffe_log.h#1 $ 
  $Date: 2022/06/10 $   
  when       who     what, where, why 
  --------   ---    ------------------------------------------------------------- 
  03/06/19   mns     qdss logging


  ==============================================================================*/

#ifndef RFLM_CCS_RF_RFFE_LOG_H
#define RFLM_CCS_RF_RFFE_LOG_H

/*===========================================================================

  INCLUDE FILES

  ===========================================================================*/
// move to .c file
//#include "rflm_ccs.h"


/*===========================================================================

  INTERNAL DEFINITIONS AND TYPES

  ===========================================================================*/

#ifdef __cplusplus
extern "C" 
{
#endif

typedef enum
{
  PING_EVT_MASK=0x01,
  PONG_EVT_MASK=0x02,
}rflm_ccs_logging_event_enums;


#define LOG_PING_PONG_BUF_SIZE_WORDS 516

#define NUM_LOG_CYCLES_DEFAULT 10
#define NUM_LOG_CYCLES_DEFAULT_BYTES NUM_LOG_CYCLES_DEFAULT * LOG_PING_PONG_BUF_SIZE_WORDS * NUM_WORDS_TO_BYTES

#define LOG_PING_OR_PONG_BUF_SIZE_WORDS (LOG_PING_PONG_BUF_SIZE_WORDS/2)
#define LOG_PING_OR_PONG_BUF_SIZE_BYTES LOG_PING_OR_PONG_BUF_SIZE_WORDS*NUM_WORDS_TO_BYTES

typedef enum
{
  RFLM_CCS_LOGGING_ENABLED               = 1,   /*!< Enables RFFE logging starightaway , when client triggers start via FTM command or EFS */
  RFLM_CCS_LOGGING_DISABLED              = 2,   /*!< Disables RFFE logging starightaway , when client triggers stop via FTM command or EFS */
  RFLM_CCS_LOGGING_FORCE_DISABLED        = 3,   /*!< Forcefully disables RFFE logging , when IU enters sleep mode */
  RFLM_CCS_LOGGING_ENABLE_IN_WAKEUP      = 4,   /*!< Postpones RFFE logging to next wakeup , when client triggers start and IU is not awake */
}rflm_ccs_logging_state_enums;

typedef struct
{
  uint32* log_buf_q6;
  uint32 num_log_cycles;
  uint32 ping_pong_buf_size;
}rflm_ccs_rffe_logging_q6_buffer_t;

/*! @brief  CCS RFFE GRFC logging fields*/
typedef struct
{
  uint32 new_rfc_log_mask;      /* bitmask for rffe_grfc logging */
} rf_ccs_rffe_grfc_logging_t;


typedef struct
{
  rflm_ccs_logging_state_enums log_state;
  uint32 log_evt;
  rf_ccs_rffe_grfc_logging_t log_settings;
  rflm_ccs_rffe_logging_q6_buffer_t log_buf_settings;
  uint32 log_mask;
  boolean pending_start_req;
}rflm_ccs_logging_state_t;

extern rflm_ccs_logging_state_t rflm_ccs_logging_state;

/*===========================================================================

FUNCTION: 

===========================================================================*/

void rflm_ccs_rf_process_rffe_log_buf(uint32 log_buf_evt_type, boolean is_isr_event);


/*===========================================================================

FUNCTION:  rflm_ccs_rf_rffe_logging_start

===========================================================================*/
/*!
  @brief
  Child api to start HW RFFE logging

  @return
  None
  */
/*=========================================================================*/
void rflm_ccs_rf_rffe_logging_start(rf_ccs_rffe_grfc_logging_t* rffe_grfc_log_settings);


/*===========================================================================

FUNCTION:  rflm_ccs_rf_rffe_logging_update

===========================================================================*/
/*!
  @brief
  Child api to update HW RFFE logging

  @return
  None
  */
/*=========================================================================*/
void rflm_ccs_rf_rffe_logging_update(rf_ccs_rffe_grfc_logging_t* rffe_grfc_log_settings);


/*===========================================================================

FUNCTION:  rflm_ccs_rf_rffe_logging_stop

===========================================================================*/
/*!
  @brief
  api to stop HW RFFE logging

  @return
  None
  */
/*=========================================================================*/
void rflm_ccs_rf_rffe_logging_stop(void);


/*===========================================================================

FUNCTION:  force_rffe_logging_stop_if_enabled

===========================================================================*/
/*!
  @brief
  api to force stop HW RFFE logging in sleep and halt apis

  @return
  None
  */
/*=========================================================================*/


void rflm_ccs_rf_force_rffe_logging_stop_if_enabled(void);

/*===========================================================================

FUNCTION:  rflm_ccs_rf_rffe_logging_start_posted

===========================================================================*/
/*!
  @brief
  API to force start HW RFFE logging in sleep and halt apis

  @return
  None
  */
/*=========================================================================*/


void rflm_ccs_rf_rffe_logging_start_posted(void);


#ifdef __cplusplus
}
#endif


#endif  // RFLM_CCS_RF_RFFE_LOG_H
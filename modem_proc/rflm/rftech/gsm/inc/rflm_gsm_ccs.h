/*=============================================================================

  Copyright (c) 2013-2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rflm.mpss/1.10/rftech/gsm/inc/rflm_gsm_ccs.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/19   dh      Featurized tuner specific code for GSM using FEATURE_RF_GSM_HAS_QTUNER
04/21/19   mpa     to bound timer_fixup_ptr to 24 bits
12/18/18   mpa     CW error FIX
10/16/14   sc      Enable non-issue sequence event tasks to be atomic
10/06/14   sc      Enable dynamic task queue management from RFSW
08/07/14   sc      Use device from FW for TQ pair assignment
07/24/14   cj      Added changes for Tx sharing feature
07/28/14   ec      Add API to process Jammer Detector CCS events
07/14/14   sc      Remove unused/unrequired functionality
06/24/14   ch      Add PA Ramp up delta start offset to CL tuner trigger time
05/29/14   sb      IP2 cal bug fixes, updates
03/31/14   ch      Created dedicated CL tuner CCS event processing
03/26/14   sb      Add support for IP2 cal for Dime PM
03/12/14   sc      Add disTx/Rx support
02/24/14   sc      Add therm read abort handling
02/06/14   sc      Pass intf idx to process GSM CCS event APIs
12/13/13   cj      Code cleanup for Bolt IRAT
12/09/13   sc      Move logging to common trace buffer
12/09/13   cj      Added IRAT CCS structure initialization and release
11/19/13   cj      Added rflm_gsm_process_irat_ccs_event API
10/31/13   sc      Mutex the variables
10/30/13   sc      Bug fix for CCs mem size ptr manipulation
10/21/13   sc      Bug fixes and logging enhancement
10/03/13   sc      Alloc/Free TQ mem management
09/13/13   sc      Add init and release of CCS mem and TQs
09/09/13   sc      Clean up header includes
09/03/13   sc      Update headers and type resource
08/30/13   sc      Update to use RFLM task struct def
08/28/13   sc	   Initial version of RFLM GSM CCS
=============================================================================*/

#ifndef RFLM_GSM_CCS_H
#define RFLM_GSM_CCS_H


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "rflm_api_cmn.h"
#include "rflm_ccs_rf_intf.h"
#include "rflm_ccs_rf_event_intf.h"
#include "rflm_cmn_intf.h"
#include "rflm_gsm_dm.h"

// This macro will sign extend CCS task trigger time, which is 22-bit uint32
#define RFLM_GSM_SIGN_EXTEND_CCS_TRIG_TIME(trigTime)  (((int32)(trigTime)) << (31 - 21)) >> (31 - 21)
#define RFLM_GSM_CCS_MAX_TQ_PAIRS 2
#define RFLM_GSM_RF_TQ_PAIR_0_ID 0
#define RFLM_GSM_RF_TQ_PAIR_1_ID 1

// This macro converts time in QS to time in USTMR
// 192 is 19.2 (*10) is USTMR clock frequency (MHz)
// 130 is 13 (*10) GSTMR clock frequence (MHz) and
// 12 is number of GSTMR clock cycles per QS
#define RFLM_GSM_QS_TO_USTMR( qs )  (( qs )*192*12)/130
#define RFLM_GSM_CCS_VALID_TIME_MASK              0xFFFFFF

/* Specify the maximum unserviced CCS events that can exist at any one time on a CCS task queue */
#define RFLM_GSM_MAX_TQA_EVENTS_PENDING 8
#define RFLM_GSM_MAX_TQB_EVENTS_PENDING 8

/* Calculate maximum combined CCS events that can exist on a task queue pair */
#define RFLM_GSM_MAX_EVENTS_PENDING ( RFLM_GSM_MAX_TQA_EVENTS_PENDING + RFLM_GSM_MAX_TQB_EVENTS_PENDING )

/*=============================================================================

                     RFLM GSM CCS DEFINITIONS AND TYPES

=============================================================================*/
/*! @enum rflm_gsm_event_process_status                                      */
/*! @brief A enum for status granularity of static ccs event process         */
typedef enum
{
  RFLM_GSM_CCS_EVENT_DEFAULT,
  RFLM_GSM_CCS_EVENT_SUCCESS,
  RFLM_GSM_CCS_EVENT_HEADER_ERROR,
  RFLM_GSM_CCS_EVENT_STATUS_ERROR,
  RFLM_GSM_CCS_EVENT_TASK_ERROR,
  RFLM_GSM_CCS_EVENT_PROCESS_FAILED,
  RFLM_GSM_CCS_EVENT_FAILED,
  RFLM_GSM_CCS_EVENT_PENDING,
}rflm_gsm_event_process_status;
                         

typedef enum
{
  RFLM_GSM_IP2_CAL_SETUP_EVENT,
  RFLM_GSM_IP2_CAL_STEP_EVENT,
  RFLM_GSM_IP2_CAL_CLEANUP_EVENT
}rflm_gsm_ip2_cal_event_type;

#ifdef FEATURE_TX_SHARING
typedef struct
{

  rflm_ccs_rf_event_description_buffer_header_type *event_header_ptr;
  ccs_rf_task_buf_item_t *event_task_ptr;
  rflm_ustmr_t trigger_time;
  uint32 rflm_gsm_intf_idx;
  uint32 rflm_handle_id;
  boolean intf_idx_valid;
}rflm_gsm_ccs_event_process_data;
#endif
/*===========================================================================*/
/*! @struct rflm_gsm_tq_mem_type                                             */
/*! @brief A structure to TQ memory data                                     */
/*! @details This structure holds the information about the CCS TQ memory    */
/*!          used to allocate memory for tasks.                              */
typedef struct
{
  /* TQ mem details */
  uint32 mem_size_max;
  uint32 *start_addr;
  uint32 *end_addr;

  /* Block Management */
  /* A 'Block' is a memory region from the:
   *      start of the TQ mem region to the end of the TQ mem region (first event submitted)
   *      OR
   *      end of the last event to the end of the TQ mem region
   *      OR
   *      start of the TQ mem region to the start of the last event (when wrapped around)
   */
  uint32 next_block_size;
  uint32 *next_block_start_addr;
  uint32 *next_block_end_addr;

  /* Maintain the details of last event submitted */
  uint32 prev_event_size;
  uint32 *prev_event_start_addr;
  uint32 *prev_event_end_addr;

  /* Event data */
  uint32 current_num_events;
  uint32 num_free_events;
  uint32 total_free_mem;
}rflm_gsm_tq_mem_type;

/*===========================================================================*/
/*! @struct rflm_gsm_tq_data                                                 */
/*! @brief A structure to contain the CCS task queue information             */
typedef struct
{
  uint8 tq_index;
  rflm_ccs_rf_tq_handle_t *tq_handle;
  uint32 *tq_data_ptr;
  uint16 tq_data_bytes;
}rflm_gsm_tq_data;


/*===========================================================================*/
/*! @struct rflm_gsm_ccs                                                     */
/*! @brief A structure to contain the events being processed                 */
/*! @details Need to manage the events being prepared and the completion.    */
/*!          The event mask will indicate free/occupied event indexes.       */
/*!          When allocating event, search the mask for free ids             */
typedef struct
{
  uint32 event_idx;
  rflm_ccs_rf_event_description_buffer_header_type *event_header_ptr;
  uint32 response_payload[2];
  uint32 *ccs_mem_addr;
  uint32 ccs_event_size;
  rflm_gsm_subscription_id sub_id;
}rflm_gsm_event_info;


typedef struct
{
  rflm_gsm_event_info     event_info[RFLM_GSM_MAX_EVENTS_PENDING];
  uint32                  event_mask;
  uint32                  sub_events_remaining[RFLM_GSM_MAX_SUBSCRIPTIONS];
  rflm_gsm_tq_mem_type    mem_mgr;
  rflm_gsm_tq_data        rf_tq0;
  rflm_gsm_tq_data        rf_tq1;
  qmutex_t                mem_lock;
  qmutex_t                rf_tq0_lock;
  qmutex_t                rf_tq1_lock;
}rflm_gsm_ccs;





#define RFLM_GSM_CCS_TOKEN_MAX_DATA_SIZE_BYTES 16

/*============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================== 

                     RFLM GSM CCS RELATED API PROTOTYPES

==============================================================================*/
rflm_gsm_event_process_status rflm_gsm_process_ccs_event
( 
  rflm_ccs_rf_event_description_buffer_header_type *event_header_ptr,
  rflm_ccs_rf_task_buf_item_t *event_task_ptr,
  rflm_ustmr_t trigger_time,
  uint32 rflm_gsm_intf_idx,
  rflm_gsm_ccs_task_q tq,
  rflm_gsm_ccs_event_type event_type
);

rflm_gsm_ccs* get_gsm_ccs(uint8 tq_pair);

#ifdef FEATURE_RF_GSM_HAS_QTUNER
rflm_gsm_event_process_status rflm_gsm_process_cltuner_event
( 
  rflm_cmn_tuner_event_data_type *tuner_event_ptr,  
  int32 power_level,
  rflm_ustmr_t trigger_time,
  int16 start_offset,
  uint32 rflm_gsm_intf_idx
);
#endif

rflm_gsm_event_process_status rflm_gsm_process_therm_read_abort
(
  rflm_ccs_rf_event_description_buffer_header_type *event_header_ptr_a,
  rflm_ccs_rf_event_description_buffer_header_type *event_header_ptr_b,
  uint32 rflm_gsm_intf_idx
);
uint32 rflm_gsm_ustmr_add_offset(uint32 time0, int32 offset);

void rflm_gsm_ccs_combine_events(rflm_ccs_rf_event_description_buffer_header_type *event_header1,
                                 ccs_rf_task_buf_item_t *event_task1,
                                 rflm_ccs_rf_event_description_buffer_header_type *event_header2,
                                 ccs_rf_task_buf_item_t *event_task2,
                                 rflm_ccs_rf_event_description_buffer_header_type *event_header_combined,
                                 ccs_rf_task_buf_item_t *event_task_combined);

void rflm_gsm_ccs_combine_events_3(rflm_ccs_rf_event_description_buffer_header_type *event_header1,
                                 ccs_rf_task_buf_item_t *event_task1,
                                 rflm_ccs_rf_event_description_buffer_header_type *event_header2,
                                 ccs_rf_task_buf_item_t *event_task2,
                                 rflm_ccs_rf_event_description_buffer_header_type *event_header3,
                                 ccs_rf_task_buf_item_t *event_task3,
                                 rflm_ccs_rf_event_description_buffer_header_type *event_header_combined,
                                 ccs_rf_task_buf_item_t *event_task_combined);


/*---------------------------------------------------------------------------*/

boolean rflm_gsm_ccs_init_ccs_info( rflm_gsm_subscription_id sub_id, uint32 device_id );

/*---------------------------------------------------------------------------*/

 void rflm_gsm_ccs_add_event_info( uint32 *ccs_mem_addr,
                                         uint32 ccs_event_size,
                                         rflm_ccs_rf_event_description_buffer_header_type *event_header_ptr,
                                         rflm_gsm_event_info **rflm_gsm_callback_args,
                                         rflm_gsm_ccs *gsm_ccs_ptr,
                                         rflm_gsm_subscription_id sub_id );



void rflm_gsm_ccs_release_ccs_info( rflm_gsm_subscription_id sub_id, uint32 device_id );

 /*---------------------------------------------------------------------------*/

 rflm_err_t rflm_gsm_rf_send_irat_script( rflm_handle_rx_t rflm_handle,
                                        rflm_gsm_irat_ccs_input_type *script_vars_in,
                                        rflm_gsm_irat_ccs_output_type *script_vars_out,
										uint32* temp_script_buf );

 /*---------------------------------------------------------------------------*/
boolean rflm_gsm_init_ip2_cal_step_info_tbl(rflm_gsm_subscription_id sub_id );

boolean rflm_gsm_reset_ip2_cal_step_info_tbl(rflm_gsm_subscription_id sub_id);

void rflm_gsm_execute_ip2_cal_steps
( 
 rflm_gsm_ip2_cal_step_ccs_input_type *ip2_cal_info,
 rflm_gsm_ip2_cal_step_ccs_output_type *ret_info,
 rflm_gsm_subscription_id sub_id
);

rflm_gsm_event_process_status rflm_gsm_process_ip2_ccs_event
( 
 uint32 curr_index,
 uint32 curr_trigger_time,
 rflm_gsm_subscription_id sub_id,
 rflm_gsm_ip2_cal_event_type event_type
);

void rflm_gsm_ustmr_sync_fixup(rflm_ccs_rf_event_header_task_type *evt_p, uint32 *ustmr_data);

rflm_gsm_event_process_status rflm_gsm_process_jdet_read_ccs_event
(
 uint32 read_index,
 uint32 curr_trigger_time,
 uint32 rflm_intf_idx,
 rflm_gsm_fw_cb_type fw_callback,
 void* fw_jdet_cb_arg
 );
void rflm_gsm_ustmr_sync_callback( rflm_ccs_rf_tq_handle_t *tq_handle,
                              uint32 ccs_task_idx,
                              ccs_rf_task_status_t task_status,
                              void *args_ptr );

#ifdef FEATURE_TX_SHARING
rflm_err_t rflm_gsm_prog_tx_enter_ccs_script( rflm_handle_tx_t rflm_handle,
                                                 rflm_gsm_init_ccs_input_type *script_vars_in,
                                                 rflm_gsm_init_ccs_output_type *script_vars_out );
rflm_err_t rflm_gsm_prog_tx_enable_ccs_script( rflm_handle_tx_t rflm_handle,
                                                 rflm_gsm_init_ccs_input_type *script_vars_in,
                                                 rflm_gsm_init_ccs_output_type *script_vars_out );
rflm_err_t rflm_gsm_prog_tx_disable_ccs_script( rflm_handle_tx_t rflm_handle,
                                                 rflm_gsm_init_ccs_input_type *script_vars_in,
                                                 rflm_gsm_init_ccs_output_type *script_vars_out );
rflm_gsm_event_process_status rflm_gsm_process_ccs_event_v2( rflm_gsm_ccs_event_process_data *in);
#endif

void rflm_gsm_ccs_update_rf_event_tokens
(  uint32 event_id,
  rflm_ccs_rf_event_header_task_type  *dm_event_ptr,
  uint8                                num_tokens,
  rflm_gsm_fixup_token_type           *event_tokens,
  uint8                                token_data[][RFLM_GSM_CCS_TOKEN_MAX_DATA_SIZE_BYTES]
);
boolean rflm_gsm_ccs_get_therm_data(      uint8 sub_id, 
                                  uint32* therm_data );

/*============================================================================*/
#ifdef __cplusplus
} // extern "C"
#endif
/*============================================================================*/
#endif /* RFLM_GSM_CCS_H */


/*!
  @file
  rflm_ccs_rf_event_intf.h

  @brief
  Types for RF CCS events inside shared memory
 
*/
#ifndef RFLM_CCS_RF_EVENT_INTF_H
#define RFLM_CCS_RF_EVENT_INTF_H

/*===========================================================================
Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rflm.mpss/1.10/api/ccs/rflm_ccs_rf_event_intf.h#1 $
$DateTime: 2020/04/16 12:31:05 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/12/18   mns     RFFE GRP changes.
05/25/18   mns     CCS MMU Changes.
10/29/14   par     Fix compilation warning 
04/24/14   ra      Temporary fix for DimePM
12/02/13   pl      remove obsolete defines, move definition to CCS header file
11/19/13   ra      Import data structure and prototype in RFLM CCS API component
10/21/13   Saul    CCS. Available payload-size vs needed-size check bug fix.
10/06/13   ra      Support RFLM CCS 
07/22/13   ra      Initial version. Ported from DI.2.0
============================================================================*/



#include "rflm_ccs_rf_intf.h"
#ifdef FEATURE_JOLOKIA_MODEM
#include "rf_event_intf.h"
#endif


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*! @brief Number of IRAT scripts 2 for startup, 2 for cleanup, 
    and 2 for on-demand TQs, last 3 for T2L connected mode  */
#define RFLM_CCS_NUM_IRAT_SCRIPT_BUFS  9 

/*! @brief Number of event for every IRAT script */
#define RFLM_CCS_NUM_IRAT_SCRIPT_EVENT 2
   
/*! @brief Max number of CCS tasks */
#define RFLM_CCS_RF_MAX_NUM_CCS_TASKS 14

#define RFLM_CCS_MC_SEQ_MAX_TOKENS_PER_SUBEVENT 24
#define RFLM_MC_SEQ_MAX_TASKS_PER_SUBEVENT 4

#define RFLM_CCS_RF_EVENT_INTF_MINOR_VERSION   0
#define RFLM_CCS_RF_EVENT_INTF_MAJOR_VERSION   2


#define RFLM_CCS_RF_EVENT_INTF_VERSION    (RFLM_CCS_RF_EVENT_INTF_MAJOR_VERSION<<16)|(RFLM_CCS_RF_EVENT_INTF_MINOR_VERSION)


/*! @brief Base IDs for RF events. All tech event IDs should be 
  in the range RFLM_CCS_RF_EVENT_ID_BASE_XXX to (RF_EVENT_ID_BASE_XXX+RF_EVENT_MAX_TECH_ID).
  The execption is RF command interface tasks/events which have the range 0x80000000 - 0xFFFFFFFF
*/
#define RFLM_CCS_RF_EVENT_ID_BASE_GSM     0x01000000
#define RFLM_CCS_RF_EVENT_ID_BASE_WCDMA   0x02000000
#define RFLM_CCS_RF_EVENT_ID_BASE_CDMA    0x03000000
#define RFLM_CCS_RF_EVENT_ID_BASE_LTE     0x04000000
#define RFLM_CCS_RF_EVENT_ID_BASE_TDSCDMA 0x05000000
#define RFLM_CCS_RF_EVENT_ID_BASE_IRAT    0x06000000
#define RFLM_CCS_RF_EVENT_MAX_TECH_ID     0x00FFFFFF
#define RFLM_CCS_RF_EVENT_ID_BASE_RFCMD   0x80000000
#define RFLM_CCS_RF_EVENT_MAX_RFCMD_ID    0xEFFFFFFF

/* There are two SBI write transactions per CCS word buffer */
#define CCS_RF_CMD_SSBI_WORDS_PER_TRANS           1


/*! @brief Event Status Type
*/
typedef enum
{
  RFLM_CCS_RF_EVENT_IDLE           = 0x0,      /*!< Event may be overwritten */
  RFLM_CCS_RF_EVENT_BUILDING       = 0x1,      /*!< Event is being constructed */
  RFLM_CCS_RF_EVENT_COMMITTED      = 0x2,      /*!< Event is finalized and pending FW/CCS processing */
  RFLM_CCS_RF_EVENT_COMPLETE       = 0x3,      /*!< Event is completed and data/status bytes are ready */
  RFLM_CCS_RF_EVENT_SW_ERROR       = 0xFF,     /*!< Event has some issue with its construction, e.g. memory overflow */
} rflm_ccs_rf_event_status_type;


/*! @brief Event Header Type
*/
typedef struct 
{
  uint32  event_id;           /*!< Event ID */
  boolean response_required;  /*!< Response to event is required or not */
  boolean make_atomic;        /*!< Make this event atomic (e.g. ISSUE_SEQ) */
  uint8 priority_if_atomic;   /*!< Priority of the event/task when event is atomic */
  uint8 num_tasks;            /*!< Number of tasks in this event*/
  rflm_ccs_rf_event_status_type event_status; /*!< Status of the data in this event */
  uint32 scratch_mem[2];      /*!< Scratch memory for FW to use per event */

} rflm_ccs_rf_event_description_buffer_header_type;

#ifndef FEATURE_JOLOKIA_MODEM
typedef ccs_rf_task_buf_item_t rf_ccs_event_task_description_type;
#endif

typedef ccs_rf_task_buf_item_t rf_ccs_event_task_description_type;

typedef struct rflm_ccs_rf_event_header_task_s
{
  /*! @brief Header of event  */
  rflm_ccs_rf_event_description_buffer_header_type header;

  /*! @brief Tasks for the event */
  ccs_rf_task_buf_item_t task_buf[RFLM_CCS_RF_MAX_NUM_CCS_TASKS];
} rflm_ccs_rf_event_header_task_type;

typedef ccs_rf_task_buf_item_t rf_ccs_event_buf_description_type;

/*! @brief Event Interface Type
*/
typedef struct
{
  rflm_ccs_rf_event_description_buffer_header_type     header;
  /*! @brief Tasks for the event */
  ccs_rf_task_buf_item_t                          tasks[RFLM_CCS_RF_MAX_NUM_CCS_TASKS];  
  /*! @brief Payload */
  uint32                                          payload[CCS_RF_MAX_NUM_TRANSACTIONS*CCS_RF_CMD_SSBI_WORDS_PER_TRANS];
} rflm_ccs_bus_cmd_intf_event_type;

typedef rflm_ccs_bus_cmd_intf_event_type rf_bus_ccs_cmd_intf;
/*! @brief Stucture to return event size to caller FW techs */
typedef struct
{
  /*! @brief Size of transaction data in bytes */
  uint32 size_transaction_data_bytes:16;
  /*! @brief Number if tasks in the event */
  uint32 num_ccs_tasks              : 8;
  /*! enq status */
  uint32 event_enq_status           : 8;
} rflm_ccs_event_info_s;

typedef struct
{
  uint32 offset  :16;  /*!@brief offset in bytes , may NOT be 32-bit aligned  */
  uint32 token   :16;
  uint16 num_bytes;    /*!@brief Number of bytes to be fixed up ,corresponding to the token-offset entry*/
  uint16 aligned_byte_offset;        /*optional*/
  //uint16 aligned_length ;            /*optional*/

}rflm_ccs_rf_token_offset_bytes_t;


typedef struct
{
  uint16 token;
  uint16 start_byte;
  uint16 offset;
  uint16 num_bytes; /*!@ brief in terms of bytes*/
}ccs_token_info_t;

/*Structure appended after every task's (config+data+uint32 shadow) by event builder into the CCS format . This structure is only for reference and typecasting.
  The actual CCS representation  is compressed and different*/
typedef struct
{
  uint16 num_tokens;
  uint16 subevent;
  /*ccs_token_info_table is of the size=num_tokens, do not assume there are RFLM_CCS_MC_SEQ_MAX_TOKENS_PER_SUBEVENT elements for a particular ccs_event_task_payload.  */
  ccs_token_info_t ccs_token_info_table[RFLM_CCS_MC_SEQ_MAX_TOKENS_PER_SUBEVENT];

}ccs_event_token_payload;


/*!This struct must be used by tech to allocate the event_token_offset_bytes_table for each MC-seq  */
typedef struct
{

  uint16 num_tokens; /*! no. of tokens per subevent */
  uint16 subevent;
  uint8 task_start;
  uint8 num_tasks;
  rflm_ccs_rf_token_offset_bytes_t token_offset_bytes[RFLM_CCS_MC_SEQ_MAX_TOKENS_PER_SUBEVENT];
  uint32 timing_tasks[RFLM_MC_SEQ_MAX_TASKS_PER_SUBEVENT];  /*Relative timings of the tasks as obtained from ccs_event */
  uint32 rffe_addr[RFLM_MC_SEQ_MAX_TASKS_PER_SUBEVENT];     /*RFFE addr info of the tasks as stored in MC-seq(DM) */
}rflm_ccs_rf_event_token_offset_bytes_t;

/*! Structure used to input the fixup information using subevent and tokens for  ccs_event/non-mc-seq fixup */
typedef struct
{

  uint16 token_id ;    /*! Desired token for event_id to be fixed */
  uint16 token_idx;    /*! idx at which the token can be found in the subevent token offset table */
  uint16 num_bytes;    /*! Number of bytes to be fixed up */
  uint8* data;         /*! Pointer to new data */
}rflm_ccs_rf_token_data_table_t;
/*!Usage: rflm_ccs_rf_token_data_table_t t_info[max_tokens];*/


typedef struct rflm_ccs_enq_event_config_s
{
  /*!< Head pointer for event (including header and tasks) */
  rflm_ccs_rf_event_header_task_type *task_p;
  /*!< Start time of script, if time needs to be immediate, 
   *     pass RFLM_CCS_RF_EVENT_TIME_IMMEDIATE */
  uint32 start_offset;
  /*!< Callback function for task completion */
  rflm_ccs_rf_task_callback_t callback;
  /*!< Argument to callback function */
  void *callback_arg;          
  /*!< TQ for programming the CCS RF tasks */
  rflm_ccs_rf_tq_handle_t *tq;
  /*!< Event timing check config */
  rflm_ccs_rf_event_time_chk_t * evt_chk_ptr;
  /*!< Callback ordering: 
   *        TRUE - CBs will be serviced in the order they finish
   *               FALSE - CBs will be serviced in the order they enqeueue */
  boolean no_cb_order;
  /*!< Tech ID */
  rflm_tech_id_t tech_id;
}rflm_ccs_enq_event_config_t;



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------*/
void rf_hal_ccs_read_status_set(void);

/*----------------------------------------------------------------------------*/
void rflm_ccs_request_dedicated_tq( uint32 tq_index );
void rflm_ccs_release_dedicated_tq( void );

/*----------------------------------------------------------------------------*/
void
rflm_ccs_send_common_rf_event
( 
   rflm_ccs_rf_event_header_task_type * cmd,
   uint16 avail_length_bytes
);


rflm_ccs_event_info_s rflm_ccs_parse_common_rf_event(
  /*! Head pointer for event (including header and tasks) */
  rflm_ccs_rf_event_header_task_type *task_p,
  /*! @brief Pointer to FW memory where CCS tasks need to be copied */
  ccs_rf_task_t *fw_ccs_task_ptr,
  /*! @brief Length of CCS tasks */
  uint32 max_ccs_task_num,
  /*! CCS desination memory for SSBI/ RFFE addr/data information */
  uint32 *ccs_dest_ptr,
  /*! Length of destination CCS memory in bytes */
  uint32 length_bytes );



rflm_ccs_event_info_s rflm_ccs_enqueue_common_rf_event_ext(
  /*! Head pointer for event (including header and tasks) */
  rflm_ccs_rf_event_header_task_type *task_p,
  /*! Start time of script, if time needs to be immediate, 
    pass RFLM_CCS_RF_EVENT_TIME_IMMEDIATE */
  uint32 start_offset,
  /*! CCS desination memory for SSBI/ RFFE addr/data information */
  uint32 *ccs_dest_ptr,
  /*! Length of destination CCS memory in bytes */
  uint32 length_bytes,
  /*! Callback function for task completion */
  rflm_ccs_rf_task_callback_t callback, 
  /*! Argument to callback function */
  void *callback_arg,          
  /*! TQ for programming the CCS RF tasks */
  rflm_ccs_rf_tq_handle_t *tq,
  /*! Is static event */
  boolean is_static_event
  );

#define rflm_ccs_enqueue_common_rf_event(task_p,start_offset,ccs_dest_ptr,length_bytes,callback,callback_arg,tq,...) \
	rflm_ccs_enqueue_common_rf_event_ext(task_p,start_offset,ccs_dest_ptr,length_bytes,callback,callback_arg,tq,(FALSE, ## __VA_ARGS__))

rflm_ccs_event_info_s rflm_ccs_rf_enqueue_event
(
  rflm_ccs_enq_event_config_t * event_config_ptr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RFLM_CCS_RF_EVENT_INTF_H */

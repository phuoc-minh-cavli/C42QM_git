#ifndef __CSD_VOC_COMMON_H__
#define __CSD_VOC_COMMON_H__

/*-----------------------------------------------------------------------------
     Copyright (c) 2009 - 2011, 2016 Qualcomm Technologies Incorporated.
  All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/voice/inc/csd_voc_common.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $
==============================================================================*/
/*=============================================================================

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.
  when          who       what, where, why
  --------     -----     ---------------------------------------------------
  08/25/14      jk       added per vocoder calibration changes  
=============================================================================*/
#include "mmdefs.h"
#include "apr_errcodes.h"
#include "apr_list.h"
#include "apr_comdef.h"
#include "apr_objmgr.h"


#undef CSD_INTERNAL
#ifdef __cplusplus
  #define CSD_INTERNAL extern "C"
#else
  #define CSD_INTERNAL extern
#endif /* __cplusplus */
  /**< This marks an internal API that is not intended for public use. */

#undef CSD_EXTERNAL
#ifdef __cplusplus
  #define CSD_EXTERNAL extern "C"
#else
  #define CSD_EXTERNAL extern
#endif /* __cplusplus */
  /**< This marks an external API that is intended for public use. */

#define CSD_VOC_STATEID_READY ( 0x00011166 )
#define CSD_VOC_STATEID_NOT_READY ( 0x00011167 )
#define CSD_VOC_COMMON_TASK_NAME ( "CSD_VOC_COMMON" )       /* %%%TBD */
#define CSD_VOC_COMMON_TASK_PRIORITY ( VOC_COMMON_PRIORITY )      /* %%%TBD */
#define CSD_VOC_COMMON_THREAD_STACK_SIZE ( 6400 )      /* %%%TBD */
#define CSD_VOC_COMMON_CACHE_LINE_SIZE ( 128 )
#define CSD_VOC_COMMON_PAGE_ALIGN ( 4096 )
#define CSD_VOC_COMMON_MIN_DATA_WIDTH ( 8 )
#define CSD_VOC_COMMON_MAX_DATA_WIDTH ( 64 )
#define CSD_MEM_PAGE_SIZE        (4096)
/* Function used by client sessions for processing commands */
typedef uint32_t (*csd_voc_common_cmd_fn_t)(uint32_t h_client);

/* Function used by client sessions for running state machine */
typedef uint32_t (*csd_voc_common_state_fn_t)(uint32_t h_client);

/* Signals that wake up the csd_voc_common task */
typedef enum {
  CSD_VOC_COMMON_SIG_NONE = 0x0,         //bit enums
  CSD_VOC_COMMON_SIG_CMD = 0x1,         //bit enums
} csd_voc_common_sig_enum_t;

/* Set of parameters that can be retrieved from OEMDB */
typedef enum {
  CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC_COLUMN_INFO, /* Get Size of Stream Calibration based on network ID / Sampling Rate */
  CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC_COLUMN_INFO, /* Get Size of Common Calibration based on network ID / Sampling Rate */
  CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_COLUMN_INFO  /* Get Size of Volume Calibration based on network ID / Sampling Rate */
} csd_voc_common_get_oemdb_column_info_enum_t;

typedef enum {
  CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC_COLUMN_INFO_SIZE,
  CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC_COLUMN_INFO_SIZE,
  CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_COLUMN_INFO_SIZE
} csd_voc_common_get_oemdb_column_info_size_enum_t;

typedef enum {
  CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC,      /* Get Stream Calibration based on network ID / Sampling Rate */
  CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC,      /* Get Common Calibration based on network ID / Sampling Rate */
  CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC,      /* Get Volume Calibration based on network ID / Sampling Rate */
  CSD_VOC_COMMON_GET_OEMDB_DEV_CFG      /* Get Device Config Calibration based on network ID / Sampling Rate */
} csd_voc_common_get_oemdb_enum_t;

typedef enum {
  CSD_VOC_COMMON_GET_OEMDB_VP3_INFO,
  CSD_VOC_COMMON_GET_OEMDB_VP3_DATA,
  CSD_VOC_COMMON_SET_OEMDB_VP3_DATA          
} csd_voc_common_process_vp3_oemdb_enum_t;

typedef enum {
  CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC_SIZE, /* Get Size of Stream Calibration based on network ID / Sampling Rate */
  CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC_SIZE, /* Get Size of Common Calibration based on network ID / Sampling Rate */
  CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_SIZE,  /* Get Size of Volume Calibration based on network ID / Sampling Rate */
  CSD_VOC_COMMON_GET_OEMDB_DEV_CFG_SIZE, /* Get Size of Device COnfiguration Calibration */
  CSD_VOC_COMMON_GET_OEMDB_VOL_TABLE_STEP_SIZE
} csd_voc_common_get_oemdb_size_enum_t;


/* Parameters that can be retrieved for VocProc */
typedef struct {
  uint32_t tx_device_id;
  uint32_t rx_device_id;
  uint32_t tx_samp_rate;      // 8000 for 8K, 16000 for 16K, etc
  uint32_t rx_samp_rate;      // 8000 for 8K, 16000 for 16K, etc
} csd_voc_common_get_oemdb_common_t;

/* Parameters that can be retrieved for Volume Tables */
typedef struct {
  uint32_t tx_device_id;
  uint32_t rx_device_id;
  uint32_t feature_id;
} csd_voc_common_get_oemdb_volume_t;

typedef struct {
  uint32_t tx_device_id;
  uint32_t rx_device_id;
} csd_voc_common_get_oemdb_dev_cfg_t;

typedef struct {
  uint32_t tx_device_id;
  uint32_t rx_device_id;
} csd_voc_common_process_oemdb_vp3_t;

typedef struct {
  uint32_t no_of_entries;
  uint32_t mid;
  uint32_t pid;
  uint32_t max_length;
} csd_voc_common_vp3_list_t;

typedef struct {
  csd_voc_common_get_oemdb_common_t     common;
  csd_voc_common_get_oemdb_volume_t     volume;
  csd_voc_common_get_oemdb_dev_cfg_t    dev_cfg;
} csd_voc_common_get_oemdb_t;

typedef struct {
  uint32_t tx_device_id;
  uint32_t rx_device_id;
} csd_voc_common_check_dev_pair_t;

typedef enum 
{
  CSD_VOC_COMMON_THREAD_STATE_ENUM_INIT,
  CSD_VOC_COMMON_THREAD_STATE_ENUM_READY,
  CSD_VOC_COMMON_THREAD_STATE_ENUM_EXIT
} csd_voc_common_thread_state_enum_t;

typedef struct
{
  bool_t             sig;
  csd_voc_common_cmd_fn_t      cmd_fn;
} csd_voc_common_session_handle_t;

typedef struct 
{
  apr_list_node_t       link;
  csd_voc_common_session_handle_t sess;
} csd_voc_common_session_list_t;

#define CSD_VOC_COMMON_NUM_SESSIONS  25

/*
 * FUNCTION: csd_voc_common_init

 * PARAMETERS:  None

 * CONTEXT: Clients context at bootup time

 * SIDE EFFECTS: create a CVC task/thread .

 * RETURN:    csd status
*/
uint32_t csd_voc_common_init ( void );

/*
 * FUNCTION: csd_voc_common_deinit

 * PARAMETERS: None

 * CONTEXT: Clients context at bootup time

 * SIDE EFFECTS: Destroys thread .

 * RETURN:    csd status
*/
uint32_t csd_voc_common_deinit( void );


/*
 * FUNCTION: csd_voc_common_get_session

 * PARAMETERS: state_fn & command fn for the session

 * CONTEXT:  CVC task context

 * SIDE EFFECTS: Reserves a csd_voc_common session for client

 * RETURN:  csd status & csd_voc_common handle
*/
uint32_t csd_voc_common_get_session(
  csd_voc_common_cmd_fn_t       cmd_fn,
  uint32_t           *h_csd_voc_common
);

/*
 * FUNCTION: csd_voc_common_free_session

 * PARAMETERS: csd_voc_common session

 * CONTEXT: CVC Task

 * SIDE EFFECTS: Free csd_voc_common session

 * RETURN:   csd status
*/
uint32_t csd_voc_common_free_session(
  uint32_t *h_csd_voc_common
);

/*
 * FUNCTION: csd_voc_common_queue_cmd

 * PARAMETERS: csd_voc_common handle and signal

 * CONTEXT: any context

 * SIDE EFFECTS: CVC task wakes up

 * RETURN:    csd status
*/
uint32_t csd_voc_common_queue_cmd(
  uint32_t h_csd_voc_common,
  csd_voc_common_sig_enum_t        sigs
);

/*
 * FUNCTION: csd_voc_common_query_oemdb

 * PARAMETERS: buffer pointer

 * CONTEXT:  CVC Task

 * SIDE EFFECTS: reserves memory for this query from static pool

 * RETURN:    csd status
*/
uint32_t csd_voc_common_query_device_info(
  csd_voc_common_get_oemdb_t       *data );

/*
 * FUNCTION: csd_voc_common_check_device_pair

 * PARAMETERS: buffer pointer

 * CONTEXT:  VC Task

 * SIDE EFFECTS: 

 * RETURN:    uint32_t rc
*/  
uint32_t csd_voc_common_check_device_pair(
  csd_voc_common_check_dev_pair_t *data );
  
/*
 * FUNCTION: csd_voc_common_get_calibration_size

 * PARAMETERS: buffer pointer

 * CONTEXT:  CVC Task

 * SIDE EFFECTS: 

 * RETURN:    size of of buffer required for calibration
*/
uint32_t csd_voc_common_get_calibration_size(
  csd_voc_common_get_oemdb_size_enum_t  type,
  csd_voc_common_get_oemdb_t            *data );

/*
 * FUNCTION: csd_voc_common_get_calibration

 * PARAMETERS: buffer pointer

 * CONTEXT:  CVC Task

 * SIDE EFFECTS: reserves memory for this query from static pool

 * RETURN:    csd status
*/
uint32_t csd_voc_common_get_calibration(
  csd_voc_common_get_oemdb_enum_t  type,
  csd_voc_common_get_oemdb_t       *data,
  uint8_t                         *buf_ptr,  /* [ in ] Buffer pointer */
  uint32_t                         buf_size,   /* [ in ] Buffer size */
  uint32_t                         *write_size /* [ out ]size of calibration written */
);

uint32_t csd_voc_common_get_cal_column_info_size(
  csd_voc_common_get_oemdb_column_info_size_enum_t  type,
  uint32_t rx_dev_id,
  uint32_t tx_dev_id
);

uint32_t csd_voc_common_get_cal_column_info(
  csd_voc_common_get_oemdb_column_info_enum_t  type,
  uint8_t                                      *buf_ptr,  /* [ in ] Buffer pointer */
  uint32_t                                     buf_size,   /* [ in ] Buffer size */
  uint32_t                                     *write_size, /* [ out ]size of calibration written */
  uint32_t                                     rx_dev_id, /* rx device id*/
  uint32_t                                     tx_dev_id /* tx device id*/
);

/*
 * FUNCTION: csd_voc_common_obtain_session_from_handle

 * PARAMETERS: csd_handle and pointer to the apr object manager
 * where the object was allocated
 
 * CONTEXT: any context

 * RETURN:    void*
*/

void* csd_voc_common_obtain_session_from_handle ( 
  int32_t csd_handle,
   apr_objmgr_t* objmgr_obj 
  );

/*
 * FUNCTION: csd_voc_commmon_is_valid_sample_rate

 * PARAMETERS: dev_id

 * CONTEXT: any context

 * RETURN:  int32_t - CSD error codes.
*/
int32_t csd_voc_commmon_is_valid_sample_rate
(
  uint32_t dev_id,
  uint32_t sample_rate
);

/*
 * FUNCTION: csd_voc_commmon_remove_device_info

 * PARAMETERS: dev_id

 * CONTEXT: any context

 * RETURN:  afe_port_id *
*/

int32_t csd_voc_commmon_remove_device_info
(
  uint32_t dev_id
);
/*
 * FUNCTION: csd_voc_commmon_get_afe_port_id

 * PARAMETERS: dev_id

 * CONTEXT: any context

 * RETURN:  afe_port_id *
*/

int32_t csd_voc_commmon_get_active_afe_port_id
(
  uint32_t dev_id,
  uint16_t *afe_port_id
);

/*
 * FUNCTION: csd_voc_commmon_get_device_info

 * DESCRIPTION: Get device info from active device records or prefetch a device
 * for later use

 * DEPENDENCIES: Has to be complimented with csd_voc_commmon_remove_device_info
 * once device info is no longer valid/required

 * PARAMETERS: dev_id

 * CONTEXT: any context

 * RETURN:  *topology_id, *afe_port_id
*/

int32_t csd_voc_commmon_get_device_info
(
  uint32_t dev_id,
  uint32_t *topology_id,
  uint16_t *afe_port_id
);


/*
 * FUNCTION: csd_voc_common_populate_memory_table

 * PARAMETERS: pva_memory_table, p_cal_buf

 * CONTEXT: any context

 * RETURN:  memory_blocks, pa_cal_buf *
*/

int32_t csd_voc_common_populate_memory_table ( uint8_t* pva_memory_table,
                                               uint8_t* p_cal_buf,
                                               uint32_t cal_buf_size,
                                               uint32_t* memory_blocks,
                                               int64_t* pa_cal_buf,
                                               void* dalmem_handle );

/*
 * FUNCTION: csd_voc_common_process_vp3

 * PARAMETERS: 

 * CONTEXT: 

 * RETURN:  
*/                                           
int32_t csd_voc_common_process_vp3(
  csd_voc_common_process_vp3_oemdb_enum_t type,
  csd_voc_common_process_oemdb_vp3_t cmd,
  uint8_t* buf_ptr,
  uint32_t mem_size,
  uint32_t* write_size );

                                               
#endif /* __CSD_VOC_COMMON_H__ */

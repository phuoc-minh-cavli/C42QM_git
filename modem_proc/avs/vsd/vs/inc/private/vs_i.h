#ifndef __VS_I_H__
#define __VS_I_H__

/**
  @file  vs_i.h
  @brief This file contains internal definitions of VS
*/

/*
  ============================================================================
  Copyright (C) 2013-2019 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //source/qcom/qct/multimedia2/api/audio/2.x/vs/main/latest/inc/vs_i.h
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
  Include files for Module
****************************************************************************/

/** CORE APIs. */
#include "msg.h"
#include "err.h"

#if ( defined AVS_MPSS_TEST )
#include "qurt_elite.h"
#endif

#if defined (VS_PROFILING) || defined (VS_PROFILING_DATA_PCIE)
#include "qurt_sclk.h"
#endif

#ifdef SERVREG_STUB
#include "vs_servreg_stub.h"
#else 
#include "servreg_common.h"
#endif //SERVREG_STUB

/* MM UTILS APIs */
#include "mmdefs.h"
#include "mmstd.h"

/** APR APIs */
#include "aprv2_packet.h"
#include "apr_lock.h"
#include "apr_event.h"
#include "apr_timer.h"
#include "apr_thread.h"
#include "apr_objmgr.h"
#include "apr_memmgr.h"
#include "apr_list.h"
#include "apr_misc.h"
#include "aprv2_api_inline.h"

/** VSD APIs. */
#include "vss_public_if.h"
#include "drv_api.h"
#include "voicemem.h"

/** VS APIs. */
#include "vs.h"

#ifndef AVS_MPSS_TEST
/** VSTMR and IPA APIs. */
#include "vstmr_xo.h"
#include "DDIPlatformInfo.h"
#ifdef IPA_STUB
#include "vs_ipa_dma_stub.h"
#include "vs_ipa_vote_stub.h"
#else
#include "ipa_dma.h"
#include "ipa_vote.h"
#endif
#endif


/****************************************************************************
  VS DEFINES
****************************************************************************/

/**
 * 2k bytes is required for creating job objects for enabling vocoder session.
 * TBD: If it can be reduced further.
 */
#define VS_HEAP_SIZE_V ( 2048 )

/* Size of VS work packet queue. */
#define VS_NUM_WORK_PKTS_V ( 20 )

#define VS_HANDLE_TOTAL_BITS_V ( 16 )
#define VS_HANDLE_INDEX_BITS_V ( 5 ) /**< 5 bits = 32 handles. */
#define VS_MAX_OBJECTS_V ( 1 << VS_HANDLE_INDEX_BITS_V )
#define VS_MAX_GENERIC_OBJECTS_PER_SESSION_V ( 32 )

/* This value is decided considering that one session per voice system. */
#define VS_MAX_NUM_SESSIONS_V ( 2 )
/* This value is bounded by CVS max session name size (9) */
#define VS_MAX_CVD_SESSION_NAME_SIZE_V ( 9 )
/* This value is based on the CVD version string*/
#define VS_MAX_CVD_VERSION_SIZE_V ( 31 )

/* Definitions for error checking */
#define VS_PARAM_UINT32_NOT_SET         (0xFFFFFFFF)
#define VS_PARAM_UINT8_NOT_SET          (0xFE)
#define VS_PARAM_UINT8_NOT_APPLICABLE   (0xFF)

/* To accomadate max frame size of vocoders supported which is for EVS/PCM_48KHz */
#if(defined VS_PCM_VOCODER)
   #define VS_MBOX_BUF_SIZE   ( 2048 )
#else
   #define VS_MBOX_BUF_SIZE   ( 384 )
#endif

/* To accomadate PCM frame for 48khz */
#define VS_HPCM_BUF_SIZE   ( 2048 ) 

#define VS_CACHE_LINE_SIZE ( 128 )
#define VS_PAGE_ALIGN_SIZE ( 4096 )
#define VS_MIN_DATA_WIDTH  ( 8 )
#define VS_MAX_DATA_WIDTH  ( 64 )

/**
 * Default timing parameters. 
 * Applicable to any network that doesn't have its own timing parameters.
 */
#define VS_VOC_TIMING_ENC_OFFSET_V     ( 8000 )
#define VS_VOC_TIMING_DEC_REQ_OFFSET_V ( 3300 )
#define VS_VOC_TIMING_DEC_OFFSET_V     ( 8300 )

/**
 * None of the timing offsets should be less than 100us.
 * This margin is maintained to handle VFR jitter.
 */
#define VS_VOC_MIN_TIMING_OFFSET_V     ( 100 )

/**
 * Margin for delivering decoder packet from the instance Dec packet
 * request is triggered.
 */
#define VS_VOC_TIMING_DEC_OFFSET_MARGIN_V     ( 1000 )

/**
 * Margin for delivering encoder packet before the client reads it.
 */
#define VS_VOC_TIMING_ENC_OFFSET_MARGIN_V     ( 1000 )

/*
 * Margin(from current TS ) to make sure that VFR start TS 
 * passed to VSTMR has not elapsed. This is needed as VSTMR 
 * needs future TS for kickstarting the VFR, if TS had already
 * elapsed then VSTMR shall kickstart the VFR at current TS.
 */
#define VS_VOC_TIMING_VFR_START_MARGIN_V     ( 2000 )


/** Bit position for the MUX_ID in the VSID
 *  BIT: 21:18  */
#define VS_MUX_ID_MASK_BIT_POS    (18) 

/** MUX_ID bit mask value in the VSID
 *  4-bits for MUX ID */
#define VS_MUX_ID_MASK_VAL        (0xF)


/** 
 * Vocode frame synchronization modes.
 */
#define VS_VOC_VFR_MODE_HARD_V (1)
#define VS_VOC_VFR_MODE_SOFT_V (0)


/**
 * This is and internal command for reseting the vocoder state
 * of each VS session, and cleaning up all the CVD reference
 * created by VS, after recieving any of below events
 *
 * Audio-PD DOWN, CVD service DOWN and ADSP SSR events.
 */
#define VS_VOC_CMDI_CLEANUP_CVD_REFERENCES ( 0x000130EA )


/** Memory Statistics Command (synchronous blocking API).
 *
 * Use this command to check the apr object usage of VS
 *
 * Payload: vs_voc_mem_use_stat_t
 *
 */
#define VS_VOC_CMD_MEM_USE_STAT ( 0x00013257 ) 

typedef struct vs_voc_mem_use_stat_t {

  uint32_t objects_used;
} vs_voc_mem_use_stat_t;

#define UNCOMMENT 0

#define VS_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "VS: Error[0x%08x], vs_state=%d", \
                            rc, vs_is_initialized, 0 ); } }

#define VS_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL,\
                      "VS: Error[0x%08x], vs_state=%d", \
                            rc, vs_is_initialized ); } }

#define VS_ACQUIRE_LOCK( lock ) \
  apr_lock_enter( lock );

#define VS_RELEASE_LOCK( lock ) \
  apr_lock_leave( lock );

#define VS_MODULO_INCREMENT( X, Y, Z ) \
          { ( X ) = ( ( ( X ) + ( Y ) ) % ( Z ) ); }

#define VS_IS_CURCULAR_BUF_FULL( Q, INCR_SIZE, TOTAL_SIZE, reclaim_offset ) \
          ( ( ( Q->write_offset + INCR_SIZE ) % TOTAL_SIZE ) == reclaim_offset )

/* Round up input to multiple of align. */
#define VS_ROUND_UP_ALIGN( input, align ) \
          ( ( input + align - 1 ) & ( ~ ( align - 1 ) ) )

#define VS_MAX_NUM_OF_MBOX_BUF_REQUESTS ( 3 )
#define VS_MAX_NUM_OF_MBOX_DATA_SLOTS   ( 4 )


/****************************************************************************
     VS GLOBAL VARIABLES DECLARATIONS
****************************************************************************/

/* flag that indicates the state of audio service (CVD/ADSP). */
extern bool_t vs_cvd_service_is_up;

/* flag that indicates the state of servreg locator service (MPSS/APSS). */
extern SERVREG_SERVICE_STATE vs_servreg_locator_service_state;

/**
 * Should be made false during initialization.
 *
 * vs_cvd_cleanup_is_done is common for cleanup during ADSP SSR/PDR.
 * and during ADSP Timeout.
 */
extern bool_t vs_cvd_cleanup_is_done;

extern bool_t vs_adsp_timeout_is_triggered;

extern bool_t vs_pdr_fwk_is_available;

extern bool_t vs_registered_with_pdr;

extern bool_t vs_registered_with_ssr;

extern bool_t vs_adsp_mbox_memory_config_is_received;

extern bool_t vs_platform_is_fusion;

extern uint64_t vs_shared_mem_table_virt_addr_start;
extern uint64_t vs_shared_mem_table_phy_addr_start;
extern uint32_t vs_shared_mem_table_size;
extern uint32_t vs_shared_mem_table_handle;

#if defined (VS_PROFILING)
extern unsigned long long vs_cmd_ticks_in_us;
#endif /* VS_PROFILING */

#if defined (VS_PROFILING_DATA_PCIE)
extern unsigned long long vs_data_begin_ticks_in_us;
extern unsigned long long vs_data_end_ticks_in_us;
#endif /* VS_PROFILING_DATA_PCIE */

/****************************************************************************
  VS TYPE DEFINITIONS
****************************************************************************/

/**
 * The following macros are here for temporary purposes.
 * Once the voicecfg is preset, we do not need this. Instead
 */
typedef enum vs_vsid_enum_t 
{

  VS_VSID_SUB1 = 0x11C05000,
  VS_VSID_SUB2 = 0x11DC5000
 
} vs_vsid_enum_t;


typedef enum vs_thread_state_enum_t
{

  VS_THREAD_STATE_ENUM_INIT,
  VS_THREAD_STATE_ENUM_READY,
  VS_THREAD_STATE_ENUM_EXIT

} vs_thread_state_enum_t;


typedef struct vs_work_item_t {

  apr_list_node_t link;
  aprv2_packet_t* packet;

} vs_work_item_t;


typedef struct vs_generic_item_t {

  apr_list_node_t link;
  uint32_t handle;
    /**< Any custom handle. */

} vs_generic_item_t;


/****************************************************************************
  COMMAND RESPONSE FUNCTION TABLE
****************************************************************************/

typedef void ( *vs_event_handler_fn_t ) ( 
  aprv2_packet_t* packet
);

typedef uint32_t ( *vs_voc_set_param_fn_t ) ( 
  void* this,
  uint32_t param_id,
  void* param_data
);

typedef uint32_t ( *vs_voc_set_cached_param_fn_t ) ( 
  void* this
);

typedef enum vs_response_fn_enum_t {

  VS_RESPONSE_FN_ENUM_ACCEPTED,
  VS_RESPONSE_FN_ENUM_RESULT,
  VS_RESPONSE_FN_ENUM_MEMORY_MAP,
  VS_RESPONSE_FN_ENUM_AVTIMER_OFFSET,
  VS_RESPONSE_FN_ENUM_MBOX_MEMORY_CONFIG,
  VS_RESPONSE_FN_ENUM_CVD_VERSION,
  VS_RESPONSE_FN_ENUM_INVALID,
  VS_RESPONSE_FN_ENUM_MAX = VS_RESPONSE_FN_ENUM_INVALID
}
  vs_response_fn_enum_t;

/*
 * Pending commands may load different sets of response and event handlers to
 * complete each job. The response function table is equivalent to the state
 * design pattern. The state context is stored in the gating command control.
 * Pending commands can be as simple or as complex as required.
 */
typedef vs_event_handler_fn_t vs_response_fn_table_t[ VS_RESPONSE_FN_ENUM_MAX ];

/****************************************************************************
 * VOICE SESSION SETUP CONTROL DEFINITIONS                                  *
 ****************************************************************************/

typedef enum vs_voc_state_enum_t
{
  VS_VOC_STATE_ENUM_UNINITIALIZED,
    /**< Reserved. */
  VS_VOC_STATE_ENUM_RESET_ENTRY,
    /**< Move into or out of reset. */
  VS_VOC_STATE_ENUM_RESET,
    /**< The voice session resource( CVD ) is not created/acquired. */
  VS_VOC_STATE_ENUM_IDLE_ENTRY,
    /**< Move into or out of idle. */
  VS_VOC_STATE_ENUM_IDLE,
    /**< The voice session resource( CVD ) is acquired and ready to be
      * configure/run.
      */
  VS_VOC_STATE_ENUM_RUN_ENTRY,
    /**< Move into or out of run. */
  VS_VOC_STATE_ENUM_RUN,
    /**< The Voice session resource ( CVD ) is running. */
  VS_VOC_STATE_ENUM_INVALID,
    /**< Reserved. */
  VS_VOC_STATE_ENUM_MAX
}
  vs_voc_state_enum_t;


typedef enum vs_voc_goal_enum_t
{
  VS_VOC_GOAL_ENUM_UNINITIALIZED,
  VS_VOC_GOAL_ENUM_NONE,
  VS_VOC_GOAL_ENUM_CREATE,
  VS_VOC_GOAL_ENUM_DESTROY,
  VS_VOC_GOAL_ENUM_CONFIGURE,
  VS_VOC_GOAL_ENUM_ENABLE,
  VS_VOC_GOAL_ENUM_DISABLE,
  VS_VOC_GOAL_ENUM_INVALID,
  VS_VOC_GOAL_ENUM_MAX,
}
  vs_voc_goal_enum_t;

typedef enum vs_voc_action_enum_t
{
  VS_VOC_ACTION_ENUM_UNINITIALIZED,
    /**< Common actions. */
  VS_VOC_ACTION_ENUM_NONE,
    /**< The first action has not started for a goal from any state. */
  VS_VOC_ACTION_ENUM_COMPLETE,
    /**<
     * Reached the last action for a goal from a state. A multi-action goal
     * that starts from and ends in the same state may require a COMPLETE
     * action to properly differentiate a terminate signal.
     */
  VS_VOC_ACTION_ENUM_CONTINUE,
    /**<
     * For multi-state goals, the last action from each state should set to
     * CONINTUE. This indicates to the next state that a goal is continuing
     * its operation from a previous state. Usually the previous state is
     * known given the current state and the continued goal. New actions can
     * be created to help discriminate the direction from where goals come
     * from as required.
     */
  VS_VOC_ACTION_ENUM_START_VOICE,
  VS_VOC_ACTION_ENUM_STOP_VOICE,
  VS_VOC_ACTION_ENUM_STANDBY_VOICE,
  VS_VOC_ACTION_ENUM_CREATE_MVM,
  VS_VOC_ACTION_ENUM_DESTROY_MVM,
  VS_VOC_ACTION_ENUM_SET_NETWORK_ON_MVM,
  VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_ON_MVM,
  VS_VOC_ACTION_ENUM_SET_VOICE_TIMING,
  VS_VOC_ACTION_ENUM_MEMORY_MAP,
  VS_VOC_ACTION_ENUM_MEMORY_UNMAP,
  VS_VOC_ACTION_ENUM_SET_VFR_CONFIG,
  VS_VOC_ACTION_ENUM_SET_TIMING_REFERENCE,
  VS_VOC_ACTION_ENUM_SET_MAX_SAMPLE_RATE,
  VS_VOC_ACTION_ENUM_GET_AVTIMER_OFFSET,
    /**< Above acion are associated to MVM. */

  VS_VOC_ACTION_ENUM_CREATE_CVS,
  VS_VOC_ACTION_ENUM_DESTROY_CVS,
  VS_VOC_ACTION_ENUM_ATTACH_CVS,
  VS_VOC_ACTION_ENUM_DETACH_CVS,
  VS_VOC_ACTION_ENUM_LISTEN_EAMR,
  VS_VOC_ACTION_ENUM_CANCEL_LISTEN_EAMR,
  VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_OVER_CVS,
  VS_VOC_ACTION_ENUM_SET_CACHED_STREAM_PROPERTIES,
  VS_VOC_ACTION_ENUM_RESYNC_CTM,
  VS_VOC_ACTION_ENUM_SET_PKTEXG_MODE,
  VS_VOC_ACTION_ENUM_SET_MBOX_CONFIG,
  VS_VOC_ACTION_ENUM_REGISTER_OOBTTY,
  VS_VOC_ACTION_ENUM_DEREGISTER_OOBTTY,
  VS_VOC_ACTION_ENUM_ENABLE_HPCM,
  VS_VOC_ACTION_ENUM_DISABLE_HPCM,
    /**< Above aciton's are associated to CVS/STREAM. */
  VS_VOC_ACTION_ENUM_VFR_ALLOCATE_AND_SYNC,
  VS_VOC_ACTION_ENUM_VFR_RELEASE,
    /**< Above aciton's are associated to VFR for VoLTE. */
  VS_VOC_ACTION_ENUM_REQUEST_MAILBOX_MEMORY_CONFIG,
  VS_VOC_ACTION_ENUM_GET_CVD_VERSION,
  VS_VOC_ACTION_ENUM_INVALID,
  VS_VOC_ACTION_ENUM_MAX,
}
  vs_voc_action_enum_t;


typedef struct vs_control_t {

  uint32_t transition_job_handle;
  uint32_t pendjob_handle;

  vs_voc_goal_enum_t goal;
    /**<
     * The goal field is the current goal for the state machine. Goals are
     * defined to complete one at a time.
     *
     * CAUTION: The goal field is intended for the user of the state machine
     *          to indicate the goal. Any attempt by the state machine to
     *          manipulate the goal will lead to live locking and
     *          unmaintainable code.
     */
  vs_voc_state_enum_t state;
    /**<
     * The state field is the current state of the state machine.
     *
     * CAUTION: The state field is intended for the state machine to keep
     *          track of its current state. Any attempt to manipulate the
     *          state field from outside the state machine will lead to
     *          live locking and unmaintainable code.
     */
  vs_voc_action_enum_t action;
    /**<
     * The action field is the next action to perform by the state machine.
     *
     * CAUTION: The action field is intended by the state machine to keep
     *          track of the series of steps to transition out of the current
     *          state or to complete a goal. Any attempt to manipulate the
     *          action field from outside the state machine will lead to
     *          live locking, system panics due to unhandled actions in the
     *          state, and unmaintainable code.
     */
  uint32_t status;
    /**<
     * The status field reports the status of the last action.
     */
}
  vs_control_t;


/****************************************************************************
 * VS Object Definitions
 ****************************************************************************/

typedef enum vs_object_type_enum_t  {

  VS_OBJECT_TYPE_ENUM_UNINITIALIZED,
  VS_OBJECT_TYPE_ENUM_HANDLE,
  VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
  VS_OBJECT_TYPE_ENUM_SEQUENCER_JOB,
  VS_OBJECT_TYPE_ENUM_SESSION,
  VS_OBJECT_TYPE_ENUM_INVALID
}
 vs_object_type_enum_t;


typedef struct vs_object_header_t {

 uint32_t handle;
  /**< The handle to the associated apr_objmgr_object_t instance. */
 vs_object_type_enum_t type;
  /**<
   * The object type defines the actual derived object.
   *
   * The derived object can be any custom object type. A session or a
   * command are two such custom object types. A free object entry is set
   * to VS_OBJECT_TYPE_ENUM_FREE.
   */
}
  vs_object_header_t;


typedef struct vs_token_object_t {

 vs_object_header_t header;

 vs_response_fn_table_t fn_table;
   /**<
    * This is the response function v-table. The response table can store
    * custom response routines for all possible responses directed to this
    * specific job.
    */
}
  vs_token_object_t;


typedef struct vs_simple_job_object_t {

  vs_object_header_t header;

  uint32_t context_handle;
    /**<
     * The parent-job handle indicates this sub-job is part of a batch-job.
     *
     * Set this value to -1 when it is unused or when there is no parent.
     */
  vs_response_fn_table_t fn_table;
    /**<
     * This is the response function v-table. The response table can store
     * custom response routines for all possible responses directed to this
     * specific job.
     */
  bool_t is_accepted;
    /**< The command accepted response flag. 0 is false and 1 is true. */
  bool_t is_completed;
    /**< The command completed response flag. 0 is false and 1 is true. */
  uint32_t status;
    /**< The status returned by the command completion. */
}
  vs_simple_job_object_t;


typedef struct vs_sequencer_job_object_t {

  vs_object_header_t header;

  uint32_t state;
    /**< The generic state variable. */
 union vs_object_t* subjob_obj;
    /**< The current sub-job object. */
  uint32_t status;
    /**< A status value. */
}
  vs_sequencer_job_object_t;


/**
 * supporting structure definitions for session object.
 */
typedef struct vs_voc_timing_t {

  uint16_t enc_offset_us;
  uint16_t dec_req_offset_us;
  uint16_t dec_offset_us;

  bool_t is_timing_ref_available;
  uint64_t tx_ref_timestamp_us;
  uint64_t rx_ref_timestamp_us;
  uint64_t vfr_start_timestamp_us;
  int64_t avtimerOffsetUs;
}
  vs_voc_timing_t;


typedef struct vs_voc_vfr_t {

  uint32_t vfr_mode;
  uint32_t vfr_cycle_duration_ms;
}
  vs_voc_vfr_t;


/* This represent a shared memory block generically.
 * It has to be appropriately typecasted for usage.
 */
typedef struct vs_shmem_block_t {

  uint64_t virtAddr;
  uint64_t phyAddr;
  uint32_t size;
}
  vs_shmem_block_t;

typedef struct vs_shmem_block_list_item_t {

  apr_list_node_t link;
  vs_shmem_block_t buf;
}
  vs_shmem_block_list_item_t;

typedef struct vs_mbox_config_t {

  vs_shmem_block_t encCircBuf;
  uint32_t encCircBufDataSize; //Idenitfies number of data slots
  uint64_t encReclaimOffset;
  apr_list_t usedEncBufferQ;
  apr_list_t freeEncBufferQ;
  vs_shmem_block_list_item_t encBuffers[VS_MAX_NUM_OF_MBOX_BUF_REQUESTS];

  vs_shmem_block_t decCircBuf;
  uint32_t decCircBufDataSize; //Idenitfies number of data slots
  uint64_t decReclaimOffset;
  apr_list_t usedDecBufferQ;
  apr_list_t freeDecBufferQ;
  vs_shmem_block_list_item_t decBuffers[VS_MAX_NUM_OF_MBOX_BUF_REQUESTS];
}
  vs_mbox_config_t;


typedef enum vs_client_state_enum_t  {

  VS_CLIENT_STATE_ENABLED,
  VS_CLIENT_STATE_DISABLED,
  VS_CLIENT_STATE_CLOSED
}
  vs_client_state_enum_t;

typedef struct vs_vocoder_t {

  uint32_t client_id;
  vs_common_event_callback_fn_t client_cb;
  void* session_context;
  vs_client_state_enum_t client_state;

  uint32_t media_id;
  bool_t is_eamr_enabled;

  vs_voc_timing_t timing_config;
  vs_voc_vfr_t vfr_config;
  vs_mbox_config_t mbox_config;

  uint8_t tty_enable;
  uint8_t pktexg_event_enable;
  uint32_t tx_sample_rate;
  uint32_t rx_sample_rate;

  /** Function pointers to have vocoder specific funtionality. */
  vs_voc_set_param_fn_t set_param_fn;
  vs_voc_set_cached_param_fn_t set_cached_param_fn;
  vs_voc_set_cached_param_fn_t publish_param_fn;

  /** vocoder specific properties which shall be published. */
  uint8_t plc;
  uint8_t encoder_rate;
  uint8_t encoder_bw;
  uint8_t decoder_bw;
  uint8_t min_rate;
  uint8_t max_rate;
  uint8_t channel_awareness_mode;
  uint8_t fer_rate;
  uint8_t fec_offset;
  uint32_t dtx;
  uint32_t dtx_param_version;
}
  vs_vocoder_t;

typedef struct vs_hpcm_t {

  vs_common_event_callback_fn_t client_cb;
  void* session_context;
  vs_client_state_enum_t client_state;

  uint8_t txMode;
  uint32_t txSampleRate;

  vs_shmem_block_t txReadBlk;
  vs_hpcm_buffer_t txReadBuf;

  vs_shmem_block_t txWriteBlk;
  vs_hpcm_buffer_t txWriteBuf;

  uint8_t rxMode;
  uint32_t rxSampleRate;

  vs_shmem_block_t rxReadBlk;
  vs_hpcm_buffer_t rxReadBuf;

  vs_shmem_block_t rxWriteBlk;
  vs_hpcm_buffer_t rxWriteBuf;
}
  vs_hpcm_t;

typedef struct vs_session_object_t {

  vs_object_header_t header;

  uint32_t vsid;
    /**< System level published/documented Voice System ID. */
  char_t session_name[ VS_MAX_CVD_SESSION_NAME_SIZE_V ];
    /** Voice system name derived from VSID. */
  apr_lock_t session_lock;
    /**< Lock to synchronize shared session members access. */
  vs_control_t session_ctrl;
    /**< Maintains Voice session state, and it's state transition,
      *  as per the policy management. */

  /** shared memory related data. */
  uint64_t mem_virt_addr_start;
  uint64_t mem_phy_addr_start;
  uint32_t mem_size;
  uint32_t voicemem_handle;

  //CVD specific variables.
  uint16_t cvs_port;
    /**< CVS session handle. */
  uint16_t mvm_port;
    /**< MVM session handle. */
  uint32_t memmap_handle;
    /**< memory map handle from ADSP. */
  bool_t is_stream_ready;
    /**< Indicates that stream is running. */

  vs_vocoder_t vocoder;
    /**< Instance of the vocoder object active during the call. */

  vs_hpcm_t hpcm;
    /**< Instance of the HPCM object. */

  uint32_t ipa_ul_handle;
  uint32_t ipa_dl_handle;
  /**< IPA handles for performing memory copy 
        operations between MDM and APQ DDR. */
  vs_voc_event_path_delay_t path_delay;

  char_t cvd_version[VS_MAX_CVD_VERSION_SIZE_V];
}
  vs_session_object_t;


typedef union vs_object_t {

  vs_object_header_t header;
  vs_token_object_t handle;
  vs_simple_job_object_t simple_job;
  vs_sequencer_job_object_t sequencer_job;
  vs_session_object_t session_obj;
}
  vs_object_t;


typedef enum vs_gating_cmd_state_enum_t {

  VS_GATING_CMD_STATE_ENUM_FETCH,
  VS_GATING_CMD_STATE_ENUM_EXECUTE,
  VS_GATING_CMD_STATE_ENUM_CONTINUE
}
  vs_gating_cmd_state_enum_t;


typedef struct vs_gating_control_t {

  apr_list_t cmd_q;
  /**< The gating (vs_work_item_t) command queue. */
  vs_gating_cmd_state_enum_t state;
  /**<
   * The current state of the gating command control.
   *
   * This variable is managed by the gating command processor. The
   * individual gating command controls indicates to the gating command
   * processor to complete or to delay the completion of the current
   * gating command.
   */
  aprv2_packet_t* packet;
  /**<
   * The current (command) packet being processed.
   */
  vs_object_t* rootjob_obj;
  /**<
   * The rootjob_obj is a temporary storage for the current gating
   * command.
   */
}
  vs_gating_control_t;

/****************************************************************************
 * VS EXTERN DECLARATIONS
 ****************************************************************************/

/* flag that indicates the state of vs task. */
extern bool_t vs_is_initialized;

extern uint32_t vs_apr_handle;
extern uint16_t vs_my_addr;
extern uint16_t vs_mvm_addr;
extern uint16_t vs_cvs_addr;

/****************************************************************************
 * VS COMMON INTERNAL FUNCTIONS
 ****************************************************************************/

void vs_cmdq_lock_fn ( 
  void
);

void vs_cmdq_unlock_fn ( 
  void
);

void vs_signal_run ( 
  void
);

uint32_t vs_prepare_and_dispatch_cmd_packet (
  uint32_t cmd_id,
  void* p_params,
  uint32_t params_size
);

uint32_t vs_common_send_event (
  uint32_t handle,
  uint32_t event_id,
  void* p_rsp,
  uint32_t rsp_size
);

uint32_t vs_create_autofree_token_object (
  vs_token_object_t** p_ret_obj
);

uint32_t vs_create_simple_job_object (
  uint32_t context_handle,
  vs_simple_job_object_t** p_ret_job_obj
);

uint32_t vs_create_sequencer_job_object (
  vs_sequencer_job_object_t** p_ret_job_obj
);

uint32_t vs_get_object (
  uint32_t handle,
  vs_object_t** ret_obj
);

uint32_t vs_get_typed_object (
  uint32_t handle,
  vs_object_type_enum_t type,
  vs_object_t** ret_obj
);

uint32_t vs_mem_free_object (
  vs_object_t* p_object
);

uint32_t vs_free_all_tracked_objects (
  void
);

/****************************************************************************
 * VS COMMAND HANDLERS.
 ****************************************************************************/

uint32_t vs_cmd_mem_use_stat_cmd_ctrl (
  vs_voc_mem_use_stat_t* param
);

uint32_t vs_voc_open_cmd_ctrl (
  vs_voc_cmd_open_t* params
);

uint32_t vs_set_param_cmd_ctrl (
  vs_common_cmd_set_param_t* params
);

uint32_t vs_voc_enable_cmd_ctrl (
  vs_gating_control_t* ctrl
);

uint32_t vs_voc_disable_cmd_ctrl (
  vs_gating_control_t* ctrl
);

uint32_t vs_voc_close_cmd_ctrl (
  vs_gating_control_t* ctrl
);

uint32_t vs_get_encoder_buffer_cmd_ctrl (
  vs_pktexg_cmd_get_encoder_buffer_t* params
);

uint32_t vs_return_encoder_buffer_cmd_ctrl (
  vs_pktexg_cmd_return_encoder_buffer_t* params
);

uint32_t vs_get_decoder_buffer_cmd_ctrl (
  vs_pktexg_cmd_get_decoder_buffer_t* params
);

uint32_t vs_put_decoder_buffer_cmd_ctrl (
  vs_pktexg_cmd_put_decoder_buffer_t* params
);

uint32_t vs_flush_buffer_cmd_ctrl (
  vs_pktexg_cmd_flush_buffers_t* params
);

uint32_t vs_voc_cleanup_adsp_reference_cmd_ctrl (
  vs_gating_control_t* ctrl
);

uint32_t vs_hpcm_open_cmd_ctrl (
  vs_hpcm_cmd_open_t* params
);

uint32_t vs_hpcm_enable_cmd_ctrl (
  vs_gating_control_t* ctrl
);

uint32_t vs_hpcm_disable_cmd_ctrl (
  vs_gating_control_t* ctrl
);

uint32_t vs_hpcm_close_cmd_ctrl (
  vs_gating_control_t* ctrl
);

uint32_t vs_hpcm_push_buffer_cmd_ctrl (
  vs_hpcm_cmd_push_buffer_t* params
);

uint32_t vs_push_oobtty_char_cmd_ctrl (
  vs_oobtty_cmd_push_char_t* cmd_params
);

/****************************************************************************
 * VS EVENT & RESPONSE HANDLERS.
 ****************************************************************************/

void vs_response_fn_trampoline (
  uint32_t fn_index,
  aprv2_packet_t* packet
);

uint32_t vs_stream_ready_event_handler (
  aprv2_packet_t* packet
);

uint32_t vs_stream_not_ready_event_handler (
  aprv2_packet_t* packet
);

uint32_t vs_stream_eamr_mode_change_event_handler (
  aprv2_packet_t* packet
);

uint32_t vs_stream_dec_buf_request_event_handler (
  aprv2_packet_t* packet
);

uint32_t vs_stream_enc_buf_ready_event_handler (
  aprv2_packet_t* packet
);

uint32_t vs_stream_oobtty_char_notify_event_handler (
  aprv2_packet_t* packet
);

uint32_t vs_stream_hpcm_notify_event_handler (
  aprv2_packet_t* packet
);

void vs_voc_transition_result_rsp_fn (
  aprv2_packet_t* packet
);

void vs_voc_mailbox_memory_config_rsp_fn (
  aprv2_packet_t* packet
);

void vs_voc_track_result_rsp_fn (
  aprv2_packet_t* packet
);

void vs_voc_memory_map_result_rsp_fn (
  aprv2_packet_t* packet
);

void vs_voc_get_avtimer_offset_rsp_fn (
  aprv2_packet_t* packet
);

void vs_voc_create_mvm_session_result_rsp_fn (
  aprv2_packet_t* p_packet
);

void vs_voc_create_cvs_session_result_rsp_fn (
  aprv2_packet_t* p_packet
);

uint32_t vs_path_delay_notify_event_handler (
  aprv2_packet_t* packet
);

void vs_voc_get_cvd_version_rsp_fn (
  aprv2_packet_t* packet
);

/****************************************************************************
 * VS STATE CONTROL HANDLERS.
 ****************************************************************************/

/** Helper State to modularize the CVD access. */
enum {
  VS_HELPER_SEQUENCER_ENUM_UNINITIALIZED,
  VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE,
  VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE_WAIT,
  VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE,
  VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE_WAIT,
  VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE,
  VS_HELPER_SEQUENCER_ENUM_GOAL_ENABLE_WAIT,
  VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE,
  VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE_WAIT,
  VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY,
  VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY_WAIT,
  VS_HELPER_SEQUENCER_ENUM_COMPLETE,
  VS_HELPER_SEQUENCER_ENUM_INVALID
};

uint32_t vs_voc_helper_vocoder_setup_ctrl (
  vs_sequencer_job_object_t* seq_obj,
  vs_session_object_t* session_obj
);

/****************************************************************************
 * VS ACTION HANDLERS.
 ****************************************************************************/

uint32_t vs_voc_action_create_mvm (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_destroy_mvm (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_create_cvs (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_request_mailbox_memory_config (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_destroy_cvs (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_attach_stream (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_detach_stream (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_listen_eamr (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_cancel_eamr (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_set_media_type_on_stream (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_set_network_type_on_mvm (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_set_media_type_on_mvm (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_set_voice_timing_params (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_calculate_timing_offsets (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_set_timing_references (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_set_vfr_config (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_send_resync_ctm (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_configure_vfr (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_release_vfr (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_send_start_voice (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_send_stop_voice (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_set_plc (
  vs_session_object_t * session_obj
);

uint32_t vs_voc_action_set_dtx ( 
  vs_session_object_t * session_obj
);

uint32_t vs_voc_action_set_min_max_rate (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_set_max_sample_rate ( 
   vs_session_object_t* session_obj
);

uint32_t vs_voc_action_register_oobtty ( 
   vs_session_object_t* session_obj
);

uint32_t vs_voc_action_deregister_oobtty ( 
   vs_session_object_t* session_obj
);

uint32_t vs_voc_action_map_memory (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_unmap_memory (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_get_avtimer_offset ( 
   vs_session_object_t* session_obj
);

uint32_t vs_voc_action_hpcm_start (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_hpcm_stop (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_send_pktexg_mode (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_set_mbox_config (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_put_mailbox_enc_request (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_read_enc_mailbox (
  vs_session_object_t* session_obj,
  vs_pktexg_buffer_t** ret_buffer
);

uint32_t vs_voc_action_put_mailbox_dec_request (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_flush_enc_mailbox (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_flush_dec_mailbox (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_initialize_mbox (
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_set_dtx_v2 ( 
  vs_session_object_t* session_obj
);

uint32_t vs_voc_action_get_cvd_version ( 
  vs_session_object_t* session_obj
);

/****************************************************************************
 * VOCODER SPECIFIC INITIALIZATION FUNCTION
 ****************************************************************************/

uint32_t vs_ssr_pdr_init (
  void
);

uint32_t vs_ssr_pdr_deinit (
  void
);

bool_t vs_is_cvd_up(
  void
);

uint32_t vs_vocevs_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocamr_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocamrwb_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocamrwb2_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocefr_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocfr_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vochr_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocevrca_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocevrcb_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocevrcwb_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocevrcnw_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocevrcnw2k_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocqcelp13k_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocg711alaw_v2_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocg711mulaw_v2_init (
  vs_session_object_t* session_obj
);

uint32_t vs_vocqcelp13k_init (
  vs_session_object_t* session_obj
);

#endif  /* __VS_I_H__ */


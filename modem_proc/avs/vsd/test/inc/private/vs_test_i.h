#ifndef __VS_TEST_I_H__
#define __VS_TEST_I_H__

/**
  @file  vs_test_i.h
  @brief This file contains internal definitions of Voice Service test client.
*/

/*
  ============================================================================

   Copyright (C) 2016 , 2018-2019 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/test/inc/private/vs_test_i.h#1 $
  $Author: pwbldsvc $

  ============================================================================
*/

/****************************************************************************
  Include files for Module
****************************************************************************/

#include "sys.h"
#include "mmdefs.h"
#include "apr_list.h"
#include "apr_timer.h"
#include "aprv2_packet.h"
#include "aprv2_api_inline.h"


/****************************************************************************
  VS_TEST DEFINES
****************************************************************************/

/* Defined 1500 bytes Considering max 3 session/subscriptions .
 *
 * Note: one session consistes of one vs_test_session_object_t and one 
 *       vs_test_modem_subs_object_t
 */
#define VS_TEST_HEAP_SIZE_V ( 2048 )
#define VS_TEST_STACK_SIZE_V ( 2048 )

/* 320 for EVS and G711 Vocoders*/
#define VS_TEST_MAX_VOC_FRAME_LENGTH ( 320 )

/* Size of VS work packet queue. */
#define VS_TEST_NUM_WORK_PKTS_V ( 20 )

#define VS_TEST_HANDLE_TOTAL_BITS_V ( 2 )
#define VS_TEST_HANDLE_INDEX_BITS_V ( 1 ) /**< 1 bits = 1 handles. */
#define VS_TEST_MAX_OBJECTS_V ( 1 << VS_TEST_HANDLE_INDEX_BITS_V )

#define SIZE_OF_ARRAY(a) (sizeof(a) / sizeof((a)[0]))

#define VS_TEST_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[0x%08x], vs_test_state=%d", \
                            rc, vs_test_is_initialized, 0 ); } }

#define VS_TEST_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Error[0x%08x], "\
                       "vs_test_state=%d", rc, vs_test_is_initialized ); } }

#define VS_TEST_ACQUIRE_LOCK( lock ) \
  apr_lock_enter( lock );

#define VS_TEST_RELEASE_LOCK( lock ) \
  apr_lock_leave( lock );

#define VS_TEST_VSID1 ( 0x11C05000 )

/* Definitions for error checking */
#define VS_TEST_VOCODER_ID_UNDEFINED_V ( 0xFFFFFFFF )
#define VS_TEST_VSID_UNDEFINED_V ( 0xFFFFFFFF )
#define VS_TEST_CODEC_MODE_UNDEFINED ( 0xFFFFFFFF )
#define VS_TEST_PROPERTY_NOT_SET_UINT32 ( 0xFFFFFFFF )
#define VS_TEST_PROPERTY_NOT_SET_UINT8 ( 0xFF )

#define VS_TEST_VOCEVS_CHANNEL_AWARE_MODE_DISABLE_V ( 0x0 )
#define VS_TEST_VOCEVS_CHANNEL_AWARE_MODE_ENABLE_V ( 0x1 )

#define VS_TEST_FRAME_RATE_UNDEFINED ( 0xFF )

#define VS_TEST_INTERNAL_EVENT_LOOPBACK_BUFFER (0x00013339)

/** Default media type ID. */
#define VS_TEST_MEDIA_ID_NONE ( 0x00010FC0 )

/****************************************************************************
  VS_TEST thread states
****************************************************************************/

typedef enum vs_test_thread_state_enum_t
{
  VS_TEST_THREAD_STATE_ENUM_INIT,
  VS_TEST_THREAD_STATE_ENUM_READY,
  VS_TEST_THREAD_STATE_ENUM_EXIT
}
  vs_test_thread_state_enum_t;

/***************************************************************************
  VS_TEST WORK REQUEST ( TYPE / ITEM ) DEFINITIONS                              *
****************************************************************************/

typedef struct vs_test_cmd_packet_t {
  
  uint32_t cmd_id;
    /**< Command id issued from client. */
  void* params;
    /**< Structure associated to each cmd_id. */
} vs_test_cmd_packet_t;

typedef struct vs_test_event_packet_t {
  
  void* session_context;
    /**< Reference to vocie adapter session object. */
  uint32_t event_id;
    /**< Event id issued from client. */
  void* params;
    /**< Structure associated to each event_id. */
} vs_test_event_packet_t;

typedef enum vs_test_work_item_packet_type_t
{
  VS_TEST_WORK_ITEM_PKT_TYPE_NONE,
  VS_TEST_WORK_ITEM_PKT_TYPE_CMD,
  VS_TEST_WORK_ITEM_PKT_TYPE_EVENT

} vs_test_work_item_packet_type_t;

typedef enum vs_test_work_item_queue_type_t
{
  VS_TEST_WORK_ITEM_QUEUE_TYPE_NONE,
  VS_TEST_WORK_ITEM_QUEUE_TYPE_NONGATING,
  VS_TEST_WORK_ITEM_QUEUE_TYPE_GATING,

} vs_test_work_item_queue_type_t;

typedef struct vs_test_work_item_t {
  
  apr_list_node_t link;
  
  aprv2_packet_t* packet;
   /**<
     * This is generic work packet.
     */
} vs_test_work_item_t;


/****************************************************************************
   THE COMMON OBJECT DEFINITIONS
****************************************************************************/

typedef enum vs_test_object_type_enum_t
{
   VS_TEST_OBJECT_TYPE_ENUM_UNINITIALIZED,
   VS_TEST_OBJECT_TYPE_ENUM_SIMPLE_JOB,
   VS_TEST_OBJECT_TYPE_ENUM_SESSION,
   VS_TEST_OBJECT_TYPE_ENUM_INVALID

} vs_test_object_type_enum_t;

typedef struct vs_test_object_header_t
{
  uint32_t handle;
   /**< The handle to the associated apr_objmgr_object_t instance. */
  vs_test_object_type_enum_t type;
   /**<
    * The object type defines the actual derived object.
    *
    * The derived object can be any custom object type. A session or a
    * command are two such custom object types. A free object entry is set
    * to VS_OBJECT_TYPE_ENUM_FREE.
    */
} vs_test_object_header_t;


/****************************************************************************
  THE SIMPLE JOB OBJECT
****************************************************************************/

typedef struct vs_test_simple_job_object_t vs_test_simple_job_object_t;

struct vs_test_simple_job_object_t {

  vs_test_object_header_t header;

  uint32_t context_handle;
   /**<
    * The parent-job handle indicates this sub-job is part of a batch-job.
    *
    * Set this value to -1 when it is unused or when there is no parent.
    */
  bool_t is_completed;
   /**< The command completed response flag. 0 is false and 1 is true. */
  uint32_t status;
   /**< The status returned by the command completion. */
};


/****************************************************************************
  THE SESSION OBJECT
****************************************************************************/

typedef struct vs_test_session_object_t vs_test_session_object_t;

struct vs_test_session_object_t {

  vs_test_object_header_t header;
  
  uint32_t vsid;
    /**< System level published/documented Voice System ID. */

  apr_lock_t data_lock;
    /**< Lock to synchornisze data access. */

  uint32_t vocoder_type;
    /**< Indicates the active vocoder type for the call. */

  bool_t dtx_mode;
    /**< Indicates the DTX mode for the current vocoder. */

/* AMR, AMR - WB, EVS */
  uint8_t codec_mode;
    /**< Indicates the vocoder type requested for the call. */

/* EVS */
  uint8_t bandwidth;
    /**< Indicates the bandwidth for the call. */

  uint8_t channel_aware_mode_enabled;
    /**< Inicates whether channel aware mode is enabled.
         1 - Enabled,
         0 - Disabled. */

  uint8_t max_tx_sr;
    /**< Indicates the max tx sampling rate for the call. */

  uint8_t max_rx_sr;
    /**< Indicates the max rx sampling rate for the call. */
  
  uint8_t fec_offset;
    /**< Specifies the forward error correction offset.
       @values 2, 3, 5, 7 */
  
  uint8_t fer_rate;
    /**< Specifies the frame error rate threshold.
          0 - Low
          1 - High */

/* EVRC vocoders */
  uint8_t avg_enc_rate;
    /**< Indicates the enc rate requested for the call. */

  uint8_t min_rate;
    /**< Minimum frame rate. */

  uint8_t max_rate;
    /**< Maximum frame rate. */

  uint8_t plc;

  uint32_t vs_handle;
    /**< Return handle for vocoder session. */

  apr_timer_t pktexg_timer;
    /**< Timer for scheduling UL/DL buffer delivery w.r.t SOFT VFR. */

  vs_pktexg_buffer_t* vs_read_buf;

  vs_pktexg_buffer_t* vs_write_buf;

  bool_t is_vs_ready;
    /**< Inidcates vocoder session is running. */
};

/****************************************************************************
  THE GENERIC VS_TEST OBJECT
****************************************************************************/

typedef union vs_test_object_t {

  vs_test_object_header_t header;
  vs_test_simple_job_object_t simple_job;
  vs_test_session_object_t session_obj;

} vs_test_object_t;


/****************************************************************************
 * PENDING COMMAND DEFINITIONS                                          *
 ****************************************************************************/

typedef enum vs_test_gating_cmd_state_t
{
  VS_TEST_GATING_CMD_STATE_FETCH,
  VS_TEST_GATING_CMD_STATE_EXECUTE,
  VS_TEST_GATING_CMD_STATE_CONTINUE

} vs_test_gating_cmd_state_t;

typedef struct vs_test_gating_control_t {
  
  apr_list_t cmd_q;
  /**< The gating (vs_test_work_item_t) queue. */
  vs_test_gating_cmd_state_t state;
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
  vs_test_object_t* rootjob_obj;
  /**<
   * The rootjob_obj is a temporary storage for the current gating
   * command.
   */
} vs_test_gating_control_t;


/****************************************************************************
 * VS_TEST INTERNAL ROUTINES                                                *
 ****************************************************************************/
uint32_t vs_test_queue_work_packet (
  vs_test_work_item_queue_type_t queue_type,
  aprv2_packet_t* packet
);

uint32_t vs_test_free_cmd_packet (
  vs_test_cmd_packet_t* cmd_packet
);

uint32_t vs_test_prepare_and_dispatch_cmd_packet (
  void* session_context,
  uint32_t cmd_id,
  void* params,
  uint32_t size
);

uint32_t vs_test_free_event_packet (
  vs_test_event_packet_t* event_packet
);

uint32_t vs_test_prepare_and_dispatch_event_packet (
  void* session_context,
  uint32_t event_id,
  void* params,
  uint32_t size
);

void vs_test_signal_run ( 
  void
);

/****************************************************************************
 * VS_TEST ENUM DEFINITIONS                                                 *
 ****************************************************************************/

/**
 * VS_TEST_LOOPBACK_SET_MEDIA_ID command opcodes
 */
 typedef enum vs_test_media_id_t {
  VS_TEST_MEDIA_ID_AMR_NB = 0,
  VS_TEST_MEDIA_ID_AMR_WB =1,
  VS_TEST_MEDIA_ID_EFR =2,
  VS_TEST_MEDIA_ID_FR = 3,
  VS_TEST_MEDIA_ID_HR = 4,
  VS_TEST_MEDIA_ID_EVRC = 5,
  VS_TEST_MEDIA_ID_4GV_NB = 6,
  VS_TEST_MEDIA_ID_4GV_WB = 7,
  VS_TEST_MEDIA_ID_4GV_NW = 8,
  VS_TEST_MEDIA_ID_4GV_NW2K = 9,
  VS_TEST_MEDIA_ID_13K = 10,
  VS_TEST_MEDIA_ID_G711_ALAW_V2 = 11,
  VS_TEST_MEDIA_ID_G711_MULAW_V2 = 12,
  VS_TEST_MEDIA_ID_EVS = 13,
/* PCM vocoders can be used by enabling feature flag in vs.scons . 
     PCM vocoder test capability is by default disabled in CRM builds*/
  VS_TEST_MEDIA_ID_PCM_8_KHZ = 14,
  VS_TEST_MEDIA_ID_PCM_16_KHZ = 15,
  VS_TEST_MEDIA_ID_PCM_32_KHZ = 16,
  VS_TEST_MEDIA_ID_PCM_48_KHZ = 17,
  VS_TEST_MEDIA_ID_EAMR = 18
 } vs_test_media_id_t;

/**
 * VS_TEST_LOOPBACK_SET_FRAME_RATE command opcodes
 */
 typedef enum vs_test_frame_rate_t {
  VS_TEST_FRAME_BLANK_RATE = 0,
  VS_TEST_FRAME_EIGHTH_RATE = 1,
  VS_TEST_FRAME_QUARTER_RATE = 2,
  VS_TEST_FRAME_HALF_RATE = 3,
  VS_TEST_FRAME_FULL_RATE = 4,
  VS_TEST_FRAME_ERASURE_RATE = 14,
  VS_TEST_FRAME_EIGHTH_RATE_NONCRITICAL = 255
 } vs_test_frame_rate_t;


/**
 * VS_TEST_LOOPBACK_SET_VOC_PROPERTY command opcodes
 */
 typedef enum set_voc_property_cmd{
  VS_TEST_VOC_PROPERTY_NONE = 0,
  VS_TEST_VOC_PROPERTY_MEDIA_ID = 1,
  VS_TEST_VOC_PROPERTY_DTX = 2,
  VS_TEST_VOC_PROPERTY_ENC_RATE = 3,
  VS_TEST_VOC_PROPERTY_MIN_MAX_RATE = 4,
  VS_TEST_VOC_PROPERTY_PLC = 5,
  VS_TEST_VOC_PROPERTY_MAX_TX_RX_SR = 6,
  VS_TEST_VOC_PROPERTY_CHANNEL_AWARE_MODE = 7,
  VS_TEST_VOC_PROPERTY_OPERATING_BANDWIDTH = 8,
  VS_TEST_VOC_PROPERTY_DTX_CONFIG = 9, /* Allows adaptive DTX mode as well */
  VS_TEST_VOC_PROPERTY_INVALID = 10
 } set_voc_property_cmd;

#endif  /* __VS_TEST_I_H__ */

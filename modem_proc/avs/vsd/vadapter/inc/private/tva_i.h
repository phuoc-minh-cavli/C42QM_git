#ifndef __TVA_I_H__
#define __TVA_I_H__

/**
  @file  tva_i.h
  @brief This file contains internal definitions of TDSCDMA voice adapter.
*/

/*
  ============================================================================
  Copyright (C) 2015 - 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/inc/private/tva_i.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
  Include files for Module
****************************************************************************/

/* MM APIs*/
#include "mmdefs.h"

/* APR APIs */
#include "apr_list.h"
#include "apr_lock.h"
#include "apr_timer.h"
#include "aprv2_packet.h"
#include "aprv2_api_inline.h"

/* WCDMA APIs. */
#include "tds_ext_api.h"

/* VOICE UTILS APIs. */
#include "voice_amr_if.h"
#include "voice_dsm_if.h"


/****************************************************************************
  TVA DEFINES
****************************************************************************/

/* Defined 4500 bytes Considering max 3 session/subscriptions .
 *
 * Note: one session consistes of one tva_session_object_t and one 
 *       tva_modem_subs_object_t
 */
#define TVA_HEAP_SIZE_V ( 3000 )

/* Size of VS work packet queue. */
#define TVA_NUM_WORK_PKTS_V ( 10 )

#define TVA_HANDLE_TOTAL_BITS_V ( 16 )
#define TVA_HANDLE_INDEX_BITS_V ( 4 ) /**< 4 bits = 16 handles. */
#define TVA_MAX_OBJECTS_V ( 1 << TVA_HANDLE_INDEX_BITS_V )

#define TVA_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[0x%08x], tva_state=%d", \
                            rc, tva_is_initialized, 0 ); } }

#define TVA_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Error[0x%08x], "\
                       "tva_state=%d", rc, tva_is_initialized ); } }

#define TVA_ACQUIRE_LOCK( lock ) \
  apr_lock_enter( lock );

#define TVA_RELEASE_LOCK( lock ) \
  apr_lock_leave( lock );

/* Definitions for error checking */
#define TVA_VOCODER_ID_UNDEFINED_V (0xFFFFFFFF)
#define TVA_VSID_UNDEFINED_V (0xFFFFFFFF)
#define TVA_CODEC_MODE_UNDEFINED ( 0xFF )


/**
 * Internal event triggered W-RLC queues channel data to AMR DSM queues.
 */
#define TVA_INTERNAL_EVENT_DELIVER_UL_BUFFER ( 0x00013189 )

/**
 * Internal event triggered to send silence frame if UL vocoder packet
 * is not available from DSP.
 */
#define TVA_INTERNAL_EVENT_DELIVER_DL_BUFFER ( 0x000131AE )

#define TVA_AMR_DSM_Q_LEN ( 1 )

/****************************************************************************
  TVA thread states
****************************************************************************/

typedef enum tva_thread_state_enum_t
{
  TVA_THREAD_STATE_ENUM_INIT,
  TVA_THREAD_STATE_ENUM_READY,
  TVA_THREAD_STATE_ENUM_EXIT
}
  tva_thread_state_enum_t;

/***************************************************************************
  TVA WORK REQUEST ( TYPE / ITEM ) DEFINITIONS                              *
****************************************************************************/

typedef struct tva_work_item_t {

  apr_list_node_t link;
  aprv2_packet_t* packet;
} tva_work_item_t;


/****************************************************************************
   THE COMMON OBJECT DEFINITIONS
****************************************************************************/

typedef enum tva_object_type_enum_t
{
   TVA_OBJECT_TYPE_ENUM_UNINITIALIZED,
   TVA_OBJECT_TYPE_ENUM_SIMPLE_JOB,
   TVA_OBJECT_TYPE_ENUM_SESSION,
   TVA_OBJECT_TYPE_ENUM_INVALID

} tva_object_type_enum_t;

typedef struct tva_object_header_t
{
  uint32_t handle;
   /**< The handle to the associated apr_objmgr_object_t instance. */
  tva_object_type_enum_t type;
   /**<
    * The object type defines the actual derived object.
    *
    * The derived object can be any custom object type. A session or a
    * command are two such custom object types. A free object entry is set
    * to VS_OBJECT_TYPE_ENUM_FREE.
    */
} tva_object_header_t;


/****************************************************************************
  THE SIMPLE JOB OBJECT
****************************************************************************/

typedef struct tva_simple_job_object_t {

  tva_object_header_t header;

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
} tva_simple_job_object_t;


/****************************************************************************
  THE SESSION OBJECT
****************************************************************************/

typedef struct tva_session_object_t {

  tva_object_header_t header;

  sys_modem_as_id_e_type asid;
    /**< Modem Active Subcription ID. */
  uint32_t vsid;
    /**< System level published/documented voice system ID. */
  apr_lock_t data_lock;
    /**< Lock to synchornisze data access. */

  uint32_t tds_handle;
    /**< Return handle for GSM voice instance. */
  bool_t is_tds_ready;
    /**< Indicates the readiness of GSM protocol software for voice 
      * traffic exchange.
      */
  uint32_t vocoder_type;
    /**< Indicates the active vocoder type for the call. */
  uint32_t codec_mode;
    /**< Indicates the vocoder type requested for the call. */
  bool_t dtx_mode;
    /**< Indicates the active dtx mode for the call. */

  bool_t is_resource_granted;
    /**< Indicates that vocoder resource access is granted. */
  uint8_t vocoder_state;
    /**< This represents possible vocoder activites/states.*/
  uint32_t enc_packet_miss_count;
    /**<
      * This maintain count of number of encoder frame missed from DSP
      * during voice call steady state.
      *
      * If this count crosses the defined threshold homing sequence are
      * delivered on uplink by the adapter assuming encoder reset would have
      * happed due to device switch, BBWE stream reconfiguration, new call
      * setup etc.
      */

  /** Refers to logical data channel as per AMRIF1 frame format.*/
  voice_amr_chan_state_t ul_chan_state;
    /**< Indicates actice AMR logical channels. */
  voice_amr_dsm_queue_t ul_queue;
    /**< AMR DSM queue for transmitting uplink AMR channels data. */
  voice_amr_chan_state_t dl_chan_state;
    /**< Indicates actice AMR logical channels. */
  voice_amr_dsm_queue_t dl_queue;
    /**< AMR DSM queue for retrieving downlink AMR channels data. */
  apr_timer_t pktexg_timer;
    /**< Timer for scheduling UL/DL buffer delivery w.r.t SOFT VFR. */
  bool_t is_timer_expiry_for_downlink;
    /*< Flag to indicated if timer expiry is for UL or DL buffer. */
  uint8_t is_dl_buffer_ready;
    /*< TBD. */

  /** Reference to VS session control.
    * Applicable if adapter is configured with asid to vsid mapping. */
  uint32_t vs_handle;
    /**< Return handle for vocoder session. */
  bool_t is_vs_ready;
    /**< Inidcates vocoder session is running. */
  vs_pktexg_buffer_t* vs_read_buf;
  vs_pktexg_buffer_t* vs_write_buf;

} tva_session_object_t;


/****************************************************************************
  THE GENERIC TVA OBJECT
****************************************************************************/

typedef union tva_object_t {
  
  tva_object_header_t header;
  tva_simple_job_object_t simple_job;
  tva_session_object_t session_obj;

} tva_object_t;


/****************************************************************************
 * PENDING MVS COMMAND DEFINITIONS                                          *
 ****************************************************************************/

typedef enum tva_gating_cmd_state_t
{
  TVA_GATING_CMD_STATE_FETCH,
  TVA_GATING_CMD_STATE_EXECUTE,
  TVA_GATING_CMD_STATE_CONTINUE

} tva_gating_cmd_state_t;

typedef struct tva_gating_control_t {
  
  apr_list_t cmd_q;
  /**< The gating (tva_work_item_t) queue. */
  tva_gating_cmd_state_t state;
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
   * This is generic reference to command packet, which could be
   * tva_cmd_packet_t or tva_event_packet_t.
   */
  tva_object_t* rootjob_obj;
  /**<
   * The rootjob_obj is a temporary storage for the current gating
   * command.
   */
} tva_gating_control_t;

#endif  /* __TVA_I_H__ */

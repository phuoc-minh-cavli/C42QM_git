#ifndef __WVA_I_H__
#define __WVA_I_H__

/**
  @file  wva_i.h
  @brief This file contains internal definitions of WCDMA voice adapter.
*/

/*
  ============================================================================
  Copyright (C) 2015 - 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/inc/private/wva_i.h#1 $
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
#include "wcdmamvsif.h"

/* VOICE UTILS APIs. */
#include "voice_amr_if.h"
#include "voice_dsm_if.h"


/****************************************************************************
  WVA DEFINES
****************************************************************************/

/* Defined 4500 bytes Considering max 3 session/subscriptions .
 *
 * Note: one session consistes of one wva_session_object_t and one 
 *       wva_modem_subs_object_t
 */
#define WVA_HEAP_SIZE_V ( 3000 )

/* Size of VS work packet queue. */
#define WVA_NUM_WORK_PKTS_V ( 10 )

#define WVA_HANDLE_TOTAL_BITS_V ( 16 )
#define WVA_HANDLE_INDEX_BITS_V ( 4 ) /**< 4 bits = 16 handles. */
#define WVA_MAX_OBJECTS_V ( 1 << WVA_HANDLE_INDEX_BITS_V )

#define WVA_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[0x%08x], wva_state=%d", \
                            rc, wva_is_initialized, 0 ); } }

#define WVA_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Error[0x%08x], "\
                       "wva_state=%d", rc, wva_is_initialized ); } }

#define WVA_ACQUIRE_LOCK( lock ) \
  apr_lock_enter( lock );

#define WVA_RELEASE_LOCK( lock ) \
  apr_lock_leave( lock );

/* Definitions for error checking */
#define WVA_VOCODER_ID_UNDEFINED_V (0xFFFFFFFF)
#define WVA_VSID_UNDEFINED_V (0xFFFFFFFF)
#define WVA_CODEC_MODE_UNDEFINED ( 0xFFFFFFFF )


/**
 * Internal event triggered W-RLC queues channel data to AMR DSM queues.
 */
#define WVA_INTERNAL_EVENT_DELIVER_DL_BUFFER ( 0x00013189 )

/**
 * Internal event triggered to send silence frame if UL vocoder packet
 * is not available from DSP.
 */
#define WVA_INTERNAL_EVENT_DELIVER_UL_BUFFER ( 0x000131AE )

#define WVA_AMR_DSM_Q_LEN ( 1 )

/****************************************************************************
  WVA thread states
****************************************************************************/

typedef enum wva_thread_state_enum_t
{
  WVA_THREAD_STATE_ENUM_INIT,
  WVA_THREAD_STATE_ENUM_READY,
  WVA_THREAD_STATE_ENUM_EXIT
}
  wva_thread_state_enum_t;

/***************************************************************************
  WVA WORK REQUEST ( TYPE / ITEM ) DEFINITIONS                              *
****************************************************************************/


typedef struct wva_work_item_t {

  apr_list_node_t link;
  aprv2_packet_t* packet;
} wva_work_item_t;


/****************************************************************************
   THE COMMON OBJECT DEFINITIONS
****************************************************************************/

typedef enum wva_object_type_enum_t
{
   WVA_OBJECT_TYPE_ENUM_UNINITIALIZED,
   WVA_OBJECT_TYPE_ENUM_SIMPLE_JOB,
   WVA_OBJECT_TYPE_ENUM_SEQUENCER_JOB,
   WVA_OBJECT_TYPE_ENUM_MODEM_SUBSCRIPTION,
   WVA_OBJECT_TYPE_ENUM_SESSION,
   WVA_OBJECT_TYPE_ENUM_INVALID

} wva_object_type_enum_t;

typedef struct wva_object_header_t
{
  uint32_t handle;
   /**< The handle to the associated apr_objmgr_object_t instance. */
  wva_object_type_enum_t type;
   /**<
    * The object type defines the actual derived object.
    *
    * The derived object can be any custom object type. A session or a
    * command are two such custom object types. A free object entry is set
    * to VS_OBJECT_TYPE_ENUM_FREE.
    */
} wva_object_header_t;

/****************************************************************************
  THE SIMPLE JOB OBJECT
****************************************************************************/

typedef struct wva_simple_job_object_t wva_simple_job_object_t;

struct wva_simple_job_object_t {

  wva_object_header_t header;

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

typedef struct wva_session_object_t {

  wva_object_header_t header;

  sys_modem_as_id_e_type asid;
    /**< Modem Active Subcription ID. */
  uint32_t vsid;
    /**< System level published/documented voice system ID. */
  apr_lock_t data_lock;
    /**< Lock to synchornisze data access. */

  uint32_t wcdma_handle;
    /**< Return handle for GSM voice instance. */
  bool_t is_wcdma_ready;
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

  /* Refers to logical data channel as per AMRIF1 frame format.*/
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
  uint8_t cfn_value;
    /*< Connection Frame Number - range from 0 to 255. */
  uint8_t is_dl_buffer_ready;
    /*< TBD. */

  /**<
    * Reference to VS session control.
    * Applicable if adapter is configured with asid to vsid mapping.
    */
  uint32_t vs_handle;
    /**< Return handle for vocoder session. */
  bool_t is_vs_ready;
    /**< Inidcates vocoder session is running. */
  vs_pktexg_buffer_t* vs_read_buf;
  vs_pktexg_buffer_t* vs_write_buf;

}wva_session_object_t;


/****************************************************************************
  THE GENERIC WVA OBJECT
****************************************************************************/

typedef union wva_object_t {
  
  wva_object_header_t header;
  wva_simple_job_object_t simple_job;
  wva_session_object_t session_obj;

} wva_object_t;


/****************************************************************************
 * PENDING MVS COMMAND DEFINITIONS                                          *
 ****************************************************************************/

typedef enum wva_gating_cmd_state_t
{
  WVA_GATING_CMD_STATE_FETCH,
  WVA_GATING_CMD_STATE_EXECUTE,
  WVA_GATING_CMD_STATE_CONTINUE

} wva_gating_cmd_state_t;

typedef struct wva_gating_control_t {
  
  apr_list_t cmd_q;
  /**< The gating (wva_work_item_t) queue. */
  wva_gating_cmd_state_t state;
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
   * wva_cmd_packet_t or wva_event_packet_t.
   */
  wva_object_t* rootjob_obj;
  /**<
   * The rootjob_obj is a temporary storage for the current gating
   * command.
   */
} wva_gating_control_t;

#endif  /* __WVA_I_H__ */

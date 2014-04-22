#ifndef __GVA_I_H__
#define __GVA_I_H__

/**
  @file  gva_i.h
  @brief This file contains internal definitions of GSM voice adapter.
*/

/*
  ============================================================================
  Copyright (C) 2015 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/inc/CT/private/gva_i.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
  Include files for Module
****************************************************************************/

#include "sys.h"
#include "mmdefs.h"
#include "apr_list.h"
#include "apr_lock.h"
#include "aprv2_packet.h"
#include "aprv2_api_inline.h"

/****************************************************************************
  GVA DEFINES
****************************************************************************/

/* Defined 1500 bytes Considering max 3 session/subscriptions .
 *
 * Note: one session consistes of one gva_session_object_t and one 
 *       gva_modem_subs_object_t
 */
#define GVA_HEAP_SIZE_V ( 1500 )

/* Size of VS work packet queue. */
#define GVA_NUM_WORK_PKTS_V ( 10 )

#define GVA_HANDLE_TOTAL_BITS_V ( 16 )
#define GVA_HANDLE_INDEX_BITS_V ( 4 ) /**< 4 bits = 16 handles. */
#define GVA_MAX_OBJECTS_V ( 1 << GVA_HANDLE_INDEX_BITS_V )

#define GVA_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[0x%08x], gva_state=%d", \
                            rc, gva_is_initialized, 0 ); } }

#define GVA_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Error[0x%08x], "\
                       "gva_state=%d", rc, gva_is_initialized ); } }

#define GVA_ACQUIRE_LOCK( lock ) \
  apr_lock_enter( lock );

#define GVA_RELEASE_LOCK( lock ) \
  apr_lock_leave( lock );

/* Definitions for error checking */
#define GVA_VOCODER_ID_UNDEFINED_V (0xFFFFFFFF)
#define GVA_VSID_UNDEFINED_V (0xFFFFFFFF)
#define GVA_CODEC_MODE_UNDEFINED ( 0xFFFFFFFF )


/****************************************************************************
  GVA thread states
****************************************************************************/

typedef enum gva_thread_state_enum_t
{
  GVA_THREAD_STATE_ENUM_INIT,
  GVA_THREAD_STATE_ENUM_READY,
  GVA_THREAD_STATE_ENUM_EXIT
}
  gva_thread_state_enum_t;

/***************************************************************************
  GVA WORK REQUEST ( TYPE / ITEM ) DEFINITIONS                              *
****************************************************************************/

typedef struct gva_work_item_t {

  apr_list_node_t link;
  aprv2_packet_t* packet;
   /**<
     * This is generic work packet, based on the pkt_type it can
     * to gva_cmd_packet_t or gva_event_packet_t.
     */
} gva_work_item_t;


/****************************************************************************
   THE COMMON OBJECT DEFINITIONS
****************************************************************************/

typedef enum gva_object_type_enum_t
{
   GVA_OBJECT_TYPE_ENUM_UNINITIALIZED,
   GVA_OBJECT_TYPE_ENUM_SIMPLE_JOB,
   GVA_OBJECT_TYPE_ENUM_SEQUENCER_JOB,
   GVA_OBJECT_TYPE_ENUM_MODEM_SUBSCRIPTION,
   GVA_OBJECT_TYPE_ENUM_SESSION,
   GVA_OBJECT_TYPE_ENUM_INVALID

} gva_object_type_enum_t;

typedef struct gva_object_header_t
{
  uint32_t handle;
   /**< The handle to the associated apr_objmgr_object_t instance. */
  gva_object_type_enum_t type;
   /**<
    * The object type defines the actual derived object.
    *
    * The derived object can be any custom object type. A session or a
    * command are two such custom object types. A free object entry is set
    * to VS_OBJECT_TYPE_ENUM_FREE.
    */
} gva_object_header_t;

/* forward declaration */
typedef struct gva_modem_subs_object_t gva_modem_subs_object_t;

/****************************************************************************
  THE SIMPLE JOB OBJECT
****************************************************************************/

typedef struct gva_simple_job_object_t gva_simple_job_object_t;

struct gva_simple_job_object_t {

  gva_object_header_t header;

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

typedef struct gva_session_object_t gva_session_object_t;

struct gva_session_object_t {

  gva_object_header_t header;

  uint32_t vsid;
    /**< System level published/documented Voice System ID. */

  gva_modem_subs_object_t* active_subs_obj;
    /**< 
      * Reference to the active modem subscription object that is currenlty
      * mapped to VSID.
      * Multiple subscription may map to same VSID, hence Adapter vsid session
      * needs to cache the active subscription context for sending command like
      * start/stop/sample_rate.
      */

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

  apr_lock_t data_lock;
    /**< Lock to synchornisze data access. */

  gva_icommon_event_callback_fn_t va_gva_event_cb;
    /**<
      * Callback function registered by voice agent to recieve command
      * responses and events.
      */
  void* va_session_context;
    /**< Session context provided during GVA_IRESOURCE_CMD_OPEN. */

  bool_t is_resource_granted;
    /**< Indicates that vocoder resource access is granted. */

  /**<
    * Reference to GSM voice instance.
    * Applicable if adapter is configured with asid to vsid mapping.
    */
  uint32_t vocoder_type;
    /**< Indicates the active vocoder type for the call. */

  uint32_t codec_mode;
    /**< Indicates the vocoder type requested for the call. */

  bool_t dtx_mode;
    /**< Indicates the active vocoder type for the call. */

  /**<
    * Reference to VS session control.
    * Applicable if adapter is configured with asid to vsid mapping.
    */
  uint32_t vs_handle;
    /**< Return handle for vocoder session. */

  vs_pktexg_buffer_t* vs_read_buf;

  vs_pktexg_buffer_t* vs_write_buf;

  bool_t is_vs_ready;
    /**< Inidcates vocoder session is running. */
};

/****************************************************************************
  THE MODEM SUBSCRIPTION INFO OBJECT
*****************************************************************************/

struct gva_modem_subs_object_t {

  gva_object_header_t header;

  sys_modem_as_id_e_type asid;
    /**< Modem Active Subcription ID. */

  uint32_t vsid;
    /**< System level published/documented voice system ID. */

  uint32_t pending_vsid;
    /**< System level published/documented voice system ID. */

  uint32_t gsm_handle;
    /**< Return handle for GSM voice instance. */

  bool_t is_gsm_ready;
    /**< Indicates the readiness of GSM protocol software for voice 
      * traffic exchange.
      */

  gva_session_object_t* session_obj;
    /**< Reference to gva session object having VSID as per the
      *  mapping available from voice agent. */
};

/****************************************************************************
  THE GENERIC GVA OBJECT
****************************************************************************/

typedef union gva_object_t {

  gva_object_header_t header;
  gva_simple_job_object_t simple_job;
  gva_modem_subs_object_t modem_subs_obj;
  gva_session_object_t session_obj;

} gva_object_t;


/****************************************************************************
 * PENDING MVS COMMAND DEFINITIONS                                          *
 ****************************************************************************/

typedef enum gva_gating_cmd_state_t
{
  GVA_GATING_CMD_STATE_FETCH,
  GVA_GATING_CMD_STATE_EXECUTE,
  GVA_GATING_CMD_STATE_CONTINUE

} gva_gating_cmd_state_t;

typedef struct gva_gating_control_t {
  
  apr_list_t cmd_q;
  /**< The gating (gva_work_item_t) queue. */
  gva_gating_cmd_state_t state;
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
   * gva_cmd_packet_t or gva_event_packet_t.
   */
  gva_object_t* rootjob_obj;
  /**<
   * The rootjob_obj is a temporary storage for the current gating
   * command.
   */
} gva_gating_control_t;


/****************************************************************************
 * GVA INTERNAL ROUTINES                                                    *
 ****************************************************************************/

GVA_INTERNAL uint32_t gva_prepare_and_dispatch_cmd_packet (
  void* session_context,
  uint32_t cmd_id,
  void* params,
  uint32_t size
);

GVA_INTERNAL void gva_signal_run ( 
  void
);


#endif  /* __GVA_I_H__ */

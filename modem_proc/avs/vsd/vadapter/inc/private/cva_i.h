#ifndef __CVA_I_H__
#define __CVA_I_H__

/**
  @file  cva_i.h
  @brief This file contains internal definitions of CDMA voice adapter.
*/

/*
  ============================================================================
  Copyright (C) 2015, 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/inc/private/cva_i.h#1 $
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
  CVA DEFINES
****************************************************************************/

/* Defined 1500 bytes Considering max 3 session/subscriptions .
 *
 * Note: one session consistes of one cva_session_object_t and one 
 *       cva_modem_subs_object_t
 */
#define CVA_HEAP_SIZE_V ( 1500 )

/* Max packet size for 1X vocoders. */
#define CVA_MAX_PACKET_SIZE ( 40 )

/* Size of VS work packet queue. */
#define CVA_NUM_WORK_PKTS_V ( 10 )

#define CVA_HANDLE_TOTAL_BITS_V ( 16 )
#define CVA_HANDLE_INDEX_BITS_V ( 4 ) /**< 4 bits = 16 handles. */
#define CVA_MAX_OBJECTS_V ( 1 << CVA_HANDLE_INDEX_BITS_V )

#define CVA_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[0x%08x], cva_state=%d", \
                            rc, cva_is_initialized, 0 ); } }

#define CVA_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Error[0x%08x], "\
                       "cva_state=%d", rc, cva_is_initialized ); } }

#define CVA_ACQUIRE_LOCK( lock ) \
  apr_lock_enter( lock );

#define CVA_RELEASE_LOCK( lock ) \
  apr_lock_leave( lock );

/* Definitions for error checking */
#define CVA_VOCODER_ID_UNDEFINED_V (0xFFFFFFFF)
#define CVA_VSID_UNDEFINED_V (0xFFFFFFFF)
#define CVA_MIN_MAX_RATE_UNDEFINED ( 0xFFFFFFFF )
#define CVA_AVG_ENC_RATE_UNDEFINED ( 0xFF )
#define CVA_FRAME_RATE_UNDEFINED ( 0xFF )

/****************************************************************************
  CVA thread states
****************************************************************************/

typedef enum cva_thread_state_enum_t
{
  CVA_THREAD_STATE_ENUM_INIT,
  CVA_THREAD_STATE_ENUM_READY,
  CVA_THREAD_STATE_ENUM_EXIT
}
  cva_thread_state_enum_t;

/***************************************************************************
  CVA WORK REQUEST ( TYPE / ITEM ) DEFINITIONS                              *
****************************************************************************/

typedef struct cva_work_item_t {

  apr_list_node_t link;
  void* packet;
} cva_work_item_t;

/****************************************************************************
   THE COMMON OBJECT DEFINITIONS
****************************************************************************/

typedef enum cva_object_type_enum_t
{
   CVA_OBJECT_TYPE_ENUM_UNINITIALIZED,
   CVA_OBJECT_TYPE_ENUM_SIMPLE_JOB,
   CVA_OBJECT_TYPE_ENUM_SESSION,
   CVA_OBJECT_TYPE_ENUM_INVALID

} cva_object_type_enum_t;

typedef struct cva_object_header_t
{
  uint32_t handle;
   /**< The handle to the associated apr_objmgr_object_t instance. */
  cva_object_type_enum_t type;
   /**<
    * The object type defines the actual derived object.
    *
    * The derived object can be any custom object type. A session or a
    * command are two such custom object types. A free object entry is set
    * to CVA_OBJECT_TYPE_ENUM_FREE.
    */
} cva_object_header_t;

/****************************************************************************
  THE SIMPLE JOB OBJECT
****************************************************************************/

typedef struct cva_simple_job_object_t {

  cva_object_header_t header;

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
} cva_simple_job_object_t;


/****************************************************************************
  THE SESSION OBJECT
****************************************************************************/

typedef struct cva_session_object_t {

  cva_object_header_t header;

  sys_modem_as_id_e_type asid;
    /**< Modem Active Subcription ID. */
  sys_vsid_type vsid;
    /**< System level published/documented Voice System ID. */
  apr_lock_t data_lock;
    /**< Lock to synchornisze data access. */
  uint8_t vocoder_state;
    /**< This represents possible vocoder activites/states.*/
  bool_t is_resource_granted;
    /**< Indicates that vocoder resource access is granted. */

  /**< Reference to ONEX session control. */
  uint32_t onex_handle;
    /**< Return handle for CDMA voice instance. */
  bool_t is_onex_ready;
    /**< Indicates the readiness of GSM protocol software for voice 
      * traffic exchange.
      */
  uint32_t vocoder_type;
    /**< Indicates the active vocoder type for the call. */
  uint32_t avg_enc_rate;
    /**< Indicates the enc rate requested for the call. */
  uint32_t min_rate;
    /**< Minimum frame rate. */
  uint32_t max_rate;
    /**< Maximum frame rate. */
  bool_t dtx_mode;
    /**< Indicates discontinuous transmission. */

  /**< Reference to VS session control. */
  uint32_t vs_handle;
    /**< Return handle for vocoder session. */
  bool_t is_vs_ready;
    /**< Inidcates vocoder session is running. */
  vs_pktexg_buffer_t* vs_read_buf;
  vs_pktexg_buffer_t* vs_write_buf;

}   cva_session_object_t;


/****************************************************************************
  THE GENERIC CVA OBJECT
****************************************************************************/

typedef union cva_object_t {
  
  cva_object_header_t header;
  cva_simple_job_object_t simple_job;
  cva_session_object_t session_obj;

} cva_object_t;


/****************************************************************************
 * PENDING MVS COMMAND DEFINITIONS                                          *
 ****************************************************************************/

typedef enum cva_gating_cmd_state_t
{
  CVA_GATING_CMD_STATE_FETCH,
  CVA_GATING_CMD_STATE_EXECUTE,
  CVA_GATING_CMD_STATE_CONTINUE

} cva_gating_cmd_state_t;

typedef struct cva_gating_control_t {
  
  apr_list_t cmd_q;
  /**< The gating (cva_work_item_t) queue. */
  cva_gating_cmd_state_t state;
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
   * cva_cmd_packet_t or cva_event_packet_t.
   */
  cva_object_t* rootjob_obj;
  /**<
   * The rootjob_obj is a temporary storage for the current gating
   * command.
   */
} cva_gating_control_t;

#endif  /* __CVA_I_H__ */

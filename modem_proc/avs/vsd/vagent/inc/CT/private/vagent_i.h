#ifndef __VAGENT_I_H__
#define __VAGENT_I_H__

/**
  @file  vagent_i.h
  @brief This file contains internal definitions of Voice Agent.
*/

/*
  ============================================================================

   Copyright (C) 2015-2018 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: vagent_i.h
  $Author: pwbldsvc $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------

  ============================================================================
*/

/****************************************************************************
  Include files for Module
****************************************************************************/

#include "sys.h"
#include "mmdefs.h"
#include "apr_list.h"
#include "cm.h"

/****************************************************************************
  VAGENT DEFINES
****************************************************************************/

/* Defined 600 bytes Considering max 3 session/subscriptions .
 *
 * Note: one session consistes of one vagent_session_object_t and one 
 *       vagent_modem_subs_object_t
 */
#define VAGENT_HEAP_SIZE_V ( 800 )

/* Size of VS work packet queue. */
#define VAGENT_NUM_WORK_PKTS_V ( 10 )

#define VAGENT_HANDLE_TOTAL_BITS_V ( 16 )
#define VAGENT_HANDLE_INDEX_BITS_V ( 3 ) /**< 3 bits = 8 handles. */
#define VAGENT_MAX_OBJECTS_V ( 1 << VAGENT_HANDLE_INDEX_BITS_V )

#define VAGENT_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "VAGENT: rc=(0x%08x), state=%d", \
                            rc, vagent_is_initialized, 0 ); } }

#define VAGENT_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "VAGENT: rc=(0x%08x), "\
                       "state=%d", rc, vagent_is_initialized ); } }

#define VAGENT_REPORT_FATAL_ON_NO_MEMORY( rc, size ) \
  { if ( rc ) { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "VAGENT: rc=(0x%08x), "\
                       "size requested=%d", rc, size ); } }

#define VAGENT_ACQUIRE_LOCK( lock ) \
  apr_lock_enter( lock );

#define VAGENT_RELEASE_LOCK( lock ) \
  apr_lock_leave( lock );

/* Definitions for error checking */
#define VAGENT_VSID_UNDEFINED_V (0xFFFFFFFF)
#define VAGENT_MODULE_ID_NONE (0xFFFFFFFF)


/****************************************************************************
  Definitions of events internal to voice agent
****************************************************************************/
#define VAGENT_EVENTI_CM_SUBS_INFO ( 0x0001318A )

typedef struct vagent_eventi_cm_subs_info_t vagent_eventi_cm_subs_info_t;

struct vagent_eventi_cm_subs_info_t {

  sys_modem_as_id_e_type asid;
    /**< Modem Active Subcription ID. */

  bool_t is_sub_active;

  uint32_t vsid;
};


/****************************************************************************
  Voice Agent states
*****************************************************************************/

typedef enum vagent_subs_state_enum_t
{
  VAGENT_SUBS_STATE_INACTIVE,
  VAGENT_SUBS_STATE_ACTIVE,
  VAGENT_SUBS_STATE_LCH
}
  vagent_subs_state_enum_t;


/***************************************************************************
  WORK REQUEST ITEM DEFINITIONS                                     *
****************************************************************************/

typedef struct vagent_event_packet_t {
  
  void* session_context;
    /**< Reference to voice agent session object. */

  uint32_t event_id;
    /**< Event id issued from client. */

  void* params;
    /**< Structure associated to each event_id. */

} vagent_event_packet_t;

typedef struct vagent_work_item_t {

  apr_list_node_t link;

  vagent_event_packet_t* packet;

} vagent_work_item_t;


/****************************************************************************
  OBJECT DEFINITIONS
****************************************************************************/

typedef enum vagent_object_type_enum_t
{
   VAGENT_OBJECT_TYPE_ENUM_UNINITIALIZED,
   VAGENT_OBJECT_TYPE_ENUM_SESSION,
   VAGENT_OBJECT_TYPE_ENUM_MODEM_SUBSCRIPTION,
   VAGENT_OBJECT_TYPE_ENUM_INVALID

} vagent_object_type_enum_t;

typedef struct vagent_object_header_t
{
  uint32_t handle;
   /**< The handle to the associated apr_objmgr_object_t instance. */

  vagent_object_type_enum_t type;
   /**<
    * The object type defines the actual derived object.
    *
    * The derived object can be any custom object type. A session or a
    * command are two such custom object types. A free object entry is set
    * to VS_OBJECT_TYPE_ENUM_FREE.
    */

} vagent_object_header_t;



typedef struct vagent_session_object_t vagent_session_object_t;
typedef struct vagent_modem_subs_object_t vagent_modem_subs_object_t;


/****************************************************************************
  SESSION OBJECT
*****************************************************************************/

struct vagent_session_object_t {

  vagent_object_header_t header;

  uint32_t vsid;
    /**< System level published/documented voice system ID. */

  vagent_modem_subs_object_t* modem_subs_obj;
    /**< 
     * Reference to the active modem subscription object that is currenlty
     * mapped to VSID.
     * Multiple subscription may map to same VSID, hence Adapter vsid session
     * needs to cache the active subscription context for updating active_calls_count
     * for 3rd party IMS solution.
     */

  vagent_subs_state_enum_t state;
    /**<
     * Monitor vocoder grant timing based on CM call events and call reference 
     * counts during single call, multiple calls and multi-party calls.
     */

  sys_voice_handover_e_type ho_type;
    /**< Reported handover type during HO events.. */

  bool_t is_ring_back_tone_playing;
    /**< Flag to indicate whether Ring back tone is in progress or not. */

  uint32_t current_module;
    /**< Module which is holding the resource currently. */

  uint32_t pending_module;
    /**< Module which has requested for resource grant. */

  bool_t ecall_resource_request;
   /**< Flag to indicate whether ECALL has requested for voice reosurce or not 
     *  Ecall runs conurently with either of the voice adapter. */

  uint32_t cva_handle;
   /** CDMA voice adapter handle recieved via CVA_IRESOURCE_REGISTER_CMD. */

  uint32_t gva_handle;
   /** GSM voice adapter handle recieved via GVA_IRESOURCE_REGISTER_CMD. */

  uint32_t wva_handle;
   /** WCDMA voice adapter handle recieved via WVA_IRESOURCE_REGISTER_CMD. */

  uint32_t tva_handle;
   /** TDSCDMA voice adapter handle recieved via TVA_IRESOURCE_REGISTER_CMD. */

  uint32_t iva_handle;
   /** IMS voice adapter handle recieved via IVA_IRESOURCE_REGISTER_CMD. */

  uint32_t ecall_handle;
   /** ECALL vadapter handle recieved via CVA_IRESOURCE_REGISTER_CMD. */

};


/****************************************************************************
  SUBSCRIPTION OBJECT
*****************************************************************************/

struct vagent_modem_subs_object_t {

  vagent_object_header_t header;

  sys_modem_as_id_e_type asid;
    /**< Modem Active Subcription ID. */

  uint32_t vsid;
    /**< System level published/documented voice system ID. */

  vagent_session_object_t* session_obj;
    /**< 
      * Reference to voice agent session object having VSID as per the
      * mapping available from call manager.
      *
      * If voice agent do not have a session object with mapped VSID, a new
      * session object would be instanciated with corresponding VSID.
      */
};


/****************************************************************************
  GENERIC VAGENT OBJECT
****************************************************************************/

typedef union vagent_object_t {

  vagent_object_header_t header;
  vagent_modem_subs_object_t modem_subs_obj;
  vagent_session_object_t session_obj;

} vagent_object_t;


/****************************************************************************
  FUNCTION PROTOTYPES
*****************************************************************************/

VAGENT_INTERNAL void vagent_int_lock_fn ( 
  void
);

VAGENT_INTERNAL void vagent_int_unlock_fn (
  void
);

VAGENT_INTERNAL uint32_t vagent_free_event_packet (
  vagent_event_packet_t* cmd_packet
);

VAGENT_INTERNAL uint32_t vagent_prepare_and_dispatch_event_packet (
 void* session_context,
 uint32_t event_id,
 void* params,
 uint32_t size
);

VAGENT_INTERNAL uint32_t vagent_mem_alloc_object (
  uint32_t size,
  vagent_object_t** ret_object
);

VAGENT_INTERNAL uint32_t vagent_mem_free_object (
  vagent_object_t* object
);

VAGENT_INTERNAL uint32_t vagent_create_modem_subs_object ( 
  vagent_modem_subs_object_t** ret_subs_obj
);

VAGENT_INTERNAL uint32_t vagent_create_session_object ( 
  vagent_session_object_t** ret_session_obj
);

VAGENT_INTERNAL void vagent_log_voice_adapter_event_info(
  void* session_context,
  uint32_t event_id
);


VAGENT_INTERNAL void vagent_cm_call_event_cb (
  cm_call_event_e_type call_event,
  const cm_mm_call_info_s_type *call_info_ptr
);

VAGENT_INTERNAL void vagent_cm_subs_info_cb (
  cm_ph_event_e_type event,
  const cm_subs_info_s_type* params 
);

VAGENT_INTERNAL uint32_t vagent_voice_adapter_event_cb (
 void* session_context,
 uint32_t event_id,
 void* params,
 uint32_t size
);


VAGENT_INTERNAL void vagent_broadcast_subscription_info (
  vagent_modem_subs_object_t* subs_obj
);

VAGENT_INTERNAL uint32_t vagent_resource_register (
  vagent_session_object_t* session_obj
);

VAGENT_INTERNAL uint32_t vagent_resource_deregister (
  vagent_session_object_t* session_obj
);

VAGENT_INTERNAL uint32_t vagent_resource_grant (
  vagent_session_object_t* session_obj,
  uint32_t module_id
);

VAGENT_INTERNAL uint32_t vagent_resource_revoke (
  vagent_session_object_t* session_obj
);


VAGENT_INTERNAL uint32_t vagent_process_cm_call_event ( 
  vagent_event_packet_t* packet
);

VAGENT_INTERNAL uint32_t vagent_process_cm_subs_info_event ( 
  vagent_event_packet_t* packet
);

VAGENT_INTERNAL uint32_t vagent_process_resource_request_event ( 
  vagent_event_packet_t* packet,
  uint32_t module_id
);

VAGENT_INTERNAL uint32_t vagent_process_ecall_resource_request_event ( 
  vagent_event_packet_t* packet,
  uint32_t module_id
);

VAGENT_INTERNAL uint32_t vagent_process_resource_released_event ( 
  vagent_event_packet_t* packet,
  uint32_t module_id
);

VAGENT_INTERNAL uint32_t vagent_process_ecall_resource_released_event ( 
  vagent_event_packet_t* packet,
  uint32_t module_id
);

VAGENT_INTERNAL void vagent_process_event_packets (
  void
);


VAGENT_INTERNAL void vagent_signal_run ( 
  void
);

VAGENT_INTERNAL int32_t vagent_run (
  void
);

VAGENT_INTERNAL int32_t vagent_worker_thread_fn (
  void* param
);

VAGENT_INTERNAL int32_t vagent_init (
  void 
);

VAGENT_INTERNAL int32_t vagent_postinit ( 
  void
);

VAGENT_INTERNAL int32_t vagent_predeinit (
  void
);

VAGENT_INTERNAL int32_t vagent_deinit (
  void 
);

#endif  /* __VAGENT_I_H__ */

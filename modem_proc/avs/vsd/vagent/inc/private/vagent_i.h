#ifndef __VAGENT_I_H__
#define __VAGENT_I_H__

/**
  @file  vagent_i.h
  @brief This file contains internal definitions of Voice Agent.
*/

/*
  ============================================================================
  Copyright (C) 2015-2019 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vagent/inc/private/vagent_i.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
  Include files for Module
****************************************************************************/

#include "sys.h"
#include "cm.h"
#include "mmdefs.h"

#include "apr_list.h"
#include "apr_lock.h"
#include "aprv2_packet.h"
#include "aprv2_api_inline.h"

/****************************************************************************
  VAGENT DEFINES
****************************************************************************/
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


#define VAGENT_EVENTI_CM_CALL_EVENT ( 0x000131F9 )

typedef struct vagent_eventi_cm_call_event_t vagent_eventi_cm_call_event_t;

struct vagent_eventi_cm_call_event_t
{
  sys_modem_as_id_e_type asid;
    /**< modem active subcription ID. */

  cm_call_event_e_type event_id;
    /**< call events as reported by CM. */

  cm_connected_call_status_e_type call_state;
    /**< Call state as reported by CM. */

  sys_voice_handover_e_type ho_type;
    /**< Reported handover type during HO events.. */

  cm_srvcc_alert_media_type alert_media_type;
    /**< Reported alert media type during SRVCC. */
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

typedef struct vagent_work_item_t {

  apr_list_node_t link;
  aprv2_packet_t* packet;
   /**<
     * This is generic work packet, based on the pkt_type it can
     * to gva_cmd_packet_t or gva_event_packet_t.
     */
} vagent_work_item_t;


/****************************************************************************
  OBJECT DEFINITIONS
****************************************************************************/

typedef enum vagent_object_type_enum_t
{
   VAGENT_OBJECT_TYPE_ENUM_UNINITIALIZED,
   VAGENT_OBJECT_TYPE_ENUM_SESSION,
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


/****************************************************************************
  SESSION OBJECT
*****************************************************************************/

typedef struct vagent_session_object_t {

  vagent_object_header_t header;

  sys_modem_as_id_e_type asid;
    /**< Modem Active Subcription ID. */

  uint32_t vsid;
    /**< System level published/documented voice system ID. */

  vagent_subs_state_enum_t state;
    /**<
     * Monitor vocoder grant timing based on CM call events and call reference 
     * counts during single call, multiple calls and multi-party calls.
     */

  sys_voice_handover_e_type ho_type;
    /**< Reported handover type during HO events.. */

  bool_t is_ring_back_tone_playing;
    /**< Flag to indicate whether Ring back tone is in progress or not. */

  vagent_voice_client_type_t current_module;
    /**< Module which is holding the resource currently. */

  vagent_voice_client_type_t pending_module;
    /**< Module which has requested for resource grant. */

  bool_t ecall_resource_request;
   /**< Flag to indicate whether ECALL has requested for voice reosurce or not 
     *  Ecall runs conurently with either of the voice adapter. */
}
  vagent_session_object_t;

#endif  /* __VAGENT_I_H__ */

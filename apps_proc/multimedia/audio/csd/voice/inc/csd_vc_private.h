#ifndef __CSD_VC_PRIVATE_H__
#define __CSD_VC_PRIVATE_H__

/*-----------------------------------------------------------------------------
     Copyright (c) 2009 - 2011, 2016 Qualcomm Technologies Incorporated.
  All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.

	$Header: //components/rel/audio.tx/2.0/audio/csd/voice/inc/csd_vc_private.h#1 $
	$DateTime: 2018/12/25 22:39:07 $
	$Author: pwbldsvc $
	$Change: 17929171 $
	$Revision: #1 $
	=========================================================================*/
 /*=============================================================================
                     
  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.
  when          who       what, where, why
  --------     -----     ---------------------------------------------------
  01/06/15      rk       Moved tx dtmf detection event QMI CB processing to voc_comman_task 
  08/25/14      jk       added per vocoder calibration changes  
  05/30/14      rp       added topology feature id  
  04/16/14      jk       added support for Voice Session Manager

=============================================================================*/

#include "mmdefs.h"
#include <string.h> /* memcpy */

#include "apr_objmgr.h"
#include "aprv2_api_inline.h"
#include "aprv2_msg_if.h"
#include "apr_log.h"

#include "csd_intf.h"
#include "csd_handle.h"
#include "csd_status.h"
#include "csd_voc_common.h"
#include "csd_vc_private.h"
#include "csd_vc.h"
#include "csd_vc_ioctl.h"
#include "csd_debug_msg.h"
#include "csd_diag_service_msg.h"
#include "vss_ivp3_public_if.h"
#include "vss_public_if.h"
#include "csd_vsm.h"

#include "acdb.h"
#include "csd_os_dependencies.h"

#include "csd_voice_debug_ioctl.h"

/*****************************************************************************
 * Voice Context Static API Prototypes                                       *
 ****************************************************************************/

CSD_INTERNAL int32_t csd_vc_init (
  struct csd_vtable** ret_vtable
);

CSD_INTERNAL int32_t csd_vc_deinit ( void );

CSD_INTERNAL int32_t csd_vc_open (
  uint32_t open_id,
  void* params,
  uint32_t size
);

CSD_INTERNAL int32_t csd_vc_close (
  uint32_t handle
);

CSD_INTERNAL int32_t csd_vc_write (
  uint32_t handle,
  void* params,
  uint32_t size
);

CSD_INTERNAL int32_t csd_vc_read (
  uint32_t handle,
  void* params,
  uint32_t size
);

CSD_INTERNAL int32_t csd_vc_ioctl (
  uint32_t handle,
  uint32_t code,
  void* params,
  uint32_t size
);

/*****************************************************************************
 * Voice Context Private Stuctures                                           *
 ****************************************************************************/

/*
 ** Start Internal Data & Structures
*/

#define CSD_VC_STREAM_NAME_MAX_SIZE 31
#define VP3_MAXIMUM_BUFFER_SIZE 1024

typedef enum {
  CSD_VC_CMD_SEND_CALIBRATION,
  CSD_VC_CMD_SEND_VP3,
  CSD_VC_CMD_RECEIVE_VP3,
  CSD_VC_CMD_SEND_RSP_CALLBACK,
  CSD_VC_CMD_SEND_STATE_NOTIFY_CALLBACK,
  CSD_VC_CMD_CLEAN_CALIBRATION_RESOURCES,
  CSD_VC_CMD_SEND_IVPCM_EVT_NOTIFY,
  CSD_VC_CMD_SEND_MAP_MEM_RSP,
  CSD_VC_CMD_SET_CAL_FID,
  CSD_VC_CMD_UPDATE_STATE,
  CSD_VC_CMD_SEND_TX_DTMF_DETECTED,
  CSD_VC_CMD_MAX
} csd_vc_command_enum_t;

typedef enum {
  CSD_VC_ENUM_CAL_UNINITIALIZED,
  CSD_VC_ENUM_CAL_MEM_ALLOCATED,
  CSD_VC_ENUM_CAL_MEM_MAPPED,  
  CSD_VC_ENUM_CAL_REGISTERED
} csd_vc_cal_state_enum_t;

typedef enum {
  CSD_VC_ENUM_VP3_UNINITIALIZED,
  CSD_VC_ENUM_VP3_MEM_ALLOCATED,
  CSD_VC_ENUM_VP3_MEM_MAPPED,
  CSD_VC_ENUM_VP3_INFO_RECEIVED,
  CSD_VC_ENUM_VP3_DATA_RECEIVED,
} csd_vc_vp3_state_enum_t;

typedef enum {
  CSD_VC_ENUM_FID_UNINIITALIZED,
  CSD_VC_ENUM_FID_SET_INITIALIZE,
  CSD_VC_ENUM_FID_VC_DISABLED,
  CSD_VC_ENUM_FID_VOL_DEREGISTERED,
  CSD_VC_ENUM_FID_VOL_REGISTERED,
} csd_vc_feature_id_state_enum_t;

typedef enum {
  CSD_VC_ENUM_CLIENT_VP3,
  CSD_VC_ENUM_CLIENT_CAL,
  CSD_VC_ENUM_CLIENT_EXTERNAL
} csd_vc_memmap_client_enum_t;

typedef struct {
  uint32_t                  size; /* This size is the adjusted 4k aligned size, NOT the origin cal data size */
  uint8_t*                  p_vir_addr;
  int64_t                   phy_addr;
  bool_t                    if_registered;
} csd_vc_cal_table_t;

typedef struct {
  uint32_t                  descriptor_mapped_handle; /* CVD handle for descriptor table */
  void*                     descriptor_memory_handle; /* Internal memory handle for descriptor table */
  void*                     data_memory_handle; /* Internal memory handle for cal data */  
  uint8_t*                  pva_descriptor_table;
  uint32_t                  num_of_memory_block;
  csd_vc_cal_table_t        static_table;
  csd_vc_cal_table_t        dynamic_table;
  csd_vc_cal_table_t        devcfg_table;
  csd_vc_cal_state_enum_t   state;  
}csd_vc_session_cal;

typedef struct {
  uint32_t                    descriptor_mapped_handle; /* CVD handle for descriptor table */
  void*                       descriptor_memory_handle; /* Internal memory handle for descriptor table */
  void*                       data_memory_handle; /* Internal memory handle for cal data */  
  uint8_t*                    pva_descriptor_table;
  uint8_t*                    pva_data;
  int64_t                     phy_addr_data;
  uint32_t                    size_data;
  csd_vc_vp3_state_enum_t     state;
}csd_vc_session_vp3;

typedef struct {
  uint32_t                        volume_feature_id;
  uint32_t                        volume_cal_size_used;
  csd_vc_feature_id_state_enum_t  set_feature_id_state;
  uint32_t                        topology_feature_id;
}csd_vc_session_fid;

typedef struct {
  bool_t                        vc_configured;
  bool_t                        vc_enabled;
  csd_vsm_handle_state_t        state;
}csd_vc_session_state_t;

typedef uint32_t csd_vc_update_state_pkt_t;


typedef struct {
  apr_list_node_t                 link;
  uint32_t                        h_csd;
  uint16_t                        h_apr_obj;
  uint16_t                        h_cvp;           /* CVP handle */
  csd_vc_event_callback_fn_t      csd_vc_cb;
  void*                           csd_vc_cb_data;
  uint8_t                         direction;        /* TX, RX or both */
  uint32_t                        rx_dev_id;
  uint32_t                        tx_dev_id;
  uint32_t                        rx_dev_sr;
  uint32_t                        tx_dev_sr;
  uint32_t                        ec_ref_id;
  uint16_t                        rx_port_id;
  uint16_t                        tx_port_id;
  uint16_t                        ec_port_id;
  uint32_t                        rx_topology_id;
  uint32_t                        tx_topology_id;
  char_t                          vc_name[CSD_VC_STREAM_NAME_MAX_SIZE];
  uint32_t                        vc_name_size;
  uint32_t                        set_dev_token; /* stores set_device client token for session until calibration is completed */  
  bool_t                          bclosing_sequence; /* used for the closing sequence */
  csd_vc_memmap_client_enum_t     memmap_client;
  csd_event_t                     csd_vc_event;
  csd_vc_session_cal              sess_cal;
  csd_vc_session_vp3              sess_vp3;
  csd_vc_session_fid              sess_fid;
  csd_vc_session_state_t          sess_state;
} csd_vc_session_item_t;

typedef union {
  csd_vc_ioctl_get_ui_property_t        getui_cmd;
  csd_vc_event_state_notify_t           state_cmd;
  csd_vc_event_command_rsp_t            rsp_cmd;
  csd_vc_ivpcm_evt_notify_v2_t          notify_cmd;
  csd_vc_rsp_map_t                      rspmap_cmd;
  csd_vc_update_state_pkt_t             update_state_cmd;
  csd_vc_event_tx_dtmf_detected_t       *dtmf_tx_detected;
} csd_vc_command_packet_t;

typedef struct {
  apr_list_node_t             link;
  csd_vc_session_item_t       *psess;
  csd_vc_command_enum_t       type;
  csd_vc_command_packet_t     cmd;
  uint32_t                    client_token; /* this is the token provided by the client*/
  uint32_t                    cmd_token;    /* this is the token generated by csd_vc */
  bool_t                      awaiting_rsp; /* only used for blocking calls */
  uint32_t                    rsp;          /* keep track of the rsp from synchrous calls */
} csd_vc_cmd_list_item_t;


#define CSD_VC_NUM_CMDS     40

#define CSD_VC_NUM_SESSIONS     10

/* Object Management */

#define CSD_VC_HANDLE_TOTAL_BITS_V ( 16 )
#define CSD_VC_HANDLE_INDEX_BITS_V ( 5 ) /* 5 bits = 32 handles. */
#define CSD_VC_MAX_OBJECTS_V ( 1 << CSD_VC_HANDLE_INDEX_BITS_V )

/* CSD VC internal IOCTLs */

/**
 * CSD_VC_IOCTL_GET_DEVICE_IDS [synchronous]
 *
 * Retrieves the device ids for a given csd-vc handle
 *
 * Return call:
 *   CSD_EOK on success.
 */

#define CSD_VC_IOCTL_GET_DEVICE_IDS ( 0x000111C4 )

typedef struct csd_vc_ioctl_get_device_ids_t csd_vc_ioctl_get_device_ids_t;
struct csd_vc_ioctl_get_device_ids_t
{
  uint32_t csd_vc_handle;
    /**< The csd voice context handle. */
  uint32_t* ret_rx_dev_id;
    /**< Rx device ID. */
  uint32_t* ret_tx_dev_id;
    /**< Tx device ID. */
};

/**
  Retrieves the internal CVP handle given a CSD_VC handle.

  @return
  CSD_EOK -- Success.\n
  Error code on failure.

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_GET_CVP_HANDLE ( 0x00011168 )

typedef struct csd_vc_ioctl_get_cvp_handle_t csd_vc_ioctl_get_cvp_handle_t;
struct csd_vc_ioctl_get_cvp_handle_t
{
  uint16_t* ret_cvp_handle;
};

/**
  Retrieves and store the VP3 data for a given CSD_VC handle.

  @return
  CSD_EOK -- Success.\n
  Error code on failure.

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_STORE_VP3_DATA ( 0x00012F01 )

typedef struct csd_vc_ioctl_store_vp3_data_t csd_vc_ioctl_store_vp3_data_t;
struct csd_vc_ioctl_store_vp3_data_t
{
  uint32_t cmd_token;
};

/**
  Retrieves the AFE Tx, Rx and EC ref ports given a CSD_VC handle.

  @return
  CSD_EOK -- Success.\n
  Error code on failure.

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_GET_AFE_PORT ( 0x00013052 )

typedef struct csd_vc_ioctl_get_afe_port_t csd_vc_ioctl_get_afe_port_t;
struct csd_vc_ioctl_get_afe_port_t
{
  uint16_t* ret_rx_afe_port;
  uint16_t* ret_tx_afe_port;
  uint16_t* ret_ec_afe_port;
};

#endif /* __CSD_VC_PRIVATE_H__ */


#ifndef __CSD_VS_PRIVATE_H__
#define __CSD_VS_PRIVATE_H__

/*-----------------------------------------------------------------------------
     Copyright (c) 2014, 2016 Qualcomm Technologies Incorporated.
  All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.

	$Header: //components/rel/audio.tx/2.0/audio/csd/voice/inc/csd_vs_private.h#2 $
	$DateTime: 2019/01/03 07:23:13 $
	$Author: pwbldsvc $
	$Change: 17964421 $
	$Revision: #2 $
	=========================================================================*/
 /*=============================================================================
                      
  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.
  when          who       what, where, why
  --------     -----     ---------------------------------------------------
  10/02/14      jk       dtmf end and generation fix
  04/16/14      jk       added support for Voice Session Manager

=============================================================================*/

#include <string.h> /* memcpy */

#include "apr_list.h"
#include "apr_log.h"
#include "apr_objmgr.h"
#include "aprv2_api_inline.h"

#include "csd_intf.h"
#include "csd_status.h"
#include "csd_handle.h"
#include "csd_debug_msg.h"
#include "csd_diag_service_msg.h"
#include "csd_vsm.h"

#include "csd_voc_common.h"
#include "csd_vs.h"
#include "csd_vs_ioctl.h"
#include "csd_vc_private.h"

#include "aprv2_msg_if.h"
#include "vss_public_if.h"
#include "acdb.h"

#include "csd_voice_debug_ioctl.h"

/*****************************************************************************
 * Voice Stream Static API Prototypes                                        *
 ****************************************************************************/

CSD_INTERNAL int32_t csd_vs_init (
  struct csd_vtable** ret_vtable
);

CSD_INTERNAL int32_t csd_vs_deinit ( void );

CSD_INTERNAL int32_t csd_vs_open (
  uint32_t open_id,
  void* params,
  uint32_t size
);

CSD_INTERNAL int32_t csd_vs_close (
  uint32_t handle
);

CSD_INTERNAL int32_t csd_vs_write (
  uint32_t handle,
  void* params,
  uint32_t size
);

CSD_INTERNAL int32_t csd_vs_read (
  uint32_t handle,
  void* params,
  uint32_t size
);

CSD_INTERNAL int32_t csd_vs_ioctl (
  uint32_t handle,
  uint32_t code,
  void* params,
  uint32_t size
);

CSD_INTERNAL int32_t csd_vs_passive_session_updater (
  uint32_t  type,
  uint32_t* h_csd_vs,
  char *session_name,
  uint32_t session_name_len
);
/*****************************************************************************
 * Voice Stream Private Stuctures                                           *
 ****************************************************************************/

/*
 ** Start Internal Data & Structures
*/
#define CSD_VS_STREAM_NAME_MAX_SIZE 31
#define CSD_VS_GET_SESSION_HANDLE    1
#define CSD_VS_RESET_SESSION_HANDLE  2
#define CSD_VS_UPDATE_SESSION_HANDLE 3
#define CSD_VS_IS_PASSIVE_SESSION    4


#define CSD_VS_MAX_VOCODER_PKT_SIZE 1024

enum csd_vs_command_enum_t
{
  CSD_VS_CMD_SEND_RSP_CALLBACK,
  CSD_VS_CMD_SEND_STATE_NOTIFY_CALLBACK,
  CSD_VS_CMD_SEND_CALIBRATION,
  CSD_VS_CMD_SEND_DEC_BUF_REQ_CALLBACK,
  CSD_VS_CMD_SEND_ENC_BUF_READY_CALLBACK,
  CSD_VS_CMD_UPDATE_STATE,
  CSD_VS_CMD_SEND_DTMF_ENDED,
  CSD_VS_CMD_SEND_DTMF_DETECTED,
  CSD_VS_COMMAND_MAX
};
typedef enum csd_vs_command_enum_t csd_vs_command_enum_t;

typedef struct csd_vs_session_item_t csd_vs_session_item_t;
typedef struct {
  uint8_t* pVABuf;
  uint8_t* pVABufEnc;
  uint8_t* pVABufDec;
  uint8_t* pVAMemoryTable;
  uint32_t mapped_pmem_handle;
  uint32_t  buf_size;
  uint64_t  pa_buf;
  void* pBufHandle;
  void* pTableMemHandle;
} csd_vs_sess_voip_mem_cfg;

typedef struct {
  bool_t                        vs_media_type_set;
  bool_t                        vs_rate_set;
  csd_vsm_handle_state_t        state;
}csd_vs_session_state_t;

typedef uint32_t csd_vs_update_state_cmd_t;

struct csd_vs_session_item_t{
  apr_list_node_t             link;
  uint16_t                    h_apr_obj;          /* Handle provide to APR */
  uint32_t                    h_csd;              /* Handle provided to CSD Client */
  uint32_t                    h_cvs;              /* Handle to CVS*/
  csd_vs_event_callback_fn_t  csd_vs_cb;          /* Callback provided by CSD Client */
  void*                       csd_vs_cb_data;     /* Callback data provided by CSD client */
  char_t                      vs_name [CSD_VS_STREAM_NAME_MAX_SIZE]; /* Name of CVS Stream */
  uint32_t                    vs_name_size;       /* Size of CVS Stream name */
  csd_event_t                 csd_vs_event;       /* Event use to keep track of synchronous events */
  bool_t                      b_calibration_done;
  bool_t                      b_voip_mem_map;      /* Boolean used to track VoIP buf mem map */
  uint32_t                    network_id_index;    /* Used for Calibration */
  uint32_t                    sampling_rate_index; /* Used for Calibration */
  uint8_t                     stream_pkt[CSD_VS_MAX_VOCODER_PKT_SIZE]; /* used to buffer vocoder packet sent by CVS*/
  uint32_t                    stream_pkt_size;
  uint32_t                    enc_type;            /* used to track the encoder media ID used */
  uint32_t                    dec_type;            /* used to track the decoder media ID used */
  uint32_t                    set_dev_token;  /* stores set_device client token for session until calibration is completed */
  csd_vs_sess_voip_mem_cfg    voip_pkt_cfg;        /* used to store VoIP out-of-band packet addresses */
  csd_vs_session_state_t      sess_state;  /* stores current session state */
};

typedef union {
  csd_vs_ioctl_get_ui_property_t    getui_cmd;
  csd_vs_event_state_notify_t       state_cmd;
  csd_vs_event_command_rsp_t        rsp_cmd;
  csd_vs_update_state_cmd_t         update_state_cmd;
  csd_vs_event_dtmf_generation_ended_t *dtmf_end;
  csd_vs_event_rx_dtmf_detected_t      *dtmf_detected;
} csd_vs_command_packet_type;

typedef struct {
  apr_list_node_t                 link;
  csd_vs_session_item_t           *psess;
  csd_vs_command_enum_t           type;
  csd_vs_command_packet_type      cmd;
  uint32_t                        client_token; /* this is the token provided by the client*/
  uint32_t                        cmd_token;    /* this is the token generated by csd_vs */
} csd_vs_cmd_list_item_t;

/* data types used for storing vs handles*/
typedef struct
{
    char_t session_name[CSD_VS_STREAM_NAME_MAX_SIZE];
    uint32_t vs_passive_sess_handle;

} vs_session_t;

#define CSD_VS_NUM_COMMANDS     40    //%%%TBD

#define CSD_VS_NUM_SESSIONS     6

#define CSD_VS_NUM_VOCPROC_HANDLES 10
#define CSD_VS_NUM_TOTAL_VOCPROC_HANDLES ( CSD_VS_NUM_VOCPROC_HANDLES * CSD_VS_NUM_SESSIONS )

#define CSD_VS_HANDLE_TOTAL_BITS_V ( 16 )
#define CSD_VS_HANDLE_INDEX_BITS_V ( 5 ) /* 5 bits = 32 handles. */
#define CSD_VS_MAX_OBJECTS_V ( 1 << CSD_VS_HANDLE_INDEX_BITS_V )


/* CSD VS internal IOCTLs */

/**
 * CSD_VS_IOCTL_GET_STREAM_LIST [synchronous]
 *
 * Gets the voice stream sessions which are currently running.
 *
 * Return call:
 *   CSD_EOK on success.
 */

#define CSD_VS_IOCTL_GET_STREAM_LIST ( 0x000111C1 )
 
typedef struct csd_vs_ioctl_get_stream_list_t csd_vs_ioctl_get_stream_list_t;
struct csd_vs_ioctl_get_stream_list_t
{
  uint32_t* buffer;
    /**<
      The buffer to be filled. 
      @verbatim
      <----- 32 bits ----->
       -------------------
      | csd vs handle     |   
       -------------------
      | csd vs handle     |   
       -------------------    
      |                   |   
      .                   .
      .                   . 
      @endverbatim
     */
  uint32_t buffer_size;
    /**< Size of the buffer in bytes. */
  uint32_t* ret_write_size;
    /**< Number of bytes written. */
  uint32_t* ret_total_size;
    /**< Number of bytes required. */
};

/**
  Gets the internal CVS handle from the CSD_VS handle.

  @return
  CSD_EOK -- Success.\n
  Error code on failure.

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_GET_CVS_HANDLE ( 0x000111C2 )

typedef struct csd_vs_ioctl_get_cvs_handle_t csd_vs_ioctl_get_cvs_handle_t;
struct csd_vs_ioctl_get_cvs_handle_t
{
  uint16_t* ret_cvs_handle;
};

struct csd_vs_ioctl_map_physical_t
{
	uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/

	vss_imemory_cmd_map_physical_t vss_map_physical;
};

typedef struct csd_vs_voip_buffer_t csd_vs_voip_buffer_t;
struct csd_vs_voip_buffer_t
{
	uint32_t timestamp;
	uint32_t media_type;
	uint32_t data_size;
};

/*
 ** End Internal Data Structures
*/


#endif /* __CSD_VS_PRIVATE_H__ */


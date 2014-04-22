#ifndef __CSDLITE_PRIVATE_H__
#define __CSDLITE_PRIVATE_H__

/*-----------------------------------------------------------------------------
     Copyright (c) 2009 - 2015 Qualcomm Technologies Incorporated.
  All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.

	$Header: //components/rel/audio.tx/2.0/audio/csdlite/inc/csdlite_private.h#1 $
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
  04/20/15      jk       creation

=============================================================================*/

#include "mmdefs.h"
#include "apr_list.h"

/*----------------------------------------------------------------------------- 
  private struct defs
-----------------------------------------------------------------------------*/

#define CSDLITE_INVALID_HANDLE ( 0 )

typedef struct csdlite_dev_sess_t
{
    csdlite_device_t rx_dev;
    csdlite_device_t tx_dev;
    csdlite_device_t ec_ref_dev;
    uint32_t dev_handle;
    bool_t device_enabled; 
}csdlite_dev_sess_t;

typedef enum{
  CSDLITE_VOICE_IDLE,
  CSDLITE_VOICE_CONFIG,
  CSDLITE_VOICE_RUNNING
}csdlite_voice_sess_state_t;

typedef enum{
  CSDLITE_VC_IDLE,
  CSDLITE_VC_ATTACHED,
  CSDLITE_VC_ENABLED
}csdlite_vc_state_t;

#define CSDLITE_SESSION_ID_MAX_LEN ( 31 )

typedef struct csdlite_voice_sess_t
{
    char_t session_id[CSDLITE_SESSION_ID_MAX_LEN + 1];
    uint32_t session_id_size;
    uint32_t vm_handle;
    uint32_t vs_handle;
    uint32_t vc_handle;
    csdlite_vc_state_t vc_state;
    csdlite_voice_sess_state_t state;
    uint32_t cb_response;
}csdlite_voice_sess_t;

typedef struct csdlite_dtmf_sess_t
{
  csdlite_callback_fn_t detect_cb_func;
  csdlite_callback_fn_t gen_cb_func;
}csdlite_dtmf_sess_t;

typedef enum{
  CSDLITE_AFE_LOOPBACK_IDLE,
  CSDLITE_AFE_LOOPBACK_RUNNING
}csdlite_afe_loopback_state_t;

typedef struct csdlite_volume_cache_t
{
  uint16_t step;
  uint16_t ramp_duration;
  bool_t applied;
}csdlite_volume_cache_t;

typedef struct csdlite_hdvoice_cache_t
{
  uint32_t hd_voice;
  bool_t applied;
}csdlite_hdvoice_cache_t;

typedef struct csdlite_tty_cache_t
{
  uint32_t tty_mode;
  bool_t applied;
}csdlite_tty_cache_t;

typedef struct csdlite_feature_cache_t
{
    csdlite_hdvoice_cache_t hd_voice;
    csdlite_tty_cache_t tty;
    csdlite_volume_cache_t volume;
    
}csdlite_feature_cache_t;

typedef enum{
  CSDLITE_VC_ENABLE,
  CSDLITE_VC_DISABLE,
  CSDLITE_VC_CONFIG,
  CSDLITE_VC_VOLUME,
  CSDLITE_VC_DTMF_DETECT,
  CSDLITE_VC_MUTE
}csdlite_vc_ioctl_token_t;

typedef enum{
  CSDLITE_VM_ATTACH,
  CSDLITE_VM_DETACH,
  CSDLITE_VM_START,
  CSDLITE_VM_STOP,
  CSDLITE_VM_HDVOICE,
  CSDLITE_VM_TTY_MODE
}csdlite_vm_ioctl_token_t;

typedef enum{
  CSDLITE_CMD_DISABLE_DEV
}csdlite_cmd_t;

#define CSDLITE_NUM_CMD ( 5 )

typedef struct csdlite_cmd_node_t{
  apr_list_node_t   link;
  csdlite_cmd_t     cmd;
}csdlite_cmd_node_t;

typedef enum{
  CSDLITE_VS_DTMF_GEN,
  CSDLITE_VS_DTMF_DETECT,
  CSDLITE_VS_MUTE,

}csdlite_vs_ioctl_token_t;

/*helper functions*/
uint32_t csdlite_cleanup_loopback( void );
uint32_t csdlite_disable_device( void );
uint32_t csdlite_vc_config( csdlite_device_info_t dev_info );
uint32_t csdlite_enable_device( csdlite_device_info_t new_dev );
uint32_t csdlite_voice_reconfig( csdlite_voice_config_t* params );
uint32_t csdlite_apply_volume( void );
uint32_t csdlite_apply_hdvoice( void );
uint32_t csdlite_apply_tty( void );
uint32_t csdlite_apply_cache( void );
uint32_t csdlite_voice_teardown( void );


uint32_t csdlite_vc_callback_fn(uint32_t cmd_token,uint32_t event_id, 
                                void* param, uint32_t param_size,
                                void* client_data);

uint32_t csdlite_vs_callback_fn(uint32_t cmd_token,uint32_t event_id, 
                                void* param, uint32_t param_size,
                                void* client_data);

uint32_t csdlite_vm_callback_fn(uint32_t cmd_token,uint32_t event_id, 
                                void* param, uint32_t param_size,
                                void* client_data);

void csdlite_dtmf_gen_timer_cb( void* client_token );

#endif /*__CSDLITE_H__*/

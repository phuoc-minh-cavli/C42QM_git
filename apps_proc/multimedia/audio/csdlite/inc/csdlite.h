#ifndef __CSDLITE_H__
#define __CSDLITE_H__

/*-----------------------------------------------------------------------------
     Copyright (c) 2009 - 2015 Qualcomm Technologies Incorporated.
  All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.

	$Header: //components/rel/audio.tx/2.0/audio/csdlite/inc/csdlite.h#1 $
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
#include "csd_vs.h"
#include "csd_vc.h"

/*----------------------------------------------------------------------------- 
  public structures 
-----------------------------------------------------------------------------*/

#define CSDLITE_INVALID_DEV ( 0xFFFFFFFF )

#define CSDLITE_RX_DTMF_DETECTED ( CSD_VS_EVENTID_RX_DTMF_DETECTED )
#define CSDLITE_TX_DTMF_DETECTED ( CSD_VC_EVENTID_TX_DTMF_DETECTED )
#define CSDLITE_DTMF_GENERATION_ENDED ( CSD_VS_EVENTID_DTMF_GENERATION_ENDED )
/** Major version of the CSDLITE. */
#define CSDLITE_VER_MAIN  0x02
/** Minor version of the CSDLITE. */
#define CSDLITE_VER_MINOR 0x00
/** Minor revision of the CSDLITE. */
#define CSDLITE_VER_REV   0x00

typedef struct csdlite_device_t
{
  uint32_t dev_id;
  uint32_t sample_rate;
  uint32_t bits_per_sample;
}csdlite_device_t;

typedef struct csdlite_device_info_t
{
  csdlite_device_t tx_dev;
  csdlite_device_t rx_dev;
  csdlite_device_t ec_ref_dev;
} csdlite_device_info_t;

typedef struct csdlite_voice_config_t
{
  uint32_t cmd_token;
  csdlite_device_info_t dev_info;
  const char_t* session_id;

}csdlite_voice_config_t;


typedef struct csdlite_voice_end_t
{
  uint32_t cmd_token;

}csdlite_voice_end_t;

typedef struct csdlite_dev_switch_t
{
  uint32_t cmd_token;
  csdlite_device_info_t dev_info;

}csdlite_dev_switch_t;

typedef struct csdlite_afe_loopback_t
{
  uint32_t cmd_token;
  csdlite_device_info_t dev_info;
  bool_t enable;
  uint16_t afe_mode;

}csdlite_afe_loopback_t;

typedef enum{
   CSDLITE_DIR_TX,
   CSDLITE_DIR_RX,
   CSDLITE_DIR_TX_RX 

}csdlite_dev_direction_t;

typedef enum{
   CSDLITE_DTMF_GEN,
   CSDLITE_DTMF_DETECT,
   CSDLITE_DIR_GEN_DETECT 

}csdlite_dtmf_mode_t;

typedef uint32_t ( *csdlite_callback_fn_t ) (
  uint32_t cmd_token,
  uint32_t event_id,
  void* param,
  uint32_t param_size,
  void* cb_data
);

typedef struct csdlite_dtmf_gen_t{
  uint32_t cmd_token;
  csdlite_device_info_t dev_info;
  csdlite_dev_direction_t direction; 
  int64_t dtmf_duration_in_ms;
  uint16_t dtmf_high_freq;
  uint16_t dtmf_low_freq;
  uint16_t dtmf_gain;
  uint16_t mix_flag;
  csdlite_callback_fn_t cb_func;

}csdlite_dtmf_gen_t;

typedef struct csdlite_dtmf_event_t{
  uint16_t low_freq;
  uint16_t high_freq;
}csdlite_dtmf_event_t;

typedef struct csdlite_dtmf_detect_t{
  uint32_t cmd_token;
  csdlite_dev_direction_t direction; 
  bool_t enable;
  csdlite_callback_fn_t cb_func;
}csdlite_dtmf_detect_t;


typedef struct csdlite_volume_t
{
  uint32_t cmd_token;
  uint16_t vol_step;
  uint16_t ramp_duration;

}csdlite_volume_t;

typedef enum{
  CSDLITE_MUTE_STREAM,
  CSDLITE_MUTE_DEV
}csdlite_mute_mode_t;

typedef struct csdlite_mute_t
{
  uint32_t cmd_token;
  csdlite_mute_mode_t mode;
  uint16_t mute_flag;
  csdlite_dev_direction_t direction; 
  uint16_t ramp_duration;

}csdlite_mute_t;

typedef enum{
  CSDLITE_HDVOICE,
  CSDLITE_SLOWTALK,
  CSDLITE_SET_TTY_MODE,
}csdlite_pp_feature_mode_t;

typedef struct csdlite_feature_t
{
  uint32_t cmd_token;
  csdlite_pp_feature_mode_t feature;
  uint32_t mode;
}csdlite_feature_t;
                                                     
typedef enum
{
  CSDLITE_SR_8K = 8000,
  CSDLITE_SR_16K = 16000,
  CSDLITE_SR_48K = 48000
}csdlite_sample_rate_t;

typedef enum 
{
  CSDLITE_BPS_16 = 16,
  CSDLITE_BPS_24 = 24,
  CSDLITE_BPS_32 = 32
}csdlite_bps_t;

struct csdlite_dtmf_resp_t
{
  uint16_t low_freq;
  uint16_t high_freq;
};

/*----------------------------------------------------------------------------- 
  public APIs def
-----------------------------------------------------------------------------*/

/******************************************************************************
 * CSD Lite Voice Config                                                      *
 ******************************************************************************/

/**
  Sets up voice control path. 
   
  @payload
  #csdlite_voice_config_t

  @return
  #csd_status

  @dependencies
  None.
*/
uint32_t csdlite_voice_config(csdlite_voice_config_t* params);

/******************************************************************************
 * CSD Lite Voice Start                                                       *
 ******************************************************************************/

/**
  Sets up voice control path and starts voice. 
   
  @payload
  #csdlite_voice_config_t

  @return
  #csd_status

  @dependencies
  None.
*/
uint32_t csdlite_voice_start(csdlite_voice_config_t* params);

/******************************************************************************
 * CSD Lite Voice End                                                       *
 ******************************************************************************/

/**
  Ends voice control path. 
   
  @payload
  #csdlite_voice_end_t

  @return
  #csd_status

  @dependencies
  None.
*/
uint32_t csdlite_voice_end(csdlite_voice_end_t* params);

/******************************************************************************
 * CSD Lite Device Switch                                                     *
 ******************************************************************************/

/**
  Switches device. 
   
  @payload
  #csdlite_dev_switch_t

  @return
  #csd_status

  @dependencies
  device is enabled.
*/
uint32_t csdlite_dev_switch(csdlite_dev_switch_t* params);

/******************************************************************************
 * CSD Lite AFE loopback                                                      *
 ******************************************************************************/

/**
  puts device in AFE loopback state. 
   
  @payload
  #csdlite_afe_loopback_t

  @return
  #csd_status

  @dependencies
*/
uint32_t csdlite_afe_loopback(csdlite_afe_loopback_t* params);

/******************************************************************************
 * CSD Lite DTMF Generation                                                   *
 ******************************************************************************/

/**
  Generates a DTMF tone 
   
  @payload
  #csdlite_dtmf_gen_t

  @return
  #csd_status

  @dependencies
*/
uint32_t csdlite_dtmf_generation(csdlite_dtmf_gen_t* params);

/******************************************************************************
 * CSD Lite DTMF Detection                                                    *
 ******************************************************************************/

/**
  Detects a DTMF tone 
   
  @payload
  #csdlite_dtmf_detect_t

  @return
  #csd_status

  @dependencies
  in voice call
*/
uint32_t csdlite_dtmf_detection(csdlite_dtmf_detect_t* params);

/******************************************************************************
 * CSD Lite Volume                                                            *
 ******************************************************************************/

/**
  Changes volume step
   
  @payload
  #csdlite_volume_t

  @return
  #csd_status

  @dependencies
*/
uint32_t csdlite_volume(csdlite_volume_t* params);

/******************************************************************************
 * CSD Lite Mute                                                              *
 ******************************************************************************/

/**
  Mute/Un-Mutes voice call
   
  @payload
  #csdlite_mute_t

  @return
  #csd_status

  @dependencies
*/
uint32_t csdlite_mute(csdlite_mute_t* params);

/******************************************************************************
 * CSD Lite Set Voice Feature                                                 *
 ******************************************************************************/

/**
  Sets specific feature provided (TTY/HDVOICE/SLOWTALK)
   
  @payload
  #csdlite_feature_t

  @return
  #csd_status

  @dependencies
*/
uint32_t csdlite_set_voice_feature( csdlite_feature_t* params);

/******************************************************************************
 * CSD Lite get Version                                                       *
 ******************************************************************************/

/**
  returns current version of CSDLITE
   
  @payload

  @return
  #csd_status

  @dependencies
*/
uint32_t csdlite_get_version( void );


/*----------------------------------------------------------------------------- 
  init/deinit/cleanup
-----------------------------------------------------------------------------*/
uint32_t csdlite_init( void );

uint32_t csdlite_deinit( void );

void csdlite_cleanup( void );


#endif /*__CSDLITE_H__*/

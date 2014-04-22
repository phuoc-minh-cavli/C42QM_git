#ifndef L1_VOICE_SERVER_H
#define L1_VOICE_SERVER_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GSM VOICE SERVER API and HANDLING FUNCTIONS

GENERAL DESCRIPTION

   This modules provides the API for voice clients to exchange voice packets
   with GSM. This module also implements all the functions used necesary 
   for gsm to interact with GSM voice Server

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2015 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/geran.mpss/5.2.0/gl1/inc/gl1_voice_server.h#2 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
10/01/13   pjr       Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gl1_msg_g.h"
#include "gsm_voice_server_if.h"

/*===========================================================================

                     Public Defines & Constants

===========================================================================*/

typedef enum
{
  /* GSM Voice Adapter hasn't opened
   * a session with GSM yet */
  GL1_VOICE_SERVER_CLOSED = 0,
  /* Vocoder Not acquired yet */
  GL1_VOICE_SERVER_OPENED,
  /* Vocoder Started, but not yet ready */
  GL1_VOICE_SERVER_STARTED,
  /* Vocoder is ready to exchange frames */
  GL1_VOICE_SERVER_READY,
  /* Stop command was send but not yet acknowledged */
  GL1_VOICE_SERVER_STOPPED
} l1_voice_server_state_t;
  
typedef enum {
  DTX_DISABLED,
  DTX_ENABLED
} l1_vs_dtx_mode_t;

/* =======================================================================
**                   ENUMERATIONS used to report events
** ======================================================================= */
typedef enum
{
  VS_EVENT_TYPE_ACQUIRE,      
  VS_EVENT_TYPE_RELEASE,      
  VS_EVENT_TYPE_START_VOCODER,
  VS_EVENT_TYPE_STOP_VOCODER, 
  VS_EVENT_TYPE_STANDBY,      
  VS_EVENT_TYPE_SET_AMR_MODE, 
  VS_EVENT_TYPE_READY_CB,     
  VS_EVENT_TYPE_NOT_READY_CB,
  VS_EVENT_TYPE_SET_DTX_MODE, 
  VS_EVENT_TYPE_REQ_OWNERSHIP,
  VS_EVENT_TYPE_INVALID 
} l1_vs_event_code_t;

typedef enum
{
  VS_EVENT_STATUS_SUCCESS,
  VS_EVENT_STATUS_FAIL,
  VS_EVENT_STATUS_FAIL_TO_ALLOC_BUFFER,
  VS_EVENT_STATUS_FAIL_TO_FLUSH_BUFFER,
  VS_EVENT_STATUS_ALREADY_OPEN,
  VS_EVENT_STATUS_VS_NOT_OPEN,
  VS_EVENT_STATUS_VS_NOT_READY,
  VS_EVENT_STATUS_INVALID_AS_ID,
  VS_EVENT_STATUS_INVALID_VOCODER_TYPE
} l1_vs_event_status_t;

typedef enum
{
  VS_EVENT_MODE_NULL,
  VS_EVENT_MODE_AMR,
  VS_EVENT_MODE_EFR,
  VS_EVENT_MODE_HR,
  VS_EVENT_MODE_AMR_WB,
  VS_EVENT_MODE_FR,

  VS_EVENT_MODE_0475,
  VS_EVENT_MODE_0515,
  VS_EVENT_MODE_0590,
  VS_EVENT_MODE_0670,
  VS_EVENT_MODE_0740,
  VS_EVENT_MODE_0795,
  VS_EVENT_MODE_1020,
  VS_EVENT_MODE_1220,
  VS_EVENT_MODE_WB_0660,
  VS_EVENT_MODE_WB_0885,
  VS_EVENT_MODE_WB_1265,

  VS_EVENT_MODE_DTX_ON,
  VS_EVENT_MODE_DTX_OFF
} l1_vs_event_mode_t;

/* voice_server_session_data_t includes all the variables associated to 
 * a voice server session */
typedef struct
{
  l1_voice_server_state_t         vocoder_state;
  void*                           client_session_data;
  gsm_icommon_event_callback_fn_t gsm_event_cb;
  gsm_ivocoder_callback_fn_t      ul_cb;
  gsm_ivocoder_callback_fn_t      dl_cb;
  l1_vs_dtx_mode_t                dtx_mode;
  uint32                          codec_mode;
  uint32                          vocoder_id;
  sys_modem_as_id_e_type          as_id;
  unsigned int                    lock;
} voice_server_session_data_t;


/* =======================================================================
**               FUNCTIONS used to interact with Voice Server
** ======================================================================= */

/* Request GSM Voice Adapter to acquire and enable then vocoder,
 * This function set the vocoder ID (AMR, AMRWB, FR, EFR or HR) */
extern uint32 l1_vs_start_client(gl1_msg_chan_type vs_msg_chan_type,
                                 gl1_msg_voc_type vs_msg_voc_type);

/* Request GSM Voice Adapter to disable the vocoder */
extern uint32 l1_vs_stop_client(void);                                            

/* Sets the codec mode for UL frames */
extern uint32 l1_vs_set_ul_amr_mode(gl1_msg_amr_mode_type codec_mode);         


/* Pass an AMR DL packet to GVA */
void l1_vs_send_amr_packet_to_client (uint8 *data, gl1_msg_amr_frame_info_type *frame);

/* Pass a DL packet to GVA */
void l1_vs_send_gsm_packet_to_client ( uint8 *data, boolean bfi, uint8 sid, 
                                       boolean taf, boolean ufi);

/* Pass an AMR UL packet to GVA */
boolean l1_vs_get_amr_packet_from_client (uint8 *data, uint8 datalength, 
                                          gl1_msg_amr_frame_info_type *frame);

/* Pass an UL packet to GVA */
boolean l1_vs_get_gsm_packet_from_client (uint8 *data, uint8 datalength, 
                                          uint8 *packet_available);

/* Enable/Disable DTX mode */
extern uint32 l1_vs_set_dtx_mode(l1_vs_dtx_mode_t dtx_mode);

/* Request GSM Voice Adapter to grant the vocoder to GSM during irat HO */
extern uint32 l1_vs_request_vocoder_ownership(void); 

/* Returns TRUE if the client is ready to exchange voice frames */
extern boolean l1_vs_is_client_ready(void);
  
/* Returns the voice server state */
extern l1_voice_server_state_t l1_vs_get_voice_server_state(void);

/* Returns the Vocoder in use (AMR, AMRWB, FR, EFR or HR) */
extern uint32 l1_vs_get_vocoder_id(void);

/* Returns true if dtx mode is enabled */
extern boolean l1_vs_get_dtx_mode(void);

/* Convert from gl1_msg_amr_frame_type to GSM_IVOCAMR_FRAME_TYPE */
extern uint32 l1_vs_map_amr_frame_type_to_gsm_ivocamr(gl1_msg_amr_frame_type frame_type);

/* Convert from gl1_msg_amr_frame_type to GSM_IVOCAMRWB_FRAME_TYPE */
extern uint32 l1_vs_map_amr_frame_type_to_gsm_ivocamrwb(gl1_msg_amr_frame_type frame_type);

/* Convert from gl1_msg_amr_mode_type to GSM_IVOCAMR_CODEC_MODE/GSM_IVOCAMRWB_CODEC_MODE */
extern uint32 l1_vs_map_amr_mode_type_to_gsm_ivoc_codec_mode(gl1_msg_amr_mode_type codec_mode);

/* Convert from GSM_IVOCAMR_CODEC_MODE/GSM_IVOCAMRWB_CODEC_MODE to gl1_msg_amr_mode_type */
extern gl1_msg_amr_mode_type l1_vs_map_gsm_ivoc_codec_mode_to_amr_codec_mode(uint32 gsm_ivocamr_codec_mode);

/* Convert from GSM_IVOCAMR_FRAME_TYPE/GSM_IVOCAMRWB_FRAME_TYPE to gl1_msg_amr_frame_type  */
extern gl1_msg_amr_frame_type l1_vs_map_gsm_ivoc_frame_type_to_amr_frame_type(uint32 frame_type);

#endif


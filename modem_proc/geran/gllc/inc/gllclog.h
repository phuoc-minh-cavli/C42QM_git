#ifndef LLC_LOG_H
#define LLC_LOG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            LLC Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging LLC packets.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gllc/inc/gllclog.h#3 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
09/05/02   ADG     Initial Revision.
===========================================================================*/

#include "geran_variation.h"

#include "comdef.h"      /* for types */
#include "gs.h"

#ifndef FEATURE_GERAN_REDUCED_DEBUG
#define llc_log_init()                                              _llc_log_init()
#define llc_log_ota_message(downlink, msg_type, length, msg_ptr)    _llc_log_ota_message(downlink, msg_type, length, msg_ptr)
#define llc_log_ready_timer_status()                                _llc_log_ready_timer_status()
#define llc_log_send_ui_frame(send_seq, sapi, pdu_len, cipher_mode) _llc_log_send_ui_frame(send_seq, sapi, pdu_len, cipher_mode)
#define llc_log_recv_ui_frame(send_seq, sapi, pdu_len, cipher_mode) _llc_log_recv_ui_frame(send_seq, sapi, pdu_len, cipher_mode)
#else /* !FEATURE_GERAN_REDUCED_DEBUG */
#define llc_log_init()
#define llc_log_ota_message(downlink, msg_type, length, msg_ptr)
#define llc_log_ready_timer_status()
#define llc_log_send_ui_frame(send_seq, sapi, pdu_len, cipher_mode)
#define llc_log_recv_ui_frame(send_seq, sapi, pdu_len, cipher_mode)
#endif /* FEATURE_GERAN_REDUCED_DEBUG */

#ifndef FEATURE_GERAN_REDUCED_DEBUG
/* INITIALISATION */
extern void _llc_log_init( void );

/* Log-packet-request callback to be registered with DIAG */
extern void llc_log_packet_request(uint16 log_code, uint8 zero_stats, uint8 additional_info);

/* LOG PACKETS */
extern void llc_log_me_info(void);
extern void llc_log_sapi_states(uint8 sapi);
extern void llc_log_xid_info(uint8 sapi);
extern void llc_log_pdu_stats(uint8 sapi, uint8 zero_stats);
extern void llc_log_periodic_stats(uint8 sapi, uint8 zero_stats);
extern void _llc_log_ota_message(boolean downlink, uint8 message_type, uint16 message_length, const uint8 *message_ptr);
extern void _llc_log_ready_timer_status(void);
extern void _llc_log_send_ui_frame(uint16 seq_num, uint8 sapi, uint16 pdu_len, uint8 cipher_mode);
extern void _llc_log_recv_ui_frame(uint16 seq_num, uint8 sapi, uint16 pdu_len, uint8 cipher_mode);


/* EVENTS */
/* extern void llc_log_some_event(args); */
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */



extern uint8 gllc_gs_queue;

#endif /* LLC_LOG_H */

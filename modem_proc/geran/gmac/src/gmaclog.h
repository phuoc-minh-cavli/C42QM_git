#ifndef MAC_LOG_H
#define MAC_LOG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            MAC Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging MAC packets.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gmac/src/gmaclog.h#4 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
08/09/02   ADG     Initial Revision.
===========================================================================*/


#include "geran_variation.h"
#include "comdef.h" /* for types */
#include "gmacdef.h"         /* For MAC mode                               */

#ifndef FEATURE_GERAN_REDUCED_DEBUG
#define mac_log_init()                              _mac_log_init()
#define mac_log_deinit()                            _mac_log_deinit()
#define mac_log_state()                             _mac_log_state()
#define mac_log_ul_tbf_release(reason)              _mac_log_ul_tbf_release(reason)
#define mac_log_dl_tbf_release(reason)              _mac_log_dl_tbf_release(reason)
#define mac_log_camped_on_cell()                    _mac_log_camped_on_cell()
#define mac_log_dl_acknack(type, len, msg)          _mac_log_dl_acknack(type, len, msg)
#define mac_log_ul_acknack(len, msg)                _mac_log_ul_acknack(len, msg)
#define mac_log_grr_events(signal_id)               _mac_log_grr_events(signal_id)
#define mac_log_gprs_pua_event(ts, cs)              _mac_log_gprs_pua_event(ts, cs)
#define mac_log_gprs_pda_event(ts, tfi)             _mac_log_gprs_pda_event(ts, tfi)
#define mac_log_packet_tbf_rel_event(type)          _mac_log_packet_tbf_rel_event(type)
#define mac_log_prr_event(access_req)               _mac_log_prr_event(access_req)
#define mac_log_ptr_event(u_ts, d_ts, u_tfi, d_tfi) _mac_log_ptr_event(u_ts, d_ts, u_tfi, d_tfi)
#else /* !FEATURE_GERAN_REDUCED_DEBUG */
#define mac_log_init()
#define mac_log_deinit()
#define mac_log_state()                             FALSE
#define mac_log_ul_tbf_release(reason) 
#define mac_log_dl_tbf_release(reason) 
#define mac_log_camped_on_cell()
#define mac_log_dl_acknack(type, len, msg)
#define mac_log_ul_acknack(len, msg)
#define mac_log_grr_events(signal_id)
#define mac_log_gprs_pua_event(ts, cs)
#define mac_log_gprs_pda_event(ts, tfi)
#define mac_log_packet_tbf_rel_event(type)
#define mac_log_prr_event(access_req)
#define mac_log_ptr_event(u_ts, d_ts, u_tfi, d_tfi)
#endif /* FEATURE_GERAN_REDUCED_DEBUG */


typedef enum
{
  MAC_LOG_UL_NORMAL_RELEASE                          = 0x0,
  MAC_LOG_UL_FREQ_ERROR_ACCESS_RETRY                 = 0x01,
  MAC_LOG_UL_FREQ_ERROR_PARTIAL_SYS_ACQ              = 0x02,
  MAC_LOG_UL_MULTISLOT_ERROR                         = 0x03,
  MAC_LOG_UL_TBF_STARTING_TIME_ERROR_ACCESS_RETRY    = 0x04,
  MAC_LOG_UL_ASSIGNMENT_ADDRESSING_ERROR             = 0x05,
  MAC_LOG_UL_ASSIGNMENT_DATA_ERROR                   = 0x06,
  MAC_LOG_UL_FREQ_ERROR_DIFFERENT_PARAMS             = 0x07,
  MAC_LOG_UL_TBF_STARTING_TIME_ERROR_WITHOUT_RETRY   = 0x08,
  MAC_LOG_UL_FREQ_BAND_NOT_SUPPORTED                 = 0x09,
  MAC_LOG_UL_CSN1_DECODING_ERROR                     = 0x0a,
  MAC_LOG_UL_CS_PAGING_ABORT                         = 0x0b,
  MAC_LOG_UL_PS_PAGING_ABNORMAL                      = 0x0c,   
  MAC_LOG_UL_NO_NETWORK_ACCESS                       = 0x0d,
  MAC_LOG_UL_GPRS_SUSPENSION                         = 0x0e,
  MAC_LOG_UL_ACCESS_REJECT                           = 0x0f,
  MAC_LOG_UL_FAILED_CONTENTION_RES                   = 0x10,
  MAC_LOG_UL_PDA_ABORT                               = 0x11,
  MAC_LOG_UL_RELEASE_IND                             = 0x12,
  MAC_LOG_UL_RLC_DL_RELEASE_IND                      = 0x13,
  MAC_LOG_UL_CELL_RESELECTION                        = 0x14,              
  MAC_LOG_UL_SENDING_PCCF                            = 0x15,
  MAC_LOG_UL_INVALID_MAC_MODE                        = 0x16,
  MAC_LOG_UL_PH_CONNECT_FAILED                       = 0x17,
  MAC_LOG_UL_TBF_RELEASE                             = 0x18,
  MAC_LOG_UL_FAI_RELEASE                             = 0x19,
  MAC_LOG_UL_EXPIRED_T3186                           = 0x1a,
  MAC_LOG_UL_EXPIRED_T3184                           = 0x1b,
  MAC_LOG_UL_EXPIRED_T3180                           = 0x1c,
  MAC_LOG_UL_EXPIRED_T3176                           = 0x1d,
  MAC_LOG_UL_EXPIRED_T3174                           = 0x1e,
  MAC_LOG_UL_EXPIRED_T3170                           = 0x1f,
  MAC_LOG_UL_EXPIRED_T3168                           = 0x20,
  MAC_LOG_UL_EXPIRED_T3164                           = 0x21,
  MAC_LOG_UL_EXPIRED_T3162                           = 0x22,
  MAC_LOG_UL_EXPIRED_N3104                           = 0x23,
  MAC_LOG_UL_UNKNOWN_ERROR                           = 0x24
}mac_log_ul_tbf_release_t;


typedef enum
{
 MAC_LOG_DL_NORMAL_RELEASE                          = 0x0,
 MAC_LOG_DL_FREQ_ERROR_ACCESS_RETRY                 = 0x01,
 MAC_LOG_DL_FREQ_ERROR_PARTIAL_SYS_ACQ              = 0x02,
 MAC_LOG_DL_MULTISLOT_ERROR                         = 0x03,
 MAC_LOG_DL_TBF_STARTING_TIME_ERROR_ACCESS_RETRY    = 0x04,
 MAC_LOG_DL_ASSIGNMENT_ADDRESSING_ERROR             = 0x05,
 MAC_LOG_DL_ASSIGNMENT_DATA_ERROR                   = 0x06,
 MAC_LOG_DL_FREQ_ERROR_DIFFERENT_PARAMS             = 0x07,
 MAC_LOG_DL_TBF_STARTING_TIME_ERROR_WITHOUT_RETRY   = 0x08,
 MAC_LOG_DL_TBF_RELEASE                             = 0x09,
 MAC_LOG_DL_FREQ_BAND_NOT_SUPPORTED                 = 0x0a,
 MAC_LOG_DL_CELL_RESELECTION                        = 0x0b,
 MAC_LOG_DL_SENDING_PCCF                            = 0x0c,        
 MAC_LOG_DL_INVALID_MAC_MODE                        = 0x0d,
 MAC_LOG_DL_RELEASE_IND                             = 0x0e,
 MAC_LOG_DL_RLC_UL_RELEASE_IND                      = 0x0f,
 MAC_LOG_DL_PH_CONNECT_FAILED                       = 0x10,
 MAC_LOG_DL_SB_ABORT                                = 0x11,
 MAC_LOG_DL_CSN1_DECODING_ERROR                     = 0x12,
 MAC_LOG_DL_CS_PAGING_ABORT                         = 0x13,
 MAC_LOG_DL_PS_PAGING_ABNORMAL                      = 0x14,   
 MAC_LOG_DL_GPRS_SUSPENSION                         = 0x15,
 MAC_LOG_DL_EXPIRED_T3184                           = 0x16,
 MAC_LOG_DL_EXPIRED_T3180                           = 0x17,
 MAC_LOG_DL_EXPIRED_T3168                           = 0x18,
 MAC_LOG_DL_UNKNOWN_ERROR                           = 0x19
}mac_log_dl_tbf_release_t;


#ifndef FEATURE_GERAN_REDUCED_DEBUG
/* INITIALISATION */
void _mac_log_init(void);
/* DE-INIT*/
void _mac_log_deinit(void);
/* LOG PACKETS */
boolean _mac_log_state(void);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
boolean mac_log_signalling_message(byte channel_type,
                                         byte message_type,
                                         byte message_length,
                                         const byte *message_ptr);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
void mac_log_ul_tbf_establish(byte ul_tfi, byte ul_ts_bmap);
void mac_log_ul_tbf_release(mac_log_ul_tbf_release_t failure_reason);
void mac_log_dl_tbf_establish(byte dl_tfi,byte dl_ts_bmap, byte rlc_mode);



 
// EGPRS 3.1: added 4 extra FPAR for EGPRS
void mac_log_dl_etbf_establish(byte dl_tfi, byte dl_ts_bmap, byte rlc_mode, byte  egprs_tbf, 
                               uint16  egprs_window_size, byte  egprs_link_qual_mode, byte  bep_period2);

// EGPRS 3.2: added 4 extra FPAR for EGPRS
void mac_log_ul_etbf_establish(byte ul_tfi, byte ul_ts_bmap, byte  egprs_tbf, uint16  egprs_window_size, 
                               byte  resegment, byte  bep_period2);

void _mac_log_dl_tbf_release(mac_log_dl_tbf_release_t failure_reason);
void _mac_log_camped_on_cell(void);

/* Two new functions for EGPRS, but can also be used in GPRS if we want. Therefore not featured.*/


// EGPRS 3.5 0x522D� EGPRS Downlink AckNack, direction MS -> NW 
void _mac_log_dl_acknack( byte type,  uint8 acknack_byte_len,  uint8* dl_acknak );

void mac_log_ul_acknack_with_byte_len( uint16 acknack_byte_len,   uint8*  ul_acknak );

// EGPRS 3.6 0x522C� EGPRS Uplink AckNack, direction NW -> MS 
void _mac_log_ul_acknack( uint16 acknack_bit_len,   uint8*  ul_acknak );


/*event logging changes*/
void _mac_log_grr_events( uint8 signal_id );
void _mac_log_gprs_pua_event( uint8 ts_alloc, uint8 cs );
void _mac_log_gprs_pda_event( uint8 ts_alloc, uint8 tfi );
void _mac_log_packet_tbf_rel_event( release_tbf_t release_type );
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
void mac_log_timer_event(const uint32 timer_id);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
void _mac_log_prr_event(access_type_e access_req);
uint8 mac_log_convert_single_ts_to_bitmp( uint8 single_ts );
void _mac_log_ptr_event(uint8 ul_ts, uint8 dl_ts, uint8 ul_tfi, uint8 dl_tfi);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

/* EVENTS */
/* extern void mac_log_some_event(args); */


#endif /* MAC_LOG_H */

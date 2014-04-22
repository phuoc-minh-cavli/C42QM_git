#ifndef RLC_LOG_H
#define RLC_LOG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RLC Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging RLC packets.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grlc/inc/grlclog.h#4 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
09/09/02   ADG     Initial Revision.
===========================================================================*/

#include "geran_variation.h"
#include "comdef.h"     /* for types */
#include "grlcmac.h"    /* for rlc_log_epuan(..) */
#include "gmacrlc.h"    /* for rlc_log_epdan(..) */
#include "log.h"        /* For log_commit(), log_alloc(), log codes   */

typedef enum
{
  RLC_LOG_UL_MAC_RELEASE_IND,
  RLC_LOG_UL_EXPIRED_T3182_IN_ACK_RELEASE,
  RLC_LOG_UL_EXPIRED_T3182_IN_ACK_TRANSFER,
  RLC_LOG_UL_EXPIRED_N3102_IN_ACK_TRANSFER,
  RLC_LOG_UL_EXPIRED_T3182_IN_UNACK_RELEASE,
  RLC_LOG_UL_EXPIRED_T3182_IN_UNACK_TRANSFER,
  RLC_LOG_UL_EXPIRED_N3102_IN_UNACK_TRANSFER
}rlc_log_ul_release_t;

typedef enum
{
  RLC_LOG_DL_MAC_RELEASE_IND,
  RLC_LOG_DL_EXPIRED_T3192_IN_ACK_TRANSFER,
  RLC_LOG_DL_EXPIRED_T3190_IN_ACK_TRANSFER,
  RLC_LOG_DL_EXPIRED_T3192_IN_UNACK_TRANSFER,
  RLC_LOG_DL_EXPIRED_T3190_IN_UNACK_TRANSFER
}rlc_log_dl_release_t;

typedef enum 
{ 
  GPRS_HEADER_TYPE, 
  EGPRS_HEADER_TYPE_1,  
  EGPRS_HEADER_TYPE_2, 
  EGPRS_HEADER_TYPE_3
}EGPRS_hdr_type;

#ifndef FEATURE_GERAN_REDUCED_DEBUG
typedef struct
{
  uint32  dl_rx;
  uint32  ul_tx;
  uint32  ul_retx;
}rlc_log_rfblks_cnts_t;

typedef struct
{
  uint32  dl_rx;
  uint32  dl_reprx;
}rlc_eng_mode_dl_cnts_t;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */


/* Log 3 Bytes of E/GPPRS Payload
*/
#define LOG_MAX_LI_E  3 

#ifndef FEATURE_GERAN_REDUCED_DEBUG
#define rlc_log_ul_statistics(stats)               _rlc_log_ul_statistics(stats)
#define rlc_log_dl_statistics(stats)               _rlc_log_dl_statistics(stats)
#define rlc_log_ul_acknack_params()                _rlc_log_ul_acknack_params()
#define rlc_log_dl_acknack_params(a,b,c,d,e,f,g)   _rlc_log_dl_acknack_params(a,b,c,d,e,f,g)
#define rlc_log_ul_release_ind(tfi, cause)         _rlc_log_ul_release_ind(tfi, cause)
#define rlc_log_dl_release_ind(tfi, cause)         _rlc_log_dl_release_ind(tfi, cause)
#define rlc_log_dl_header(a,b,c,d,e,f)             _rlc_log_dl_header(a,b,c,d,e,f)
#define rlc_log_ul_header(a,b)                     _rlc_log_ul_header(a,b)
#define rlc_log_epuan(puan_msg)                    _rlc_log_epuan(puan_msg)
#define rlc_log_epdan(pdan_msg)                    _rlc_log_epdan(pdan_msg)
#else /* !FEATURE_GERAN_REDUCED_DEBUG */
#define rlc_log_ul_statistics(stats)
#define rlc_log_dl_statistics(stats)
#define rlc_log_ul_acknack_params() 
#define rlc_log_dl_acknack_params(a,b,c,d,e,f,g)
#define rlc_log_ul_release_ind(tfi, cause) 
#define rlc_log_dl_release_ind(tfi, cause) 
#define rlc_log_dl_header(a,b,c,d,e,f)
#define rlc_log_ul_header(a,b)
#define rlc_log_epuan(puan_msg)
#define rlc_log_epdan(pdan_msg)
#endif /* FEATURE_GERAN_REDUCED_DEBUG */

#ifndef FEATURE_GERAN_REDUCED_DEBUG
/* INITIALISATION */
extern void rlc_log_init(void);
extern log_on_demand_status_enum_type rlc_log_on_demand(log_code_type log_code);
extern void rlc_log_packet_request(uint16 log_code, uint8 zero_stats, uint8 additional_info);

/* LOG PACKETS */
extern void _rlc_log_ul_statistics(boolean zero_stats);
extern void _rlc_log_dl_statistics(boolean zero_stats);
extern void _rlc_log_ul_acknack_params(void);

extern void _rlc_log_dl_acknack_params
(
 uint8  dl_tfi,
 uint8  fai, 
 uint8  ssn,
 uint8  vq,
 uint32 hu32,
 uint32 lu32,
 uint8  cs
);

extern void _rlc_log_ul_release_ind(uint8 ul_tfi, rlc_log_ul_release_t release_cause);
extern void _rlc_log_dl_release_ind(uint8 dl_tfi, rlc_log_dl_release_t release_cause);
extern rlc_log_rfblks_cnts_t* rlc_log_rfblks_cnts(void);
extern rlc_eng_mode_dl_cnts_t* rlc_log_dl_blks_cnts(void);


extern void _rlc_log_dl_header
(
 byte        type,
 const byte* dl_header,
 byte        crc_status,
 byte        time_slot,
 uint32      frame_number,
 const byte* dl_payload
);

extern void _rlc_log_ul_header
(
 byte        type,   
 const byte* ul_header
);

extern void _rlc_log_epuan(const mr_ul_pkt_acknack_ind_t* puan_msg);
extern void _rlc_log_epdan(const rm_dl_pkt_acknack_req_t* pdan_msg);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
/* EVENTS */
/* extern void rlc_log_some_event(args); */

#endif /* RLC_LOG_H */

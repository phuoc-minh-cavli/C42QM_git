#ifndef ROHC_LOGGING_H
#define ROHC_LOGGING_H
/*===========================================================================
               P S _ R O H C _ L O G G I N G . H

DESCRIPTION
  This file contains ROHC logging specific data types.

Copyright (c) 2014-2015 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/protocols/hc/inc/ps_rohc_logging.h#1 $
  $Author: pwbldsvc $
  $DateTime: 2020/03/17 08:03:16 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/22/15    ds     changed data type. 
05/27/14    ss     Initial version
===========================================================================*/
/*===========================================================================

                             INCLUDE FILES 

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "log.h"

typedef enum rohc_packet_type_e_type {
  ROHC_PACKET_TYPE_NONE       =  0,
  ROHC_PACKET_TYPE_R0         =  1,
  ROHC_PACKET_TYPE_R1         =  2,
  ROHC_PACKET_TYPE_R1TS       =  3,
  ROHC_PACKET_TYPE_R1ID       =  4,
  ROHC_PACKET_TYPE_R1Ext0     =  5,
  ROHC_PACKET_TYPE_R1TSExt0   =  6,
  ROHC_PACKET_TYPE_R1IDExt0   =  7,
  ROHC_PACKET_TYPE_R1Ext1     =  8,
  ROHC_PACKET_TYPE_R1TSExt1   =  9,
  ROHC_PACKET_TYPE_R1IDExt1   = 10,
  ROHC_PACKET_TYPE_R1Ext2     = 11,
  ROHC_PACKET_TYPE_R1TSExt2   = 12,
  ROHC_PACKET_TYPE_R1IDExt2   = 13,
  ROHC_PACKET_TYPE_R1Ext3     = 14,
  ROHC_PACKET_TYPE_R1TSExt3   = 15,
  ROHC_PACKET_TYPE_R1IDExt3   = 16,
  ROHC_PACKET_TYPE_UO0        = 17,
  ROHC_PACKET_TYPE_UO1        = 18,
  ROHC_PACKET_TYPE_UO1TS      = 19,
  ROHC_PACKET_TYPE_UO1ID      = 20,
  ROHC_PACKET_TYPE_UO1IDExt0  = 21,
  ROHC_PACKET_TYPE_UO1IDExt1  = 22,
  ROHC_PACKET_TYPE_UO1IDExt2  = 23,
  ROHC_PACKET_TYPE_UO1IDExt3  = 24,
  ROHC_PACKET_TYPE_UOR2       = 25,
  ROHC_PACKET_TYPE_UOR2TS     = 26,
  ROHC_PACKET_TYPE_UOR2ID     = 27,
  ROHC_PACKET_TYPE_UOR2Ext0   = 28,
  ROHC_PACKET_TYPE_UOR2TSExt0 = 29,
  ROHC_PACKET_TYPE_UOR2IDExt0 = 30,
  ROHC_PACKET_TYPE_UOR2Ext1   = 31,
  ROHC_PACKET_TYPE_UOR2TSExt1 = 32,
  ROHC_PACKET_TYPE_UOR2IDExt1 = 33,
  ROHC_PACKET_TYPE_UOR2Ext2   = 34,
  ROHC_PACKET_TYPE_UOR2TSExt2 = 35,
  ROHC_PACKET_TYPE_UOR2IDExt2 = 36,
  ROHC_PACKET_TYPE_UOR2Ext3   = 37,
  ROHC_PACKET_TYPE_UOR2TSExt3 = 38,
  ROHC_PACKET_TYPE_UOR2IDExt3 = 39,
  ROHC_PACKET_TYPE_R0CRC      = 40,
  ROHC_PACKET_TYPE_IRDYN      = 41,
  ROHC_PACKET_TYPE_IR_S       = 42,
  ROHC_PACKET_TYPE_IR_S_D     = 43,
  ROHC_PACKET_TYPE_IR_S_D_P   = 44,
  ROHC_PACKET_TYPE_UNC_IR     = 45,
  ROHC_PACKET_TYPE_UNC_NORMAL = 46,
  ROHC_PACKET_TYPE_FORCE_32_BIT = 0x7FFFFFFF
} rohc_packet_type_e_type;

typedef enum rohc_feedback_type_e_type {
  ROHC_FEEDBACK_TYPE_ACK   = 0x00000000,
  ROHC_FEEDBACK_TYPE_NACK  = 0x00000001,
  ROHC_FEEDBACK_TYPE_SNACK = 0x00000002,
  ROHC_FEEDBACK_TYPE_FORCE_32_BIT = 0x7FFFFFFF
} rohc_feedback_type_e_type;

/* This enum will be used for Logging */
typedef enum rohc_feedback_option_mask_e_type {
  ROHC_FEEDBACK_OPTION_NONE         = 0x00000000,
  ROHC_FEEDBACK_OPTION_CRC          = 0x00000001,
  ROHC_FEEDBACK_OPTION_REJECT       = 0x00000002,
  ROHC_FEEDBACK_OPTION_SN_NOT_VALID = 0x00000004,
  ROHC_FEEDBACK_OPTION_SN           = 0x00000008,
  ROHC_FEEDBACK_OPTION_CLOCK        = 0x00000010,
  ROHC_FEEDBACK_OPTION_JITTER       = 0x00000020,
  ROHC_FEEDBACK_OPTION_LOSS         = 0x00000040,
  ROHC_FEEDBACK_OPTION_CONTEXT_MEMORY = 0x00000080,
  ROHC_FEEDBACK_OPTION_MAX          = 0x40000000
} rohc_feedback_option_mask_e_type;

/**
 * Channel specific statistics for the compressor.
 * The stats can be accessed via comp_get_stats() function.
 */
typedef struct comp_stats {
    /* General */
    uint8 version;             /* Version (QXDM enforced)                     */
    uint32 out_packets;        /* Nr of packets generated by compressor.      */
    uint32 bytes_original;     /* Total nr of bytes of original headers.      */
    uint32 bytes_comp;         /* Total nr of bytes after compression.        */
    uint32 pkt_bytes_original; /* Total nr of bytes of original packets.      */
    uint32 pkt_bytes_comp;     /* Total nr of bytes after compression.        */
  
    /* Packet Types */
    uint32 ir_produced;        /* Nr of IRs produced by compressor.           */
    uint32 ir_dyn_produced;    /* Nr of IR-DYNs produced by compressor.       */
    uint32 pkt_in_fo_state;    /* Nr of packets produced by compressor 
                                 in first order state.                       */
    uint32 pkt_in_so_state;    /* Nr of packets produced by compressor 
                                 in second order state.                      */
    uint32 t0_produced;        /* Nr of type0 packets produced by compressor. */
    uint32 t1_produced;        /* Nr of type1 packets produced by compressor. */
    uint32 t2_produced;        /* Nr of type2 packets produced by compressor. */
    uint32 uncompressed;       /* Nr of packets sent uncompressed.            */

    /* Feedback */
    uint32 in_acks;            /* Nr of acks received by compressor.          */
    uint32 in_nacks;           /* Nr of nacks received by compressor.         */
    uint32 in_snacks;          /* Nr of snacks received by compressor.        */
  
    /* Compressor Errors */
    uint32 comp_failed;        /* Nr of compression failures.                 */
    uint32 comp_failed_ir_state; /* Nr. of compression failure in IR state.   */
    uint32 comp_failed_fo_state; /* Nr. of compression failure in FO state.   */
    uint32 comp_failed_so_state; /* Nr. of compression failure in FO state.   */

    /* Compressor Errors in % */
    float comp_failure_percent_ir_state; /* compression failure % in IR state */
    float comp_failure_percent_fo_state; /* compression failure % in FO state */
    float comp_failure_percent_so_state; /* compression failure % in SO state */
} comp_stats_t;

/**
 * Channel specific statistics for the decompressor.
 * The stats can be accessed via decomp_get_stats() function.
 */
typedef struct decomp_stats {
    /* General */
    uint8 version;       /* version (QXDM enforced)               */
    uint32 in_packets;     /* Nr of packets in to decompressor.     */
    uint32 bytes_original; /* Total original HDR number of bytes.   */
    uint32 bytes_decomp;   /* Total HDR bytes before decompression. */
  
    /* Packet Types */
    uint32 ir_received;     /* Nr of IRs received by decompressor.           */
    uint32 ir_dyn_received; /* Nr of IR-DYNs received by decompressor.       */
    uint32 pkt_recvd_fc_state; /* Nr of packets received in Full Context 
                                 state.                                     */
    uint32 pkt_recvd_sc_state; /* Nr of packets received in static context 
                                 state.                                     */
    uint32 t0_received;     /* Nr of type0 packets received by decompressor. */
    uint32 t1_received;     /* Nr of type1 packets received by decompressor. */
    uint32 t2_received;     /* Nr of type2 packets received by decompressor. */
    uint32 uncompressed;    /* Nr of packets received uncompressed.          */

    /* Feedback */
    uint32 out_acks;       /* Nr of acks generated by decompressor.           */
    uint32 out_nacks;      /* Nr of nacks generated by decompressor.          */
    uint32 out_snacks;     /* Nr of static nacks generated by decompressor.   */
    uint32 piggybacked_fb; /* Nr of piggybacked fbs received by decompressor. */
  
    /* Decompressor Errors */
    uint32 decomp_failed;    /* Nr of decompression failures.                 */
    uint32 ir_failed;        /* Nr of IR packet failures.                     */
    uint32 ir_dyn_failed;    /* Nr of IR-DYN packet failures.                 */
    uint32 decomp_failed_fc_state; /* Nr of decompression failure in FC state.*/
    uint32 decomp_failed_sc_state; /* Nr of decompression failure in SC state.*/
    uint32 t0_failed;        /* Nr of type0 packet failures.                  */
    uint32 t1_failed;        /* Nr of type1 packet failures.                  */
    uint32 t2_failed;        /* Nr of type2 packet failures.                  */
    uint32 crc_failures;     /* Nr of CRC decompression failures.             */
    uint32 parse_errors;     /* Nr of header parsing/illegal packet errors    */
    uint32 illegal_states;   /* Nr of packets failed in lower context states. */
    uint32 invalid_contexts; /* Nr of times the Decomp ctx was invalidated.   */
    uint32 unknown_profiles; /* Nr of failures due to unsuppoted profiles.    */

    /* Context repair */
    uint32 context_repair_successes; /* Nr of successes in context repair.    */
    uint32 context_repair_discards;  /* Nr of pkt discarded after successful  
                                                                 ctx repair  */
       /* Decompressor Errors % */
    float decomp_failure_percent_ir_state; /* decompression failure % in No 
                                              context state.                 */
    float decomp_failure_percent_sc_state; /* decompression failure % in 
                                              Static Context state.          */
    float decomp_failure_percent_fc_state; /* decompression failure % in 
                                              Full Context  state.           */
} decomp_stats_t;

/*---------------------------------------------------------------------------
  Log packet for log codes:
    LOG_ROHC_COMPRESSOR_C
    LOG_ROHC_DECOMPRESSOR_C
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST {
  log_hdr_type               hdr;
  unsigned int               received_timestamp;
  unsigned int               rohc_instance;
  unsigned int               cid;
  unsigned int               profile;
  unsigned int               mode;
  unsigned int               state; /* rohc_comp_state_e_type/rohc_decomp_state_e_type */
  unsigned int               error;
  rohc_packet_type_e_type    packet_type;
  unsigned int               compressed_header_size;
  unsigned int               original_header_size;
  unsigned int               sn;
  unsigned int               sn_window_size;
  unsigned int               sn_min_ref;
  unsigned int               sn_max_ref;
  unsigned int               rtp_ts;
  unsigned int               rtp_ts_scaled;
  unsigned int               rtp_ts_window_size;
  unsigned int               rtp_ts_min_ref;
  unsigned int               rtp_ts_max_ref;
} rohc_log_packet_s_type;

/*---------------------------------------------------------------------------
  Log packet for log codes:
    LOG_ROHC_FEEDBACK_COMPRESSOR_C
    LOG_ROHC_FEEDBACK_DECOMPRESSOR_C
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST {
  log_hdr_type                     hdr;
  unsigned int                     received_timestamp;
  unsigned int                     rohc_instance;
  unsigned int                     cid;
  unsigned int                     profile;
  unsigned int                     mode;
  unsigned int                     error;
  rohc_feedback_type_e_type        fb_type;
  unsigned int                     fb_size;
  unsigned int                     sn;
  unsigned int                     rtp_ts;
  rohc_feedback_option_mask_e_type received_options;
} rohc_feedback_log_packet_s_type;

/*---------------------------------------------------------------------------
  Log packet for ROHC Compressor Statistics
  Log Code:  LOG_ROHC_COMPRESSOR_STATS_C
---------------------------------------------------------------------------*/
typedef struct {
  log_hdr_type  hdr;
  comp_stats_t  comp_stat;
} rohc_comp_stat_log_packet_s_type;

/*---------------------------------------------------------------------------
  Log packet for ROHC Decompressor Statistics
  Log Code:  LOG_ROHC_DECOMPRESSOR_STATS_C
---------------------------------------------------------------------------*/
typedef struct {
  log_hdr_type    hdr;
  decomp_stats_t  decomp_stat;
} rohc_decomp_stat_log_packet_s_type;

#endif /* ROHC_LOGGING_H */

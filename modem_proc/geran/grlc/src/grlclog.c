/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RLC Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging RLC packets.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlclog.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/02   ADG     Initial Revision.
04/07/13    hv     CR461047 - Make safe usage of memcpy() and memmove() in GRLC
===========================================================================*/
#include <stringl.h>

#include "geran_variation.h"
#include "customer.h"        /* Include before using any FEATUREs */


/* Debug F3 messages will be emitted if the following line is uncommented */
/* #define RLC_LOG_DEBUG */

#if defined(RLC_LOG_DEBUG)
#error code not present
#else
#define DEBUGMSG(a,b,c,d)       /* */
#endif /* defined(RLC_LOG_DEBUG) */


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <string.h>          /* For memcpy()                               */

#include "comdef.h"          /* For log_commit(), log_alloc(), log codes   */
#include "log_codes_gsm.h"
#include "log_codes_gsm_v.h"   /* For GSM/GPRS log codes                     */
#ifdef DEBUG_USE_OLD_DIAG
#error code not present
#else
#include "gsmdiag.h"
#include "gsmdiag_v.h"
#endif
#include "grlcultypes.h"
#include "grlcdiag.h"        /* Interface to the rest of RLC               */
#include "grlclog.h"
#include "grlculproc.h"
#include "grlcdli.h"
#include "grlcdl.h"
#include "geran_multi_sim.h"
#include "grlc_static_main.h"
#include "grlc_dyn_api.h"

#ifndef FEATURE_GERAN_REDUCED_DEBUG
/*===========================================================================
              TYPE DEFINITIONS FOR EVENTS & LOG PACKETS
===========================================================================*/

/*lint -esym(754, LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C)
  uint16 access_reject_cnt;
  uint16 arc_retry_cnt;
  uint16 arc_wo_retry_cnt;
  uint16 arc_sys_info_cnt;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_UL_EVENT_COUNTS_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_UL_EVENT_COUNTS_C)
  uint16 llc_event_cnt;
  uint16 mac_event_cnt;
  uint16 pl1_event_cnt;
  uint16 tmr_event_cnt;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_UL_STATS_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_UL_STATS_C)
  uint32 grlc_ul_state;
  uint32 grlc_ul_substate;
  uint32 ack_tx_pdu_cnt;
  uint32 ack_tx_oct_cnt;
  uint32 unack_tx_pdu_cnt;
  uint32 unack_tx_oct_cnt;
  uint16 coding_sch_changes;
  uint8  ul_seq_va;
  uint8  ul_seq_vs;
  uint8  ul_seq_nseg;
  uint8  ul_seq_vsf;
  uint8  ul_seq_pack_vsf;
  uint8  ul_seq_nack_vsf;
  int8   ul_n3102;
  int8   ul_n3104;
  uint32 ul_gen_purpose_count_a;
  uint32 ul_gen_purpose_count_b;
  uint32 ul_gen_purpose_count_c;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_DL_STATS_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_DL_STATS_C)
  uint32 grlc_dl_state;
  uint32 ack_rx_pdu_cnt;
  uint32 ack_rx_oct_cnt;
  uint32 unack_rx_pdu_cnt;
  uint32 unack_pdu_filled_cnt;
  uint32 unack_rx_oct_cnt;
  uint32 unack_oct_filled_cnt;
  uint32 ack_nack_req_cnt;
  uint8  dl_seq_vq;
  uint8  dl_seq_vr;
  uint8  dl_seq_ssn;
  uint32 dl_gen_purpose_count_a;
  uint32 dl_gen_purpose_count_b;
  uint32 dl_gen_purpose_count_c;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C)
  uint8  ul_tfi;
  uint8  fai;   /* Final Ack Indicator */
  uint8  ssn;   /* Starting Sequence Number */
  uint8  cs;    /* Coding Scheme, 0=CS1, 1=CS2.. */
  uint8  cv;    /* Countdown Value */
  uint8  va;    /* ACK State Var */
  uint8  vs;    /* Send State Var */
  uint8  si;    /* Stall Indicator */
  uint32 hu32; /* Higher 32 bits of RBB field */
  uint32 lu32; /* Lower 32 bits of RBB field */
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C)
  uint8  dl_tfi;
  uint8  fai;   /* Final Ack Indicator */
  uint8  ssn;   /* Starting Sequence Number  = V(r) Receive State Var*/
  uint8  vq;    /* Receive Window Var */
  uint32 hu32;  /* Higher 32 bits of RBB field */
  uint32 lu32;  /* Lower 32 bits of RBB field */
  uint8  cs;    /* Coding Scheme, 0=CS1, 1=CS2.. */
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_UL_RELEASE_IND_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_UL_RELEASE_IND_C)
  uint8 ul_tfi;
  uint8 release_cause;
LOG_RECORD_END

/*lint -esym(754, LOG_GPRS_RLC_DL_RELEASE_IND_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_GPRS_RLC_DL_RELEASE_IND_C)
  uint8 dl_tfi;
  uint8 release_cause;
LOG_RECORD_END

/*lint -esym(754, LOG_EGPRS_RLC_DL_HEADER_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_EGPRS_RLC_DL_HEADER_C)
    byte  type;
    byte  dl_hdr[5];
    byte      crc_status; /* Bit0:CRC_1, Bit1:CRC_2, Bit2:E_1,
                          ** Bit3:FBI_1, Bit4:E_2, Bit5:FBI_2
                          */
    byte   time_slot;
    uint32 frame_number;
    byte      dl_payload[4*LOG_MAX_LI_E];
LOG_RECORD_END

/*lint -esym(754, LOG_EGPRS_RLC_UL_HEADER_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_EGPRS_RLC_UL_HEADER_C)
    byte  type;
    byte  ul_hdr[6];
LOG_RECORD_END


/*lint -esym(754, LOG_EGPRS_RLC_EPUAN_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_EGPRS_RLC_EPUAN_C)
      byte      tfi;           /* Temporary Flow Indicator  [0..31]                */
      byte      mcs;           /* Commanded Mod. & Coding Scheme  [0..8]           */
      byte      mask;          /* | d | d | scc | pre | res | eow | bow | fai |    */
                               /*   8   7    6     5     4     3     2     1       */
                                      /* scc - Starting Colour Code [0/1]
                                      ** pre - preemptive tx. [0/1]
                                      ** res - Resegment [1-IR OFF , 0- IR ON]
                                      ** bow - Begin Window of EPUAN
                                      ** eow - End Window of EPUAN
                                      ** fai  - Final Ack Indicator
                                      */
      word      ssn;           /* Starting Sequence Number [0..2047]                */
      byte      crbb_len;      /* Compressed Bitmap Length [Max size 128]           */
      uint32    crbb[4];       /* crbb array holding crbb_len bits                  */
      byte      urbb_len;      /* Uncompressed Bitmap Length [Max Size : 160 bits   */
      uint32    urbb[5];       /* urbb array holding urbb_len bits                  */
LOG_RECORD_END

/*lint -esym(754, LOG_EGPRS_RLC_EPDAN_C_tag::hdr)*/
LOG_RECORD_DEFINE(LOG_EGPRS_RLC_EPDAN_C)
      byte      tfi;        /* Temporary Flow Indicator  [0..31]                     */
      byte      mask;       /* | 8PSK | GMSK | scc | esp2 |esp1  | eow | bow | fai | */
                            /*    8      7      6     5     4       3     2     1    */
                            /* 8PSK: 1: psk_bep is vaild, 0: psk_bpe not valid.
                            ** GMSK: 1: gmsk_bep is valid, 0: gmsk_bep is not valid.
                            ** scc - Starting Colour Code [0/1]
                            ** esp2:esp1 - ES/P
                            **   0      0    -  N/A
                            **   0      1    - First Partial Bitmap
                            **   1      0    - Next Partial Bitmap
                            **   1      1    - Next Partial Bitmap w/ Quality Report
                            ** bow - Begin Window of EPUAN
                            ** eow - End Window of EPUAN
                            ** fai  - Final Ack Indicator
                            */
      word      ssn;        /* Starting Sequence Number [0..2047]                    */
      byte      crbb_len;   /* Compressed Bitmap Length [Max size 128]               */
      uint32    crbb[4];    /* crbb array holding crbb_len bits                      */
      byte      urbb_len;   /* Uncompressed Bitmap Length [Max Size : 160 bits       */
      uint32    urbb[5];    /* urbb array holding urbb_len bits                      */
      byte      gmsk_bep;   /* Serving cell GMSK BEP   mean_BEP = gmsk_bep & 0x1F;
                            ** cv_BEP = gmsk_bep>>5;
                            */
      byte      psk_bep;    /* Serving cell 8PSK BEP    mean_BEP = psk_bep & 0x1F;
                            ** cv_BEP = psk_bep>>5;
                            */
      byte      c_value;    /* useful parameter and make the log packet even number
                            ** bytes.
                            */
LOG_RECORD_END

/*===========================================================================
                         FUNCTION DECLARATIONS
===========================================================================*/
/* Log-on-demand callback to be registered with DIAG */

/* Log-packet-request callback to be registered with DIAG */
static boolean rlc_log_ul_abnrml_rls_counts(  boolean zero_stats);
static boolean rlc_log_ul_event_counts(  boolean zero_stats);

/*===========================================================================

FUNCTION  rlc_log_packet_init

DESCRIPTION
  Initialises a gsmdiag_pkt_info structure ready for use with logging functions

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void rlc_log_packet_init
(
  gsmdiag_pkt_info_t *info_ptr,
  log_code_type       pkt_id,
  unsigned int        pkt_size
)
{
  if (info_ptr != NULL)
  {
    info_ptr->pkt_id   = pkt_id;
    info_ptr->pkt_size = pkt_size;
  }

  return;
}

/*===========================================================================

FUNCTION  rlc_log_init()

DESCRIPTION
  This function initializes the log_on_demand packets.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_init(void)
{
  if (rlc_log_initialized)
  {
      DEBUGMSG( "rlc_log_init()> already initialised", 0,0,0 );
      return;
  }
  else
  {
      /* Call this only once */
      rlc_log_initialized = TRUE;

      DEBUGMSG( "rlc_log_init register log-on-demand", 0,0,0 );
      (void)log_on_demand_register(LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C,
                                    rlc_log_on_demand);
      (void)log_on_demand_register(LOG_GPRS_RLC_UL_EVENT_COUNTS_C,
                                    rlc_log_on_demand);
      (void)log_on_demand_register(LOG_GPRS_RLC_UL_STATS_C,
                                    rlc_log_on_demand);
      (void)log_on_demand_register(LOG_GPRS_RLC_DL_STATS_C,
                                    rlc_log_on_demand);

      DEBUGMSG( "rlc_log_init register log-packet-request", 0,0,0 );
      (void)diagpkt_gsm_gprs_lpr_register(LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C,
                                          rlc_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(LOG_GPRS_RLC_UL_EVENT_COUNTS_C,
                                          rlc_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(LOG_GPRS_RLC_UL_STATS_C,
                                          rlc_log_packet_request);
      (void)diagpkt_gsm_gprs_lpr_register(LOG_GPRS_RLC_DL_STATS_C,
                                          rlc_log_packet_request);

      DEBUGMSG( "rlc_log_init finished", 0,0,0 );
  }
}


/*===========================================================================

FUNCTION  rlc_log_on_demand

DESCRIPTION
  This function calls relevant log packet routine when the DM demands it.

RETURN VALUE
  Status of the call (should always be good)

SIDE EFFECTS
  None

===========================================================================*/
log_on_demand_status_enum_type rlc_log_on_demand_dyn(log_code_type log_code)
{
  /* HV- temporarily default to using ..ID_1, until Diag Team provides new API
  */
  rlc_log_packet_request_dyn(log_code, FALSE, 0);

  return LOG_ON_DEMAND_SENT_S;

} /* rlc_log_on_demand */


/*===========================================================================

FUNCTION  rlc_log_packet_request

DESCRIPTION
  This function calls relevant log packet routine when the DM demands it.
  log-packet-request works like an extended version log-on-demand,
  with extra parameters to clear statistics and to supply additional
  information according to packet type.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rlc_log_packet_request_dyn(
                            uint16 log_code,
                            uint8  zero_stats,
                            uint8  additional_info)
{
  /* Default to ID_1 until the Diag team provides the new API
  */

  NOTUSED(additional_info);

  switch (log_code)
  {
    case LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C:
      (void)rlc_log_ul_abnrml_rls_counts( zero_stats);
      break;

    case LOG_GPRS_RLC_UL_EVENT_COUNTS_C:
      (void)rlc_log_ul_event_counts( zero_stats);
      break;

    case LOG_GPRS_RLC_UL_STATS_C:
      rlc_log_ul_statistics(zero_stats);
      break;

    case LOG_GPRS_RLC_DL_STATS_C:
      rlc_log_dl_statistics(zero_stats);
      break;

    default:
        /* Prevent Lint warning about no default */
        break;
  } /* end switch */
} /* rlc_log_packet_request*/

/*===========================================================================

FUNCTION  rlc_log_ul_abnrml_rls_counts

DESCRIPTION
  This function populates and sends a Uplink Abnormal Release Counts
  log packet, optionally clearing the counts in the process.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static boolean rlc_log_ul_abnrml_rls_counts(  boolean zero_stats)
{
    const grlc_diag_ul_abnormal_release_counts_t *rpt_ptr;  /* data source */
    gsmdiag_pkt_info_t                           pkt_info;
    LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C_type     *log_ptr;  /* log buffer */

    /* Ensure logging is initialised */
    rlc_log_init();

    rlc_log_packet_init( &pkt_info, LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C,
                                    sizeof(LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C_type));

    /* Clear the counts if required */
    if (zero_stats)
    {
        DEBUGMSG( "rlc_log_ul_abnrml_rls_counts> clear counts", 0,0,0 );
        grlc_diag_clr_ul_abnormal_release_counts();
    }

    /* Obtain the current counts */
    rpt_ptr = grlc_diag_get_ul_abnormal_release_counts();
    if (rpt_ptr == NULL)
    {
        DEBUGMSG( "rlc_log_ul_abnrml_rls_counts> failed to get counts", 0,0,0 );
        return FALSE;
    }

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_UL_ABNRML_RLS_COUNTS_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr == NULL)
    {
        DEBUGMSG( "rlc_log_ul_abnrml_rls_counts> log_alloc failed", 0,0,0 );
    }
    else
    {
        /* Fill packet and commit it */
        log_ptr->access_reject_cnt = rpt_ptr->access_reject_cnt;
        log_ptr->arc_retry_cnt     = rpt_ptr->arc_retry_cnt;
        log_ptr->arc_wo_retry_cnt  = rpt_ptr->arc_wo_retry_cnt;
        log_ptr->arc_sys_info_cnt  = rpt_ptr->arc_sys_info_cnt;

        gsmdiag_log_commit(&pkt_info);
        DEBUGMSG( "rlc_log_ul_abnrml_rls_counts> packet logged", 0,0,0 );
    }

    return (log_ptr != NULL);
} /* rlc_log_ul_abnrml_rls_counts */



/*===========================================================================

FUNCTION  rlc_log_ul_event_counts

DESCRIPTION
  This function populates and sends a Uplink Event Counts
  log packet, optionally clearing the counts in the process.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean rlc_log_ul_event_counts(  boolean zero_stats)
{
    const grlc_diag_ul_event_counts_t   *rpt_ptr;           /* data source */
    gsmdiag_pkt_info_t                  pkt_info;
    LOG_GPRS_RLC_UL_EVENT_COUNTS_C_type *log_ptr;           /* log buffer */

    /* Ensure logging is initialised */
    rlc_log_init();

    rlc_log_packet_init( &pkt_info, LOG_GPRS_RLC_UL_EVENT_COUNTS_C,
                                    sizeof(LOG_GPRS_RLC_UL_EVENT_COUNTS_C_type));

    /* Clear the counts if required */
    if (zero_stats)
    {
        DEBUGMSG( "rlc_log_ul_event_counts> clear counts", 0,0,0 );
        grlc_diag_clr_ul_event_counts();
    }

    /* Obtain the current counts */
    rpt_ptr = grlc_diag_get_ul_event_counts();
    if (rpt_ptr == NULL)
    {
        DEBUGMSG( "rlc_log_ul_event_counts> failed to get counts", 0,0,0 );
        return FALSE;
    }

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_UL_EVENT_COUNTS_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr == NULL)
    {
        DEBUGMSG( "rlc_log_ul_event_counts> log_alloc failed", 0,0,0 );
    }
    else
    {
        /* Fill packet and commit it */
        log_ptr->llc_event_cnt = rpt_ptr->llc_event_cnt;
        log_ptr->mac_event_cnt = rpt_ptr->mac_event_cnt;
        log_ptr->pl1_event_cnt = rpt_ptr->pl1_event_cnt;
        log_ptr->tmr_event_cnt = rpt_ptr->tmr_event_cnt;

        gsmdiag_log_commit(&pkt_info);
        DEBUGMSG( "rlc_log_ul_event_counts> packet logged", 0,0,0 );
    }

    return (log_ptr != NULL);
} /* rlc_log_ul_event_counts */



/*===========================================================================

FUNCTION  _rlc_log_ul_statistics

DESCRIPTION
  This function populates and sends a Uplink Statistics
  log packet, optionally clearing the counts in the process.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void _rlc_log_ul_statistics(  boolean zero_stats)
{
    const grlc_diag_ul_statistics_t *rpt_ptr;               /* data source */
    gsmdiag_pkt_info_t              pkt_info;
    LOG_GPRS_RLC_UL_STATS_C_type    *log_ptr;               /* log buffer */

    /* Ensure logging is initialised */
    rlc_log_init();

    rlc_log_packet_init( &pkt_info, LOG_GPRS_RLC_UL_STATS_C,
                                    sizeof(LOG_GPRS_RLC_UL_STATS_C_type));

    /* Clear the counts if required */
    if (zero_stats)
    {
        DEBUGMSG( "_rlc_log_ul_statistics> clear counts", 0,0,0 );
        grlc_diag_clr_ul_statistics();
    }

    /* Obtain the current counts */
    rpt_ptr = grlc_diag_get_ul_statistics();
    if (rpt_ptr == NULL)
    {
        DEBUGMSG( "_rlc_log_ul_statistics> failed to get counts", 0,0,0 );
        return;
    }

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_UL_STATS_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr == NULL)
    {
        DEBUGMSG( "_rlc_log_ul_statistics> log_alloc failed", 0,0,0 );
    }
    else
    {
        /* Fill packet and commit it */
        log_ptr->grlc_ul_state          = rpt_ptr->grlc_ul_state;
        log_ptr->grlc_ul_substate       = rpt_ptr->grlc_ul_substate;
        log_ptr->ack_tx_pdu_cnt         = rpt_ptr->ack_tx_pdu_cnt;
        log_ptr->ack_tx_oct_cnt         = rpt_ptr->ack_tx_oct_cnt;
        log_ptr->unack_tx_pdu_cnt       = rpt_ptr->unack_tx_pdu_cnt;
        log_ptr->unack_tx_oct_cnt       = rpt_ptr->unack_tx_oct_cnt;
        log_ptr->coding_sch_changes     = rpt_ptr->coding_sch_changes;
        log_ptr->ul_seq_va              = rpt_ptr->ul_seq_va;
        log_ptr->ul_seq_vs              = rpt_ptr->ul_seq_vs;
        log_ptr->ul_seq_nseg            = rpt_ptr->ul_seq_nseg;
        log_ptr->ul_seq_vsf             = rpt_ptr->ul_seq_vsf;
        log_ptr->ul_seq_pack_vsf        = rpt_ptr->ul_seq_pack_vsf;
        log_ptr->ul_seq_nack_vsf        = rpt_ptr->ul_seq_nack_vsf;
        log_ptr->ul_n3102               = rpt_ptr->ul_n3102;
        log_ptr->ul_n3104               = rpt_ptr->ul_n3104;
        log_ptr->ul_gen_purpose_count_a = rpt_ptr->ul_gen_purpose_count_a;
        log_ptr->ul_gen_purpose_count_b = rpt_ptr->ul_gen_purpose_count_b;
        log_ptr->ul_gen_purpose_count_c = rpt_ptr->ul_gen_purpose_count_c;

        gsmdiag_log_commit(&pkt_info);
        DEBUGMSG( "_rlc_log_ul_statistics> packet logged", 0,0,0 );
    }

    return;
} /* _rlc_log_ul_statistics */



/*===========================================================================

FUNCTION  _rlc_log_dl_statistics

DESCRIPTION
  This function populates and sends a Downlink Statistics
  log packet, optionally clearing the counts in the process.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void _rlc_log_dl_statistics(boolean zero_stats)
{
    const grlc_diag_dl_statistics_t *rpt_ptr;               /* data source */
    gsmdiag_pkt_info_t              pkt_info;
    LOG_GPRS_RLC_DL_STATS_C_type    *log_ptr;               /* log buffer */

    /* Ensure logging is initialised */
    rlc_log_init();

    rlc_log_packet_init( &pkt_info, LOG_GPRS_RLC_DL_STATS_C,
                                    sizeof(LOG_GPRS_RLC_DL_STATS_C_type));

    /* Clear the counts if required */
    if (zero_stats)
    {
        DEBUGMSG( "_rlc_log_dl_statistics> clear counts", 0,0,0 );
        grlc_diag_clr_dl_statistics();
    }

    /* Obtain the current counts */
    rpt_ptr = grlc_diag_get_dl_statistics();
    if (rpt_ptr == NULL)
    {
        DEBUGMSG( "_rlc_log_dl_statistics> failed to get counts", 0,0,0 );
        return;
    }

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_DL_STATS_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr == NULL)
    {
        DEBUGMSG( "_rlc_log_dl_statistics> log_alloc failed", 0,0,0 );
    }
    else
    {
        /* Fill packet and commit it */
        log_ptr->grlc_dl_state          = rpt_ptr->grlc_dl_state;
        log_ptr->ack_rx_pdu_cnt         = rpt_ptr->ack_rx_pdu_cnt;
        log_ptr->ack_rx_oct_cnt         = rpt_ptr->ack_rx_oct_cnt;
        log_ptr->unack_rx_pdu_cnt       = rpt_ptr->unack_rx_pdu_cnt;
        log_ptr->unack_pdu_filled_cnt   = rpt_ptr->unack_pdu_filled_cnt;
        log_ptr->unack_rx_oct_cnt       = rpt_ptr->unack_rx_oct_cnt;
        log_ptr->unack_oct_filled_cnt   = rpt_ptr->unack_oct_filled_cnt;
        log_ptr->ack_nack_req_cnt       = rpt_ptr->ack_nack_req_cnt;
        log_ptr->dl_seq_vq              = rpt_ptr->dl_seq_vq;
        log_ptr->dl_seq_vr              = rpt_ptr->dl_seq_vr;
        log_ptr->dl_seq_ssn             = rpt_ptr->dl_seq_ssn;
        log_ptr->dl_gen_purpose_count_a = rpt_ptr->dl_gen_purpose_count_a;
        log_ptr->dl_gen_purpose_count_b = rpt_ptr->dl_gen_purpose_count_b;
        log_ptr->dl_gen_purpose_count_c = rpt_ptr->dl_gen_purpose_count_c;

        gsmdiag_log_commit(&pkt_info);
        DEBUGMSG( "_rlc_log_dl_statistics> packet logged", 0,0,0 );
    }

    return;
} /* _rlc_log_dl_statistics */


/*===========================================================================

FUNCTION  _rlc_log_ul_acknack_params

DESCRIPTION
  This function populates and sends a uplink acknack description parameters .

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void _rlc_log_ul_acknack_params(void)
{
    uint16 u;

    gsmdiag_pkt_info_t                            pkt_info;
    /* log buffer */
    LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C_type    *log_ptr;

    rlc_log_packet_init( &pkt_info, LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C,
                                    sizeof(LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C_type));

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */

    log_ptr = (LOG_GPRS_RLC_UL_ACKNACK_PARAMS_VER2_C_type *)gsmdiag_log_alloc(&pkt_info);
    if (log_ptr != NULL)
    {
      log_ptr->lu32 = 0L;
      log_ptr->hu32 = 0L;
      log_ptr->ul_tfi = (uint8)ul.cur_res.ul_tfi;
      log_ptr->ssn    = ul.ul_acknack.ssn;
      log_ptr->fai    = ul.ul_acknack.fai;
      log_ptr->cs     = ul.cur_res.cs;
      log_ptr->cv     = ul.cv;
      log_ptr->va     = (uint8)ul.va;
      log_ptr->vs     = (uint8)ul.vs;
      log_ptr->si     = ul.si;

      for (u=0; u<4; u++)
      {
        /* RBB[4..7] into the lower 32 bits
        */
        log_ptr->lu32 |= (ul.ul_acknack.rbb[u+4] << (8*((4-u)-1)) );

        /* RBB[0..3] into the higher 32 bits
        */
        log_ptr->hu32 |= (ul.ul_acknack.rbb[u] << (8*((4-u)-1)) );
      }
      gsmdiag_log_commit(&pkt_info);
    }
} /* _rlc_log_ul_acknack_params */


/*===========================================================================

FUNCTION  _rlc_log_dl_acknack_params

DESCRIPTION
  This function populates and sends a downlink acknack description parameters .

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void _rlc_log_dl_acknack_params
(
 uint8  dl_tfi,
 uint8  fai,
 uint8  ssn,
 uint8  vq,
 uint32 hu32,
 uint32 lu32,
 uint8  cs
)
{
    gsmdiag_pkt_info_t                          pkt_info;
    LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C_type  *log_ptr;       /* log buffer */

    rlc_log_packet_init( &pkt_info, LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C,
                                    sizeof(LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C_type));

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_DL_ACKNACK_PARAMS_VER2_C_type    *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr != NULL)
    {
      log_ptr->dl_tfi = dl_tfi;
      log_ptr->fai    = fai;
      log_ptr->ssn    = ssn;
      log_ptr->vq     = vq;
      log_ptr->hu32   = hu32;
      log_ptr->lu32   = lu32;
      log_ptr->cs    = cs;

      gsmdiag_log_commit(&pkt_info);
    }
} /* _rlc_log_dl_acknack_params */


/*===========================================================================

FUNCTION  _rlc_log_ul_release_ind

DESCRIPTION
  This function populates and sends uplink release cause.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void _rlc_log_ul_release_ind
(
 uint8                 ul_tfi,
 rlc_log_ul_release_t  release_cause
)
{
    gsmdiag_pkt_info_t                  pkt_info;
    LOG_GPRS_RLC_UL_RELEASE_IND_C_type  *log_ptr;               /* log buffer */

    rlc_log_packet_init( &pkt_info, LOG_GPRS_RLC_UL_RELEASE_IND_C,
                                    sizeof(LOG_GPRS_RLC_UL_RELEASE_IND_C_type));

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_UL_RELEASE_IND_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr != NULL)
    {
      log_ptr->ul_tfi        = ul_tfi;
      log_ptr->release_cause = (byte)release_cause;

      gsmdiag_log_commit(&pkt_info);
    }
} /* _rlc_log_ul_release_ind */


/*===========================================================================

FUNCTION  _rlc_log_dl_release_ind

DESCRIPTION
  This function populates and sends downlink release cause.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void _rlc_log_dl_release_ind
(
 uint8  dl_tfi,
 rlc_log_dl_release_t release_cause
)
{
    gsmdiag_pkt_info_t                  pkt_info;
    LOG_GPRS_RLC_DL_RELEASE_IND_C_type  *log_ptr;               /* log buffer */

    rlc_log_packet_init( &pkt_info, LOG_GPRS_RLC_DL_RELEASE_IND_C,
                                    sizeof(LOG_GPRS_RLC_DL_RELEASE_IND_C_type));

    /* Try to allocate memory for the log packet -             */
    /* this will fail if the log mask is not set appropriately */
    log_ptr = (LOG_GPRS_RLC_DL_RELEASE_IND_C_type *)gsmdiag_log_alloc(&pkt_info);

    if (log_ptr != NULL)
    {
      log_ptr->dl_tfi           = dl_tfi;
      log_ptr->release_cause    = (byte)release_cause;

      gsmdiag_log_commit(&pkt_info);
    }
} /* _rlc_log_dl_release_ind */

/*===========================================================================
===
===  FUNCTION      rlc_log_rfblks_cnts()
===
===  DESCRIPTION
===
===    RLC function to fill and return the Uplink Statistics Info
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
rlc_log_rfblks_cnts_t* rlc_log_rfblks_cnts_dyn(void)
{
  static rlc_log_rfblks_cnts_t  temp_st;

  temp_st.dl_rx   = dl.rx_blk_cnt;
  temp_st.ul_tx   = ul.ptx_cnt;
  temp_st.ul_retx = ul.nack_cnt + ul.pack_cnt;

  return &temp_st;
}

/*===========================================================================
===
===  FUNCTION      rlc_log_dl_blks_cnts()
===
===  DESCRIPTION
===
===    RLC function to fill and return the Downlink Statistics Info
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
rlc_eng_mode_dl_cnts_t* rlc_log_dl_blks_cnts_dyn(void)
{
  static rlc_eng_mode_dl_cnts_t  temp_st;

  temp_st.dl_rx   = dl.eng_mode_rcvd_dl_block_cnt + dl.eng_mode_rcvd_dup_dl_block_cnt;
  temp_st.dl_reprx  = dl.eng_mode_rcvd_dup_dl_block_cnt;
  return &temp_st;
}


/*===========================================================================

FUNCTION  _rlc_log_dl_header

DESCRIPTION
  EGPRS 3.3 0x5206  EGPRS Downlink Header
  This function should be called when RLC has got a data block of DL TBF.
  For GPRS type = 0, For EGPRS type value is defined as spec says.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void _rlc_log_dl_header
(
 byte        type,
 const byte* dl_header,
 byte        crc_status,
 byte        time_slot,
 uint32      frame_number,
 const byte* dl_payload
)
{
  gsmdiag_pkt_info_t              pkt_info;
  LOG_EGPRS_RLC_DL_HEADER_C_type *log_ptr;    /* pointer to log buffer */

  rlc_log_packet_init( &pkt_info, LOG_EGPRS_RLC_DL_HEADER_C,
                                  sizeof(LOG_EGPRS_RLC_DL_HEADER_C_type));

  log_ptr = (LOG_EGPRS_RLC_DL_HEADER_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->type = type;

    /* Copy 5 Bytes of DL Header */
    (void)memscpy((void*)log_ptr->dl_hdr, sizeof(log_ptr->dl_hdr), (void*)dl_header, 5);

    log_ptr->crc_status  =  crc_status;
    log_ptr->time_slot     =  time_slot;
    log_ptr->frame_number  =  frame_number;

    /* Copy 4*LOG_MAX_LI_E = 12 bytes of DL Payload if Header Type-1, otherwise
       copy 2*LOG_MAX_LI_E = 6 bytes*/
    (void)memscpy((void*)log_ptr->dl_payload,
                  sizeof(log_ptr->dl_payload),
                 (void*)dl_payload,
                 (((type ==(byte) EGPRS_HEADER_TYPE_1)? 4 : 2)*LOG_MAX_LI_E));

    gsmdiag_log_commit(&pkt_info);
  }
} /* _rlc_log_dl_header */


/*===========================================================================

FUNCTION  _rlc_log_ul_header

DESCRIPTION
  EGPRS 3.4 0x5205  EGPRS Uplink Header
  This function should be called when RLC has got a data block of UL TBF.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void _rlc_log_ul_header
(
 byte        type,
 const byte* ul_header
)
{
  gsmdiag_pkt_info_t              pkt_info;
  LOG_EGPRS_RLC_UL_HEADER_C_type *log_ptr;    /* pointer to log buffer */

  rlc_log_packet_init( &pkt_info, LOG_EGPRS_RLC_UL_HEADER_C,
                                  sizeof(LOG_EGPRS_RLC_UL_HEADER_C_type));

  log_ptr = (LOG_EGPRS_RLC_UL_HEADER_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    log_ptr->type = type;

    /* Copy 6 Bytes of DL Header */
    (void)memscpy((void*)log_ptr->ul_hdr, sizeof(log_ptr->ul_hdr), (void*)ul_header, 6);

    gsmdiag_log_commit(&pkt_info);
  }

} /* _rlc_log_ul_header */


/*===========================================================================

FUNCTION  _rlc_log_epuan

DESCRIPTION
  EGPRS RLC UL ACK/NACK (0x5209)
  This function should be called when RLC has got it from MAC.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void _rlc_log_epuan (const mr_ul_pkt_acknack_ind_t* puan_msg)
{

  gsmdiag_pkt_info_t         pkt_info;
  LOG_EGPRS_RLC_EPUAN_C_type *log_ptr;

  rlc_log_packet_init( &pkt_info, LOG_EGPRS_RLC_EPUAN_C,
                                  sizeof(LOG_EGPRS_RLC_EPUAN_C_type));

  log_ptr = (LOG_EGPRS_RLC_EPUAN_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    /* Initialize mask */
    log_ptr->mask    = 0;

    log_ptr->tfi      = puan_msg->tfi;
    log_ptr->mcs      = puan_msg->mcs;

    /* log_ptr->mask
    ** | d | d | scc | pre | res | eow | bow | fai |
    **   8   7    6     5     4     3     2     1
    */
    log_ptr->mask     = (uint8)(  ( (byte)puan_msg->egprs_acknack_desc.fai       )  |
                                  ( ((byte)puan_msg->egprs_acknack_desc.bow) << 1)  |
                                  ( ((byte)puan_msg->egprs_acknack_desc.eow) << 2)  |
                                  ( ((byte)puan_msg->resegment) << 3             )  |
                                  ( ((byte)puan_msg->pre_emptive_tx) << 4        )  |
                                  ( ((byte)puan_msg->egprs_acknack_desc.scc) << 5)
                               );

    log_ptr->ssn      = puan_msg->egprs_acknack_desc.ssn;

    if (puan_msg->egprs_acknack_desc.comp_present)
    {
       log_ptr->crbb_len = puan_msg->egprs_acknack_desc.crbb_len;
       (void)memscpy((void*)log_ptr->crbb, sizeof(log_ptr->crbb), (void*)puan_msg->egprs_acknack_desc.crbb, 16);
    }
    else
    {
       log_ptr->crbb_len = 0;
    }

    log_ptr->urbb_len = puan_msg->egprs_acknack_desc.urbb_len;
    if (log_ptr->urbb_len)
    {
      (void)memscpy((void*)log_ptr->urbb, sizeof(log_ptr->urbb), (void*)puan_msg->egprs_acknack_desc.urbb, 20);
    }

    gsmdiag_log_commit(&pkt_info);
  }
} /* _rlc_log_epuan */

/*===========================================================================

FUNCTION  _rlc_log_epdan

DESCRIPTION
  EGPRS RLC UL ACK/NACK (0x5211)
  This function should be called when RLC has got it from MAC.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void _rlc_log_epdan (const rm_dl_pkt_acknack_req_t* pdan_msg)
{
  gsmdiag_pkt_info_t         pkt_info;
  LOG_EGPRS_RLC_EPDAN_C_type *log_ptr;

  rlc_log_packet_init( &pkt_info, LOG_EGPRS_RLC_EPDAN_C,
                                  sizeof(LOG_EGPRS_RLC_EPDAN_C_type));

  log_ptr = (LOG_EGPRS_RLC_EPDAN_C_type *)gsmdiag_log_alloc(&pkt_info);

  if (log_ptr != NULL)
  {
    /* Initialize mask */
    log_ptr->mask    = 0;

    log_ptr->tfi     = pdan_msg->tfi;

    /* log_ptr->mask
    ** log_ptr->mask
    ** | 8PSK | GMSK | scc | esp2 |esp1  | eow | bow | fai |
    **    8      7      6     5     4       3     2     1
    */
    log_ptr->ssn      = pdan_msg->e_acknack_desc.ssn;

    if (pdan_msg->e_acknack_desc.comp_present)
    {
       log_ptr->crbb_len = pdan_msg->e_acknack_desc.crbb_len;
       (void)memscpy((void*)log_ptr->crbb, sizeof(log_ptr->crbb), (void*)pdan_msg->e_acknack_desc.crbb, 16);

       /* Store Starting Colour Code into MASK */
       log_ptr->mask |= (pdan_msg->e_acknack_desc.scc & 0x01) << 5;

    }
    else
    {
       log_ptr->crbb_len = 0;
    }

    log_ptr->urbb_len = pdan_msg->e_acknack_desc.urbb_len;
    if (log_ptr->urbb_len)
    {
      (void)memscpy((void*)log_ptr->urbb, sizeof(log_ptr->urbb), (void*)pdan_msg->e_acknack_desc.urbb, 20);
    }

    /* Store other fields into MASK */
    log_ptr->mask   |= (pdan_msg->esp                & 0x03) << 3;
    log_ptr->mask   |= (pdan_msg->e_acknack_desc.eow & 0x01) << 2;
    log_ptr->mask   |= (pdan_msg->e_acknack_desc.bow & 0x01) << 1;
    log_ptr->mask   |= (pdan_msg->e_acknack_desc.fai & 0x01) << 0;


    /* get the BEP and C from channel quality report*/
    if( pdan_msg->pl1_qrep_ptr != NULL)
    {
      log_ptr->gmsk_bep = 0;
      log_ptr->psk_bep  = 0;

      log_ptr->mask    |= (pdan_msg->pl1_qrep_ptr->gmsk_mean_bep_present & 0x01) << 6;
      log_ptr->mask    |= (pdan_msg->pl1_qrep_ptr->psk_mean_bep_present  & 0x01) << 7;


      log_ptr->c_value = pdan_msg->pl1_qrep_ptr->c_value;

      log_ptr->gmsk_bep |= (pdan_msg->pl1_qrep_ptr->gmsk_cv_bep & 0x07) << 5;
      log_ptr->gmsk_bep |= (pdan_msg->pl1_qrep_ptr->gmsk_m_bep  & 0x1F) << 0;

      log_ptr->psk_bep  |= (pdan_msg->pl1_qrep_ptr->psk_cv_bep & 0x07)  << 5;
      log_ptr->psk_bep  |= (pdan_msg->pl1_qrep_ptr->psk_m_bep  & 0x1F)  << 0;

    }
    else
    {
      log_ptr->mask    &= 0x3F;      /* set both GMSK and 8PSK BEP Flag invalid */
      log_ptr->c_value = 0xFF;       /* 0xFF indicating C value is out of range.
                                     ** Not 0-63
                                     */
    }

    gsmdiag_log_commit(&pkt_info);
  }
} /* _rlc_log_epdan */
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */



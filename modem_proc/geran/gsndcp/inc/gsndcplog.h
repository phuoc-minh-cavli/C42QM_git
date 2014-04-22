#ifndef SNDCP_LOG_H
#define SNDCP_LOG_H

#include "geran_variation.h"
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           SNDCP Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging SNDCP packets.

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gsndcp/inc/gsndcplog.h#2 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
09/13/02   ADG     Initial Revision.
===========================================================================*/

#include "comdef.h"    /* for types */
#include "gsni.h"
#include "dsm.h"
#include "log.h"       /* For log_commit(), log_alloc(), log codes   */

#ifndef FEATURE_GERAN_REDUCED_DEBUG
/* INITIALISATION */
extern void sndcp_log_init(void);
/* Log-on-demand callback to be registered with DIAG */
extern log_on_demand_status_enum_type sndcp_log_on_demand(log_code_type log_code);
/* Log-packet-request callback to be registered with DIAG */
extern void sndcp_log_packet_request(uint16 log_code, uint8 zero_stats, uint8 additional_info);


/* LOG PACKETS */
extern void sndcp_log_active_nsapis(void);
extern void sndcp_log_nsapi_pdp_context(uint8 nsapi);
extern void sndcp_log_nsapi_state(uint8 nsapi);
extern void sndcp_log_pdu_stats(uint8 nsapi, uint8 zero_stats);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
extern void sndcp_log_dl_tcp_hdr(dsm_item_type *npdu); 
extern void sndcp_log_ul_tcp_hdr(dsm_item_type *npdu);

/* EVENTS */
/* extern void sndcp_log_some_event(args); */

#endif /* SNDCP_LOG_H */

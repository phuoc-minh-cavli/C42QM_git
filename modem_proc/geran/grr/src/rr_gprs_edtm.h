#ifndef RR_GPRS_EDTM_H
#define RR_GPRS_EDTM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_gprs_edtm.h

GENERAL DESCRIPTION
   This module contains functions for dealing with Enhanced DTM

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2006-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  
$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_gprs_edtm.h#2 $  

18/07/2006 SJW Initial submit
dd/mm/yyyy XXX Comment format

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_EDTM

#include "comdef.h"
#include "mac_grr.h"
#include "mm_rr.h"
#include "rr_events.h"
#include "rr_task.h"
#include "rr_l1.h"
#include "sys_type.h"
#include "rr_conn_release.h"
#include "geran_multi_sim.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/* define a special frame number that can be used to distinguish the  */
/* request references that can be "ignored" by RR. The max value of a */
/* frame number in this context is 42432, so use a larger figure here */
#define RR_EDTM_FN ((word)0xEDED)

/* define a special TC number used to flag DL_UNIT_DATA_IND from EDTM */
#define RR_EDTM_TC ((byte)0xED)

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
extern boolean    rr_gprs_edtm_supported( );
extern void       rr_gprs_edtm_process_pacch_sys_info(mac_grr_data_ind_t *  );
extern void       rr_gprs_edtm_complete_cs_establishment( );
extern rr_event_T rr_gprs_edtm_control(rr_event_T, void *  );
extern rr_event_T rr_gprs_edtm_process_pkt_cs_est_reject(mac_grr_edtm_pkt_cs_est_rej_t *  );
extern rr_event_T rr_gprs_edtm_process_pkt_cs_release(mac_grr_edtm_cs_release_ind_t * );
extern boolean    rr_gprs_edtm_prioritise_saved_msg(rr_cmd_bdy_type *  );
extern void       rr_gprs_edtm_store_channel_needed(channel_type_T );
extern boolean    rr_gprs_edtm_process_channel_release( );
extern boolean    rr_gprs_edtm_ps_release_required( );
extern void       rr_gprs_edtm_signal_cs_release( );
extern uint32     rr_gprs_edtm_retrieve_bsi_status_mask( );
extern uint16     rr_gprs_edtm_retrieve_psi_status_mask( );
extern void       rr_gprs_edtm_dual_transfer_started(boolean );
extern void       rr_gprs_edtm_dedicated_mode_started(rr_l1_message_id_enum_T  );
extern void       rr_gprs_edtm_cache_dtm_release(rr_cmd_bdy_type *);
extern boolean    rr_gprs_edtm_ps_allowed_after_cs_release(rr_conn_release_mode_t  );
extern void       rr_gprs_edtm_complete_cs_release( );
extern void       rr_gprs_edtm_gmac_uplink_pending(boolean uplink_pending  );
extern boolean    rr_gprs_edtm_enhanced_release_started( );
extern boolean    rr_gprs_edtm_cs_establishment_in_progress( );

#endif /* FEATURE_GSM_EDTM */

#endif

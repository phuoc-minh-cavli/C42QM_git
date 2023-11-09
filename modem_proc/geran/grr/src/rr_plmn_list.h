#ifndef RR_PLMN_LIST_H
#define RR_PLMN_LIST_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             rr_plmn_list.h

GENERAL DESCRIPTION
   This module contains functions for dealing with rr PLMN list construction.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_plmn_list.h#6 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  03/28/11    sjw     New DSC handler for BPLMN power scan in Dual SIM mode
  08/09/02    he      Initial Revision.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"
#include "rr_task.h"
#include "rr_candidate_cell_db.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  RR_INACTIVE_BEFORE_PLMN_SEARCH,   /* RR wasn't active RAT before PLMN search */
  RR_IN_SERVICE_BEFORE_PLMN_SEARCH, /* RR was active RAT and in service */
  RR_NO_SERVICE_BEFORE_PLMN_SEARCH  /* RR was active RAT but not in service */
}rr_status_before_plc_e;

typedef enum
{
  RR_PLC_FOREGROUND_SEARCH,
  RR_PLC_BACKGROUND_SEARCH,
#ifdef FEATURE_WCDMA
  RR_PLC_WTOG_SEARCH,
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
  RR_PLC_LTOG_SEARCH,
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
  RR_PLC_TTOG_SEARCH,
  RR_PLC_GTOT_SEARCH,
#endif /*FEATURE_GSM_TDS*/
  RR_PLC_SGLTE_X2G_FOREGROUND_SEARCH,
  RR_PLC_SGLTE_X2G_BACKGROUND_SEARCH,
  RR_PLC_SEARCH_TYPE_MAX
}rr_plc_search_type_e;

typedef enum
{
  RR_PLC_COMPLETE_GOTO_INACTIVE, /* go to inactive state */
  RR_PLC_COMPLETE_START_IDLE     /* go to camp state */
}rr_action_after_plc_e;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/**
  @brief Perform one-time initialisation of this module.

  This function should be called once, and only once, when the RR thread is
  started.
*/
extern void rr_plc_task_start_init(void);

extern boolean rr_plc_is_automatic_search( void );

extern boolean rr_plc_search_in_foreground( void );

extern boolean rr_plmn_list_srch_from_nas( void );

/* function used not only in PLMN search, but also in PLMN selection */
extern void rr_plc_construct_plmn_list(
  rr_available_plmn_db_T *plmn_db_ptr,
  sys_detailed_plmn_list_s_type *plmn_list_ptr,
  rr_forbidden_lai_check_option_e check_forbidden_lai
);

extern void rr_plc_init( void );

extern sys_rat_pri_list_info_s_type *rr_plc_get_rat_pri_list_info( void );

extern void rr_plc_setup(
  rr_plc_search_type_e search_type,
  uint32 timer_val
);

extern rr_event_T rr_plmn_list_control(
  rr_event_T plc_event,
  rr_cmd_bdy_type *new_message
);

extern rr_action_after_plc_e rr_plc_get_action_after_plmn_search_complete( void );

#if (defined FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
extern uint32 rr_plc_get_wtog_search_timer_value( void );
#endif /* FEATURE_WCDMA || FEATURE_LTE || defined (FEATURE_GSM_TDS)*/

extern void    rr_plc_set_ps_service_priority( boolean );

extern boolean rr_plc_get_ps_service_priority( void );

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
extern boolean rr_plc_wtog_decoding_bcch( void );
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
extern boolean rr_plc_bplmn_pscan_active( void );
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef FEATURE_LTE
extern boolean rr_plc_l2g_search_in_progress( void );
#endif /* FEATURE_LTE */

extern void rr_plc_set_band_filter(
  sys_band_mask_type band_mask
);

extern sys_band_mask_type rr_plc_get_band_filter( void );

#ifdef FEATURE_SGLTE
rr_event_T rr_plc_sglte_g2x_control(rr_event_T rr_event, rr_cmd_bdy_type *message_ptr );

boolean rr_is_sglte_plmn_srch_active( void );

boolean rr_x2g_bplmn_search_active( void );
#endif /* FEATURE_SGLTE */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
extern void rr_ms_set_plc_gsm_aborted( void );
#endif /* FEATURE_DUAL_SIM  || FEATURE_TRIPLE_SIM */

extern boolean rr_cell_search_in_progress(void);

extern boolean rr_gps_advance_search_in_progress( void );

#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH 
extern void rr_set_gps_plmn_search_scan_ref_time(uint64 scan_ref_time);
extern uint64 rr_get_gps_plmn_search_scan_ref_time(void);
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */

#endif /* RR_PLMN_LIST_H */

/* EOF */


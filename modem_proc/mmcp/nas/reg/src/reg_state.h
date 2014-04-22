#ifndef REG_STATE_H
#define REG_STATE_H
/*==============================================================================


                  R E G  -  S T A T E   H E A D E R   F I L E


                                  DESCRIPTION

This header file defines the types and functions required to implement the
REG finite state machine.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state.h_v   1.2   07 Jun 2002 14:23:32   kabraham  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/reg/src/reg_state.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/27/12   us      Adding prototype for new function. 
02/13/02   kwa     Initial release.
05/20/02   kwa     Added output_name function.
06/09/02   kwa     Added functions to get the service_status and active_rat
                   from the current service_state.
08/06/02   kwa     Externalized reg_state_update_sim_state.
09/19/02   kwa     Added function pointer to reg_state_s_type for processing
                   the cm_act_req message.
03/27/03   kwa     Added function reg_state_process_mmr_service_ind.
05/22/03   kwa     Modified functions to pass const pointers to eliminate
                   lint warnings.
06/04/03   kwa     Removed one remaining lint warning.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
10/22/03   kwa     Removed const from the msg_p parameter in the typedef
                   reg_state_mmr_reg_cnf_f_type.
11/19/03   kwa     Removed obsolete function for retrieving the state name.
01/12/04   kwa     Added prototype for reg_state_automatic_search_initiated.
01/22/04   kwa     Added prototype for reg_state_service_status_set.
05/17/04   kwa     Added function pointer to reg_state_s_type for processing
                   the mmr_camped_ind message.
08/17/04   kwa     Removed unnecessary function reg_state_update_sim_state.
12/01/04   up      Function 'reg_state_current_state_get' is added to get the
                   current state of REG.
01/24/05   up      Function 'reg_state_service_plmn_get' is added to get the
                   current serving plmn.
02/22/05   up      Updated for Cingular ENS feature. 
                   Added functions reg_state_process_sim_refresh_fcn_ind,
                   reg_state_unexpected_sim_refresh_fcn_ind,
                   reg_state_process_fcn_hplmn. Added the new function 
                   pointer type reg_state_sim_refresh_fcn_ind_f_type.
                   Updated reg_state_s_type to add the new function pointer.
09/19/05   up      Added reg_state_ignore_hplmn_search_request and updated
                   to support on demand HPLMN search from user.
11/07/05   ajt     Added function pointer to reg_state_s_type for receiving the
                   process_mmr_emergency_num_list_ind from MM.                  
01/05/06   sn      Added reg_state_prev_plmn_get and reg_state_roaming_ind_get
01/18/06   up      Added reg_state_plmn_search_transaction_id_get to get and
                   reg_state_plmn_search_transaction_id_set to generate transID.
04/06/06   sn      Added reg_state_process_cell_service_ind.
04/11/06   sn      Added reg_state_reg_service_domain_get
07/03/06   sn      Added reg_state_active_rat_set
12/15/09   abhi   Removing UOOS code. 
09/20/11   sla     Store GSTK sent PLMN 
09/20/11   sla    Store last gstk service status sent.
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity
3/26/13   c_mg    Ported CR-447611
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "reg_cm_v.h"
#include "reg_mm_v.h"
#include "reg_task.h"
#include "reg_task_int.h"
#ifdef FEATURE_CIOT
#include "emm_ext_msg.h"
#endif

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

typedef enum 
{
  REG_STATE_NONE = -1,                           /* FOR INTERNAL USE ONLY! */
  REG_STATE_INACTIVE,
  REG_STATE_IDLE,
  REG_STATE_REGISTERING,
  REG_STATE_DEREGISTERING,
  REG_STATE_MANUAL_SEARCH,
  REG_STATE_AUTOMATIC_SEARCH,
  REG_STATE_OFFLINE,
  REG_STATE_MAX                                  /* FOR INTERNAL USE ONLY! */
} reg_state_e_type;

typedef enum reg_mode_foreground_search_status_e
{
  REG_MODE_FOREGROUND_SEARCH_NONE = -1,             /* FOR INTERNAL USE ONLY! */
  REG_MODE_FOREGROUND_SEARCH_STARTED,
  REG_MODE_FOREGROUND_SEARCH_ATTEMPTED
                   /* FOR INTERNAL USE ONLY! */
} reg_mode_foreground_search_status_e_type;

#ifdef FEATURE_SGLTE
boolean reg_state_sglte_merge_in_progress;
boolean reg_state_sglte_split_in_progress;

#define IS_SGLTE_MERGE_IN_PROGRESS (REG_SUB_IS_SGLTE && reg_state_sglte_merge_in_progress == TRUE)
#define IS_SGLTE_SPLIT_IN_PROGRESS (REG_SUB_IS_SGLTE && reg_state_sglte_split_in_progress == TRUE)
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define MSG_HIGH_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_HIGH, "ds%d" str , (sub+1), (p1), (p2), (p3))
    
#define MSG_MED_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_MED, "ds%d" str , (sub+1), (p1), (p2), (p3))
    
#define MSG_LOW_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_LOW, "ds%d" str , (sub+1), (p1), (p2), (p3))
    
#define MSG_ERROR_DS( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_ERROR, "ds%d" str , (sub+1), (p1), (p2), (p3))

/* Warning: 3rd argument is not logged */
#define MSG_FATAL_DS( sub , str, p1, p2, p3)\
    ERR_FATAL ("ds%d" str , (sub+1), (p1), (p2))

extern uint8 reg_nv_context_id_sim[MAX_NAS_STACKS];
#define reg_nv_context_id reg_nv_context_id_sim[reg_as_id]

#define REG_OTHER_SUB(as_id) ((as_id == SYS_MODEM_AS_ID_1) ? SYS_MODEM_AS_ID_2 : SYS_MODEM_AS_ID_1)


extern boolean reg_state_is_pwr_on_manual_plmn_selection_sim[MAX_NAS_STACKS];

extern sys_stop_mode_reason_e_type reg_state_last_stop_mode_reason_sim[MAX_NAS_STACKS];
#define reg_state_last_stop_mode_reason reg_state_last_stop_mode_reason_sim[reg_as_id]

extern reg_mode_foreground_search_status_e_type reg_mode_foreground_search_status_sim[MAX_NAS_STACKS];
#define reg_mode_foreground_search_status reg_mode_foreground_search_status_sim[reg_as_id]

#ifdef DISABLE_9x05_OPTIMIZATIONS
extern boolean reg_state_is_roaming_plmn_to_be_camped_sim[MAX_NAS_STACKS];
#define reg_state_is_roaming_plmn_to_be_camped reg_state_is_roaming_plmn_to_be_camped_sim[reg_as_id]
#endif

extern rex_crit_sect_type  reg_sim_crit_sect;

#else

/* If DUAL SIM is not defined, no need to print subscription id */
#ifndef MSG_HIGH_DS
#define MSG_HIGH_DS( sub , str, p1, p2, p3)\
    MSG_HIGH (str , (p1), (p2), (p3))
#endif
#ifndef MSG_MED_DS
#define MSG_MED_DS( sub , str, p1, p2, p3)\
    MSG_MED (str , (p1), (p2), (p3))
#endif
#ifndef MSG_LOW_DS
#define MSG_LOW_DS( sub , str, p1, p2, p3)\
    MSG_LOW (str , (p1), (p2), (p3))
#endif
#ifndef MSG_ERROR_DS
#define MSG_ERROR_DS( sub , str, p1, p2, p3)\
    MSG_ERROR (str , (p1), (p2), (p3))
#endif
#ifndef MSG_FATAL_DS
#define MSG_FATAL_DS( sub , str, p1, p2, p3)\
    ERR_FATAL (str , (p1), (p2), (p3))
#endif

extern boolean reg_state_is_pwr_on_manual_plmn_selection;

extern sys_stop_mode_reason_e_type reg_state_last_stop_mode_reason;

extern reg_mode_foreground_search_status_e_type reg_mode_foreground_search_status;

#ifdef DISABLE_9x05_OPTIMIZATIONS
extern boolean reg_state_is_roaming_plmn_to_be_camped;
#endif


#endif
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
extern boolean reg_state_is_stack_suspended;
#endif

#if defined (FEATURE_DUAL_SIM ) || defined(FEATURE_SGLTE)
#define REG_SUB (reg_as_id)
#ifndef FEATURE_DUAL_SIM
#define REG_LOCAL_SUB (reg_as_id)
#else
#define REG_LOCAL_SUB (as_id)
#endif
#else
#define REG_SUB 0
#define REG_LOCAL_SUB 0
#endif

#ifdef FEATURE_DUAL_SIM
extern sys_wwan_adavnce_scan_pram_s_type*  reg_state_info_gps_wwan_search_info_param_sim[MAX_AS_IDS];
#define reg_state_info_gps_wwan_search_info_param reg_state_info_gps_wwan_search_info_param_sim[reg_as_id]
#else
extern sys_wwan_adavnce_scan_pram_s_type*  reg_state_info_gps_wwan_search_info_param;
#endif

#ifdef FEATURE_CIOT
typedef struct reg_state_edrx_params
{
   mm_cmd_get_edrx_params_rsp_s edrx;
   /* Absoulte time of a next eDRX page*/
   uint64 next_page_ms;
}reg_state_edrx_params_s_type;
#endif

#define REG_DEBUG_MESSAGE_BUFFER_SIZE      40
extern byte reg_msg_info;

extern reg_state_e_type reg_state_curr_p;
extern reg_state_e_type reg_state_next_p;


#if defined(FEATURE_TDSCDMA) || defined (FEATURE_SGLTE)
typedef enum reg_lte_timer_e
{
    REG_LTE_TIMER_NONE = -1,
    REG_LTE_LONG_TIMER,
    REG_LTE_SHORT_TIMER,
} reg_lte_timer_e_type;
#endif
#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
extern sys_overall_feature_t reg_sub_config_sim[MAX_AS_IDS];
#define reg_sub_config reg_sub_config_sim[reg_sub_id]

#define REG_SUB_IS_SGLTE (reg_sub_config == SYS_OVERALL_FEATURE_MODE_SGLTE)
#define REG_SUB_IS_SGLTE_SUB(x) (reg_sub_config_sim[x]== SYS_OVERALL_FEATURE_MODE_SGLTE)
#endif

typedef struct reg_debug_buffer_s
{ 
  byte message_set;
  byte message_id;
  boolean to_reg;
  reg_state_e_type present_reg_state;
  sys_mode_pref_e_type                 mode_pref;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  sys_modem_as_id_e_type as_id;
#endif
  byte msg_info;

}reg_debug_buffer_s_type;

extern volatile reg_debug_buffer_s_type reg_debug_buffer[REG_DEBUG_MESSAGE_BUFFER_SIZE];
extern byte reg_debug_buffer_index;
extern void reg_add_message_to_debug_buffer
(
  byte message_set, 
  byte message_id,
  boolean to_reg,
  sys_mode_pref_e_type                 mode_pref
);
extern void reg_populate_reg_msg_info(reg_cmd_type* reg_cmd_p);



/*==============================================================================

FUNCTION NAME

  process_cm_network_list_req

==============================================================================*/
extern reg_state_e_type process_cm_network_list_req
(
  const cm_network_list_req_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  process_cm_service_req

==============================================================================*/

extern reg_state_e_type process_cm_service_req
(
  const cm_service_req_s_type* msg_p
);


extern reg_state_e_type process_cm_sim_available_req
(
  cm_sim_available_req_s_type* msg_p
);


extern reg_state_e_type process_cm_sim_not_available_req
(
  cm_sim_not_available_req_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  process_cm_stop_mode_req

==============================================================================*/

extern reg_state_e_type process_cm_stop_mode_req
(
  const cm_stop_mode_req_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  process_cm_ph_status_chgd_req

==============================================================================*/

extern reg_state_e_type process_cm_ph_status_chgd_req
(
  const cm_ph_status_chgd_req_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

  process_cm_set_drx_req

==============================================================================*/

extern reg_state_e_type process_cm_set_drx_req
(
     const cm_set_drx_req_s_type* msg_p
);


#ifdef FEATURE_TDSCDMA

/*==============================================================================

FUNCTION NAME

  process_cm_set_hplmn_irat_timer

==============================================================================*/

extern reg_state_e_type process_cm_set_hplmn_irat_timer
(
    const cm_set_hplmn_irat_search_timer_req_s_type *msg_p
);

#endif


/*==============================================================================

FUNCTION NAME

  process_cm_enter_psm_mode_req

==============================================================================*/

extern reg_state_e_type process_cm_enter_psm_mode_req
(
     const cm_enter_psm_mode_req_s_type* msg_p
);


 /*==============================================================================
 
 FUNCTION NAME
 
   process_mmr_camped_ind
 
 ==============================================================================*/
 
 extern reg_state_e_type process_mmr_camped_ind
 (
     const mmr_camped_ind_s_type* msg_p
 );


 
   /*==============================================================================
   
   FUNCTION NAME
   
	 process_mmr_plmn_search_cnf
   
   ==============================================================================*/
   
   extern reg_state_e_type process_mmr_plmn_search_cnf
   (
     const mmr_plmn_search_cnf_s_type* msg_p
   );

/*==============================================================================

FUNCTION NAME

  process_cm_hplmn_search_req

==============================================================================*/

extern reg_state_e_type process_cm_hplmn_search_req
(
  const cm_hplmn_search_req_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

  process_mmr_mm_information_ind

==============================================================================*/

extern reg_state_e_type process_mmr_mm_information_ind
(
  const mmr_mm_information_ind_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  process_mmr_reg_cnf

==============================================================================*/

extern reg_state_e_type process_mmr_reg_cnf
(
  mmr_reg_cnf_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  process_mmr_service_ind

==============================================================================*/

extern reg_state_e_type process_mmr_service_ind
(
  mmr_service_ind_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  process_mmr_sim_available_cnf

==============================================================================*/


extern reg_state_e_type process_mmr_sim_available_cnf
(
  mmr_sim_available_cnf_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  process_mmr_sim_not_available_cnf

==============================================================================*/

extern reg_state_e_type process_mmr_sim_not_available_cnf
(
  mmr_sim_not_available_cnf_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  process_mmr_stop_mode_cnf

==============================================================================*/

extern reg_state_e_type process_mmr_stop_mode_cnf
(
  mmr_stop_mode_cnf_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  process_idle_sim_refresh_fcn_ind

==============================================================================*/
extern reg_state_e_type process_idle_sim_refresh_fcn_ind
(
  sim_refresh_fcn_ind_s_type* msg_p
);

 /*==============================================================================

FUNCTION NAME

  process_sim_refresh_fcn_ind

==============================================================================*/
extern reg_state_e_type process_sim_refresh_fcn_ind
(
  sim_refresh_fcn_ind_s_type* msg_p
);

#ifdef FEATURE_FEMTO_CSG
/*==============================================================================

FUNCTION NAME

  process_mmr_csg_list_read_complete
==============================================================================*/
extern reg_state_e_type process_mmr_csg_list_read_complete
(
 void
);

#endif

#ifdef FEATURE_TDSCDMA
/*==============================================================================

FUNCTION NAME

  process_cm_set_hplmn_irat_timer

==============================================================================*/
extern reg_state_e_type process_cm_set_hplmn_irat_timer
(
  const cm_set_hplmn_irat_search_timer_req_s_type *msg_p
);
#endif


/*==============================================================================

FUNCTION NAME

  reg_state_process_mmr_reg_reject_ind

==============================================================================*/

extern reg_state_e_type reg_state_process_mmr_reg_reject_ind
(
  mmr_reg_reject_ind_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  reg_state_handle_mmr_reg_reject_ind

==============================================================================*/
extern reg_state_e_type reg_state_handle_mmr_reg_reject_ind 
(
  mmr_reg_reject_ind_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  reg_state_process_mmr_reg_cnf

==============================================================================*/

extern reg_state_e_type reg_state_process_mmr_reg_cnf
(
  mmr_reg_cnf_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_network_list_req

==============================================================================*/
extern reg_state_e_type reg_state_process_cm_network_list_req
(
  const cm_network_list_req_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

  process_mmr_ph_status_change_cnf

==============================================================================*/

extern reg_state_e_type process_mmr_ph_status_change_cnf
(
  mmr_ph_status_change_cnf_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

process_mmr_reg_reject_ind

==============================================================================*/

extern reg_state_e_type process_mmr_reg_reject_ind
(
  mmr_reg_reject_ind_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

  process_mmr_enter_psm_mode_rsp

==============================================================================*/

extern reg_state_e_type process_mmr_enter_psm_mode_rsp
(
     const mmr_enter_psm_mode_rsp_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  reg_state_unexpected_cm_network_list_req

==============================================================================*/

extern reg_state_e_type reg_state_unexpected_cm_network_list_req
(
  const cm_network_list_req_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_service_req

==============================================================================*/

extern reg_state_e_type reg_state_process_cm_service_req
(
  const cm_service_req_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_sim_available_req

==============================================================================*/

extern reg_state_e_type reg_state_process_cm_sim_available_req
(
  cm_sim_available_req_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_sim_not_available_req

==============================================================================*/

extern reg_state_e_type reg_state_process_cm_sim_not_available_req
(
  cm_sim_not_available_req_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_stop_mode_req

==============================================================================*/

extern reg_state_e_type reg_state_process_cm_stop_mode_req
(
  const cm_stop_mode_req_s_type* msg_p
);

/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_ph_status_chgd_req

==============================================================================*/

extern reg_state_e_type reg_state_process_cm_ph_status_chgd_req
(
  const cm_ph_status_chgd_req_s_type* msg_p
);

#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  reg_state_process_cm_set_drx_req

==============================================================================*/

extern reg_state_e_type reg_state_process_cm_set_drx_req
(
  const cm_set_drx_req_s_type* msg_p
);

#endif

/*==============================================================================

FUNCTION NAME

  reg_state_process_sim_refresh_fcn_ind

==============================================================================*/
extern reg_state_e_type reg_state_process_sim_refresh_fcn_ind
(
  sim_refresh_fcn_ind_s_type* msg_p
);


extern reg_state_e_type reg_state_process_backoff_fplmn_timer_expiration
(
     void
);

extern reg_state_e_type process_update_lte_cap_timer_expiration
(
     void
);

extern reg_state_e_type reg_state_process_t3245_timer_expiration
(
     void
);

extern reg_state_e_type process_update_irat_ciot_timer_expiration
(
     void
);

extern reg_state_e_type process_hplmn_search_timer_expiration
(
  void
);

extern reg_state_e_type process_backoff_fplmn_timer_expiration
(
  void
);

extern reg_state_e_type reg_state_process_update_lte_cap_timer_expiration(void);


extern reg_state_e_type reg_state_process_t3245_expiry(void);

extern reg_state_e_type process_mmr_reg_req_ignored_indication(void);

extern reg_state_e_type reg_state_process_mmr_reg_req_ignored_ind(void);

extern reg_state_e_type reg_state_t3245_exp_clean_up(void);

extern reg_state_e_type process_irat_ciot_search_timer_expiry (void);


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*
** One reg_state_s_type is declared for each of the operating states.
*/
//extern reg_state_s_type  reg_state_inactive;
//extern reg_state_s_type  reg_state_idle;
//extern reg_state_s_type  reg_state_registering;
//extern reg_state_s_type  reg_state_deregistering;
//extern reg_state_s_type  reg_state_automatic_search;
//extern reg_state_s_type  reg_state_manual_search;
//extern reg_state_s_type  reg_state_offline;

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/
extern reg_state_e_type reg_state_process_backoff_fplmn_timer_expiration(void);

#ifdef FEATURE_ENHANCED_NW_SELECTION
/*==============================================================================

FUNCTION NAME

  reg_state_process_fcn_act_hplmn

DESCRIPTION

  Function that processes common functionality for FCN - Acting HPLMN.
  fcn_delay - TRUE means FCN processing should be delayed.

RETURN VALUE

  boolean - TRUE if processing is done successfully.

==============================================================================*/
extern void reg_state_process_fcn_act_hplmn
(
  boolean fcn_delay
);

#endif


extern reg_cm_service_req_e_type reg_state_get_last_cm_service_req_type(void);



/*==============================================================================

FUNCTION NAME

  reg_state_process_fcn_hplmn

DESCRIPTION

  Function that processes FCN - HPLMN Timer.
  file_id - GSM-HPLMN or USIM-HPLMN.

RETURN VALUE

  boolean - TRUE if processing is done successfully.

==============================================================================*/
extern void reg_state_process_fcn_hplmn
(
  void
);

#ifdef FEATURE_NAS_GW
#if defined FEATURE_MMCP_EGPRS || defined FEATURE_GSM_DTM || defined FEATURE_HSDPA
/*==============================================================================

FUNCTION NAME

  reg_state_process_cell_service_ind

==============================================================================*/
void reg_state_process_cell_service_ind
(
  mmr_cell_service_ind_s_type* mmr_ind
);
#endif
#endif

#ifdef FEATURE_HSPA_CALL_STATUS_IND
/*==============================================================================

FUNCTION NAME

  reg_state_process_hspa_call_status_ind

==============================================================================*/
extern void reg_state_process_hspa_call_status_ind
(
  const mmr_hspa_call_status_ind_s_type* mmr_ind
);
#endif

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
/*==============================================================================

FUNCTION NAME

  reg_state_process_conn_mode_service_status_ind

==============================================================================*/
extern void reg_state_process_conn_mode_service_status_ind
(
  const mmr_conn_mode_service_status_ind_s_type* mmr_ind
);
#endif

/*==============================================================================

FUNCTION NAME

  reg_state_main

DESCRIPTION

  Function that processes each of the input events for the REG state machine.

RETURN VALUE

  None

==============================================================================*/

extern void reg_state_main
(
  reg_cmd_type* reg_cmd_p
);


/*==============================================================================

FUNCTION NAME

  reg_state_cm_transaction_id_get

DESCRIPTION

  Function that returns the last transaction id received from CM.

RETURN VALUE

  reg_cm_transaction_id_type      The last transaction id received from CM.

==============================================================================*/

extern reg_cm_transaction_id_type reg_state_cm_transaction_id_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_state_print_service_state

DESCRIPTION

  Function that outputs the service state for debug.

RETURN VALUE

  None

==============================================================================*/

extern void reg_state_print_service_state
(
  sys_plmn_service_state_s_type service_state
);



#ifdef FEATURE_LTE

/*==============================================================================

FUNCTION NAME

  reg_state_update_service_state_if_required

==============================================================================*/


extern void reg_state_update_service_state_if_required(reg_state_e_type state_p );

#endif

/*==============================================================================

FUNCTION NAME

  reg_state_sim_state_get

DESCRIPTION

  Function that returns the current SIM state of the MS.

RETURN VALUE

  sys_srv_status_e_type       The current SIM state of the MS.

==============================================================================*/

extern sys_sim_state_e_type reg_state_sim_state_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_state_service_status_get

DESCRIPTION

  Function that returns the current service status of the MS.

RETURN VALUE

  sys_srv_status_e_type       The current service status of the MS.

==============================================================================*/

extern sys_srv_status_e_type reg_state_service_status_get
(
  void
);

#ifdef FEATURE_DUAL_SIM
/*==============================================================================

FUNCTION NAME

  reg_state_acq_status_get

DESCRIPTION

  Function that returns the acq status that was received previously from access stratum.

RETURN VALUE

  sys_acq_status_e_type       

==============================================================================*/
extern sys_acq_status_e_type reg_state_acq_status_get
(
  void
);
#endif

/*==============================================================================

FUNCTION NAME

  reg_state_service_status_set

DESCRIPTION

  Function that sets the current service status of the MS.

RETURN VALUE

  None

==============================================================================*/

extern void reg_state_service_status_set
(
  sys_srv_status_e_type service_status
);

/*==============================================================================

FUNCTION NAME

  reg_state_process_mmr_service_ind

==============================================================================*/

extern reg_state_e_type reg_state_process_mmr_service_ind
(
  mmr_service_ind_s_type* msg_p
);

/*==============================================================================  

FUNCTION NAME

  reg_state_service_lac_get

==============================================================================*/

extern word reg_state_service_lac_get
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_state_service_plmn_get

DESCRIPTION

  Function that returns the current serving plmn.

RETURN VALUE

  sys_plmn_id_s_type       The current serving plmn.

==============================================================================*/

extern sys_plmn_id_s_type reg_state_service_plmn_get
(
  void
);

#ifdef FEATURE_SGLTE
/*==============================================================================

FUNCTION NAME

  reg_state_service_plmn_get_per_subs

==============================================================================*/
extern sys_plmn_id_s_type reg_state_service_plmn_get_per_subs
(
  sys_modem_as_id_e_type as_id
);
#endif

/*==============================================================================

FUNCTION NAME

  reg_state_plmn_reg_type_get

DESCRIPTION

  Function that returns the current reg_plmn domain type of the MS.

RETURN VALUE

  sys_reg_domain_e_type       The current domain type of the reg_plmn.

==============================================================================*/

extern sys_reg_domain_e_type reg_state_plmn_reg_type_get
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_state_active_rat_get

DESCRIPTION

  Function that returns the RAT currently in use by the MS.

RETURN VALUE

  sys_srv_status_e_type        The RAT currently in use by the MS.

==============================================================================*/

extern sys_radio_access_tech_e_type reg_state_active_rat_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_state_automatic_search_initiated

DESCRIPTION

  Function that stores the dual mode flag and rat used for the initial
  automatic PLMN search.

RETURN VALUE

  None

==============================================================================*/

extern void reg_state_automatic_search_initiated
(
  boolean                      dual_mode,
  sys_radio_access_tech_e_type rat
);


/*==============================================================================

FUNCTION NAME

  reg_state_manual_search_initiated

DESCRIPTION

  Function that stores the mode preference and rat used for the initial
  manual PLMN search.

RETURN VALUE

  None

==============================================================================*/

extern void reg_state_manual_search_initiated
(
#ifndef FEATURE_RAT_PRIORITY_LIST
  sys_mode_pref_e_type         mode_pref,
#else
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info,
#endif
  reg_cm_network_list_req_e_type    search_type,  

  sys_radio_access_tech_e_type rat
);


/*==============================================================================

FUNCTION NAME

  reg_state_current_state_get

DESCRIPTION

  Function to get the current state of REG.

RETURN VALUE

  reg_state_e_type - the current state of REG

==============================================================================*/

extern reg_state_e_type reg_state_current_state_get();

#ifdef FEATURE_ENHANCED_NW_SELECTION
/*==============================================================================

FUNCTION NAME

  reg_state_delay_fcn_act_hplmn_get

DESCRIPTION

  Function to get if any FCN - Acting HPLMN is waiting or not.

RETURN VALUE

  boolean - Returns TRUE if any FCN - Acting HPLMN is delayed earlier.

==============================================================================*/

extern boolean reg_state_delay_fcn_act_hplmn_get
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_state_delay_fcn_act_hplmn_set

DESCRIPTION

  Function to set delay_fcn_act_hplmn, set to TRUE when a delay is required
  to process FCN - Acting HPLMN. Set to FALSE when this FCN is processed.

RETURN VALUE

  None.

==============================================================================*/

extern void reg_state_delay_fcn_act_hplmn_set
(
  boolean delay_fcn
);
#endif

/*==============================================================================

FUNCTION NAME

  reg_state_prev_plmn_get

DESCRIPTION
  Function to get the value to the previously camped on PLMN.

RETURN VALUE

  sys_plmn_id_s_type  previously camped on PLMN ID

==============================================================================*/

extern sys_plmn_id_s_type reg_state_prev_plmn_get
(
  void
);

/*==============================================================================
FUNCTION NAME
  reg_state_process_cm_enter_psm_mode_req

DESCRIPTION
  Function to process CM_ENTER_PSM_MODE_REQ.

RETURN VALUE
  Next REG state
==============================================================================*/
extern reg_state_e_type reg_state_process_cm_enter_psm_mode_req
(
  const cm_enter_psm_mode_req_s_type* msg_p
);

/*==============================================================================
FUNCTION NAME
  reg_state_process_cm_mcc_change_ind

DESCRIPTION
  Function to process CM_MCC_CHANGE_IND

RETURN VALUE
  Next REG state
==============================================================================*/

extern reg_state_e_type reg_state_process_cm_mcc_change_ind
(
  const cm_mcc_change_ind_s_type* msg_p
);
/*==============================================================================
FUNCTION NAME
  reg_state_process_unexpected_cm_enter_psm_mode_req

DESCRIPTION
  Function to process CM_ENTER_PSM_MODE_REQ.

RETURN VALUE
  Next REG state
==============================================================================*/
extern reg_state_e_type reg_state_process_unexpected_cm_enter_psm_mode_req
(
  const cm_enter_psm_mode_req_s_type* msg_p
);
/*==============================================================================
FUNCTION NAME
  reg_state_process_mmr_enter_psm_mode_rsp

DESCRIPTION
  Function to process MMR_ENTER_PSM_MODE_RSP.

RETURN VALUE
  Next REG state
==============================================================================*/
extern reg_state_e_type reg_state_process_mmr_enter_psm_mode_rsp
(
  const mmr_enter_psm_mode_rsp_s_type* msg_p
);

/*==============================================================================
FUNCTION NAME
  reg_state_process_unexpected_cm_enter_psm_mode_req

DESCRIPTION
  Function to process MMR_ENTER_PSM_MODE_RSP.

RETURN VALUE
  Next REG state
==============================================================================*/
extern reg_state_e_type reg_state_process_unexpected_mmr_enter_psm_mode_rsp
(
  const mmr_enter_psm_mode_rsp_s_type* msg_p
);


/*==============================================================================

FUNCTION NAME

  reg_state_process_hplmn_search_timer_expiration

==============================================================================*/

reg_state_e_type reg_state_process_hplmn_search_timer_expiration
(
     void
);


/*==============================================================================
FUNCTION NAME
  reg_state_prev_plmn_set

DESCRIPTION
  Function to set the value of previously camped on PLMN.

RETURN VALUE
  None
==============================================================================*/
extern void reg_state_prev_plmn_set
(
  sys_plmn_id_s_type plmn
);
/*==============================================================================
FUNCTION NAME
  reg_state_single_cell_search_set

DESCRIPTION
  Function to set the value flag to indicate GPS cell search

RETURN VALUE
  None
==============================================================================*/
extern void reg_state_single_cell_search_set
(
  uint8 total_num_cells
);
/*==============================================================================
FUNCTION NAME
  reg_state_single_cell_search_get

DESCRIPTION
  Function to get the value flag to indicate GPS cell search

RETURN VALUE
  None
==============================================================================*/
extern boolean reg_state_single_cell_search_get
(
  void
);
/*==============================================================================

FUNCTION NAME

  reg_state_roaming_ind_get

DESCRIPTION
  Function to get the current roaming status.

RETURN VALUE
  
  sys_roam_status_e_type Current roaming status

==============================================================================*/

extern sys_roam_status_e_type reg_state_roaming_ind_get 
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_state_plmn_search_transaction_id_get

DESCRIPTION
  Function to get the current transaction id sent in last MMR_PLMN_SEARCH_REQ.

RETURN VALUE
  
  Current PLMN search transaction id.

==============================================================================*/

extern byte reg_state_plmn_search_transaction_id_get 
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_state_plmn_search_transaction_id_set

DESCRIPTION
  Function to set the current transaction id to next value and return the new transaction id.
  This function will generate TI from 0 to 15.

RETURN VALUE
  
  New PLMN search transaction id to be sent in next PLMN Search req.

==============================================================================*/

extern byte reg_state_plmn_search_transaction_id_set
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_state_reg_service_domain_get

DESCRIPTION
  Function to return the current registered service domain.

RETURN VALUE
  
  Current registered service domain.

==============================================================================*/

extern sys_srv_domain_e_type reg_state_reg_service_domain_get 
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_state_active_rat_set

DESCRIPTION
  Function to set the current active rat

RETURN VALUE
  
  None

==============================================================================*/
extern void reg_state_active_rat_set
(
  sys_radio_access_tech_e_type  rat
);

/*==============================================================================

FUNCTION NAME

  reg_state_camped_ind_received_set

DESCRIPTION
  Function to set whether a camped ind is received

RETURN VALUE
  
  None

==============================================================================*/
extern void reg_state_camped_ind_received_set
(
  boolean           flag
);

void reg_state_process_mmgsdi_subscription_ready
(
  void
);

#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
/*==============================================================================

FUNCTION NAME

  reg_state_rlf_plmn_set

DESCRIPTION
  Function to set rlf plmn

RETURN VALUE
  
  None

==============================================================================*/
extern void reg_state_rlf_plmn_set
(
  sys_plmn_id_s_type rlf_plmn
);


/*==============================================================================

FUNCTION NAME

  reg_state_camped_ind_received_set

DESCRIPTION
  Function to get rlf plmn

RETURN VALUE
  
  None

==============================================================================*/
extern sys_plmn_id_s_type reg_state_rlf_plmn_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_state_camped_ind_received_set

DESCRIPTION
  Function to get rat on which rlf happened

RETURN VALUE
  
  None

==============================================================================*/
sys_radio_access_tech_e_type reg_state_rlf_rat_get
(
  void
);
#endif /*FEATURE_LTE*/

/*==============================================================================

FUNCTION NAME

  reg_state_user_resel_plmn_rat_get

DESCRIPTION
  Function to get plmn and rat combination which should be put at end for user reselection and foreground search

RETURN VALUE
  
  None

==============================================================================*/
void reg_state_user_resel_plmn_rat_get
(
  sys_plmn_id_s_type *plmn,
  sys_radio_access_tech_e_type *rat
);
/*==============================================================================

FUNCTION NAME

  reg_state_user_resel_get_rat

DESCRIPTION
  Function to get the rat value from the cached location 

RETURN VALUE
  
  None

==============================================================================*/
sys_radio_access_tech_e_type reg_state_user_resel_get_rat(void);
/*==============================================================================

FUNCTION NAME

  reg_state_process_steering_of_roaming_refresh

DESCRIPTION
  Function to process steering of roaming refresh command

RETURN VALUE
  
  None

==============================================================================*/
extern void reg_state_process_steering_of_roaming_refresh(mmgsdi_refresh_source_enum_type source);


#ifdef FEATURE_GSTK 
/*==============================================================================

FUNCTION NAME

  reg_state_is_gstk_event_to_be_send

DESCRIPTION
  Function to tell whether IDLE event need to be send or not to GSTK

RETURN VALUE
  
  None

==============================================================================*/
extern boolean reg_state_is_gstk_event_to_be_send
(
 void 
);
/*==============================================================================

FUNCTION NAME

  reg_state_set_gstk_event_flag

DESCRIPTION
  Function to set whether IDLE event need to be send or not to GSTK

RETURN VALUE
  
  None

==============================================================================*/
extern void reg_state_set_gstk_event_flag(boolean flag);

/*==============================================================================

FUNCTION NAME

  reg_state_reset_last_gstk_event_sent

DESCRIPTION
  function resets srv statsus to NONE incase of power off

RETURN VALUE
  
  None

==============================================================================*/
extern void reg_state_reset_last_gstk_event_sent(void);

/*==============================================================================

FUNCTION NAME

  reg_state_update_last_gstk_sent_plmn

DESCRIPTION
  Function to store PLMN id of the cell where GSTK event is sent

RETURN VALUE
  
  None

==============================================================================*/
extern void reg_state_update_last_gstk_sent_plmn(sys_plmn_id_s_type plmn);
/*==============================================================================

FUNCTION NAME

  reg_state_update_last_gstk_sent_svc

DESCRIPTION
  Function to store service staus which is sent in IDLE event to GSTK

RETURN VALUE
  
  None

==============================================================================*/
extern void reg_state_update_last_gstk_sent_svc(sys_srv_status_e_type srv_status);
/*==============================================================================

FUNCTION NAME

  reg_state_update_last_gstk_sent_svc

DESCRIPTION
  Function to get the service staus which is sent in IDLE event to GSTK

RETURN VALUE
  
  None

==============================================================================*/
extern sys_srv_status_e_type reg_state_get_last_gstk_sent_svc(void);


/*==============================================================================

FUNCTION NAME

  reg_state_update_last_gstk_sent_rat

==============================================================================*/

extern void reg_state_update_last_gstk_sent_rat(sys_radio_access_tech_e_type rat);

/*==============================================================================

FUNCTION NAME

  reg_state_get_last_gstk_sent_rat

==============================================================================*/

extern sys_radio_access_tech_e_type reg_state_get_last_gstk_sent_rat(void);

extern void reg_state_reset_last_gstk_rat_sent(void);
#endif/*FEATURE_GSTK*/


#ifdef DISABLE_9x05_OPTIMIZATIONS
/*==============================================================================

FUNCTION NAME

  reg_state_is_no_service_to_be_send_due_to_china_mcc

==============================================================================*/


extern boolean reg_state_is_no_service_to_be_send_due_to_china_mcc(sys_plmn_id_s_type plmn);
#endif


#ifdef FEATURE_LTE

/*==============================================================================

FUNCTION NAME

  reg_state_proc_mmr_ps_detach_cnf

==============================================================================*/

extern void reg_state_proc_mmr_ps_detach_cnf
(
  void
);


#endif

#ifdef FEATURE_FEMTO_CSG
extern void reg_state_update_last_cm_service_req_type(reg_cm_service_req_e_type service_req_type);
extern sys_csg_info_s_type  reg_state_csg_info_get(void);
extern reg_state_e_type reg_state_process_unexpected_mmr_csg_list_read_complete
(
 void
);
extern boolean reg_state_power_on_csg_search;
#endif

#ifdef FEATURE_SGLTE

extern boolean reg_sglte_mode;

extern sys_ue_mode_e_type  reg_sglte_ue_mode ;

extern sys_srv_status_e_type reg_state_get_service_status_per_subs(sys_modem_as_id_e_type as_id);

extern sys_srv_domain_e_type reg_state_get_service_domain_per_subs(sys_modem_as_id_e_type as_id);

extern sys_srv_capability_e_type reg_state_get_plmn_service_capability(sys_modem_as_id_e_type as_id);

extern sys_radio_access_tech_e_type reg_state_get_active_rat_per_subs(sys_modem_as_id_e_type as_id);
extern boolean reg_state_sglte_ue_mode_change_possible(sys_plmn_id_s_type plmn);
extern void reg_state_validate_timer_split_operation(void);
extern void reg_state_validate_timer_merge_operation(void);
extern void reg_state_start_appropriate_validate_timer(sys_ue_mode_e_type sglte_mode);
#define REG_SGLTE_DUAL_STACK (REG_SUB_IS_SGLTE && (reg_sglte_mode == TRUE) && (reg_cs_stack_id != reg_ps_stack_id))
#define REG_SGLTE_DUAL_STACK_SUB(x) (REG_SUB_IS_SGLTE_SUB(x)&& (reg_sglte_mode == TRUE) && (reg_cs_stack_id != reg_ps_stack_id))
#define IS_UE_IN_SGLTE_STATE_4 (REG_SUB_IS_SGLTE && reg_sglte_mode && (reg_cs_stack_id == reg_ps_stack_id))

extern void reg_state_set_rlf_started(boolean value);

extern boolean reg_state_get_rlf_started(void);
#endif
#ifdef FEATURE_LTE
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)

extern boolean reg_state_force_pref_pending_sim[MAX_NAS_STACKS]; ;                
#define reg_state_force_pref_pending reg_state_force_pref_pending_sim[reg_as_id]
#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern boolean reg_state_force_pref_pending_sim[MAX_NAS_STACKS]; ;                
#define reg_state_force_pref_pending reg_state_force_pref_pending_sim[reg_as_id]
#else
extern boolean reg_state_force_pref_pending;
#endif
extern boolean reg_state_get_force_pref_pending(void);
#endif


#if defined FEATURE_TDSCDMA || defined FEATURE_SGLTE
#define REG_MAX_LTE_AVAILABLE_CELL_NUM 1000
typedef struct cell_list
{
  sys_lac_type  lac;
  word cell_id;
  earfcn_nas_type earfcn;  
  int8    count;     
  rex_timer_cnt_type      timer_count;

  
  struct cell_list  *next_ptr;
}cell_list_type;

typedef struct lte_cell_list
{
  sys_plmn_id_s_type      plmn;
  sys_radio_access_tech_e_type rat;     
  uint32                  length;
  cell_list_type      *cell_ptr;
  struct lte_cell_list  *next_ptr;
} lte_cell_list_type;

lte_cell_list_type  *lte_available_list_ptr;

lte_cell_list_type  *lte_non_available_list_ptr;

lte_cell_list_type*  reg_state_add_cell_id_to_lte_list(  lte_cell_list_type* list_ptr,sys_plmn_id_s_type plmn, sys_radio_access_tech_e_type rat,sys_lac_type lac,word cell_id,rex_timer_cnt_type timer_count );

lte_cell_list_type* reg_state_create_new_plmn_rat_list_entry(sys_plmn_id_s_type plmn, sys_radio_access_tech_e_type rat,sys_lac_type lac,word cell_id, rex_timer_cnt_type timer_count );

cell_list_type* reg_state_create_new_cell_list_entry(sys_lac_type lac,word cell_id ,rex_timer_cnt_type timer_count);          
void reg_state_cell_present_in_list( byte *found_in_avaialble_list, byte *found_in_non_avaialble_list);
word reg_get_cell_id(void );

int8 reg_state_increment_cell_list_counter
(
  lte_cell_list_type* lte_list_ptr,
  sys_plmn_id_s_type plmn, 
  sys_radio_access_tech_e_type rat,
  sys_lac_type lac,
  word cell_id, 
  int8 count
);


boolean reg_is_cell_present_in_list
(

  lte_cell_list_type* list_ptr,
  sys_plmn_id_s_type plmn, 
  sys_radio_access_tech_e_type rat,  
  sys_lac_type lac,
  word cell_id 
);

lte_cell_list_type* reg_state_move_cell_up_queue(lte_cell_list_type* list_head_ptr,
  sys_plmn_id_s_type plmn, 
  sys_radio_access_tech_e_type rat,
  sys_lac_type lac,
  word cell_id 
);
void reg_state_set_irat_hplmn_timer_search_ongoing(boolean value);
void reg_state_set_irat_lte_timer_search_ongoing(boolean value);

boolean reg_state_get_irat_hplmn_timer_search_ongoing (void);

boolean reg_state_get_irat_lte_timer_search_ongoing (void);

lte_cell_list_type* reg_delete_cell_entry_from_list(lte_cell_list_type* lte_list_ptr, sys_plmn_id_s_type plmn,sys_radio_access_tech_e_type rat, sys_lac_type lac,word cell_id );  

boolean reg_state_cell_not_present_in_lte_lists(void);

boolean reg_state_add_tds_or_gsm_cell_id_to_lte_list(boolean lte_avaialble,sys_radio_access_tech_e_type rat, uint32 earfcn);  

boolean reg_state_search_earfcn_in_list(earfcn_nas_type earfcn, uint32 earfcn_curr);       

boolean reg_state_add_earfcn_to_lte_available_list(sys_radio_access_tech_e_type rat,sys_plmn_id_s_type plmn, sys_lac_type lac,word cell_id,uint32 earfcn);    

void reg_state_cell_present_in_list( byte *found_in_avaialble_list, byte *found_in_non_avaialble_list);

lte_cell_list_type* reg_state_delete_tds_or_gsm_cell_from_list(lte_cell_list_type* list_ptr,sys_radio_access_tech_e_type rat);

lte_cell_list_type* reg_delete_last_cell_entry_from_list(lte_cell_list_type* list_ptr);

lte_cell_list_type* reg_convert_byte_array_to_lte_cell_list( lte_cell_list_type* lte_list_ptr, byte *byte_array,boolean lte_available);       

byte* reg_convert_lte_list_into_array(lte_cell_list_type* lte_list_ptr, uint32 *length, boolean lte_available);      
void reg_state_clear_lte_lists(void);
word reg_get_cell_id(void );

void reg_state_set_lte_search_requested(boolean value);
boolean reg_state_get_lte_search_requested(void);
boolean reg_state_cell_id_changed(sys_plmn_service_state_s_type service_state_local, word cell_id);
void reg_state_start_irat_timers_with_10sec(void);
void reg_state_dump_earfcn_list(earfcn_nas_type* earfcn_curr);                 

void reg_state_dump_the_list(lte_cell_list_type* temp_list_ptr);
boolean reg_state_get_force_bplmn_search_start(void);

#define MAX_LTE_TIMER_COUNT 10
#define MAX_LTE_SEARCH_LIST_DURATION 10*24*60*60*1000

byte reg_state_start_backoff_lte_search_timer_count(lte_cell_list_type* lte_list_ptr);
lte_cell_list_type* reg_state_process_back_off_lte_timer_expiry(lte_cell_list_type* lte_list_ptr );
boolean reg_state_get_force_irat_search_initiated (void);
void reg_state_set_force_irat_search_initiated(boolean value);
#endif
#ifdef FEATURE_SGLTE_DUAL_SIM
extern boolean reg_state_get_trm_failure_sglte_main(void);
#endif 
extern void reg_check_for_null_ptr(void* msg_p);

/*==============================================================================

FUNCTION NAME

  reg_state_get_hplmn_search_timer_at_power_up

==============================================================================*/
extern boolean reg_state_get_hplmn_search_timer_at_power_up(void);

/*==============================================================================

FUNCTION NAME

  reg_state_get_power_up_system_selection

==============================================================================*/
extern boolean reg_state_get_power_up_system_selection(void);
#endif
#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  reg_state_sim_status_get

==============================================================================*/

extern sys_sim_state_e_type reg_state_sim_status_get
(
  void
);
/*==============================================================================

FUNCTION NAME

  reg_state_active_is_lte_rat
 
DESCRIPTION: This function returns TRUE if active RAT is any of the LTE rats

==============================================================================*/
boolean reg_state_is_active_rat_lte(sys_radio_access_tech_e_type rat);

/*==============================================================================

FUNCTION NAME

  reg_state_active_is_lte_rat_legacy
 
DESCRIPTION: This function returns TRUE if active RAT is of the LTE legacy type

==============================================================================*/
boolean reg_state_is_active_rat_lte_legacy(sys_radio_access_tech_e_type rat);

/*==============================================================================
FUNCTION NAME:  reg_state_update_edrx_params

DESCRIPTION
  This REG function updates edrx params

RETURN VALUE
  none
==============================================================================*/
void reg_state_update_edrx_params
(
  mmr_edrx_params_change_ind_s_type *msg_ptr
);

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
/*==============================================================================
FUNCTION NAME:  reg_state_is_stack_suspended_get

DESCRIPTION
  return if stack is suspended or not

RETURN VALUE
  boolean
==============================================================================*/
extern boolean reg_state_is_stack_suspended_get
(
 void
);
#endif
#ifdef FEATURE_CIOT
//reg_state_irat_ciot_search_timer_expiry_f_type reg_state_unexpected_ciot_irat_search_timer_expiry;
//reg_state_process_cm_mcc_change_ind_f_type     reg_state_unexpected_process_cm_mcc_change_ind;
void reg_state_remove_other_rats (sys_rat_pri_list_info_s_type* rat_pri_list_info, 
                                        sys_radio_access_tech_e_type rat);
/*==============================================================================

FUNCTION NAME

  reg_state_is_preferred_rat
 
DESCRIPTION: This function returns TRUE if active RAT is the most preferred 
 
 
NOTES: This function will determine if IRAT search needs to be started

==============================================================================*/
boolean reg_state_is_preferred_rat(sys_radio_access_tech_e_type rat);

/*==============================================================================

FUNCTION NAME

 reg_state_get_edrx_info
 
 Return the EDRX parameters along with the time the next page occurs
 
==============================================================================*/
void reg_state_get_edrx_info(reg_state_edrx_params_s_type* info);
/*==============================================================================

FUNCTION NAME

 reg_state_set_psm_ready
 
 Sets a flag that indicates if a PSM (Auto) ready was received
 
==============================================================================*/
void reg_state_set_psm_ready (boolean value);
/*==============================================================================

FUNCTION NAME

 reg_state_get_psm_ready
 
 Returns a flag that indicates if a PSM (Auto) ready was received
 
==============================================================================*/
boolean reg_state_get_psm_ready (void);
/*==============================================================================

FUNCTION NAME

 reg_state_timers_expired
 
 Return a boolean value indicating if selected REG timers have expired
 
==============================================================================*/
boolean reg_state_get_timers_expired(void);

/*==============================================================================

FUNCTION NAME

 reg_state_set_timer_expired
 
 Sets a flag reg_state_timer_expired
 
==============================================================================*/
void reg_state_set_timer_expired (boolean value);
/*==============================================================================

FUNCTION NAME

 reg_state_set_psm_sleep_blocked
 
 Sets a flag that indicates that the PSM sleep was blocked
==============================================================================*/
void reg_state_set_psm_sleep_blocked (boolean value);
boolean reg_state_get_psm_sleep_blocked (void);

/*==============================================================================

FUNCTION NAME

 reg_state_get_current_MCC
 
 Returns the value of the current MCC
==============================================================================*/
uint32 reg_state_get_current_MCC(void);
/*==============================================================================

FUNCTION NAME

 reg_state_set_current_MCC
 
 Updates the current MCC
==============================================================================*/
void reg_state_set_current_MCC(sys_plmn_id_s_type plmn);
boolean reg_state_repeat_srv_req_get (void);
extern void reg_state_reset_repeat_srv_req (void);
/*==============================================================================

FUNCTION NAME

 reg_state_reset_psm_alignment
 
DESCRIPTION: 
 This function resets the flags that are used during PSM alignment.
 If the PSM sleep is blocked, a message will be sent to CM
==============================================================================*/
void reg_state_reset_psm_alignment(void);
/*==============================================================================

FUNCTION NAME

 reg_state_get_HLOS_MCC
 
 Returns the value of the HLOS MCC that was injected bny the MMODE
==============================================================================*/
uint32 reg_state_get_HLOS_MCC(void);


/*==============================================================================

FUNCTION NAME

  reg_state_output_name

==============================================================================*/

extern void reg_state_output_name ( void );

extern void reg_mode_create_available_plmn_list_ciot
(
  const sys_plmn_rat_list_s_type*    higher_priority_plmn_list_p,  
  sys_detailed_plmn_list_s_type* detailed_list
);


/*===========================================================================
FUNCTION 
           reg_state_get_override_roam_status
DESCRIPTION 
          check if we need to override roaming status as per requirements
===========================================================================*/
extern sys_roam_status_e_type reg_state_get_override_roam_status
(
  sys_plmn_id_s_type            plmn,
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type        tmp_as_id,
#endif
  sys_roam_status_e_type        curr_roam_status
);

/*==============================================================================

FUNCTION NAME

  reg_state_calculate_restart_value
 
 DESCRIPTION
  This function determines if a given REG timer needs to be restarted in order
  to meet the alignment requirements with eDRX and PSM
 
 RETURN VALUE
  0 if a timer does not need to be restarted
  non-zero : a new timeout value
==============================================================================*/
extern uint64 reg_state_calculate_restart_value (void);


extern void reg_state_process_sim_client_id
(
  mmgsdi_client_id_type   cid
);


/*==============================================================================

FUNCTION NAME

 reg_state_cache_edrx_params
 
DESCRIPTION: 
 This function stores the EDRX parameters sent by the NW

==============================================================================*/
extern void reg_state_cache_edrx_params(mm_cmd_get_edrx_params_rsp_s edrx);

#endif


/*===========================================================================
FUNCTION NAME 
         reg_timers_get_hplmn_search_timer

DESCRIPTION
  Function to update the HPLMN timer value
  
RETURN VALUE
  None
==========================================================================*/
extern uint32 reg_timers_get_hplmn_search_timer(void);



#endif

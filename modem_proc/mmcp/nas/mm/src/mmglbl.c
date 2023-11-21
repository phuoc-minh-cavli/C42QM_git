/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmglbl.c_v   1.24   17 Jul 2002 13:06:06   kishores  $
$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/mmglbl.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/14   gps     Created New EFS item Ss_Screening_Indicator
06/03/01   sbk     Global variables are renamed to use mm_ prefix
06/08/01   jca     Added/deleted some regional variables.
08/09/01   jc      Add log initialization.
08/15/01   ab      Commented out diag.h and log_config to get latest MM to build
                   in VST.  Renamed CM to CNM.
09/18/01   sbk     Added support for using NVMEM
09/21/01   sbk     Removed unused and unnecessary code
10/19/01   sbk     Removed hardcoded ue capability value.
04/15/02   ks      Setting the mm_sig_low_priority & nreg_release_pending
                   values to default values "FALSE"
04/23/02   jca     Deleted mm_cnm_active_transaction. Added mm_cnm_rel_req_received.
06/05/02   ks      Added support for Dual mode operation.
06/26/02   ks      Setting the MSCR field to default value in
                   mm_system_information system variable.
07/03/02   jca     Deleted mm_plmn_list_state.
07/12/02   ks      Now returning the ACTIVE_RAT information rather than
                   the Preferred RAT.
07/17/02   ks      Setting the mm_gsm_supported and mm_wcdma_supported.
07/22/02   ks      Added mm_rat_change_pending & mm_serv_change_pending to support
                   Dynamic RAT and Service change.
08/09/02   jca     Now initializing mm_pending_lu_request.
08/09/02   mks     Added mm_hi_access_class & mm_lo_access_class to cache UE access
                   class information.
08/28/02   cd      Added initialization of mm_service_plmn.mm_service_state
10/07/02   jca     Changes to support CS/PS combined attach procedures.
10/30/02   mks     Added placeholder for CM service establishment cause
11/08/02   cd      Added access_class_restriction to mm_system_information
11/14/02   ks      Added SGSNR info to mm_system_information
11/14/02   ks      Added definition for mm_deep_sleep_pending and setting it
                   to default value.
11/14/02   ks      Changed the default value for mm_sim_valid to TRUE, this
                   is being set to FALSE if gsdi_get_card_mode fails.
11/25/02   mks     Reverted the default value for mm_sim_valid to FALSE. The flag
                   will be set to TRUE if gsdi_get_card_mode returns success
                   upon receiving SIM inserted event.
12/12/02   ks      Replaced mm_sim_valid with mm_serving_plmn.info.sim_state.
12/16/02   jca     Renamed mm_waiting_to_send_reg_cnf to mm_ready_to_send_reg_cnf.
12/17/02   cd      Changed initialization of SIM state to SYS_SIM_STATE_NOT_AVAILABLE
02/07/03   jca     Added boolean mm_dual_mode.
02/13/03   jca     Added boolean mm_waiting_for_service_cnf.
02/21/03   vdr     Added support for SMS Bearer Selection feature
03/07/03   mks     Initialize the ps_data_suspend flag to FALSE in mm_serving_plmn.info
04/03/03   cd      Updated initialization of mm_serving_plmn structure to account for
                   the equivalent plmn list
08/28/03   ks      Reading NV_FORCE_UE_SGSNR_R99_I and NV_FORCE_UE_MSCRR_R99_I
                   from NV during initialization.
09/10/03   ks      re-initializing NV_FORCE_UE_SGSNR_R99_I and NV_FORCE_UE_MSCRR_R99_I.
09/15/03   cd      Added get_preferred_band_class() and get_active_band()
01/16/03   ks      Removed the FEATURE_GSM_GPRS around FORCE_SGSNR_99 and
                   FORCE_MSCR_R99, as this was causing problem in setting the
                   right value in case of single mode operation.
01/28/04   jca     Added regional variables mm_previous_serving_lai and
                   mm_previous_idle_substate.
04/06/04   cd      Added initialization of global mm_rat_handler_status
04/06/04   cd      LCS support
05/07/04   ks      Replaced the data type from boolean to ue_force_rel_mode_type
                   for variables mm_nv_force_sgsnr_r99 & mm_nv_force_mscr_r99
05/12/04   cd      Added support for WCDMA multiband feature
                   Fixed lint errors.
                   Removed obsolete function mm_get_active_rat()
05/14/04   cd      Enhancements to CS FOR
06/01/04   jca     Added extern boolean mm_needs_to_send_mmr_service_ind.
06/08/04   cd      LCS support
07/21/04   ks      Now setting the UE revision level to R99 by default.
07/30/04   jca     Added regional variable mm_band_pref.
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
08/18/04   cd      Setting the default revision level to R99
09/17/04   cd      Corrected type in ERR msg reporting setting the MSCR to R99
04/14/05   jca     Added boolean flag mm_waiting_for_stop_mode_cnf.
04/20/05   jca     Added boolean flag mm_waiting_for_deact_cnf.
05/30/05   abe     Added boolean flag mm_sms_is_active, which is used to release
                   the PS session after getting the MMCNM_REL_REQ
06/06/05   abe     Added code for CCBS support
07/20/05   abe     Declared and initialized the variable mm_previous_lai_during_wtog_cco
                   for FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER.
08/08/05   abe     Definition for the variable mm_hplmn_from_imsi
                   is removed for the feature FEATURE_ENHANCED_NW_SELECTION.
08/14/05   hj      Added handling for monitoring CCCH paging channel when detached from
                   GPRS Services.
09/09/05   ajt     Added definition of mm_prev_cell_access  
10/08/05   abe     Added support for FEATURE_INTERRAT_PCCO_GTOW                  
01/18/06    up      Use NV_REL_MODE_MAX instead of NV_FORCE_UE_DYNAMIC for
                    MAX value checking.
01/30/06   hj      Added support for CS_ONLY and PS_ONLY routing options for SMS.
04/06/06   sn      Updated mm_initialize to initialise DTM status.
06/07/06   ss       Added definition for is_WtoGCCO_in_prgress
06/23/06   up      mm_gs_association_established is added.
07/14/06   pk    Updated for Feature DSAC
03/21/07   NR      Removing mm_cnm_service_type global variable
05/21/07   NR      Adding new global for storing call_type which is to be sent in RRC_EST_REQ  
26/12/07   jbk     Setting Default NAS release compliance behavior to Rel5 
12/18/09   RI      Updates for combined EFS/NV/USIM reads for multimode.
07/05/11   rajesh  Defined and initialised "mm_service_ind_blocked_due_to_cs_reg"
08/03/11   jbk      NV support to disable CB duplicate detection.
08/23/11   PM      Defined and set value from EFS for global variable preserve_ui_speech_codec_preference
10/01/11   abhi  resetting CSFB state
10/04/11   sla     Foreground search list LAI,RAC are initialized
10/12/11   sla     Defined the variable mm_is_foreground_search_to_be_initiated
                   and initialized to zero
10/28/11   rajesh  defined "mm_lai_change_force_lau_for_emergency".
12/06/11   gps     Remembering DSAC information of the cell where last registration was attempted
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stringl/stringl.h>
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "aconfig.h"
#include "mmrat.h"
#include "mm_v.h"
#include "mm_as_types.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "err.h"
#include "naslog_v.h"
#include "ghdi_nvmem.h"
#include "ghdi_nvmem_v.h"
#include "mm_coord.h"
#include "gmmllcm.h"
#include "fs_lib.h"
#include "cm.h"
#include "mcfg_fs.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

#include "mmsecurity.h"
#include "mmauth.h"
#include "gmm_msg.h"
#include "reg_mm.h"
#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#include "atomic_ops.h"
#endif
#include "mm_sim.h"
#include "dog_hb_rex.h"
#include "ULogFront.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
extern ULogHandle mm_get_ulog_handle();

/*   The EFS item would indicate a 0x0 or 0x1 for the als value to be used     */
/*   The value 0x0 => 4   and 0x1  =>  6 */

/* NV Variable which is User Configured through UI, about his AMR preference */
/* -------------AMR CODEC  USER PREFERENCES-- ------------------*/                    
/*                                  -----  BITMAP  ------                                         */          
/* --------------------------------------------------------------*/                    
/* |  08   |  07  |   06  |  05  |       04     |      03      |     02      |     01     |  */   
/* --------------------------------------------------------------*/          
/* |  XX..|  XX..|  XX..|  XX  | WCDMA   |     GSM    |    GSM    |   GSM    |  */          
/* |  XX..|  XX..|  XX..|  XX  | AMR  WB  |  HR AMR  | AMR WB | AMR NB |   */          
/* ---------------------------------------------------------------*/                    
byte                      gsm_wcdma_amr_codec_preference = 0x0;

/* mm_as_id is used stack Id*/
mm_as_id_e_type mm_as_id = MM_AS_ID_1;
mm_as_id_e_type mm_sub_id = MM_AS_ID_1;

boolean mm_emm_is_qrd_device = FALSE;

boolean mm_is_qrd_device = FALSE;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
/* Flag to indicate whether IMSI stored in NV is same as the 
   one in USIM/SIM 
*/
boolean      mm_imsi_is_native_sim[MAX_NAS_STACKS];
#define      mm_imsi_is_native mm_imsi_is_native_sim[mm_as_id]
#ifdef FEATURE_RAT_PRIORITY_LIST
sys_rat_pri_list_info_s_type    mm_rat_pri_list_info_sim[MAX_NAS_STACKS];
#define mm_rat_pri_list_info mm_rat_pri_list_info_sim[mm_as_id]
#endif
serving_plmn_info_T       mm_serving_plmn_sim[MAX_NAS_STACKS];
byte                      mm_timer_status_sim[MAX_NAS_STACKS][MAX_MM_TIMER+1];

byte     gmm_attach_fail_counter_sim[MAX_NAS_STACKS];
#define  gmm_attach_fail_counter gmm_attach_fail_counter_sim[mm_as_id]

boolean  gmm_trigger_attach_on_ps_call_sim[MAX_NAS_STACKS];
#define  gmm_trigger_attach_on_ps_call gmm_trigger_attach_on_ps_call_sim[mm_as_id]

boolean  mm_psm_ready_req_rejected_sim[MAX_NAS_STACKS];
#define  mm_psm_ready_req_rejected mm_psm_ready_req_rejected_sim[mm_as_id]

boolean  mm_psm_r12_enabled_sim[MAX_NAS_STACKS];
#define  mm_psm_r12_enabled mm_psm_r12_enabled_sim[mm_as_id]

uint32  mm_psm_active_timer_sim[MAX_NAS_STACKS];
#define mm_psm_active_timer mm_psm_active_timer_sim[mm_as_id]

uint32  mm_psm_gprs_ready_timer_sim[MAX_NAS_STACKS];
#define mm_psm_gprs_ready_timer mm_psm_gprs_ready_timer_sim[mm_as_id]

uint64  mm_psm_periodic_tau_update_timer_sim[MAX_NAS_STACKS];
#define mm_psm_periodic_tau_update_timer mm_psm_periodic_tau_update_timer_sim[mm_as_id]

uint64  mm_psm_periodic_rau_update_timer_sim[MAX_NAS_STACKS];
#define mm_psm_periodic_rau_update_timer mm_psm_periodic_rau_update_timer_sim[mm_as_id]

boolean  mm_psm_restore_pending_sim[MAX_NAS_STACKS];
#define mm_psm_restore_pending mm_psm_restore_pending_sim[mm_as_id]

uint32  mm_psm_nv_list_bitmask_sim[MAX_NAS_STACKS];
#define mm_psm_nv_list_bitmask mm_psm_nv_list_bitmask_sim[mm_as_id]

mm_psm_cache_s_type *mm_psm_cache_ptr_sim[MAX_NAS_STACKS];
#define mm_psm_cache_ptr mm_psm_cache_ptr_sim[mm_as_id]

boolean mm_psm_mode_req_received_sim[MAX_NAS_STACKS];
#define mm_psm_mode_req_received mm_psm_mode_req_received_sim[mm_as_id]

nas_edrx_config_s_type mm_edrx_cfg_sim[MAX_NAS_STACKS];
#define mm_edrx_cfg mm_edrx_cfg_sim[mm_as_id]

sys_radio_access_tech_e_type mm_last_active_rat_stopped_sim[MAX_NAS_STACKS];
location_information_T    mm_location_information_sim[MAX_NAS_STACKS];
mm_attach_failed_status_type mm_mmr_attach_ind_status_sim[MAX_NAS_STACKS];
/* Flag to indicate whether GSM access is allowed or not */
boolean      auth_gsm_access_sim[MAX_NAS_STACKS];
#define      auth_gsm_access auth_gsm_access_sim[mm_as_id]
uint16       mm_client_activated_group_sim[MAX_NAS_STACKS];
uint16       mm_nas_changed_group_sim[MAX_NAS_STACKS];
boolean      mm_deep_sleep_alt_scan_sim[MAX_NAS_STACKS];
#else
/* Flag to indicate whether IMSI stored in NV is same as the 
   one in USIM/SIM 
*/
boolean      mm_imsi_is_native;
#ifdef FEATURE_RAT_PRIORITY_LIST
sys_rat_pri_list_info_s_type    mm_rat_pri_list_info;
#endif
serving_plmn_info_T       mm_serving_plmn;
byte                      mm_timer_status[MAX_MM_TIMER+1];
boolean  mm_psm_ready_req_rejected;
byte     gmm_attach_fail_counter;
boolean  gmm_trigger_attach_on_ps_call;
boolean  mm_psm_r12_enabled;
uint32   mm_psm_active_timer;
uint32   mm_psm_gprs_ready_timer;
uint64   mm_psm_periodic_tau_update_timer;
uint64   mm_psm_periodic_rau_update_timer;

boolean  mm_psm_restore_pending;
uint32   mm_psm_nv_list_bitmask;
mm_psm_cache_s_type  *mm_psm_cache_ptr;
boolean  mm_psm_mode_req_received;
nas_edrx_config_s_type mm_edrx_cfg;
sys_radio_access_tech_e_type mm_last_active_rat_stopped;
location_information_T    mm_location_information;
mm_attach_failed_status_type mm_mmr_attach_ind_status ;                              
/* Flag to indicate whether GSM access is allowed or not */
boolean      auth_gsm_access;
uint16       mm_client_activated_group;
uint16       mm_nas_changed_group ;
boolean      mm_deep_sleep_alt_scan;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
mm_emm_dos_mitigation_feature_config_T mm_emm_dos_mitigation_feature_config_info_sim[MAX_NAS_STACKS];
#define mm_emm_dos_mitigation_feature_config_info mm_emm_dos_mitigation_feature_config_info_sim[mm_as_id]

uint8   mm_emm_is_t3247_enabled_sim[MAX_NAS_STACKS];
#define mm_emm_is_t3247_enabled mm_emm_is_t3247_enabled_sim[mm_as_id]

uint8  mm_emm_max_attempt_counter_dos_mitigation_sim[MAX_NAS_STACKS];
#define mm_emm_max_attempt_counter_dos_mitigation mm_emm_max_attempt_counter_dos_mitigation_sim[mm_as_id]

uint8  mm_emm_max_fplmn_count_dos_mitigation_sim[MAX_NAS_STACKS];
#define mm_emm_max_fplmn_count_dos_mitigation mm_emm_max_fplmn_count_dos_mitigation_sim[mm_as_id]

uint8   mm_emm_usim_gprs_counter_sim[MAX_NAS_STACKS];
#define mm_emm_usim_gprs_counter mm_emm_usim_gprs_counter_sim[mm_as_id]

uint8   mm_emm_usim_non_gprs_counter_sim[MAX_NAS_STACKS];
#define mm_emm_usim_non_gprs_counter mm_emm_usim_non_gprs_counter_sim[mm_as_id]

uint8  mm_emm_t3247_test_val_sim[MAX_NAS_STACKS];
#define mm_emm_t3247_test_val mm_emm_t3247_test_val_sim[mm_as_id]

boolean mm_is_sim_cs_invalid_due_to_dos_sim[MAX_NAS_STACKS];
#define mm_is_sim_cs_invalid_due_to_dos mm_is_sim_cs_invalid_due_to_dos_sim[mm_as_id]

boolean mm_is_sim_ps_invalid_due_to_dos_sim[MAX_NAS_STACKS];
#define mm_is_sim_ps_invalid_due_to_dos mm_is_sim_ps_invalid_due_to_dos_sim[mm_as_id]

boolean mm_is_sim_to_be_ps_invalid_after_power_off_sim[MAX_NAS_STACKS];
#define mm_is_sim_to_be_ps_invalid_after_power_off mm_is_sim_to_be_ps_invalid_after_power_off_sim[mm_as_id]

boolean mm_is_sim_to_be_cs_invalid_after_power_off_sim[MAX_NAS_STACKS];
#define mm_is_sim_to_be_cs_invalid_after_power_off mm_is_sim_to_be_cs_invalid_after_power_off_sim[mm_as_id]

#if !defined(FEATURE_NAS_CS_DISABLED) || !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
boolean mm_need_to_send_no_srv_on_call_end_due_to_dos_sim[MAX_NAS_STACKS];
#define mm_need_to_send_no_srv_on_call_end_due_to_dos mm_need_to_send_no_srv_on_call_end_due_to_dos_sim[mm_as_id]
#endif

#else

mm_emm_dos_mitigation_feature_config_T mm_emm_dos_mitigation_feature_config_info;
uint8   mm_emm_is_t3247_enabled;
uint8  mm_emm_max_attempt_counter_dos_mitigation;
uint8  mm_emm_max_fplmn_count_dos_mitigation;
uint8   mm_emm_usim_gprs_counter;
uint8   mm_emm_usim_non_gprs_counter;
uint8  mm_emm_t3247_test_val;

boolean mm_is_sim_cs_invalid_due_to_dos = FALSE;
boolean mm_is_sim_ps_invalid_due_to_dos = FALSE;
boolean mm_is_sim_to_be_ps_invalid_after_power_off = FALSE;
boolean mm_is_sim_to_be_cs_invalid_after_power_off = FALSE;

#if !defined(FEATURE_NAS_CS_DISABLED) || !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
boolean mm_need_to_send_no_srv_on_call_end_due_to_dos = FALSE;
#endif

#endif

#ifdef FEATURE_DUAL_SIM
ue_nas_rel_compliance_type mm_nv_nas_rel_compliance_sim[MAX_AS_IDS];
#define mm_nv_nas_rel_compliance mm_nv_nas_rel_compliance_sim[mm_sub_id]

boolean                    mm_map_rej_cause12_to_rej_cause15_sim[MAX_AS_IDS];
#define mm_map_rej_cause12_to_rej_cause15 mm_map_rej_cause12_to_rej_cause15_sim[mm_sub_id]

#else
ue_nas_rel_compliance_type mm_nv_nas_rel_compliance = NV_NAS_COMPLIANCE_REL10;
boolean                    mm_map_rej_cause12_to_rej_cause15;
#endif



boolean mm_conn_mode_manual_search_enabled = FALSE;
boolean mm_nv_conn_mode_manual_search = FALSE;

volatile mm_emm_debug_buffer_s_type mm_emm_debug_buffer[MM_DEBUG_MESSAGE_BUFFER_SIZE];
dword mm_emm_debug_buffer_index = 0;

byte             mm_msg_info;
sys_manual_search_timer_value_type mm_emm_manual_search_timer_value; 
boolean mm_partial_search_allowed = FALSE;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
boolean mm_ps_auth_sent_to_card_sim[MAX_NAS_STACKS];
imsi_data_T               mm_stored_imsi_sim[MAX_NAS_STACKS];
#else
boolean mm_ps_auth_sent_to_card;
imsi_data_T               mm_stored_imsi;
#endif /*FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
boolean mm_cs_auth_sent_to_card_sim[MAX_NAS_STACKS];
#else
boolean mm_cs_auth_sent_to_card;
#endif /*FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/


boolean full_srv_req_in_emerg;
boolean edrx_gsm_fake_negotiation;

#ifdef FEATURE_NAS_EAB
boolean nas_gsm_eab_support;
#endif

//Define the MM REX critical section variable
rex_crit_sect_type  mm_ftd_crit_sect;

#if defined(FEATURE_SGLTE) || defined(FEATURE_DUAL_SIM) || defined(FEATURE_1XSRLTE) || defined(FEATURE_SVLTE_DUAL_SIM)
sys_ue_mode_e_type mm_current_ue_mode =SYS_UE_MODE_NONE;
#endif

#ifdef FEATURE_NAS_REL11
mm_nas_nv_sim_parameters_T  mm_nas_nv_sim_hfns;
#endif


// Define the TIMER REX critical section variable
rex_crit_sect_type  timer_crit_sect;

// Define the NV REX critical section variable
rex_crit_sect_type  nas_nv_crit_sect;

/* putting out of dual sim so that can be used for single sim as well  */
ghdi_nvmem_status_T mm_nvmem_default_values_generator(
       ghdi_nvmem_status_T in_status, nv_items_enum_type item_type, nv_item_type *nv_item);

#if defined(FEATURE_LTE)
#if defined(FEATURE_3G_USER_NW_REDIR_TO_LTE) || defined(FEATURE_LTE_REL12)
uint32 mm_extend_lte_disable_duration;
#endif
#endif

byte mm_emm_read_nv;

/*
 * -------- global variable preserve_ui_speech_codec_preference --------------
 *
 * This flag tells whether or not to preserve speech codec preferences given by UI.
 * This will be set after reading EFS. This is done inside mm_init()
 */
byte                      preserve_ui_speech_codec_preference = 0; 

/* -----------------   QRD CSVT call NV item. -------------- 
 *
 * This indicates whether device is QRD device or not
 */
byte                      nas_is_qmss_enabled                 = 0;

mm_system_information_T   mm_system_information;

/* Variables to control the UE revision level behavior based on network support */
#ifdef FEATURE_DUAL_SIM
ue_force_rel_mode_type     mm_nv_force_sgsnr_r99_sim[MAX_AS_IDS];
#define mm_nv_force_sgsnr_r99 mm_nv_force_sgsnr_r99_sim[mm_sub_id]

ue_force_rel_mode_type     mm_nv_force_mscr_r99_sim[MAX_AS_IDS];
#define mm_nv_force_mscr_r99 mm_nv_force_mscr_r99_sim[mm_sub_id]
#else
ue_force_rel_mode_type     mm_nv_force_sgsnr_r99 = NV_FORCE_UE_REL99;
ue_force_rel_mode_type     mm_nv_force_mscr_r99 = NV_FORCE_UE_REL99;
#endif

#ifdef FEATURE_NAS_GW
#ifdef FEATURE_SGLTE
typedef struct forbidden_list_tag
{
   inter_task_lai_T        lai[MAX_NO_FORBIDDEN_LAIS];
   byte                    write_ptr;
} forbidden_list_T;
#ifndef FEATURE_NAS_CS_DISABLED
void extern mm_copy_auth_cs_database(mm_as_id_e_type dest,
                                                   mm_as_id_e_type src);
#endif
extern boolean mm_is_cs_timer(timer_id_T timer_id);

extern mm_cmd_type mmcoord_pending_reg_message_sim[MAX_NAS_STACKS];
extern mm_cmd_type mmcoord_pending_reg_list_req_message_sim[MAX_NAS_STACKS];
extern mm_cmd_type mmcoord_pending_svc_cnf_message_sim[MAX_NAS_STACKS];
extern byte auth_gsm_cksn_sim[MAX_NAS_STACKS];
extern boolean mmcoord_reg_req_pending_sim[MAX_NAS_STACKS];
extern boolean mm_plmn_list_req_pending_sim[MAX_NAS_STACKS];
extern boolean mm_plmn_search_in_progress_sim[MAX_NAS_STACKS];
extern boolean pending_mm_msg_after_release_sim[MAX_NAS_STACKS];
extern boolean mm_cs_domain_possible_reest_sim[MAX_NAS_STACKS];
extern boolean pending_mm_msg_valid_sim[MAX_NAS_STACKS];
extern boolean send_sequence_number_mm_cc_ss_sim[MAX_NAS_STACKS];
extern forbidden_list_T forbidden_list_sim[MAX_NAS_STACKS];
extern lai_reject_list_T lai_reject_list_sim[MAX_NAS_STACKS];
extern byte lu_cause_sim[MAX_NAS_STACKS];
boolean mmcoord_svc_cnf_pending_sim[MAX_NAS_STACKS];
#endif

#if defined(FEATURE_WRLF_SYSTEM_SEL) && defined(FEATURE_NAS_GW)
#ifdef FEATURE_MODEM_HEAP
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern serving_plmn_info_T *mm_wrlf_serving_plmn_ptr_sim[MAX_NAS_STACKS];
#define mm_wrlf_serving_plmn_ptr mm_wrlf_serving_plmn_ptr_sim[mm_as_id]
#else
extern serving_plmn_info_T *mm_wrlf_serving_plmn_ptr;
#endif
#endif
#endif
/*
 * List of global variables
 */
#ifdef FEATURE_SGLTE
mm_sglte_stack_activity_status mm_sglte_current_stack_status = SGLTE_INACTIVE_STATE;
#endif

#ifdef FEATURE_DUAL_SIM
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read_per_subs(a,b,mm_nv_context_id)
#else
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read(a,b)
#endif

#ifdef FEATURE_MODEM_HEAP
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern boolean pending_mm_msg_valid_sim[MAX_NAS_STACKS];
#else
extern boolean pending_mm_msg_valid;
#endif
#endif


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
mm_as_id_e_type mm_cs_stack_id = MM_AS_ID_1;
mm_as_id_e_type mm_ps_stack_id = MM_AS_ID_1;

byte                      mm_state_sim[MAX_NAS_STACKS];
byte                      mm_idle_substate_sim[MAX_NAS_STACKS];
IE_info_T                 mm_IE_info_sim[MAX_NAS_STACKS][MAXNO_IES_IN_MESSAGE];
byte                      mm_no_of_ies_sim[MAX_NAS_STACKS];
mm_system_information_T   mm_system_information_sim[MAX_NAS_STACKS];
mm_last_reg_dsac_information_T mm_last_reg_dsac_information_sim[MAX_NAS_STACKS];
boolean                   mm_llc_needs_to_be_resumed_sim[MAX_NAS_STACKS];
boolean                   mm_nreg_req_received_sim[MAX_NAS_STACKS];
boolean                   mm_cnm_rel_req_received_sim[MAX_NAS_STACKS];
boolean                   mm_present_all_plmns_sim[MAX_NAS_STACKS];
held_service_request_T    mm_held_cnm_service_req_sim[MAX_NAS_STACKS];
boolean                   mm_reg_waiting_for_reg_cnf_sim[MAX_NAS_STACKS];
boolean                   mm_ready_to_send_reg_cnf_sim[MAX_NAS_STACKS];
boolean                   mm_needs_to_send_mmr_service_ind_sim[MAX_NAS_STACKS];
boolean                   mm_waiting_for_service_cnf_sim[MAX_NAS_STACKS];
boolean                   mm_waiting_for_stop_mode_cnf_sim[MAX_NAS_STACKS];
boolean                   mm_waiting_for_deact_cnf_sim[MAX_NAS_STACKS];
boolean                   mm_waiting_for_ph_status_change_cnf_sim[MAX_NAS_STACKS];
lu_reject_information_T   mm_lu_reject_info_sim[MAX_NAS_STACKS];
byte                      mm_serving_lai_sim[MAX_NAS_STACKS][LAI_SIZE];
byte                      mm_lu_attempt_counter_sim[MAX_NAS_STACKS];
byte                      mm_pending_lu_request_sim[MAX_NAS_STACKS];
byte                      mm_pending_no_service_to_reg_sim[MAX_NAS_STACKS];
byte                      mm_lu_start_reason_sim[MAX_NAS_STACKS];
byte                      mm_cipher_key_sequence_number_sim[MAX_NAS_STACKS];
boolean                   mm_follow_on_request_sim[MAX_NAS_STACKS];
boolean                   mm_follow_on_proceed_sim[MAX_NAS_STACKS];
byte                      mm_reject_cause_sim[MAX_NAS_STACKS];
boolean                   mm_cnm_is_active_sim[MAX_NAS_STACKS];
boolean                   mm_sms_is_active_sim[MAX_NAS_STACKS];
boolean                   mm_T3213_timed_out_sim[MAX_NAS_STACKS];
byte                      mm_idle_transition_timer_sim[MAX_NAS_STACKS];
boolean                   mm_possible_imsi_attach_sim[MAX_NAS_STACKS];
ef_kc_T                   mm_ef_kc_sim[MAX_NAS_STACKS];
boolean                   mm_ef_kc_valid_sim[MAX_NAS_STACKS];
uint32                    mm_op_mode_sim[MAX_NAS_STACKS];
boolean                   mm_sig_low_priority_sim[MAX_NAS_STACKS];
boolean                   mm_nreg_release_pending_sim[MAX_NAS_STACKS];
boolean                   mm_integrity_protected_sim[MAX_NAS_STACKS];
boolean                   mm_gsm_supported;
boolean                   mm_wcdma_supported;
boolean                   mm_tdscdma_supported;

sys_radio_access_tech_e_type  mm_ue_pref_rat_sim[MAX_NAS_STACKS];
boolean                   mm_rat_change_pending_sim[MAX_NAS_STACKS];
boolean                   mm_serv_change_pending_sim[MAX_NAS_STACKS];
boolean                   mm_to_only_reg_sim[MAX_NAS_STACKS];
sys_radio_access_tech_e_type mm_previous_serving_rat_sim[MAX_NAS_STACKS];
boolean                   mm_dual_mode_sim[MAX_NAS_STACKS];
sys_band_mask_type        mm_band_pref_sim[MAX_NAS_STACKS];
establishment_cause_T     mm_cnm_srv_est_cause_sim[MAX_NAS_STACKS];

rrc_lo_access_class_e_type mm_lo_access_class_sim[MAX_NAS_STACKS];
rrc_hi_access_class_type   mm_hi_access_class_sim[MAX_NAS_STACKS];
boolean                    mm_deep_sleep_pending_sim[MAX_NAS_STACKS] ;
boolean                   mm_hard_abort_pending_sim[MAX_NAS_STACKS];
boolean                   mm_hard_abort_ue_sglte_mode_switch_pending_sim[MAX_NAS_STACKS];
mm_connection_type_T      mm_connection_type_sim[MAX_NAS_STACKS];

mm_pended_stop_mode_req_s_type mm_pended_stop_mode_req_sim[MAX_NAS_STACKS];
#define mm_pended_stop_mode_req mm_pended_stop_mode_req_sim[mm_as_id]

boolean                    mm_waiting_for_csmt_response_sim[MAX_NAS_STACKS];


#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
sys_band_mask_type         mm_ue_band_class_pref_sim[MAX_NAS_STACKS];
sys_band_mask_e_type       mm_active_band_sim[MAX_NAS_STACKS];
#endif

sys_band_class_e_type     mm_band_indicator_sim[MAX_NAS_STACKS];
byte                      mm_previous_serving_lai_sim[MAX_NAS_STACKS][LAI_SIZE];

byte                      mm_gstk_previous_serving_lai_sim[MAX_NAS_STACKS][LAI_SIZE];

byte                      mm_previous_idle_substate_sim[MAX_NAS_STACKS];
mm_as_cell_access_status_T mm_prev_cell_access_sim[MAX_NAS_STACKS];

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
boolean           mm_lcs_active_sim[MAX_NAS_STACKS];
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

#ifdef FEATURE_CCBS
boolean           mm_supports_NIMO_CM_connection_sim[MAX_NAS_STACKS];
#endif

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
WtoGCCO_in_progress_enum_T  WtoGCCO_state_sim[MAX_NAS_STACKS];
#endif

#ifdef FEATURE_INTERRAT_PCCO_GTOW
WtoGCCO_in_progress_enum_T  GtoWCCO_state_sim[MAX_NAS_STACKS];
#endif

mm_rat_handler_status_type mm_rat_handler_status_sim[MAX_NAS_STACKS];

/* Used to remember if combine procedure is performed last to make MSC-GS association */
boolean mm_gs_association_established_sim[MAX_NAS_STACKS];
boolean mm_initiate_lu_upon_entry_into_new_cell_sim[MAX_NAS_STACKS];
uint32 mm_serving_cell_id_sim[MAX_NAS_STACKS];
boolean mm_serving_cell_valid_sim[MAX_NAS_STACKS];
uint32 mm_lu_failed_cell_id_sim[MAX_NAS_STACKS];

#ifdef FEATURE_DSAC
boolean                    mm_ps_access_barred_to_unbarred_sim[MAX_NAS_STACKS];
boolean                    mm_cs_access_barred_to_unbarred_sim[MAX_NAS_STACKS];
byte                       mm_last_procedure_to_update_registration_sim[MAX_NAS_STACKS];

#define  mm_ps_access_barred_to_unbarred (mm_ps_access_barred_to_unbarred_sim[mm_as_id])
#define  mm_cs_access_barred_to_unbarred (mm_cs_access_barred_to_unbarred_sim[mm_as_id])
#define  mm_last_procedure_to_update_registration (mm_last_procedure_to_update_registration_sim[mm_as_id])
#endif 

boolean                 mm_manual_mode_user_selection_procedure_sim[MAX_NAS_STACKS];

#ifdef FEATURE_ENHANCED_NW_SELECTION
boolean                    mm_force_lu_sim[MAX_NAS_STACKS];
dword                      mm_foreground_search_timer_value_sim[MAX_NAS_STACKS];
mm_lai_timer_info_list_info_T mm_foreground_search_lai_timer_list_sim[MAX_NAS_STACKS];
#endif

boolean                    mm_managed_roaming_enabled_sim[MAX_AS_IDS];

/******            mm_controls_resel_dec_mt_cs_call      ********
 *  In case of CS MT call, this flag indicates to MM if MM is in 
 *  control of allowing/disallowing Reselection when asked by RRC
 *  */
byte mm_controls_resel_dec_mt_cs_call_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define mm_controls_resel_dec_mt_cs_call mm_controls_resel_dec_mt_cs_call_sim[mm_as_id]

/* defined in ms_cc_mm.h file this should be in synchronous with the ones defined in RRC 
   used for storing call_type sent by CC, which should be passed in RRC_EST_REQ for CS Domain */
call_type_for_rrc_est_req_T call_type_for_rrc_est_req_sim[MAX_NAS_STACKS];

/* DUAL SIM : Flag if HARD ABORT is pending for DUAL SWITCH */
sys_stop_mode_reason_e_type                   mm_abort_dual_switch_pending_sim[MAX_NAS_STACKS];

#ifdef FEATURE_TRIPLE_SIM
uint8 mm_nv_context_id_sim[MAX_AS_IDS] = {MM_AS_ID_1, MM_AS_ID_2, MM_AS_ID_3};
#else
uint8 mm_nv_context_id_sim[MAX_AS_IDS] = {MM_AS_ID_1, MM_AS_ID_2};
#endif

#define mm_nv_context_id (mm_nv_context_id_sim[mm_sub_id])

boolean      mm_mmr_cell_service_ind_pending_sim[MAX_NAS_STACKS];

#ifdef FEATURE_RAT_PRIORITY_LIST
#ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
sys_rat_pri_list_info_s_type    mm_rat_pri_list_info_to_nw_sim[MAX_NAS_STACKS];
#define mm_rat_pri_list_info_to_nw mm_rat_pri_list_info_to_nw_sim[mm_as_id]
#endif
#endif /* FEATURE_RAT_PRIORITY_LIST */

/* SIM/USIM access class information to be sent to RR/RRC */
byte         mm_sim_access_class_sim[MAX_NAS_STACKS][MM_SIM_EFACC_SIZE];
#define      mm_sim_access_class mm_sim_access_class_sim[mm_as_id]

/*LU reject cause for each failure attempt */
byte         mm_lu_reject_cause_sim[MAX_NAS_STACKS][MAX_LU_ATTEMPT_SIZE];
#define      mm_lu_reject_cause mm_lu_reject_cause_sim[mm_as_id]

/* GSM security context for USIM */
boolean      auth_usim_gsm_security_context_sim[MAX_NAS_STACKS] ;
#define      auth_usim_gsm_security_context auth_usim_gsm_security_context_sim[mm_as_id]


/* Mobile operation mode */
byte         mm_ms_opmode_sim[MAX_NAS_STACKS];
#define      mm_ms_opmode mm_ms_opmode_sim[mm_as_id]

/* 2 or 3 digit MNC */
byte         mm_mnc_length_sim[MAX_NAS_STACKS];
#define      mm_mnc_length mm_mnc_length_sim[mm_as_id]

/* Flag to remember last network selection mode send to RR/RRC */
sys_network_selection_mode_e_type last_requested_network_selection_mode_sim[MAX_NAS_STACKS];
#define      last_requested_network_selection_mode last_requested_network_selection_mode_sim[mm_as_id]

#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
byte mm_pending_lu_type_sim[MAX_NAS_STACKS];
#define mm_pending_lu_type mm_pending_lu_type_sim[mm_as_id]
#endif

mmr_cause_e_type mm_reg_cause_sim[MAX_NAS_STACKS];

#define mm_connection_type   mm_connection_type_sim[mm_as_id]

#ifdef FEATURE_TC
boolean mm_tc_is_active_sim[MAX_NAS_STACKS];
#define mm_tc_is_active mm_tc_is_active_sim[mm_as_id]
#endif

boolean mm_service_ind_blocked_due_to_cs_reg_sim[MAX_NAS_STACKS];
#define mm_service_ind_blocked_due_to_cs_reg mm_service_ind_blocked_due_to_cs_reg_sim[mm_as_id]

boolean mm_service_ind_blocked_due_to_irat_cs_reg_sim[MAX_NAS_STACKS];
#define mm_service_ind_blocked_due_to_irat_cs_reg mm_service_ind_blocked_due_to_irat_cs_reg_sim[mm_as_id]

boolean mm_connection_activated_sim[MAX_NAS_STACKS]; 
#define mm_connection_activated mm_connection_activated_sim[mm_as_id]

mm_csfb_service_ind_status_enum_T mm_csfb_service_ind_status_sim[MAX_NAS_STACKS];
#define mm_csfb_service_ind_status mm_csfb_service_ind_status_sim[mm_as_id]

boolean mm_is_foreground_search_to_be_initiated_sim[MAX_NAS_STACKS];
#define mm_is_foreground_search_to_be_initiated mm_is_foreground_search_to_be_initiated_sim[mm_as_id]

boolean mm_is_gmm_moved_dereg_due_to_csreg_sim[MAX_NAS_STACKS];
#define mm_is_gmm_moved_dereg_due_to_csreg  mm_is_gmm_moved_dereg_due_to_csreg_sim[mm_as_id]

#ifdef FEATURE_NAS_REL10
sys_plmn_id_s_type mm_3gpp_t3246_plmn_id_sim[MAX_NAS_STACKS];
#define mm_3gpp_t3246_plmn_id mm_3gpp_t3246_plmn_id_sim[mm_as_id]

boolean mm_store_integrity_protected_sim[MAX_NAS_STACKS];
#define mm_store_integrity_protected mm_store_integrity_protected_sim[mm_as_id]

dword   mm_t3246_backoff_timer_value_sim[MAX_NAS_STACKS];
#define mm_t3246_backoff_timer_value  mm_t3246_backoff_timer_value_sim[mm_as_id]
#endif

#ifdef FEATURE_LTE
boolean mm_csfb_is_blind_redir_to_lte_req_to_be_sent_sim[MAX_NAS_STACKS];
#define mm_csfb_is_blind_redir_to_lte_req_to_be_sent mm_csfb_is_blind_redir_to_lte_req_to_be_sent_sim[mm_as_id]

boolean mm_stop_mode_local_detach_peding_sim[MAX_NAS_STACKS];
#define mm_stop_mode_local_detach_peding mm_stop_mode_local_detach_peding_sim[mm_as_id]

boolean mm_stop_mode_local_detach_peding_sim[MAX_NAS_STACKS];
#define mm_stop_mode_local_detach_peding mm_stop_mode_local_detach_peding_sim[mm_as_id]

sys_ps_detach_e_type mm_ps_detach_type_sim[MAX_NAS_STACKS];
#define mm_ps_detach_type mm_ps_detach_type_sim[mm_as_id]
#endif /* FEATURE_LTE */


#ifdef FEATURE_NAS_REL10
boolean mm_is_per_ms_t3212_present_sim[MAX_NAS_STACKS];
#define mm_is_per_ms_t3212_present  mm_is_per_ms_t3212_present_sim[mm_as_id]

uint64   mm_stored_per_ms_t3212_value_sim[MAX_NAS_STACKS];
#define mm_stored_per_ms_t3212_value mm_stored_per_ms_t3212_value_sim[mm_as_id]
#endif

#if defined(FEATURE_WRLF_SYSTEM_SEL) && defined(FEATURE_NAS_GW)
boolean                   mm_received_suitable_search_end_ind_sim[MAX_NAS_STACKS];
#define mm_received_suitable_search_end_ind mm_received_suitable_search_end_ind_sim[mm_as_id]
#endif

word mm_coord_camped_cell_id_sim[MAX_NAS_STACKS];

#ifdef FEATURE_NAS_REL11
#ifdef FEATURE_WCDMA
boolean  mm_is_first_rrc_connection_attempt_sim[MAX_AS_IDS];
#define  mm_is_first_rrc_connection_attempt mm_is_first_rrc_connection_attempt_sim[mm_sub_id]
#endif
#endif
boolean  mm_is_cs_connected_due_to_srvcc_sim[MAX_NAS_STACKS];
#define  mm_is_cs_connected_due_to_srvcc mm_is_cs_connected_due_to_srvcc_sim[mm_as_id]

#if defined(FEATURE_NAS_REL10) && defined(FEATURE_LTE)
boolean mm_is_lu_to_be_initiated_t3346_ltogw_sim[MAX_NAS_STACKS];
#define mm_is_lu_to_be_initiated_t3346_ltogw mm_is_lu_to_be_initiated_t3346_ltogw_sim[mm_as_id]
#endif

#else


byte                      mm_state;
byte                      mm_idle_substate;
IE_info_T                 mm_IE_info[MAXNO_IES_IN_MESSAGE];
byte                      mm_no_of_ies;
mm_last_reg_dsac_information_T mm_last_reg_dsac_information;
boolean                   mm_llc_needs_to_be_resumed;
mm_csfb_service_ind_status_enum_T mm_csfb_service_ind_status;
boolean                   mm_nreg_req_received;
byte                      mm_pending_no_service_to_reg;
boolean                   mm_cnm_rel_req_received;

/******            mm_controls_resel_dec_mt_cs_call      ********
 *  In case of CS MT call, this flag indicates to MM if MM is in 
 *  control of allowing/disallowing Reselection when asked by RRC
 *  */
byte                      mm_controls_resel_dec_mt_cs_call = FALSE;

boolean                   mm_present_all_plmns;
held_service_request_T    mm_held_cnm_service_req;
boolean                   mm_reg_waiting_for_reg_cnf;
boolean                   mm_ready_to_send_reg_cnf;
boolean                   mm_needs_to_send_mmr_service_ind;
boolean                   mm_waiting_for_service_cnf;
boolean                   mm_waiting_for_stop_mode_cnf;
boolean                   mm_waiting_for_deact_cnf;
boolean                   mm_waiting_for_ph_status_change_cnf;
boolean                   mm_hard_abort_ue_sglte_mode_switch_pending;
lu_reject_information_T   mm_lu_reject_info;
byte                      mm_serving_lai[LAI_SIZE];
byte                      mm_lu_attempt_counter;
byte                      mm_pending_lu_request;
byte                      mm_lu_start_reason;
byte                      mm_cipher_key_sequence_number;
boolean                   mm_follow_on_request;
boolean                   mm_follow_on_proceed;
byte                      mm_reject_cause;
boolean                   mm_cnm_is_active;
boolean                   mm_sms_is_active;
boolean                   mm_T3213_timed_out;
byte                      mm_idle_transition_timer;
boolean                   mm_possible_imsi_attach;
ef_kc_T                   mm_ef_kc;
boolean                   mm_ef_kc_valid;
uint32                    mm_op_mode;
boolean                   mm_sig_low_priority;
boolean                   mm_nreg_release_pending;
boolean                   mm_integrity_protected;
boolean                   mm_gsm_supported;
boolean                   mm_wcdma_supported;
boolean                   mm_tdscdma_supported;
sys_radio_access_tech_e_type  mm_ue_pref_rat;
boolean                   mm_rat_change_pending;
boolean                   mm_serv_change_pending;
boolean                   mm_to_only_reg;
sys_radio_access_tech_e_type mm_previous_serving_rat;

boolean                   mm_dual_mode;
sys_band_mask_type        mm_band_pref;
#ifdef FEATURE_LTE
boolean                   mm_stop_mode_local_detach_peding;
sys_ps_detach_e_type      mm_ps_detach_type;
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
 #ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
sys_rat_pri_list_info_s_type    mm_rat_pri_list_info_to_nw;
#endif
#endif /* FEATURE_RAT_PRIORITY_LIST */

#if defined(FEATURE_LTE)
boolean mm_csfb_is_blind_redir_to_lte_req_to_be_sent;
#endif

establishment_cause_T     mm_cnm_srv_est_cause;

mm_connection_type_T   mm_connection_type;
rrc_lo_access_class_e_type mm_lo_access_class;
rrc_hi_access_class_type   mm_hi_access_class;
boolean                    mm_deep_sleep_pending ;
boolean                   mm_hard_abort_pending;
boolean                   mm_waiting_for_csmt_response;

#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
sys_band_mask_type         mm_ue_band_class_pref;
sys_band_mask_e_type       mm_active_band;
#endif

sys_band_class_e_type     mm_band_indicator;
byte                      mm_previous_serving_lai[LAI_SIZE];

byte                      mm_gstk_previous_serving_lai[LAI_SIZE];

byte                      mm_previous_idle_substate;
mm_as_cell_access_status_T mm_prev_cell_access;

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
boolean           mm_lcs_active;
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

#ifdef FEATURE_CCBS
boolean           mm_supports_NIMO_CM_connection;
#endif

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
WtoGCCO_in_progress_enum_T  WtoGCCO_state;
#endif

#if defined(FEATURE_NAS_REL10) && defined(FEATURE_LTE)
boolean mm_is_lu_to_be_initiated_t3346_ltogw;
#endif

#ifdef FEATURE_INTERRAT_PCCO_GTOW
WtoGCCO_in_progress_enum_T  GtoWCCO_state;
#endif

mm_rat_handler_status_type mm_rat_handler_status;

/* Used to remember if combine procedure is performed last to make MSC-GS association */
boolean mm_gs_association_established;
boolean mm_initiate_lu_upon_entry_into_new_cell;
uint32 mm_serving_cell_id;
boolean mm_serving_cell_valid;
uint32 mm_lu_failed_cell_id;


#ifdef FEATURE_DSAC
boolean                    mm_ps_access_barred_to_unbarred;
boolean                    mm_cs_access_barred_to_unbarred;
byte                       mm_last_procedure_to_update_registration;
#endif 

boolean                 mm_manual_mode_user_selection_procedure;

#ifdef FEATURE_ENHANCED_NW_SELECTION
boolean                    mm_force_lu;
dword                      mm_foreground_search_timer_value;
mm_lai_timer_info_list_info_T mm_foreground_search_lai_timer_list;
#endif

boolean                    mm_managed_roaming_enabled;

/* defined in ms_cc_mm.h file this should be in synchronous with the ones defined in RRC 
   used for storing call_type sent by CC, which should be passed in RRC_EST_REQ for CS Domain */
call_type_for_rrc_est_req_T call_type_for_rrc_est_req;

boolean      mm_mmr_cell_service_ind_pending;

/* SIM/USIM access class information to be sent to RR/RRC */
byte         mm_sim_access_class[MM_SIM_EFACC_SIZE];

/*LU reject cause for each failure attempt */
byte         mm_lu_reject_cause[MAX_LU_ATTEMPT_SIZE];

/* GSM security context for USIM */
boolean      auth_usim_gsm_security_context;

/* Mobile operation mode */
byte         mm_ms_opmode;

/* 2 or 3 digit MNC */
byte         mm_mnc_length;

/* Flag to remember last network selection mode send to RR/RRC */
sys_network_selection_mode_e_type last_requested_network_selection_mode;
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
byte mm_pending_lu_type;
#endif

#ifdef FEATURE_TC
boolean mm_tc_is_active;
#endif

boolean mm_service_ind_blocked_due_to_cs_reg;
boolean mm_service_ind_blocked_due_to_irat_cs_reg; 
boolean mm_connection_activated;
boolean mm_is_foreground_search_to_be_initiated; 

boolean mm_is_gmm_moved_dereg_due_to_csreg;

#ifdef FEATURE_NAS_REL10
sys_plmn_id_s_type mm_3gpp_t3246_plmn_id;
boolean mm_store_integrity_protected             = FALSE;
dword   mm_t3246_backoff_timer_value             = 0;

uint64   mm_stored_per_ms_t3212_value          = 0;
boolean mm_is_per_ms_t3212_present            = FALSE;
#endif
mm_pended_stop_mode_req_s_type mm_pended_stop_mode_req;

#if defined(FEATURE_WRLF_SYSTEM_SEL) && defined(FEATURE_NAS_GW)
boolean    mm_received_suitable_search_end_ind = FALSE ;
#endif /* FEATURE_DUAL_SIM*/

word mm_coord_camped_cell_id;

#ifdef FEATURE_NAS_REL11
#ifdef FEATURE_WCDMA
boolean mm_is_first_rrc_connection_attempt;
#endif
#endif

boolean mm_is_cs_connected_due_to_srvcc;

#endif /*  FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/



#ifdef FEATURE_LTE
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
boolean mm_multimode_rat_change_pending_sim[MAX_NAS_STACKS];
#define mm_multimode_rat_change_pending mm_multimode_rat_change_pending_sim[mm_as_id]
#else
boolean mm_multimode_rat_change_pending;
#endif
#endif


/* this variable is used in classmark information reading from EFS
    Allowed values o and 1, default value 1*/
byte ss_screening_indicator;

/* this variable is used to perform retry for emergency call on access
   class not allowed cell. Allowed values 0 and 1, default value 0 */
byte mm_retry_em_call_on_access_blocked_cell = 0;
#ifdef DISABLE_9x05_OPTIMIZATIONS
#ifdef FEATURE_NAS_REL12
byte mm_optimised_ps_signalling_connection_release = 0x01;
#else
byte mm_optimised_ps_signalling_connection_release = 0;
#endif
#endif
byte mm_ps_sig_conn_rel_after_ps_sms = 0;


byte mm_managed_roaming_retry_lu_in_manual_mode = 0;


byte gmm_psm_local_detach_enabled = 0x01;


byte mm_nv_wcdma_hsupa_category = MM_RRC_WCMDA_HSUPA_DEFAULT_CATEGORY; /*RRC_WCMDA_HSUPA_DEFAULT_CATEGORY*/


/*
 * -------- global variable mmode_nv_setting_for_device_mode --------------
 *
 * This flag tells the choice for this device - whether the device is DSDS or DSDA capable
 * This will be set after reading MMODE EFS - /nv/item_files/modem/mmode/device_mode
 */
sys_modem_device_mode_e_type  mmode_nv_setting_for_device_mode = SYS_MODEM_DEVICE_MODE_SINGLE_SIM;


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
/* use in MN to use appropriate as_id */
boolean      mm_mn_sglte_svlte_mode = FALSE;
#ifdef FEATURE_NAS_ECALL
mmr_ecall_mode_e_type mm_ecall_mode_type_sim[MAX_NAS_STACKS];
boolean mm_need_to_perform_ecall_inactivity_sim[MAX_NAS_STACKS];
byte    mm_call_est_cause_sim[MAX_NAS_STACKS];
uint16  mm_ecall_inactivity_timer_sim[MAX_NAS_STACKS];
#endif
#else
#ifdef FEATURE_NAS_ECALL
mmr_ecall_mode_e_type mm_ecall_mode_type;
boolean mm_need_to_perform_ecall_inactivity;
byte    mm_call_est_cause = NO_CNM_SERVICE;
uint16  mm_ecall_inactivity_timer = 0;  
#endif
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
rex_crit_sect_type  mm_sim_crit_sect;
#endif

mm_nas_feature_to_be_configured_T mm_nas_feature_to_be_configured;
 
#if defined(FEATURE_SGLTE) || defined(FEATURE_SVLTE_DUAL_SIM)
nas_sglte_feature_config_T sglte_nas_mm_config;
#endif

#ifdef FEATURE_GSM_CB
void cb_get_nv_items(mm_nas_nv_items_T*);
#endif
void mn_set_nv_items(mm_nas_feature_to_be_configured_T *);
#ifdef FEATURE_LTE
/*Store the previous RAT info, as this is required to 
  post an event during RAT change (b/w G,W & L)*/
#if defined(FEATURE_SGLTE) || defined(FEATURE_DUAL_SIM)
sys_radio_access_tech_e_type mm_previous_rat_sim[MAX_NAS_STACKS];
#define mm_previous_rat mm_previous_rat_sim[mm_as_id]
byte mm_sglte_nv_enabled =FALSE;
#else
sys_radio_access_tech_e_type mm_previous_rat = SYS_RAT_NONE;
#endif
#endif

#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
sys_overall_feature_t mm_sub_config_sim[MAX_AS_IDS];
#define mm_sub_config mm_sub_config_sim[mm_sub_id]
#endif

#if defined(FEATURE_TRIPLE_SIM)
mm_as_id_e_type mm_sub_id_stack[MAX_NAS_STACKS] = {MM_AS_ID_1, MM_AS_ID_2, MM_AS_ID_3};
#elif defined FEATURE_SGLTE_DUAL_SIM
mm_as_id_e_type mm_sub_id_stack[MAX_NAS_STACKS] = {MM_AS_ID_1, MM_AS_ID_2, MM_AS_ID_1};
#elif defined(FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
mm_as_id_e_type mm_sub_id_stack[MAX_NAS_STACKS] = {MM_AS_ID_1, MM_AS_ID_2};
#endif 

#ifdef FEATURE_SGLTE
mm_sms_bearer_selection_enum_T mm_nv_sms_preferred_domain_id;
#endif

#ifdef FEATURE_DUAL_SIM
mm_sms_bearer_selection_enum_T mm_sms_preferred_domain_id_sim[MAX_AS_IDS];
#else
mm_sms_bearer_selection_enum_T mm_sms_preferred_domain_id;
#endif

#if defined(FEATURE_LTE)
/* This EFS item if set to TRUE will be used to perform LAU for 
   emergency call whenever there is change in LA */
#ifdef FEATURE_DUAL_SIM
boolean mm_lai_change_force_lau_for_emergency_sim[MAX_AS_IDS];
#define mm_lai_change_force_lau_for_emergency mm_lai_change_force_lau_for_emergency_sim[mm_sub_id]
#else
boolean mm_lai_change_force_lau_for_emergency;
#endif
boolean mm_nv_exclude_old_lai_type_ie    = FALSE;
sys_pch_state_s_type mm_rrc_pch_state_info;
#endif

#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
#ifdef FEATURE_DUAL_SIM
mm_utran_ps_voice_parameters_info mm_utran_ps_voice_parameters_sim[MAX_NAS_STACKS];
#define mm_utran_ps_voice_parameters mm_utran_ps_voice_parameters_sim[mm_as_id]
#else
mm_utran_ps_voice_parameters_info mm_utran_ps_voice_parameters;
#endif
#endif

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
sys_csg_id_type     mm_serving_csg_id_sim[MAX_NAS_STACKS] = {SYS_CSG_ID_INVALID, SYS_CSG_ID_INVALID};
#define                   mm_serving_csg_id  mm_serving_csg_id_sim[mm_as_id]
#else
sys_csg_id_type     mm_serving_csg_id = SYS_CSG_ID_INVALID;
#endif /*FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/
#ifdef FEATURE_DISABLE_CSG_BYDEFAULT
sys_csg_support_e_type    mm_nv_csg_support = SYS_CSG_SUPPORT_DISABLED;
#else
sys_csg_support_e_type    mm_nv_csg_support = SYS_CSG_SUPPORT_WCDMA;
#endif
#endif /*FEATURE_FEMTO_CSG*/
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  boolean mm_is_sim_busy_sim[MAX_AS_IDS] = {FALSE,FALSE};
#define mm_is_sim_busy mm_is_sim_busy_sim[mm_as_id]
#else
  boolean mm_is_sim_busy = FALSE;
#endif /*FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/
#endif

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
sys_csg_selection_config_e_type mm_csg_selection_config_sim[MAX_NAS_STACKS] = 
                                               {SYS_CSG_SELECTION_CONFIG_NONE,
                                                SYS_CSG_SELECTION_CONFIG_NONE};
#define mm_csg_selection_config mm_csg_selection_config_sim[mm_as_id]
sys_csg_list_s_type mm_rejected_csg_list_sim[MAX_NAS_STACKS] = {{0}, {0}};
#define mm_rejected_csg_list mm_rejected_csg_list_sim[mm_as_id]
sys_csg_search_type_e_type mm_csg_search_type_sim[MAX_NAS_STACKS] = 
                                                 {SYS_CSG_SEARCH_TYPE_NONE,
                                                  SYS_CSG_SEARCH_TYPE_NONE};
#define mm_csg_search_type mm_csg_search_type_sim[mm_as_id]
#else
sys_csg_selection_config_e_type  mm_csg_selection_config = 
                                                 SYS_CSG_SELECTION_CONFIG_NONE;
sys_csg_list_s_type mm_rejected_csg_list = {0};
sys_csg_search_type_e_type mm_csg_search_type = SYS_CSG_SEARCH_TYPE_NONE;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
#endif /* FEATURE_FEMTO_CSG*/

#ifdef FEATURE_DUAL_DATA
/*This stores that data priroty indicated by each client*/
uint8 mm_data_pri_value_sim[MAX_AS_IDS];
#define mm_data_pri_value mm_data_pri_value_sim[mm_sub_id]
#endif 
#endif

#if (defined FEATURE_LTE_REL12) 
#if defined FEATURE_DUAL_SIM
 uint16 	  mm_emm_service_req_backoff_timer_value_sim[MAX_NAS_STACKS];
#define      mm_emm_service_req_backoff_timer_value mm_emm_service_req_backoff_timer_value_sim[mm_as_id]
#else
 uint16 	  mm_emm_service_req_backoff_timer_value;
#endif 
#endif

byte                      mm_nv_tighter_capability;
byte                      mm_nv_vamos_feature_support;
mmr_cause_e_type mm_reg_cause;

boolean mm_is_gsm_active_rat_for_thermal_read = FALSE;

#ifdef FEATURE_LTE
 typedef PACKED struct PACKED_POST{ 
   boolean	  eps_encrypt_alg_128_eea0_supported; 
   boolean	  eps_encrypt_alg_128_eea1_supported; 
   boolean	  eps_encrypt_alg_128_eea2_supported;
   boolean	  eps_encrypt_alg_eea3_supported;
   boolean	  eps_encrypt_alg_eea4_supported;
   boolean	  eps_encrypt_alg_eea5_supported;
   boolean	  eps_encrypt_alg_eea6_supported;
   boolean	  eps_encrypt_alg_eea7_supported;
   boolean	  eps_integrity_alg_128_eia1_supported; 
   boolean	  eps_integrity_alg_128_eia2_supported; 
   boolean	  eps_integrity_alg_128_eia3_supported; 
   boolean	  eps_integrity_alg_128_eia4_supported; 
   boolean	  eps_integrity_alg_128_eia5_supported; 
   boolean	  eps_integrity_alg_128_eia6_supported; 
   boolean	  eps_integrity_alg_128_eia7_supported; 
   boolean	  umts_encrypt_alg_uea0_supported; 
   boolean	  umts_encrypt_alg_uea1_supported;
   boolean	  umts_encrypt_alg_uea2_supported;
   boolean	  umts_encrypt_alg_uea3_supported;
   boolean	  umts_encrypt_alg_uea4_supported;
   boolean	  umts_encrypt_alg_uea5_supported;
   boolean	  umts_encrypt_alg_uea6_supported;
   boolean	  umts_encrypt_alg_uea7_supported;
   boolean	  ucs2_supported; 
   boolean	  umts_integrity_alg_uia1_supported; 
   boolean	  umts_integrity_alg_uia2_supported;
   boolean	  umts_integrity_alg_uia3_supported;
   boolean	  umts_integrity_alg_uia4_supported;
   boolean	  umts_integrity_alg_uia5_supported;
   boolean	  umts_integrity_alg_uia6_supported;
   boolean	  umts_integrity_alg_uia7_supported;
   boolean	  gprs_encrypt_alg_1_supported; 
   boolean	  gprs_encrypt_alg_2_supported; 
   boolean	  gprs_encrypt_alg_3_supported; 
   boolean	  gprs_encrypt_alg_4_supported; 
   boolean	  gprs_encrypt_alg_5_supported; 
   boolean	  gprs_encrypt_alg_6_supported; 
   boolean	  gprs_encrypt_alg_7_supported; 
 
 } ue_security_capability;

extern boolean emm_disable_geran_cap ;

#endif


extern void emm_set_and_write_tin_type(nas_mm_tin_type tin);



 /*===========================================================================
FUNCTION  mm_read_carrier_specific_efs

DESCRIPTION
  This function reads carrier specific EFS/NV values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_read_carrier_specific_efs(void)
{
  mcfg_fs_status_e_type  status = MCFG_FS_STATUS_EFS_ERR;

    /* CONN MODE MANUAL SEARCH */
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/nas/conn_mode_manual_search", 
                         &mm_nv_conn_mode_manual_search, sizeof(boolean),
                         MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

  if ((mm_emm_read_nv & (1<<MM_MCFG_REFRESH)))
  {
    if(status == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= setting conn_mode_manual_search to %d",
                             mm_nv_conn_mode_manual_search);
    }
    else
    {
      mm_nv_conn_mode_manual_search = TRUE;
      MSG_HIGH_DS_2(MM_SUB,"=MM= conn_mode_manual_search efs read failed, status: %d, default %d",
                         status,mm_nv_conn_mode_manual_search);
    }
    mm_conn_mode_manual_search_enabled = mm_nv_conn_mode_manual_search;

#ifdef FEATURE_NAS_GW
#ifdef FEATURE_LTE
    /* EXCLUDE OLD LAI TYPE */
    dog_hb_report(DOG_MM_RPT);
    status  = mcfg_fs_read("/nv/item_files/modem/nas/exclude_old_lai_type_field", 
                          &mm_nv_exclude_old_lai_type_ie, sizeof(boolean),
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

    if (status == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= setting exclude_old_lai_type_ie to %d",
                                  mm_nv_exclude_old_lai_type_ie);
    }
    else
    {
      mm_nv_exclude_old_lai_type_ie = FALSE;
      MSG_HIGH_DS_2(MM_SUB,"=MM= exclude_old_lai_type_ie efs read failed, status: %d. default %d",
                   status,mm_nv_exclude_old_lai_type_ie);
    }
#endif
#endif

#if defined(FEATURE_3G_USER_NW_REDIR_TO_LTE) || defined(FEATURE_LTE_REL12)

    dog_hb_report(DOG_MM_RPT);
    status = mcfg_fs_read("/nv/item_files/modem/mmode/extend_lte_disable_duration", 
                         &mm_extend_lte_disable_duration, sizeof(uint32),
                         MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

    if (status == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= setting extend_lte_disable_duration to %d",
                             mm_extend_lte_disable_duration);
    }
    else
    {
      mm_extend_lte_disable_duration = MM_DEFAULT_LTE_DISABLE_DURATION;
      MSG_HIGH_DS_2(MM_SUB,"=MM= extend_lte_disable_duration efs read failed, status: %d, default %d",
                    status,mm_extend_lte_disable_duration);
    }

#endif
    mm_emm_read_nv = (mm_emm_read_nv ^ (mm_emm_read_nv & (1<<MM_MCFG_REFRESH)));
  }

#ifdef FEATURE_NAS_GW

#if defined(FEATURE_SGLTE) || defined(FEATURE_SVLTE_DUAL_SIM)
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/nas/sglte_nas_nv_config", 
                         &sglte_nas_mm_config, sizeof(nas_sglte_feature_config_T),
                         MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status == MCFG_FS_STATUS_OK)
  {
     MSG_HIGH_DS_1(MM_SUB ,"PS timer value is %d",sglte_nas_mm_config.ps_timer_value);   
  }
  else
  {
    sglte_nas_mm_config.ps_timer_value = 5;
    MSG_HIGH_DS_2(MM_SUB ,"sglte_nas_nv_config efs read failed, status: %d. default PS timer value %d",
                            status,sglte_nas_mm_config.ps_timer_value);
  }
#endif

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/nas/csg_support_configuration", 
                         &mm_nv_csg_support, sizeof(sys_csg_support_e_type),
                         MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status == MCFG_FS_STATUS_OK)
  {
     MSG_HIGH_DS_1(MM_SUB ,"setting mm_nv_csg_support to %d",mm_nv_csg_support);   
  }
  else
  {
#ifdef FEATURE_DISABLE_CSG_BYDEFAULT
    mm_nv_csg_support = SYS_CSG_SUPPORT_DISABLED;
#else
    mm_nv_csg_support = SYS_CSG_SUPPORT_WCDMA;
#endif
    MSG_HIGH_DS_2(MM_SUB ,"mm_nv_csg_support efs read failed, status: %d. default %d",
                            status,mm_nv_csg_support);
  }
#endif /*FEATURE_FEMTO_CSG*/
#endif

   dog_hb_report(DOG_MM_RPT);
   status = mcfg_fs_read("/nv/item_files/modem/nas/dos_mitigation_feature_config", 
                            &mm_emm_dos_mitigation_feature_config_info, sizeof(mm_emm_dos_mitigation_feature_config_T),
                            MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

   if(status != MCFG_FS_STATUS_OK)
   {
     mm_emm_is_t3247_enabled = TRUE;  
     mm_emm_max_attempt_counter_dos_mitigation = 5;
     mm_emm_max_fplmn_count_dos_mitigation = 10;
   }
   else
   {
     mm_emm_is_t3247_enabled = mm_emm_dos_mitigation_feature_config_info.t3247_enabled;  
     mm_emm_max_attempt_counter_dos_mitigation = mm_emm_dos_mitigation_feature_config_info.max_count;
     mm_emm_max_fplmn_count_dos_mitigation = mm_emm_dos_mitigation_feature_config_info.max_plmn_entries;
   }

   if (TRUE == mm_emm_is_t3247_enabled)
   {
      if (mm_emm_max_attempt_counter_dos_mitigation > T3247_PARAMS_COUNT_MAX)
      {
         MSG_HIGH_DS_1(MM_SUB ,"=MM= Wrong value for T3247 parameter Max Count %d", mm_emm_dos_mitigation_feature_config_info.max_count);
         mm_emm_max_attempt_counter_dos_mitigation = T3247_PARAMS_COUNT_MAX;  /* Set the max value */
      }

      if (mm_emm_max_attempt_counter_dos_mitigation > 0)
      {
        if (mm_emm_max_fplmn_count_dos_mitigation > REG_SIM_DOS_MITIGATION_PLMN_LIST_LENGTH)
        {
          MSG_HIGH_DS_1(MM_SUB ,"=MM= Wrong value for T3247 parameter Max Entries %d", mm_emm_dos_mitigation_feature_config_info.max_plmn_entries);
          mm_emm_max_fplmn_count_dos_mitigation = REG_SIM_DOS_MITIGATION_PLMN_LIST_LENGTH;  /* Set the max value */
        }
      }
   }
   else
   {
    mm_emm_max_attempt_counter_dos_mitigation = 0;
    mm_emm_max_fplmn_count_dos_mitigation = 0;
   }

   MSG_HIGH_DS_4(MM_SUB ,"dos_mitigation_feature_config efs read status: %d, t3247 enabled %d,  max_count value %d, max_entries value %d",
                            status, mm_emm_is_t3247_enabled,
                           mm_emm_max_attempt_counter_dos_mitigation,mm_emm_max_fplmn_count_dos_mitigation);

   dog_hb_report(DOG_MM_RPT);
   status = mcfg_fs_read("/nv/item_files/modem/nas/t3247_timer_val_test", 
                            &mm_emm_t3247_test_val, sizeof(mm_emm_t3247_test_val),
                            MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

   if(status != MCFG_FS_STATUS_OK)
   {
    mm_emm_t3247_test_val = 0;
   }
	MSG_HIGH_DS_2(MM_SUB ,"t3247_test_timer_val efs read status: %d, timer value (in mins) %d",
                            status,mm_emm_t3247_test_val);

 /* User Preferences of AMR Speech Codecs is captured by the NV_UMTS_AMR_CODEC_PREFERENCE_CONFIG_I NV Item */
#ifdef FEATURE_NAS_GW
   dog_hb_report(DOG_MM_RPT);
    if (mm_ghdi_nvmem_read(NV_UMTS_AMR_CODEC_PREFERENCE_CONFIG_I,
            (ghdi_nvmem_data_T *)&gsm_wcdma_amr_codec_preference) != GHDI_SUCCESS)
   {
     gsm_wcdma_amr_codec_preference = 0x05;  /* Set the default value */
     MSG_ERROR_DS(MM_SUB, "=MM= Unable to read NV for WCDMA/GSM AMR CODEC Preferences flag",0,0,0);
   }
   else
   {
     if (gsm_wcdma_amr_codec_preference > 0x0F)
     {
       MSG_HIGH_DS_1(MM_SUB ,"=MM= Wrong value for WCDMA GSM AMR CODEC Preference %d", gsm_wcdma_amr_codec_preference);
       gsm_wcdma_amr_codec_preference = 0x05;  /* Set the default value */
     }
   }
    /*  Add the AMR Feature Flag specific guarding for the Values that could be set in NV
       if the underlying  AMR Feature Flag support is enabled only then the corresponding value should allowed to be set in NV */
#ifndef FEATURE_GSM_AMR_WB
   /* Unset the GSM AMR WB Bit      0000 00X0 */
   gsm_wcdma_amr_codec_preference &= ~0x02; //0xFD;
#endif
#ifndef FEATURE_VOC_AMR_WB
   /* Unset the WCDMA AMR WB Bit    0000 X000 */
   gsm_wcdma_amr_codec_preference &= ~0x08;
#endif
   MSG_HIGH_DS_1(MM_SUB ,"=MM= WCDMA GSM AMR CODEC  Preference : %X ",gsm_wcdma_amr_codec_preference);
#endif
  status = mcfg_fs_read("/nv/item_files/modem/nas/map_cause12_to_cause15", 
                     &mm_map_rej_cause12_to_rej_cause15, sizeof(boolean),
                     MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);
  if (status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= Setting NV mm_map_rej_cause12_to_rej_cause15 to %d",mm_map_rej_cause12_to_rej_cause15);
  }
  else
  {
    mm_map_rej_cause12_to_rej_cause15 = FALSE;
    MSG_HIGH_DS_1(MM_SUB, "=MM= NV read failed, setting NV mm_map_rej_cause12_to_rej_cause15 to default %d",mm_map_rej_cause12_to_rej_cause15);
  }

}


/*===========================================================================

FUNCTION MM_INITIALISE

DESCRIPTION
  This function initializes resources and global data used by the mm task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_initialise(void)
{
   int     i = 0;
   mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
   byte    null_lai[LAI_SIZE] = { 0xff, 0xff, 0xff, 0, 0 };
   nas_edrx_config_s_type edrx_cfg;
#ifdef FEATURE_NAS_REL10   
   mm_backoff_nv_info_s_type nv_backoff_info;
#endif

#ifdef FEATURE_LTE
   emm_ctrl_data_type		  *emm_ctrl_data_ptr;
#endif
#ifdef FEATURE_NAS_GW
   mm_nas_nv_items_T mm_nas_nv_items;
#if defined(FEATURE_SGLTE) || defined(FEATURE_DUAL_SIM)
   uint8 index;
#endif 
#ifdef FEATURE_SEGMENT_LOADING
   interface_t* ptr_T = NULL;
   interface_t* ptr_W = NULL;
   ptr_T = get_tdscdma_interface();
   ptr_W = get_wcdma_interface();
#endif
#endif
#ifdef FEATURE_LTE
   emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif
   memset((void*)&edrx_cfg, 0, sizeof(nas_edrx_config_s_type));

   if( ghdi_nvmem_register_task(MM_NVIF_SIG, mm_wait,
      mm_nvmem_default_values_generator) != GHDI_SUCCESS)
   {
      MSG_FATAL_DS(MM_SUB, "=MM= UE could not register with nvmem task", 0,0,0);
   }

#ifdef FEATURE_NAS_REL10   
    memset( &nv_backoff_info, 0x00, sizeof(mm_backoff_nv_info_s_type) );
#endif


  mm_emm_read_nv = (byte) (mm_emm_read_nv|(1<<(byte)MM_EMM_CAMP));
  mm_emm_read_nv = (byte) (mm_emm_read_nv|(1<<(byte)MM_EMM_REGISTER));
  mm_emm_read_nv = (byte) (mm_emm_read_nv|(1<<(byte)MM_ATTACH_COMPLETE));
  mm_emm_read_nv = (byte) (mm_emm_read_nv|(1<<(byte)MM_MCFG_REFRESH));


   // If this NV is enabled then E Calls are not allowed in a NW without the UE being registered
  // in the NW. Default value should be set to 0.
  if((status = mcfg_fs_read("/nv/item_files/modem/mmode/sd/full_srv_req_in_emerg", 
                     &full_srv_req_in_emerg, sizeof(boolean),
                     MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1)) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= Setting NV full_srv_req_in_emerg to %d",full_srv_req_in_emerg);
  }
  else
  {
    full_srv_req_in_emerg = FALSE;
    MSG_HIGH_DS_1(MM_SUB, "=MM= NV read failed, setting full_srv_req_in_emerg to default %d",full_srv_req_in_emerg);
  }


#if defined(FEATURE_NAS_REL10) || defined(FEATURE_LTE_REL10)
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
   if((status = mcfg_fs_read("/nv/item_files/modem/nas/mm_backoff_remaining_info", 
                              &nv_backoff_info, sizeof(mm_backoff_nv_info_s_type),
                              MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1))== MCFG_FS_STATUS_OK) 
   {
#ifdef FEATURE_NAS_GW
     mm_t3246_backoff_timer_value_sim[MM_AS_ID_1]  = nv_backoff_info.t3246_value;  
     memscpy(&mm_3gpp_t3246_plmn_id_sim[MM_AS_ID_1], sizeof(sys_plmn_id_s_type), &nv_backoff_info.t3246_plmn, 
             sizeof(sys_plmn_id_s_type));	
#endif
     
     gmm_t3346_backoff_timer_value_sim[MM_AS_ID_1] = nv_backoff_info.t3346_value;
     memscpy(&gmm_3gpp_t3346_plmn_id_sim[MM_AS_ID_1], sizeof(sys_plmn_id_s_type), &nv_backoff_info.t3346_plmn, 
              sizeof(sys_plmn_id_s_type));
	if(
#ifdef FEATURE_NAS_GW
	 mm_t3246_backoff_timer_value_sim[MM_AS_ID_1] || 
#endif
     gmm_t3346_backoff_timer_value_sim[MM_AS_ID_1])
	 {
	 	mm_is_t3246_or_t3346_value_non_zero_sim[MM_AS_ID_1] = TRUE;
	 }	
#ifdef FEATUTE_LTE
#ifdef FEATURE_CIOT
     emm_ctrl_data_ptr_sim[MM_AS_ID_1]->is_t3346_started_for_mo_excp = 	 nv_backoff_info.is_t3346_started_for_mo_excp;
#endif
#endif  
   }   
   else
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= Unable to read NV mm_backoff_remaining_info , status = %d",status);
     gmm_t3346_backoff_timer_value_sim[MM_AS_ID_1] = 0;
     sys_plmn_undefine_plmn_id(&gmm_3gpp_t3346_plmn_id_sim[MM_AS_ID_1]);
#ifdef FEATURE_NAS_GW
     mm_t3246_backoff_timer_value_sim[MM_AS_ID_1]  = 0;
     sys_plmn_undefine_plmn_id(&mm_3gpp_t3246_plmn_id_sim[MM_AS_ID_1]);
#endif
#ifdef FEATUTE_LTE
#ifdef FEATURE_CIOT
     emm_ctrl_data_ptr_sim[MM_AS_ID_1]->is_t3346_started_for_mo_excp = 	FALSE;
#endif
#endif
     mm_psm_update_cache_bitmask(mm_psm_nv_list_bitmask,
                                 MM_PSM_EFS_MM_BACKOFF_REMAINING_INFO);
   } /*Read SIM1 T3246/T3346  backoff timer and PLMN information*/

   if(!MM_SUB_IS_SGLTE_SUB(MM_AS_ID_1))
   {
     if((status = mcfg_fs_read("/nv/item_files/modem/nas/mm_backoff_remaining_info", 
                       &nv_backoff_info, sizeof(mm_backoff_nv_info_s_type),
                       MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_2))== MCFG_FS_STATUS_OK)
     {
#ifdef FEATURE_NAS_GW
       mm_t3246_backoff_timer_value_sim[MM_AS_ID_2]  = nv_backoff_info.t3246_value;

       memscpy(&mm_3gpp_t3246_plmn_id_sim[MM_AS_ID_2], sizeof(sys_plmn_id_s_type), &nv_backoff_info.t3246_plmn, 
               sizeof(sys_plmn_id_s_type));
#endif
       gmm_t3346_backoff_timer_value_sim[MM_AS_ID_2] = nv_backoff_info.t3346_value;
       memscpy(&gmm_3gpp_t3346_plmn_id_sim[MM_AS_ID_2], sizeof(sys_plmn_id_s_type), &nv_backoff_info.t3346_plmn, 
                sizeof(sys_plmn_id_s_type));
	if(
#ifdef FEATURE_NAS_GW
	 mm_t3246_backoff_timer_value_sim[MM_AS_ID_2] || 
#endif
     gmm_t3346_backoff_timer_value_sim[MM_AS_ID_2])
	 {
	 	mm_is_t3246_or_t3346_value_non_zero_sim[MM_AS_ID_2] = TRUE;
	 }	
#ifdef FEATUTE_LTE
#ifdef FEATURE_CIOT
     emm_ctrl_data_ptr_sim[MM_AS_ID_2]->is_t3346_started_for_mo_excp = 	 nv_backoff_info.is_t3346_started_for_mo_excp;
#endif
#endif
     }   
     else
     {
       MSG_HIGH_DS_1(MM_SUB, "=MM= Unable to read NV mm_backoff_remaining_info , status = %d",status);
       gmm_t3346_backoff_timer_value_sim[MM_AS_ID_2]  = 0;
       sys_plmn_undefine_plmn_id(&gmm_3gpp_t3346_plmn_id_sim[MM_AS_ID_2]);
#ifdef FEATURE_NAS_GW
       mm_t3246_backoff_timer_value_sim[MM_AS_ID_2]   = 0;
       sys_plmn_undefine_plmn_id(&mm_3gpp_t3246_plmn_id_sim[MM_AS_ID_2]);
#endif
#ifdef FEATUTE_LTE
#ifdef FEATURE_CIOT
     emm_ctrl_data_ptr_sim[MM_AS_ID_2]->is_t3346_started_for_mo_excp = FALSE;
#endif
#endif
       MSG_HIGH_DS_0(MM_SUB,"=MM= SIM2 Unable to read NV for Back-off timer value ");
       mm_as_id = MM_AS_ID_2;
       mm_psm_update_cache_bitmask(mm_psm_nv_list_bitmask,
                                   MM_PSM_EFS_MM_BACKOFF_REMAINING_INFO);
       mm_as_id = MM_AS_ID_1;
     } /*Read SIM2 T3246/T3346 backoff timer and PLMN information*/ 
   }
#else
   if((status = mcfg_fs_read("/nv/item_files/modem/nas/mm_backoff_remaining_info", 
                    &nv_backoff_info, sizeof(mm_backoff_nv_info_s_type),
                    MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1))== MCFG_FS_STATUS_OK)
   {
#ifdef FEATURE_NAS_GW
     mm_t3246_backoff_timer_value = nv_backoff_info.t3246_value;
     memscpy(&mm_3gpp_t3246_plmn_id, sizeof(sys_plmn_id_s_type), &nv_backoff_info.t3246_plmn, 
             sizeof(sys_plmn_id_s_type));
#endif
     gmm_t3346_backoff_timer_value = nv_backoff_info.t3346_value;
     memscpy(&gmm_3gpp_t3346_plmn_id, sizeof(sys_plmn_id_s_type), &nv_backoff_info.t3346_plmn, 
              sizeof(sys_plmn_id_s_type)); 
     if(
#ifdef FEATURE_NAS_GW
	 mm_t3246_backoff_timer_value || 
#endif
     gmm_t3346_backoff_timer_value)
	 {
	 	mm_is_t3246_or_t3346_value_non_zero = TRUE;
	 }				  
#ifdef FEATUTE_LTE
#ifdef FEATURE_CIOT
     emm_ctrl_data_ptr->is_t3346_started_for_mo_excp = 	 nv_backoff_info.is_t3346_started_for_mo_excp;
#endif
#endif
   }   
   else
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= Unable to read NV mm_backoff_remaining_info , status = %d",status);
     gmm_t3346_backoff_timer_value = 0;
	 mm_is_t3246_or_t3346_value_non_zero = FALSE;
     sys_plmn_undefine_plmn_id(&gmm_3gpp_t3346_plmn_id);
#ifdef FEATURE_NAS_GW
     mm_t3246_backoff_timer_value = 0;
     sys_plmn_undefine_plmn_id(&mm_3gpp_t3246_plmn_id);
#endif
#ifdef FEATUTE_LTE
#ifdef FEATURE_CIOT
     emm_ctrl_data_ptr->is_t3346_started_for_mo_excp = FALSE;
#endif
#endif
     mm_psm_update_cache_bitmask(mm_psm_nv_list_bitmask,
                                 MM_PSM_EFS_MM_BACKOFF_REMAINING_INFO);
   } /*Read SIM1 T3246/T3346  backoff timer and PLMN information*/
 /*mm_timer_t3246_remaining_to_efs*/
#endif
#endif
#ifdef FEATURE_NAS_REL11
   memset(&mm_nas_nv_sim_hfns,0,sizeof(mm_nas_nv_sim_parameters_T));
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   for ( mm_as_id=MM_AS_ID_1; mm_as_id < (mm_as_id_e_type)MAX_NAS_STACKS; mm_as_id++ )
#endif
   {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
     mm_sub_id = mm_sub_id_stack[mm_as_id];
#endif
     mm_imsi_is_native = FALSE;
#ifdef FEATURE_RAT_PRIORITY_LIST
     mm_rat_pri_list_info.num_items = 0;
#endif
     mm_serving_plmn.info.sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
     /* Set the default values to the Serving PLMN */
     mm_serving_plmn.info.plmn.identity[0] = 0xFF;
     mm_serving_plmn.info.plmn.identity[1] = 0xFF;
     mm_serving_plmn.info.plmn.identity[2] = 0xFF;
     mm_serving_plmn.lac.lac[0] = 0xFF;
     mm_serving_plmn.lac.lac[1] = 0xFF;
     mm_serving_plmn.rac = 0xFF;
  #if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
     mm_serving_plmn.info.csg_info.csg_id = SYS_CSG_ID_INVALID;
  #endif
     
     mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
     mm_serving_plmn.info.sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
     mm_serving_plmn.info.plmn_service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;
     mm_serving_plmn.info.plmn_forbidden = FALSE;
     mm_serving_plmn.info.roaming_ind = SYS_ROAM_STATUS_OFF;
     mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
     mm_serving_plmn.info.active_rat = SYS_RAT_NONE;
     mm_serving_plmn.info.ps_data_suspend = FALSE;
     mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
     /*initialize to NA, plmn_reg_type is applicable to CAMP_ONLY */ 
     mm_serving_plmn.info.plmn_reg_type = SYS_REG_DOMAIN_NOT_APPLICABLE;
  
  #ifdef FEATURE_GSM_DTM
     mm_serving_plmn.dtm_supported = FALSE;
  #endif
     mm_serving_plmn.egprs_supported = FALSE;
  
     mm_serving_plmn.update_equiv_plmn_list = FALSE;
     memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));
  
     mm_serving_plmn.t3122_running = TRUE;
  
     mm_serving_plmn.arfcn.num = 0xffff;
     mm_serving_plmn.arfcn.band = SYS_BAND_NONE;
     mm_serving_plmn.bsic = 0xff;
     for ( i=0; i< MAX_MM_TIMER+1; i++ )
     {
        mm_timer_status[i] = TIMER_STOPPED;
     }

     mm_read_psm_config();

     mm_last_active_rat_stopped  = SYS_RAT_NONE;
     mm_stored_imsi.length = 0;
     mm_location_information.location_update_status = NOT_UPDATED;
     (void)memscpy( mm_location_information.lai, LAI_SIZE, null_lai, LAI_SIZE );
     (void)memset( mm_location_information.tmsi, 0xFF, TMSI_SIZE );
     mm_mmr_attach_ind_status = ATTACH_FAILED_IND_NOT_REQUIRED;
     auth_gsm_access = FALSE;
     mm_need_to_process_sim_avail_req = FALSE;
     mm_client_activated_group = SYS_NAS_INFO_MASK_NONE;
     mm_nas_changed_group = SYS_NAS_INFO_MASK_NONE ;
     mm_plmn_search_in_progress = FALSE;
     mm_deep_sleep_alt_scan           = FALSE;

}


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mm_as_id = MM_AS_ID_1; 
#endif
/*  Putting definition out of dual sim*/
   mm_msg_info = 0;
  
for(i=0;i<MM_DEBUG_MESSAGE_BUFFER_SIZE;i++)
{
mm_emm_debug_buffer[i].debug_buffer_type.mm_info_debug_buffer.message_set=0;
mm_emm_debug_buffer[i].debug_buffer_type.mm_info_debug_buffer.message_id=0;
#ifdef FEATURE_LTE
mm_emm_debug_buffer[i].debug_buffer_type.emm_info_debug_buffer.msg_umid=0;
#endif
mm_emm_debug_buffer[i].present_mm_state = 0;
mm_emm_debug_buffer[i].present_gmm_state = 0;
mm_emm_debug_buffer[i].present_emm_state = 0;
mm_emm_debug_buffer[i].msg_info = 0;
mm_emm_debug_buffer[i].mm_emm_message_direction = TO_MM;
}

   mm_emm_debug_buffer_index = 0;

#ifdef FEATURE_NAS_GW
   mm_gsm_supported = mm_wcdma_supported = mm_tdscdma_supported = FALSE;
   /* 
   * Initialize STRUCT mm_nas_nv_items to 0.
   * Later in the function, values read from EFS will be stored here.
   */
   memset( &mm_nas_nv_items, 0x00, sizeof(mm_nas_nv_items_T) );




   /* setting to default value*/
   ss_screening_indicator = 0x01;
   mm_retry_em_call_on_access_blocked_cell = 0;

   mm_managed_roaming_retry_lu_in_manual_mode = 0;


#ifdef DISABLE_9x05_OPTIMIZATIONS
#ifdef FEATURE_NAS_REL12
   mm_optimised_ps_signalling_connection_release = 0x01;
#else
   mm_optimised_ps_signalling_connection_release = 0;
#endif
#endif
   mm_ps_sig_conn_rel_after_ps_sms = 0;
   gmm_psm_local_detach_enabled = 0x01;

   
#ifdef FEATURE_NAS_ECALL
#ifdef FEATURE_DUAL_SIM
    status =  mcfg_fs_read("/nv/item_files/modem/nas/t3242",
                          &mm_ecall_inactivity_timer_sim[MM_AS_ID_1],
                          sizeof(mm_ecall_inactivity_timer),
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);
   if (status != MCFG_FS_STATUS_OK)
   {
     MSG_HIGH_DS_1(MM_SUB ,"=MM= Unable to read NV t3242, status : %d ",status);
     /*ERA GLONASS has 8hrs as default value*/
     mm_ecall_inactivity_timer_sim[MM_AS_ID_1] = 8*60;
   }

      /* ecall inactivity configurable timer in mins t3242 and t3243 */
   status =mcfg_fs_read("/nv/item_files/modem/nas/t3242",
                          &mm_ecall_inactivity_timer_sim[MM_AS_ID_2],
                          sizeof(mm_ecall_inactivity_timer),
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_2);
   if (status != MCFG_FS_STATUS_OK)
   {
     MSG_HIGH_DS_1(MM_SUB ,"=MM= Unable to read NV t3242, status : %d ",status);
     /*ERA GLONASS has 8hrs as default value*/
     mm_ecall_inactivity_timer_sim[MM_AS_ID_2] = 8*60;
   }

   MSG_HIGH_DS_3(MM_SUB, "=MM= NV NAS eCall inactivity timer: %d %d %d msecs ",status,
                                               mm_ecall_inactivity_timer_sim[MM_AS_ID_1],
                                               mm_ecall_inactivity_timer_sim[MM_AS_ID_2]);
#else 
   /* ecall inactivity configurable timer in mins t3242 and t3243 */
   status =  mcfg_fs_read("/nv/item_files/modem/nas/t3242",
                          &mm_ecall_inactivity_timer_sim[MM_AS_ID_1],
                          sizeof(mm_ecall_inactivity_timer),
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);
   if (status != MCFG_FS_STATUS_OK)
   {
     MSG_HIGH_DS_1(MM_SUB ,"=MM= Unable to read NV t3242, status : %d ",status);
     /*ERA GLONASS has 8hrs as default value*/
     mm_ecall_inactivity_timer = 8*60;
   }

   MSG_HIGH_DS_2(MM_SUB, "=MM= NV NAS eCall inactivity timer: %d %d msecs ",status, mm_ecall_inactivity_timer);
#endif
#endif

   /* Set the supported mode of operation in UE */
#if defined(FEATURE_GSM) && defined(FEATURE_NAS_GW)
   mm_gsm_supported = TRUE;
#endif
#ifdef FEATURE_SEGMENT_LOADING
   if (ptr_W != NULL )
   {
      mm_wcdma_supported = TRUE;
   }

   if (ptr_T != NULL )
   {
      mm_tdscdma_supported = TRUE;
   }
#else
#if defined(FEATURE_WCDMA) && defined(FEATURE_NAS_GW)
   mm_wcdma_supported = TRUE;
#endif

#ifdef FEATURE_TDSCDMA
   mm_tdscdma_supported = TRUE;
#endif
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   for ( mm_as_id=MM_AS_ID_1; mm_as_id < (mm_as_id_e_type)MAX_NAS_STACKS; mm_as_id++ )
#endif
   {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
     mm_sub_id = mm_sub_id_stack[mm_as_id];
#endif
     mm_read_imei(NULL);
     mm_state = NULL;
     mm_idle_substate = NULL;
     mm_previous_idle_substate = NULL;
     mm_llc_needs_to_be_resumed = FALSE;
     mm_system_information.ATT = FALSE;
     mm_system_information.T3212_timer_value = 0;
     mm_system_information.reestablishment_allowed = FALSE;
     mm_system_information.mscr = 0;
     mm_system_information.sgsnr = FALSE;
     mm_system_information.cell_access = MM_AS_ACCESS_ALL_CALLS;
     mm_last_reg_dsac_information.cell_access = MM_AS_ACCESS_ALL_CALLS;
#ifdef FEATURE_DSAC
     mm_invalidate_dsac_info();      
     mm_invalidate_ppac_info(); 
#endif
     mm_rat_change_pending  = FALSE;
     mm_serv_change_pending = FALSE;
     mm_to_only_reg         = FALSE;
    mm_previous_serving_rat     = SYS_RAT_NONE;
     mm_nreg_req_received    = FALSE;
     mm_controls_resel_dec_mt_cs_call = FALSE;

     mm_cnm_rel_req_received = FALSE;
     mm_pending_no_service_to_reg = FALSE;
  
     mm_deep_sleep_pending = FALSE;

     mm_lu_reject_info.mm_conn_rej_timer_value = 0;
     
	 mm_held_cnm_service_req.present = FALSE;
  
     mm_reg_waiting_for_reg_cnf       = FALSE;
     mm_ready_to_send_reg_cnf         = FALSE;
     mm_needs_to_send_mmr_service_ind = FALSE;
     mm_waiting_for_service_cnf       = FALSE;
     mm_waiting_for_stop_mode_cnf     = FALSE;
     mm_waiting_for_deact_cnf         = FALSE;
     mmcoord_reg_req_pending          = FALSE;
     mm_waiting_for_csmt_response     = FALSE;
  
  #if defined (FEATURE_WRLF_SYSTEM_SEL) || defined (FEATURE_LTE)
     mm_rlf_stop_mode_pending = MM_RLF_STOP_MODE_PENDING_NONE;
  #endif
     mm_waiting_for_ph_status_change_cnf = FALSE;
     mm_hard_abort_pending            = FALSE;
  #ifdef FEATURE_LTE
     mm_stop_mode_local_detach_peding = FALSE;
     mm_ps_detach_type                = SYS_PS_DETACH_TYPE_NONE;
  #endif
  #ifdef FEATURE_DUAL_SIM
     mm_abort_dual_switch_pending = SYS_STOP_MODE_REASON_NONE;
  #endif
#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE
     mm_hard_abort_ue_sglte_mode_switch_pending = FALSE;
#endif
#ifdef FEATURE_SGLTE
     mm_no_service_ind_during_split = FALSE;
#endif
     mm_lu_reject_info.reject_cause = 0;
     (void)memset( mm_lu_reject_info.lai, 0xff, LAI_SIZE );
  
     (void)memscpy( mm_serving_lai, LAI_SIZE,
                   null_lai,
                   LAI_SIZE );
  
     (void)memscpy( mm_previous_serving_lai, LAI_SIZE,
                   null_lai,
                   LAI_SIZE );
  
     (void)memscpy( mm_gstk_previous_serving_lai, LAI_SIZE,
                   null_lai,
                   LAI_SIZE );

  #ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
     WtoGCCO_state = CCO_INACTIVE;
  #endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */
  
  #ifdef FEATURE_INTERRAT_PCCO_GTOW
     GtoWCCO_state  = CCO_INACTIVE;   
  #endif /* FEATURE_INTERRAT_PCCO_GTOW */      
  
  
     mm_lu_attempt_counter = 0;
  
     mm_initiate_lu_upon_entry_into_new_cell = FALSE;
     mm_cipher_key_sequence_number = 7;
  
     mm_ef_kc.cipher_key_sequence_number = 7;
  
     mm_follow_on_request = FALSE;
  
     mm_follow_on_proceed = FALSE;
  
     mm_reject_cause = 0;
  
     mm_init_forbidden_LAIs();
    
     mm_cnm_is_active = FALSE;
  
     mm_sms_is_active = FALSE;
  
     mm_T3213_timed_out = FALSE;
  
     mm_idle_transition_timer = NULL;
  
     mm_pending_lu_request = NO_LU;
     mm_lu_start_reason = NO_LU;
  
     mm_possible_imsi_attach = FALSE;
  
     mm_ef_kc_valid = FALSE;
  
     mm_sig_low_priority = FALSE;
     mm_nreg_release_pending = FALSE;
     mm_ue_pref_rat = SYS_RAT_NONE;
  #ifndef FEATURE_RAT_PRIORITY_LIST
     mm_band_pref   = SYS_BAND_MASK_EMPTY;
  #endif /* FEATURE_RAT_PRIORITY_LIST */
  
     mm_serving_cell_valid = FALSE;

#if defined(FEATURE_NAS_REL10) && defined(FEATURE_LTE)
     mm_is_lu_to_be_initiated_t3346_ltogw = FALSE;
#endif
     /* Initialize the CNM service establish cause */
     mm_cnm_srv_est_cause = MM_RR_NO_CNM_SERVICE;
  
     mm_connection_type = NO_MM_CONNECTION;
     /* Set the default values for UE access class */
     mm_lo_access_class = RRC_LO_ACCESS_CLASS_ABSENT;
     mm_hi_access_class = RRC_HI_ACCESS_CLASS_ABSENT;
  
     mm_sms_preferred_domain_id = PS_PREFERRED;
  
  #ifdef FEATURE_SGLTE
     mm_nv_sms_preferred_domain_id = PS_PREFERRED;
  #endif
  
  #if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
  #ifndef FEATURE_RAT_PRIORITY_LIST
     mm_ue_band_class_pref = 0x00;
  #endif /* FEATURE_RAT_PRIORITY_LIST */
     mm_active_band = SYS_BAND_MASK_EMPTY;
  #endif
     mm_band_indicator = SYS_BAND_CLASS_NONE; 
  #if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
     mm_lcs_active = FALSE;
  #endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
  
     mm_rat_handler_status = MM_RAT_INITIAL;
  
  #ifdef FEATURE_CCBS
     mm_supports_NIMO_CM_connection = TRUE;
  #endif
  
  #if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW)
     glif_grr_camp_state = GLIF_CAMP_ON_CCCH ;
  #endif /* #if defined(FEATURE_GSM_GPRS) && defined (FEATURE_NAS_GW) */
 
     mm_managed_roaming_enabled = FALSE;

     mm_is_foreground_search_to_be_initiated  = TRUE;
  
     mm_is_gmm_moved_dereg_due_to_csreg = FALSE;
  #ifdef FEATURE_NAS_REL10
     mm_3gpp_t3246_plmn_id.identity[0]= 0xFF;
     mm_3gpp_t3246_plmn_id.identity[1]= 0xFF;
     mm_3gpp_t3246_plmn_id.identity[2]= 0xFF;
     mm_store_integrity_protected     = FALSE;
     mm_t3246_backoff_timer_value     = 0;
	 mm_is_t3246_or_t3346_value_non_zero = FALSE;
  #endif
  
  #ifdef FEATURE_NAS_REL10
     mm_stored_per_ms_t3212_value = 0;
     mm_is_per_ms_t3212_present   = FALSE;
  #endif
#ifdef FEATURE_NAS_GW
#if defined FEATURE_MMCP_EGPRS || defined FEATURE_GSM_DTM || defined FEATURE_HSDPA
     mm_mmr_cell_service_ind_pending = FALSE;  
#endif
#endif
     memset( (void *)mm_sim_access_class, 0, sizeof(mm_sim_access_class));
     
     for ( i=0; i< MAX_LU_ATTEMPT_SIZE; i++)
     {
       mm_lu_reject_cause[i] = 0xFF;
     }
  
     auth_usim_gsm_security_context = FALSE;
     /* TSTS Variables Initialization */  
     auth_gsm_cksn               = 0x07;
     auth_gsm_gprs_cksn          = 0x07;
     auth_cs_gprs_cksn           = 0x07;
     auth_ps_gprs_cksn           = 0x07;
     auth_ps_ciphering_algorithm = 0xFF;
     auth_gsm_gprs_keys_present  = FALSE;
  
     mm_csfb_service_ind_status = MM_CSFB_SEND_SERVICE_STATUS_NONE;

     mm_sim_perso_state = MMGSDI_PERSO_STATUS_NONE;
  
     mm_sim_card_mode           = MMGSDI_APP_UNKNOWN;
     mm_interrat_state          = MM_INTERRAT_CHANGE_STATE_TYPE_LO;
     mm_service_loss_on_rat     = SYS_RAT_NONE;
     mm_plmn_list_req_pending   = FALSE;
#ifdef FEATURE_NAS_GW
     mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_NONE;
#endif
     mm_waiting_for_abort_cnf = FALSE;
     mm_stop_mode_req_pended = FALSE;
     mm_reg_req_revd_active_call = FALSE;
     mm_lu_pended_nmo_unknown      = FALSE;
     mm_cell_changed_to_cs_capable = FALSE;
     mm_waiting_for_open_session_cnf = FALSE;
     mm_last_paging_cause = 0xFF;
     rrc_connection_status = MMRRC_IDLE;
     cs_session_status = MMRRC_IDLE;
     ps_session_status = MMRRC_IDLE;
     pending_mm_msg_after_release = FALSE;
     pending_gmm_msg_after_release = FALSE;
  
     mm_cs_domain_possible_reest = FALSE;
     mm_ps_domain_possible_reest = FALSE;
     pending_gmm_msg_valid = FALSE;
     pending_mm_msg_valid = FALSE;
     send_sequence_number_mm_cc_ss = 0;
  #if defined(FEATURE_LTE)
     mm_csfb_is_blind_redir_to_lte_req_to_be_sent = FALSE;
     mm_stop_mode_local_detach_peding = FALSE;
  #endif
  
  #ifdef FEATURE_TC
     mm_tc_is_active = FALSE;
  #endif
     mm_reg_cause = MMR_CAUSE_NONE;
     mm_manual_mode_user_selection_procedure = FALSE;
  #ifdef FEATURE_ENHANCED_NW_SELECTION
     mm_force_lu = FALSE;
  #endif

  mm_map_rej_cause12_to_rej_cause15 = FALSE;

  #ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
     mm_pending_lu_type = MM_LU_CAUSE_RESERVED;
  #endif
     mm_service_ind_blocked_due_to_cs_reg = FALSE;
     mm_service_ind_blocked_due_to_irat_cs_reg = FALSE;
     mm_connection_activated = FALSE;
     mm_is_sim_cs_invalid_due_to_dos = FALSE;
     mm_is_sim_ps_invalid_due_to_dos = FALSE;
     mm_is_sim_to_be_ps_invalid_after_power_off = FALSE;
     mm_is_sim_to_be_cs_invalid_after_power_off = FALSE;
#if !defined(FEATURE_NAS_CS_DISABLED) || !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
     mm_need_to_send_no_srv_on_call_end_due_to_dos = FALSE;
#endif
   
  #if defined(FEATURE_DUAL_SIM) && !defined(FEATURE_LTE)
     mm_sim_mode_pref = SYS_MODE_PREF_NONE;
  #endif
     mm_gstk_send_location_status = FALSE;
  
      /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
  #if (defined (FEATURE_GSM) || defined (FEATURE_WCDMA)) && defined(FEATURE_NAS_GW)
  #ifdef FEATURE_MODEM_HEAP
      gmm_rat_chng_pended_msg = NULL;
  #endif
      gmm_rat_chng_pended_msg_valid = FALSE;
  #endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
  
  #ifdef FEATURE_MODEM_HEAP
      pending_mm_msg  = NULL;
      pending_gmm_msg = NULL;
  #endif
  
     /* TSTS Variables Initialization */ 
    
     mm_ms_opmode = 0;
  
     mm_mnc_length = 2;
  
  
     mm_lu_pended_nmo_unknown = FALSE;
  #ifdef FEATURE_ENHANCED_NW_SELECTION
    /* ----------------------------
    ** Default timeout = 6 minutes
    ** ---------------------------- */
     mm_foreground_search_timer_value =360000 ;
     mm_foreground_search_lai_timer_list.length =0;
     for ( i=0; i< MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH; i++)
     {
       memscpy(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai, LAI_SIZE, null_lai, LAI_SIZE);
       mm_foreground_search_lai_timer_list.lai_timer_list[i].rac = 0xFF;
       mm_foreground_search_lai_timer_list.lai_timer_list[i].is_ps_registered = FALSE;
       mm_foreground_search_lai_timer_list.lai_timer_list[i].ps_attempt_counter = 0;
     }
  #endif
  #if defined(FEATURE_WRLF_SYSTEM_SEL) && defined(FEATURE_NAS_GW)
     mm_received_suitable_search_end_ind = FALSE;
     mm_wcdma_rlf_state = WCDMA_RLF_IDLE;
     mm_wrlf_rat_change_pending = FALSE;
  #endif
  #ifdef FEATURE_LTE
     mm_multimode_rat_change_pending = FALSE;
  #endif
  #ifdef FEATURE_LTE
     mm_csfb_is_blind_redir_to_lte_req_to_be_sent = FALSE;
     mm_csfb_set_state(MM_CSFB_STATE_NULL);
     mm_ps_detach_state = MM_PS_DETACH_STATE_NONE;
     mm_previous_rat = SYS_RAT_NONE;
  #endif
  #if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
     mm_utran_ps_voice_parameters.is_ps_voice_on_utran_supported = FALSE;
     mm_utran_ps_voice_parameters.is_last_utran_voims_supported = FALSE;
  #endif
  #if defined(FEATURE_WRLF_SYSTEM_SEL) && defined(FEATURE_NAS_GW)
    mm_wcdma_rlf_state = WCDMA_RLF_IDLE;
    mm_wrlf_rat_change_pending = FALSE;
    mm_wrlf_pending_rr_message_ptr = NULL;
    mm_wrlf_pending_rrc_message_ptr = NULL;	
    mm_wrlf_serving_plmn_ptr = NULL;
    mm_received_suitable_search_end_ind = FALSE;
  #endif 
  
  #if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
    mm_serving_csg_id = SYS_CSG_ID_INVALID;
  #endif /*FEATURE_FEMTO_CSG*/
   
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
     mm_is_sim_busy = FALSE;
#endif

#ifdef FEATURE_NAS_REL11
#ifdef FEATURE_WCDMA
   mm_is_first_rrc_connection_attempt = TRUE;
#endif
#endif

   mm_is_cs_connected_due_to_srvcc    = FALSE;
   mm_coord_camped_cell_id = 0xFF;
   gmm_attach_fail_counter = 0;
   gmm_trigger_attach_on_ps_call = FALSE;
#ifdef FEATURE_NAS_ECALL
   mm_ecall_mode_type = MMR_ECALL_MODE_NORMAL;
   mm_need_to_perform_ecall_inactivity = FALSE;
   mm_call_est_cause = NO_CNM_SERVICE;  
#endif
   mm_service_ind_blocked_due_to_cs_reg = FALSE;
   mm_service_ind_blocked_due_to_irat_cs_reg = FALSE;
   mm_connection_activated = FALSE;
   mm_is_gsm_active_rat_for_thermal_read = FALSE;
}

#ifdef FEATURE_DUAL_SIM 
  mm_sub_capability_sim[MM_AS_ID_1] = SUBS_CAPABILITY_MULTIMODE;
  mm_sub_capability_sim[MM_AS_ID_2] = SUBS_CAPABILITY_GSM_ONLY;
#ifdef FEATURE_TRIPLE_SIM
  mm_sub_capability_sim[MM_AS_ID_3] = SUBS_CAPABILITY_GSM_ONLY;
#endif
#endif
   mm_nv_tighter_capability         = 1;
   mm_nv_vamos_feature_support      = VAMOS_OFF;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   mm_as_id = MM_AS_ID_1; 
#endif


#ifdef FEATURE_VAMOS
   status = mcfg_fs_read("/nv/item_files/modem/geran/vamos_support", 
                           &mm_nv_vamos_feature_support,sizeof(vamos_support_e),
                            MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);
   
   if (status != MCFG_FS_STATUS_OK)
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= Unable to read NV vamos_support , status = %d",status);
     mm_nv_vamos_feature_support = VAMOS_1;
   }
   MSG_HIGH_DS_2(MM_SUB, "=MM= NV NAS VAMOS FEATURE status,VAMOS FEATURE VALUE : %d, %d",status,mm_nv_vamos_feature_support);
#endif


#ifdef FEATURE_LTE
  mm_rrc_pch_state_info.is_in_pch_state = FALSE;
#endif
#endif
     status = mcfg_fs_read("/nv/item_files/modem/nas/nas_edrx_config", 
                             &edrx_cfg,sizeof(nas_edrx_config_s_type),
                             MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);
    /* Currently disabled by default for UMTS
       Appropriate values to be set when UMTS EDRX is supported */
    ULOG_RT_PRINTF_2(mm_get_ulog_handle(),
                     "=MM= EFS edrx config read: status = %d, size of nas_edrx_config_s_type  = %d",
                     status, sizeof(nas_edrx_config_s_type));
#ifdef FEATURE_NAS_GW
    mm_edrx_cfg.umts.enabled = FALSE;
    mm_edrx_cfg.umts.ptw = 0;
    mm_edrx_cfg.umts.cycle_length = 0;
#endif
    if(status != MCFG_FS_STATUS_OK)
    {
		       /* Set default values */
      mm_edrx_cfg.lte.enabled = TRUE;
      mm_edrx_cfg.lte.ptw = 4; /* 4 seconds per Table 10.5.5.32/3GPP TS 24.008 */
      mm_edrx_cfg.lte.cycle_length = 1; /* 10,24 seconds per Table 10.5.5.32/3GPP TS 24.008 */
#ifdef FEATURE_CIOT
      mm_edrx_cfg.version = 1;
      mm_edrx_cfg.M1.enabled = TRUE;
      mm_edrx_cfg.M1.ptw = 9; /* 12.8 seconds per Table 10.5.5.32/3GPP TS 24.008 */
      mm_edrx_cfg.M1.cycle_length = 10; /* 327.68s seconds per Table 10.5.5.32/3GPP TS 24.008 */

      mm_edrx_cfg.NB1.enabled = TRUE;
      mm_edrx_cfg.NB1.ptw = 4; /* 12.8 seconds per Table 10.5.5.32/3GPP TS 24.008 */
      mm_edrx_cfg.NB1.cycle_length = 10; /* 327.68s seconds per Table 10.5.5.32/3GPP TS 24.008 */
#endif
#ifdef FEATURE_NAS_GW
      mm_edrx_cfg.gsm.enabled = FALSE;
      mm_edrx_cfg.gsm.ptw = 0; /* PTW value is not applied in GSM mode */
      mm_edrx_cfg.gsm.cycle_length = 0; /*1,88 seconds per Table 10.5.5.32/3GPP TS 24.008 */
#endif
    }
    else
    {
      if (mm_validate_edrx_cfg(edrx_cfg.lte.ptw, 
                              edrx_cfg.lte.cycle_length,
                              SYS_RAT_LTE_RADIO_ACCESS) == FALSE)
      {
        MSG_ERROR_DS_0(MM_SUB,"=MM= Validation failed for LTE eDRX config");
        ULOG_RT_PRINTF_0(mm_get_ulog_handle(),
                         "=MM= Validation failed for LTE eDRX config");
		       /* Set default values */
        mm_edrx_cfg.lte.enabled = TRUE;
        mm_edrx_cfg.lte.ptw = 4; /* 4 seconds per Table 10.5.5.32/3GPP TS 24.008 */
        mm_edrx_cfg.lte.cycle_length = 1; /* 10,24 seconds per Table 10.5.5.32/3GPP TS 24.008 */
      }
      else
      {
        mm_edrx_cfg.lte.enabled = edrx_cfg.lte.enabled;
        mm_edrx_cfg.lte.ptw = edrx_cfg.lte.ptw;
        mm_edrx_cfg.lte.cycle_length = edrx_cfg.lte.cycle_length;
      }
#ifdef FEATURE_CIOT
      if (edrx_cfg.version >= 1 )
      {
        if(mm_validate_edrx_cfg(edrx_cfg.M1.ptw, 
                               edrx_cfg.M1.cycle_length,
                               SYS_RAT_LTE_M1_RADIO_ACCESS) == FALSE)
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= Validation failed for M1 eDRX config");
          ULOG_RT_PRINTF_0(mm_get_ulog_handle(),
                         "=MM= Validation failed for M1 eDRX config");
          /* Set default values */
          mm_edrx_cfg.version = edrx_cfg.version; 
          mm_edrx_cfg.M1.enabled = TRUE;
          mm_edrx_cfg.M1.ptw = 9; /* 12.8 seconds per Table 10.5.5.32/3GPP TS 24.008 */
          mm_edrx_cfg.M1.cycle_length = 10; /* 327.68sseconds per Table 10.5.5.32/3GPP TS 24.008 */
        }
        else
        {
          mm_edrx_cfg.version = edrx_cfg.version; 
          mm_edrx_cfg.M1.enabled = edrx_cfg.M1.enabled;
          mm_edrx_cfg.M1.ptw = edrx_cfg.M1.ptw;
          mm_edrx_cfg.M1.cycle_length = edrx_cfg.M1.cycle_length;
        }

        if (mm_validate_edrx_cfg(edrx_cfg.NB1.ptw, 
                                 edrx_cfg.NB1.cycle_length,
                                 SYS_RAT_LTE_NB1_RADIO_ACCESS) == FALSE)
        {
          MSG_ERROR_DS_0(MM_SUB,"=MM= Validation failed for NB1 eDRX config");
          ULOG_RT_PRINTF_0(mm_get_ulog_handle(),
                         "=MM= Validation failed for NB1 eDRX config");
          /* Set default values */
          mm_edrx_cfg.version = edrx_cfg.version; 
          mm_edrx_cfg.NB1.enabled = TRUE;
          mm_edrx_cfg.NB1.ptw = 4; /* 12.8 seconds per Table 10.5.5.32/3GPP TS 24.008 */
          mm_edrx_cfg.NB1.cycle_length = 10; /* 327.68s seconds per Table 10.5.5.32/3GPP TS 24.008 */
        }
        else
        {
          mm_edrx_cfg.version = edrx_cfg.version; 
          mm_edrx_cfg.NB1.enabled = edrx_cfg.NB1.enabled;
          mm_edrx_cfg.NB1.ptw = edrx_cfg.NB1.ptw;
          mm_edrx_cfg.NB1.cycle_length = edrx_cfg.NB1.cycle_length;
        }
      }
#endif
#ifdef FEATURE_NAS_GW
      if (mm_validate_edrx_cfg(edrx_cfg.gsm.ptw, 
                               edrx_cfg.gsm.cycle_length,
                               SYS_RAT_GSM_RADIO_ACCESS) == FALSE)
      {
        mm_edrx_cfg.gsm.enabled = FALSE;
        mm_edrx_cfg.gsm.ptw = 0; /* PTW value is not applied in GSM mode */
        mm_edrx_cfg.gsm.cycle_length = 0; /* 1,88 seconds per Table 10.5.5.32/3GPP TS 24.008 */
      }
      else
      {
        mm_edrx_cfg.gsm.enabled = edrx_cfg.gsm.enabled;
        mm_edrx_cfg.gsm.ptw = edrx_cfg.gsm.ptw;
        mm_edrx_cfg.gsm.cycle_length = edrx_cfg.gsm.cycle_length;
      }
#endif	  
    }
#ifdef FEATURE_NAS_GW
    MSG_HIGH_DS_8(MM_SUB, "=MM= EFS edrx config read: status = %d,gsm_enabled = %d, umts_enabled = %d, lte_enabled = %d \
    lte_ptw = %d. lte_cycle_length = %d gsm_ptw = %d gsm_cycle_length = %d",
    status, mm_edrx_cfg.gsm.enabled, mm_edrx_cfg.umts.enabled, mm_edrx_cfg.lte.enabled,
    edrx_cfg.lte.ptw, edrx_cfg.lte.cycle_length,edrx_cfg.gsm.ptw, edrx_cfg.gsm.cycle_length);
#else
	MSG_HIGH_DS_6(MM_SUB, "=MM= EFS edrx config read: status = %d, lte_enabled = %d \
    lte_ptw = %d. lte_cycle_length = %d gsm_ptw = %d gsm_cycle_length = %d",
    status,mm_edrx_cfg.lte.enabled,
    edrx_cfg.lte.ptw, edrx_cfg.lte.cycle_length,edrx_cfg.gsm.ptw, edrx_cfg.gsm.cycle_length);
#endif

#ifdef FEATURE_CIOT
    MSG_HIGH_DS_7(MM_SUB, "=MM= EFS edrx config read: version = %d, M1_enabled = %d, NB1_enabled = %d,\
    M1_ptw = %d. M1_cycle_length = %d NB1_ptw = %d NB1_cycle_length = %d",
    mm_edrx_cfg.version, mm_edrx_cfg.M1.enabled, mm_edrx_cfg.NB1.enabled,
    mm_edrx_cfg.M1.ptw, mm_edrx_cfg.M1.cycle_length,mm_edrx_cfg.NB1.ptw, mm_edrx_cfg.NB1.cycle_length);
#endif
#ifdef FEATURE_NAS_GW
    ULOG_RT_PRINTF_5(mm_get_ulog_handle(),"=MM= EFS edrx config read: gsm_enabled = %d, umts_enabled = %d, lte_enabled = %d, lte_ptw = %d. lte_cycle_length = %d",
					 mm_edrx_cfg.gsm.enabled, mm_edrx_cfg.umts.enabled, mm_edrx_cfg.lte.enabled,
                     mm_edrx_cfg.lte.ptw, mm_edrx_cfg.lte.cycle_length);
#else
	ULOG_RT_PRINTF_3(mm_get_ulog_handle(),"=MM= EFS edrx config read: lte_enabled = %d, lte_ptw = %d. lte_cycle_length = %d",
                     mm_edrx_cfg.lte.enabled, mm_edrx_cfg.lte.ptw, mm_edrx_cfg.lte.cycle_length);
#endif

#ifdef FEATURE_CIOT
    ULOG_RT_PRINTF_6(mm_get_ulog_handle(),"=MM= EFS edrx config read: M1_enabled = %d, NB1_enabled = %d, M1_ptw = %d. M1_cycle_length = %d NB1_ptw = %d. NB1_cycle_length = %d",
                     mm_edrx_cfg.M1.enabled,
                     mm_edrx_cfg.NB1.enabled,  mm_edrx_cfg.M1.ptw, mm_edrx_cfg.M1.cycle_length,
                     mm_edrx_cfg.NB1.ptw, mm_edrx_cfg.NB1.cycle_length);
#endif
#ifdef FEATURE_NAS_GW
/**************************************************
   Subscription sepcfic data initilization and NV read 
**************************************************/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mm_sub_id = MM_AS_ID_1;
  for ( index = 0; index < mm_sim_max_subscription ; index++ )
#endif
  {

#if defined(FEATURE_LTE)
   mm_lai_change_force_lau_for_emergency = FALSE;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  #ifdef FEATURE_DUAL_DATA
    mm_data_pri_value = 0;
  #endif 
    mm_sub_id++;
#endif
  }
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mm_sub_id = MM_AS_ID_1;
#endif
#endif
    MSG_HIGH_DS_1(MM_SUB, "=MM= NV_NAS_REL_COMPLIANCE : %d",mm_nv_nas_rel_compliance);
  mm_read_carrier_specific_efs();
}


/*===========================================================================

FUNCTION MM_RESET_GLOBAL_VARIABLES

DESCRIPTION
  This function resets the MM global variables

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_reset_global_variables(void)
{
#ifdef FEATURE_NAS_GW
   int       i = 0;
   byte    null_lai[LAI_SIZE] = { 0xff, 0xff, 0xff, 0, 0 };
#endif
   gmm_attach_fail_counter = 0;
   gmm_trigger_attach_on_ps_call = FALSE;
   mm_serving_plmn.arfcn.num = 0xffff;
   mm_serving_plmn.arfcn.band = SYS_BAND_NONE;
   mm_serving_plmn.bsic = 0xff;
   mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
   mm_serving_plmn.t3122_running = TRUE;
   mm_psm_ready_req_rejected = FALSE;
   mm_ps_auth_sent_to_card = FALSE;
   mm_cs_auth_sent_to_card = FALSE;    
     /*Reset search in progress during deact  */
   mm_plmn_search_in_progress = FALSE;
#ifdef FEATURE_NAS_GW
   mm_system_information.T3212_timer_value = 0;

   mm_system_information.cell_access = MM_AS_ACCESS_ALL_CALLS; 
   mm_last_reg_dsac_information.cell_access = MM_AS_ACCESS_ALL_CALLS;
#ifdef FEATURE_DSAC
   mm_invalidate_dsac_info();
   mm_invalidate_ppac_info();
#endif  
   mm_cnm_rel_req_received          = FALSE;
   mm_pending_no_service_to_reg     = FALSE;
   mm_held_cnm_service_req.present  = FALSE;

   mm_reg_waiting_for_reg_cnf       = FALSE; 

   mm_ready_to_send_reg_cnf         = FALSE; 

   mm_needs_to_send_mmr_service_ind = FALSE; 

   mm_waiting_for_service_cnf       = FALSE; 

   mmcoord_reg_req_pending          = FALSE; 
#if defined (FEATURE_WRLF_SYSTEM_SEL) || defined (FEATURE_LTE)
   mm_rlf_stop_mode_pending = MM_RLF_STOP_MODE_PENDING_NONE;
#endif

   mm_waiting_for_deact_cnf         = FALSE;

   mm_waiting_for_stop_mode_cnf     = FALSE;
   mm_serving_plmn.egprs_supported   = FALSE;

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
   WtoGCCO_state = CCO_INACTIVE;  
#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */

#ifdef FEATURE_INTERRAT_PCCO_GTOW
   GtoWCCO_state  = CCO_INACTIVE;  
#endif /* FEATURE_INTERRAT_PCCO_GTOW */      
   mm_lu_attempt_counter = 0; 

   mm_initiate_lu_upon_entry_into_new_cell = FALSE;

   mm_follow_on_request                    = FALSE;  

   mm_follow_on_proceed                    = FALSE;  

   mm_T3213_timed_out                      = FALSE;

   mm_idle_transition_timer                = NULL; 

   mm_pending_lu_request                   = NO_LU; 
   mm_lu_start_reason                      = NO_LU;

   mm_csfb_service_ind_status = MM_CSFB_SEND_SERVICE_STATUS_NONE;

    /* Initialize the CNM service establish cause */
   mm_cnm_srv_est_cause = MM_RR_NO_CNM_SERVICE; 
   mm_connection_type = NO_MM_CONNECTION;

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   mm_lcs_active = FALSE; 
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

   mm_rat_handler_status = MM_RAT_INITIAL; 
#ifdef FEATURE_NAS_GW
#if defined FEATURE_MMCP_EGPRS || defined FEATURE_GSM_DTM || defined FEATURE_HSDPA
   mm_mmr_cell_service_ind_pending = FALSE;
#endif
#endif
   //sys_plmn_undefine_plmn_id(&mmcoord_mmr_reg_cnf.service_state.plmn);   

   mm_to_only_reg = FALSE;
   mm_previous_serving_rat = SYS_RAT_NONE;

#ifdef FEATURE_NAS_GW
#ifdef FEATURE_MODEM_HEAP
   if(pending_mm_msg != NULL)
   {
     modem_mem_free(pending_mm_msg,MODEM_MEM_CLIENT_NAS);
     pending_mm_msg = NULL;
     pending_mm_msg_valid = FALSE;
   }
#endif
#endif
#ifdef FEATURE_NAS_ECALL
   mm_need_to_perform_ecall_inactivity = FALSE;
   mm_call_est_cause = NO_CNM_SERVICE;  
#endif
#if defined(FEATURE_LTE)
   mm_service_loss_on_multimode_rat = SYS_RAT_NONE;
#endif
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
   mm_pending_lu_type = MM_LU_CAUSE_RESERVED;
#endif
#if defined(FEATURE_WRLF_SYSTEM_SEL) && defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_CS_DISABLED)
   mm_wcdma_rlf_state = WCDMA_RLF_IDLE;
   mm_wrlf_rat_change_pending = FALSE;
#endif
   mm_lu_pended_nmo_unknown = FALSE;

   /*Reset serving LAI */
   mm_serving_lai[0] = 0xFF;
   mm_serving_lai[1] = 0xFF;
   mm_serving_lai[2] = 0xFF;
   mm_serving_lai[3] = 0xFF;
   mm_serving_lai[4] = 0xFE;

   for ( i=0; i< MAX_LU_ATTEMPT_SIZE; i++)
   {
     mm_lu_reject_cause[i] = 0xFF;
   }   
#ifdef FEATURE_TC
   mm_tc_is_active = FALSE;
#endif
#ifdef FEATURE_ENHANCED_NW_SELECTION
  /* ----------------------------
  ** Default timeout = 6 minutes
  ** ---------------------------- */
   mm_foreground_search_timer_value =360000 ;
   mm_foreground_search_lai_timer_list.length =0;
   for( i=0; i< MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH; i++)
   {
    memscpy(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai, LAI_SIZE, null_lai, LAI_SIZE);
    mm_foreground_search_lai_timer_list.lai_timer_list[i].rac = 0xFF;
    mm_foreground_search_lai_timer_list.lai_timer_list[i].is_ps_registered = FALSE;
    mm_foreground_search_lai_timer_list.lai_timer_list[i].ps_attempt_counter = 0;
   }
#endif

#ifdef FEATURE_LTE
   mm_stop_mode_local_detach_peding = FALSE;
   mm_ps_detach_type                = SYS_PS_DETACH_TYPE_NONE;
#endif

   mm_service_ind_blocked_due_to_cs_reg = FALSE;
   mm_service_ind_blocked_due_to_irat_cs_reg = FALSE;
   mm_connection_activated = FALSE;
#ifdef FEATURE_LTE
   mm_csfb_is_blind_redir_to_lte_req_to_be_sent = FALSE;
   mm_csfb_set_state(MM_CSFB_STATE_NULL);
#endif
   mm_is_foreground_search_to_be_initiated  = TRUE;
   
   mm_is_gmm_moved_dereg_due_to_csreg       = FALSE;
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
  mm_utran_ps_voice_parameters.is_ps_voice_on_utran_supported = FALSE;
  mm_utran_ps_voice_parameters.is_last_utran_voims_supported = FALSE;
#endif

#ifdef FEATURE_NAS_REL10
   mm_store_integrity_protected     = FALSE;

   mm_stored_per_ms_t3212_value = 0;
   mm_is_per_ms_t3212_present   = FALSE;
#endif

  mm_state = MM_NULL;
  mm_substate_control(MM_STD_DEACTIVATE);
  mm_previous_idle_substate = NULL;
  mm_llc_needs_to_be_resumed = FALSE;
#if defined(FEATURE_WRLF_SYSTEM_SEL) && defined(FEATURE_NAS_GW)
   mm_received_suitable_search_end_ind = FALSE;
#endif
#ifdef FEATURE_LTE
  mm_rrc_pch_state_info.is_in_pch_state = FALSE;
#endif
#ifdef FEATURE_SGLTE
  if(MM_IS_IN_SGLTE_MODE)
  {
    mm_sglte_current_stack_status = SGLTE_INACTIVE_STATE;
    mm_hard_abort_ue_sglte_mode_switch_pending = FALSE;
  }
  mm_no_service_ind_during_split = FALSE;
#else
#ifdef FEATURE_1XSRLTE
  mm_hard_abort_ue_sglte_mode_switch_pending = FALSE;
#endif
#endif

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  mm_is_sim_busy = FALSE;
#endif
   mm_coord_camped_cell_id = 0xFF;
   mm_lu_reject_info.mm_conn_rej_timer_value = 0;

  mm_reg_req_revd_active_call = FALSE;
   
  (void)memscpy( mm_previous_serving_lai, LAI_SIZE,
                 null_lai,
                 LAI_SIZE );

#ifdef FEATURE_NAS_REL11
#ifdef FEATURE_WCDMA
  mm_is_first_rrc_connection_attempt = TRUE;
#endif
#endif
  mm_is_cs_connected_due_to_srvcc    = FALSE;
  
#endif
  mm_reg_cause            = MMR_CAUSE_NONE;
  mm_is_gsm_active_rat_for_thermal_read = FALSE;
}


boolean is_cs_lte_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
#endif

  return is_lte_mode_per_nas_stacks(stack_id_nas);
}

/*===========================================================================

FUNCTION mm_is_rat_lte

DESCRIPTION
  This function verifies if the passed rat is of LTE type
  
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if successful, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_is_rat_lte
(
  sys_radio_access_tech_e_type rat
)
{
    return ((rat == SYS_RAT_LTE_RADIO_ACCESS)
#ifdef FEATURE_CIOT
    || (rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
#ifndef FEATURE_CATM1_DISABLE
    || (rat == SYS_RAT_LTE_M1_RADIO_ACCESS)
#endif
#endif
    );
}
/*===========================================================================

FUNCTION mm_checkif_rat_pri_list_has_lte_rat

DESCRIPTION
  This function verifies if the passed rat is present in the
  rat priority list parameter.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if successful, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_checkif_rat_pri_list_has_lte_rat
(
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info
)
{
    return (mm_checkif_rat_pri_list_has_rat(rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS)
#ifdef FEATURE_CIOT
    || mm_checkif_rat_pri_list_has_rat(rat_pri_list_info, SYS_RAT_LTE_NB1_RADIO_ACCESS)
    || mm_checkif_rat_pri_list_has_rat(rat_pri_list_info, SYS_RAT_LTE_M1_RADIO_ACCESS)
#endif
    );
}

/*===========================================================================

FUNCTION IS_LTE_MODE

DESCRIPTION
  This function tells if the UE can operate in UMTS op mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if UMTS op mode, FALSE otherwise

SIDE EFFECTS
  None
 
===========================================================================*/

boolean is_lte_mode(void)
{
   if (mm_is_rat_lte(mm_serving_plmn.info.active_rat))
   {
      return TRUE;
   }

   return FALSE;
}



void mm_read_camp_nv(void)
{

   mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;

#ifdef FEATURE_NAS_GW
   mm_nas_nv_items_T mm_nas_nv_items;
#endif
#ifdef FEATURE_CIOT
   boolean mt_page_rach_retry_needed;
#endif

  if ((mm_emm_read_nv & (1<<MM_EMM_CAMP)))
  {
   memset( &mm_emm_manual_search_timer_value, 0x00, sizeof(sys_manual_search_timer_value_type) );
   if((status = mcfg_fs_read("/nv/item_files/modem/nas/mm_manual_search_timer_value", &mm_emm_manual_search_timer_value,
                       sizeof(sys_manual_search_timer_value_type), MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1)) == MCFG_FS_STATUS_OK)
   { 
   	 mm_partial_search_allowed_set(TRUE);
     if(mm_emm_manual_search_timer_value.FirstPLMNTimer >= mm_emm_manual_search_timer_value.MaxManualPLMNTimer)
     {
        mm_emm_manual_search_timer_value.FirstPLMNTimer = 0;
     }
     else if(mm_emm_manual_search_timer_value.MorePLMNTimer >= mm_emm_manual_search_timer_value.MaxManualPLMNTimer)
     {
        mm_emm_manual_search_timer_value.MorePLMNTimer = 0;
     }
     MSG_HIGH_DS_3(MM_SUB,"=MM= Manual Search timer Value FIRST %u MORE %u MAX %u",
                                        mm_emm_manual_search_timer_value.FirstPLMNTimer,
                                        mm_emm_manual_search_timer_value.MorePLMNTimer,
                                        mm_emm_manual_search_timer_value.MaxManualPLMNTimer);

   }
   else
   {
     /* Setting this value to zero so that default can be 0*/
       mm_emm_manual_search_timer_value.FirstPLMNTimer = 0;
       mm_emm_manual_search_timer_value.MorePLMNTimer =  0;
       mm_emm_manual_search_timer_value.MaxManualPLMNTimer = 0;
   }

   MSG_HIGH_DS_6(MM_SUB,"=MM= Manual Search  first timer Value status = %d length = %d, version %u,read Value from NV FIRST %u MORE %u MAX %u ",
                                       status,
                                       sizeof(sys_manual_search_timer_value_type),
                                       mm_emm_manual_search_timer_value.version,
                                       mm_emm_manual_search_timer_value.FirstPLMNTimer,
                                       mm_emm_manual_search_timer_value.MorePLMNTimer,
                                       mm_emm_manual_search_timer_value.MaxManualPLMNTimer);

#ifdef FEATURE_CIOT
   status = mcfg_fs_read("/nv/item_files/modem/nas/mt_page_rach_retry_needed",
                           &mt_page_rach_retry_needed,
                           sizeof(boolean),
                           MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);
   emm_ctrl_data_ptr->mt_page_rach_retry_needed = TRUE;
   if(status == MCFG_FS_STATUS_OK)
   {
     emm_ctrl_data_ptr->mt_page_rach_retry_needed = mt_page_rach_retry_needed;
   }
   MSG_HIGH_DS_2(MM_SUB ,"=MM= mt_page_rach_retry_needed status : mt_page_rach_retry_needed %d ",status,mt_page_rach_retry_needed);
#endif

#ifdef FEATURE_NAS_GW
   status = mcfg_fs_read("/nv/item_files/modem/nas/nas_config_feature",
                           &mm_nas_nv_items,
                           sizeof(mm_nas_nv_items_T),
                           MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);

   MSG_HIGH_DS_1(MM_SUB ,"=MM= NV NAS CONFIG FEATURE status : %d ",status);

   if (status != MCFG_FS_STATUS_OK)
   {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= Unable to read NV nas_config_feature, status : %d ",status);
      mm_nas_feature_to_be_configured.als_itc_value = (byte)(ALS_ITC_VALUE_4) ;
      mm_nas_feature_to_be_configured.No_auto_answer_on_hold = 0;
      mm_nas_feature_to_be_configured.MT_Call_reject_cause = 21; /*Call rejected*/
      mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page = 0;
      mm_nas_feature_to_be_configured.CS_Domain_absolutely_preferred = 0;
   /* mn global preserve_ui_speech_codec_preference will be set to default value of 0 */
     preserve_ui_speech_codec_preference = 0;
   }
   else
   {
#ifdef FEATURE_GSM_CB
      cb_get_nv_items
      (
        &mm_nas_nv_items
      ); 
#endif

   memscpy(&mm_nas_feature_to_be_configured,sizeof(mm_nas_feature_to_be_configured_T),&mm_nas_nv_items,sizeof(mm_nas_feature_to_be_configured_T));

   MSG_HIGH_DS_5(MM_SUB ,"=MM= NV NAS CONFIG FEATURE  als_itc_value: %d, No_auto_answer_on_hold: %d, MT_Call_reject_cause: %d, UE_init_rel_on_spurious_page: %d, CS_Domain_absolutely_preferred: %d ",
                          mm_nas_feature_to_be_configured.als_itc_value,
                          mm_nas_feature_to_be_configured.No_auto_answer_on_hold,
                          mm_nas_feature_to_be_configured.MT_Call_reject_cause,
                          mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page,
                          mm_nas_feature_to_be_configured.CS_Domain_absolutely_preferred);
   if (mm_nas_feature_to_be_configured.als_itc_value != (byte)(ALS_ITC_VALUE_6))
   {
     mm_nas_feature_to_be_configured.als_itc_value	= (byte)( ALS_ITC_VALUE_4);
   }
   if (mm_nas_feature_to_be_configured.No_auto_answer_on_hold !=1)
   {
      mm_nas_feature_to_be_configured.No_auto_answer_on_hold  =0;
   }
   if (mm_nas_feature_to_be_configured.MT_Call_reject_cause != 17)
   {
      mm_nas_feature_to_be_configured.MT_Call_reject_cause = 21;
   }
   if (mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page != 1)
   {
      mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page = 0;
   }
   if (mm_nas_feature_to_be_configured.CS_Domain_absolutely_preferred != 1)
   {
      mm_nas_feature_to_be_configured.CS_Domain_absolutely_preferred = 0;
   }
   if((mm_nas_nv_items.version >= 2) && (mm_nas_nv_items.cc_preserve_ui_speech_codec_preference != 0))
   {
      preserve_ui_speech_codec_preference = 1;
   }
   else
   {
      preserve_ui_speech_codec_preference = 0;
   }
   if(mm_nas_nv_items.version >= 6)
   {
      if(mm_nas_nv_items.ss_screaning_indicator > 1)
      {
         ss_screening_indicator = 0x01;
      }
      else
      {
         ss_screening_indicator = mm_nas_nv_items.ss_screaning_indicator;
      }
   }
   if(mm_nas_nv_items.version >= 7)
   {
      if(mm_nas_nv_items.retry_em_call_on_access_blocked_cell > 1)
      {
         mm_retry_em_call_on_access_blocked_cell = 0x01;
      }
      else
      {
         mm_retry_em_call_on_access_blocked_cell	 =	mm_nas_nv_items.retry_em_call_on_access_blocked_cell;
      }
   }

   if(mm_nas_nv_items.version >= 8)
   {
      if(mm_nas_nv_items.mm_managed_roaming_retry_lu_in_manual_mode > 1)
      {
         mm_managed_roaming_retry_lu_in_manual_mode = 0x01;
      }
      else
      {
         mm_managed_roaming_retry_lu_in_manual_mode	=  mm_nas_nv_items.mm_managed_roaming_retry_lu_in_manual_mode;
      }
   }
 #ifdef DISABLE_9x05_OPTIMIZATIONS  
   if(mm_nas_nv_items.version >= 9)
   {
      if(mm_nas_nv_items.optimised_ps_signalling_connection_release > 1)
      {
         mm_optimised_ps_signalling_connection_release = 0x01;
      }
      else
      {
         mm_optimised_ps_signalling_connection_release   =  mm_nas_nv_items.optimised_ps_signalling_connection_release;
      }			
   }
#endif

   MSG_HIGH_DS_1(MM_SUB ,"=MM= version of mn_nas_nv_items = %d ",mm_nas_nv_items.version);

   if(mm_nas_nv_items.version >= 10)
   {
      if(mm_nas_nv_items.ps_sig_conn_rel_after_ps_sms >= 1)
      {
         mm_ps_sig_conn_rel_after_ps_sms = 0x01;
      }
      else
      {
         mm_ps_sig_conn_rel_after_ps_sms	= 0;
      }
   }
   MSG_HIGH_DS_1(MM_SUB ,"=MM= value of ps_sig_conn_rel_after_ps_sms = %d ",mm_nas_nv_items.ps_sig_conn_rel_after_ps_sms);

   if (mm_nas_nv_items.version >= 14)
   {
      if(mm_nas_nv_items.psm_local_detach_enabled > 1)
      {
         gmm_psm_local_detach_enabled = 0x01;
      }
      else
      {
         gmm_psm_local_detach_enabled  = mm_nas_nv_items.psm_local_detach_enabled;
      }
   }
   MSG_HIGH_DS_1(MM_SUB ,"=MM= value of psm_local_detach_enabled = %d ",mm_nas_nv_items.psm_local_detach_enabled);
  
   }
#ifdef FEATURE_NAS_REL11  
   mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page = 0;
   MSG_HIGH_DS_1(MM_SUB ,"=MM= Since Feature Rel_11 is enabled, NV NAS CONFIG FEATURE  UE_init_rel_on_spurious_page is forced to: %d",mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page);
#endif

   MSG_HIGH_DS_2(MM_SUB, "=MM= preserve_ui_speech_codec_preference is set to %d, status of EFS read = %d ",preserve_ui_speech_codec_preference,status);
#ifndef FEATURE_NAS_DISABLE_VOICE
   mn_set_nv_items(&mm_nas_feature_to_be_configured);
#endif

#endif
   if((status = mcfg_fs_read("/nv/item_files/modem/mmode/qmss_enabled",
                 &mm_emm_is_qrd_device,sizeof(boolean), 
                  MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1)) == MCFG_FS_STATUS_OK) 
   {
     MSG_HIGH_1("MM: setting QRD device setting to %d",mm_emm_is_qrd_device);
   }
   else
   {
     MSG_HIGH_1("MM: setting QRD device to false as read failed, status = %d", status);
     mm_emm_is_qrd_device = FALSE;
   }

   mm_is_qrd_device = FALSE;    

   if((status = mcfg_fs_read("/nv/item_files/modem/mmode/qmss_enabled",
                      &mm_is_qrd_device,sizeof(boolean), 
                      MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1)) == MCFG_FS_STATUS_OK)
   {
     MSG_HIGH("MM: setting QRD device setting to %d",mm_is_qrd_device,0,0);
   }
   else
   {
     MSG_HIGH("MM: setting QRD device to false as read failed",0,0,0);
     mm_is_qrd_device = FALSE;
   }

#ifdef FEATURE_NAS_GW
     /* Read from NV whether or not csvti nv qmss is enable */
   if((status = mcfg_fs_read("/nv/item_files/modem/mmode/qmss_enabled", 
                     &nas_is_qmss_enabled, sizeof(byte),
                      MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1))== MCFG_FS_STATUS_OK) 
   {
     if(nas_is_qmss_enabled != TRUE)
     {
        nas_is_qmss_enabled = FALSE;
     }
     MSG_HIGH("MM: setting nas_is_qmss_enabled to %d",nas_is_qmss_enabled,0,0);
   }
   else
   {
     nas_is_qmss_enabled = FALSE;
     MSG_HIGH_DS_2(MM_SUB,"MM: nas_is_qmss_enabled could not be set from EFS, nas_is_qmss_enabled = %d, STATUS = %d",nas_is_qmss_enabled,status);
   }
   mm_emm_read_nv = (mm_emm_read_nv ^ (mm_emm_read_nv & (1<<MM_EMM_CAMP)));
 #endif
  
  }
 
}


void mm_read_registration_nv(void)
{

   mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
   mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;
   ghdi_status_T nv_status = GHDI_SUCCESS;
   cm_operator_e_type operator_name = OPERATOR_NULL;

#ifdef FEATURE_LTE
   ue_security_capability ue_security;
   emm_ctrl_data_type *emm_s1_ctrl_data_ptr = NULL;
   boolean lsti_config = FALSE;
#endif

#ifdef FEATURE_LTE
   emm_s1_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif

#ifdef FEATURE_TDSCDMA
   uint8 nv_special_tdscdma_test_setting_enable = 0;
#endif

  if (mm_emm_read_nv & (1<<MM_EMM_REGISTER))
  {
#ifdef FEATURE_NAS_GW
     mm_nv_force_sgsnr_r99 = NV_FORCE_UE_REL99;
     mm_nv_force_mscr_r99 = NV_FORCE_UE_REL99;
     mm_nv_tighter_capability         = 1;

     status = mcfg_fs_read("/nv/item_files/modem/nas/tighter_capability", 
                          &mm_nv_tighter_capability,sizeof(mm_nv_tighter_capability), 
                          MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);
   
   if ((status != MCFG_FS_STATUS_OK) || (mm_nv_tighter_capability > 3))
   {
     mm_nv_tighter_capability = 1;
   }

   MSG_HIGH_DS_2(MM_SUB, "=MM= NV NAS Tighter capability : %d, %d",status,mm_nv_tighter_capability);
#endif

#ifdef FEATURE_CIOT
    if((status = mcfg_fs_read("/nv/item_files/modem/nas/dcn_list", 
                           &emm_s1_ctrl_data_ptr->dcn_list, 
                           sizeof(lte_nas_dcn_lst_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_0(MM_SUB ,"=EMM= Read DCN list from EFS ");
    }
    else
    {
      MSG_HIGH_DS_1(MM_SUB ,"=EMM= FAILED to read DCN list from EFS status = %d", status);
    }
#endif
#ifdef FEATURE_NAS_EAB
  /* If below internal NV is enabled then Rel11 EAB feature support in GSM will be suported*/
  if((status = mcfg_fs_read("/nv/item_files/modem/nas/nas_gsm_eab_support", 
                             &nas_gsm_eab_support, sizeof(boolean),
                             MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1)) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= Setting NV nas_gsm_eab_support to %d",nas_gsm_eab_support);
  }
  else
  {
    nas_gsm_eab_support = FALSE;
    MSG_HIGH_DS_1(MM_SUB, "=MM= NV read failed, setting nas_gsm_eab_support to default %d",nas_gsm_eab_support);
  }
#endif

  /* If this NV is enabled then eDRX negotiation will always be successful in NAS irrespective of network provided value*/
  if((status = mcfg_fs_read("/nv/item_files/modem/nas/edrx_gsm_fake_negotiation", 
                             &edrx_gsm_fake_negotiation, sizeof(boolean),
                             MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1)) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= Setting NV edrx_gsm_fake_negotiation to %d",edrx_gsm_fake_negotiation);
  }
  else
  {
    edrx_gsm_fake_negotiation = FALSE;
    MSG_HIGH_DS_1(MM_SUB, "=MM= NV read failed, setting edrx_gsm_fake_negotiation to default %d",edrx_gsm_fake_negotiation);
  }

#ifdef FEATURE_NAS_GW

  /**************************************************
   Subscription sepcfic data initilization and NV read 
**************************************************/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mm_sub_id = MM_AS_ID_1;
  for ( index = 0; index < mm_sim_max_subscription ; index++ )
#endif
  {
     /* Read the UE Revision control NV item */


   operator_name = OPERATOR_NULL;

   if(mcfg_fs_read("/nv/item_files/modem/mmode/operator_name", 
             &operator_name, sizeof(cm_operator_e_type),
             MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id) == MCFG_FS_STATUS_OK)
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= Setting operator to %d",operator_name);
   }
   else
   {
     operator_name = OPERATOR_NULL;
     MSG_HIGH_DS_1(MM_SUB, "=MM= NV read failed, setting operator to NULL %d",operator_name);
   }

   if (mm_ghdi_nvmem_read(NV_FORCE_UE_SGSNR_R99_I,
            (ghdi_nvmem_data_T *)&mm_nv_force_sgsnr_r99) != GHDI_SUCCESS)
   {
     if ((operator_name == OPERATOR_CMCC) || (operator_name == OPERATOR_CU))
     {
       mm_nv_force_sgsnr_r99 = NV_FORCE_UE_DYNAMIC;
     }
     else
     {
       mm_nv_force_sgsnr_r99 = NV_FORCE_UE_REL99;
     }
   }
   else if (mm_nv_force_sgsnr_r99 >= NV_REL_MODE_MAX)
   {
     if ((operator_name == OPERATOR_CMCC) || (operator_name == OPERATOR_CU))
     {
       MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for FORCE_SGSNR_R99 NV %d",
             mm_nv_force_mscr_r99,0,0);
       mm_nv_force_sgsnr_r99 = NV_FORCE_UE_DYNAMIC;
     }
     else
     {
     MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for FORCE_SGSNR_R99 NV %d, default R 99",
          mm_nv_force_sgsnr_r99,0,0);
     mm_nv_force_sgsnr_r99 = NV_FORCE_UE_REL99;
   }
   }

   if (mm_ghdi_nvmem_read(NV_FORCE_UE_MSCR_R99_I,
            (ghdi_nvmem_data_T *)&mm_nv_force_mscr_r99) != GHDI_SUCCESS)
   {
     if ((operator_name == OPERATOR_CMCC) || (operator_name == OPERATOR_CU))
     {
       mm_nv_force_mscr_r99 = NV_FORCE_UE_DYNAMIC;
     }
     else
     {
       mm_nv_force_mscr_r99 = NV_FORCE_UE_REL99;
     }
   }
   else if (mm_nv_force_mscr_r99 >= NV_REL_MODE_MAX)
   {
     if ((operator_name == OPERATOR_CMCC) || (operator_name == OPERATOR_CU))
     {
       MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for FORCE_MSCR_R99 NV %d",
                          mm_nv_force_mscr_r99,0,0);
       mm_nv_force_mscr_r99 = NV_FORCE_UE_DYNAMIC;
     }
     else
     {
       MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for FORCE_MSCR_R99 NV %d, default R99",
                          mm_nv_force_mscr_r99,0,0);
       mm_nv_force_mscr_r99 = NV_FORCE_UE_REL99;
     }
   }
   /*
   Update mm_nv_nas_rel_compliance as '0' when either of NV_FORCE_UE_SGSNR_R99_I
   or NV_FORCE_UE_SGSNR_R99_I indicate '0'.
   */
   if((mm_nv_force_sgsnr_r99 == NV_FORCE_UE_REL97) || (mm_nv_force_mscr_r99 == NV_FORCE_UE_REL97))
   {
     mm_nv_nas_rel_compliance = NV_NAS_COMPLIANCE_REL99;
   }

   MSG_HIGH_DS_2(MM_SUB, "=MM= NV_FORCE_UE_SGSNR_R99 : %d, NV_FORCE_UE_MSCR_R99_I : %d",
               mm_nv_force_sgsnr_r99, mm_nv_force_mscr_r99);


    if (mm_ghdi_nvmem_read(NV_MGRF_SUPPORTED_I,
       (ghdi_nvmem_data_T *)&mm_managed_roaming_enabled) != GHDI_SUCCESS)
    {
       mm_managed_roaming_enabled = FALSE;  /* Set the default value */
       MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to read NV for MANAGED ROAMING flag; disabled");
    }
    else
    {
       if ((mm_managed_roaming_enabled != FALSE) && (mm_managed_roaming_enabled != TRUE))
       {
         MSG_HIGH_DS_1(MM_SUB ,"=MM= Wrong value for MANAGED ROAMING flag %d", mm_managed_roaming_enabled);
         mm_managed_roaming_enabled = FALSE;  /* Set the default value */
       }
    }


  /* Read the UE NAS Release Compliance control NV item */
  if (ghdi_nvmem_read(NV_NAS_RELEASE_COMPLIANCE_I,
            (ghdi_nvmem_data_T *)&mm_nv_nas_rel_compliance) != GHDI_SUCCESS)
   {
      mm_nv_nas_rel_compliance = NV_NAS_COMPLIANCE_REL10;
   }
   else if (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL_MAX)
   {
      MSG_ERROR_DS(MM_SUB, "=MM=  Wrong value for NAS_REL_COMPLIANCE NV %d, default R 6",
          mm_nv_nas_rel_compliance,0,0);
      mm_nv_nas_rel_compliance = NV_NAS_COMPLIANCE_REL10;
   }


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    mm_sub_id++;
#endif
  }
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mm_sub_id = MM_AS_ID_1;
#endif

#endif


#if defined(FEATURE_GPRS_PS_HANDOVER) && defined(FEATURE_NAS_GW)
 if ((nv_status = ghdi_nvmem_read(NV_PSHO_FEATURE_SUPPORT_I ,
          (ghdi_nvmem_data_T *)&gmm_nv_psho_feature_support)) != GHDI_SUCCESS)
 {
   gmm_nv_psho_feature_support = FALSE;  /* Set the default value */
   MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for PSHO Feature support flag: %d", nv_status);
 }
 else
 {
   if ((gmm_nv_psho_feature_support != FALSE) && (gmm_nv_psho_feature_support != TRUE))
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= Wrong value for PSHO Feature support flag %d", gmm_nv_psho_feature_support);
     gmm_nv_psho_feature_support = FALSE;  /* Set the default value */
   }
 }
#endif


  
#ifdef FEATURE_GPRS_EDA
 if ((nv_status = ghdi_nvmem_read(NV_EDA_FEATURE_SUPPORT_I,
          (ghdi_nvmem_data_T *)&gmm_nv_eda_feature_support)) != GHDI_SUCCESS)
 {
   gmm_nv_eda_feature_support = GPRS_EDGE_EDA_SUPPORTED;  /* Set the default value */
   MSG_ERROR_DS_1(MM_SUB, "=MM= Unable to read NV for MSC12/EDA Support: %d", nv_status);
 }
 else if (gmm_nv_eda_feature_support > 1)
 {
   MSG_HIGH_DS_1(MM_SUB, "=MM= Invalid value for MSC12/EDA Support %d", gmm_nv_eda_feature_support);
   gmm_nv_eda_feature_support = GPRS_EDGE_EDA_SUPPORTED;  /* Set the default value */
 }
#endif 

  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/isr", 
                              &emm_s1_ctrl_data_ptr->isr_efs_flag, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'ISR_EFS_FLAG' is %d",emm_s1_ctrl_data_ptr->isr_efs_flag);
  }
  else
  {
    emm_s1_ctrl_data_ptr->isr_efs_flag = FALSE;
    MSG_HIGH_DS_1(MM_SUB,"=EMM= 'ISR_EFS_FLAG' file could not be read from EFS, status =%d ", efs_status);
  }

    /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/emm_combined_proc", 
                              &emm_s1_ctrl_data_ptr->emm_combined_proc_efs_flag, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'EMM_COMBINED_PROC_EFS_FLAG' is %d",emm_s1_ctrl_data_ptr->emm_combined_proc_efs_flag);
  }
  else
  {
    emm_s1_ctrl_data_ptr->emm_combined_proc_efs_flag = TRUE;
    MSG_HIGH_DS_1(MM_SUB,"=EMM= 'EMM_COMBINED_PROC_EFS_FLAG' file could not be read from EFS, status = %d", efs_status);
  }

    /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);

  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/avoid_guti_nas_security_check", 
                              &emm_s1_ctrl_data_ptr->avoid_guti_nas_security_check, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'AVOID_GUTI_NAS_SECURITY_CHECK' is %d",emm_s1_ctrl_data_ptr->avoid_guti_nas_security_check);
  }
  else
  {
    emm_s1_ctrl_data_ptr->avoid_guti_nas_security_check = FALSE;
    MSG_HIGH_DS_1(MM_SUB,"=EMM= 'AVOID_GUTI_NAS_SECURITY_CHECK' file could not be read from EFS, status = %d", efs_status);
  }

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

    /* GERAN cap */

  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);
#ifdef FEATURE_NAS_GW
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/geran_cap", 
                              &emm_disable_geran_cap, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= GERAN_CAP is %d",emm_disable_geran_cap);
  }
  else
  {
    emm_disable_geran_cap = FALSE;
    MSG_HIGH_DS_1(MM_SUB,"=EMM= GERAN_CAP file could not be read from EFS, status = %d", efs_status);
  }

#ifdef FEATURE_TDSCDMA
	/* Report to the Dog HB */
	dog_hb_report(DOG_MM_RPT);
  
	/*------------ Check for the TDS NV0 policy flag ------------*/
	if((efs_status = mcfg_fs_read("/nv/item_files/modem/tdscdma/rrc/special_test_setting_enabled", 
						&nv_special_tdscdma_test_setting_enable, 
										sizeof(uint8), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
	{
	  MSG_HIGH_DS_1(MM_SUB ,"=EMM= TDS RRC NV special_test_setting_enabled val = %d",
									  nv_special_tdscdma_test_setting_enable);
	}
	else
	{
	  MSG_ERROR_DS_1(MM_SUB,"=EMM= Unable to access TDS RRC NV	special_test_setting_enabled, status = %d", efs_status);
	  /* making as 2 for the failure case.*/
	  nv_special_tdscdma_test_setting_enable = 2;
	}
	
	if (nv_special_tdscdma_test_setting_enable != 2)
	{
	  /* Report to the Dog HB */
	  dog_hb_report(DOG_MM_RPT);
	  /*------------ Check for the TDS ciphering Flag ------------*/
	  if((efs_status = mcfg_fs_read("/nv/item_files/modem/tdscdma/rrc/ciphering_enabled", 
							   &emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag, 
							   sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
	  {
		MSG_HIGH_DS_1(MM_SUB ,"=EMM= NV TDS_RRC_CIPHERING_ENABLED = %d",
						  emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag);
	  }
	  else
	  {
		MSG_ERROR_DS_1(MM_SUB,"=EMM= Unable to access NV to read TDS_RRC_CIPHERING_ENABLED, status = %d", efs_status);
		emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag = FALSE;
	  }
	  
	  /* Report to the Dog HB */
	  dog_hb_report(DOG_MM_RPT);
	  
	  /*------------ Check for the TDS INTEGRITY Flag ------------*/
	  if((efs_status = mcfg_fs_read("/nv/item_files/modem/tdscdma/rrc/integrity_enabled", 
								&emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag, 
								sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
	  {
		MSG_HIGH_DS_1(MM_SUB ,"=EMM= NV TDS_RRC_INTEGRITY_ENABLED = %d",
							emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag);
	  }
	  else
	  {
		MSG_ERROR_DS_1(MM_SUB,"=EMM= Unable to access NV to read TDS_RRC_INTEGRITY_ENABLED, status = %d", efs_status);
		/*By default in the fail case make the	integrity as TRUE as per latest NV0 policy discussion */
		emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag = TRUE;
	  }
	  
	  /* Report to the Dog HB */
	  dog_hb_report(DOG_MM_RPT);
	  
	  
	  /*------------ Check for the TDS Fake Security Flag ------------*/
	  if(mcfg_fs_read("/nv/item_files/modem/tdscdma/rrc/fake_sec_enabled", 
								&emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag, 
								sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id ) == MCFG_FS_STATUS_OK)
	  {
		MSG_HIGH_DS_1(MM_SUB ,"=EMM= NV TDS_RRC_FAKE_SEC_ENABLED = %d",
							   emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag );
	  }
	  else
	  {
		MSG_ERROR_DS_1(MM_SUB,"=EMM= Unable to access NV to read TDS_RRC_FAKE_SEC_ENABLED, status = %d", efs_status);
		emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag = FALSE;
	  }
	  /*If TDS integrity is TRUE irrespective of fake security value make it FALSE	*/
	  if (emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag == TRUE )
	  {
		MSG_HIGH_DS_0(MM_SUB,"=EMM= TDS RRC integrity Enable Making Fake sec FALSE");
		emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag = FALSE;
	  }
	}
	else 
	{
	  /*Its the case when the NV0 policy says 2 value, treat it as fail 
		   make cipher = false , fake Security	as FALSE, intergrity as TRUE*/
	  emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag = FALSE;
	  emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag = TRUE;
	  emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag = FALSE;
	  
	}
#endif
#endif

    /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);

  if ((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/lte_nas_ue_sec_capability", 
                        &ue_security, 
                        sizeof(ue_security), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Read EFS : UE Security");
    if( ue_security.eps_encrypt_alg_128_eea0_supported )
    {
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea0_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_enc_exists = TRUE;
    }
    if (ue_security.eps_encrypt_alg_128_eea1_supported)
    {
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea1_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_enc_exists = TRUE;
    }
    if (ue_security.eps_encrypt_alg_128_eea2_supported)
    {
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea2_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_enc_exists = TRUE;
    }
    if (ue_security.eps_encrypt_alg_eea3_supported)
    {
      MSG_HIGH_DS_0(MM_SUB,"=EMM= ZUC Enc Algo supported thru NAS EFS ");
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_eea3_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_enc_exists = TRUE;
    }

    if (ue_security.eps_integrity_alg_128_eia1_supported)
    {
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_integrity_alg_128_eia1_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_int_exists = TRUE;
    }
    if (ue_security.eps_integrity_alg_128_eia2_supported)
    {
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_integrity_alg_128_eia2_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_int_exists = TRUE;
    }
    if (ue_security.eps_integrity_alg_128_eia3_supported)
    {
      MSG_HIGH_DS_0(MM_SUB,"=EMM= ZUC Int Algo supported thru NAS EFS ");
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_integrity_alg_128_eia3_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_int_exists = TRUE;
    }

  } else
  {
     /* temp */
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Read EFS : UE Security, NV read failed, status = %d", efs_status);
    emm_s1_ctrl_data_ptr->ue_security_capability.spare_bit = FALSE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea0_supported = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea1_supported = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea2_supported = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_eea3_supported = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_integrity_alg_128_eia1_supported = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_integrity_alg_128_eia2_supported = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_integrity_alg_128_eia3_supported = TRUE;

    emm_s1_ctrl_data_ptr->ue_security_capability.eps_int_exists = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_enc_exists = TRUE;

  }

  /* LSTI EFS should be read after emm_3gpp_spec_ver is read in the function
 emm_read_carrier_specific_efs() */
#ifdef FEATURE_LTE_REL9
  if(emm_s1_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL9)
  {
#endif

    /* Report to the Dog HB */
    dog_hb_report(DOG_MM_RPT);

    /* Read LSTI config from EFS */
    if ((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/lte_nas_lsti_config", 
                                &lsti_config, 
                                sizeof(byte), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=EMM= Read EFS : LSTI config %d", lsti_config);

      if(lsti_config == TRUE)
      {
        /* Default value - NULL encryption */
        emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea0_supported = TRUE;
        /* Default value - SNOW3G encryption*/
        /* Default value - spare bit - to indicate NULL MACI*/
        emm_s1_ctrl_data_ptr->ue_security_capability.spare_bit = TRUE;
         
        emm_s1_ctrl_data_ptr->ue_security_capability.eps_enc_exists = TRUE;
        emm_s1_ctrl_data_ptr->ue_security_capability.eps_int_exists = TRUE;
      }
    }  
#ifdef FEATURE_LTE_REL9
  }
#endif
  
  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);

  /* set the exclude_identity_type_ie from EFS */
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/exclude_ptmsi_type_field", 
             &emm_s1_ctrl_data_ptr->exclude_identity_type_ie, 
             sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= setting exclude_identity_type_ie to %d",emm_s1_ctrl_data_ptr->exclude_identity_type_ie);
  }
  else
  {
    emm_s1_ctrl_data_ptr->exclude_identity_type_ie = FALSE;
    MSG_HIGH_DS_1(MM_SUB,"=MM= exclude_identity_type_ie file could not be read from EFS, status = %d", efs_status);
  }

  #ifdef FEATURE_CIOT
  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);
  NAS_ENTER_CRIT_SECT(nas_ciot_config_crit_sec);

  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/nas_ciot_capability_config", 
                              &emm_s1_ctrl_data_ptr->nas_ciot_capability_config, 
                              sizeof(emm_s1_ctrl_data_ptr->nas_ciot_capability_config), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'CIOT capability' is %d",emm_s1_ctrl_data_ptr->nas_ciot_capability_config);
    if(mm_get_ulog_handle() != NULL)
     {
	  ULOG_RT_PRINTF_2(mm_get_ulog_handle(), "=EMM= Rest CIOT nas_ciot_capability_config = %d, efs_status = %d", 
							 emm_s1_ctrl_data_ptr->nas_ciot_capability_config,
							 efs_status);
     }
  }
  else
  {
    emm_s1_ctrl_data_ptr->nas_ciot_capability_config = 0x0120;
    MSG_HIGH_DS_1(MM_SUB,"=EMM= 'nas_ciot_capability_config' file could not be read from EFS, status = %x", efs_status);
    if(mm_get_ulog_handle() != NULL)
    {
	  ULOG_RT_PRINTF_2(mm_get_ulog_handle(), "=EMM= 'nas_ciot_capability_config' file could not be read from EFS, nas_ciot_capability_config %d status = %d", 
							 emm_s1_ctrl_data_ptr->nas_ciot_capability_config,
							 efs_status);
    }
  }
  NAS_EXIT_CRIT_SECT(nas_ciot_config_crit_sec);
#endif

#ifdef FEATURE_LTE_TO_1X
  dog_hb_report(DOG_MM_RPT);
  /* set the srvcc_capability_indicator from EFS */
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/l21xsrvcc_support", 
             &emm_s1_ctrl_data_ptr->onexsrvcc_capability_support, 
             sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB,"=MM= Setting 1x srvcc_capability_support to %d",
              emm_s1_ctrl_data_ptr->onexsrvcc_capability_support);
  }
  else
  {
    emm_s1_ctrl_data_ptr->onexsrvcc_capability_support = FALSE;
    MSG_HIGH_DS_2(MM_SUB,"=MM= 1x srvcc_capability_support file read failed. default %d, status = %d",
                        emm_s1_ctrl_data_ptr->onexsrvcc_capability_support, efs_status);
  }
#endif

#ifdef FEATURE_NAS_GW
  dog_hb_report(DOG_MM_RPT);
  /* set the l2g_srvcc_capability_indicator from EFS */
  if((efs_status = mcfg_fs_read("/nv/item_files/modem/nas/nas_l2g_srvcc_support", 
             &emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support, 
             sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id) ) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Setting L2G SRVCC support to %d",
              emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support);
  }
  else
  {
    emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support = TRUE;
    MSG_HIGH_DS_2(MM_SUB ,"=MM= L2G SRVCC support file read failed. default %d, status = %d",
                        emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support, efs_status);
  }


  dog_hb_report(DOG_MM_RPT);
#endif
  if((efs_status = mcfg_fs_read("/nvm/alpha/modem/nas/lte_nas_TIN_item", 
                           &emm_s1_ctrl_data_ptr->TIN, 
                           sizeof(nas_mm_tin_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id) ) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= Read TIN from EFS = %d", emm_s1_ctrl_data_ptr->TIN);
  }
  else
  {
    /* Default value */
    emm_set_and_write_tin_type(NAS_MM_TIN_NONE);
    MSG_HIGH_DS_2(MM_SUB ,"=EMM= FAILED to read TIN from EFS = %d, status = %d", emm_s1_ctrl_data_ptr->TIN, efs_status);
  }
  

#ifdef FEATURE_DUAL_SIM
   if(mm_sim_max_subscription > 1)
   {
      emm_ctrl_data_type *emm_s2_ctrl_data_ptr = NULL;
      mm_as_id = MM_AS_ID_2;
      mm_sub_id = mm_sub_id_stack[mm_as_id];
      emm_s2_ctrl_data_ptr = emm_db_get_ctrl_data();
      dog_hb_report(DOG_MM_RPT);
      if((efs_status = mcfg_fs_read("/nvm/alpha/modem/nas/lte_nas_TIN_item", 
                                      &emm_s2_ctrl_data_ptr->TIN, 
                                      sizeof(nas_mm_tin_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
      {
         MSG_HIGH_DS_1(MM_SUB ,"=EMM= Read TIN from EFS = %d", emm_s2_ctrl_data_ptr->TIN);
      }
      else
      {
          /* Default value */
         emm_set_and_write_tin_type(NAS_MM_TIN_NONE);
         MSG_HIGH_DS_2(MM_SUB ,"=EMM= FAILED to read TIN from EFS = %d, status = %d", emm_s2_ctrl_data_ptr->TIN, efs_status);
      }
      //Copy the EFS data to other stack which share the same value
#ifdef FEATURE_NAS_GW
    emm_s2_ctrl_data_ptr->l2g_srvcc_capability_support = emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support;
#endif
#ifdef FEATURE_CIOT
    NAS_ENTER_CRIT_SECT(nas_ciot_config_crit_sec);

    emm_s2_ctrl_data_ptr->nas_ciot_capability_config = emm_s1_ctrl_data_ptr->nas_ciot_capability_config;
    NAS_EXIT_CRIT_SECT(nas_ciot_config_crit_sec);
#endif
#ifdef FEATURE_LTE_TO_1X
    emm_s2_ctrl_data_ptr->onexsrvcc_capability_support = emm_s1_ctrl_data_ptr->onexsrvcc_capability_support;
#endif
    emm_s2_ctrl_data_ptr->isr_efs_flag = emm_s1_ctrl_data_ptr->isr_efs_flag;
    emm_s2_ctrl_data_ptr->emm_combined_proc_efs_flag = emm_s1_ctrl_data_ptr->emm_combined_proc_efs_flag;
    emm_s2_ctrl_data_ptr->avoid_guti_nas_security_check = emm_s1_ctrl_data_ptr->avoid_guti_nas_security_check;
#ifdef FEATURE_NAS_GW
#ifdef FEATURE_TDSCDMA
    emm_s2_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag = emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag;
    emm_s2_ctrl_data_ptr->emm_tds_nv_integ_enable_flag = emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag;
    emm_s2_ctrl_data_ptr->emm_tds_nv_fake_sec_flag = emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag;
#endif
#endif
    emm_s2_ctrl_data_ptr->ue_security_capability = emm_s1_ctrl_data_ptr->ue_security_capability;
    emm_s2_ctrl_data_ptr->exclude_identity_type_ie = emm_s1_ctrl_data_ptr->exclude_identity_type_ie;

      mm_as_id = MM_AS_ID_1;
      mm_sub_id = mm_sub_id_stack[mm_as_id];
   }
#endif


#if defined  FEATURE_LTE && defined FEATURE_3G_USER_NW_REDIR_TO_LTE
  /* IS ACCEPTED ON LTE */
  dog_hb_report(DOG_MM_RPT);
  status  = mcfg_fs_read("/nv/item_files/modem/nas/is_accepted_on_lte",
                         &emm_ctrl_data_ptr->is_accepted_on_lte,
                         sizeof(boolean),
                         MCFG_FS_TYPE_EFS,
                         (mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status == MCFG_FS_STATUS_OK)
  {
     MSG_HIGH_DS_1(MM_SUB ,"MM: Is_accepted_on_lte flag value : %d",
             emm_ctrl_data_ptr->is_accepted_on_lte);
  }
  else
  {
     emm_ctrl_data_ptr->is_accepted_on_lte = FALSE;
     MSG_HIGH_DS_2(MM_SUB,"MM: is_accepted_on_lte file could not be read from EFS %d. default %d",
                        status,emm_ctrl_data_ptr->is_accepted_on_lte);
  }
#endif

  mm_emm_read_nv = (mm_emm_read_nv ^ (mm_emm_read_nv & (1<<MM_EMM_REGISTER)));
#ifdef FEATURE_NAS_GW
  if (gmm_state == GMM_REGISTERED)
#endif
  {
	gmm_read_attach_complete_nv();
  }
  }

}


#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean is_lte_mode_per_nas_stacks( sys_modem_as_id_e_type as_id )
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if(IS_NOT_VALID_STACK_ID(as_id))
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Invalid STACK ID:%d", as_id+1,0,0);
     return FALSE;
   }

   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_LTE_RADIO_ACCESS)
   {
      return TRUE;
   }

   return FALSE;
#else
  (void)as_id;
  return is_lte_mode();
#endif

}


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION mm_process_mcfg_refresh

DESCRIPTION
  This function processes MCFG NV refresh indication for MM specific EFS/NVs

DEPENDENCIES
  None

RETURN VALUE
  Preferred band

SIDE EFFECTS
  None

===========================================================================*/
void mm_process_mcfg_refresh(void)
{
  mm_emm_read_nv = (byte) (mm_emm_read_nv|(1<<(byte)MM_MCFG_REFRESH));
  mm_read_carrier_specific_efs();

#ifdef FEATURE_NAS_GW

  /* NV_MGRF_SUPPORTED_I */
  if(mm_ghdi_nvmem_read(NV_MGRF_SUPPORTED_I,
     (ghdi_nvmem_data_T *)&mm_managed_roaming_enabled) != GHDI_SUCCESS)
  {
    mm_managed_roaming_enabled = FALSE;  /* Set the default value */
    MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to read NV for MANAGED ROAMING flag; disabled");
  }
  else
  {
    if((mm_managed_roaming_enabled != FALSE) && (mm_managed_roaming_enabled != TRUE))
    {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= Wrong value for MANAGED ROAMING flag %d", mm_managed_roaming_enabled);
      mm_managed_roaming_enabled = FALSE;  /* Set the default value */
    }
  }

#endif
  MSG_HIGH_DS_0(MM_SUB,"MM Refresh Done");
}
#endif


ghdi_nvmem_status_T mm_nvmem_default_values_generator(
       ghdi_nvmem_status_T in_status, nv_items_enum_type item_type, nv_item_type *nv_item)
{
   switch(item_type)
   {
      case NV_UE_RAT_CAPABILITY_I:
         nv_item->ue_rat_capability = ue_umts_only_rat;
         break;

      default:
         return in_status;
   }

   return NV_DONE_S;
}

/*===========================================================================

FUNCTION IS_GSM_MODE

DESCRIPTION
  This function tells if the UE can operate in GSM op mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if GSM is Active RAT, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean is_gsm_mode(void)
{
   if (mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS)
   {
      return TRUE;
   }

   return FALSE;
}

boolean is_gsm_mode_per_nas_stacks(sys_modem_as_id_e_type as_id)
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if(IS_NOT_VALID_STACK_ID(as_id))
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Invalid STACK ID:%d", as_id+1,0,0);
     return FALSE;
   }

   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_GSM_RADIO_ACCESS)
   {
      return TRUE;
   }

   return FALSE;
#else
  (void) as_id;
  return is_gsm_mode();
#endif
}

boolean is_cs_gsm_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
#endif

  return is_gsm_mode_per_nas_stacks(stack_id_nas);
}

boolean is_umts_mode(void)
{
#ifndef FEATURE_SEGMENT_LOADING
   if (mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)
   {
     return TRUE;
   }
   else if (mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS)
   {
     return TRUE;
   }
   else
   {
     return FALSE;
   }
#else
   if ((mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS) && 
       (get_wcdma_interface()))
   {
     return TRUE;
   }
   else if ((mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS) &&
            (get_tdscdma_interface()))
   {
     return TRUE;
   }
   else
   {
     return FALSE;
   }
#endif
}


boolean is_umts_mode_per_nas_stacks( sys_modem_as_id_e_type as_id )
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if(IS_NOT_VALID_STACK_ID(as_id))
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Invalid STACK ID:%d", as_id+1,0,0);
     return FALSE;
   }
#ifdef FEATURE_SEGMENT_LOADING
   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS && (get_wcdma_interface()))
   {
     return TRUE;
   }
   else if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS &&  (get_tdscdma_interface()))
   {
      return TRUE;
   }   
#else
   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)
   {
      return TRUE;
   }
   else if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS)
   {
     return TRUE;
   }
#endif
   return FALSE;
#else
  (void)as_id;
   return is_umts_mode();
#endif

}

boolean is_cs_umts_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
#endif

  return is_umts_mode_per_nas_stacks(stack_id_nas);
}

boolean is_tdscdma_mode(void)
{
 #ifdef FEATURE_SEGMENT_LOADING
   if ((mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS) && (get_tdscdma_interface()))
   {
     return TRUE;
   }
   else
   {
      return FALSE;
   }
#else
   return (mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS);
#endif

}

boolean is_tdscdma_mode_per_nas_stacks( sys_modem_as_id_e_type as_id )
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if(IS_NOT_VALID_STACK_ID(as_id))
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Invalid STACK ID:%d", as_id+1,0,0);
     return FALSE;
   }

#ifdef FEATURE_SEGMENT_LOADING
   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS && (get_tdscdma_interface()))
   {
     return TRUE;
   }
#else
   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS)
   {
      return TRUE;
   }
#endif
   return FALSE;
#else
  (void)as_id;
   return is_tdscdma_mode();
#endif
}

boolean is_cs_tdscdma_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
#endif

  return is_tdscdma_mode_per_nas_stacks(stack_id_nas);
}

boolean is_wcdma_mode(void)
{
#ifdef FEATURE_SEGMENT_LOADING
   if ((mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS) && (get_wcdma_interface()))
   {
     return TRUE;
   }
   else
   {
      return FALSE;
   }
#else
   return (mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS);
#endif
}




#ifdef FEATURE_NAS_GW


/*===========================================================================

FUNCTION get_preferred_mode

DESCRIPTION
  This function tells if the UE can operate in UMTS op mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if UMTS op mode, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

sys_radio_access_tech_e_type get_preferred_mode(void)
{
  return mm_ue_pref_rat;
}


/*===========================================================================

FUNCTION IS_UMTS_MODE

DESCRIPTION
  This function tells if the UE can operate in UMTS op mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if UMTS op mode, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


boolean is_wcdma_mode_per_nas_stacks( sys_modem_as_id_e_type as_id )
{

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if(IS_NOT_VALID_STACK_ID(as_id))
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Invalid STACK ID:%d", as_id+1,0,0);
     return FALSE;
   }


#ifdef FEATURE_SEGMENT_LOADING
   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS && (get_wcdma_interface()))
   {
     return TRUE;
   }
#else
   if (mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)
   {
      return TRUE;
   }
#endif
   return FALSE;
#else
  (void)as_id;
  return is_wcdma_mode();
#endif

}

boolean is_cs_wcdma_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
#endif

  return is_wcdma_mode_per_nas_stacks(stack_id_nas);

}


boolean is_ps_tdscdma_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
#endif

  return is_tdscdma_mode_per_nas_stacks(stack_id_nas);
}
boolean is_ps_umts_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
#endif

  return is_umts_mode_per_nas_stacks(stack_id_nas);
}
boolean is_ps_gsm_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
#endif

  return is_gsm_mode_per_nas_stacks(stack_id_nas);
}

boolean is_ps_wcdma_mode( sys_modem_as_id_e_type sub_id )
{
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB,"Invalid SUB ID: %d", sub_id+1,0,0);
    return FALSE;
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
#endif
#elif defined FEATURE_SGLTE
  stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
#endif

  return is_wcdma_mode_per_nas_stacks(stack_id_nas);
}


/*===========================================================================

FUNCTION IS_WCDMA_MODE

DESCRIPTION
  This function tells if the UE can operate in WCDMA operating mode.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if WCDMA op mode, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION IS_TDSCDMA_MODE

DESCRIPTION
  This function tells if the UE can operate in TD-SCDMA operating mode.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if TD-SCDMA op mode, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION Is_WtoGCCO_inProgress

DESCRIPTION
  This function tells if W to G CCO is in progress

DEPENDENCIES
  None

RETURN VALUE
  TRUE if W to G CCO is in progress, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean is_WtoGCCO_in_progress
(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  sys_modem_as_id_e_type as_id
#else
  void
#endif
)
{
#ifdef FEATURE_INTERRAT_PCCO_GTOW
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if(IS_NOT_VALID_STACK_ID(as_id))
   {
     MSG_FATAL_DS(MM_SUB, "=MM= Invalid AS ID:%d", as_id+1,0,0);
   }

   return (boolean) (GtoWCCO_state_sim[as_id] == CCO_IN_PROGRESS);
#else
   return (boolean) (GtoWCCO_state == CCO_IN_PROGRESS);
#endif
#else
   return FALSE;
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
}

#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
/*===========================================================================

FUNCTION get_preferred_band_class

DESCRIPTION
  This function returns the band class preference especified by the user

DEPENDENCIES
  None

RETURN VALUE
  Preferred band

SIDE EFFECTS
  None

===========================================================================*/

sys_band_mask_type get_preferred_band_class(void)
{
  return mm_ue_band_class_pref;
}

/*===========================================================================

FUNCTION get_active_band

DESCRIPTION
  This function returns the active band

DEPENDENCIES
  None

RETURN VALUE
  Preferred band

SIDE EFFECTS
  None

===========================================================================*/

sys_band_mask_e_type get_active_band(void)
{
  return mm_active_band;
}
#endif /* (FEATURE_GSM_BAND_PREF ||FEATURE_WCDMA_MULTIBAND) */


#ifdef FEATURE_SGLTE

void mm_cpoy_individual_csdata(mm_as_id_e_type dest,mm_as_id_e_type src)
{
  equiv_plmn_list_s_type            equiv_plmn_list_temp;
  mm_system_information_sim[dest].T3212_timer_value    
                            =  mm_system_information_sim[src].T3212_timer_value;
  mm_system_information_sim[dest].ATT  = mm_system_information_sim[src].ATT;
  mm_system_information_sim[dest].reestablishment_allowed = 
                     mm_system_information_sim[src].reestablishment_allowed;
  mm_system_information_sim[dest].mscr = mm_system_information_sim[src].mscr;
  mm_system_information_sim[dest].sgsnr     = mm_system_information_sim[src].sgsnr;
  mm_system_information_sim[dest].cell_access     = mm_system_information_sim[src].cell_access;
  mm_system_information_sim[dest].dsac_present = mm_system_information_sim[src].dsac_present;
  mm_system_information_sim[dest].cs_cell_access = mm_system_information_sim[src].cs_cell_access;
  mm_system_information_sim[dest].ppac_present = mm_system_information_sim[src].ppac_present;
  mm_system_information_sim[dest].cs_reg_allowed = mm_system_information_sim[src].cs_reg_allowed;
  mm_system_information_sim[dest].page_rsp_allowed = mm_system_information_sim[src].page_rsp_allowed;           

  mm_last_reg_dsac_information_sim[dest].cell_access = mm_last_reg_dsac_information_sim[src].cell_access;
  mm_last_reg_dsac_information_sim[dest].dsac_present = mm_last_reg_dsac_information_sim[src].dsac_present;
  mm_last_reg_dsac_information_sim[dest].cs_cell_access = mm_last_reg_dsac_information_sim[src].cs_cell_access;

  memscpy(&mm_serving_plmn_sim[dest].info.plmn,sizeof(sys_plmn_id_s_type),
                              &mm_serving_plmn_sim[src].info.plmn,sizeof(sys_plmn_id_s_type)) ;
  memscpy(&mm_serving_plmn_sim[dest].info.registered_plmn,sizeof(sys_plmn_id_s_type),
                              &mm_serving_plmn_sim[src].info.registered_plmn,sizeof(sys_plmn_id_s_type)) ;
  memscpy(&mm_serving_plmn_sim[dest].info.active_band,sizeof(sys_active_band_u_type),
                              &mm_serving_plmn_sim[src].info.active_band,sizeof(sys_active_band_u_type)) ;
  memscpy(&mm_serving_plmn_sim[dest].info.lac,sizeof(sys_lac_type),
                              &mm_serving_plmn_sim[src].info.lac,sizeof(sys_lac_type)) ;
                              
  mm_serving_plmn_sim[dest].info.plmn_service_capability   = mm_serving_plmn_sim[src].info.plmn_service_capability;
  mm_serving_plmn_sim[dest].info.plmn_forbidden    =  mm_serving_plmn_sim[src].info.plmn_forbidden;
  mm_serving_plmn_sim[dest].info.roaming_ind  =  mm_serving_plmn_sim[src].info.roaming_ind ;
  mm_serving_plmn_sim[dest].info.service_status    =  mm_serving_plmn_sim[src].info.service_status  ;
  mm_serving_plmn_sim[dest].info.cs_service_status    =  mm_serving_plmn_sim[src].info.cs_service_status   ;
  mm_serving_plmn_sim[dest].info.no_service_cause    =  mm_serving_plmn_sim[src].info.no_service_cause   ;
  mm_serving_plmn_sim[dest].info.suitable_search_time    =  mm_serving_plmn_sim[src].info.suitable_search_time   ;
  mm_serving_plmn_sim[dest].info.acq_status    =  mm_serving_plmn_sim[src].info.acq_status   ;
  mm_serving_plmn_sim[dest].info.plmn_reg_type    =  mm_serving_plmn_sim[src].info.plmn_reg_type   ;
  mm_serving_plmn_sim[dest].info.emc_attached    =  mm_serving_plmn_sim[src].info.emc_attached   ;
  mm_serving_plmn_sim[dest].info.suitable_search_time    =  mm_serving_plmn_sim[src].info.suitable_search_time   ; 
  memscpy(&mm_serving_plmn_sim[dest].lac,sizeof(location_area_code_T),
                              &mm_serving_plmn_sim[src].lac,sizeof(location_area_code_T)) ;
  memscpy(&mm_serving_plmn_sim[dest].info.active_band,sizeof(sys_active_band_u_type),
                              &mm_serving_plmn_sim[src].info.active_band,sizeof(sys_active_band_u_type)) ;                            
  mm_serving_plmn_sim[dest].mm_service_state = mm_serving_plmn_sim[src].mm_service_state;
  mm_serving_plmn_sim[dest].network_op_mode = mm_serving_plmn_sim[src].network_op_mode;
  mm_serving_plmn_sim[dest].update_equiv_plmn_list = mm_serving_plmn_sim[src].update_equiv_plmn_list;
#ifdef FEATURE_GSM_DTM
  mm_serving_plmn_sim[dest].dtm_supported = mm_serving_plmn_sim[src].dtm_supported;
#endif /* FEATURE_GSM_DTM */
  
  mm_serving_plmn_sim[dest].t3122_running = mm_serving_plmn_sim[src].t3122_running;
  /*  Swapping of equivalent list */
  memscpy (&equiv_plmn_list_temp,sizeof(equiv_plmn_list_s_type), &mm_serving_plmn_sim[src].equiv_plmn_list,
                    sizeof(equiv_plmn_list_s_type));
  memscpy (&mm_serving_plmn_sim[src].equiv_plmn_list,sizeof(equiv_plmn_list_s_type), &mm_serving_plmn_sim[dest].equiv_plmn_list,
                    sizeof(equiv_plmn_list_s_type));
  memscpy (&mm_serving_plmn_sim[dest].equiv_plmn_list,sizeof(equiv_plmn_list_s_type), &equiv_plmn_list_temp ,
                    sizeof(equiv_plmn_list_s_type));
/* swapping end here */  

memscpy(&mmcoord_mmr_reg_cnf_sim[dest].service_state.plmn,sizeof(sys_plmn_id_s_type),
                              &mmcoord_mmr_reg_cnf_sim[src].service_state.plmn,sizeof(sys_plmn_id_s_type)) ;
  memscpy(&mmcoord_mmr_reg_cnf_sim[dest].service_state.registered_plmn,sizeof(sys_plmn_id_s_type),
                              &mmcoord_mmr_reg_cnf_sim[src].service_state.registered_plmn,sizeof(sys_plmn_id_s_type)) ;
  memscpy(&mmcoord_mmr_reg_cnf_sim[dest].service_state.active_band,sizeof(sys_active_band_u_type),
                              &mmcoord_mmr_reg_cnf_sim[src].service_state.active_band,sizeof(sys_active_band_u_type)) ;
  memscpy(&mmcoord_mmr_reg_cnf_sim[dest].service_state.lac,sizeof(sys_lac_type),
                              &mmcoord_mmr_reg_cnf_sim[src].service_state.lac,sizeof(sys_lac_type)) ;
                              
  mmcoord_mmr_reg_cnf_sim[dest].service_state.sim_state = mmcoord_mmr_reg_cnf_sim[src].service_state.sim_state; 
  mmcoord_mmr_reg_cnf_sim[dest].service_state.plmn_service_capability   = mmcoord_mmr_reg_cnf_sim[src].service_state.plmn_service_capability;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.plmn_forbidden    =  mmcoord_mmr_reg_cnf_sim[src].service_state.plmn_forbidden;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.roaming_ind  =  mmcoord_mmr_reg_cnf_sim[src].service_state.roaming_ind ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.service_status    =  mmcoord_mmr_reg_cnf_sim[src].service_state.service_status  ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.active_rat    =  mmcoord_mmr_reg_cnf_sim[src].service_state.active_rat  ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.cs_service_status    =  mmcoord_mmr_reg_cnf_sim[src].service_state.cs_service_status   ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.no_service_cause    =  mmcoord_mmr_reg_cnf_sim[src].service_state.no_service_cause   ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.suitable_search_time    =  mmcoord_mmr_reg_cnf_sim[src].service_state.suitable_search_time   ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.acq_status    =  mmcoord_mmr_reg_cnf_sim[src].service_state.acq_status   ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.plmn_reg_type    =  mmcoord_mmr_reg_cnf_sim[src].service_state.plmn_reg_type   ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.emc_attached    =  mmcoord_mmr_reg_cnf_sim[src].service_state.emc_attached   ;
  mmcoord_mmr_reg_cnf_sim[dest].service_state.suitable_search_time    =  mmcoord_mmr_reg_cnf_sim[src].service_state.suitable_search_time   ; 
  mmcoord_mmr_reg_cnf_sim[dest].lai_forbidden = mmcoord_mmr_reg_cnf_sim[src].lai_forbidden;
  memscpy(&mmcoord_mmr_reg_cnf_sim[dest].available_plmn_list,sizeof(sys_detailed_plmn_list_s_type),
                              &mmcoord_mmr_reg_cnf_sim[src].available_plmn_list,sizeof(sys_detailed_plmn_list_s_type)) ;
  memscpy(&mmcoord_mmr_reg_cnf_sim[dest].service_state.active_band,sizeof(sys_active_band_u_type),
                              &mmcoord_mmr_reg_cnf_sim[src].service_state.active_band,sizeof(sys_active_band_u_type)) ;
  mmcoord_mmr_reg_cnf_sim[dest].update_equivalent_plmn_list = mmcoord_mmr_reg_cnf_sim[src].update_equivalent_plmn_list;
  mmcoord_mmr_reg_cnf_sim[dest].ue_oos = mmcoord_mmr_reg_cnf_sim[src].ue_oos;
  mmcoord_mmr_reg_cnf_sim[dest].only_for_reg = mmcoord_mmr_reg_cnf_sim[src].only_for_reg;
  mmcoord_mmr_reg_cnf_sim[dest].search_done = mmcoord_mmr_reg_cnf_sim[dest].search_done;
    /*  Swapping of equivalent list */
  memscpy (&equiv_plmn_list_temp,sizeof(equiv_plmn_list_s_type),&mmcoord_mmr_reg_cnf_sim[src].equivalent_plmn_list,
                    sizeof(equiv_plmn_list_s_type));
  memscpy (&mmcoord_mmr_reg_cnf_sim[src].equivalent_plmn_list,sizeof(equiv_plmn_list_s_type), &mmcoord_mmr_reg_cnf_sim[dest].equivalent_plmn_list,
                    sizeof(equiv_plmn_list_s_type));
  memscpy (&mmcoord_mmr_reg_cnf_sim[dest].equivalent_plmn_list,sizeof(equiv_plmn_list_s_type),&equiv_plmn_list_temp ,
                    sizeof(equiv_plmn_list_s_type));
/* swapping end here */ 
}
#if 0 
void mm_spliting_individually(void)
{
mm_system_information_T   mm_system_information;
mm_last_reg_dsac_information_T mm_last_reg_dsac_information;
mmr_reg_cnf_s_type   mmcoord_mmr_reg_cnf;
serving_plmn_info_T       mm_serving_plmn;
}
#endif
void mm_invalidate_cs_stack_database(void)
{
   mm_system_information_sim[mm_cs_stack_id].T3212_timer_value = 0;

   mm_system_information_sim[mm_cs_stack_id].cell_access = MM_AS_ACCESS_ALL_CALLS; 
   mm_last_reg_dsac_information_sim[mm_cs_stack_id].cell_access = MM_AS_ACCESS_ALL_CALLS; 
   mm_cnm_rel_req_received_sim[mm_cs_stack_id]          = FALSE;
   mm_held_cnm_service_req_sim[mm_cs_stack_id].present  = FALSE;
   mm_reg_waiting_for_reg_cnf_sim[mm_cs_stack_id]       = FALSE; 
   mm_ready_to_send_reg_cnf_sim[mm_cs_stack_id]         = FALSE; 
   mm_needs_to_send_mmr_service_ind_sim[mm_cs_stack_id] = FALSE; 
   mm_waiting_for_service_cnf_sim[mm_cs_stack_id]       = FALSE; 
   mmcoord_reg_req_pending_sim[mm_cs_stack_id]          = FALSE; 
   mm_waiting_for_deact_cnf_sim[mm_cs_stack_id]         = FALSE;
   mm_waiting_for_stop_mode_cnf_sim[mm_cs_stack_id]     = FALSE;
   mm_state_sim[mm_cs_stack_id] = MM_NULL;
   mm_idle_substate_sim[mm_cs_stack_id] = MM_NULL_SUBSTATE;
   mm_previous_idle_substate = NULL;
#if defined(FEATURE_WRLF_SYSTEM_SEL) && defined(FEATURE_NAS_GW)
   mm_received_suitable_search_end_ind = FALSE;
#endif
  mm_serving_lai_sim[mm_cs_stack_id][0] = 0xFF;
  mm_serving_lai_sim[mm_cs_stack_id][1] = 0xFF;
  mm_serving_lai_sim[mm_cs_stack_id][2] = 0xFF;
  mm_serving_lai_sim[mm_cs_stack_id][3] = 0xFF;
  mm_serving_lai_sim[mm_cs_stack_id][4] = 0xFE;
  mm_lu_attempt_counter_sim[mm_cs_stack_id] = 0; 
  mm_initiate_lu_upon_entry_into_new_cell_sim[mm_cs_stack_id] = FALSE;
  mm_follow_on_request_sim[mm_cs_stack_id]                  = FALSE;  
  mm_follow_on_proceed_sim[mm_cs_stack_id]                    = FALSE;  
  mm_T3213_timed_out_sim[mm_cs_stack_id]                      = FALSE;
  mm_idle_transition_timer_sim[mm_cs_stack_id]                = NULL; 
  mm_pending_lu_request_sim[mm_cs_stack_id]                   = NO_LU; 
  mm_pending_no_service_to_reg_sim[mm_cs_stack_id]            = FALSE;
  mm_lu_start_reason_sim[mm_cs_stack_id]                      = NO_LU;
    /* Initialize the CNM service establish cause */
  mm_cnm_srv_est_cause_sim[mm_cs_stack_id] = MM_RR_NO_CNM_SERVICE; 
  mm_connection_type_sim[mm_cs_stack_id] = NO_MM_CONNECTION;
  mm_serving_plmn_sim[mm_cs_stack_id].info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
  mm_serving_plmn_sim[mm_cs_stack_id].mm_service_state = SERVICE_NOT_AVAILABLE;
  mm_serving_plmn_sim[mm_cs_stack_id].gmm_service_state = SERVICE_NOT_AVAILABLE;
  mm_is_cs_connected_due_to_srvcc_sim[mm_cs_stack_id]         = FALSE;

}
void mm_reset_variables(mm_as_id_e_type src)
{
/* RESETTING of the variables */
  mm_initiate_lu_upon_entry_into_new_cell_sim[src] = FALSE;
  rrc_connection_status_sim[src] = FALSE;
  cs_session_status_sim[src] = MMRRC_IDLE;
  mm_held_cnm_service_req_sim[src].present = FALSE;
  mm_last_paging_cause_sim[src] = 0xFF;
  mm_integrity_protected_sim [src] = FALSE;
  mm_store_integrity_protected_sim[src] = FALSE;
  mm_is_gmm_moved_dereg_due_to_csreg_sim[src] = FALSE;   
  mm_service_ind_blocked_due_to_cs_reg_sim[src] = FALSE;
  mm_service_ind_blocked_due_to_irat_cs_reg_sim[src] = FALSE;
  mm_connection_activated_sim[src] = FALSE;
  mm_manual_mode_user_selection_procedure_sim[src] = FALSE;
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
  mm_lcs_active_sim[src] = FALSE;
#endif
  mm_connection_type_sim[src] = NO_MM_CONNECTION;
  mm_cnm_srv_est_cause_sim[src] = MM_RR_NO_CNM_SERVICE;
  mm_possible_imsi_attach_sim[src] = FALSE;
  mm_cnm_is_active_sim[src] = FALSE ;
  mm_sms_is_active_sim[src] = FALSE;
  mm_follow_on_request_sim[src] = FALSE;
  mm_follow_on_proceed_sim[src] = FALSE;
  mm_pending_lu_request_sim[src] =  FALSE;
  mm_pending_no_service_to_reg_sim[src] = FALSE;
  mm_lu_start_reason_sim[src] = NO_LU;
  mm_cnm_rel_req_received_sim[src] = FALSE;
  mm_state_sim[src] = MM_IDLE ;
  mm_idle_substate_sim[src] = MM_NORMAL_SERVICE;
  mm_plmn_search_in_progress_sim[src] = FALSE;
  mm_system_information_sim[src].T3212_timer_value = 0;
  mm_is_per_ms_t3212_present_sim[src]  = FALSE;
  mm_is_cs_connected_due_to_srvcc_sim[src]   = FALSE;
#if defined(FEATURE_NAS_REL10) && defined(FEATURE_LTE)
  mm_is_lu_to_be_initiated_t3346_ltogw = FALSE;
#endif

}
void mm_copy_cs_database(mm_as_id_e_type dest,mm_as_id_e_type src)
{
  uint8 i=0;
  uint64 remaining_timer_value = 0;
  mm_as_id_e_type as_id_local;
  mm_log_mm_reg_variables();
/* copy individual imp date first */
  mm_cpoy_individual_csdata(dest,src);
/* copy auth database as static */
#ifndef FEATURE_NAS_CS_DISABLED
  mm_copy_auth_cs_database(dest,src);
#endif
/* memcpy relate varaibles start here */
  memscpy((void *)&mm_location_information_sim[dest],sizeof(location_information_T),
               (void *)&mm_location_information_sim[src],sizeof(location_information_T));
  memscpy((void *)&mm_held_cnm_service_req_sim[dest],sizeof(held_service_request_T),
              (void *)&mm_held_cnm_service_req_sim[src],sizeof(held_service_request_T));
  memscpy((void *)&mm_lu_reject_info_sim[dest],sizeof(lu_reject_information_T),
              (void *)&mm_lu_reject_info_sim[src],sizeof(lu_reject_information_T));
  memscpy((void *)mm_serving_lai_sim[dest],LAI_SIZE,(void *)mm_serving_lai_sim[src],LAI_SIZE);  
  memscpy((void *)mm_previous_serving_lai_sim[dest],LAI_SIZE, 
                                     (void *)mm_previous_serving_lai_sim[src],LAI_SIZE);
  memscpy((void *)&mm_ef_kc_sim[dest],sizeof(ef_kc_T), 
                           (void *)&mm_ef_kc_sim[src],sizeof(ef_kc_T));
  memscpy((void *)&mm_ef_kc_sim[dest],sizeof(ef_kc_T), 
                           (void *)&mm_ef_kc_sim[src],sizeof(ef_kc_T));  
 
  memscpy((void *)&auth_gsm_cipherkey_sim[dest],sizeof(auth_gsm_cipherkey_type), 
                     (void *)&auth_gsm_cipherkey_sim[src],sizeof(auth_gsm_cipherkey_type));
  memscpy((void *)mm_sim_access_class_sim[dest],MM_SIM_EFACC_SIZE, 
                              (void *)mm_sim_access_class_sim[src],MM_SIM_EFACC_SIZE);
   /*LU reject cause for each failure attempt */
  memscpy((void *)mm_lu_reject_cause_sim[dest],MAX_LU_ATTEMPT_SIZE, 
                              (void *)mm_lu_reject_cause_sim[src],MAX_LU_ATTEMPT_SIZE);
#ifdef FEATURE_ENHANCED_NW_SELECTION
  memscpy((void *)&mm_foreground_search_lai_timer_list_sim[dest],sizeof(mm_lai_timer_info_list_info_T), 
                    (void *)&mm_foreground_search_lai_timer_list_sim[src],sizeof(mm_lai_timer_info_list_info_T));
 #endif
#ifdef FEATURE_NAS_REL10
  memscpy((void *)&mm_3gpp_t3246_plmn_id_sim[dest],sizeof(sys_plmn_id_s_type), 
                    (void *)&mm_3gpp_t3246_plmn_id_sim[src],sizeof(sys_plmn_id_s_type));
#endif
  /* This variable contains the RAND for which authentication is performing right now */
  
  memscpy((void *)&forbidden_list_sim[dest],2*sizeof (forbidden_list_T), 
                              (void*)&forbidden_list_sim[src],2*sizeof (forbidden_list_T));
  memscpy((void *)&lai_reject_list_sim[dest],sizeof(lai_reject_list_T), 
                   (void *)&lai_reject_list_sim[src],sizeof(lai_reject_list_T));

  /* memcpy variables end here */
/*###########################################################################*/
 /* All variables related to assigned  start here*/
  mm_state_sim[dest] = mm_state_sim[src];
  mm_idle_substate_sim[dest]  = mm_idle_substate_sim[src];
  mm_cnm_rel_req_received_sim[dest] =  mm_cnm_rel_req_received_sim[src];
  mm_present_all_plmns_sim[dest]  = mm_present_all_plmns_sim[src];

  mm_lu_attempt_counter_sim[dest] = mm_lu_attempt_counter_sim[src];
  mm_pending_lu_request_sim[dest] = mm_pending_lu_request_sim[src];
  mm_pending_no_service_to_reg_sim[dest] = mm_pending_no_service_to_reg_sim[src];
  mm_lu_start_reason_sim[dest] = mm_lu_start_reason_sim[src];
  mm_cipher_key_sequence_number_sim[dest] = mm_cipher_key_sequence_number_sim[src];
  mm_follow_on_request_sim[dest] = mm_follow_on_request_sim[src];
  mm_follow_on_proceed_sim[dest] = mm_follow_on_proceed_sim[src];
  mm_reject_cause_sim[dest] = mm_reject_cause_sim[src];
  mm_cnm_is_active_sim[dest] = mm_cnm_is_active_sim[src];
  mm_sms_is_active_sim[dest] = mm_sms_is_active_sim[src];
  mm_T3213_timed_out_sim[dest] = mm_T3213_timed_out_sim[src];
  mm_idle_transition_timer_sim[dest] = mm_idle_transition_timer_sim[src];


  mm_possible_imsi_attach_sim[dest]  = mm_possible_imsi_attach_sim[src];
  mm_ef_kc_valid_sim[dest]  = mm_ef_kc_valid_sim[src];
  mm_integrity_protected_sim[dest]  = mm_integrity_protected_sim[src];
  mm_dual_mode_sim[dest]  = mm_dual_mode_sim[src];
  mm_band_pref_sim[dest]  = mm_band_pref_sim[src];
  mm_cnm_srv_est_cause_sim[dest]  = mm_cnm_srv_est_cause_sim[src];
  mm_connection_type_sim[dest]  = mm_connection_type_sim[src];
  mm_ue_band_class_pref_sim[dest]  = mm_ue_band_class_pref_sim[src];
  mm_active_band_sim[dest]  = mm_active_band_sim[src];
  mm_band_indicator_sim[dest]  = mm_band_indicator_sim[src];
  mm_lu_pended_nmo_unknown_sim[dest]  = mm_lu_pended_nmo_unknown_sim[src];

  mm_previous_idle_substate_sim[dest]  = mm_previous_idle_substate_sim[src];
  mm_prev_cell_access_sim[dest]  = mm_prev_cell_access_sim[src];
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
  mm_lcs_active_sim[dest]  = mm_lcs_active_sim[src];
#endif
#ifdef FEATURE_CCBS
  mm_supports_NIMO_CM_connection_sim[dest] = mm_supports_NIMO_CM_connection_sim[src];
#endif
  mm_initiate_lu_upon_entry_into_new_cell_sim[dest] = mm_initiate_lu_upon_entry_into_new_cell_sim[src];
  mm_serving_cell_id_sim[dest] = mm_serving_cell_id_sim[src];
  mm_serving_cell_valid_sim[dest] = mm_serving_cell_valid_sim[src];
  mm_lu_failed_cell_id_sim[dest] = mm_lu_failed_cell_id_sim[src];
#ifdef FEATURE_DSAC
  mm_cs_access_barred_to_unbarred_sim[dest] = mm_cs_access_barred_to_unbarred_sim[src];
  mm_last_procedure_to_update_registration_sim[dest] = mm_last_procedure_to_update_registration_sim[src];
#endif 
  mm_manual_mode_user_selection_procedure_sim[dest] = mm_manual_mode_user_selection_procedure_sim[src];
#ifdef FEATURE_ENHANCED_NW_SELECTION
  mm_force_lu_sim[dest] = mm_force_lu_sim[src];
  mm_foreground_search_timer_value_sim[dest] = mm_foreground_search_timer_value_sim[src];

#endif
  mm_mmr_cell_service_ind_pending_sim[dest] = mm_mmr_cell_service_ind_pending_sim[src];
  mm_imsi_is_native_sim[dest] = mm_imsi_is_native_sim[src];
/* Flag to indicate whether GSM access is allowed or not */
  auth_gsm_access_sim[dest] = auth_gsm_access_sim[src];
/* GSM security context for USIM */
  auth_usim_gsm_security_context_sim[dest] = auth_usim_gsm_security_context_sim[src];
/* Flag to remember last network selection mode send to RR/RRC */
  last_requested_network_selection_mode_sim[dest] = last_requested_network_selection_mode_sim[src];
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
  mm_pending_lu_type_sim[dest] = mm_pending_lu_type_sim[src];
#endif
  mm_service_ind_blocked_due_to_cs_reg_sim[dest] = mm_service_ind_blocked_due_to_cs_reg_sim[src];
  mm_service_ind_blocked_due_to_irat_cs_reg_sim[dest] = mm_service_ind_blocked_due_to_irat_cs_reg_sim[src];
  mm_connection_activated_sim[dest] = mm_connection_activated_sim[src];
  mm_is_foreground_search_to_be_initiated_sim[dest] =  mm_is_foreground_search_to_be_initiated_sim[src];
  mm_is_gmm_moved_dereg_due_to_csreg_sim[dest] = mm_is_gmm_moved_dereg_due_to_csreg_sim[src];
#ifdef FEATURE_NAS_REL10

  mm_store_integrity_protected_sim[dest] = mm_store_integrity_protected_sim[src];
  mm_t3246_backoff_timer_value_sim[dest] = mm_t3246_backoff_timer_value_sim[src];
  mm_stored_per_ms_t3212_value_sim[dest] = mm_stored_per_ms_t3212_value_sim[src];
  mm_is_per_ms_t3212_present_sim[dest] = mm_is_per_ms_t3212_present_sim[src];
#endif
  mm_last_paging_cause_sim[dest] = mm_last_paging_cause_sim[src];
  mm_cell_changed_to_cs_capable_sim[dest] = mm_cell_changed_to_cs_capable_sim[src];
  lu_cause_sim[dest] = lu_cause_sim[src];
/* Declare mm_plmn_search_in_progress variable*/
  mm_plmn_search_in_progress_sim[dest] = mm_plmn_search_in_progress_sim[src];
  pending_mm_msg_after_release_sim[dest] = pending_mm_msg_after_release_sim[src];
  mm_cs_domain_possible_reest_sim[dest] = mm_cs_domain_possible_reest_sim[src];
  pending_mm_msg_valid_sim[dest] = pending_mm_msg_valid_sim[src];
  send_sequence_number_mm_cc_ss_sim[dest] = send_sequence_number_mm_cc_ss_sim[src];
/* newly Added copied variables */
  rrc_connection_status_sim [dest] = rrc_connection_status_sim[src];
  cs_session_status_sim[dest] = cs_session_status_sim[src];

  as_id_local = mm_as_id;
  /* resetting of the variables end here */
  /* Move CS timers */
  for (i=MIN_MM_TIMER; i < MAX_MM_TIMER+1; i++)
  {
     mm_as_id = src;
     if ((mm_timer_status_sim[src][i] != TIMER_STOPPED) && mm_is_cs_timer(i))
     {
       /* Find out how much timeout is left */
       remaining_timer_value =(uint64) gs_enquire_timer_ext ( 
                                                           get_mm_timer_queue(),
                                                           i
                                                         ); 
       MSG_HIGH_DS_3(MM_SUB, "=MM= Moving CS timer from SRC to DEST : remaining time left before expiry %ul, src as id = %d, Dest as id = %d",remaining_timer_value,src,dest );
       mm_stop_timer(i);
       mm_as_id = dest;
       mm_start_timer(i,remaining_timer_value);
     }
  }
  mm_as_id = as_id_local;
  mm_coord_camped_cell_id_sim[dest] = mm_coord_camped_cell_id_sim[src];
  mm_is_cs_connected_due_to_srvcc_sim[dest] = mm_is_cs_connected_due_to_srvcc_sim[src];
  /* Assigned end here */
}
#endif 

#if defined(FEATURE_SGLTE) || defined(FEATURE_SVLTE_DUAL_SIM)
rex_timer_cnt_type mm_get_ps_timer_value(void)
{
 return (sglte_nas_mm_config.ps_timer_value *5000);
}
#endif

#endif
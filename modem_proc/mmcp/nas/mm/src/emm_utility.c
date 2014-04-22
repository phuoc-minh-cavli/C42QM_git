
/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.
	
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/emm_utility.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/07/10   MNK     1XCSFB introduction 
09/24/10   MNK     Handle ESR & TAU collision
06/21/10   zren    Updated EMM to include correct MME routing info in conn est 
                   req for TAU after IRAT   
05/26/10   zren    Updated to send LIMITED SERVICE status in REG CNF for 
                   LIMITED SERVICE REQ when get service available service ind 
                   from RRC
05/24/10   zren    Updated NONCE UE handling upon receiving TAU ACCEPT 
                   Updated TX failure causes handling for TAU COMPLETE message
05/19/10   zren    Added support for GSTK EMM REGISTERED EVT and NW REJ EVT
05/18/10   zren    Added NONCE UE support in TAU request
05/17/10   zren    Updated EMM to only process EMM timers
05/11/10   RI      Mainlined spec changes upto Dec 09.
05/10/10   zren    Added code to init local var in GSTK interface functions
05/07/10   zren    Added temp FPLMN feature
                   Fixed and consolidated equivalent PLMN list handling
                   Added code to handle found plmn list in RRC SERVICE IND
04/22/10   zren    Updated to update service domain upon detach completion
04/22/10   zren    Updated to use global structure for NAS outgoing message
04/09/10   zren    Added combined procedure feature
04/02/10   zren    Added failure handling for RRC connection release causes of
                   CRE_FAILURE and OOS_DURING_CRE 
02/24/10   zren    Added support for network selection modes of LIMITED MANUAL
                   ACQUISITION and LIMITED ROUTINE ACQUISITION
02/16/10   MNK     function to set tin type
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
02/05/10   zren    Updated processing of connection abort in waiting for RRC 
                   confirmation state
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
01/21/10   zren    Added EMM/ESM STATUS feature
01/07/10   zren    Updated release client and pended message processing
11/16/09   MNK     Preserve the plmn service state upon i-RAT & Cleanup emm_ctrl_data as necessary
11/04/09   MNK     Preserve the security context upon i-RAT & Cleanup emm_ctrl_data as necessary
10/05/09   hnam    Added support for forbidden tracking are list
08/14/09   zren    Modified procedure connection release handling
07/21/09   zren    Added support to set high priority flag in RRC_CONN_EST_REQ
07/15/09   zren    Modified message list handling functions to use message 
                   pointer as parameter
06/23/09   zren    Added support of processing EPLMN list, forbidden PLMN list,
                   forbidden TAI list for roaming and forbidden TAI list for
                   regional provision of service 
06/13/09   RI      Modified emm_send_rrc_data_request() to include EMM ctrl data.
06/02/09   zren    Added SEMAPHORE to protect access_barring_flag
05/29/09   zren    Added support for TAU feature
05/19/09   hnam    Added support for transmission failure scenario - when 
                   connection is released by RRC during any UL procedure failures
04/20/09   zren    Fixed a bug in emm_find_pended_message() which was causing
                   EMM fall into an infinite loop.
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
04/15/09   zren    Completed MO detach feature
04/06/09   MNK     Added code to generate based on new transaction id format
04/02/09   zren    Added support for new transaction ID format 
03/26/09   RI      Added support for GS Timers in LTE instead of REX signals
03/24/09   zren    Added support for service request and paging feature
02/19/09   MNK     Changed prototype_gs_send_message to gs_send_message so that code works for the unit 
                   test,As integrationa and full stack integration as well
02/06/09   vdr     Modified failure delivery callbacks in order to delete message 
                   from the cache mem
01/30/09   vdr     Enabled Access Barring timer processing
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "emm.h"
#include "rex.h"
#include "sys.h"
#include "policyman.h"
#include "lte_rrc_ext_msg.h"
#include "emm_database.h"
#include "emm_mem.h"
#include "emm_timer_sigs.h"
#include "gs_v.h"
#include "emm_connection_type.h"
#include "emm_connection_handler.h"
#include "lte_nas_common_v.h"
#include "lte_nas_emm_message_description.h"
#include "emm_rrc_if.h"
#include "emm_utility.h"
#include "sys_stru_v.h"
#include "mm_v.h"
#include "emm_update_lib.h"
#include "emm_sim_handler.h"
#include "err.h"
#include "emm_esm_handler.h"
#include "emm_rrc_handler.h"
#include "msgr_nas.h"
#include "fs_public.h"
#include "gmm_msg.h"
#include "sys_eplmn_list_v.h"
#include "ghdi_exp_int.h"
#include "extern_mmtask.h"
#include "emm_mem.h"
#include "nv_items.h"
#include "emm_service_request_lib.h"
#include "emm_csfb_handler.h"
#include "lte_rrc_ext_api.h"
#include "mm_coord.h"
#include "msgr_rex.h"
#ifdef FEATURE_DUAL_SIM
#include "lte_rrc_ext_msg.h"
#include "esm_emm_msgr_enum_msg.h"
#include "emm_esm_if_msg.h"
#include "esm_emm_msg_if.h"
#include "reg_sim.h"
#include "lte_tlb.h"
#endif
#include "ULogFront.h"
#include "ds3gpp_api.h"
#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#endif
#include "ttl_map.h"
#include "mcfg_fs.h"
#ifdef FEATURE_TDSCDMA
#include "tds_rrc_api.h"
#endif
#include "stringl.h"
#include "dog_hb_rex.h"
#include "emm_ext_msg.h"
#ifdef FEATURE_GSTK
#ifdef TEST_FRAMEWORK
 #error code not present
#endif
#endif
#ifdef FEATURE_DUAL_SIM
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read_per_subs(a,b,mm_nv_context_id)
#else
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read(a,b)
#endif

extern ULogHandle mm_get_ulog_handle();

boolean emm_disable_geran_cap = FALSE;

#define MAX_NAS_INCOMING_MSG 2

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern ue_rev_level_indicator_type get_revision_level_indicator_per_nas_stacks
(
  sys_modem_as_id_e_type as_id 
);
#define get_revision_level_indicator() get_revision_level_indicator_per_nas_stacks((sys_modem_as_id_e_type)mm_as_id)
#else
extern ue_rev_level_indicator_type get_revision_level_indicator( void );
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_get(a,b) sys_eplmn_per_nas_stacks_list_get(a,b,(sys_modem_as_id_e_type)mm_as_id)
#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a,(sys_modem_as_id_e_type)mm_sub_id,(sys_modem_as_id_e_type)mm_as_id)
#define sys_eplmn_list_rplmn() (sys_eplmn_per_nas_stacks_list_rplmn((sys_modem_as_id_e_type)mm_as_id))

extern sys_modem_stack_id_e_type mm_set_cm_stack_id(mm_as_id_e_type as_id);
#endif
void emm_delete_manual_ftai
(
  lte_nas_tai_lst1_type *tai_tobe_deleted,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

extern emm_mo_detach_type nas_emm_power_off_detach_type;
extern boolean nas_emm_tau_cs_rej;
extern boolean full_srv_req_in_emerg;
extern lte_nas_emm_eps_update_val_type nas_emm_tau_cs_rej_tau_type;
#ifdef FEATURE_DUAL_SIM
extern void  esm_tag_as_id
(
  msgr_hdr_struct_type* msg_ptr
);
#endif

#ifdef FEATURE_LTE
lte_3gpp_release_ver_e emm_3gpp_spec_ver = LTE_3GPP_REL10_MAX;
#endif

#ifdef FEATURE_1XSRLTE
#define SRLTE_ESR_TIMER 500
#endif

#if defined FEATURE_DUAL_SIM
extern uint8  mm_emm_max_attempt_counter_dos_mitigation_sim[MAX_NAS_STACKS];
#define mm_emm_max_attempt_counter_dos_mitigation mm_emm_max_attempt_counter_dos_mitigation_sim[mm_as_id]

extern uint8   mm_emm_usim_gprs_counter_sim[MAX_NAS_STACKS];
#define mm_emm_usim_gprs_counter mm_emm_usim_gprs_counter_sim[mm_as_id]

extern uint8   mm_emm_usim_non_gprs_counter_sim[MAX_NAS_STACKS];
#define mm_emm_usim_non_gprs_counter mm_emm_usim_non_gprs_counter_sim[mm_as_id]

#else
extern uint8  mm_emm_max_attempt_counter_dos_mitigation;
extern uint8   mm_emm_usim_gprs_counter;
extern uint8   mm_emm_usim_non_gprs_counter;
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===========================================================================

FUNCTION    : EMM_PRINT_EMM_CAUSE

DESCRIPTION:  This function will print the causes recieved by EMM module for any procedure
 
DEPENDENCIES
  None

RETURN VALUE
none

SIDE EFFECTS
  None

===========================================================================*/

void emm_print_emm_cause(lte_nas_emm_cause_type   emm_cause)
{
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Cause - Value = %u ",emm_cause);
}


/*===========================================================================

FUNCTION    : EMM_VALIDATE_ATTACH_PLMN

DESCRIPTION:  This function waits for UIM response to check if camped PLMN is
              fine to proceed with ATTACH or to be in LIMITED_SERVICE until card 
              refreshes for another ATTACH
 
DEPENDENCIES
  None

RETURN VALUE
 TRUE: Proceed with ATTACH
 FALSE: Current PLMN is invalid, ABORT ATTACH procedure & continue to be in LIMITED_SERVICE

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_validate_attach_plmn
(
  emm_ctrl_data_type *emm_ctrl_data_ptr,
  sys_plmn_id_s_type *plmn_to_attach
)
{
  boolean emm_is_attach_allowed = TRUE;
  
  

#ifdef FEATURE_GSTK
  MSG_HIGH_DS_1(MM_SUB,"=EMM= processing 'emm_validate_attach_plmn', attaching_ctrl_timer = %u",
                emm_ctrl_data_ptr->attach_ctrl_timer);

  if(emm_ctrl_data_ptr->attach_ctrl_timer > 0)
  {
    gstk_attach_ctrl_result_enum_type gstk_attach_ctrl_result;

    MSG_HIGH_DS_0(MM_SUB,"=EMM= sending attach_ctrl_req to GSTK, to validate this ATTACH");
 
    if(gstk_send_attach_ctrl_req(rex_self(),
                                 UIM_EMM_CMD_Q_SIG,
                                 plmn_to_attach->identity) == GSTK_SUCCESS)
    {
      /* Wait on signal mask*/
      (void)mm_wait( UIM_EMM_CMD_Q_SIG );
      (void) rex_clr_sigs( rex_self(), UIM_EMM_CMD_Q_SIG);
      gstk_attach_ctrl_result = gstk_get_attach_ctrl_res();
      MSG_HIGH_DS_1(MM_SUB, "=EMM= received UIM response, gstk_attach_ctrl_result = %d",
                    gstk_attach_ctrl_result);
      switch(gstk_attach_ctrl_result)
      {
        case GSTK_ATTACH_CTRL_INVALID:
        case GSTK_ATTACH_CTRL_TIMER_EXP:        
        case GSTK_ATTACH_CTRL_ALLOWED:        
          emm_is_attach_allowed = TRUE;
          break;        
        case GSTK_ATTACH_CTRL_NOT_ALLOWED:        
          emm_is_attach_allowed = FALSE;
          break;
        default:
            MSG_FATAL_DS(MM_SUB,"=EMM= Illegal response from UIM",0,0,0);
          break;
      }
    }
    else
    {
      MSG_ERROR_DS_0(MM_SUB, "=EMM= GSTK ATTACH CTRL REQ failed - Proceed with ATTACH");
      emm_is_attach_allowed = TRUE;     
    }
  }
  else
  { 
    /*This feature is not supported, just return TRUE to proceed with default behavior*/    
    emm_is_attach_allowed = TRUE;
  }
#else
  /*This feature is not supported, just return TRUE to proceed with default behavior*/    
  MSH_HIGH("=EMM= FEATURE_GSTK is not enabled",0,0,0); 
  emm_is_attach_allowed = TRUE;
#endif
  return emm_is_attach_allowed;
}

/*===========================================================================

FUNCTION    : EMM_RESET_POWEROFF_DETACH_PARAMS

DESCRIPTION:  This function will reset all the detach related params during poweroff
  
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_reset_poweroff_detach_params
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{

  

  emm_update_srv_domain_mo_detach(emm_ctrl_data_ptr);

  /* Reset ongoing detach flag */
  emm_ctrl_data_ptr->ongoing_detach_procedure = FALSE;

  /* Reset detach attempt counter */
  emm_ctrl_data_ptr->emm_detach_attempt_counter = 0;

  /* Reset detach reason cause */
  emm_ctrl_data_ptr->detach_reason = INVALID_DETACH;
}
/*===========================================================================

FUNCTION    : 

DESCRIPTION
  
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

emm_security_context_info *emm_get_context_by_state(emm_ctrl_data_type *emm_ctrl_data_ptr, 
                                                   lte_nas_context_state context_state)
{
 int index;

 

 for( index = 0; index < EMM_MAX_CONTEXTS; index++ )
 {
  if(( context_state == emm_ctrl_data_ptr->context_info[index].context_state ) &&
     ( emm_ctrl_data_ptr->context_info[index].in_use ) )
  {
    return (emm_security_context_info *)(&emm_ctrl_data_ptr->context_info[index]);
  }
 }

 return NULL;

}

/*===========================================================================
FUNCTION EMM_GET_PLMN_STATE_INFO

DESCRIPTION
  This function populates all the parameter of PLMN_STATE_INFO in emm_ctrl_data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_get_plmn_state_info
(
  sys_srv_status_e_type  srv_status,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{

  ASSERT(emm_ctrl_data_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band);

  
  emm_ctrl_data_ptr->plmn_service_state->plmn = 
    emm_convert_rrc_plmn_id_to_nas_plmn_id(
      emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  emm_ctrl_data_ptr->plmn_service_state->plmn_service_capability = 
                                            SYS_SRV_CAPABILITY_CS_PS_SERVICE;

  emm_ctrl_data_ptr->plmn_service_state->plmn_forbidden = FALSE ;

  emm_ctrl_data_ptr->plmn_service_state->service_status = srv_status;
#ifndef FEATURE_CIOT
  emm_ctrl_data_ptr->plmn_service_state->active_rat = SYS_RAT_LTE_RADIO_ACCESS;
#endif
  SYS_LTE_BAND_MASK_ADD_BAND(&emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band,
                             emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_band); 

  emm_ctrl_data_ptr->plmn_service_state->tac = 
                   emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;

}/* emm_get_plmn_state_info */

/*===========================================================================
FUNCTION    EMM_LOCAL_DETACH

DESCRIPTION
  This function performs the NAS local detach 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_local_detach
(
   emm_ctrl_data_type *emm_ctrl_data_ptr
)
{ 
  emm_state_type  emm_state = EMM_INVALID_STATE ;

  emm_state    = EMM_GET_STATE() ;

  switch(emm_state)
  { 
    case EMM_DEREGISTERED:
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE); 
      /*Do not send LIMITED_SERVICE event to GSTK, as this is in the middle of OOS/RLF procedure*/
      break;
    case EMM_INVALID_STATE:
    case EMM_NULL:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Unexpected state %d to locally detach", emm_state);
      break;
    default:
      emm_build_and_send_detach_ind(EMM_DETACHED);
      MSG_HIGH_DS_0(MM_SUB, "=EMM= now processing LTE_RRC_DEACTIVATE_CNF received earlier");
      emm_process_rrc_deactivate_cnf(&emm_ctrl_data_ptr->last_rrc_deactivate_cnf,emm_ctrl_data_ptr);
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE); 
      /*Do not send LIMITED_SERVICE event to GSTK, as this is in the middle of OOS/RLF procedure*/
      emm_reset_ctrl_data(emm_ctrl_data_ptr, TRUE);
      MSG_HIGH_DS_0(MM_SUB, "=EMM= now locally deactivated");
      break;
  }
}/* emm_local_detach */

/*===========================================================================
FUNCTION EMM_UPDATE_GW_SEC_CAPABILITIES

DESCRIPTION
  Update the LTE OTA message and / OR EMM internal data structure (emm_ctrl_data) 
  with the corresponding Security capabilties
  1. For UMTS, EMM_CTRL_DATA & OTA message structure are updated
  2. For GPRS, only EMM_CTRL_DATA is updated (existing code will update the OTA structure)

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_NAS_GW
void emm_update_gw_sec_capabilities
(
  emm_ctrl_data_type              *emm_ctrl_data_ptr,
  lte_nas_emm_ue_capability_type  *ue_capability
)
{
  gprs_hw_classmark_T         classmark;
  ue_rev_level_indicator_type ms_revision_level;
  boolean                     gsm_spt               = FALSE;
  boolean                     umts_spt              = FALSE;
  boolean                     tds_spt               = FALSE;
  /* Reset the classmark structure */
  memset(&classmark,0,sizeof(gprs_hw_classmark_T));

  gsm_spt  = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_GSM_RADIO_ACCESS);
  umts_spt = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_UMTS_RADIO_ACCESS);
  tds_spt  = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_TDS_RADIO_ACCESS);
  
  MSG_HIGH_DS_3(MM_SUB,"=EMM= UMTS_SPT = %d, GSM_SPT = %d TDS_SPT %d",
                umts_spt,gsm_spt,tds_spt);

#ifdef FEATURE_TDSCDMA
  /* populate TDS UE capabilities if plmn id is supported by TD carrier */
  if((tds_spt == TRUE) && 
     (reg_mode_is_plmn_td_compliant(emm_ctrl_data_ptr->plmn_service_state->plmn) == TRUE))
  {
    /*=====================================================================================
                                    If TDS_GCF supported
    =====================================================================================*/
    if(emm_ctrl_data_ptr->emm_tds_nv_fake_sec_flag)
    {
      /*UMTS ENC ALG*/
      ue_capability->ue_sec_capability.umts_encrypt_alg_uea2_supported = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea2_supported = TRUE;
      ue_capability->ue_sec_capability.umts_enc_exists = TRUE;    
      emm_ctrl_data_ptr->ue_security_capability.umts_enc_exists = TRUE;
      /*UMTS int - uia1*/
      ue_capability->ue_sec_capability.umts_integrity_alg_uia2_supported = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_integrity_alg_uia2_supported = TRUE;
      ue_capability->ue_sec_capability.umts_int_exists = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_int_exists = TRUE;
    }
    else
    {
      /*UMTS ENC ALG*/
      ue_capability->ue_sec_capability.umts_encrypt_alg_uea2_supported = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea2_supported = FALSE;
      ue_capability->ue_sec_capability.umts_enc_exists = FALSE;    
      emm_ctrl_data_ptr->ue_security_capability.umts_enc_exists = FALSE;
      /*UMTS int - uia1*/
      ue_capability->ue_sec_capability.umts_integrity_alg_uia2_supported = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_integrity_alg_uia2_supported = FALSE;
      ue_capability->ue_sec_capability.umts_int_exists = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_int_exists = FALSE;
    }
    MSG_HIGH_DS_2(MM_SUB, "=EMM= emm_tds_nv_fake_sec_flag == %d, Setting umts_int_exists flags to (1/0) = (%d)",
                            emm_ctrl_data_ptr->emm_tds_nv_fake_sec_flag,ue_capability->ue_sec_capability.umts_int_exists);
    /*=====================================================================================
                        Update INT_Algos based on TDS_CIPHERING NV flag
    =====================================================================================*/
    if(emm_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag == TRUE)
    {
      /*UMTS enc - uea0, uea1*/
      ue_capability->ue_sec_capability.umts_encrypt_alg_uea0_supported = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea0_supported = TRUE;
      ue_capability->ue_sec_capability.umts_encrypt_alg_uea1_supported = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea1_supported = TRUE;
      ue_capability->ue_sec_capability.umts_enc_exists = TRUE;    
      emm_ctrl_data_ptr->ue_security_capability.umts_enc_exists = TRUE;
    }
    else
    {
      /*UMTS enc - uea0, uea1*/
      ue_capability->ue_sec_capability.umts_encrypt_alg_uea0_supported = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea0_supported = FALSE;
      ue_capability->ue_sec_capability.umts_encrypt_alg_uea1_supported = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea1_supported = FALSE;
      ue_capability->ue_sec_capability.umts_enc_exists = FALSE;    
      emm_ctrl_data_ptr->ue_security_capability.umts_enc_exists = FALSE;
    }
    MSG_HIGH_DS_2(MM_SUB, "=EMM= emm_tds_nv_cipher_enable_flag == %d, Setting umts_enc_exists flags to (1/0) = (%d)",
                            emm_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag,ue_capability->ue_sec_capability.umts_enc_exists);

    /*=====================================================================================
                        Update INT_Algos based on TDS_INTEGRITY NV flag
    =====================================================================================*/
    if(emm_ctrl_data_ptr->emm_tds_nv_integ_enable_flag == TRUE)
    {
      /*UMTS int - uia1*/
      ue_capability->ue_sec_capability.umts_integrity_alg_uia1_supported = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_integrity_alg_uia1_supported = TRUE;
      ue_capability->ue_sec_capability.umts_int_exists = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_int_exists = TRUE;
    }
    else
    {
      /*UMTS int - uia1*/
      ue_capability->ue_sec_capability.umts_integrity_alg_uia1_supported = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_integrity_alg_uia1_supported = FALSE;
      ue_capability->ue_sec_capability.umts_int_exists = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_int_exists = FALSE;
    }
    MSG_HIGH_DS_2(MM_SUB, "=EMM= emm_tds_nv_integ_enable_flag == %d, Setting umts_int_exists flags to (1/0) = (%d)",
                           emm_ctrl_data_ptr->emm_tds_nv_integ_enable_flag,ue_capability->ue_sec_capability.umts_int_exists );

  }
  else
#endif
  /*Update UMTS security capabilities (in EMM_CTRL_DATA & OTA_Message structure)*/
  if(umts_spt == TRUE)  
  {

    /*=====================================================================================
                                    If UMTS_GCF supported
    =====================================================================================*/
    if(emm_ctrl_data_ptr->emm_nv_fake_sec_flag)
    {
      /*UMTS ENC ALG*/
      ue_capability->ue_sec_capability.umts_encrypt_alg_uea2_supported = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea2_supported = TRUE;
      ue_capability->ue_sec_capability.umts_enc_exists = TRUE;    
      emm_ctrl_data_ptr->ue_security_capability.umts_enc_exists = TRUE;
      /*UMTS int - uia1*/
      ue_capability->ue_sec_capability.umts_integrity_alg_uia2_supported = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_integrity_alg_uia2_supported = TRUE;
      ue_capability->ue_sec_capability.umts_int_exists = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_int_exists = TRUE;
    }
    else
    {
      /*UMTS ENC ALG*/
      ue_capability->ue_sec_capability.umts_encrypt_alg_uea2_supported = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea2_supported = FALSE;
      ue_capability->ue_sec_capability.umts_enc_exists = FALSE;    
      emm_ctrl_data_ptr->ue_security_capability.umts_enc_exists = FALSE;
      /*UMTS int - uia1*/
      ue_capability->ue_sec_capability.umts_integrity_alg_uia2_supported = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_integrity_alg_uia2_supported = FALSE;
      ue_capability->ue_sec_capability.umts_int_exists = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_int_exists = FALSE;
    }
    MSG_HIGH_DS_2(MM_SUB, "=EMM= emm_nv_fake_sec_flag == %d, Setting umts_int_exists flags to (1/0) =(%d) ",
                           emm_ctrl_data_ptr->emm_nv_fake_sec_flag,ue_capability->ue_sec_capability.umts_int_exists);

    /*=====================================================================================
                        Update INT_Algos based on UMTS_CIPHERING NV flag
    =====================================================================================*/
    if(emm_ctrl_data_ptr->emm_umts_nv_cipher_enable_flag == TRUE)
    {
      /*UMTS enc - uea0, uea1*/
      ue_capability->ue_sec_capability.umts_encrypt_alg_uea0_supported = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea0_supported = TRUE;
      ue_capability->ue_sec_capability.umts_encrypt_alg_uea1_supported = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea1_supported = TRUE;
      ue_capability->ue_sec_capability.umts_enc_exists = TRUE;    
      emm_ctrl_data_ptr->ue_security_capability.umts_enc_exists = TRUE;
    }
    else if(emm_ctrl_data_ptr->emm_umts_nv_cipher_enable_flag == FALSE)
    {
      /*UMTS enc - uea0, uea1*/
      ue_capability->ue_sec_capability.umts_encrypt_alg_uea0_supported = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea0_supported = FALSE;
      ue_capability->ue_sec_capability.umts_encrypt_alg_uea1_supported = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea1_supported = FALSE;
      ue_capability->ue_sec_capability.umts_enc_exists = FALSE;    
      emm_ctrl_data_ptr->ue_security_capability.umts_enc_exists = FALSE;
    }
    MSG_HIGH_DS_2(MM_SUB, "=EMM= emm_umts_nv_cipher_enable_flag == %d, Setting umts_enc_exists flags to (1/0) = (%d)",
                            emm_ctrl_data_ptr->emm_umts_nv_cipher_enable_flag,ue_capability->ue_sec_capability.umts_enc_exists);

    /*=====================================================================================
                        Update INT_Algos based on UMTS_INTEGRITY NV flag
    =====================================================================================*/
    if(emm_ctrl_data_ptr->emm_umts_nv_integ_enable_flag == TRUE)
    {
      /*UMTS int - uia1*/
      ue_capability->ue_sec_capability.umts_integrity_alg_uia1_supported = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_integrity_alg_uia1_supported = TRUE;
      ue_capability->ue_sec_capability.umts_int_exists = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_int_exists = TRUE;
    }
    else if(emm_ctrl_data_ptr->emm_umts_nv_integ_enable_flag == FALSE)
    {
      /*UMTS int - uia1*/
      ue_capability->ue_sec_capability.umts_integrity_alg_uia1_supported = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_integrity_alg_uia1_supported = FALSE;
      ue_capability->ue_sec_capability.umts_int_exists = FALSE;
      emm_ctrl_data_ptr->ue_security_capability.umts_int_exists = FALSE;
    }
    MSG_HIGH_DS_2(MM_SUB, "=EMM= emm_umts_nv_integ_enable_flag == %d, Setting umts_int_exists flags to (1/0) = (%d)",
                           emm_ctrl_data_ptr->emm_umts_nv_integ_enable_flag ,ue_capability->ue_sec_capability.umts_int_exists);

  }
  else
  {
    /*UMTS ENC ALG*/
    ue_capability->ue_sec_capability.umts_encrypt_alg_uea2_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea2_supported = FALSE;
    /*UMTS int - uia1 */
    ue_capability->ue_sec_capability.umts_integrity_alg_uia2_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.umts_integrity_alg_uia2_supported = FALSE;
        
    /*UMTS enc - uea0, uea1 */
    ue_capability->ue_sec_capability.umts_encrypt_alg_uea0_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea0_supported = FALSE;
    ue_capability->ue_sec_capability.umts_encrypt_alg_uea1_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.umts_encrypt_alg_uea1_supported = FALSE;
    ue_capability->ue_sec_capability.umts_enc_exists = FALSE;    
    emm_ctrl_data_ptr->ue_security_capability.umts_enc_exists = FALSE;

    /*UMTS int - uia1*/
    ue_capability->ue_sec_capability.umts_integrity_alg_uia1_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.umts_integrity_alg_uia1_supported = FALSE;
    ue_capability->ue_sec_capability.umts_int_exists = FALSE;
    MSG_HIGH_DS_2(MM_SUB, "=EMM= umts_spt %d and tds_spt %d, Setting umts_int_exists and umts_enc_exists flags to False", umts_spt, tds_spt);
    emm_ctrl_data_ptr->ue_security_capability.umts_int_exists = FALSE;
  }

  if((emm_disable_geran_cap == FALSE) &&
     (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                      SYS_RAT_UMTS_RADIO_ACCESS)|| 
      mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                      SYS_RAT_GSM_RADIO_ACCESS) ||
      mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                      SYS_RAT_TDS_RADIO_ACCESS)))
  {
    emm_ctrl_data_ptr->ue_security_capability.gprs_enc_exists = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_1_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_2_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_3_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_4_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_5_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_6_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_7_supported = FALSE;

    /*===================================================================================
                  Update GPRS security Capabilities in the EMM_CTRL_DATA
    ===================================================================================*/
#ifdef FEATURE_DUAL_SIM
    nas_read_gprs_hw_classmark_per_subs(&classmark,(sys_modem_as_id_e_type)mm_sub_id);
#else
    nas_read_gprs_hw_classmark(&classmark);
#endif
    
      ms_revision_level = get_revision_level_indicator();

      if((classmark.GEA_1_supported)||(classmark.GEA_2_supported)||
         (classmark.GEA_3_supported)||(classmark.GEA_4_supported)||
         (classmark.GEA_5_supported)||(classmark.GEA_6_supported)||
         (classmark.GEA_7_supported))
      {
      emm_ctrl_data_ptr->ue_security_capability.gprs_enc_exists = TRUE;
      MSG_HIGH_DS_0(MM_SUB, "=EMM= gsm_supported, Setting umts_int_exists, and umts_enc_exists flags to True ");
      emm_ctrl_data_ptr->ue_security_capability.umts_int_exists = TRUE;
      emm_ctrl_data_ptr->ue_security_capability.umts_enc_exists = TRUE;
      ue_capability->ue_sec_capability.umts_enc_exists = TRUE;  
      ue_capability->ue_sec_capability.umts_int_exists = TRUE;
      }
      if(classmark.GEA_1_supported)
      {
      emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_1_supported = TRUE;
      }
    if (ms_revision_level == UE_REV_REL99)
    {
      if(classmark.GEA_2_supported)
      {
        emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_2_supported = TRUE;
      }
      if(classmark.GEA_3_supported)
      {
        emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_3_supported = TRUE;
      }
      if(classmark.GEA_4_supported)
      {
        emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_4_supported = TRUE;
      }
      if(classmark.GEA_5_supported)
      {
        emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_5_supported = TRUE;
      }
      if(classmark.GEA_6_supported)
      {
        emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_6_supported = TRUE;
      }
      if(classmark.GEA_7_supported)
      {
        emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_7_supported = TRUE;
      }  
    }
  }
  else /*If GW is not in the priority list, then reset these bits*/
  {
    emm_ctrl_data_ptr->ue_security_capability.gprs_enc_exists = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_1_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_2_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_3_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_4_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_5_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_6_supported = FALSE;
    emm_ctrl_data_ptr->ue_security_capability.gprs_encrypt_alg_7_supported = FALSE;
  }
}
#endif
emm_client_list_type *emm_find_client_record
(
  msgr_tech_module_type client,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  emm_client_list_type *current_list_entry = NULL ;

   

  if (emm_ctrl_data_ptr->client_list == (struct emm_agent_list_tag *)NULL)
  {
    return NULL ;
  }
  
  current_list_entry = emm_ctrl_data_ptr->client_list ;

  do
  {
   if (current_list_entry->client == client)
   {
     break ;
   }
   else
   {
     current_list_entry = current_list_entry->next_client ;
   }
  } while(current_list_entry != (emm_client_list_type *)NULL) ;

  return current_list_entry ;

} /* end of emm_find_client_record() */

emm_pended_msg_list_type *emm_create_new_message_buffer
(
  lte_nas_outgoing_msg_type   *emm_message_ptr,
  dword                        sequence_number,
  rrc_l2_callback_type         message_callback,
  rrc_connection_failure_type  tx_failure_callback,
  emm_ctrl_data_type          *emm_ctrl_data_ptr

)
{
  emm_pended_msg_list_type *new_pended_msg = NULL ;

   

  if((new_pended_msg = (emm_pended_msg_list_type *)emm_modem_mem_alloc(sizeof(emm_pended_msg_list_type),
                                                                        emm_ctrl_data_ptr)) == NULL)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Not enough memory to allocate new message buffer",0,0,0) ;
  }

  new_pended_msg->nas_out_message = *emm_message_ptr;

  new_pended_msg->sequence_number         = sequence_number ;

  new_pended_msg->message_sent_indicator  = FALSE ;

  new_pended_msg->msg_delivery_callback  = message_callback ;

  new_pended_msg->msg_tx_failure_callback = tx_failure_callback ;

  new_pended_msg->emm_ctrl_data_pen_ptr = emm_ctrl_data_ptr ;
#ifdef FEATURE_CIOT
  // Needs to be looked at only for a connection setup message
  new_pended_msg->send_on_resume = FALSE;
#endif
  return new_pended_msg ;

} /* end of emm_create_new_message_buffer() */

emm_pended_msg_list_type *emm_add_new_message_client
(
  lte_nas_outgoing_msg_type   *emm_message_ptr,
  dword                        sequence_number,
  msgr_tech_module_type        client,
  rrc_l2_callback_type         message_callback,
  rrc_connection_failure_type  tx_failure_callback,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
)
{
  emm_client_list_type     *new_client = NULL;
  emm_pended_msg_list_type *new_pended_msg = NULL ;
  boolean                   this_is_connection_startup_message = FALSE ;

   

  /* Only EMM message can be connection start up message */
  if((emm_ctrl_data_ptr->client_list == NULL) && 
     (emm_message_ptr->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES))
  {
    this_is_connection_startup_message = TRUE ;
  }

  if((new_client = (emm_client_list_type *)emm_modem_mem_alloc(sizeof(emm_client_list_type),
                                                              emm_ctrl_data_ptr)) == NULL)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Not enough memory to allocate new client",0,0,0) ;
  }

  new_client->client = client ;

  new_client->next_client = emm_ctrl_data_ptr->client_list ;

  if ((new_pended_msg = emm_create_new_message_buffer(emm_message_ptr,
                                                      sequence_number,
                                                      message_callback,
                                                      tx_failure_callback,
                                                      emm_ctrl_data_ptr)) == NULL)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Not enough memory to allocate new message buffer",0,0,0) ;
  }

  if (this_is_connection_startup_message == TRUE)
  {
#ifdef FEATURE_CIOT  
    new_pended_msg->send_on_resume = emm_ctrl_data_ptr->send_on_resume;
#endif	  
    new_client->connection_setup_message = new_pended_msg ;
  }
  else
  {
    new_client->connection_setup_message = (emm_pended_msg_list_type *)NULL ;
  }

  new_client->next_message = new_pended_msg ;

  emm_ctrl_data_ptr->client_list = new_client ;

  return new_pended_msg ;
  
} /* end of emm_add_new_message_client() */

emm_pended_msg_list_type *emm_add_new_message
(
  emm_client_list_type        *client_record,
  lte_nas_outgoing_msg_type   *emm_message_ptr,
  dword                        sequence_number,
  rrc_l2_callback_type         message_callback,
  rrc_connection_failure_type  tx_failure_callback,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
)
{
  emm_pended_msg_list_type *new_pended_msg = NULL ;
  emm_pended_msg_list_type *cur_ptr = NULL ;

  if ((new_pended_msg = emm_create_new_message_buffer(emm_message_ptr,
                                                      sequence_number,
                                                      message_callback, 
                                                      tx_failure_callback, 
                                                      emm_ctrl_data_ptr)) == NULL)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Not enough memory to allocate new message buffer",0,0,0) ;
  }

  cur_ptr = client_record->next_message;
  while(cur_ptr->next_message != NULL)
  {
    cur_ptr = cur_ptr->next_message;
  }
  
  cur_ptr->next_message = new_pended_msg;

  new_pended_msg->next_message = NULL;

  /* Check if need to set connection setup message */
  /* Note: Only EMM message can be connection start up message */
  if((client_record->connection_setup_message == NULL) &&
     (emm_message_ptr->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES))
  {
    client_record->connection_setup_message = new_pended_msg;
#ifdef FEATURE_CIOT	
	client_record->connection_setup_message->send_on_resume = emm_ctrl_data_ptr->send_on_resume;
#endif	
  }

  return new_pended_msg ;
} /* end of emm_add_new_message() */

/*===========================================================================

FUNCTION  EMM_CACHE_NEW_MESSAGE

DESCRIPTION
  This function caches a message in message list

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the added message

SIDE EFFECTS
  None

===========================================================================*/
emm_pended_msg_list_type  *emm_cache_new_message
(
  lte_nas_outgoing_msg_type   *emm_message_ptr,
  dword                        sequence_number,
  msgr_tech_module_type        client,
  rrc_l2_callback_type         message_callback,
  rrc_connection_failure_type  tx_failure_callback,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
)
{
  emm_client_list_type     *client_list = NULL;
  emm_pended_msg_list_type *cached_message ;

   

  client_list = emm_find_client_record(client, emm_ctrl_data_ptr);

  if (client_list == NULL)
  {
    cached_message = emm_add_new_message_client(emm_message_ptr, sequence_number,
                                                client, message_callback, 
                                                tx_failure_callback, 
                                                emm_ctrl_data_ptr) ;
  }
  else
  {
    cached_message = emm_add_new_message(client_list, emm_message_ptr, 
                                         sequence_number, message_callback, 
                                         tx_failure_callback, 
                                         emm_ctrl_data_ptr);
  }

  return cached_message ;
} /* end of emm_cache_new_message() */

void emm_send_all_pended_messages
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_client_list_type       *client = NULL ;
  emm_pended_msg_list_type   *message = NULL ;

   

  if (emm_ctrl_data_ptr->client_list != NULL)
  {
    client = emm_ctrl_data_ptr->client_list ;

    while (client != NULL)
    {
      if (client->next_message != NULL)
      {
        message = client->next_message ;

        while (message != NULL)
        {
          if (message->message_sent_indicator == FALSE)
          {
            emm_send_rrc_data_request(&message->nas_out_message, 
                                      message->sequence_number, 
                                      emm_ctrl_data_ptr) ;

            message->message_sent_indicator = TRUE ;
          }

          message = message->next_message ;
        }
      }

      client = client->next_client ;
    }
  }
} /* end of emm_send_all_pended_messages() */

/*===========================================================================
FUNCTION    EMM_TRANSLATE_REL_CAUSE

DESCRIPTION
  This function translates the connection release cause to EMM connection cause

DEPENDENCIES
  None

RETURN VALUE
  EMM connection cause

SIDE EFFECTS
  None
===========================================================================*/
lte_emm_connection_cause_type emm_translate_rel_cause
(
  lte_rrc_conn_rel_reason_e rrc_rel_reason
)
{
  lte_emm_connection_cause_type emm_connection_cause = TRANSMISSION_LTE_RRC_UL_DATA_CNF_SUCCESS ;

  switch (rrc_rel_reason)
  {
    case LTE_RRC_CONN_REL_NORMAL:
      emm_connection_cause = CONN_REL_LTE_RRC_CONN_REL_NORMAL ;
      break ;
    case LTE_RRC_CONN_REL_RLF:
      emm_connection_cause = CONN_REL_LTE_RRC_CONN_REL_RLF ;
      break ;
    case LTE_RRC_CONN_REL_CRE_FAILURE:
      emm_connection_cause = CONN_REL_LTE_RRC_CONN_REL_CRE_FAILURE;
      break;
    case LTE_RRC_CONN_REL_OOS_DURING_CRE:
      emm_connection_cause = CONN_REL_LTE_RRC_CONN_REL_OOS_DURING_CRE;
      break;
    case LTE_RRC_CONN_REL_ABORTED:
      emm_connection_cause = CONN_REL_LTE_RRC_CONN_REL_ABORTED ;
      break;
    case LTE_RRC_CONN_REL_SIB_READ_ERROR:
      emm_connection_cause = CONN_REL_LTE_RRC_CONN_REL_SIB_READ_ERROR;
      break ;
    case LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS:
      emm_connection_cause = CONN_REL_LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS;
      break ;
    case LTE_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_SGLTE_SRVCC_SUCCESS:
      emm_connection_cause = CONN_REL_LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS;
      break ;
	case LTE_RRC_CONN_REL_RLF_SEC_NOT_ACTIVE://DET
	  emm_connection_cause = CONN_REL_LTE_RRC_CONN_RLF_SEC_NOT_ACTIVE;
	  break;
    default :
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal RRC release cause",0,0,0) ;
      break ;
  }

  return emm_connection_cause ;
} /* end of emm_translate_rel_cause() */

/*===========================================================================
FUNCTION    EMM_TRANSLATE_CNF_CAUSE

DESCRIPTION
  This function translates a given RRC_CONN_EST_CNF_CAUSE to EMM connection 
  cause type

DEPENDENCIES
  None

RETURN VALUE
  The translated EMM connection cause

SIDE EFFECTS
  None
===========================================================================*/
lte_emm_connection_cause_type emm_translate_cnf_cause
(
  lte_rrc_conn_est_status_e rrc_conn_est_status
)
{
  lte_emm_connection_cause_type emm_connection_cause = TRANSMISSION_LTE_RRC_UL_DATA_CNF_SUCCESS ;

  switch (rrc_conn_est_status)
  {
    case LTE_RRC_CONN_EST_SUCCESS:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_SUCCESS ;
      break ;
    case LTE_RRC_CONN_EST_FAILURE:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE ;
      break ;
    case LTE_RRC_CONN_EST_FAILURE_ABORTED:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ABORTED ;
      break ;
    case LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED ;
      break ;
#ifdef FEATURE_NAS_EAB
    case LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB ;
      break ;
#endif
    case LTE_RRC_CONN_EST_FAILURE_CELL_RESEL:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CELL_RESEL ;
      break ;
    case LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE ;
      break ;
    case LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED ;
      break ;
    case LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE ;
      break ;
    case LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED ;
      break ;
    case LTE_RRC_CONN_EST_FAILURE_SI_FAILURE:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_SI_FAILURE ;
      break ;
    case LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT ;
      break ;
    case LTE_RRC_CONN_EST_FAILURE_TAI_CHANGE :
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TAI_CHANGED ;
      break ;
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE || defined FEATURE_CATM_LTE_EXT_GNSS_COEX || defined FEATURE_WWAN_GNSS_DYNAMIC
    case LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
      emm_connection_cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE;
      break;
#endif 

    default :
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal RRC connection confirmation status",0,0,0) ;
      break ;
  }

  return emm_connection_cause ;
} /* end of emm_translate_cnf_cause() */

/*===========================================================================
FUNCTION    EMM_PROCESS_UNSENT_MESSAGES

DESCRIPTION
  This function traverse the pended message list and processes pended messages
  if needed.
  - This function removes processed messages
  - This funtion removes client without any pended message left

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_unsent_messages
(
  lte_emm_connection_cause_type  cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr
)
{
  emm_client_list_type      **prev_client_ptr = NULL ;
  emm_client_list_type       *curr_client = NULL ;
  emm_pended_msg_list_type  **prev_msg_ptr = NULL ;
  emm_pended_msg_list_type   *curr_msg = NULL ;
  lte_nas_tai_lst1_type       tai;
  boolean                     msg_processed = FALSE;

   

  memset((void *)&tai, 0x00, sizeof(tai));

  if (emm_ctrl_data_ptr->client_list != NULL)
  {
    prev_client_ptr = &emm_ctrl_data_ptr->client_list;
    curr_client = emm_ctrl_data_ptr->client_list;

    while (curr_client != NULL)
    {
      if (curr_client->next_message != NULL)
      {
        prev_msg_ptr = &curr_client->next_message;
        curr_msg = curr_client->next_message;       

        while (curr_msg != NULL)
        {          
          msg_processed = FALSE;

          /* Process the message if connection release or est failure 
             - IS NOT due to connection abortion 
             or 
             - IS due to connection abortion and processing flag is set  
          */
          if((emm_ctrl_data_ptr->aborting_conn == FALSE) ||
             (curr_msg->process_upon_conn_abort == TRUE))
          {
            /* Call the tx failure call back function if there is one */
            if(curr_msg->msg_tx_failure_callback != NULL)
            {
              (void)curr_msg->msg_tx_failure_callback(cause,
                                                     curr_msg->sequence_number,
                                                     emm_failure_cause,
                                                     tai,
                                                     curr_msg,
                                                     emm_ctrl_data_ptr);
            }
            msg_processed = TRUE;
          }            

          if(msg_processed == TRUE)
          {
            /* Remove the processed message */
            *prev_msg_ptr = curr_msg->next_message;
            switch (curr_msg->nas_out_message.nas_hdr.msg_id)
            {
#ifdef FEATURE_CIOT
               /* delete the memory allocated for esm_data_transport message */
               case CP_SERVICE_REQUEST:
                 if(curr_msg->nas_out_message.emm_outgoing_msg.cp_service_req.esm_msg_container_exists == TRUE)
                 {
                   emm_modem_mem_free(curr_msg->nas_out_message.emm_outgoing_msg.cp_service_req.\
                                      esm_msg_container.esm_data_transport.user_data_container.\
                                      container_value_ptr,
                                      emm_ctrl_data_ptr);
                 }
                 break;
               case ESM_DATA_TRANSPORT:
                 if(curr_msg->nas_out_message.esm_outgoing_msg.esm_data_transport.\
                                    user_data_container.container_value_ptr != NULL)
                 {
                   emm_modem_mem_free(curr_msg->nas_out_message.esm_outgoing_msg.esm_data_transport.\
                                      user_data_container.container_value_ptr,
                                      emm_ctrl_data_ptr);
                 }
                 break;
#endif
            case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT:
              if(curr_msg->nas_out_message.esm_outgoing_msg.default_bearer_context_rej.\
                    ext_protocol_conf_opt.content_ptr != NULL)
              {
                emm_modem_mem_free(curr_msg->nas_out_message.esm_outgoing_msg.default_bearer_context_rej.\
                                ext_protocol_conf_opt.content_ptr,
                                emm_ctrl_data_ptr);
              }
              break;
            case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT:
              /* delete the memory allocated for default_bearer_context_accept message */
              if(curr_msg->nas_out_message.esm_outgoing_msg.default_bearer_context_accept.\
                   ext_protocol_conf_opt.content_ptr != NULL)
              {
                emm_modem_mem_free(curr_msg->nas_out_message.esm_outgoing_msg.\
                                   default_bearer_context_accept.\
                                   ext_protocol_conf_opt.content_ptr,
                                   emm_ctrl_data_ptr);
              }
              break;
            case ATTACH_COMPLETE:
              /* delete the memory allocated for default_bearer_context_accept message, which is piggyback in emm message*/
              if(curr_msg->nas_out_message.emm_outgoing_msg.attach_complete.esm_msg_container.\
                   default_bearer_context_accept.ext_protocol_conf_opt.content_ptr !=NULL)
              {
                emm_modem_mem_free(curr_msg->nas_out_message.emm_outgoing_msg.attach_complete.\
                                esm_msg_container.default_bearer_context_accept.\
                                ext_protocol_conf_opt.content_ptr,
                                emm_ctrl_data_ptr);
              }
              break;
            case DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT:
              /* delete the memory allocated for deactivate_eps_bearer_context_accept  */
              if((curr_msg->nas_out_message.esm_outgoing_msg.deactivate_eps_bearer_context_accept.\
                 ext_protocol_conf_opt.content_ptr != NULL)) 
              {
                emm_modem_mem_free(curr_msg->nas_out_message.esm_outgoing_msg.deactivate_eps_bearer_context_accept.\
                                   ext_protocol_conf_opt.content_ptr,
                                   emm_ctrl_data_ptr);
              }
              break;
            default:
              break;
           }
            emm_modem_mem_free(curr_msg, emm_ctrl_data_ptr);
            curr_msg = *prev_msg_ptr;            
          }
          else
          {
            /* Message is not processed. Find next message. */
            prev_msg_ptr = &curr_msg->next_message;
            curr_msg = curr_msg->next_message;
          }
        }
      }

      if(curr_client->next_message == NULL)
      {
        /* Remove client without any message left */
        *prev_client_ptr = curr_client->next_client;
        emm_modem_mem_free(curr_client, emm_ctrl_data_ptr);
        curr_client = *prev_client_ptr;
      }
      else
      {
        /* Client is not empty. No need to remove. Find next client. */
        prev_client_ptr = &curr_client->next_client;
        curr_client = curr_client->next_client;
      }
    }
  }
} /* end of emm_process_unsent_messages() */

emm_pended_msg_list_type *emm_find_pended_message
(
  dword               trans_id,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  emm_client_list_type *client = (emm_client_list_type *)NULL ;
  emm_pended_msg_list_type *message = (emm_pended_msg_list_type *)NULL ;

   

  if (emm_ctrl_data_ptr->client_list != (emm_client_list_type *)NULL)
  {
    client = emm_ctrl_data_ptr->client_list ;

    while (client != (emm_client_list_type *)NULL)
    {
      if (client->next_message != (emm_pended_msg_list_type *)NULL)
      {
        message = client->next_message ;

        while (message != (emm_pended_msg_list_type *)NULL)
        {
          if (message->sequence_number == trans_id)
          {
            return message ;
          }

          message = message->next_message ;
        }
      }
      else
      {
        break ;
      }

      client = client->next_client ;
    }
  }

  return (emm_pended_msg_list_type *)NULL ;

} /* end of emm_find_pended_message() */


emm_pended_msg_list_type *emm_find_pended_esr
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  emm_client_list_type *client = (emm_client_list_type *)NULL ;
  emm_pended_msg_list_type *message = (emm_pended_msg_list_type *)NULL ;

   

  if (emm_ctrl_data_ptr->client_list != (emm_client_list_type *)NULL)
  {
    client = emm_ctrl_data_ptr->client_list ;

    while (client != (emm_client_list_type *)NULL)
    {
      if (client->next_message != (emm_pended_msg_list_type *)NULL)
      {
        message = client->next_message ;

        while (message != (emm_pended_msg_list_type *)NULL)
        {
          if (message->nas_out_message.nas_hdr.msg_id == EXT_SERVICE_REQUEST)
          {
            return message ;
          }

          message = message->next_message ;
        }
      }
      else
      {
        break ;
      }

      client = client->next_client ;
    }
  }

  return (emm_pended_msg_list_type *)NULL ;

} /* end of emm_find_pended_message() */

void emm_delete_message_client
(
  emm_client_list_type *client_tobe_deleted, 
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  emm_client_list_type **prev_client_ptr = (emm_client_list_type **)NULL ;
  emm_client_list_type *client = (emm_client_list_type *)NULL ;

   

  if (emm_ctrl_data_ptr->client_list != (emm_client_list_type *)NULL)
  {
    prev_client_ptr = &emm_ctrl_data_ptr->client_list ;

    client = emm_ctrl_data_ptr->client_list ;

    while (client != (emm_client_list_type *)NULL)
    {
      if (client == client_tobe_deleted)
      {
        *prev_client_ptr = client->next_client ;

        emm_modem_mem_free(client_tobe_deleted,emm_ctrl_data_ptr) ;

        return ;
      }
      else
      {
        prev_client_ptr = &client->next_client ;

        client = client->next_client ;
      }
    }
  }

} /* end of emm_delete_message_client() */




/*===========================================================================

FUNCTION EMM_DELETE_PENDED_MESSAGE_WITH_ID

DESCRIPTION
  This function deletes the pended message of specified message type in the queue.
  This function is called on successful transmission of message or 
  when we are done with the message in failure cases.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_delete_pended_message_with_msg_id
(
  lte_nas_message_id  pended_msg_id,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  emm_client_list_type     *client = (emm_client_list_type *)NULL ;
  emm_pended_msg_list_type *message = (emm_pended_msg_list_type *)NULL ;
  

   

  if (emm_ctrl_data_ptr->client_list != (emm_client_list_type *)NULL)
  {
    client = emm_ctrl_data_ptr->client_list ;

    while (client != (emm_client_list_type *)NULL)
    {
      message = client->next_message ;

      while (message != (emm_pended_msg_list_type *)NULL)
      {
        if (message->nas_out_message.nas_hdr.msg_id  == pended_msg_id)
        {
           emm_delete_pended_message(message,emm_ctrl_data_ptr);
           return ;
        }
        else
        {
           message = message->next_message ;
        }
      }
      
      client = client->next_client ;
    }
  }
} /* end of emm_delete_pended_message_with_msg_id() */

/*===========================================================================

FUNCTION EMM_DELETE_PENDED_MESSAGE

DESCRIPTION
  This function deletes the pended message in the queue.
  This function is called on successful transmission of message or 
  when we are done with the message in failure cases.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_delete_pended_message
(
  emm_pended_msg_list_type *message_tobe_deleted, 
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{
  emm_client_list_type     *client = (emm_client_list_type *)NULL ;
  emm_pended_msg_list_type *message = (emm_pended_msg_list_type *)NULL ;
  emm_pended_msg_list_type **prev_message_ptr = (emm_pended_msg_list_type **)NULL ;

   

  if (emm_ctrl_data_ptr->client_list != (emm_client_list_type *)NULL)
  {
    client = emm_ctrl_data_ptr->client_list ;

    while (client != (emm_client_list_type *)NULL)
    {
      message = client->next_message ;

      prev_message_ptr = &client->next_message ;

      while (message != (emm_pended_msg_list_type *)NULL)
      {
        if (message == message_tobe_deleted)
        {
          *prev_message_ptr = message->next_message ;

          if (client->connection_setup_message  == message)
          {
            client->connection_setup_message = NULL ;
          }

          switch (message->nas_out_message.nas_hdr.msg_id)
          {

#if defined (FEATURE_LTE_REL9) 
#ifdef FEATURE_NAS_GPS_ENABLE
            case UL_GENERIC_NAS_TRANSPORT:
            /* delete the memory allocated for UL_GENERIC_NAS message */
             emm_modem_mem_free(message->nas_out_message.emm_outgoing_msg.\
                                ul_generic_nas_transport_msg.generic_msg_container.container_value_ptr,
                                 emm_ctrl_data_ptr);
             message->nas_out_message.emm_outgoing_msg.\
                                ul_generic_nas_transport_msg.generic_msg_container.container_value_ptr = NULL;

             if(message->nas_out_message.emm_outgoing_msg.\
                ul_generic_nas_transport_msg.additional_info_exists == TRUE)
             {
               emm_modem_mem_free(message->nas_out_message.emm_outgoing_msg.\
                                  ul_generic_nas_transport_msg.additional_info.additional_info_value_ptr,
                                  emm_ctrl_data_ptr);
               message->nas_out_message.emm_outgoing_msg.\
                                  ul_generic_nas_transport_msg.additional_info.additional_info_value_ptr = NULL;
             }
             break;
#endif
#endif
#ifdef FEATURE_CIOT
            case CP_SERVICE_REQUEST:
              /* delete the memory allocated for esm_data_transport message */
              if(message->nas_out_message.emm_outgoing_msg.cp_service_req.esm_msg_container_exists == TRUE)
              {
                emm_modem_mem_free(message->nas_out_message.emm_outgoing_msg.\
                                   cp_service_req.esm_msg_container.\
                                   esm_data_transport.user_data_container.container_value_ptr,
                                   emm_ctrl_data_ptr);
                message->nas_out_message.emm_outgoing_msg.cp_service_req.esm_msg_container.\
                               esm_data_transport.user_data_container.container_value_ptr = NULL;
              }
              break;
            case ESM_DATA_TRANSPORT:
              /* delete the memory allocated for esm_data_transport message */
              if(message->nas_out_message.esm_outgoing_msg.esm_data_transport.\
                               user_data_container.container_value_ptr != NULL)
              { 
                emm_modem_mem_free(message->nas_out_message.esm_outgoing_msg.esm_data_transport.\
                                  user_data_container.container_value_ptr,
                                  emm_ctrl_data_ptr);
                message->nas_out_message.esm_outgoing_msg.esm_data_transport.\
                               user_data_container.container_value_ptr = NULL;
              }
              break;
#endif
            case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT:
              /* delete the memory allocated for default_bearer_context_accep message */
              if(message->nas_out_message.esm_outgoing_msg.default_bearer_context_accept.\
                ext_protocol_conf_opt.content_ptr != NULL) 
              {
                emm_modem_mem_free(message->nas_out_message.esm_outgoing_msg.default_bearer_context_accept.\
                                   ext_protocol_conf_opt.content_ptr,
                                   emm_ctrl_data_ptr);
                message->nas_out_message.esm_outgoing_msg.default_bearer_context_accept.\
                                   ext_protocol_conf_opt.content_ptr = NULL;
              }
              break;
            case ATTACH_COMPLETE:
              /* delete the memory allocated for default_bearer_context_accep message and piggyback in emm msg*/
              if(message->nas_out_message.emm_outgoing_msg.attach_complete.\
                 esm_msg_container.default_bearer_context_accept.ext_protocol_conf_opt.content_ptr !=NULL) 
              {
                emm_modem_mem_free(message->nas_out_message.emm_outgoing_msg.attach_complete.\
                                   esm_msg_container.default_bearer_context_accept.\
                                   ext_protocol_conf_opt.content_ptr,
                                   emm_ctrl_data_ptr);
                message->nas_out_message.emm_outgoing_msg.attach_complete.\
                               esm_msg_container.default_bearer_context_accept.\
                               ext_protocol_conf_opt.content_ptr = NULL;
              }
              break;
            case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT:
              /* delete the memory allocated for default_bearer_context_rej message */
              if(message->nas_out_message.esm_outgoing_msg.default_bearer_context_rej.\
                   ext_protocol_conf_opt.content_ptr != NULL)
              {
                emm_modem_mem_free(message->nas_out_message.esm_outgoing_msg.default_bearer_context_rej.\
                                   ext_protocol_conf_opt.content_ptr,
                                   emm_ctrl_data_ptr);
                message->nas_out_message.esm_outgoing_msg.default_bearer_context_rej.\
                               ext_protocol_conf_opt.content_ptr = NULL;
              }
              break;
            case DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT:
              /* delete the memory allocated for deactivate_eps_bearer_context_accept  */
              if(message->nas_out_message.esm_outgoing_msg.deactivate_eps_bearer_context_accept.\
                 ext_protocol_conf_opt.content_ptr != NULL) 
              {
                emm_modem_mem_free(message->nas_out_message.esm_outgoing_msg.deactivate_eps_bearer_context_accept.\
                                   ext_protocol_conf_opt.content_ptr,
                                   emm_ctrl_data_ptr);
                message->nas_out_message.esm_outgoing_msg.deactivate_eps_bearer_context_accept.\
                                   ext_protocol_conf_opt.content_ptr = NULL;
              }
              break;
            default: 
              break;
          }
          emm_modem_mem_free(message, emm_ctrl_data_ptr);
          if (client->next_message == (emm_pended_msg_list_type *)NULL)
          {
            emm_delete_message_client(client, emm_ctrl_data_ptr) ;
          }
          return ;
        }
        else
        {
          prev_message_ptr = &message->next_message ;

          message = message->next_message ;
        }
      }
      
      client = client->next_client ;
    }
  }
} /* end of emm_delete_pended_message() */

#ifdef FEATURE_CIOT
/* =================================================================================

FUNCTION EMM_DELETE_PENDED_CON_SETUP_MESSAGE

DESCRIPTION
  This function deletes the pended con setup message in the queue.
  This function is called on successful resumption from IDLE state,
  if UE is not supposed to send pended conenction setup message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===================================================================================*/

void emm_del_con_setup_msg(emm_ctrl_data_type *emm_ctrl_data_ptr)
{

  emm_client_list_type     *client = (emm_client_list_type *)NULL ;
  emm_pended_msg_list_type *message = (emm_pended_msg_list_type *)NULL ;
  lte_nas_message_id  msg_id_to_del;


  if (emm_ctrl_data_ptr->client_list != (emm_client_list_type *)NULL)
  {
    if (emm_ctrl_data_ptr->client_list->connection_setup_message != (emm_pended_msg_list_type *)NULL)
    {
      msg_id_to_del = emm_ctrl_data_ptr->client_list->connection_setup_message->nas_out_message.nas_hdr.msg_id;
	  emm_ctrl_data_ptr->client_list->connection_setup_message = NULL;
	  emm_delete_pended_message_with_msg_id(msg_id_to_del,emm_ctrl_data_ptr);
	}
	
  } 	

}

#endif

#ifdef TEST_FRAMEWORK
#error code not present
#endif


/* EMM transaction ID used between EMM and RRC has 32 bits with the format as
+--------------+--------------+----------------------------+
|    8 bits      |   8 bits     |           16 bits            |
|  TECHNOLOGY  |  MODULE_ID   |       TRANSACTION_ID       |
+--------------+--------------+----------------------------+
- TECHNLOGY specifies LTE, eHRPD etc..
- MODULE_ID specifies ESM-BCM, ESM-BPM, EMM etc..
- TRANSACTION_ID is the EMM transaction ID */


dword emm_get_nas_transaction_id
(
  void
)
{
  dword    emm_transaction_id = 0;

  /* Populate the TRANSACTION_ID */
  emm_transaction_id = (emm_ctrl_data_ptr->nas_data_request_transaction_id & 0xFFFF) + 1; 
  /* Populate the TECHNOLOGY */
  emm_transaction_id |= ((unsigned int)NAS_LTE << 24); 
  /* Populate the MODULE_ID */
  emm_transaction_id |= ((unsigned int)LTE_NAS_EMM << 16);

  emm_ctrl_data_ptr->nas_data_request_transaction_id = emm_transaction_id;
  return emm_transaction_id ;
} /* end of emm_get_nas_transaction_id() */


/*===========================================================================
FUNCTION EMM_GET_CAMPED_TAI

DESCRIPTION
  This function returns the camped TAI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
lte_nas_tai_lst1_type emm_get_camped_tai
(
  emm_ctrl_data_type *emm_ctrl_data_ptr,
  boolean            temp_forbidden
)
{
  lte_nas_tai_lst1_type camped_tai;
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);
  camped_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
     emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
  camped_tai.tac  = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;
  camped_tai.t3247_specific = temp_forbidden;
  return camped_tai;
}
/*===========================================================================
FUNCTION EMM_DELETE_FORBIDDEN_TAI

DESCRIPTION
  This function deletes TAI from the appropriate "Forbidden" list either from:
    o rrc_borbidden_list, or
    o forbidden_for_service_list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_delete_forbidden_tai
(
  lte_nas_tai_lst1_type *tai_tobe_deleted, /* Pointer to the TAI */
  lte_nas_tai_lst2_type *forbidden_list    /* Pointer to a list  */
)
{
  byte index = 0 ;

  for (index=0; index<forbidden_list->tai_lst_length && index < LTE_RRC_MAX_NUM_TRACKING_AREAS; index++)
  {
    if ((PLMN_MATCH(tai_tobe_deleted->plmn.identity, forbidden_list->diff_plmn[index].plmn.identity) == TRUE) &&
        (tai_tobe_deleted->tac == forbidden_list->diff_plmn[index].tac))
    {
      byte length_tobe_moved = forbidden_list->tai_lst_length - 1 - index ;
      if((length_tobe_moved > 0) && (index < forbidden_list->tai_lst_length - 1))
      {
        memsmove((void*)&forbidden_list->diff_plmn[index],
                 sizeof(forbidden_list->diff_plmn) - 
                       (index * sizeof(lte_nas_tai_lst1_type)),
                 (void*)&forbidden_list->diff_plmn[index+1],
                 length_tobe_moved*sizeof(lte_nas_tai_lst1_type));
      }
      forbidden_list->tai_lst_length-- ;
      break ;
    }
  }
} /* end of emm_delete_forbidden_tai() */

/*===========================================================================

FUNCTION EMM_ADD_FORBIDDEN_TAI

DESCRIPTION
  This function adds TAI to the appropriate "Forbidden" list either to:
    o rrc_borbidden_list, or
    o forbidden_for_service_list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_add_forbidden_tai
(
  lte_nas_tai_lst2_type *forbidden_list,      /* Pointer to the list               */
  lte_nas_tai_lst1_type  forbidden_tai,
  emm_ctrl_data_type    *emm_ctrl_data_ptr  /* Pointer to Common Global EMM Data */
)
{
  lte_nas_emm_tai_lst_info_type  *local_list_ptr = NULL;
  byte                       length_tobe_moved = 0;

  /*allocate memory on heap*/
  local_list_ptr = (lte_nas_emm_tai_lst_info_type*)emm_modem_mem_alloc(
                               sizeof(lte_nas_emm_tai_lst_info_type),
                                emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void*)local_list_ptr);

  /* Create searchable list */
  local_list_ptr->tai_lst_length               = 1; 
  local_list_ptr->tai_lst_elem[0].tai_lst      = NAS_EMM_TAI_LST2 ; // Init type
  local_list_ptr->tai_lst_elem[0].tai.tai_lst2 = *forbidden_list  ;


  if(mm_timer_status[EMM_FORBIDDEN_TAI_CLEAR_TIMER] == TIMER_STOPPED)  
  {
    mm_start_timer(EMM_FORBIDDEN_TAI_CLEAR_TIMER, EMM_FORBIDDEN_TAI_TIMER_DURATION);
  }

  if (emm_search_tai_list(&forbidden_tai, local_list_ptr) == FALSE)
  {
    byte *list_length = &forbidden_list->tai_lst_length ;

    /* Determine length is to be moved */
    if ((*list_length) == LTE_RRC_MAX_NUM_TRACKING_AREAS)
    {
      length_tobe_moved = (*list_length) - 1 ;
    }
    else
    {
      length_tobe_moved = *list_length ;
    }

    /* Move the "old" list entries */
    memsmove((void*)&forbidden_list->diff_plmn[1],
             sizeof(forbidden_list->diff_plmn) - 
                   sizeof(lte_nas_tai_lst1_type),
             (void*)&forbidden_list->diff_plmn[0],
             length_tobe_moved*sizeof(lte_nas_tai_lst1_type));

    /* Add new TAI to the list */
    forbidden_list->diff_plmn[0] = forbidden_tai ;

    if ((*list_length) < LTE_RRC_MAX_NUM_TRACKING_AREAS)
    {
      (*list_length)++ ;
    }
  }
  /*--------LOG FORBIDDEN TRACKING AREA LIST ------*/
  log_forbidden_tracking_area_info(emm_ctrl_data_ptr->forbidden_for_service_list_ptr,
                                   emm_ctrl_data_ptr->rrc_forbidden_list_ptr);   

  /* Free memory */
  emm_modem_mem_free(local_list_ptr, emm_ctrl_data_ptr);
} /* end of emm_add_forbidden_tai() */

/*===========================================================================

FUNCTION EMM_DELETE_PLMN_FROM_LIST

DESCRIPTION
  This function deletes current PLMN from the list of equivalent PLMNs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_delete_plmn_from_list
(
  /* pointer to PLMN which needs to be deleted from the list */
  sys_plmn_id_s_type        *plmn_tobe_deleted, 

  /* pointer to a PLMN list */
  lte_nas_emm_plmn_lst_type *plmn_list 
)
{
  byte index       = 0 ;
  byte list_length = 0 ;

  list_length = plmn_list->length_of_plmn_lst_contents ;

  if (((index = emm_search_plmn_list(plmn_tobe_deleted, plmn_list)) != 0xFF) &&
       (list_length <= MAX_NO_OF_PLMNs))
  {
    sys_plmn_id_s_type *current_list_plmn = &plmn_list->plmn[index] ;

    if (index < list_length - 1)
    {
      sys_plmn_id_s_type *next_list_plmn = &plmn_list->plmn[index+1] ;

      /* Delete required PLMN from the list */
      memsmove((void*)current_list_plmn,
               sizeof(plmn_list->plmn) - 
                     (index * sizeof(sys_plmn_id_s_type)),
               (void*)next_list_plmn, 
               (list_length - 1 - index) * sizeof(sys_plmn_id_s_type));
    }
    else
    {
      /* Nothing to do */
    }

    /* Decrease length of the list */
    plmn_list->length_of_plmn_lst_contents-- ;
  }
} /* end of emm_delete_plmn_from_list() */

/*===========================================================================

FUNCTION EMM_SEARCH_PLMN_LIST

DESCRIPTION
  This function searches through the list of forbidden PLMNs

DEPENDENCIES
  None

RETURN VALUE
  [0..(List Length-1)] - wanted PLMN has been found in the searched list
  0xFF                 - otherwise

SIDE EFFECTS
  None
===========================================================================*/
byte emm_search_plmn_list
(
  /* pointer to PLMN which needs to be deleted from the list */
  sys_plmn_id_s_type        *plmn_tobe_searched, 

  /* pointer to a PLMN list */
  lte_nas_emm_plmn_lst_type *plmn_list 
)
{
  byte search_index = 0 ;

  for(search_index=0; search_index<plmn_list->length_of_plmn_lst_contents && search_index < MAX_NO_OF_PLMNs; search_index++)
  {
    if (PLMN_MATCH(plmn_tobe_searched->identity, plmn_list->plmn[search_index].identity) == TRUE)
    {
      return search_index ;
    }
  }

  return 0xFF ;
} /* end of emm_search_plmn_list() */

/*===========================================================================
FUNCTION    EMM_UPDATE_EQ_PLMN_LIST

DESCRIPTION
  This function updates the equivalent plmn list entries in the emm database, 
  based on the forbidden TAI LIST received during ATTACH_ACCEPT & TAU_ACCEPT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_update_eq_plmn_list
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  byte                        index = 0;
  lte_nas_emm_plmn_lst_type  *eq_plmn_list_ptr = NULL;

  

  eq_plmn_list_ptr = &emm_ctrl_data_ptr->emm_equivalent_PLMN_list;

  MSG_LOW_DS_1(MM_SUB,"=EMM= Updating eq plmn list w/ length %d",
               eq_plmn_list_ptr->length_of_plmn_lst_contents);

  while((index < eq_plmn_list_ptr->length_of_plmn_lst_contents) &&
        (index < MAX_NO_OF_PLMNs))
  {
    /* Check whether in forbidden PLMN lists */
    if(emm_is_plmn_forbidden(eq_plmn_list_ptr->plmn[index], 
                             emm_ctrl_data_ptr) == TRUE)
    {
      /* Delete PLMN from the EQ PLMN list. */
      /* Note: Length will be subtracted by one inside function call. No need
         to increase index value to access next PLMN. */
      emm_delete_plmn_from_list(&eq_plmn_list_ptr->plmn[index], 
                                eq_plmn_list_ptr);
    }
    else
    {
      /* Increase index value to access next PLMN */
      index = index + 1;
    }
  }  
} /* emm_update_eq_plmn_list */

/*===========================================================================

FUNCTION MM_SEND_MESSAGE

DESCRIPTION
  This function sends a message to a queue and handle the status return

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


/*void mm_send_message( IMH_T * p_message, gs_queue_id_T queue)
{
   gs_status_T status;

   nas_assert_check(p_message != (IMH_T *)NULL) ;

//#ifdef TEST_FRAMEWORK
//   status = prototype_gs_send_message( queue, (void *)p_message, TRUE );
//#else
   status = gs_send_message( queue, (void *)p_message, TRUE );
//#endif

   if (status != GS_SUCCESS)
   {
      MSG_ERROR_DS_1(MM_SUB, "unable to send message, gs status %d", status);
   }
}*/


/*===========================================================================

FUNCTION    EMM_GET_REG_INIT_RRC_TRANS_ID

DESCRIPTION
  This function generates transaction ID for EMM outgoing RRC primitive which 
  is initiated by REG CMD.

  EMM trans ID used between EMM and RRC has 32 bits with the format as
  +--------------+--------------+----------------------------+
  |  8 bits      |   8 bits     |           16 bits            |
  |  TECHNOLOGY  |  MODULE_ID   |       TRANSACTION_ID       |
  +--------------+--------------+----------------------------+
  - TECHNLOGY specifies LTE, eHRPD etc..
  - MODULE_ID specifies ESM-BCM, ESM-BPM, EMM etc..
  - TRANSACTION_ID is the EMM transaction ID 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dword emm_get_reg_init_rrc_trans_id
(
  void
)
{
  dword    emm_transaction_id = 0;

  /* Populate the TRANSACTION_ID */
  emm_transaction_id = (emm_ctrl_data_ptr->reg_init_rrc_trans_id & 0xFFFF) + 1; 
  /* Populate the TECHNOLOGY */
  emm_transaction_id |= ((unsigned int)NAS_LTE << 24); 
  /* Populate the MODULE_ID */
  emm_transaction_id |= ((unsigned int)LTE_NAS_EMM << 16);

  emm_ctrl_data_ptr->reg_init_rrc_trans_id = emm_transaction_id;
  return emm_transaction_id ;

} /* end of emm_get_reg_init_rrc_trans_id() */

/*===========================================================================
FUNCTION EMM_CONVERT_RRC_PLMN_ID_TO_NAS_PLMN_ID

DESCRIPTION
  This function converts from the RRC PLMN ID format (1 BCD digit per byte)
  to the NAS PLMN_id_T format (2 BCD digits per byte).

DEPENDENCIES
  None

RETURN VALUE
  PLMN ID converted to PLMN_id_T format (3 byte array - 2 BCD digits/byte)

SIDE EFFECTS
  None
===========================================================================*/
sys_plmn_id_s_type emm_convert_rrc_plmn_id_to_nas_plmn_id
( 
  lte_rrc_plmn_s plmn_id 
)
{
  sys_plmn_id_s_type nas_plmn_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (plmn_id.num_mnc_digits == 2)
  {
    plmn_id.mnc[2] = 0x0F;
  }

  /* ----------------------------------------
  ** PLMN octet 1 = MCC digit 2 | MCC digit 1
  ** ---------------------------------------- */
  nas_plmn_id.identity[0] = (byte) ((plmn_id.mcc[1] << 4) + plmn_id.mcc[0]);

  /* ----------------------------------------
  ** PLMN octet 2 = MNC digit 3 | MCC digit 3
  ** ---------------------------------------- */
   nas_plmn_id.identity[1] = (byte) ((plmn_id.mnc[2] << 4) + plmn_id.mcc[2]);

  /* ----------------------------------------
   ** PLMN octet 3 = MNC digit 2 | MNC digit 1
   ** ---------------------------------------- */
   nas_plmn_id.identity[2] = (byte) ((plmn_id.mnc[1] << 4) + plmn_id.mnc[0]);

   return nas_plmn_id;
} /* end of emm_convert_rrc_plmn_id_to_nas_plmn_id() */

/*===========================================================================
FUNCTION EMM_CONVERT_NAS_PLMN_ID_TO_RRC_PLMN_ID

DESCRIPTION
  This function converts from the NAS PLMN ID format (2 BCD digits per byte)
  to the RRC PLMN_id_T format (1 BCD digit per byte).
 
DEPENDENCIES
  None

RETURN VALUE
  PLMN ID converted to rrc_plmn_id_type format

SIDE EFFECTS
  None
===========================================================================*/
lte_rrc_plmn_s emm_convert_nas_plmn_id_to_rrc_plmn_id
(
  sys_plmn_id_s_type plmn_id
)
{
  lte_rrc_plmn_s rrc_plmn_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------
  ** PLMN octet 1 = MCC digit 2 | MCC digit 1
  ** ---------------------------------------- */
  rrc_plmn_id.mcc[0] = plmn_id.identity[0] & 0x0F;

  rrc_plmn_id.mcc[1] = (plmn_id.identity[0] & 0xF0) >> 4;

  /* ----------------------------------------
  ** PLMN octet 2 = MNC digit 3 | MCC digit 3
  ** ---------------------------------------- */
  rrc_plmn_id.mcc[2] = plmn_id.identity[1] & 0x0F;

  if ((plmn_id.identity[1] & 0xF0) == 0xF0)
  {
    rrc_plmn_id.num_mnc_digits = 2;
    rrc_plmn_id.mnc[2] = 0x0F; /* Set the 3rd digit to 0xF as per the spec */
  }
  else
  {
    rrc_plmn_id.num_mnc_digits = 3;
    rrc_plmn_id.mnc[2] = (plmn_id.identity[1] & 0xF0) >> 4;
  }

  /* ----------------------------------------
  ** PLMN octet 3 = MNC digit 2 | MNC digit 1
  ** ---------------------------------------- */
  rrc_plmn_id.mnc[0] = plmn_id.identity[2] & 0x0F;

  rrc_plmn_id.mnc[1] = (plmn_id.identity[2] & 0xF0) >> 4;

  return rrc_plmn_id;
} /* end of emm_convert_nas_plmn_id_to_rrc_plmn_id() */

/*=========================================================================== 
 
FUNCTION  EMM_ALREADY_CAMPED

DESCRIPTION
  This function checks if UE is already camped on LTE
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE: UE is camped
  FALSE: UE is not camped

SIDE EFFECTS
  None
 
===========================================================================*/
boolean emm_already_camped
(
 mmr_reg_req_s_type *reg_req_ptr, 
 emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  boolean            camped = FALSE;
  emm_state_type     emm_state = EMM_INVALID_STATE;
  emm_substate_type  emm_substate;
  boolean service_change_pending;



  
  if(((emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_CS_PS) &&
       (reg_req_ptr->req_service_domain == SYS_SRV_DOMAIN_PS_ONLY))
      ||
      ((emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
       (reg_req_ptr->req_service_domain == SYS_SRV_DOMAIN_CS_PS)))
  {
    service_change_pending = TRUE;
  }
  else
  {
    service_change_pending = FALSE;
  }

  emm_state = EMM_GET_STATE();
  emm_substate = EMM_GET_SUBSTATE();

  MSG_HIGH_DS_2(MM_SUB,"=EMM= Chk if UE camped - state %d, substate %d", 
                emm_state, emm_substate);
  MSG_HIGH_DS_2(MM_SUB,"=EMM= Chk if UE camped - Srv status %d, reg srv domain %d",
                emm_ctrl_data_ptr->plmn_service_state->service_status, 
                emm_ctrl_data_ptr->plmn_service_state->reg_service_domain);
  //Do not return TRUE if there is a REG request already getting processed.
  if(reg_service_report != (emm_reg_service_report_type)emm_send_reg_cnf)
  {
    switch(emm_state)
    {
      case EMM_NULL:
      case EMM_DEREGISTERED:
        camped = FALSE;
        break ;
      case EMM_REGISTERED:
      case EMM_SERVICE_REQUEST_INITIATED:
      case EMM_DEREGISTERED_INITIATED:
        if((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                              SYS_SRV_STATUS_SRV)
           &&
           ((emm_state != EMM_REGISTERED) || 
            (emm_substate != EMM_REGISTERED_NO_CELL_AVAILABLE))
            &&
            (emm_ctrl_data_ptr->rrc_active == TRUE)
           &&
           ((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain
                                                    == SYS_SRV_DOMAIN_PS_ONLY) ||
            (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain 
                                                       == SYS_SRV_DOMAIN_CS_PS)))
        {
          if(PLMN_MATCH(reg_req_ptr->plmn.identity,emm_ctrl_data_ptr->plmn_service_state->plmn.identity)
  #if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW) 
              && ((emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id == reg_req_ptr->csg_id) ||
                  (reg_req_ptr->csg_id == SYS_CSG_ID_INVALID))
              
  #endif 
            )
          {
            camped = TRUE; 
          }
  
        }
        else
        {
          camped = FALSE;
        }
        break ;
    
      case EMM_TRACKING_AREA_UPDATING_INITIATED:
        if((service_change_pending == FALSE)
           &&
           (emm_ctrl_data_ptr->rrc_active == TRUE)
           &&
		   (emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                              SYS_SRV_STATUS_SRV)
           &&
           ((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain
                                                    == SYS_SRV_DOMAIN_CS_ONLY) ||
            (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain
                                                    == SYS_SRV_DOMAIN_PS_ONLY) ||
            (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain 
                                                       == SYS_SRV_DOMAIN_CS_PS)))
        {
          camped = (PLMN_MATCH(reg_req_ptr->plmn.identity,emm_ctrl_data_ptr->plmn_service_state->plmn.identity)) ? TRUE : FALSE ;
        }
        else
        {
          camped = FALSE;
        }
        break;

      case EMM_REGISTERED_INITIATED:
        if((service_change_pending == FALSE)
            &&
           (emm_ctrl_data_ptr->rrc_active == TRUE)
		    &&
           (emm_ctrl_data_ptr->rlf_state == FALSE))
        {
          camped = (PLMN_MATCH(reg_req_ptr->plmn.identity,emm_ctrl_data_ptr->plmn_service_state->plmn.identity)) ? TRUE : FALSE ;
        }
        else
        {
          camped = FALSE;
        }
        break;

      default:
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM state",0,0,0) ;
        break ;
    }
  
    if(camped == TRUE)
    {
      if((mm_rat_pri_list_match(&reg_req_ptr->rat_pri_list_info,
                           &emm_ctrl_data_ptr->reg_req_pending_ptr->rat_pri_list_info)
                    == FALSE)
#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED) || !defined(FEATURE_NAS_DISABLE_VOICE)
         ||
         (emm_lte_cs_domain_param_match(reg_req_ptr->lte_cs_domain_param,
                                      *(emm_ctrl_data_ptr->reg_lte_pended_cs_domain_param_ptr))
                    == FALSE)
#endif
         )
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= RAT PRI LIST or lte_cs_domain_pref changed");
        camped = FALSE;
      }
    }
  }

  return camped ;

} /* end of emm_already_camped() */

/*===========================================================================

FUNCTION  EMM_STOP_ALL_TIMERS

DESCRIPTION
  This function stops all EMM timers except for EMM_POWEROFF_DETACH_TIMER

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_stop_all_timers
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  int i = 0;

  for(i = MIN_EMM_TIMER; i <= MAX_EMM_TIMER; i++)
  {
    if (mm_timer_status[i] != TIMER_STOPPED)
    {
      //if( (i != EMM_POWEROFF_DETACH_TIMER) && (i != TIMER_T3421) )
      if((i != EMM_POWEROFF_DETACH_TIMER) && 
         (i != EMM_FORBIDDEN_TAI_CLEAR_TIMER) &&
         (i != TIMER_EMM_PS_DETACH) &&
         (i != TIMER_T3412)&& (i != TIMER_T3423)&&
         (i != TIMER_T3247) &&
         (i != TIMER_T3448))
      {
        mm_stop_timer( (timer_id_T)i );
        mm_timer_status[i] = TIMER_STOPPED;
      }
    }
  }

} /* end of emm_stop_all_timers() */

/*===========================================================================

FUNCTION  EMM_STOP_CELL_BARRING_TIMERS

DESCRIPTION
  This function stops barring for all call types or only the non-EAB specific
  (Emergency call, MT call) call types.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_stop_cell_barring_timers
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
#ifdef FEATURE_NAS_EAB
  , boolean          stop_eab_barring
#endif
)
{
  /* Parameter stop_eab_barring indicates whether call types specific to EAB
  barring should be unbarred. When EAB barring is active there is no barring
  timer provided by RRC. So only the barring bitmask is updated in
  emm_ctrl_data_ptr->access_barring_flag to indefinitely bar the UE for
  EAB specific call types (MO Signaling, MO Data, MO CSFB and Delay Tolerant)
  and no timer is started in NAS for these call types. Therefore barring timers
  corresponding to these calls are already stopped and no additional check is
  necessary to make sure that these timers are NOT accidentally stopped when
  EAB barring is active and the parameter stop_eab_barring indicates that EAB
  barring should not be stopped. Though EAB barring never starts the timers
  corresponding to the EAB specific call types, hence making EAB barring indefinite,
  RRC can lift EAB barring by sending LTE_RRC_EAB_UPDATE_IND to NAS in which case
  emm_ctrl_data_ptr->access_barring_flag is updated to stop barring for EAB 
  specific call types */
  
     emm_timer_barring_status_e_type status;
	 emm_timer_barring_status_category_e_type category;  
	(void)pthread_mutex_lock(emm_ctrl_data_ptr->emm_mutex_ptr);
#ifdef FEATURE_NAS_EAB
    if((emm_ctrl_data_ptr->eab_barring == TRUE) &&
       (stop_eab_barring == FALSE))
    {
      /* Lift Access Class barring for non-EAB specific types of calls */
      emm_ctrl_data_ptr->access_barring_flag &= EMM_EAB_BARRING_BITMASK;
    }
    else
#endif
    {
      /* Lift Access Class barring for all types of calls */
      emm_ctrl_data_ptr->access_barring_flag = 0 ; 
#ifdef FEATURE_NAS_EAB
      emm_ctrl_data_ptr->eab_barring = FALSE;
#endif
    }
    (void)pthread_mutex_unlock(emm_ctrl_data_ptr->emm_mutex_ptr);

#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
  if (rex_get_timer(&emm_ctrl_data_ptr->emm_access_barring_emergency_timer) > 0)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Emergency Call barring timer has been stopped");
    (void)rex_clr_timer(&emm_ctrl_data_ptr->emm_access_barring_emergency_timer) ;
  }
#endif
  if (rex_get_timer(&emm_ctrl_data_ptr->emm_access_barring_mt_access_timer) > 0)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= MT Call barring timer has been stopped");
    (void)rex_clr_timer(&emm_ctrl_data_ptr->emm_access_barring_mt_access_timer) ;
  }
#ifdef FEATURE_CIOT
  if (rex_get_timer(&emm_ctrl_data_ptr->emm_access_barring_mo_excep_data_timer) > 0)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= MO Exception barring timer has been stopped");
    (void)rex_clr_timer(&emm_ctrl_data_ptr->emm_access_barring_mo_excep_data_timer) ;
	status = EMM_TIMER_BARRING_STATUS_STOP;
	category = EMM_TIMER_BARRING_MO_EXP_DATA;
	emm_send_emm_timer_barring_status_ind(status, category);
  }
#endif

  if (rex_get_timer(&emm_ctrl_data_ptr->emm_access_barring_mo_signaling_timer) > 0)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= MO Signaling barring timer has been stopped");
    (void)rex_clr_timer(&emm_ctrl_data_ptr->emm_access_barring_mo_signaling_timer) ;
    if(emm_ctrl_data_ptr->ongoing_detach_procedure == TRUE)
    {
      /* 5.5.2.2.4 a: Restart detach when access is not barred any more */
      emm_start_mo_detach(emm_ctrl_data_ptr->ongoing_detach_type, FALSE, NULL, 
                          emm_detach_failed_routine, emm_ctrl_data_ptr) ;
    }
  }

  if (rex_get_timer(&emm_ctrl_data_ptr->emm_access_barring_mo_data_timer) > 0)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= MO Data Call barring timer has been stopped");
    (void)rex_clr_timer(&emm_ctrl_data_ptr->emm_access_barring_mo_data_timer) ;
	status = EMM_TIMER_BARRING_STATUS_STOP;
	category = EMM_TIMER_BARRING_MO_DATA;
	emm_send_emm_timer_barring_status_ind(status, category);
  }

#ifdef FEATURE_NAS_GW
  if (rex_get_timer(&emm_ctrl_data_ptr->emm_access_barring_mo_csfb_timer) > 0)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= MO CSFB Call barring timer has been stopped");
    (void)rex_clr_timer(&emm_ctrl_data_ptr->emm_access_barring_mo_csfb_timer) ;
  }
#endif

  emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;

} /* end of emm_stop_cell_barring_timers() */

lte_emm_connection_cause_type emm_translate_ul_data_cnf_status
(
  lte_rrc_ul_data_cnf_status_e rrc_status
)
{
  lte_emm_connection_cause_type emm_connection_cause = TRANSMISSION_LTE_RRC_UL_DATA_CNF_SUCCESS ;

  switch (rrc_status)
  {
    case LTE_RRC_UL_DATA_CNF_SUCCESS:
      emm_connection_cause = TRANSMISSION_LTE_RRC_UL_DATA_CNF_SUCCESS ;
      break ;
    case LTE_RRC_UL_DATA_CNF_FAILURE_TXN:
      emm_connection_cause = TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN ;
      break ;
    case LTE_RRC_UL_DATA_CNF_FAILURE_HO:
      emm_connection_cause = TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO ;
      break ;
    case LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL:
      emm_connection_cause = TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL ;
      break ;
    case LTE_RRC_UL_DATA_CNF_FAILURE_RLF:
      emm_connection_cause = TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_RLF ;
      break ; 
    case LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
      emm_connection_cause = TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN ;
      break ; 
    default:
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal RRC UL DATA CONFIRMATION status",0,0,0) ;
      break ;
  }

  return emm_connection_cause ;

} /* end of emm_translate_ul_data_cnf_status() */

lte_nas_tai_lst1_type  emm_convert_ta
(
  lte_rrc_tracking_area_s ta
)
{
  lte_nas_tai_lst1_type nas_tai ; 

  memset((void *)&nas_tai,0,sizeof(lte_nas_tai_lst1_type));

  nas_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(ta.plmn) ;
    nas_tai.tac  = (word)ta.tac ;

  return nas_tai ;
} /* end of emm_convert_ta() */

/*===========================================================================

FUNCTION    EMM_SEARCH_TAI_LIST

DESCRIPTION
  This checks wheather provided TAI is belonging to the stored TAI list

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - provided TAI IS belonging to the stored TAI list
  FALSE - provided TAI is NOT belonging to the stored TAI list

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_search_tai_list
(
  /* Provided TAI */
  lte_nas_tai_lst1_type *provided_tai,

  /* Stored TAI list */
  lte_nas_emm_tai_lst_info_type *tai_list
)
{
  /* Index var for stored TAI list */
  byte list_index = 0 ;
  
  /* Index var for element of the stored TAI list */
  byte index      = 0 ;

  for (list_index=0; list_index<tai_list->tai_lst_length; list_index++)
  {
    switch (tai_list->tai_lst_elem[list_index].tai_lst)
    {
      case NAS_EMM_TAI_LST0:
        {
          /* Extract PLMN from the TAI list element */
          sys_plmn_id_s_type list_plmn = tai_list->tai_lst_elem[list_index].tai.tai_lst0.plmn ;

          /* Check whether extracted PLMN matches */
          if (!PLMN_MATCH(list_plmn.identity, provided_tai->plmn.identity))
          {
            /* If PLMN does not match go to the next element of the list */
            continue ;
          }

          /* Search throught the TAC list */
          for (index=0; index<tai_list->tai_lst_elem[list_index].tai.tai_lst0.tac_lst_length; index++)
          {
            if (tai_list->tai_lst_elem[list_index].tai.tai_lst0.tac_lst[index] == provided_tai->tac)
            {
              /* Provided TAI IS belonging to the stored TAI list */
              return TRUE ;
            }
          }
        }
        break ;
      case NAS_EMM_TAI_LST1:
        {
          /* Extract PLMN from the TAI list element */
          sys_plmn_id_s_type list_plmn = tai_list->tai_lst_elem[list_index].tai.tai_lst1.plmn ;
          word               list_tac  = tai_list->tai_lst_elem[list_index].tai.tai_lst1.tac ;

          /* Check whether extracted PLMN matches */
          if (!PLMN_MATCH(list_plmn.identity, provided_tai->plmn.identity))
          {
            /* If PLMN does not match go to the next element of the list */
            continue ;
          }

          if (provided_tai->tac == list_tac)
          {
            /* Provided TAI IS belonging to the stored TAI list */
            return TRUE ;
          }
        }
        break ;
      case NAS_EMM_TAI_LST2:
        /* Search throught the TAC list */
        for (index=0; index<tai_list->tai_lst_elem[list_index].tai.tai_lst2.tai_lst_length; index++)
        {
          /* Extract PLMN from the TAI list element */
          sys_plmn_id_s_type list_plmn = tai_list->tai_lst_elem[list_index].tai.tai_lst2.diff_plmn[index].plmn ;
          word               list_tac  = tai_list->tai_lst_elem[list_index].tai.tai_lst2.diff_plmn[index].tac ;

          /* Check whether extracted PLMN matches */
          if (!PLMN_MATCH(list_plmn.identity, provided_tai->plmn.identity))
          {
            /* If PLMN does not match go to the next element of the list */
            continue ;
          }

          if (provided_tai->tac == list_tac)
          {
            /* Provided TAI IS belonging to the stored TAI list */
            return TRUE ;
          }
        }
        break ;
      default:
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal TAI list type",0,0,0) ;
        break ;
    }
  }

  /* Provided TAI is NOT belonging to the stored TAI list */
  return FALSE ;
} /* end of emm_search_tai_list() */


/*===========================================================================

FUNCTION    EMM_BLOCK_TAI_LIST_PLMNS

DESCRIPTION
  This functions blocks all the PLMNs that are part of TAI list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_block_tai_list_plmns
(
  sys_plmn_id_s_type *plmn,
  rex_timer_cnt_type  timer,
  sys_block_plmn_cause_e_type cause,
  lte_nas_emm_tai_lst_info_type *tai_list
)
{
  /* Index var for stored TAI list */
  byte list_index = 0 ;
  
  /* Index var for element of the stored TAI list */
  byte index      = 0 ;

  for (list_index=0; list_index<tai_list->tai_lst_length; list_index++)
  {
    switch (tai_list->tai_lst_elem[list_index].tai_lst)
    {
      case NAS_EMM_TAI_LST0:
        {
          /* Extract PLMN from the TAI list element */
          sys_plmn_id_s_type list_plmn = tai_list->tai_lst_elem[list_index].tai.tai_lst0.plmn ;

          /* Check whether extracted PLMN matches */
          if (PLMN_MATCH(list_plmn.identity, plmn->identity))
          {
            continue ;
          }
          reg_sim_add_plmn_to_backoff_fplmn_list(list_plmn,timer,cause,(sys_modem_as_id_e_type)mm_sub_id);
        }
        break ;
      case NAS_EMM_TAI_LST1:
        {
          /* Extract PLMN from the TAI list element */
          sys_plmn_id_s_type list_plmn = tai_list->tai_lst_elem[list_index].tai.tai_lst1.plmn ;

          /* Check whether extracted PLMN matches */
          if (PLMN_MATCH(list_plmn.identity, plmn->identity))
          {
            continue ;
          }

          reg_sim_add_plmn_to_backoff_fplmn_list(list_plmn,timer,cause,(sys_modem_as_id_e_type)mm_sub_id);
        }
        break ;
      case NAS_EMM_TAI_LST2:
        /* Search throught the TAC list */
        for (index=0; index<tai_list->tai_lst_elem[list_index].tai.tai_lst2.tai_lst_length; index++)
        {
          /* Extract PLMN from the TAI list element */
          sys_plmn_id_s_type list_plmn = tai_list->tai_lst_elem[list_index].tai.tai_lst2.diff_plmn[index].plmn ;

          /* Check whether extracted PLMN matches */
          if (PLMN_MATCH(list_plmn.identity, plmn->identity))
          {
            continue ;
          }

          reg_sim_add_plmn_to_backoff_fplmn_list(list_plmn,timer,cause,(sys_modem_as_id_e_type)mm_sub_id);
        }
        break ;
      default:
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal TAI list type",0,0,0) ;
        break ;
    }
  }

  /* Provided TAI is NOT belonging to the stored TAI list */
  return ;
} /* end of emm_search_tai_list() */

/*===========================================================================

FUNCTION    EMM_DELETE_TAI_FROM_LIST

DESCRIPTION
  This function deletes provided TAI from the TAI list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_delete_tai_from_list
(
  /* Provided TAI */
  lte_nas_tai_lst1_type *provided_tai,

  /* Stored TAI list */
  lte_nas_emm_tai_lst_info_type *tai_list
)
{
  /* Index var for stored TAI list */
  byte list_index = 0 ;
  
  /* Index var for element of the stored TAI list */
  byte index      = 0 ;


  MSG_HIGH_DS_0(MM_SUB, "=EMM= Remove the current TAI from stored TAI list");

  for (list_index=0; list_index<tai_list->tai_lst_length && list_index < MAX_TAI_LST_LENGTH; list_index++)
  {
    switch (tai_list->tai_lst_elem[list_index].tai_lst)
    {
      case NAS_EMM_TAI_LST0:
        {
          /* Extract PLMN from the TAI list element */
          sys_plmn_id_s_type list_plmn       = tai_list->tai_lst_elem[list_index].tai.tai_lst0.plmn ;
          byte               tac_list_length = tai_list->tai_lst_elem[list_index].tai.tai_lst0.tac_lst_length ;

          /* Check whether extracted PLMN matches */
          if (!PLMN_MATCH(list_plmn.identity, provided_tai->plmn.identity))
          {
            /* If PLMN does not match go to the next element of the list */
            continue ;
          }

          /* Search throught the TAC list */
          for (index=0; index<tac_list_length && index < MAX_NO_OF_TAI; index++)
          {
            if (tai_list->tai_lst_elem[list_index].tai.tai_lst0.tac_lst[index] == provided_tai->tac)
            {
              word *current_tac = &tai_list->tai_lst_elem[list_index].tai.tai_lst0.tac_lst[index] ;

              if (index < tac_list_length - 1)
              {
                word *next_tac = &tai_list->tai_lst_elem[list_index].tai.tai_lst0.tac_lst[index+1] ;

                /* Delete required TAC from the list */
                memsmove((void*)current_tac,
                         sizeof(tai_list->tai_lst_elem[list_index].tai.tai_lst0.tac_lst) -
                               (index * sizeof(word)),
                         (void*)next_tac, 
                         (tac_list_length - 1 - index) * sizeof(word));
              }
              else
              {
                /* Nothing to do */
              }
     
              /* Decrease length of the list */
              tai_list->tai_lst_elem[list_index].tai.tai_lst0.tac_lst_length-- ;

              return ;
            }
          }
        }
        break ;
      case NAS_EMM_TAI_LST1:
        {
          /* Extract PLMN from the TAI list element */
          sys_plmn_id_s_type list_plmn = tai_list->tai_lst_elem[list_index].tai.tai_lst1.plmn ;
          word               list_tac  = tai_list->tai_lst_elem[list_index].tai.tai_lst1.tac ;

          /* Check whether extracted PLMN matches */
          if (!PLMN_MATCH(list_plmn.identity, provided_tai->plmn.identity))
          {
            /* If PLMN does not match go to the next element of the list */
            continue ;
          }

          if (provided_tai->tac == list_tac)
          {
            emm_tai_lst_elem_type *current_tai = &tai_list->tai_lst_elem[list_index] ;

            if (list_index < tai_list->tai_lst_length - 1)
            {
              emm_tai_lst_elem_type *next_tai = &tai_list->tai_lst_elem[list_index+1] ;

              /* Delete required TAC from the list */
              memsmove((void*)current_tai,
                       sizeof(tai_list->tai_lst_elem) - 
                             (list_index * sizeof(emm_tai_lst_elem_type)), 
                       (void*)next_tai, 
                       (tai_list->tai_lst_length - 1 - list_index) * sizeof(emm_tai_lst_elem_type));
            }
            else
            {
              /* Nothing to do */
            }
     
            /* Decrease length of the list */
            tai_list->tai_lst_length-- ;

            return ;
          }
        }
        break ;
      case NAS_EMM_TAI_LST2:
        {
          byte tai_list_length = tai_list->tai_lst_elem[list_index].tai.tai_lst2.tai_lst_length ;

          /* Search throught the TAC list */
          for (index=0; index<tai_list_length && index < LTE_RRC_MAX_NUM_TRACKING_AREAS; index++)
          {
            /* Extract PLMN from the TAI list element */
            sys_plmn_id_s_type list_plmn = tai_list->tai_lst_elem[list_index].tai.tai_lst2.diff_plmn[index].plmn ;
            word               list_tac  = tai_list->tai_lst_elem[list_index].tai.tai_lst2.diff_plmn[index].tac ;

            /* Check whether extracted PLMN matches */
            if (!PLMN_MATCH(list_plmn.identity, provided_tai->plmn.identity))
            {
              /* If PLMN does not match go to the next element of the list */
              continue ;
            }

            if (provided_tai->tac == list_tac)
            {
              lte_nas_tai_lst1_type *current_tai = &tai_list->tai_lst_elem[list_index].tai.tai_lst2.diff_plmn[index] ;

              if (index < tai_list_length - 1)
              {
                lte_nas_tai_lst1_type *next_tai = &tai_list->tai_lst_elem[list_index].tai.tai_lst2.diff_plmn[index+1] ;

                /* Delete required TAC from the list */
                memsmove((void*)current_tai,
                         sizeof(tai_list->tai_lst_elem[list_index].tai.tai_lst2.diff_plmn) -
                               (index * sizeof(lte_nas_tai_lst1_type)),
                         (void*)next_tai, 
                         (tai_list_length - 1 - index) * sizeof(lte_nas_tai_lst1_type));
              }
              else
              {
                /* Nothing to do */
              }
     
              /* Decrease length of the list */
              tai_list->tai_lst_elem[list_index].tai.tai_lst2.tai_lst_length-- ;

              return ;
            }
          }
          break ;
        }
      default:
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal TAI list type",0,0,0) ;
        break ;
    }
  }
} /* end of emm_delete_tai_from_list() */

#ifdef FEATURE_CIOT
int8 emm_search_dcn_list(emm_ctrl_data_type *emm_ctrl_data_ptr,sys_plmn_id_s_type  plmn, int8 *index)
{
  int8 i= 0;
  for(i=0;i<LTE_NAS_MAX_DCN_IDS;i++)
  {
    if(sys_plmn_id_is_undefined( emm_ctrl_data_ptr->dcn_list.dcn_list[i].plmn))
    {
      *index = i;
	  return -1;
    }
    else if( sys_plmn_match(emm_ctrl_data_ptr->dcn_list.dcn_list[i].plmn,plmn))
    {
      return i;
	  
    }
  }
  return -1;
}
void emm_process_dcn_list(uint16 dcn_id,  emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  int8 i= 0, index = -1;
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= update the DCN id list");
  if(sys_plmn_id_is_undefined( emm_ctrl_data_ptr->rplmn_info.rplmn))
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= RPLMN not valid");
    return;
  }
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Remove the current TAI from stored TAI list");
  i = emm_search_dcn_list(emm_ctrl_data_ptr,emm_ctrl_data_ptr->rplmn_info.rplmn,&index);
  
  if(i >= 0 && i< LTE_NAS_MAX_DCN_IDS)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= update the existing DCN id for the plmn");
    emm_ctrl_data_ptr->dcn_list.dcn_list[i].plmn = emm_ctrl_data_ptr->rplmn_info.rplmn;
	emm_ctrl_data_ptr->dcn_list.dcn_list[i].dcn_id = dcn_id;
  }
  else if(index >= 0 && index < LTE_NAS_MAX_DCN_IDS)
  {
    
    MSG_HIGH_DS_0(MM_SUB, "=EMM= update the new DCN id for the plmn");
    emm_ctrl_data_ptr->dcn_list.dcn_list[index].plmn = emm_ctrl_data_ptr->rplmn_info.rplmn;
	emm_ctrl_data_ptr->dcn_list.dcn_list[index].dcn_id = dcn_id;
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= update the new DCN id for the plmn replacing the oldest entry");
    for(i=0;i<LTE_NAS_MAX_DCN_IDS-1;i++)
    {
     emm_ctrl_data_ptr->dcn_list.dcn_list[i].plmn = emm_ctrl_data_ptr->dcn_list.dcn_list[i+1].plmn;
     emm_ctrl_data_ptr->dcn_list.dcn_list[i].dcn_id = emm_ctrl_data_ptr->dcn_list.dcn_list[i+1].dcn_id;
    }
	emm_ctrl_data_ptr->dcn_list.dcn_list[i].plmn = emm_ctrl_data_ptr->rplmn_info.rplmn;
	emm_ctrl_data_ptr->dcn_list.dcn_list[i].dcn_id = dcn_id;
  }
  /* dump the whole list for debugging, remove when feature is stable*/
  for(i=0;i<LTE_NAS_MAX_DCN_IDS;i++)
  {
    sys_plmn_get_mcc_mnc
    (
      emm_ctrl_data_ptr->dcn_list.dcn_list[i].plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
     );
	MSG_HIGH_DS_3(MM_SUB, "=EMM= DCN PLMN(%u-%u) ID (%d)", mcc, mnc,emm_ctrl_data_ptr->dcn_list.dcn_list[i].dcn_id);
  }
}

#endif
/*===========================================================================

FUNCTION    EMM_PROCESS_TAI_LIST

DESCRIPTION
  This processes newly received TAI list in order to reprocess existing 
  forbidden lists: 
    o rrc_forbidden_list
    o forbidden_for_service_list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function may modify:
    o rrc_forbidden_list
    o forbidden_for_service_list
===========================================================================*/
void emm_process_tai_list
(
  lte_nas_emm_tai_lst_info_type  *tai_list,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  
  byte  list_index = 0; /* Index var for stored TAI list */
  byte  index      = 0; /* Index var for elements of the stored TAI list */
  lte_nas_tai_lst1_type  tai_tobe_deleted;

  for(list_index=0; list_index < tai_list->tai_lst_length; list_index++)
  {
    switch(tai_list->tai_lst_elem[list_index].tai_lst)
    {
      case NAS_EMM_TAI_LST0:
        /* Extract PLMN from the TAI list element */
        tai_tobe_deleted.plmn = 
                        tai_list->tai_lst_elem[list_index].tai.tai_lst0.plmn;

        /* Remove PLMN from forbidden PLMN lists */
        emm_remove_forbidden_plmn(tai_tobe_deleted.plmn, emm_ctrl_data_ptr);

        /* Search throught the TAC list */
        for (index=0; index < tai_list->tai_lst_elem[list_index].\
                                          tai.tai_lst0.tac_lst_length; index++)
        {
          tai_tobe_deleted.tac = 
                tai_list->tai_lst_elem[list_index].tai.tai_lst0.tac_lst[index];

          /* Delete LAIs from the forbidden lists */
          emm_delete_forbidden_tai(&tai_tobe_deleted,
                                   emm_ctrl_data_ptr->rrc_forbidden_list_ptr);
          emm_delete_forbidden_tai(&tai_tobe_deleted,
                               emm_ctrl_data_ptr->forbidden_for_service_list_ptr);
          emm_delete_manual_ftai(&tai_tobe_deleted,emm_ctrl_data_ptr);
        }      
        break ;
      case NAS_EMM_TAI_LST1:
        /* Create TAI which needs to be deleted */
        tai_tobe_deleted.plmn = 
                          tai_list->tai_lst_elem[list_index].tai.tai_lst1.plmn;
        tai_tobe_deleted.tac = 
                           tai_list->tai_lst_elem[list_index].tai.tai_lst1.tac;

        /* Remove PLMN from forbidden PLMN lists */
        emm_remove_forbidden_plmn(tai_tobe_deleted.plmn, emm_ctrl_data_ptr);

        /* Delete LAIs from the forbidden lists */
        emm_delete_forbidden_tai(&tai_tobe_deleted,
                                   emm_ctrl_data_ptr->rrc_forbidden_list_ptr);
        emm_delete_forbidden_tai(&tai_tobe_deleted,
                               emm_ctrl_data_ptr->forbidden_for_service_list_ptr);
        emm_delete_manual_ftai(&tai_tobe_deleted,emm_ctrl_data_ptr);
        break ;
      case NAS_EMM_TAI_LST2:
        /* Search throught the TAC list */
        for (index=0; index<tai_list->tai_lst_elem[list_index].tai.tai_lst2\
                                                      .tai_lst_length; index++)
        {
          /* Create TAI which needs to be deleted */
          tai_tobe_deleted.plmn = tai_list->tai_lst_elem[list_index].\
                                            tai.tai_lst2.diff_plmn[index].plmn;
          tai_tobe_deleted.tac = tai_list->tai_lst_elem[list_index].tai.\
                                                 tai_lst2.diff_plmn[index].tac;

          /* Remove PLMN from forbidden PLMN lists */
          emm_remove_forbidden_plmn(tai_tobe_deleted.plmn, emm_ctrl_data_ptr);

          /* Delete LAIs from the forbidden lists */
          emm_delete_forbidden_tai(&tai_tobe_deleted,
                                   emm_ctrl_data_ptr->rrc_forbidden_list_ptr);
          emm_delete_forbidden_tai(&tai_tobe_deleted,
                               emm_ctrl_data_ptr->forbidden_for_service_list_ptr);
          emm_delete_manual_ftai(&tai_tobe_deleted,emm_ctrl_data_ptr);
        }
        break ;
      default:
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal TAI list type",0,0,0) ;
        break ;
    }
  }

  /*--------LOG FORBIDDEN TRACKING AREA LIST ------*/
  log_forbidden_tracking_area_info(
                                emm_ctrl_data_ptr->forbidden_for_service_list_ptr,
                                emm_ctrl_data_ptr->rrc_forbidden_list_ptr);

} /* end of emm_process_tai_list() */

/*===========================================================================

FUNCTION    EMM_UPDATE_EPS_STATUS

DESCRIPTION
  This function changes EPS UPDATE STATUS

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - MMR_REG_CNF or MMR_SERVICE_IND IS required
  FALSE - MMR_REG_CNF or MMR_SERVICE_IND is NOT required

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_update_eps_status
(
  /* Current EMM EPS Update Status */
  emm_eps_update_status_type current_eps_update_status,

  /* EMM control data */
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  boolean neighbor_update_needed = FALSE ;
  boolean ds_suspend_request     = FALSE ;

   

  if (current_eps_update_status == EMM_STATUS_UPDATED)
  {
    ds_suspend_request = FALSE ;
  }
  else
  {
    ds_suspend_request = TRUE ;
  }

  if (emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend == ds_suspend_request)
  {
    neighbor_update_needed = FALSE ;
  }
  else
  {
    neighbor_update_needed = TRUE ;
  }

  emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend = ds_suspend_request ;

  return neighbor_update_needed ;
} /* end of emm_update_eps_status() */

/*===========================================================================

FUNCTION    EMM_UPDATE_PLMN_SERVICE_STATUS

DESCRIPTION
  This function changes current PLMN Service Status

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - MMR_REG_CNF or MMR_SERVICE_IND IS required
  FALSE - MMR_REG_CNF or MMR_SERVICE_IND is NOT required

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_update_plmn_service_status
(
  /* Current EMM EPS Update Status */
  sys_srv_status_e_type          current_sys_srv_status,

  /* EMM control data */
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  boolean neighbor_update_needed = FALSE ;
  boolean ds_suspend_request     = FALSE ;

   

  if (current_sys_srv_status == SYS_SRV_STATUS_SRV)
  {
    ds_suspend_request = FALSE ;
  }
  else
  {
    ds_suspend_request = TRUE ;
  }

  if (emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend == ds_suspend_request)
  {
    neighbor_update_needed = FALSE ;
  }
  else
  {
    neighbor_update_needed = TRUE ;
  }

  emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend = ds_suspend_request ;

  return neighbor_update_needed ;
} /* end of emm_update_eps_status() */

/*===========================================================================

FUNCTION    EMM_UPDATE_PLMN_SERVICE_STATUS

DESCRIPTION
Reset as needed. Only Some parameters will need to be reset and other retained to be used
on coming back to LTE upon inter-RAT procedures

THIS SHOULD BE CALLED ONLY AFTER DEACTIVATE CNF is received from the RRC in the NON-INTER RAT
cases and should be called after EMM receives DETACH indication with reason as IRAT

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
 IF any data needed for Inter-RAT handling is reset it will cause unpredictable behaviour.
 So, WHEN CHANGING THIS FUNCTION UTMOST CAUTION needs to be excercised 

===========================================================================*/
void emm_reset_ctrl_data
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr,
  boolean              reset_ftai_list       
)
{
  sys_sim_state_e_type sim_state;
  sys_radio_access_tech_e_type  cur_rat;


  /* Reset plmn_service_state except for sim_state */

  /* Save the sim state before resetting the plmn_service_state. After restting save the sim state back*/
  sim_state = emm_ctrl_data_ptr->plmn_service_state->sim_state;
  cur_rat   = emm_ctrl_data_ptr->plmn_service_state->active_rat;

  memset( ((void *)emm_ctrl_data_ptr->plmn_service_state),0x0,sizeof(sys_plmn_service_state_s_type) );

  /* set RAT to NONE as 0 is SYS_GSM_RADIO_ACCESS*/
  emm_ctrl_data_ptr->plmn_service_state->active_rat = cur_rat;

  //emm_ctrl_data_ptr->plmn_service_state = plmn_service_state;
  /* Save the saved sim-state back */
  emm_ctrl_data_ptr->plmn_service_state->sim_state = sim_state;


  /* Reset the rest */
  emm_ctrl_data_ptr->emm_connection_state = EMM_IDLE_STATE;
  emm_ctrl_data_ptr->rrc_deact_req.deact_reason = SYS_STOP_MODE_REASON_NONE;
  emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = FALSE;

  emm_ctrl_data_ptr->t3412_value          = EMM_DEFAULT_T3412;
  emm_ctrl_data_ptr->t3402_value = EMM_DEFAULT_T3402;
  emm_ctrl_data_ptr->t3402_attach_rej_param.is_t3402_recv_attach_rej = FALSE;

  /* As per the spec Table 10.2.1: EPS mobility management timers - UE side, the default value of t3423 is the same as t3412 and as per the spec t3412 is configured by the operator/NW. So, is it right to set t3412 to 0? */
  emm_ctrl_data_ptr->t3423_value           = emm_ctrl_data_ptr->t3412_value;

  emm_stop_all_timers (emm_ctrl_data_ptr);
  emm_ctrl_data_ptr->access_barring_flag   = 0;
#ifdef FEATURE_NAS_EAB
  emm_ctrl_data_ptr->eab_barring = FALSE;
#endif
  emm_ctrl_data_ptr->reg_init_rrc_trans_id = 0;
  emm_ctrl_data_ptr->nas_data_request_transaction_id         = 0;
  if(emm_ctrl_data_ptr->emm_tai_list_ptr != NULL)
  {
    emm_ctrl_data_ptr->emm_tai_list_ptr->tai_lst_length             = 0;
  }
  emm_ctrl_data_ptr->emm_attach_attempt_counter = 0;
  emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter = 0;
  emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
  emm_ctrl_data_ptr->eps_bearer_context_status_updated  = 0;
  memset((void *)(&emm_ctrl_data_ptr->eps_bearer_context_status), 0x0, sizeof(lte_nas_eps_bearer_context_status_type) );
  emm_ctrl_data_ptr->emm_ongoing_eps_ta_update  = 
                                            (lte_nas_emm_eps_update_val_type)0;
  emm_ctrl_data_ptr->emm_detach_attempt_counter = 0;
  emm_ctrl_data_ptr->ongoing_detach_type  = INVALID_DETACH_TYPE;
  emm_ctrl_data_ptr->ongoing_detach_procedure = 0 ; 
  emm_ctrl_data_ptr->update_eplmn_list = FALSE; 
  emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
  if(reset_ftai_list == TRUE)
  {
    if(emm_ctrl_data_ptr->rrc_forbidden_list_ptr != NULL)
    {
      emm_ctrl_data_ptr->rrc_forbidden_list_ptr->tai_lst_length = 0 ;
      emm_clear_rpm_ftai_list(FALSE,emm_ctrl_data_ptr);
    }
    if(emm_ctrl_data_ptr->forbidden_for_service_list_ptr != NULL)
    {
      emm_ctrl_data_ptr->forbidden_for_service_list_ptr->tai_lst_length = 0 ;
    }
    emm_clear_manual_ftai_list();
  }
  if(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL)
  {
    memset((void *)(emm_ctrl_data_ptr->last_rrc_service_ind_ptr),0x0,sizeof(lte_rrc_service_ind_s) );
  }
  emm_ctrl_data_ptr->last_rrc_service_req_trans_id = 0;

  if(emm_ctrl_data_ptr->pending_reg_cmd_ptr != NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Pending REG cmd is not released");
    emm_modem_mem_free(emm_ctrl_data_ptr->pending_reg_cmd_ptr,
                       emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->pending_reg_cmd_ptr = NULL;
  }

  emm_ctrl_data_ptr->reg_req_pended_indication  = 0;

#ifdef FEATURE_CIOT
  emm_ctrl_data_ptr->attach_without_pdn = FALSE;
  emm_ctrl_data_ptr->conn_suspended_rat = SYS_RAT_NONE;
  emm_ctrl_data_ptr->is_conn_suspended = FALSE;
#endif
  if(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL)
  {
    memset( (void *)(emm_ctrl_data_ptr->reg_req_pending_ptr), 0x0, sizeof(mmr_reg_req_s_type) );            
  }
     
  emm_ctrl_data_ptr->last_reg_cause = MMR_CAUSE_NONE;         
     
  emm_ctrl_data_ptr->service_req_cause = LTE_RRC_EST_CAUSE_MAX;          
  emm_ctrl_data_ptr->esm_srv_req_trans_id = EMM_INVALID_ESM_SRV_REQ_TRANS_ID; 

  emm_ctrl_data_ptr->phy_cell_id = 0;

  emm_ctrl_data_ptr->freq = 0;  

  emm_ctrl_data_ptr->cell_identity = 0;

  emm_ctrl_data_ptr->ota_msg_size = 0; 
  memset((void *)&emm_ctrl_data_ptr->last_rrc_deactivate_cnf,
         0x0,
         sizeof(lte_rrc_deactivate_cnf_s));

  //(void)pthread_mutex_unlock(emm_ctrl_data_ptr->emm_mutex_ptr);
  // Do not set the mutex to NULL
  //emm_ctrl_data_ptr->emm_mutex_ptr = NULL;  
  if(emm_ctrl_data_ptr->pdn_conn_req_ptr != NULL)
  {
    memset((void *)(emm_ctrl_data_ptr->pdn_conn_req_ptr),
           0x0, 
           sizeof(lte_nas_esm_pdn_connectivity_req) ); 
  }

  /* Reset nonce ue */
  memset((void *)emm_ctrl_data_ptr->nonce_ue, 
         0x0, 
         sizeof(emm_ctrl_data_ptr->nonce_ue));

  emm_ctrl_data_ptr->mt_detach_info.type = INVALID_MT_DETACH_TYPE;
  emm_ctrl_data_ptr->mt_detach_info.emm_cause = LTE_NAS_NO_FAILURE;

  emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
  emm_ctrl_data_ptr->last_tau_type = TAU_CAUSE_INVALID;  
  emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE; 
   
  /*reset the local epsloci*/
  memset( &emm_ctrl_data_ptr->last_known_epsloci[0], 0x00, EPSLOCI_LENGTH );

#ifdef FEATURE_ENHANCED_NW_SELECTION
  emm_ctrl_data_ptr->refresh_pending = FALSE;
#endif 
#ifdef FEATURE_DUAL_SIM
  emm_ctrl_data_ptr->tau_trm_priority = FALSE;
#endif
  /* Reset last received RRC active EPS bearer update ind */
  memset((void *)&emm_ctrl_data_ptr->last_rrc_bearer_ind, 0x0, 
         sizeof(lte_rrc_active_eps_bearer_update_ind_s));
#ifdef FEATURE_DUAL_SIM
 *emm_ctrl_data_ptr->dds_switch_pended_ptr  = FALSE;
#endif
emm_ctrl_data_ptr->emm_ignore_tau_req = FALSE;
emm_ctrl_data_ptr->is_periodic_tau_required = FALSE;
}/* emm_reset_ctrl_data */
 
/*===========================================================================
FUNCTION    EMM_SEND_IDLE_INFO_TO_GSTK

DESCRIPTION
  This function sends NO_SVC Indication to GSTK.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Sent IDLE_INFO info sent to GSTK
  FALSE: Could not send IDLE_INFO to GSTK

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_idle_info_to_gstk
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
#ifdef FEATURE_GSTK 
  gstk_io_send_mm_idle_evt_type  emm_idle_evt;
  uint8                          dummy;

#ifdef FEATURE_NAS_GW
  boolean is_full_rat_mode =     FALSE;

  if((mmcoord_pending_reg_message.cmd.mmr_reg_req.message_header.message_set == MS_MM_REG) && 
      (mmcoord_pending_reg_message.cmd.mmr_reg_req.message_header.message_id == MMR_REG_REQ) &&
      (mmcoord_pending_reg_message.cmd.mmr_reg_req.type == MMR_REG_REQ_FULL_RAT_MODE))
  {
     is_full_rat_mode = TRUE;
  }

   MSG_HIGH_DS_2(MM_SUB,"=EMM=  is_full_rat_mode = %d, mm_follow_on_request = %d",is_full_rat_mode, mm_follow_on_request);
  /* For FULL RAT mode, don't send IDLE event unless there is an EM call */
  if((mm_is_multimode_sub() == FALSE) || ((is_full_rat_mode == TRUE) && (mm_follow_on_request == FALSE)))
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub or full rat mode, return from function");
    return;
  }
#endif
  
  /* Init */
  memset((void *)&emm_idle_evt, 0x0, sizeof(gstk_io_send_mm_idle_evt_type));

  /* Fill in the structure values for "gstk_io_send_mm_idle_evt_type"*/  
  emm_idle_evt.location_state = 
                         emm_ctrl_data_ptr->plmn_service_state->service_status;

  emm_idle_evt.rat = (uint8)SYS_RAT_LTE_RADIO_ACCESS;

  memscpy((void*)&emm_idle_evt.mcc_mnc[0], 
          sizeof(emm_idle_evt.mcc_mnc), 
          (void*)&emm_ctrl_data_ptr->plmn_service_state->plmn.identity[0], 
          sizeof(sys_plmn_id_s_type));  
  memscpy((void*)&emm_idle_evt.cell_id.cell_id[0],
          sizeof(emm_idle_evt.cell_id.cell_id),
          (void*)&emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.cell_identity, 
          sizeof(lte_rrc_global_cell_id_t));

  emm_idle_evt.cell_id.cell_len = sizeof(lte_rrc_global_cell_id_t);
#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
  emm_idle_evt.as_id = (sys_modem_as_id_e_type)mm_sub_id;
  emm_idle_evt.stack_id = mm_set_cm_stack_id(mm_as_id);
#else
  emm_idle_evt.as_id = 0;
  emm_idle_evt.stack_id = 0;
#endif
  memscpy((void*)&emm_idle_evt.lac[0], 
          sizeof(emm_idle_evt.lac),
          (void *)&emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac, 
          sizeof(uint16));
  emm_idle_evt.as_id = 0;
  /*Send EMM_IDLE info to GSTK*/  
  if(gstk_io_ctrl(GSTK_IO_SEND_MM_IDLE_EVENT, &emm_idle_evt, &dummy) != GSTK_SUCCESS)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= could not send IDLE_INFO to GSTK");
  }
#endif /*#ifdef FEATURE_GSTK*/
  MSG_HIGH_DS_0(MM_SUB, "=EMM= sent IDLE_INFO to GSTK");
}

/*===========================================================================
FUNCTION    EMM_SEND_FULL_SVC_TO_GSTK

DESCRIPTION
  This function sends FULL_SVC Indication to GSTK.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Successully sent FULL_SVC_IND to GSTK
  FALSE: Failed to send FULL_SVC_IND to GSTK

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_full_svc_to_gstk
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  #ifdef FEATURE_GSTK 
  gstk_io_send_emm_registered_evt_type  emm_registered_evt;
  uint8                                 dummy;
  sys_plmn_id_s_type                    plmn;
  gstk_status_enum_type  error_number = GSTK_SUCCESS;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  if(mm_is_multimode_sub() == FALSE)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function");
    return;
  }
#endif

  /* Init */
  memset((void *)&emm_registered_evt, 
         0x0, 
         sizeof(gstk_io_send_emm_registered_evt_type));

  emm_registered_evt.location_state = 
                 (uint8)SYS_SRV_STATUS_SRV;
  emm_registered_evt.rat = (uint8)SYS_RAT_LTE_RADIO_ACCESS;

  plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
      emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  memscpy((void*)&emm_registered_evt.mcc_mnc[0], 
          sizeof(emm_registered_evt.mcc_mnc),
          (void *)&plmn, 
          sizeof(sys_plmn_id_s_type));

  memscpy((void*)&emm_registered_evt.cell_id.cell_id[0],
          sizeof(emm_registered_evt.cell_id.cell_id),  
          (void*)&emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.cell_identity, 
          sizeof(lte_rrc_global_cell_id_t));

  emm_registered_evt.cell_id.cell_len = sizeof(lte_rrc_global_cell_id_t);

#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
  emm_registered_evt.as_id = (sys_modem_as_id_e_type)mm_sub_id;

#else
  emm_registered_evt.as_id = 0;
#endif

  memscpy((void*)&emm_registered_evt.lac[0], 
          sizeof(emm_registered_evt.lac),
          (void*)&emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac, 
          sizeof(uint16));
  error_number = gstk_io_ctrl(GSTK_IO_SEND_EMM_REGISTERED_EVENT,&emm_registered_evt,&dummy);
  MSG_HIGH_DS_1(MM_SUB, "=EMM= sending FULL_SERV_IND to GSTK Success(0)/Failure(non zero) = (%d)",error_number);
#endif /*FEATURE_GSTK*/

}
/*===========================================================================
FUNCTION    EMM_SEND_LIMITED_SVC_TO_GSTK

DESCRIPTION
  This function sends LIMITED_SVC Indication to GSTK.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Successully sent LIMITED_SVC_IND to GSTK
  FALSE: Failed to send LIMITED_SVC_IND to GSTK

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_limited_svc_to_gstk
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  #ifdef FEATURE_GSTK 
  gstk_io_send_emm_registered_evt_type  emm_registered_evt;
  uint8                                 dummy;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  if(mm_is_multimode_sub() == FALSE)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function");
    return;
  }
#endif

  /* Init */
  memset((void *)&emm_registered_evt, 
         0x0, 
         sizeof(gstk_io_send_emm_registered_evt_type));

  emm_registered_evt.location_state = 
                 (uint8)SYS_SRV_STATUS_LIMITED;
  emm_registered_evt.rat = (uint8)SYS_RAT_LTE_RADIO_ACCESS;

  memscpy((void*)&emm_registered_evt.mcc_mnc[0],
          sizeof(emm_registered_evt.mcc_mnc),  
          (void*)&emm_ctrl_data_ptr->plmn_service_state->plmn.identity[0], 
          sizeof(sys_plmn_id_s_type));

  memscpy((void*)&emm_registered_evt.cell_id.cell_id[0], 
          sizeof(emm_registered_evt.cell_id.cell_id),
          (void*)&emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.cell_identity, 
          sizeof(lte_rrc_global_cell_id_t));

  emm_registered_evt.cell_id.cell_len = sizeof(lte_rrc_global_cell_id_t);

#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
  emm_registered_evt.as_id = (sys_modem_as_id_e_type)mm_sub_id;
  emm_registered_evt.stack_id = mm_set_cm_stack_id(mm_as_id);
#else
  emm_registered_evt.as_id = 0;
  emm_registered_evt.stack_id = 0;
#endif


  memscpy((void*)&emm_registered_evt.lac[0], 
          sizeof(emm_registered_evt.lac),
          (void*)&emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac, 
          sizeof(uint16));

  if(gstk_io_ctrl(GSTK_IO_SEND_MM_IDLE_EVENT,&emm_registered_evt,&dummy)
       == GSTK_SUCCESS)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= sent LIMITED_SERV_IND to GSTK");
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Failed to send LIMITED_SERV_INDto GSTK");
  }
#endif /*FEATURE_GSTK*/

}

/*===========================================================================
FUNCTION    EMM_SEND_NO_SVC_TO_GSTK

DESCRIPTION
  This function sends NO_SVC Indication to GSTK.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Successully sent EMM_NO_SVC_IND to GSTK
  FALSE: Failed to send EMM_NO_SVC_IND to GSTK

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_no_svc_to_gstk
(
  void
)
{
#ifdef FEATURE_GSTK 
  gstk_io_send_mm_idle_evt_type  emm_idle_evt;
  uint8                          dummy;

  /* Init */
  memset((void *)&emm_idle_evt, 0x0, sizeof(gstk_io_send_mm_idle_evt_type));

  /* Fill in the structure values for "gstk_io_send_mm_idle_evt_type"*/  
  emm_idle_evt.location_state =(uint8)SYS_SRV_STATUS_NO_SRV;
  emm_idle_evt.rat = (uint8)SYS_RAT_LTE_RADIO_ACCESS;

  emm_idle_evt.as_id = 0;

#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
  emm_idle_evt.as_id = (sys_modem_as_id_e_type)mm_sub_id;
  emm_idle_evt.stack_id = mm_set_cm_stack_id(mm_as_id);
#else
  emm_idle_evt.as_id = 0;
  emm_idle_evt.stack_id = 0;
#endif


  /*Send EMM_NO_SVC Indication to GSTK*/  
  if(gstk_io_ctrl(GSTK_IO_SEND_MM_IDLE_EVENT, &emm_idle_evt, &dummy) != GSTK_SUCCESS)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= could not send NO_SERV_IND to GSTK");
  }
#endif /*#ifdef FEATURE_GSTK*/
  MSG_HIGH_DS_0(MM_SUB, "=EMM= sent NO_SERV_IND to GSTK");
}

/*===========================================================================
FUNCTION    EMM_SEND_REGISTERED_EVT_TO_GSTK

DESCRIPTION
  This function sends GSTK EMM REGISTERED EVENT to GSTK.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_registered_evt_to_gstk
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
#ifdef FEATURE_GSTK 
  gstk_io_send_emm_registered_evt_type  emm_registered_evt;
  uint8                                 dummy;
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  if(mm_is_multimode_sub() == FALSE)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function");
    return;
  }
#endif

  /* Init */
  memset((void *)&emm_registered_evt, 
         0x0, 
         sizeof(gstk_io_send_emm_registered_evt_type));

  emm_registered_evt.location_state = 
                 emm_ctrl_data_ptr->plmn_service_state->service_status;

  emm_registered_evt.rat = (uint8)SYS_RAT_LTE_RADIO_ACCESS;

  memscpy((void*)&emm_registered_evt.mcc_mnc[0],
          sizeof(emm_registered_evt.mcc_mnc),  
          (void*)&emm_ctrl_data_ptr->plmn_service_state->plmn.identity[0], 
          sizeof(sys_plmn_id_s_type));

  memscpy((void*)&emm_registered_evt.cell_id.cell_id[0], 
          sizeof(emm_registered_evt.cell_id.cell_id),
          (void*)&emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.cell_identity, 
          sizeof(lte_rrc_global_cell_id_t));

  emm_registered_evt.cell_id.cell_len = sizeof(lte_rrc_global_cell_id_t);

#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
  emm_registered_evt.as_id = (sys_modem_as_id_e_type)mm_sub_id;
  emm_registered_evt.stack_id = mm_set_cm_stack_id(mm_as_id);
#else
  emm_registered_evt.as_id = 0;
  emm_registered_evt.stack_id = 0;
#endif


  memscpy((void*)&emm_registered_evt.lac[0],
          sizeof(emm_registered_evt.lac),  
          (void*)&emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac, 
          sizeof(uint16));

  if(gstk_io_ctrl(GSTK_IO_SEND_EMM_REGISTERED_EVENT,&emm_registered_evt,&dummy)
       == GSTK_SUCCESS)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= sent REGISTERED EVT to GSTK");
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Failed to send REGISTERED EVT to GSTK");
  }
#endif /*FEATURE_GSTK*/
  
}/* emm_send_registered_evt_to_gstk */

#ifdef FEATURE_GSTK 
/*===========================================================================
FUNCTION    EMM_SEND_NW_REJ_EVT_TO_GSTK

DESCRIPTION
  This function sends NW REJ EVENT to GSTK.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_nw_rej_evt_to_gstk
(
  gstk_nw_rej_enum_type   nw_rej_type,
  lte_nas_emm_cause_type  emm_cause,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  gstk_io_send_nw_rej_evt_type       nw_rej_evt;
  uint8                              dummy;
  gstk_update_attach_type_enum_type  gstk_update_type 
                                                 = GSTK_UPDATE_ATTACH_TYPE_MAX;
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  if(mm_is_multimode_sub() == FALSE)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function");
    return;
  }
#endif
  

  switch(nw_rej_type)
  {
    case GSTK_EMM_ATTACH_REJ:
      switch(emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain)
      {
        case SYS_SRV_DOMAIN_PS_ONLY:
          gstk_update_type = GSTK_EPS_ATTACH;
          break;
        case SYS_SRV_DOMAIN_CS_PS:
          gstk_update_type = GSTK_COMBINED_EPS_IMSI_ATTACH;
          break;
        default:
          MSG_ERROR_DS_1(MM_SUB,"=EMM= Invalid SRV domain to map to GSTK update type - %u",
                         emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain);
          break;
      }
      break;
    case GSTK_TRACKING_AREA_UPDATE_REJ:
      switch(emm_ctrl_data_ptr->emm_ongoing_eps_ta_update)
      {
        case TA_UPDATING:
          gstk_update_type = GSTK_EMM_TA_UPDATE;
          break;
        case COMBINED_TA_LA_UPDATING:
          gstk_update_type = GSTK_COMBINED_TA_LA_UPDATE;
          break;
        case COMBINED_TA_LA_UPDATING_IMSI_ATTACH:
          gstk_update_type = GSTK_COMBINED_TA_LA_UPDATE_IMSI_ATTACH;
          break;
        case PERIODIC_UPDATING:
          gstk_update_type = GSTK_PERIODIC_TA_UPDATE;
          break;
        default:
          MSG_ERROR_DS_1(MM_SUB,"=EMM= Invalid TAU type to map to GSTK update type - %u",
                         emm_ctrl_data_ptr->emm_ongoing_eps_ta_update);
          break;
      }
      break;
    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM=Invalid NW REJ type for GSTK - %u", nw_rej_type);
      break;
  }

  /* Init */
  memset((void *)&nw_rej_evt, 0x0, sizeof(gstk_io_send_nw_rej_evt_type));

  nw_rej_evt.nw_rej_type = nw_rej_type;

#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
  nw_rej_evt.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#else
  nw_rej_evt.as_id = 0;
#endif

  memscpy((void*)&nw_rej_evt.mcc_mnc[0], 
          sizeof(nw_rej_evt.mcc_mnc),
          (void*)&emm_ctrl_data_ptr->plmn_service_state->plmn.identity[0],
          sizeof(sys_plmn_id_s_type));

  memscpy((void*)&nw_rej_evt.lac[0],
          sizeof(nw_rej_evt.lac),  
          (void*)&emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac, 
          sizeof(uint16));

  nw_rej_evt.rat = (uint8)SYS_RAT_LTE_RADIO_ACCESS;

  nw_rej_evt.upd_attach_type = gstk_update_type;

  nw_rej_evt.rej_cause = (uint8)emm_cause;
#ifdef FEATURE_GSTK
  if(gstk_io_ctrl(GSTK_IO_SEND_NW_REJ_EVENT, &nw_rej_evt, &dummy)
       == GSTK_SUCCESS)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= sent NW REJ EVT to GSTK");
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Failed to send NW REJ EVT to GSTK");
  }
#endif /*FEATURE_GSTK*/
}/* emm_send_nw_rej_evt_to_gstk */

#endif

/*===========================================================================
FUNCTION    EMM_CONN_ABORT_MARK_PENDED_MESSAGE

DESCRIPTION
  This function marks all existing pended messages to be processed upon 
  receiving release indication with cause of aborted 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_conn_abort_mark_pended_message
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  emm_client_list_type      *client = NULL ;
  emm_pended_msg_list_type  *message = NULL ;

  MSG_LOW_DS_0(MM_SUB,"=EMM= Mark pended msg to be processed upon reving conn abort rel ind");

  /* Traverse the messages from all clients and set the flag */
  if (emm_ctrl_data_ptr->client_list != NULL)
  {
    client = emm_ctrl_data_ptr->client_list ;

    while (client != NULL)
    {
      if (client->next_message != NULL)
      {
        message = client->next_message ;

        while (message != NULL)
        {
          message->process_upon_conn_abort = TRUE;
          message = message->next_message ;
        }
      }
      /* All existing messages are marked to be processed/deleted in current 
         client. Need to reset connection set up message pointer. */
      client->connection_setup_message = NULL;
      client = client->next_client ;
    }
  }
} /* end of emm_conn_abort_mark_pended_message */

/*===========================================================================

FUNCTION  EMM_SEND_EMM_STATUS

DESCRIPTION
  This function sends EMM_STATUS message w/ the given emm cause

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_emm_status
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr,
  lte_nas_emm_cause_type   emm_cause
)
{
  lte_nas_emm_status                 *mo_emm_status_ptr;
  dword                               sequence_number = 0xFFFFFFFF;

  
  mm_check_for_null_ptr((void *)emm_ctrl_data_ptr->out_msg_ptr);

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Send EMM STATUS message w/ cause - %u", emm_cause);

  /* Init */
  memset((void *)emm_ctrl_data_ptr->out_msg_ptr, 
         0x0, 
         sizeof(lte_nas_outgoing_msg_type));

  mo_emm_status_ptr = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.mo_emm_status;

  mo_emm_status_ptr->lte_nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES;
  mo_emm_status_ptr->lte_nas_hdr.msg_id = EMM_STATUS;
  mo_emm_status_ptr->security_hdr = PLAIN_NAS_MSG;
  mo_emm_status_ptr->emm_cause = emm_cause;

  /* Obtain sequence number */
  sequence_number = emm_get_nas_transaction_id() ;

  if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number, 
                                MSGR_NAS_EMM,
                                NULL, 
                                NULL, 
                                emm_ctrl_data_ptr))
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Cannot send EMM STATUS message");
  }

} /* emm_send_emm_status */

#ifdef FEATURE_CIOT
/*===========================================================================

FUNCTION  emm_write_dcn_list_to_efs

DESCRIPTION
  This function writes DCN list to EFS 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if EFS written successfully, else FALSE

SIDE EFFECTS
  None

===========================================================================*/

boolean emm_write_dcn_list_to_efs(emm_ctrl_data_type      *emm_ctrl_data_ptr)
{
  mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;

  efs_status = mcfg_fs_write("/nv/item_files/modem/nas/dcn_list",
                             &emm_ctrl_data_ptr->dcn_list,
                             sizeof(lte_nas_dcn_lst_type),
                             MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR,
                             MCFG_FS_ALLPERMS,
                             MCFG_FS_TYPE_EFS,
                             (mcfg_fs_sub_id_e_type)mm_sub_id);

  if( efs_status != MCFG_FS_STATUS_OK )
  {
    MSG_ERROR_DS_1(MM_SUB, "EMM :Write to EFS RPLMN TYPE file failed, status - %d", efs_status);
    return FALSE;
  }
  
  return TRUE;
}

/*===========================================================================

FUNCTION  emm_invalidate_dcn_list

DESCRIPTION
  This function invalidated the DCN list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void emm_invalidate_dcn_list
(
  emm_ctrl_data_type		*emm_ctrl_data_ptr
)
{
  uint8 i = 0;
  MSG_HIGH_DS_0(MM_SUB, "=EMM= invalidate DCN LIST");
  for(i=0;i<LTE_NAS_MAX_DCN_IDS;i++)
  {
	sys_plmn_undefine_plmn_id(&emm_ctrl_data_ptr->dcn_list.dcn_list[i].plmn);
  }
}
#endif
/*===========================================================================

FUNCTION  emm_write_rplmn_type_to_efs

DESCRIPTION
  This function writes RPLMN TYPE to EFS 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if EFS written successfully, else FALSE

SIDE EFFECTS
  None

===========================================================================*/

boolean emm_write_rplmn_type_to_efs(mm_rplmn_type_enum_T rplmn_type)
{
  mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;

  efs_status = mcfg_fs_write("/nv/item_files/modem/nas/mm_rplmn_type",
                             &rplmn_type,
                             sizeof(mm_rplmn_type_enum_T),
                             MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR,
                             MCFG_FS_ALLPERMS,
                             MCFG_FS_TYPE_EFS,
                             (mcfg_fs_sub_id_e_type)mm_sub_id);

  if( efs_status != MCFG_FS_STATUS_OK )
  {
    MSG_ERROR_DS_1(MM_SUB, "EMM :Write to EFS RPLMN TYPE file failed, status - %d", efs_status);
    return FALSE;
  }
  
  if(mm_psm_cache_ptr != NULL)
  {
    REG_SET_BM_32BIT(mm_psm_cache_ptr->psm_nv_list_bitmask, MM_PSM_EFS_MM_RPLMN_TYPE);
  }
  return TRUE;
}


/*===========================================================================

FUNCTION  iemm_write_tin_to_efs

DESCRIPTION
  This function writes TIN to EFS 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_write_tin_to_efs(emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;

  

  efs_status = mcfg_fs_write("/nvm/alpha/modem/nas/lte_nas_TIN_item",
                             &emm_ctrl_data_ptr->TIN, sizeof(nas_mm_tin_type),
                             MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR,
                             MCFG_FS_ALLPERMS,
                             MCFG_FS_TYPE_EFS,
                             (mcfg_fs_sub_id_e_type)mm_sub_id);

  if( efs_status != MCFG_FS_STATUS_OK )
  {
    MSG_ERROR_DS_1(MM_SUB, "EMM :Write to EFS TIN file failed, status - %d", efs_status);
  }
}

#ifdef FEATURE_3G_USER_NW_REDIR_TO_LTE   
/*===========================================================================
 
FUNCTION  emm_write_lte_accepted_status_to_efs
 
DESCRIPTION
  This function writes LTE status to EFS
 
DEPENDENCIES
  None
 
RETURN VALUE
  None
 
SIDE EFFECTS
  None
 
===========================================================================*/
 
void emm_write_lte_accepted_status_to_efs(emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;
 
  
  efs_status = mcfg_fs_write("/nv/item_files/modem/nas/is_accepted_on_lte",
                             &emm_ctrl_data_ptr->is_accepted_on_lte,
                             sizeof(boolean),
                             MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR,
                             MCFG_FS_ALLPERMS,
                             MCFG_FS_TYPE_EFS,
                             (mcfg_fs_sub_id_e_type)mm_sub_id);
  if( efs_status != MCFG_FS_STATUS_OK )
  {
    MSG_ERROR_DS_1(MM_SUB, "EMM :Write to EFS  file failed status - %d", efs_status);
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Write to EFS file succeeded ");
  }
}
#endif 
/*===========================================================================

FUNCTION  emm_set_and_write_tin_type

DESCRIPTION
  This function sets the TIN type and writes the TIN to EFS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_set_and_write_tin_type(nas_mm_tin_type tin)
{

  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  

  /* If TIN is changed for RAT RELATED TMSI to PTMSI or GUTI send
  ** ISR off indication to ESM or SM based on present active RAT.
  */
 #if defined(FEATURE_NAS_GW) && defined(FEATURE_NAS_SM_IRAT)
  if( emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
  {
    if(tin != NAS_MM_TIN_RAT_RELATED_TMSI)
    {
      if(is_lte_mode())
      {
        emm_build_and_send_isr_status_change_ind(FALSE);
      }
      else
      {
        gmm_send_sm_isr_status_ind(FALSE);
      }
    }
  }
  /* If TIN is changed for PTMSI or GUTI to RAT RELATED TMSI send
  ** ISR ON indication to ESM or SM based on present active RAT.
  */
  else if(tin == NAS_MM_TIN_RAT_RELATED_TMSI)
  {
    if(is_lte_mode())
    {
      emm_build_and_send_isr_status_change_ind(TRUE);
    }
    else
    {
      gmm_send_sm_isr_status_ind(TRUE);
    }
  }
 #endif
  if(emm_ctrl_data_ptr->TIN != tin)
  {
    MSG_HIGH_DS_2(MM_SUB, "=EMM= Set TIN from %d to %d", emm_ctrl_data_ptr->TIN, tin);
    emm_ctrl_data_ptr->TIN = tin;
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

    if(emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)
#endif
#endif
    {
      emm_write_tin_to_efs(emm_ctrl_data_ptr);
    }
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= TIN %d is same as in efs. do not write", tin);
  }
}/* emm_set_and_write_tin_type */

/*===========================================================================
FUNCTION EMM_UPDATE_NW_SELECTION_MODE

DESCRIPTION
  This function checks the network selection mode in last received REG REQ and
  update the network selection mode if needed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_update_nw_selection_mode
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  sys_network_selection_mode_e_type  nw_sel_mode = 
                                               SYS_NETWORK_SELECTION_MODE_NONE;

  
  

  switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
  {
    case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
      nw_sel_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
      break;
    case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
      nw_sel_mode = SYS_NETWORK_SELECTION_MODE_MANUAL;
      break;
    default:
      break;
  }

  if(nw_sel_mode != SYS_NETWORK_SELECTION_MODE_NONE)
  {
    emm_send_rrc_nw_sel_mode_reset_req(nw_sel_mode, NULL, emm_ctrl_data_ptr);
    mm_emm_store_reg_req_nw_selection_mode(nw_sel_mode);
  } 
    
} /* emm_update_nw_selection_mode() */

/*=========================================================================== 
 
FUNCTION  EMM_SEND_TAU_COMPLETE_IND

DESCRIPTION
  This function sends NAS_EMM_TAU_COMPLETE_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_send_tau_complete_ind(void)
{
  emm_tau_complete_ind_type  tau_complete_ind;
  emm_tau_complete_ind_s_type  tau_complete_ind_s;

  emm_ctrl_data_type  *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending NAS_EMM_TAU_COMPLETE_IND");
    /* Init */
  memset((void *)&tau_complete_ind, 0x0, sizeof(emm_tau_complete_ind_type)); 
  memset((void *)&tau_complete_ind_s, 0x0, sizeof(emm_tau_complete_ind_s_type)); 
  tau_complete_ind.message_header.message_set = MS_MM_DS;
  tau_complete_ind.message_header.message_id = (int) EMM_TAU_COMPLETE_IND;
  msgr_init_hdr((msgr_hdr_s *)&tau_complete_ind_s.msg_hdr,
                MSGR_NAS_EMM, 
                NAS_EMM_TAU_COMPLETE_IND); 

#ifdef FEATURE_CIOT
  if(emm_ctrl_data_ptr->hc_config_status_exists == TRUE)
  {
    tau_complete_ind.hc_config_status = tau_complete_ind_s.hc_config_status = emm_ctrl_data_ptr->hc_config_status;
    tau_complete_ind.hc_config_status_exists = tau_complete_ind_s.hc_config_status_exists = TRUE;
  }
#endif
  if(emm_ctrl_data_ptr->last_tau_type == TAU_CAUSE_TA_CHANGE)
  {
    tau_complete_ind.tau_type = tau_complete_ind_s.tau_type = NAS_3GPP_TAU_CHANGE_TYPE;
  }
  else
  {
    tau_complete_ind.tau_type = tau_complete_ind_s.tau_type = NAS_3GPP_OTHER_TYPE;
  }
  
  emm_ctrl_data_ptr->last_tau_type = TAU_CAUSE_INVALID;
  tau_complete_ind.plmn = tau_complete_ind_s.plmn = emm_ctrl_data_ptr->plmn_service_state->plmn;
  tau_complete_ind.tac = tau_complete_ind_s.tac =  emm_ctrl_data_ptr->plmn_service_state->tac;

  PUT_IMH_LEN(sizeof( emm_tau_complete_ind_type) - sizeof(IMH_T),
                &tau_complete_ind.message_header );

  /* Add sent messages to the debug buffer */
  mm_add_message_to_debug_buffer(tau_complete_ind.message_header.message_set,
                                 tau_complete_ind.message_header.message_id,
                                 FROM_MM
                                 ,(sys_modem_as_id_e_type)mm_as_id
                                 );
	 
  ds_pdn_mgr_nas_to_ds_msg_hdlr((void *)&tau_complete_ind);
  
  emm_msgr_send((msgr_hdr_s *)&tau_complete_ind_s,sizeof(emm_tau_complete_ind_s_type));
} /* emm_send_tau_complete_ind */

/*===========================================================================

FUNCTION    EMM_TAU_COMPLETE_SUCCEEDED

DESCRIPTION
  This function is called upon successful TAU COMPLETE message delivery

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void emm_tau_complete_succeeded
(
  dword  trans_id
)
{
  emm_ctrl_data_type  *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  

  if ((emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == COMBINED_TA_LA_UPDATING ||
          emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == COMBINED_TA_LA_UPDATING_IMSI_ATTACH) &&
       (emm_ctrl_data_ptr->srv_domain_registered == SYS_SRV_DOMAIN_PS_ONLY)) 
  {
    emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_ACCEPT_PS_ONLY, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
  }
  else if(emm_ctrl_data_ptr->srv_domain_registered == SYS_SRV_DOMAIN_CS_PS || emm_ctrl_data_ptr->srv_domain_registered == SYS_SRV_DOMAIN_PS_ONLY)
  {
    emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_ACCEPT, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
  }
#ifdef FEATURE_NAS_GW
  if(emm_ctrl_data_ptr->tau_pending_gw_attach == TRUE)
  {
    emm_ctrl_data_ptr->tau_pending_gw_attach = FALSE;  
  }
#endif
  if(emm_ctrl_data_ptr->radio_cap_update != EMM_RADIO_CAP_UPDATE_NONE)
  {
    emm_ctrl_data_ptr->radio_cap_update = EMM_RADIO_CAP_UPDATE_NONE;  
  }

  if((emm_ctrl_data_ptr->sg_association_to_be_established != NAS_EMM_SG_ASSOC_NOT_REQ)
     &&
     (emm_ctrl_data_ptr->srv_domain_registered == SYS_SRV_DOMAIN_CS_PS))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Reset SGs assoc flag - Combined TAU COMPLETE");
    emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_NOT_REQ;
  }

  if(*emm_ctrl_data_ptr->psm_set_cfg_status_ptr == NAS_PSM_CFG_REQ_PROCESSING)
  {
    *emm_ctrl_data_ptr->psm_set_cfg_status_ptr = NAS_PSM_CFG_REQ_NONE;
  }

  if(emm_ctrl_data_ptr->edrx_set_cfg_req_status == NAS_EDRX_CFG_REQ_PROCESSING)
  {
    emm_ctrl_data_ptr->edrx_set_cfg_req_status = NAS_EDRX_CFG_REQ_NONE;
  }

#ifdef FEATURE_NAS_GW
  /* Update GW flag(s) */          
  emm_attach_tau_success_update_gw();
  emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;
  emm_set_tau_trm_priority(FALSE);
#endif


  if(mm_timer_status[TIMER_T3440] == TIMER_STOPPED)
  {
    /* Send out all pended messages */
    emm_send_all_pended_messages(emm_ctrl_data_ptr); 
#ifdef FEATURE_LTE_TO_1X
    /* Send out all pending IRAT uplink messages */
    emm_send_all_pending_irat_msg(emm_ctrl_data_ptr);
#endif
#if defined(FEATURE_LTE_TO_1X) ||( defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
    if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED) || 
       (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED))
    {
      emm_send_esr_if_buffered(emm_ctrl_data_ptr);
    }
#endif

    if(((*emm_ctrl_data_ptr->psm_set_cfg_status_ptr == NAS_PSM_CFG_REQ_PENDING) ||
       (emm_ctrl_data_ptr->edrx_set_cfg_req_status == NAS_EDRX_CFG_REQ_PENDING)||
       (emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_OTHERS))
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
     && (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
     )
    {
      MSG_HIGH_DS_2(MM_SUB, "=EMM= PSM(%d)/EDRX(%d)/ (TIMER 3411 has alread expired) Set cfg params req pending, Start TAU procedure ",
                            *emm_ctrl_data_ptr->psm_set_cfg_status_ptr,
                            emm_ctrl_data_ptr->edrx_set_cfg_req_status);
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      emm_initiate_tau_procedure(EMM_INITIAL_UPDATE, 
                                 emm_get_eps_update_type(emm_ctrl_data_ptr), 
                                 FALSE, 
                                 emm_ctrl_data_ptr);
    }
  }
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
  else if(((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED) || 
            (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED))
           &&
           (emm_ctrl_data_ptr->pended_esr_msg->emm_outgoing_msg.ext_service_req.service_type 
              == LTE_MO_CSFB_ECALL_1XCSFB_ECALL))
  {
    emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,
                             LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
  }
#endif
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

  else if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING)
  {
    emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,
                             LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
  }
#endif
#endif

  emm_send_tau_complete_ind();

  if(emm_ctrl_data_ptr->reg_complete_ack_pending != EMM_PENDING_REG_ACK_NONE)
  {
    emm_ctrl_data_ptr->reg_complete_ack_pending = EMM_PENDING_REG_ACK_NONE;
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
    if(emm_ctrl_data_ptr->is_plmn_block_req_pending == TRUE)
    {
      emm_process_block_plmn_req(&emm_ctrl_data_ptr->pending_block_plmn_req, emm_ctrl_data_ptr);
    }
  }
}

/*===========================================================================

FUNCTION EMM_GET_MO_DETACH_TYPE

DESCRIPTION
  This function returns MO detach type according to current service domain

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
emm_mo_detach_type  emm_get_mo_detach_type
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_mo_detach_type  detach_type = INVALID_DETACH_TYPE;

  
  

  switch(emm_ctrl_data_ptr->srv_domain_registered)
  {
    case SYS_SRV_DOMAIN_CS_PS:
      detach_type = COMBINED_EPS_DETACH;
      break;
    case SYS_SRV_DOMAIN_PS_ONLY:
      detach_type = EPS_DETACH;
      break;
    default:
      switch(emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain)
      { 
        case SYS_SRV_DOMAIN_CS_PS:
          detach_type = COMBINED_EPS_DETACH;
          break;
        case SYS_SRV_DOMAIN_PS_ONLY:
          detach_type = EPS_DETACH;
          break;
        default:
          MSG_FATAL_DS(MM_SUB,"=EMM= Invalid REG REQ srv domain %d to set detach type",
               emm_ctrl_data_ptr->plmn_service_state->reg_service_domain,
               0,0);
          break;
      }
      break;
  }

  MSG_HIGH_DS_3(MM_SUB,"=EMM= Detach type %d, srv domain reged %d, reg req srv domain %d",
              detach_type,
              emm_ctrl_data_ptr->srv_domain_registered, 
              emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain);

  return detach_type;
    
} /* emm_get_mo_detach_type() */

/*===========================================================================

FUNCTION  EMM_IS_COMBINED_REG_ALLOWED

DESCRIPTION
  This function checks if combined registration is allowed with LTE RAT
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE: Combined registration IS allowed 
  FALSE: Combined registration IS NOT allowed 

SIDE EFFECTS
  None

===========================================================================*/
boolean  emm_is_combined_reg_allowed
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type  sys_plmn;
  boolean return_value = TRUE;

  sys_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  if(emm_search_plmn_list(&sys_plmn, 
                          &emm_ctrl_data_ptr->combined_reg_fplmn_list) != 0xFF)
  {
    return_value = FALSE;
  }

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Is combined reg allowed on srving PLMN (1/0) ? %d",return_value);
  return return_value;

} /* emm_is_combined_reg_allowed */

/*===========================================================================

FUNCTION EMM_GET_EPS_UPDATE_TYPE

DESCRIPTION
  This function returns TAU type according to current service domain


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
lte_nas_emm_eps_update_val_type  emm_get_eps_update_type
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  lte_nas_emm_eps_update_val_type  eps_update_type = PERIODIC_UPDATING;

  
  

  switch(emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain)
  {
    case SYS_SRV_DOMAIN_PS_ONLY:
      eps_update_type = TA_UPDATING;
      if(emm_ctrl_data_ptr->sg_association_to_be_established != NAS_EMM_SG_ASSOC_NOT_REQ)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Reset SGs assoc flag - PS only mode");
        emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_NOT_REQ; 
      }
      break;
    case SYS_SRV_DOMAIN_CS_PS:
      MSG_HIGH_DS_3(MM_SUB,"=EMM= Sim state %d,sg assoc to be ested %d,reged srv domain %d",
                    emm_ctrl_data_ptr->plmn_service_state->sim_state,
                    emm_ctrl_data_ptr->sg_association_to_be_established,
                    emm_ctrl_data_ptr->srv_domain_registered);
      
      if((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                       SYS_SIM_STATE_AVAILABLE)
         &&
         (emm_is_combined_reg_allowed(emm_ctrl_data_ptr) == TRUE)
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

         &&
         (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)
#endif
#endif
#ifdef FEATURE_CIOT
         &&( !(mm_checkif_rat_pri_list_has_only_rat(SYS_RAT_LTE_NB1_RADIO_ACCESS)) ||
		 (emm_ctrl_data_ptr->force_req_ser_domain ==  FALSE))
#endif
        )
      {
        if((emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC)
#ifdef FEATURE_NAS_GW
           ||                
           (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_DUE_TO_SRVCC)
#endif
          )
        {
          eps_update_type = COMBINED_TA_LA_UPDATING_IMSI_ATTACH;
        }
        else if(emm_ctrl_data_ptr->srv_domain_registered == 
                                                        SYS_SRV_DOMAIN_PS_ONLY)
        {
          eps_update_type = COMBINED_TA_LA_UPDATING_IMSI_ATTACH;
        }
        else
        {
          eps_update_type = COMBINED_TA_LA_UPDATING;
        }
      }
      else
      {
        /* If USIM is marked as CS invalid or serving PLMN is not allowed for
           combined registration or UE is attached for emergency service only, 
           set TAU to TA updating. */
        eps_update_type = TA_UPDATING;
        if(emm_ctrl_data_ptr->sg_association_to_be_established != NAS_EMM_SG_ASSOC_NOT_REQ)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Reset SGs assoc flag - CS not allowed");
          emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_NOT_REQ; 
        }
      }
      break;
    default:
      MSG_FATAL_DS(MM_SUB,"=EMM= Invalid srv domain to set update type",0,0,0);
      break;
  }

  MSG_HIGH_DS_2(MM_SUB,"=EMM= EPS update type %d w/ REQ REQ srv domain %d", 
                eps_update_type, 
                emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain);
   
  return eps_update_type;
      
} /* emm_get_eps_update_type() */

/*===========================================================================

FUNCTION EMM_UPDATE_SRV_DOMAIN_MO_DETACH

DESCRIPTION
  This function updates service domain info upon MO detach procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_update_srv_domain_mo_detach
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  sys_srv_domain_e_type  srv_domain = SYS_SRV_DOMAIN_NO_SRV;

  

  if(emm_ctrl_data_ptr->plmn_service_state->reg_service_domain ==
                                                          SYS_SRV_DOMAIN_CS_PS)
  {
    switch(emm_ctrl_data_ptr->ongoing_detach_type)
    {
      case EPS_DETACH:
        srv_domain = SYS_SRV_DOMAIN_CS_ONLY;
        break;
      case IMSI_DETACH_MO_DETACH:
        srv_domain = SYS_SRV_DOMAIN_PS_ONLY;
        break;
      default:
        break;
    }
  }

  MSG_MED_DS_3(MM_SUB,"=EMM= Update service domain from %u to %u on MO detach type -%u",
               emm_ctrl_data_ptr->plmn_service_state->reg_service_domain,
               srv_domain,
               emm_ctrl_data_ptr->ongoing_detach_type);

  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = srv_domain;
  emm_ctrl_data_ptr->srv_domain_registered = srv_domain;
    
} /* emm_update_srv_domain_mo_detach() */

/*===========================================================================

FUNCTION  EMM_IS_PLMN_FORBIDDEN

DESCRIPTION
  This function checks if a given PLMN belongs to any list of 
  - Forbidden PLMNs list 
  - Forbidden PLMNs for GPRS service list
  - Temporary forbidden PLMNs list 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: PLMN is in forbidden lists
  FALSE: PLMN is not in forbidden lists

SIDE EFFECTS
  None

===========================================================================*/
boolean  emm_is_plmn_forbidden
(
  sys_plmn_id_s_type   sys_plmn,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  boolean  plmn_id_is_undefined;
  boolean  mnc_includes_pcs_digit;
  uint32   mcc;
  uint32   mnc;

  
  if(
#ifdef FEATURE_DUAL_SIM
     (reg_sim_per_subs_plmn_forbidden(sys_plmn,(sys_modem_as_id_e_type)mm_sub_id) == TRUE) ||
     (reg_sim_plmn_gprs_forbidden(sys_plmn,(sys_modem_as_id_e_type)mm_sub_id) == TRUE) ||
#else
     (reg_sim_plmn_forbidden(sys_plmn) == TRUE) ||
     (reg_sim_plmn_gprs_forbidden(sys_plmn) == TRUE) ||
#endif
     (reg_sim_plmn_backoff_forbidden(sys_plmn,(sys_modem_as_id_e_type)mm_sub_id) == TRUE))
  {
    sys_plmn_get_mcc_mnc(sys_plmn,
                         &plmn_id_is_undefined,
                         &mnc_includes_pcs_digit,
                         &mcc,
                         &mnc);
    MSG_HIGH_DS_2(MM_SUB, "=EMM= PLMN %d-%d is forbidden", mcc, mnc);
    return TRUE;
  }

  return FALSE;

} /* emm_is_plmn_forbidden */

/*===========================================================================

FUNCTION  EMM_IS_TAC_FORBIDDEN

DESCRIPTION
  This function validates whether the received RRC_SERVICE_IND 
  consists of TAC that is a part of FORBIDDEN_LIST

DEPENDENCIES
  None

RETURN VALUE
  TRUE: SERVICE_IND does not contain TAC that is part of FORBIDDEN LISTS
  FALSE: SERVICE_IND contains TAC that is part of FORBIDDEN LISTS

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_search_tai_list2
(
  lte_nas_tai_lst1_type   tai_to_be_searched,  
  lte_nas_tai_lst2_type   *tai_list
)
{
  byte                  list_index  = 0;      

  for (list_index=0; list_index < tai_list->tai_lst_length; list_index++)
  {
    /* Check whether extracted PLMN matches */
    if ((sys_plmn_match((tai_list->diff_plmn[list_index].plmn), tai_to_be_searched.plmn) == TRUE)&&
        (tai_list->diff_plmn[list_index].tac == tai_to_be_searched.tac))
    {
      /* TAI is found in the TAI_LIST */
      return TRUE;
    }       

  }  
  return FALSE;
  
}
/*===========================================================================

FUNCTION  EMM_IS_TAI_FORBIDDEN

DESCRIPTION
  This function will check if the requested TAI is present in the forbiden tai list

DEPENDENCIES
  None

RETURN VALUE
  TRUE: TAI is forbidden
  FALSE: TAI is not forbidden

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_is_tai_forbidden
(
  lte_nas_tai_lst1_type tai,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  boolean  plmn_id_is_undefined;
  boolean  mnc_includes_pcs_digit;
  uint32   mcc;
  uint32   mnc;

  
  
  if((emm_search_tai_list2(tai,emm_ctrl_data_ptr->forbidden_for_service_list_ptr) == TRUE)||
     (emm_search_tai_list2(tai,emm_ctrl_data_ptr->rrc_forbidden_list_ptr) == TRUE) ||
     (emm_is_tai_forbidden_for_manual(tai, emm_ctrl_data_ptr) == TRUE))
  {
    sys_plmn_get_mcc_mnc(tai.plmn,
                         &plmn_id_is_undefined,
                         &mnc_includes_pcs_digit,
                         &mcc,
                         &mnc);

    MSG_HIGH_DS_3(MM_SUB, "=EMM= TA is forbidden - PLMN %d-%d, TAC %d", mcc, mnc, tai.tac);
    return TRUE; 
  } 
  return FALSE;   
}

/*===========================================================================

FUNCTION  EMM_HANDLE_TEMPORARY_FORBIDDEN_PLMN

DESCRIPTION
  This function adds current PLMN to temporary forbidden PLMN list upon the 
  starting of T3402 if backoff time is set

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_handle_temporary_forbidden_plmn
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  dword  backoff_time = 0;

  

  switch(emm_ctrl_data_ptr->temp_fplmn_backoff_time)
  {
    case EMM_TEMP_FPLMN_DISABLED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Temporary forbidden PLMN feature is disabled");
      return;
    case EMM_TEMP_FPLMN_USE_T3402:
      backoff_time = emm_ctrl_data_ptr->t3402_value;
      break;
    default:
      backoff_time = emm_ctrl_data_ptr->temp_fplmn_backoff_time;
      break;
  }

  MSG_HIGH_DS_1(MM_SUB,"=EMM= Adding curr PLMN to temp FPLMN list w/ backoff time %d (ms)",
                backoff_time);

  emm_add_plmn_to_backoff_fplmn_list(emm_ctrl_data_ptr->last_attempted_tai.plmn, (uint32)backoff_time, SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE);    

} /* emm_handle_temporary_forbidden_plmn */

/*===========================================================================

FUNCTION  EMM_REMOVE_FORBIDDEN_PLMN

DESCRIPTION
  This function removes PLMN from following lists
  - Forbidden PLMN list
  - Forbidden PLMN for GPRS service list
  - Temporary forbidden PLMN list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_remove_forbidden_plmn
(
  sys_plmn_id_s_type   sys_plmn,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  

  /* Delete PLMN from the forbidden PLMN list */
  mm_remove_plmn_from_fplmn_list(sys_plmn);    


  mm_remove_plmn_from_gprs_fplmn_list(sys_plmn);

  if(emm_ctrl_data_ptr->temp_fplmn_backoff_time != EMM_TEMP_FPLMN_DISABLED)
    
  {
    /* Delete PLMN from the temp forbidden PLMNs list */
    emm_remove_plmn_from_backoff_fplmn_list(sys_plmn);
  }

} /* emm_remove_forbidden_plmn */

/*===========================================================================

FUNCTION  EMM_SET_NONCE_UE

DESCRIPTION
  This function generates a 32 bits random number and sets the nonce ue 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_set_nonce_ue
(
  byte                *nonce_ue_ptr,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  dword rand_value = 0;  

  rand_value = ((mm_get_random_value(RAND_MAX) << 16) | 
                mm_get_random_value(RAND_MAX));

  /* Index 0 for MSB, index 3 for LSB */
  nonce_ue_ptr[0] = (byte)((rand_value & 0xFF000000) >> 24);
  nonce_ue_ptr[1] = (byte)((rand_value & 0x00FF0000) >> 16);
  nonce_ue_ptr[2] = (byte)((rand_value & 0x0000FF00) >> 8);
  nonce_ue_ptr[3] = (byte)(rand_value & 0x000000FF);

  MSG_MED_DS_1(MM_SUB, "=EMM= Set nonce ue - %x", rand_value);

  /* Save the nonce in EMM data base */
  memscpy((void*)&emm_ctrl_data_ptr->nonce_ue[0], 
          sizeof(emm_ctrl_data_ptr->nonce_ue),
          (void *)nonce_ue_ptr, 
          sizeof(emm_ctrl_data_ptr->nonce_ue));

} /* emm_set_nonce_ue */

/*===========================================================================

FUNCTION  EMM_GET_TAU_REQ_MME_ROUTING_INFO

DESCRIPTION
  This function returns MME routing info to be used in connection setup request
  for TAU REQUEST message

DEPENDENCIES
  None

RETURN VALUE
  MME Routing Info type

SIDE EFFECTS
  None

===========================================================================*/
lte_nas_conn_req_routing_info  emm_get_tau_req_mme_routing_info
(
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  lte_nas_conn_req_routing_info  routing_info = ROUTING_INFO_NONE;

  

  switch(emm_ctrl_data_ptr->tau_cause)
  {
    case TAU_CAUSE_TA_CHANGE:
      routing_info = ROUTING_INFO_GUMMEI;
      break;
    case TAU_CAUSE_LOAD_BALANCING:
      routing_info = ROUTING_INFO_NONE;
      break;
    default:
#ifdef FEATURE_NAS_GW
      if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI)
      {
        routing_info = ROUTING_INFO_GUMMEI;  
      }
      else
#endif
      {          
        routing_info = ROUTING_INFO_STMSI;
      }
      break;
  }

  MSG_LOW_DS_1(MM_SUB,"=EMM= Using MME routing info %d in conn est req for TAU REQ",
               routing_info);

  return routing_info;

} /* emm_get_tau_req_mme_routing_info */

/*===========================================================================

FUNCTION    EMM_CHK_TA_CHANGE

DESCRIPTION
  This function checks if serving TAI is changed from the last attempted TAI
 
DEPENDENCIES
  None

RETURN VALUE
  FALSE - If TA has not changed
  TRUE  - If TA has changed

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_chk_ta_change
( 
  lte_rrc_service_ind_s  *rrc_service_ind,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  boolean  result = FALSE;
  lte_nas_tai_lst1_type  current_tai;
 
   
   

  current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  current_tai.tac  = rrc_service_ind->camped_svc_info.tac;

  /* Last attempted TAI belongs to TAI list */
  if(emm_search_tai_list(&emm_ctrl_data_ptr->last_attempted_tai,
                         emm_ctrl_data_ptr->emm_tai_list_ptr) == TRUE)
  {
    if(emm_search_tai_list(&current_tai,
                           emm_ctrl_data_ptr->emm_tai_list_ptr) == FALSE)
    {
      /* Serving TAI does not belong to TAI list. Report TA cahnge. */
      result = TRUE;  
    }
  }
  /* Last attempted TAI does not belong to TAI list */
  else
  {
    if((sys_plmn_match(current_tai.plmn,
                       emm_ctrl_data_ptr->last_attempted_tai.plmn) != TRUE)
       || 
       (current_tai.tac != emm_ctrl_data_ptr->last_attempted_tai.tac))
    {
      /* Serving TAI is different from last attempted TAI. Report TA cahnge. */
      result = TRUE;
    }
  }

  MSG_HIGH_DS_1(MM_SUB,"=EMM= TAI change - %d (Srving TAI vs Last attempted TAI)",
                result);

  return result;

}/* end of emm_chk_ta_change */

#ifdef FEATURE_LTE_REL10
/*===========================================================================
FUNCTION    EMM_SAVE_T3346_PLMN

DESCRIPTION
  LAST attempted plmn is stored in the EMM_CTRL_DATA
  shared between GWLT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_save_t3346_plmn
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type last_attempted_plmn;

  /* Copy the current plmn global last_attempted_plmn  */ 
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
  {
    last_attempted_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

    memscpy((void*)emm_ctrl_data_ptr->t3346_plmn_ptr,
            sizeof(sys_plmn_id_s_type),
            (void*)&last_attempted_plmn,
            sizeof(sys_plmn_id_s_type));
  }

}

#endif

/*===========================================================================

FUNCTION    EMM_SAVE_LAST_ATTEMPTED_TAI

DESCRIPTION
LAST attempted TAI is stored in the EMM_CTRL_DATA
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_save_last_attempted_tai
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  
  if(emm_ctrl_data_ptr->last_rrc_service_ind_ptr == NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function emm_save_last_attempted_tai");
    return;
  }
  /* Copy the current TAI info (last_rrc_service_ind)
     into the global emm_ctrl_data_ptr->last_attempted_tai     
  */ 

  emm_ctrl_data_ptr->last_attempted_tai.plmn = 
    emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  emm_ctrl_data_ptr->last_attempted_tai.tac = 
                   emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac; 

}/* end of emm_save_last_attempted_tai */

/*===========================================================================

FUNCTION  EMM_RAT_CHANGE_STOP_TIMERS

DESCRIPTION
  This function stops EMM timers except ISR timer when UE is switching to
  another RAT
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_rat_change_stop_timers(void)
{

  emm_ctrl_data_type  *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  
  

  emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
  if (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode != SYS_NETWORK_SELECTION_MODE_LIMITED)
  {
    mm_stop_timer(TIMER_T3402);
    mm_stop_timer(TIMER_T3411);
  }
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE || defined FEATURE_CATM_LTE_EXT_GNSS_COEX || defined FEATURE_WWAN_GNSS_DYNAMIC
  mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

} /* emm_rat_change_stop_timers */

/*===========================================================================

FUNCTION  EMM_SEND_REG_REG_IND

DESCRIPTION
  This function sends REG REJ IND upon ATTACH/TAU reject.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_reg_rej_ind
(
  lte_nas_emm_cause_type  emm_cause,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  sys_srv_domain_e_type  srv_domain = SYS_SRV_DOMAIN_NONE;

  
  if(emm_ctrl_data_ptr->reg_req_pending_ptr == NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function emm_send_reg_rej_ind");
    return;
  }

  switch(emm_cause)
  {
    case LTE_NAS_ILLEGAL_UE:
    case LTE_NAS_ILLEGAL_ME:
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
    case LTE_NAS_SEVERE_NW_FAILURE:
    case LTE_NAS_CONGESTION:
      if(emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)
      {
        srv_domain = SYS_SRV_DOMAIN_CS_PS;
      }
      else
      {
        srv_domain = SYS_SRV_DOMAIN_PS_ONLY;
      }
      break;
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
    case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW:
    case LTE_NAS_IMPLICITLY_DETACHED:
      srv_domain = SYS_SRV_DOMAIN_PS_ONLY;
      break;
    case LTE_NAS_IMSI_UNKNOWN_IN_HSS:
    case LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE:
    case LTE_NAS_CS_DOMAIN_NOT_AVAILABLE:
      srv_domain = SYS_SRV_DOMAIN_CS_ONLY;
      break;
    default:
      break;
  }

  if(srv_domain != SYS_SRV_DOMAIN_NONE)
  {
    mm_send_mmr_reg_reject_ind(
          srv_domain,
          (byte)emm_cause,
          emm_ctrl_data_ptr->plmn_service_state->plmn,
          emm_ctrl_data_ptr->plmn_service_state->tac
  #if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
         ,emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id
  #endif 
        ,emm_ctrl_data_ptr->plmn_service_state->active_rat
         );
  }

}/* emm_send_reg_rej_ind */

/*=========================================================================== 
 
FUNCTION    EMM_CLEAN_MSG_BUFFER

DESCRIPTION
  This function cleans message buffer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_clean_msg_buffer
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_client_list_type       *curr_client = NULL;
  emm_pended_msg_list_type   *curr_msg = NULL;

   

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Cleaning msg buffer");

  while(emm_ctrl_data_ptr->client_list != NULL)
  {
    curr_client = emm_ctrl_data_ptr->client_list;

    /* Remove all messages */
    while(curr_client->next_message != NULL)
    {
      curr_msg = curr_client->next_message;

      MSG_LOW_DS_3(MM_SUB,"=EMM= Removing msg %d w/ trans id 0x%x from client %d",
                   curr_msg->nas_out_message.nas_hdr.msg_id,
                   curr_msg->sequence_number,
                   curr_client->client);

      curr_client->next_message = curr_msg->next_message;
      emm_modem_mem_free(curr_msg, emm_ctrl_data_ptr);
    }

    /* Remove client */
    MSG_LOW_DS_1(MM_SUB, "=EMM= Removing client %d", curr_client->client);
    emm_ctrl_data_ptr->client_list = curr_client->next_client;
    emm_modem_mem_free(curr_client,emm_ctrl_data_ptr);
  }

}/* emm_clean_msg_buffer */

/*=========================================================================== 
 
FUNCTION  EMM_LOCAL_SWITCHOFF_DETACH

DESCRIPTION 
  This function performs local switch off detach

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_local_switchoff_detach
(
  sys_stop_mode_reason_e_type   stop_mode_reason,
  emm_ctrl_data_type           *emm_ctrl_data_ptr 
)
{
  

  MSG_HIGH_DS_2(MM_SUB,"=EMM= Local switch off detaching - srv status %d, srv domain %d",
                emm_ctrl_data_ptr->plmn_service_state->service_status,
                emm_ctrl_data_ptr->plmn_service_state->reg_service_domain);

  /* Notify ESM */
  if(stop_mode_reason == SYS_STOP_MODE_REASON_PSM)
  {
    emm_build_and_send_detach_ind(EMM_DETACHED_PSM);
  }
  else
  {
    emm_build_and_send_detach_ind(EMM_DETACHED);
  }

  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_NO_SRV;
  emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;

  if(emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                         SYS_SRV_STATUS_NO_SRV)
  {
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
  }
  else
  {
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);
  }

  if(emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == FALSE)
  {
    /* Send RRC deactivate request */
    emm_init_rrc_deact_req(stop_mode_reason, emm_ctrl_data_ptr);
  }
  else if(emm_ctrl_data_ptr->rrc_deact_req.deact_reason != stop_mode_reason)
  {
    /* Stop mode collided with L to GW RAT change deactivation. If stop mode 
       cause is different from rat change deact cause, cache the deact req to 
       deactivate RRC one more time with new cause */    
    emm_ctrl_data_ptr->rrc_deact_req.deact_reason = stop_mode_reason;
    emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = TRUE;    
  }

}/* emm_local_switchoff_detach */


/*===========================================================================

FUNCTION EMM_ADD_COMBINED_REG_FORBIDDEN_PLMN

DESCRIPTION
  This function adds the given PLMN to the forbidden PLMN list for combined
  registration

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_add_combined_reg_forbidden_plmn
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr 
)
{
  sys_plmn_id_s_type  sys_plmn;
  byte               *list_len_ptr = NULL;
  byte                len_to_move = 0;
  boolean             plmn_id_is_undefined;
  boolean             mnc_includes_pcs_digit;
  uint32              mcc;
  uint32              mnc;

  sys_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  sys_plmn_get_mcc_mnc(sys_plmn,
                       &plmn_id_is_undefined,
                       &mnc_includes_pcs_digit,
                       &mcc,
                       &mnc);

  MSG_HIGH_DS_2(MM_SUB, "=EMM= Adding PLMN %d-%d to FPLMN list for combined reg", mcc, mnc);

  if(emm_search_plmn_list(&sys_plmn, 
                          &emm_ctrl_data_ptr->combined_reg_fplmn_list) == 0xFF)
  {
    list_len_ptr = 
       &emm_ctrl_data_ptr->combined_reg_fplmn_list.length_of_plmn_lst_contents;

    nas_assert_check((boolean)(*list_len_ptr <= MAX_NO_OF_PLMNs));

    MSG_HIGH_DS_1(MM_SUB,"=EMM= FPLMN list for combined reg - old length %d", 
                  *list_len_ptr);

    /* Determine length is to be moved */
    if(*list_len_ptr == MAX_NO_OF_PLMNs)
    {
      len_to_move = *list_len_ptr - 1;
    }
    else
    {
      len_to_move = *list_len_ptr;
    }

    if(len_to_move > 0)
    {
      /* Move the "old" list entries */
      memsmove((void*)&emm_ctrl_data_ptr->combined_reg_fplmn_list.plmn[1],
               sizeof(emm_ctrl_data_ptr->combined_reg_fplmn_list.plmn) - 
                     sizeof(sys_plmn_id_s_type),
               (void*)&emm_ctrl_data_ptr->combined_reg_fplmn_list.plmn[0],
               len_to_move * sizeof(sys_plmn_id_s_type));
    }

    /* Add new PLMN to the list */
    emm_ctrl_data_ptr->combined_reg_fplmn_list.plmn[0] = sys_plmn;

    /* Increase length of the list if list is not full yet */
    if(*list_len_ptr < MAX_NO_OF_PLMNs)
    {
      (*list_len_ptr) ++;
    }

    MSG_HIGH_DS_1(MM_SUB,"=EMM= FPLMN list for combined reg - new length %d", 
                  *list_len_ptr);
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= PLMN existing in FPLMN list for combined reg");
  }

} /* emm_add_combined_reg_forbidden_plmn */

/*=========================================================================== 
 
FUNCTION  EMM_SEND_PLMN_CHANGE_IND

DESCRIPTION
  This function sends NAS_EMM_PLMN_CHANGE_IND w/ new PLMN information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_send_plmn_change_ind
(
  sys_plmn_id_s_type  sys_plmn
)
{
  boolean                   plmn_id_is_undefined;
  boolean                   mnc_includes_pcs_digit;
  uint32                    mcc;
  uint32                    mnc;
  emm_plmn_change_ind_type  plmn_chg_ind;

  /* Init */
  memset((void *)&plmn_chg_ind, 0x0, sizeof(emm_plmn_change_ind_type));
  
  sys_plmn_get_mcc_mnc(sys_plmn,
                       &plmn_id_is_undefined,
                       &mnc_includes_pcs_digit,
                       &mcc,
                       &mnc);

  MSG_HIGH_DS_2(MM_SUB, "=EMM= Sending NAS_EMM_PLMN_CHANGE_IND w/ PLMN %d-%d", mcc, mnc);
  
  plmn_chg_ind.message_header.message_set = MS_MM_DS;
  plmn_chg_ind.message_header.message_id = (int) EMM_PLMN_CHANGE_IND;
  
  plmn_chg_ind.plmn = sys_plmn;
  PUT_IMH_LEN(sizeof( emm_plmn_change_ind_type ) - sizeof(IMH_T),
                &plmn_chg_ind.message_header );
  
  /* Add sent messages to the debug buffer */
  mm_add_message_to_debug_buffer(plmn_chg_ind.message_header.message_set,
                                 plmn_chg_ind.message_header.message_id,
                                 FROM_MM
                                 ,(sys_modem_as_id_e_type)mm_as_id
                                 );

  ds_pdn_mgr_nas_to_ds_msg_hdlr((void *)&plmn_chg_ind);
  
} /* emm_send_plmn_change_ind */

/*=========================================================================== 
 
FUNCTION  EMM_SEND_RESET_APN_SWITCH_IND

DESCRIPTION
  This function sends NAS_EMM_RESET_APN_SWITCH_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_send_reset_apn_switch_ind
(
  void
)
{
  emm_reset_apn_switch_ind_type  reset_apn_switch_ind;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending NAS_EMM_RESET_APN_SWITCH_IND");

  /* Init */
  memset((void *)&reset_apn_switch_ind, 0x0, sizeof(emm_reset_apn_switch_ind_type));

  reset_apn_switch_ind.message_header.message_set = MS_MM_DS;
  reset_apn_switch_ind.message_header.message_id = (int) EMM_RESET_APN_SWITCH_IND;
  
  PUT_IMH_LEN(sizeof( emm_reset_apn_switch_ind_type) - sizeof(IMH_T),
                &reset_apn_switch_ind.message_header );
  
  ds_pdn_mgr_nas_to_ds_msg_hdlr((void *)&reset_apn_switch_ind);

} /* emm_reset_apn_switch_ind */

/*=========================================================================== 
 
FUNCTION  EMM_RELEASE_ATTACH_PDN_CONN_REQ

DESCRIPTION
  This function reset PDN CONN REQ transaction ID and notify ESM to terminate
  the pending PDN CONN REQ procedure for attach

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_release_attach_pdn_conn_req
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr,
  emm_failure_type     emm_failure_cause
)
{

  if(emm_ctrl_data_ptr->pdn_conn_req_trans_id != 
                                             EMM_INVALID_PDN_CONN_REQ_TRANS_ID)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Releasing attach pdn conn req resource");

    /* Notify ESM/DS */  
    emm_build_and_send_failure_ind(EMM_ATTACH_FAILED, 
                                   LTE_NAS_NO_FAILURE,
                                   emm_ctrl_data_ptr->pdn_conn_req_trans_id,
                                   emm_failure_cause);
      
    emm_ctrl_data_ptr->pdn_conn_req_trans_id = 
                                             EMM_INVALID_PDN_CONN_REQ_TRANS_ID;
  }
} /* emm_release_attach_pdn_conn_req */

/*=========================================================================== 
 
FUNCTION  EMM_GET_CURRENT_TAI

DESCRIPTION
  This function get the current TAI

DEPENDENCIES
  None

RETURN VALUE
  Serving TAI

SIDE EFFECTS
  None
 
===========================================================================*/
lte_nas_tai_lst1_type  emm_get_current_tai
(
  lte_rrc_service_ind_s  *rrc_service_ind
)
{
  lte_nas_tai_lst1_type  curr_tai;
  memset((void *)&curr_tai,0,sizeof(lte_nas_tai_lst1_type));

  curr_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                       rrc_service_ind->camped_svc_info.selected_plmn);
  curr_tai.tac = rrc_service_ind->camped_svc_info.tac;

  return curr_tai;

} /* emm_get_current_tai */

/*=========================================================================== 
 
FUNCTION  EMM_IS_MANUAL_REQ_PENDING

DESCRIPTION
  This function checks if a manual service request is still pending

DEPENDENCIES
  None

RETURN VALUE
  Serving TAI

SIDE EFFECTS
  None
 
===========================================================================*/
boolean  emm_is_manual_req_pending
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  boolean  manual_req_pending = FALSE;

  
  

  if(((emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == 
                                          SYS_NETWORK_SELECTION_MODE_MANUAL) ||
      (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == 
                        SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
     &&
     (reg_service_report == (emm_reg_service_report_type)emm_send_reg_cnf))
  {
    manual_req_pending = TRUE;
  }

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Manual REQ pending status %d", manual_req_pending);

  return manual_req_pending;

} /* emm_is_manual_req_pending */

/*=========================================================================== 
 
FUNCTION  EMM_IS_MANUAL_REG_ALLOWED

DESCRIPTION
  This function checks if a registration in manual mode is allowed

DEPENDENCIES
  None

RETURN VALUE
  Serving TAI

SIDE EFFECTS
  None
 
===========================================================================*/
boolean  emm_is_manual_reg_allowed
(
  lte_nas_tai_lst1_type  srving_tai,  
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  boolean  reg_allowed = FALSE;
  
  
  

  /* Registration in manual mode is allowed only when all following conditions
     are matched
     1. Serving PLMN must match the requested PLMN or EPLMN if Limited manual
     2. Serving TAI is not in FTAI list for manual selection and CSG is in white list 
     3. Manual selection flag is on
        or 
        Manual selection flag is off and serving PLMN and TAI are not in any 
        forbidden lists 
     4. If not manual CSG selection then CSG should be in white list
            or
        if manual csg selection then ignore forbidden tai and fplmn
    */
  if(((sys_plmn_match(srving_tai.plmn, 
                      emm_ctrl_data_ptr->reg_req_pending_ptr->plmn) == TRUE) ||
       ((emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode != SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION) ||
      ((sys_eplmn_list_equivalent_plmn(emm_ctrl_data_ptr->reg_req_pending_ptr->plmn) == TRUE) &&
        (sys_eplmn_list_equivalent_plmn(srving_tai.plmn) == TRUE))))
       &&
     ((((emm_is_tai_forbidden_for_manual(srving_tai, emm_ctrl_data_ptr) == FALSE) 
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
      &&(mm_is_csg_in_white_list(srving_tai.plmn, &emm_ctrl_data_ptr->plmn_service_state->csg_info) == TRUE)
#endif 
       )
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
      || (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
#endif 
       ) &&
      (!reg_sim_plmn_backoff_forbidden(srving_tai.plmn,(sys_modem_as_id_e_type)mm_sub_id))))
  {
    if(emm_ctrl_data_ptr->manual_user_sel == TRUE)
    {
      reg_allowed = TRUE;
    }
    else if((emm_is_plmn_forbidden(srving_tai.plmn, emm_ctrl_data_ptr) == FALSE)
            &&
            (emm_is_tai_forbidden(srving_tai, emm_ctrl_data_ptr)== FALSE))
    {
      reg_allowed = TRUE;
    }
  }

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Manual registration allowance %d", reg_allowed);

  return reg_allowed;

} /* emm_is_manual_reg_allowed */

/*=========================================================================== 
 
FUNCTION  EMM_SEND_ATTACH_COMPLETE_IND

DESCRIPTION
  This function sends NAS_EMM_ATTACH_COMPLETE_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_send_attach_complete_ind(void)
{
  emm_attach_complete_ind_type  attach_complete_ind;
  
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sending NAS_EMM_ATTACH_COMPLETE_IND attach without pdn %d ",emm_ctrl_data_ptr->attach_without_pdn);

  /* Init */
  memset((void *)&attach_complete_ind, 0x0, sizeof(emm_attach_complete_ind_type));

  attach_complete_ind.message_header.message_set = MS_MM_DS;
  attach_complete_ind.message_header.message_id = (int) EMM_ATTACH_COMPLETE_IND;

#ifdef FEATURE_CIOT
  attach_complete_ind.attach_without_pdn =   emm_ctrl_data_ptr->attach_without_pdn;
#endif
  
  PUT_IMH_LEN(sizeof( emm_attach_complete_ind_type) - sizeof(IMH_T),
                &attach_complete_ind.message_header );

  /* Add sent messages to the debug buffer */
  mm_add_message_to_debug_buffer(attach_complete_ind.message_header.message_set,
                                 attach_complete_ind.message_header.message_id,
                                 FROM_MM
                                 ,(sys_modem_as_id_e_type)mm_as_id
                                 );

  ds_pdn_mgr_nas_to_ds_msg_hdlr((void *)&attach_complete_ind);

}
/*===========================================================================

FUNCTION    EMM_SEND_EMM_BARRING_STATUS_IND

DESCRIPTION
  This function is send barring timer start/stop indication for MO DATA/ MO EXCEPTION DATA 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void emm_send_emm_timer_barring_status_ind
(
emm_timer_barring_status_e_type status,
emm_timer_barring_status_category_e_type category
)
{ 	
	emm_timer_barring_status_ind_type emm_timer_barring_status_ind;
	MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending NAS_EMM_TIMER_BARRING_STATUS_IND");
	
	memset((void *)&emm_timer_barring_status_ind, 0x0, sizeof(emm_timer_barring_status_ind_type));
	
	 emm_timer_barring_status_ind.message_header.message_set = MS_MM_DS;
	 emm_timer_barring_status_ind.message_header.message_id = (int) EMM_TIMER_BARRING_STATUS_IND;

     PUT_IMH_LEN(sizeof( emm_timer_barring_status_ind) - sizeof(IMH_T),
                &emm_timer_barring_status_ind.message_header );	 

	emm_timer_barring_status_ind.barring_category = category;
	emm_timer_barring_status_ind.status = status;

     
	mm_add_message_to_debug_buffer(emm_timer_barring_status_ind.message_header.message_set,
									 emm_timer_barring_status_ind.message_header.message_id,
									 FROM_MM
									 ,(sys_modem_as_id_e_type)mm_as_id
									 );
	
	  ds_pdn_mgr_nas_to_ds_msg_hdlr((void *)&emm_timer_barring_status_ind);

}

/*=========================================================================== 
 
FUNCTION  EMM_SEND_CPSR_STATUS_IND

DESCRIPTION
  This function sends NAS_EMM_CPSR_STATUS_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_send_cpsr_status_ind(emm_cpsr_status status)
{
  emm_cpsr_status_ind_type emm_cpsr_status_ind;
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sending NAS_EMM_CPSR_STATUS_IND with status : %d", status);

  memset((void *)&emm_cpsr_status_ind, 0x0, sizeof(emm_cpsr_status_ind_type));

  emm_cpsr_status_ind.message_header.message_set = MS_MM_DS;
  emm_cpsr_status_ind.message_header.message_id = (int) EMM_CPSR_STATUS_IND;
  emm_cpsr_status_ind.status = status;

  PUT_IMH_LEN(sizeof( emm_cpsr_status_ind) - sizeof(IMH_T),
                &emm_cpsr_status_ind.message_header );	

  mm_add_message_to_debug_buffer(emm_cpsr_status_ind.message_header.message_set,
									 emm_cpsr_status_ind.message_header.message_id,
									 FROM_MM
									 ,(sys_modem_as_id_e_type)mm_as_id
									 );
	
	  ds_pdn_mgr_nas_to_ds_msg_hdlr((void *)&emm_cpsr_status_ind);			
  
}

/*===========================================================================

FUNCTION    EMM_ATTACH_COMPLETE_SUCCEEDED

DESCRIPTION
  This function is called upon successful ATTACH COMPLETE message delivery

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void emm_attach_complete_succeeded
(
  dword  trans_id
)
{
  emm_ctrl_data_type  *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  
  TTLBOOTUP(LTE_EMM_ATTACH_COMPLETE);

  if(EMM_GET_STATE() == EMM_REGISTERED  ||
     EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED ||
     EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED)
  {

    gmm_read_attach_complete_nv();
    emm_send_attach_complete_ind();
    if (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS ||
       ((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
       (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_PS_ONLY)))
    {
      emm_send_diag_event(EVENT_NAS_ATTACH, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_ACCEPT, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
  }
    else if(emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_CS_PS &&
               emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY)
    {
      // If requested service domain is CS + PS & ACCEPT includes onoly PS, send ACCEPT_PS_ONLY DIAG event
      emm_send_diag_event(EVENT_NAS_ATTACH, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_ACCEPT_PS_ONLY, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
    }
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB,"=EMM= L2 ack recv, not send ATTACH_COMPLETE_IND as emm_state %d",
                  EMM_GET_STATE()); 
  }

  if((EMM_GET_STATE() == EMM_REGISTERED) &&
     ((*emm_ctrl_data_ptr->psm_set_cfg_status_ptr == NAS_PSM_CFG_REQ_PENDING) 
      ||
     (emm_ctrl_data_ptr->edrx_set_cfg_req_status == NAS_EDRX_CFG_REQ_PENDING)
      ||
     (emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_OTHERS))
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
     && (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
     )
  {
    MSG_HIGH_DS_2(MM_SUB, "=EMM= PSM(%d)/EDRX(%d)/ (Timer3411 expired) Set cfg params req pending, Start TAU procedure ",
                          *emm_ctrl_data_ptr->psm_set_cfg_status_ptr,
                          emm_ctrl_data_ptr->edrx_set_cfg_req_status);
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
    emm_initiate_tau_procedure(EMM_INITIAL_UPDATE, 
                               emm_get_eps_update_type(emm_ctrl_data_ptr), 
                               FALSE, 
                               emm_ctrl_data_ptr);
  }

  if(emm_ctrl_data_ptr->reg_complete_ack_pending !=  EMM_PENDING_REG_ACK_NONE)
  {
    emm_ctrl_data_ptr->reg_complete_ack_pending = EMM_PENDING_REG_ACK_NONE;
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
    if(emm_ctrl_data_ptr->is_plmn_block_req_pending == TRUE)
    {
      emm_process_block_plmn_req(&emm_ctrl_data_ptr->pending_block_plmn_req, emm_ctrl_data_ptr);
    }
  }
} /* end of emm_attach_complete_succeeded() */

/*===========================================================================

FUNCTION  EMM_SET_RRC_EPLMN_LIST

DESCRIPTION
  This function populates the EPLMN list to be sent to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void  emm_set_rrc_eplmn_list
(
  lte_rrc_plmn_list_s  *rrc_eplmn_list_ptr,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  byte                  len_of_eplmn_list= 0;
  byte                  index = 0;         
  sys_plmn_id_s_type    rplmn;
  sys_plmn_list_s_type  eplmn_list;  

  rrc_eplmn_list_ptr->num_plmns = 0;
  

#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

  if((emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY) ||
     (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_EXIST))
  {
    MSG_HIGH_DS_2(MM_SUB,"=EMM= Populating RRC EPLMN list w/ EMM EPLMN list - %d, %d",
                  emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents,
                  emm_ctrl_data_ptr->last_registered_tai_valid);
    
    if((emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents > 0)
       &&
       (emm_ctrl_data_ptr->last_registered_tai_valid == TRUE))
    {
      rrc_eplmn_list_ptr->plmn[0] = emm_convert_nas_plmn_id_to_rrc_plmn_id(
                                   emm_ctrl_data_ptr->last_registered_tai.plmn);

      rrc_eplmn_list_ptr->num_plmns++;

      len_of_eplmn_list = MIN(emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents,
                                                       LTE_RRC_MAX_PLMN_LIST_SIZE);
      len_of_eplmn_list = MIN(len_of_eplmn_list,MAX_NO_OF_PLMNs);

      for(index = 0; index <len_of_eplmn_list; index ++)
      {
        rrc_eplmn_list_ptr->plmn[rrc_eplmn_list_ptr->num_plmns] = 
          emm_convert_nas_plmn_id_to_rrc_plmn_id(
            emm_ctrl_data_ptr->emm_equivalent_PLMN_list.plmn[index]);
        rrc_eplmn_list_ptr->num_plmns++;
      }
    }
  }
  else
 #endif
#endif
  if(emm_ctrl_data_ptr->update_eplmn_list == TRUE)
  {
    MSG_HIGH_DS_2(MM_SUB,"=EMM= Populating RRC EPLMN list w/ EMM EPLMN list - %d, %d",
                  emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents,
                  emm_ctrl_data_ptr->last_registered_tai_valid);

    if((emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents > 0)
       &&
       (emm_ctrl_data_ptr->last_registered_tai_valid == TRUE))
    {
      if(emm_is_plmn_forbidden(emm_ctrl_data_ptr->last_registered_tai.plmn, emm_ctrl_data_ptr) == FALSE)
      {
        rrc_eplmn_list_ptr->plmn[0] = emm_convert_nas_plmn_id_to_rrc_plmn_id(
                                     emm_ctrl_data_ptr->last_registered_tai.plmn);
  
        rrc_eplmn_list_ptr->num_plmns++;
      }

      len_of_eplmn_list = MIN(emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents,
                                                       LTE_RRC_MAX_PLMN_LIST_SIZE);
      len_of_eplmn_list = MIN(len_of_eplmn_list,MAX_NO_OF_PLMNs);

      for(index = 0; index <len_of_eplmn_list; index ++)
      {
        if(emm_is_plmn_forbidden(emm_ctrl_data_ptr->emm_equivalent_PLMN_list.plmn[index], emm_ctrl_data_ptr) == FALSE)
        {
          rrc_eplmn_list_ptr->plmn[rrc_eplmn_list_ptr->num_plmns] = 
            emm_convert_nas_plmn_id_to_rrc_plmn_id(
              emm_ctrl_data_ptr->emm_equivalent_PLMN_list.plmn[index]);
         rrc_eplmn_list_ptr->num_plmns++;
        }
      }
    }
  }
  else
  {
    /* Get EPLMN list */
    sys_eplmn_list_get(&eplmn_list, &rplmn);
  
    MSG_HIGH_DS_1(MM_SUB,"=EMM= Populated RRC EPLMN list w/ REG EPLMN list - Length %d",
                  eplmn_list.length);
   
    if(eplmn_list.length > 0)
    {
      /* Copy the rplmn to the first member */
      if(emm_is_plmn_forbidden(rplmn, emm_ctrl_data_ptr) == FALSE)
      {
        rrc_eplmn_list_ptr->plmn[0] = emm_convert_nas_plmn_id_to_rrc_plmn_id(rplmn);
        rrc_eplmn_list_ptr->num_plmns++;
      }
      
      len_of_eplmn_list = (byte)(MIN(eplmn_list.length, LTE_RRC_MAX_PLMN_LIST_SIZE));
      len_of_eplmn_list = (byte)(MIN(len_of_eplmn_list,MAX_NO_OF_PLMNs));

      for(index = 0; index < len_of_eplmn_list; index ++)
      {
        if(emm_is_plmn_forbidden(eplmn_list.plmn[index], emm_ctrl_data_ptr) == FALSE)
        {
          rrc_eplmn_list_ptr->plmn[rrc_eplmn_list_ptr->num_plmns] = 
                emm_convert_nas_plmn_id_to_rrc_plmn_id(eplmn_list.plmn[index]);
          rrc_eplmn_list_ptr->num_plmns++;
        }
      }
    }
  }

  MSG_HIGH_DS_1(MM_SUB,"=EMM= Populated RRC EPLMN list - Length %d", 
                rrc_eplmn_list_ptr->num_plmns);

} /* emm_set_rrc_eplmn_list */

/*=========================================================================== 
 
FUNCTION  EMM_SET_RRC_RPLMN

DESCRIPTION
  This function sets the RPLMN field of the primitives that to be sent to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void  emm_set_rrc_rplmn
(
  boolean             *rplmn_valid_ptr,
  lte_rrc_plmn_s      *rplmn_ptr,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_state_type     emm_state = EMM_INVALID_STATE;
  

  emm_state = EMM_GET_STATE();
  *rplmn_valid_ptr = FALSE;
  memset((void *)rplmn_ptr, 0xFF, sizeof(lte_rrc_plmn_s));

  MSG_HIGH_DS_3(MM_SUB,"=EMM= Setting RPLMN - Sim state %d, EMM state %d, Last RTAI %d",
                emm_ctrl_data_ptr->plmn_service_state->sim_state,
                emm_state,
                emm_ctrl_data_ptr->last_registered_tai_valid);

  if(((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_AVAILABLE) ||
      (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                     SYS_SIM_STATE_CS_INVALID)) &&
     (sys_plmn_id_is_undefined(emm_ctrl_data_ptr->rplmn_info.rplmn) == FALSE))
  {
    *rplmn_valid_ptr = TRUE;
    *rplmn_ptr = emm_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->rplmn_info.rplmn);
  }

} /* emm_set_rrc_rplmn */


/*=========================================================================== 
 FUNCTION  emm_add_to_rpm_ftai_list

DESCRIPTION
  Updates rpm ftai list and rrc forbidden ftai list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_add_to_rpm_ftai_list
(
  lte_nas_tai_lst1_type  tai,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  emm_rpm_ftai_list_T *traverse_list_ptr, *prev_list_ptr;
  ASSERT(emm_ctrl_data_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->rrc_forbidden_list_ptr != NULL);
  if(emm_ctrl_data_ptr->rpm_ftai_list_ptr == NULL)
  {
    emm_ctrl_data_ptr->rpm_ftai_list_ptr = (emm_rpm_ftai_list_T *) emm_modem_mem_alloc(sizeof(emm_rpm_ftai_list_T),
                                                emm_ctrl_data_ptr);
    traverse_list_ptr = emm_ctrl_data_ptr->rpm_ftai_list_ptr;
  }
  else
  {
    traverse_list_ptr = emm_ctrl_data_ptr->rpm_ftai_list_ptr;
    while(traverse_list_ptr != NULL)
    {
      prev_list_ptr = traverse_list_ptr;
      traverse_list_ptr = traverse_list_ptr->next;
    }
    prev_list_ptr->next = (emm_rpm_ftai_list_T *) emm_modem_mem_alloc(sizeof(emm_rpm_ftai_list_T),
                                                emm_ctrl_data_ptr);
    traverse_list_ptr = prev_list_ptr->next;
  }
  ASSERT(traverse_list_ptr != NULL);
  traverse_list_ptr->next = NULL;
  traverse_list_ptr->tai = tai;
  emm_add_forbidden_tai(emm_ctrl_data_ptr->rrc_forbidden_list_ptr,
                        tai, emm_ctrl_data_ptr);
}



/*=========================================================================== 
 
FUNCTION  emm_clear_rpm_ftai_list

DESCRIPTION
  clears the rpm ftai list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_clear_rpm_ftai_list
(
  boolean            remove_from_ftai_list,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  emm_rpm_ftai_list_T *traverse_list_ptr,*swap_list_ptr;
  ASSERT(emm_ctrl_data_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->rrc_forbidden_list_ptr != NULL);
  traverse_list_ptr = emm_ctrl_data_ptr->rpm_ftai_list_ptr;
  while(traverse_list_ptr != NULL)
  {
    if(remove_from_ftai_list == TRUE)
    {
      emm_delete_forbidden_tai(&traverse_list_ptr->tai,
                             emm_ctrl_data_ptr->rrc_forbidden_list_ptr);
    }
    swap_list_ptr = traverse_list_ptr->next;
    emm_modem_mem_free(traverse_list_ptr, emm_ctrl_data_ptr);
    traverse_list_ptr = swap_list_ptr;
  }
  emm_ctrl_data_ptr->rpm_ftai_list_ptr = NULL;
}


/*=========================================================================== 
FUNCTION  emm_ctrl_data_mem_alloc

DESCRIPTION
  Allocate the memmory for ctrl data pointers variables

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_ctrl_data_mem_alloc
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  

  MSG_HIGH_DS_0(MM_SUB, "=EMM= ctrl data dynamic mem_alloc for EMM procedure messages");

  /* Allocate memory for TAI list */
  if(emm_ctrl_data_ptr->emm_tai_list_ptr == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_alloc for TAI list ");
    emm_ctrl_data_ptr->emm_tai_list_ptr = 
         (lte_nas_emm_tai_lst_info_type *) emm_modem_mem_alloc(sizeof(lte_nas_emm_tai_lst_info_type),
                                              emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->emm_tai_list_ptr);
  }
  if(emm_ctrl_data_ptr->reg_req_pending_ptr == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_alloc for reg_req_pending_ptr");
    emm_ctrl_data_ptr->reg_req_pending_ptr = 
         (mmr_reg_req_s_type *) emm_modem_mem_alloc(sizeof(mmr_reg_req_s_type),
                                              emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->reg_req_pending_ptr);
  }
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
  if(emm_ctrl_data_ptr->emergency_num_list_ptr == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_alloc for emergency_num_list_ptr");
    emm_ctrl_data_ptr->emergency_num_list_ptr = 
         (mmr_emergency_num_list_s_type *) emm_modem_mem_alloc(sizeof(mmr_emergency_num_list_s_type),
                                              emm_ctrl_data_ptr);
   mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->emergency_num_list_ptr);
  }
#endif
  if(emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_alloc for emm_nv_pref_mode_read_ptr");
    emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr = 
         (nv_item_type *) emm_modem_mem_alloc(sizeof(nv_item_type),
                                              emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr);
  }
  if(emm_ctrl_data_ptr->last_rrc_service_ind_ptr == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_alloc for last_rrc_service_ind_ptr");
    emm_ctrl_data_ptr->last_rrc_service_ind_ptr = 
         (lte_rrc_service_ind_s *) emm_modem_mem_alloc(sizeof(lte_rrc_service_ind_s),
                                              emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->last_rrc_service_ind_ptr);
  }
  if(emm_ctrl_data_ptr->rrc_forbidden_list_ptr == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_alloc for rrc_forbidden_list_ptr");
    emm_ctrl_data_ptr->rrc_forbidden_list_ptr = 
         (lte_nas_tai_lst2_type *) emm_modem_mem_alloc(sizeof(lte_nas_tai_lst2_type),
                                              emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->rrc_forbidden_list_ptr);
  }
  if(emm_ctrl_data_ptr->forbidden_for_service_list_ptr == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_alloc for forbidden_for_service_list_ptr");
    emm_ctrl_data_ptr->forbidden_for_service_list_ptr = 
         (lte_nas_tai_lst2_type *) emm_modem_mem_alloc(sizeof(lte_nas_tai_lst2_type),
                                              emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->forbidden_for_service_list_ptr);
  }
  if(emm_ctrl_data_ptr->manual_ftai_list_ptr == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_alloc for manual_ftai_list_ptr");
    emm_ctrl_data_ptr->manual_ftai_list_ptr = 
         (lte_nas_tai_lst2_type *) emm_modem_mem_alloc(sizeof(lte_nas_tai_lst2_type),
                                              emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->manual_ftai_list_ptr);

  }

}


/*=========================================================================== 
 
FUNCTION  emm_ctrl_data_mem_free

DESCRIPTION
  Free the memory allocated dynamically 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_ctrl_data_mem_free
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  
  MSG_HIGH_DS_0(MM_SUB, "=EMM= freeing dynamic memory");

  /* free memory for TAI list  */
  if(emm_ctrl_data_ptr->emm_tai_list_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_free for TAI list");
    emm_modem_mem_free(emm_ctrl_data_ptr->emm_tai_list_ptr, emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->emm_tai_list_ptr = NULL;
  }
  if(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_free for reg_req_pending_ptr");
    emm_modem_mem_free(emm_ctrl_data_ptr->reg_req_pending_ptr, emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->reg_req_pending_ptr = NULL;
  }
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED  
  if(emm_ctrl_data_ptr->emergency_num_list_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_free for emergency_num_list_ptr");
    emm_modem_mem_free(emm_ctrl_data_ptr->emergency_num_list_ptr, emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->emergency_num_list_ptr = NULL;
  }
#endif 
  if(emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_free for emm_nv_pref_mode_read_ptr");
    emm_modem_mem_free(emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr, emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr = NULL;
  }
  if(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_free for last_rrc_service_ind_ptr");
    emm_modem_mem_free(emm_ctrl_data_ptr->last_rrc_service_ind_ptr, emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->last_rrc_service_ind_ptr = NULL;
  }
  if(emm_ctrl_data_ptr->rrc_forbidden_list_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_free for rrc_forbidden_list_ptr");
    emm_clear_rpm_ftai_list(FALSE,emm_ctrl_data_ptr);
    emm_modem_mem_free(emm_ctrl_data_ptr->rrc_forbidden_list_ptr, emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->rrc_forbidden_list_ptr = NULL;
  }
  if(emm_ctrl_data_ptr->forbidden_for_service_list_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_free for forbidden_for_service_list_ptr");
    emm_modem_mem_free(emm_ctrl_data_ptr->forbidden_for_service_list_ptr, emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->forbidden_for_service_list_ptr = NULL;
  }
  if(emm_ctrl_data_ptr->manual_ftai_list_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_free for manual_ftai_list_ptr");
    emm_modem_mem_free(emm_ctrl_data_ptr->manual_ftai_list_ptr, emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->manual_ftai_list_ptr = NULL;
  }
}

/*=========================================================================== 
 
FUNCTION  emm_ctrl_data_mem_alloc_check

DESCRIPTION
  This functions asserts if memory for emm_ctrl_data pointer variables is not allocated correctly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_ctrl_data_mem_alloc_check
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  if((emm_ctrl_data_ptr->emm_tai_list_ptr == NULL) ||
     (emm_ctrl_data_ptr->reg_req_pending_ptr == NULL) ||
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
     (emm_ctrl_data_ptr->emergency_num_list_ptr == NULL) ||
#endif
     (emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr == NULL) ||
     (emm_ctrl_data_ptr->last_rrc_service_ind_ptr == NULL) ||
     (emm_ctrl_data_ptr->rrc_forbidden_list_ptr == NULL) ||
     (emm_ctrl_data_ptr->forbidden_for_service_list_ptr == NULL) ||
     (emm_ctrl_data_ptr->manual_ftai_list_ptr == NULL))
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= emm_ctrl_data_mem_alloc_check fail",0,0,0);
  }
}


/*=========================================================================== 
 
FUNCTION  emm_procedure_message_mem_alloc

DESCRIPTION
  Allocate the memmory for ctrl data pointers variables

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_procedure_message_mem_alloc
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  MSG_HIGH_DS_0(MM_SUB, "=EMM= ctrl data dynamic mem_alloc for EMM procedure messages");

  /* Allocate memory for PDN Conn req */
  if(emm_ctrl_data_ptr->pdn_conn_req_ptr == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_alloc for pdn conn req ");
    emm_ctrl_data_ptr->pdn_conn_req_ptr = 
    	  (lte_nas_esm_pdn_connectivity_req *)emm_mem_alloc(sizeof(lte_nas_esm_pdn_connectivity_req),
                                              emm_ctrl_data_ptr);	
    mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->pdn_conn_req_ptr);

  }

  /* Allocate memory for out_msg_ptr */
  if(emm_ctrl_data_ptr->out_msg_ptr == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_alloc for out_msg");
    emm_ctrl_data_ptr->out_msg_ptr = 
    	  (lte_nas_outgoing_msg_type *)emm_mem_alloc(sizeof(lte_nas_outgoing_msg_type),
                                              emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->out_msg_ptr);

  }

  /* Allocate memory for ota message*/
  if(emm_ctrl_data_ptr->emm_ota_message_ptr == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_alloc for ota message");
    emm_ctrl_data_ptr->emm_ota_message_ptr = (byte *)emm_mem_alloc(sizeof(byte)*EMM_MAX_MESSAGE_LENGTH,
                                                                 emm_ctrl_data_ptr);                                                            
    mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->emm_ota_message_ptr);

  }

  /* Allocate memory for one incoming message and for additional
     messages allocate dynamically and add it to queue */
  if(emm_ctrl_data_ptr->nas_incoming_msg_ptr == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_alloc for nas incoming message");
    emm_ctrl_data_ptr->nas_incoming_msg_ptr  =  
          (lte_nas_incoming_msg_type *)emm_mem_alloc(sizeof(lte_nas_incoming_msg_type)*MAX_NAS_INCOMING_MSG,
                                       emm_ctrl_data_ptr);                                   
    mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->nas_incoming_msg_ptr);
  }
}

/*=========================================================================== 
 
FUNCTION  emm_procedure_message_mem_free

DESCRIPTION
  Free the memory allocated dynamically 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_procedure_message_mem_free
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  MSG_HIGH_DS_0(MM_SUB, "=EMM= freeing dynamic memory");

  /* free memory for PDN Conn req */
  if(emm_ctrl_data_ptr->pdn_conn_req_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_free for pdn conn req");
    emm_mem_free(emm_ctrl_data_ptr->pdn_conn_req_ptr, emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->pdn_conn_req_ptr = NULL;
  }

  /* free memory for out_msg_ptr */
  if(emm_ctrl_data_ptr->out_msg_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_free for out_msg");
    emm_mem_free(emm_ctrl_data_ptr->out_msg_ptr, emm_ctrl_data_ptr);
	emm_ctrl_data_ptr->out_msg_ptr = NULL;
  }

  /* free memory for ota message */
  if(emm_ctrl_data_ptr->emm_ota_message_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_free for ota message");
    emm_mem_free(emm_ctrl_data_ptr->emm_ota_message_ptr, emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->emm_ota_message_ptr = NULL;
  }

  /* free memory for ota message */
  if(emm_ctrl_data_ptr->nas_incoming_msg_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= mem_free for nas incoming message");
    emm_mem_free(emm_ctrl_data_ptr->nas_incoming_msg_ptr, emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->nas_incoming_msg_ptr = NULL;
  }
}

/*===========================================================================

FUNCTION  EMM_CLEAR_MANUAL_FTAI_LIST

DESCRIPTION
  This function clears forbidden TAI list for manual selection

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_clear_manual_ftai_list(void)
{
  emm_ctrl_data_type  *emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  

  if(emm_ctrl_data_ptr->manual_ftai_list_ptr == NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= This function should not be called from this SUB");
    return;
  }

  (void)pthread_mutex_lock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);

  emm_ctrl_data_ptr->manual_ftai_list_ptr->tai_lst_length = 0;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= FTAI list for manual sel is cleared");

  (void)pthread_mutex_unlock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);

}/* emm_clear_manual_ftai_list */

/*===========================================================================

FUNCTION EMM_ADD_MANUAL_FTAI

DESCRIPTION
  This function adds the given TAI to forbidden TAI list for manual selection 
  if necessary

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_add_manual_ftai
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr,
  boolean             temp_forbidden
)
{
   
  if ((emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == 
                                          SYS_NETWORK_SELECTION_MODE_MANUAL) ||
      (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == 
                        SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  {
    (void)pthread_mutex_lock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);
  
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Add serving TAI to FTAI list for manual Sel");
    
    emm_add_forbidden_tai(emm_ctrl_data_ptr->manual_ftai_list_ptr,
                          emm_get_camped_tai(emm_ctrl_data_ptr,temp_forbidden),
                           emm_ctrl_data_ptr);
  
    (void)pthread_mutex_unlock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);
  }
} /* end of emm_add_manual_ftai() */


/*===========================================================================

FUNCTION EMM_DELETE_MANUAL_FTAI

DESCRIPTION
  This function deletes the given TAI from forbidden TAI list for manual selection 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_delete_manual_ftai
(
  lte_nas_tai_lst1_type *tai_tobe_deleted,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
   
  if((emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == 
                                          SYS_NETWORK_SELECTION_MODE_MANUAL) ||
      (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == 
                        SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  {
    (void)pthread_mutex_lock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);
    
    emm_delete_forbidden_tai(tai_tobe_deleted,
                                   emm_ctrl_data_ptr->manual_ftai_list_ptr);
  
    (void)pthread_mutex_unlock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);
  }
} /* end of emm_add_manual_ftai() */


/*===========================================================================

FUNCTION  EMM_IS_TAI_FORBIDDEN_FOR_MANUAL

DESCRIPTION
  This function will check if the requested TAI is present in the forbiden TAI
  list for manual slection

DEPENDENCIES
  None

RETURN VALUE
  TRUE: TAI is forbidden
  FALSE: TAI is not forbidden

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_is_tai_forbidden_for_manual
(
  lte_nas_tai_lst1_type  tai,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  boolean  plmn_id_is_undefined;
  boolean  mnc_includes_pcs_digit;
  uint32   mcc;
  uint32   mnc;

  
  
  (void)pthread_mutex_lock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);
  
  if(emm_search_tai_list2(tai, emm_ctrl_data_ptr->manual_ftai_list_ptr) == TRUE)
  {
    sys_plmn_get_mcc_mnc(tai.plmn,
                         &plmn_id_is_undefined,
                         &mnc_includes_pcs_digit,
                         &mcc,
                         &mnc);

    MSG_HIGH_DS_3(MM_SUB,"=EMM= TA is forbidden for manual sel - PLMN %d-%d, TAC %d",
                  mcc, mnc, tai.tac);

    (void)pthread_mutex_unlock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);

    return TRUE; 
  } 

  (void)pthread_mutex_unlock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);

  return FALSE;

}/* emm_is_tai_forbidden_for_manual */

/*=========================================================================== 
 
FUNCTION  EMM_SEND_PS_DETACH_CNF

DESCRIPTION
  This function sends PS DETACH CNF and cleans up PS DETACH state machine

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void  emm_send_ps_detach_cnf
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending PS DETACH CNF");

  emm_ctrl_data_ptr->ps_detach_info.status = EMM_PS_DETACH_NONE;
  mm_stop_timer(TIMER_EMM_PS_DETACH);
#ifdef FEATURE_WWAN_GNSS_DYNAMIC
  if (gmm_check_wwan_resumption_req())
  {
    /* Send low priority indication to L-RRC */
    emm_send_rrc_wwan_set_priority(FALSE);
    gmm_set_wwan_resumption_req(FALSE);
  }
#endif

  mm_send_mmr_ps_detach_cnf();
#ifdef FEATURE_NAS_GW
  emm_set_mm_ps_detach_state(MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ);
#endif
}/* emm_send_ps_detach_cnf */

/*=========================================================================== 
 
FUNCTION  EMM_PS_DETACH_CLEAN_UP

DESCRIPTION
  This function performs a local clean up and detach 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void  emm_ps_detach_clean_up
(
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  emm_failure_type       emm_failure_cause;
  

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  MSG_HIGH_DS_0(MM_SUB, "=EMM= PS detach clean up/Local detach");

  switch(EMM_GET_STATE())
  {
    case EMM_SERVICE_REQUEST_INITIATED:
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      if(EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED)
      {
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, TRUE,
                                  LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                  emm_failure_cause);
      }
      else
      {
        mm_stop_timer(TIMER_T3430);
        emm_remove_release_client(emm_tau_request_release_indication);
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);      
      }
      /* Process any pending messages */
      emm_process_unsent_messages(CONN_CNF_LTE_RRC_CONN_EST_FAILURE, 
                                  emm_failure_cause,
                                  emm_ctrl_data_ptr);      
#ifdef FEATURE_LTE_TO_1X
      /* Process all pending irat uplink messages */
      emm_process_pending_irat_msg(LTE_NAS_IRAT_INVALID_STATE,
                                   LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                   LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                                   emm_ctrl_data_ptr);
#endif
      break;
    case EMM_DEREGISTERED_INITIATED:
      emm_abort_mo_detach(emm_ctrl_data_ptr, FALSE);
      break ;
    default:
      break;    
  }

  emm_build_and_send_detach_ind(EMM_DETACHED);  
  emm_stop_all_timers(emm_ctrl_data_ptr);
  emm_ctrl_data_ptr->ongoing_detach_procedure = FALSE;

  if(emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                            SYS_SRV_STATUS_SRV)
  {
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
  }

  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_NO_SRV;
  emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;

  if((emm_ctrl_data_ptr->rrc_active == FALSE) && (emm_ctrl_data_ptr->reg_req_pended_indication == TRUE)
      &&(emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_NONE))
  {
    EMM_MOVES_TO_DEREGISTERED_STATE( EMM_DEREGISTERED_ATTACH_NEEDED );
  }
  else
  {
    EMM_MOVES_TO_DEREGISTERED_STATE( EMM_DEREGISTERED_PLMN_SEARCH );
  }

#ifdef FEATURE_NAS_GW
  if (emm_ctrl_data_ptr->rrc_active != TRUE)
  {
    /*Forcefully gmm state to dereg when CM send PDN disconnect
    (with cause ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT) and LTE is not active*/
    gmm_force_set_state(GMM_DEREGISTERED, GMM_NO_CELL_AVAILABLE);
  }
#endif
  if(((EMM_GET_CONNECTION_STATE() == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE) ||
     (EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE))
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_IRAT_DISABLED)
      &&
      (emm_ctrl_data_ptr->l2gcco_state == EMM_LTOG_CCO_INACTIVE)
#endif
    )
  {
    emm_rrc_abort_connection(emm_ctrl_data_ptr, 
                             FALSE,
                             LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
  }

} /* emm_ps_detach_clean_up */



/*===========================================================================

FUNCTION  EMM_CACHE_REG_CMD

DESCRIPTION: This function caches REG CMD

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_cache_reg_cmd
(
  mm_cmd_type         *mm_cmd_ptr,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Caching REG CMD - 0x%x", mm_cmd_ptr->cmd.hdr.message_id);

  if(emm_ctrl_data_ptr->pending_reg_cmd_ptr == NULL)
  {
    emm_ctrl_data_ptr->pending_reg_cmd_ptr = 
                   emm_modem_mem_alloc(sizeof(mm_cmd_type), emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->pending_reg_cmd_ptr);

  }
  else
  {
    memset((void *)emm_ctrl_data_ptr->pending_reg_cmd_ptr,
           0x0, sizeof(mm_cmd_type));
  }

  *(emm_ctrl_data_ptr->pending_reg_cmd_ptr) = *mm_cmd_ptr;

}/* emm_cache_reg_cmd */

/*===========================================================================

FUNCTION  EMM_FREE_DSM_MEMORY_AND_CLEAR_ATT_PTR

DESCRIPTION: This function frees the memory allocated to DSM item & clears the attach_ptr 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_free_dsm_memory_and_clear_att_ptr
(
  msgr_attach_struct_type *att_ptr,
  dsm_item_type *dsm_ptr
)
{
  MSG_HIGH_DS_0(MM_SUB, "=EMM= DSM items are freed & att_ptr is cleared");
  if(dsm_ptr != NULL)
  {
    /* Free the memory allocated to dsm_ptr */ 
    dsm_free_packet(&dsm_ptr); 
  }

  /* Clear the att_ptr which is also a structure to dsm_ptr*/
  if(att_ptr != NULL)
  {
  msgr_clear_attach(att_ptr); 
  }
   
}

/*===========================================================================

FUNCTION  emm_read_sim_available_nv

DESCRIPTION: This function reads nv at sim available. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_LTE  
 void emm_read_sim_available_nv
 (
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_ctrl_data_ptr->emm_3gpp_spec_ver = lte_rrc_get_3gpp_release_version();
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL8)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= Invalid UE release version = %d",
                 emm_ctrl_data_ptr->emm_3gpp_spec_ver, 0, 0);
  }
  MSG_HIGH_DS_1(MM_SUB ,"=EMM= 3GPP_SPEC = %d",emm_ctrl_data_ptr->emm_3gpp_spec_ver);
  emm_3gpp_spec_ver = emm_ctrl_data_ptr->emm_3gpp_spec_ver;
}
#endif

/*===========================================================================

FUNCTION  LTE_NAS_NV_READ

DESCRIPTION: This function reads all the NV items, required for LTE-NAS 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void lte_nas_nv_read
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  byte            nv_read_val = FALSE;
  ghdi_status_T   nv_status   = GHDI_SUCCESS ;

#if defined (FEATURE_LTE_REL9)
  
  if((nv_status = mm_ghdi_nvmem_read(
         NV_AAGPS_MT_LRSUPPORT_I,(ghdi_nvmem_data_T *)&(nv_read_val))) != GHDI_SUCCESS)
   {
     MSG_ERROR_DS_0(MM_SUB, "Unable to access NV to read NV_AAGPS_MT_LRSUPPORT_I, resetting LPP");
     emm_ctrl_data_ptr->lcs_supported = FALSE;
   }
   else
   {
     emm_ctrl_data_ptr->lcs_supported = (0x08 & nv_read_val) >> 3; // Bit 3  Ignore CP NI trigger (for CP LPP EPC-MT-LR)
     MSG_HIGH_DS_1(MM_SUB, "=EMM= LCS = %d", emm_ctrl_data_ptr->lcs_supported);
   }
#endif

}

/*===========================================================================

FUNCTION  EMM_CONVERT_EPS_BC_STATUS_TO_BIT_MASK

DESCRIPTION: This function coverts lte_nas_eps_bearer_context_status_type eps
             bearer context status to bit mask of two bytes
                          EBI/bit
             LSB   7  6  5  4  3  2 1 0 
             MSB  15 14 13 12 11 10 9 8

DEPENDENCIES
  None

RETURN VALUE
  unit16 

SIDE EFFECTS
  None

===========================================================================*/
uint16  emm_convert_eps_bc_status_to_bit_mask
(
  lte_nas_eps_bearer_context_status_type  eps_bc_status
)
{
  uint16  bit_mask = 0;

  if(eps_bc_status.eps_bearer_context_5_exists == TRUE)
  {
    bit_mask = bit_mask |(uint16)(1 << 5);
  }

  if(eps_bc_status.eps_bearer_context_6_exists == TRUE)
  {
    bit_mask = bit_mask | (uint16)(1 << 6);
  }

  if(eps_bc_status.eps_bearer_context_7_exists == TRUE)
  {
    bit_mask = bit_mask | (uint16)(1 << 7);
  }

  if(eps_bc_status.eps_bearer_context_8_exists == TRUE)
  {
    bit_mask = bit_mask | (uint16)(1 << 8);
  }

  if(eps_bc_status.eps_bearer_context_9_exists == TRUE)
  {
    bit_mask = bit_mask | (uint16)(1 << 9);
  }

  if(eps_bc_status.eps_bearer_context_10_exists == TRUE)
  {
    bit_mask = bit_mask |(uint16) (1 << 10);
  }

  if(eps_bc_status.eps_bearer_context_11_exists == TRUE)
  {
    bit_mask = bit_mask | (uint16)(1 << 11);
  }

  if(eps_bc_status.eps_bearer_context_12_exists == TRUE)
  {
    bit_mask = bit_mask | (uint16)(1 << 12);
  }

  if(eps_bc_status.eps_bearer_context_13_exists == TRUE)
  {
    bit_mask = bit_mask | (uint16)(1 << 13);
  }

  if(eps_bc_status.eps_bearer_context_14_exists == TRUE)
  {
    bit_mask = bit_mask | (uint16)(1 << 14);
  }

  if(eps_bc_status.eps_bearer_context_15_exists == TRUE)
  {
    bit_mask = bit_mask | (uint16)(1 << 15);
  }

  return bit_mask;

}/* emm_convert_eps_bc_status_to_bit_mask */

/*===========================================================================

FUNCTION  EMM_UPDATE_EPS_BC_STATUS_BY_BIT_MASK

DESCRIPTION: This function updates eps bearer context status with given bit mask

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_update_bc_status_by_bit_mask
(
  uint16               bit_mask,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  

  memset((void *)&emm_ctrl_data_ptr->eps_bearer_context_status, 0x0, 
         sizeof(lte_nas_eps_bearer_context_status_type));

  if((bit_mask & (1 << 5)) > 0)
  {
    emm_ctrl_data_ptr->eps_bearer_context_status.eps_bearer_context_5_exists 
      = TRUE;
  }

  if((bit_mask & (1 << 6)) > 0)
  {
    emm_ctrl_data_ptr->eps_bearer_context_status.eps_bearer_context_6_exists 
      = TRUE;
  }

  if((bit_mask & (1 << 7)) > 0)
  {
    emm_ctrl_data_ptr->eps_bearer_context_status.eps_bearer_context_7_exists 
      = TRUE;
  }

  if((bit_mask & (1 << 8)) > 0)
  {
    emm_ctrl_data_ptr->eps_bearer_context_status.eps_bearer_context_8_exists 
      = TRUE;
  }

  if((bit_mask & (1 << 9)) > 0)
  {
    emm_ctrl_data_ptr->eps_bearer_context_status.eps_bearer_context_9_exists 
      = TRUE;
  }

  if((bit_mask & (1 << 10)) > 0)
  {
    emm_ctrl_data_ptr->eps_bearer_context_status.eps_bearer_context_10_exists 
      = TRUE;
  }

  if((bit_mask & (1 << 11)) > 0)
  {
    emm_ctrl_data_ptr->eps_bearer_context_status.eps_bearer_context_11_exists 
      = TRUE;
  }

  if((bit_mask & (1 << 12)) > 0)
  {
    emm_ctrl_data_ptr->eps_bearer_context_status.eps_bearer_context_12_exists 
      = TRUE;
  }

  if((bit_mask & (1 << 13)) > 0)
  {
    emm_ctrl_data_ptr->eps_bearer_context_status.eps_bearer_context_13_exists 
      = TRUE;
  }

  if((bit_mask & (1 << 14)) > 0)
  {
    emm_ctrl_data_ptr->eps_bearer_context_status.eps_bearer_context_14_exists 
      = TRUE;
  }

  if((bit_mask & (1 << 15)) > 0)
  {
    emm_ctrl_data_ptr->eps_bearer_context_status.eps_bearer_context_15_exists 
      = TRUE;
  }

}/* emm_update_bc_status_by_bit_mask */


/*===========================================================================

FUNCTION  EMM_CHK_EPS_BEARER_SYNC_RECOVERY

DESCRIPTION: This function performs TAU when both of following conditions are
             meet to bring UE and NW back in sync of the EPS bearer status
             - UE receives PDN CONN REJ with cause - Multiple PDN connections
               for a given APN not allowed (55)
             - UE is in REGISTERED or SERVICE REQUEST INITED state
             - NW has extra active bearer(s) that UE does not have

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_chk_eps_bearer_sync_recovery
(
  lte_nas_esm_incoming_msg_type  *msg_ptr,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  byte    index = 0;
  uint16  ue_bearer_status = 0, nw_bearer_status = 0;

  

  if((msg_ptr->nas_hdr.msg_id == PDN_CONNECTIVITY_REJECT) 
     &&
     (msg_ptr->pdn_connectivity_rej.esm_cause.valid == TRUE) 
     &&
     (msg_ptr->pdn_connectivity_rej.esm_cause.esm_cause == 
                           NAS_ESM_MULTIPLE_PDN_CONN_FOR_GIVEN_APN_NOT_ALLOWED)
     &&
     ((EMM_GET_STATE() == EMM_REGISTERED) || 
      (EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED)))
  {
    /* Convert NW active bearer context status to bit mask */
    nas_assert_check((boolean)(emm_ctrl_data_ptr->last_rrc_bearer_ind.num_active_bearers <= 
                                           LTE_RRC_MAX_NUM_ACTIVE_EPS_BEARERS));

    for(index = 0;index < emm_ctrl_data_ptr->last_rrc_bearer_ind.\
                                                  num_active_bearers; index ++)
    {
      nw_bearer_status = nw_bearer_status | (1 << emm_ctrl_data_ptr->\
                         last_rrc_bearer_ind.active_eps_bearers[index].eps_id);
    }

    /* Convert UE active bearer context status to bit mask */    
    ue_bearer_status = emm_convert_eps_bc_status_to_bit_mask(
                                 emm_ctrl_data_ptr->eps_bearer_context_status);

    /* Update UE bit mask to have bearer contexts that NW and UE both have */
    ue_bearer_status = ue_bearer_status & nw_bearer_status;

    /* If update UE mask is still different from NW mask, it means that NW has 
       extra bearer context. In this case TAU should be triggered. */
    if((nw_bearer_status ^ ue_bearer_status) > 0)
    {
      /* Update global with updated UE mask to be used in TAU REQ message */
      emm_update_bc_status_by_bit_mask(ue_bearer_status, emm_ctrl_data_ptr);

      MSG_HIGH_DS_0(MM_SUB,"=EMM= Initing TAU - PDN conn failure/Extra EPS BC on NW side");
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                                 emm_get_eps_update_type(emm_ctrl_data_ptr),
                                 FALSE, 
                                 emm_ctrl_data_ptr);
    }
  }   
}/* emm_chk_eps_bearer_sync_recovery */


/*===========================================================================

FUNCTION  emm_add_plmn_to_backoff_fplmn_list

DESCRIPTION
If PLMN is not present in backoff FPLMN list then 
        a)Add PLMN the list 
         b) send change indication to REG


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void emm_add_plmn_to_backoff_fplmn_list
(
  sys_plmn_id_s_type plmn,
  rex_timer_cnt_type  timer,
  sys_block_plmn_cause_e_type cause
)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = emm_db_get_ctrl_data();
   
  if(reg_sim_add_plmn_to_backoff_fplmn_list(plmn,timer,cause,(sys_modem_as_id_e_type)mm_sub_id))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Adding PLMN to backoff forbidden PLMN list");
    //Block RPLMN as well if current blocked PLMN is part of EPLMN list
    if (sys_eplmn_list_equivalent_plmn(plmn))
    {
      sys_plmn_id_s_type rplmn = sys_eplmn_list_rplmn();
      if(sys_plmn_match(rplmn, plmn) == FALSE)
      {
        reg_sim_add_plmn_to_backoff_fplmn_list(rplmn,timer,cause,(sys_modem_as_id_e_type)mm_sub_id);
      }
    }
    if(emm_ctrl_data_ptr->last_registered_tai_valid &&
       PLMN_MATCH(plmn.identity,emm_ctrl_data_ptr->last_registered_tai.plmn.identity))
    {
      emm_block_tai_list_plmns(&plmn,timer,cause,emm_ctrl_data_ptr->emm_tai_list_ptr);
    }
#ifdef FEATURE_LTE_TO_1X
    mm_send_mmr_fplmn_list_change_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
#endif 
  }
}


/*===========================================================================

FUNCTION  emm_add_eplmn_to_backoff_fplmn_list

DESCRIPTION
This function adds EPLMN passed as the parameter to the backoff list. 
  The timer value passed is not the actual timer value but the current running timer plus
  the timer value passed.
  This function simply adds the PLMN to backoff list with the timer value of the PLMN which
  is already present in the list.


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void emm_add_eplmn_to_backoff_fplmn_list
(
  sys_plmn_id_s_type plmn,
  rex_timer_cnt_type  timer,
  sys_block_plmn_cause_e_type cause
)
{
  if(reg_sim_add_eplmn_to_backoff_fplmn_list(plmn,timer,cause,(sys_modem_as_id_e_type)mm_sub_id))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Adding EPLMN to backoff forbidden PLMN list");
#ifdef FEATURE_LTE_TO_1X
    mm_send_mmr_fplmn_list_change_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
#endif 
  }
}


/*===========================================================================

FUNCTION  emm_remove_plmn_from_backoff_fplmn_list

DESCRIPTION
If PLMN is present in backoff FPLMN list then 
        a)remove PLMN the list 
         b) send change indication to REG


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void emm_remove_plmn_from_backoff_fplmn_list
(
  sys_plmn_id_s_type plmn
)
{
  if(reg_sim_remove_plmn_from_backoff_fplmn_list(plmn,SYS_BLOCK_PLMN_CAUSE_NONE,
                                                 (sys_modem_as_id_e_type)mm_sub_id))
  {  
#ifdef FEATURE_LTE_TO_1X 
    mm_send_mmr_fplmn_list_change_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
#endif 
  }
}


/*===========================================================================
FUNCTION      EMM_IS_PLMN_REG_ALLOWED

DESCRIPTION
 it would check if plmn/tai is forbidden or not for the ue to register on
 the plmn
 
DEPENDENCIES
  None 

RETURN VALUE
  TRUE   -  If registration is allowed
  FALSE  -  If registration is not allowed
 
SIDE EFFECTS
  None
===========================================================================*/
boolean emm_is_plmn_reg_allowed
(
  lte_rrc_service_ind_s     *rrc_service_ind,
  emm_ctrl_data_type        *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type  tai;
  sys_plmn_id_s_type rrc_reported_plmn ;
  boolean is_reg_allowed = FALSE;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  sys_csg_info_s_type  rrc_reported_csg; 
#endif 

   
  if(rrc_service_ind == NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function emm_is_plmn_reg_allowed");
    return FALSE;
  }

  tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                              rrc_service_ind->camped_svc_info.selected_plmn);
  tai.tac =  rrc_service_ind->camped_svc_info.tac;

  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  rrc_reported_csg = rrc_service_ind->camped_svc_info.csg_info;
#endif 

  if((((emm_is_plmn_forbidden(rrc_reported_plmn,emm_ctrl_data_ptr) == FALSE) &&
      (emm_is_tai_forbidden(tai,emm_ctrl_data_ptr)== FALSE)                
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW) 
       && (mm_is_csg_in_white_list(rrc_reported_plmn, &rrc_reported_csg) == TRUE)
#endif 
       )
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
      || (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
#endif 
      ) &&
      (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == TRUE) &&
      (rrc_service_ind->camped_svc_info.cell_access_status !=
                                       LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY) && 
     ((mm_timer_status[TIMER_T3402] == TIMER_STOPPED) ||
      (emm_chk_ta_change(rrc_service_ind, emm_ctrl_data_ptr) == TRUE)) 
#ifdef FEATURE_LTE_REL9
     &&
     (rrc_service_ind->camped_svc_info.limited_mode_only == FALSE) &&
     ((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_AVAILABLE) ||
      (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_CS_INVALID))
#endif
    )
  {
    is_reg_allowed = TRUE;
  }
  
  return is_reg_allowed;
}

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
/*===========================================================================
FUNCTION      EMM_IS_MANUAL_CSG_SELECTION_SUCCESS

DESCRIPTION
 it would check if manual csg selection is successful
 
DEPENDENCIES
  None 

RETURN VALUE
  TRUE   -  If manual CSG selection success
  FALSE  -  If not manual_csg_selection
 
SIDE EFFECTS
  None
===========================================================================*/
boolean emm_is_manual_csg_selection_success
(
  emm_ctrl_data_type        *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type rrc_reported_plmn ;
  boolean manual_csg_selection_success = FALSE;
  sys_csg_id_type  rrc_reported_csg_id; 


  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                                    emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  rrc_reported_csg_id = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.csg_info.csg_id;

  if((sys_plmn_match(emm_ctrl_data_ptr->reg_req_pending_ptr->plmn, rrc_reported_plmn) == TRUE)&&
      (rrc_reported_csg_id == emm_ctrl_data_ptr->reg_req_pending_ptr->csg_id)&&
      (emm_ctrl_data_ptr->reg_req_pending_ptr->csg_id != SYS_CSG_ID_INVALID))
  {
    manual_csg_selection_success = TRUE;
  }
  return manual_csg_selection_success;
}

#endif 
/*===========================================================================
FUNCTION    EMM_RESIZE_MEM_FOR_OTA_MSG_PTR

DESCRIPTION
  Allocates memory for the ota_msg_ptr if ul_msg is greater than 
  EMM_MAX_MESSAGE_LENGTH
 
DEPENDENCIES
  None

RETURN VALUE
 NONE

SIDE EFFECTS
  None
===========================================================================*/
size_t emm_resize_mem_for_ota_msg_ptr
( 
  emm_ctrl_data_type      *emm_ctrl_data_ptr,
  lte_nas_message_id       msg_id
)
{
  size_t msg_len =0;

  switch (msg_id)
  {

#if defined (FEATURE_LTE_REL9) 
#ifdef FEATURE_NAS_GPS_ENABLE
    case UL_GENERIC_NAS_TRANSPORT:
      /*--------------------------------------------------------------------
      This message could have length more upto 65535 + 65535
      -----------------------------------------------------------------------*/
      msg_len = emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.ul_generic_nas_transport_msg.generic_msg_container.container_length +
              emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.ul_generic_nas_transport_msg.additional_info.additional_info_length + 1;
      break;
#endif
#endif
#ifdef FEATURE_CIOT
    case CP_SERVICE_REQUEST:
      if (emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.cp_service_req.esm_msg_container_exists == TRUE) 
      {
        msg_len = emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.cp_service_req.esm_msg_container.esm_data_transport.user_data_container.container_length;
      }
      msg_len += 270;//Spec Table 8.2.33.1: CONTROL PLANE SERVICE REQUEST message contentmax size of control plane 
      if (mm_get_ulog_handle() != NULL)
      {
        ULOG_RT_PRINTF_0(mm_get_ulog_handle(), "=EMM= Resize memory for ESM_DATA_TRANSPORT");
      }
      break;
    case ESM_DATA_TRANSPORT:
      msg_len = emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.esm_data_transport.user_data_container.container_length;
      msg_len += 12;//Spec 24.301 Table 8.3.25.1: ESM DATA TRANSPORT message content-max size of emm data transport msg
#endif
    case PDN_CONNECTIVITY_REQUEST:
      if (emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.pdn_connectivity_req.ext_protocol_conf_opt.valid) 
      {
        msg_len = emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.pdn_connectivity_req.ext_protocol_conf_opt.len;
        msg_len += 975;//Spec 24.301 Table 8.3.20.1: PDN CONNECTIVITY REQUEST message content
      }
      break;
    case ESM_INFORMATION_RESPONSE:
      if (emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.esm_info_resp.ext_protocol_conf_opt.valid)
      {
        msg_len = emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.esm_info_resp.ext_protocol_conf_opt.len;  
        msg_len += 358;//Spec 24.301 Table 8.3.14 ESM INFORMATION RESPONSE message content
      }
      break;
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT:
      if (emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.default_bearer_context_accept.ext_protocol_conf_opt.valid) 
      {
        msg_len = emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.default_bearer_context_accept.ext_protocol_conf_opt.len;
        msg_len += 256;//Spec 24.301 Table 8.3.4      Activate default EPS bearer context accept
      }
      break;
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT:
      if (emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.default_bearer_context_rej.ext_protocol_conf_opt.valid) 
      {
        msg_len = emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.default_bearer_context_rej.ext_protocol_conf_opt.len;
        msg_len += 257;//Spec 24.301 Table 8.3.5      Activate default EPS bearer context reject
      }
      break;
    case DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT:
      if (emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.deactivate_eps_bearer_context_accept.ext_protocol_conf_opt.valid) 
      {
        msg_len = emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.deactivate_eps_bearer_context_accept.ext_protocol_conf_opt.len;
        msg_len += 256;//Spec 24.301 Table 8.3.11      Activate default EPS bearer context reject
      }
      break;
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT:
      if (emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.dedicated_bearer_context_accept.ext_protocol_conf_opt.valid) 
      {
        msg_len = emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.dedicated_bearer_context_accept.ext_protocol_conf_opt.len;
        msg_len += 513;//Spec 24.301 Table 8.3.1      Activate dedicated EPS bearer context accept
      }
      break;
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT:
      if (emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.dedicated_bearer_context_rej.ext_protocol_conf_opt.valid) 
      {
        msg_len = emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.dedicated_bearer_context_rej.ext_protocol_conf_opt.len; 
        msg_len += 513;//Spec 24.301 Table 8.3.2      Activate dedicated EPS bearer context reject
      }
      break;
    case BEARER_RESOURCE_ALLOCATION_REQUEST:
      msg_len = emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.bearer_alloc_req.ext_protocol_conf_opt.len; 
      msg_len += 785;//Spec 24.301 Table 8.3.8      Bearer resource allocation request
      break;
    case BEARER_RESOURCE_MODIFICATION_REQUEST:
      msg_len = emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.bearer_resource_modifi_req.ext_protocol_conf_opt.len; 
      msg_len += 1045;//Spec 24.301 Table 8.3.10    Bearer resource modification request
      break;
    case MODIFY_EPS_BEARER_CONTEXT_ACCEPT:
      msg_len = emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.modify_eps_bearer_ctxt_accept.ext_protocol_conf_opt.len; 
      msg_len += 513;//Spec 24.301 Table 8.3.16    Modify EPS bearer context accept
      break;
    case MODIFY_EPS_BEARER_CONTEXT_REJECT:
      msg_len = emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg.modify_eps_bearer_ctxt_rej.ext_protocol_conf_opt.len; 
      msg_len += 513;//Spec 24.301 Table 8.3.17    Modify EPS bearer context reject
      break;
    default:
      break;
  }

  /* if length is greater the max emm length re-allocate the buffer */
  if(msg_len > EMM_MAX_MESSAGE_LENGTH )
  {
    MSG_HIGH_DS_1(MM_SUB,"=EMM= realloc the memory for emm_ota_message_ptr to %ld bytes",
                  msg_len);
    if(emm_ctrl_data_ptr->emm_ota_message_ptr != NULL)
    {
      emm_mem_free(emm_ctrl_data_ptr->emm_ota_message_ptr,emm_ctrl_data_ptr);
      emm_ctrl_data_ptr->emm_ota_message_ptr = NULL;
    }
    emm_ctrl_data_ptr->emm_ota_message_ptr_flipped = TRUE;
    emm_ctrl_data_ptr->emm_ota_message_ptr = 
                       (byte *)emm_mem_alloc(sizeof(byte)*msg_len,
                                                   emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void *)emm_ctrl_data_ptr->emm_ota_message_ptr);
  } 
  else
  {
   msg_len  = EMM_MAX_MESSAGE_LENGTH;
  }
  return msg_len;
}


/*===========================================================================
FUNCTION    EMM_REALLOC_MEM_FOR_OTA_MSG_PTR

DESCRIPTION
  This function reallocates the memory (2k) for ota_msg_ptr which was
  earlier allocated dynamically while processing UL_GENERIC_NAS_TRANSPORT 
  if the length of the OTA message was estimated to be greater than 2k

DEPENDENCIES
  None

RETURN VALUE
 NONE

SIDE EFFECTS
  None
===========================================================================*/
void emm_realloc_mem_for_ota_msg_ptr
( 
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
   
  /* Message transmission completed, the memory allcoated */
  if(emm_ctrl_data_ptr->emm_ota_message_ptr != NULL)
  {
    emm_mem_free(emm_ctrl_data_ptr->emm_ota_message_ptr,emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->emm_ota_message_ptr = NULL;
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= this case is impossible, 'emm_ctrl_data_ptr->emm_ota_message_ptr' cannot be NULL");
  }

  /* Allocate memory for ota message*/
  emm_ctrl_data_ptr->emm_ota_message_ptr = (byte *)emm_mem_alloc(sizeof(byte)*EMM_MAX_MESSAGE_LENGTH,
                                                                       emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void *)emm_ctrl_data_ptr->emm_ota_message_ptr);
  MSG_HIGH_DS_0(MM_SUB, "=EMM= emm_ota_message_ptr freed & memory size is reallocated back to 2K");
  emm_ctrl_data_ptr->emm_ota_message_ptr_flipped = FALSE;
}

/*===========================================================================
FUNCTION  EMM_POPULATE_IMEI

DESCRIPTION
  This function populates IMEI to given structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_populate_imei
(
  lte_nas_emm_imei_type  *imei_ptr,
  boolean                 eps_mobile_id_type
)
{
  byte  ghdi_imei[MAX_IMEI_LEN+1];
  ghdi_status_T  status = GHDI_SUCCESS;

  memset((void *)ghdi_imei, 0x0, MAX_IMEI_LEN + 1);

  memset((void *)imei_ptr, 0x0, sizeof(lte_nas_emm_imei_type));

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
  status = ghdi_ds_read_imei(ghdi_imei,(sys_modem_as_id_e_type)mm_sub_id);
#else
  status = ghdi_read_imei(ghdi_imei);
#endif

  if(status == GHDI_SUCCESS)
  {
    if(ghdi_imei[0] == MAX_IMEI_LEN)
    {
      /* Ensure that the last digit of the IMEI is set to zero
       (see TS 23.003 section 6.2.1) */      
      ghdi_imei[8] &= 0x0F;
      if(eps_mobile_id_type == TRUE)
      {
        ghdi_imei[1] &= 0xF8;
        ghdi_imei[1] |= IMEI_ID;
      }
    }
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Unable to read IMEI");
  }

  if(ghdi_imei[0] == MAX_IMEI_LEN)
  {
      imei_ptr->length = MAX_IMEI_LEN;
      memscpy((void*)&imei_ptr->value[0],   
              sizeof(imei_ptr->value),
              (void*)&ghdi_imei[1], 
              sizeof(byte) * MAX_IMEI_LEN);
    }
    else
    {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid IMEI length %d", ghdi_imei[0]);
    }

} /* emm_populate_imei */

/*===========================================================================

FUNCTION  EMM_CHK_LIMITED_SERVICE_STATE

DESCRIPTION
  This function checkes if UE is in limited service state (23.122 3.5)

DEPENDENCIES
  None

RETURN VALUE
  TRUE - UE is in limited service state
  FALSE - UE is not in limited service state

SIDE EFFECTS
  None

===========================================================================*/
boolean  emm_chk_limited_service_state
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type  srving_tai;
  boolean                limited_srv_state = FALSE;


  memset((void *)&srving_tai.plmn, 0xFF, sizeof(srving_tai.plmn));
  srving_tai.tac = 0xFFFE;

  srving_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
  srving_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;

  if((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                   SYS_SIM_STATE_NOT_AVAILABLE)
     ||
     (emm_ctrl_data_ptr->plmn_service_state->sim_state ==
                                                      SYS_SIM_STATE_PS_INVALID)
     ||
     (emm_ctrl_data_ptr->plmn_service_state->sim_state ==
                                                   SYS_SIM_STATE_CS_PS_INVALID)
     ||
     (emm_is_plmn_forbidden(srving_tai.plmn, emm_ctrl_data_ptr) == TRUE)
     ||
     (emm_is_tai_forbidden(srving_tai, emm_ctrl_data_ptr) == TRUE)
     ||
     (emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                      SYS_SRV_STATUS_LIMITED &&
      mm_timer_status[TIMER_T3402] != TIMER_STOPPED)
     ||
     ((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                      SYS_SRV_STATUS_LIMITED_REGIONAL ||
       emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                      SYS_SRV_STATUS_LIMITED) &&
      mm_timer_status[TIMER_T3346] == TIMER_ACTIVE))
  {
    limited_srv_state = TRUE;
  }

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Limited service state - %d", limited_srv_state);
  return limited_srv_state;

} /* emm_chk_limited_service_state */

/*===========================================================================

FUNCTION  EMM_RESET_EMC_SRV_STATE

DESCRIPTION
  This function reset emergency bearer service states

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

void emm_reset_emc_srv_state
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_security_context_info  *sec_ctxt_ptr = NULL;

  

#ifdef FEATURE_LTE_REL9
  switch(emm_ctrl_data_ptr->emc_srv_status)
  {
    case LTE_NAS_EMC_SRV_ONLY:
      emm_build_and_send_detach_ind(EMM_DETACHED);

      /* Delete EPLMN list and update RRC 
         Note: If active RAT is not LTE, GMM updates REG */
      if(mm_is_rat_lte(emm_ctrl_data_ptr->plmn_service_state->active_rat))
      {
        emm_ctrl_data_ptr->update_eplmn_list = TRUE;
      }
      emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
      emm_send_rrc_system_update(emm_ctrl_data_ptr);

      /* Delete full native EPS securitu context */
      sec_ctxt_ptr = emm_get_context_by_state(emm_ctrl_data_ptr, 
                                              CONTEXT_STATE_CURRENT);
      if(sec_ctxt_ptr != NULL)
      {
        emm_reset_security_info(sec_ctxt_ptr);
      }

      emm_restore_registration_ctxt(emm_ctrl_data_ptr);

      emm_ctrl_data_ptr->emc_srv_status = LTE_NAS_EMC_SRV_NONE;

      emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_NO_SRV;
      emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;

      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.\
                                                               emc_bs_supported
        = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
                                                 ims_emergency_support_enabled;
      break;
    default:
      break;
  }
  emm_ctrl_data_ptr->non_emc_bearer_deact_in_progress = FALSE;
#endif

} /* emm_reset_emc_srv_state */
#endif
#endif
/*===========================================================================
FUNCTION      EMM_PLMN_EQUIVALENT_T3346

DESCRIPTION
 it would check if plmn is same as t3346 or is in eplmn list
 
DEPENDENCIES
  None 

RETURN VALUE
  TRUE   -  If plmn is t3346 or in eplmn list
  FALSE  -  If plmn is a new plmn and not in eplmn list
 
SIDE EFFECTS
  None
===========================================================================*/
boolean emm_plmn_equivalent_t3346
(
  lte_rrc_service_ind_s     *rrc_service_ind,
  emm_ctrl_data_type        *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type   last_t3346_plmn;
  sys_plmn_id_s_type   rrc_reported_plmn;
  
   

  rrc_reported_plmn   = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);   
 
  memscpy((void*)&last_t3346_plmn,
          sizeof(last_t3346_plmn),
          (void*)emm_ctrl_data_ptr->t3346_plmn_ptr, 
          sizeof(sys_plmn_id_s_type));

  /* if last attempted plmn is same as previous plmn or is equivalent
     do not attempt registration */
  if((sys_plmn_match(rrc_reported_plmn, last_t3346_plmn) == TRUE)
     ||
     ((sys_eplmn_list_equivalent_plmn(last_t3346_plmn)   == TRUE) &&
      (sys_eplmn_list_equivalent_plmn(rrc_reported_plmn) == TRUE)))
  {
    return TRUE;
  }

  return FALSE;
}


/*===========================================================================
FUNCTION      emm_dsm_pushdown

DESCRIPTION
 creates a dsm packet by putting into appropriate pool
 
DEPENDENCIES
  None 

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
void emm_dsm_pushdown
(
   uint16              required_len,
   dsm_item_type       **dsm_ptr_ptr,
   byte                *message_ptr
)
{
  uint16 dsm_packet_length = 0;
  uint16 len=0;

  /*if we do not have enough small items use large item pool*/
  if((required_len <= DSM_DS_SMALL_ITEM_SIZ) && 
     (required_len <= NAS_REM_SMALL_ITEM_POOL_SIZE))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d small item pool", required_len);
    /* Pushdown DSM */
    dsm_packet_length = dsm_pushdown(dsm_ptr_ptr, 
                                     message_ptr,
                                     required_len, 
                                     DSM_DS_SMALL_ITEM_POOL);

    if(required_len != dsm_packet_length)
    {
      /*free any partial memory allocated */
      dsm_free_packet(dsm_ptr_ptr);

      MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d large item pool", dsm_packet_length);
      /* If Small item pool is running out always try Large item pool */
      len = dsm_pushdown(dsm_ptr_ptr,message_ptr,required_len, DSM_DS_LARGE_ITEM_POOL);
      if(len != required_len) 
      {
        MSG_FATAL_DS(MM_SUB,"=EMM= Failed to push len %d large item pool",dsm_packet_length,0,0);
      }
    }
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d large item pool", dsm_packet_length);

    /* If Small item pool is running out always use Large item pool */
    dsm_packet_length = dsm_pushdown(dsm_ptr_ptr, 
                                     message_ptr,
                                     required_len, 
                                     DSM_DS_LARGE_ITEM_POOL);

    if(required_len != dsm_packet_length)
    {
      /*free any partial memory allocated */
      dsm_free_packet(dsm_ptr_ptr);

      MSG_HIGH_DS_1(MM_SUB, "=EMM= dsm push len %d small item pool", required_len);

      /* If Small item pool is running out always try Large item pool */
      len = dsm_pushdown(dsm_ptr_ptr,message_ptr,required_len, DSM_DS_SMALL_ITEM_POOL);
      if(len != required_len) 
      {
        MSG_FATAL_DS(MM_SUB,"=EMM= Failed to push len %d small item pool",required_len,0,0);
      }
    }
  }
}

/*===========================================================================
FUNCTION  EMM_CACHE_REGISTRATION_CTXT

DESCRIPTION
  This function caches current registration conetxt including last registered 
  TAI, GUTI, TIN type and EPS update status when UE is emergency attached for 
  a later restoration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

void  emm_cache_registration_ctxt
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_reg_ctxt_type  *reg_ctxt_ptr = NULL;

  

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Caching EMM registration context");

  reg_ctxt_ptr = &emm_ctrl_data_ptr->cached_reg_ctxt;

  reg_ctxt_ptr->last_registered_tai_valid = 
                                  emm_ctrl_data_ptr->last_registered_tai_valid;
  reg_ctxt_ptr->last_registered_tai = emm_ctrl_data_ptr->last_registered_tai;
  reg_ctxt_ptr->guti_valid = emm_ctrl_data_ptr->emm_guti_valid;
  reg_ctxt_ptr->guti = emm_ctrl_data_ptr->emm_guti;
#ifdef FEATURE_NAS_GW
  reg_ctxt_ptr->tin_type = emm_ctrl_data_ptr->TIN;
#endif
  reg_ctxt_ptr->update_status = emm_ctrl_data_ptr->emm_update_status;

}/* emm_cache_registration_ctxt */
#endif
#endif
/*===========================================================================

FUNCTION  EMM_RESTORE_REGISTRATION_CTXT

DESCRIPTION
  This function restores cached registration conetxt including last registered 
  TAI, GUTI, TIN type and EPS update status when UE is no longer emergency 
  attached.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

void  emm_restore_registration_ctxt
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_reg_ctxt_type  *reg_ctxt_ptr = NULL;

  

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Restoring EMM registration context");

  reg_ctxt_ptr = &emm_ctrl_data_ptr->cached_reg_ctxt;

  emm_ctrl_data_ptr->last_registered_tai_valid =
                                       reg_ctxt_ptr->last_registered_tai_valid;
  emm_ctrl_data_ptr->last_registered_tai = reg_ctxt_ptr->last_registered_tai;
  emm_ctrl_data_ptr->emm_guti_valid = reg_ctxt_ptr->guti_valid;
  emm_ctrl_data_ptr->emm_guti = reg_ctxt_ptr->guti;
#ifdef FEATURE_NAS_GW
  emm_ctrl_data_ptr->TIN = reg_ctxt_ptr->tin_type;
#endif
  emm_ctrl_data_ptr->emm_update_status = reg_ctxt_ptr->update_status;

}/* emm_restore_registration_ctxt */
#endif
#endif
#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION  EMM_TAG_AS_ID

DESCRIPTION
  This function tags as_ids to the primitive before calling msgr_send.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_tag_as_id
(
  msgr_hdr_struct_type* msg_ptr
)
{
  switch(msg_ptr->id)
  {
    /* Messages to LTE RRC */
    case LTE_RRC_SERVICE_REQ:
    case LTE_RRC_PLMN_SEARCH_REQ:
    case LTE_RRC_PLMN_SEARCH_ABORT_REQ:
    case LTE_RRC_CONN_EST_REQ:
    case LTE_RRC_UL_DATA_REQ:
    case LTE_RRC_CONN_ABORT_REQ:
    case LTE_RRC_CELL_BAR_REQ:
    case LTE_RRC_DEACTIVATE_REQ:
    case LTE_RRC_SIM_UPDATE_REQ:
    case LTE_RRC_SYSTEM_UPDATE_REQ:
    case LTE_RRC_NW_SEL_MODE_RESET_REQ:
    case LTE_RRC_ACTIVATION_RSP:
    case LTE_RRC_NAS_UMTS_KEY_RSP:
    case LTE_RRC_NAS_LTE_KEY_RSP:
    case LTE_RRC_DRX_INFO_REQ:
    case LTE_RRC_FORBIDDEN_TA_LIST_RESET_REQ:
    case LTE_RRC_IRAT_TUNNEL_UL_MSG_REQ:
    case LTE_RRC_CSFB_CALL_STATUS_IND:
    case LTE_RRC_NAS_GSM_KEY_RSP:
    case LTE_RRC_DSDS_TRM_PRIORITY_REQ:
    case LTE_RRC_UE_MODE_IND:
    case LTE_RRC_UE_CAP_CHANGE_IND:
    case LTE_RRC_EDRX_CFG_REQ:
      msgr_set_hdr_inst(msg_ptr,(uint8)SYS_AS_ID_TO_INST_ID((sys_modem_as_id_e_type)mm_sub_id));
      break;
  
    /* Messages to TLB*/
    case LTE_TLB_DL_INFO_IND:
      msgr_set_hdr_inst(msg_ptr,(uint8)SYS_AS_ID_TO_INST_ID((sys_modem_as_id_e_type)mm_sub_id));
      break;
  
    /* Messages to DS*/
    case NAS_EMM_PLMN_CHANGE_IND:
    case NAS_EMM_ATTACH_COMPLETE_IND:
    case NAS_EMM_TAI_LIST_IND:
    case NAS_EMM_T3402_CHANGED_IND:
    case NAS_EMM_RESET_APN_SWITCH_IND:
      msgr_set_hdr_inst(msg_ptr,(uint8)SYS_AS_ID_TO_INST_ID((sys_modem_as_id_e_type)mm_sub_id));
      break;
  
    /* 1X CSFB - IRAT messages*/
    case NAS_EMM_IRAT_FAILURE_IND:
    case NAS_EMM_IRAT_DL_MSG_IND:
    case NAS_EMM_IRAT_IMSI_ATTACH_IND:
    case NAS_EMM_IRAT_CTXT_LOST_IND:
    case NAS_EMM_IRAT_UL_MSG_CNF:
    case LTE_RRC_IRAT_HDR_UL_TUNNEL_MSG_REQ:
    case NAS_EMM_IRAT_HDR_FAILURE_IND:
    case NAS_EMM_IRAT_HDR_DL_MSG_IND:
    case NAS_EMM_IRAT_HDR_UL_MSG_CNF:
      msgr_set_hdr_inst(msg_ptr,(uint8)SYS_AS_ID_TO_INST_ID((sys_modem_as_id_e_type)mm_sub_id));
      break;
  
    /* Messages to ESM*/
    case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
    case NAS_ESM_1XCSFB_ESR_CALL_ABORT_RSP:
    case NAS_ESM_ACTIVE_EPS_IND:
    case NAS_ESM_DATA_IND:
    case NAS_ESM_FAILURE_IND:
    case NAS_ESM_DETACH_IND:
    case NAS_ESM_EPS_BEARER_STATUS_IND:
    case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
#ifdef FEATURE_CIOT  //ESM_IND
    case NAS_ESM_ATTACH_STATUS_IND:
#endif
    case NAS_ESM_GET_ISR_STATUS_IND:
    case NAS_ESM_ISR_STATUS_CHANGE_IND:
    case NAS_ESM_1XCSFB_ESR_CALL_RSP:
    case NAS_ESM_UNBLOCK_ALL_APNS_IND:
#ifdef FEATURE_CIOT
    case NAS_ESM_DATA_CNF:
#endif
    case NAS_ESM_RESTORE_CTXT_PSM_REQ:
      msgr_set_hdr_inst(msg_ptr,(uint8)SYS_AS_ID_TO_INST_ID((sys_modem_as_id_e_type)mm_sub_id));
      break;
  
    /* Messages to GPS*/
    case NAS_EMM_DL_GENERIC_NAS_TRANSPORT_IND:
    case NAS_EMM_GENERIC_NAS_TRANSPORT_FAILURE_IND:
    case NAS_EMM_UL_GENERIC_NAS_TRANSPORT_CNF:
      msgr_set_hdr_inst(msg_ptr,(uint8)SYS_AS_ID_TO_INST_ID((sys_modem_as_id_e_type)mm_sub_id));
      break;
  
    case LTE_RRC_DS_STATUS_CHANGE_REQ:
       break; /*No specific Id*/
  
    default:
      msgr_set_hdr_inst(msg_ptr,(uint8)SYS_AS_ID_TO_INST_ID((sys_modem_as_id_e_type)mm_sub_id));
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Unknown message to tag UMID %d", msg_ptr->id);
      break;
  }
}
#endif

void emm_populate_mm_msg_info_outgoing_msg
(
  msgr_hdr_struct_type* msg_ptr
)
{
  if((msg_ptr->id)==LTE_RRC_DEACTIVATE_REQ)
  {
    mm_msg_info = (byte)(((lte_rrc_deactivate_req_s *)msg_ptr)->deactivate_reason);
  }
  else if ((msg_ptr->id)==LTE_RRC_SERVICE_REQ)
  {
    if (mm_checkif_rat_pri_list_has_rat((&((lte_rrc_service_req_s*)msg_ptr)->rat_pri_list),SYS_RAT_GSM_RADIO_ACCESS))
    {
      mm_msg_info = (byte)(mm_msg_info |(1<<(byte)SYS_RAT_GSM_RADIO_ACCESS));
    }        
    if (mm_checkif_rat_pri_list_has_rat((&((lte_rrc_service_req_s*)msg_ptr)->rat_pri_list),SYS_RAT_UMTS_RADIO_ACCESS))
    {
      mm_msg_info = (byte)(mm_msg_info |(1<<(byte)SYS_RAT_UMTS_RADIO_ACCESS));
    }
    if (mm_checkif_rat_pri_list_has_rat((&((lte_rrc_service_req_s*)msg_ptr)->rat_pri_list),SYS_RAT_LTE_RADIO_ACCESS))
    {
      mm_msg_info = (byte)(mm_msg_info |(1<<(byte)SYS_RAT_LTE_RADIO_ACCESS));
    }
    if (mm_checkif_rat_pri_list_has_rat((&((lte_rrc_service_req_s*)msg_ptr)->rat_pri_list),SYS_RAT_TDS_RADIO_ACCESS))
    {
      mm_msg_info = (byte)(mm_msg_info |(1<<(byte)SYS_RAT_TDS_RADIO_ACCESS));
    }
    if (mm_checkif_rat_pri_list_has_rat((&((lte_rrc_service_req_s*)msg_ptr)->rat_pri_list),SYS_RAT_LTE_M1_RADIO_ACCESS))
    {
      mm_msg_info = (byte)(mm_msg_info |(1<<(byte)SYS_RAT_LTE_M1_RADIO_ACCESS));
    }
    if (mm_checkif_rat_pri_list_has_rat((&((lte_rrc_service_req_s*)msg_ptr)->rat_pri_list),SYS_RAT_LTE_NB1_RADIO_ACCESS))
    {
      mm_msg_info = (byte)(mm_msg_info |(1<<(byte)SYS_RAT_LTE_NB1_RADIO_ACCESS));
    }
  }
  else if((msg_ptr->id)==LTE_RRC_PLMN_SEARCH_REQ)
  {
    mm_msg_info = (byte)(((lte_rrc_plmn_search_req_s*)msg_ptr)->trans_id);
  } 
}


void emm_msgr_send_error(msgr_hdr_struct_type*     msg_ptr,
                   uint32                     msg_len)
{
#ifdef FEATURE_DUAL_SIM
  emm_tag_as_id(msg_ptr);
#endif
  emm_add_message_to_debug_buffer(msg_ptr->id, FROM_EMM ,(sys_modem_as_id_e_type)mm_as_id);

  if(msgr_send(msg_ptr,msg_len)!= E_SUCCESS)
  {
    MSG_ERROR_DS_1(MM_SUB,"=NAS= Failed to send umid %d", msg_ptr->id);
  } 
}

void emm_msgr_send(msgr_hdr_struct_type*     msg_ptr,
                   uint32                     msg_len)
{
#ifdef FEATURE_DUAL_SIM
  emm_tag_as_id(msg_ptr);
#endif
  emm_populate_mm_msg_info_outgoing_msg(msg_ptr);
  emm_add_message_to_debug_buffer(msg_ptr->id, FROM_EMM ,(sys_modem_as_id_e_type)mm_as_id);

  if(msgr_send(msg_ptr,msg_len)!= E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB,"=NAS= Failed to send umid %d", msg_ptr->id,0,0 );
  } 
}


/*===========================================================================
FUNCTION      EMM_REGISTRATION_ALLOWANCE_CHK

DESCRIPTION
  This function checks if a registration is allowed 
 
DEPENDENCIES
  None 

RETURN VALUE
  TRUE   -  If registration is allowed
  FALSE  -  If registration is not allowed
 
SIDE EFFECTS
  None
===========================================================================*/
boolean emm_registration_allowance_chk
(
  lte_rrc_service_ind_s     *rrc_service_ind,
  emm_ctrl_data_type        *emm_ctrl_data_ptr
)
{
  boolean reg_allowed = FALSE;
  lte_nas_tai_lst1_type current_tai ;
  
  
  switch (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
  {
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
        if(emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE)
           {
             reg_allowed = TRUE;
           }
           break;
      case SYS_NETWORK_SELECTION_MODE_MANUAL:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
        /* Create NAS current TAI */
        current_tai = emm_get_current_tai(rrc_service_ind) ;
        if(emm_is_manual_reg_allowed(current_tai, emm_ctrl_data_ptr) == TRUE)
        {
         reg_allowed = TRUE;
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_LIMITED:
#ifdef FEATURE_1XSRLTE
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
      case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
#endif 
        break;
      default:
        MSG_HIGH_DS_1(MM_SUB,"=EMM= Invalid NW Sel Mode in REG REQ - %d",
                      emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
        break;
  }
   MSG_HIGH_DS_1(MM_SUB,"=EMM= reg allowed - %d", reg_allowed);
  return reg_allowed;
}

/*===========================================================================
 
FUNCTION MM_SEND_CM_T3402_CHANGED_IND

DESCRIPTION
  This function sends ps data suspend indication to the cm

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_cm_t3402_changed_ind
(
  dword  t3402_value
)
{
  emm_t3402_changed_ind_s_type  t3402_changed_ind;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending NAS_EMM_T3402_CHANGED_IND");

  msgr_init_hdr((msgr_hdr_s*)&t3402_changed_ind.msg_hdr,
                MSGR_NAS_EMM, 
                NAS_EMM_T3402_CHANGED_IND);

  t3402_changed_ind.t3402_value = t3402_value;
  emm_msgr_send_error((msgr_hdr_s*)&t3402_changed_ind,
               sizeof(emm_t3402_changed_ind_s_type));

  MSG_HIGH_DS_1(MM_SUB,"=EMM= Sending NAS_EMM_T3402_CHANGED_IND T3402 value = %d milliseconds",
                     t3402_value);
  
}

/*=========================================================================== 
 
FUNCTION  EMM_SEND_TAI_LIST_IND

DESCRIPTION
  This function sends NAS_EMM_TAI_LIST_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_send_tai_list_ind
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
	emm_tai_list_ind_type  *tai_list_ind = NULL;
	IxErrnoType error_number = E_SUCCESS;
	
	ASSERT(emm_ctrl_data_ptr != NULL);
	ASSERT(emm_ctrl_data_ptr->emm_tai_list_ptr != NULL);
	
	tai_list_ind = (emm_tai_list_ind_type *) emm_modem_mem_alloc(sizeof(emm_tai_list_ind_type),
												emm_ctrl_data_ptr);
	mm_check_for_null_ptr((void*)tai_list_ind);
	
	msgr_init_hdr((msgr_hdr_s *)&tai_list_ind->msg_hdr,
				   MSGR_NAS_EMM, 
				   NAS_EMM_TAI_LIST_IND); 
	
	
	memscpy((void *)&tai_list_ind->tai_lst_info,sizeof(lte_nas_emm_tai_lst_info_type),
			(void *)emm_ctrl_data_ptr->emm_tai_list_ptr,sizeof(lte_nas_emm_tai_lst_info_type));
	

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending NAS_EMM_TAI_LIST_IND");

  emm_msgr_send((msgr_hdr_s *)tai_list_ind,
			(uint32)(sizeof(emm_tai_list_ind_type)));
  
  emm_modem_mem_free(tai_list_ind, emm_ctrl_data_ptr);
  

} /* emm_send_tai_list_ind */

/*===========================================================================
FUNCTION  EMM_SEND_T3402_CHANGED_IND

DESCRIPTION
  This function sends NAS_EMM_T3402_CHANGED_IND with T3402 value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_t3402_changed_ind
(
  dword  t3402_value
)
{
  emm_ds_t3402_changed_ind_s_type  t3402_changed_ind;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending NAS_EMM_T3402_CHANGED_IND");
  
  /* Init */
  memset((void *)&t3402_changed_ind, 0x0, sizeof(emm_ds_t3402_changed_ind_s_type));

  t3402_changed_ind.message_header.message_set = MS_MM_DS;
  t3402_changed_ind.message_header.message_id = (int) EMM_T3402_CHANGED_IND;

  t3402_changed_ind.t3402_value = t3402_value;
  
  PUT_IMH_LEN(sizeof( emm_t3402_changed_ind_s_type) - sizeof(IMH_T),
                &t3402_changed_ind.message_header );

   /* Add sent messages to the debug buffer */
  mm_add_message_to_debug_buffer(t3402_changed_ind.message_header.message_set,
                                 t3402_changed_ind.message_header.message_id,
                                 FROM_MM
                                 ,(sys_modem_as_id_e_type)mm_as_id
                                 );

  ds_pdn_mgr_nas_to_ds_msg_hdlr((void *)&t3402_changed_ind);

  MSG_HIGH_DS_1(MM_SUB,"=EMM= Sending NAS_EMM_T3402_CHANGED_IND T3402 value = %d milliseconds",
                     t3402_value);

  emm_send_cm_t3402_changed_ind(t3402_value);
  
} /* emm_send_t3402_changed_ind */

#ifdef FEATURE_CIOT
void emm_handle_timer_t3448(boolean  timer_start , dword t3448_value)
{
 emm_timer_t3448_status_ind_type  t3448_status;

 MSG_HIGH_DS_2(MM_SUB, "=EMM= Sending EMM_TIMER_T3448_STATUS_IND to DS timer_start %d T3448 value = %d milliseconds",timer_start,t3448_value);

   /* Init */
  memset((void *)&t3448_status, 0x0, sizeof(emm_timer_t3448_status_ind_type));

  t3448_status.message_header.message_set = MS_MM_DS;
  t3448_status.message_header.message_id = (int) EMM_TIMER_T3448_STATUS_IND;
  /*t3448 is non zero only for timer start cases*/ 
  t3448_status.timer_t3448_value = t3448_value;
  mm_stop_timer(TIMER_T3448);
  if(timer_start == TRUE)
  {
   t3448_status.status = EMM_TIMER_T3448_STATUS_START;
   mm_start_timer(TIMER_T3448, t3448_value); 
  }
  else
  {
   t3448_status.status = EMM_TIMER_T3448_STATUS_STOP;
  }
  PUT_IMH_LEN(sizeof( emm_timer_t3448_status_ind_type) - sizeof(IMH_T),
                &t3448_status.message_header );
     /* Add sent messages to the debug buffer */
  mm_add_message_to_debug_buffer(t3448_status.message_header.message_set,
                                 t3448_status.message_header.message_id,
                                 FROM_MM,
                                 (sys_modem_as_id_e_type)mm_as_id
                                 );

  ds_pdn_mgr_nas_to_ds_msg_hdlr((void *)&t3448_status);
}
#endif
/*===========================================================================
FUNCTION    emm_bit_mask_check

DESCRIPTION
  Function checks value passed to see if particular element  is set

DEPENDENCIES
  None

RETURN VALUE
  returns TRUE if particular bit is set, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

boolean emm_bit_mask_check
(
  uint32 value,
  byte bit_mask
)
{
  if((value & (1 << bit_mask)) > 0)
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION    emm_bit_mask_reset

DESCRIPTION
  Function resets a particular bit in the value feild.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/

void emm_bit_mask_reset
(
  uint32 *value_ptr,
  byte bit_mask
)
{
  *value_ptr = *value_ptr & (~(1 << bit_mask));
}


#ifdef FEATURE_NAS_GW
/*===========================================================================
FUNCTION    emm_is_srvcc_supported

DESCRIPTION
  This function checks if UE supports SRVCC to GERAN or UMTS with
  the current configuration and EFS settings

DEPENDENCIES
  None

RETURN VALUE
  TRUE if SRVCC is supported, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/

boolean emm_is_srvcc_supported
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{

#ifdef FEATURE_CIOT
    return FALSE;
#endif
  /*
  ** Return TRUE if UE rat priority list has
  **  a) Umts or
  **  b) GSM and l2g_srvcc_capability_support efs is set to TRUE
  */
  if((emm_ctrl_data_ptr->srvcc_capability_support == TRUE) &&
     ((mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_UMTS_RADIO_ACCESS) == TRUE) || 
      ((mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_GSM_RADIO_ACCESS) == TRUE) && 
       (emm_ctrl_data_ptr->l2g_srvcc_capability_support == TRUE))))
  {
    return TRUE;
  }
  return FALSE;
}
#endif

#ifdef FEATURE_LTE_TO_1X
/*===========================================================================
FUNCTION    emm_is_1xsrvcc_supported

DESCRIPTION
  This function checks if UE supports SRVCC to 1x with
  the current configuration and EFS settings

DEPENDENCIES
  None

RETURN VALUE
  TRUE if SRVCC is supported, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/

boolean emm_is_1xsrvcc_supported
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  /*
  ** Return TRUE if UE rat priority list has 1x in RAT list and 1xsrvcc 
  ** is set through NV
  */
  uint32 i;
   
  if(emm_ctrl_data_ptr->onexsrvcc_capability_support == TRUE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= 1xsrvcc is supported in NV");
    for (i = 0; i < emm_ctrl_data_ptr->reg_req_pending_ptr->rat_pri_list_info.num_items; i++)
    {
      if(emm_ctrl_data_ptr->reg_req_pending_ptr->rat_pri_list_info.priority_list_info[i].acq_sys_mode == 
                                          SYS_SYS_MODE_CDMA)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= 1xsrvcc is supported");
        return TRUE;
      }
    }
  }
  MSG_HIGH_DS_0(MM_SUB, "=EMM= 1xsrvcc is not supported");
  return FALSE;
}
#endif


/*===========================================================================
FUNCTION    EMM_SET_TRM_PRIORITY

DESCRIPTION
  Set the TRM PRIORTIY 
DEPENDENCIES
  None

RETURN VALUE
 None 

SIDE EFFECTS
  None
===========================================================================*/
void emm_set_trm_priority
(
  lte_trm_priority_e   trm_priority
)
{
#if defined  FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = emm_db_get_ctrl_data();
   

  emm_ctrl_data_ptr->lte_trm_priority = trm_priority;
#else
  (void)trm_priority;
#endif 
 
}

#if defined  FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
/*===========================================================================
FUNCTION    emm_get_lte_trm_priority

DESCRIPTION
  This function returns the TRM priority as HIGH is Attach or CSFB is going on. 

DEPENDENCIES
  None

RETURN VALUE
TRM priority

SIDE EFFECTS
  None
===========================================================================*/
lte_trm_priority_e emm_get_lte_trm_priority
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  
  lte_trm_priority_e   trm_priority = LTE_TRM_PRIORITY_LOW;
  emm_state_type  emm_state; 

   

  emm_state = EMM_GET_STATE();

  if(((emm_state == EMM_REGISTERED_INITIATED) || 
      (emm_ctrl_data_ptr->reg_complete_ack_pending == EMM_PENDING_REG_ACK_ATTACH_COMPLETE) 
#ifdef FEATURE_DUAL_SIM
      || (emm_ctrl_data_ptr->tau_trm_priority == TRUE)
#endif
      )
#ifdef FEATURE_1XSRLTE
    &&!(((emm_get_current_ue_mode() == SYS_UE_MODE_1XSRLTE_ONLY )||
        (emm_get_current_ue_mode() == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED))&&
        (emm_ctrl_data_ptr->current_ue_submode_srlte == TRUE))
#endif
     )
  {
    trm_priority = LTE_TRM_PRIORITY_HIGH;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Setting TRM priortiy as HIGH due to Attach or Irat redir TAU procedure");
  }
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
  else if(emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE)
  {
    trm_priority = LTE_TRM_PRIORITY_HIGH;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Setting TRM priortiy as HIGH due to CSFB procedure");
  }
#endif
  return trm_priority;
}


/*===========================================================================
FUNCTION    emm_update_lte_trm_priority

DESCRIPTION
  This function re-evaluate TRM priorty and send update to LRRC if required 

DEPENDENCIES
  None

RETURN VALUE
TRM priority

SIDE EFFECTS
  None
===========================================================================*/
void emm_update_lte_trm_priorty
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  lte_trm_priority_e   trm_priority;

   

  trm_priority = emm_get_lte_trm_priority(emm_ctrl_data_ptr);

  if(trm_priority != emm_ctrl_data_ptr->lte_trm_priority)
  {
    emm_ctrl_data_ptr->lte_trm_priority = trm_priority;
    emm_send_rrc_trm_priority(trm_priority);
  }

  return;
}


#endif 


/*===========================================================================
FUNCTION    emm_set_tau_trm_priority

DESCRIPTION
  This function sets the TAU TRM priority and updates the access stratum if TRM priority is chaning to low

DEPENDENCIES
  None

RETURN VALUE
TRM priority

SIDE EFFECTS
  None
===========================================================================*/
void emm_set_tau_trm_priority
(
  boolean tau_trm_priority
)
{
#ifdef FEATURE_DUAL_SIM
  emm_ctrl_data_type *emm_ctrl_data_ptr = emm_db_get_ctrl_data();
   
  if(emm_ctrl_data_ptr->tau_trm_priority != tau_trm_priority)
  {
    MSG_HIGH_DS_2(MM_SUB, "=EMM= Set TAU TRM priortiy from %d to %d",
                emm_ctrl_data_ptr->tau_trm_priority, tau_trm_priority);
  
    emm_ctrl_data_ptr->tau_trm_priority = tau_trm_priority;
    if(tau_trm_priority == FALSE)
    {
      emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
    }
  }
#else
  (void) tau_trm_priority;
#endif
}


#ifdef FEATURE_1XSRLTE
/*===========================================================================
FUNCTION    emm_ue_is_in_srlte_state

DESCRIPTION
  This function checks if srlte esr is allowed

DEPENDENCIES
  None

RETURN VALUE
  TRUE if it is allowed, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_ue_is_in_srlte_state(emm_ctrl_data_type   *emm_ctrl_data_ptr)
{

  
  if((emm_get_current_ue_mode() == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED)||
     (emm_get_current_ue_mode() == SYS_UE_MODE_1XSRLTE_ONLY))
  {
     if(emm_ctrl_data_ptr->current_ue_submode_srlte == TRUE)
     {
       return TRUE;
     }
  }
  return FALSE;

}
#endif

#if defined FEATURE_1XSRLTE||defined FEATURE_SGLTE
/*===========================================================================
FUNCTION    emm_update_sub_ue_mode

DESCRIPTION
  This function update ue submode 
 
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void emm_update_sub_ue_mode(boolean   current_ue_submode_srlte)
{
  emm_ctrl_data_type  *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  
  emm_ctrl_data_ptr->current_ue_submode_srlte = current_ue_submode_srlte;
}
#endif

/*===========================================================================
FUNCTION    emm_get_tmsi_based_nri

DESCRIPTION
  This function generate tmsi based nri

DEPENDENCIES
  None

RETURN VALUE
gmm_ie_tmsi_based_NRI_T

SIDE EFFECTS
  None
===========================================================================*/
gmm_ie_tmsi_based_NRI_T emm_get_tmsi_based_nri(emm_ctrl_data_type   *emm_ctrl_data_ptr)
{
  gmm_ie_tmsi_based_NRI_T tmsi_based_NRI;
   
  tmsi_based_NRI.length = 2;
  /* Populate TMSI bit 23 to 17 to Octet3 from bit 8 to bit 2*/
  tmsi_based_NRI.tmsi_based_nri_value[0] = emm_ctrl_data_ptr->emm_location_area->tmsi[2] << 1;
 /* Populate TMSI bit 16 to Octet3 bit 1*/
  tmsi_based_NRI.tmsi_based_nri_value[0] |= emm_ctrl_data_ptr->emm_location_area->tmsi[1] >> 7;
 /* Populate TMSI bit 14,15 to Octet4 bit 7 and 8*/
  tmsi_based_NRI.tmsi_based_nri_value[1] = (emm_ctrl_data_ptr->emm_location_area->tmsi[1] << 1) & 0xC0;
  return tmsi_based_NRI;
}

#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

/*===========================================================================
FUNCTION  EMM_IS_REGISTRATION_FOR_EMC_PDN_CONN

DESCRIPTION
  This function checks whether registration is for initiating PDN connection
  for emergency bearer services

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean  emm_is_registration_for_emc_pdn_conn
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  

  if((reg_service_report == emm_send_reg_cnf) &&
     ((emm_ctrl_data_ptr->reg_req_pending_ptr->type == 
                                                MMR_REG_REQ_SRV_MODE_UPDATE)||
      (full_srv_req_in_emerg == TRUE)) &&
     (emm_bit_mask_check(emm_ctrl_data_ptr->reg_req_pending_ptr->additional_info,
                                   (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE))
  {
    return TRUE;
  }

  return FALSE;

}/* emm_is_registration_for_emc_pdn_conn */

/*===========================================================================
FUNCTION  EMM_SEND_DEACT_NON_EMC_BEARER_IND

DESCRIPTION
  This function sends NAS_EMM_DEACT_NON_EMC_BEARER_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void emm_send_deact_non_emc_bearer_ind
(
  emm_ctrl_data_type        *emm_ctrl_data_ptr
)
{
  emm_deact_non_emc_bearer_ind_s_type  deact_non_emc_bearer_ind;
  //IxErrnoType error_number = E_SUCCESS;

  MSG_HIGH_DS_0(MM_SUB,"=EMM= Sending NAS_EMM_DEACT_NON_EMC_BEARER_IND");
  
  /* Init */
  memset((void *)&deact_non_emc_bearer_ind, 0x0, sizeof(emm_deact_non_emc_bearer_ind_s_type));

  deact_non_emc_bearer_ind.message_header.message_set = MS_MM_DS;
  deact_non_emc_bearer_ind.message_header.message_id = (int) EMM_DEACT_NON_EMC_BEARER_IND;

  
  PUT_IMH_LEN(sizeof( emm_deact_non_emc_bearer_ind_s_type) - sizeof(IMH_T),
                &deact_non_emc_bearer_ind.message_header );

   /* Add sent messages to the debug buffer */
  mm_add_message_to_debug_buffer(deact_non_emc_bearer_ind.message_header.message_set,
                                 deact_non_emc_bearer_ind.message_header.message_id,
                                 FROM_MM
                                 ,(sys_modem_as_id_e_type)mm_as_id
                                 );

  ds_pdn_mgr_nas_to_ds_msg_hdlr((void *)&deact_non_emc_bearer_ind);
  {
    emm_ctrl_data_ptr->non_emc_bearer_deact_in_progress = TRUE;
  }
  MSG_HIGH_DS_1(MM_SUB,"=EMM= Sending NAS_EMM_DEACT_NON_EMC_BEARER_IND Success(0)/Failure(non Zero) non_emc_bearer_deact_in_progress = %d",
                        emm_ctrl_data_ptr->non_emc_bearer_deact_in_progress);

}


#endif /* FEATURE_LTE_REL9 */
#endif

void emm_send_diag_event(event_id_enum_type event,nas_event_end_result end_result,nas_event_end_reason end_reason,lte_nas_emm_cause_type cause)
{
  
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  byte event_size = 6;
  byte diag_event[6];
#else
  byte event_size = 5;
  byte diag_event[5];
#endif

  
  emm_ctrl_data_type  *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  memset((void *)&diag_event[0],0x00,event_size);
  


  switch (event)
  {
    case EVENT_NAS_ATTACH:

      diag_event[0] = (byte)emm_ctrl_data_ptr->plmn_service_state->active_rat;
      diag_event[1] = emm_ctrl_data_ptr->eps_attach_type;
      diag_event[2] = (byte)end_result;
      diag_event[3] = (byte)end_reason;
      diag_event[4] = (byte)cause;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
      diag_event[5] = mm_as_id; 
#endif    
      event_report_payload(EVENT_NAS_ATTACH,(uint8)event_size,(void*)diag_event);

    
      break;

    case EVENT_NAS_TAU:

      diag_event[0] = (byte)emm_ctrl_data_ptr->plmn_service_state->active_rat;
      if ((nas_emm_tau_cs_rej == TRUE) &&
         (nas_emm_tau_cs_rej_tau_type == COMBINED_TA_LA_UPDATING ||
          nas_emm_tau_cs_rej_tau_type == COMBINED_TA_LA_UPDATING_IMSI_ATTACH))
      {
        diag_event[1] = (byte)nas_emm_tau_cs_rej_tau_type;
        nas_emm_tau_cs_rej = FALSE;
        nas_emm_tau_cs_rej_tau_type = TA_UPDATING;
      }
      else
      {
        diag_event[1] = (byte)emm_ctrl_data_ptr->emm_ongoing_eps_ta_update;
      }
      diag_event[2] = (byte)end_result;
      diag_event[3] = (byte)end_reason;
      diag_event[4] = (byte)cause;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
      diag_event[5] = mm_as_id; 
#endif  
      event_report_payload(EVENT_NAS_TAU,(uint8)event_size,(void*)diag_event);   

      break;

    case EVENT_NAS_MO_DETACH:
    
      diag_event[0] = (byte)emm_ctrl_data_ptr->plmn_service_state->active_rat;
      if (emm_ctrl_data_ptr->emm_substate == EMM_REGISTERED_IMSI_DETACH_INITIATED)
      {
        diag_event[1] = (byte)IMSI_DETACH_MO_DETACH;
      }

      else if (emm_ctrl_data_ptr->detach_reason == SWITCH_OFF)
      {
        diag_event[1] = (byte)nas_emm_power_off_detach_type;
      }
      else 
      {
        diag_event[1] = (byte)emm_ctrl_data_ptr->ongoing_detach_type;
      }
      diag_event[2] = (byte)end_result;
      diag_event[3] = (byte)end_reason;
      diag_event[4] = (byte)cause;   

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
      diag_event[5] = mm_as_id; 
#endif  
      event_report_payload(EVENT_NAS_MO_DETACH,(uint8)event_size,(void*)diag_event);  

      break;

    case EVENT_NAS_MT_DETACH:

      diag_event[0] = (byte)emm_ctrl_data_ptr->plmn_service_state->active_rat;
      diag_event[1] = (byte)emm_ctrl_data_ptr->mt_detach_info.type;
      diag_event[2] = (byte)end_result;
      diag_event[3] = (byte)end_reason;
      diag_event[4] = (byte)cause;  
 
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
      diag_event[5] = mm_as_id; 
#endif  
      event_report_payload(EVENT_NAS_MT_DETACH,(uint8)event_size,(void*)diag_event); 

      break;

    default:
      break;
  }
  MSG_HIGH_DS_6(MM_SUB, "EMM: NAS procedure %d RAT %d procedure type %d End result %d End reason %d Cause sent %d ",
                event, diag_event[0], diag_event[1],diag_event[2], diag_event[3], diag_event[4]);
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  MSG_HIGH_DS_1(MM_SUB,"AS_ID %d \n",diag_event[5]);
#endif 
}

/*===========================================================================

FUNCTION  emm_get_call_mode

DESCRIPTION: This function is call to get the call mode. This is done in replacement of policyman api
policyman_get_call_mode. 

DEPENDENCIES
  None

RETURN VALUE
  sys_call_mode_t

SIDE EFFECTS
  None
===========================================================================*/

sys_call_mode_t emm_get_call_mode(void)
{
  return SYS_CALL_MODE_NORMAL;
}

/*===========================================================================

FUNCTION  emm_skip_emc_retry

DESCRIPTION: This function is call to check if EMC retry should be skipped or not 
based on cs rat availability and NV lteonly_mcc and plmn is in whitelist or not 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

boolean emm_skip_emc_retry(void)
{
  emm_ctrl_data_type        *emm_ctrl_data_ptr = NULL;
  sys_plmn_id_s_type        selected_plmn;
  sys_call_mode_t      e911_call_mode;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  

  selected_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  e911_call_mode = emm_get_call_mode();

  if((e911_call_mode == SYS_CALL_MODE_VOLTE_ONLY)||
     (emm_ctrl_data_ptr->cs_rat_available == FALSE))
  {
    MSG_HIGH_DS_0(MM_SUB,"=EMM=Do not skip EMC retry");
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

#if defined  FEATURE_DUAL_SIM 
/*===========================================================================

FUNCTION  emm_skip_emc_retry_per_subs

DESCRIPTION: This function is call to check if EMC retry should be skipped or not 
based on cs rat availability and NV lteonly_mcc and plmn is in whitelist or not 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_skip_emc_retry_per_subs(sys_modem_as_id_e_type sub_id)
{
  
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  sys_plmn_id_s_type        selected_plmn;
  sys_call_mode_t      e911_call_mode;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data_per_subs(sub_id);

  selected_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  e911_call_mode = emm_get_call_mode();

  if((e911_call_mode == SYS_CALL_MODE_VOLTE_ONLY) ||
     (emm_ctrl_data_ptr->cs_rat_available == FALSE))
  {
    MSG_HIGH_DS_0(MM_SUB,"=EMM=Do not skip EMC retry");
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}
#endif
#endif
#endif
/*===========================================================================
FUNCTION  emm_is_s1_mode_only_3gpp

DESCRIPTION
  This function checks whether UE supports S1 mode only for 3gpp rats

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_NAS_GW
boolean  emm_is_s1_mode_only_3gpp
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  

  if((mm_checkif_rat_pri_list_has_lte_rat(&mm_rat_pri_list_info) == TRUE) &&
      (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_UMTS_RADIO_ACCESS) == FALSE) &&
      (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_GSM_RADIO_ACCESS) == FALSE) && 
      (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_TDS_RADIO_ACCESS) == FALSE))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM=UE is s1 mode only for 3GPP rats");
    return TRUE;
  }
  return FALSE;
}/* emm_is_s1_mode_only_3gpp */
#endif
/*===========================================================================
FUNCTION  emm_is_s1_mode_only

DESCRIPTION
  This function checks whether rat priority list has LTE only.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean  emm_is_s1_mode_only
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  if((mm_rat_pri_list_info.num_items == 1) &&
     ((mm_rat_pri_list_info.priority_list_info[0].acq_sys_mode == SYS_SYS_MODE_LTE)||
      (mm_rat_pri_list_info.priority_list_info[0].acq_sys_mode == SYS_SYS_MODE_LTE_M1)||
      (mm_rat_pri_list_info.priority_list_info[0].acq_sys_mode == SYS_SYS_MODE_LTE_NB1)))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM=UE is LTE only mode");
    return TRUE;
  }
  return FALSE;
}/* emm_is_s1_mode_only */

/*===========================================================================
FUNCTION  emm_read_carrier_specific_efs

DESCRIPTION
  This function reads carrier specific EFS/NV values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_read_carrier_specific_efs(void)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
#ifdef FEATURE_LTE_REL9
  byte efs_val = 0;
#endif
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

   /*GMM DRX CN COEFF S1 */
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/nas/gmm_drx_cn_coeff_s1",
                          &gmm_nv_drx_cn_coeff_s1,sizeof(sys_drx_cn_coefficient_s1_e_type),MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_as_id);

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= gmm_drx_cn_coeff_s1 = %d", gmm_nv_drx_cn_coeff_s1);
  }
  else 
  {
    gmm_nv_drx_cn_coeff_s1 = SYS_DRX_NOT_SPECIFIED_BY_MS;
    MSG_HIGH_DS_1(MM_SUB, "=MM= Unable to read NV for gmm_drx_cn_coeff_s1 , status = %d", status);
  }

  if((gmm_nv_drx_cn_coeff_s1 == SYS_DRX_NOT_SPECIFIED_BY_MS) ||
     (gmm_nv_drx_cn_coeff_s1 >= SYS_DRX_CN_COEFFICIENT_S1_CN6_T32 && 
      (gmm_nv_drx_cn_coeff_s1 <= SYS_DRX_CN_COEFFICIENT_S1_CN9_T256)))
  {
    gmm_drx_cn_coeff_s1 = gmm_nv_drx_cn_coeff_s1;
  }
  else
  {
    gmm_nv_drx_cn_coeff_s1 = SYS_DRX_NOT_SPECIFIED_BY_MS; 
    gmm_drx_cn_coeff_s1 = gmm_nv_drx_cn_coeff_s1;
  }

  /* ATTACH CTRL TIMER */
#ifdef FEATURE_GSTK
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/uim/gstk/attach_ctrl_timer", 
                        &emm_ctrl_data_ptr->attach_ctrl_timer, 
                        sizeof(emm_ctrl_data_ptr->attach_ctrl_timer),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'ATTACH_CTRL_TIMER' is %u",
                  emm_ctrl_data_ptr->attach_ctrl_timer);
  }
  else
  {
    emm_ctrl_data_ptr->attach_ctrl_timer = 0x00;
    MSG_HIGH_DS_2(MM_SUB,"=EMM= 'ATTACH_CTRL_TIMER' file could not be read from EFS %d. default %u",
                  status, emm_ctrl_data_ptr->attach_ctrl_timer);
  }
#endif

#ifdef FEATURE_NAS_GW
  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);
  
  status = efs_get("/nv/item_files/modem/nas/volte_rej_mt_csfb", 
                              &emm_ctrl_data_ptr->volte_rej_mt_csfb, 
                              sizeof(boolean));

  if(status == sizeof(boolean))
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'volte_rej_mt_csfb' is %d",emm_ctrl_data_ptr->volte_rej_mt_csfb);
  }
  else
  {
    emm_ctrl_data_ptr->volte_rej_mt_csfb = FALSE;
    MSG_HIGH_DS_2(MM_SUB,"=EMM= 'volte_rej_mt_csfb' file could not be read from EFS, status %d set defaul valuse as %d", status, emm_ctrl_data_ptr->volte_rej_mt_csfb);  
  }
#endif
  /* TEMP FPLMN BACKOFF TIME */
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/nas/lte_nas_temp_fplmn_backoff_time",
                        &emm_ctrl_data_ptr->temp_fplmn_backoff_time,
                        sizeof(dword),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= Setting temp fplmn backoff timer value to %d (ms)", 
                  emm_ctrl_data_ptr->temp_fplmn_backoff_time);
  }
  else
  {
    emm_ctrl_data_ptr->temp_fplmn_backoff_time = EMM_TEMP_FPLMN_DISABLED;
    MSG_HIGH_DS_2(MM_SUB ,"=EMM= Unable to read temp fplmn timer %d. default %d",
                  status, emm_ctrl_data_ptr->temp_fplmn_backoff_time);
  }

  /* SRLTE ESR TIME */
#ifdef FEATURE_1XSRLTE
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/nas/lte_nas_srlte_esr_time",
                        &emm_ctrl_data_ptr->srlte_esr_time,
                        sizeof(dword),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= Setting srlte esr timer value to %d (ms)", 
                  emm_ctrl_data_ptr->srlte_esr_time);
  }
  else
  {
    emm_ctrl_data_ptr->srlte_esr_time = SRLTE_ESR_TIMER;/*In msec*/
    MSG_HIGH_DS_2(MM_SUB ,"=EMM= Unable to read srlte esr timer %d. default %d",
                  status, emm_ctrl_data_ptr->srlte_esr_time);
  }
#endif

#ifdef FEATURE_NAS_GW
  /* MM LAI CHANGE FORCE LAU FOR EMERGENCY */
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/nas/nas_lai_change_force_lau_for_emergency", 
                        &mm_lai_change_force_lau_for_emergency,
                        sizeof(boolean),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= setting mm_lai_change_force_lau_for_emergency to %d",
                                       mm_lai_change_force_lau_for_emergency);
  }
  else
  {
    mm_lai_change_force_lau_for_emergency = FALSE;
    MSG_HIGH_DS_2(MM_SUB,"=MM= mm_lai_change_force_lau_for_emergency file could not be read from EFS %d. default %d",
                          status,mm_lai_change_force_lau_for_emergency);
  }
#endif
  /* AGGRESSION MANAGEMENT */
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/nas/aggression_management", 
                        &emm_ctrl_data_ptr->aggression_management, 
                        sizeof(boolean),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Aggression management feature setting - %d",
                  emm_ctrl_data_ptr->aggression_management);
  }
  else
  {
    emm_ctrl_data_ptr->aggression_management = FALSE;
    MSG_HIGH_DS_2(MM_SUB,"=MM= aggression_management file could not be read from EFS %d. default %d",
                  status, emm_ctrl_data_ptr->aggression_management);
  }

#ifdef FEATURE_NAS_GW
  /* SGLTE DEVICE */
  dog_hb_report(DOG_MM_RPT);
  status  = mcfg_fs_read("/mmode/cmph/sglte_device", 
                         &emm_ctrl_data_ptr->is_sglte_device, 
                         sizeof(boolean),
                         MCFG_FS_TYPE_EFS,
                         (mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= efs sglte_device %d",emm_ctrl_data_ptr->is_sglte_device);
  }
  else
  {
    emm_ctrl_data_ptr->is_sglte_device = FALSE;
    MSG_HIGH_DS_2(MM_SUB,"=EMM= sglte_device file could not be read from EFS %d. default %d",
                  status, emm_ctrl_data_ptr->is_sglte_device);
  }
#endif



#ifdef FEATURE_LTE_REL9
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
  /* MM VOIMS DM MO */
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/nas/mobility_management_for_voims_feature", 
                        &emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.mm_voims_dm_mo,
                        sizeof(boolean),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= setting mm_voims_dm_mo to %d",
               emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.mm_voims_dm_mo);
  }
  else
  {
    emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.mm_voims_dm_mo = FALSE;
    MSG_HIGH_DS_2(MM_SUB,"=MM= mm_voims_dm_mo file could not be read from EFS %d. default %d",
                  status, emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.mm_voims_dm_mo);
  }
#endif

  /* LPP SUPPORTED */
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/gps/cgps/sm/gnss_lpp_enable", 
                        &efs_val, 
                        sizeof(byte),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status == MCFG_FS_STATUS_OK)
  {
    emm_ctrl_data_ptr->lpp_supported = (0x02 & efs_val) >> 1 ; //Bit 1  LPP CP Enable/Disabled
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= LPP is %d", emm_ctrl_data_ptr->lpp_supported);
  }
  else
  {
    emm_ctrl_data_ptr->lpp_supported = FALSE;
    MSG_HIGH_DS_2(MM_SUB,"=EMM= LPP file could not be read from EFS %d. default %d",
                  status, emm_ctrl_data_ptr->lpp_supported);
  }
#endif

#ifdef FEATURE_SGLTE
  /* SRVCC OVER SGLTE */
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/lte/rrc/cap/srvcc_over_sglte",
                        &emm_ctrl_data_ptr->srvcc_over_sglte_support,
                        sizeof(boolean),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);

  if (status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB, "EMM: Read srvcc_over_sglte from EFS: %d ",
                  emm_ctrl_data_ptr->srvcc_over_sglte_support);
  }
  else
  {
    emm_ctrl_data_ptr->srvcc_over_sglte_support = FALSE;
    MSG_HIGH_DS_2(MM_SUB,"EMM: srvcc_over_sglte support file read failed %d. default %d",
                  status, emm_ctrl_data_ptr->srvcc_over_sglte_support);
  }
#endif

  /* LTE CONNECTION CONTROL */
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/lte_connection_control", 
                        &emm_ctrl_data_ptr->lte_connection_control, 
                        sizeof(byte),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= setting lte_connection_control to %d",
                  emm_ctrl_data_ptr->lte_connection_control);
  }
  else
  {
    emm_ctrl_data_ptr->lte_connection_control = 0;
    MSG_HIGH_DS_2(MM_SUB,"=EMM= lte_connection_control file could not be read from EFS %d. default %d",
                  status, emm_ctrl_data_ptr->lte_connection_control);
  }

#if defined(FEATURE_NAS_GW)
  /* IGNORE MT CSFB DURING VOLTE CALL */
  dog_hb_report(DOG_MM_RPT);
  status = mcfg_fs_read("/nv/item_files/modem/nas/lte_nas_ignore_mt_csfb_during_volte_call", 
                        &emm_ctrl_data_ptr->ignore_mt_csfb_during_volte_call, 
                        sizeof(boolean),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);  

  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=EMM= 'lte_nas_ignore_mt_csfb_during_volte_call' is %d",
                  emm_ctrl_data_ptr->ignore_mt_csfb_during_volte_call);
  }
  else
  {
    emm_ctrl_data_ptr->ignore_mt_csfb_during_volte_call = FALSE;
    MSG_HIGH_DS_2(MM_SUB,"=EMM= 'lte_nas_ignore_mt_csfb_during_volte_call' file could not be read from EFS %d. default = %d",
                  status, emm_ctrl_data_ptr->ignore_mt_csfb_during_volte_call);
  }
#endif
    dog_hb_report(DOG_MM_RPT);
    status = mcfg_fs_read("/nv/item_files/modem/nas/map_cause_17_to_cause_18", 
                        &emm_ctrl_data_ptr->emm_map_cause_17_to_cause_18, 
                        sizeof(boolean),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);  
    if(status != MCFG_FS_STATUS_OK)
    {
      emm_ctrl_data_ptr->emm_map_cause_17_to_cause_18 = FALSE;
    }


    MSG_HIGH_DS_2(MM_SUB,"=EMM= emm_map_cause_17_to_cause_18 efs status = %d value = %d ",status,emm_ctrl_data_ptr->emm_map_cause_17_to_cause_18);
	
    dog_hb_report(DOG_MM_RPT);
  
    status = mcfg_fs_read("/nv/item_files/modem/nas/mt_detach_abnormal_handling", 
                        &emm_ctrl_data_ptr->emm_mt_detach_abnormal_handling, 
                        sizeof(boolean),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);  

    if(status != MCFG_FS_STATUS_OK)
    {
      emm_ctrl_data_ptr->emm_mt_detach_abnormal_handling = FALSE;
    }
    MSG_HIGH_DS_2(MM_SUB,"=EMM= emm_mt_detach_abnormal_handling (status = %d value = %d)", status, emm_ctrl_data_ptr->emm_mt_detach_abnormal_handling);

#ifdef FEATURE_CIOT
    dog_hb_report(DOG_MM_RPT);
    status = mcfg_fs_read("/nv/item_files/modem/nas/ciot_irat_tau_mobility", 
                          &emm_ctrl_data_ptr->ciot_irat_tau_mobility, 
                          sizeof(boolean),
                          MCFG_FS_TYPE_EFS,
                          (mcfg_fs_sub_id_e_type)mm_sub_id);

    if (status == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= ciot_irat_tau_mobility feature setting - %d",
                    emm_ctrl_data_ptr->ciot_irat_tau_mobility);
    }
    else
    {
      emm_ctrl_data_ptr->ciot_irat_tau_mobility = TRUE;
      MSG_HIGH_DS_2(MM_SUB,"=MM= ciot_irat_tau_mobility file could not be read from EFS %d. default %d",
                    status, emm_ctrl_data_ptr->ciot_irat_tau_mobility);
    }
#endif

    dog_hb_report(DOG_MM_RPT);
    if((status = mcfg_fs_read("/nv/item_files/modem/nas/emm_reset_ul_cnt_cpsr", 
                              &emm_ctrl_data_ptr->reset_nas_ul_cnt_rel15, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id )) != MCFG_FS_STATUS_OK)
    {
      emm_ctrl_data_ptr->reset_nas_ul_cnt_rel15 = TRUE;
    }

    MSG_HIGH_DS_2(MM_SUB ,"=MM=  status = %d emm_reset_ul_cnt_cpsr %d",status,emm_ctrl_data_ptr->reset_nas_ul_cnt_rel15);

#ifdef FEATURE_FMD_SPEED_INFO
	/* Report to the Dog HB */
		 dog_hb_report(DOG_MM_RPT);
		 if((status = mcfg_fs_read("/nv/item_files/modem/nas/fmd_force_local_detach", 
					 &emm_ctrl_data_ptr->fmd_force_local_detach, 
					 sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id )) != MCFG_FS_STATUS_OK)
		  {
			emm_ctrl_data_ptr->fmd_force_local_detach = FALSE;
		  }
		  MSG_HIGH_DS_2(MM_SUB,"=EMM= Force local detach status %d enabled  %d)",
								 status, emm_ctrl_data_ptr->fmd_force_local_detach);
		  
		  /* Report to the Dog HB */
		  dog_hb_report(DOG_MM_RPT);	  
		  if((status = mcfg_fs_read("/nv/item_files/modem/nas/fmd_psm_fallback_timer", 
					 &emm_ctrl_data_ptr->fmd_psm_fallback_timer, 
					 sizeof(uint8), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id )) != MCFG_FS_STATUS_OK)
		  {
			emm_ctrl_data_ptr->fmd_psm_fallback_timer = 0;
		  }
		  MSG_HIGH_DS_2(MM_SUB,"=EMM= PSM fallback timer status %d value  %d)",
								 status,emm_ctrl_data_ptr->fmd_psm_fallback_timer);
#endif
}

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION  emm_process_mcfg_refresh

DESCRIPTION
  This function processes MCFG NV refresh indication for EMM specific EFS/NVs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_mcfg_refresh(void)
{
  emm_read_carrier_specific_efs();

  MSG_HIGH_DS_0(MM_SUB,"EMM Refresh Done");
}
#endif

#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
/*===========================================================================

FUNCTION  emm_ignore_cs_page_service_notification

DESCRIPTION: This function is called by EMM when it receives MT page or CS service nf

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_ignore_cs_page_cs_service_notification(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
    if((emm_ctrl_data_ptr->ignore_mt_csfb_during_volte_call == TRUE)&&
       ((emm_ctrl_data_ptr->mmtel_call_status.video_call_status == TRUE)||
       (emm_ctrl_data_ptr->mmtel_call_status.voice_call_status == TRUE)))
    {
      return TRUE;
    }
    return FALSE;
}

/*===========================================================================

FUNCTION  emm_reject_cs_page_service_notification

DESCRIPTION: This function is called by EMM when it receives MT page or CS service nf

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_reject_cs_service_notification(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
    if((emm_ctrl_data_ptr->volte_rej_mt_csfb == TRUE)&&
       ((emm_ctrl_data_ptr->mmtel_call_status.video_call_status == TRUE)||
       (emm_ctrl_data_ptr->mmtel_call_status.voice_call_status == TRUE)))
    {
      MSG_HIGH_DS_3(MM_SUB,"volte_rej_mt_csfb %d,video_call_status %d, voice_call_status%d",
                    emm_ctrl_data_ptr->volte_rej_mt_csfb,emm_ctrl_data_ptr->mmtel_call_status.video_call_status,
                    emm_ctrl_data_ptr->mmtel_call_status.voice_call_status);
      return TRUE;
    }
    return FALSE;
}

/*===========================================================================

FUNCTION  emm_send_esr_csfb_reject_msg

DESCRIPTION: This function is called by EMM when it receives CS service nf and volte call 
             start indication is active 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_esr_csfb_reject_msg
(
  emm_ctrl_data_type        *emm_ctrl_data_ptr
)
{
  lte_nas_emm_extended_service_req  *emm_extended_service_request  = NULL;
  emm_security_context_info *context_ptr = NULL;
  memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
         0x0,
         sizeof(lte_nas_outgoing_msg_type)) ;
  emm_extended_service_request = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.ext_service_req;
  
  memscpy((void*)&emm_extended_service_request->m_tmsi[0],
         sizeof(emm_extended_service_request->m_tmsi),
         (void*)&emm_ctrl_data_ptr->emm_guti.m_tmsi[0],
         sizeof(emm_ctrl_data_ptr->emm_guti.m_tmsi));

  /* Fill out the fields of service request message */
  emm_extended_service_request->lte_nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES;
  emm_extended_service_request->lte_nas_hdr.msg_id = EXT_SERVICE_REQUEST;
  emm_extended_service_request->service_type = LTE_MT_CSFB_1XCSFB;
  emm_extended_service_request->csfb_resp = CSFB_REJECT;
    /* Populate NAS key set identifier */
  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  if( context_ptr != NULL )
  {
    /* Find the type of context i.e mapped or native*/
    if(context_ptr->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS)
    {
      emm_extended_service_request->nas_key_set_identifier.security_context = 0;
    }
    else if (context_ptr->context_type == CONTEXT_TYPE_MAPPED_EPS)
    {
     emm_extended_service_request->nas_key_set_identifier.security_context = 1;
    } 
    else 
    { 
      MSG_FATAL_DS(MM_SUB,"=EMM= Security Context should be valid at the time of sending ESR. Current Security Contex=%d",context_ptr->context_type,0,0);    
    } 

    emm_extended_service_request->nas_key_set_identifier.keyset_id = context_ptr->nasKSI.keyset_id;
    MSG_HIGH_DS_2(MM_SUB,"=EMM= EXT_SERV_REQ with current context type %d, KSI %d " , 
                  context_ptr->context_type,
                  emm_extended_service_request->nas_key_set_identifier.keyset_id);
  }
#ifdef FEATURE_LTE_REL9
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
  {
    emm_extended_service_request->eps_bearer_context_status_exists = TRUE;
    emm_extended_service_request->eps_bearer_context_status = 
                                       emm_ctrl_data_ptr->eps_bearer_context_status;
  }
#endif
  (void)emm_send_message( emm_ctrl_data_ptr->out_msg_ptr, 
                          emm_get_nas_transaction_id(), 
                          MSGR_NAS_EMM,
                          NULL,
                          NULL,
                          emm_ctrl_data_ptr);
}

#endif
/*===========================================================================

FUNCTION  emm_mmtel_status

DESCRIPTION: This function is called by EMM to check MMTEL status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_mmtel_status(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
    if((emm_ctrl_data_ptr->mmtel_call_status.video_call_status == TRUE)||
      (emm_ctrl_data_ptr->mmtel_call_status.voice_call_status == TRUE)||
      (emm_ctrl_data_ptr->mmtel_call_status.sms_status == TRUE))
    {
      return TRUE;
    }
    return FALSE;
#else

    return (emm_ctrl_data_ptr->mmtel_call_status.sms_status);
#endif
}
/*===========================================================================

FUNCTION  emm_wms_sms_status

DESCRIPTION: This function is called by EMM to check SMS status sent from WMS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_wms_sms_status(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
    if((emm_ctrl_data_ptr->wms_sms_status.sms_over_nas_status == TRUE)||
      (emm_ctrl_data_ptr->wms_sms_status.sms_over_s102_status == TRUE)||
      (emm_ctrl_data_ptr->wms_sms_status.sms_over_ims_status == TRUE))
    {
      return TRUE;
    }
    return FALSE;
}

/*===========================================================================
FUNCTION    emm_abort_attach_procedure

DESCRIPTION
  Function aborts the attach procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_abort_attach_procedure
(
  void
)
{
  if(EMM_GET_STATE() == EMM_REGISTERED_INITIATED)
  {
    emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT(emm_ctrl_data_ptr != NULL);
    mm_stop_timer(TIMER_T3410);
    emm_delete_pended_message_with_msg_id(ATTACH_REQUEST,emm_ctrl_data_ptr);
    MSG_LOW_DS_0(MM_SUB, "Deleted Attach Request from pended message queue\n");
    if(EMM_GET_SUBSTATE() == EMM_WAITING_FOR_ESM_RESPONSE)
    {
      emm_build_and_send_detach_ind(EMM_DETACHED);
    }
    emm_send_diag_event(EVENT_NAS_ATTACH, (nas_event_end_result)PROC_END,
    (nas_event_end_reason)PROC_END_LOWER_LAYER_FAILURE, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
    emm_remove_release_client(emm_attach_release_indication);
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
  }
}

/*===========================================================================
FUNCTION    emm_abort_tau_procedure

DESCRIPTION
  Function aborts the tracking area update procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_abort_tau_procedure
(
  void
)
{
  if(EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED)
  {
    emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT(emm_ctrl_data_ptr != NULL);
    mm_stop_timer(TIMER_T3430);
    emm_delete_pended_message_with_msg_id(TRACKING_AREA_UPADTE_REQUEST,emm_ctrl_data_ptr);
    MSG_LOW_DS_0(MM_SUB, "Deleted TRACKING_AREA_UPADTE_REQUEST from pended message queue\n");
    emm_remove_release_client(emm_tau_request_release_indication);
    emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_END,
     (nas_event_end_reason)PROC_END_LOWER_LAYER_FAILURE, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
    if(( emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_TA_CHANGE )
       && (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED))
    {
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
      MSG_HIGH_DS_2(MM_SUB, "=EMM= Change EMM status to %d as TAU cause is %d",
                    EMM_STATUS_NOT_UPDATED, emm_ctrl_data_ptr->tau_cause);
    }
    EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
#ifndef FEATURE_NAS_DISABLE_VOICE
    if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED)
    {
      emm_csfb_failure_cause_type failure_cause_type;
      failure_cause_type.failure_type = AS_REJECT_CAUSE;
      failure_cause_type.failure_cause.lte_nas_failure_cause = LTE_NAS_LRRC_CONN_EST_FAILURE_LINK_FAILURE;//skip tau later
      emm_process_extended_service_req_failure(failure_cause_type,emm_ctrl_data_ptr);
    }
#endif
#endif
  }
}



/*===========================================================================

FUNCTION  emm_set_call_type_based_on_sib2

DESCRIPTION: This function is called by EMM to get call type used in RRC connection establishment request 
             triggered by service request cause as MO DATA and tau procedure cause as MO SIGNALING
             The call type is choosed for skipping ACB is most likely type when there is multiple indication
DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE there is skippable call type for AC barring, otherewise FALSE 

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_set_call_type_based_on_sib2(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
  boolean skip_ac_barring = FALSE;

   
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
  if((emm_ctrl_data_ptr->mmtel_call_status.voice_call_status == TRUE)&&
     (emm_ctrl_data_ptr->sib2_ac_barring_info.skip_ac_barring_for_volte_call == TRUE))/*voice*/
  {
    emm_ctrl_data_ptr->call_type = SYS_LTE_REESTAB_CALLTYPE_VOLTE;
    skip_ac_barring = TRUE;
  }
  else if((emm_ctrl_data_ptr->mmtel_call_status.video_call_status == TRUE)&&
          (emm_ctrl_data_ptr->sib2_ac_barring_info.skip_ac_barring_for_video_call == TRUE))/*video*/
  {
    emm_ctrl_data_ptr->call_type = SYS_LTE_REESTAB_CALLTYPE_VT;
    skip_ac_barring = TRUE;
  }
  else
#endif
  if(((emm_ctrl_data_ptr->mmtel_call_status.sms_status == TRUE)||/*sms over ip*/
           (emm_ctrl_data_ptr->wms_sms_status.sms_over_ims_status == TRUE))&&
           (emm_ctrl_data_ptr->sib2_ac_barring_info.skip_ac_barring_for_for_sms == TRUE))
  {
    emm_ctrl_data_ptr->call_type = SYS_LTE_REESTAB_CALLTYPE_SMS_OVER_IP;
    skip_ac_barring = TRUE;
  }
  else if(((emm_ctrl_data_ptr->wms_sms_status.sms_over_s102_status == TRUE)||/*sms over nas or s102*/
          (emm_ctrl_data_ptr->wms_sms_status.sms_over_nas_status == TRUE)) &&
          (emm_ctrl_data_ptr->sib2_ac_barring_info.skip_ac_barring_for_for_sms))
  {
    emm_ctrl_data_ptr->call_type = SYS_LTE_REESTAB_CALLTYPE_SMS;
    skip_ac_barring = TRUE;
  }
  else
  {
    emm_set_call_type_per_mmtel_and_sms_status(emm_ctrl_data_ptr);
  }
  return skip_ac_barring;
}

/*===========================================================================

FUNCTION  emm_set_call_type_per_mmtel_and_sms_status 
 
DESCRIPTION: set call type based on mmtel call status or wms sms status
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_set_call_type_per_mmtel_and_sms_status(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{

#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
  if(emm_ctrl_data_ptr->mmtel_call_status.voice_call_status == TRUE)
  {
    emm_ctrl_data_ptr->call_type = SYS_LTE_REESTAB_CALLTYPE_VOLTE;
  }
  else if(emm_ctrl_data_ptr->mmtel_call_status.video_call_status == TRUE)
  {
    emm_ctrl_data_ptr->call_type = SYS_LTE_REESTAB_CALLTYPE_VT;
  }
  else
#endif
  if((emm_ctrl_data_ptr->mmtel_call_status.sms_status == TRUE)||/*sms over ip*/
           (emm_ctrl_data_ptr->wms_sms_status.sms_over_ims_status == TRUE))
  {
    emm_ctrl_data_ptr->call_type = SYS_LTE_REESTAB_CALLTYPE_SMS_OVER_IP;
  }
  else if((emm_ctrl_data_ptr->wms_sms_status.sms_over_s102_status == TRUE)||/*sms over nas or s102*/
          (emm_ctrl_data_ptr->wms_sms_status.sms_over_nas_status == TRUE))
  {
    emm_ctrl_data_ptr->call_type = SYS_LTE_REESTAB_CALLTYPE_SMS;
  }
}

/*===========================================================================

FUNCTION  emm_check_mplmn_in_progress

DESCRIPTION: This function is called by EMM to check MPLMN search in progress

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_check_mplmn_in_progress(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
  
  if((*emm_ctrl_data_ptr->conn_mode_manual_search_ptr == FALSE) ||
#ifdef FEATURE_DUAL_SIM
      (( mm_check_mplmn_in_progress_on_other_subs() == FALSE )&&
#endif
      (emm_ctrl_data_ptr->plmn_search_in_progress == FALSE)
#ifdef FEATURE_DUAL_SIM
       )
#endif
     )
  {
    return FALSE;
  }
  return TRUE;
}

#ifdef FEATURE_LTE_REL13

/*===========================================================================
FUNCTION   emm_pend_mo_detach

DESCRIPTION  
     This function pends the MO detach if TAU is going on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_pend_mo_detach
(
  emm_mo_detach_type detach_type,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  boolean detach_pended = FALSE;
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL13) &&
     (EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED))
  {
    emm_ctrl_data_ptr->ongoing_detach_type = detach_type;
    emm_ctrl_data_ptr->ongoing_detach_procedure = TRUE;
    detach_pended = TRUE;
  }
  return detach_pended;
}
#endif

/*===========================================================================
FUNCTION  emm_get_3gpp_rel_ver_per_sub

DESCRIPTION
  This function returns 3gpp spec version per subs

DEPENDENCIES
  None

RETURN VALUE
  3gpp release version

SIDE EFFECTS
  None
===========================================================================*/
lte_3gpp_release_ver_e emm_get_3gpp_rel_ver_per_sub
(
  sys_modem_as_id_e_type as_id
)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = emm_db_get_ctrl_data_per_subs((mm_as_id_e_type)as_id);

  
  return emm_ctrl_data_ptr->emm_3gpp_spec_ver;
}

/*===========================================================================
FUNCTION  emm_check_ready_for_psm

DESCRIPTION: This function checks if NAS is ready to enter PSM mode

DEPENDENCIES
  None

RETURN VALUE
  PSM ready status

SIDE EFFECTS
  None
===========================================================================*/
sys_psm_status_e_type emm_check_ready_for_psm(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
  sys_psm_status_e_type status = SYS_PSM_STATUS_READY;
  emm_state_type emm_state = EMM_GET_STATE();
  emm_substate_type emm_substate = EMM_GET_SUBSTATE();

  
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

  if(emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE)
  {
    status = SYS_PSM_STATUS_NOT_READY_EMC_CALL;
  }
  else
#endif
#endif
  if (mm_timer_status[TIMER_T3324] == TIMER_ACTIVE)
  {
    status = SYS_PSM_STATUS_NOT_READY_ACTIVE_TIMER_RUNNING;
  }
  else if(
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
      ((mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)||
           (mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG))
          ||
#endif
          ((emm_ctrl_data_ptr->emm_connection_state == EMM_CONNECTED_STATE) ||
           (emm_ctrl_data_ptr->emm_connection_state == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE) ||
		   (emm_ctrl_data_ptr->emm_connection_state == EMM_RELEASING_RRC_CONNECTION_STATE))
          ||
          (mm_timer_status[TIMER_T3411] == TIMER_ACTIVE)
          ||
          (mm_timer_status[TIMER_T3402] == TIMER_ACTIVE))
  {
    status = SYS_PSM_STATUS_NOT_READY_NOT_IDLE;
  }
  else if ((emm_ctrl_data_ptr->t3324_value != NAS_INVALID_PSM_TIMER_VALUE) &&
           (*emm_ctrl_data_ptr->allow_psm_in_oos_ltd_srv_ptr == FALSE) &&
           ((emm_state != EMM_REGISTERED) ||
            (emm_substate != EMM_REGISTERED_NORMAL_SERVICE)) 
#ifdef FEATURE_FMD_SPEED_INFO
			&&
			(emm_ctrl_data_ptr->fmd_psm_fallback_timer_expired != TRUE)
#endif      
)
  {
    if ((emm_ctrl_data_ptr->plmn_service_state->service_status == SYS_SRV_STATUS_NO_SRV) ||
        (emm_ctrl_data_ptr->plmn_service_state->service_status == SYS_SRV_STATUS_LIMITED))
    {
      status = SYS_PSM_STATUS_NOT_READY_ACTIVE_OOS;
    }
    else
    {
       status = SYS_PSM_STATUS_NOT_READY_NOT_IDLE;
    }
  }

#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
  MSG_HIGH_DS_1(MM_SUB, "=EMM= emc_srv_status %d", emm_ctrl_data_ptr->emc_srv_status);
#endif

#ifdef FEATURE_NAS_GW
  MSG_HIGH_DS_1(MM_SUB, "=EMM= mm_csfb_state %d", mm_csfb_state);
#endif

  MSG_HIGH_DS_7(MM_SUB, "=EMM= PSM ready: emm_conn_state %d, t3324_value %d, emm_state %d,\
  emm_substate %d,t3324_status %d, psm_ready_status = %d, psm_exit = %d",
  emm_ctrl_data_ptr->emm_connection_state, emm_ctrl_data_ptr->t3324_value, emm_state, emm_substate,
  mm_timer_status[TIMER_T3324], status, *emm_ctrl_data_ptr->allow_psm_in_oos_ltd_srv_ptr);

  return status;
}

/*===========================================================================
FUNCTION  emm_save_psm_context

DESCRIPTION: This function saves PSM context upon entering PSM

DEPENDENCIES
  None

RETURN VALUE
  TRUE: PSM context saved successfully, else FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_save_psm_context(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
  boolean result = TRUE;

  mm_psm_cache_ptr = (mm_psm_cache_s_type *)modem_mem_alloc(sizeof(mm_psm_cache_s_type), MODEM_MEM_CLIENT_NAS);
  mm_check_for_null_ptr((void*)mm_psm_cache_ptr);
  memset((void*)mm_psm_cache_ptr, 0, sizeof(mm_psm_cache_s_type));

   mm_psm_cache_ptr->rat = emm_ctrl_data_ptr->plmn_service_state->active_rat;
  mm_psm_cache_ptr->plmn = emm_ctrl_data_ptr->plmn_service_state->plmn;
  mm_psm_cache_ptr->sim_state = emm_ctrl_data_ptr->plmn_service_state->sim_state;
  mm_psm_cache_ptr->reg_service_domain = emm_ctrl_data_ptr->plmn_service_state->reg_service_domain;
  mm_psm_cache_ptr->t3324_value = emm_ctrl_data_ptr->t3324_value;
#ifdef FEATURE_NAS_GW
  mm_psm_cache_ptr->lte.TIN = emm_ctrl_data_ptr->TIN;
#endif
  mm_psm_cache_ptr->timers_timestamp = time_get_ms_native();
  mm_psm_cache_ptr->lte.lte_cs_capability = emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability;
  mm_psm_cache_ptr->lte.last_attempted_tai = emm_ctrl_data_ptr->last_attempted_tai;
  mm_psm_cache_ptr->lte.tai_list = *(emm_ctrl_data_ptr->emm_tai_list_ptr);
  mm_psm_cache_ptr->lte.t3412_value = emm_ctrl_data_ptr->t3412_value;
  mm_psm_cache_ptr->lte.t3402_value = emm_ctrl_data_ptr->t3402_value;
  mm_psm_cache_ptr->lte.t3412_timer = (uint64)gs_enquire_timer_ext(get_mm_timer_queue(),
                                                                             TIMER_T3412);
  mm_psm_cache_ptr->lte.eps_nw_feature_support = 
                           emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support;
#ifdef FEATURE_CIOT
  mm_psm_cache_ptr->lte.attach_without_pdn = emm_ctrl_data_ptr->attach_without_pdn;
  mm_psm_cache_ptr->lte.cp_sms_service_status = emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status;
  mm_psm_cache_ptr->lte.is_conn_suspended = emm_ctrl_data_ptr->is_conn_suspended;
  mm_psm_cache_ptr->lte.conn_suspended_rat = emm_ctrl_data_ptr->conn_suspended_rat;
  MSG_HIGH_DS_8(MM_SUB,"=EMM= save CIOT cp_ciot = %d, hc_cp_ciot = %d s1_u_data = %d, up_ciot = %d epco_supported = %d er_without_pdn_supported %d attach wd pdn = %d SMS status %d",  
                  mm_psm_cache_ptr->lte.eps_nw_feature_support.cp_ciot_supported,
                  mm_psm_cache_ptr->lte.eps_nw_feature_support.hc_cp_ciot_supported,
                  mm_psm_cache_ptr->lte.eps_nw_feature_support.s1_u_data_supported, 
                  mm_psm_cache_ptr->lte.eps_nw_feature_support.up_ciot_supported,
                  mm_psm_cache_ptr->lte.eps_nw_feature_support.epco_supported,
                  mm_psm_cache_ptr->lte.eps_nw_feature_support.er_without_pdn_supported,
                  mm_psm_cache_ptr->lte.attach_without_pdn,
                  mm_psm_cache_ptr->lte.cp_sms_service_status);
  MSG_HIGH_DS_2(MM_SUB,"=EMM= save CE Rest %d conn_suspended %d",  
                  mm_psm_cache_ptr->lte.eps_nw_feature_support.ce_restriction, mm_psm_cache_ptr->lte.is_conn_suspended);
#endif
  mm_psm_cache_ptr->lte.gmm_drx_cn_coeff_s1 = emm_get_gmm_drx_cn_coff_s1();
  mm_psm_cache_ptr->lte.edrx_params = emm_ctrl_data_ptr->edrx_params;
  
  mm_psm_cache_ptr->lte.ftai_clear_timer = (uint32)gs_enquire_timer_ext(get_mm_timer_queue(),
                                                                        EMM_FORBIDDEN_TAI_CLEAR_TIMER);

  mm_psm_cache_ptr->t3247_value = (dword)gs_enquire_timer_ext(get_mm_timer_queue(),
                                                    TIMER_T3247);

  if( (mm_psm_cache_ptr->lte.ftai_clear_timer != 0) ||
      (mm_psm_cache_ptr->t3247_value != 0) )
  {
    mm_psm_cache_ptr->lte.rrc_forbidden_list = *(emm_ctrl_data_ptr->rrc_forbidden_list_ptr);
    mm_psm_cache_ptr->lte.forbidden_for_service_list = *(emm_ctrl_data_ptr->forbidden_for_service_list_ptr);
    (void)pthread_mutex_lock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);
    mm_psm_cache_ptr->lte.manual_ftai_list = *(emm_ctrl_data_ptr->manual_ftai_list_ptr);
    (void)pthread_mutex_unlock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);
  }

  mm_psm_cache_ptr->lte.combined_reg_fplmn_list = emm_ctrl_data_ptr->combined_reg_fplmn_list;
  mm_psm_cache_ptr->lte.tau_attempt_counter = emm_ctrl_data_ptr->emm_tau_attempt_counter;
  mm_psm_cache_ptr->lte.force_req_ser_domain = emm_ctrl_data_ptr->force_req_ser_domain;
#if defined FEATURE_3G_USER_NW_REDIR_TO_LTE
  mm_psm_cache_ptr->lte.is_accepted_on_lte = emm_ctrl_data_ptr->is_accepted_on_lte;
#endif
  mm_psm_cache_ptr->lte.sg_association_to_be_established =
                               emm_ctrl_data_ptr->sg_association_to_be_established;
  mm_psm_cache_ptr->lte.ue_security_capability = emm_ctrl_data_ptr->ue_security_capability;


#if defined(FEATURE_NAS_REL10) || defined(FEATURE_LTE_REL10)
  if(mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
  {
    result = (result && mm_write_backoff_remaining_info_to_efs()) ? TRUE : FALSE;
#ifdef FEATURE_NAS_GW
    gmm_set_gmm_is_congestion_due_to_rau(FALSE);
#endif
  }
#endif

  mm_psm_cache_ptr->usim_gprs_counter = mm_emm_usim_gprs_counter;
  mm_psm_cache_ptr->usim_non_gprs_counter = mm_emm_usim_non_gprs_counter ;
  mm_psm_cache_ptr->sim_cs_invalid_due_to_dos = *emm_ctrl_data_ptr->is_sim_cs_invalid_due_to_dos;
  mm_psm_cache_ptr->sim_to_be_cs_invalid_after_power_off =   *emm_ctrl_data_ptr->is_sim_to_be_cs_invalid_after_power_off;

  result = (result && emm_write_rplmn_type_to_efs(emm_ctrl_data_ptr->rplmn_info.rplmn_type))
           ? TRUE : FALSE;
  

  return result;
}

/*===========================================================================
FUNCTION  emm_check_esr_for_ps

DESCRIPTION
  This function checks whether ESR for PS shall be started

DEPENDENCIES
  None

RETURN VALUE
  access class value

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_check_esr_for_ps(emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  
  if((reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE) &&
                 (emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.esr_ps_supported == TRUE)) 
  {
    return TRUE;
  }
  return FALSE;
}

void lte_nas_msgr_register
(
  msgr_tech_module_type  mod,
  msgr_client_t *msgr_client_id, //!< Message Client Handle
  msgr_id_t  id, //!< Message Queue ID
  msgr_umid_type umid //!< UMID for which to register
)
{
  if(msgr_register(mod,msgr_client_id,id,umid) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "msgr register fail ID %d",umid, 0,0 );
  }
}

void lte_nas_msgr_deregister
(
  msgr_tech_module_type  mod,           //!< TECH,MODULE Performing Operation
  msgr_client_t         *msgr_client_id, //!< Message Client Handle
  msgr_umid_type         umid            //!< UMID for which to register
)
{
  if(msgr_deregister(mod,msgr_client_id,umid) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "msgr register fail ID %d",umid, 0,0 );
  }
}


void lte_nas_msgr_client_set_rex_q
(
  msgr_client_t      *msgr_client_id,   //!< Message Client Handle
  rex_tcb_type       *tcb,              //!< REX TCB pointer
  rex_sigs_type       sig,              //!< REX signal to set
  q_type             *msg_queue,        //!< queue to place the msg
  q_type             *free_msg_queue,   //!< queue to get an empty buffer
  uint16              msgr_hdr_offset,  //!< offset to the msgr_hdr
  uint16              cmd_type_offset,  //!< offset to the legacy cmd_type
  uint16              max_msg_size      //!< max size of msgs for the queue
)
{
  if(msgr_client_set_rex_q(msgr_client_id,tcb,
                           sig,msg_queue,
                           free_msg_queue,msgr_hdr_offset,
                           cmd_type_offset,max_msg_size) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "msgr_client_set_rex_q fail",0, 0,0 );
  }
}

void lte_nas_msgr_client_create
(
  msgr_client_t   *msgr_client_id
)
{
  if(msgr_client_create(msgr_client_id)  != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "msgr_client_create fail %d",msgr_client_id, 0,0 );
  }
}



#ifdef FEATURE_LTE_REL10
/*===========================================================================
FUNCTION  emm_evaluate_low_priority_indicator_value

DESCRIPTION
  This function evaluates low priority indicator value for Device Properties IE

DEPENDENCIES
  None

RETURN VALUE
  TRUE if low priority, otherwise FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_evaluate_low_priority_indicator_value
(
  emm_ctrl_data_type *emm_ctrl_data_ptr,
  lte_rrc_est_cause_e conn_est_cause
)
{
  boolean low_priority = FALSE;
  

  /* Low priority indicator shall be set to TRUE when all below conditions are satisfied:
     a) UE is configured for NAS signalling low priority
     a.1) And if UE supports REL-11 then UE is also configured for Extended access barring (EAB)
     b) UE is not performing emergency attach
     c) UE does not have a PDN connection for emergency bearer services established or
        UE is not establishing a PDN connection for emergency bearer services
     d) UE is not configured to use access class 11 - 15 in selected PLMN
     e) UE is not responding to paging */
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
     ((reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE) 
#ifdef FEATURE_NAS_EAB
      &&
      ((emm_ctrl_data_ptr->eab_barring_flag == FALSE) ||
       (reg_sim_per_subs_chk_eab_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE))
#endif
     ) &&
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
     (conn_est_cause != LTE_RRC_EST_CAUSE_EMERGENCY) &&
#endif
     (emm_check_high_priority(emm_ctrl_data_ptr) == FALSE) &&
     (emm_ctrl_data_ptr->responding_to_page == FALSE))
  {
    low_priority = TRUE;
  }

  return low_priority;
}
#endif /* FEATURE_LTE_REL10 */

#ifdef FEATURE_NAS_EAB
/*===========================================================================
FUNCTION  emm_eab_cat_c_chk

DESCRIPTION: This function checks whether EAB is applicable for the UE of category C,
             i.e, the UE is neither in the PLMN listed as most preferred PLMN of
             the country where the UEs are roaming in the operator-defined PLMN
             selector list on the USIM, nor in their HPLMN nor in a PLMN that is
             equivalent to their HPLMN

DEPENDENCIES
  None

RETURN VALUE
  TRUE if EAB is applicable else FALSE

SIDE EFFECTS
  None
===========================================================================*/

boolean emm_eab_cat_c_chk(  emm_ctrl_data_type     *emm_ctrl_data_ptr)
{
  sys_plmn_id_s_type    current_camped_plmn;
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc,saved_oplmn_mcc;
  uint32  mnc;
  boolean result_eab = TRUE;
   
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  
  current_camped_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
              emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
                                                  
                                                  
  if (                                           
#ifdef FEATURE_DUAL_SIM
       (reg_sim_per_subs_is_ehplmn(current_camped_plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
       (reg_sim_is_ehplmn(current_camped_plmn) )
#endif
       ||(sys_eplmn_list_equivalent_plmn(current_camped_plmn)&& reg_sim_is_ehplmn_in_eplmn())
     )
     {
     	 result_eab = FALSE;
     }
     else
     {
        sys_plmn_get_mcc_mnc
        (
          current_camped_plmn,
          &plmn_id_is_undefined,
          &mnc_includes_pcs_digit,
          &mcc,
          &mnc
        );

        sys_plmn_get_mcc_mnc
        (
          emm_ctrl_data_ptr->most_pref_oplmn,
          &plmn_id_is_undefined,
          &mnc_includes_pcs_digit,
          &saved_oplmn_mcc,
          &mnc
        );

        if(sys_mcc_same_country(saved_oplmn_mcc,mcc) == FALSE)
        {
          emm_ctrl_data_ptr->most_pref_oplmn = reg_sim_get_most_pref_oplmn_plmn(mcc
#ifdef FEATURE_DUAL_SIM                          
                                            ,(sys_modem_as_id_e_type)mm_sub_id
#endif                   
                                                                                    );
        }

        if(sys_plmn_match(current_camped_plmn,emm_ctrl_data_ptr->most_pref_oplmn)==TRUE)
        {
          result_eab = FALSE;
        }
                
    }

 return result_eab;
}
#endif

#ifdef FEATURE_CIOT
emm_pended_msg_list_type *emm_find_pended_cpsr_with_esm_data
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  emm_client_list_type *client = (emm_client_list_type *)NULL ;
  emm_pended_msg_list_type *message = (emm_pended_msg_list_type *)NULL ;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  if (emm_ctrl_data_ptr->client_list != (emm_client_list_type *)NULL)
  {
    client = emm_ctrl_data_ptr->client_list ;

    while (client != (emm_client_list_type *)NULL)
    {
      if (client->next_message != (emm_pended_msg_list_type *)NULL)
      {
        message = client->next_message ;

        while (message != (emm_pended_msg_list_type *)NULL)
        {
          if ((message->nas_out_message.nas_hdr.msg_id == CP_SERVICE_REQUEST)
             &&(message->nas_out_message.emm_outgoing_msg.cp_service_req.esm_msg_container_exists == TRUE))
             {
               return message ;
             }
          message = message->next_message ;
        }
      }
      else
      {
        break ;
      }

      client = client->next_client ;
    }
  }

  return (emm_pended_msg_list_type *)NULL ;

} /* end of emm_find_pended_cpsr_with_esm_data() */

/*===========================================================================
FUNCTION  emm_cp_opt_active

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean  emm_cp_opt_active
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  if(emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.cp_ciot_supported == TRUE)
  {
    return TRUE;
  }
  return FALSE;
}/* emm_cp_opt_active */

/*===========================================================================
FUNCTION  emm_up_opt_active

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean  emm_up_opt_active
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  if(emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.up_ciot_supported == TRUE)
  {
    return TRUE;
  }
  return FALSE;
}/* emm_up_opt_active */

/*===========================================================================
FUNCTION  emm_chk_complete_cpsr_by_security_msg

DEPENDENCIES
  None

RETURN VALUE
  NONE TRUE if msg is security protected,otherwise return FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_chk_complete_cpsr_by_security_msg
(
  lte_nas_security_hdr_type sec_hdr,
  lte_nas_message_id        msg_id 
)
{
  if((sec_hdr == INTEGRITY_PROTECT) 
      ||(sec_hdr == INTEGRITY_PROTECT_CIPHER)
      ||(sec_hdr == INTEGRITY_PROTECT_SECURITY_MODE_CMD))
  {
    switch(msg_id)
    {
      case SECURITY_MODE_COMMAND:
      case EMM_STATUS:
      case ESM_DATA_TRANSPORT:
      case CS_SERV_NOTIFICATION:
      case DOWNLINK_NAS_TRANSPORT:
        return TRUE;
      default:
       return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}
#endif

/*===========================================================================
FUNCTION  emm_invalidate_edrx_params

DESCRIPTION
  This function invalidates eDRX parameters and updates upper layer and RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_invalidate_edrx_params
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  if(emm_ctrl_data_ptr->edrx_params.enabled == TRUE)
  {
    emm_ctrl_data_ptr->edrx_params.enabled = FALSE;
    emm_ctrl_data_ptr->edrx_params.ptw = NAS_INVALID_EDRX_VALUE;
    emm_ctrl_data_ptr->edrx_params.cycle_length = NAS_INVALID_EDRX_VALUE;

    /* Send eDRX update to RRC */
    emm_send_rrc_edrx_update(emm_ctrl_data_ptr, emm_ctrl_data_ptr->edrx_params.enabled);
  }

  /* Send eDRX params change ind to upper layers */
  if ((emm_get_rat_for_eDRX_param() != SYS_RAT_NONE)&&
      (emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.edrx_support == TRUE))
  {
    mm_send_mmr_edrx_params_change_ind(emm_get_rat_for_eDRX_param(),
                                       emm_ctrl_data_ptr->edrx_params.enabled,
                                       emm_ctrl_data_ptr->edrx_params.ptw,
                                       emm_ctrl_data_ptr->edrx_params.cycle_length);
  }
}


/*===========================================================================
FUNCTION  emm_chk_attach_with_imsi

DESCRIPTION
  This function checks the conditions for if attach with IMSI has to be done

DEPENDENCIES
  None

RETURN VALUE
  TRUE if attach with IMSI needs to be done, else FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_chk_attach_with_imsi
(
   emm_ctrl_data_type *emm_ctrl_data_ptr
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

  ,boolean            emc_attach
#endif
#endif
)
{
  boolean result = FALSE;
  boolean config = reg_sim_per_subs_chk_attach_with_imsi_cfg((sys_modem_as_id_e_type)mm_sub_id);

  if(
#ifdef FEATURE_CIOT
     (((EMM_GET_STATE() == EMM_DEREGISTERED) && 
       (emm_ctrl_data_ptr->plmn_service_state->active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS))||
#endif
      (config == TRUE)
#ifdef FEATURE_CIOT
     )
#endif
     &&
     ((emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_AVAILABLE)
      ||
      (emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_CS_INVALID))
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

     && (emc_attach == FALSE)
#endif
#endif
    )
  {
    sys_plmn_id_s_type    camped_plmn;
    sys_plmn_id_s_type    rplmn;
    boolean               gsm_spt  = FALSE;
    boolean               umts_spt = FALSE;
    boolean               tds_spt  = FALSE;

    sys_plmn_undefine_plmn_id(&rplmn);

    camped_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                      emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

#ifdef FEATURE_NAS_GW
    gsm_spt  = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,
                                               SYS_RAT_GSM_RADIO_ACCESS);
    umts_spt = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,
                                               SYS_RAT_UMTS_RADIO_ACCESS);
    tds_spt  = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,
                                               SYS_RAT_TDS_RADIO_ACCESS);

    if((gsm_spt || umts_spt || tds_spt) &&
       (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI) &&
       (mm_is_rai_valid()) &&
       (mm_is_ptmsi_valid()))
    {
      rplmn = gmm_stored_rai.plmn_id;
    }
    else
#endif
    if(emm_ctrl_data_ptr->last_registered_tai_valid)
    {
      rplmn = emm_ctrl_data_ptr->last_registered_tai.plmn;
    }

    /* Attach with IMSI if camped PLMN is not last registered PLMN
       and also not an EPLMN */
    if(!sys_plmn_id_is_undefined(rplmn) &&
       ((sys_plmn_match(camped_plmn, rplmn) == FALSE) &&
        ((sys_eplmn_list_equivalent_plmn(camped_plmn) == FALSE)
         ||
         (sys_eplmn_list_equivalent_plmn(rplmn) == FALSE))))
    {
      result = TRUE;
    }
  }

  MSG_HIGH_DS_2(MM_SUB,"=EMM= AttachWithIMSI = %d, config = %d", result, config);

  return result;
} /* end emm_chk_attach_with_imsi() */
/*===========================================================================

FUNCTION emm_get_timer_multiplier

DESCRIPTION
  This function returns the EMM multipiler to be applied to timers
  
DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/

uint32 emm_get_timer_multiplier (void)
{
#ifdef FEATURE_CIOT
    if(emm_ctrl_data_ptr->plmn_service_state->active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
        return CIoT_EMM_FACTOR;
    else
        return 1;
#else
        return 1;
#endif
}
/*===========================================================================

FUNCTION esm_get_timer_multiplier

DESCRIPTION
  This function returns the EMM multipiler to be applied to timers
  
DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/
uint32 esm_get_timer_multiplier (void)
{
#ifdef FEATURE_CIOT
    if(emm_ctrl_data_ptr->plmn_service_state->active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
        return CIoT_ESM_FACTOR;
    else
        return 1;
#else
        return 1;
#endif
}

/*===========================================================================

FUNCTION emm_chk_nb1_rat

DESCRIPTION
  This function returns true if UE is in NB1 rat
  else return false
  
DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_chk_nb1_rat (void)
{
#ifdef FEATURE_CIOT
  if(emm_ctrl_data_ptr->plmn_service_state->active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
    return TRUE;
  else
    return FALSE;
#else
    return FALSE;
#endif
}

#ifdef FEATURE_CIOT
boolean emm_is_attach_without_pdn_supported(void)
{
  if((emm_bit_mask_check(emm_ctrl_data_ptr->nas_ciot_capability_config, EMM_ER_WITHOUT_PDN) == TRUE) &&
  	  (emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.er_without_pdn_supported == TRUE))
  {
    return TRUE;
  }
  else
  { 
    return FALSE;
  }
}
#endif

/*===========================================================================

FUNCTION emm_get_timer_extention_value

DESCRIPTION
  This function returns extention timer value
   return 240000 it if it is NB1 rat else return 0
  
DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/
dword emm_get_timer_extention_value (void)
{
#ifdef FEATURE_CIOT
  if(emm_chk_nb1_rat() == TRUE)
    return EMM_TIMER_NB1_EXT_VALUE;
  else
    return 0;
#else
    return 0;
#endif
}

/*===========================================================================

FUNCTION emm_chk_m1_rat

DESCRIPTION
  This function returns true if UE is in M1 rat
  else return false
  
DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_chk_m1_rat (void)
{
#ifdef FEATURE_CIOT
  if(emm_ctrl_data_ptr->plmn_service_state->active_rat == SYS_RAT_LTE_M1_RADIO_ACCESS)
    return TRUE;
  else
#endif
    return FALSE;
}


/*===========================================================================
 FUNCTION	 emm_is_mapping_cause_16_17_to_cause_18_allowed
 
 DESCRIPTION
   This function allow if mapping of cause 16,17 to 18 is allowed
 
 DEPENDENCIES
   None
 
 RETURN VALUE
   boolean TRUE/FALSE .. default value should return FALSE, not impact other cases
 
 SIDE EFFECTS
   None
 ===========================================================================*/
 


boolean emm_is_mapping_cause_16_17_to_cause_18_allowed(emm_ctrl_data_type *emm_ctrl_data_ptr)
{
   boolean result = FALSE;
   sys_plmn_id_s_type  sys_plmn;
   ASSERT(emm_ctrl_data_ptr != NULL);
   ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);
   
   sys_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
		   emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

   if(
#ifdef FEATURE_NAS_GW  
   (emm_is_gcf_enable() == FALSE) &&
#endif
   	  (emm_ctrl_data_ptr->emm_map_cause_17_to_cause_18 == TRUE)&&
#ifdef FEATURE_DUAL_SIM
   	  (reg_sim_check_mcc_same_country_per_sub( sys_plmn,
   	  (sys_modem_as_id_e_type)mm_as_id) == TRUE)
#else
     (reg_sim_check_mcc_same_country(sys_plmn) == TRUE)
#endif

    )
   {
     result = TRUE;
   }

   MSG_HIGH_DS_2(MM_SUB, "=EMM= [Debug] NV read value = %d and return result is %d ", 
                    emm_ctrl_data_ptr->emm_map_cause_17_to_cause_18, result);
   return result;
}


/*===========================================================================
FUNCTION emm_get_rat_for_eDRX

DESCRIPTION
  This function returns rat info for updating eDRX parameter
  
DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/
sys_radio_access_tech_e_type  emm_get_rat_for_eDRX_param (void)
{
  /* Send eDRX params change ind to upper layers */
  if((mm_edrx_cfg.lte.enabled == TRUE) &&
     (emm_ctrl_data_ptr->plmn_service_state->active_rat == SYS_RAT_LTE_RADIO_ACCESS))
  {
    return SYS_RAT_LTE_RADIO_ACCESS;
  }
#ifdef FEATURE_CIOT
  else if((mm_edrx_cfg.NB1.enabled == TRUE) &&
          (emm_ctrl_data_ptr->plmn_service_state->active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS))
  {
    return SYS_RAT_LTE_NB1_RADIO_ACCESS;
  }
#ifndef FEATURE_CATM1_DISABLE
  else if((mm_edrx_cfg.M1.enabled == TRUE) &&
          (emm_ctrl_data_ptr->plmn_service_state->active_rat == SYS_RAT_LTE_M1_RADIO_ACCESS))
  {
    return SYS_RAT_LTE_M1_RADIO_ACCESS;
  }
#endif
#endif
  else
    return SYS_RAT_NONE;
}

/*===========================================================================

FUNCTION emm_restore_eDRX_param_per_rat

DESCRIPTION
  This function restore eDRX param 
  
DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/
void emm_restore_eDRX_param_per_rat(void)
{
    switch (emm_get_rat_for_eDRX_param()) 
    {
      case SYS_RAT_LTE_RADIO_ACCESS:
#ifdef FEATURE_CIOT
      case SYS_RAT_LTE_M1_RADIO_ACCESS:
      case SYS_RAT_LTE_NB1_RADIO_ACCESS:
#endif
        emm_ctrl_data_ptr->edrx_params = mm_psm_cache_ptr->lte.edrx_params;
        break;
      default:
        emm_ctrl_data_ptr->edrx_params.enabled = FALSE;
        emm_ctrl_data_ptr->edrx_params.ptw = NAS_INVALID_EDRX_VALUE;
        emm_ctrl_data_ptr->edrx_params.cycle_length = NAS_INVALID_EDRX_VALUE;
        break;
    }
}

#ifdef FEATURE_CIOT

uint16 emm_get_ciot_ue_supported_param(void)
{
    emm_ctrl_data_type * emm_ctrl_data_ptr = NULL;    
    uint16 ciot_config = 0 ;
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
                
    if(emm_ctrl_data_ptr != NULL)
    {
      NAS_ENTER_CRIT_SECT(nas_ciot_config_crit_sec);

      ciot_config = emm_ctrl_data_ptr->nas_ciot_capability_config;

      NAS_EXIT_CRIT_SECT(nas_ciot_config_crit_sec);
    }

    MSG_HIGH_DS_2(MM_SUB, "=EMM= data_ptr = 0x%p, ciot_config = %d",
                emm_ctrl_data_ptr, ciot_config);

    return ciot_config;
}

/*==============================================================================
FUNCTION  emm_get_restore_psm_nbiot_info

DESCRIPTION
 This function returns the current state of CIoT data

DEPENDENCIES
 None

RETURN VALUE
 None

SIDE EFFECTS
 None
/*============================================================================*/
psm_nbiot_info emm_get_restored_psm_nbiot_info(sys_modem_as_id_e_type subs_id)
{
    emm_ctrl_data_type * emm_ctrl_data_ptr = NULL;    
    psm_nbiot_info psm;
    ULogHandle handle = mm_get_ulog_handle();

    memset ((void*)&psm, 0, sizeof(psm_nbiot_info));
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    if (emm_ctrl_data_ptr == NULL)
    {
        if(handle != NULL)
        {
          ULOG_RT_PRINTF_0(handle, "=EMM= API CIOT ctrldata NULL");
        } 
        return psm;
    }
    if ((emm_ctrl_data_ptr->emm_state == EMM_DEREGISTERED) &&
    (EMM_GET_SUBSTATE() == EMM_DEREGISTERED_WAITING_PSM_RESTORE_RSP)&&
     (mm_psm_cache_ptr != NULL))
    {        
        if(handle != NULL)
        {
          ULOG_RT_PRINTF_0(handle, "=EMM= API restored CIOT reading from cache");
        }                
        psm.cp_ciot_supported = 
            mm_psm_cache_ptr->lte.eps_nw_feature_support.cp_ciot_supported;      
        psm.hc_cp_ciot_supported = 
            mm_psm_cache_ptr->lte.eps_nw_feature_support.hc_cp_ciot_supported;
        psm.s1_u_data_supported = 
            mm_psm_cache_ptr->lte.eps_nw_feature_support.s1_u_data_supported;        
        psm.up_ciot_supported = 
            mm_psm_cache_ptr->lte.eps_nw_feature_support.up_ciot_supported;          
        psm.epco_ciot_cap = 
            mm_psm_cache_ptr->lte.eps_nw_feature_support.epco_supported;
		psm.attach_without_pdn = mm_psm_cache_ptr->lte.attach_without_pdn;
    }
    else
    {    
        if(handle != NULL)
        {
          ULOG_RT_PRINTF_0(handle, "=EMM= API restored CIOT reading from database");
        }
        psm.cp_ciot_supported = 
           emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.cp_ciot_supported;
        psm.hc_cp_ciot_supported = 
           emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.hc_cp_ciot_supported;
        psm.s1_u_data_supported = 
           emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.s1_u_data_supported;        
        psm.up_ciot_supported = 
           emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.up_ciot_supported;        
        psm.epco_ciot_cap = emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.epco_supported;
		psm.attach_without_pdn = emm_ctrl_data_ptr->attach_without_pdn;
    }    
        
    if(handle != NULL)
    {
          ULOG_RT_PRINTF_6(handle, "=EMM= API CIOT cp_ciot = %d, hc_cp_ciot = %d s1_u_data = %d, up_ciot = %d epco_ciot_cap = %d attach wihtout pdn %d ", psm.cp_ciot_supported, 
                  psm.hc_cp_ciot_supported,psm.s1_u_data_supported, 
                  psm.up_ciot_supported, psm.epco_ciot_cap,psm.attach_without_pdn);
    }          
    return psm;    
}
/*==============================================================================
FUNCTION  emm_get_active_rat

DESCRIPTION
 This function returns the current active rat

DEPENDENCIES
 None

RETURN VALUE
 Ratio Access Technology of the current rat

SIDE EFFECTS
 None
============================================================================*/
sys_radio_access_tech_e_type emm_get_active_rat(void)
{
    emm_ctrl_data_type * emm_ctrl_data_ptr = NULL;
    sys_radio_access_tech_e_type rat = SYS_RAT_NONE;
    ULogHandle handle = mm_get_ulog_handle();

    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    if (emm_ctrl_data_ptr == NULL)
    {
        if(handle != NULL)
        {
          ULOG_RT_PRINTF_0(handle, "=EMM= API rat CIOT ctrldata NULL");
        } 
        return rat;
    }
    if ((emm_ctrl_data_ptr->emm_state == EMM_DEREGISTERED) &&
    (EMM_GET_SUBSTATE() == EMM_DEREGISTERED_WAITING_PSM_RESTORE_RSP)&&
     (mm_psm_cache_ptr != NULL))
    {        
        if(handle != NULL)
        {
          ULOG_RT_PRINTF_0(handle, "=EMM= API rat CIOT reading from cache");
        }
        rat = mm_psm_cache_ptr->rat;
    }
    else
    {
        if(handle != NULL)
        {
          ULOG_RT_PRINTF_0(handle, "=EMM= API rat CIOT reading from database");
        }
        rat = emm_ctrl_data_ptr->plmn_service_state->active_rat;
    }
    return rat;
}
#endif
#ifdef FEATURE_CIOT
/*===========================================================================
FUNCTION  emm_send_mmr_phy_cmd_ind

DESCRIPTION
  This function sends a command to REG to inform about the
  time when a next eDRX page will start
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_mmr_phy_cmd_ind (uint32 arg)
{

  mmr_edrx_page_start_s_type mmr_ind; 
  mmr_ind.message_header.message_set = MS_MM_REG;
  mmr_ind.message_header.message_id  = (int)MMR_PHY_EDRX_INFO_IND;
  mmr_ind.next_page_start_time_ms = arg;  
  PUT_IMH_LEN( sizeof( mmr_ind ) - sizeof(IMH_T), &mmr_ind.message_header );
  MSG_HIGH_DS_0(MM_SUB, "=EMM= sent MMR_PHY_EDRX_INFO_IND to REG");
  mm_send_message( (IMH_T*)&mmr_ind, GS_QUEUE_REG );
}

/*=========================================================================== 
 
FUNCTION  EMM_SEND_DETACH_IND

DESCRIPTION
  This function sends EMM_SEND_DETACH_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_send_detach_ind(void)
{
  emm_detach_ind_type  detach_ind;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending NAS_EMM_DETACH_IND");
    /* Init */
  memset((void *)&detach_ind, 0x0, sizeof(emm_detach_ind_type));

  detach_ind.message_header.message_set = MS_MM_DS;
  detach_ind.message_header.message_id = (int) EMM_DETACH_IND;
  
  PUT_IMH_LEN(sizeof( emm_detach_ind_type) - sizeof(IMH_T),
                &detach_ind.message_header );

   /* Add sent messages to the debug buffer */
  mm_add_message_to_debug_buffer(detach_ind.message_header.message_set,
                                 detach_ind.message_header.message_id,
                                 FROM_MM
                                 ,(sys_modem_as_id_e_type)mm_as_id
                                 );

  //mm_send_message((IMH_T *)&detach_ind, GS_QUEUE_DS );
  ds_pdn_mgr_nas_to_ds_msg_hdlr((void *)&detach_ind);
}

#endif

#ifdef FEATURE_LTE_REL13
/*===========================================================================
FUNCTION  emm_override_dt_access

DESCRIPTION
  This function checks if overwrite the est cause for MMTEL Voice/Video
  SMSoIP, SMS over NAS and SMS over S102. This function is called only in context
  of a UE configured for low priority during TAU, SR and ESR procedures.
  NAS wil set the cause but LRRC will decide for ACB check

DEPENDENCIES
  None

RETURN VALUE
  TRUE if overwrite is allowed, else FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_override_dt_access
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  boolean skip_acb_for_low_pri = FALSE;

  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL13) &&
      ((emm_mmtel_status(emm_ctrl_data_ptr) == TRUE)
        || (emm_wms_sms_status(emm_ctrl_data_ptr) == TRUE)))
  {
      skip_acb_for_low_pri = TRUE;
  }
  return skip_acb_for_low_pri;
} /* end emm_override_dt_access() */
#endif


/*===========================================================================
FUNCTION  emm_check_if_sec_attack_mitigation_possible

DESCRIPTION
  This function return TRUE: Follow T3247 counter behavior, FALSE: no

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_check_if_sec_attack_mitigation_possible
(
  lte_nas_emm_cause_type  emm_cause,
  boolean                 is_plain_msg,
  lte_nas_message_id      msg_id,
  boolean*                dos_specific_tai
)
{
  boolean result = FALSE;
  boolean apply_t3247_feature = FALSE;
  boolean init_timer = FALSE;
  *dos_specific_tai  = FALSE;

  if (TRUE == is_plain_msg)
  {
     apply_t3247_feature = mm_get_t3247_behavior_enabled();

     if (apply_t3247_feature)
     {
        switch (msg_id)
        {
          case EMM_AUTHENTICATION_REJECT:
             /* 
                Start timer and return FALSE to follow legacy behavior for following cases
                a. max_count = 0;
                b. has reached max+1 rejects
             */
             init_timer = TRUE;

             /* For counter < max_count, return TRUE and follow DoS mitigation behavior. */
             if (mm_emm_usim_gprs_counter < mm_emm_max_attempt_counter_dos_mitigation)
             {
                result = TRUE;
             }

             break;

          case TRACKING_AREA_REJECT:
          case ATTACH_REJECT:
          case SERVICE_REJECT:
            switch (emm_cause)
            {
              case LTE_NAS_ILLEGAL_UE:
              case LTE_NAS_ILLEGAL_ME:
              case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:

                 /* 
                    Start timer and return FALSE to follow legacy behavior for following cases
                    a. max_count = 0;
                    b. has reached max+1 rejects
                */
                init_timer = TRUE;

                /* For counter < max_count, return TRUE and follow DoS mitigation behavior. */
                if ( (mm_emm_usim_gprs_counter     < mm_emm_max_attempt_counter_dos_mitigation) &&
                     (mm_emm_usim_non_gprs_counter < mm_emm_max_attempt_counter_dos_mitigation) )
                {   
                  result = TRUE;
                }

                break;

              case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
                 /* 
                   Start timer and return FALSE to follow legacy behavior for following cases
                   a. max_count = 0;
                   b. has reached max+1 rejects
                */
                init_timer = TRUE;

                /* For counter < max_count, return TRUE and follow DoS mitigation behavior. */
                if (mm_emm_usim_gprs_counter < mm_emm_max_attempt_counter_dos_mitigation)
                {
                   result = TRUE;
                }
                break;

              case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
              case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
              case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
                /*Special handling for cause #12,13,15 - return FALSE to apply legacy behavior */
                *dos_specific_tai = TRUE;
                init_timer = TRUE;
                break;

              case LTE_NAS_PLMN_NOT_ALLOWED:
              case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
                 init_timer = TRUE;
                 result = TRUE;
                 break;

              default:
                 break;
           }
           break;

        default:
           break;
      }
    }
  }

  if ((TRUE == init_timer) && (mm_timer_status[ TIMER_T3247 ] != TIMER_ACTIVE) )
  {
    mm_start_timer(TIMER_T3247, DEFAULT_TIMEOUT);
  }
  
  MSG_HIGH_DS_1(MM_SUB, "=EMM= emm_check_if_sec_attack_mitigation_possible() returns %d", result);
  return result;
}

/*===========================================================================
FUNCTION    emm_process_update_sec_attack_mitigation_counters

DESCRIPTION
  This function updates the T3247 supported counters based on EMM cause

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_update_sec_attack_mitigation_counters
(
  lte_nas_emm_cause_type  emm_cause,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
   sys_plmn_id_s_type      plmn;

   MSG_HIGH_DS_0(MM_SUB, "=EMM= emm_process_update_sec_attack_mitigation_counters()");
   /*Spec TS 24.301, section 5.3.7b */
   switch (emm_cause)
   {
     case LTE_NAS_ILLEGAL_UE:
     case LTE_NAS_ILLEGAL_ME:
     case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
     case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
        mm_emm_usim_gprs_counter++;

        if (emm_cause != LTE_NAS_EPS_SERVICES_NOT_ALLOWED)
        {
           mm_emm_usim_non_gprs_counter++;
        }

	MSG_HIGH_DS_2(MM_SUB, "=EMM= USIM GPRS counter = %d and mm_emm_usim_non_gprs_counter = %d", mm_emm_usim_gprs_counter,mm_emm_usim_non_gprs_counter);

        emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED,
                                     SYS_SIM_STATE_AVAILABLE,
                                     emm_ctrl_data_ptr,
                                     TRUE) ;

        MSG_HIGH_DS_0(MM_SUB, "=EMM= Add TAI to FTAI list for roaming");
        emm_add_forbidden_tai(emm_ctrl_data_ptr->rrc_forbidden_list_ptr,
                              emm_get_camped_tai(emm_ctrl_data_ptr,TRUE),
                              emm_ctrl_data_ptr);
        emm_add_manual_ftai(emm_ctrl_data_ptr, TRUE);

        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE);
        emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);

        break;

     case LTE_NAS_PLMN_NOT_ALLOWED:
     case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
        plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

        emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED,
                                       SYS_SIM_STATE_AVAILABLE,
                                       emm_ctrl_data_ptr,
                                       TRUE) ;

        if(
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
             (reg_sim_per_subs_is_ehplmn(plmn,(sys_modem_as_id_e_type)mm_sub_id) == FALSE)
#else
             (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), plmn) == FALSE)
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
             (reg_sim_is_ehplmn(plmn) == FALSE)
#else
             (sys_plmn_match(reg_sim_read_hplmn(), plmn) == FALSE)
#endif
#endif
           )
        {
           switch (emm_cause)
           {
             case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
               if (mm_emm_max_attempt_counter_dos_mitigation <= 1)
               {
                  /* Add current PLMN into GPRS forbidden PLMN list */
                  mm_add_plmn_to_gprs_fplmn_list(plmn, FALSE);
               }
               else
               {
                  mm_add_plmn_to_gprs_fplmn_list(plmn, TRUE);
               }
               break;

             case LTE_NAS_PLMN_NOT_ALLOWED:
               if (mm_emm_max_attempt_counter_dos_mitigation <= 1)
               {
                  /* Add current PLMN into forbidden PLMN list */
                  mm_add_plmn_to_fplmn_list(plmn, FALSE);
               }
               else
               {
                  mm_add_plmn_to_fplmn_list(plmn, TRUE);
               }
               break;

           default:
              break;
           }

           EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);
           emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);
        }
        else
        {
           MSG_HIGH_DS_0(MM_SUB, "=EMM= Add TAI to FTAI list for roaming");
           emm_add_forbidden_tai(emm_ctrl_data_ptr->rrc_forbidden_list_ptr,
                                 emm_get_camped_tai(emm_ctrl_data_ptr,TRUE),
                                 emm_ctrl_data_ptr);
           emm_add_manual_ftai(emm_ctrl_data_ptr, TRUE);

           EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE);
           emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);
        }
 
        break;

    default:
      break;
   }
}

/*=========================================================================== 
 
FUNCTION  emm_clear_forbidden_tai_list

DESCRIPTION
  clears the ftai list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_clear_forbidden_tai_list
(
  lte_nas_tai_lst2_type *forbidden_list,
  boolean               dos_specific
)
{
  lte_nas_tai_lst1_type  tai_tobe_deleted;
  int index = 0;
  int count = 0; 
  /* TAI is added in the FTAI list with the most recent entry on top.
     When TAI at index "x" is deleted from FTAI list, entries from
     index (x+1) is moved up to index "x".
  */
  ASSERT(forbidden_list != (lte_nas_tai_lst2_type *)NULL); 
  ASSERT(forbidden_list->tai_lst_length <= LTE_RRC_MAX_NUM_TRACKING_AREAS); 

  count = forbidden_list->tai_lst_length;

  MSG_HIGH_DS_1(MM_SUB, "=EMM= emm_clear_forbidden_tai_list(): list count = %d", count);

  while (count > 0)
  {
     if (forbidden_list->diff_plmn[index].t3247_specific == dos_specific)
     {
       tai_tobe_deleted.plmn = forbidden_list->diff_plmn[index].plmn;
       tai_tobe_deleted.tac  = forbidden_list->diff_plmn[index].tac;

       emm_delete_forbidden_tai(&tai_tobe_deleted,
                                forbidden_list);
     }
     else
     {
        index++;
     }
     count--;
  }   

  return;
}

/*=========================================================================== 
 
FUNCTION  emm_clear_manual_forbidden_tai_list

DESCRIPTION
  clears the manual ftai list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_clear_manual_forbidden_tai_list
(
  boolean  dos_specific
)
{
  lte_nas_tai_lst1_type  tai_tobe_deleted;
  int index = 0;
  int count = 0; 

  emm_ctrl_data_type  *emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL); 

  if(emm_ctrl_data_ptr->manual_ftai_list_ptr == NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= This function should not be called from this SUB");
    return;
  }

  count = emm_ctrl_data_ptr->manual_ftai_list_ptr->tai_lst_length;

  MSG_HIGH_DS_1(MM_SUB, "=EMM= emm_clear_manual_forbidden_tai_list(): list count = %d", count);

  (void)pthread_mutex_lock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);

   while (count > 0)
   {
       if (emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[index].t3247_specific == dos_specific)
       {
          tai_tobe_deleted.plmn = emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[index].plmn;
          tai_tobe_deleted.tac  = emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[index].tac;

          emm_delete_forbidden_tai(&tai_tobe_deleted,
                                   emm_ctrl_data_ptr->manual_ftai_list_ptr);
       }
       else
       {
          index++;
       }
       count--;
   }

  (void)pthread_mutex_unlock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);

  return;
}

/*=========================================================================== 
 
FUNCTION  emm_is_tai_forbidden_due_to_dos_mitigation

DESCRIPTION
  checks if a particular TAI is forbidden due to DoS mitigation feature

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
boolean emm_is_tai_forbidden_due_to_dos_mitigation
(
  lte_nas_tai_lst1_type   tai_to_be_searched,
  lte_nas_tai_lst2_type   *tai_list
)
{
   byte list_index  = 0;
   boolean result = FALSE;

   ASSERT(tai_list != NULL);

   for (list_index=0; list_index < tai_list->tai_lst_length; list_index++)
   {
     /* Check whether extracted PLMN matches and is forbidden due to DoS mitigation*/
     if ((tai_list->diff_plmn[list_index].t3247_specific == TRUE) &&
         (sys_plmn_match((tai_list->diff_plmn[list_index].plmn), tai_to_be_searched.plmn) == TRUE)&&
         (tai_list->diff_plmn[list_index].tac == tai_to_be_searched.tac))
     {
       /* TAI is found in the TAI_LIST */
       result = TRUE;
       break;
     }       
   }
   
   MSG_HIGH_DS_1(MM_SUB, "=EMM= emm_is_tai_forbidden_due_to_dos_mitigation(): result = %d", result);
   return result;  
}

/*=========================================================================== 
 
FUNCTION  emm_is_tai_forbidden_for_manual_due_to_dos_mitigation

DESCRIPTION
  checks if a particular TAI is forbidden due to DoS mitigation feature

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
boolean emm_is_tai_forbidden_for_manual_due_to_dos_mitigation
(
  lte_nas_tai_lst1_type   tai_to_be_searched,
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  byte list_index = 0;
  boolean result = FALSE;

  ASSERT(emm_ctrl_data_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->manual_ftai_list_ptr != NULL);

  (void)pthread_mutex_lock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);

  for (list_index=0; list_index < emm_ctrl_data_ptr->manual_ftai_list_ptr->tai_lst_length; list_index++)
  {
     /* Check whether extracted PLMN matches and is forbidden due to DoS mitigation*/
     if ((emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[list_index].t3247_specific == TRUE) &&
         (sys_plmn_match((emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[list_index].plmn), tai_to_be_searched.plmn) == TRUE)&&
         (emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[list_index].tac == tai_to_be_searched.tac))
     {
       /* TAI is found in the TAI_LIST */
       result = TRUE;
       break;
     }       
  }

  (void)pthread_mutex_unlock(emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr);
 
  MSG_HIGH_DS_1(MM_SUB, "=EMM= emm_is_tai_forbidden_for_manual_due_to_dos_mitigation(): result = %d", result);
  return result;  
}

/*=========================================================================== 
 
FUNCTION  emm_evaluate_if_sim_revalidation_req

DESCRIPTION
  checks if SIM needs to revalidated when LPM mode is turned off

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_evaluate_if_sim_revalidation_req
(
  sys_sim_state_e_type    previous_sim_state,
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
   ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);
   ASSERT(emm_ctrl_data_ptr->is_sim_ps_invalid_due_to_dos != NULL); 
   ASSERT(emm_ctrl_data_ptr->is_sim_cs_invalid_due_to_dos != NULL);

   if (mm_get_t3247_behavior_enabled() && (mm_emm_max_attempt_counter_dos_mitigation == 0) )
   {
       *emm_ctrl_data_ptr->is_sim_ps_invalid_due_to_dos = TRUE;
       if ( (previous_sim_state == SYS_SIM_STATE_AVAILABLE) &&
            (emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_CS_PS_INVALID) )
       {
	  *emm_ctrl_data_ptr->is_sim_cs_invalid_due_to_dos = TRUE;

       }
   }

   return;
}

#ifdef FEATURE_CIOT
/*=========================================================================== 
 
FUNCTION  emm_is_mo_excep_nas_config_enabled

DESCRIPTION
   This function return TRUE if MO exception is enabled from config  else FALSE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/

boolean emm_is_mo_excep_nas_config_enabled(void)
{

   return reg_sim_per_subs_chk_mo_excep_cfg((sys_modem_as_id_e_type)mm_sub_id);
}
/*=========================================================================== 
 
FUNCTION  emm_is_mo_exception_barred

DESCRIPTION
   This function return TRUE if MO exception is barred else FALSE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/


boolean emm_is_mo_exception_barred(void)
{
   boolean result = FALSE;
   emm_ctrl_data_type	*emm_ctrl_data_ptr = emm_db_get_ctrl_data();
   ASSERT(emm_ctrl_data_ptr != NULL); 
   if((emm_ctrl_data_ptr->access_barring_flag & EMM_MO_EXCEPTION_BARRED)>0)
   {
     result =  TRUE;
   }
   else 
   {
     result =  FALSE;
   }	
   MSG_HIGH_DS_1(MM_SUB, "=EMM= [debug] emm_is_mo_exception_barred result = %d", result);
   return result;
}

/*=========================================================================== 
 
FUNCTION  emm_is_t3446_started_for_mo_exception

DESCRIPTION
   This function return TRUE if Timer t3346 is not running for mo exception cause

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/

boolean emm_is_t3446_started_for_mo_exception()
{
  boolean result = FALSE;
  emm_ctrl_data_type  *emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL); 

  if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) && 
  	  (emm_ctrl_data_ptr->is_t3346_started_for_mo_excp == TRUE))
  {
    result = TRUE;
  }
   MSG_HIGH_DS_1(MM_SUB, "=EMM= [debug] emm_is_t3446_started_for_mo_exception result = %d", result);
  return result;
}
/*=========================================================================== 
 
FUNCTION  emm_call_required_timer_expiry

DESCRIPTION
   This function return TRUE if posting call successul

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/

boolean  emm_call_required_timer_expiry(void)
{
  boolean result = FALSE;
  emm_ctrl_data_type  *emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL); 
  
  if((emm_ctrl_data_ptr->access_barring_flag & EMM_MO_SIGNALING_BARRED)>0)
   {
	 result = TRUE;
	 emm_ctrl_data_ptr->emm_start_mo_excp_signalling = TRUE;
#if defined FEATURE_DUAL_SIM
	 emm_post_timer_expiry(mm_as_id,EMM_ACCESS_BARRING_TIMER);
#else
	 emm_post_timer_expiry(EMM_ACCESS_BARRING_TIMER);
#endif
	
  }
  else if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) && 
       (emm_is_t3446_started_for_mo_exception() == FALSE))
  {
	
     result = TRUE;
     emm_ctrl_data_ptr->emm_start_mo_excp_signalling = TRUE;
#if defined FEATURE_DUAL_SIM
     emm_post_timer_expiry(mm_as_id,TIMER_T3346);
#else
     emm_post_timer_expiry(TIMER_T3346);
#endif
		   
  }	
  MSG_HIGH_DS_1(MM_SUB, "=EMM= [debug] emm_call_required_timer_expiry result = %d", result);
  
  return result; 

}

/*************************************
Function Name EMM_MO_EXCEPTION_ATTACHED_ALLOWED

Function description
Return TRUE if 
	UE is in dereg state/REG initaited state
	timer 3346 running or barring timer running ? If(timer 3346 running it is not started for MO exception)
	UE not barred for MO exception 
ELSE FALSE

Argument :- No argument for NOW
Return Type:- TRUE or FALSE
*********************************************/


boolean emm_mo_exception_attach_allowed (sys_modem_as_id_e_type sub_id)
{
  boolean result = FALSE;
#if !defined FEATURE_DUAL_SIM
(void)sub_id;
#endif
  if(emm_ctrl_data_ptr->plmn_service_state->active_rat != SYS_RAT_LTE_NB1_RADIO_ACCESS)
  {
    return result;
  }
  if(emm_is_mo_exception_barred() == FALSE)
  {
   if(EMM_GET_STATE() == EMM_DEREGISTERED)
   {
    result = emm_call_required_timer_expiry();
   }
   else if(EMM_GET_STATE() == EMM_REGISTERED_INITIATED)
   {
     emm_ctrl_data_ptr->emm_start_mo_excp_signalling = TRUE;
     result = TRUE;
   }
  }
  MSG_HIGH_DS_2(MM_SUB, "=EMM= emm_mo_exception_attach_allowed result = %d emm_state = %d", result,EMM_GET_STATE());

  return result;

}

/*=========================================================================== 
 
FUNCTION  EMM_SEND_DS_BEARER_CTXT_TRANSFER_IND

DESCRIPTION
  This function sends NAS_EMM_BEARER_CTXT_TRANSFER_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_send_ds_bearer_ctxt_transfer_ind
(
  sys_radio_access_tech_e_type   rat
)
{
  emm_bearer_ctxt_transfer_ind_type  bearer_ctxt_transfer_ind;
  
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sending EMM_BEARER_CTXT_TRANSFER_IND attach without pdn %d ",emm_ctrl_data_ptr->attach_without_pdn);

  /* Init */
  memset((void *)&bearer_ctxt_transfer_ind, 0x0, sizeof(emm_bearer_ctxt_transfer_ind_type));

  bearer_ctxt_transfer_ind.message_header.message_set = MS_MM_DS;
  bearer_ctxt_transfer_ind.message_header.message_id = (int) EMM_BEARER_CTXT_TRANSFER_IND;

  bearer_ctxt_transfer_ind.rat =   rat;

  PUT_IMH_LEN(sizeof(emm_bearer_ctxt_transfer_ind_type) - sizeof(IMH_T),
                &bearer_ctxt_transfer_ind.message_header );

  /* Add sent messages to the debug buffer */
  mm_add_message_to_debug_buffer(bearer_ctxt_transfer_ind.message_header.message_set,
                                 bearer_ctxt_transfer_ind.message_header.message_id,
                                 FROM_MM
                                 ,(sys_modem_as_id_e_type)mm_as_id
                                 );

  ds_pdn_mgr_nas_to_ds_msg_hdlr((void *)&bearer_ctxt_transfer_ind);

}

void emm_process_pending_mo_exception_data(boolean clear_mo_exception) 
{
  emm_process_pending_mo_exception_data_s_type  process_pending_mo_exception_data;
  
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sending EMM_PROCESS_PENDING_MO_EXCEPTION_DATA_IND, is_mo_exception_to_be_cleared - %d",clear_mo_exception);

  /* Init */
  memset((void *)&process_pending_mo_exception_data, 0x0, sizeof(emm_process_pending_mo_exception_data_s_type));

  process_pending_mo_exception_data.message_header.message_set = MS_MM_DS;
  process_pending_mo_exception_data.message_header.message_id = (int) EMM_PROCESS_PENDING_MO_EXCEPTION_DATA_IND;

  process_pending_mo_exception_data.is_mo_exception_to_be_cleared = clear_mo_exception;

 
  PUT_IMH_LEN(sizeof( emm_process_pending_mo_exception_data_s_type) - sizeof(IMH_T),
                &process_pending_mo_exception_data.message_header );

  /* Add sent messages to the debug buffer */
  mm_add_message_to_debug_buffer(process_pending_mo_exception_data.message_header.message_set,
                                 process_pending_mo_exception_data.message_header.message_id,
                                 FROM_MM
                                 ,(sys_modem_as_id_e_type)mm_as_id
                                 );

  ds_pdn_mgr_nas_to_ds_msg_hdlr((void *)&process_pending_mo_exception_data);

}

#ifdef FEATURE_FMD_SPEED_INFO
void emm_handle_fmd_psm_fallback_timer_expiry(emm_ctrl_data_type *emm_ctrl_data_ptr)
{
   sys_psm_status_e_type status = emm_check_ready_for_psm(emm_ctrl_data_ptr);
   emm_ctrl_data_ptr->fmd_psm_fallback_timer_expired = TRUE;

  if(status == SYS_PSM_STATUS_READY)
  {
      mm_send_mmr_psm_auto_ready_ind();
      emm_ctrl_data_ptr->psm_active = TRUE;
  }
}
#endif

#endif

#endif /*FEATURE_LTE*/


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

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/emm_rrc_handler.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/26/15   us     If Timer 3346 is running and EMM substate is EMM_REGISTERED_ATT_TO_UPDATE 
                  then on receiving a page indication do TAU.   
09/17/12   cogolt Added support for Rel-10 ESM backoff timer
07/05/12   sar    Rel-10 change when no active bearers
05/29/12   pahire  Added changes for UE Specific DRX Cycle for QChat/LTE 
01/19/12   MNK   Fix for CR 331514 (1xcsfb: NAS needs to distnguish RLF vs OOS failure when indicating the failure to CM)  
08/09/11   abhi   reseting deact_cnf and ignore_gw_activation on receiving ph_staus_change_cnf
07/04/11   Abhi   Send ph status confirm to REG on receiving deactivate confirm from LRRC
06/16/11   PA      Added changes for Rel 9 Emergency Bearer services  
05/27/11   PA      Added changes for Security Key generation during W2L PS HO
02/01/10   PA      Added changes for Security Key generation during L2W PS HO
11/29/10   MNK     More 1XCSFB changes including adversarial scenarios
11/01/10   MNK     Introduce e1XCSFB 
10/07/10   MNK     1XCSFB introduction 
09/23/10   MNK     Handle Extended service reject for CSFB flows 
09/20/10   MNK     ESR validation for MT calls added 
09/17/10   MNK     CS_NOTIFICATION for 3GPPCSFB added
08/09/10   MNK     Make code compatible for 1xCSFB & 3GPPCSFB
08/30/10   MNK     CSFB changes
08/13/10   MNK     Reset reg_req_pended_indication upon deactivate_cnf 
08/07/10   MNK     Reset emm ctrl data when moving from DEREGISTERED to DEREGISTERED_NO_CELL_AVAILABLE to cover up the scenario 
                   where LTE attach failed and BSR took UE to 1X and then UE came back to LTE, but EMM uses the cached PDN connect request
                   as emm ctrl data is nort cleared when UE moved to DEREGISTERED_NO_CELL_AVAILABLE previously.
06/30/10   zren    Updated to only process unsent messages in idle state when 
                   receiving service ind w/ no service 
06/29/10   zren    Updated EMM to handle service request procedure and unsent 
                   messages correctly for redirection and reselection 
06/25/10   zren    Updated TAU type setting for TAU for IRAT 
06/23/10   zren    Updated EMM to send camped indication after receiving 
                   LTE_RRC_SERVICE_IND due to MMR_REG_REQ after IRAT 
05/26/10   zren    Updated to send LIMITED SERVICE status in REG CNF for 
                   LIMITED SERVICE REQ when get service available service ind 
                   from RRC
05/26/10   zren    Updated to only process pending REG REQ or message when EMM 
                   is in IDLE state after received a service available 
                   RRC SERVICE IND
05/18/10   zren    Updated fun param to set only_for_reg flag in REG CNF and 
                   REG IND
05/17/10   zren    Updated EMM to only process EMM timers
05/11/10   RI      Mainlined spec changes upto Dec 09.
05/11/10   zren    Updated to use trans ID rcved in PLMN SEARCH REQ
05/07/10   zren    Merged function emm_rrc_process_service_indication and 
                   function emm_rrc_process_normal_service_indication
                   Fixed to not process ACT DEF BC REQ when attach accept 
                   carries unexpected EMM cause and is treated as attach reject
                   Added code to handle found plmn list in RRC SERVICE IND
05/04/10   RI      Modified the decoding of NAS messages to use the DL cnt from 
                   the SQN in the message.
04/19/10   MNK     Minor Changes - F3 messages
04/13/10   MNK     ISR implementation
04/09/10   zren    Added combined procedure feature
03/16/10   RI      Handling of security header information.
03/16/10   zren    Updated MO detach procedure upon receiving MMR_STOP_MODE
02/24/10   zren    Added support for network selection modes of LIMITED MANUAL
                   ACQUISITION and LIMITED ROUTINE ACQUISITION
02/23/10   RI      Reset the secure exchange of NAS messages when in RRC
                   connection releasing state.
02/23/10   MNK     Added support for GW to LTE i-RAT
02/19/10   MNK     Remove the protocol state error checks for security and emm information
02/16/10   MNK     Add ignore_GW_activation_ind
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
02/05/10   zren    Updated processing of connection abort in waiting for RRC 
                   confirmation state
02/03/10   MNK     Fix emm state transitions for lte-eHRPD scenario
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
01/30/10   MNK     Fix issues with service_ind handling.
01/22/10   RI      Updated the handling of context during detach.
01/21/10   zren    Added EMM/ESM STATUS feature
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
11/16/09   RI      Added support for EMM INFORMATION request.
11/04/09   MNK     Cleanup emm_ctrl_data as necessary if RRC is deactivated due to non-i-RAT
10/14/09   MNK     Commented clearing the emm_ctrl_data when deactivate_cnf is received for now
10/05/09   hnam    Added support for TLB
09/26/09   RI      Added support for GUTI Reallocation command.
09/21/09   MNK     Changes due to IRAT design
09/25/09   hnam    Added TLB support
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/20/09   zren    Updated to stop barring timers if new cell is detected
08/04/09   hnam    Added support for taking care of ACT DEF EPS BEARER CONTEXT REQ
                    (on receipt of corrupted ATTACH ACCEPT message)
07/31/09   hnam    Replaced LTE_ERROR with ERR in switch case of ESM in 
                     emm_process_rrc_data_indication(), as this was asserting
07/29/09   zren    Added support to use global cell_identity
07/01/09   tmai    Add the code to handle detach USIM removal case for 
                   detach and rrc service indication collision
06/18/09   zren    Modified emm_registered_process_rrc_service_ind to process 
                   RRC_SERVICE_IND received in REGISTERED_PLMN_SEARCH state
                   to either move to REGISTERED_NORMAL_SERVICE or statr TAU
06/13/09   RI      Updated the NAS security processing.
                   Added Identity Request support.
06/11/09   zren    Added support to trigger TAU when UE returns back to 
                   REGISTERED_NORMAL_SERVICE state and EPS bearer context was
                   updated in earlier REGISTERED_NO_CELL_AVAILABLE state
06/02/09   MNK     CR# 184007. These are the changes required to make sure at the time of attach, 
                   EMM receives (ATTACH ACCEPT + ACTIVATE DEFAULT BEARER CONTEXT REQUEST) and EMM sends
                    (ATTACH COMPLETE + ACTIVATE DEFAULT BEARER CONTEXT ACCEPT)
05/29/09   zren    Added support for TAU feature
05/17/09   hnam    Added the code to support security
05/14/09   zren    Added EPS status and SIM state checks before starting Service
                   Request Procedure 
05/11/09   zren    Added support for MT detach procedure 
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
04/15/09   RI      Added support for LTE NAS Authentication procedure.
04/15/09   zren    Completed MO detach feature
04/02/09   zren    Added support for new transaction ID format 
03/26/09   RI      Added support for GS Timers in LTE.
03/24/09   zren    Added support for service request and paging feature
03/12/09   RI      Added support for SMC.
03/03/09   hnam    Added checking for the validity of the NAS messages parsed by the MSG_LIB after decoding
02/26/09   vdr     Fixed Lint errors and warnings
02/09/09   vdr     Fixed lte_rrc_conn_est_cnf_s processing according to lte_rrc_ext_msg.h#54
02/06/09   MNK     Fixed the bug in emm_process_rrc_ul_data_cnf where wrong trasn_id is being used to match the pending message
02/06/09   vdr     Modified lte_rrc_conn_est_cnf_s processing according to lte_rrc_ext_msg.h#54
02/05/09   MNK     Changed decode_nas_msg to decode_nas_msg_fp and nas_retrieve_msg to nas_retrieve_msg_fp
01/30/09   vdr     Added processing of new LTE_RRC_CONN_EST_TIMER_UPDATE_IND
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "emm.h"
#include "rex.h"
#include "emm_database.h"
#include "esm_utils.h"
#include "emm_connection_handler.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_irat_msg.h"
#include "emm_update_lib.h"
#include "lte_nas_common_v.h"
#include "emm_utility.h"
#include "emm_rrc_if.h"
#include "emm_reg_handler.h"
#include "emm_rrc_emm_if.h"
#include "emm_update_lib.h"
#include "lte_nas_emm_message_type.h"
#include "lte_nas_msg_parse.h"
#include "lte_nas.h"
#include "emm_esm_handler.h"
#include "emm_sim_handler.h"
#include "sys_stru_v.h"
#include "emm_service_request_lib.h"
#include "emm_procedure_reject_handler.h"
#include "emm_security.h"
#include "lte_nas_security_info.h"
#include "err.h"
#include "mm_v.h"
#include "lte_nas_tlb.h"
#include "emm_cnm_handler.h"
#include "gmm_int_v.h"
#include "emm_csfb_handler.h"
#include "mm_multimode_common.h"
#include "ds3gpp_api.h"
#include "emm_mem.h"
#include "ref_cnt_obj.h"
#include <dsm_item.h>
#include <dsm_init.h>
#ifdef FEATURE_LTE_REL9
#include "emm_gps_handler.h"
#endif
#include "sys_eplmn_list_v.h"
#include "stringl.h"
#include "mmgsdilib.h"
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
#include "reg_state.h"
#endif
#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE)) 
#error code not present
#endif // TEST_FRAMEWORK

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/* Global to keep track of the DL SQN for DL msgs */
byte  dl_sqn_for_current_msg;
/* Global to check whether read security context only */
boolean  read_security_context_only = FALSE;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_validate(a) sys_eplmn_per_nas_stacks_list_validate(a,(sys_modem_as_id_e_type)mm_as_id)
#endif

#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_ENHANCED_NW_SELECTION
#define reg_sim_read_ens_flag() reg_sim_per_nas_stacks_read_ens_flag((sys_modem_as_id_e_type)mm_as_id)
#endif
#endif

#ifdef FEATURE_SGLTE
  extern void mmcoord_process_pstimer_expiry_merging(void);
  extern boolean mm_is_sglteplmn_in_sglte_roam(sys_plmn_id_s_type  pPlmnId);
  extern boolean mm_process_if_both_stack_has_camped_non_sglte_plmn
                                                           (mm_as_id_e_type in_as_id);
  extern mm_as_id_e_type mm_get_ps_stack_id(void);
  extern boolean mm_get_is_limited_regional_sent(void);
  extern void mm_set_is_limited_regional_sent(boolean val);
  extern boolean mm_is_non_sglte_plmn_in_sglte_home(sys_plmn_id_s_type pPlmnId);
  extern  boolean mm_process_sglte_plmn_state4_gsm_oos(sys_plmn_id_s_type pPlmnId);
  extern void mm_set_is_reg_ps_split_reg (boolean val);
#endif
extern void emm_set_mm_msg_info_for_debug_buffer(byte msg_info);
#ifdef FEATURE_CIOT
extern mm_cmd_type          mmcoord_pending_reg_list_req_message;
extern void mmcoord_process_mmr_plmn_search_req( mm_cmd_type *message );
#endif
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===========================================================================

FUNCTION  EMM_ATTACH_NEEDED_RRC_SERVICE_IND

DESCRIPTION
  This function processes the RRC SERVICE IND recived in DEREGISTERED ATTACH 
  NEEDED state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_attach_needed_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type     rrc_reported_plmn;
  lte_nas_tai_lst1_type  tai;
  emm_failure_type       emm_failure_cause;

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  sys_csg_info_s_type rrc_reported_csg; 
#endif 
#ifdef FEATURE_SGLTE
  boolean mm_is_sglte_plmn_in_roam;
#endif
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
    tai.plmn = rrc_reported_plmn;
    tai.tac = rrc_service_ind->camped_svc_info.tac;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
    rrc_reported_csg = rrc_service_ind->camped_svc_info.csg_info;
#endif 
    /* If the timer is running compare with prev PLMN and TAC */
    if(((mm_timer_status[TIMER_T3402] != TIMER_STOPPED) ||
        (mm_timer_status[TIMER_T3411] != TIMER_STOPPED)) 
       && 
       (emm_chk_ta_change(rrc_service_ind, emm_ctrl_data_ptr) == FALSE) 
       &&
       (emm_ctrl_data_ptr->manual_user_sel == FALSE))
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TA not changed, start attach after T3402/T3411 expiry ");
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
      if(mm_timer_status[TIMER_T3402] != TIMER_STOPPED)
      {
        /* T3402 is running, report limited service */
        reg_service_report(MMR_CAUSE_FAILURE_MAX_ATTEMPTED, 
                           FALSE, 
                           emm_ctrl_data_ptr);
      }
      else
      {
        /* T3411 is running, send camp ind to CM indicating UE is on LTE */
        emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
      }
    }
    else
    {       
      switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
      {
        case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
        case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
#ifdef FEATURE_SGLTE
          mm_is_sglte_plmn_in_roam= 
                  mm_is_sglteplmn_in_sglte_roam(emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   rrc_service_ind->camped_svc_info.selected_plmn));
#endif
          if((((emm_is_plmn_forbidden(rrc_reported_plmn,emm_ctrl_data_ptr) == FALSE)&&
             (emm_is_tai_forbidden(tai,emm_ctrl_data_ptr)== FALSE)
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
             && (mm_is_csg_in_white_list(rrc_reported_plmn, &rrc_reported_csg) == TRUE)
#endif 
             )
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
            || (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
#endif
            )
            &&
             (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == TRUE) &&
             (rrc_service_ind->camped_svc_info.cell_access_status != 
                                           LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)
#ifdef FEATURE_SGLTE
              && (mm_is_sglte_plmn_in_roam == FALSE)
#endif
            )
          {
            emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);                          

            if((emm_chk_ta_change(rrc_service_ind, emm_ctrl_data_ptr) == FALSE) &&
               ((emm_ctrl_data_ptr->emm_attach_attempt_counter >0)&&
               (emm_ctrl_data_ptr->emm_attach_attempt_counter < MAX_NUMBER_ATTACH_OF_ATTEMPTS)))
            {
              emm_init_attach_procedure(EMM_UPDATE_RECOVERY, emm_ctrl_data_ptr);
            }
            else
            {
              if (sys_plmn_match(tai.plmn,emm_ctrl_data_ptr->last_attempted_tai.plmn) != TRUE) 
              {
                emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
              }
              emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
            }
          }
          else
          {
#ifdef FEATURE_SGLTE
            if (mm_is_sglte_plmn_in_roam == TRUE)
            {
              emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
              reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);
              MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited to REG, REG will send Limited Regional to CM ");
            }
            else
#endif
            {
              emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
              reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
            }
            emm_send_reg_attach_failed_ind();                             
          }
          break;
        case SYS_NETWORK_SELECTION_MODE_MANUAL:
        case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
#ifdef FEATURE_SGLTE
          mm_is_sglte_plmn_in_roam= 
          mm_is_sglteplmn_in_sglte_roam(emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   rrc_service_ind->camped_svc_info.selected_plmn));
#endif
          if((emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE)
#ifdef FEATURE_SGLTE
              && (mm_is_sglte_plmn_in_roam == FALSE)
#endif
          )
          {
            emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
            if((emm_chk_ta_change(rrc_service_ind, emm_ctrl_data_ptr) == FALSE) &&
               (emm_ctrl_data_ptr->manual_user_sel == FALSE) &&
               ((emm_ctrl_data_ptr->emm_attach_attempt_counter >0)&&
               (emm_ctrl_data_ptr->emm_attach_attempt_counter < MAX_NUMBER_ATTACH_OF_ATTEMPTS)))
            {
              emm_init_attach_procedure(EMM_UPDATE_RECOVERY, emm_ctrl_data_ptr);
            }
            else
            {
              if (sys_plmn_match(tai.plmn,emm_ctrl_data_ptr->last_attempted_tai.plmn) != TRUE) 
              {
                emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
              }
              emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
            }
          }
          else
          {
#ifdef FEATURE_SGLTE
            if (mm_is_sglte_plmn_in_roam == TRUE)
            {
              emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
              reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);              
              MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited TO REG, REG will send Limited Regional to CM ");
            }
            else
#endif
            {
              emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
              reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
            }
            

            emm_send_reg_attach_failed_ind();                             
          }
          break;
        case SYS_NETWORK_SELECTION_MODE_LIMITED:
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
          break;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
        case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
          if((emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
             && (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == TRUE) 
             && (rrc_service_ind->camped_svc_info.cell_access_status != 
                                           LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY))
          {
            emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
            emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
          }
          else
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }
          break;
#endif
        default:
          MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW Sel Mode - %u",
                         emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
          break;
      }
    }
  }
  else if (rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_NOT_AVAILABLE)
  {
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                          SYS_SRV_STATUS_NO_SRV;
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

    emm_send_reg_attach_failed_ind();                                  
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE); 
  }

} /* end of emm_attach_needed_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_WAITING_PDN_CONN_REQ_RRC_SERVICE_IND

DESCRIPTION
  This function processes the RRC SERVICE IND recived in DEREGISTERED WAITING 
  PDN CONN REQ state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_waiting_pdn_conn_req_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type     rrc_reported_plmn;
  lte_nas_tai_lst1_type  tai;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  sys_csg_info_s_type rrc_reported_csg;
#endif 

  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  tai.plmn = rrc_reported_plmn;
  tai.tac = rrc_service_ind->camped_svc_info.tac;

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  rrc_reported_csg = rrc_service_ind->camped_svc_info.csg_info;
#endif 

  switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
  {
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
      if((((emm_is_plmn_forbidden(rrc_reported_plmn,emm_ctrl_data_ptr) == TRUE) ||
            (emm_is_tai_forbidden(tai,emm_ctrl_data_ptr) == TRUE)
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
         || (mm_is_csg_in_white_list(rrc_reported_plmn, &rrc_reported_csg) == FALSE) 
#endif 
         )
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
        && (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == FALSE)
#endif 
         )
         ||
         (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == FALSE) 
         ||
         (rrc_service_ind->camped_svc_info.cell_access_status == 
                                           LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)
        )
      {
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
      }
      break;
    case SYS_NETWORK_SELECTION_MODE_MANUAL:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
      if(emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == FALSE)
      {
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
      }
      break;
    case SYS_NETWORK_SELECTION_MODE_LIMITED:
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
      break;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
    case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
      if((emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == FALSE)
         ||
         (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == FALSE) 
         ||
         (rrc_service_ind->camped_svc_info.cell_access_status == 
                                           LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY))
      {
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
      }  
      break;
#endif
    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW Sel Mode - %u",
                     emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
      break;
  }

} /* end of emm_waiting_pdn_conn_req_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_NO_SERVICE_RRC_SERVICE_IND

DESCRIPTION
  This function processes the RRC SERVICE IND recived in DEREGISTERED NO CELL
  AVAILABLE state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_no_service_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type  tai;
  sys_plmn_id_s_type rrc_reported_plmn ;
#ifdef FEATURE_SGLTE
  boolean mm_is_sglte_plmn_in_roam;
#endif

  tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  tai.tac = rrc_service_ind->camped_svc_info.tac;

  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(rrc_service_ind->camped_svc_info.selected_plmn);

  if (rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
    {
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
#ifdef FEATURE_SGLTE
        mm_is_sglte_plmn_in_roam= 
        mm_is_sglteplmn_in_sglte_roam(emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   rrc_service_ind->camped_svc_info.selected_plmn));
#endif
        if((emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE)
#ifdef FEATURE_SGLTE
              && (mm_is_sglte_plmn_in_roam == FALSE)
#endif
              &&(mm_timer_status[EMM_POWEROFF_DETACH_TIMER] != TIMER_ACTIVE)
        )
        {
          // Do not start an attach if power-off detach due to CM_STOP_MODE is pending.
          emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
#ifdef FEATURE_SGLTE
          if (mm_is_sglte_plmn_in_roam == TRUE)
          {
             emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
             reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);
             MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited TO REG, REG will send Limited Regional to CM ");
          }
          else
#endif
          { 
             emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
             reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }
          
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_MANUAL:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
#ifdef FEATURE_SGLTE
        mm_is_sglte_plmn_in_roam= 
        mm_is_sglteplmn_in_sglte_roam(emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   rrc_service_ind->camped_svc_info.selected_plmn));
#endif
        if((emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE)
#ifdef FEATURE_SGLTE
            && (mm_is_sglte_plmn_in_roam == FALSE)
#endif
            &&(mm_timer_status[EMM_POWEROFF_DETACH_TIMER] != TIMER_ACTIVE) 
        )
        {
          // Do not start an attach if power-off detach due to CM_STOP_MODE is pending.
          emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
#ifdef FEATURE_SGLTE
          if (mm_is_sglte_plmn_in_roam == TRUE)
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);
            MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited TO REG, REG will send Limited Regional to CM ");
          }
          else
#endif   
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }
          
        }
        break;
#ifdef FEATURE_1XSRLTE
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
        if(emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE)            
        {
          emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                                 SYS_REG_DOMAIN_PS_SERVICE_ONLY;
        }
        else 
        {
          emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                                 SYS_REG_DOMAIN_CAMPED_ONLY;
        }
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                 SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
        break;
      case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
        if(emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE)
        {
          emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                                 SYS_REG_DOMAIN_PS_SERVICE_ONLY;
        }
        else 
        {
          emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                                 SYS_REG_DOMAIN_CAMPED_ONLY;
        }
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                 SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
        break;
#endif 
      case SYS_NETWORK_SELECTION_MODE_LIMITED:
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
        break;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
        case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
      if((emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == FALSE)
         && 
         (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == FALSE) 
         &&
         (rrc_service_ind->camped_svc_info.cell_access_status == 
                                           LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)
         &&(mm_timer_status[EMM_POWEROFF_DETACH_TIMER] != TIMER_ACTIVE)
         )
      {
        // Do not start an attach if power-off detach due to CM_STOP_MODE is pending.
        emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
        emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
      }
      else
      {
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
      }
      break;
#endif
      default:
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW Sel Mode - %u",
                       emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
        break;
    }
  }
  else if (rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_NOT_AVAILABLE)
  {
    sys_plmn_id_s_type null_plmn = {{0x00, 0x00, 0x00}} ;

    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                         SYS_SRV_STATUS_NO_SRV;

    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

    if (PLMN_MATCH(emm_ctrl_data_ptr->plmn_service_state->plmn.identity,rrc_reported_plmn.identity))
    {
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE) ; 
    }
    else
    {
      if (!PLMN_MATCH(emm_ctrl_data_ptr->plmn_service_state->plmn.identity,null_plmn.identity))
      {
        
        emm_send_rrc_service_req(SYS_NETWORK_SELECTION_MODE_AUTOMATIC,
                                 TRUE,
                                 &emm_ctrl_data_ptr->plmn_service_state->plmn,
                                 TRUE,
                                 TRUE,
                                 FALSE,
                                 FALSE,
                                 FALSE,
                                 0,
                                 emm_ctrl_data_ptr,
                                 &emm_ctrl_data_ptr->reg_req_pending_ptr->rat_pri_list_info,
                                 emm_ctrl_data_ptr->reg_req_pending_ptr->type
#ifdef FEATURE_LTE_REL9
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->additional_info
#endif                      
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->csg_id
#endif 
                                ,SYS_SCAN_SCOPE_FULL_BAND 
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->trm_timeout
#ifdef FEATURE_CIOT
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->scan_info.new_scan
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->scan_info.use_timer
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->scan_info.scan_time//FRACQ,
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->scan_info.nb1_acqdb_timer_status
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->scan_info.nb1_fullband_timer_status
#endif
                                );
      }
    }
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Invalid Service Indication");
  }
} /* end of emm_no_service_rrc_service_ind()*/

/*===========================================================================

FUNCTION  EMM_ATTEMPTING_TO_ATTACH_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received in DEREGISTERED ATTEMPTING 
  TO ATTACH state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_attempting_to_attach_rrc_service_ind
(
  lte_rrc_service_ind_s  *rrc_service_ind,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type  srving_tai;
  emm_failure_type       emm_failure_cause;

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  sys_csg_info_s_type rrc_reported_csg;
#endif 
#ifdef FEATURE_SGLTE
  boolean mm_is_sglte_plmn_in_roam;
#endif
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  /*
  ** Allow attach for an higher access UE even when T3346 is running
  */
  if((emm_chk_ta_change(rrc_service_ind, emm_ctrl_data_ptr) == FALSE) 
#ifdef FEATURE_LTE_REL11
     &&
     ((emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL11) ||
      (mm_timer_status[TIMER_T3346] == TIMER_STOPPED) ||
      (emm_check_high_priority(emm_ctrl_data_ptr) == FALSE))
#endif
    )
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= No TAI chg - Attach to be resumed upon T3411/3402 expiry");
  }
  else
  {       
    srving_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
    srving_tai.tac = rrc_service_ind->camped_svc_info.tac;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  rrc_reported_csg = rrc_service_ind->camped_svc_info.csg_info;
#endif 

    switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
    {
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
        emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;

#ifdef FEATURE_SGLTE
        mm_is_sglte_plmn_in_roam= 
        mm_is_sglteplmn_in_sglte_roam(emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   rrc_service_ind->camped_svc_info.selected_plmn));
#endif
        if((((emm_is_plmn_forbidden(srving_tai.plmn, emm_ctrl_data_ptr) == FALSE)
           &&
           (emm_is_tai_forbidden(srving_tai, emm_ctrl_data_ptr)== FALSE)
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
          && (mm_is_csg_in_white_list(srving_tai.plmn, &rrc_reported_csg) == TRUE)
#endif 
             )
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
            || (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
#endif
            ) &&
           (emm_validate_attach_plmn(emm_ctrl_data_ptr, &srving_tai.plmn) == TRUE)
           &&
           (rrc_service_ind->camped_svc_info.cell_access_status != 
                                             LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)
#ifdef FEATURE_SGLTE
              && (mm_is_sglte_plmn_in_roam == FALSE)
#endif
           )
        {
          emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
          if (sys_plmn_match(srving_tai.plmn,
                       emm_ctrl_data_ptr->last_attempted_tai.plmn) != TRUE)
          {
            emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
          }          
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
#ifdef FEATURE_SGLTE
          if (mm_is_sglte_plmn_in_roam == TRUE)
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                      SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);
            MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited to REG, REG will send Limited Regional to CM ");
          }
          else
#endif
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }
          
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_MANUAL:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
        emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;

#ifdef FEATURE_SGLTE
        mm_is_sglte_plmn_in_roam= 
        mm_is_sglteplmn_in_sglte_roam(emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   rrc_service_ind->camped_svc_info.selected_plmn));
#endif
        if((emm_is_manual_reg_allowed(srving_tai, emm_ctrl_data_ptr) == TRUE)
#ifdef FEATURE_SGLTE
              && (mm_is_sglte_plmn_in_roam == FALSE)
#endif
        )
        {
          emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
		  if (sys_plmn_match(srving_tai.plmn,
                       emm_ctrl_data_ptr->last_attempted_tai.plmn) != TRUE)
          {
            emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
          }
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
#ifdef FEATURE_SGLTE
          if (mm_is_sglte_plmn_in_roam == TRUE)
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                      SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);
            MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited to REG, REG will send Limited Regional to CM ");
          }
          else
#endif
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_LIMITED:
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
        break;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
      case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
        if((emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
           && (emm_validate_attach_plmn(emm_ctrl_data_ptr,&srving_tai.plmn) == TRUE) 
           && (rrc_service_ind->camped_svc_info.cell_access_status != 
                                         LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY))
        {
          emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                      SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
        }
        break;
#endif
      default:
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW Sel Mode - %u",
                       emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
        break;
    }
  }
} /* end of emm_attempting_to_attach_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_NORMAL_SERVICE_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received in DEREGISTERED NORMAL 
  SERVICE state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_normal_service_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type rrc_reported_plmn ;
  lte_nas_tai_lst1_type  tai;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  sys_csg_info_s_type rrc_reported_csg;
#endif 
#ifdef FEATURE_SGLTE
  boolean mm_is_non_sglte_plmn_in_home;
#endif

  tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  tai.tac = rrc_service_ind->camped_svc_info.tac;


  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(rrc_service_ind->camped_svc_info.selected_plmn);

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  rrc_reported_csg = rrc_service_ind->camped_svc_info.csg_info;
#endif 

  if (rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
    {
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
#ifdef FEATURE_SGLTE
        mm_is_non_sglte_plmn_in_home = mm_is_non_sglte_plmn_in_sglte_home(mm_serving_plmn.info.plmn);
#endif
        if((((emm_is_plmn_forbidden(tai.plmn, emm_ctrl_data_ptr) == FALSE)
           &&
           (emm_is_tai_forbidden(tai, emm_ctrl_data_ptr)== FALSE)
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
           && (mm_is_csg_in_white_list(rrc_reported_plmn, &rrc_reported_csg) == TRUE)
#endif 
             )
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
            || (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
#endif
            ) &&
           (emm_validate_attach_plmn(emm_ctrl_data_ptr, &tai.plmn) == TRUE)
           &&
           (rrc_service_ind->camped_svc_info.cell_access_status != 
                                             LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)
           
#ifdef FEATURE_SGLTE
           &&(mm_is_non_sglte_plmn_in_home==FALSE) 
#endif
       )
        {
          emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
#ifdef FEATURE_SGLTE
          if ( mm_process_sglte_plmn_state4_gsm_oos(mm_serving_plmn.info.plmn)==TRUE )
          {
             MSG_HIGH_DS_0(MM_SUB, "EMM:LTE got SGLTE PLMN in State4 While GSM OOS ");
          }
#endif
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
#ifdef FEATURE_SGLTE
          if(mm_is_non_sglte_plmn_in_home == TRUE)
          {
             emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
             MSG_HIGH_DS_0(MM_SUB, "EMM: Found NON SGLTE PLMN in SGLTE HOME sending Limited to REG, REG will send Limited Regional to CM ");
             reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);           
          }
          else
#endif
          { 
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }          
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_MANUAL:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
#ifdef FEATURE_SGLTE
        mm_is_non_sglte_plmn_in_home = mm_is_non_sglte_plmn_in_sglte_home(mm_serving_plmn.info.plmn);
#endif
        if((emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE) 
#ifdef FEATURE_SGLTE
           &&(mm_is_non_sglte_plmn_in_home==FALSE) 
#endif
         )
        {
          emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
#ifdef FEATURE_SGLTE
          if ( mm_process_sglte_plmn_state4_gsm_oos(mm_serving_plmn.info.plmn)==TRUE )
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM=LTE got SGLTE PLMN in State4 While GSM OOS ");
          }
#endif
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
#ifdef FEATURE_SGLTE
          if(mm_is_non_sglte_plmn_in_home == TRUE)
          {
             emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
             reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);
             MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited to REG, REG will send Limited Regional to CM ");
          }
          else
#endif    
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_LIMITED:
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
        break;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
      case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
        if((emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
           && (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == TRUE) 
           && (rrc_service_ind->camped_svc_info.cell_access_status != 
                                         LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY))
        {
          emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                      SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
        }
        break;
#endif
      default:
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW Sel Mode - %u",
                       emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
        break;
    }   
  }
  else if (rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_NOT_AVAILABLE)
  {
    sys_plmn_id_s_type null_plmn = {{0x00, 0x00, 0x00}} ;

    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_NO_SRV;

    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

    if (PLMN_MATCH(emm_ctrl_data_ptr->plmn_service_state->plmn.identity,rrc_reported_plmn.identity))
    {
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE) ; 
    }
    else
    {
      if (!PLMN_MATCH(emm_ctrl_data_ptr->plmn_service_state->plmn.identity,null_plmn.identity))
      {
        
        emm_send_rrc_service_req(SYS_NETWORK_SELECTION_MODE_AUTOMATIC,
                                 TRUE,
                                 &emm_ctrl_data_ptr->plmn_service_state->plmn,
                                 TRUE,
                                 TRUE,
                                 FALSE,
                                 FALSE,
                                 FALSE,
                                 0,
                                 emm_ctrl_data_ptr,
                                 &emm_ctrl_data_ptr->reg_req_pending_ptr->rat_pri_list_info,
                                 emm_ctrl_data_ptr->reg_req_pending_ptr->type
#ifdef FEATURE_LTE_REL9
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->additional_info
#endif         
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->csg_id
#endif 
                                ,SYS_SCAN_SCOPE_FULL_BAND 
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->trm_timeout
#ifdef FEATURE_CIOT
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->scan_info.new_scan
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->scan_info.use_timer
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->scan_info.scan_time//FRACQ,
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->scan_info.nb1_acqdb_timer_status
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->scan_info.nb1_fullband_timer_status
#endif
                                );
      }
    }
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Invalid Service Indication");
  }
} /* end of emm_normal_service_rrc_service_ind() */

/*===========================================================================

FUNCTION   EMM_PLMN_SEARCH_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received in DEREGISTERED PLMN 
  SEARCH state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_plmn_search_rrc_service_ind
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
   

  emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
  reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

} /* end of emm_plmn_search_rrc_service_ind() */

/*===========================================================================

FUNCTION   EMM_LIMITED_SERVICE_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received in DEREGISTERED LIMITED 
  SERVICE state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_limited_service_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type  tai;

  tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  tai.tac = rrc_service_ind->camped_svc_info.tac;

  emm_get_plmn_state_info(SYS_SRV_STATUS_LIMITED, emm_ctrl_data_ptr);

  switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
  {
#ifdef FEATURE_1XSRLTE
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
      if(emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE)            
      {
        emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                               SYS_REG_DOMAIN_PS_SERVICE_ONLY;
      }
      else 
      {
        emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                               SYS_REG_DOMAIN_CAMPED_ONLY;
      }
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                               SYS_SRV_STATUS_LIMITED;
      break;
    case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
      if(emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE)
      {
        emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                               SYS_REG_DOMAIN_PS_SERVICE_ONLY;
      }
      else 
      {
        emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                               SYS_REG_DOMAIN_CAMPED_ONLY;
      }
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                               SYS_SRV_STATUS_LIMITED;
      break;
#endif 
#ifdef FEATURE_LTE_REL9
    case SYS_NETWORK_SELECTION_MODE_LIMITED:
      if((emm_bit_mask_check(emm_ctrl_data_ptr->reg_req_pending_ptr->additional_info,
                                        (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE) &&
         (emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE))
      {
        emm_ctrl_data_ptr->plmn_service_state->extend_srv_info = 
                                         SYS_EXTEND_SRV_INFO_FULL_SRV_POSSIBLE;
      }
      emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info); 
      break;
#endif
    default:
      break;
  }

  reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
  
} /* end of emm_limited_service_rrc_service_ind() */

/*===========================================================================

FUNCTION   EMM_DEREGISTERED_PROCESS_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received in DEREGISTERED state
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_deregistered_process_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  emm_deregistered_substate_type emm_substate ;

  if((emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == TRUE) ||
     (emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == TRUE))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Deacting RRC - Do not proc srv ind further");
    return;
  }

  emm_substate = (emm_deregistered_substate_type)EMM_GET_SUBSTATE() ;

  if((emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain != 
                                                        SYS_SRV_DOMAIN_PS_ONLY)
     &&
     (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain != 
                                                         SYS_SRV_DOMAIN_CS_PS))
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid Service Domain %d",
                   emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain);

    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);

    /* Report no service */
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                         SYS_SRV_STATUS_NO_SRV;
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
  }
  else
  {  
    switch(emm_substate)
    {

      case EMM_DEGEGISTERED_WAITING_PDN_CONN_REQ:
        emm_waiting_pdn_conn_req_rrc_service_ind(rrc_service_ind,
                                                 emm_ctrl_data_ptr);
        break ;
      case EMM_DEREGISTERED_PLMN_SEARCH:
        emm_plmn_search_rrc_service_ind(emm_ctrl_data_ptr);
        break;
      case EMM_DEREGISTERED_ATTACH_NEEDED:
        emm_attach_needed_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr);
        break ;
      case EMM_DEREGISTERED_NO_CELL_AVAILABLE:
        emm_no_service_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      case EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH:
        emm_attempting_to_attach_rrc_service_ind(rrc_service_ind,
                                                 emm_ctrl_data_ptr);
        break ;
      case EMM_DEREGISTERED_NORMAL_SERVICE:
        emm_normal_service_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      case EMM_DEREGISTERED_LIMITED_SERVICE:
      case EMM_DEREGISTERED_NO_IMSI:
        emm_limited_service_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      default:
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal Deregistered substate",0,0,0) ;
        break ;
    }
  }

} /* end of emm_deregistered_process_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_ATTACH_COLLISION_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND recived during ATTACH procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_attach_collision_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type  rrc_reported_plmn ;
  emm_failure_type    emm_failure_cause;

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);

  /*
  ** If reg request pending is set then this means that RLF got triggered and this service request .
  ** was for RLF recovery so send camp indication to let CM know UE camps on LTE.
  */
  if(emm_ctrl_data_ptr->reg_req_pended_indication == TRUE)
  {
    emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
  }
  
  if((sys_plmn_match(rrc_reported_plmn, 
                     emm_ctrl_data_ptr->last_attempted_tai.plmn) != TRUE)
     ||
     (rrc_service_ind->camped_svc_info.tac != 
                                    emm_ctrl_data_ptr->last_attempted_tai.tac))
  {
    if((rrc_service_ind->camped_svc_info.cell_access_status != 
                                            LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

      ||(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING)
#endif
#endif
      )
    {
	  emm_clean_msg_buffer(emm_ctrl_data_ptr);
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TAI changed - Reiniting ATTACH.");
      emm_init_attach_procedure(EMM_INITIAL_UPDATE,emm_ctrl_data_ptr);
    }
    else
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Attach init state, cell access emergency only");
      memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

      mm_stop_timer(TIMER_T3410);
      emm_remove_release_client(emm_attach_release_indication);
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
      emm_clean_msg_buffer(emm_ctrl_data_ptr);

      /* Report to ESM and invalidate pdn conn req*/
      emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);

      /* Notify ESM */
      emm_build_and_send_detach_ind(EMM_DETACHED);
    }
  }

} /* end of emm_attach_collision_rrc_service_ind() */

/*===========================================================================

FUNCTION    EMM_REGISTERED_PROCESS_POSSIBLE_TAU_RRC_SERVICE_IND

DESCRIPTION
  This function checks whether lte_rrc_service_ind_s primitive requires to 
  start TAU in REGISTERED state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If TAU is not required but TAI has been changed this function modifs the 
  following global control vars:
    o last_registered_tai_valid
    o last_registered_tai
    o plmn_service_state
  This function may initiate TAU procedure or change EMM substate to
  EMM_MOVES_TO_REGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE)
===========================================================================*/
void emm_registered_process_possible_tau_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type    current_tai;
  boolean                  ta_in_tai_list = FALSE;
  boolean                  start_tau = FALSE;
  emm_update_attempt_type update_type = EMM_INITIAL_UPDATE;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  sys_csg_info_s_type  csg_info;
#endif 
  boolean 				   is_user_data_pending = FALSE;
#ifdef FEATURE_CIOT
  boolean 				   is_signalling_active_flag = FALSE;
boolean 				   dl_data_expected = FALSE;

#endif


  /* Create NAS current TAI */
  current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(rrc_service_ind->camped_svc_info.selected_plmn) ;
  current_tai.tac = rrc_service_ind->camped_svc_info.tac ;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  csg_info = rrc_service_ind->camped_svc_info.csg_info;
#endif 

#ifdef FEATURE_SGLTE
  if(mm_timer_status[TIMER_DELAY_TAU] == TIMER_ACTIVE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Delay TAU running, start tau on timer expiry");
    return ;
  }
#endif

  /* Send camped ind if necessary */
  if(emm_ctrl_data_ptr->reg_req_pended_indication == TRUE)
  {
    switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
    {
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
        if(
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

           (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) 
           ||
#endif
#endif
           ((((emm_is_plmn_forbidden(current_tai.plmn, emm_ctrl_data_ptr) 
                                                                   == FALSE) &&
            (emm_is_tai_forbidden(current_tai, emm_ctrl_data_ptr) == FALSE)
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
           && (mm_is_csg_in_white_list(current_tai.plmn, &csg_info) == TRUE)
#endif 
           )
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
          || (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
#endif 
          )
         && (rrc_service_ind->camped_svc_info.cell_access_status != 
                                          LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)) 
           )
        {
          emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
        }
        else
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED,
                             FALSE,
                             emm_ctrl_data_ptr);
          return;
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_MANUAL:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
        if(
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

           (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) 
           ||
#endif
#endif
           (emm_is_manual_reg_allowed(current_tai, emm_ctrl_data_ptr) == TRUE))
        {
          emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
          /* User manual PLMN selection. If a TAU is necessary and allowed, UE 
             should the TAU regardless T3402/T3411 status. Stop these timers 
             here if running. */
          mm_stop_timer(TIMER_T3402);
          mm_stop_timer(TIMER_T3411);
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE || defined FEATURE_CATM_LTE_EXT_GNSS_COEX || defined FEATURE_WWAN_GNSS_DYNAMIC
          mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

        }
        else
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED,
                             FALSE,
                             emm_ctrl_data_ptr);
          return;
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_LIMITED:
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

        if(emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)
#endif
#endif
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                          SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED,
                             FALSE,
                             emm_ctrl_data_ptr);
          return;
        }
#ifdef FEATURE_LTE_REL9
        break;
#endif
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
        case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
          if((emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)            
             && (rrc_service_ind->camped_svc_info.cell_access_status != 
                                           LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY))
          {
            emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
          }
          else
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
            return;
          }
          break;
#endif
      default:
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW Sel Mode in REG REQ - %d",
                       emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
        return;
    }
  }
  /* 
  ** If UE camps on a different PLMN from the manually requested PLMN in limited routine
  ** manual then return limited service to REG/CM.
  */
  else if((emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == 
                          SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION) &&
          (emm_is_manual_reg_allowed(current_tai, emm_ctrl_data_ptr) == FALSE))
  {
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                          SYS_SRV_STATUS_LIMITED;
#if defined(FEATURE_LTE_TO_1X) || defined(FEATURE_NAS_GW)
#ifndef FEATURE_NAS_DISABLE_VOICE
    if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) || 
       (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED))
    {
       emm_process_buffered_csfb_hard_failure();
    }
#endif
#endif
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED,
                       FALSE,
                       emm_ctrl_data_ptr);
    return;
  }
  else if(
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

          (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
          &&
#endif
#endif
          ((((emm_is_plmn_forbidden(current_tai.plmn, emm_ctrl_data_ptr) == TRUE)
           ||(emm_is_tai_forbidden(current_tai, emm_ctrl_data_ptr) == TRUE) 
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
           || (mm_is_csg_in_white_list(current_tai.plmn, &csg_info) == FALSE)            
#endif
           ) &&
          ((emm_ctrl_data_ptr->manual_user_sel == FALSE) ||
           (emm_is_manual_reg_allowed(current_tai, emm_ctrl_data_ptr) == FALSE)
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
           || (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == FALSE)
#endif 
         ))))
  {
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                          SYS_SRV_STATUS_LIMITED;
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
#ifndef FEATURE_NAS_DISABLE_VOICE
    if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) || 
       (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED))
    {
       emm_process_buffered_csfb_hard_failure();
    }
#endif
#endif
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED,
                       FALSE,
                       emm_ctrl_data_ptr);
    return;
  }
  else if (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

      && emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY
#endif
#endif
     )
  {
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                       SYS_SRV_STATUS_LIMITED;
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
    return;
  }
#ifdef FEATURE_SGLTE
  if ((mm_process_if_both_stack_has_camped_non_sglte_plmn(mm_get_ps_stack_id())== TRUE)||
     ((mm_is_sglteplmn_in_sglte_roam(current_tai.plmn ))==TRUE))
  {
    MSG_HIGH_DS_0(MM_SUB, "EMM: Sent Limited regional As UE MODE change pending ");
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
    
    reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,
                       FALSE,
                       emm_ctrl_data_ptr);
    return ;
  }
  else if ( mm_process_sglte_plmn_state4_gsm_oos(mm_serving_plmn.info.plmn)==TRUE )
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM=LTE got SGLTE PLMN in State4 While GSM OOS ");
  }
#endif

  ta_in_tai_list = emm_search_tai_list(&current_tai,
                                       emm_ctrl_data_ptr->emm_tai_list_ptr);

  /* UE comes out of no service. Set service status. */
  if(emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                         SYS_SRV_STATUS_NO_SRV)
  {
    if((ta_in_tai_list == TRUE) && 
       (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED)
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

       &&
       (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)
#endif
#endif 
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
       && (mm_is_csg_in_white_list(current_tai.plmn, &csg_info) == TRUE)
#endif 
      )
    {
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                            SYS_SRV_STATUS_SRV;
    }
    else
    {
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
    }
  }

#ifdef FEATURE_LTE_REL10
    if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) 
    {
      /* if there is not active bearer & there is no PS detach pending then start Attach procedure */ 
      if(((emm_convert_eps_bc_status_to_bit_mask(
              emm_ctrl_data_ptr->eps_bearer_context_status) == 0) &&
         (emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_NONE)
#ifdef FEATURE_CIOT
			   && (emm_is_attach_without_pdn_supported()== FALSE)
#endif
       )
         ||
        (emm_chk_attach_with_imsi(emm_ctrl_data_ptr
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

                                  ,FALSE
#endif
#endif
                                 ) == TRUE))
      {
        /* for Tau rej#40 we move it to DE-REG NORMAL service*/
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
        MSG_HIGH_DS_0(MM_SUB, "=EMM= No active bearers or Attach with IMSI configured, start attach procedure");
        /* Start initial type attach procedure */
        emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr); 
        return;
      }
    }
#endif

  /* Check whether current RRC reported TAI belongs to the current TAI list */
  if(ta_in_tai_list == TRUE)
  {
    /* Current TAI IS part of the current TAI list */
    MSG_HIGH_DS_3(MM_SUB, "=EMM= TA in TAI list - US %d, T3402 %d, T3411 %d",
                  emm_ctrl_data_ptr->emm_update_status,
                  mm_timer_status[TIMER_T3402],
                  mm_timer_status[TIMER_T3411]);

    if((emm_ctrl_data_ptr->reg_complete_ack_pending != EMM_PENDING_REG_ACK_NONE) &&
       (
#ifdef FEATURE_NAS_GW
        (emm_ctrl_data_ptr->ps_resumption_status == EMM_PS_RESUMPTION_PENDING) ||
        (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_DUE_TO_SRVCC) ||
#endif
        (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC)))
    {
      /* Do not start TAU here. Let ATTACH/TAU COMPLETE NAK to trigger reattach
         or TA change TAU. */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= ATTACH/TAU COMPLETE ACK pending, restart proc on NAK");
      return;
    }

#ifdef FEATURE_CIOT
	ds3gpp_is_user_data_or_signalling_pending(&is_user_data_pending,&is_signalling_active_flag,&dl_data_expected);
	is_signalling_active_flag = FALSE;    
	dl_data_expected = FALSE;
#else
    is_user_data_pending = ds3gpp_per_subs_is_ps_data_available((sys_modem_as_id_e_type)mm_sub_id);
#endif


    if(emm_ctrl_data_ptr->emm_update_status != EMM_STATUS_UPDATED)
    {
#ifdef FEATURE_1XSRLTE
      if(emm_ctrl_data_ptr->srlte_tau_needed == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU delayed in 1X SRLTE mode until call end on other stack");
      }
      else
#endif
      if((mm_timer_status[TIMER_T3402] == TIMER_ACTIVE) ||
         (mm_timer_status[TIMER_T3411] == TIMER_ACTIVE)
#ifdef FEATURE_LTE_REL14
         || ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL14) &&
         (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE))
#endif
        )
      {
        if(emm_search_tai_list(&emm_ctrl_data_ptr->last_attempted_tai,
                               emm_ctrl_data_ptr->emm_tai_list_ptr) == TRUE)
        {
          /* T3402/T3411 was started on a TA which belongs to TA list. The
             current serving TA is also in the TA list. Do not start TAU
             until T3402/T3411 expiry. */
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE);
        }
        else
        {
          /* T3402/T3411 was started on a TA which does not belong to TA list.
             The current serving TA is different since it belongs to the TA list.
             Start TAU right away. */
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - Srving TA in TAL w/ T3402/T3411 running");
          emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;
          start_tau = TRUE;
        }
      }
      else
      {  
        /* EU was updated to not updated by GW for RAU failure. Start TAU. */
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Initing TAU - EU STATUS %d",
                      emm_ctrl_data_ptr->emm_update_status);
        
        if((emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_INVALID)||
           (emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_PERIODIC))
        {
            emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
        }
        start_tau = TRUE;
      }      
    }
#ifdef FEATURE_NAS_GW
    else if((emm_ctrl_data_ptr->ps_resumption_status == EMM_PS_RESUMPTION_PENDING)&&
            (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE) &&
            (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE))
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - GW PS session suspended");
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      start_tau = TRUE;
    }
    else if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI)
    {
      MSG_HIGH_DS_2(MM_SUB, "=EMM= Initing TAU - T3423 timer status %d, TIN type %d",
                    mm_timer_status[TIMER_T3423],
                    emm_ctrl_data_ptr->TIN);
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      start_tau = TRUE;
    }
#if defined(FEATURE_FEMTO_CSG)
    else if ((mm_is_csg_in_white_list(current_tai.plmn, &csg_info) == FALSE) &&
            (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE) &&
            (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE) &&
            (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE))
    {
      MSG_HIGH_DS_2(MM_SUB, "=EMM= Initing TAU - Manual CSG SELECTION success",
                    mm_timer_status[TIMER_T3423],
                    emm_ctrl_data_ptr->TIN);
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      start_tau = TRUE;
    }
#endif 
#endif
    else if(((emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC)
#ifdef FEATURE_NAS_GW
              ||            
             (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_DUE_TO_SRVCC)
#endif
             ) &&
             (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE) &&
             (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE))
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= Initing TAU - SGs association to be established cause %d",
                    emm_ctrl_data_ptr->sg_association_to_be_established);
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      start_tau = TRUE;
    }
#ifdef DISABLE_9x05_OPTIMIZATIONS
    else if( (*emm_ctrl_data_ptr->set_drx_req_pending_ptr == TRUE)&&
        (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE) &&
        (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE)) 
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= SET_DRX_REQ pending, Start TAU procedure ");
      start_tau = TRUE;
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
    }
#endif
    else if((emm_ctrl_data_ptr->radio_cap_update != EMM_RADIO_CAP_UPDATE_NONE) &&
            (emm_ctrl_data_ptr->tau_cause != TAU_CAUSE_LOAD_BALANCING) &&
            (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE) &&
            (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE))
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Initiate TAU - Update radio capabilities pending");
      start_tau = TRUE;
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;
    }
#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED) || !defined(FEATURE_NAS_DISABLE_VOICE)
    else if((emm_lte_cs_domain_param_match(
                      emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param,
                      emm_ctrl_data_ptr->reg_lte_cs_domain_param) == FALSE) &&
            (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE) &&
            (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE))
    {
      
        MSG_HIGH_DS_3(MM_SUB, "=EMM= Dbg REG Valid = %d voice_domain_pref = %d voice_or_data_centric = %d",
                       emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.is_voice_domain_ie_valid,
                       emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.voice_domain_pref, 
                       emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.voice_or_data_centric);

        MSG_HIGH_DS_3(MM_SUB, "=EMM= Dbg EMM Valid = %d voice_domain_pref = %d voice_or_data_centric = %d",
                       emm_ctrl_data_ptr->reg_lte_cs_domain_param.is_voice_domain_ie_valid,
                       emm_ctrl_data_ptr->reg_lte_cs_domain_param.voice_domain_pref, 
                       emm_ctrl_data_ptr->reg_lte_cs_domain_param.voice_or_data_centric);

       MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - change in lte_cs_domain_param");
       emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
       start_tau = TRUE;
    }
#endif
    else if(((*emm_ctrl_data_ptr->psm_set_cfg_status_ptr == NAS_PSM_CFG_REQ_PENDING)
             ||
             (emm_ctrl_data_ptr->edrx_set_cfg_req_status == NAS_EDRX_CFG_REQ_PENDING)) &&
            (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE) &&
            (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE)
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

         && (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
#endif
     )
    {
      MSG_HIGH_DS_2(MM_SUB, "=EMM= PSM(%d)/EDRX(%d) Set cfg params req pending, Start TAU procedure ",
                           *emm_ctrl_data_ptr->psm_set_cfg_status_ptr,
                           emm_ctrl_data_ptr->edrx_set_cfg_req_status);
      start_tau = TRUE;
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
    }
    else if(EMM_GET_SUBSTATE() == EMM_REGISTERED_UPDATE_NEEDED)
    {  
      if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_LOW_LAYER_FAILURE)
      {  
        if(is_user_data_pending == FALSE)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - Low layer failure and no data pending");
          start_tau = TRUE;
        }
        else if(emm_ctrl_data_ptr->eps_bearer_context_status_updated == TRUE)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - BC updated in REGED_NO_CELL_AVAILABLE");
          emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
          start_tau = TRUE;
        }
        else
        {
          emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
        }
      }
	  else if(emm_ctrl_data_ptr->is_periodic_tau_required)
	  {
	    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_PERIODIC;
	    MSG_HIGH_DS_1(MM_SUB, "=EMM= Initing TAU %d", emm_ctrl_data_ptr->tau_cause);
        start_tau = TRUE;

	  }
      else if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_TA_CHANGE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= TA change to orig cell");
        emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
        emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;
      }
      else if(emm_ctrl_data_ptr->tau_cause != TAU_CAUSE_INVALID)
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Initing TAU %d", emm_ctrl_data_ptr->tau_cause);
        start_tau = TRUE;
      }
    }
    else if((EMM_GET_STATE() == EMM_REGISTERED) &&
               ((EMM_GET_SUBSTATE() == EMM_REGISTERED_NO_CELL_AVAILABLE) ||
               (EMM_GET_SUBSTATE() == EMM_REGISTERED_PLMN_SEARCH) ||
               (EMM_GET_SUBSTATE() == EMM_REGISTERED_NORMAL_SERVICE)))
    {  
      /* 5.5.3.2.2 f - Start TAU when EPS bearer context is updated while in 
         NO_CELL_AVAILABLE substate and UE returns to NORMAL_SERVICE substate  */
      if(emm_ctrl_data_ptr->eps_bearer_context_status_updated == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - BC updated in REGED_NO_CELL_AVAILABLE");
        emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
        start_tau = TRUE;
      }
      else if((emm_ctrl_data_ptr->tau_cause != TAU_CAUSE_INVALID) &&
              (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE) &&
              (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE))
      {  
        if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_LOW_LAYER_FAILURE)
        {  
          if(is_user_data_pending == FALSE)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - Delayed lower layer failure");
            start_tau = TRUE;
          }
          else
          {
            emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
          }
        }
        else if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_TA_CHANGE)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= TA change to orig cell");
          emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
          emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;
        }
        else
        {
          MSG_HIGH_DS_2(MM_SUB, "=EMM= Initing TAU %d - REGISTERED %d substate",
                        emm_ctrl_data_ptr->tau_cause,
                        EMM_GET_SUBSTATE());
          start_tau = TRUE;
        }
		if((start_tau == TRUE) && 
		   (emm_ctrl_data_ptr->emm_tau_attempt_counter != 0) &&
		   (emm_ctrl_data_ptr->emm_tau_attempt_counter < MAX_NUMBER_TAU_OF_ATTEMPTS ))
        {
          update_type = EMM_UPDATE_RECOVERY;
        }
      }
    }
  }
  else
  {
    /* Current TAI is NOT part of the current TAI list */
    MSG_HIGH_DS_3(MM_SUB, "=EMM= TA not in TAI list - US %d, T3402 %d, T3411 %d",
                  emm_ctrl_data_ptr->emm_update_status,
                  mm_timer_status[TIMER_T3402],
                  mm_timer_status[TIMER_T3411]);
 
    if(emm_ctrl_data_ptr->reg_complete_ack_pending != EMM_PENDING_REG_ACK_NONE)
    {
      /* Do not start TAU here. Let ATTACH/TAU COMPLETE NAK to trigger reattach
         or TA change TAU. */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= ATTACH/TAU COMPLETE ACK pending, restart proc on NAK");
      return;
    }

    if((emm_ctrl_data_ptr->emm_update_status != EMM_STATUS_UPDATED)
       &&
       ((mm_timer_status[TIMER_T3402] == TIMER_ACTIVE) ||
        (mm_timer_status[TIMER_T3411] == TIMER_ACTIVE)
#ifdef FEATURE_LTE_REL14
         || ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL14) &&
            (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE))
#endif
        )
       &&
       (sys_plmn_match(emm_ctrl_data_ptr->last_attempted_tai.plmn, 
                       current_tai.plmn) == TRUE) 
       &&
       (emm_ctrl_data_ptr->last_attempted_tai.tac == current_tai.tac))
    {
      /* EU is not updated. Current serving TA is the same TA on which T3402 was
         started. Do not start TAU untill T3402 expiry. */
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE);
    }
    else
    {
      /* For all other cases, serving TA is not in TA list, start TAU. */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - Srving TA not in TAL");
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;
      if((sys_plmn_match(emm_ctrl_data_ptr->last_attempted_tai.plmn, 
                       current_tai.plmn) == TRUE)&&
         (emm_ctrl_data_ptr->last_attempted_tai.tac == current_tai.tac)&&
         (emm_ctrl_data_ptr->emm_tau_attempt_counter != 0))
      {
        update_type = EMM_UPDATE_RECOVERY;
      }
      start_tau = TRUE;
    }

    /* Is this a good way to determine if timer is running? Can there be a race condition? when just before doing this 
    what happens if timer expires*/
    if(mm_timer_status[ TIMER_T3423 ] == TIMER_ACTIVE )
    {
      mm_stop_timer(TIMER_T3423);
    }
  }

#ifdef FEATURE_ENHANCED_NW_SELECTION
  if((start_tau != TRUE) && 
     (EMM_GET_SUBSTATE() != EMM_REGISTERED_ATTEMPTING_TO_UPDATE) &&
     (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE) &&
     (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE) &&
     (emm_ctrl_data_ptr->reg_req_pending_ptr->type == MMR_REG_REQ_EF_RAT_REFRESH) &&
     (emm_ctrl_data_ptr->reg_req_pended_indication == TRUE) &&
     (emm_ctrl_data_ptr->refresh_pending == TRUE) && 
     (reg_sim_read_ens_flag() == TRUE))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - EF_RAT_REFRESH");
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
    start_tau = TRUE;
  }
  MSG_HIGH_DS_0(MM_SUB, "=EMM= reset refresh_pending flag");
  emm_ctrl_data_ptr->refresh_pending = FALSE;
#endif 

  if(start_tau == TRUE)
  {
    if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_PERIODIC)
    {
      emm_initiate_tau_procedure(update_type,
                                 PERIODIC_UPDATING,
                                 FALSE, 
                                 emm_ctrl_data_ptr);
    }
    else
    {
      emm_initiate_tau_procedure(update_type,
                                 emm_get_eps_update_type(emm_ctrl_data_ptr),
                                 FALSE, 
                                 emm_ctrl_data_ptr);
    }
  }
  else
  {  
    if(EMM_GET_SUBSTATE() != EMM_REGISTERED_ATTEMPTING_TO_UPDATE)
    {
      if(sys_eplmn_list_validate(TRUE) == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= EPLMN list validated - TAU not needed");
        emm_send_rrc_system_update(emm_ctrl_data_ptr);
#ifdef FEATURE_NAS_GW
        /* Update G RR and W RRC */
        mm_send_rrc_eq_plmn_list_change_ind();
        mm_send_rr_eq_plmn_list_change_ind(); 
#endif
      }

      /* EMM moves into NORMAL SERVICE substate */
      emm_enforce_normal_service(&current_tai, emm_ctrl_data_ptr);
#ifdef FEATURE_NAS_GW
	  /* Set manual flag false for the case same registered TAI for Manual*/
	  emm_set_mm_manual_selection_flag(FALSE);
#endif
      emm_ctrl_data_ptr->manual_user_sel = FALSE;
	  
	  /* Reset the Attach failed Ind type for UPDATED TAC case */
      if ((emm_get_mmr_attach_failed_ind_status() ==  ATTACH_FAILED_IND_REQUIRED ) &&
           (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE) &&
           (mm_timer_status[TIMER_T3430] != TIMER_ACTIVE) &&
           (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE) &&
          (emm_ctrl_data_ptr->plmn_service_state->service_status ==
                                                        SYS_SRV_STATUS_SRV) &&
          ((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                       SYS_SRV_DOMAIN_CS_PS) ||
           (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                       SYS_SRV_DOMAIN_PS_ONLY)))
      {
        emm_reset_attach_ind_status_type();
      }
 

      if(((mm_timer_status[TIMER_T3411] == TIMER_ACTIVE) &&
          (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == 
                                                       SYS_SRV_DOMAIN_CS_PS) &&
          (emm_is_combined_reg_allowed(emm_ctrl_data_ptr) == TRUE) &&
          (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_AVAILABLE) &&
          (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                       SYS_SRV_DOMAIN_PS_ONLY))
         ||
         (mm_timer_status[TIMER_T3402] == TIMER_ACTIVE))
      {
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM);
      }
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
#ifndef FEATURE_NAS_DISABLE_VOICE
      if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED)
      {
         if(emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS)
         {
            emm_send_esr_if_buffered(emm_ctrl_data_ptr);
         }
         else if((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain != SYS_SRV_DOMAIN_CS_PS))
         {
            emm_process_buffered_csfb_hard_failure();
         }
      }
      else if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED)
      {
         emm_send_esr_if_buffered(emm_ctrl_data_ptr);
      }
#endif
#endif
    }
    else if(mm_timer_status[TIMER_T3402] == TIMER_ACTIVE )
    {
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
      reg_service_report(MMR_CAUSE_FAILURE_MAX_ATTEMPTED,
                         FALSE,
                         emm_ctrl_data_ptr);

    }
  }

} /* end of emm_registered_process_possible_tau_rrc_service_ind() */

/*===========================================================================

FUNCTION   EMM_REGISTERED_LIMITED_PROCESS_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received in REGISTERED LIMITED 
  SERVICE state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_registered_limited_process_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type  tai;



  tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  tai.tac = rrc_service_ind->camped_svc_info.tac;

  emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;

  switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
  {
#ifdef FEATURE_1XSRLTE
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
      if(emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE)            
      {
        emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                               SYS_REG_DOMAIN_PS_SERVICE_ONLY;
      }
      else 
      {
        emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                               SYS_REG_DOMAIN_CAMPED_ONLY;
      }
      break;
    case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
      if(emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE)
      {
        emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                               SYS_REG_DOMAIN_PS_SERVICE_ONLY;
      }
      else 
      {
        emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                               SYS_REG_DOMAIN_CAMPED_ONLY;
      }
      break;
#endif 
#ifdef FEATURE_LTE_REL9
    case SYS_NETWORK_SELECTION_MODE_LIMITED:
      if((emm_bit_mask_check(emm_ctrl_data_ptr->reg_req_pending_ptr->additional_info,
                                       (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE) &&
         (emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE))
      {
        emm_ctrl_data_ptr->plmn_service_state->extend_srv_info = 
                                         SYS_EXTEND_SRV_INFO_FULL_SRV_POSSIBLE;
      }
      emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);             
      break;
#endif
    default:
      break;
  }

  reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

} /* emm_registered_limited_process_rrc_service_ind */

/*===========================================================================

FUNCTION    EMM_REGISTERED_PROCESS_RRC_SERVICE_IND

DESCRIPTION
  This function checks whether lte_rrc_service_ind_s primitive requires to 
  start TAU when EMM in REGISTERED state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May stop timers:
    o
===========================================================================*/
void emm_registered_process_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{

  switch (EMM_GET_SUBSTATE())
  {
    case EMM_REGISTERED_UPDATE_NEEDED:
    case EMM_REGISTERED_PLMN_SEARCH:
    case EMM_REGISTERED_NO_CELL_AVAILABLE:
    case EMM_REGISTERED_NORMAL_SERVICE:
    case EMM_REGISTERED_ATTEMPTING_TO_UPDATE:
    case EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM:
    case EMM_REGISTERED_IMSI_DETACH_INITIATED:
      /* Check whether TAU is no longer required, if it is start TAU procedure */
      emm_registered_process_possible_tau_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
      break ; 
    case EMM_REGISTERED_LIMITED_SERVICE:
      emm_registered_limited_process_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr);
      break ;
    case EMM_REGISTERED_WAITING_FOR_ESM_ISR_STATUS:
#ifdef FEATURE_CIOT
#ifndef FEATURE_CATM1_DISABLE
    case EMM_REGISTERED_WAITING_FOR_ESM_CIOT_RAT_CHANGE:
#endif
#endif
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Rcved SRV IND in WAITING_FOR_ESM_ISR_STATUS state");
      /* Do nothing for now */
      break;
    default:
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM REGISTERED substate",0,0,0) ;
      break ;
  }
} /* end of emm_registered_process_rrc_service_ind() */

/*===========================================================================

FUNCTION    EMM_TAU_COLLISION_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received during TAU procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_tau_collision_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type  current_tai;
  boolean                start_tau = FALSE;
  emm_failure_type               emm_failure_cause;

   
   

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  current_tai = emm_get_current_tai(rrc_service_ind);
 /*
  ** If reg request pending is set then this means that RLF got triggered and this service request .
  ** was for RLF recovery so send camp indication to let CM know UE camps on LTE.
  */
  if(emm_ctrl_data_ptr->reg_req_pended_indication == TRUE)
  {
    emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
  }
  
  if(emm_search_tai_list(&current_tai, emm_ctrl_data_ptr->emm_tai_list_ptr)
       == TRUE)
  {
    if(emm_search_tai_list(&emm_ctrl_data_ptr->last_attempted_tai,
                           emm_ctrl_data_ptr->emm_tai_list_ptr) == FALSE)
    {
      /* Current TA is in TA list. The TA that TAU was started on is not in TA
         list. TAU need to be restarted. */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TA (In TAL) changed from TAU started on");
      start_tau = TRUE;
    }
  }
  else
  {
    if((sys_plmn_match(emm_ctrl_data_ptr->last_attempted_tai.plmn, 
                       current_tai.plmn) == FALSE) 
       ||
       (emm_ctrl_data_ptr->last_attempted_tai.tac != current_tai.tac))
    {
      /* Current TA is not in TA list and is different frm the TA that TAU was
         started on. TAU need to be restarted. */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TA (Not in TAL) changed from TAU started on");
     start_tau = TRUE;
    }
  }
  
  if(rrc_service_ind->camped_svc_info.cell_access_status == 
                                       LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)
  {
    mm_stop_timer(TIMER_T3430);
    emm_remove_release_client(emm_tau_request_release_indication);
    EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_PLMN_SEARCH);

    emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
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
    if(start_tau == TRUE)
    {       
      start_tau = FALSE;
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED ;
      emm_save_mobility_info(emm_ctrl_data_ptr);
    }

    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
  }

  if(start_tau == TRUE)
  {
    /* Delete pended TAU message if any before starting TAU to avoid processing 
        any buffered TAU Request if it was not ACKed */
    emm_delete_pended_message_with_msg_id(TRACKING_AREA_UPADTE_REQUEST,emm_ctrl_data_ptr);
	
	
    /* TAU is being restarted due to TA change. Set EPS update status to EU2 
       NOT UPDATED. (24.301 5.5.3.2.6 e) */
    emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED ;
    emm_save_mobility_info(emm_ctrl_data_ptr);
    emm_cancel_authentication();

    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;

    emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                               emm_get_eps_update_type(emm_ctrl_data_ptr),
                               TRUE, 
                               emm_ctrl_data_ptr);
  }
} /* end of emm_tau_collision_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_SERVICE_REQUEST_COLLISION_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received during SERVICE REQUEST
  procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_service_request_collision_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type current_tai ;
   

  /* Create NAS current TAI */
  current_tai = emm_get_current_tai(rrc_service_ind);

  /*
    ** If reg request pending is set then this means that RLF got triggered and this service request .
    ** was for RLF recovery so send camp indication to let CM know UE camps on LTE.
  */
  if(emm_ctrl_data_ptr->reg_req_pended_indication == TRUE)
  {
    emm_send_reg_camped_ind(rrc_service_ind->camped_svc_info);
  }

  /* 
    Check whether current RRC reported TAI is belonging to the 
    current TAI list
  */
  if (emm_search_tai_list(&current_tai,emm_ctrl_data_ptr->emm_tai_list_ptr))
  {
    /* Current TAI IS part of the current TAI list */

    MSG_HIGH_DS_0(MM_SUB, "=EMM= TAI is part of the existing TAI list, no TAU is required");

    MSG_HIGH_DS_0(MM_SUB, "=EMM= Keep running Service Request procedure");
  }
  else
  {
    /* Current TAI is NOT part of the current TAI list */

    /* Start TAU procedure */
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;
    emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                               emm_get_eps_update_type(emm_ctrl_data_ptr),
                               FALSE, 
                               emm_ctrl_data_ptr);
  }
} /* end of emm_service_request_collision_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_DETACH_COLLISION_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received during DETACH procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_detach_collision_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type current_tai ;
  
  /* Create NAS current TAI */
  current_tai = emm_get_current_tai(rrc_service_ind);

  if(emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY ||
     emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS)
  {
    /* 
      Check whether current RRC reported TAI is belonging to the 
      current TAI list
    */
    if (emm_search_tai_list(&current_tai,emm_ctrl_data_ptr->emm_tai_list_ptr))
    {
      /* Current TAI IS part of the current TAI list */

      MSG_HIGH_DS_0(MM_SUB, "=EMM= TAI is part of the existing TAI list, no TAU is required, Keep running DETACH Procedure");

      //emm_misc_actions_on_service_indication(rrc_service_ind,emm_ctrl_data_ptr);
    }
    else
    {

      /* EMM PS detach procedure will be aborted on TAI Change
         in case PS Detach is triggered due to DDS switch completion
      */
#ifdef FEATURE_LTE_REL13
      if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL13) &&
         (emm_ctrl_data_ptr->detach_reason == SWITCH_OFF))
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Perform local switch off detach");
        emm_complete_mo_detach(emm_ctrl_data_ptr);    
      }
      else
#endif

#ifdef FEATURE_DUAL_SIM
      if (*emm_ctrl_data_ptr->dds_switch_pended_ptr)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Perform local detach due to high priority user action");
        emm_complete_mo_detach(emm_ctrl_data_ptr);    
      }
      else
#endif
      {
        /* Current TAI is NOT part of the current TAI list */
        /* Start TAU procedure */
        emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;
        emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                                   emm_get_eps_update_type(emm_ctrl_data_ptr),
                                   FALSE, 
                                   emm_ctrl_data_ptr);
      }
    }
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= not starting tau. service domain is %d",
                  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain);
  }
} /* end of emm_detach_collision_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_PROCESS_DIV_DUPLEX_CHG

DESCRIPTION
  This function performs local detach and reattach (if UE is camped) upon 
  division duplex change

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_div_duplex_chg
(
  emm_ctrl_data_type     *emm_ctrl_data_ptr,
  lte_rrc_service_ind_s  *rrc_srv_ind_ptr
)
{
  emm_state_type  emm_state = EMM_INVALID_STATE;
  emm_state_type  emm_substate = EMM_INVALID_STATE;
  emm_failure_type        emm_failure_cause;

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  emm_state = EMM_GET_STATE();
  emm_substate = EMM_GET_SUBSTATE();

  MSG_HIGH_DS_3(MM_SUB, "=EMM= DIV DUPLEX change - RRC active %d, Srv status %d, State %d",
                emm_ctrl_data_ptr->rrc_active,
                emm_ctrl_data_ptr->plmn_service_state->service_status,
                emm_state);

  if(rrc_srv_ind_ptr->svc_status != LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Unexpected div duplex chg while UE is not camped");
    return;
  }

  /* Update service status if REG REQ is pending */
  if((emm_ctrl_data_ptr->reg_req_pended_indication == TRUE) &&
     (emm_ctrl_data_ptr->reg_init_rrc_trans_id == rrc_srv_ind_ptr->trans_id))
  {
    emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;
    emm_send_reg_camped_ind(rrc_srv_ind_ptr->camped_svc_info);
  }

  /* Abort ongoing spedific procedures and stop related timers */
  switch(emm_state)
  {
    case EMM_REGISTERED_INITIATED:
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      mm_stop_timer(TIMER_T3410);
      emm_remove_release_client(emm_attach_release_indication);
      if (emm_substate == (emm_state_type)EMM_WAITING_FOR_NW_RESPONSE)
      {  
        emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
      }
      emm_clean_msg_buffer(emm_ctrl_data_ptr);
      break;  
    case EMM_REGISTERED:
      mm_stop_timer(TIMER_T3411);
      mm_stop_timer(TIMER_T3402);
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE || defined FEATURE_CATM_LTE_EXT_GNSS_COEX || defined FEATURE_WWAN_GNSS_DYNAMIC
      mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

      break;
    case EMM_SERVICE_REQUEST_INITIATED:
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      if(emm_state == EMM_SERVICE_REQUEST_INITIATED)
      {
        /* 5.6.1.6 Abnormal case g in UE. Abort service request and start detach
           Keep the connection and notify ESM if necessary */
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  TRUE, LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                  emm_failure_cause);
      }
      else
      {
        mm_stop_timer(TIMER_T3430);
        emm_remove_release_client(emm_tau_request_release_indication);
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
      if(emm_ctrl_data_ptr->detach_reason == SWITCH_OFF)
      {
        /* Let switch off detach proceed. Do nothing here. */
        return;
      }
      else
      {
        /* Abort non switch off detach and do a local detach */
        emm_abort_mo_detach(emm_ctrl_data_ptr, TRUE);
        emm_clean_msg_buffer(emm_ctrl_data_ptr);
      }
      break;
    default:
      return;
  }
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
#ifndef FEATURE_NAS_DISABLE_VOICE
  if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) || 
     (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED))
  {
     emm_process_buffered_csfb_hard_failure();
  }
#endif
#endif
  /* Local detach */
  emm_build_and_send_detach_ind(EMM_DETACHED);

  emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_NO_SRV;
  emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;


  if(emm_state == EMM_DEREGISTERED_INITIATED)
  {
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);    
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr); 
  }
  else
  {
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED);
    if((reg_service_report !=(emm_reg_service_report_type)emm_send_reg_cnf) &&
       (emm_ctrl_data_ptr->is_plmn_block_req_pending == FALSE))
    {
      reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN, FALSE, emm_ctrl_data_ptr); 
    }
    if(emm_ctrl_data_ptr->is_plmn_block_req_pending == TRUE)
    {
	  emm_ctrl_data_ptr->reg_complete_ack_pending =	EMM_PENDING_REG_ACK_NONE;
      emm_process_block_plmn_req(&emm_ctrl_data_ptr->pending_block_plmn_req, emm_ctrl_data_ptr);
    }
    else if(emm_ctrl_data_ptr->reg_req_pended_indication == FALSE)
    { 
      emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
      emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
    }  
  }

} /* emm_process_div_duplex_chgdiv_duplex_chg */

#ifdef FEATURE_LTE_REL10
/*===========================================================================
FUNCTION    EMM_CHK_BACKOFF_CONGESTION_ALLOWED

DESCRIPTION
  checks if registraion is allowed on plmn due to congestion while t3346
  is running. puts emm in appropriate state due to new serv ind

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if reg is allowed on plmn
  FALSE - if reg is not allowed on plmn

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_chk_backoff_congestion_allowed
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  emm_state_type emm_state   = EMM_INVALID_STATE;
  boolean        reg_allowed = TRUE;
  emm_state = EMM_GET_STATE();

#ifdef FEATURE_LTE_REL11 
   if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
      (emm_state == EMM_REGISTERED) &&
      (EMM_GET_SUBSTATE() == EMM_REGISTERED_NORMAL_SERVICE)&&
      (EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE))
   {
     MSG_HIGH_DS_0(MM_SUB, "=EMM= In Registered-Normal service and Connected state");
     return reg_allowed;
   }
  /* If UE is higher access allowed UE or if the an emergency 
  ** PDN exists then TAU is allowed(T3346 should not be considered)
  */
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
     ((emm_check_high_priority(emm_ctrl_data_ptr) == TRUE)
#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

       ||
      (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_EXIST)
#endif
#endif
      ))
  {
    return reg_allowed;
  }
#endif

  /* check T3346 and eplmn list for plmn registration */
  if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
     (emm_plmn_equivalent_t3346(rrc_service_ind,
                                emm_ctrl_data_ptr) == TRUE)
#ifdef FEATURE_CIOT

  &&((emm_ctrl_data_ptr->emm_start_mo_excp_signalling == FALSE) ||
     (emm_ctrl_data_ptr->is_t3346_started_for_mo_excp == TRUE))
#endif
                                )
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= registration not allowed on plmn T3346 %d",
                  mm_timer_status[TIMER_T3346]);

    /* need to do registration upon t3346 timer expiry */
    if(emm_chk_ta_change(rrc_service_ind, emm_ctrl_data_ptr) == TRUE)    
    {
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
      emm_save_mobility_info(emm_ctrl_data_ptr);
    }

    /* move to approptiate based update status */
    switch(emm_state)
    {
      case EMM_DEREGISTERED:
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH);
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                             SYS_SRV_STATUS_LIMITED_REGIONAL;
        break;

     case EMM_REGISTERED:
        if(emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED)
        {
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                          SYS_SRV_STATUS_SRV;
        } 
        else
        {
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE);
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                             SYS_SRV_STATUS_LIMITED_REGIONAL;
        }
        break;

      default:
         MSG_ERROR_DS_1(MM_SUB, "=EMM= rrc_service_ind unhandled state %d", emm_state);
         break;
    }  

    reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN, FALSE, emm_ctrl_data_ptr);
#ifdef FEATURE_NAS_GW
    if(emm_ctrl_data_ptr->reg_init_rrc_trans_id == rrc_service_ind->trans_id)
    {
      /* Post RAT change event */
      mmsend_rat_change_event();
    }
#endif
    reg_allowed = FALSE;
  }
  
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
#ifndef FEATURE_NAS_DISABLE_VOICE
  if(!reg_allowed &&
     ((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) || 
      (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED)))
  {
     emm_process_buffered_csfb_hard_failure();
   }
#endif
#endif
  return reg_allowed;
}

#endif /* end FEATURE_LTE_REL10*/

/*===========================================================================
FUNCTION    EMM_RRC_PROCESS_SERVICE_INDICATION

DESCRIPTION
  This function processes RRC_SERVICE_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_rrc_process_service_indication
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  emm_state_type        emm_state       = EMM_INVALID_STATE ;
  sys_plmn_id_s_type    rrc_reported_plmn;
  sys_plmn_id_s_type    last_saved_plmn;
  lte_rrc_plmn_s        plmn;
  boolean               is_plmn_match = FALSE;
  lte_nas_local_cause_enum_T    local_cause = AS_REJ_LRRC_LOWER_LAYER_FAILURE;
  emm_failure_type      emm_failure_cause;
  lte_nas_tai_lst1_type  tai;
  emm_connection_state_type emm_connection_state_type= EMM_GET_CONNECTION_STATE();
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  plmn = rrc_service_ind->camped_svc_info.selected_plmn;

  MSG_HIGH_DS_6(MM_SUB, "=EMM= RRC_SERVICE_IND - MCC: D1 %u, D2 %u, D3 %u MNC: D1 %u, D2 %u, D3 %u",
                plmn.mcc[0],
                plmn.mcc[1],
                plmn.mcc[2],
                plmn.mnc[0],
                plmn.mnc[1],
                plmn.mnc[2]);
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
  MSG_HIGH_DS_6(MM_SUB, "=EMM= RRC_SERVICE_IND - TAC %u, Cell ID %u, IMS EMC spt %d Div Duplex %d, Detach Reqd %d search done%d",
                rrc_service_ind->camped_svc_info.tac,
                rrc_service_ind->camped_svc_info.cell_identity,
                rrc_service_ind->camped_svc_info.ims_emergency_support_enabled,
                rrc_service_ind->div_duplex,
                rrc_service_ind->detach_reqd,
				rrc_service_ind->oos_search_done); //FRACQ
#else
  MSG_HIGH_DS_5(MM_SUB, "=EMM= RRC_SERVICE_IND - TAC %u, Cell ID %u, Div Duplex %d, Detach Reqd %d search done%d",
                rrc_service_ind->camped_svc_info.tac,
                rrc_service_ind->camped_svc_info.cell_identity,
                rrc_service_ind->div_duplex,
                rrc_service_ind->detach_reqd,
				rrc_service_ind->oos_search_done); //FRACQ
#endif

  if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    /*reset RLF state*/
    emm_ctrl_data_ptr->rlf_state = FALSE;
  }
  
#ifdef  FEATURE_FEMTO_CSG
  MSG_HIGH_DS_2(MM_SUB, "=EMM= RRC_SERVICE_IND - CSG Id = %d,  Hybrid cell = %d",
                rrc_service_ind->camped_svc_info.csg_info.csg_id,
                rrc_service_ind->camped_svc_info.csg_info.hybrid_cell);
#endif 
#ifdef FEATURE_CIOT
MSG_HIGH_DS_2(MM_SUB, "=EMM= RRC_SERVICE_IND - Attach without PDN = %d  UPCIOT support %d",
								rrc_service_ind->camped_svc_info.attachWithoutPDNConnectivity,
								rrc_service_ind->up_ciot_supported);

#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  MSG_HIGH_DS_1(MM_SUB, "=EMM= RRC_SERVICE_IND - ACQ Status = %d",
                rrc_service_ind->acq_status);
#endif 

  /* If psm_restore_done flag is TRUE send the restored drx/edrx parameters to RRC now independent of
     service status */
  if(emm_ctrl_data_ptr->psm_restore_done == TRUE)
  {
	  
#ifdef FEATURE_NAS_GW
    /* Send DRX update to W-RRC */
    mm_send_rrc_set_drx_ind(); 
#endif
    /* Send DRX update to L-RRC */
    emm_send_rrc_drx_update();

    /* Send eDRX update to RRC */
    if(emm_ctrl_data_ptr->edrx_params.enabled == TRUE)
    {
      emm_send_rrc_edrx_update(emm_ctrl_data_ptr, emm_ctrl_data_ptr->edrx_params.enabled);
    }
    emm_ctrl_data_ptr->psm_restore_done = FALSE;
  }
  /* else send edrx params to RRC if those are valid and RRC is being activated */
  else if ((emm_ctrl_data_ptr->edrx_params.enabled == TRUE) &&
           (emm_ctrl_data_ptr->rrc_active == FALSE) && 
           (emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == FALSE) &&
           (emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == FALSE))
  {
    emm_send_rrc_edrx_update(emm_ctrl_data_ptr, emm_ctrl_data_ptr->edrx_params.enabled);
  }
  
#ifdef FEATURE_NAS_GW
  /* Reset the MM band related variables */
  mm_band_indicator = SYS_BAND_CLASS_NONE;
#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
  mm_active_band = SYS_BAND_MASK_EMPTY;
#endif
#endif

  if((emm_ctrl_data_ptr->rrc_active == FALSE) && 
     (emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == FALSE) &&
     (emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == FALSE))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= RRC is activated");
    emm_ctrl_data_ptr->rrc_active = TRUE;
  }
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
  emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = FALSE;
#endif
  if((EMM_GET_CONNECTION_STATE() == EMM_RELEASING_RRC_CONNECTION_STATE) &&
     (emm_ctrl_data_ptr->aborting_conn == FALSE))
  { 
    emm_connection_state_type = EMM_GET_CONNECTION_STATE();
    EMM_MOVES_TO_IDLE_STATE() ;

    /* reset the secure exchange messages */
    emm_ctrl_data_ptr->nas_secure_exchange = FALSE;         
  }

  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  last_saved_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  is_plmn_match = sys_plmn_match(last_saved_plmn, rrc_reported_plmn);

  if(rrc_service_ind->no_svc_cause == SYS_NO_SVC_CAUSE_RLF)
  {
    memscpy(&emm_ctrl_data_ptr->rlf_plmn, sizeof(sys_plmn_id_s_type),
           &emm_ctrl_data_ptr->plmn_service_state->plmn, sizeof(sys_plmn_id_s_type));
  }
  /* The tuple of plmn_identity, cell_identity identifies a cell uniquely. If 
     either of plmn_identity and cell_identity is different, that means that 
     UE has reselected to a new cell. UE needs to stop barring timers when cell
     is changed. */
  if((is_plmn_match == FALSE) ||
     (emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.cell_identity !=
       rrc_service_ind->camped_svc_info.cell_identity))
  {
      emm_stop_cell_barring_timers(emm_ctrl_data_ptr
#ifdef FEATURE_NAS_EAB
                                   ,TRUE
#endif      
                                  );
#ifdef FEATURE_CIOT
   emm_ctrl_data_ptr->rach_retry_counter = 0;
#endif
  }

#ifdef FEATURE_CIOT
  /* Got service. Need to run a local detach and attach right after this 
     if UE is attached on a different RAT than the active RAT
  */  
  if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
  { 
     if ((is_plmn_match == FALSE) &&
	   mm_checkif_rat_pri_list_has_only_rat(SYS_RAT_LTE_NB1_RADIO_ACCESS) &&
	   (emm_chk_nb1_rat() == TRUE) &&
           (emm_bit_mask_check(emm_ctrl_data_ptr->nas_ciot_capability_config,EMM_SMS_ONLY) == TRUE))
     {
        emm_ctrl_data_ptr->force_req_ser_domain = TRUE;
        MSG_HIGH_DS_1(MM_SUB, "=EMM= force_req_ser_domain %d ",emm_ctrl_data_ptr->force_req_ser_domain);
     }     
     emm_state = EMM_GET_STATE();     
     if ((emm_state != EMM_DEREGISTERED) && 
         (emm_state != EMM_NULL) &&
         (emm_ctrl_data_ptr->attached_lte_rat != SYS_RAT_NONE) &&
         (emm_ctrl_data_ptr->attached_lte_rat != emm_ctrl_data_ptr->reg_req_pending_ptr->rat))
     {
  #ifndef FEATURE_CATM1_DISABLE
        if ((emm_ctrl_data_ptr->ciot_irat_tau_mobility == TRUE) &&
            ((emm_ctrl_data_ptr->attached_lte_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS) ||
             (emm_ctrl_data_ptr->attached_lte_rat == SYS_RAT_LTE_M1_RADIO_ACCESS) ) &&
            (esm_get_attach_without_pdn() != ESM_BC_ON_ATTACH_ATTACH_WITHOUT_PDN))
        {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Inter-RAT TAU mobility between NB1 and S1 ");
            emm_build_and_send_ciot_rat_change_ind(mm_serving_plmn.info.active_rat);
            EMM_REGISTERED_CHANGE_SUB_STATE(EMM_REGISTERED_WAITING_FOR_ESM_CIOT_RAT_CHANGE);
        }
        else
#endif
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Local Detach on a Service Ind");
       
  #ifndef FEATURE_CATM1_DISABLE       
#if defined (FEATURE_CIOT) && defined(FEATURE_LTE_REL14)
          if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL14) && 
             (emm_ctrl_data_ptr->reg_req_pending_ptr->rat == SYS_RAT_LTE_M1_RADIO_ACCESS) &&
             (mm_timer_status[TIMER_T3448] != TIMER_STOPPED))
          {
            emm_handle_timer_t3448(FALSE, 0);
          }
#endif
#endif

#ifdef FEATURE_NAS_GW
          if(mm_service_loss_on_rat != SYS_RAT_NONE)
          {
            gmm_sm_send_rat_change_req(mm_service_loss_on_rat, TRUE);
            mm_send_cm_bearer_ctxt_transfer_ind(mm_serving_plmn.info.active_rat);
            if(mm_location_information.location_update_status != UPDATED)
            {
              emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
              MSG_HIGH_DS_0(MM_SUB,"=MM= Set SGs assosicaiton flag - MM update status is not updated on upper layer rat change");
            }
            mm_service_loss_on_rat = SYS_RAT_NONE;
          }
		  gmm_process_gprs_release_ind(NO_GPRS_CONTEXT);
#endif
          emm_build_and_send_detach_ind(EMM_DETACHED);    
          /* Acquired service on a different LTE rat, stop timers*/
          emm_rat_change_stop_timers(); 
          emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;
		  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_NO_SRV;
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED); 
          emm_ctrl_data_ptr->attached_lte_rat = SYS_RAT_NONE;
          /* Report to ESM and invalidate pdn conn req*/
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
          emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
       }
     }
     /*case where cell 1 supports attach withour PDN and celll 2 doesn't support attach without PDN, if no active bearer then do local detach and attach*/
     if(emm_ctrl_data_ptr->attach_without_pdn == TRUE && 
 	    rrc_service_ind->camped_svc_info.attachWithoutPDNConnectivity == FALSE &&
 	    (emm_convert_eps_bc_status_to_bit_mask(
              emm_ctrl_data_ptr->eps_bearer_context_status) == 0))
     {
       MSG_HIGH_DS_0(MM_SUB, "=EMM= Local Detach as cell doesnt support attach witout PDN and no pdn active as well");
       emm_build_and_send_detach_ind(EMM_DETACHED); 
       EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED);
     }
#ifdef FEATURE_CIOT	 
#ifndef FEATURE_CATM1_DISABLE  

	 if((emm_chk_nb1_rat() == FALSE ) &&
	   (emm_ctrl_data_ptr->emm_start_mo_excp_signalling))
	{
	  emm_process_pending_mo_exception_data(TRUE) ;
	  emm_ctrl_data_ptr->emm_start_mo_excp_signalling = FALSE;
	}
#endif
	   
#endif

     /*case where Ue registered with PDN on PLMN 1, but UE moved to PLMN2 and it supports attach without PDN, send cmd to ESM so that it doesn't detach on last pdn disconnect*/ 
	 /*need to be decided based on syteme's input, for now keep the behavior of doing detach
     if((emm_ctrl_data_ptr->attach_without_pdn == FALSE) &&
	 (emm_bit_mask_check(emm_ctrl_data_ptr->nas_ciot_capability_config, EMM_ER_WITHOUT_PDN) == TRUE)&&
 	(rrc_service_ind->camped_svc_info.attachWithoutPDNConnectivity == TRUE )&&
 	(emm_convert_eps_bc_status_to_bit_mask(
           emm_ctrl_data_ptr->eps_bearer_context_status) != 0))
     {
       MSG_HIGH_DS_0(MM_SUB, "=EMM= Set attach without PDN to true and send command to ESM");
       emm_ctrl_data_ptr->attach_without_pdn = TRUE;
       emm_build_and_send_attach_status_ind(TRUE);
     }*/
  }
  if ((rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_NOT_AVAILABLE)&&
      (rrc_service_ind-> oos_due_to_rach_optmz == TRUE))
  {
      emm_ctrl_data_ptr->plmn_service_state->oos_due_to_rach_optmz = TRUE;
  }
  else
  {
      emm_ctrl_data_ptr->plmn_service_state->oos_due_to_rach_optmz = FALSE;
  }
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Rach indication set to %d", emm_ctrl_data_ptr->plmn_service_state->oos_due_to_rach_optmz);
#endif

  /* At this point, EMM can be only in IDLE or CONNECTED state 
     Only process pending REG_REQ and pending message in IDLE state 
     Note: If this service ind comes when a RLF is recovered, the connection
     state will be CONNECTED and RRC will send DATA_CNF for the pending 
     message before the RLF. */
  if((EMM_GET_CONNECTION_STATE() == EMM_IDLE_STATE) &&
     (emm_reg_container != NULL))
  {
    if(emm_ctrl_data_ptr->client_list != NULL)
    {
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      emm_process_unsent_messages(CONN_REL_LTE_RRC_CONN_REL_NORMAL,
                                  emm_failure_cause, 
                                  emm_ctrl_data_ptr);
#ifdef FEATURE_LTE_TO_1X
      /* Process all pending irat uplink messages */
      emm_process_pending_irat_msg(LTE_NAS_IRAT_INVALID_STATE,
                                   LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                   LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                                   emm_ctrl_data_ptr);
#endif
    }
    if(emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == FALSE)
    {
      emm_reg_container(emm_ctrl_data_ptr);
    }
  }

  MSG_HIGH_DS_4(MM_SUB, "=EMM= SRV REQ pending %d, SRV REQ 0x%x, RRC SRV IND 0x%x, num_plmn %d",
                emm_ctrl_data_ptr->reg_req_pended_indication,
                emm_ctrl_data_ptr->reg_init_rrc_trans_id,
                rrc_service_ind->trans_id,
                rrc_service_ind->camped_svc_info.shared_plmn_list.num_plmn);

  emm_state = EMM_GET_STATE();
#ifdef FEATURE_NAS_GW
  if(emm_ctrl_data_ptr->tau_pending_gw_attach == TRUE &&
	 rrc_service_ind->detach_reqd == TRUE)
  {
     rrc_service_ind->detach_reqd = FALSE;
  }
#endif

  if((emm_ctrl_data_ptr->reg_req_pended_indication == TRUE) &&
     (emm_ctrl_data_ptr->reg_init_rrc_trans_id != rrc_service_ind->trans_id))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= SRV IND is not rsp for pending SRV REQ");
    if((rrc_service_ind->detach_reqd == TRUE) && 
    (emm_state != EMM_DEREGISTERED)) 
    {
           /*To delay process div duplex and also reset pending div duplex if it happened again since UE is back to the same mode now */
          emm_ctrl_data_ptr->emm_lte_mode_change_pending = (emm_ctrl_data_ptr->emm_lte_mode_change_pending == FALSE) ? TRUE : FALSE ;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Div Duplex Chg handling pending - %d",
                        emm_ctrl_data_ptr->emm_lte_mode_change_pending);
    }
    return;
  }
  
  if(emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_REQ_PENDING)
  {
    emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;

    /* save the cell ID, cell frequency and global cell ID for cell barring */
    emm_ctrl_data_ptr->phy_cell_id = 
                                  rrc_service_ind->camped_svc_info.phy_cell_id;
    emm_ctrl_data_ptr->freq = rrc_service_ind->camped_svc_info.earfcn;
    emm_ctrl_data_ptr->cell_identity = 
                                rrc_service_ind->camped_svc_info.cell_identity;

    if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
    {
      emm_send_plmn_change_ind(rrc_reported_plmn);
    }

    memscpy(emm_ctrl_data_ptr->last_rrc_service_ind_ptr, sizeof(lte_rrc_service_ind_s),
            rrc_service_ind, sizeof(lte_rrc_service_ind_s));

    if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_NOT_AVAILABLE)
    {
      if(rrc_service_ind->no_svc_cause != SYS_NO_SVC_CAUSE_RLF)
      {
        memset((void *)emm_ctrl_data_ptr->plmn_service_state->plmn.identity,
               0xFF, sizeof(sys_plmn_id_s_type));
        emm_ctrl_data_ptr->plmn_service_state->tac = 0xFFFE;
      }
       
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                         SYS_SRV_STATUS_NO_SRV;
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
      emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id = SYS_CSG_ID_INVALID;
#endif 
    }
    else
    {
      emm_ctrl_data_ptr->plmn_service_state->plmn = 
       emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

      emm_ctrl_data_ptr->plmn_service_state->tac = 
                   emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;

				   
      SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band);
      SYS_LTE_BAND_MASK_ADD_BAND(&emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band,
                                 emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_band);

#ifdef FEATURE_NAS_GW
      /* Set the MSCR to 1 in case of LTE operation */
      mm_system_information.mscr = 1;  
#endif
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
       emm_ctrl_data_ptr->plmn_service_state->csg_info = rrc_service_ind->camped_svc_info.csg_info;
#endif 
#ifdef FEATURE_CIOT
	emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.up_ciot_supported = 
												  (rrc_service_ind->up_ciot_supported)
												  &&emm_bit_mask_check(emm_ctrl_data_ptr->nas_ciot_capability_config, EMM_UP_CIOT)
												  &&emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.up_ciot_supported;
	emm_ctrl_data_ptr->is_conn_suspended = (emm_ctrl_data_ptr->is_conn_suspended)
												  &&emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.up_ciot_supported;
												  
	if(emm_ctrl_data_ptr->is_conn_suspended == FALSE)
	{
	 emm_ctrl_data_ptr->conn_suspended_rat = SYS_RAT_NONE;
	}
#endif
    }

#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
    emm_ctrl_data_ptr->plmn_service_state->acq_status = rrc_service_ind->acq_status;
#endif 
    emm_set_tau_trm_priority(FALSE);
    if(emm_ctrl_data_ptr->suitable_search_end_pending == TRUE)
    {
      emm_send_reg_t311_timer_expiry_ind();
      emm_ctrl_data_ptr->suitable_search_end_pending = FALSE;
    }
    emm_process_ps_detach_req(emm_ctrl_data_ptr->ps_detach_info.ps_detach_req, 
                              emm_ctrl_data_ptr);


    return;
  }

  /* rrc_active can be FALSE only when RRC is being deactivated */ 
  if(emm_ctrl_data_ptr->rrc_active == FALSE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= RRC is being deactivated - Do not further process");
    emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;
    reg_service_report = (emm_reg_service_report_type)emm_send_reg_ind;
    return;
  }

#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  emm_ctrl_data_ptr->plmn_service_state->acq_status = rrc_service_ind->acq_status;
#endif 
  if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    /* Set t311_rem_time = 0 & no_svc_cause = SYS_NO_SVC_CAUSE_NORMAL
       to prevent Service request to be sent in connected state during
       w2l handover*/
    emm_ctrl_data_ptr->last_rrc_service_ind_ptr->t311_rem_time = 0;
    emm_ctrl_data_ptr->last_rrc_service_ind_ptr->no_svc_cause = 
                                                SYS_NO_SVC_CAUSE_NORMAL;
    
/*Set lu initiated flag to TRUE if Attach Rejected in GW with congestion and moved to LTE such that in case of going back to GW, LU should be performed
As per 24.008: 4.4.1
The normal location updating procedure shall also be started when the MS, configured to use CS fallback and SMS over SGs, 
or SMS over SGs only, enters a GERAN or UTRAN cell after intersystem change from S1 mode to Iu or A/Gb mode, if timer T3346 is running, 
and the location area of the current cell is the same as the stored location area.*/
#ifdef FEATURE_NAS_GW
#ifdef FEATURE_NAS_REL10
#ifdef FEATURE_LTE
    if(mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
    {
      mm_is_lu_to_be_initiated_t3346_ltogw = TRUE;
    }
#endif
#endif
#endif
#ifdef FEATURE_NAS_REL11
#ifdef FEATURE_NAS_GW
/*Reset flag when moved from LTE to G to LTE*/
      gmm_is_lw_to_g_interrat_in_progress = FALSE;
#endif
#endif
    /* This function sends rat change request to SM and 
       resets the loss on service global variables of LTE and GW */

    /* If GW exists then update MM/GMM */
/*    if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                       SYS_RAT_UMTS_RADIO_ACCESS) || 
       mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                       SYS_RAT_GSM_RADIO_ACCESS)  || 
       mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                       SYS_RAT_TDS_RADIO_ACCESS))*/
									   
#ifdef FEATURE_CIOT
if(emm_ctrl_data_ptr->conn_suspended_rat != emm_ctrl_data_ptr->plmn_service_state->active_rat )
  {
   emm_ctrl_data_ptr->is_conn_suspended = FALSE;
   emm_ctrl_data_ptr->conn_suspended_rat = SYS_RAT_NONE;
  }
#endif

#ifdef FEATURE_NAS_GW
   emm_service_acquired_on_lte(emm_ctrl_data_ptr,emm_convert_rrc_plmn_id_to_nas_plmn_id(rrc_service_ind->camped_svc_info.selected_plmn));
#if defined(FEATURE_WRLF_SYSTEM_SEL) && !defined(FEATURE_NAS_CS_DISABLED)
    if((mm_wcdma_rlf_state == WCDMA_RLF_STARTED)&&
       (emm_registration_allowance_chk(rrc_service_ind,emm_ctrl_data_ptr) == TRUE))
    {
      if(pmm_mode == PMM_CONNECTED)
      {
        if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
        {
          emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);
        }

        MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU to be performed - PMM CONNECTED MODE");
        emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      }
      MSG_HIGH_DS_0(MM_SUB, "=EMM= send RRC ABORT REQ TO WCDMA TO COME OUT OF RLF STATE");
      mm_send_rrc_abort_req_during_wrlf();
    }
#endif
#endif

    emm_ctrl_data_ptr->suitable_search_end_pending = FALSE;
  }


#ifdef FEATURE_NAS_GW
  emm_ctrl_data_ptr->ignore_GW_activation_ind = FALSE;
#endif
  /* save the cell ID ,cell frequency and global cell ID for cell barring */
  emm_ctrl_data_ptr->phy_cell_id = rrc_service_ind->camped_svc_info.phy_cell_id;
  emm_ctrl_data_ptr->freq = rrc_service_ind->camped_svc_info.earfcn;
  emm_ctrl_data_ptr->cell_identity = 
                                rrc_service_ind->camped_svc_info.cell_identity;

  memscpy(emm_ctrl_data_ptr->last_rrc_service_ind_ptr, sizeof(lte_rrc_service_ind_s),
            rrc_service_ind, sizeof(lte_rrc_service_ind_s));

  emm_ctrl_data_ptr->plmn_service_state->sib8_available = 
                                               rrc_service_ind->sib8_available;

  emm_state = EMM_GET_STATE();

  /* RRC reports that LTE service IS avaliable */
  if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    emm_ctrl_data_ptr->plmn_service_state->plmn  =
      emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

    emm_ctrl_data_ptr->plmn_service_state->tac = 
                   emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;
				   
    SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band);
    SYS_LTE_BAND_MASK_ADD_BAND(&emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band,
                               emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_band);

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
     emm_ctrl_data_ptr->plmn_service_state->csg_info = rrc_service_ind->camped_svc_info.csg_info;
#endif 

    emm_send_plmn_change_ind(rrc_reported_plmn);
#ifdef FEATURE_CIOT
	emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.up_ciot_supported = 
												  (rrc_service_ind->up_ciot_supported)
												  &&emm_bit_mask_check(emm_ctrl_data_ptr->nas_ciot_capability_config, EMM_UP_CIOT)
												  &&emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.up_ciot_supported;
	emm_ctrl_data_ptr->is_conn_suspended = (emm_ctrl_data_ptr->is_conn_suspended)
												  &&emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.up_ciot_supported;
												  
	if(emm_ctrl_data_ptr->is_conn_suspended == FALSE)
	{
	 emm_ctrl_data_ptr->conn_suspended_rat = SYS_RAT_NONE;
	}
#endif	

#ifdef FEATURE_NAS_GW
    /* Set the MSCR to 1 in case of LTE operation */
    mm_system_information.mscr = 1;
#endif
    if(((rrc_service_ind->detach_reqd == TRUE)|| 
    (emm_ctrl_data_ptr->emm_lte_mode_change_pending == TRUE)) && 
    (emm_state != EMM_DEREGISTERED)) 
    {
     if(emm_registration_allowance_chk(rrc_service_ind,emm_ctrl_data_ptr) == FALSE)
      {
        if(rrc_service_ind->detach_reqd == TRUE)
        {
          /* reset pending div duplex if it happened again since UE is back to the same mode now */
          emm_ctrl_data_ptr->emm_lte_mode_change_pending = (emm_ctrl_data_ptr->emm_lte_mode_change_pending == FALSE) ? TRUE : FALSE ;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Pending DETACH/ATTACH due to div_duplex_chg, emm_lte_mode_change_pending set to %d\n",
                        emm_ctrl_data_ptr->emm_lte_mode_change_pending);
        }
      }
     else
      {
        /* process div duplex if div duplex happened only once - but not twice in which case there is no mode change effectively */
        if(((emm_ctrl_data_ptr->emm_lte_mode_change_pending == FALSE) && (rrc_service_ind->detach_reqd == TRUE)) ||
        ((emm_ctrl_data_ptr->emm_lte_mode_change_pending == TRUE) && (rrc_service_ind->detach_reqd == FALSE)))
        {
          emm_set_tau_trm_priority(FALSE);
          emm_process_div_duplex_chg(emm_ctrl_data_ptr, rrc_service_ind);
          return;
        }
        /* If there is no mode change effectively, reset the flag, as we do not need to ATTACH / DETACH */
        emm_ctrl_data_ptr->emm_lte_mode_change_pending = FALSE;
      }
    }

#ifdef FEATURE_LTE_REL10
    if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) 
    {
      /* if registration is not allowe wait for t3346 timer expiry */
      if(emm_chk_backoff_congestion_allowed(rrc_service_ind, 
                                            emm_ctrl_data_ptr) == FALSE)
      {
		if(((emm_connection_state_type == EMM_RELEASING_RRC_CONNECTION_STATE) && 
    	(emm_ctrl_data_ptr->emm_connection_state == EMM_IDLE_STATE) && 
    	(emm_check_ready_for_psm(emm_ctrl_data_ptr) == SYS_PSM_STATUS_READY)))
    	{
    	 if(mm_psm_ready_req_rejected == TRUE)
    	 {
    	 	mm_send_mmr_psm_ready_ind();
    	 }
		 else if((*emm_ctrl_data_ptr->t3324_expired_ptr == TRUE) && (emm_ctrl_data_ptr->t3324_value != NAS_INVALID_PSM_TIMER_VALUE))
		 {
		 	emm_timer_t3324_expire(emm_ctrl_data_ptr);
		 } 
    	}
        emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;
        emm_set_tau_trm_priority(FALSE);
        return;
      }
    }
#endif
    if((EMM_GET_CONNECTION_STATE() == EMM_IDLE_STATE) &&
       (emm_ctrl_data_ptr->client_list != NULL))
    {
      tai.plmn = rrc_reported_plmn;
      tai.tac = rrc_service_ind->camped_svc_info.tac;
      if(emm_is_tai_forbidden(tai,emm_ctrl_data_ptr)
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED) && defined(FEATURE_NAS_E911_ENABLE)
        && ((*emm_ctrl_data_ptr->emc_srv_status != NAS_EMC_SRV_PENDING) &&
        (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == FALSE))
#endif
        )
	  {
	    emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
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
      }
      else
      {
        emm_autonomous_connection_setup(emm_ctrl_data_ptr) ;
      }
    }

    if((emm_ctrl_data_ptr->plmn_service_state->sim_state != 
                                                    SYS_SIM_STATE_AVAILABLE) &&
       (emm_ctrl_data_ptr->plmn_service_state->sim_state != 
                                                   SYS_SIM_STATE_CS_INVALID) &&
       (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode != 
                                          SYS_NETWORK_SELECTION_MODE_LIMITED)
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

        &&(emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)&&
           !(((emm_state == EMM_DEREGISTERED)||(emm_state == EMM_REGISTERED_INITIATED))&&
            (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING))
#endif
#endif
#ifdef FEATURE_CIOT
         && (emm_ctrl_data_ptr->plmn_service_state->sim_state != 
                                                   SYS_SIM_STATE_CP_SMS_INVALID)
#endif
        )
    {
      MSG_HIGH_DS_3(MM_SUB, "=EMM= Invalid SIM_STATE %d, EMM state %d, NW_SEL_MODE %d",
                    emm_ctrl_data_ptr->plmn_service_state->sim_state,
                    emm_state,
                    emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
      if(emm_state == EMM_DEREGISTERED)
      {        
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH) ;        
      }
      else
      {
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_PLMN_SEARCH) ;      
      } 
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                  SYS_SRV_STATUS_LIMITED ;
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
      return;  
    }
    emm_state = EMM_GET_STATE();
    switch(emm_state)
    {
      case EMM_DEREGISTERED:      
        emm_deregistered_process_rrc_service_ind(rrc_service_ind,
                                                 emm_ctrl_data_ptr);
        break ;
      case EMM_REGISTERED_INITIATED:
        emm_attach_collision_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      case EMM_REGISTERED:
#if defined(FEATURE_NAS_GW) && defined(FEATURE_NAS_SM_IRAT)
        if(emm_ctrl_data_ptr->TIN != NAS_MM_TIN_RAT_RELATED_TMSI)
        {
          emm_registered_process_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        }
        else if(EMM_GET_SUBSTATE() != EMM_REGISTERED_WAITING_FOR_ESM_ISR_STATUS)
        {
          emm_build_and_send_get_isr_status_ind();
          EMM_REGISTERED_CHANGE_SUB_STATE(EMM_REGISTERED_WAITING_FOR_ESM_ISR_STATUS);
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Received service indication in waiting for esm isr status substate");
        }
#else
#ifdef FEATURE_CIOT
#ifndef FEATURE_CATM1_DISABLE
        if(EMM_GET_SUBSTATE() == EMM_REGISTERED_WAITING_FOR_ESM_CIOT_RAT_CHANGE)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Received service indication in waiting for CIOT Rat change substate");
        }
        else
#endif
#endif
        {
          emm_registered_process_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        }
#endif
        break ;
      case EMM_TRACKING_AREA_UPDATING_INITIATED:
        emm_tau_collision_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      case EMM_SERVICE_REQUEST_INITIATED:
        emm_service_request_collision_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
	    if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE
#if defined(FEATURE_LTE_TO_1X) || defined(FEATURE_NAS_GW) 
		&&
		  	 (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)
#endif
		)
        {
		  emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_SRV;
        }
        break ;
      case EMM_DEREGISTERED_INITIATED:
        emm_detach_collision_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      default:
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM state",0,0,0) ;
        break ;
    }
	  /* Send eDRX params change ind to upper layers */
	  if (emm_get_rat_for_eDRX_param() != SYS_RAT_NONE) 
	  {
		mm_send_mmr_edrx_params_change_ind(emm_get_rat_for_eDRX_param(),
										   (boolean)(rrc_service_ind->camped_svc_info.edrx_support &
													 emm_ctrl_data_ptr->edrx_params.enabled
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE
	
													 & (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
#endif
													 ),
										   emm_ctrl_data_ptr->edrx_params.ptw,
										   emm_ctrl_data_ptr->edrx_params.cycle_length);
	  }

    if(emm_ctrl_data_ptr->reg_init_rrc_trans_id == rrc_service_ind->trans_id)
    {
#ifdef FEATURE_NAS_GW
      mmsend_rat_change_event();//Post RAT change event
#ifdef FEATURE_ENHANCED_NW_SELECTION
      mm_inform_updated_foreground_search_list();
#endif
#endif
    }
  }
  else /* RRC reports that LTE service is NOT avaliable */
  {
    if(rrc_service_ind->no_svc_cause == SYS_NO_SVC_CAUSE_RLF)
    {
      emm_ctrl_data_ptr->rlf_state = TRUE;
    }
    switch(emm_state)
    {
      case EMM_DEREGISTERED:
        if((emm_ctrl_data_ptr->mt_detach_info.type == REATTACH_NOT_REQUIURED)&&
           ((emm_ctrl_data_ptr->mt_detach_info.emm_cause == LTE_NAS_ILLEGAL_UE)||
           (emm_ctrl_data_ptr->mt_detach_info.emm_cause == LTE_NAS_ILLEGAL_ME)||
           (emm_ctrl_data_ptr->mt_detach_info.emm_cause == LTE_NAS_IMSI_UNKNOWN_IN_HSS)||
           (emm_ctrl_data_ptr->mt_detach_info.emm_cause == LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED)||
           (emm_ctrl_data_ptr->mt_detach_info.emm_cause == LTE_NAS_EPS_SERVICES_NOT_ALLOWED)))
        {
          /* Mobility info and EPS status */
          emm_complete_mt_detach(0);
        }
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
        break;
      case EMM_REGISTERED_INITIATED:
        if((emm_ctrl_data_ptr->rlf_state == FALSE)
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE
          || ((emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING)
          ||(emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr)))
#endif
#endif
		  )
		{
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
          mm_stop_timer(TIMER_T3410);
          emm_delete_pended_message_with_msg_id(ATTACH_REQUEST,emm_ctrl_data_ptr);
          MSG_LOW_DS_0(MM_SUB, "Deleted Attach Request from pended message queue\n");
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

          if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING)
          {
            emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
          }
#endif
#endif
          if(EMM_GET_SUBSTATE() == EMM_WAITING_FOR_ESM_RESPONSE)
          {
            emm_build_and_send_detach_ind(EMM_DETACHED);
          }
          emm_send_diag_event(EVENT_NAS_ATTACH, (nas_event_end_result)PROC_END,
			      (nas_event_end_reason)PROC_END_LOWER_LAYER_FAILURE, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
          emm_remove_release_client(emm_attach_release_indication);
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
		}
        break;
      case EMM_DEREGISTERED_INITIATED:
        if(emm_ctrl_data_ptr->detach_reason == NORMAL_DETACH)
        {
          if(emm_ctrl_data_ptr->ps_detach_info.status == 
                                                     EMM_PS_DETACH_CNF_PENDING)
          {
            emm_abort_mo_detach(emm_ctrl_data_ptr, FALSE);
            /* If PS DETACH pending, do clean up later.*/
            EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
          }
          else
          {          
            if(emm_ctrl_data_ptr->ongoing_detach_type == IMSI_DETACH_MO_DETACH)
            {
              emm_abort_mo_detach(emm_ctrl_data_ptr, FALSE);
              EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
            }
            else if(EMM_GET_CONNECTION_STATE() != EMM_CONNECTED_STATE)
            { 
              emm_abort_mo_detach(emm_ctrl_data_ptr, FALSE);         
              EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
            }
          }
        }
        else if((mm_timer_status[EMM_POWEROFF_DETACH_TIMER] == TIMER_ACTIVE)&&
          (emm_ctrl_data_ptr->detach_reason == SWITCH_OFF))
        {
          emm_remove_release_client(emm_poweroff_detach_release_ind);
          mm_stop_timer(EMM_POWEROFF_DETACH_TIMER); 
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
          emm_reset_poweroff_detach_params(emm_ctrl_data_ptr);
          emm_init_rrc_deact_req(emm_ctrl_data_ptr->rrc_deact_req.deact_reason, 
                         emm_ctrl_data_ptr);
        }
        else
        {
          emm_abort_mo_detach(emm_ctrl_data_ptr, FALSE);
        }
        break ;
      case EMM_REGISTERED:
        mm_stop_timer(TIMER_DELAY_TAU);
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
        break;
      case EMM_TRACKING_AREA_UPDATING_INITIATED:
        if((emm_ctrl_data_ptr->rlf_state == FALSE)
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE			
          || ((emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING)
          ||(emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr)))
#endif
#endif
		  )
        {
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
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_CS_DISABLED))
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
        break;      
      case EMM_SERVICE_REQUEST_INITIATED:

        if(emm_ctrl_data_ptr->rlf_state == FALSE
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
             || (emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE)
#endif
          )
        {
          /* EMM will be moved to REGISTERED_NO_CELL_AVAILABLE substate in
             emm_abort_service_request */
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
          //MNK:CSFB_FAILURE
          if(emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE)
          {
              //emm_convert_rrc_failure_to_local_cause(rrc_service_ind->no_svc_cause);
            if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB) &&
               (rrc_service_ind->no_svc_cause == SYS_NO_SVC_CAUSE_RLF))
            {
              local_cause = LTE_NAS_REJ_LRRC_RADIO_LINK_FAILURE;  
            }
#ifdef FEATURE_DUAL_SIM
            if(((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB) ||
                (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED)) &&
               (rrc_service_ind->acq_status == SYS_ACQ_STATUS_NO_RESOURCE))
            {
              local_cause = LTE_NAS_REJ_NO_SERVICE_TRM_UNAVAILABLE;
            }
#endif
            emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                      TRUE, local_cause,
                                      emm_failure_cause);
          }
          else
#endif
          {
            emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                      TRUE, LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                      emm_failure_cause);

          }
	}
        break ;
      default:
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM state",0,0,0) ;
        break ;
    }

    if(EMM_GET_CONNECTION_STATE() == EMM_IDLE_STATE) 
    {
      emm_failure_cause.cause_type = LTE_NAS_IRAT_LOWER_LAYER_FAILURE;
      /* Process all unsent messages in buffer */
      emm_process_unsent_messages(CONN_CNF_LTE_RRC_CONN_EST_FAILURE,
                                  emm_failure_cause, 
                                  emm_ctrl_data_ptr);
#ifdef FEATURE_LTE_TO_1X
      /* Process all pending irat uplink messages */
      emm_process_pending_irat_msg(LTE_NAS_IRAT_LOWER_LAYER_FAILURE,
                                   LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                   LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                                   emm_ctrl_data_ptr);
#endif
    }

    /* Indicate the Upper layers that Service is no longer avaliable */
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                         SYS_SRV_STATUS_NO_SRV;

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
    emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id = SYS_CSG_ID_INVALID;
#endif 

#ifdef FEATURE_1XSRLTE
    if((emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode ==
                        SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
       (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode ==
                        SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY))
    {
      emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                            SYS_REG_DOMAIN_NO_SERVICE;
    }
#endif 

    if(rrc_service_ind->no_svc_cause != SYS_NO_SVC_CAUSE_RLF)
    {
      memset((void *)emm_ctrl_data_ptr->plmn_service_state->plmn.identity, 
             0xFF,
             sizeof(sys_plmn_id_s_type));
      emm_ctrl_data_ptr->plmn_service_state->tac = 0xFFFE;
    }

    if(emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_CNF_PENDING)
    {
      emm_ps_detach_clean_up(emm_ctrl_data_ptr);
    }
    /* If registration ack and PLMN blocking is pending then block the PLMN
       emm_process_block_plmn_req will not trigger service ind/cnf as service status is NO service */
    if(emm_ctrl_data_ptr->reg_complete_ack_pending != EMM_PENDING_REG_ACK_NONE)
    {
      if(emm_ctrl_data_ptr->is_plmn_block_req_pending == TRUE)
      {
        emm_process_block_plmn_req(&emm_ctrl_data_ptr->pending_block_plmn_req, emm_ctrl_data_ptr);
      }
    }

    /* Inform REG regarding current level of Service */
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
    emm_send_reg_attach_failed_ind();
    if(emm_ctrl_data_ptr->suitable_search_end_pending == TRUE)
    {
      emm_send_reg_t311_timer_expiry_ind();
      emm_ctrl_data_ptr->suitable_search_end_pending = FALSE;
    }
  }
  emm_state = EMM_GET_STATE();
#ifdef FEATURE_DUAL_SIM
  /*
  ** Do not reset TAU TRM priority if
  **  a) if EMM state is TAU initiated or
  **  b) if EMM state is REGISTERED and TAU complete is pending or 
  **  c) if EMM state is REGISTERED and EMM is waiting for ISR status response from ESM.
  */
  if((emm_ctrl_data_ptr->tau_trm_priority == TRUE) &&
     !((emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED) ||
       ((emm_state == EMM_REGISTERED) &&
        ((emm_ctrl_data_ptr->reg_complete_ack_pending == EMM_PENDING_REG_ACK_TAU_COMPLETE) ||
         (EMM_GET_SUBSTATE() == EMM_REGISTERED_WAITING_FOR_ESM_ISR_STATUS)))))
  {
    emm_set_tau_trm_priority(FALSE);
  }
#endif
#ifdef FEATURE_CIOT
 if(emm_ctrl_data_ptr->reg_req_pended_indication &&
 	emm_ctrl_data_ptr->emm_start_mo_excp_signalling &&
 	((emm_state == EMM_REGISTERED)||
 	 (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED)))
 {
   emm_process_pending_mo_exception_data(FALSE);
 }
#endif
  emm_ctrl_data_ptr->reg_req_pended_indication = FALSE ;
  if(((emm_connection_state_type == EMM_RELEASING_RRC_CONNECTION_STATE) && 
    	(emm_ctrl_data_ptr->emm_connection_state == EMM_IDLE_STATE) && 
    	(emm_check_ready_for_psm(emm_ctrl_data_ptr) == SYS_PSM_STATUS_READY)))
    	{
    	 if(mm_psm_ready_req_rejected == TRUE)
    	 {
    	 	mm_send_mmr_psm_ready_ind();
    	 }
		 else if((*emm_ctrl_data_ptr->t3324_expired_ptr == TRUE) && (emm_ctrl_data_ptr->t3324_value != NAS_INVALID_PSM_TIMER_VALUE))
		 {
		 	emm_timer_t3324_expire(emm_ctrl_data_ptr);
		 } 
    	}
} /* end of emm_rrc_process_service_indication() */


/*===========================================================================
FUNCTION    EMM_RRC_PROCESS_BARRING_UPDATE_INDICATION

DESCRIPTION
  This function processes LTE_RRC_BARRING_UPDATE_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_rrc_process_barring_update_indication
(
  lte_rrc_barring_update_ind_s *rrc_barring_update_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
   
  MSG_HIGH_DS_2(MM_SUB, "=EMM= LTE_RRC_BARRING_UPDATE_IND - emergency_access_barred %u, skip_acb_for_volte_call %u",
                rrc_barring_update_ind->emergency_access_barred,
                rrc_barring_update_ind->skip_acb_for_volte_call);
 MSG_HIGH_DS_4(MM_SUB, "=EMM= LTE_RRC_BARRING_UPDATE_IND - ac_Barring_r12_present %d,ac barring skip for voice %d,ac barring skip for video %d,ac barring for SMS %d ",
                         rrc_barring_update_ind->ac_barring_info.ac_Barring_r12_present,
                         rrc_barring_update_ind->ac_barring_info.ac_BarringSkipForMMTELVoice_r12,
                         rrc_barring_update_ind->ac_barring_info.ac_BarringSkipForMMTELVideo_r12,
                         rrc_barring_update_ind->ac_barring_info.ac_BarringSkipForSMS_r12);

  emm_ctrl_data_ptr->sib2_ac_barring_info.ac_Barring_r12_present = rrc_barring_update_ind->ac_barring_info.ac_Barring_r12_present;

  if(rrc_barring_update_ind->ac_barring_info.ac_Barring_r12_present)/*use sib2 info for skipping ACB*/
  {
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
    emm_ctrl_data_ptr->sib2_ac_barring_info.skip_ac_barring_for_volte_call = rrc_barring_update_ind->ac_barring_info.ac_BarringSkipForMMTELVoice_r12;
    emm_ctrl_data_ptr->sib2_ac_barring_info.skip_ac_barring_for_video_call = rrc_barring_update_ind->ac_barring_info.ac_BarringSkipForMMTELVideo_r12;
#endif
    emm_ctrl_data_ptr->sib2_ac_barring_info.skip_ac_barring_for_for_sms = rrc_barring_update_ind->ac_barring_info.ac_BarringSkipForSMS_r12;
  }
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
  else/*falls back to legacy -NV*/
  {
    emm_ctrl_data_ptr->skip_acb_for_volte_call = rrc_barring_update_ind->skip_acb_for_volte_call;
  }
#endif
#ifdef FEATURE_LTE_REL9
  emm_send_mmr_lte_cell_info_ind(rrc_barring_update_ind->emergency_access_barred);
#endif
  return;
}



/*===========================================================================
FUNCTION    EMM_ESTIMATE_DL_COUNT

DESCRIPTION
  This function derives a 32 bit DL count from the given 4 bits of DL NAS count from MME.
  This derivation is explained in Section 4.8 of Multimode Inter-RAT Security FDD 
  (Doc# 80-N1321-1 A)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_estimate_dl_count
(
  lte_rrc_nas_umts_key_ind_s *rrc_nas_umts_key_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr,
  byte *nas_dl_count,
  size_t nas_dl_count_buf_len
)
{
  emm_security_context_info *context_ptr = NULL;
  byte  out_count[4]; 
  uint8 y1, y2, y_hash;
  word over_flow_cnt = 0;
  word under_flow_cnt = 0;
   
  
  memset(out_count,0, sizeof(byte)* 4);

  /* get the current context */
  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  if( context_ptr != NULL )
  {
    /* Ignore the spare bits in MSB*/
    rrc_nas_umts_key_ind->dl_nas_count &= 0xF;

    if(mmgsdi_is_test_mode_logging_enabled_for_subscription((sys_modem_as_id_e_type)mm_sub_id))
    {
      MSG_HIGH_DS_4(MM_SUB, "=EMM= [Current UE DL NAS Count] context_ptr->nas_dl_cnt[0] = %d context_ptr->nas_dl_cnt[1] = %d context_ptr->nas_dl_cnt[2] = %d context_ptr->nas_dl_cnt[3] = %d",
                             context_ptr->nas_dl_cnt[0],context_ptr->nas_dl_cnt[1],context_ptr->nas_dl_cnt[2],context_ptr->nas_dl_cnt[3]);
    }

    y1 = ((context_ptr->nas_dl_cnt[0] & 0xF0) >> 4);
    y2 = context_ptr->nas_dl_cnt[0] & 0x0F;
    y_hash = (y2 - 8) & 0xF;

    MSG_HIGH_DS_4(MM_SUB, "=EMM= y1 = %d, y2 = %d, y_hash = %d [DEBUG RX] rrc_nas_umts_key_ind->dl_nas_count %d ", y1, y2, y_hash,rrc_nas_umts_key_ind->dl_nas_count);

    if(y2 > y_hash) /* y_hash does not wrap around */
    {
      if(((y2 <= rrc_nas_umts_key_ind->dl_nas_count) && (rrc_nas_umts_key_ind->dl_nas_count <= 15)) ||
         ((y_hash < rrc_nas_umts_key_ind->dl_nas_count) && (rrc_nas_umts_key_ind->dl_nas_count < y2)))
      {
        out_count[0] = (y1 << 4) | rrc_nas_umts_key_ind->dl_nas_count;
        out_count[1] = context_ptr->nas_dl_cnt[1];
        out_count[2] = context_ptr->nas_dl_cnt[2];
        out_count[3] = 0;
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Debug out_count[0] = %d", out_count[0]);
      }
      else if(rrc_nas_umts_key_ind->dl_nas_count <= y_hash)
      {
        if (y1 >= 15) /* y1+1 will overflow */
        {
          over_flow_cnt = ( (word )context_ptr->nas_dl_cnt[2] << 8) | context_ptr->nas_dl_cnt[1];
          over_flow_cnt++ ;

          MSG_HIGH_DS_0(MM_SUB, "=EMM= overflow detected, Increment overflow count");
          out_count[0] = ((y1-15) << 4) | rrc_nas_umts_key_ind->dl_nas_count;
          /* Increment Overflow count */
          out_count[1] = (byte )(over_flow_cnt & 0xFF);
          out_count[2] = (byte )((over_flow_cnt & 0xFF00) >> 8);
          out_count[3] = 0;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Debug out_count[0] = %d", out_count[0]);
        }
        else /* y1+1 in not overflow */
        {
          out_count[0] = ((y1+1) << 4) | rrc_nas_umts_key_ind->dl_nas_count;
          out_count[1] = context_ptr->nas_dl_cnt[1];
          out_count[2] = context_ptr->nas_dl_cnt[2];
          out_count[3] = 0;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Debug out_count[0] = %d", out_count[0]);
         }
      }
    }
    else /* y_hash does wrap around */
    {
      if(((y2 <= rrc_nas_umts_key_ind->dl_nas_count) && (rrc_nas_umts_key_ind->dl_nas_count <= y_hash)) ||
         (rrc_nas_umts_key_ind->dl_nas_count < y2))
      {
        out_count[0] = (y1 << 4) | rrc_nas_umts_key_ind->dl_nas_count;
        out_count[1] = context_ptr->nas_dl_cnt[1];
        out_count[2] = context_ptr->nas_dl_cnt[2];
        out_count[3] = 0;
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Debug out_count[0] = %d", out_count[0]);
      }
      else if((y_hash < rrc_nas_umts_key_ind->dl_nas_count) && (rrc_nas_umts_key_ind->dl_nas_count <= 15))
      {
        if (y1 == 0) /* y1-1 will underflow */
        {
          under_flow_cnt = ( (word )context_ptr->nas_dl_cnt[2] << 8) | context_ptr->nas_dl_cnt[1];
          under_flow_cnt--;
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Underflow detected, Decrement underflow count");
          out_count[0] = ((y1+15) << 4) | rrc_nas_umts_key_ind->dl_nas_count;
          /* Increment Overflow count */
          out_count[1] = (byte )(under_flow_cnt & 0xFF);
          out_count[2] = (byte )((under_flow_cnt & 0xFF00) >> 8);
          out_count[3] = 0;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Debug out_count[0] = %d", out_count[0]);
        }
        else /* y1-1 will not underflow */
        {
          out_count[0] = ((y1-1) << 4) | rrc_nas_umts_key_ind->dl_nas_count;
          out_count[1] = context_ptr->nas_dl_cnt[1];
          out_count[2] = context_ptr->nas_dl_cnt[2];
          out_count[3] = 0;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Debug out_count[0] = %d", out_count[0]);
        }
      }
    }
    context_ptr->nas_dl_cnt[0] = out_count[0];
    context_ptr->nas_dl_cnt[1] = out_count[1];
    context_ptr->nas_dl_cnt[2] = out_count[2];
    context_ptr->nas_dl_cnt[3] = out_count[3];
  }

  memscpy((void*)nas_dl_count,
           nas_dl_count_buf_len,
           (void*)&out_count[0],
           sizeof(out_count));

  if(mmgsdi_is_test_mode_logging_enabled_for_subscription((sys_modem_as_id_e_type)mm_sub_id))
  {
    MSG_HIGH_DS_6(MM_SUB, "=EMM= [emm_estimate_dl_count] out_count[0] = %d out_count[1] = %d out_count[2] out_count[3] = %d over_flow_cnt = %d, under_flow_cnt = %d", 
                          out_count[0],
                          out_count[1],
                          out_count[2],
                          out_count[3],
                          over_flow_cnt,
                          under_flow_cnt);
  }
} /* end of emm_estimate_dl_count() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */


#ifdef FEATURE_CIOT
/*===========================================================================

FUNCTION emm_nb1_validate_reg_accept_msg

DESCRIPTION
  This function returns true if UE have CP CIoT capability in attach/tau accept msg in NB1 rat
  else return false
  
DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_nb1_validate_reg_accept_msg
(
  lte_nas_emm_incoming_msg_type  *msg_ptr
)
{

  mm_check_for_null_ptr((void*)msg_ptr);
#ifndef FEATURE_CATM1_DISABLE

  if(emm_chk_nb1_rat()== FALSE) 
  {
    return TRUE;
  }
#endif
  switch (msg_ptr->hdr.msg_id)
  {
    case ATTACH_ACCEPT:
      if(msg_ptr->attach_accept.eps_nw_feature_support.cp_ciot_supported == FALSE)
      {
        lte_nas_emm_attach_rej_type  *attach_reject_ptr = NULL;
        /* Allocate memory */
        attach_reject_ptr = emm_modem_mem_alloc(sizeof(lte_nas_emm_attach_rej_type),emm_ctrl_data_ptr);
        mm_check_for_null_ptr((void*)attach_reject_ptr);
		MSG_ERROR_DS_0(MM_SUB, "=EMM= CP is not support in NB1 and treat as Attach reject with cause 15");
        attach_reject_ptr->lte_nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES;
        attach_reject_ptr->lte_nas_hdr.msg_id = ATTACH_REJECT;
        attach_reject_ptr->security_hdr = msg_ptr->attach_accept.security_hdr;
        attach_reject_ptr->emm_cause = LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA;
        emm_process_attach_reject(attach_reject_ptr,emm_ctrl_data_ptr);
        /* Free momery */
        emm_modem_mem_free(attach_reject_ptr, emm_ctrl_data_ptr);
        return FALSE;
      }
      break;
    case TRACKING_AREA_UPDATE_ACCEPT:
      if(msg_ptr->tau_accept.eps_nw_feature_support.cp_ciot_supported == FALSE)
      {
        lte_nas_emm_tau_rej *tau_reject_ptr = NULL;
        /* Allocate memory */
        tau_reject_ptr = emm_modem_mem_alloc(sizeof(lte_nas_emm_tau_rej),emm_ctrl_data_ptr);
        mm_check_for_null_ptr((void*)tau_reject_ptr);
		MSG_ERROR_DS_0(MM_SUB, "=EMM= CP is not support in NB1 and treat as TAU reject with cause 15");
        tau_reject_ptr->lte_nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES;
        tau_reject_ptr->lte_nas_hdr.msg_id = TRACKING_AREA_REJECT;
        tau_reject_ptr->security_hdr = msg_ptr->tau_accept.security_hdr;
        tau_reject_ptr->emm_cause = LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA;
        emm_process_tau_reject(tau_reject_ptr,emm_ctrl_data_ptr);
        /* Free momery */
        emm_modem_mem_free(tau_reject_ptr, emm_ctrl_data_ptr);
        return FALSE;
      }
      break;
    default:
      break;
  }

  return TRUE;
}
#endif

/*===========================================================================
FUNCTION    EMM_PROCESS_EMM_MESSAGE

DESCRIPTION
  This function processes the received EMM messages from NW if there is no 
  protocol state error. Otherwise, this function ignores the message and sends
  EMM STATUS message to NW. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_emm_message
(
  lte_nas_emm_incoming_msg_type     *incloming_message,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
)
{
  emm_state_type  emm_state = EMM_INVALID_STATE;
  boolean         protocol_state_error = FALSE;

  emm_state = EMM_GET_STATE();

  if((emm_state == EMM_NULL) || (emm_state == EMM_INVALID_STATE))
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Ignoring emm message in EMM NULL/INVALID state");
    return;
  }
  /* Adding DL data ind debugging here */
  emm_set_mm_msg_info_for_debug_buffer((byte)incloming_message->hdr.msg_id);
  emm_add_message_to_debug_buffer(LTE_RRC_DL_DATA_IND,TO_EMM,(sys_modem_as_id_e_type)mm_as_id);

  switch (incloming_message->hdr.msg_id)
  {
    case ATTACH_ACCEPT:
      if(emm_state == EMM_REGISTERED_INITIATED)
      {
        if(EMM_GET_SUBSTATE() == EMM_WAITING_FOR_NW_RESPONSE)
        {
#ifdef FEATURE_CIOT
          if(emm_nb1_validate_reg_accept_msg(incloming_message) == TRUE)
          {
#endif
            emm_process_attach_accept(&incloming_message->attach_accept, 
                                      emm_ctrl_data_ptr);
#ifdef FEATURE_CIOT
          }
#endif
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Ignoring dup ATTACH ACCEPT");
        }
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break ;
    case ATTACH_REJECT:
      if(emm_state == EMM_REGISTERED_INITIATED)
      {
        emm_process_attach_reject(&incloming_message->attach_rej, 
                                  emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break ;
     case DETACH_REQUEST:
#ifdef FEATURE_LTE_REL11
        /*Spec 24.301 5.2.2.4 abnormal case d)*/
        if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
           (emm_state == EMM_DEREGISTERED_INITIATED) &&
           (emm_ctrl_data_ptr->detach_reason == SWITCH_OFF) &&
           (nas_emm_power_off_detach_type  != INVALID_DETACH_TYPE))
       {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Detach Request discarded while switch off type MO detach ongoing");
       }
       else
#endif
      if(emm_state != EMM_DEREGISTERED)
      {
        emm_start_mt_detach(&incloming_message->mt_detach_req,
                            emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break;
    case DETACH_ACCEPT:
      if((emm_state == EMM_DEREGISTERED_INITIATED) ||
         ((emm_state == EMM_REGISTERED) && 
          (EMM_GET_SUBSTATE() == (emm_substate_type)EMM_REGISTERED_IMSI_DETACH_INITIATED)))
      {
        emm_process_detach_accept(&incloming_message->mt_detach_accept, 
                                  emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break;
    case TRACKING_AREA_UPDATE_ACCEPT:
      if(emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED)
      {
#ifdef FEATURE_CIOT
        if(emm_nb1_validate_reg_accept_msg(incloming_message) == TRUE)
        {
#endif
          /* Received TAU accept message. Process the accept message */
          emm_process_tau_accept(&incloming_message->tau_accept,
                                 emm_ctrl_data_ptr);
#ifdef FEATURE_CIOT
        }
#endif
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break ;
    case TRACKING_AREA_REJECT:
      if(emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED)
      {
        /* Received TAU reject message. Process the reject message */
        emm_process_tau_reject(&incloming_message->tau_rej, 
                               emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break ;
    case SERVICE_REJECT:
      if(emm_state == EMM_SERVICE_REQUEST_INITIATED)
      {
      	if (mm_get_ulog_handle() != NULL)
		{
        ULOG_RT_PRINTF_0(mm_get_ulog_handle(), "=EMM= SERVICE REJ Received");
	    }
        /* Received service reject message. Process the reject message */
        emm_process_reject_message(incloming_message, emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break;  
#ifdef FEATURE_CIOT
    case SERVICE_ACCEPT:
      if(emm_state == EMM_SERVICE_REQUEST_INITIATED ||
	     (emm_state == EMM_REGISTERED 
#ifdef FEATURE_LTE_REL14
	     && ((emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL14) ||
             (mm_timer_status[TIMER_T3449] != TIMER_STOPPED))
#endif
         )
	    )
      {
        /* Received service accept message. Process the message */
        emm_process_service_accept_message(&incloming_message->service_accept, emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break;
#endif
    case SECURITY_MODE_COMMAND:
      if (mm_get_ulog_handle() != NULL)
      {
        ULOG_RT_PRINTF_0(mm_get_ulog_handle(), "=EMM= SEC MODE COMMAND is received");
      }
      emm_process_security_mode_cmd(&incloming_message->security_mode_cmd, 
                                      emm_ctrl_data_ptr);

        emm_ctrl_data_ptr->curr_smc_status = LTE_SMC_STATUS_NONE;
      break;
    case EMM_AUTHENTICATION_REQUEST:
      emm_process_auth_request(&incloming_message->auth_req, 
                                 emm_ctrl_data_ptr); 
      break;
    case EMM_AUTHENTICATION_REJECT:
      emm_process_authentication_reject(emm_ctrl_data_ptr);
      break;
    case EMM_IDENTITY_REQUEST:
      emm_process_identity_request(&incloming_message->identity_req, 
                                     emm_ctrl_data_ptr);
      break;
    case GUTI_REALLOCATION_COMMAND:
      emm_process_guti_realloc_cmd(&incloming_message->guti_realloc_cmd, 
                                     emm_ctrl_data_ptr); 
      break;
    case EMM_INFORMATION:
      emm_process_emm_info(&incloming_message->emm_info, 
                             emm_ctrl_data_ptr);
      break;
    case EMM_STATUS:
      /* No need to do anything */
      MSG_HIGH_DS_1(MM_SUB, "=EMM= Rcved EMM_STATUS w/ cause %u",
                    incloming_message->mt_emm_status.emm_cause);
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      break;
    case DOWNLINK_NAS_TRANSPORT:
      if((emm_state == EMM_REGISTERED || 
         (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED && (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED))) 
         && ((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS)
#ifdef FEATURE_CIOT
             || (emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status == SYS_CP_SMS_STATUS_AVAILABLE)
#endif
         ))
      {
        emm_ctrl_data_ptr->emm_sms_active = TRUE;
#ifdef FEATURE_DUAL_SIM
        mm_check_other_mm_cnm_connections();
#endif  
        emm_process_dl_nas_transport(&incloming_message->dl_nas_transport);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break;
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
    case CS_SERV_NOTIFICATION:
      emm_process_cs_service_notification(&incloming_message->cs_serv_notification,emm_ctrl_data_ptr);
      break;
#endif
#if defined (FEATURE_LTE_REL9) 
#ifdef FEATURE_NAS_GPS_ENABLE
    case DL_GENERIC_NAS_TRANSPORT:
     if((emm_state == EMM_REGISTERED)&& 
     ((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY)||
      (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS)))
      {
        emm_process_dl_generic_nas_transport_msg(&incloming_message->dl_generic_nas_transport_msg,emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break;
#endif
#endif
    default:
      /* Should never happen - msg type check is done at caller function */
      MSG_ERROR_DS_1(MM_SUB,"=EMM= Illigal EMM Message ID - %d", 
                (byte)incloming_message->hdr.msg_id);
      break;
  } /* switch (incloming_message->message_id) */

  if(protocol_state_error == TRUE)
  {
    MSG_HIGH_DS_2(MM_SUB, "=EMM= Protocol state error - Rcved msg %u in EMM state %u",
                  incloming_message->hdr.msg_id,
                  emm_state);
    emm_send_emm_status(emm_ctrl_data_ptr,
                        LTE_NAS_MSG_TYPE_INCOMPATIBLE_WITH_PROTOCOL_STATE);
#ifdef TEST_FRAMEWORK
    #error code not present
#endif

  }

} /* end of emm_process_emm_message() */


/*===========================================================================
FUNCTION    EMM_FREE_DSM_MEMORY_IN_RRC_DATA_IND

DESCRIPTION
  This function frees the DSM memory in the received RRC data indication when
  EMM is not in EMM_CONNECTED_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_free_dsm_memory_in_rrc_data_ind
(
  lte_rrc_dl_data_ind_s *rrc_data_ind
)
{
  uint8  num_attach = 0;
  uint32 dsm_packet_length = 0;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */

  num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)rrc_data_ind);

  if ( num_attach == 1 )
  {
    att_ptr = msgr_get_attach((msgr_hdr_struct_type *)rrc_data_ind, 0);
    msgr_get_dsm_attach(att_ptr, &dsm_ptr);
    mm_check_for_null_ptr((void *)dsm_ptr);

    dsm_packet_length               = dsm_length_packet(dsm_ptr);
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Freeing DSM memory! item len %d", dsm_packet_length);
    
    /* free the dsm memory */
    dsm_free_packet(&dsm_ptr);
    nas_assert_check((boolean)(dsm_ptr == NULL));
  }
  else
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= DSM DL not found. RRC using old interface",0,0,0);
  }
}

#ifdef TEST_FRAMEWORK
 #error code not present
#endif // TEST_FRAMEWORK

boolean emm_chk_resize_needed_msg_id(byte msg_id)
{
  switch (msg_id) 
  {
#ifdef FEATURE_CIOT
    case ESM_DATA_TRANSPORT:
#endif
    case PDN_CONNECTIVITY_REJECT:
    case PDN_DISCONNECT_REJECT:
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
    case DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST:
    case MODIFY_EPS_BEARER_CONTEXT_REQUEST:
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST:
    case BEARER_RESOURCE_ALLOCATION_REJECT:
    case BEARER_RESOURCE_MODIFICATION_REJECT:
     return TRUE;
    default:
     return FALSE;
  }
} /* End emm_chk_resize_needed_msg_id() */

/*===========================================================================
FUNCTION    EMM_RESIZE_MEM_FOR_DL_OTA_MSG_PTR

DESCRIPTION
  This function resizes memory allocated for OTA buffer for certain ESM messages
  received from the NW
 
DEPENDENCIES
  None

RETURN VALUE
 NONE

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_resize_mem_for_dl_ota_msg_ptr
( 
  byte                    *src,
  uint32                  msg_length,
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  byte sec_hdr = 0;
  byte  *msg_tmp_ptr;
  boolean result = FALSE;
  dword nas_count = 0;
  lte_nas_dl_security_info_type dl_security_info;
  byte *decipherable_ota_msg;
  byte plain_nas_msg_offset = 6;
  uint32 actual_buffer_length =0;
  byte msg_id = 0;

  memset(&dl_security_info,0,sizeof(lte_nas_dl_security_info_type));

  ASSERT(src != NULL) ; 
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  sec_hdr = (byte)((src[0] & 0xF0) >> 4);

  if ((sec_hdr == INTEGRITY_PROTECT_CIPHER) || (sec_hdr == INTEGRITY_PROTECT))
  {
    /* Allocate memory for ota message*/
    msg_tmp_ptr = (byte *)emm_modem_mem_alloc(sizeof(byte)*msg_length,emm_ctrl_data_ptr);

    mm_check_for_null_ptr((void *) msg_tmp_ptr);

    memscpy(msg_tmp_ptr,msg_length,src,msg_length);

    decipherable_ota_msg = msg_tmp_ptr + plain_nas_msg_offset;

    msg_id = (byte)((decipherable_ota_msg[2] & 0xFF));

    if (sec_hdr == INTEGRITY_PROTECT_CIPHER)
    {
      read_security_context_only = TRUE;//enable only read dl security info
      emm_get_dl_security_info(emm_ctrl_data_ptr, &dl_security_info,FALSE,msg_tmp_ptr);
      read_security_context_only = FALSE;//reset it back to FALSE
      actual_buffer_length = msg_length - plain_nas_msg_offset;
      nas_count = (( dword) dl_security_info.nas_count[2] << 16  ) | ( ( dword) dl_security_info.nas_count[1] << 8 ) | ( dword) dl_security_info.nas_count[0] ;
      lte_security_stream_decipher( dl_security_info.security_alg.ciphering_alg, 
                                    dl_security_info.security_alg.ciphering_alg_key, 
                                    decipherable_ota_msg,/*Input buffer address*/
                                    (uint16)actual_buffer_length, 
                                    decipherable_ota_msg,/*output buffer address*/
                                    dl_security_info.bearer,
                                    nas_count);
    }
    if (((byte)(decipherable_ota_msg[0] & 0x0F) == EPS_SESSION_MANAGEMENT_MESSAGES) && 
        (emm_chk_resize_needed_msg_id(msg_id) == TRUE))
    {
      result = TRUE;
    }
    /* Free memory */
    emm_modem_mem_free(msg_tmp_ptr, emm_ctrl_data_ptr);
  }
  return result;
}


/*===========================================================================
FUNCTION    EMM_PROCESS_RRC_DATA_INDICATION

DESCRIPTION
  This function processes RRC data indication. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -e550 Symbol 'sec_params' not accessed */
void emm_process_rrc_data_indication
(
  lte_rrc_dl_data_ind_s *rrc_data_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  // jalokia lte_nas_incoming_msg_type incloming_message_ptr ;
  lte_nas_incoming_msg_type *incloming_message_ptr = NULL;
  lte_nas_dl_security_info_type dl_security_info;
  byte message_counter = 0 ;
  /*lte_nas_protocol_discriminator protocol_descriminator = RESERVED ; */
  lte_nas_protocol_discriminator protocol_descriminator  ; 
  byte message_index = 0 ;
  boolean continue_processing = TRUE;
  boolean attach_accept_successful = TRUE;

  uint8  num_attach = 0;
  uint32 dsm_packet_length;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
  byte *emm_temp_ota_msg_ptr;
  word temp_ota_msg_size;
  
  byte  sec_hdr;

  emm_security_context_info *context_ptr = NULL;
  byte  prev_dl_cnt[4]; 
  boolean is_smc_msg = FALSE;  
  
#ifdef FEATURE_CIOT
  byte  *emm_ota_message_tmp_ptr = NULL;
  emm_failure_type                emm_failure_cause;
#endif

  if(EMM_GET_CONNECTION_STATE() != EMM_CONNECTED_STATE)
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Ignoring RRC DL DATA IND rcved in conn state %d",
                   EMM_GET_CONNECTION_STATE());
    emm_free_dsm_memory_in_rrc_data_ind(rrc_data_ind);
    return;
  }

  memset(&dl_security_info,0,sizeof(lte_nas_dl_security_info_type));
  memset((void *)prev_dl_cnt, 0, sizeof(byte)*4); 
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  if(emm_ctrl_data_ptr->emm_ota_message_ptr != NULL)
  {
    memset(emm_ctrl_data_ptr->emm_ota_message_ptr, 0, EMM_MAX_MESSAGE_LENGTH);
  
    /* See if NAS has sent the DT message as a DSM item by checking for attachments */
    num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)rrc_data_ind);
  
    if ( num_attach == 1 )
    {
      att_ptr = msgr_get_attach((msgr_hdr_struct_type *)rrc_data_ind, 0);
      msgr_get_dsm_attach(att_ptr, &dsm_ptr);
      mm_check_for_null_ptr((void *)dsm_ptr);
  
      dsm_packet_length               = dsm_length_packet(dsm_ptr);
      MSG_HIGH_DS_1(MM_SUB, "=EMM= DSM DL Data found! item len %d", dsm_packet_length);
      
      if( dsm_packet_length <= EMM_MAX_MESSAGE_LENGTH )
      {
      
        /* store the RRC message payload, for SMC processing */
        emm_ctrl_data_ptr->ota_msg_size = (word)dsm_packet_length;
  
        emm_ctrl_data_ptr->ota_msg_size = dsm_pullup(&dsm_ptr, 
                                                     emm_ctrl_data_ptr->emm_ota_message_ptr, 
                                                     emm_ctrl_data_ptr->ota_msg_size);
      }
      else
      {
  #ifdef FEATURE_CIOT
        if( emm_cp_opt_active( emm_ctrl_data_ptr )== TRUE )
        {
          /* Allocate memory for tmp ota message*/
          emm_ota_message_tmp_ptr = (byte *)emm_modem_mem_alloc(sizeof(byte)*dsm_packet_length,emm_ctrl_data_ptr);
  
          mm_check_for_null_ptr((void *) emm_ota_message_tmp_ptr);
  
          dsm_packet_length  = dsm_pullup(&dsm_ptr, 
                                          emm_ota_message_tmp_ptr,
                                          (uint16)dsm_packet_length);
  
          if((dsm_packet_length <= EMM_MAX_ESM_DATA_LENGTH) && 
             (emm_resize_mem_for_dl_ota_msg_ptr(emm_ota_message_tmp_ptr,dsm_packet_length,emm_ctrl_data_ptr) == TRUE))
          {
            //Resize msg ptr for small data over CP
            if(emm_ctrl_data_ptr->emm_ota_message_ptr != NULL)
            {
              emm_mem_free(emm_ctrl_data_ptr->emm_ota_message_ptr,emm_ctrl_data_ptr);
              emm_ctrl_data_ptr->emm_ota_message_ptr = NULL;
            }
  
            emm_ctrl_data_ptr->emm_ota_message_ptr_flipped = TRUE;
  
            emm_ctrl_data_ptr->emm_ota_message_ptr = (byte *)emm_mem_alloc(sizeof(byte)*dsm_packet_length,
                                                     emm_ctrl_data_ptr);
            mm_check_for_null_ptr((void *)emm_ctrl_data_ptr->emm_ota_message_ptr) ;										   
            emm_ctrl_data_ptr->ota_msg_size = (word) dsm_packet_length;
  
            MSG_HIGH_DS_1(MM_SUB, "=EMM= Resize emm_ota_message_ptr msg to len %d", dsm_packet_length);
          }
          else
          {
            emm_ctrl_data_ptr->ota_msg_size = EMM_MAX_MESSAGE_LENGTH;
            MSG_ERROR_DS_3(MM_SUB, "=EMM= DSM item length %d greater than %d bytes, truncating to max read %d bytes",
                       dsm_packet_length,
                       EMM_MAX_MESSAGE_LENGTH,
                       emm_ctrl_data_ptr->ota_msg_size);
          }
  
          memscpy(emm_ctrl_data_ptr->emm_ota_message_ptr,
                  emm_ctrl_data_ptr->ota_msg_size,
                  emm_ota_message_tmp_ptr,
                  emm_ctrl_data_ptr->ota_msg_size);
  
          /* Free memory of tmp ptr*/
          emm_modem_mem_free(emm_ota_message_tmp_ptr, emm_ctrl_data_ptr);
        }
        else
  #endif
        {
          emm_ctrl_data_ptr->ota_msg_size = EMM_MAX_MESSAGE_LENGTH;
          emm_ctrl_data_ptr->ota_msg_size = dsm_pullup(&dsm_ptr, 
                                                     emm_ctrl_data_ptr->emm_ota_message_ptr, 
                                                     emm_ctrl_data_ptr->ota_msg_size);
  
          MSG_ERROR_DS_3(MM_SUB, "=EMM= DSM item length %d greater than %d bytes, truncating to max read %d bytes",
                       dsm_packet_length,
                       EMM_MAX_MESSAGE_LENGTH,
                       emm_ctrl_data_ptr->ota_msg_size);
          /* free the additional memory greater than EMM_MAX_MSG_LEN */
          dsm_free_packet(&dsm_ptr);
        }
      }
      /* Received dsm should have been freed */
      nas_assert_check((boolean)(dsm_ptr == NULL));
    }
    else
    {
      MSG_FATAL_DS(MM_SUB,"=EMM= DSM DL not found. RRC using old interface",0,0,0);
    }
  
    sec_hdr = (byte)((emm_ctrl_data_ptr->emm_ota_message_ptr[0] & 0xF0) >> 4);
    if(sec_hdr == INTEGRITY_PROTECT_SECURITY_MODE_CMD)
    { 
      is_smc_msg = TRUE;
    }
  
    dl_sqn_for_current_msg = 0;
  
    /* get the DL SQN from the OTA message */
    get_dl_nas_sqn((byte *)emm_ctrl_data_ptr->emm_ota_message_ptr,
                     &dl_sqn_for_current_msg);
             
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Rcved DL MSG w/ SQN %d", dl_sqn_for_current_msg);
    
    /* get the current context */
    context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );
  
    if( context_ptr != NULL )
    {
      /* Save the current nas CL connt in case the rcved msg is invalid. */
      memscpy((void*)&prev_dl_cnt[0], 
              sizeof(prev_dl_cnt),
              (void*)&context_ptr->nas_dl_cnt[0], 
              sizeof(context_ptr->nas_dl_cnt)); 
  
      MSG_HIGH_DS_2(MM_SUB, "=EMM= Current context of type %d, KSI %d ",
                    context_ptr->context_type,
                    context_ptr->nasKSI.keyset_id);
    }
  
    message_counter = decode_nas_msg_fp((byte *)emm_ctrl_data_ptr->emm_ota_message_ptr, 
                                        (word)emm_ctrl_data_ptr->ota_msg_size,
                                        emm_get_dl_security_info(emm_ctrl_data_ptr, 
                                                                 &dl_security_info,
                                                                 is_smc_msg,
                                                                 emm_ctrl_data_ptr->emm_ota_message_ptr),
                                                                 emm_ctrl_data_ptr,
                                                                 emm_ctrl_data_ptr->nas_incoming_msg_ptr);
  }
  else
  {
    return;
  }

#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */

  if(message_counter == 0)
  {
    if( context_ptr != NULL )
    {
       MSG_HIGH_DS_0(MM_SUB, "=EMM= DL MSG dropped, rolling back NAS DL CNT");
       memscpy((void*)&context_ptr->nas_dl_cnt[0],
               sizeof(context_ptr->nas_dl_cnt),
               (void*)&prev_dl_cnt[0],
               sizeof(prev_dl_cnt));
       emm_ctrl_data_ptr->last_incoming_sqn = prev_dl_cnt[0];
#ifdef TEST_FRAMEWORK
       #error code not present
#endif
    }
#ifdef TEST_FRAMEWORK
    #error code not present
#endif
    return;
  }

  incloming_message_ptr =(lte_nas_incoming_msg_type*) emm_modem_mem_alloc(
                         sizeof(lte_nas_incoming_msg_type),
                         emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void *)incloming_message_ptr) ;

  for (message_index=0; message_index<message_counter; message_index++)
  {
    protocol_descriminator = nas_retrieve_msg_fp(message_index, incloming_message_ptr) ;

    /* save the security context information */
    if( context_ptr != NULL )
    {
      /*
      MME may re-establish the secure exchange of NAS messages:
      - by replying with a NAS message that is integrity protected and ciphered 
        using the current EPS security context.
      */
      (void)get_nas_sec_info(&sec_hdr);
      if( sec_hdr == INTEGRITY_PROTECT_CIPHER ||
          sec_hdr == INTEGRITY_PROTECT )
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Rx INTEGRITY_PROTECT msg %d, set nas_secure_exchange to TRUE",
                      sec_hdr);
        emm_ctrl_data_ptr->nas_secure_exchange = TRUE;
      }
    }

#ifdef FEATURE_CIOT
   if((emm_cp_opt_active(emm_ctrl_data_ptr) == TRUE) &&
      (EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED) &&
      ((esm_get_module_from_trans_id(emm_ctrl_data_ptr->esm_srv_req_trans_id) == (unsigned)LTE_ESM_EDT) ||
       (emm_ctrl_data_ptr->esm_srv_req_trans_id == EMM_INVALID_ESM_SRV_REQ_TRANS_ID)||
       (emm_ctrl_data_ptr->service_req_cause == LTE_RRC_EST_CAUSE_MAX))&&
      (emm_chk_complete_cpsr_by_security_msg(sec_hdr,incloming_message_ptr->nas_hdr.msg_id) == TRUE))
   {
      /* spec 24.301 5.6.1.4.2 case m)if the MME does not accept the request, the MME shall:
         -send an ESM DATA TRANSPORT message to the UE, if downlink user data is pending to be delivered via the control plane;
         -send a signalling message to the UE if downlink signalling is pending;or 
         -send a SERVICE ACCEPT message to complete the service request procedure.*/
      if(emm_ctrl_data_ptr->service_req_cause == LTE_RRC_EST_CAUSE_MAX)
      {
        emm_failure_cause.cause_type = LTE_NAS_IRAT_T3417_EXPIRY;
        emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                     LTE_NAS_NO_FAILURE,
                                     emm_ctrl_data_ptr->esm_srv_req_trans_id,
                                     emm_failure_cause);
      }
       //conclude CPSR with service accept except for S1_U switch from idle
      emm_complete_service_request(emm_ctrl_data_ptr);
#ifdef FEATURE_LTE_REL14
      if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL14) && (is_smc_msg == TRUE))
      {
        mm_start_timer(TIMER_T3449,DEFAULT_TIMEOUT); 
      }
#endif
	  if (mm_get_ulog_handle() != NULL)
	  {
			ULOG_RT_PRINTF_0(mm_get_ulog_handle(), "=EMM= Complete ESR");
	  }
      emm_temp_ota_msg_ptr = emm_modem_mem_alloc(emm_ctrl_data_ptr->ota_msg_size,emm_ctrl_data_ptr);
	  mm_check_for_null_ptr((void *)emm_temp_ota_msg_ptr);
	  temp_ota_msg_size = emm_ctrl_data_ptr->ota_msg_size;

      memscpy(emm_temp_ota_msg_ptr,temp_ota_msg_size,emm_ctrl_data_ptr->emm_ota_message_ptr,emm_ctrl_data_ptr->ota_msg_size);
	      /* Send out all pended messages */ 
      emm_send_all_pended_messages(emm_ctrl_data_ptr);
	  emm_ctrl_data_ptr->ota_msg_size =  temp_ota_msg_size;

	  memscpy(emm_ctrl_data_ptr->emm_ota_message_ptr,emm_ctrl_data_ptr->ota_msg_size,emm_temp_ota_msg_ptr,temp_ota_msg_size);	                     
	  	  
	  emm_modem_mem_free(emm_temp_ota_msg_ptr,
                       emm_ctrl_data_ptr);

	  emm_temp_ota_msg_ptr = NULL;
	  if (emm_ctrl_data_ptr->esm_data_pending_after_res == TRUE)
	  {	  
	    /* All pended ESM data packets msut have been sent out above, reset the flag */
	    emm_ctrl_data_ptr->esm_data_pending_after_res = FALSE;
	  }

    }
#endif

    switch (protocol_descriminator)
    {
      case EPS_MOBILITY_MANAGEMENT_MESSAGES:
        /* Check for valid flag only for EMM messages*/
        if(incloming_message_ptr->nas_hdr.valid)
        {
          emm_process_emm_message(&incloming_message_ptr->emm_incoming_msg, emm_ctrl_data_ptr) ;
        }
        else
        {
          attach_accept_successful = FALSE;
          MSG_HIGH_DS_2(MM_SUB, "=EMM= Invalid EMM msg %u - Cause %u",
                        incloming_message_ptr->nas_hdr.msg_id,
                        incloming_message_ptr->nas_hdr.emm_decode_fail_reason);
          emm_send_emm_status(emm_ctrl_data_ptr, 
                              incloming_message_ptr->nas_hdr.emm_decode_fail_reason);
#ifdef TEST_FRAMEWORK
        #error code not present
#endif
        }
        break ;
      case EPS_SESSION_MANAGEMENT_MESSAGES:
        if(EMM_GET_STATE() == EMM_REGISTERED_INITIATED)
        {
          /* Here the assumption is the MSG_LIB would always say there are 2 messages when processing ATTACH ACCEPT.
             So, in this case if attach accept came with any message other than ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST,
             EMM considers the attach accept as invalid and starts mo detach. 
          */
          /*If EMM-REGISTERED without PDN connection is supported by the UE and the MME, 
		        an ESM DUMMY MESSAGE is included in the ESM message container information element of the ATTACH REQUEST message and 
			the UE receives the ATTACH ACCEPT message combined with a PDN CONNECTIVITY REJECT message, 
			the UE shall send an ATTACH COMPLETE message together with an ESM DUMMY MESSAGE contained in the ESM message container information element to the network. 
			Further UE behaviour is implementation specific.
	           */
          if(incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id != ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST 
#ifdef FEATURE_CIOT
            && (incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id != ESM_DUMMY_MESSAGE &&
                incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id != PDN_CONNECTIVITY_REJECT)
#endif
           )
          {
            if((EMM_GET_SUBSTATE() != (emm_substate_type)EMM_WAITING_FOR_ESM_RESPONSE))
            {
              if(incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id != ESM_INFORMATION_REQUEST)
              {
                MSG_ERROR_DS_1(MM_SUB, "=EMM= ATTACH ACCEPT must carry ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST. Received msg id=%d,discarding this message",
                               incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id);
				if ((incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id == ESM_DATA_TRANSPORT) 
                  && (incloming_message_ptr->esm_incoming_msg.esm_data_transport.dsm_ptr != NULL))
                  {
                    dsm_free_packet((dsm_item_type**)&incloming_message_ptr->esm_incoming_msg.esm_data_transport.dsm_ptr);
                    incloming_message_ptr->esm_incoming_msg.esm_data_transport.dsm_ptr = NULL;
                  }
                continue_processing = FALSE;
#ifdef TEST_FRAMEWORK
                #error code not present
#endif /* TEST_FRAMEWORK */
              }            
              else if(emm_ctrl_data_ptr->nas_secure_exchange == FALSE)
              {
                MSG_ERROR_DS_0(MM_SUB, "=EMM= ESM_INFORMATION_REQUEST received before secure exchange of NAS messages is established, discarding this message");
                continue_processing = FALSE;
              }
            }
          }
          else
          {
            if(message_counter != 2)
            {
              MSG_ERROR_DS_0(MM_SUB, "=EMM= ESM message received without ATTACH ACCEPT, discarding this message\n");
              continue_processing = FALSE;
#ifdef TEST_FRAMEWORK
              #error code not present
#endif /* TEST_FRAMEWORK */
            }
            else
            {
              if((attach_accept_successful == TRUE) && 
                 (EMM_GET_STATE() == EMM_REGISTERED_INITIATED) &&
                 (EMM_GET_SUBSTATE() == (emm_substate_type)EMM_WAITING_FOR_NW_RESPONSE))
              {
                /* MOVING to EMM_WAITING_FOR_ESM_RESPONSE before sending the message to ESM*/
                EMM_MOVES_TO_REGISTERED_INITIATED_STATE((emm_substate_type)EMM_WAITING_FOR_ESM_RESPONSE) ;

                /* ESM will release all resources allocated for currently cached 
                   PDN_CONNECTIVITY_REQ upon receipt of ACTIVATE DEFAULT EPS 
                   BEARER CONTEXT REQUEST. 
                   Discard currently cached PDN_CONNECTIVITY_REQ since it is no 
                   longer reusable */
                emm_ctrl_data_ptr->pdn_conn_req_trans_id = 
                                               EMM_INVALID_PDN_CONN_REQ_TRANS_ID;
                /* DO NOT Check for valid flag. Let ESM handle that*/
                emm_process_incoming_esm_message(&incloming_message_ptr->esm_incoming_msg, 
                                                 emm_ctrl_data_ptr) ;
#ifdef TEST_FRAMEWORK
                #error code not present
#endif /* TEST_FRAMEWORK */

                continue_processing = FALSE;
              }
              else
              {
                MSG_ERROR_DS_0(MM_SUB, "=EMM= Dropping ACT DEF BC REQ - Invalid ATTACH ACCEPT");
                continue_processing = FALSE; 
              }
            }
          }
        }
        else if(EMM_GET_STATE() == EMM_DEREGISTERED)
        {
          /*For only Attach reject with #19 we delete the cached PDN..need to send 
            PDN Rej to ESM for other attach reject causes no need to process ESM mesage*/
          if(incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id != PDN_CONNECTIVITY_REJECT || 
             emm_ctrl_data_ptr->pdn_conn_req_trans_id != EMM_INVALID_PDN_CONN_REQ_TRANS_ID)
          {
            /*Block unnecessary ESM messages being sent to ESM*/
            MSG_HIGH_DS_1(MM_SUB, "=EMM= do not send message to ESM msg_id %d",
                          incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id);
            continue_processing = FALSE;  
#ifdef TEST_FRAMEWORK
            #error code not present
#endif /* TEST_FRAMEWORK */
          }
        }
        if((EMM_GET_STATE() != EMM_DEREGISTERED_INITIATED) &&
           (continue_processing == TRUE))
        {
          /* DO NOT Check for valid flag. Let ESM handle that*/
          emm_process_incoming_esm_message(&incloming_message_ptr->esm_incoming_msg, emm_ctrl_data_ptr) ;
#ifdef TEST_FRAMEWORK
          #error code not present
#endif /* TEST_FRAMEWORK */
        }
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
        break ;
      case RESERVED_FOR_TESTS_PROCEDURES:    
        if(incloming_message_ptr->nas_hdr.valid)
        {
          /*Pass the PLAIN TLB message pointer and length of the message to emm_process_incoming_tlb_msg block*/
          emm_process_incoming_tlb_msg((byte *)incloming_message_ptr->nas_hdr.ota_msg_ptr, 
                                    (word)incloming_message_ptr->nas_hdr.ota_msg_len);
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB, "=EMM= security procedure failed for 'RESERVED_FOR_TESTS_PROCEDURES', discarding this message");
#ifdef TEST_FRAMEWORK
          #error code not present
#endif /* TEST_FRAMEWORK */
        }
        break;
      default:      
        /* Here the assumption is the MSG_LIB would always say there are 2 messages when processing ATTACH ACCEPT.
           So, in this case if attach accept did not come with any ESM message,
           EMM considers the attach accept as invalid and starts mo detach.
        */        
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Received message with unknown protocol discriminator %d, discarding this message",
                       incloming_message_ptr->emm_incoming_msg.hdr.pd);
        break ;
    } /* switch (protocol_descriminator) */
  } /* for (message_index=0; message_index<message_counter; message_counter++) */
//jalokia
  emm_modem_mem_free(incloming_message_ptr, emm_ctrl_data_ptr);
#ifdef TEST_FRAMEWORK
  #error code not present
#endif

} /* end of emm_rrc_process_data_indication() */

/*lint -restore */
/*===========================================================================

FUNCTION    EMM_PROCESS_RRC_UL_DATA_CNF

DESCRIPTION
  This function processes RRC_UL_DATA_CNF primitive


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_rrc_ul_data_cnf
(
  lte_rrc_ul_data_cnf_s *rrc_ul_data_cnf,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  emm_pended_msg_list_type *found_message = (emm_pended_msg_list_type *)NULL;
  boolean                   perform_tau   = FALSE ;
  word                      status_index  = 0 ;
  emm_failure_type          emm_failure_cause;

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  for (status_index=0; status_index<rrc_ul_data_cnf->num_trans_ids; status_index++)
  {
    MSG_HIGH_DS_2(MM_SUB, "=EMM= UL DATA CNF - Msg 0x%x, status %d",
                  rrc_ul_data_cnf->trans_id[status_index],
                  rrc_ul_data_cnf->status);

    found_message = emm_find_pended_message(rrc_ul_data_cnf->trans_id[status_index], emm_ctrl_data_ptr) ;

    if (found_message != (emm_pended_msg_list_type *)NULL)
    {
      if (rrc_ul_data_cnf->status != LTE_RRC_UL_DATA_CNF_SUCCESS)
      {
        if (found_message->msg_tx_failure_callback != NULL)
        {  
            emm_failure_cause.cause_type       = LTE_NAS_IRAT_UL_MSG_CNF_FAILURE;
            emm_failure_cause.cause.cnf_status = rrc_ul_data_cnf->status;
            if (TRUE == found_message->msg_tx_failure_callback(emm_translate_ul_data_cnf_status(rrc_ul_data_cnf->status), 
                                                               rrc_ul_data_cnf->trans_id[status_index],
                                                               emm_failure_cause,
                                                               emm_convert_ta(rrc_ul_data_cnf->ta), 
                                                               found_message,
                                                               emm_ctrl_data_ptr))
            {
              perform_tau = TRUE ;
            }
        }
      }
      else
      {
        if (status_index > 1)
        {
          MSG_ERROR_DS_0(MM_SUB, "=EMM= Violation of the protocol, cancel lte_rrc_ul_data_cnf processing");
          break ;
        }

        found_message->message_sent_indicator = TRUE ;

        if (found_message->msg_delivery_callback != NULL)
        {
          found_message->msg_delivery_callback(rrc_ul_data_cnf->trans_id[status_index]) ;
        }
      }

      if (found_message->message_sent_indicator == TRUE)
      {
        emm_delete_pended_message(found_message, emm_ctrl_data_ptr) ;
      }
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Transaction ID (%u) has not been found",
                     rrc_ul_data_cnf->trans_id[status_index]);
    }
  }
  
#if defined  FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
#endif 

  if (perform_tau)
  {
    /* Initiate TAU */
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
    emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                               emm_get_eps_update_type(emm_ctrl_data_ptr),
                               FALSE, 
                               emm_ctrl_data_ptr);
  }
} /* end of emm_process_rrc_ul_data_cnf() */

/*===========================================================================

FUNCTION  EMM_PROCESS_RRC_DEACTIVATE_CNF

DESCRIPTION
  This function processes RRC_UL_DEACTIVATE_CNF primitive

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_rrc_deactivate_cnf
(
  lte_rrc_deactivate_cnf_s *rrc_deactivate_cnf,
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{
  emm_security_context_info  *context_ptr = NULL;
  emm_security_context_info  *current_context_ptr = NULL;
  sys_stop_mode_reason_e_type save_temp_stop_mode_reason;
  emm_failure_type            emm_failure_cause;

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Rcved RRC DEACT CNF - %d", rrc_deactivate_cnf->status);
  
  if (emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == FALSE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Ignoring RRC DEACT CNF"); 
	return;
  }
  
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

#ifdef FEATURE_NAS_GW
  /* Set Ignore to FALSE. GW will check this flag before proceeding to act on the activation_ind */
  emm_ctrl_data_ptr->ignore_GW_activation_ind = FALSE;
#endif
  save_temp_stop_mode_reason = emm_ctrl_data_ptr->rrc_deact_req.deact_reason;

#ifdef FEATURE_LTE_REL10
  /* this flag could be set if Attach/TAU/SR/ESR is triggered due to paging.
     reset the flag */
  emm_ctrl_data_ptr->responding_to_page = FALSE;
#endif
#ifdef FEATURE_CIOT
   emm_ctrl_data_ptr->emm_start_mo_excp_signalling = FALSE;
#endif


  /* Stop T3324 and reset the flag psm_set_cfg_req_pending when lte is deactivated */

  if ((emm_ctrl_data_ptr->rrc_deact_req.deact_reason != SYS_STOP_MODE_REASON_DEEP_SLEEP) &&
      (emm_ctrl_data_ptr->rrc_deact_req.deact_reason != SYS_STOP_MODE_REASON_ALT_SCAN))
  {
    /* Reset the flag edrx_set_cfg_req_status when lte is deactivated */
    emm_ctrl_data_ptr->edrx_set_cfg_req_status = NAS_EDRX_CFG_REQ_NONE;
    *emm_ctrl_data_ptr->psm_set_cfg_status_ptr = NAS_PSM_CFG_REQ_NONE;
    *emm_ctrl_data_ptr->allow_psm_in_oos_ltd_srv_ptr = FALSE;
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
	if(reg_state_is_stack_suspended_get())
	{
	   MSG_HIGH_DS_0(MM_SUB, "=EMM= Stack is suspended, not stopping t3324");
	}
	else
#endif
	{
       mm_stop_timer(TIMER_T3324);
	}
    emm_ctrl_data_ptr->psm_active = FALSE;
  }

  if(emm_ctrl_data_ptr->emm_ph_status_change_cnf_pending == TRUE)
  {
    emm_ctrl_data_ptr->emm_ph_status_change_cnf_pending = FALSE;

    emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending = FALSE;
#ifdef FEATURE_NAS_GW
    emm_ctrl_data_ptr->ignore_GW_activation_ind = TRUE;
#endif
    mm_send_mmr_ph_status_change_cnf();
    return;
  }

  if(emm_ctrl_data_ptr->ongoing_detach_procedure == TRUE)
  {
    emm_ps_detach_clean_up(emm_ctrl_data_ptr);
  }

  if((emm_ctrl_data_ptr->rrc_deact_req.deact_reason != 
                                                 SYS_STOP_MODE_REASON_ALT_SCAN)
#ifdef FEATURE_DUAL_SIM
     && (emm_ctrl_data_ptr->rrc_deact_req.deact_reason != 
                                              SYS_STOP_MODE_REASON_DSDS_SUSPEND)
#endif
#if defined FEATURE_SGLTE || defined FEATURE_GPS_WWAN_PRIORITY_SEARCH
     && (emm_ctrl_data_ptr->rrc_deact_req.deact_reason != 
                                                 SYS_STOP_MODE_REASON_SUSPEND_STACK) 
#endif
    && (emm_ctrl_data_ptr->rrc_deact_req.deact_reason != 
                                                 SYS_STOP_MODE_REASON_DUAL_SWITCH)
    && (emm_ctrl_data_ptr->rrc_deact_req.deact_reason != 
                                                 SYS_STOP_MODE_REASON_DEEP_SLEEP)
     )
  {
    if (emm_ctrl_data_ptr->rrc_deact_req.deact_reason != 
                                                 SYS_STOP_MODE_REASON_PSM)
    {
      /* save the NAS security context info in emm ctrl data to the USIM  or EFS */
      current_context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr,
                                               CONTEXT_STATE_CURRENT);
      if( current_context_ptr != NULL )
      {
        context_ptr = emm_get_context_by_type (emm_ctrl_data_ptr, 
                                               CONTEXT_TYPE_FULL_NATIVE_EPS);

        /* if there are only mapped contexts at this time, do not save it to USIM or EFS */
        if( context_ptr != NULL )
        {
          if (emm_ctrl_data_ptr->eps_storage_supported)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= eps_storage_supported supported, update context in USIM");
            emm_update_usim_eps_security_context(context_ptr);
          }
          else
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= eps_storage_supported NOT supported, update context in EFS");
            nas_assert_check( (boolean)(emm_write_efs_security_context(context_ptr) == 1 ));
          }
        }  
      }
      else
      {
        /* If there is no current EPS context.
                 Any native EPS NAS security context stored in UICC or EFS is marked as invalid at power off 
            */
        if( emm_ctrl_data_ptr->detach_reason == SWITCH_OFF )
             emm_update_stored_security_context(emm_ctrl_data_ptr, TRUE);

      }
    }

    if(emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                   SYS_SIM_STATE_NOT_AVAILABLE)
    {
      /* Invalidate GUTI */
      emm_ctrl_data_ptr->emm_guti_valid = FALSE;
      memset((void *)&emm_ctrl_data_ptr->emm_guti, 
             0xFF, 
             sizeof(lte_nas_emm_guti_type));

      /* Invalidate IMSI */
      memset((void *)&emm_ctrl_data_ptr->imsi, 0xFF, sizeof(lte_rrc_imsi_s));
    }

    switch(emm_ctrl_data_ptr->rrc_deact_req.deact_reason)
    {
      case SYS_STOP_MODE_REASON_DEEP_SLEEP:
#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE
      case SYS_STOP_MODE_REASON_UE_MODE_CHANGE:
#endif
		if(emm_ctrl_data_ptr->rrc_deact_req.deact_reason == 
		                     SYS_STOP_MODE_REASON_DEEP_SLEEP)
		{
		  /*Send NO_SRV to GSTK*/
          emm_send_no_svc_to_gstk();
        }
        /* Move EMM connection state to IDLE*/
        EMM_MOVES_TO_IDLE_STATE();
        /* Move EMM mobility state */
        if(EMM_GET_STATE() == EMM_DEREGISTERED)
        {
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
        }
        else
        {
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
        }   
        break;
      case SYS_STOP_MODE_REASON_POWER_OFF:
      case SYS_STOP_MODE_REASON_PSM:
        /* Reset as needed. Only Some parameters will need to be reset and other 
           retained to be used on coming back to LTE upon inter-RAT procedures */
        emm_reset_ctrl_data(emm_ctrl_data_ptr, TRUE);
		mm_stop_timer(TIMER_T3247);
        mm_reset_global_variables();
#ifdef FEATURE_NAS_GW
        gmm_reset_global_variables();
#endif
        EMM_MOVES_TO_NULL_STATE();
        mm_last_active_rat_stopped = SYS_RAT_LTE_RADIO_ACCESS;
        /*Reset the ACTIVE_RAT to NONE, as REG/CM will come up with a new REG_REQ once 
          stop_mode_cnf is sent (above) - then MM/EMM will behave similar to UE power up scenario*/
        emm_ctrl_data_ptr->plmn_service_state->active_rat = SYS_RAT_NONE;
        break;
      case SYS_STOP_MODE_REASON_IRAT:
        /* Move EMM connection state to IDLE*/
        EMM_MOVES_TO_IDLE_STATE();
        /* Move EMM mobility state */
        if(EMM_GET_STATE() == EMM_DEREGISTERED)
        {
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
          emm_reset_ctrl_data(emm_ctrl_data_ptr, FALSE);
          mm_reset_global_variables();
#ifdef FEATURE_NAS_GW
          gmm_reset_global_variables();
#endif
        }
        else
        {
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
        }
        mm_last_active_rat_stopped = SYS_RAT_LTE_RADIO_ACCESS;
        emm_ctrl_data_ptr->last_reg_cause = MMR_CAUSE_NONE;

        break;
      case SYS_STOP_MODE_REASON_MODE_CHANGE: 
      case SYS_STOP_MODE_REASON_LPM:
      case SYS_STOP_MODE_REASON_OFFLINE: 
	  /* Move EMM connection state to IDLE*/
	  EMM_MOVES_TO_IDLE_STATE();
#ifdef FEATURE_FMD_SPEED_INFO
		if(( emm_ctrl_data_ptr->rrc_deact_req.deact_reason == SYS_STOP_MODE_REASON_LPM) &&
			 (mm_timer_status[TIMER_FMD_PSM_FALLBACK] != TIMER_ACTIVE))
		{
			if(emm_ctrl_data_ptr->fmd_psm_fallback_timer != 0)
			{
			  mm_stop_timer(TIMER_FMD_PSM_FALLBACK);
			  mm_start_timer(TIMER_FMD_PSM_FALLBACK, (emm_ctrl_data_ptr->fmd_psm_fallback_timer)*1000);
			}
		}  
#endif

        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
        /* Update GMM state if necessary */
#ifdef FEATURE_NAS_GW
        gmm_multimode_set_state(GMM_DEREGISTERED, GMM_NO_CELL_AVAILABLE);
#endif
        mm_reset_global_variables();
#ifdef FEATURE_NAS_GW
        gmm_reset_global_variables();
#endif
        emm_reset_attempt_counter();
        mm_last_active_rat_stopped = SYS_RAT_LTE_RADIO_ACCESS;
        /*Reset the ACTIVE_RAT to NONE, as REG/CM will come up with a new REG_REQ once 
          stop_mode_cnf is sent (above) - then MM/EMM will behave similar to UE power up scenario*/
        emm_ctrl_data_ptr->plmn_service_state->active_rat = SYS_RAT_NONE;
        emm_ctrl_data_ptr->last_reg_cause = MMR_CAUSE_NONE;

        break;
      default:
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
        /* Update GMM state if necessary */
#ifdef FEATURE_NAS_GW
        gmm_multimode_set_state(GMM_DEREGISTERED, GMM_NO_CELL_AVAILABLE);
#endif
        break;
    }
  }
  else
  {
    if(EMM_GET_STATE() == EMM_DEREGISTERED)
    {
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
    }
    else if(EMM_GET_STATE() == EMM_REGISTERED)
    {
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
    }   
  }

  /* If UE is in LTE RLF and stop mode reqason is alt scan then donot clean the message buffer */
  if((emm_ctrl_data_ptr->rrc_deact_req.deact_reason != SYS_STOP_MODE_REASON_ALT_SCAN) ||
     (EMM_GET_CONNECTION_STATE() != EMM_CONNECTED_STATE))
  {
    /* LTE stack is deactivated. Clean up message buffer. */
    MSG_HIGH_DS_1(MM_SUB, "=EMM= LTE stack is deactivated, Clean up message buffer stop_mode_reason %d",
                  emm_ctrl_data_ptr->rrc_deact_req.deact_reason);
    emm_clean_msg_buffer(emm_ctrl_data_ptr);
	  /* LTE stack is deactivated. Clean up PDN CONN REQ for ATTACH if necessary */
    emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
    emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
  }

  /* RRC is deactivated and it needs new rrc service req or plmn search req to go active again.
     Set service status to no service. */
  emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_NO_SRV;

  emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending = FALSE; 
  
  if(emm_ctrl_data_ptr->mm_send_mmr_stop_mode_cnf_fp != NULL)
  {
    if(emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == TRUE)
    {
      /* Stop mode collided with L to GW RAT change deactivation. Need to deact 
         RRC with stop mode cause one more time */
      emm_init_rrc_deact_req(emm_ctrl_data_ptr->rrc_deact_req.deact_reason, 
                             emm_ctrl_data_ptr);
    }
    else
    {
      /* Reset deact reason */
      emm_ctrl_data_ptr->rrc_deact_req.deact_reason = SYS_STOP_MODE_REASON_NONE;
#ifdef FEATURE_WWAN_GNSS_DYNAMIC
      if (gmm_check_wwan_resumption_req())
      {
        /* Send low priority indication to L-RRC */
        emm_send_rrc_wwan_set_priority(FALSE);
        gmm_set_wwan_resumption_req(FALSE);
      }
#endif
      /* Report Stop Mode confirmation to upper layers */
      emm_ctrl_data_ptr->mm_send_mmr_stop_mode_cnf_fp();
    }
  }
  else
  {
    emm_ctrl_data_ptr->mm_send_mmr_stop_mode_cnf_fp = mm_send_mmr_stop_mode_cnf;
  }

  if(emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_REQ_PENDING)
  {
    emm_process_ps_detach_req(emm_ctrl_data_ptr->ps_detach_info.ps_detach_req, 
                              emm_ctrl_data_ptr);
  }
  else if(emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_CNF_PENDING)
  {
    emm_ps_detach_clean_up(emm_ctrl_data_ptr);  
  }

  /* Process any pending service request */
  if(emm_reg_container != NULL)
  {
    emm_reg_container(emm_ctrl_data_ptr);
  }
  else if(emm_ctrl_data_ptr->reg_req_pended_indication == TRUE)
  {
    emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;
    reg_service_report = (emm_reg_service_report_type)emm_send_reg_ind;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Reset reg_req_pended_indication to FALSE");
  }

  if(emm_ctrl_data_ptr->pending_reg_cmd_ptr != NULL)
  {
    if ((save_temp_stop_mode_reason == SYS_STOP_MODE_REASON_ALT_SCAN)||
        (save_temp_stop_mode_reason== SYS_STOP_MODE_REASON_DEEP_SLEEP)
#ifdef FEATURE_CIOT        
        ||(save_temp_stop_mode_reason== SYS_STOP_MODE_REASON_IRAT)
#endif        
        )
     {
       switch(emm_ctrl_data_ptr->pending_reg_cmd_ptr->cmd.hdr.message_id)
       {
         
         case MMR_PLMN_SEARCH_REQ:
         /* Execute only if we are on the same LTE RAT*/
        if(mm_is_rat_lte(emm_ctrl_data_ptr->pending_reg_cmd_ptr->cmd.mmr_plmn_search_req.rat)&&
            (emm_ctrl_data_ptr->loaded_lte_rat != SYS_RAT_NONE)&&
            (emm_ctrl_data_ptr->pending_reg_cmd_ptr->cmd.mmr_plmn_search_req.rat == emm_ctrl_data_ptr->loaded_lte_rat)) 
           {
           emm_process_reg_command(emm_ctrl_data_ptr->pending_reg_cmd_ptr,
                                  emm_ctrl_data_ptr);
           }
           break;
         case MMR_PLMN_SEARCH_ABORT_REQ:
           emm_send_empty_plmnsearch_cnf(
             &emm_ctrl_data_ptr->pending_reg_cmd_ptr->cmd.mmr_plmn_search_req);
           reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
           break;
         default:
           MSG_HIGH_DS_1(MM_SUB ,"=MM= Discarding unexpected pending REG cmd 0x%x", 
               emm_ctrl_data_ptr->pending_reg_cmd_ptr->cmd.hdr.message_id);
           break;
       }
       
     }
    /* In case UE comes back to normal handler before receiving DEACT CNF in 
       OOS/RLF hander, free the memory for pending reg cmd. */

    emm_modem_mem_free(emm_ctrl_data_ptr->pending_reg_cmd_ptr,
                       emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->pending_reg_cmd_ptr = NULL;
  }
#ifdef FEATURE_CIOT
#ifdef FEATURE_NAS_GW
  else if ( (mm_plmn_list_req_pending == TRUE) && 
         ((mmcoord_pending_reg_list_req_message.cmd.hdr.message_set == MS_MM_REG) &&
         (mmcoord_pending_reg_list_req_message.cmd.hdr.message_id  == (int) MMR_PLMN_SEARCH_REQ)))
    {
      MSG_HIGH_DS_0(MM_SUB, "=MM= LTE RAT stopped, Continue search on GSM");
      //mmcoord_pending_reg_list_req_message.cmd.mmr_plmn_search_req.rat = active_rat;
      mmcoord_process_mmr_plmn_search_req(&mmcoord_pending_reg_list_req_message);
      mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
      mmcoord_pending_reg_list_req_message.cmd.hdr.message_set = 0;
    }
#endif
#endif

  
  /* Do not deallocate memory since UE is trying to camp on LTE */
  if(emm_ctrl_data_ptr->reg_req_pended_indication != TRUE)
  {
    /* Free the memory allocated dynamically */
    emm_procedure_message_mem_free(emm_ctrl_data_ptr);
  }
} /* end of emm_process_rrc_deactivate_cnf() */

/*===========================================================================

FUNCTION    EMM_ABORT_ONGOING_SPECIFIC_PROCEDURE

DESCRIPTION
  This function aborts EMM ongoing specific procedures

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_abort_ongoing_specific_procedure
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  

  switch(EMM_GET_STATE())
  {
    case EMM_DEREGISTERED_INITIATED:
      /* Abort detach procedure */
      emm_abort_mo_detach(emm_ctrl_data_ptr, TRUE);
      break;
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      /* Abort TAU procedure */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Aborting ongoing specific procedure - TAU");
      mm_stop_timer(TIMER_T3430);
      emm_remove_release_client(emm_tau_request_release_indication);
      break;
    default:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= No EMM specific procedure ongoing");
      /* No EMM specific procedure is aborted, return FALSE */
      return FALSE;
  }
  /* A EMM specific procedure is aborted, return TRUE */
  return TRUE;
} /* emm_abort_ongoing_specific_procedure */

/*===========================================================================

FUNCTION    EMM_PROCESS_RRC_PAGE_INDICATION

DESCRIPTION
  This function processes the RRC PAGE INDICATION.Refer to 5.6.2 Paging procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void emm_process_rrc_page_indication
(
  lte_rrc_page_ind_s      *rrc_page_ind,
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  emm_state_type     emm_state = EMM_INVALID_STATE;
  emm_substate_type  emm_substate;
  boolean            emm_specific_procedure_aborted = FALSE;

  emm_csfb_rsp  csfb_response;
  boolean buffer_esr = FALSE;

  emm_state = EMM_GET_STATE();
  emm_substate = EMM_GET_SUBSTATE();

  /*initialize it*/
  memset((void *)&csfb_response,0,sizeof(emm_csfb_rsp));

  MSG_HIGH_DS_6(MM_SUB, "=EMM= RRC_PAGE_IND - UE ID type %d, EMM State %d, Substate %d Srv status %d, Update status %d, Sim state %d",
                rrc_page_ind->ue_id.type,
                emm_state,
                emm_substate,
                emm_ctrl_data_ptr->plmn_service_state->service_status,
                emm_ctrl_data_ptr->emm_update_status,
                emm_ctrl_data_ptr->plmn_service_state->sim_state);
   if (mm_get_ulog_handle() != NULL)
   {
     ULOG_RT_PRINTF_2(mm_get_ulog_handle(), "=EMM= RRC_PAGE_IND - UE ID type %d, EMM State %d ",rrc_page_ind->ue_id.type, emm_state);
   }
  if(emm_ctrl_data_ptr->psm_active == TRUE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Ignoring Paging during PSM active");
    return;
  }

#if defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
  if((rrc_page_ind->cn_domain == LTE_RRC_PAGING_CN_DOMAIN_CS)&&
     ((emm_ignore_cs_page_cs_service_notification(emm_ctrl_data_ptr) == TRUE)||
      (emm_reject_cs_service_notification(emm_ctrl_data_ptr) == TRUE)))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Ignoring MT CS Paging during VOLTE call");
    return;
  }
#endif
  switch (rrc_page_ind->ue_id.type)
  {
    case LTE_RRC_UE_ID_STMSI:
      if(rrc_page_ind->cn_domain == LTE_RRC_PAGING_CN_DOMAIN_CS)
      {
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
#ifdef FEATURE_LTE_REL10
        if (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
        {
          mm_stop_timer(TIMER_T3346);
#ifdef FEATURE_CIOT
		  emm_ctrl_data_ptr->is_t3346_started_for_mo_excp = FALSE;
#endif

#ifdef FEATURE_NAS_GW
          gmm_set_gmm_is_congestion_due_to_rau(FALSE);
#endif
          if(EMM_GET_SUBSTATE() == EMM_REGISTERED_ATTEMPTING_TO_UPDATE)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Received CS page ind. initiating TAU\n");
           /* set the flag to remember TAU is triggered due to paging.
              this information will be used to encode device properties IE and setting
              rrc_est_cause to delay tolerant */
            emm_ctrl_data_ptr->responding_to_page = TRUE;
            emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                                       emm_get_eps_update_type(emm_ctrl_data_ptr),
                                       FALSE, 
                                       emm_ctrl_data_ptr);
          }
        }
#endif
        MSG_HIGH_DS_2(MM_SUB, "=EMM= Received Page with S-TMSI = M-TMSI(%x) + MMEC(%x) and CN domain set to CS",
                      rrc_page_ind->ue_id.value.s_tmsi.m_tmsi,
                      rrc_page_ind->ue_id.value.s_tmsi.mmec);
        //emm_validate_page_req();/* In this check for the validity of PAGE for the current EMM state */
        if(emm_check_if_csfb_allowed(LTE_MT_CSFB_1XCSFB,NAS_EMM_3GPP_CSFB,emm_ctrl_data_ptr,&buffer_esr) == TRUE )
        {
          /* Now, validate the PAGE with MM*/
          if(mm_csfb_process_mt_page (EMM_CSFB_PAGINGI_TYPE_TMSI) == TRUE)
          {
            csfb_response.valid = TRUE;
            csfb_response.csfb_rsp =  CSFB_ACCEPT;
                /**-------------------------------------------------------------
           **At reception of an indication of paging for CS services from EMM, 
           **the MS shall stop timer T3246, if it is running. The MM sublayer in the MS requests 
           **EMM to perform the service request procedure for CS fallback.
           **(24.008 section 4.5.1.3.4.) ------------------------------------------------*/
#ifdef FEATURE_NAS_REL10
            mm_stop_timer(TIMER_T3246);
#endif
            emm_ctrl_data_ptr->responding_to_page = TRUE;
            emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MT_ACCESS,LTE_MT_CSFB_1XCSFB,csfb_response,NAS_EMM_3GPP_CSFB);
          }
          else
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= MM rejected the PAGE message");
          }
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB, "=EMM= CSFB not allowed");
        }
#endif
        return;
      }

      if( EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE )
      {
        MSG_ERROR_DS_0(MM_SUB, "=EMM= Unexpected RRC_PAGE_IND");
        return;
      }

      /* Only proceed if there is no EMM specific procedure or service request
         procedure ongoing and EMM is not in IDLE or DEREGISTERED state */
      if(((emm_state == EMM_REGISTERED) && 
          (emm_substate == EMM_REGISTERED_NORMAL_SERVICE ||
           emm_substate == EMM_REGISTERED_UPDATE_NEEDED ||
           emm_substate == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM ||
           emm_substate == EMM_REGISTERED_WAITING_FOR_ESM_ISR_STATUS
#ifdef FEATURE_CIOT
#ifndef FEATURE_CATM1_DISABLE
           ||
           emm_substate == EMM_REGISTERED_WAITING_FOR_ESM_CIOT_RAT_CHANGE
#endif     
#endif
        ))
         &&
         (emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                            SYS_SRV_STATUS_SRV)
         &&
         (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED) 
         &&
         ((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_AVAILABLE) ||
          (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                      SYS_SIM_STATE_CP_SMS_INVALID) ||
          (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_CS_INVALID))
         &&(
#ifndef FEATURE_NAS_IMS_VOICE_DISABLED
            (emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag == TRUE)||
#endif
             emm_check_mplmn_in_progress(emm_ctrl_data_ptr)== FALSE))
      {
#ifdef FEATURE_LTE_REL10
        if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
           (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE))
        {
          /* Stop the Timer */
          mm_stop_timer(TIMER_T3346);
#ifdef FEATURE_CIOT
		  emm_ctrl_data_ptr->is_t3346_started_for_mo_excp = FALSE;
#endif

#ifdef FEATURE_NAS_GW
          gmm_set_gmm_is_congestion_due_to_rau(FALSE);
#endif
          emm_timer_t3346_expire(TRUE, emm_ctrl_data_ptr) ;
        }  
#endif        
        if(emm_state != EMM_TRACKING_AREA_UPDATING_INITIATED)
        {
          emm_ctrl_data_ptr->responding_to_page = TRUE;
          if(emm_check_esr_for_ps(emm_ctrl_data_ptr) == TRUE)
          {
            /* Start extended service request procedure for PS */
            emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MT_ACCESS,LTE_PACKET_SERVICES_VIA_S1,
                                                         csfb_response
#if defined(FEATURE_LTE_TO_1X) ||(defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
                                                         ,NAS_EMM_CSFB_NONE
#endif
                                                         );
          }
          else
          {
#ifdef FEATURE_CIOT
            if((emm_cp_opt_active( emm_ctrl_data_ptr )== TRUE) && 
               (emm_ctrl_data_ptr->is_conn_suspended == FALSE)
#ifdef FEATURE_LTE_REL14
               &&(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL14)
               &&(mm_timer_status[TIMER_T3448] == TIMER_STOPPED) 
               &&(ds3gpp_check_pending_data_for_cpsr() == TRUE)
#endif
               )
#ifdef FEATURE_LTE_REL14
            {
              //Data at DS is pending and DS will start the CPSR with ESM container
              // remember that DS data is pending
              emm_ctrl_data_ptr->is_ds_data_pending_while_page = TRUE;
            }
            else if (emm_cp_opt_active( emm_ctrl_data_ptr )== TRUE &&
			   emm_ctrl_data_ptr->is_conn_suspended == FALSE)
#endif
            {
              emm_start_nas_cp_service_request_procedure(LTE_RRC_EST_CAUSE_MT_ACCESS,NULL,NULL,FALSE);
            }
            else
#endif
            {             
              /* Start service request procedure with the cause of MT_ACCESS. */
              emm_start_nas_service_request_procedure(emm_ctrl_data_ptr, 
                                                  LTE_RRC_EST_CAUSE_MT_ACCESS);
            }
          }
        }
      }
#ifdef FEATURE_LTE_REL10
      else if (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
      {
        mm_stop_timer(TIMER_T3346);
#ifdef FEATURE_CIOT
        emm_ctrl_data_ptr->is_t3346_started_for_mo_excp = FALSE;
#endif
#ifdef FEATURE_NAS_GW
        gmm_set_gmm_is_congestion_due_to_rau(FALSE);
#endif
        if(EMM_GET_SUBSTATE() == EMM_REGISTERED_ATTEMPTING_TO_UPDATE)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Received page ind. initiating TAU\n");
         /* set the flag to remember TAU is triggered due to paging.
            this information will be used to encode device properties IE and setting
            rrc_est_cause to delay tolerant */
          emm_ctrl_data_ptr->responding_to_page = TRUE;
          emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                            emm_get_eps_update_type(emm_ctrl_data_ptr),
                            FALSE, 
                            emm_ctrl_data_ptr);
        }
      }
#endif
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Ignoring RRC_PAGE_IND - SR not allowed");
      }
      break;
    case LTE_RRC_UE_ID_IMSI:
      if(rrc_page_ind->cn_domain == LTE_RRC_PAGING_CN_DOMAIN_CS)
      {
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
#ifdef FEATURE_LTE_REL10
        if (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
        {
          mm_stop_timer(TIMER_T3346);
#ifdef FEATURE_CIOT
		  emm_ctrl_data_ptr->is_t3346_started_for_mo_excp = FALSE;
#endif

	  gmm_set_gmm_is_congestion_due_to_rau(FALSE);
          if(EMM_GET_SUBSTATE() == EMM_REGISTERED_ATTEMPTING_TO_UPDATE)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Received page ind. initiating TAU\n");
            /* set the flag to remember TAU is triggered due to paging.
               this information will be used to encode device properties IE and setting
               rrc_est_cause to delay tolerant */
            emm_ctrl_data_ptr->responding_to_page = TRUE;
            emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                               emm_get_eps_update_type(emm_ctrl_data_ptr),
                               FALSE, 
                               emm_ctrl_data_ptr);
          }
        }
#endif
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Received Page with IMSI and CN domain set to CS");
        //emm_validate_page_req();/* In this check for the validity of PAGE for the current EMM state */
        if(emm_check_if_csfb_allowed(LTE_MT_CSFB_1XCSFB,NAS_EMM_3GPP_CSFB,emm_ctrl_data_ptr,&buffer_esr) == TRUE )
        {
          /* Now, validate the PAGE with MM*/
          if(mm_csfb_process_mt_page (EMM_CSFB_PAGING_TYPE_IMSI) == TRUE)
          {
            csfb_response.valid = TRUE;
            csfb_response.csfb_rsp =  CSFB_ACCEPT;
              /**-------------------------------------------------------------
           **At reception of an indication of paging for CS services from EMM, 
           **the MS shall stop timer T3246, if it is running. The MM sublayer in the MS requests 
           **EMM to perform the service request procedure for CS fallback.
           **(24.008 section 4.5.1.3.4.) ------------------------------------------------*/
#ifdef FEATURE_NAS_REL10
            mm_stop_timer(TIMER_T3246);
#endif
            emm_ctrl_data_ptr->responding_to_page = TRUE;
            emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MT_ACCESS,LTE_MT_CSFB_1XCSFB,csfb_response,NAS_EMM_3GPP_CSFB);
          }
          else
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= MM rejected the PAGE message");
          }
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= MM rejected the PAGE message");
        }
#endif
        return;
      }

#ifdef FEATURE_LTE_REL10
      /* Stop the Timer */
      mm_stop_timer(TIMER_T3346);
#ifdef FEATURE_NAS_GW
      gmm_set_gmm_is_congestion_due_to_rau(FALSE);
#endif
#endif

#ifdef FEATURE_CIOT
		emm_ctrl_data_ptr->is_t3346_started_for_mo_excp = FALSE;
#endif


      if((emm_state == EMM_REGISTERED) || 
         (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED) ||
         (emm_state == EMM_DEREGISTERED_INITIATED))
      {      
        /* 5.6.2.2.2 - Abort ongoing EMM specific procedure */
        /* Note: Attach does not apply here since EMM only gets page ind in the REGISTERED state */
        emm_specific_procedure_aborted = emm_abort_ongoing_specific_procedure(emm_ctrl_data_ptr);

        /* Send detach ind to ESM to release bearers */
        emm_build_and_send_detach_ind(EMM_DETACHED);
        emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED, 
                                     SYS_SIM_STATE_AVAILABLE, 
                                     emm_ctrl_data_ptr, FALSE) ;
#ifdef FEATURE_NAS_GW
    if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_UMTS_RADIO_ACCESS)|| 
       mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_GSM_RADIO_ACCESS) ||
       mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_TDS_RADIO_ACCESS))
    {
      gmm_invalidate_parameters();
    }
#endif
        if((emm_specific_procedure_aborted == TRUE) &&
           (emm_ctrl_data_ptr->ps_detach_info.status == 
                                                    EMM_PS_DETACH_CNF_PENDING))
        {
          emm_ps_detach_clean_up(emm_ctrl_data_ptr);
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
        }
        else
        {
          /* Move to EMM_DEREGISTERED_NORMAL_SERVICE state to be ready to re-attach */
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE) ;
        }

#ifdef FEATURE_LTE_REL10
        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
        {
          emm_send_unblock_all_apns_ind();
          /* set the flag to remember Attach is triggered due to paging.
            this information will be used to encode device properties IE and setting
            rrc_est_cause to delay tolerant */
          emm_ctrl_data_ptr->responding_to_page = TRUE;
        }
#endif
#ifdef FEATURE_LTE_TO_1X
        /* Notify context lost */
        emm_send_irat_ctxt_lost_ind(LTE_NAS_CTXT_LOST_IMSI_PAGING);
#endif
        /* If a EMM specific procedure is aborted, attach procedure will be 
           triggered by receiving a RRC service ind after RRC goes to IDLE 
           and RRC connection is released */
        if (emm_specific_procedure_aborted == FALSE)
        {
          /* If no EMM specific procedure is aborted, no RRC service ind will 
             come. EMM should start attach procedure here. */
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
      }
      else
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Invalid EMM state %d, ignore RRC_PAGE_IND with IMSI",
                     (int)emm_state);
      }
      break;
    default:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= Unknown RRC_PAGE_IND UE ID type");
      break;
  }
} /* emm_process_rrc_page_indication */

/*=============================================================================

FUNCTION    EMM_PROCESS_RRC_PLMN_SEARCH_CNF

DESCRIPTION
  This function processes LTE_RRC_PLMN_SEARCH_CNF primitive

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void  emm_process_rrc_plmn_search_cnf
(
  lte_rrc_plmn_search_cnf_s  *rrc_plmn_search_cnf_ptr,
  emm_ctrl_data_type         *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type current_tai;
  emm_state_type            emm_state = (emm_state_type)EMM_UNKNOWN_STATE;
  int length = 0;

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
  if(rrc_plmn_search_cnf_ptr->search_type == SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH && rrc_plmn_search_cnf_ptr->found_plmn_rat_list.gps_wwan_scan_result !=NULL)
  {
	length = rrc_plmn_search_cnf_ptr->found_plmn_rat_list.gps_wwan_scan_result->wwan_scan_result_per_rat.num_scan_result_per_rat;

  }
  else 
#endif	  
  if(rrc_plmn_search_cnf_ptr->search_type != SYS_SERVICE_SEARCH_CELL_SEARCH)
  {
    length = rrc_plmn_search_cnf_ptr->found_plmn_rat_list.plmn_list.length;
  }
  else
  {
    length = rrc_plmn_search_cnf_ptr->found_plmn_rat_list.cell_info_list.lte_cell_list.num_cell;
  } 
  MSG_HIGH_DS_3(MM_SUB, "=EMM= Rcved RRC PLMN SEARCH CNF - Trans ID %d, Status %d No of PLMNs found %d",
                rrc_plmn_search_cnf_ptr->trans_id,
                rrc_plmn_search_cnf_ptr->status,
                length);

  emm_state = EMM_GET_STATE();
  current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->
                                                                                 camped_svc_info.selected_plmn) ;
  current_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac ;

  /* Dont send Plmn to mmode if the number of PLMNs found are 0*/
  if( (mm_emm_is_qrd_device == TRUE || mm_partial_search_allowed_get() == TRUE) && 
  	  rrc_plmn_search_cnf_ptr->status == LTE_RRC_PLMN_SEARCH_PARTIAL &&
  	  length != 0 
 #ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
	  && rrc_plmn_search_cnf_ptr->search_type != SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH
 #endif	  
  	  )
  {
     return ;
  }
  if(emm_ctrl_data_ptr->ps_detach_info.status != EMM_PS_DETACH_NONE)
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= PS Detach pending %d - Discarding PLMN search result",
                  emm_ctrl_data_ptr->ps_detach_info.status);
    rrc_plmn_search_cnf_ptr->status = LTE_RRC_PLMN_SEARCH_ABORTED;
  }

  emm_send_mmr_plmn_search_cnf(rrc_plmn_search_cnf_ptr);

  /*EMM just like MM will always send the service indication to REG after PLMN search cnf, for updating the service status*/
  if( rrc_plmn_search_cnf_ptr->status != LTE_RRC_PLMN_SEARCH_PARTIAL)						  
  {
#ifdef FEATURE_NAS_GW
    /*Accept ACTIVEATION_INDICATION from RRC*/
    emm_ctrl_data_ptr->ignore_GW_activation_ind    = FALSE;
#endif
    mm_set_plmn_search_in_progress_flag(FALSE);
    if(!(((emm_state == EMM_REGISTERED_INITIATED )||
          (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED))||
          ((mm_timer_status[TIMER_T3411] == TIMER_ACTIVE)&&
           (
#ifdef FEATURE_NAS_GW
            (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI)||
#endif
            (emm_search_tai_list(&current_tai,emm_ctrl_data_ptr->emm_tai_list_ptr)== FALSE))&&
           ((sys_plmn_match(current_tai.plmn,emm_ctrl_data_ptr->last_attempted_tai.plmn) == TRUE)&&
            (current_tai.tac == emm_ctrl_data_ptr->last_attempted_tai.tac)))))
    {
        if(emm_ctrl_data_ptr->last_reg_cause != MMR_CAUSE_NONE)
        {
          emm_send_reg_ind(emm_ctrl_data_ptr->last_reg_cause, FALSE, emm_ctrl_data_ptr);
        }
        else
       {
          emm_send_reg_ind(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
       }
    }
  }
} /* emm_process_rrc_plmn_search_cnf */


/*=============================================================================

FUNCTION    EMM_PROCESS_LTE_RRC_CCO_NACC_COMPLETED_IND

DESCRIPTION
  This function processes LTE_RRC_CCO_NACC_COMPLETED_IND primitive

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
#if defined(FEATURE_GSM) && defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_IRAT_DISABLED)
void emm_process_lte_rrc_cco_nacc_completed_ind
(
  emm_ctrl_data_type         *emm_ctrl_data_ptr
)
{
   

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Rcved LTE_RRC_CCO_NACC_COMPLETED_IND");
  EMM_MOVES_TO_IDLE_STATE();
  mm_start_timer(TIMER_T3412,emm_ctrl_data_ptr->t3412_value);
  if(emm_ctrl_data_ptr->t3324_value != NAS_INVALID_PSM_TIMER_VALUE)
  {
    if(emm_ctrl_data_ptr->t3324_value != 0)
    {
      mm_start_timer(TIMER_T3324, emm_ctrl_data_ptr->t3324_value);
    }
    else
    {
      emm_timer_t3324_expire(emm_ctrl_data_ptr);
    }
  }
  /* Send ESM connection release indication */
  emm_esm_release_indication(CONN_REL_LTE_RRC_CONN_REL_NORMAL, emm_ctrl_data_ptr);
  mm_process_cco_complete_ind(emm_ctrl_data_ptr);
  /* This is for scenario where when service req on LTE is pending and we receive stop cnf 
     from GSM before CCO Complete indication from LTE. In this case we will pend the reg request.
     Please check emm_process_reg_req for reg request pending.
     The pending reg request is processed now.*/
  if (emm_reg_container != (emm_reg_container_type)NULL)
  {
    emm_reg_container(emm_ctrl_data_ptr) ;
  }
}
#endif

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
/*=============================================================================

FUNCTION    emm_process_lte_rrc_csg_info_update_ind

DESCRIPTION
  This function processes LTE_RRC CSG info update

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void emm_process_lte_rrc_csg_info_update_ind
(
  lte_rrc_csg_info_update_ind_s  *csg_info_update,
  emm_ctrl_data_type                  *emm_ctrl_data_ptr
)
{

  sys_plmn_id_s_type  rrc_plmn_id;

   
  MSG_HIGH_DS_0(MM_SUB, "=EMM= LTE_RRC_CSG_INFO_UPDATE_IND");

  rrc_plmn_id = emm_convert_rrc_plmn_id_to_nas_plmn_id(csg_info_update->selected_plmn);

  if((mm_is_rat_lte(emm_ctrl_data_ptr->plmn_service_state->active_rat)) &&
      (emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id == csg_info_update->csg_info.csg_id) &&
      (sys_plmn_match(emm_ctrl_data_ptr->plmn_service_state->plmn, rrc_plmn_id)))
  {
    emm_ctrl_data_ptr->plmn_service_state->csg_info.hnb_name = csg_info_update->csg_info.hnb_name;
    emm_send_reg_home_nodeb_name_ind(csg_info_update);
  }
}
#endif 

#ifdef FEATURE_NAS_GW
/*=============================================================================
FUNCTION    emm_process_lte_rrc_sib_info_ind

DESCRIPTION
  This function processes SIB INFO IND from LTE_RRC for SIB6 and SIB7

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void emm_process_lte_rrc_sib_info_ind
(
  emm_rrc_cmd_type         *rrc_cmd_ptr,
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{

  switch(rrc_cmd_ptr->rrc_msgr_cmd.msg_hdr.id)
  {
    case LTE_RRC_SIB6_INFO_IND:
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Received LTE_RRC_SIB6_INFO_IND");
    emm_send_mmr_rrc_sib6_info_ind(&rrc_cmd_ptr->rrc_msgr_cmd.rrc_sib6_info_ind);
    break;

    case LTE_RRC_SIB7_INFO_IND:
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Received LTE_RRC_SIB7_INFO_IND");
    emm_send_mmr_rrc_sib7_info_ind(&rrc_cmd_ptr->rrc_msgr_cmd.rrc_sib7_info_ind);
    break;

    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Unexpected SIB INFO IND primitive %d", 
                                   rrc_cmd_ptr->rrc_msgr_cmd.msg_hdr.id);
    break;
  }
}

#endif
#ifdef FEATURE_NAS_EAB
/*=============================================================================
FUNCTION    emm_process_lte_rrc_eab_update_ind

DESCRIPTION
  This function processes EAB UPDATE IND from LTE_RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void emm_process_lte_rrc_eab_update_ind
(
  lte_rrc_eab_update_ind_s *eab_update_ind,
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{
  ASSERT(eab_update_ind != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS_2(MM_SUB, "=EMM= Received LTE_RRC_EAB_UPDATE_IND, eab_valid %d, eab_barring %d",
                eab_update_ind->eab_valid, emm_ctrl_data_ptr->eab_barring);

  if(eab_update_ind->eab_valid == FALSE)
  {
    emm_stop_eab_barring(emm_ctrl_data_ptr);
  }
}
#endif

#ifdef FEATURE_CIOT
/*=============================================================================
FUNCTION    emm_process_lte_rrc_ab_update_ind

DESCRIPTION
  This function processes AB UPDATE IND from LTE_RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void emm_process_lte_rrc_ab_update_ind
(
  lte_rrc_ab_update_ind_s * ab_update_ind,
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{
  word parsing_index = 0;
  ASSERT(ab_update_ind != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS_2(MM_SUB, "=EMM= Received LTE_RRC_AB_UPDATE_IND,LRRC send ab_barring flag = 0x%x NAS stored barring mask = 0x%x",
  	                                                     ab_update_ind->call_type_bitmask,emm_ctrl_data_ptr->access_barring_flag);


// add logic to handle mo exception
  for (parsing_index=0; parsing_index < TOTAL_NUMBER_OF_CALL_TYPES; parsing_index++)
  {
	 if (((1<<parsing_index) & ab_update_ind->call_type_bitmask) != 0)
	 {
       emm_barring_call_back(1<<parsing_index);
	 }
  } 

}

#endif

boolean emm_process_rrc_primitive
(
  void                  *rrc_message_router_cmd,// Changed the i/p parameter to void*, to be compatible to function pointer
  emm_ctrl_data_type    *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
)
{
  emm_rrc_cmd_type   *rrc_msgr_cmd; //Initialize emm_rrc_cmd_type pointer
  emm_security_context_info  *context_ptr = NULL;
  boolean             start_tau = FALSE;

  rrc_msgr_cmd = (emm_rrc_cmd_type*)rrc_message_router_cmd; //Typecast rrc_message_router_cmd to emm_rrc_cmd_type
   

  switch (rrc_msgr_cmd->rrc_msgr_cmd.msg_hdr.id)
  {
    case LTE_RRC_CONN_EST_CNF:
      emm_process_connection_confirmation(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_conn_est_cnf,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_UL_DATA_CNF:
      emm_process_rrc_ul_data_cnf(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_ul_data_cnf,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_DEACTIVATE_CNF:
      emm_process_rrc_deactivate_cnf(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_deactivate_cnf,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_ACTIVATION_IND:
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_IRAT_DISABLED)
      if((emm_ctrl_data_ptr->plmn_service_state->active_rat == 
                                                   SYS_RAT_GSM_RADIO_ACCESS) ||
         (emm_ctrl_data_ptr->plmn_service_state->active_rat == 
                                                   SYS_RAT_UMTS_RADIO_ACCESS)||
         (emm_ctrl_data_ptr->plmn_service_state->active_rat == 
                                                   SYS_RAT_TDS_RADIO_ACCESS))
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= LTE_RRC_ACTIVATION_IND received, current rat =%d",
                      emm_ctrl_data_ptr->plmn_service_state->active_rat);
        mm_emm_process_GW_to_LTE_rat_change(
                                       &rrc_msgr_cmd->rrc_msgr_cmd.rrc_act_ind,
                                        mm_emm_dispatcher_fn_ptr,
                                        emm_ctrl_data_ptr);
      }
      else
#endif
      {
        MSG_ERROR_DS_1(MM_SUB, "=EMM= LTE_RRC_ACTIVATION_IND shouldn't be received when active rat is LTE or none, discard the message, current rat=%d",
                       emm_ctrl_data_ptr->plmn_service_state->active_rat);
      }
      break ;
#ifdef FEATURE_NAS_GW
    case LTE_RRC_NAS_UMTS_KEY_IND:
      emm_rrc_process_GW_key_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_umts_key_ind,emm_ctrl_data_ptr) ;
      break;

    case LTE_RRC_NAS_GSM_KEY_IND:
      emm_rrc_process_GW_key_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_umts_key_ind,emm_ctrl_data_ptr) ;
      break;
#endif
   case LTE_RRC_NAS_LTE_KEY_IND:
      emm_rrc_process_lte_key_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_lte_key_ind,emm_ctrl_data_ptr) ;
      break;

    case LTE_RRC_SERVICE_IND:
      emm_rrc_process_service_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_service_ind,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_BARRING_UPDATE_IND:
      emm_rrc_process_barring_update_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_barring_update_ind,emm_ctrl_data_ptr) ;
      break;
    case LTE_RRC_CONN_REL_IND:
      emm_process_rrc_release_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_conn_rel_ind,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_DL_DATA_IND:
      emm_process_rrc_data_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_dl_data_ind,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_PAGE_IND:
      emm_process_rrc_page_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_page_ind, emm_ctrl_data_ptr);
      break ;
    case LTE_RRC_ACTIVE_EPS_BEARER_UPDATE_IND:
      context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr,
                                               CONTEXT_STATE_CURRENT);
      if( context_ptr != NULL )
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= LTE_RRC_ACTIVE_EPS_BEARER_UPDATE_IND, set nas_secure_exchange to TRUE");
        emm_ctrl_data_ptr->nas_secure_exchange = TRUE;
      }

      /* Received RRC_ACTIVE_ESP_IND which indicates the completion of service request procedure if there is one ongoing */
      /* If needed, complete service request procedure */
      if ( EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED )
      {
        if((emm_ctrl_data_ptr->service_req_cause == 
                                                  LTE_RRC_EST_CAUSE_MT_ACCESS) &&
             (emm_ctrl_data_ptr->cell_barred_tau_pending == TRUE))
        {
          /* TAU was delayed because of access class barring. Since UE get paged
            and conn is established, start TAU. */
          if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_PERIODIC)
          {
            /* Connection is up by NW paging. No need to start delayed
             periodic TAU. Reset the TAU cause.*/
            emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
          }
          else
          {
            emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;
            start_tau = TRUE;
          }
         }
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
          if( (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE) || 
            (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) || 
            (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED))
          {
            /* Process the service request procedure completion */
            emm_complete_service_request(emm_ctrl_data_ptr);
#ifdef FEATURE_LTE_REL14
            if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL14)
            {
              mm_start_timer(TIMER_T3449,DEFAULT_TIMEOUT); 
            }
#endif
            if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) || (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED))
            {
              emm_send_esr_if_buffered(emm_ctrl_data_ptr);
            }
          }
#else
          emm_complete_service_request(emm_ctrl_data_ptr);
#ifdef FEATURE_LTE_REL14
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL14)
          {
            mm_start_timer(TIMER_T3449,DEFAULT_TIMEOUT); 
          }
#endif
#endif
          /* Send out all pended messages */
          emm_send_all_pended_messages(emm_ctrl_data_ptr);
#ifdef FEATURE_LTE_TO_1X
          emm_send_all_pending_irat_msg(emm_ctrl_data_ptr);
#endif
        }

      /* NW configured radio bearers. No need to sync drb staus anymore. */
      if((EMM_GET_STATE() == EMM_REGISTERED_INITIATED) &&
        (EMM_GET_SUBSTATE() == EMM_WAITING_FOR_ESM_RESPONSE))
      {
        emm_ctrl_data_ptr->sync_drb_status = FALSE;
      }

      emm_process_rrc_eps_bearer_update(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_active_eps_bearer_update_ind,emm_ctrl_data_ptr) ;
      /* Start delayed TAU because of access class barring */
      if(start_tau == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Init delayed TAU upon completion of MT SR");
        emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                                   emm_get_eps_update_type(emm_ctrl_data_ptr),
                                   FALSE, 
                                   emm_ctrl_data_ptr);      
      }
      break ;
    case LTE_RRC_CONN_EST_TIMER_UPDATE_IND:
      emm_start_connection_delay_timers(rrc_msgr_cmd->rrc_msgr_cmd.rrc_conn_est_timer_update_ind.call_type_bitmask,
                                       (dword)rrc_msgr_cmd->rrc_msgr_cmd.rrc_conn_est_timer_update_ind.barring_timer_value,
                                        emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_PLMN_SEARCH_CNF:
      emm_process_rrc_plmn_search_cnf(
                               &rrc_msgr_cmd->rrc_msgr_cmd.rrc_plmn_search_cnf,
                                emm_ctrl_data_ptr);
      if(rrc_msgr_cmd->rrc_msgr_cmd.rrc_plmn_search_cnf.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL 
	      && rrc_msgr_cmd->rrc_msgr_cmd.rrc_plmn_search_cnf.status != LTE_RRC_PLMN_SEARCH_PARTIAL)
      {
        emm_ctrl_data_ptr->plmn_search_in_progress = FALSE;
      }
      break;
#ifdef FEATURE_LTE_TO_1X
    case LTE_RRC_IRAT_TUNNEL_UL_MSG_CNF:
#ifdef FEATURE_LTE_TO_HDR_OH
    case LTE_RRC_IRAT_HDR_TUNNEL_UL_MSG_CNF:
#endif
      emm_process_rrc_irat_tunnel_ul_msg_cnf(
                        &rrc_msgr_cmd->rrc_msgr_cmd.rrc_irat_tunnel_ul_msg_cnf,
                         emm_ctrl_data_ptr);
      break;
    case LTE_RRC_IRAT_TUNNEL_DL_MSG_IND:
      emm_process_rrc_irat_tunnel_dl_msg_ind(
                        &rrc_msgr_cmd->rrc_msgr_cmd.rrc_irat_tunnel_dl_msg_ind,
                         emm_ctrl_data_ptr);
      break;
#ifdef FEATURE_LTE_TO_HDR_OH
    case LTE_RRC_IRAT_HDR_TUNNEL_DL_MSG_IND:
      emm_process_rrc_irat_3gpp2_tunnel_dl_msg_ind(
                        &rrc_msgr_cmd->rrc_msgr_cmd.rrc_irat_cdma_tunnel_dl_msg_ind,
                         emm_ctrl_data_ptr);
      break;
#endif
    case LTE_RRC_1XCSFB_HO_STARTED_IND:
      emm_process_rrc_1xcsfb_ho_started_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_1xcsfb_ho_started_ind,emm_ctrl_data_ptr);
      break;
    case LTE_RRC_1XCSFB_HO_FAILED_IND:
      emm_process_rrc_1xcsfb_ho_failed_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_1xcsfb_ho_failed_ind,emm_ctrl_data_ptr);
      break;

    case LTE_RRC_1XSRVCC_HO_STARTED_IND:
      emm_process_rrc_1xsrvcc_ho_started_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_1xsrvcc_ho_started_ind,emm_ctrl_data_ptr);
      break;
    case LTE_RRC_1XSRVCC_HO_FAILED_IND:
      emm_process_rrc_1xsrvcc_ho_failed_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_1xsrvcc_ho_failed_ind,emm_ctrl_data_ptr);
      break;
#endif
#if defined(FEATURE_GSM) && defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_IRAT_DISABLED)
    case LTE_RRC_CCO_NACC_COMPLETED_IND:
      emm_process_lte_rrc_cco_nacc_completed_ind(emm_ctrl_data_ptr);
      break;
#endif
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
    case LTE_RRC_CSG_INFO_UPDATE_IND:
      emm_process_lte_rrc_csg_info_update_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_csg_info_update_ind, emm_ctrl_data_ptr);
      break;  
#endif 
#ifdef FEATURE_NAS_GW
    case LTE_RRC_SIB6_INFO_IND:
    case LTE_RRC_SIB7_INFO_IND:
      emm_process_lte_rrc_sib_info_ind(rrc_msgr_cmd, emm_ctrl_data_ptr);
      break;
#endif
#ifdef FEATURE_NAS_EAB
    case LTE_RRC_EAB_UPDATE_IND:
      emm_process_lte_rrc_eab_update_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_eab_update_ind, emm_ctrl_data_ptr);
      break;
#endif

#ifdef FEATURE_CIOT
    case LTE_RRC_AB_UPDATE_IND:
     emm_process_lte_rrc_ab_update_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_ab_update_ind, emm_ctrl_data_ptr);
	 break;
#endif
    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Unknown RRC primitive %d",rrc_msgr_cmd->rrc_msgr_cmd.msg_hdr.id);
      break ;
  }
  return TRUE;
} /* end of emm_process_rrc_primitive() */

#endif /*FEATURE_LTE*/


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

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/emm_reg_handler.c#2 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/11/14   pahire  Added changes for UE Specific DRX Cycle for QChat/LTE 
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity 
07/04/11   abhi    Added processing of ph_status_change_request 
07/11/10   zren    Added support for SYS_STOP_MODE_REASON_DEEP_SLEEP 
06/02/10   zren    Updated EMM to not update EMM state when receives SERVICE 
                   REQUEST in REGISTERED UPDATE NEEDED state
05/18/10   zren    Updated fun param to set only_for_reg flag in REG CNF and 
                   REG IND
05/11/10   zren    Updated to use trans ID rcved in PLMN SEARCH REQ
05/07/10   zren    Added code to set ue_oos field in REG CNF
04/09/10   zren    Added combined procedure feature
04/01/10   zren    Updated to start switch off detach upon receiving stop mode
                   with offline cause
03/24/10   zren    Updated to send RRC SERVICE REQ w/o aborting connection when
                   T311 is still running
03/17/10   MNK     Code changes for handling stop mode in deregister state
03/16/10   zren    Updated MO detach procedure upon receiving MMR_STOP_MODE
03/01/10   MNK     Combine REG_REQ saving for MM and EMM
02/25/10   MNK     Call GW state setting API when MMR_REG_REQ received
02/24/10   zren    Added support for network selection modes of LIMITED MANUAL
                   ACQUISITION and LIMITED ROUTINE ACQUISITION
02/24/10   MNK     Activate GW if multimode UE when MMR_REG_REQ received in LTE
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
02/09/10   MNK     Send STOP_MODE_CNF even in EMM_NULL state. This was happeningwhen REG/EMM reject CM_SERVICE_REQ/REG_REQ and STOP_MODE (specifically when LIMITED_ROUTINE acquisition is requested) comes from CM due to e.g SIM_AVAILABLE
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
01/30/10   MNK     Fix issues with Deregister state.
01/12/09   MNK     add easy debug statement
10/30/09   zren    Updated to send service request failure indication when 
                   service request procedure is aborted due to MO detach if
                   necessary
09/21/09   MNK     Changes due to IRAT design
09/14/09   hnam    Activated NAS events
08/20/09   hnam    Modified the REG_INCOMING events' type based on the 
                   corresponding changes in naslog.h
08/14/09   zren    Modified procedure connection release handling
08/14/09   hnam    Added support NAS events
06/23/09   zren    Added support of processing EPLMN list, forbidden PLMN list,
                   forbidden TAI list for roaming and forbidden TAI list for
                   regional provision of service 
06/17/09   zren    Modified power off detach success CB function to abort 
                   signaling connection
05/29/09   zren    Moved reg container setup from emm_send_rrc_service_req to
                   emm_process_reg_req
05/11/09   zren    Added headers for MO detach procedure functions 
04/27/09   hnam    Set the "only_for_reg" flag in "mmr_service_ind_s" and "mmr_reg_cnf_s_type" to FALSE
04/15/09   RI      Added cell barring flag to emm_rrc_abort_connection()
04/15/09   zren    Completed MO detach feature
04/03/09   vdr     Fix preparation of the MMR_REG_CNF and MMR_REG_IND
02/23/09   vdr     Fixed Lint errors & warnings
02/20/09   vdr     Modified RRC Service Request primitive in order to incorporate
                   requested lte_band_pref
02/06/09   vdr     Modified failure delivery callback in order to delete message 
                   from the cache mem
01/21/09   vdr     RRC lte_rrc_found_plmn_list_s type has been changed
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "emm.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
#include "rex.h"
#include "emm_reg_handler.h"
#include "assert.h"
#include "emm_database.h"
#include "emm_state.h"
#include "msgr_lte.h"
#include "reg_mm_v.h"
#include "emm_rrc_if.h"
#include "emm_connection_handler.h"
#include "emm_connection_type.h"
#include "emm_utility.h"
#include "emm_update_lib.h"
#include "ms.h"
#include "lte_rrc_ext_msg.h"
#include "emm_sim_handler.h"
#include "sys_stru_v.h"
#include "emm_security.h"
#include "err.h"
#include "naslog_v.h"
#include "event.h"
#include "event_defs.h"
#include "emm_service_request_lib.h"
#include "emm_esm_handler.h"
#include "mm_multimode_common.h"
#include "emm_timer_sigs.h"
#include "emm_csfb_handler.h"
#include "sys_eplmn_list_v.h"
#include "gmm_int_v.h"
#include "stringl.h"
#include "emm_mem.h"
#include "emm_rrc_handler.h"
#include "cm_v.h"
#include "ULogFront.h"
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
#include "ref_cnt_obj.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_SGLTE
extern void mm_set_is_pstimer_expired(boolean val);
#endif

extern ULogHandle mm_get_ulog_handle();

/*===========================================================================

FUNCTION EMM_PROCESS_PLMN_SEARCH_REQ

DESCRIPTION
  This function processes MMR_PLMN_SEARCH_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_validate(a) sys_eplmn_per_nas_stacks_list_validate(a,(sys_modem_as_id_e_type)mm_as_id)
#endif
void emm_process_plmn_search_req
(
  mm_cmd_type                *reg_cmd_ptr, 
  emm_ctrl_data_type         *emm_ctrl_data_ptr
)
{
  mmr_plmn_search_req_s_type  *reg_plmnsearch_ptr = NULL;
  log_lte_nas_emm_incoming_event_type  event;
  reg_mm_plmn_search_status_e_type  search_result = REG_MM_PLMN_SEARCH_SUCCESS;
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
  mm_csfb_state_type csfb_state = MM_CSFB_STATE_NULL;
  csfb_state = mm_csfb_get_state();
#endif

  reg_plmnsearch_ptr = &reg_cmd_ptr->cmd.mmr_plmn_search_req;

  MSG_HIGH_DS_4(MM_SUB, "=EMM= PLMN_SEARCH_REQ - Sel Mode %d, Type %d PS Detach %d REG REQ %d",
                reg_plmnsearch_ptr->network_selection_mode,
                reg_plmnsearch_ptr->service_search,
                emm_ctrl_data_ptr->ps_detach_info.status,
                emm_ctrl_data_ptr->reg_req_pended_indication);

  event.emm_in_msg = (byte)MMR_PLMN_SEARCH_REQ;
  event_report_payload(EVENT_LTE_REG_INCOMING_MSG,sizeof(event),(void*)&event);

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  mm_csg_search_type = reg_plmnsearch_ptr->csg_search_type;
  if((reg_plmnsearch_ptr->service_search == SYS_SERVICE_SEARCH_CSG) &&
      (mm_is_csg_search_allowed(mm_csg_search_type,
                                mm_csg_selection_config,
                                &search_result) == FALSE))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= CSG Search not allowed");
    mm_send_empty_mmr_plmn_search_cnf(reg_plmnsearch_ptr, search_result);
    return;
  }
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Received CSG Search Type - %d",
                mm_csg_search_type);
#endif

  if((emm_ctrl_data_ptr->ps_detach_info.status != EMM_PS_DETACH_NONE)
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
     ||
     (csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)||
     (csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
#endif
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

     ||
     ((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) &&
      (reg_plmnsearch_ptr->network_selection_mode == 
                                       SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
      (reg_plmnsearch_ptr->service_search == SYS_SERVICE_SEARCH_NONE))
#endif
#endif
    )
  {
    emm_send_empty_plmnsearch_cnf(reg_plmnsearch_ptr);
    return;
  }

  /* Check if there is REG REQ pending */
  if(emm_ctrl_data_ptr->reg_req_pended_indication == TRUE)
  {
    if((reg_plmnsearch_ptr->network_selection_mode != 
                                          SYS_NETWORK_SELECTION_MODE_MANUAL) &&
       (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode ==        
                                          SYS_NETWORK_SELECTION_MODE_MANUAL))
    {
      emm_send_empty_plmnsearch_cnf(reg_plmnsearch_ptr);
      return;
    }

    MSG_HIGH_DS_0(MM_SUB, "=EMM= MMR_PLMN_SEARCH_REQ calls off pending MMR_REG_REQ");
    emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;
    reg_service_report = (emm_reg_service_report_type)emm_send_reg_ind;
  }
  
  if((*emm_ctrl_data_ptr->conn_mode_manual_search_ptr == TRUE ) &&
     (reg_plmnsearch_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) &&
     (reg_plmnsearch_ptr->service_search != SYS_SERVICE_SEARCH_CELL_SEARCH)
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH	
      && (reg_plmnsearch_ptr->service_search != SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH)
#endif
    )
  {
    if((( EMM_GET_CONNECTION_STATE() == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE ) ||
       ( EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE ))&&
       ( emm_ctrl_data_ptr->emm_sms_active == FALSE )&&
       ( emm_mmtel_status(emm_ctrl_data_ptr) == FALSE )&&
       ((( EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED )
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
        &&
       ( emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)
#endif
#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

        &&
       ( emm_ctrl_data_ptr->call_type !=  SYS_LTE_REESTAB_CALLTYPE_VOLTE)&&
       ( emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
#endif
       )||
       (EMM_GET_STATE() == EMM_REGISTERED)||
       (EMM_GET_STATE() == EMM_DEREGISTERED)))
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Abort RRC connection for manual plmn serach request");
        emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL) ;
      }
  }
    emm_send_rrc_plmn_search_req( reg_plmnsearch_ptr, 
                                       emm_ctrl_data_ptr, 
                                       TRUE);
} /* end of emm_process_plmn_search_req() */

/*===========================================================================

FUNCTION  EMM_PROCESS_PENDED_REG_REQ

DESCRIPTION
  This function processes pending PLMN_SEARCH_REQ and REG_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_pended_reg_req
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  if (emm_ctrl_data_ptr->reg_req_pended_indication == TRUE)
  {
    mmr_reg_req_s_type *reg_req_ptr = emm_ctrl_data_ptr->reg_req_pending_ptr;
    emm_ctrl_data_ptr->plmn_service_state->active_rat = emm_ctrl_data_ptr->reg_req_pending_ptr->rat;
    /* allocate the memory for ctrl data variables */
    emm_procedure_message_mem_alloc(emm_ctrl_data_ptr);

    switch(EMM_GET_STATE())
    {
      case EMM_NULL:
      case EMM_DEREGISTERED:
        switch (reg_req_ptr->network_selection_mode)
        {
          case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
          case SYS_NETWORK_SELECTION_MODE_MANUAL:
          case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
          case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
#ifdef FEATURE_1XSRLTE
          case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
          case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
#endif 
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
          case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
#endif 
            if((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                            SYS_SIM_STATE_NOT_AVAILABLE) ||
               (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                 SYS_SIM_STATE_PS_INVALID))
            {
              emm_reject_service_req(emm_ctrl_data_ptr) ;
            }
            else
            {                  
              emm_send_rrc_service_req(reg_req_ptr->network_selection_mode,
                                       TRUE,
                                       &reg_req_ptr->plmn,
                                       TRUE,
                                       TRUE,
                                       FALSE, // e-plmn revisit
                                       TRUE,
                                       FALSE,
                                       0,
                                       emm_ctrl_data_ptr,
                                       &reg_req_ptr->rat_pri_list_info,
                                       reg_req_ptr->type
#ifdef FEATURE_LTE_REL9
                                      ,reg_req_ptr->additional_info
#endif                 
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                                      ,reg_req_ptr->csg_id
#endif 
                                      ,reg_req_ptr->scan_scope 
                                      ,reg_req_ptr->trm_timeout
				      ,reg_req_ptr->scan_info.new_scan
                                      ,reg_req_ptr->scan_info.use_timer
                                      ,reg_req_ptr->scan_info.scan_time
				      ,reg_req_ptr->scan_info.nb1_acqdb_timer_status
                                      ,reg_req_ptr->scan_info.nb1_fullband_timer_status
                                       );
#ifdef FEATURE_1XSRLTE
              if((reg_req_ptr->network_selection_mode == 
                               SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
                 (reg_req_ptr->network_selection_mode == 
                               SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY))
              {
                EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE) ;
              }
              else
#endif 
              {
                EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED);
              }

              reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf ;
            }
            break;              
      case SYS_NETWORK_SELECTION_MODE_LIMITED:
            emm_send_rrc_service_req(reg_req_ptr->network_selection_mode,
                                     FALSE,
                                     &reg_req_ptr->plmn,
                                     TRUE,
                                     TRUE,
                                     FALSE, // e-plmn revisit
                                     TRUE,
                                     FALSE,
                                     0,
                                     emm_ctrl_data_ptr,
                                     &reg_req_ptr->rat_pri_list_info,
                                     reg_req_ptr->type
#ifdef FEATURE_LTE_REL9
                                    ,reg_req_ptr->additional_info
#endif                   
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                                    ,reg_req_ptr->csg_id
#endif 
                                    ,reg_req_ptr->scan_scope
                                    ,reg_req_ptr->trm_timeout
				    ,reg_req_ptr->scan_info.new_scan
                                    ,reg_req_ptr->scan_info.use_timer
                                    ,reg_req_ptr->scan_info.scan_time
			            ,reg_req_ptr->scan_info.nb1_acqdb_timer_status
                                    ,reg_req_ptr->scan_info.nb1_fullband_timer_status
                                     );

              reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf ;
              EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE) ;
            break ;
          default:
            MSG_ERROR_DS_1(MM_SUB, "=EMM= Unknown Network Selection mode %d",
                           reg_req_ptr->network_selection_mode);
            emm_reject_service_req(emm_ctrl_data_ptr) ;
            break ;
        }
        break ;
      case EMM_REGISTERED:
        /* New system, clear ongoing detach flag to avoid the detach retry in
           case of there is a previously access barred detach */
        emm_ctrl_data_ptr->ongoing_detach_procedure = FALSE;

        switch (reg_req_ptr->network_selection_mode)
        {
          case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
          case SYS_NETWORK_SELECTION_MODE_MANUAL:
          case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
          case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
#ifdef FEATURE_1XSRLTE
          case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
          case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
#endif 
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
          case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
#endif 
#ifdef FEATURE_ENHANCED_NW_SELECTION
            emm_ctrl_data_ptr->refresh_pending = TRUE;
#endif 
            emm_send_rrc_service_req(reg_req_ptr->network_selection_mode,
                                     TRUE,
                                     &reg_req_ptr->plmn,
                                     TRUE,
                                     TRUE,
                                     FALSE, // e-plmn revisit
                                     TRUE,
                                     FALSE,
                                     0,
                                     emm_ctrl_data_ptr,
                                     &reg_req_ptr->rat_pri_list_info,
                                     reg_req_ptr->type
#ifdef FEATURE_LTE_REL9
                                    ,reg_req_ptr->additional_info
#endif                                   
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                                    ,reg_req_ptr->csg_id
#endif 
                                    ,reg_req_ptr->scan_scope 
                                    ,reg_req_ptr->trm_timeout
#ifdef FEATURE_CIOT
				    ,reg_req_ptr->scan_info.new_scan
                                    ,reg_req_ptr->scan_info.use_timer
                                    ,reg_req_ptr->scan_info.scan_time //FRACQ
                                    ,reg_req_ptr->scan_info.nb1_acqdb_timer_status
                                    ,reg_req_ptr->scan_info.nb1_fullband_timer_status
#endif
                                    );

#ifdef FEATURE_1XSRLTE
            if((reg_req_ptr->network_selection_mode == 
                             SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
               (reg_req_ptr->network_selection_mode == 
                             SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY))
            {
              EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_LIMITED_SERVICE) ;
            }
            else
#endif 
            {
              EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_PLMN_SEARCH);
            }
            reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf ;
            break ;
          case SYS_NETWORK_SELECTION_MODE_LIMITED:
            emm_send_rrc_service_req(reg_req_ptr->network_selection_mode,
                                     FALSE,
                                     &reg_req_ptr->plmn,
                                     TRUE,
                                     TRUE,
                                     FALSE, // e-plmn revisit
                                     TRUE,
                                     FALSE,
                                     0,
                                     emm_ctrl_data_ptr,
                                     &reg_req_ptr->rat_pri_list_info,
                                     reg_req_ptr->type
#ifdef FEATURE_LTE_REL9
                                    ,reg_req_ptr->additional_info
#endif               
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                                    ,reg_req_ptr->csg_id
#endif 
                                    ,reg_req_ptr->scan_scope 
                                    ,reg_req_ptr->trm_timeout
#ifdef FEATURE_CIOT
				    ,reg_req_ptr->scan_info.new_scan
                                    ,reg_req_ptr->scan_info.use_timer
                                    ,reg_req_ptr->scan_info.scan_time //FRACQ
                                    ,reg_req_ptr->scan_info.nb1_acqdb_timer_status
                                    ,reg_req_ptr->scan_info.nb1_fullband_timer_status
#endif
                                    );

            EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_LIMITED_SERVICE) ;
            reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf ;
            break ;
          default:
            MSG_ERROR_DS_1(MM_SUB, "=EMM= Unknown Network Selection mode %d",
                           reg_req_ptr->network_selection_mode);
            emm_reject_service_req(emm_ctrl_data_ptr) ;
            break ;
        }
        break;
      default:
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid state %d to send cached REG REQ",
                       EMM_GET_STATE());
        emm_reject_service_req(emm_ctrl_data_ptr);
        break;
    }
  }

  emm_reg_container = (emm_reg_container_type)NULL;
} /* end of emm_process_pended_reg_req() */


void emm_process_ram_reg_req
(
  mmr_reg_req_s_type *reg_req_ptr, 
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{

  emm_failure_type emm_failure_cause;
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  switch(reg_req_ptr->network_selection_mode)
  {
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
      emm_reject_service_req(emm_ctrl_data_ptr) ;
      MSG_HIGH_DS_1(MM_SUB, "=EMM= Reject REG_REQ rcved in %u state", EMM_GET_STATE());
      break;
    case SYS_NETWORK_SELECTION_MODE_MANUAL:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
    case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
#endif 
      emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL) ;

      emm_ctrl_data_ptr->reg_req_pended_indication = TRUE ;
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      //memcpy((void *)emm_ctrl_data_ptr->reg_req_pending_ptr,(void *)reg_req_ptr, sizeof(mmr_reg_req_s_type)) ;
      mm_emm_store_reg_req(reg_req_ptr, emm_ctrl_data_ptr);

      emm_setup_reg_container(emm_process_pended_reg_req) ;

      reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf ;

      if (EMM_GET_STATE() == EMM_REGISTERED_INITIATED)
      {
        // If EMM is in REG_INIT state & a PDN_CON_REQ is still cached, then abort the attach
        // & move to DEREG_ATTACH_NEEDED state.
        emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED);
        mm_stop_timer(TIMER_T3410);
        // Remove connection release call back and clean up states.
        emm_remove_release_client(emm_attach_release_indication);
        emm_clean_msg_buffer(emm_ctrl_data_ptr);
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                            SYS_SRV_STATUS_LIMITED;
        emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                             SYS_SRV_DOMAIN_NO_SRV;
        emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;
      }
      else
      { 
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_PLMN_SEARCH);
      }
      break;
    case SYS_NETWORK_SELECTION_MODE_LIMITED:
#ifdef FEATURE_1XSRLTE
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
    case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
#endif
      emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL) ;

      reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf ;

      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_LIMITED_SERVICE) ;
      break;
    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW selection mode - %u",
                     reg_req_ptr->network_selection_mode);
      break;
  }

} /* end of emm_process_ram_reg_req() */


/*===========================================================================

FUNCTION  EMM_PROCESS_IRAT_TO_LTE_REG_REQ

DESCRIPTION
  This function processes MMR_REG_REQ with type IRAT_TO_LTE
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_HDR_TO_LTE
void emm_process_irat_to_lte_reg_req
(
  mmr_reg_req_s_type *reg_req_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  mmr_reg_cnf_s_type   *mmr_reg_cnf_ptr = NULL;   
  log_lte_nas_emm_outgoing_msg_event_type event;

  mmr_reg_cnf_ptr =(mmr_reg_cnf_s_type*) emm_modem_mem_alloc(
                         sizeof(mmr_reg_cnf_s_type),
                         emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void *)mmr_reg_cnf_ptr) ;
  MSG_HIGH_DS_0(MM_SUB, "Processing DO to LTE inter-rat REG REQ");

  emm_activate_lte();
  mm_emm_store_reg_req(reg_req_ptr, emm_ctrl_data_ptr);  
    /* 
  ** Re-init reg_service_report to emm_send_reg_ind routine 
  ** in order to start sending MMR_SERVICE_IND upon change of
  ** TAI, TAU procedure completion or any sorts of NW rejection
  */
  reg_service_report = (emm_reg_service_report_type)emm_send_reg_ind ;

  /* Configure MMR_REG_CNF primitive */
  mmr_reg_cnf_ptr->message_header.message_set = MS_MM_REG ;
  mmr_reg_cnf_ptr->message_header.message_id  = (int)MMR_REG_CNF ;

  mmr_reg_cnf_ptr->cause = MMR_CAUSE_NOT_SPECIFIED;
  emm_ctrl_data_ptr->last_reg_cause = MMR_CAUSE_NOT_SPECIFIED;

  /* Copy serving PLMN state info */
  /*Set PLMN to undefined*/
  sys_plmn_undefine_plmn_id(&emm_ctrl_data_ptr->plmn_service_state->plmn);
  emm_ctrl_data_ptr->plmn_service_state->plmn_service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;

  /* Service status to no service */
  emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_NO_SRV;
  emm_ctrl_data_ptr->plmn_service_state->active_rat = SYS_RAT_LTE_RADIO_ACCESS;
  emm_ctrl_data_ptr->plmn_service_state->no_service_cause = SYS_NO_SVC_CAUSE_IRAT_TO_LTE;
  emm_ctrl_data_ptr->plmn_service_state->suitable_search_time = 0;

  mmr_reg_cnf_ptr->service_state = *(emm_ctrl_data_ptr->plmn_service_state);
  
  /* Available PLMN List*/   
  mmr_reg_cnf_ptr->available_plmn_list.length = 0;

  /* Report Equivalent PLMN List if list is avaliable */
  mmr_reg_cnf_ptr->update_equivalent_plmn_list = 0;
  mmr_reg_cnf_ptr->equivalent_plmn_list.length = 0;
  emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
  emm_ctrl_data_ptr->reg_req_pended_indication  = FALSE;

  mmr_reg_cnf_ptr->ue_oos = FALSE;

  mmr_reg_cnf_ptr->only_for_reg =  FALSE;

  /*lint -save -e778 */
  PUT_IMH_LEN( sizeof( mmr_reg_cnf_s_type) - sizeof(IMH_T),&mmr_reg_cnf_ptr->message_header );
  /*lint -restore */

  MSG_HIGH_DS_0(MM_SUB, "=EMM= sent MMR_REG_CNF");
  MSG_HIGH_DS_6(MM_SUB, "=EMM= cause = %d sim_state = %d svc_capability = %d =EMM= plmn_forbidden = %d svc_status = %d reg_svc_domain = %d",
                mmr_reg_cnf_ptr->cause,
                mmr_reg_cnf_ptr->service_state.sim_state,
                mmr_reg_cnf_ptr->service_state.plmn_service_capability,
                mmr_reg_cnf_ptr->service_state.plmn_forbidden,
                mmr_reg_cnf_ptr->service_state.service_status,
                mmr_reg_cnf_ptr->service_state.reg_service_domain);
  MSG_HIGH_DS_6(MM_SUB, "=EMM= active_rat = %d ps_data_suspend = %d sib8_available = %d equiv PLMN update=%d list len=%d info_blocked_at_reg = %d",
                mmr_reg_cnf_ptr->service_state.active_rat,
                mmr_reg_cnf_ptr->service_state.ps_data_suspend,
                mmr_reg_cnf_ptr->service_state.sib8_available,
                mmr_reg_cnf_ptr->update_equivalent_plmn_list,
                mmr_reg_cnf_ptr->equivalent_plmn_list.length,
                mmr_reg_cnf_ptr->only_for_reg);
  
  mm_send_message( (IMH_T*)mmr_reg_cnf_ptr, GS_QUEUE_REG );
  event.emm_out_msg = (byte)MMR_REG_CNF;
  event_report_payload(EVENT_LTE_REG_OUTGOING_MSG,sizeof(event), (void*)&event);   
  emm_modem_mem_free(mmr_reg_cnf_ptr, emm_ctrl_data_ptr);
 
}

#endif

/*===========================================================================
FUNCTION    emm_process_radio_cap_update_req

DESCRIPTION
  Function process the radio capability change request sent from CM in CM service request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void emm_process_radio_cap_update_req
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  emm_state_type  emm_state = EMM_INVALID_STATE;
  lte_nas_tai_lst1_type current_tai;
  emm_failure_type    emm_failure_cause;

  
  emm_state = EMM_GET_STATE();
  MSG_HIGH_DS_2(MM_SUB, "=EMM= Radio capability update function emm_state %d radio_cap_update %d",
                emm_state,
                emm_ctrl_data_ptr->radio_cap_update);
  if(emm_ctrl_data_ptr->last_rrc_service_ind_ptr == NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function emm_process_radio_cap_update_req");
    return;
  }

  /*
    EMM needs to recover TAI info from latest RRC Service or 
    Connected Mode Cell Change indications
  */
  current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->
                                                                                 camped_svc_info.selected_plmn) ;
  current_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac ;
  switch(emm_state)
  {
    case EMM_REGISTERED:
      if(emm_ctrl_data_ptr->radio_cap_update == EMM_RADIO_CAP_UPDATE_NONE)
      {
        emm_ctrl_data_ptr->radio_cap_update = EMM_RADIO_CAP_UPDATE_PENDING;
      }
      else if(emm_ctrl_data_ptr->radio_cap_update == EMM_RADIO_CAP_UPDATE_PENDING)
      {
        emm_ctrl_data_ptr->radio_cap_update = EMM_RADIO_CAP_UPDATE_NONE;
      }

      //If UE is in updated state then abort if any TAU recovery is ongoing.
      if((emm_ctrl_data_ptr->radio_cap_update != EMM_RADIO_CAP_UPDATE_NONE) &&
         (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED))
      {
        mm_stop_timer(TIMER_T3411);
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE || defined FEATURE_CATM_LTE_EXT_GNSS_COEX || defined FEATURE_WWAN_GNSS_DYNAMIC
        mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

      }
      break;
    case EMM_SERVICE_REQUEST_INITIATED:
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      if(emm_ctrl_data_ptr->radio_cap_update == EMM_RADIO_CAP_UPDATE_NONE)
      {
        emm_ctrl_data_ptr->radio_cap_update = EMM_RADIO_CAP_UPDATE_PENDING;
      }
  
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      if(emm_state == EMM_SERVICE_REQUEST_INITIATED)
      {
        mm_stop_timer(TIMER_T3411);
		emm_failure_cause.cause.state = EMM_INVALID_STATE_TEMP;
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE || defined FEATURE_CATM_LTE_EXT_GNSS_COEX || defined FEATURE_WWAN_GNSS_DYNAMIC
        mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  TRUE, LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                  emm_failure_cause);
      }
      else
      {
        mm_stop_timer(TIMER_T3430);
		emm_failure_cause.cause.state = EMM_INVALID_STATE_REG_IN_PROGRESS;
        emm_remove_release_client(emm_tau_request_release_indication);
        if((emm_search_tai_list(&current_tai,emm_ctrl_data_ptr->emm_tai_list_ptr)) &&
           (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED))
        {
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);
        }
        else
        {
          emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE);
        }
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

    case EMM_REGISTERED_INITIATED:
      mm_stop_timer(TIMER_T3410);
      emm_remove_release_client(emm_attach_release_indication);
      emm_clean_msg_buffer(emm_ctrl_data_ptr);
      if(EMM_GET_SUBSTATE() == EMM_WAITING_FOR_ESM_RESPONSE)
      {
        /* Local detach */
        emm_build_and_send_detach_ind(EMM_DETACHED);
      }
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                            SYS_SRV_STATUS_LIMITED;
      emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                             SYS_SRV_DOMAIN_NO_SRV;
      emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);
      break;

    default:
      break;
  }
}

#ifndef FEATURE_NAS_GW
#if defined(FEATURE_NAS_REL10) || defined(FEATURE_LTE_REL10)
void emm_start_backoff_congestion_timer()
{
  if(EMM_GET_STATE() == EMM_NULL || EMM_GET_STATE() == EMM_DEREGISTERED)
  {
    if(gmm_t3346_backoff_timer_value > 0 && mm_timer_status[TIMER_T3346] == TIMER_STOPPED)
    {
           mm_start_timer(TIMER_T3346, (uint64)gmm_t3346_backoff_timer_value);
    }
    gmm_t3346_backoff_timer_value = 0;
  }
}
#endif
#endif


/*===========================================================================

FUNCTION  EMM_PROCESS_REG_REQ

DESCRIPTION
  This function processes MMR_REG_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_reg_req
(
  mm_cmd_type        *reg_cmd_ptr, 
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  mmr_reg_req_s_type        *reg_req_ptr = NULL;
  emm_state_type             emm_state = EMM_INVALID_STATE;
  emm_connection_state_type  conn_state;
  emm_mo_detach_type         detach_type = INVALID_DETACH_TYPE;
  log_lte_nas_emm_incoming_event_type  event;
  boolean radio_cap_update_flag = FALSE;
  boolean service_change_pending;

  event.emm_in_msg = (byte)MMR_REG_REQ;
  event_report_payload(EVENT_LTE_REG_INCOMING_MSG,sizeof(event),(void*)&event);

  reg_req_ptr = &reg_cmd_ptr->cmd.mmr_reg_req ;

  radio_cap_update_flag = emm_bit_mask_check(reg_req_ptr->additional_info,
                                (byte)SYS_ADDTNL_INFO_FORCE_RADIO_CAP_UPDATE);

  MSG_HIGH_DS_4(MM_SUB, "=EMM= MMR_REG_REQ - Srv Domain %d, NW Sel Mode %d, Type %d, PSM restore done %d",
                reg_req_ptr->req_service_domain,
                reg_req_ptr->network_selection_mode,
                reg_req_ptr->type,
                emm_ctrl_data_ptr->psm_restore_done);

#ifdef FEATURE_LTE_REL9
  MSG_HIGH_DS_1(MM_SUB, "=EMM= MMR_REG_REQ - Addtnl info 0x%x",
                reg_req_ptr->additional_info);
#endif

#ifdef FEATURE_NAS_GW
    /*Clear MM serving LAI */
  mm_clear_serving_lai_on_moving_to_lte();
#endif
  
  /* MNK: Store it for GW side*/
  mm_serving_plmn.ms_op_mode = reg_req_ptr->req_service_domain;
  
  NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);

  /* Copy the RAT pri list to the global variable first, as mm_activate_gw uses it*/
  mm_rat_pri_list_info = reg_req_ptr->rat_pri_list_info;
  
  NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);
#ifdef FEATURE_NAS_GW
  /* Call GW blindly...*/
  mm_activate_gw();
#else
#if defined(FEATURE_NAS_REL10) || defined(FEATURE_LTE_REL10)
  emm_start_backoff_congestion_timer();
#endif
#endif

#ifdef FEATURE_HDR_TO_LTE
  if(reg_req_ptr->type == MMR_REG_REQ_IRAT_TO_LTE)
  {
    emm_process_irat_to_lte_reg_req(reg_req_ptr, emm_ctrl_data_ptr);
    /* we get SERVICE_IND directly without ACT_IND */
    /* allocate the memory for ctrl data variables */
    emm_procedure_message_mem_alloc(emm_ctrl_data_ptr);
    return;  
  }
#endif
  emm_store_attach_ind_status_type(reg_req_ptr );
#ifdef FEATURE_NAS_GW
  if(mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY
#ifdef FEATURE_SGLTE
    && (!MM_IS_IN_SGLTE_MODE)
#endif  
    )              
  {
    emm_set_possible_imsi_attach(TRUE);
  }
#endif
#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED) || !defined(FEATURE_NAS_DISABLE_VOICE)
#ifdef FEATURE_LTE_REL9
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9) &&
     (reg_cmd_ptr->cmd.mmr_reg_req.lte_cs_domain_param.is_voice_domain_ie_valid == TRUE) &&
     ((reg_cmd_ptr->cmd.mmr_reg_req.lte_cs_domain_param.voice_domain_pref == 
                                         SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED) ||
      (reg_cmd_ptr->cmd.mmr_reg_req.lte_cs_domain_param.voice_domain_pref == 
                                         SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED)))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Setting PS voice on EUTRAN supported bit to TRUE voice_domain_pref = %d",
                  reg_cmd_ptr->cmd.mmr_reg_req.lte_cs_domain_param.voice_domain_pref);
    emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_ps_voice_on_eutran_supported = TRUE;
  }
#endif
#endif
#ifdef FEATURE_LTE_REL9
  if(reg_req_ptr->type == MMR_REG_REQ_SRV_MODE_UPDATE)
  {
    mm_emm_store_reg_req(reg_req_ptr, emm_ctrl_data_ptr);
    emm_send_rrc_nw_sel_mode_reset_req(reg_req_ptr->network_selection_mode,
                                      &reg_req_ptr->rat_pri_list_info, 
                                       emm_ctrl_data_ptr);
    if(EMM_GET_STATE() == EMM_DEREGISTERED)
    {
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED);
    }
    else
    {
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_PLMN_SEARCH);
    }
    reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf;

    emm_rrc_process_service_indication(emm_ctrl_data_ptr->last_rrc_service_ind_ptr,
                                        emm_ctrl_data_ptr);
    return;  
  }
#endif

#ifdef FEATURE_CIOT
  if(emm_bit_mask_check(reg_req_ptr->additional_info,(byte)SYS_ADDTNL_INFO_START_MO_EXCEPTION) &&
	  (emm_is_mo_excep_nas_config_enabled() == TRUE ) &&
	  (emm_is_mo_exception_barred() == FALSE) && (emm_is_t3446_started_for_mo_exception() == FALSE))
	{
	  emm_ctrl_data_ptr->emm_start_mo_excp_signalling = TRUE; 
	  MSG_HIGH_DS_1(MM_SUB, "=EMM= emm_start_mo_excp_signalling is set to - %d ",
                emm_ctrl_data_ptr->emm_start_mo_excp_signalling);
	}
  
#endif

  emm_state = EMM_GET_STATE();

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

  if((emm_already_camped(reg_req_ptr, emm_ctrl_data_ptr) == TRUE)
#ifdef FEATURE_ENHANCED_NW_SELECTION
     &&(reg_req_ptr->type != MMR_REG_REQ_EF_RAT_REFRESH)
#endif
    )
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= UE is already camped");

    if(reg_req_ptr->network_selection_mode != 
                     emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
    {
      emm_send_rrc_nw_sel_mode_reset_req(reg_req_ptr->network_selection_mode,
                                         NULL, 
                                         emm_ctrl_data_ptr);
    }

    mm_emm_store_reg_req(reg_req_ptr, emm_ctrl_data_ptr);

    switch(emm_ctrl_data_ptr->plmn_service_state->reg_service_domain)
    {
      case SYS_SRV_DOMAIN_CS_PS:
        switch(reg_req_ptr->req_service_domain)
        { 
          case SYS_SRV_DOMAIN_CS_PS:
            if((emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED)||
               (emm_state == EMM_REGISTERED_INITIATED))
            {
              reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf;
            }
            else
            {
               emm_send_reg_cnf(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
            }
            break;
          case SYS_SRV_DOMAIN_PS_ONLY:
#ifdef FEATURE_NAS_GW
            mm_pending_lu_request = NO_LU;
#endif
			detach_type = IMSI_DETACH_MO_DETACH;
            break;
          case SYS_SRV_DOMAIN_CS_ONLY:
            detach_type = EPS_DETACH;
            break;
          default:
            detach_type = COMBINED_EPS_DETACH;
            break;
        }

        if(detach_type != INVALID_DETACH_TYPE)
        {
          

#ifdef FEATURE_LTE_REL13
          if(emm_pend_mo_detach(
                  detach_type,
                  emm_ctrl_data_ptr) == FALSE)
#endif
          {
            reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf;
            emm_start_mo_detach(detach_type, 
                                FALSE, 
                                NULL, 
                                emm_detach_failed_routine, 
                                emm_ctrl_data_ptr);
          }
        }
        break;
      case SYS_SRV_DOMAIN_PS_ONLY:
        switch(reg_req_ptr->req_service_domain)
        { 
          case SYS_SRV_DOMAIN_CS_PS:
            if((emm_is_combined_reg_allowed(emm_ctrl_data_ptr) == TRUE) &&
               (service_change_pending == TRUE))
            {
              reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf;
              emm_initiate_tau_procedure(EMM_INITIAL_UPDATE, 
                                         COMBINED_TA_LA_UPDATING_IMSI_ATTACH, 
                                         FALSE, 
                                         emm_ctrl_data_ptr);
            }
            else if((emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED)||
                    (emm_state == EMM_REGISTERED_INITIATED))
            {
              reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf;
            }
            else
            {
              emm_send_reg_cnf(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
            }
            break;
          case SYS_SRV_DOMAIN_PS_ONLY:
            if((emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED)||
               (emm_state == EMM_REGISTERED_INITIATED))
            {
              reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf;
            }
            else
            {
              emm_send_reg_cnf(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
            }
            break;
          default:
#ifdef FEATURE_LTE_REL13
           if(emm_pend_mo_detach(
                   EPS_DETACH,
                   emm_ctrl_data_ptr) == FALSE)
#endif
            {
              reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf;
              emm_start_mo_detach(EPS_DETACH, 
                                  FALSE, 
                                  NULL, 
                                  emm_detach_failed_routine, 
                                  emm_ctrl_data_ptr);
            }
            break;
        }
        break;

      case SYS_SRV_DOMAIN_CS_ONLY:
        reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf;
        break;

      default:
        if(emm_state != EMM_REGISTERED_INITIATED)
        {
          MSG_FATAL_DS(MM_SUB,"=EMM= Camped in invalid service domain -%u", 
                  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain,0,0);
        }
        else
        {
          reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf;
        }
        break;
    }
  }
  else
  {
    /* Set active rat to LTE */
#ifdef FEATURE_CIOT
     emm_ctrl_data_ptr->plmn_service_state->active_rat = reg_req_ptr->rat;
#else
    emm_ctrl_data_ptr->plmn_service_state->active_rat = SYS_RAT_LTE_RADIO_ACCESS;
#endif


    if((reg_req_ptr->req_service_domain != SYS_SRV_DOMAIN_PS_ONLY) &&
       (reg_req_ptr->req_service_domain != SYS_SRV_DOMAIN_CS_PS))
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= Rejecting REG REQ w/ invalid service domain -%u ",
                    reg_req_ptr->req_service_domain);
      emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_NO_SRV;
      emm_ctrl_data_ptr->plmn_service_state->no_service_cause = SYS_NO_SVC_CAUSE_NORMAL;
      emm_ctrl_data_ptr->plmn_service_state->plmn.identity[0] = 0xFF;
      emm_ctrl_data_ptr->plmn_service_state->plmn.identity[1] = 0xFF;
      emm_ctrl_data_ptr->plmn_service_state->plmn.identity[2] = 0xFF;

      emm_send_reg_cnf(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
    }
    else
    {
      conn_state = EMM_GET_CONNECTION_STATE();

      /* No need to send service request to RRC if all following conditions  
         are meet
         1. UE is connected or going to be connected
         2. REG CNF is already pending
         3. RRC is already camped on requested PLMN  
         4. No changes between new REG REQ and pending REG REQ */
      if(((conn_state == EMM_CONNECTED_STATE)||
          (conn_state == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE)) 
         &&
         (reg_service_report == (emm_reg_service_report_type)emm_send_reg_cnf)
         &&
         (emm_ctrl_data_ptr->rrc_active == TRUE)
         &&
         (emm_ctrl_data_ptr->last_rrc_service_ind_ptr->svc_status == 
                                                  LTE_RRC_SVC_STATUS_AVAILABLE)
         &&
         (sys_plmn_match(emm_ctrl_data_ptr->plmn_service_state->plmn,
                         reg_req_ptr->plmn) == TRUE)
         &&
         ((reg_req_ptr->network_selection_mode != 
                                          SYS_NETWORK_SELECTION_MODE_MANUAL) ||
          (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode 
                                         == SYS_NETWORK_SELECTION_MODE_MANUAL))
         &&
         (reg_req_ptr->req_service_domain == 
                         emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain)
         &&
         (reg_req_ptr->sms_only == emm_ctrl_data_ptr->reg_req_pending_ptr->sms_only)
         &&
         (mm_rat_pri_list_match(&reg_req_ptr->rat_pri_list_info,
               &emm_ctrl_data_ptr->reg_req_pending_ptr->rat_pri_list_info) == TRUE)

#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED) || !defined(FEATURE_NAS_DISABLE_VOICE)
         &&
         (emm_lte_cs_domain_param_match(reg_req_ptr->lte_cs_domain_param,
                                  *(emm_ctrl_data_ptr->reg_lte_pended_cs_domain_param_ptr))
                == TRUE)
#endif
         )
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= REG CNF already pending - Dont send RRC SRV REQ");
        mm_emm_store_reg_req(reg_req_ptr, emm_ctrl_data_ptr);
        return;
      }

      emm_ctrl_data_ptr->reg_req_pended_indication = TRUE ;
      mm_emm_store_reg_req(reg_req_ptr, emm_ctrl_data_ptr);

      switch(reg_req_ptr->network_selection_mode)
      {
        case SYS_NETWORK_SELECTION_MODE_MANUAL:
        case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
          if(reg_req_ptr->type == MMR_REG_REQ_USER_SELECTION)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Manual user PLMN selection in process");
            emm_ctrl_data_ptr->manual_user_sel = TRUE;
#ifdef FEATURE_NAS_GW
            emm_set_mm_manual_selection_flag(TRUE);
            gmm_delete_gprs_forbidden_rai();
#endif
            if(sys_eplmn_list_validate(FALSE) == TRUE)
            {
              MSG_HIGH_DS_0(MM_SUB, "=EMM= EPLMN list invalidated - Manual user sel");
              emm_send_rrc_system_update(emm_ctrl_data_ptr);              
#ifdef FEATURE_NAS_GW
              /* Update G RR and W RRC */
              mm_send_rrc_eq_plmn_list_change_ind();
              mm_send_rr_eq_plmn_list_change_ind(); 
#endif
            }
          }
          break;
        case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
        case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
#ifdef FEATURE_1XSRLTE
        case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
#endif 
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
        case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
#endif 
          if(sys_eplmn_list_validate(TRUE) == TRUE)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= EPLMN list validated - New REG REQ");
            emm_send_rrc_system_update(emm_ctrl_data_ptr);
#ifdef FEATURE_NAS_GW
            /* Update G RR and W RRC */
            mm_send_rrc_eq_plmn_list_change_ind();
            mm_send_rr_eq_plmn_list_change_ind();         
#endif
          }
          break;
        default:
          break;
      }

      if((emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == TRUE) ||
         (emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == TRUE))
      {
        MSG_HIGH_DS_3(MM_SUB, "=EMM= Deacting RRC (%d, %d, %d) - Process SRV REQ later",
                      emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending,
                      emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending,
                      emm_ctrl_data_ptr->rrc_deact_req.deact_reason);

        if(emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == TRUE)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= SRV REQ calls off STOP MODE REQ processing");
          emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = FALSE;
          emm_ctrl_data_ptr->rrc_deact_req.deact_reason = 
                                                     SYS_STOP_MODE_REASON_NONE;
        }
        emm_setup_reg_container(emm_process_pended_reg_req);
        return;
      }

      reg_service_report = (emm_reg_service_report_type)NULL ;
      if(radio_cap_update_flag == TRUE)
      {
        emm_process_radio_cap_update_req(emm_ctrl_data_ptr);
      }
#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED) || !defined(FEATURE_NAS_DISABLE_VOICE)
      /* Update pended lte_cs_domain_param in service request so that if new service request come, 
         we can check if there is any voice domain preference change
      */
      *(emm_ctrl_data_ptr->reg_lte_pended_cs_domain_param_ptr) = 
				   emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param;   
#endif
      if((conn_state == EMM_CONNECTED_STATE)||
         (conn_state == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE))
      {
        if((emm_ctrl_data_ptr->rlf_state == TRUE) &&
            (radio_cap_update_flag == FALSE))
        {
#ifdef FEATURE_SGLTE
          if( MM_IS_IN_SGLTE_MODE && (!MM_IS_IN_SGLTE_STATE_4) && (!sys_plmn_match(emm_ctrl_data_ptr->rlf_plmn,reg_req_ptr->plmn)))
          {
            emm_rrc_abort_connection(emm_ctrl_data_ptr, 
                                     FALSE,
                                     LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
            emm_setup_reg_container(emm_process_pended_reg_req);
            MSG_HIGH_DS_0(MM_SUB, "=EMM= pending REG REQ and aborting RLF");
          }
          else
#endif
          {
            /* we could get MMR REG REQ in connected state for ALT SCAN*/
            /* allocate the memory for ctrl data variables */
            emm_procedure_message_mem_alloc(emm_ctrl_data_ptr);

            emm_send_rrc_service_req(reg_req_ptr->network_selection_mode,
                                   TRUE,
                                   &reg_req_ptr->plmn,
                                   TRUE,
                                   TRUE,
                                   FALSE, // e-plmn revisit
                                   TRUE,
                                   FALSE,
                                   0,
                                   emm_ctrl_data_ptr,
                                   &reg_req_ptr->rat_pri_list_info,
                                   reg_req_ptr->type
  #ifdef FEATURE_LTE_REL9
                                  ,reg_req_ptr->additional_info
  #endif                         
  #if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                                  ,reg_req_ptr->csg_id
  #endif 
                                  ,reg_req_ptr->scan_scope 
                                  ,reg_req_ptr->trm_timeout
#ifdef FEATURE_CIOT
					,reg_req_ptr->scan_info.new_scan
                                        ,reg_req_ptr->scan_info.use_timer
                                        ,reg_req_ptr->scan_info.scan_time //FRACQ
                                        ,reg_req_ptr->scan_info.nb1_acqdb_timer_status
                                        ,reg_req_ptr->scan_info.nb1_fullband_timer_status
#endif
                                  );
          }
        }
        else
        {
          /* CCO inactive check is to cover for scenario where when service req on LTE is pending 
                and we receive stop cnf from GSM before CCO Complete indication from LTE.
                With this change we will send service request to LTE on cco complete indication*/
          if((mm_timer_status[TIMER_T3440] == TIMER_STOPPED)
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_IRAT_DISABLED)
            &&
           (emm_ctrl_data_ptr->l2gcco_state == EMM_LTOG_CCO_INACTIVE)
#endif
           )
          {
#ifdef FEATURE_NAS_GW
            /* If we dont set this flag, and we recive act ind from GW,
              then NAS will try to activate both LTE and GW */
            emm_ctrl_data_ptr->ignore_GW_activation_ind = TRUE;
#endif
            //If service change is pending then abort any ongoing attach or tau procedures
            if(service_change_pending)
            {
              emm_abort_attach_procedure();
              emm_abort_tau_procedure();
            }
            emm_rrc_abort_connection(emm_ctrl_data_ptr, 
                                     FALSE,
                                     LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
          }
          emm_setup_reg_container(emm_process_pended_reg_req);
        }
        reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf ;
      }
      else if(conn_state == EMM_RELEASING_RRC_CONNECTION_STATE && emm_ctrl_data_ptr->aborting_conn == TRUE)
      {
        /*EMM is getting service request while waiting for connection release from LRRC, 
        **save the new service request to be processed on connection release */
        emm_setup_reg_container(emm_process_pended_reg_req);
        reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf ;
      }
      else
      {
        /* allocate the memory for ctrl data variables */
        emm_procedure_message_mem_alloc(emm_ctrl_data_ptr);

        switch (emm_state)
        {
          case EMM_NULL:
          case EMM_DEREGISTERED:
            switch (reg_req_ptr->network_selection_mode)
            {
              case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
              case SYS_NETWORK_SELECTION_MODE_MANUAL:
              case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
              case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
#ifdef FEATURE_1XSRLTE
              case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
              case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
#endif 
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
              case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
#endif 
#ifdef FEATURE_FMD_SPEED_INFO
if(reg_req_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
{
	mm_stop_timer(TIMER_FMD_PSM_FALLBACK);
}
#endif
                if((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                SYS_SIM_STATE_NOT_AVAILABLE) ||
                   (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                     SYS_SIM_STATE_PS_INVALID))
                {
                  emm_reject_service_req(emm_ctrl_data_ptr) ;
                }
                else
                {                  
                  emm_send_rrc_service_req(reg_req_ptr->network_selection_mode,
                                           TRUE,
                                           &reg_req_ptr->plmn,
                                           TRUE,
                                           TRUE,
                                           FALSE, // e-plmn revisit
                                           TRUE,
                                           FALSE,
                                           0,
                                           emm_ctrl_data_ptr,
                                           &reg_req_ptr->rat_pri_list_info,
                                           reg_req_ptr->type
#ifdef FEATURE_LTE_REL9
                                          ,reg_req_ptr->additional_info
#endif                       
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                                          ,reg_req_ptr->csg_id
#endif 
                                          ,reg_req_ptr->scan_scope  
                                          ,reg_req_ptr->trm_timeout
#ifdef FEATURE_CIOT
					,reg_req_ptr->scan_info.new_scan
                                        ,reg_req_ptr->scan_info.use_timer
                                        ,reg_req_ptr->scan_info.scan_time //FRACQ
                                        ,reg_req_ptr->scan_info.nb1_acqdb_timer_status
                                        ,reg_req_ptr->scan_info.nb1_fullband_timer_status
#endif
                                          );

#ifdef FEATURE_1XSRLTE
                  if((reg_req_ptr->network_selection_mode == 
                                   SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
                     (reg_req_ptr->network_selection_mode == 
                                   SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY))
                  {
                    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE) ;
                  }
                  else
#endif 
                  {
                    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED);
                  }

                  reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf ;
                }
                break;
              case SYS_NETWORK_SELECTION_MODE_LIMITED:
#ifdef FEATURE_FMD_SPEED_INFO
			  if(emm_bit_mask_check(emm_ctrl_data_ptr->reg_req_pending_ptr->additional_info,(byte)SYS_ADDTNL_INFO_PSEUDO_ONLINE_MODE) == TRUE)
				{
				 	mm_stop_timer(TIMER_FMD_PSM_FALLBACK);
	                MSG_HIGH_DS_1(REG_SUB, "=MM= Stopping fmd PSM fallback timer %d", emm_ctrl_data_ptr->fmd_psm_fallback_timer);
				}
#endif
                emm_send_rrc_service_req(reg_req_ptr->network_selection_mode,
                                         FALSE,
                                         &reg_req_ptr->plmn,
                                         TRUE,
                                         TRUE,
                                         FALSE, // e-plmn revisit
                                         TRUE,
                                         FALSE,
                                         0,
                                         emm_ctrl_data_ptr,
                                         &reg_req_ptr->rat_pri_list_info,
                                         reg_req_ptr->type
#ifdef FEATURE_LTE_REL9
                                        ,reg_req_ptr->additional_info
#endif                        
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                                        ,reg_req_ptr->csg_id
#endif 
                                        ,reg_req_ptr->scan_scope
                                        ,reg_req_ptr->trm_timeout
#ifdef FEATURE_CIOT
					,reg_req_ptr->scan_info.new_scan
                                        ,reg_req_ptr->scan_info.use_timer
                                        ,reg_req_ptr->scan_info.scan_time //FRACQ
                                        ,reg_req_ptr->scan_info.nb1_acqdb_timer_status
                                        ,reg_req_ptr->scan_info.nb1_fullband_timer_status
#endif
                                        );

                  reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf ;

                  EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE) ;
                  /*Do not send LIMITED_SEVICE event to GSTK*/
                break ;
              default:
                MSG_ERROR_DS_1(MM_SUB, "=EMM= Unknown Network Selection mode (%d)",
                               reg_req_ptr->network_selection_mode);
                emm_reject_service_req(emm_ctrl_data_ptr) ;
                break ;
            }
            //if( mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_UMTS_RADIO_ACCESS) || mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_GSM_RADIO_ACCESS) )
            //{
              /* MNK: Irrespective of what hapened(service_req processed or rejected) above call this for now. */
             // mm_activate_gw();
            //}

            break ;
          case EMM_REGISTERED:
            /* New system, clear ongoing detach flag to avoid the detach retry in
               case of there is a previously access barred detach */
            emm_ctrl_data_ptr->ongoing_detach_procedure = FALSE;

            switch (reg_req_ptr->network_selection_mode)
            {
              case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
              case SYS_NETWORK_SELECTION_MODE_MANUAL:
              case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
              case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
#ifdef FEATURE_1XSRLTE
              case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
              case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
#endif 
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
              case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
#endif 

#ifdef FEATURE_ENHANCED_NW_SELECTION
                emm_ctrl_data_ptr->refresh_pending = TRUE;
#endif  
                emm_send_rrc_service_req(reg_req_ptr->network_selection_mode,
                                         TRUE,
                                         &reg_req_ptr->plmn,
                                         TRUE,
                                         TRUE,
                                         FALSE, // e-plmn revisit
                                         TRUE,
                                         FALSE,
                                         0,
                                         emm_ctrl_data_ptr,
                                         &reg_req_ptr->rat_pri_list_info,
                                         reg_req_ptr->type
#ifdef FEATURE_LTE_REL9
                                        ,reg_req_ptr->additional_info
#endif                
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                                        ,reg_req_ptr->csg_id
#endif
                                        ,reg_req_ptr->scan_scope  
                                        ,reg_req_ptr->trm_timeout
#ifdef FEATURE_CIOT
					,reg_req_ptr->scan_info.new_scan
                                        ,reg_req_ptr->scan_info.use_timer
                                        ,reg_req_ptr->scan_info.scan_time //FRACQ
                                        ,reg_req_ptr->scan_info.nb1_acqdb_timer_status
                                        ,reg_req_ptr->scan_info.nb1_fullband_timer_status
#endif
                                        );
#ifdef FEATURE_1XSRLTE
                if((reg_req_ptr->network_selection_mode == 
                                 SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
                   (reg_req_ptr->network_selection_mode == 
                                 SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY))
                {
                  EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_LIMITED_SERVICE) ;
                }
                else
#endif 
                {
                  EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_PLMN_SEARCH);
                }
              
                reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf ;
                break ;
             case SYS_NETWORK_SELECTION_MODE_LIMITED:
                emm_send_rrc_service_req(reg_req_ptr->network_selection_mode,
                                         FALSE,
                                         &reg_req_ptr->plmn,
                                         TRUE,
                                         TRUE,
                                         FALSE, // e-plmn revisit
                                         TRUE,
                                         FALSE,
                                         0,
                                         emm_ctrl_data_ptr,
                                         &reg_req_ptr->rat_pri_list_info,
                                         reg_req_ptr->type
#ifdef FEATURE_LTE_REL9
                                        ,reg_req_ptr->additional_info
#endif           
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
                                        ,reg_req_ptr->csg_id
#endif 
                                        ,reg_req_ptr->scan_scope
                                        ,reg_req_ptr->trm_timeout
#ifdef FEATURE_CIOT
					,reg_req_ptr->scan_info.new_scan
                                        ,reg_req_ptr->scan_info.use_timer
                                        ,reg_req_ptr->scan_info.scan_time //FRACQ
                                        ,reg_req_ptr->scan_info.nb1_acqdb_timer_status
                                        ,reg_req_ptr->scan_info.nb1_fullband_timer_status
#endif
                                        );
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

                if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
                {
                  EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_PLMN_SEARCH);
                }
                else
#endif
#endif
                {
                  EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_LIMITED_SERVICE);
                }
                
                reg_service_report = (emm_reg_service_report_type)emm_send_reg_cnf ;
                break ;
              default:
                MSG_ERROR_DS_1(MM_SUB, "=EMM= Unknown Network Selection mode (%d)",
                               reg_req_ptr->network_selection_mode);
                emm_reject_service_req(emm_ctrl_data_ptr) ;
                break ;
            }

            break ;
          case EMM_REGISTERED_INITIATED:
          case EMM_TRACKING_AREA_UPDATING_INITIATED:
          case EMM_SERVICE_REQUEST_INITIATED:
            emm_process_ram_reg_req(reg_req_ptr, emm_ctrl_data_ptr) ;
            break ;
          case EMM_DEREGISTERED_INITIATED:
            emm_reject_service_req(emm_ctrl_data_ptr) ;

            MSG_HIGH_DS_0(MM_SUB, "=EMM= REG_REQ is received in DEREGISTERED_INITIATED state, rejected");
            break ;
          default:
            MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM state %u",(dword)emm_state,0,0) ;
            break ;
        }
      }
    }
  }
  
} /* end of emm_process_reg_req() */

/*===========================================================================

FUNCTION EMM_PROCESS_ABORT_PLMNSEARCH

DESCRIPTION
  This function processes MMR_PLMN_SEARCH_ABORT_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_abort_plmnsearch
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  log_lte_nas_emm_incoming_event_type  event;

#ifdef FEATURE_NAS_GW
  mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_NONE; 
#endif

#ifdef FEATURE_LTE
#ifdef FEATURE_CIOT
      emm_ctrl_data_ptr->reg_list_pended_indication = FALSE;
#endif
#endif


  MSG_HIGH_DS_0(MM_SUB, "=EMM= Rcved MMR_PLMN_SEARCH_ABORT_REQ");

  event.emm_in_msg = (byte)MMR_PLMN_SEARCH_ABORT_REQ;
  event_report_payload(EVENT_LTE_REG_INCOMING_MSG,sizeof(event),(void*)&event);

  emm_send_rrc_plmn_search_abort_req();

} /* end of emm_abort_plmnsearch() */

/*===========================================================================

FUNCTION  EMM_COMPLETE_POWEROFF_DETACH

DESCRIPTION
  This function handles RRC L2 call back upon successful RRC_DATA_CNF or 
  RRC_EST_CNF for power off detach procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_complete_poweroff_detach
(
  dword trans_id
)
{
  emm_ctrl_data_type        *emm_ctrl_data_ptr;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();  
  emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);

  /* Move to this state as we are aborting the RRC connection, even though we are going to wait for 
  abort confirmation*/
  EMM_MOVES_TO_DEREGISTERED_STATE((emm_substate_type)EMM_DEREGISTERED_NO_CELL_AVAILABLE);
  /*Do not send the LIMITED_SERVICE event to GSTk*/
  emm_reset_poweroff_detach_params(emm_ctrl_data_ptr);
} /* end of emm_complete_poweroff_detach() */

/*===========================================================================

FUNCTION  EMM_COMPLETE_POWEROFF_DETACH_FAILED

DESCRIPTION
  This function processes the transmission failures or lower layer failures in
  a poweroff MO detach procedure. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE   - Need to start Tracking Area Updating procedure
  FALSE  - No need to start Tracking Area Updating procedure

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_complete_poweroff_detach_failed
(
  lte_emm_connection_cause_type  cause,
  dword                          trans_id,
  emm_failure_type               emm_failure_cause,
  lte_nas_tai_lst1_type          tac,
  struct emm_pended_msg_list_tag *message_ptr,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  emm_pended_msg_list_type *pended_message_ptr = (struct emm_pended_msg_list_tag *)message_ptr ;
   

  /* Remove connection release CB function */
  emm_remove_release_client(emm_poweroff_detach_release_ind);

  /* Deactivate RRC if necessary. Switch off detach conn release handler may 
     have already started RRC deactivation. */
  if((emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == FALSE) &&
     (emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == FALSE)) 
  {
    emm_init_rrc_deact_req(emm_ctrl_data_ptr->rrc_deact_req.deact_reason, 
                           emm_ctrl_data_ptr);
  }

  pended_message_ptr->message_sent_indicator = TRUE ;
  
  EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
  emm_reset_poweroff_detach_params(emm_ctrl_data_ptr);
  return FALSE ;
} /* end of emm_complete_poweroff_detach_failed() */

/*===========================================================================

FUNCTION  EMM_PROCESS_STOP_MODE_REQ

DESCRIPTION
  This function processes STOP_MODE_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_stop_mode_req
(
  mm_cmd_type              *reg_cmd_ptr, 
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{
  log_lte_nas_emm_incoming_event_type  event;
  emm_state_type                       emm_state = EMM_INVALID_STATE;
  emm_failure_type                     emm_failure_cause;
  unsigned long long curr_time = 0;
  
  ASSERT(reg_cmd_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->is_sim_ps_invalid_due_to_dos != NULL); 
  ASSERT(emm_ctrl_data_ptr->is_sim_cs_invalid_due_to_dos != NULL);
  

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  emm_state = EMM_GET_STATE();

  MSG_HIGH_DS_2(MM_SUB, "=EMM= Rcved MMR_STOP_MODE_REQ w/ reason %u in EMM state %u",
                reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason,
                emm_state);

  
  if (mm_get_ulog_handle() != NULL)
  { 
    curr_time = time_get_ms_native();
    ULOG_RT_PRINTF_2(mm_get_ulog_handle(), "=EMM= Rcved MMR_STOP_MODE_REQ w/ reason , cause = %d at time %llu ",reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason, curr_time);
  }
  
  event.emm_in_msg = (byte)MMR_STOP_MODE_REQ;
  event_report_payload(EVENT_LTE_REG_INCOMING_MSG,sizeof(event),(void*)&event);

  emm_reg_container = (emm_reg_container_type)NULL;

  if( ( (reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_LPM)||
        (reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_POWER_OFF) ) &&
      (mm_get_t3247_behavior_enabled() == TRUE) )
  {
      if ( (reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_LPM) && 
           ( (*emm_ctrl_data_ptr->is_sim_cs_invalid_due_to_dos || *emm_ctrl_data_ptr->is_sim_ps_invalid_due_to_dos) &&
	     ( (emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_NOT_AVAILABLE) ||
	       (emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_CS_PS_INVALID) ) ) )
      {
	mmgsdi_session_manage_illegal_subscription(mm_sim_mmgsdi_info.session_id,MMGSDI_SESSION_APP_IS_LEGAL);
      }

      mm_emm_clear_dos_counters();
  }
    /* NAS to inform high priority indication for Stop mode procedures 
  ** like OTA PS Detach, Deactivation, RRC connection abort to
  ** prioritize WWAN tech over GPS.*/

#ifdef FEATURE_WWAN_GNSS_DYNAMIC
  if ((gmm_check_wwan_tech_active() == FALSE) &&
       (emm_state != EMM_NULL))
  {
    emm_send_rrc_wwan_set_priority(TRUE);
    gmm_set_wwan_resumption_req(TRUE);
  }
#endif

  switch(emm_state)
  {
    case EMM_NULL:
      /* Since we have not even started. We don't need to deactivate RRC. 
         Send STOP_MODE_CNF right away */
      if(emm_ctrl_data_ptr->mm_send_mmr_stop_mode_cnf_fp != NULL)
      {
        emm_ctrl_data_ptr->mm_send_mmr_stop_mode_cnf_fp(); 
      }
      else
      {
        MSG_ERROR_DS_0(MM_SUB, "=EMM= STOP_MODE_CNF function pointer is NULL. That is BAD and Shouldn't happen");
      }
      /* Nothing to do */
      break ;
    case EMM_DEREGISTERED:
      if((emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == TRUE) &&
            (emm_ctrl_data_ptr->rrc_deact_req.deact_reason != 
                reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason))
      {
        /* Stop mode collided with L to GW RAT change deactivation. If stop mode 
           cause is different from rat change deact cause, cache the deact req to 
           deactivate RRC one more time with new cause */    
        emm_ctrl_data_ptr->rrc_deact_req.deact_reason = 
                         reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason;
        emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = TRUE;    
      }
      else if(reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason == 
                                             SYS_STOP_MODE_REASON_LOCAL_DETACH)
      {
        emm_init_rrc_deact_req(SYS_STOP_MODE_REASON_MODE_CHANGE,
                               emm_ctrl_data_ptr);
      }
      else
      {
        emm_init_rrc_deact_req(reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason,
                               emm_ctrl_data_ptr);
      }
      break ;
    /* EMM_REGISTERED_INITIATED
       EMM_REGISTERED
       EMM_TRACKING_AREA_UPDATING_INITIATED
       EMM_SERVICE_REQUEST_INITIATED
       EMM_DEREGISTERED_INITIATED */
    default:
      /* Stop any ongoing procedure first */
      switch(emm_state)
      {
        case EMM_SERVICE_REQUEST_INITIATED:
        case EMM_TRACKING_AREA_UPDATING_INITIATED:
		  if((emm_ctrl_data_ptr->rlf_state == TRUE) &&
             (reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_ALT_SCAN ))
          {
            MSG_HIGH_DS_0(MM_SUB,"=EMM= RLF happened in TAU/Service Request procedure and keep it intact");
          }
          else
          {
            emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
            if(emm_state == EMM_SERVICE_REQUEST_INITIATED)
            {
              mm_stop_timer(TIMER_T3411);
              mm_stop_timer(TIMER_T3402);
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE || defined FEATURE_CATM_LTE_EXT_GNSS_COEX || defined FEATURE_WWAN_GNSS_DYNAMIC
              mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

              /* 5.6.1.6 Abnormal case g in UE. Abort service request and start detach
                 Keep the connection and notify ESM if necessary */
              emm_abort_service_request(emm_ctrl_data_ptr, 
                                        FALSE, 
                                        TRUE, 
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
          }
          break;
        case EMM_REGISTERED_INITIATED:
		   if((emm_ctrl_data_ptr->rlf_state == TRUE) &&
              (reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_ALT_SCAN ))
          {
            MSG_HIGH_DS_0(MM_SUB,"=EMM= RLF happened in attach procedure and keep it intact");
          }
		  else 
		  {
            emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
            mm_stop_timer(TIMER_T3410);
            emm_remove_release_client(emm_attach_release_indication);
            if(EMM_GET_SUBSTATE() == EMM_WAITING_FOR_NW_RESPONSE)
            {
              emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
            }
            else
            {
              emm_build_and_send_detach_ind(EMM_DETACHED);
            }
            EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
            emm_clean_msg_buffer(emm_ctrl_data_ptr);
		  }
          break;      
        case EMM_DEREGISTERED_INITIATED:
          emm_abort_mo_detach(emm_ctrl_data_ptr, FALSE);
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
          emm_clean_msg_buffer(emm_ctrl_data_ptr);
          break;
        case EMM_REGISTERED:
          break;
        default:
          break;
      }

      /* Start detach or send RRC deactivate req */
      switch(reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason)
      {
        case SYS_STOP_MODE_REASON_IRAT:
        case SYS_STOP_MODE_REASON_ALT_SCAN:
#ifdef FEATURE_DUAL_SIM
        case SYS_STOP_MODE_REASON_DSDS_SUSPEND:
#endif
#if defined FEATURE_SGLTE || defined FEATURE_GPS_WWAN_PRIORITY_SEARCH
        case SYS_STOP_MODE_REASON_SUSPEND_STACK:
#endif
        case SYS_STOP_MODE_REASON_DEEP_SLEEP:
#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE
        case SYS_STOP_MODE_REASON_UE_MODE_CHANGE:
#endif
        case SYS_STOP_MODE_REASON_DUAL_SWITCH:
          if((emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == TRUE) &&
                (emm_ctrl_data_ptr->rrc_deact_req.deact_reason != 
                    reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason))
          {
            /* Stop mode collided with L to GW RAT change deactivation. If stop mode 
               cause is different from rat change deact cause, cache the deact req to 
               deactivate RRC one more time with new cause */    
            emm_ctrl_data_ptr->rrc_deact_req.deact_reason = 
                             reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason;
            emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = TRUE;    
          }
          else
          {
            emm_init_rrc_deact_req(
                             reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason,
                             emm_ctrl_data_ptr);
          }
          break;
        case SYS_STOP_MODE_REASON_POWER_OFF:
        case SYS_STOP_MODE_REASON_OFFLINE:
        case SYS_STOP_MODE_REASON_MODE_CHANGE:
        case SYS_STOP_MODE_REASON_LPM:
        case SYS_STOP_MODE_REASON_PSM:
          /* Check if OTA power off detach is possible */
          	if(reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_LPM && 
				emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_CP_SMS_INVALID)
          	{
          		emm_ctrl_data_ptr->plmn_service_state->sim_state = SYS_SIM_STATE_AVAILABLE;
          	}	
          if(
             ((reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason != SYS_STOP_MODE_REASON_PSM)
              ||
              ((cm_is_psm_context_saved((sys_modem_as_id_e_type)mm_as_id) == FALSE) && 
               (emm_ctrl_data_ptr->psm_local_detach_enabled == FALSE))) && 
             ((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                           SYS_SRV_STATUS_SRV)
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

             ||((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                     SYS_SRV_STATUS_LIMITED) &&
             (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY))
#endif
#endif
              ) &&
             ((emm_ctrl_data_ptr->rrc_active == TRUE) || 
              (emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == TRUE))  &&
              (
#ifdef FEATURE_LTE_REL11
              (emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) ||
#endif
              (mm_timer_status[TIMER_T3346] != TIMER_ACTIVE))&&
              !(emm_connection_establishment_barred(LTE_RRC_EST_CAUSE_MO_SIGNALING,NULL,emm_ctrl_data_ptr) && 
              (emm_ctrl_data_ptr->emm_connection_state == EMM_IDLE_STATE)) 
#ifdef FEATURE_FMD_SPEED_INFO 
			&&!(reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason == SYS_STOP_MODE_REASON_LPM && 
              emm_ctrl_data_ptr->fmd_force_local_detach == TRUE)
#endif
			  )
          {			  
            /* Save the stop reason to be used later in RRC DEACT REQ */
            emm_ctrl_data_ptr->rrc_deact_req.deact_reason = 
                           reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason;

            /* Deact REQ will be sent upon completion of switch off detach
               procedure. Reset deact req pending flag if it is on. */
            if(emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == TRUE)
            {
              emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = FALSE;
              emm_ctrl_data_ptr->rrc_active = TRUE;
            }
  
            /* Start power off detach */
            emm_start_mo_detach(emm_get_mo_detach_type(emm_ctrl_data_ptr), 
                                TRUE, 
                                emm_complete_poweroff_detach,
                                emm_complete_poweroff_detach_failed,
                                emm_ctrl_data_ptr);
          }
          else
          {
            emm_local_switchoff_detach(reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason,
                                       emm_ctrl_data_ptr);
          }
          break;
        case SYS_STOP_MODE_REASON_LOCAL_DETACH:
          /* Local detach and deactivate RRC use mapped cause MODE CHANGE */
          emm_local_switchoff_detach(SYS_STOP_MODE_REASON_MODE_CHANGE,
                                     emm_ctrl_data_ptr);
          break;
        default:
            MSG_ERROR_DS_1(MM_SUB, "=EMM= Unknown stop mode reason: %d",
                           reg_cmd_ptr->cmd.mmr_stop_mode_req.stop_mode_reason);
          break;
      }
      break;
  }
  emm_send_reg_attach_failed_ind();

} /* end of emm_process_stop_mode_req() */

/*===========================================================================

FUNCTION  EMM_PROCESS_PS_DETACH_REQ

DESCRIPTION
  This function processes PS DETACH REQ from REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_ps_detach_req
(
  mmr_ps_detach_req_s_type  ps_detach_req, 
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{
  emm_state_type  emm_state = EMM_INVALID_STATE;
  emm_failure_type    emm_failure_cause;
  boolean             camp_only_mode = FALSE;

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  emm_state = EMM_GET_STATE();

  MSG_HIGH_DS_3(MM_SUB, "=EMM= PS_DETACH_REQ - RRC active %d, Srv status %d, State %d",
                emm_ctrl_data_ptr->rrc_active,
                emm_ctrl_data_ptr->plmn_service_state->service_status,
                emm_state);

  MSG_HIGH_DS_3(MM_SUB, "=EMM= PS DETACH REQ - Pending REG REQ %d, DEACT REQ/CNF %d/%d",
                emm_ctrl_data_ptr->reg_req_pended_indication,
                emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending,
                emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending);

  /* Always reset status */
  emm_ctrl_data_ptr->ps_detach_info.status = EMM_PS_DETACH_NONE;

  /* Save the REQ in EMM data base */
  emm_ctrl_data_ptr->ps_detach_info.ps_detach_req = ps_detach_req;

#ifdef FEATURE_1XSRLTE
  if((emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == 
                      SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY ) || 
     (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == 
                      SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY ))
  {
    camp_only_mode = TRUE;
  }
#endif 

#ifdef FEATURE_WWAN_GNSS_DYNAMIC
    /* NAS to inform high priority indication for Stop mode procedures 
  ** like OTA PS Detach, Deactivation, RRC connection abort to
  ** prioritize WWAN tech over GPS.*/
  if ((gmm_check_wwan_tech_active() == FALSE) &&
       (emm_state != EMM_NULL) &&
       (emm_state != EMM_DEREGISTERED))
  {
    emm_send_rrc_wwan_set_priority(TRUE);
    gmm_set_wwan_resumption_req(TRUE);
  }
#endif

  if((((emm_ctrl_data_ptr->reg_req_pended_indication == TRUE) && 
       (emm_state != EMM_DEREGISTERED) &&
       (camp_only_mode == FALSE)) ||
      (emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == TRUE) ||
      (emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == TRUE)) &&
      (emm_ctrl_data_ptr->ps_detach_info.ps_detach_req.detach_type != SYS_PS_DETACH_TYPE_LOCAL_DETACH)) 
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Caching PS DETACH REQ - REG/STOP MODE REQ pending");
    emm_ctrl_data_ptr->ps_detach_info.status = EMM_PS_DETACH_REQ_PENDING;
    return;
  }

  emm_cancel_authentication();

  switch(emm_state)
  {
    case EMM_REGISTERED_INITIATED:
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      mm_stop_timer(TIMER_T3410);
      emm_remove_release_client(emm_attach_release_indication);
      emm_clean_msg_buffer(emm_ctrl_data_ptr);
      /* Report to ESM and invalidate pdn conn req*/
      emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
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
        mm_stop_timer(TIMER_T3411);
        mm_stop_timer(TIMER_T3402);
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE || defined FEATURE_CATM_LTE_EXT_GNSS_COEX || defined FEATURE_WWAN_GNSS_DYNAMIC
        mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

        /* 5.6.1.6 Abnormal case g in UE. Abort service request and start detach
           Keep the connection and notify ESM if necessary */
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  TRUE, LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                  emm_failure_cause);
      }
      else
#ifdef FEATURE_LTE_REL13
       if((emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL13)
#ifdef FEATURE_DUAL_SIM
          ||
          (*emm_ctrl_data_ptr->dds_switch_pended_ptr == TRUE)
#endif
         )
#endif
      {
        mm_stop_timer(TIMER_T3430);
        emm_remove_release_client(emm_tau_request_release_indication);
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);
      }
      if(EMM_GET_STATE() != EMM_TRACKING_AREA_UPDATING_INITIATED)
      {
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
      }
      break;
    case EMM_DEREGISTERED_INITIATED:
      /* Set status to CNF pending and let ongoing detach proceed */
      emm_ctrl_data_ptr->ps_detach_info.status = EMM_PS_DETACH_CNF_PENDING;
      return;
    case EMM_DEREGISTERED:
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      mm_stop_timer(TIMER_T3411);
      mm_stop_timer(TIMER_T3402);
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE || defined FEATURE_CATM_LTE_EXT_GNSS_COEX || defined FEATURE_WWAN_GNSS_DYNAMIC
      mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

      /* Report to ESM and invalidate pdn conn req*/
      emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);

      if(EMM_GET_SUBSTATE() == EMM_DEGEGISTERED_WAITING_PDN_CONN_REQ)
      {
        /* For new REG Req if we are in waiting for PDN CONN
           we do not process it. so move it to PLMN SEARCH */
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH); 

        /* Notify ESM */
        emm_build_and_send_detach_ind(EMM_DETACHED);  
      }

      /* Invalid state to perform MO detach. Send ps detach cnf right away. */
      emm_send_ps_detach_cnf(emm_ctrl_data_ptr);
      return;
    default:
      /* Invalid state to perform MO detach. Send ps detach cnf right away. */
      emm_send_ps_detach_cnf(emm_ctrl_data_ptr);
      return;
  }
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
#ifndef FEATURE_NAS_DISABLE_VOICE
   if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED)
   {
      emm_csfb_failure_cause_type failure_cause_type;
      failure_cause_type.failure_type = AS_REJECT_CAUSE;
      failure_cause_type.failure_cause.lte_nas_failure_cause = AS_REJ_LRRC_CONN_EST_FAILURE;
      emm_process_extended_service_req_failure(failure_cause_type,emm_ctrl_data_ptr);
   }
   else if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED)
   {
      emm_csfb_failure_cause_type failure_cause_type;
      failure_cause_type.failure_type = ONEX_CSFB_REJECT_CAUSE;
      failure_cause_type.failure_cause.lte_nas_failure_cause = ONEX_REJ_1XCSFB_HO_FAIL_CALL_REL_NORMAL;
      emm_process_extended_service_req_failure(failure_cause_type,emm_ctrl_data_ptr);
   }
#endif
#endif
  if((emm_ctrl_data_ptr->rrc_active == TRUE) &&
     (emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                         SYS_SRV_STATUS_SRV) &&
     ((emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED) ||
      (emm_state == EMM_REGISTERED_INITIATED)) &&
      (
#ifdef FEATURE_LTE_REL11
      (emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) ||
#endif
      (mm_timer_status[TIMER_T3346] != TIMER_ACTIVE)) &&
      (emm_ctrl_data_ptr->ps_detach_info.ps_detach_req.detach_type != SYS_PS_DETACH_TYPE_LOCAL_DETACH))
  {

#ifdef FEATURE_LTE_REL13
    if(emm_pend_mo_detach(
            emm_get_mo_detach_type(emm_ctrl_data_ptr),
            emm_ctrl_data_ptr) == FALSE)
#endif
    {
      /* Start MO normal detach */
      emm_start_mo_detach(emm_get_mo_detach_type(emm_ctrl_data_ptr), 
                          FALSE, 
                          NULL, 
                          emm_detach_failed_routine, 
                          emm_ctrl_data_ptr);
    }

    emm_ctrl_data_ptr->ps_detach_info.status = EMM_PS_DETACH_CNF_PENDING;
  }
  else
  {
    /* Notify ESM */
    emm_build_and_send_detach_ind(EMM_DETACHED);

    emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_NO_SRV;
    emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;

    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);
    emm_send_ps_detach_cnf(emm_ctrl_data_ptr);
  }
  
  emm_send_reg_attach_failed_ind();

  

} /* emm_process_ps_detach_req() */


#ifdef DISABLE_9x05_OPTIMIZATIONS
/*===========================================================================

FUNCTION  EMM_PROCESS_SET_DRX_REQ

DESCRIPTION
  This function processes SET DRX REQ from REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_set_drx_req
(
  mmr_set_drx_req_s_type    set_drx_req, 
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{
  emm_substate_type emm_substate;
  emm_state_type  emm_state = EMM_INVALID_STATE;

   
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Rx DRX value (%d)", set_drx_req.drx_coefficient);
									  
#ifdef FEATURE_CIOT
  if((emm_chk_nb1_rat() == TRUE )
#ifdef FEATURE_CATM1_DISABLE
    ||(TRUE)
#endif
   )
  {
    mm_send_mmr_set_drx_cnf(FALSE);
    return;
  }
#endif

  if (  (set_drx_req.drx_coefficient != SYS_DRX_NOT_SPECIFIED_BY_MS &&
         (set_drx_req.drx_coefficient < SYS_DRX_CN_COEFFICIENT_S1_CN6_T32 ||
          set_drx_req.drx_coefficient > SYS_DRX_CN_COEFFICIENT_S1_CN9_T256)
         )
      )
   {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= Invalid DRX value (%d) recevied, Rejecting DRX_SET_REQ ",
                    set_drx_req.drx_coefficient);
      mm_send_mmr_set_drx_cnf(FALSE);
      return;
   }

   /* If no change in DRX value then no need to start RAU/TAU, send success in SET_DRX_CNF.  */
  if (emm_get_gmm_drx_cn_coff_s1() == set_drx_req.drx_coefficient)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Rx DRX is same as what the UE has, send MMR_SET_DRX_CNF ");
    mm_send_mmr_set_drx_cnf(TRUE);
    return;
  }
  
  emm_state = EMM_GET_STATE();
  emm_substate = EMM_GET_SUBSTATE();
  
  if((emm_state == EMM_REGISTERED && 
      (emm_substate == EMM_REGISTERED_NORMAL_SERVICE || 
       emm_substate == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM))
     && 
     (emm_ctrl_data_ptr->rrc_active == TRUE && 
      emm_ctrl_data_ptr->reg_req_pended_indication == FALSE))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Initing TAU- DRX change, new DRX value = %d ",
                  set_drx_req.drx_coefficient);

  emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent = NAS_SEND_DRX_ON_ALL_RATS;
  *emm_ctrl_data_ptr->set_drx_req_pending_ptr = TRUE;
     emm_set_gmm_drx_cn_coff_s1(set_drx_req.drx_coefficient);
     emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
    emm_initiate_tau_procedure(EMM_INITIAL_UPDATE, 
                               emm_get_eps_update_type(emm_ctrl_data_ptr), 
                               FALSE, 
                               emm_ctrl_data_ptr);
  }
  else if(emm_state == EMM_REGISTERED &&
          (mm_timer_status[TIMER_T3402] != TIMER_STOPPED ||
           mm_timer_status[TIMER_T3411] != TIMER_STOPPED))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= RRC not active, new DRX value = %d ",
                  set_drx_req.drx_coefficient);
    emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent = NAS_SEND_DRX_ON_ALL_RATS;
    *emm_ctrl_data_ptr->set_drx_req_pending_ptr = TRUE;
     emm_set_gmm_drx_cn_coff_s1(set_drx_req.drx_coefficient);
   }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Send DRX CNF FALSE ");
    mm_send_mmr_set_drx_cnf(FALSE);
  } 

} /* emm_process_set_drx_req() */

#endif

/*===========================================================================

FUNCTION  EMM_PROCESS_BLOCK_PLMN_REQ

DESCRIPTION
  This function processes block PLMN request  from REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_block_plmn_req
(
  mmr_block_plmn_req_s_type *block_plmn_req_ptr, 
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type        plmn;
  rex_timer_cnt_type        timer_count; 
  boolean                   is_plmn_blocked_for_eplmn_check;
  sys_block_plmn_cause_e_type cause;
  emm_failure_type          emm_failure_cause;

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));
  plmn = block_plmn_req_ptr->plmn;
  timer_count = block_plmn_req_ptr->timer_count;
  is_plmn_blocked_for_eplmn_check = block_plmn_req_ptr->is_plmn_blocked_for_eplmn_check;
  cause = block_plmn_req_ptr->cause;

  MSG_HIGH_DS_3(MM_SUB, "=EMM= MMR_BLOCK_PLMN_REQ PLMN = %x %x %x",
                plmn.identity[0], plmn.identity[1], plmn.identity[2]);
  emm_ctrl_data_ptr->is_plmn_block_req_pending = FALSE;

  /*If LTE is not active the block PLMN*/
  if(emm_ctrl_data_ptr->rrc_active == FALSE)
  {
    if(is_plmn_blocked_for_eplmn_check == TRUE)
    {
      emm_add_eplmn_to_backoff_fplmn_list(plmn, timer_count, cause);
    }
    else
    {
      emm_add_plmn_to_backoff_fplmn_list(plmn, timer_count,cause);
    }
  }
  /*If EMM is waiting for L2 ack for attach complete then pend PLMN block request*/
  else if(emm_ctrl_data_ptr->reg_complete_ack_pending != EMM_PENDING_REG_ACK_NONE)
  {
    emm_ctrl_data_ptr->pending_block_plmn_req.plmn = plmn;
    emm_ctrl_data_ptr->pending_block_plmn_req.timer_count = timer_count;
    emm_ctrl_data_ptr->pending_block_plmn_req.is_plmn_blocked_for_eplmn_check = 
                                                          is_plmn_blocked_for_eplmn_check;
    emm_ctrl_data_ptr->pending_block_plmn_req.cause = cause;
    emm_ctrl_data_ptr->is_plmn_block_req_pending = TRUE;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Waiting for registration complted ack - pending PLMN block request");
  }
  else
  {
    /*Block the PLMN 
         and declare limited service is UE is in LTE service */
    if(is_plmn_blocked_for_eplmn_check == TRUE)
    {
      emm_add_eplmn_to_backoff_fplmn_list(plmn, timer_count,cause);
    }
    else
    {
      emm_add_plmn_to_backoff_fplmn_list(plmn, timer_count,cause);
    }
    //if camped PLMN is different from blocked PLMN then no need to do below ... 
    if((sys_plmn_match(emm_ctrl_data_ptr->plmn_service_state->plmn, plmn) ||
       (sys_plmn_id_is_undefined(plmn))) &&
       (mm_is_rat_lte(emm_ctrl_data_ptr->plmn_service_state->active_rat)) &&
       (emm_ctrl_data_ptr->reg_req_pended_indication == FALSE))
    {
      switch(EMM_GET_STATE())
      {
        case EMM_DEREGISTERED:
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE);
		  mm_stop_timer(TIMER_T3411);
		  emm_ctrl_data_ptr->emm_attach_attempt_counter = 0;
		  MSG_HIGH_DS(MM_SUB,"=EMM= Stopping 3411 and resetting attach counter", 0,0,0);
          break;

        case EMM_REGISTERED:
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_LIMITED_SERVICE);
		  mm_stop_timer(TIMER_T3411);
		  emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
		  MSG_HIGH_DS(MM_SUB,"=EMM= Stopping 3411 and resetting tau counter", 0,0,0);
          break;

        case EMM_SERVICE_REQUEST_INITIATED:
        case EMM_TRACKING_AREA_UPDATING_INITIATED:
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
          if(EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED)
          {
            emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                      TRUE, LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                      emm_failure_cause);
          }
          else
          {
            lte_nas_tai_lst1_type  current_tai;
            current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->
                                                                                 camped_svc_info.selected_plmn) ;
            current_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac ;
            if(((emm_search_tai_list(&current_tai,emm_ctrl_data_ptr->emm_tai_list_ptr) == FALSE)
#ifdef FEATURE_NAS_GW
                 ||
                 (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI)
#endif
                )&&
               (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED))
            {
              emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
              emm_save_mobility_info(emm_ctrl_data_ptr);
            }
            mm_stop_timer(TIMER_T3430);
            emm_remove_release_client(emm_tau_request_release_indication);
          }
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_LIMITED_SERVICE);
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

        case EMM_REGISTERED_INITIATED:
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
          mm_stop_timer(TIMER_T3410);
          emm_remove_release_client(emm_attach_release_indication);
          emm_clean_msg_buffer(emm_ctrl_data_ptr);
          if(EMM_GET_SUBSTATE() == EMM_WAITING_FOR_NW_RESPONSE)
          {
            emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr, emm_failure_cause);
          }
          else
          {
            emm_build_and_send_detach_ind(EMM_DETACHED);
          }
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE);
          break;

        default:
          break;
      }
      if(emm_ctrl_data_ptr->plmn_service_state->service_status == SYS_SRV_STATUS_SRV)
      {
        emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_LIMITED;
      }
      reg_service_report(MMR_CAUSE_FAILURE_MAX_ATTEMPTED, 
                                     FALSE, 
                                    emm_ctrl_data_ptr);
      
      emm_rrc_abort_connection(emm_ctrl_data_ptr, 
                                   FALSE,
                                   LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
    }
  }

}
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
void emm_process_sim_busy_ind
(
  mm_cmd_type                *reg_cmd_ptr, 
  emm_ctrl_data_type         *emm_ctrl_data_ptr
)
{
  mm_is_sim_busy = reg_cmd_ptr->cmd.mmr_sim_busy_ind.is_sim_busy;
  emm_abort_ongoing_procedure(FALSE);
}
#endif


/*===========================================================================
FUNCTION  EMM_PROCESS_PSM_SET_CONFIG_PARAMS_REQ

DESCRIPTION
  This function processes PSM Set Config Params request from REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_psm_set_config_params_req
(
  mm_cmd_type         *reg_cmd_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  mmr_psm_set_config_params_req_s_type *msg_ptr;
  boolean update_result = FALSE;
  emm_state_type  emm_state = EMM_GET_STATE();
  emm_substate_type emm_substate = EMM_GET_SUBSTATE();

  msg_ptr = &reg_cmd_ptr->cmd.mmr_psm_set_cfg_params_req;

  mm_send_mmr_psm_set_config_params_rsp();

  /* Update the psm config params if changed */
  update_result = mm_update_psm_config_params(msg_ptr);

  MSG_HIGH_DS_8(MM_SUB, "=EMM= PSM set config params: update_result = %d, emm_state = %d,\
  emm_substate = %d, reg_req_pended = %d, psm_set_cfg_req_status = %d,\
  reg_complete_ack = %d, emm_conn_state = %d, t3440_stopped = %d",
  update_result, emm_state, emm_substate, emm_ctrl_data_ptr->reg_req_pended_indication,
  *emm_ctrl_data_ptr->psm_set_cfg_status_ptr, emm_ctrl_data_ptr->reg_complete_ack_pending,
  emm_ctrl_data_ptr->emm_connection_state, (mm_timer_status[TIMER_T3440] == TIMER_STOPPED));

  /* Trigger TAU if possible */
  if((update_result == TRUE) &&
     (*emm_ctrl_data_ptr->psm_set_cfg_status_ptr == NAS_PSM_CFG_REQ_NONE) &&
     (emm_state == EMM_REGISTERED) &&
     ((emm_substate == EMM_REGISTERED_NORMAL_SERVICE)
       ||
      (emm_substate == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM)) &&
     (mm_timer_status[TIMER_T3440] == TIMER_STOPPED) &&
     (emm_ctrl_data_ptr->reg_complete_ack_pending == EMM_PENDING_REG_ACK_NONE) &&
     (emm_ctrl_data_ptr->emm_connection_state != EMM_RELEASING_RRC_CONNECTION_STATE) &&
     (emm_ctrl_data_ptr->reg_req_pended_indication == FALSE) 
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

     && (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
#endif
     )
  {
    *emm_ctrl_data_ptr->psm_set_cfg_status_ptr = NAS_PSM_CFG_REQ_PENDING;
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
    emm_initiate_tau_procedure(EMM_INITIAL_UPDATE, 
                               emm_get_eps_update_type(emm_ctrl_data_ptr), 
                               FALSE, 
                               emm_ctrl_data_ptr);
  } /* Else remember to trigger TAU later */
  else if((update_result == TRUE) &&
          (emm_state != EMM_DEREGISTERED) &&
          (emm_state != EMM_DEREGISTERED_INITIATED))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Pended PSM_set_cfg_req");
    *emm_ctrl_data_ptr->psm_set_cfg_status_ptr = NAS_PSM_CFG_REQ_PENDING;
  }
}


/*===========================================================================
FUNCTION  EMM_PROCESS_PSM_GET_TIMER_INFO_REQ

DESCRIPTION
  This function processes PSM Get Timer Info request from REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_psm_get_timer_info_req
(
  mm_cmd_type         *reg_cmd_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{

  boolean user_config = reg_cmd_ptr->cmd.mmr_psm_get_timer_info_req.user_config;

  MSG_HIGH_DS_1(MM_SUB, "=EMM= PSM timer info user_config = %d", user_config);

#if defined(FEATURE_LTE) && defined(FEATURE_NAS_GW)
  if ((is_lte_mode() == TRUE) &&
      (user_config == FALSE) &&
      (emm_ctrl_data_ptr->t3324_value != NAS_INVALID_PSM_TIMER_VALUE) &&
      (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI)
     )
  {
     mm_send_mmr_psm_get_timer_info_rsp(emm_ctrl_data_ptr->t3324_value,
                                        mm_psm_gprs_ready_timer,
                                        emm_ctrl_data_ptr->t3412_value,
                                        mm_psm_periodic_rau_update_timer);
  }
  else if ((user_config == FALSE) &&
           (gmm_psm_timer_ctxt.t3324_value != NAS_INVALID_PSM_TIMER_VALUE) &&
           (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI))
  {
     mm_send_mmr_psm_get_timer_info_rsp(gmm_psm_timer_ctxt.t3324_value,
                                        glif_ready_timer_value,
                                        mm_psm_periodic_tau_update_timer,
                                        gmm_t3312_timer_value);
  }
  else
  {
    /* Return values from NV/EFS cache */
    mm_send_mmr_psm_get_timer_info_rsp(mm_psm_active_timer,
                                       mm_psm_gprs_ready_timer,
                                       mm_psm_periodic_tau_update_timer,
                                       mm_psm_periodic_rau_update_timer);
  }
#elif defined(FEATURE_LTE)
  if ((emm_ctrl_data_ptr->t3324_value != NAS_INVALID_PSM_TIMER_VALUE) &&
      (user_config == FALSE))
  {
    /* Return values from NW */
    mm_send_mmr_psm_get_timer_info_rsp(emm_ctrl_data_ptr->t3324_value,
                                       NAS_INVALID_PSM_TIMER_VALUE,
                                       emm_ctrl_data_ptr->t3412_value,
                                       NAS_INVALID_PSM_TIMER_VALUE);
  }  
  else
  {
    /* Return values from NV/EFS cache */
    mm_send_mmr_psm_get_timer_info_rsp(mm_psm_active_timer,
                                       NAS_INVALID_PSM_TIMER_VALUE,
                                       mm_psm_periodic_tau_update_timer,
                                       NAS_INVALID_PSM_TIMER_VALUE);
  }
#elif defined(FEATURE_NAS_GW)
  if ((gmm_psm_timer_ctxt.t3324_value != NAS_INVALID_PSM_TIMER_VALUE) &&
      (user_config == FALSE))
  {
    /* Return values from NW */
    mm_send_mmr_psm_get_timer_info_rsp(gmm_psm_timer_ctxt.t3324_value,
                                       glif_ready_timer_value,
                                       NAS_INVALID_PSM_TIMER_VALUE,
                                       gmm_t3312_timer_value);
  }  
  else
  {
    /* Return values from NV/EFS cache */
    mm_send_mmr_psm_get_timer_info_rsp(mm_psm_active_timer,
                                       mm_psm_gprs_ready_timer,
                                       NAS_INVALID_PSM_TIMER_VALUE,
                                       mm_psm_periodic_rau_update_timer);
  }
#endif
}


/*===========================================================================
FUNCTION  EMM_PROCESS_ENTER_PSM_MODE_REQ

DESCRIPTION
  This function processes Enter PSM Mode request from REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_enter_psm_mode_req
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  /* Save ESM context only if EMM state is Registered Normal sevice */
  if((emm_ctrl_data_ptr->emm_state == EMM_REGISTERED) &&
     (emm_ctrl_data_ptr->emm_substate == EMM_REGISTERED_NORMAL_SERVICE))
  {
    emm_send_esm_save_ctxt_psm_req();
  }
  else
  {
    mm_send_mmr_enter_psm_mode_rsp(FALSE);
  }
} /* end emm_process_enter_psm_mode_req() */

/*===========================================================================
FUNCTION  EMM_PROCESS_PSM_SAVE_MMGSDI_EF_REQ

DESCRIPTION: This function processes MMR_PSM_SAVE_MMGSDI_EF_REQ message for writing MMGSDI 
 . EF files before entering PSM mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_psm_save_mmgsdi_ef_req
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
   emm_security_context_info  *context_ptr = NULL;
   emm_security_context_info  *current_context_ptr = NULL;

  /* Save ESM context only if EMM state is Registered Normal sevice */
  if((emm_ctrl_data_ptr->emm_state == EMM_REGISTERED) &&
     (emm_ctrl_data_ptr->emm_substate == EMM_REGISTERED_NORMAL_SERVICE))
  {
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
    mm_send_mmr_psm_save_mmgsdi_ef_rsp(TRUE);
  }
  else
  {
    mm_send_mmr_psm_save_mmgsdi_ef_rsp(FALSE);
  }
} /* end emm_process_enter_psm_mode_req() */


/*===========================================================================
FUNCTION  EMM_PROCESS_SET_EDRX_CFG_REQ

DESCRIPTION
  This function processes Set eDRX Config request from REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_set_edrx_cfg_req
(
  mm_cmd_type         *reg_cmd_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  mmr_set_edrx_cfg_req_s_type *msg_ptr;
  boolean update_result = FALSE;
  emm_state_type  emm_state = EMM_GET_STATE();
  emm_substate_type emm_substate = EMM_GET_SUBSTATE();

  msg_ptr = &reg_cmd_ptr->cmd.mmr_set_edrx_cfg_req;

  mm_send_mmr_set_edrx_cfg_rsp();

  MSG_HIGH_DS_8(MM_SUB, "=MM= Set eDRX config req: change_mask = %d,\
  rat = %d, edrx_enabled = %d, edrx_ptw = %d, edrx_cycle_length = %d,\
  lte_edrx_enabled = %d, lte_ptw = %d, lte_cycle_length = %d",
  msg_ptr->change_mask, msg_ptr->rat_type, msg_ptr->edrx_enabled, 
  msg_ptr->edrx_ptw, msg_ptr->edrx_cycle_length, mm_edrx_cfg.lte.enabled,
  mm_edrx_cfg.lte.ptw, mm_edrx_cfg.lte.cycle_length);

  /* Update the edrx config params if changed */
  update_result = mm_update_edrx_cfg_params(msg_ptr);

  if (mm_is_rat_lte(msg_ptr->rat_type))
  {
    MSG_HIGH_DS_8(MM_SUB, "=EMM= update_result = %d, emm_state = %d,\
    emm_substate = %d, reg_req_pended = %d, edrx_set_cfg_req_status = %d,\
    reg_complete_ack = %d, emm_conn_state = %d, t3440_stopped = %d",
    update_result, emm_state, emm_substate, emm_ctrl_data_ptr->reg_req_pended_indication,
    emm_ctrl_data_ptr->edrx_set_cfg_req_status, emm_ctrl_data_ptr->reg_complete_ack_pending,
    emm_ctrl_data_ptr->emm_connection_state, (mm_timer_status[TIMER_T3440] == TIMER_STOPPED));

    /* Trigger TAU if possible */
    if((update_result == TRUE) &&
       (emm_ctrl_data_ptr->edrx_set_cfg_req_status == NAS_EDRX_CFG_REQ_NONE) &&
       (emm_state == EMM_REGISTERED) &&
       ((emm_substate == EMM_REGISTERED_NORMAL_SERVICE)
        ||
        (emm_substate == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM)) &&
       (mm_timer_status[TIMER_T3440] == TIMER_STOPPED) &&
       (emm_ctrl_data_ptr->reg_complete_ack_pending == EMM_PENDING_REG_ACK_NONE) &&
       (emm_ctrl_data_ptr->emm_connection_state != EMM_RELEASING_RRC_CONNECTION_STATE) &&
       (emm_ctrl_data_ptr->reg_req_pended_indication == FALSE) &&
       (msg_ptr->rat_type == emm_ctrl_data_ptr->plmn_service_state->active_rat)
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

       && (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
#endif
       )
    {
      emm_ctrl_data_ptr->edrx_set_cfg_req_status = NAS_EDRX_CFG_REQ_PENDING;
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      emm_initiate_tau_procedure(EMM_INITIAL_UPDATE, 
                                 emm_get_eps_update_type(emm_ctrl_data_ptr), 
                                 FALSE, 
                                 emm_ctrl_data_ptr);
    } /* Else remember to trigger TAU later */
    else if((update_result == TRUE) &&
            (emm_state != EMM_DEREGISTERED) &&
            (emm_state != EMM_DEREGISTERED_INITIATED))
    {
#ifdef FEATURE_CIOT   
      if ((emm_ctrl_data_ptr->attached_lte_rat == SYS_RAT_NONE) ||
          (emm_ctrl_data_ptr->attached_lte_rat == msg_ptr->rat_type))
#endif
      {
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

        MSG_HIGH_DS_1(MM_SUB, "=EMM= Pended eDRX_set_cfg_req, emc_srv_status %d",
                                                   emm_ctrl_data_ptr->emc_srv_status);
#endif
#else
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Pended eDRX_set_cfg_req");
#endif
        emm_ctrl_data_ptr->edrx_set_cfg_req_status = NAS_EDRX_CFG_REQ_PENDING;
      }
    }
  }
#ifdef FEATURE_NAS_GW
  else if ((update_result == TRUE) &&
           (msg_ptr->rat_type == SYS_RAT_GSM_RADIO_ACCESS))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Pended GSM eDRX_set_cfg_req");
    emm_set_mm_edrx_cfg_req_status(NAS_EDRX_CFG_REQ_PENDING);
  }
#endif
}


/*===========================================================================
FUNCTION  EMM_PROCESS_GET_EDRX_PARAMS_REQ

DESCRIPTION
  This function processes Get eDRX Params request from REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_get_edrx_params_req
(
  mm_cmd_type         *reg_cmd_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  switch(reg_cmd_ptr->cmd.mmr_get_edrx_params_req.rat_type)
  {
    case SYS_RAT_LTE_RADIO_ACCESS:
      if(emm_ctrl_data_ptr->edrx_params.enabled == TRUE)
      {
        /* Return values from NW */
        mm_send_mmr_get_edrx_params_rsp(SYS_RAT_LTE_RADIO_ACCESS,
                                        mm_edrx_cfg.lte.enabled,
                                        emm_ctrl_data_ptr->edrx_params.ptw,
                                        emm_ctrl_data_ptr->edrx_params.cycle_length);
      }
      else
      {
        /* Return LTE eDRX params values from NV/EFS cache */
        mm_send_mmr_get_edrx_params_rsp(SYS_RAT_LTE_RADIO_ACCESS,
                                        mm_edrx_cfg.lte.enabled,
                                        mm_edrx_cfg.lte.ptw,
                                        mm_edrx_cfg.lte.cycle_length);
      }
      break;
    case SYS_RAT_LTE_M1_RADIO_ACCESS:
      if(emm_ctrl_data_ptr->edrx_params.enabled == TRUE)
      {
        /* Return values from NW */
        mm_send_mmr_get_edrx_params_rsp(SYS_RAT_LTE_M1_RADIO_ACCESS,
                                        mm_edrx_cfg.M1.enabled,
                                        emm_ctrl_data_ptr->edrx_params.ptw,
                                        emm_ctrl_data_ptr->edrx_params.cycle_length);
      }
      else
      {
        /* Return LTE eDRX params values from NV/EFS cache */
        mm_send_mmr_get_edrx_params_rsp(SYS_RAT_LTE_M1_RADIO_ACCESS,
                                        mm_edrx_cfg.M1.enabled,
                                        mm_edrx_cfg.M1.ptw,
                                        mm_edrx_cfg.M1.cycle_length);
      }
      break;
    case SYS_RAT_LTE_NB1_RADIO_ACCESS:
      if(emm_ctrl_data_ptr->edrx_params.enabled == TRUE)
      {
        /* Return values from NW */
        mm_send_mmr_get_edrx_params_rsp(SYS_RAT_LTE_NB1_RADIO_ACCESS,
                                        mm_edrx_cfg.NB1.enabled,
                                        emm_ctrl_data_ptr->edrx_params.ptw,
                                        emm_ctrl_data_ptr->edrx_params.cycle_length);
      }
      else
      {
        /* Return LTE eDRX params values from NV/EFS cache */
        mm_send_mmr_get_edrx_params_rsp(SYS_RAT_LTE_NB1_RADIO_ACCESS,
                                        mm_edrx_cfg.NB1.enabled,
                                        mm_edrx_cfg.NB1.ptw,
                                        mm_edrx_cfg.NB1.cycle_length);
      }
      break;
    case SYS_RAT_GSM_RADIO_ACCESS:
      /* Return GSM eDRX params values from NV/EFS cache */
      mm_send_mmr_get_edrx_params_rsp(SYS_RAT_GSM_RADIO_ACCESS,
                                      mm_edrx_cfg.gsm.enabled,
                                      mm_edrx_cfg.gsm.ptw,
                                      mm_edrx_cfg.gsm.cycle_length);
      break;
    default:
      /* Return UMTS eDRX params values from NV/EFS cache */
      mm_send_mmr_get_edrx_params_rsp(SYS_RAT_UMTS_RADIO_ACCESS,
                                      mm_edrx_cfg.umts.enabled,
                                      mm_edrx_cfg.umts.ptw,
                                      mm_edrx_cfg.umts.cycle_length);
      break;
  }
}
/*===========================================================================
FUNCTION  emm_process_send_plmn_home_roam

DESCRIPTION: This function processes MMR_SEND_PLMN_HOME_ROAM message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_send_plmn_home_roam
(
  mm_cmd_type         *reg_cmd_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
    emm_ctrl_data_ptr->home_mcc = reg_cmd_ptr->cmd.mmr_send_plmn_home_roam_ind.home;
    emm_send_rrc_system_update(emm_ctrl_data_ptr);
}
/*===========================================================================

FUNCTION EMM_PROCESS_REG_COMMAND

DESCRIPTION
  This function processes primitives received from REG 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_reg_command
( 
  mm_cmd_type        *reg_msg_ptr, 
  emm_ctrl_data_type *emm_ctrl_data_ptr 
)
{
  sys_psm_status_e_type status;

  switch (reg_msg_ptr->cmd.hdr.message_id)
  {
    case MMR_PLMN_SEARCH_REQ:
      emm_process_plmn_search_req(reg_msg_ptr, emm_ctrl_data_ptr) ;
      break ;
    case MMR_REG_REQ:
      emm_process_reg_req(reg_msg_ptr, emm_ctrl_data_ptr) ;
      break ;
    case MMR_STOP_MODE_REQ:
      emm_process_stop_mode_req(reg_msg_ptr, emm_ctrl_data_ptr) ;
      break ;
    case MMR_PLMN_SEARCH_ABORT_REQ:
      emm_process_abort_plmnsearch(emm_ctrl_data_ptr) ;
      break ;
    case MMR_PS_DETACH_REQ:
      mm_start_timer(TIMER_EMM_PS_DETACH, DEFAULT_TIMEOUT); 
      if((emm_ctrl_data_ptr->emm_connection_state == EMM_CONNECTED_STATE) &&
         ((emm_ctrl_data_ptr->last_rrc_service_ind_ptr->no_svc_cause == 
                                                       SYS_NO_SVC_CAUSE_RLF) ||
         (emm_ctrl_data_ptr->last_rrc_service_ind_ptr->t311_rem_time > 0)))
      {
        emm_ctrl_data_ptr->ps_detach_info.status = EMM_PS_DETACH_REQ_PENDING;
        emm_rrc_abort_connection(emm_ctrl_data_ptr, 
                                 FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
      }
      else
      {
        emm_process_ps_detach_req(reg_msg_ptr->cmd.mmr_ps_detach_req, 
                                emm_ctrl_data_ptr);
      }
      break;
    case MMR_PH_STATUS_CHANGE_REQ:
      emm_process_ph_status_change_req(reg_msg_ptr, emm_ctrl_data_ptr);
      break;
	  
#ifdef DISABLE_9x05_OPTIMIZATIONS
    case MMR_SET_DRX_REQ:
      emm_process_set_drx_req(reg_msg_ptr->cmd.mmr_set_drx_req,emm_ctrl_data_ptr );
      break;
#endif
    case MMR_BLOCK_PLMN_REQ:
      emm_process_block_plmn_req(&(reg_msg_ptr->cmd.mmr_block_plmn_req), emm_ctrl_data_ptr);
      break;
    case MMR_CLEAR_LAI_REJECT_LIST_REQ:
      emm_clear_manual_ftai_list();
      break;
	case MMR_NET_SEL_MODE_CHANGE_IND:
	  emm_send_rrc_nw_sel_mode_reset_req(reg_msg_ptr->cmd.mmr_net_sel_mode_change_ind.network_selection_mode, NULL, emm_ctrl_data_ptr);
	  break;
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
    case MMR_SIM_BUSY_IND:
	  emm_process_sim_busy_ind(reg_msg_ptr, emm_ctrl_data_ptr);
	  break;
#endif

    case MMR_PSM_READY_REQ:
      status = emm_check_ready_for_psm(emm_ctrl_data_ptr);

      if(status != SYS_PSM_STATUS_NONE)
      {
        mm_send_mmr_psm_ready_rsp(status);
      }
      break;

    case MMR_PSM_SET_CONFIG_PARAMS_REQ:
      emm_process_psm_set_config_params_req(reg_msg_ptr, emm_ctrl_data_ptr);
      break;

    case MMR_PSM_GET_TIMER_INFO_REQ:
      emm_process_psm_get_timer_info_req(reg_msg_ptr, emm_ctrl_data_ptr);
      break;

    case MMR_ENTER_PSM_MODE_REQ:
      emm_process_enter_psm_mode_req(emm_ctrl_data_ptr);
      break;

    case MMR_SET_EDRX_CFG_REQ:
      emm_process_set_edrx_cfg_req(reg_msg_ptr, emm_ctrl_data_ptr);
      break;

    case MMR_GET_EDRX_PARAMS_REQ:
      emm_process_get_edrx_params_req(reg_msg_ptr, emm_ctrl_data_ptr);
      break;
    case MMR_SEND_PLMN_HOME_ROAM:
      emm_process_send_plmn_home_roam(reg_msg_ptr, emm_ctrl_data_ptr);
      break;

    case MMR_PSM_SAVE_MMGSDI_EF_REQ:
      emm_process_psm_save_mmgsdi_ef_req(emm_ctrl_data_ptr);
      break;
    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Unknown or illigal REG command: %d",
                    reg_msg_ptr->cmd.hdr.message_id);
      break ;
  } /* switch (reg_msg_ptr->cmd.hdr.message_id) */

} /* end of emm_process_reg_command */


/*===========================================================================

FUNCTION EMM_SEND_EMPTY_PLMNSEARCH_CNF

DESCRIPTION
  This function sends MMR_PLMN_SEARCH_CNF with status of incomplete

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_empty_plmnsearch_cnf
(
  mmr_plmn_search_req_s_type  *plmn_search_req_ptr
)
{
  mm_send_empty_mmr_plmn_search_cnf(plmn_search_req_ptr,
                                    REG_MM_PLMN_SEARCH_ABORTED);

} /* end of emm_send_empty_plmnsearch_cnf() */

/*===========================================================================

FUNCTION EMM_SEND_MMR_PLMN_SEARCH_CNF

DESCRIPTION
  This function builds MMR_PLMN_SEARCH_CNF with data from LTE RRC PLMN SEARCH
  CNF and sends it to REG 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmr_plmn_search_cnf
(
  lte_rrc_plmn_search_cnf_s  *rrc_plmn_search_cnf_ptr
)
{
  mmr_plmn_search_cnf_s_type     *mmr_plmn_search_cnf_ptr = NULL;
  uint32                                    index = 0 ;
  log_lte_nas_emm_outgoing_msg_event_type   event;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data(); 
  
  mmr_plmn_search_cnf_ptr =(mmr_plmn_search_cnf_s_type*) emm_modem_mem_alloc(
                         sizeof(mmr_plmn_search_cnf_s_type),
                         emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void *)mmr_plmn_search_cnf_ptr) ;
  /* Init */
  memset((void *)mmr_plmn_search_cnf_ptr,0x0,sizeof(mmr_plmn_search_cnf_s_type));

  mmr_plmn_search_cnf_ptr->message_header.message_set = MS_MM_REG;
  mmr_plmn_search_cnf_ptr->message_header.message_id  = (int)MMR_PLMN_SEARCH_CNF;

#if defined (FEATURE_WTOG_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_WTOW_BACKGROUND_PLMN_SEARCH) \
    || defined (FEATURE_RAT_PRIORITY_LIST)
  mmr_plmn_search_cnf_ptr->network_selection_mode = 
                                  rrc_plmn_search_cnf_ptr->network_select_mode;
#endif

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
	if(rrc_plmn_search_cnf_ptr->search_type == SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH)
	{
	  ref_cnt_obj_add_ref(rrc_plmn_search_cnf_ptr->found_plmn_rat_list.gps_wwan_scan_result);
    
      if(rrc_plmn_search_cnf_ptr->found_plmn_rat_list.gps_wwan_scan_result != NULL)
      {
		MSG_HIGH_DS_1(MM_SUB, "=EMM= Number of elements = %d", rrc_plmn_search_cnf_ptr->found_plmn_rat_list.gps_wwan_scan_result->wwan_scan_result_per_rat.num_scan_result_per_rat);
        mmr_plmn_search_cnf_ptr->found_plmn_list.gps_wwan_scan_result = rrc_plmn_search_cnf_ptr->found_plmn_rat_list.gps_wwan_scan_result;
		memscpy(mmr_plmn_search_cnf_ptr->found_plmn_list.gps_wwan_scan_result,sizeof(sys_loc_wwan_scan_result_s_type),
		  rrc_plmn_search_cnf_ptr->found_plmn_rat_list.gps_wwan_scan_result,sizeof(sys_loc_wwan_scan_result_s_type));
	    mmr_plmn_search_cnf_ptr->plmn_search_result = REG_MM_PLMN_SEARCH_SUCCESS;
		
      }
	  else
	  {
        MSG_FATAL_DS(MM_SUB,"=EMM= RRC_GPS_WWAN_SCAN_RESULT is NULL", rrc_plmn_search_cnf_ptr->found_plmn_rat_list.gps_wwan_scan_result, 0,0);
	  }
	}
	else 
#endif
  if(rrc_plmn_search_cnf_ptr->status == LTE_RRC_PLMN_SEARCH_COMPLETED)
  {
    mmr_plmn_search_cnf_ptr->plmn_search_result = REG_MM_PLMN_SEARCH_SUCCESS;
	
    mmr_plmn_search_cnf_ptr->found_plmn_list = 
                           rrc_plmn_search_cnf_ptr->found_plmn_rat_list;
	}
  else
  {
    if(rrc_plmn_search_cnf_ptr->status == LTE_RRC_PLMN_SEARCH_ABORTED_ON_RLF)
    {
      mmr_plmn_search_cnf_ptr->plmn_search_result = REG_MM_PLMN_SEARCH_ABORTED_ON_RLF;
    }
    else if(rrc_plmn_search_cnf_ptr->status == LTE_RRC_PLMN_SEARCH_TIMED_OUT)
    {
      mmr_plmn_search_cnf_ptr->plmn_search_result =  REG_MM_PLMN_SEARCH_TIME_OUT;
    }
    else if( rrc_plmn_search_cnf_ptr->status == LTE_RRC_PLMN_SEARCH_PARTIAL)   
    {
      mmr_plmn_search_cnf_ptr->plmn_search_result = REG_MM_PLMN_SEARCH_PARTIAL;
    }
    else
    {
      mmr_plmn_search_cnf_ptr->plmn_search_result = REG_MM_PLMN_SEARCH_ABORTED;
    }
    if(mm_emm_is_qrd_device == FALSE && mm_partial_search_allowed_get() == FALSE)
    {
     memset(&(mmr_plmn_search_cnf_ptr->found_plmn_list), 0, sizeof(mmr_plmn_search_cnf_ptr->found_plmn_list));
    }
    else
    {
      mmr_plmn_search_cnf_ptr->found_plmn_list = 
                           rrc_plmn_search_cnf_ptr->found_plmn_rat_list;
    }
  } 
  mmr_plmn_search_cnf_ptr->transaction_id = (byte)rrc_plmn_search_cnf_ptr->trans_id;
  mmr_plmn_search_cnf_ptr->service_search = rrc_plmn_search_cnf_ptr->search_type;

  if (mm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_2(mm_get_ulog_handle(), "=EMM= RRC search type = %d, MMR plmn search type = %d ",rrc_plmn_search_cnf_ptr->search_type, mmr_plmn_search_cnf_ptr->service_search);
  }
  
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
   if(mmr_plmn_search_cnf_ptr->service_search == SYS_SERVICE_SEARCH_CSG && 
       mmr_plmn_search_cnf_ptr->plmn_search_result == REG_MM_PLMN_SEARCH_SUCCESS)
   {
     mm_process_csg_search_list(mm_csg_search_type,
                                     &mmr_plmn_search_cnf_ptr->found_plmn_list.plmn_list);
   }
#endif 

  /*lint -save -e778 */
  PUT_IMH_LEN(sizeof(mmr_plmn_search_cnf_s_type) - sizeof(IMH_T),
              &mmr_plmn_search_cnf_ptr->message_header);
  /*lint -restore */

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending MMR_PLMN_SEARCH_CNF");
  mm_send_message((IMH_T*)mmr_plmn_search_cnf_ptr, GS_QUEUE_REG);

  event.emm_out_msg = (byte)MMR_PLMN_SEARCH_CNF;
  event_report_payload(EVENT_LTE_REG_OUTGOING_MSG,sizeof(event),(void*)&event);     
/*provide an event to report all available plmn list found*/
if((mmr_plmn_search_cnf_ptr->service_search != SYS_SERVICE_SEARCH_CELL_SEARCH)
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH	
      && (mmr_plmn_search_cnf_ptr->service_search != SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH)
#endif
  )
{
#ifdef FEATURE_DUAL_SIM
  nasdiag_plmn_list_event(&(mmr_plmn_search_cnf_ptr->found_plmn_list.plmn_list),(sys_modem_as_id_e_type) mm_sub_id);
#else
  nasdiag_plmn_list_event(&(mmr_plmn_search_cnf_ptr->found_plmn_list.plmn_list), 0);
#endif
}
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH	
if(rrc_plmn_search_cnf_ptr->search_type == SYS_SERVICE_SEARCH_GPS_ADVANCE_SEARCH)
{
  ref_cnt_obj_release(rrc_plmn_search_cnf_ptr->found_plmn_rat_list.gps_wwan_scan_result);
}
#endif
  emm_modem_mem_free(mmr_plmn_search_cnf_ptr, emm_ctrl_data_ptr);  
} /* end of emm_rrc_plmn_list_confirmation */

#ifdef FEATURE_LTE_REL9
/*===========================================================================

FUNCTION EMM_SEND_MMR_LTE_CELL_INFO_IND

DESCRIPTION
  This function builds EMM_SEND_MMR_LTE_CELL_INFO_IND with data 
  from LTE RRC PLMN SERVICE IND and sends it to REG 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmr_lte_cell_info_ind
(
  boolean emergency_access_barred
)
{
  mmr_lte_cell_info_ind_s_type mmr_lte_cell_info_ind;

  /* Init */
  memset((void *)&mmr_lte_cell_info_ind,0x0,sizeof(mmr_lte_cell_info_ind_s_type));

  mmr_lte_cell_info_ind.message_header.message_set = MS_MM_REG;
  mmr_lte_cell_info_ind.message_header.message_id  = (int)MMR_LTE_CELL_INFO_IND;
  mmr_lte_cell_info_ind.lte_cell_info.emergency_access_barred = emergency_access_barred;

  PUT_IMH_LEN(sizeof(mmr_lte_cell_info_ind_s_type) - sizeof(IMH_T),
              &mmr_lte_cell_info_ind.message_header);

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sending MMR_LTE_CELL_INFO_IND emergency_access_barred = %d",
                emergency_access_barred);
  mm_send_message(&mmr_lte_cell_info_ind.message_header, GS_QUEUE_REG);
} /* end of emm_send_mmr_lte_cell_info_ind */

#endif


/*===========================================================================

FUNCTION EMM_SET_PLMN_SRV_CAP

DESCRIPTION
  This function set plmn_service_capability field

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_set_plmn_srv_cap
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  

  if(emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                         SYS_SRV_STATUS_NO_SRV)
  {
    emm_ctrl_data_ptr->plmn_service_state->plmn_service_capability = 
                                              SYS_SRV_CAPABILITY_NOT_AVAILABLE;
  }
  else
  {
    if(emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                        SYS_SRV_DOMAIN_PS_ONLY)
    {
      /* If UE is registered for PS service, set plmn srv cap to PS only */
      emm_ctrl_data_ptr->plmn_service_state->plmn_service_capability = 
                                                        SYS_SRV_CAPABILITY_PS_SERVICE_ONLY;
    }
    else
    {
      /* Otherwise always set plmn srv cap to CS and PS */
      emm_ctrl_data_ptr->plmn_service_state->plmn_service_capability = 
                                                          SYS_SRV_CAPABILITY_CS_PS_SERVICE;
    }
  }
}/* emm_set_plmn_srv_cap */

/*===========================================================================

FUNCTION EMM_SEND_REG_IND

DESCRIPTION
  This function sends a MMR_SERVICE_IND message to the REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_reg_ind
(
  mmr_cause_e_type     mmr_cause,
  boolean              only_for_reg,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  mmr_service_ind_s_type                    mmr_service_ind ;
  log_lte_nas_emm_outgoing_msg_event_type   event;
  unsigned int i;
  boolean  plmn_id_is_undefined;
  boolean  mnc_includes_pcs_digit;
  uint32   mcc;
  uint32   mnc;
  lte_nas_tai_lst1_type current_tai;

  /* Init */
  memset((void *)&mmr_service_ind, 0x0, sizeof(mmr_service_ind_s_type));

  current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
  current_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;

  /* Configure MMR_REG_CNF primitive */
  mmr_service_ind.message_header.message_set = MS_MM_REG ;
  mmr_service_ind.message_header.message_id  = (int)MMR_SERVICE_IND ;

  /* Init MMR cause */
  mmr_service_ind.cause = mmr_cause ;
  /*if UE max attempt failed during PLMN search and TAI change we end up here, don't reset the flag */
  if((mm_get_plmn_search_in_progress() == FALSE)||(emm_ctrl_data_ptr->last_reg_cause != MMR_CAUSE_FAILURE_MAX_ATTEMPTED) )
  {
    emm_ctrl_data_ptr->last_reg_cause = mmr_cause;
  }

  emm_set_plmn_srv_cap(emm_ctrl_data_ptr);

  /* Copy serving PLMN state info */
  mmr_service_ind.service_state = *(emm_ctrl_data_ptr->plmn_service_state);

  mmr_service_ind.service_state.no_service_cause = 
                         emm_ctrl_data_ptr->last_rrc_service_ind_ptr->no_svc_cause;

  mmr_service_ind.service_state.suitable_search_time = (dword)emm_ctrl_data_ptr->last_rrc_service_ind_ptr->t311_rem_time;
  

  if(emm_search_tai_list(&current_tai, emm_ctrl_data_ptr->emm_tai_list_ptr) == FALSE)
  {
    mmr_service_ind.service_state.lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
  }

#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

  if((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                     SYS_SRV_STATUS_LIMITED ||
      emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                              SYS_SRV_STATUS_LIMITED_REGIONAL)
     &&
     (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY))
  {
    mmr_service_ind.service_state.eps_nw_feature_support.emc_bs_supported = 
      emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
                                                 ims_emergency_support_enabled;
  }

  if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
  {
    mmr_service_ind.service_state.emc_attached = TRUE;
  }
#endif
#endif

  /* Report Equivalent PLMN List if list is avaliable */
  mmr_service_ind.update_equivalent_plmn_list = emm_ctrl_data_ptr->update_eplmn_list;
  if(mmr_service_ind.update_equivalent_plmn_list == TRUE)
  {
    if(emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents >
         MAX_NO_OF_PLMNs)
    {
      MSG_ERROR_DS_2(MM_SUB, "=EMM= Truncating EPLMN list length from %d to %d",
                     emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents,
                     MAX_NO_OF_PLMNs);
      emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents
        = MAX_NO_OF_PLMNs;
    }

    if(emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents >
         SYS_PLMN_LIST_MAX_LENGTH)
    {
      mmr_service_ind.equivalent_plmn_list.length = SYS_PLMN_LIST_MAX_LENGTH;
    }
    else
    {
      mmr_service_ind.equivalent_plmn_list.length = 
       emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents;
    }

    if(mmr_service_ind.equivalent_plmn_list.length > 0)
    {
      memscpy((void *)&mmr_service_ind.equivalent_plmn_list.plmn[0],
              sizeof(mmr_service_ind.equivalent_plmn_list.plmn),
             (void *)&emm_ctrl_data_ptr->emm_equivalent_PLMN_list.plmn[0],
             mmr_service_ind.equivalent_plmn_list.length * sizeof(sys_plmn_id_s_type));
    }
  }
#ifdef FEATURE_NAS_GW
#ifdef FEATURE_SGLTE
  if(!MM_IS_IN_SGLTE_MODE || (mm_as_id == mm_cs_stack_id))
#endif
  {
	mm_send_rr_status_ind(mmr_service_ind.service_state.service_status, mmr_service_ind.service_state.reg_service_domain);
  }
#endif
  /* EPLMN list updated is necessary. Reset the flag. */
  emm_ctrl_data_ptr->update_eplmn_list = FALSE;

#ifdef FEATURE_CIOT
if(mm_serving_plmn.info.active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
{
   mmr_service_ind.oos_srch_done = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->oos_search_done; //FRACQ
}
#ifndef FEATURE_CATM1_DISABLE
else
{
   mmr_service_ind.oos_srch_done = TRUE;
}
#endif
#endif
  mmr_service_ind.only_for_reg = only_for_reg;
  mmr_service_ind.cell_id= 0XFFFF;


#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  mmr_service_ind.service_state.csg_in_white_list = 
                    (mm_is_csg_in_allowed_list(mmr_service_ind.service_state.plmn, mmr_service_ind.service_state.csg_info.csg_id) ||
                    mm_is_csg_in_operator_list(mmr_service_ind.service_state.plmn, mmr_service_ind.service_state.csg_info.csg_id, FALSE));
#endif 
#ifdef FEATURE_CIOT
  if(emm_bit_mask_check(emm_ctrl_data_ptr->nas_ciot_capability_config,EMM_SMS_ONLY) == FALSE ||
      (mm_checkif_rat_pri_list_has_only_rat(SYS_RAT_LTE_NB1_RADIO_ACCESS) == FALSE) )
  {
    mmr_service_ind.service_state.cp_sms_service_status = SYS_CP_SMS_STATUS_NOT_AVAILABLE;
  }
  else if(emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status == SYS_CP_SMS_STATUS_SERVICE_FAIL_DUE_TO_NW_FAILURE ||
  	 emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status == SYS_CP_SMS_STATUS_SERVICE_FAIL_DUE_TO_CONGESTION) 
  {
    mmr_service_ind.service_state.cp_sms_service_status = SYS_CP_SMS_STATUS_SERVICE_TEMP_FAILURE;
  }
  else
  {
    mmr_service_ind.service_state.cp_sms_service_status = emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status;
  }
#endif
  /*lint -save -e778 */
  PUT_IMH_LEN( sizeof(mmr_service_ind_s_type) - sizeof(IMH_T),&mmr_service_ind.message_header );
  /*lint -restore */

  MSG_HIGH_DS_0(MM_SUB, "=EMM= sent MMR_SERVICE_IND");
  MSG_HIGH_DS_8(MM_SUB, "=EMM= cause = %d sim_state = %d svc_capability = %d plmn_forbidden = %d svc_status = %d reg_svc_domain = %d active_rat = %d ps_data_suspend = %d",
                mmr_service_ind.cause,
                mmr_service_ind.service_state.sim_state,
                mmr_service_ind.service_state.plmn_service_capability,
                mmr_service_ind.service_state.plmn_forbidden,
                mmr_service_ind.service_state.service_status,
                mmr_service_ind.service_state.reg_service_domain,
                mmr_service_ind.service_state.active_rat,
                mmr_service_ind.service_state.ps_data_suspend);

  MSG_HIGH_DS_8(MM_SUB, "=EMM= sib8_available = %d equiv PLMN update=%d list len=%d info_blocked_at_reg = %d cs_svc_status = %d lte_cs_capability = %d extend_srv_info = %d plmn_reg_type = %d",
                mmr_service_ind.service_state.sib8_available,
                mmr_service_ind.update_equivalent_plmn_list,
                mmr_service_ind.equivalent_plmn_list.length,
                mmr_service_ind.only_for_reg,
                mmr_service_ind.service_state.cs_service_status,
                mmr_service_ind.service_state.lte_cs_capability,
                mmr_service_ind.service_state.extend_srv_info,
                mmr_service_ind.service_state.plmn_reg_type);
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
  MSG_HIGH_DS_2(MM_SUB, "=EMM=  emc_srv_spt = %d emc_attached = %d",
                mmr_service_ind.service_state.eps_nw_feature_support.emc_bs_supported,
                mmr_service_ind.service_state.emc_attached);
#endif
#ifdef FEATURE_CIOT
       MSG_HIGH_DS_1(MM_SUB, "=EMM= sent MMR_SERVICE_IND oos done =%d",  mmr_service_ind.oos_srch_done); //FRACQ
	MSG_HIGH_DS_8(MM_SUB, "=EMM=  cp_sms_service_status = %d cp_ciot_supported = %d up_ciot_supported = %d s1_u_data_supported = %d er_without_pdn_supported = %d hc_cp_ciot_supported = %d epco_supported %d CE_Res %d",
					mmr_service_ind.service_state.cp_sms_service_status,
					emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.cp_ciot_supported,
					emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.up_ciot_supported,
					emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.s1_u_data_supported,
					emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.er_without_pdn_supported,
					emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.hc_cp_ciot_supported,
					emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.epco_supported,
					emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.ce_restriction);
#endif
 
  for (i = 0; i< mmr_service_ind.equivalent_plmn_list.length; i++)
  {
    sys_plmn_get_mcc_mnc(mmr_service_ind.equivalent_plmn_list.plmn[i],
                         &plmn_id_is_undefined,
                         &mnc_includes_pcs_digit,
                         &mcc,
                         &mnc);
    MSG_MED_DS_3(MM_SUB, "=EMM= Equiv PLMN(%d) %d-%d", i, mcc, mnc);
  }
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  MSG_HIGH_DS_2(MM_SUB, "=EMM= csg_id = %d csg_in_white_list = %d",
                mmr_service_ind.service_state.csg_info.csg_id,
                mmr_service_ind.service_state.csg_in_white_list);
#endif 
  mm_send_message( (IMH_T*)&mmr_service_ind, GS_QUEUE_REG );
  emm_update_rrc_rplmn_info();
  event.emm_out_msg = (byte)MMR_SERVICE_IND;
  event_report_payload(EVENT_LTE_REG_OUTGOING_MSG,sizeof(event), (void*)&event);

  emm_ctrl_data_ptr->plmn_service_state->extend_srv_info = 
                                                      SYS_EXTEND_SRV_INFO_NONE;

  emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type   = 
                                                 SYS_REG_DOMAIN_NOT_APPLICABLE;

} /* end of emm_send_reg_ind() */

/*===========================================================================

FUNCTION EMM_SEND_REG_CNF

DESCRIPTION
  This function sends a MMR_REG_CNF message to the REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_reg_cnf
(
  mmr_cause_e_type     mmr_cause,
  boolean              only_for_reg,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  mmr_reg_cnf_s_type     *mmr_reg_cnf_ptr = NULL;   
  log_lte_nas_emm_outgoing_msg_event_type   event;
  byte                                      index = 0;
  unsigned int                         i;
  boolean  plmn_id_is_undefined;
  boolean  mnc_includes_pcs_digit;
  uint32   mcc;
  uint32   mnc;
  lte_nas_tai_lst1_type current_tai;

   
  mmr_reg_cnf_ptr =(mmr_reg_cnf_s_type*) emm_modem_mem_alloc(
                         sizeof(mmr_reg_cnf_s_type),
                         emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void *)mmr_reg_cnf_ptr) ;
  
  /* Init */
  memset((void *)mmr_reg_cnf_ptr, 0x0, sizeof(mmr_reg_cnf_s_type));

  current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
  current_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;

  /* 
  ** Re-init reg_service_report to emm_send_reg_ind routine 
  ** in order to start sending MMR_SERVICE_IND upon change of
  ** TAI, TAU procedure completion or any sorts of NW rejection
  */
  reg_service_report = (emm_reg_service_report_type)emm_send_reg_ind ;

  /* Configure MMR_REG_CNF primitive */
  mmr_reg_cnf_ptr->message_header.message_set = MS_MM_REG ;
  mmr_reg_cnf_ptr->message_header.message_id  = (int)MMR_REG_CNF ;

  /* Init MMR cause */
  mmr_reg_cnf_ptr->cause = mmr_cause ;
  emm_ctrl_data_ptr->last_reg_cause = mmr_cause;

  emm_set_plmn_srv_cap(emm_ctrl_data_ptr);

  /* Copy serving PLMN state info */
  mmr_reg_cnf_ptr->service_state = *(emm_ctrl_data_ptr->plmn_service_state);

  mmr_reg_cnf_ptr->service_state.no_service_cause = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->no_svc_cause;

  mmr_reg_cnf_ptr->service_state.suitable_search_time = (dword)emm_ctrl_data_ptr->last_rrc_service_ind_ptr->t311_rem_time;

  if(emm_search_tai_list(&current_tai, emm_ctrl_data_ptr->emm_tai_list_ptr) == FALSE)
  {
    mmr_reg_cnf_ptr->service_state.lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
  }


#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

  if((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                     SYS_SRV_STATUS_LIMITED ||
      emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                              SYS_SRV_STATUS_LIMITED_REGIONAL)
     &&
     (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY))
  {
    mmr_reg_cnf_ptr->service_state.eps_nw_feature_support.emc_bs_supported = 
      emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
                                                 ims_emergency_support_enabled;
  }

  if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
  {
    mmr_reg_cnf_ptr->service_state.emc_attached = TRUE;
  }
#endif
#endif

  /* Available PLMN List*/   
  mmr_reg_cnf_ptr->available_plmn_list.length = 
                          emm_ctrl_data_ptr->last_rrc_service_ind_ptr->found_plmn_list.num_found_plmns; 

  for(index=0;index<mmr_reg_cnf_ptr->available_plmn_list.length;index++)
  {
    /*The ID of the PLMN*/
    mmr_reg_cnf_ptr->available_plmn_list.info[index].plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                    emm_ctrl_data_ptr->last_rrc_service_ind_ptr->found_plmn_list.found_plmn[index].plmn);

    /*The radio access technology of the PLMN */
    mmr_reg_cnf_ptr->available_plmn_list.info[index].rat = mm_serving_plmn.info.active_rat;

    /*Indicates the type of service domain the PLMN can provide*/
    mmr_reg_cnf_ptr->available_plmn_list.info[index].plmn_service_capability = 
               emm_ctrl_data_ptr->plmn_service_state->plmn_service_capability;
    /*Indicates if the PLMN is forbidden*/
    mmr_reg_cnf_ptr->available_plmn_list.info[index].plmn_forbidden = FALSE ;

    /*Indicates the type of PLMN*/
    if((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_AVAILABLE) ||
       (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                     SYS_SIM_STATE_CS_INVALID))
    {
      if (PLMN_MATCH(emm_ctrl_data_ptr->emm_home_plmn.identity,mmr_reg_cnf_ptr->available_plmn_list.info[index].plmn.identity))
      {
        mmr_reg_cnf_ptr->available_plmn_list.info[index].list_category = SYS_DETAILED_PLMN_LIST_CATEGORY_HPLMN ;
      }
      else
      {
        mmr_reg_cnf_ptr->available_plmn_list.info[index].list_category = SYS_DETAILED_PLMN_LIST_CATEGORY_OTHER ;
      }
    }
    else 
    {
      mmr_reg_cnf_ptr->available_plmn_list.info[index].list_category = SYS_DETAILED_PLMN_LIST_CATEGORY_OTHER ;
    }          

    /*The signal quality of the PLMN*/
    mmr_reg_cnf_ptr->available_plmn_list.info[index].signal_quality = 
                       emm_ctrl_data_ptr->last_rrc_service_ind_ptr->found_plmn_list.found_plmn[index].quality;

    /*The signal strength of the PLMN*/
    mmr_reg_cnf_ptr->available_plmn_list.info[index].signal_strength = 
                           emm_ctrl_data_ptr->last_rrc_service_ind_ptr->found_plmn_list.found_plmn[index].rsrp;
  }

  /* Update Equivalent PLMN List if necessary */
  mmr_reg_cnf_ptr->update_equivalent_plmn_list = emm_ctrl_data_ptr->update_eplmn_list;
  if (mmr_reg_cnf_ptr->update_equivalent_plmn_list == TRUE)
  {
    if(emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents >
         MAX_NO_OF_PLMNs)
    {
      MSG_ERROR_DS_2(MM_SUB, "=EMM= Truncating EPLMN list length from %d to %d",
                     emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents,
                     MAX_NO_OF_PLMNs);
      emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents
        = MAX_NO_OF_PLMNs;
    }

    if(emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents >
         SYS_PLMN_LIST_MAX_LENGTH)
    {
      mmr_reg_cnf_ptr->equivalent_plmn_list.length = SYS_PLMN_LIST_MAX_LENGTH;
    }
    else
    {
      mmr_reg_cnf_ptr->equivalent_plmn_list.length = 
       emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents;
    }

    if(mmr_reg_cnf_ptr->equivalent_plmn_list.length > 0)
    {
      memscpy((void*)&mmr_reg_cnf_ptr->equivalent_plmn_list.plmn[0],
              sizeof(mmr_reg_cnf_ptr->equivalent_plmn_list.plmn),
              (void*)&emm_ctrl_data_ptr->emm_equivalent_PLMN_list.plmn[0],
              mmr_reg_cnf_ptr->equivalent_plmn_list.length * sizeof(sys_plmn_id_s_type));
    }
  }
#ifdef FEATURE_NAS_GW
#ifdef FEATURE_SGLTE
  if(!MM_IS_IN_SGLTE_MODE || (mm_as_id == mm_cs_stack_id))
#endif
  {
	mm_send_rr_status_ind(mmr_reg_cnf_ptr->service_state.service_status, mmr_reg_cnf_ptr->service_state.reg_service_domain);
  }
#endif
  /* EPLMN list updated is necessary. Reset the flag. */
  emm_ctrl_data_ptr->update_eplmn_list = FALSE;

  mmr_reg_cnf_ptr->ue_oos = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->use_oos;
#ifdef FEATURE_CIOT
  if(mm_serving_plmn.info.active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
  {
    mmr_reg_cnf_ptr->oos_srch_done = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->oos_search_done; //FRACQ
  }
 #ifndef FEATURE_CATM1_DISABLE
  else
  {
    mmr_reg_cnf_ptr->oos_srch_done = TRUE;
  }
  #endif
  #endif
  mmr_reg_cnf_ptr->only_for_reg =  only_for_reg;
  mmr_reg_cnf_ptr->cell_id= 0XFFFF;
#ifdef FEATURE_CIOT
    if(emm_bit_mask_check(emm_ctrl_data_ptr->nas_ciot_capability_config,EMM_SMS_ONLY) == FALSE ||
       (mm_checkif_rat_pri_list_has_only_rat(SYS_RAT_LTE_NB1_RADIO_ACCESS) == FALSE))
    {
      mmr_reg_cnf_ptr->service_state.cp_sms_service_status = SYS_CP_SMS_STATUS_NOT_AVAILABLE;
    }
    else if(emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status == SYS_CP_SMS_STATUS_SERVICE_FAIL_DUE_TO_NW_FAILURE ||
	   emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status == SYS_CP_SMS_STATUS_SERVICE_FAIL_DUE_TO_CONGESTION) 
    {
	  mmr_reg_cnf_ptr->service_state.cp_sms_service_status = SYS_CP_SMS_STATUS_SERVICE_TEMP_FAILURE;
    }
    else
    {
	  mmr_reg_cnf_ptr->service_state.cp_sms_service_status = emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status;
    }
#endif


#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  mmr_reg_cnf_ptr->service_state.csg_in_white_list = 
                    (mm_is_csg_in_allowed_list(mmr_reg_cnf_ptr->service_state.plmn, mmr_reg_cnf_ptr->service_state.csg_info.csg_id) ||
                    mm_is_csg_in_operator_list(mmr_reg_cnf_ptr->service_state.plmn, mmr_reg_cnf_ptr->service_state.csg_info.csg_id, FALSE));
#endif 
  /*lint -save -e778 */
  PUT_IMH_LEN( sizeof( mmr_reg_cnf_s_type) - sizeof(IMH_T),&mmr_reg_cnf_ptr->message_header );
  /*lint -restore */

  MSG_HIGH_DS_0(MM_SUB, "=EMM= sent MMR_REG_CNF");
  MSG_HIGH_DS_8(MM_SUB, "=EMM= cause = %d sim_state = %d svc_capability = %d plmn_forbidden = %d svc_status = %d reg_svc_domain = %d active_rat = %d ps_data_suspend = %d",
                mmr_reg_cnf_ptr->cause,
                mmr_reg_cnf_ptr->service_state.sim_state,
                mmr_reg_cnf_ptr->service_state.plmn_service_capability,
                mmr_reg_cnf_ptr->service_state.plmn_forbidden,
                mmr_reg_cnf_ptr->service_state.service_status,
                mmr_reg_cnf_ptr->service_state.reg_service_domain,
                mmr_reg_cnf_ptr->service_state.active_rat,
                mmr_reg_cnf_ptr->service_state.ps_data_suspend);
  MSG_HIGH_DS_8(MM_SUB, "=EMM= sib8_available = %d equiv PLMN update=%d list len = %d info_blocked_at_reg = %d cs_svc_status = %d lte_cs_capability = %d extend_srv_info = %d plmn_reg_type = %d ",
                mmr_reg_cnf_ptr->service_state.sib8_available,
                mmr_reg_cnf_ptr->update_equivalent_plmn_list,
                mmr_reg_cnf_ptr->equivalent_plmn_list.length,
                mmr_reg_cnf_ptr->only_for_reg,
                mmr_reg_cnf_ptr->service_state.cs_service_status,
                mmr_reg_cnf_ptr->service_state.lte_cs_capability,
                mmr_reg_cnf_ptr->service_state.extend_srv_info,
                mmr_reg_cnf_ptr->service_state.plmn_reg_type);

#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
  MSG_HIGH_DS_2(MM_SUB, "=EMM= emc_srv_spt = %d emc_attached = %d",
                mmr_reg_cnf_ptr->service_state.eps_nw_feature_support.emc_bs_supported,
                mmr_reg_cnf_ptr->service_state.emc_attached);
#endif
MSG_HIGH_DS_1(MM_SUB, "=EMM= sent MMR_REG_CNF OOS search done =%d",mmr_reg_cnf_ptr->oos_srch_done); //FRACQ
#ifdef FEATURE_CIOT
  MSG_HIGH_DS_8(MM_SUB, "=EMM=  cp_sms_service_status = %d cp_ciot_supported = %d up_ciot_supported = %d s1_u_data_supported = %d er_without_pdn_supported = %d hc_cp_ciot_supported = %d epco_supported %d CE res %d",
				  mmr_reg_cnf_ptr->service_state.cp_sms_service_status,
                  emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.cp_ciot_supported,
                  emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.up_ciot_supported,
	              emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.s1_u_data_supported,
	              emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.er_without_pdn_supported,
	              emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.hc_cp_ciot_supported,
	              emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.epco_supported,
	              emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.ce_restriction);
#endif


  for (i = 0; i< mmr_reg_cnf_ptr->equivalent_plmn_list.length; i++)
  {
    sys_plmn_get_mcc_mnc(mmr_reg_cnf_ptr->equivalent_plmn_list.plmn[i],
                         &plmn_id_is_undefined,
                         &mnc_includes_pcs_digit,
                         &mcc,
                         &mnc);
    MSG_MED_DS_3(MM_SUB, "=EMM= Equiv PLMN(%d) %d-%d", i, mcc, mnc);
  }
#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
  MSG_HIGH_DS_2(MM_SUB, "=EMM= csg_id = %d csg_in_white_list = %d",
                mmr_reg_cnf_ptr->service_state.csg_info.csg_id,
                mmr_reg_cnf_ptr->service_state.csg_in_white_list);
#endif 

  mm_send_message( (IMH_T*)mmr_reg_cnf_ptr, GS_QUEUE_REG );
  emm_update_rrc_rplmn_info();
  
#ifdef FEATURE_FMD_SPEED_INFO
  if((emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)&&
  (emm_bit_mask_check(emm_ctrl_data_ptr->reg_req_pending_ptr->additional_info,(byte)SYS_ADDTNL_INFO_PSEUDO_ONLINE_MODE) == TRUE)
     && (emm_ctrl_data_ptr->fmd_psm_fallback_timer !=0))
  {  
      mm_stop_timer(TIMER_FMD_PSM_FALLBACK);
	  mm_start_timer(TIMER_FMD_PSM_FALLBACK, (emm_ctrl_data_ptr->fmd_psm_fallback_timer)*1000);
	  MSG_HIGH_DS_1(REG_SUB, "=MM= Start fmd PSM fallback timer %d", emm_ctrl_data_ptr->fmd_psm_fallback_timer);
  }
#endif

  event.emm_out_msg = (byte)MMR_REG_CNF;
  event_report_payload(EVENT_LTE_REG_OUTGOING_MSG,sizeof(event), (void*)&event);

  emm_ctrl_data_ptr->plmn_service_state->extend_srv_info = 
                                                      SYS_EXTEND_SRV_INFO_NONE;

  emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type   = 
                                                 SYS_REG_DOMAIN_NOT_APPLICABLE;

  emm_ctrl_data_ptr->last_rrc_service_ind_ptr->found_plmn_list.num_found_plmns = 0;
  emm_modem_mem_free(mmr_reg_cnf_ptr, emm_ctrl_data_ptr);
} /* end of emm_send_reg_cnf() */

void emm_reject_service_req
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
   

  emm_send_reg_cnf(MMR_CAUSE_FAILURE_REJECT_REMAIN_ON_PLMN,
                   FALSE, 
                   emm_ctrl_data_ptr) ;

  emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;
  emm_ctrl_data_ptr->manual_user_sel = FALSE;

} /* end of emm_reject_service_req() */

void emm_send_reg_camped_ind
(  
  lte_rrc_camped_svc_info_s    camped_svc_info
)
{
  mmr_camped_ind_s_type                     camped_ind;
  log_lte_nas_emm_outgoing_msg_event_type   event;

  camped_ind.message_header.message_set = MS_MM_REG;
  camped_ind.message_header.message_id  = (int)MMR_CAMPED_IND;

  camped_ind.active_rat = mm_serving_plmn.info.active_rat;


  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&camped_ind.active_band.lte_active_band);
  
  SYS_LTE_BAND_MASK_ADD_BAND(&camped_ind.active_band.lte_active_band,camped_svc_info.selected_band); 

  camped_ind.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(camped_svc_info.selected_plmn);

  camped_ind.lac_id = (sys_lac_type)camped_svc_info.tac;

  PUT_IMH_LEN( sizeof( mmr_camped_ind_s_type ) - sizeof(IMH_T), &camped_ind.message_header );

  MSG_HIGH_DS_2(MM_SUB, "MM sent MMR_CAMPED_IND RAT: %d, BAND %u",
                camped_ind.active_rat,
                camped_svc_info.selected_band);

  mm_send_message( (IMH_T*)&camped_ind, GS_QUEUE_REG );
  event.emm_out_msg = (byte)MMR_CAMPED_IND;
  event_report_payload(EVENT_LTE_REG_OUTGOING_MSG,sizeof(event), (void*)&event);
} /* end of emm_send_reg_camped_ind() */


/*===========================================================================

FUNCTION  emm_process_ph_status_change_req

DESCRIPTION
  This function processes PH_STATUS_CHANGE_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_ph_status_change_req
(
  mm_cmd_type              *reg_cmd_ptr, 
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{
   MSG_HIGH_DS_0(MM_SUB, "=EMM= Rcved MMR_PH_STATUS_CHANGE_REQ");

   /* Restore T3402 value to 12 minutes default value
    & notify T3402 clients of the T3402 value*/
   emm_ctrl_data_ptr->t3402_value = EMM_DEFAULT_T3402; 
   emm_ctrl_data_ptr->t3402_attach_rej_param.is_t3402_recv_attach_rej = FALSE;
   emm_send_t3402_changed_ind(emm_ctrl_data_ptr->t3402_value);
   if(mm_psm_mode_req_received == FALSE)
   {
     (void)emm_write_rplmn_type_to_efs(emm_ctrl_data_ptr->rplmn_info.rplmn_type);
   }
#ifdef FEATURE_CIOT
   (void)emm_write_dcn_list_to_efs(emm_ctrl_data_ptr);
#endif
   if(mm_last_active_rat_stopped != SYS_RAT_NONE)
   {
     emm_ctrl_data_ptr->emm_ph_status_change_cnf_pending = TRUE;
  
     emm_send_rrc_deact_req(reg_cmd_ptr->cmd.mmr_ph_status_change_req.stop_mode_reason, emm_ctrl_data_ptr);
   }
   else
   {
     mm_send_mmr_ph_status_change_cnf();
   }
}

#if defined(FEATURE_FEMTO_CSG) && defined (FEATURE_NAS_GW)
/*===========================================================================

FUNCTION  emm_send_reg_home_nodeb_name_ind

DESCRIPTION
  This function sends  MMR_HOME_NODEB_NAME_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_reg_home_nodeb_name_ind
(
  lte_rrc_csg_info_update_ind_s  *rrc_csg_info_update
)
{
  mmr_home_nodeb_name_ind_s_type mmr_hnb_ind;
 
  mmr_hnb_ind.message_header.message_set = MS_MM_REG;
  mmr_hnb_ind.message_header.message_id  = (int)MMR_HOME_NODEB_NAME_IND;

  mmr_hnb_ind.csg_info = rrc_csg_info_update->csg_info;
  mmr_hnb_ind.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(rrc_csg_info_update->selected_plmn);

  PUT_IMH_LEN( sizeof( mmr_home_nodeb_name_ind_s_type ) - sizeof(IMH_T), &mmr_hnb_ind.message_header );

  MSG_HIGH_DS_0(MM_SUB, "=EMM= sent MMR_HOME_NODEB_NAME_IND");

  mm_send_message( (IMH_T*)&mmr_hnb_ind, GS_QUEUE_REG );
  
} /* end of emm_send_reg_camped_ind() */
#endif 


#ifdef FEATURE_NAS_GW
/*===========================================================================
FUNCTION  EMM_SEND_MMR_RRC_SIB6_INFO_IND

DESCRIPTION
  This function sends SIB6 INFO to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_mmr_rrc_sib6_info_ind
(
  lte_rrc_sib6_info_ind_s  *rrc_sib6_info_ind_ptr
)
{
  mmr_rrc_sib6_info_ind_s_type mmr_rrc_sib6_info_ind;
 
  mmr_rrc_sib6_info_ind.message_header.message_set = MS_MM_REG;
  mmr_rrc_sib6_info_ind.message_header.message_id  = (int)MMR_RRC_SIB6_INFO_IND;

  mmr_rrc_sib6_info_ind.sib6_present = rrc_sib6_info_ind_ptr->sib6_present;
  mmr_rrc_sib6_info_ind.priority_UTRAN_FDD = rrc_sib6_info_ind_ptr->priority_UTRAN_FDD;
  mmr_rrc_sib6_info_ind.priority_UTRAN_TDD = rrc_sib6_info_ind_ptr->priority_UTRAN_TDD;

  PUT_IMH_LEN( sizeof( mmr_rrc_sib6_info_ind_s_type ) - sizeof(IMH_T), &mmr_rrc_sib6_info_ind.message_header );

  MSG_HIGH_DS_0(MM_SUB, "=EMM= sent MMR_RRC_SIB6_INFO_IND to REG");

  mm_send_message( (IMH_T*)&mmr_rrc_sib6_info_ind, GS_QUEUE_REG );
}

/*===========================================================================
FUNCTION  EMM_SEND_MMR_RRC_SIB7_INFO_IND

DESCRIPTION
  This function sends SIB7 INFO to REG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_mmr_rrc_sib7_info_ind
(
  lte_rrc_sib7_info_ind_s  *rrc_sib7_info_ind_ptr
)
{
  mmr_rrc_sib7_info_ind_s_type mmr_rrc_sib7_info_ind;
 
  mmr_rrc_sib7_info_ind.message_header.message_set = MS_MM_REG;
  mmr_rrc_sib7_info_ind.message_header.message_id  = (int)MMR_RRC_SIB7_INFO_IND;

  mmr_rrc_sib7_info_ind.sib7_present = rrc_sib7_info_ind_ptr->sib7_present;
  mmr_rrc_sib7_info_ind.priority = rrc_sib7_info_ind_ptr->priority;

  PUT_IMH_LEN( sizeof( mmr_rrc_sib7_info_ind_s_type ) - sizeof(IMH_T), &mmr_rrc_sib7_info_ind.message_header );

  MSG_HIGH_DS_0(MM_SUB, "=EMM= sent MMR_RRC_SIB7_INFO_IND to REG");

  mm_send_message( (IMH_T*)&mmr_rrc_sib7_info_ind, GS_QUEUE_REG );
}
#endif

/*==============================================================================

FUNCTION NAME

  emm_send_mmr_sim_state_revalidate_ind

==============================================================================*/
void emm_send_mmr_sim_state_revalidate_ind
(
 const sys_sim_state_e_type sim_state
)
{
  mmr_sim_state_revalidate_ind_s_type msg;
  memset(&msg,0x00,sizeof(mmr_sim_state_revalidate_ind_s_type));
  
  msg.message_header.message_set = MS_MM_REG;
  msg.message_header.message_id  = (byte)MMR_SIM_STATE_REVALIDATE_IND;

  msg.sim_state = sim_state;
  
  PUT_IMH_LEN( sizeof( mmr_sim_state_revalidate_ind_s_type ) - sizeof(IMH_T),
                            &msg.message_header );

  MSG_HIGH_DS_1(MM_SUB, "=EMM= MMR_SIM_STATE_REVALIDATE_IND sim_state %d", sim_state);
  mm_send_message( (IMH_T *)&msg, GS_QUEUE_REG ); 
}
#endif /*FEATURE_LTE*/
